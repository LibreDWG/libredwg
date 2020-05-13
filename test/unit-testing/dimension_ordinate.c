#define DWG_TYPE DWG_TYPE_DIMENSION_ORDINATE
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  double elevation, act_measurement, horiz_dir, lspace_factor, text_rotation,
      ins_rotation, oblique_angle;
  BITCODE_B flip_arrow1, flip_arrow2;
  BITCODE_RC class_version, flag, flag1, flag2;
  BITCODE_BS lspace_style, attachment;
  char *user_text;
  dwg_point_2d text_midpt, clone_ins_pt;
  dwg_point_3d def_pt, feature_location_pt, leader_endpt, extrusion, ins_scale;
  BITCODE_H dimstyle, block;

  dwg_ent_dim_ordinate *dim_ordinate = dwg_object_to_DIMENSION_ORDINATE (obj);
  dwg_ent_dim *dim = dwg_object_to_DIMENSION (obj);

  /* common */
  CHK_ENTITY_TYPE (dim, DIMENSION_ORDINATE, class_version, RC);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_ORDINATE, lspace_style, BS);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_ORDINATE, lspace_factor, BD);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_ORDINATE, act_measurement, BD);
  CHK_ENTITY_TYPE (dim, DIMENSION_ORDINATE, attachment, BS);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_ORDINATE, elevation, BD);
  CHK_ENTITY_3RD (dim, DIMENSION_ORDINATE, extrusion);
  CHK_ENTITY_2RD_W_OLD (dim, DIMENSION_ORDINATE, clone_ins_pt);
  CHK_ENTITY_2RD_W_OLD (dim, DIMENSION_ORDINATE, text_midpt);
  CHK_ENTITY_UTF8TEXT (dim, DIMENSION_ORDINATE, user_text);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_ORDINATE, text_rotation, BD);
  CHK_ENTITY_MAX (dim, DIMENSION_ORDINATE, text_rotation, BD, MAX_ANGLE);
  CHK_ENTITY_3RD_W_OLD (dim, DIMENSION_ORDINATE, ins_scale);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_ORDINATE, ins_rotation, BD);
  CHK_ENTITY_MAX (dim, DIMENSION_ORDINATE, ins_rotation, BD, MAX_ANGLE);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_ORDINATE, horiz_dir, BD);
  CHK_ENTITY_MAX (dim, DIMENSION_ORDINATE, horiz_dir, BD, MAX_ANGLE);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_ORDINATE, flip_arrow1, B);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_ORDINATE, flip_arrow2, B);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_ORDINATE, flag1, RC);
  CHK_ENTITY_TYPE (dim, DIMENSION_ORDINATE, flag, RC);

  /* ordinate */
  CHK_ENTITY_3RD_W_OLD (dim_ordinate, DIMENSION_ORDINATE, def_pt);
  CHK_ENTITY_3RD_W_OLD (dim_ordinate, DIMENSION_ORDINATE, feature_location_pt);
  CHK_ENTITY_3RD_W_OLD (dim_ordinate, DIMENSION_ORDINATE, leader_endpt);
  CHK_ENTITY_TYPE_W_OLD (dim_ordinate, DIMENSION_ORDINATE, flag2, RC);

  CHK_ENTITY_H (dim_ordinate, DIMENSION_ORDINATE, dimstyle);
  CHK_ENTITY_H (dim_ordinate, DIMENSION_ORDINATE, block);
}
