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

#ifdef IS_ENCODER
#define ENCODER if (1)
#define DECODER if (0)
#endif

#ifdef IS_DECODER
#define ENCODER if (0)
#define DECODER if (1)
#undef IF_ENCODE_FROM_EARLIER
#define IF_ENCODE_FROM_EARLIER if (0)
#endif

#ifdef IS_PRINT
#define ENCODER if (0)
#define DECODER if (0)
#undef IF_ENCODE_FROM_EARLIER
#define IF_ENCODE_FROM_EARLIER if (0)
#endif


  // char version[6] handled seperately
  for (i=0; i<5; i++) {
    FIELD_RC(zero_5[i]);
  }
  FIELD_RC(is_maint);
  FIELD_RC(zero_one_or_three);
  FIELD_RL(preview_addr);
  FIELD_RC(dwg_version);
  FIELD_RC(maint_version);
  FIELD_RS(codepage);

  /* Until R_2004 here follows the sections */

  SINCE(R_2004) {
    IF_ENCODE_FROM_EARLIER {
      dwg->header.unknown_3[1] = 0x1f; /* dwg_version */
      dwg->header.unknown_3[2] = 0x8;  /* maint_version */
      dwg->header.rl_28_80 = 0x80;
    }
    for (i = 0; i < 3; i++) {
      FIELD_RC(unknown_3[i]);
    }
    FIELD_RL(security_type);
    FIELD_RL(rl_1c_address);
    FIELD_RL(summary_info_address);
    FIELD_RL(vba_proj_address);
    FIELD_RL(rl_28_80);
    ENCODER {
      for (i = 0; i < 54; i++)
        bit_write_RC(dat, 0);
    }
    else {
      dat->byte += 54;
    }
    /* now at 0x80 follows the encrypted header data */
  }





