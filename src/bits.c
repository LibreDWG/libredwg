/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2009 Free Software Foundation, Inc.                        */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * bits.c: low level read and write functions
 * written by Felipe Castro
 * modified by Felipe Corrêa da Silva Sances
 * modified by Rodrigo Rodrigues da Silva
 * modified by James Mike Dupont
 */


#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "bits.h"
#include "logging.h"

/*------------------------------------------------------------------------------
 * Private functions prototypes
 */
unsigned int
bit_ckr8(unsigned int dx, unsigned char *adr, long n);

/*------------------------------------------------------------------------------
 * Public functions
 */

/* Advance bits (forward or backward)
 */
void
bit_advance_position(Bit_Chain * dat, int advance)
{
  int endpos;

  endpos = dat->bit + advance;
  if (dat->byte >= dat->size - 1 && endpos > 7)
    {
      dat->bit = 7;
      return;
    }
  dat->bit = endpos % 8;
  dat->byte += endpos / 8;
}

/** Read 1 bit.
 */
BITCODE_B
bit_read_B(Bit_Chain * dat)
{
  unsigned char result;
  unsigned char byte;

  byte = dat->chain[dat->byte];
  result = (byte & (0x80 >> dat->bit)) >> (7 - dat->bit);

  bit_advance_position(dat, 1);
  return result;
}

/** Write 1 bit.
 */
void
bit_write_B(Bit_Chain * dat, unsigned char value)
{
  if (dat->byte >= dat->size - 1)
    bit_chain_alloc(dat);

  if (value)
    dat->chain[dat->byte] |= 0x80 >> dat->bit;
  else
    dat->chain[dat->byte] &= ~(0x80 >> dat->bit);

  bit_advance_position(dat, 1);
}

/** Read 2 bits.
 */
BITCODE_BB
bit_read_BB(Bit_Chain * dat)
{
  unsigned char result;
  unsigned char byte;

  byte = dat->chain[dat->byte];
  if (dat->bit < 7)
    result = (byte & (0xc0 >> dat->bit)) >> (6 - dat->bit);
  else
    {
      result = (byte & 0x01) << 1;
      if (dat->byte < dat->size - 1)
        {
          byte = dat->chain[dat->byte + 1];
          result |= (byte & 0x80) >> 7;
        }
    }

  bit_advance_position(dat, 2);
  return result;
}

/** Write 2 bits.
 */
void
bit_write_BB(Bit_Chain * dat, unsigned char value)
{
  unsigned char mask;
  unsigned char byte;

  if (dat->byte >= dat->size - 1)
    bit_chain_alloc(dat);

  byte = dat->chain[dat->byte];
  if (dat->bit < 7)
    {
      mask = 0xc0 >> dat->bit;
      dat->chain[dat->byte] = (byte & ~mask) | (value << (6 - dat->bit));
    }
  else
    {
      dat->chain[dat->byte] = (byte & 0xfe) | (value >> 1);
      if (dat->byte < dat->size - 1)
        {
          byte = dat->chain[dat->byte + 1];
          dat->chain[dat->byte + 1] = (byte & 0x7f) | ((value & 0x01) << 7);
        }
    }

  bit_advance_position(dat, 2);
}

/** Read 1 nibble.
 */
BITCODE_4BITS
bit_read_4BITS(Bit_Chain * dat)
{
  BITCODE_4BITS result = bit_read_RC(dat);
  bit_advance_position(dat, -4);
  //perhaps we have an issue here when the 4bit field is near the end of a bitstream?
  // (since we initially advance 8bits and then later rewind 4bits)
  return (result & 0xf0) >> 4;
}

/** Write 1 nibble.
 */
void
bit_write_4BITS(Bit_Chain * dat, unsigned char value)
{
  unsigned char byte;
  unsigned char remainder1, remainder2;

  bit_advance_position(dat, 4);
  byte = dat->chain[dat->byte];
  remainder1 = byte & (0xff << (8 - dat->bit));
  remainder2 = byte & (0xff >> (dat->bit+4));

//TODO: implement me.

}

/** Read 1 byte (raw char).
 */
