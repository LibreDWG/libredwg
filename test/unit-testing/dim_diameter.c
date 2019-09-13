#define DWG_TYPE DWG_TYPE_DIMENSION_DIAMETER
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  double elevation, act_measurement, horiz_dir, lspace_factor, text_rotation,
      ins_rotation, leader_len;
  BITCODE_B flip_arrow1, flip_arrow2;
  BITCODE_RC class_version, flag, flag1;
  BITCODE_BS lspace_style, attachment;
  char *user_text;
  dwg_point_2d text_midpt, pt12;
  dwg_point_3d pt10, first_arc_pt, ext, ins_scale, pt3d;
  BITCODE_H dimstyle, block;

  dwg_ent_dim_diameter *dim_diameter = dwg_object_to_DIMENSION_DIAMETER (obj);
  dwg_ent_dim *dim = dwg_object_to_DIMENSION (obj);

  /* common */
  CHK_ENTITY_TYPE (dim, DIMENSION_DIAMETER, class_version, RC, class_version);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_DIAMETER, lspace_style, BS,
                         lspace_style);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_DIAMETER, lspace_factor, BD,
                         lspace_factor);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_DIAMETER, act_measurement, BD,
                         act_measurement);
  CHK_ENTITY_TYPE (dim, DIMENSION_DIAMETER, attachment, BS, attachment);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_DIAMETER, elevation, BD, elevation);
  CHK_ENTITY_3RD (dim, DIMENSION_DIAMETER, extrusion, ext);
  CHK_ENTITY_2RD_W_OLD (dim, DIMENSION_DIAMETER, clone_ins_pt, pt12);
  CHK_ENTITY_2RD_W_OLD (dim, DIMENSION_DIAMETER, text_midpt, text_midpt);
  CHK_ENTITY_UTF8TEXT (dim, DIMENSION_DIAMETER, user_text, user_text);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_DIAMETER, text_rotation, BD,
                         text_rotation);
  CHK_ENTITY_3RD_W_OLD (dim, DIMENSION_DIAMETER, ins_scale, ins_scale);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_DIAMETER, ins_rotation, BD,
                         ins_rotation);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_DIAMETER, horiz_dir, BD, horiz_dir);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_DIAMETER, flip_arrow1, B, flip_arrow1);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_DIAMETER, flip_arrow2, B, flip_arrow2);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_DIAMETER, flag1, RC, flag1);
  CHK_ENTITY_TYPE (dim, DIMENSION_DIAMETER, flag, RC, flag);

  /* diameter */
  CHK_ENTITY_3RD_W_OLD (dim_diameter, DIMENSION_DIAMETER, def_pt, pt10);
  CHK_ENTITY_3RD_W_OLD (dim_diameter, DIMENSION_DIAMETER, first_arc_pt,
                        first_arc_pt);
  CHK_ENTITY_TYPE (dim_diameter, DIMENSION_DIAMETER, leader_len, BD,
                   leader_len);
  if (leader_len
          != dwg_ent_dim_diameter_get_leader_length (dim_diameter, &error)
      || error)
    fail ("old API dwg_ent_dim_diameter_get_leader_length");
  else
    pass ();

  CHK_ENTITY_H (dim_diameter, DIMENSION_DIAMETER, dimstyle, dimstyle);
  CHK_ENTITY_H (dim_diameter, DIMENSION_DIAMETER, block, block);
}
