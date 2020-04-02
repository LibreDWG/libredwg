/* -*- c -*- */
/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2009-2010,2018-2020 Free Software Foundation, Inc.         */
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
 */

#include "spec.h"

/* (1/7) */
DWG_ENTITY (TEXT)

  DXF {
    //TODO can be skipped with DXF if STANDARD
    FIELD_HANDLE (style, 5, 7);
  }
  SUBCLASS (AcDbText)
  PRE (R_13) {
    FIELD_2RD (insertion_pt, 10);
    FIELD_RD (height, 40);
    FIELD_TV (text_value, 1);
    if (R11OPTS (1))
      FIELD_RD (rotation, 50);
    if (R11OPTS (2))
      FIELD_RD (width_factor, 41);
    if (R11OPTS (4))
      FIELD_RD (oblique_ang, 51);
    if (R11OPTS (8)) {
      DECODER { _ent->ltype_r11 = bit_read_RC (dat); }
      ENCODER { bit_write_RC (dat, _ent->ltype_r11); }
      PRINT   { LOG_TRACE ("ltype_r11: " FORMAT_RS "\n", _ent->ltype_r11); }
    }
    if (R11OPTS (16))
      FIELD_CAST (generation, RC, BS, 71);
    if (R11OPTS (32))
      FIELD_CAST (horiz_alignment, RC, BS, 72);
    if (R11OPTS (64))
      FIELD_2RD (alignment_pt, 11);
    if (R11OPTS (256))
      FIELD_CAST (vert_alignment, RC, BS, 73);
  }
  VERSIONS (R_13, R_14)
    {
      FIELD_BD (elevation, 30);
      FIELD_2RD (insertion_pt, 10);
      FIELD_2RD (alignment_pt, 11);
      FIELD_3BD (extrusion, 210);
      FIELD_BD (thickness, 39);
      FIELD_BD (oblique_ang, 51);
      FIELD_BD (rotation, 50);
      FIELD_BD (height, 40);
      FIELD_BD (width_factor, 41);
      FIELD_TV (text_value, 1);
      FIELD_BS (generation, 71);
      FIELD_BS (horiz_alignment, 72);
      FIELD_BS (vert_alignment, 73);
    }

  IF_FREE_OR_SINCE (R_2000)
    {
      /* We assume that the user (the client application)
         is responsible for taking care of properly updating the dataflags field
         which indicates which fields in the data structures are valid and which are
         undefined */
      BITCODE_RC dataflags;
      FIELD_RC (dataflags, 0);
      dataflags = FIELD_VALUE (dataflags);

      if (!(dataflags & 0x01))
        FIELD_RD (elevation, 30);
      FIELD_2RD (insertion_pt, 10);

      if (!(dataflags & 0x02))
        FIELD_2DD (alignment_pt, 10.0, 20.0, 11);

      FIELD_BE (extrusion, 210);
      FIELD_BT (thickness, 39);

      if (!(dataflags & 0x04))
        FIELD_RD (oblique_ang, 51);
      if (!(dataflags & 0x08))
        FIELD_RD (rotation, 50);

      FIELD_RD (height, 40);

      if (!(dataflags & 0x10))
        FIELD_RD (width_factor, 41);

      FIELD_T (text_value, 1);

      if (!(dataflags & 0x20))
        FIELD_BS (generation, 71);
      if (!(dataflags & 0x40))
        FIELD_BS (horiz_alignment, 72);
      if (!(dataflags & 0x80))
        FIELD_BS (vert_alignment, 73);
    }

  COMMON_ENTITY_HANDLE_DATA;
  SINCE (R_13)
    {
      IF_ENCODE_FROM_PRE_R13 {
        //FIXME: should really just lookup the style table; style is the index.
        FIELD_VALUE (style) = 0; //dwg_resolve_handle (dwg, obj->ltype_rs);
      }
#ifndef IS_DXF
      FIELD_HANDLE (style, 5, 7);
#endif
    }
  SUBCLASS (AcDbText)

DWG_ENTITY_END

