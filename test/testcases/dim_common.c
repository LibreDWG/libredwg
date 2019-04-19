#define DWG_TYPE DWG_TYPE_DIMENSION_ALIGNED
#include "common.c"
#include <dejagnu.h>

/* This function checks API functions for integrity
   @params dwg_object* obj
 */
void
api_process (dwg_object *obj)
{
  int error;
  double elevation, act_measure, horiz_dir, lspace_factor, text_rot, ins_rot;
  BITCODE_RC flag1;
  BITCODE_BS lspace_style, attachment;
  BITCODE_B flip_arrow1, flip_arrow2;
  char *user_text;
  dwg_point_2d text_mid_pt, pt12;
  dwg_point_3d extrusion, ins_scale;
  dwg_ent_dim *dim = dwg_object_to_DIMENSION (obj);

  horiz_dir = dwg_ent_dim_get_horiz_dir (dim, &error);
  if (!error && horiz_dir == dim->horiz_dir)
    pass ("Working Properly");
  else
    fail ("error in reading horiz dir");

  lspace_factor = dwg_ent_dim_get_lspace_factor (dim, &error);
  if (!error && dim->lspace_factor == lspace_factor) // error check
    pass ("Working Properly");
  else
    fail ("error in reading lspace factor");

  lspace_style = dwg_ent_dim_get_lspace_style (dim, &error);
  if (!error && lspace_style == dim->lspace_style)
    pass ("Working Properly");
  else
    fail ("error in reading lspace style");

  attachment = dwg_ent_dim_get_attachment (dim, &error);
  if (!error && attachment == dim->attachment)
    pass ("Working Properly");
  else
    fail ("error in reading attach point");

  elevation = dwg_ent_dim_get_elevation (dim, &error);
  if (!error && elevation == dim->elevation)
    pass ("Working Properly");
  else
    fail ("error in reading elevation");

  dwg_ent_dim_get_extrusion (dim, &extrusion, &error);
  if (!error && extrusion.x == dim->extrusion.x
      && extrusion.y == dim->extrusion.y && extrusion.z == dim->extrusion.z)
    pass ("Working Properly");
  else
    fail ("error in reading extrusion");

  dwg_ent_dim_get_ins_scale (dim, &ins_scale, &error);
  if (!error && dim->ins_scale.x == ins_scale.x
      && dim->ins_scale.y == ins_scale.y && dim->ins_scale.z == ins_scale.z)
    pass ("Working Properly");
  else
    fail ("error in reading ins_scale");

  dwg_ent_dim_get_clone_ins_pt (dim, &pt12, &error);
  if (!error && dim->clone_ins_pt.x == pt12.x && dim->clone_ins_pt.y == pt12.y)
    pass ("Working Properly");
  else
    fail ("error in reading clone_ins_pt");

  dwg_ent_dim_get_text_midpt (dim, &text_mid_pt, &error);
  if (!error && text_mid_pt.x == dim->text_midpt.x
      && text_mid_pt.y == dim->text_midpt.y)
    pass ("Working Properly");
  else
    fail ("error in reading text_mid_pt");

  user_text = dwg_ent_dim_get_user_text (dim, &error);
  if (!error && dim->user_text == user_text)
    pass ("Working Properly");
  else
    fail ("error in reading user_text");

  text_rot = dwg_ent_dim_get_text_rotation (dim, &error);
  if (!error && text_rot == dim->text_rotation)
    pass ("Working Properly");
  else
    fail ("error in reading text rotation");

  ins_rot = dwg_ent_dim_get_ins_rotation (dim, &error);
  if (!error && ins_rot == dim->ins_rotation) // error check
    pass ("Working Properly");
  else
    fail ("error in reading ins rotation");

  flip_arrow1 = dwg_ent_dim_get_flip_arrow1 (dim, &error);
  if (!error && flip_arrow1 == dim->flip_arrow1) // error check
    pass ("Working Properly");
  else
    fail ("error in reading arrow1");

  flip_arrow2 = dwg_ent_dim_get_flip_arrow2 (dim, &error);
  if (!error && flip_arrow2 == dim->flip_arrow2)
    pass ("Working Properly");
  else
    fail ("error in reading arrow1 \n");

  flag1 = dwg_ent_dim_get_flag1 (dim, &error);
  if (!error && dim->flag1 == flag1)
    pass ("Working Properly");
  else
    fail ("error in reading flags1");

  act_measure = dwg_ent_dim_get_act_measurement (dim, &error);
  if (!error && act_measure == dim->act_measurement)
    pass ("Working Properly");
  else
    fail ("error in reading act_measurement");
}
