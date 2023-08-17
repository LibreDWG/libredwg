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
  FIELD_BL (address, 0);
  FIELD_TFF (version, 11, 0);
#ifndef IS_JSON
  VALUE_RC (0xf, 0);
#endif
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
  else if (FIELD_VALUE (unknown_10) == 0x14)
    {
      FIELD_VECTOR_INL (unknown_rc4, RC, 3, 0)
    }
  else // 0x10
    {
      FIELD_VECTOR_INL (unknown_rc4, RC, 4, 0)
    }
  FIELD_RCu (num_sections, 0);
  REPEAT_F (num_sections, 6, sections, Dwg_SecondHeader_Sections)
  REPEAT_BLOCK
      // address+sizes of sections 0-2 is correct, 3+4 is empty
      SUB_FIELD_RCd (sections[rcount1], nr, 0);
      SUB_FIELD_BL (sections[rcount1], address, 0);
      SUB_FIELD_BL (sections[rcount1], size, 0);
  END_REPEAT_BLOCK
  END_REPEAT_F (sections)

  /* 0: handseed
     1: block control objhandle
     2: layer control objhandle
     3: style control objhandle
     4: ltype control objhandle
     5: view control objhandle
     6: ucs control objhandle
     7: vport control objhandle
     8: appid control objhandle
     9: dimstyle control objhandle
     10: vx control objhandle
     11: dictionary objhandle
     12: mlstyle objhandle
     13: group dictionary objhandle
   */
  FIELD_BS (num_handles, 0); // always 14
  REPEAT_F (num_handles, 14, handles, Dwg_SecondHeader_Handles)
  REPEAT_BLOCK
      SUB_FIELD_RCd (handles[rcount1], num_hdl, 0); // max 8, the size
      SUB_FIELD_RCd (handles[rcount1], nr, 0);
      SUB_FIELD_VECTOR (handles[rcount1], hdl, RC, num_hdl, 0);
      // log this handle backup similar to real handles
      if (_obj->handles[rcount1].name && DWG_LOGLEVEL >= DWG_LOGLEVEL_TRACE)
        {
          LOG_TRACE ("[%u] %s: 0.%hu.", (unsigned)rcount1,
                     _obj->handles[rcount1].name,
                     _obj->handles[rcount1].num_hdl)
          for (int i = 0; i < _obj->handles[rcount1].num_hdl; i++)
            LOG_TRACE ("%hX", _obj->handles[rcount1].hdl[i]);
          LOG_TRACE ("\n")
        }
  END_REPEAT_BLOCK
  END_REPEAT_F (handles)

  // CRC check extra
  FIELD_RSx (crc, 0);
  VERSION (R_14) {
    FIELD_RLL (junk_r14, 0);
  }
}
