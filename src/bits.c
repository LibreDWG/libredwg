/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2009,2018-2021 Free Software Foundation, Inc.              */
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
 * modified by Reini Urban
 */

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <inttypes.h>
#include <math.h>
#include <assert.h>
#if defined(HAVE_WCHAR_H) && defined(SIZEOF_WCHAR_T) && SIZEOF_WCHAR_T == 2
#  include <wchar.h>
#endif
// else we roll our own, Latin-1 only.

#ifdef DWG_ABORT
static unsigned int errors = 0;
#  ifndef DWG_ABORT_LIMIT
#    define DWG_ABORT_LIMIT 200
#  endif
#endif

static unsigned int loglevel;
#define DWG_LOGLEVEL loglevel
#include "logging.h"
#include "bits.h"

/*------------------------------------------------------------------------------
 * Public functions
 */

/* Advance bits (forward or backward)
 */
void
bit_advance_position (Bit_Chain *dat, long advance)
{
  const unsigned long pos  = bit_position (dat);
  const unsigned long endpos = dat->size * 8;
  long bits = (long)dat->bit + advance;
  if (pos + advance > endpos)
    {
      loglevel = dat->opts & DWG_OPTS_LOGLEVEL;
      LOG_ERROR ("%s buffer overflow at pos %lu.%u, size %lu, advance by %ld",
                 __FUNCTION__, dat->byte, dat->bit, dat->size, advance);
    }
  else if ((long)pos + advance < 0)
    {
      loglevel = dat->opts & DWG_OPTS_LOGLEVEL;
      LOG_ERROR ("buffer underflow at pos %lu.%u, size %lu, advance by %ld",
                 dat->byte, dat->bit, dat->size, advance)
      dat->byte = 0;
      dat->bit = 0;
      return;
    }
  dat->byte += (bits >> 3);
  dat->bit = bits & 7;
}

/* Absolute get in bits
 */
unsigned long
bit_position (Bit_Chain *dat)
{
  return (dat->byte * 8) + (dat->bit & 7);
}

/* Absolute set in bits
 */
void
bit_set_position (Bit_Chain *dat, unsigned long bitpos)
{
  dat->byte = bitpos >> 3;
  dat->bit = bitpos & 7;
  if (dat->byte > dat->size || (dat->byte == dat->size && dat->bit))
    {
      loglevel = dat->opts & DWG_OPTS_LOGLEVEL;
      LOG_ERROR ("%s buffer overflow at %lu, have %lu", __FUNCTION__,
                 dat->byte, dat->size)
    }
}

/* Set the chain so that dat->byte starts at 0 */
void
bit_reset_chain (Bit_Chain *dat)
{
  unsigned long pos = dat->byte;
  dat->byte = 0;
  if (pos < dat->size) // not already overflowed
    dat->chain += pos;
  if (dat->size > 0)
    dat->size -= pos;
}

#ifdef DWG_ABORT
#  define CHK_OVERFLOW(func, retval)                                          \
    if (dat->byte >= dat->size)                                               \
      {                                                                       \
        loglevel = dat->opts & DWG_OPTS_LOGLEVEL;                             \
        LOG_ERROR ("%s buffer overflow at %lu >= %lu", func, dat->byte,       \
                   dat->size)                                                 \
        if (++errors > DWG_ABORT_LIMIT)                                       \
          abort ();                                                           \
        return retval;                                                        \
      }
#else
#  define CHK_OVERFLOW(func, retval)                                          \
    if (dat->byte >= dat->size)                                               \
      {                                                                       \
        loglevel = dat->opts & DWG_OPTS_LOGLEVEL;                             \
        LOG_ERROR ("%s buffer overflow at %lu >= %lu", func, dat->byte,       \
                   dat->size)                                                 \
        return retval;                                                        \
      }
#endif

#define CHK_OVERFLOW_PLUS(plus, func, retval)                                 \
    if (dat->byte + (plus) > dat->size)                                       \
      {                                                                       \
        loglevel = dat->opts & DWG_OPTS_LOGLEVEL;                             \
        LOG_ERROR ("%s buffer overflow at %lu + %d >= %lu", func, dat->byte,  \
                   (int)(plus), dat->size)                                    \
        return retval;                                                        \
      }

/** Read 1 bit.
 */
BITCODE_B
bit_read_B (Bit_Chain *dat)
{
  unsigned char result;
  unsigned char byte;

  CHK_OVERFLOW(__FUNCTION__,0)
  byte = dat->chain[dat->byte];
  result = (byte & (0x80 >> dat->bit)) >> (7 - dat->bit);

  bit_advance_position (dat, 1);
  return result;
}

/** Write 1 bit.
 */
void
bit_write_B (Bit_Chain *dat, unsigned char value)
{
  if (dat->byte >= dat->size)
    bit_chain_alloc (dat);

  if (value)
    dat->chain[dat->byte] |= 0x80 >> dat->bit;
  else
    dat->chain[dat->byte] &= ~(0x80 >> dat->bit);

  bit_advance_position (dat, 1);
}

/** Read 2 bits.
 */
BITCODE_BB
bit_read_BB (Bit_Chain *dat)
{
  unsigned char result;
  unsigned char byte;

  CHK_OVERFLOW(__FUNCTION__,0)
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

  bit_advance_position (dat, 2);
  return result;
}

/** Write 2 bits.
 */
void
bit_write_BB (Bit_Chain *dat, unsigned char value)
{
  unsigned char mask;
  unsigned char byte;

  if (dat->byte >= dat->size)
    bit_chain_alloc (dat);
  byte = dat->chain[dat->byte];
  if (dat->bit < 7)
    {
      mask = 0xc0 >> dat->bit;
      dat->chain[dat->byte] = (byte & ~mask) | (value << (6 - dat->bit));
    }
  else
    {
      dat->chain[dat->byte] = (byte & 0xfe) | (value >> 1);
      if (dat->byte + 1 >= dat->size)
        bit_chain_alloc (dat);
      byte = dat->chain[dat->byte + 1];
      dat->chain[dat->byte + 1] = (byte & 0x7f) | ((value & 0x01) << 7);
    }

  bit_advance_position (dat, 2);
}

/** Read 1-3 bits
 *  Keep reading bits until a zero bit is encountered, => 0,2,6,7.
 *  0: 0, 10: 2, 110: 6, 111: 7. 100 for 4 or 101 for 5 is invalid.
 */
BITCODE_3B
bit_read_3B (Bit_Chain *dat)
{
  BITCODE_3B result = bit_read_B (dat);
  if (result)
    {
      BITCODE_3B next = bit_read_B (dat);
      if (next)
        {
          next = bit_read_B (dat);
          return next ? 7 : 6;
        }
      else
        {
          return 2;
        }
    }
  else
    {
      return 0;
    }
}

/** Write 1-3 bits
 */
void
bit_write_3B (Bit_Chain *dat, unsigned char value)
{
  if (value > 7)
    {
      loglevel = dat->opts & DWG_OPTS_LOGLEVEL;
      LOG_ERROR ("Invalid bit_write_3B value %d > 7", value)
      return;
    }
  bit_write_B (dat, value & 1);
  if (value)
    {
      value >>= 1;
      bit_write_B (dat, value & 1);
      if (value)
        {
          value >>= 1;
          bit_write_B (dat, value & 1);
        }
    }
}

/** Read 4 bits.
 */
BITCODE_4BITS
bit_read_4BITS (Bit_Chain *dat)
{
  // clang-format off
  BITCODE_4BITS b =
         bit_read_B (dat) << 3 |
         bit_read_B (dat) << 2 |
         bit_read_B (dat) << 1 |
         bit_read_B (dat);
  // clang-format on
  return b;
}

/** Write 4 bits.
 */
void
bit_write_4BITS (Bit_Chain *dat, unsigned char value)
{
  bit_write_B (dat, value & 8);
  bit_write_B (dat, value & 4);
  bit_write_B (dat, value & 2);
  bit_write_B (dat, value & 1);
}

/** Read 1 byte (raw char).
 */
BITCODE_RC
bit_read_RC (Bit_Chain *dat)
{
  unsigned char result;
  unsigned char byte;

  CHK_OVERFLOW(__FUNCTION__,0)
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
      else
        {
          loglevel = dat->opts & DWG_OPTS_LOGLEVEL;
          LOG_ERROR ("%s buffer overflow at %lu", __FUNCTION__, dat->byte + 1)
          return 0;
        }
    }

  bit_advance_position (dat, 8);
  return ((unsigned char)result);
}

/** Write 1 byte (raw char).
 */
void
bit_write_RC (Bit_Chain *dat, unsigned char value)
{
  unsigned char byte;
  unsigned char remainder;

  if (dat->bit == 0)
    {
      while (dat->byte >= dat->size)
        bit_chain_alloc (dat);
      dat->chain[dat->byte] = value;
    }
  else
    {
      while (dat->byte + 1 >= dat->size)
        bit_chain_alloc (dat);
      byte = dat->chain[dat->byte];
      remainder = byte & (0xff << (8 - dat->bit));
      dat->chain[dat->byte] = remainder | (value >> dat->bit);
      byte = dat->chain[dat->byte + 1];
      remainder = byte & (0xff >> dat->bit);
      dat->chain[dat->byte + 1] = remainder | (value << (8 - dat->bit));
    }

  bit_advance_position (dat, 8);
}

/** Read 1 raw short (BE short).
 */
BITCODE_RS
bit_read_RS (Bit_Chain *dat)
{
  unsigned char byte1, byte2;

  // least significant byte first:
  byte1 = bit_read_RC (dat);
  CHK_OVERFLOW(__FUNCTION__,0)
  byte2 = bit_read_RC (dat);
  return (BITCODE_RS) ((byte2 << 8) | byte1);
}

/** Read 1 raw short little-endian.
 */
BITCODE_RS
bit_read_RS_LE (Bit_Chain *dat)
{
  unsigned char byte1, byte2;
  byte1 = bit_read_RC (dat);
  CHK_OVERFLOW(__FUNCTION__,0)
  byte2 = bit_read_RC (dat);
  return (BITCODE_RS) ((byte1 << 8) | byte2);
}

/** Write 1 raw short (BE short).
 */
void
bit_write_RS (Bit_Chain *dat, BITCODE_RS value)
{
  // least significant byte first:
  bit_write_RC (dat, value & 0xFF);
  bit_write_RC (dat, value >> 8);
}

/** Write 1 raw short little-endian.
 */
void
bit_write_RS_LE (Bit_Chain *dat, BITCODE_RS value)
{
  bit_write_RC (dat, value >> 8);
  bit_write_RC (dat, value & 0xFF);
}

/** Read 1 raw long (4 byte, BE).
 */
BITCODE_RL
bit_read_RL (Bit_Chain *dat)
{
  BITCODE_RS word1, word2;

  // least significant word first
  word1 = bit_read_RS (dat);
  CHK_OVERFLOW(__FUNCTION__,0)
  word2 = bit_read_RS (dat);
  return ((((uint32_t)word2) << 16) | ((uint32_t)word1));
}

/** Write 1 raw long (4 byte, BE).
 */
void
bit_write_RL (Bit_Chain *dat, BITCODE_RL value)
{
  const uint32_t l = value;
  // least significant word first:
  bit_write_RS (dat, l & 0xFFFF);
  bit_write_RS (dat, l >> 16);
}

/** Read 1 raw long (4 byte, LE).
 */
BITCODE_RL
bit_read_RL_LE (Bit_Chain *dat)
{
  BITCODE_RS word1, word2;

  // most significant word first
  word1 = bit_read_RS_LE (dat);
  word2 = bit_read_RS_LE (dat);
  return ((((uint32_t)word1) << 16) | ((uint32_t)word2));
}

/** Write 1 raw long (4 byte, LE).
 */
void
bit_write_RL_LE (Bit_Chain *dat, BITCODE_RL value)
{
  // most significant word first:
  bit_write_RS_LE (dat, value >> 16);
  bit_write_RS_LE (dat, value & 0xFFFF);
}

/** Read 1 raw 64bit long (8 byte, BE).
 */
