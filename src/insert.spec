/* -*- c -*- */
/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2026 Free Software Foundation, Inc.                        */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * insert.spec: the fields of an INSERT, shared with the ACIDBLOCKREFERENCE
 * of Inventor, which is an AcDbBlockReference with the same layout.
 * Included in the body of DWG_ENTITY (INSERT) and
 * DWG_ENTITY (ACIDBLOCKREFERENCE) in dwg.spec.
 */

  SUBCLASS (AcDbBlockReference);
#ifdef IS_DXF
    FIELD_RS0 (has_attribs, 66);
    FIELD_HANDLE_NAME (block_header, 2, BLOCK_HEADER);
#endif
  PRE (R_2_0b) {
    FIELD_TV (block_name, 2);
    FIELD_2RD (ins_pt, 10);
    FIELD_2RD_1 (scale, 41);
    FIELD_RD (rotation, 50);
  }
  VERSIONS (R_2_0b, R_11) {
    DECODER { FIELD_VALUE (has_attribs) = R11FLAG (FLAG_R11_HAS_ATTRIBS) ? 1 : 0; }
    FIELD_HANDLE (block_header, 2, 2);
    FIELD_2RD (ins_pt, 10)
#ifndef IS_JSON
    if (R11OPTS (1)) {
      FIELD_RD (scale.x, 41);
    }
    if (R11OPTS (2)) {
      FIELD_RD (scale.y, 42);
    }
    if (R11OPTS (4)) {
      FIELD_RD (rotation, 50);
    }
    if (R11OPTS (8)) {
      FIELD_RD (scale.z, 43);
    }
#else
    if (R11OPTS (1|2|8)) {
      FIELD_3RD (scale, 0);
    }
    if (R11OPTS (4)) {
      FIELD_RD (rotation, 50);
    }
#endif
    if (R11OPTS (16)) {
      FIELD_RS (num_cols, 70);
    }
    if (R11OPTS (32)) {
      FIELD_RS (num_rows, 71);
    }
    if (R11OPTS (64)) {
      FIELD_RD (col_spacing, 44);
    }
    if (R11OPTS (128)) {
      FIELD_RD (row_spacing, 45);
    }
    if (R11OPTS (256)) {
      FIELD_3RD	(extrusion, 210);
    }
  }
  SINCE (R_13b1) {
    FIELD_3DPOINT (ins_pt, 10);
  }
  VERSIONS (R_13b1, R_14)
    {
      FIELD_3BD_1 (scale, 41); // 42,43
    }
  SINCE (R_2000b)
    {
      JSON {
        FIELD_BB (scale_flag, 0);
        FIELD_3BD_1 (scale, 41);
      }
      DXF_OR_PRINT {
        if (_obj->scale.x != 1.0 || _obj->scale.y != 1.0 || _obj->scale.z != 1.0)
          FIELD_3BD_1 (scale, 41);
      }
      DECODER
        {
          FIELD_BB (scale_flag, 0);
          if (FIELD_VALUE (scale_flag) == 3)
            {
              FIELD_VALUE (scale.x) = 1.0;
              FIELD_VALUE (scale.y) = 1.0;
              FIELD_VALUE (scale.z) = 1.0;
            }
          else if (FIELD_VALUE (scale_flag) == 1)
            {
              FIELD_VALUE (scale.x) = 1.0;
              FIELD_DD (scale.y, 1.0, 42);
              FIELD_DD (scale.z, 1.0, 43);
            }
          else if (FIELD_VALUE (scale_flag) == 2)
            {
              FIELD_RD (scale.x, 41);
              FIELD_VALUE (scale.y) = FIELD_VALUE (scale.x);
              FIELD_VALUE (scale.z) = FIELD_VALUE (scale.x);
            }
          else //if (FIELD_VALUE (scale_flag) == 0)
            {
              FIELD_RD (scale.x, 41);
              FIELD_DD (scale.y, FIELD_VALUE (scale.x), 42);
              FIELD_DD (scale.z, FIELD_VALUE (scale.x), 43);
            }
          FIELD_3PT_TRACE (scale, DD, 41);
        }
      ENCODER
        {
          if (bit_eq_DD (FIELD_VALUE (scale.x), 1.0) &&
              bit_eq_DD (FIELD_VALUE (scale.y), 1.0) &&
              bit_eq_DD (FIELD_VALUE (scale.z), 1.0))
            {
              FIELD_VALUE (scale_flag) = 3;
              FIELD_BB (scale_flag, 0);
            }
          else if (bit_eq_DD (FIELD_VALUE (scale.x), FIELD_VALUE (scale.y)) &&
                   bit_eq_DD (FIELD_VALUE (scale.x), FIELD_VALUE (scale.z)))
            {
              FIELD_VALUE (scale_flag) = 2;
              FIELD_BB (scale_flag, 0);
              FIELD_RD (scale.x, 41);
            }
          else if (bit_eq_DD (FIELD_VALUE (scale.x), 1.0))
            {
              FIELD_VALUE (scale_flag) = 1;
              FIELD_BB (scale_flag, 0);
              FIELD_DD (scale.y, 1.0, 42);
              FIELD_DD (scale.z, 1.0, 43);
            }
          else
            {
              FIELD_VALUE (scale_flag) = 0;
              FIELD_BB (scale_flag, 0);
              FIELD_RD (scale.x, 41);
              FIELD_DD (scale.y, FIELD_VALUE (scale.x), 42);
              FIELD_DD (scale.z, FIELD_VALUE (scale.x), 43);
            }
          FIELD_3PT_TRACE (scale, DD, 41);
        }
    }

  PRE (R_13b1) {
    DXF { FIELD_BE (extrusion, 210); } // in DWG?
  }
  LATER_VERSIONS {
    FIELD_BD0 (rotation, 50);
    DXF {
      FIELD_BE (extrusion, 210);
    } else {
      FIELD_3DPOINT (extrusion, 0);
    }
    FIELD_B (has_attribs, 0); // 66 above
  }

  SINCE (R_2004a)
    {
      if (FIELD_VALUE (has_attribs))
        FIELD_BL (num_owned, 0);
    }

  COMMON_ENTITY_HANDLE_DATA;
  SINCE (R_13b1) {
    FIELD_HANDLE (block_header, 5, 0);
  }
  VERSIONS (R_13b1, R_2000)
    {
      if (FIELD_VALUE (has_attribs))
        {
          FIELD_HANDLE (first_attrib, 4, 0);
          FIELD_HANDLE (last_attrib, 4, 0);
        }
    }

  //Spec typo? Spec says "2004:" but I think it should be "2004+:"
  // just like field num_owned (AND just like in MINSERT)
  IF_FREE_OR_SINCE (R_2004a)
    {
      if (FIELD_VALUE (has_attribs))
        {
          HANDLE_VECTOR (attribs, num_owned, 4, 0);
        }
    }

  SINCE (R_13b1) {
    if (FIELD_VALUE (has_attribs)) {
      FIELD_HANDLE (seqend, 3, 0);
    }
  }