BITCODE_RC
bit_read_RC(Bit_Chain * dat)
{
  unsigned char result;
  unsigned char byte;

  byte = dat->chain[dat->byte];
  if (dat->bit == 0)
    result = byte;
  else
    {
      result = byte << dat->bit;
      if (dat->byte < dat->size - 1)
        {
          byte = dat->chain[dat->byte + 1];
          result |= byte >> (8 - dat->bit);
        }
    }

  bit_advance_position(dat, 8);
  return ((unsigned char) result);
}

/** Write 1 byte (raw char).
 */
void
bit_write_RC(Bit_Chain * dat, unsigned char value)
{
  unsigned char byte;
  unsigned char remainder;

  if (dat->byte >= dat->size - 1)
    bit_chain_alloc(dat);

  if (dat->bit == 0)
    {
      dat->chain[dat->byte] = value;
    }
  else
    {
      byte = dat->chain[dat->byte];
      remainder = byte & (0xff << (8 - dat->bit));
      dat->chain[dat->byte] = remainder | (value >> dat->bit);
      if (dat->byte < dat->size - 1)
        {
          byte = dat->chain[dat->byte + 1];
          remainder = byte & (0xff >> dat->bit);
          dat->chain[dat->byte + 1] = remainder | (value << (8 - dat->bit));
        }
    }

  bit_advance_position(dat, 8);
}

/** Read 1 word (raw short).
 */
BITCODE_RS
bit_read_RS(Bit_Chain * dat)
{
  unsigned char byte1, byte2;

  //least significant byte first:
  byte1 = bit_read_RC(dat);
  byte2 = bit_read_RC(dat);
  return ((unsigned int) ((byte2 << 8) | byte1));
}

/** Write 1 word (raw short).
 */
void
bit_write_RS(Bit_Chain * dat, unsigned int value)
{
  //least significant byte first:
  bit_write_RC(dat, value & 0xFF);
  bit_write_RC(dat, value >> 8);
}

/** Read 1 raw long (2 words).
 */
long unsigned int
bit_read_RL(Bit_Chain * dat)
{
  unsigned int word1, word2;

  //least significant word first:
  word1 = bit_read_RS(dat);
  word2 = bit_read_RS(dat);
  return ((((long unsigned int) word2) << 16) | ((long unsigned int) word1));
}

/** Write 1 raw long (2 words).
 */
void
bit_write_RL(Bit_Chain * dat, long unsigned int value)
{
  //least significant word first:
  bit_write_RS(dat, value & 0xFFFF);
  bit_write_RS(dat, value >> 16);
}

/** Read 1 raw double (8 bytes).
 */
BITCODE_RD
bit_read_RD(Bit_Chain * dat)
{
  int i;
  unsigned char byte[8];
  double *result;

  //TODO: I think it might not work on big-endian platforms:
  for (i = 0; i < 8; i++)
    byte[i] = bit_read_RC(dat);

  result = (double *) byte;
  return (*result);
}

/** Write 1 raw double (8 bytes).
 */
void
bit_write_RD(Bit_Chain * dat, double value)
{
  int i;
  unsigned char *val;

  //TODO: I think it might not work on big-endian platforms:
  val = (unsigned char *) &value;

  for (i = 0; i < 8; i++)
    bit_write_RC(dat, val[i]);
}

/** Read 1 bitshort (compacted data).
 */
BITCODE_BS
bit_read_BS(Bit_Chain * dat)
{
  unsigned char two_bit_code;
  unsigned int result;

  two_bit_code = bit_read_BB(dat);

  if (two_bit_code == 0)
    {
      result = bit_read_RS(dat);
      return (result);
    }
  else if (two_bit_code == 1)
    {
      result = (unsigned char)bit_read_RC(dat);
      return (result);
    }
  else if (two_bit_code == 2)
    return (0);
  else
    /* if (two_bit_code == 3) */
    return (256);
}

/** Write 1 bitshort (compacted data).
 */