/* (2/16) */
DWG_ENTITY (ATTRIB)

  DXF {
    //TODO can be skipped with DXF if STANDARD
    FIELD_HANDLE (style, 5, 7);
  }
  SUBCLASS (AcDbText)
  PRE (R_13)
    {
      LOG_ERROR ("TODO ATTRIB")
    }
  VERSIONS (R_13, R_14)
    {
      FIELD_BD (elevation, 30);
      FIELD_2RD (insertion_pt, 10);
      FIELD_2RD (alignment_pt, 11);
      FIELD_3BD (extrusion, 210);
      FIELD_BD (thickness, 39);
      FIELD_BD (oblique_ang, 51);
      FIELD_BD (rotation, 50);
      FIELD_BD (height, 40);
      FIELD_BD (width_factor, 41);
      FIELD_TV (text_value, 1);
      FIELD_BS (generation, 71);
      FIELD_BS (horiz_alignment, 72);
      FIELD_BS (vert_alignment, 73);
    }

  IF_FREE_OR_SINCE (R_2000)
    {
      /* We assume that the user (the client application)
         is responsible for taking care of properly updating the dataflags field
         which indicates which fields in the data structures are valid and which are
         undefined */
      BITCODE_RC dataflags;
      FIELD_RC (dataflags, 0);
      dataflags = FIELD_VALUE (dataflags);

      if (!(dataflags & 0x01))
        FIELD_RD (elevation, 30);
      FIELD_2RD (insertion_pt, 10);

      if (!(dataflags & 0x02))
        FIELD_2DD (alignment_pt, 10.0, 20.0, 11);

      FIELD_BE (extrusion, 210);
      FIELD_BT (thickness, 39);

      if (!(dataflags & 0x04))
        FIELD_RD (oblique_ang, 51);
      if (!(dataflags & 0x08))
        FIELD_RD (rotation, 50);

      FIELD_RD (height, 40);

      if (!(dataflags & 0x10))
        FIELD_RD (width_factor, 41);

      FIELD_T (text_value, 1);

      if (!(dataflags & 0x20))
        FIELD_BS (generation, 71);
      if (!(dataflags & 0x40))
        FIELD_BS (horiz_alignment, 72);
      if (!(dataflags & 0x80))
        FIELD_BS (vert_alignment, 74);
    }

  SUBCLASS (AcDbAttribute)
  SINCE (R_2010)
    {
      int dxf = dat->version == R_2010 ? 280: 0;
      FIELD_RC (class_version, dxf); // 0 = r2010
      VALUEOUTOFBOUNDS (class_version, 10)
    }
  SINCE (R_2018)
    {
      FIELD_RC (type, 70); // 1=single line, 2=multi line attrib, 4=multi line attdef

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

  FIELD_T (tag, 2);
  FIELD_BS (field_length, 73);
  FIELD_RC (flags, 70); // 1 invisible, 2 constant, 4 verify, 8 preset

  SINCE (R_2007) {
    FIELD_B (lock_position_flag, 0); // 70
  }

  COMMON_ENTITY_HANDLE_DATA;

  FIELD_HANDLE (style, 5, 0); // unexpected here in DXF

DWG_ENTITY_END

/* (3/15) */
DWG_ENTITY (ATTDEF)

  DXF {
    //TODO can be skipped with DXF if STANDARD
    FIELD_HANDLE (style, 5, 7);
  }
  SUBCLASS (AcDbText)
  PRE (R_13)
    {
      LOG_ERROR ("TODO ATTDEF")
    }
  VERSIONS (R_13, R_14)
    {
      FIELD_BD (elevation, 30);
      FIELD_2RD (insertion_pt, 10);
      FIELD_2RD (alignment_pt, 11);
      FIELD_3BD (extrusion, 210);
      FIELD_BD (thickness, 39);
      FIELD_BD (oblique_ang, 51);
      FIELD_BD (rotation, 50);
      FIELD_BD (height, 40);
      FIELD_BD (width_factor, 41);
      FIELD_T (default_value, 1);
      FIELD_BS (generation, 71);
      FIELD_BS (horiz_alignment, 72);
      FIELD_BS (vert_alignment, 74);
    }

  IF_FREE_OR_SINCE (R_2000)
    {
      /* We assume that the user (the client application)
         is responsible for taking care of properly updating the dataflags field
         which indicates which fields in the data structures are valid and which are
         undefined */
      BITCODE_RC dataflags;
      FIELD_RC (dataflags, 0);
      dataflags = FIELD_VALUE (dataflags);

      if (!(dataflags & 0x01))
        FIELD_RD (elevation, 30);
      FIELD_2RD (insertion_pt, 10);

      if (!(dataflags & 0x02))
        FIELD_2DD (alignment_pt, 10.0, 20.0, 11);

      FIELD_BE (extrusion, 210);
      FIELD_BT (thickness, 39);

      if (!(dataflags & 0x04))
        FIELD_RD (oblique_ang, 51);
      if (!(dataflags & 0x08))
        FIELD_RD (rotation, 50);

      FIELD_RD (height, 40);

      if (!(dataflags & 0x10))
        FIELD_RD (width_factor, 41);

      FIELD_T (default_value, 1);

      if (!(dataflags & 0x20))
        FIELD_BS (generation, 71);
      if (!(dataflags & 0x40))
        FIELD_BS (horiz_alignment, 72);
      if (!(dataflags & 0x80))
        FIELD_BS (vert_alignment, 74);
    }

  SUBCLASS (AcDbAttributeDefinition);
  SINCE (R_2010)
    {
      int dxf = dat->version == R_2010 ? 280: 0;
      FIELD_RC (class_version, dxf); // 0 = r2010
      VALUEOUTOFBOUNDS (class_version, 10)
    }
  DXF { FIELD_T (prompt, 3); }
  DXF { FIELD_T (tag, 2); }
  IF_FREE_OR_SINCE (R_2018)
    {
      FIELD_RC (type, 70); // 1=single line, 2=multi line attrib, 4=multi line attdef

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

  FIELD_T (tag, 0);
  FIELD_BS (field_length, 0); //DXF 73, unused
  FIELD_RC (flags, 70); // 1 invisible, 2 constant, 4 verify, 8 preset

  SINCE (R_2007) {
    FIELD_B (lock_position_flag, 70);
  }

  // specific to ATTDEF
  SINCE (R_2010) {
    FIELD_RC (attdef_class_version, 280);
    VALUEOUTOFBOUNDS (attdef_class_version, 10)
  }
  FIELD_T (prompt, 0);

  COMMON_ENTITY_HANDLE_DATA;
  FIELD_HANDLE (style, 5, 0);

DWG_ENTITY_END

/* (4/12) */
DWG_ENTITY (BLOCK)

  SUBCLASS (AcDbBlockBegin)
  BLOCK_NAME (name, 2) //special pre-R13 naming rules

  COMMON_ENTITY_HANDLE_DATA;

#ifdef IS_DXF
  {
    Dwg_Object *o
        = _ent->ownerhandle && _ent->ownerhandle->obj
              ? _ent->ownerhandle->obj : NULL;
    VALUE_BL (0, 70);
    if (!o)
      o = dwg_ref_object (dwg, _ent->ownerhandle);
    if (!o || o->fixedtype != DWG_TYPE_BLOCK_HEADER)
      {
        Dwg_Bitcode_3RD nullpt = { 0.0, 0.0, 0.0 };
        VALUE_3BD (nullpt, 10);
      }
    else
      {
        Dwg_Object_BLOCK_HEADER *hdr = o->tio.object->tio.BLOCK_HEADER;
        VALUE_3BD (hdr->base_pt, 10);
      }
    BLOCK_NAME (name, 3); // special pre-R13 naming rules
    VALUE_TFF ("", 1);
  }
#endif

DWG_ENTITY_END

/* (5/13) */
DWG_ENTITY (ENDBLK)

  SUBCLASS (AcDbBlockEnd)
  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/* (6) */
DWG_ENTITY (SEQEND)

  //SUBCLASS (AcDbSequenceEnd) //unused
  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/* (7/14) */
DWG_ENTITY (INSERT)

  SUBCLASS (AcDbBlockReference)
#ifdef IS_DXF
    FIELD_HANDLE_NAME (block_header, 2, BLOCK_HEADER);
    if (FIELD_VALUE (has_attribs))
      FIELD_B (has_attribs, 66);
#endif
  PRE (R_13) {
    FIELD_2RD (ins_pt, 10);
  } else {
    FIELD_3DPOINT (ins_pt, 10);
  }

  VERSIONS (R_13, R_14)
    {
      FIELD_3BD_1 (scale, 41); // 42,43
    }

  SINCE (R_2000)
    {
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
          if (FIELD_VALUE (scale.x) == 1.0 &&
              FIELD_VALUE (scale.y) == 1.0 &&
              FIELD_VALUE (scale.z) == 1.0)
            {
              FIELD_VALUE (scale_flag) = 3;
              FIELD_BB (scale_flag, 0);
            }
          else if (FIELD_VALUE (scale.x) == FIELD_VALUE (scale.y) &&
                   FIELD_VALUE (scale.x) == FIELD_VALUE (scale.z))
            {
              FIELD_VALUE (scale_flag) = 2;
              FIELD_BB (scale_flag, 0);
              FIELD_RD (scale.x, 41);
            }
          else if (FIELD_VALUE (scale.x) == 1.0)
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

  PRE (R_13) {
    FIELD_RD (rotation, 50);
  } else {
    FIELD_BD (rotation, 50);
    FIELD_3DPOINT (extrusion, 210);
    FIELD_B (has_attribs, 0); // 66 above
  }

  SINCE (R_2004)
    {
      if (FIELD_VALUE (has_attribs))
        FIELD_BL (num_owned, 0);
    }

  COMMON_ENTITY_HANDLE_DATA;
  FIELD_HANDLE (block_header, 5, 0);
  //There is a typo in the spec. it says "R13-R200:".
  //I guess it means "R13-R2000:" (just like in MINSERT)
  VERSIONS (R_13, R_2000)
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
          HANDLE_VECTOR (attrib_handles, num_owned, 4, 0);
        }
    }

  if (FIELD_VALUE (has_attribs)) {
    FIELD_HANDLE (seqend, 3, 0);
  }

DWG_ENTITY_END

/* (8) 20.4.10*/
DWG_ENTITY (MINSERT)

  SUBCLASS (AcDbBlockReference)
#ifdef IS_DXF
    FIELD_HANDLE_NAME (block_header, 2, BLOCK_HEADER);
    if (FIELD_VALUE (has_attribs))
      FIELD_B (has_attribs, 66);
#endif
  FIELD_3DPOINT (ins_pt, 10);

  VERSIONS (R_13, R_14) {
    FIELD_3BD_1 (scale, 41);
  }

  SINCE (R_2000)
    {
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
          if (FIELD_VALUE (scale.x) == 1.0 &&
              FIELD_VALUE (scale.y) == 1.0 &&
              FIELD_VALUE (scale.z) == 1.0)
            {
              FIELD_VALUE (scale_flag) = 3;
              FIELD_BB (scale_flag, 0);
            }
          else if (FIELD_VALUE (scale.x) == 1.0)
             {
              FIELD_VALUE (scale_flag) = 1;
              FIELD_BB (scale_flag, 0);
              FIELD_DD (scale.y, 1.0, 42);
              FIELD_DD (scale.z, 1.0, 43);
             }
          else if (FIELD_VALUE (scale.x) == FIELD_VALUE (scale.y) &&
                   FIELD_VALUE (scale.x) == FIELD_VALUE (scale.z))
            {
              FIELD_VALUE (scale_flag) = 2;
              FIELD_BB (scale_flag, 0);
              FIELD_RD (scale.x, 41);
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

  FIELD_BD (rotation, 50);
  FIELD_3BD (extrusion, 210);
  FIELD_B (has_attribs, 0);

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
  IF_FREE_OR_VERSIONS (R_13, R_2000)
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
        HANDLE_VECTOR (attrib_handles, num_owned, 4, 0);
      }
    }

  if (FIELD_VALUE (has_attribs))
    {
      FIELD_HANDLE (seqend, 3, 0);
    }

DWG_ENTITY_END

//(9) Unknown

/* (10/20) */
DWG_ENTITY (VERTEX_2D)

  SUBCLASS (AcDbVertex)
  SUBCLASS (AcDb2dVertex)
  PRE (R_13)
  {
    FIELD_2RD (point, 10);
    if (R11OPTS (1))
      FIELD_RD (start_width, 40);
    if (R11OPTS (2))
      FIELD_RD (end_width, 41);
    if (R11OPTS (4))
      FIELD_RD (tangent_dir, 50);
    if (R11OPTS (8))
      FIELD_RC (flag, 70);
  }
  SINCE (R_13)
  {
    DXF {
      if (FIELD_VALUE (flag) != 0)
        FIELD_RC (flag, 70);
    } else {
      FIELD_RC (flag, 70);
    }
    FIELD_3BD (point, 10);

  /* Decoder and Encoder routines could be the same but then we
     wouldn't compress data when saving. So we explicitly implemented
     the encoder routine with the compression technique described in
     the spec. --Juca */
    DXF_OR_PRINT {
      if (FIELD_VALUE (flag) != 0) {
        if (FIELD_VALUE (start_width) != 0.0)
          FIELD_BD (start_width, 40);
        if (FIELD_VALUE (end_width) != 0.0)
          FIELD_BD (end_width, 41);
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
      if (FIELD_VALUE (start_width) && FIELD_VALUE (start_width)==FIELD_VALUE (end_width))
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
      if (FIELD_VALUE (flag) != 0 && FIELD_VALUE (bulge) != 0.0)
        FIELD_BD (bulge, 42);
    } else {
      FIELD_BD (bulge, 42);
    }
    SINCE (R_2010) {
      FIELD_BL (id, 91);
    }
    DXF {
      if (FIELD_VALUE (flag) != 0)
        FIELD_BD (tangent_dir, 50);
    } else {
      FIELD_BD (tangent_dir, 50);
    }
  }

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/*(11)*/
DWG_ENTITY (VERTEX_3D)

  SUBCLASS (AcDbVertex)
  SUBCLASS (AcDb3dPolylineVertex) //SUBCLASS (AcDb3dVertex)?
  FIELD_RC (flag, 0);
  FIELD_3BD (point, 10);
  DXF { FIELD_RC (flag, 70); }

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/*(12)*/
DWG_ENTITY (VERTEX_MESH)

  SUBCLASS (AcDbVertex)
  SUBCLASS (AcDbPolyFaceMeshVertex) //?
  FIELD_RC (flag, 0);
  FIELD_3BD (point, 10);
  DXF { FIELD_RC (flag, 70); }

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/*(13)*/
DWG_ENTITY (VERTEX_PFACE)

  SUBCLASS (AcDbVertex)
  SUBCLASS (AcDbPolyFaceMeshVertex)
  FIELD_RC (flag, 0);
  FIELD_3BD (point, 10);
  DXF { FIELD_RC (flag, 70); }

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/*(14)*/
DWG_ENTITY (VERTEX_PFACE_FACE)

  SUBCLASS (AcDbFaceRecord)
  DXF {
    BITCODE_3RD pt = { 0.0, 0.0, 0.0 };
    VALUE_3BD (pt, 10);
    VALUE_RC ((BITCODE_RC)128, 70);
    FIELD_BS (vertind[0], 71);
    if (FIELD_VALUE (vertind[1]))
      FIELD_BS (vertind[1], 72);
    if (FIELD_VALUE (vertind[2]))
      FIELD_BS (vertind[2], 73);
    if (FIELD_VALUE (vertind[3]))
      FIELD_BS (vertind[3], 74);
  } else {
    //FIELD_VALUE (pt) = { 0.0, 0.0, 0.0 };
    FIELD_VALUE (flag) = 128;
    FIELD_BS (vertind[0], 71);
    FIELD_BS (vertind[1], 72);
    FIELD_BS (vertind[2], 73);
    FIELD_BS (vertind[3], 74);
  }
  //TODO R13 has color_rs and ltype_rs for all vertices, not in DXF

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/*(15)*/
DWG_ENTITY (POLYLINE_2D)

  //SUBCLASS (AcDbCurve)
  SUBCLASS (AcDb2dPolyline)
  PRE (R_13)
  {
    if (R11OPTS (1))
      FIELD_CAST (flag, RC, RS, 70);
    if (R11OPTS (2))
      FIELD_RD (start_width, 40);
    //??
    if (R11OPTS (4))
      FIELD_RS (curve_type, 75);
    if (R11OPTS (8))
      FIELD_RD (end_width, 40);
  }
  SINCE (R_13)
  {
    DXF {
      FIELD_B (has_vertex, 66);
    }
    else {
      FIELD_VALUE (has_vertex) = 1;
    }
    DXF {
      if (FIELD_VALUE (flag) != 0)
        FIELD_BS (flag, 70);
      if (FIELD_VALUE (curve_type) != 0)
        FIELD_BS (curve_type, 75);
    } else {
      FIELD_BS (flag, 70);
      FIELD_BS (curve_type, 75);
    }
    DECODER_OR_ENCODER {
      FIELD_BD (start_width, 40);
      FIELD_BD (end_width, 41);
      FIELD_BT (thickness, 39);
      FIELD_BD (elevation, 30);
    }
    DXF {
      BITCODE_3RD pt = { 0.0, 0.0, 0.0 };
      pt.z = FIELD_VALUE (elevation);
      if (FIELD_VALUE (thickness != 0.0))
        FIELD_BT (thickness, 39);
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

  IF_FREE_OR_VERSIONS (R_13, R_2000)
    {
      FIELD_HANDLE (first_vertex, 4, 0);
      FIELD_HANDLE (last_vertex, 4, 0);
    }

  IF_FREE_OR_SINCE (R_2004)
    {
      HANDLE_VECTOR (vertex, num_owned, 3, 0);
    }

  IF_FREE_OR_SINCE (R_13)
    {
      FIELD_HANDLE (seqend, 3, 0);
    }

DWG_ENTITY_END

/*(16)*/
DWG_ENTITY (POLYLINE_3D)

  SUBCLASS (AcDb3dPolyline)
  DXF {
    FIELD_B (has_vertex, 66);
  }
  else {
    FIELD_VALUE (has_vertex) = 1;
  }
  FIELD_RC (curve_type, 75);
  FIELD_RC (flag, 70);

  SINCE (R_2004) {
    FIELD_BL (num_owned, 0);
  }

  COMMON_ENTITY_HANDLE_DATA;
  IF_FREE_OR_VERSIONS (R_13, R_2000)
    {
      FIELD_HANDLE (first_vertex, 4, 0);
      FIELD_HANDLE (last_vertex, 4, 0);
    }
  IF_FREE_OR_SINCE (R_2004)
    {
      HANDLE_VECTOR (vertex, num_owned, 3, 0);
    }
  FIELD_HANDLE (seqend, 3, 0);

DWG_ENTITY_END

/* (17/8) */
DWG_ENTITY (ARC)

  //SUBCLASS (AcDbCurve)
  SUBCLASS (AcDbCircle)
  PRE (R_13) {
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
    FIELD_BT (thickness, 39);
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
  PRE (R_13) {
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
    FIELD_BT (thickness, 39);
    FIELD_BE (extrusion, 210);
  }

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/* (19/1) */
DWG_ENTITY (LINE)

  //SUBCLASS (AcDbCurve)
  SUBCLASS (AcDbLine)
  PRE (R_13) {
    if (R11FLAG (4))
      FIELD_3RD (start, 10)
    else
      FIELD_2RD (start, 10)

    if (R11FLAG (4))
      FIELD_3RD (end, 11)
    else
      FIELD_2RD (end, 11)

    if (R11OPTS (1))
      FIELD_3RD (extrusion, 210);
    if (R11OPTS (2))
      FIELD_RD (thickness, 39);
  }
  VERSIONS (R_13, R_14)
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

  SINCE (R_13) {
    FIELD_BT (thickness, 39);
    FIELD_BE (extrusion, 210);
  }

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/**
 * DIMENSION_common declaration
 */
#ifndef COMMON_ENTITY_DIMENSION
#define COMMON_ENTITY_DIMENSION \
    SINCE (R_2010) \
      { \
        FIELD_RC (class_version, 280); /* 0=r2010 */ \
        VALUEOUTOFBOUNDS (class_version, 10) \
      } \
    DXF { \
      FIELD_VALUE (blockname) = dwg_dim_blockname (dwg, obj); \
      FIELD_BE (extrusion, 210); \
      FIELD_T (blockname, 2); \
      FIELD_3BD (def_pt, 10); \
    } else { \
      FIELD_3BD (extrusion, 210); \
    } \
    FIELD_2RD (text_midpt, 11); \
    FIELD_BD (elevation, 31); \
    DXF { \
      FIELD_RC (flag, 70); \
    } else { \
      FIELD_RC (flag1, 0); \
    } \
    DECODER { \
      BITCODE_RC flag = FIELD_VALUE (flag1); \
      flag &= 0xE0; /* clear the upper flag bits, and fix them: */ \
      flag = (flag & 1) ? flag & 0x7F : flag | 0x80; /* bit 7 is inverse of bit 0 */ \
      flag = (flag & 2) ? flag | 0x20 : flag & 0xDF; /* set bit 5 to bit 1 */ \
      if      (_obj->flag == DWG_TYPE_DIMENSION_ALIGNED)  flag |= 1; \
      else if (_obj->flag == DWG_TYPE_DIMENSION_ANG2LN)   flag |= 2; \
      else if (_obj->flag == DWG_TYPE_DIMENSION_DIAMETER) flag |= 3; \
      else if (_obj->flag == DWG_TYPE_DIMENSION_RADIUS)   flag |= 4; \
      else if (_obj->flag == DWG_TYPE_DIMENSION_ANG3PT)   flag |= 5; \
      else if (_obj->flag == DWG_TYPE_DIMENSION_ORDINATE) flag |= 6; \
      FIELD_VALUE (flag) = flag; \
    } \
    DXF { \
      if (dat->from_version >= R_2007) { \
        FIELD_T (user_text, 1); \
      } else if (_obj->user_text && strlen (_obj->user_text)) { \
        FIELD_TV (user_text, 1); \
      } \
    } else { \
      FIELD_T (user_text, 1); \
    } \
    FIELD_BD (text_rotation, 53); \
    FIELD_BD (horiz_dir, 51); \
    FIELD_3BD_1 (ins_scale, 0); \
    FIELD_BD (ins_rotation, 54); \
    SINCE (R_2000) \
      { \
        FIELD_BS (attachment, 71); \
        FIELD_BS (lspace_style, 72); \
        FIELD_BD (lspace_factor, 41); \
        FIELD_BD (act_measurement, 42); \
      } \
    SINCE (R_2007) \
      { \
        FIELD_B (unknown, 73); \
        FIELD_B (flip_arrow1, 74); \
        FIELD_B (flip_arrow2, 75); \
      } \
    FIELD_2RD (clone_ins_pt, 12); \
    DXF { \
      FIELD_HANDLE (dimstyle, 5, 3); \
    }
#endif

/*(20)*/
DWG_ENTITY (DIMENSION_ORDINATE)

  SUBCLASS (AcDbDimension)
  COMMON_ENTITY_DIMENSION
  SUBCLASS (AcDbOrdinateDimension)
  DECODER_OR_ENCODER {
    FIELD_3BD (def_pt, 10);
  }
  FIELD_3BD (feature_location_pt, 13);
  FIELD_3BD (leader_endpt, 14);
  FIELD_RC (flag2, 70);
  DECODER {
    BITCODE_RC flag = FIELD_VALUE (flag);
    flag = (FIELD_VALUE (flag2) & 1)
            ? flag | 0x80 : flag & 0xBF; /* set bit 6 */
    FIELD_VALUE (flag) = flag;
  }
  JSON { FIELD_RC (flag, 0); }

  COMMON_ENTITY_HANDLE_DATA;
  FIELD_HANDLE (dimstyle, 5, 0);
  FIELD_HANDLE (block, 5, 0);

DWG_ENTITY_END

/* (21/23) */
DWG_ENTITY (DIMENSION_LINEAR)

  // TODO PRE (R_R13)
  SUBCLASS (AcDbDimension)
  COMMON_ENTITY_DIMENSION
  JSON { FIELD_RC (flag, 0); }
  SUBCLASS (AcDbAlignedDimension)
  FIELD_3BD (_13_pt, 13);
  FIELD_3BD (_14_pt, 14);
  FIELD_3BD (def_pt, 10);
  FIELD_BD (ext_line_rotation, 52);
  FIELD_BD (dim_rotation, 50);
  SUBCLASS (AcDbRotatedDimension)

  COMMON_ENTITY_HANDLE_DATA;
  FIELD_HANDLE (dimstyle, 5, 0);
  FIELD_HANDLE (block, 5, 0);

DWG_ENTITY_END

/*(22)*/
DWG_ENTITY (DIMENSION_ALIGNED)

  SUBCLASS (AcDbDimension)
  COMMON_ENTITY_DIMENSION
  JSON { FIELD_RC (flag, 0); }
  SUBCLASS (AcDbAlignedDimension)
  FIELD_3BD (_13_pt, 13); // TODO: rename
  FIELD_3BD (_14_pt, 14); // TODO: rename
  DECODER_OR_ENCODER {
    FIELD_3BD (def_pt, 10);
  }
  FIELD_BD (ext_line_rotation, 0); /* 52 */

  COMMON_ENTITY_HANDLE_DATA;
  FIELD_HANDLE (dimstyle, 5, 0);
  FIELD_HANDLE (block, 5, 0);

DWG_ENTITY_END

/*(23)*/
DWG_ENTITY (DIMENSION_ANG3PT)

  SUBCLASS (AcDbDimension)
  COMMON_ENTITY_DIMENSION
  JSON { FIELD_RC (flag, 0); }
  SUBCLASS (AcDb3PointAngularDimension)
  DECODER_OR_ENCODER {
    FIELD_3BD (def_pt, 10);
  }
  FIELD_3BD (_13_pt, 13);
  FIELD_3BD (_14_pt, 14);
  FIELD_3BD (first_arc_pt, 15);

  COMMON_ENTITY_HANDLE_DATA;
  FIELD_HANDLE (dimstyle, 5, 0);
  FIELD_HANDLE (block, 5, 0);

DWG_ENTITY_END

/*(24)*/
DWG_ENTITY (DIMENSION_ANG2LN)

  SUBCLASS (AcDbDimension)
  COMMON_ENTITY_DIMENSION
  JSON { FIELD_RC (flag, 0); }
  SUBCLASS (AcDb2LineAngularDimension)
  FIELD_2RD (_16_pt, 16);
  FIELD_3BD (_13_pt, 13);
  FIELD_3BD (_14_pt, 14);
  FIELD_3BD (first_arc_pt, 15);
  DECODER_OR_ENCODER {
    FIELD_3BD (def_pt, 10);
  }

  COMMON_ENTITY_HANDLE_DATA;
  FIELD_HANDLE (dimstyle, 5, 0);
  FIELD_HANDLE (block, 5, 0);

DWG_ENTITY_END

/*(25)*/
DWG_ENTITY (DIMENSION_RADIUS)

  SUBCLASS (AcDbDimension)
  COMMON_ENTITY_DIMENSION
  JSON { FIELD_RC (flag, 0); }
  SUBCLASS (AcDbRadialDimension)
  DECODER_OR_ENCODER {
    FIELD_3BD (def_pt, 10);
  }
  FIELD_3BD (first_arc_pt, 15);
  FIELD_BD (leader_len, 40);

  COMMON_ENTITY_HANDLE_DATA;
  FIELD_HANDLE (dimstyle, 5, 0);
  FIELD_HANDLE (block, 5, 0);

DWG_ENTITY_END

/*(26)*/
DWG_ENTITY (DIMENSION_DIAMETER)

  SUBCLASS (AcDbDimension)
  COMMON_ENTITY_DIMENSION
  JSON { FIELD_RC (flag, 0); }
  SUBCLASS (AcDbDiametricDimension)
  FIELD_3BD (first_arc_pt, 15);
  DECODER_OR_ENCODER {
    FIELD_3BD (def_pt, 10);
  }
  FIELD_BD (leader_len, 40);

  COMMON_ENTITY_HANDLE_DATA;
  FIELD_HANDLE (dimstyle, 5, 0);
  FIELD_HANDLE (block, 5, 0);

DWG_ENTITY_END

/* (27/2) */
DWG_ENTITY (POINT)

  SUBCLASS (AcDbPoint)
  //TODO PRE (R_13)
  FIELD_BD (x, 10);
  FIELD_BD (y, 20);
  FIELD_BD (z, 30);
  FIELD_BT (thickness, 39);
  FIELD_BE (extrusion, 210);
  FIELD_BD (x_ang, 50);

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/* (28/22) */
DWG_ENTITY (_3DFACE)

  SUBCLASS (AcDbFace)
  // TODO PRE (R_R13)
  VERSIONS (R_13, R_14)
    {
      FIELD_3BD (corner1, 10);
      FIELD_3BD (corner2, 11);
      FIELD_3BD (corner3, 12);
      FIELD_3BD (corner4, 13);
      FIELD_BS (invis_flags, 70);
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
    }

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/*(29)*/
DWG_ENTITY (POLYLINE_PFACE)

  SUBCLASS (AcDbPolyFaceMesh)
  DXF {
    BITCODE_3RD pt = { 0.0, 0.0, 0.0 };
    FIELD_B (has_vertex, 66);
    KEY (elevation); VALUE_3BD (pt, 10);
    KEY (flag); VALUE_BL (64, 70);
  }
  else {
    FIELD_VALUE (has_vertex) = 1;
  }
  FIELD_BS (numverts, 71);
  FIELD_BS (numfaces, 72);

  SINCE (R_2004) {
    FIELD_BL (num_owned, 0);
  }

  COMMON_ENTITY_HANDLE_DATA;
  IF_FREE_OR_VERSIONS (R_13, R_2000)
    {
      FIELD_HANDLE (first_vertex, 4, 0);
      FIELD_HANDLE (last_vertex, 4, 0);
    }
  IF_FREE_OR_SINCE (R_2004)
    {
      HANDLE_VECTOR (vertex, num_owned, 4, 0);
    }
  FIELD_HANDLE (seqend, 3, 0);

DWG_ENTITY_END

/*(30)*/
DWG_ENTITY (POLYLINE_MESH)

  SUBCLASS (AcDbPolygonMesh)
  FIELD_BS (flag, 70);
  FIELD_BS (curve_type, 75);
  FIELD_BS (num_m_verts, 71);
  FIELD_BS (num_n_verts, 72);
  FIELD_BS (m_density, 73);
  FIELD_BS (n_density, 74);

  SINCE (R_2004) {
    FIELD_BL (num_owned, 0);
  }

  COMMON_ENTITY_HANDLE_DATA;
  VERSIONS (R_13, R_2000)
    {
      FIELD_HANDLE (first_vertex, 4, 0);
      FIELD_HANDLE (last_vertex, 4, 0);
    }
  IF_FREE_OR_SINCE (R_2004)
    {
      VALUEOUTOFBOUNDS (num_owned, 100000)
      HANDLE_VECTOR (vertex, num_owned, 4, 0);
    }
  FIELD_HANDLE (seqend, 3, 0);

DWG_ENTITY_END

/* (31/11) */
DWG_ENTITY (SOLID)

  SUBCLASS (AcDbTrace)
  PRE (R_13) {
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
    FIELD_BT (thickness, 39);
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
  PRE (R_13) {
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
    FIELD_BT (thickness, 39);
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

  DXF { FIELD_HANDLE (style, 5, 7); }
  SUBCLASS (AcDbShape)
  PRE (R_13) {
    FIELD_HANDLE (style, 5, 0);
    FIELD_2RD (ins_pt, 10);
    FIELD_RS (style_id, 0); // dxf: 2
    if (R11OPTS (1))
      FIELD_3RD (extrusion, 210);
    if (R11OPTS (2))
      FIELD_RD (ins_pt.z, 38);
  }
  LATER_VERSIONS {
    FIELD_3BD (ins_pt, 10);
    FIELD_BD (scale, 40);  // documented as size
    FIELD_BD (rotation, 50);
    FIELD_BD (width_factor, 41);
    FIELD_BD (oblique, 51);
    FIELD_BD (thickness, 39);
#ifdef IS_DXF
    {
      Dwg_Object *style;
      if (_obj->style)
        style = dwg_resolve_handle (dwg, _obj->style->absolute_ref);
      else
        {
          Dwg_Object_Ref *ctrlref = dwg->header_vars.STYLE_CONTROL_OBJECT;
          Dwg_Object *ctrl
            = ctrlref ? dwg_resolve_handle (dwg, ctrlref->absolute_ref) : NULL;
          Dwg_Object_STYLE_CONTROL *_ctrl
            = ctrl ? ctrl->tio.object->tio.STYLE_CONTROL : NULL;
          Dwg_Object_Ref *styleref = _ctrl && _obj->style_id < _ctrl->num_entries
                                     ? _ctrl->entries[_obj->style_id] // index
                                     : NULL;
          style = styleref ? dwg_resolve_handle (dwg, styleref->absolute_ref) : NULL;
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
  PRE (R_13) {
    FIELD_3RD (center, 10);
    FIELD_RD (width, 40);
    FIELD_RD (height, 41);
    FIELD_RS (on_off, 68);
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

  SINCE (R_2000)
    {
      FIELD_3BD (view_target, 17);
      FIELD_3BD (view_direction, 16);
      FIELD_BD (view_twist, 51);
      FIELD_BD (view_height, 45);
      FIELD_BD (lens_length, 42);
      FIELD_BD (front_clip_z, 43);
      FIELD_BD (back_clip_z, 44);
      FIELD_BD (snap_angle, 50);
      FIELD_2RD (view_center, 12);
      FIELD_2RD (snap_base, 13);
      FIELD_2RD (snap_spacing, 14);
      FIELD_2RD (grid_spacing, 15);
      FIELD_BS (circle_zoom, 72);
    }

  SINCE (R_2007) {
    FIELD_BS (grid_major, 61);
  }

  IF_FREE_OR_SINCE (R_2000)
    {
      FIELD_BL (num_frozen_layers, 0);
      FIELD_BL (status_flag, 90);
      FIELD_T (style_sheet, 1);
      FIELD_RC (render_mode, 281);
      FIELD_B (ucs_at_origin, 74);
      FIELD_B (ucs_per_viewport, 71);
      FIELD_3BD (ucs_origin, 110);
      FIELD_3BD (ucs_x_axis, 111);
      FIELD_3BD (ucs_y_axis, 112);
      FIELD_BD (ucs_elevation, 146);
      FIELD_BS (ucs_ortho_view_type, 79);
    }

  SINCE (R_2004) {
    FIELD_BS (shadeplot_mode, 170);
  }

  SINCE (R_2007)
    {
      FIELD_B (use_default_lights, 292);
      FIELD_RC (default_lighting_type, 282);
      FIELD_BD (brightness, 141);
      FIELD_BD (contrast, 142);
      FIELD_CMC (ambient_color, 63,421);
    }

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/*(35)*/
DWG_ENTITY (ELLIPSE)

  //SUBCLASS (AcDbCurve)
  SUBCLASS (AcDbEllipse)
  FIELD_3BD (center, 10);
  FIELD_3BD (sm_axis, 11);
  FIELD_3BD (extrusion, 210);
  FIELD_BD (axis_ratio, 40);
  FIELD_BD (start_angle, 41);
  FIELD_BD (end_angle, 42);

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/*(36)*/
DWG_ENTITY (SPLINE)

  //SUBCLASS (AcDbCurve)
  SUBCLASS (AcDbSpline)
  FIELD_BL (scenario, 0);
  UNTIL (R_2013) {
    if (FIELD_VALUE (scenario) != 1 && FIELD_VALUE (scenario) != 2)
      LOG_ERROR ("unknown scenario %d", FIELD_VALUE (scenario));
    DECODER {
      if (FIELD_VALUE (scenario) == 1)
        FIELD_VALUE (splineflags1) = 8;
      else if (FIELD_VALUE (scenario) == 2)
        FIELD_VALUE (splineflags1) = 9;
    }
  }
  SINCE (R_2013) {
    FIELD_BL (splineflags1, 0);
    FIELD_BL (knotparam, 0);
    if (FIELD_VALUE (splineflags1) & 1)
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
      FIELD_VALUE (flag) = 8 + //planar
        FIELD_VALUE (closed_b) +
        (FIELD_VALUE (periodic) << 1) +
        (FIELD_VALUE (rational) << 2) +
        (FIELD_VALUE (weighted) << 4) +
        ((FIELD_VALUE (splineflags1) & ~5) << 7);
      LOG_TRACE ("flag: %d [70]\n", FIELD_VALUE (flag));
    }
    FIELD_VECTOR (knots, BD, num_knots, 40);
    REPEAT (num_ctrl_pts, ctrl_pts, Dwg_SPLINE_control_point)
    REPEAT_BLOCK
        SUB_FIELD_3BD_inl (ctrl_pts[rcount1], xyz, 10);
        if (!FIELD_VALUE (weighted))
          FIELD_VALUE (ctrl_pts[rcount1].w) = 0; // skipped when encoding
        else
          SUB_FIELD_BD (ctrl_pts[rcount1], w, 41);
    END_REPEAT_BLOCK
    SET_PARENT_OBJ (ctrl_pts);
    END_REPEAT (ctrl_pts);
  }
  else { // bezier spline, scenario 2
    DECODER {
      FIELD_VALUE (flag) = 8 + 32 + // planar, not rational
        // ignore method fit points and closed bits
        ((FIELD_VALUE (splineflags1) & ~5) << 7);
      LOG_TRACE ("flag: %d [70]\n", FIELD_VALUE (flag));
    }
    FIELD_BD (fit_tol, 44); // def: 0.0000001
    FIELD_3BD (beg_tan_vec, 12);
    FIELD_3BD (end_tan_vec, 13);
    FIELD_BL (num_fit_pts, 74);
    FIELD_3DPOINT_VECTOR (fit_pts, num_fit_pts, 11);
  }

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

//TODO: 37, 38 and 39 are ACIS entities

#define WIRESTRUCT_fields(name)                       \
  FIELD_RC (name.type, 0);                            \
  FIELD_BL (name.selection_marker, 0);                \
  FIELD_BS (name.color, 0);                           \
  FIELD_BL (name.acis_index, 0);                      \
  FIELD_BL (name.num_points, 0);                      \
  FIELD_3DPOINT_VECTOR (name.points, name.num_points, 0); \
  FIELD_B (name.transform_present, 0);                \
  if (FIELD_VALUE (name.transform_present))            \
    {                                                 \
      FIELD_3BD (name.axis_x, 0);                     \
      FIELD_3BD (name.axis_y, 0);                     \
      FIELD_3BD (name.axis_z, 0);                     \
      FIELD_3BD (name.translation, 0);                \
      FIELD_BD (name.scale, 0);                       \
      FIELD_B (name.has_rotation, 0);                 \
      FIELD_B (name.has_reflection, 0);               \
      FIELD_B (name.has_shear, 0);                    \
    }

#if defined (IS_DECODER)

#define DECODE_3DSOLID decode_3dsolid (dat, hdl_dat, obj, (Dwg_Entity_3DSOLID *)_obj);

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
  int idx;
  int error = 0;

  FIELD_B (acis_empty, 0);
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
              FIELD_BL (block_size[i], 0);
              if (FIELD_VALUE (block_size[i]) > 0
                  && AVAIL_BITS (dat) > 8 * FIELD_VALUE (block_size[i]))
                {
                  FIELD_TFv (encr_sat_data[i], FIELD_VALUE (block_size[i]), 1);
                  total_size += FIELD_VALUE (block_size[i]);
                }
              else
                {
                  _obj->encr_sat_data[i] = NULL;
                  _obj->block_size[i] = 0;
                }
            }
          while (FIELD_VALUE (block_size[i++]) > 0 && AVAIL_BITS (dat) >= 16); // crc RS

          // de-obfuscate SAT data
          FIELD_VALUE (acis_data) = malloc (total_size + 1);
          num_blocks = i - 1;
          FIELD_VALUE (num_blocks) = num_blocks;
          LOG_TRACE ("num_blocks: " FORMAT_BL "\n", FIELD_VALUE (num_blocks));
          idx = 0;
          for ( i =0; i < num_blocks; i++)
            {
              for (j = 0; j < FIELD_VALUE (block_size[i]); j++)
                {
                  if (FIELD_VALUE (encr_sat_data[i][j] <= 32))
                    {
                      FIELD_VALUE (acis_data)[idx++]
                        = FIELD_VALUE (encr_sat_data[i][j]);
                    }
                  else
                    {
                      FIELD_VALUE (acis_data)[idx++]
                        = 159 - FIELD_VALUE (encr_sat_data[i][j]);
                    }
                }
            }
          FIELD_VALUE (acis_data)[idx] = '\0';
          // DXF 1 + 3 if >255
          LOG_TRACE ("acis_data:\n%s\n", FIELD_VALUE (acis_data));
        }
      else //if (FIELD_VALUE (version)==2)
        /* version 2, SAB: binary, unencrypted SAT format for ACIS 7.0/ShapeManager.
           ACIS versions:
           R14 release            106   (ACIS 1.6)
           R15 (2000) release     400   (ACIS 4.0)
           R18 (2004) release     20800 (ASM ShapeManager, forked from ACIS 7.0)
           R21 (2007) release     21200
           R24 (2010) release     21500
           R27 (2013) release     21800
           R?? (2018) release     223.0.1.1930
        */
        {
          FIELD_VALUE (acis_data) = NULL;
          //TODO string in strhdl, even <r2007
          FIELD_VALUE (num_blocks) = 2;
          LOG_TRACE ("num_blocks: 2\n");
          FIELD_VALUE (block_size) = calloc (3, sizeof (BITCODE_RL));
          FIELD_VALUE (encr_sat_data) = calloc (3, sizeof (char*));
          FIELD_TFv (encr_sat_data[0], 15, 1); // "ACIS BinaryFile"
          FIELD_VALUE (block_size[0]) = 15;
          FIELD_RL (block_size[1], 0);
          if (FIELD_VALUE (block_size[1]) > obj->size)
            {
              LOG_ERROR ("Invalid ACIS 2 SAB block_size[1] %d. Max. %d",
                         _obj->block_size[1], obj->size);
              _obj->block_size[1] = 0;
              return DWG_ERR_VALUEOUTOFBOUNDS;
            }
          // Binary SAB, unencrypted
          FIELD_TFv (encr_sat_data[1], FIELD_VALUE (block_size[1]), 1);
          total_size = FIELD_VALUE (block_size[1]);
        }

      FIELD_B (wireframe_data_present, 0);
      if (FIELD_VALUE (wireframe_data_present))
        {
          FIELD_B (point_present, 0);
          if (FIELD_VALUE (point_present))
            {
              FIELD_3BD (point, 0);
            }
          else
            {
              FIELD_VALUE (point.x) = 0;
              FIELD_VALUE (point.y) = 0;
              FIELD_VALUE (point.z) = 0;
            }
          FIELD_BL (num_isolines, 0);
          FIELD_B (isoline_present, 0);
          if (FIELD_VALUE (isoline_present))
            {
              FIELD_BL (num_wires, 0);
              REPEAT (num_wires, wires, Dwg_3DSOLID_wire)
              REPEAT_BLOCK
                  WIRESTRUCT_fields (wires[rcount1])
              END_REPEAT_BLOCK
              SET_PARENT_OBJ (wires)
              END_REPEAT (wires);
              FIELD_BL (num_silhouettes, 0);
              REPEAT (num_silhouettes, silhouettes, Dwg_3DSOLID_silhouette)
              REPEAT_BLOCK
                  SUB_FIELD_BL (silhouettes[rcount1], vp_id, 0);
                  SUB_FIELD_3BD (silhouettes[rcount1], vp_target, 0);
                  SUB_FIELD_3BD (silhouettes[rcount1], vp_dir_from_target, 0);
                  SUB_FIELD_3BD (silhouettes[rcount1], vp_up_dir, 0);
                  SUB_FIELD_B (silhouettes[rcount1], vp_perspective, 0);
                  SUB_FIELD_BL (silhouettes[rcount1], num_wires, 0);
                  REPEAT2 (silhouettes[rcount1].num_wires, silhouettes[rcount1].wires, Dwg_3DSOLID_wire)
                  REPEAT_BLOCK
                      WIRESTRUCT_fields (silhouettes[rcount1].wires[rcount2])
                  END_REPEAT_BLOCK
                  SET_PARENT_OBJ (silhouettes[rcount1].wires)
                  END_REPEAT (silhouettes[rcount1].wires);
              END_REPEAT_BLOCK
              SET_PARENT_OBJ (silhouettes)
              END_REPEAT (silhouettes);
            }
        }

      FIELD_B (acis_empty_bit, 0);
      if (!FIELD_VALUE (acis_empty_bit))
        {
          LOG_ERROR ("TODO: Implement parsing of ACIS data at the end "
                    "of 3dsolid object parsing (acis_empty_bit==0).");
        }

      if (FIELD_VALUE (version) > 1) {
        SINCE (R_2007) {
          FIELD_BL (unknown_2007, 0);
        }
      }

      COMMON_ENTITY_HANDLE_DATA;
      if (FIELD_VALUE (version) > 1) {
        SINCE (R_2007) {
          FIELD_HANDLE (history_id, ANYCODE, 350);
        }
      }
    }
  return error;
}
#else
#define DECODE_3DSOLID {}
#define FREE_3DSOLID {}
#endif //#if IS_DECODER

#ifdef IS_ENCODER

#define ENCODE_3DSOLID encode_3dsolid(dat, hdl_dat, obj, (Dwg_Entity_3DSOLID *)_obj);
static int encode_3dsolid (Bit_Chain* dat, Bit_Chain* hdl_dat,
                           Dwg_Object *restrict obj,
                           Dwg_Entity_3DSOLID *restrict _obj)
{
  Dwg_Data* dwg = obj->parent;
  //BITCODE_BL j;
  //BITCODE_BL vcount;
  BITCODE_BL i = 0;
  BITCODE_BL num_blocks = FIELD_VALUE (num_blocks);
  int idx = 0;
  int error = 0;

  FIELD_B (acis_empty, 0);
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
          if (!num_blocks)
            num_blocks = 100; // max
          if (!FIELD_VALUE (block_size))
            {
              if (!FIELD_VALUE (acis_data))
                {
                  VALUE_RL (0, 0);
                  return error;
                }
              FIELD_VALUE (block_size) = calloc (2, sizeof (BITCODE_BL));
              FIELD_VALUE (block_size[0]) = strlen ((char*)FIELD_VALUE (acis_data));
              FIELD_VALUE (block_size[1]) = 0;
              LOG_TRACE ("default block_size[0] = %d\n", (int)FIELD_VALUE (block_size[0]));
              num_blocks = 1;
            }
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
                  FIELD_VALUE (encr_sat_data[i])
                    = encrypt_sat1 (FIELD_VALUE (block_size[i]),
                                    FIELD_VALUE (acis_data), &idx);
                  LOG_TRACE ("encrypt_sat1 %d\n", i);
                }
              FIELD_BL (block_size[i], 0);
              FIELD_TF (encr_sat_data[i], FIELD_VALUE (block_size[i]), 1);
            }
        }
      else //if (FIELD_VALUE (version)==2)
        {
          LOG_TRACE ("acis_data:\n%s\n", FIELD_VALUE (acis_data));
          VALUE_RL (15, 0);
          if (!FIELD_VALUE (encr_sat_data) || !FIELD_VALUE (encr_sat_data[0]))
            bit_write_TF (dat, (BITCODE_TF)"ACIS BinaryFile", 15);
          else
            FIELD_TF (encr_sat_data[0], 15, 1);
          if (!FIELD_VALUE (block_size))
            {
              FIELD_VALUE (block_size) = calloc (3, sizeof (BITCODE_BL));
              FIELD_VALUE (block_size[0]) = 15;
              if (!FIELD_VALUE (acis_data))
                {
                  VALUE_RL (0, 0);
                  return error;
                }
              FIELD_VALUE (block_size[1]) = strlen ((char*)FIELD_VALUE (acis_data));
              LOG_TRACE ("default block_size[0] = %d\n", (int)FIELD_VALUE (block_size[1]));
            }
          if (!FIELD_VALUE (block_size[1]))
            {
              if (FIELD_VALUE (acis_data))
                FIELD_VALUE (block_size[1]) = strlen ((char*)FIELD_VALUE (acis_data));
              else if (FIELD_VALUE (encr_sat_data[1]))
                FIELD_VALUE (block_size[1]) = strlen (FIELD_VALUE (encr_sat_data[1]));
              else
                {
                  VALUE_RL (0, 0);
                  return error;
                }
            }
          FIELD_RL (block_size[1], 0);
          // Binary SAB, unencrypted
          FIELD_TF (encr_sat_data[1], FIELD_VALUE (block_size[1]), 1);
        }
    }
  return error;
}
#else
#define ENCODE_3DSOLID {}
#define FREE_3DSOLID {}
#endif //#if IS_ENCODER

#ifdef IS_FREE
static int free_3dsolid (Dwg_Object *restrict obj, Dwg_Entity_3DSOLID *restrict _obj)
{
  int error = 0;
  BITCODE_BL i;
  BITCODE_BL vcount, rcount1, rcount2;
  Bit_Chain *dat = &pdat;

  if (!FIELD_VALUE (acis_empty))
    {
      if (FIELD_VALUE (encr_sat_data))
        {
          for (i=0; i <= FIELD_VALUE (num_blocks); i++)
            {
              if (FIELD_VALUE (encr_sat_data[i]) != NULL)
                FIELD_TF (encr_sat_data[i], block_size[i], 0);
            }
        }
      FREE_IF (FIELD_VALUE (encr_sat_data));
      FREE_IF (FIELD_VALUE (block_size));
      FREE_IF (FIELD_VALUE (acis_data));

      REPEAT (num_wires, wires, Dwg_3DSOLID_wire)
      REPEAT_BLOCK
          WIRESTRUCT_fields (wires[rcount1])
      END_REPEAT_BLOCK
      SET_PARENT_OBJ (wires)
      END_REPEAT (wires);

      REPEAT (num_silhouettes, silhouettes, Dwg_3DSOLID_silhouette)
      REPEAT_BLOCK
          SUB_FIELD_BL (silhouettes[rcount1], vp_id, 0);
          SUB_FIELD_3BD (silhouettes[rcount1], vp_target, 0);
          SUB_FIELD_3BD (silhouettes[rcount1], vp_dir_from_target, 0);
          SUB_FIELD_3BD (silhouettes[rcount1], vp_up_dir, 0);
          SUB_FIELD_B (silhouettes[rcount1], vp_perspective, 0);
          SUB_FIELD_BL (silhouettes[rcount1], num_wires, 0);
          REPEAT2 (silhouettes[rcount1].num_wires, silhouettes[rcount1].wires, Dwg_3DSOLID_wire)
          REPEAT_BLOCK
              WIRESTRUCT_fields (silhouettes[rcount1].wires[rcount2])
          END_REPEAT_BLOCK
          END_REPEAT (silhouettes[rcount1].wires);
      END_REPEAT_BLOCK
      SET_PARENT_OBJ (silhouettes)
      END_REPEAT (silhouettes);
    }

  FIELD_B (acis_empty_bit, 0);
  if (FIELD_VALUE (version) > 1) {
    SINCE (R_2007) {
      FIELD_BL (unknown_2007, 0);
    }
  }
  COMMON_ENTITY_HANDLE_DATA;

  if (FIELD_VALUE (version) > 1) {
    SINCE (R_2007) {
      FIELD_HANDLE (history_id, ANYCODE, 350);
    }
  }
  return error;
}
#undef FREE_3DSOLID
#define FREE_3DSOLID free_3dsolid (obj, (Dwg_Entity_3DSOLID *)_obj)
#endif

#define ACTION_3DSOLID \
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
  FREE_3DSOLID

/*(37)*/
DWG_ENTITY (REGION)
  SUBCLASS (AcDbModelerGeometry)
  ACTION_3DSOLID;
DWG_ENTITY_END

/*(38)*/
DWG_ENTITY (_3DSOLID)
  SUBCLASS (AcDbModelerGeometry)
  ACTION_3DSOLID;
DWG_ENTITY_END

/*(39)*/
DWG_ENTITY (BODY)
  SUBCLASS (AcDbModelerGeometry)
  ACTION_3DSOLID;
DWG_ENTITY_END

/*(40)*/
DWG_ENTITY (RAY)

  //SUBCLASS (AcDbCurve)
  SUBCLASS (AcDbRay)
  FIELD_3BD (point, 10);
  FIELD_3BD (vector, 11);

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/*(41)*/
DWG_ENTITY (XLINE)

  //SUBCLASS (AcDbCurve)
  SUBCLASS (AcDbXline)
  FIELD_3BD (point, 10);
  FIELD_3BD (vector, 11);

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/*(42)*/
DWG_OBJECT (DICTIONARY)

#ifdef IS_DXF
  SUBCLASS (AcDbDictionary)
  SINCE (R_2000)
  {
    if (FIELD_VALUE (hard_owner))
      FIELD_RC (hard_owner, 280);
    FIELD_BS (cloning, 281);
  }
#else
  FIELD_BL (numitems, 0);
  VERSION (R_14)
    FIELD_RC (hard_owner, 0); // always 0
  SINCE (R_2000)
    {
      IF_ENCODE_FROM_EARLIER {
        FIELD_VALUE (cloning) = FIELD_VALUE (hard_owner) & 0xffff;
      }
      FIELD_BS (cloning, 281);
      FIELD_RC (hard_owner, 280);
    }
  VALUEOUTOFBOUNDS (numitems, 10000)
#endif

#ifdef IS_DXF
  if (FIELD_VALUE (itemhandles) && FIELD_VALUE (texts)) {
     REPEAT (numitems, texts, T)
      {
        int dxf = FIELD_VALUE (hard_owner) & 1 ? 360 : 350;
        FIELD_T (texts[rcount1], 3);
        VALUE_HANDLE (_obj->itemhandles[rcount1], itemhandles, 2, dxf);
      }
      END_REPEAT (texts)
    }
#else
  FIELD_VECTOR_T (texts, T, numitems, 3);
#endif

  START_OBJECT_HANDLE_STREAM;
#ifndef IS_DXF
  // or DXF 360 if hard_owner
  HANDLE_VECTOR_N (itemhandles, FIELD_VALUE (numitems), 2, 350);
#endif

DWG_OBJECT_END

// DXF as ACDBDICTIONARYWDFLT
DWG_OBJECT (DICTIONARYWDFLT)

#ifdef IS_DXF
  SUBCLASS (AcDbDictionary)
  SINCE (R_2000)
  {
    if (FIELD_VALUE (hard_owner))
      FIELD_RC (hard_owner, 280);
    FIELD_BS (cloning, 281);
  }
#else
  FIELD_BL (numitems, 0);
  VERSION (R_14)
    FIELD_RL (cloning_r14, 0); // always 0
  SINCE (R_2000)
    {
      IF_ENCODE_FROM_EARLIER {
        FIELD_VALUE (cloning) = FIELD_VALUE (cloning_r14) & 0xffff;
      }
      FIELD_BS (cloning, 281);
      FIELD_RC (hard_owner, 0);
    }
#endif
  VALUEOUTOFBOUNDS (numitems, 10000)
#ifdef IS_DXF
    if (FIELD_VALUE (itemhandles) && FIELD_VALUE (texts)) {
      REPEAT (numitems, texts, T)
      {
        int dxf = FIELD_VALUE (hard_owner) & 1 ? 360 : 350;
        FIELD_T (texts[rcount1], 3);
        VALUE_HANDLE (_obj->itemhandles[rcount1], itemhandles, 2, dxf);
      }
      END_REPEAT (texts)
    }
#else
  FIELD_VECTOR_T (texts, T, numitems, 3);
#endif

  START_OBJECT_HANDLE_STREAM;
#ifndef IS_DXF
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

/*(44)*/
DWG_ENTITY (MTEXT)

  DXF { UNTIL (R_2007) {
    FIELD_HANDLE (style, 5, 7);
  } }
  SUBCLASS (AcDbMText)
  FIELD_3BD (insertion_pt, 10);
  FIELD_3BD (extrusion, 210);
  FIELD_3BD (x_axis_dir, 11);

  FIELD_BD (rect_width, 41);
  SINCE (R_2007) {
    FIELD_BD (rect_height, 46);
  }

  FIELD_BD (text_height, 40);
  FIELD_BS (attachment, 71);
  FIELD_BS (drawing_dir, 72);
  FIELD_BD (extents_height, 42);
  FIELD_BD (extents_width, 43);
  FIELD_T (text, 1); // or 3 if >250
  /* doc error:
  UNTIL (R_2007) {
    FIELD_HANDLE (style, 5, 0);
  }
  */

  SINCE (R_2000)
    {
      FIELD_BS (linespace_style, 73);
      FIELD_BD (linespace_factor, 44);
      FIELD_B (unknown_bit, 0); //annotative?
    }

  SINCE (R_2004)
    {
      FIELD_BL (bg_fill_flag, 90);
      if (FIELD_VALUE (bg_fill_flag) & (dat->version <= R_2018 ? 1 : 0x10))
        {
          FIELD_BL (bg_fill_scale, 45); // def: 1.5
          FIELD_CMC (bg_fill_color, 63,421);
          FIELD_BL (bg_fill_trans, 441);
        }
    }
  SINCE (R_2018)
  {
    FIELD_B (annotative, 0);
    FIELD_BS (class_version, 0); // def: 0
    VALUEOUTOFBOUNDS (class_version, 10)
    FIELD_B (default_flag, 0);   // def: 1
    // redundant fields
    FIELD_BL (attachment, 71);
    FIELD_3BD (x_axis_dir, 11);
    FIELD_3BD (insertion_pt, 10);
    FIELD_BD (rect_width, 41);
    FIELD_BD (rect_height, 0);
    FIELD_BD (extents_width, 42);
    FIELD_BD (extents_height, 43);
    // end redundant fields

    DECODE_UNKNOWN_BITS
    FIELD_BL (column_type, 75);
    if (FIELD_VALUE (column_type)) //DEBUGGING
      {
        FIELD_BL (num_column_heights, 76);
        FIELD_BD (column_width, 48);
        FIELD_BD (gutter, 49);
        FIELD_B (auto_height, 79);
        FIELD_B (flow_reversed, 74);
        if (!FIELD_VALUE (auto_height) && FIELD_VALUE (column_type) == 2)
          {
            FIELD_VECTOR (column_heights, BD, num_column_heights, 50);
          }
      }
  }

  COMMON_ENTITY_HANDLE_DATA;
  FIELD_HANDLE (style, 5, 0);
  SINCE (R_2018)
    FIELD_HANDLE (appid, 5, 0);

DWG_ENTITY_END

/* (45) unstable */
DWG_ENTITY (LEADER)

  //SUBCLASS (AcDbCurve)
  SUBCLASS (AcDbLeader)
  FIELD_B (unknown_bit_1, 0);
  FIELD_BS (path_type, 72);
  FIELD_BS (annot_type, 73); //0: text, 1: tol, 2: insert, 3 (def): none
  FIELD_BL (num_points, 76);
  FIELD_3DPOINT_VECTOR (points, num_points, 10);
  FIELD_3DPOINT (origin, 0);
  FIELD_3DPOINT (extrusion, 210);
  FIELD_3DPOINT (x_direction, 211);
  FIELD_3DPOINT (inspt_offset, 212);

  VERSIONS (R_14, R_2007) {
    FIELD_3DPOINT (endptproj, 0);
  }
  VERSIONS (R_13, R_14) {
    FIELD_BD (dimgap, 0);
  }

  FIELD_BD (box_height, 40);
  FIELD_BD (box_width , 41);
  FIELD_B (hookline_dir, 74);
  FIELD_B (arrowhead_on, 71);
  FIELD_BS (arrowhead_type, 0);

  VERSIONS (R_13, R_14)
    {
      FIELD_BD (dimasz, 0);
      FIELD_B (unknown_bit_2, 0);
      FIELD_B (unknown_bit_3, 0);
      FIELD_BS (unknown_short_1, 0);
      FIELD_BS (byblock_color, 77);
      FIELD_B (hookline_on, 75);
      FIELD_B (unknown_bit_5, 0);
    }

  SINCE (R_2000)
    {
      FIELD_B (hookline_on, 75);
      FIELD_B (unknown_bit_5, 0);
    }

  COMMON_ENTITY_HANDLE_DATA;

  SINCE (R_13) {
    FIELD_HANDLE (associated_annotation, 2, 340);
  }
  FIELD_HANDLE (dimstyle, 5, 3); // ODA bug, DXF documented as 2

DWG_ENTITY_END

/*(46)*/
DWG_ENTITY (TOLERANCE)

  SUBCLASS (AcDbFcf)   // for Feature Control Frames
  DXF { FIELD_HANDLE (dimstyle, 5, 3); }
  VERSIONS (R_13, R_14)
    {
      FIELD_BS (unknown_short, 0); //spec-typo? Spec says S instead of BS.
      FIELD_BD (height, 0);
      FIELD_BD (dimgap, 0);
    }

  FIELD_3DPOINT (ins_pt, 10);
  FIELD_3DPOINT (x_direction, 11);
  DXF  { FIELD_BE (extrusion, 210); }
  else { FIELD_3DPOINT (extrusion, 210); }
  FIELD_T (text_string, 1);

  COMMON_ENTITY_HANDLE_DATA;
  FIELD_HANDLE (dimstyle, 5, 0);

DWG_ENTITY_END

/*(47)*/
DWG_ENTITY (MLINE)

  SUBCLASS (AcDbMline)
  DXF { FIELD_HANDLE (mlinestyle, 5, 340); }
  FIELD_BD (scale, 40);
  FIELD_RC (justification, 70); /* spec-typo? Spec says EC instead of RC */
  FIELD_3DPOINT (base_point, 10);
  FIELD_3DPOINT (extrusion, 210);
  FIELD_BS (flags, 71);
  FIELD_RCu (num_lines, 73); //aka linesinstyle
  FIELD_BS (num_verts, 72);
  VALUEOUTOFBOUNDS (num_verts, 5000)
  VALUEOUTOFBOUNDS (num_lines, 1000)

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
          //REPEAT3 (verts[rcount1].lines[rcount2].num_segparms,
          //        verts[rcount1].lines[rcount2].segparms,
          //        BITCODE_BD)
          //REPEAT_BLOCK
          //  SUB_FIELD_BD (verts[rcount1].lines[rcount2], segparms[rcount3], 41);
          //END_REPEAT_BLOCK
          //END_REPEAT (verts[rcount1].lines[rcount2].segparms);

          SUB_FIELD_BS (verts[rcount1].lines[rcount2], num_areafillparms, 75);
          VALUEOUTOFBOUNDS (verts[rcount1].lines[rcount2].num_areafillparms, 5000)
          FIELD_VECTOR (verts[rcount1].lines[rcount2].areafillparms, BD, verts[rcount1].lines[rcount2].num_areafillparms, 42)
          //REPEAT3 (verts[rcount1].lines[rcount2].num_areafillparms,
          //        verts[rcount1].lines[rcount2].areafillparms,
          //        BITCODE_BD)
          //REPEAT_BLOCK
          //  SUB_FIELD_BD (verts[rcount1].lines[rcount2], areafillparms[rcount3], 42);
          //END_REPEAT_BLOCK
          //END_REPEAT (verts[rcount1].lines[rcount2].areafillparms);
      END_REPEAT_BLOCK
      SET_PARENT (verts[rcount1].lines, &_obj->verts[rcount1])
      END_REPEAT (verts[rcount1].lines);
  END_REPEAT_BLOCK
  SET_PARENT_OBJ (verts)
  END_REPEAT (verts);

  COMMON_ENTITY_HANDLE_DATA;
  FIELD_HANDLE (mlinestyle, 5, 0);

DWG_ENTITY_END

/*(48)*/
DWG_OBJECT (BLOCK_CONTROL)

  DXF {
    VALUE_RL (FIELD_VALUE (num_entries) +
              (dwg->header_vars.BLOCK_RECORD_PSPACE ? 2 : 1), 70);
  } else {
    FIELD_BL (num_entries, 70);
  }

  CONTROL_HANDLE_STREAM;
  HANDLE_VECTOR (entries, num_entries, 2, 0);
  FIELD_HANDLE (model_space, 3, 0);
  FIELD_HANDLE (paper_space, 3, 0);

DWG_OBJECT_END

/* (49/1) */
DWG_OBJECT (BLOCK_HEADER)

  //DXF: the name must be from the block_entity!
  COMMON_TABLE_FLAGS (Block)
  DXF {
    // not allowed to be skipped, can be 0
    VALUE_HANDLE (_obj->layout, layout, 5, 340);
    // The DXF TABLE.BLOCK_RECORD only has this. More later in the BLOCKS section.
    return 0;
  }

  PRE (R_13)
  {
    FIELD_RD (base_pt.z, 30);
    FIELD_2RD (base_pt, 10);
    FIELD_RC (block_scaling, 0);
    FIELD_CAST (num_owned, RS, BL, 0);
    FIELD_RC (flag2, 0);
    FIELD_CAST (num_inserts, RS, RL, 0);
    FIELD_RS (flag3, 0);

    FIELD_VALUE (anonymous)    = FIELD_VALUE (flag) & 1;
    FIELD_VALUE (hasattrs)     = FIELD_VALUE (flag) & 2;
    FIELD_VALUE (blkisxref)    = FIELD_VALUE (flag) & 4;
    FIELD_VALUE (xrefoverlaid) = FIELD_VALUE (flag) & 8;
  }
  SINCE (R_13) {
    FIELD_B (anonymous, 0); // bit 1
    FIELD_B (hasattrs, 0);  // bit 2
    FIELD_B (blkisxref, 0); // bit 4
    FIELD_B (xrefoverlaid, 0); // bit 8
  }
  SINCE (R_2000) {
    FIELD_B (loaded_bit, 0); // bit 32
  }
  SINCE (R_13) {
    FIELD_VALUE (flag) = FIELD_VALUE (anonymous) |
                        FIELD_VALUE (hasattrs) << 1 |
                        FIELD_VALUE (blkisxref) << 2 |
                        FIELD_VALUE (xrefoverlaid) << 3 |
                        FIELD_VALUE (xrefdep) << 4 |
                        FIELD_VALUE (xrefref) << 6;
  }
  SINCE (R_2004) { // but not in 2007
    FIELD_BL (num_owned, 0);
    if (FIELD_VALUE (num_owned) > 0xf00000)
      {
        LOG_WARN ("Unreasonable high num_owned value")
      }
  }

  SINCE (R_13) {
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

  SINCE (R_2007)
    {
      FIELD_BS (insert_units, 70);
      FIELD_B (explodable, 280);
      FIELD_RC (block_scaling, 281);
    }

  SINCE (R_13) {
    START_OBJECT_HANDLE_STREAM;
    FIELD_HANDLE (null_handle, 5, 0);
    FIELD_HANDLE (block_entity, 3, 0);
  }

  VERSIONS (R_13, R_2000)
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

  SINCE (R_13) {
    FIELD_HANDLE (endblk_entity, 3, 0);
  }
  IF_FREE_OR_SINCE (R_2000)
    {
      if (FIELD_VALUE (num_inserts) && FIELD_VALUE (num_inserts) < 0xf00000) {
        HANDLE_VECTOR (inserts, num_inserts, ANYCODE, 0);
      }
      FIELD_HANDLE (layout, 5, 340);
    }

DWG_OBJECT_END

/*(50)*/
DWG_OBJECT (LAYER_CONTROL)

  DXF {
    VALUE_RL (FIELD_VALUE (num_entries)-1, 70);
  } else {
    FIELD_BL (num_entries, 70);
  }

  CONTROL_HANDLE_STREAM;
  HANDLE_VECTOR (entries, num_entries, 2, 0);

DWG_OBJECT_END

/* (51/2) */
DWG_OBJECT (LAYER)

  LAYER_TABLE_FLAGS (Layer);

  PRE (R_13)
  {
    FIELD_RS (color_rs, 62);  // color
    FIELD_RS (ltype_rs, 7);   // style

    DECODER {
      FIELD_VALUE (on)            = FIELD_VALUE (color_rs) >= 0;
      FIELD_VALUE (frozen)        = FIELD_VALUE (flag) & 1;
      FIELD_VALUE (frozen_in_new) = FIELD_VALUE (flag) & 2;
      FIELD_VALUE (locked)        = FIELD_VALUE (flag) & 4;
    }
  }
  VERSIONS (R_13, R_14)
  {
    FIELD_B (frozen, 0); // bit 1
    FIELD_B (on, 0);     // really: negate the color
    FIELD_B (frozen_in_new, 0);
    FIELD_B (locked, 0);
  }
  SINCE (R_2000) {
    int flag = FIELD_VALUE (flag);
    FIELD_BSx (flag, 0); // 70,290,370
    flag = FIELD_VALUE (flag);
    // contains frozen (1 bit), on (2 bit), frozen by default in new viewports (4 bit),
    // locked (8 bit), plotting flag (16 bit), and linewt (mask with 0x03E0)
    //FIELD_VALUE (flag) = (BITCODE_RC)FIELD_VALUE (flag_s) & 0xff;
    FIELD_VALUE (frozen) = flag & 1;
    FIELD_VALUE (on) = !(flag & 2);
    FIELD_VALUE (frozen_in_new) = flag & 4;
    FIELD_VALUE (locked) = flag & 8;
    FIELD_VALUE (plotflag) = flag & (1<<15) ? 1 : 0;
    FIELD_VALUE (linewt) = (flag & 0x03E0) >> 5;
    DXF_OR_PRINT {
      int lw = dxf_cvt_lweight (FIELD_VALUE (linewt));
      FIELD_B (plotflag, 290);
      JSON {
        FIELD_RC (linewt, 370);
      } else {
        KEY (linewt); VALUE_RC ((signed char)lw, 370);
      }
    }
  }
  FIELD_CMC (color, 62,420);
  VERSIONS (R_13, R_14)
  {
    DECODER { FIELD_VALUE (on) = FIELD_VALUE (color.index) >= 0; }
    FIELD_VALUE (flag) = FIELD_VALUE (frozen) |
      (FIELD_VALUE (frozen_in_new) << 1) |
      (FIELD_VALUE (locked) << 2) |
      (FIELD_VALUE (color.index) < 0 ? 32 : 0) |
      (FIELD_VALUE (xrefdep) << 4) |
      (FIELD_VALUE (xrefref) << 6);
  }

  START_OBJECT_HANDLE_STREAM;
  FIELD_HANDLE (xref, 5, 0);
  IF_FREE_OR_SINCE (R_2000) {
    FIELD_HANDLE (plotstyle, 5, 390);
  }
  IF_FREE_OR_SINCE (R_2007) {
    FIELD_HANDLE (material, ANYCODE, 347);
  }
  FIELD_HANDLE (ltype, 5, 6);
  //TODO handle: DXF 370
  //FIELD_HANDLE (null_handle, 5); // doc error?

DWG_OBJECT_END

/* STYLE table (52) */
DWG_OBJECT (STYLE_CONTROL)

  DXF {
    VALUE_RL (FIELD_VALUE (num_entries)-1, 70);
  } else {
    FIELD_BL (num_entries, 70);
  }

  CONTROL_HANDLE_STREAM;
  HANDLE_VECTOR (entries, num_entries, 2, 0);

DWG_OBJECT_END

/* (53/3) preR13+DXF: STYLE, documented as SHAPEFILE */
DWG_OBJECT (STYLE)

  COMMON_TABLE_FLAGS (TextStyle)

  SINCE (R_13)
  {
    FIELD_B (shape_file, 0);   //wrong oda doc
    FIELD_B (vertical, 0);     //
    FIELD_VALUE (flag) |= (FIELD_VALUE (vertical) ? 4 : 0) +
                         (FIELD_VALUE (shape_file) ? 1 : 0);
  }
  PRE (R_13)
  {
    FIELD_RD (fixed_height, 40);
    FIELD_RD (width_factor, 41);
    FIELD_RD (oblique_ang, 50);
    FIELD_RC (generation, 71);
    FIELD_RD (last_height, 42);
    FIELD_TFv (font_name, 128, 3);

    FIELD_VALUE (shape_file) = FIELD_VALUE (flag) & 4;
    FIELD_VALUE (vertical)   = FIELD_VALUE (flag) & 1;
  }
  LATER_VERSIONS
  {
    FIELD_BD (fixed_height, 40);
    FIELD_BD (width_factor, 41);
    FIELD_BD (oblique_ang, 50);
    FIELD_RC (generation, 71);
    FIELD_BD (last_height, 42);
    FIELD_T (font_name, 3);
    FIELD_T (bigfont_name, 4);
    //1001 1000 1071 mandatory r2007+ if .ttf
    //long truetype font’s pitch and family, charset, and italic and bold flags
    DXF {
      char _buf[256];
      char *s;
      if (_obj->font_name)
        {
          SINCE (R_2007) {
            s = bit_convert_TU ((BITCODE_TU)_obj->font_name);
            strncpy (_buf, s, 255);
            free (s);
          }
          else {
            strncpy (_buf, _obj->font_name, 255);
          }
          _buf[255] = '\0';
          if ((s = strstr (_buf, ".ttf")) ||
              (s = strstr (_buf, ".TTF")))
            {
              *s = 0;
              VALUE_TFF ("ACAD", 1001);
              VALUE_TFF (_buf, 1000);
              VALUE_RL (34, 1071);
            }
        }
    }

    START_OBJECT_HANDLE_STREAM;
    FIELD_HANDLE (extref, 5, 0);
  }

DWG_OBJECT_END

//(54): Unknown
//(55): Unknown

/*(56)*/
DWG_OBJECT (LTYPE_CONTROL)

  FIELD_BS (num_entries, 70);

  CONTROL_HANDLE_STREAM;
  HANDLE_VECTOR (entries, num_entries, 2, 0);
  FIELD_HANDLE (byblock, 3, 0);
  FIELD_HANDLE (bylayer, 3, 0);

DWG_OBJECT_END

/* (57/5) */
DWG_OBJECT (LTYPE)

  COMMON_TABLE_FLAGS (Linetype)

  PRE (R_13) {
    FIELD_TFv (description, 48, 3);
  }
  LATER_VERSIONS {
    FIELD_T (description, 3);
    FIELD_BD (pattern_len, 40); // total length
  }
  FIELD_RC (alignment, 72);
  FIELD_RCu (num_dashes, 73);
  REPEAT (num_dashes, dashes, Dwg_LTYPE_dash)
  REPEAT_BLOCK
      SUB_FIELD_BD (dashes[rcount1],length, 49);
      SUB_FIELD_BS (dashes[rcount1],complex_shapecode, 75);
      SUB_FIELD_HANDLE (dashes[rcount1],style, 5, 340);
      SUB_FIELD_RD (dashes[rcount1],x_offset, 44);
      SUB_FIELD_RD (dashes[rcount1],y_offset, 45);
      SUB_FIELD_BD (dashes[rcount1],scale, 46);
      SUB_FIELD_BD (dashes[rcount1],rotation, 50);
      SUB_FIELD_BSx (dashes[rcount1],shape_flag, 74);
      DECODER {
        if (FIELD_VALUE (dashes[rcount1].shape_flag) & 0x4)
          FIELD_VALUE (has_strings_area) = 1;
        PRE (R_13) {
          FIELD_VALUE (pattern_len) += FIELD_VALUE (dashes[rcount1].length);
        }
      }
  END_REPEAT_BLOCK
  SET_PARENT_OBJ (dashes)
  END_REPEAT (dashes);  // there's a 2nd loop below, don't free

  UNTIL (R_2004) {
    JSON {
      if (FIELD_VALUE (has_strings_area))
        FIELD_BINARY (strings_area, 256, 0);
    }
    else {
      FIELD_BINARY (strings_area, 256, 0);
    }
  }
  LATER_VERSIONS {
    if (FIELD_VALUE (has_strings_area))
      FIELD_BINARY (strings_area, 512, 0);
  }

  START_OBJECT_HANDLE_STREAM;
  FIELD_HANDLE (extref_handle, 5, 0);

DWG_OBJECT_END

//(58): Unknown
//(59): Unknown

/*(60)*/
DWG_OBJECT (VIEW_CONTROL)

  DXF {
    VALUE_RL (FIELD_VALUE (num_entries)-1, 70);
  } else {
    FIELD_BL (num_entries, 70);
  }

  CONTROL_HANDLE_STREAM;
  HANDLE_VECTOR (entries, num_entries, 2, 0);

DWG_OBJECT_END

/* (61/6) */
DWG_OBJECT (VIEW)

  COMMON_TABLE_FLAGS (View)

  PRE (R_13)
  {
    FIELD_RD (height, 40);
    FIELD_2RD (center, 10);
    FIELD_RD (width, 41);
    FIELD_3RD (target, 12);
    FIELD_3RD (direction, 11);
    FIELD_CAST (VIEWMODE, RS, 4BITS, 71);
    FIELD_RD (lens_length, 42);
    FIELD_RD (front_clip, 43);
    FIELD_RD (back_clip, 44);
    FIELD_RD (twist_angle, 50);
  }
  LATER_VERSIONS
  {
    FIELD_BD (height, 40);
    FIELD_BD (width, 41);
    FIELD_2RD (center, 20);
    FIELD_3BD (target, 12);
    FIELD_3BD (direction, 11);
    FIELD_BD (twist_angle, 50);
    FIELD_BD (lens_length, 42);
    FIELD_BD (front_clip, 43);
    FIELD_BD (back_clip, 44);
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
      //TODO FIELD_VALUE (ambient_color.byte) = ?; //+ name, book_name
    }
    FIELD_B (use_default_lights, 292);
    FIELD_RC (default_lightning_type, 282);
    FIELD_BD (brightness, 141);
    FIELD_BD (contrast, 142);
    FIELD_CMC (ambient_color, 63,421);
  }
  SINCE (R_13) {
    FIELD_B (pspace_flag, 0);
    FIELD_VALUE (flag) =
      FIELD_VALUE (pspace_flag) |
      FIELD_VALUE (xrefdep) << 4 |
      (FIELD_VALUE (xrefindex_plus1)>0 ? 32 : 0) |
      FIELD_VALUE (xrefref) << 6;
  }
  SINCE (R_2000)
    {
      FIELD_B (associated_ucs, 72);

      if (FIELD_VALUE (associated_ucs) & 1)
        {
          FIELD_3BD (origin, 110);
          FIELD_3BD (x_direction, 111);
          FIELD_3BD (y_direction, 112);
          FIELD_BD (elevation, 146);
          FIELD_BS (orthographic_view_type, 79);
        }
    }

  SINCE (R_2007) {
    FIELD_B (camera_plottable, 73);
  }
  SINCE (R_13) {
    START_OBJECT_HANDLE_STREAM;
    FIELD_HANDLE (null_handle, 5, 0);
  }
  SINCE (R_2007) {
    FIELD_HANDLE (background, 4, 332);
    FIELD_HANDLE (visualstyle, 5, 348);
    FIELD_HANDLE (sun, 3, 361);
  }
  IF_FREE_OR_SINCE (R_2000)
    {
      if (FIELD_VALUE (associated_ucs) & 1)
        {
          FIELD_HANDLE (base_ucs, 5, 346);
          FIELD_HANDLE (named_ucs, 5, 345);
        }
    }
  SINCE (R_2007) {
    FIELD_HANDLE (livesection, 4, 334);
  }

DWG_OBJECT_END

/*(62)*/
DWG_OBJECT (UCS_CONTROL)

  FIELD_BS (num_entries, 70); //BS or BL?

  CONTROL_HANDLE_STREAM;
  HANDLE_VECTOR (entries, num_entries, 2, 0);

DWG_OBJECT_END

/* (63/7) */
DWG_OBJECT (UCS)

  COMMON_TABLE_FLAGS (Ucs)

  PRE (R_13)
  {
    FIELD_3RD (origin, 10);
    FIELD_3RD (x_direction, 11);
    FIELD_3RD (y_direction, 12);
  }
  LATER_VERSIONS
  {
    FIELD_3BD (origin, 10);
    FIELD_3BD (x_direction, 11);
    FIELD_3BD (y_direction, 12);
  }

  SINCE (R_2000)
  {
    FIELD_BD (elevation, 146);
    FIELD_BS (orthographic_view_type, 71);
    FIELD_BS (orthographic_type, 71); // on pairs with 13?
  }

  IF_FREE_OR_SINCE (R_13)
  {
    START_OBJECT_HANDLE_STREAM;
    FIELD_HANDLE (null_handle, 5, 0);
  }
  IF_FREE_OR_SINCE (R_2000)
  {
    FIELD_HANDLE (base_ucs, ANYCODE, 346);
    FIELD_HANDLE (named_ucs, 5, 345);
  }

DWG_OBJECT_END

/* (0x40/64) */
DWG_OBJECT (VPORT_CONTROL)

  DXF {
    VALUE_RL (FIELD_VALUE (num_entries)-1, 70);
  } else {
    FIELD_BS (num_entries, 70);
  }

  CONTROL_HANDLE_STREAM;
  HANDLE_VECTOR (entries, num_entries, 2, 0);

DWG_OBJECT_END

/* 0x41/65 /8 */
DWG_OBJECT (VPORT)

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
  FIELD_RD (aspect_ratio, 41); // = viewwidth / VIEWSIZE
  FIELD_RD (lens_length, 42);
  FIELD_RD (front_clip, 43);
  FIELD_RD (back_clip, 44);
  FIELD_RD (SNAPANG, 50);
  FIELD_RD (view_twist, 51);

  PRE (R_13) {
    FIELD_RC (UCSFOLLOW, 71);
  }
  else {
    FIELD_VALUE (VIEWMODE) |= ((FIELD_VALUE (UCSFOLLOW) << 2) | FIELD_VALUE (UCSVP));
    FIELD_4BITS (VIEWMODE, 71); // UCSFOLLOW is bit 3 of 71, UCSVP bit 0
  }
  FIELD_RS (circle_zoom, 72);
  FIELD_RC (FASTZOOM, 73);
  FIELD_RC (UCSICON, 74);
  FIELD_RC (GRIDMODE, 76);
  FIELD_CAST (SNAPMODE, RS, B, 75);
  FIELD_RC (SNAPSTYLE, 77);
  FIELD_RS (SNAPISOPAIR, 78);
  SINCE (R_2000) {
    FIELD_RC (render_mode, 281);
  }

  IF_FREE_OR_SINCE (R_2000)
  {
    FIELD_3BD (ucs_origin, 110);
    FIELD_3BD (ucs_x_axis, 111);
    FIELD_3BD (ucs_y_axis, 112);
    // TODO: skip if empty
    FIELD_HANDLE (named_ucs, 5, 345);
    if (FIELD_VALUE (ucs_orthografic_type))
      FIELD_HANDLE (base_ucs, 5, 346);
    FIELD_BS (ucs_orthografic_type, 79);
    FIELD_BD (ucs_elevation, 146);
  }
  IF_FREE_OR_SINCE (R_2007)
  {
    FIELD_HANDLE (visualstyle, 5, 348);
    FIELD_BS (grid_flags, 60);
    FIELD_BS (grid_major, 61);
    FIELD_HANDLE (background, 4, 332);
    FIELD_HANDLE (sun, 5, 361); //was shade_plot_handle

    IF_ENCODE_FROM_EARLIER {
      FIELD_VALUE (use_default_lights) = 1;
      FIELD_VALUE (default_lightning_type) = 1;
      FIELD_VALUE (ambient_color.index) = 250;
      //TODO FIELD_VALUE (ambient_color.rgb) = ?;
      //TODO FIELD_VALUE (ambient_color.byte) = ?; //+ name, book_name
    }
    FIELD_B (use_default_lights, 292);
    FIELD_RC (default_lightning_type, 282);
    FIELD_BD (brightness, 141);
    FIELD_BD (contrast, 142);
    //TODO: 63, 421, 423 only when non-black
    FIELD_CMC (ambient_color, 63,421); // +421, 431
  }
  //TODO 1001 1070
  REACTORS (4);
  XDICOBJHANDLE (3);

  }
  /* end of DXF: now DWG */
  else {

  PRE (R_13)
  { // TODO verify
    FIELD_RD (VIEWSIZE, 40);
    FIELD_RD (aspect_ratio, 41);
    DECODER {
      FIELD_VALUE (viewwidth) = FIELD_VALUE (aspect_ratio) * FIELD_VALUE (VIEWSIZE);
      LOG_TRACE ("viewwidth: %f (calc)\n", FIELD_VALUE (viewwidth))
    }
    FIELD_2RD (VIEWCTR, 12);
    FIELD_3RD (view_target, 17);
    FIELD_3RD (VIEWDIR, 16);
    FIELD_RD (view_twist, 51);
    FIELD_RD (lens_length, 42);
    FIELD_RD (front_clip, 43);
    FIELD_RD (back_clip, 44);
    FIELD_CAST (VIEWMODE, RS, 4BITS, 71);

    FIELD_2RD (lower_left, 10);
    FIELD_2RD (upper_right, 11);
    FIELD_RC (UCSFOLLOW, 71);
    FIELD_RS (circle_zoom, 72); //circle sides
    FIELD_RC (FASTZOOM, 73);
    FIELD_RC (UCSICON, 74);
    FIELD_RC (GRIDMODE, 76);
    FIELD_2RD (GRIDUNIT, 15);
    FIELD_CAST (SNAPMODE, RS, B, 75);
    FIELD_RC (SNAPSTYLE, 77);
    FIELD_RS (SNAPISOPAIR, 78);
    FIELD_RD (SNAPANG, 50);
    FIELD_2RD (SNAPBASE, 13);
    FIELD_2RD (SNAPUNIT, 14);
  }
  else
  {
    FIELD_BD (VIEWSIZE, 40); // i.e view height
    FIELD_BD (viewwidth, 0);
    DECODER {
      FIELD_VALUE (aspect_ratio) = FIELD_VALUE (VIEWSIZE) == 0.0
        ? 0.0
        : FIELD_VALUE (viewwidth) / FIELD_VALUE (VIEWSIZE);
      LOG_TRACE ("aspect_ratio: %f (calc)\n", FIELD_VALUE (aspect_ratio))
    }
    JSON {
      FIELD_BD (aspect_ratio, 0);
    }
    FIELD_2RD (VIEWCTR, 12);
    FIELD_3BD (view_target, 17);
    FIELD_3BD (VIEWDIR, 16);
    FIELD_BD (view_twist, 51);
    FIELD_BD (lens_length, 42);
    FIELD_BD (front_clip, 43);
    FIELD_BD (back_clip, 44);
    FIELD_4BITS (VIEWMODE, 71);

    SINCE (R_2000) {
      FIELD_RC (render_mode, 281);
    }
    SINCE (R_2007)
    {
      IF_ENCODE_FROM_EARLIER {
        FIELD_VALUE (use_default_lights) = 1;
        FIELD_VALUE (default_lightning_type) = 1;
        FIELD_VALUE (ambient_color.index) = 250;
        //TODO FIELD_VALUE (ambient_color.rgb) = ?;
        //TODO FIELD_VALUE (ambient_color.byte) = ?; //+ name, book_name
      }
      FIELD_B (use_default_lights, 292);
      FIELD_RC (default_lightning_type, 282);
      FIELD_BD (brightness, 141);
      FIELD_BD (contrast, 142);
      FIELD_CMC (ambient_color, 63,421); // +421, 431
    }

    FIELD_2RD (lower_left, 10);
    FIELD_2RD (upper_right, 11);
    FIELD_B (UCSFOLLOW, 0); // bit 3 of 71
    FIELD_BS (circle_zoom, 72);
    FIELD_B (FASTZOOM, 73);
    FIELD_BB (UCSICON, 74);
    FIELD_B (GRIDMODE, 76);
    FIELD_2RD (GRIDUNIT, 15);
    FIELD_B (SNAPMODE, 75);
    FIELD_B (SNAPSTYLE, 77);
    FIELD_BS (SNAPISOPAIR, 78);
    FIELD_BD (SNAPANG, 50);
    FIELD_2RD (SNAPBASE, 13);
    FIELD_2RD (SNAPUNIT, 14);

    SINCE (R_2000)
    {
      FIELD_B (unknown, 0);
      FIELD_B (UCSVP, 65); // bit 0 of 71
      FIELD_3BD (ucs_origin, 110);
      FIELD_3BD (ucs_x_axis, 111);
      FIELD_3BD (ucs_y_axis, 112);
      FIELD_BD (ucs_elevation, 146);
      FIELD_BS (ucs_orthografic_type, 79);
    }

    SINCE (R_2007)
    {
      FIELD_BS (grid_flags, 60);
      FIELD_BS (grid_major, 61);
    }

    START_OBJECT_HANDLE_STREAM;
    FIELD_HANDLE (null_handle, 5, 0);
  }

  IF_FREE_OR_SINCE (R_2007)
    {
      FIELD_HANDLE (background, 4, 332); //soft ptr
      FIELD_HANDLE (visualstyle, 5, 348); //hard ptr
      FIELD_HANDLE (sun, 3, 361); //hard owner
    }

  IF_FREE_OR_SINCE (R_2000)
    {
      FIELD_HANDLE (named_ucs, 5, 345);
      FIELD_HANDLE (base_ucs, 5, 346);
    }
 }

DWG_OBJECT_END

/*(66)*/
DWG_OBJECT (APPID_CONTROL)

  FIELD_BS (num_entries, 70);

  CONTROL_HANDLE_STREAM;
  HANDLE_VECTOR (entries, num_entries, 2, 0);

DWG_OBJECT_END

/* (67/9) Registered Apps */
DWG_OBJECT (APPID)

  COMMON_TABLE_FLAGS (RegApp)

  SINCE (R_13) {
    DXF {
      if (FIELD_VALUE (unknown))
        FIELD_RC (unknown, 71); // not in DXF if 0. has_something
    } else {
      FIELD_RC (unknown, 71);
    }
  }

  START_OBJECT_HANDLE_STREAM;
  FIELD_HANDLE (null_handle, 5, 0);

DWG_OBJECT_END

/*(68)*/
DWG_OBJECT (DIMSTYLE_CONTROL)

  FIELD_BS (num_entries, 70);
  SUBCLASS (AcDbDimStyleTable)
  SINCE (R_2000)
    { /* number of additional hard handles, undocumented */
      FIELD_RCu (num_morehandles, 71);
    }

  CONTROL_HANDLE_STREAM;
  HANDLE_VECTOR (entries, num_entries, 2, 0);
  HANDLE_VECTOR (morehandles, num_morehandles, 5, 340);

DWG_OBJECT_END

/* (69/10) */
DWG_OBJECT (DIMSTYLE)

  COMMON_TABLE_FLAGS (DimStyle)

  PRE (R_13)
    {
      FIELD_RC (DIMTOL, 71);
      FIELD_RC (DIMLIM, 72);
      FIELD_RC (DIMTIH, 73);
      FIELD_RC (DIMTOH, 74);
      FIELD_RC (DIMSE1, 75);
      FIELD_RC (DIMSE2, 76);
      FIELD_RC (DIMALT, 170);
      FIELD_RC (DIMTOFL, 172);
      FIELD_RC (DIMSAH, 173);
      FIELD_RC (DIMTIX, 174);
      FIELD_RC (DIMSOXD, 175);
      FIELD_CAST (DIMALTD, RC, BS, 171);
      FIELD_CAST (DIMZIN, RC, BS, 78);
      FIELD_RC (DIMSD1, 281);
      FIELD_RC (DIMSD2, 282);
      FIELD_CAST (DIMTOLJ, RC, BS, 283);
      FIELD_CAST (DIMJUST, RC, BS, 280);
      FIELD_CAST (DIMFIT, RC, BS, 287);
      FIELD_RC (DIMUPT, 288);
      FIELD_CAST (DIMTZIN, RC, BS, 284);
      FIELD_CAST (DIMMALTZ, RC, BS, 285);
      FIELD_CAST (DIMMALTTZ, RC, BS, 286);
      FIELD_CAST (DIMTAD, RC, BS, 77);
      FIELD_RS (DIMUNIT, 270);
      FIELD_RS (DIMAUNIT, 275);
      FIELD_RS (DIMDEC, 271);
      FIELD_RS (DIMTDEC, 272);
      FIELD_RS (DIMALTU, 273);
      FIELD_RS (DIMALTTD, 274);
      FIELD_RD (DIMSCALE, 40);
      FIELD_RD (DIMASZ, 41);
      FIELD_RD (DIMEXO, 42);
      FIELD_RD (DIMDLI, 43);
      FIELD_RD (DIMEXE, 44);
      FIELD_RD (DIMRND, 45);
      FIELD_RD (DIMDLE, 46);
      FIELD_RD (DIMTP, 47);
      FIELD_RD (DIMTM, 48);
      FIELD_RD (DIMTXT, 140);
      FIELD_RD (DIMCEN, 141);
      FIELD_RD (DIMTSZ, 142);
      FIELD_RD (DIMALTF, 143);
      FIELD_RD (DIMLFAC, 144);
      FIELD_RD (DIMTVP, 145);
      FIELD_RD (DIMTFAC, 146);
      FIELD_RD (DIMGAP, 147);
      FIELD_TV (DIMPOST, 3);
      FIELD_TV (DIMAPOST, 4); //??
      FIELD_TV (DIMBLK_T, 5);
      FIELD_TV (DIMBLK1_T, 6);
      FIELD_TV (DIMBLK2_T, 7);
      FIELD_CAST (DIMCLRD_N, RC, RS, 176);
      FIELD_CAST (DIMCLRE_N, RC, RS, 177);
      FIELD_CAST (DIMCLRT_N, RC, RS, 178);
    }
  VERSIONS (R_13, R_14)
    {
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
      FIELD_CAST (DIMMALTZ, RC, BS, 285);
      FIELD_CAST (DIMMALTTZ, RC, BS, 286);
      FIELD_CAST (DIMTAD, RC, BS, 77);
      FIELD_BS (DIMUNIT, 270);
      FIELD_BS (DIMAUNIT, 275);
      FIELD_BS (DIMDEC, 271);
      FIELD_BS (DIMTDEC, 272);
      FIELD_BS (DIMALTU, 273);
      FIELD_BS (DIMALTTD, 274);
      FIELD_BD (DIMSCALE, 40);
      FIELD_BD (DIMASZ, 41);
      FIELD_BD (DIMEXO, 42);
      FIELD_BD (DIMDLI, 43);
      FIELD_BD (DIMEXE, 44);
      FIELD_BD (DIMRND, 45);
      FIELD_BD (DIMDLE, 46);
      FIELD_BD (DIMTP, 47);
      FIELD_BD (DIMTM, 48);
      FIELD_BD (DIMTXT, 140);
      FIELD_BD (DIMCEN, 141);
      FIELD_BD (DIMTSZ, 142);
      FIELD_BD (DIMALTF, 143);
      FIELD_BD (DIMLFAC, 144);
      FIELD_BD (DIMTVP, 145);
      FIELD_BD (DIMTFAC, 146);
      FIELD_BD (DIMGAP, 147);
      FIELD_TV (DIMPOST, 3);
      FIELD_TV (DIMAPOST, 4);
      FIELD_TV (DIMBLK_T, 5);
      FIELD_TV (DIMBLK1_T, 6);
      FIELD_TV (DIMBLK2_T, 7);
      FIELD_CMC (DIMCLRD, 176,0);
      FIELD_CMC (DIMCLRE, 177,0);
      FIELD_CMC (DIMCLRT, 178,0);
    }
  else FREE {
      FIELD_TV (DIMBLK_T, 5);
      FIELD_TV (DIMBLK1_T, 6);
      FIELD_TV (DIMBLK2_T, 7);
  }
  IF_FREE_OR_SINCE (R_2000)
    {
      FIELD_T (DIMPOST, 3);
      FIELD_T (DIMAPOST, 4);
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

  SINCE (R_2007)
    {
      FIELD_BD (DIMFXL, 49);
      FIELD_BD (DIMJOGANG, 50);
      FIELD_BS (DIMTFILL, 69);
      FIELD_CMC (DIMTFILLCLR, 70,0);
    }

  SINCE (R_2000)
    {
      FIELD_B (DIMTOL, 71);
      FIELD_B (DIMLIM, 72);
      FIELD_B (DIMTIH, 73);
      FIELD_B (DIMTOH, 74);
      FIELD_B (DIMSE1, 75);
      FIELD_B (DIMSE2, 76);
      FIELD_BS (DIMTAD, 77);
      FIELD_BS (DIMZIN, 78);
      FIELD_BS (DIMAZIN, 79);
    }

  SINCE (R_2007)
    {
      FIELD_BS (DIMARCSYM, 90);
    }

  IF_FREE_OR_SINCE (R_2000)
    {
      FIELD_BD (DIMTXT, 140);
      FIELD_BD (DIMCEN, 141);
      FIELD_BD (DIMTSZ, 142);
      FIELD_BD (DIMALTF, 143);
      FIELD_BD (DIMLFAC, 144);
      FIELD_BD (DIMTVP, 145);
      FIELD_BD (DIMTFAC, 146);
      FIELD_BD (DIMGAP, 147);
      FIELD_BD (DIMALTRND, 148);
      FIELD_B (DIMALT, 170);
      FIELD_BS (DIMALTD, 171);
      FIELD_B (DIMTOFL, 172);
      FIELD_B (DIMSAH, 173);
      FIELD_B (DIMTIX, 174);
      FIELD_B (DIMSOXD, 175);
      FIELD_CMC (DIMCLRD, 176,0);
      FIELD_CMC (DIMCLRE, 177,0);
      FIELD_CMC (DIMCLRT, 178,0);
      FIELD_BS (DIMADEC, 179);
      FIELD_BS (DIMDEC, 271);
      FIELD_BS (DIMTDEC, 272);
      FIELD_BS (DIMALTU, 273);
      FIELD_BS (DIMALTTD, 274);
      FIELD_BS (DIMAUNIT, 275);
      FIELD_BS (DIMFRAC, 276);
      FIELD_BS (DIMLUNIT, 277);
      FIELD_BS (DIMDSEP, 278);
      FIELD_BS (DIMTMOVE, 279);
      FIELD_BS (DIMJUST, 280);
      FIELD_B (DIMSD1, 281);
      FIELD_B (DIMSD2, 282);
      FIELD_BS (DIMTOLJ, 283);
      FIELD_BS (DIMTZIN, 284);
      FIELD_BS (DIMALTZ, 285);
      FIELD_BS (DIMALTTZ, 286);
      FIELD_B (DIMUPT, 288);
      FIELD_BS (DIMFIT, 289);
    }

  SINCE (R_2007)
    {
      FIELD_B (DIMFXLON, 290);
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

  SINCE (R_13)
  {
    FIELD_B (flag, 70); // Bit 0 of 70
    FIELD_VALUE (flag) = FIELD_VALUE (flag) |
                        FIELD_VALUE (xrefdep) << 4 |
                        FIELD_VALUE (xrefref) << 6;

    START_OBJECT_HANDLE_STREAM;
    FIELD_HANDLE (extref_handle, 5, 0);
    FIELD_HANDLE (DIMTXSTY, 5, 340); /* Text style (DIMTXSTY) */
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
      FIELD_HANDLE (DIMLTYPE, ANYCODE, 345);
      FIELD_HANDLE (DIMLTEX1, ANYCODE, 346);
      FIELD_HANDLE (DIMLTEX2, ANYCODE, 347);
    }

DWG_OBJECT_END

/* VIEWPORT ENTITY CONTROL (70)*/
DWG_OBJECT (VPORT_ENTITY_CONTROL)

  FIELD_BS (num_entries, 70);

  CONTROL_HANDLE_STREAM;
  HANDLE_VECTOR (entries, num_entries, 4, 0);

DWG_OBJECT_END

/* VIEWPORT ENTITY HEADER (71/11) */
DWG_OBJECT (VPORT_ENTITY_HEADER)

  COMMON_TABLE_FLAGS (ViewportEntity) //??

  SINCE (R_13) {
    DXF {
      ;
    } else {
      FIELD_B (flag1, 70); // bit 1 of 70
    }
    FIELD_VALUE (flag) =
      (FIELD_VALUE (flag1) << 1) |
      (FIELD_VALUE (xrefdep) << 4) |
      (FIELD_VALUE (xrefref) << 6);

    START_OBJECT_HANDLE_STREAM;
    FIELD_HANDLE (vport_entity, 5, 0);
  }

DWG_OBJECT_END

/*(72)*/
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
  FIELD_CMC (fill_color, 62,420); /*!< default 256 */
#ifdef IS_DXF
  // 0 - 90
  FIELD_VALUE (start_angle) = rad2deg (FIELD_VALUE (start_angle));
  FIELD_VALUE (end_angle)   = rad2deg (FIELD_VALUE (end_angle));
  while (FIELD_VALUE (start_angle) > 90.0) FIELD_VALUE (start_angle) -= 90.0;
  while (FIELD_VALUE (end_angle)   > 90.0) FIELD_VALUE (end_angle)   -= 90.0;
  VALUE (FIELD_VALUE (start_angle), RD, 51)
  VALUE (FIELD_VALUE (end_angle), RD, 52)
#else
  FIELD_BD (start_angle, 51); /*!< default 90 deg */
  FIELD_BD (end_angle, 52);   /*!< default 90 deg */
#endif
  FIELD_RCu (num_lines, 71);
  REPEAT (num_lines, lines, Dwg_MLINESTYLE_line)
  REPEAT_BLOCK
      SUB_FIELD_BD (lines[rcount1], offset, 49);
      SUB_FIELD_CMC (lines[rcount1], color, 62,420); /*!< default: 0 */
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
  END_REPEAT_BLOCK
  SET_PARENT_OBJ (lines)
  END_REPEAT (lines);

  START_OBJECT_HANDLE_STREAM;

DWG_OBJECT_END

//pg.135
DWG_OBJECT (DICTIONARYVAR)

  SUBCLASS (DictionaryVariables)
  FIELD_RC (intval, 280);
  FIELD_T (str, 1);
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
#ifndef IS_DECODER
  BITCODE_BL rcount1, rcount2;
#endif
  int error = 0;
  //Dwg_Data* dwg = obj->parent;

  FIELD_BL (is_gradient_fill, 450);
  FIELD_BL (reserved, 451);
  FIELD_BD (gradient_angle, 460);
  FIELD_BD (gradient_shift, 461);
  FIELD_BL (single_color_gradient, 452);
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
      SUB_FIELD_CMC (colors[rcount1], color, 63,421);
  END_REPEAT_BLOCK
  SET_PARENT_OBJ (colors)
  END_REPEAT (colors);
  FIELD_T (gradient_name, 470);
  return error;
}
#endif

#ifdef IS_JSON
#  define JSON_END_REPEAT(f)  ENDHASH
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
#ifdef IS_FREE
  if (dat->from_version >= R_2004)
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
  FIELD_3BD (extrusion, 210);
  FIELD_T (name, 2); //default: SOLID
  FIELD_B (solid_fill, 70); //default: 1, pattern_fill: 0
  FIELD_B (associative, 71);
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
              _obj->paths[rcount1].num_segs_or_paths = 0;
              JSON_END_REPEAT (paths);
              return DWG_ERR_VALUEOUTOFBOUNDS;
            }
#define segs paths[rcount1].segs
          REPEAT2 (paths[rcount1].num_segs_or_paths, segs, Dwg_HATCH_PathSeg)
          REPEAT_BLOCK
              SUB_FIELD_RC (segs[rcount2],type_status, 72);
              switch (FIELD_VALUE (segs[rcount2].type_status))
                {
                    case 1: /* LINE */
                      SUB_FIELD_2RD (segs[rcount2],first_endpoint, 10);
                      SUB_FIELD_2RD (segs[rcount2],second_endpoint, 11);
                      break;
                    case 2: /* CIRCULAR ARC */
                      SUB_FIELD_2RD (segs[rcount2], center, 10);
                      SUB_FIELD_BD (segs[rcount2], radius, 40);
                      SUB_FIELD_BD (segs[rcount2], start_angle, 50);
                      SUB_FIELD_BD (segs[rcount2], end_angle, 51);
                      SUB_FIELD_B (segs[rcount2], is_ccw, 73);
                      break;
                    case 3: /* ELLIPTICAL ARC */
                      SUB_FIELD_2RD (segs[rcount2], center, 10);
                      SUB_FIELD_2RD (segs[rcount2], endpoint, 11);
                      SUB_FIELD_BD (segs[rcount2], minor_major_ratio, 40);
                      SUB_FIELD_BD (segs[rcount2], start_angle, 50);
                      SUB_FIELD_BD (segs[rcount2], end_angle, 51);
                      SUB_FIELD_B (segs[rcount2], is_ccw, 73);
                      break;
                    case 4: /* SPLINE */
                      SUB_FIELD_BL (segs[rcount2], degree, 94);
                      SUB_FIELD_B (segs[rcount2], is_rational, 73);
                      SUB_FIELD_B (segs[rcount2], is_periodic, 74);
                      SUB_FIELD_BL (segs[rcount2], num_knots, 95);
                      SUB_FIELD_BL (segs[rcount2], num_control_points, 96);
                      if (FIELD_VALUE (segs[rcount2].num_knots > 10000))
                        {
                          LOG_ERROR ("Invalid HATCH.paths.segs.num_knots " FORMAT_BL,
                                    _obj->segs[rcount2].num_knots);
                          _obj->segs[rcount2].num_knots = 0;
                          JSON_END_REPEAT (segs);
                          JSON_END_REPEAT (paths);
                          return DWG_ERR_VALUEOUTOFBOUNDS;
                        }
                      FIELD_VECTOR (segs[rcount2].knots, BD,
                                    segs[rcount2].num_knots, 40);
                      if (FIELD_VALUE (segs[rcount2].num_control_points > 10000))
                        {
                          LOG_ERROR ("Invalid HATCH.paths.segs.num_control_points " FORMAT_BL,
                                    _obj->segs[rcount2].num_control_points);
                          _obj->segs[rcount2].num_control_points = 0;
                          JSON_END_REPEAT (segs);
                          JSON_END_REPEAT (paths);
                          return DWG_ERR_VALUEOUTOFBOUNDS;
                        }
#define control_points segs[rcount2].control_points
                      REPEAT3 (segs[rcount2].num_control_points, control_points, Dwg_HATCH_ControlPoint)
                      REPEAT_BLOCK
                          SUB_FIELD_2RD (control_points[rcount3], point, 10);
                          if (FIELD_VALUE (segs[rcount2].is_rational))
                            SUB_FIELD_BD (control_points[rcount3], weight, 40)
                      END_REPEAT_BLOCK
                      SET_PARENT (control_points,
                                 &_obj->segs[rcount2])
                      END_REPEAT (control_points);
#undef control_points
                      SINCE (R_2013) // r2014 really
                        {
#define seg segs[rcount2]
                          SUB_FIELD_BL (seg, num_fitpts, 97);
                          FIELD_2RD_VECTOR (seg.fitpts, seg.num_fitpts, 11);
#undef seg
                        }
                      break;
                    default:
                      LOG_ERROR ("Invalid HATCH.type_status %d\n",
                                FIELD_VALUE (segs[rcount2].type_status));
                      DEBUG_HERE_OBJ
                      _obj->segs[rcount2].type_status = 0;
                      JSON_END_REPEAT (segs);
                      JSON_END_REPEAT (paths);
                      return DWG_ERR_VALUEOUTOFBOUNDS;
                }
          END_REPEAT_BLOCK
          SET_PARENT (segs, &_obj->paths[rcount1])
          END_REPEAT (segs);
#undef segs
        }
      else
        { /* POLYLINE PATH */
          SUB_FIELD_B (paths[rcount1],bulges_present, 72);
          SUB_FIELD_B (paths[rcount1],closed, 73);
          SUB_FIELD_BL (paths[rcount1],num_segs_or_paths, 93);
#define polyline_paths paths[rcount1].polyline_paths
          REPEAT2 (paths[rcount1].num_segs_or_paths, polyline_paths, Dwg_HATCH_PolylinePath)
          REPEAT_BLOCK
              SUB_FIELD_2RD (polyline_paths[rcount2],point, 10);
              if (FIELD_VALUE (paths[rcount1].bulges_present))
                {
                  SUB_FIELD_BD (polyline_paths[rcount2],bulge, 42);
                }
          END_REPEAT_BLOCK
          SET_PARENT (polyline_paths, &_obj->paths[rcount1])
          END_REPEAT (polyline_paths);
#undef polyline_paths
        }
      SUB_FIELD_BL (paths[rcount1],num_boundary_handles, 97);
