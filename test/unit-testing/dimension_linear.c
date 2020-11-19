#define DWG_TYPE DWG_TYPE_DIMENSION_LINEAR
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  double elevation, act_measurement, horiz_dir, lspace_factor, text_rotation,
      ins_rotation, oblique_angle, dim_rotation;
  BITCODE_B flip_arrow1, flip_arrow2;
  BITCODE_RC class_version, flag, flag1;
  BITCODE_BS lspace_style, attachment;
  char *user_text;
  int isnew;
  dwg_point_2d text_midpt, clone_ins_pt;
  dwg_point_3d def_pt, xline1_pt, xline2_pt, extrusion, ins_scale, pt3d;
  BITCODE_H dimstyle, block;

  dwg_ent_dim_linear *dim_linear = dwg_object_to_DIMENSION_LINEAR (obj);
  dwg_ent_dim *dim = dwg_object_to_DIMENSION (obj);

  /* common */
  CHK_ENTITY_TYPE (dim, DIMENSION_LINEAR, class_version, RC);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_LINEAR, lspace_style, BS);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_LINEAR, lspace_factor, BD);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_LINEAR, act_measurement, BD);
  CHK_ENTITY_TYPE (dim, DIMENSION_LINEAR, attachment, BS);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_LINEAR, elevation, BD);
  CHK_ENTITY_3RD (dim, DIMENSION_LINEAR, extrusion);
  CHK_ENTITY_2RD_W_OLD (dim, DIMENSION_LINEAR, clone_ins_pt);
  CHK_ENTITY_2RD_W_OLD (dim, DIMENSION_LINEAR, text_midpt);
  CHK_ENTITY_UTF8TEXT (dim, DIMENSION_LINEAR, user_text);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_LINEAR, text_rotation, BD);
  CHK_ENTITY_MAX (dim, DIMENSION_LINEAR, text_rotation, BD, MAX_ANGLE);
  CHK_ENTITY_3RD_W_OLD (dim, DIMENSION_LINEAR, ins_scale);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_LINEAR, ins_rotation, BD);
  CHK_ENTITY_MAX (dim, DIMENSION_LINEAR, ins_rotation, BD, MAX_ANGLE);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_LINEAR, horiz_dir, BD);
  CHK_ENTITY_MAX (dim, DIMENSION_LINEAR, horiz_dir, BD, MAX_ANGLE);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_LINEAR, flip_arrow1, B);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_LINEAR, flip_arrow2, B);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_LINEAR, flag1, RC);
  CHK_ENTITY_TYPE (dim, DIMENSION_LINEAR, flag, RC);

  /* linear */
  CHK_ENTITY_3RD_W_OLD (dim_linear, DIMENSION_LINEAR, def_pt);
  CHK_ENTITY_3RD (dim_linear, DIMENSION_LINEAR, xline1_pt);
#ifdef USE_DEPRECATED_API
  dwg_ent_dim_linear_get_13_pt (dim_linear, &pt3d, &error);
  if (error || memcmp (&xline1_pt, &pt3d, sizeof (xline1_pt)))
    fail ("DIMENSION_LINEAR.xline1_pt");
  else
    pass ();
#endif
  CHK_ENTITY_3RD (dim_linear, DIMENSION_LINEAR, xline2_pt);
#ifdef USE_DEPRECATED_API
  dwg_ent_dim_linear_get_14_pt (dim_linear, &pt3d, &error);
  if (error || memcmp (&xline2_pt, &pt3d, sizeof (xline2_pt)))
    fail ("DIMENSION_LINEAR.xline2_pt");
  else
    pass ();
#endif
  CHK_ENTITY_TYPE (dim_linear, DIMENSION_LINEAR, oblique_angle, BD);
  CHK_ENTITY_TYPE_W_OLD (dim_linear, DIMENSION_LINEAR, dim_rotation, BD);

  CHK_ENTITY_H (dim_linear, DIMENSION_LINEAR, dimstyle);
  CHK_ENTITY_H (dim_linear, DIMENSION_LINEAR, block);
}