void
bit_write_BS(Bit_Chain * dat, unsigned int value)
{

  if (value > 256)
    {
      bit_write_BB(dat, 0);
      bit_write_RS(dat, value);
    }
  else if (value == 0)
    bit_write_BB(dat, 2);
  else if (value == 256)
    bit_write_BB(dat, 3);
  else
    {
      bit_write_BB(dat, 1);
      bit_write_RC(dat, value);
    }
}

/** Read 1 bitlong (compacted data).
 */
BITCODE_BL
bit_read_BL(Bit_Chain * dat)
{
  unsigned char two_bit_code;
  long unsigned int result;

  two_bit_code = bit_read_BB(dat);

  if (two_bit_code == 0)
    {
      result = bit_read_RL(dat);
      return (result);
    }
  else if (two_bit_code == 1)
    {
      result = bit_read_RC(dat) & 0xFF;
      return (result);
    }
  else if (two_bit_code == 2)
    return (0);
  else /* if (two_bit_code == 3) */
    {
      LOG_ERROR("bit_read_BL: unexpected 2-bit code: '11'")
      return (256);
    }
}

/** Write 1 bitlong (compacted data).
 */
void
bit_write_BL(Bit_Chain * dat, long unsigned int value)
{
  if (value > 255)
    {
      bit_write_BB(dat, 0);
      bit_write_RL(dat, value);
    }
  else if (value == 0)
    bit_write_BB(dat, 2);
  else
    {
      bit_write_BB(dat, 1);
      bit_write_RC(dat, value);
    }
}

/** Read 1 bitdouble (compacted data).
 */
BITCODE_BD
bit_read_BD(Bit_Chain * dat)
{
  unsigned char two_bit_code;
  long int *res;
  double result;

  two_bit_code = bit_read_BB(dat);

  if (two_bit_code == 0)
    {
      result = bit_read_RD(dat);
      return (result);
    }
  else if (two_bit_code == 1)
    return (1.0);
  else if (two_bit_code == 2)
    return (0.0);
  else /* if (two_bit_code == 3) */
    {
      LOG_ERROR("bit_read_BD: unexpected 2-bit code: '11'")
      /* create a Not-A-Number (NaN) */
      res = (long int *) &result;
      res[0] = -1;
      res[1] = -1;
      return (result);
    }
}

/** Write 1 bitdouble (compacted data).
 */
void
bit_write_BD(Bit_Chain * dat, double value)
{
  if (value == 0.0)
    bit_write_BB(dat, 2);
  else if (value == 1.0)
    bit_write_BB(dat, 1);
  else
    {
      bit_write_BB(dat, 0);
      bit_write_RD(dat, value);
    }
}

/** Read 1 modular char (max 4 bytes).
 */
BITCODE_MC
bit_read_MC(Bit_Chain * dat)
{
  int i, j;
  int negative;
  unsigned char byte[4];
  long unsigned int result;

  negative = 0;
  result = 0;
  for (i = 3, j = 0; i >= 0; i--, j += 7)
    {
      byte[i] = bit_read_RC(dat);
      if (!(byte[i] & 0x80))
        {
          if ((byte[i] & 0x40))
            {
              negative = 1;
              byte[i] &= 0xbf;
            }
          result |= (((long unsigned int) byte[i]) << j);
          return (negative ? -((long int) result) : (long int) result);
        }
      else
        byte[i] &= 0x7f;
      result |= ((long unsigned int) byte[i]) << j;
    }

  LOG_ERROR("bit_read_MC: error parsing modular char.")
  return 0; /* error... */
}

/** Write 1 modular char (max 4 bytes).
 */
void
bit_write_MC(Bit_Chain * dat, long int val)
{
  int i, j;
  int negative;
  unsigned char byte[4];
  long unsigned int mask;
  long unsigned int value;

  if (val < 0)
    {
      negative = 1;
      value = (long unsigned int) -val;
    }
  else
    {
      negative = 0;
      value = (long unsigned int) val;
    }

  mask = 0x0000007f;
  for (i = 3, j = 0; i > -1; i--, j += 7)
    {
      byte[i] = (unsigned char) ((value & mask) >> j);
      byte[i] |= 0x80;
      mask = mask << 7;
    }
  for (i = 0; i < 3; i++)
    if (byte[i] & 0x7f)
      break;

  if (byte[i] & 0x40)
    i--;
  byte[i] &= 0x7f;
  if (negative)
    byte[i] |= 0x40;
  for (j = 3; j >= i; j--)
    bit_write_RC(dat, byte[j]);
  //if (value == 64) printf ("(%2X) \n", byte[i]);
}

