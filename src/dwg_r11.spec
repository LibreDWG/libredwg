/* -*- c -*- */
/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2018 Free Software Foundation, Inc.                  */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * dwg_r11.spec: pre-R13 DWG entities and objects specification
 * written by Reini Urban
 */

#include "spec.h"

/*(1)*/
DWG_ENTITY (TEXT);

  VERSIONS(R_13,R_14)
    {
      FIELD_BD(elevation);
      FIELD_2RD(insertion_pt);
      FIELD_2RD(alignment_pt);
      FIELD_3BD(extrusion);
      FIELD_BD(thickness);
      FIELD_BD(oblique_ang);
      FIELD_BD(rotation_ang);
      FIELD_BD(height);
      FIELD_BD(width_factor);
      FIELD_TV(text_value);
      FIELD_BS(generation);
      FIELD_BS(horiz_alignment);
      FIELD_BS(vert_alignment);
    }

  SINCE(R_2000)
    {
      /* We assume that the user (the client application)
      is responsible for taking care of properly updating the dataflags field
      which indicates which fields in the data structures are valid and which are
      undefined */

      FIELD_RC (dataflags);

      if (!(FIELD_VALUE(dataflags) & 0x01))
        {
          FIELD_RD(elevation);
        }

      FIELD_2RD(insertion_pt);

      if (!(FIELD_VALUE(dataflags) & 0x02))
        {
          FIELD_2DD(alignment_pt, 10.0, 20.0);
        }

      FIELD_BE(extrusion);
      FIELD_BT(thickness);

      if (!(FIELD_VALUE(dataflags) & 0x04))
        {
          FIELD_RD(oblique_ang);
        }

      if (!(FIELD_VALUE(dataflags) & 0x08))
        {
          FIELD_RD(rotation_ang);
        }

      FIELD_RD(height);

      if (!(FIELD_VALUE(dataflags) & 0x10))
        {
          FIELD_RD(width_factor);
        }

      FIELD_TV(text_value);

      if (!(FIELD_VALUE(dataflags) & 0x20))
        {
          FIELD_BS(generation);
        }

      if (!(FIELD_VALUE(dataflags) & 0x40))
        {
          FIELD_BS(horiz_alignment);
        }

      if (!(FIELD_VALUE(dataflags) & 0x80))
        {
          FIELD_BS(vert_alignment);
        }
    }

  COMMON_ENTITY_HANDLE_DATA;

  UNTIL(R_2007)
    {
      FIELD_HANDLE(style, ANYCODE);
    }

DWG_ENTITY_END

/*(2)*/
DWG_ENTITY(ATTRIB);

  VERSIONS(R_13,R_14)
    {
      FIELD_BD(elevation);
      FIELD_2RD(insertion_pt);
      FIELD_2RD(alignment_pt);
      FIELD_3BD(extrusion);
      FIELD_BD(thickness);
      FIELD_BD(oblique_ang);
      FIELD_BD(rotation_ang);
      FIELD_BD(height);
      FIELD_BD(width_factor);
      FIELD_TV(text_value);
      FIELD_BS(generation);
      FIELD_BS(horiz_alignment);
      FIELD_BS(vert_alignment);
    }

  FIELD_TV(tag);
  FIELD_BS(field_length);
  FIELD_RC(flags);

  COMMON_ENTITY_HANDLE_DATA;

  FIELD_HANDLE(style, 5);

DWG_ENTITY_END

/*(3)*/
DWG_ENTITY(ATTDEF);

  VERSIONS(R_13,R_14)
    {
      FIELD_BD(elevation);
      FIELD_2RD(insertion_pt);
      FIELD_2RD(alignment_pt);
      FIELD_3BD(extrusion);
      FIELD_BD(thickness);
      FIELD_BD(oblique_ang);
      FIELD_BD(rotation_ang);
      FIELD_BD(height);
      FIELD_BD(width_factor);
      FIELD_TV(default_value);
      FIELD_BS(generation);
      FIELD_BS(horiz_alignment);
      FIELD_BS(vert_alignment);
    }

  SINCE(R_2000)
    {
      /* We assume that the user (the client application)
      is responsible for taking care of properly updating the dataflags field
      which indicates which fields in the data structures are valid and which are
      undefined */

      FIELD_RC (dataflags);
      if (!(FIELD_VALUE(dataflags) & 0x01))
        {
          FIELD_RD(elevation);
        }
      FIELD_2RD(insertion_pt);
      if (!(FIELD_VALUE(dataflags) & 0x02))
        {
          FIELD_2DD(alignment_pt, 10.0, 20.0);
        }
      FIELD_BE(extrusion);
      FIELD_BT(thickness);
      if (!(FIELD_VALUE(dataflags) & 0x04))
        {
          FIELD_RD(oblique_ang);
        }
      if (!(FIELD_VALUE(dataflags) & 0x08))
        {
          FIELD_RD(rotation_ang);
        }
      FIELD_RD(height);
      if (!(FIELD_VALUE(dataflags) & 0x10))
        {
          FIELD_RD(width_factor);
        }
      FIELD_TV(default_value);
      if (!(FIELD_VALUE(dataflags) & 0x20))
        {
          FIELD_BS(generation);
        }
      if (!(FIELD_VALUE(dataflags) & 0x40))
        {
          FIELD_BS(horiz_alignment);
        }
      if (!(FIELD_VALUE(dataflags) & 0x80))
        {
          FIELD_BS(vert_alignment);
        }
    }

  FIELD_TV(tag);
  FIELD_BS(field_length);
  FIELD_RC(flags);

  SINCE(R_2007)
    {
      FIELD(lock_position_flag, B);
    }

  SINCE(R_2010)
    {
      FIELD_RC (version);
    }
  FIELD_TV(prompt);

  COMMON_ENTITY_HANDLE_DATA;

  FIELD_HANDLE(style, 5);

DWG_ENTITY_END

/*(4)*/
DWG_ENTITY(BLOCK);

  FIELD_TV(name);

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/*(5)*/
DWG_ENTITY(ENDBLK);

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/*(6)*/
DWG_ENTITY(SEQEND);

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/*(7)*/
DWG_ENTITY(INSERT);

  FIELD_3BD(ins_pt);

  VERSIONS(R_13,R_14)
    {
      FIELD_3BD(scale);
    }

  SINCE(R_2000)
    {
      DECODER
        {
          FIELD_BB(scale_flag);
          if (FIELD_VALUE(scale_flag) == 3)
            {
              FIELD_VALUE(scale.x) = 1.0;
              FIELD_VALUE(scale.y) = 1.0;
              FIELD_VALUE(scale.z) = 1.0;
            }
          else if (FIELD_VALUE(scale_flag) == 1)
            {
              FIELD_VALUE(scale.x) = 1.0;
              FIELD_DD(scale.y, 1.0);
              FIELD_DD(scale.z, 1.0);
            }
          else if (FIELD_VALUE(scale_flag) == 2)
            {
              FIELD_RD(scale.x); 
              FIELD_VALUE(scale.y) = FIELD_VALUE(scale.x);
              FIELD_VALUE(scale.z) = FIELD_VALUE(scale.x);
            }
          else //if (FIELD_VALUE(scale_flag) == 0)
            {
              FIELD_RD(scale.x);
              FIELD_DD(scale.y, FIELD_VALUE(scale.x));
              FIELD_DD(scale.z, FIELD_VALUE(scale.x));
            }
        }

      ENCODER
        {
          if (FIELD_VALUE(scale.x)==1.0 && FIELD_VALUE(scale.y)==1.0 && FIELD_VALUE(scale.z)==1.0)
            {
              FIELD_VALUE(scale_flag) = 3;
              FIELD_BB (scale_flag);
            }
          else if (FIELD_VALUE(scale.x)==FIELD_VALUE(scale.y) && FIELD_VALUE(scale.x)==FIELD_VALUE(scale.z))
            {
              FIELD_VALUE(scale_flag) = 2;
              FIELD_BB (scale_flag);
              FIELD_RD(scale.x);
            }
          else if (FIELD_VALUE(scale.x)==1.0)
            {
              FIELD_VALUE(scale_flag) = 1;
              FIELD_BB (scale_flag);
              FIELD_RD(scale.x);
              FIELD_DD(scale.y, 1.0);
              FIELD_DD(scale.z, 1.0);
            }
          else
            {
              FIELD_VALUE(scale_flag) = 0;
              FIELD_BB(scale_flag);
              FIELD_RD(scale.x);
              FIELD_DD(scale.y, FIELD_VALUE(scale.x));
              FIELD_DD(scale.z, FIELD_VALUE(scale.x));
            }
        }
    }

  FIELD_BD(rotation_ang);
  FIELD_3DPOINT(extrusion);
  FIELD_B(has_attribs);

  SINCE(R_2004)
    {
      if (FIELD_VALUE(has_attribs))
        {
          FIELD_BL(owned_obj_count);
        }
    }

  COMMON_ENTITY_HANDLE_DATA;

  FIELD_HANDLE(block_header, 5);

  //There is a typo in the spec. it says "R13-R200:".
  //I guess it means "R13-R2000:" (just like in MINSERT)
  VERSIONS(R_13,R_2000)
    {
      if (FIELD_VALUE(has_attribs))
        {
          FIELD_HANDLE(first_attrib, 4);
          FIELD_HANDLE(last_attrib, 4);
        }
    }

  //Spec typo? Spec says "2004:" but I think it should be "2004+:"
  // just like field owned_obj_count (AND just like in MINSERT)
  SINCE(R_2004)
    {
      if (FIELD_VALUE(has_attribs))
        {
          HANDLE_VECTOR(attrib_handles, owned_obj_count, 4);
        }
    }

  if (FIELD_VALUE(has_attribs))
    {
      FIELD_HANDLE(seqend, 3);
    }

DWG_ENTITY_END

/*(8)*/
DWG_ENTITY(MINSERT);

  FIELD_3BD(ins_pt);

  VERSIONS(R_13,R_14)
    {
      FIELD_3BD(scale);
    }

  SINCE(R_2000)
    {
      DECODER
        {
          FIELD_BB(scale_flag);
          if (FIELD_VALUE(scale_flag) == 3)
            {
              FIELD_VALUE(scale.x) = 1.0;
              FIELD_VALUE(scale.y) = 1.0;
              FIELD_VALUE(scale.z) = 1.0;
            }
          else if (FIELD_VALUE(scale_flag) == 1)
            {
              FIELD_VALUE(scale.x) = 1.0;
              FIELD_DD(scale.y, 1.0);
              FIELD_DD(scale.z, 1.0);
            }
          else if (FIELD_VALUE(scale_flag) == 2)
            {
              FIELD_RD(scale.x);
              FIELD_VALUE(scale.y) = FIELD_VALUE(scale.x);
              FIELD_VALUE(scale.z) = FIELD_VALUE(scale.x);
            }
          else
            {
              assert(FIELD_VALUE(scale_flag) == 0);
              FIELD_RD(scale.x);
              FIELD_DD(scale.y, FIELD_VALUE(scale.x));
              FIELD_DD(scale.z, FIELD_VALUE(scale.x));
            }
        }

      ENCODER
        {
          if (FIELD_VALUE(scale.x) == 1.0 &&
              FIELD_VALUE(scale.y) == 1.0 &&
              FIELD_VALUE(scale.z) == 1.0)
            {
              FIELD_VALUE(scale_flag) = 3;
              FIELD_BB(scale_flag);
            }
          else if (FIELD_VALUE(scale.x) == 1.0)
             {
              FIELD_VALUE(scale_flag) = 1;
              FIELD_BB(scale_flag);
              FIELD_DD(scale.y, 1.0);
              FIELD_DD(scale.z, 1.0);
             }
          else if (FIELD_VALUE(scale.x) == FIELD_VALUE(scale.y) &&
                   FIELD_VALUE(scale.x) == FIELD_VALUE(scale.z))
            {
              FIELD_VALUE(scale_flag) = 2;
              FIELD_BB(scale_flag);
              FIELD_RD(scale.x);
            }
          else
            {
              FIELD_VALUE(scale_flag) = 0;
              FIELD_BB(scale_flag);
              FIELD_RD(scale.x);
              FIELD_DD(scale.y, FIELD_VALUE(scale.x));
              FIELD_DD(scale.z, FIELD_VALUE(scale.x));
            }
        }
    }

  FIELD_BD(rotation_ang);
  FIELD_3BD(extrusion);
  FIELD_B(has_attribs);

  SINCE(R_2004)
    {
      if (FIELD_VALUE(has_attribs))
        {
          FIELD_BL (owned_obj_count);
        }
    }

  FIELD_BS(numcols);
  FIELD_BS(numrows);
  FIELD_BD(col_spacing);
  FIELD_BD(row_spacing);

  COMMON_ENTITY_HANDLE_DATA;

  FIELD_HANDLE(block_header, 5);

  VERSIONS(R_13,R_2000)
    if (FIELD_VALUE(has_attribs))
      {
        FIELD_HANDLE(first_attrib, 4);
        FIELD_HANDLE(last_attrib, 4);
      }

  SINCE(R_2004)
    {
      HANDLE_VECTOR(attrib_handles, owned_obj_count, 4);
    }

  if (FIELD_VALUE(has_attribs))
    {
      FIELD_HANDLE(seqend, 3);
    }

DWG_ENTITY_END

//(9) Unknown

/*(10)*/
DWG_ENTITY(VERTEX_2D);

  FIELD_RC(flags);
  FIELD_3BD(point);

/* Decoder and Encoder routines could be the same but then we wouldn't compress
data when saving. So we explicitely implemented the encoder routine with the
compression technique described in the spec. --Juca */

  DECODER
    {
      FIELD_BD(start_width);

      if (FIELD_VALUE(start_width) < 0)
        {
          FIELD_VALUE(start_width) = -FIELD_VALUE(start_width);
          FIELD_VALUE(end_width) = FIELD_VALUE(start_width);
        }
      else
        {
          FIELD_BD(end_width);
        }
    }

  ENCODER
    {
      if (FIELD_VALUE(start_width) && FIELD_VALUE(start_width)==FIELD_VALUE(end_width))
        {
          //TODO: This is ugly! We should have a better way of doing such things
          FIELD_VALUE(start_width) = -FIELD_VALUE(start_width);
          FIELD_BD(start_width);
          FIELD_VALUE(start_width) = -FIELD_VALUE(start_width);
        }
      else
        {
          FIELD_BD(start_width);
          FIELD_BD(end_width);
        }
    }

  SINCE(R_2010)
    {
      FIELD_BL(id);
    }
  FIELD_BD(bulge);
  FIELD_BD(tangent_dir);

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/*(11)*/
DWG_ENTITY(VERTEX_3D);

  FIELD_RC(flags);
  FIELD_3BD(point);

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/*(12)*/
DWG_ENTITY(VERTEX_MESH);

  FIELD_RC (flags);
  FIELD_3BD(point);

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/*(13)*/
DWG_ENTITY(VERTEX_PFACE);

  FIELD_RC(flags);
  FIELD_3BD(point);

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/*(14)*/
DWG_ENTITY(VERTEX_PFACE_FACE);

  FIELD_BS(vertind[0]);
  FIELD_BS(vertind[1]);
  FIELD_BS(vertind[2]);
  FIELD_BS(vertind[3]);

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/*(15)*/
DWG_ENTITY(POLYLINE_2D);

  FIELD_BS(flags);
  FIELD_BS(curve_type);
  FIELD_BD(start_width);
  FIELD_BD(end_width);
  FIELD_BT(thickness);
  FIELD_BD(elevation);
  FIELD_BE(extrusion);

  SINCE(R_2004)
    {
      FIELD_BL(owned_obj_count);
    }

  COMMON_ENTITY_HANDLE_DATA;

  VERSIONS(R_13,R_2000)
    {
      FIELD_HANDLE(first_vertex, 4);
      FIELD_HANDLE(last_vertex, 4);
    }

  SINCE(R_2004)
    {
      HANDLE_VECTOR(vertex, owned_obj_count, 4);
    }

  FIELD_HANDLE(seqend, 3);

DWG_ENTITY_END

/*(16)*/
DWG_ENTITY(POLYLINE_3D);

  FIELD_RC (flags_1);
  FIELD_RC (flags_2);

  SINCE(R_2004)
    {
      FIELD_BL (owned_obj_count);
    }

  COMMON_ENTITY_HANDLE_DATA;

  VERSIONS(R_13,R_2000)
    {
      FIELD_HANDLE(first_vertex, 4);
      FIELD_HANDLE(last_vertex, 4);
    }

  SINCE(R_2004)
    {
      HANDLE_VECTOR(vertex, owned_obj_count, 4);  //code 3
    }

  FIELD_HANDLE(seqend, 3);

