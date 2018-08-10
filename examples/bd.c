/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2018 Free Software Foundation, Inc.                        */
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
  double bd;
  int size;
  unsigned long pos;
  Bit_Chain dat = {NULL,16,0,0,NULL,0,0};

  if (argc < 2) {
    printf ("usage: examples/bd 00110000000000000000000000000001100000000000000010000000101001000\n");
    return 1;
  }

  size = 1+(strlen(argv[1])/8);
  dat.chain = malloc(size);
  dat.size = size;
  bit_write_bits(&dat, argv[1]);
  pos = bit_position(&dat);
  bit_set_position(&dat, 0);
  // TODO: accept more types, like CMC, BS, BL, HANDLE and print all possible variants
  if (pos == 64)
    bd = bit_read_RD(&dat);
  else
    bd = bit_read_BD(&dat);
  pos = bit_position(&dat);
  printf( "%.16f @%lu\n", bd, pos);
  free (dat.chain);
  return 0;
}