/** Read 1 modular short (max 2 words).
 */
BITCODE_MS
bit_read_MS(Bit_Chain * dat)
{
  int i, j;
  unsigned int word[2];
  long unsigned int result;

  result = 0;
  for (i = 1, j = 0; i > -1; i--, j += 15)
    {
      word[i] = bit_read_RS(dat);
      if (!(word[i] & 0x8000))
        {
          result |= (((long unsigned int) word[i]) << j);
          return (result);
        }
      else
        word[i] &= 0x7fff;
      result |= ((long unsigned int) word[i]) << j;
    }
  LOG_ERROR("bit_read_MS: error parsing modular short.")
  return 0; /* error... */
}

/** Write 1 modular short (max 2 words).
 */
void
bit_write_MS(Bit_Chain * dat, long unsigned int value)
{
  int i, j;
  unsigned int word[4];
  long unsigned int mask;

  mask = 0x00007fff;
  for (i = 1, j = 0; i >= 0; i--, j += 15)
    {
      word[i] = ((unsigned int) ((value & mask) >> j)) | 0x8000;
      mask = mask << 15;
    }
  /* TODO: useless?
   for (i = 0; i < 1; i++)
   if (word[i] & 0x7fff)
   break;
   */
  i = 1;
  word[i] &= 0x7fff;
  for (j = 1; j >= i; j--)
    bit_write_RS(dat, word[j]);
}

/** Read bit-extrusionn.
 */
void
bit_read_BE(Bit_Chain * dat, double *x, double *y, double *z)
{
  if (dat->version >= R_2000 && bit_read_B(dat))
    {
      *x = 0.0;
      *y = 0.0;
      *z = 1.0;
    }
  else
    {
      *x = bit_read_BD(dat);
      *y = bit_read_BD(dat);
      *z = bit_read_BD(dat);
    }
}

/** Write bit-extrusionn.
 */
void
bit_write_BE(Bit_Chain * dat, double x, double y, double z)
{
  if (dat->version >= R_2000 && x == 0.0 && y == 0.0 && z == 1.0)
    bit_write_B(dat, 1);
  else
    {
      bit_write_B(dat, 0);
      bit_write_BD(dat, x);
      bit_write_BD(dat, y);
      bit_write_BD(dat, z);
    }
}

/** Read bit-double with default.
 */
BITCODE_DD
bit_read_DD(Bit_Chain * dat, double default_value)
{
  unsigned char two_bit_code;
  unsigned char *uchar_result;

  two_bit_code = bit_read_BB(dat);
  if (two_bit_code == 0)
    return default_value;
  if (two_bit_code == 3)
    return (bit_read_RD(dat));
  if (two_bit_code == 2)
    {
      uchar_result = (unsigned char *) &default_value;
      uchar_result[4] = bit_read_RC(dat);
      uchar_result[5] = bit_read_RC(dat);
      uchar_result[0] = bit_read_RC(dat);
      uchar_result[1] = bit_read_RC(dat);
      uchar_result[2] = bit_read_RC(dat);
      uchar_result[3] = bit_read_RC(dat);

      return default_value;
    }
  else /* if (two_bit_code == 1) */
    {
      uchar_result = (unsigned char *) &default_value;
      uchar_result[0] = bit_read_RC(dat);
      uchar_result[1] = bit_read_RC(dat);
      uchar_result[2] = bit_read_RC(dat);
      uchar_result[3] = bit_read_RC(dat);

      return default_value;
    }
}

/** Write bit-double with default.
 */
