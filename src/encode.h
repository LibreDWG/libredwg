/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2009-2019 Free Software Foundation, Inc.                   */
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

#endif
