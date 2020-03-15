#define DWG_TYPE DWG_TYPE_ARC_DIMENSION
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  double elevation, act_measurement, horiz_dir, lspace_factor, text_rotation,
      ins_rotation;
  BITCODE_B flip_arrow1, flip_arrow2;
  BITCODE_RC class_version, flag, flag1;
  BITCODE_BS lspace_style, attachment;
  char *user_text;
  dwg_point_2d text_midpt, pt12, pt2d;
  dwg_point_3d pt10, pt13, pt14, pt15, pt16, pt17, unknown_pt, ext, ins_scale, pt3d;
  BITCODE_BD leader_len;
  BITCODE_RC flag2;

  BITCODE_H dimstyle, block;
  int isnew;

#ifdef DEBUG_CLASSES
  dwg_ent_arc_dimension *arc_dimension = dwg_object_to_ARC_DIMENSION (obj);
#endif
  dwg_ent_dim *dim = dwg_object_to_DIMENSION (obj);

  if (obj->fixedtype == DWG_TYPE_UNKNOWN_ENT)
    return;
#ifdef DEBUG_CLASSES
  /* common */
  CHK_ENTITY_TYPE (dim, ARC_DIMENSION, class_version, RC, class_version);
  CHK_ENTITY_TYPE_W_OLD (dim, ARC_DIMENSION, lspace_style, BS,
                         lspace_style);
  CHK_ENTITY_TYPE_W_OLD (dim, ARC_DIMENSION, lspace_factor, BD,
                         lspace_factor);
  CHK_ENTITY_TYPE_W_OLD (dim, ARC_DIMENSION, act_measurement, BD,
                         act_measurement);
  CHK_ENTITY_TYPE (dim, ARC_DIMENSION, attachment, BS, attachment);
  CHK_ENTITY_TYPE_W_OLD (dim, ARC_DIMENSION, elevation, BD, elevation);
  CHK_ENTITY_3RD (dim, ARC_DIMENSION, extrusion, ext);
  CHK_ENTITY_2RD_W_OLD (dim, ARC_DIMENSION, clone_ins_pt, pt12);
  CHK_ENTITY_2RD_W_OLD (dim, ARC_DIMENSION, text_midpt, text_midpt);
  CHK_ENTITY_UTF8TEXT (dim, ARC_DIMENSION, user_text, user_text);
  CHK_ENTITY_TYPE_W_OLD (dim, ARC_DIMENSION, text_rotation, BD,
                         text_rotation);
  CHK_ENTITY_3RD_W_OLD (dim, ARC_DIMENSION, ins_scale, ins_scale);
  CHK_ENTITY_TYPE_W_OLD (dim, ARC_DIMENSION, ins_rotation, BD,
                         ins_rotation);
  CHK_ENTITY_TYPE_W_OLD (dim, ARC_DIMENSION, horiz_dir, BD, horiz_dir);
  CHK_ENTITY_MAX (dim, ARC_DIMENSION, text_rotation, BD, 6.284);
  CHK_ENTITY_MAX (dim, ARC_DIMENSION, ins_rotation, BD, 6.284);
  CHK_ENTITY_MAX (dim, ARC_DIMENSION, horiz_dir, BD, 6.284);
  CHK_ENTITY_TYPE_W_OLD (dim, ARC_DIMENSION, flip_arrow1, B, flip_arrow1);
  CHK_ENTITY_TYPE_W_OLD (dim, ARC_DIMENSION, flip_arrow2, B, flip_arrow2);
  CHK_ENTITY_TYPE_W_OLD (dim, ARC_DIMENSION, flag1, RC, flag1);
  CHK_ENTITY_TYPE (dim, ARC_DIMENSION, flag, RC, flag);

  /* arc_dimension */
  CHK_ENTITY_3RD (arc_dimension, ARC_DIMENSION, def_pt, pt10);
  CHK_ENTITY_3RD (arc_dimension, ARC_DIMENSION, _13_pt, pt13);
  CHK_ENTITY_3RD (arc_dimension, ARC_DIMENSION, _14_pt, pt14);
  CHK_ENTITY_3RD (arc_dimension, ARC_DIMENSION, _15_pt, pt15);
  CHK_ENTITY_3RD (arc_dimension, ARC_DIMENSION, _16_pt, pt16);
  CHK_ENTITY_3RD (arc_dimension, ARC_DIMENSION, _17_pt, pt17);
  CHK_ENTITY_3RD (arc_dimension, ARC_DIMENSION, unknown_pt, unknown_pt);
  CHK_ENTITY_TYPE (arc_dimension, ARC_DIMENSION, leader_len, BD, leader_len);
  CHK_ENTITY_TYPE (arc_dimension, ARC_DIMENSION, flag2, RC, flag2);

  CHK_ENTITY_H (arc_dimension, ARC_DIMENSION, dimstyle, dimstyle);
  CHK_ENTITY_H (arc_dimension, ARC_DIMENSION, block, block);
#endif
}
