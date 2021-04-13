/* -*- c -*- */
/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2009-2010,2018-2026 Free Software Foundation, Inc.         */
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
 * modified by Gian Maria Gentilini
 */

#include "spec.h"
#include "dwg_spec_shared.h"

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

  IF_FREE_OR_SINCE (R_2000b)
    {
      BITCODE_RC dataflags;
#ifdef IS_ENCODER
      if (dat->from_version <= R_2000 || dat->opts & DWG_OPTS_INDXF)
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

/* AcDbMTextObjectEmbedded_fields is in dwg_spec_shared.h */

DWG_ENT_SUBCLASS_DECL (ATTRIB, AcDbMTextObjectEmbedded);
DWG_ENT_SUBCLASS_DECL (ATTDEF, AcDbMTextObjectEmbedded);

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

  IF_FREE_OR_SINCE (R_2000b)
    {
      BITCODE_RC dataflags;
#ifdef IS_ENCODER
      if (dat->from_version <= R_2000 || dat->opts & DWG_OPTS_INDXF)
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
    FIELD_RC (flags, 70);
    LOG_FLAG_ATTDEF
    FIELD_BS0 (field_length, 73);
    FIELD_BS0 (vert_alignment, 74);
    LOG_VERT_ALIGNMENT
    SINCE (R_2004a) {
      FIELD_RC (is_locked_in_block, 280);
      SUBCLASS (AcDbXrecord);
      FIELD_RC (keep_duplicate_records, 280);
    }
  }
  SINCE (R_2010b)
    {
      FIELD_RC (is_locked_in_block, 0);
      VALUEOUTOFBOUNDS (is_locked_in_block, 2)
    }
  SINCE (R_2018b)
    {
      FIELD_RC (mtext_type, 70); // 1=single line, 2=multi line attrib, 4=multi line attdef
      if (FIELD_VALUE (mtext_type) > 1)
        CALL_SUBCLASS (_obj, ATTRIB, AcDbMTextObjectEmbedded);
    }

  SINCE (R_13b1) {
    FIELD_T (tag, 0);
    FIELD_BS0 (field_length, 0);
    FIELD_RC (flags, 0); // 1 invisible, 2 constant, 4 verify, 8 preset
    LOG_FLAG_ATTDEF
    SINCE (R_2007a) {
      FIELD_B (lock_position_flag, 0); // 70
    }
    // XRECORD subclass
    SINCE (R_2010b) {
      FIELD_RC (keep_duplicate_records, 0);
      VALUEOUTOFBOUNDS (keep_duplicate_records, 1)
    }
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

  IF_FREE_OR_SINCE (R_2000b)
    {
      BITCODE_RC dataflags;
#ifdef IS_ENCODER
      if (dat->from_version <= R_2000 || dat->opts & DWG_OPTS_INDXF)
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
        FIELD_BE (extrusion, 210);
        FIELD_BT (thickness, 39);
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
    VALUE_RC (0, 280); // 0 = 2010+ (class_version)
    FIELD_T (prompt, 3);
    FIELD_T (tag, 2);
    FIELD_RC (mtext_type, 70);
    SINCE (R_13b1) {
      FIELD_BS0 (field_length, 73); // unused
      FIELD_BS0 (vert_alignment, 74);
    }
    SINCE (R_2004a) {
      FIELD_RC (is_locked_in_block, 280);
      // SUBCLASS (AcDbXrecord);
      // FIELD_RC (keep_duplicate_records, 280);
    }
  }
  SINCE (R_2010b)
    {
      FIELD_RC (is_locked_in_block, 0);
      VALUEOUTOFBOUNDS (is_locked_in_block, 1)
    }
  IF_FREE_OR_SINCE (R_2018)
    {
      // 1=single line, 2=multi line attrib, 4=multi line attdef
      FIELD_RC (mtext_type, 0);
      if (FIELD_VALUE (mtext_type) > 1)
        CALL_SUBCLASS (_obj, ATTDEF, AcDbMTextObjectEmbedded);
    }
  SINCE (R_13b1) {
    FIELD_T (tag, 0);
    FIELD_BS (field_length, 0); //DXF 73, unused
    FIELD_RC (flags, 0); // 1 invisible, 2 constant, 4 verify, 8 preset
    LOG_FLAG_ATTDEF
    SINCE (R_2007a)
      FIELD_B (lock_position_flag, 0);
    // XRECORD subclass
    SINCE (R_2010b) {
      FIELD_RC (keep_duplicate_records, 0);
      VALUEOUTOFBOUNDS (keep_duplicate_records, 1)
    }
    FIELD_T (prompt, 0);
  }

  COMMON_ENTITY_HANDLE_DATA;
  SINCE (R_13b1)
    FIELD_HANDLE (style, 5, 0);

DWG_ENTITY_END

DWG_ENT_SUBCLASS (ATTRIB, AcDbMTextObjectEmbedded);
DWG_ENT_SUBCLASS (ATTDEF, AcDbMTextObjectEmbedded);

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
    ON_FREE { // set via dwg_add_BLOCK
      FIELD_TV (name, 2);
    }
  }
#endif
#ifdef IS_DXF
  SINCE (R_13b1)
  {
    Dwg_Object_BLOCK_HEADER *_hdr = NULL;
    Dwg_Object *hdr
        = _ent->ownerhandle && _ent->ownerhandle->obj
              ? _ent->ownerhandle->obj : NULL;
    if (!hdr)
      hdr = dwg_ref_object (dwg, _ent->ownerhandle);
    if (hdr && hdr->fixedtype == DWG_TYPE_BLOCK_HEADER)
      _hdr = hdr->tio.object->tio.BLOCK_HEADER;
    else if (_ent->entmode == 2) {
      hdr = dwg_model_space_object (dwg);
      _hdr = hdr ? hdr->tio.object->tio.BLOCK_HEADER : NULL;
    }
    else if (_ent->entmode == 1) {
      hdr = dwg_paper_space_object (dwg);
      _hdr = hdr ? hdr->tio.object->tio.BLOCK_HEADER : NULL;
    }
    if (_hdr && (bit_empty_T (dat, _obj->name) || bit_eq_T (dat, _obj->name, "*"))) {
      VALUE_T (_hdr->name, 2);   // from BLOCK_HEADER
    }
    else {
      BLOCK_NAME (name, 2);
    }
    COMMON_ENTITY_HANDLE_DATA;

    if (!_hdr)
      {
        Dwg_Bitcode_3RD nullpt = { 0.0, 0.0, 0.0 };
        VALUE_BL (0, 70); // flags: anon, has_attribs, is_xref, is_overlaid, ...
        VALUE_3BD (nullpt, 10);
      }
    else
      {
        VALUE_BL (_hdr->flag & 0x3f, 70);
        VALUE_3BD (_hdr->base_pt, 10);
      }
    SINCE (R_13) {
      if (_hdr && (bit_empty_T (dat, _obj->name) || bit_eq_T (dat, _obj->name, "*"))) {
        VALUE_T (_hdr->name, 3);   // from BLOCK_HEADER
      } else {
        BLOCK_NAME (name, 3); // for entget() from BLOCK_HEADER
      }
    }
    if (_hdr) {
      VALUE_T (_hdr->xref_pname, 1);   // from BLOCK_HEADER
      VALUE_T0 (_hdr->description, 4); // from BLOCK_HEADER
    } else {
      VALUE_TFF ("", 1);
    }
  }
#else
  SINCE (R_13b1) {
    BLOCK_NAME (name, 2) // special pre-R13 naming rules
    COMMON_ENTITY_HANDLE_DATA;
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

  SINCE (R_2004a)
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

  IF_FREE_OR_SINCE (R_2004a)
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
    FIELD_2RD (point, 10);
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
    SINCE (R_2010b) {
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
    VALUE_RS ((BITCODE_RS)128, 70);
    FIELD_BSd (vertind[0], 71);
    FIELD_BSd (vertind[1], 72);
    FIELD_BSd (vertind[2], 73);
    FIELD_BSd0 (vertind[3], 74);
  }
#elif defined IS_JSON
  FIELD_RC (flag, 0);
  FIELD_VECTOR_INL (vertind, BSd, 4, 71);
#else
  //FIELD_VALUE (pt) = { 0.0, 0.0, 0.0 };
  PRE (R_13b1)
  {
    if (R11OPTS (OPTS_R11_VERTEX_HAS_FLAG)) {
      FIELD_RC (flag, 0);
      LOG_FLAG_VERTEX
    }
    if (R11OPTS (OPTS_R11_VERTEX_HAS_INDEX1)) {
      FIELD_RSd (vertind[0], 71);
    }
    if (R11OPTS (OPTS_R11_VERTEX_HAS_INDEX2)) {
      FIELD_RSd (vertind[1], 72);
    }
    if (R11OPTS (OPTS_R11_VERTEX_HAS_INDEX3)) {
      FIELD_RSd (vertind[2], 73);
    }
    if (R11OPTS (OPTS_R11_VERTEX_HAS_INDEX4)) {
      FIELD_RSd (vertind[3], 74);
    }
  }
  LATER_VERSIONS {
    FIELD_VALUE (flag) = 128;
    FIELD_BSd (vertind[0], 71);
    FIELD_BSd (vertind[1], 72);
    FIELD_BSd (vertind[2], 73);
    FIELD_BSd (vertind[3], 74);
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

    SINCE (R_2004a) {
      FIELD_BL (num_owned, 0);
    }
  }
  COMMON_ENTITY_HANDLE_DATA;

  IF_FREE_OR_VERSIONS (R_13b1, R_2000)
    {
      FIELD_HANDLE (first_vertex, 4, 0);
      FIELD_HANDLE (last_vertex, 4, 0);
    }

  IF_FREE_OR_SINCE (R_2004a)
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
    KEY (flag); VALUE_RS ((BITCODE_RS)(flag | 8), 70);
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
  SINCE (R_2004a) {
    FIELD_BL (num_owned, 0);
  }

  COMMON_ENTITY_HANDLE_DATA;
  IF_FREE_OR_VERSIONS (R_13b1, R_2000)
    {
      FIELD_HANDLE (first_vertex, 4, 0);
      FIELD_HANDLE (last_vertex, 4, 0);
    }
  IF_FREE_OR_SINCE (R_2004a)
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
  SINCE (R_2000b)
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
 * Shared macros for dimensions, 3DSOLID, TABLE values.
 * Used by both dwg.spec and dwg2.spec.
 */

/* removed: COMMON_ENTITY_DIMENSION - now in dwg_spec_shared.h */
#if 0
    SUBCLASS (AcDbDimension)                                                  \
    SINCE (R_2010b)                                                            \
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
          FREE (blockname);                                                   \
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
    SINCE (R_2000b)                                                            \
    {                                                                         \
      FIELD_BS (attachment, 71);                                              \
      FIELD_BS1 (lspace_style, 72);                                           \
      FIELD_BD1 (lspace_factor, 41);                                          \
      FIELD_BD (act_measurement, 42);                                         \
    }                                                                         \
    SINCE (R_2007a)                                                            \
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

/* varies. i.e. a jogged radius dim */
DWG_ENTITY (LARGE_RADIAL_DIMENSION)

  COMMON_ENTITY_DIMENSION
  SUBCLASS (AcDbRadialDimensionLarge)
  DXF {
    FIELD_3BD (jog_pt, 13);
    FIELD_3BD (ovr_center, 14);
    FIELD_3BD (chord_pt, 15);
    FIELD_BD (jog_angle, 40);
  } else {
    FIELD_3BD (def_pt, 0);
    FIELD_3BD (chord_pt, 0);
    FIELD_BD (jog_angle, 0);
    FIELD_3BD (ovr_center, 0);
    FIELD_3BD (jog_pt, 0);
  }

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
  SINCE (R_2000b)
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
    KEY (flag); VALUE_RS ((BITCODE_RS)64, 70);
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

  SINCE (R_2004a) {
    FIELD_BL (num_owned, 0);
  }

  COMMON_ENTITY_HANDLE_DATA;
  IF_FREE_OR_VERSIONS (R_13b1, R_2000)
    {
      FIELD_HANDLE (first_vertex, 4, 0);
      FIELD_HANDLE (last_vertex, 4, 0);
    }
  IF_FREE_OR_SINCE (R_2004a)
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

  SINCE (R_2004a) {
    FIELD_BL (num_owned, 0);
  }

  COMMON_ENTITY_HANDLE_DATA;
  VERSIONS (R_13b1, R_2000)
    {
      FIELD_HANDLE (first_vertex, 4, 0);
      FIELD_HANDLE (last_vertex, 4, 0);
    }
  IF_FREE_OR_SINCE (R_2004a)
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
    // The overall paperspace viewport (entmode 0) has on_off=0, id=0.
    // Active viewports auto-increment id starting at 1.
    if (_ent->entmode == 0) {
      FIELD_VALUE (on_off) = 0;
      FIELD_VALUE (id) = 0;
      dwg->last_viewport_id = 0;
    } else {
      FIELD_VALUE (on_off) = 1;
      dwg->last_viewport_id++;
      FIELD_VALUE (id) = dwg->last_viewport_id;
    }
    FIELD_RS (on_off, 68);
    FIELD_RS (id, 69);
  }

  SINCE (R_2000b) {
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
      SINCE (R_2007a) {
        FIELD_BS (grid_major, 61);
      }
    }
  }

  SINCE (R_2000b) {
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
      SINCE (R_2004a) {
        FIELD_BS (shadeplot_mode, 170);
      }
      SINCE (R_2007a) {
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
      SINCE (R_2004a) {
        FIELD_BS (shadeplot_mode, 170);
      }
    }
  }
  DXF {
    FIELD_HANDLE (visualstyle, 5, 348);
  }

  SINCE (R_2007a) {
    FIELD_B (use_default_lights, 292);
    FIELD_RC (default_lighting_type, 282);
    FIELD_BD (brightness, 141);
    FIELD_BD (contrast, 142);
    FIELD_CMC (ambient_color, 63);
  }

  COMMON_ENTITY_HANDLE_DATA;
  VERSIONS (R_13b1, R_14) {
    FIELD_HANDLE (vport_entity_header, 5, 0); // => VX
  }
  VERSIONS (R_2000b, R_2002) {
    HANDLE_VECTOR (frozen_layers, num_frozen_layers, 5, 341);
    FIELD_HANDLE (clip_boundary, 5, 340);
  }
  SINCE (R_2004a) {
    HANDLE_VECTOR (frozen_layers, num_frozen_layers, 4, 341);
    FIELD_HANDLE (clip_boundary, 5, 340);
  }
  VERSIONS (R_2000b, R_2002) {
    FIELD_HANDLE (vport_entity_header, 5, 0);  // => VX
  }
  SINCE (R_2000b) {
    FIELD_HANDLE (named_ucs, 5, 345);
    FIELD_HANDLE (base_ucs, 5, 346);
  }
  SINCE (R_2007a) {
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
  SINCE (R_2013b) {
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

/* removed: WIRESTRUCT_fields, 3DSOLID helpers, ACTION_3DSOLID - now in dwg_spec_shared.h */
#if 0
#define WIRESTRUCT_fields_REMOVED(name)               \
  SUB_FIELD_RC (name, type, 0);                       \
  SUB_FIELD_BLd (name, selection_marker, 0);          \
  PRE (R_2004a) {                                      \
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
                REALLOC (FIELD_VALUE (encr_sat_data), (i+1) * sizeof (char*));
              FIELD_VALUE (block_size) = (BITCODE_BL*)
                REALLOC (FIELD_VALUE (block_size), (i+1) * sizeof (BITCODE_BL));
              if (!FIELD_VALUE (encr_sat_data) || !FIELD_VALUE (block_size))
                return DWG_ERR_OUTOFMEM;
              FIELD_BL (block_size[i], 0);
              if (FIELD_VALUE (block_size[i]) > 0
                  && AVAIL_BITS (dat) > 8 * FIELD_VALUE (block_size[i]))
                {
                  BITCODE_BL len = FIELD_VALUE (block_size[i]);
                  FIELD_TFv (encr_sat_data[i], len, 1);
                  if (!FIELD_VALUE (encr_sat_data) || !FIELD_VALUE (encr_sat_data[i]))
                    FIELD_VALUE (block_size[i]) = 0;
                  FIELD_VALUE (encr_sat_data[i][len]) = 0;
                  total_size += FIELD_VALUE (block_size[i]);
                }
              else
                {
                  FIELD_VALUE (encr_sat_data[i]) = (char*)CALLOC (1, 1);
                  FIELD_VALUE (block_size[i]) = 0;
                }
            }
          while (FIELD_VALUE (block_size[i++]) > 0 && AVAIL_BITS (dat) >= 16); // crc RS
          if (!FIELD_VALUE (encr_sat_data) || !FIELD_VALUE (block_size))
            {
              FREE (FIELD_VALUE (block_size));
              FREE (FIELD_VALUE (encr_sat_data));
              return DWG_ERR_VALUEOUTOFBOUNDS;
            }

          // de-obfuscate SAT data
          FIELD_VALUE (acis_data) = (BITCODE_RC *)MALLOC (total_size + 1);
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
          FIELD_VALUE (block_size) = (BITCODE_BL*)CALLOC (2, sizeof (BITCODE_BL));
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
              FIELD_VALUE (acis_data) = (unsigned char*)CALLOC (size, 1);
              // Binary SAB. unencrypted, documented format until "End-of-ACIS-data"
              // TODO There exist also SAB streams with a given number of records, but I
              // haven't seen them here. See dwg_convert_SAB_to_SAT1
              // Better read the first header line here, to check for num_records 0.
              // Or even parse the whole SAB format here, and store the SAB different
              // to the ASCII acis_data.
              FIELD_TFF (acis_data, size, 1); // SAB "ACIS BinaryFile"
              LOG_TRACE ("Unknown ACIS 2 SAB sab_size %" PRIuSIZE
                         " starting at %" PRIuSIZE "\n",
                         size, pos);
              if ((p = (char *)memmem (_obj->acis_data, size, end,
                                       strlen (end))))
                {
                  size = p - (char*)_obj->acis_data;
                  size += strlen (end);
                  dat->byte = pos + size;
                  _obj->sab_size = size & 0xFFFFFFFF;
                  LOG_TRACE ("Found End-of-ACIS-data. sab_size: %" PRIuSIZE
                             ", new pos: %" PRIuSIZE "\n",
                             size, dat->byte);
                }
              else if ((p = (char *)memmem (_obj->acis_data, size, end1,
                                            strlen (end1))))
                {
                  size = p - (char*)_obj->acis_data;
                  size += strlen (end1);
                  dat->byte = pos + size;
                  _obj->sab_size = size & 0xFFFFFFFF;
                  LOG_TRACE ("Found End-of-ASM-data. sab_size: %" PRIuSIZE
                             ", new pos: %" PRIuSIZE "\n",
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
                  = (BITCODE_BL *)CALLOC (2, sizeof (BITCODE_BL));
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
              FIELD_VALUE (block_size) = (BITCODE_BL*)REALLOC (FIELD_VALUE (block_size), (num_blocks + 1) * sizeof (BITCODE_BL));
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
      SINCE (R_2007a)                                                         \
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
  SINCE (R_2013b)                                                             \
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
    SINCE (R_2007a) {                                                         \
      SUBCLASS (AcDb3dSolid);                                                 \
      FIELD_HANDLE (history_id, 4, 350);                                      \
    }                                                                         \
  } else if (FIELD_VALUE (version) > 1                                        \
             && (!IF_IS_DECODER || AVAIL_BITS (hdl_dat) >= 8)) {              \
      FIELD_HANDLE (history_id, 4, 350);                                      \
  }                                                                           \
  ON_FREE { FIELD_HANDLE (history_id, 4, 350); }

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
#endif /* removed 3DSOLID block */

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
  SINCE (R_2000b)
    FIELD_RC0 (cloning, 281);
#else
  FIELD_BL (numitems, 0);
  SINCE (R_13c3) {
    SINCE (R_2000b)
      {
        IF_ENCODE_FROM_EARLIER {
          FIELD_VALUE (cloning) = FIELD_VALUE (is_hardowner) & 0xffff;
        }
        FIELD_BS (cloning, 281);
      }
    if (dat->version != R_13c3 || dwg->header.is_maint > 4)
      FIELD_RC (is_hardowner, 280);
  }
  //VALUEOUTOFBOUNDS (numitems, 10000)
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
        fprintf (dat->fh, ":%s", JSON_SPC);
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
  if (FIELD_VALUE (is_hardowner))
    FIELD_RC (is_hardowner, 280);
  FIELD_BS (cloning, 281);
#else
  FIELD_BL (numitems, 0);
  FIELD_BS (cloning, 281);
  FIELD_RC (is_hardowner, 280);
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
        fprintf (dat->fh, ":%s", JSON_SPC);
        VALUE_HANDLE (_obj->itemhandles[rcount1], itemhandles, 2, 350);
      }
  }
  ENDRECORD()
#else
  FIELD_VECTOR_T (texts, T, numitems, 3);
#endif

  START_OBJECT_HANDLE_STREAM;
#if !defined(IS_DXF) && !defined (IS_JSON)
  HANDLE_VECTOR_N (itemhandles, FIELD_VALUE (numitems), 2, 0);
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
  SINCE (R_2000b) {
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

/* (44)
 */
DWG_ENTITY (MTEXT)

  SUBCLASS (AcDbMText)
  FIELD_3BD (ins_pt, 10);
  FIELD_3BD (extrusion, 210);
  FIELD_3BD (x_axis_dir, 11);

  DXF {
    FIELD_BD (text_height, 40);
    FIELD_BD (rect_width, 41);
    SINCE (R_2007a) {
      FIELD_BD (rect_height, 46);
    }
  } else {
    FIELD_BD (rect_width, 41);
    SINCE (R_2007a) {
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

  SINCE (R_2000b)
    {
      FIELD_BS (linespace_style, 73);
      FIELD_BD (linespace_factor, 44);
      FIELD_B (unknown_b0, 0);
    }
  SINCE (R_2004a)
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
  VERSIONS (R_13c3, R_2007) {
    FIELD_3DPOINT (endptproj, 213);
  }

  VERSIONS (R_13b1, R_14) {
    FIELD_BD (dimgap, 0);
  }
  FIELD_BD (box_height, 0);
  FIELD_BD (box_width, 0);
  FIELD_B (hookline_dir, 0); // if hook line is on x direction
  FIELD_B (arrowhead_on, 0);
  ENCODER {
    dwg_calc_hookline_on (_obj);
    LOG_TRACE("hookline_on: %d (calc)\n", FIELD_VALUE (hookline_on));
  }
  FIELD_BSx (arrowhead_type, 0);
  DECODER {
    dwg_calc_hookline_on (_obj);
    LOG_TRACE("hookline_on: %d (calc)\n", FIELD_VALUE (hookline_on));
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

  SINCE (R_2000b)
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

/* (46)
   See https://docs.intellicad.org/files/oda/2021_11/oda_drawings_docs/db_fcf.html
 */
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
    SINCE (R_2007a) // AC1020 aka R_2006
      {
        FIELD_BS (insert_units, 70);
        FIELD_B (explodable, 280);
        FIELD_RC (block_scaling, 281);
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
  SINCE (R_2000b) {
    FIELD_B (xref_loaded, 0); // bit 32
  }
  SINCE (R_13b1) {
    FIELD_VALUE (flag) |= FIELD_VALUE (anonymous) |
                          FIELD_VALUE (hasattrs) << 1 |
                          FIELD_VALUE (blkisxref) << 2 |
                          FIELD_VALUE (xrefoverlaid) << 3;
  }
  SINCE (R_2004a) {
    if (!FIELD_VALUE (blkisxref) && !FIELD_VALUE (xrefoverlaid))
      {
        FIELD_BL (num_owned, 0);
        VALUEOUTOFBOUNDS (num_owned, 0xf00000)
      }
  }

  SINCE (R_13b1) {
    FIELD_3DPOINT (base_pt, 10);
    FIELD_T (xref_pname, 1); // and 3
  }

  IF_FREE_OR_SINCE (R_2000b)
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

  SINCE (R_2007a) // AC1020 aka R_2006
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
  IF_FREE_OR_SINCE (R_2004a)
    {
      if (FIELD_VALUE (num_owned) < 0xf00000) {
        HANDLE_VECTOR (entities, num_owned, 4, 0);
      }
    }
  IF_FREE_OR_SINCE (R_13b1) {
    FIELD_HANDLE (endblk_entity, 3, 0);
  }
  IF_FREE_OR_SINCE (R_2000b)
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
      FIELD_VALUE (off)           = FIELD_VALUE (color.index) < 0;
      FIELD_VALUE (frozen)        = FIELD_VALUE (flag) & 1;
      FIELD_VALUE (frozen_in_new) = FIELD_VALUE (flag) & 2;
      FIELD_VALUE (locked)        = FIELD_VALUE (flag) & 4;
    }
  }
  VERSIONS (R_13b1, R_14) {
    FIELD_B (frozen, 0); // bit 1
    FIELD_B (off, 0);    // also: negate the color. see below
    FIELD_B (frozen_in_new, 0);
    FIELD_B (locked, 0);
  }
  SINCE (R_2000b) {
    // separate DXF flag 70 from the internal DWG flag0 bitmask
    int flag0;
    ENCODER {
      // with json we only use flag0, with DXF we use flag
      if (dwg->opts & DWG_OPTS_INDXF) {
        FIELD_VALUE (off)           = FIELD_VALUE (color.index) < 0;
        FIELD_VALUE (frozen)        = FIELD_VALUE (flag) & 1;
        FIELD_VALUE (frozen_in_new) = FIELD_VALUE (flag) & 2;
        FIELD_VALUE (locked)        = FIELD_VALUE (flag) & 4;
        FIELD_VALUE (is_xref_ref)   = FIELD_VALUE (flag) & 64;
        FIELD_VALUE (plotflag) = 1;
      }
      if (!(dwg->opts & DWG_OPTS_INJSON))
        FIELD_VALUE (flag0) = (FIELD_VALUE (frozen) ? 1 : 0) +
          (FIELD_VALUE (off) ? 2 : 0) +
          (FIELD_VALUE (frozen_in_new) ? 4 : 0) +
          (FIELD_VALUE (locked) ? 8 : 0) +
          (FIELD_VALUE (plotflag) ? 16 : 0) +
          (FIELD_VALUE (linewt) ? (FIELD_VALUE (linewt) & 0x1F) << 5 : 0);
    }
    FIELD_BSx (flag0, 0); // -> 70,290,370
    flag0 = FIELD_VALUE (flag0);
    //DECODER {
    //  FIELD_VALUE (flag) |= flag0 & ~0x3f0; // without plotflag
    //  LOG_TRACE ("  => flag %u [BL 70]\n", FIELD_VALUE (flag));
    //}
    // DWG: frozen (1), off (2), frozen by default (4),
    //      locked (8), plotting flag (16), and linewt (mask with 0x03E0)
    FIELD_VALUE (frozen) = (flag0 & 1) ? 1 : 0;
    LOG_LAYER_FLAG(frozen);
    FIELD_VALUE (off) = (flag0 & 2) ? 1 : 0;
    LOG_LAYER_FLAG(off);
    FIELD_VALUE (frozen_in_new) = (flag0 & 4) ? 1 : 0;
    LOG_LAYER_FLAG(frozen_in_new);
    FIELD_VALUE (locked) = (flag0 & 8) ? 1 : 0;
    LOG_LAYER_FLAG(locked);
    FIELD_VALUE (plotflag) = (flag0 & 16) ? 1 : 0;
    LOG_LAYER_FLAG(plotflag);
    FIELD_VALUE (linewt) = (flag0 & 0x03E0) >> 5;
    LOG_LAYER_FLAG(linewt);
    // DXF: frozen (1), frozen by default in new viewports (2),
    //      locked (4), is_xref_ref (16), is_xref_resolved (32), is_xref_dep (64).
    FIELD_VALUE (flag) = FIELD_VALUE (frozen) |
      (FIELD_VALUE (frozen_in_new) << 1) |
      (FIELD_VALUE (locked) << 2) |
      (FIELD_VALUE (is_xref_ref) << 4) |
      ((FIELD_VALUE (is_xref_resolved) ? 1 : 0) << 5) |
      (FIELD_VALUE (is_xref_dep) << 6);
    DECODER {
      LOG_TRACE ("  => flag %u [BL 70]\n", FIELD_VALUE (flag));
    }
    JSON {
      FIELD_RC (linewt, 370);
    }
  }
  SINCE (R_13b1) {
    DXF {
      if (FIELD_VALUE (color.index) == 256) {
        FIELD_VALUE (color.index) = FIELD_VALUE (color.rgb) & 0xFF;
      }
      // Negative value in case of disabled layer
      FIELD_VALUE (color.index) = - FIELD_VALUE(color.index);
    }
    FIELD_CMC (color, 62);
  }
  VERSIONS (R_13b1, R_14) {
    DECODER {
      FIELD_VALUE (off) = FIELD_VALUE (color.index) < 0;
      LOG_LAYER_FLAG(off);
    }
    // for DWG
    FIELD_VALUE (flag0) |= FIELD_VALUE (frozen) |
      (FIELD_VALUE (frozen_in_new) << 1) |
      (FIELD_VALUE (locked) << 2) |
      (FIELD_VALUE (off) ? 32 : 0);
    // for DXF
    FIELD_VALUE (flag) |= FIELD_VALUE (frozen) |
      (FIELD_VALUE (frozen_in_new) << 1) |
      (FIELD_VALUE (locked) << 2) |
      (FIELD_VALUE (off) ? 32: 0);
  }

  SINCE (R_13b1) {
    START_OBJECT_HANDLE_STREAM;
  }
  SINCE (R_2000b) {
    FIELD_HANDLE (plotstyle, 5, 0);
  }
  SINCE (R_2007a) {
    ENCODER {
#ifndef DEBUG_CLASSES
      _obj->material->absolute_ref = 0;
      _obj->material->handleref.value = 0;
#endif
    }
    FIELD_HANDLE (material, 5, 0);
  }
  SINCE (R_13b1) {
    FIELD_HANDLE (ltype, 5, 6);
  }
  DXF {
    SINCE (R_2000b) {
      if (_obj->name &&
          (bit_eq_T (dat, _obj->name, "Defpoints") ||
           bit_eq_T (dat, _obj->name, "DEFPOINTS")))
      {
        _obj->plotflag = 0;
        FIELD_B (plotflag, 290);
      } else {
        FIELD_B1 (plotflag, 290);
      }
    }
    SINCE (R_13b1) {
      int lw = dxf_cvt_lweight (FIELD_VALUE (linewt));
      if (lw) {
        KEY (linewt); VALUE_BSd (lw, 370);
      }
    }
    SINCE (R_2000b) {
      FIELD_HANDLE (plotstyle, 5, 390);
    }
    SINCE (R_2007a) {
      FIELD_HANDLE (material, 5, 347);
    }
  }
  SINCE (R_2013b) {
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
      DECODER_OR_ENCODER {
        LOG_TRACE ("  ")
        LOG_FLAG_TextStyle
      }
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
            FREE (s);
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
  DXF {
    FIELD_BD (pattern_len, 40);
  } else {
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
  }
  SINCE (R_13b1)
  {
    REPEAT (numdashes, dashes, Dwg_LTYPE_dash)
    REPEAT_BLOCK
      SUB_FIELD_BD (dashes[rcount1],length, 49);
      DXF {
        SUB_FIELD_BS (dashes[rcount1],shape_flag, 74);
        LOG_LTYPE_SHAPE_FLAG
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
        LOG_LTYPE_SHAPE_FLAG
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
#if !defined(IS_DECODER) && defined(USE_WRITE) && !defined(DECODE_TEST_C)
      // downconvert from 512
      if (dwg->header.from_version > R_2004)
        dwg_convert_LTYPE_strings_area (dwg, _obj);
#endif
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
                LOG_TRACE ("dashes[%u].text: \"%s\" [TF %" PRIuSIZE " 9] @%u\n", rcount1,
                           _obj->dashes[rcount1].text, strlen (_obj->dashes[rcount1].text), dash_i);
                dash_i += (unsigned)(strnlen (_obj->dashes[rcount1].text,
                                              256 - dash_i)
                                     + 1);
              }
          }
      }
    }
    LATER_VERSIONS {
#if !defined(IS_DECODER) && defined(USE_WRITE) && !defined(DECODE_TEST_C)
        // upconvert from 256
        if (dwg->header.from_version <= R_2004)
          dwg_convert_LTYPE_strings_area (dwg, _obj);
#endif
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
                  LOG_TRACE ("dashes[%u].text: \"%s\" [TF %" PRIuSIZE " 9] @%u\n", rcount1,
                             _obj->dashes[rcount1].text, strlen (_obj->dashes[rcount1].text), dash_i);
                  dash_i += ((2 * bit_wcs2nlen ((BITCODE_TU)_obj->dashes[rcount1].text,
                                                256 - (dash_i / 2))) + 2 ) & 0xFFFF;
                }
            }
        }
      }
    } // r2007
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
  SINCE (R_2000b) {
    FIELD_RC (render_mode, 281);
  }
  SINCE (R_2007a) {
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
  SINCE (R_2000b) {
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

  SINCE (R_2007a) {
    FIELD_B (is_camera_plottable, 73);
  }
  SINCE (R_13b1) {
    START_OBJECT_HANDLE_STREAM;
  }
  SINCE (R_2007a) {
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
  PRE (R_2000b)
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
  SINCE (R_2000b)
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
    { // 1 and 2 are swapped in DXF output, 0 and 3 are on place
      BITCODE_RC ucsicon = FIELD_VALUE (UCSICON);
      if (ucsicon == 1)
        ucsicon = 2;
      else if (ucsicon == 2)
        ucsicon = 1;
      VALUE_RS (ucsicon, 74);
    }
    FIELD_RS (SNAPMODE, 75);
    FIELD_RS (GRIDMODE, 76);
    FIELD_RS (SNAPSTYLE, 77);
    FIELD_RS (SNAPISOPAIR, 78);

    SINCE (R_2000b) {
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
    SINCE (R_2007a) {
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
      SINCE (R_2000b) {
        FIELD_RC (render_mode, 281);
      }
      SINCE (R_2007a) {
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
        SINCE (R_2007a) {
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

      SINCE (R_2000b) {
        FIELD_B (ucs_at_origin, 0);
        FIELD_B (UCSVP, 71);
        FIELD_3BD (ucsorg, 110);
        FIELD_3BD (ucsxdir, 111);
        FIELD_3BD (ucsydir, 112);
        FIELD_BD (ucs_elevation, 146);
        FIELD_BS (UCSORTHOVIEW, 79);
      }

      SINCE (R_2007a) {
        FIELD_BS (grid_flags, 60);
        FIELD_BS (grid_major, 61);
      }
    }

    SINCE (R_13b1) {
      START_OBJECT_HANDLE_STREAM;
    }
    SINCE (R_2000b) {
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
      if (strncmp (_obj->name, "ADE_", 4) == 0) {
        VALUE_RS (1, 71); // in DXF only with ADE_PROJECTION
      }
    } else {
      // TODO dependent on maint_version?
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
  SINCE (R_2000b) {
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
  else ON_FREE {
      FIELD_TV (DIMBLK_T, 5);
      FIELD_TV (DIMBLK1_T, 6);
      FIELD_TV (DIMBLK2_T, 7);
  }
  IF_FREE_OR_SINCE (R_2000b)
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

  SINCE (R_2007a)
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

  SINCE (R_2000b)
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

  SINCE (R_2007a)
    {
      FIELD_BS (DIMARCSYM, 0);
    }

  IF_FREE_OR_SINCE (R_2000b)
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

  SINCE (R_2007a)
    {
      FIELD_B (DIMFXLON, 0);
    }

  IF_FREE_OR_SINCE (R_2010b)
    {
      FIELD_B (DIMTXTDIRECTION, 294); // ODA documented as 295. Undocumented in ACAD
      FIELD_BD (DIMALTMZF, 0); // undocumented
      FIELD_T (DIMALTMZS, 0); // undocumented
      FIELD_BD (DIMMZF, 0); // undocumented
      FIELD_T (DIMMZS, 0); // undocumented
    }

  SINCE (R_2000b)
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
  IF_FREE_OR_SINCE (R_2000b)
    {
      FIELD_HANDLE (DIMLDRBLK, 5, 341); /* Leader arrow (DIMLDRBLK)*/
      FIELD_HANDLE (DIMBLK, 5, 342);  /* Arrow */
      FIELD_HANDLE (DIMBLK1, 5, 343); /* Arrow 1 */
      FIELD_HANDLE (DIMBLK2, 5, 344); /* Arrow 2 */
    }
  IF_FREE_OR_SINCE (R_2007a)
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
   Not in DXF
*/
DWG_TABLE (VX_TABLE_RECORD)

  COMMON_TABLE_FLAGS (VX)
  PRE (R_13b1)
    {
      FIELD_RS (vport_entity_address, 0);
      FIELD_RSd (r11_viewport_index, 0);
      FIELD_RSd (r11_prev_entry_index, 0);
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
  LOG_MLINESTYLE_FLAG
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
        switch (FIELD_VALUE (lines[rcount1].lt.index)) {
        case 32767: VALUE_TFF ("BYLAYER", 6); break; /* default (SHRT_MAX) */
        case 32766: VALUE_TFF ("BYBLOCK", 6); break;
        case 0:  VALUE_TFF ("CONTINUOUS", 6); break;
        //TODO else lookup name on LTYPE_CONTROL list
        default: /*FIELD_HANDLE_NAME (lt.ltype, 5, 6);*/
                 VALUE_TFF ("", 6); break;
        }
#else
        SUB_FIELD_BSd (lines[rcount1], lt.index, 6);
#endif
      }
      LATER_VERSIONS {
        SUB_FIELD_HANDLE (lines[rcount1], lt.ltype, 5, 6);
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

//(78 + varies) pg.136
DWG_ENTITY (HATCH)

  SUBCLASS (AcDbHatch)
#if !defined (IS_DXF) && !defined (IS_INDXF)
  SINCE (R_2004a)
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
                      SINCE (R_2010)
                        {
#define seg segx[rcount2]
                          SUB_FIELD_BL (seg, num_fitpts, 97);
                          FIELD_2RD_VECTOR (seg.fitpts, seg.num_fitpts, 11);
                          SUB_FIELD_2RD (seg, start_tangent, 12);
                          SUB_FIELD_2RD (seg, end_tangent, 13);
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
  ENCODER {
    if (_obj->num_seeds && !_obj->seeds)
      {
        LOG_WARN ("HATCH.num_seeds %u but seeds NULL; force 0",
                  _obj->num_seeds);
        _obj->num_seeds = 0;
      }
  }
  FIELD_BL (num_seeds, 98);
  VALUEOUTOFBOUNDS (num_seeds, 10000)
  DECODER {
    int64_t avail_bits = (int64_t)(dat->size * 8) - bit_position (dat);
    if (avail_bits < 0)
      avail_bits = 0;
    if (_obj->num_seeds && avail_bits < (int64_t)_obj->num_seeds * 128)
      {
        BITCODE_BL max_seeds = (BITCODE_BL)(avail_bits / 128);
        LOG_WARN ("Truncate HATCH.num_seeds from %u to %u",
                  _obj->num_seeds, max_seeds);
        _obj->num_seeds = max_seeds;
      }
  }
  DECODER {
    if (_obj->num_seeds > 0)
      {
        FIELD_2RD_VECTOR (seeds, num_seeds, 10);
      }
  }
#ifndef IS_DECODER
  FIELD_2RD_VECTOR (seeds, num_seeds, 10);
#endif
#ifdef IS_DXF
  SINCE (R_2004a)
    {
      if (_obj->is_gradient_fill)
        error |= DWG_FUNC_N (ACTION,_HATCH_gradientfill)(dat,str_dat,obj,_obj);
    }
#endif

  DECODER {
    int64_t avail_bits = (int64_t)(hdl_dat->size * 8) - bit_position (hdl_dat);
    if (avail_bits < 8)
      {
        LOG_WARN ("Skip HATCH common handles due to short handle stream");
      }
    else
      {
        COMMON_ENTITY_HANDLE_DATA;
      }
  }
#ifndef IS_DECODER
  COMMON_ENTITY_HANDLE_DATA;
#endif

DWG_ENTITY_END

#if defined (DEBUG_CLASSES) || defined (IS_FREE)

// Hatched closed polygon
// debugging
DWG_ENTITY (MPOLYGON)

  SUBCLASS (AcDbMPolygon)

  FIELD_BS (style, 75); // 0=normal (odd parity); 1=outer; 2=whole //??

#if !defined (IS_DXF) && !defined (IS_INDXF)
  SINCE (R_2004a)
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
  SINCE (R_2004a)
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
  FIELD_2RD (image_size, 13);
  FIELD_HANDLE (imagedef, 5, 340); // hard pointer
  FIELD_BS (display_props, 70);
  FIELD_B (clipping, 280);
  FIELD_RC (brightness, 281);
  FIELD_RC (contrast, 282);
  FIELD_RC (fade, 283);
  FIELD_HANDLE (imagedefreactor, 3, 360); // hard owner
  SINCE (R_2010b) {
    FIELD_B (clip_mode, 0);
  }
  FIELD_BS (clip_boundary_type, 71); // 1 rect, 2 polygon
  if (FIELD_VALUE (clip_boundary_type) == 1)
    FIELD_VALUE (num_clip_verts) = 2;
  else
    FIELD_BL (num_clip_verts, 91);
  FIELD_2RD_VECTOR (clip_verts, num_clip_verts, 14);

  DXF { SINCE (R_2010b) {
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
  UNTIL (R_2002) {
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
  ON_FREE { FIELD_TV (plotview_name, 6); FIELD_HANDLE (plotview, 5, 6); }
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
  SINCE (R_2004a)
    {
      FIELD_BS (shadeplot_type, 76);
      FIELD_BS (shadeplot_reslevel, 77);
      FIELD_BS (shadeplot_customdpi, 78);
    }
  START_OBJECT_HANDLE_STREAM;
  SINCE (R_2007a) {
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
  UNTIL (R_2002) {
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
  ON_FREE { FIELD_TV (plotsettings.plotview_name, 6); FIELD_HANDLE (plotsettings.plotview, 5, 6); }
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
  SINCE (R_2004a)
    {
      FIELD_BS (plotsettings.shadeplot_type, 76);
      FIELD_BS (plotsettings.shadeplot_reslevel, 77);
      FIELD_BS (plotsettings.shadeplot_customdpi, 78);
    }
  SINCE (R_2007a) {
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

  SINCE (R_2004a) {
    FIELD_BL (num_viewports, 0);
    VALUEOUTOFBOUNDS (num_viewports, 10000)
  }

  START_OBJECT_HANDLE_STREAM;
  FIELD_HANDLE (block_header, 4, 330); // => pspace or mspace (owner)
  FIELD_HANDLE0 (active_viewport, 4, 331);
  FIELD_HANDLE0 (base_ucs, 5, 346);
  FIELD_HANDLE0 (named_ucs, 5, 345);

  SINCE (R_2004a) {
    HANDLE_VECTOR (viewports, num_viewports, 4, 0);
  }

DWG_OBJECT_END

//20.4.85 p.211
DWG_ENTITY (LWPOLYLINE)

  SUBCLASS (AcDbPolyline)
#ifdef IS_DXF
  FIELD_BL (num_points, 90);
  // 1 closed, 128 plinegen
  VALUE_BS (((FIELD_VALUE (flag) & 256) ? 128 : 0) + (FIELD_VALUE (flag) & 512 ? 1 : 0), 70);
  FIELD_BD (const_width, 43);
#else
  FIELD_BS (flag, 70); // 512 closed, 256 plinegen, 4 constwidth, 8 elevation, 2 thickness
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
  SINCE (R_2010b) {
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
        SINCE (R_2010b) {
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
    IF_FREE_OR_SINCE (R_2000b) {
      FIELD_2DD_VECTOR (points, num_points, 10);
    }

    FIELD_VECTOR (bulges, BD, num_bulges, 42);
    SINCE (R_2010b) {
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
  SINCE (R_2000b) {
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

  SINCE (R_2000b) {
    FIELD_RC (lock_aspect, 0);
  }

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

//pg.276
#if 0 /* no proxy subtypes yet. seems to be the same as LWPOLYLINE */
DWG_SUBENT (PROXY_LWPOLYLINE)

  HANDLE_UNKNOWN_BITS;
  FIELD_RL (data_size, 0);
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
  IF_FREE_OR_SINCE (R_2000b) {
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

  //HANDLE_UNKNOWN_BITS;
  SUBCLASS (AcDbProxyEntity)
  FIELD_BL (proxy_id, 90); // always 499
  DXF {
    PRE (R_2018) {
      FIELD_BL (class_id, 91);
    }
  }
  PRE (R_2018) {
    FIELD_BLx (dwg_versions, 95);
    FIELD_VALUE (maint_version) = FIELD_VALUE(dwg_versions) >> 8;
    FIELD_VALUE (dwg_version) = FIELD_VALUE(dwg_versions) & 0xff;
    LOG_TRACE ("> maint_version: %x\n", _obj->maint_version);
    LOG_TRACE ("> dwg_version: %x\n", _obj->dwg_version);
  }
  SINCE (R_2018) {
    FIELD_BLx (dwg_version, 0);
    FIELD_BLx (maint_version, 0);
    FIELD_VALUE (dwg_versions) = (FIELD_VALUE(maint_version) << 8) + FIELD_VALUE(dwg_version);
    DXF {
      FIELD_BLx (dwg_versions, 95);
    }
  }
  SINCE (R_2000b) {
    FIELD_B (from_dxf, 70); // Original Data Format: 0 dwg, 1 dxf
  }

  // proxy_data_size is _ent->preview_size. see par 29
  DXF_OR_PRINT {
    // preview 92/310 is also proxy data
    FIELD_BL (proxy_data_size, 92)
  } else {
    FIELD_VALUE (proxy_data_size) = _ent->preview_size;
  }
  FIELD_TF (proxy_data, _obj->proxy_data_size, 310);

  DECODER {
    _obj->data_numbits = (obj->hdlpos - bit_position (dat)) & 0xFFFFFFFF;
    if (dat->size > obj->size)
      {
        LOG_TRACE ("dat not restricted, dat->size %" PRIuSIZE
                   " > obj->size " FORMAT_RL "\n",
                   dat->size, obj->size);
        _obj->data_numbits
            = (((obj->address * 8) + obj->bitsize) - bit_position (dat))
              & 0xFFFFFFFF;
      }
    _obj->data = bit_read_bits (dat, _obj->data_numbits);
    LOG_TRACE ("> data_numbits: " FORMAT_BL " [ 93]\n", _obj->data_numbits);
    LOG_TRACE_TF (_obj->data, (_obj->data_numbits / 8) + (_obj->data_numbits % 8 ? 1 : 0));
  }
  ENCODER {
    LOG_TRACE ("data_numbits: " FORMAT_BL " [ 93]\n", _obj->data_numbits);
    bit_write_bits (dat, _obj->data, _obj->data_numbits);
  }
  DXF_OR_PRINT {
    unsigned bytes = _obj->data_numbits / 8;
    if (_obj->data_numbits % 8) bytes++;
    FIELD_BL (data_numbits, 93)
    FIELD_BINARY (data, bytes, 310);
  }
  FREE {
    FIELD_BINARY (data, _obj->data_numbits / 8, 0);
  }

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
#elif defined(IS_ENCODER) || defined(IS_JSON) || defined(IS_FREE)
  HANDLE_VECTOR (objids, num_objids, ANYCODE, 0);
#elif defined(IS_DXF)
  for (rcount1 = 0; rcount1 < _obj->num_objids; rcount1++)
    {
      int dxf = 330;
      if (!_obj->objids[rcount1]) {
        LOG_ERROR ("Illegal %s.objids[%u]", obj->name, rcount1);
      }
      else
        {
          unsigned code = _obj->objids[rcount1]->handleref.code;
          switch (code)
            {
            case 2: dxf = 330; break;
            case 3: dxf = 340; break;
            case 4: case 6: case 8: case 10: case 12: dxf = 350; break;
            case 5: dxf = 360; break;
            default: LOG_ERROR ("Illegal %s objids[%u].code %u", obj->name,
                                rcount1, code);
            }
          VALUE_HANDLE (_obj->objids[rcount1], objids, code, dxf);
        }
    }
  SINCE (R_2000b) { // end of Object ID's
    if (FIELD_VALUE (num_objids)) {
      VALUE_RS (0, 94);
    }
  }
#endif

DWG_ENTITY_END

//(499) pg.149 r2000+
// ACAD_PROXY_OBJECT really
DWG_OBJECT (PROXY_OBJECT)

  //HANDLE_UNKNOWN_BITS;
#ifdef IS_DXF
  PRE (R_2000b) {
    SUBCLASS (AcDbZombieObject)
  }
  LATER_VERSIONS {
    SUBCLASS (AcDbProxyObject)
  }
#endif
  FIELD_BL (proxy_id, 90); // always 499
  DXF {
    PRE (R_2018) {
      FIELD_BL (class_id, 91);
    }
  }
  PRE (R_2018) {
    FIELD_BLx (dwg_versions, 95);
    FIELD_VALUE (maint_version) = FIELD_VALUE(dwg_versions) >> 8;
    FIELD_VALUE (dwg_version) = FIELD_VALUE(dwg_versions) & 0xff;
    LOG_TRACE ("> maint_version: %x\n", _obj->maint_version);
    LOG_TRACE ("> dwg_version: %x\n", _obj->dwg_version);
  }
  SINCE (R_2018) {
    FIELD_BLx (dwg_version, 0);
    FIELD_BLx (maint_version, 0);
    FIELD_VALUE (dwg_versions) = (FIELD_VALUE(maint_version) << 8) + FIELD_VALUE(dwg_version);
    DXF {
      FIELD_BLx (dwg_versions, 95);
    }
  }
  SINCE (R_2000b)
  {
    FIELD_B (from_dxf, 70); // Original Data Format: 0 dwg, 1 dxf
  }

  DECODER {
    _obj->data_numbits = (obj->hdlpos - bit_position (dat)) & 0xFFFFFFFF;
    if (dat->size > obj->size)
      {
        LOG_TRACE ("dat not restricted, dat->size %" PRIuSIZE
                   " > obj->size " FORMAT_RL "\n",
                   dat->size, obj->size);
        _obj->data_numbits
            = (((obj->address * 8) + obj->bitsize) - bit_position (dat))
              & 0xFFFFFFFF;
      }
    _obj->data = bit_read_bits (dat, _obj->data_numbits);
    LOG_TRACE ("> data_numbits: " FORMAT_BL " [ 93]\n", _obj->data_numbits);
    LOG_TRACE_TF (_obj->data, (_obj->data_numbits / 8) + (_obj->data_numbits % 8 ? 1 : 0));
  }
  ENCODER {
    LOG_TRACE ("data_numbits: " FORMAT_BL " [ 93]\n", _obj->data_numbits);
    bit_write_bits (dat, _obj->data, _obj->data_numbits);
  }
  DXF_OR_PRINT {
    unsigned bytes = _obj->data_numbits / 8;
    if (_obj->data_numbits % 8) bytes++;
    FIELD_BL (data_numbits, 93)
    FIELD_BINARY (data, bytes, 310);
  }
  FREE {
    FIELD_BINARY (data, _obj->data_numbits / 8, 0);
  }

  START_OBJECT_HANDLE_STREAM;
#if defined(IS_DECODER)
  while (hdl_dat->byte < hdl_dat->size - 1)
    {
      Dwg_Handle hdl;
      if (bit_read_H (hdl_dat, &hdl))
        break; // error
      else
        {
          BITCODE_H ref
              = dwg_add_handleref (dwg, hdl.code, hdl.value, NULL);
          PUSH_HV (_obj, num_objids, objids, ref);
        }
    }
  LOG_TRACE ("num_objids: " FORMAT_BL "\n", _obj->num_objids);
#elif defined(IS_ENCODER) || defined(IS_JSON) || defined(IS_FREE)
  HANDLE_VECTOR (objids, num_objids, ANYCODE, 0);
#elif defined(IS_DXF)
  for (rcount1 = 0; rcount1 < _obj->num_objids; rcount1++)
    {
      int dxf = 330;
      if (!_obj->objids[rcount1]) {
        LOG_ERROR ("Illegal %s.objids[%u]", obj->name, rcount1);
      }
      else
        {
          unsigned code = _obj->objids[rcount1]->handleref.code;
          switch (code)
            {
            case 2: dxf = 330; break;
            case 3: dxf = 340; break;
            case 4: case 6: case 8: case 10: case 12: dxf = 350; break;
            case 5: dxf = 360; break;
            default: LOG_ERROR ("Illegal %s objids[%u].code %u", obj->name,
                                rcount1, code);
            }
          VALUE_HANDLE (_obj->objids[rcount1], objids, code, dxf);
        }
    }
  SINCE (R_2000b) { // end of Object ID's
    if (FIELD_VALUE (num_objids)) {
      VALUE_RS (0, 94);
    }
  }
#endif

DWG_OBJECT_END

/* removed: TABLE_value_fields - now in dwg_spec_shared.h */
#if 0
#define TABLE_value_fields_REMOVED(value)                                     \
  PRE (R_2007a) { FIELD_VALUE (value.data_type) &= ~0x200; }                   \
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
          SINCE (R_2007a) { FIELD_BL (value.data_size, 0); }                   \
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
          FIELD_VALUE (value.format_flags) = 0;                               \
          FIELD_VALUE (value.data_type) = 0;                                  \
          error |= DWG_ERR_INVALIDTYPE;                                       \
          break;                                                              \
          /*return DWG_ERR_INVALIDTYPE; */                                    \
        }                                                                     \
    }                                                                         \
  SINCE (R_2007a)                                                              \
  {                                                                           \
    FIELD_BL (value.unit_type, 94);                                           \
    FIELD_T (value.format_string, 300);                                       \
    if (FIELD_VALUE (value.unit_type) != 12)                                  \
      FIELD_T (value.value_string, 302);                                      \
  }
#endif /* removed TABLE_value_fields */

DWG_OBJECT (FIELD)

  SUBCLASS (AcDbField)
  FIELD_T (id, 1);
  FIELD_T (code, 2); // and code 3 for subsequent >255 chunks
  DXF {
    PRE (R_2007a) {
      FIELD_TV (format, 4);
    }
  }
  FIELD_BL (num_childs, 90);
  VALUEOUTOFBOUNDS (num_childs, 20000)
  DXF {
    HANDLE_VECTOR (childs, num_childs, 3, 360);
  }
  FIELD_BL (num_objects, 97);
  VALUEOUTOFBOUNDS (num_objects, 20000)
  DXF {
    HANDLE_VECTOR (objects, num_objects, 5, 331);
  }
#ifndef IS_DXF
  PRE (R_2007a) {
    FIELD_TV (format, 4);
  }
#endif
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

#ifndef IS_DXF
  FIELD_T (value_string, 301);
  FIELD_BL (value_string_length, 98); //ODA bug TV
#endif

  FIELD_BL (num_childval, 93);
  VALUEOUTOFBOUNDS (num_childval, 20000)
  DECODER {
    int64_t avail_bits = (int64_t)(dat->size * 8) - bit_position (dat);
    if (avail_bits < 0)
      avail_bits = 0;
    if (_obj->num_childval
        && (int64_t)_obj->num_childval * (int64_t)sizeof (Dwg_FIELD_ChildValue)
               > avail_bits)
      {
        LOG_WARN ("Truncate FIELD.num_childval from %u to 0", _obj->num_childval);
        _obj->num_childval = 0;
      }
  }
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
  DXF {
    FIELD_T (value_string, 301); // TODO: and 9 for subsequent >255 chunks
    FIELD_BL (value_string_length, 98); //ODA bug TV
  }

  START_OBJECT_HANDLE_STREAM;
  HANDLE_VECTOR (childs, num_childs, 3, 0);
  HANDLE_VECTOR (objects, num_objects, 5, 0);

DWG_OBJECT_END

DWG_OBJECT (FIELDLIST)

  SUBCLASS (AcDbIdSet)
  FIELD_BL (num_fields, 90);
  VALUEOUTOFBOUNDS (num_fields, 20000)
  FIELD_B (unknown, 0); // has handles?

  START_OBJECT_HANDLE_STREAM;
  DECODER {
    int64_t avail_bits = (int64_t)(hdl_dat->size * 8) - bit_position (hdl_dat);
    if (avail_bits < 0)
      avail_bits = 0;
    if (_obj->num_fields && avail_bits < (int64_t)_obj->num_fields * 8)
      {
        BITCODE_BL max_fields = (BITCODE_BL)(avail_bits / 8);
        LOG_WARN ("Truncate FIELDLIST.num_fields from %u to %u",
                  _obj->num_fields, max_fields);
        _obj->num_fields = max_fields;
      }
  }
  HANDLE_VECTOR (fields, num_fields, 0, 330); // 2 or 4, or 3.0.0
  SUBCLASS (AcDbFieldList)

DWG_OBJECT_END
