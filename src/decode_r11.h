/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2022 Free Software Foundation, Inc.                        */
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

int decode_preR13 (Bit_Chain *restrict dat, Dwg_Data *restrict dwg);
int decode_entity_preR13 (Bit_Chain *restrict dat, Dwg_Object *restrict obj,
                          Dwg_Object_Entity *ent);

