#define DWG_TYPE DWG_TYPE_DIMENSION_ANG2LN
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
  dwg_point_3d def_pt, extrusion, ins_scale, pt3d;
  BITCODE_H dimstyle, block;

  BITCODE_3BD xline1start_pt;
  BITCODE_3BD xline1end_pt;
  BITCODE_3BD xline2start_pt;
  BITCODE_3BD xline2end_pt;

  dwg_ent_dim_ang2ln *dim_ang2ln = dwg_object_to_DIMENSION_ANG2LN (obj);
  dwg_ent_dim *dim = dwg_object_to_DIMENSION (obj);

  /* common */
  CHK_ENTITY_TYPE (dim, DIMENSION_ANG2LN, class_version, RC);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_ANG2LN, lspace_style, BS);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_ANG2LN, lspace_factor, BD);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_ANG2LN, act_measurement, BD);
  CHK_ENTITY_TYPE (dim, DIMENSION_ANG2LN, attachment, BS);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_ANG2LN, elevation, BD);
  CHK_ENTITY_3RD (dim, DIMENSION_ANG2LN, extrusion);
  CHK_ENTITY_2RD_W_OLD (dim, DIMENSION_ANG2LN, clone_ins_pt);
  CHK_ENTITY_2RD_W_OLD (dim, DIMENSION_ANG2LN, text_midpt);
  CHK_ENTITY_UTF8TEXT (dim, DIMENSION_ANG2LN, user_text);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_ANG2LN, text_rotation, BD);
  CHK_ENTITY_3RD_W_OLD (dim, DIMENSION_ANG2LN, ins_scale);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_ANG2LN, ins_rotation, BD);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_ANG2LN, horiz_dir, BD);
  CHK_ENTITY_MAX (dim, DIMENSION_ANG2LN, text_rotation, BD, MAX_ANGLE);
  CHK_ENTITY_MAX (dim, DIMENSION_ANG2LN, ins_rotation, BD, MAX_ANGLE);
  CHK_ENTITY_MAX (dim, DIMENSION_ANG2LN, horiz_dir, BD, MAX_ANGLE);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_ANG2LN, flip_arrow1, B);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_ANG2LN, flip_arrow2, B);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_ANG2LN, flag1, RC);
  CHK_ENTITY_TYPE (dim, DIMENSION_ANG2LN, flag, RC);
  // Note: def_pt.z should be the elevation here, but is not.
  CHK_ENTITY_2RD (dim_ang2ln, DIMENSION_ANG2LN, def_pt);

  /* ang2ln */
  CHK_ENTITY_3RD (dim_ang2ln, DIMENSION_ANG2LN, xline1start_pt);
#ifdef USE_DEPRECATED_API
  dwg_ent_dim_ang2ln_get_13_pt (dim_ang2ln, &pt3d, &error);
  if (error || memcmp (&xline1start_pt, &pt3d, sizeof (xline1start_pt)))
    fail ("old API dwg_ent_dim_ang2ln_get_13_pt");
  else
    pass ();
#endif
  CHK_ENTITY_3RD (dim_ang2ln, DIMENSION_ANG2LN, xline1end_pt);
#ifdef USE_DEPRECATED_API
  dwg_ent_dim_ang2ln_get_14_pt (dim_ang2ln, &pt3d, &error);
  if (error || memcmp (&xline1end_pt, &pt3d, sizeof (xline1end_pt)))
    fail ("old API dwg_ent_dim_ang2ln_get_14_pt");
  else
    pass ();
#endif
  CHK_ENTITY_3RD (dim_ang2ln, DIMENSION_ANG2LN, xline2start_pt);
  /*
  dwg_ent_dim_ang2ln_get_15_pt (dim_ang2ln, &pt3d, &error);
  if (error || memcmp (&xline2start_pt, &pt3d, sizeof (xline2start_pt)))
    fail ("old API dwg_ent_dim_ang2ln_get_15_pt");
  else
    pass ();
  */
  CHK_ENTITY_3RD (dim_ang2ln, DIMENSION_ANG2LN, xline2end_pt);
#ifdef USE_DEPRECATED_API
  dwg_ent_dim_ang2ln_get_16_pt (dim_ang2ln, &pt3d, &error);
  if (error || memcmp (&xline2end_pt, &pt3d, sizeof (xline2end_pt)))
    fail ("old API dwg_ent_dim_ang2ln_get_16_pt");
  else
    pass ();
#endif

  CHK_ENTITY_H (dim_ang2ln, DIMENSION_ANG2LN, dimstyle);
  CHK_ENTITY_H (dim_ang2ln, DIMENSION_ANG2LN, block);
}