#if defined (IS_DXF) && !defined (IS_ENCODER)
      DXF {
        if (_obj->boundary_handles && rcount1 < _obj->num_boundary_handles) {
          FIELD_HANDLE (boundary_handles[rcount1], 0, 330)
        } else {
          LOG_WARN ("HATCH.num_path < num_boundary_handles or empty boundary_handles")
          VALUE_HANDLE ((BITCODE_H)NULL, boundary_handles, 0, 330)
        }
      }
#endif
      DECODER {
        FIELD_VALUE (num_boundary_handles) += FIELD_VALUE (paths[rcount1].num_boundary_handles);
        FIELD_VALUE (has_derived) =
          FIELD_VALUE (has_derived) || (FIELD_VALUE (paths[rcount1].flag) & 0x4);
      }
  END_REPEAT_BLOCK
  SET_PARENT_OBJ (paths)
  END_REPEAT (paths);
#ifdef IS_DXF
  SINCE (R_2004)
    {
      error |= DWG_FUNC_N (ACTION,_HATCH_gradientfill)(dat,str_dat,obj,_obj);
    }
#endif
  FIELD_BS (style, 75); // 0=normal (odd parity); 1=outer; 2=whole
  FIELD_BS (pattern_type, 76); // 0=user; 1=predefined; 2=custom
  if (!FIELD_VALUE (solid_fill))
    {
      FIELD_BD (angle, 52);
      FIELD_BD (scale_spacing, 41); //default 1.0
      FIELD_B (double_flag, 77);
      FIELD_BS (num_deflines, 78);
      REPEAT (num_deflines, deflines, Dwg_HATCH_DefLine)
      REPEAT_BLOCK
          SUB_FIELD_BD (deflines[rcount1], angle, 53);
          SUB_FIELD_2BD_1 (deflines[rcount1], pt0, 43);
          SUB_FIELD_2BD_1 (deflines[rcount1], offset, 45);
          SUB_FIELD_BS (deflines[rcount1], num_dashes, 79);
          FIELD_VECTOR (deflines[rcount1].dashes, BD, deflines[rcount1].num_dashes, 49)
      END_REPEAT_BLOCK
      SET_PARENT_OBJ (deflines)
      END_REPEAT (deflines);
    }

  if (FIELD_VALUE (has_derived))
    FIELD_BD (pixel_size, 47);
  FIELD_BL (num_seeds, 98);
  VALUEOUTOFBOUNDS (num_seeds, 10000)
  FIELD_2RD_VECTOR (seeds, num_seeds, 10);
  VALUEOUTOFBOUNDS (num_boundary_handles, 10000)

  COMMON_ENTITY_HANDLE_DATA;
  HANDLE_VECTOR (boundary_handles, num_boundary_handles, 4, 0); /* DXF: inlined above */

