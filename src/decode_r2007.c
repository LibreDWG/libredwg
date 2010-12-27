/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2010 Free Software Foundation, Inc.                        */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * decode_r2007.c: functions to decode R2007 (r21) files
 * written by Till Heuschmann
 */

#include <stdio.h>
#include <stdlib.h>
#ifndef __APPLE__
#include <sys/malloc.h>
#endif
#include <sys/types.h>
#include <stdbool.h>
#include "bits.h"
#include "logging.h"

int
read_r2007_meta_data(Bit_Chain* dat, Dwg_Data * dwg)
{
  printf("\nInitial version of R2007 decoder\n");

  return 0;
}
