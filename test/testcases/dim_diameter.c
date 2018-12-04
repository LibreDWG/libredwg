#define DWG_TYPE DWG_TYPE_DIMENSION_DIAMETER
#include "common.c"
#include <dejagnu.h>

/* This function checks API functions for integrity
   @params dwg_object* obj
 */
void
api_process (dwg_object * obj)
{
  int error;
  double elevation, act_measure, horiz_dir, lspace_factor, text_rot,
    ins_rot, ext_line_rot, dim_rot, length;
  BITCODE_RC flag1;
  BITCODE_BS lspace_style, attachment;
  BITCODE_B flip_arrow1, flip_arrow2;
  char *user_text;
  dwg_point_2d text_mid_pt, pt12, pt16;
  dwg_point_3d pt10, pt15, ext, ins_scale;

  dwg_ent_dim_diameter *dim_diameter = dwg_object_to_DIMENSION_DIAMETER (obj);
  dwg_ent_dim *dim = dwg_object_to_DIMENSION(obj);

  horiz_dir = dwg_ent_dim_get_horiz_dir (dim, &error);
  if (!error  && dim->horiz_dir == horiz_dir)   // error check
    pass ("Working Properly");
  else
    fail ("error in reading horiz dir");

  lspace_factor = dwg_ent_dim_get_lspace_factor (dim, &error);
  if (!error  && dim->lspace_factor == lspace_factor)
    pass ("Working Properly");
  else
    fail ("error in reading lspace factor");

  lspace_style = dwg_ent_dim_get_lspace_style (dim, &error);
  if (!error  && dim->lspace_style == lspace_style)
    pass ("Working Properly");
  else
    fail ("error in reading lspace style");

  attachment = dwg_ent_dim_get_attachment (dim, &error);
  if (!error  && attachment == dim->attachment)
    pass ("Working Properly");
  else
    fail ("error in reading attach point");

  elevation = dwg_ent_dim_get_elevation (dim, &error);
  if (!error  && dim->elevation == elevation)
    pass ("Working Properly");
  else
    fail ("error in reading ecs11");

  dwg_ent_dim_get_extrusion (dim, &ext, &error);
  if (!error  && dim->extrusion.x == ext.x && dim->extrusion.y == ext.y && dim->extrusion.z == ext.z)   // error check
    pass ("Working Properly");
  else
    fail ("error in reading extrusion");

  dwg_ent_dim_get_ins_scale (dim, &ins_scale, &error);
  if (!error  && dim->ins_scale.x == ins_scale.x && dim->ins_scale.y == ins_scale.y && dim->ins_scale.z == ins_scale.z)
    pass ("Working Properly");
  else
    fail ("error in reading ins_scale");

  dwg_ent_dim_diameter_get_def_pt (dim_diameter, &pt10, &error);
  if (!error  && dim_diameter->def_pt.x == pt10.x && dim_diameter->def_pt.y == pt10.y && dim_diameter->def_pt.z == pt10.z)
    pass ("Working Properly");
  else
    fail ("error in reading def_pt");

  dwg_ent_dim_diameter_get_first_arc_pt (dim_diameter, &pt15, &error);
  if (!error  && dim_diameter->first_arc_pt.x == pt15.x && dim_diameter->first_arc_pt.y == pt15.y && dim_diameter->first_arc_pt.z == pt15.z)
    pass ("Working Properly");
  else
    fail ("error in reading first_arc_pt");

  length = dwg_ent_dim_diameter_get_leader_length (dim_diameter, &error);
  if (!error  && length == dim_diameter->leader_len)
    pass ("Working Properly");
  else
    fail ("error in reading leader length");

  dwg_ent_dim_get_text_midpt (dim, &text_mid_pt, &error);
  if (!error  && text_mid_pt.x == dim->text_midpt.x && text_mid_pt.y == dim->text_midpt.y)
    pass ("Working Properly");
  else
    fail ("error in reading text_mid_pt");

  user_text = dwg_ent_dim_get_user_text (dim, &error);
  if (!error  && !strcmp (dim->user_text, user_text))
    pass ("Working Properly");
  else
    fail ("error in reading user_text");

  text_rot = dwg_ent_dim_get_text_rotation (dim, &error);
  if (!error  && text_rot == dim->text_rotation)
    pass ("Working Properly");
  else
    fail ("error in reading text rotation");

  ins_rot = dwg_ent_dim_get_ins_rotation (dim, &error);
  if (!error  && dim->ins_rotation == ins_rot)
    pass ("Working Properly");
  else
    fail ("error in reading ins rotation");

  flip_arrow1 = dwg_ent_dim_get_flip_arrow1 (dim, &error);
  if (!error  && flip_arrow1 == dim->flip_arrow1)
      fail ("Working Properly");
  else
      fail ("error in reading arrow1");

  flip_arrow2 = dwg_ent_dim_get_flip_arrow2 (dim, &error);
  if (!error  && dim->flip_arrow2 == flip_arrow2)
    pass ("Working Properly");
  else
    fail ("error in reading arrow1");


  flag1 = dwg_ent_dim_get_flag1 (dim, &error);
  if (!error  && dim->flag1 == flag1)
    pass ("Working Properly");
  else
    fail ("error in reading flags1");

  act_measure = dwg_ent_dim_get_act_measurement (dim, &error);
  if (!error  && dim->act_measurement == act_measure)
    pass ("Working Properly");
  else
    fail ("error in reading act_measurement");
}