DWG_OBJECT_END

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

  //SUBCLASS (AcDbImage)
  SUBCLASS (AcDbRasterImage)
  FIELD_BL (class_version, 90);
  if (FIELD_VALUE (class_version) > 10)
    return DWG_ERR_VALUEOUTOFBOUNDS;
  FIELD_3DPOINT (pt0, 10);
  FIELD_3DPOINT (uvec, 11);
  FIELD_3DPOINT (vvec, 12);
  FIELD_2RD (size, 13);
  FIELD_BS (display_props, 70);
  FIELD_B (clipping, 280);
  FIELD_RC (brightness, 281);
  FIELD_RC (contrast, 282);
  FIELD_RC (fade, 283);
  SINCE (R_2010) {
    FIELD_B (clip_mode, 290); // 0 outside, 1 inside
  }
  FIELD_BS (clip_boundary_type, 71); // 1 rect, 2 polygon
  if (FIELD_VALUE (clip_boundary_type) == 1)
    FIELD_VALUE (num_clip_verts) = 2;
  else
    FIELD_BL (num_clip_verts, 91);
  FIELD_2RD_VECTOR (clip_verts, num_clip_verts, 14);

  COMMON_ENTITY_HANDLE_DATA;
  FIELD_HANDLE (imagedef, 5, 340); // hard pointer
  FIELD_HANDLE (imagedefreactor, 3, 360); // hard owner

DWG_ENTITY_END

//pg.142 test-data/*/Leader_*.dwg
DWG_OBJECT (IMAGEDEF)

  SUBCLASS (AcDbRasterImageDef)
  FIELD_BL (class_version, 90);
  if (FIELD_VALUE (class_version) > 10)
    return DWG_ERR_VALUEOUTOFBOUNDS;
  FIELD_2RD (image_size, 10);
  FIELD_T (file_path, 1);
  FIELD_B (is_loaded, 280);
  FIELD_RC (resunits, 281);
  FIELD_2RD (pixel_size, 11);

  START_OBJECT_HANDLE_STREAM;
  //DEBUG_POS_OBJ
  //FIELD_HANDLE (xrefctrl, 0, 0); ///libdxfrw bug?

DWG_OBJECT_END

//PG.143
DWG_OBJECT (IMAGEDEF_REACTOR)

  SUBCLASS (AcDbRasterImageDefReactor)
  FIELD_BL (class_version, 90);
  if (FIELD_VALUE (class_version) > 10)
    return DWG_ERR_VALUEOUTOFBOUNDS;

  START_OBJECT_HANDLE_STREAM;

DWG_OBJECT_END

//pg.144
DWG_OBJECT (LAYER_INDEX)

  SUBCLASS (AcDbLayerIndex)
  DXF {
    VALUE_BL (1.0 * FIELD_VALUE (timestamp1), 40);
    VALUE_BL (1.0 * FIELD_VALUE (timestamp2), 40);
  } else {
    FIELD_BL (timestamp1, 40);
    FIELD_BL (timestamp2, 40);
  }
  FIELD_BL (num_entries, 0);
  VALUEOUTOFBOUNDS (num_entries, 20000)
  // TODO: merge entries with H*
  REPEAT (num_entries, entries, Dwg_LAYER_entry)
  REPEAT_BLOCK
      SUB_FIELD_BL (entries[rcount1], idxlong, 0);
      SUB_FIELD_T (entries[rcount1], layername, 8);
  END_REPEAT_BLOCK
  SET_PARENT_OBJ (entries)
  END_REPEAT (entries)

  START_OBJECT_HANDLE_STREAM;
  HANDLE_VECTOR (layer_entries, num_entries, 5, 0);

DWG_OBJECT_END

//pg.145
DWG_OBJECT (LAYOUT)

  SUBCLASS (AcDbPlotSettings)
  FIELD_T (page_setup_name, 1);
  FIELD_T (printer_or_config, 2);
  FIELD_BS (plot_layout_flags, 70);
  FIELD_BD (left_margin, 40);
  FIELD_BD (bottom_margin, 41);
  FIELD_BD (right_margin, 42);
  FIELD_BD (top_margin, 43);
  FIELD_BD (paper_width, 44);
  FIELD_BD (paper_height, 45);
  FIELD_T (paper_size, 4);
  FIELD_2BD_1 (plot_origin, 46);
  FIELD_BS (plot_paper_unit, 72);
  FIELD_BS (plot_rotation, 73);
  FIELD_BS (plot_type, 74);
  FIELD_2BD_1 (plot_window_ll, 48);
  FIELD_2BD_1 (plot_window_ur, 140);

  VERSIONS (R_13, R_2000) {
    FIELD_TV (plot_view_name, 6);
  }

  FIELD_BD (paper_units, 142);
  FIELD_BD (drawing_units, 143);
  FIELD_T (current_style_sheet, 7);
  FIELD_BS (scale_type, 75);
  FIELD_BD (scale_factor, 147);
  FIELD_2BD_1 (paper_image_origin, 148);

  SINCE (R_2004)
    {
      FIELD_BS (shade_plot_mode, 76);
      FIELD_BS (shade_plot_reslevel, 77);
      FIELD_BS (shade_plot_customdpi, 78);
    }

  SUBCLASS (AcDbLayout)
  FIELD_T (layout_name, 1);
  FIELD_BS (tab_order, 71);
  FIELD_BS (flag, 70);
  FIELD_3DPOINT (ucs_origin, 13);
  FIELD_2RD (minimum_limits, 10);
  FIELD_2RD (maximum_limits, 11);
  FIELD_3DPOINT (ins_point, 12);
  FIELD_3DPOINT (ucs_x_axis, 16);
  FIELD_3DPOINT (ucs_y_axis, 17);
  FIELD_BD (elevation, 146);
  FIELD_BS (orthoview_type, 76);
  FIELD_3DPOINT (extent_min, 14);
  FIELD_3DPOINT (extent_max, 15);

  SINCE (R_2004) {
    FIELD_BL (num_viewports, 0);
    VALUEOUTOFBOUNDS (num_viewports, 10000)
  }

  START_OBJECT_HANDLE_STREAM;
  IF_FREE_OR_SINCE (R_2004) {
    FIELD_HANDLE (plotview, 5, 6);
  }
  IF_FREE_OR_SINCE (R_2007) {
    FIELD_HANDLE (visualstyle, 4, 0);
  }
  FIELD_HANDLE (block_header, 4, 330); // => BLOCK_HEADER.pspace or mspace
  FIELD_HANDLE (active_viewport, 4, 331);
  FIELD_HANDLE (base_ucs, 5, 346);
  FIELD_HANDLE (named_ucs, 5, 345);

  IF_FREE_OR_SINCE (R_2004) {
    HANDLE_VECTOR (viewports, num_viewports, 4, 0);
  }

DWG_OBJECT_END

//20.4.85 p.211
DWG_ENTITY (LWPOLYLINE)

  SUBCLASS (AcDbPolyline)
#ifdef IS_DXF
  FIELD_BL (num_points, 90);
  VALUE_BS ((FIELD_VALUE (flag) & 128) + (FIELD_VALUE (flag) & 512 ? 1 : 0),
            70); //1 closed, 128 plinegen
