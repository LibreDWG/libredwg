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
int dwg_encode_entity (Dwg_Object *restrict obj, Bit_Chain *dat,
                       Bit_Chain *restrict hdl_dat, Bit_Chain *str_dat);
int dwg_encode_object (Dwg_Object *restrict obj, Bit_Chain *dat,
                       Bit_Chain *restrict hdl_dat, Bit_Chain *str_dat);
int dwg_encode_common_entity_handle_data (Bit_Chain *dat, Bit_Chain *hdl_dat,
                                          Dwg_Object *restrict obj);
int dwg_encode_xdata (Bit_Chain *restrict dat,
                      Dwg_Object_XRECORD *restrict _obj, unsigned xdata_size);
bool dwg_encode_unknown_bits (Bit_Chain *restrict dat,
                              Dwg_Object *restrict obj);
void downconvert_TABLESTYLE (Dwg_Object *restrict obj);
void upconvert_TABLESTYLE (Dwg_Object *restrict obj);

/* ACAD_PROXY_ENTITY/OBJECT data, see encode.c */
int dwg_proxy_from_unknown (Dwg_Data *restrict dwg, Dwg_Object *restrict obj,
                            BITCODE_RC **datap, BITCODE_BL *numbitsp,
                            BITCODE_H **objidsp, BITCODE_BL *num_objidsp,
                            BITCODE_BL *versionp);
BITCODE_RC *dwg_proxy_bits (const BITCODE_RC *restrict data,
                            BITCODE_BL numbits, BITCODE_BL from, BITCODE_BL n);
int dwg_proxy_split (const BITCODE_RC *restrict data, BITCODE_BL numbits,
                     BITCODE_BL *dbits, BITCODE_BL *sbits);
BITCODE_RC *dwg_proxy_join (const BITCODE_RC *restrict d, BITCODE_BL dbits,
                            const BITCODE_RC *restrict s, BITCODE_BL sbits,
                            int has_strings, BITCODE_BL *numbitsp);
BITCODE_BL dwg_dxf_class_number (const Dwg_Data *restrict dwg,
                                 Dwg_Version_Type version, BITCODE_BL number);

#endif