void
bit_write_DD(Bit_Chain * dat, double value, double default_value)
{
  unsigned char *uchar_value;

  unsigned int *uint_value;
  unsigned int *uint_default;

  if (value == default_value)
    bit_write_BB(dat, 0);
  else
    {
      uchar_value = (unsigned char *) &value;
      uint_value = (unsigned int *) &value;
      uint_default = (unsigned int *) &default_value;
      if (uint_value[0] == uint_default[0])
        {
          if (uint_value[1] != uint_default[1])
            {
              bit_write_BB(dat, 2);
              bit_write_RC(dat, uchar_value[4]);
              bit_write_RC(dat, uchar_value[5]);
              bit_write_RC(dat, uchar_value[0]);
              bit_write_RC(dat, uchar_value[1]);
              bit_write_RC(dat, uchar_value[2]);
              bit_write_RC(dat, uchar_value[3]);
            }
          else
            {
              bit_write_BB(dat, 1);
              bit_write_RC(dat, uchar_value[0]);
              bit_write_RC(dat, uchar_value[1]);
              bit_write_RC(dat, uchar_value[2]);
              bit_write_RC(dat, uchar_value[3]);
            }
        }
      else
        {
          bit_write_BB(dat, 0);
          bit_write_RD(dat, value);
        }
    }
}

/** Read bit-thickness.
 */
BITCODE_BT
bit_read_BT(Bit_Chain * dat)
{
  int mode = 0;

  if (dat->version >= R_2000)
    mode = bit_read_B(dat);

  return (mode ? 0.0 : bit_read_BD(dat));
}

/** Write bit-thickness.
 */
void
bit_write_BT(Bit_Chain * dat, double value)
{
  if (dat->version >= R_2000 && value == 0.0)
    bit_write_B(dat, 1);
  else
    {
      bit_write_B(dat, 0);
      bit_write_BD(dat, value);
    }
}

/** Read handle-references.
 */
int
bit_read_H(Bit_Chain * dat, Dwg_Handle * handle)
{
  unsigned char *val;
  int i;

  handle->code = bit_read_RC(dat);
  handle->size = handle->code & 0x0f;
  handle->code = (handle->code & 0xf0) >> 4;

  handle->value = 0;
  if (handle->size > 4)
    {
      LOG_ERROR(
          "handle-reference is longer than 4 bytes: %i.%i.%lu",
          handle->code, handle->size, handle->value)
      handle->size = 0;
      return (-1);
    }

  //XXX is this code portable?
  val = (unsigned char *) &handle->value;
  for (i = handle->size - 1; i >= 0; i--)
    val[i] = bit_read_RC(dat);

  return (0);
}

/** Write handle-references.
 */
void
bit_write_H(Bit_Chain * dat, Dwg_Handle * handle)
{
  int i, j;
  unsigned char *val;
  unsigned char code_counter;

  if (handle->value == 0)
    {
      bit_write_RC(dat, (handle->code << 4));
      return;
    }

  val = (unsigned char *) &handle->value;
  for (i = 3; i >= 0; i--)
    if (val[i])
      break;

  code_counter = handle->code << 4;
  code_counter |= i + 1;

  bit_write_RC(dat, code_counter);

  for (; i >= 0; i--)
    bit_write_RC(dat, val[i]);
}

/** Only read CRK-numbers, without checking, only in order to go to
 * the next byte, while jumping contingent non-used bits
 */
unsigned int
bit_read_CRC(Bit_Chain * dat)
{
  unsigned int result;
  unsigned char res[2];

  if (dat->bit > 0)
    {
      dat->byte++;
      dat->bit = 0;
    }

  res[0] = bit_read_RC(dat);
  res[1] = bit_read_RC(dat);

  result = (unsigned int) (res[0] << 8 | res[1]);

  return result;
}

/** Read and check CRC-number.
 */
int
bit_check_CRC(Bit_Chain * dat, long unsigned int start_address,
    unsigned int seed)
{
  unsigned int calculated;
  unsigned int read;
  unsigned char res[2];

  if (dat->bit > 0)
    dat->byte++;
  dat->bit = 0;

  calculated = bit_ckr8(seed, &(dat->chain[start_address]), dat->byte
      - start_address);

  res[0] = bit_read_RC(dat);
  res[1] = bit_read_RC(dat);

  read = (unsigned int) (res[0] << 8 | res[1]);

  return (calculated == read);
}