#else
  FIELD_BS (flag, 70); // 512 closed, 128 plinegen, 4 constwidth, 8 elevation, 2 thickness
                       // 1 extrusion, 16 num_bulges, 1024 vertexidcount, 32 numwidths
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
    VERSIONS (R_13, R_14) {
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
DWG_ENTITY (PROXY_LWPOLYLINE)

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

  VERSIONS (R_13, R_14) {
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
DWG_ENTITY (PROXY_ENTITY)

  DECODE_UNKNOWN_BITS
  FIELD_BL (class_id, 91);
  PRE (R_2018)
  {
    FIELD_BL (version, 95); // i.e. version << 8 + maint_version
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

  LOG_INFO ("TODO PROXY_ENTITY data\n");
  /*
  //TODO: figure out how to deal with the arbitrary size vector databits
  //      described on the spec
  FIELD_RC (*data);
  */

  COMMON_ENTITY_HANDLE_DATA;
  //FIELD_MS (size, 0);

DWG_ENTITY_END

//(499) pg.149 r2000+
DWG_OBJECT (PROXY_OBJECT)

  DECODE_UNKNOWN_BITS
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

  LOG_INFO ("TODO PROXY_OBJECT data\n");
  /*
  //TODO: save at least the remaining bytes
  //TODO: figure out how to deal with the arbitrary size vector databits
  FIELD_RC (*data);
  */

  START_OBJECT_HANDLE_STREAM;

DWG_OBJECT_END

DWG_OBJECT (OBJECTCONTEXTDATA)

  SUBCLASS (AcDbObjectContextData)
  SINCE (R_2010) {
    IF_ENCODE_FROM_EARLIER {
      FIELD_VALUE (class_version) = 3;
    }
    FIELD_BS (class_version, 70);
    if (FIELD_VALUE (class_version) > 10)
      return DWG_ERR_VALUEOUTOFBOUNDS;
  }
  FIELD_B (has_file, 290);
  FIELD_B (defaultflag, 0);

  START_OBJECT_HANDLE_STREAM;

DWG_OBJECT_END

// 20.4.99 Value, page 241. for FIELD and TABLE
#define TABLE_value_fields(value)                                             \
  SINCE (R_2007) { FIELD_BL (value.flags, 93); }                              \
  if ((FIELD_VALUE (value.flags) & 0x01) == 0x00)                             \
    {                                                                         \
      FIELD_BL (value.data_type, 90);                                         \
    }                                                                         \
  else                                                                        \
    {                                                                         \
      FIELD_VALUE (value.data_type) = 512; /* kGeneral since r2007*/          \
    }                                                                         \
  switch (FIELD_VALUE (value.data_type))                                      \
    {                                                                         \
    case 0: /* kUnknown */                                                    \
      FIELD_BL (value.data_long, 0);                                          \
      break;                                                                  \
    case 1: /* kLong */                                                       \
      FIELD_BL (value.data_long, 0);                                          \
      break;                                                                  \
    case 2: /* kDouble */                                                     \
      FIELD_BD (value.data_double, 0);                                        \
      break;                                                                  \
    case 4: /* kString */                                                     \
      FIELD_T (value.data_string, 0);                                         \
      break;                                                                  \
    case 8: /* kDate */                                                       \
      FIELD_BL (value.data_size, 0);                                          \
      FIELD_BINARY (value.data_date, FIELD_VALUE (value.data_size), 0);       \
      break;                                                                  \
    case 16: /* kPoint */                                                     \
      FIELD_BL (value.data_size, 0);                                          \
      FIELD_2RD (value.data_point, 0);                                        \
      break;                                                                  \
    case 32: /* k3dPoint */                                                   \
      FIELD_BL (value.data_size, 0);                                          \
      FIELD_3RD (value.data_3dpoint, 0);                                      \
      break;                                                                  \
    case 64: /* kObjectId */                                                  \
      /* data is a HANDLE */                                                  \
      /* read from appropriate place in handles section */                    \
      break;                                                                  \
    case 128: /* kBuffer */                                                   \
      LOG_ERROR ("Unknown data type in TABLE entity: \"kBuffer\".\n")         \
      break;                                                                  \
    case 256: /* kResBuf */                                                   \
      LOG_ERROR ("Unknown data type in TABLE entity: \"kResBuf\".\n")         \
      break;                                                                  \
    case 512: /* kGeneral since r2007*/                                       \
      SINCE (R_2007) { FIELD_BL (value.data_size, 0); }                       \
      else                                                                    \
      {                                                                       \
        LOG_ERROR ("Unknown data type in TABLE entity: \"kGeneral before "    \
                   "R_2007\".\n")                                             \
      }                                                                       \
      break;                                                                  \
    default:                                                                  \
      LOG_ERROR ("Invalid data type in TABLE entity\n")                       \
      DEBUG_HERE_OBJ                                                          \
      error |= DWG_ERR_INVALIDTYPE;                                           \
      break;                                                                  \
      /*return DWG_ERR_INVALIDTYPE; */                                        \
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
  END_REPEAT_BLOCK
  SET_PARENT_OBJ (childval)
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

  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbGeoData)
  FIELD_BL (class_version, 90); //1 for r2009, 2 for r2010 (default), 3 for r2013 (same as r2010)
  if (FIELD_VALUE (class_version) > 10)
    return DWG_ERR_VALUEOUTOFBOUNDS;
  FIELD_HANDLE (host_block, 4, 330);
  FIELD_BS (coord_type, 70); // 0 unknown, 1 local grid, 2 projected grid,
                             // 3 geographic (defined by latitude/longitude) (default)
  if (FIELD_VALUE (class_version) > 1) // or SINCE(R_2010)
    {
      FIELD_3BD (design_pt, 10);
      FIELD_3BD (ref_pt, 11);
      FIELD_BD (unit_scale_horiz, 40);
      FIELD_BL (units_value_horiz, 91);
      FIELD_BD (unit_scale_vert, 41);
      FIELD_BL (units_value_vert, 92);
      FIELD_3BD (up_dir, 210);
      FIELD_2RD (north_dir, 12);
      FIELD_BL (scale_est, 95); // None = 1 (default: ScaleEstMethodUnity),
                                // User defined = 2, Grid scale at reference point = 3,
                                // Prismodial = 4
      FIELD_BD (user_scale_factor, 141);
      FIELD_B (sea_level_corr, 294);
      FIELD_BD (sea_level_elev, 142);
      FIELD_BD (coord_proj_radius, 143);
      FIELD_T (coord_system_def, 0);
      FIELD_T (geo_rss_tag, 302);
    }
  else
    {
      FIELD_3BD (ref_pt, 11);
      FIELD_BL (units_value_horiz, 91);
      FIELD_3BD (design_pt, 10);
      FIELD_3BD (obs_pt, 0); // 0,0,0
      FIELD_3BD (up_dir, 210);
      FIELD_BD (north_dir_angle_deg, 0);
      FIELD_3BD (north_dir, 12); // obsolete: 1,1,1
      FIELD_T (coord_system_def, 0);
      FIELD_T (geo_rss_tag, 302);
      FIELD_BD (unit_scale_horiz, 40);
      FIELD_T (coord_system_datum, 0); //obsolete
      FIELD_T (coord_system_wkt, 0); //obsolete
    }
  FIELD_T (observation_from_tag, 305);
  FIELD_T (observation_to_tag, 306);
  FIELD_T (observation_coverage_tag, 0);
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
      //SUB_FIELD_BL (geomesh_faces[rcount1],face4, 0);
  END_REPEAT_BLOCK
  END_REPEAT (geomesh_faces);

  UNTIL (R_2007) // r2009, class_version 1 really
    {
      FIELD_B (has_civil_data, 0);
      FIELD_B (obsolete_false, 0);
      FIELD_RD (refpt0y, 0);
      FIELD_RD (refpt0x, 0);
      FIELD_RD (refpt1y, 0);
      FIELD_RD (refpt1x, 0);
      FIELD_BL (unknown1, 0);
      FIELD_BL (unknown2, 0);
      FIELD_2RD (zero0, 0);
      FIELD_2RD (zero1, 0);
      FIELD_B (unknown_b, 0);
      FIELD_BD (north_dir_angle_deg, 0);
      FIELD_BD (north_dir_angle_rad, 0);
      FIELD_BL (scale_est, 0);
      FIELD_BD (user_scale_factor, 0);
      FIELD_B (sea_level_corr, 0);
      FIELD_BD (sea_level_elev, 0);
      FIELD_BD (coord_proj_radius, 143);
    }
  START_OBJECT_HANDLE_STREAM;

DWG_OBJECT_END

//pg.220, 20.4.91
DWG_OBJECT (RASTERVARIABLES)

  SUBCLASS (AcDbRasterVariables)
  FIELD_BL (class_version, 90);
  if (FIELD_VALUE (class_version) > 10)
    return DWG_ERR_VALUEOUTOFBOUNDS;
  FIELD_BS (display_frame, 70);
  FIELD_BS (display_quality, 71);
  FIELD_BS (units, 72);

  START_OBJECT_HANDLE_STREAM;

DWG_OBJECT_END

// 20.4.93 page 221
DWG_OBJECT (SORTENTSTABLE)

  SUBCLASS (AcDbSortentsTable)
  FIELD_BL (num_ents, 0);
  VALUEOUTOFBOUNDS (num_ents, 50000)
  HANDLE_VECTOR (sort_ents, num_ents, 0, 5);

  START_OBJECT_HANDLE_STREAM;
  FIELD_HANDLE (dictionary, 4, 0);
  HANDLE_VECTOR_N (ents, FIELD_VALUE (num_ents), 4, 331);

DWG_OBJECT_END

//pg.222, 20.4.94 to clip external references
DWG_OBJECT (SPATIAL_FILTER)

  SUBCLASS (AcDbSpatialFilter)
  FIELD_BS (num_points, 70);
  VALUEOUTOFBOUNDS (num_points, 10000)
  FIELD_2RD_VECTOR (points, num_points, 10);
  FIELD_3BD (extrusion, 210);
  FIELD_3BD (clip_bound_origin, 10);
  FIELD_BS (display_boundary, 71);
  FIELD_BS (front_clip_on, 72);
  if (FIELD_VALUE (front_clip_on) == 1)
    FIELD_BD (front_clip_dist, 40);

  FIELD_BS (back_clip_on, 73);
  if (FIELD_VALUE (back_clip_on) == 1)
    FIELD_BD (back_clip_dist, 41);

  FIELD_VECTOR_N (inverse_block_transform, BD, 12, 40);
  FIELD_VECTOR_N (clip_bound_transform, BD, 12, 40);

  START_OBJECT_HANDLE_STREAM;

DWG_OBJECT_END

//pg.153
DWG_OBJECT (SPATIAL_INDEX)

  SUBCLASS (AcDbSpatialIndex)
  FIELD_BL (timestamp1, 0);
  FIELD_BL (timestamp2, 0);

  START_OBJECT_HANDLE_STREAM;

DWG_OBJECT_END

#if defined (DEBUG_CLASSES) || defined (IS_FREE)

// 20.4.101.3 Content format for TABLECONTENT and Cell_Style_Field
#define ContentFormat_fields(fmt) \
      FIELD_BL (fmt.property_override_flags, 90); \
      FIELD_BL (fmt.property_flags, 91); \
      FIELD_BL (fmt.value_data_type, 92); \
      FIELD_BL (fmt.value_unit_type, 93); \
      FIELD_T (fmt.value_format_string, 300); \
      FIELD_BD (fmt.rotation, 40); \
      FIELD_BD (fmt.block_scale, 140); \
      FIELD_BL (fmt.cell_alignment, 94); \
      FIELD_CMC (fmt.content_color, 62,420);   \
      FIELD_HANDLE (fmt.text_style, 3, 92); \
      FIELD_BD (fmt.text_height, 92)

// Cell style 20.4.101.4 for TABLE, TABLECONTENT and CELLSTYLEMAP
#define CellStyle_fields(sty) \
      FIELD_BL (sty.type, 90); \
      FIELD_BS (sty.data_flags, 170); \
      FIELD_BL (sty.property_override_flags, 91); \
      FIELD_BL (sty.merge_flags, 92); \
      FIELD_CMC (sty.background_color, 62,420);    \
      FIELD_BL (sty.content_layout, 93); \
      ContentFormat_fields (sty.content_format); \
      FIELD_BS (sty.margin_override_flags, 171); \
      FIELD_BD (sty.vert_margin, 40); \
      FIELD_BD (sty.horiz_margin, 40); \
      FIELD_BD (sty.bottom_margin, 40); \
      FIELD_BD (sty.right_margin, 40); \
      FIELD_BD (sty.margin_horiz_spacing, 40); \
      FIELD_BD (sty.margin_vert_spacing, 40); \
      FIELD_BL (sty.num_borders, 94); /* 0-6 */ \
      VALUEOUTOFBOUNDS (sty.num_borders, 10000) \
      REPEAT2 (sty.num_borders, sty.border, Dwg_BorderStyle) \
      REPEAT_BLOCK \
          SUB_FIELD_BL (sty.border[rcount2],edge_flags, 95);                \
          SUB_FIELD_BL (sty.border[rcount2],border_property_overrides_flag, 90); \
          SUB_FIELD_BL (sty.border[rcount2],border_type, 91);               \
          SUB_FIELD_CMC (sty.border[rcount2],color, 62,420);                \
          SUB_FIELD_BLd (sty.border[rcount2],linewt, 92);                   \
          SUB_FIELD_HANDLE (sty.border[rcount2],line_type, 3, 340);         \
          SUB_FIELD_BL (sty.border[rcount2],invisible, 93);                 \
          SUB_FIELD_BD (sty.border[rcount2],double_line_spacing, 93);       \
      END_REPEAT_BLOCK \
      END_REPEAT (sty.border)

// clang-format off
#define row tdata.rows[rcount1]
#define cell row.cells[rcount2]
#define content cell.cell_contents[rcount3]
#define geom cell.geometry[0]
#define attr content.attrs[rcount4]
#define merged fdata.merged_cells[rcount1]

// pg.237 20.4.97 for TABLE (2010+) and TABLECONTENT
#define TABLECONTENT_fields             \
  SUBCLASS (AcDbDataTableContent)	\
  FIELD_T (ldata.name, 1);		\
  FIELD_T (ldata.description, 300);	\
  FIELD_BL (tdata.num_cols, 90);	\
  REPEAT (tdata.num_cols, tdata.cols, Dwg_TableDataColumn)	\
  REPEAT_BLOCK				\
      SUB_FIELD_T (tdata.cols[rcount1],name, 300);		\
      SUB_FIELD_BL (tdata.cols[rcount1],custom_data, 91);	\
      CellStyle_fields (tdata.cols[rcount1].cellstyle);		\
  END_REPEAT_BLOCK			\
  SET_PARENT (tdata.cols, &_obj->tdata)	\
  END_REPEAT (tdata.cols);		\
  FIELD_BL (tdata.num_rows, 90);	\
  REPEAT (tdata.num_rows, tdata.rows, Dwg_TableRow)	\
  REPEAT_BLOCK				\
      FIELD_BL (row.num_cells, 90);	\
      REPEAT2 (row.num_cells, row.cells, Dwg_TableCell)	\
      REPEAT_BLOCK			\
          SUB_FIELD_BL (cell,flag, 90);		\
          SUB_FIELD_T (cell,tooltip, 300);	\
          SUB_FIELD_BL (cell,customdata, 91);	\
          SUB_FIELD_BL (cell,num_customdata_items, 90);	\
          REPEAT3 (cell.num_customdata_items, cell.customdata_items, Dwg_TABLE_CustomDataItem) \
          REPEAT_BLOCK				\
              SUB_FIELD_T (cell.customdata_items[rcount3],name, 300);	 \
              TABLE_value_fields (cell.customdata_items[rcount3].value); \
              if (error & DWG_ERR_INVALIDTYPE)	\
                {			\
                  JSON_END_REPEAT (cell.customdata_items);      \
                  JSON_END_REPEAT (row.cells);                  \
                  JSON_END_REPEAT (tdata.rows);                 \
                  return error;		\
                }			\
          END_REPEAT_BLOCK		\
          SET_PARENT_FIELD (cell.customdata_items, cell_parent, &_obj->cell)	\
          END_REPEAT (cell.customdata_items);		\
          SUB_FIELD_BL (cell,has_linked_data, 92);	\
          if (FIELD_VALUE (cell.has_linked_data))	\
            {						\
              SUB_FIELD_HANDLE (cell,data_link, 5, 340);  \
              SUB_FIELD_BL (cell,num_rows, 93);		\
              SUB_FIELD_BL (cell,num_cols, 94);		\
              SUB_FIELD_BL (cell,unknown, 96);		\
            }						\
          SUB_FIELD_BL (cell,num_cell_contents, 95);	\
          REPEAT3 (cell.num_cell_contents, cell.cell_contents, Dwg_TableCellContent) \
          REPEAT_BLOCK					\
              SUB_FIELD_BL (content,type, 90);		\
              if (FIELD_VALUE (content.type) == 1)	\
                {					\
                  /* 20.4.99 Value, page 241 */         \
                  TABLE_value_fields (content.value)	\
                  if (error & DWG_ERR_INVALIDTYPE)	\
                    {					\
                      JSON_END_REPEAT (cell.cell_contents);	\
                      JSON_END_REPEAT (row.cells);		\
                      JSON_END_REPEAT (tdata.rows);		\
                      return error;			\
                    }					\
                }					\
              else if (FIELD_VALUE (content.type) == 2) { /* Field */	\
                SUB_FIELD_HANDLE (content,handle, 3, 340);	\
              }						\
              else if (FIELD_VALUE (content.type) == 4) { /* Block */	\
                SUB_FIELD_HANDLE (content,handle, 3, 340);	\
              }						\
              SUB_FIELD_BL (content,num_attrs, 91);	\
              REPEAT4 (content.num_attrs, content.attrs, Dwg_TableCellContent_Attr)	\
              REPEAT_BLOCK				\
                  SUB_FIELD_HANDLE (attr,attdef, 5, 330); \
                  SUB_FIELD_T (attr,value, 301);	\
                  SUB_FIELD_BL (attr,index, 92);	\
              END_REPEAT_BLOCK				\
              SET_PARENT (content.attrs, &_obj->content)	\
              END_REPEAT (content.attrs);		\
              if (FIELD_VALUE (content.has_content_format_overrides))	\
                {					\
                  ContentFormat_fields (content.content_format);	\
                }					\
          END_REPEAT_BLOCK				\
          SET_PARENT (cell.cell_contents, &_obj->cell)	\
          END_REPEAT (cell.cell_contents);		\
          SUB_FIELD_BL (cell, style_id, 90);		\
          SUB_FIELD_BL (cell, has_geom_data, 91);	\
          if (FIELD_VALUE (cell.has_geom_data))		\
            {						\
              SUB_FIELD_BL (cell,geom_data_flag, 91);	\
              SUB_FIELD_BD (cell,width_w_gap, 40);	\
              SUB_FIELD_BD (cell,height_w_gap, 41);	\
              SUB_FIELD_BL (cell,num_geometry, 94);	\
              SUB_FIELD_HANDLE (cell,tablegeometry, 4, 330); \
              REPEAT (cell.num_geometry, cell.geometry, Dwg_CellContentGeometry) \
              REPEAT_BLOCK \
                  SUB_FIELD_3BD (geom,dist_top_left, 10); \
                  SUB_FIELD_3BD (geom,dist_center, 11);	\
                  SUB_FIELD_BD (geom,content_width, 43); \
                  SUB_FIELD_BD (geom,content_height, 44); \
                  SUB_FIELD_BD (geom,width, 45); \
                  SUB_FIELD_BD (geom,height, 46); \
                  SUB_FIELD_BL (geom,unknown, 95); \
              END_REPEAT_BLOCK	\
              SET_PARENT_FIELD (cell.geometry, cell_parent, &_obj->cell) \
              END_REPEAT (cell.geometry); \
            }						\
      END_REPEAT_BLOCK					\
      SET_PARENT_FIELD (row.cells, row_parent, &_obj->row)	\
      END_REPEAT (row.cells);				\
      SUB_FIELD_BL (row,custom_data, 91);		\
      SUB_FIELD_BL (row,num_customdata_items, 90);	\
      REPEAT3 (row.num_customdata_items, row.customdata_items, Dwg_TABLE_CustomDataItem) \
      REPEAT_BLOCK					\
          SUB_FIELD_T (row.customdata_items[rcount3],name, 300);	\
          TABLE_value_fields (row.customdata_items[rcount3].value);	\
          if (error & DWG_ERR_INVALIDTYPE)		\
            {						\
              JSON_END_REPEAT (row.customdata_items);		\
              JSON_END_REPEAT (tdata.rows);			\
              return error;				\
            }						\
      END_REPEAT_BLOCK					\
      SET_PARENT_FIELD (row.customdata_items, row_parent, &_obj->row)	\
      END_REPEAT (row.customdata_items);			\
      {							\
        CellStyle_fields (row.cellstyle);		\
        SUB_FIELD_BL (row,style_id, 90);		\
        SUB_FIELD_BL (row,height, 40);			\
      }							\
  END_REPEAT_BLOCK					\
  SET_PARENT (tdata.rows, &_obj->tdata)			\
  END_REPEAT (tdata.rows);				\
  FIELD_BL (tdata.num_field_refs, 0);			\
  HANDLE_VECTOR (tdata.field_refs, tdata.num_field_refs, 3, 0);	\
  FIELD_BL (fdata.num_merged_cells, 90);		\
  REPEAT (fdata.num_merged_cells, fdata.merged_cells, Dwg_FormattedTableMerged)	\
  REPEAT_BLOCK						\
      SUB_FIELD_BL (merged,top_row, 91);		\
      SUB_FIELD_BL (merged,left_col, 92);		\
      SUB_FIELD_BL (merged,bottom_row, 93);		\
      SUB_FIELD_BL (merged,right_col, 94);		\
  END_REPEAT_BLOCK					\
  SET_PARENT (fdata.merged_cells, &_obj->fdata)		\
  END_REPEAT (fdata.merged_cells)

// clang-format on

DWG_OBJECT (TABLECONTENT)
  DECODE_UNKNOWN_BITS
  TABLECONTENT_fields;

  START_OBJECT_HANDLE_STREAM;
  FIELD_HANDLE (table_style, 3, 340);
DWG_OBJECT_END

// pg.246 20.4.102 and TABLE
// added with r2008, backcompat with r2007
// The cellstyle map can contain custom cell styles, whereas the TABLESTYLE
// only contains the Table (R24), _Title, _Header and _Data cell style.
DWG_OBJECT (CELLSTYLEMAP)

  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbCellStyleMap)
  FIELD_BL (num_cells, 90);
  VALUEOUTOFBOUNDS (num_cells, 5000)
  DECODER {
    if (obj->size < 20)
      {
        LOG_ERROR ("CELLSTYLEMAP too small for 3 cells")
        return DWG_ERR_VALUEOUTOFBOUNDS;
      }
  }
  REPEAT (num_cells, cells, Dwg_CELLSTYLEMAP_Cell)
  REPEAT_BLOCK
      CellStyle_fields (cells[rcount1].style);
      SUB_FIELD_BL (cells[rcount1],id, 90);
      SUB_FIELD_BL (cells[rcount1],type, 91);
      SUB_FIELD_T (cells[rcount1],name, 300);
  END_REPEAT_BLOCK
  SET_PARENT_OBJ (cells)
  END_REPEAT (cells);

DWG_OBJECT_END

// pg.229 20.4.96, as ACAD_TABLE (varies)
// works ok for the pre-2010 variant, deriving from INSERT
// r2010+ it is TABLECONTENT
DWG_ENTITY (TABLE)

  DECODE_UNKNOWN_BITS
  SINCE (R_2010) //AC1024
    {
      FIELD_RC (unknown_rc, 0);
      FIELD_HANDLE (unknown_h, 5, 0);
      FIELD_BL (unknown_bl, 0);
      VERSION (R_2010)
        FIELD_B (unknown_b, 0); // default 1
      VERSION (R_2013)
        FIELD_BL (unknown_bl1, 0);
      // i.e. TABLECONTENT: 20.4.96.2 AcDbTableContent subclass: 20.4.97
      TABLECONTENT_fields;
    }
  else {
    SUBCLASS (AcDbBlockReference)
    FIELD_3BD (insertion_point, 10);
    VERSIONS (R_13, R_14) {
      FIELD_3BD_1 (scale, 41);
    }
    SINCE (R_2000)
      {
        FIELD_BB (data_flags, 0);
        switch (FIELD_VALUE (data_flags))
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
              LOG_ERROR ("Invalid data_flags in TABLE entity %d\n",
                        (int)FIELD_VALUE (data_flags))
              _obj->data_flags = 0;
              DEBUG_HERE_OBJ
              return DWG_ERR_INVALIDTYPE;
              //break;
          }
  #ifndef IS_FREE
        FIELD_3PT_TRACE (scale, DD, 41);
  #endif
      }
  
    FIELD_BD (rotation, 50);
    FIELD_3BD (extrusion, 210);
    FIELD_B (has_attribs, 66);
  
    SINCE (R_2004) {
      FIELD_BL (num_owned, 0);
      VALUEOUTOFBOUNDS (num_owned, 10000)
    }
  
    SUBCLASS (AcDbTable)
    FIELD_BS (flag_for_table_value, 90);
    FIELD_3BD (horiz_direction, 11);
    FIELD_BL (num_cols, 92);
    VALUEOUTOFBOUNDS (num_cols, 5000)
    FIELD_BL (num_rows, 91);
    VALUEOUTOFBOUNDS (num_rows, 5000)
    FIELD_VECTOR (col_widths, BD, num_cols, 142);
    FIELD_VECTOR (row_heights, BD, num_rows, 141);
    FIELD_VALUE (num_cells) = FIELD_VALUE (num_rows) * FIELD_VALUE (num_cols);
    REPEAT (num_cells, cells, Dwg_TABLE_Cell)
    REPEAT_BLOCK
        //SUBCLASS (AcDbDataCell)
        SUB_FIELD_BS (cells[rcount1],type, 171);
        SUB_FIELD_RC (cells[rcount1],flags, 172);
        SUB_FIELD_B (cells[rcount1],merged_value, 173);
        SUB_FIELD_B (cells[rcount1],autofit_flag, 174);
        SUB_FIELD_BL (cells[rcount1],merged_width_flag, 175);
        SUB_FIELD_BL (cells[rcount1],merged_height_flag, 176);
        SUB_FIELD_BD (cells[rcount1],rotation_value, 145);
  
        if (FIELD_VALUE (cells[rcount1].type) == 1)
          { /* text cell */
            SUB_FIELD_T (cells[rcount1],text_string, 1);
          }
        if (FIELD_VALUE (cells[rcount1].type) == 2)
          { /* block cell */
            SUB_FIELD_BD (cells[rcount1],block_scale, 144);
            SUB_FIELD_B (cells[rcount1],additional_data_flag, 0);
            if (FIELD_VALUE (cells[rcount1].additional_data_flag) == 1)
              {
                SUB_FIELD_BS (cells[rcount1],num_attr_defs, 179);
                SUB_FIELD_BS (cells[rcount1],attr_def_index, 0);
                SUB_FIELD_T (cells[rcount1],attr_def_text, 300);
                //total_num_attr_defs += FIELD_VALUE (cells[rcount1].num_attr_defs);
              }
          }
        if (FIELD_VALUE (cells) &&
            (FIELD_VALUE (cells[rcount1].type) == 1 ||
             FIELD_VALUE (cells[rcount1].type) == 2))
          { /* common to both text and block cells */
            SUB_FIELD_B (cells[rcount1],additional_data_flag, 0);
            if (FIELD_VALUE (cells[rcount1].additional_data_flag) == 1)
              {
                BITCODE_BL cell_flag;
                SUB_FIELD_BL (cells[rcount1],cell_flag_override, 177);
                cell_flag = FIELD_VALUE (cells[rcount1].cell_flag_override);
                SUB_FIELD_RC (cells[rcount1],virtual_edge_flag, 178);
  
                if (cell_flag & 0x01)
                  SUB_FIELD_RS (cells[rcount1],cell_alignment, 170);
                if (cell_flag & 0x02)
                  SUB_FIELD_B (cells[rcount1],background_fill_none, 283);
                if (cell_flag & 0x04)
                  SUB_FIELD_CMC (cells[rcount1],background_color, 63,421);
                if (cell_flag & 0x08)
                  SUB_FIELD_CMC (cells[rcount1],content_color, 64,422);
                if (cell_flag & 0x10) {
                  SUB_FIELD_HANDLE (cells[rcount1],text_style, 5, 7);
                }
                if (cell_flag & 0x20)
                  SUB_FIELD_BD (cells[rcount1],text_height, 140);
                if (cell_flag & 0x00040)
                  SUB_FIELD_CMC (cells[rcount1],top_grid_color, 69,0);
                if (cell_flag & 0x00400)
                  SUB_FIELD_BS (cells[rcount1],top_grid_linewt, 279);
                if (cell_flag & 0x04000)
                  SUB_FIELD_BS (cells[rcount1],top_visibility, 289);
                if (cell_flag & 0x00080)
                  SUB_FIELD_CMC (cells[rcount1],right_grid_color, 65,423);
                if (cell_flag & 0x00800)
                  SUB_FIELD_BS (cells[rcount1],right_grid_linewt, 275);
                if (cell_flag & 0x08000)
                  SUB_FIELD_BS (cells[rcount1],right_visibility, 285);
                if (cell_flag & 0x00100)
                  SUB_FIELD_CMC (cells[rcount1],bottom_grid_color, 66,0);
                if (cell_flag & 0x01000)
                  SUB_FIELD_BS (cells[rcount1],bottom_grid_linewt, 276);
                if (cell_flag & 0x10000)
                  SUB_FIELD_BS (cells[rcount1],bottom_visibility, 286);
                if (cell_flag & 0x00200)
                  SUB_FIELD_CMC (cells[rcount1],left_grid_color, 68,0);
                if (cell_flag & 0x02000)
                  SUB_FIELD_BS (cells[rcount1],left_grid_linewt, 278);
                if (cell_flag & 0x20000)
                  SUB_FIELD_BS (cells[rcount1],left_visibility, 288);
  
                SUB_FIELD_BL (cells[rcount1],unknown, 0);
  
                // 20.4.99 Value, page 241
                TABLE_value_fields (cells[rcount1].value)
                if (error & DWG_ERR_INVALIDTYPE)
                  {
                    JSON_END_REPEAT (cells);
                    return error;
                  }
              }
          }
    END_REPEAT_BLOCK
    SET_PARENT_OBJ (cells)
#ifndef IS_FREE
    END_REPEAT (cells);
#endif
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
          FIELD_CMC (title_row_color, 64,422);
        if (table_flag & 0x0040)
          FIELD_CMC (header_row_color, 64,422);
        if (table_flag & 0x0080)
          FIELD_CMC (data_row_color, 64,422);
        if (table_flag & 0x0100)
          FIELD_B (title_row_fill_none, 283);
        if (table_flag & 0x0200)
          FIELD_B (header_row_fill_none, 283);
        if (table_flag & 0x0400)
          FIELD_B (data_row_fill_none, 283);
        if (table_flag & 0x0800)
          FIELD_CMC (title_row_fill_color, 63,421);
        if (table_flag & 0x1000)
          FIELD_CMC (header_row_fill_color, 63,421);
        if (table_flag & 0x2000)
          FIELD_CMC (data_row_fill_color, 63,421);
        if (table_flag & 0x4000)
          FIELD_BS (title_row_alignment, 170);
        if (table_flag & 0x8000)
          FIELD_BS (header_row_alignment, 170);
        if (table_flag & 0x10000)
          FIELD_BS (data_row_alignment, 170);
        if (table_flag & 0x20000)
          FIELD_HANDLE (title_text_style, 5, 7);
        if (table_flag & 0x40000)
          FIELD_HANDLE (header_text_style, 5, 7);
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
          FIELD_CMC (title_horiz_top_color, 64,422);
        if (border_color & 0x0002)
          FIELD_CMC (title_horiz_ins_color, 65,423);
        if (border_color & 0x0004)
          FIELD_CMC (title_horiz_bottom_color, 66,424);
        if (border_color & 0x0008)
          FIELD_CMC (title_vert_left_color, 63,421);
        if (border_color & 0x0010)
          FIELD_CMC (title_vert_ins_color, 68,426);
        if (border_color & 0x0020)
          FIELD_CMC (title_vert_right_color, 69,427);
        if (border_color & 0x0040)
          FIELD_CMC (header_horiz_top_color, 64,422);
        if (border_color & 0x0080)
          FIELD_CMC (header_horiz_ins_color, 65,423);
        if (border_color & 0x0100)
          FIELD_CMC (header_horiz_bottom_color, 66,424);
        if (border_color & 0x0200)
          FIELD_CMC (header_vert_left_color, 63,421);
        if (border_color & 0x0400)
          FIELD_CMC (header_vert_ins_color, 68,426);
        if (border_color & 0x0800)
          FIELD_CMC (header_vert_right_color, 69,427);
        if (border_color & 0x1000)
          FIELD_CMC (data_horiz_top_color, 64,422);
        if (border_color & 0x2000)
          FIELD_CMC (data_horiz_ins_color, 65,423);
        if (border_color & 0x4000)
          FIELD_CMC (data_horiz_bottom_color, 66,424);
        if (border_color & 0x8000)
          FIELD_CMC (data_vert_left_color, 63,421);
        if (border_color & 0x10000)
          FIELD_CMC (data_vert_ins_color, 68,426);
        if (border_color & 0x20000)
          FIELD_CMC (data_vert_right_color, 69,427);
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
    FIELD_HANDLE (block_header, 5, 2);
    VERSIONS (R_13, R_2000)
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
        if (!_obj->attrib_handles && _obj->num_owned)
          _obj->num_owned = 0;
  #endif
        HANDLE_VECTOR (attrib_handles, num_owned, 4, 0)
      }
  
    if (FIELD_VALUE (has_attribs)) {
      FIELD_HANDLE (seqend, 3, 0);
    }
    FIELD_HANDLE (table_style, 5, 342);
    _REPEAT_CNF (_obj->num_cells, cells, Dwg_TABLE_Cell, 1)
    REPEAT_BLOCK
        if (FIELD_VALUE (cells[rcount1].type) == 1)
          { /* text cell */
            SUB_FIELD_HANDLE (cells[rcount1],cell_handle, 5, 344);
          }
        else
          {
            /* block cell */
            SUB_FIELD_HANDLE (cells[rcount1],cell_handle, 5, 340)
          }

        if (FIELD_VALUE (cells[rcount1].type) == 2 &&
            FIELD_VALUE (cells[rcount1].additional_data_flag) == 1)
          HANDLE_VECTOR (cells[rcount1].attr_def_id, cells[rcount1].num_attr_defs, 4, 331);
        if (FIELD_VALUE (cells[rcount1].additional_data_flag2) == 1 &&
            FIELD_VALUE (cells[rcount1].cell_flag_override) & 0x08)
          SUB_FIELD_HANDLE (cells[rcount1],text_style_override, ANYCODE, 7);
    END_REPEAT_BLOCK
    SET_PARENT_OBJ (cells)
    END_REPEAT (cells);

    if (FIELD_VALUE (has_table_overrides))
      {
        BITCODE_BL table_flag;
        table_flag = FIELD_VALUE (table_flag_override);
        if (table_flag & 0x20000)
          FIELD_HANDLE (title_row_style_override, ANYCODE, 7);
        if (table_flag & 0x40000)
          FIELD_HANDLE (header_row_style_override, ANYCODE, 7);
        if (table_flag & 0x80000)
          FIELD_HANDLE (data_row_style_override, ANYCODE, 7);
      }
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
            SUB_FIELD_BL (break_heights[rcount1],flag, 0);
        END_REPEAT_BLOCK
        SET_PARENT_OBJ (break_heights)
        END_REPEAT (break_heights);
      }
    FIELD_BL (num_break_rows, 0);
    VALUEOUTOFBOUNDS (num_break_rows, 5000)
    REPEAT (num_break_rows, break_rows, Dwg_TABLE_BreakRow)
    REPEAT_BLOCK
        SUB_FIELD_3BD (break_rows[rcount1],position, 0);
        SUB_FIELD_BL (break_rows[rcount1],start, 0);
        SUB_FIELD_BL (break_rows[rcount1],end, 0);
    END_REPEAT_BLOCK
    SET_PARENT_OBJ (break_rows)
    END_REPEAT (break_rows);

    COMMON_ENTITY_HANDLE_DATA;
    FIELD_HANDLE (table_style, 5, 342);
  }

DWG_ENTITY_END

#undef row
#undef cell
#undef content
#undef geom
#undef attr
#undef merged

#endif /* DEBUG_CLASSES */

//pg.246 20.4.103
// stable
DWG_OBJECT (TABLEGEOMETRY)

  SUBCLASS (AcDbTableGeometry)
  FIELD_BL (num_rows, 90);
  VALUEOUTOFBOUNDS (num_rows, 5000)
  FIELD_BL (num_cols, 91);
  VALUEOUTOFBOUNDS (num_cols, 5000)
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
          #undef geom
      END_REPEAT_BLOCK
      SET_PARENT_FIELD (cell.geometry, geom_parent, &_obj->cell)
      END_REPEAT (cell.geometry);
      #undef cell
  END_REPEAT_BLOCK
  SET_PARENT_OBJ (cells)
  END_REPEAT (cells);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// See TABLE and p20.4.101
// Added with r2005
// TABLESTYLE only contains the Table (R24), _Title, _Header and _Data cell style.
DWG_OBJECT (TABLESTYLE)
  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbTableStyle)
  PRE (R_2010) {
    FIELD_T (name, 3);
    FIELD_BS (flow_direction, 70);
    FIELD_BS (flags, 71);
    FIELD_BD (horiz_cell_margin, 40);
    FIELD_BD (vert_cell_margin, 41);
    FIELD_B (title_suppressed, 280);
    FIELD_B (header_suppressed, 281);

    FIELD_VALUE (num_rowstyles) = 3;
    // 0: data, 1: title, 2: header
    REPEAT_CN (3, rowstyles, Dwg_TABLESTYLE_rowstyles)
    REPEAT_BLOCK
        #define rowstyle rowstyles[rcount1]
        // TODO in DXF by name
        SUB_FIELD_HANDLE (rowstyle,text_style, 5, 7);
        SUB_FIELD_BD (rowstyle,text_height, 140);
        SUB_FIELD_BS (rowstyle,text_alignment, 170);
        SUB_FIELD_CMC (rowstyle,text_color, 62,0); //FIXME
        SUB_FIELD_CMC (rowstyle,fill_color, 63,0);
        SUB_FIELD_B (rowstyle,has_bgcolor, 283);

        _obj->rowstyle.num_borders = 6;
        // top, horizontal inside, bottom, left, vertical inside, right
        _REPEAT_CN (6, rowstyle.borders, Dwg_TABLESTYLE_border, 2)
        REPEAT_BLOCK
            #define border rowstyle.borders[rcount2]
            SUB_FIELD_BSd (border,linewt, 274+rcount2);
            SUB_FIELD_B (border,visible, 284+rcount2);
            SUB_FIELD_CMC (border,color, 64+rcount2, 0);
        END_REPEAT_BLOCK
        END_REPEAT (rowstyle.borders)

        SINCE (R_2007) {
          SUB_FIELD_BL (rowstyle,data_type, 90);
          SUB_FIELD_BL (rowstyle,unit_type, 91);
          SUB_FIELD_TU (rowstyle,format_string, 1);
        }
    END_REPEAT_BLOCK
    END_REPEAT (rowstyles)
  }
  LATER_VERSIONS {
    LOG_ERROR ("TABLESTYLE r2010+ not yet implemented") // TABLE/CELLSTYLEMAP
  }
  START_OBJECT_HANDLE_STREAM;

DWG_OBJECT_END

//(79 + varies) pg.247 20.4.104
DWG_OBJECT (XRECORD)

  DXF {
    SUBCLASS (AcDbXrecord)
    SINCE (R_2000) {
      FIELD_BS (cloning_flags, 280);
    }
  }
  FIELD_BL (xdata_size, 0);
  //VALUEOUTOFBOUNDS (xdata_size, 0xf0000)
  FIELD_XDATA (xdata, xdata_size);
#ifndef IS_DXF
  SINCE (R_2000) {
    FIELD_BS (cloning_flags, 280);
  }
