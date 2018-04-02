/* -*- c -*- */
/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2018 Free Software Foundation, Inc.                        */
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

  SINCE(R_2000) {
    IF_ENCODE_FROM_EARLIER {
      BITCODE_RS tmpunknown[] = {5, 0x893, 5, 0x893, 0, 1};
      FIELD_VALUE(aux_intro[0]) = 0xff;
      FIELD_VALUE(aux_intro[1]) = 0x77;
      FIELD_VALUE(aux_intro[2]) = 0x01;
      FIELD_VALUE(minus_1) = -1;
      FIELD_VALUE(dwg_version) = dwg->header.dwg_version;
      FIELD_VALUE(maint_version) = dwg->header.maint_version;
      memcpy(FIELD_VALUE(unknown_rs),tmpunknown,sizeof(tmpunknown));
      FIELD_VALUE(TDCREATE)   = dwg->header_vars.TDCREATE;
      FIELD_VALUE(TDUPDATE)   = dwg->header_vars.TDUPDATE;
    }
  }

  for (i=0; i<3; i++) {
    FIELD_RC(aux_intro[i], 0); /* ff 77 01 */
  }
  FIELD_RS (dwg_version, 0); /* AC1010 = 17 ... AC1032 = 33 */
  FIELD_RS (maint_version, 0);
  FIELD_RL (num_saves, 0);
  FIELD_RL (minus_1, 0);
  FIELD_RS (num_saves_1, 0);
  FIELD_RS (num_saves_2, 0);
  FIELD_RL (zero, 0);
  FIELD_RS (dwg_version_1, 0);
  FIELD_RS (maint_version_1, 0);
  FIELD_RS (dwg_version_2, 0);
  FIELD_RS (maint_version_2, 0);
  for (i=0; i<6; i++) {
    #undef FORMAT_RS
    #define FORMAT_RS "0x%" PRIx16
    FIELD_RS(unknown_rs[i], 0); /* 5 0x893 5 0x893 0 1 0 0 0 0 */
    #undef FORMAT_RS
    #define FORMAT_RS "%" PRIu16
  }
  for (i=0; i<5; i++) { // documented as 5xRL
    FIELD_RS(zero_rs[i], 0);
  }
  FIELD_TIMEBLL(TDCREATE, 0);
  FIELD_TIMEBLL(TDUPDATE, 0);
  FIELD_RL(HANDSEED, 0);
  FIELD_RL(plot_stamp, 0);
  FIELD_RS(zero_1, 0);
  FIELD_RS(num_saves_3, 0);
  FIELD_RL(zero_2, 0);
  FIELD_RL(zero_3, 0);
  FIELD_RL(zero_4, 0);
  FIELD_RL(num_saves_4, 0);
  FIELD_RL(zero_5, 0);
  FIELD_RL(zero_6, 0);
  FIELD_RL(zero_7, 0);

  SINCE(R_2018) {
    for (i = 0; i < 3; i++) {
      FIELD_RS(zero_18[i], 0);
    }
  }
