/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2018-2020 Free Software Foundation, Inc.                   */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * unknown.c: decode unknown bitstreams
 * written by Reini Urban
 */

#ifndef UNKNOWN_H
#define UNKNOWN_H

struct _unknown_field
{
  int code;
  const char *value;
  unsigned char *bytes;
  int num_bits;
  Dwg_Bits type;
  const char *name; // in dwg.spec
  // number of occurrences of this type:value pair (max 1423)
  unsigned short num;
  int pos[5]; // 5x found bit offset in dxf->bytes or -1 if not found
  // many typical values are 5x found (handle 0, BL 2)
};

struct _unknown_dxf
{
  const char *name;
  const char *dxf;
  const unsigned int handle;
  const char *bytes;
  const int is_entity;
  const int num_bits; // size of dumped unknown_bits TF
  const int commonsize;
  const int hdloff;
  const int strsize;
  const int hdlsize;
  const int bitsize;
  const struct _unknown_field *fields;
};

// dynamic helper companion
struct _dxf
{
  unsigned char *found;    // coverage per bit for found 1
  unsigned char *possible; // coverage for mult. finds >1
  int num_bits;            // copy of unknown_dxf.num_bits
  int num_filled;
  int num_empty;
  int num_possible;
  double percentage;
};

#endif