BITCODE_RLL
bit_read_RLL (Bit_Chain *dat)
{
  BITCODE_RL word1, word2;

  // least significant word first
  word1 = bit_read_RL (dat);
  CHK_OVERFLOW(__FUNCTION__,0)
  word2 = bit_read_RL (dat);
  return ((((uint64_t)word2) << 32) | ((uint64_t)word1));
}

/** Write 1 raw 64bit long  (8 byte, BE).
 */
void
bit_write_RLL (Bit_Chain *dat, BITCODE_RLL value)
{
  // least significant word first
  bit_write_RL (dat, value & 0xFFFFFFFF);
  bit_write_RL (dat, value >> 32);
}

/** Read 1 raw double (8 bytes, IEEE-754).
 */
BITCODE_RD
bit_read_RD (Bit_Chain *dat)
{
  int i;
  unsigned char byte[8];
  double *result;

  //CHK_OVERFLOW_PLUS (8, __FUNCTION__, bit_nan ())
  // TODO: big-endian
  for (i = 0; i < 8; i++)
    byte[i] = bit_read_RC (dat);

  result = (double *)byte;
  return (*result);
}

/** Write 1 raw double (8 bytes, IEEE-754).
 */
void
bit_write_RD (Bit_Chain *dat, double value)
{
  int i;
  unsigned char *val;

  // TODO: big-endian
  val = (unsigned char *)&value;

  for (i = 0; i < 8; i++)
    bit_write_RC (dat, val[i]);
}

/** Read 1 bitshort (compacted data).
 */
BITCODE_BS
bit_read_BS (Bit_Chain *dat)
{
  const unsigned char two_bit_code = bit_read_BB (dat);
  if (two_bit_code == 0)
    {
      CHK_OVERFLOW(__FUNCTION__, 0)
      return bit_read_RS (dat);
    }
  else if (two_bit_code == 1)
    return (BITCODE_BS)bit_read_RC (dat) & 0xFF;
  else if (two_bit_code == 2)
    return 0;
  else /* if (two_bit_code == 3) */
    return 256;
}

/** Write 1 bitshort (compacted data).
 */
void
bit_write_BS (Bit_Chain *dat, BITCODE_BS value)
{
  // BITCODE_BS is defined as uint16_t, but better safe than sorry
  const uint16_t l = value;
  if (l > 256)
    {
      bit_write_BB (dat, 0);
      bit_write_RS (dat, value);
    }
  else if (value == 0)
    bit_write_BB (dat, 2);
  else if (value == 256)
    bit_write_BB (dat, 3);
  else
    {
      bit_write_BB (dat, 1);
      bit_write_RC (dat, value);
    }
}

/** Read 1 bitlong (compacted data).
 */
BITCODE_BL
bit_read_BL (Bit_Chain *dat)
{
  const unsigned char two_bit_code = bit_read_BB (dat);
  if (two_bit_code == 0)
    {
      CHK_OVERFLOW(__FUNCTION__, 0)
      return bit_read_RL (dat);
    }
  else if (two_bit_code == 1)
    return (BITCODE_BL)bit_read_RC (dat) & 0xFF;
  else if (two_bit_code == 2)
    return 0;
  else /* if (two_bit_code == 3) */
    {
      loglevel = dat->opts & DWG_OPTS_LOGLEVEL;
      LOG_ERROR ("bit_read_BL: unexpected 2-bit code: '11'")
      return 256;
    }
}

/** Write 1 bitlong (compacted data).
 */
void
bit_write_BL (Bit_Chain *dat, BITCODE_BL value)
{
  // BITCODE_BL is signed int32_t
  const uint32_t l = value;
  if (l > 255)
    {
      bit_write_BB (dat, 0);
      bit_write_RL (dat, value);
    }
  else if (l == 0)
    bit_write_BB (dat, 2);
  else
    {
      bit_write_BB (dat, 1);
      bit_write_RC (dat, value);
    }
}

/** Write signed bitlong (compacted data).
 */
void
bit_write_BLd (Bit_Chain *dat, BITCODE_BLd value)
{
  const BITCODE_BL l = value;
  if (l > 255)
    {
      bit_write_BB (dat, 0);
      bit_write_RL (dat, (BITCODE_RL)value);
    }
  else if (l == 0)
    bit_write_BB (dat, 2);
  else
    {
      bit_write_BB (dat, 1);
      bit_write_RC (dat, value);
    }
}

/** Read object type 2010+ (BB + 1 or 2 bytes).
 *  par 2.12
 */
BITCODE_BS
bit_read_BOT (Bit_Chain *dat)
{
  unsigned char two_bit_code;

  two_bit_code = bit_read_BB (dat);

  if (two_bit_code == 0)
    {
      CHK_OVERFLOW(__FUNCTION__, 0)
      return bit_read_RC (dat);
    }
  else if (two_bit_code == 1)
    return bit_read_RC (dat) + 0x1f0;
  else
    return bit_read_RS (dat);
}

/** Write object type 2010+ (BB + 1 or 2 bytes).
 */
void
bit_write_BOT (Bit_Chain *dat, BITCODE_BS value)
{
  if (value < 256)
    {
      bit_write_BB (dat, 0);
      bit_write_RC (dat, value);
    }
  else if (value < 0x7fff)
    {
      bit_write_BB (dat, 1);
      bit_write_RC (dat, value - 0x1f0);
    }
  else
    {
      bit_write_BB (dat, 1);
      bit_write_RS (dat, value);
    }
}

/** Read 1 bitlonglong (compacted uint64_t) for REQUIREDVERSIONS, preview_size.
 *  ODA doc bug. ODA say 1-3 bits until the first 0 bit. See 3BLL.
 *  The first 3 bits indicate the length l (see paragraph 2.1). Then
 *  l bytes follow, which represent the number (the least significant
 *  byte is first).
 */
BITCODE_BLL
bit_read_BLL (Bit_Chain *dat)
{
  unsigned int i, len;
  BITCODE_BLL result = 0ULL;
  len = bit_read_BB (dat) << 1 | bit_read_B (dat);
  switch (len)
    {
    case 1:
      return bit_read_RC (dat);
    case 2:
      return bit_read_RS (dat);
    case 4:
      return bit_read_RL (dat);
    default:
      CHK_OVERFLOW(__FUNCTION__, 0)
      for (i = 0; i < len; i++)
        {
          result <<= 8;
          result |= bit_read_RC (dat);
        }
      return result;
    }
}

/** Read 1 bitlonglong (compacted uint64_t) as documented (but unused).
 *  The first 1-3 bits indicate the length l (see paragraph 2.1). Then
 *  l bytes follow, which represent the number (the least significant
 *  byte is first).
 */
BITCODE_BLL
bit_read_3BLL (Bit_Chain *dat)
{
  unsigned int i, len;
  BITCODE_BLL result = 0ULL;
  len = bit_read_3B (dat);
  CHK_OVERFLOW(__FUNCTION__, 0)
  for (i = 0; i < len; i++)
    {
      result <<= 8;
      result |= bit_read_RC (dat);
    }
  return result;
}

/** Write 1 bitlonglong (compacted data).
 */
void
bit_write_BLL (Bit_Chain *dat, BITCODE_BLL value)
{
  // 64bit into how many bytes?
  int i;
  int len = 0;
  BITCODE_BLL umax = 0xf000000000000000ULL;
  for (i = 16; i; i--, umax >>= 8)
    {
      if (value & umax)
        {
          len = i;
          break;
        }
    }
  bit_write_BB (dat, len << 2);
  bit_write_B (dat, len & 1);
  for (i = 0; i < len; i++)
    {
      // least significant byte first
      bit_write_RC (dat, value & 0xFF);
      value >>= 8;
    }
}
void
bit_write_3BLL (Bit_Chain *dat, BITCODE_BLL value)
{
  // 64bit into how many bytes?
  int i;
  int len = 0;
  BITCODE_BLL umax = 0xf000000000000000ULL;
  for (i = 16; i; i--, umax >>= 8)
    {
      if (value & umax)
        {
          len = i;
          break;
        }
    }
  bit_write_3B (dat, len);
  for (i = 0; i < len; i++)
    {
      // least significant byte first
      bit_write_RC (dat, value & 0xFF);
      value >>= 8;
    }
}

/** Read 1 bitdouble (compacted data).
 */
BITCODE_BD
bit_read_BD (Bit_Chain *dat)
{
  unsigned char two_bit_code;

  two_bit_code = bit_read_BB (dat);
  if (two_bit_code == 0)
    {
      CHK_OVERFLOW(__FUNCTION__, bit_nan ())
      return bit_read_RD (dat);
    }
  else if (two_bit_code == 1)
    return 1.0;
  else if (two_bit_code == 2)
    return 0.0;
  else /* if (two_bit_code == 3) */
    {
      loglevel = dat->opts & DWG_OPTS_LOGLEVEL;
      LOG_ERROR ("bit_read_BD: unexpected 2-bit code: '11'")
      return bit_nan ();
    }
}

/* Create a Not-A-Number (NaN) without libm dependency.
   For releases this returns 0.0, nan is only used for errors during development.
 */
BITCODE_BD
bit_nan (void)
{
#ifdef IS_RELEASE
  return 0.0;
#else
  double result;
  int32_t *res = (int32_t *)&result;
  res[0] = -1;
  res[1] = -1;
  return result;
#endif
}

int
bit_isnan (BITCODE_BD number)
{
  int32_t *res = (int32_t *)&number;
  return (res[0] == -1 && res[1] == -1);
}

/** Write 1 bitdouble (compacted data).
 */
void
bit_write_BD (Bit_Chain *dat, double value)
{
  if (value == 0.0)
    bit_write_BB (dat, 2);
  else if (value == 1.0)
    bit_write_BB (dat, 1);
  else
    {
      bit_write_BB (dat, 0);
      bit_write_RD (dat, value);
    }
}

/** Read 1 modular char (max 5 bytes, signed).
    Read bytes until the high bit of the byte is 0, drop the highest bit and
 pad with 0. If the last byte has 0x40 set, it's negative. Since the result is
 int32_t (4 byte), but there needs to be the high/follow bit set, the stream
 can be max 5 byte long (5*7 = 35 bit) 10000000 10000000 10000000 10000000
 00000100
 =>  0000000  0000000  0000000  0000000  0000100 (5*7 = 35)
 => 00001000 00000000 00000000 00000000          (4*8 = 32)
 */
BITCODE_MC
bit_read_MC (Bit_Chain *dat)
{
  int i, j;
  int negative;
  unsigned char byte[5];
  BITCODE_UMC result;

  negative = 0;
  result = 0;
  for (i = 4, j = 0; i >= 0; i--, j += 7)
    {
      byte[i] = bit_read_RC (dat);
      CHK_OVERFLOW(__FUNCTION__, 0)
      if (!(byte[i] & 0x80))
        {
          if ((byte[i] & 0x40))
            {
              negative = 1;
              byte[i] &= 0xbf;
            }
          result |= (((BITCODE_UMC)byte[i]) << j);
          return (negative ? -((BITCODE_MC)result) : (BITCODE_MC)result);
        }
      else
        byte[i] &= 0x7f;

      result |= ((BITCODE_UMC)byte[i]) << j;
    }

  loglevel = dat->opts & DWG_OPTS_LOGLEVEL;
  LOG_ERROR (
      "bit_read_MC: error parsing modular char. i=%d, j=%d, result=0x%lx,\n"
      " @%lu.@%u: [0x%x 0x%x 0x%x 0x%x 0x%x]",
      i, j, result, dat->byte - 5, dat->bit, dat->chain[dat->byte - 5],
      dat->chain[dat->byte - 4], dat->chain[dat->byte - 3],
      dat->chain[dat->byte - 2], dat->chain[dat->byte - 1])
  return 0; /* error... */
}

/** Write 1 modular char (max 5 bytes, signed).
 */
