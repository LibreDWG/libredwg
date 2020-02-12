/* -*- c -*- */
/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2018,2020 Free Software Foundation, Inc.                   */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * auxheader.spec: DWG file header specification
 * written by Reini Urban
 */

#include "spec.h"

  SINCE (R_2000) {
    IF_ENCODE_FROM_EARLIER {
      BITCODE_RS tmpunknown[] = {5, 0x893, 5, 0x893, 0, 1};
      FIELD_VALUE (aux_intro_1) = 0xff;
      FIELD_VALUE (aux_intro_2) = 0x77;
      FIELD_VALUE (aux_intro_3) = 0x01;
      FIELD_VALUE (minus_1) = -1;
      FIELD_VALUE (dwg_version) = dwg->header.dwg_version;
      FIELD_VALUE (maint_version) = dwg->header.maint_version;
      FIELD_VALUE (dwg_version_2) = dwg->header.dwg_version;
      FIELD_VALUE (maint_version_2) = dwg->header.maint_version;
      memcpy (FIELD_VALUE (unknown_6rs), tmpunknown, sizeof(tmpunknown));
      FIELD_VALUE (TDCREATE) = dwg->header_vars.TDCREATE.value;
      FIELD_VALUE (TDUPDATE) = dwg->header_vars.TDUPDATE.value;
      FIELD_VALUE (HANDSEED) = dwg->header_vars.HANDSEED
                                   ? dwg->header_vars.HANDSEED->absolute_ref
                                   : 0;
    }
  }

  FIELD_RCx (aux_intro_1, 0); /* ff 77 01 */
  FIELD_RCx (aux_intro_2, 0);
  FIELD_RCx (aux_intro_3, 0);
  FIELD_RSx (dwg_version, 0);
  FIELD_RS (maint_version, 0);
  FIELD_RL (numsaves, 0);
  FIELD_RL (minus_1, 0);
  FIELD_RS (numsaves_1, 0);
  FIELD_RS (numsaves_2, 0);
  FIELD_RL (zero, 0);
  FIELD_RSx (dwg_version_1, 0);
  FIELD_RS (maint_version_1, 0);
  FIELD_RSx (dwg_version_2, 0);
  FIELD_RS (maint_version_2, 0);
#ifdef IS_JSON
  FIELD_TFFx (unknown_6rs, 12, 0);
#else
  // TODO: inlined vector without malloc
  for (i = 0; i < 6; i++) {
    FIELD_RSx (unknown_6rs[i], 0); /* 5 0x893 5 0x893 0 1 */
  }
#endif
#ifdef IS_JSON
  FIELD_TFFx (unknown_5rl, 20, 0)
#else
  for (i = 0; i < 5; i++) {
    FIELD_RL (unknown_5rl[i], 0);
  }
#endif
  //DEBUG_HERE;
  FIELD_RD (TDCREATE, 0);
  //DEBUG_HERE;
  FIELD_RD (TDUPDATE, 0);
  FIELD_RLx (HANDSEED, 0);
  FIELD_RL (plot_stamp, 0);
  FIELD_RS (zero_1, 0);
  FIELD_RS (numsaves_3, 0);
  FIELD_RL (zero_2, 0);
  FIELD_RL (zero_3, 0);
  FIELD_RL (zero_4, 0);
  FIELD_RL (numsaves_4, 0);
  FIELD_RL (zero_5, 0);
  FIELD_RL (zero_6, 0);
  FIELD_RL (zero_7, 0);
  FIELD_RL (zero_8, 0);
  //DEBUG_HERE;
  SINCE (R_2018) {
#ifdef IS_JSON
    FIELD_TFFx (zero_18, 6, 0);
#else
    for (i = 0; i < 3; i++) {
      FIELD_RS (zero_18[i], 0);
    }
#endif
  }
