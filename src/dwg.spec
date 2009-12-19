#ifdef IS_ENCODER
#define ENCODER if (1)
#define DECODER if (0)
#endif

#ifdef IS_DECODER
#define ENCODER if (0)
#define DECODER if (1)
#endif

#define TODO_ENCODER fprintf(stderr, "TODO: Encoder\n");
#define TODO_DECODER fprintf(stderr, "TODO: Decoder\n");

DWG_ENTITY(UNUSED);

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/*(1)*/
DWG_ENTITY (TEXT);

  VERSIONS(R_13,R_14)
    {
      FIELD(elevation, BD);
      FIELD_2RD(insertion_pt);
      FIELD_2RD(alignment_pt);
      FIELD_3BD(extrusion);
      FIELD(thickness, BD);
      FIELD(oblique_ang, BD);
      FIELD(rotation_ang, BD);
      FIELD(height, BD);
      FIELD(width_factor, BD);
      FIELD(text_value, TV);
      FIELD(generation, BS);
      FIELD(horiz_alignment, BS);
      FIELD(vert_alignment, BS);
    }

  SINCE(R_2000)
    {
      /* We assume that the user (the client application)
      is responsible for taking care of properly updating the dataflags field 
      which indicates which fields in the data structures are valid and which are
      undefined */

      FIELD(dataflags, RC);

      if (!(GET_FIELD(dataflags) & 0x01))
        {
          FIELD(elevation, RD);
        }

      FIELD_2RD(insertion_pt);

      if (!(GET_FIELD(dataflags) & 0x02))
        {
          FIELD_2DD(alignment_pt, 10, 20);
        }

      FIELD_BE(extrusion);
      FIELD(thickness, BT);

      if (!(GET_FIELD(dataflags) & 0x04))
        {
          FIELD(oblique_ang, RD);
        }

      if (!(GET_FIELD(dataflags) & 0x08))
        {
          FIELD(rotation_ang, RD);
        }

      FIELD(height, RD);

      if (!(GET_FIELD(dataflags) & 0x10))
        {
          FIELD(width_factor, RD);
        }

      FIELD(text_value, TV);

      if (!(GET_FIELD(dataflags) & 0x20))
        {
          FIELD(generation, BS);
        }

      if (!(GET_FIELD(dataflags) & 0x40)) 
        {
          FIELD(horiz_alignment, BS);
        }

      if (!(GET_FIELD(dataflags) & 0x80))
        {
          FIELD(vert_alignment, BS);
        }
    }

  COMMON_ENTITY_HANDLE_DATA;

  FIELD_HANDLE(style, 5);
DWG_ENTITY_END

/*(2)*/
DWG_ENTITY(ATTRIB);

  VERSIONS(R_13,R_14)
    {
      FIELD(elevation, BD);
      FIELD_2RD(insertion_pt);
      FIELD_2RD(alignment_pt);
      FIELD_3BD(extrusion);
      FIELD(thickness, BD);
      FIELD(oblique_ang, BD);
      FIELD(rotation_ang, BD);
      FIELD(height, BD);
      FIELD(width_factor, BD);
      FIELD(text_value, TV);
      FIELD(generation, BS);
      FIELD(horiz_alignment, BS);
      FIELD(vert_alignment, BS);
    }

  SINCE(R_2000)
    {
      /* We assume that the user (the client application)
      is responsible for taking care of properly updating the dataflags field 
      which indicates which fields in the data structures are valid and which are
      undefined */

      FIELD(dataflags, RC);
      if (!(GET_FIELD(dataflags) & 0x01))
        {
          FIELD(elevation, RD);
        }
      FIELD_2RD(insertion_pt);
      if (!(GET_FIELD(dataflags) & 0x02))
        {
          FIELD_2DD(alignment_pt, 10, 20);
        }
      FIELD_BE(extrusion);
      FIELD(thickness, BT);
      if (!(GET_FIELD(dataflags) & 0x04))
        {
          FIELD(oblique_ang, RD);
        }
      if (!(GET_FIELD(dataflags) & 0x08))
        {
          FIELD(rotation_ang, RD);
        }
      FIELD(height, RD);
      if (!(GET_FIELD(dataflags) & 0x10))
        {
          FIELD(width_factor, RD);
        }
      FIELD(text_value, TV);
      if (!(GET_FIELD(dataflags) & 0x20))
        {
          FIELD(generation, BS);
        }
      if (!(GET_FIELD(dataflags) & 0x40))
        {
          FIELD(horiz_alignment, BS);
        }
      if (!(GET_FIELD(dataflags) & 0x80))
        {
          FIELD(vert_alignment, BS);
        }
    }

  FIELD(tag, TV);
  FIELD(field_length, BS);
  FIELD(flags, RC);

  SINCE(R_2007)
    {
      FIELD(lock_position_flag, B);
    }

  COMMON_ENTITY_HANDLE_DATA;

  FIELD_HANDLE(style, 5);

DWG_ENTITY_END

/*(3)*/
DWG_ENTITY(ATTDEF);

  VERSIONS(R_13,R_14)
    {
      FIELD(elevation, BD);
      FIELD_2RD(insertion_pt);
      FIELD_2RD(alignment_pt);
      FIELD_3BD(extrusion);
      FIELD(thickness, BD);
      FIELD(oblique_ang, BD);
      FIELD(rotation_ang, BD);
      FIELD(height, BD);
      FIELD(width_factor, BD);
      FIELD(default_value, TV);
      FIELD(generation, BS);
      FIELD(horiz_alignment, BS);
      FIELD(vert_alignment, BS);
    }

  SINCE(R_2000)
    {
      /* We assume that the user (the client application)
      is responsible for taking care of properly updating the dataflags field 
      which indicates which fields in the data structures are valid and which are
      undefined */

      FIELD(dataflags, RC);
      if (!(GET_FIELD(dataflags) & 0x01))
        {
          FIELD(elevation, RD);
        }
      FIELD_2RD(insertion_pt);
      if (!(GET_FIELD(dataflags) & 0x02))
        {
          FIELD_2DD(alignment_pt, 10, 20);
        }
      FIELD_BE(extrusion);
      FIELD(thickness, BT);
      if (!(GET_FIELD(dataflags) & 0x04))
        {
          FIELD(oblique_ang, RD);
        }
      if (!(GET_FIELD(dataflags) & 0x08))
        {
          FIELD(rotation_ang, RD);
        }
      FIELD(height, RD);
      if (!(GET_FIELD(dataflags) & 0x10))
        {
          FIELD(width_factor, RD);
        }
      FIELD(default_value, TV);
      if (!(GET_FIELD(dataflags) & 0x20))
        {
          FIELD(generation, BS);
        }
      if (!(GET_FIELD(dataflags) & 0x40))
        {
          FIELD(horiz_alignment, BS);
        }
      if (!(GET_FIELD(dataflags) & 0x80))
        {
          FIELD(vert_alignment, BS);
        }
    }

  FIELD(tag, TV);
  FIELD(field_length, BS);
  FIELD(flags, RC);

  SINCE(R_2007)
    {
      FIELD(lock_position_flag, B);
    }

  FIELD(prompt, TV);

  COMMON_ENTITY_HANDLE_DATA;

  FIELD_HANDLE(style, 5);

DWG_ENTITY_END

