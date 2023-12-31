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
#  error in_dxf.h must be included after __FILE__ because of FORMAT_BD
#endif

EXPORT int dwg_encode (Dwg_Data *restrict dwg, Bit_Chain *restrict dat);

/** Returns the class for the object.
    First searching class by name, not offset
    because indxf has a different class order. Fixes up the obj->type then.
    Also searches via dxfname aliases, like DICTIONARYWDFLT =>
   ACDBDICTIONARYWDFLT and fixes up obj->type also then. If obj->dxfname is
   NULL, the search is done by index, without any fixups.
 */
Dwg_Class *dwg_encode_get_class (Dwg_Data *restrict dwg,
                                 Dwg_Object *restrict obj);

Dwg_Object *find_prev_entity (Dwg_Object *obj);
void in_postprocess_handles (Dwg_Object *restrict obj);
void in_postprocess_SEQEND (Dwg_Object *restrict obj, BITCODE_BL num_owned,
                            BITCODE_H *owned);
void dwg_set_dataflags (Dwg_Object *obj);
int dwg_encode_unknown_rest (Bit_Chain *restrict dat,
                             Dwg_Object *restrict obj);
bool dwg_encode_unknown_bits (Bit_Chain *restrict dat,
                              Dwg_Object *restrict obj);
void downconvert_TABLESTYLE (Dwg_Object *restrict obj);

#endif
