// unstable
#define DWG_TYPE DWG_TYPE_ARC_DIMENSION
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
  dwg_point_3d def_pt, extrusion, ins_scale;
  BITCODE_H dimstyle, block;

  BITCODE_3BD xline1_pt;
  BITCODE_3BD xline2_pt;
  BITCODE_3BD center_pt;
  BITCODE_B is_partial;
  BITCODE_BD arc_start_param;
  BITCODE_BD arc_end_param;
  BITCODE_B has_leader;
  BITCODE_3BD leader1_pt;
  BITCODE_3BD leader2_pt;

  dwg_ent_arc_dimension *arc_dimension = dwg_object_to_ARC_DIMENSION (obj);
  dwg_ent_dim *dim = dwg_object_to_DIMENSION (obj);

  // if (obj->fixedtype == DWG_TYPE_UNKNOWN_ENT)
  //   return;

  /* DIMENSION_COMMON */
  CHK_ENTITY_TYPE (dim, ARC_DIMENSION, class_version, RC);
  CHK_ENTITY_TYPE (dim, ARC_DIMENSION, lspace_style, BS);
  CHK_ENTITY_TYPE (dim, ARC_DIMENSION, lspace_factor, BD);
  CHK_ENTITY_TYPE (dim, ARC_DIMENSION, act_measurement, BD);
  CHK_ENTITY_TYPE (dim, ARC_DIMENSION, attachment, BS);
  CHK_ENTITY_TYPE (dim, ARC_DIMENSION, elevation, BD);
  CHK_ENTITY_3RD (dim, ARC_DIMENSION, extrusion);
  CHK_ENTITY_2RD (dim, ARC_DIMENSION, clone_ins_pt);
  CHK_ENTITY_2RD (dim, ARC_DIMENSION, text_midpt);
  CHK_ENTITY_UTF8TEXT (dim, ARC_DIMENSION, user_text);
  CHK_ENTITY_TYPE (dim, ARC_DIMENSION, text_rotation, BD);
  CHK_ENTITY_3RD (dim, ARC_DIMENSION, ins_scale);
  CHK_ENTITY_TYPE (dim, ARC_DIMENSION, ins_rotation, BD);
  CHK_ENTITY_TYPE (dim, ARC_DIMENSION, horiz_dir, BD);
  CHK_ENTITY_MAX (dim, ARC_DIMENSION, text_rotation, BD, MAX_ANGLE);
  CHK_ENTITY_MAX (dim, ARC_DIMENSION, ins_rotation, BD, MAX_ANGLE);
  CHK_ENTITY_MAX (dim, ARC_DIMENSION, horiz_dir, BD, MAX_ANGLE);
  CHK_ENTITY_TYPE (dim, ARC_DIMENSION, flip_arrow1, B);
  CHK_ENTITY_TYPE (dim, ARC_DIMENSION, flip_arrow2, B);
  CHK_ENTITY_TYPE (dim, ARC_DIMENSION, flag1, RC);
  CHK_ENTITY_TYPE (dim, ARC_DIMENSION, flag, RC);
  CHK_ENTITY_H (dim, ARC_DIMENSION, dimstyle);
  CHK_ENTITY_H (dim, ARC_DIMENSION, block);

  /* arc_dimension */
  CHK_ENTITY_3RD (arc_dimension, ARC_DIMENSION, def_pt);
  CHK_ENTITY_3RD (arc_dimension, ARC_DIMENSION, xline1_pt);
  CHK_ENTITY_3RD (arc_dimension, ARC_DIMENSION, xline2_pt);
  CHK_ENTITY_3RD (arc_dimension, ARC_DIMENSION, center_pt);
  CHK_ENTITY_3RD (arc_dimension, ARC_DIMENSION, leader1_pt);
  CHK_ENTITY_3RD (arc_dimension, ARC_DIMENSION, leader2_pt);
  CHK_ENTITY_TYPE (arc_dimension, ARC_DIMENSION, arc_start_param, BD);
  CHK_ENTITY_TYPE (arc_dimension, ARC_DIMENSION, arc_end_param, BD);
  CHK_ENTITY_TYPE (arc_dimension, ARC_DIMENSION, is_partial, B);
  CHK_ENTITY_TYPE (arc_dimension, ARC_DIMENSION, has_leader, B);
}