DWG_ENTITY_END

/*(17)*/
DWG_ENTITY(ARC);

  FIELD_3BD(center);
  FIELD_BD(radius);
  FIELD_BT(thickness);
  FIELD_BE(extrusion);
  FIELD_BD(start_angle);
  FIELD_BD(end_angle);

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/*(18)*/
DWG_ENTITY(CIRCLE);

  FIELD_3BD(center);
  FIELD_BD(radius);
  FIELD_BT(thickness);
  FIELD_BE(extrusion);

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/*(19)*/
DWG_ENTITY(LINE);

  VERSIONS(R_13,R_14)
    {
      FIELD_3BD(start);
      FIELD_3BD(end);
    }

  SINCE(R_2000)
    {
      DECODER
        {
          FIELD(Zs_are_zero, B);
          FIELD_RD(start.x);
          FIELD_DD(end.x, FIELD_VALUE(start.x));
          FIELD_RD(start.y);
          FIELD_DD(end.y, FIELD_VALUE(start.y));

          if (FIELD_VALUE(Zs_are_zero))
            {
              FIELD_VALUE(start.z) = 0.0;
              FIELD_VALUE(end.z) = 0.0;
            }
          else
            {
              FIELD_RD(start.z);
              FIELD_DD(end.z, FIELD_VALUE(start.z));
            }
        }

      ENCODER
        {
          FIELD_VALUE(Zs_are_zero) = (FIELD_VALUE(start.z) == 0.0 && FIELD_VALUE(end.z) == 0.0);
          FIELD_B(Zs_are_zero);
          FIELD_RD(start.x);
          FIELD_DD(end.x, FIELD_VALUE(start.x));
          FIELD_RD(start.y);
          FIELD_DD(end.y, FIELD_VALUE(start.y));
          if (!FIELD_VALUE(Zs_are_zero))
            {
              FIELD_RD(start.z);
              FIELD_DD(end.z, FIELD_VALUE(start.z));
            }
        }
    }

  FIELD_BT(thickness);
  FIELD_BE(extrusion);

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/**
 * Macro for common DIMENSION declaration
 */
/*FIELD_BD(elevation.ecs_11); \ */
#define DIMENSION_COMMON_DECODE \
    SINCE(R_2010) \
      { \
        FIELD_RC (version); \
      } \
    FIELD_3BD(extrusion); \
    FIELD_2RD(text_midpt); \
    FIELD_BD(elevation.ecs_12); \
    FIELD_RC(flags_1); \
    FIELD_TV(user_text); \
    FIELD_BD(text_rot); \
    FIELD_BD(horiz_dir); \
    FIELD_3BD(ins_scale); \
    FIELD_BD(ins_rotation); \
    SINCE(R_2000) \
      { \
        FIELD_BS(attachment_point); \
        FIELD_BS(lspace_style); \
        FIELD_BD(lspace_factor); \
        FIELD_BD(act_measurement); \
      } \
    SINCE(R_2007) \
      { \
        FIELD_B(unknown); \
        FIELD_B(flip_arrow1); \
        FIELD_B(flip_arrow2); \
      }


/*(20)*/
DWG_ENTITY(DIMENSION_ORDINATE);

  DIMENSION_COMMON_DECODE;
  FIELD_2RD(_12_pt);
  FIELD_3BD(_10_pt);
  FIELD_3BD(_13_pt);
  FIELD_3BD(_14_pt);
  FIELD_RC(flags_2);

  COMMON_ENTITY_HANDLE_DATA;
  UNTIL(R_2007)
    {
      FIELD_HANDLE(dimstyle, 5);
      FIELD_HANDLE(block, 5);
    }

DWG_ENTITY_END

/*(21)*/
DWG_ENTITY(DIMENSION_LINEAR);

  DIMENSION_COMMON_DECODE;
  FIELD_2RD(_12_pt);
  FIELD_3BD(_13_pt);
  FIELD_3BD(_14_pt);
  FIELD_3BD(_10_pt);
  FIELD_BD(ext_line_rot);
  FIELD_BD(dim_rot);

  COMMON_ENTITY_HANDLE_DATA;
  UNTIL(R_2007)
    {
      FIELD_HANDLE(dimstyle, 5);
      FIELD_HANDLE(block, 5);
    }

DWG_ENTITY_END

/*(22)*/
DWG_ENTITY(DIMENSION_ALIGNED);

  DIMENSION_COMMON_DECODE;
  FIELD_2RD(_12_pt);
  FIELD_3BD(_13_pt);
  FIELD_3BD(_14_pt);
  FIELD_3BD(_10_pt);
  FIELD_BD(ext_line_rot);

  COMMON_ENTITY_HANDLE_DATA;
  UNTIL(R_2007)
    {
      FIELD_HANDLE(dimstyle, 5);
      FIELD_HANDLE(block, 5);
    }

DWG_ENTITY_END

/*(23)*/
DWG_ENTITY(DIMENSION_ANG3PT);

  DIMENSION_COMMON_DECODE;
  FIELD_2RD(_12_pt);
  FIELD_3BD(_10_pt);
  FIELD_3BD(_13_pt);
  FIELD_3BD(_14_pt);
  FIELD_3BD(_15_pt);

  COMMON_ENTITY_HANDLE_DATA;
  UNTIL(R_2007)
    {
      FIELD_HANDLE(dimstyle, 5);
      FIELD_HANDLE(block, 5);
    }

DWG_ENTITY_END

/*(24)*/
DWG_ENTITY(DIMENSION_ANG2LN);

  DIMENSION_COMMON_DECODE;
  FIELD_2RD(_12_pt);
  FIELD_2RD(_16_pt);
  FIELD_3BD(_13_pt);
  FIELD_3BD(_14_pt);
  FIELD_3BD(_15_pt);
  FIELD_3BD(_10_pt);

  COMMON_ENTITY_HANDLE_DATA;
  UNTIL(R_2007)
    {
      FIELD_HANDLE(dimstyle, 5);
      FIELD_HANDLE(block, 5);
    }

DWG_ENTITY_END

/*(25)*/
DWG_ENTITY(DIMENSION_RADIUS);

  DIMENSION_COMMON_DECODE;
  FIELD_2RD(_12_pt);
  FIELD_3BD(_10_pt);
  FIELD_3BD(_15_pt);
  FIELD_BD(leader_len);

  COMMON_ENTITY_HANDLE_DATA;
  UNTIL(R_2007)
    {
      FIELD_HANDLE(dimstyle, 5);
      FIELD_HANDLE(block, 5);
    }

DWG_ENTITY_END

/*(26)*/
DWG_ENTITY(DIMENSION_DIAMETER);

  DIMENSION_COMMON_DECODE;
  FIELD_2RD(_12_pt);
  FIELD_3BD(_15_pt);
  FIELD_3BD(_10_pt);
  FIELD_BD(leader_len);

  COMMON_ENTITY_HANDLE_DATA;
  UNTIL(R_2007)
    {
      FIELD_HANDLE(dimstyle, 5);
      FIELD_HANDLE(block, 5);
    }

DWG_ENTITY_END

/*(27)*/
DWG_ENTITY(POINT);

  FIELD_BD(x);
  FIELD_BD(y);
  FIELD_BD(z);
  FIELD_BT(thickness);
  FIELD_BE(extrusion);
  FIELD_BD(x_ang);

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/*(28)*/
DWG_ENTITY(_3DFACE);

  VERSIONS(R_13,R_14)
    {
      FIELD_3BD(corner1);
      FIELD_3BD(corner2);
      FIELD_3BD(corner3);
      FIELD_3BD(corner4);
      FIELD_BS(invis_flags);
    }

  SINCE(R_2000)
    {
      FIELD_B(has_no_flags);

      DECODER
        {
          FIELD_B(z_is_zero);
          FIELD_RD(corner1.x);
          FIELD_RD(corner1.y);
          if(FIELD_VALUE(z_is_zero))
            {
              FIELD_VALUE(corner1.z) = 0;
            }
          else
            {
              FIELD_RD(corner1.z);
            }
        }

      ENCODER
        {
          FIELD_VALUE(z_is_zero) = (FIELD_VALUE(corner1.z) == 0);
          FIELD_B(z_is_zero);
          FIELD_RD(corner1.x);
          FIELD_RD(corner1.y);
          if(!FIELD_VALUE(z_is_zero))
            {
              FIELD_RD(corner1.z);
            }
        }

      FIELD_DD(corner2.x, FIELD_VALUE(corner1.x));
      FIELD_DD(corner2.y, FIELD_VALUE(corner1.y));
      FIELD_DD(corner2.z, FIELD_VALUE(corner1.z));
      FIELD_DD(corner3.x, FIELD_VALUE(corner2.x));
      FIELD_DD(corner3.y, FIELD_VALUE(corner2.y));
      FIELD_DD(corner3.z, FIELD_VALUE(corner2.z));
      FIELD_DD(corner4.x, FIELD_VALUE(corner3.x));
      FIELD_DD(corner4.y, FIELD_VALUE(corner3.y));
      FIELD_DD(corner4.z, FIELD_VALUE(corner3.z));
    }

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/*(29)*/
DWG_ENTITY(POLYLINE_PFACE);

  FIELD_BS(numverts);
  FIELD_BS(numfaces);

  SINCE(R_2004)
    {
      FIELD_BL(owned_obj_count);
    }

  COMMON_ENTITY_HANDLE_DATA;

  VERSIONS(R_13, R_2000)
    {
      FIELD_HANDLE(first_vertex, 4);
      FIELD_HANDLE(last_vertex, 4);
    }
  SINCE(R_2004)
    {
      HANDLE_VECTOR(vertex, owned_obj_count, 4);
    }
  FIELD_HANDLE(seqend, 3);

DWG_ENTITY_END

/*(30)*/
DWG_ENTITY(POLYLINE_MESH);

  FIELD_BS(flags);
  FIELD_BS(curve_type);
  FIELD_BS(m_vert_count);
  FIELD_BS(n_vert_count);
  FIELD_BS(m_density);
  FIELD_BS(n_density);

  SINCE(R_2004)
    {
      FIELD_BL(owned_obj_count);
    }

  COMMON_ENTITY_HANDLE_DATA;

  VERSIONS(R_13, R_2000)
    {
      FIELD_HANDLE(first_vertex, 4);
      FIELD_HANDLE(last_vertex, 4);
    }
  SINCE(R_2004)
    {
      HANDLE_VECTOR(vertex, owned_obj_count, 4);
    }
  FIELD_HANDLE(seqend, 3);

DWG_ENTITY_END

/*(31)*/
DWG_ENTITY(SOLID);

  FIELD_BT(thickness);
  FIELD_BD(elevation);
  FIELD_2RD(corner1);
  FIELD_2RD(corner2);
  FIELD_2RD(corner3);
  FIELD_2RD(corner4);
  FIELD_BE(extrusion);

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/*(32)*/
DWG_ENTITY(TRACE);

  FIELD_BT(thickness);
  FIELD_BD(elevation);
  FIELD_2RD(corner1);
  FIELD_2RD(corner2);
  FIELD_2RD(corner3);
  FIELD_2RD(corner4);
  FIELD_BE(extrusion);

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/*(33)*/
DWG_ENTITY(SHAPE);

  FIELD_3BD(ins_pt);
  FIELD_BD(scale);
  FIELD_BD(rotation);
  FIELD_BD(width_factor);
  FIELD_BD(oblique);
  FIELD_BD(thickness);
  FIELD_BS(shape_no);
  FIELD_3BD(extrusion);

  COMMON_ENTITY_HANDLE_DATA;
  FIELD_HANDLE(shapefile, 5);

DWG_ENTITY_END

/*(34)*/
DWG_ENTITY(VIEWPORT);

  FIELD_3BD(center);
  FIELD_BD(width);
  FIELD_BD(height);

  SINCE(R_2000)
    {
      FIELD_3BD(view_target);
      FIELD_3BD(view_direction);
      FIELD_BD(view_twist_angle);
      FIELD_BD(view_height);
      FIELD_BD(lens_length);
      FIELD_BD(front_clip_z);
      FIELD_BD(back_clip_z);
      FIELD_BD(snap_angle);
      FIELD_2RD(view_center);
      FIELD_2RD(snap_base);
      FIELD_2RD(snap_spacing);
      FIELD_2RD(grid_spacing);
      FIELD_BS(circle_zoom);
    }

  SINCE(R_2007)
    {
      FIELD_BS(grid_major);
    }

  SINCE(R_2000)
    {
      FIELD_BL(frozen_layer_count);
      FIELD_BL(status_flags);
      FIELD_TV(style_sheet);
      FIELD_RC(render_mode);
      FIELD_B(ucs_at_origin);
      FIELD_B(ucs_per_viewport);
      FIELD_3BD(ucs_origin);
      FIELD_3BD(ucs_x_axis);
      FIELD_3BD(ucs_y_axis);
      FIELD_BD(ucs_elevation);
      FIELD_BS(ucs_ortho_view_type);
    }

  SINCE(R_2004)
    {
      FIELD_BS(shadeplot_mode);
    }

  SINCE(R_2007)
    {
      FIELD_B(use_def_lights);
      FIELD_RC(def_lighting_type);
      FIELD_BD(brightness);
      FIELD_BD(contrast);
      FIELD_CMC(ambient_light_color);
    }

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/*(35)*/
DWG_ENTITY(ELLIPSE);

  FIELD_3BD(center);
  FIELD_3BD(sm_axis);
  FIELD_3BD(extrusion);
  FIELD_BD(axis_ratio);
  FIELD_BD(start_angle);
  FIELD_BD(end_angle);

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/*(36)*/
DWG_ENTITY(SPLINE);

  FIELD_BL(scenario);
  UNTIL(R_2013) {
    if (FIELD_VALUE(scenario) != 1 && FIELD_VALUE(scenario) != 2)
      {
        fprintf(stderr, "Error: unknown scenario %d", FIELD_VALUE(scenario));
      }
  }
  SINCE(R_2013) {
    FIELD_BL(splineflags1);
    FIELD_BL(knotparam);
  }

  FIELD_BL(degree);

  if (FIELD_VALUE(scenario) & 2)
    {
      FIELD_BD(fit_tol);
      FIELD_3BD(beg_tan_vec);
      FIELD_3BD(end_tan_vec);
      FIELD_BL(num_fit_pts);
    }
  if (FIELD_VALUE(scenario) & 1)
    {
      FIELD_B(rational);
      FIELD_B(closed_b);
      FIELD_B(periodic);
      FIELD_BD(knot_tol);
      FIELD_BD(ctrl_tol);
      FIELD_BL(num_knots);
      FIELD_BL(num_ctrl_pts);
      FIELD_B(weighted);
    }

  if (FIELD_VALUE(scenario) & 1) {
    REPEAT(num_knots, knots, BITCODE_BD)
      {
        FIELD_BD(knots[rcount]);
      }
    REPEAT(num_ctrl_pts, ctrl_pts, Dwg_Entity_SPLINE_control_point)
      {
        FIELD_3BD(ctrl_pts[rcount]);
        if (!FIELD_VALUE(weighted))
          {
            FIELD_VALUE(ctrl_pts[rcount].w) = 0; // skipped when encoding
          }
        else
          {
            FIELD_BD(ctrl_pts[rcount].w);
          }
      }
  }
  if (FIELD_VALUE(scenario) & 2) {
    REPEAT(num_fit_pts, fit_pts, Dwg_Entity_SPLINE_point)
      {
        FIELD_3BD(fit_pts[rcount]);
      }
  }

  COMMON_ENTITY_HANDLE_DATA;
DWG_ENTITY_END

//TODO: 37, 38 and 39 are ACIS entities

#define PARSE_WIRE_STRUCT(name)                       \
  FIELD_RC(name.type);                                \
  FIELD_BL(name.selection_marker);                    \
  FIELD_BS(name.color);                               \
  FIELD_BL(name.acis_index);                          \
  FIELD_BL(name.num_points);                          \
  FIELD_3DPOINT_VECTOR(name.points, name.num_points); \
  FIELD_B(name.transform_present);                    \
  if (FIELD_VALUE(name.transform_present))            \
    {                                                 \
      FIELD_3BD(name.axis_x);                         \
      FIELD_3BD(name.axis_y);                         \
      FIELD_3BD(name.axis_z);                         \
      FIELD_3BD(name.translation);                    \
      FIELD_BD(name.scale);                           \
      FIELD_B(name.has_rotation);                     \
      FIELD_B(name.has_reflection);                   \
      FIELD_B(name.has_shear);                        \
    }