void
bit_write_MC (Bit_Chain *dat, BITCODE_MC val)
{
  int i, j;
  int negative = 0;
  unsigned char byte[5] = { 0, 0, 0, 0, 0 };
  BITCODE_UMC mask = 0x0000007f;
  BITCODE_UMC value = (BITCODE_UMC)val;

  if (val < 0)
    {
      negative = 1;
      value = (BITCODE_UMC)-val;
    }
  for (i = 4, j = 0; i >= 0; i--, j += 7)
    {
      byte[i] = (unsigned char)((value & mask) >> j);
      byte[i] |= 0x80;
      mask = mask << 7;
    }
  for (i = 0; i < 4; i++)
    if (byte[i] & 0x7f)
      break;

  if (byte[i] & 0x40 && i > 0)
    i--;
  byte[i] &= 0x7f;
  if (negative)
    byte[i] |= 0x40;
  for (j = 4; j >= i; j--)
    bit_write_RC (dat, byte[j]);
}

/** Read unsigned modular char (max 5 bytes, unsigned).
    Can be quite large if there are many deleted handles.
 */
BITCODE_UMC
bit_read_UMC (Bit_Chain *dat)
{
  int i, j;
  // eg handle FD485E65F
  #define MAX_BYTE_UMC 6
  unsigned char byte[MAX_BYTE_UMC] = { 0, 0, 0, 0, 0, 0 };
  BITCODE_UMC result;

  result = 0;
  for (i = MAX_BYTE_UMC-1, j = 0; i >= 0; i--, j += 7)
    {
      byte[i] = bit_read_RC (dat);
      CHK_OVERFLOW(__FUNCTION__, 0)
      if (!(byte[i] & 0x80))
        {
          result |= (((BITCODE_UMC)byte[i]) << j);
          return result;
        }
      else
        byte[i] &= 0x7f;

      result |= ((BITCODE_UMC)byte[i]) << j;
    }

  loglevel = dat->opts & DWG_OPTS_LOGLEVEL;
  LOG_ERROR (
      "bit_read_UMC: error parsing modular char, i=%d,j=%d,result=0x%lx", i, j,
      result)
  LOG_HANDLE ("  @%lu.%u: [0x%x 0x%x 0x%x 0x%x 0x%x]\n", dat->byte - 5,
            dat->bit, dat->chain[dat->byte - 5], dat->chain[dat->byte - 4],
            dat->chain[dat->byte - 3], dat->chain[dat->byte - 2],
            dat->chain[dat->byte - 1])
  return 0; /* error... */
}

/** Write 1 modular char (max 5 bytes, unsigned).
 */
void
bit_write_UMC (Bit_Chain *dat, BITCODE_UMC val)
{
  int i, j;
  int negative;
  unsigned char byte[5];
  BITCODE_UMC mask;
  BITCODE_UMC value;

  value = val;
  mask = 0x0000007f;
  for (i = 4, j = 0; i >= 0; i--, j += 7)
    {
      byte[i] = (unsigned char)((value & mask) >> j);
      byte[i] |= 0x80;
      mask = mask << 7;
    }
  for (i = 0; i < 4; i++)
    if (byte[i] & 0x7f)
      break;

  if (byte[i] & 0x40 && i > 0)
    i--;
  byte[i] &= 0x7f;
  for (j = 4; j >= i; j--)
    bit_write_RC (dat, byte[j]);
}

/** Read 1 modular short (max 2 words).
 */
BITCODE_MS
bit_read_MS (Bit_Chain *dat)
{
  int i, j;
  BITCODE_RS word[2] = {0, 0};
  BITCODE_MS result;

  result = 0;
  for (i = 1, j = 0; i >= 0; i--, j += 15)
    {
      word[i] = bit_read_RS (dat);
      CHK_OVERFLOW(__FUNCTION__, 0)
      if (!(word[i] & 0x8000))
        {
          result |= ((BITCODE_MS)word[i] << j);
          return result;
        }
      else
        word[i] &= 0x7fff;
      result |= ((BITCODE_MS)word[i] << j);
    }
  loglevel = dat->opts & DWG_OPTS_LOGLEVEL;
  LOG_ERROR ("bit_read_MS: error parsing modular short, i=%d,j=%d", i, j)
  return 0; /* error... */
}

/** Write 1 modular short (max 2 words).
 */
void
bit_write_MS (Bit_Chain *dat, BITCODE_MS value)
{
  bit_write_RS (dat, value);
  if (value > 0x7fff)
    bit_write_RS (dat, value >> 15);
}

/** Read bit-extrusion.
 */
void
bit_read_BE (Bit_Chain *restrict dat, double *restrict x, double *restrict y,
             double *restrict z)
{
  if (dat->from_version >= R_2000 && bit_read_B (dat))
    {
      *x = 0.0;
      *y = 0.0;
      *z = 1.0;
    }
  else
    {
      *x = bit_read_BD (dat);
      *y = bit_read_BD (dat);
      *z = bit_read_BD (dat);
      // normalize
      if (*x == 0.0 && *y == 0.0)
        *z = (*z <= 0.0) ? -1.0 : 1.0;
    }
}

/** Write bit-extrusion.
 */
void
bit_write_BE (Bit_Chain *dat, double x, double y, double z)
{
  if (dat->version >= R_2000 && x == 0.0 && y == 0.0 && z == 1.0)
    bit_write_B (dat, 1);
  else
    {
      bit_write_B (dat, 0);
      bit_write_BD (dat, x);
      bit_write_BD (dat, y);
      // normalize
      if (x == 0.0 && y == 0.0)
        z = (z <= 0.0) ? -1.0 : 1.0;
      bit_write_BD (dat, z);
    }
}

// TODO ensure length 1, dwg_geom_normalize()
void
normalize_BE (BITCODE_3BD ext)
{
  if (ext.x == 0.0 && ext.y == 0.0)
    ext.z = (ext.z <= 0.0) ? -1.0 : 1.0;
}

/** Read bit-double with default.
 */
BITCODE_DD
bit_read_DD (Bit_Chain *dat, double default_value)
{
  unsigned char two_bit_code;
  unsigned char *uchar_result;

  two_bit_code = bit_read_BB (dat);
  if (two_bit_code == 0)
    {
      CHK_OVERFLOW(__FUNCTION__, bit_nan ())
      return default_value;
    }
  if (two_bit_code == 3)
    return bit_read_RD (dat);
  if (two_bit_code == 2)
    {
      // dbl: 7654 3210
      // first 2 bits eq (6-7), the rest not (0-5)
      uchar_result = (unsigned char *)&default_value;
      CHK_OVERFLOW_PLUS (6, __FUNCTION__, bit_nan ())
      uchar_result[4] = bit_read_RC (dat);
      uchar_result[5] = bit_read_RC (dat);
      uchar_result[0] = bit_read_RC (dat);
      uchar_result[1] = bit_read_RC (dat);
      uchar_result[2] = bit_read_RC (dat);
      uchar_result[3] = bit_read_RC (dat);
      CHK_OVERFLOW(__FUNCTION__, bit_nan ())
      return default_value;
    }
  else /* if (two_bit_code == 1) */
    {
      // first 4bits eq, only last 4
      uchar_result = (unsigned char *)&default_value;
      CHK_OVERFLOW_PLUS (4, __FUNCTION__, bit_nan ())
      uchar_result[0] = bit_read_RC (dat);
      uchar_result[1] = bit_read_RC (dat);
      uchar_result[2] = bit_read_RC (dat);
      uchar_result[3] = bit_read_RC (dat);
      CHK_OVERFLOW(__FUNCTION__, bit_nan ())
      return default_value;
    }
}

int
bit_eq_DD (double value, double default_value)
{
  return fabs (value - default_value) < 1e-12;
}

/** Write bit-double with default.
 */
BITCODE_BB
bit_write_DD (Bit_Chain *dat, double value, double default_value)
{
  BITCODE_BB bits = 0;

  if (fabs (value - default_value) < 1e-12)
    bit_write_BB (dat, 0);
  else
    {
      const unsigned char *uchar_value = (const unsigned char *)&value;
      const unsigned char *uchar_default = (const unsigned char *)&default_value;
      const uint16_t *uint_value = (const uint16_t *)&uchar_value;
      const uint16_t *uint_default = (const uint16_t *)&uchar_default;
      // dbl: 7654 3210, little-endian only
      // check the first 2 bits for eq
      if (uint_value[0] == uint_default[0])
        {
          // first 4 bits eq, i.e. next 2 bits also
          if (uint_value[1] == uint_default[1])
            {
              bits = 1;
              bit_write_BB (dat, 1);
              bit_write_RC (dat, uchar_value[0]);
              bit_write_RC (dat, uchar_value[1]);
              bit_write_RC (dat, uchar_value[2]);
              bit_write_RC (dat, uchar_value[3]);
            }
          else
            {
              bits = 2;
              bit_write_BB (dat, 2);
              bit_write_RC (dat, uchar_value[4]);
              bit_write_RC (dat, uchar_value[5]);
              bit_write_RC (dat, uchar_value[0]);
              bit_write_RC (dat, uchar_value[1]);
              bit_write_RC (dat, uchar_value[2]);
              bit_write_RC (dat, uchar_value[3]);
            }
        }
      else
        {
          bits = 3;
          bit_write_BB (dat, 3);
          bit_write_RD (dat, value);
        }
    }
  return bits;
}

/** Read bit-thickness.
 */
BITCODE_BT
bit_read_BT (Bit_Chain *dat)
{
  int mode = 0;

  if (dat->from_version >= R_2000)
    mode = bit_read_B (dat);

  return (mode ? 0.0 : bit_read_BD (dat));
}

/** Write bit-thickness.
 */
void
bit_write_BT (Bit_Chain *dat, double value)
{
  if (dat->version >= R_2000)
    {
      if (value == 0.0)
        bit_write_B (dat, 1);
      else
        {
          bit_write_B (dat, 0);
          bit_write_BD (dat, value);
        }
    }
  else
    bit_write_BD (dat, value);
}

/** Read handle-references. Returns error code: DWG_ERR_INVALIDHANDLE
 *  or 0 on success.
 */
int
bit_read_H (Bit_Chain *restrict dat, Dwg_Handle *restrict handle)
{
  unsigned long pos = dat->byte;
  handle->code = bit_read_RC (dat);
  if (pos == dat->byte)
    return DWG_ERR_INVALIDHANDLE;
  handle->is_global = 0;
  handle->value = 0;
  if (dat->from_version < R_13)
    {
      BITCODE_RC *restrict val;
      if (pos == dat->byte)
        return DWG_ERR_INVALIDHANDLE;
      handle->size = handle->code;
      if (handle->size > sizeof(BITCODE_RC *))
        {
          loglevel = dat->opts & DWG_OPTS_LOGLEVEL;
          LOG_WARN ("Invalid handle-reference, longer than 8 bytes: " FORMAT_H,
                    ARGS_H (*handle));
          return DWG_ERR_INVALIDHANDLE;
        }
      handle->code = 0;
      val = (BITCODE_RC *)&(handle->value);
      for (int i = handle->size - 1; i >= 0; i--)
        val[i] = bit_read_RC (dat);
      return 0;
    }
  handle->size = handle->code & 0xf;
  handle->code = (handle->code & 0xf0) >> 4;

  // size must not exceed 8
  if (handle->size > sizeof(BITCODE_RC *) || handle->code > 14)
    {
      loglevel = dat->opts & DWG_OPTS_LOGLEVEL;
      LOG_WARN ("Invalid handle-reference, longer than 8 bytes: " FORMAT_H,
                ARGS_H (*handle));
      return DWG_ERR_INVALIDHANDLE;
    }

  // TODO: little-endian only
  // x86_64 gcc-9.[0-2] miscompilation with val[i]: (%rbx) being dat+1
  // we work aorund this one, but you never know what else is being miscompiled.
  // apparently fixed in gcc-9.3, but 9.3 is still broken for cperl.
#if defined(__GNUC__) && (__GNUC__ == 9) && (__GNUC_MINOR__ <= 2) \
  && (SIZEOF_SIZE_T == 8)                                         \
  && (defined(__x86_64__) || defined(__x86_64) || defined(_M_AMD64) || defined(_M_X64))
  {
#warning x86_64 gcc-9.[0-2] codegen is seriously broken. better use 9.3 or an older version
    BITCODE_RC *restrict val;
    val = (BITCODE_RC *)&(handle->value);
    for (int i = handle->size - 1; i >= 0; i--)
      {
        BITCODE_RC c = bit_read_RC (dat);
        val[i] = c;
      }
  }
#else
  {
    BITCODE_RC *restrict val;
    val = (BITCODE_RC *)&(handle->value);
    for (int i = handle->size - 1; i >= 0; i--)
      val[i] = bit_read_RC (dat);
  }
#endif

  return 0;
}

