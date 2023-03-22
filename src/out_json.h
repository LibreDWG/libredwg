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
 * out_json.h: write dwg as json
 * written by Reini Urban
 */

#ifndef OUT_JSON_H
#define OUT_JSON_H

#include "dwg.h"
#include "bits.h"

#ifdef IN_DXF_H
#  error in_dxf.h must be included after out_json.h because of FORMAT_BD
#endif

EXPORT int dwg_write_json (Bit_Chain *restrict dat, Dwg_Data *restrict dwg);
EXPORT int dwg_write_geojson (Bit_Chain *restrict dat, Dwg_Data *restrict dwg);

// converts a TV to a UTF-8 string (with codepage conversion) and quoting
// ", \n \r, U+XXXX
char *json_cquote (char *restrict dest, const char *restrict src,
                   const size_t len, const BITCODE_RS codepage);

#endif