#endif

  START_OBJECT_HANDLE_STREAM;
  DECODER {
      for (vcount=0; bit_position (hdl_dat) < obj->handlestream_size; vcount++)
        {
          FIELD_VALUE (objid_handles) = vcount
            ? realloc (FIELD_VALUE (objid_handles),
                                   (vcount+1) * sizeof (Dwg_Object_Ref))
            : malloc (sizeof (Dwg_Object_Ref));
          FIELD_HANDLE_N (objid_handles[vcount], vcount, ANYCODE, 0);
          if (!FIELD_VALUE (objid_handles[vcount]))
            {
              if (!vcount)
                free (FIELD_VALUE (objid_handles));
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
        VALUE_H (_obj->objid_handles[rcount1], 340);
    END_REPEAT (objid_handles)
  }
#endif

DWG_OBJECT_END

//(80 + varies)
/// DXF as ACDBPLACEHOLDER
DWG_OBJECT (PLACEHOLDER)

  // no own data members
  START_OBJECT_HANDLE_STREAM;
  // no SUBCLASS marker

DWG_OBJECT_END

// SCALE (varies)
// 20.4.92 page 221
DWG_OBJECT (SCALE)

  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbScale)
  FIELD_BS (flag, 70);
  PRE (R_2007) {
    FIELD_TV (name, 300);
  }
  FIELD_BD (paper_units, 140);
  FIELD_BD (drawing_units, 141);
  FIELD_B (has_unit_scale, 290);
  SINCE (R_2007) {
#ifdef IS_DECODER
    *str_dat = *dat;
    obj->has_strings = 1;
#endif
    FIELD_T (name, 0);
  }

  START_OBJECT_HANDLE_STREAM;

DWG_OBJECT_END

#if defined (DEBUG_CLASSES) || defined (IS_FREE)

// just guessing:
// VBA_PROJECT (81 + varies), a blob
DWG_OBJECT (VBA_PROJECT)

  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbVbaProject)
#ifndef IS_JSON
  FIELD_RL (data_size, 90);
#endif
  FIELD_BINARY (data, FIELD_VALUE (data_size), 310);

  START_OBJECT_HANDLE_STREAM;

DWG_OBJECT_END

#endif

/* pg. 157, 20.4.48 (varies)
   AcDbMLeader, now implemented.
 */
DWG_ENTITY (MULTILEADER)

  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbMLeader)
  DXF_OR_PRINT { VALUE_TFF ("CONTEXT_DATA{", 300); } //AcDbObjectContextData

  SINCE (R_2010)
    {
      FIELD_BS (class_version, 270); // default 2. 1 <= r2004
      VALUEOUTOFBOUNDS (class_version, 10)
    }
#if 0
  FIELD_BS (ctx.class_version, 70); // default 3
  FIELD_B (ctx.has_xdic_file, 0);
  FIELD_B (ctx.is_default, 290);
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
      END_REPEAT_BLOCK
      SET_PARENT (lnode.breaks, (struct _dwg_LEADER_Line *)&_obj->lnode);
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
          END_REPEAT_BLOCK
          SET_PARENT (lline.breaks, &_obj->lline);
          END_REPEAT (lline.breaks);
          SUB_FIELD_BL (lline, line_index, 91);

          SINCE (R_2010)
            {
              SUB_FIELD_BS (lline, type, 170);
              SUB_FIELD_CMC (lline, color, 92,0);
              SUB_FIELD_HANDLE (lline, ltype, 5, 340);
              SUB_FIELD_BLd (lline, linewt, 171);
              SUB_FIELD_BD (lline, arrow_size, 40);
              SUB_FIELD_HANDLE (lline, arrow_handle, 5, 341);
              SUB_FIELD_BL (lline, flags, 93);
            }
            #undef lline
      END_REPEAT_BLOCK
      SET_PARENT (lnode.lines, &_obj->lnode);
      END_REPEAT (lnode.lines)
      SINCE (R_2010)
        SUB_FIELD_BS (lnode, attach_dir, 271);
      DXF_OR_PRINT { VALUE_TFF ("}", 305); }
  END_REPEAT_BLOCK
  SET_PARENT_OBJ (ctx.leaders)
  END_REPEAT (ctx.leaders)
  DXF_OR_PRINT { VALUE_TFF ("}", 303); }

  FIELD_BD (ctx.scale, 40);
  FIELD_3BD (ctx.content_base, 10);
  FIELD_BD (ctx.text_height, 41);
  FIELD_BD (ctx.arrow_size, 140);
  FIELD_BD (ctx.landing_gap, 145);
  FIELD_BS (ctx.text_left, 174);
  FIELD_BS (ctx.text_right, 175);
  FIELD_BS (ctx.text_alignment, 176);
  FIELD_BS (ctx.attach_type, 177);

  FIELD_B (ctx.has_content_txt, 290);
  if (FIELD_VALUE (ctx.has_content_txt))
    {
      FIELD_T (ctx.content.txt.default_text, 304);
      FIELD_3BD (ctx.content.txt.normal, 11);
      FIELD_HANDLE (ctx.content.txt.style, 5, 340);
      FIELD_3BD (ctx.content.txt.location, 12);
      FIELD_3BD (ctx.content.txt.direction, 13);
      FIELD_BD (ctx.content.txt.rotation, 42);
      FIELD_BD (ctx.content.txt.width, 43);
      FIELD_BD (ctx.content.txt.height, 44);
      FIELD_BD (ctx.content.txt.line_spacing_factor, 45);
      FIELD_BS (ctx.content.txt.line_spacing_style, 170);
      FIELD_CMC (ctx.content.txt.color, 90,0);
      FIELD_BS (ctx.content.txt.alignment, 171);
      FIELD_BS (ctx.content.txt.flow, 172);
      FIELD_CMC (ctx.content.txt.bg_color, 91,0);
      FIELD_BD (ctx.content.txt.bg_scale, 141); // r2000!!
      FIELD_BL (ctx.content.txt.bg_transparency, 92);
      FIELD_B (ctx.content.txt.is_bg_fill, 291);
      FIELD_B (ctx.content.txt.is_bg_mask_fill, 292);
      FIELD_BS (ctx.content.txt.col_type, 173);
      FIELD_B (ctx.content.txt.is_height_auto, 293);
      FIELD_BD (ctx.content.txt.col_width, 142);
      FIELD_BD (ctx.content.txt.col_gutter, 143);
      FIELD_B (ctx.content.txt.is_col_flow_reversed, 294);
      FIELD_BL (ctx.content.txt.num_col_sizes, 0);
      //VALUEOUTOFBOUNDS (ctx.content.txt.num_col_sizes, 5000)
      FIELD_VECTOR (ctx.content.txt.col_sizes, BD, ctx.content.txt.num_col_sizes, 144);
      FIELD_B (ctx.content.txt.word_break, 295);
      FIELD_B (ctx.content.txt.unknown, 0);
    }
  else // a union. either txt or blk
    {
      FIELD_B (ctx.has_content_blk, 296);
      if (FIELD_VALUE (ctx.has_content_blk))
        {
          FIELD_HANDLE (ctx.content.blk.block_table, 4, 341);
          FIELD_3BD (ctx.content.blk.normal, 14);
          FIELD_3BD (ctx.content.blk.location, 15);
          FIELD_3BD (ctx.content.blk.scale, 16);
          FIELD_BD (ctx.content.blk.rotation, 46);
          FIELD_CMC (ctx.content.blk.color, 93,0);
          FIELD_VECTOR_N (ctx.content.blk.transform, BD, 16, 47);
        }
    }

  FIELD_3BD (ctx.base, 110);
  FIELD_3BD (ctx.base_dir, 111);
  FIELD_3BD (ctx.base_vert, 112);
  FIELD_B (ctx.is_normal_reversed, 297);

  SINCE (R_2010)
    {
      FIELD_BS (ctx.text_top, 273);
      FIELD_BS (ctx.text_bottom, 272);
    }
  DXF_OR_PRINT { VALUE_TFF ("}", 301); } //end CONTEXT_DATA
  // END MLEADER_AnnotContext

  FIELD_BL (flags, 90); // override flags
  FIELD_BS (type, 170);
  FIELD_CMC (color, 91,0);
  FIELD_BLd (linewt, 171);
  FIELD_B (has_landing, 290);
  FIELD_B (has_dogleg, 291);
  FIELD_BD (landing_dist, 41); // FIXME unstable
  DECODER {
    if (bit_isnan (FIELD_VALUE (landing_dist)))
      {
        FIELD_VALUE (landing_dist) = 0.0;
        return DWG_ERR_VALUEOUTOFBOUNDS;
      }
  }
  FIELD_BD (arrow_size, 42);
  FIELD_BS (style_content, 172);
  FIELD_BS (text_left, 173);
  FIELD_BS (text_right, 95);
  FIELD_BS (text_angletype, 174);
  FIELD_BS (attach_type, 175); // unknown at ODA
  FIELD_CMC (text_color, 92,0);
  FIELD_B (has_text_frame, 292);
  FIELD_CMC (block_color, 93,0);
  FIELD_3BD (block_scale, 10);
  FIELD_BD (block_rotation, 43);
  FIELD_BS (style_attachment, 176);
  FIELD_B (is_annotative, 293);

  VERSIONS (R_2000, R_2007)
    {
      FIELD_BL (num_arrowheads, 0);
      VALUEOUTOFBOUNDS (num_arrowheads, 5000)
      REPEAT (num_arrowheads, arrowheads, Dwg_LEADER_ArrowHead)
      REPEAT_BLOCK
          SUB_FIELD_BL (arrowheads[rcount1],is_default, 94);
          SUB_FIELD_HANDLE (arrowheads[rcount1],arrowhead, 5, 345);
      END_REPEAT_BLOCK
      SET_PARENT_OBJ (arrowheads)
      END_REPEAT (arrowheads);

      FIELD_BL (num_blocklabels, 0);
      VALUEOUTOFBOUNDS (num_blocklabels, 5000)
      REPEAT (num_blocklabels, blocklabels, Dwg_LEADER_BlockLabel)
      REPEAT_BLOCK
          SUB_FIELD_HANDLE (blocklabels[rcount1],attdef, 4, 330);
          SUB_FIELD_T (blocklabels[rcount1],label_text, 302);
          SUB_FIELD_BS (blocklabels[rcount1],ui_index, 177);
          SUB_FIELD_BD (blocklabels[rcount1],width, 44);
      END_REPEAT_BLOCK
      SET_PARENT_OBJ (blocklabels)
      END_REPEAT (blocklabels)
      FIELD_B (neg_textdir, 294);
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
    FIELD_B (text_extended, 295);

  COMMON_ENTITY_HANDLE_DATA;
  FIELD_HANDLE (mleaderstyle, 5, 340);
  FIELD_HANDLE (ltype, 5, 341);
  FIELD_HANDLE (arrow_handle, 5, 342);
  FIELD_HANDLE (text_style, 5, 343);
  FIELD_HANDLE (block_style, 5, 344);

DWG_ENTITY_END

/* par 20.4.87 (varies) */
DWG_OBJECT (MLEADERSTYLE)

  SUBCLASS (AcDbMLeaderStyle)
  SINCE (R_2010)
    {
      IF_ENCODE_FROM_EARLIER {
        FIELD_VALUE (class_version) = 2;
      }
      FIELD_BS (class_version, 179);
      VALUEOUTOFBOUNDS (class_version, 10)
    }

  FIELD_BS (content_type, 170);
  FIELD_BS (mleader_order, 171);
  FIELD_BS (leader_order, 172);
  FIELD_BL (max_points, 90);
  FIELD_BD (first_seg_angle, 40);
  FIELD_BD (second_seg_angle, 41);
  FIELD_BS (type, 173);
  FIELD_CMC (line_color, 91,0);
  FIELD_BLd (linewt, 92);
  FIELD_B (has_landing, 290);
  FIELD_BD (landing_gap, 42);
  FIELD_B (has_dogleg, 291);
  FIELD_BD (landing_dist, 43);
  FIELD_T (description, 3);
  FIELD_BD (arrow_head_size, 44);
  FIELD_T (text_default, 300);
  FIELD_BS (attach_left, 174);
  FIELD_BS (attach_right, 178);
  FIELD_BS (text_angle_type, 175);
  FIELD_BS (text_align_type, 176);
  FIELD_CMC (text_color, 93,0);
  FIELD_BD (text_height, 45);
  FIELD_B (text_frame, 292);
  //is_new_format: if the object has extended data for APPID “ACAD_MLEADERVER”.
  if (FIELD_VALUE (is_new_format) || dat->version >= R_2010) {
    FIELD_B (text_always_left, 297);
  }
  FIELD_BD (align_space, 46);
  FIELD_CMC (block_color, 94,0);
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
  FIELD_B (changed, 295);
  FIELD_B (is_annotative, 296);
  FIELD_BD (break_size, 143);

  SINCE (R_2010)
    {
      FIELD_BS (attach_dir, 271);
      FIELD_BS (attach_top, 273);
      FIELD_BS (attach_bottom, 272);
    }

  START_OBJECT_HANDLE_STREAM;
  FIELD_HANDLE (line_type, 5, 340);
  FIELD_HANDLE (arrow_head, 5, 341);
  FIELD_HANDLE (text_style, 5, 342);
  FIELD_HANDLE (block, 5, 343);

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
DWG_ENTITY (WIPEOUT)

  //SUBCLASS (AcDbImage)
  //SUBCLASS (AcDbRasterImage)
  SUBCLASS (AcDbWipeout)
  FIELD_BL (class_version, 90);
  VALUEOUTOFBOUNDS (class_version, 10)
  FIELD_3DPOINT (pt0, 10);
  FIELD_3DPOINT (uvec, 11);
  FIELD_3DPOINT (vvec, 12);
  FIELD_2RD (size, 13);
  FIELD_BS (display_props, 70);
  FIELD_B (clipping, 280);
  FIELD_RC (brightness, 281);
  FIELD_RC (contrast, 282);
  FIELD_RC (fade, 283);

  SINCE (R_2010) {
    FIELD_B (clip_mode, 290);
  }
  FIELD_BS (clip_boundary_type, 71); // 1 rect, 2 polygon
  if (FIELD_VALUE (clip_boundary_type) == 1)
    FIELD_VALUE (num_clip_verts) = 2;
  else
    FIELD_BL (num_clip_verts, 91);
  VALUEOUTOFBOUNDS (num_clip_verts, 5000)
  FIELD_2RD_VECTOR (clip_verts, num_clip_verts, 14);

  COMMON_ENTITY_HANDLE_DATA;
  FIELD_HANDLE (imagedef, 5, 340);
  FIELD_HANDLE (imagedefreactor, 3, 360);

DWG_ENTITY_END

// (varies)
// in DXF as {PDF,DWF,DGN}DEFINITION
// no DWF, DGN coverage yet
DWG_OBJECT (UNDERLAYDEFINITION)

  //DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbUnderlayDefinition)
  FIELD_T (filename, 1);
  FIELD_T (name, 2);
  START_OBJECT_HANDLE_STREAM;

DWG_OBJECT_END

// (varies)
// in DXF as 0 DGNUNDERLAY DWFUNDERLAY PDFUNDERLAY
// looks perfect, but no DWF, DGN coverage yet
DWG_ENTITY (UNDERLAY)

  //DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbUnderlayReference)
  FIELD_3BD (extrusion, 210);
  FIELD_3DPOINT (insertion_pt, 10);
  FIELD_BD (angle, 50);
  FIELD_3BD_1 (scale, 41);
  FIELD_RC (flag, 280);
  FIELD_RCd (contrast, 281); // 20-100. def: 100
  FIELD_RCd (fade, 282);  // 0-80

  FIELD_BL (num_clip_verts, 0);
  VALUEOUTOFBOUNDS (num_clip_verts, 5000)
  FIELD_2RD_VECTOR (clip_verts, num_clip_verts, 11);

  COMMON_ENTITY_HANDLE_DATA;
  FIELD_HANDLE (underlay_layer, 5, 0);
  FIELD_HANDLE (definition_id, 5, 340);

DWG_ENTITY_END

DWG_ENTITY (CAMERA) // i.e. a named view, not persistent in a DWG. CAMERADISPLAY=1

  //DECODE_UNKNOWN_BITS
  COMMON_ENTITY_HANDLE_DATA;
  FIELD_HANDLE (view, 5, 0);

DWG_ENTITY_END

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
  COMMON_ENTITY_HANDLE_DATA; // including this
DWG_ENTITY_END

/* container to hold a raw class object (varies) */
DWG_OBJECT (UNKNOWN_OBJ)
  DECODE_UNKNOWN_BITS
DWG_OBJECT_END

// just a dummy dwg filer, ignored for dxf
DWG_OBJECT (DUMMY)
  DECODE_UNKNOWN_BITS
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

// (varies) UNSTABLE
// works ok on all example_20* but this coverage seems limited
DWG_OBJECT (PERSSUBENTMANAGER)

  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbPersSubentManager)
  FIELD_BL (class_version, 90); //2
  VALUEOUTOFBOUNDS (class_version, 10)
  FIELD_BL (unknown_bl1, 90); //0
  FIELD_BL (unknown_bl2, 90); //2
  FIELD_BL (unknown_bl3, 90); //3
  FIELD_BL (unknown_bl4, 90); //0
  FIELD_BL (unknown_bl5, 90); //1
  FIELD_BL (unknown_bl6, 90); //1

  START_OBJECT_HANDLE_STREAM;

DWG_OBJECT_END

// (varies) UNSTABLE
// works ok on all Surface_20* but this coverage seems limited.
// field names may change.
// See AcDbAssocDependency.h
DWG_OBJECT (ASSOCDEPENDENCY)

  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbAssocDependency)
  FIELD_BL (class_version, 90); //2
  VALUEOUTOFBOUNDS (class_version, 10)
  FIELD_BL (status, 90); //1 or depbody
  FIELD_B  (isread_dep, 290); //0
  FIELD_B  (iswrite_dep, 290); //1
  FIELD_B  (isobjectstate_dep, 290); //1
  FIELD_B  (unknown_b4, 290); //1
  FIELD_BL (order, 90); //-2147483648 (-1) or 0
  FIELD_B  (unknown_b5, 290); //0
  FIELD_BL (depbodyid, 90); //1

  START_OBJECT_HANDLE_STREAM;
  FIELD_HANDLE (writedep, 4, 360);
  FIELD_HANDLE (readdep, 4, 330);
  FIELD_HANDLE (node, 3, 330);
DWG_OBJECT_END

#define AcDbAssocParamBasedActionBody_fields \
  SUBCLASS (AcDbAssocActionBody) \
  FIELD_BL (aab_status, 90); \
  SUBCLASS (AcDbAssocParamBasedActionBody) \
  FIELD_BL (pab_status, 90); \
  FIELD_BL (pab_l2, 90); \
  FIELD_BL (num_deps, 90); \
  FIELD_BL (pab_l4, 90); \
  FIELD_BL (pab_l5, 90)

#define AcDbAssocPathBasedSurfaceActionBody_fields \
  AcDbAssocParamBasedActionBody_fields; \
  SUBCLASS (AcDbAssocSurfaceActionBody)	\
  FIELD_BL (sab_status, 90); \
  FIELD_B (sab_b1, 290); \
  FIELD_BL (sab_l2, 90); \
  FIELD_B (sab_b2, 290); \
  FIELD_BS (sab_s1, 70); \
  SUBCLASS (AcDbAssocPathBasedSurfaceActionBody) \
  FIELD_BL (pbsab_status, 90)

// (varies) UNSTABLE
// works ok on all Surface_20* but this coverage seems limited.
// field names may change.
// See AcDbAssocActionBody.h
// summary: 78/98=79.59%
DWG_OBJECT (ASSOCPLANESURFACEACTIONBODY)
  DECODE_UNKNOWN_BITS
  AcDbAssocPathBasedSurfaceActionBody_fields;
  SUBCLASS (AcDbAssocPlaneSurfaceActionBody)
  FIELD_BL (psab_status, 90);

  START_OBJECT_HANDLE_STREAM;
  HANDLE_VECTOR (writedeps, num_deps, 0, 360);
  HANDLE_VECTOR (readdeps, num_deps, 0, 360);
  FIELD_VECTOR_T (descriptions, T, num_deps, 1);
DWG_OBJECT_END

// (varies) UNSTABLE
// 1-4 references, see associativity bits 1-8.
DWG_OBJECT (DIMASSOC)

  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbDimAssoc)
  FIELD_HANDLE (dimensionobj, 4, 330);
  FIELD_BLx (associativity, 90);
  FIELD_RC (trans_space_flag, 70);
  // missing from DWG, always 0
  DXF { FIELD_BS (rotated_type, 71); }
  REPEAT_CN (4, ref, Dwg_DIMASSOC_Ref)
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
      SUB_FIELD_B  (ref[rcount1], has_lastpt_ref, 75);
      SUB_FIELD_T  (ref[rcount1], classname, 1); // "AcDbOsnapPointRef"
      SUB_FIELD_RC (ref[rcount1], osnap_type, 72); // 0-13
      SUB_FIELD_BS (ref[rcount1], num_mainobjs, 0); // 1 or 2
      SUB_VALUEOUTOFBOUNDS (ref[rcount1], num_mainobjs, 100)
      SUB_FIELD_BS (ref[rcount1], main_subent_type, 73);  // if 0 not in DXF
      if (FIELD_VALUE (ref[rcount1].main_subent_type))
        {
          SUB_FIELD_BL (ref[rcount1], main_gsmarker, 91);
          SUB_FIELD_BS (ref[rcount1], intsect_subent_type, 74); // if 0 not in DXF
          SUB_HANDLE_VECTOR (ref[rcount1], mainobjs, num_mainobjs, 4, 331);
          if (FIELD_VALUE (ref[rcount1].intsect_subent_type))
            SUB_FIELD_HANDLE (ref[rcount1], intsectobj, 4, 332); // 0 (absent), 1-3
          SUB_FIELD_BD (ref[rcount1], osnap_dist, 40);
        }
      SUB_FIELD_3BD (ref[rcount1], osnap_pt, 10);
  END_REPEAT_BLOCK
  SET_PARENT_OBJ (ref)
  END_REPEAT (ref)
  //FIELD_BL (intsect_gsmarker, 92);

  START_OBJECT_HANDLE_STREAM;
  //FIELD_HANDLE (xrefobj, 4, 301); // TODO optional
  //FIELD_HANDLE (intsectxrefobj, 4, 302); // TODO multiple

DWG_OBJECT_END

// r2007+ UNSTABLE
// dbvisualstyle.h
DWG_OBJECT (VISUALSTYLE)

  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbVisualStyle)
  FIELD_T (description, 2);
  FIELD_BS (style_type, 70);
  SINCE (R_2010) {
    FIELD_BS (ext_lighting_model, 177);
    FIELD_B (has_ext, 291)
  }
  FIELD_BS (face_lighting_model, 71);
  SINCE (R_2010) { FIELD_BS (face_lighting_model_ext, 176); } // 1
  FIELD_BS (face_lighting_quality, 72);
  SINCE (R_2010) { FIELD_BS (face_lighting_quality_ext, 176); } // 1
  FIELD_BS (face_color_mode, 73);
  SINCE (R_2010) { FIELD_BS (face_color_mode_ext, 176); } // 1
  SINCE (R_2010) {
    FIELD_BS (face_modifier, 90);
    FIELD_BS (face_modifier_ext, 0);
  }
  FIELD_BD (face_opacity, 40);
  SINCE (R_2010) { FIELD_BS (face_opacity_ext, 176); } // 1
  FIELD_BD (face_specular, 41);
  SINCE (R_2010) { FIELD_BS (face_specular_ext, 176); } // 1
  FIELD_CMC (face_mono_color, 63,421);
  SINCE (R_2010) { FIELD_BS (face_mono_color_ext, 176); } // 1
  UNTIL (R_2007) { FIELD_BS (face_modifier, 90); } // ?
  FIELD_BS (edge_model, 74);
  SINCE (R_2010) { FIELD_BS (edge_model_ext, 176); } // 1
  FIELD_BL (edge_style, 91);
  SINCE (R_2010) { FIELD_BS (edge_style_ext, 176); } // 1
  FIELD_CMC (edge_intersection_color, 64,422);
  SINCE (R_2010) { FIELD_BS (edge_intersection_color_ext, 176); } // 1
  FIELD_CMC (edge_obscured_color, 65,423);
  SINCE (R_2010) { FIELD_BS (edge_obscured_color_ext, 176); } // 1
  FIELD_BS (edge_obscured_line_pattern, 75);
  SINCE (R_2010) { FIELD_BS (edge_obscured_line_pattern_ext, 176); } // 1
  SINCE (R_2010) {
    FIELD_BS (edge_intersection_line_pattern, 175);
    FIELD_BS (edge_intersection_line_pattern_ext, 176);
  }
//#if defined (DEBUG_CLASSES) || defined (IS_FREE)
  // unstable <r2010:
  FIELD_BD (edge_crease_angle, 42);
  SINCE (R_2010) { FIELD_BS (edge_crease_angle_ext, 176); }
  FIELD_BS (edge_modifier, 92);
  SINCE (R_2010) { FIELD_BS (edge_modifier_ext, 176); } // 1
  FIELD_CMC (edge_color, 66,424);
  SINCE (R_2010) { FIELD_BS (edge_color_ext, 176); } // 1
  FIELD_BD (edge_opacity, 43);
  SINCE (R_2010) { FIELD_BS (edge_opacity_ext, 176); } // 1
  FIELD_BS (edge_width, 76); // 1
  SINCE (R_2010) { FIELD_BS (edge_width_ext, 176); } // 1
  FIELD_BS (edge_overhang, 77); // 6
  SINCE (R_2010) { FIELD_BS (edge_overhang_ext, 176); } // 1
  FIELD_BS (edge_jitter, 78); // 2
  SINCE (R_2010) { FIELD_BS (edge_jitter_ext, 176); } // 1
  FIELD_CMC (edge_silhouette_color, 67,425);
  SINCE (R_2010) { FIELD_BS (edge_silhouette_color_ext, 176); } // 1
  FIELD_BS (edge_silhouette_width, 79); // 3 or 5
  SINCE (R_2010) { FIELD_BS (edge_silhouette_width_ext, 176); } // 1
  UNTIL (R_2007) {
    //FIELD_B (unknown_b, 0);
    DEBUG_HERE_OBJ
    //FIELD_BS (edge_intersection_line_pattern, 175); // 1 or 7
  }
  FIELD_BS (edge_halo_gap, 170); // 0
  SINCE (R_2010) { FIELD_BS (edge_halo_gap_ext, 176); }
  //FIELD_CMC (color, 62,420);
  //SINCE (R_2010) { FIELD_BS (color_ext, 176); } // 1

  FIELD_BS (num_edge_isolines, 171);
  VALUEOUTOFBOUNDS (num_edge_isolines, 5000)
  SINCE (R_2010) { FIELD_BS (num_edge_isolines_ext, 176); } // 1

  FIELD_B (edge_hide_precision_flag, 290);
  SINCE (R_2010) { FIELD_BS (edge_hide_precision_flag_ext, 176); }
  UNTIL (R_2007) {
    FIELD_BS (edge_style_apply_flag, 174);
  }
  FIELD_BS (display_style, 93); // 1
  SINCE (R_2010) { FIELD_BS (display_style_ext, 176); } // 1
  FIELD_BS (display_shadow_type, 173); // 0
  SINCE (R_2010) { FIELD_BS (display_shadow_type_ext, 176); } // 1
  FIELD_BD (display_brightness, 44); // 0.0
  SINCE (R_2010) { FIELD_BS (display_brightness_ext, 176); } // 1
  DXF {
    UNTIL (R_2007) { FIELD_B (has_ext, 291); }
    FIELD_BD (unknown_float45, 45);  // 0.0
  }
#if !(defined (DEBUG_CLASSES) || defined (IS_FREE))
  UNTIL (R_2007) {
    DECODER { // unstable, overwrite with defaults
      FIELD_VALUE (edge_crease_angle) = 1.0;
      FIELD_VALUE (edge_opacity) = 1.0;
      FIELD_VALUE (edge_width) = 1;
      FIELD_VALUE (edge_silhouette_width) = 3; // or 5
      FIELD_VALUE (edge_overhang) = 6;
      FIELD_VALUE (edge_jitter) = 2;
      FIELD_VALUE (display_style) = 1;
      FIELD_VALUE (edge_hide_precision_flag) = 0;
      FIELD_VALUE (has_ext) = 1;
      SINCE (R_2010) {
        FIELD_VALUE (edge_crease_angle_ext) = 1;
        FIELD_VALUE (edge_color_ext) = 1;
        FIELD_VALUE (edge_opacity_ext) = 1;
        FIELD_VALUE (edge_width_ext) = 1;
        FIELD_VALUE (edge_overhang_ext) = 1;
        FIELD_VALUE (edge_jitter_ext) = 1;
        FIELD_VALUE (edge_silhouette_color_ext) = 1;
        FIELD_VALUE (edge_silhouette_width_ext) = 1;
      }
    }
  }
#endif

  START_OBJECT_HANDLE_STREAM;
  //FIELD_HANDLE (dictionary, 5, 0);
DWG_OBJECT_END

// (varies) In Work
/* Container for all properties relating to a generic light.  A
   dictionary of these objects is resident in the database, in the
   named object dictionary as ACAD_LIGHTS (Object LIGHTLIST). They are
   indexed by the name of the setting objects. In the user interface,
   the contents of this dictionary correspond to user-defined light
   properties (displayed on a property palette accessible by selection
   of the light using a variety of methods.
   SpotLight, PointLight, DistantLight. dbLight.h
 */
DWG_ENTITY (LIGHT)

  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbLight);
  FIELD_BL (class_version, 90); //1
  VALUEOUTOFBOUNDS (class_version, 10)
  FIELD_T (name, 1);
  FIELD_BS (type, 70);
  FIELD_B (status, 290); //127

  FIELD_CMC (color, 63,421);
  FIELD_B (plot_glyph, 291);

#if 0
    FIELD_BS (lamp_color_type, 0); //0: in kelvin, 1: as preset
    if (FIELD_VALUE (lamp_color_type) == 0) {
      FIELD_BD (lamp_color_temp, 0);
    } else {
      FIELD_BS (lamp_color_preset, 0);
      if (FIELD_VALUE (lamp_color_preset) == 14) // Custom
        FIELD_BLx (lamp_color_rgb, 0);
    }
    FIELD_B (has_target_grip, 0);
    FIELD_BS (glyph_display_type, 0);
    FIELD_T (web_file, 0);
    FIELD_3BD (web_rotation, 0);
    FIELD_BS (physical_intensity_method, 0);
    FIELD_BS (drawable_type, 0);
#endif

  FIELD_BD (intensity, 40);
  FIELD_3BD (position, 10);
  FIELD_3BD (target, 11);
  FIELD_BS (attenuation_type, 72);
  FIELD_B (use_attenuation_limits, 292);
  FIELD_BD (attenuation_start_limit, 41);
  FIELD_BS (shadow_map_size, 91); //256 11
  FIELD_BD (attenuation_end_limit, 42);
  FIELD_BD (hotspot_angle, 50);
  if (FIELD_VALUE (type == 3))
    FIELD_BD (falloff_angle, 51);
  FIELD_B (cast_shadows, 293);
  FIELD_BS (shadow_type, 73);   // 0 or 1
  FIELD_RC (shadow_map_softness, 280);

  COMMON_ENTITY_HANDLE_DATA;
  FIELD_HANDLE (lights_layer, 5, 0);

DWG_ENTITY_END

// (varies)
// ENHANCEDBLOCK => AcDbDynamicBlockRoundTripPurgePreventer
DWG_OBJECT (DYNAMICBLOCKPURGEPREVENTER)

  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbDynamicBlockPurgePreventer)
  FIELD_BS (flag, 70); //1 class_version would be 90
  START_OBJECT_HANDLE_STREAM;
  FIELD_HANDLE (block, 5, 0)
DWG_OBJECT_END

// UNSTABLE. missing color index 62: 21
DWG_OBJECT (DBCOLOR)

  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbColor)
  FIELD_BL (class_version, 90); //0
  VALUEOUTOFBOUNDS (class_version, 10)
#if 0
  FIELD_BB (unknown1, 0); //2
  FIELD_RLx (rgb, 420);   //32
  FIELD_RC (unknown2, 0); //8
#elsif 0
  FIELD_BSx (rgb, 420);   //18
  FIELD_RS  (unknown1, 0);//16
  FIELD_RC  (unknown2, 0);//8
#else
  DXF { FIELD_VALUE (rgb) = FIELD_VALUE (rgb) & 0xffffff; }
  FIELD_BLx (rgb, 420);   //34
  DECODER {
    FIELD_VALUE (unknown1) = FIELD_VALUE (rgb) & 0xff000000; //0xc2
  }
  FIELD_RC (unknown2, 0); //8
#endif
  DXF {
    char *s = malloc (strlen (_obj->name) + strlen (_obj->catalog) + 2);
    strcpy (s, _obj->catalog);
    strcat (s, "$");
    strcat (s, _obj->name);
    VALUE_TV (s, 430);
    free (s);
  }
  FIELD_T (name, 0);    //2nd part of 430: DIC \d+
  FIELD_T (catalog, 0); //1st part of 430: DIC COLOR GUIDE (R)

  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// (varies) UNSTABLE