#ifdef IS_DECODER

#define DECODE_3DSOLID decode_3dsolid(dat, obj, _obj);
void decode_3dsolid(Bit_Chain* dat, Dwg_Object* obj, Dwg_Entity_3DSOLID* _obj);

void decode_3dsolid(Bit_Chain* dat, Dwg_Object* obj, Dwg_Entity_3DSOLID* _obj)
{
  Dwg_Data* dwg = obj->parent;
  int vcount, rcount, rcount2;
  int i=0;
  unsigned long j;
  int index;
  int total_size = 0;
  int num_blocks = 0;

  FIELD_B(acis_empty);
  if (!FIELD_VALUE(acis_empty))
    {
      FIELD_B (unknown);
      FIELD_BS (version);
      if (FIELD_VALUE(version) == 1)
        {
          do
            {
              FIELD_VALUE(sat_data) = (BITCODE_RC**)
                realloc(FIELD_VALUE(sat_data), (i+1) * sizeof(BITCODE_RC*));
              FIELD_VALUE(block_size) = (BITCODE_BL*)
                realloc(FIELD_VALUE(block_size), (i+1) * sizeof(BITCODE_BL));
              FIELD_BL (block_size[i]);
              FIELD_VECTOR (sat_data[i], RC, block_size[i]);
              total_size += FIELD_VALUE(block_size[i]);
            } while(FIELD_VALUE(block_size[i++]));

          // de-obfuscate SAT data
          FIELD_VALUE(raw_sat_data) = (unsigned char*)
            malloc (total_size * sizeof(unsigned char*));
          num_blocks = i-1;
          FIELD_VALUE(num_blocks) = num_blocks;
          index = 0;
          for (i=0; i<num_blocks; i++)
            {
              for (j=0; j<FIELD_VALUE(block_size[i]); j++)
                {
                  if (FIELD_VALUE(sat_data[i][j] <= 32))
                    {
                      FIELD_VALUE(raw_sat_data)[index++] = FIELD_VALUE(sat_data[i][j]);
                    }
                  else
                    {
                      FIELD_VALUE(raw_sat_data)[index++] = 159 - FIELD_VALUE(sat_data[i][j]);
                    }
                }
            }
          LOG_TRACE("Raw SAT data:\n%s\n", FIELD_VALUE(raw_sat_data));
        }
      else //if (FIELD_VALUE(version)==2)
        {
          //TODO
          LOG_ERROR("TODO: Implement parsing of SAT file (version 2) in entities 37,38 and 39.\n");
        }

      FIELD_B (wireframe_data_present);
      if (FIELD_VALUE(wireframe_data_present))
        {
          FIELD_B (point_present);
          if (FIELD_VALUE(point_present))
            {
              FIELD_3BD (point);
            }
          else
            {
              FIELD_VALUE(point.x) = 0;
              FIELD_VALUE(point.y) = 0;
              FIELD_VALUE(point.z) = 0;
            }
          FIELD_BL (num_isolines);
          FIELD_B (isoline_present);
          if (FIELD_VALUE(isoline_present))
            {
              FIELD_BL (num_wires);
              REPEAT(num_wires, wires, Dwg_Entity_3DSOLID_wire)
                {
                  PARSE_WIRE_STRUCT(wires[rcount])
                }
              FIELD_BL(num_silhouettes);
              REPEAT(num_silhouettes, silhouettes, Dwg_Entity_3DSOLID_silhouette)
                {
                  FIELD_BL(silhouettes[rcount].vp_id);
                  FIELD_3BD(silhouettes[rcount].vp_target);
                  FIELD_3BD(silhouettes[rcount].vp_dir_from_target);
                  FIELD_3BD(silhouettes[rcount].vp_up_dir);
                  FIELD_B(silhouettes[rcount].vp_perspective);
                  FIELD_BL(silhouettes[rcount].num_wires);
                  REPEAT2(silhouettes[rcount].num_wires, silhouettes[rcount].wires, Dwg_Entity_3DSOLID_wire)
                    {
                      PARSE_WIRE_STRUCT(silhouettes[rcount].wires[rcount2])
                    }
                }
            }
        }

      FIELD_B(ACIS_empty_bit);
      if (!FIELD_VALUE(ACIS_empty_bit))
        {
          LOG_ERROR("TODO: Implement parsing of ACIS data in the end of 3dsolid object parsing (ACIS_empty_bit==0).\n");
        }

      SINCE(R_2007)
        {
          FIELD_BL(unknown_2007);
        }

      COMMON_ENTITY_HANDLE_DATA;

      SINCE(R_2007)
        {
          FIELD_HANDLE(history_id, ANYCODE);
        }
    }
}
#else
#define DECODE_3DSOLID {}
#define FREE_3DSOLID {}
#endif //#if IS_DECODER

#ifdef IS_ENCODER

#define ENCODE_3DSOLID encode_3dsolid(dat, obj, _obj);
void encode_3dsolid(Bit_Chain* dat, Dwg_Object* obj, Dwg_Entity_3DSOLID* _obj);

void encode_3dsolid(Bit_Chain* dat, Dwg_Object* obj, Dwg_Entity_3DSOLID* _obj)
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
void free_3dsolid(Bit_Chain* dat, Dwg_Object* obj, Dwg_Entity_3DSOLID* _obj);

void free_3dsolid(Bit_Chain* dat, Dwg_Object* obj, Dwg_Entity_3DSOLID* _obj)
{
  unsigned int i;
  if (FIELD_VALUE(version) == 1)
    {
      for (i=0; i<FIELD_VALUE(num_blocks); i++)
        {
          FIELD_VECTOR (sat_data[i], RC, block_size[i]);
        }
      free(_obj->sat_data);
      free(_obj->block_size);
      free(_obj->raw_sat_data);
    }
}
#undef FREE_3DSOLID
#define FREE_3DSOLID free_3dsolid(dat, obj, _obj)
#endif


/*(37)*/
DWG_ENTITY(REGION);
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
DWG_ENTITY(_3DSOLID);
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
DWG_ENTITY(BODY);
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
DWG_ENTITY(RAY);

  FIELD_3BD(point);
  FIELD_3BD(vector);

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/*(41)*/
DWG_ENTITY(XLINE);

  FIELD_3BD(point);
  FIELD_3BD(vector);

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/*(42)*/
DWG_OBJECT(DICTIONARY);

  FIELD_BL (numitems);

  VERSION(R_14)
    {
      FIELD_RC(hard_owner); //?
    }
  SINCE(R_2000)
    {
      FIELD_BS(cloning); // 281
      FIELD_RC(hard_owner);
    }

  if (FIELD_VALUE(numitems) > 10000)
    {
      fprintf(
          stderr,
          "Strange: dictionary with more than 10 thousand entries! Handle: %lu\n",
          obj->handle.value);
      return;
    }

  FIELD_VECTOR(text, TV, numitems);
  UNTIL(R_2007)
    {
      FIELD_HANDLE(parenthandle, 8); // 8 with 2000, was 4
    }
  REACTORS(4);
  XDICOBJHANDLE(3);
  SINCE(R_2000)
    {
      HANDLE_VECTOR(itemhandles, numitems, 2);
    }

DWG_ENTITY_END

DWG_OBJECT(DICTIONARYWDLFT);

  FIELD_BL (numitems);

  VERSION(R_14)
    {
      FIELD_RL(unknown_r14); // cloning+hard_owner+?
    }
  SINCE(R_2000)
    {
      FIELD_BS(cloning);
      FIELD_RC(hard_owner);
    }

  FIELD_VECTOR(text, TV, numitems);

  UNTIL(R_2007)
    {
      FIELD_HANDLE(parenthandle, 4);
    }
  REACTORS(4);
  XDICOBJHANDLE(3);

  HANDLE_VECTOR(itemhandles, numitems, 2);

  FIELD_HANDLE(defaultid, 5); // DXF:340 - default object id (one of the itemhandles)

DWG_OBJECT_END

/*(43) not seen */
DWG_ENTITY(OLEFRAME);

  FIELD_BS(flags);

  SINCE(R_2000)
    {
      FIELD_BS(mode);
    }

  FIELD_BL(data_length);
  FIELD_VECTOR(data, RC, data_length);

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/*(44)*/
DWG_ENTITY(MTEXT);

  //spec-typo ? Spec says BD but we think it might be 3BD:
  FIELD_3BD(insertion_pt);
  FIELD_3BD(extrusion);
  FIELD_3BD(x_axis_dir);

  FIELD_BD(rect_width);
  SINCE(R_2007)
    {
      FIELD_BD(rect_height);
    }

  FIELD_BD(text_height);
  FIELD_BS(attachment);
  FIELD_BS(drawing_dir);
  FIELD_BD(extends_ht); //not documented
  FIELD_BD(extends_wid);
  FIELD_TV(text);

  SINCE(R_2000)
    {
      FIELD_BS(linespace_style);
      FIELD_BD(linespace_factor);
      FIELD(unknown_bit, B);
    }

  SINCE(R_2004)
    {
      FIELD_BL (bg_flag);
    }

  COMMON_ENTITY_HANDLE_DATA;

  UNTIL(R_2007)
    {
      FIELD_HANDLE(style, 5);
    }

DWG_ENTITY_END

/*(45)*/
DWG_ENTITY(LEADER);

  FIELD(unknown_bit_1, B);
  FIELD_BS(annot_type);
  FIELD_BS(path_type);
  FIELD_BL (numpts);
  FIELD_3DPOINT_VECTOR(points, numpts);
  FIELD_3DPOINT(origin);
  FIELD_3DPOINT(extrusion);
  FIELD_3DPOINT(x_direction);
  FIELD_3DPOINT(offset_to_block_ins_pt);

  SINCE(R_14)
    {
      FIELD_3DPOINT(endptproj);
    }

  VERSIONS(R_13,R_14)
    {
      FIELD_BD(dimgap);
    }

  FIELD_BD(box_height);
  FIELD_BD(box_width);
  FIELD_B(hooklineonxdir);
  FIELD_B(arrowhead_on);

  VERSIONS(R_13,R_14)
    {
      FIELD_BS(arrowhead_type);
      FIELD_BD(dimasz);
      FIELD_B(unknown_bit_2);
      FIELD_B(unknown_bit_3);
      FIELD_BS(unknown_short_1);
      FIELD_BS(byblock_color);
      FIELD_B(unknown_bit_4);
      FIELD_B(unknown_bit_5);
    }

  SINCE(R_2000)
    {
      FIELD_BS(unknown_short_1);
      FIELD_B(unknown_bit_4);
      FIELD_B(unknown_bit_5);
    }

  COMMON_ENTITY_HANDLE_DATA;

  //FIXME reading these handles lead to a segfault
  SINCE(R_13) // TODO until 2007?
  {
    //TODO check if field is present in R_13.
    //Juca thinks it is present but inactive/not used.
    FIELD_HANDLE(associated_annotation, 2);
  }
  //UNTIL(R_2007) // TODO until 2007?
  {
    FIELD_HANDLE(dimstyle, 5);
  }

DWG_ENTITY_END

/*(46)*/
DWG_ENTITY(TOLERANCE);

  VERSIONS(R_13, R_14)
    {
      FIELD_BS(unknown_short); //spec-typo? Spec says S instead of BS.
      FIELD_BD(height);
      FIELD_BD(dimgap);
    }

  FIELD_3DPOINT(ins_pt);
  FIELD_3DPOINT(x_direction);
  FIELD_3DPOINT(extrusion);
  FIELD_TV(text_string);

  COMMON_ENTITY_HANDLE_DATA;
  FIELD_HANDLE(dimstyle, 5);

DWG_ENTITY_END

/*(47)*/
DWG_ENTITY(MLINE);

  FIELD_BD(scale);
  FIELD_RC(just); /*spec-typo? Spec says EC instead of RC...*/
  FIELD_3DPOINT(base_point);
  FIELD_3DPOINT(extrusion);
  FIELD_BS(open_closed);
  FIELD_RC(num_lines);
  FIELD_BS(num_verts);

  REPEAT(num_verts, verts, Dwg_Entity_MLINE_vert)
    {
      FIELD_3DPOINT(verts[rcount].vertex);
      FIELD_3DPOINT(verts[rcount].vertex_direction);
      FIELD_3DPOINT(verts[rcount].miter_direction);

      REPEAT2(num_lines, verts[rcount].lines, Dwg_Entity_MLINE_line)
        {
          FIELD_BS(verts[rcount].lines[rcount2].num_segparms);
          REPEAT3(verts[rcount].lines[rcount2].num_segparms, verts[rcount].lines[rcount2].segparms, BITCODE_BD)
            {
              FIELD_BD(verts[rcount].lines[rcount2].segparms[rcount3]);
            }

          FIELD_BS(verts[rcount].lines[rcount2].num_areafillparms);
          REPEAT3(verts[rcount].lines[rcount2].num_areafillparms, verts[rcount].lines[rcount2].areafillparms, BITCODE_BD)
            {
              FIELD_BD(verts[rcount].lines[rcount2].areafillparms[rcount3]);
            }
        }
    }

  COMMON_ENTITY_HANDLE_DATA;
  FIELD_HANDLE(mline_style, 5);

DWG_ENTITY_END


/*(49)*/
DWG_OBJECT(BLOCK_HEADER);

  PRE(R_13)
  {
    FIELD_RC (flag);
    FIELD_TF (entry_name, 32);
    FIELD_RS (used);
    FIELD_RC (block_scaling);
    FIELD_RS (owned_object_count);
    FIELD_RC (flag2);
    FIELD_RS (insert_count);
    FIELD_RS (flag3);
  }
  SINCE(R_13) {

  FIELD_3DPOINT(base_pt);
  FIELD_TV(xref_pname);

  SINCE(R_2000)
    {
      FIELD_INSERT_COUNT(insert_count, RL);
      FIELD_TV(block_description);

      FIELD_BL(preview_data_size);
      FIELD_VECTOR(preview_data, RC, preview_data_size);
    }

  SINCE(R_2007)
    {
      FIELD_BS(insert_units);
      FIELD_B(explodable);
      FIELD_RC(block_scaling);
    }

  FIELD_HANDLE(block_control_handle, 4);
  REACTORS(4);
  XDICOBJHANDLE(3);
  FIELD_HANDLE(null_handle, 5);
  FIELD_HANDLE(block_entity, 3);

  VERSIONS(R_13,R_2000)
    {
      if (!FIELD_VALUE(blkisxref) && !FIELD_VALUE(xrefoverlaid))
        {
          FIELD_HANDLE(first_entity, 4);
          FIELD_HANDLE(last_entity, 4);
        }
    }

  SINCE(R_2004)
    {
      HANDLE_VECTOR(entities, owned_object_count, 4);
    }

  FIELD_HANDLE(endblk_entity, 3);

  SINCE(R_2000)
    {
      if (FIELD_VALUE(insert_count))
        HANDLE_VECTOR(insert_handles, insert_count, 7)
      FIELD_HANDLE(layout_handle, 5);
    }
  }

DWG_OBJECT_END

/*(50)*/
DWG_OBJECT(LAYER_CONTROL);

  FIELD_BL(num_entries);
  UNTIL(R_2007)
    {
      FIELD_HANDLE(null_handle, 4);
    }
  XDICOBJHANDLE(3);
  HANDLE_VECTOR(layers, num_entries, 2);

DWG_OBJECT_END