/*(4)*/
DWG_ENTITY(BLOCK);

  FIELD(name, TV);

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
          FIELD(scale_flag, BB);
          if (GET_FIELD(scale_flag) == 3)
            {
              GET_FIELD(scale.x) = 1.0;
              GET_FIELD(scale.y) = 1.0;
              GET_FIELD(scale.z) = 1.0;
            }
          else if (GET_FIELD(scale_flag) == 1)
            {
              GET_FIELD(scale.x) = 1.0;
              FIELD_DD(scale.y, 1.0);
              FIELD_DD(scale.z, 1.0);
            }
          else if (GET_FIELD(scale_flag) == 2)
            {
              FIELD(scale.x, RD); 
              GET_FIELD(scale.y) = GET_FIELD(scale.x);
              GET_FIELD(scale.z) = GET_FIELD(scale.x);
            }
          else //if (GET_FIELD(scale_flag) == 0)
            {
              FIELD(scale.x, RD);
              FIELD_DD(scale.y, GET_FIELD(scale.x));
              FIELD_DD(scale.z, GET_FIELD(scale.x));
            }
        }

      ENCODER
        {
          if (GET_FIELD(scale.x)==1.0 && GET_FIELD(scale.y)==1.0 && GET_FIELD(scale.z)==1.0)
            {
              GET_FIELD(scale_flag) = 3;
              FIELD(scale_flag, BB);
            }
          else if (GET_FIELD(scale.x)==GET_FIELD(scale.y) && GET_FIELD(scale.x)==GET_FIELD(scale.z))
            {
              GET_FIELD(scale_flag) = 2;
              FIELD(scale_flag, BB);
              FIELD(scale.x, RD);
            }
          else if (GET_FIELD(scale.x)==1.0)
            {
              GET_FIELD(scale_flag) = 1;
              FIELD(scale_flag, BB);
              FIELD(scale.x, RD);
              FIELD_DD(scale.y, 1.0);
              FIELD_DD(scale.z, 1.0);
            }
          else
            {
              GET_FIELD(scale_flag) = 0;
              FIELD(scale_flag, BB);
              FIELD(scale.x, RD);
              FIELD_DD(scale.y, GET_FIELD(scale.x));
              FIELD_DD(scale.z, GET_FIELD(scale.x));
            }
        }
    }

  FIELD(rotation_ang, BD);
  FIELD_3DPOINT(extrusion);
  FIELD(has_attribs, B);

  SINCE(R_2004)
    {
      FIELD(owned_obj_count, BL);
    }

  COMMON_ENTITY_HANDLE_DATA;

  FIELD_HANDLE(block_header, 5);

  //There is a typo in the spec. it says "R13-R200:".
  //I guess it means "R13-R2000:" (just like in MINSERT)
  VERSIONS(R_13,R_2000)
    {
    if(GET_FIELD(has_attribs))
      {
        FIELD_HANDLE(first_attrib, 4);
        FIELD_HANDLE(last_attrib, 4);
      }
    }

  //Spec typo? Spec says "2004:" but I think it should be "2004+:"
  // just like field owned_obj_count (AND just like in MINSERT)
  SINCE(R_2004)
    {
      HANDLE_VECTOR(attrib_handles, owned_obj_count, 4);
    }

  if (GET_FIELD(has_attribs))
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
          FIELD(scale_flag, BB);
          if (GET_FIELD(scale_flag) == 3){
            GET_FIELD(scale.x) = GET_FIELD(scale.y) = GET_FIELD(scale.y) = 1.0;
          }
          else if (GET_FIELD(scale_flag) == 1)
            {
              GET_FIELD(scale.x) = 1.0;
              FIELD_DD(scale.y, 1.0);
              FIELD_DD(scale.z, 1.0);
            }
          else if (GET_FIELD(scale_flag) == 2)
            {
              FIELD(scale.x, RD);
              GET_FIELD(scale.y) = GET_FIELD(scale.x);
              GET_FIELD(scale.z) = GET_FIELD(scale.x);
            }
          else //if (GET_FIELD(scale_flag) == 0)
            {
              FIELD(scale.x, RD);
              FIELD_DD(scale.y, GET_FIELD(scale.x));
              FIELD_DD(scale.z, GET_FIELD(scale.x));
            }
        }

      ENCODER
        {
          if (GET_FIELD(scale.x) == GET_FIELD(scale.y) == GET_FIELD(scale.z) == 1.0)
            {
              GET_FIELD(scale_flag) = 3;
              FIELD_BB(scale_flag);
            }
          else if (GET_FIELD(scale.x) == 1.0)
             {
              GET_FIELD(scale_flag) = 1;
              FIELD_BB(scale_flag);
              FIELD_DD(scale.y, 1.0);
              FIELD_DD(scale.z, 1.0);
             }
           else if (GET_FIELD(scale.x) == GET_FIELD(scale.y) == GET_FIELD(scale.z))
             {
              GET_FIELD(scale_flag) = 2;
              FIELD_BB(scale_flag);
              FIELD_RD(scale.x);
             }
           else
             {
              GET_FIELD(scale_flag) = 0;
              FIELD_BB(scale_flag);
              FIELD_RD(scale.x);
              FIELD_DD(scale.y, GET_FIELD(scale.x));
              FIELD_DD(scale.z, GET_FIELD(scale.x));
             }
        }
    }

  FIELD(rotation_ang, BD);
  FIELD_3BD(extrusion);
  FIELD(has_attribs, B);

  SINCE(R_2004)
    {
      FIELD(owned_obj_count, BL);
    }

  FIELD(numcols, BS);
  FIELD(numrows, BS);
  FIELD(col_spacing, BD);
  FIELD(row_spacing, BD);

  COMMON_ENTITY_HANDLE_DATA;

  FIELD_HANDLE(block_header, 5);

  VERSIONS(R_13,R_2000)
    if (GET_FIELD(has_attribs))
      {
        FIELD_HANDLE(first_attrib, 4);
        FIELD_HANDLE(last_attrib, 4);
      }

  SINCE(R_2004)
    {
      HANDLE_VECTOR(attrib_handles, owned_obj_count, 4);
    }

  if (GET_FIELD(has_attribs))
    {
      FIELD_HANDLE(seqend, 3);
    }

DWG_ENTITY_END

//(9) Unknown

/*(10)*/
DWG_ENTITY(VERTEX_2D);

  FIELD(flags, RC);
  FIELD_3BD(point);
  FIELD(start_width, BD);

  DECODER
    {
      if (GET_FIELD(start_width) < 0)
        {
          GET_FIELD(start_width) = -GET_FIELD(start_width);
          GET_FIELD(end_width) = GET_FIELD(start_width);
        }
      else
        {
          FIELD(end_width, BD);
        }
    }

  ENCODER
    {
      TODO_ENCODER
    }

  FIELD(bulge, BD);
  FIELD(tangent_dir, BD);

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/*(11)*/
DWG_ENTITY(VERTEX_3D);

  FIELD(flags, RC);
  FIELD_3BD(point);

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/*(12)*/
DWG_ENTITY(VERTEX_MESH);

  FIELD(flags, RC);
  FIELD_3BD(point);

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/*(13)*/
DWG_ENTITY(VERTEX_PFACE);

  FIELD(flags, RC);
  FIELD_3BD(point);

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/*(14)*/
DWG_ENTITY(VERTEX_PFACE_FACE);

  FIELD(vertind[0], BS);
  FIELD(vertind[1], BS);
  FIELD(vertind[2], BS);
  FIELD(vertind[3], BS);

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/*(15)*/
DWG_ENTITY(POLYLINE_2D);

  FIELD(flags, BS);
  FIELD(curve_type, BS);
  FIELD(start_width, BD);
  FIELD(end_width, BD);
  FIELD(thickness, BT);
  FIELD(elevation, BD);
  FIELD_BE(extrusion);

  SINCE(R_2004)
    {
      FIELD(owned_obj_count, BL);
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

  FIELD(flags_1, RC);
  FIELD(flags_2, RC);

  SINCE(R_2004)
    {
      FIELD(owned_obj_count, BL);
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

/*(17)*/
DWG_ENTITY(ARC);

  FIELD_3BD(center);
  FIELD(radius, BD);
  FIELD(thickness, BT);
  FIELD_BE(extrusion);
  FIELD(start_angle, BD);
  FIELD(end_angle, BD);

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/*(18)*/
DWG_ENTITY(CIRCLE);

  FIELD_3BD(center);
  FIELD(radius, BD);
  FIELD(thickness, BT);
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
          FIELD(start.x, RD);
          FIELD_DD(end.x, GET_FIELD(start.x));
          FIELD(start.y, RD);
          FIELD_DD(end.y, GET_FIELD(start.y));

          if (GET_FIELD(Zs_are_zero))
            {
              GET_FIELD(start.z) = 0.0;
              GET_FIELD(end.z) = 0.0;
            }
          else
            {
              FIELD(start.z, RD);
              FIELD_DD(end.z, GET_FIELD(start.z));
            }
        }

      ENCODER
        {
          GET_FIELD(Zs_are_zero) = (GET_FIELD(start.z) == 0.0 && GET_FIELD(end.z) == 0.0);
          FIELD_B(Zs_are_zero);
          FIELD_RD(start.x);
          FIELD_DD(end.x, GET_FIELD(start.x));
          FIELD_RD(start.y);
          FIELD_DD(end.y, GET_FIELD(start.y));
          if (!GET_FIELD(Zs_are_zero))
            {
              FIELD_RD(start.z);
              FIELD_DD(end.z, GET_FIELD(start.z));
            }
        }
    }

  FIELD(thickness, BT);
  FIELD_BE(extrusion);

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/**
 * Macro for common DIMENSION declaration
 */
/*FIELD_BD(elevation.ecs_11); \ */
#define DIMENSION_COMMON_DECODE \
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
  FIELD_HANDLE(dimstyle, 5);
  FIELD_HANDLE(block, 5);

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
  FIELD_HANDLE(dimstyle, 5);
  FIELD_HANDLE(block, 5);

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
  FIELD_HANDLE(dimstyle, 5);
  FIELD_HANDLE(block, 5);

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
  FIELD_HANDLE(dimstyle, 5);
  FIELD_HANDLE(block, 5);

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
  FIELD_HANDLE(dimstyle, 5);
  FIELD_HANDLE(block, 5);

DWG_ENTITY_END

/*(25)*/
DWG_ENTITY(DIMENSION_RADIUS);

  DIMENSION_COMMON_DECODE;
  FIELD_2RD(_12_pt);
  FIELD_3BD(_10_pt);
  FIELD_3BD(_15_pt);
  FIELD_BD(leader_len);

  COMMON_ENTITY_HANDLE_DATA;
  FIELD_HANDLE(dimstyle, 5);
  FIELD_HANDLE(block, 5);

DWG_ENTITY_END

