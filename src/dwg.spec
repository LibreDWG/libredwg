/* -*- c -*- */
/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2009-2010,2018-2023 Free Software Foundation, Inc.         */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * dwg.spec: DWG entities and objects specification
 * written by Felipe Corrêa da Silva Sances
 * modified by Rodrigo Rodrigues da Silva
 * modified by Till Heuschmann
 * modified by Reini Urban
 * modified by Denis Pruchkovsky
 * modified by Michal Josef Špaček
 */

#include "spec.h"

/* (1/7) */
DWG_ENTITY (TEXT)

  SUBCLASS (AcDbText)
  PRE (R_13b1) {
    FIELD_2RD (ins_pt, 10);
    FIELD_RD (height, 40);
    PRE (R_2_0)
      FIELD_RD (oblique_angle, 51);
    FIELD_TV (text_value, 1);
    if (R11OPTS (1))
      FIELD_RD (rotation, 50);
    if (R11OPTS (2))
      FIELD_RD (width_factor, 41);
    if (R11OPTS (4))
      FIELD_RD (oblique_angle, 51);
    if (R11OPTS (8))
      FIELD_HANDLE0 (style, 1, 7);
    if (R11OPTS (16)) {
      FIELD_CAST (generation, RC, BS, 71);
      LOG_TEXT_GENERATION
    }
    if (R11OPTS (32)) {
      FIELD_CAST (horiz_alignment, RC, BS, 72);
      LOG_HORIZ_ALIGNMENT
    }
    if (R11OPTS (64))
      FIELD_2RD (alignment_pt, 11);
    if (R11OPTS (128))
      FIELD_3RD (extrusion, 210);
    if (R11OPTS (256)) {
      FIELD_CAST (vert_alignment, RC, BS, 73);
      LOG_VERT_ALIGNMENT
    }
  }
  VERSIONS (R_13b1, R_14)
    {
      DXF {
        FIELD_BD0 (thickness, 39);
        FIELD_2RD (ins_pt, 10);
        FIELD_BD (elevation, 30);
        FIELD_BD (height, 40);
        FIELD_TV (text_value, 1);
        FIELD_BD0 (rotation, 50);
        FIELD_BD1 (width_factor, 41);
        FIELD_BD0 (oblique_angle, 51);
        FIELD_HANDLE0 (style, 5, 7);
        FIELD_BS (generation, 71);
        FIELD_BS (horiz_alignment, 72);
        FIELD_2RD (alignment_pt, 11);
        FIELD_BE (extrusion, 210);
        SUBCLASS (AcDbText)
      } else {
        FIELD_BD (elevation, 30);
        FIELD_2RD (ins_pt, 10);
        FIELD_2RD (alignment_pt, 11);
        FIELD_3BD (extrusion, 210);
        FIELD_BD0 (thickness, 39);
        FIELD_BD0 (oblique_angle, 51);
        FIELD_BD0 (rotation, 50);
        FIELD_BD (height, 40);
        FIELD_BD (width_factor, 41);
        FIELD_TV (text_value, 1);
        FIELD_BS (generation, 71);
        LOG_TEXT_GENERATION
        FIELD_BS (horiz_alignment, 72);
        LOG_HORIZ_ALIGNMENT
        FIELD_BS (vert_alignment, 73);
        LOG_VERT_ALIGNMENT
      }
    }

  IF_FREE_OR_SINCE (R_2000)
    {
      BITCODE_RC dataflags;
#ifdef IS_ENCODER
      if (dat->from_version < R_2000)
        dwg_set_dataflags (obj);
#endif
      FIELD_RC (dataflags, 0);
      dataflags = FIELD_VALUE (dataflags);
      DECODER_OR_ENCODER {
        dwg_log_dataflags (DWG_LOGLEVEL, DWG_LOGLEVEL_TRACE, dataflags);
      }

      DXF {
        FIELD_BT0 (thickness, 39);
        FIELD_2RD (ins_pt, 10);
        FIELD_RD (elevation, 30);
      } else {
        if (!(dataflags & 0x01))
          FIELD_RD (elevation, 0);
        FIELD_2RD (ins_pt, 10);
      }
      DXF {
        FIELD_RD (height, 40);
        FIELD_T (text_value, 1);
      } else {
        if (!(dataflags & 0x02))
          FIELD_2DD (alignment_pt, ins_pt, 0);
        FIELD_BE (extrusion, 0);
        FIELD_BT (thickness, 0);
      }
      DXF {
        FIELD_RD0 (rotation, 50);
        FIELD_RD1 (width_factor, 41);
        FIELD_RD0 (oblique_angle, 51);
        FIELD_HANDLE0 (style, 5, 7);
      } else {
        if (!(dataflags & 0x04))
          FIELD_RD (oblique_angle, 0);
        if (!(dataflags & 0x08))
          FIELD_RD (rotation, 0);
        FIELD_RD (height, 0);
        if (!(dataflags & 0x10)) {
          FIELD_RD (width_factor, 0);
        }
#ifdef IS_DECODER
        else
          FIELD_VALUE (width_factor) = 1.0;
#endif
        FIELD_T (text_value, 0);
      }

      if (!(dataflags & 0x20)) {
        FIELD_BS (generation, 71);
        LOG_TEXT_GENERATION
      }
      if (!(dataflags & 0x40)) {
        FIELD_BS (horiz_alignment, 72);
        LOG_HORIZ_ALIGNMENT
      }
      if (!(dataflags & 0x80)) {
        FIELD_BS0 (vert_alignment, 0);
        LOG_VERT_ALIGNMENT
      }

      DXF {
        FIELD_2DD (alignment_pt, ins_pt, 11);
        FIELD_RD (elevation, 31);
        FIELD_BE (extrusion, 210);
        SUBCLASS (AcDbText) // not in ODA, but ACAD
        FIELD_BS0 (vert_alignment, 73);
      }
    }

  COMMON_ENTITY_HANDLE_DATA;
  SINCE (R_13b1)
    {
      IF_ENCODE_FROM_PRE_R13 {
        //FIXME: should really just lookup the style table; style is the index.
        FIELD_VALUE (style) = 0; //dwg_resolve_handle (dwg, obj->style->r11_idx);
      }
#ifndef IS_DXF
      FIELD_HANDLE (style, 5, 7);
#endif
    }

DWG_ENTITY_END

/* (2/16) */
DWG_ENTITY (ATTRIB)

  SUBCLASS (AcDbText)
  PRE (R_13b1)
    {
      FIELD_2RD (ins_pt, 10);
      FIELD_RD (height, 40);
      FIELD_TV (text_value, 1);
#ifdef IS_ENCODER
      if (FIELD_VALUE (tag) && !dwg_is_valid_tag (FIELD_VALUE (tag))) {
        LOG_WARN ("Fixup invalid tag %s", FIELD_VALUE (tag));
        FIELD_VALUE (tag) = fixup_invalid_tag (dat, FIELD_VALUE (tag));
        LOG_WARN ("to %s", FIELD_VALUE (tag));
      }
#endif
      FIELD_TV (tag, 2);
      DECODER {
        if (FIELD_VALUE (tag) && !dwg_is_valid_tag (FIELD_VALUE (tag)))
          LOG_WARN ("Invalid tag %s", FIELD_VALUE (tag))
      }
      FIELD_RC (flags, 70); // 1 invisible, 2 constant, 4 verify, 8 preset
      LOG_FLAG_ATTRIB
      if (R11OPTS (2)) {
        FIELD_RD (rotation, 50);
      }
      if (R11OPTS (4)) {
        FIELD_RD (width_factor, 41);
      }
      if (R11OPTS (8)) {
        FIELD_RD (oblique_angle, 51);
      }
      if (R11OPTS (16)) {
        FIELD_HANDLE0 (style, 1, 7);
      }
      if (R11OPTS (32)) {
        FIELD_CAST (generation, RC, BS, 71);
        LOG_TEXT_GENERATION
      }
      if (R11OPTS (64)) {
        FIELD_CAST (horiz_alignment, RC, BS, 72);
        LOG_HORIZ_ALIGNMENT
      }
      if (R11OPTS (128)) {
        FIELD_2RD (alignment_pt, 11);
      }
      if (R11OPTS (0x100))
        FIELD_3RD (extrusion, 210);
      if (R11OPTS (0x200)) {
        FIELD_CAST (vert_alignment, RC, BS, 74);
        LOG_VERT_ALIGNMENT
      }
    }
  VERSIONS (R_13b1, R_14)
    {
      DXF {
        FIELD_2RD (ins_pt, 10);
        FIELD_RD (elevation, 30);
      } else {
        FIELD_BD (elevation, 30);
        FIELD_2RD (ins_pt, 10);
      }
      FIELD_2RD (alignment_pt, 11);
      FIELD_3BD (extrusion, 210);
      FIELD_BD (thickness, 39);
      FIELD_BD0 (oblique_angle, 51);
      DXF {
        FIELD_HANDLE0 (style, 5, 7);
      }
      FIELD_BD0 (rotation, 50);
      FIELD_BD0 (height, 40);
      FIELD_BD1 (width_factor, 41);
      FIELD_TV (text_value, 1);
      FIELD_BS0 (generation, 71);
      LOG_TEXT_GENERATION
      FIELD_BS0 (horiz_alignment, 72);
      LOG_HORIZ_ALIGNMENT
      FIELD_BS0 (vert_alignment, 0);
      LOG_VERT_ALIGNMENT
    }

  IF_FREE_OR_SINCE (R_2000)
    {
      BITCODE_RC dataflags;
#ifdef IS_ENCODER
      if (dat->from_version < R_2000)
        dwg_set_dataflags (obj);
#endif
      FIELD_RC (dataflags, 0);
      dataflags = FIELD_VALUE (dataflags);
      DECODER_OR_ENCODER {
        dwg_log_dataflags (DWG_LOGLEVEL, DWG_LOGLEVEL_TRACE, dataflags);
      }

      DXF {
        FIELD_BT0 (thickness, 39);
        FIELD_2RD (ins_pt, 10);
        FIELD_RD (elevation, 30);
      } else {
        if (!(dataflags & 0x01))
          FIELD_RD (elevation, 0);
        FIELD_2RD (ins_pt, 10);
      }
      DXF {
        FIELD_RD (height, 40);
        FIELD_T (text_value, 1);
      } else {
        if (!(dataflags & 0x02))
          FIELD_2DD (alignment_pt, ins_pt, 0);
        FIELD_BE (extrusion, 0);
        FIELD_BT (thickness, 0);
      }
      DXF {
        FIELD_RD0 (rotation, 50);
        FIELD_RD1 (width_factor, 41);
        FIELD_RD0 (oblique_angle, 51);
        FIELD_HANDLE0 (style, 5, 7);
      } else {
        if (!(dataflags & 0x04))
          FIELD_RD (oblique_angle, 51);
        if (!(dataflags & 0x08))
          FIELD_RD (rotation, 50);
        FIELD_RD (height, 40);
        if (!(dataflags & 0x10)) {
          FIELD_RD (width_factor, 41);
        }
#ifdef IS_DECODER
        else
          FIELD_VALUE (width_factor) = 1.0;
#endif
        FIELD_T (text_value, 1);
      }

      if (!(dataflags & 0x20)) {
        FIELD_BS (generation, 71);
        LOG_TEXT_GENERATION
      }
      if (!(dataflags & 0x40)) {
        FIELD_BS (horiz_alignment, 72);
        LOG_HORIZ_ALIGNMENT
      }
      if (!(dataflags & 0x80)) {
        FIELD_BS (vert_alignment, 0);
        LOG_VERT_ALIGNMENT
      }
      DXF {
        FIELD_2DD (alignment_pt, ins_pt, 11);
        FIELD_RD (elevation, 31);
        FIELD_BE (extrusion, 210);
      }
    }

  SUBCLASS (AcDbAttribute)
  DXF {
    FIELD_T (tag, 2);
    FIELD_RC (type, 70);
    //FIELD_BS (field_length, 73);
    FIELD_BS0 (vert_alignment, 74);
    LOG_VERT_ALIGNMENT
    SINCE (R_2004) {
      FIELD_RC (class_version, 280);
    }
  }
  SINCE (R_2010)
    {
      FIELD_RC (class_version, 0); // 0 = r2010
      VALUEOUTOFBOUNDS (class_version, 10)
    }
  SINCE (R_2018)
    {
      FIELD_RC (type, 0); // 1=single line, 2=multi line attrib, 4=multi line attdef
      if (FIELD_VALUE (type) > 1)
        {
          SUBCLASS (AcDbMText)
          LOG_WARN ("MTEXT fields")
          // TODO fields handles to MTEXT entities. how many?
          FIELD_HANDLE (mtext_handles, 0, 340); //TODO

          FIELD_BS (annotative_data_size, 70);
          if (FIELD_VALUE (annotative_data_size) > 1)
            {
              FIELD_RC (annotative_data_bytes, 0);
              FIELD_HANDLE (annotative_app, 0, 0); //TODO
              FIELD_BS (annotative_short, 0);
            }
        }
    }

  SINCE (R_13b1)
    {
      FIELD_T (tag, 0);
      FIELD_BS0 (field_length, 0);
      FIELD_RC (flags, 0); // 1 invisible, 2 constant, 4 verify, 8 preset
    }

  SINCE (R_2007) {
    FIELD_B (lock_position_flag, 0); // 70
  }

  COMMON_ENTITY_HANDLE_DATA;
  SINCE (R_13b1) {
    FIELD_HANDLE (style, 5, 0); // unexpected here in DXF
  }

DWG_ENTITY_END

/* (3/15) */
DWG_ENTITY (ATTDEF)

  SUBCLASS (AcDbText)
  PRE (R_13b1)
    {
      FIELD_2RD (ins_pt, 10);
      FIELD_RD (height, 40);
      FIELD_TV (default_value, 1);
      FIELD_TV (prompt, 3);
#ifdef IS_ENCODER
      if (FIELD_VALUE (tag) && !dwg_is_valid_tag (FIELD_VALUE (tag))) {
        LOG_WARN ("Fixup invalid tag %s", FIELD_VALUE (tag));
        FIELD_VALUE (tag) = fixup_invalid_tag (dat, FIELD_VALUE (tag));
        LOG_WARN ("to %s", FIELD_VALUE (tag));
      }
#endif
      FIELD_TV (tag, 2);
      DECODER {
        if (FIELD_VALUE (tag) && !dwg_is_valid_tag (FIELD_VALUE (tag)))
          LOG_WARN ("Invalid tag %s", FIELD_VALUE (tag))
      }
      FIELD_RC (flags, 70); // 1 invisible, 2 constant, 4 verify, 8 preset
      LOG_FLAG_ATTDEF
      //if (R11OPTS (1)) { // since when?
      //  FIELD_RD (elevation, 30);
      //}
      if (R11OPTS (2)) {
        FIELD_RD (rotation, 50);
      }
      if (R11OPTS (4)) {
        FIELD_RD (width_factor, 41);
      }
      if (R11OPTS (8)) {
        FIELD_RD (oblique_angle, 51);
      }
      if (R11OPTS (16)) {
        FIELD_HANDLE0 (style, 1, 7);
      }
      if (R11OPTS (32)) {
        FIELD_CAST (generation, RC, BS, 71);
        LOG_TEXT_GENERATION
      }
      if (R11OPTS (64)) {
        FIELD_CAST (horiz_alignment, RC, BS, 72);
        LOG_HORIZ_ALIGNMENT
      }
      if (R11OPTS (128)) {
        FIELD_2RD (alignment_pt, 11);
      }
      if (R11OPTS (0x100))
        FIELD_3RD (extrusion, 210);
      if (R11OPTS (0x200)) {
        FIELD_CAST (vert_alignment, RC, BS, 74);
        LOG_VERT_ALIGNMENT
      }
    }
  VERSIONS (R_13b1, R_14)
    {
      DXF {
        FIELD_2RD (ins_pt, 10);
        FIELD_RD (elevation, 30);
      } else {
        FIELD_BD (elevation, 30);
        FIELD_2RD (ins_pt, 10);
      }
      FIELD_2RD (alignment_pt, 0);
      FIELD_3BD (extrusion, 0);
      FIELD_BD0 (thickness, 39);
      FIELD_BD0 (oblique_angle, 51);
      DXF {
        FIELD_HANDLE0 (style, 5, 7);
      }
      FIELD_BD0 (rotation, 50);
      FIELD_BD (height, 40);
      FIELD_BD1 (width_factor, 41);
      FIELD_T (default_value, 1);
      FIELD_BS0 (generation, 71);
      LOG_TEXT_GENERATION
      FIELD_BS0 (horiz_alignment, 72);
      LOG_HORIZ_ALIGNMENT
      FIELD_BS (vert_alignment, 0);
      LOG_VERT_ALIGNMENT
      DXF {
        if (_obj->alignment_pt.x != 0.0 &&
            _obj->alignment_pt.y != 0.0)
          {
            FIELD_2RD (alignment_pt, 11);
            FIELD_RD (elevation, 31);
          }
        FIELD_BE (extrusion, 210);
      }
    }

  IF_FREE_OR_SINCE (R_2000)
    {
      BITCODE_RC dataflags;
#ifdef IS_ENCODER
      if (dat->from_version < R_2000)
        dwg_set_dataflags (obj);
#endif
      FIELD_RC (dataflags, 0);
      dataflags = FIELD_VALUE (dataflags);
      DECODER_OR_ENCODER {
        dwg_log_dataflags (DWG_LOGLEVEL, DWG_LOGLEVEL_TRACE, dataflags);
      }

      DXF {
        FIELD_BT0 (thickness, 39);
        FIELD_2RD (ins_pt, 10);
        FIELD_RD (elevation, 30);
      } else {
        if (!(dataflags & 0x01))
          FIELD_RD (elevation, 30);
        FIELD_2RD (ins_pt, 10);
      }
      DXF {
        FIELD_RD (height, 40);
        FIELD_T (default_value, 1);
      } else {
        if (!(dataflags & 0x02))
          FIELD_2DD (alignment_pt, ins_pt, 0);
        FIELD_BE (extrusion, 0);
        FIELD_BT (thickness, 0);
      }
      DXF {
        FIELD_RD0 (rotation, 50);
        FIELD_RD1 (width_factor, 41);
        FIELD_RD0 (oblique_angle, 51);
        FIELD_HANDLE0 (style, 5, 7);
      } else {
        if (!(dataflags & 0x04))
          FIELD_RD (oblique_angle, 51);
        if (!(dataflags & 0x08))
          FIELD_RD0 (rotation, 50);
        FIELD_RD (height, 40);
        if (!(dataflags & 0x10)) {
          FIELD_RD (width_factor, 41);
        }
#ifdef IS_DECODER
        else
          FIELD_VALUE (width_factor) = 1.0;
#endif
        FIELD_T (default_value, 1);
      }

      if (!(dataflags & 0x20)) {
        FIELD_BS (generation, 71);
        LOG_TEXT_GENERATION
      }
      if (!(dataflags & 0x40)) {
        FIELD_BS (horiz_alignment, 72);
        LOG_HORIZ_ALIGNMENT
      }
      if (!(dataflags & 0x80)) {
        FIELD_BS (vert_alignment, 0);
        LOG_VERT_ALIGNMENT
      }
      DXF {
        FIELD_2DD (alignment_pt, ins_pt, 11);
        FIELD_RD (elevation, 31);
        FIELD_BE (extrusion, 210);
      }
    }

  SUBCLASS (AcDbAttributeDefinition);
  DXF {
    FIELD_T (prompt, 3);
    FIELD_T (tag, 2);
    FIELD_RC (type, 70);
    SINCE (R_13b1) {
      //FIELD_BS (field_length, 73);
      FIELD_BS0 (vert_alignment, 74);
    }
    SINCE (R_2004) {
      FIELD_RC (class_version, 280);
    }
  }
  SINCE (R_2010)
    {
      //int dxf = dat->version == R_2010 ? 280: 0;
      FIELD_RC (class_version, 0); // 0 = r2010
      VALUEOUTOFBOUNDS (class_version, 10)
    }
  IF_FREE_OR_SINCE (R_2018)
    {
      FIELD_RC (type, 0); // 1=single line, 2=multi line attrib, 4=multi line attdef

      if (FIELD_VALUE (type) > 1)
        {
          SUBCLASS (AcDbMText)
          LOG_WARN ("MTEXT fields")
          // TODO fields handles to MTEXT entities. how many?
          FIELD_HANDLE (mtext_handles, 0, 340); //TODO

          FIELD_BS (annotative_data_size, 70);
          if (FIELD_VALUE (annotative_data_size) > 1)
            {
              FIELD_RC (annotative_data_bytes, 0);
              FIELD_HANDLE (annotative_app, 0, 0); //TODO
              FIELD_BS (annotative_short, 0);
            }
        }
    }
  SINCE (R_13b1)
  {
    FIELD_T (tag, 0);
    FIELD_BS (field_length, 0); //DXF 73, unused
    FIELD_RC (flags, 0); // 1 invisible, 2 constant, 4 verify, 8 preset
    SINCE (R_2007) {
      FIELD_B (lock_position_flag, 0);
    }
    // specific to ATTDEF
    SINCE (R_2010) {
      FIELD_RC (attdef_class_version, 0);
      VALUEOUTOFBOUNDS (attdef_class_version, 10)
    }
    FIELD_T (prompt, 0);
  }

  COMMON_ENTITY_HANDLE_DATA;
  SINCE (R_13b1)
    FIELD_HANDLE (style, 5, 0);

DWG_ENTITY_END

/* (4/12) */
DWG_ENTITY (BLOCK)

  SUBCLASS (AcDbBlockBegin)
#ifndef IS_DXF
  PRE (R_2_0b) {
    FIELD_TV (name, 2);
    FIELD_2RD (base_pt, 10);
  }
  VERSIONS (R_2_0b, R_11) {
    FIELD_2RD (base_pt, 10);
    if (R11OPTS (2))
      FIELD_TV (xref_pname, 1);
    if (R11OPTS (4)) {
      FIELD_TV (name, 2);
    }
    FREE { // set via dwg_add_BLOCK
      FIELD_TV (name, 2);
    }
  }
#endif
  SINCE (R_13b1) {
    BLOCK_NAME (name, 2) // special pre-R13 naming rules
    COMMON_ENTITY_HANDLE_DATA;
  }
#ifdef IS_DXF
  {
    Dwg_Object_BLOCK_HEADER *_hdr = NULL;
    Dwg_Object *hdr
        = _ent->ownerhandle && _ent->ownerhandle->obj
              ? _ent->ownerhandle->obj : NULL;
    if (!hdr)
      hdr = dwg_ref_object (dwg, _ent->ownerhandle);
    if (!hdr || hdr->fixedtype != DWG_TYPE_BLOCK_HEADER)
      {
        Dwg_Bitcode_3RD nullpt = { 0.0, 0.0, 0.0 };
        VALUE_BL (0, 70); // flags: anon, has_attribs, is_xref, is_overlaid, ...
        VALUE_3BD (nullpt, 10);
      }
    else
      {
        _hdr = hdr->tio.object->tio.BLOCK_HEADER;
        VALUE_BL (_hdr->flag & 0x3f, 70);
        VALUE_3BD (_hdr->base_pt, 10);
      }
    SINCE (R_13)
      BLOCK_NAME (name, 3); // for entget() from BLOCK_HEADER
    if (_hdr) {
      VALUE_T (_hdr->xref_pname, 1);   // from BLOCK_HEADER
      VALUE_T0 (_hdr->description, 4); // from BLOCK_HEADER
    } else {
      VALUE_TFF ("", 1);
    }
  }
#endif

DWG_ENTITY_END

/* (5/13) */
DWG_ENTITY (ENDBLK)

  SUBCLASS (AcDbBlockEnd)
  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/* (6/17) */
DWG_ENTITY (SEQEND)

  //SUBCLASS (AcDbSequenceEnd) //unused
  PRE (R_13b1) {
    FIELD_RLx (begin_addr_r11, 0);
  }
  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/* (7/14) */
DWG_ENTITY (INSERT)

  SUBCLASS (AcDbBlockReference)
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
    DECODER { FIELD_VALUE (has_attribs) = R11FLAG (FLAG_R11_HAS_ATTRIBS); }
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
  SINCE (R_2000)
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
              FIELD_RD (scale.x, 41);
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

  SINCE (R_2004)
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
  IF_FREE_OR_SINCE (R_2004)
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

DWG_ENTITY_END

/* (8) 20.4.10 */
DWG_ENTITY (MINSERT)

  SUBCLASS (AcDbMInsertBlock)
#ifdef IS_DXF
    FIELD_HANDLE_NAME (block_header, 2, BLOCK_HEADER);
    if (FIELD_VALUE (has_attribs))
      FIELD_B (has_attribs, 66);
#endif
  FIELD_3DPOINT (ins_pt, 10);

  VERSIONS (R_13b1, R_14) {
    FIELD_3BD_1 (scale, 41);
  }

  SINCE (R_2000)
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
          else
            {
              assert (FIELD_VALUE (scale_flag) == 0);
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

  FIELD_BD0 (rotation, 50);
  DXF {
    FIELD_BE (extrusion, 210);
  } else {
    FIELD_3DPOINT (extrusion, 0);
  }
  FIELD_B (has_attribs, 0); // 66 above

  SINCE (R_2004)
    {
      if (FIELD_VALUE (has_attribs))
        FIELD_BL (num_owned, 0);
    }

  FIELD_BS (num_cols, 70);
  FIELD_BS (num_rows, 71);
  FIELD_BD (col_spacing, 44);
  FIELD_BD (row_spacing, 45);

  COMMON_ENTITY_HANDLE_DATA;
  FIELD_HANDLE (block_header, 5, 0);
  IF_FREE_OR_VERSIONS (R_13b1, R_2000)
  {
    if (FIELD_VALUE (has_attribs))
      {
        FIELD_HANDLE (first_attrib, 4, 0);
        FIELD_HANDLE (last_attrib, 4, 0);
      }
  }

  IF_FREE_OR_SINCE (R_2004)
    {
    if (FIELD_VALUE (has_attribs))
      {
        HANDLE_VECTOR (attribs, num_owned, 4, 0);
      }
    }

  if (FIELD_VALUE (has_attribs))
    {
      FIELD_HANDLE (seqend, 3, 0);
    }

DWG_ENTITY_END

/* (none/21) R2.4-R10 only */
DWG_ENTITY (_3DLINE)
  VERSIONS (R_2_4, R_9c1) {
    if (R11OPTS (1)) {
      FIELD_3RD (start, 10);
    } else {
      FIELD_2RD (start, 10);
    }
    if (R11OPTS (2)) {
      FIELD_3RD (end, 11);
    } else {
      FIELD_2RD (end, 11);
    }
  }
  SINCE (R_10) {
    FIELD_3RD (start, 10)
    FIELD_3RD (end, 11)

    if (R11OPTS (1))
      FIELD_3RD (extrusion, 210);
  }
  //COMMON_ENTITY_HANDLE_DATA;
DWG_ENTITY_END

/* (10/20) */
DWG_ENTITY (VERTEX_2D)

  SUBCLASS (AcDbVertex)
  SUBCLASS (AcDb2dVertex)
  PRE (R_13b1)
  {
#ifdef IN_JSON
    FIELD_3RD (point, 10)
#else
    FIELD_2RD (point, 10);
#endif
    if (R11OPTS (1))
      FIELD_RD (start_width, 40);
    if (R11OPTS (2))
      FIELD_RD (end_width, 41);
    if (R11OPTS (4))
      FIELD_RD (bulge, 42);
    if (R11OPTS (8)) {
      FIELD_RC (flag, 0);
      LOG_FLAG_VERTEX
    }
    if (R11OPTS (16))
      FIELD_RD (tangent_dir, 50);
  }
  SINCE (R_13b1)
  {
    FIELD_RC (flag, 0);
    LOG_FLAG_VERTEX
    FIELD_3BD (point, 10);

  /* Decoder and Encoder routines could be the same but then we
     wouldn't compress data when saving. So we explicitly implemented
     the encoder routine with the compression technique described in
     the spec. --Juca */
    DXF_OR_PRINT {
      if (FIELD_VALUE (flag) != 0) {
        FIELD_BD0 (start_width, 40);
        FIELD_BD0 (end_width, 41);
      }
    }
    DECODER
    {
      FIELD_BD (start_width, 40);
      if (FIELD_VALUE (start_width) < 0)
        {
          FIELD_VALUE (start_width) = -FIELD_VALUE (start_width);
          FIELD_VALUE (end_width) = FIELD_VALUE (start_width);
        }
      else
        {
          FIELD_BD (end_width, 41);
        }
    }

  ENCODER
    {
      if (FIELD_VALUE (start_width) && FIELD_VALUE (start_width) == FIELD_VALUE (end_width))
        {
          //TODO: This is ugly! We should have a better way of doing such things
          FIELD_VALUE (start_width) = -FIELD_VALUE (start_width);
          FIELD_BD (start_width, 40);
          FIELD_VALUE (start_width) = -FIELD_VALUE (start_width);
        }
      else
        {
          FIELD_BD (start_width, 40);
          FIELD_BD (end_width, 41);
        }
    }

    DXF {
      if (FIELD_VALUE (flag) != 0)
        FIELD_BD0 (bulge, 42);
    } else {
      FIELD_BD (bulge, 42);
    }
    SINCE (R_2010) {
      FIELD_BL0 (id, 91);
    }
    DXF {
      FIELD_RC0 (flag, 70);
      if (FIELD_VALUE (flag) != 0)
        FIELD_BD (tangent_dir, 50);
    } else {
      FIELD_BD (tangent_dir, 50);
    }
  }

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/* (11/20) */
DWG_ENTITY (VERTEX_3D)

  SUBCLASS (AcDbVertex)
  SUBCLASS (AcDb3dPolylineVertex)
  PRE (R_13b1)
  {
    FIELD_2RD (point, 10);
    if (R11OPTS (8)) {
      FIELD_RC (flag, 0);
      LOG_FLAG_VERTEX
    }
  }
  LATER_VERSIONS {
    FIELD_RC (flag, 0);
    LOG_FLAG_VERTEX
    FIELD_3BD (point, 10);
  }
  DXF { FIELD_RC (flag, 70); }

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/* (12/20) */
DWG_ENTITY (VERTEX_MESH)

  SUBCLASS (AcDbVertex)
  SUBCLASS (AcDbPolyFaceMeshVertex)
  PRE (R_13b1)
  {
    FIELD_2RD (point, 10);
    FIELD_RC (flag, 0);
    LOG_FLAG_VERTEX
  }
  LATER_VERSIONS {
    FIELD_RC (flag, 0);
    LOG_FLAG_VERTEX
    FIELD_3BD (point, 10);
  }
  DXF { FIELD_RC (flag, 70); }

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/* (13/20) */
DWG_ENTITY (VERTEX_PFACE)

  SUBCLASS (AcDbVertex)
  SUBCLASS (AcDbPolyFaceMeshVertex)
  PRE (R_13b1)
  {
    FIELD_2RD (point, 10);
    if (R11OPTS (OPTS_R11_VERTEX_HAS_FLAG)) {
      FIELD_RC (flag, 0);
      LOG_FLAG_VERTEX
    }
  }
  LATER_VERSIONS {
    FIELD_RC (flag, 0);
    LOG_FLAG_VERTEX
    FIELD_3BD (point, 10);
  }
  DXF { FIELD_RC (flag, 70); }

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/* (14/20) */
DWG_ENTITY (VERTEX_PFACE_FACE)

  SUBCLASS (AcDbFaceRecord)
#ifdef IS_DXF
  {
    BITCODE_3RD pt = { 0.0, 0.0, 0.0 };
    UNTIL (R_9) {
      VALUE_2RD (pt, 10)
    }
    LATER_VERSIONS {
      VALUE_3BD (pt, 10)
    }
    VALUE_RC ((BITCODE_RC)128, 70);
    FIELD_BS (vertind[0], 71);
    FIELD_BS (vertind[1], 72);
    FIELD_BS (vertind[2], 73);
    FIELD_BS0 (vertind[3], 74);
  }
#else
  //FIELD_VALUE (pt) = { 0.0, 0.0, 0.0 };
  PRE (R_13b1)
  {
    if (R11OPTS (OPTS_R11_VERTEX_HAS_FLAG)) {
      FIELD_RC (flag, 0);
      LOG_FLAG_VERTEX
    }
    if (R11OPTS (OPTS_R11_VERTEX_HAS_INDEX1)) {
      FIELD_RS (vertind[0], 71);
    }
    if (R11OPTS (OPTS_R11_VERTEX_HAS_INDEX2)) {
      FIELD_RS (vertind[1], 72);
    }
    if (R11OPTS (OPTS_R11_VERTEX_HAS_INDEX3)) {
      FIELD_RS (vertind[2], 73);
    }
    if (R11OPTS (OPTS_R11_VERTEX_HAS_INDEX4)) {
      FIELD_RS (vertind[3], 74);
    }
  }
  LATER_VERSIONS {
    FIELD_VALUE (flag) = 128;
    FIELD_BS (vertind[0], 71);
    FIELD_BS (vertind[1], 72);
    FIELD_BS (vertind[2], 73);
    FIELD_BS (vertind[3], 74);
  }
#endif
  //TODO R13 has color_r11 and ltype_r11 for all vertices, not in DXF
  COMMON_ENTITY_HANDLE_DATA;
DWG_ENTITY_END

/* (15/19)
   r11 has all-in-one: n/m mesh (FLAG 16). curve-fit (FLAG 2),
   spline-fit (FLAGS 4), 3dpline (FLAG 8), pface_mesh: FLAG 64
   is_closed FLAG(1)
*/
DWG_ENTITY (POLYLINE_2D)

  //SUBCLASS (AcDbCurve)
  SUBCLASS (AcDb2dPolyline)
  PRE (R_13b1)
  {
    if (R11OPTS (1)) {
      FIELD_CAST (flag, RC, BS, 70);
      LOG_FLAG_POLYLINE
    }
    if (R11OPTS (2))
      FIELD_RD (start_width, 40);
    if (R11OPTS (4))
      FIELD_RD (end_width, 41);
    if (R11OPTS (8))
      FIELD_3RD (extrusion, 210);
    if (R11OPTS (16))
      FIELD_RS (num_m_verts, 71);
    if (R11OPTS (32))
      FIELD_RS (num_n_verts, 72);
    if (R11OPTS (0x100)) {
      FIELD_RS (curve_type, 75);
      LOG_POLYLINE_CURVETYPE
    }
    if (R11OPTS (OPTS_R11_POLYLINE_IN_EXTRA) &&
        obj->size > 20)
    {
      // Note: layer is then the extras_start offset
      DECODER {
        _obj->extra_r11_size = (obj->address + obj->size - dat->byte) & 0xFFFFFFFF;
        if (dat->version >= R_11b1)
          _obj->extra_r11_size -= 2;
        if (_obj->extra_r11_size > obj->size)
          _obj->extra_r11_size = 0;
      }
      FIELD_TFv (extra_r11_text, _obj->extra_r11_size, 0);
    }
    DECODER {
      FIELD_VALUE (has_vertex) = R11FLAG (FLAG_R11_HAS_ATTRIBS) ? 1 : 0;
    }
  }
  SINCE (R_13b1)
  {
    DXF {
      FIELD_B (has_vertex, 66);
    }
    else {
      FIELD_VALUE (has_vertex) = 1;
    }
    FIELD_BS0 (flag, 70);
    LOG_FLAG_POLYLINE
    FIELD_BS0 (curve_type, 75);
    LOG_POLYLINE_CURVETYPE
    DECODER_OR_ENCODER {
      FIELD_BD (start_width, 40);
      FIELD_BD (end_width, 41);
      FIELD_BT (thickness, 39);
      FIELD_BD (elevation, 0);
    }
    DXF {
      BITCODE_3RD pt = { 0.0, 0.0, 0.0 };
      pt.z = FIELD_VALUE (elevation);
      FIELD_BT0 (thickness, 39);
      KEY (elevation); VALUE_3BD (pt, 10);
      FIELD_BD (start_width, 40);
      FIELD_BD (end_width, 41);
    }
    FIELD_BE (extrusion, 210);

    SINCE (R_2004) {
      FIELD_BL (num_owned, 0);
    }
  }
  COMMON_ENTITY_HANDLE_DATA;

  IF_FREE_OR_VERSIONS (R_13b1, R_2000)
    {
      FIELD_HANDLE (first_vertex, 4, 0);
      FIELD_HANDLE (last_vertex, 4, 0);
    }

  IF_FREE_OR_SINCE (R_2004)
    {
      HANDLE_VECTOR (vertex, num_owned, 3, 0);
    }

  IF_FREE_OR_SINCE (R_13b1)
    {
      FIELD_HANDLE (seqend, 3, 0);
    }

DWG_ENTITY_END

/* (16/19) */
DWG_ENTITY (POLYLINE_3D)

  SUBCLASS (AcDb3dPolyline)
  DXF {
    BITCODE_3RD pt = { 0.0, 0.0, 0.0 };
    BITCODE_RC flag = FIELD_VALUE (flag);
    FIELD_B (has_vertex, 66);
    KEY (elevation); VALUE_3BD (pt, 10);
    KEY (flag); VALUE_RC ((BITCODE_RC)(flag | 8), 70);
  }
  else {
    FIELD_VALUE (has_vertex) = 1;
  }
  PRE (R_13b1)
  {
    if (R11OPTS (1)) {
      FIELD_CAST (flag, RC, BS, 70);
      LOG_FLAG_POLYLINE
    }
    if (R11OPTS (2))
      FIELD_RD (start_width, 40);
    if (R11OPTS (4))
      FIELD_RD (end_width, 41);
    if (R11OPTS (8))
      FIELD_3RD (extrusion, 210);
    if (R11OPTS (0x100)) { // 3dmesh only
      FIELD_RS (curve_type, 75);
      LOG_POLYLINE_CURVETYPE
    }
    DECODER {
      FIELD_VALUE (has_vertex) = R11FLAG (FLAG_R11_HAS_ATTRIBS) ? 1 : 0;
    }
  }
  LATER_VERSIONS {
    FIELD_RC0 (curve_type, 75);
    LOG_POLYLINE_CURVETYPE
    FIELD_RC (flag, 0);
    LOG_FLAG_POLYLINE
  }
  SINCE (R_2004) {
    FIELD_BL (num_owned, 0);
  }

  COMMON_ENTITY_HANDLE_DATA;
  IF_FREE_OR_VERSIONS (R_13b1, R_2000)
    {
      FIELD_HANDLE (first_vertex, 4, 0);
      FIELD_HANDLE (last_vertex, 4, 0);
    }
  IF_FREE_OR_SINCE (R_2004)
    {
      HANDLE_VECTOR (vertex, num_owned, 3, 0);
    }
  SINCE (R_13b1) {
    FIELD_HANDLE (seqend, 3, 0);
  }

DWG_ENTITY_END

/* (17/8) */
DWG_ENTITY (ARC)

  //SUBCLASS (AcDbCurve)
  SUBCLASS (AcDbCircle)
  PRE (R_13b1) {
    FIELD_2RD (center, 10);
    FIELD_RD (radius, 40);
    FIELD_RD (start_angle, 50);
    FIELD_RD (end_angle, 51);
    if (R11OPTS (1))
      FIELD_3RD (extrusion, 210);
    if (R11OPTS (2))
      FIELD_RD (center.z, 30);
  }
  LATER_VERSIONS {
    FIELD_3BD (center, 10);
    FIELD_BD (radius, 40);
    FIELD_BT0 (thickness, 39);
    FIELD_BE (extrusion, 210);
    SUBCLASS (AcDbArc)
    FIELD_BD (start_angle, 50);
    FIELD_BD (end_angle, 51);
  }

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/* (18/3) */
DWG_ENTITY (CIRCLE)

  //SUBCLASS (AcDbCurve)
  SUBCLASS (AcDbCircle)
  PRE (R_13b1) {
    FIELD_2RD (center, 10);
    FIELD_RD (radius, 40);
    if (R11OPTS (1))
      FIELD_3RD (extrusion, 210);
    if (R11OPTS (2))
      FIELD_RD (center.z, 38);
  }
  LATER_VERSIONS {
    FIELD_3BD (center, 10);
    FIELD_BD (radius, 40);
    FIELD_BT0 (thickness, 39);
    FIELD_BE (extrusion, 210);
  }

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/* (19/1) */
DWG_ENTITY (LINE)

  //SUBCLASS (AcDbCurve)
  SUBCLASS (AcDbLine)
  PRE (R_10) {
      FIELD_2RD (start, 10)
      FIELD_2RD (end, 11)
  }
  VERSIONS (R_10, R_12) {
    if (!R11FLAG (FLAG_R11_HAS_ELEVATION)) {
      FIELD_3RD (start, 10)
      FIELD_3RD (end, 11)
    } else {
      FIELD_2RD (start, 10)
      FIELD_2RD (end, 11)
      if (((! R11OPTS (1) && obj->size >= 56) || (R11OPTS (1) && obj->size >= 80))
        && _ent->flag_r11 == FLAG_R11_HAS_ELEVATION) // GH #586
        FIELD_2RD (unknown_r11, 0)
    }
  }
  PRE (R_13b1) {
    if (R11OPTS (1))
      FIELD_3RD (extrusion, 210);
  }
  VERSIONS (R_13b1, R_14)
    {
      FIELD_3BD (start, 10);
      FIELD_3BD (end, 11);
    }
  SINCE (R_2000)
    {
      ENCODER {
        FIELD_VALUE (z_is_zero) = (FIELD_VALUE (start.z) == 0.0 &&
                                   FIELD_VALUE (end.z) == 0.0);
      }
      DXF_OR_PRINT
        {
          JSON { FIELD_B (z_is_zero, 0); }
          FIELD_3DPOINT (start, 10);
          FIELD_3DPOINT (end, 11);
        }
      else
        {
          FIELD_B (z_is_zero, 0);
          FIELD_RD (start.x, 10);
          FIELD_DD (end.x, FIELD_VALUE (start.x), 11);
          FIELD_RD (start.y, 20);
          FIELD_DD (end.y, FIELD_VALUE (start.y), 21);

          if (FIELD_VALUE (z_is_zero))
            {
              FIELD_VALUE (start.z) = 0.0;
              FIELD_VALUE (end.z) = 0.0;
            }
          else
            {
              FIELD_RD (start.z, 30);
              FIELD_DD (end.z, FIELD_VALUE (start.z), 31);
            }
          FIELD_3PT_TRACE (start, DD, 10);
          FIELD_3PT_TRACE (end, DD, 11);
        }
    }

  SINCE (R_13b1) {
    FIELD_BT0 (thickness, 39);
    FIELD_BE (extrusion, 210);
  }

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/**
 * DIMENSION_common declaration
 */
#ifndef COMMON_ENTITY_DIMENSION
#define COMMON_ENTITY_DIMENSION                                               \
    SUBCLASS (AcDbDimension)                                                  \
    SINCE (R_2010)                                                            \
    {                                                                         \
      FIELD_RC (class_version, 280); /* 0=r2010 */                            \
      VALUEOUTOFBOUNDS (class_version, 10)                                    \
    }                                                                         \
    PRE (R_13b1)                                                              \
    {                                                                         \
      FIELD_HANDLE (block, 2, 2);                                             \
      if (dat->version >= R_10) {                                             \
        FIELD_3RD (def_pt, 10);                                               \
      } else {  /* ANG2LN */                                                  \
        FIELD_2RD (def_pt, 10);                                               \
      }                                                                       \
      FIELD_2RD (text_midpt, 11);                                             \
      if (R11OPTS (1))                                                        \
        FIELD_2RD (clone_ins_pt, 12);                                         \
      if (R11OPTS (2))                                                        \
        FIELD_RC (flag, 70);                                                  \
      if (R11OPTS (4))                                                        \
        FIELD_TV (user_text, 1);                                              \
    }                                                                         \
    LATER_VERSIONS                                                            \
    {                                                                         \
      DXF                                                                     \
      {                                                                       \
        /* converted to utf8 */                                               \
        char *blockname = dwg_dim_blockname (dwg, obj);                       \
        VALUE_TV0 (blockname, 2);                                             \
        if (blockname)                                                        \
          free (blockname);                                                   \
        FIELD_3BD (def_pt, 10);                                               \
      }                                                                       \
      else { FIELD_3BD (extrusion, 210); }                                    \
      FIELD_2RD (text_midpt, 11);                                             \
      FIELD_BD (elevation, 31);                                               \
      DXF { FIELD_RC0 (flag, 70); }                                           \
      else { FIELD_RC (flag1, 0); }                                           \
    }                                                                         \
    DECODER                                                                   \
    {                                                                         \
      SINCE (R_13b1) {                                                        \
        /* clear the upper flag bits, and fix them: */                        \
        BITCODE_RC flag = FIELD_VALUE (flag1) & 0xe0;                         \
        /* bit 7 (non-default) is inverse of bit 0 */                         \
        flag = (FIELD_VALUE (flag1) & 1) ? flag & 0x7F : flag | 0x80;         \
        /* set bit 5 (use block) to bit 1. always set since r13 */            \
        flag = (FIELD_VALUE (flag1) & 2) ? flag | 0x20 : flag & 0xDF;         \
        if (obj->fixedtype == DWG_TYPE_DIMENSION_ALIGNED)                     \
          flag |= 1;                                                          \
        else if (obj->fixedtype == DWG_TYPE_DIMENSION_ANG2LN)                 \
          flag |= 2;                                                          \
        else if (obj->fixedtype == DWG_TYPE_DIMENSION_DIAMETER)               \
          flag |= 3;                                                          \
        else if (obj->fixedtype == DWG_TYPE_DIMENSION_RADIUS)                 \
          flag |= 4;                                                          \
        else if (obj->fixedtype == DWG_TYPE_DIMENSION_ANG3PT)                 \
          flag |= 5;                                                          \
        else if (obj->fixedtype == DWG_TYPE_DIMENSION_ORDINATE)               \
          flag |= 6;                                                          \
        FIELD_VALUE (flag) = flag;                                            \
        LOG_TRACE ("flag => 0x%x [RC 70]\n", flag);                           \
      }                                                                       \
    }                                                                         \
    DXF                                                                       \
    {                                                                         \
      if (dat->from_version >= R_2007)                                        \
        {                                                                     \
          FIELD_T (user_text, 1);                                             \
        }                                                                     \
      else if (_obj->user_text && strlen (_obj->user_text))                   \
        {                                                                     \
          FIELD_TV (user_text, 1);                                            \
        }                                                                     \
    }                                                                         \
    else                                                                      \
    {                                                                         \
      SINCE (R_13b1)                                                          \
      {                                                                       \
        FIELD_T (user_text, 1);                                               \
        FIELD_BD0 (text_rotation, 53);                                        \
        FIELD_BD0 (horiz_dir, 51);                                            \
        FIELD_3BD_1 (ins_scale, 0);                                           \
        FIELD_BD0 (ins_rotation, 54);                                         \
      }                                                                       \
    }                                                                         \
    SINCE (R_2000)                                                            \
    {                                                                         \
      FIELD_BS (attachment, 71);                                              \
      FIELD_BS1 (lspace_style, 72);                                           \
      FIELD_BD1 (lspace_factor, 41);                                          \
      FIELD_BD (act_measurement, 42);                                         \
    }                                                                         \
    SINCE (R_2007)                                                            \
    {                                                                         \
      FIELD_B (unknown, 73); /* always 0 */                                   \
      FIELD_B (flip_arrow1, 74);                                              \
      FIELD_B (flip_arrow2, 75);                                              \
    }                                                                         \
    SINCE (R_13b1) {                                                          \
      FIELD_2RD0 (clone_ins_pt, 12);                                          \
    }                                                                         \
    DXF                                                                       \
    {                                                                         \
      FIELD_BD0 (ins_rotation, 54);                                           \
      FIELD_BD0 (horiz_dir, 51);                                              \
      FIELD_BE (extrusion, 210);                                              \
      FIELD_BD0 (text_rotation, 53);                                          \
      FIELD_HANDLE0 (dimstyle, 5, 3);                                         \
    }                                                                         \
    JSON { FIELD_RC (flag, 0); }
#endif

/* (20/23) */
DWG_ENTITY (DIMENSION_ORDINATE)

  COMMON_ENTITY_DIMENSION
  SUBCLASS (AcDbOrdinateDimension)
  PRE (R_13b1) {
    if (R11OPTS (8)) { // if dxf 13 (extension_defining_pt)
      if (dat->version >= R_10) {
        FIELD_3RD (feature_location_pt, 13)
      } else {
        FIELD_2RD (feature_location_pt, 13)
      }
    }
    if (R11OPTS (16)) { // extension_defining_point2
      if (dat->version >= R_10) {
        FIELD_3RD (leader_endpt, 14)
      } else {
        FIELD_2RD (leader_endpt, 14)
      }
    }
    if (R11OPTS (0x400))
      FIELD_RD0 (text_rotation, 53);
    if (R11OPTS (0x8000))
      FIELD_HANDLE (dimstyle, 2, 0);
  } else {
    FIELD_3BD (def_pt, 0);
    FIELD_3BD (feature_location_pt, 13);
    FIELD_3BD (leader_endpt, 14);
    FIELD_RC (flag2, 0);
  }
  DECODER {
    SINCE (R_13b1) {
      BITCODE_RC flag = FIELD_VALUE (flag);
      flag = (FIELD_VALUE (flag2) & 1)
        ? flag | 0x80 : flag & 0xBF; /* set bit 6 */
      FIELD_VALUE (flag) = flag;
    }
  }
  JSON { FIELD_RC (flag, 0); }

  SINCE (R_13b1) {
    COMMON_ENTITY_HANDLE_DATA;
    FIELD_HANDLE (dimstyle, 5, 0);
    FIELD_HANDLE (block, 5, 0);
  }

DWG_ENTITY_END

/* (21/23) */
DWG_ENTITY (DIMENSION_LINEAR)

  COMMON_ENTITY_DIMENSION
  SUBCLASS (AcDbAlignedDimension)
  PRE (R_13b1) {
    if (R11OPTS (8)) {
      if (dat->version >= R_10) {
        FIELD_3RD (xline1_pt, 13)
      } else {
        FIELD_2RD (xline1_pt, 13)
      }
    }
    if (R11OPTS (16)) {
      if (dat->version >= R_10) {
        FIELD_3RD (xline2_pt, 14)
      } else {
        FIELD_2RD (xline2_pt, 14)
      }
    }
    if (R11OPTS (0x100))
      FIELD_RD (dim_rotation, 50);
    if (R11OPTS (0x200))
      FIELD_RD (oblique_angle, 52); // ext_line_rotation
    if (R11OPTS (0x400))
      FIELD_RD0 (text_rotation, 53);
    if (R11OPTS (0x4000))
      FIELD_3RD (extrusion, 210);
    if (R11OPTS (0x8000))
      FIELD_HANDLE (dimstyle, 2, 0);
  }
  LATER_VERSIONS {
    FIELD_3BD (xline1_pt, 13);
    FIELD_3BD (xline2_pt, 14);
    FIELD_3BD (def_pt, 0);
    FIELD_BD (oblique_angle, 52);
    FIELD_BD0 (dim_rotation, 50);
    SUBCLASS (AcDbRotatedDimension)

    COMMON_ENTITY_HANDLE_DATA;
    FIELD_HANDLE (dimstyle, 5, 0);
    FIELD_HANDLE (block, 5, 0);
  }

DWG_ENTITY_END

/* (22/23) */
DWG_ENTITY (DIMENSION_ALIGNED)

  COMMON_ENTITY_DIMENSION
  SUBCLASS (AcDbAlignedDimension)
  PRE (R_13b1) {
    if (R11OPTS (8)) {
      if (dat->version >= R_10) {
        FIELD_3RD (xline1_pt, 13)
      } else {
        FIELD_2RD (xline1_pt, 13)
      }
    }
    if (R11OPTS (16)) {
      if (dat->version >= R_10) {
        FIELD_3RD (xline2_pt, 14)
      } else {
        FIELD_2RD (xline2_pt, 14)
      }
    }
    if (R11OPTS (0x100))
      FIELD_RD (oblique_angle, 50);
    if (R11OPTS (0x400))
      FIELD_RD0 (text_rotation, 53);
    if (R11OPTS (0x8000))
      FIELD_HANDLE (dimstyle, 2, 0);
  }
  LATER_VERSIONS {
    FIELD_3BD (xline1_pt, 13);
    FIELD_3BD (xline2_pt, 14);
    FIELD_3BD (def_pt, 0);
    FIELD_BD (oblique_angle, 50);

    COMMON_ENTITY_HANDLE_DATA;
    FIELD_HANDLE (dimstyle, 5, 0);
    FIELD_HANDLE (block, 5, 0);
  }
DWG_ENTITY_END

/* (23/23) */
DWG_ENTITY (DIMENSION_ANG3PT)

  COMMON_ENTITY_DIMENSION
  SUBCLASS (AcDb3PointAngularDimension)
  PRE (R_13b1) {
    if (R11OPTS (8)) {
      if (dat->version >= R_10) {
        FIELD_3RD (xline1_pt, 13)
      } else {
        FIELD_2RD (xline1_pt, 13)
      }
    }
    if (R11OPTS (16)) {
      if (dat->version >= R_10) {
        FIELD_3RD (xline2_pt, 14)
      } else {
        FIELD_2RD (xline2_pt, 14)
      }
    }
    if (R11OPTS (32)) {
      if (dat->version >= R_10) {
        FIELD_3RD (center_pt, 15)
      } else {
        FIELD_2RD (center_pt, 15)
      }
    }
    if (R11OPTS (64))
      FIELD_2RD (xline2end_pt, 0);
    if (R11OPTS (0x400))
      FIELD_RD0 (text_rotation, 53);
    if (R11OPTS (0x8000))
      FIELD_HANDLE (dimstyle, 2, 0);
  }
  LATER_VERSIONS {
    FIELD_3BD (def_pt, 0);
    FIELD_3BD (xline1_pt, 13);
    FIELD_3BD (xline2_pt, 14);
    FIELD_3BD (center_pt, 15);

    COMMON_ENTITY_HANDLE_DATA;
    FIELD_HANDLE (dimstyle, 5, 0);
    FIELD_HANDLE (block, 5, 0);
  }

DWG_ENTITY_END

/* (24/23) */
DWG_ENTITY (DIMENSION_ANG2LN)

  COMMON_ENTITY_DIMENSION
  SUBCLASS (AcDb2LineAngularDimension)
  JSON { FIELD_3RD (def_pt, 0) }
  PRE (R_13b1) {
    if (R11OPTS (8)) {
      if (dat->version >= R_10) {
        FIELD_3RD (xline1start_pt, 13)
      } else {
        FIELD_2RD (xline1start_pt, 13)
      }
    }
    if (R11OPTS (16)) {
      if (dat->version >= R_10) {
        FIELD_3RD (xline1end_pt, 14)
      } else {
        FIELD_2RD (xline1end_pt, 14)
      }
    }
    if (R11OPTS (32)) {
      if (dat->version >= R_10) {
        FIELD_3RD (xline2start_pt, 15)
      } else {
        FIELD_2RD (xline2start_pt, 15)
      }
    }
    if (R11OPTS (64)) {
      FIELD_2RD (xline2end_pt, 16);
    }
    if (R11OPTS (0x400)) {
      FIELD_RD0 (text_rotation, 53);
    }
    if (R11OPTS (0x8000))
      FIELD_HANDLE (dimstyle, 2, 0);
  }
  LATER_VERSIONS {
#ifndef IS_JSON
    FIELD_2RD (def_pt, 0);
#endif
    FIELD_3BD (xline1start_pt, 13);
    FIELD_3BD (xline1end_pt, 14);
    FIELD_3BD (xline2start_pt, 15);
    FIELD_3BD (xline2end_pt, 16);

    COMMON_ENTITY_HANDLE_DATA;
    FIELD_HANDLE (dimstyle, 5, 0);
    FIELD_HANDLE (block, 5, 0);
  }

DWG_ENTITY_END

/* (25/23) */
DWG_ENTITY (DIMENSION_RADIUS)

  COMMON_ENTITY_DIMENSION
  SUBCLASS (AcDbRadialDimension)
  PRE (R_13b1) {
    if (R11OPTS (32)) {
      if (dat->version >= R_10) {
        FIELD_3RD (first_arc_pt, 15);
      } else {
        FIELD_2RD (first_arc_pt, 15);
      }
    }
    if (R11OPTS (128))
      FIELD_RD (leader_len, 40);
    if (R11OPTS (0x400))
      FIELD_RD0 (text_rotation, 53);
    if (R11OPTS (0x4000))
      FIELD_3RD (extrusion, 210);
    if (R11OPTS (0x8000))
      FIELD_HANDLE (dimstyle, 2, 0);
  } LATER_VERSIONS {
    FIELD_3BD (def_pt, 0);
    FIELD_3BD (first_arc_pt, 15);
    FIELD_BD (leader_len, 40);

    COMMON_ENTITY_HANDLE_DATA;
    FIELD_HANDLE (dimstyle, 5, 0);
    FIELD_HANDLE (block, 5, 0);
 }

DWG_ENTITY_END

/* (26/23) */
DWG_ENTITY (DIMENSION_DIAMETER)

  COMMON_ENTITY_DIMENSION
  SUBCLASS (AcDbDiametricDimension)
  PRE (R_13b1) {
    if (R11OPTS (32)) {
      if (dat->version >= R_10) {
        FIELD_3RD (first_arc_pt, 15);
      } else {
        FIELD_2RD (first_arc_pt, 15);
      }
    }
    if (R11OPTS (128))
      FIELD_RD (leader_len, 40);
    if (R11OPTS (0x400))
      FIELD_RD0 (text_rotation, 53);
    if (R11OPTS (0x4000))
      FIELD_3RD (extrusion, 210);
    if (R11OPTS (0x8000))
      FIELD_HANDLE (dimstyle, 2, 0);
  } LATER_VERSIONS {
    FIELD_3BD (first_arc_pt, 15);
    FIELD_3BD (def_pt, 0); // = far_chord_pt
    FIELD_BD (leader_len, 40);

    COMMON_ENTITY_HANDLE_DATA;
    FIELD_HANDLE (dimstyle, 5, 0);
    FIELD_HANDLE (block, 5, 0);
 }

DWG_ENTITY_END

/* varies */
DWG_ENTITY (LARGE_RADIAL_DIMENSION)

  COMMON_ENTITY_DIMENSION
  SUBCLASS (AcDbRadialDimensionLarge)
  FIELD_3BD (def_pt, 0);
  FIELD_3BD (first_arc_pt, 15);
  FIELD_BD (leader_len, 40);
  FIELD_3BD (ovr_center, 12);
  FIELD_3BD (jog_point, 13);

  COMMON_ENTITY_HANDLE_DATA;
  FIELD_HANDLE (dimstyle, 5, 0);
  FIELD_HANDLE (block, 5, 0);
DWG_ENTITY_END

/* (27/2) */
DWG_ENTITY (POINT)

  SUBCLASS (AcDbPoint)
  PRE (R_13b1) {
    FIELD_RD (x, 10);
    FIELD_RD (y, 20);
    if (dat->version >= R_10 && !R11FLAG (FLAG_R11_HAS_ELEVATION)) {
      FIELD_RD (z, 30);
    }
    if (R11OPTS (1))
      FIELD_3RD (extrusion, 210);
    if (R11OPTS (2))
      FIELD_RD (x_ang, 50);
  } LATER_VERSIONS {
    FIELD_BD (x, 10);
    FIELD_BD (y, 20);
    FIELD_BD (z, 30);
    FIELD_BT0 (thickness, 39);
    FIELD_BE (extrusion, 210);
    FIELD_BD (x_ang, 50);
  }

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/* (28/22) */
DWG_ENTITY (_3DFACE)

  SUBCLASS (AcDbFace)
  PRE (R_10) {
    if (R11OPTS (1)) {
      FIELD_3RD (corner1, 10)
    }
    else {
      FIELD_2RD (corner1, 10)
    }
    if (R11OPTS (2)) {
      FIELD_3RD (corner2, 11)
    }
    else {
      FIELD_2RD (corner2, 11)
    }
    if (R11OPTS (4)) {
      FIELD_3RD (corner3, 12)
    }
    else {
      FIELD_2RD (corner3, 12)
    }
    if (R11OPTS (8)) {
      FIELD_3RD (corner4, 13)
    }
    else {
      FIELD_2RD (corner4, 13)
    }
  }
  VERSIONS (R_10, R_11)
    {
      if (R11FLAG (FLAG_R11_HAS_ELEVATION)) {
        FIELD_2RD (corner1, 10)
        FIELD_2RD (corner2, 11)
        FIELD_2RD (corner3, 12)
        FIELD_2RD (corner4, 13)
      }
      else {
        FIELD_3RD (corner1, 10)
        FIELD_3RD (corner2, 11)
        FIELD_3RD (corner3, 12)
        FIELD_3RD (corner4, 13)
      }
      if (R11OPTS (1)) {
        FIELD_RS (invis_flags, 70);
        LOG_3DFACE_INVISIBLE
      }
    }
  VERSIONS (R_13b1, R_14)
    {
      FIELD_3BD (corner1, 10);
      FIELD_3BD (corner2, 11);
      FIELD_3BD (corner3, 12);
      FIELD_3BD (corner4, 13);
      FIELD_BS0 (invis_flags, 70);
      LOG_3DFACE_INVISIBLE
    }
  SINCE (R_2000)
    {
      FIELD_B (has_no_flags, 0);
      DXF_OR_PRINT
        {
          JSON { FIELD_B (z_is_zero, 0); }
          FIELD_3DPOINT (corner1, 10);
        }
      DECODER
        {
          FIELD_B (z_is_zero, 0);
          FIELD_RD (corner1.x, 10);
          FIELD_RD (corner1.y, 20);
          if (FIELD_VALUE (z_is_zero))
            FIELD_VALUE (corner1.z) = 0;
          else
            FIELD_RD (corner1.z, 30);
        }
      ENCODER
        {
          FIELD_VALUE (z_is_zero) = (FIELD_VALUE (corner1.z) == 0);
          FIELD_B (z_is_zero, 0);
          FIELD_RD (corner1.x, 10);
          FIELD_RD (corner1.y, 20);
          if (!FIELD_VALUE (z_is_zero))
            FIELD_RD (corner1.z, 30);
        }
      FIELD_3DD (corner2, corner1, 11);
      FIELD_3DD (corner3, corner2, 12);
      FIELD_3DD (corner4, corner3, 13);
      if (!FIELD_VALUE (has_no_flags))
        FIELD_BS0 (invis_flags, 70);
        LOG_3DFACE_INVISIBLE
    }

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/* (29) */
DWG_ENTITY (POLYLINE_PFACE)

  SUBCLASS (AcDbPolyFaceMesh)
  DXF {
    BITCODE_3RD pt = { 0.0, 0.0, 0.0 };
    FIELD_B (has_vertex, 66);
    KEY (elevation); VALUE_3BD (pt, 10);
    KEY (flag); VALUE_RC ((BITCODE_RC)64, 70);
  }
  else {
    FIELD_VALUE (has_vertex) = 1;
  }
  PRE (R_13b1) {
    if (R11OPTS (1)) {
      FIELD_CAST (flag, RC, BS, 70);
      LOG_FLAG_POLYLINE
    }
    if (R11OPTS (16))
      FIELD_RS (numverts, 71);
    if (R11OPTS (32))
      FIELD_RS (numfaces, 72);
  }
  LATER_VERSIONS {
    FIELD_BS (numverts, 71);
    FIELD_BS (numfaces, 72);
  }

  SINCE (R_2004) {
    FIELD_BL (num_owned, 0);
  }

  COMMON_ENTITY_HANDLE_DATA;
  IF_FREE_OR_VERSIONS (R_13b1, R_2000)
    {
      FIELD_HANDLE (first_vertex, 4, 0);
      FIELD_HANDLE (last_vertex, 4, 0);
    }
  IF_FREE_OR_SINCE (R_2004)
    {
      HANDLE_VECTOR (vertex, num_owned, 4, 0);
    }
  SINCE (R_13b1) {
    FIELD_HANDLE (seqend, 3, 0);
  }

DWG_ENTITY_END

/* (30) */
DWG_ENTITY (POLYLINE_MESH)

  SUBCLASS (AcDbPolygonMesh)
  DXF {
    BITCODE_3RD pt = { 0.0, 0.0, 0.0 };
    BITCODE_BS flag = FIELD_VALUE (flag) | 16U;
    FIELD_B (has_vertex, 66);
    KEY (elevation); VALUE_3BD (pt, 10);
    KEY (flag); VALUE_BS (flag, 70);
  }
  PRE (R_13b1) {
    DXF {
      FIELD_RS (num_m_verts, 71);
      FIELD_RS (num_n_verts, 72);
      FIELD_RS0 (m_density, 73);
      FIELD_RS0 (n_density, 74);
      FIELD_BS0 (curve_type, 75);
    } else {
      if (R11OPTS (1)) {
        FIELD_CAST (flag, RC, BS, 70);
        LOG_FLAG_POLYLINE
      }
      DECODER {
        FIELD_VALUE (has_vertex) = R11FLAG (FLAG_R11_HAS_ATTRIBS) ? 1 : 0;
      }
      if (R11OPTS (16))
        FIELD_RS (num_m_verts, 71);
      if (R11OPTS (32))
        FIELD_RS (num_n_verts, 72);
      if (R11OPTS (64))
        FIELD_RS (m_density, 73);
      if (R11OPTS (128))
        FIELD_RS (n_density, 74);
      if (R11OPTS (256)) {
        FIELD_RS (curve_type, 75);
        LOG_POLYLINE_CURVETYPE
      }
    }
  }
  LATER_VERSIONS {
    FIELD_BS (flag, 0);
    LOG_FLAG_POLYLINE
    FIELD_BS (curve_type, 75);
    LOG_POLYLINE_CURVETYPE
    FIELD_BS (num_m_verts, 71);
    FIELD_BS (num_n_verts, 72);
    FIELD_BS (m_density, 73);
    FIELD_BS (n_density, 74);
  }

  SINCE (R_2004) {
    FIELD_BL (num_owned, 0);
  }

  COMMON_ENTITY_HANDLE_DATA;
  VERSIONS (R_13b1, R_2000)
    {
      FIELD_HANDLE (first_vertex, 4, 0);
      FIELD_HANDLE (last_vertex, 4, 0);
    }
  IF_FREE_OR_SINCE (R_2004)
    {
      VALUEOUTOFBOUNDS (num_owned, 100000)
      HANDLE_VECTOR (vertex, num_owned, 4, 0);
    }
  SINCE (R_13b1) {
    FIELD_HANDLE (seqend, 3, 0);
  }

DWG_ENTITY_END

/* (31/11) */
DWG_ENTITY (SOLID)

  SUBCLASS (AcDbTrace)
  PRE (R_13b1) {
    FIELD_2RD (corner1, 10);
    FIELD_2RD (corner2, 11);
    FIELD_2RD (corner3, 12);
    FIELD_2RD (corner4, 13);
    if (R11OPTS (1))
      FIELD_3RD (extrusion, 210);
    if (R11OPTS (2))
      FIELD_RD (elevation, 38);
  }
  LATER_VERSIONS {
    FIELD_BT0 (thickness, 39);
    FIELD_BD (elevation, 38);
    FIELD_2RD (corner1, 10);
    FIELD_2RD (corner2, 11);
    FIELD_2RD (corner3, 12);
    FIELD_2RD (corner4, 13);
    FIELD_BE (extrusion, 210);

    COMMON_ENTITY_HANDLE_DATA;
  }

DWG_ENTITY_END

/* (32/9) */
DWG_ENTITY (TRACE)

  SUBCLASS (AcDbTrace)
  PRE (R_13b1) {
    FIELD_2RD (corner1, 10);
    FIELD_2RD (corner2, 11);
    FIELD_2RD (corner3, 12);
    FIELD_2RD (corner4, 13);
    if (R11OPTS (1))
      FIELD_3RD (extrusion, 210);
    if (R11OPTS (2))
      FIELD_RD (elevation, 38);
  }
  LATER_VERSIONS {
    FIELD_BT0 (thickness, 39);
    FIELD_BD (elevation, 38);
    FIELD_2RD (corner1, 10);
    FIELD_2RD (corner2, 11);
    FIELD_2RD (corner3, 12);
    FIELD_2RD (corner4, 13);
    FIELD_BE (extrusion, 210);

    COMMON_ENTITY_HANDLE_DATA;
  }

DWG_ENTITY_END

/* (33/4) */
DWG_ENTITY (SHAPE)

  SUBCLASS (AcDbShape)
  PRE (R_2_0) {
    FIELD_2RD (ins_pt, 10);
    FIELD_RD (scale, 40);
    FIELD_RD (rotation, 50);
    FIELD_RS (style_id, 0); // => shapename
  }
  VERSIONS (R_2_0, R_11) {
    FIELD_2RD (ins_pt, 10);
    FIELD_RD (scale, 40);
    DXF {
      // TODO style is optional. convert from style_id
      FIELD_HANDLE (style, 5, 2);
    } else {
      FIELD_CAST (style_id, RC, BS, 0);
    }
    if (R11OPTS (1))
      FIELD_RD0 (rotation, 50);
    if (R11OPTS (2)) // HAS_LOAD_NUM
      FIELD_HANDLE (style, 1, 0); // -> shapename 2
    if (R11OPTS (4))
      FIELD_RD0 (width_factor, 41);
    if (R11OPTS (8))
      FIELD_RD0 (oblique_angle, 51);
    DECODER {
      // thickness already in common
      if (R11FLAG (FLAG_R11_HAS_THICKNESS))
        _obj->thickness = _ent->thickness_r11;
      if (R11FLAG (FLAG_R11_HAS_ELEVATION))
        _obj->ins_pt.z = _ent->elevation_r11;
    }
  }
  SINCE (R_13b1) {
    FIELD_3BD (ins_pt, 10);
    FIELD_BD (scale, 40);  // documented as size
    FIELD_BD0 (rotation, 50);
    FIELD_BD (width_factor, 41);
    FIELD_BD (oblique_angle, 51);
    DXF { FIELD_HANDLE (style, 5, 7); }
    FIELD_BD0 (thickness, 39);
#ifdef IS_DXF
    { // FIXME use the tblname API (needed for r11 also)
      Dwg_Object *style;
      if (_obj->style)
        style = dwg_resolve_handle (dwg, _obj->style->absolute_ref);
      else
        {
          Dwg_Object_Ref *ctrlref = dwg->header_vars.STYLE_CONTROL_OBJECT;
          Dwg_Object *ctrl
            = ctrlref ? dwg_resolve_handle (dwg, ctrlref->absolute_ref) : NULL;
          Dwg_Object_STYLE_CONTROL *_ctrl
              = ctrl && ctrl->fixedtype == DWG_TYPE_STYLE_CONTROL
                    ? ctrl->tio.object->tio.STYLE_CONTROL
                    : NULL;
          Dwg_Object_Ref *styleref
              = _ctrl && _ctrl->entries && _obj->style_id < _ctrl->num_entries
                    ? _ctrl->entries[_obj->style_id] // index
                    : NULL;
          style = styleref ? dwg_resolve_handle (dwg, styleref->absolute_ref)
                           : NULL;
        }
      if (style && style->fixedtype == DWG_TYPE_STYLE)
        // dxf 2 for the name from SHAPE styles
        VALUE_T (style->tio.object->tio.STYLE->name, 2);
    }
#else
    FIELD_BS (style_id, 0); // STYLE index in dwg to SHAPEFILE
#endif
    FIELD_3BD (extrusion, 210);

    COMMON_ENTITY_HANDLE_DATA;
    FIELD_HANDLE (style, 5, 0);
  }

DWG_ENTITY_END

/* (34/24) */
DWG_ENTITY (VIEWPORT)

  SUBCLASS (AcDbViewport)
  PRE (R_13b1) {
    FIELD_3RD (center, 10);
    FIELD_RD (width, 40);
    FIELD_RD (height, 41);
    FIELD_RS (id, 69);
  }
  LATER_VERSIONS {
    FIELD_3BD (center, 10);
    FIELD_BD (width, 40);
    FIELD_BD (height, 41);
  }
  DXF {
    FIELD_VALUE (on_off) = 1;
    FIELD_VALUE (id) = 1;
    FIELD_RS (on_off, 68);
    FIELD_RS (id, 69);
  }

  SINCE (R_2000) {
    DXF {
      FIELD_2RD (VIEWCTR, 12);
      FIELD_2RD (SNAPBASE, 13);
      FIELD_2RD (SNAPUNIT, 14);
      FIELD_2RD (GRIDUNIT, 15);
      FIELD_3BD (VIEWDIR, 16);
      FIELD_3BD (view_target, 17);
      FIELD_BD (lens_length, 42);
      FIELD_BD (front_clip_z, 43);
      FIELD_BD (back_clip_z, 44);
      FIELD_BD (VIEWSIZE, 45);
      FIELD_BD (SNAPANG, 50);
      FIELD_BD (twist_angle, 51);
      FIELD_BS (circle_zoom, 72);
    } else {
      FIELD_3BD (view_target, 17);
      FIELD_3BD (VIEWDIR, 16);
      FIELD_BD (twist_angle, 51);
      FIELD_BD (VIEWSIZE, 45);
      FIELD_BD (lens_length, 42);
      FIELD_BD (front_clip_z, 43);
      FIELD_BD (back_clip_z, 44);
      if (dwg->header.dwg_version != 0x1a) { // AC1020/R_2006 only here
        FIELD_BD (SNAPANG, 50);
        FIELD_2RD (VIEWCTR, 12);
        FIELD_2RD (SNAPBASE, 13);
      } else {
        // on R_2006: no SNAPANG, SNAPBASE
        FIELD_2RD (VIEWCTR, 12);
      }
      FIELD_2RD (SNAPUNIT, 14);
      FIELD_2RD (GRIDUNIT, 15);
      FIELD_BS (circle_zoom, 72);
      SINCE (R_2007) {
        FIELD_BS (grid_major, 61);
      }
    }
  }

  SINCE (R_2000) {
    FIELD_BL (num_frozen_layers, 0);
    FIELD_BL (status_flag, 90);
    FIELD_T (style_sheet, 1);
    FIELD_RC (render_mode, 281);
    DXF {
      FIELD_B (UCSVP, 71);
      FIELD_B (ucs_at_origin, 74);
      FIELD_3BD (ucsorg, 110);
      FIELD_3BD (ucsxdir, 111);
      FIELD_3BD (ucsydir, 112);
      FIELD_BS (UCSORTHOVIEW, 79);
      FIELD_BD (ucs_elevation, 146);
      SINCE (R_2004) {
        FIELD_BS (shadeplot_mode, 170);
      }
      SINCE (R_2007) {
        FIELD_BS (grid_major, 61);
      }
    } else {
      FIELD_B (ucs_at_origin, 74);
      FIELD_B (UCSVP, 71);
      FIELD_3BD (ucsorg, 110);
      FIELD_3BD (ucsxdir, 111);
      FIELD_3BD (ucsydir, 112);
      FIELD_BD (ucs_elevation, 146);
      FIELD_BS (UCSORTHOVIEW, 79);
      SINCE (R_2004) {
        FIELD_BS (shadeplot_mode, 170);
      }
    }
  }
  DXF {
    FIELD_HANDLE (visualstyle, 5, 348);
  }

  SINCE (R_2007) {
    FIELD_B (use_default_lights, 292);
    FIELD_RC (default_lighting_type, 282);
    FIELD_BD (brightness, 141);
    FIELD_BD (contrast, 142);
    FIELD_CMC (ambient_color, 63);
  }

  COMMON_ENTITY_HANDLE_DATA;
  VERSIONS (R_13b1, R_14) {
    FIELD_HANDLE (vport_entity_header, 5, 0);
  }
  VERSION (R_2000) {
    HANDLE_VECTOR (frozen_layers, num_frozen_layers, 5, 341);
    FIELD_HANDLE (clip_boundary, 5, 340);
  }
  SINCE (R_2004) {
    HANDLE_VECTOR (frozen_layers, num_frozen_layers, 4, 341);
    FIELD_HANDLE (clip_boundary, 5, 340);
  }
  VERSION (R_2000) {
    FIELD_HANDLE (vport_entity_header, 5, 0);
  }
  SINCE (R_2000) {
    FIELD_HANDLE (named_ucs, 5, 345);
    FIELD_HANDLE (base_ucs, 5, 346);
  }
  SINCE (R_2007) {
    FIELD_HANDLE (background, 4, 332);
    FIELD_HANDLE (visualstyle, 5, 0);
    FIELD_HANDLE (shadeplot, 4, 333);
    FIELD_HANDLE (sun, 3, 361);
  }

DWG_ENTITY_END

/* (35) */
DWG_ENTITY (ELLIPSE)

  //SUBCLASS (AcDbCurve)
  SUBCLASS (AcDbEllipse)
  FIELD_3BD (center, 10);
  FIELD_3BD (sm_axis, 11);
  FIELD_3BD (extrusion, 210);
  FIELD_BD (axis_ratio, 40); // i.e RadiusRatio
  FIELD_BD (start_angle, 41);
  FIELD_BD (end_angle, 42);

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/* (36) */
DWG_ENTITY (SPLINE)

  //SUBCLASS (AcDbCurve)
  SUBCLASS (AcDbSpline)
  FIELD_BL (scenario, 0);
  LOG_SPLINE_SCENARIO
  UNTIL (R_2013) {
    if (FIELD_VALUE (scenario) != 1 && FIELD_VALUE (scenario) != 2)
      LOG_ERROR ("unknown scenario %d", FIELD_VALUE (scenario));
    DECODER {
      if (FIELD_VALUE (scenario) == 1)
        FIELD_VALUE (splineflags) = 8;
      else if (FIELD_VALUE (scenario) == 2)
        FIELD_VALUE (splineflags) = 9;
    }
  }
  SINCE (R_2013) {
    FIELD_BL (splineflags, 0);
    LOG_SPLINE_SPLINEFLAGS
    FIELD_BL (knotparam, 0);
    LOG_SPLINE_KNOTPARAM
    if (FIELD_VALUE (splineflags) & 1)
      FIELD_VALUE (scenario) = 2;
    if (FIELD_VALUE (knotparam) == 15)
      FIELD_VALUE (scenario) = 1;
  }

  // extrusion on planar
  DXF { VALUE_RD (0.0, 210); VALUE_RD (0.0, 220); VALUE_RD (1.0, 230);
        VALUE_BL (_obj->flag & ~0x420, 70);
      }
  FIELD_BL (degree, 71);

  if (FIELD_VALUE (scenario) & 1) { // spline
    FIELD_B (rational, 0); // flag bit 2
    FIELD_B (closed_b, 0); // flag bit 0
    FIELD_B (periodic, 0); // flag bit 1
    FIELD_BD (knot_tol, 0); // def: 0.0000001
    FIELD_BD (ctrl_tol, 0); // def: 0.0000001
    FIELD_BL (num_knots, 72);
    FIELD_BL (num_ctrl_pts, 73);
    FIELD_B (weighted, 0);
    DXF {
      FIELD_BL (num_fit_pts, 74); // i.e. 0
      FIELD_BD (knot_tol, 42);
      FIELD_BD (ctrl_tol, 43);
    }

    DECODER {
      // not 32
      FIELD_VALUE (flag) = 8 +          /* planar */
        FIELD_VALUE (closed_b) +        /* 1 */
        (FIELD_VALUE (periodic) << 1) + /* 2 */
        (FIELD_VALUE (rational) << 2) + /* 4 */
        (FIELD_VALUE (weighted) << 4);  /* 16 */
        // ignore method fit points and closed bits
        /*((FIELD_VALUE (splineflags) & ~5) << 7)*/
      LOG_TRACE ("=> flag: %d [70]\n", FIELD_VALUE (flag));
    }
    FIELD_VECTOR (knots, BD, num_knots, 40);
    REPEAT (num_ctrl_pts, ctrl_pts, Dwg_SPLINE_control_point)
    REPEAT_BLOCK
        SUB_FIELD_3BD_inl (ctrl_pts[rcount1], xyz, 10);
        if (!FIELD_VALUE (weighted))
          FIELD_VALUE (ctrl_pts[rcount1].w) = 0; // skipped when encoding
        else
          SUB_FIELD_BD (ctrl_pts[rcount1], w, 41);
        SET_PARENT_OBJ (ctrl_pts[rcount1]);
    END_REPEAT_BLOCK
    END_REPEAT (ctrl_pts);
    DXF_OR_FREE {
      FIELD_3DPOINT_VECTOR (fit_pts, num_fit_pts, 11);
    } // else calc. fit_pts
  }
  else { // bezier spline, scenario 2
    DECODER {
      // flag 32 in DXF
      FIELD_VALUE (flag) = 8 + 32 + // planar, not rational
        // ignore method fit points and closed bits
        ((FIELD_VALUE (splineflags) & ~5) << 7);
      LOG_TRACE ("=> flag: %d [70]\n", FIELD_VALUE (flag));
      FIELD_VALUE (knot_tol) = 0.0000001;
      FIELD_VALUE (ctrl_tol) = 0.0000001;
    }
    DXF {
      FIELD_BL (num_knots, 72);
      //FIXME: calculate knots and ctrl_pts from fit_pts for DXF
      FIELD_BL (num_ctrl_pts, 73);
      FIELD_BL (num_fit_pts, 74);
      FIELD_BD (knot_tol, 42);
      FIELD_BD (ctrl_tol, 43);
    }
    FIELD_BD (fit_tol, 44); // def: 0.0000001
    FIELD_3BD (beg_tan_vec, 12);
    FIELD_3BD (end_tan_vec, 13);
    FIELD_BL (num_fit_pts, 0);
    FIELD_3DPOINT_VECTOR (fit_pts, num_fit_pts, 11);
  }

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

// 37, 38 and 39 are ACIS entities
#define WIRESTRUCT_fields(name)                       \
  SUB_FIELD_RC (name, type, 0);                       \
  SUB_FIELD_BLd (name, selection_marker, 0);          \
  PRE (R_2004) {                                      \
    FIELD_CAST (name.color, BS, BL, 0);               \
  } else {                                            \
    SUB_FIELD_BL (name, color, 0);                    \
  }                                                   \
  SUB_FIELD_BLd (name, acis_index, 0);                \
  /* TODO: align num_points to 255 */                 \
  SUB_FIELD_BL (name, num_points, 0);                 \
  FIELD_3DPOINT_VECTOR (name.points, name.num_points, 0); \
  SUB_FIELD_B (name, transform_present, 0);           \
  if (FIELD_VALUE (name.transform_present))           \
    {                                                 \
      SUB_FIELD_3BD (name, axis_x, 0);                \
      SUB_FIELD_3BD (name, axis_y, 0);                \
      SUB_FIELD_3BD (name, axis_z, 0);                \
      SUB_FIELD_3BD (name, translation, 0);           \
      SUB_FIELD_3BD (name, scale, 0);                 \
      SUB_FIELD_B (name, has_rotation, 0);            \
      SUB_FIELD_B (name, has_reflection, 0);          \
      SUB_FIELD_B (name, has_shear, 0);               \
    }

#if defined (IS_DECODER)

#  define DECODE_3DSOLID                                                      \
    decode_3dsolid (dat, hdl_dat, obj, (Dwg_Entity_3DSOLID *)_obj);           \
    if (FIELD_VALUE (encr_sat_data) && !FIELD_VALUE (encr_sat_data[0]))       \
      FIELD_VALUE (block_size[0]) = 0;

static int decode_3dsolid (Bit_Chain* dat, Bit_Chain* hdl_dat,
                           Dwg_Object *restrict obj,
                           Dwg_Entity_3DSOLID *restrict _obj)
{
  Dwg_Data* dwg = obj->parent;
  BITCODE_BL j;
  BITCODE_BL vcount;
  BITCODE_BL i = 0;
  BITCODE_BL total_size = 0;
  BITCODE_BL num_blocks = 0;
  int acis_data_idx;
  int error = 0;

  FIELD_B (acis_empty, 290);
  if (dat->byte >= dat->size)
    return DWG_ERR_VALUEOUTOFBOUNDS;
  if (!FIELD_VALUE (acis_empty))
    {
      FIELD_B (unknown, 0);
      IF_ENCODE_FROM_EARLIER {
        FIELD_VALUE (version) = 1;
      }
      FIELD_BS (version, 70);
      // which is SAT format ACIS 4.0 (since r2000+)
      if (FIELD_VALUE (version) == 1)
        {
          do
            {
              FIELD_VALUE (encr_sat_data) = (char**)
                realloc (FIELD_VALUE (encr_sat_data), (i+1) * sizeof (char*));
              FIELD_VALUE (block_size) = (BITCODE_BL*)
                realloc (FIELD_VALUE (block_size), (i+1) * sizeof (BITCODE_BL));
              if (!FIELD_VALUE (encr_sat_data) || !FIELD_VALUE (block_size))
                return DWG_ERR_OUTOFMEM;
              FIELD_BL (block_size[i], 0);
              if (FIELD_VALUE (block_size[i]) > 0
                  && AVAIL_BITS (dat) > 8 * FIELD_VALUE (block_size[i]))
                {
                  FIELD_TFv (encr_sat_data[i], FIELD_VALUE (block_size[i]), 1);
                  if (!FIELD_VALUE (encr_sat_data) || !FIELD_VALUE (encr_sat_data[i]))
                    FIELD_VALUE (block_size[i]) = 0;
                  total_size += FIELD_VALUE (block_size[i]);
                }
              else
                {
                  FIELD_VALUE (encr_sat_data[i]) = (char*)calloc (1, 1);
                  FIELD_VALUE (block_size[i]) = 0;
                }
            }
          while (FIELD_VALUE (block_size[i++]) > 0 && AVAIL_BITS (dat) >= 16); // crc RS
          if (!FIELD_VALUE (encr_sat_data) || !FIELD_VALUE (block_size))
            {
              free (FIELD_VALUE (block_size));
              free (FIELD_VALUE (encr_sat_data));
              return DWG_ERR_VALUEOUTOFBOUNDS;
            }

          // de-obfuscate SAT data
          FIELD_VALUE (acis_data) = (BITCODE_RC *)malloc (total_size + 1);
          num_blocks = i - 1;
          FIELD_VALUE (num_blocks) = num_blocks;
          LOG_TRACE ("num_blocks: " FORMAT_BL "\n", FIELD_VALUE (num_blocks));
          acis_data_idx = 0;
          for ( i = 0; i < num_blocks; i++)
            {
              for (j = 0; j < FIELD_VALUE (block_size[i]); j++)
                {
                  if ((BITCODE_RC)FIELD_VALUE (encr_sat_data[i][j]) <= 32)
                    {
                      FIELD_VALUE (acis_data)[acis_data_idx++]
                        = (BITCODE_RC)FIELD_VALUE (encr_sat_data[i][j]);
                    }
                  else
                    {
                      FIELD_VALUE (acis_data)[acis_data_idx++]
                        = 159 - (BITCODE_RC)FIELD_VALUE (encr_sat_data[i][j]);
                    }
                }
            }
          FIELD_VALUE (acis_data)[acis_data_idx] = '\0';
          // DXF 1 + 3 if >255
          LOG_TRACE ("acis_data:\n%s\n", FIELD_VALUE (acis_data));
        }
      else if (FIELD_VALUE (version)==2)
        /* version 2, SAB: binary, unencrypted SAT format for ACIS 7.0/ShapeManager.
           ACIS versions:
           R14 release            106   (ACIS 1.6)
           R15 (2000) release     400   (ACIS 4.0)
           R18 (2004) release     20800 (ASM ShapeManager, forked from ACIS 7.0)
           R21 (2007) release     21200
           R24 (2010) release     21500
           R27 (2013) release     21800
           R?? (2018) release            223.0.1.1930
        */
        {
          FIELD_VALUE (block_size) = (BITCODE_BL*)calloc (2, sizeof (BITCODE_BL));
          FIELD_VALUE (encr_sat_data) = NULL;
          //TODO string in strhdl, even <r2007
          // either has_ds_data (r2013+) or the blob is here
          if (!obj->tio.entity->has_ds_data && dat->size > dat->byte + 1)
            {
              char *p;
              // Note that r2013+ has End-of-ASM-data (not ACIS anymore, but their fork)
              const char end[] = "\016\003End\016\002of\016\004ACIS\r\004data";
              const char end1[] = "\016\003End\016\002of\016\003ASM\r\004data";
              size_t pos = dat->byte;
              size_t size = dat->size - pos - 1;
              FIELD_VALUE (acis_data) = (unsigned char*)calloc (size, 1);
              // Binary SAB. unencrypted, documented format until "End-of-ACIS-data"
              // TODO There exist also SAB streams with a given number of records, but I
              // haven't seen them here. See dwg_convert_SAB_to_SAT1
              // Better read the first header line here, to check for num_records 0.
              // Or even parse the whole SAB format here, and store the SAB different
              // to the ASCII acis_data.
              FIELD_TFF (acis_data, size, 1); // SAB "ACIS BinaryFile"
              LOG_TRACE ("Unknown ACIS 2 SAB sab_size %zu starting at %zu\n",
                         size, pos);
              if ((p = (char *)memmem (_obj->acis_data, size, end,
                                       strlen (end))))
                {
                  size = p - (char*)_obj->acis_data;
                  size += strlen (end);
                  dat->byte = pos + size;
                  _obj->sab_size = size & 0xFFFFFFFF;
                  LOG_TRACE (
                      "Found End-of-ACIS-data. sab_size: %zu, new pos: %zu\n",
                      size, dat->byte);
                }
              else if ((p = (char *)memmem (_obj->acis_data, size, end1,
                                            strlen (end1))))
                {
                  size = p - (char*)_obj->acis_data;
                  size += strlen (end1);
                  dat->byte = pos + size;
                  _obj->sab_size = size & 0xFFFFFFFF;
                  LOG_TRACE (
                      "Found End-of-ASM-data. sab_size: %zu, new pos: %zu\n",
                      size, dat->byte);
                }
              else
                LOG_TRACE ("No End-of-ACIS or ASM data marker found\n");
              _obj->block_size[0] = _obj->sab_size = size & 0xFFFFFFFF;
            }
          else
            LOG_WARN ("SAB from AcDs blob not yet implemented");
          //total_size = FIELD_VALUE (_obj->block_size[0]);
        }
    }
  return error;
}
#else
#define DECODE_3DSOLID {}
#define FREE_3DSOLID {}
#endif //#if IS_DECODER

#ifdef IS_ENCODER
#  define ENCODE_3DSOLID                                                      \
    encode_3dsolid (dat, hdl_dat, obj, (Dwg_Entity_3DSOLID *)_obj);
static int
encode_3dsolid (Bit_Chain *dat, Bit_Chain *hdl_dat, Dwg_Object *restrict obj,
                Dwg_Entity_3DSOLID *restrict _obj)
{
  Dwg_Data* dwg = obj->parent;
  BITCODE_BL i = 0;
  BITCODE_BL num_blocks = FIELD_VALUE (num_blocks);
  int acis_data_idx = 0;
  int error = 0;

  FIELD_B (acis_empty, 290);
  if (!FIELD_VALUE (acis_empty))
    {
      FIELD_B (unknown, 0);
      FIELD_BS (version, 70);
      // which is SAT format ACIS 4.0 (since r2000+)
      if (FIELD_VALUE (version) == 1)
        {
          // from decode and indxf we already have all fields.
          // from other importers we have acis_data, but maybe not
          // encr_sat_data.
          if (!FIELD_VALUE (block_size))
            {
              if (!FIELD_VALUE (acis_data))
                {
                  VALUE_RL (0, 0);
                  return error;
                }
              // Later split into 4096 byte sized blocks
              FIELD_VALUE (block_size)
                  = (BITCODE_BL *)calloc (2, sizeof (BITCODE_BL));
              FIELD_VALUE (block_size[0])
                  = strlen ((char *)FIELD_VALUE (acis_data)) & 0xFFFFFFFF;
              FIELD_VALUE (block_size[1]) = 0;
              LOG_TRACE ("default block_size[0] = %d\n",
                         (int)FIELD_VALUE (block_size[0]));
              num_blocks = 1;
            }
          /* insecure. e.g. oss-fuzz issue 32165
             all inputs: dwg, injson and indxf have correct num_blocks values
          else if (!num_blocks)
            {
              num_blocks = 0;
              for (i = 0; FIELD_VALUE (block_size[i]); i++)
                num_blocks++;
            }
          */
          LOG_TRACE ("acis_data:\n%s\n", FIELD_VALUE (acis_data));
          for (i = 0; FIELD_VALUE (block_size[i]) && i < num_blocks; i++)
            {
              if (!FIELD_VALUE (encr_sat_data[i]))
                {
                  if (!FIELD_VALUE (acis_data))
                    {
                      VALUE_RL (0, 0);
                      return error;
                    }
                  // global acis_data_idx is needed for the global acis_data
                  FIELD_VALUE (encr_sat_data[i])
                    = dwg_encrypt_SAT1 (FIELD_VALUE (block_size[i]),
                                    FIELD_VALUE (acis_data), &acis_data_idx);
                  LOG_TRACE ("dwg_encrypt_SAT1 %d\n", i);
                }
              FIELD_BL (block_size[i], 0);
              FIELD_TF (encr_sat_data[i], FIELD_VALUE (block_size[i]), 1);
            }
          /*
          if (num_blocks > FIELD_VALUE (num_blocks))
            {
              FIELD_VALUE (block_size) = (BITCODE_BL*)realloc (FIELD_VALUE (block_size), (num_blocks + 1) * sizeof (BITCODE_BL));
              FIELD_VALUE (num_blocks) = num_blocks;
            }
          */
          FIELD_BL (block_size[num_blocks], 0);
        }
      else //if (FIELD_VALUE (version)==2)
        {
          if (_obj->acis_data && _obj->sab_size)
            {
              LOG_TRACE ("acis_data [TF %u 1]:\n%.*s\n", (unsigned)FIELD_VALUE (sab_size),
                         15, FIELD_VALUE (acis_data));
              // Binary SAB, unencrypted
              if (obj->tio.entity->has_ds_data)
                {
                  LOG_WARN ("Disable SAB from AcDs blob"); // TODO AcDs support
                  obj->tio.entity->has_ds_data = 0;
                }
              bit_write_TF (dat, _obj->acis_data, _obj->sab_size);
              LOG_TRACE_TF (&_obj->acis_data[15], (int)(_obj->sab_size - 15));
            }
        }
    }
  return error;
}
#else
#define ENCODE_3DSOLID {}
#define FREE_3DSOLID {}
#endif //#if IS_ENCODER

#ifdef IS_FREE
#undef FREE_3DSOLID
#define FREE_3DSOLID {} free_3dsolid (obj, (Dwg_Entity_3DSOLID *)_obj);
static int free_3dsolid (Dwg_Object *restrict obj, Dwg_Entity_3DSOLID *restrict _obj)
{
  int error = 0;
  Bit_Chain *dat = &pdat;

  if (!FIELD_VALUE (acis_empty))
    {
      if (FIELD_VALUE (encr_sat_data))
        {
          LOG_HANDLE ("Free %s.num_blocks %u\n", obj->name, _obj->num_blocks)
          for (BITCODE_BL i = 0; i <= FIELD_VALUE (num_blocks); i++)
            {
              if (FIELD_VALUE (encr_sat_data[i]) != NULL)
                FIELD_TF (encr_sat_data[i], block_size[i], 0);
            }
        }
      FREE_IF (FIELD_VALUE (encr_sat_data));
      FREE_IF (FIELD_VALUE (block_size));
    }
  FREE_IF (FIELD_VALUE (acis_data));
  return error;
}
#else
#define FREE_3DSOLID {}
#endif

#define COMMON_3DSOLID                                                        \
  FIELD_B (wireframe_data_present, 0);                                        \
  if (FIELD_VALUE (wireframe_data_present))                                   \
    {                                                                         \
      FIELD_B (point_present, 0);                                             \
      if (FIELD_VALUE (point_present))                                        \
        {                                                                     \
          FIELD_3BD (point, 0);                                               \
        }                                                                     \
      else                                                                    \
        {                                                                     \
          FIELD_VALUE (point.x) = 0;                                          \
          FIELD_VALUE (point.y) = 0;                                          \
          FIELD_VALUE (point.z) = 0;                                          \
        }                                                                     \
      FIELD_BL (isolines, 0);                                                 \
      FIELD_B (isoline_present, 0);                                           \
      if (FIELD_VALUE (isoline_present))                                      \
        {                                                                     \
          FIELD_BL (num_wires, 0);                                            \
          REPEAT (num_wires, wires, Dwg_3DSOLID_wire)                         \
          REPEAT_BLOCK                                                        \
            WIRESTRUCT_fields (wires[rcount1])                                \
            SET_PARENT (wires[rcount1], (Dwg_Entity__3DSOLID *)_obj);         \
          END_REPEAT_BLOCK                                                    \
          END_REPEAT (wires);                                                 \
          FIELD_BL (num_silhouettes, 0);                                      \
          REPEAT (num_silhouettes, silhouettes, Dwg_3DSOLID_silhouette)       \
          REPEAT_BLOCK                                                        \
            SUB_FIELD_BL (silhouettes[rcount1], vp_id, 0);                    \
            SUB_FIELD_3BD (silhouettes[rcount1], vp_target, 0); /* ?? */      \
            SUB_FIELD_3BD (silhouettes[rcount1], vp_dir_from_target, 0);      \
            SUB_FIELD_3BD (silhouettes[rcount1], vp_up_dir, 0);               \
            SUB_FIELD_B (silhouettes[rcount1], vp_perspective, 0);            \
            SUB_FIELD_B (silhouettes[rcount1], has_wires, 0);                 \
            if (_obj->silhouettes[rcount1].has_wires)                         \
              {                                                               \
                SUB_FIELD_BL (silhouettes[rcount1], num_wires, 0);            \
                REPEAT2 (silhouettes[rcount1].num_wires,                      \
                         silhouettes[rcount1].wires, Dwg_3DSOLID_wire)        \
                REPEAT_BLOCK                                                  \
                  WIRESTRUCT_fields (silhouettes[rcount1].wires[rcount2])     \
                  SET_PARENT (silhouettes[rcount1].wires[rcount2], (Dwg_Entity__3DSOLID *)_obj); \
                END_REPEAT_BLOCK                                              \
                END_REPEAT (silhouettes[rcount1].wires);                      \
              }                                                               \
            SET_PARENT (silhouettes[rcount1], (Dwg_Entity__3DSOLID *)_obj);    \
          END_REPEAT_BLOCK                                                    \
          END_REPEAT (silhouettes);                                           \
        }                                                                     \
    }                                                                         \
                                                                              \
  FIELD_B (acis_empty_bit, 0); /* ?? */                                       \
  if (FIELD_VALUE (version) > 1)                                              \
    {                                                                         \
      SINCE (R_2007)                                                          \
      {                                                                       \
        FIELD_BL (num_materials, 0);                                          \
        REPEAT (num_materials, materials, Dwg_3DSOLID_material)               \
        REPEAT_BLOCK                                                          \
          SUB_FIELD_BL (materials[rcount1], array_index, 0);                  \
          SUB_FIELD_BL (materials[rcount1], mat_absref, 0); /* ?? */          \
          SUB_FIELD_HANDLE (materials[rcount1], material_handle, 5, 0);       \
          SET_PARENT (materials[rcount1], (Dwg_Entity__3DSOLID *)_obj);       \
        END_REPEAT_BLOCK                                                      \
        END_REPEAT (materials);                                               \
      }                                                                       \
    }                                                                         \
  SINCE (R_2013)                                                              \
  {                                                                           \
    FIELD_B (has_revision_guid, 0);                                           \
    DXF {                                                                     \
      if (!_obj->revision_guid[0])                                            \
        {                                                                     \
          VALUE_TFF ("{00000000-0000-0000-0000-000000000000}", 2)             \
          LOG_WARN ("empty revision_guid");                                   \
        }                                                                     \
      else                                                                    \
        {                                                                     \
          FIELD_TFF (revision_guid, 38, 2);                                   \
        }                                                                     \
    }                                                                         \
    else                                                                      \
    {                                                                         \
      FIELD_BL (revision_major, 0);                                           \
      FIELD_BS (revision_minor1, 0);                                          \
      FIELD_BS (revision_minor2, 0);                                          \
      FIELD_TFFx (revision_bytes, 8, 0);                                      \
      DECODER { dxf_3dsolid_revisionguid ((Dwg_Entity_3DSOLID *)_obj); }      \
    }                                                                         \
    FIELD_BL (end_marker, 0);                                                 \
  }                                                                           \
                                                                              \
  DXF {                                                                       \
    SINCE (R_2007) {                                                          \
      SUBCLASS (AcDb3dSolid);                                                 \
      FIELD_HANDLE (history_id, 4, 350);                                      \
    }                                                                         \
  } else if (FIELD_VALUE (version) > 1) {                                     \
      FIELD_HANDLE (history_id, 4, 350);                                      \
  }                                                                           \
  FREE { FIELD_HANDLE (history_id, 4, 350); }

#define ACTION_3DSOLID \
  SUBCLASS (AcDbModelerGeometry); \
  DXF_OR_PRINT { \
    DXF_3DSOLID \
  } \
  DECODER { \
    DECODE_3DSOLID \
  } \
  ENCODER { \
    ENCODE_3DSOLID \
  } \
  JSON { \
    JSON_3DSOLID \
  } \
  FREE_3DSOLID \
  COMMON_3DSOLID

/* (37) */
DWG_ENTITY (REGION)
  ACTION_3DSOLID;
  COMMON_ENTITY_HANDLE_DATA;
DWG_ENTITY_END

/* (38) */
DWG_ENTITY (_3DSOLID)
  ACTION_3DSOLID;
  COMMON_ENTITY_HANDLE_DATA;
DWG_ENTITY_END

/* (39) */
DWG_ENTITY (BODY)
  ACTION_3DSOLID;
  COMMON_ENTITY_HANDLE_DATA;
DWG_ENTITY_END

/* (40) r13+ only */
DWG_ENTITY (RAY)
  SUBCLASS (AcDbRay)
  FIELD_3BD (point, 10);
  FIELD_3BD (vector, 11);
  COMMON_ENTITY_HANDLE_DATA;
DWG_ENTITY_END

/* (41) r13+ only */
DWG_ENTITY (XLINE)
  SUBCLASS (AcDbXline)
  FIELD_3BD (point, 10);
  FIELD_3BD (vector, 11);
  COMMON_ENTITY_HANDLE_DATA;
DWG_ENTITY_END

/* (42) */
DWG_OBJECT (DICTIONARY)

#ifdef IS_DXF
  SUBCLASS (AcDbDictionary)
  SINCE (R_13c3)
    FIELD_RC0 (is_hardowner, 280);
  SINCE (R_2000)
    FIELD_RC0 (cloning, 281);
#else
  FIELD_BL (numitems, 0);
  SINCE (R_13c3) {
    SINCE (R_2000)
      {
        IF_ENCODE_FROM_EARLIER {
          FIELD_VALUE (cloning) = FIELD_VALUE (is_hardowner) & 0xffff;
        }
        FIELD_BS (cloning, 281);
      }
    if (dat->version != R_13c3 || dwg->header.maint_version > 4)
      FIELD_RC (is_hardowner, 280);
  }
  VALUEOUTOFBOUNDS (numitems, 10000)
#endif

#ifdef IS_DXF
  if (FIELD_VALUE (itemhandles) && FIELD_VALUE (texts)) {
     REPEAT (numitems, texts, T)
      {
        int dxf = FIELD_VALUE (is_hardowner) & 1 ? 360 : 350;
        // ACAD_SORTENTS, ACAD_FILTER and SPATIAL are always hard 360
        if (dxf == 350 && dat->from_version >= R_2007)
          {
            char *text = FIELD_VALUE (texts[rcount1]);
#ifdef HAVE_NATIVE_WCHAR2
            const wchar_t *wstr1 = L"ACAD_SORTENTS";
            const wchar_t *wstr2 = L"ACAD_FILTER";
            const wchar_t *wstr3 = L"SPATIAL";
#else
            const uint8_t wstr1[]
                = { 'A', 0, 'C', 0, 'A', 0, 'D', 0, '_', 0, 'S', 0,
                    'O', 0, 'R', 0, 'T', 0, 'E', 0, 'N', 0, 'T', 0,  'S',
                     0,  0,  0 };
            const uint8_t wstr2[]
                = { 'A', 0, 'C', 0, 'A', 0, 'D', 0, '_', 0, 'F', 0,
                    'I', 0, 'L', 0, 'T', 0, 'E', 0, 'R', 0,  0,  0 };
            const uint8_t wstr3[] = { 'S', 0, 'P', 0, 'A', 0, 'T', 0,
                                      'I', 0, 'A', 0, 'L', 0,  0,  0 };
#endif
            if (text && (bit_eq_TU (text, (BITCODE_TU)wstr1) ||
                         bit_eq_TU (text, (BITCODE_TU)wstr2) ||
                         bit_eq_TU (text, (BITCODE_TU)wstr3)))
              dxf = 360;
          }
        else if (dxf == 350)
          {
            char *text = FIELD_VALUE (texts[rcount1]);
            if (!text)
              ;
            else if (strEQc (text, "ACAD_SORTENTS") ||
                     strEQc (text, "ACAD_FILTER") ||
                     strEQc (text, "SPATIAL"))
              dxf = 360;
          }
        FIELD_T (texts[rcount1], 3);
        VALUE_HANDLE (_obj->itemhandles[rcount1], itemhandles, 2, dxf);
      }
      END_REPEAT (texts)
    }
#elif defined (IS_JSON)
  // use a simple map: "items": { "text": [ handle ], ... }
  // the texts are all unique
  RECORD (items);
  if (FIELD_VALUE (itemhandles) && FIELD_VALUE (texts)) {
    for (rcount1 = 0; rcount1 < _obj->numitems; rcount1++)
      {
        FIRSTPREFIX
        VALUE_T (_obj->texts[rcount1]);
        fprintf (dat->fh, ": ");
        VALUE_HANDLE (_obj->itemhandles[rcount1], itemhandles, 2, 350);
      }
  }
  ENDRECORD()
#else
  FIELD_VECTOR_T (texts, T, numitems, 3);
#endif

  START_OBJECT_HANDLE_STREAM;
#if !defined(IS_DXF) && !defined (IS_JSON)
  // or DXF 360 if is_hardowner
  HANDLE_VECTOR_N (itemhandles, FIELD_VALUE (numitems), 2, 0);
#endif

DWG_OBJECT_END

// DXF as ACDBDICTIONARYWDFLT
DWG_OBJECT (DICTIONARYWDFLT)

#ifdef IS_DXF
  SUBCLASS (AcDbDictionary)
  SINCE (R_2000)
  {
    if (FIELD_VALUE (is_hardowner))
      FIELD_RC (is_hardowner, 280);
    FIELD_BS (cloning, 281);
  }
#else
  FIELD_BL (numitems, 0);
  // cloning from DICTIONARY
  SINCE (R_13c3) {
    SINCE (R_2000)
      {
        IF_ENCODE_FROM_EARLIER {
          FIELD_VALUE (cloning) = FIELD_VALUE (is_hardowner) & 0xffff;
        }
        FIELD_BS (cloning, 281);
      }
    if (dat->version != R_13c3 || dwg->header.maint_version > 4)
      FIELD_RC (is_hardowner, 280);
  }
#endif
  VALUEOUTOFBOUNDS (numitems, 10000)
#ifdef IS_DXF
    if (FIELD_VALUE (itemhandles) && FIELD_VALUE (texts)) {
      REPEAT (numitems, texts, T)
      {
        int dxf = FIELD_VALUE (is_hardowner) & 1 ? 360 : 350;
        FIELD_T (texts[rcount1], 3);
        VALUE_HANDLE (_obj->itemhandles[rcount1], itemhandles, 2, dxf);
      }
      END_REPEAT (texts)
    }
#elif defined (IS_JSON)
  // use a simple map: "items": { "text": [ handle ], ... }
  // the texts are all unique
  RECORD (items);
  if (FIELD_VALUE (itemhandles) && FIELD_VALUE (texts)) {
    for (rcount1 = 0; rcount1 < _obj->numitems; rcount1++)
      {
        FIRSTPREFIX
        VALUE_T (_obj->texts[rcount1]);
        fprintf (dat->fh, ": ");
        VALUE_HANDLE (_obj->itemhandles[rcount1], itemhandles, 2, 350);
      }
  }
  ENDRECORD()
#else
  FIELD_VECTOR_T (texts, T, numitems, 3);
#endif

  START_OBJECT_HANDLE_STREAM;
#if !defined(IS_DXF) && !defined (IS_JSON)
  IF_FREE_OR_SINCE (R_2000)
    {
      HANDLE_VECTOR_N (itemhandles, FIELD_VALUE (numitems), 2, 350);
    }
#endif
  SUBCLASS (AcDbDictionaryWithDefault)
  FIELD_HANDLE (defaultid, 5, 340);

DWG_OBJECT_END

/*(43) pre-R13c4 OLE 1 only.
 converted on opening to OLE2FRAME on demand
 */
DWG_ENTITY (OLEFRAME)

  //SUBCLASS (AcDbFrame)
  //SUBCLASS (AcDbOleFrame)
  FIELD_BS (flag, 70);
  SINCE (R_2000) {
    FIELD_BS (mode, 0);
  }

  ENCODER {
    if (FIELD_VALUE (data_size) && !FIELD_VALUE (data))
      FIELD_VALUE (data_size) = 0;
  }
#ifndef IS_JSON
  FIELD_BL (data_size, 90);
#endif
  FIELD_BINARY (data, FIELD_VALUE (data_size), 310);

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/* (44) */
DWG_ENTITY (MTEXT)

  SUBCLASS (AcDbMText)
  FIELD_3BD (ins_pt, 10);
  FIELD_3BD (extrusion, 210);
  FIELD_3BD (x_axis_dir, 11);

  DXF {
    FIELD_BD (text_height, 40);
    FIELD_BD (rect_width, 41);
    SINCE (R_2007) {
      FIELD_BD (rect_height, 46);
    }
  } else {
    FIELD_BD (rect_width, 41);
    SINCE (R_2007) {
      FIELD_BD (rect_height, 46);
    }
    FIELD_BD (text_height, 40);
  }
  FIELD_BS (attachment, 71);
  FIELD_BS (flow_dir, 72);
  FIELD_BD (extents_height, 0); // not in DXF, only as Embedded Object below
  FIELD_BD (extents_width, 0);
  // FIXME DXF break
  FIELD_T (text, 1); // or 3 if len >250
  /* in DXF only if non-default style */
  FIELD_HANDLE0 (style, 5, 7);

  SINCE (R_2000)
    {
      FIELD_BS (linespace_style, 73);
      FIELD_BD (linespace_factor, 44);
      FIELD_B (unknown_b0, 0);
    }
  SINCE (R_2004)
    {
      FIELD_BL0 (bg_fill_flag, 90);
      if (FIELD_VALUE (bg_fill_flag) & (dat->version <= R_2018 ? 1 : 16))
        {
          FIELD_BL (bg_fill_scale, 45); // def: 1.5
          FIELD_CMC (bg_fill_color, 63);
          FIELD_BL (bg_fill_trans, 441);
        }
    }
  SINCE (R_2018)
  {
    FIELD_B (is_not_annotative, 0);
    if (FIELD_VALUE (is_not_annotative))
      {
        // AnnotScaleObject
        DXF { VALUE_TFF ( "Embedded Object", 101 ); }
        FIELD_BS (class_version, 0); // 1-4
        VALUEOUTOFBOUNDS (class_version, 10)
        FIELD_B (default_flag, 70);   // def: 1
        FIELD_HANDLE (appid, 5, 0);
        // redundant fields
        FIELD_BL (ignore_attachment, 0); // not in DXF. prev as BS
        DXF {
          FIELD_3BD (ins_pt, 10);
          FIELD_3BD (x_axis_dir, 11);
        } else {
          FIELD_3BD (x_axis_dir, 11);
          FIELD_3BD (ins_pt, 10);
        }
        FIELD_BD (rect_width, 40);
        FIELD_BD (rect_height, 41);
        DXF {
          FIELD_BD (extents_width, 42);
          FIELD_BD (extents_height, 43);
        } else {
          FIELD_BD (extents_height, 43);
          FIELD_BD (extents_width, 42);
        }
        // end redundant fields

        FIELD_BS (column_type, 71);
        if (FIELD_VALUE (column_type))
          {
            if (FIELD_VALUE (column_type) == 1)
              {
                FIELD_VALUE (num_column_heights) = 0;
                FIELD_BL (numfragments, 72);
              }
            else
              {
                FIELD_BL (num_column_heights, 72);
              }
            FIELD_BD (column_width, 44);
            FIELD_BD (gutter, 45);
            FIELD_B (auto_height, 73);
            FIELD_B (flow_reversed, 74);
            if (!FIELD_VALUE (auto_height) && FIELD_VALUE (column_type) == 2)
              {
                FIELD_VECTOR (column_heights, BD, num_column_heights, 46);
              }
          }
      }
  }
  COMMON_ENTITY_HANDLE_DATA;
DWG_ENTITY_END

/* (45) unstable */
DWG_ENTITY (LEADER)

  //SUBCLASS (AcDbCurve)
  SUBCLASS (AcDbLeader)
  DXF { FIELD_HANDLE (dimstyle, 5, 3); }
  FIELD_B (unknown_bit_1, 0);
  DXF { FIELD_B (arrowhead_on, 71); }
  FIELD_BS (annot_type, 73); // 0: text, 1: tol, 2: insert, 3 (def): none
  LOG_LEADER_ANNOTTYPE
  FIELD_BS (path_type, 72); // 0: straight, 1: spline
  LOG_LEADER_PATHTYPE
  DXF {
    if (FIELD_VALUE (hookline_dir) || FIELD_VALUE (hookline_on))
      FIELD_B (hookline_dir, 74);
    FIELD_B0 (hookline_on, 75);
    FIELD_BD (box_height, 40);
    FIELD_BD (box_width, 41);
  }
  FIELD_BL (num_points, 76);
  FIELD_3DPOINT_VECTOR (points, num_points, 10);
  FIELD_3DPOINT (origin, 0);
  FIELD_3DPOINT (extrusion, 210);
  FIELD_3DPOINT (x_direction, 211);
  FIELD_3DPOINT (inspt_offset, 212);

  VERSIONS (R_14, R_2007) {
    FIELD_3DPOINT (endptproj, 0);
  }
  VERSIONS (R_13b1, R_14) {
    FIELD_BD (dimgap, 0);
  }

  FIELD_BD (box_height, 0);
  FIELD_BD (box_width, 0);
  FIELD_B (hookline_dir, 0); // if hook line is on x direction
  FIELD_B (arrowhead_on, 0);
  ENCODER {
    if (FIELD_VALUE (hookline_on))
      FIELD_VALUE(arrowhead_type) |= 8;
  }
  FIELD_BSx (arrowhead_type, 0);
  DECODER {
    // Note that ODA doesn't spec it, and ODA's code does
    // take bit 1 from path_type instead
    FIELD_VALUE (hookline_on) = FIELD_VALUE (arrowhead_type) & 8 ? 0 : 1;
    LOG_TRACE("=> hookline_on: %d [B 75]\n", FIELD_VALUE (hookline_on));
  }
  JSON {
    FIELD_B (hookline_on, 0);
  }
  VERSIONS (R_13b1, R_14)
    {
      FIELD_BD (dimasz, 0);
      FIELD_B (unknown_bit_2, 0);
      FIELD_B (unknown_bit_3, 0);
      FIELD_BS (unknown_short_1, 0);
      FIELD_BS (byblock_color, 77);
      FIELD_B (unknown_bit_4, 0);
      FIELD_B (unknown_bit_5, 0);
    }

  SINCE (R_2000)
    {
      FIELD_B (unknown_bit_4, 0);
      FIELD_B (unknown_bit_5, 0);
    }

  COMMON_ENTITY_HANDLE_DATA;
  SINCE (R_13b1) {
    FIELD_HANDLE (associated_annotation, 2, 340);
  }
  FIELD_HANDLE (dimstyle, 5, 0); // ODA bug, DXF documented as 2

DWG_ENTITY_END

/* (46) */
DWG_ENTITY (TOLERANCE)

  SUBCLASS (AcDbFcf)   // for Feature Control Frames
  DXF { FIELD_HANDLE (dimstyle, 5, 3); }
  VERSIONS (R_13b1, R_14)
    {
      FIELD_BS (unknown_short, 0); //ODA typo: Spec says S instead of BS.
      FIELD_BD (height, 0);
      FIELD_BD (dimgap, 0);
    }

  FIELD_3DPOINT (ins_pt, 10);
  FIELD_3DPOINT (x_direction, 11);
  DXF  { FIELD_BE (extrusion, 210); }
  else { FIELD_3DPOINT (extrusion, 210); }
  FIELD_T (text_value, 1);

  COMMON_ENTITY_HANDLE_DATA;
  FIELD_HANDLE (dimstyle, 5, 0);

DWG_ENTITY_END

/* (47) */
DWG_ENTITY (MLINE)

  SUBCLASS (AcDbMline)
  DXF { FIELD_HANDLE (mlinestyle, 5, 340); }
  FIELD_BD (scale, 40);
  FIELD_RC (justification, 70); /* spec typo. spec says EC instead of RC */
  LOG_MLINE_JUSTIFICATION
  FIELD_3DPOINT (base_point, 10);
  FIELD_3DPOINT (extrusion, 210);
  FIELD_BS (flags, 71);
  LOG_MLINE_FLAGS
  FIELD_RCu (num_lines, 73); //aka linesinstyle
  FIELD_BS (num_verts, 72);
  VALUEOUTOFBOUNDS (num_verts, 5000)

  REPEAT (num_verts, verts, Dwg_MLINE_vertex)
  REPEAT_BLOCK
      SUB_FIELD_3DPOINT (verts[rcount1], vertex, 11);
      SUB_FIELD_3DPOINT (verts[rcount1], vertex_direction, 12);
      SUB_FIELD_3DPOINT (verts[rcount1], miter_direction, 13);
      FIELD_VALUE (verts[rcount1].num_lines) = FIELD_VALUE (num_lines);

      REPEAT2 (num_lines, verts[rcount1].lines, Dwg_MLINE_line)
      REPEAT_BLOCK
          SUB_FIELD_BS (verts[rcount1].lines[rcount2], num_segparms, 74);
          VALUEOUTOFBOUNDS (verts[rcount1].lines[rcount2].num_segparms, 5000)
          FIELD_VECTOR (verts[rcount1].lines[rcount2].segparms, BD, verts[rcount1].lines[rcount2].num_segparms, 41)

          SUB_FIELD_BS (verts[rcount1].lines[rcount2], num_areafillparms, 75);
          VALUEOUTOFBOUNDS (verts[rcount1].lines[rcount2].num_areafillparms, 5000)
          FIELD_VECTOR (verts[rcount1].lines[rcount2].areafillparms, BD, verts[rcount1].lines[rcount2].num_areafillparms, 42)
          SET_PARENT (verts[rcount1].lines[rcount2], &_obj->verts[rcount1]);
      END_REPEAT_BLOCK
      END_REPEAT (verts[rcount1].lines);
      SET_PARENT_OBJ (verts[rcount1]);
  END_REPEAT_BLOCK
  END_REPEAT (verts);

  COMMON_ENTITY_HANDLE_DATA;
  FIELD_HANDLE (mlinestyle, 5, 0);

DWG_ENTITY_END

/* (48) */
DWG_OBJECT (BLOCK_CONTROL)

  PRE (R_13b1) {
    //VALUE_RS (obj->size, 0);
    FIELD_RS (num_entries, 70);
    FIELD_RSx (flags_r11, 0);
    JSON { KEY (address); }
    VALUE_RLx (obj->address & 0xFFFFFFFF, 0);
  } LATER_VERSIONS {
    FIELD_BL (num_entries, 70); // or BS?
  }
  CONTROL_HANDLE_STREAM;
  HANDLE_VECTOR (entries, num_entries, 2, 0);
  FIELD_HANDLE (model_space, 3, 0);
  SINCE (R_13b1) {
    FIELD_HANDLE (paper_space, 3, 0);
  }

DWG_OBJECT_END

/* (49/T1) */
DWG_TABLE (BLOCK_HEADER)

  //DXF: the name must be from the block_entity!
  COMMON_TABLE_FLAGS (Block)
  DXF {
    // not allowed to be skipped, can be 0
    VALUE_HANDLE (_obj->layout, layout, 5, 340);
    if (FIELD_VALUE (preview_size))
      {
        FIELD_BINARY (preview, FIELD_VALUE (preview_size), 310);
      }
    if (FIELD_VALUE (num_inserts))
      {
        VALUE_TFF ("{BLKREFS", 102);
        HANDLE_VECTOR (inserts, num_inserts, 4, 331);
        VALUE_TFF ("}", 102);
      }
    // The DXF TABLE.BLOCK_RECORD only has this. More later in the BLOCKS section.
    return 0;
  }

  PRE (R_13b1)
  {
    FIELD_RLx (block_offset_r11, 0);
    DECODER_OR_ENCODER {
      if (_obj->block_offset_r11 >= 0x40000000)
        {
          BITCODE_RL off = _obj->block_offset_r11 & 0x3fffffff;
          LOG_TRACE ("abs. offset => " FORMAT_RLx "\n",
                     off + dwg->header.blocks_start);
        }
      else
        {
          LOG_TRACE ("abs. offset => " FORMAT_RLx "\n",
                     _obj->block_offset_r11 + dwg->header.blocks_start);
        }
    }
    if (!obj->size || obj->size == 38)
      FIELD_RC (unknown_r11, 0);
    SINCE (R_11)
    {
      FIELD_HANDLE (block_entity, 2, 0);
      FIELD_RSd (flag2, 0);
    }
    FIELD_VALUE (anonymous)    = FIELD_VALUE (flag) & 1;
    FIELD_VALUE (hasattrs)     = FIELD_VALUE (flag) & 2;
    FIELD_VALUE (blkisxref)    = FIELD_VALUE (flag) & 4;
    FIELD_VALUE (xrefoverlaid) = FIELD_VALUE (flag) & 8;
  }
  SINCE (R_13b1) {
    FIELD_B (anonymous, 0); // bit 1
    FIELD_B (hasattrs, 0);  // bit 2
    FIELD_B (blkisxref, 0); // bit 4
    FIELD_B (xrefoverlaid, 0); // bit 8
  }
  SINCE (R_2000) {
    FIELD_B (loaded_bit, 0); // bit 32
  }
  SINCE (R_13b1) {
    FIELD_VALUE (flag) |= FIELD_VALUE (anonymous) |
                          FIELD_VALUE (hasattrs) << 1 |
                          FIELD_VALUE (blkisxref) << 2 |
                          FIELD_VALUE (xrefoverlaid) << 3;
  }
  SINCE (R_2004) { // but not in 2007
    FIELD_BL (num_owned, 0);
    if (FIELD_VALUE (num_owned) > 0xf00000)
      {
        LOG_WARN ("Unreasonable high num_owned value")
      }
  }

  SINCE (R_13b1) {
    FIELD_3DPOINT (base_pt, 10);
    FIELD_T (xref_pname, 1); // and 3
  }

  IF_FREE_OR_SINCE (R_2000)
    {
      FIELD_NUM_INSERTS (num_inserts, RL, 0);
      FIELD_T (description, 4);

#ifndef IS_JSON
      FIELD_BL (preview_size, 0);
#endif
      VALUEOUTOFBOUNDS (preview_size, 0xa00000)
      else
        {
          FIELD_BINARY (preview, FIELD_VALUE (preview_size), 310);
        }
    }

  SINCE (R_2007) // AC1020 aka R_2006
    {
      FIELD_BS (insert_units, 70);
      FIELD_B (explodable, 280);
      FIELD_RC (block_scaling, 281);
    }

  SINCE (R_13b1) {
    START_OBJECT_HANDLE_STREAM;
    FIELD_HANDLE (block_entity, 3, 0);
  }

  VERSIONS (R_13b1, R_2000)
    {
      if (!FIELD_VALUE (blkisxref) && !FIELD_VALUE (xrefoverlaid))
        {
          FIELD_HANDLE (first_entity, 4, 0);
          FIELD_HANDLE (last_entity, 4, 0);
        }
    }
  IF_FREE_OR_SINCE (R_2004)
    {
      if (FIELD_VALUE (num_owned) < 0xf00000) {
        HANDLE_VECTOR (entities, num_owned, 4, 0);
      }
    }
  IF_FREE_OR_SINCE (R_13b1) {
    FIELD_HANDLE (endblk_entity, 3, 0);
  }
  IF_FREE_OR_SINCE (R_2000)
    {
      if (FIELD_VALUE (num_inserts) && FIELD_VALUE (num_inserts) < 0xf00000) {
        HANDLE_VECTOR (inserts, num_inserts, 4, 0);
      }
      FIELD_HANDLE (layout, 5, 340);
    }

DWG_OBJECT_END

/* (50) */
DWG_OBJECT (LAYER_CONTROL)

  PRE (R_13b1) {
    //VALUE_RS (obj->size, 0);
    FIELD_RS (num_entries, 70);
    FIELD_RSx (flags_r11, 0);
    JSON { KEY (address); }
    VALUE_RLx (obj->address & 0xFFFFFFFF, 0);
  } LATER_VERSIONS {
    FIELD_BL (num_entries, 70);
  }
  CONTROL_HANDLE_STREAM;
  HANDLE_VECTOR (entries, num_entries, 2, 0);

DWG_OBJECT_END

/* (51/T2) */
DWG_TABLE (LAYER)

  COMMON_TABLE_FLAGS (Layer);
  PRE (R_13b1) {
    FIELD_CMC (color, 62);
    FIELD_HANDLE (ltype, 2, 6);
    if (obj->size == 38)
      FIELD_CAST (flag0, RC, BS, 0);

    DECODER {
      FIELD_VALUE (on)            = FIELD_VALUE (color.index) >= 0;
      FIELD_VALUE (frozen)        = FIELD_VALUE (flag) & 1;
      FIELD_VALUE (frozen_in_new) = FIELD_VALUE (flag) & 2;
      FIELD_VALUE (locked)        = FIELD_VALUE (flag) & 4;
    }
  }
  VERSIONS (R_13b1, R_14) {
    FIELD_B (frozen, 0); // bit 1
    FIELD_B (on, 0);     // really: negate the color
    FIELD_B (frozen_in_new, 0);
    FIELD_B (locked, 0);
  }
  SINCE (R_2000) {
    // separate DXF flag 70 from the internal DWG flag0 bitmask
    int flag0 = FIELD_VALUE (flag0);
    FIELD_BSx (flag0, 0); // -> 70,290,370
    flag0 = FIELD_VALUE (flag0);
    // DWG: frozen (1), on (2), frozen by default (4),
    //      locked (8), plotting flag (16), and linewt (mask with 0x03E0)
    FIELD_VALUE (frozen) = flag0 & 1;
    FIELD_VALUE (on) = !(flag0 & 2);
    FIELD_VALUE (frozen_in_new) = (flag0 & 4) ? 1 : 0;
    FIELD_VALUE (locked) = (flag0 & 8) ? 1 : 0;
    FIELD_VALUE (plotflag) = (flag0 & 16) ? 1 : 0;
    FIELD_VALUE (linewt) = (flag0 & 0x03E0) >> 5;
    // DXF: frozen (1), frozen by default in new viewports (2),
    //      locked (4), is_xref_ref (16), is_xref_resolved (32), is_xref_dep (64).
    FIELD_VALUE (flag) |= FIELD_VALUE (frozen) |
      (FIELD_VALUE (frozen_in_new) << 1) |
      (FIELD_VALUE (locked) << 2) |
      (FIELD_VALUE (is_xref_ref) << 3) |
      ((FIELD_VALUE (is_xref_resolved) ? 1 : 0) << 4) |
      (FIELD_VALUE (is_xref_dep) << 5);
    JSON {
      FIELD_RC (linewt, 370);
    }
  }
  SINCE (R_13b1) {
    FIELD_CMC (color, 62);
  }
  VERSIONS (R_13b1, R_14) {
    DECODER { FIELD_VALUE (on) = FIELD_VALUE (color.index) >= 0; }
    // for DWG
    FIELD_VALUE (flag0) |= FIELD_VALUE (frozen) |
      (FIELD_VALUE (frozen_in_new) << 1) |
      (FIELD_VALUE (locked) << 2) |
      (FIELD_VALUE (color.index) < 0 ? 32 : 0);
    // for DXF
    FIELD_VALUE (flag) |= FIELD_VALUE (frozen) |
      (FIELD_VALUE (frozen_in_new) << 1) |
      (FIELD_VALUE (locked) << 2) |
      (FIELD_VALUE (color.index) < 0 ? 32 : 0);
  }

  SINCE (R_13b1) {
    START_OBJECT_HANDLE_STREAM;
  }
  SINCE (R_2000) {
    FIELD_HANDLE (plotstyle, 5, 0);
  }
  SINCE (R_2007) {
    FIELD_HANDLE (material, 5, 0);
  }
  SINCE (R_13b1) {
    FIELD_HANDLE (ltype, 5, 6);
  }
  DXF {
    SINCE (R_2000) {
      if (_obj->name &&
          (bit_eq_T (dat, _obj->name, "Defpoints") ||
           bit_eq_T (dat, _obj->name, "DEFPOINTS")))
      {
        _obj->plotflag = 0;
        FIELD_B (plotflag, 290);
      } else {
        FIELD_B0 (plotflag, 290);
      }
    }
    SINCE (R_13b1) {
      int lw = dxf_cvt_lweight (FIELD_VALUE (linewt));
      KEY (linewt); VALUE_BSd (lw, 370);
    }
    SINCE (R_2000) {
      FIELD_HANDLE (plotstyle, 5, 390);
    }
    SINCE (R_2007) {
      DXF { FIELD_HANDLE (material, 5, 0); } // yet unstable class
      else {
        FIELD_HANDLE (material, 5, 347);
      }
    }
  }
  SINCE (R_2013) {
    FIELD_HANDLE (visualstyle, 5, 348);
  }
DWG_OBJECT_END

/* STYLE table (52) */
DWG_OBJECT (STYLE_CONTROL)

  PRE (R_13b1) {
    //VALUE_RS (obj->size, 0);
    FIELD_RS (num_entries, 70);
    FIELD_RSx (flags_r11, 0);
    JSON { KEY (address); }
    VALUE_RLx (obj->address & 0xFFFFFFFF, 0);
  } LATER_VERSIONS {
    FIELD_BL (num_entries, 70);
  }
  CONTROL_HANDLE_STREAM;
  HANDLE_VECTOR (entries, num_entries, 2, 0);

DWG_OBJECT_END

/* (53/T3) preR13+DXF: STYLE, documented as SHAPEFILE */
DWG_TABLE (STYLE)

  COMMON_TABLE_FLAGS (TextStyle)

  SINCE (R_13b1) {
    FIELD_B (is_shape, 0);        //wrong oda doc
    FIELD_B (is_vertical, 0);     //
    DECODER_OR_ENCODER {
      FIELD_VALUE (flag) |= (FIELD_VALUE (is_vertical) ? 4 : 0) +
                            (FIELD_VALUE (is_shape) ? 1 : 0);
      LOG_TRACE ("flag => %d [RC 70]\n", FIELD_VALUE (flag));
    }
  }
  PRE (R_13b1) {
    FIELD_RD (text_size, 40);
    FIELD_RD (width_factor, 41);
    FIELD_RD (oblique_angle, 50);
    FIELD_RC (generation, 71);
    LOG_TEXT_GENERATION
    FIELD_RD (last_height, 42);
    FIELD_TFv (font_file, 64, 3);
    SINCE (R_2_4)
      FIELD_TFv (bigfont_file, 64, 4);
    DECODER {
      FIELD_VALUE (is_shape)    = FIELD_VALUE (flag) & 4;
      FIELD_VALUE (is_vertical) = FIELD_VALUE (flag) & 1;
    }
  } LATER_VERSIONS {
    FIELD_BD (text_size, 40);
    FIELD_BD (width_factor, 41); // xScale
    FIELD_BD (oblique_angle, 50);
    FIELD_RC (generation, 71);
    LOG_TEXT_GENERATION
    FIELD_BD (last_height, 42);
    FIELD_T (font_file, 3);
    FIELD_T (bigfont_file, 4);
#ifdef IS_DXF
    {
      char _buf[256];
      char *s;
      // only print to DXF, if not in EED already
      if (FIELD_VALUE (font_file) && !dxf_has_STYLE_eed (dat, obj->tio.object))
        {
          if (IS_FROM_TU (dat)) {
            s = bit_convert_TU ((BITCODE_TU)FIELD_VALUE (font_file));
            strncpy (_buf, s, 255);
            free (s);
          }
          else {
            strncpy (_buf, FIELD_VALUE (font_file), 255);
          }
          _buf[255] = '\0';
          if ((s = strstr (_buf, ".ttf")) ||
              (s = strstr (_buf, ".TTF")))
            {
              *s = 0;
              VALUE_TFF ("ACAD", 1001);
              VALUE_TFF (_buf, 1000); // typeface
              VALUE_RL (34, 1071); // ttf_flags
            }
        }
    }
#endif

    START_OBJECT_HANDLE_STREAM;
    // Note: the ODA spec lists the ownerhandle as shapefile control here
  }

DWG_OBJECT_END

//(54): Unknown
//(55): Unknown

/* (56) */
DWG_OBJECT (LTYPE_CONTROL)

  PRE (R_13b1) {
    //VALUE_RS (obj->size, 0);
    FIELD_RS (num_entries, 70);
    FIELD_RSx (flags_r11, 0);
    JSON { KEY (address); }
    VALUE_RLx (obj->address & 0xFFFFFFFF, 0);
  } LATER_VERSIONS {
    FIELD_BS (num_entries, 70);
  }
  CONTROL_HANDLE_STREAM;
  HANDLE_VECTOR (entries, num_entries, 2, 0);
  SINCE (R_13b1) {
    FIELD_HANDLE (byblock, 3, 0);
    FIELD_HANDLE (bylayer, 3, 0);
  }

DWG_OBJECT_END

/* (57/T4)
 * Unstable, ACAD import errors
 */
DWG_TABLE (LTYPE)

  COMMON_TABLE_FLAGS (Linetype)

  PRE (R_13b1) {
    FIELD_TFv (description, 48, 3);
  }
  LATER_VERSIONS {
    FIELD_T (description, 3);
    FIELD_BD (pattern_len, 0); // total length
  }
  FIELD_RC (alignment, 72);
  FIELD_RCu (numdashes, 73);
  DXF { FIELD_BD (pattern_len, 40); }
  PRE (R_13b1)
  {
    FIELD_RD (pattern_len, 40);
#ifndef IS_JSON
    FIELD_VECTOR_INL (dashes_r11, RD, 12, 49);
#else
    FIELD_VECTOR_N (dashes_r11, RD, 12, 49);
#endif
    PRE (R_11) {
      if (obj->size > 187) // !! encode,add
        FIELD_RC (unknown_r11, 0);
    }
  }
  SINCE (R_13b1)
  {
    REPEAT (numdashes, dashes, Dwg_LTYPE_dash)
    REPEAT_BLOCK
      SUB_FIELD_BD (dashes[rcount1],length, 49);
      DXF {
        SUB_FIELD_BS (dashes[rcount1],shape_flag, 74);
        if (_obj->dashes[rcount1].shape_flag) // eg BATTING
          {
            SUB_FIELD_BS (dashes[rcount1],complex_shapecode, 75);
            SUB_FIELD_HANDLE (dashes[rcount1],style, 5, 340);
            SUB_FIELD_BD (dashes[rcount1],scale, 46);
            SUB_FIELD_BD (dashes[rcount1],rotation, 50); // absolute or relative
            SUB_FIELD_RD (dashes[rcount1],x_offset, 44);
            SUB_FIELD_RD (dashes[rcount1],y_offset, 45);
            if (_obj->dashes[rcount1].shape_flag & 2) // 10
              {
                SUB_FIELD_T (dashes[rcount1],text, 9);
              }
          }
      } else {
        SUB_FIELD_BS (dashes[rcount1],complex_shapecode, 75);
        SUB_FIELD_HANDLE (dashes[rcount1],style, 5, 340);
        SUB_FIELD_RD (dashes[rcount1],x_offset, 44);
        SUB_FIELD_RD (dashes[rcount1],y_offset, 45);
        SUB_FIELD_BD (dashes[rcount1],scale, 46);
        SUB_FIELD_BD (dashes[rcount1],rotation, 50);
        SUB_FIELD_BS (dashes[rcount1],shape_flag, 74);
      }
      DECODER {
        if (FIELD_VALUE (dashes[rcount1].shape_flag) & 2)
          FIELD_VALUE (has_strings_area) = 1;
        PRE (R_13b1) {
          FIELD_VALUE (pattern_len) += FIELD_VALUE (dashes[rcount1].length);
        }
      }
      SET_PARENT_OBJ (dashes[rcount1]);
    END_REPEAT_BLOCK
    END_REPEAT (dashes);
    
    UNTIL (R_2004) {
      FIELD_BINARY (strings_area, 256, 0);
      DECODER {
        unsigned int dash_i = 0;
        for (rcount1 = 0; _obj->strings_area && rcount1 < _obj->numdashes; rcount1++)
          {
            if (_obj->dashes[rcount1].shape_flag & 2)
              {
                if (dash_i >= 256)
                  {
                    LOG_ERROR ("dashes[%u] overflow @%u", rcount1, dash_i);
                    break;
                  }
                _obj->dashes[rcount1].text = (char*)&_obj->strings_area[dash_i];
                LOG_TRACE ("dashes[%u] @%u\n", rcount1, dash_i);
                dash_i += (unsigned)(strnlen (_obj->dashes[rcount1].text,
                                              256 - dash_i)
                                     + 1);
              }
          }
      }
    }
  }
  LATER_VERSIONS {
    if (FIELD_VALUE (has_strings_area)) {
      FIELD_BINARY (strings_area, 512, 0);
      DECODER {
        BITCODE_RS dash_i = 0;
        for (rcount1 = 0; _obj->strings_area && rcount1 < _obj->numdashes; rcount1++)
          {
            if (_obj->dashes[rcount1].shape_flag & 2)
              {
                if (dash_i >= 512)
                  {
                    LOG_ERROR ("dashes[%u] overflow @%u", rcount1, dash_i);
                    break;
                  }
                _obj->dashes[rcount1].text = (char *)&_obj->strings_area[dash_i];
                LOG_TRACE ("dashes[%u] @%u\n", rcount1, dash_i);
                dash_i += ((2 * bit_wcs2nlen ((BITCODE_TU)_obj->dashes[rcount1].text,
                                             256 - (dash_i / 2))) + 2 ) & 0xFFFF;
              }
          }
      }
    }
  } // r13

  SINCE (R_13b1) {
    START_OBJECT_HANDLE_STREAM;
  }

DWG_OBJECT_END

// (58) : Unknown
// (59) : Unknown

/* (60) */
DWG_OBJECT (VIEW_CONTROL)

  PRE (R_13b1) {
    //VALUE_RS (obj->size, 0);
    FIELD_RS (num_entries, 70);
    FIELD_RSx (flags_r11, 0);
    JSON { KEY (address); }
    VALUE_RLx (obj->address & 0xFFFFFFFF, 0);
  } LATER_VERSIONS {
    FIELD_BL (num_entries, 70);
  }
  CONTROL_HANDLE_STREAM;
  HANDLE_VECTOR (entries, num_entries, 2, 0);

DWG_OBJECT_END

/* (61/T5) */
DWG_TABLE (VIEW)

  COMMON_TABLE_FLAGS (View)

  // subclass AbstractViewTableRecord:
  PRE (R_13b1)
  {
    FIELD_RD (VIEWSIZE, 40);
    FIELD_2RD (VIEWCTR, 10);
    if (obj->size == 58)
      FIELD_RC (unknown_r11, 0);
    if (obj->size > 58)
      FIELD_RD (view_width, 41);
    if (obj->size > 66)
      FIELD_3RD (VIEWDIR, 11);
    if (obj->size > 89)
      FIELD_RS (flag_3d, 0);
    if (obj->size == 66 || obj->size == 92)
      FIELD_RC (unknown_r2, 0);
    SINCE (R_10) {
      FIELD_3RD (view_target, 12);
      FIELD_CAST (VIEWMODE, RS, 4BITS, 0);
      FIELD_RD (lens_length, 42); // defaults to 50.0
      FIELD_RD (front_clip_z, 43);
      FIELD_RD (back_clip_z, 44);
      FIELD_RD (twist_angle, 50);
      DXF {
        FIELD_CAST (VIEWMODE, RS, 4BITS, 71);
      }
    }
  }
  LATER_VERSIONS
  {
    FIELD_BD (VIEWSIZE, 40); // i.e view height
    FIELD_BD (view_width, 0);
    DECODER {
      FIELD_VALUE (aspect_ratio) = FIELD_VALUE (VIEWSIZE) == 0.0
        ? 0.0
        : FIELD_VALUE (view_width) / FIELD_VALUE (VIEWSIZE);
      LOG_TRACE ("aspect_ratio: %f (calc)\n", FIELD_VALUE (aspect_ratio))
    }
    JSON {
      FIELD_BD (aspect_ratio, 0);
    }
    // subclass ViInfo (shared with VPORT, but different DXF codes)
    FIELD_2RD (VIEWCTR, 10);
    DXF {
      FIELD_BD (view_width, 41);
      FIELD_3BD (VIEWDIR, 11);
    }
    FIELD_3BD (view_target, 12);
    FIELD_3BD (VIEWDIR, 0);
    FIELD_BD (twist_angle, 50);
    FIELD_BD (lens_length, 42);
    FIELD_BD (front_clip_z, 43);
    FIELD_BD (back_clip_z, 44);
    FIELD_4BITS (VIEWMODE, 71);
  }
  SINCE (R_2000) {
    FIELD_RC (render_mode, 281);
  }
  SINCE (R_2007) {
    IF_ENCODE_FROM_EARLIER {
      FIELD_VALUE (use_default_lights) = 1;
      FIELD_VALUE (default_lightning_type) = 1;
      FIELD_VALUE (ambient_color.index) = 250;
      //TODO FIELD_VALUE (ambient_color.rgb) = ?;
    }
    FIELD_HANDLE0 (background, 4, 332);
    FIELD_HANDLE0 (visualstyle, 5, 348);
    FIELD_B (use_default_lights, 292);
    FIELD_RC (default_lightning_type, 282);
    FIELD_BD (brightness, 141);
    FIELD_BD (contrast, 142);
    FIELD_CMC (ambient_color, 63);
    FIELD_HANDLE0 (sun, 3, 361);
  }
  // end of ViInfo

  // subclass ViewTableRecord:
  SINCE (R_13b1) {
    FIELD_B (is_pspace, 0);
    FIELD_VALUE (flag) |= FIELD_VALUE (is_pspace);
  }
  SINCE (R_2000) {
    FIELD_B (associated_ucs, 72);
    if (FIELD_VALUE (associated_ucs)) {
      FIELD_3BD (ucsorg, 110);
      FIELD_3BD (ucsxdir, 111);
      FIELD_3BD (ucsydir, 112);
      FIELD_BD (ucs_elevation, 146);
      FIELD_BS (UCSORTHOVIEW, 79);
      FIELD_HANDLE (base_ucs, 5, 346);
      FIELD_HANDLE0 (named_ucs, 5, 345);
    }
  }

  SINCE (R_2007) {
    FIELD_B (is_camera_plottable, 73);
  }
  SINCE (R_13b1) {
    START_OBJECT_HANDLE_STREAM;
  }
  SINCE (R_2007) {
    FIELD_HANDLE0 (livesection, 4, 334); // a SECTIONOBJECT?
  }

DWG_OBJECT_END

/* (62) */
DWG_OBJECT (UCS_CONTROL)

  PRE (R_13b1) {
    //VALUE_RS (obj->size, 0);
    FIELD_RS (num_entries, 70);
    FIELD_RSx (flags_r11, 0);
    JSON { KEY (address); }
    VALUE_RLx (obj->address & 0xFFFFFFFF, 0);
  } LATER_VERSIONS {
    FIELD_BS (num_entries, 70); //BS or BL?
  }
  CONTROL_HANDLE_STREAM;
  HANDLE_VECTOR (entries, num_entries, 2, 0);

DWG_OBJECT_END

/* (63/T6) */
DWG_TABLE (UCS)

  COMMON_TABLE_FLAGS (UCS)
  PRE (R_2000)
  {
    ENCODER {
      if (FIELD_VALUE (ucs_elevation))
        FIELD_VALUE (ucsorg).z = FIELD_VALUE (ucs_elevation);
    }
  }
  PRE (R_13b1)
  {
    FIELD_3RD (ucsorg, 10);
    FIELD_3RD (ucsxdir, 11);
    FIELD_3RD (ucsydir, 12);
  }
  LATER_VERSIONS
  {
    FIELD_3BD (ucsorg, 10);
    FIELD_3BD (ucsxdir, 11);
    FIELD_3BD (ucsydir, 12);
  }
  SINCE (R_2000)
  {
    DXF {
      FIELD_BS (UCSORTHOVIEW, 79);
      FIELD_BD (ucs_elevation, 146);
      if (FIELD_VALUE (UCSORTHOVIEW)) {
        FIELD_HANDLE0 (base_ucs, 5, 346);
      }
    } else {
      FIELD_BD0 (ucs_elevation, 146);
      FIELD_BS (UCSORTHOVIEW, 79);
    }
    FIELD_HANDLE (base_ucs, 5, 0);
    FIELD_HANDLE (named_ucs, 5, 0);

    FIELD_BS (num_orthopts, 0);
    REPEAT (num_orthopts, orthopts, Dwg_UCS_orthopts)
    REPEAT_BLOCK
      SUB_FIELD_BS (orthopts[rcount1],type, 71);
      SUB_FIELD_3BD (orthopts[rcount1],pt, 13);
      SET_PARENT_OBJ (orthopts[rcount1]);
    END_REPEAT_BLOCK
    END_REPEAT (orthopts);
  }
  SINCE (R_13b1) {
    START_OBJECT_HANDLE_STREAM;
  }
DWG_OBJECT_END

/* (64) */
DWG_OBJECT (VPORT_CONTROL)

  PRE (R_13b1) {
    //VALUE_RS (obj->size, 0);
    FIELD_RS (num_entries, 70);
    FIELD_RSx (flags_r11, 0);
    JSON { KEY (address); }
    VALUE_RLx (obj->address & 0xFFFFFFFF, 0);
  } LATER_VERSIONS {
    FIELD_BS (num_entries, 70); //BS or BL?
  }
  CONTROL_HANDLE_STREAM;
  HANDLE_VECTOR (entries, num_entries, 2, 0);

DWG_OBJECT_END

/* (65) */
DWG_TABLE (VPORT)

  COMMON_TABLE_FLAGS (Viewport)

  DXF { // has a different order of fields

    FIELD_2RD (lower_left, 10);
    FIELD_2RD (upper_right, 11);
    FIELD_2RD (VIEWCTR, 12);
    FIELD_2RD (SNAPBASE, 13);
    FIELD_2RD (SNAPUNIT, 14);
    FIELD_2RD (GRIDUNIT, 15);
    FIELD_3RD (VIEWDIR, 16);
    FIELD_3RD (view_target, 17);
    FIELD_RD (VIEWSIZE, 40);
    FIELD_RD (aspect_ratio, 41); // = view_width / VIEWSIZE
    FIELD_RD (lens_length, 42);
    FIELD_RD (front_clip_z, 43);
    FIELD_RD (back_clip_z, 44);
    FIELD_RD (SNAPANG, 50);
    FIELD_RD (view_twist, 51);
    FIELD_RS (UCSFOLLOW, 71);
    // FIELD_VALUE (VIEWMODE) |= (FIELD_VALUE (UCSFOLLOW) << 2);
    //VIEWMODE: UCSVP bit 0, ucs_at_origin bit 1, UCSFOLLOW bit 3
    FIELD_RS (circle_zoom, 72); // 1000
    FIELD_RS (FASTZOOM, 73);
    FIELD_RS (UCSICON, 74);
    FIELD_RS (SNAPMODE, 75);
    FIELD_RS (GRIDMODE, 76);
    FIELD_RS (SNAPSTYLE, 77);
    FIELD_RS (SNAPISOPAIR, 78);

    SINCE (R_2000) {
      FIELD_RS (render_mode, 281); // ODA has it as RC
      FIELD_RS (UCSVP, 65); // in DWG as bit 0 of VIEWMODE. ODA bug, documented as 71
      FIELD_3BD (ucsorg, 110);
      FIELD_3BD (ucsxdir, 111);
      FIELD_3BD (ucsydir, 112);
      FIELD_HANDLE0 (named_ucs, 5, 345);
      if (FIELD_VALUE (UCSORTHOVIEW))
        FIELD_HANDLE (base_ucs, 5, 346);
      FIELD_RS (UCSORTHOVIEW, 79);
      FIELD_RD (ucs_elevation, 146);
    }
    SINCE (R_2007) {
      FIELD_HANDLE0 (background, 4, 332);
      FIELD_HANDLE0 (visualstyle, 5, 348);
      FIELD_RS (grid_flags, 60);
      FIELD_RS (grid_major, 61);
      FIELD_B (use_default_lights, 292);
      FIELD_RC (default_lightning_type, 282);
      FIELD_RD (brightness, 141);
      FIELD_RD (contrast, 142);
      FIELD_CMC (ambient_color, 63);
      FIELD_HANDLE0 (sun, 5, 361);
    }

    //TODO convert back 1001 1070 xdata
    REACTORS (4);
    XDICOBJHANDLE (3);

  }
  /* end of DXF: now DWG */
  else {

    PRE (R_13b1) {
      FIELD_2RD (lower_left, 10);
      FIELD_2RD (upper_right, 11);
      FIELD_3RD (view_target, 17);
      FIELD_3RD (VIEWDIR, 16);
      FIELD_RD (view_twist, 51);
      FIELD_RD (VIEWSIZE, 40);
      FIELD_2RD (VIEWCTR, 12);
      FIELD_RD (aspect_ratio, 41);
      DECODER {
        FIELD_VALUE (view_width) = FIELD_VALUE (aspect_ratio) * FIELD_VALUE (VIEWSIZE);
        LOG_TRACE ("view_width: %f (calc)\n", FIELD_VALUE (view_width))
      }
      FIELD_RD (lens_length, 42);
      FIELD_RD (front_clip_z, 43);
      FIELD_RD (back_clip_z, 44);
      FIELD_RS (UCSFOLLOW, 71);
      FIELD_RS (circle_zoom, 72); //circle sides
      FIELD_RS (FASTZOOM, 73);
      FIELD_RS (UCSICON, 74);
      FIELD_RS (SNAPMODE, 75);
      FIELD_RS (GRIDMODE, 76);
      FIELD_RS (SNAPSTYLE, 77);
      FIELD_RS (SNAPISOPAIR, 78);
      FIELD_RD (SNAPANG, 50);
      FIELD_2RD (SNAPBASE, 13);
      FIELD_2RD (SNAPUNIT, 14);
      FIELD_2RD (GRIDUNIT, 15);
    }
    else // PRE (R_13
    {
      FIELD_BD (VIEWSIZE, 40);  // i.e view height
      FIELD_BD (view_width, 0); // -nan in example_2000
      DECODER {
        FIELD_VALUE (aspect_ratio) = FIELD_VALUE (VIEWSIZE) == 0.0
          ? 0.0
          : FIELD_VALUE (view_width) / FIELD_VALUE (VIEWSIZE);
        LOG_TRACE ("aspect_ratio: %f (calc)\n", FIELD_VALUE (aspect_ratio))
      }
      JSON {
        FIELD_BD (aspect_ratio, 0);
      }
      // subclass ViInfo (shared with VIEW, but different DXF codes)
      FIELD_2RD (VIEWCTR, 12);
      FIELD_3BD (view_target, 17);
      FIELD_3BD (VIEWDIR, 16);
      FIELD_BD (view_twist, 51);
      FIELD_BD (lens_length, 42);
      FIELD_BD (front_clip_z, 43);
      FIELD_BD (back_clip_z, 44);
      // UCSFOLLOW is bit 3 of 71, UCSVP bit 0, ucs_at_origin bit 1. below decoded again.
      FIELD_4BITS (VIEWMODE, 71);
      SINCE (R_2000) {
        FIELD_RC (render_mode, 281);
      }
      SINCE (R_2007) {
        IF_ENCODE_FROM_EARLIER {
          FIELD_VALUE (use_default_lights) = 1;
          FIELD_VALUE (default_lightning_type) = 1;
          FIELD_VALUE (ambient_color.index) = 250;
          //TODO FIELD_VALUE (ambient_color.rgb) = ?;
          //TODO FIELD_VALUE (ambient_color.byte) = ?; //+ name, book_name
        }
        VERSIONS (R_13b1, R_2004) {
          FIELD_HANDLE (sun, 3, 361);
        }
        SINCE (R_2007) {
          FIELD_HANDLE (background, 4, 332); //soft ptr
          FIELD_HANDLE (visualstyle, 5, 348); //hard ptr
          FIELD_HANDLE (sun, 3, 361); //hard owner
        }
        FIELD_B (use_default_lights, 292);
        FIELD_RC (default_lightning_type, 282);
        FIELD_BD (brightness, 141);
        FIELD_BD (contrast, 142);
        FIELD_CMC (ambient_color, 63); // +421, 431
      }
    
      FIELD_2RD (lower_left, 10);
      FIELD_2RD (upper_right, 11);
      FIELD_B (UCSFOLLOW, 0); // bit 3 of 71
      FIELD_BS (circle_zoom, 72);
      FIELD_B (FASTZOOM, 73);
      FIELD_BB (UCSICON, 74); // bits 0 and 1 of 71: uscicon_on, ucsicon_at_origin
      FIELD_B (GRIDMODE, 76);
      FIELD_2RD (GRIDUNIT, 15);
      FIELD_B (SNAPMODE, 75);
      FIELD_B (SNAPSTYLE, 77);
      FIELD_BS (SNAPISOPAIR, 78);
      if (dwg->header.dwg_version != 0x1a) { // AC1020/R_2006 only here
        FIELD_BD (SNAPANG, 50);
        FIELD_2RD (SNAPBASE, 13);
      }
      FIELD_2RD (SNAPUNIT, 14);
    
      SINCE (R_2000) {
        FIELD_B (ucs_at_origin, 0);
        FIELD_B (UCSVP, 71);
        FIELD_3BD (ucsorg, 110);
        FIELD_3BD (ucsxdir, 111);
        FIELD_3BD (ucsydir, 112);
        FIELD_BD (ucs_elevation, 146);
        FIELD_BS (UCSORTHOVIEW, 79);
      }
    
      SINCE (R_2007) {
        FIELD_BS (grid_flags, 60);
        FIELD_BS (grid_major, 61);
      }
    }

    SINCE (R_13b1) {
      START_OBJECT_HANDLE_STREAM;
    }
    SINCE (R_2000) {
      FIELD_HANDLE0 (named_ucs, 5, 345);
      FIELD_HANDLE0 (base_ucs, 5, 346);
    }
  } // DWG

DWG_OBJECT_END

/* (66) */
DWG_OBJECT (APPID_CONTROL)

  PRE (R_13b1) {
    //VALUE_RS (obj->size, 0);
    FIELD_RS (num_entries, 70);
    FIELD_RSx (flags_r11, 0);
    JSON { KEY (address); }
    VALUE_RLx (obj->address & 0xFFFFFFFF, 0);
  } LATER_VERSIONS {
    FIELD_BS (num_entries, 70); //BS or BL?
  }
  CONTROL_HANDLE_STREAM;
  HANDLE_VECTOR (entries, num_entries, 2, 0);

DWG_OBJECT_END

/* (67/T8) Registered Apps */
DWG_TABLE (APPID)

  COMMON_TABLE_FLAGS (RegApp)
  SINCE (R_13b1) {
    DXF {
      FIELD_RS0 (unknown, 71); // in DXF only with ADE_PROJECTION
    } else {
      FIELD_RC (unknown, 71);
    }
    START_OBJECT_HANDLE_STREAM;
  }
DWG_OBJECT_END

/* (68) */
DWG_OBJECT (DIMSTYLE_CONTROL)

  PRE (R_13b1) {
    //VALUE_RS (obj->size, 0);
    FIELD_RS (num_entries, 70);
    FIELD_RSx (flags_r11, 0);
    JSON { KEY (address); }
    VALUE_RLx (obj->address & 0xFFFFFFFF, 0);
  } LATER_VERSIONS {
    FIELD_BS (num_entries, 70); //BS or BL?
  }
  SINCE (R_2000) {
    SUBCLASS (AcDbDimStyleTable)
    /* number of additional hard handles, undocumented */
    FIELD_RCu (num_morehandles, 71);
  }
  CONTROL_HANDLE_STREAM;
  HANDLE_VECTOR (entries, num_entries, 2, 0);
  SINCE (R_13b1) {
    HANDLE_VECTOR (morehandles, num_morehandles, 5, 340);
  }

DWG_OBJECT_END

/* (69/T9) */
DWG_TABLE (DIMSTYLE)

  DXF {
    FIELD_VALUE (flag) = FIELD_VALUE (flag0);
  }
  COMMON_TABLE_FLAGS (DimStyle)

  PRE (R_13b1)
    {
      FIELD_RD (DIMSCALE, 40); // d42
      FIELD_RD (DIMASZ, 41);
      FIELD_RD (DIMEXO, 42);
      FIELD_RD (DIMDLI, 43);
      FIELD_RD (DIMEXE, 44);
      FIELD_RD (DIMRND, 45);
      FIELD_RD (DIMDLE, 46);
      FIELD_RD (DIMTP, 47);
      FIELD_RD (DIMTM, 48); // ok
      FIELD_RD (DIMTXT, 140);
      FIELD_RD (DIMCEN, 141); // ok
      FIELD_RD (DIMTSZ, 142);
      FIELD_RD (DIMALTF, 143);
      FIELD_RD (DIMLFAC, 144);
      FIELD_RD (DIMTVP, 145); // db2
      FIELD_RC (DIMTOL, 71);
      FIELD_RC (DIMLIM, 72);
      FIELD_RC (DIMTIH, 73);
      FIELD_RC (DIMTOH, 74);
      FIELD_RC (DIMSE1, 75);
      FIELD_RC (DIMSE2, 76);
      FIELD_CAST (DIMTAD, RC, RS, 77); // ok
      FIELD_CAST (DIMZIN, RC, BS, 78); // dc1
      FIELD_RC (DIMALT, 170);
      FIELD_CAST (DIMALTD, RC, BS, 171); // ok
      FIELD_RC (DIMTOFL, 172);           // ok
      FIELD_RC (DIMSAH, 173);            // ok
      FIELD_RC (DIMTIX, 174);            // ok
      FIELD_RC (DIMSOXD, 175);           // ok
      FIELD_TFv (DIMPOST, 16, 3);        // ok dc8
      FIELD_TFv (DIMAPOST, 16, 4);       // dd8
      FIELD_TFv (DIMBLK_T, 16, 5);       //?? unsupported by ODA
      FIELD_TFv (DIMBLK1_T, 16, 6);      //?? unsupported by ODA
      FIELD_TFv (DIMBLK2_T, 66, 7);      //?? unsupported by ODA
      // DEBUG_HERE; //e18
      // dat->byte += 50; //unknown: DIMSHO, DIMASO (global)
      FIELD_RS (DIMCLRD_N, 176); // e4a
      FIELD_RS (DIMCLRE_N, 177);
      FIELD_RS (DIMCLRT_N, 178); // e4e
      FIELD_RC (DIMUPT, 0);      //??
      FIELD_RD (DIMTFAC, 146);   // e51
      FIELD_RD (DIMGAP, 147);    // e59
    }
  VERSIONS (R_13b1, R_14)
    {
      DXF {
        FIELD_TV (DIMPOST, 3);
        FIELD_TV (DIMAPOST, 4);
        FIELD_TV (DIMBLK_T, 5);
        FIELD_TV (DIMBLK1_T, 6);
        FIELD_TV (DIMBLK2_T, 7);
        FIELD_BD (DIMSCALE, 40);
        FIELD_BD (DIMASZ, 41);
        FIELD_BD (DIMEXO, 42);
        FIELD_BD (DIMDLI, 43);
        FIELD_BD (DIMEXE, 44);
        FIELD_BD (DIMRND, 45);
        FIELD_BD (DIMDLE, 46);
        FIELD_BD (DIMTP, 47);
        FIELD_BD (DIMTM, 48);
      }
      FIELD_B (DIMTOL, 71);
      FIELD_B (DIMLIM, 72);
      FIELD_B (DIMTIH, 73);
      FIELD_B (DIMTOH, 74);
      FIELD_B (DIMSE1, 75);
      FIELD_B (DIMSE2, 76);
      FIELD_B (DIMALT, 170);
      FIELD_B (DIMTOFL, 172);
      FIELD_B (DIMSAH, 173);
      FIELD_B (DIMTIX, 174);
      FIELD_B (DIMSOXD, 175);
      FIELD_CAST (DIMALTD, RC, BS, 171);
      FIELD_CAST (DIMZIN, RC, BS, 78);
      FIELD_B (DIMSD1, 281);
      FIELD_B (DIMSD2, 282);
      FIELD_CAST (DIMTOLJ, RC, BS, 283);
      FIELD_CAST (DIMJUST, RC, BS, 280);
      FIELD_CAST (DIMFIT, RC, BS, 287);
      FIELD_B (DIMUPT, 288);
      FIELD_CAST (DIMTZIN, RC, BS, 284);
      FIELD_CAST (DIMALTZ, RC, BS, 285);
      FIELD_CAST (DIMALTTZ, RC, BS, 286);
      FIELD_CAST (DIMTAD, RC, BS, 77);
      FIELD_BS (DIMUNIT, 270);
      FIELD_BS (DIMAUNIT, 0);
      FIELD_BS (DIMDEC, 271);
      FIELD_BS (DIMTDEC, 272);
      FIELD_BS (DIMALTU, 273);
      FIELD_BS (DIMALTTD, 274);
      DXF {
        FIELD_HANDLE (DIMTXSTY, 5, 340);
        FIELD_BS (DIMAUNIT, 275);
      }
      FIELD_BD (DIMSCALE, 40);
      FIELD_BD (DIMASZ, 0);
      FIELD_BD (DIMEXO, 0);
      FIELD_BD (DIMDLI, 0);
      FIELD_BD (DIMEXE, 0);
      FIELD_BD (DIMRND, 0);
      FIELD_BD (DIMDLE, 0);
      FIELD_BD (DIMTP, 0);
      FIELD_BD (DIMTM, 0);
      FIELD_BD (DIMTXT, 140);
      FIELD_BD (DIMCEN, 141);
      FIELD_BD (DIMTSZ, 142);
      FIELD_BD (DIMALTF, 143);
      FIELD_BD (DIMLFAC, 144);
      FIELD_BD (DIMTVP, 145);
      FIELD_BD (DIMTFAC, 146);
      FIELD_BD (DIMGAP, 147);
      FIELD_TV (DIMPOST, 0);
      FIELD_TV (DIMAPOST, 0);
      FIELD_TV (DIMBLK_T, 0);
      FIELD_TV (DIMBLK1_T, 0);
      FIELD_TV (DIMBLK2_T, 0);
      FIELD_CMC (DIMCLRD, 176);
      FIELD_CMC (DIMCLRE, 177);
      FIELD_CMC (DIMCLRT, 178);
    }
  else FREE {
      FIELD_TV (DIMBLK_T, 5);
      FIELD_TV (DIMBLK1_T, 6);
      FIELD_TV (DIMBLK2_T, 7);
  }
  IF_FREE_OR_SINCE (R_2000)
    {
      FIELD_T0 (DIMPOST, 3); // name of dimtxtsty
      FIELD_T0 (DIMAPOST, 4)
      FIELD_BD1 (DIMSCALE, 40);
      FIELD_BD0 (DIMASZ, 41);
      FIELD_BD0 (DIMEXO, 42);
      FIELD_BD0 (DIMDLI, 43);
      FIELD_BD0 (DIMEXE, 44);
      FIELD_BD0 (DIMRND, 45);
      FIELD_BD0 (DIMDLE, 46);
      FIELD_BD0 (DIMTP, 47);
      FIELD_BD0 (DIMTM, 48);
    }

  SINCE (R_2007)
    {
      FIELD_BD0 (DIMFXL, 49);
      FIELD_BD0 (DIMJOGANG, 50);
      FIELD_BS0 (DIMTFILL, 69);
      FIELD_CMC0 (DIMTFILLCLR, 70);
      DXF {
        FIELD_BS0 (DIMARCSYM, 90);
        FIELD_B0 (DIMFXLON, 290);
      }
    }

  SINCE (R_2000)
    {
      FIELD_B0 (DIMTOL, 71);
      FIELD_B0 (DIMLIM, 72);
      FIELD_B (DIMTIH, 73);
      FIELD_B (DIMTOH, 74);
      FIELD_B0 (DIMSE1, 75);
      FIELD_B0 (DIMSE2, 76);
      FIELD_BS0 (DIMTAD, 77);
      FIELD_BS0 (DIMZIN, 78);
      FIELD_BS0 (DIMAZIN, 79);
    }

  SINCE (R_2007)
    {
      FIELD_BS (DIMARCSYM, 0);
    }

  IF_FREE_OR_SINCE (R_2000)
    {
      FIELD_BD0 (DIMTXT, 140);
      FIELD_BD0 (DIMCEN, 141);
      FIELD_BD0 (DIMTSZ, 142);
      FIELD_BD0 (DIMALTF, 143);
      FIELD_BD0 (DIMLFAC, 144);
      FIELD_BD0 (DIMTVP, 145);
      FIELD_BD0 (DIMTFAC, 146);
      FIELD_BD0 (DIMGAP, 147);
      FIELD_BD0 (DIMALTRND, 148);
      FIELD_B0 (DIMALT, 170);
      FIELD_BS0 (DIMALTD, 171);
      FIELD_B0 (DIMTOFL, 172);
      FIELD_B0 (DIMSAH, 173);
      FIELD_B0 (DIMTIX, 174);
      FIELD_B0 (DIMSOXD, 175);
      FIELD_CMC0 (DIMCLRD, 176);
      FIELD_CMC0 (DIMCLRE, 177);
      FIELD_CMC0 (DIMCLRT, 178);
      FIELD_BS0 (DIMADEC, 179);
      FIELD_BS0 (DIMDEC, 271);
      FIELD_BS0 (DIMTDEC, 272);
      FIELD_BS0 (DIMALTU, 273);
      FIELD_BS0 (DIMALTTD, 274);
      FIELD_BS0 (DIMAUNIT, 275);

      FIELD_BS0 (DIMFRAC, 276);
      FIELD_BS0 (DIMLUNIT, 277);
      FIELD_BS0 (DIMDSEP, 278);
      FIELD_BS0 (DIMTMOVE, 279);

      FIELD_BS0 (DIMJUST, 280);
      FIELD_B0 (DIMSD1, 281);
      FIELD_B0 (DIMSD2, 282);
      FIELD_BS (DIMTOLJ, 283);
      FIELD_BS0 (DIMTZIN, 284);
      FIELD_BS0 (DIMALTZ, 285);
      FIELD_BS0 (DIMALTTZ, 286);
      FIELD_B0 (DIMUPT, 288);
      FIELD_BS0 (DIMATFIT, 289);
      FIELD_HANDLE (DIMTXSTY, 5, 340);
    }

  SINCE (R_2007)
    {
      FIELD_B (DIMFXLON, 0);
    }

  IF_FREE_OR_SINCE (R_2010)
    {
      FIELD_B (DIMTXTDIRECTION, 295);
      FIELD_BD (DIMALTMZF, 0); // undocumented
      FIELD_T (DIMALTMZS, 0); // undocumented
      FIELD_BD (DIMMZF, 0); // undocumented
      FIELD_T (DIMMZS, 0); // undocumented
    }

  SINCE (R_2000)
    {
      FIELD_BSd (DIMLWD, 371);
      FIELD_BSd (DIMLWE, 372);
    }

  SINCE (R_13b1)
  {
    FIELD_B (flag0, 0); // Bit 0 of 70
    FIELD_VALUE (flag) |= FIELD_VALUE (flag0); //dimtofl?

    START_OBJECT_HANDLE_STREAM;
    UNTIL (R_14) {
      FIELD_HANDLE (DIMTXSTY, 5, 0); // 2000+ already before
    }
  }
  IF_FREE_OR_SINCE (R_2000)
    {
      FIELD_HANDLE (DIMLDRBLK, 5, 341); /* Leader arrow (DIMLDRBLK)*/
      FIELD_HANDLE (DIMBLK, 5, 342);  /* Arrow */
      FIELD_HANDLE (DIMBLK1, 5, 343); /* Arrow 1 */
      FIELD_HANDLE (DIMBLK2, 5, 344); /* Arrow 2 */
    }
  IF_FREE_OR_SINCE (R_2007)
    {
      FIELD_HANDLE (DIMLTYPE, 5, 345);
      FIELD_HANDLE (DIMLTEX1, 5, 346);
      FIELD_HANDLE (DIMLTEX2, 5, 347);
    }

DWG_OBJECT_END

/* (70) */
DWG_OBJECT (VX_CONTROL)

  PRE (R_13b1) {
    //VALUE_RS (obj->size, 0);
    FIELD_RS (num_entries, 70);
    FIELD_RSx (flags_r11, 0);
    JSON { KEY (address); }
    VALUE_RLx (obj->address & 0xFFFFFFFF, 0);
  } LATER_VERSIONS {
    FIELD_BS (num_entries, 70); //BS or BL?
  }
  SUBCLASS (AcDbVXTable)
  CONTROL_HANDLE_STREAM;
  HANDLE_VECTOR (entries, num_entries, 2, 0);

DWG_OBJECT_END

/* (71/T10)
*/
DWG_TABLE (VX_TABLE_RECORD)

  COMMON_TABLE_FLAGS (VX)
  PRE (R_13b1)
    {
      FIELD_RS (vport_entity_address, 0);
      FIELD_RSd (r11_viewport_index, 0);
      FIELD_RS (r11_prev_entry_index, 0);
    }
  LATER_VERSIONS
    {
      FIELD_B (is_on, 290); // bit 1 of 70
      FIELD_VALUE (flag) |= FIELD_VALUE (is_on) << 1;

      START_OBJECT_HANDLE_STREAM;
      FIELD_HANDLE (viewport, 4, 338);
      FIELD_HANDLE (prev_entry, 5, 340);
    }

DWG_OBJECT_END

/* (72) */
DWG_OBJECT (GROUP)

  SUBCLASS (AcDbGroup)
  FIELD_T (name, 300);
  FIELD_BS (unnamed, 70);
  FIELD_BS (selectable, 71);
  FIELD_BL (num_groups, 0);
  VALUEOUTOFBOUNDS (num_groups, 10000)

  START_OBJECT_HANDLE_STREAM;
  HANDLE_VECTOR (groups, num_groups, 5, 340);

DWG_OBJECT_END

/* (73) */
DWG_OBJECT (MLINESTYLE)

  SUBCLASS (AcDbMlineStyle)
  FIELD_T (name, 2);
  FIELD_T (description, 0);
  FIELD_BS (flag, 70);  /*!< 1 = Fill on,
                             2 = Display miters,
                             16 = Start square end (line) cap,
                             32 = Start inner arcs cap,
                             64 = Start round (outer arcs) cap,
                             256 = End square (line) cap,
                             512 = End inner arcs cap,
                             1024 = End round (outer arcs) cap */
  DXF { FIELD_T (description, 3); }
  FIELD_CMC (fill_color, 62); /*!< default 256 */
#ifdef IS_DXF
  {
    // 0 - 91.0
    BITCODE_BD start_angle = rad2deg (FIELD_VALUE (start_angle));
    BITCODE_BD end_angle   = rad2deg (FIELD_VALUE (end_angle));
    if (start_angle < 0.0) start_angle += 360.0;
    if (end_angle < 0.0)   end_angle   += 360.0;
    while (start_angle > 91.0) start_angle -= 90.0;
    while (end_angle   > 91.0) end_angle   -= 90.0;
    VALUE_RD (start_angle, 51);
    VALUE_RD (end_angle, 52);
  }
#else
  FIELD_BD (start_angle, 51); /*!< default 90 deg */
  FIELD_BD (end_angle, 52);   /*!< default 90 deg */
#endif
  FIELD_RCu (num_lines, 71);
  REPEAT (num_lines, lines, Dwg_MLINESTYLE_line)
  REPEAT_BLOCK
      SUB_FIELD_BD (lines[rcount1], offset, 49);
      SUB_FIELD_CMC (lines[rcount1], color, 62); /*!< default: 0 */
      PRE (R_2018)
      {
#if defined (IS_DXF) && !defined (IS_ENCODER)
        switch (FIELD_VALUE (lines[rcount1].lt_index)) {
        case 32767: VALUE_TFF ("BYLAYER", 6); break; /* default (SHRT_MAX) */
        case 32766: VALUE_TFF ("BYBLOCK", 6); break;
        case 0:  VALUE_TFF ("CONTINUOUS", 6); break;
        //TODO else lookup name on LTYPE_CONTROL list
        default: /*FIELD_HANDLE_NAME (lt.ltype, 5, 6);*/
                 VALUE_TFF ("", 6); break;
        }
#else
        SUB_FIELD_BSd (lines[rcount1], lt_index, 6);
#endif
      }
      LATER_VERSIONS {
        SUB_FIELD_HANDLE (lines[rcount1], lt_ltype, 5, 6);
      }
      SET_PARENT_OBJ (lines[rcount1]);
  END_REPEAT_BLOCK
  END_REPEAT (lines);

  START_OBJECT_HANDLE_STREAM;

DWG_OBJECT_END

//pg.135
DWG_OBJECT (DICTIONARYVAR)

  SUBCLASS (DictionaryVariables)
  FIELD_RCd (schema, 280);
  FIELD_T (strvalue, 1);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

#ifndef IS_INDXF
int DWG_FUNC_N (ACTION,_HATCH_gradientfill)(
                        Bit_Chain *restrict dat,
                        Bit_Chain *restrict str_dat,
                        const Dwg_Object *restrict obj,
                        Dwg_Entity_HATCH *restrict _obj);

int DWG_FUNC_N (ACTION,_HATCH_gradientfill)(
                        Bit_Chain *restrict dat,
                        Bit_Chain *restrict str_dat,
                        const Dwg_Object *restrict obj,
                        Dwg_Entity_HATCH *restrict _obj)
{
  BITCODE_BL vcount, rcount3, rcount4;
  int error = 0;
  Dwg_Data* dwg = obj->parent;

  FIELD_BL (is_gradient_fill, 450);
  FIELD_BL (reserved, 451);
  FIELD_BD (gradient_angle, 460);
  FIELD_BD (gradient_shift, 461);
  FIELD_BL (single_color_gradient, 452); //bool
  FIELD_BD (gradient_tint, 462);
  FIELD_BL (num_colors, 453); //default: 2
  if (FIELD_VALUE (is_gradient_fill) != 0 && FIELD_VALUE (num_colors) > 1000)
    {
      LOG_ERROR ("Invalid gradient fill HATCH.num_colors " FORMAT_BL,
                _obj->num_colors);
      _obj->num_colors = 0;
      return DWG_ERR_VALUEOUTOFBOUNDS;
    }
  REPEAT (num_colors, colors, Dwg_HATCH_Color)
  REPEAT_BLOCK
      SUB_FIELD_BD (colors[rcount1], shift_value, 463);
      SUB_FIELD_CMC (colors[rcount1], color, 63);
      SET_PARENT_OBJ (colors[rcount1]);
  END_REPEAT_BLOCK
  END_REPEAT (colors);
  FIELD_T (gradient_name, 470);
  return error;
}
#endif

#ifdef IS_JSON
#  define JSON_END_REPEAT(f)  ENDHASH; ENDARRAY;
#else
#  define JSON_END_REPEAT(f)  END_REPEAT (f)
#endif

//(78 + varies) pg.136
DWG_ENTITY (HATCH)

  SUBCLASS (AcDbHatch)
#if !defined (IS_DXF) && !defined (IS_INDXF)
  SINCE (R_2004)
    {
      error |= DWG_FUNC_N (ACTION,_HATCH_gradientfill)(dat,str_dat,obj,_obj);
    }
#endif
  DXF {
    BITCODE_3RD pt = { 0.0, 0.0, 0.0 };
    pt.z = FIELD_VALUE (elevation);
    KEY (elevation); VALUE_3BD (pt, 10);
  } else {
    FIELD_BD (elevation, 30);
  }
  ENCODER { normalize_BE (FIELD_VALUE (extrusion)); }
  FIELD_3BD (extrusion, 210);
  DECODER { normalize_BE (FIELD_VALUE (extrusion)); }
  DXF {
    if (FIELD_VALUE (is_solid_fill)) {
      VALUE_TFF ("SOLID", 2); // not "SOLID,_I"
    } else {
      FIELD_T (name, 2);
    }
  } else {
    FIELD_T (name, 2); //default: SOLID
   }
  FIELD_B (is_solid_fill, 70); //default: 1, pattern_fill: 0
  FIELD_B (is_associative, 71);
  FIELD_BL (num_paths, 91);
  VALUEOUTOFBOUNDS (num_paths, 10000)
  REPEAT (num_paths, paths, Dwg_HATCH_Path)
  REPEAT_BLOCK
      SUB_FIELD_BL (paths[rcount1], flag, 92);
      if (!(FIELD_VALUE (paths[rcount1].flag) & 2))
        {
          SUB_FIELD_BL (paths[rcount1], num_segs_or_paths, 93);
          if (FIELD_VALUE (paths[rcount1].num_segs_or_paths > 10000))
            {
              LOG_ERROR ("Invalid HATCH.num_segs_or_paths " FORMAT_BL,
                        _obj->paths[rcount1].num_segs_or_paths);
              FIELD_VALUE (paths[rcount1].num_segs_or_paths) = 0;
              JSON_END_REPEAT (paths);
              return DWG_ERR_VALUEOUTOFBOUNDS;
            }
#define segx paths[rcount1].segs
          REPEAT2 (paths[rcount1].num_segs_or_paths, segx, Dwg_HATCH_PathSeg)
          REPEAT_BLOCK
              SUB_FIELD_RC (segx[rcount2],curve_type, 72); // 1-4
              switch (FIELD_VALUE (segx[rcount2].curve_type))
                {
                    case 1: /* LINE */
                      SUB_FIELD_2RD (segx[rcount2],first_endpoint, 10);
                      SUB_FIELD_2RD (segx[rcount2],second_endpoint, 11);
                      break;
                    case 2: /* CIRCULAR ARC */
                      SUB_FIELD_2RD (segx[rcount2], center, 10);
                      SUB_FIELD_BD (segx[rcount2], radius, 40);
                      SUB_FIELD_BD (segx[rcount2], start_angle, 50);
                      SUB_FIELD_BD (segx[rcount2], end_angle, 51);
                      SUB_FIELD_B (segx[rcount2], is_ccw, 73);
                      break;
                    case 3: /* ELLIPTICAL ARC */
                      SUB_FIELD_2RD (segx[rcount2], center, 10);
                      SUB_FIELD_2RD (segx[rcount2], endpoint, 11);
                      SUB_FIELD_BD (segx[rcount2], minor_major_ratio, 40);
                      SUB_FIELD_BD (segx[rcount2], start_angle, 50);
                      SUB_FIELD_BD (segx[rcount2], end_angle, 51);
                      SUB_FIELD_B (segx[rcount2], is_ccw, 73);
                      break;
                    case 4: /* SPLINE */
                      SUB_FIELD_BL (segx[rcount2], degree, 94);
                      SUB_FIELD_B (segx[rcount2], is_rational, 73);
                      SUB_FIELD_B (segx[rcount2], is_periodic, 74);
                      SUB_FIELD_BL (segx[rcount2], num_knots, 95);
                      SUB_FIELD_BL (segx[rcount2], num_control_points, 96);
                      if (FIELD_VALUE (segx[rcount2].num_knots > 10000))
                        {
                          LOG_ERROR ("Invalid HATCH.paths[" FORMAT_BL
                                     "].segs[" FORMAT_BL
                                     "].num_knots " FORMAT_BL,
                                     rcount1, rcount2,
                                     FIELD_VALUE (segx[rcount2].num_knots));
                          FIELD_VALUE (segx[rcount2].num_knots) = 0;
                          JSON_END_REPEAT (segx);
                          JSON_END_REPEAT (paths);
                          return DWG_ERR_VALUEOUTOFBOUNDS;
                        }
                      FIELD_VECTOR (segx[rcount2].knots, BD,
                                    segx[rcount2].num_knots, 40);
                      if (FIELD_VALUE (segx[rcount2].num_control_points > 10000))
                        {
                          LOG_ERROR (
                              "Invalid HATCH.paths[" FORMAT_BL
                              "].segs[" FORMAT_BL
                              "].num_control_points " FORMAT_BL,
                              rcount1, rcount2,
                              FIELD_VALUE (segx[rcount2].num_control_points));
                          FIELD_VALUE (segx[rcount2].num_control_points) = 0;
                          JSON_END_REPEAT (segx);
                          JSON_END_REPEAT (paths);
                          return DWG_ERR_VALUEOUTOFBOUNDS;
                        }
#define control_points segx[rcount2].control_points
                      REPEAT3 (segx[rcount2].num_control_points, control_points, Dwg_HATCH_ControlPoint)
                      REPEAT_BLOCK
                          SUB_FIELD_2RD (control_points[rcount3], point, 10);
                          if (FIELD_VALUE (segx[rcount2].is_rational))
                            SUB_FIELD_BD (control_points[rcount3], weight, 40)
                          SET_PARENT (control_points[rcount3], &FIELD_VALUE (segx[rcount2]));
                      END_REPEAT_BLOCK
                      END_REPEAT (control_points);
#undef control_points
                      SINCE (R_2013) // r2014 really
                        {
#define seg segx[rcount2]
                          SUB_FIELD_BL (seg, num_fitpts, 97);
                          FIELD_2RD_VECTOR (seg.fitpts, seg.num_fitpts, 11);
                        }
                      break;
                    default:
                      LOG_ERROR ("Invalid HATCH.curve_type %d\n",
                                FIELD_VALUE (seg.curve_type));
                      DEBUG_HERE_OBJ
                      FIELD_VALUE (seg.curve_type) = 0;
                      JSON_END_REPEAT (segx);
                      JSON_END_REPEAT (paths);
                      return DWG_ERR_VALUEOUTOFBOUNDS;
                }
              SET_PARENT (seg, &_obj->paths[rcount1]);
#undef seg
          END_REPEAT_BLOCK
          END_REPEAT (segx);
#undef segx
        }
      else
        { /* POLYLINE PATH */
          SUB_FIELD_B (paths[rcount1],bulges_present, 72);
          SUB_FIELD_B (paths[rcount1],closed, 73);
          SUB_FIELD_BL (paths[rcount1],num_segs_or_paths, 93);
#define pline_paths paths[rcount1].polyline_paths
          REPEAT2 (paths[rcount1].num_segs_or_paths, pline_paths, Dwg_HATCH_PolylinePath)
          REPEAT_BLOCK
              SUB_FIELD_2RD (pline_paths[rcount2],point, 10);
              if (FIELD_VALUE (paths[rcount1].bulges_present))
                {
                  SUB_FIELD_BD (pline_paths[rcount2],bulge, 42);
                }
              SET_PARENT (pline_paths[rcount2], &_obj->paths[rcount1]);
          END_REPEAT_BLOCK
          END_REPEAT (pline_paths);
#undef pline_paths
        }
      SUB_FIELD_BL (paths[rcount1],num_boundary_handles, 97);
      DECODER {
        FIELD_VALUE (has_derived) =
          FIELD_VALUE (has_derived) || (FIELD_VALUE (paths[rcount1].flag) & 0x4);
      }
      VALUEOUTOFBOUNDS (paths[rcount1].num_boundary_handles, 10000)
      SUB_HANDLE_VECTOR (paths[rcount1], boundary_handles, num_boundary_handles, 4, 330);
      SET_PARENT_OBJ (paths[rcount1]);
  END_REPEAT_BLOCK
  END_REPEAT (paths);

  FIELD_BS (style, 75);        // 0=normal (odd parity); 1=outer; 2=whole
  FIELD_BS (pattern_type, 76); // 0=user; 1=predefined; 2=custom
  if (!FIELD_VALUE (is_solid_fill))
    {
      FIELD_BD (angle, 52);
      FIELD_BD (scale_spacing, 41);
      FIELD_B (double_flag, 77);
      
      FIELD_BS (num_deflines, 78);
      REPEAT (num_deflines, deflines, Dwg_HATCH_DefLine)
      REPEAT_BLOCK
          SUB_FIELD_BD (deflines[rcount1], angle, 53);
          SUB_FIELD_2BD_1 (deflines[rcount1], pt0, 43);
          SUB_FIELD_2BD_1 (deflines[rcount1], offset, 45);
          SUB_FIELD_BS (deflines[rcount1], num_dashes, 79);
          SUB_FIELD_VECTOR (deflines[rcount1], dashes, BD, num_dashes, 49)
          SET_PARENT_OBJ (deflines[rcount1]);
      END_REPEAT_BLOCK
      END_REPEAT (deflines);
    }

  JSON { FIELD_B (has_derived, 0); }
  if (FIELD_VALUE (has_derived))
    FIELD_BD (pixel_size, 47);
  FIELD_BL (num_seeds, 98);
  VALUEOUTOFBOUNDS (num_seeds, 10000)
  FIELD_2RD_VECTOR (seeds, num_seeds, 10);
#ifdef IS_DXF
  SINCE (R_2004)
    {
      if (_obj->is_gradient_fill)
        error |= DWG_FUNC_N (ACTION,_HATCH_gradientfill)(dat,str_dat,obj,_obj);
    }
#endif

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

#if defined (DEBUG_CLASSES) || defined (IS_FREE)

// Hatched closed polygon
// debugging
DWG_ENTITY (MPOLYGON)

  SUBCLASS (AcDbMPolygon)

  FIELD_BS (style, 75); // 0=normal (odd parity); 1=outer; 2=whole //??

#if !defined (IS_DXF) && !defined (IS_INDXF)
  SINCE (R_2004)
    {
      error |= DWG_FUNC_N (ACTION,_HATCH_gradientfill)(dat,str_dat,obj,(Dwg_Entity_HATCH *)_obj);
    }
#endif
  DXF {
    BITCODE_3RD pt = { 0.0, 0.0, 0.0 };
    pt.z = FIELD_VALUE (elevation);
    KEY (elevation); VALUE_3BD (pt, 10);
  } else {
    FIELD_BD (elevation, 30);
  }
  ENCODER { normalize_BE (FIELD_VALUE (extrusion)); }
  FIELD_3BD (extrusion, 210);
  DECODER { normalize_BE (FIELD_VALUE (extrusion)); }
  DXF {
    if (FIELD_VALUE (is_solid_fill)) {
      VALUE_TFF ("SOLID", 2); // not "SOLID,_I"
    } else {
      FIELD_T (name, 2);
    }
  } else {
    FIELD_T (name, 2); //default: SOLID
   }
  FIELD_B (is_solid_fill, 70); //default: 1, pattern_fill: 0
  FIELD_B (is_associative, 71);
  FIELD_BL (num_paths, 91);
  VALUEOUTOFBOUNDS (num_paths, 10000)
  REPEAT (num_paths, paths, Dwg_HATCH_Path)
  REPEAT_BLOCK
      SUB_FIELD_BL (paths[rcount1], flag, 92);
      if (!(FIELD_VALUE (paths[rcount1].flag) & 2))
        {
          SUB_FIELD_BL (paths[rcount1], num_segs_or_paths, 93);
          if (FIELD_VALUE (paths[rcount1].num_segs_or_paths > 10000))
            {
              LOG_ERROR ("Invalid HATCH.num_segs_or_paths " FORMAT_BL,
                        _obj->paths[rcount1].num_segs_or_paths);
              FIELD_VALUE (paths[rcount1].num_segs_or_paths) = 0;
              JSON_END_REPEAT (paths);
              return DWG_ERR_VALUEOUTOFBOUNDS;
            }
#define segx paths[rcount1].segs
          REPEAT2 (paths[rcount1].num_segs_or_paths, segx, Dwg_HATCH_PathSeg)
          REPEAT_BLOCK
              SUB_FIELD_RC (segx[rcount2],curve_type, 72); // 1-4
              switch (FIELD_VALUE (segx[rcount2].curve_type))
                {
                    case 1: /* LINE */
                      SUB_FIELD_2RD (segx[rcount2],first_endpoint, 10);
                      SUB_FIELD_2RD (segx[rcount2],second_endpoint, 11);
                      break;
                    case 2: /* CIRCULAR ARC */
                      SUB_FIELD_2RD (segx[rcount2], center, 10);
                      SUB_FIELD_BD (segx[rcount2], radius, 40);
                      SUB_FIELD_BD (segx[rcount2], start_angle, 50);
                      SUB_FIELD_BD (segx[rcount2], end_angle, 51);
                      SUB_FIELD_B (segx[rcount2], is_ccw, 73);
                      break;
                    case 3: /* ELLIPTICAL ARC */
                      SUB_FIELD_2RD (segx[rcount2], center, 10);
                      SUB_FIELD_2RD (segx[rcount2], endpoint, 11);
                      SUB_FIELD_BD (segx[rcount2], minor_major_ratio, 40);
                      SUB_FIELD_BD (segx[rcount2], start_angle, 50);
                      SUB_FIELD_BD (segx[rcount2], end_angle, 51);
                      SUB_FIELD_B (segx[rcount2], is_ccw, 73);
                      break;
                    case 4: /* SPLINE */
                      SUB_FIELD_BL (segx[rcount2], degree, 94);
                      SUB_FIELD_B (segx[rcount2], is_rational, 73);
                      SUB_FIELD_B (segx[rcount2], is_periodic, 74);
                      SUB_FIELD_BL (segx[rcount2], num_knots, 95);
                      SUB_FIELD_BL (segx[rcount2], num_control_points, 96);
                      if (FIELD_VALUE (segx[rcount2].num_knots) > 10000)
                        {
                          LOG_ERROR ("Invalid HATCH.paths[" FORMAT_BL
                                     "].segs[" FORMAT_BL
                                     "].num_knots " FORMAT_BL,
                                     rcount1, rcount2,
                                     FIELD_VALUE (segx[rcount2].num_knots));
                          FIELD_VALUE (segx[rcount2].num_knots) = 0;
                          JSON_END_REPEAT (segx);
                          JSON_END_REPEAT (paths);
                          return DWG_ERR_VALUEOUTOFBOUNDS;
                        }
                      FIELD_VECTOR (segx[rcount2].knots, BD,
                                    segx[rcount2].num_knots, 40);
                      if (FIELD_VALUE (segx[rcount2].num_control_points) > 10000)
                        {
                          LOG_ERROR (
                              "Invalid HATCH.paths[" FORMAT_BL
                              "].segs[" FORMAT_BL
                              "].num_control_points " FORMAT_BL,
                              rcount1, rcount2,
                              FIELD_VALUE (segx[rcount2].num_control_points));
                          FIELD_VALUE (segx[rcount2].num_control_points) = 0;
                          JSON_END_REPEAT (segx);
                          JSON_END_REPEAT (paths);
                          return DWG_ERR_VALUEOUTOFBOUNDS;
                        }
#define control_points segx[rcount2].control_points
                      REPEAT3 (segx[rcount2].num_control_points, control_points, Dwg_HATCH_ControlPoint)
                      REPEAT_BLOCK
                          SUB_FIELD_2RD (control_points[rcount3], point, 10);
                          if (FIELD_VALUE (segx[rcount2].is_rational))
                            SUB_FIELD_BD (control_points[rcount3], weight, 40)
                          SET_PARENT (control_points[rcount3], &FIELD_VALUE (segx[rcount2]));
                      END_REPEAT_BLOCK
                      END_REPEAT (control_points);
#undef control_points
                      SINCE (R_2013) // r2014 really
                        {
#define seg segx[rcount2]
                          SUB_FIELD_BL (seg, num_fitpts, 97);
                          FIELD_2RD_VECTOR (seg.fitpts, seg.num_fitpts, 11);
                        }
                      break;
                    default:
                      LOG_ERROR ("Invalid HATCH.curve_type %d\n",
                                FIELD_VALUE (segx[rcount2].curve_type));
                      DEBUG_HERE_OBJ
                      FIELD_VALUE (seg.curve_type) = 0;
                      JSON_END_REPEAT (segx);
                      JSON_END_REPEAT (paths);
                      return DWG_ERR_VALUEOUTOFBOUNDS;
                }
              SET_PARENT (seg, &_obj->paths[rcount1]);
#undef seg
          END_REPEAT_BLOCK
          END_REPEAT (segx);
#undef segx
        }
      else
        { /* POLYLINE PATH */
          SUB_FIELD_B (paths[rcount1],bulges_present, 72);
          SUB_FIELD_B (paths[rcount1],closed, 73);
          SUB_FIELD_BL (paths[rcount1],num_segs_or_paths, 93);
#define pline_paths paths[rcount1].polyline_paths
          REPEAT2 (paths[rcount1].num_segs_or_paths, pline_paths, Dwg_HATCH_PolylinePath)
          REPEAT_BLOCK
              SUB_FIELD_2RD (pline_paths[rcount2],point, 10);
              if (FIELD_VALUE (paths[rcount1].bulges_present))
                {
                  SUB_FIELD_BD (pline_paths[rcount2],bulge, 42);
                }
              SET_PARENT (pline_paths[rcount2], &_obj->paths[rcount1]);
          END_REPEAT_BLOCK
          END_REPEAT (pline_paths);
#undef pline_paths
        }
      SUB_FIELD_BL (paths[rcount1],num_boundary_handles, 97);
      //DECODER {
      //  FIELD_VALUE (has_derived) =
      //        FIELD_VALUE (has_derived) || (FIELD_VALUE (paths[rcount1].flag) & 0x4);
      //}
      VALUEOUTOFBOUNDS (paths[rcount1].num_boundary_handles, 10000)
      SUB_HANDLE_VECTOR (paths[rcount1], boundary_handles, num_boundary_handles, 4, 330);
      SET_PARENT (paths[rcount1], (Dwg_Entity_HATCH *)_obj);
  END_REPEAT_BLOCK
  END_REPEAT (paths);

  FIELD_BS (style, 75);        // 0=normal (odd parity); 1=outer; 2=whole
  FIELD_BS (pattern_type, 76); // 0=user; 1=predefined; 2=custom
  if (!FIELD_VALUE (is_solid_fill))
    {
      FIELD_BD (angle, 52);
      FIELD_BD1 (scale_spacing, 41); //default 1.0
      FIELD_B (double_flag, 77);
      
      FIELD_BS (num_deflines, 78);
      REPEAT (num_deflines, deflines, Dwg_HATCH_DefLine)
      REPEAT_BLOCK
          SUB_FIELD_BD (deflines[rcount1], angle, 53);
          SUB_FIELD_2BD_1 (deflines[rcount1], pt0, 43);
          SUB_FIELD_2BD_1 (deflines[rcount1], offset, 45);
          SUB_FIELD_BS (deflines[rcount1], num_dashes, 79);
          SUB_FIELD_VECTOR (deflines[rcount1], dashes, BD, num_dashes, 49)
          SET_PARENT (deflines[rcount1], (Dwg_Entity_HATCH *)_obj);
      END_REPEAT_BLOCK
      END_REPEAT (deflines);
    }

  //if (FIELD_VALUE (has_derived))
  //  FIELD_BD (pixel_size, 47);
  //FIELD_BL (num_seeds, 98);
  //VALUEOUTOFBOUNDS (num_seeds, 10000)
  //FIELD_2RD_VECTOR (seeds, num_seeds, 10);
#ifdef IS_DXF
  SINCE (R_2004)
    {
      if (_obj->is_gradient_fill)
        error |= DWG_FUNC_N (ACTION,_HATCH_gradientfill)(dat,str_dat,obj,(Dwg_Entity_HATCH *)_obj);
    }
#endif

  FIELD_CMC (hatch_color, 62);
  FIELD_2RD (x_dir, 11);
  FIELD_BL (num_boundary_handles, 99);

  COMMON_ENTITY_HANDLE_DATA;
DWG_ENTITY_END

#endif

//pg.139
DWG_OBJECT (IDBUFFER)

  SUBCLASS (AcDbIdBuffer)
  FIELD_RC (unknown, 0);
  FIELD_BL (num_obj_ids, 0);
  VALUEOUTOFBOUNDS (num_obj_ids, 10000)

  START_OBJECT_HANDLE_STREAM;
  HANDLE_VECTOR (obj_ids, num_obj_ids, 4, 330);

DWG_OBJECT_END

//pg.204 20.4.80
DWG_ENTITY (IMAGE)

  SUBCLASS (AcDbRasterImage)
  FIELD_BL (class_version, 90);
  if (FIELD_VALUE (class_version) > 10)
    return DWG_ERR_VALUEOUTOFBOUNDS;
  FIELD_3DPOINT (pt0, 10);
  FIELD_3DPOINT (uvec, 11);
  FIELD_3DPOINT (vvec, 12);
  FIELD_2RD (size, 13);
  FIELD_HANDLE (imagedef, 5, 340); // hard pointer
  FIELD_BS (display_props, 70);
  FIELD_B (clipping, 280);
  FIELD_RC (brightness, 281);
  FIELD_RC (contrast, 282);
  FIELD_RC (fade, 283);
  FIELD_HANDLE (imagedefreactor, 3, 360); // hard owner
  SINCE (R_2010) {
    FIELD_B (clip_mode, 0);
  }
  FIELD_BS (clip_boundary_type, 71); // 1 rect, 2 polygon
  if (FIELD_VALUE (clip_boundary_type) == 1)
    FIELD_VALUE (num_clip_verts) = 2;
  else
    FIELD_BL (num_clip_verts, 91);
  FIELD_2RD_VECTOR (clip_verts, num_clip_verts, 14);

  DXF { SINCE (R_2010) {
    FIELD_B (clip_mode, 290);
  }}
  COMMON_ENTITY_HANDLE_DATA;
DWG_ENTITY_END

//pg.142 test-data/*/Leader_*.dwg
DWG_OBJECT (IMAGEDEF)
  SUBCLASS (AcDbRasterImageDef)
  FIELD_BL (class_version, 90);
  if (FIELD_VALUE (class_version) > 10)
    return DWG_ERR_VALUEOUTOFBOUNDS;
  DXF {
    FIELD_T (file_path, 1);
    FIELD_2RD (image_size, 10);
    FIELD_2RD (pixel_size, 11);
    FIELD_B (is_loaded, 280);
    FIELD_RC (resunits, 281);
  } else {
    FIELD_2RD (image_size, 10);
    FIELD_T (file_path, 1);
    FIELD_B (is_loaded, 280);
    FIELD_RC (resunits, 281);
    FIELD_2RD (pixel_size, 11);
  }
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

//PG.143
DWG_OBJECT (IMAGEDEF_REACTOR)
  SUBCLASS (AcDbRasterImageDefReactor)
  FIELD_BL (class_version, 90);
  if (FIELD_VALUE (class_version) > 10)
    return DWG_ERR_VALUEOUTOFBOUNDS;

  START_OBJECT_HANDLE_STREAM;
  DXF { VALUE_HANDLE (obj->tio.object->ownerhandle, ownerhandle, 3, 330); }
DWG_OBJECT_END

// AutoCAD 2000
DWG_OBJECT (INDEX)
  SUBCLASS (AcDbIndex)
  FIELD_TIMEBLL (last_updated, 40);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

//pg.144
DWG_OBJECT (LAYER_INDEX)
  SUBCLASS (AcDbIndex)
  FIELD_TIMEBLL (last_updated, 40);
  SUBCLASS (AcDbLayerIndex)
  FIELD_BL (num_entries, 0);
  VALUEOUTOFBOUNDS (num_entries, 20000)
  DXF { VALUE_BL (0, 90); }
  REPEAT (num_entries, entries, Dwg_LAYER_entry)
  REPEAT_BLOCK
      SUB_FIELD_BL (entries[rcount1], numlayers, 0);
      SUB_FIELD_T (entries[rcount1], name, 8);
      SUB_FIELD_HANDLE (entries[rcount1], handle, 5, 360);
      DXF { SUB_FIELD_BL (entries[rcount1], numlayers, 90); }
      SET_PARENT_OBJ (entries[rcount1]);
  END_REPEAT_BLOCK
  END_REPEAT (entries)
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// (varies)
DWG_OBJECT (PLOTSETTINGS)
  // See also LAYOUT
  SUBCLASS (AcDbPlotSettings)
  FIELD_T (printer_cfg_file, 1);
  FIELD_T (paper_size, 2);
  DXF {
    FIELD_T (canonical_media_name, 4);
    FIELD_T (plotview_name, 6);
  }
  FIELD_BSx (plot_flags, 0);
  FIELD_BD (left_margin, 40);
  FIELD_BD (bottom_margin, 41);
  FIELD_BD (right_margin, 42);
  FIELD_BD (top_margin, 43);
  FIELD_BD (paper_width, 44);
  FIELD_BD (paper_height, 45);
  FIELD_T (canonical_media_name, 0);
  FIELD_2BD_1 (plot_origin, 46);
  FIELD_BS (plot_paper_unit, 0);
  FIELD_BS (plot_rotation_mode, 0);
  FIELD_BS (plot_type, 0);
  FIELD_2BD_1 (plot_window_ll, 48);
  FIELD_2BD_1 (plot_window_ur, 140);
  UNTIL (R_2000) {
    ENCODER {
      if (_obj->plotview && !_obj->plotview_name)
        {
          _obj->plotview_name = dwg_handle_name (dwg, "VIEW", _obj->plotview);
          LOG_TRACE ("plotsettings.plotview_name => %s\n", _obj->plotview_name);
        }
    }
    FIELD_T (plotview_name, 0);
    DECODER {
      if (!bit_empty_T (dat, _obj->plotview_name))
        {
          _obj->plotview
            = dwg_find_tablehandle (dwg, _obj->plotview_name, "VIEW");
          if (_obj->plotview)
            LOG_TRACE ("plotsettings.plotview => " FORMAT_REF "\n",
                       ARGS_REF (_obj->plotview))
        }
    }
  }
  LATER_VERSIONS {
    DECODER {
      if (!_obj->plotview && !bit_empty_T (dat, _obj->plotview_name))
        {
          _obj->plotview
            = dwg_find_tablehandle (dwg, _obj->plotview_name, "VIEW");
          if (_obj->plotview)
            LOG_TRACE ("plotsettings.plotview => " FORMAT_REF "\n",
                       ARGS_REF (_obj->plotview))
        }
    }
    DXF {
      FIELD_T (plotview_name, 0);
    } else {
      FIELD_HANDLE (plotview, 4, 0);
    }
    DECODER {
      if (!_obj->plotview_name && _obj->plotview)
        {
          _obj->plotview_name = dwg_handle_name (dwg, "VIEW", _obj->plotview);
          LOG_TRACE ("plotsettings.plotview_name => %s\n", _obj->plotview_name);
        }
    }
  }
  FREE { FIELD_TV (plotview_name, 6); FIELD_HANDLE (plotview, 5, 6); }
  FIELD_BD (paper_units, 142);
  FIELD_BD (drawing_units, 143);
  DXF {
    FIELD_BS (plot_flags, 70);
    FIELD_BS (plot_paper_unit, 72);
    FIELD_BS (plot_rotation_mode, 73);
    FIELD_BS (plot_type, 74);
  }
  FIELD_T (stylesheet, 7);
  FIELD_BS (std_scale_type, 75);
  FIELD_BD (std_scale_factor, 147);
  FIELD_2BD_1 (paper_image_origin, 148);
  SINCE (R_2004)
    {
      FIELD_BS (shadeplot_type, 76);
      FIELD_BS (shadeplot_reslevel, 77);
      FIELD_BS (shadeplot_customdpi, 78);
    }
  START_OBJECT_HANDLE_STREAM;
  SINCE (R_2007) {
    FIELD_HANDLE (shadeplot, 4, 333);
  }
DWG_OBJECT_END

//pg.145
DWG_OBJECT (LAYOUT)

  SUBCLASS (AcDbPlotSettings)
  FIELD_T (plotsettings.printer_cfg_file, 1);
  FIELD_T (plotsettings.paper_size, 2);
  DXF {
    FIELD_T (plotsettings.canonical_media_name, 4);
    FIELD_T (plotsettings.plotview_name, 6);
  }
  FIELD_BSx (plotsettings.plot_flags, 0);
  FIELD_BD (plotsettings.left_margin, 40);
  FIELD_BD (plotsettings.bottom_margin, 41);
  FIELD_BD (plotsettings.right_margin, 42);
  FIELD_BD (plotsettings.top_margin, 43);
  FIELD_BD (plotsettings.paper_width, 44);
  FIELD_BD (plotsettings.paper_height, 45);
  FIELD_T (plotsettings.canonical_media_name, 0);
  FIELD_2BD_1 (plotsettings.plot_origin, 46); // 46, 47
  FIELD_BS (plotsettings.plot_paper_unit, 0);
  FIELD_BS (plotsettings.plot_rotation_mode, 0);
  FIELD_BS (plotsettings.plot_type, 0);
  FIELD_2BD_1 (plotsettings.plot_window_ll, 48);
  FIELD_2BD_1 (plotsettings.plot_window_ur, 140);
  UNTIL (R_2000) {
    ENCODER {
      if (_obj->plotsettings.plotview && !_obj->plotsettings.plotview_name)
        {
          _obj->plotsettings.plotview_name = dwg_handle_name (dwg, "VIEW",
                                             _obj->plotsettings.plotview);
          LOG_TRACE ("plotsettings.plotview_name => %s\n", _obj->plotsettings.plotview_name);
        }
    }
    FIELD_T (plotsettings.plotview_name, 6);
    DECODER {
      if (!bit_empty_T (dat, _obj->plotsettings.plotview_name))
        {
          _obj->plotsettings.plotview = dwg_find_tablehandle (dwg,
                                      _obj->plotsettings.plotview_name, "VIEW");
          if (_obj->plotsettings.plotview)
            LOG_TRACE ("plotsettings.plotview => " FORMAT_REF "\n",
                       ARGS_REF (_obj->plotsettings.plotview))
        }
    }
  }
  LATER_VERSIONS {
    DECODER {
      if (!_obj->plotsettings.plotview && !bit_empty_T (dat, _obj->plotsettings.plotview_name))
        {
          _obj->plotsettings.plotview = dwg_find_tablehandle (dwg,
              _obj->plotsettings.plotview_name, "VIEW");
          if (_obj->plotsettings.plotview)
            LOG_TRACE ("plotsettings.plotview => " FORMAT_REF "\n",
                       ARGS_REF (_obj->plotsettings.plotview))
        }
    }
    FIELD_HANDLE (plotsettings.plotview, 4, 0);
    DECODER {
      if (!_obj->plotsettings.plotview_name && _obj->plotsettings.plotview)
        {
          _obj->plotsettings.plotview_name = dwg_handle_name (dwg, "VIEW",
                                              _obj->plotsettings.plotview);
          LOG_TRACE ("plotsettings.plotview_name => %s\n", _obj->plotsettings.plotview_name);
        }
    }
  }
  FREE { FIELD_TV (plotsettings.plotview_name, 6); FIELD_HANDLE (plotsettings.plotview, 5, 6); }
  FIELD_BD (plotsettings.paper_units, 142);
  FIELD_BD (plotsettings.drawing_units, 143);
  DXF {
    FIELD_BS (plotsettings.plot_flags, 70);
    FIELD_BS (plotsettings.plot_paper_unit, 72);
    FIELD_BS (plotsettings.plot_rotation_mode, 73);
    FIELD_BS (plotsettings.plot_type, 74);
  }
  FIELD_T (plotsettings.stylesheet, 7);
  FIELD_BS (plotsettings.std_scale_type, 75);
  FIELD_BD (plotsettings.std_scale_factor, 147);
  FIELD_2BD_1 (plotsettings.paper_image_origin, 148);
  SINCE (R_2004)
    {
      FIELD_BS (plotsettings.shadeplot_type, 76);
      FIELD_BS (plotsettings.shadeplot_reslevel, 77);
      FIELD_BS (plotsettings.shadeplot_customdpi, 78);
    }
  SINCE (R_2007) {
    FIELD_HANDLE (plotsettings.shadeplot, 4, 333);
  }

  SUBCLASS (AcDbLayout)
  FIELD_T (layout_name, 1);
  DXF {
    FIELD_BS (layout_flags, 70);
  }
  FIELD_BS (tab_order, 71);
  FIELD_BSx (layout_flags, 0);
  FIELD_3DPOINT (INSBASE, 0);
  FIELD_2RD (LIMMIN, 10);
  FIELD_2RD (LIMMAX, 11);
  DXF {
    FIELD_3DPOINT (INSBASE, 12);
    FIELD_3DPOINT (EXTMIN, 14);
    FIELD_3DPOINT (EXTMAX, 15);
    FIELD_BD (ucs_elevation, 146);
  }
  FIELD_3DPOINT (UCSORG, 13);
  FIELD_3DPOINT (UCSXDIR, 16);
  FIELD_3DPOINT (UCSYDIR, 17);
  FIELD_BD (ucs_elevation, 0);
  FIELD_BS (UCSORTHOVIEW, 76);
  FIELD_3DPOINT (EXTMIN, 0);
  FIELD_3DPOINT (EXTMAX, 0);

  SINCE (R_2004) {
    FIELD_BL (num_viewports, 0);
    VALUEOUTOFBOUNDS (num_viewports, 10000)
  }

  START_OBJECT_HANDLE_STREAM;
  FIELD_HANDLE (block_header, 4, 330); // => pspace or mspace (owner)
  FIELD_HANDLE0 (active_viewport, 4, 331);
  FIELD_HANDLE0 (base_ucs, 5, 346);
  FIELD_HANDLE0 (named_ucs, 5, 345);

  SINCE (R_2004) {
    HANDLE_VECTOR (viewports, num_viewports, 4, 0);
  }

DWG_OBJECT_END

//20.4.85 p.211
DWG_ENTITY (LWPOLYLINE)

  SUBCLASS (AcDbPolyline)
#ifdef IS_DXF
  FIELD_BL (num_points, 90);
  // 1 closed, 128 plinegen
  VALUE_BS ((FIELD_VALUE (flag) & 128) + (FIELD_VALUE (flag) & 512 ? 1 : 0), 70);
  FIELD_BD (const_width, 43);
#else
  FIELD_BS (flag, 70); // 512 closed, 128 plinegen, 4 constwidth, 8 elevation, 2 thickness
                       // 1 extrusion, 16 num_bulges, 1024 vertexidcount, 32 numwidths
  LOG_FLAG_LWPOLYLINE
#endif

  if (FIELD_VALUE (flag) & 4)
    FIELD_BD (const_width, 43);
  if (FIELD_VALUE (flag) & 8)
    FIELD_BD (elevation, 38);
  if (FIELD_VALUE (flag) & 2)
    FIELD_BD (thickness, 39);
  if (FIELD_VALUE (flag) & 1) //clashes with the dxf closed bit flag 512
    FIELD_3BD (extrusion, 210);

#ifndef IS_DXF
  FIELD_BL (num_points, 90);
  VALUEOUTOFBOUNDS (num_points, 20000)
#endif

  if (FIELD_VALUE (flag) & 16)
    FIELD_BL (num_bulges, 0);
  SINCE (R_2010) {
    if (FIELD_VALUE (flag) & 1024)
      FIELD_BL (num_vertexids, 0); //always same as num_points
  }
  if (FIELD_VALUE (flag) & 32)
    FIELD_BL (num_widths, 0);

#ifdef IS_DXF
    REPEAT (num_points, points, BITCODE_2RD)
      {
        FIELD_2RD (points[rcount1], 10);
        if (FIELD_VALUE (num_widths) && FIELD_VALUE (widths) &&
            rcount1 < FIELD_VALUE (num_widths) &&
            FIELD_VALUE (num_bulges) == FIELD_VALUE (num_points) &&
            (FIELD_VALUE (widths[rcount1].start) != 0.0 ||
             FIELD_VALUE (widths[rcount1].end) != 0.0))
          {
            FIELD_BD (widths[rcount1].start, 40);
            FIELD_BD (widths[rcount1].end, 41);
          }
        if (FIELD_VALUE (num_bulges) && FIELD_VALUE (bulges) &&
            FIELD_VALUE (num_bulges) == FIELD_VALUE (num_points))
          FIELD_BD (bulges[rcount1], 42);
        SINCE (R_2010) {
          if (FIELD_VALUE (num_vertexids) && FIELD_VALUE (vertexids) &&
              FIELD_VALUE (num_vertexids) == FIELD_VALUE (num_points))
            FIELD_BL (vertexids[rcount1], 91);
        }
      }
    END_REPEAT (points)
#else
#  ifndef IS_RELEASE
    if (FIELD_VALUE (num_points) > 20000) {
      LOG_ERROR ("Invalid LWPOLYLINE.num_points %ld", (long)FIELD_VALUE (num_points));
      _obj->num_points = 0;
      DEBUG_HERE_OBJ
      return DWG_ERR_VALUEOUTOFBOUNDS;
    }
#  endif
    VERSIONS (R_13b1, R_14) {
      FIELD_2RD_VECTOR (points, num_points, 10);
    }
    IF_FREE_OR_SINCE (R_2000) {
      FIELD_2DD_VECTOR (points, num_points, 10);
    }

    FIELD_VECTOR (bulges, BD, num_bulges, 42);
    SINCE (R_2010) {
      FIELD_VECTOR (vertexids, BL, num_vertexids, 91);
    }
    REPEAT (num_widths, widths, Dwg_LWPOLYLINE_width)
    REPEAT_BLOCK
        SUB_FIELD_BD (widths[rcount1],start, 40);
        SUB_FIELD_BD (widths[rcount1],end, 41);
    END_REPEAT_BLOCK
    END_REPEAT (widths)
#endif

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

//(74+varies) pg.149
DWG_ENTITY (OLE2FRAME)

  SUBCLASS (AcDbOle2Frame)
#ifdef IS_DXF
  // via dwg_decode_ole2() from the first 0x80 bytes in data
  FIELD_BS (oleversion, 70); //  always 2
  FIELD_TF (oleclient, strlen (_obj->oleclient), 3);
  FIELD_3BD (pt1, 10);  // upper left
  FIELD_3BD (pt2, 11);  // lower right
#endif
  FIELD_BS (type, 71); // 1: Link, 2: Embedded, 3: Static
  SINCE (R_2000) {
    FIELD_BS (mode, 72); // tile_mode, 0: mspace, 1: pspace
    DXF { FIELD_RC (lock_aspect, 73); }
  }
  ENCODER {
    if (FIELD_VALUE (data_size) && !FIELD_VALUE (data))
      FIELD_VALUE (data_size) = 0;
  }
#ifndef IS_JSON
  FIELD_BL (data_size, 90);
#endif
  FIELD_BINARY (data, FIELD_VALUE (data_size), 310);
#ifdef IS_DECODER
  dwg_decode_ole2 (_obj);
#endif
#ifdef IS_DXF
  VALUE_TFF ("OLE", 1);
#endif

  SINCE (R_2000) {
    FIELD_RC (lock_aspect, 0);
  }

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

//pg.276
#if 0 /* no proxy subtypes yet. seems to be the same as LWPOLYLINE */
DWG_SUBENT (PROXY_LWPOLYLINE)

  DECODE_UNKNOWN_BITS
  FIELD_RL (size);
  FIELD_BS (flag, 70);

  if (FIELD_VALUE (flag) & 4)
    FIELD_BD (const_width, 43);
  if (FIELD_VALUE (flag) & 8)
    FIELD_BD (elevation, 38);
  if (FIELD_VALUE (flag) & 2)
    FIELD_BD (thickness, 39);
  if (FIELD_VALUE (flag) & 1)
    FIELD_3BD (extrusion, 210);

  FIELD_BL (num_points, 90);
  VALUEOUTOFBOUNDS (num_points, 20000)

  if (FIELD_VALUE (flag) & 16)
    FIELD_BL (num_bulges, 0);
  if (FIELD_VALUE (flag) & 32)
    FIELD_BL (num_widths, 0);

  VERSIONS (R_13b1, R_14) {
    FIELD_2RD_VECTOR (points, num_points);
  }
  IF_FREE_OR_SINCE (R_2000) {
    FIELD_2DD_VECTOR (points, num_points);
  }

  FIELD_VECTOR (bulges, BD, num_bulges);
  REPEAT (num_widths, widths, Dwg_LWPOLYLINE_width)
  REPEAT_BLOCK
      SUB_FIELD_BD (widths[rcount1].start);
      SUB_FIELD_BD (widths[rcount1].end);
  END_REPEAT_BLOCK
  END_REPEAT (widths)

  FIELD_RC (unknown_1);
  FIELD_RC (unknown_2);
  FIELD_RC (unknown_3);

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END
#endif /* no proxy subtypes yet */

//(498) pg.149 r2000+
// ACAD_PROXY_ENTITY really
DWG_ENTITY (PROXY_ENTITY)

  //DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbProxyEntity)
  UNTIL (R_14) {
    FIELD_BL (class_id, 90);
  }
  LATER_VERSIONS {
    FIELD_BL (class_id, 91);
  }
  PRE (R_2018)
  {
    int dxf = dat->version <= R_14 ? 91: 95;
    FIELD_BL (version, dxf); // i.e. version << 8 + maint_version
  }
  SINCE (R_2018)
  { // if encode from earlier: maint_version = version<<16 + acad version
    FIELD_BL (version, 71);
    FIELD_BL (maint_version, 97);
  }
  SINCE (R_2000)
  {
    FIELD_B (from_dxf, 70); // Original Data Format: 0 dwg, 1 dxf
  }

  DECODER {
    unsigned char opts = dat->opts;
    _obj->data_numbits = ((dat->size * 8) - bit_position (dat)) & 0xFFFFFFFF;
    _obj->data_size = (dat->size - dat->byte) & 0xFFFFFFFF;
    if (dat->size > obj->size)
      {
            LOG_TRACE (
                "dat not restricted, dat->size %zu > obj->size " FORMAT_RL
                "\n", dat->size, obj->size);
            _obj->data_numbits
                = (((obj->address * 8) + obj->bitsize) - bit_position (dat))
                  & 0xFFFFFFFF;
            _obj->data_size = _obj->data_numbits % 8;
            if (_obj->data_numbits)
                  _obj->data_size++;
      }
    LOG_TRACE ("data_numbits: " FORMAT_BL "\n", _obj->data_numbits);
    LOG_TRACE ("data_size: " FORMAT_BL "\n", _obj->data_size);
    dat->opts &= 0xf0;
    dat->size++;
    FIELD_TF (data, _obj->data_size, 310);
    dat->size--;
    dat->opts = opts;
  }
  ENCODER {
    // write is always aligned
    if (!_obj->data_numbits)
      _obj->data_numbits = 8 * _obj->data_size;
    LOG_TRACE ("data_numbits: " FORMAT_BL "\n", _obj->data_numbits);
    LOG_TRACE ("data_size: " FORMAT_BL "\n", _obj->data_size);
  }
  JSON {
    FIELD_BL (data_numbits, 0);
  }
  DXF_OR_PRINT {
    // preview 92/310 is also proxy data
    FIELD_BL (data_size, 93);
  }
#ifndef IS_DECODER
  FIELD_BINARY (data, FIELD_VALUE (data_size), 310);
#endif
#if defined IS_DECODER || defined IS_ENCODER
  {
    int bits = _obj->data_numbits - (_obj->data_size * 8);
    if (!(bits > -8 && bits <= 0))
      LOG_ERROR ("Invalid data_numbits %u - (_obj->data_size %u * 8): %d",
                 _obj->data_numbits, _obj->data_size, bits);
    assert (bits > -8 && bits <= 0);
    if (bits < 0)
      // back off a few bits, we wrote too much
      bit_advance_position (dat, bits);
  }
#endif

  COMMON_ENTITY_HANDLE_DATA;
#ifdef IS_DECODER
  {
    size_t pos = bit_position (hdl_dat);
    unsigned char opts = dat->opts;
    dat->opts &= 0xf0;
    _obj->num_objids = 0;
    while (hdl_dat->byte < hdl_dat->size)
      {
        Dwg_Handle hdl;
        if (bit_read_H (hdl_dat, &hdl))
          break;
        else
          _obj->num_objids++;
      }
    dat->opts = opts;
    bit_set_position (hdl_dat, pos);
  }
#endif
  HANDLE_VECTOR (objids, num_objids, ANYCODE, 340); // code 3 or 4

DWG_ENTITY_END

//(499) pg.149 r2000+
// ACAD_PROXY_OBJECT really
DWG_OBJECT (PROXY_OBJECT)

  //DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbProxyObject)
  FIELD_BL (class_id, 91);
  PRE (R_2018)
  {
    FIELD_BL (version, 95);
  }
  SINCE (R_2018)
  { // if encode from earlier: maint_version = version<<16 + acad version
    FIELD_BL (version, 71);
    FIELD_BL (maint_version, 97);
  }
  SINCE (R_2000)
  {
    FIELD_B (from_dxf, 70); // Original Data Format: 0 dwg, 1 dxf
  }

  DECODER {
    unsigned char opts = dat->opts;
    _obj->data_numbits = ((dat->size * 8) - bit_position (dat)) & 0xFFFFFFFF;
    _obj->data_size = (dat->size - dat->byte) & 0xFFFFFFFF;
    if (dat->size > obj->size)
      {
        LOG_TRACE ("dat not restricted, dat->size %zu > obj->size %u\n",
                   dat->size, obj->size);
        _obj->data_numbits
          = (((obj->address * 8) + obj->bitsize) - bit_position (dat)) & 0xFFFFFFFF;
        _obj->data_size = _obj->data_numbits / 8;
        if (_obj->data_numbits % 8)
          _obj->data_size++;
      }
    else
      if (!_obj->data_size) {
        _obj->data_size = _obj->data_numbits / 8;
        if (_obj->data_numbits % 8)
          _obj->data_size++;
      }
    LOG_TRACE ("data_numbits => " FORMAT_BL "\n", _obj->data_numbits);
    LOG_TRACE ("data_size => " FORMAT_BL "\n", _obj->data_size);
    FIELD_VALUE (num_objids) = 0;
    dat->opts &= 0xf0;
    dat->size++;
    FIELD_TF (data, _obj->data_size, 310);
    dat->size--;
    dat->opts = opts;
  }
  ENCODER {
    // write is always aligned
    if (!_obj->data_numbits)
      _obj->data_numbits = 8 * _obj->data_size;
    LOG_TRACE ("data_numbits => " FORMAT_BL "\n", _obj->data_numbits);
    LOG_TRACE ("data_size => " FORMAT_BL "\n", _obj->data_size);
  }
  JSON {
    FIELD_BL (data_numbits, 0);
  }
  DXF_OR_PRINT {
    // preview 92/310 is also proxy data
    FIELD_BL (data_size, 93);
  }
#ifndef IS_DECODER
  FIELD_BINARY (data, FIELD_VALUE (data_size), 310);
#endif
#if defined IS_DECODER || defined IS_ENCODER
  {
    int bits;
    if (!_obj->data_size)
      _obj->data_size = _obj->data_numbits / 8;
    bits = _obj->data_numbits - (int)(_obj->data_size * 8);
    if (!(bits > -8 && bits <= 0))
      LOG_ERROR ("Invalid data_numbits %u - (_obj->data_size %u * 8): %d",
                 _obj->data_numbits, _obj->data_size, bits);
    assert (bits > -8 && bits <= 0);
    if (bits < 0)
      // back off a few bits, we wrote too much
      bit_advance_position (dat, bits);
  }
#endif

  START_OBJECT_HANDLE_STREAM;
#ifdef IS_DECODER
  {
    size_t pos = bit_position (hdl_dat);
    unsigned char opts = dat->opts;
    dat->opts &= 0xf0;
    _obj->num_objids = 0;
    while (hdl_dat->byte < hdl_dat->size)
      {
        Dwg_Handle hdl;
        if (bit_read_H (hdl_dat, &hdl))
          break; // error
        else
          _obj->num_objids++;
      }
    LOG_TRACE ("num_objids: " FORMAT_BL "\n", _obj->num_objids);
    dat->opts = opts;
    bit_set_position (hdl_dat, pos);
  }
#endif
  HANDLE_VECTOR (objids, num_objids, ANYCODE, 340); // code 3 or 4

DWG_OBJECT_END

// 20.4.99 Value, page 241. for FIELD and TABLE
#define TABLE_value_fields(value)                                             \
  PRE (R_2007) { FIELD_VALUE (value.data_type) &= ~0x200; }                   \
  LATER_VERSIONS { FIELD_BL (value.format_flags, 93); }                       \
  FIELD_BL (value.data_type, 90);                                             \
  if (!(dat->version >= R_2007 && FIELD_VALUE (value.format_flags) & 1))      \
    {                                                                         \
      switch (FIELD_VALUE (value.data_type))                                  \
        {                                                                     \
        case 0: /* kUnknown */                                                \
          FIELD_BL (value.data_long, 91);                                     \
          break;                                                              \
        case 1: /* kLong */                                                   \
          FIELD_BL (value.data_long, 91);                                     \
          break;                                                              \
        case 2: /* kDouble */                                                 \
          FIELD_BD (value.data_double, 140);                                  \
          break;                                                              \
        case 4:                           /* kString */                       \
          FIELD_T (value.data_string, 1); /* and 2. TODO multiple lines */    \
          break;                                                              \
        case 8: /* kDate */                                                   \
          FIELD_BL (value.data_size, 92);                                     \
          FIELD_BINARY (value.data_date, FIELD_VALUE (value.data_size), 310); \
          break;                                                              \
        case 16: /* kPoint */                                                 \
          FIELD_2RD (value.data_point, 11);                                   \
          break;                                                              \
        case 32: /* k3dPoint */                                               \
          FIELD_3RD (value.data_3dpoint, 11);                                 \
          break;                                                              \
        case 64: /* kObjectId */                                              \
          FIELD_HANDLE (value.data_handle, -1, 330);                          \
          break;                                                              \
        case 128: /* kBuffer */                                               \
          LOG_ERROR ("Unknown data type in TABLE entity: \"kBuffer\".\n")     \
          break;                                                              \
        case 256: /* kResBuf */                                               \
          LOG_ERROR ("Unknown data type in TABLE entity: \"kResBuf\".\n")     \
          break;                                                              \
        case 512: /* kGeneral since r2007*/                                   \
          SINCE (R_2007) { FIELD_BL (value.data_size, 0); }                   \
          else                                                                \
          {                                                                   \
            LOG_ERROR (                                                       \
                "Unknown data type in TABLE entity: \"kGeneral before "       \
                "R_2007\".\n")                                                \
          }                                                                   \
          break;                                                              \
        default:                                                              \
          LOG_ERROR ("Invalid data type in TABLE entity\n")                   \
          DEBUG_HERE_OBJ                                                      \
          error |= DWG_ERR_INVALIDTYPE;                                       \
          break;                                                              \
          /*return DWG_ERR_INVALIDTYPE; */                                    \
        }                                                                     \
    }                                                                         \
  SINCE (R_2007)                                                              \
  {                                                                           \
    FIELD_BL (value.unit_type, 94);                                           \
    FIELD_T (value.format_string, 300);                                       \
    FIELD_T (value.value_string, 302);                                        \
  }

DWG_OBJECT (FIELD)

  SUBCLASS (AcDbField)
  FIELD_T (id, 1);
  FIELD_T (code, 2); // and code 3 for subsequent >255 chunks
  // DXF { }
  FIELD_BL (num_childs, 90);
  VALUEOUTOFBOUNDS (num_childs, 20000)
  FIELD_BL (num_objects, 97);
  VALUEOUTOFBOUNDS (num_objects, 20000)
  PRE (R_2007) {
    FIELD_TV (format, 4);
  }
  FIELD_BL (evaluation_option, 91);
  FIELD_BL (filing_option, 92);
  FIELD_BL (field_state, 94);
  FIELD_BL (evaluation_status, 95);
  FIELD_BL (evaluation_error_code, 96);
  //DEBUG_HERE_OBJ
  FIELD_T (evaluation_error_msg, 300);
  TABLE_value_fields (value)
  if (error & DWG_ERR_INVALIDTYPE)
    return error;

  FIELD_T (value_string, 301); // and 9 for subsequent >255 chunks
  FIELD_BL (value_string_length, 98); //ODA bug TV

  FIELD_BL (num_childval, 93);
  VALUEOUTOFBOUNDS (num_childval, 20000)
  REPEAT (num_childval, childval, Dwg_FIELD_ChildValue)
  REPEAT_BLOCK
      SUB_FIELD_T (childval[rcount1],key, 6);
      TABLE_value_fields (childval[rcount1].value)
      if (error & DWG_ERR_INVALIDTYPE)
        {
          JSON_END_REPEAT (childval);
          return error;
        }
      SET_PARENT_OBJ (childval[rcount1]);
  END_REPEAT_BLOCK
  END_REPEAT (childval)

  START_OBJECT_HANDLE_STREAM;
  HANDLE_VECTOR (childs, num_childs, 3, 360);
  HANDLE_VECTOR (objects, num_objects, 5, 331);

DWG_OBJECT_END

DWG_OBJECT (FIELDLIST)

  SUBCLASS (AcDbIdSet)
  FIELD_BL (num_fields, 90);
  VALUEOUTOFBOUNDS (num_fields, 20000)
  FIELD_B (unknown, 0); // has handles?

  START_OBJECT_HANDLE_STREAM;
  HANDLE_VECTOR (fields, num_fields, 0, 330); // 2 or 4, or 3.0.0
  SUBCLASS (AcDbFieldList)

DWG_OBJECT_END

// only one v2 testcase yet, but decodes fine
DWG_OBJECT (GEODATA)

  SUBCLASS (AcDbGeoData)
  UNTIL (R_2007) // r2009, class_version 1 really
    {
      // 1 for r2009, 2 for r2010 (default), 3 for r2013 (same as r2010)
      ENCODER {
        _obj->class_version = 1;
        _obj->scale_vec.x = _obj->scale_vec.y = _obj->scale_vec.z = 1.0;
      }
      FIELD_BL (class_version, 90);
      DXF { FIELD_BS (coord_type, 70); }
      FIELD_HANDLE (host_block, 4, 330);
      FIELD_BS (coord_type, 0); // 0 unknown, 1 local grid, 2 projected grid,
                                // 3 geographic (defined by latitude/longitude) (default)
      FIELD_3BD_1 (ref_pt, 40); // wrong in ODA docs?
      FIELD_BL (units_value_horiz, 91); // 0-12, hor_units
      FIELD_3BD (design_pt, 10);
      FIELD_3BD (obs_pt, 11);    // always 0,0,0
      FIELD_3BD (up_dir, 210);
      // TODO compute if downgrading
      FIELD_BD (north_dir_angle_deg, 52);
      FIELD_3BD_1 (scale_vec, 43); // always 1,1,1

      FIELD_T (coord_system_def, 301); // & 303 if too long
      FIELD_T (geo_rss_tag, 302);
      FIELD_BD (unit_scale_horiz, 46); // hor_unit_scale
      FIELD_T (coord_system_datum, 303); //obsolete, ""
      FIELD_T (coord_system_wkt, 304);   //obsolete, ""
    }
  else // r2010+
    {
      IF_ENCODE_FROM_EARLIER {
        _obj->class_version = dat->version >= R_2013 ? 3 : 2;
      }
      FIELD_BL (class_version, 90); // TODO set by dwgversion 2 or 3
      FIELD_HANDLE (host_block, 4, 330);
      FIELD_BS (coord_type, 70); // 0 unknown, 1 local grid, 2 projected grid,
                                 // 3 geographic (defined by latitude/longitude) (default)
      FIELD_3BD (design_pt, 10);
      FIELD_3BD (ref_pt, 11);
      FIELD_BD (unit_scale_horiz, 40);
      FIELD_BL (units_value_horiz, 91); // hor_units
      FIELD_BD (unit_scale_vert, 41);   // 0xffffffff
      FIELD_BL (units_value_vert, 92);  // vert_units
      FIELD_3BD (up_dir, 210);
      // TODO compute if upgrading
      FIELD_2RD (north_dir, 12); // obsolete: 1,1,1
      // Civil3D fields:
      FIELD_BL (scale_est, 95); // None = 1 (default: ScaleEstMethodUnity),
                                // User defined = 2, Grid scale at reference point = 3,
                                // Prismodial = 4
      FIELD_BD (user_scale_factor, 141);
      FIELD_B (do_sea_level_corr, 294);
      FIELD_BD (sea_level_elev, 142);
      FIELD_BD (coord_proj_radius, 143);
      FIELD_T (coord_system_def, 301); // and 303 if longer
      FIELD_T (geo_rss_tag, 302);
    }
  if (FIELD_VALUE (class_version) > 10)
    return DWG_ERR_VALUEOUTOFBOUNDS;
  FIELD_T (observation_from_tag, 305);
  FIELD_T (observation_to_tag, 306);
  FIELD_T (observation_coverage_tag, 307);
  FIELD_BL (num_geomesh_pts, 93);
  VALUEOUTOFBOUNDS (num_geomesh_pts, 50000)
  REPEAT_N (FIELD_VALUE (num_geomesh_pts), geomesh_pts, Dwg_GEODATA_meshpt)
  REPEAT_BLOCK
      SUB_FIELD_2RD (geomesh_pts[rcount1],source_pt, 13);
      SUB_FIELD_2RD (geomesh_pts[rcount1],dest_pt, 14);
  END_REPEAT_BLOCK
  END_REPEAT (geomesh_pts);
  FIELD_BL (num_geomesh_faces, 96);
  VALUEOUTOFBOUNDS (num_geomesh_faces, 50000)
  REPEAT_N (FIELD_VALUE (num_geomesh_faces), geomesh_faces, Dwg_GEODATA_meshface)
  REPEAT_BLOCK
      SUB_FIELD_BL (geomesh_faces[rcount1],face1, 97);
      SUB_FIELD_BL (geomesh_faces[rcount1],face2, 98);
      SUB_FIELD_BL (geomesh_faces[rcount1],face3, 99);
  END_REPEAT_BLOCK
  END_REPEAT (geomesh_faces);

  UNTIL (R_2007) // r2009, class_version 1
    {
      ENCODER {
        _obj->ref_pt2d.x = _obj->ref_pt.y; _obj->ref_pt2d.y = _obj->ref_pt.x;
      }
      DXF { VALUE_TFF ("CIVIL3D_DATA_BEGIN", 3); }
      FIELD_B (has_civil_data, 0); // 1
      FIELD_B (obsolete_false, 292); // 0
      FIELD_2RD (ref_pt2d, 14);     // (y, x)
      FIELD_2RD (ref_pt2d, 15);
      FIELD_BL (unknown1, 93); // always 0
      FIELD_BL (unknown2, 94); // always 0
      FIELD_B (unknown_b, 293); // 0
      FIELD_2RD (zero1, 16);   // always origin (0,0)
      FIELD_2RD (zero2, 17);
      FIELD_B (unknown_b, 0); // 0
      FIELD_BD (north_dir_angle_deg, 54);
      FIELD_BD (north_dir_angle_rad, 140);
      FIELD_BL (scale_est, 95);
      FIELD_BD (user_scale_factor, 141);
      FIELD_B (do_sea_level_corr, 294);
      FIELD_BD (sea_level_elev, 142);
      FIELD_BD (coord_proj_radius, 143);
      DXF { VALUE_TFF ("CIVIL3D_DATA_END", 4); }
    }
  START_OBJECT_HANDLE_STREAM;

DWG_OBJECT_END

//pg.220, 20.4.91
DWG_OBJECT (RASTERVARIABLES)
  SUBCLASS (AcDbRasterVariables)
  FIELD_BL (class_version, 90);
  if (FIELD_VALUE (class_version) > 10)
    return DWG_ERR_VALUEOUTOFBOUNDS;
  FIELD_BS (image_frame, 70);
  FIELD_BS (image_quality, 71);
  FIELD_BS (units, 72);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// 20.4.93 page 221
DWG_OBJECT (SORTENTSTABLE)

  SUBCLASS (AcDbSortentsTable)
  FIELD_BL (num_ents, 0);
  VALUEOUTOFBOUNDS (num_ents, 50000)
  // read these code 0 handles from the normal stream
  str_dat = hdl_dat; hdl_dat = dat;
  HANDLE_VECTOR (sort_ents, num_ents, 0, 0);
  hdl_dat = str_dat;

  START_OBJECT_HANDLE_STREAM;
  FIELD_HANDLE (block_owner, 4, 0); // mspace or pspace
  HANDLE_VECTOR_N (ents, FIELD_VALUE (num_ents), 4, 0);

  DXF {
    for (vcount = 0; vcount < _obj->num_ents; vcount++)
      {
        FIELD_HANDLE (ents[vcount], 4, 331);
        FIELD_HANDLE (sort_ents[vcount], 0, 5);
      }
  }

DWG_OBJECT_END

//pg.222, 20.4.94 to clip external references
DWG_OBJECT (SPATIAL_FILTER)

  SUBCLASS (AcDbFilter)
  SUBCLASS (AcDbSpatialFilter)
  FIELD_BS (num_clip_verts, 70);
  VALUEOUTOFBOUNDS (num_clip_verts, 10000)
  FIELD_2RD_VECTOR (clip_verts, num_clip_verts, 10);
  FIELD_3BD (extrusion, 210);
  FIELD_3BD (origin, 11);
  FIELD_BS (display_boundary_on, 71);
  FIELD_BS (front_clip_on, 72);
  if (FIELD_VALUE (front_clip_on))
    FIELD_BD (front_clip_z, 40);

  FIELD_BS (back_clip_on, 73);
  if (FIELD_VALUE (back_clip_on))
    FIELD_BD (back_clip_z, 41);

  FIELD_VECTOR_N (inverse_transform, BD, 12, 40);
  FIELD_VECTOR_N (transform, BD, 12, 40);

  START_OBJECT_HANDLE_STREAM;

DWG_OBJECT_END

//pg.153, unstable, no coverage
DWG_OBJECT (SPATIAL_INDEX)

  SUBCLASS (AcDbIndex)
  FIELD_TIMEBLL (last_updated, 40);
  SUBCLASS (AcDbSpatialIndex)
  FIELD_BD (num1, 40);
  FIELD_BD (num2, 40);
  FIELD_BD (num3, 40);
  FIELD_BD (num4, 40);
  FIELD_BD (num5, 40);
  FIELD_BD (num6, 40);
  FIELD_BL (num_hdls, 90);
  HANDLE_VECTOR (hdls, num_hdls, 5, 330);
  FIELD_BL (bindata_size, 90);
  FIELD_BINARY (bindata, FIELD_VALUE (bindata_size), 310);
  DXF { VALUE_TFF ("END ACDBSPATIALINDEX BINARY DATA", 1); }

  START_OBJECT_HANDLE_STREAM;

DWG_OBJECT_END

// 20.4.101.3 Content format for TABLECONTENT and CellStyle_Field
#define ContentFormat_fields(fmt)                 \
  DXF { VALUE_TFF ("CONTENTFORMAT", 300) }        \
  DXF { VALUE_TFF ("CONTENTFORMAT_BEGIN", 1) }    \
  FIELD_BLx (fmt.property_override_flags, 90);    \
  FIELD_BLx (fmt.property_flags, 91);             \
  FIELD_BLx (fmt.value_data_type, 92);            \
  FIELD_BLx (fmt.value_unit_type, 93);            \
  FIELD_T (fmt.value_format_string, 300);         \
  FIELD_BD (fmt.rotation, 40);                    \
  FIELD_BD (fmt.block_scale, 140);                \
  FIELD_BL (fmt.cell_alignment, 94);              \
  FIELD_CMTC (fmt.content_color, 62);             \
  FIELD_HANDLE (fmt.text_style, 3, 340);          \
  FIELD_BD (fmt.text_height, 144);                \
  DXF { VALUE_TFF ("CONTENTFORMAT_END", 309) }

// Cell style 20.4.101.4 for TABLE, TABLECONTENT, TABLESTYLE, and CELLSTYLEMAP
#define CellStyle_fields(sty)						\
  DXF { VALUE_TFF ("TABLEFORMAT_BEGIN", 1) }				\
  FIELD_BL (sty.type, 90);						\
  FIELD_BSx (sty.data_flags, 170);					\
  if (FIELD_VALUE (sty.data_flags))					\
    {									\
      FIELD_BLx (sty.property_override_flags, 91);			\
      FIELD_BLx (sty.merge_flags, 92);					\
      FIELD_CMTC (sty.bg_color, 62);					\
      FIELD_BL (sty.content_layout, 93);				\
      ContentFormat_fields (sty.content_format);			\
      FIELD_BSx (sty.margin_override_flags, 171);			\
      if (FIELD_VALUE (sty.margin_override_flags))			\
	{								\
	  DXF { VALUE_TFF ("MARGIN", 301) }				\
	  DXF { VALUE_TFF ("CELLMARGIN_BEGIN", 1) }			\
	  FIELD_BD (sty.vert_margin, 40);				\
	  FIELD_BD (sty.horiz_margin, 40);				\
	  FIELD_BD (sty.bottom_margin, 40);				\
	  FIELD_BD (sty.right_margin, 40);				\
	  FIELD_BD (sty.margin_horiz_spacing, 40);			\
	  FIELD_BD (sty.margin_vert_spacing, 40);			\
	  DXF { VALUE_TFF ("CELLMARGIN_END", 309) }			\
	}								\
      FIELD_BL (sty.num_borders, 94); /* 0-6 */			\
      VALUEOUTOFBOUNDS (sty.num_borders, 6);				\
      REPEAT2 (sty.num_borders, sty.borders, Dwg_GridFormat)		\
      REPEAT_BLOCK							\
	DXF {								\
	  if (FIELD_VALUE (sty.borders[rcount2].index_mask))		\
	    {								\
	      SUB_FIELD_BL (sty.borders[rcount2],index_mask, 95);	\
	      VALUE_TFF ("GRIDFORMAT", 302);				\
	      VALUE_TFF ("GRIDFORMAT_BEGIN", 1);			\
	    }								\
	}								\
	SUB_FIELD_BLx (sty.borders[rcount2],index_mask, 0);		\
	if (FIELD_VALUE (sty.borders[rcount2].index_mask))		\
	  {								\
	    SUB_FIELD_BL (sty.borders[rcount2],border_overrides, 90);	\
	    SUB_FIELD_BL (sty.borders[rcount2],border_type, 91);	\
	    SUB_FIELD_CMTC (sty.borders[rcount2],color, 62);		\
	    SUB_FIELD_BLd (sty.borders[rcount2],linewt, 92);		\
	    SUB_FIELD_HANDLE (sty.borders[rcount2],ltype, 3, 340);	\
	    SUB_FIELD_BL (sty.borders[rcount2],visible, 93);		\
	    SUB_FIELD_BD (sty.borders[rcount2],double_line_spacing, 40);\
	  }								\
	DXF { VALUE_TFF ("GRIDFORMAT_END", 309) }			\
      END_REPEAT_BLOCK							\
      END_REPEAT (sty.borders);						\
    }									\
  DXF { VALUE_TFF ("TABLEFORMAT_END", 309) }

#if defined (DEBUG_CLASSES) || defined (IS_FREE)

// clang-format off
#define row tdata.rows[rcount1]
#define cell row.cells[rcount2]
#define content cell.cell_contents[rcount3]
#define geom cell.geometry[0]
#define attr content.attrs[rcount4]
#define merged fdata.merged_cells[rcount1]

// pg.237 20.4.97 for TABLE (2010+) and TABLECONTENT
#define TABLECONTENTs_fields					\
  SUBCLASS (AcDbLinkedData)                                     \
  FIELD_T (ldata.name, 1);					\
  FIELD_T (ldata.description, 300);				\
  SUBCLASS (AcDbLinkedTableData)                                \
  FIELD_BL (tdata.num_cols, 90);				\
  REPEAT (tdata.num_cols, tdata.cols, Dwg_TableDataColumn)	\
  REPEAT_BLOCK							\
      SUB_FIELD_T (tdata.cols[rcount1],name, 300);		\
      DXF { VALUE_TFF ("LINKEDTABLEDATACOLUMN_BEGIN", 1) }      \
      SUB_FIELD_BL (tdata.cols[rcount1],custom_data, 91);	\
      DXF { VALUE_TFF ("DATAMAP_BEGIN", 1) }                    \
      CellStyle_fields (tdata.cols[rcount1].cellstyle);		\
      DXF { VALUE_TFF ("DATAMAP_END", 309) }                    \
      DXF { VALUE_TFF ("LINKEDTABLEDATACOLUMN_END", 309) }      \
      SET_PARENT (tdata.cols[rcount1], &_obj->tdata);		\
  END_REPEAT_BLOCK						\
  END_REPEAT (tdata.cols);					\
  FIELD_BL (tdata.num_rows, 90);				\
  REPEAT (tdata.num_rows, tdata.rows, Dwg_TableRow)		\
  REPEAT_BLOCK							\
      FIELD_BL (row.num_cells, 90);				\
      REPEAT2 (row.num_cells, row.cells, Dwg_TableCell)		\
      REPEAT_BLOCK						\
          SUB_FIELD_BL (cell,flag, 90);				\
          SUB_FIELD_T (cell,tooltip, 300);			\
          SUB_FIELD_BL (cell,customdata, 91);			\
          SUB_FIELD_BL (cell,num_customdata_items, 90);		\
          REPEAT3 (cell.num_customdata_items, cell.customdata_items, Dwg_TABLE_CustomDataItem) \
          REPEAT_BLOCK						\
              SUB_FIELD_T (cell.customdata_items[rcount3],name, 300);	 \
              TABLE_value_fields (cell.customdata_items[rcount3].value); \
              if (error & DWG_ERR_INVALIDTYPE)			\
                {						\
                  JSON_END_REPEAT (cell.customdata_items);	\
                  JSON_END_REPEAT (row.cells);			\
                  JSON_END_REPEAT (tdata.rows);			\
                  return error;					\
                }						\
              SET_PARENT_FIELD (cell.customdata_items[rcount3], cell_parent, &_obj->cell);\
          END_REPEAT_BLOCK					\
          END_REPEAT (cell.customdata_items);			\
          SUB_FIELD_BL (cell,has_linked_data, 92);		\
          if (FIELD_VALUE (cell.has_linked_data))		\
            {							\
              SUB_FIELD_HANDLE (cell,data_link, 5, 340);	\
              SUB_FIELD_BL (cell,num_rows, 93);			\
              SUB_FIELD_BL (cell,num_cols, 94);			\
              SUB_FIELD_BL (cell,unknown, 96);			\
            }							\
          SUB_FIELD_BL (cell,num_cell_contents, 95);		\
          DXF { VALUE_TFF ("CONTENT", 302) }                    \
          DXF { VALUE_TFF ("CELLCONTENT_BEGIN", 1) }            \
          REPEAT3 (cell.num_cell_contents, cell.cell_contents, Dwg_TableCellContent) \
          REPEAT_BLOCK						\
              SUB_FIELD_BL (content,type, 90);			\
              if (FIELD_VALUE (content.type) == 1)		\
                {						\
                  DXF { VALUE_TFF ("VALUE", 300) }              \
                  /* 20.4.99 Value, page 241 */         	\
                  TABLE_value_fields (content.value)		\
                  if (error & DWG_ERR_INVALIDTYPE)		\
                    {						\
                      JSON_END_REPEAT (cell.cell_contents);	\
                      JSON_END_REPEAT (row.cells);		\
                      JSON_END_REPEAT (tdata.rows);		\
                      return error;				\
                    }						\
                }						\
              else if (FIELD_VALUE (content.type) == 2) { /* Field */	\
                SUB_FIELD_HANDLE (content,handle, 3, 340);	\
              }							\
              else if (FIELD_VALUE (content.type) == 4) { /* Block */	\
                SUB_FIELD_HANDLE (content,handle, 3, 340);	\
              }							\
              SUB_FIELD_BL (content,num_attrs, 91);		\
              REPEAT4 (content.num_attrs, content.attrs, Dwg_TableCellContent_Attr) \
              REPEAT_BLOCK					\
                  SUB_FIELD_HANDLE (attr,attdef, 5, 330);	\
                  SUB_FIELD_T (attr,value, 301);		\
                  SUB_FIELD_BL (attr,index, 92);		\
                  SET_PARENT (attr, &_obj->content);	        \
              END_REPEAT_BLOCK					\
              END_REPEAT (content.attrs);			\
              DXF { VALUE_TFF ("CELLCONTENT_END", 309) }        \
              DXF { VALUE_TFF ("FORMATTEDCELLCONTENT_BEGIN", 1) }  \
              FIELD_BS (content.has_content_format_overrides, 170) \
              if (FIELD_VALUE (content.has_content_format_overrides)) \
                {						\
                  ContentFormat_fields (content.content_format);\
                }						\
              DXF { VALUE_TFF ("FORMATTEDCELLCONTENT_END", 309) } \
              SET_PARENT (content, &_obj->cell);                \
          END_REPEAT_BLOCK					\
          END_REPEAT (cell.cell_contents);			\
          SUB_FIELD_BL (cell, style_id, 90);			\
          SUB_FIELD_BL (cell, has_geom_data, 91);		\
          if (FIELD_VALUE (cell.has_geom_data))			\
            {							\
              SUB_FIELD_BL (cell,geom_data_flag, 91);		\
              SUB_FIELD_BD (cell,width_w_gap, 40);		\
              SUB_FIELD_BD (cell,height_w_gap, 41);		\
              SUB_FIELD_BL (cell,num_geometry, 94);		\
              SUB_FIELD_HANDLE (cell,tablegeometry, 4, 330);	\
              REPEAT (cell.num_geometry, cell.geometry, Dwg_CellContentGeometry) \
              REPEAT_BLOCK					\
                  SUB_FIELD_3BD (geom,dist_top_left, 10);	\
                  SUB_FIELD_3BD (geom,dist_center, 11);		\
                  SUB_FIELD_BD (geom,content_width, 43);	\
                  SUB_FIELD_BD (geom,content_height, 44);	\
                  SUB_FIELD_BD (geom,width, 45);		\
                  SUB_FIELD_BD (geom,height, 46);		\
                  SUB_FIELD_BL (geom,unknown, 95);		\
                  SET_PARENT_FIELD (geom, cell_parent, &_obj->cell); \
              END_REPEAT_BLOCK					\
              END_REPEAT (cell.geometry);			\
            }							\
          SET_PARENT_FIELD (cell, row_parent, &_obj->row);	\
      END_REPEAT_BLOCK						\
      END_REPEAT (row.cells);					\
      SUB_FIELD_BL (row,custom_data, 91);			\
      SUB_FIELD_BL (row,num_customdata_items, 90);		\
      REPEAT3 (row.num_customdata_items, row.customdata_items, Dwg_TABLE_CustomDataItem) \
      REPEAT_BLOCK						\
          SUB_FIELD_T (row.customdata_items[rcount3],name, 300);\
          TABLE_value_fields (row.customdata_items[rcount3].value);\
          if (error & DWG_ERR_INVALIDTYPE)			\
            {							\
              JSON_END_REPEAT (row.customdata_items);		\
              JSON_END_REPEAT (tdata.rows);			\
              return error;					\
            }							\
          SET_PARENT_FIELD (row.customdata_items[rcount3], row_parent, &_obj->row); \
      END_REPEAT_BLOCK						\
      END_REPEAT (row.customdata_items);			\
      {								\
        CellStyle_fields (row.cellstyle);			\
        SUB_FIELD_BL (row,style_id, 90);			\
        SUB_FIELD_BL (row,height, 40);				\
      }								\
      SET_PARENT (row, &_obj->tdata);				\
  END_REPEAT_BLOCK						\
  END_REPEAT (tdata.rows);					\
  FIELD_BL (tdata.num_field_refs, 0);				\
  HANDLE_VECTOR (tdata.field_refs, tdata.num_field_refs, 3, 0);	\
  FIELD_BL (fdata.num_merged_cells, 90);			\
  REPEAT (fdata.num_merged_cells, fdata.merged_cells, Dwg_FormattedTableMerged)	\
  REPEAT_BLOCK							\
      SUB_FIELD_BL (merged,top_row, 91);			\
      SUB_FIELD_BL (merged,left_col, 92);			\
      SUB_FIELD_BL (merged,bottom_row, 93);			\
      SUB_FIELD_BL (merged,right_col, 94);			\
      SET_PARENT (merged, &_obj->fdata);			\
  END_REPEAT_BLOCK						\
  END_REPEAT (fdata.merged_cells)

// clang-format on

DWG_OBJECT (TABLECONTENT)
  DECODE_UNKNOWN_BITS
  TABLECONTENTs_fields;

  START_OBJECT_HANDLE_STREAM;
  FIELD_HANDLE (tablestyle, 3, 340);
DWG_OBJECT_END

// pg.229 20.4.96, as ACAD_TABLE (varies)
// works ok for the pre-2010 variant, deriving from INSERT
// r2010+ it is TABLECONTENT
DWG_ENTITY (TABLE)

  DECODE_UNKNOWN_BITS
  SINCE (R_2010) //AC1024
    {
      FIELD_RC (unknown_rc, 0);
      FIELD_HANDLE (tablestyle, 5, 342);
      //FIELD_HANDLE (unknown_h, 5, 0);
      FIELD_BL (unknown_bl, 0);
      VERSION (R_2010)
        FIELD_B (unknown_b, 0); // default 1
      VERSION (R_2013)
        FIELD_BL (unknown_bl1, 0);
      // i.e. TABLECONTENT: 20.4.96.2 AcDbTableContent subclass: 20.4.97
      // FIXME: same offset as TABLECONTENT, or subclass
      TABLECONTENTs_fields;

#undef row
#undef cell
#undef content
#undef geom
#undef attr
#undef merged

    }
  else {
    SUBCLASS (AcDbBlockReference)
    FIELD_3BD (ins_pt, 10);
    VERSIONS (R_13b1, R_14) {
      FIELD_3BD_1 (scale, 41);
    }
    JSON {
      FIELD_BB (scale_flag, 0);
      FIELD_3BD_1 (scale, 41);
    }
    DXF_OR_PRINT {
      if (_obj->scale.x != 1.0 || _obj->scale.y != 1.0 || _obj->scale.z != 1.0)
        FIELD_3BD_1 (scale, 41);
    }
    DECODER_OR_ENCODER {
      SINCE (R_2000)
      {
        FIELD_BB (scale_flag, 0);
        switch (FIELD_VALUE (scale_flag))
          {
            case 0:
              FIELD_VALUE (scale.x) = 1.0;
              FIELD_DD (scale.y, FIELD_VALUE (scale.x), 42);
              FIELD_DD (scale.z, FIELD_VALUE (scale.x), 43);
              break;
            case 1:
              FIELD_VALUE (scale.x) = 1.0;
              FIELD_DD (scale.y, 1.0, 42);
              FIELD_DD (scale.z, 1.0, 43);
              break;
            case 2:
              FIELD_RD (scale.x, 41);
              FIELD_VALUE (scale.y) = FIELD_VALUE (scale.x);
              FIELD_VALUE (scale.z) = FIELD_VALUE (scale.x);
              break;
            case 3:
              FIELD_VALUE (scale.x) = 1.0;
              FIELD_VALUE (scale.y) = 1.0;
              FIELD_VALUE (scale.z) = 1.0;
              break;
            default:
              LOG_ERROR ("Invalid scale_flag in TABLE entity %d\n",
                        (int)FIELD_VALUE (scale_flag))
              _obj->scale_flag = 0;
              DEBUG_HERE_OBJ
              return DWG_ERR_INVALIDTYPE;
              //break;
          }
  #ifndef IS_FREE
        FIELD_3PT_TRACE (scale, DD, 41);
  #endif
      }
    }
  
    FIELD_BD0 (rotation, 50);
    FIELD_3BD (extrusion, 210);
    FIELD_B (has_attribs, 66);
  
    SINCE (R_2004) {
      FIELD_BL (num_owned, 0);
      VALUEOUTOFBOUNDS (num_owned, 10000)
    }
    FIELD_HANDLE (block_header, 5, 2);
    VERSIONS (R_13b1, R_2000)
      {
        if (FIELD_VALUE (has_attribs))
          {
            FIELD_HANDLE (first_attrib, 4, 0);
            FIELD_HANDLE (last_attrib, 4, 0);
          }
      }
  
    SINCE (R_2004)
      {
  #if defined (IS_JSON) || defined (IS_DXF)
        if (!_obj->attribs && _obj->num_owned)
          _obj->num_owned = 0;
  #endif
        HANDLE_VECTOR (attribs, num_owned, 4, 0)
      }
    if (FIELD_VALUE (has_attribs)) {
      FIELD_HANDLE (seqend, 3, 0);
    }
  
    SUBCLASS (AcDbTable)
    FIELD_HANDLE (tablestyle, 5, 342);
    FIELD_BS (flag_for_table_value, 90);
    FIELD_3BD (horiz_direction, 11);
    FIELD_BL (num_cols, 92);
    VALUEOUTOFBOUNDS (num_cols, 5000)
    FIELD_BL (num_rows, 91);
    VALUEOUTOFBOUNDS (num_rows, 5000)
    FIELD_VECTOR (col_widths, BD, num_cols, 142);
    FIELD_VECTOR (row_heights, BD, num_rows, 141);
    FIELD_VALUE (num_cells) = (unsigned long)FIELD_VALUE (num_rows) * FIELD_VALUE (num_cols);
    #define cell cells[rcount1]
    REPEAT (num_cells, cells, Dwg_TABLE_Cell)
    REPEAT_BLOCK
        //SUBCLASS (AcDbDataCell)
        SUB_FIELD_BS (cell,type, 171);
        SUB_FIELD_RC (cell,flags, 172);
        SUB_FIELD_B (cell,is_merged_value, 173);
        SUB_FIELD_B (cell,is_autofit_flag, 174);
        SUB_FIELD_BL (cell,merged_width_flag, 175);
        SUB_FIELD_BL (cell,merged_height_flag, 176);
        DXF {
          PRE (R_2007) {
            SUB_FIELD_CAST (cell,cell_flag_override, BS, BL, 177);
          } LATER_VERSIONS {
            SUB_FIELD_BL (cell,cell_flag_override, 91);
          }
          SUB_FIELD_BS (cell,virtual_edge_flag, 178);
        }
        SUB_FIELD_BD (cell,rotation, 145);

        if (FIELD_VALUE (cell.type) == 1)
          { /* text cell */
            SUB_FIELD_HANDLE0 (cell,text_style, 3, 344);
            // TODO: <r2007 and empty style and shorter than 250, single dxf 1 line
            // else split into mult. text lines
            SUB_FIELD_T (cell,text_value, 1);
            SUB_FIELD_B (cell,additional_data_flag, 0);
          }
        if (FIELD_VALUE (cell.type) == 2)
          { /* block cell */
            SUB_FIELD_HANDLE0 (cell,block_handle, 3, 340);
            SUB_FIELD_BD (cell,block_scale, 144);
            SUB_FIELD_B (cell,additional_data_flag, 0);
            if (FIELD_VALUE (cell.additional_data_flag))
              {
                #define attr cell.attr_defs[rcount2]
                REPEAT2 (cell.num_attr_defs, cell.attr_defs, Dwg_TABLE_AttrDef)
                REPEAT_BLOCK
                    SUB_FIELD_HANDLE (cell.attr_defs[rcount2],attdef, 4, 331);
                    SUB_FIELD_BS (cell.attr_defs[rcount2],index, 179);
                    SUB_FIELD_T (cell.attr_defs[rcount2],text, 300); // dxf?
                END_REPEAT_BLOCK
                END_REPEAT (cell.attr_defs);
                //total_num_attr_defs += FIELD_VALUE (cell.num_attr_defs);
                #undef attr
              }
          }
        if (FIELD_VALUE (cells) &&
            (FIELD_VALUE (cell.type) == 1 ||
             FIELD_VALUE (cell.type) == 2))
          { /* common to both text and block cells */
            if (FIELD_VALUE (cell.additional_data_flag) == 1)
              {
                BITCODE_BL cell_flag;
                SUB_FIELD_BL (cell,cell_flag_override, 0);
                cell_flag = FIELD_VALUE (cell.cell_flag_override);
                SUB_FIELD_RC (cell,virtual_edge_flag, 0);
  
                if (cell_flag & 0x01)
                  SUB_FIELD_RS (cell,cell_alignment, 170);
                if (cell_flag & 0x02)
                  SUB_FIELD_B (cell,bg_fill_none, 283);
                if (cell_flag & 0x04)
                  SUB_FIELD_CMTC (cell,bg_color, 63);
                if (cell_flag & 0x08)
                  {
                    SUB_FIELD_CMTC (cell,content_color, 64);
                    SUB_FIELD_HANDLE (cell,text_style, 3, 7); //?
                  }
                if (cell_flag & 0x10) {
                  SUB_FIELD_HANDLE (cell,text_style, 3, 7);
                }
                if (cell_flag & 0x20)
                  SUB_FIELD_BD (cell,text_height, 140);
                if (cell_flag & 0x00040)
                  SUB_FIELD_CMTC (cell,top_grid_color, 69);
                if (cell_flag & 0x00400)
                  SUB_FIELD_BS (cell,top_grid_linewt, 279);
                if (cell_flag & 0x04000)
                  SUB_FIELD_BS (cell,top_visibility, 289);
                if (cell_flag & 0x00080)
                  SUB_FIELD_CMTC (cell,right_grid_color, 65);
                if (cell_flag & 0x00800)
                  SUB_FIELD_BS (cell,right_grid_linewt, 275);
                if (cell_flag & 0x08000)
                  SUB_FIELD_BS (cell,right_visibility, 285);
                if (cell_flag & 0x00100)
                  SUB_FIELD_CMTC (cell,bottom_grid_color, 66);
                if (cell_flag & 0x01000)
                  SUB_FIELD_BS (cell,bottom_grid_linewt, 276);
                if (cell_flag & 0x10000)
                  SUB_FIELD_BS (cell,bottom_visibility, 286);
                if (cell_flag & 0x00200)
                  SUB_FIELD_CMTC (cell,left_grid_color, 68);
                if (cell_flag & 0x02000)
                  SUB_FIELD_BS (cell,left_grid_linewt, 278);
                if (cell_flag & 0x20000)
                  SUB_FIELD_BS (cell,left_visibility, 288);
  
                SUB_FIELD_BL (cell,unknown, 0);
  
                // 20.4.99 Value, page 241
                TABLE_value_fields (cell.value)
                if (error & DWG_ERR_INVALIDTYPE)
                  {
                    JSON_END_REPEAT (cells);
                    return error;
                  }
              }
          }
        SET_PARENT_OBJ (cell);
    END_REPEAT_BLOCK
#ifndef IS_FREE // FIXME: bogus cells value 0x21, not a pointer
    END_REPEAT (cells);
#endif
    #undef cell
    /* End Cell Data (remaining data applies to entire table)*/
  
    /* COMMON: */
    FIELD_B (has_table_overrides, 0);
    if (FIELD_VALUE (has_table_overrides))
      {
        BITCODE_BL table_flag;
        FIELD_BL (table_flag_override, 93);
        table_flag = FIELD_VALUE (table_flag_override);
        if (table_flag & 0x0001)
          FIELD_B (title_suppressed, 280);
        FIELD_B (header_suppressed, 281); // yes, unchecked. always true
        if (table_flag & 0x0004)
          FIELD_BS (flow_direction, 70);
        if (table_flag & 0x0008)
          FIELD_BD (horiz_cell_margin, 40);
        if (table_flag & 0x0010)
          FIELD_BD (vert_cell_margin, 41);
        if (table_flag & 0x0020)
          FIELD_CMTC (title_row_color, 64); // CMTC?
        if (table_flag & 0x0040)
          FIELD_CMTC (header_row_color, 64); // CMTC?
        if (table_flag & 0x0080)
          FIELD_CMTC (data_row_color, 64);
        if (table_flag & 0x0100)
          FIELD_B (title_row_fill_none, 283);
        if (table_flag & 0x0200)
          FIELD_B (header_row_fill_none, 283);
        if (table_flag & 0x0400)
          FIELD_B (data_row_fill_none, 283);
        if (table_flag & 0x0800)
          FIELD_CMTC (title_row_fill_color, 63); // CMTC?
        if (table_flag & 0x1000)
          FIELD_CMTC (header_row_fill_color, 63); // CMTC?
        if (table_flag & 0x2000)
          {
            FIELD_CMTC (data_row_fill_color, 63); // CMTC?
            FIELD_HANDLE (title_row_style_override, ANYCODE, 7);
          }
        if (table_flag & 0x4000)
          FIELD_BS (title_row_alignment, 170);
        if (table_flag & 0x8000)
          FIELD_BS (header_row_alignment, 170);
        if (table_flag & 0x10000)
          FIELD_BS (data_row_alignment, 170);
        if (table_flag & 0x20000)
          FIELD_HANDLE (title_text_style, 5, 7);
        if (table_flag & 0x40000)
          {
            FIELD_HANDLE (header_text_style, 5, 7);
            //FIELD_HANDLE (header_row_style_override, ANYCODE, 7); ??
          }
        if (table_flag & 0x80000)
          FIELD_HANDLE (data_text_style, 5, 7);
        if (table_flag & 0x100000)
          FIELD_BD (title_row_height, 140);
        if (table_flag & 0x200000)
          FIELD_BD (header_row_height, 140);
        if (table_flag & 0x400000)
          FIELD_BD (data_row_height, 140);
      }
  
    FIELD_B (has_border_color_overrides, 0);
    if (FIELD_VALUE (has_border_color_overrides))
      {
        BITCODE_BL border_color;
        FIELD_BL (border_color_overrides_flag, 94);
        border_color = FIELD_VALUE (border_color_overrides_flag);
        if (border_color & 0x0001)
          FIELD_CMTC (title_horiz_top_color, 64);
        if (border_color & 0x0002)
          FIELD_CMTC (title_horiz_ins_color, 65);
        if (border_color & 0x0004)
          FIELD_CMTC (title_horiz_bottom_color, 66);
        if (border_color & 0x0008)
          FIELD_CMTC (title_vert_left_color, 63);
        if (border_color & 0x0010)
          FIELD_CMTC (title_vert_ins_color, 68);
        if (border_color & 0x0020)
          FIELD_CMTC (title_vert_right_color, 69);
        if (border_color & 0x0040)
          FIELD_CMTC (header_horiz_top_color, 64);
        if (border_color & 0x0080)
          FIELD_CMTC (header_horiz_ins_color, 65);
        if (border_color & 0x0100)
          FIELD_CMTC (header_horiz_bottom_color, 66);
        if (border_color & 0x0200)
          FIELD_CMTC (header_vert_left_color, 63);
        if (border_color & 0x0400)
          FIELD_CMTC (header_vert_ins_color, 68);
        if (border_color & 0x0800)
          FIELD_CMTC (header_vert_right_color, 69);
        if (border_color & 0x1000)
          FIELD_CMTC (data_horiz_top_color, 64);
        if (border_color & 0x2000)
          FIELD_CMTC (data_horiz_ins_color, 65);
        if (border_color & 0x4000)
          FIELD_CMTC (data_horiz_bottom_color, 66);
        if (border_color & 0x8000)
          FIELD_CMTC (data_vert_left_color, 63);
        if (border_color & 0x10000)
          FIELD_CMTC (data_vert_ins_color, 68);
        if (border_color & 0x20000)
          FIELD_CMTC (data_vert_right_color, 69);
      }
  
    FIELD_B (has_border_lineweight_overrides, 0);
    if (FIELD_VALUE (has_border_lineweight_overrides))
      {
        BITCODE_BL border_linewt;
        FIELD_BL (border_lineweight_overrides_flag, 95);
        border_linewt = FIELD_VALUE (border_lineweight_overrides_flag);
        if (border_linewt & 0x0001)
          FIELD_BS (title_horiz_top_linewt, 0);
        if (border_linewt & 0x0002)
          FIELD_BS (title_horiz_ins_linewt, 0);
        if (border_linewt & 0x0004)
          FIELD_BS (title_horiz_bottom_linewt, 0);
        if (border_linewt & 0x0008)
          FIELD_BS (title_vert_left_linewt, 0);
        if (border_linewt & 0x0010)
          FIELD_BS (title_vert_ins_linewt, 0);
        if (border_linewt & 0x0020)
          FIELD_BS (title_vert_right_linewt, 0);
        if (border_linewt & 0x0040)
          FIELD_BS (header_horiz_top_linewt, 0);
        if (border_linewt & 0x0080)
          FIELD_BS (header_horiz_ins_linewt, 0);
        if (border_linewt & 0x0100)
          FIELD_BS (header_horiz_bottom_linewt, 0);
        if (border_linewt & 0x0200)
          FIELD_BS (header_vert_left_linewt, 0);
        if (border_linewt & 0x0400)
          FIELD_BS (header_vert_ins_linewt, 0);
        if (border_linewt & 0x0800)
          FIELD_BS (header_vert_right_linewt, 0);
        if (border_linewt & 0x1000)
          FIELD_BS (data_horiz_top_linewt, 0);
        if (border_linewt & 0x2000)
          FIELD_BS (data_horiz_ins_linewt, 0);
        if (border_linewt & 0x4000)
          FIELD_BS (data_horiz_bottom_linewt, 0);
        if (border_linewt & 0x8000)
          FIELD_BS (data_vert_left_linewt, 0);
        if (border_linewt & 0x10000)
          FIELD_BS (data_vert_ins_linewt, 0);
        if (border_linewt & 0x20000)
          FIELD_BS (data_vert_right_linewt, 0);
      }
  
    FIELD_B (has_border_visibility_overrides, 0);
    if (FIELD_VALUE (has_border_visibility_overrides))
      {
        BITCODE_BL border_visibility;
        FIELD_BL (border_visibility_overrides_flag, 96);
        border_visibility = FIELD_VALUE (border_visibility_overrides_flag);
        if (border_visibility & 0x0001)
          FIELD_BS (title_horiz_top_visibility, 0);
        if (border_visibility & 0x0002)
          FIELD_BS (title_horiz_ins_visibility, 0);
        if (border_visibility & 0x0004)
          FIELD_BS (title_horiz_bottom_visibility, 0);
        if (border_visibility & 0x0008)
          FIELD_BS (title_vert_left_visibility, 0);
        if (border_visibility & 0x0010)
          FIELD_BS (title_vert_ins_visibility, 0);
        if (border_visibility & 0x0020)
          FIELD_BS (title_vert_right_visibility, 0);
        if (border_visibility & 0x0040)
          FIELD_BS (header_horiz_top_visibility, 0);
        if (border_visibility & 0x0080)
          FIELD_BS (header_horiz_ins_visibility, 0);
        if (border_visibility & 0x0100)
          FIELD_BS (header_horiz_bottom_visibility, 0);
        if (border_visibility & 0x0200)
          FIELD_BS (header_vert_left_visibility, 0);
        if (border_visibility & 0x0400)
          FIELD_BS (header_vert_ins_visibility, 0);
        if (border_visibility & 0x0800)
          FIELD_BS (header_vert_right_visibility, 0);
        if (border_visibility & 0x1000)
          FIELD_BS (data_horiz_top_visibility, 0);
        if (border_visibility & 0x2000)
          FIELD_BS (data_horiz_ins_visibility, 0);
        if (border_visibility & 0x4000)
          FIELD_BS (data_horiz_bottom_visibility, 0);
        if (border_visibility & 0x8000)
          FIELD_BS (data_vert_left_visibility, 0);
        if (border_visibility & 0x10000)
          FIELD_BS (data_vert_ins_visibility, 0);
        if (border_visibility & 0x20000)
          FIELD_BS (data_vert_right_visibility, 0);
      }
  
    COMMON_ENTITY_HANDLE_DATA;
  }
  SINCE (R_2010)
  {
    //... p237
    LOG_WARN ("TODO TABLE r2010+")
  
    FIELD_BS (unknown_bs, 0); //default 38
    FIELD_3BD (hor_dir, 11);
    FIELD_BL (has_break_data, 0); //BL or B?
    if (FIELD_VALUE (has_break_data))
      {
        FIELD_BL (break_flag, 0);
        FIELD_BL (break_flow_direction, 0);
        FIELD_BD (break_spacing, 0);
        FIELD_BL (break_unknown1, 0);
        FIELD_BL (break_unknown2, 0);
        FIELD_BL (num_break_heights, 0);
        VALUEOUTOFBOUNDS (num_break_heights, 5000)
        REPEAT (num_break_heights, break_heights, Dwg_TABLE_BreakHeight)
        REPEAT_BLOCK
            SUB_FIELD_3BD (break_heights[rcount1],position, 0);
            SUB_FIELD_BD (break_heights[rcount1],height, 0);
            SUB_FIELD_BL (break_heights[rcount1],flag, 0); // default: 2
            SET_PARENT_OBJ (break_heights[rcount1]);
        END_REPEAT_BLOCK
        END_REPEAT (break_heights);
      }
    FIELD_BL (num_break_rows, 0);
    VALUEOUTOFBOUNDS (num_break_rows, 5000)
    REPEAT (num_break_rows, break_rows, Dwg_TABLE_BreakRow)
    REPEAT_BLOCK
        SUB_FIELD_3BD (break_rows[rcount1],position, 0);
        SUB_FIELD_BL (break_rows[rcount1],start, 0);
        SUB_FIELD_BL (break_rows[rcount1],end, 0);
        SET_PARENT_OBJ (break_rows[rcount1]);
    END_REPEAT_BLOCK
    END_REPEAT (break_rows);

    COMMON_ENTITY_HANDLE_DATA;
    FIELD_HANDLE (tablestyle, 5, 342);
  }

DWG_ENTITY_END

#undef row
#undef cell
#undef content
#undef geom
#undef attr
#undef merged

#endif /* DEBUG_CLASSES */

// See TABLE and p20.4.101
// Added with r2005. unstable for r2010+
// TABLESTYLE only contains the Table (R24), _Title, _Header and _Data cell style.
DWG_OBJECT (TABLESTYLE)
  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbTableStyle)
  UNTIL (R_2007) {
    FIELD_T (name, 3);
    FIELD_BS (flow_direction, 70);
    FIELD_BS (flags, 71);
    FIELD_BD (horiz_cell_margin, 40);
    FIELD_BD (vert_cell_margin, 41);
    FIELD_B (is_title_suppressed, 280);
    FIELD_B (is_header_suppressed, 281);
#ifdef IS_ENCODER
    if (dwg->header.from_version > R_2007)
      downconvert_TABLESTYLE (obj);
#endif
  }
  LATER_VERSIONS { // r2010+
#ifdef IS_ENCODER
    //if (dwg->header.from_version <= R_2007)
    //  upconvert_TABLESTYLE (obj);
#endif
    FIELD_RCd (unknown_rc, 70);
    FIELD_T (name, 3);
    FIELD_BL (unknown_bl1, 0);
    FIELD_BL (unknown_bl2, 0);
    FIELD_HANDLE (cellstyle, DWG_HDL_HARDOWN, 0);
    CellStyle_fields (sty.cellstyle);
    DXF { VALUE_TFF ("CELLSTYLE_BEGIN", 1) }
    FIELD_BL0 (sty.id, 90);
    FIELD_BL0 (sty.type, 91);
    FIELD_T0 (sty.name, 300);
    DXF { VALUE_TFF ("CELLSTYLE_END", 309) }

    DECODER {
      FIELD_VALUE (flow_direction) =
        _obj->sty.cellstyle.property_override_flags & 0x10000;
    }
    FIELD_BL (numoverrides, 0);
    // FIXME style overrides for 0-6
    if (FIELD_VALUE (numoverrides))
      {
        FIELD_BL (unknown_bl3, 0);
        CellStyle_fields (ovr.cellstyle);
        DXF { VALUE_TFF ("CELLSTYLE_BEGIN", 1) }
        FIELD_BL0 (ovr.id, 90);
        FIELD_BL0 (ovr.type, 91);
        FIELD_T0 (ovr.name, 300);
        DXF { VALUE_TFF ("CELLSTYLE_END", 309) }
        LOG_WARN ("TODO TABLESTYLE r2010+ missing fields")
      }
  }

  UNTIL (R_2007) {
    FIELD_VALUE (num_rowstyles) = 3;
    // 0: data, 1: title, 2: header
    #define rowstyle rowstyles[rcount1]
    REPEAT_CN (3, rowstyles, Dwg_TABLESTYLE_rowstyles)
    REPEAT_BLOCK
        // TODO in DXF by name
        SUB_FIELD_HANDLE (rowstyle,text_style, 5, 7);
        SUB_FIELD_BD (rowstyle,text_height, 140);
        SUB_FIELD_BS (rowstyle,text_alignment, 170);
        SUB_FIELD_CMTC (rowstyle,text_color, 62);
        SUB_FIELD_CMTC (rowstyle,fill_color, 63);
        SUB_FIELD_B (rowstyle,has_bgcolor, 283);

        _obj->rowstyle.num_borders = 6;
        // grid: top, horizontal inside, bottom, left, vertical inside, right
        _REPEAT_CN (6, rowstyle.borders, Dwg_TABLESTYLE_border, 2)
        REPEAT_BLOCK
            #define border rowstyle.borders[rcount2]
            SUB_FIELD_BSd (border,linewt, 274+rcount2);
            SUB_FIELD_B (border,visible, 284+rcount2);
            SUB_FIELD_CMTC (border,color, 64+rcount2);
        END_REPEAT_BLOCK
        END_REPEAT (rowstyle.borders)

        //SINCE (R_2007) {
        //  SUB_FIELD_BL (rowstyle,data_type, 90);
        //  SUB_FIELD_BL (rowstyle,unit_type, 91);
        //  SUB_FIELD_TU (rowstyle,format_string, 1);
        //}
        #undef border
    END_REPEAT_BLOCK
    END_REPEAT (rowstyles)
    #undef rowstyle
  }
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// pg.246 20.4.102 and TABLE
// added with r2008, backcompat with r2007
// The cellstyle map can contain custom cell styles, whereas the TABLESTYLE
// only contains the Table (R24), _Title, _Header and _Data cell style.
DWG_OBJECT (CELLSTYLEMAP)
  SUBCLASS (AcDbCellStyleMap)
  FIELD_BL (num_cells, 90);
  REPEAT (num_cells, cells, Dwg_TABLESTYLE_CellStyle)
  REPEAT_BLOCK
      DXF { VALUE_TFF ("CELLSTYLE", 300); }
      CellStyle_fields (cells[rcount1].cellstyle);
      DXF { VALUE_TFF ("CELLSTYLE_BEGIN", 1) }
      SUB_FIELD_BL (cells[rcount1],id, 90);
      SUB_FIELD_BL (cells[rcount1],type, 91);
      SUB_FIELD_T (cells[rcount1],name, 300);
      DXF { VALUE_TFF ("CELLSTYLE_END", 309) }
      SET_PARENT (cells[rcount1], (Dwg_Object_TABLESTYLE*)_obj);
  END_REPEAT_BLOCK
  END_REPEAT (cells);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// pg.246 20.4.103
// unstable. Cannot be encoded for now
DWG_OBJECT (TABLEGEOMETRY)
  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbTableGeometry)
  FIELD_BL (numrows, 90);
  VALUEOUTOFBOUNDS (numrows, 5000)
  FIELD_BL (numcols, 91);
  VALUEOUTOFBOUNDS (numcols, 5000)
  FIELD_BL (num_cells, 92);
  VALUEOUTOFBOUNDS (num_cells, 10000)
  REPEAT (num_cells, cells, Dwg_TABLEGEOMETRY_Cell)
  REPEAT_BLOCK
      #define cell cells[rcount1]
      SUB_FIELD_BL (cell,geom_data_flag, 93);
      SUB_FIELD_BD (cell,width_w_gap, 40);
      SUB_FIELD_BD (cell,height_w_gap, 41);
      SUB_FIELD_HANDLE (cell,tablegeometry, 4, 330);
      SUB_FIELD_BL (cell,num_geometry, 94);
      VALUEOUTOFBOUNDS (cell.num_geometry, 10000)
      REPEAT2 (cell.num_geometry, cell.geometry, Dwg_CellContentGeometry)
      REPEAT_BLOCK
          #define geom cell.geometry[rcount2]
          SUB_FIELD_3BD (geom,dist_top_left, 10);
          SUB_FIELD_3BD (geom,dist_center, 11);
          SUB_FIELD_BD (geom,content_width, 43);
          SUB_FIELD_BD (geom,content_height, 44);
          SUB_FIELD_BD (geom,width, 45);
          SUB_FIELD_BD (geom,height, 46);
          SUB_FIELD_BL (geom,unknown, 95);
          SET_PARENT_FIELD (geom, geom_parent, &_obj->cell);
          #undef geom
      END_REPEAT_BLOCK
      END_REPEAT (cell.geometry);
      SET_PARENT_OBJ (cell);
      #undef cell
  END_REPEAT_BLOCK
  END_REPEAT (cells);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

//(79 + varies) pg.247 20.4.104
DWG_OBJECT (XRECORD)

  DXF {
    SUBCLASS (AcDbXrecord)
    SINCE (R_2000) {
      FIELD_BS0 (cloning, 280);
    }
  }
  ENCODER {
    size_t pos = bit_position (dat);
    unsigned xdata_size = _obj->xdata_size;
    FIELD_BL (xdata_size, 0);
    FIELD_XDATA (xdata, xdata_size);
    if (xdata_size != _obj->xdata_size)
      { // easiest is to write both again.
        // else do BL patching with very unlikely bitwise memmove
        bit_set_position (dat, pos);
        FIELD_BL (xdata_size, 0);
        FIELD_XDATA (xdata, xdata_size);
      }
  } else {
    FIELD_BL (xdata_size, 0);
    FIELD_XDATA (xdata, xdata_size);
  }
  SINCE (R_2000) {
    FIELD_BS (cloning, 0);
  }

  START_OBJECT_HANDLE_STREAM;
  DECODER {
      for (vcount=0; bit_position (hdl_dat) < obj->handlestream_size; vcount++)
        {
          FIELD_VALUE (objid_handles) = vcount
            ? (BITCODE_H*)realloc (FIELD_VALUE (objid_handles),
                                   (vcount+1) * sizeof (Dwg_Object_Ref))
            : (BITCODE_H*)malloc (sizeof (Dwg_Object_Ref));
          FIELD_HANDLE_N (objid_handles[vcount], vcount, ANYCODE, 0);
          if (!FIELD_VALUE (objid_handles[vcount]))
            {
              if (!vcount) {
                free (FIELD_VALUE (objid_handles));
                FIELD_VALUE (objid_handles) = NULL;
              }
              break;
            }
        }
      FIELD_VALUE (num_objid_handles) = vcount;
    }
  VALUEOUTOFBOUNDS (num_objid_handles, 10000)
#ifndef IS_FREE
  FIELD_TRACE (num_objid_handles, BL);
#endif
#ifndef IS_DECODER
  HANDLE_VECTOR (objid_handles, num_objid_handles, 4, 0);
#endif
#ifdef IS_DXF
  if (FIELD_VALUE (objid_handles)) {
    REPEAT (num_objid_handles, objid_handles, T)
      VALUE_HANDLE (_obj->objid_handles[rcount1], objid_handles, 4, 340);
    END_REPEAT (objid_handles)
  }
#endif

DWG_OBJECT_END

//(80 + varies)
/// DXF as ACDBPLACEHOLDER
DWG_OBJECT (PLACEHOLDER)
  // no own data members
  // no SUBCLASS marker
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// SCALE (varies)
// 20.4.92 page 221
DWG_OBJECT (SCALE)
  SUBCLASS (AcDbScale)
  FIELD_BS (flag, 70); // always 0
  FIELD_T (name, 300);
  FIELD_BD (paper_units, 140);
  FIELD_BD (drawing_units, 141);
  FIELD_B (is_unit_scale, 290);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// VBA_PROJECT (81 + varies), a blob
DWG_OBJECT (VBA_PROJECT)

  SINCE (R_2000) {
    SUBCLASS (AcDbVbaProject)
#ifndef IS_JSON
    FIELD_BL (data_size, 90);
#endif
#ifndef IS_FREE
    if (!obj->size || _obj->data_size < obj->size)
      {
        FIELD_BINARY (data, FIELD_VALUE (data_size), 310);
      }
    else
      LOG_ERROR ("Invalid data_size ignored");
#else
    FIELD_BINARY (data, FIELD_VALUE (data_size), 310);
#endif

    START_OBJECT_HANDLE_STREAM;
  }
DWG_OBJECT_END

#define MLEADER_CONTEXT_DATA_fields                                                \
  DXF_OR_PRINT { VALUE_TFF ("CONTEXT_DATA{", 300); } /* AcDbObjectContextData */   \
  FIELD_BD (ctx.scale_factor, 40);                                                 \
  FIELD_3BD (ctx.content_base, 10);                                                \
  FIELD_BD (ctx.text_height, 41);                                                  \
  FIELD_BD (ctx.arrow_size, 140);                                                  \
  FIELD_BD (ctx.landing_gap, 145);                                                 \
  FIELD_BS (ctx.text_left, 174);                                                   \
  FIELD_BS (ctx.text_right, 175);                                                  \
  FIELD_BS (ctx.text_angletype, 176);                                              \
  FIELD_BS (ctx.text_alignment, 177);                                              \
  FIELD_B (ctx.has_content_txt, 290);                                              \
  if (FIELD_VALUE (ctx.has_content_txt))                                           \
    {                                                                              \
      DECODER { FIELD_VALUE (ctx.content.txt.type) = 2; }                          \
      FIELD_T (ctx.content.txt.default_text, 304);                                 \
      FIELD_3BD (ctx.content.txt.normal, 11);                                      \
      FIELD_HANDLE (ctx.content.txt.style, 5, 340);                                \
      FIELD_3BD (ctx.content.txt.location, 12);                                    \
      FIELD_3BD (ctx.content.txt.direction, 13);                                   \
      FIELD_BD (ctx.content.txt.rotation, 42);                                     \
      FIELD_BD (ctx.content.txt.width, 43);                                        \
      FIELD_BD (ctx.content.txt.height, 44);                                       \
      FIELD_BD (ctx.content.txt.line_spacing_factor, 45);                          \
      FIELD_BS (ctx.content.txt.line_spacing_style, 170);                          \
      FIELD_CMC (ctx.content.txt.color, 90);                                       \
      FIELD_BS (ctx.content.txt.alignment, 171);                                   \
      FIELD_BS (ctx.content.txt.flow, 172);                                        \
      FIELD_CMC (ctx.content.txt.bg_color, 91);                                    \
      FIELD_BD (ctx.content.txt.bg_scale, 141);                                    \
      FIELD_BL (ctx.content.txt.bg_transparency, 92);                              \
      FIELD_B (ctx.content.txt.is_bg_fill, 291);                                   \
      FIELD_B (ctx.content.txt.is_bg_mask_fill, 292);                              \
      FIELD_BS (ctx.content.txt.col_type, 173);                                    \
      FIELD_B (ctx.content.txt.is_height_auto, 293);                               \
      FIELD_BD (ctx.content.txt.col_width, 142);                                   \
      FIELD_BD (ctx.content.txt.col_gutter, 143);                                  \
      FIELD_B (ctx.content.txt.is_col_flow_reversed, 294);                         \
      FIELD_BL (ctx.content.txt.num_col_sizes, 0);                                 \
      /*VALUEOUTOFBOUNDS (ctx.content.txt.num_col_sizes, 5000)*/                   \
      FIELD_VECTOR (ctx.content.txt.col_sizes, BD, ctx.content.txt.num_col_sizes, 144); \
      FIELD_B (ctx.content.txt.word_break, 295);                                   \
      FIELD_B (ctx.content.txt.unknown, 0);                                        \
      DXF { VALUE_B (0, 296); }                                                    \
    }                                                                              \
  else /* a union. either txt or blk */                                            \
    {                                                                              \
      FIELD_B (ctx.has_content_blk, 296);                                          \
      if (FIELD_VALUE (ctx.has_content_blk))                                       \
        {                                                                          \
          DECODER { FIELD_VALUE (ctx.content.txt.type) = 1; }                      \
          FIELD_HANDLE (ctx.content.blk.block_table, 4, 341);                      \
          FIELD_3BD (ctx.content.blk.normal, 14);                                  \
          FIELD_3BD (ctx.content.blk.location, 15);                                \
          FIELD_3BD (ctx.content.blk.scale, 16);                                   \
          FIELD_BD (ctx.content.blk.rotation, 46);                                 \
          FIELD_CMC (ctx.content.blk.color, 93);                                   \
          FIELD_VECTOR_N (ctx.content.blk.transform, BD, 16, 47);                  \
        }                                                                          \
    }                                                                              \
                                                                                   \
  FIELD_3BD (ctx.base, 110);                                                       \
  FIELD_3BD (ctx.base_dir, 111);  /* dxf only 2d? */                               \
  FIELD_3BD (ctx.base_vert, 112); /* dxf only 2d */                                \
  FIELD_B (ctx.is_normal_reversed, 297);                                           \
  /* END MLEADER_AnnotContext */


/* pg. 157, 20.4.48 (varies)
   AcDbMLeader
 */
DWG_ENTITY (MULTILEADER)

  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbMLeader)
  SINCE (R_2010) {
    FIELD_BS (class_version, 270); // default 2. 1 <= r2004
    VALUEOUTOFBOUNDS (class_version, 10)
  }
#ifdef IS_DXF
  MLEADER_CONTEXT_DATA_fields;
#endif
  FIELD_BL (ctx.num_leaders, 0);
  VALUEOUTOFBOUNDS (ctx.num_leaders, 5000) // MAX_LEADER_NUMBER
  DXF_OR_PRINT { VALUE_TFF ("LEADER{", 302); }
  REPEAT (ctx.num_leaders, ctx.leaders, Dwg_LEADER_Node)
  REPEAT_BLOCK
      #define lnode ctx.leaders[rcount1]
      SUB_FIELD_B (lnode, has_lastleaderlinepoint, 290);
      SUB_FIELD_B (lnode, has_dogleg, 291);
      if (FIELD_VALUE (lnode.has_lastleaderlinepoint))
        SUB_FIELD_3BD (lnode, lastleaderlinepoint, 10);
      if (FIELD_VALUE (lnode.has_dogleg))
        SUB_FIELD_3BD (lnode, dogleg_vector, 11);
      SUB_FIELD_BL (lnode, num_breaks, 0);
      VALUEOUTOFBOUNDS (lnode.num_breaks, 5000)
      REPEAT2 (lnode.num_breaks, lnode.breaks, Dwg_LEADER_Break)
      REPEAT_BLOCK
          SUB_FIELD_3BD (lnode.breaks[rcount2], start, 11);
          SUB_FIELD_3BD (lnode.breaks[rcount2], end, 12);
          SET_PARENT (lnode.breaks[rcount2], (struct _dwg_LEADER_Line *)&_obj->lnode);
      END_REPEAT_BLOCK
      END_REPEAT (lnode.breaks);

      SUB_FIELD_BL (lnode, branch_index, 90);
      SUB_FIELD_BD (lnode, dogleg_length, 40);
      DXF_OR_PRINT { VALUE_TFF ("LEADER_LINE{", 304); }
      SUB_FIELD_BL (lnode, num_lines, 0);
      VALUEOUTOFBOUNDS (lnode.num_lines, 5000)
      REPEAT2 (lnode.num_lines, lnode.lines, Dwg_LEADER_Line)
      REPEAT_BLOCK
          #define lline lnode.lines[rcount2]
          SUB_FIELD_BL (lline, num_points, 0);
          FIELD_3DPOINT_VECTOR (lline.points, lline.num_points, 10);
          SUB_FIELD_BL (lline, num_breaks, 0);
          VALUEOUTOFBOUNDS (lline.num_breaks, 5000)
          REPEAT3 (lline.num_breaks, lline.breaks, Dwg_LEADER_Break)
          REPEAT_BLOCK
              SUB_FIELD_3BD (lline.breaks[rcount3], start, 11);
              SUB_FIELD_3BD (lline.breaks[rcount3], end, 12);
              SET_PARENT (lline.breaks[rcount3], &_obj->lline);
          END_REPEAT_BLOCK
          END_REPEAT (lline.breaks);
          SUB_FIELD_BL (lline, line_index, 91);

          SINCE (R_2010)
            {
              SUB_FIELD_BS (lline, type, 170);
              SUB_FIELD_CMC (lline, color, 92);
              SUB_FIELD_HANDLE (lline, ltype, 5, 340);
              SUB_FIELD_BLd (lline, linewt, 171);
              SUB_FIELD_BD (lline, arrow_size, 40);
              SUB_FIELD_HANDLE (lline, arrow_handle, 5, 341);
              SUB_FIELD_BL (lline, flags, 93);
            }
            SET_PARENT (lline, &_obj->lnode);
            #undef lline
      END_REPEAT_BLOCK
      END_REPEAT (lnode.lines)
      SINCE (R_2010)
        SUB_FIELD_BS (lnode, attach_dir, 271);
      DXF_OR_PRINT { VALUE_TFF ("}", 305); }
      SET_PARENT_OBJ (lnode);
  END_REPEAT_BLOCK
  END_REPEAT (ctx.leaders)

#ifndef IS_DXF
  MLEADER_CONTEXT_DATA_fields;
#endif
  DXF_OR_PRINT { VALUE_TFF ("}", 303); }
  SINCE (R_2010)
    {
      FIELD_BS (ctx.text_top, 273);
      FIELD_BS (ctx.text_bottom, 272);
    }
  DXF_OR_PRINT { VALUE_TFF ("}", 301); }
  /* end CONTEXT_DATA */

  FIELD_HANDLE (mleaderstyle, 5, 340);
  FIELD_BLx (flags, 90); // override flags
  FIELD_BS (type, 170);
  FIELD_CMC (line_color, 91);
  FIELD_HANDLE (line_ltype, 5, 341);
  FIELD_BLd (line_linewt, 171);
  FIELD_B (has_landing, 290);
  FIELD_B (has_dogleg, 291);
  FIELD_BD (landing_dist, 41);
  DECODER {
    if (bit_isnan (FIELD_VALUE (landing_dist)))
      {
        FIELD_VALUE (landing_dist) = 0.0;
        return DWG_ERR_VALUEOUTOFBOUNDS;
      }
  }
  FIELD_HANDLE0 (arrow_handle, 5, 342);
  FIELD_BD0 (arrow_size, 42);
  FIELD_BS (style_content, 172);
  FIELD_HANDLE (text_style, 5, 343);
  FIELD_BS (text_left, 173);
  FIELD_BS (text_right, 95);
  FIELD_BS (text_angletype, 174);
  FIELD_BS (text_alignment, 175); // unknown at ODA
  FIELD_CMC (text_color, 92);
  FIELD_B (has_text_frame, 292);
  FIELD_HANDLE0 (block_style, 5, 344);
  FIELD_CMC (block_color, 93);
  FIELD_3BD (block_scale, 10);
  FIELD_BD (block_rotation, 43);
  FIELD_BS (style_attachment, 176);
  FIELD_B (is_annotative, 293);

  VERSIONS (R_14, R_2007)
    {
      FIELD_BL (num_arrowheads, 0);
      VALUEOUTOFBOUNDS (num_arrowheads, 5000)
      REPEAT (num_arrowheads, arrowheads, Dwg_LEADER_ArrowHead)
      REPEAT_BLOCK
          SUB_FIELD_B (arrowheads[rcount1],is_default, 94);
          SUB_FIELD_HANDLE (arrowheads[rcount1],arrowhead, 5, 345);
          SET_PARENT_OBJ (arrowheads[rcount1]);
      END_REPEAT_BLOCK
      END_REPEAT (arrowheads);

      FIELD_BL (num_blocklabels, 0);
      VALUEOUTOFBOUNDS (num_blocklabels, 5000)
      REPEAT (num_blocklabels, blocklabels, Dwg_LEADER_BlockLabel)
      REPEAT_BLOCK
          SUB_FIELD_HANDLE (blocklabels[rcount1],attdef, 4, 330);
          SUB_FIELD_T (blocklabels[rcount1],label_text, 302);
          SUB_FIELD_BS (blocklabels[rcount1],ui_index, 177);
          SUB_FIELD_BD (blocklabels[rcount1],width, 44);
          SET_PARENT_OBJ (blocklabels[rcount1]);
      END_REPEAT_BLOCK
      END_REPEAT (blocklabels)
      FIELD_B (is_neg_textdir, 294);
      FIELD_BS (ipe_alignment, 178);
      FIELD_BS (justification, 179);
      FIELD_BD (scale_factor, 45);
    }

  SINCE (R_2010)
    {
      FIELD_BS (attach_dir, 271);
      FIELD_BS (attach_top, 273);
      FIELD_BS (attach_bottom, 272);
    }
  SINCE (R_2013)
    FIELD_B (is_text_extended, 295);

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/* par 20.4.87 (varies) */
DWG_OBJECT (MLEADERSTYLE)

  SUBCLASS (AcDbMLeaderStyle)
  SINCE (R_2010) {
    IF_ENCODE_FROM_EARLIER {
      FIELD_VALUE (class_version) = 2;
    }
    // else set on EED for APPID “ACAD_MLEADERVER”
    FIELD_BS (class_version, 179);
    VALUEOUTOFBOUNDS (class_version, 10)
  }
  else {
    FIELD_VALUE (class_version) = 2;
    JSON { FIELD_BS (class_version, 0); }
  }
  FIELD_BS (content_type, 170);
  FIELD_BS (mleader_order, 171);
  FIELD_BS (leader_order, 172);
  FIELD_BL (max_points, 90);
  FIELD_BD (first_seg_angle, 40);
  FIELD_BD (second_seg_angle, 41);
  FIELD_BS (type, 173);
  FIELD_CMC (line_color, 91);
  FIELD_HANDLE (line_type, 5, 340);
  FIELD_BLd (linewt, 92);
  FIELD_B (has_landing, 290);
  FIELD_BD (landing_gap, 42);
  FIELD_B (has_dogleg, 291);
  FIELD_BD (landing_dist, 43);
  FIELD_T (description, 3);
  FIELD_HANDLE (arrow_head, 5, 341);
  FIELD_BD (arrow_head_size, 44);
  FIELD_T (text_default, 300);
  FIELD_HANDLE (text_style, 5, 342);
  FIELD_BS (attach_left, 174);
  FIELD_BS (attach_right, 178);
  if (FIELD_VALUE (class_version) >= 2) {
    FIELD_BS (text_angle_type, 175);
  }
  FIELD_BS (text_align_type, 176);
  FIELD_CMC (text_color, 93); // as RGB only
  FIELD_BD (text_height, 45);
  FIELD_B (has_text_frame, 292);
  if (FIELD_VALUE (class_version) >= 2) {
    FIELD_B (text_always_left, 297);
  } else {
    DXF {
      FIELD_B (text_always_left, 297); // in DXF always
    }
  }
  FIELD_BD (align_space, 46);
  FIELD_HANDLE (block, 5, 343);
  FIELD_CMC (block_color, 94);
  JSON {
    FIELD_3BD (block_scale, 0)
  } else {
    FIELD_BD (block_scale.x, 47);
    FIELD_BD (block_scale.y, 49);
    FIELD_BD (block_scale.z, 140);
  }
  FIELD_B (use_block_scale, 293);
  FIELD_BD (block_rotation, 141);
  FIELD_B (use_block_rotation, 294);
  FIELD_BS (block_connection, 177);
  FIELD_BD (scale, 142);
  FIELD_B (is_changed, 295);
  FIELD_B (is_annotative, 296);
  FIELD_BD (break_size, 143);

  SINCE (R_2010)
    {
      FIELD_BS (attach_dir, 271);
      FIELD_BS (attach_top, 273);
      FIELD_BS (attach_bottom, 272);
    }
  SINCE (R_2013) {
    FIELD_B (text_extended, 298);
  }
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

////////////////////
// These variable objects are not described in the spec:
//

DWG_OBJECT (WIPEOUTVARIABLES)
  SUBCLASS (AcDbWipeoutVariables)
  //DXF { VALUE_BL (0, 90); } /* class_version */
  FIELD_BS (display_frame, 70);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// R2000+ picture. undocumented (varies)
// unstable. cannot be encoded for now
DWG_ENTITY (WIPEOUT)

  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbWipeout)
  // this must be an exact copy of AcDbRasterImage
  FIELD_BL (class_version, 90);
  VALUEOUTOFBOUNDS (class_version, 10)
  FIELD_3DPOINT (pt0, 10);
  FIELD_3DPOINT (uvec, 11);
  FIELD_3DPOINT (vvec, 12);
  FIELD_2RD (size, 13);
  FIELD_HANDLE (imagedef, 5, 340);
  FIELD_BS (display_props, 70);
  FIELD_B (clipping, 280);
  FIELD_RC (brightness, 281);
  FIELD_RC (contrast, 282);
  FIELD_RC (fade, 283);
  FIELD_HANDLE (imagedefreactor, 3, 360);
  SINCE (R_2010) {
    FIELD_B (clip_mode, 0);
  }
  FIELD_BS (clip_boundary_type, 71); // 1 rect, 2 polygon
  if (FIELD_VALUE (clip_boundary_type) == 1)
    FIELD_VALUE (num_clip_verts) = 2;
  else
    FIELD_BL (num_clip_verts, 91);
  VALUEOUTOFBOUNDS (num_clip_verts, 5000)
  FIELD_2RD_VECTOR (clip_verts, num_clip_verts, 14);
  DXF { SINCE (R_2010) { // is_inverted
    FIELD_B (clip_mode, 290);
  } }
  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

// (varies)
DWG_OBJECT (PDFDEFINITION)
  SUBCLASS (AcDbUnderlayDefinition)
  FIELD_T (filename, 1);
  FIELD_T (name, 2);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (DGNDEFINITION)
  SUBCLASS (AcDbUnderlayDefinition)
  FIELD_T (filename, 1);
  FIELD_T (name, 2);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// no coverage
DWG_OBJECT (DWFDEFINITION)
  SUBCLASS (AcDbUnderlayDefinition)
  FIELD_T (filename, 1);
  FIELD_T (name, 2);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// (varies)
// In C++ as UNDERLAYREFERENCE. A bit better than WIPEOUT.
#define UNDERLAY_fields                                                 \
  SUBCLASS (AcDbUnderlayReference)                                      \
  FIELD_HANDLE (definition_id, 5, 340);                                 \
  FIELD_3BD (extrusion, 0);                                             \
  FIELD_3DPOINT (ins_pt, 10);                                           \
  FIELD_BD0 (angle, 0);                                                 \
  DXF {                                                                 \
    if (_obj->scale.x != 1.0 || _obj->scale.y != 1.0 || _obj->scale.z != 1.0) \
      FIELD_3BD_1 (scale, 41);                                          \
  }                                                                     \
  else {                                                                \
    FIELD_3BD_1 (scale, 41);                                            \
  }                                                                     \
  DXF {                                                                 \
    FIELD_BD0 (angle, 50);                                              \
    FIELD_BE (extrusion, 210);                                          \
  }                                                                     \
  FIELD_RC0 (flag, 280);                                                \
  FIELD_RCd (contrast, 281); /* 20-100. def: 100. DXF optional */       \
  FIELD_RCd (fade, 282);     /* 0-80. DXF opt */                        \
  FIELD_BL (num_clip_verts, 0);                                         \
  VALUEOUTOFBOUNDS (num_clip_verts, 5000)                               \
    FIELD_2RD_VECTOR (clip_verts, num_clip_verts, 11);                  \
  if (FIELD_VALUE (flag) & 16)                                          \
    {                                                                   \
      FIELD_BS0 (num_clip_inverts, 170);                                \
      FIELD_2RD_VECTOR (clip_inverts, num_clip_inverts, 12);            \
    }                                                                   \
  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY (PDFUNDERLAY)
  UNDERLAY_fields
DWG_ENTITY_END

// no coverage yet
DWG_ENTITY (DGNUNDERLAY)
  UNDERLAY_fields
DWG_ENTITY_END

// no coverage yet
DWG_ENTITY (DWFUNDERLAY)
  UNDERLAY_fields
DWG_ENTITY_END

DWG_ENTITY (CAMERA) // i.e. a named view, not persistent in a DWG. CAMERADISPLAY=1
  //DECODE_UNKNOWN_BITS
  COMMON_ENTITY_HANDLE_DATA;
  FIELD_HANDLE (view, 5, 0);
DWG_ENTITY_END

// sectionplane, r2007+
DWG_ENTITY (SECTIONOBJECT)
  //DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbSection)
  FIELD_BL (state, 90);
  FIELD_BL (flags, 91);
  FIELD_T (name, 1);
  FIELD_3BD (vert_dir, 10);
  FIELD_BD (top_height, 40);
  FIELD_BD (bottom_height, 41);
  FIELD_BS (indicator_alpha, 70);
  FIELD_CMTC (indicator_color, 62); //dxf doc bug: 63, 411
  FIELD_BL (num_verts, 92);
  FIELD_3DPOINT_VECTOR (verts, num_verts, 11);
  FIELD_BL (num_blverts, 93);
  FIELD_3DPOINT_VECTOR (blverts, num_blverts, 12);

  COMMON_ENTITY_HANDLE_DATA;
  FIELD_HANDLE (section_settings, 5, 360);
DWG_ENTITY_END

DWG_OBJECT (SECTION_MANAGER)
  //DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbSectionManager)
  FIELD_B (is_live, 70);
  FIELD_BS (num_sections, 90);
  START_OBJECT_HANDLE_STREAM;
  HANDLE_VECTOR (sections, num_sections, 5, 330);
DWG_OBJECT_END

// Unstable
DWG_OBJECT (SECTION_SETTINGS)
  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbSectionSettings)
  FIELD_BL (curr_type, 90);
  FIELD_BL (num_types, 91);
  VALUEOUTOFBOUNDS (num_types, 4) // max 4 types: live on/off, 2d, 3d
  REPEAT (num_types, types, Dwg_SECTION_typesettings)
  REPEAT_BLOCK
      DXF { VALUE_TFF ("SectionTypeSettings", 1); }
      SUB_FIELD_BL (types[rcount1], type, 90);
      SUB_FIELD_BL (types[rcount1], generation, 91);
      SUB_FIELD_BL (types[rcount1], num_sources, 92);
      SUB_HANDLE_VECTOR (types[rcount1], sources, num_sources, 5, 330);
      SUB_FIELD_HANDLE (types[rcount1], destblock, 4, 331);
      SUB_FIELD_T (types[rcount1], destfile, 1);
      SUB_FIELD_BL (types[rcount1], num_geom, 93);
      REPEAT2 (types[rcount1].num_geom, types[rcount1].geom, Dwg_SECTION_geometrysettings)
      REPEAT_BLOCK
          DXF { VALUE_TFF ("SectionGeometrySettings", 2); }
          SUB_FIELD_BL (types[rcount1].geom[rcount2], num_geoms, 90);
          SUB_FIELD_BL (types[rcount1].geom[rcount2], hexindex, 91);
          SUB_FIELD_BL (types[rcount1].geom[rcount2], flags, 92);
          SUB_FIELD_CMC (types[rcount1].geom[rcount2], color, 62);
          SUB_FIELD_T (types[rcount1].geom[rcount2], layer, 8);
          SUB_FIELD_T (types[rcount1].geom[rcount2], ltype, 6);
          SUB_FIELD_BD (types[rcount1].geom[rcount2], ltype_scale, 40);
          SUB_FIELD_T (types[rcount1].geom[rcount2], plotstyle, 1);
          SINCE (R_2000)
            SUB_FIELD_BLd (types[rcount1].geom[rcount2], linewt, 370);
          SUB_FIELD_BS (types[rcount1].geom[rcount2], face_transparency, 70);
          SUB_FIELD_BS (types[rcount1].geom[rcount2], edge_transparency, 71);
          SUB_FIELD_BS (types[rcount1].geom[rcount2], hatch_type, 72);
          ENCODER {
            if (bit_empty_T (dat, _obj->types[rcount1].geom[rcount2].hatch_pattern))
              _obj->types[rcount1].geom[rcount2].hatch_pattern = bit_set_T (dat, "SOLID");
          }
          SUB_FIELD_T (types[rcount1].geom[rcount2], hatch_pattern, 2);
          DECODER {
            if (bit_empty_T (dat, _obj->types[rcount1].geom[rcount2].hatch_pattern))
              {
                free (_obj->types[rcount1].geom[rcount2].hatch_pattern);
                _obj->types[rcount1].geom[rcount2].hatch_pattern = bit_set_T (dat, "SOLID");
              }
          }
          SUB_FIELD_BD (types[rcount1].geom[rcount2], hatch_angle, 41);
          SUB_FIELD_BD (types[rcount1].geom[rcount2], hatch_spacing, 42);
          SUB_FIELD_BD (types[rcount1].geom[rcount2], hatch_scale, 43);
          DXF { VALUE_TFF ("SectionGeometrySettingsEnd", 3); }
          SET_PARENT (types[rcount1].geom[rcount2], &_obj->types[rcount1]);
      END_REPEAT_BLOCK
      END_REPEAT (types[rcount1].geom)
      DXF { VALUE_TFF ("SectionTypeSettingsEnd", 3); }
      SET_PARENT_OBJ (types[rcount1]);
  END_REPEAT_BLOCK
  END_REPEAT (types)
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

#ifndef IS_DXF

/* UNKNOWN (varies)
   container to hold a unknown class entity, see classes.inc
   every DEBUGGING class holds a bits array, an prefix offset and a bitsize.
   It starts after the common_entity|object_data until and goes until the end
   of final padding, to the CRC.
   (obj->address+obj->common_size/8 .. obj->address+obj->size)
 */
DWG_ENTITY (UNKNOWN_ENT)
  DECODE_UNKNOWN_BITS
  //COMMON_ENTITY_HANDLE_DATA; // including this
DWG_ENTITY_END

/* container to hold a raw class object (varies) */
DWG_OBJECT (UNKNOWN_OBJ)
  DECODE_UNKNOWN_BITS
DWG_OBJECT_END

// just a dummy dwg filer, ignored for dxf.
// for now we use it as empty PROXY_OBJECT
DWG_OBJECT (DUMMY)
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

#endif /* IS_DXF */

DWG_OBJECT (LONG_TRANSACTION)
  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbLongTransaction)
  LOG_INFO ("TODO LONG_TRANSACTION\n");
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// (varies) UNSTABLE
DWG_OBJECT (OBJECT_PTR) //empty? only xdata. CAseDLPNTableRecord
  DECODE_UNKNOWN_BITS
  DEBUG_HERE_OBJ
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

/* In work area:
   The following entities/objects are all stored with partial fields,
   plus as raw bits for examples/unknown.
   Coverage might be missing for some cases, or field names may change.
 */

// abstract subclass. as field value
#define AcDbEvalVariant_fields(value)                                         \
  {                                                                           \
    int dxf;                                                                  \
    SUB_FIELD_BSd (value, code, 70);                                          \
    dxf = FIELD_VALUE (value.code);                                           \
    if (dxf)                                                                  \
      switch (dwg_resbuf_value_type (dxf))                                    \
        {                                                                     \
        case DWG_VT_REAL:                                                     \
          SUB_FIELD_BD (value, u.bd, dxf);                                    \
          break;                                                              \
        case DWG_VT_INT32:                                                    \
          SUB_FIELD_BL (value, u.bl, dxf);                                    \
          break;                                                              \
        case DWG_VT_INT16:                                                    \
          SUB_FIELD_BS (value, u.bs, dxf);                                    \
          break;                                                              \
        case DWG_VT_INT8:                                                     \
          SUB_FIELD_RC (value, u.rc, dxf);                                    \
          break;                                                              \
        case DWG_VT_STRING:                                                   \
          SUB_FIELD_T (value, u.text, dxf);                                   \
          break;                                                              \
        case DWG_VT_HANDLE:                                                   \
          SUB_FIELD_HANDLE (value, u.handle, 5, dxf);                         \
          break;                                                              \
        case DWG_VT_BINARY:                                                   \
        case DWG_VT_OBJECTID:                                                 \
        case DWG_VT_POINT3D:                                                  \
        case DWG_VT_INVALID:                                                  \
        case DWG_VT_INT64:                                                    \
        case DWG_VT_BOOL:                                                     \
        default:                                                              \
          LOG_ERROR ("Invalid EvalVariant.value.type %d", _obj->value.code)   \
          break;                                                              \
        }                                                                     \
    else /* dxf */                                                            \
      {                                                                       \
        DXF { VALUE_TFF ("AcDbEvalVariant NULL = -9999", 309) }               \
      }                                                                       \
  }

#define AcDbValueParam_fields(valprefix)                                \
  FIELD_BL (valprefix.class_version, 90);                               \
  FIELD_T (valprefix.name, 1);                                          \
  FIELD_BL (valprefix.unit_type, 90);                                   \
  FIELD_BL (valprefix.num_vars, 90);                                    \
  REPEAT2 (valprefix.num_vars, valprefix.vars, Dwg_VALUEPARAM_vars)     \
  REPEAT_BLOCK                                                          \
    AcDbEvalVariant_fields (valprefix.vars[rcount2].value);             \
    FIELD_HANDLE (valprefix.vars[rcount2].handle, 4, 330);              \
  END_REPEAT_BLOCK                                                      \
  END_REPEAT (valprefix.vars)                                           \
  FIELD_HANDLE (valprefix.controlled_objdep, 4, 330)
    
#define AcDbAssocDependency_fields                         \
  SUBCLASS (AcDbAssocDependency);                          \
  FIELD_BS (assocdep.class_version, 90); /* 2 */           \
  VALUEOUTOFBOUNDS (assocdep.class_version, 3);            \
  FIELD_BL (assocdep.status, 90);                          \
  FIELD_B (assocdep.is_read_dep, 290);                     \
  FIELD_B (assocdep.is_write_dep, 290);                    \
  FIELD_B (assocdep.is_attached_to_object, 290);           \
  FIELD_B (assocdep.is_delegating_to_owning_action, 290);  \
  FIELD_BLd (assocdep.order, 90); /* -1 or 0 or -10000 */  \
  FIELD_HANDLE (assocdep.dep_on, 3, 330);                  \
  FIELD_B (assocdep.has_name, 290);                        \
  if (FIELD_VALUE (assocdep.has_name)) {                   \
    FIELD_T (assocdep.name, 1);                            \
  }                                                        \
  FIELD_HANDLE (assocdep.readdep, 4, 330);                 \
  FIELD_HANDLE (assocdep.node, 3, 330);                    \
  FIELD_HANDLE (assocdep.dep_body, 4, 360);                \
  FIELD_BLd (assocdep.depbodyid, 90)

// (varies) UNSTABLE
// works ok on all Surface_20* but this coverage seems limited.
// field names may change.
// See AcDbAssocDependency.h
DWG_OBJECT (ASSOCDEPENDENCY)
  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbAssocDependency);
  FIELD_BS (class_version, 90);
  VALUEOUTOFBOUNDS (class_version, 3);
  FIELD_BL (status, 90);
  FIELD_B (is_read_dep, 290);
  FIELD_B (is_write_dep, 290);
  FIELD_B (is_attached_to_object, 290);
  FIELD_B (is_delegating_to_owning_action, 290);
  FIELD_BLd (order, 90); /* -1 or 0 */
  FIELD_HANDLE (dep_on, 3, 330);
  FIELD_B (has_name, 290);
  if (FIELD_VALUE (has_name)) {
    FIELD_T (name, 1);
  }
  FIELD_HANDLE (readdep, 4, 330);
  FIELD_HANDLE (node, 3, 330);
  FIELD_HANDLE (dep_body, 4, 360);
  FIELD_BLd (depbodyid, 90);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

#define AcDbAssocActionParam_fields       \
  SUBCLASS (AcDbAssocActionParam)         \
  SINCE (R_2013) { _obj->is_r2013 = 1; }  \
  FIELD_BS (is_r2013, 90);                \
  SINCE (R_2013) {                        \
    FIELD_BL (aap_version, 90);           \
  }                                       \
  FIELD_T (name, 1)

#define AcDbAssocActionBody_fields  \
  SUBCLASS (AcDbAssocActionBody)    \
  FIELD_BL (aab_version, 90)

// embedded struct, not inlined
#define AcDbAssocParamBasedActionBody_fields(pab)           \
  PRE (R_2013) {                                            \
    SUBCLASS (AcDbAssocParamBasedActionBody);               \
    SUB_FIELD_BL (pab,version, 90);                         \
    SUB_FIELD_BL (pab,minor, 90);                           \
    SUB_FIELD_BL (pab,num_deps, 90);                        \
    SUB_HANDLE_VECTOR (pab,deps, num_deps, 4, 360);         \
    SUB_FIELD_BL (pab,l4, 90);                              \
    SUB_FIELD_BL (pab,num_values, 90);                      \
    if (!FIELD_VALUE (pab.num_values)) {                    \
      SUB_FIELD_BL (pab,l5, 90);                            \
      SUB_FIELD_HANDLE (pab,assocdep, 5, 330);              \
    }                                                       \
    REPEAT (pab.num_values, pab.values, Dwg_VALUEPARAM)     \
    REPEAT_BLOCK                                            \
        AcDbValueParam_fields (pab.values[rcount1])         \
    END_REPEAT_BLOCK                                        \
    END_REPEAT (pab.values)                                 \
  }

#define AcDbAssocSurfaceActionBody_fields(sab)              \
  SUBCLASS (AcDbAssocSurfaceActionBody)                     \
  SUB_FIELD_BL (sab,version, 90);                           \
  SUB_FIELD_HANDLE (sab,assocdep, 5, 330);                  \
  SUB_FIELD_B  (sab,is_semi_assoc, 290);                    \
  SUB_FIELD_BL (sab,l2, 90);                                \
  SUB_FIELD_B  (sab,is_semi_ovr, 290);                      \
  SUB_FIELD_BS (sab,grip_status, 70)

#define AcDbAssocPathBasedSurfaceActionBody_fields          \
  AcDbAssocActionBody_fields;                               \
  AcDbAssocParamBasedActionBody_fields (pab);               \
  AcDbAssocSurfaceActionBody_fields (sab);                  \
  SUBCLASS (AcDbAssocPathBasedSurfaceActionBody)            \
  FIELD_BL (pbsab_status, 90)

// (varies) UNSTABLE
// works ok on all Surface_20* but this coverage seems limited.
// See AcDbAssocActionBody.h
DWG_OBJECT (ASSOCPLANESURFACEACTIONBODY)
  DECODE_UNKNOWN_BITS
  AcDbAssocPathBasedSurfaceActionBody_fields;
  SUBCLASS (AcDbAssocPlaneSurfaceActionBody)
  FIELD_BL (class_version, 90);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// (varies) UNSTABLE
DWG_OBJECT (ASSOCEXTENDSURFACEACTIONBODY)
  DECODE_UNKNOWN_BITS
  AcDbAssocPathBasedSurfaceActionBody_fields;
  SUBCLASS (AcDbAssocExtendSurfaceActionBody)
  FIELD_BL (class_version, 90);
  FIELD_RC (option, 280);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// (varies) UNSTABLE
// See AcDbAssocActionBody.h and ASSOCPLANESURFACEACTIONBODY
DWG_OBJECT (ASSOCEXTRUDEDSURFACEACTIONBODY)
  DECODE_UNKNOWN_BITS
  AcDbAssocPathBasedSurfaceActionBody_fields;
  SUBCLASS (AcDbAssocExtrudedSurfaceActionBody)
  FIELD_BL (class_version, 90);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// (varies) UNSTABLE
// See AcDbAssocActionBody.h and ASSOCPLANESURFACEACTIONBODY
DWG_OBJECT (ASSOCLOFTEDSURFACEACTIONBODY)
  DECODE_UNKNOWN_BITS
  AcDbAssocPathBasedSurfaceActionBody_fields;
  SUBCLASS (AcDbAssocLoftedSurfaceActionBody)
  FIELD_BL (class_version, 90);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// (varies) UNSTABLE
DWG_OBJECT (ASSOCNETWORKSURFACEACTIONBODY)
  DECODE_UNKNOWN_BITS
  AcDbAssocPathBasedSurfaceActionBody_fields;
  SUBCLASS (AcDbAssocNetworkSurfaceActionBody)
  DXF { FIELD_BL (class_version, 90); }
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// (varies) UNSTABLE
DWG_OBJECT (ASSOCOFFSETSURFACEACTIONBODY)
  DECODE_UNKNOWN_BITS
  AcDbAssocPathBasedSurfaceActionBody_fields;
  SUBCLASS (AcDbAssocOffsetSurfaceActionBody)
  FIELD_BL (class_version, 90);
  FIELD_B (b1, 290);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// (varies) UNSTABLE
// See AcDbAssocActionBody.h and ASSOCPLANESURFACEACTIONBODY
DWG_OBJECT (ASSOCREVOLVEDSURFACEACTIONBODY)
  DECODE_UNKNOWN_BITS
  AcDbAssocPathBasedSurfaceActionBody_fields;
  SUBCLASS (AcDbAssocRevolvedSurfaceActionBody)
  FIELD_BL (class_version, 90);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// (varies) UNSTABLE
DWG_OBJECT (ASSOCTRIMSURFACEACTIONBODY)
  DECODE_UNKNOWN_BITS
  AcDbAssocPathBasedSurfaceActionBody_fields;
  SUBCLASS (AcDbAssocTrimSurfaceActionBody)
  FIELD_BL (class_version, 90);
  FIELD_B (b1, 290);
  FIELD_B (b2, 290);
  FIELD_BD (distance, 40);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// (varies) UNSTABLE
DWG_OBJECT (ASSOCBLENDSURFACEACTIONBODY)
  DECODE_UNKNOWN_BITS
  AcDbAssocPathBasedSurfaceActionBody_fields;
  SUBCLASS (AcDbAssocBlendSurfaceActionBody)
  FIELD_BL (class_version, 90);
  FIELD_B (b1, 290);
  FIELD_B (b2, 291);
  FIELD_B (b3, 292);
  FIELD_BS (blend_options, 72);
  FIELD_B (b4, 293);
  FIELD_B (b5, 294);
  FIELD_BS (bs2, 73);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// (varies) UNSTABLE
// See AcDbAssocActionBody.h and ASSOCPLANESURFACEACTIONBODY
DWG_OBJECT (ASSOCPATCHSURFACEACTIONBODY)
  DECODE_UNKNOWN_BITS
  AcDbAssocPathBasedSurfaceActionBody_fields;
  SUBCLASS (AcDbAssocPatchSurfaceActionBody)
  DXF { FIELD_BL (class_version, 90); }
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// (varies) UNSTABLE
DWG_OBJECT (ASSOCFILLETSURFACEACTIONBODY)
  DECODE_UNKNOWN_BITS
  AcDbAssocPathBasedSurfaceActionBody_fields;
  SUBCLASS (AcDbAssocFilletSurfaceActionBody)
  FIELD_BL (class_version, 90);
  FIELD_BS (status, 70);
  FIELD_2RD (pt1, 10);
  FIELD_2RD (pt2, 10);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

/*
TvVisualStyle:
  FIELD_T (name, 0);
  FIELD_B (is_default, 0);
 */

// r2007+ STABLE
// dbvisualstyle.h
DWG_OBJECT (VISUALSTYLE)
  SUBCLASS (AcDbVisualStyle)

  UNTIL (R_2007) {
    DECODER { // unstable might exit, use sane defaults
      FIELD_VALUE (edge_crease_angle) = 1.0;
      FIELD_VALUE (edge_opacity) = 1.0;
      FIELD_VALUE (edge_width) = 1;
      FIELD_VALUE (edge_silhouette_width) = 3; // or 5
      FIELD_VALUE (edge_overhang) = 6;
      FIELD_VALUE (edge_jitter) = 2;
      FIELD_VALUE (display_settings) = 1;
      SINCE (R_2010) {
        FIELD_VALUE (internal_only) = 1;
        FIELD_VALUE (edge_crease_angle_int) = 1;
        FIELD_VALUE (edge_color_int) = 1;
        FIELD_VALUE (edge_opacity_int) = 1;
        FIELD_VALUE (edge_width_int) = 1;
        FIELD_VALUE (edge_overhang_int) = 1;
        FIELD_VALUE (edge_jitter_int) = 1;
        FIELD_VALUE (edge_silhouette_color_int) = 1;
        FIELD_VALUE (edge_silhouette_width_int) = 1;
      }
    }
  }

  FIELD_T (description, 2);
  FIELD_BL (style_type, 70); // enum of internal styles, 0-27
  PRE (R_2010) {
    FIELD_BL (face_lighting_model, 71);
    FIELD_BL (face_lighting_quality, 72);
    FIELD_BL (face_color_mode, 73);
    DXF { FIELD_BL (face_modifier, 90); }
    FIELD_BD (face_opacity, 40);
    FIELD_BD (face_specular, 41);
    DXF { VALUE_BL (5, 62); } // color
    FIELD_CMC (face_mono_color, 63);
    FIELD_BL (face_modifier, 0);

    FIELD_BL (edge_model, 74);
    FIELD_BL (edge_style, 91);
    FIELD_CMC (edge_intersection_color, 64);
    FIELD_CMC (edge_obscured_color, 65);
    FIELD_BLd (edge_obscured_ltype, 75);
    DXF { FIELD_BL (edge_intersection_ltype, 175); }
    FIELD_BD (edge_crease_angle, 42);
    if (_obj->edge_crease_angle < -360.0 || _obj->edge_crease_angle > 360.0)
    {
      LOG_ERROR ("Invalid edge_crease_angle %f, skipping", _obj->edge_crease_angle);
      _obj->edge_crease_angle = 0.0;
      return DWG_ERR_VALUEOUTOFBOUNDS;
    }

    FIELD_BL (edge_modifier, 92);
    FIELD_CMC (edge_color, 66);
    FIELD_BD (edge_opacity, 43);
    FIELD_CAST (edge_width, BS, BL, 76); // 1
    FIELD_CAST (edge_overhang, BS, BL, 77); // 6
    FIELD_BL (edge_jitter, 78); // 2 documented as BS
    FIELD_CMC (edge_silhouette_color, 67);
    FIELD_CAST (edge_silhouette_width, BS, BL, 79); // 3 or 5
    FIELD_CAST (edge_halo_gap, RC, BL, 170); // 0
    FIELD_CAST (edge_isolines, BS, BL, 171);
    VALUEOUTOFBOUNDS (edge_isolines, 5000)
    FIELD_B (edge_do_hide_precision, 290);
    FIELD_CAST (edge_style_apply, BS, BL, 174);
    FIELD_CAST (edge_intersection_ltype, BS, BL, 0); // DXF above
    FIELD_BL (display_settings, 93); // 1
    FIELD_BLd (display_brightness_bl, 44); // 0
    DECODER {
      FIELD_VALUE (display_brightness) = (double)FIELD_VALUE (display_brightness_bl);
    }
    FIELD_BL (display_shadow_type, 173); // 0
    DXF { FIELD_B (internal_only, 291); }
    SINCE (R_2007) {
      FIELD_BD (bd2007_45, 45);  // 0.0
    }
    FIELD_B (internal_only, 0);
  }
  SINCE (R_2010) {
    if (!_obj->ext_lighting_model) {
      DXF { _obj->ext_lighting_model = dat->version < R_2013 ? 2 : 3; }
      ENCODER { _obj->ext_lighting_model = 2; }
    }
    FIELD_BS (ext_lighting_model, 177);
    FIELD_B (internal_only, 291);
    if (!IF_IS_DXF || dat->version == R_2010) {
      FIELD_BL (face_lighting_model, 71);       FIELD_BS (face_lighting_model_int, 176);   /* 0 */
      FIELD_BL (face_lighting_quality, 72);     FIELD_BS (face_lighting_quality_int, 176); /* 1 */
      FIELD_BL (face_color_mode, 73);           FIELD_BS (face_color_mode_int, 176);       /* 2 */
      FIELD_CAST (face_modifier, BS, BL, 90);   FIELD_BS (face_modifier_int, 176);
      FIELD_BD (face_opacity, 40);              FIELD_BS (face_opacity_int, 176);
      FIELD_BD (face_specular, 41);             FIELD_BS (face_specular_int, 176);
      FIELD_CMC (face_mono_color, 63);          FIELD_BS (face_mono_color_int, 176);
      FIELD_BL (edge_model, 74);                FIELD_BS (edge_model_int, 176);
      FIELD_BL (edge_style, 91);                FIELD_BS (edge_style_int, 176);
      FIELD_CMC (edge_intersection_color, 64);  FIELD_BS (edge_intersection_color_int, 176);
      FIELD_CMC (edge_obscured_color, 65);      FIELD_BS (edge_obscured_color_int, 176);
      FIELD_BL (edge_obscured_ltype, 75);       FIELD_BS (edge_obscured_ltype_int, 176);
      FIELD_BL (edge_intersection_ltype, 175);  FIELD_BS (edge_intersection_ltype_int, 176);
      FIELD_BD (edge_crease_angle, 42);         FIELD_BS (edge_crease_angle_int, 176);
      FIELD_BL (edge_modifier, 92);             FIELD_BS (edge_modifier_int, 176); // this may be 0,1, or 2
      FIELD_CMC (edge_color, 66);               FIELD_BS (edge_color_int, 176);
      FIELD_BD (edge_opacity, 43);              FIELD_BS (edge_opacity_int, 176);
      FIELD_BL (edge_width, 76);                FIELD_BS (edge_width_int, 176);
      FIELD_BL (edge_overhang, 77);             FIELD_BS (edge_overhang_int, 176);
      FIELD_BL (edge_jitter, 78);               FIELD_BS (edge_jitter_int, 176);
      FIELD_CMC (edge_silhouette_color, 67);    FIELD_BS (edge_silhouette_color_int, 176);
      FIELD_BL (edge_silhouette_width, 79);     FIELD_BS (edge_silhouette_width_int, 176);
      FIELD_BL (edge_halo_gap, 170);            FIELD_BS (edge_halo_gap_int, 176);
      FIELD_BL (edge_isolines, 171);
      VALUEOUTOFBOUNDS (edge_isolines, 5000)    FIELD_BS (edge_isolines_int, 176);
      FIELD_B (edge_do_hide_precision, 290);    FIELD_BS (edge_do_hide_precision_int, 176);

      FIELD_BL (display_settings, 93);          FIELD_BS (display_settings_int, 176);
      FIELD_BD (display_brightness, 44);        FIELD_BS (display_brightness_int, 176);
      DECODER {
        if (FIELD_VALUE (display_brightness) >= -INT32_MAX && FIELD_VALUE (display_brightness) < INT32_MAX)
          FIELD_VALUE (display_brightness_bl) = (BITCODE_BLd)FIELD_VALUE (display_brightness);
      }
      FIELD_BL (display_shadow_type, 173);      FIELD_BS (display_shadow_type_int, 176);
    } /* DXF 2010 */

    SINCE (R_2013) {
      DXF {
        if (!_obj->num_props) _obj->num_props = 58;
        FIELD_BS (num_props, 70);
        FIELD_BL (face_lighting_model, 90);       FIELD_BS (face_lighting_model_int, 176);   /* 0 */
        FIELD_BL (face_lighting_quality, 90);     FIELD_BS (face_lighting_quality_int, 176); /* 1 */
        FIELD_BL (face_color_mode, 90);           FIELD_BS (face_color_mode_int, 176);       /* 2 */
        FIELD_CAST (face_modifier, BS, BL, 90);   FIELD_BS (face_modifier_int, 176);
        FIELD_BD (face_opacity, 40);              FIELD_BS (face_opacity_int, 176);
        FIELD_BD (face_specular, 40);             FIELD_BS (face_specular_int, 176);
        FIELD_CMC (face_mono_color, 62);          FIELD_BS (face_mono_color_int, 176);
        FIELD_BL (edge_model, 90);                FIELD_BS (edge_model_int, 176);
        FIELD_BL (edge_style, 90);                FIELD_BS (edge_style_int, 176);
        FIELD_CMC (edge_intersection_color, 62);  FIELD_BS (edge_intersection_color_int, 176);
        FIELD_CMC (edge_obscured_color, 62);      FIELD_BS (edge_obscured_color_int, 176);
        FIELD_BL (edge_obscured_ltype, 90);       FIELD_BS (edge_obscured_ltype_int, 176);
        FIELD_BL (edge_intersection_ltype, 90);   FIELD_BS (edge_intersection_ltype_int, 176);
        FIELD_BD (edge_crease_angle, 40);         FIELD_BS (edge_crease_angle_int, 176);
        FIELD_BL (edge_modifier, 90);             FIELD_BS (edge_modifier_int, 176);
        FIELD_CMC (edge_color, 62);               FIELD_BS (edge_color_int, 176);
        FIELD_BD (edge_opacity, 40);              FIELD_BS (edge_opacity_int, 176);
        FIELD_BL (edge_width, 90);                FIELD_BS (edge_width_int, 176);
        FIELD_BL (edge_overhang, 90);             FIELD_BS (edge_overhang_int, 176);
        FIELD_BL (edge_jitter, 90);               FIELD_BS (edge_jitter_int, 176);
        FIELD_CMC (edge_silhouette_color, 62);    FIELD_BS (edge_silhouette_color_int, 176);
        FIELD_BL (edge_silhouette_width, 90);     FIELD_BS (edge_silhouette_width_int, 176);
        FIELD_BL (edge_halo_gap, 90);             FIELD_BS (edge_halo_gap_int, 176);
        FIELD_BL (edge_isolines, 90);             FIELD_BS (edge_isolines_int, 176);
        FIELD_B (edge_do_hide_precision, 290);    FIELD_BS (edge_do_hide_precision_int, 176);
        FIELD_BL (display_settings, 90);          FIELD_BS (display_settings_int, 176);
        FIELD_BD (display_brightness, 40);        FIELD_BS (display_brightness_int, 176);
        FIELD_BL (display_shadow_type, 90);       FIELD_BS (display_shadow_type_int, 176);
      }

      DECODER { FIELD_VALUE (num_props) = 58; }
      // 290 290 290 290 290 290 290 290 290 90 40 40
      // 90 62 90 90 62 290 90 90 90 290 90 62
      // 40 90 1 290 40 40
      FIELD_B (b_prop1c, 290);                FIELD_BS (b_prop1c_int, 176);
      FIELD_B (b_prop1d, 290);                FIELD_BS (b_prop1d_int, 176);
      FIELD_B (b_prop1e, 290);                FIELD_BS (b_prop1e_int, 176);
      FIELD_B (b_prop1f, 290);                FIELD_BS (b_prop1f_int, 0);
      FIELD_B (b_prop20, 290);                FIELD_BS (b_prop20_int, 176);
      FIELD_B (b_prop21, 290);                FIELD_BS (b_prop21_int, 0);
      FIELD_B (b_prop22, 290);                FIELD_BS (b_prop22_int, 176);
      FIELD_B (b_prop23, 290);                FIELD_BS (b_prop23_int, 176);
      FIELD_B (b_prop24, 290);                FIELD_BS (b_prop24_int, 176);
      FIELD_BL (bl_prop25, 90);               FIELD_BS (bl_prop25_int, 176);
      FIELD_BD (bd_prop26, 40);               FIELD_BS (bd_prop26_int, 176);
      FIELD_BD (bd_prop27, 40);               FIELD_BS (bd_prop27_int, 176);

      FIELD_BL (bl_prop28, 90);               FIELD_BS (bl_prop28_int, 176);
      FIELD_CMC (c_prop29, 62);               FIELD_BS (c_prop29_int, 176);
      FIELD_BL (bl_prop2a, 90);               FIELD_BS (bl_prop2a_int, 176);
      FIELD_BL (bl_prop2b, 90);               FIELD_BS (bl_prop2b_int, 176);
      FIELD_CMC (c_prop2c, 62);               FIELD_BS (c_prop2c_int, 176);
      FIELD_B (b_prop2d, 290);                FIELD_BS (b_prop2d_int, 176);
      FIELD_BL (bl_prop2e, 90);               FIELD_BS (bl_prop2e_int, 176);
      FIELD_BL (bl_prop2f, 90);               FIELD_BS (bl_prop2f_int, 176);
      FIELD_BL (bl_prop30, 90);               FIELD_BS (bl_prop30_int, 176);
      FIELD_B (b_prop31, 290);                FIELD_BS (b_prop31_int, 176);
      FIELD_BL (bl_prop32, 90);               FIELD_BS (bl_prop32_int, 176);
      FIELD_CMC (c_prop33, 62);               FIELD_BS (c_prop33_int, 176);

      FIELD_BD (bd_prop34, 40);               FIELD_BS (bd_prop34_int, 176);
      FIELD_BL (edge_wiggle, 90);             FIELD_BS (edge_wiggle_int, 176);  // prop 0x35
      FIELD_T (strokes, 1);                   FIELD_BS (strokes_int, 176);      // prop 0x36
      FIELD_B (b_prop37, 290);                FIELD_BS (b_prop37_int, 176);
      FIELD_BD (bd_prop38, 40);               FIELD_BS (bd_prop38_int, 176);
      FIELD_BD (bd_prop39, 40);               FIELD_BS (bd_prop39_int, 176);
    }
  }
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

/* LIGHT: SpotLight, PointLight, DistantLight. dbLight.h
 */
DWG_ENTITY (LIGHT)

  SUBCLASS (AcDbLight);
  FIELD_BL (class_version, 90);
  VALUEOUTOFBOUNDS (class_version, 10)
  FIELD_T (name, 1);
  FIELD_BL (type, 70);
  LOG_LIGHT_TYPE
  FIELD_B (status, 290);
#ifdef IS_DXF
  UNTIL (R_2000) {
    FIELD_BL (light_color.rgb, 90);
  } else {
    FIELD_CMC (light_color, 63);
  }
#else
  FIELD_CMC (light_color, 63);
#endif
  FIELD_B (plot_glyph, 291); /* if it's plottable */
  FIELD_BD (intensity, 40);
  FIELD_3BD (position, 10);
  FIELD_3BD (target, 11);
  FIELD_BL (attenuation_type, 72);
  LOG_LIGHT_ATTENUATION_TYPE
  FIELD_B (use_attenuation_limits, 292);
  FIELD_BD (attenuation_start_limit, 41);
  FIELD_BD (attenuation_end_limit, 42);
  FIELD_BD (hotspot_angle, 50);
  FIELD_BD (falloff_angle, 51);
  FIELD_B (cast_shadows, 293);
  FIELD_BL (shadow_type, 73);
  FIELD_BS (shadow_map_size, 91);
  FIELD_RCd (shadow_map_softness, 280);

  DECODER {
    // LIGHTINGUNITS is a member of the AcDbVariableDictionary
    // NOD => DICTIONARY => DICTIONARYVAR
    // may not be cached
    char *value = dwg_variable_dict (dwg, "LIGHTINGUNITS");
    LOG_TRACE ("vardict.LIGHTINGUNITS: %s\n", value);
    if (value && strEQ (value, "2")) /* PHOTOMETRIC */
      FIELD_VALUE (is_photometric) = 1;
  }
  LOG_TRACE ("is_photometric: %d\n", FIELD_VALUE (is_photometric));
  if (FIELD_VALUE (is_photometric))
  {
    FIELD_B (has_photometric_data, 1);
    // IES light model
    if (FIELD_VALUE (has_photometric_data))
      {
        DXF { VALUE_B (0, 295); }
        FIELD_B (has_webfile, 290);
        FIELD_T (webfile, 300);
        FIELD_BS (physical_intensity_method, 70);
        FIELD_BD (physical_intensity, 40);
        FIELD_BD (illuminance_dist, 41);
        FIELD_BS (lamp_color_type, 71); //0: temp. in kelvin, 1: as preset
        FIELD_BD (lamp_color_temp, 42);
        FIELD_BS (lamp_color_preset, 72);
        FIELD_3BD_1 (web_rotation, 43);
        // ExtendedLigthShape
        FIELD_BS (extlight_shape, 73);
        LOG_LIGHT_EXTLIGHT_SHAPE
        FIELD_BD (extlight_length, 46);
        FIELD_BD (extlight_width, 47);
        FIELD_BD (extlight_radius, 48);
        FIELD_BS (webfile_type, 74);
        FIELD_BS (web_symetry, 75);
        FIELD_BS (has_target_grip, 76); //bool
        FIELD_BD (web_flux, 49);
        FIELD_BD (web_angle1, 50);
        FIELD_BD (web_angle2, 51);
        FIELD_BD (web_angle3, 52);
        FIELD_BD (web_angle4, 53);
        FIELD_BD (web_angle5, 54);
        FIELD_BS (glyph_display_type, 77);
      }
  }
  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

// (varies)
// ENHANCEDBLOCK => AcDbDynamicBlockRoundTripPurgePreventer
// DXF: ACDB_DYNAMICBLOCKPURGEPREVENTER_VERSION
// same struct as BLOCKREPRESENTATION
DWG_OBJECT (DYNAMICBLOCKPURGEPREVENTER)
  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbDynamicBlockPurgePreventer)
  FIELD_BS (flag, 70); //1 class_version would be 90
  START_OBJECT_HANDLE_STREAM;
  FIELD_HANDLE (block, 5, 0)
DWG_OBJECT_END

// ACDB_BLOCKREPRESENTATION_DATA
// same struct as above
DWG_OBJECT (BLOCKREPRESENTATION)
  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbBlockRepresentationData)
  FIELD_BS (flag, 70);
  START_OBJECT_HANDLE_STREAM;
  FIELD_HANDLE (block, 3, 340);
DWG_OBJECT_END

// UNSTABLE
DWG_OBJECT (DBCOLOR)
  SUBCLASS (AcDbColor)
  FIELD_CMC (color, 62);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// (varies) UNSTABLE
// dbhelix.h
DWG_ENTITY (HELIX)

  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbSpline)
  FIELD_BL (scenario, 0);
  LOG_SPLINE_SCENARIO
  UNTIL (R_2013) {
    if (FIELD_VALUE (scenario) != 1 && FIELD_VALUE (scenario) != 2)
      LOG_ERROR ("unknown scenario %d", FIELD_VALUE (scenario));
    DECODER {
      if (FIELD_VALUE (scenario) == 1)
        FIELD_VALUE (splineflags) = 8;
      else if (FIELD_VALUE (scenario) == 2)
        FIELD_VALUE (splineflags) = 9;
    }
  }
  SINCE (R_2013) {
    FIELD_BL (splineflags, 0);
    LOG_SPLINE_SPLINEFLAGS
    FIELD_BL (knotparam, 0);
    LOG_SPLINE_KNOTPARAM
    if (FIELD_VALUE (splineflags) & 1)
      FIELD_VALUE (scenario) = 2;
    if (FIELD_VALUE (knotparam) == 15)
      FIELD_VALUE (scenario) = 1;
  }

  // extrusion on planar
  DXF { VALUE_RD (0.0, 210); VALUE_RD (0.0, 220); VALUE_RD (1.0, 230);
        FIELD_BL (flag, 70);
      }
  FIELD_BL (degree, 71);

  if (FIELD_VALUE (scenario) & 1) { // spline
    FIELD_B (rational, 0); // flag bit 2
    FIELD_B (closed_b, 0); // flag bit 0
    FIELD_B (periodic, 0); // flag bit 1
    FIELD_BD (knot_tol, 42); // def: 0.0000001
    FIELD_BD (ctrl_tol, 43); // def: 0.0000001
    FIELD_BL (num_knots, 72);
    FIELD_BL (num_ctrl_pts, 73);
    FIELD_B (weighted, 0);

    DECODER {
      // not 32
      FIELD_VALUE (flag) = 8 +          /* planar */
        FIELD_VALUE (closed_b) +        /* 1 */
        (FIELD_VALUE (periodic) << 1) + /* 2 */
        (FIELD_VALUE (rational) << 2) + /* 4 */
        (FIELD_VALUE (weighted) << 4);  /* 16 */
        // ignore method fit points and closed bits
        /*((FIELD_VALUE (splineflags) & ~5) << 7)*/
      LOG_TRACE ("=> flag: %d [70]\n", FIELD_VALUE (flag));
    }
    FIELD_VECTOR (knots, BD, num_knots, 40);
    REPEAT (num_ctrl_pts, ctrl_pts, Dwg_SPLINE_control_point)
    REPEAT_BLOCK
        SUB_FIELD_3BD_inl (ctrl_pts[rcount1], xyz, 10);
        if (!FIELD_VALUE (weighted))
          FIELD_VALUE (ctrl_pts[rcount1].w) = 0; // skipped when encoding
        else
          SUB_FIELD_BD (ctrl_pts[rcount1], w, 41);
        SET_PARENT (ctrl_pts[rcount1], (Dwg_Entity_SPLINE*)_obj);
    END_REPEAT_BLOCK
    END_REPEAT (ctrl_pts);
  }
  else { // bezier spline, scenario 2
    DECODER {
      // flag 32 in DXF
      FIELD_VALUE (flag) = 8 + 32 + // planar, not rational
        // ignore method fit points and closed bits
        ((FIELD_VALUE (splineflags) & ~5) << 7);
      LOG_TRACE ("=> flag: %d [70]\n", FIELD_VALUE (flag));
    }
    FIELD_BD (fit_tol, 44); // def: 0.0000001
    FIELD_3BD (beg_tan_vec, 12);
    FIELD_3BD (end_tan_vec, 13);
    FIELD_BL (num_fit_pts, 74);
    FIELD_3DPOINT_VECTOR (fit_pts, num_fit_pts, 11);
  }

  SUBCLASS (AcDbHelix)
  FIELD_BL (major_version, 90);
  FIELD_BL (maint_version, 91);
  FIELD_3BD (axis_base_pt, 10);
  FIELD_3BD (start_pt, 11);
  FIELD_3BD (axis_vector, 12);
  FIELD_BD (radius, 40);
  FIELD_BD (turns, 41);
  FIELD_BD (turn_height, 42);
  FIELD_B (handedness, 290);
  FIELD_RC (constraint_type, 280);

  COMMON_ENTITY_HANDLE_DATA;
DWG_ENTITY_END

#define AcDbAssocAnnotationActionBody_fields      \
  UNTIL (R_2010) {                                \
    if (FIELD_VALUE (actionbody))                 \
    {                                             \
      AcDbAssocActionBody_fields;                 \
      AcDbAssocParamBasedActionBody_fields (pab); \
    }                                             \
  }                                               \
  LATER_VERSIONS {                                \
    FIELD_BS (aaab_version, 90);                  \
    FIELD_HANDLE (assoc_dep, 5, 330);             \
  }

// undocumented fields, unstable, but looks stable.
// types: Sphere|Cylinder|Cone|Torus|Box|Wedge|Pyramid
DWG_ENTITY (MESH)
  SUBCLASS (AcDbSubDMesh)
  FIELD_BS (dlevel, 71);       // version, 2
  FIELD_B (is_watertight, 72); // blend_crease 0
  FIELD_BL (num_subdiv_vertex, 91); // subdivision_levels 0
  FIELD_3DPOINT_VECTOR (subdiv_vertex, num_subdiv_vertex, 10);
  FIELD_BL (num_vertex, 92); //14 @14
  FIELD_3DPOINT_VECTOR (vertex, num_vertex, 10);
  FIELD_BL (num_faces, 93); // 30
  FIELD_VECTOR (faces, BL, num_faces, 90);

  FIELD_BL (num_edges, 94); // 19
  REPEAT (num_edges, edges, Dwg_MESH_edge)
  REPEAT_BLOCK
      SUB_FIELD_BL (edges[rcount1], idxfrom, 90);
      SUB_FIELD_BL (edges[rcount1], idxto, 90);
      SET_PARENT_OBJ (edges[rcount1]);
  END_REPEAT_BLOCK
  END_REPEAT (edges);
  //FIELD_VECTOR (edges, Dwg_MESH_edge, num_edges * 2, 90);
  FIELD_BL (num_crease, 95); // edge creases 19
  FIELD_VECTOR (crease, BD, num_crease, 140);
  COMMON_ENTITY_HANDLE_DATA;
DWG_ENTITY_END

DWG_OBJECT (LIGHTLIST)
  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbLightList)
  FIELD_BL (class_version, 90);
  FIELD_BL (num_lights, 90);
  REPEAT (num_lights, lights, Dwg_LIGHTLIST_light)
  REPEAT_BLOCK
      SUB_FIELD_HANDLE (lights[rcount1],handle, 5, 5)
      SUB_FIELD_T (lights[rcount1],name, 1)
  END_REPEAT_BLOCK
  END_REPEAT (lights)
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// hard-owned child of AcDbViewportTableRecord or AcDbViewport 361
// DXF docs put that as Entity, wrong!
// Missing: 421
DWG_OBJECT (SUN)
  SUBCLASS (AcDbSun)
  FIELD_BL (class_version, 90);
  VALUEOUTOFBOUNDS (class_version, 10)
  FIELD_B (is_on, 290);       // status, isOn
  FIELD_CMC (color, 63);
  FIELD_BD (intensity, 40);   //
  FIELD_B (has_shadow, 291);  // shadow on/off
  FIELD_BL (julian_day, 91);
  FIELD_BL (msecs, 92);
  FIELD_B (is_dst, 292);      // isDayLightSavingsOn
  FIELD_BL (shadow_type, 70); // 0 raytraced, 1 shadow maps
  FIELD_BS (shadow_mapsize, 71); // max 3968
  FIELD_RCd (shadow_softness, 280);

  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

#define AcDbRenderSettings_fields                                             \
    SUBCLASS (AcDbRenderSettings)                                             \
    VERSION (R_2013) { /* version 0x1f */                                     \
      VALUE_BL (_obj->class_version + 1, 90)                                  \
    } else {                                                                  \
      FIELD_BL (class_version, 90);                                           \
    }                                                                         \
    FIELD_T (name, 1);                                                        \
    FIELD_B (fog_enabled, 290);                                               \
    FIELD_B (fog_background_enabled, 290);                                    \
    FIELD_B (backfaces_enabled, 290);                                         \
    FIELD_B (environ_image_enabled, 290);                                     \
    FIELD_T (environ_image_filename, 1);                                      \
    FIELD_T (description, 1);                                                 \
    FIELD_BL (display_index, 90);                                             \
    VERSION (R_2013) {                                                        \
      FIELD_B (has_predefined, 290);                                          \
    }

DWG_OBJECT (RENDERSETTINGS)
  AcDbRenderSettings_fields;
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (MENTALRAYRENDERSETTINGS)
  AcDbRenderSettings_fields;
  SUBCLASS (AcDbMentalRayRenderSettings);
  FIELD_BL (mr_version, 90); /* = 2 */
  FIELD_BL (sampling1, 90);
  FIELD_BL (sampling2, 90);
  FIELD_BS (sampling_mr_filter, 70);
  FIELD_BD (sampling_filter1, 40);
  FIELD_BD (sampling_filter2, 40);
  FIELD_BD (sampling_contrast_color1, 40);
  FIELD_BD (sampling_contrast_color2, 40);
  FIELD_BD (sampling_contrast_color3, 40);
  FIELD_BD (sampling_contrast_color4, 40);
  FIELD_BS (shadow_mode, 70);
  FIELD_B  (shadow_maps_enabled, 290);
  FIELD_B  (ray_tracing_enabled, 290);
  FIELD_BL (ray_trace_depth1, 90);
  FIELD_BL (ray_trace_depth2, 90);
  FIELD_BL (ray_trace_depth3, 90);
  FIELD_B  (global_illumination_enabled, 290);
  FIELD_BL (gi_sample_count, 90);
  FIELD_B  (gi_sample_radius_enabled, 290);
  FIELD_BD (gi_sample_radius, 40);
  FIELD_BL (gi_photons_per_light, 90);
  FIELD_BL (photon_trace_depth1, 90);
  FIELD_BL (photon_trace_depth2, 90);
  FIELD_BL (photon_trace_depth3, 90);
  FIELD_B  (final_gathering_enabled, 290);
  FIELD_BL (fg_ray_count, 90);
  FIELD_B  (fg_sample_radius_state1, 290);
  FIELD_B  (fg_sample_radius_state2, 290);
  FIELD_B  (fg_sample_radius_state3, 290);
  FIELD_BD (fg_sample_radius1, 40);
  FIELD_BD (fg_sample_radius2, 40);
  FIELD_BD (light_luminance_scale, 40);
  FIELD_BS (diagnostics_mode, 70);
  FIELD_BS (diagnostics_grid_mode, 70);
  FIELD_BD (diagnostics_grid_float, 40);
  FIELD_BS (diagnostics_photon_mode, 70);
  FIELD_BS (diagnostics_bsp_mode, 70);
  FIELD_B  (export_mi_enabled, 290);
  FIELD_T  (mr_description, 1);
  FIELD_BL (tile_size, 90);
  FIELD_BS (tile_order, 70);
  FIELD_BL (memory_limit, 90);
  FIELD_B  (diagnostics_samples_mode, 290);
  FIELD_BD (energy_multiplier, 40);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (RAPIDRTRENDERSETTINGS)
  AcDbRenderSettings_fields;
  SUBCLASS (AcDbRapidRTRenderSettings)
  FIELD_BL (rapidrt_version, 90);
  FIELD_BL (render_target, 70);
  FIELD_BL (render_level, 90);
  FIELD_BL (render_time, 90);
  FIELD_BL (lighting_model, 70);
  FIELD_BL (filter_type, 70);
  FIELD_BD (filter_width, 40);
  FIELD_BD (filter_height, 40);
  VERSION (R_2013)
    {}
  else
    FIELD_B (has_predefined, 290); // when RENDERSETTINGS does not handle it
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

/* MATERIAL classes */

// each color writes RC flag, BD factor, BL rgb if flag=1
#define MAT_COLOR(color, dxf1, dxf2, dxf3)                                    \
  {                                                                           \
    FIELD_RC (color.flag, dxf1);     /* 0 Use current color, 1 Override */    \
    FIELD_BD (color.factor, dxf2);   /* 0.0 - 1.0 */                          \
    if (_obj->color.flag == 1)                                                \
      {                                                                       \
        FIELD_BLx (color.rgb, dxf3);                                          \
      }                                                                       \
  }

// We need to declare it first, because it's recursive. Only here
DWG_SUBCLASS_DECL (MATERIAL, Texture_diffusemap);

/* if source == 2 */
#define MAT_TEXTURE(map, value)                                               \
  {                                                                           \
    FIELD_BS (map.texturemode, 277);                                          \
    if (FIELD_VALUE (map.texturemode) == 0)                                   \
      {                                                                       \
        /* woodtexture */                                                     \
        MAT_COLOR (map.color1, 278, 460, 95);                                 \
        MAT_COLOR (map.color2, 279, 461, 96);                                 \
      }                                                                       \
    else if (FIELD_VALUE (map.texturemode) == 1)                              \
      {                                                                       \
        /* marbletexture */                                                   \
        MAT_COLOR (map.color1, 280, 465, 97);                                 \
        MAT_COLOR (map.color2, 281, 466, 98);                                 \
      }                                                                       \
    else if (FIELD_VALUE (map.texturemode) == 2)                              \
      {                                                                       \
        /* generic texture variant */                                         \
        FIELD_BS (genproctype, 0);                                            \
        switch (_obj->genproctype) {                                          \
        case 1:                                                               \
          FIELD_B (genprocvalbool, 291); break;                               \
        case 2:                                                               \
          FIELD_BS (genprocvalint, 271); break;                               \
        case 3:                                                               \
          FIELD_BD (genprocvalreal, 469); break;                              \
        case 4:                                                               \
          FIELD_CMC (genprocvalcolor, 62); break;                             \
        case 5:                                                               \
          FIELD_T (genprocvaltext, 301); break;                               \
        case 6:                                                               \
          FIELD_BS (num_gentextures, 0);                                      \
          REPEAT (num_gentextures, gentextures, Dwg_MATERIAL_gentexture)      \
          REPEAT_BLOCK                                                        \
            _obj->gentextures[rcount1].material = _obj;                       \
            SUB_FIELD_T (gentextures[rcount1], genprocname, 300);             \
            LOG_WARN ("recursive MATERIAL.gentextures")                       \
            CALL_SUBCLASS (_obj->gentextures[rcount1].material, MATERIAL,     \
                           Texture_diffusemap);                               \
            SET_PARENT_OBJ (gentextures[rcount1]);                            \
          END_REPEAT_BLOCK                                                    \
          END_REPEAT (gentextures)                                            \
          FIELD_B (genproctableend, 292);                                     \
        default:                                                              \
          break;                                                              \
         }                                                                    \
      }                                                                       \
  }

#define MAT_MAPPER(map, dxf4, dxf5, dxf6, dxf7)                               \
  {                                                                           \
    /* 0 Inherit, 1 Planar (def), 2 Box, 3 Cylinder, 4 Sphere */              \
    FIELD_RC (map.projection, dxf4);                                          \
    /* 0 Inherit, 1 Tile (def), 2 Crop, 3 Clamp, 4 Mirror */                  \
    FIELD_RC (map.tiling, dxf5);                                              \
    /* 1 no, 2: scale to curr ent, 4: w/ current block transform */           \
    FIELD_RC (map.autotransform, dxf6);                                       \
    FIELD_VECTOR_N (map.transmatrix, BD, 16, dxf7);                           \
  }

#define MAT_MAP(map, dxf1, dxf2, dxf3, dxf4, dxf5, dxf6, dxf7)                \
  FIELD_BD (map.blendfactor, dxf1);                                           \
  MAT_MAPPER (map, dxf4, dxf5, dxf6, dxf7);                                   \
  FIELD_RC (map.source, dxf2); /* 0 scene, 1 file (def), 2 procedural */      \
  if (FIELD_VALUE (map.source) == 1)                                          \
    {                                                                         \
      FIELD_T (map.filename, dxf3); /* if NULL no map */                      \
    }                                                                         \
  else if (FIELD_VALUE (map.source) == 2)                                     \
    MAT_TEXTURE (map, 0)

#define Texture_diffusemap_fields MAT_TEXTURE (diffusemap, 0)
DWG_SUBCLASS (MATERIAL, Texture_diffusemap);

// (varies)
DWG_OBJECT (MATERIAL)
  //DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbMaterial)
  FIELD_T (name, 1);
  FIELD_T (description, 2);
  MAT_COLOR (ambient_color, 70, 40, 90);
  MAT_COLOR (diffuse_color, 71, 41, 91);

  MAT_MAP (diffusemap, 42, 72, 3, 73, 74, 75, 43);
  DXF {
    SINCE (R_2007)
      CALL_SUBCLASS (_obj, MATERIAL, Texture_diffusemap);
      /* MAT_TEXTURE (diffusemap, 0) */
    //DXF { VALUE_B (1, 292); } /* genproctableend  */
    //DXF { VALUE_BS (value, 277); } /* ?? */
  }
  MAT_COLOR (specular_color, 76, 45, 92);
  DXF { FIELD_BD (specular_gloss_factor, 44); }
  MAT_MAP (specularmap, 46, 77, 4, 78, 79, 170, 47);
  FIELD_BD (specular_gloss_factor, 0); // def: 0.5
  MAT_MAP (reflectionmap, 48, 171, 6, 172, 173, 174, 49);
  FIELD_BD (opacity_percent, 140);      // def: 1.0
  MAT_MAP (opacitymap, 141, 175, 7, 176, 177, 178, 142);
  MAT_MAP (bumpmap, 143, 179, 8, 270, 271, 272, 144);
  FIELD_BD (refraction_index, 145);     // def: 1.0
  MAT_MAP (refractionmap, 146, 273, 9, 274, 275, 276, 147);

  SINCE (R_2007) {
    // no DXF if 0
    FIELD_BD0 (translucence, 148);
    FIELD_BD0 (self_illumination, 149);
    FIELD_BD0 (reflectivity, 468);
    FIELD_BL0 (illumination_model, 93);
    FIELD_BL0 (channel_flags, 94);
    FIELD_BL0 (mode, 282);
  }

#if 0
  // missing:
  FIELD_BD (indirect_bump_scale, 461);
  FIELD_BD (reflectance_scale, 462);
  FIELD_BD (transmittance_scale, 463);
  FIELD_B (two_sided_material, 290);
  FIELD_BD (luminance, 464);
  FIELD_BS (luminance_mode, 270);
  FIELD_BS (normalmap_method, 271);
  FIELD_BD (normalmap_strength, 465); //def: 1.0
  MAT_MAP (normalmap, 42, 72, 3, 73, 74, 75, 43);
  FIELD_B (is_anonymous, 293);
  FIELD_BS (global_illumination, 272); // 0 none, 1 cast, 2 receive, 3 cast&receive
  FIELD_BS (final_gather, 273);        // 0 none, 1 cast, 2 receive, 3 cast&receive
  FIELD_BD (color_bleed_scale, 460);

  // saveas: ADVMATERIAL into xdic
  //FIELD_B (genproctableend, 292); // always 1
  //78
  //172
  //176
  //270
  //274
#endif
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_ENTITY (ARC_DIMENSION)

  DECODE_UNKNOWN_BITS
  COMMON_ENTITY_DIMENSION
  SUBCLASS (AcDbArcDimension)
  FIELD_3BD (def_pt, 0);
  FIELD_3BD (xline1_pt, 13);
  FIELD_3BD (xline2_pt, 14);
  FIELD_3BD (center_pt, 15);
  FIELD_B (is_partial, 70);
  FIELD_BD (arc_start_param, 41);
  FIELD_BD (arc_end_param, 42);
  FIELD_B (has_leader, 71);
  FIELD_3BD (leader1_pt, 16);
  FIELD_3BD (leader2_pt, 17);

  COMMON_ENTITY_HANDLE_DATA;
  FIELD_HANDLE (dimstyle, 5, 0);
  FIELD_HANDLE (block, 5, 2);
DWG_ENTITY_END

// as ACAD_LAYERFILTERS in the NOD
DWG_OBJECT (LAYERFILTER)
  SUBCLASS (AcDbLayerFilter)
  FIELD_BL (num_names, 0);
  FIELD_VECTOR_T (names, T, num_names, 8);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// abstract subclass. requires evalexpr
#define AcDbEvalExpr_fields                                                   \
  SUBCLASS (AcDbEvalExpr)                                                     \
  DXF { FIELD_BL (evalexpr.nodeid, 90); }                                     \
  FIELD_BLd (evalexpr.parentid, 0);                                           \
  DXF {                                                                       \
    VALUE_BL (33, 98);                                                        \
    VALUE_BL (29, 99);                                                        \
  } else {                                                                    \
    FIELD_BL (evalexpr.major, 98);                                            \
    FIELD_BL (evalexpr.minor, 99);                                            \
  }                                                                           \
  if (IF_IS_DXF && FIELD_VALUE (evalexpr.value_code) == -9999)                \
    {                                                                         \
      ; /* 70 -9999 not in DXF */                                             \
    }                                                                         \
  else                                                                        \
    {                                                                         \
      DXF { VALUE_TFF ("", 1); }                                              \
      FIELD_BSd (evalexpr.value_code, 70);                                    \
      /* TODO not a union yet */                                              \
      switch (_obj->evalexpr.value_code)                                      \
        {                                                                     \
        case 40:                                                              \
          FIELD_BD (evalexpr.value.num40, 40);                                \
          break;                                                              \
        case 10:                                                              \
          FIELD_2RD (evalexpr.value.pt2d, 10);                                \
          break;                                                              \
        case 11:                                                              \
          FIELD_2RD (evalexpr.value.pt3d, 11);                                \
          break;                                                              \
        case 1:                                                               \
          FIELD_T (evalexpr.value.text1, 1);                                  \
          break;                                                              \
        case 90:                                                              \
          FIELD_BL (evalexpr.value.long90, 90);                               \
          break;                                                              \
        case 91:                                                              \
          FIELD_HANDLE (evalexpr.value.handle91, 5, 91);                      \
          break;                                                              \
        case 70:                                                              \
          FIELD_BS (evalexpr.value.short70, 70);                              \
          break;                                                              \
        case -9999:                                                           \
        default:                                                              \
          break;                                                              \
        }                                                                     \
    }                                                                         \
  FIELD_BL (evalexpr.nodeid, 0)

#define AcDbShHistoryNode_fields(history_node)                          \
  SUBCLASS (AcDbShHistoryNode);                                         \
  FIELD_BL (history_node.major, 90);                                    \
  FIELD_BL (history_node.minor, 91);                                    \
  FIELD_VECTOR_N1 (history_node.trans, BD, 16, 40);                     \
  FIELD_CMC (history_node.color, 62);                                   \
  FIELD_BL (history_node.step_id, 92);                                  \
  FIELD_HANDLE (history_node.material, 5, 347)

// Stable
// same as Wedge
DWG_OBJECT (ACSH_BOX_CLASS)
  //DECODE_UNKNOWN_BITS
  AcDbEvalExpr_fields;
  AcDbShHistoryNode_fields (history_node);
  SUBCLASS (AcDbShPrimitive)
  SUBCLASS (AcDbShBox)
  FIELD_BL (major, 90); //33
  FIELD_BL (minor, 91); //29
  FIELD_BD (length, 40);
  FIELD_BD (width, 41);
  FIELD_BD (height, 42);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// Stable
DWG_OBJECT (ACSH_WEDGE_CLASS)
  //DECODE_UNKNOWN_BITS
  AcDbEvalExpr_fields;
  AcDbShHistoryNode_fields (history_node);
  SUBCLASS (AcDbShPrimitive)
  SUBCLASS (AcDbShWedge)
  FIELD_BL (major, 90); //33
  FIELD_BL (minor, 91); //29
  FIELD_BD (length, 40);
  FIELD_BD (width, 41);
  FIELD_BD (height, 42);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// Stable
DWG_OBJECT (ACSH_SPHERE_CLASS)
  AcDbEvalExpr_fields;
  AcDbShHistoryNode_fields (history_node);
  SUBCLASS (AcDbShPrimitive)
  SUBCLASS (AcDbShSpere)
  FIELD_BL (major, 90); //33
  FIELD_BL (minor, 91); //29
  FIELD_BD (radius, 40);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// Stable
DWG_OBJECT (ACSH_CYLINDER_CLASS)
  AcDbEvalExpr_fields;
  AcDbShHistoryNode_fields (history_node);
  SUBCLASS (AcDbShPrimitive)
  SUBCLASS (AcDbShCylinder)
  FIELD_BL (major, 90);
  FIELD_BL (minor, 91);
  FIELD_BD (height, 40);
  FIELD_BD (major_radius, 41);
  FIELD_BD (minor_radius, 42);
  FIELD_BD (x_radius, 43);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// Unstable
DWG_OBJECT (ACSH_CONE_CLASS)
  AcDbEvalExpr_fields;
  AcDbShHistoryNode_fields (history_node);
  SUBCLASS (AcDbShPrimitive)
  SUBCLASS (AcDbShCone)
  FIELD_BL (major, 90);
  FIELD_BL (minor, 91);
  /* same as Cylinder */
  FIELD_BD (height, 40);
  FIELD_BD (major_radius, 41);
  FIELD_BD (minor_radius, 42);
  FIELD_BD (x_radius, 43);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END


DWG_OBJECT (ACSH_PYRAMID_CLASS)
  AcDbEvalExpr_fields;
  AcDbShHistoryNode_fields (history_node);
  SUBCLASS (AcDbShPrimitive)
  SUBCLASS (AcDbShPyramid)
  FIELD_BL (major, 90); //33
  FIELD_BL (minor, 91); //29
  FIELD_BD (height, 40);
  FIELD_BL (sides, 92);
  FIELD_BD (radius, 41);
  FIELD_BD (topradius, 42);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (ACSH_FILLET_CLASS)
  DECODE_UNKNOWN_BITS
  AcDbEvalExpr_fields;
  AcDbShHistoryNode_fields (history_node);
  SUBCLASS (AcDbShFillet)
  FIELD_BL (major, 90); //33
  FIELD_BL (minor, 91); //1 or 29
  FIELD_BL (bl92, 92);
  FIELD_BL (num_edges, 93);
  FIELD_VECTOR (edges, BL, num_edges, 94)
  FIELD_BL (num_radiuses, 95);
  FIELD_VECTOR (radiuses, BD, num_radiuses, 41)
  FIELD_BL (num_startsetbacks, 96);
  FIELD_BL (num_endsetbacks, 97);
  FIELD_VECTOR (endsetbacks, BD, num_endsetbacks, 43)
  FIELD_VECTOR (startsetbacks, BD, num_startsetbacks, 42)
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (ACSH_CHAMFER_CLASS)
  DECODE_UNKNOWN_BITS
  AcDbEvalExpr_fields;
  AcDbShHistoryNode_fields (history_node);
  SUBCLASS (AcDbShChamfer)
  FIELD_BL (major, 90); //33
  FIELD_BL (minor, 91); //1
  FIELD_BL (bl92, 92);
  FIELD_BD (base_dist, 41);
  FIELD_BD (other_dist, 42);
  FIELD_BL (num_edges, 93);
  FIELD_VECTOR (edges, BL, num_edges, 94)
  FIELD_BL (bl95, 95);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (ACSH_TORUS_CLASS)
  AcDbEvalExpr_fields;
  AcDbShHistoryNode_fields (history_node);
  SUBCLASS (AcDbShPrimitive)
  SUBCLASS (AcDbShTorus)
  FIELD_BL (major, 90); //33
  FIELD_BL (minor, 91); //1
  FIELD_BD (major_radius, 40);
  FIELD_BD (minor_radius, 41);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (ACSH_BREP_CLASS)
  DECODE_UNKNOWN_BITS
  AcDbEvalExpr_fields;
  AcDbShHistoryNode_fields (history_node);
  SUBCLASS (AcDbShPrimitive)
  SUBCLASS (AcDbShBrep)
  FIELD_BL (major, 90); // also in DWG?
  FIELD_BL (minor, 91);
  ACTION_3DSOLID;
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (ACSH_BOOLEAN_CLASS)
  AcDbEvalExpr_fields;
  AcDbShHistoryNode_fields (history_node);
  SUBCLASS (AcDbShBoolean)
  FIELD_BL (major, 90);
  FIELD_BL (minor, 91);
  FIELD_RCd (operation, 280);
  FIELD_BL (operand1, 92);
  FIELD_BL (operand2, 93);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (ACSH_HISTORY_CLASS)
  SUBCLASS (AcDbShHistory)
  FIELD_BL (major, 90);
  FIELD_BL (minor, 91);
  FIELD_HANDLE (owner, 2, 360);
  FIELD_BL (h_nodeid, 92);
  FIELD_B (show_history, 280);
  FIELD_B (record_history, 281);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

/* TODO replace by CALL_PRIVATE (ASSOCACTION) */
#define AcDbAssocAction_fields                                                \
  SUBCLASS (AcDbAssocAction)                                                  \
  /* until r2010: 1, 2013+: 2 */                                              \
  FIELD_BS (class_version, 90);                                               \
  /* 0 WellDefined, 1 UnderConstrained, 2 OverConstrained,                    \
     3 Inconsistent, 4 NotEvaluated, 5 NotAvailable,                          \
     6 RejectedByClient */                                                    \
  FIELD_BL (geometry_status, 90);                                             \
  FIELD_HANDLE (owningnetwork, 4, 330);                                       \
  FIELD_HANDLE (actionbody, 3, 360);                                          \
  FIELD_BL (action_index, 90);                                                \
  FIELD_BL (max_assoc_dep_index, 90);                                         \
  FIELD_BL (num_deps, 90);                                                    \
  REPEAT (num_deps, deps, Dwg_ASSOCACTION_Deps)                               \
  REPEAT_BLOCK                                                                \
  {                                                                           \
    int dxf = _obj->deps[rcount1].is_owned ? 360 : 330;                       \
    int code = _obj->deps[rcount1].is_owned ? 3 : 4;                          \
    SUB_FIELD_B (deps[rcount1], is_owned, 0);                                 \
    SUB_FIELD_HANDLE (deps[rcount1], dep, code, dxf);                         \
  }                                                                           \
  END_REPEAT_BLOCK                                                            \
  END_REPEAT (deps);                                                          \
  if (FIELD_VALUE (class_version) > 1)                                        \
    {                                                                         \
      VALUE_BS (0, 90);                                                       \
      FIELD_BL (num_owned_params, 90);                                        \
      HANDLE_VECTOR (owned_params, num_owned_params, 3, 360);                 \
      VALUE_BS (0, 90);                                                       \
      FIELD_BL (num_values, 90);                                              \
      REPEAT (num_values, values, Dwg_VALUEPARAM)                             \
      REPEAT_BLOCK                                                            \
          AcDbValueParam_fields (values[rcount1])                             \
      END_REPEAT_BLOCK                                                        \
      END_REPEAT (values);                                                    \
    }

// (varies) stable
// call as dwg_##action_ASSOCACTION_private
DWG_OBJECT (ASSOCACTION)
  AcDbAssocAction_fields;
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

#define AdDbAssocIndexPersSubentId_fields \
  DXF { FIELD_T (classname, 1) }       \
  else {                               \
    PRE (R_2013) {                     \
      FIELD_T (classname, 0);          \
    } LATER_VERSIONS {                 \
      FIELD_B (has_classname, 0);      \
      FIELD_BL (bl1, 0);               \
    }                                  \
  }                                    \
  FIELD_BS (class_version, 90);        \
  FIELD_BL (subent_type, 90);          \
  FIELD_BL (subent_index, 90);         \
  FIELD_B (dependent_on_compound_object, 290)

#define AcDbAssocPersSubentId_fields            \
  SUBCLASS (AcDbAssocPersSubentId)              \
  FIELD_T (classname, 1);                       \
  FIELD_B (dependent_on_compound_object, 290)

#define AcDbAssocEdgePersSubentId_fields \
  DXF { FIELD_T (classname, 1) }         \
  else {                                 \
    PRE (R_2013) {                       \
      FIELD_T (classname, 0);            \
    } LATER_VERSIONS {                   \
      FIELD_B (has_classname, 0);        \
      FIELD_BL (bl1, 0);                 \
    }                                    \
  }                                      \
  FIELD_BS (class_version, 90);          \
  FIELD_BL (index1, 90);                 \
  FIELD_BL (index2, 90);                 \
  FIELD_B (dependent_on_compound_object, 290)

DWG_OBJECT (ASSOCVALUEDEPENDENCY)
  DECODE_UNKNOWN_BITS
  AcDbAssocDependency_fields;
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (ASSOCGEOMDEPENDENCY)
  AcDbAssocDependency_fields;
  SUBCLASS (AcDbAssocGeomDependency)
  FIELD_BS (class_version, 90); // always 0
  FIELD_B (enabled, 290);       // always 1
  AcDbAssocPersSubentId_fields;
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// (varies) stable
// Subclass of AcDbAssocAction
// Object1 --ReadDep--> Action1 --WriteDep1--> Object2 --ReadDep--> Action2 ...
DWG_OBJECT (ASSOCNETWORK)
  AcDbAssocAction_fields;
  SUBCLASS (AcDbAssocNetwork)
  FIELD_BS (network_version, 90);
  FIELD_BL (network_action_index, 90);
  FIELD_BL (num_actions, 90);
  VALUEOUTOFBOUNDS (num_actions, 100)
  REPEAT (num_actions, actions, Dwg_ASSOCACTION_Deps)
  REPEAT_BLOCK
  {
    int dxf = _obj->actions[rcount1].is_owned ? 360 : 330;
    int code = _obj->actions[rcount1].is_owned ? DWG_HDL_HARDOWN : DWG_HDL_SOFTPTR;
    SUB_FIELD_B (actions[rcount1], is_owned, 0);
    SUB_FIELD_HANDLE (actions[rcount1], dep, code, dxf);
  }
  END_REPEAT_BLOCK
  END_REPEAT (actions);
  FIELD_BL (num_owned_actions, 90);
  HANDLE_VECTOR (owned_actions, num_owned_actions, 4, 330);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

#define AcDbBlockElement_fields                 \
  AcDbEvalExpr_fields;                          \
  SUBCLASS (AcDbBlockElement);                  \
  FIELD_T (name, 300);                          \
  DECODER {                                     \
    FIELD_BL (be_major, 98);                    \
    FIELD_BL (be_minor, 99);                    \
  } else {                                      \
    PRE (R_2007) {                              \
      VALUE_BL (25, 98);                        \
      VALUE_BL (104, 99);                       \
    } LATER_VERSIONS {                          \
      VALUE_BL (33, 98);                        \
      VALUE_BL (29, 99);                        \
    }                                           \
  }                                             \
  FIELD_BL (eed1071, 1071)

#define AcDbBlockGrip_fields                    \
  AcDbBlockElement_fields;                      \
  SUBCLASS (AcDbBlockGrip)                      \
  FIELD_BL (bg_bl91, 91);                       \
  FIELD_BL (bg_bl92, 92);                       \
  FIELD_3BD (bg_location, 1010);                \
  FIELD_B (bg_insert_cycling, 280);             \
  FIELD_BLd (bg_insert_cycling_weight, 93)

#define AcDbBlockParameter_fields               \
  AcDbBlockElement_fields;                      \
  SUBCLASS (AcDbBlockParameter);                \
  FIELD_B (show_properties, 280);               \
  FIELD_B (chain_actions, 281)

#define AcDbBlockAction_fields                                                \
  AcDbBlockElement_fields;                                                    \
  SUBCLASS (AcDbBlockAction)                                                  \
  DXF                                                                         \
  {                                                                           \
    FIELD_BL (num_actions, 70);                                               \
    FIELD_VECTOR (actions, BL, num_actions, 91);                              \
    FIELD_BL (num_deps, 71);                                                  \
    HANDLE_VECTOR (deps, num_deps, 4, 330);                                   \
    FIELD_3BD (display_location, 1010);                                       \
  }                                                                           \
  else                                                                        \
  {                                                                           \
    FIELD_3BD (display_location, 0);                                          \
    FIELD_BL (num_deps, 71);                                                  \
    HANDLE_VECTOR (deps, num_deps, 4, 330);                                   \
    FIELD_BL (num_actions, 70);                                               \
    FIELD_VECTOR (actions, BL, num_actions, 91);                              \
  }

#define AcDbBlockGripExpr_fields                                              \
  SUBCLASS (AcDbBlockGripExpr);                                               \
  FIELD_BL (grip_type, 91); /* ?? */                                          \
  FIELD_T (grip_expr, 300)

#define BlockParam_Connection(conn, dxf1, dxf2)                               \
  FIELD_BL (conn.code, dxf1);                                                 \
  FIELD_T (conn.name, dxf2)

#define BlockAction_ConnectionPt(conn_pt, dxf1, dxf2)                         \
  FIELD_BL (conn_pt.code, dxf1);                                              \
  FIELD_T (conn_pt.name, dxf2)

#define BlockAction_ConnectionPts(conn_pts, start, n, dxf1, dxf2)             \
  DXF {                                                                       \
    for (vcount = start; vcount < (start + n); vcount++) {                    \
      FIELD_BL (conn_pts[vcount].code, dxf1 + vcount);                        \
    }                                                                         \
    for (vcount = start; vcount < (start + n); vcount++) {                    \
      FIELD_T (conn_pts[vcount].name, dxf2 + vcount);                         \
    }                                                                         \
  } else {                                                                    \
    for (vcount = start; vcount < (start + n); vcount++) {                    \
      FIELD_BL (conn_pts[vcount].code, 0);                                    \
      FIELD_T (conn_pts[vcount].name, 0);                                     \
    }                                                                         \
  }

#define AcDbBlockParamValueSet_fields(var, i_code, d_code, s_code, t_code)    \
  DXF { SUB_FIELD_T (var, desc, t_code); }                                    \
  JSON { SUB_FIELD_T (var, desc, t_code); }                                   \
  SUB_FIELD_BL (var, flags, i_code)                                           \
  SUB_FIELD_BD (var, minimum, d_code)                                         \
  SUB_FIELD_BD (var, maximum, d_code + 1)                                     \
  SUB_FIELD_BD (var, increment, d_code + 2)                                   \
  SUB_FIELD_BS (var, num_valuelist, s_code)                                   \
  SUB_FIELD_VECTOR (var, valuelist, BD, num_valuelist, d_code + 3)

#define BlockParam_PropInfo(_prop, num_code, d_code, t_code)         \
  SUB_FIELD_BL (_prop, num_connections, num_code)                    \
  REPEAT2 (_prop.num_connections, _prop.connections, Dwg_BLOCKPARAMETER_connection) \
  REPEAT_BLOCK                                                       \
      SUB_FIELD_BL (_prop.connections[rcount2], code, d_code);       \
      SUB_FIELD_T (_prop.connections[rcount2], name, t_code);        \
  END_REPEAT_BLOCK                                                   \
  END_REPEAT (_prop.connections)

#define AcDbBlock1PtParameter_fields              \
  AcDbBlockParameter_fields;                      \
  SUBCLASS (AcDbBlock1PtParameter);               \
  FIELD_3BD (def_pt, 1010);                       \
  DXF { FIELD_BL (num_propinfos, 93); } /* 2 */   \
  BlockParam_PropInfo (prop1, 170, 91, 301);      \
  BlockParam_PropInfo (prop2, 171, 92, 302);      \
  FIELD_BL (num_propinfos, 0)

#define AcDbBlock2PtParameter_fields              \
  AcDbBlockParameter_fields;                      \
  SUBCLASS (AcDbBlock2PtParameter);               \
  FIELD_3BD (def_basept, 1010);                   \
  FIELD_3BD (def_endpt, 1011);                    \
  DXF {                                           \
    VALUE_BL (4, 170); /* =num_prop_states */     \
    FIELD_VECTOR_N (prop_states, BL, 4, 91);      \
  }                                               \
  BlockParam_PropInfo (prop1, 171, 92, 301);      \
  BlockParam_PropInfo (prop2, 172, 93, 302);      \
  BlockParam_PropInfo (prop3, 173, 94, 303);      \
  BlockParam_PropInfo (prop4, 174, 95, 304);      \
  FIELD_VECTOR_N (prop_states, BL, 4, 0);         \
  FIELD_BS (parameter_base_location, 177);        \
  if (0) {                                        \
    FIELD_3BD (upd_basept, 0);                    \
    FIELD_3BD (basept, 0);                        \
    FIELD_3BD (upd_endpt, 0);                     \
    FIELD_3BD (endpt, 0);                         \
  }

#define AcDbBlockActionWithBasePt_fields          \
  AcDbBlockAction_fields;                         \
  SUBCLASS (AcDbBlockActionWithBasePt)            \
  FIELD_3BD (offset, 0);                          \
  BlockAction_ConnectionPts (conn_pts, 0, 2, 92, 301); \
  DXF { FIELD_3BD (offset, 1011); }               \
  FIELD_B (dependent, 280);                       \
  FIELD_3BD (base_pt, 1012)
  /* if (0) {
    FIELD_3BD (stretch_pt, 0);
  } */

#define AcDbBlockAction_doubles_fields            \
  FIELD_BD (action_offset_x, 140);                \
  FIELD_BD (action_offset_y, 141);                \
  FIELD_BD (angle_offset, 0);                     \
  DXF { VALUE_RC ((BITCODE_RC)1, 280); } /* Action XY type. 1? */
  
#define AcDbBlockConstraintParameter_fields                \
  AcDbBlock2PtParameter_fields;                            \
  SUBCLASS (AcDbBlockConstraintParameter);                 \
  FIELD_HANDLE (dependency, 5, 330)

#define AcDbBlockLinearConstraintParameter_fields     \
  AcDbBlockConstraintParameter_fields;                \
  SUBCLASS (AcDbBlockLinearConstraintParameter)       \
  FIELD_T (expr_name, 305);                           \
  FIELD_T (expr_description, 306);                    \
  FIELD_BD (value, 140);                              \
  AcDbBlockParamValueSet_fields (value_set,96,128,175,307)


DWG_OBJECT (BLOCKVISIBILITYGRIP)
  AcDbBlockGrip_fields;
  SUBCLASS (AcDbBlockVisibilityGrip)
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (BLOCKGRIPLOCATIONCOMPONENT)
  AcDbEvalExpr_fields;
  AcDbBlockGripExpr_fields;
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (BLOCKALIGNMENTGRIP)
  AcDbBlockGrip_fields;
  SUBCLASS (AcDbBlockAlignmentGrip)
  FIELD_3BD_1 (orientation, 140);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (BLOCKALIGNMENTPARAMETER)
  AcDbBlock2PtParameter_fields;
  SUBCLASS (AcDbBlockAlignmentParameter)
  FIELD_B (align_perpendicular, 280)
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (BLOCKLINEARPARAMETER)
  AcDbBlock2PtParameter_fields;
  SUBCLASS (AcDbBlockLinearParameter)
  FIELD_T (distance_name, 305);
  FIELD_T (distance_desc, 306);
  FIELD_BD (distance, 140);
  AcDbBlockParamValueSet_fields (value_set,96,141,175,307);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (BLOCKBASEPOINTPARAMETER)
  AcDbBlock1PtParameter_fields;
  SUBCLASS (AcDbBlockBasepointParameter)
  FIELD_3BD (pt, 1011);
  FIELD_3BD (base_pt, 1012)
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (BLOCKFLIPPARAMETER)
  AcDbBlock2PtParameter_fields;
  SUBCLASS (AcDbBlockFlipParameter)
  FIELD_T (flip_label, 305);
  FIELD_T (flip_label_desc, 306);
  FIELD_T (base_state_label, 307);
  FIELD_T (flipped_state_label, 308);
  FIELD_3BD (def_label_pt, 1012);
  FIELD_BL (bl96, 96);
  FIELD_T (tooltip, 309);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (BLOCKFLIPGRIP)
  AcDbBlockGrip_fields;
  SUBCLASS (AcDbBlockFlipGrip)
  FIELD_BL (combined_state, 0);
  FIELD_3BD_1 (orientation, 140);
  DXF { FIELD_BL (combined_state, 93); }
  /*FIELD_BS (upd_state, 0);
    FIELD_BS (state, 0);*/
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (BLOCKLINEARGRIP)
  AcDbBlockGrip_fields;
  SUBCLASS (AcDbBlockLinearGrip)
  FIELD_3BD_1 (orientation, 140);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (BLOCKLOOKUPGRIP)
  AcDbBlockGrip_fields;
  SUBCLASS (AcDbBlockLookupGrip)
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (BLOCKXYPARAMETER)
  AcDbBlock2PtParameter_fields;
  SUBCLASS (AcDbBlockXYParameter)
  FIELD_T (x_label, 305);
  FIELD_T (x_label_desc, 306);
  FIELD_T (y_label, 307);
  FIELD_T (y_label_desc, 308);
  FIELD_BD (x_value, 142);
  FIELD_BD (y_value, 141);
  DXF {
    AcDbBlockParamValueSet_fields (y_value_set,97,146,176,309);
    AcDbBlockParamValueSet_fields (x_value_set,96,142,175,410);
  } else {
    AcDbBlockParamValueSet_fields (x_value_set,96,142,175,410);
    AcDbBlockParamValueSet_fields (y_value_set,97,146,176,309);
  }
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (BLOCKMOVEACTION)
  AcDbBlockAction_fields;
  SUBCLASS (AcDbBlockMoveAction)
  BlockAction_ConnectionPt (conn_pts[0], 92, 301);
  BlockAction_ConnectionPt (conn_pts[1], 93, 302);
  AcDbBlockAction_doubles_fields;
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (BLOCKFLIPACTION)
  AcDbBlockAction_fields;
  SUBCLASS (AcDbBlockFlipAction)
  BlockAction_ConnectionPts (conn_pts, 0, 4, 92, 301);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (BLOCKROTATIONPARAMETER)
  AcDbBlock2PtParameter_fields;
  SUBCLASS (AcDbBlockRotationParameter)
  FIELD_3BD (def_base_angle_pt, 0);
  FIELD_T (angle_name, 305);
  FIELD_T (angle_desc, 306);
  DXF { FIELD_3BD (def_base_angle_pt, 1011); }
  FIELD_BD (angle, 140);
  AcDbBlockParamValueSet_fields (angle_value_set,96,141,175,307);
  //FIELD_3BD (base_angle_pt, 0);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (BLOCKROTATEACTION)
  AcDbBlockActionWithBasePt_fields;
  SUBCLASS (AcDbBlockRotationAction)
  BlockAction_ConnectionPts (conn_pts, 2, 1, 94, 303);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (BLOCKROTATIONGRIP)
  AcDbBlockGrip_fields;
  SUBCLASS (AcDbBlockRotationGrip)
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (BLOCKSCALEACTION)
  AcDbBlockActionWithBasePt_fields;
  SUBCLASS (AcDbBlockScaleAction)
  BlockAction_ConnectionPts (conn_pts, 0, 3, 94, 303);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// ACAD_ENHANCEDBLOCK?
DWG_OBJECT (BLOCKVISIBILITYPARAMETER)
  AcDbBlock1PtParameter_fields;
  SUBCLASS (AcDbBlockVisibilityParameter)
  FIELD_B (is_initialized, 281);
  FIELD_T (blockvisi_name, 301);
  FIELD_T (blockvisi_desc, 302);
  FIELD_B (unknown_bool, 91); // history_compression, history_required or is_visible?
  FIELD_BL (num_blocks, 93);
  HANDLE_VECTOR (blocks, num_blocks, 4, 331);
  FIELD_BL (num_states, 92);
  REPEAT (num_states, states, Dwg_BLOCKVISIBILITYPARAMETER_state)
  REPEAT_BLOCK
      SUB_FIELD_T (states[rcount1], name, 303);
      SUB_FIELD_BL (states[rcount1], num_blocks, 94);
      SUB_HANDLE_VECTOR (states[rcount1], blocks, num_blocks, 4, 332);
      SUB_FIELD_BL (states[rcount1], num_params, 95);
      SUB_HANDLE_VECTOR (states[rcount1], params, num_params, 4, 333);
      SET_PARENT_OBJ (states[rcount1]);
  END_REPEAT_BLOCK
  END_REPEAT (states)
  //FIELD_T (cur_state_name, 0);
  //FIELD_BL (cur_state, 0);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// unstable, but fields still wrong
// arrays of nodes (of EvalExpr) and edges
// ACAD_EVALUATION_GRAPH
DWG_OBJECT (EVALUATION_GRAPH)

  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbEvalGraph)
  FIELD_BLd (first_nodeid, 96);
  FIELD_BLd (first_nodeid_copy, 97);// always same as first_nodeid
  FIELD_BL (num_nodes, 0);
  REPEAT (num_nodes, nodes, Dwg_EVAL_Node)
  REPEAT_BLOCK
      SUB_FIELD_BL (nodes[rcount1], id, 91); // starting with 0
      SUB_FIELD_BL (nodes[rcount1], edge_flags, 93); // always 32
      if (_obj->nodes[rcount1].edge_flags != 32) {
        _obj->nodes[rcount1].edge_flags = 0;
        _obj->num_nodes = rcount1;
        break;
      }
      SUB_FIELD_BLd (nodes[rcount1], nextid, 95); // 1
      SUB_FIELD_HANDLE (nodes[rcount1], evalexpr, 5, 360);
      SUB_FIELD_VECTOR_INL (nodes[rcount1], node, BLd, 4, 92)
      if (FIELD_VALUE (has_graph))
        SUB_FIELD_B (nodes[rcount1], active_cycles, 0);
      SET_PARENT_OBJ (nodes[rcount1]);
  END_REPEAT_BLOCK
  END_REPEAT (nodes)

  FIELD_BL (num_edges, 0);
  REPEAT (num_edges, edges, Dwg_EVAL_Edge)
  REPEAT_BLOCK
      SUB_FIELD_BL (edges[rcount1], id, 92); // starting with 0
      SUB_FIELD_BLd (edges[rcount1], nextid, 93); //
      SUB_FIELD_BLd (edges[rcount1], e1, 94); // incoming edges
      SUB_FIELD_BLd (edges[rcount1], e2, 91); //
      SUB_FIELD_BLd (edges[rcount1], e3, 91); //
      SUB_FIELD_BLd (edges[rcount1], out_edge[0], 92); //
      SUB_FIELD_BLd (edges[rcount1], out_edge[1], 92); //
      SUB_FIELD_BLd (edges[rcount1], out_edge[2], 92); //
      SUB_FIELD_BLd (edges[rcount1], out_edge[3], 92); //
      SUB_FIELD_BLd (edges[rcount1], out_edge[4], 92); //
      SET_PARENT_OBJ (edges[rcount1]);
  END_REPEAT_BLOCK
  END_REPEAT (edges)

  START_OBJECT_HANDLE_STREAM;
  //HANDLE_VECTOR (evalexpr, num_evalexpr, 5, 360);
DWG_OBJECT_END

DWG_ENTITY (PLANESURFACE)
  DECODE_UNKNOWN_BITS
  ACTION_3DSOLID;
  FIELD_BS (modeler_format_version, 70); //def 1
  //FIELD_BL (bindata_size, 90);
  //FIELD_TF (bindata, FIELD_VALUE (bindata_size), 1); // in DXF as encrypted ASCII

  SUBCLASS (AcDbSurface)
  FIELD_BS (u_isolines, 71);
  FIELD_BS (v_isolines, 72);
  //SUBCLASS (AcDbPlaneSurface)
  //FIELD_BL (class_version, 90);
  //if (FIELD_VALUE (class_version) > 10)
  //  return DWG_ERR_VALUEOUTOFBOUNDS;
  COMMON_ENTITY_HANDLE_DATA;
DWG_ENTITY_END

#define AcDbObjectContextData_fields                                    \
  SUBCLASS (AcDbObjectContextData);                                     \
  FIELD_BS (class_version, 70);                                         \
  FIELD_B (is_default, 290)

#define AcDbAnnotScaleObjectContextData_fields                          \
  AcDbObjectContextData_fields;                                         \
  SUBCLASS (AcDbAnnotScaleObjectContextData);                           \
  FIELD_HANDLE (scale, 2, 340)

#define AcDbTextObjectContextData_fields                                \
  SUBCLASS (AcDbTextObjectContextData)                                  \
  FIELD_BS (horizontal_mode, 70);                                       \
  FIELD_BD (rotation, 50);                                              \
  FIELD_2RD (ins_pt, 10);                                               \
  FIELD_2RD (alignment_pt, 11)

//  #ifdef IS_DXF
//    FIELD_HANDLE_NAME (block, 2, BLOCK_HEADER);
#define AcDbDimensionObjectContextData_fields           \
  SUBCLASS (AcDbDimensionObjectContextData);            \
  DXF { FIELD_HANDLE (dimension.block, 5, 2); }         \
  DXF { FIELD_B (dimension.b293, 293); }                \
  FIELD_2RD (dimension.def_pt, 10); /* text location */ \
  DXF { VALUE_RD (0.0, 30); }                           \
  FIELD_B (dimension.is_def_textloc, 294); /* 1 */      \
  FIELD_BD (dimension.text_rotation, 140);              \
  FIELD_HANDLE (dimension.block, 5, 0);                 \
  FIELD_B (dimension.b293, 0);                          \
  FIELD_B (dimension.dimtofl, 298);                     \
  FIELD_B (dimension.dimosxd, 291);                     \
  FIELD_B (dimension.dimatfit, 70);                     \
  FIELD_B (dimension.dimtix, 292);                      \
  FIELD_B (dimension.dimtmove, 71);                     \
  FIELD_RC (dimension.override_code, 280);              \
  FIELD_B (dimension.has_arrow2, 295);                  \
  FIELD_B (dimension.flip_arrow2, 296);                 \
  FIELD_B (dimension.flip_arrow1, 297)

/*=============================================================================*/

/* In work area:
   The following entities/objects are only stored as raw UNKNOWN_ENT/OBJ,
   unless enabled via --enable-debug/-DDEBUG_CLASSES */

#if defined (DEBUG_CLASSES) || defined (IS_FREE)

// (varies) UNSTABLE
// 1-4 references, see associativity bits 1-8.
DWG_OBJECT (DIMASSOC)

  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbDimAssoc)
  FIELD_BLx (associativity, 90);
  FIELD_B (trans_space_flag, 70);
  FIELD_RC (rotated_type, 71);
  FIELD_HANDLE (dimensionobj, 4, 330);
  REPEAT_CN (4, ref, Dwg_DIMASSOC_Ref) // i.e. AcDbOsnapPointRef
  REPEAT_BLOCK
      // TODO: there could be much more blocks, up to 5.
      // 0 1 2 3 => 1 2 4 8. skip unset bits
      if (!(FIELD_VALUE (associativity) & (1<<rcount1)))
        {
#ifdef IS_JSON
          ENDHASH;
#endif
          continue;
        }
      LOG_HANDLE ("DIMASSOC_Ref.rcount1: %d\n", rcount1);
      // DXF: 1, 72, 10, ??, 75
      SUB_FIELD_T  (ref[rcount1], classname, 1); // "AcDbOsnapPointRef"
      SUB_FIELD_RC (ref[rcount1], osnap_type, 72); // 0-13
      // idpaths:
      SUB_FIELD_BL0 (ref[rcount1], num_intsectobj, 74);
      SUB_HANDLE_VECTOR (ref[rcount1], intsectobj, num_intsectobj, 5, 332);

      SUB_FIELD_BD (ref[rcount1], osnap_dist, 40);
      SUB_FIELD_3BD (ref[rcount1], osnap_pt, 10);

      // XrefFullSubentPath
      SUB_FIELD_BL (ref[rcount1], num_xrefs, 0); // 1 or 2
      SUB_VALUEOUTOFBOUNDS (ref[rcount1], num_xrefs, 100)
      SUB_HANDLE_VECTOR (ref[rcount1], xrefs, num_xrefs, 4, 331);

      // restrict only when writing, not when reading?
      //if (FIELD_VALUE (ref[rcount1].osnap_type) == 6 || FIELD_VALUE (ref[rcount1].osnap_type) == 11)
      // {
      SUB_FIELD_BL0 (ref[rcount1], main_subent_type, 73);
      SUB_FIELD_BL (ref[rcount1], main_gsmarker, 91);
      SUB_FIELD_BL (ref[rcount1], num_xrefpaths, 0);
      FIELD_VECTOR_T (ref[rcount1].xrefpaths, T, ref[rcount1].num_xrefpaths, 301)
      // }
      SUB_FIELD_B  (ref[rcount1], has_lastpt_ref, 75);
      if (FIELD_VALUE (ref[rcount1].has_lastpt_ref))
        {
          SUB_FIELD_3BD (ref[rcount1], lastpt_ref, 0);
        }
      SET_PARENT_OBJ (ref[rcount1]);
  END_REPEAT_BLOCK
  END_REPEAT (ref)

  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// (varies) fails the unit-test
// See AcDbAssocActionBody.h and ASSOCPLANESURFACEACTIONBODY
DWG_OBJECT (ASSOCSWEPTSURFACEACTIONBODY)
  DECODE_UNKNOWN_BITS
  AcDbAssocPathBasedSurfaceActionBody_fields;
  SUBCLASS (AcDbAssocSweptSurfaceActionBody)
  FIELD_BL (class_version, 90);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (ASSOCEDGECHAMFERACTIONBODY)
  DECODE_UNKNOWN_BITS
  AcDbAssocPathBasedSurfaceActionBody_fields;
  SUBCLASS (AcDbAssocEdgeChamferActionBody)
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (ASSOCEDGEFILLETACTIONBODY)
  DECODE_UNKNOWN_BITS
  AcDbAssocPathBasedSurfaceActionBody_fields;
  SUBCLASS (AcDbAssocEdgeFilletActionBody)
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (ASSOCRESTOREENTITYSTATEACTIONBODY)
  DECODE_UNKNOWN_BITS
  AcDbAssocActionBody_fields;
  SUBCLASS (AcDbAssocRestoreEntityStateActionBody)
  FIELD_BL (class_version, 90);
  FIELD_HANDLE (entity, 5, 330); // hardptr
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (ASSOCMLEADERACTIONBODY)
  DECODE_UNKNOWN_BITS
  AcDbAssocAnnotationActionBody_fields;
  SUBCLASS (AcDbAssocMLeaderActionBody)
  FIELD_BL (class_version, 90);
  FIELD_BL (num_actions, 90);
  VALUEOUTOFBOUNDS (num_actions, 100)
  REPEAT (num_actions, actions, Dwg_ASSOCACTIONBODY_action)
  REPEAT_BLOCK
    SUB_FIELD_BL (actions[rcount1], depid, 0);
    SUB_FIELD_HANDLE (actions[rcount1], dep, 5, 330); // hardptr
  END_REPEAT_BLOCK
  END_REPEAT (actions);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// crashes
// See AcDbAssocActionBody.h and AcDbAssocDimDependencyBody.h
DWG_OBJECT (ASSOCALIGNEDDIMACTIONBODY)
  DECODE_UNKNOWN_BITS
  AcDbAssocAnnotationActionBody_fields;
  SUBCLASS (ACDBASSOCALIGNEDDIMACTIONBODY)
  //SUBCLASS (AcDbAssocAlignedDimActionBody)
  FIELD_BL (class_version, 90);
  //or status, 90 //has d_node or r_node?
  START_OBJECT_HANDLE_STREAM;
  FIELD_HANDLE (r_node, 4, 330);
  FIELD_HANDLE (d_node, 4, 330);
DWG_OBJECT_END

DWG_OBJECT (ASSOC3POINTANGULARDIMACTIONBODY)
  DECODE_UNKNOWN_BITS
  AcDbAssocAnnotationActionBody_fields;
  SUBCLASS (Assoc3PointAngularDimActionBody)
  FIELD_BS (class_version, 90);
  START_OBJECT_HANDLE_STREAM;
  FIELD_HANDLE (r_node, 4, 330);
  FIELD_HANDLE (d_node, 4, 330);
  FIELD_HANDLE (assocdep, 5, 330);
DWG_OBJECT_END

DWG_OBJECT (ASSOCORDINATEDIMACTIONBODY)
  DECODE_UNKNOWN_BITS
  AcDbAssocAnnotationActionBody_fields;
  SUBCLASS (AssocOrdinatedDimActionBody)
  FIELD_BL (class_version, 90);
  START_OBJECT_HANDLE_STREAM;
  FIELD_HANDLE (r_node, 5, 330);
  FIELD_HANDLE (d_node, 5, 330);
DWG_OBJECT_END

DWG_OBJECT (ASSOCROTATEDDIMACTIONBODY)
  DECODE_UNKNOWN_BITS
  AcDbAssocAnnotationActionBody_fields;
  SUBCLASS (AssocRotatedDimActionBody)
  FIELD_BS (class_version, 90);
  START_OBJECT_HANDLE_STREAM;
  FIELD_HANDLE (r_node, 5, 330);
  FIELD_HANDLE (d_node, 5, 330);
DWG_OBJECT_END

// subclass only
//DWG_OBJECT (OBJECTCONTEXTDATA)
//  DECODE_UNKNOWN_BITS
//  SUBCLASS (AcDbObjectContextData)
//  SINCE (R_2010) {
//    FIELD_BS (class_version, 70);
//    if (FIELD_VALUE (class_version) > 10)
//      return DWG_ERR_VALUEOUTOFBOUNDS;
//  }
//  else {
//    IF_ENCODE_FROM_EARLIER {
//      FIELD_VALUE (class_version) = 3;
//    }
//  }
//  FIELD_B (is_default, 290);
//  FIELD_B (has_xdic, 0);
//  START_OBJECT_HANDLE_STREAM;
//DWG_OBJECT_END

DWG_OBJECT (CONTEXTDATAMANAGER)
  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbContextDataManager)
  FIELD_HANDLE (objectcontext, 5, 0);
  FIELD_BL (num_submgrs, 0);
  REPEAT (num_submgrs, submgrs, Dwg_CONTEXTDATA_submgr)
  REPEAT_BLOCK
      SUB_FIELD_HANDLE (submgrs[rcount1],handle, 5, 0);
      SUB_FIELD_BL (submgrs[rcount1],num_entries, 90);
      REPEAT2 (submgrs[rcount1].num_entries, submgrs[rcount1].entries, Dwg_CONTEXTDATA_dict)
      REPEAT_BLOCK
          SUB_FIELD_HANDLE (submgrs[rcount1].entries[rcount2],itemhandle, 5, 350);
          SUB_FIELD_T (submgrs[rcount1].entries[rcount2],text, 3);
          SET_PARENT (submgrs[rcount1].entries[rcount2], &_obj->submgrs[rcount1]);
      END_REPEAT_BLOCK
      END_REPEAT (submgrs[rcount1].entries)
      SET_PARENT_OBJ (submgrs[rcount1]);
  END_REPEAT_BLOCK
  END_REPEAT (submgrs)

  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// (varies) DEBUGGING
DWG_OBJECT (SUNSTUDY)

  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbSunStudy)
  FIELD_BL (class_version, 90);
  VALUEOUTOFBOUNDS (class_version, 10)
  FIELD_T (setup_name, 1);
  FIELD_T (description, 2);
  FIELD_BL (output_type, 70);
  if (FIELD_VALUE (output_type) == 0) // Sheet_Set
    {
      FIELD_B (use_subset, 290);
      FIELD_T (sheet_set_name, 3);
      FIELD_T (sheet_subset_name, 4);
    }
  FIELD_B (select_dates_from_calendar, 291);
  FIELD_BL (num_dates, 91);
  VALUEOUTOFBOUNDS (num_dates, 10000)
  REPEAT (num_dates, dates, Dwg_SUNSTUDY_Dates)
  REPEAT_BLOCK
      SUB_FIELD_BL (dates[rcount1], julian_day, 90);
      SUB_FIELD_BL (dates[rcount1], msecs, 90);
  END_REPEAT_BLOCK
  END_REPEAT (dates);
  FIELD_B (select_range_of_dates, 292);
  if (FIELD_VALUE (select_range_of_dates))
    {
       FIELD_BL (start_time, 93);
       FIELD_BL (end_time, 94);
       FIELD_BL (interval, 95);
    }
  FIELD_BL (num_hours, 91);
  VALUEOUTOFBOUNDS (num_hours, 10000)
  FIELD_VECTOR (hours, B, num_hours, 290);

  FIELD_BL (shade_plot_type, 74);
  FIELD_BL (numvports, 75);
  FIELD_BL (numrows, 76);
  FIELD_BL (numcols, 77);
  FIELD_BD (spacing, 40);
  FIELD_B (lock_viewports, 293);
  FIELD_B (label_viewports, 294);

  START_OBJECT_HANDLE_STREAM;
  FIELD_HANDLE (page_setup_wizard, 5, 340);
  FIELD_HANDLE (view, 5, 341);
  FIELD_HANDLE (visualstyle, 2, 342);
  FIELD_HANDLE (text_style, 2, 343);

DWG_OBJECT_END

// (varies) UNSTABLE
// in DXF as POSITIONMARKER (rename?, no), command: GEOMARKPOSITION, GEOMARKPOINT
// r2014+
DWG_ENTITY (GEOPOSITIONMARKER)

  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbGeoPositionMarker)
  FIELD_BL (class_version, 90); // 0
  FIELD_3BD (position, 10);
  FIELD_BD (radius, 40);
  FIELD_T (notes, 1);
  FIELD_BD (landing_gap, 40);
  FIELD_B (mtext_visible, 290);
  FIELD_RCd (text_alignment, 280);
  FIELD_B (enable_frame_text, 290);
  if (FIELD_VALUE (enable_frame_text))
    {
      DECODER {
        dwg_add_object (dwg);
        _obj->mtext = &dwg->object[dwg->num_objects - 1];
        dwg_setup_MTEXT (_obj->mtext);
      }
      DXF { VALUE_TFF ( "Embedded Object", 101 ); }
      CALL_ENTITY (MTEXT, _obj->mtext);
    }
  COMMON_ENTITY_HANDLE_DATA;
DWG_ENTITY_END

#define SweepOptions_fields  \
  FIELD_BD (draft_angle, 42); \
  FIELD_BD (draft_start_distance, 43); \
  FIELD_BD (draft_end_distance, 44); \
  FIELD_BD (twist_angle, 45); \
  FIELD_BD (scale_factor, 48); \
  FIELD_BD (align_angle, 49); \
  FIELD_VECTOR_N (sweep_entity_transmatrix, BD, 16, 46); \
  FIELD_VECTOR_N (path_entity_transmatrix, BD, 16, 47); \
  FIELD_B (is_solid, 290); \
  FIELD_BS (sweep_alignment_flags, 70); \
  FIELD_BS (path_flags, 71); \
  FIELD_B (align_start, 292); \
  FIELD_B (bank, 293); \
  FIELD_B (base_point_set, 294); \
  FIELD_B (sweep_entity_transform_computed, 295); \
  FIELD_B (path_entity_transform_computed, 296); \
  FIELD_3BD (reference_vector_for_controlling_twist, 11)

// r2007+
DWG_ENTITY (EXTRUDEDSURFACE)

  DECODE_UNKNOWN_BITS
  ACTION_3DSOLID;
  //FIELD_BS (modeler_format_version, 70); //def 1
  SUBCLASS (AcDbSurface)
  FIELD_BS (u_isolines, 71);
  FIELD_BS (v_isolines, 72);
  SUBCLASS (AcDbExtrudedSurface)
#ifdef IS_DXF
  //FIELD_BL (class_version, 90); // or entity type?
  CALL_SUBENT (_obj->entity, 90)
#else
  // here and at SweptSurface
  SweepOptions_fields;
#endif
  FIELD_3BD (sweep_vector, 10);
  FIELD_VECTOR_N (sweep_transmatrix, BD, 16, 40);
#ifdef IS_DXF
  SweepOptions_fields;
#else
  //FIELD_BL (class_version, 90); // or entity type?
  CALL_SUBENT (_obj->entity, 90)
#endif

  COMMON_ENTITY_HANDLE_DATA;
  //FIELD_HANDLE (sweep_entity, 5, 0);
  //FIELD_HANDLE (path_entity, 5, 0);

DWG_ENTITY_END

// r2007+
DWG_ENTITY (LOFTEDSURFACE)

  DECODE_UNKNOWN_BITS
  ACTION_3DSOLID;
  FIELD_BS (modeler_format_version, 70); //def 1
  VALUEOUTOFBOUNDS (modeler_format_version, 3)

  SUBCLASS (AcDbSurface)
  FIELD_BS (u_isolines, 71);
  FIELD_BS (v_isolines, 72);
  SUBCLASS (AcDbLoftedSurface)
  FIELD_VECTOR_N (loft_entity_transmatrix, BD, 16, 40);
  //90 77
  //90 544
  //310
  //90 77
  //90 608
  //310
  //FIELD_BL (class_version, 90);
  FIELD_BL (plane_normal_lofting_type, 70);
  FIELD_BD (start_draft_angle, 41);
  FIELD_BD (end_draft_angle, 42);
  FIELD_BD (start_draft_magnitude, 43);
  FIELD_BD (end_draft_magnitude, 44);
  FIELD_B (arc_length_parameterization, 290);
  FIELD_B (no_twist, 291);
  FIELD_B (align_direction, 292);
  FIELD_B (simple_surfaces, 293);
  FIELD_B (closed_surfaces, 294);
  FIELD_B (solid, 295);
  FIELD_B (ruled_surface, 296);
  FIELD_B (virtual_guide, 297);
  FIELD_BS (num_cross_sections, 0);
  FIELD_BS (num_guide_curves, 0);
  VALUEOUTOFBOUNDS (num_cross_sections, 5000)
  VALUEOUTOFBOUNDS (num_guide_curves, 5000)

  COMMON_ENTITY_HANDLE_DATA;
  HANDLE_VECTOR (cross_sections, num_cross_sections, 5, 310);
  HANDLE_VECTOR (guide_curves, num_guide_curves, 5, 310);
  FIELD_HANDLE (path_curve, 5, 0);

DWG_ENTITY_END

// r2007+
DWG_ENTITY (REVOLVEDSURFACE)

  DECODE_UNKNOWN_BITS
  ACTION_3DSOLID;
  FIELD_BS (modeler_format_version, 70); //def 1

  SUBCLASS (AcDbSurface)
  FIELD_BS (u_isolines, 71);
  FIELD_BS (v_isolines, 72);
  SUBCLASS (AcDbRevolvedSurface)
  FIELD_BL (class_version, 90);
  VALUEOUTOFBOUNDS (class_version, 10)

  FIELD_BL (id, 90);
  //FIELD_BL (bindata_size, 90);
  //FIELD_BINARY (bindata, FIELD_VALUE (bindata_size), 310);
  FIELD_3BD (axis_point, 10);
  FIELD_3BD (axis_vector, 11);
  FIELD_BD (revolve_angle, 40);
  FIELD_BD (start_angle, 41);
  FIELD_VECTOR_N (revolved_entity_transmatrix, BD, 16, 42);
  FIELD_BD (draft_angle, 43);
  FIELD_BD (draft_start_distance, 44);
  FIELD_BD (draft_end_distance, 45);
  FIELD_BD (twist_angle, 46);
  FIELD_B (solid, 290);
  FIELD_B (close_to_axis, 291);

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

DWG_ENTITY (SWEPTSURFACE)

  DECODE_UNKNOWN_BITS
  ACTION_3DSOLID;
  FIELD_BS (modeler_format_version, 70); //def 1

  SUBCLASS (AcDbSurface)
  FIELD_BS (u_isolines, 71);
  FIELD_BS (v_isolines, 72);
  SUBCLASS (AcDbSweptSurface)
  FIELD_BL (class_version, 90);
  VALUEOUTOFBOUNDS (class_version, 10)

  FIELD_BL (sweep_entity_id, 90);
#ifndef IS_JSON
  FIELD_BL (sweepdata_size, 90);
#endif
  VALUEOUTOFBOUNDS (sweepdata_size, 5000)
  FIELD_BINARY (sweepdata, FIELD_VALUE (sweepdata_size), 310);
  FIELD_BL (path_entity_id, 90);
#ifndef IS_JSON
  FIELD_BL (pathdata_size, 90);
#endif
  VALUEOUTOFBOUNDS (pathdata_size, 5000)
  FIELD_BINARY (pathdata, FIELD_VALUE (pathdata_size), 310);
  // here and at ExtrudedSurface
  SweepOptions_fields;
  COMMON_ENTITY_HANDLE_DATA;
DWG_ENTITY_END

DWG_ENTITY (NURBSURFACE)
  DECODE_UNKNOWN_BITS
  ACTION_3DSOLID;
  SUBCLASS (AcDbSurface)
  FIELD_BS (u_isolines, 71);
  FIELD_BS (v_isolines, 72);
  SUBCLASS (AcDbNurbSurface)
  SINCE (R_2013) {
    FIELD_BS (short170, 170);
    FIELD_B (cv_hull_display, 290);
    FIELD_3BD (uvec1, 10);
    FIELD_3BD (vvec1, 11);
    FIELD_3BD (uvec2, 12);
    FIELD_3BD (vvec2, 13);
  }
  COMMON_ENTITY_HANDLE_DATA;
DWG_ENTITY_END

// (varies)
// works ok on all example_20* but this coverage seems limited
// The static variant
DWG_OBJECT (PERSUBENTMGR)
  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbPersSubentManager)
  FIELD_BL (class_version, 90); //2
  VALUEOUTOFBOUNDS (class_version, 3)
  FIELD_BL (unknown_0, 90); //always 0
  FIELD_BL (unknown_2, 90); //always 2

  FIELD_BL (numassocsteps, 90);  //3
  FIELD_BL (numassocsubents, 90);//0
  FIELD_BL (num_steps, 90);      //1
  FIELD_VECTOR (steps, BL, num_steps, 90); //1
  FIELD_BL (num_subents, 90);
  // nope: 3x BL and CALL_SUBENT
  FIELD_VECTOR (subents, BL, num_subents, 90);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// See AcDbAssocPersSubentIdPE.h
// The dynamic variant
DWG_OBJECT (ASSOCPERSSUBENTMANAGER)
  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbAssocPersSubentManager)
  FIELD_BL (class_version, 90); //1 or 2 (r2013+)
  FIELD_BL (unknown_3, 90); //3
  FIELD_BL (unknown_0, 90); //0
  FIELD_BL (unknown_2, 90); //2

  FIELD_BL (num_steps, 90);   //3
  FIELD_VECTOR (steps, BL, num_steps, 90);
  FIELD_BL (num_subents, 90);
  // TODO subent struct
  FIELD_BL (unknown_bl6, 90); //5
  FIELD_BL (unknown_bl6a, 90); //0 10 0100000100 0100000011
  FIELD_BL (unknown_bl7a, 90); //4
  FIELD_BL (unknown_bl7, 90); //3 0b0100000011
  FIELD_BL (unknown_bl7, 90); //2
  FIELD_BL (unknown_bl8, 90); //2
  FIELD_BL (unknown_bl9, 90); //2
  FIELD_BL (unknown_bl10, 90); //21
  FIELD_BL (unknown_bl11, 90); //0
  FIELD_BL (unknown_bl12, 90); //0
  FIELD_BL (unknown_bl13, 90); //0
  FIELD_BL (unknown_bl14, 90); //0
  FIELD_BL (unknown_bl15, 90); //1 [[133,142]]
  FIELD_BL (unknown_bl16, 90); //3
  FIELD_BL (unknown_bl17, 90); //1
  FIELD_BL (unknown_bl18, 90); //1000000000
  FIELD_BL (unknown_bl19, 90); //1001
  FIELD_BL (unknown_bl20, 90); //1
  FIELD_BL (unknown_bl21, 90); //1000000000
  FIELD_BL (unknown_bl22, 90); //51001
  FIELD_BL (unknown_bl23, 90); //1
  FIELD_BL (unknown_bl24, 90); //1000000000
  FIELD_BL (unknown_bl25, 90); //351001
  FIELD_BL (unknown_bl26, 90); //0
  FIELD_BL (unknown_bl27, 90); //0
  FIELD_BL (unknown_bl28, 90); //0
  FIELD_BL (unknown_bl29, 90); //900
  FIELD_BL (unknown_bl30, 90); //0
  FIELD_BL (unknown_bl31, 90); //900
  FIELD_BL (unknown_bl32, 90); //0
  FIELD_BL (unknown_bl33, 90); //2
  FIELD_BL (unknown_bl34, 90); //2
  FIELD_BL (unknown_bl35, 90); //3 0100000011
  FIELD_BL (unknown_bl36, 90); //0
  FIELD_B  (unknown_b37, 290); //0

  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

#define AcDbShSubentMaterial_fields                                           \
    SUBCLASS (AcDbShSubentMaterial)                                           \
    FIELD_BL (material.major, 90);                                            \
    FIELD_BL (material.minor, 91);                                            \
    FIELD_BL (material.reflectance, 92);                                      \
    FIELD_BL (material.displacement, 93);                                     \
    FIELD_HANDLE (material.material, 5, 331)

#define AcDbShSubentColor_fields                                              \
    SUBCLASS (AcDbShSubentColor)                                              \
    FIELD_BL (color.major, 90); /* 33 */                                      \
    FIELD_BL (color.minor, 91); /* 1 */                                       \
    FIELD_BL (color.transparency, 92);                                        \
    FIELD_BL (color.bl93, 93);                                                \
    FIELD_BL (color.is_face_variable, 290);                                   \
    FIELD_CMC (color.color,62)

#define CLASS_HAS(x) 1

// Class AcDbSweepOptions? DEBUGGING
// dbSweepOptions.h dbsurf.h
DWG_OBJECT (ACSH_SWEEP_CLASS)
  DECODE_UNKNOWN_BITS
  AcDbEvalExpr_fields;
  AcDbShHistoryNode_fields (history_node);
  SUBCLASS (AcDbShPrimitive)
  SUBCLASS (AcDbShSweepBase)
  FIELD_BL (major, 90); //33
  FIELD_BL (minor, 91); //29
  FIELD_3BD (direction, 10); //0,0,0
  // sweep_options
  // sweep_entity
  // path_entity
  FIELD_BL (bl92, 92); //77
#ifndef IS_JSON
  FIELD_BL (shsw_text_size, 90); //744
#endif
  FIELD_BINARY (shsw_text, FIELD_VALUE (shsw_text_size), 310);
  FIELD_BL (shsw_bl93, 93); //77
#ifndef IS_JSON
  FIELD_BL (shsw_text2_size, 90); //480
#endif
  FIELD_BINARY (shsw_text2, FIELD_VALUE (shsw_text2_size), 310);
  FIELD_BD (draft_angle, 42); //0.0
  FIELD_BD (start_draft_dist, 43); //0.0
  FIELD_BD (end_draft_dist, 44); //0.0
  FIELD_BD (scale_factor, 45); //1.0
  FIELD_BD (twist_angle, 48); //0.0
  FIELD_BD (align_angle, 49); //0.0
  FIELD_VECTOR_N (sweepentity_transform, BD, 16, 46);
  FIELD_VECTOR_N (pathentity_transform, BD, 16, 47);
  FIELD_RC (align_option, 70); //2
  FIELD_RC (miter_option, 71); //2
  FIELD_B (has_align_start, 290); //1
  FIELD_B (bank, 292); //1
  FIELD_B (check_intersections, 293); //0
  FIELD_B (shsw_b294, 294); //1
  FIELD_B (shsw_b295, 295); //1
  FIELD_B (shsw_b296, 296); //1
  FIELD_3BD (pt2, 11); //0,0,0

  SUBCLASS (AcDbShSweep)
  // align_option
  // miter_option

  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (ACSH_EXTRUSION_CLASS)
  DECODE_UNKNOWN_BITS
  AcDbEvalExpr_fields;
  AcDbShHistoryNode_fields (history_node);
  SUBCLASS (AcDbShPrimitive)
  SUBCLASS (AcDbShSweepBase)
  FIELD_BL (major, 90); //33
  FIELD_BL (minor, 91); //29
  FIELD_3BD (direction, 10); //0,0,0
  // sweep_options
  // sweep_entity
  // path_entity
  FIELD_BL (bl92, 92); //77
#ifndef IS_JSON
  FIELD_BL (shsw_text_size, 90); //744
#endif
  FIELD_BINARY (shsw_text, FIELD_VALUE (shsw_text_size), 310);
  FIELD_BL (shsw_bl93, 93); //77
#ifndef IS_JSON
  FIELD_BL (shsw_text2_size, 90); //480
#endif
  FIELD_BINARY (shsw_text2, FIELD_VALUE (shsw_text2_size), 310);
  FIELD_BD (draft_angle, 42); //0.0
  FIELD_BD (start_draft_dist, 43); //0.0
  FIELD_BD (end_draft_dist, 44); //0.0
  FIELD_BD (scale_factor, 45); //1.0
  FIELD_BD (twist_angle, 48); //0.0
  FIELD_BD (align_angle, 49); //0.0
  FIELD_VECTOR_N (sweepentity_transform, BD, 16, 46);
  FIELD_VECTOR_N (pathentity_transform, BD, 16, 47);
  FIELD_RC (align_option, 70); //2
  FIELD_RC (miter_option, 71); //2
  FIELD_B (has_align_start, 290); //1
  FIELD_B (bank, 292); //1
  FIELD_B (check_intersections, 293); //0
  FIELD_B (shsw_b294, 294); //1
  FIELD_B (shsw_b295, 295); //1
  FIELD_B (shsw_b296, 296); //1
  FIELD_3BD (pt2, 11); //0,0,0

  SUBCLASS (AcDbShExtrusion)

  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (ACSH_LOFT_CLASS)
  DECODE_UNKNOWN_BITS
  AcDbEvalExpr_fields;
  AcDbShHistoryNode_fields (history_node);
  SUBCLASS (AcDbShPrimitive)
  SUBCLASS (AcDbShLoft)
  FIELD_BL (major, 90);
  FIELD_BL (minor, 91);
  FIELD_BL (num_crosssects, 92);
  REPEAT (num_crosssects, crosssects, BITCODE_H)
  REPEAT_BLOCK
  {
    CALL_SUBENT (_obj->crosssects[rcount1], 93);
  }
  END_REPEAT_BLOCK
  END_REPEAT (crosssects);

  FIELD_BL (num_guides, 95);
  REPEAT (num_guides, guides, BITCODE_H)
  REPEAT_BLOCK
  {
    CALL_SUBENT (_obj->guides[rcount1], 96);
  }
  END_REPEAT_BLOCK
  END_REPEAT (guides);

  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (ACSH_REVOLVE_CLASS)
  DECODE_UNKNOWN_BITS
  AcDbEvalExpr_fields;
  AcDbShHistoryNode_fields (history_node);
  SUBCLASS (AcDbShPrimitive)
  SUBCLASS (AcDbShRevolve)
  FIELD_BL (major, 90); //33
  FIELD_BL (minor, 91); //29
  FIELD_3BD (axis_pt, 10);
  FIELD_2RD (direction, 11); // 3d in dxf
  FIELD_BD (revolve_angle, 40);
  FIELD_BD (start_angle, 41);
  FIELD_BD (draft_angle, 43);
  FIELD_BD (bd44, 44);
  FIELD_BD (bd45, 45);
  FIELD_BD (twist_angle, 46);
  FIELD_B (b290, 290);
  FIELD_B (is_close_to_axis, 291);
  CALL_SUBENT (obj->sweep_entity, 90);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// called COORDINATION_MODEL in the DXF docs
// also called NAVISWORKSREFERENCE
DWG_ENTITY (NAVISWORKSMODEL)
  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbNavisworksModel)
  FIELD_BS (flags, 70);
  FIELD_HANDLE (definition, 2, 340);
  FIELD_VECTOR_N (transmatrix, BD, 16, 40);
  FIELD_BD (unitfactor, 40);
  COMMON_ENTITY_HANDLE_DATA;
DWG_ENTITY_END

// (varies)
// Stored in ACAD_BIM_DEFINITIONS dictionary
DWG_OBJECT (NAVISWORKSMODELDEF)
  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbNavisworksModelDef)
  FIELD_BS (flags, 70);
  FIELD_T (path, 1);
  FIELD_B (status, 290);
  FIELD_3BD (min_extent, 10);
  FIELD_3BD (max_extent, 11);
  FIELD_B (host_drawing_visibility, 290); // DWG_model_item_visible
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// LiveMap raster image underlay r2015+
DWG_OBJECT (GEOMAPIMAGE)
  DECODE_UNKNOWN_BITS

  //SUBCLASS (AcDbImage)
  //SUBCLASS (AcDbRasterImage)
  SUBCLASS (AcDbGeomapImage)
  FIELD_BL (class_version, 90);
  VALUEOUTOFBOUNDS (class_version, 10)
  FIELD_3DPOINT (pt0, 10);
  //FIELD_3DPOINT (uvec, 11);
  //FIELD_3DPOINT (vvec, 12);
  FIELD_2RD (size, 13);
  FIELD_BS (display_props, 70);
  FIELD_B (clipping, 280); // i.e. clipping_enabled
  FIELD_RC (brightness, 281);
  FIELD_RC (contrast, 282);
  FIELD_RC (fade, 283);

/* VBA props:
origin
  FIELD_BD (rotation, 0);
image_width
image_height
name
image_file
image_visibility
transparency
height
width
  FIELD_B (show_rotation, 0);
  FIELD_BD (scale_factor, 0);
geoimage_brightness
geoimage_contrast
geoimage_fade
geoimage_position
geoimage_width
geoimage_height
*/
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (ANNOTSCALEOBJECTCONTEXTDATA)
  DECODE_UNKNOWN_BITS
  AcDbAnnotScaleObjectContextData_fields;
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (ANGDIMOBJECTCONTEXTDATA)
  DECODE_UNKNOWN_BITS
  AcDbAnnotScaleObjectContextData_fields;
  AcDbDimensionObjectContextData_fields;
  SUBCLASS (AcDbAngularDimensionObjectContextData)
  FIELD_3BD (arc_pt, 11);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (DMDIMOBJECTCONTEXTDATA)
  DECODE_UNKNOWN_BITS
  AcDbAnnotScaleObjectContextData_fields;
  AcDbDimensionObjectContextData_fields;
  SUBCLASS (AcDbDiametricDimensionObjectContextData)
  FIELD_3BD (first_arc_pt, 11);
  FIELD_3BD (def_pt, 12);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (ORDDIMOBJECTCONTEXTDATA)
  DECODE_UNKNOWN_BITS
  AcDbAnnotScaleObjectContextData_fields;
  AcDbDimensionObjectContextData_fields;
  SUBCLASS (AcDbOrdinateDimensionObjectContextData)
  FIELD_3BD (feature_location_pt, 11); // = origin
  FIELD_3BD (leader_endpt, 12);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (RADIMOBJECTCONTEXTDATA)
  DECODE_UNKNOWN_BITS
  AcDbAnnotScaleObjectContextData_fields;
  AcDbDimensionObjectContextData_fields;
  SUBCLASS (AcDbRadialDimensionObjectContextData)
  FIELD_3BD (first_arc_pt, 11);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (RADIMLGOBJECTCONTEXTDATA)
  DECODE_UNKNOWN_BITS
  AcDbAnnotScaleObjectContextData_fields;
  AcDbDimensionObjectContextData_fields;
  SUBCLASS (AcDbRadialDimensionLargeObjectContextData)
  FIELD_3BD (ovr_center, 12);
  FIELD_3BD (jog_point, 13);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (MLEADEROBJECTCONTEXTDATA)
  DECODE_UNKNOWN_BITS
  AcDbAnnotScaleObjectContextData_fields;
  SUBCLASS (AcDbMLeaderObjectContextData)
  // ?? ...
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (MTEXTATTRIBUTEOBJECTCONTEXTDATA)
  DECODE_UNKNOWN_BITS
  AcDbAnnotScaleObjectContextData_fields;
  AcDbTextObjectContextData_fields;
  SUBCLASS (AcDbMTextAttributeObjectContextData)
  FIELD_B (enable_context, 290);
  if (FIELD_VALUE (enable_context))
    {
      /*
      DECODER {
        dwg_add_object (dwg);
        _obj->context = &dwg->object[dwg->num_objects - 1];
        dwg_setup_SCALE (_obj->context);
      } */
      DXF { VALUE_TFF ( "Embedded Object", 101 ); }
      //CALL_ENTITY (SCALE, _obj->context);
    }
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// (varies) TODO
DWG_OBJECT (DATATABLE)
  DECODE_UNKNOWN_BITS
#ifdef IS_DXF
  UNTIL (R_2000) {
    SUBCLASS (ACDBDATATABLE)
  } LATER_VERSIONS {
    SUBCLASS (AcDbDataTable)
  }
#endif
  DEBUG_HERE_OBJ
  FIELD_BS (flags, 70);
  FIELD_BL (num_cols, 90);
  FIELD_BL (num_rows, 91);
  FIELD_T (table_name, 1);
  REPEAT (num_cols, cols, Dwg_DATATABLE_column)
  REPEAT_BLOCK
      SUB_FIELD_BL (cols[rcount1],type, 92);
      SUB_FIELD_T (cols[rcount1],text, 2);

      REPEAT2 (num_rows, cols[rcount1].rows, Dwg_DATATABLE_row) //CellType?
      REPEAT_BLOCK
          // almost like Dwg_TABLE_value
          //switch case 1:
          SUB_FIELD_BL (cols[rcount1].rows[rcount2],value.data_long, 93);
          //switch case 2:
          SUB_FIELD_BD (cols[rcount1].rows[rcount2],value.data_double, 40);
          //switch case 3:
          SUB_FIELD_T (cols[rcount1].rows[rcount2],value.data_string, 3);
          SET_PARENT (cols[rcount1].rows[rcount2], &_obj->cols[rcount1]);
      END_REPEAT_BLOCK
      END_REPEAT (cols[rcount1].rows)
      SET_PARENT_OBJ (cols[rcount1]);
  END_REPEAT_BLOCK
  END_REPEAT (cols)
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

#endif /* DEBUG_CLASS || IS_FREE */

DWG_OBJECT (DATALINK)
  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbDataLink)
  FIELD_T (data_adapter, 1);
  FIELD_T (description, 300);
  FIELD_T (tooltip, 301);
  FIELD_T (connection_string, 302);
  FIELD_BL (option, 90); // 2
  FIELD_BL (update_option, 91); // 1179649
  FIELD_BL (bl92, 92); // 1
  FIELD_BS (year, 170);
  FIELD_BS (month, 171);
  FIELD_BS (day, 172);
  FIELD_BS (hour, 173);
  FIELD_BS (minute, 174);
  FIELD_BS (seconds, 175);
  FIELD_BS (msec, 176);
  FIELD_BS (path_option, 177); // 1
  FIELD_BL (bl93, 93); // 0
  FIELD_T (update_status, 304);
  FIELD_BL (num_customdata, 94); // 2
  DXF { VALUE_TFF ("CUSTOMDATA", 305); }
  DEBUG_HERE_OBJ
  DXF { VALUE_TFF ("DATAMAP_BEGIN", 1); }
  REPEAT (num_customdata, customdata, Dwg_DATALINK_customdata)
  REPEAT_BLOCK
      SUB_FIELD_HANDLE (customdata[rcount1],target, DWG_HDL_HARDOWN, 330);
      // ACEXCEL_UPDATEOPTIONS, ACEXCEL_CONNECTION_STRING, ACEXCEL_SOURCEDATA
      SUB_FIELD_T (customdata[rcount1],text, 304);
      SET_PARENT_OBJ (customdata[rcount1]);
  END_REPEAT_BLOCK
  END_REPEAT (customdata)
  DXF { VALUE_TFF ("DATAMAP_END", 309); }
  FIELD_HANDLE (hardowner, DWG_HDL_HARDOWN, 360);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (ALDIMOBJECTCONTEXTDATA)
  DECODE_UNKNOWN_BITS
  AcDbAnnotScaleObjectContextData_fields;
  AcDbDimensionObjectContextData_fields;
  SUBCLASS (AcDbAlignedDimensionObjectContextData)
  FIELD_3BD (dimline_pt, 11);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (MTEXTOBJECTCONTEXTDATA)
  DECODE_UNKNOWN_BITS
  AcDbAnnotScaleObjectContextData_fields;
  SUBCLASS (AcDbMTextObjectContextData)
  FIELD_BL (attachment, 70);
  // From MTEXT Embedded object
  FIELD_3BD (x_axis_dir, 11);
  FIELD_3BD (ins_pt, 10); // ODA bug
  FIELD_BD (rect_width, 40);
  FIELD_BD (rect_height, 41);
  FIELD_BD (extents_width, 42);
  FIELD_BD (extents_height, 43);

  FIELD_BL (column_type, 71);
  VALUEOUTOFBOUNDS (column_type, 2)
  if (FIELD_VALUE (column_type))
    {
      FIELD_BL (num_column_heights, 72);
      FIELD_BD (column_width, 44);
      FIELD_BD (gutter, 45);
      FIELD_B (auto_height, 73);
      FIELD_B (flow_reversed, 74);
      if (!FIELD_VALUE (auto_height) && FIELD_VALUE (column_type) == 2)
        {
          FIELD_VECTOR (column_heights, BD, num_column_heights, 46);
        }
    }
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (TEXTOBJECTCONTEXTDATA)
  DECODE_UNKNOWN_BITS
  AcDbAnnotScaleObjectContextData_fields;
  AcDbTextObjectContextData_fields;
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (BLKREFOBJECTCONTEXTDATA)
  DECODE_UNKNOWN_BITS
  AcDbAnnotScaleObjectContextData_fields;
  SUBCLASS (AcDbBlkrefObjectContextData);
  FIELD_BD (rotation, 50)
  FIELD_3BD (ins_pt, 10);
  FIELD_3BD_1 (scale_factor, 42);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (LEADEROBJECTCONTEXTDATA)
  DECODE_UNKNOWN_BITS
  AcDbAnnotScaleObjectContextData_fields;
  SUBCLASS (AcDbLeaderObjectContextData)
  FIELD_BL (num_points, 70); /* 3 */
  FIELD_3DPOINT_VECTOR (points, num_points, 10);
  FIELD_3DPOINT (x_direction, 11);
  FIELD_B (b290, 290); /* 1 */
  FIELD_3DPOINT (inspt_offset, 12);
  FIELD_3DPOINT (endptproj, 13);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// TOLERANCE
DWG_OBJECT (FCFOBJECTCONTEXTDATA)
  DECODE_UNKNOWN_BITS
  AcDbAnnotScaleObjectContextData_fields;
  SUBCLASS (AcDbFcfObjectContextData)
  FIELD_3BD (location, 10);
  FIELD_3BD (horiz_dir, 11);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// stable and needed for the NOD
DWG_OBJECT (DETAILVIEWSTYLE)
  SUBCLASS (AcDbModelDocViewStyle)
  FIELD_BS (mdoc_class_version, 70); // 0
  FIELD_T (desc, 3);
  FIELD_B (is_modified_for_recompute, 290);
  SINCE (R_2018) {
    FIELD_T (display_name, 300);
    FIELD_BL (viewstyle_flags, 90);
  }

  SUBCLASS (AcDbDetailViewStyle)
  FIELD_BS (class_version, 70); // 0
  DXF { VALUE_BS (0, 71); }     // version_minor?
  FIELD_BL (flags, 90);
  DXF { VALUE_BS (1, 71); }
  FIELD_HANDLE (identifier_style, 5, 340); // textstyle
  DXF { SINCE (R_2004) {
    FIELD_CMC (identifier_color, 62); // in dxf all colors only r2004+
  }} else {
    FIELD_CMC (identifier_color, 62);
  }
  FIELD_BD (identifier_height, 40); // 5.0
  DXF {
    FIELD_HANDLE (arrow_symbol, 5, 340);
    SINCE (R_2004) {
      FIELD_CMC (arrow_symbol_color, 62);
    }
    FIELD_BD (arrow_symbol_size, 40);
  }
  FIELD_T (identifier_exclude_characters, 300);
  FIELD_BD (identifier_offset, 40);
  FIELD_RC (identifier_placement, 280);
  FIELD_HANDLE (arrow_symbol, 5, 0);
  DXF { SINCE (R_2004) {
    FIELD_CMC (arrow_symbol_color, 0);
  }} else {
    FIELD_CMC (arrow_symbol_color, 0);
  }
  FIELD_BD (arrow_symbol_size, 0);
  DXF { VALUE_BS (2, 71); }
  FIELD_HANDLE (boundary_ltype, 5, 340); // ltype
  FIELD_BLd (boundary_linewt, 90);
  DXF { SINCE (R_2004) {
    FIELD_CMC (boundary_line_color, 62);
  }} else {
    FIELD_CMC (boundary_line_color, 62);
  }
  DXF { VALUE_BS (3, 71); }
  FIELD_HANDLE (viewlabel_text_style, 5, 340); // textstyle
  DXF { SINCE (R_2004) {
    FIELD_CMC (viewlabel_text_color, 62);
  }} else {
    FIELD_CMC (viewlabel_text_color, 62);
  }
  FIELD_BD (viewlabel_text_height, 40);
  FIELD_BL (viewlabel_attachment, 90);
  FIELD_BD (viewlabel_offset, 40);
  FIELD_BL (viewlabel_alignment, 90);
  FIELD_T (viewlabel_pattern, 300);
  DXF { VALUE_BS (4, 71); }
  FIELD_HANDLE (connection_ltype, 5, 340); // ltype
  FIELD_BLd (connection_linewt, 90);
  DXF { SINCE (R_2004) {
    FIELD_CMC (connection_line_color, 62);
  }} else {
    FIELD_CMC (connection_line_color, 62);
  }
  FIELD_HANDLE (borderline_ltype, 5, 340);
  FIELD_BLd (borderline_linewt, 90);
  DXF { SINCE (R_2004) {
    FIELD_CMC (borderline_color, 62);
  }} else {
    FIELD_CMC (borderline_color, 62);
  }
  FIELD_RC (model_edge, 280); // type, origin, direction
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// stable and needed for the NOD
// subclass: VIEWSTYLE_ModelDoc => "AcDbModelDocViewStyle"
DWG_OBJECT (SECTIONVIEWSTYLE)
  SUBCLASS (AcDbModelDocViewStyle)
  FIELD_BS (mdoc_class_version, 70); // 0
  FIELD_T (desc, 3);
  FIELD_B (is_modified_for_recompute, 290);
  SINCE (R_2018) {
    FIELD_T (display_name, 300);
    FIELD_BL (viewstyle_flags, 90);
  }

  SUBCLASS (AcDbSectionViewStyle)
  FIELD_BS (class_version, 70); // 0
  DXF { VALUE_BS (0, 71); }     // version_minor?
  FIELD_BL (flags, 90); // 102
  DXF { VALUE_BS (1, 71); }
  FIELD_HANDLE (identifier_style, 5, 340); // textstyle
  DXF { SINCE (R_2004) {
    FIELD_CMC (identifier_color, 62); // in dxf all colors only r2004+
  }} else {
    FIELD_CMC (identifier_color, 62);
  }
  FIELD_BD (identifier_height, 40); // 5.0
  FIELD_HANDLE (arrow_start_symbol, 5, 340);
  FIELD_HANDLE (arrow_end_symbol, 5, 340);
  DXF { SINCE (R_2004) {
    FIELD_CMC (arrow_symbol_color, 62);
  }} else {
    FIELD_CMC (arrow_symbol_color, 62);
  }
  FIELD_BD (arrow_symbol_size, 40);
  FIELD_T (identifier_exclude_characters, 300); // I, O, Q, S, X, Z
  FIELD_BD (arrow_symbol_extension_length, 40);
  DXF {
    FIELD_BLd (identifier_position, 90);
    FIELD_BD (identifier_offset, 40);
    FIELD_BLd (arrow_position, 90);
    VALUE_BS (2, 71);
  }
  FIELD_HANDLE (plane_ltype, 5, 340); // ltype
  FIELD_BLd (plane_linewt, 90);
  DXF { SINCE (R_2004) {
    FIELD_CMC (plane_line_color, 62);
  }} else {
    FIELD_CMC (plane_line_color, 62);
  }
  FIELD_HANDLE (bend_ltype, 5, 340); // ltype
  FIELD_BLd (bend_linewt, 90);
  DXF { SINCE (R_2004) {
    FIELD_CMC (bend_line_color, 62);
  }} else {
    FIELD_CMC (bend_line_color, 62);
  }
  FIELD_BD (bend_line_length, 40);
  DXF {
    FIELD_BD (end_line_overshoot, 40);
  }
  FIELD_BD (end_line_length, 40);
  DXF { VALUE_BS (3, 71); }
  FIELD_HANDLE (viewlabel_text_style, 5, 340); // textstyle
  DXF { SINCE (R_2004) {
    FIELD_CMC (viewlabel_text_color, 62);
  }} else {
    FIELD_CMC (viewlabel_text_color, 62);
  }
  FIELD_BD (viewlabel_text_height, 40);
  FIELD_BL (viewlabel_attachment, 90);
  FIELD_BD (viewlabel_offset, 40); // 5.0
  FIELD_BL (viewlabel_alignment, 90);
  FIELD_T (viewlabel_pattern, 300);
  DXF { VALUE_BS (4, 71); }
  DXF { SINCE (R_2004) {
    FIELD_CMC (hatch_color, 62);
    FIELD_CMC (hatch_bg_color, 62);
  }} else {
    FIELD_CMC (hatch_color, 62);
    FIELD_CMC (hatch_bg_color, 62);
  }
  FIELD_T (hatch_pattern, 300);
  FIELD_BD (hatch_scale, 40);
  FIELD_BLd (hatch_transparency, 90);
  FIELD_B (unknown_b1, 290);
  FIELD_B (unknown_b2, 290);
  FIELD_BLd (identifier_position, 0); // DXF see above
  FIELD_BD (identifier_offset, 0);
  FIELD_BLd (arrow_position, 0);
  FIELD_BD (end_line_overshoot, 0);
  FIELD_BL (num_hatch_angles, 90);
  FIELD_VECTOR (hatch_angles, BD, num_hatch_angles, 40);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (GRADIENT_BACKGROUND)
  SUBCLASS (AcDbGradientBackground)
  FIELD_BL (class_version, 90); /* 1 */
  // all rgb's with method c2
  FIELD_BLx (color_top, 90);
  FIELD_BLx (color_middle, 91);
  FIELD_BLx (color_bottom, 92);
  FIELD_BD (horizon, 140);
  FIELD_BD (height, 141);
  FIELD_BD (rotation, 142);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// DXF: GRADIENT_BACKGROUND with ODA! (looks like an ODA bug)
DWG_OBJECT (GROUND_PLANE_BACKGROUND)
  SUBCLASS (AcDbGroundPlaneBackground)
  FIELD_BL (class_version, 90); /* 1 */
  // all rgb's with method c2
  FIELD_BLx (color_sky_zenith, 90);
  FIELD_BLx (color_sky_horizon, 91);
  FIELD_BLx (color_underground_horizon, 92);
  FIELD_BLx (color_underground_azimuth, 93);
  FIELD_BLx (color_near, 94);
  FIELD_BLx (color_far, 95);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// DXF: RAPIDRTRENDERENVIRONMENT
DWG_OBJECT (IBL_BACKGROUND)
  SUBCLASS (AcDbIBLBackground)
  FIELD_BL (class_version, 90); /* 2 */
  FIELD_B (enable, 290);
  FIELD_T (name, 1);
  FIELD_BD (rotation, 40);
  FIELD_B (display_image, 290);
  FIELD_HANDLE (secondary_background, 5, 340);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (IMAGE_BACKGROUND)
  SUBCLASS (AcDbImageBackground)
  FIELD_BL (class_version, 90); /* 1 */
  FIELD_T (filename, 300);
  FIELD_B (fit_to_screen, 290);
  FIELD_B (maintain_aspect_ratio, 291);
  FIELD_B (use_tiling, 292);
  FIELD_2BD_1 (offset, 140);
  FIELD_2BD_1 (scale, 142);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (SKYLIGHT_BACKGROUND)
  SUBCLASS (AcDbSkyBackground); 
  FIELD_BL (class_version, 90); /* 1 */
  FIELD_HANDLE (sunid, 5, 340);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (SOLID_BACKGROUND)
  SUBCLASS (AcDbSolidBackground)
  FIELD_BL (class_version, 90); /* 1 */
  FIELD_BLx (color, 90);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END


// part of ACAD_RENDER_ENTRIES
DWG_OBJECT (RENDERENTRY)
  SUBCLASS (AcDbRenderEntry);
  FIELD_BL (class_version, 90);
  FIELD_T (image_file_name, 1);
  FIELD_T (preset_name, 1);
  FIELD_T (view_name, 1);
  FIELD_BL (dimension_x, 90);
  FIELD_BL (dimension_y, 90);
  FIELD_BS (start_year, 70);
  FIELD_BS (start_month, 70);
  FIELD_BS (start_day, 70);
  FIELD_BS (start_minute, 70);
  FIELD_BS (start_second, 70);
  FIELD_BS (start_msec, 70);
  FIELD_BD (render_time, 40);
  FIELD_BL (memory_amount, 90);
  FIELD_BL (material_count, 90);
  FIELD_BL (light_count, 90);
  FIELD_BL (triangle_count, 90);
  FIELD_BL (display_index, 90);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// officially documented, dbRender.h (ACAD_RENDER_ENVIRONMENT ??)
DWG_OBJECT (RENDERENVIRONMENT)
  SUBCLASS (AcDbRenderEnvironment)
  FIELD_BL (class_version, 90);     /*!< default 1 */
  FIELD_B (fog_enabled, 290);
  FIELD_B (fog_background_enabled, 290);
  FIELD_RC (fog_color_r, 280);
  FIELD_RC (fog_color_g, 280);
  FIELD_RC (fog_color_b, 280);
  FIELD_BD (fog_density_near, 40); /* default 100.0 (opaque fog) */
  FIELD_BD (fog_density_far, 40);
  FIELD_BD (fog_distance_near, 40); /* default 100.0 (at the far clipping plane) */
  FIELD_BD (fog_distance_far, 40);
  FIELD_B (environ_image_enabled, 290);
  FIELD_T (environ_image_filename, 1);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// officially documented, dbRender.h
DWG_OBJECT (RENDERGLOBAL)
  SUBCLASS (AcDbRenderGlobal)
  FIELD_BL (class_version, 90);     /*!< default 2 */
  FIELD_BL (procedure, 90);         /*!< 0 view, 1 crop, 2 selection */
  FIELD_BL (destination, 90);       /*!< 0 window, 1 viewport */
  FIELD_B (save_enabled, 290);
  FIELD_T (save_filename, 1);
  FIELD_BL (image_width, 90);
  FIELD_BL (image_height, 90);
  FIELD_B (predef_presets_first, 290);
  FIELD_B (highlevel_info, 290);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

#if defined (DEBUG_CLASSES) || defined (IS_FREE)

DWG_OBJECT (LAYOUTPRINTCONFIG)
  DECODE_UNKNOWN_BITS
  SUBCLASS (CAcLayoutPrintConfig)
  FIELD_BS (class_version, 0);
  DEBUG_HERE_OBJ
  FIELD_BS (flag, 93);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// AutoCAD Mechanical
DWG_OBJECT (ACMESCOPE)
  DECODE_UNKNOWN_BITS
  SUBCLASS (AcMeScope)
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (ACMECOMMANDHISTORY)
  DECODE_UNKNOWN_BITS
  SUBCLASS (AcMeCommandHistory)
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (ACMESTATEMGR)
  DECODE_UNKNOWN_BITS
  SUBCLASS (AcMeStateMgr)
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// CDocDataContainer
DWG_OBJECT (CSACDOCUMENTOPTIONS)
  DECODE_UNKNOWN_BITS
  //size 161
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (MOTIONPATH)
  FIELD_BL (class_version, 90);
  FIELD_HANDLE (camera_path, 5, 340);
  FIELD_HANDLE (target_path, 5, 340);
  FIELD_HANDLE (viewtable, 5, 340);
  FIELD_BS (frames, 90);
  FIELD_BS (frame_rate, 90);
  FIELD_B (corner_decel, 290);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// dxfname: ACDBCURVEPATH
DWG_OBJECT (CURVEPATH)
  //SUBCLASS (AcDbNamedPath)
  SUBCLASS (AcDbCurvePath)
  FIELD_BL (class_version, 90);
  FIELD_HANDLE (entity, 5, 340);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (POINTPATH)
  //SUBCLASS (AcDbNamedPath)
  SUBCLASS (AcDbPointPath)
  FIELD_BS (class_version, 90);
  FIELD_3BD (point, 10);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

//not in dxf
DWG_OBJECT (TVDEVICEPROPERTIES)
  FIELD_BLx (flags, 0); // bitmask
  FIELD_BS (max_regen_threads, 0);
  FIELD_BL (use_lut_palette, 0);
  FIELD_BLL (alt_hlt, 0);
  FIELD_BLL (alt_hltcolor, 0);
  FIELD_BLL (geom_shader_usage, 0);
  // ver > 3
  FIELD_BL (blending_mode, 0)
  //ver 2 or >4:
  FIELD_BD (antialiasing_level, 0)
  FIELD_BD (bd2, 0)
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// r2000+ expresstools. command ATEXT
DWG_ENTITY (ARCALIGNEDTEXT)
  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbArcAlignedText);
  DXF {
    FIELD_T (text_value, 1);
    FIELD_T (t2, 2);
    FIELD_T (t3, 3);
    FIELD_T (style, 7); // as name
    FIELD_3BD (center, 10);
    FIELD_BD (radius, 40);
    FIELD_D2T (xscale, 41);
    FIELD_D2T (text_size, 42);
    FIELD_D2T (char_spacing, 43);
    FIELD_D2T (offset_from_arc, 44);
    FIELD_D2T (right_offset, 45);
    FIELD_D2T (left_offset, 46);
    FIELD_BD (start_angle, 50);
    FIELD_BD (end_angle, 51);
    FIELD_BS (is_reverse, 70);
    FIELD_BS (text_direction, 71);
    FIELD_BS (alignment, 72);
    FIELD_BS (text_position, 73);
    FIELD_BS (font_19, 74);
    FIELD_BS (bs2, 75);
    FIELD_BS (is_underlined, 76);
    FIELD_BS (bs1, 77);
    FIELD_BS (font, 78);
    FIELD_BS (is_shx, 79);
    FIELD_BL (color, 90);
    FIELD_3BD (extrusion, 210);
    FIELD_B (wizard_flag, 280);
    FIELD_HANDLE (arc_handle, 5, 330);
  } else { // DWG
    FIELD_D2T (text_size, 42);
    FIELD_D2T (xscale, 41);
    FIELD_D2T (char_spacing, 43);
    FIELD_T (style, 7);
    FIELD_T (t2, 2);
    FIELD_T (t3, 3);
    FIELD_T (text_value, 1);
    FIELD_D2T (offset_from_arc, 44);
    FIELD_D2T (right_offset, 45);
    FIELD_D2T (left_offset, 46);
    FIELD_3BD (center, 10);
    FIELD_BD (radius, 40);
    FIELD_BD (start_angle, 50);
    FIELD_BD (end_angle, 51);
    FIELD_3BD (extrusion, 210);
    FIELD_BL (color, 90);
    FIELD_BS (bs1, 77);
    FIELD_BS (font, 78);
    FIELD_BS (is_shx, 79);
    FIELD_BS (font_19, 74);
    FIELD_BS (bs2, 75);
    FIELD_BS (is_underlined, 76);
    FIELD_BS (alignment, 72);
    FIELD_BS (is_reverse, 70);
    FIELD_BS (wizard_flag, 280);
    FIELD_BS (text_position, 73);
    FIELD_BS (text_direction, 71);
    FIELD_HANDLE (arc_handle, 5, 330);
  }
  COMMON_ENTITY_HANDLE_DATA;
DWG_ENTITY_END

// r2000+ expresstools. Remote Text
DWG_ENTITY (RTEXT)
  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbRText);
  FIELD_3BD (pt, 10);
  DXF  {
    FIELD_BE (extrusion, 210);
  }
  else { FIELD_3DPOINT (extrusion, 210); }
  FIELD_BD (rotation, 50);
  FIELD_BD (height, 50);
  DXF {
    FIELD_HANDLE (style, 5, 7);
  }
  FIELD_BS (flags, 70);
  FIELD_T (text_value, 1); // TODO can be split into mult. remote text src or diesel expr
  COMMON_ENTITY_HANDLE_DATA;
  FIELD_HANDLE (style, 5, 0);
DWG_ENTITY_END

DWG_OBJECT (ASSOCEDGEACTIONPARAM)
  DECODE_UNKNOWN_BITS
  AcDbAssocActionParam_fields;
  SUBCLASS (AcDbAssocSingleDependencyActionParam);
  FIELD_BL (asdap_class_version, 90);
  FIELD_HANDLE (dep, 4, 330);
  SUBCLASS (AcDbAssocEdgeActionParam);
  FIELD_BL (class_version, 90);
  FIELD_HANDLE (param, 3, 330);
  FIELD_B (has_action, 290);
  FIELD_BL (action_type, 90);
  switch (_obj->action_type)
    {
    case 11:
      CALL_SUBCURVE (subent, ARC);
      break;
    case 17:
      CALL_SUBCURVE (subent, ELLIPSE);
      break;
    case 19:
      CALL_SUBCURVE (subent, LINE);
      break;
    case 23:
      CALL_SUBCURVE (subent, LINESEG3D);
      break;
    case 42:
      CALL_SUBCURVE (subent, NURB3D);
      break;
    case 27:
      CALL_SUBCURVE (subent, CURVE3D);
      break;
    default:
      LOG_ERROR ("Unknown ASSOCEDGEACTIONPARAM.action_type %d", _obj->action_type);
    }
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END


DWG_ENTITY (ALIGNMENTPARAMETERENTITY)
  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbBlockAlignmentParameterEntity)
  COMMON_ENTITY_HANDLE_DATA;
DWG_ENTITY_END

DWG_ENTITY (BASEPOINTPARAMETERENTITY)
  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbBlockBasepointParameterEntity)
  COMMON_ENTITY_HANDLE_DATA;
DWG_ENTITY_END

DWG_ENTITY (FLIPPARAMETERENTITY)
  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbBlockFlipParameterEntity)
  COMMON_ENTITY_HANDLE_DATA;
DWG_ENTITY_END

DWG_ENTITY (LINEARPARAMETERENTITY)
  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbBlockLinearParameterEntity)
  COMMON_ENTITY_HANDLE_DATA;
DWG_ENTITY_END

DWG_ENTITY (POINTPARAMETERENTITY)
  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbBlockPointParameterEntity)
  COMMON_ENTITY_HANDLE_DATA;
DWG_ENTITY_END

DWG_ENTITY (ROTATIONPARAMETERENTITY)
  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbBlockRotationParameterEntity)
  COMMON_ENTITY_HANDLE_DATA;
DWG_ENTITY_END

#if 0
DWG_ENTITY (BLOCKANGULARCONSTRAINTPARAMETERENTITY)
  DECODE_UNKNOWN_BITS
  AcDbBlockConstraintParameter_fields;
  SUBCLASS (AcDbBlockAngularConstraintParameterEntity)
  FIELD_3BD (center_pt, 0);
  FIELD_3BD (label_pt, 0);
  FIELD_T (expr_name, 305); // A copy of the EvalExpr.name.
  FIELD_T (expr_description, 306);
  DXF {
    FIELD_3BD (center_pt, 1011);
    FIELD_3BD (label_pt, 1012);
  }
  FIELD_BD (angle, 140); // offset is the result
  FIELD_B (orientation_on_both_grips, 280);
  // 0x60,0x8d,0xaf,0x133
  AcDbBlockParamValueSet_fields (value_set,96,128,175,307);
  COMMON_ENTITY_HANDLE_DATA;
DWG_ENTITY_END
#endif

DWG_ENTITY (VISIBILITYPARAMETERENTITY)
  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbBlockVisibilityParameterEntity)
  COMMON_ENTITY_HANDLE_DATA;
DWG_ENTITY_END

DWG_ENTITY (FLIPGRIPENTITY)
  DECODE_UNKNOWN_BITS
  //BLOCKGRIP_fields
  SUBCLASS (AcDbBlockFlipGripEntity)
  COMMON_ENTITY_HANDLE_DATA;
DWG_ENTITY_END

// ASME_PIPE_BLOCK, Civil_Slope_Marker_Imperial, Odkaz_sour_YX_situace
DWG_ENTITY (LINEARGRIPENTITY)
  DECODE_UNKNOWN_BITS
  //BLOCKGRIP_fields
  SUBCLASS (AcDbBlockLinearGripEntity)
  COMMON_ENTITY_HANDLE_DATA;
DWG_ENTITY_END

DWG_ENTITY (POLARGRIPENTITY)
  DECODE_UNKNOWN_BITS
  //BLOCKGRIP_fields
  SUBCLASS (AcDbBlockPolarGripEntity)
  COMMON_ENTITY_HANDLE_DATA;
DWG_ENTITY_END

DWG_ENTITY (ROTATIONGRIPENTITY)
  DECODE_UNKNOWN_BITS
  //BLOCKGRIP_fields
  SUBCLASS (AcDbBlockRotationGripEntity)
  COMMON_ENTITY_HANDLE_DATA;
DWG_ENTITY_END

DWG_ENTITY (VISIBILITYGRIPENTITY)
  DECODE_UNKNOWN_BITS
  //BLOCKGRIP_fields
  SUBCLASS (AcDbBlockVisibilityGripEntity)
  COMMON_ENTITY_HANDLE_DATA;
DWG_ENTITY_END

DWG_ENTITY (XYGRIPENTITY)
  DECODE_UNKNOWN_BITS
  //BLOCKGRIP_fields
  SUBCLASS (AcDbBlockXYGripEntity)
  COMMON_ENTITY_HANDLE_DATA;
DWG_ENTITY_END

DWG_ENTITY (XYPARAMETERENTITY)
  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbBlockXYParameterEntity)
  COMMON_ENTITY_HANDLE_DATA;
DWG_ENTITY_END

// dxf: ACDB_DYNAMICBLOCKPROXYNODE
// subclass only?
DWG_OBJECT (DYNAMICBLOCKPROXYNODE)
  DECODE_UNKNOWN_BITS
  AcDbEvalExpr_fields;
  //SUBCLASS (AcDbDynamicBlockProxyNode)
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

#endif /* DEBUG_CLASSES || IS_FREE */
/*=============================================================================*/

DWG_ENTITY (POINTCLOUD)
  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbPointCloud)
  FIELD_BS (class_version, 70);
  FIELD_3BD (origin, 10);
  FIELD_T (saved_filename, 1)
  FIELD_BL (num_source_files, 90)
  if (!FIELD_VALUE (num_source_files))
    {
      FIELD_3BD (extents_min, 11);
      FIELD_3BD (extents_max, 12);
      FIELD_RLL (numpoints, 92);
      FIELD_T (ucs_name, 3);
      FIELD_3BD (ucs_origin, 13);
      FIELD_3BD (ucs_x_dir, 210);
      FIELD_3BD (ucs_y_dir, 211);
      FIELD_3BD (ucs_z_dir, 212);
      SINCE (R_2013) {
        FIELD_HANDLE (pointclouddef, 5, 330);
        FIELD_HANDLE (reactor, 3, 360);
        FIELD_B (show_intensity, 0);
        FIELD_BS (intensity_scheme, 71);
        SUB_FIELD_BD (intensity_style, min_intensity, 40);
        SUB_FIELD_BD (intensity_style, max_intensity, 41);
        SUB_FIELD_BD (intensity_style, intensity_low_treshold, 42);
        SUB_FIELD_BD (intensity_style, intensity_high_treshold, 43);
        FIELD_B (show_clipping, 0);
        FIELD_BL (num_clippings, 0);
        REPEAT (num_clippings, clippings, Dwg_POINTCLOUD_Clippings)
        REPEAT_BLOCK
          SUB_FIELD_B (clippings[rcount1], is_inverted, 0);
          SUB_FIELD_BS (clippings[rcount1], type, 0);
          if (_obj->clippings[rcount1].type == 3) // polygon
            {
              SUB_FIELD_BL (clippings[rcount1], num_vertices, 0)
            }
          else // box or rectangle
            _obj->clippings[rcount1].num_vertices = 2;
          SUB_FIELD_2RD_VECTOR (clippings[rcount1], vertices, num_vertices, 0);
          if (_obj->clippings[rcount1].type == 1) // box
            {
              SUB_FIELD_BD (clippings[rcount1], z_min, 0);
              SUB_FIELD_BD (clippings[rcount1], z_max, 0);
            }
          SET_PARENT_OBJ (clippings[rcount1]);
        END_REPEAT_BLOCK
        END_REPEAT (clippings)
      }
    }
  FIELD_VECTOR_T (source_files, T, num_source_files, 2)
  COMMON_ENTITY_HANDLE_DATA;
DWG_ENTITY_END

DWG_ENTITY (POINTCLOUDEX)
  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbPointCloud)
  FIELD_BS (class_version, 70); // 1
  FIELD_3BD (extents_min, 10);
  FIELD_3BD (extents_max, 11);
  FIELD_3BD (ucs_origin, 12);
  FIELD_3BD (ucs_x_dir, 210);
  FIELD_3BD (ucs_y_dir, 211);
  FIELD_3BD (ucs_z_dir, 212);

  FIELD_B (is_locked, 290);
  FIELD_HANDLE (pointclouddefex, 5, 330);
  FIELD_HANDLE (reactor, 3, 360);
  FIELD_T (name, 1)
  FIELD_B (show_intensity, 291);

  DXF {
    FIELD_BS (stylization_type, 71);
    FIELD_T (intensity_colorscheme, 1);
    FIELD_T (cur_colorscheme, 1);
    FIELD_T (classification_colorscheme, 1);
    FIELD_BD (elevation_min, 40);
    FIELD_BD (elevation_max, 41);
    FIELD_BL (intensity_min, 90);
    FIELD_BL (intensity_max, 91);
    FIELD_BS (intensity_out_of_range_behavior, 71);
    FIELD_BS (elevation_out_of_range_behavior, 72);
    FIELD_B (elevation_apply_to_fixed_range, 292);
    FIELD_B (intensity_as_gradient, 293);
    FIELD_B (elevation_as_gradient, 294);
    FIELD_B (show_cropping, 295);
    FIELD_BL (num_croppings, 92);
    if (!_obj->num_croppings)
      {
        FIELD_BL (unknown_bl0, 93);
        FIELD_BL (unknown_bl1, 93);
      }
  }
  else { // DWG
    FIELD_B (show_cropping, 295);
    FIELD_BL (num_croppings, 92);
    if (!_obj->num_croppings)
      {
        FIELD_BL (unknown_bl0, 93);
        FIELD_BL (unknown_bl1, 93);
        FIELD_BS (stylization_type, 71);
        FIELD_T (intensity_colorscheme, 1);
        FIELD_T (cur_colorscheme, 1);
        FIELD_T (classification_colorscheme, 1);
        FIELD_BD (elevation_min, 40);
        FIELD_BD (elevation_max, 41);
        FIELD_BL (intensity_min, 90);
        FIELD_BL (intensity_max, 91);
        FIELD_BS (intensity_out_of_range_behavior, 71);
        FIELD_BS (elevation_out_of_range_behavior, 72);
        FIELD_B (elevation_apply_to_fixed_range, 292);
        FIELD_B (intensity_as_gradient, 293);
        FIELD_B (elevation_as_gradient, 294);
      }
  }
  REPEAT (num_croppings, croppings, Dwg_POINTCLOUDEX_Croppings)
  REPEAT_BLOCK
      SUB_FIELD_BS (croppings[rcount1], type, 280);
      SUB_FIELD_B (croppings[rcount1], is_inside, 290);
      SUB_FIELD_B (croppings[rcount1], is_inverted, 290);
      SUB_FIELD_3BD (croppings[rcount1], crop_plane, 13);
      SUB_FIELD_3BD (croppings[rcount1], crop_x_dir, 213);
      SUB_FIELD_3BD (croppings[rcount1], crop_y_dir, 213);
      SUB_FIELD_BL (croppings[rcount1], num_pts, 93)
      SUB_FIELD_3BD_VECTOR (croppings[rcount1], pts, num_pts, 13);
      SET_PARENT_OBJ (croppings[rcount1]);
  END_REPEAT_BLOCK
  END_REPEAT (croppings)
  COMMON_ENTITY_HANDLE_DATA;
DWG_ENTITY_END

/* (none/5) pre r2.1 only */
DWG_ENTITY (REPEAT)
  //COMMON_ENTITY_HANDLE_DATA;
DWG_ENTITY_END

/* (none/6) pre r2.1 only */
DWG_ENTITY (ENDREP)
  FIELD_RS (numcols, 70);
  FIELD_RS (numrows, 71);
  FIELD_RD (colspacing, 40);
  FIELD_RD (rowspacing, 41);
  //COMMON_ENTITY_HANDLE_DATA;
DWG_ENTITY_END

/* (none/10) pre r2.0 only */
DWG_ENTITY (LOAD)
  FIELD_TV (file_name, 1);
  //COMMON_ENTITY_HANDLE_DATA;
DWG_ENTITY_END

/* (none/18) R_2_0b-R_13b1 */
DWG_ENTITY (JUMP)
  FIELD_RLx (jump_address_raw, 0);
  DECODER {
    size_t len;
    BITCODE_TF trailing;
    _obj->jump_address = _obj->jump_address_raw;
    _obj->jump_entity_section = DWG_ENTITY_SECTION;
    if (_obj->jump_address_raw > 0xffffff)
      {
        _obj->jump_address &= 0xffffff;
        switch ((_obj->jump_address_raw & 0xff000000) >> 24)
          {
          case 0:
            break;
          case 0x40:
            _obj->jump_entity_section = DWG_BLOCKS_SECTION;
            break;
          case 0x80:
            _obj->jump_entity_section = DWG_EXTRA_SECTION;
            break;
          default:
            LOG_ERROR ("Invalid jump_entity_section %x ignored",
                       (_obj->jump_address_raw & 0xff000000) >> 24)
          }
      }
    LOG_TRACE ("jump_entity_section: ");
    switch (_obj->jump_entity_section)
      {
      case DWG_ENTITY_SECTION:
        LOG_TRACE ("DWG_ENTITY_SECTION");
        break;
      case DWG_BLOCKS_SECTION:
        LOG_TRACE ("DWG_BLOCKS_SECTION");
        break;
      case DWG_EXTRA_SECTION:
        LOG_TRACE ("DWG_EXTRA_SECTION");
        break;
      default:
        LOG_ERROR ("Unknown entity section");
      }
    LOG_TRACE ("\n");
    LOG_TRACE ("jump_address: " FORMAT_RLx "\n", _obj->jump_address);

    /* crc16 in jump entity is before the end of entity often */
    if (dat->version >= R_11b1)
      {
        if (!bit_check_CRC (dat, obj->address, 0xC0C1))
          error |= DWG_ERR_WRONGCRC;
      }

    /* log trailing data */
    if (dat->byte < obj->address + obj->size)
      {
        len = obj->address + obj->size - dat->byte;
        trailing = bit_read_TF (dat, len);
        LOG_TRACE ("trailing (%zu): ", len);
        LOG_TRACE_TF (trailing, len);
        free (trailing);
      }
  }
DWG_ENTITY_END

DWG_OBJECT (POINTCLOUDDEF)
  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbPointCloudDef)
  FIELD_BL (class_version, 90);
  FIELD_T (source_filename, 1);
  FIELD_B (is_loaded, 280);
  DXF {
    UNTIL (R_2010) {
      VALUE_RL ((BITCODE_RL)(_obj->numpoints >> 32), 91);
      VALUE_RL ((BITCODE_RL)(_obj->numpoints & 0xffffffff), 92);
    } LATER_VERSIONS {
      FIELD_RLL (numpoints, 160);
    }
  }
  else {
    FIELD_RLL (numpoints, 160);
  }
  FIELD_3BD (extents_min, 10);
  FIELD_3BD (extents_max, 11);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (POINTCLOUDDEFEX)
  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbPointCloudDefEx)
  FIELD_BL (class_version, 90);
  FIELD_T (source_filename, 1);
  FIELD_B (is_loaded, 280);
  FIELD_RLL (numpoints, 160);
  FIELD_3BD (extents_min, 10);
  FIELD_3BD (extents_max, 11);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (POINTCLOUDDEF_REACTOR)
  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbPointCloudDefReactor)
  FIELD_BL (class_version, 90);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (POINTCLOUDDEF_REACTOR_EX)
  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbPointCloudDefReactorEx)
  FIELD_BL (class_version, 90);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (POINTCLOUDCOLORMAP)
  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbPointCloudColorMap)
  FIELD_BS (class_version, 70);
  FIELD_T (def_intensity_colorscheme, 1);
  FIELD_T (def_elevation_colorscheme, 1);
  FIELD_T (def_classification_colorscheme, 1);

  FIELD_BL (num_colorramps, 90);
  REPEAT (num_colorramps, colorramps, Dwg_POINTCLOUDCOLORMAP_Ramp)
  REPEAT_BLOCK
    #define ramp colorramps[rcount1]
    SUB_FIELD_BS (ramp, class_version, 70);
    SUB_FIELD_BL (ramp, num_ramps, 90);
    REPEAT2 (ramp.num_ramps, colorramps[rcount1].ramps, Dwg_ColorRamp)
    REPEAT_BLOCK
      if (1) {
        SUB_FIELD_T (ramp.ramps[rcount2], colorscheme, 1);
      } else {
        SUB_FIELD_BL (ramp.ramps[rcount2], unknown_bl, 91);
        SUB_FIELD_B (ramp.ramps[rcount2], unknown_b, 290);
      }
      SET_PARENT (ramp.ramps[rcount2], _obj->colorramps);
    END_REPEAT_BLOCK
    END_REPEAT (ramp.ramps)
    SET_PARENT_OBJ (ramp);
    #undef ramp
  END_REPEAT_BLOCK
  END_REPEAT (colorramps)

  FIELD_BL (num_classification_colorramps, 90);
  REPEAT (num_classification_colorramps, classification_colorramps, Dwg_POINTCLOUDCOLORMAP_Ramp)
  REPEAT_BLOCK
    #define ramp classification_colorramps[rcount1]
    SUB_FIELD_BS (ramp, class_version, 70);
    SUB_FIELD_BL (ramp, num_ramps, 90);
    REPEAT2 (ramp.num_ramps, ramp.ramps, Dwg_ColorRamp)
    REPEAT_BLOCK
      if (1) {
        SUB_FIELD_T (ramp.ramps[rcount2], colorscheme, 1);
      } else {
        SUB_FIELD_BL (ramp.ramps[rcount2], unknown_bl, 91);
        SUB_FIELD_B (ramp.ramps[rcount2], unknown_b, 290);
      }
      SET_PARENT (ramp.ramps[rcount2], _obj->classification_colorramps);
    END_REPEAT_BLOCK
    END_REPEAT (ramp.ramps)
    SET_PARENT_OBJ (ramp);
    #undef ramp
  END_REPEAT_BLOCK
  END_REPEAT (classification_colorramps)
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END


#define AcConstraintGroupNode_fields(node)                       \
  PRE (R_2013) {                                                 \
    DXF { VALUE_HANDLE (obj->tio.object->ownerhandle, xx, 4, 330); } \
  }                                                              \
  SUB_FIELD_BLd (node,nodeid, 90);                               \
  PRE (R_2013) {                                                 \
    SUB_FIELD_RC (node,status, 70);                              \
  }                                                              \
  SUB_FIELD_BL (node,num_connections, 90);                       \
  FIELD_VECTOR (node.connections, BL, node.num_connections, 90); \
  SINCE (R_2013) {                                               \
    SUB_FIELD_RC (node,status, 70);                              \
  }

#define AcGeomConstraint_fields(node)          \
  AcConstraintGroupNode_fields (node);         \
  SUBCLASS (AcGeomConstraint);                 \
  FIELD_BL (ownerid, 90);                      \
  FIELD_B (is_implied, 290);                   \
  FIELD_B (is_active, 290)

#define AcConstraintGeometry_fields(node)      \
  AcConstraintGroupNode_fields (node);         \
  SUBCLASS (AcConstraintGeometry);             \
  FIELD_HANDLE (geom_dep, 4, 330);             \
  FIELD_BL (nodeid, 90)

#define AcConstraintPoint_fields(node)         \
  AcGeomConstraint_fields (node);              \
  SUBCLASS (AcConstraintPoint);                \
  if (FIELD_VALUE (geom_dep))                  \
    FIELD_3BD (point, 10)

#define AcConstraintImplicitPoint_fields(node) \
  AcConstraintPoint_fields (node);             \
  /*SUBCLASS (AcConstraintImplicitPoint);*/    \
  FIELD_RC (point_type, 280);                  \
  FIELD_BLd (point_idx, 90); /* default: -1 */ \
  FIELD_BLd (curve_id, 90) /* default: 0 */

#define AcExplicitConstraint_fields(node)       \
  AcGeomConstraint_fields (node);               \
  SUBCLASS (AcExplicitConstraint)               \
  FIELD_HANDLE (value_dep, 3, 340);             \
  FIELD_HANDLE (dim_dep, 3, 340)

#define AcAngleConstraint_fields(node)          \
  AcExplicitConstraint_fields (node);           \
  SUBCLASS (AcAngleConstraint);                 \
  FIELD_RC (sector_type, 280)

#define AcParallelConstraint_fields(node)       \
  AcGeomConstraint_fields (node);               \
  SUBCLASS (AcParallelConstraint);              \
  FIELD_BLd (datum_line_idx, 90)

#define AcDistanceConstraint_fields(node)       \
  AcExplicitConstraint_fields (node);           \
  SUBCLASS (AcDistanceConstraint);              \
  FIELD_RC (dir_type, 280)                      \
  if (FIELD_VALUE (dir_type))                   \
    FIELD_3BD (distance, 10)

#define AcConstrainedEllipse_fields(node)       \
  AcGeomConstraint_fields (node);               \
  FIELD_3BD (center_pt, 10);                    \
  FIELD_3BD (sm_axis, 11);                      \
  FIELD_BD (axis_ratio, 40)

#define AcConstrainedBoundedEllipse_fields(node)\
  AcConstrainedEllipse_fields (node);           \
  FIELD_3BD (start_pt, 10);                     \
  FIELD_3BD (end_pt, 11)

#define AcConstrainedSpline_fields(node)        \
  AcConstraintGeometry_fields (node);           \
  FIELD_B (b70_1, 70);                          \
  FIELD_B (b70_2, 70);                          \
  FIELD_BL (bl90, 90);                          \
  FIELD_BD (bd40, 40); /* default 1e-10 */      \
  FIELD_BL (num_logical_knots, 90);             \
  FIELD_BL (num_physical_knots, 90);            \
  FIELD_BL (knots_grow_length, 90);             \
  FIELD_VECTOR (knots, BD, num_knots, 40);      \
  FIELD_BL (num_logical_knots, 90);             \
  FIELD_BL (num_physical_knots, 90);            \
  FIELD_BL (knots_grow_length, 90);             \
  FIELD_VECTOR (knots, BD, num_knots, 40);      \
  /* ... */

// Class AcDbAssoc2dConstraintGroup
// see https://help.autodesk.com/view/OARX/2018/ENU/?guid=OREF-AcDbAssoc2dConstraintGroup
DWG_OBJECT (ASSOC2DCONSTRAINTGROUP)
  DECODE_UNKNOWN_BITS
  AcDbAssocAction_fields;
  FIELD_BL (version, 90);       // 2
  FIELD_B (b1, 70);             // 0
  FIELD_3BD (workplane[0], 10); // 0,0,0
  FIELD_3BD (workplane[1], 10); // 1,0,0
  FIELD_3BD (workplane[2], 10); // 0,1,0
  FIELD_HANDLE (h1, 4, 360);
  FIELD_BL (num_actions, 90); // 2
  VALUEOUTOFBOUNDS (num_actions, 100)
  HANDLE_VECTOR (actions, num_actions, 4, 360);

  FIELD_BL (num_nodes, 90); // 9
  REPEAT (num_nodes, nodes, Dwg_CONSTRAINTGROUPNODE)
  REPEAT_BLOCK
      AcConstraintGroupNode_fields (nodes[rcount1]);
      SET_PARENT_OBJ (nodes[rcount1]);
  END_REPEAT_BLOCK
  END_REPEAT (nodes)
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (ASSOCVARIABLE)
  DECODE_UNKNOWN_BITS
  AcDbAssocAction_fields;
  SUBCLASS (AcDbAssocVariable)
  FIELD_BL (av_class_version, 90); /* 2 */
  FIELD_T (name, 1);
  FIELD_T (t58, 1);
  FIELD_T (evaluator, 1);
  FIELD_T (desc, 1);
  AcDbEvalVariant_fields(value);
  FIELD_B (has_t78, 290);
  FIELD_T (t78, 1);
  FIELD_B (b290, 290);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (ASSOCACTIONPARAM)
  DECODE_UNKNOWN_BITS
  AcDbAssocActionParam_fields;
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

#define AcDbAssocCompoundActionParam_fields \
  SUBCLASS (AcDbAssocCompoundActionParam); \
  FIELD_BS (class_version, 90); \
  FIELD_BS (bs1, 90); \
  FIELD_BL (num_params, 90); \
  HANDLE_VECTOR (params, num_params, 4, 360); \
  if (FIELD_VALUE (has_child_param)) /* FIXME */ \
    { \
      FIELD_BS (child_status, 90); \
      FIELD_BL (child_id, 90); \
      FIELD_HANDLE (child_param, 3, 330); \
    } \
  if (FIELD_VALUE (child_id)) \
    { \
      FIELD_HANDLE (h330_2, 3, 330); \
      FIELD_BL (bl2, 90); \
      FIELD_HANDLE (h330_3, 3, 330); \
    }

#define AcDbCompoundObjectId_fields                                           \
    SUBCLASS (AcDbCompoundObjectId)                                           \
    FIELD_B (has_object, 290);                                                \
    if (FIELD_VALUE (has_object))                                             \
      {                                                                       \
        FIELD_T (name, 1);                                                    \
        CALL_SUBENT (_obj->object, 0)                                         \
      }

DWG_OBJECT (ASSOCCOMPOUNDACTIONPARAM)
  DECODE_UNKNOWN_BITS
  AcDbAssocActionParam_fields;
  AcDbAssocCompoundActionParam_fields;
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (ASSOCOSNAPPOINTREFACTIONPARAM)
  DECODE_UNKNOWN_BITS
  AcDbAssocActionParam_fields;
  AcDbAssocCompoundActionParam_fields;
  SUBCLASS (AcDbAssocPathActionParam);
  SUBCLASS (ACDBASSOCOSNAPPOINTREFACTIONPARAM);
  FIELD_BS (status, 90);
  FIELD_RC (osnap_mode, 90);
  FIELD_BD (param, 40);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (ASSOCPOINTREFACTIONPARAM)
  DECODE_UNKNOWN_BITS
  AcDbAssocActionParam_fields;
  AcDbAssocCompoundActionParam_fields;
  SUBCLASS (AcDbAssocPointRefActionParam);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// unused
#define AcDbAssocPathActionParam_fields(pap)              \
  SUBCLASS (AcDbAssocPathActionParam);                    \
  SUB_FIELD_BL (pap,status, 90);                          \
  SUB_FIELD_BS (pap,class_version, 90);                   \
  SUB_FIELD_BS (pap,bs1, 90);                             \
  SUB_FIELD_BL (pap,num_params, 90);                      \
  SUB_HANDLE_VECTOR (pap, params, num_params, 4, 360);    \
  if (FIELD_VALUE (pap.has_child_param)) {                \
    SUB_FIELD_BS (pap,child_status, 90);                  \
    SUB_FIELD_BL (pap,child_id, 90);                      \
    SUB_FIELD_HANDLE (pap,child_param, 3, 330);           \
    }                                                     \
  if (FIELD_VALUE (pap.child_id)) {                       \
    SUB_FIELD_HANDLE (pap,h330_2, 3, 330);                \
    SUB_FIELD_BL (pap,bl2, 90);                           \
    SUB_FIELD_HANDLE (pap,h330_3, 3, 330);                \
  }

DWG_OBJECT (ASSOCOBJECTACTIONPARAM)
  DECODE_UNKNOWN_BITS
  AcDbAssocActionParam_fields;
  SUBCLASS (AcDbAssocSingleDependencyActionParam);
  FIELD_BL (asdap_class_version, 90);
  FIELD_HANDLE (dep, 4, 330);
  SUBCLASS (AcDbAssocObjectActionParam);
  FIELD_BS (class_version, 90);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (ASSOCPATHACTIONPARAM)
  DECODE_UNKNOWN_BITS
  AcDbAssocActionParam_fields;
  AcDbAssocCompoundActionParam_fields;
  SUBCLASS (AcDbAssocPathActionParam);
  FIELD_BL (version, 90);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// not enough coverage
DWG_OBJECT (BLOCKPOLARPARAMETER)
  AcDbBlock2PtParameter_fields;
  SUBCLASS (AcDbBlockPolarParameter)
  FIELD_T (angle_name, 305);
  FIELD_T (angle_desc, 306);
  FIELD_T (distance_name, 305);
  FIELD_T (distance_desc, 306);
  FIELD_BD (offset, 140);
  AcDbBlockParamValueSet_fields (angle_value_set,96,142,175,410);
  AcDbBlockParamValueSet_fields (distance_value_set,97,146,176,309);
  //FIELD_3BD (base_angle_pt, 0);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (BLOCKPOLARGRIP)
  AcDbBlockGrip_fields;
  SUBCLASS (AcDbBlockPolarGrip)
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (ASSOCDIMDEPENDENCYBODY)
  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbAssocDependencyBody)
  FIELD_BS (adb_version, 90); // always 1
  SUBCLASS (AcDbImpAssocDimDependencyBodyBase)
  FIELD_BS (dimbase_version, 90); // always 1
  FIELD_T (name, 1);
  SUBCLASS (AdDbAssocDimDependencyBody)
  FIELD_BS (class_version, 90); // 1
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (BLOCKARRAYACTION)
  DECODE_UNKNOWN_BITS
  AcDbBlockAction_fields;
  SUBCLASS (AcDbBlockArrayAction)
  BlockAction_ConnectionPts (conn_pts, 0, 4, 92, 301);
  FIELD_BD (column_offset, 140);
  FIELD_BD (row_offset, 141);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (BLOCKANGULARCONSTRAINTPARAMETER)
  DECODE_UNKNOWN_BITS
  AcDbBlockConstraintParameter_fields;
  SUBCLASS (AcDbBlockAngularConstraintParameter)
  FIELD_3BD (center_pt, 0);
  FIELD_3BD (end_pt, 0);
  FIELD_T (expr_name, 305); // A copy of the EvalExpr.name
  FIELD_T (expr_description, 306);
  DXF {
    FIELD_3BD (center_pt, 1011);
    FIELD_3BD (end_pt, 1012);
  }
  FIELD_BD (angle, 140); // offset is the result
  FIELD_B (orientation_on_both_grips, 280);
  // 0x60,0x8d,0xaf,0x133
  AcDbBlockParamValueSet_fields (value_set,96,128,175,307);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// unused
#define AssocFaceActionParam_fields(fap)                  \
  AcDbAssocActionParam_fields;                            \
  SUBCLASS (AcDbAssocSingleDependencyActionParam);        \
  SUB_FIELD_BL (fap,asdap_class_version, 90);             \
  SUB_FIELD_HANDLE (fap, dep, 4, 330);                    \
  SUBCLASS (AcDbAssocFaceActionParam);                    \
  SUB_FIELD_BL (fap,class_version, 90);                   \
  SUB_FIELD_BL (fap,status, 90)

DWG_OBJECT (ASSOCFACEACTIONPARAM)
  DECODE_UNKNOWN_BITS
  AcDbAssocActionParam_fields;
  SUBCLASS (AcDbAssocSingleDependencyActionParam);
  FIELD_BL (asdap_class_version, 90);
  FIELD_HANDLE (dep, 4, 330);
  SUBCLASS (AcDbAssocFaceActionParam);
  FIELD_BL (class_version, 90);
  FIELD_BL (index, 90);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (ASSOCVERTEXACTIONPARAM)
  DECODE_UNKNOWN_BITS
  AcDbAssocActionParam_fields;
  SUBCLASS (AcDbAssocSingleDependencyActionParam);
  FIELD_BL (asdap_class_version, 90);
  FIELD_HANDLE (dep, 4, 330);
  SUBCLASS (AcDbAssocVertexActionParam);
  FIELD_BL (class_version, 90);
  FIELD_3BD (pt, 10);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (ASSOCASMBODYACTIONPARAM)
  DECODE_UNKNOWN_BITS
  AcDbAssocActionParam_fields;
  SUBCLASS (AcDbAssocSingleDependencyActionParam);
  FIELD_BL (asdap_class_version, 90);
  FIELD_HANDLE (dep, 4, 330);
  SUBCLASS (AcDbAssocAsmbodyActionParam);
  FIELD_BL (class_version, 90);
  ACTION_3DSOLID;
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

#define AcDbAssocArrayActionBody_fields                     \
  AcDbAssocActionBody_fields;                               \
  AcDbAssocParamBasedActionBody_fields (pab);               \
  SUBCLASS (AcDbAssocArrayActionBody);                      \
  FIELD_BL (aaab_version, 90);                              \
  FIELD_T (paramblock, 1);                                  \
  FIELD_VECTOR_N (transmatrix, BD, 16, 40)

#define AcDbAssocArrayItem_fields(item)                          \
  SUB_FIELD_BL (item, class_version, 90);                        \
  SUB_FIELD_VECTOR_INL (items[rcount1], itemloc, BL, 3, 90);     \
  SUB_FIELD_BLx (item, flags, 90);                               \
  /* TODO compare against default transmatrix */                 \
  if (FIELD_VALUE (item.is_default_transmatrix))                 \
    {                                                            \
      /* TODO x_dir computed from transmatrix */                 \
      SUB_FIELD_3BD (item, x_dir, 11);                           \
    }                                                            \
  else                                                           \
    {                                                            \
      SUB_FIELD_VECTOR_N (item, transmatrix, BD, 16, 40);        \
    }                                                            \
  if (FIELD_VALUE (item.flags) & 2)                              \
    {                                                            \
      SUB_FIELD_VECTOR_N (item, rel_transform, BD, 16, 40);      \
    }                                                            \
  if (FIELD_VALUE (item.has_h1))                                 \
    SUB_FIELD_HANDLE (item, h1, 4, 330);                         \
  if (FIELD_VALUE (item.flags) & 0x10)                           \
    SUB_FIELD_HANDLE (item, h2, 4, 330)

#define AcDbAssocArrayParameters_fields                          \
  FIELD_BL (aap_version, 90);                                    \
  FIELD_BL (num_items, 90);                                      \
  /* _obj->classname = "AcDbAssocArrayItem"; */                  \
  FIELD_T (classname, 1);                                        \
  REPEAT (num_items, items, Dwg_ASSOCARRAYITEM)                  \
  REPEAT_BLOCK                                                   \
      AcDbAssocArrayItem_fields (items[rcount1]);                \
      SET_PARENT (items[rcount1], (Dwg_Object_ASSOCARRAYPARAMETERS*)_obj); \
  END_REPEAT_BLOCK                                               \
  END_REPEAT (items)

#define AcDbAssocArrayCommonParameters_fields                    \
  SUBCLASS (AcDbAssocArrayCommonParameters)                      \
  AcDbAssocArrayParameters_fields;                               \
  FIELD_BL (numitems, 0);                                        \
  FIELD_BL (numrows, 0);                                         \
  FIELD_BL (numlevels, 0)

DWG_OBJECT (ASSOCARRAYMODIFYPARAMETERS)
  DECODE_UNKNOWN_BITS
  AcDbAssocArrayCommonParameters_fields;
  SUBCLASS (AcDbAssocArrayModifyParameters)
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (ASSOCARRAYPATHPARAMETERS)
  DECODE_UNKNOWN_BITS
  AcDbAssocArrayCommonParameters_fields;
  SUBCLASS (AcDbAssocArrayPathParameters)
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (ASSOCARRAYPOLARPARAMETERS)
  DECODE_UNKNOWN_BITS
  AcDbAssocArrayCommonParameters_fields;
  SUBCLASS (AcDbAssocArrayPolarParameters)
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (ASSOCARRAYRECTANGULARPARAMETERS)
  DECODE_UNKNOWN_BITS
  AcDbAssocArrayCommonParameters_fields;
  SUBCLASS (AcDbAssocArrayRectangularParameters)
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// subclasses only
#define AcDbAssocAsmBasedEntityPersSubentId_fields

DWG_OBJECT (BLOCKPARAMDEPENDENCYBODY)
  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbAssocDependencyBody)
  FIELD_BS (adb_version, 90); // always 1
  SUBCLASS (AcDbImpAssocDimDependencyBodyBase)
  FIELD_BS (dimbase_version, 90); // always 1
  FIELD_T (name, 1);
  SUBCLASS (AcDbBlockParameterDependencyBody)
  FIELD_BS (class_version, 90); // 0
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (BLOCKDIAMETRICCONSTRAINTPARAMETER)
  DECODE_UNKNOWN_BITS
  AcDbBlockConstraintParameter_fields;
  SUBCLASS (AcDbBlockDiametricConstraintParameter)
  FIELD_T (expr_name, 305); // A copy of the EvalExpr.name. expr being distance
  FIELD_T (expr_description, 306);
  FIELD_BD (distance, 140); /* -1.0 */
  AcDbBlockParamValueSet_fields (value_set,96,128,175,307);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (BLOCKRADIALCONSTRAINTPARAMETER)
  DECODE_UNKNOWN_BITS
  AcDbBlockConstraintParameter_fields;
  SUBCLASS (AcDbBlockRadialConstraintParameter)
  FIELD_T (expr_name, 305); // A copy of the EvalExpr.name. expr being distance
  FIELD_T (expr_description, 306);
  FIELD_BD (distance, 140);
  AcDbBlockParamValueSet_fields (value_set,96,128,175,307);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (BLOCKALIGNEDCONSTRAINTPARAMETER)
  AcDbBlockLinearConstraintParameter_fields;
  SUBCLASS (AcDbBlockAlignedConstraintParameter)
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (BLOCKLINEARCONSTRAINTPARAMETER)
  AcDbBlockLinearConstraintParameter_fields;
  //SUBCLASS (AcDbBlockLinearConstraintParameter)
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (BLOCKHORIZONTALCONSTRAINTPARAMETER)
  AcDbBlockLinearConstraintParameter_fields;
  SUBCLASS (AcDbBlockHorizontalConstraintParameter)
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (BLOCKVERTICALCONSTRAINTPARAMETER)
  AcDbBlockLinearConstraintParameter_fields;
  SUBCLASS (AcDbBlockVerticalConstraintParameter)
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (BLOCKLOOKUPACTION)
  DECODE_UNKNOWN_BITS
  AcDbBlockAction_fields;
  SUBCLASS (AcDbBlockLookupAction)
  ENCODER {
    FIELD_VALUE (numelems) = _obj->numrows * _obj->numcols;
  }
  FIELD_BL (numrows, 92);
  FIELD_BL (numcols, 93);
  DECODER {
    FIELD_VALUE (numelems) = _obj->numrows * _obj->numcols;
  }
  FIELD_VECTOR_T (exprs, T, numelems, 302)
  DXF { VALUE_TFF ("", 301); }
  REPEAT (numelems, lut, Dwg_BLOCKLOOKUPACTION_lut)
  REPEAT_BLOCK
    BlockAction_ConnectionPts (lut[rcount1].conn_pts, 0, 3, 94, 303);
    SUB_FIELD_B (lut[rcount1], b282, 282);
    SUB_FIELD_B (lut[rcount1], b281, 281);
    SET_PARENT_OBJ (lut[rcount1]);
  END_REPEAT_BLOCK
  END_REPEAT (lut)
  FIELD_B (b280, 280);
  DXF { FIELD_VECTOR_T (exprs, T, numelems, 302); }
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (BLOCKLOOKUPPARAMETER)
  AcDbBlock1PtParameter_fields;
  SUBCLASS (AcDbBlockLookupParameter)
  FIELD_BL (index, 0);
  FIELD_T (lookup_name, 303);
  FIELD_T (lookup_desc, 304);
  DXF { FIELD_BL (index, 94); }
  FIELD_T (unknown_t, 0);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (BLOCKPOINTPARAMETER)
  AcDbBlock1PtParameter_fields;
  SUBCLASS (AcDbBlockPointParameter)
  FIELD_T (position_name, 303);
  FIELD_T (position_desc, 304);
  FIELD_3BD (def_label_pt, 1011);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// DXF wrong
DWG_OBJECT (BLOCKSTRETCHACTION)
  DECODE_UNKNOWN_BITS
  AcDbBlockAction_fields;
  SUBCLASS (AcDbBlockStretchAction)
  BlockAction_ConnectionPt (conn_pts[0], 92, 301);
  BlockAction_ConnectionPt (conn_pts[1], 93, 302);
  FIELD_BL (num_pts, 72);
  FIELD_2RD_VECTOR (pts, num_pts, 1011);
  FIELD_BL (num_hdls, 73);
  // TODO one struct
  HANDLE_VECTOR (hdls, num_hdls, 0, 331);
  FIELD_VECTOR (shorts, BS, num_hdls, 74);

  FIELD_BL (num_codes, 75);
  // FIXME 3x BL?
  FIELD_VECTOR (codes, BL, num_codes, 76);
  // ...
  AcDbBlockAction_doubles_fields;
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (BLOCKPOLARSTRETCHACTION)
  DECODE_UNKNOWN_BITS
  AcDbBlockAction_fields;
  SUBCLASS (AcDbBlockPolarStretchAction)
  BlockAction_ConnectionPts (conn_pts, 0, 6, 92, 301);
  FIELD_BL (num_pts, 72);
  FIELD_2RD_VECTOR (pts, num_pts, 10);
  FIELD_BL (num_hdls, 73);
  // TODO one struct
  HANDLE_VECTOR (hdls, num_hdls, 0, 331);
  FIELD_VECTOR (shorts, BS, num_hdls, 74);

  FIELD_BL (num_codes, 75);
  // FIXME 3x BL?
  FIELD_VECTOR (codes, BL, num_codes, 76);
  // ...
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (ASSOCARRAYACTIONBODY)
  DECODE_UNKNOWN_BITS
  AcDbAssocArrayActionBody_fields;
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

#if defined (DEBUG_CLASSES) || defined (IS_FREE)

// no coverage yet
DWG_OBJECT (ASSOCARRAYMODIFYACTIONBODY)
  DECODE_UNKNOWN_BITS
  AcDbAssocArrayActionBody_fields;
  SUBCLASS (AcDbAssocArrayModifyActionBody)
  FIELD_BS (status, 70);
  FIELD_BL (num_items, 90);
  REPEAT (num_items, items, Dwg_ARRAYITEMLOCATOR)
  REPEAT_BLOCK
      SUB_FIELD_VECTOR_INL (items[rcount1], itemloc, BL, 3, 90);
      SET_PARENT_OBJ (items[rcount1]);
  END_REPEAT_BLOCK
  END_REPEAT (items)
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (BLOCKPROPERTIESTABLE)
  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbBlockPropertiesTable)
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (BLOCKPROPERTIESTABLEGRIP)
  DECODE_UNKNOWN_BITS
  AcDbBlockGrip_fields;
  SUBCLASS (AcDbBlockPropertiesTableGrip)
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

#endif

// not enough coverage
DWG_OBJECT (BLOCKUSERPARAMETER)
  DECODE_UNKNOWN_BITS
  AcDbBlock1PtParameter_fields;
  SUBCLASS (AcDbBlockUserParameter)
  FIELD_BS (flag, 90);
  FIELD_HANDLE (assocvariable, 5, 330);
  FIELD_T (expr, 301);
  //FIELD_T (name, 302);
  //FIELD_T (desc, 303);
  AcDbEvalVariant_fields (value);
  FIELD_BS (type, 170);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (BLOCKXYGRIP)
  DECODE_UNKNOWN_BITS
  AcDbBlockGrip_fields;
  SUBCLASS (AcDbBlockXYGrip)
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

//DWG_OBJECT (PARTIAL_VIEWING_FILTER)
//  SUBCLASS (AcDbFilter)
//  START_OBJECT_HANDLE_STREAM;
//DWG_OBJECT_END

DWG_OBJECT (PARTIAL_VIEWING_INDEX)
  FIELD_BL (num_entries, 0);
  if (FIELD_VALUE (num_entries))
    FIELD_B (has_entries, 0);
  REPEAT (num_entries, entries, Dwg_PARTIAL_VIEWING_INDEX_Entry)
  REPEAT_BLOCK
    SUB_FIELD_3BD (entries[rcount1], extents_min, 0);
    SUB_FIELD_3BD (entries[rcount1], extents_max, 0);
    SUB_FIELD_HANDLE (entries[rcount1], object, 5, 0);
    SET_PARENT_OBJ (entries[rcount1]);
  END_REPEAT_BLOCK
  END_REPEAT (entries)
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

#if defined (DEBUG_CLASSES) || defined (IS_FREE)
 
// (varies) DEBUGGING
DWG_OBJECT (BREAKDATA)
  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbBreakData)
  FIELD_BL (num_pointrefs, 90);
  HANDLE_VECTOR (pointrefs, num_pointrefs, 5, 330);
  START_OBJECT_HANDLE_STREAM;
  FIELD_HANDLE (dimref, 5, 331);
DWG_OBJECT_END

// (varies) DEBUGGING
DWG_OBJECT (BREAKPOINTREF)
  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbBreakPointRef)
  // XrefFullSubendPath
DWG_OBJECT_END

#endif // DEBUGGING

/* Those undocumented objects are also stored as raw UNKNOWN_OBJ */

#if 0

/* Missing DXF names:
  ARRAY ATTBLOCKREF ATTDYNBLOCKREF BLOCKREF DYNBLOCKREF XREF
  CENTERMARK CENTERLINE
*/

// EXACXREFPANELOBJECT
DWG_OBJECT (XREFPANELOBJECT)
  DECODE_UNKNOWN_BITS
  SUBCLASS(ExAcXREFPanelObject)
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// FIXME Class only, not an object.
DWG_OBJECT (NPOCOLLECTION)
  DECODE_UNKNOWN_BITS
  SUBCLASS(AcDbImpNonPersistentObjectsCollection)
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// NanoSPDS License Parser (flexlm) or nanoCAD SPDS.
// Probably used by 3rd party extensions or
// AEC/MAP/MECH to mark or protect their objects.
// Entity Wall PtDbWall
// Entity Format mcsDbObjectFormat
// Entity NOTEPOSITION mcsDbObjectNotePosition
// Entity mcsDbObject mcsDbObject
// Entity spdsLevelMark mcsDbObjectLevelMark
// Entity McDbContainer2 McDbContainer2
// Entity spdsRelationMark mcsDbObjectRelationMark
// Entity McDbMarker McDbMarker

DWG_OBJECT (ACDSRECORD)
  DECODE_UNKNOWN_BITS
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (ACDSSCHEMA)
  DECODE_UNKNOWN_BITS
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

#endif
