// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_LARGE_RADIAL_DIMENSION
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  double elevation, act_measurement, horiz_dir, lspace_factor, text_rotation,
      ins_rotation, leader_len;
  BITCODE_T user_text;
  BITCODE_B flip_arrow1, flip_arrow2;
  BITCODE_RC class_version, flag, flag1;
  BITCODE_BS lspace_style, attachment;
  dwg_point_2d text_midpt, clone_ins_pt;
  dwg_point_3d def_pt, first_arc_pt, extrusion, ins_scale;
  BITCODE_H dimstyle, block;
  dwg_point_3d ovr_center, jog_point;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_ent_large_radial_dimension *dim_large
      = dwg_object_to_LARGE_RADIAL_DIMENSION (obj);
  dwg_ent_dim *dim = dwg_object_to_DIMENSION (obj);

  /* DIMENSION_COMMON */
  CHK_ENTITY_TYPE (dim, LARGE_RADIAL_DIMENSION, class_version, RC);
  CHK_ENTITY_TYPE (dim, LARGE_RADIAL_DIMENSION, lspace_style, BS);
  CHK_ENTITY_TYPE (dim, LARGE_RADIAL_DIMENSION, lspace_factor, BD);
  CHK_ENTITY_TYPE (dim, LARGE_RADIAL_DIMENSION, act_measurement, BD);
  CHK_ENTITY_TYPE (dim, LARGE_RADIAL_DIMENSION, attachment, BS);
  CHK_ENTITY_TYPE (dim, LARGE_RADIAL_DIMENSION, elevation, BD);
  CHK_ENTITY_3RD (dim, LARGE_RADIAL_DIMENSION, extrusion);
  CHK_ENTITY_3RD (dim, LARGE_RADIAL_DIMENSION, def_pt);
  CHK_ENTITY_2RD (dim, LARGE_RADIAL_DIMENSION, clone_ins_pt);
  CHK_ENTITY_2RD (dim, LARGE_RADIAL_DIMENSION, text_midpt);
  CHK_ENTITY_UTF8TEXT (dim, LARGE_RADIAL_DIMENSION, user_text);
  CHK_ENTITY_TYPE (dim, LARGE_RADIAL_DIMENSION, text_rotation, BD);
  CHK_ENTITY_3RD (dim, LARGE_RADIAL_DIMENSION, ins_scale);
  CHK_ENTITY_TYPE (dim, LARGE_RADIAL_DIMENSION, ins_rotation, BD);
  CHK_ENTITY_TYPE (dim, LARGE_RADIAL_DIMENSION, horiz_dir, BD);
  CHK_ENTITY_MAX (dim, LARGE_RADIAL_DIMENSION, text_rotation, BD, MAX_ANGLE);
  CHK_ENTITY_MAX (dim, LARGE_RADIAL_DIMENSION, ins_rotation, BD, MAX_ANGLE);
  CHK_ENTITY_MAX (dim, LARGE_RADIAL_DIMENSION, horiz_dir, BD, MAX_ANGLE);
  CHK_ENTITY_TYPE (dim, LARGE_RADIAL_DIMENSION, flip_arrow1, B);
  CHK_ENTITY_TYPE (dim, LARGE_RADIAL_DIMENSION, flip_arrow2, B);
  CHK_ENTITY_TYPE (dim, LARGE_RADIAL_DIMENSION, flag1, RC);
  CHK_ENTITY_TYPE (dim, LARGE_RADIAL_DIMENSION, flag, RC);
  CHK_ENTITY_H (dim, LARGE_RADIAL_DIMENSION, dimstyle);
  CHK_ENTITY_H (dim, LARGE_RADIAL_DIMENSION, block);
  /* radius */
  CHK_ENTITY_3RD (dim_large, LARGE_RADIAL_DIMENSION, first_arc_pt);
  CHK_ENTITY_TYPE (dim_large, LARGE_RADIAL_DIMENSION, leader_len, BD);

  CHK_ENTITY_3RD (dim_large, LARGE_RADIAL_DIMENSION, ovr_center);
  CHK_ENTITY_3RD (dim_large, LARGE_RADIAL_DIMENSION, jog_point);
#endif
}