/** Write handle-references.
 *  TODO
 *  seperate SoftPtr:   BB 0 + RLL
 *           HardPtr:   BB 1 + RLL
 *           SoftOwner: BB 2 + RLL
 *           HardOwner: BB 3 + RLL
 */
void
bit_write_H (Bit_Chain *restrict dat, Dwg_Handle *restrict handle)
{
  int i;
  union {
    unsigned char p[8];
    unsigned long v;
  } val;
  unsigned char size;

  if (!handle)
    {
      bit_write_RC (dat, 0);
      return;
    }
  if (handle->value == 0)
    {
      bit_write_RC (dat, handle->code << 4);
      return;
    }

  assert(sizeof(val) <= 8);
  // TODO: little-endian only. support sizes <= 8, not just 4
  memset (&val, 0, sizeof(val));
  val.v = handle->value;
  for (i = sizeof(val) - 1; i >= 0; i--)
    if (val.p[i])
      break;

  size = handle->code << 4;
  size |= i + 1;
  bit_write_RC (dat, size);

  for (; i >= 0; i--)
    bit_write_RC (dat, val.p[i]);
}

/** Only read old 16bit CRC-numbers, without checking, only in order
 *  to go to the next byte, while skipping non-aligned bits.
 */
uint16_t
bit_read_CRC (Bit_Chain *dat)
{
  uint16_t result;
  loglevel = dat->opts & DWG_OPTS_LOGLEVEL;

  if (dat->bit > 0)
    {
      dat->byte++;
      dat->bit = 0;
    }
  result = bit_read_RS (dat);
  LOG_TRACE ("read CRC at %lu: %04X\n", dat->byte, result)

  return result;
}

/** Read and check old 16bit CRC.
 */
int
bit_check_CRC (Bit_Chain *dat, long unsigned int start_address, uint16_t seed)
{
  uint16_t calculated;
  uint16_t read;
  long size;
  loglevel = dat->opts & DWG_OPTS_LOGLEVEL;

  if (dat->bit > 0)
    {
      dat->byte++;
      dat->bit = 0;
    }

  if (start_address > dat->byte || dat->byte >= dat->size)
    {
      loglevel = dat->opts & DWG_OPTS_LOGLEVEL;
      LOG_ERROR ("%s buffer overflow at pos %lu-%lu, size %lu", __FUNCTION__,
                 start_address, dat->byte, dat->size)
      return 0;
    }
  size = dat->byte - start_address;
  calculated = bit_calc_CRC (seed, &dat->chain[start_address], size);
  read = bit_read_RS (dat);
  LOG_TRACE ("crc: %04X [RSx]\n", read);
  if (calculated == read)
    {
      LOG_HANDLE (" check_CRC %lu-%lu = %ld: %04X == %04X\n", start_address,
                  dat->byte - 2, size, calculated, read)
      return 1;
    }
  else
    {
      LOG_WARN ("check_CRC mismatch %lu-%lu = %ld: %04X <=> %04X\n",
                start_address, dat->byte - 2, size, calculated, read)
      return 0;
    }
}

/** Create and write old 16bit CRC.
 */
uint16_t
bit_write_CRC (Bit_Chain *dat, long unsigned int start_address, uint16_t seed)
{
  uint16_t crc;
  long size;
  loglevel = dat->opts & DWG_OPTS_LOGLEVEL;

  while (dat->bit > 0)
    bit_write_B (dat, 0);

  if (start_address > dat->byte || (dat->byte + 2) >= dat->size)
      bit_chain_alloc (dat);
  if (start_address > dat->byte || dat->byte >= dat->size)
    {
      loglevel = dat->opts & DWG_OPTS_LOGLEVEL;
      LOG_ERROR ("%s buffer overflow at pos %lu-%lu, size %lu", __FUNCTION__,
                 start_address, dat->byte, dat->size)
      return 0;
    }
  size = dat->byte - start_address;
  crc = bit_calc_CRC (seed, &dat->chain[start_address], size);

  LOG_TRACE ("write CRC %04X from %lu-%lu = %ld\n", crc, start_address,
             dat->byte, size);
  bit_write_RS (dat, crc);
  return crc;
}

uint16_t
bit_write_CRC_LE (Bit_Chain *dat, long unsigned int start_address, uint16_t seed)
{
  uint16_t crc;
  long size;

  while (dat->bit > 0)
    bit_write_B (dat, 0);

  if (start_address > dat->byte || (dat->byte + 2) >= dat->size)
      bit_chain_alloc (dat);
  if (start_address > dat->byte || dat->byte >= dat->size)
    {
      loglevel = dat->opts & DWG_OPTS_LOGLEVEL;
      LOG_ERROR ("%s buffer overflow at pos %lu-%lu, size %lu", __FUNCTION__,
                 start_address, dat->byte, dat->size)
      return 0;
    }
  size = dat->byte - start_address;
  crc = bit_calc_CRC (seed, &dat->chain[start_address], size);

  loglevel = dat->opts & DWG_OPTS_LOGLEVEL;
  LOG_TRACE ("write CRC %04X from %lu-%lu = %ld\n", crc, start_address,
             dat->byte, size);
  bit_write_RS_LE (dat, crc);
  return crc;
}

void
bit_read_fixed (Bit_Chain *restrict dat, BITCODE_RC *restrict dest,
                unsigned int length)
{
  if (dat->byte + length > dat->size)
    {
      loglevel = dat->opts & DWG_OPTS_LOGLEVEL;
      LOG_ERROR ("%s buffer overflow at pos %lu, size %lu", __FUNCTION__,
                 dat->byte, dat->size)
      memset (dest, 0, length);
      return;
    }
  if (dat->bit == 0)
    {
      memcpy (dest, &dat->chain[dat->byte], length);
      dat->byte += length;
    }
  else
    {
      for (unsigned int i = 0; i < length; i++)
        {
          dest[i] = bit_read_RC (dat);
        }
    }
}

/** Read fixed text with zero-termination.
 *  After usage, the allocated memory must be properly freed.
 *  preR11
 */
ATTRIBUTE_MALLOC
BITCODE_TF
bit_read_TF (Bit_Chain *restrict dat, unsigned int length)
{
  BITCODE_RC *chain;
  CHK_OVERFLOW_PLUS (length,__FUNCTION__,NULL)
  chain = (BITCODE_RC *)calloc (length + 1, 1);
  if (!chain)
    {
      loglevel = dat->opts & DWG_OPTS_LOGLEVEL;
      LOG_ERROR ("Out of memory");
      return NULL;
    }
  bit_read_fixed (dat, chain, length);
  chain[length] = '\0';

  return (BITCODE_TF)chain;
}

/** Read fixed text with zero-termination.
 *  After usage, the allocated memory must be properly freed.
 *  preR11
 */
ATTRIBUTE_MALLOC
BITCODE_TF
bit_read_bits (Bit_Chain *dat, unsigned long bits)
{
  unsigned bytes = bits / 8;
  int rest = bits % 8;
  BITCODE_RC *restrict chain;
  CHK_OVERFLOW_PLUS (bytes,__FUNCTION__,NULL)
  chain = (BITCODE_RC *)calloc (bytes + (rest ? 2 : 1), 1);
  if (!chain)
    {
      loglevel = dat->opts & DWG_OPTS_LOGLEVEL;
      LOG_ERROR ("Out of memory");
      return NULL;
    }

  bit_read_fixed (dat, chain, bytes);
  chain[bytes] = '\0';
  if (rest)
    {
      // protect against the last bit_advance error
      dat->size++;
      chain[bytes + 1] = '\0';
      for (int i = 0; i < rest; i++)
        {
          BITCODE_RC last = bit_read_B (dat);
          chain[bytes] |= last << i;
        }
      dat->size--;
      // we are now in overflow state
    }
  return (BITCODE_TF)chain;
}

/** Write fixed-length text.
 */
void
bit_write_TF (Bit_Chain *restrict dat, BITCODE_TF restrict chain, unsigned int length)
{
  if (!chain)
    {
      loglevel = dat->opts & DWG_OPTS_LOGLEVEL;
      LOG_ERROR ("Empty TF with length %u", length);
      if (length <= 128) // either chain or length is wrong
        {
          for (unsigned int i = 0; i < length; i++)
            bit_write_RC (dat, 0);
        }
      return;
    }
  if (dat->bit == 0 && dat->byte + length < dat->size)
    {
      memcpy (&dat->chain[dat->byte], chain, length);
      dat->byte += length;
    }
  else
    {
      for (unsigned int i = 0; i < length; i++)
        bit_write_RC (dat, (BITCODE_RC)chain[i]);
    }
}

/** Read simple text. After usage, the allocated memory must be properly freed.
 */
ATTRIBUTE_MALLOC
BITCODE_TV
bit_read_TV (Bit_Chain *restrict dat)
{
  unsigned int i;
  unsigned int length;
  unsigned char *chain;

  CHK_OVERFLOW_PLUS (1,__FUNCTION__,NULL)
  if (dat->from_version < R_13)
    length = bit_read_RS (dat);
  else
    length = bit_read_BS (dat);
  CHK_OVERFLOW_PLUS (length,__FUNCTION__,NULL)
  chain = (unsigned char *)malloc (length + 1);
  if (!chain)
    {
      loglevel = dat->opts & DWG_OPTS_LOGLEVEL;
      LOG_ERROR ("Out of memory");
      return NULL;
    }
  for (i = 0; i < length; i++)
    chain[i] = bit_read_RC (dat);
  // check if the string is already zero-terminated or not.
  // only observed >=r2004 as writer app
  if (length > 0 && dat->from_version > R_2000 && chain[length - 1] != '\0')
    LOG_HANDLE ("TV-not-ZERO %u\n ", length)
  else if (length > 0 && dat->from_version <= R_2000 && chain[length - 1] == '\0')
    LOG_HANDLE ("TV-ZERO %u\n", length)
  // normally not needed, as the DWG since r2004 itself contains the ending \0 as last char
  chain[i] = '\0';
  return (char *)chain;
}

// Usage: hex(c >> 4), hex(c & 0xf)
static int heX (unsigned char c)
{
  c &= 0xf; // 0-15
  return c >= 10 ? 'A' + c - 10 : '0' + c;
}

/* Converts TU to ASCII with embedded \U+XXXX.
   Max len widechars.
   No codepage support yet, only cp 30.
 */
ATTRIBUTE_MALLOC
char *
bit_embed_TU_size (BITCODE_TU restrict wstr, const int len)
{
  char *str;
  int read, write, size;
  uint16_t c = 0;

  if (!wstr)
    return NULL;
  size = len + 1;
  str = (char*)malloc (size);
  if (!str)
    return NULL;
  read = write = 0;
  while (read < len)
    {
#ifdef HAVE_ALIGNED_ACCESS_REQUIRED
      // for strict alignment CPU's like sparc only. also for UBSAN.
      if ((uintptr_t)wstr % SIZEOF_SIZE_T)
        {
          unsigned char *b = (unsigned char *)wstr;
          c = TU_to_int(b);
          wstr++;
        }
      else
#endif
      c = *wstr++;
      read++;
      if (c < 256)
        {
          if (write + 1 >= size) // TODO should not happen
            {
              size += 2;
              str = (char*)realloc (str, size);
            }
          str[write++] = c & 0xFF;
        }
      else
        {
          if (write + 7 > size)
            {
              size += 8;
              str = (char*)realloc (str, size);
            }
          str[write++] = '\\';
          str[write++] = 'U';
          str[write++] = '+';
          str[write++] = heX (c >> 12);
          str[write++] = heX (c >> 8);
          str[write++] = heX (c >> 4);
          str[write++] = heX (c);
        }
    }
  str[write] = '\0';
  return str;
}