// dbhelix.h
DWG_ENTITY (HELIX)

  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbSpline)
  FIELD_BL (scenario, 0);
  UNTIL (R_2013) {
    if (FIELD_VALUE (scenario) != 1 && FIELD_VALUE (scenario) != 2)
      LOG_ERROR ("unknown scenario %d", FIELD_VALUE (scenario));
  }
  SINCE (R_2013) {
    FIELD_BL (splineflags1, 0);
    FIELD_BL (knotparam, 0);
    if (FIELD_VALUE (splineflags1) & 1)
      FIELD_VALUE (scenario) = 2;
    if (FIELD_VALUE (knotparam) == 15)
      FIELD_VALUE (scenario) = 1;
  }

  DXF {
    // extrusion on planar
    VALUE_RD (0.0, 210); VALUE_RD (0.0, 220); VALUE_RD (1.0, 230);
    FIELD_BL (flag, 70);
  }
  FIELD_BL (degree, 71);

  if (FIELD_VALUE (scenario) & 2) // bezier spline
    {
      FIELD_VALUE (flag) = 8 + 32 + //planar, not rational
        // ignore method fit points and closed bits
        ((FIELD_VALUE (splineflags1) & ~5) << 7);
      FIELD_BD (fit_tol, 44); // def: 0.0000001
      FIELD_3BD (beg_tan_vec, 12);
      FIELD_3BD (end_tan_vec, 13);
      FIELD_BL (num_fit_pts, 74);
      VALUEOUTOFBOUNDS (num_fit_pts, 5000)
    }
  if (FIELD_VALUE (scenario) & 1) // spline
    {
      FIELD_B (rational, 0); // flag bit 2
      FIELD_B (closed_b, 0); // flag bit 0
      FIELD_B (periodic, 0); // flag bit 1
      FIELD_BD (knot_tol, 42); // def: 0.0000001
      FIELD_BD (ctrl_tol, 43); // def: 0.0000001
      FIELD_BL (num_knots, 72);
      VALUEOUTOFBOUNDS (num_knots, 10000)
      FIELD_BL (num_ctrl_pts, 73);
      VALUEOUTOFBOUNDS (num_ctrl_pts, 10000)
      FIELD_B (weighted, 0);

      FIELD_VALUE (flag) = 8 + //planar
        FIELD_VALUE (closed_b) +
        (FIELD_VALUE (periodic) << 1) +
        (FIELD_VALUE (rational) << 2) +
        (FIELD_VALUE (weighted) << 3);
    }

  if (FIELD_VALUE (scenario) & 1) {
    FIELD_VECTOR (knots, BD, num_knots, 40)
    REPEAT (num_ctrl_pts, ctrl_pts, Dwg_SPLINE_control_point)
    REPEAT_BLOCK
        SUB_FIELD_3BD_inl (ctrl_pts[rcount1], xyz, 10);
        if (!FIELD_VALUE (weighted))
          FIELD_VALUE (ctrl_pts[rcount1].w) = 0; // skipped when encoding
        else
          SUB_FIELD_BD (ctrl_pts[rcount1], w, 41);
    END_REPEAT_BLOCK
    SET_PARENT (ctrl_pts, (Dwg_Entity_SPLINE*)_obj);
    END_REPEAT (ctrl_pts);
  }
  if (FIELD_VALUE (scenario) & 2) {
    FIELD_3DPOINT_VECTOR (fit_pts, num_fit_pts, 11)
  }

  SUBCLASS (AcDbHelix)
  FIELD_BS (major_version, 90);
  FIELD_BS (maint_version, 91);
  FIELD_3BD (axis_base_pt, 10);
  FIELD_3BD_1 (start_pt, 11);
  FIELD_3BD_1 (axis_vector, 12);
  FIELD_BD (radius, 40);
  FIELD_BD (num_turns, 41);
  FIELD_BD (turn_height, 43);
  FIELD_B (handedness, 290); //0 left, 1 right (twist)
  FIELD_BS (constraint_type, 280); //0 constrain turn height, 1 turns, 2 height

DWG_ENTITY_END

// (varies) UNSTABLE
DWG_OBJECT (PLOTSETTINGS)
  DECODE_UNKNOWN_BITS
  // See also LAYOUT
  SUBCLASS (AcDbPlotSettings)
  FIELD_BS (plot_layout, 70); /*!< plot layout flag:
                                1 = PlotViewportBorders
                                2 = ShowPlotStyles
                                4 = PlotCentered
                                8 = PlotHidden
                                16 = UseStandardScale
                                32 = PlotPlotStyles
                                64 = ScaleLineweights
                                128 = PrintLineweights
                                512 = DrawViewportsFirst
                                1024 = ModelType
                                2048 = UpdatePaper
                                4096 = ZoomToPaperOnUpdate
                                8192 = Initializing
                                16384 = PrevPlotInit */
  FIELD_T (page_setup_name, 1);
  FIELD_T (printer_cfg_file, 2);
  FIELD_T (paper_size, 4);
  FIELD_HANDLE (plotview, 0, 6);
  //DEBUG_HERE_OBJ;
  FIELD_BD (left_margin, 40); // 6.349999904632568
  FIELD_BD (bottom_margin, 41);
  FIELD_BD (right_margin, 42);
  FIELD_BD (top_margin, 43);
  FIELD_BD (paper_width, 44); // in mm
  FIELD_BD (paper_height, 45); // in mm
  FIELD_2BD_1 (plot_origin, 46); // + 47
  FIELD_2BD_1 (plot_window_ll, 48); // + 49
  FIELD_2BD_1 (plot_window_ur, 140); // + 141
  FIELD_BD (paper_units, 142);
  FIELD_BD (drawing_units, 143);
  FIELD_BS (plot_paper_unit, 72); /*!< 0 inches, 1 mm, 2 pixel */
  FIELD_BS (plot_rotation, 73);   /*!< 0 normal, 1 90, 2 180, 3 270 deg */
  FIELD_BS (plot_type, 74);       /*!< 0 display, 1 extents, 2 limits, 3 view (see DXF 6),
                                       4 window (see 48-140), 5 layout */
  FIELD_HANDLE (stylesheet, 0, 7);
  FIELD_B (use_std_scale, 0);
  FIELD_BS (std_scale_type, 75); /*!< 0 = scaled to fit,
                                   1 = 1/128"=1', 2 = 1/64"=1', 3 = 1/32"=1'
                                   4 = 1/16"=1', 5 = 3/32"=1', 6 = 1/8"=1'
                                   7 = 3/16"=1', 8 = 1/4"=1', 9 = 3/8"=1'
                                   10 = 1/2"=1', 11 = 3/4"=1', 12 = 1"=1'
                                   13 = 3"=1', 14 = 6"=1', 15 = 1'=1'
                                   16 = 1:1, 17= 1:2, 18 = 1:4 19 = 1:8, 20 = 1:10, 21= 1:16
                                   22 = 1:20, 23 = 1:30, 24 = 1:40, 25 = 1:50, 26 = 1:100
                                   27 = 2:1, 28 = 4:1, 29 = 8:1, 30 = 10:1, 31 = 100:1, 32 = 1000:1
                               */
  FIELD_BD (std_scale_factor, 147); /*!< value of 75 */
  FIELD_2BD_1 (paper_image_origin, 148); // + 149
  SINCE (R_2004)
    {
      FIELD_BS (shade_plot_mode, 76); /*!< 0 display, 1 wireframe, 2 hidden, 3 rendered, 4 visualstyle,
                                           5 renderPreset */
      FIELD_BS (shade_plot_reslevel, 77); /*!< 0 draft, 1 preview, 2 nomal,
                                               3 presentation, 4 maximum, 5 custom */
      FIELD_BS (shade_plot_customdpi, 78); /*!< 100-32767 */
      FIELD_HANDLE (shade_plot_id, 0, 333); // optional
    }
DWG_OBJECT_END

// unstable
// See AcDbAssocActionBody.h and AcDbAssocDimDependencyBody.h
DWG_OBJECT (ASSOCALIGNEDDIMACTIONBODY)

  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbAssocActionBody)
  FIELD_BL (aab_status, 90); //1 or ownerid
  SUBCLASS (AcDbAssocParamBasedActionBody)
  FIELD_BL (pab_status, 90);
  FIELD_BL (pab_l2, 90);
  FIELD_BL (pab_l3, 90);
  //FIELD_HANDLE (writedep, 0, 360);
  FIELD_BL (pab_l4, 90);
  FIELD_BL (pab_l5, 90);
  //FIELD_BL (pab_l6, 90);
  SUBCLASS (ACDBASSOCALIGNEDDIMACTIONBODY)
  FIELD_BL (dcm_status, 90); //has d_node or r_node

  //TODO: DXF has a different order
  START_OBJECT_HANDLE_STREAM;
  VERSION (R_2013) {
    FIELD_HANDLE (readdep, 4, 330);
    FIELD_HANDLE (writedep, 3, 360);
    FIELD_HANDLE (r_node, 4, 330);
    FIELD_HANDLE (d_node, 4, 330);
  } else {
    FIELD_HANDLE (writedep, 3, 360);
    FIELD_HANDLE (readdep, 4, 330);
    FIELD_HANDLE (d_node, 3, 330);
    FIELD_HANDLE (r_node, 4, 330);
  }
DWG_OBJECT_END

/* In work area:
   The following entities/objects are only stored as raw UNKNOWN_ENT/OBJ,
   unless enabled via --enable-debug/-DDEBUG_CLASSES */

#if defined (DEBUG_CLASSES) || defined (IS_FREE)

// (varies) DEBUGGING
// See AcDbAssocActionBody.h and ASSOCPLANESURFACEACTIONBODY
DWG_OBJECT (ASSOCEXTRUDEDSURFACEACTIONBODY)
  DECODE_UNKNOWN_BITS
  AcDbAssocPathBasedSurfaceActionBody_fields;
  SUBCLASS (AcDbAssocExtrudedSurfaceActionBody)
  FIELD_BL (esab_status, 90);

  START_OBJECT_HANDLE_STREAM;
  HANDLE_VECTOR (writedeps, num_deps, 0, 360);
  HANDLE_VECTOR (readdeps, num_deps, 0, 360);
  FIELD_VECTOR_T (descriptions, T, num_deps, 1);
DWG_OBJECT_END

// See AcDbAssocActionBody.h and ASSOCPLANESURFACEACTIONBODY
DWG_OBJECT (ASSOCLOFTEDSURFACEACTIONBODY)
  DECODE_UNKNOWN_BITS
  AcDbAssocPathBasedSurfaceActionBody_fields;
  SUBCLASS (AcDbAssocLoftedSurfaceActionBody)
  FIELD_BL (lsab_status, 90);

  START_OBJECT_HANDLE_STREAM;
  HANDLE_VECTOR (writedeps, num_deps, 0, 360);
  HANDLE_VECTOR (readdeps, num_deps, 0, 360);
  FIELD_VECTOR_T (descriptions, T, num_deps, 1);
DWG_OBJECT_END

// See AcDbAssocActionBody.h and ASSOCPLANESURFACEACTIONBODY
DWG_OBJECT (ASSOCREVOLVEDSURFACEACTIONBODY)
  DECODE_UNKNOWN_BITS
  AcDbAssocPathBasedSurfaceActionBody_fields;
  SUBCLASS (AcDbAssocRevolvedSurfaceActionBody)
  FIELD_BL (rsab_status, 90);

  START_OBJECT_HANDLE_STREAM;
  HANDLE_VECTOR (writedeps, num_deps, 0, 360);
  HANDLE_VECTOR (readdeps, num_deps, 0, 360);
  FIELD_VECTOR_T (descriptions, T, num_deps, 1);
DWG_OBJECT_END

// See AcDbAssocActionBody.h and ASSOCPLANESURFACEACTIONBODY
DWG_OBJECT (ASSOCSWEPTSURFACEACTIONBODY)
  DECODE_UNKNOWN_BITS
  AcDbAssocPathBasedSurfaceActionBody_fields;
  SUBCLASS (AcDbAssocSweptSurfaceActionBody)
  FIELD_BL (ssab_status, 90);

  START_OBJECT_HANDLE_STREAM;
  HANDLE_VECTOR (writedeps, num_deps, 0, 360);
  HANDLE_VECTOR (readdeps, num_deps, 0, 360);
  FIELD_VECTOR_T (descriptions, T, num_deps, 1);
DWG_OBJECT_END

// DEBUGGING
DWG_OBJECT (EVALUATION_GRAPH)

  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbEvalGraph)
  FIELD_BL (has_graph, 96);        // 1
  FIELD_BL (unknown1, 97);         // 1
  FIELD_BL (unknown2, 0);          // 1
  FIELD_BL (nodeid, 91);           // 0
  if (FIELD_VALUE (has_graph))
    {
      FIELD_BL (edge_flags, 93);   // 32
      FIELD_BL (num_evalexpr, 95); // 1
      // maybe REPEAT num_evalexpr: edge1-4, evalexpr
      FIELD_BL (node_edge1, 92);   // -1
      FIELD_BL (node_edge2, 92);   // -1
      FIELD_BL (node_edge3, 92);   // -1
      FIELD_BL (node_edge4, 92);   // -1
      VALUEOUTOFBOUNDS (num_evalexpr, 20)
    }

  START_OBJECT_HANDLE_STREAM;
  HANDLE_VECTOR (evalexpr, num_evalexpr, 5, 360);
DWG_OBJECT_END

#undef ASSOCACTION_fields
#define ASSOCACTION_fields   \
  SUBCLASS (AcDbAssocAction) \
  /* 0 WellDefined, 1 UnderConstrained, 2 OverConstrained, \
     3 Inconsistent, 4 NotEvaluated, 5 NotAvailable,       \
     6 RejectedByClient */                                 \
  FIELD_BL (solution_status, 90);                          \
  FIELD_BL (geometry_status, 90); /* 0 */                  \
  FIELD_HANDLE (readdep, 5, 330);                          \
  FIELD_HANDLE (writedep, 5, 360);                         \
  FIELD_BL (constraint_status, 90); /* 1 */                             \
  FIELD_BL (dof, 90);               /* 2 remaining degree of freedom */ \
  FIELD_B (is_body_a_proxy, 90)     /* 0 */

// subclass of AcDbAssocAction DEBUGGING
// Object1 --ReadDep--> Action1 --WriteDep1--> Object2 --ReadDep--> Action2 ...
DWG_OBJECT (ASSOCNETWORK)
  DECODE_UNKNOWN_BITS
  ASSOCACTION_fields;

  SUBCLASS (AcDbAssocNetwork)
  FIELD_BL (unknown_n1, 90);
  FIELD_BL (unknown_n2, 90);
  FIELD_BL (num_actions, 90);
  VALUEOUTOFBOUNDS (num_actions, 100)

  START_OBJECT_HANDLE_STREAM;
  HANDLE_VECTOR (actions, num_actions, 5, 330);
DWG_OBJECT_END

// (varies) working on DEBUGGING
DWG_OBJECT (MATERIAL)
  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbMaterial)
  FIELD_T (name, 1);
  FIELD_T (description, 2);
#ifdef IS_DXF
  FIELD_BS (normalmap_projection, 73);
  FIELD_BS (specularmap_projection, 78);
  FIELD_BS (reflectionmap_projection, 172);
  FIELD_BS (opacitymap_projection, 176);
  FIELD_BS (bumpmap_projection, 270);
  FIELD_BS (refractionmap_projection, 274);
  return 0;
#endif

  FIELD_BS (ambient_color_flag, 70); // 0 Use current color, 1 Override
  FIELD_BD (ambient_color_factor, 40); // 0.0 - 1.0
  FIELD_CMC (ambient_color, 90,0);

  DEBUG_HERE_OBJ; // TODO from here on the order of the fields is unknown
  FIELD_BS (diffuse_color_flag, 71); // 0 Use current color, 1 Override
  FIELD_BD (diffuse_color_factor, 41); // 0.0 - 1.0
  FIELD_CMC (diffuse_color, 91,0);
  FIELD_BS (diffusemap_source, 72);  // 0 scene, 1 file (def), 2 procedural
  FIELD_T  (diffusemap_filename, 3); // if NULL no diffuse map
  FIELD_BD (diffusemap_blendfactor, 42); // 1.0
  FIELD_BS (diffusemap_projection, 73); // 0 Inherit, 1 Planar (def), 2 Box, 3 Cylinder, 4 Sphere
  FIELD_BS (diffusemap_tiling, 74);     // 0 Inherit, 1 Tile (def), 2 Crop, 3 Clamp, 4 Mirror
  FIELD_BS (diffusemap_autotransform, 75); // 1 no, 2: scale to curr ent,
                                           // 4: w/ current block transform
  DEBUG_HERE_OBJ;
  FIELD_VECTOR_N (diffusemap_transmatrix, BD, 16, 43);

  FIELD_BD (specular_gloss_factor, 44); //def: 0.5
  FIELD_BS (specular_color_flag, 76); // 0 Use current color, 1 Override
  FIELD_BD (specular_color_factor, 45); // 0.0 - 1.0
  FIELD_CMC (specular_color, 92,0);

  FIELD_BS (specularmap_source, 77); // 0 current, 1 image file (default)
  FIELD_T  (specularmap_filename, 4); // if NULL no specular map
  FIELD_BD (specularmap_blendfactor, 46); // 1.0
  FIELD_BS (specularmap_projection, 78);  // 0 Inherit, 1 Planar (def), 2 Box, 3 Cylinder, 4 Sphere
  FIELD_BS (specularmap_tiling, 79); // 0 Inherit, 1 Tile (def), 2 Crop, 3 Clamp, 4 Mirror
  FIELD_BS (specularmap_autotransform, 170); // 1 no, 2: scale to curr ent,
                                        // 4: w/ current block transform
  FIELD_VECTOR_N (specularmap_transmatrix, BD, 16, 47);

  FIELD_BS (reflectionmap_source, 171); // 0 current, 1 image file (default)
  FIELD_T  (reflectionmap_filename, 6); // if NULL no diffuse map
  FIELD_BD (reflectionmap_blendfactor, 48); // 1.0
  FIELD_BS (reflectionmap_projection, 172); // 0 Inherit, 1 Planar (def), 2 Box, 3 Cylinder, 4 Sphere
  FIELD_BS (reflectionmap_tiling, 173); // 0 Inherit, 1 Tile (def), 2 Crop, 3 Clamp, 4 Mirror
  FIELD_BS (reflectionmap_autotransform, 174); // 1 no, 2: scale to curr ent,
                                        // 4: w/ current block transform
  DEBUG_HERE_OBJ;
  FIELD_VECTOR_N (reflectionmap_transmatrix, BD, 16, 49);

  FIELD_BD (opacity_percent, 140); //def: 1.0
  FIELD_BS (opacitymap_source, 175); // 0 current, 1 image file (default)
  FIELD_T  (opacitymap_filename, 7); // if NULL no specular map
  FIELD_BD (opacitymap_blendfactor, 141); // 1.0
  FIELD_BS (opacitymap_projection, 176); // 0 Inherit, 1 Planar (def), 2 Box, 3 Cylinder, 4 Sphere
  FIELD_BS (opacitymap_tiling, 177); // 0 Inherit, 1 Tile (def), 2 Crop, 3 Clamp, 4 Mirror
  FIELD_BS (opacitymap_autotransform, 178); // 1 no, 2: scale to curr ent,
                                        // 4: w/ current block transform
  DEBUG_HERE_OBJ;
  FIELD_VECTOR_N (opacitymap_transmatrix, BD, 16, 142);

  FIELD_BS (bumpmap_source, 179); // 0 current, 1 image file (default)
  FIELD_T  (bumpmap_filename, 8); // if NULL no specular map
  FIELD_BD (bumpmap_blendfactor, 143); // 1.0
  FIELD_BS (bumpmap_projection, 270); // 0 Inherit, 1 Planar (def), 2 Box, 3 Cylinder, 4 Sphere
  FIELD_BS (bumpmap_tiling, 271); // 0 Inherit, 1 Tile (def), 2 Crop, 3 Clamp, 4 Mirror
  FIELD_BS (bumpmap_autotransform, 272); // 1 no, 2: scale to curr ent,
                                        // 4: w/ current block transform
  DEBUG_HERE_OBJ;
  FIELD_VECTOR_N (bumpmap_transmatrix, BD, 16, 144);

  FIELD_BD (refraction_index, 145); //def: 1.0
  FIELD_BS (refractionmap_source, 273); // 0 current, 1 image file (default)
  FIELD_T  (refractionmap_filename, 9); // if NULL no specular map
  FIELD_BD (refractionmap_blendfactor, 146); // 1.0
  FIELD_BS (refractionmap_projection, 274); // 0 Inherit, 1 Planar (def), 2 Box, 3 Cylinder, 4 Sphere
  FIELD_BS (refractionmap_tiling, 275); // 0 Inherit, 1 Tile (def), 2 Crop, 3 Clamp, 4 Mirror
  FIELD_BS (refractionmap_autotransform, 276); // 1 no, 2: scale to curr ent,
                                        // 4: w/ current block transform
  FIELD_VECTOR_N (refractionmap_transmatrix, BD, 16, 147);
  FIELD_BD (color_bleed_scale, 460);
  FIELD_BD (indirect_dump_scale, 461);
  FIELD_BD (reflectance_scale, 462);
  FIELD_BD (transmittance_scale, 463);
  FIELD_B (two_sided_material, 290);
  DEBUG_HERE_OBJ;
  FIELD_BD (luminance, 464);
  FIELD_BS (luminance_mode, 270);
  FIELD_BS (normalmap_method, 271);
  FIELD_BD (normalmap_strength, 465); //def: 1.0
  FIELD_BS (normalmap_source, 72); // 0 current, 1 image file (default)
  FIELD_T  (normalmap_filename, 3); // if NULL no specular map
  FIELD_BD (normalmap_blendfactor, 42); // 1.0
  FIELD_BS (normalmap_projection, 73); // 0 Inherit, 1 Planar (def), 2 Box, 3 Cylinder, 4 Sphere
  FIELD_BS (normalmap_tiling, 74); // 0 Inherit, 1 Tile (def), 2 Crop, 3 Clamp, 4 Mirror
  FIELD_BS (normalmap_autotransform, 75); // 0 inherit, 1 no, 2 object (scale to curr ent),
                                          // 4 model (w/ current block transform)
  DEBUG_HERE_OBJ;
  FIELD_VECTOR_N (normalmap_transmatrix, BD, 16, 43);
  FIELD_B (materials_anonymous, 293);
  FIELD_BS (global_illumination_mode, 272); // 0 none, 1 cast, 2 receive, 3 cast&receive
  FIELD_BS (final_gather_mode, 273);  // 0 none, 1 cast, 2 receive, 3 cast&receive
  FIELD_T (genprocname, 300);
  FIELD_B (genprocvalbool, 291);
  FIELD_BS (genprocvalint, 271);
  FIELD_BD (genprocvalreal, 469);
  FIELD_T (genprocvaltext, 301);
  FIELD_B (genproctableend, 292);
  FIELD_CMC (genprocvalcolorindex, 62,420);
  FIELD_BL (genprocvalcolorrgb, 420); //int32
  FIELD_T (genprocvalcolorname, 430);
  FIELD_BS (map_utile, 270);
  FIELD_BD (translucence, 148);
  FIELD_BL (self_illumination, 90);
  FIELD_BD (reflectivity, 468);
  FIELD_BL (illumination_model, 93);
  DEBUG_HERE_OBJ;
  FIELD_BL (channel_flags, 94);
  //78
  //172
  //176
  //270
  //274
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// (varies) DEBUGGING, UNKNOWN FIELDS
// hard-owned child of AcDbViewportTableRecord or AcDbViewport 361
// DXF docs put that as Entity, wrong!
DWG_OBJECT (SUN)
  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbSun)
  FIELD_BL (class_version, 90); //1
  VALUEOUTOFBOUNDS (class_version, 10)
  FIELD_B (is_on, 290); // status, isOn
  FIELD_B (has_shadow, 291); // shadow on/off
  FIELD_B (is_dst, 292);  // isDayLightSavingsOn
  FIELD_BS (unknown, 421); //16777215
  DEBUG_HERE_OBJ
  //27 111111111100001000000000 011 [32,58]
  bit_advance_position (dat, 24);
  FIELD_BD (intensity, 40); //01
  FIELD_B (has_shadow, 291); //1
  FIELD_BL (julian_day, 91); // same as TIMEBLL
  FIELD_BL (time, 92);    // in seconds past midnight
  FIELD_B (is_dst, 292);  // isDayLightSavingsOn
  //
  DEBUG_HERE_OBJ //128
  //22 0 010000001011000000010 | 14 0 1011000000010
  bit_advance_position (dat, 21);

  //FIELD_BD (altitude, 0); //calculated? 10
  //FIELD_BD (azimuth, 0);  //calculated? 10
  //FIELD_3BD (direction, 0); //calculated? 101001
  if (FIELD_VALUE (has_shadow))
  {
    FIELD_BS (shadow_type, 70); // 0 raytraced, 1 shadow maps
    if (FIELD_VALUE (shadow_type)>2) {
      LOG_ERROR ("Invalid SUN.shadow_type %d", (int)FIELD_VALUE (shadow_type));
      return DWG_ERR_VALUEOUTOFBOUNDS;
    }
    FIELD_BS (shadow_softness, 280); //1 [94-103]
    FIELD_BS (shadow_mapsize, 71); //256 usually
  }
  //there's still 5.4 - 11.3 bits free for some fields
  //421:16777215

  DEBUG_HERE_OBJ //DEBUG_POS_OBJ
  rcount1 = bit_position (dat);
  rcount2 = rcount1 - obj->address * 8; // cur offset
#if 0
  FIELD_VALUE (num_bytes) = (obj->bitsize - rcount2) / 8;
  FIELD_VALUE (num_bits)  = (obj->bitsize - rcount2) % 8;
  LOG_TRACE ("num_bytes: %d, num_bits: %d\n", FIELD_VALUE (num_bytes), FIELD_VALUE (num_bits));
  if (obj->bitsize > rcount2) {
    FIELD_TF (bytes, FIELD_VALUE (num_bytes), 0);
    FIELD_VECTOR (bits, B, num_bits, 0);
  }
#endif
  bit_set_position (dat, rcount1 + 60);

#if 0
  //find handle stream
  for (vcount=bit_position (dat);
       dat->byte < obj->address+obj->size;
       bit_set_position (dat,++vcount))
    {
      DEBUG_POS_OBJ
      // @9979.6 5.0.0, @9980.0 4.0.0, @9991.1 3.0.0
      // search for a valid code=4 handle followed by a valid code=3
      FIELD_HANDLE (ownerhandle, 4, 0);
      if (_obj->ownerhandle &&
          _obj->ownerhandle->handleref.code == 4 &&
          _obj->ownerhandle->absolute_ref < dwg->num_object_refs)
        {
          //reactors also 4. could check num_reactors
          FIELD_HANDLE (xdicobjhandle, 3, 0);
          if (_obj->xdicobjhandle &&
              _obj->xdicobjhandle->handleref.code == 3 &&
              _obj->xdicobjhandle->absolute_ref < dwg->num_object_refs)
            {
              bit_set_position (dat, vcount);
              break;
            }
        }
    }
#endif

  FIELD_CMC (color, 63,421);

  START_OBJECT_HANDLE_STREAM;
  FIELD_HANDLE (skyparams, 5, 0); //AcGiSkyParameters class?
  DEBUG_POS_OBJ //@9992.1

DWG_OBJECT_END

// (varies) DEBUGGING, UNKNOWN FIELDS
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
      SUB_FIELD_BL (dates[rcount1], time, 90);
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
  FIELD_BS (type, 0); // POINT, LATLONG, MYLOCATION
  FIELD_3BD (position, 10);
  //FIELD_3BD (lat_lon_alt, 0); // the same?
  FIELD_3BD (extrusion, 210);
  FIELD_BD (radius, 40);
  FIELD_BD (landing_gap, 0);
  FIELD_T (text, 1);
  FIELD_BS (text_alignment, 0); // 0 left, 1 center, 2 right
  FIELD_B (mtext_visible, 0);
  //FIELD_B (mtext_is_vertical, 0);
  //FIELD_BD (mtext_actual_width, 0);
  //FIELD_BD (mtext_actual_height, 0);
  FIELD_B (enable_frame_text, 0);
  FIELD_T (notes, 0);

  COMMON_ENTITY_HANDLE_DATA;
  //FIELD_HANDLE (leader_handle, 5, 0); //or drawn automatically?
  FIELD_HANDLE (mtext_handle, 5, 0);
  FIELD_HANDLE (text_style, 5, 7);

DWG_ENTITY_END

// r2007+
DWG_ENTITY (EXTRUDEDSURFACE)

  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbModelerGeometry)
  ACTION_3DSOLID;
  //FIELD_BS (modeler_format_version, 70); //def 1
  SUBCLASS (AcDbSurface)
  FIELD_BS (u_isolines, 71);
  FIELD_BS (v_isolines, 72);
  SUBCLASS (AcDbExtrudedSurface)
  FIELD_BL (class_version, 90);
  VALUEOUTOFBOUNDS (class_version, 10)
  FIELD_3BD (sweep_vector, 10);
  FIELD_VECTOR_N (sweep_transmatrix, BD, 16, 40);
  FIELD_BD (draft_angle, 42);
  FIELD_BD (draft_start_distance, 43);
  FIELD_BD (draft_end_distance, 44);
  FIELD_BD (twist_angle, 45);
  FIELD_BD (scale_factor, 48);
  FIELD_BD (align_angle, 49);
  FIELD_VECTOR_N (sweep_entity_transmatrix, BD, 16, 46);
  FIELD_VECTOR_N (path_entity_transmatrix, BD, 16, 47);
  FIELD_B (solid, 290);
  FIELD_BS (sweep_alignment_flags, 290); //0=No alignment; 1=Align sweep entity to path
                  // 2=Translate sweep entity to path; 3=Translate path to sweep entity
  FIELD_B (align_start, 292);
  FIELD_B (bank, 293);
  FIELD_B (base_point_set, 294);
  FIELD_B (sweep_entity_transform_computed, 295);
  FIELD_B (path_entity_transform_computed, 296);
  FIELD_3BD (reference_vector_for_controlling_twist, 11);

  COMMON_ENTITY_HANDLE_DATA;
  FIELD_HANDLE (sweep_entity, 5, 0);
  FIELD_HANDLE (path_entity, 5, 0);

DWG_ENTITY_END

// r2007+
DWG_ENTITY (LOFTEDSURFACE)

  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbModelerGeometry)
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
  SUBCLASS (AcDbModelerGeometry)
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
  SUBCLASS (AcDbModelerGeometry)
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
  FIELD_VECTOR_N (sweep_entity_transmatrix, BD, 16, 40);
  FIELD_VECTOR_N (path_entity_transmatrix, BD, 16, 41);
  FIELD_BD (draft_angle, 42);
  FIELD_BD (draft_start_distance, 43);
  FIELD_BD (draft_end_distance, 44);
  FIELD_BD (twist_angle, 45);
  FIELD_VECTOR_N (sweep_entity_transmatrix1, BD, 16, 46);
  FIELD_VECTOR_N (path_entity_transmatrix1, BD, 16, 47);
  FIELD_BD (scale_factor, 48);
  FIELD_BD (align_angle, 49);
  FIELD_B (solid, 290);
  FIELD_RC (sweep_alignment, 70);
  FIELD_B (align_start, 292);
  FIELD_B (bank, 293);
  FIELD_B (base_point_set, 294);
  FIELD_B (sweep_entity_transform_computed, 295);
  FIELD_B (path_entity_transform_computed, 296);
  FIELD_3BD (reference_vector_for_controlling_twist, 11);

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

DWG_ENTITY (PLANESURFACE)

  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbModelerGeometry)
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

// (varies) DEBUGGING
DWG_OBJECT (ASSOCACTION)
  DECODE_UNKNOWN_BITS
  ASSOCACTION_fields;

  rcount1 = bit_position (dat);
  DEBUG_HERE_OBJ
  //17bit 00101000101000101:
  FIELD_T (body.evaluatorid, 0);
  FIELD_T (body.expression, 0);
  FIELD_BL (body.value, 0); //rbuf really
  //FIELD_B (is_actionevaluation_in_progress, 90);
  DEBUG_POS_OBJ
  bit_set_position (dat, rcount1 + 27);
  FIELD_BL (status, 90); //27-36
  if (FIELD_VALUE (status) > 0x100) {
    LOG_ERROR ("Invalid ASSOCACTION.status " FORMAT_BL, FIELD_VALUE (status));
    _obj->status = 0;
    return DWG_ERR_VALUEOUTOFBOUNDS;
  }
  DEBUG_HERE_OBJ
  FIELD_HANDLE (actionbody, 5, 0);
  FIELD_HANDLE (callback, 3, 0);
  FIELD_HANDLE (owningnetwork, 3, 0);
  FIELD_BL (num_deps, 90);
  VALUEOUTOFBOUNDS (num_deps, 5000)
  //HANDLE_VECTOR (readdeps, num_deps, 5, 330);
  //HANDLE_VECTOR (writedeps, num_deps, 0, 360);
  //FIELD_BL (unknown_assoc, 90);

  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// DEBUGGING