/*(51)*/
DWG_OBJECT(LAYER);

  PRE(R_13)
  {
    FIELD_RC (flag);
    FIELD_TF (entry_name, 32);
    FIELD_RS (used);
    FIELD_RS (color_rs);   // color number, if negative layer is off
    FIELD_RS (linetype_rs);
  }
  SINCE(R_13) {
    IF_ENCODE_FROM_PRE_R13 {
      FIELD_VALUE(_64_flag) = FIELD_VALUE(flag) & 64;
      FIELD_VALUE(frozen) = FIELD_VALUE(flag) & 1;
      FIELD_VALUE(on)     = FIELD_VALUE(color_rs) < 0 ? 0 : 1;
      FIELD_VALUE(frozen_in_new) = FIELD_VALUE(flag) & 2;
      FIELD_VALUE(locked) = FIELD_VALUE(flag) & 4;
    }
    FIELD_TV(entry_name);
    FIELD(_64_flag, B);
    SINCE(R_2010)
      {
        FIELD_B(xrefdep);
        if (FIELD_VALUE(xrefdep))
            FIELD_BS(xrefindex_plus1);
      }
    OTHER_VERSIONS
      {
        FIELD_BS(xrefindex_plus1);
        FIELD_B(xrefdep);
      }
    VERSIONS(R_13, R_14)
      {
        FIELD(frozen, B);
        FIELD(on, B);
        FIELD(frozen_in_new, B);
        FIELD(locked, B);
        FIELD_VALUE(flag) =      // dxf flag 70
          FIELD_VALUE(_64_flag) << 8 |
          FIELD_VALUE(locked)   << 3 |
          FIELD_VALUE(frozen_in_new) << 2 |
          FIELD_VALUE(on)       << 1;
          FIELD_VALUE(frozen);
      }
    SINCE(R_2000)
      {
        // adds bit 16: plotting flag from 70
        // and lineweight: 0x03e0 from 70
        IF_ENCODE_FROM_EARLIER {
          FIELD_VALUE(flag_rs) = FIELD_VALUE(flag);
        }
        FIELD_BS(flag_rs);
      }

    FIELD_CMC(color);
    
    UNTIL(R_2007)
      {
        FIELD_HANDLE(layer_control, 4);
        REACTORS(4);
        XDICOBJHANDLE(3);
        FIELD_HANDLE(null_handle, 5);
      }
    
    VERSIONS(R_2000, R_2007)
      {
        FIELD_HANDLE(plotstyle, 5);
      }
    
    SINCE(R_2007)
      {
        FIELD_HANDLE(material, ANYCODE);
      }
    
    UNTIL(R_2007)
      {
        FIELD_HANDLE(linetype, 5);
      }
    //FIELD_HANDLE(null_handle2, 5); // doc error
  }

DWG_OBJECT_END

/*(52)*/
DWG_OBJECT(SHAPEFILE_CONTROL);

  FIELD_BL (num_entries);
  FIELD_HANDLE (null_handle, 4);
  XDICOBJHANDLE(3);
  HANDLE_VECTOR (shapefiles, num_entries, 2);

DWG_OBJECT_END

/*(53)*/
DWG_OBJECT(SHAPEFILE);

  PRE(R_13)
  {
    FIELD_RC (flag);        // 70
    FIELD_TF (entry_name, 32);
    FIELD_RS (used);
    FIELD_RD (fixed_height); // 40
    FIELD_RD (width_factor); // 41
    FIELD_RD (oblique_ang);  // 50
    FIELD_RC (generation);   // 71
    FIELD_RD (last_height);  // 42
    FIELD_TF (font_name, 128); // 3
  }
  SINCE(R_13) {
    IF_ENCODE_FROM_PRE_R13 {
      FIELD_VALUE(_64_flag)   = FIELD_VALUE(flag) & 64;
      FIELD_VALUE(shape_file) = FIELD_VALUE(flag) & 1;
      FIELD_VALUE(vertical)   = FIELD_VALUE(flag) & 4;
    }
    FIELD_TV (entry_name);  // 2
    FIELD_B (_64_flag);     // bit 64 of 70
    PRE(R_2010)
    {
      FIELD_BS (xrefindex_plus1);
      FIELD_B (xrefdep);
    }
    LATER_VERSIONS
    {
      FIELD_B (xrefdep);
      if (FIELD_VALUE(xrefdep)) {
        FIELD_BS (xrefindex_plus1);
      }
    }
    FIELD_B (vertical);      // bit 1 of 70
    FIELD_B (shape_file);    // bit 4 of 70
    FIELD_VALUE(flag) =      // dxf flag 70
      FIELD_VALUE(_64_flag) << 8 |
      FIELD_VALUE(shape_file) << 2 |
      FIELD_VALUE(vertical);
    FIELD_BD (fixed_height); // 40
    FIELD_BD (width_factor); // 41
    FIELD_BD (oblique_ang);  // 50
    FIELD_RC (generation);   // 71
    FIELD_BD (last_height);  // 42
    FIELD_TV (font_name);    // 3
    FIELD_TV (bigfont_name); // 4

    FIELD_HANDLE(shapefile_control, 4);
    REACTORS(4);
    XDICOBJHANDLE(3);
    UNTIL(R_2007) {
        FIELD_HANDLE(null_handle, 5);
    }
  }

DWG_OBJECT_END

//(54): Unknown
//(55): Unknown

/*(56)*/
DWG_OBJECT(LTYPE_CONTROL);

  FIELD_BS (num_entries);
  UNTIL(R_2007)
    {
      FIELD_HANDLE(null_handle, 4);
    }
  XDICOBJHANDLE(3);
  HANDLE_VECTOR(linetypes, num_entries, 2);
  FIELD_HANDLE(bylayer, 3);
  FIELD_HANDLE(byblock, 3);

DWG_OBJECT_END

/*(57)*/
DWG_OBJECT(LTYPE);

  FIELD_TV(entry_name);
  FIELD(_64_flag, B);
  SINCE(R_2010)
    {
      FIELD_B(xrefdep);
      if (FIELD_VALUE(xrefdep))
        {
          FIELD_BS(xrefindex_plus1);
        }
    }
  OTHER_VERSIONS
    {
      FIELD_BS(xrefindex_plus1);
      FIELD_B(xrefdep);
    }
  FIELD_TV(description);
  SINCE(R_2010)
    {
      FIELD_BD(pattern_len);
    }
  OTHER_VERSIONS
    {
      FIELD_BD(pattern_len);
    }
  FIELD_RC (alignment);
  FIELD_RC (num_dashes);
  REPEAT(num_dashes, dash, LTYPE_dash)
    {
      FIELD_BD(dash[rcount].length);
      FIELD_BS(dash[rcount].complex_shapecode);
      FIELD_RD(dash[rcount].x_offset);
      FIELD_RD(dash[rcount].y_offset);
      FIELD_BD(dash[rcount].scale);
      FIELD_BD(dash[rcount].rotation);
      FIELD_BS(dash[rcount].shape_flag);
      if (FIELD_VALUE(dash[rcount].shape_flag) & 0x2)
        FIELD_VALUE(text_area_is_present) = 1;
    }

  UNTIL(R_2004)
    {
      FIELD_VECTOR_N(strings_area, RC, 256);
    }
  LATER_VERSIONS
    {
      if (FIELD_VALUE(text_area_is_present))
        {
          FIELD_VECTOR_N(strings_area, RC, 512);
        }
    }

  FIELD_HANDLE(linetype_control, 4);
  REACTORS(4);
  XDICOBJHANDLE(3);
  FIELD_HANDLE(null_handle, 5);
  HANDLE_VECTOR(shapefiles, num_dashes, 5);

DWG_OBJECT_END

//(58): Unknown
//(59): Unknown

/*(60)*/
DWG_OBJECT(VIEW_CONTROL);

  FIELD_BL(num_entries);
  UNTIL(R_2007)
    {
      FIELD_HANDLE(null_handle, 4);
    }
  XDICOBJHANDLE(3);
  HANDLE_VECTOR(views, num_entries, 2);

DWG_OBJECT_END

/*(61)*/
DWG_OBJECT(VIEW);

  FIELD_TV(entry_name);
  FIELD(_64_flag, B);
  SINCE(R_2010)
    {
      FIELD_B(xrefdep);
      if (FIELD_VALUE(xrefdep))
        {
          FIELD_BS(xrefindex_plus1);
        }
    }
  OTHER_VERSIONS
    {
      FIELD_BS(xrefindex_plus1);
      FIELD_B(xrefdep);
    }
  FIELD_BD(height);
  FIELD_BD(width);
  FIELD_2RD(center);
  FIELD_3BD(target);
  FIELD_3BD(direction);
  FIELD_BD(twist_angle);
  FIELD_BD(lens_length);
  FIELD_BD(front_clip);
  FIELD_BD(back_clip);
  FIELD(view_mode, 4BITS);

  SINCE(R_2000)
    {
      FIELD_RC (render_mode);
    }

  FIELD(pspace_flag, B);

  SINCE(R_2000)
    {
      FIELD(associated_ucs, B);

      if (FIELD_VALUE(associated_ucs) & 1)
        {
          FIELD_3BD(origin);
          FIELD_3BD(x_direction);
          FIELD_3BD(y_direction);
          FIELD_BD(elevation);
          FIELD_BS(orthographic_view_type);
        }
    }

  SINCE(R_2007)
    {
      FIELD(camera_plottable, B);
    }

  FIELD_HANDLE(view_control_handle, 4);
  REACTORS(4);
  XDICOBJHANDLE(3);
  FIELD_HANDLE(null_handle, 5);

  SINCE(R_2000)
    {
      if (FIELD_VALUE(associated_ucs) & 1)
        {
          FIELD_HANDLE(base_ucs_handle, ANYCODE);
          FIELD_HANDLE(named_ucs_handle, ANYCODE);
	}
    }

  SINCE(R_2007)
    {
      FIELD_HANDLE(live_section, ANYCODE);
    }

DWG_OBJECT_END

/*(62)*/
DWG_OBJECT(UCS_CONTROL);

  FIELD_BS (num_entries);
  UNTIL(R_2007)
    {
      FIELD_HANDLE(null_handle, 4);
    }
  XDICOBJHANDLE(3);
  HANDLE_VECTOR(ucs, num_entries, 2);

DWG_OBJECT_END

/*(63)*/
DWG_OBJECT(UCS);

  FIELD_TV(entry_name);
  FIELD(_64_flag, B);
  SINCE(R_2010)
    {
      FIELD_B(xrefdep);
      if (FIELD_VALUE(xrefdep))
        {
          FIELD_BS(xrefindex_plus1);
        }
    }
  OTHER_VERSIONS
    {
      FIELD_BS(xrefindex_plus1);
      FIELD_B(xrefdep);
    }
  FIELD_3BD(origin);
  FIELD_3BD(x_direction);
  FIELD_3BD(y_direction);

  SINCE(R_2000)
    {
      FIELD_BD(elevation);
      FIELD_BS(orthographic_view_type);
      FIELD_BS(orthographic_type);
    }

  FIELD_HANDLE(ucs_control_handle, 4);
  REACTORS(4);
  XDICOBJHANDLE(3);
  FIELD_HANDLE(null_handle, 5);

  SINCE(R_2000)
    {
      FIELD_HANDLE(base_ucs_handle, ANYCODE);
      FIELD_HANDLE(unknown, ANYCODE);
    }
DWG_OBJECT_END

/*(64)*/
DWG_OBJECT(VPORT_CONTROL);

  FIELD_BS (num_entries);
  UNTIL(R_2007)
    {
      FIELD_HANDLE (null_handle, 4);
    }
  XDICOBJHANDLE(3);
  HANDLE_VECTOR(vports, num_entries, 2);

DWG_OBJECT_END

/*(65)*/
DWG_OBJECT(VPORT);

  FIELD_TV (entry_name);
  FIELD_B (_64_flag);
  SINCE(R_2010)
    {
      FIELD_B(xrefdep);
      if (FIELD_VALUE(xrefdep))
        {
          FIELD_BS(xrefindex_plus1);
        }
    }
  OTHER_VERSIONS
    {
      FIELD_BS(xrefindex_plus1);
      FIELD_B(xrefdep);
    }
  FIELD_BD (VIEWSIZE);
  FIELD_BD (aspect_ratio);
  FIELD_2RD (VIEWCTR);
  FIELD_3BD (view_target);
  FIELD_3BD (VIEWDIR);
  FIELD_BD (view_twist);
  FIELD_BD (lens_length);
  FIELD_BD (front_clip);
  FIELD_BD (back_clip);
  FIELD_4BITS (view_mode);

  SINCE(R_2000)
    {
      FIELD_RC (render_mode);
    }

  SINCE(R_2007)
    {
      FIELD_B (use_default_lights);
      FIELD_RC (default_lightining_type);
      FIELD_BD (brightness);
      FIELD_BD (contrast);
      FIELD_CMC (ambient_color);
    }

  FIELD_2RD (lower_left);
  FIELD_2RD (upper_right);
  FIELD_B (UCSFOLLOW);
  FIELD_BS (circle_zoom);
  FIELD_B (FASTZOOM);
  FIELD_B (UCSICON_0);
  FIELD_B (UCSICON_1);
  FIELD_B (GRIDMODE);
  FIELD_2RD (GRIDUNIT);
  FIELD_B (SNAPMODE);
  FIELD_B (SNAPSTYLE);
  FIELD_BS (SNAPISOPAIR);
  FIELD_BD (SNAPANG);
  FIELD_2RD (SNAPBASE);
  FIELD_2RD (SNAPUNIT);

  SINCE(R_2000)
    {
      FIELD_B (unknown);
      FIELD_B (ucs_pre_viewport);
      FIELD_3BD (ucs_origin);
      FIELD_3BD (ucs_x_axis);
      FIELD_3BD (ucs_y_axis);
      FIELD_BD (ucs_elevation);
      FIELD_BS (ucs_orthografic_type);
    }

  SINCE(R_2007)
    {
      FIELD_BS (grid_flags);
      FIELD_BS (grid_major);
    }

  FIELD_HANDLE (vport_control, 4);
  REACTORS(4);
  XDICOBJHANDLE(3);
  FIELD_HANDLE(null_handle, 5);

  SINCE(R_2007)
    {
      FIELD_HANDLE (background_handle, ANYCODE);
      FIELD_HANDLE (visual_style_handle, ANYCODE);
      FIELD_HANDLE (sun_handle, ANYCODE);
    }

  SINCE(R_2000)
    {
      FIELD_HANDLE (named_ucs_handle, ANYCODE);
      FIELD_HANDLE (base_ucs_handle, ANYCODE);
    }

DWG_OBJECT_END

/*(66)*/
DWG_OBJECT(APPID_CONTROL);

  FIELD_BS(num_entries);
  UNTIL(R_2007)
    {
      FIELD_HANDLE (null_handle, 4);
    }
  XDICOBJHANDLE(3);
  HANDLE_VECTOR(apps, num_entries, 2);

DWG_OBJECT_END

/*(67)*/
DWG_OBJECT(APPID);

  FIELD_TV (entry_name);
  FIELD_B (_64_flag);
  SINCE(R_2010)
    {
      FIELD_B(xrefdep);
      if (FIELD_VALUE(xrefdep))
        {
          FIELD_BS(xrefindex_plus1);
        }
    }
  OTHER_VERSIONS
    {
      FIELD_BS(xrefindex_plus1);
      FIELD_B(xrefdep);
    }
  FIELD_RC (unknown);

  UNTIL(R_2007)
    {
      FIELD_HANDLE (app_control, 4);
      REACTORS(4);
      XDICOBJHANDLE(3);
      FIELD_HANDLE (null_handle, 5);
    }

DWG_OBJECT_END

/*(68)*/
DWG_OBJECT(DIMSTYLE_CONTROL);

  FIELD_BS (num_entries);
  SINCE(R_2000)
    {
      /*
      This is not stated in the spec.
       I've seen it in a R2000 file but not on a R14,
       so I'll assume it is a R_2000+ field.
      TODO: this should be checked with more sample files from various DWG versions.
        ~Juca
      */
      FIELD_RC(unknown);
    }

  FIELD_HANDLE (null_handle, 4);
  XDICOBJHANDLE(3);
  HANDLE_VECTOR (dimstyles, num_entries, 2);

DWG_OBJECT_END

