/* -*- c -*- */
/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2009, 2010 Free Software Foundation, Inc.                  */
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
 */

#include "spec.h"

/* (1/7) */
DWG_ENTITY (TEXT)

  PRE(R_13) {
    FIELD_2RD (insertion_pt, 10);
    FIELD_RD (height, 40);
    FIELD_TV (text_value, 1);
    if (R11OPTS(1))
      FIELD_RD (rotation, 50);
    if (R11OPTS(2))
      FIELD_RD (width_factor, 41);
    if (R11OPTS(4))
      FIELD_RD (oblique_ang, 51);
    if (R11OPTS(8)) {
      DECODER { _ent->linetype_r11 = bit_read_RC(dat); }
      ENCODER { bit_write_RC(dat, _ent->linetype_r11); }
      PRINT   { LOG_TRACE("linetype_rs: " FORMAT_RS "\n", _ent->linetype_r11); }
    }
    if (R11OPTS(16))
      FIELD_CAST (generation, RC, BS, 71);
    if (R11OPTS(32))
      FIELD_CAST (horiz_alignment, RC, BS, 72);
    if (R11OPTS(64))
      FIELD_2RD (alignment_pt, 11);
    if (R11OPTS(256))
      FIELD_CAST (vert_alignment, RC, BS, 73);
  }
  VERSIONS(R_13, R_14)
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

  SINCE(R_2000)
    {
      /* We assume that the user (the client application)
         is responsible for taking care of properly updating the dataflags field
         which indicates which fields in the data structures are valid and which are
         undefined */
      BITCODE_RC dataflags;
      FIELD_RC (dataflags, 0);
      dataflags = FIELD_VALUE(dataflags);

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

  VERSIONS(R_13, R_2007)
    {
      IF_ENCODE_FROM_PRE_R13 {
        //FIXME: should really just lookup the style table; style is the index.
        FIELD_VALUE(style) = 0; //dwg_resolve_handle(obj->parent, obj->linetype_rs);
      }
      FIELD_HANDLE (style, 5, 7);
    }

DWG_ENTITY_END

/* (2/16) */
DWG_ENTITY(ATTRIB)

  PRE(R_13)
    {
      LOG_ERROR("TODO ATTRIB")
    }
  VERSIONS(R_13, R_14)
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

  SINCE(R_2000)
    {
      /* We assume that the user (the client application)
         is responsible for taking care of properly updating the dataflags field
         which indicates which fields in the data structures are valid and which are
         undefined */
      BITCODE_RC dataflags;
      FIELD_RC (dataflags, 0);
      dataflags = FIELD_VALUE(dataflags);

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

  SINCE(R_2010)
    {
      FIELD_RC (class_version, 280); // 0 = r2010
    }
  SINCE(R_2018)
    {
      FIELD_RC (type, 70); // 1=single line, 2=multi line attrib, 4=multi line attdef

      if (FIELD_VALUE(type) > 1)
        {
          LOG_WARN("MTEXT fields")
          // TODO fields handles to MTEXT entities. how many?
          FIELD_HANDLE (mtext_handles, 0, 340); //TODO

          FIELD_BS (annotative_data_size, 70);
          if (FIELD_VALUE(annotative_data_size) > 1)
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

  SINCE(R_2007) {
    FIELD_B (lock_position_flag, 70);
  }

  COMMON_ENTITY_HANDLE_DATA;

  SINCE(R_2007) {
    START_STRING_STREAM
    FIELD_TU (text_value, 1);
    FIELD_TU (tag, 2);
    END_STRING_STREAM
  }
  FIELD_HANDLE (style, 5, 7);

DWG_ENTITY_END

/* (3/15) */
DWG_ENTITY(ATTDEF)

  PRE(R_13)
    {
      LOG_ERROR("TODO ATTDEF")
    }
  VERSIONS(R_13, R_14)
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
      FIELD_BS (vert_alignment, 73);
    }

  SINCE(R_2000)
    {
      /* We assume that the user (the client application)
         is responsible for taking care of properly updating the dataflags field
         which indicates which fields in the data structures are valid and which are
         undefined */
      BITCODE_RC dataflags;
      FIELD_RC (dataflags, 0);
      dataflags = FIELD_VALUE(dataflags);

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

  FIELD_T (tag, 2);
  FIELD_BS (field_length, 73);
  FIELD_RC (flags, 70); // 1 invisible, 2 constant, 4 verify, 8 preset

  SINCE(R_2007) {
    FIELD_B (lock_position_flag, 70);
  }
  SINCE(R_2010) {
    FIELD_RC (class_version, 280);
  }
  FIELD_T (prompt, 3);

  COMMON_ENTITY_HANDLE_DATA;

  //FIELD_HANDLE (style, 5, 7);

DWG_ENTITY_END

/* (4/12) */
DWG_ENTITY(BLOCK)

  FIELD_T (name, 2);

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/* (5/13) */
DWG_ENTITY(ENDBLK)

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/* (6) */
DWG_ENTITY(SEQEND)

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/* (7/14) */
DWG_ENTITY(INSERT)

  FIELD_3DPOINT (ins_pt, 10);

  VERSIONS(R_13, R_14)
    {
      FIELD_3BD_1 (scale, 41); // 42,43
    }

  SINCE(R_2000)
    {
      DECODER
        {
          FIELD_BB (scale_flag, 0);
          if (FIELD_VALUE(scale_flag) == 3)
            {
              FIELD_VALUE(scale.x) = 1.0;
              FIELD_VALUE(scale.y) = 1.0;
              FIELD_VALUE(scale.z) = 1.0;
            }
          else if (FIELD_VALUE(scale_flag) == 1)
            {
              FIELD_VALUE(scale.x) = 1.0;
              FIELD_DD (scale.y, 1.0, 42);
              FIELD_DD (scale.z, 1.0, 43);
            }
          else if (FIELD_VALUE(scale_flag) == 2)
            {
              FIELD_RD (scale.x, 41); 
              FIELD_VALUE(scale.y) = FIELD_VALUE (scale.x);
              FIELD_VALUE(scale.z) = FIELD_VALUE (scale.x);
            }
          else //if (FIELD_VALUE(scale_flag) == 0)
            {
              FIELD_RD (scale.x, 41);
              FIELD_DD (scale.y, FIELD_VALUE (scale.x), 42);
              FIELD_DD (scale.z, FIELD_VALUE (scale.x), 43);
            }
        }

      ENCODER
        {
          if (FIELD_VALUE(scale.x)==1.0 && FIELD_VALUE(scale.y)==1.0 && FIELD_VALUE(scale.z)==1.0)
            {
              FIELD_VALUE(scale_flag) = 3;
              FIELD_BB (scale_flag, 0);
            }
          else if (FIELD_VALUE(scale.x)==FIELD_VALUE(scale.y) && FIELD_VALUE(scale.x)==FIELD_VALUE(scale.z))
            {
              FIELD_VALUE(scale_flag) = 2;
              FIELD_BB (scale_flag, 0);
              FIELD_RD (scale.x, 41);
            }
          else if (FIELD_VALUE(scale.x)==1.0)
            {
              FIELD_VALUE(scale_flag) = 1;
              FIELD_BB (scale_flag, 0);
              FIELD_RD (scale.x, 41);
              FIELD_DD (scale.y, 1.0, 42);
              FIELD_DD (scale.z, 1.0, 43);
            }
          else
            {
              FIELD_VALUE(scale_flag) = 0;
              FIELD_BB (scale_flag, 0);
              FIELD_RD (scale.x, 41);
              FIELD_DD (scale.y, FIELD_VALUE (scale.x), 42);
              FIELD_DD (scale.z, FIELD_VALUE (scale.x), 43);
            }
        }
    }

  FIELD_BD (rotation, 50);
  FIELD_3DPOINT (extrusion, 210);
  FIELD_B (has_attribs, 66);

  SINCE(R_2004)
    {
      if (FIELD_VALUE(has_attribs))
        {
          FIELD_BL (owned_obj_count, 0);
        }
    }

  COMMON_ENTITY_HANDLE_DATA;

  FIELD_HANDLE (block_header, 5, 2);

  //There is a typo in the spec. it says "R13-R200:".
  //I guess it means "R13-R2000:" (just like in MINSERT)
  VERSIONS(R_13, R_2000)
    {
      if (FIELD_VALUE(has_attribs))
        {
          FIELD_HANDLE (first_attrib, 4, 0);
          FIELD_HANDLE (last_attrib, 4, 0);
        }
    }

  //Spec typo? Spec says "2004:" but I think it should be "2004+:"
  // just like field owned_obj_count (AND just like in MINSERT)
  SINCE(R_2004)
    {
      if (FIELD_VALUE(has_attribs))
        {
          HANDLE_VECTOR(attrib_handles, owned_obj_count, 4, 0);
        }
    }

  if (FIELD_VALUE(has_attribs))
    FIELD_HANDLE (seqend, 3, 0);

DWG_ENTITY_END

/* (8) 20.4.10*/
DWG_ENTITY(MINSERT)

  FIELD_3BD (ins_pt, 10);

  VERSIONS(R_13, R_14) {
    FIELD_3BD_1 (scale, 41);
  }

  SINCE(R_2000)
    {
      DECODER
        {
          FIELD_BB (scale_flag, 0);
          if (FIELD_VALUE(scale_flag) == 3)
            {
              FIELD_VALUE(scale.x) = 1.0;
              FIELD_VALUE(scale.y) = 1.0;
              FIELD_VALUE(scale.z) = 1.0;
            }
          else if (FIELD_VALUE(scale_flag) == 1)
            {
              FIELD_VALUE(scale.x) = 1.0;
              FIELD_DD (scale.y, 1.0, 42);
              FIELD_DD (scale.z, 1.0, 43);
            }
          else if (FIELD_VALUE(scale_flag) == 2)
            {
              FIELD_RD (scale.x, 41);
              FIELD_VALUE(scale.y) = FIELD_VALUE(scale.x);
              FIELD_VALUE(scale.z) = FIELD_VALUE(scale.x);
            }
          else
            {
              assert(FIELD_VALUE (scale_flag) == 0);
              FIELD_RD (scale.x, 41);
              FIELD_DD (scale.y, FIELD_VALUE(scale.x), 42);
              FIELD_DD (scale.z, FIELD_VALUE(scale.x), 43);
            }
        }

      ENCODER
        {
          if (FIELD_VALUE(scale.x) == 1.0 &&
              FIELD_VALUE(scale.y) == 1.0 &&
              FIELD_VALUE(scale.z) == 1.0)
            {
              FIELD_VALUE(scale_flag) = 3;
              FIELD_BB (scale_flag, 0);
            }
          else if (FIELD_VALUE(scale.x) == 1.0)
             {
              FIELD_VALUE(scale_flag) = 1;
              FIELD_BB (scale_flag, 0);
              FIELD_DD (scale.y, 1.0, 42);
              FIELD_DD (scale.z, 1.0, 43);
             }
          else if (FIELD_VALUE(scale.x) == FIELD_VALUE(scale.y) &&
                   FIELD_VALUE(scale.x) == FIELD_VALUE(scale.z))
            {
              FIELD_VALUE(scale_flag) = 2;
              FIELD_BB (scale_flag, 0);
              FIELD_RD (scale.x, 41);
            }
          else
            {
              FIELD_VALUE(scale_flag) = 0;
              FIELD_BB (scale_flag, 0);
              FIELD_RD (scale.x, 41);
              FIELD_DD (scale.y, FIELD_VALUE(scale.x), 42);
              FIELD_DD (scale.z, FIELD_VALUE(scale.x), 43);
            }
        }
    }

  FIELD_BD (rotation, 50);
  FIELD_3BD (extrusion, 210);
  FIELD_B (has_attribs, 66);

  SINCE(R_2004)
    {
      if (FIELD_VALUE(has_attribs))
        FIELD_BL (owned_obj_count, 0);
    }

  FIELD_BS (numcols, 70);
  FIELD_BS (numrows, 71);
  FIELD_BD (col_spacing, 44);
  FIELD_BD (row_spacing, 45);

  COMMON_ENTITY_HANDLE_DATA;

  FIELD_HANDLE (block_header, 5, 2);

  VERSIONS(R_13, R_2000)
  {
    if (FIELD_VALUE(has_attribs))
      {
        FIELD_HANDLE (first_attrib, 4, 0);
        FIELD_HANDLE (last_attrib, 4, 0);
      }
  }

  SINCE(R_2004)
    {
      HANDLE_VECTOR(attrib_handles, owned_obj_count, 4, 0);
    }

  if (FIELD_VALUE(has_attribs))
    FIELD_HANDLE (seqend, 3, 0);

DWG_ENTITY_END

//(9) Unknown

/* (10/20) */
DWG_ENTITY(VERTEX_2D)

  PRE(R_13)
  {
    FIELD_2RD (point, 10);
    if (R11OPTS(1))
      FIELD_RD (start_width, 40);
    if (R11OPTS(2))
      FIELD_RD (end_width, 41);
    if (R11OPTS(4))
      FIELD_RD (tangent_dir, 50);
    if (R11OPTS(8))
      FIELD_RC (flag, 70);
  }
  SINCE(R_13)
  {
    FIELD_RC (flag, 70);
    FIELD_3BD (point, 10);

  /* Decoder and Encoder routines could be the same but then we
     wouldn't compress data when saving. So we explicitely implemented
     the encoder routine with the compression technique described in
     the spec. --Juca */
  DECODER
    {
      FIELD_BD (start_width, 40);

      if (FIELD_VALUE(start_width) < 0)
        {
          FIELD_VALUE(start_width) = -FIELD_VALUE (start_width);
          FIELD_VALUE(end_width) = FIELD_VALUE (start_width);
        }
      else
        {
          FIELD_BD (end_width, 41);
        }
    }

  ENCODER
    {
      if (FIELD_VALUE(start_width) && FIELD_VALUE(start_width)==FIELD_VALUE(end_width))
        {
          //TODO: This is ugly! We should have a better way of doing such things
          FIELD_VALUE(start_width) = -FIELD_VALUE (start_width);
          FIELD_BD (start_width, 40);
          FIELD_VALUE(start_width) = -FIELD_VALUE (start_width);
        }
      else
        {
          FIELD_BD (start_width, 40);
          FIELD_BD (end_width, 41);
        }
    }

    FIELD_BD (bulge, 42);
    SINCE(R_2010) {
      FIELD_BL (id, 91);
    }
    FIELD_BD (tangent_dir, 50);
  }

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/*(11)*/
DWG_ENTITY(VERTEX_3D)

  FIELD_RC (flag, 70);
  FIELD_3BD (point, 10);

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/*(12)*/
DWG_ENTITY(VERTEX_MESH)

  FIELD_RC (flag, 70);
  FIELD_3BD (point, 10);

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/*(13)*/
DWG_ENTITY(VERTEX_PFACE)

  FIELD_RC (flag, 70);
  FIELD_3BD (point, 10);

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/*(14)*/
DWG_ENTITY(VERTEX_PFACE_FACE)

  FIELD_BS (vertind[0], 71);
  FIELD_BS (vertind[1], 72);
  FIELD_BS (vertind[2], 73);
  FIELD_BS (vertind[3], 74);
  //TODO R13 has color_rs and linetype_rs for all vertices, not in DXF

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/*(15)*/
DWG_ENTITY(POLYLINE_2D)

  PRE(R_13)
  {
    if (R11OPTS(1))
      FIELD_CAST (flag, RC, RS, 70);
    if (R11OPTS(2))
      FIELD_RD (start_width, 40);
    //??
    if (R11OPTS(4))
      FIELD_RS (curve_type, 75);
    if (R11OPTS(8))
      FIELD_RD (end_width, 40);
  }
  SINCE (R_13)
  {
    FIELD_BS (flag, 70);
    FIELD_BS (curve_type, 75);
    FIELD_BD (start_width, 40);
    FIELD_BD (end_width, 41);
    FIELD_BT (thickness, 39);
    FIELD_BD (elevation, 30);
    FIELD_BE (extrusion, 210);

    SINCE(R_2004) {
      FIELD_BL (owned_obj_count, 0);
    }
  }
  COMMON_ENTITY_HANDLE_DATA;

  VERSIONS(R_13, R_2000)
    {
      FIELD_HANDLE (first_vertex, 4, 0);
      FIELD_HANDLE (last_vertex, 4, 0);
    }

  SINCE(R_2004)
    {
      HANDLE_VECTOR(vertex, owned_obj_count, 3, 0);
    }

  SINCE (R_13)
  {
    FIELD_HANDLE (seqend, 3, 0);
  }

DWG_ENTITY_END

/*(16)*/
DWG_ENTITY(POLYLINE_3D)

  FIELD_RC (flag, 0); // => 70, 75 TODO for dxf
  FIELD_RC (flag2, 0);

  SINCE(R_2004) {
    FIELD_BL (owned_obj_count, 0);
  }

  COMMON_ENTITY_HANDLE_DATA;

  VERSIONS(R_13, R_2000)
    {
      FIELD_HANDLE (first_vertex, 4, 0);
      FIELD_HANDLE (last_vertex, 4, 0);
    }

  SINCE(R_2004)
    {
      HANDLE_VECTOR(vertex, owned_obj_count, 3, 0);
    }

  FIELD_HANDLE (seqend, 3, 0);

DWG_ENTITY_END

/* (17/8) */
DWG_ENTITY(ARC)

  PRE(R_13) {
    FIELD_2RD (center, 10);
    FIELD_RD (radius, 40);
    FIELD_RD (start_angle, 50);
    FIELD_RD (end_angle, 51);
    if (R11OPTS(1))
      FIELD_3RD (extrusion, 210);
    if (R11OPTS(2))
      FIELD_RD (center.z, 30);
  }
  LATER_VERSIONS {
    FIELD_3BD (center, 10);
    FIELD_BD (radius, 40);
    FIELD_BT (thickness, 39);
    FIELD_BE (extrusion, 210);
    FIELD_BD (start_angle, 50);
    FIELD_BD (end_angle, 51);
  }

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/* (18/3) */
DWG_ENTITY(CIRCLE)

  PRE(R_13) {
    FIELD_2RD (center, 10);
    FIELD_RD (radius, 40);
    if (R11OPTS(1))
      FIELD_3RD (extrusion, 210);
    if (R11OPTS(2))
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
DWG_ENTITY(LINE)

  PRE(R_13) {
    if (_ent->flag_r11 & 4)
      FIELD_3RD (start, 10)
    else
      FIELD_2RD (start, 10)

    if (_ent->flag_r11 & 4)
      FIELD_3RD (end, 11)
    else
      FIELD_2RD (end, 11)

    if (R11OPTS(1))
      FIELD_3RD (extrusion, 210);
    if (R11OPTS(2))
      FIELD_RD (thickness, 39);
  }
  VERSIONS(R_13, R_14)
    {
      FIELD_3BD (start, 10);
      FIELD_3BD (end, 11);
    }
  SINCE(R_2000)
    {
      DECODER
        {
          FIELD_B (Zs_are_zero, 0);
          FIELD_RD (start.x, 10);
          FIELD_DD (end.x, FIELD_VALUE(start.x), 11);
          FIELD_RD (start.y, 20);
          FIELD_DD (end.y, FIELD_VALUE(start.y), 21);

          if (FIELD_VALUE(Zs_are_zero))
            {
              FIELD_VALUE(start.z) = 0.0;
              FIELD_VALUE(end.z) = 0.0;
            }
          else
            {
              FIELD_RD (start.z, 30);
              FIELD_DD (end.z, FIELD_VALUE(start.z), 31);
            }
        }

      ENCODER
        {
          FIELD_VALUE(Zs_are_zero) = (FIELD_VALUE(start.z) == 0.0 && FIELD_VALUE (end.z) == 0.0);
          FIELD_B (Zs_are_zero, 0);
          FIELD_RD (start.x, 10);
          FIELD_DD (end.x, FIELD_VALUE(start.x), 11);
          FIELD_RD (start.y, 20);
          FIELD_DD (end.y, FIELD_VALUE(start.y), 21);
          if (!FIELD_VALUE(Zs_are_zero))
            {
              FIELD_RD (start.z, 30);
              FIELD_DD (end.z, FIELD_VALUE(start.z), 31);
            }
        }
    }

  SINCE(R_13) {
    FIELD_BT (thickness, 39);
    FIELD_BE (extrusion, 210);
  }

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/**
 * Macro for common DIMENSION declaration
 */
#define DIMENSION_COMMON_DECODE \
    SINCE(R_2010) \
      { \
        FIELD_RC (class_version, 280); /* 0=r2010 */ \
      } \
    FIELD_3BD (extrusion, 210); \
    FIELD_2RD (text_midpt, 11); \
    FIELD_BD (elevation, 31); \
    FIELD_RC (flags_1, 70); \
    FIELD_T (user_text, 1); \
    FIELD_BD (text_rot, 53); \
    FIELD_BD (horiz_dir, 51); \
    FIELD_3BD (ins_scale, 41); \
    FIELD_BD (ins_rotation, 54); \
    SINCE(R_2000) \
      { \
        FIELD_BS (attachment, 71); \
        FIELD_BS (lspace_style, 72); \
        FIELD_BD (lspace_factor, 41); \
        FIELD_BD (act_measurement, 42); \
      } \
    SINCE(R_2007) \
      { \
        FIELD_B (unknown, 73); \
        FIELD_B (flip_arrow1, 74); \
        FIELD_B (flip_arrow2, 75); \
      } \
    FIELD_2RD (clone_ins_pt, 12);


/*(20)*/
DWG_ENTITY(DIMENSION_ORDINATE)

  DIMENSION_COMMON_DECODE;
  FIELD_3BD (ucsorigin_pt, 10);
  FIELD_3BD (feature_location_pt, 13);
  FIELD_3BD (leader_endpt, 14);
  FIELD_RC (flags_2, 70);

  COMMON_ENTITY_HANDLE_DATA;
  UNTIL(R_2007)
    {
      FIELD_HANDLE (dimstyle, 5, 3);
      FIELD_HANDLE (block, 5, 2);
    }

DWG_ENTITY_END

/* (21/23) */
DWG_ENTITY(DIMENSION_LINEAR)

  // TODO PRE(R_R13)
  DIMENSION_COMMON_DECODE;
  FIELD_3BD (_13_pt, 13);
  FIELD_3BD (_14_pt, 14);
  FIELD_3BD (def_pt, 10);
  FIELD_BD (ext_line_rot, 52);
  FIELD_BD (dim_rot, 50);

  COMMON_ENTITY_HANDLE_DATA;
  UNTIL(R_2007)
    {
      FIELD_HANDLE (dimstyle, 5, 3);
      FIELD_HANDLE (block, 5, 2);
    }

DWG_ENTITY_END

/*(22)*/
DWG_ENTITY(DIMENSION_ALIGNED)

  DIMENSION_COMMON_DECODE;
  FIELD_3BD (_13_pt, 13);
  FIELD_3BD (_14_pt, 14);
  FIELD_3BD (def_pt, 10);
  FIELD_BD (ext_line_rot, 52);

  COMMON_ENTITY_HANDLE_DATA;
  UNTIL(R_2007)
    {
      FIELD_HANDLE (dimstyle, 5, 3);
      FIELD_HANDLE (block, 5, 2);
    }

DWG_ENTITY_END

/*(23)*/
DWG_ENTITY(DIMENSION_ANG3PT)

  DIMENSION_COMMON_DECODE;
  FIELD_3BD (def_pt, 10);
  FIELD_3BD (_13_pt, 13);
  FIELD_3BD (_14_pt, 14);
  FIELD_3BD (first_arc_pt, 15);

  COMMON_ENTITY_HANDLE_DATA;
  UNTIL(R_2007)
    {
      FIELD_HANDLE (dimstyle, 5, 3);
      FIELD_HANDLE (block, 5, 2);
    }

DWG_ENTITY_END

/*(24)*/
DWG_ENTITY(DIMENSION_ANG2LN)

  DIMENSION_COMMON_DECODE;
  FIELD_2RD (_16_pt, 16);
  FIELD_3BD (_13_pt, 13);
  FIELD_3BD (_14_pt, 14);
  FIELD_3BD (first_arc_pt, 15);
  FIELD_3BD (def_pt, 10);

  COMMON_ENTITY_HANDLE_DATA;
  UNTIL(R_2007)
    {
      FIELD_HANDLE (dimstyle, 5, 3);
      FIELD_HANDLE (block, 5, 2);
    }

DWG_ENTITY_END

/*(25)*/
DWG_ENTITY(DIMENSION_RADIUS)

  DIMENSION_COMMON_DECODE;
  FIELD_3BD (def_pt, 10);
  FIELD_3BD (first_arc_pt, 15);
  FIELD_BD (leader_len, 40);

  COMMON_ENTITY_HANDLE_DATA;
  UNTIL(R_2007)
    {
      FIELD_HANDLE (dimstyle, 5, 3);
      FIELD_HANDLE (block, 5, 2);
    }

DWG_ENTITY_END

/*(26)*/
DWG_ENTITY(DIMENSION_DIAMETER)

  DIMENSION_COMMON_DECODE;
  FIELD_3BD (first_arc_pt, 15);
  FIELD_3BD (def_pt, 10);
  FIELD_BD (leader_len, 40);

  COMMON_ENTITY_HANDLE_DATA;
  UNTIL(R_2007)
    {
      FIELD_HANDLE (dimstyle, 5, 3);
      FIELD_HANDLE (block, 5, 2);
    }

DWG_ENTITY_END

/* (27/2) */
DWG_ENTITY(POINT)

  //TODO PRE(R_13)
  FIELD_BD (x, 10);
  FIELD_BD (y, 20);
  FIELD_BD (z, 30);
  FIELD_BT (thickness, 39);
  FIELD_BE (extrusion, 210);
  FIELD_BD (x_ang, 50);

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/* (28/22) */
DWG_ENTITY(_3DFACE)

  // TODO PRE(R_R13)
  VERSIONS(R_13, R_14)
    {
      FIELD_3BD (corner1, 10);
      FIELD_3BD (corner2, 11);
      FIELD_3BD (corner3, 12);
      FIELD_3BD (corner4, 13);
      FIELD_BS (invis_flags, 70);
    }

  SINCE(R_2000)
    {
      FIELD_B (has_no_flags, 0);

      DECODER
        {
          FIELD_B (z_is_zero, 0);
          FIELD_RD (corner1.x, 10);
          FIELD_RD (corner1.y, 20);
          if (FIELD_VALUE(z_is_zero))
              FIELD_VALUE(corner1.z) = 0;
          else
            FIELD_RD (corner1.z, 30);
        }

      ENCODER
        {
          FIELD_VALUE(z_is_zero) = (FIELD_VALUE (corner1.z) == 0);
          FIELD_B (z_is_zero, 0);
          FIELD_RD (corner1.x, 10);
          FIELD_RD (corner1.y, 20);
          if (!FIELD_VALUE(z_is_zero))
            FIELD_RD (corner1.z, 30);
        }

      FIELD_3DD (corner2, corner1, 11);
      FIELD_3DD (corner3, corner2, 11);
      FIELD_3DD (corner4, corner3, 11);
    }

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/*(29)*/
DWG_ENTITY(POLYLINE_PFACE)

  FIELD_BS (numverts, 71);
  FIELD_BS (numfaces, 72);

  SINCE(R_2004) {
    FIELD_BL (owned_obj_count, 0);
  }

  COMMON_ENTITY_HANDLE_DATA;

  VERSIONS(R_13, R_2000)
    {
      FIELD_HANDLE (first_vertex, 4, 0);
      FIELD_HANDLE (last_vertex, 4, 0);
    }
  SINCE(R_2004)
    {
      HANDLE_VECTOR(vertex, owned_obj_count, 4, 0);
    }
  FIELD_HANDLE (seqend, 3, 0);

DWG_ENTITY_END

/*(30)*/
DWG_ENTITY(POLYLINE_MESH)

  FIELD_BS (flag, 70);
  FIELD_BS (curve_type, 75);
  FIELD_BS (m_vert_count, 71);
  FIELD_BS (n_vert_count, 72);
  FIELD_BS (m_density, 73);
  FIELD_BS (n_density, 74);

  SINCE(R_2004) {
    FIELD_BL (owned_obj_count, 0);
  }

  COMMON_ENTITY_HANDLE_DATA;

  VERSIONS(R_13, R_2000)
    {
      FIELD_HANDLE (first_vertex, 4, 0);
      FIELD_HANDLE (last_vertex, 4, 0);
    }
  SINCE(R_2004)
    {
      HANDLE_VECTOR(vertex, owned_obj_count, 4, 0);
    }
  FIELD_HANDLE (seqend, 3, 0);

DWG_ENTITY_END

/* (31/11) */
DWG_ENTITY(SOLID)

  PRE(R_13) {
    FIELD_2RD (corner1, 11);
    FIELD_2RD (corner2, 12);
    FIELD_2RD (corner3, 13);
    FIELD_2RD (corner4, 14);
    if (R11OPTS(1))
      FIELD_3RD (extrusion, 210);
    if (R11OPTS(2))
      FIELD_RD (elevation, 38);
  }
  LATER_VERSIONS {
    FIELD_BT (thickness, 39);
    FIELD_BD (elevation, 38);
    FIELD_2RD (corner1, 11);
    FIELD_2RD (corner2, 12);
    FIELD_2RD (corner3, 13);
    FIELD_2RD (corner4, 14);
    FIELD_BE (extrusion, 210);

    COMMON_ENTITY_HANDLE_DATA;
  }

DWG_ENTITY_END

/* (32/9) */
DWG_ENTITY(TRACE)

  PRE(R_13) {
    FIELD_2RD (corner1, 10);
    FIELD_2RD (corner2, 11);
    FIELD_2RD (corner3, 12);
    FIELD_2RD (corner4, 13);
    if (R11OPTS(1))
      FIELD_3RD (extrusion, 210);
    if (R11OPTS(2))
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
DWG_ENTITY(SHAPE)

  PRE(R_13) {
    FIELD_HANDLE (shapefile, 5, 0);
    FIELD_2RD (ins_pt, 10);
    FIELD_RS (shape_no, 2);
    if (R11OPTS(1))
      FIELD_3RD (extrusion, 210);
    if (R11OPTS(2))
      FIELD_RD (ins_pt.z, 38);
  }
  LATER_VERSIONS {
    FIELD_3BD (ins_pt, 10);
    FIELD_BD (scale, 40);
    FIELD_BD (rotation, 50);
    FIELD_BD (width_factor, 41);
    FIELD_BD (oblique, 51);
    FIELD_BD (thickness, 39);
    FIELD_BS (shape_no, 2);
    FIELD_3BD (extrusion, 210);

    COMMON_ENTITY_HANDLE_DATA;
    FIELD_HANDLE (shapefile, 5, 0);
  }

DWG_ENTITY_END

/* (34/24) */
DWG_ENTITY(VIEWPORT)

  PRE(R_13) {
    FIELD_3RD (center, 10);
    FIELD_RD (width, 40);
    FIELD_RD (height, 41);
    FIELD_RS (unknown, 68);
  }
  LATER_VERSIONS {
    FIELD_3BD (center, 10);
    FIELD_BD (width, 40);
    FIELD_BD (height, 41);
  }

  SINCE(R_2000)
    {
      FIELD_3BD (view_target, 17);
      FIELD_3BD (view_direction, 16);
      FIELD_BD (view_twist_angle, 51);
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

  SINCE(R_2007) {
    FIELD_BS (grid_major, 61);
  }

  SINCE(R_2000)
    {
      FIELD_BL (frozen_layer_count, 0);
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

  SINCE(R_2004) {
    FIELD_BS (shadeplot_mode, 170);
  }

  SINCE(R_2007)
    {
      FIELD_B (use_default_lights, 292);
      FIELD_RC (default_lighting_type, 282);
      FIELD_BD (brightness, 141);
      FIELD_BD (contrast, 142);
      FIELD_CMC (ambient_color, 63);
    }

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/*(35)*/
DWG_ENTITY(ELLIPSE)

  FIELD_3BD (center, 10);
  FIELD_3BD (sm_axis, 11);
  FIELD_3BD (extrusion, 210);
  FIELD_BD (axis_ratio, 40);
  FIELD_BD (start_angle, 41);
  FIELD_BD (end_angle, 42);

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/*(36)*/
DWG_ENTITY(SPLINE)

  FIELD_BL (scenario, 0);
  UNTIL(R_2013) {
    if (FIELD_VALUE(scenario) != 1 && FIELD_VALUE(scenario) != 2)
      fprintf(stderr, "Error: unknown scenario %d", FIELD_VALUE (scenario));
  }
  SINCE(R_2013) {
    FIELD_BL (splineflags1, 0);
    FIELD_BL (knotparam, 0);
  }

  FIELD_BL (degree, 71);

  if (FIELD_VALUE(scenario) & 2) // bezier spline
    {
      FIELD_BD (fit_tol, 44); // def: 0.0000001
      FIELD_3BD (beg_tan_vec, 12);
      FIELD_3BD (end_tan_vec, 13);
      FIELD_BL (num_fit_pts, 74);
    }
  if (FIELD_VALUE(scenario) & 1) // spline
    {
      FIELD_B (rational, 0); // flag bit 2
      FIELD_B (closed_b, 0); // flag bit 0
      FIELD_B (periodic, 0); // flag bit 1
      FIELD_BD (knot_tol, 42); // def: 0.0000001
      FIELD_BD (ctrl_tol, 43); // def: 0.0000001
      FIELD_BL (num_knots, 72);
      FIELD_BL (num_ctrl_pts, 73);
      FIELD_B (weighted, 0);
      
      FIELD_VALUE(flag) = FIELD_VALUE(closed_b) +
        (FIELD_VALUE(periodic) << 1) +
        (FIELD_VALUE(rational) << 2) +
        (FIELD_VALUE(weighted) << 3);
      // planar, linear
    }
  
  if (FIELD_VALUE(scenario) & 1) {
    REPEAT(num_knots, knots, BITCODE_BD)
      {
        FIELD_BD (knots[rcount], 40);
      }
    END_REPEAT(knots);
    REPEAT(num_ctrl_pts, ctrl_pts, Dwg_Entity_SPLINE_control_point)
      {
        FIELD_3BD (ctrl_pts[rcount], 10);
        if (!FIELD_VALUE(weighted))
            FIELD_VALUE(ctrl_pts[rcount].w) = 0; // skipped when encoding
        else
          FIELD_BD (ctrl_pts[rcount].w, 41);
      }
    END_REPEAT(ctrl_pts);
  }
  if (FIELD_VALUE(scenario) & 2) {
    REPEAT(num_fit_pts, fit_pts, Dwg_Entity_SPLINE_point)
      {
        FIELD_3BD (fit_pts[rcount], 11);
      }
    END_REPEAT(fit_pts);
  }

  COMMON_ENTITY_HANDLE_DATA;
DWG_ENTITY_END

//TODO: 37, 38 and 39 are ACIS entities

#define PARSE_WIRE_STRUCT(name)                       \
  FIELD_RC (name.type, 0);                            \
  FIELD_BL (name.selection_marker, 0);                \
  FIELD_BS (name.color, 0);                           \
  FIELD_BL (name.acis_index, 0);                      \
  FIELD_BL (name.num_points, 0);                      \
  FIELD_3DPOINT_VECTOR (name.points, name.num_points, 0); \
  FIELD_B (name.transform_present, 0);                \
  if (FIELD_VALUE(name.transform_present))            \
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

#ifdef IS_DECODER

#define DECODE_3DSOLID decode_3dsolid(dat, hdl_dat, obj, _obj);
void decode_3dsolid(Bit_Chain* dat, Bit_Chain* hdl_dat, Dwg_Object* obj, Dwg_Entity_3DSOLID* _obj);

void decode_3dsolid(Bit_Chain* dat, Bit_Chain* hdl_dat, Dwg_Object* obj, Dwg_Entity_3DSOLID* _obj)
{
  Dwg_Data* dwg = obj->parent;
  int vcount, rcount, rcount2;
  int i=0;
  unsigned long j;
  int index;
  int total_size = 0;
  int num_blocks = 0;

  FIELD_B (acis_empty, 0);
  if (!FIELD_VALUE(acis_empty))
    {
      FIELD_B (unknown, 0);
      FIELD_BS (version, 70);
      if (FIELD_VALUE(version) == 1)
        {
          do
            {
              FIELD_VALUE(sat_data) = (BITCODE_RC**)
                realloc(FIELD_VALUE(sat_data), (i+1) * sizeof (BITCODE_RC*));
              FIELD_VALUE(block_size) = (BITCODE_BL*)
                realloc(FIELD_VALUE(block_size), (i+1) * sizeof (BITCODE_BL));
              FIELD_BL (block_size[i], 0);
              FIELD_TF (sat_data[i], FIELD_VALUE(block_size[i]), 0);
              total_size += FIELD_VALUE (block_size[i]);
            } while(FIELD_VALUE (block_size[i++]));

          // de-obfuscate SAT data
          FIELD_VALUE(raw_sat_data) = malloc (total_size+1);
          num_blocks = i-1;
          FIELD_VALUE(num_blocks) = num_blocks;
          index = 0;
          for (i=0; i<num_blocks; i++)
            {
              for (j=0; j<FIELD_VALUE (block_size[i]); j++)
                {
                  if (FIELD_VALUE(sat_data[i][j] <= 32))
                    {
                      FIELD_VALUE(raw_sat_data)[index++] = FIELD_VALUE (sat_data[i][j]);
                    }
                  else
                    {
                      FIELD_VALUE(raw_sat_data)[index++] = 159 - FIELD_VALUE (sat_data[i][j]);
                    }
                }
            }
          FIELD_VALUE(raw_sat_data)[index] = '\0';
          LOG_TRACE("Raw SAT data:\n%s\n", FIELD_VALUE (raw_sat_data)); // DXF 1 + 3 if >255
        }
      else //if (FIELD_VALUE(version)==2)
        {
          //TODO
          LOG_ERROR("TODO: Implement parsing of SAT file (version 2) in entities 37,38 and 39.\n");
        }

      FIELD_B (wireframe_data_present, 0);
      if (FIELD_VALUE(wireframe_data_present))
        {
          FIELD_B (point_present, 0);
          if (FIELD_VALUE(point_present))
            {
              FIELD_3BD (point, 0);
            }
          else
            {
              FIELD_VALUE(point.x) = 0;
              FIELD_VALUE(point.y) = 0;
              FIELD_VALUE(point.z) = 0;
            }
          FIELD_BL (num_isolines, 0);
          FIELD_B (isoline_present, 0);
          if (FIELD_VALUE(isoline_present))
            {
              FIELD_BL (num_wires, 0);
              REPEAT(num_wires, wires, Dwg_Entity_3DSOLID_wire)
                {
                  PARSE_WIRE_STRUCT(wires[rcount])
                }
              END_REPEAT(wires);
              FIELD_BL (num_silhouettes, 0);
              REPEAT(num_silhouettes, silhouettes, Dwg_Entity_3DSOLID_silhouette)
                {
                  FIELD_BL (silhouettes[rcount].vp_id, 0);
                  FIELD_3BD (silhouettes[rcount].vp_target, 0);
                  FIELD_3BD (silhouettes[rcount].vp_dir_from_target, 0);
                  FIELD_3BD (silhouettes[rcount].vp_up_dir, 0);
                  FIELD_B (silhouettes[rcount].vp_perspective, 0);
                  FIELD_BL (silhouettes[rcount].num_wires, 0);
                  REPEAT2(silhouettes[rcount].num_wires, silhouettes[rcount].wires,
                          Dwg_Entity_3DSOLID_wire)
                    {
                      PARSE_WIRE_STRUCT(silhouettes[rcount].wires[rcount2])
                    }
                  END_REPEAT(silhouettes[rcount].wires);
                }
              END_REPEAT(silhouettes);
            }
        }

      FIELD_B (ACIS_empty_bit, 0);
      if (!FIELD_VALUE(ACIS_empty_bit))
        {
          LOG_ERROR("TODO: Implement parsing of ACIS data in the end of 3dsolid object parsing (ACIS_empty_bit==0).\n");
        }

      SINCE(R_2007) {
          FIELD_BL (unknown_2007, 0);
      }

      COMMON_ENTITY_HANDLE_DATA;

      SINCE(R_2007) {
          FIELD_HANDLE (history_id, ANYCODE, 350);
      }
    }
}
#else
#define DECODE_3DSOLID {}
#define FREE_3DSOLID {}
#endif //#if IS_DECODER

#ifdef IS_ENCODER

#define ENCODE_3DSOLID encode_3dsolid(dat, hdl_dat, obj, _obj);
void encode_3dsolid(Bit_Chain* dat, Bit_Chain* hdl_dat, Dwg_Object* obj, Dwg_Entity_3DSOLID* _obj);

void encode_3dsolid(Bit_Chain* dat, Bit_Chain* hdl_dat, Dwg_Object* obj, Dwg_Entity_3DSOLID* _obj)
{
  //TODO Implement-me
  assert(dat);
  assert(obj);
}
#else
#define ENCODE_3DSOLID {}
#define FREE_3DSOLID {}
#endif //#if IS_ENCODER

#ifdef IS_FREE
void free_3dsolid(Dwg_Object* obj, Dwg_Entity_3DSOLID* _obj);

void free_3dsolid(Dwg_Object* obj, Dwg_Entity_3DSOLID* _obj)
{
  unsigned int i;
  int vcount;
  if (FIELD_VALUE(version) == 1)
    {
      for (i=0; i < FIELD_VALUE (num_blocks); i++)
        {
          FIELD_TF (sat_data[i], block_size[i], 0);
        }
      free(_obj->sat_data);
      free(_obj->block_size);
      free(_obj->raw_sat_data);
    }
}
#undef FREE_3DSOLID
#define FREE_3DSOLID free_3dsolid(obj, _obj)
#endif


/*(37)*/
DWG_ENTITY(REGION)
  DECODER
    {
      DECODE_3DSOLID;
    }
  ENCODER
    {
      ENCODE_3DSOLID;
    }
  FREE_3DSOLID;
DWG_ENTITY_END

/*(38)*/
DWG_ENTITY(_3DSOLID)
  DECODER
    {
      DECODE_3DSOLID;
    }
  ENCODER
    {
      ENCODE_3DSOLID;
    }
  FREE_3DSOLID;
DWG_ENTITY_END

/*(39)*/
DWG_ENTITY(BODY)
  DECODER
    {
      DECODE_3DSOLID;
    }
  ENCODER
    {
      ENCODE_3DSOLID;
    }
  FREE_3DSOLID;
DWG_ENTITY_END

/*(40)*/
DWG_ENTITY(RAY)

  FIELD_3BD (point, 10);
  FIELD_3BD (vector, 11);

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/*(41)*/
DWG_ENTITY(XLINE)

  FIELD_3BD (point, 10);
  FIELD_3BD (vector, 11);

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/*(42)*/
DWG_OBJECT(DICTIONARY)

  FIELD_BL (numitems, 0);

  VERSION(R_14)
    FIELD_RC (hard_owner, 0); // always 0
  SINCE(R_2000)
    {
      IF_ENCODE_FROM_EARLIER {
        FIELD_VALUE(cloning) = FIELD_VALUE(hard_owner) & 0xffff;
      }
      FIELD_BS (cloning, 281);
      FIELD_RC (hard_owner, 0);
    }
  if (FIELD_VALUE(numitems) > 10000)
    {
      LOG_ERROR("Invalid dictionary with more than 10.000 entries. Handle: %lu\n",
              obj->handle.value);
      return;
    }
  FIELD_VECTOR_T (text, numitems, 0);

  START_HANDLE_STREAM;
  FIELD_HANDLE (parenthandle, 4, 0);
  REACTORS(4);
  XDICOBJHANDLE(3);
  SINCE(R_2000)
    {
      HANDLE_VECTOR(itemhandles, numitems, 2, 0);
    }

DWG_ENTITY_END

DWG_OBJECT(DICTIONARYWDLFT)

  FIELD_BL (numitems, 0);

  VERSION(R_14)
    FIELD_RL (cloning_rl, 0); // always 0
  SINCE(R_2000)
    {
      IF_ENCODE_FROM_EARLIER {
        FIELD_VALUE(cloning) = FIELD_VALUE(cloning_rl) & 0xffff;
      }
      FIELD_BS (cloning, 281);
      FIELD_RC (hard_owner, 0);
    }

  if (FIELD_VALUE(numitems) > 10000)
    {
      LOG_ERROR("Invalid dictionary with more than 10.000 entries. Handle: %lu\n",
              obj->handle.value);
      return;
    }
  FIELD_VECTOR_T (text, numitems, 0);

  START_HANDLE_STREAM;
  FIELD_HANDLE (parenthandle, 4, 0);
  REACTORS(4);
  XDICOBJHANDLE(3);
  SINCE(R_2000)
    {
      HANDLE_VECTOR(itemhandles, numitems, 2, 0);
    }
  FIELD_HANDLE (defaultid, 5, 7);

DWG_OBJECT_END

/*(43) not seen */
DWG_ENTITY(OLEFRAME)

  FIELD_BS (flag, 70);
  SINCE(R_2000) {
    FIELD_BS (mode, 0);
  }

  FIELD_BL (data_length, 0);
  FIELD_TF (data, FIELD_VALUE(data_length), 0);

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/*(44)*/
DWG_ENTITY(MTEXT)

  FIELD_3BD (insertion_pt, 10);
  FIELD_3BD (extrusion, 210);
  FIELD_3BD (x_axis_dir, 11);

  FIELD_BD (rect_width, 41);
  SINCE(R_2007) {
    FIELD_BD (rect_height, 46);
  }

  FIELD_BD (text_height, 40);
  FIELD_BS (attachment, 71);
  FIELD_BS (drawing_dir, 72);
  FIELD_BD (extents_height, 0); //not documented
  FIELD_BD (extents_width, 0);
  FIELD_T (text, 1); // or 3
  /* doc error:
  UNTIL(R_2007) {
    FIELD_HANDLE (style, 5, 7);
  }
  */

  SINCE(R_2000)
    {
      FIELD_BS (linespace_style, 73);
      FIELD_BD (linespace_factor, 44);
      FIELD_B (unknown_bit, 0);
    }

  SINCE(R_2004)
    {
      FIELD_BL (bg_flag, 90);
      if (FIELD_VALUE(bg_flag) & (dat->version <= R_2018 ? 1 : 0x10))
        {
          FIELD_BL (bg_scale_factor, 45); // def: 1.5
          FIELD_CMC (bg_color, 63);
          FIELD_BL (bg_transparent, 441);
        }
    }
  SINCE(R_2018)
    {
      FIELD_B(annotative, 0);
      if (!FIELD_VALUE(annotative))
        {
          FIELD_BS (class_version, 0); // def: 0
          FIELD_B (default_flag, 0);   // def: 1
        }
      // redundant fields
      FIELD_HANDLE (reg_app, 5, 0);
      FIELD_BL (attachment, 0);
      FIELD_3BD (x_axis_dir, 10);
      FIELD_3BD (insertion_pt, 11);
      FIELD_BD (rect_width, 40);
      FIELD_BD (rect_height, 41);
      FIELD_BD (extents_width, 42);
      FIELD_BD (extents_height, 43);
      // end redundant fields
      FIELD_BL (column_type, 71);
      if (FIELD_VALUE(column_type))
        {
          FIELD_BL (num_column_heights, 72);
          FIELD_BD (column_width, 44);
          FIELD_BD (gutter, 45);
          FIELD_B (auto_height, 73);
          FIELD_B (flow_reversed, 74);
          if (!FIELD_VALUE(auto_height) && FIELD_VALUE(column_type) == 2)
            {
              FIELD_VECTOR (column_heights, BD, num_column_heights, 46);
            }
        }
    }

  COMMON_ENTITY_HANDLE_DATA;
  UNTIL(R_2007) {
    FIELD_HANDLE (style, 5, 7);
  }

DWG_ENTITY_END

/*(45)*/
DWG_ENTITY(LEADER)

  FIELD_B (unknown_bit_1, 0);
  FIELD_BS (annot_type, 0);
  FIELD_BS (path_type, 0);
  FIELD_BL (numpts, 0);
  FIELD_3DPOINT_VECTOR (points, numpts, 10);
  FIELD_3DPOINT (origin, 0);
  FIELD_3DPOINT (extrusion, 210);
  FIELD_3DPOINT (x_direction, 211);
  FIELD_3DPOINT (offset_to_block_ins_pt, 212);

  SINCE(R_14) {
    FIELD_3DPOINT (endptproj, 0);
  }
  VERSIONS(R_13, R_14) {
    FIELD_BD (dimgap, 0);
  }

  FIELD_BD (box_height, 40);
  FIELD_BD (box_width , 41);
  FIELD_B (hooklineonxdir, 0);
  FIELD_B (arrowhead_on, 0);

  VERSIONS(R_13, R_14)
    {
      FIELD_BS (arrowhead_type, 0);
      FIELD_BD (dimasz, 0);
      FIELD_B (unknown_bit_2, 0);
      FIELD_B (unknown_bit_3, 0);
      FIELD_BS (unknown_short_1, 0);
      FIELD_BS (byblock_color, 0);
      FIELD_B (unknown_bit_4, 0);
      FIELD_B (unknown_bit_5, 0);
    }

  SINCE(R_2000)
    {
      FIELD_BS (unknown_short_1, 0);
      FIELD_B (unknown_bit_4, 0);
      FIELD_B (unknown_bit_5, 0);
    }

  COMMON_ENTITY_HANDLE_DATA;

  //FIXME reading these handles lead to a segfault
  //TODO check if field is present in R_13.
  //Juca thinks it is present but inactive/not used.
  SINCE(R_13) { // TODO until 2007?
    FIELD_HANDLE (associated_annotation, 2, 340);
  }

  //UNTIL(R_2007) // TODO until 2007?
  {
    FIELD_HANDLE (dimstyle, 5, 2);
  }

DWG_ENTITY_END

/*(46)*/
DWG_ENTITY(TOLERANCE)

  VERSIONS(R_13, R_14)
    {
      FIELD_BS (unknown_short, 0); //spec-typo? Spec says S instead of BS.
      FIELD_BD (height, 0);
      FIELD_BD (dimgap, 0);
    }

  FIELD_3DPOINT (ins_pt, 10);
  FIELD_3DPOINT (x_direction, 11);
  FIELD_3DPOINT (extrusion, 210);
  FIELD_T (text_string, 1);

  COMMON_ENTITY_HANDLE_DATA;
  FIELD_HANDLE (dimstyle, 5, 0);

DWG_ENTITY_END

/*(47)*/
DWG_ENTITY(MLINE)

  FIELD_BD (scale, 40);
  FIELD_RC (justification, 70); /* spec-typo? Spec says EC instead of RC */
  FIELD_3DPOINT (base_point, 10);
  FIELD_3DPOINT (extrusion, 210);
  FIELD_BS (flags, 71);
  FIELD_RC (num_lines, 73); //aka linesinstyle
  FIELD_BS (num_verts, 72);

  REPEAT(num_verts, verts, Dwg_MLINE_vertex)
    {
      FIELD_3DPOINT (verts[rcount].vertex, 10);
      FIELD_3DPOINT (verts[rcount].vertex_direction, 210);
      FIELD_3DPOINT (verts[rcount].miter_direction, 11);

      REPEAT2(num_lines, verts[rcount].lines, Dwg_MLINE_line)
        {
          FIELD_BS (verts[rcount].lines[rcount2].num_segparms, 74);
          REPEAT3(verts[rcount].lines[rcount2].num_segparms,
                  verts[rcount].lines[rcount2].segparms,
                  BITCODE_BD)
            {
              FIELD_BD (verts[rcount].lines[rcount2].segparms[rcount3], 41);
            }
          END_REPEAT(verts[rcount].lines[rcount2].segparms);

          FIELD_BS (verts[rcount].lines[rcount2].num_areafillparms, 75);
          REPEAT3(verts[rcount].lines[rcount2].num_areafillparms,
                  verts[rcount].lines[rcount2].areafillparms,
                  BITCODE_BD)
            {
              FIELD_BD (verts[rcount].lines[rcount2].areafillparms[rcount3], 42);
            }
          END_REPEAT(verts[rcount].lines[rcount2].areafillparms);
        }
      END_REPEAT(verts[rcount].lines);
    }
  END_REPEAT(verts);

  COMMON_ENTITY_HANDLE_DATA;
  FIELD_HANDLE (mlinestyle, 5, 340);

DWG_ENTITY_END

/*(48)*/
DWG_OBJECT(BLOCK_CONTROL)

  DXF {
    VALUE (FIELD_VALUE(num_entries)-2, RL, 70);
  } else {
    FIELD_BL (num_entries, 70);
  }

  START_HANDLE_STREAM;
  //printf("dat->from_version=%d < cur_ver=%d\n", dat->from_version, cur_ver);
  FIELD_HANDLE (null_handle, 4, 0);
  //printf("dat->from_version=%d < cur_ver=%d\n", dat->from_version, cur_ver);
  XDICOBJHANDLE(3);
  HANDLE_VECTOR(block_headers, num_entries, 2, 0);
  FIELD_HANDLE (model_space, 3, 0);
  UNTIL(R_2007) {
    FIELD_HANDLE (paper_space, 3, 0);
  }

DWG_OBJECT_END

/* (49/1) */
DWG_OBJECT(BLOCK_HEADER)

  COMMON_TABLE_FLAGS

  PRE(R_13)
  {
    FIELD_RD (base_pt.z, 30);
    FIELD_2RD (base_pt, 10);
    FIELD_RC (block_scaling, 0);
    FIELD_CAST (owned_object_count, RS, BL, 0);
    FIELD_RC (flag2, 0);
    FIELD_CAST (insert_count, RS, RL, 0);
    FIELD_RS (flag3, 0);
  }
  SINCE(R_13) {
    FIELD_B (anonymous, 0); // bit 1
    FIELD_B (hasattrs, 0);  // bit 2
    FIELD_B (blkisxref, 0); // bit 4
    FIELD_B (xrefoverlaid, 0); // bit 8
  }
  SINCE(R_2000) {
    FIELD_B (loaded_bit, 0); // bit 32
  }
  SINCE(R_13) {
    FIELD_VALUE(flag) = FIELD_VALUE(anonymous) |
                        FIELD_VALUE(hasattrs) << 1 |
                        FIELD_VALUE(blkisxref) << 2 |
                        FIELD_VALUE(xrefoverlaid) << 3 |
                        FIELD_VALUE(xrefdep) << 4 |
                        FIELD_VALUE(xrefref) << 6;
  }
  SINCE(R_2004) { // but not in 2007
    FIELD_BL (owned_object_count, 0);
    if (FIELD_VALUE(owned_object_count) > 0xf00000)
      {
        LOG_WARN("Unreasonable high owned_object_count value")
      }
  }

  SINCE(R_13) {
    FIELD_3DPOINT (base_pt, 10);
    FIELD_T (xref_pname, 1); // and 3
  }

  SINCE(R_2000)
    {
      FIELD_INSERT_COUNT (insert_count, RL, 0);
      FIELD_T (description, 4);

      FIELD_BL (preview_data_size, 0);
      if (FIELD_VALUE(preview_data_size) > 0xf00000)
        {
          LOG_WARN("Unreasonable high preview_data_size value")
        }
      else
        {
          FIELD_TF (preview_data, FIELD_VALUE(preview_data_size), 310);
        }
    }

  SINCE(R_2007)
    {
      FIELD_BS (insert_units, 70);
      FIELD_B (explodable, 280);
      FIELD_RC (block_scaling, 281);
    }

  SINCE(R_13) {
    START_HANDLE_STREAM;
    FIELD_HANDLE (block_control_handle, 4, 0);
    REACTORS(4);
    XDICOBJHANDLE(3);
    FIELD_HANDLE (null_handle, 5, 0);
    FIELD_HANDLE (block_entity, 3, 0);
  }

  VERSIONS(R_13, R_2000)
    {
      if (!FIELD_VALUE(blkisxref) && !FIELD_VALUE(xrefoverlaid))
        {
          FIELD_HANDLE (first_entity, 4, 0);
          FIELD_HANDLE (last_entity, 4, 0);
        }
    }

  SINCE(R_2004)
    {
      if (FIELD_VALUE(owned_object_count) < 0xf00000)
        {
          HANDLE_VECTOR(entities, owned_object_count, 4, 0);
        }
    }

  SINCE(R_13) {
    FIELD_HANDLE (endblk_entity, 3, 0);
  }
  SINCE(R_2000)
    {
      if (FIELD_VALUE(insert_count) && FIELD_VALUE(insert_count) < 0xf00000) {
        HANDLE_VECTOR(insert_handles, insert_count, ANYCODE, 0);
      }
      FIELD_HANDLE (layout_handle, 5, 0);
    }

DWG_OBJECT_END

/*(50)*/
DWG_OBJECT(LAYER_CONTROL)

  FIELD_BL (num_entries, 70);

  START_HANDLE_STREAM;
  FIELD_HANDLE (null_handle, 4, 0);
  XDICOBJHANDLE(3);
  HANDLE_VECTOR(layers, num_entries, 2, 0);

DWG_OBJECT_END

/* (51/2) */
DWG_OBJECT(LAYER)

  COMMON_TABLE_FLAGS

  PRE(R_13)
  {
    FIELD_RS (color_rs, 62);     // color
    FIELD_RS (linetype_rs, 7);   // style
  }
  VERSIONS(R_13, R_14)
  {
    FIELD_B (frozen, 0); // bit 1
    FIELD_B (on, 0); // unused, negate the color
    FIELD_B (frozen_in_new, 0);
    FIELD_B (locked, 0);
    FIELD_VALUE(flag) = FIELD_VALUE(frozen) |
      (FIELD_VALUE(frozen_in_new) << 1) |
      (FIELD_VALUE(locked) << 2) |
      (FIELD_VALUE(color_rs) < 0 ? 32 : 0) |
      (FIELD_VALUE(xrefdep) << 4) |
      (FIELD_VALUE(xrefref) << 6);
  }
  SINCE(R_2000) {
    FIELD_BS (flag_s, 70); // 70,290,370
    // contains frozen (1 bit), on (2 bit), frozen by default in new viewports (4 bit),
    // locked (8 bit), plotting flag (16 bit), and lineweight (mask with 0x03E0)
    FIELD_VALUE(flag) = (BITCODE_RC)FIELD_VALUE(flag_s) & 0xff;
    FIELD_VALUE(frozen) = FIELD_VALUE(flag) & 1;
    FIELD_VALUE(on) = FIELD_VALUE(flag) & 2;
    FIELD_VALUE(frozen_in_new) = FIELD_VALUE(flag) & 4;
    FIELD_VALUE(locked) = FIELD_VALUE(flag) & 8;
    //TODO plotf 290 bit 16, linewidth 370 (flag_s & 0x03E0) >> 5
  }
  FIELD_CMC (color, 62);

  //LOG_TRACE("@%lu.%u (%lu)\n", hdl_dat->byte, hdl_dat->bit, bit_position(hdl_dat));
  START_HANDLE_STREAM;
  if (0) {
    SINCE(R_2007) {
      // 64266: 64138 + 170 (- 42)
      long pos = bit_position(hdl_dat);
      for (; !_obj->layer_control || _obj->layer_control->absolute_ref != 2;
           bit_set_position(hdl_dat, pos) )
        {
          LOG_TRACE("@%lu.%u (%lu)\n", hdl_dat->byte, hdl_dat->bit, pos);
          FIELD_HANDLE (layer_control, 4, 0);
          pos--;
        }
    } else {
      FIELD_HANDLE (layer_control, 4, 0);
    }
  }
  FIELD_HANDLE (layer_control, 4, 0);
  REACTORS(4);
  XDICOBJHANDLE(3);
  FIELD_HANDLE (xref, 5, 0);
  SINCE(R_2000) {
    FIELD_HANDLE (plotstyle, 5, 390);
  }
  SINCE(R_2007) {
    FIELD_HANDLE (material, ANYCODE, 347);
  }
  FIELD_HANDLE (linetype, 5, 6);
  //FIELD_HANDLE (null_handle, 5); // doc error?

DWG_OBJECT_END

/* STYLE table (52) */
DWG_OBJECT(SHAPEFILE_CONTROL)

  FIELD_BL (num_entries, 70);

  START_HANDLE_STREAM;
  FIELD_HANDLE (null_handle, 4, 0);
  XDICOBJHANDLE(3);
  HANDLE_VECTOR(styles, num_entries, 2, 0);

DWG_OBJECT_END

/* (53/3) preR13: STYLE */
DWG_OBJECT(SHAPEFILE)

  COMMON_TABLE_FLAGS

  SINCE(R_13)
  {
    FIELD_B (vertical, 0);     //1
    FIELD_B (shape_file, 0);   //1
  }
  PRE(R_13)
  {
    FIELD_RD (fixed_height, 40);
    FIELD_RD (width_factor, 41);
    FIELD_RD (oblique_ang, 50);
    FIELD_RC (generation, 71);
    FIELD_RD (last_height, 42);
    FIELD_TF (font_name, 128, 3);
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
    //TODO 1071  long truetype font’s pitch and family, charset, and italic and bold flags

    START_HANDLE_STREAM;
    FIELD_HANDLE (shapefile_control, 4, 0);
    REACTORS(4);
    XDICOBJHANDLE(3);
    UNTIL(R_2007) {
      FIELD_HANDLE (null_handle, 5, 0);
    }
  }

DWG_OBJECT_END

//(54): Unknown
//(55): Unknown

/*(56)*/
DWG_OBJECT(LTYPE_CONTROL)

  FIELD_BS (num_entries, 70);

  START_HANDLE_STREAM;
  FIELD_HANDLE (null_handle, 4, 0);
  XDICOBJHANDLE(3);
  HANDLE_VECTOR(linetypes, num_entries, 2, 0);
  FIELD_HANDLE (bylayer, 3, 0);
  FIELD_HANDLE (byblock, 3, 0);

DWG_OBJECT_END

/* (57/5) */
DWG_OBJECT(LTYPE)

  COMMON_TABLE_FLAGS

  PRE(R_13)
  {
    FIELD_TF (description, 48, 3);
    FIELD_RC (alignment, 72);
  }
  LATER_VERSIONS
  {
    FIELD_T (description, 48);
    FIELD_BD (pattern_len, 40); // total length
    FIELD_RC (alignment, 72);
  }
  FIELD_RC (num_dashes, 73);
  REPEAT(num_dashes, dash, LTYPE_dash)
    {
      PRE(R_13)
      {
        FIELD_RD (dash[rcount].length, 49);
#ifndef IS_PRINT        
        FIELD_VALUE(pattern_len) += FIELD_VALUE(dash[rcount].length);
#endif
        FIELD_RS (dash[rcount].complex_shapecode, 74);
        FIELD_RD (dash[rcount].x_offset, 44);
        FIELD_RD (dash[rcount].y_offset, 45);
        FIELD_RD (dash[rcount].scale, 46);
        FIELD_RD (dash[rcount].rotation, 50);
        FIELD_RS (dash[rcount].shape_flag, 75);
        if (FIELD_VALUE(dash[rcount].shape_flag) & 0x2)
          FIELD_VALUE(text_area_is_present) = 1;
      }
      LATER_VERSIONS
      {
        FIELD_BD (dash[rcount].length, 49);
        FIELD_BS (dash[rcount].complex_shapecode, 74);
        FIELD_RD (dash[rcount].x_offset, 44);
        FIELD_RD (dash[rcount].y_offset, 45);
        FIELD_BD (dash[rcount].scale, 46);
        FIELD_BD (dash[rcount].rotation, 50);
        FIELD_BS (dash[rcount].shape_flag, 75);
        if (FIELD_VALUE(dash[rcount].shape_flag) & 0x2)
          FIELD_VALUE(text_area_is_present) = 1;
      }
    }
  END_REPEAT(dash);

  UNTIL(R_2004) {
    FIELD_TF (strings_area, 256, 3);
  }
  LATER_VERSIONS
    {
      if (FIELD_VALUE(text_area_is_present))
        {
          FIELD_TF (strings_area, 512, 3);
        }
    }

  START_HANDLE_STREAM;
  FIELD_HANDLE (linetype_control, 4, 0);
  REACTORS(4);
  XDICOBJHANDLE(3);
  FIELD_HANDLE (null_handle, 5, 0);
  HANDLE_VECTOR(shapefiles, num_dashes, 5, 340);

DWG_OBJECT_END

//(58): Unknown
//(59): Unknown

/*(60)*/
DWG_OBJECT(VIEW_CONTROL)

  FIELD_BL (num_entries, 70);

  START_HANDLE_STREAM;
  FIELD_HANDLE (null_handle, 4, 0);
  XDICOBJHANDLE(3);
  HANDLE_VECTOR(views, num_entries, 2, 0);

DWG_OBJECT_END

/* (61/6) */
DWG_OBJECT(VIEW)

  COMMON_TABLE_FLAGS

  PRE(R_13)
  {
    FIELD_RD (height, 40);
    FIELD_2RD (center, 10);
    FIELD_RD (width, 41);
    FIELD_3RD (target, 12);
    FIELD_3RD (direction, 11);
    FIELD_CAST (view_mode, RS, 4BITS, 71);
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
    FIELD_4BITS(view_mode, 71);
  }
  SINCE(R_2000) {
    FIELD_RC (render_mode, 281);
  }
  SINCE(R_2007) {
    IF_ENCODE_FROM_EARLIER {
      FIELD_VALUE(use_default_lights) = 1;
      FIELD_VALUE(default_lightning_type) = 1;
      FIELD_VALUE(ambient_color.index) = 250;
      //TODO FIELD_VALUE(ambient_color.rgb) = ?;
      //TODO FIELD_VALUE(ambient_color.byte) = ?; //+ name, book_name
    }
    FIELD_B (use_default_lights, 292);
    FIELD_RC (default_lightning_type, 282);
    FIELD_BD (brightness, 141);
    FIELD_BD (contrast, 142);
    FIELD_CMC (ambient_color, 63);
  }
  SINCE(R_13) {
    FIELD_B (pspace_flag, 0);
    FIELD_VALUE(flag) =
      FIELD_VALUE(pspace_flag) |
      FIELD_VALUE(xrefdep) << 4 |
      (FIELD_VALUE(xrefindex_plus1)>0 ? 32 : 0) |
      FIELD_VALUE(xrefref) << 6;
  }
  SINCE(R_2000)
    {
      FIELD_B (associated_ucs, 72);

      if (FIELD_VALUE(associated_ucs) & 1)
        {
          FIELD_3BD (origin, 110);
          FIELD_3BD (x_direction, 111);
          FIELD_3BD (y_direction, 112);
          FIELD_BD (elevation, 146);
          FIELD_BS (orthographic_view_type, 79);
        }
    }

  SINCE(R_2007) {
    FIELD_B (camera_plottable, 73);
  }
  SINCE(R_13) {
    START_HANDLE_STREAM;
    FIELD_HANDLE (view_control_handle, 4, 0);
    REACTORS(4);
    XDICOBJHANDLE(3);
    FIELD_HANDLE (null_handle, 5, 0);
  }
  SINCE(R_2007) {
    FIELD_HANDLE (background_handle, 4, 332);
    FIELD_HANDLE (visual_style_handle, 5, 348);
    FIELD_HANDLE (sun_handle, 3, 361);
  }
  SINCE(R_2000)
    {
      if (FIELD_VALUE(associated_ucs) & 1)
        {
          FIELD_HANDLE (base_ucs_handle, 5, 346);
          FIELD_HANDLE (named_ucs_handle, 5, 345);
	}
    }
  SINCE(R_2007) {
    FIELD_HANDLE (live_section_handle, 4, 334);
  }

DWG_OBJECT_END

/*(62)*/
DWG_OBJECT(UCS_CONTROL)

  FIELD_BS (num_entries, 70);

  START_HANDLE_STREAM;
  FIELD_HANDLE (null_handle, 4, 0);
  XDICOBJHANDLE(3);
  HANDLE_VECTOR(ucs, num_entries, 2, 0);

DWG_OBJECT_END

/* (63/7) */
DWG_OBJECT(UCS)

  COMMON_TABLE_FLAGS

  PRE(R_13)
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

  SINCE(R_2000)
  {
    FIELD_BD (elevation, 146);
    FIELD_BS (orthographic_view_type, 71);
    FIELD_BS (orthographic_type, 71); // on pairs with 13?
  }

  SINCE(R_13)
  {
    START_HANDLE_STREAM;
    FIELD_HANDLE (ucs_control_handle, 4, 0);
    REACTORS(4);
    XDICOBJHANDLE(3);
    FIELD_HANDLE (null_handle, 5, 0);
  }
  SINCE(R_2000)
    {
      FIELD_HANDLE (base_ucs_handle, ANYCODE, 346);
      FIELD_HANDLE (unknown, ANYCODE, 0);
    }
DWG_OBJECT_END

/*(64)*/
DWG_OBJECT(VPORT_CONTROL)

  FIELD_BS (num_entries, 70);

  START_HANDLE_STREAM;
  FIELD_HANDLE (null_handle, 4, 0);
  XDICOBJHANDLE(3);
  HANDLE_VECTOR(vports, num_entries, 2, 0);

DWG_OBJECT_END

/* (65/8) */
DWG_OBJECT(VPORT)

  COMMON_TABLE_FLAGS

  PRE(R_13)
  { // TODO verify
    FIELD_RD (VIEWSIZE, 40);
    FIELD_RD (aspect_ratio, 41);
    FIELD_2RD (VIEWCTR, 12);
    FIELD_3RD (view_target, 17);
    FIELD_3RD (VIEWDIR, 16);
    FIELD_RD (view_twist, 50);
    FIELD_RD (lens_length, 42);
    FIELD_RD (front_clip, 43);
    FIELD_RD (back_clip, 33);
    FIELD_CAST (view_mode, RS, 4BITS, 71);

    FIELD_2RD (lower_left, 10);
    FIELD_2RD (upper_right, 11);
    FIELD_RC (UCSFOLLOW, 71);
    FIELD_RS (circle_zoom, 72);
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
    FIELD_BD (VIEWSIZE, 40);
    FIELD_BD (aspect_ratio, 41);
    FIELD_2RD (VIEWCTR, 12);
    FIELD_3BD (view_target, 17);
    FIELD_3BD (VIEWDIR, 16);
    FIELD_BD (view_twist, 50);
    FIELD_BD (lens_length, 42);
    FIELD_BD (front_clip, 43);
    FIELD_BD (back_clip, 33);
    FIELD_4BITS (view_mode, 71);

    SINCE(R_2000) {
      FIELD_RC (render_mode, 281);
    }
    SINCE(R_2007)
    {
      IF_ENCODE_FROM_EARLIER {
        FIELD_VALUE(use_default_lights) = 1;
        FIELD_VALUE(default_lightning_type) = 1;
        FIELD_VALUE(ambient_color.index) = 250;
        //TODO FIELD_VALUE(ambient_color.rgb) = ?;
        //TODO FIELD_VALUE(ambient_color.byte) = ?; //+ name, book_name
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
    FIELD_BB (UCSICON, 74);
    FIELD_B (GRIDMODE, 76);
    FIELD_2RD (GRIDUNIT, 15);
    FIELD_B (SNAPMODE, 75);
    FIELD_B (SNAPSTYLE, 77);
    FIELD_BS (SNAPISOPAIR, 78);
    FIELD_BD (SNAPANG, 50);
    FIELD_2RD (SNAPBASE, 13);
    FIELD_2RD (SNAPUNIT, 14);

    SINCE(R_2000)
    {
      FIELD_B (unknown, 0);
      FIELD_B (ucs_pre_viewport, 71); // bit 0 of 71
      FIELD_3BD (ucs_origin, 110);
      FIELD_3BD (ucs_x_axis, 111);
      FIELD_3BD (ucs_y_axis, 112);
      FIELD_BD (ucs_elevation, 146);
      FIELD_BS (ucs_orthografic_type, 79);
    }

    SINCE(R_2007)
    {
      FIELD_BS (grid_flags, 60);
      FIELD_BS (grid_major, 61);
    }

    START_HANDLE_STREAM;
    FIELD_HANDLE (vport_control, 4, 0);
    REACTORS(4);
    XDICOBJHANDLE(3);
    FIELD_HANDLE (null_handle, 5, 0);
  }

  SINCE(R_2007)
    {
      FIELD_HANDLE (background_handle, 4, 332);
      FIELD_HANDLE (visual_style_handle, 5, 348);
      FIELD_HANDLE (sun_handle, 3, 361); //333 shade_plot?
    }

  SINCE(R_2000)
    {
      FIELD_HANDLE (named_ucs_handle, 5, 345);
      FIELD_HANDLE (base_ucs_handle, 5, 346);
    }

DWG_OBJECT_END

/*(66)*/
DWG_OBJECT(APPID_CONTROL)

  FIELD_BS (num_entries, 70);

  START_HANDLE_STREAM;
  FIELD_HANDLE (null_handle, 4, 0);
  XDICOBJHANDLE(3);
  HANDLE_VECTOR(apps, num_entries, 2, 0);

DWG_OBJECT_END

/* (67/9) */
DWG_OBJECT(APPID)

  COMMON_TABLE_FLAGS

  SINCE(R_13) {
    FIELD_RC (unknown, 71); // not in DXF if 0. has_something
  }
  VERSIONS(R_13, R_2007)
    {
      //START_HANDLE_STREAM;
      FIELD_HANDLE (app_control, 4, 0);
      REACTORS(4);
      XDICOBJHANDLE(3);
      FIELD_HANDLE (null_handle, 5, 0);
    }

DWG_OBJECT_END

/*(68)*/
DWG_OBJECT(DIMSTYLE_CONTROL)

  FIELD_BS (num_entries, 70);
  SINCE(R_2000)
    {
      /* number of additional hard handles, undocumented */
      FIELD_RC (num_morehandles, 0);
    }

  START_HANDLE_STREAM;
  FIELD_HANDLE (null_handle, 4, 0);
  XDICOBJHANDLE(3);
  HANDLE_VECTOR (dimstyles, num_entries, 2, 0);
  HANDLE_VECTOR (morehandles, num_morehandles, 5, 0);

DWG_OBJECT_END

/* (69/10) */
DWG_OBJECT(DIMSTYLE)

  COMMON_TABLE_FLAGS

  PRE(R_13)
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
  VERSIONS(R_13, R_14)
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
      FIELD_CMC (DIMCLRD, 176);
      FIELD_CMC (DIMCLRE, 177);
      FIELD_CMC (DIMCLRT, 178);
    }

  SINCE(R_2000)
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

  SINCE(R_2007)
    {
      FIELD_BD (DIMFXL, 49);
      FIELD_BD (DIMJOGANG, 50);
      FIELD_BS (DIMTFILL, 69);
      FIELD_CMC (DIMTFILLCLR, 70);
    }

  SINCE(R_2000)
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

  SINCE(R_2007)
    {
      FIELD_BS (DIMARCSYM, 90);
    }

  SINCE(R_2000)
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
      FIELD_CMC (DIMCLRD, 176);
      FIELD_CMC (DIMCLRE, 177);
      FIELD_CMC (DIMCLRT, 178);
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

  SINCE(R_2007)
    {
      FIELD_B (DIMFXLON, 290);
    }

  SINCE(R_2010)
    {
      FIELD_B (DIMTXTDIRECTION, 295);
      FIELD_BD (DIMALTMZF, 0); // undocumented
      FIELD_T (DIMALTMZS, 0); // undocumented
      FIELD_BD (DIMMZF, 0); // undocumented
      FIELD_T (DIMMZS, 0); // undocumented
    }

  SINCE(R_2000)
    {
      FIELD_BS (DIMLWD, 371);
      FIELD_BS (DIMLWE, 372);
    }

  SINCE(R_13)
  {
    FIELD_B (flag, 70); // Bit 0 of 70

    START_HANDLE_STREAM;
    FIELD_HANDLE (dimstyle_control, 4, 0);
    REACTORS(4);
    XDICOBJHANDLE(3);
    FIELD_HANDLE (extref_handle, 5, 0);
    FIELD_HANDLE (DIMTXSTY, 5, 340); /* Text style (DIMTXSTY) */
  }

  SINCE(R_2000)
    {
      FIELD_HANDLE (DIMLDRBLK, 5, 341); /* Leader arrow	(DIMLDRBLK)*/
      FIELD_HANDLE (DIMBLK, 5, 342);  /* Arrow */
      FIELD_HANDLE (DIMBLK1, 5, 343); /* Arrow 1 */
      FIELD_HANDLE (DIMBLK2, 5, 344); /* Arrow 2 */
    }

  SINCE(R_2007)
    {
      FIELD_HANDLE (DIMLTYPE, ANYCODE, 345);
      FIELD_HANDLE (DIMLTEX1, ANYCODE, 346);
      FIELD_HANDLE (DIMLTEX2, ANYCODE, 347);
    }

DWG_OBJECT_END

/* VIEWPORT ENTITY CONTROL (70)*/
DWG_OBJECT(VP_ENT_HDR_CONTROL)

  FIELD_BS (num_entries, 70);

  START_HANDLE_STREAM;
  FIELD_HANDLE (null_handle, 4, 0);
  XDICOBJHANDLE(3);
  HANDLE_VECTOR (vp_ent_headers, num_entries, 4, 0);

DWG_OBJECT_END

/* VIEWPORT ENTITY HEADER (71/11) */
DWG_OBJECT(VP_ENT_HDR)

  COMMON_TABLE_FLAGS

  SINCE(R_13) {
    FIELD_B (flag1, 70); // bit 1 of 70
    FIELD_VALUE(flag) =
      (FIELD_VALUE(flag1) << 1) |
      (FIELD_VALUE(xrefdep) << 4) |
      (FIELD_VALUE(xrefref) << 6);

    FIELD_HANDLE (vp_ent_ctrl, ANYCODE, 0);
    XDICOBJHANDLE(3);
    FIELD_HANDLE (vp_ent, 5, 0);
  }

DWG_OBJECT_END

/*(72)*/
DWG_OBJECT(GROUP)

  FIELD_T (name, 300);
  FIELD_BS (unnamed, 70);
  FIELD_BS (selectable, 71);
  FIELD_BL (num_handles, 0);

  START_HANDLE_STREAM;
  FIELD_HANDLE (parenthandle, 4, 330);
  REACTORS(4);
  XDICOBJHANDLE(3);
  HANDLE_VECTOR(group_entries, num_handles, 5, 340);

DWG_OBJECT_END

/* (73) undocumented DXF codes */
DWG_OBJECT(MLINESTYLE)

  FIELD_T (entry_name, 2);
  FIELD_T (desc, 3);
  FIELD_BS (flag, 70);
  FIELD_CMC (fill_color, 62);
  FIELD_BD (start_angle, 51);
  FIELD_BD (end_angle, 52);
  FIELD_RC (num_lines, 71);
  REPEAT(num_lines, lines, Dwg_MLINESTYLE_line)
  {
    FIELD_BD (lines[rcount].offset, 49);
#ifndef IS_FREE
    FIELD_CMC (lines[rcount].color, 62);
#endif
    PRE(R_2018)
    {
#ifdef IS_DXF
        switch (FIELD_VALUE(lines[rcount].ltindex)) {
        case 32767: VALUE_TV("BYLAYER", 6); break;
        case 32766: VALUE_TV("BYBLOCK", 6); break;
        case 0: VALUE_TV("CONTINUOUS", 6); break;
        //else lookup on LTYPE_CONTROL list
        default: /*FIELD_HANDLE(ltype, 5, 6);*/
                 VALUE_TV("", 6); break;
        }
#else
        FIELD_BS (lines[rcount].ltindex, 6);
#endif
    }
    SINCE(R_2018)
    {
      FIELD_HANDLE (lines[rcount].ltype, 5, 6);
    }
  }
  END_REPEAT(lines);

  START_HANDLE_STREAM;
  FIELD_HANDLE (parenthandle, 4, 0);
  REACTORS(4);
  XDICOBJHANDLE(3);

DWG_OBJECT_END

//pg.135
DWG_OBJECT(DICTIONARYVAR)

  FIELD_RC (intval, 0);
  FIELD_T (str, 0);
  FIELD_HANDLE (parenthandle, 4, 0);
  REACTORS(4);
  XDICOBJHANDLE(3);

DWG_OBJECT_END

//(78 + varies) pg.136
DWG_ENTITY(HATCH)

  SINCE(R_2004)
    {
      FIELD_BL (is_gradient_fill, 450);
      FIELD_BL (reserved, 451);
      FIELD_BD (gradient_angle, 460);
      FIELD_BD (gradient_shift, 461);
      FIELD_BL (single_color_gradient, 452);
      FIELD_BD (gradient_tint, 462);
      FIELD_BL (num_colors, 453); //default: 2
      REPEAT(num_colors, colors, Dwg_Entity_HATCH_Color)
        {
          FIELD_BD (colors[rcount].unknown_double, 463); //value
          FIELD_BS (colors[rcount].unknown_short, 0);
          FIELD_BL (colors[rcount].rgb_color, 63); // 63 for color as ACI. 421 for rgb
          FIELD_RC (colors[rcount].ignored_color_byte, 0);
        }
      END_REPEAT(colors);
      FIELD_T (gradient_name, 470);
    }

  FIELD_BD (elevation, 30);
  FIELD_3BD (extrusion, 210);
  FIELD_T (name, 2); //default: SOLID
  FIELD_B (solid_fill, 70); //default: 1, pattern_fill: 0
  FIELD_B (associative, 71);
  FIELD_BL (num_paths, 91);
  REPEAT(num_paths, paths, Dwg_Entity_HATCH_Path)
    {
      FIELD_BL (paths[rcount].flag, 92);
      if (!(FIELD_VALUE(paths[rcount].flag) & 2))
        {
          FIELD_BL (paths[rcount].num_segs_or_paths, 93);
          REPEAT2(paths[rcount].num_segs_or_paths, paths[rcount].segs,
                  Dwg_Entity_HATCH_PathSeg)
            {
              FIELD_RC (paths[rcount].segs[rcount2].type_status, 72);
              switch (FIELD_VALUE(paths[rcount].segs[rcount2].type_status))
                {
                    case 1: /* LINE */
                      FIELD_2RD (paths[rcount].segs[rcount2].first_endpoint, 10);
                      FIELD_2RD (paths[rcount].segs[rcount2].second_endpoint, 11);
                      break;
                    case 2: /* CIRCULAR ARC */
                      FIELD_2RD (paths[rcount].segs[rcount2].center, 10);
                      FIELD_BD (paths[rcount].segs[rcount2].radius, 40);
                      FIELD_BD (paths[rcount].segs[rcount2].start_angle, 50);
                      FIELD_BD (paths[rcount].segs[rcount2].end_angle, 51);
                      FIELD_B (paths[rcount].segs[rcount2].is_ccw, 73);
                      break;
                    case 3: /* ELLIPTICAL ARC */
                      FIELD_2RD (paths[rcount].segs[rcount2].center, 10);
                      FIELD_2RD (paths[rcount].segs[rcount2].endpoint, 11);
                      FIELD_BD (paths[rcount].segs[rcount2].minor_major_ratio, 40);
                      FIELD_BD (paths[rcount].segs[rcount2].start_angle, 50);
                      FIELD_BD (paths[rcount].segs[rcount2].end_angle, 51);
                      FIELD_B (paths[rcount].segs[rcount2].is_ccw, 73);
                      break;
                    case 4: /* SPLINE */
                      FIELD_BL (paths[rcount].segs[rcount2].degree, 94);
                      FIELD_B (paths[rcount].segs[rcount2].is_rational, 73);
                      FIELD_B (paths[rcount].segs[rcount2].is_periodic, 74);
                      FIELD_BL (paths[rcount].segs[rcount2].num_knots, 95);
                      FIELD_BL (paths[rcount].segs[rcount2].num_control_points, 96);
                      FIELD_VECTOR (paths[rcount].segs[rcount2].knots, BD,
                                    paths[rcount].segs[rcount2].num_knots, 40);
                      REPEAT3(paths[rcount].segs[rcount2].num_control_points,
                              paths[rcount].segs[rcount2].control_points,
                              Dwg_Entity_HATCH_ControlPoint)
                        {
                          FIELD_2RD (paths[rcount].segs[rcount2].control_points[rcount3].point, 10);
                          if (FIELD_VALUE(paths[rcount].segs[rcount2].is_rational))
                            {
                              FIELD_BD (paths[rcount].segs[rcount2].control_points[rcount3].weigth, 40);
                            }
                        }
                      END_REPEAT(paths[rcount].segs[rcount2].control_points);
                      SINCE(R_2013) // r2014 really
                        {
                          FIELD_BL(paths[rcount].segs[rcount2].num_fitpts, 97);
                          FIELD_2RD_VECTOR(paths[rcount].segs[rcount2].fitpts,
                                           paths[rcount].segs[rcount2].num_fitpts, 11);
                        }
                      break;
                    default:
                      LOG_ERROR("Invalid type_status in HATCH entity\n")
                      break;
                }
            }
          END_REPEAT(paths[rcount].segs);
        }
      else
        { /* POLYLINE PATH */
          FIELD_B (paths[rcount].bulges_present, 72);
          FIELD_B (paths[rcount].closed, 73);
          FIELD_BL (paths[rcount].num_segs_or_paths, 91);
          REPEAT2(paths[rcount].num_segs_or_paths, paths[rcount].polyline_paths,
                  Dwg_Entity_HATCH_PolylinePath)
            {
              FIELD_2RD (paths[rcount].polyline_paths[rcount2].point, 10);
              if (FIELD_VALUE(paths[rcount].bulges_present))
                {
                  FIELD_BD (paths[rcount].polyline_paths[rcount2].bulge, 42);
                }
            }
          END_REPEAT(paths[rcount].polyline_paths);
        }
      FIELD_BL (paths[rcount].num_boundary_handles, 97);
      DECODER {
        FIELD_VALUE (num_boundary_handles) += FIELD_VALUE (paths[rcount].num_boundary_handles);
        FIELD_VALUE (has_derived) =
          FIELD_VALUE (has_derived) || (FIELD_VALUE (paths[rcount].flag) & 0x4);
      }
    }
  END_REPEAT(paths);
  FIELD_BS (style, 75); // 0=normal (odd parity); 1=outer; 2=whole
  FIELD_BS (pattern_type, 76); // 0=user; 1=predefined; 2=custom
  if (!FIELD_VALUE(solid_fill))
    {
      FIELD_BD (angle, 52);
      FIELD_BD (scale_spacing, 41); //default 1.0
      FIELD_B (double_flag, 77);
      FIELD_BS (num_deflines, 78);
      REPEAT(num_deflines, deflines, Dwg_Entity_HATCH_DefLine)
        {
          FIELD_BD (deflines[rcount].angle, 53);
          FIELD_2BD_1 (deflines[rcount].pt0, 43);
          FIELD_2BD_1 (deflines[rcount].offset, 45);
          FIELD_BS (deflines[rcount].num_dashes, 79);
          REPEAT2 (deflines[rcount].num_dashes, deflines[rcount].dashes, BITCODE_BD)
            {
              FIELD_BD (deflines[rcount].dashes[rcount2], 49);
            }
          END_REPEAT(deflines[rcount].dashes);
        }
      END_REPEAT(deflines);
    }

  if (FIELD_VALUE (has_derived))
      FIELD_BD (pixel_size, 47);
  FIELD_BL (num_seeds, 98);
  FIELD_2RD_VECTOR (seeds, num_seeds, 10);

  COMMON_ENTITY_HANDLE_DATA;
  HANDLE_VECTOR (boundary_handles, num_boundary_handles, 4, 330);

DWG_OBJECT_END

//pg.139
DWG_OBJECT(IDBUFFER)

  FIELD_RC (unknown, 0);
  FIELD_BL (num_obj_ids, 0);

  START_HANDLE_STREAM;
  FIELD_HANDLE (parenthandle, 4, 0);
  REACTORS(4);
  XDICOBJHANDLE(3);
  HANDLE_VECTOR(obj_ids, num_obj_ids, 4, 330);

DWG_OBJECT_END

//pg.204 20.4.80
DWG_ENTITY(IMAGE)

  FIELD_BL (class_version, 90);
  FIELD_3DPOINT (pt0, 10);
  FIELD_3DPOINT (uvec, 11);
  FIELD_3DPOINT (vvec, 12);
  FIELD_RD (size.width, 13);
  FIELD_RD (size.height, 14);
  FIELD_BS (display_props, 70);
  FIELD_B (clipping, 280);
  FIELD_RC (brightness, 281);
  FIELD_RC (contrast, 282);
  FIELD_RC (fade, 283);
  SINCE(R_2010) {
    FIELD_B (clip_mode, 290); // 0 outside, 1 inside
  }
  FIELD_BS (clip_boundary_type, 71); // 1 rect, 2 polygon
  if (FIELD_VALUE(clip_mode) == 1)
    {
      FIELD_2RD (boundary_pt0, 14);
      FIELD_2RD (boundary_pt1, 14);
    }
  else
    {
      FIELD_BL (num_clip_verts, 91);
      FIELD_2RD_VECTOR(clip_verts, num_clip_verts, 14);
    }

  COMMON_ENTITY_HANDLE_DATA;
  FIELD_HANDLE (imagedef, 5, 0); // hard pointer
  FIELD_HANDLE (imagedefreactor, 3, 0); // hard owner

DWG_ENTITY_END

//pg.142 test-data/*/Leader_*.dwg
DWG_OBJECT(IMAGEDEF)

  FIELD_BL (class_version, 90);
  FIELD_2RD (image_size, 10);
  FIELD_T (file_path, 1);
  FIELD_B (is_loaded, 280);
  FIELD_RC (resunits, 281);
  FIELD_2RD (pixel_size, 11);

  START_HANDLE_STREAM;
  FIELD_HANDLE (parenthandle, 3, 0);
  REACTORS(4);
  XDICOBJHANDLE(3);

DWG_OBJECT_END

//PG.143
DWG_OBJECT(IMAGEDEF_REACTOR)

  FIELD_BL (class_version, 90);

  START_HANDLE_STREAM;
  FIELD_HANDLE (parenthandle, 4, 0);
  REACTORS(4);
  XDICOBJHANDLE(3);

DWG_OBJECT_END

//pg.144
DWG_OBJECT(LAYER_INDEX)

  FIELD_BL (timestamp1, 40);
  FIELD_BL (timestamp2, 40);
  FIELD_BL (num_entries, 0);
  REPEAT (num_entries, entries, Dwg_LAYER_entry)
    {
      FIELD_BL (entries[rcount].idxlong, 0);
      FIELD_T (entries[rcount].layer, 8);
    }
  END_REPEAT(entries)

  START_HANDLE_STREAM;
  FIELD_HANDLE (parenthandle, 4, 0);
  REACTORS(4);
  XDICOBJHANDLE(3);
  HANDLE_VECTOR(entry_handles, num_entries, ANYCODE, 0);

DWG_OBJECT_END

//pg.145
DWG_OBJECT(LAYOUT)

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
  FIELD_BS (paper_units, 72);
  FIELD_BS (plot_rotation, 73);
  FIELD_BS (plot_type, 74);
  FIELD_2BD_1 (window_min, 48);
  FIELD_2BD_1 (window_max, 140);

  VERSIONS(R_13, R_2000) {
    FIELD_TV (plot_view_name, 6);
  }

  FIELD_BD (real_world_units, 142);
  FIELD_BD (drawing_units, 143);
  FIELD_T (current_style_sheet, 7);
  FIELD_BS (scale_type, 75);
  FIELD_BD (scale_factor, 147);
  FIELD_2BD_1 (paper_image_origin, 148);

  SINCE(R_2004)
    {
      FIELD_BS (shade_plot_mode, 76);
      FIELD_BS (shade_plot_res_level, 77);
      FIELD_BS (shade_plot_custom_dpi, 78);
    }

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

  SINCE(R_2004) {
    FIELD_BL (num_viewports, 0);
  }

  START_HANDLE_STREAM;
  FIELD_HANDLE (parenthandle, 4, 0);
  REACTORS(4);
  XDICOBJHANDLE(3);

  SINCE(R_2004) {
    FIELD_HANDLE (plot_view, 5, 6);
  }
  SINCE(R_2007) {
    FIELD_HANDLE (visual_style, 4, 0);
  }
  FIELD_HANDLE (pspace_block_record, 4, 330);
  FIELD_HANDLE (last_viewport, 4, 331);
  FIELD_HANDLE (base_ucs, 5, 346);
  FIELD_HANDLE (named_ucs, 5, 345);

  SINCE(R_2004) {
    HANDLE_VECTOR(viewports, num_viewports, 4, 0);
  }

DWG_OBJECT_END

//pg.147
DWG_ENTITY(LWPLINE)

  FIELD_BS (flag, 70);

  if (FIELD_VALUE(flag) & 4)
    FIELD_BD (const_width, 43);
  if (FIELD_VALUE(flag) & 8)
    FIELD_BD (elevation, 38);
  if (FIELD_VALUE(flag) & 2)
    FIELD_BD (thickness, 39);
  if (FIELD_VALUE(flag) & 1)
    FIELD_3BD (normal, 210);

  FIELD_BL (num_points, 90);

  if (FIELD_VALUE(flag) & 16)
    FIELD_BL (num_bulges, 0);
  if (FIELD_VALUE(flag) & 32)
    FIELD_BL (num_widths, 0);

  VERSIONS(R_13, R_14) {
    FIELD_2RD_VECTOR (points, num_points, 10);
  }
  SINCE(R_2000) {
    FIELD_2DD_VECTOR (points, num_points, 10);
  }

  FIELD_VECTOR (bulges, BD, num_bulges, 42);
  REPEAT(num_widths, widths, Dwg_Entity_LWPLINE_width)
    {
      FIELD_BD (widths[rcount].start, 40);
      FIELD_BD (widths[rcount].end, 41);
    }
  END_REPEAT(widths)

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

//(74+varies) pg.149
DWG_ENTITY(OLE2FRAME)

  FIELD_BS (flag, 70);
  SINCE(R_2000) {
    FIELD_BS (mode, 0);
  }

  FIELD_BL (data_length, 0);
  FIELD_TF (data, FIELD_VALUE(data_length), 0);

  SINCE(R_2000) {
    FIELD_RC (unknown, 0);
  }

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

//pg.276
#if 0 /* no proxy subtypes yet */
DWG_ENTITY(PROXY_LWPOLYLINE)

  FIELD_RL (size);
  FIELD_BS (flag);

  if (FIELD_VALUE(flag) & 4)
    FIELD_BD (const_width);
  if (FIELD_VALUE(flag) & 8)
    FIELD_BD (elevation);
  if (FIELD_VALUE(flag) & 2)
    FIELD_BD (thickness);
  if (FIELD_VALUE(flag) & 1)
    FIELD_3BD (normal);

  FIELD_BL (num_points);

  if (FIELD_VALUE(flag) & 16)
    FIELD_BL (num_bulges);
  if (FIELD_VALUE(flag) & 32)
    FIELD_BL (num_widths);

  VERSIONS(R_13, R_14) {
    FIELD_2RD_VECTOR (points, num_points);
  }

  SINCE(R_2000) {
    FIELD_2DD_VECTOR (points, num_points);
  }

  FIELD_VECTOR (bulges, BD, num_bulges);
  REPEAT(num_widths, widths, Dwg_Entity_LWPLINE_width)
    {
      FIELD_BD (widths[rcount].start);
      FIELD_BD (widths[rcount].end);
    }
  END_REPEAT(widths)

  FIELD_RC (unknown_1);
  FIELD_RC (unknown_2);
  FIELD_RC (unknown_3);

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END
#endif /* no proxy subtypes yet */

//(498) pg.149 r2000+
DWG_ENTITY(PROXY_ENTITY)

  FIELD_BL (class_id, 91);
  PRE(R_2018)
  {
    FIELD_BL (version, 95);
  }
  SINCE(R_2018)
  { // if encode from earlier: maint_version = version<<16 + acad version
    FIELD_BL (version, 71);
    FIELD_BL (maint_version, 97);
  }
  SINCE(R_2000)
  {
    FIELD_B (from_dxf, 70); // Original Data Format: 0 dwg, 1 dxf
  }

  LOG_INFO("TODO PROXY_ENTITY data\n");
  /*
  //TODO: figure out how to deal with the arbitrary size vector databits described on the spec
  FIELD_RC (*data);

  START_HANDLE_STREAM;
  FIELD_HANDLE (parenthandle, 4, 0);
  REACTORS(4);
  XDICOBJHANDLE(3);
  FIELD_MS (size);

  COMMON_ENTITY_HANDLE_DATA;
  */

DWG_ENTITY_END

//(499) pg.149 r2000+
DWG_OBJECT(PROXY)

  FIELD_BL (class_id, 91);
  PRE(R_2018)
  {
    FIELD_BL (version, 95);
  }
  SINCE(R_2018)
  { // if encode from earlier: maint_version = version<<16 + acad version
    FIELD_BL (version, 71);
    FIELD_BL (maint_version, 97);
  }
  SINCE(R_2000)
  {
    FIELD_B (from_dxf, 70); // Original Data Format: 0 dwg, 1 dxf
  }

  LOG_INFO("TODO PROXY_OBJECT data\n");
  /*
  //TODO: figure out how to deal with the arbitrary size vector databits
  FIELD_RC (*data);
  START_HANDLE_STREAM;
  FIELD_HANDLE (parenthandle, 4, 0);
  REACTORS(4);
  XDICOBJHANDLE(3);
  */

DWG_OBJECT_END

DWG_OBJECT(OBJECTCONTEXTDATA)

  SINCE(R_2010) {
    IF_ENCODE_FROM_EARLIER {
      FIELD_VALUE(class_version) = 3;
    }
    FIELD_BS (class_version, 0);
  }
  FIELD_B (has_file, 0);
  FIELD_B (defaultflag, 0);

DWG_OBJECT_END

// 20.4.99 Value, page 241. for FIELD and TABLE
#define Table_Value(value) \
  SINCE(R_2007) \
    { \
      FIELD_BL (value.flags, 93); \
    } \
  FIELD_BL (value.data_type, 90); \
  switch (FIELD_VALUE(value.data_type)) \
    { \
    case 0: /* kUnknown */ \
      FIELD_BL (value.data_long, 0); \
      break; \
    case 1: /* kLong */ \
      FIELD_BL (value.data_long, 0); \
      break; \
    case 2: /* kDouble */ \
      FIELD_BD (value.data_double, 0); \
      break; \
    case 4: /* kString */ \
      FIELD_T (value.data_string, 0); \
      break; \
    case 8: /* kDate */ \
      FIELD_BL (value.data_size, 0); \
      FIELD_TF (value.data_date, FIELD_VALUE(value.data_size), 0); \
      break; \
    case 16: /* kPoint */ \
      FIELD_BL (value.data_size, 0); \
      FIELD_2RD (value.data_point, 0); \
      break; \
    case 32: /* k3dPoint */ \
      FIELD_BL (value.data_size, 0); \
      FIELD_3RD (value.data_3dpoint, 0); \
      break; \
    case 64: /* kObjectId */ \
      /* data is a HANDLE */ \
      /* read from appropriate place in handles section */ \
      break; \
    case 128: /* kBuffer */ \
      LOG_ERROR("Unknown data type in TABLE entity: \"kBuffer\".\n") \
        break; \
    case 256: /* kResBuf */ \
      LOG_ERROR("Unknown data type in TABLE entity: \"kResBuf\".\n") \
        break; \
    case 512: /* kGeneral since r2007*/ \
      LOG_ERROR("Unknown data type in TABLE entity: \"kGeneral\".\n") \
        break; \
    default: \
      LOG_ERROR("Invalid data type in TABLE entity\n") \
        break; \
    } \
  SINCE(R_2007) \
    { \
      FIELD_BL (value.unit_type, 94); \
      FIELD_T (value.format_string, 300); \
      FIELD_T (value.value_string, 302); \
    }

DWG_OBJECT(FIELD)

  //LOG_INFO("TODO FIELD\n");
  FIELD_T (id, 1);
  FIELD_T (code, 2); // and code 3 for subsequent >255 chunks
  FIELD_BL (num_childhdl, 90);
  HANDLE_VECTOR (childhdl, num_childhdl, 360, 0);
  FIELD_BL (num_objects, 97);
  HANDLE_VECTOR (objects, num_objects, 331, 0);

  VERSION(R_2004) {
    FIELD_TV (format, 4);
  }

  FIELD_BL (evaluation_option, 91);
  FIELD_BL (filing_option, 92);
  FIELD_BL (field_state, 94);
  FIELD_BL (evaluation_status, 95);
  FIELD_BL (evaluation_error_code, 96);
  FIELD_T (evaluation_error_msg, 300);
  Table_Value(value)
  FIELD_T (value_string, 301); // and 9 for subsequent >255 chunks
  FIELD_T (value_string_length, 98);

  FIELD_BL (num_childval, 93);
  REPEAT_N((long)FIELD_VALUE(num_childval), childval, Dwg_FIELD_ChildValue)
    {
      FIELD_T (childval[rcount].key, 6);
      Table_Value(childval[rcount].value)
    }
  END_REPEAT(childval)

DWG_OBJECT_END

DWG_OBJECT(FIELDLIST)

  SINCE(R_2018) {
    FIELD_BL (num_fields, 0);
    FIELD_B (unknown, 0); // has handles?
    START_HANDLE_STREAM;
    HANDLE_VECTOR (field_handles, num_fields, 0, 330);
  }

DWG_OBJECT_END

DWG_OBJECT(GEODATA)

  FIELD_BL (class_version, 90); //1 for r2009, 2 for r2010 (default)
  FIELD_HANDLE (host_block, 4, 330);
  FIELD_BS (coord_type, 70); // 0 unknown, local grid 1, projected grid 2, geographic (defined by latitude/longitude) 3 (default)
  SINCE(R_2010)
    {
      FIELD_3BD (design_pt, 10);
      FIELD_3BD (ref_pt, 11);
      FIELD_BD (unit_scale_horiz, 40);
      FIELD_BL (units_value_horiz, 91);
      FIELD_BD (unit_scale_vert, 41);
      FIELD_BL (units_value_vert, 92);
      FIELD_3BD (up_dir, 210);
      FIELD_3BD (north_dir, 12);
      FIELD_BL (scale_est, 95); // None = 1 (default), User specified scale factor = 2, Grid scale at reference point = 3, Prismodial = 4
      FIELD_BD (user_scale_factor, 141);
      FIELD_B (sea_level_corr, 294);
      FIELD_BD (sea_level_elev, 142);
      FIELD_BD (coord_proj_radius, 143);
      FIELD_T (coord_system_def, 0);
      FIELD_T (geo_rss_tag, 302);
    }
    VERSIONS(R_2007, R_2010)// r2009-2010 really, class_version 1
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
  REPEAT_N(FIELD_VALUE(num_geomesh_pts), geomesh_pts, Dwg_GEODATA_meshpt)
    {
      FIELD_2RD (geomesh_pts[rcount].source_pt, 13);
      FIELD_2RD (geomesh_pts[rcount].dest_pt, 14);
    }
  END_REPEAT(geomesh_pts);
  FIELD_BL (num_geomesh_faces, 96);
  REPEAT_N(FIELD_VALUE(num_geomesh_faces), geomesh_faces, Dwg_GEODATA_meshface)
    {
      FIELD_BL (geomesh_faces[rcount].face1, 97);
      FIELD_BL (geomesh_faces[rcount].face2, 98);
      FIELD_BL (geomesh_faces[rcount].face3, 99);
      FIELD_BL (geomesh_faces[rcount].face4, 0);
    }
  END_REPEAT(geomesh_faces);
  UNTIL(R_2007) // r2009, class_version 1 really
    {
      FIELD_BL (scale_est, 0);
      FIELD_BD (user_scale_factor, 0);
      FIELD_B (sea_level_corr, 0);
      FIELD_BD (sea_level_elev, 0);
      FIELD_BD (coord_proj_radius, 143);
    }

DWG_OBJECT_END

//pg.220, 20.4.91
DWG_OBJECT(RASTERVARIABLES)

  FIELD_BL (class_version, 90);
  FIELD_BS (display_frame, 70);
  FIELD_BS (display_quality, 71);
  FIELD_BS (units, 72);

  START_HANDLE_STREAM;
  FIELD_HANDLE (parenthandle, 4, 0);
  REACTORS(4);
  XDICOBJHANDLE(3);

DWG_OBJECT_END

// 20.4.93 page 221
DWG_OBJECT(SORTENTSTABLE)

  FIELD_BL (num_ents, 0);

  START_HANDLE_STREAM;
  HANDLE_VECTOR (sort_handles, num_ents, ANYCODE, 0);
  FIELD_HANDLE (parenthandle, 4, 0);
  REACTORS(4);
  XDICOBJHANDLE(3);
  UNTIL(R_2007) {
    FIELD_HANDLE (owner_handle, 4, 0);
  }
  HANDLE_VECTOR (ents, num_ents, 4, 0);

DWG_OBJECT_END

//pg.222, 20.4.94 to clip external references
DWG_OBJECT(SPATIAL_FILTER)

  FIELD_BS (num_points, 70);
  FIELD_2RD_VECTOR (points, num_points, 10);
  FIELD_3BD (extrusion, 210);
  FIELD_3BD (clip_bound_origin, 10);
  FIELD_BS (display_boundary, 71);
  FIELD_BS (front_clip_on, 72);
  if (FIELD_VALUE(front_clip_on) == 1)
    FIELD_BD (front_clip_dist, 40);

  FIELD_BS (back_clip_on, 73);
  if (FIELD_VALUE(back_clip_on) == 1)
    FIELD_BD (back_clip_dist, 41);

  FIELD_VECTOR_N (inverse_block_transform, BD, 12, 40);
  FIELD_VECTOR_N (clip_bound_transform, BD, 12, 40);

  START_HANDLE_STREAM;
  FIELD_HANDLE (parenthandle, 4, 0);
  REACTORS(4);
  XDICOBJHANDLE(3);

DWG_OBJECT_END

//pg.153 TODO (Unhandled)
DWG_OBJECT(SPATIAL_INDEX)

  FIELD_BL (timestamp1, 0);
  FIELD_BL (timestamp2, 0);

  //TODO: parse this: "unknown X rest of bits to handles"
  SINCE(R_2007) {
    START_HANDLE_STREAM;
    FIELD_HANDLE (parenthandle, 4, 0);
    REACTORS(4);
    XDICOBJHANDLE(3);
  }

DWG_OBJECT_END

//pg.229 20.4.96
DWG_ENTITY(TABLE)

  SINCE(R_2010)
    {
      FIELD_RC (unknown_rc, 0);
      FIELD_HANDLE (unknown_h, 5, 0);
      FIELD_BL (unknown_bl, 0);
      VERSION(R_2010)
        FIELD_B (unknown_b, 0); // default 1
      VERSION(R_2013)
        FIELD_BL (unknown_bl1, 0);

      //below as 20.4.96.2 AcDbTableContent: 20.4.97
    }

  FIELD_3BD (insertion_point, 10);
    
  VERSIONS(R_13, R_14) {
    FIELD_3BD_1 (scale, 41);
  }
  
  SINCE(R_2000)
    {
      FIELD_BB (data_flags, 0);
      switch (FIELD_VALUE(data_flags))
        {
          case 0:
            FIELD_VALUE(scale.x) = 1.0;
            FIELD_DD (scale.y, FIELD_VALUE (scale.x), 42);
            FIELD_DD (scale.z, FIELD_VALUE (scale.x), 43);
            break;
          case 1:
            FIELD_VALUE(scale.x) = 1.0;
            FIELD_DD (scale.y, 1.0, 42);
            FIELD_DD (scale.z, 1.0, 43);
            break;
          case 2:
            FIELD_RD (scale.x, 41);
            FIELD_VALUE(scale.y) = FIELD_VALUE (scale.x);
            FIELD_VALUE(scale.z) = FIELD_VALUE (scale.x);
            break;
          case 3:
            FIELD_VALUE(scale.x) = 1.0;
            FIELD_VALUE(scale.y) = 1.0;
            FIELD_VALUE(scale.z) = 1.0;
            break;
          default:
            LOG_ERROR("Invalid data_flags in TABLE entity\n")
            break;
        }
    }
  
  FIELD_BD (rotation, 50);
  FIELD_3BD (extrusion, 210);
  FIELD_B (has_attribs, 66);
  
  SINCE(R_2004) {
    FIELD_BL (owned_object_count, 0);
  }
  
  FIELD_BS (flag_for_table_value, 90);
  FIELD_3BD (horiz_direction, 11);
  FIELD_BL (num_cols, 92);
  FIELD_BL (num_rows, 91);
  FIELD_VECTOR (col_widths, BD, num_cols, 142);
  FIELD_VECTOR (row_heights, BD, num_rows, 141);
  FIELD_VALUE(num_cells) = FIELD_VALUE(num_rows) * FIELD_VALUE(num_cols);
  REPEAT(num_cells, cells, Dwg_TABLE_Cell)
    {
      FIELD_BS (cells[rcount].type, 171);
      FIELD_RC (cells[rcount].flags, 172);
      FIELD_B (cells[rcount].merged_value, 173);
      FIELD_B (cells[rcount].autofit_flag, 174);
      FIELD_BL (cells[rcount].merged_width_flag, 175);
      FIELD_BL (cells[rcount].merged_height_flag, 176);
      FIELD_BD (cells[rcount].rotation_value, 145);
  
      if (FIELD_VALUE(cells[rcount].type) == 1)
        { /* text cell */
          FIELD_T (cells[rcount].text_string, 1);
        }
      if (FIELD_VALUE(cells[rcount].type) == 2)
        { /* block cell */
          FIELD_BD (cells[rcount].block_scale, 144);
          FIELD_B (cells[rcount].additional_data_flag, 0);
          if (FIELD_VALUE(cells[rcount].additional_data_flag) == 1)
            {
              FIELD_BS (cells[rcount].attr_def_count, 179);
              FIELD_BS (cells[rcount].attr_def_index, 0);
              FIELD_T (cells[rcount].attr_def_text, 300);
              //total_attr_def_count += FIELD_VALUE (cells[rcount].attr_def_count);
            }
        }
      if (FIELD_VALUE(cells[rcount].type) == 1 ||
          FIELD_VALUE(cells[rcount].type) == 2)
        { /* common to both text and block cells */
          FIELD_B (cells[rcount].additional_data_flag, 0);
          if (FIELD_VALUE(cells[rcount].additional_data_flag) == 1)
            {
              BITCODE_BL cell_flag;
              FIELD_BL (cells[rcount].cell_flag_override, 177);
              cell_flag = FIELD_VALUE(cells[rcount].cell_flag_override);
              FIELD_RC (cells[rcount].virtual_edge_flag, 178);
  
              if (cell_flag & 0x01)
                FIELD_RS (cells[rcount].cell_alignment, 170);
              if (cell_flag & 0x02)
                FIELD_B (cells[rcount].background_fill_none, 283);
              if (cell_flag & 0x04)
                FIELD_CMC (cells[rcount].background_color, 63);
              if (cell_flag & 0x08)
                FIELD_CMC (cells[rcount].content_color, 64);
              if (cell_flag & 0x10)
                FIELD_HANDLE (cells[rcount].text_style, 5, 7);
              if (cell_flag & 0x20)
                FIELD_BD (cells[rcount].text_height, 140);
              if (cell_flag & 0x00040)
                FIELD_CMC (cells[rcount].top_grid_color, 69);
              if (cell_flag & 0x00400)
                FIELD_BS (cells[rcount].top_grid_linewt, 279);
              if (cell_flag & 0x04000)
                FIELD_BS (cells[rcount].top_visibility, 289);
              if (cell_flag & 0x00080)
                FIELD_CMC (cells[rcount].right_grid_color, 65);
              if (cell_flag & 0x00800)
                FIELD_BS (cells[rcount].right_grid_linewt, 275);
              if (cell_flag & 0x08000)
                FIELD_BS (cells[rcount].right_visibility, 285);
              if (cell_flag & 0x00100)
                FIELD_CMC (cells[rcount].bottom_grid_color, 66);
              if (cell_flag & 0x01000)
                FIELD_BS (cells[rcount].bottom_grid_linewt, 276);
              if (cell_flag & 0x10000)
                FIELD_BS (cells[rcount].bottom_visibility, 286);
              if (cell_flag & 0x00200)
                FIELD_CMC (cells[rcount].left_grid_color, 68);
              if (cell_flag & 0x02000)
                FIELD_BS (cells[rcount].left_grid_linewt, 278);
              if (cell_flag & 0x20000)
                FIELD_BS (cells[rcount].left_visibility, 288);
  
              FIELD_BL (cells[rcount].unknown, 0);

              // 20.4.99 Value, page 241
              Table_Value(cells[rcount].value)
            }
        }
    }
  END_REPEAT(cells);
  /* End Cell Data (remaining data applies to entire table)*/
  
  /* COMMON: */
  
  FIELD_B (has_table_overrides, 0);
  if (FIELD_VALUE(has_table_overrides))
    {
      BITCODE_BL table_flag;
      FIELD_BL (table_flag_override, 93);
      table_flag = FIELD_VALUE(table_flag_override);
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
        FIELD_CMC (title_row_color, 64);
      if (table_flag & 0x0040)
        FIELD_CMC (header_row_color, 64);
      if (table_flag & 0x0080)
        FIELD_CMC (data_row_color, 64);
      if (table_flag & 0x0100)
        FIELD_B (title_row_fill_none, 283);
      if (table_flag & 0x0200)
        FIELD_B (header_row_fill_none, 283);
      if (table_flag & 0x0400)
        FIELD_B (data_row_fill_none, 283);
      if (table_flag & 0x0800)
        FIELD_CMC (title_row_fill_color, 63);
      if (table_flag & 0x1000)
        FIELD_CMC (header_row_fill_color, 63);
      if (table_flag & 0x2000)
        FIELD_CMC (data_row_fill_color, 63);
      if (table_flag & 0x4000)
        FIELD_BS (title_row_align, 170);
      if (table_flag & 0x8000)
        FIELD_BS (header_row_align, 170);
      if (table_flag & 0x10000)
        FIELD_BS (data_row_align, 170);
      if (table_flag & 0x20000)
        FIELD_HANDLE (title_text_style, 5, 7);
      if (table_flag & 0x40000)
        FIELD_HANDLE (header_text_style, 5, 7); // doc error
      if (table_flag & 0x80000)
        FIELD_HANDLE (data_text_style, 5, 7); // doc error
      if (table_flag & 0x100000)
        FIELD_BD (title_row_height, 140);
      if (table_flag & 0x200000)
        FIELD_BD (header_row_height, 140);
      if (table_flag & 0x400000)
        FIELD_BD (data_row_height, 140);
    }
  
  FIELD_B (has_border_color_overrides, 0);
  if (FIELD_VALUE(has_border_color_overrides))
    {
      BITCODE_BL border_color;
      FIELD_BL (border_color_overrides_flag, 94);
      border_color = FIELD_VALUE(border_color_overrides_flag);
      if (border_color & 0x0001)
        FIELD_CMC (title_horiz_top_color, 64);
      if (border_color & 0x0002)
        FIELD_CMC (title_horiz_ins_color, 65);
      if (border_color & 0x0004)
        FIELD_CMC (title_horiz_bottom_color, 66);
      if (border_color & 0x0008)
        FIELD_CMC (title_vert_left_color, 63);
      if (border_color & 0x0010)
        FIELD_CMC (title_vert_ins_color, 68);
      if (border_color & 0x0020)
        FIELD_CMC (title_vert_right_color, 69);
      if (border_color & 0x0040)
        FIELD_CMC (header_horiz_top_color, 64);
      if (border_color & 0x0080)
        FIELD_CMC (header_horiz_ins_color, 65);
      if (border_color & 0x0100)
        FIELD_CMC (header_horiz_bottom_color, 66);
      if (border_color & 0x0200)
        FIELD_CMC (header_vert_left_color, 63);
      if (border_color & 0x0400)
        FIELD_CMC (header_vert_ins_color, 68);
      if (border_color & 0x0800)
        FIELD_CMC (header_vert_right_color, 69);
      if (border_color & 0x1000)
        FIELD_CMC (data_horiz_top_color, 64);
      if (border_color & 0x2000)
        FIELD_CMC (data_horiz_ins_color, 65);
      if (border_color & 0x4000)
        FIELD_CMC (data_horiz_bottom_color, 66);
      if (border_color & 0x8000)
        FIELD_CMC (data_vert_left_color, 63);
      if (border_color & 0x10000)
        FIELD_CMC (data_vert_ins_color, 68);
      if (border_color & 0x20000)
        FIELD_CMC (data_vert_right_color, 69);
    }
  
  FIELD_B (has_border_lineweight_overrides, 0);
  if (FIELD_VALUE(has_border_lineweight_overrides))
    {
      BITCODE_BL border_linewt;
      FIELD_BL (border_lineweight_overrides_flag, 95);
      border_linewt = FIELD_VALUE(border_lineweight_overrides_flag);
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
  if (FIELD_VALUE(has_border_visibility_overrides))
    {
      BITCODE_BL border_visibility;
      FIELD_BL (border_visibility_overrides_flag, 96);
      border_visibility = FIELD_VALUE(border_visibility_overrides_flag);
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
  
  FIELD_HANDLE (block_header, 5, 2);
  
  VERSIONS(R_13, R_2000)
    {
      if (FIELD_VALUE(has_attribs))
        {
          FIELD_HANDLE (first_attrib, 4, 0);
          FIELD_HANDLE (last_attrib, 4, 0);
        }
    }
  
  SINCE(R_2004)
    {
      HANDLE_VECTOR (attribs, owned_object_count, 4, 0)
    }
  
  if (FIELD_VALUE(has_attribs))
    FIELD_HANDLE (seqend, 3, 0);
  
  FIELD_HANDLE (table_style_id, 5, 342);

  REPEAT(num_cells, cells, Dwg_TABLE_Cell)
    {
      if (FIELD_VALUE(cells[rcount].type) == 1)
        { /* text cell */
          FIELD_HANDLE (cells[rcount].cell_handle, 5, 344);
        }
      else
        { /* block cell */
          FIELD_HANDLE (cells[rcount].cell_handle, 5, 340);
        }
  
      if (FIELD_VALUE(cells[rcount].type) == 2 &&
          FIELD_VALUE(cells[rcount].additional_data_flag) == 1)
        {
          HANDLE_VECTOR (cells[rcount].attr_def_id, cells[rcount].attr_def_count, 4, 331);
        }
  
      if (FIELD_VALUE(cells[rcount].additional_data_flag2) == 1 &&
          FIELD_VALUE(cells[rcount].cell_flag_override) & 0x08)
        {
          FIELD_HANDLE (cells[rcount].text_style_override, ANYCODE, 7);
        }
    }
  END_REPEAT(cells);
  
  if (FIELD_VALUE(has_table_overrides))
    {
      BITCODE_BL table_flag;
      table_flag = FIELD_VALUE(table_flag_override);
      if (table_flag & 0x20000)
        FIELD_HANDLE (title_row_style_override, ANYCODE, 7);
      if (table_flag & 0x40000)
        FIELD_HANDLE (header_row_style_override, ANYCODE, 7);
      if (table_flag & 0x80000)
        FIELD_HANDLE (data_row_style_override, ANYCODE, 7);
    }

  SINCE(R_2010)
    {
      //... p237
      LOG_WARN("TODO TABLE r2010+")

      FIELD_BS (unknown_bs, 0); //default 38
      FIELD_3BD (hor_dir, 11);
      FIELD_BL (has_break_data, 0); //BL or B?
      if (FIELD_VALUE(has_break_data))
        {
          FIELD_BL (break_flag, 0);
          FIELD_BL (break_flow_direction, 0);
          FIELD_BD (break_spacing, 0);
          FIELD_BL (break_unknown1, 0);
          FIELD_BL (break_unknown2, 0);
          FIELD_BL (num_break_heights, 0);
          REPEAT(num_break_heights, break_heights, Dwg_TABLE_BreakHeight)
            {
              FIELD_3BD (break_heights[rcount].position, 0);
              FIELD_BD (break_heights[rcount].height, 0);
              FIELD_BL (break_heights[rcount].flag, 0);
            }
          END_REPEAT(break_heights);
        }
      FIELD_BL (num_break_rows, 0);
      REPEAT(num_break_rows, break_rows, Dwg_TABLE_BreakRow)
        {
          FIELD_3BD (break_rows[rcount].position, 0);
          FIELD_BL (break_rows[rcount].start, 0);
          FIELD_BL (break_rows[rcount].end, 0);
        }
      END_REPEAT(break_rows);
    }

DWG_ENTITY_END

// 20.4.101.3 Content format for TABLECONTENT and Cell_Style_Field
#define Content_Format(fmt) \
      FIELD_BL (fmt.property_override_flags, 90); \
      FIELD_BL (fmt.property_flags, 91); \
      FIELD_BL (fmt.value_data_type, 92); \
      FIELD_BL (fmt.value_unit_type, 93); \
      FIELD_T (fmt.value_format_string, 300); \
      FIELD_BD (fmt.rotation, 40); \
      FIELD_BD (fmt.block_scale, 140); \
      FIELD_BL (fmt.cell_align, 94); \
      FIELD_CMC (fmt.content_color, 62); \
      FIELD_HANDLE (fmt.text_style, 3, 92); \
      FIELD_BD (fmt.text_height, 92)

// Cell style 20.4.101.4 for TABLE, TABLECONTENT and CELLSTYLEMAP
#define Cell_Style_Fields(sty) \
      FIELD_BL (sty.type, 90); \
      FIELD_BS (sty.data_flags, 170); \
      FIELD_BL (sty.property_override_flags, 91); \
      FIELD_BL (sty.merge_flags, 92); \
      FIELD_CMC (sty.background_color, 62); \
      FIELD_BL (sty.content_layout, 93); \
      Content_Format(sty.content_format); \
      FIELD_BS (sty.margin_override_flags, 171); \
      FIELD_BD (sty.vert_margin, 40); \
      FIELD_BD (sty.horiz_margin, 40); \
      FIELD_BD (sty.bottom_margin, 40); \
      FIELD_BD (sty.right_margin, 40); \
      FIELD_BD (sty.margin_horiz_spacing, 40); \
      FIELD_BD (sty.margin_vert_spacing, 40); \
      FIELD_BL (sty.num_borders, 94); /* 0-6 */ \
      REPEAT2(sty.num_borders, sty.border, Dwg_BorderStyle) \
        { \
          FIELD_BL (sty.border[rcount2].edge_flags, 95); \
          FIELD_BL (sty.border[rcount2].border_property_overrides_flag, 90); \
          FIELD_BL (sty.border[rcount2].border_type, 91); \
          FIELD_CMC (sty.border[rcount2].color, 62); \
          FIELD_BL (sty.border[rcount2].linewt, 92); \
          FIELD_HANDLE (sty.border[rcount2].line_type, 3, 340); \
          FIELD_BL (sty.border[rcount2].invisible, 93); \
          FIELD_BD (sty.border[rcount2].double_line_spacing, 93); \
        } \
      END_REPEAT (sty.border)

//pg.237 20.4.97
DWG_OBJECT(TABLECONTENT)

  FIELD_T (ldata.name, 1);
  FIELD_T (ldata.desc, 300);

  FIELD_BL (tdata.num_cols, 90);
  REPEAT(tdata.num_cols, tdata.cols, Dwg_TableDataColumn)
    {
      FIELD_T (tdata.cols[rcount].name, 300);
      FIELD_BL (tdata.cols[rcount].custom_data, 91);
      Cell_Style_Fields(tdata.cols[rcount].cell_style);
    }
  END_REPEAT(tdata.cols);
  FIELD_BL (tdata.num_rows, 90);
  REPEAT(tdata.num_rows, tdata.rows, Dwg_TableRow)
    {
      #define row tdata.rows[rcount]
      FIELD_BL (row.num_cells, 90);
      REPEAT2(row.num_cells, row.cells, Dwg_TableCell)
        {
          #define cell row.cells[rcount2]
          FIELD_BL (cell.flag, 90);
          FIELD_T (cell.tooltip, 300);
          FIELD_BL (cell.customdata, 91);
          FIELD_BL (cell.num_customdata_items, 90);
          REPEAT3(cell.num_customdata_items, cell.customdata_items, Dwg_TABLE_CustomDataItem)
            {
              FIELD_T (cell.customdata_items[rcount3].name, 300);
              Table_Value(cell.customdata_items[rcount3].value);
            }
          END_REPEAT(cell.customdata_items);
          FIELD_BL (cell.has_linked_data, 92);
          if (FIELD_VALUE(cell.has_linked_data))
            {
              FIELD_HANDLE (cell.data_link, 5, 340);
              FIELD_BL (cell.num_rows, 93);
              FIELD_BL (cell.num_cols, 94);
              FIELD_BL (cell.unknown, 96);
            }
          FIELD_BL (cell.num_cell_contents, 95);
          REPEAT3(cell.num_cell_contents, cell.cell_contents, Dwg_TableCellContent)
            {
              #define content tdata.rows[rcount].cells[rcount2].cell_contents[rcount3]

              FIELD_BL(content.type, 90);
              if (FIELD_VALUE(content.type) == 1)
                {
                  // 20.4.99 Value, page 241
                  Table_Value(content.value)
                }
              else if (FIELD_VALUE(content.type) == 2) { // Field
                FIELD_HANDLE (content.handle, 3, 340);
              }
              else if (FIELD_VALUE(content.type) == 4) { // Block
                FIELD_HANDLE (content.handle, 3, 340);
              }
              FIELD_BL (content.num_attrs, 91);
              REPEAT4(content.num_attrs, content.attrs, Dwg_TableCellContentAttr)
                {
                  #define attr content.attrs[rcount4]
                  FIELD_HANDLE (attr.attdef, 5, 330);
                  FIELD_T (attr.value, 301);
                  FIELD_BL (attr.index, 92);
                  #undef attr
                }
              END_REPEAT(content.attrs);
              if (FIELD_VALUE(content.has_content_format_overrides))
                {
                  Content_Format(content.content_format);
                }
              #undef content
            }
          END_REPEAT(cell.cell_contents);
          FIELD_BL (cell.style_id, 90);
          FIELD_BL (cell.has_geom_data, 91);
          if (FIELD_VALUE(cell.has_geom_data))
            {
              FIELD_BL (cell.geom_data_flag, 91);
              FIELD_BD (cell.unknown_d40, 40);
              FIELD_BD (cell.unknown_d41, 41);
              FIELD_BL (cell.has_cell_geom, 0);
              FIELD_HANDLE (cell.cell_geom_handle, ANYCODE, 0);
              if (FIELD_VALUE(cell.has_cell_geom))
                {
                  REPEAT_N(1, cell.geom_data, Dwg_CellContentGeometry)
                    {
#define geom cell.geom_data[0]
                      FIELD_3BD (geom.dist_top_left, 0); 
                      FIELD_3BD (geom.dist_center, 0); 
                      FIELD_BD (geom.content_width, 0); 
                      FIELD_BD (geom.width, 0); 
                      FIELD_BD (geom.height, 0); 
                      FIELD_BD (geom.unknown, 0);
#undef geom
                    }
                  END_REPEAT (cell.geom_data);
                }
            }
          #undef cell
        }
      END_REPEAT(row.cells);
      FIELD_BL (row.custom_data, 91);
      FIELD_BL (row.num_customdata_items, 90);
      REPEAT3(row.num_customdata_items, row.customdata_items, Dwg_TABLE_CustomDataItem)
        {
          FIELD_T (row.customdata_items[rcount3].name, 300);
          Table_Value(row.customdata_items[rcount3].value);
        }
      END_REPEAT(row.customdata_items);
      {
        Cell_Style_Fields(row.cell_style);
        FIELD_BL (row.style_id, 90);
        FIELD_BL (row.height, 40);
      }
      #undef row
    }
  END_REPEAT(tdata.rows);
  FIELD_BL (tdata.num_field_refs, 0);
  HANDLE_VECTOR (tdata.field_refs, tdata.num_field_refs, 3, 0);

  FIELD_BL (fdata.num_merged_cells, 90);
  REPEAT(fdata.num_merged_cells, fdata.merged_cells, Dwg_FormattedTableMerged)
    {
      #define merged fdata.merged_cells[rcount]
      FIELD_BL (merged.top_row, 91);
      FIELD_BL (merged.left_col, 92);
      FIELD_BL (merged.bottom_row, 93);
      FIELD_BL (merged.right_col, 94);
      #undef merged
    }
  END_REPEAT(fdata.merged_cells);

  START_HANDLE_STREAM;
  FIELD_HANDLE (table_style, 3, 340);
  
DWG_OBJECT_END

// pg.246 20.4.102
// added with r2008, backcompat with r2007
// unused
//#define DEBUG_CELLSTYLEMAP
#ifdef DEBUG_CELLSTYLEMAP
DWG_OBJECT(CELLSTYLEMAP)

  FIELD_BL (num_cells, 90);
  REPEAT(num_cells, cells, Dwg_CELLSTYLEMAP_Cell)
    {
      Cell_Style_Fields(cells[rcount].style);
      FIELD_BL (cells[rcount].id, 90);
      FIELD_BL (cells[rcount].type, 91);
      FIELD_T (cells[rcount].name, 300);
    }
  END_REPEAT (cells);

DWG_OBJECT_END
#endif
  
//pg.246 20.4.103
DWG_OBJECT(TABLEGEOMETRY)

  FIELD_BL (num_rows, 90);
  FIELD_BL (num_cols, 91);
  FIELD_BL (num_cells, 92);
  REPEAT(num_cells, cells, Dwg_TABLEGEOMETRY_Cell)
    {
      #define cell cells[rcount]
      FIELD_BL (cell.flag, 93);
      FIELD_BD (cell.width_w_gap, 40);
      FIELD_BD (cell.height_w_gap, 41);
      FIELD_HANDLE (cell.unknown, 3, 330);
      FIELD_BL (cell.num_geom_data, 94);
      REPEAT2(cell.num_geom_data, cell.geom_data, Dwg_CellContentGeometry)
        {
          #define geom cell.geom_data[rcount2]
          FIELD_3BD (geom.dist_top_left, 0); 
          FIELD_3BD (geom.dist_center, 0); 
          FIELD_BD (geom.content_width, 0); 
          FIELD_BD (geom.width, 0); 
          FIELD_BD (geom.height, 0); 
          FIELD_BD (geom.unknown, 0);
          #undef geom
        }
      END_REPEAT(cell.geom_data);
    }
  END_REPEAT(cells);

DWG_OBJECT_END

//(79 + varies) pg.247 20.4.104
DWG_OBJECT(XRECORD)

  FIELD_BL (num_databytes, 0);
  FIELD_XDATA (xdata, num_databytes);

  SINCE(R_2000) {
    FIELD_BS (cloning_flags, 280);
  }

  START_HANDLE_STREAM;
  FIELD_HANDLE (parenthandle, 4, 0); // 3 or 8
  REACTORS(4);
  XDICOBJHANDLE(3);

  DECODER
    {
      for (vcount=0;
           hdl_dat->byte < obj->tio.object->datpos + (obj->bitsize/8);
           vcount++)
        {
          FIELD_VALUE(objid_handles) = vcount
            ? realloc(FIELD_VALUE(objid_handles),
                                   (vcount+1) * sizeof(Dwg_Object_Ref))
            : malloc(sizeof(Dwg_Object_Ref));
          FIELD_HANDLE_N (objid_handles[vcount], vcount, ANYCODE, 0);
          if (!FIELD_VALUE(objid_handles[vcount]))
            break;
        }
      FIELD_VALUE(num_objid_handles) = vcount;
      FIELD_TRACE(num_objid_handles, BL);
    }
    #ifndef IS_DECODER
      HANDLE_VECTOR(objid_handles, num_objid_handles, 4, 0);
    #endif

DWG_OBJECT_END

//(80 + varies)
DWG_OBJECT(PLACEHOLDER)

  // no own data members
  START_HANDLE_STREAM;
  FIELD_HANDLE (parenthandle, 4, 0);
  REACTORS(4);
  XDICOBJHANDLE(3);

DWG_OBJECT_END

// just guessing:
// VBA_PROJECT (81 + varies)
//#define DEBUG_VBA_PROJECT
#ifdef DEBUG_VBA_PROJECT
DWG_OBJECT(VBA_PROJECT)

  FIELD_RL (num_bytes, 0)
  FIELD_TF (bytes, num_bytes, 0)

  START_HANDLE_STREAM;
  FIELD_HANDLE (parenthandle, 4, 0);
  REACTORS(4);
  XDICOBJHANDLE(3);

DWG_OBJECT_END
#endif

// SCALE (varies)
// 20.4.92 page 221
DWG_OBJECT(SCALE)

  FIELD_BS (flag, 70);
  FIELD_T (name, 300);
  FIELD_BD (paper_units, 140);
  FIELD_BD (drawing_units, 141);
  FIELD_B (has_unit_scale, 290);
      
  START_HANDLE_STREAM;
  FIELD_HANDLE (parenthandle, 4, 0);
  REACTORS(4);
  XDICOBJHANDLE(3);

DWG_OBJECT_END

/* pg. 157, 20.4.48 (varies)
   AcDbMLeader. broken leader_lines/points
 */
//#define DEBUG_MULTILEADER
#ifdef DEBUG_MULTILEADER
DWG_ENTITY(MULTILEADER)

  SINCE(R_2010)
    {
      IF_ENCODE_FROM_EARLIER {
        FIELD_VALUE(class_version) = 2;
      }
      FIELD_BS (class_version, 270); // default 2
      FIELD_BS (ctx.class_version, 70); // default 3
      FIELD_B (ctx.has_xdic_file, 0);
      FIELD_B (ctx.is_default, 290);
      FIELD_HANDLE (ctx.scale_handle, 5, 340);
    }

  FIELD_BL (ctx.num_leaders, 0);
  REPEAT(ctx.num_leaders, ctx.leaders, Dwg_Leader)
    {
#     define lev1 ctx.leaders[rcount]
      FIELD_B (lev1.is_valid, 290);  //1
      FIELD_B (lev1.num_lines, 291); //1
      FIELD_3BD (lev1.connection, 10);
      FIELD_3BD (lev1.direction, 11);
      FIELD_BL (lev1.num_breaks, 0);
      REPEAT2(lev1.num_breaks, lev1.breaks, Leader_Break)
        {
          FIELD_3BD (lev1.breaks[rcount2].start, 12);
          FIELD_3BD (lev1.breaks[rcount2].end, 13);
        }
      END_REPEAT (lev1.breaks);
      FIELD_BL (lev1.index, 90);
      FIELD_BD (lev1.landing_distance, 40); //ok
      // num_lines was missing
      REPEAT2(lev1.num_lines, lev1.lines, Leader_Line)
        {
#         define lev2 lev1.lines[rcount2]
          FIELD_BL (lev2.num_points, 0); //ok?
          REPEAT3(lev2.num_points, lev2.points, BITCODE_3BD)
            {
              FIELD_3BD (lev2.points[rcount3], 10); //nok
            }
          END_REPEAT (lev2.points);
          FIELD_BL (lev2.num_breaks, 0);
          FIELD_BL (lev2.segment_index, 90);
          REPEAT2(lev2.num_breaks, lev2.breaks, Leader_Break)
            {
              FIELD_3BD (lev2.breaks[rcount2].start, 11);
              FIELD_3BD (lev2.breaks[rcount2].end, 12);
            }
          END_REPEAT(lev2.breaks);
          FIELD_BL (lev2.index, 91);
	  SINCE (R_2010)
            {
              FIELD_BS (lev2.type, 170);
              FIELD_CMC (lev2.color, 92);
              FIELD_HANDLE (lev2.type_handle, 5, 340);
              FIELD_BL (lev2.weight, 171);
              FIELD_BD (lev2.arrow_size, 40);
              FIELD_HANDLE (lev2.arrow_handle, 5, 341);
              FIELD_BL (lev2.flags, 93);
            }
#         undef lev2
        }
      END_REPEAT (lev1.lines);
      SINCE (R_2010)
        {
          FIELD_BS (lev1.attach_dir, 271);
        }
#     undef lev1
    }
  END_REPEAT (ctx.leaders);

  FIELD_BD (ctx.scale, 40);
  FIELD_3BD (ctx.content_base, 10); //broken
  FIELD_BD (ctx.text_height, 41);
  FIELD_BD (ctx.arrow_size, 140);
  FIELD_BD (ctx.landing_gap, 145);
  FIELD_BS (ctx.text_left, 174);
  FIELD_BS (ctx.text_right, 175);
  FIELD_BS (ctx.text_align, 176);
  FIELD_BS (ctx.attach_type, 177);

  FIELD_B (ctx.has_text_content, 290);
  if (FIELD_VALUE (ctx.has_text_content))
    {
      FIELD_T (ctx.txt.label, 304);
      FIELD_3BD (ctx.txt.normal, 11);
      FIELD_HANDLE (ctx.txt.style, 5, 340);
      FIELD_3BD (ctx.txt.location, 12);
      FIELD_3BD (ctx.txt.direction, 13);
      FIELD_BD (ctx.txt.rotation, 42);
      FIELD_BD (ctx.txt.width, 43);
      FIELD_BD (ctx.txt.height, 44);
      FIELD_BD (ctx.txt.line_spacing_factor, 45);
      FIELD_BS (ctx.txt.line_spacing_style, 170);
      FIELD_CMC (ctx.txt.color, 90);
      FIELD_BS (ctx.txt.align, 171);
      FIELD_BS (ctx.txt.flow, 172);
      FIELD_CMC (ctx.txt.bg_color, 91);
      FIELD_BD (ctx.txt.bg_scale, 141);
      FIELD_BL (ctx.txt.bg_transparency, 92);
      FIELD_B (ctx.txt.is_bg_fill, 291);
      FIELD_B (ctx.txt.is_bg_mask_fill, 292);
      FIELD_BS (ctx.txt.col_type, 173);
      FIELD_B (ctx.txt.is_height_auto, 293);
      FIELD_BD (ctx.txt.col_width, 142);
      FIELD_BD (ctx.txt.col_gutter, 143);
      FIELD_B (ctx.txt.is_col_flow_reversed, 294);
      FIELD_BL (ctx.txt.num_col_sizes, 0);
      FIELD_VECTOR (ctx.txt.col_sizes, BD, ctx.txt.num_col_sizes, 144);
      FIELD_B (ctx.txt.word_break, 295);
      FIELD_B (ctx.txt.unknown, 0);
    }

  FIELD_B (ctx.has_content_block, 0);
  if (FIELD_VALUE (ctx.has_content_block))
    {
      FIELD_HANDLE (ctx.blk.block_table, 4, 341);
      FIELD_3BD (ctx.blk.normal, 14);
      FIELD_3BD (ctx.blk.location, 15);
      FIELD_3BD (ctx.blk.scale, 16);
      FIELD_BD (ctx.blk.rotation, 46);
      FIELD_CMC (ctx.blk.color, 93);
      for (rcount = 0; rcount < 16; rcount++)
        {
          FIELD_BD (ctx.blk.transform[rcount], 47);
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

  FIELD_HANDLE (leaderstyle, 5, 340);
  FIELD_BL (flags, 90); // override flags
  FIELD_BS (type, 170);
  FIELD_CMC (color, 91);
  FIELD_HANDLE (ltype, 5, 341);
  FIELD_BL (linewt, 171);
  FIELD_B (landing, 290);
  FIELD_B (dog_leg, 291);
  FIELD_BD (landing_dist, 41);
  FIELD_HANDLE (arrow_head, 5, 342);
  FIELD_BD (arrow_head_size, 42);
  FIELD_BS (style_content, 172);
  FIELD_HANDLE (text_style, 5, 343);
  FIELD_BS (text_left, 95);
  FIELD_BS (text_right, 95);
  FIELD_BS (text_angletype, 174);
  FIELD_BS (attach_type, 175);
  FIELD_CMC (text_color, 92);
  FIELD_B (text_frame, 292);
  FIELD_HANDLE (block_style, 5, 344);
  FIELD_CMC (block_color, 93);
  FIELD_3BD (block_scale, 10);
  FIELD_BD (block_rotation, 43);
  FIELD_BS (style_attachment, 176);
  FIELD_B (is_annotative, 293);

  VERSIONS (R_2000, R_2007)
    {
      FIELD_BL (num_arrowheads, 0);
      REPEAT(num_arrowheads, arrowheads, Leader_ArrowHead)
        {
          FIELD_BL (arrowheads->is_default, 94);
          FIELD_HANDLE (arrowheads->arrowhead, 5, 345);
        }
      END_REPEAT(arrowheads);
      FIELD_BL (num_blocklabels, 0);
      REPEAT(num_blocklabels, blocklabels, Leader_BlockLabel)
        {
          FIELD_HANDLE (blocklabels->attdef, 4, 330);
          FIELD_T (blocklabels->label_text, 302);
          FIELD_BS (blocklabels->ui_index, 177);
          FIELD_BD (blocklabels->width, 44);
        }
      END_REPEAT(blocklabels);
      FIELD_B (neg_textdir, 294);
      FIELD_BS (ipe_align, 178);
      FIELD_BS (justification, 179);
      FIELD_BD (scale_factor, 45);
    }

  SINCE (R_2010)
    {
      FIELD_BS (attach_dir, 271);
      FIELD_BS (attach_top, 273);
      FIELD_BS (attach_bottom, 272);
    }
  SINCE (R_2013) {
    FIELD_B (text_extended, 295);
  }

  //COMMON_ENTITY_HANDLE_DATA; //??

DWG_ENTITY_END
#endif

/* par 20.4.87 (varies) */
DWG_OBJECT(MLEADERSTYLE)

  SINCE (R_2010)
    {
      IF_ENCODE_FROM_EARLIER {
        FIELD_VALUE(class_version) = 2;
      }
      FIELD_BS (class_version, 179);
    }

  FIELD_BS (content_type, 170);
  FIELD_BS (mleader_order, 171);
  FIELD_BS (leader_order, 172);
  FIELD_BL (max_points, 90);
  FIELD_BD (first_seg_angle, 40);
  FIELD_BD (second_seg_angle, 41);
  FIELD_BS (type, 173);
  FIELD_CMC (line_color, 91);
  FIELD_BL (linewt, 92);
  FIELD_B (landing, 290);
  FIELD_BD (landing_gap, 42);
  FIELD_B (dog_leg, 291);
  FIELD_BD (landing_dist, 43);
  FIELD_T (description, 3);
  FIELD_BD (arrow_head_size, 44);
  FIELD_T (text_default, 300);
  FIELD_BS (attach_left, 174);
  FIELD_BS (attach_right, 178);
  FIELD_BS (text_angle_type, 175);
  FIELD_BS (text_align_type, 176);
  FIELD_CMC (text_color, 93);
  FIELD_BD (text_height, 45);
  FIELD_B (text_frame, 292);
  //is_new_format: if the object has extended data for APPID “ACAD_MLEADERVER”.
  if (FIELD_VALUE(is_new_format) || dat->version >= R_2010) {
    FIELD_B (text_always_left, 297);
  }
  FIELD_BD (align_space, 46);
  FIELD_CMC (block_color, 94);
  FIELD_BD (block_scale.x, 47);
  FIELD_BD (block_scale.y, 49);
  FIELD_BD (block_scale.z, 140);
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

  START_HANDLE_STREAM;
  // wrong documentation in the ODA. the handles are at the end.
  FIELD_HANDLE (line_type, 5, 340);
  FIELD_HANDLE (arrow_head, 5, 341);
  FIELD_HANDLE (text_style, 5, 342);
  FIELD_HANDLE (block, 5, 343);

DWG_OBJECT_END

////////////////////
// These variable objects are not described in the spec:
//

DWG_OBJECT(WIPEOUTVARIABLE)

  FIELD_BS (display_frame, 0);

  START_HANDLE_STREAM;
  FIELD_HANDLE (parenthandle, 3, 0);
  REACTORS(4);
  XDICOBJHANDLE(3);

DWG_OBJECT_END

// R2000+ picture. undocumented (varies)
DWG_ENTITY(WIPEOUT)

  FIELD_BL (class_version, 90);
  FIELD_3DPOINT (pt0, 10);
  FIELD_3DPOINT (uvec, 11);
  FIELD_3DPOINT (vvec, 12);
  FIELD_RD (size.width, 13);
  FIELD_RD (size.height, 23);
  FIELD_BS (display_props, 70);
  FIELD_B (clipping, 280);
  FIELD_RC (brightness, 281);
  FIELD_RC (contrast, 282);
  FIELD_RC (fade, 283);

  SINCE(R_2010) {
    FIELD_B (clip_mode, 290);
  }
  FIELD_BS (clip_boundary_type, 71); // 1 rect, 2 polygon
  if (FIELD_VALUE(clip_mode))
    {
      FIELD_2RD (boundary_pt0, 14);
      FIELD_2RD (boundary_pt1, 14);
    }
  else
    {
      FIELD_BL (num_clip_verts, 91);
      FIELD_2RD_VECTOR(clip_verts, num_clip_verts, 14);
    }

  COMMON_ENTITY_HANDLE_DATA;
  FIELD_HANDLE (imagedef, 5, 0);
  FIELD_HANDLE (imagedefreactor, 3, 0);

DWG_ENTITY_END

/* (varies) container to hold a raw class entity (none observed in the wild) */
DWG_ENTITY(UNKNOWN_ENT)

  FIELD_VALUE(num_bytes) = obj->bitsize / 8;
  FIELD_VALUE(num_bits)  = obj->bitsize % 8;

  FIELD_TF (bytes, FIELD_VALUE(num_bytes), 0);
  FIELD_VECTOR (bits, B, num_bits, 0);
  //COMMON_ENTITY_HANDLE_DATA; // including this

DWG_ENTITY_END

/* container to hold a raw class object (varies) */
DWG_OBJECT(UNKNOWN_OBJ)

  FIELD_VALUE(num_bytes) = obj->bitsize / 8;
  FIELD_VALUE(num_bits)  = obj->bitsize % 8;

  FIELD_TF (bytes, FIELD_VALUE(num_bytes), 0);
  FIELD_VECTOR (bits, B, num_bits, 0);

DWG_OBJECT_END

DWG_ENTITY(DUMMY)

  LOG_INFO("TODO DUMMY\n");
  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

DWG_ENTITY(LONG_TRANSACTION)

  LOG_INFO("TODO LONG_TRANSACTION\n");
  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

// r2007+
DWG_OBJECT(VISUALSTYLE)

  FIELD_HANDLE (dictionary, 5, 0);

DWG_OBJECT_END

/*
Those undocumented objects are stored as raw UNKNOWN_OBJ:

// r2000+
DWG_OBJECT(ARCALIGNEDTEXT)
DWG_OBJECT_END

//(varies) lots of fields in the DXF docs
DWG_OBJECT(MATERIAL)
DWG_OBJECT_END

DWG_OBJECT(DIMASSOC)
DWG_OBJECT_END

DWG_OBJECT(DBCOLOR)
DWG_OBJECT_END

DWG_OBJECT(TABLESTYLE)
DWG_OBJECT_END

DWG_OBJECT(SECTIONVIEWSTYLE)
DWG_OBJECT_END

DWG_OBJECT(DETAILVIEWSTYLE)
DWG_OBJECT_END

DWG_OBJECT(ASSOC2DCONSTRAINTGROUP)
DWG_OBJECT_END

DWG_OBJECT(ASSOCGEOMDEPENDENCY)
DWG_OBJECT_END

DWG_OBJECT(LEADEROBJECTCONTEXTDATA)
DWG_OBJECT_END
*/