/*(26)*/
DWG_ENTITY(DIMENSION_DIAMETER);

  DIMENSION_COMMON_DECODE;
  FIELD_2RD(_12_pt);
  FIELD_3BD(_15_pt);
  FIELD_3BD(_10_pt);
  FIELD_BD(leader_len);

  COMMON_ENTITY_HANDLE_DATA;
  FIELD_HANDLE(dimstyle, 5);
  FIELD_HANDLE(block, 5);

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
          if(GET_FIELD(z_is_zero))
            {
              GET_FIELD(corner1.z) = 0;
            }
          else
            {
              FIELD_RD(corner1.z);
            }
        }

      ENCODER
        {
          GET_FIELD(z_is_zero) = (GET_FIELD(corner1.z) == 0);
          FIELD_B(z_is_zero);
          FIELD_RD(corner1.x);
          FIELD_RD(corner1.y);
          if(!GET_FIELD(z_is_zero))
            {
              FIELD_RD(corner1.z);
            }
        }

      FIELD_DD(corner2.x, GET_FIELD(corner1.x));
      FIELD_DD(corner2.y, GET_FIELD(corner1.y));
      FIELD_DD(corner2.z, GET_FIELD(corner1.z));
      FIELD_DD(corner3.x, GET_FIELD(corner2.x));
      FIELD_DD(corner3.y, GET_FIELD(corner2.y));
      FIELD_DD(corner3.z, GET_FIELD(corner2.z));
      FIELD_DD(corner4.x, GET_FIELD(corner3.x));
      FIELD_DD(corner4.y, GET_FIELD(corner3.y));
      FIELD_DD(corner4.z, GET_FIELD(corner3.z));
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
  int i;

  FIELD_BS(scenario);
  if (GET_FIELD(scenario) != 1 && GET_FIELD(scenario) != 2)
    {
      fprintf(stderr, "Error: unknown scenario %d", GET_FIELD(scenario));
    }

  FIELD_BS(degree);
  if (GET_FIELD(scenario) == 2)
    {
      FIELD_BD(fit_tol);
      FIELD_3BD(beg_tan_vec);
      FIELD_3BD(end_tan_vec);
      FIELD_BS(num_fit_pts);
      REPEAT(num_fit_pts, fit_pts, Dwg_Entity_SPLINE_point)
        {
          FIELD_3BD(fit_pts[i]);
        }
    }
  if (GET_FIELD(scenario) == 1)
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
  REPEAT(num_knots, knots, BITCODE_BD)
    {
      FIELD_BD(knots[i]);
    }

  REPEAT(num_ctrl_pts, ctrl_pts, Dwg_Entity_SPLINE_control_point)
    {
//TODO: does it work both for encoder and decoder routines?
      FIELD_3BD(ctrl_pts[i]);
      if (!GET_FIELD(weighted))
        {
          //TODO check what "D" means on spec.
          //assuming typo - should be BD
          //assuming w=0 when not present.
          GET_FIELD(ctrl_pts[i].w) = 0;
        }
      else
        {
          FIELD_BD(ctrl_pts[i].w);
        }
    }

//TODO: spec talks about "Repeat numfitpts times { Fit pt 3BD }" here

  COMMON_ENTITY_HANDLE_DATA;
DWG_ENTITY_END

//TODO: 37, 38 and 39 are ACIS entities

#define PARSE_WIRE_STRUCT(name)                       \
  FIELD_RC(name.type);                                \
  FIELD_BL(name.selection_marker);                    \
  FIELD_BS(name.color);                               \
  FIELD_BL(name.acis_index);                          \
  FIELD_BL(name.num_points);                             \
  FIELD_3DPOINT_VECTOR(name.points, name.num_points); \
  FIELD_B(name.transform_present);                      \
  if (GET_FIELD(name.transform_present))              \
    {                                                 \
      FIELD_3BD(name.axis_x);                         \
      FIELD_3BD(name.axis_y);                         \
      FIELD_3BD(name.axis_z);                         \
      FIELD_3BD(name.translation);                    \
      FIELD_BD(name.scale);                           \
      FIELD_B(name.has_rotation);                     \
      FIELD_B(name.has_reflection);                    \
      FIELD_B(name.has_shear);                        \
    }

#ifdef IS_DECODER

#define DECODE_3DSOLID decode_3dsolid(dat, obj, _obj);