/*(69)*/
DWG_OBJECT(DIMSTYLE);

  FIELD_TV (entry_name);
  FIELD_B (_64_flag);
  SINCE(R_2010)
    {
      FIELD_B(xrefdep);
      if (FIELD_VALUE(xrefdep))
        {
          FIELD_BS(xrefindex_plus1);
        }
    }
  OTHER_VERSIONS
    {
      FIELD_BS(xrefindex_plus1);
      FIELD_B(xrefdep);
    }

  VERSIONS(R_13, R_14)
    {
      FIELD_B (DIMTOL);
      FIELD_B (DIMLIM);
      FIELD_B (DIMTIH);
      FIELD_B (DIMTOH);
      FIELD_B (DIMSE1);
      FIELD_B (DIMSE2);
      FIELD_B (DIMALT);
      FIELD_B (DIMTOFL);
      FIELD_B (DIMSAH);
      FIELD_B (DIMTIX);
      FIELD_B (DIMSOXD);
      FIELD_CAST (DIMALTD, RC, BS);
      FIELD_CAST (DIMZIN, RC, BS);
      FIELD_B (DIMSD1);
      FIELD_B (DIMSD2);
      FIELD_CAST (DIMTOLJ, RC, BS);
      FIELD_CAST (DIMJUST, RC, BS);
      FIELD_CAST (DIMFIT, RC, BS);
      FIELD_B (DIMUPT);
      FIELD_CAST (DIMTZIN, RC, BS);
      FIELD_CAST (DIMMALTZ, RC, BS);
      FIELD_CAST (DIMMALTTZ, RC, BS);
      FIELD_CAST (DIMTAD, RC, BS);
      FIELD_BS (DIMUNIT);
      FIELD_BS (DIMAUNIT);
      FIELD_BS (DIMDEC);
      FIELD_BS (DIMTDEC);
      FIELD_BS (DIMALTU);
      FIELD_BS (DIMALTTD);
      FIELD_BD (DIMSCALE);
      FIELD_BD (DIMASZ);
      FIELD_BD (DIMEXO);
      FIELD_BD (DIMDLI);
      FIELD_BD (DIMEXE);
      FIELD_BD (DIMRND);
      FIELD_BD (DIMDLE);
      FIELD_BD (DIMTP);
      FIELD_BD (DIMTM);
      FIELD_BD (DIMTXT);
      FIELD_BD (DIMCEN);
      FIELD_BD (DIMTSZ);
      FIELD_BD (DIMALTF);
      FIELD_BD (DIMLFAC);
      FIELD_BD (DIMTVP);
      FIELD_BD (DIMTFAC);
      FIELD_BD (DIMGAP);
      FIELD_T (DIMPOST_T);
      FIELD_T (DIMAPOST_T);
      FIELD_T (DIMBLK_T);
      FIELD_T (DIMBLK1_T);
      FIELD_T (DIMBLK2_T);
      FIELD_CMC (DIMCLRD);
      FIELD_CMC (DIMCLRE);
      FIELD_CMC (DIMCLRT);
    }

  SINCE(R_2000)
    {
      FIELD_TV (DIMPOST);
      FIELD_TV (DIMAPOST);
      FIELD_BD (DIMSCALE);
      FIELD_BD (DIMASZ);
      FIELD_BD (DIMEXO);
      FIELD_BD (DIMDLI);
      FIELD_BD (DIMEXE);
      FIELD_BD (DIMRND);
      FIELD_BD (DIMDLE);
      FIELD_BD (DIMTP);//
      FIELD_BD (DIMTM);//
    }

  SINCE(R_2007)
    {
      FIELD_BD (DIMFXL);
      FIELD_BD (DIMJOGANG);
      FIELD_BS (DIMTFILL);
      FIELD_CMC (DIMTFILLCLR);
    }

  SINCE(R_2000)
    {
      FIELD_B (DIMTOL);
      FIELD_B (DIMLIM);
      FIELD_B (DIMTIH);
      FIELD_B (DIMTOH);
      FIELD_B (DIMSE1);
      FIELD_B (DIMSE2);
      FIELD_BS (DIMTAD);
      FIELD_BS (DIMZIN);
      FIELD_BS (DIMAZIN);
    }

  SINCE(R_2007)
    {
      FIELD_BS (DIMARCSYM);
    }

  SINCE(R_2000)
    {
      FIELD_BD (DIMTXT);
      FIELD_BD (DIMCEN);
      FIELD_BD (DIMTSZ);
      FIELD_BD (DIMALTF);
      FIELD_BD (DIMLFAC);
      FIELD_BD (DIMTVP);
      FIELD_BD (DIMTFAC);
      FIELD_BD (DIMGAP);
      FIELD_BD (DIMALTRND);
      FIELD_B (DIMALT);
      FIELD_BS (DIMALTD);
      FIELD_B (DIMTOFL);
      FIELD_B (DIMSAH);
      FIELD_B (DIMTIX);
      FIELD_B (DIMSOXD);
      FIELD_CMC (DIMCLRD);
      FIELD_CMC (DIMCLRE);
      FIELD_CMC (DIMCLRT);
      FIELD_BS (DIMADEC);
      FIELD_BS (DIMDEC);
      FIELD_BS (DIMTDEC);
      FIELD_BS (DIMALTU);
      FIELD_BS (DIMALTTD);
      FIELD_BS (DIMAUNIT);
      FIELD_BS (DIMFRAC);
      FIELD_BS (DIMLUNIT);
      FIELD_BS (DIMDSEP);
      FIELD_BS (DIMTMOVE);
      FIELD_BS (DIMJUST);
      FIELD_B (DIMSD1);
      FIELD_B (DIMSD2);
      FIELD_BS (DIMTOLJ);
      FIELD_BS (DIMTZIN);
      FIELD_BS (DIMALTZ);
      FIELD_BS (DIMALTTZ);
      FIELD_B (DIMUPT);
      FIELD_BS (DIMFIT);
    }

  SINCE(R_2007)
    {
      FIELD_B (DIMFXLON);
    }

  SINCE(R_2000)
    {
      FIELD_BS (DIMLWD);
      FIELD_BS (DIMLWE);
    }

  FIELD_B (unknown);

  FIELD_HANDLE(dimstyle_control, 4);
  REACTORS(4);
  XDICOBJHANDLE(3);
  UNTIL(R_2007)
    {
      FIELD_HANDLE(null_handle, 5);
    }
  FIELD_HANDLE(shapefile, 5); /* Text style (DIMTXSTY) */

  SINCE(R_2000)
    {
      FIELD_HANDLE(leader_block,5); /* Leader arrow	(DIMLDRBLK)*/
      FIELD_HANDLE(dimblk,5); /* Arrow */
      FIELD_HANDLE(dimblk1,5); /* Arrow 1 */
      FIELD_HANDLE(dimblk2,5); /* Arrow 2 */
    }

  SINCE(R_2007)
    {
      FIELD_HANDLE(dimltype, ANYCODE);
      FIELD_HANDLE(dimltex1, ANYCODE);
      FIELD_HANDLE(dimltex2, ANYCODE);
    }

DWG_OBJECT_END

/*(70)*/
DWG_OBJECT(VP_ENT_HDR_CONTROL);

  FIELD_BS (num_entries);
  UNTIL(R_2007)
    {
      FIELD_HANDLE (null_handle, 4);
    }
  XDICOBJHANDLE(3);
  HANDLE_VECTOR(viewport_entity_headers, num_entries, 2);

DWG_OBJECT_END

/*(71)*/
DWG_OBJECT(VP_ENT_HDR);

  FIELD_TV(entry_name);
  FIELD(_64_flag, B);
  SINCE(R_2010)
    {
      FIELD_B(xrefdep);
      if (FIELD_VALUE(xrefdep))
        {
          FIELD_BS(xrefindex_plus1);
        }
    }
  OTHER_VERSIONS
    {
      FIELD_BS(xrefindex_plus1);
      FIELD_B(xrefdep);
    }
  FIELD(one_flag, B);
  FIELD_HANDLE(vp_ent_ctrl, ANYCODE);
  XDICOBJHANDLE(3);
  FIELD_HANDLE(null, 5);

DWG_OBJECT_END

/*(72)*/
DWG_OBJECT(GROUP);

  FIELD_TV (str);
  FIELD_BS (unnamed);
  FIELD_BS (selectable);
  FIELD_BL (num_handles);
  UNTIL(R_2007)
    {
      FIELD_HANDLE (parenthandle, 4);
    }
  REACTORS(4);
  XDICOBJHANDLE(3);
  HANDLE_VECTOR(group_entries, num_handles, 5);

DWG_OBJECT_END

/*(73)*/
DWG_OBJECT(MLINESTYLE);

  FIELD_TV(name);
  FIELD_TV(desc);
  FIELD_BS(flags);
  FIELD_CMC(fillcolor);
  FIELD_BD(startang);
  FIELD_BD(endang);
  FIELD_RC (linesinstyle);

  REPEAT(linesinstyle, lines, Dwg_Object_MLINESTYLE_line)
  {
    FIELD_BD(lines[rcount].offset);
#ifndef IS_FREE
    FIELD_CMC(lines[rcount].color);
#endif
    FIELD_BS(lines[rcount].ltindex);
  }
  UNTIL(R_2007)
    {
      FIELD_HANDLE(parenthandle, 8); // was 4
    }
  REACTORS(4);
  XDICOBJHANDLE(3);

DWG_OBJECT_END


//pg.135
DWG_OBJECT(DICTIONARYVAR);

  FIELD_RC(intval);
  FIELD_TV(str);

  UNTIL(R_2007)
    {
      FIELD_HANDLE(parenthandle, 4);
    }
  REACTORS(4);
  XDICOBJHANDLE(3);

DWG_OBJECT_END

//(78 + varies) pg.136
DWG_ENTITY(HATCH);

  SINCE(R_2004)
    {
      FIELD_BL (is_gradient_fill);
      FIELD_BL (reserved);
      FIELD_BD (gradient_angle);
      FIELD_BD (gradient_shift);
      FIELD_BL (single_color_gradient);
      FIELD_BD (gradient_tint);
      FIELD_BL (num_colors);
      REPEAT(num_colors, colors, Dwg_Entity_HATCH_Color)
        {
          FIELD_BD(colors[rcount].unknown_double);
          FIELD_BS(colors[rcount].unknown_short);
          FIELD_BL(colors[rcount].rgb_color);
          FIELD_RC(colors[rcount].ignored_color_byte);
        }
      FIELD_TV (gradient_name);
    }

  FIELD_BD (elevation);
  FIELD_3BD (extrusion);
  FIELD_TV (name);
  FIELD_B (solid_fill);
  FIELD_B (associative);
  FIELD_BL (num_paths);
  REPEAT(num_paths, paths, Dwg_Entity_HATCH_Path)
    {
      FIELD_BL(paths[rcount].flag);
      if (!(FIELD_VALUE(paths[rcount].flag) & 2))
        {
          FIELD_BL(paths[rcount].num_segs_or_paths);
          REPEAT2(paths[rcount].num_segs_or_paths, paths[rcount].segs,
                  Dwg_Entity_HATCH_PathSeg)
            {
              FIELD_RC(paths[rcount].segs[rcount2].type_status);
              switch (FIELD_VALUE(paths[rcount].segs[rcount2].type_status))
                {
                    case 1: /* LINE */
                      FIELD_2RD(paths[rcount].segs[rcount2].first_endpoint);
                      FIELD_2RD(paths[rcount].segs[rcount2].second_endpoint);
                      break;
                    case 2: /* CIRCULAR ARC */
                      FIELD_2RD(paths[rcount].segs[rcount2].center);
                      FIELD_BD(paths[rcount].segs[rcount2].radius);
                      FIELD_BD(paths[rcount].segs[rcount2].start_angle);
                      FIELD_BD(paths[rcount].segs[rcount2].end_angle);
                      FIELD_B(paths[rcount].segs[rcount2].is_ccw);
                      break;
                    case 3: /* ELLIPTICAL ARC */
                      FIELD_2RD(paths[rcount].segs[rcount2].center);
                      FIELD_2RD(paths[rcount].segs[rcount2].endpoint);
                      FIELD_BD(paths[rcount].segs[rcount2].minor_major_ratio);
                      FIELD_BD(paths[rcount].segs[rcount2].start_angle);
                      FIELD_BD(paths[rcount].segs[rcount2].end_angle);
                      FIELD_B(paths[rcount].segs[rcount2].is_ccw);
                      break;
                    case 4: /* SPLINE */
                      FIELD_BL(paths[rcount].segs[rcount2].degree);
                      FIELD_B(paths[rcount].segs[rcount2].is_rational);
                      FIELD_B(paths[rcount].segs[rcount2].is_periodic);
                      FIELD_BL(paths[rcount].segs[rcount2].num_knots);
                      FIELD_BL(paths[rcount].segs[rcount2].num_control_points);
                      FIELD_VECTOR(paths[rcount].segs[rcount2].knots, BD,
                                   paths[rcount].segs[rcount2].num_knots)
                      REPEAT3(paths[rcount].segs[rcount2].num_control_points,
                              paths[rcount].segs[rcount2].control_points,
                              Dwg_Entity_HATCH_ControlPoint)
                        {
                          FIELD_2RD(paths[rcount].segs[rcount2].control_points[rcount3].point);
                          if (FIELD_VALUE(paths[rcount].segs[rcount2].is_rational))
                            {
                              FIELD_BD(paths[rcount].segs[rcount2].control_points[rcount3].weigth);
                            }
                        }
                      break;
                    default:
                      LOG_ERROR("Invalid type_status in HATCH entity\n")
                      break;
                }
            }
        }
      else
        { /* POLYLINE PATH */
          FIELD_B(paths[rcount].bulges_present);
          FIELD_B(paths[rcount].closed);
          FIELD_BL(paths[rcount].num_segs_or_paths);
          REPEAT2(paths[rcount].num_segs_or_paths, paths[rcount].polyline_paths,
                  Dwg_Entity_HATCH_PolylinePath)
            {
              FIELD_2RD (paths[rcount].polyline_paths[rcount2].point);
              if (FIELD_VALUE(paths[rcount].bulges_present))
                {
                  FIELD_BD (paths[rcount].polyline_paths[rcount2].bulge);
                }
            }
        }
      FIELD_BL(paths[rcount].num_boundary_handles);
      DECODER {
        FIELD_VALUE (num_boundary_handles) += FIELD_VALUE(paths[rcount].num_boundary_handles);
        FIELD_VALUE (has_derived) =
          FIELD_VALUE (has_derived) || (FIELD_VALUE (paths[rcount].flag) & 0x4);
      }
    }
  FIELD_BS(style);
  FIELD_BS(pattern_type);
  if (!FIELD_VALUE(solid_fill))
    {
      FIELD_BD (angle);
      FIELD_BD (scale_spacing);
      FIELD_B (double_flag);
      FIELD_BS (num_deflines);
      REPEAT (num_deflines, deflines, Dwg_Entity_HATCH_DefLine)
        {
          FIELD_BD (deflines[rcount].angle);
          FIELD_2BD (deflines[rcount].pt0);
          FIELD_2BD (deflines[rcount].offset);
          FIELD_BS (deflines[rcount].num_dashes);
          REPEAT2 (deflines[rcount].num_dashes, deflines[rcount].dashes, BITCODE_BD)
            {
              FIELD_BD (deflines[rcount].dashes[rcount2]);
            }
        }
    }

  if (FIELD_VALUE (has_derived))
    {
      FIELD_BD (pixel_size);
    }
  FIELD_BL (num_seeds);
  FIELD_2RD_VECTOR (seeds, num_seeds)

  COMMON_ENTITY_HANDLE_DATA;
  HANDLE_VECTOR (boundary_handles, num_boundary_handles, ANYCODE);

DWG_OBJECT_END

//pg.139
DWG_OBJECT(IDBUFFER);

  FIELD_RC (unknown);
  FIELD_BL (num_ids);
  UNTIL(R_2007)
    {
      FIELD_HANDLE (parenthandle, 4);
    }
  REACTORS(4);
  XDICOBJHANDLE(3);
  HANDLE_VECTOR(obj_ids, num_ids, 4);

DWG_OBJECT_END

//pg.140
DWG_ENTITY(IMAGE);

  FIELD_BL (class_version);
  FIELD_3DPOINT(pt0);
  FIELD_3DPOINT(uvec);
  FIELD_3DPOINT(vvec);
  FIELD_RD(size.width);
  FIELD_RD(size.height);
  FIELD_BS(display_props);
  FIELD(clipping, B);
  FIELD_RC (brightness);
  FIELD_RC (contrast);
  FIELD_RC (fade);

  FIELD_BS(clip_boundary_type);
  if (FIELD_VALUE(clip_boundary_type) == 1)
    {
      FIELD_2RD(boundary_pt0);
      FIELD_2RD(boundary_pt1);
    }
  else
    {
      FIELD_BL (num_clip_verts);
      REPEAT(num_clip_verts, clip_verts, Dwg_Entity_IMAGE_clip_vert)
      {
        FIELD_2RD(clip_verts[rcount]);
      }
    }

  COMMON_ENTITY_HANDLE_DATA;
  FIELD_HANDLE(imagedef,5);
  FIELD_HANDLE(imagedefreactor,3);

DWG_ENTITY_END

