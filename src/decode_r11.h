/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2022-2023 Free Software Foundation, Inc.                   */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * decode_r11.h: preR13 decoding
 * written by Reini Urban
 */

#ifndef DECODE_R11_H
#define DECODE_R11_H

// exported only for unit-testing/decode_test
// not part of the public API
EXPORT int decode_preR13 (Bit_Chain *restrict dat, Dwg_Data *restrict dwg);
EXPORT int decode_entity_preR13 (Bit_Chain *restrict dat,
                                 Dwg_Object *restrict obj,
                                 Dwg_Object_Entity *ent);
EXPORT Dwg_Object_Ref *dwg_decode_preR13_handleref (Bit_Chain *restrict dat,
                                                    int size,
                                                    Dwg_Data *restrict dwg);
#endif