inline void decode_3dsolid(Bit_Chain* dat, Dwg_Object* obj, Dwg_Entity_3DSOLID* _obj){
	Dwg_Data* dwg = obj->parent;
  int vcount, rcount, rcount2;
  FIELD_B(acis_empty);
  int i=0;
  if (!GET_FIELD(acis_empty))
    {
      FIELD_B (unknown);
      FIELD_BS (version);
      if (GET_FIELD(version)==1)
        {
          do
            {
              GET_FIELD(sat_data) = (BITCODE_RC**) realloc(GET_FIELD(sat_data), i * sizeof(BITCODE_RC*));
              FIELD_BL (block_size);
              FIELD_VECTOR (sat_data[i++], RC, block_size);
            } while(GET_FIELD(block_size));
        }
      else
        {
          //TODO
          fprintf(stderr, "TODO: Implement parsing of ACIS file in entities 37,38 and 39.\n");
        }

      FIELD_B (wireframe_data_present);
      if (GET_FIELD(wireframe_data_present))
        {
          FIELD_B (point_present);
          if (GET_FIELD(point_present))
            {
              FIELD_3BD (point);
            }
          else
            {
              GET_FIELD(point.x) = 0;
              GET_FIELD(point.y) = 0;
              GET_FIELD(point.z) = 0;
            }
          FIELD_BL (num_isolines);
          FIELD_B (isoline_present);
          if (GET_FIELD(isoline_present))
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
    }
}
#else
#define DECODE_3DSOLID {}
#endif //#if IS_DECODER

#ifdef IS_ENCODER

#define ENCODE_3DSOLID encode_3dsolid(dat, obj, _obj);

inline void encode_3dsolid(Bit_Chain* dat, Dwg_Object* obj, Dwg_Entity_3DSOLID* _obj){
  //TODO Implement-me
}
#else
#define ENCODE_3DSOLID {}
#endif //#if IS_ENCODER


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

  FIELD(numitems, BL);

  VERSION(R_14)
    {
      FIELD(unknown_r14, RC);
    }

  SINCE(R_2000)
    {
      FIELD(cloning, BS);
      FIELD(hard_owner, RC);
    }

  if (GET_FIELD(numitems) > 10000)
    {
      fprintf(
          stderr,
          "Strange: dictionary with more than 10 thousand entries! Handle: %lu\n",
          obj->handle.value);
      return;
    }

  FIELD_VECTOR(text, TV, numitems);
  FIELD_HANDLE(parenthandle, 4);
  REACTORS(4);
  XDICOBJHANDLE(3);
  HANDLE_VECTOR(itemhandles, numitems, 2);

DWG_ENTITY_END

//(43): Unknown

/*(44)*/
DWG_ENTITY(MTEXT);

  //spec-typo ? Spec says BD but we think it might be 3BD:
  FIELD_3BD(insertion_pt);
  FIELD_3BD(extrusion);
  FIELD_3BD(x_axis_dir);

  SINCE(R_2007)
    {
      FIELD(rect_height, BD);
    }

  FIELD(rect_width, BD);
  FIELD(text_height, BD);
  FIELD(attachment, BS);
  FIELD(drawing_dir, BS);
  FIELD(extends_ht, BD); //not documented
  FIELD(extends_wid, BD);
  FIELD(text, TV);

  SINCE(R_2000)
    {
      FIELD(linespace_style, BS);
      FIELD(linespace_factor, BD);
      FIELD(unknown_bit, B);
    }

  SINCE(R_2004)
    {
      FIELD(unknown_long, BL);
    }

  COMMON_ENTITY_HANDLE_DATA;

  FIELD_HANDLE(style, 5);

DWG_ENTITY_END

/*(45)*/
DWG_ENTITY(LEADER);

  FIELD(unknown_bit_1, B);
  FIELD(annot_type, BS);
  FIELD(path_type, BS);
  FIELD(numpts, BL);
  FIELD_3DPOINT_VECTOR(points, numpts);
  FIELD_3DPOINT(end_pt_proj);
  FIELD_3DPOINT(extrusion);
  FIELD_3DPOINT(x_direction);

  SINCE(R_14)
    {
      FIELD_3DPOINT(unknown_pt);
    }

  VERSIONS(R_13,R_14)
    {
      FIELD(dimgap, BD);
    }

  FIELD(box_height, BD);
  FIELD(box_width, BD);
  FIELD(hooklineonxdir, B);
  FIELD(arrowhead_on, B);

  VERSIONS(R_13,R_14)
    {
      FIELD(arrowhead_type, BS);
      FIELD(dimasz, BD);
      FIELD(unknown_bit_2, B);
      FIELD(unknown_bit_3, B);
      FIELD(unknown_short_1, BS);
      FIELD(byblock_color, BS);
      FIELD(unknown_bit_4, B);
      FIELD(unknown_bit_5, B);
    }

  SINCE(R_2000)
    {
      FIELD(unknown_short_1, BS);
      FIELD(unknown_bit_4, B);
      FIELD(unknown_bit_5, B);
    }

  COMMON_ENTITY_HANDLE_DATA;

  //FIXME reading these handles leaded to a segfault
  //SINCE(R_14)
  //{
  //TODO check if field is present in R_13.
  //Juca thinks it is present but inactive/not used.
  //FIELD_HANDLE(associated_annotation, 5);
  //}
  //FIELD_HANDLE(dimstyle, 5);

DWG_ENTITY_END

/*(46)*/
DWG_ENTITY(TOLERANCE);

  VERSIONS(R_13, R_14)
    {
      FIELD(unknown_short, BS); //spec-typo? Spec says S instead of BS.
      FIELD(height, BD);
      FIELD(dimgap, BD);
    }

  FIELD_3DPOINT(ins_pt);
  FIELD_3DPOINT(x_direction);
  FIELD_3DPOINT(extrusion);
  FIELD(text_string, BS);

  COMMON_ENTITY_HANDLE_DATA;
  FIELD_HANDLE(dimstyle, 5);

DWG_ENTITY_END

/*(47)*/
DWG_ENTITY(MLINE);

  FIELD(scale, BD);
  FIELD(just, RC); //spec-typo? Spec says EC instead of RC...
  FIELD_3DPOINT(base_point);
  FIELD_3DPOINT(extrusion);
  FIELD(open_closed, BS);
  FIELD(num_lines, RC);
  FIELD(num_verts, BS);

  REPEAT(num_verts, verts, Dwg_Entity_MLINE_vert)
    {
      FIELD_3DPOINT(verts[rcount].vertex);
      FIELD_3DPOINT(verts[rcount].vertex_direction);
      FIELD_3DPOINT(verts[rcount].miter_direction);

      REPEAT2(num_lines, verts[rcount].lines, Dwg_Entity_MLINE_line)
        {
          FIELD(verts[rcount].lines[rcount2].num_segparms, BS);
          REPEAT3(verts[rcount].lines[rcount2].num_segparms, verts[rcount].lines[rcount2].segparms, BITCODE_BD)
            {
              FIELD(verts[rcount].lines[rcount2].segparms[rcount3], BD);
            }

          FIELD(verts[rcount].lines[rcount2].num_areafillparms, BS);
          REPEAT3(verts[rcount].lines[rcount2].num_areafillparms, verts[rcount].lines[rcount2].areafillparms, BITCODE_BD)
            {
              FIELD(verts[rcount].lines[rcount2].areafillparms[rcount3], BD);
            }
        }
    }

  COMMON_ENTITY_HANDLE_DATA;
  FIELD_HANDLE(mline_style, 5);

DWG_ENTITY_END

/*(48)*/
DWG_OBJECT(BLOCK_CONTROL);

  FIELD(num_entries, BS);
  FIELD_HANDLE(null_handle, 4);
  XDICOBJHANDLE(3);
  HANDLE_VECTOR(block_headers, num_entries, 2);
  FIELD_HANDLE(model_space,3);
  FIELD_HANDLE(paper_space,3);

DWG_OBJECT_END

/*(49)*/
DWG_OBJECT(BLOCK_HEADER);

  FIELD(entry_name, TV);
  FIELD(_64_flag, B);
  FIELD(xrefindex_plus1, BS);
  FIELD(xdep, B);
  FIELD(anonymous, B);
  FIELD(hasattrs, B);
  FIELD(blkisxref, B);
  FIELD(xrefoverlaid, B);

  SINCE(R_2000)
    {
      FIELD(loaded_bit, B);
    }

  SINCE(R_2004)
    {
      FIELD(owned_object_count, BL);
    }

  FIELD_3DPOINT(base_pt);
  FIELD(xref_pname, TV);

  SINCE(R_2000)
    {

      //skip non-zero bytes and a terminating zero:
      GET_FIELD(insert_count)=0;
      while (bit_read_RC(dat))
        {
          GET_FIELD(insert_count)++;
        };

      FIELD(block_description, TV);

      FIELD(size_of_preview_data, BL);
      FIELD_VECTOR(binary_preview_data, RC, size_of_preview_data);
    }

  SINCE(R_2007)
    {
      FIELD(insert_units, BS);
      FIELD(explodable, B);
      FIELD(block_scaling, RC);
    }

  FIELD_HANDLE(block_control_handle, 4);
  REACTORS(4);
  XDICOBJHANDLE(3);
  FIELD_HANDLE(NULL_handle, 5);
  FIELD_HANDLE(block_entity, 3);

  VERSIONS(R_13,R_2000)
    {
      if (!GET_FIELD(blkisxref) && !GET_FIELD(xrefoverlaid))
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
      HANDLE_VECTOR(insert_handles, insert_count, ANYCODE)
      FIELD_HANDLE(layout_handle, ANYCODE);
    }

DWG_OBJECT_END

/*(50)*/
DWG_OBJECT(LAYER_CONTROL);

  FIELD(num_entries, BS);
  FIELD_HANDLE(null_handle, 4);
  XDICOBJHANDLE(3);
  HANDLE_VECTOR(layers, num_entries, 2);

DWG_OBJECT_END

/*(51)*/
DWG_OBJECT(LAYER);

  FIELD(entry_name, TV);
  FIELD(_64_flag, B);
  FIELD(xrefindex_plus1, BS);
  FIELD(xrefdep, B);
  VERSIONS(R_13, R_14)
    {
      FIELD(frozen, B);
      FIELD(on, B);
      FIELD(frozen_in_new, B);
      FIELD(locked, B);
    }

  SINCE(R_2000)
    {
      FIELD(values, BS);      
    }

  FIELD_CMC(color);

  FIELD_HANDLE(layer_control, 4);
  REACTORS(4);
  XDICOBJHANDLE(3);
  FIELD_HANDLE(null_handle, 5);

  SINCE(R_2000)
    {
      FIELD_HANDLE(plotstyle, 5);
    }

  SINCE(R_2007)
    {
      FIELD_HANDLE(material, ANYCODE);
    }

  FIELD_HANDLE(linetype, 5);

DWG_OBJECT_END

/*(52)*/
DWG_OBJECT(SHAPEFILE_CONTROL);

  FIELD(num_entries, BS);
  FIELD_HANDLE(null_handle, 4);
  XDICOBJHANDLE(3);
  HANDLE_VECTOR(shapefiles, num_entries, 2);

DWG_OBJECT_END

/*(53)*/
DWG_OBJECT(SHAPEFILE);

  FIELD(entry_name, TV);
  FIELD(_64_flag, B);
  FIELD(xrefindex_plus1, BS);
  FIELD(xrefdep, B);
  FIELD(vertical, B);
  FIELD(shape_file, B);
  FIELD(fixed_height, BD);
  FIELD(width_factor, BD);
  FIELD(oblique_ang, BD);
  FIELD(generation, RC);
  FIELD(last_height, BD);
  FIELD(font_name, TV);
  FIELD(bigfont_name, TV);
  FIELD_HANDLE(shapefile_control, 4);
  REACTORS(4);
  XDICOBJHANDLE(3);
  FIELD_HANDLE(null_handle, 5);

DWG_OBJECT_END

//(54): Unknown
//(55): Unknown

/*(56)*/
DWG_OBJECT(LTYPE_CONTROL);

  FIELD(num_entries, BS);
  FIELD_HANDLE(null_handle, 4);
  XDICOBJHANDLE(3);
  HANDLE_VECTOR(linetypes, num_entries, 2);
  FIELD_HANDLE(bylayer, 3);
  FIELD_HANDLE(byblock, 3);

DWG_OBJECT_END

/*(57)*/
DWG_OBJECT(LTYPE);
  char R2007plus_text_area_is_present = 0;

  FIELD(entry_name, TV);
  FIELD(_64_flag, B);
  FIELD(xrefindex_plus1, BS);
  FIELD(xrefdep, B);
  FIELD(description, TV);
  FIELD(pattern_len, BD);
  FIELD(alignment, RC);
  FIELD(num_dashes, RC);

  REPEAT(num_dashes, dash, LTYPE_dash)
    {
      FIELD(dash[rcount].length, BD);
      FIELD(dash[rcount].complex_shapecode, BS);
      FIELD(dash[rcount].x_offset, RD);
      FIELD(dash[rcount].y_offset, RD);
      FIELD(dash[rcount].scale, BD);
      FIELD(dash[rcount].rotation, BD);
      FIELD(dash[rcount].shape_flag, BS);
      if (GET_FIELD(dash[rcount].shape_flag) & 0x02)
        R2007plus_text_area_is_present = 1;
    }

  UNTIL(R_2004)
    {
      FIELD_VECTOR_N(strings_area, RC, 256);
    }
  LATER_VERSIONS
    {
      if (R2007plus_text_area_is_present)
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

  FIELD(num_entries, BS);
  FIELD_HANDLE(null_handle, 4);
  XDICOBJHANDLE(3);
  HANDLE_VECTOR(views, num_entries, 2);

DWG_OBJECT_END

/*(61)*/
DWG_OBJECT(VIEW);

  FIELD(entry_name, TV);
  FIELD(_64_flag, B);
  FIELD(xrefindex_plus1, BS);
  FIELD(xrefdep, B);
  FIELD(height, BD);
  FIELD(width, BD);
  FIELD_2RD(center);
  FIELD_3BD(target);
  FIELD_3BD(direction);
  FIELD(twist_angle, BD);
  FIELD(lens_legth, BD);
  FIELD(front_clip, BD);
  FIELD(back_clip, BD);
  FIELD(view_mode, RC); //??? 4bits

  SINCE(R_2000)
    {
      FIELD(render_mode, RC);
    }

  FIELD(pspace_flag, B);

  SINCE(R_2000)
    {
      FIELD(associated_ucs, B);
      FIELD_3BD(origin);
      FIELD_3BD(x_direction);
      FIELD_3BD(y_direction);
      FIELD(elevation, BD);
      FIELD(orthographic_view_type, BS);
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
      FIELD_HANDLE(base_ucs_handle, ANYCODE);
      FIELD_HANDLE(named_ucs_handle, ANYCODE); 
    }

  SINCE(R_2007)
    {
      FIELD_HANDLE(live_section, ANYCODE);
    }

DWG_OBJECT_END

/*(62)*/
DWG_OBJECT(UCS_CONTROL);

  FIELD(num_entries, BS);
  FIELD_HANDLE(null_handle, 4);
  XDICOBJHANDLE(3);
  HANDLE_VECTOR(ucs, num_entries, 2);

DWG_OBJECT_END

/*(63)*/
DWG_OBJECT(UCS);

  FIELD(entry_name, TV);
  FIELD(_64_flag, B);
  FIELD(xrefindex_plus1, BS);
  FIELD(xrefdep, B);
  FIELD_3BD(x_direction);
  FIELD_3BD(y_direction);

  SINCE(R_2000)
    {
      FIELD(elevation, BD);
      FIELD(orthographic_view_type, BS);
      FIELD(orthographic_type, BS);
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
  FIELD_HANDLE (null_handle, 4);
  XDICOBJHANDLE(3);
  HANDLE_VECTOR(vports, num_entries, 2); 

DWG_OBJECT_END

/*(65)*/
DWG_OBJECT(VPORT);

  FIELD_TV (entry_name);
  FIELD_B (_64_flag);
  FIELD_BS (xrefindex_plus1);
  FIELD_B (xrefdep);
  FIELD_BD (view_height);
  FIELD_BD (aspect_ratio);
  FIELD_2RD (view_center);
  FIELD_3BD (view_target);
  FIELD_3BD (view_dir);
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
  FIELD_B (fast_zoom);
  FIELD_B (UCSICON_0);
  FIELD_B (UCSICON_1);
  FIELD_B (grid_on_off);
  FIELD_2RD (grid_spacing);
  FIELD_B (snap_on_off);
  FIELD_B (snap_style);
  FIELD_BS (snap_isopair);
  FIELD_BD (snap_rot);
  FIELD_2RD (snap_base);
  FIELD_2RD (snap_spacing);

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
  FIELD_HANDLE (null_handle, 4);
  XDICOBJHANDLE(3);
  HANDLE_VECTOR(apps, num_entries, 2);

DWG_OBJECT_END

/*(67)*/
DWG_OBJECT(APPID);

  FIELD_TV (entry_name);
  FIELD_B (_64_flag);
  FIELD_BS (xrefindex_plus1);
  FIELD_B (xrefdep);
  FIELD_RC (unknown);

  FIELD_HANDLE (app_control, 4);
  REACTORS(4);
  XDICOBJHANDLE(3);
  FIELD_HANDLE (null_handle, 5);

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
      FIELD_HANDLE (unknown_handle, ANYCODE);
    }

  FIELD_HANDLE (null_handle, 4);
  XDICOBJHANDLE(3);
  HANDLE_VECTOR (dimstyles, num_entries, 2);

DWG_OBJECT_END

/*(69)*/
DWG_OBJECT(DIMSTYLE);

  FIELD_TV (entry_name);
  FIELD_B (_64_flag);
  FIELD_BS (xrefindex_plus1);
  FIELD_B (xrefdep);

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
      FIELD_RC (DIMALTD);
      FIELD_RC (DIMZIN);
      FIELD_B (DIMSD1);
      FIELD_B (DIMSD2);
      FIELD_RC (DIMTOLJ);
      FIELD_RC (DIMJUST);
      FIELD_RC (DIMFIT);
      FIELD_B (DIMUPT);
      FIELD_RC (DIMTZIN);
      FIELD_RC (DIMMALTZ);
      FIELD_RC (DIMMALTTZ);
      FIELD_RC (DIMTAD);
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
      FIELD_BS (DIMCLRD);
      FIELD_BS (DIMCLRE);
      FIELD_BS (DIMCLRT);
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
      FIELD_BS (DIMCLRD);
      FIELD_BS (DIMCLRE);
      FIELD_BS (DIMCLRT);
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
  FIELD_HANDLE(null_handle, 5);
  FIELD_HANDLE(shapefile, 5); /*(DIMTXSTY)*/

  SINCE(R_2000)
    {
      FIELD_HANDLE(leader_block,5); /*(DIMLDRBLK)*/
      FIELD_HANDLE(dimblk,5); /*(DIMBLK)*/
      FIELD_HANDLE(dimblk1,5); /*(DIMBLK1)*/
      FIELD_HANDLE(dimblk2,5); /*(DIMBLK2)*/
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
  FIELD_HANDLE (null_handle, 4);
  XDICOBJHANDLE(3);
  HANDLE_VECTOR(viewport_entity_headers, num_entries, 2);

DWG_OBJECT_END

/*(71)*/
DWG_OBJECT(VP_ENT_HDR);

  FIELD(entry_name, TV);
  FIELD(_64_flag, B);
  FIELD(xrefindex_plus1, BS);
  FIELD(xrefdep, B);
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
  FIELD_HANDLE (parenthandle, 4);
  REACTORS(4);
  XDICOBJHANDLE(3);
  HANDLE_VECTOR(group_entries, num_handles, 5);

DWG_OBJECT_END;

/*(73)*/
DWG_OBJECT(MLINESTYLE);

  FIELD(name, TV);
  FIELD(desc, TV);
  FIELD(flags, BS);
  FIELD_CMC(fillcolor);
  FIELD(startang, BD);
  FIELD(endang, BD);
  FIELD(linesinstyle, RC);

  //XXX Ugly! We must find a better way to handle arbitrary vectors
  REPEAT(linesinstyle, lines, Dwg_Object_MLINESTYLE_line)
  {
    FIELD(lines[rcount].offset, BD);
    FIELD_CMC(lines[rcount].color);
    FIELD(lines[rcount].ltindex, BS);
  }
  FIELD_HANDLE(parenthandle, 4);
  REACTORS(4);
  XDICOBJHANDLE(3);

DWG_OBJECT_END


//pg.135
DWG_OBJECT(DICTIONARYVAR);

  FIELD_RC(intval);
  FIELD_TV(str);

  FIELD_HANDLE(parenthandle, 4);
  REACTORS(4);
  XDICOBJHANDLE(3);

DWG_OBJECT_END

//pg.136
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

  FIELD_BD (z_coord); /* x,y always 0.0 */
  FIELD_3BD (extrusion);
  FIELD_TV (name);
  FIELD_B (solid_fill);
  FIELD_B (associative);
  FIELD_BL (num_paths);
  REPEAT(num_paths, paths, Dwg_Entity_HATCH_Path)
    {
      FIELD_BL(paths[rcount].flag);
      if (!(GET_FIELD(paths[rcount].flag) & 2))
        {
          FIELD_BL(paths[rcount].num_path_segs);
          REPEAT2(paths[rcount].num_path_segs, paths[rcount].segs, Dwg_Entity_HATCH_PathSeg)
            {
              FIELD_RC(paths[rcount].segs[rcount2].type_status);
              switch (GET_FIELD(paths[rcount].segs[rcount2].type_status))
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
                      FIELD_VECTOR(paths[rcount].segs[rcount2].knots, BD, paths[rcount].segs[rcount2].num_knots)
                      REPEAT3(paths[rcount].segs[rcount2].num_control_points, paths[rcount].segs[rcount2].control_points, Dwg_Entity_HATCH_ControlPoint)
                        {
                          FIELD_2RD(paths[rcount].segs[rcount2].control_points[rcount3].point);
                          if (GET_FIELD(paths[rcount].segs[rcount2].is_rational))
                            {
                              FIELD_BD(paths[rcount].segs[rcount2].control_points[rcount3].weigth);
                            }
                        }                      
                      break;
                }
            }
        }
      else
        { /* POLYLINE PATH */
          FIELD_B(paths[rcount].bulges_present);
          FIELD_B(paths[rcount].closed);
          FIELD_BL(paths[rcount].num_path_segs);
          REPEAT2(paths[rcount].num_path_segs, paths[rcount].polyline_paths, Dwg_Entity_HATCH_PolylinePath)
            {
              FIELD_2RD (paths[rcount].polyline_paths[rcount2].point);
              if (GET_FIELD(paths[rcount].bulges_present))
                {
                  FIELD_BD (paths[rcount].polyline_paths[rcount2].bulge);
                }
            }
        }
      FIELD_BL(paths[rcount].num_boundary_obj_handles);
    }

// TODO: incomplete parsing. check spec. 

DWG_OBJECT_END

//pg.139
DWG_OBJECT(IDBUFFER);

  FIELD_RC (unknown);
  FIELD_BL (num_ids);
  FIELD_HANDLE (parenthandle, 4);
  REACTORS(4);
  XDICOBJHANDLE(3);
  HANDLE_VECTOR(obj_ids, num_ids, 4);

DWG_OBJECT_END

//pg.140
DWG_ENTITY(IMAGE);

  FIELD(class_version, BL);
  FIELD_3DPOINT(pt0);
  FIELD_3DPOINT(uvec);
  FIELD_3DPOINT(vvec);
  FIELD(size.width, RD);
  FIELD(size.height, RD);
  FIELD(display_props, BS);
  FIELD(clipping, B);
  FIELD(brightness, RC);
  FIELD(contrast, RC);
  FIELD(fade, RC);

  FIELD(clip_boundary_type, BS);
  if (GET_FIELD(clip_boundary_type) == 1)
    {
      FIELD_2RD(boundary_pt0);
      FIELD_2RD(boundary_pt1);
    }
  else
    {
      FIELD(num_clip_verts, BL);
      REPEAT(num_clip_verts, clip_verts, Dwg_Entity_IMAGE_clip_vert) 
        {
          FIELD_2RD(clip_verts[rcount]);
        }
    }

  COMMON_ENTITY_HANDLE_DATA;
  FIELD_HANDLE(imagedef,5);
  FIELD_HANDLE(imagedefreactor,3);

DWG_ENTITY_END

//pg.142
DWG_OBJECT(IMAGEDEF);

  FIELD_BL (class_version);
  FIELD_2RD (image_size);
  FIELD_TV (file_path);
  FIELD_B (is_loaded);
  FIELD_RC (resunits);
  FIELD_2RD (pixel_size);
  FIELD_HANDLE (parent_handle, 3);
  REACTORS(4);
  XDICOBJHANDLE(3);

DWG_OBJECT_END

//PG.143
DWG_OBJECT(IMAGEDEFREACTOR);

  FIELD_BL (class_version);
  FIELD_HANDLE (parent_handle, 4);
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
  FIELD_HANDLE (parent_handle, 4);
  REACTORS(4);
  XDICOBJHANDLE(3);
  HANDLE_VECTOR(entry_handles, num_entries, ANYCODE);

DWG_OBJECT_END

//pg.145
DWG_OBJECT(LAYOUT);

  FIELD(page_setup_name, TV);
  FIELD(printer_or_config, TV);
  FIELD(plot_layout_flags, BS);
  FIELD(left_margin, BD);
  FIELD(bottom_margin, BD);
  FIELD(right_margin, BD);
  FIELD(top_margin, BD);
  FIELD(paper_width, BD);
  FIELD(paper_height, BD);
  FIELD(paper_size, TV);
  FIELD_2BD(plot_origin);
  FIELD(paper_units, BS);
  FIELD(plot_rotation, BS);
  FIELD(plot_type, BS);
  FIELD_2BD(window_min);
  FIELD_2BD(window_max);

  VERSIONS(R_13,R_2000)
    {
      FIELD(plot_view_name, TV);
    }

  FIELD(real_world_units, BD);
  FIELD(drawing_units, BD);
  FIELD(current_style_sheet, TV);
  FIELD(scale_type, BS);
  FIELD(scale_factor, BD);
  FIELD_2BD(paper_image_origin);

  SINCE(R_2004)
    {
      FIELD(shade_plot_mode, BS);
      FIELD(shade_plot_res_level, BS);
      FIELD(shade_plot_custom_dpi, BS);
    }

  FIELD(layout_name, TV);
  FIELD(tab_order, BS);
  FIELD(flags, BS);
  FIELD_3DPOINT(ucs_origin);
  FIELD_2RD(minimum_limits);
  FIELD_2RD(maximum_limits);
  FIELD_3DPOINT(ins_point);
  FIELD_3DPOINT(ucs_x_axis);
  FIELD_3DPOINT(ucs_y_axis);
  FIELD(elevation, BD);
  FIELD(orthoview_type, BS);
  FIELD_3DPOINT(extent_min);
  FIELD_3DPOINT(extent_max);

  SINCE(R_2004)
    {
      FIELD(viewport_count, RL);
    }

  FIELD_HANDLE(parenthandle, 4);
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
  int i;

  FIELD_BS(flags);

  if (GET_FIELD(flags) & 4)
    FIELD_BD(const_width);
  if (GET_FIELD(flags) & 8)
    FIELD_BD(elevation);
  if (GET_FIELD(flags) & 2)
    FIELD_BD(thickness);
  if (GET_FIELD(flags) & 1)
    FIELD_3BD(normal);
  FIELD_BL(num_points);

  if (GET_FIELD(flags) & 16)
    FIELD_BL(num_bulges);
  if (GET_FIELD(flags) & 32)
    FIELD_BL(num_widths);

  VERSIONS(R_13,R_14)
    {
      FIELD_2RD_VECTOR(points, num_points);
    }

  SINCE(R_2000)
    {
      FIELD_2RD(points[0]);
      for (i = 1; i < GET_FIELD(num_points); i++)
        {
          FIELD_2DD(points[i], GET_FIELD(points[i - 1].x), GET_FIELD(points[i - 1].y));
        }
    }

  FIELD_VECTOR(bulges, BD, num_bulges);
  REPEAT(num_widths, widths, Dwg_Entity_LWPLINE_width)
    {
      FIELD_BD(widths[rcount].start);
      FIELD_BD(widths[rcount].end);
    }

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

//pg.149
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


//pg.149
DWG_OBJECT(PROXY);

  SINCE(R_2000)
    {
      FIELD_BL (class_id);
      FIELD_BL (object_drawing_format);
      FIELD_B (original_data_format);
    }

  //TODO: figure out how to deal with the arbitrary size vector databits described on the spec

DWG_OBJECT_END


//pg.150
DWG_OBJECT(RASTERVARIABLES);

  BITCODE_BL (class_version);
  BITCODE_BS (display_frame);
  BITCODE_BS (display_quality);
  BITCODE_BS (units);
  FIELD_HANDLE (parenthandle, 4);
  REACTORS(4);
  XDICOBJHANDLE(3);

DWG_OBJECT_END

//pg.151
DWG_OBJECT(SORTENTSTABLE);

  FIELD_BL (num_entries);
  FIELD_HANDLE (sort_handle, 0);
  FIELD_HANDLE (parenthandle, 4);
  REACTORS(4);
  XDICOBJHANDLE(3);
  FIELD_HANDLE (owner_handle, 4);
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
  if (GET_FIELD(front_clip_on)==1)
    {
      FIELD_BD (front_clip_dist);
    }

  FIELD_BS (back_clip_on);
  if (GET_FIELD(back_clip_on)==1)
    {
      FIELD_BD (back_clip_dist);
    }

  FIELD_VECTOR_N (inverse_block_transform, BD, 12);
  FIELD_VECTOR_N (clip_bound_transform, BD, 12);
  FIELD_HANDLE (parenthandle, 4);
  REACTORS(4);
  XDICOBJHANDLE(3);

DWG_OBJECT_END

//pg.153
DWG_OBJECT(SPATIAL_INDEX);

  //TODO: Implement-me!

DWG_OBJECT_END

//pg.158
DWG_ENTITY(TABLE);
  int total_attr_def_count = 0;

  FIELD_3BD (insertion_point);

  VERSIONS(R_13,R_14)
    {
      FIELD_3BD (scale);
    }

  SINCE(R_2000)
    {
      FIELD_BB (data_flags);
      switch (GET_FIELD(data_flags))
        {
          case 0:
            GET_FIELD(scale.x)=1.0;
            FIELD_DD(scale.y, GET_FIELD(scale.x));
            FIELD_DD(scale.z, GET_FIELD(scale.x));
            break;
          case 1:
            GET_FIELD(scale.x)=1.0;
            FIELD_DD(scale.y, 1.0);
            FIELD_DD(scale.z, 1.0);
            break;
          case 2:
            FIELD_RD(scale.x);
            GET_FIELD(scale.y) = GET_FIELD(scale.x);
            GET_FIELD(scale.z) = GET_FIELD(scale.x);
            break;
          case 3:
            GET_FIELD(scale.x)=1.0;
            GET_FIELD(scale.y)=1.0;
            GET_FIELD(scale.z)=1.0;
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
  REPEAT_N(GET_FIELD(num_rows)*GET_FIELD(num_cols), cells, Dwg_Entity_TABLE_Cell)
    {
      FIELD_BS(cells[rcount].type);
      FIELD_RC(cells[rcount].flags);
      FIELD_B(cells[rcount].merged_value);
      FIELD_B(cells[rcount].autofit_flag);
      FIELD_BL(cells[rcount].merged_width_flag);
      FIELD_BL(cells[rcount].merged_height_flag);
      FIELD_BD(cells[rcount].rotation_value);

      if (GET_FIELD(cells[rcount].type)==1)
        { /* text cell */
          FIELD_TV(cells[rcount].text_string);
        }
      if (GET_FIELD(cells[rcount].type)==2)
        { /* block cell */
          FIELD_BD(cells[rcount].block_scale);
          FIELD_B(cells[rcount].additional_data_flag);
          if (GET_FIELD(cells[rcount].additional_data_flag) == 1)
            {
              FIELD_BS(cells[rcount].attr_def_count);
              FIELD_BS(cells[rcount].attr_def_index);
              FIELD_TV(cells[rcount].attr_def_text);
              total_attr_def_count += GET_FIELD(cells[rcount].attr_def_count);
            }
        }
      if (GET_FIELD(cells[rcount].type)==1 || GET_FIELD(cells[rcount].type)==2)
        { /* common to both text and block cells */
          FIELD_B(cells[rcount].additional_data_flag);
          if (GET_FIELD(cells[rcount].additional_data_flag) == 1)
            {
              FIELD_BL(cells[rcount].cell_flag_override);
              FIELD_RC(cells[rcount].virtual_edge_flag);
              if (GET_FIELD(cells[rcount].cell_flag_override) & 0x01)
                {
                  FIELD_RS(cells[rcount].cell_alignment);
                }
              if (GET_FIELD(cells[rcount].cell_flag_override) & 0x02)
                {
                  FIELD_B(cells[rcount].background_fill_none);
                }
              if (GET_FIELD(cells[rcount].cell_flag_override) & 0x04)
                {
                  FIELD_CMC(cells[rcount].background_color);
                }
              if (GET_FIELD(cells[rcount].cell_flag_override) & 0x08)
                {
                  FIELD_CMC(cells[rcount].content_color);
                }
              if (GET_FIELD(cells[rcount].cell_flag_override) & 0x20)
                {
                  FIELD_BD(cells[rcount].text_height);
                }
              if (GET_FIELD(cells[rcount].cell_flag_override) & 0x00040)
                {
                  FIELD_CMC(cells[rcount].top_grid_color);
                }
              if (GET_FIELD(cells[rcount].cell_flag_override) & 0x00400)
                {
                  FIELD_BS(cells[rcount].top_grid_linewt);
                }
              if (GET_FIELD(cells[rcount].cell_flag_override) & 0x04000)
                {
                  FIELD_BS(cells[rcount].top_visibility);
                }
              if (GET_FIELD(cells[rcount].cell_flag_override) & 0x00080)
                {
                  FIELD_CMC(cells[rcount].right_grid_color);
                }
              if (GET_FIELD(cells[rcount].cell_flag_override) & 0x00800)
                {
                  FIELD_BS(cells[rcount].right_grid_linewt);
                }
              if (GET_FIELD(cells[rcount].cell_flag_override) & 0x08000)
                {
                  FIELD_BS(cells[rcount].right_visibility);
                }
              if (GET_FIELD(cells[rcount].cell_flag_override) & 0x00100)
                {
                  FIELD_CMC(cells[rcount].bottom_grid_color);
                }
              if (GET_FIELD(cells[rcount].cell_flag_override) & 0x01000)
                {
                  FIELD_BS(cells[rcount].bottom_grid_linewt);
                }
              if (GET_FIELD(cells[rcount].cell_flag_override) & 0x10000)
                {
                  FIELD_BS(cells[rcount].bottom_visibility);
                }
              if (GET_FIELD(cells[rcount].cell_flag_override) & 0x00200)
                {
                  FIELD_CMC(cells[rcount].left_grid_color);
                }
              if (GET_FIELD(cells[rcount].cell_flag_override) & 0x02000)
                {
                  FIELD_BS(cells[rcount].left_grid_linewt);
                }
              if (GET_FIELD(cells[rcount].cell_flag_override) & 0x20000)
                {
                  FIELD_BS(cells[rcount].left_visibility);
                }

              SINCE(R_2007)
                {
                  FIELD_BL(cells[rcount].unknown);
                  FIELD_BL(cells[rcount].flags_2007);
                  FIELD_BL(cells[rcount].data_type);
                  switch (GET_FIELD(cells[rcount].data_type))
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
                        if (loglevel)
                          fprintf(stderr, "ERROR: Unknwon data type in TABLE entity: \"kBuffer\".\n");
                        break;
                      case 128: /* kResBuf */
                        if (loglevel)
                          fprintf(stderr, "ERROR: Unknwon data type in TABLE entity: \"kResBuf\".\n");
                        break;
                      case 256: /* kGeneral */
                        if (loglevel)
                          fprintf(stderr, "ERROR: Unknwon data type in TABLE entity: \"kGeneral\".\n");
                        break;
                      default:
                        if (loglevel)
                          fprintf(stderr, "ERROR: Invalid data type in TABLE entity\n");
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
  if (GET_FIELD(table_overrides_present)==1)
    {
      FIELD_BL (table_flag_override);
      if (GET_FIELD(table_flag_override) & 0x0001)
        {
          FIELD_B (title_suppressed);
        }

      FIELD_B (header_suppressed);

      if (GET_FIELD(table_flag_override) & 0x0004)
        {
          FIELD_BS (flow_direction);
        }

      if (GET_FIELD(table_flag_override) & 0x0008)
        {
          FIELD_BD (horiz_cell_margin);
        }

      if (GET_FIELD(table_flag_override) & 0x0010)
        {
          FIELD_BD (vert_cell_margin);
        }

      if (GET_FIELD(table_flag_override) & 0x0020)
        {
          FIELD_CMC (title_row_color);
        }

      if (GET_FIELD(table_flag_override) & 0x0040)
        {
          FIELD_CMC (header_row_color);
        }

      if (GET_FIELD(table_flag_override) & 0x0080)
        {
          FIELD_CMC (data_row_color);
        }

      if (GET_FIELD(table_flag_override) & 0x0100)
        {
          FIELD_B (title_row_fill_none);
        }

      if (GET_FIELD(table_flag_override) & 0x0200)
        {
          FIELD_B (header_row_fill_none);
        }

      if (GET_FIELD(table_flag_override) & 0x0400)
        {
          FIELD_B (data_row_fill_none);
        }

      if (GET_FIELD(table_flag_override) & 0x0800)
        {
          FIELD_CMC (title_row_fill_color);
        }

      if (GET_FIELD(table_flag_override) & 0x1000)
        {
          FIELD_CMC (header_row_fill_color);
        }

      if (GET_FIELD(table_flag_override) & 0x2000)
        {
          FIELD_CMC (data_row_fill_color);
        }

      if (GET_FIELD(table_flag_override) & 0x4000)
        {
          FIELD_BS (title_row_align);
        }

      if (GET_FIELD(table_flag_override) & 0x8000)
        {
          FIELD_BS (header_row_align);
        }

      if (GET_FIELD(table_flag_override) & 0x10000)
        {
          FIELD_BS (data_row_align);
        }

      if (GET_FIELD(table_flag_override) & 0x100000)
        {
          FIELD_BD (title_row_height);
        }

      if (GET_FIELD(table_flag_override) & 0x200000)
        {
          FIELD_BD (header_row_height);
        }

      if (GET_FIELD(table_flag_override) & 0x400000)
        {
          FIELD_BD (data_row_height);
        }
    }

  FIELD_B (border_color_overrides_present);
  if (GET_FIELD(border_color_overrides_present)==1)
    {
      FIELD_BL (border_color_overrides_flag);
      if (GET_FIELD(border_color_overrides_flag) & 0x0001)
        {
          FIELD_CMC (title_horiz_top_color);
        }

      if (GET_FIELD(border_color_overrides_flag) & 0x0002)
        {
          FIELD_CMC (title_horiz_ins_color);
        }

      if (GET_FIELD(border_color_overrides_flag) & 0x0004)
        {
          FIELD_CMC (title_horiz_bottom_color);
        }

      if (GET_FIELD(border_color_overrides_flag) & 0x0008)
        {
          FIELD_CMC (title_vert_left_color);
        }

      if (GET_FIELD(border_color_overrides_flag) & 0x0010)
        {
          FIELD_CMC (title_vert_ins_color);
        }

      if (GET_FIELD(border_color_overrides_flag) & 0x0020)
        {
          FIELD_CMC (title_vert_right_color);
        }

      if (GET_FIELD(border_color_overrides_flag) & 0x0040)
        {
          FIELD_CMC (header_horiz_top_color);
        }

      if (GET_FIELD(border_color_overrides_flag) & 0x0080)
        {
          FIELD_CMC (header_horiz_ins_color);
        }

      if (GET_FIELD(border_color_overrides_flag) & 0x0100)
        {
          FIELD_CMC (header_horiz_bottom_color);
        }

      if (GET_FIELD(border_color_overrides_flag) & 0x0200)
        {
          FIELD_CMC (header_vert_left_color);
        }

      if (GET_FIELD(border_color_overrides_flag) & 0x0400)
        {
          FIELD_CMC (header_vert_ins_color);
        }

      if (GET_FIELD(border_color_overrides_flag) & 0x0800)
        {
          FIELD_CMC (header_vert_right_color);
        }

      if (GET_FIELD(border_color_overrides_flag) & 0x1000)
        {
          FIELD_CMC (data_horiz_top_color);
        }

      if (GET_FIELD(border_color_overrides_flag) & 0x2000)
        {
          FIELD_CMC (data_horiz_ins_color);
        }

      if (GET_FIELD(border_color_overrides_flag) & 0x4000)
        {
          FIELD_CMC (data_horiz_bottom_color);
        }

      if (GET_FIELD(border_color_overrides_flag) & 0x8000)
        {
          FIELD_CMC (data_vert_left_color);
        }

      if (GET_FIELD(border_color_overrides_flag) & 0x10000)
        {
          FIELD_CMC (data_vert_ins_color);
        }

      if (GET_FIELD(border_color_overrides_flag) & 0x20000)
        {
          FIELD_CMC (data_vert_right_color);
        }
    }

  FIELD_B (border_lineweight_overrides_present);
  if (GET_FIELD(border_lineweight_overrides_present)==1)
    {
      FIELD_BL (border_lineweight_overrides_flag);
      if (GET_FIELD(border_lineweight_overrides_flag) & 0x0001)
        {
          FIELD_BS (title_horiz_top_lineweigh);
        }

      if (GET_FIELD(border_lineweight_overrides_flag) & 0x0002)
        {
          FIELD_BS (title_horiz_ins_lineweigh);
        }

      if (GET_FIELD(border_lineweight_overrides_flag) & 0x0004)
        {
          FIELD_BS (title_horiz_bottom_lineweigh);
        }

      if (GET_FIELD(border_lineweight_overrides_flag) & 0x0008)
        {
          FIELD_BS (title_vert_left_lineweigh);
        }

      if (GET_FIELD(border_lineweight_overrides_flag) & 0x0010)
        {
          FIELD_BS (title_vert_ins_lineweigh);
        }

      if (GET_FIELD(border_lineweight_overrides_flag) & 0x0020)
        {
          FIELD_BS (title_vert_right_lineweigh);
        }

      if (GET_FIELD(border_lineweight_overrides_flag) & 0x0040)
        {
          FIELD_BS (header_horiz_top_lineweigh);
        }

      if (GET_FIELD(border_lineweight_overrides_flag) & 0x0080)
        {
          FIELD_BS (header_horiz_ins_lineweigh);
        }

      if (GET_FIELD(border_lineweight_overrides_flag) & 0x0100)
        {
          FIELD_BS (header_horiz_bottom_lineweigh);
        }

      if (GET_FIELD(border_lineweight_overrides_flag) & 0x0200)
        {
          FIELD_BS (header_vert_left_lineweigh);
        }

      if (GET_FIELD(border_lineweight_overrides_flag) & 0x0400)
        {
          FIELD_BS (header_vert_ins_lineweigh);
        }

      if (GET_FIELD(border_lineweight_overrides_flag) & 0x0800)
        {
          FIELD_BS (header_vert_right_lineweigh);
        }

      if (GET_FIELD(border_lineweight_overrides_flag) & 0x1000)
        {
          FIELD_BS (data_horiz_top_lineweigh);
        }

      if (GET_FIELD(border_lineweight_overrides_flag) & 0x2000)
        {
          FIELD_BS (data_horiz_ins_lineweigh);
        }

      if (GET_FIELD(border_lineweight_overrides_flag) & 0x4000)
        {
          FIELD_BS (data_horiz_bottom_lineweigh);
        }

      if (GET_FIELD(border_lineweight_overrides_flag) & 0x8000)
        {
          FIELD_BS (data_vert_left_lineweigh);
        }

      if (GET_FIELD(border_lineweight_overrides_flag) & 0x10000)
        {
          FIELD_BS (data_vert_ins_lineweigh);
        }

      if (GET_FIELD(border_lineweight_overrides_flag) & 0x20000)
        {
          FIELD_BS (data_vert_right_lineweigh);
        }
    }

  FIELD_B (border_visibility_overrides_present);
  if (GET_FIELD(border_visibility_overrides_present)==1)
    {
      FIELD_BL (border_visibility_overrides_flag);
      if (GET_FIELD(border_visibility_overrides_flag) & 0x0001)
        {
          FIELD_BS (title_horiz_top_visibility);
        }

      if (GET_FIELD(border_visibility_overrides_flag) & 0x0002)
        {
          FIELD_BS (title_horiz_ins_visibility);
        }

      if (GET_FIELD(border_visibility_overrides_flag) & 0x0004)
        {
          FIELD_BS (title_horiz_bottom_visibility);
        }

      if (GET_FIELD(border_visibility_overrides_flag) & 0x0008)
        {
          FIELD_BS (title_vert_left_visibility);
        }

      if (GET_FIELD(border_visibility_overrides_flag) & 0x0010)
        {
          FIELD_BS (title_vert_ins_visibility);
        }

      if (GET_FIELD(border_visibility_overrides_flag) & 0x0020)
        {
          FIELD_BS (title_vert_right_visibility);
        }

      if (GET_FIELD(border_visibility_overrides_flag) & 0x0040)
        {
          FIELD_BS (header_horiz_top_visibility);
        }

      if (GET_FIELD(border_visibility_overrides_flag) & 0x0080)
        {
          FIELD_BS (header_horiz_ins_visibility);
        }

      if (GET_FIELD(border_visibility_overrides_flag) & 0x0100)
        {
          FIELD_BS (header_horiz_bottom_visibility);
        }

      if (GET_FIELD(border_visibility_overrides_flag) & 0x0200)
        {
          FIELD_BS (header_vert_left_visibility);
        }

      if (GET_FIELD(border_visibility_overrides_flag) & 0x0400)
        {
          FIELD_BS (header_vert_ins_visibility);
        }

      if (GET_FIELD(border_visibility_overrides_flag) & 0x0800)
        {
          FIELD_BS (header_vert_right_visibility);
        }

      if (GET_FIELD(border_visibility_overrides_flag) & 0x1000)
        {
          FIELD_BS (data_horiz_top_visibility);
        }

      if (GET_FIELD(border_visibility_overrides_flag) & 0x2000)
        {
          FIELD_BS (data_horiz_ins_visibility);
        }

      if (GET_FIELD(border_visibility_overrides_flag) & 0x4000)
        {
          FIELD_BS (data_horiz_bottom_visibility);
        }

      if (GET_FIELD(border_visibility_overrides_flag) & 0x8000)
        {
          FIELD_BS (data_vert_left_visibility);
        }

      if (GET_FIELD(border_visibility_overrides_flag) & 0x10000)
        {
          FIELD_BS (data_vert_ins_visibility);
        }

      if (GET_FIELD(border_visibility_overrides_flag) & 0x20000)
        {
          FIELD_BS (data_vert_right_visibility);
        }
    }

  FIELD_HANDLE (block_header, 5);

  VERSIONS(R_13, R_2000)
    {
      if (GET_FIELD(has_attribs))
        {
          FIELD_HANDLE (first_attrib, 4);
          FIELD_HANDLE (last_attrib, 4);
        }
    }

  SINCE(R_2004)
    {
      HANDLE_VECTOR(attribs, owned_object_count, 4)
    }

  if (GET_FIELD(has_attribs))
    {
      FIELD_HANDLE(seqend, 3);
    }

  FIELD_HANDLE(table_style_id, ANYCODE);

  REPEAT_N(GET_FIELD(num_rows)*GET_FIELD(num_cols), cells, Dwg_Entity_TABLE_Cell)
    {
      FIELD_HANDLE(cells[rcount].cell_handle, ANYCODE);

      if (GET_FIELD(cells[rcount].type) == 2 &&
          GET_FIELD(cells[rcount].additional_data_flag) == 1)
        {
          HANDLE_VECTOR(cells[rcount].attr_def_id, cells[rcount].attr_def_count, ANYCODE);
        }

      if (GET_FIELD(cells[rcount].additional_data_flag2) == 1 &&
          GET_FIELD(cells[rcount].cell_flag_override) & 0x08)
        {
          FIELD_HANDLE(cells[rcount].text_style_override, ANYCODE);
        }
    }

  FIELD_HANDLE (title_row_style_override, ANYCODE);
  FIELD_HANDLE (header_row_style_override, ANYCODE);
  FIELD_HANDLE (data_row_style_override, ANYCODE);

DWG_ENTITY_END

//pg.164
DWG_OBJECT(XRECORD);

/*

  FIELD_BL(numdatabytes);
  FIELD_BS(cloning_flags);
  FIELD_RS(indicator);
  //since it also counts the first RS indicator
  //GET_FIELD(numdatabytes)--;
  FIELD_VECTOR(data, RC, numdatabytes);
  FIELD_HANDLE(parent, 3);
  REACTORS(4);
  XDICOBJHANDLE(3);
  //XXX how to known when I run out of data?
  //BITCODE_H* objid_handles;
*/
DWG_OBJECT_END

////////////////////
// These objects are not described in the spec:
//
// DICTIONARYWDLFT
// WIPEOUTVARIABLE
// PLACEHOLDER
// VBA_PROJECT
//

