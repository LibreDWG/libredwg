/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2009-2020 Free Software Foundation, Inc.                   */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * encode.h: encoding function prototypes, write from dwg to dat buffer.
 * written by Felipe Castro
 * modified by Felipe Corrêa da Silva Sances
 * modified by Rodrigo Rodrigues da Silva
 * modified by Reini Urban
 */

#ifndef ENCODE_H
#define ENCODE_H

#include "bits.h"
#include "dwg.h"

/* only when the versions mismatch */
#define ENCODE_UNKNOWN_AS_DUMMY

#ifdef IN_DXF_H
#error in_dxf.h must be included after __FILE__ because of FORMAT_BD
#endif

int dwg_encode (Dwg_Data *restrict dwg, Bit_Chain *restrict dat);
/** Returns the class for the object.
    First searching class by name, not offset
    because indxf has a different class order. Fixes up the obj->type then.
    Also searches via dxfname aliases, like DICTIONARYWDFLT => ACDBDICTIONARYWDFLT
    and fixes up obj->type also then.
    If obj->dxfname is NULL, the search is done by index, without any fixups.
 */
Dwg_Class *dwg_encode_get_class (Dwg_Data *restrict dwg, Dwg_Object *restrict obj);
char *encrypt_sat1 (BITCODE_BL blocksize, BITCODE_RC *restrict acis_data, int *restrict idx);
unsigned int section_max_decomp_size (const Dwg_Data *dwg, const Dwg_Section_Type id);

// push to entities and entries handles array
#define PUSH_HV(_obj, numfield, hvfield, ref)                                 \
  {                                                                           \
    _obj->hvfield                                                             \
        = realloc (_obj->hvfield, (_obj->numfield + 1) * sizeof (BITCODE_H)); \
    _obj->hvfield[_obj->numfield] = ref;                                      \
    LOG_TRACE ("%s[%d] = " FORMAT_REF " [H]\n", #hvfield, _obj->numfield,     \
               ARGS_REF (_obj->hvfield[_obj->numfield]));                     \
    _obj->numfield++;                                                         \
  }

#endif