#if !(defined (HAVE_NATIVE_WCHAR2) && defined (HAVE_WCSNLEN))

/* len of wide string (unix-only) */
size_t
bit_wcs2nlen (const BITCODE_TU restrict wstr, const size_t maxlen)
{
  size_t len;

  if (!wstr)
    return 0;
  len = 0;
#  ifdef HAVE_ALIGNED_ACCESS_REQUIRED
  // for strict alignment CPU's like sparc only. also for UBSAN.
  if ((uintptr_t)wstr % SIZEOF_SIZE_T)
    {
      unsigned char *b = (unsigned char *)wstr;
      uint16_t c = TU_to_int(b);
      while (c)
        {
          len++;
          if (len >= maxlen)
            return 0;
          b += 2;
          c = TU_to_int(b);
        }
      return len;
    }
  else
#  endif
  {
    BITCODE_TU c = wstr;
    while (*c)
      {
        len++;
        if (len >= maxlen)
          return 0;
        c++;
      }
    return len;
  }
}

#endif
#ifndef HAVE_NATIVE_WCHAR2

/* len of wide string (unix-only) */
size_t
bit_wcs2len (const BITCODE_TU restrict wstr)
{
  size_t len;

  if (!wstr)
    return 0;
  len = 0;
#  ifdef HAVE_ALIGNED_ACCESS_REQUIRED
  // for strict alignment CPU's like sparc only. also for UBSAN.
  if ((uintptr_t)wstr % SIZEOF_SIZE_T)
    {
      unsigned char *b = (unsigned char *)wstr;
      uint16_t c = TU_to_int(b);
      while (c)
        {
          len++;
          b += 2;
          c = TU_to_int(b);
        }
      return len;
    }
  else
#  endif
  {
    BITCODE_TU c = wstr;
    while (*c++)
      {
        len++;
      }
    return len;
  }
}

/* copy wide string (unix-only) */
BITCODE_TU
bit_wcs2cpy (BITCODE_TU restrict dest, const BITCODE_TU restrict src)
{
  BITCODE_TU d;

  if (!dest)
    return src;
  d = (BITCODE_TU)dest;
#  ifdef HAVE_ALIGNED_ACCESS_REQUIRED
  // for strict alignment CPU's like sparc only. also for UBSAN.
  if ((uintptr_t)src % SIZEOF_SIZE_T)
    {
      unsigned char *b = (unsigned char *)src;
      *d = TU_to_int(b);
      while (*d)
        {
          b += 2;
          *d = TU_to_int(b);
        }
      return dest;
    }
  else
#  endif
  {
    BITCODE_TU s = (BITCODE_TU)src;
    while ((*d++ = *s++))
      ;
    return dest;
  }
}

#if 0
/* compare wide string (unix-only). returns 0 if the same or 1 if not */
// untested, unused
int
bit_wcs2cmp (BITCODE_TU restrict dest, const BITCODE_TU restrict src)
{
  BITCODE_TU d;

  if (!dest)
    return -1;
  d = (BITCODE_TU)dest;
#  ifdef HAVE_ALIGNED_ACCESS_REQUIRED
  // for strict alignment CPU's like sparc only. also for UBSAN.
  if ((uintptr_t)src % SIZEOF_SIZE_T)
    {
      unsigned char *s = (unsigned char *)src;
      uint16_t s1 = TU_to_int(s);
      while (*d++ == s1)
        {
          s += 2;
          s1 = TU_to_int(s);
        }
      return (*d || *s1) ? 1 : 0;
    }
  else
#  endif
  {
    BITCODE_TU s = (BITCODE_TU)src;
    while ((*d++ == *s++))
      ;
    return (*d || *s) ? 1 : 0;
  }
}
#endif

#endif /* HAVE_NATIVE_WCHAR2 */

#ifndef HAVE_STRNLEN
/* bounds-checked len of string */
size_t
bit_strnlen (const char *restrict str, const size_t maxlen)
{
  size_t len;
  char *c = str;

  if (!str)
    return 0;
  len = 0;
  while (*c)
    {
      len++;
      if (len >= maxlen)
        return 0;
      c++;
    }
  return len;
}
#endif

/* converts TU to ASCII with embedded \U+XXXX */
char *
bit_embed_TU (BITCODE_TU restrict wstr)
{
  BITCODE_TU tmp = wstr;
  int len = 0;
  uint16_t c = 0;

  if (!wstr)
    return NULL;
  while ((c = *tmp++))
    len++;
  return bit_embed_TU_size (wstr, len);
}

/** Write ASCIIZ text.
    Starting with r2004 as writer (not target version) acad always
    writes a terminating zero.
 */
void
bit_write_TV (Bit_Chain *restrict dat, BITCODE_TV restrict chain)
{
  int i;
  int length = (chain && *chain) ? strlen ((const char *)chain) : 0;
  if (dat->version <= R_2000 && length)
    length++;
  if (dat->from_version < R_13)
    bit_write_RS (dat, length);
  else
    bit_write_BS (dat, length);
  for (i = 0; i < length; i++)
    bit_write_RC (dat, (unsigned char)chain[i]);
}

static int ishex (int c)
{
  return ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f')
          || (c >= 'A' && c <= 'F'));
}

/** Write ASCII or Unicode text.
 */
void
bit_write_T (Bit_Chain *restrict dat, BITCODE_T restrict s)
{
  int i, length;

  // only if from r2007+ DWG. not JSON, DXF, add API.
  if (IS_FROM_TU (dat))
    {
      // downconvert TU to TV
      if (dat->version < R_2007)
        {
          if (!s)
            bit_write_BS (dat, 0);
          else
            {
              BITCODE_TV str = bit_embed_TU ((BITCODE_TU)s);
              length = strlen ((const char *)str) + 1;
              bit_write_BS (dat, length);
              for (i = 0; i < length; i++)
                bit_write_RC (dat, (unsigned char)str[i]);
              if (str) free (str);
            }
        }
      else
        {
          bit_write_TU (dat, (BITCODE_TU)s);
        }
    }
  else
    {
      // convert TV to TU. parse \U+xxxx to wchars
      if (dat->version >= R_2007)
        {
          if (!s)
            {
              bit_write_BS (dat, 0);
            }
          else
            {
              uint16_t c;
              BITCODE_TU ws = (BITCODE_TU)malloc ((strlen (s) + 1) * 2);
              BITCODE_TU orig = ws;
              while ((c = *s++))
                {
                  if (c == '\\' && s[0] == 'U' && s[1] == '+' && ishex (s[2])
                      && ishex (s[3]) && ishex (s[4]) && ishex (s[5]))
                    {
                      unsigned x;
                      if (sscanf (&s[2], "%04X", &x) > 0)
                        {
                          *ws++ = x;
                          s += 6;
                        }
                      else
                        *ws++ = c;
                    }
                  else
                    *ws++ = c;
                }
              *ws = 0;
              bit_write_TU (dat, orig);
              free (orig);
            }
        }
      else
        bit_write_TV (dat, s);
    }
}

/** Read UCS-2 unicode text. no supplementary planes
 *  See also bfr_read_string()
 */
BITCODE_TU
bit_read_TU (Bit_Chain *restrict dat)
{
  unsigned int i;
  unsigned int length;
  BITCODE_TU chain;

  CHK_OVERFLOW_PLUS (1,__FUNCTION__,NULL)
  length = bit_read_BS (dat);
  CHK_OVERFLOW_PLUS (length * 2,__FUNCTION__,NULL)
  chain = (BITCODE_TU)malloc ((length + 1) * 2);
  if (!chain)
    {
      loglevel = dat->opts & DWG_OPTS_LOGLEVEL;
      LOG_ERROR ("Out of memory")
      return NULL;
    }
  for (i = 0; i < length; i++)
    chain[i] = bit_read_RS (dat); // probably without byte swapping
  // normally not needed, as the DWG itself contains the ending 0 as last char
  // but we enforce writing it.
  chain[length] = 0;
  return chain;
}

BITCODE_TU
bit_read_TU_len (Bit_Chain *restrict dat, unsigned int *lenp)
{
  unsigned int i;
  unsigned int length;
  BITCODE_TU chain;

  CHK_OVERFLOW_PLUS (1,__FUNCTION__,NULL)
  length = bit_read_BS (dat);
  CHK_OVERFLOW_PLUS (length * 2,__FUNCTION__,NULL)
  chain = (BITCODE_TU)malloc ((length + 1) * 2);
  if (!chain)
    {
      loglevel = dat->opts & DWG_OPTS_LOGLEVEL;
      LOG_ERROR ("Out of memory")
      return NULL;
    }
  for (i = 0; i < length; i++)
    chain[i] = bit_read_RS (dat); // probably without byte swapping
  // normally not needed, as the DWG itself contains the ending 0 as last char
  // but we enforce writing it.
  chain[length] = 0;
  *lenp = length;
  return chain;
}

/** String16: Read ASCII prefixed by a RS length
 */
BITCODE_TV
bit_read_T16 (Bit_Chain *restrict dat)
{
  BITCODE_RS i, length;
  BITCODE_TV chain;

  CHK_OVERFLOW(__FUNCTION__,NULL)
  length = bit_read_RS (dat);
  CHK_OVERFLOW_PLUS (length,__FUNCTION__,NULL)
  chain = (BITCODE_TV)malloc (length + 1);
  if (!chain)
    {
      loglevel = dat->opts & DWG_OPTS_LOGLEVEL;
      LOG_ERROR ("Out of memory")
      return NULL;
    }
  for (i = 0; i < length; i++)
    chain[i] = bit_read_RC (dat);
  chain[length] = 0;
  return chain;
}

/** String16: Read Unicode prefixed by a RS length
 */
BITCODE_TU
bit_read_TU16 (Bit_Chain *restrict dat)
{
  BITCODE_RS i, length;
  BITCODE_TU chain;

  CHK_OVERFLOW_PLUS (2,__FUNCTION__,NULL)
  length = bit_read_RS (dat);
  CHK_OVERFLOW_PLUS (length * 2,__FUNCTION__,NULL)
  chain = (BITCODE_TU)malloc ((length + 1) * 2);
  if (!chain)
    {
      loglevel = dat->opts & DWG_OPTS_LOGLEVEL;
      LOG_ERROR ("Out of memory")
      return NULL;
    }
  for (i = 0; i < length; i++)
    chain[i] = bit_read_RS (dat);
  chain[length] = 0;
  return chain;
}

/** String32: Read ASCII/UCS-2 string prefixed by a RL size (not length)
 */
BITCODE_T32
bit_read_T32 (Bit_Chain *restrict dat)
{
  BITCODE_RL i, size;

  size = bit_read_RL (dat);
  // only if from r2007+ DWG, not JSON, DXF
  if (IS_FROM_TU (dat))
    {
      BITCODE_TU wstr;
      BITCODE_RL len = size / 2;
      if (dat->byte + size >= dat->size)
        {
          loglevel = dat->opts & DWG_OPTS_LOGLEVEL;
          LOG_ERROR ("%s buffer overflow at %lu, size %u", __FUNCTION__,
                     dat->byte, size)
            return NULL;
        }
      wstr = (BITCODE_TU)malloc (size + 2);
      if (!wstr)
        {
          loglevel = dat->opts & DWG_OPTS_LOGLEVEL;
          LOG_ERROR ("Out of memory")
            return NULL;
        }
      for (i = 0; i < len; i++)
        wstr[i] = bit_read_RS (dat);
      wstr[len] = 0;
      return (BITCODE_T32)wstr;
    }
  else
    {
      BITCODE_T32 str;
      if (dat->byte + size >= dat->size)
        {
          loglevel = dat->opts & DWG_OPTS_LOGLEVEL;
          LOG_ERROR ("%s buffer overflow at %lu, size %u", __FUNCTION__,
                     dat->byte, size)
            return NULL;
        }
      str = (BITCODE_T32)malloc (size + 1);
      if (!str)
        {
          loglevel = dat->opts & DWG_OPTS_LOGLEVEL;
          LOG_ERROR ("Out of memory")
            return NULL;
        }
      for (i = 0; i < size; i++)
        str[i] = bit_read_RC (dat);
      str[size] = 0;
      return str;
    }
}