DWG_OBJECT (ASSOCOSNAPPOINTREFACTIONPARAM)
  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbAssocActionParam)
  FIELD_B  (unknown1, 0); //
  FIELD_RC (unknown, 0); //01010101
  FIELD_T (name, 1); //@9-10
  DEBUG_HERE_OBJ
  FIELD_B  (unknown1, 0); //
  DEBUG_HERE_OBJ
  FIELD_B  (unknown1, 0); //
  DEBUG_HERE_OBJ
  FIELD_BS (status, 90); //0
  FIELD_B  (unknown1, 0); //
  DEBUG_HERE_OBJ
  //DEBUG_HERE_OBJ
  SUBCLASS (AcDbAssocCompoundActionParam)
  FIELD_BD (unknown3, 40); //-1 32-97
  FIELD_BS (flags, 90); //0 read/write deps
  //...
  DEBUG_HERE_OBJ
  FIELD_BS (num_actions, 90); //1
  VALUEOUTOFBOUNDS (num_actions, 1000)
  DEBUG_HERE_OBJ

  bit_advance_position (dat, 122-118);
  START_OBJECT_HANDLE_STREAM;
  DEBUG_POS_OBJ
  FIELD_HANDLE (writedep, ANYCODE, 360); //122-129
  bit_advance_position (dat, 168-130);
  DEBUG_POS_OBJ
  HANDLE_VECTOR (actions, num_actions, 4, 330);
DWG_OBJECT_END

//??
DWG_OBJECT (ASSOCVERTEXACTIONPARAM)
  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbAssocActionParam)
  FIELD_B  (unknown1, 0); //
  FIELD_RC (unknown, 0); //01010101
  FIELD_T (name, 1); //@9-10
  DEBUG_HERE_OBJ
  FIELD_B  (unknown1, 0); //
  DEBUG_HERE_OBJ
  FIELD_B  (unknown1, 0); //
  DEBUG_HERE_OBJ
  FIELD_BS (status, 90); //0
  FIELD_B  (unknown1, 0); //
  DEBUG_HERE_OBJ
DWG_OBJECT_END

// See AcDbAssocPersSubentIdPE.h?
DWG_OBJECT (ASSOCPERSSUBENTMANAGER)
  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbAssocPersSubentManager)
  FIELD_BL (class_version, 90); //1
  FIELD_BL (unknown_bl1, 90); //3
  FIELD_BL (unknown_bl2, 90); //0
  FIELD_BL (unknown_bl3, 90); //2
  FIELD_BL (unknown_bl4, 90); //3
  FIELD_BL (unknown_bl5, 90); //1
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

// Class AcDbAssoc2dConstraintGroup
// see https://help.autodesk.com/view/OARX/2018/ENU/?guid=OREF-AcDbAssoc2dConstraintGroup
DWG_OBJECT (ASSOC2DCONSTRAINTGROUP)
  DECODE_UNKNOWN_BITS
  ASSOCACTION_fields;

  SUBCLASS (AcDbAssocNetwork)
  FIELD_BL (l5, 90); //2
  FIELD_B (b1, 70);  //0
  FIELD_3BD (workplane[0], 10); // 0,0,0
  FIELD_3BD (workplane[1], 10); // 1,0,0
  FIELD_3BD (workplane[2], 10); // 0,1,0
  FIELD_HANDLE (h1, 0, 360);
  FIELD_BL (num_actions, 90); //2
  HANDLE_VECTOR (actions, num_actions, 0, 360);
  FIELD_BL (l7, 90); //9
  FIELD_BL (l8, 90); //9

  FIELD_T (t1, 1);
  //DXF { FIELD_TV ("AcConstrainedCircle", 1); }
  FIELD_HANDLE (h2, 0, 330);
  FIELD_BL (cl1, 90); //1
  FIELD_RC (cs1, 70); //1
  FIELD_BL (cl2, 90); //1
  FIELD_BL (cl3, 90); //3
  FIELD_HANDLE (h3, 0, 330);
  FIELD_BL (cl4, 90); //0
  FIELD_3BD (c1, 10); // @133
  FIELD_3BD (c2, 10);
  FIELD_3BD (c3, 10);
  FIELD_BD (w1, 40); // @279
  FIELD_BD (w2, 40);
  FIELD_BD (w3, 40);

  FIELD_T (t2, 1);
  //DXF { FIELD_TV ("AcConstrainedImplicitPoint", 1); }
  // ...
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// Class AcDbSweepOptions? DEBUGGING
// dbSweepOptions.h dbsurf.h
DWG_OBJECT (ACSH_SWEEP_CLASS)

  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbEvalExpr)
  FIELD_BL (class_version, 90); //1
  FIELD_BL (ee_bl98, 98); //33 major/minor?
  FIELD_BL (ee_bl99, 99); //29
  SUBCLASS (AcDbShHistoryNode)
  FIELD_BL (shhn_bl90, 90); //33
  FIELD_BL (shhn_bl91, 91); //29
  FIELD_VECTOR_N1 (shhn_pts, BD, 16, 40); //40-55
  FIELD_CMC (color, 62,421); //256
  FIELD_B  (shhn_b92, 92);     //1
  FIELD_BL (shhn_bl347, 347);   //269

  SUBCLASS (AcDbShPrimitive)
  SUBCLASS (AcDbShSweepBase)
  FIELD_BL (shsw_bl90, 90); //33
  FIELD_BL (shsw_bl91, 91); //29
  FIELD_3BD (basept, 10); //0,0,0
  FIELD_BL (shsw_bl92, 92); //77
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

DWG_OBJECT (ACSH_BOX_CLASS)

  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbEvalExpr)
  FIELD_BL (class_version, 90); //1
  FIELD_BL (ee_bl98, 98); //33 major/minor?
  FIELD_BL (ee_bl99, 99); //29
  SUBCLASS (AcDbShHistoryNode)
  FIELD_BL (shhn_bl90, 90); //33
  FIELD_BL (shhn_bl91, 91); //29
  FIELD_VECTOR_N1 (shhn_pts, BD, 16, 40); //40-55
  FIELD_CMC (color, 62,421); //256
  FIELD_B  (shhn_b92, 92);     //0
  FIELD_BL (shhn_bl347, 347);   //11

  SUBCLASS (AcDbShPrimitive)
  SUBCLASS (AcDbShBox)
  FIELD_BL (shb_bl90, 90); //33
  FIELD_BL (shb_bl91, 91); //29
  FIELD_BD (shb_bd40, 40); //len?    1298.99.. (pos: 514)
  FIELD_BD (shb_bd41, 41); //width?  20.0..
  FIELD_BD (shb_bd42, 42); //height? 420.0
  START_OBJECT_HANDLE_STREAM;

DWG_OBJECT_END

DWG_OBJECT (ACSH_EXTRUSION_CLASS)

  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbEvalExpr)
  FIELD_BL (class_version, 90); //1
  FIELD_BL (ee_bl98, 98); //33 major/minor?
  FIELD_BL (ee_bl99, 99); //29
  SUBCLASS (AcDbShHistoryNode)
  FIELD_BL (shhn_bl90, 90); //33
  FIELD_BL (shhn_bl91, 91); //29
  FIELD_VECTOR_N1 (shhn_pts, BD, 16, 40); //40-55
  FIELD_CMC (color, 62,421); //256
  FIELD_B  (shhn_b92, 92);     //0
  FIELD_BL (shhn_bl347, 347);   //11

  SUBCLASS (AcDbShPrimitive)
  SUBCLASS (AcDbShSweepBase)
  FIELD_BL (shsw_bl90, 90); //33
  FIELD_BL (shsw_bl91, 91); //29
  FIELD_3BD (basept, 10); //0,0,0
  FIELD_BL (shsw_bl92, 92); //77
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

DWG_OBJECT (ACSH_HISTORY_CLASS)
  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbEvalExpr)
  FIELD_BL (class_version, 90); //1
  FIELD_BL (ee_bl98, 98); //33 major/minor?
  FIELD_BL (ee_bl99, 99); //29
  SUBCLASS (AcDbShHistoryNode)
  FIELD_BL (shhn_bl90, 90); //33
  FIELD_BL (shhn_bl91, 91); //29
  FIELD_VECTOR_N1 (shhn_pts, BD, 16, 40); //40-55
  FIELD_CMC (color, 62,421); //256
  FIELD_B  (shhn_b92, 92);     //0
  FIELD_BL (shhn_bl347, 347);   //11
  //??
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// (varies) TODO
// no coverage. Stored in ACAD_BIM_DEFINITIONS dictionary
DWG_OBJECT (NAVISWORKSMODELDEF)

  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbNavisworksModelDef)
  DEBUG_HERE_OBJ
  FIELD_T (path, 1);
  FIELD_B (status, 290);
  FIELD_3BD (min_extent, 10);
  FIELD_3BD (max_extent, 11);
  FIELD_B (host_drawing_visibility, 290);

  START_OBJECT_HANDLE_STREAM;

DWG_OBJECT_END

// officially documented, dbRender.h
DWG_OBJECT (RENDERENVIRONMENT)
  DECODE_UNKNOWN_BITS

  SUBCLASS (AcDbRenderEnvironment)
  DEBUG_HERE_OBJ
  FIELD_BL (class_version, 90);     /*!< default 1 */
  FIELD_B (fog_enabled, 290);
  FIELD_B (fog_background_enabled, 290);
  FIELD_CMC (fog_color, 0,280);
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
  DECODE_UNKNOWN_BITS

  SUBCLASS (AcDbRenderGlobal)
  DEBUG_HERE_OBJ
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

DWG_ENTITY (ARC_DIMENSION)
  DECODE_UNKNOWN_BITS

  SUBCLASS (AcDbDimension)
  COMMON_ENTITY_DIMENSION
  SUBCLASS (AcDbArcDimension)
  FIELD_3BD (def_pt, 10);
  FIELD_3BD (_13_pt, 13);
  FIELD_3BD (_14_pt, 14);
  FIELD_3BD (_15_pt, 15);
  FIELD_3BD (unknown_pt, 0); // clone_ins_pt?
  FIELD_3BD (_16_pt, 16);
  FIELD_3BD (_17_pt, 17);
  //FIELD_B (unknown_b, 0);
  DEBUG_HERE_OBJ
  FIELD_BD (leader_len, 40);
  FIELD_BD (ins_scale.x, 41);
  FIELD_RC (flag2, 71);

  COMMON_ENTITY_HANDLE_DATA;
DWG_ENTITY_END

// varies: UNKNOWN FIELDS
// types: Sphere|Cylinder|Cone|Torus|Box|Wedge|Pyramid
DWG_ENTITY (MESH)
  DECODE_UNKNOWN_BITS
  FIELD_BL (class_version, 90);
  VALUEOUTOFBOUNDS (class_version, 10)
  SUBCLASS (AcDbSubDMesh)
  FIELD_RC (dlevel, 71); // 2
  FIELD_RC (is_watertight, 72); // 0

  FIELD_BL (num_subdiv_vertex, 91); //?
  VALUEOUTOFBOUNDS (num_subdiv_vertex, 5000)
  FIELD_3DPOINT_VECTOR (subdiv_vertex, num_subdiv_vertex, 10);

  FIELD_BL (num_vertex, 92);
  VALUEOUTOFBOUNDS (num_vertex, 5000)
  FIELD_3DPOINT_VECTOR (vertex, num_vertex, 10);
  FIELD_BL (num_faces, 93);
  VALUEOUTOFBOUNDS (num_faces, 5000)
  FIELD_VECTOR (faces, BL, num_faces, 90);
  FIELD_BL (num_edges, 94);
  VALUEOUTOFBOUNDS (num_edges, 5000)

  REPEAT (num_edges, edges, Dwg_MESH_edge)
  REPEAT_BLOCK
      SUB_FIELD_BL (edges[rcount1], from, 90);
      SUB_FIELD_BL (edges[rcount1], to, 90);
  END_REPEAT_BLOCK
  SET_PARENT_OBJ (edges);
  END_REPEAT (edges);
  //FIELD_VECTOR (edges, Dwg_MESH_edge, num_edges, 90);
  FIELD_BL (num_crease, 95);
  VALUEOUTOFBOUNDS (num_crease, 5000)
  FIELD_VECTOR (crease, BD, num_crease, 140);
  /* 90 ?
     ? BD crease
  */
  COMMON_ENTITY_HANDLE_DATA;
DWG_ENTITY_END

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
DWG_OBJECT_END

DWG_OBJECT (ALDIMOBJECTCONTEXTDATA)
  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbObjectContextData)
  FIELD_BS (class_version, 70); // 4
  FIELD_B (has_file, 290);
  //FIELD_B (defaultflag, 0);
  SUBCLASS (AcDbAnnotScaleObjectContextData)
  FIELD_HANDLE (scale, 2, 340);
  SUBCLASS (AcDbDimensionObjectContextData)
  FIELD_T (name, 2);
  FIELD_2RD (def_pt, 10);
  //HOLE([140,159],11000000000000000000) len = 20
   FIELD_B (b293, 293); // 0
   FIELD_B (b294, 294); // 1
   FIELD_BD (bd140, 140); // 0.0
   FIELD_B (b298, 298); // 0
   FIELD_B (b291, 291); // 0
   FIELD_BS (flag, 70); // 0
   FIELD_B (b292, 292); // 0
   FIELD_BS (bs71, 71); // 0
   FIELD_B (b280, 280); // 0
   FIELD_B (b295, 295); // 0
   FIELD_B (b296, 296); // 0
   FIELD_B (b297, 297); // 0

  SUBCLASS (AcDbAlignedDimensionObjectContextData)
  FIELD_3BD (_11pt, 11);

  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (BLKREFOBJECTCONTEXTDATA)
  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbObjectContextData)
  FIELD_BS (class_version, 70);
  VALUEOUTOFBOUNDS (class_version, 10)
  FIELD_B (has_file, 290);
  FIELD_B (defaultflag, 0);
  SUBCLASS (AcDbAnnotScaleObjectContextData)
  FIELD_HANDLE (scale, 2, 340); /* to SCALE */
  // ?? ...
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (LEADEROBJECTCONTEXTDATA)
  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbObjectContextData)
  FIELD_BS (class_version, 70);
  VALUEOUTOFBOUNDS (class_version, 10)
  FIELD_B (has_file, 290);
  FIELD_B (defaultflag, 0);
  SUBCLASS (AcDbAnnotScaleObjectContextData)
  FIELD_BL (num_points, 70); /* 3 */
  FIELD_3DPOINT_VECTOR (points, num_points, 10);
  FIELD_3DPOINT (x_direction, 11);
  FIELD_B (b290, 290); /* 1 */
  FIELD_3DPOINT (inspt_offset, 12);
  FIELD_3DPOINT (endptproj, 13);

  START_OBJECT_HANDLE_STREAM;
  FIELD_HANDLE (scale, 2, 340); /* to SCALE */
DWG_OBJECT_END

DWG_OBJECT (MLEADEROBJECTCONTEXTDATA)
  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbObjectContextData)
  FIELD_BS (class_version, 70);
  VALUEOUTOFBOUNDS (class_version, 10)
  FIELD_B (has_file, 290);
  FIELD_B (defaultflag, 0);
  SUBCLASS (AcDbAnnotScaleObjectContextData)
  FIELD_HANDLE (scale, 2, 340); /* to SCALE */
  // ?? ...
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (TEXTOBJECTCONTEXTDATA)
  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbObjectContextData)
  FIELD_BS (class_version, 70); // 4
  FIELD_B (has_file, 290);
  //FIELD_B (defaultflag, 0);
  SUBCLASS (AcDbAnnotScaleObjectContextData)
  FIELD_HANDLE (scale, 2, 340);
  FIELD_BS (flag, 70); // 0
  FIELD_BD (rotation, 50); // 0.0 or 90.0
  FIELD_2RD (insertion_pt, 10);
  FIELD_2RD (alignment_pt, 11);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (MTEXTATTRIBUTEOBJECTCONTEXTDATA)
  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbObjectContextData)
  FIELD_BS (class_version, 70);
  VALUEOUTOFBOUNDS (class_version, 10)
  FIELD_B (has_file, 290);
  FIELD_B (defaultflag, 0);
  SUBCLASS (AcDbAnnotScaleObjectContextData)
  FIELD_HANDLE (scale, 2, 340); /* to SCALE */
  // ?? ...
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (MTEXTOBJECTCONTEXTDATA)
  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbObjectContextData)
  FIELD_BS (class_version, 70); // 4
  FIELD_B (has_file, 290);
  //FIELD_B (defaultflag, 0);
  SUBCLASS (AcDbAnnotScaleObjectContextData)
  FIELD_HANDLE (scale, 2, 340);
  FIELD_BS (flag, 70); // 6
  FIELD_3RD (insertion_pt, 11);
  FIELD_3RD (x_axis_dir, 10);
  FIELD_BD (text_height, 40);
  FIELD_BD (rect_width, 41);
  FIELD_BD (extents_width, 42);
  FIELD_BD (extents_height, 43);
  FIELD_BS (attachment, 71); // or column_type?
  if (FIELD_VALUE (attachment))
    {
      FIELD_BS (drawing_dir, 72);
      FIELD_BS (linespace_style, 73);
      FIELD_BD (linespace_factor, 44);
      FIELD_BD (bd45, 45);
      FIELD_BS (bs74, 74);
      FIELD_BD (rect_height, 46);
    }
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (LIGHTLIST)
  DECODE_UNKNOWN_BITS
DWG_OBJECT_END

// (varies) TODO
DWG_OBJECT (DATATABLE)
  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbDataTable)
  DEBUG_HERE_OBJ
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (DATALINK)
  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbDataLink)
  FIELD_BS (class_version, 0); // 10
  FIELD_T (appname, 1);
  FIELD_T (description, 300);
  FIELD_T (link, 301);
  FIELD_T (cell, 302);
  //FIELD_BS (bs90, 90); // 2
  FIELD_BS (bs91, 91); // 1179649
  FIELD_BS (bs92, 92); // 1
  FIELD_BS (year, 170);
  FIELD_BS (month, 171);
  FIELD_BS (day, 172);
  FIELD_BS (hour, 173);
  FIELD_BS (minute, 174);
  FIELD_BS (seconds, 175);
  FIELD_BS (bs176, 176); // 0
  FIELD_BS (bs171, 177); // 1
  FIELD_BS (bs93, 93); // 0
  FIELD_T (t304, 304);
  FIELD_BS (num_deps, 94); // 2
  HANDLE_VECTOR (deps, num_deps, 0, 330);
  FIELD_HANDLE (writedep, 0, 360);
  FIELD_T (t305, 305); // CUSTOMDATA
  DEBUG_HERE_OBJ
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (DETAILVIEWSTYLE)
  DECODE_UNKNOWN_BITS

  SUBCLASS (AcDbModelDocViewStyle)
  FIELD_BS (class_version, 70); // 0
  FIELD_T (name, 3);
  FIELD_B (is_modified_for_recompute, 290);

  SUBCLASS (AcDbDetailViewStyle)
  FIELD_BS (identifier_placement, 70);
  FIELD_BS (model_edge, 90); // type, origin, direction
  SINCE (R_2007) {
    FIELD_B (show_arrowheads, 280);
  }
  // DXF: 90 280 71 340 62 40 340 62 40 340 42 40 300 40 280 71 340 90 62 71 340 62 40 90 40 90 300 71 340 90 62 340 90 62 280
  // pi: 90 (r2007+: 280) 71 62 62 62 62 62 40 40 ?? 40 90 ?? 40 300 90 40 71 90 90 ?? 90 ??
  FIELD_BS (connection_line_weight, 71);
  DEBUG_HERE_OBJ
  FIELD_CMC (connection_line_color, 62,420);
  FIELD_CMC (identifier_color, 62,420);
  FIELD_CMC (arrow_symbol_color, 62,420);
  FIELD_CMC (boundary_line_color, 62,420);
  FIELD_CMC (viewlabel_text_color, 62,420);

  FIELD_BD (identifier_height, 40);
  FIELD_BD (identifier_offset, 42);
  FIELD_HANDLE (identifier_style, 5, 340); // textstyle
  FIELD_HANDLE (arrow_symbol, 5, 340); // NULL
  FIELD_BD (arrow_symbol_size, 40);
  FIELD_BS (boundary_line_weight, 71);
  FIELD_HANDLE (boundary_line_type, 5, 340); // ltype
  FIELD_HANDLE (viewlabel_text_style, 5, 340); // textstyle
  FIELD_HANDLE (connection_line_type, 5, 340); // ltype
  FIELD_BD (viewlabel_text_height, 40);
  FIELD_T (viewlabel_field, 300);
  FIELD_BD (viewlabel_offset, 42);
  FIELD_BS (viewlabel_attachment, 70);
  FIELD_BS (viewlabel_alignment, 72);
  FIELD_BS (viewlabel_pattern, 0);
  FIELD_B (show_viewlabel, 280);
  FIELD_BD (borderline_weight, 71);
  FIELD_CMC (borderline_color, 62,420);
  FIELD_HANDLE (borderline_type, 5, 340);

  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// subclass: VIEWSTYLE_ModelDoc => "AcDbModelDocViewStyle"
DWG_OBJECT (SECTIONVIEWSTYLE)
  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbModelDocViewStyle)
  FIELD_BS (class_version, 70); // 0
  FIELD_T (name, 3);
  FIELD_B (is_modified_for_recompute, 290);
  FIELD_T (description, 300);
  FIELD_BL (identifier_exclude_characters, 90); // 102

  SUBCLASS (AcDbSectionViewStyle)
  // DXF: 100 70 3 290 3 90 100 70 71 90 71 340 62 40 340 340 62 40 300 40 90 40 90 71 340 90 62 340 90 62 40 40 40 71 340 62 40 90 40 90 300(field) 71 62 62 300 40 90 290 290 90 6 40 40 40 40 40
  //pi: 70 3 290
  FIELD_CMC (identifier_color, 62,420);
  FIELD_BD (identifier_height, 40); // 5.0
  FIELD_CMC (plane_line_color, 62,420);
  FIELD_BD (identifier_offset, 42); // 5.0
  FIELD_T (viewlabel_text, 300); // I, O, Q, S, X, Z
  // 40 90 62 40 40 62 40 62 40 300 71
  FIELD_BD (arrow_symbol_size, 40);
  FIELD_BS (viewlabel_pattern, 90);
  FIELD_CMC (arrow_symbol_color, 62,420);
  FIELD_BL (hatch_pattern, 90);
  FIELD_CMC (bend_line_color, 62,420);
  FIELD_BD (identifier_position, 40);
  FIELD_BD (viewlabel_text_height, 40);
  FIELD_CMC (viewlabel_text_color, 62,420);
  FIELD_BD (bend_line_length, 40);
  DEBUG_HERE_OBJ
  FIELD_CMC (hatch_color, 62,420);
  FIELD_CMC (hatch_bg_color, 62,420);
  FIELD_HANDLE (identifier_style, 5, 340); // textstyle
  FIELD_HANDLE (arrow_start_symbol, 5, 340);
  FIELD_HANDLE (arrow_end_symbol, 5, 340);
  FIELD_BD (arrow_symbol_extension_length, 40);
  FIELD_BS (plane_line_weight, 71);
  FIELD_HANDLE (plane_line_type, 5, 340); // ltype
  FIELD_HANDLE (bend_line_type, 5, 340); // ltype
  FIELD_HANDLE (viewlabel_text_style, 5, 340); // textstyle
  FIELD_BS (bend_line_weight, 0);
  FIELD_BD (end_line_length, 0);
  FIELD_BD (arrow_position, 40);
  FIELD_T (viewlabel_field, 300);
  FIELD_BD (viewlabel_offset, 42);
  FIELD_BS (viewlabel_attachment, 70);
  FIELD_BS (viewlabel_alignment, 72);
  FIELD_BD (hatch_scale, 0);
  FIELD_BD (hatch_angles, 0);
  FIELD_BS (hatch_transparency, 0);
  FIELD_B (is_continuous_labeling, 290);
  FIELD_B (show_arrowheads, 290);
  FIELD_B (show_viewlabel, 290);
  FIELD_B (show_all_plane_lines, 290);
  FIELD_B (show_all_bend_indentifiers, 290);
  FIELD_B (show_end_and_bend_lines, 290);

  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// as ACAD_LAYERFILTERS in the DNO
DWG_OBJECT (LAYERFILTER)
  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbLayerFilter)
  DEBUG_HERE_OBJ
  FIELD_T (name, 2);
  FIELD_T (description, 3);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (LAYOUTPRINTCONFIG)
  DECODE_UNKNOWN_BITS
  SUBCLASS (CAcLayoutPrintConfig)
  FIELD_BS (class_version, 0);
  DEBUG_HERE_OBJ
  FIELD_BS (flag, 93);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

// see unknown 34/117=29.1%
// possible: [......29    7 7 xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx9 99   7  9........5...9 99 9.9 9.........5...9    9..9 99    9....]
// 90 -10000 at offset 16/117
DWG_OBJECT (ASSOCGEOMDEPENDENCY)
  DECODE_UNKNOWN_BITS
  //90 2 DependentOnObjectStatus (ok, NotInitializedYet, InvalidObjectId)
  //90 -10000
  //330 -> CIRCLE
  SUBCLASS (AcDbAssocDependency)
  FIELD_BS (class_version, 90);
  FIELD_BS (dependent_on_object_status, 90);
  FIELD_B (has_cached_value, 290);
  FIELD_B (is_actionevaluation_in_progress, 290);
  FIELD_B (is_attached_to_object, 290);
  FIELD_B (is_delegating_to_owning_action, 290);
  FIELD_BS (bs90_2, 90);
  FIELD_B (b290_5, 290);
  FIELD_BS (bs90_3, 90);
  SUBCLASS (AcDbAssocGeomDependency)
  FIELD_BS (bs90_4, 90);
  FIELD_B (b290_6, 290);
  FIELD_T (t, 1);
  FIELD_B (dependent_on_compound_object, 290);

  START_OBJECT_HANDLE_STREAM;
  FIELD_HANDLE (h330_1, 4, 330);
  FIELD_HANDLE (h330_2, 4, 330);
  FIELD_HANDLE (h330_3, 4, 330);
  FIELD_HANDLE (h360, 3, 360);
DWG_OBJECT_END

// AutoCAD Mechanical
DWG_OBJECT (ACMESCOPE)
  DECODE_UNKNOWN_BITS
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (ACMECOMMANDHISTORY)
  DECODE_UNKNOWN_BITS
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (ACMESTATEMGR)
  DECODE_UNKNOWN_BITS
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (DOCUMENTOPTIONS)
  DECODE_UNKNOWN_BITS
  //size 161
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (RAPIDRTRENDERSETTINGS)
  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbRenderSettings)
  FIELD_BS (class_version, 90);
  FIELD_T (name, 1);
  FIELD_B (fog_enabled, 290);
  FIELD_B (fog_background_enabled, 290);
  FIELD_B (b290_1, 290);
  //FIELD_CMC (fog_color, 280);
  //FIELD_BD (fog_distance_near, 40);
  //FIELD_BD (fog_distance_far, 40);
  FIELD_B (environ_image_enabled, 290);
  FIELD_T (environ_image_filename, 1);
  FIELD_T (description, 1);
  FIELD_BS (bs90, 90);
  SUBCLASS (AcDbRapidRTRenderSettings)
  FIELD_BS (bs90_0, 90);
  FIELD_RC (rc70_1, 70);
  FIELD_BS (bs90_1, 90);
  FIELD_BS (bs90_2, 90);
  FIELD_RC (rc70_2, 70);
  FIELD_RC (rc70_3, 70);
  FIELD_BD (fog_density_near, 40);
  FIELD_BD (fog_density_far, 40);
  FIELD_B (b290_2, 290);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

DWG_OBJECT (MENTALRAYRENDERSETTINGS)
  DECODE_UNKNOWN_BITS
  SUBCLASS (AcDbRenderSettings)
  FIELD_BS (class_version, 90);
  FIELD_T (name, 1);
  FIELD_B (fog_enabled, 290);
  FIELD_B (fog_background_enabled, 290);
  FIELD_B (b290_1, 290);
  //FIELD_CMC (fog_color, 280);
  FIELD_B (environ_image_enabled, 290);
  FIELD_T (environ_image_filename, 1);
  FIELD_T (description, 1);
  FIELD_BS (bs90, 90);
  SUBCLASS (AcDbMentalRayRenderSettings)
  FIELD_BS (bs90_0, 90);
  FIELD_RC (rc70_1, 70);
  FIELD_BS (bs90_1, 90);
  FIELD_BS (bs90_2, 90);
  FIELD_BD (fog_density_near, 40);
  FIELD_BD (fog_density_far, 40);
  FIELD_BD (samples_distance_near, 40);
  FIELD_BD (samples_distance_far, 40);
  FIELD_BD (fog_distance_near, 40);
  FIELD_BD (fog_distance_far, 40);
  FIELD_RC (rc70_2, 70);
  FIELD_B (b290_3, 290);
  FIELD_B (b290_4, 290);
  FIELD_BS (bs90_3, 90);
  FIELD_BS (bs90_4, 90);
  FIELD_BS (bs90_5, 90);
  FIELD_B (b290_5, 290);
  FIELD_BS (bs90_6, 90);
  FIELD_B (b290_6, 290);
  FIELD_BD (bd40_2, 40);
  FIELD_BS (bs90_7, 90);
  FIELD_BS (bs90_8, 90);
  FIELD_BS (bs90_9, 90);
  FIELD_BS (bs90_10, 90);
  FIELD_B (b290_7, 290);
  FIELD_BS (bs90_11, 90);
  FIELD_B (b290_8, 290);
  FIELD_B (b290_9, 290);
  FIELD_B (b290_10, 290);
  FIELD_BD (bd40_3, 40);
  FIELD_BD (bd40_4, 40);
  FIELD_BD (bd40_5, 40);
  FIELD_RC (rc70_3, 70);
  FIELD_RC (rc70_4, 70);
  FIELD_BD (bd40_6, 40);
  FIELD_RC (rc70_5, 70);
  FIELD_RC (rc70_6, 70);
  FIELD_B (b290_11, 290);
  FIELD_T (mrdescription, 1);
  FIELD_BS (bs90_12, 90);
  FIELD_RC (rc70_7, 70);
  FIELD_BS (bs90_13, 90);
  FIELD_B (b290_12, 290);
  FIELD_BD (bd40_7, 40);
  START_OBJECT_HANDLE_STREAM;
DWG_OBJECT_END

#endif /* DEBUG_CLASSES */

/* Those undocumented objects are also stored as raw UNKNOWN_OBJ */

#if 0

/* Missing DXF names:
  ACDBPOINTCLOUDEX  ARRAY
  ATTBLOCKREF ATTDYNBLOCKREF BLOCKREF DYNBLOCKREF XREF
  CENTERMARK CENTERLINE
  SECTIONOBJECT
*/

// r2000+ expresstools. abbrev. ATEXT
DWG_OBJECT (ARCALIGNEDTEXT)
  DECODE_UNKNOWN_BITS
DWG_OBJECT_END

// EXACXREFPANELOBJECT
DWG_OBJECT (XREFPANELOBJECT)
  DECODE_UNKNOWN_BITS
DWG_OBJECT_END

DWG_OBJECT (NPOCOLLECTION)
  DECODE_UNKNOWN_BITS
DWG_OBJECT_END

// TODO POINTCLOUDEX
DWG_OBJECT (POINTCLOUD)
  DECODE_UNKNOWN_BITS
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

// r2000+ expresstools
DWG_ENTITY (RTEXT)
  DECODE_UNKNOWN_BITS
  COMMON_ENTITY_HANDLE_DATA;
DWG_ENTITY_END

DWG_OBJECT (ACDSRECORD)
  DECODE_UNKNOWN_BITS
DWG_OBJECT_END

DWG_OBJECT (ACDSSCHEMA)
  DECODE_UNKNOWN_BITS
DWG_OBJECT_END

DWG_OBJECT (ACSH_REVOLVE_CLASS)
  DECODE_UNKNOWN_BITS
DWG_OBJECT_END

DWG_OBJECT (ACSH_SPHERE_CLASS)
  DECODE_UNKNOWN_BITS
DWG_OBJECT_END

DWG_OBJECT (RAPIDRTRENDERENVIRONMENT)
  DECODE_UNKNOWN_BITS
DWG_OBJECT_END

#endif
