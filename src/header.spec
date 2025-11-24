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

  // char version[11] handled separately
  FIELD_RC (is_maint, 0);

  VERSIONS (R_2_0b, R_13b1) {
    FIELD_RC (zero_one_or_three, 0); // 1,3,5,0xcd,0
    /*
       AC1.50: 3, 5, 74; 3, 5, 83;
       AC2.10: 3, 5, 83
       AC1001: 3, 5, 101
       AC1002: 3, 5, 104; 3, 5, 114
       AC1003: 3, 5, 120; 3, 5, 122
       AC1004: 3, 5, 129
       AC1006: 3, 5, 158; 3, 5, 160
       AC1009: 3, 5, 204; 3, 5, 205
     */
    FIELD_RS (numentity_sections, 0); // 3
    FIELD_CAST (sections, RS, RL, 0); // 5 or 6. the real size is num_sections
    FIELD_RS (numheader_vars, 0); // 74,83,101,104,114,120,122,129,158,160,204,205
    FIELD_RC (dwg_version, 0); // 0

    // The 3 entity data sections
    FIELD_RLx (entities_start, 0);
    FIELD_RLx (entities_end, 0);
    FIELD_RLx (blocks_start, 0);
    FIELD_RLx (blocks_size, 0);
    FIELD_RLx (extras_start, 0);
    FIELD_RLx (extras_size, 0);
  }
  SINCE (R_13b1) {
    FIELD_RC (zero_one_or_three, 0);
    FIELD_RL (thumbnail_address, 0); //@0x0d
    FIELD_RC (dwg_version, 0); // of app which stored it. eg. SaveAs
    FIELD_RC (maint_version, 0);
    ENCODER {
       if (!FIELD_VALUE (codepage))
         FIELD_VALUE (codepage) = dat->codepage;
    }
    FIELD_RS (codepage, 0); //@0x13: 29/30 for ANSI_1252, since r2007 UCS-16
    LOG_TRACE ("%s\n", dwg_codepage_dxfstr ((Dwg_Codepage)FIELD_VALUE (codepage)))
    DECODER {
      dat->codepage = FIELD_VALUE (codepage);
    }
    PRE (R_2004a) {
      FIELD_RL (sections, 0); // 3 or 5 or 6. the real size is num_sections
      // de-/encode the section locator records extra:
      // REPEAT sections: RC number, RL address, RL size
      // RS CRC
    }
  }

  /* Until R_2004 here follows the sections */
  SINCE (R_2004a) {
    IF_ENCODE_FROM_EARLIER {
      FIELD_VALUE (app_dwg_version) = FIELD_VALUE (dwg_version);
      FIELD_VALUE (app_maint_version) = FIELD_VALUE (maint_version);
      FIELD_VALUE (r2004_header_address) = 0x80;
    }
    FIELD_RC (unknown_0, 0);
    FIELD_RC (app_dwg_version, 0);
    FIELD_RC (app_maint_version, 0);
    FIELD_RL (security_type, 0); /* 0x0001 = encrypt data (for all sections but
				             AcDb:Preview and AcDb:SummaryInfo)
				    0x0002 = encrypt properties (for AcDb:Preview
				             and AcDb:SummaryInfo)
				    0x0010 = sign data
				    0x0020 = add timestamp */
    FIELD_RL (rl_1c_address, 0); /* mostly 0, probably some unused section */
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