/** Create and write CRC-number.
 */
unsigned int
bit_write_CRC(Bit_Chain * dat, long unsigned int start_address,
    unsigned int seed)
{
  unsigned int crc;

  while (dat->bit > 0)
    bit_write_B(dat, 0);

  crc = bit_ckr8(seed, &(dat->chain[start_address]), dat->byte - start_address);

  bit_write_RC(dat, (unsigned char) (crc >> 8));
  bit_write_RC(dat, (unsigned char) (crc & 0xFF));

  return (crc);
}

/** Read simple text. After usage, the allocated memory must be properly freed.
 */
BITCODE_TV
bit_read_TV(Bit_Chain * dat)
{
  unsigned int i;
  unsigned int length;
  unsigned char *chain;

  length = bit_read_BS(dat);
  chain = (unsigned char *) malloc(length + 1);
  for (i = 0; i < length; i++)
    {
      chain[i] = bit_read_RC(dat);
      if (chain[i] == 0)
        chain[i] = '*';
      else if (!isprint (chain[i]))
        chain[i] = '~';
    }
  chain[i] = '\0';

  return (chain);
}

/** Write simple text.
 */
void
bit_write_TV(Bit_Chain * dat, unsigned char *chain)
{
  int i;
  int length;

  length = strlen((const char *)chain);
  bit_write_BS(dat, length);
  for (i = 0; i < length; i++)
    bit_write_RC(dat, chain[i]);
}

/** Read 1 bitlong according to normal order
 */
long unsigned int
bit_read_L(Bit_Chain * dat)
{
  unsigned char btk[4];

  btk[3] = bit_read_RC(dat);
  btk[2] = bit_read_RC(dat);
  btk[1] = bit_read_RC(dat);
  btk[0] = bit_read_RC(dat);

  return (*((long unsigned int *) btk));
}

/** Write 1 bitlong according to normal order
 */
void
bit_write_L(Bit_Chain * dat, long unsigned int value)
{
  unsigned char *btk;

  btk = (unsigned char *) value;
  bit_write_RC(dat, btk[3]);
  bit_write_RC(dat, btk[2]);
  bit_write_RC(dat, btk[1]);
  bit_write_RC(dat, btk[0]);
}

/** Read color
 */
void
bit_read_CMC(Bit_Chain * dat, Dwg_Color* color)
{
  color->index = bit_read_BS(dat);
  if (dat->version >= R_2004)
    {
      color->rgb = bit_read_BL(dat);
      color->byte = bit_read_RC(dat);
      if (color->byte & 1)
        color->name = (char*)bit_read_TV(dat);
      if (color->byte & 2)
        color->book_name = (char*)bit_read_TV(dat);
    }
}

/** Write color
 */
void
bit_write_CMC(Bit_Chain * dat, Dwg_Color* color)
{
  bit_write_BS(dat, color->index);
  if (dat->version >= R_2004)
    {
      bit_write_BL(dat, color->rgb);
      bit_write_RC(dat, color->byte);
      if (color->byte & 1)
        bit_write_TV(dat,(unsigned char*) color->name);
      if (color->byte & 2)
        bit_write_TV(dat, (unsigned char*)color->book_name);
    }
}

/** Search for a sentinel; if found, positions "dat->byte" immediately after it.
 */
int
bit_search_sentinel(Bit_Chain * dat, unsigned char sentinel[16])
{
  long unsigned int i, j;

  for (i = 0; i < dat->size; i++)
    {
      for (j = 0; j < 16; j++)
        {
          if (dat->chain[i + j] != sentinel[j])
            break;
        }
      if (j == 16)
        {
          dat->byte = i + j;
          dat->bit = 0;
          return -1;
        }
    }
  return 0;
}

void
bit_write_sentinel(Bit_Chain * dat, unsigned char sentinel[16])
{
  int i;

  for (i = 0; i < 16; i++)
    bit_write_RC(dat, sentinel[i]);
}

/*
 * Allocates memory space for bit_chain
 */