/** String32: Read ASCII/UCS-4 or -2 string prefixed by a RL size (not length)
 */
BITCODE_TU32
bit_read_TU32 (Bit_Chain *restrict dat)
{
  BITCODE_RL i, size;

  size = bit_read_RL (dat);
  // only if from r2007+ DWG, not JSON, DXF
  if (IS_FROM_TU (dat))
    {
      BITCODE_TU wstr;
      BITCODE_RL rl1, len = size / 4;
      unsigned long pos = bit_position (dat);
      if (dat->byte + size >= dat->size)
        {
          loglevel = dat->opts & DWG_OPTS_LOGLEVEL;
          LOG_ERROR ("%s buffer overflow at %lu, size %u", __FUNCTION__,
                     dat->byte, size)
          return NULL;
        }
      wstr = (BITCODE_TU)malloc (size + 2);
      if (!wstr)
        {
          loglevel = dat->opts & DWG_OPTS_LOGLEVEL;
          LOG_ERROR ("Out of memory")
            return NULL;
        }
      rl1 = bit_read_RL (dat);
      if (rl1 & 0x00ff0000) /* 00 xx 00 nn */
        { // only UCS-2
          bit_set_position (dat, pos);
          len = size / 2;
          LOG_HANDLE ("TU32 is only UCS-2\n");
          for (i = 0; i < len; i++)
            {
              wstr[i] = bit_read_RS (dat);
            }
        }
      else
        {
          wstr[0] = (BITCODE_RS)rl1;
          for (i = 1; i < len; i++)
            {
              wstr[i] = (BITCODE_RS)bit_read_RL (dat);
            }
        }
      wstr[len] = 0;
      return (BITCODE_T32)wstr;
    }
  else
    {
      BITCODE_T32 str;
      if (dat->byte + size >= dat->size)
        {
          loglevel = dat->opts & DWG_OPTS_LOGLEVEL;
          LOG_ERROR ("%s buffer overflow at %lu, size %u", __FUNCTION__,
                     dat->byte, size)
            return NULL;
        }
      str = (BITCODE_T32)malloc (size + 1);
      if (!str)
        {
          loglevel = dat->opts & DWG_OPTS_LOGLEVEL;
          LOG_ERROR ("Out of memory")
            return NULL;
        }
      for (i = 0; i < size; i++)
        str[i] = bit_read_RC (dat);
      str[size] = 0;
      return str;
    }
}

/** Write UCS-2 unicode text. Must be zero-delimited.
 */
void
bit_write_TU (Bit_Chain *restrict dat, BITCODE_TU restrict chain)
{
  unsigned int i;
  unsigned int length;

  if (chain)
    length = bit_wcs2len (chain) + 1;
  else
    length = 0;

  bit_write_BS (dat, length);
  for (i = 0; i < length; i++)
    bit_write_RS (dat, chain[i]); // probably without byte swapping
}

// Unicode with RS length
void bit_write_TU16 (Bit_Chain *restrict dat, BITCODE_TU restrict chain)
{
  unsigned int i;
  unsigned int length;

  if (chain)
    length = bit_wcs2len (chain) + 1;
  else
    length = 0;

  bit_write_RS (dat, length);
  for (i = 0; i < length; i++)
    bit_write_RS (dat, chain[i]);
}
// ASCII/UCS-2 string with a RL size (not length)
void bit_write_T32 (Bit_Chain *restrict dat, BITCODE_T32 restrict chain)
{
  unsigned int i;
  unsigned int length;

  if (dat->version >= R_2007)
    {
      if (chain)
        length = bit_wcs2len ((BITCODE_TU)chain) + 1;
      else
        length = 0;
      bit_write_RL (dat, length * 2);
      for (i = 0; i < length; i++)
        bit_write_RS (dat, chain[i]);
    }
  else
    {
      if (chain)
        length = strlen (chain) + 1;
      else
        length = 0;
      bit_write_RL (dat, length);
      for (i = 0; i < length; i++)
        bit_write_RC (dat, chain[i]);
    }
}
// ASCII/UCS-4 or -2 string with a RL size (not length)
void bit_write_TU32 (Bit_Chain *restrict dat, BITCODE_TU32 restrict chain)
{
  unsigned int i;
  unsigned int length;

  if (dat->version >= R_2007)
    {
      if (chain)
        length = bit_wcs2len ((BITCODE_TU)chain) + 1;
      else
        length = 0;
      bit_write_RL (dat, length * 4);
      for (i = 0; i < length; i++)
        bit_write_RL (dat, chain[i]);
    }
  else
    {
      if (chain)
        length = strlen (chain) + 1;
      else
        length = 0;
      bit_write_RL (dat, length);
      for (i = 0; i < length; i++)
        bit_write_RC (dat, chain[i]);
    }
}

BITCODE_T
bit_read_T (Bit_Chain *restrict dat)
{
  if (IS_FROM_TU (dat))
    return (BITCODE_T)bit_read_TU (dat);
  else
    return (BITCODE_T)bit_read_TV (dat);
}

/* converts UCS-2LE to UTF-8.
   first pass to get the dest len. single malloc.
 */
char *
bit_convert_TU (const BITCODE_TU restrict wstr)
{
  BITCODE_TU tmp = wstr;
  char *str;
  int i, len = 0;
  uint16_t c = 0;

  if (!wstr)
    return NULL;
#ifdef HAVE_ALIGNED_ACCESS_REQUIRED
  /* access bytewise. cannot copy to heap yet, because we have no length */
  if ((uintptr_t)wstr % SIZEOF_SIZE_T)
    {
      unsigned char *b = (unsigned char*)wstr;
      c = TU_to_int(b);
      while (c)
        {
          len++;
          if (c >= 0x80)
            {
              len++;
              if (c >= 0x800)
                len++;
            }
          b += 2;
          c = TU_to_int(b);
        }
    }
  else
#endif
  while ((c = *tmp++))
    {
      len++;
      if (c >= 0x80)
        {
          len++;
          if (c >= 0x800)
            len++;
        }
#if 0
        loglevel = 5;
        LOG_INSANE ("U+%04X ", c);
#endif
    }
  str = (char*)malloc (len + 1);
  if (!str)
    {
      loglevel = 1;
      LOG_ERROR ("Out of memory")
      return NULL;
    }
  i = 0;
  tmp = wstr;
#ifdef HAVE_ALIGNED_ACCESS_REQUIRED
  if ((uintptr_t)wstr % SIZEOF_SIZE_T)
    {
      unsigned char *b = (unsigned char*)wstr;
      // possible gcc bug
      c = TU_to_int(b);
      while (c && i < len)
        {
          if (c < 0x80)
            {
              str[i++] = c & 0xFF;
            }
          else if (c < 0x800)
            {
              str[i++] = (c >> 6) | 0xC0;
              str[i++] = (c & 0x3F) | 0x80;
            }
          else /* if (c < 0x10000) */
            {
              str[i++] = (c >> 12) | 0xE0;
              str[i++] = ((c >> 6) & 0x3F) | 0x80;
              str[i++] = (c & 0x3F) | 0x80;
            }

          b += 2;
          c = TU_to_int(b);
        }
    }
  else
#endif
  while ((c = *tmp++) && i < len)
    {
      if (c < 0x80)
        {
          str[i++] = c & 0xFF;
        }
      else if (c < 0x800)
        {
          str[i++] = (c >> 6) | 0xC0;
          str[i++] = (c & 0x3F) | 0x80;
        }
      else /* if (c < 0x10000) */
        { /* windows ucs-2 has no D800-DC00 surrogate pairs. go straight up */
          /*if (i+3 > len) {
            str = realloc(str, i+3);
            len = i+2;
          }*/
          str[i++] = (c >> 12) | 0xE0;
          str[i++] = ((c >> 6) & 0x3F) | 0x80;
          str[i++] = (c & 0x3F) | 0x80;
        }
      /*
      else if (c < 0x110000)
        {
          str[i++] = (c >> 18) | 0xF0;
          str[i++] = ((c >> 12) & 0x3F) | 0x80;
          str[i++] = ((c >> 6) & 0x3F) | 0x80;
          str[i++] = (c & 0x3F) | 0x80;
        }
      else
        HANDLER (OUTPUT, "ERROR: overlarge unicode codepoint U+%0X", c);
     */
    }
  if (i <= len + 1)
    str[i] = '\0';
  return str;
}

#define EXTEND_SIZE(str, i, len)                \
  if (i > len)                                  \
    str = (char*) realloc (str, i+1)

/* converts UCS-2LE to UTF-8. len is the wstr length, not the resulting utf8-size.
   single pass with realloc. */
char *
bit_TU_to_utf8_len (const BITCODE_TU restrict wstr, const int len)
{
  BITCODE_TU tmp = wstr;
  char *str;
  int i = 0;
  uint16_t c = 0;

  if (!wstr || !len)
    return NULL;
  str = (char*)malloc (len + 1);
  if (!str)
    {
      loglevel = 1;
      LOG_ERROR ("Out of memory")
      return NULL;
    }
  i = 0;
  tmp = wstr;
#ifdef HAVE_ALIGNED_ACCESS_REQUIRED
  if ((uintptr_t)wstr % SIZEOF_SIZE_T)
    {
      unsigned char *b = (unsigned char*)wstr;
      // possible gcc bug
      c = TU_to_int(b);
      while (c && i < len)
        {
          if (c < 0x80)
            {
              str[i++] = c & 0xFF;
            }
          else if (c < 0x800)
            {
              EXTEND_SIZE(str, i + 1, len);
              str[i++] = (c >> 6) | 0xC0;
              str[i++] = (c & 0x3F) | 0x80;
            }
          else /* if (c < 0x10000) */
            {
              EXTEND_SIZE(str, i + 2, len);
              str[i++] = (c >> 12) | 0xE0;
              str[i++] = ((c >> 6) & 0x3F) | 0x80;
              str[i++] = (c & 0x3F) | 0x80;
            }

          b += 2;
          c = TU_to_int(b);
        }
    }
  else
#endif
  while ((c = *tmp++) && i < len)
    {
      if (c < 0x80)
        {
          str[i++] = c & 0xFF;
        }
      else if (c < 0x800)
        {
          EXTEND_SIZE(str, i + 1, len);
          str[i++] = (c >> 6) | 0xC0;
          str[i++] = (c & 0x3F) | 0x80;
        }
      else /* if (c < 0x10000) */
        { /* windows ucs-2 has no D800-DC00 surrogate pairs. go straight up */
          /*if (i+3 > len) {
            str = realloc(str, i+3);
            len = i+2;
          }*/
          EXTEND_SIZE(str, i + 2, len);
          str[i++] = (c >> 12) | 0xE0;
          str[i++] = ((c >> 6) & 0x3F) | 0x80;
          str[i++] = (c & 0x3F) | 0x80;
        }
      /*
      else if (c < 0x110000)
        {
          EXTEND_SIZE(str, i + 3, len);
          str[i++] = (c >> 18) | 0xF0;
          str[i++] = ((c >> 12) & 0x3F) | 0x80;
          str[i++] = ((c >> 6) & 0x3F) | 0x80;
          str[i++] = (c & 0x3F) | 0x80;
        }
      else
        HANDLER (OUTPUT, "ERROR: overlarge unicode codepoint U+%0X", c);
     */
    }
  EXTEND_SIZE(str, i + 1, len);
  str[i] = '\0';
  return str;
}

