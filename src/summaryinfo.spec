/* -*- c -*- */
/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2019 Free Software Foundation, Inc.                        */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * summaryinfo.spec: DWG file AcDb::SummaryInfo section specification
 * written by Reini Urban
 */

  #include "spec.h"

  SINCE (R_2004) {
    IF_ENCODE_FROM_EARLIER {
      FIELD_VALUE (TDINDWG)    = dwg->header_vars.TDINDWG;
      FIELD_VALUE (TDCREATE)   = dwg->header_vars.TDCREATE;
      FIELD_VALUE (TDUPDATE)   = dwg->header_vars.TDUPDATE;
      if (dat->version < R_2007 ||       /* if 2004 -> 2004: TV -> TV */
          dat->from_version >= R_2007) { /* or already TU */
        FIELD_VALUE (HYPERLINKBASE) = (BITCODE_TU)dwg->header_vars.HYPERLINKBASE;
      }
      else { /* 2004 -> 2007+ */
        FIELD_VALUE (HYPERLINKBASE) = bit_utf8_to_TU (dwg->header_vars.HYPERLINKBASE, 0);
      }
    }
  }

  FIELD_TU16 (TITLE, 1);
  FIELD_TU16 (SUBJECT, 1);
  FIELD_TU16 (AUTHOR, 1);
  FIELD_TU16 (KEYWORDS, 1);
  FIELD_TU16 (COMMENTS, 1);
  FIELD_TU16 (LASTSAVEDBY, 1);
  FIELD_TU16 (REVISIONNUMBER, 1);
  FIELD_TU16 (HYPERLINKBASE, 1);
  //DEBUG_HERE;
  FIELD_TIMERLL (TDINDWG, 0);
  FIELD_TIMERLL (TDCREATE, 0);
  FIELD_TIMERLL (TDUPDATE, 0);

  // CustomPropertyInfo str pair's
  FIELD_RS (num_props, 0);
  REPEAT (num_props, props, Dwg_SummaryInfo_Property)
  REPEAT_BLOCK
    FIELD_TU16 (props[rcount1].tag, 0);   // CUSTOMPROPERTYTAG
    FIELD_TU16 (props[rcount1].value, 0); // CUSTOMPROPERTY
  END_REPEAT_BLOCK
  END_REPEAT (props)

  FIELD_RL (unknown1, 0);
  FIELD_RL (unknown2, 0);
