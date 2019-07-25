#define DWG_TYPE DWG_TYPE_DIMENSION_RADIUS
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  double elevation, act_measurement, horiz_dir, lspace_factor,
    text_rotation, ins_rotation, leader_len;
  BITCODE_B flip_arrow1, flip_arrow2;
  BITCODE_RC class_version, flag, flag1;
  BITCODE_BS lspace_style, attachment;
  char *user_text;
  dwg_point_2d text_midpt, pt12;
  dwg_point_3d def_pt, first_arc_pt, ext, ins_scale;
  BITCODE_H dimstyle, block;

  dwg_ent_dim_radius *dim_radius = dwg_object_to_DIMENSION_RADIUS (obj);
  dwg_ent_dim *dim = dwg_object_to_DIMENSION (obj);

  /* common */
  CHK_ENTITY_TYPE (dim, DIMENSION_ORDINATE, class_version, RC, class_version);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_ORDINATE, lspace_style, BS, lspace_style);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_ORDINATE, lspace_factor, BD, lspace_factor);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_ORDINATE, act_measurement,
                         BD, act_measurement);
  CHK_ENTITY_TYPE (dim, DIMENSION_ORDINATE, attachment, BS, attachment);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_ORDINATE, elevation, BD, elevation);
  CHK_ENTITY_3RD (dim, DIMENSION_ORDINATE, extrusion, ext);
  CHK_ENTITY_2RD_W_OLD (dim, DIMENSION_ORDINATE, clone_ins_pt, pt12);
  CHK_ENTITY_2RD_W_OLD (dim, DIMENSION_ORDINATE, text_midpt, text_midpt);
  CHK_ENTITY_UTF8TEXT (dim, DIMENSION_ORDINATE, user_text, user_text);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_ORDINATE, text_rotation, BD, text_rotation);
  CHK_ENTITY_3RD_W_OLD (dim, DIMENSION_ORDINATE, ins_scale, ins_scale);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_ORDINATE, ins_rotation, BD, ins_rotation);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_ORDINATE, horiz_dir, BD, horiz_dir);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_ORDINATE, flip_arrow1, B, flip_arrow1);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_ORDINATE, flip_arrow2, B, flip_arrow2);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_ORDINATE, flag1, RC, flag1);
  CHK_ENTITY_TYPE (dim, DIMENSION_ORDINATE, flag, RC, flag);

  /* radius */
  CHK_ENTITY_3RD_W_OLD (dim_radius, DIMENSION_RADIUS, first_arc_pt,
                        first_arc_pt);
  CHK_ENTITY_TYPE (dim_radius, DIMENSION_RADIUS, leader_len, BD,
                   leader_len);
  if (leader_len != dwg_ent_dim_radius_get_leader_length (dim_radius, &error)
      || error)
    fail ("old API dwg_ent_dim_radius_get_leader_length");
  else
    pass ();

  CHK_ENTITY_H (dim_radius, DIMENSION_RADIUS, dimstyle, dimstyle);
  CHK_ENTITY_H (dim_radius, DIMENSION_RADIUS, block, block);
}