/** converts UTF-8 (dxf,json) to ASCII TV.
    Unquotes \" to ", \\ to \,
    undo json_cquote(), \uxxxx or other unicode => \U+XXXX.
    Returns NULL if not enough room in dest.
*/
char *
bit_utf8_to_TV (char *restrict dest, const unsigned char *restrict src,
                const int destlen, const int srclen, const unsigned cquoted)
{
  unsigned char c;
  unsigned char *s = (unsigned char *)src;
  const char* endp = dest + destlen;
  const unsigned char* ends = src + srclen;
  char *d = dest;

  while ((c = *s++))
    {
      if (dest >= endp)
        return NULL;
      else if (cquoted && c == '\\' && dest+1 < endp && s + 1 <= ends &&
          // skip \" to " and \\ to \.
          (*s == '"' || *s == '\\' || *s == 'r' || *s == 'n'))
        {
            if (*s == 'r')
              {
                *dest++ = '\r';
                s++;
              }
            else if (*s == 'n')
              {
                *dest++ = '\n';
                s++;
              }
        }
      // \uxxxx => \U+XXXX as in bit_embed_TU
      else if (c == '\\' && dest+7 < endp && *s == 'u' && s + 5 <= ends)
        {
          *dest++ = c;
          *dest++ = 'U';
          *dest++ = '+';
          s++;
          *dest++ = *s++;
          *dest++ = *s++;
          *dest++ = *s++;
          *dest++ = *s++;
        }
      else if (c < 128)
        {
          *dest++ = c;
        }
      else if ((c & 0xe0) == 0xc0 && s + 1 <= ends)
        {
          /* ignore invalid utf8 for now */
          if (dest+7 < endp)
            {
              BITCODE_RS wc = ((c & 0x1f) << 6) | (*s & 0x3f);
              *dest++ = '\\';
              *dest++ = 'U';
              *dest++ = '+';
              *dest++ = heX (wc >> 12);
              *dest++ = heX (wc >> 8);
              *dest++ = heX (wc >> 4);
              *dest++ = heX (wc);
              s++;
            }
          else
            return NULL;
        }
      else if ((c & 0xf0) == 0xe0)
        {
          /* warn on invalid utf8 */
          if (dest+2 < endp && s + 1 <= ends &&
              (*s < 0x80 || *s > 0xBF || *(s+1) < 0x80 || *(s+1) > 0xBF))
            {
              LOG_WARN ("utf-8: BAD_CONTINUATION_BYTE %s", s);
            }
          if (dest+1 < endp && c == 0xe0 && *s < 0xa0)
            {
              LOG_WARN ("utf-8: NON_SHORTEST %s", s);
            }
          if (dest+7 < endp && s + 1 <= ends)
            {
              BITCODE_RS wc = ((c & 0x0f) << 12) | ((*s & 0x3f) << 6) | (*(s+1) & 0x3f);
              *dest++ = '\\';
              *dest++ = 'U';
              *dest++ = '+';
              *dest++ = heX (wc >> 12);
              *dest++ = heX (wc >> 8);
              *dest++ = heX (wc >> 4);
              *dest++ = heX (wc);
            }
          else
            return NULL;
          if (s + 2 > ends)
            break;
          s++;
          s++;
        }
      if (s >= ends)
        break;
      /* everything above 0xf0 exceeds ucs-2, 4-6 byte seqs */
    }

  if (dest >= endp)
    return NULL;
  else
    *dest = '\0';
  return d;
}

/** converts UTF-8 to UCS-2. Returns a copy.
    TODO: unquote json_cquote as above.
 */
ATTRIBUTE_MALLOC
BITCODE_TU
bit_utf8_to_TU (char *restrict str, const unsigned cquoted)
{
  BITCODE_TU wstr;
  int i = 0;
  int len = strlen (str);
  unsigned char c;

  wstr = (BITCODE_TU)malloc (2 * (len + 1));
  if (!wstr)
    {
      loglevel = 1;
      LOG_ERROR ("Out of memory")
      return NULL;
    }
  while (len >= 0 && (c = *str++))
    {
      len--;
      if (c < 128)
        {
          wstr[i++] = c;
        }
      else if ((c & 0xe0) == 0xc0)
        {
          /* ignore invalid utf8 for now */
          if (len >= 1)
            wstr[i++] = ((c & 0x1f) << 6) | (str[1] & 0x3f);
          len--;
          str++;
        }
      else if ((c & 0xf0) == 0xe0)
        {
          /* ignore invalid utf8? */
          if (len >= 2 &&
              ((unsigned char)str[1] < 0x80 || (unsigned char)str[1] > 0xBF
               || (unsigned char)str[2] < 0x80 || (unsigned char)str[2] > 0xBF))
            {
              LOG_WARN ("utf-8: BAD_CONTINUATION_BYTE %s", str);
            }
          if (len >= 1 && c == 0xe0 && (unsigned char)str[1] < 0xa0)
            {
              LOG_WARN ("utf-8: NON_SHORTEST %s", str);
            }
          if (len >= 2)
            wstr[i++]
              = ((c & 0x0f) << 12) | ((str[1] & 0x3f) << 6) | (str[2] & 0x3f);
          str++;
          str++;
          len--;
          len--;
        }
      /* everything above 0xf0 exceeds ucs-2, 4-6 byte seqs */
    }
  wstr[i] = '\0';
  return wstr;
}

/* compare an ASCII/TU string to ASCII name */
int
bit_eq_T (Bit_Chain *restrict dat, const BITCODE_T restrict wstr1, const char *restrict str2)
{
  if (!wstr1 || !str2)
    return str2 == wstr1;
  if (IS_FROM_TU (dat))
    return bit_eq_TU (str2, (BITCODE_TU)wstr1);
  else
    return !strcmp (wstr1, str2);
}

/* compare an ASCII/utf-8 string to a r2007+ name */
int
bit_eq_TU (const char *restrict str, BITCODE_TU restrict wstr)
{
  char *utf8;
  int result;
  if (!str)
    return (wstr && *wstr) ? 0 : 1;
  utf8 = bit_convert_TU (wstr);
  result = utf8 ? (strcmp (str, utf8) ? 0 : 1) : 0;
  free (utf8);
  return result;
}

/* check if the string (ascii or unicode) is NULL or empty */
int bit_empty_T (Bit_Chain *restrict dat, BITCODE_T restrict str)
{
  if (!str)
    return 1;
  // importer hack: in_json/dxf still write all strings as TV
  if (!(IS_FROM_TU (dat)))
    return !*str;
  else
    {
      uint16_t c;
#ifdef HAVE_ALIGNED_ACCESS_REQUIRED
      if ((uintptr_t)str % SIZEOF_SIZE_T)
        {
          unsigned char *b = (unsigned char*)str;
          c = TU_to_int(b);
          return !c;
        }
#endif
      c = *(BITCODE_TU)str;
      return !c;
    }
}

BITCODE_T
bit_set_T (Bit_Chain *dat, const char* restrict src)
{
  if (!(IS_FROM_TU (dat)))
    return strdup (src);
  else
    return (BITCODE_T)bit_utf8_to_TU ((char*)src, 0);
}

/** Read 1 bitlong according to normal order
 */
BITCODE_RL
bit_read_L (Bit_Chain *dat) { return bit_read_RL_LE (dat); }

/** Write 1 bitlong according to normal order
 */
void
bit_write_L (Bit_Chain *dat, BITCODE_RL value)
{
  bit_write_RL_LE (dat, value);
  return;
}

/** Read 2 time BL bitlong (compacted data).
 *  julian days + milliseconds since midnight
 *  used for TDCREATE, TDUPDATE, and all other DATE variables.
 *  pre-R13 read 2xRL
 */
BITCODE_TIMEBLL
bit_read_TIMEBLL (Bit_Chain *dat)
{
  BITCODE_TIMEBLL date;
  if (dat->from_version < R_13)
    {
      date.days = bit_read_RL (dat);
      date.ms = bit_read_RL (dat);
    }
  else
    {
      date.days = bit_read_BL (dat);
      date.ms = bit_read_BL (dat);
    }
  date.value = date.days + (date.ms * 1e-8);
  return date;
}

/** Write 2 time BL bitlong (compacted data).
 *  Ignores the double value.
 */
void
bit_write_TIMEBLL (Bit_Chain *dat, BITCODE_TIMEBLL date)
{
  if (dat->version < R_13)
    {
      bit_write_RL (dat, date.days);
      bit_write_RL (dat, date.ms);
    }
  else
    {
      bit_write_BL (dat, date.days);
      bit_write_BL (dat, date.ms);
    }
}

BITCODE_TIMERLL
bit_read_TIMERLL (Bit_Chain *dat)
{
  BITCODE_TIMERLL date;
  date.days = bit_read_RL (dat);
  date.ms = bit_read_RL (dat);
  date.value = date.days + (date.ms * 1e-8); // just for display, not calculations
  return date;
}

/** Write 2x time RL.
 *  Ignores the double value.
 */
void
bit_write_TIMERLL (Bit_Chain *restrict dat, BITCODE_TIMERLL date)
{
  bit_write_RL (dat, date.days);
  bit_write_RL (dat, date.ms);
}

/** Read color
 */
int
bit_read_CMC (Bit_Chain *dat, Bit_Chain *str_dat, Dwg_Color *restrict color)
{
  memset (color, 0, sizeof (Dwg_Color));
  if (dat->from_version < R_13)
    color->index = (BITCODE_RSd)bit_read_RS (dat);
  else
    color->index = bit_read_BS (dat);
  if (dat->from_version >= R_2004) // truecolor
    {
      CHK_OVERFLOW_PLUS (1,__FUNCTION__,DWG_ERR_VALUEOUTOFBOUNDS)
      color->rgb = bit_read_BL (dat);
      CHK_OVERFLOW_PLUS (1,__FUNCTION__,DWG_ERR_VALUEOUTOFBOUNDS)
      color->method = color->rgb >> 0x18;
      color->flag = bit_read_RC (dat);
      CHK_OVERFLOW_PLUS (0,__FUNCTION__,DWG_ERR_VALUEOUTOFBOUNDS)
      if (color->flag < 4)
        {
          color->name      = (color->flag & 1) ? (char *)bit_read_T (str_dat) : NULL;
          color->book_name = (color->flag & 2) ? (char *)bit_read_T (str_dat) : NULL;
        }
      else
        {
          LOG_ERROR ("Invalid CMC flag 0x%x ignored", color->flag);
          color->flag = 0;
        }
      if (color->method < 0xc0 || color->method > 0xc8)
        {
          LOG_ERROR ("Invalid CMC method 0x%x ignored", color->method);
          color->method = 0xc2;
          color->rgb = 0xc2000000 | (color->rgb & 0xffffff);
        }
      // fixup index by palette lookup
      color->index = dwg_find_color_index (color->rgb);
    }
  return 0;
}

// from old palette to r2004+ truecolor (FIXME)
void
bit_upconvert_CMC (Bit_Chain *dat, Dwg_Color *restrict color)
{
  if (dat->version >= R_2004 && dat->from_version < R_2004)
    {
      if (!color->method)
        color->method = 0xc3;
      if (color->index == 256)
        color->method = 0xc0;
      else if (color->index == 0)
        color->method = 0xc1;

      color->rgb = color->method << 0x18;
      if (color->method == 0xc3)
        color->rgb |= dwg_rgb_palette_index (color->index);
    }
}

// from r2004+ truecolor to old palette index
void
bit_downconvert_CMC (Bit_Chain *dat, Dwg_Color *restrict color)
{
  if (dat->version < R_2004 && dat->from_version >= R_2004)
    {
      if (!color->method && color->rgb & 0xFF000000)
        color->method = color->rgb >> 0x18;
      color->rgb &= 0x00FFFFFF;
      color->index = dwg_find_color_index (color->rgb);
      switch (color->method)
        {
        case 0x0:
        case 0xc0:
          color->index = 256;
          break; // ByLayer
        case 0xc1:
          color->index = 0;
          break;   // ByBlock
        case 0xc2: // Entity
        case 0xc3: // TrueColor
          if (color->index == 256)
            color->index = color->rgb & 0xff;
          break;
        case 0xc8:
          color->index = 0;
          break; // none
        default:
          break;
        }
    }
}

/** Write color
 */
