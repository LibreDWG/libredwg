/* -*- c -*- */
/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2018-2024 Free Software Foundation, Inc.                   */
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

  SINCE (R_2000b) {
    IF_ENCODE_FROM_EARLIER {
      BITCODE_RS def_unknown_6rs[] = { 4, 0x565, 0, 0, 2, 1 };
      BITCODE_RL def_unknown_5rl[] = { 0, 0, 0, 256, 393218 };
      FIELD_VALUE (aux_intro[0]) = 0xff;
      FIELD_VALUE (aux_intro[1]) = 0x77;
      FIELD_VALUE (aux_intro[2]) = 0x01;
      FIELD_VALUE (minus_1) = -1;
      FIELD_VALUE (dwg_version) = dwg->header.dwg_version;
      FIELD_VALUE (maint_version) = dwg->header.maint_version;
      FIELD_VALUE (dwg_version_2) = dwg->header.dwg_version;
      FIELD_VALUE (maint_version_2) = dwg->header.maint_version;
      memcpy (FIELD_VALUE (unknown_6rs), def_unknown_6rs, sizeof(def_unknown_6rs));
      memcpy (FIELD_VALUE (unknown_5rl), def_unknown_5rl, sizeof(def_unknown_5rl));
      memcpy (&_obj->TDCREATE, &dwg->header_vars.TDCREATE, sizeof(BITCODE_TIMERLL));
      memcpy (&_obj->TDUPDATE, &dwg->header_vars.TDUPDATE, sizeof(BITCODE_TIMERLL));
      FIELD_VALUE (HANDSEED) = dwg->header_vars.HANDSEED
                                   ? dwg->header_vars.HANDSEED->absolute_ref
                                   : 0;
    }
  }

  FIELD_VECTOR_INL (aux_intro, RC, 3, 0) /* ff 77 01 */
  FIELD_RSx (dwg_version, 0);
#ifndef IS_JSON
  UNTIL (R_2013) {
    FIELD_CAST (maint_version, RS, RLx, 0);
  } LATER_VERSIONS
#endif
    FIELD_RLx (maint_version, 0);
  FIELD_RL (numsaves, 0);
  FIELD_RLd (minus_1, 0);
  FIELD_RS (numsaves_1, 0);
  FIELD_RS (numsaves_2, 0);
  FIELD_RL (zero, 0);
  FIELD_RSx (dwg_version_1, 0);
#ifndef IS_JSON
  UNTIL (R_2013) {
    FIELD_CAST (maint_version_1, RS, RLx, 0);
  } LATER_VERSIONS
#endif
    FIELD_RLx (maint_version_1, 0);
  FIELD_RSx (dwg_version_2, 0);
#ifndef IS_JSON
  UNTIL (R_2013) {
    FIELD_CAST (maint_version_2, RS, RLx, 0);
  } LATER_VERSIONS
#endif
    FIELD_RLx (maint_version_2, 0);
  FIELD_VECTOR_INL (unknown_6rs, RS, 6, 0)
  FIELD_VECTOR_INL (unknown_5rl, RL, 5, 0)
  FIELD_TIMERLL (TDCREATE, 0);
  FIELD_TIMERLL (TDUPDATE, 0);
  FIELD_HV (HANDSEED, 0);
  FIELD_RL (plot_stamp, 0);
  FIELD_RS (zero_1, 0);
  FIELD_RS (numsaves_3, 0);
  FIELD_RL (zero_2, 0);
  FIELD_RL (zero_3, 0);
  FIELD_RL (zero_4, 0);
  FIELD_RL (numsaves_4, 0);
  FIELD_RL (zero_5, 0);
  FIELD_RL (zero_6, 0);
  SINCE (R_2004a) {
    FIELD_RL (zero_7, 0);
    FIELD_RL (zero_8, 0);
  }
  SINCE (R_2018) {
    FIELD_VECTOR_INL (zero_18, RS, 3, 0)
  }
