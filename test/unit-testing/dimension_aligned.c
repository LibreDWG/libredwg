#define DWG_TYPE DWG_TYPE_DIMENSION_ALIGNED
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  double elevation, act_measurement, horiz_dir, lspace_factor, text_rotation,
      ins_rotation, oblique_angle;
  BITCODE_B flip_arrow1, flip_arrow2;
  BITCODE_RC class_version, flag, flag1;
  BITCODE_BS lspace_style, attachment;
  char *user_text;
  dwg_point_2d text_midpt, clone_ins_pt, pt2d;
  dwg_point_3d def_pt, xline1_pt, xline2_pt, ext, ins_scale, pt3d;
  BITCODE_H dimstyle, block;
  int isnew;

  dwg_ent_dim_aligned *dim_aligned = dwg_object_to_DIMENSION_ALIGNED (obj);
  dwg_ent_dim *dim = dwg_object_to_DIMENSION (obj);

  /* common */
  CHK_ENTITY_TYPE (dim, DIMENSION_ALIGNED, class_version, RC, class_version);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_ALIGNED, lspace_style, BS,
                         lspace_style);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_ALIGNED, lspace_factor, BD,
                         lspace_factor);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_ALIGNED, act_measurement, BD,
                         act_measurement);
  CHK_ENTITY_TYPE (dim, DIMENSION_ALIGNED, attachment, BS, attachment);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_ALIGNED, elevation, BD, elevation);
  CHK_ENTITY_3RD (dim, DIMENSION_ALIGNED, extrusion, ext);
  CHK_ENTITY_2RD_W_OLD (dim, DIMENSION_ALIGNED, clone_ins_pt, clone_ins_pt);
  CHK_ENTITY_2RD_W_OLD (dim, DIMENSION_ALIGNED, text_midpt, text_midpt);
  CHK_ENTITY_UTF8TEXT (dim, DIMENSION_ALIGNED, user_text, user_text);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_ALIGNED, text_rotation, BD,
                         text_rotation);
  CHK_ENTITY_3RD_W_OLD (dim, DIMENSION_ALIGNED, ins_scale, ins_scale);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_ALIGNED, ins_rotation, BD,
                         ins_rotation);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_ALIGNED, horiz_dir, BD, horiz_dir);
  CHK_ENTITY_MAX (dim, DIMENSION_ALIGNED, text_rotation, BD, MAX_ANGLE);
  CHK_ENTITY_MAX (dim, DIMENSION_ALIGNED, ins_rotation, BD, MAX_ANGLE);
  CHK_ENTITY_MAX (dim, DIMENSION_ALIGNED, horiz_dir, BD, MAX_ANGLE);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_ALIGNED, flip_arrow1, B, flip_arrow1);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_ALIGNED, flip_arrow2, B, flip_arrow2);
  CHK_ENTITY_TYPE_W_OLD (dim, DIMENSION_ALIGNED, flag1, RC, flag1);
  CHK_ENTITY_TYPE (dim, DIMENSION_ALIGNED, flag, RC, flag);

  /* aligned */
  CHK_ENTITY_3RD_W_OLD (dim_aligned, DIMENSION_ALIGNED, def_pt, def_pt);
  CHK_ENTITY_3RD (dim_aligned, DIMENSION_ALIGNED, xline1_pt, xline1_pt);
  dwg_ent_dim_aligned_get_13_pt (dim_aligned, &pt3d, &error);
  if (error || memcmp (&xline1_pt, &pt3d, sizeof (xline1_pt)))
    fail ("old API dwg_ent_dim_aligned_get_13_pt");
  else
    pass ();

  CHK_ENTITY_3RD (dim_aligned, DIMENSION_ALIGNED, xline2_pt, xline2_pt);
  dwg_ent_dim_aligned_get_14_pt (dim_aligned, &pt3d, &error);
  if (error || memcmp (&xline2_pt, &pt3d, sizeof (xline2_pt)))
    fail ("old API dwg_ent_dim_aligned_get_14_pt");
  else
    pass ();

  CHK_ENTITY_TYPE (dim_aligned, DIMENSION_ALIGNED, oblique_angle, BD,
                   oblique_angle);

  CHK_ENTITY_H (dim_aligned, DIMENSION_ALIGNED, dimstyle, dimstyle);
  CHK_ENTITY_H (dim_aligned, DIMENSION_ALIGNED, block, block);
}