void
bit_write_CMC (Bit_Chain *dat, Bit_Chain *str_dat, Dwg_Color *restrict color)
{
  if (dat->version >= R_2004) // truecolor
    {
      if (dat->from_version < R_2004)
        bit_upconvert_CMC (dat, color);
      bit_write_BS (dat, 0);  // index override
      bit_write_BL (dat, color->rgb);
      if (!color->method && color->rgb & 0xFF000000)
        color->method = color->rgb >> 0x18;
      if (color->method == 0xc2) // for entity
        {
          if (color->name && !bit_empty_T (dat, color->name))
            color->flag |= 1;
          if (color->name && !bit_empty_T (dat, color->book_name))
            color->flag |= 2;
          bit_write_RC (dat, color->flag);
          if (color->flag & 1)
            bit_write_T (str_dat, color->name);
          if (color->flag & 2)
            bit_write_T (str_dat, color->book_name);
        }
      else
        bit_write_RC (dat, 0); // ignore the flag
    }
  else
    {
      bit_downconvert_CMC (dat, color);
      if (dat->version < R_13)
        bit_write_RS (dat, color->index);
      else
        bit_write_BS (dat, color->index);
    }
}

/** Read entity color (2004+) (truecolor rgb and alpha support)
 *  Does also references, DBCOLOR lookups.
 */
void
bit_read_ENC (Bit_Chain *dat, Bit_Chain *hdl_dat, Bit_Chain *str_dat,
              Dwg_Color *restrict color)
{
  color->index = bit_read_BS (dat);
  if (dat->from_version >= R_2004)
    {
      uint16_t flag = (((uint32_t)color->index) >> 8) & 0xff;
      color->index &= 0x1ff;
      if (flag & 0x80)
        color->rgb = bit_read_BL (dat); // ODA bug, documented as BS
      if (flag & 0x40)
        {
          color->handle = (BITCODE_H)calloc (1, sizeof (Dwg_Object_Ref));
          if (!color->handle)
            {
              loglevel = dat->opts & DWG_OPTS_LOGLEVEL;
              LOG_ERROR ("Out of memory")
              return;
            }
          bit_read_H (hdl_dat, &(color->handle->handleref)); // => DBCOLOR
          // else defer to dwg_decode_common_entity_handle_data ()
        }
      if (flag & 0x20)
        {
          BITCODE_BL alpha = bit_read_BL (dat);
          color->alpha_type = alpha & 0xff; // 0, 1 or 3
          color->alpha = alpha >> 8;
        }
      color->flag = (uint16_t)flag;
    }
}

/** Write entity color (2004+)
 */
void
bit_write_ENC (Bit_Chain *dat, Bit_Chain *hdl_dat, Bit_Chain *str_dat,
               Dwg_Color *restrict color)
{
  bit_write_BS (dat, (color->index & 0x1ff) | (color->flag << 8));
  if (dat->version >= R_2004)
    {
      uint16_t flag = color->flag;
      if (flag & 0x20)
        bit_write_BL (dat, color->alpha);
      if (!(flag & 0x40) && (flag & 0x80))
        bit_write_BL (dat, color->rgb);
      // ?? wide?
      if ((flag & 0x41) == 0x41)
        bit_write_T (str_dat, color->name);
      if ((flag & 0x42) == 0x42)
        bit_write_T (str_dat, color->book_name);
      if (flag & 0x40)
        bit_write_H (hdl_dat, &(color->handle->handleref)); // => DBCOLOR
    }
}

/** Search for a sentinel; if found, positions "dat->byte" immediately after it
    and returns -1
 */
int
bit_search_sentinel (Bit_Chain *dat, unsigned char sentinel[16])
{
  long unsigned int i, j;

  if (dat->size < 16) // too short
    return 0;
  for (i = 0; i < dat->size - 16; i++)
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
bit_write_sentinel (Bit_Chain *dat, unsigned char sentinel[16])
{
  int i;

  for (i = 0; i < 16; i++)
    bit_write_RC (dat, sentinel[i]);
}

void
bit_chain_init (Bit_Chain *dat, const int size)
{
  dat->chain = (unsigned char *)calloc (1, size);
  if (!dat->chain)
    {
      loglevel = dat->opts & DWG_OPTS_LOGLEVEL;
      LOG_ERROR ("Out of memory")
      abort();
    }
  dat->size = (long unsigned int)size;
  dat->byte = 0;
  dat->bit = 0;
}

void
bit_chain_init_dat (Bit_Chain *restrict dat, const int size,
                    const Bit_Chain *restrict from_dat)
{
  bit_chain_init (dat, size);
  bit_chain_set_version (dat, from_dat);
}

/*
 * Allocates or adds more memory space for bit_chain
 * adds a 1kB page.
 */
void bit_chain_alloc_size (Bit_Chain *dat, const size_t size)
{
  if (dat->size == 0)
    {
      bit_chain_init (dat, size);
    }
  else
    {
      dat->chain
          = (unsigned char *)realloc (dat->chain, dat->size + size);
      if (!dat->chain)
        {
          loglevel = dat->opts & DWG_OPTS_LOGLEVEL;
          LOG_ERROR ("Out of memory")
          abort();
        }
      memset (&dat->chain[dat->size], 0, size);
      dat->size += size;
    }
}

#define CHAIN_BLOCK 4096
void
bit_chain_alloc (Bit_Chain *dat)
{
  bit_chain_alloc_size (dat, CHAIN_BLOCK);
}

void
bit_chain_free (Bit_Chain *dat)
{
  if (dat->chain)
    {
      free (dat->chain);
      dat->chain = NULL;
    }
  dat->size = 0;
}

void
bit_print (Bit_Chain *dat, long unsigned int size)
{
  unsigned char sig;
  long unsigned int i, j;

  printf ("---------------------------------------------------------");
  if (size > dat->size)
    size = dat->size;
  for (i = 0; i < size; i++)
    {
      if (i % 16 == 0)
        printf ("\n[0x%04X]: ", (unsigned int)i);
      printf ("%02X ", (unsigned char)dat->chain[i]);
      if (i % 16 == 15)
        for (j = i - 15; j <= i; j++)
          {
            sig = dat->chain[j];
            printf ("%c", sig >= ' ' && sig < 128 ? sig : '.');
          }
    }
  puts ("");
  puts ("---------------------------------------------------------");
}

// The i-th bit of a string.
// 0b1000_0000,0 >> 8 = 1
#define BIT(b, i) (((b)[(i) / 8] & (0x80 >> (i) % 8)) >> (7 - (i) % 8))

void
bit_write_bits (Bit_Chain *restrict dat, const char *restrict bits)
{
  char *p = (char *)bits;
  for (; *p; p++)
    {
      if (*p == '0' || *p == '1')
        bit_write_B (dat, *p != '0');
      else
        {
          HANDLER (OUTPUT, "ERROR: Invalid binary input %s\n", p);
          return;
        }
    }
}

// accept a string of hex bytes with optional whitespace
long
bit_write_hexbits (Bit_Chain *restrict dat, const char *restrict bytes)
{
  char *p = (char *)bytes;
  long len = 0;
  unsigned char b = '\0';

  for (; *p; p++)
    {
      if (*p != ' ' && *p != '\n')
        {
          len++;
          if (*p >= 'a' && *p <= 'f')
            {
              if (len % 2)
                b = (*p + 10 - 'a') << 4;
              else
                bit_write_RC (dat, b + *p + 10 - 'a');
            }
          else if (*p >= 'A' && *p <= 'F')
            {
              if (len % 2)
                b = (*p + 10 - 'A') << 4;
              else
                bit_write_RC (dat, b + *p + 10 - 'A');
            }
          else if (*p >= '0' && *p <= '9')
            {
              if (len % 2)
                b = (*p - '0') << 4;
              else
                bit_write_RC (dat, b + *p - '0');
            }
          else
            {
              HANDLER (OUTPUT, "ERROR: Invalid hex input %s\n", p);
              return 0;
            }
        }
    }
  return len;
}

void
bit_print_bits (unsigned char *bits, long unsigned int bitsize)
{
  for (long unsigned int i = 0; i < bitsize; i++)
    {
      unsigned char bit = i % 8;
      unsigned char result = (bits[i / 8] & (0x80 >> bit)) >> (7 - bit);
      // if (i && (i % 8 == 0)) printf(" ");
      printf ("%d", result ? 1 : 0);
    }
  printf ("\n");
}

void
bit_fprint_bits (FILE *fp, unsigned char *bits, long unsigned int bitsize)
{
  for (long unsigned int i = 0; i < bitsize; i++)
    {
      unsigned char bit = i % 8;
      unsigned char result = (bits[i / 8] & (0x80 >> bit)) >> (7 - bit);
      /*if (i && !bit) HANDLER (fp, " ");*/
      HANDLER (fp, "%d", result ? 1 : 0);
      // fprintf(fp, "%d", BIT(bits, i) ? 1 : 0);
    }
}

void
bit_explore_chain (Bit_Chain *dat, long unsigned int datsize)
{
  unsigned char sig;
  long unsigned int i, k;

  if (datsize > dat->size)
    datsize = dat->size;

  for (k = 0; k < 8; k++)
    {
      printf ("---------------------------------------------------------");
      dat->byte = 0;
      dat->bit = k;
      for (i = 0; i < datsize - 1; i++)
        {
          if (i % 16 == 0)
            printf ("\n[0x%04X]: ", (unsigned int)i);
          sig = bit_read_RC (dat);
          printf ("%c", sig >= ' ' && sig < 128 ? sig : '.');
        }
      puts ("");
    }
  puts ("---------------------------------------------------------");
}

uint16_t
bit_calc_CRC (const uint16_t seed, unsigned char *addr, long len)
{
  unsigned char al;
  uint16_t dx = seed;

  static const uint16_t crctable[256] = {
    0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241, 0xC601,
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
    0x4100, 0x81C1, 0x8081, 0x4040
  };

  for (; len > 0; len--)
    {
      al = (unsigned char)((*addr) ^ ((unsigned char)(dx & 0xFF)));
      dx = ((dx >> 8) & 0xFF) ^ crctable[al];
      addr++;
    }
  return dx;
}

uint32_t
bit_calc_CRC32 (const uint32_t seed, unsigned char *addr, long len)
{

  static const uint32_t crctable[256] = {
    0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
    0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
    0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
    0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
    0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
    0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
    0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
    0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
    0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
    0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
    0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106,
    0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
    0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
    0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
    0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
    0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
    0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
    0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
    0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
    0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
    0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
    0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
    0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
    0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
    0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
    0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
    0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
    0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
    0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
    0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
    0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
    0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
    0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
    0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
    0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
    0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
    0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
    0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
    0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
    0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
    0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
    0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
    0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
  };

  uint32_t invertedCrc = ~seed;
  while (len--) {
    uint8_t byte = *addr++;
    invertedCrc = (invertedCrc >> 8) ^ crctable[(invertedCrc ^ byte) & 0xff];
  }
  return ~invertedCrc;
}

bool does_cross_unicode_datversion (Bit_Chain *restrict dat)
{
  if ((dat->version < R_2007 && dat->from_version >= R_2007)
      || (dat->version >= R_2007 && dat->from_version < R_2007))
    return true;
  else
    return false;
}

/* Copy the whole content of tmp_data to dat, and reset tmp_dat.
   WARN: This might change dat->chain  */
void bit_copy_chain (Bit_Chain *restrict dat, Bit_Chain *restrict tmp_dat)
{
  unsigned long i;
  unsigned long dat_bits = bit_position (tmp_dat);
  unsigned long size = tmp_dat->byte;
  while (dat->byte + size > dat->size)
    bit_chain_alloc (dat);
  // check if dat is byte aligned, tmp_dat always is. we can use memcpy then.
  if (!dat->bit)
    {
      assert(!tmp_dat->bit);
      memcpy (&dat->chain[dat->byte], &tmp_dat->chain[0], size);
      dat->byte += size;
    }
  else
    {
      bit_set_position (tmp_dat, 0);
      for (i = 0; i < size; i++)
        {
          bit_write_RC (dat, bit_read_RC (tmp_dat));
        }
      for (i = 0; i < dat_bits % 8; i++)
        {
          bit_write_B (dat, bit_read_B (tmp_dat));
        }
    }
  bit_set_position (tmp_dat, 0);
}
