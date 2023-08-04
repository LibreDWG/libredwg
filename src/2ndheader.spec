/* -*- c -*- */
/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2023 Free Software Foundation, Inc.                        */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * 2ndheader.spec: DWG 2ndheader specification
 * written by Reini Urban
 */

#include "spec.h"

VERSIONS (R_13, R_2000) {
  IF_ENCODE_FROM_EARLIER {
    // documented as 0x18,0x78,0x01,0x04 for R13, 0x18,0x78,0x01,0x05 for R14
    // r14:      7d f4 78 01
    // r2000:    14 64 78 01
    VERSION (R_13) {
      BITCODE_RC unknown_rc4[] = { 0x18, 0x78, 0x01, 0x04 };
      memcpy (_obj->unknown_rc4, unknown_rc4, 4);
    }
    VERSION (R_14) {
      BITCODE_RC unknown_rc4[] = { 0x18, 0x78, 0x01, 0x05 };
      memcpy (_obj->unknown_rc4, unknown_rc4, 4);
    }
    VERSION (R_2000) {
      BITCODE_RC unknown_rc4[] = { 0x14, 0x64, 0x78, 0x01 };
      memcpy (_obj->unknown_rc4, unknown_rc4, 4);
    }
  }
  
  FIELD_RL (size, 0);
  FIELD_BLx (address, 0);
  FIELD_TFF (version, 12, 0);
  FIELD_VECTOR_INL (null_b, B, 4, 0);
  FIELD_RC (unknown_10, 0); // 0x10
  if (
#if defined IS_ENCODER
      dat->version < R_2000
#else
      dat->from_version < R_2000
#endif
      && FIELD_VALUE (unknown_10) == 0x18)
    {
      FIELD_VECTOR_INL (unknown_rc4, RC, 2, 0)
    }
  else
    {
      FIELD_VECTOR_INL (unknown_rc4, RC, 4, 0)
    }
  FIELD_RC (num_sections, 0);
  REPEAT_F (num_sections, 6, sections, Dwg_SecondHeader_Sections)
  REPEAT_BLOCK
      // address+sizes of sections 0-2 is correct, 3+4 is empty
      SUB_FIELD_RC (sections[rcount1], nr, 0);
      SUB_FIELD_BLx (sections[rcount1], address, 0);
      SUB_FIELD_BL (sections[rcount1], size, 0);
  END_REPEAT_BLOCK
  END_REPEAT_F (sections)

  FIELD_BS (num_handlers, 0); // 14, resp. 16 in r14
  REPEAT_F (num_handlers, 16, handlers, Dwg_SecondHeader_Handlers)
  REPEAT_BLOCK
      SUB_FIELD_RC (handlers[rcount1], num_data, 0);
      SUB_FIELD_RC (handlers[rcount1], nr, 0);
      SUB_FIELD_VECTOR (handlers[rcount1], data, RC, num_data, 0);
  END_REPEAT_BLOCK
  END_REPEAT_F (handlers)

  // TODO: CRC check
  // crc = bit_read_CRC (dat);
  FIELD_RSx (crc, 0);
  VERSION (R_14) {
    FIELD_RL (junk_r14_1, 0);
    FIELD_RL (junk_r14_2, 0);
  }
}
