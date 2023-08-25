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
 * out_dxf.h: write dwg as DXF (Ascii or Binary)
 * written by Reini Urban
 */

#ifndef OUT_DXF_H
#define OUT_DXF_H

#include "dwg.h"
#include "bits.h"

#ifdef IN_DXF_H
#  error in_dxf.h must be included after out_dxf.h because of FORMAT_BD
#endif
#ifndef DXF_PRECISION
#  define DXF_PRECISION 16
#endif

#define DXF_FORMAT_FLT "%0." _XSTR(DXF_PRECISION) "G"
#define DXF_FLT_MAXLEN 18

const char *dxf_format (int code) RETURNS_NONNULL;
const char *dxf_codepage (BITCODE_RS code, Dwg_Data *dwg) RETURNS_NONNULL;
int dxf_is_sorted_INSERT (const Dwg_Object *restrict obj);
int dxf_is_sorted_POLYLINE (const Dwg_Object *restrict obj);
int dxf_validate_DICTIONARY (Dwg_Object *obj);
bool dxf_has_STYLE_eed (Bit_Chain *restrict dat,
                        const Dwg_Object_Object *restrict obj);
bool dxf_has_xrefdep_vertbar (Bit_Chain *restrict dat, const char *name);
bool dxf_is_xrefdep_name (Bit_Chain *restrict dat, const char *name);

EXPORT int dwg_write_dxf (Bit_Chain *dat, Dwg_Data *dwg) __nonnull_all;
EXPORT int dwg_write_dxfb (Bit_Chain *dat, Dwg_Data *dwg) __nonnull_all;

#endif
