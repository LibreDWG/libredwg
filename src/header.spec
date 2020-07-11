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
 * header.spec: DWG file header specification
 * written by Reini Urban
 */

#include "spec.h"

// char version[6] handled separately. older releases just had a version[12]
#ifdef IS_JSON
  KEY (zero_5);
  fprintf (dat->fh, "[ %d, %d, %d, %d, %d ]",
          _obj->zero_5[0], _obj->zero_5[1], _obj->zero_5[2], _obj->zero_5[3],
          _obj->zero_5[4]);
#else
  for (i = 0; i < 5; i++) {
    FIELD_RC (zero_5[i], 0);
  }
#endif
  FIELD_RC (is_maint, 0);

  PRE (R_13) {
    FIELD_RC (zero_one_or_three, 0); // 1,3,0,5,0,0x00cd,0
    for (i = 0; i < 3; i++) { // 3, 5, 205
      FIELD_RS (unknown_s[i], 0);
    }
    FIELD_RC (dwg_version, 0); // 0
  } LATER_VERSIONS {
    FIELD_RC (zero_one_or_three, 0);
    FIELD_RL (thumbnail_address, 0); //@0x0d
    FIELD_RC (dwg_version, 0); // of app which stored it. eg. SaveAs
    FIELD_RC (maint_version, 0);
    FIELD_RS (codepage, 0); //@0x13: 29/30 for ANSI_1252, since r2007 UTF-16
  }

  /* Until R_2004 here follows the sections */

  SINCE (R_2004) {
    IF_ENCODE_FROM_EARLIER {
      FIELD_VALUE (app_dwg_version) = FIELD_VALUE (dwg_version);
      FIELD_VALUE (app_maint_version) = FIELD_VALUE (maint_version);
      FIELD_VALUE (r2004_header_address) = 0x80;
    }
    FIELD_RC (unknown_0, 0);
    FIELD_RC (app_dwg_version, 0);
    FIELD_RC (app_maint_version, 0);
    FIELD_RL (security_type, 0);
    FIELD_RL (rl_1c_address, 0); /* mostly 0 */
    FIELD_RL (summaryinfo_address, 0);
    FIELD_RL (vbaproj_address, 0);
    FIELD_RL (r2004_header_address, 0); /* mostly 128/0x80 */
    ENCODER {
      for (i = 0; i < 54; i++)
        bit_write_RC (dat, 0);
    }
    else {
      dat->byte += 54; // empty slack
    }
    /* at 0x80 follows the encrypted r2004_header */
  }
