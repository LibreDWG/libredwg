/* -*- c -*- */
/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2023-2024 Free Software Foundation, Inc.                   */
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
  FIELD_RL (size, 0);
#ifdef IS_DECODER
  _VECTOR_CHKCOUNT_STATIC(size, _obj->size, 8, dat)
#endif
  FIELD_BL (address, 0);
  FIELD_TFF (version, 11, 0);
  FIELD_RC (is_maint, 0);
  FIELD_RC (zero_one_or_three, 0);
  FIELD_BSx (dwg_versions, 0);
  DECODER {
    LOG_TRACE("=> header dwg_version: 0x%x\n", _obj->dwg_versions & 0xFF);
    LOG_TRACE("=> header maint_version: 0x%x\n", (_obj->dwg_versions >> 8) & 0xFF);
  }
  FIELD_RS (codepage, 0);
  FIELD_BS (num_sections, 0);
  VALUEOUTOFBOUNDS (num_sections, 6);
  REPEAT_F (num_sections, 6, sections, Dwg_SecondHeader_Sections)
  REPEAT_BLOCK
      // address+sizes of sections 0-2 is correct, 3+4 is empty
      SUB_FIELD_RCd (sections[rcount1], nr, 0);
      LOG_TRACE (" %s\n", rcount1 < 6 ? dwg_section_name (dwg, rcount1) : "");
      SUB_VALUEOUTOFBOUNDS (handles[rcount1], nr, 6);
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
  VALUEOUTOFBOUNDS (num_handles, 14);
  REPEAT_F (num_handles, 14, handles, Dwg_SecondHeader_Handles)
  REPEAT_BLOCK
      SUB_FIELD_RCd (handles[rcount1], num_hdl, 0); // max 8, the size
      SUB_VALUEOUTOFBOUNDS (handles[rcount1], num_hdl, 8);
      SUB_FIELD_RCd (handles[rcount1], nr, 0);
      SUB_VALUEOUTOFBOUNDS (handles[rcount1], nr, 13);
      SUB_FIELD_VECTOR_INL (handles[rcount1], hdl, RC, _obj->handles[rcount1].num_hdl, 0);
      // log this handle backup similar to real handles
      if (_obj->handles[rcount1].name && DWG_LOGLEVEL >= DWG_LOGLEVEL_TRACE)
        {
          LOG_TRACE ("[%u] %s: 0.%hu.", (unsigned)rcount1,
                     _obj->handles[rcount1].name,
                     _obj->handles[rcount1].num_hdl);
          for (int i = 0; i < MIN (_obj->handles[rcount1].num_hdl, 8); i++)
            LOG_TRACE ("%hX", _obj->handles[rcount1].hdl[i]);
          LOG_TRACE ("\n")
        }
  END_REPEAT_BLOCK
  END_REPEAT_F (handles)
}
