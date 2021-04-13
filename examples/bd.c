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
 * bd.c: print double of bits
 * written by Reini Urban
 *
 */

#include "../src/config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dwg.h"
#include "../src/bits.h"

int
main (int argc, char *argv[])
{
  char *input;
  int hex = 0;
  int size, bits;
  unsigned long pos;
  Bit_Chain dat;

  GC_INIT ();
  dat = EMPTY_CHAIN (0);
  if (argc < 2)
    {
      printf ("usage: examples/bd "
              "001100000000000000000000000000011000000000000000100000001010010"
              "00\n");
      printf ("or examples/bd -x '8055 40f9 3284 d222 3e40 7436 e0d9 23fd'\n");
      return 1;
    }
  if (argc > 2 && !strcmp (argv[1], "-x"))
    {
      hex = 1;
      input = argv[2];
      size = strlen (input);
    }
  else
    {
      input = argv[1];
      bits = strlen (input);
      size = (bits / 8);
    }

  dat.chain = MALLOC (size + 1);
  dat.size = size;
  dat.version = R_2004;
  if (hex)
    {
      dat.size = bit_write_hexbits (&dat, input);
      bits = dat.size / 8;
    }
  else
    bit_write_bits1 (&dat, input);
  pos = bit_position (&dat);
  bit_set_position (&dat, 0);
  // accept more types, like CMC, BS, BL, HANDLE and print all possible
  // variants
  if (pos == 64)
    {
      double d = bit_read_RD (&dat);
      printf ("%.15f RD @%lu\n", d, bit_position (&dat));
    }
  else if (pos == 2 || pos > 64)
    {
      double d = bit_read_BD (&dat);
      printf ("%.15f BD @%lu (%ld)\n", d, bit_position (&dat), pos);
      if (pos == 2)
        {
          int i;
          bit_set_position (&dat, 0);
          i = bit_read_BB (&dat);
          printf ("%d BB @%lu\n", i, bit_position (&dat));
          if (i == 2)
            printf ("%d BS/BL @%lu\n", 0, bit_position (&dat));
          else if (i == 3)
            printf ("%d BS @%lu\n", 256, bit_position (&dat));
        }
    }
  else if (pos == 34)
    {
      long l = bit_read_BL (&dat);
      printf ("%ld BL @%lu\n", l, bit_position (&dat));
    }
  else if (pos == 32)
    {
      long l = bit_read_RL (&dat);
      printf ("%ld RL @%lu\n", l, bit_position (&dat));
    }
  else if (pos == 16)
    {
      int l = (int)bit_read_RS (&dat);
      printf ("%d RS @%lu\n", l, bit_position (&dat));
    }
  else if (pos == 10)
    {
      long l = bit_read_BS (&dat);
      Dwg_Handle h;
      printf ("%ld BS @%lu\n", l, bit_position (&dat));
      bit_set_position (&dat, 0);
      if (!bit_read_H (&dat, &h) && h.size == 1)
        printf (FORMAT_H " H @%lu (%ld)\n", ARGS_H (h), bit_position (&dat),
                pos);
    }
  else if (pos == 8)
    {
      int l = (int)bit_read_RC (&dat);
      Dwg_Handle h;
      printf ("%d RC @%lu\n", l, bit_position (&dat));
      bit_set_position (&dat, 0);
      if (!bit_read_H (&dat, &h) && h.size == 1)
        printf (FORMAT_H " H @%lu (%ld)\n", ARGS_H (h), bit_position (&dat),
                pos);
    }
  else
    {
      Dwg_Handle h;
      Dwg_Color c;
      double d = bit_read_BD (&dat);
      if (pos >= bit_position (&dat))
        {
          printf ("%.15f BD? @%lu (%ld)\n", d, bit_position (&dat), pos);
        }
      else
        { // divide into chunks of 34,32,18,16,10,8,2
          printf ("? (%ld)\n", pos);
        }
      bit_set_position (&dat, 0);
      if (!bit_read_H (&dat, &h))
        printf (FORMAT_H " H? @%lu (%ld)\n", ARGS_H (h), bit_position (&dat),
                pos);
      bit_set_position (&dat, 0);
      bit_read_CMC (&dat, &dat, &c);
      if (c.index < 257)
        printf ("%d 0x%06X 0x%x CMC? @%lu (%ld)\n", c.index, c.rgb, c.flag,
                bit_position (&dat), pos);
    }
  FREE (dat.chain);
  return 0;
}
