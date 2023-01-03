#define DWG_TYPE DWG_TYPE_DIMENSION_ANG3PT
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  double elevation, act_measurement, horiz_dir, lspace_factor, text_rotation,
      ins_rotation;
  BITCODE_B flip_arrow1, flip_arrow2;
  BITCODE_RC class_version, flag, flag1;
  BITCODE_BS lspace_style, attachment;
  char *user_text;
  dwg_point_2d text_midpt, clone_ins_pt, pt2d;
  dwg_point_3d def_pt, xline1_pt, xline2_pt, center_pt, extrusion, ins_scale,
      pt3d;
  BITCODE_H dimstyle, block;

  dwg_ent_dim_ang3pt *dim_ang3pt = dwg_object_to_DIMENSION_ANG3PT (obj);
  dwg_ent_dim *dim = dwg_object_to_DIMENSION (obj);

  /* common */
  CHK_ENTITY_TYPE (dim, DIMENSION_ANG3PT, class_version, RC);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_ANG3PT, lspace_style, BS);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_ANG3PT, lspace_factor, BD);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_ANG3PT, act_measurement, BD);
  CHK_ENTITY_TYPE (dim, DIMENSION_ANG3PT, attachment, BS);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_ANG3PT, elevation, BD);
  CHK_ENTITY_3RD (dim, DIMENSION_ANG3PT, extrusion);
  CHK_ENTITY_2RD_W_OLD (dim, DIMENSION_ANG3PT, clone_ins_pt);
  CHK_ENTITY_2RD_W_OLD (dim, DIMENSION_ANG3PT, text_midpt);
  CHK_ENTITY_UTF8TEXT (dim, DIMENSION_ANG3PT, user_text);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_ANG3PT, text_rotation, BD);
  CHK_ENTITY_3RD_W_OLD (dim, DIMENSION_ANG3PT, ins_scale);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_ANG3PT, ins_rotation, BD);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_ANG3PT, horiz_dir, BD);
  CHK_ENTITY_MAX (dim, DIMENSION_ANG3PT, text_rotation, BD, MAX_ANGLE);
  CHK_ENTITY_MAX (dim, DIMENSION_ANG3PT, ins_rotation, BD, MAX_ANGLE);
  CHK_ENTITY_MAX (dim, DIMENSION_ANG3PT, horiz_dir, BD, MAX_ANGLE);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_ANG3PT, flip_arrow1, B);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_ANG3PT, flip_arrow2, B);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_ANG3PT, flag1, RC);
  CHK_ENTITY_TYPE (dim, DIMENSION_ANG3PT, flag, RC);

  /* ang3pt */
  CHK_ENTITY_3RD_W_OLD (dim_ang3pt, DIMENSION_ANG3PT, def_pt);
  CHK_ENTITY_3RD (dim_ang3pt, DIMENSION_ANG3PT, center_pt);
#ifdef USE_DEPRECATED_API
  dwg_ent_dim_ang3pt_get_first_arc_pt (dim_ang3pt, &pt3d, &error);
  if (error || memcmp (&center_pt, &pt3d, sizeof (center_pt)))
    fail ("old API dwg_ent_dim_ang3pt_get_first_arc_pt");
  else
    pass ();
#endif
  CHK_ENTITY_3RD (dim_ang3pt, DIMENSION_ANG3PT, xline1_pt);
#ifdef USE_DEPRECATED_API
  dwg_ent_dim_ang3pt_get_13_pt (dim_ang3pt, &pt3d, &error);
  if (error || memcmp (&xline1_pt, &pt3d, sizeof (xline1_pt)))
    fail ("old API dwg_ent_dim_ang3pt_get_13_pt");
  else
    pass ();
#endif
  CHK_ENTITY_3RD (dim_ang3pt, DIMENSION_ANG3PT, xline2_pt);
#ifdef USE_DEPRECATED_API
  dwg_ent_dim_ang3pt_get_14_pt (dim_ang3pt, &pt3d, &error);
  if (error || memcmp (&xline2_pt, &pt3d, sizeof (xline2_pt)))
    fail ("old API dwg_ent_dim_ang3pt_get_14_pt");
  else
    pass ();
#endif

  CHK_ENTITY_H (dim_ang3pt, DIMENSION_ANG3PT, dimstyle);
  CHK_ENTITY_H (dim_ang3pt, DIMENSION_ANG3PT, block);
}