//pg.142 test-data/*/Leader_*.dwg
DWG_OBJECT(IMAGEDEF);

  FIELD_BL (class_version);
  FIELD_2RD (image_size);
  FIELD_TV (file_path);
  FIELD_B (is_loaded);
  FIELD_RC (resunits);
  FIELD_2RD (pixel_size);
  UNTIL(R_2007) //?
    {
      FIELD_HANDLE (parenthandle, 3);
    }
  SINCE(R_2010)
    {
      FIELD_HANDLE(null_handle, 5);
    }
  REACTORS(4);
  XDICOBJHANDLE(3);

DWG_OBJECT_END

//PG.143
DWG_OBJECT(IMAGEDEF_REACTOR);

  FIELD_BL (class_version);
  UNTIL(R_2007)
    {
      FIELD_HANDLE (parenthandle, 4);
    }
  REACTORS(4);
  XDICOBJHANDLE(3);

DWG_OBJECT_END

//pg.144
DWG_OBJECT(LAYER_INDEX);

  FIELD_BL (timestamp1);
  FIELD_BL (timestamp2);
  FIELD_BL (num_entries);
  REPEAT (num_entries, entries, Dwg_LAYER_entry)
    {
      FIELD_BL (entries[rcount].index_long);
      FIELD_TV (entries[rcount].index_str);
    }
  UNTIL(R_2007)
    {
      FIELD_HANDLE (parenthandle, 4);
    }
  REACTORS(4);
  XDICOBJHANDLE(3);
  HANDLE_VECTOR(entry_handles, num_entries, ANYCODE);

DWG_OBJECT_END

//pg.145
DWG_OBJECT(LAYOUT);

  FIELD_TV(page_setup_name);
  FIELD_TV(printer_or_config);
  FIELD_BS(plot_layout_flags);
  FIELD_BD(left_margin);
  FIELD_BD(bottom_margin);
  FIELD_BD(right_margin);
  FIELD_BD(top_margin);
  FIELD_BD(paper_width);
  FIELD_BD(paper_height);
  FIELD_TV(paper_size);
  FIELD_2BD(plot_origin);
  FIELD_BS(paper_units);
  FIELD_BS(plot_rotation);
  FIELD_BS(plot_type);
  FIELD_2BD(window_min);
  FIELD_2BD(window_max);

  VERSIONS(R_13,R_2000)
    {
      FIELD_TV(plot_view_name);
    }

  FIELD_BD(real_world_units);
  FIELD_BD(drawing_units);
  FIELD_TV(current_style_sheet);
  FIELD_BS(scale_type);
  FIELD_BD(scale_factor);
  FIELD_2BD(paper_image_origin);

  SINCE(R_2004)
    {
      FIELD_BS(shade_plot_mode);
      FIELD_BS(shade_plot_res_level);
      FIELD_BS(shade_plot_custom_dpi);
    }

  FIELD_TV(layout_name);
  FIELD_BS(tab_order);
  FIELD_BS(flags);
  FIELD_3DPOINT(ucs_origin);
  FIELD_2RD(minimum_limits);
  FIELD_2RD(maximum_limits);
  FIELD_3DPOINT(ins_point);
  FIELD_3DPOINT(ucs_x_axis);
  FIELD_3DPOINT(ucs_y_axis);
  FIELD_BD(elevation);
  FIELD_BS(orthoview_type);
  FIELD_3DPOINT(extent_min);
  FIELD_3DPOINT(extent_max);

  SINCE(R_2004)
    {
      FIELD_BL(viewport_count);
    }
  UNTIL(R_2007)
    {
      FIELD_HANDLE(parenthandle, 4);
    }
  REACTORS(4);
  XDICOBJHANDLE(3);

  SINCE(R_2004)
    {
      FIELD_HANDLE(plot_view_handle, ANYCODE);
    }

  SINCE(R_2007)
    {
      FIELD_HANDLE(visual_style_handle, ANYCODE);
    }

  FIELD_HANDLE(associated_paperspace_block_record_handle, ANYCODE);
  FIELD_HANDLE(last_active_viewport_handle, ANYCODE);
  FIELD_HANDLE(base_ucs_handle, ANYCODE);
  FIELD_HANDLE(named_ucs_handle, ANYCODE);

  SINCE(R_2004)
    {
      HANDLE_VECTOR(viewport_handles, viewport_count, ANYCODE);
    }

DWG_OBJECT_END

//pg.147
DWG_ENTITY(LWPLINE);

  FIELD_BS(flags);

  if (FIELD_VALUE(flags) & 4)
    {
      FIELD_BD(const_width);
    }
  if (FIELD_VALUE(flags) & 8)
    {
      FIELD_BD(elevation);
    }
  if (FIELD_VALUE(flags) & 2)
    {
      FIELD_BD(thickness);
    }
  if (FIELD_VALUE(flags) & 1)
    {
      FIELD_3BD(normal);
    }

  FIELD_BL(num_points);

  if (FIELD_VALUE(flags) & 16)
    {
      FIELD_BL(num_bulges);
    }
  if (FIELD_VALUE(flags) & 32)
    {
      FIELD_BL(num_widths);
    }

  VERSIONS(R_13,R_14)
    {
      FIELD_2RD_VECTOR(points, num_points);
    }

  SINCE(R_2000)
    {
      FIELD_2DD_VECTOR(points, num_points);
    }

  FIELD_VECTOR(bulges, BD, num_bulges);
  REPEAT(num_widths, widths, Dwg_Entity_LWPLINE_width)
    {
      FIELD_BD(widths[rcount].start);
      FIELD_BD(widths[rcount].end);
    }

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

//(74+varies) pg.149
DWG_ENTITY(OLE2FRAME);

  FIELD_BS(flags);

  SINCE(R_2000)
    {
      FIELD_BS(mode);
    }

  FIELD_BL(data_length);
  FIELD_VECTOR(data, RC, data_length);

  SINCE(R_2000)
    {
      FIELD_RC(unknown);
    }

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

//pg.276
#if 0 /* no proxy subtypes yet */
DWG_ENTITY(PROXY_LWPOLYLINE);

  FIELD_RL(size);
  FIELD_BS(flags);

  if (FIELD_VALUE(flags) & 4)
    {
      FIELD_BD(const_width);
    }
  if (FIELD_VALUE(flags) & 8)
    {
      FIELD_BD(elevation);
    }
  if (FIELD_VALUE(flags) & 2)
    {
      FIELD_BD(thickness);
    }
  if (FIELD_VALUE(flags) & 1)
    {
      FIELD_3BD(normal);
    }

  FIELD_BL(num_points);

  if (FIELD_VALUE(flags) & 16)
    {
      FIELD_BL(num_bulges);
    }
  if (FIELD_VALUE(flags) & 32)
    {
      FIELD_BL(num_widths);
    }

  VERSIONS(R_13,R_14)
    {
      FIELD_2RD_VECTOR(points, num_points);
    }

  SINCE(R_2000)
    {
      FIELD_2DD_VECTOR(points, num_points);
    }

  FIELD_VECTOR(bulges, BD, num_bulges);
  REPEAT(num_widths, widths, Dwg_Entity_LWPLINE_width)
    {
      FIELD_BD(widths[rcount].start);
      FIELD_BD(widths[rcount].end);
    }

  FIELD_RC(unknown_1);
  FIELD_RC(unknown_2);
  FIELD_RC(unknown_3);

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END
#endif /* no proxy subtypes yet */

//(498) pg.149 r2000+
DWG_ENTITY(PROXY_ENTITY);

  FIELD_BL (class_id);
  FIELD_BL (object_drawing_format);
  FIELD_B (original_data_format);

  LOG_INFO("TODO PROXY_ENTITY data\n");
  /*
  //TODO: figure out how to deal with the arbitrary size vector databits described on the spec
  FIELD_RC(*data);
  UNTIL(R_2007)
  {
    FIELD_HANDLE (parenthandle, 4);
  }
  REACTORS(4);
  XDICOBJHANDLE(3);
  FIELD_MS(size);

  COMMON_ENTITY_HANDLE_DATA;

  */

DWG_ENTITY_END

//(499) pg.149 r2000+
DWG_OBJECT(PROXY);

  FIELD_BL (class_id);
  FIELD_BL (object_drawing_format);
  FIELD_B (original_data_format);

  LOG_INFO("TODO PROXY_OBJECT data\n");
  /*
  //TODO: figure out how to deal with the arbitrary size vector databits
  FIELD_RC(*data);
  UNTIL(R_2007)
  {
    FIELD_HANDLE (parenthandle, 4);
  }
  REACTORS(4);
  XDICOBJHANDLE(3);
  */

DWG_OBJECT_END

DWG_OBJECT(OBJECTCONTEXTDATA);

  SINCE(R_2010) {
    IF_ENCODE_FROM_EARLIER {
      FIELD_VALUE(version) = 3;
    }
    FIELD_BS (version);
  }
  FIELD_B (has_file);
  FIELD_B (defaultflag);

DWG_OBJECT_END

DWG_OBJECT(CELLSTYLEMAP);

  SINCE(R_2000)
    {
      FIELD_BL(num_cells);
      REPEAT_N((long)FIELD_VALUE(num_cells), cells, Dwg_Object_CELLSTYLEMAP_Cell)
        {
          LOG_INFO("TODO CELLSTYLEMAP TABLE_Cell struct\n");
          // TODO read the TABLE_Cell struct
          FIELD_BL(cells[rcount].id);
          FIELD_BL(cells[rcount].type);
          FIELD_TV(cells[rcount].name);
        }
    }
DWG_OBJECT_END

DWG_OBJECT(FIELD);

  SINCE(R_2004)
    {
      LOG_INFO("TODO FIELD\n");
      FIELD_TV(id);
      FIELD_TV(code);

      FIELD_BL(num_childhdl);
      HANDLE_VECTOR(childhdl, num_childhdl, 360);
      FIELD_BL(num_objects);
      HANDLE_VECTOR(objects, num_objects, 331);

      UNTIL(R_2004)
      {
        BITCODE_TV format;
      }

      FIELD_BL(evaluation_option);
      FIELD_BL(filing_option);
      FIELD_BL(field_state);
      FIELD_BL(evaluation_status);
      FIELD_BL(evaluation_error_code);
      FIELD_TV(evaluation_error_msg);
      FIELD_BL (value); /* TODO value p20.4.99 */
      FIELD_TV(value_string);
      FIELD_TV(value_string_length);

      FIELD_BL(num_childval);
      REPEAT_N((long)FIELD_VALUE(num_childval), childval, Dwg_Object_FIELD_ChildValue)
        {
          FIELD_TV(childval[rcount].key);
          FIELD_BL (childval[rcount].value); /* TODO value p20.4.99 */
        }
    }
DWG_OBJECT_END

DWG_OBJECT(FIELDLIST);

  SINCE(R_2018) {
    FIELD_BL (num_fields);
    FIELD_B (unknown);
    HANDLE_VECTOR (field_handles, num_fields, 0);
  }

DWG_OBJECT_END

//pg.150
DWG_OBJECT(RASTERVARIABLES);

  FIELD_BL (version);
  FIELD_BS (display_frame);
  FIELD_BS (display_quality);
  FIELD_BS (units);
  UNTIL(R_2007)
    {
      FIELD_HANDLE (parenthandle, 4);
    }
  REACTORS(4);
  XDICOBJHANDLE(3);

DWG_OBJECT_END

//pg.151
DWG_OBJECT(SORTENTSTABLE);

  FIELD_BL (num_entries);
  HANDLE_VECTOR (sort_handles, num_entries, 0);
  UNTIL(R_2007)
    {
      FIELD_HANDLE (parenthandle, 4);
    }
  REACTORS(4);
  XDICOBJHANDLE(3);
  UNTIL(R_2007)
    {
      FIELD_HANDLE (owner_handle, 4);
    }
  HANDLE_VECTOR (object_handles, num_entries, 4);

DWG_OBJECT_END

//pg.152
DWG_OBJECT(SPATIAL_FILTER);

  FIELD_BS (num_points);
  FIELD_2RD_VECTOR (points, num_points);
  FIELD_3BD (extrusion);
  FIELD_3BD (clip_bound_origin);
  FIELD_BS (display_boundary);
  FIELD_BS (front_clip_on);
  if (FIELD_VALUE(front_clip_on)==1)
    {
      FIELD_BD (front_clip_dist);
    }

  FIELD_BS (back_clip_on);
  if (FIELD_VALUE(back_clip_on)==1)
    {
      FIELD_BD (back_clip_dist);
    }

  FIELD_VECTOR_N (inverse_block_transform, BD, 12);
  FIELD_VECTOR_N (clip_bound_transform, BD, 12);
  UNTIL(R_2007)
    {
      FIELD_HANDLE (parenthandle, 4);
    }
  REACTORS(4);
  XDICOBJHANDLE(3);

DWG_OBJECT_END

//pg.153
DWG_OBJECT(SPATIAL_INDEX);

  FIELD_BL (timestamp1);
  FIELD_BL (timestamp2);
  //TODO: parse this: "unknown X rest of bits to handles"
/*
  UNTIL(R_2007)
    {
      FIELD_HANDLE (parenthandle, 4);
    }
  REACTORS(4);
  XDICOBJHANDLE(3);
*/
DWG_OBJECT_END

