/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2018-2019 Free Software Foundation, Inc.                   */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * bits.c: print all possible type/values of given bits
 * written by Reini Urban
 */

#include "../src/config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "dwg.h"
#include "../src/bits.h"
static int maxoff = 0;

static int
decode (Bit_Chain *dat, int off, const int size)
{
  int pos = 0;
  int bs = 0;
  printf ("decode offset:%d, size:%d\n", off, size);
  if (off >= size)
    return 0;
  if (off < maxoff)
    return 0;

  bit_set_position (dat, off);
  if (size - off >= 66)
    {
      double d = bit_read_BD (dat);
      pos = (int)bit_position (dat);
      if (d != bit_nan ())
        printf ("%.15f BD @%d (%d)\n", d, pos, size);
      bit_set_position (dat, off);
    }
  if (size - off >= 64)
    {
      double d = bit_read_RD (dat);
      pos = (int)bit_position (dat);
      if (d != bit_nan ())
        printf ("%.15f RD @%d (%d)\n", d, pos, size);
      bit_set_position (dat, off);
    }
  if (size - off >= 34)
    {
      long l = (long)bit_read_BL (dat);
      pos = (int)bit_position (dat);
      if (pos <= size - off)
        printf ("%ld BL @%d (%d)\n", l, pos, size);
      bit_set_position (dat, off);
    }
  if (size - off >= 32)
    {
      long l = (long)bit_read_RL (dat);
      pos = (int)bit_position (dat);
      printf ("%ld RL @%d (%d)\n", l, pos, size);
      bit_set_position (dat, off);
    }
  if (size - off >= 16)
    {
      int l = (int)bit_read_RS (dat);
      pos = (int)bit_position (dat);
      printf ("%d RS @%d (%d)\n", l, pos, size);
      bit_set_position (dat, off);
    }
  if (size - off >= 10)
    {
      bs = bit_read_BS (dat);
      pos = (int)bit_position (dat);
      if (pos <= size - off)
        printf ("%d BS @%d (%d)\n", bs, pos, size);
      bit_set_position (dat, off);
    }
  if (size - off >= 8)
    {
      int l = bit_read_RC (dat);
      pos = (int)bit_position (dat);
      printf ("%d RC @%d (%d)\n", l, pos, size);
      bit_set_position (dat, off);
    }
  if (size - off >= 8)
    {
      Dwg_Handle h;
      int err;
      err = bit_read_H (dat, &h);
      pos = (int)bit_position (dat);
      if (!err && h.size == 1 && pos <= size - off)
        {
          printf ("%x.%d.%lX H @%d (%d)\n", h.code, h.size, h.value, pos,
                  size);
        }
      bit_set_position (dat, off);
    }
  if (size - off >= 4)
    {
      Dwg_Color c;
      bit_read_CMC (dat, &c);
      pos = (int)bit_position (dat);
      if (c.index < 257 && pos <= size - off)
        {
          printf ("%d 0x%06X 0x%x CMC @%d (%d)\n", c.index, c.rgb, c.flag, pos,
                  size);
        }
      bit_set_position (dat, off);
    }
  if (size - off >= 2)
    {
      int i = bit_read_BB (dat);
      pos = (int)bit_position (dat);
      printf ("%d BB @%d (%d)\n", i, pos, size);
      if (i == 2)
        printf ("%d BS/BL @%d (%d)\n", 0, pos, size);
      else if (i == 3)
        printf ("%d BS @%d (%d)\n", 256, pos, size);
      bit_set_position (dat, off);
    }
  // if BS is a valid length try TV also
  if (bs > 0 && bs < size - off - pos)
    {
      int i;
      char *s = bit_read_TV (dat);
      pos = (int)bit_position (dat);
      for (i = 0; i < bs; i++)
        if (!isprint (s[i]))
          break;
      if (i == bs)
        printf ("%s TV @%d (%d)\n", s, pos, size);
    }

  while (off < size)
    {
      if (maxoff < off)
        maxoff = off;
      // printf ("offset %d\n", pos);
      return decode (dat, pos, size);
    }
  return 0;
}

int
main (int argc, char *argv[])
{
  int hex = 0;
  int i;
  int pos;
  Bit_Chain dat = { NULL, 0, 0, 0, NULL, 0, 0 };

  if (argc < 2)
    {
      printf ("usage: examples/bits "
              "001100000000000000000000000000011000000000000000100000001010010"
              "00\n");
      printf ("or examples/bits -x '8055 40f9 3284 d222 3e40 7436 e0d9 23fd'\n");
      return 1;
    }
  if (argc > 2 && !strcmp (argv[1], "-x"))
    {
      hex = 1;
      i = 2;
    }
  else
    {
      i = 1;
    }

  //dat.chain = malloc (size + 1);
  //dat.size = size;
  dat.version = R_2004;
  do 
    {
      char *input = argv[i];
      int bits = strlen (input);
      if (hex)
        dat.size += bits;
      else
        dat.size += bits / 8;
      dat.chain = realloc (dat.chain, dat.size + 1);
      if (hex)
        {
          int size = bit_write_hexbits (&dat, input);
          dat.size -= dat.size - size;
        }
      else
        bit_write_bits (&dat, input);
      i++;
    }
  while (i < argc);

  pos = (int)bit_position (&dat);
  // accept all types, like CMC, BS, BL, HANDLE and print all possible variants
  pos = decode (&dat, 0, (int)pos);

  free (dat.chain);
  return 0;
}
