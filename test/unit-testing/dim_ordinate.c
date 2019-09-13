#define DWG_TYPE DWG_TYPE_DIMENSION_ORDINATE
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  double elevation, act_measurement, horiz_dir, lspace_factor, text_rotation,
      ins_rotation, ext_line_rotation;
  BITCODE_B flip_arrow1, flip_arrow2;
  BITCODE_RC class_version, flag, flag1, flag2;
  BITCODE_BS lspace_style, attachment;
  char *user_text;
  dwg_point_2d text_midpt, pt12;
  dwg_point_3d pt10, pt13, pt14, ext, ins_scale, pt3d;
  BITCODE_H dimstyle, block;

  dwg_ent_dim_ordinate *dim_ordinate = dwg_object_to_DIMENSION_ORDINATE (obj);
  dwg_ent_dim *dim = dwg_object_to_DIMENSION (obj);

  /* common */
  CHK_ENTITY_TYPE (dim, DIMENSION_ORDINATE, class_version, RC, class_version);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_ORDINATE, lspace_style, BS,
                         lspace_style);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_ORDINATE, lspace_factor, BD,
                         lspace_factor);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_ORDINATE, act_measurement, BD,
                         act_measurement);
  CHK_ENTITY_TYPE (dim, DIMENSION_ORDINATE, attachment, BS, attachment);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_ORDINATE, elevation, BD, elevation);
  CHK_ENTITY_3RD (dim, DIMENSION_ORDINATE, extrusion, ext);
  CHK_ENTITY_2RD_W_OLD (dim, DIMENSION_ORDINATE, clone_ins_pt, pt12);
  CHK_ENTITY_2RD_W_OLD (dim, DIMENSION_ORDINATE, text_midpt, text_midpt);
  CHK_ENTITY_UTF8TEXT (dim, DIMENSION_ORDINATE, user_text, user_text);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_ORDINATE, text_rotation, BD,
                         text_rotation);
  CHK_ENTITY_3RD_W_OLD (dim, DIMENSION_ORDINATE, ins_scale, ins_scale);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_ORDINATE, ins_rotation, BD,
                         ins_rotation);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_ORDINATE, horiz_dir, BD, horiz_dir);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_ORDINATE, flip_arrow1, B, flip_arrow1);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_ORDINATE, flip_arrow2, B, flip_arrow2);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_ORDINATE, flag1, RC, flag1);
  CHK_ENTITY_TYPE (dim, DIMENSION_ORDINATE, flag, RC, flag);

  /* ordinate */
  CHK_ENTITY_3RD_W_OLD (dim_ordinate, DIMENSION_ORDINATE, def_pt, pt10);
  CHK_ENTITY_3RD_W_OLD (dim_ordinate, DIMENSION_ORDINATE, feature_location_pt,
                        pt13);
  CHK_ENTITY_3RD_W_OLD (dim_ordinate, DIMENSION_ORDINATE, leader_endpt, pt14);
  CHK_ENTITY_TYPE_W_OLD (dim_ordinate, DIMENSION_ORDINATE, flag2, RC, flag2);

  CHK_ENTITY_H (dim_ordinate, DIMENSION_ORDINATE, dimstyle, dimstyle);
  CHK_ENTITY_H (dim_ordinate, DIMENSION_ORDINATE, block, block);
}