//pg.158
DWG_ENTITY(TABLE);
  //int total_attr_def_count = 0;

  FIELD_3BD (insertion_point);

  VERSIONS(R_13,R_14)
    {
      FIELD_3BD (scale);
    }

  SINCE(R_2000)
    {
      FIELD_BB (data_flags);
      switch (FIELD_VALUE(data_flags))
        {
          case 0:
            FIELD_VALUE(scale.x) = 1.0;
            FIELD_DD(scale.y, FIELD_VALUE(scale.x));
            FIELD_DD(scale.z, FIELD_VALUE(scale.x));
            break;
          case 1:
            FIELD_VALUE(scale.x) = 1.0;
            FIELD_DD(scale.y, 1.0);
            FIELD_DD(scale.z, 1.0);
            break;
          case 2:
            FIELD_RD(scale.x);
            FIELD_VALUE(scale.y) = FIELD_VALUE(scale.x);
            FIELD_VALUE(scale.z) = FIELD_VALUE(scale.x);
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

  FIELD_BD (rotation);
  FIELD_3BD (extrusion);
  FIELD_B (has_attribs);

  SINCE(R_2004)
    {
      FIELD_BL (owned_object_count);
    }

  FIELD_BS (flag_for_table_value);
  FIELD_3BD (horiz_direction);
  FIELD_BL (num_cols);
  FIELD_BL (num_rows);
  FIELD_VECTOR(col_widths, BD, num_cols);
  FIELD_VECTOR(row_heights, BD, num_rows);
  REPEAT_N((long)(FIELD_VALUE(num_rows)*FIELD_VALUE(num_cols)), cells, Dwg_Entity_TABLE_Cell)
    {
      FIELD_BS(cells[rcount].type);
      FIELD_RC(cells[rcount].flags);
      FIELD_B(cells[rcount].merged_value);
      FIELD_B(cells[rcount].autofit_flag);
      FIELD_BL(cells[rcount].merged_width_flag);
      FIELD_BL(cells[rcount].merged_height_flag);
      FIELD_BD(cells[rcount].rotation_value);

      if (FIELD_VALUE(cells[rcount].type)==1)
        { /* text cell */
          FIELD_TV(cells[rcount].text_string);
        }
      if (FIELD_VALUE(cells[rcount].type)==2)
        { /* block cell */
          FIELD_BD(cells[rcount].block_scale);
          FIELD_B(cells[rcount].additional_data_flag);
          if (FIELD_VALUE(cells[rcount].additional_data_flag) == 1)
            {
              FIELD_BS(cells[rcount].attr_def_count);
              FIELD_BS(cells[rcount].attr_def_index);
              FIELD_TV(cells[rcount].attr_def_text);
              //total_attr_def_count += FIELD_VALUE(cells[rcount].attr_def_count);
            }
        }
      if (FIELD_VALUE(cells[rcount].type)==1 || FIELD_VALUE(cells[rcount].type)==2)
        { /* common to both text and block cells */
          FIELD_B(cells[rcount].additional_data_flag);
          if (FIELD_VALUE(cells[rcount].additional_data_flag) == 1)
            {
              FIELD_BL(cells[rcount].cell_flag_override);
              FIELD_RC(cells[rcount].virtual_edge_flag);
              if (FIELD_VALUE(cells[rcount].cell_flag_override) & 0x01)
                {
                  FIELD_RS(cells[rcount].cell_alignment);
                }
              if (FIELD_VALUE(cells[rcount].cell_flag_override) & 0x02)
                {
                  FIELD_B(cells[rcount].background_fill_none);
                }
              if (FIELD_VALUE(cells[rcount].cell_flag_override) & 0x04)
                {
                  FIELD_CMC(cells[rcount].background_color);
                }
              if (FIELD_VALUE(cells[rcount].cell_flag_override) & 0x08)
                {
                  FIELD_CMC(cells[rcount].content_color);
                }
              if (FIELD_VALUE(cells[rcount].cell_flag_override) & 0x20)
                {
                  FIELD_BD(cells[rcount].text_height);
                }
              if (FIELD_VALUE(cells[rcount].cell_flag_override) & 0x00040)
                {
                  FIELD_CMC(cells[rcount].top_grid_color);
                }
              if (FIELD_VALUE(cells[rcount].cell_flag_override) & 0x00400)
                {
                  FIELD_BS(cells[rcount].top_grid_linewt);
                }
              if (FIELD_VALUE(cells[rcount].cell_flag_override) & 0x04000)
                {
                  FIELD_BS(cells[rcount].top_visibility);
                }
              if (FIELD_VALUE(cells[rcount].cell_flag_override) & 0x00080)
                {
                  FIELD_CMC(cells[rcount].right_grid_color);
                }
              if (FIELD_VALUE(cells[rcount].cell_flag_override) & 0x00800)
                {
                  FIELD_BS(cells[rcount].right_grid_linewt);
                }
              if (FIELD_VALUE(cells[rcount].cell_flag_override) & 0x08000)
                {
                  FIELD_BS(cells[rcount].right_visibility);
                }
              if (FIELD_VALUE(cells[rcount].cell_flag_override) & 0x00100)
                {
                  FIELD_CMC(cells[rcount].bottom_grid_color);
                }
              if (FIELD_VALUE(cells[rcount].cell_flag_override) & 0x01000)
                {
                  FIELD_BS(cells[rcount].bottom_grid_linewt);
                }
              if (FIELD_VALUE(cells[rcount].cell_flag_override) & 0x10000)
                {
                  FIELD_BS(cells[rcount].bottom_visibility);
                }
              if (FIELD_VALUE(cells[rcount].cell_flag_override) & 0x00200)
                {
                  FIELD_CMC(cells[rcount].left_grid_color);
                }
              if (FIELD_VALUE(cells[rcount].cell_flag_override) & 0x02000)
                {
                  FIELD_BS(cells[rcount].left_grid_linewt);
                }
              if (FIELD_VALUE(cells[rcount].cell_flag_override) & 0x20000)
                {
                  FIELD_BS(cells[rcount].left_visibility);
                }

              SINCE(R_2007)
                {
                  FIELD_BL(cells[rcount].unknown);
                  FIELD_BL(cells[rcount].flags_2007);
                  FIELD_BL(cells[rcount].data_type);
                  switch (FIELD_VALUE(cells[rcount].data_type))
                    {
                      case 0: /* kLong */
                        FIELD_BL(cells[rcount].data_long);
                        break;
                      case 1: /* kDouble */
                        FIELD_BD(cells[rcount].data_double);
                        break;
                      case 2: /* kString */
                        FIELD_TV(cells[rcount].data_string);
                        break;
                      case 4: /* kDate */
                        FIELD_BL(cells[rcount].data_size);
                        FIELD_VECTOR(cells[rcount].data_date, RC, cells[rcount].data_size);
                        break;
                      case 8: /* kPoint */
                        FIELD_BL(cells[rcount].data_size);
                        FIELD_2RD(cells[rcount].data_point);
                        break;
                      case 16: /* k3dPoint */
                        FIELD_BL(cells[rcount].data_size);
                        FIELD_3RD(cells[rcount].data_3dpoint);
                        break;
                      case 32: /* kObjectId */
                        //data is a HANDLE
                        //read from appropriate place in handles section
                        break;
                      case 64: /* kBuffer */
                        LOG_ERROR("Unknown data type in TABLE entity: \"kBuffer\".\n")
                        break;
                      case 128: /* kResBuf */
                        LOG_ERROR("Unknown data type in TABLE entity: \"kResBuf\".\n")
                        break;
                      case 256: /* kGeneral */
                        LOG_ERROR("Unknown data type in TABLE entity: \"kGeneral\".\n")
                        break;
                      default:
                        LOG_ERROR("Invalid data type in TABLE entity\n")
                        break;
                    }
                  FIELD_BL(cells[rcount].unknown2);
                  FIELD_TV(cells[rcount].format_string);
                  FIELD_TV(cells[rcount].value_string);
                } /* end SINCE(2007)*/
            }
        }
    } /* End Cell Data (remaining data applies to entire table)*/

/* COMMON: */

  FIELD_B (table_overrides_present);
  if (FIELD_VALUE(table_overrides_present)==1)
    {
      FIELD_BL (table_flag_override);
      if (FIELD_VALUE(table_flag_override) & 0x0001)
        {
          FIELD_B (title_suppressed);
        }

      FIELD_B (header_suppressed);

      if (FIELD_VALUE(table_flag_override) & 0x0004)
        {
          FIELD_BS (flow_direction);
        }

      if (FIELD_VALUE(table_flag_override) & 0x0008)
        {
          FIELD_BD (horiz_cell_margin);
        }

      if (FIELD_VALUE(table_flag_override) & 0x0010)
        {
          FIELD_BD (vert_cell_margin);
        }

      if (FIELD_VALUE(table_flag_override) & 0x0020)
        {
          FIELD_CMC (title_row_color);
        }

      if (FIELD_VALUE(table_flag_override) & 0x0040)
        {
          FIELD_CMC (header_row_color);
        }

      if (FIELD_VALUE(table_flag_override) & 0x0080)
        {
          FIELD_CMC (data_row_color);
        }

      if (FIELD_VALUE(table_flag_override) & 0x0100)
        {
          FIELD_B (title_row_fill_none);
        }

      if (FIELD_VALUE(table_flag_override) & 0x0200)
        {
          FIELD_B (header_row_fill_none);
        }

      if (FIELD_VALUE(table_flag_override) & 0x0400)
        {
          FIELD_B (data_row_fill_none);
        }

      if (FIELD_VALUE(table_flag_override) & 0x0800)
        {
          FIELD_CMC (title_row_fill_color);
        }

      if (FIELD_VALUE(table_flag_override) & 0x1000)
        {
          FIELD_CMC (header_row_fill_color);
        }

      if (FIELD_VALUE(table_flag_override) & 0x2000)
        {
          FIELD_CMC (data_row_fill_color);
        }

      if (FIELD_VALUE(table_flag_override) & 0x4000)
        {
          FIELD_BS (title_row_align);
        }

      if (FIELD_VALUE(table_flag_override) & 0x8000)
        {
          FIELD_BS (header_row_align);
        }

      if (FIELD_VALUE(table_flag_override) & 0x10000)
        {
          FIELD_BS (data_row_align);
        }

      if (FIELD_VALUE(table_flag_override) & 0x100000)
        {
          FIELD_BD (title_row_height);
        }

      if (FIELD_VALUE(table_flag_override) & 0x200000)
        {
          FIELD_BD (header_row_height);
        }

      if (FIELD_VALUE(table_flag_override) & 0x400000)
        {
          FIELD_BD (data_row_height);
        }
    }

  FIELD_B (border_color_overrides_present);
  if (FIELD_VALUE(border_color_overrides_present)==1)
    {
      FIELD_BL (border_color_overrides_flag);
      if (FIELD_VALUE(border_color_overrides_flag) & 0x0001)
        {
          FIELD_CMC (title_horiz_top_color);
        }

      if (FIELD_VALUE(border_color_overrides_flag) & 0x0002)
        {
          FIELD_CMC (title_horiz_ins_color);
        }

      if (FIELD_VALUE(border_color_overrides_flag) & 0x0004)
        {
          FIELD_CMC (title_horiz_bottom_color);
        }

      if (FIELD_VALUE(border_color_overrides_flag) & 0x0008)
        {
          FIELD_CMC (title_vert_left_color);
        }

      if (FIELD_VALUE(border_color_overrides_flag) & 0x0010)
        {
          FIELD_CMC (title_vert_ins_color);
        }

      if (FIELD_VALUE(border_color_overrides_flag) & 0x0020)
        {
          FIELD_CMC (title_vert_right_color);
        }

      if (FIELD_VALUE(border_color_overrides_flag) & 0x0040)
        {
          FIELD_CMC (header_horiz_top_color);
        }

      if (FIELD_VALUE(border_color_overrides_flag) & 0x0080)
        {
          FIELD_CMC (header_horiz_ins_color);
        }

      if (FIELD_VALUE(border_color_overrides_flag) & 0x0100)
        {
          FIELD_CMC (header_horiz_bottom_color);
        }

      if (FIELD_VALUE(border_color_overrides_flag) & 0x0200)
        {
          FIELD_CMC (header_vert_left_color);
        }

      if (FIELD_VALUE(border_color_overrides_flag) & 0x0400)
        {
          FIELD_CMC (header_vert_ins_color);
        }

      if (FIELD_VALUE(border_color_overrides_flag) & 0x0800)
        {
          FIELD_CMC (header_vert_right_color);
        }

      if (FIELD_VALUE(border_color_overrides_flag) & 0x1000)
        {
          FIELD_CMC (data_horiz_top_color);
        }

      if (FIELD_VALUE(border_color_overrides_flag) & 0x2000)
        {
          FIELD_CMC (data_horiz_ins_color);
        }

      if (FIELD_VALUE(border_color_overrides_flag) & 0x4000)
        {
          FIELD_CMC (data_horiz_bottom_color);
        }

      if (FIELD_VALUE(border_color_overrides_flag) & 0x8000)
        {
          FIELD_CMC (data_vert_left_color);
        }

      if (FIELD_VALUE(border_color_overrides_flag) & 0x10000)
        {
          FIELD_CMC (data_vert_ins_color);
        }

      if (FIELD_VALUE(border_color_overrides_flag) & 0x20000)
        {
          FIELD_CMC (data_vert_right_color);
        }
    }

  FIELD_B (border_lineweight_overrides_present);
  if (FIELD_VALUE(border_lineweight_overrides_present)==1)
    {
      FIELD_BL (border_lineweight_overrides_flag);
      if (FIELD_VALUE(border_lineweight_overrides_flag) & 0x0001)
        {
          FIELD_BS (title_horiz_top_lineweigh);
        }

      if (FIELD_VALUE(border_lineweight_overrides_flag) & 0x0002)
        {
          FIELD_BS (title_horiz_ins_lineweigh);
        }

      if (FIELD_VALUE(border_lineweight_overrides_flag) & 0x0004)
        {
          FIELD_BS (title_horiz_bottom_lineweigh);
        }

      if (FIELD_VALUE(border_lineweight_overrides_flag) & 0x0008)
        {
          FIELD_BS (title_vert_left_lineweigh);
        }

      if (FIELD_VALUE(border_lineweight_overrides_flag) & 0x0010)
        {
          FIELD_BS (title_vert_ins_lineweigh);
        }

      if (FIELD_VALUE(border_lineweight_overrides_flag) & 0x0020)
        {
          FIELD_BS (title_vert_right_lineweigh);
        }

      if (FIELD_VALUE(border_lineweight_overrides_flag) & 0x0040)
        {
          FIELD_BS (header_horiz_top_lineweigh);
        }

      if (FIELD_VALUE(border_lineweight_overrides_flag) & 0x0080)
        {
          FIELD_BS (header_horiz_ins_lineweigh);
        }

      if (FIELD_VALUE(border_lineweight_overrides_flag) & 0x0100)
        {
          FIELD_BS (header_horiz_bottom_lineweigh);
        }

      if (FIELD_VALUE(border_lineweight_overrides_flag) & 0x0200)
        {
          FIELD_BS (header_vert_left_lineweigh);
        }

      if (FIELD_VALUE(border_lineweight_overrides_flag) & 0x0400)
        {
          FIELD_BS (header_vert_ins_lineweigh);
        }

      if (FIELD_VALUE(border_lineweight_overrides_flag) & 0x0800)
        {
          FIELD_BS (header_vert_right_lineweigh);
        }

      if (FIELD_VALUE(border_lineweight_overrides_flag) & 0x1000)
        {
          FIELD_BS (data_horiz_top_lineweigh);
        }

      if (FIELD_VALUE(border_lineweight_overrides_flag) & 0x2000)
        {
          FIELD_BS (data_horiz_ins_lineweigh);
        }

      if (FIELD_VALUE(border_lineweight_overrides_flag) & 0x4000)
        {
          FIELD_BS (data_horiz_bottom_lineweigh);
        }

      if (FIELD_VALUE(border_lineweight_overrides_flag) & 0x8000)
        {
          FIELD_BS (data_vert_left_lineweigh);
        }

      if (FIELD_VALUE(border_lineweight_overrides_flag) & 0x10000)
        {
          FIELD_BS (data_vert_ins_lineweigh);
        }

      if (FIELD_VALUE(border_lineweight_overrides_flag) & 0x20000)
        {
          FIELD_BS (data_vert_right_lineweigh);
        }
    }

  FIELD_B (border_visibility_overrides_present);
  if (FIELD_VALUE(border_visibility_overrides_present)==1)
    {
      FIELD_BL (border_visibility_overrides_flag);
      if (FIELD_VALUE(border_visibility_overrides_flag) & 0x0001)
        {
          FIELD_BS (title_horiz_top_visibility);
        }

      if (FIELD_VALUE(border_visibility_overrides_flag) & 0x0002)
        {
          FIELD_BS (title_horiz_ins_visibility);
        }

      if (FIELD_VALUE(border_visibility_overrides_flag) & 0x0004)
        {
          FIELD_BS (title_horiz_bottom_visibility);
        }

      if (FIELD_VALUE(border_visibility_overrides_flag) & 0x0008)
        {
          FIELD_BS (title_vert_left_visibility);
        }

      if (FIELD_VALUE(border_visibility_overrides_flag) & 0x0010)
        {
          FIELD_BS (title_vert_ins_visibility);
        }

      if (FIELD_VALUE(border_visibility_overrides_flag) & 0x0020)
        {
          FIELD_BS (title_vert_right_visibility);
        }

      if (FIELD_VALUE(border_visibility_overrides_flag) & 0x0040)
        {
          FIELD_BS (header_horiz_top_visibility);
        }

      if (FIELD_VALUE(border_visibility_overrides_flag) & 0x0080)
        {
          FIELD_BS (header_horiz_ins_visibility);
        }

      if (FIELD_VALUE(border_visibility_overrides_flag) & 0x0100)
        {
          FIELD_BS (header_horiz_bottom_visibility);
        }

      if (FIELD_VALUE(border_visibility_overrides_flag) & 0x0200)
        {
          FIELD_BS (header_vert_left_visibility);
        }

      if (FIELD_VALUE(border_visibility_overrides_flag) & 0x0400)
        {
          FIELD_BS (header_vert_ins_visibility);
        }

      if (FIELD_VALUE(border_visibility_overrides_flag) & 0x0800)
        {
          FIELD_BS (header_vert_right_visibility);
        }

      if (FIELD_VALUE(border_visibility_overrides_flag) & 0x1000)
        {
          FIELD_BS (data_horiz_top_visibility);
        }

      if (FIELD_VALUE(border_visibility_overrides_flag) & 0x2000)
        {
          FIELD_BS (data_horiz_ins_visibility);
        }

      if (FIELD_VALUE(border_visibility_overrides_flag) & 0x4000)
        {
          FIELD_BS (data_horiz_bottom_visibility);
        }

      if (FIELD_VALUE(border_visibility_overrides_flag) & 0x8000)
        {
          FIELD_BS (data_vert_left_visibility);
        }

      if (FIELD_VALUE(border_visibility_overrides_flag) & 0x10000)
        {
          FIELD_BS (data_vert_ins_visibility);
        }

      if (FIELD_VALUE(border_visibility_overrides_flag) & 0x20000)
        {
          FIELD_BS (data_vert_right_visibility);
        }
    }

  FIELD_HANDLE (block_header, 5);

  VERSIONS(R_13, R_2000)
    {
      if (FIELD_VALUE(has_attribs))
        {
          FIELD_HANDLE (first_attrib, 4);
          FIELD_HANDLE (last_attrib, 4);
        }
    }

  SINCE(R_2004)
    {
      HANDLE_VECTOR(attribs, owned_object_count, 4)
    }

  if (FIELD_VALUE(has_attribs))
    {
      FIELD_HANDLE(seqend, 3);
    }

  FIELD_HANDLE(table_style_id, ANYCODE);

  REPEAT_N((long)(FIELD_VALUE(num_rows)*FIELD_VALUE(num_cols)), cells, Dwg_Entity_TABLE_Cell)
    {
      FIELD_HANDLE(cells[rcount].cell_handle, ANYCODE);

      if (FIELD_VALUE(cells[rcount].type) == 2 &&
          FIELD_VALUE(cells[rcount].additional_data_flag) == 1)
        {
          HANDLE_VECTOR(cells[rcount].attr_def_id, cells[rcount].attr_def_count, ANYCODE);
        }

      if (FIELD_VALUE(cells[rcount].additional_data_flag2) == 1 &&
          FIELD_VALUE(cells[rcount].cell_flag_override) & 0x08)
        {
          FIELD_HANDLE(cells[rcount].text_style_override, ANYCODE);
        }
    }

  FIELD_HANDLE (title_row_style_override, ANYCODE);
  FIELD_HANDLE (header_row_style_override, ANYCODE);
  FIELD_HANDLE (data_row_style_override, ANYCODE);

DWG_ENTITY_END

//(79 + varies) pg.164
DWG_OBJECT(XRECORD);

  FIELD_BL(num_databytes);
  FIELD_XDATA(xdata, num_databytes);

  SINCE(R_2000)
  {
    FIELD_BS(cloning_flags);
  }

  UNTIL(R_2007)
    {
      FIELD_HANDLE(parenthandle, ANYCODE); // 3 or 8
    }
  REACTORS(4);
  XDICOBJHANDLE(3);

  DECODER
    {
      for (vcount=0;
           dat->byte < obj->tio.object->datbyte + (obj->tio.object->bitsize/8);
           vcount++)
        {
          FIELD_VALUE(objid_handles) = vcount
            ? (BITCODE_H*) realloc(FIELD_VALUE(objid_handles), sizeof(Dwg_Object_Ref) * (vcount+1))
            : (BITCODE_H*) malloc(sizeof(Dwg_Object_Ref));
          FIELD_HANDLE_N(objid_handles[vcount], vcount, ANYCODE);
        }
      FIELD_VALUE(num_objid_handles) = vcount;
    }

DWG_OBJECT_END

//(80 + varies)
DWG_OBJECT(PLACEHOLDER);

  // no own data members
  UNTIL(R_2007)
    {
      FIELD_HANDLE(parenthandle, 4);
    }
  REACTORS(4);
  XDICOBJHANDLE(3);

DWG_OBJECT_END

// just guessing:
// VBA_PROJECT (81 + varies)
DWG_OBJECT(VBA_PROJECT);

  FIELD_RL(num_bytes)
  FIELD_VECTOR(bytes, RC, num_bytes)

  UNTIL(R_2007)
    {
      FIELD_HANDLE(parenthandle, 4);
    }
  REACTORS(4);
  XDICOBJHANDLE(3);

DWG_OBJECT_END

// SCALE (varies)
DWG_OBJECT(SCALE);

  SINCE(R_2000)
    {
      FIELD_BS(unknown);
      FIELD_TV(name);
      FIELD_BD(paper);
      FIELD_BD(drawing);
      FIELD_B(has_unit_scale);
      REACTORS(ANYCODE);
      XDICOBJHANDLE(3);
    }

DWG_OBJECT_END

/* par 20.4.48 (varies) */
DWG_ENTITY(MLEADER);

  SINCE(R_2010)
    {
      FIELD_BS (version);
      FIELD_BS (ctx.version);
      FIELD_B (ctx.has_xdic_file);
      FIELD_B (ctx.is_default);
      FIELD_HANDLE (ctx.scale_handle, ANYCODE);
    }

  FIELD_BL (ctx.num_leaders);
  REPEAT(ctx.num_leaders, ctx.leaders, Dwg_Leader)
    {
#     define lev1 ctx.leaders[rcount]
      FIELD_B (lev1.unknown1);
      FIELD_B (lev1.unknown2);
      FIELD_3BD (lev1.connection);
      FIELD_3BD (lev1.direction);
      FIELD_BL (lev1.unknown3);
      FIELD_BL (lev1.index);
      FIELD_BL (lev1.num_lines);
      REPEAT2(lev1.num_lines, lev1.lines, Leader_Line)
        {
#         define lev2 lev1.lines[rcount2]
          FIELD_BL (lev2.num_points);
          REPEAT3(lev2.num_points, lev2.points, BITCODE_3BD)
            {
              FIELD_3BD (lev2.points[rcount3]);
            }
          //VECTOR_FREE (lev2.points);

          FIELD_BL (lev2.unknown);
          FIELD_BL (lev2.index);

	  SINCE (R_2010)
            {
              FIELD_BS (lev2.type);
              FIELD_CMC (lev2.color);
              FIELD_HANDLE (lev2.type_handle, ANYCODE);
              FIELD_BL (lev2.weight);
              FIELD_BD (lev2.arrow_size);
              FIELD_HANDLE (lev2.arrow_handle, ANYCODE);
              FIELD_BL (lev2.flags);
            }
#         undef lev2
        }
      //VECTOR_FREE (lev1.lines);
      SINCE (R_2010)
        {
          FIELD_BS (lev1.attach_dir);
        }
#     undef lev1
    }
  //VECTOR_FREE (ctx.leaders);

  FIELD_BD (ctx.scale);
  FIELD_3BD (ctx.content_base);
  FIELD_BD (ctx.text_height);
  FIELD_BD (ctx.arrow_size);
  FIELD_BD (ctx.landing_gap);
  FIELD_BS (ctx.text_left);
  FIELD_BS (ctx.text_right);
  FIELD_BS (ctx.text_align);
  FIELD_BS (ctx.attach_type);

  FIELD_B (ctx.has_text_content);
  if (FIELD_VALUE (ctx.has_text_content))
    {
      FIELD_TV (ctx.txt.label);
      FIELD_3BD (ctx.txt.normal);
      FIELD_HANDLE (ctx.txt.style, ANYCODE);
      FIELD_3BD (ctx.txt.location);
      FIELD_3BD (ctx.txt.direction);
      FIELD_BD (ctx.txt.rotation);
      FIELD_BD (ctx.txt.width);
      FIELD_BD (ctx.txt.heigth);
      FIELD_BD (ctx.txt.spacing_factor);
      FIELD_HANDLE (ctx.txt.spacing_style, ANYCODE);
      FIELD_CMC (ctx.txt.color);
      FIELD_BS (ctx.txt.align);
      FIELD_BS (ctx.txt.flow);
      FIELD_CMC (ctx.txt.bg_color);
      FIELD_BD (ctx.txt.bg_scale);
      FIELD_BL (ctx.txt.bg_transparency);
      FIELD_B (ctx.txt.is_bg_fill);
      FIELD_B (ctx.txt.is_bg_mask_fill);
      FIELD_BS (ctx.txt.col_type);
      FIELD_B (ctx.txt.is_heigth_auto);
      FIELD_BD (ctx.txt.unknown1);
      FIELD_BD (ctx.txt.unknown2);
      FIELD_B (ctx.txt.is_col_flow_reversed);
      FIELD_BL (ctx.txt.unknown3);
      FIELD_B (ctx.txt.word_break1);
      FIELD_B (ctx.txt.word_break2);
    }

  FIELD_B (ctx.has_content_block);
  if (FIELD_VALUE (ctx.has_content_block))
    {
      FIELD_HANDLE (ctx.blk.block_table, ANYCODE);
      FIELD_3BD (ctx.blk.normal);
      FIELD_3BD (ctx.blk.location);
      FIELD_3BD (ctx.blk.scale);
      FIELD_BD (ctx.blk.rotation);
      FIELD_CMC (ctx.blk.color);
      for (rcount = 0; rcount < 16; rcount++)
        {
          FIELD_BD (ctx.blk.transform[rcount]);
        }
    }

  FIELD_3BD (ctx.base);
  FIELD_3BD (ctx.base_dir);
  FIELD_3BD (ctx.base_vert);
  FIELD_B (ctx.is_normal_reversed);

  SINCE (R_2010)
    {
      FIELD_BS (ctx.text_top);
      FIELD_BS (ctx.text_bottom);
    }

  FIELD_HANDLE (leaderstyle, ANYCODE);
  FIELD_BL (flags);
  FIELD_BS (type);
  FIELD_CMC (color);
  FIELD_HANDLE (ltype, ANYCODE);
  FIELD_BL (line_weigth);
  FIELD_B (landing);
  FIELD_B (dog_leg);
  FIELD_BD (landing_dist);
  FIELD_HANDLE (arrow_head, ANYCODE);
  FIELD_BD (arrow_head_size);
  FIELD_BS (style_content);
  FIELD_HANDLE (text_style, ANYCODE);
  FIELD_BS (text_left);
  FIELD_BS (text_rigth);
  FIELD_BS (text_angle);
  FIELD_BS (attach_type);
  FIELD_CMC (text_color);
  FIELD_B (text_frame);
  FIELD_HANDLE (block_style, ANYCODE);
  FIELD_CMC (block_color);
  FIELD_3BD (block_scale);
  FIELD_BD (block_rotation);
  FIELD_BS (style_attachment);
  FIELD_B (is_annotative);

  VERSIONS (R_2000, R_2007)
    {
      FIELD_BL (num_arrowheads);
      REPEAT(num_arrowheads, arrowheads, Leader_ArrowHead)
        {
          FIELD_BL(arrowheads->is_default);
          FIELD_HANDLE(arrowheads->arrowhead, ANYCODE);
        }
      FIELD_BL (num_blocklabels);
      REPEAT(num_blocklabels, blocklabels, Leader_BlockLabel)
        {
          FIELD_HANDLE (blocklabels->attdef, ANYCODE);
          FIELD_TV (blocklabels->label_text);
          FIELD_BS (blocklabels->ui_index);
          FIELD_BD (blocklabels->width);
        }
      FIELD_B (neg_textdir);
      FIELD_BS (ipe_align);
      FIELD_BS (justification);
      FIELD_BD (scale_factor);
    }

  SINCE (R_2010)
    {
      FIELD_BS (attach_dir);
      FIELD_BS (attach_top);
      FIELD_BS (attach_bottom);
    }
  SINCE (R_2013)
    {
      FIELD_B (text_extended);
    }

  //COMMON_ENTITY_HANDLE_DATA; //??

DWG_ENTITY_END

/* par 20.4.87 (varies) */
DWG_OBJECT(MLEADERSTYLE);

  SINCE (R_2010)
    {
      IF_ENCODE_FROM_EARLIER {
        FIELD_VALUE(version) = 2;
      }
      FIELD_BS (version);
    }

  FIELD_BS (content_type);
  FIELD_BS (mleader_order);
  FIELD_BS (leader_order);
  FIELD_BL (max_points);
  FIELD_BD (first_seg_angle);
  FIELD_BD (second_seg_angle);
  FIELD_BS (type);
  FIELD_CMC (line_color);
  FIELD_HANDLE (line_type, 4);
  FIELD_BL (line_weight);
  FIELD_B (landing);
  FIELD_BD (landing_gap);
  FIELD_B (dog_leg);
  FIELD_BD (landing_dist);
  FIELD_TV (description);
  FIELD_HANDLE (arrow_head, 4);
  FIELD_BD (arrow_head_size);
  FIELD_TV (text_default);
  FIELD_HANDLE (text_style, 4);
  FIELD_BS (attach_left);
  FIELD_BS (attach_right);
  FIELD_BS (text_angle_type);
  FIELD_BS (text_align_type);
  FIELD_CMC (text_color);
  FIELD_BD (text_heigth);
  FIELD_B (text_frame);
  FIELD_B (text_always_left);
  FIELD_BD (align_space);
  FIELD_HANDLE (block, 4);
  FIELD_CMC (block_color);
  FIELD_3BD (block_scale);
  FIELD_B (use_block_scale);
  FIELD_BD (block_rotation);
  FIELD_B (use_block_rotation);
  FIELD_BS (block_connection);
  FIELD_BD (scale);
  FIELD_B (changed);
  FIELD_B (is_annotative);
  FIELD_BD (break_size);

  SINCE (R_2010)
    {
      FIELD_BS (attach_dir);
      FIELD_BS (attach_top);
      FIELD_BS (attach_bottom);
    }

DWG_OBJECT_END

////////////////////
// These variable objects are not described in the spec:
//

DWG_OBJECT(WIPEOUTVARIABLE);

  FIELD_BS(display_frame);

  UNTIL(R_2007)
    {
      FIELD_HANDLE (parenthandle, 3);
    }
  REACTORS(4);
  XDICOBJHANDLE(3);

DWG_OBJECT_END

// R2000+ picture. undocumented (varies)
DWG_ENTITY(WIPEOUT);

  FIELD_BL (class_version);
  FIELD_3DPOINT(pt0);
  FIELD_3DPOINT(uvec);
  FIELD_3DPOINT(vvec);
  FIELD_RD (size.width);
  FIELD_RD (size.height);
  FIELD_BS (display_props);
  FIELD(clipping, B);
  FIELD_RC (brightness);
  FIELD_RC (contrast);
  FIELD_RC (fade);

  FIELD_BS (clip_boundary_type);
  if (FIELD_VALUE(clip_boundary_type) == 1)
    {
      FIELD_2RD(boundary_pt0);
      FIELD_2RD(boundary_pt1);
    }
  else
    {
      FIELD_BL (num_clip_verts);
      REPEAT(num_clip_verts, clip_verts, Dwg_Entity_IMAGE_clip_vert)
        {
          FIELD_2RD(clip_verts[rcount]);
        }
    }

  COMMON_ENTITY_HANDLE_DATA;

  FIELD_HANDLE(imagedef,5);
  FIELD_HANDLE(imagedefreactor,3);

DWG_ENTITY_END

/* (varies) container to hold a raw class entity (none observed in the wild) */
DWG_ENTITY(UNKNOWN_ENT);

  FIELD_VALUE(num_bytes) = obj->bitsize / 8;
  FIELD_VALUE(num_bits)  = obj->bitsize % 8;

  FIELD_VECTOR(bytes, RC, num_bytes);
  FIELD_VECTOR(bits, B, num_bits);
  //COMMON_ENTITY_HANDLE_DATA; // including this

DWG_ENTITY_END

/* container to hold a raw class object (varies) */
DWG_OBJECT(UNKNOWN_OBJ);

  FIELD_VALUE(num_bytes) = obj->bitsize / 8;
  FIELD_VALUE(num_bits)  = obj->bitsize % 8;

  FIELD_VECTOR(bytes, RC, num_bytes);
  FIELD_VECTOR(bits, B, num_bits);
DWG_OBJECT_END

DWG_ENTITY(DUMMY);

  FIELD_BS(flags);
  LOG_INFO("TODO DUMMY\n");
  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

DWG_ENTITY(LONG_TRANSACTION);

  FIELD_BS(flags);
  LOG_INFO("TODO LONG_TRANSACTION\n");
  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/*
// r2007+
DWG_OBJECT(VISUALSTYLE);

  // hard-pointer to H DICTIONARY_VISUALSTYLE
  FIELD_HANDLE(visualstyle, 5);

DWG_OBJECT_END

// r2000+
DWG_OBJECT(ARCALIGNEDTEXT);

  LOG_INFO("TODO ARCALIGNEDTEXT\n");

DWG_OBJECT_END

//(varies)
DWG_OBJECT(MATERIAL);
DWG_OBJECT_END

DWG_OBJECT(DIMASSOC);
DWG_OBJECT_END

DWG_OBJECT(DBCOLOR);
DWG_OBJECT_END

DWG_OBJECT(TABLESTYLE);
DWG_OBJECT_END

DWG_OBJECT(SECTIONVIEWSTYLE);
DWG_OBJECT_END

DWG_OBJECT(DETAILVIEWSTYLE);
DWG_OBJECT_END

DWG_OBJECT(ASSOC2DCONSTRAINTGROUP);
DWG_OBJECT_END

DWG_OBJECT(ASSOCGEOMDEPENDENCY);
DWG_OBJECT_END

DWG_OBJECT(LEADEROBJECTCONTEXTDATA);
DWG_OBJECT_END
*/

