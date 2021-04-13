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
  BITCODE_BS bs = 0;
  printf ("decode offset:%d, size:%d\n", off, size);
  if (off >= size)
    return 0;

  bit_set_position (dat, off);
  if (size - off >= 66)
    {
      double d = bit_read_BD (dat);
      int p = (int)bit_position (dat);
      if (d != bit_nan ())
        {
          printf ("%.15f BD @%d @%u.%u (%d)\n", d, p, p/8, p%8, size);
          pos = p;
        }
      bit_set_position (dat, off);
    }
  if (size - off >= 64)
    {
      double d = bit_read_RD (dat);
      int p = (int)bit_position (dat);
      if (d != bit_nan ())
        {
          printf ("%.15f RD @%d @%u.%u (%d)\n", d, p, p/8, p%8, size);
          pos = p;
        }
      bit_set_position (dat, off);
    }
  if (size - off >= 34)
    {
      BITCODE_BL l = (long)bit_read_BL (dat);
      int p = (int)bit_position (dat);
      if (p <= size - off)
        {
          printf ("%u BL @%d @%u.%u (%d)\n", l, p, p/8, p%8, size);
          pos = p;
        }
      bit_set_position (dat, off);
    }
  if (size - off >= 32)
    {
      BITCODE_BL l = (long)bit_read_RL (dat);
      pos = (int)bit_position (dat);
      printf ("%u RL @%d @%u.%u (%d)\n", l, pos, pos/8, pos%8, size);
      bit_set_position (dat, off);
    }
  if (size - off >= 16)
    {
      BITCODE_RS l = (int)bit_read_RS (dat);
      pos = (int)bit_position (dat);
      printf ("%d RS @%d (%d)\n", l, pos, size);
      bit_set_position (dat, off);
    }
  if (size - off >= 10)
    {
      int p;
      bs = bit_read_BS (dat);
      p = (int)bit_position (dat);
      if (p <= size - off)
        {
          printf ("%d BS @%d (%d)\n", bs, p, size);
          pos = p;
        }
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
      int err, p;
      err = bit_read_H (dat, &h);
      p = (int)bit_position (dat);
      if (!err && h.size == 1 && p <= size - off)
        {
          printf ("%x.%d.%lX H @%d (%d)\n", h.code, h.size, h.value, p, size);
          pos = p;
        }
      bit_set_position (dat, off);
    }
  if (size - off >= 4)
    {
      Dwg_Color c;
      int p;
      bit_read_CMC (dat, dat, &c);
      p = (int)bit_position (dat);
      if (c.index < 257 && p <= size - off)
        {
          printf ("%d 0x%06X 0x%x CMC @%d (%d)\n", c.index, c.rgb, c.flag, p,
                  size);
          pos = p;
        }
      FREE (c.name);
      FREE (c.book_name);
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
      int p = (int)bit_position (dat);
      if (s)
        {
          for (i = 0; i < bs; i++)
            if (!isprint (s[i]))
              break;
          if (i == bs)
            {
              printf ("%s TV @%d (%d)\n", s, pos, size);
              pos = p;
            }
          FREE (s);
        }
    }

  while (pos < size)
    {
      if (pos <= off)
        pos++;
      if (maxoff < pos)
        maxoff = pos;
      // printf ("offset %d\n", pos);
      return decode (dat, off + 1, size);
    }
  return 0;
}

int
main (int argc, char *argv[])
{
  int hex = 0;
  int print_bits = 0;
  int i;
  int pos;
  Bit_Chain dat;

  GC_INIT ();
  dat = EMPTY_CHAIN (0);
  if (argc < 2)
    {
      printf ("usage: examples/bits "
              "001100000000000000000000000000011000000000000000100000001010010"
              "00\n");
      printf (
          "or examples/bits -x '8055 40f9 3284 d222 3e40 7436 e0d9 23fd'\n");
      return 1;
    }
  i = 1;
  if (argc > 2 && !strcmp (argv[i], "-x"))
    {
      hex = 1;
      i++;
    }
  if (argc > 2 && !strcmp (argv[i], "-b"))
    {
      print_bits = 1;
      i++;
    }

  // dat.chain = MALLOC (size + 1);
  // dat.size = size;
  dat.version = R_2004;
  do
    {
      char *input = argv[i];
      int bits = strlen (input);
      if (hex)
        dat.size += bits;
      else
        dat.size += bits / 8;
      dat.chain = REALLOC (dat.chain, dat.size + 1);
      if (hex)
        {
          int size = bit_write_hexbits (&dat, input);
          dat.size -= dat.size - size;
        }
      else
        bit_write_bits1 (&dat, input);
      i++;
    }
  while (i < argc);

  pos = (int)bit_position (&dat);
  if (print_bits)
    bit_print_bits (&dat.chain[0], pos);
  // accept all types, like CMC, BS, BL, HANDLE and print all possible variants
  pos = decode (&dat, 0, (int)pos);

  FREE (dat.chain);
  return 0;
}