#define CHAIN_BLOCK 40960
void
bit_chain_alloc(Bit_Chain * dat)
{
  if (dat->size == 0)
    {
      dat->chain = (unsigned char *)calloc(1, CHAIN_BLOCK);
      dat->size = CHAIN_BLOCK;
      dat->byte = 0;
      dat->bit = 0;
    }
  else
    {
      dat->chain = (unsigned char *)realloc(dat->chain, dat->size + CHAIN_BLOCK);
      dat->size += CHAIN_BLOCK;
    }
}

void
bit_print(Bit_Chain * dat, long unsigned int size)
{
  unsigned char sig;
  long unsigned int i, j, k;

  printf("---------------------------------------------------------");
  if (size > dat->size)
    size = dat->size;
  for (i = 0; i < size; i++)
    {
      if (i % 16 == 0)
        printf("\n[0x%04X]: ", (unsigned int) i);
      printf("%02X ", dat->chain[i]);
      if (i % 16 == 15)
        for (j = i - 15; j <= i; j++)
          {
            sig = dat->chain[j];
            printf("%c", sig >= ' ' && sig < 128 ? sig : '.');
          }
    }
  puts("");
  puts("---------------------------------------------------------");
}

void
bit_explore_chain(Bit_Chain * dat, long unsigned int size)
{
  unsigned char sig;
  long unsigned int i, j, k;

  if (size > dat->size)
    size = dat->size;

  for (k = 0; k < 8; k++)
    {
      printf("---------------------------------------------------------");
      dat->byte = 0;
      dat->bit = k;
      for (i = 0; i < size - 1; i++)
        {
          if (i % 16 == 0)
            printf("\n[0x%04X]: ", (unsigned int) i);
          sig = bit_read_RC(dat);
          printf("%c", sig >= ' ' && sig < 128 ? sig : '.');
        }
      puts("");
    }
  puts("---------------------------------------------------------");
}

/*------------------------------------------------------------------------------
 * Private functions
 */

unsigned int
bit_ckr8(unsigned int dx, unsigned char *adr, long n)
{
  register unsigned char al;

  static unsigned int ckrtable[256] =
    { 0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241, 0xC601,
        0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440, 0xCC01, 0x0CC0,
        0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40, 0x0A00, 0xCAC1, 0xCB81,
        0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841, 0xD801, 0x18C0, 0x1980, 0xD941,
        0x1B00, 0xDBC1, 0xDA81, 0x1A40, 0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01,
        0x1DC0, 0x1C80, 0xDC41, 0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0,
        0x1680, 0xD641, 0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081,
        0x1040, 0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
        0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441, 0x3C00,
        0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41, 0xFA01, 0x3AC0,
        0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840, 0x2800, 0xE8C1, 0xE981,
        0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41, 0xEE01, 0x2EC0, 0x2F80, 0xEF41,
        0x2D00, 0xEDC1, 0xEC81, 0x2C40, 0xE401, 0x24C0, 0x2580, 0xE541, 0x2700,
        0xE7C1, 0xE681, 0x2640, 0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0,
        0x2080, 0xE041, 0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281,
        0x6240, 0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
        0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41, 0xAA01,
        0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840, 0x7800, 0xB8C1,
        0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41, 0xBE01, 0x7EC0, 0x7F80,
        0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40, 0xB401, 0x74C0, 0x7580, 0xB541,
        0x7700, 0xB7C1, 0xB681, 0x7640, 0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101,
        0x71C0, 0x7080, 0xB041, 0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0,
        0x5280, 0x9241, 0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481,
        0x5440, 0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
        0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841, 0x8801,
        0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40, 0x4E00, 0x8EC1,
        0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41, 0x4400, 0x84C1, 0x8581,
        0x4540, 0x8701, 0x47C0, 0x4680, 0x8641, 0x8201, 0x42C0, 0x4380, 0x8341,
        0x4100, 0x81C1, 0x8081, 0x4040 };

  for (; n > 0; n--)
    {
      al = (unsigned char) ((*adr) ^ ((unsigned char) (dx & 0xFF)));
      dx = (dx >> 8) & 0xFF;
      dx = dx ^ ckrtable[al & 0xFF];
      adr++;
    }
  return (dx);
}
