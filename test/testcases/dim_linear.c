#define DWG_TYPE DWG_TYPE_DIMENSION_LINEAR
#include "common.c"
#include <dejagnu.h>

/* This function checks API functions for integrity
   @params dwg_object* obj 
 */
void
api_process (dwg_object * obj)
{
  int error;
  double ecs11, ecs12, act_measure, horiz_dir, lspace_factor, text_rot,
    ins_rot, ext_line_rot, dim_rot;
  BITCODE_RC flags1, flags2;
  BITCODE_BS lspace_style, attach_pt;
  BITCODE_B flip_arrow1, flip_arrow2;
  char *user_text;
  dwg_point_2d text_mid_pt, pt12;
  dwg_point_3d pt10, pt13, pt14, ext, ins_scale;
  dwg_ent_dim_linear *dim_linear = dwg_object_to_DIMENSION_LINEAR (obj);


  horiz_dir = dwg_ent_dim_linear_get_horiz_dir (dim_linear, &error);
  if (!error  && dim_linear->horiz_dir == horiz_dir)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading horiz dir");

  lspace_factor = dwg_ent_dim_linear_get_elevation_ecs11 (dim_linear, &error);
  if (!error  && dim_linear->lspace_factor == lspace_factor)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading lspace factor");

  lspace_style = dwg_ent_dim_linear_get_elevation_ecs11 (dim_linear, &error);
  if (!error  && dim_linear->lspace_style == lspace_style)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading lspace style");


  attach_pt = dwg_ent_dim_linear_get_elevation_ecs11 (dim_linear, &error);
  if (!error  && attach_pt == dim_linear->attachment_point)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading attach point");

  ecs11 = dwg_ent_dim_linear_get_elevation_ecs11 (dim_linear, &error);
  if (!error  && dim_linear->elevation.ecs_11 == ecs11)
    pass ("Working Properly");
  else
    fail ("error in reading ecs11");

  ecs12 = dwg_ent_dim_linear_get_elevation_ecs12 (dim_linear, &error);
  if (!error  && dim_linear->elevation.ecs_12 == ecs12)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading ecs12");


  dwg_ent_dim_linear_get_extrusion (dim_linear, &ext, &error);
  if (!error  && dim_linear->extrusion.x == ext.x && dim_linear->extrusion.y == ext.y && dim_linear->extrusion.z == ext.z)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading extrusion");

  dwg_ent_dim_linear_get_ins_scale (dim_linear, &ins_scale, &error);
  if (!error  && dim_linear->ins_scale.x == ins_scale.x && dim_linear->ins_scale.y == ins_scale.y && dim_linear->ins_scale.z == ins_scale.z)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading ins_scale");

  dwg_ent_dim_linear_get_10_pt (dim_linear, &pt10, &error);
  if (!error  && dim_linear->_10_pt.x == pt10.x && dim_linear->_10_pt.y == pt10.y && dim_linear->_10_pt.z == pt10.z)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading pt10");

  dwg_ent_dim_linear_get_13_pt (dim_linear, &pt13, &error);
  if (!error  && dim_linear->_13_pt.x == pt13.x && dim_linear->_13_pt.y == pt13.y && dim_linear->_13_pt.z == pt13.z)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading pt13");

  dwg_ent_dim_linear_get_14_pt (dim_linear, &pt14, &error);
  if (!error  && dim_linear->_14_pt.x == pt14.x && dim_linear->_14_pt.y == pt14.y && dim_linear->_14_pt.z == pt14.z)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading pt14");

  dwg_ent_dim_linear_get_12_pt (dim_linear, &pt12, &error);
  if (!error  && dim_linear->_12_pt.x == pt12.x && dim_linear->_12_pt.y == pt12.y)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading pt12");

  dwg_ent_dim_linear_get_text_mid_pt (dim_linear, &text_mid_pt, &error);
  if (!error  && text_mid_pt.x == dim_linear->text_midpt.x && text_mid_pt.y == dim_linear->text_midpt.y)	// error checking
    pass ("Working Properly");
  else
    fail ("error in reading text_mid_pt");


  user_text = dwg_ent_dim_linear_get_user_text (dim_linear, &error);
  if (!error  && !strcmp (dim_linear->user_text, user_text))	// error checking
    pass ("Working Properly");
  else
    fail ("error in reading user_text");


  text_rot = dwg_ent_dim_linear_get_text_rot (dim_linear, &error);
  if (!error  && text_rot == dim_linear->text_rot)	// error checking
    pass ("Working Properly");
  else
    fail ("error in reading text rotation");


  ins_rot = dwg_ent_dim_linear_get_ins_rotation (dim_linear, &error);
  if (!error  && dim_linear->ins_rotation == ins_rot)	// error checking
    pass ("Working Properly");
  else
    fail ("error in reading ins rotation");

  flip_arrow1 = dwg_ent_dim_linear_get_flip_arrow1 (dim_linear, &error);
  if (!error  && flip_arrow1 == dim_linear->flip_arrow1)
      pass ("Working Properly");
  else
      fail ("error in reading arrow1");


  flip_arrow2 = dwg_ent_dim_linear_get_flip_arrow2 (dim_linear, &error);
  if (!error  && dim_linear->flip_arrow2 == flip_arrow2)	// error reporting
    pass ("Working Properly");
  else
    fail ("error in reading arrow1");

  flags1 = dwg_ent_dim_linear_get_flags1 (dim_linear, &error);
  if (!error  && dim_linear->flags_1 == flags1)
    pass ("Working Properly");
  else
    fail ("error in reading flags1");

  act_measure = dwg_ent_dim_linear_get_act_measurement (dim_linear, &error);
  if (!error  && dim_linear->act_measurement == act_measure)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading act_measurement");

  ext_line_rot = dwg_ent_dim_linear_get_ext_line_rotation (dim_linear, &error);
  if (!error  && ext_line_rot == dim_linear->ext_line_rot)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading ext line rot");

  dim_rot = dwg_ent_dim_linear_get_dim_rot (dim_linear, &error);
  if (!error  && dim_linear->dim_rot == dim_rot)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading dim rot \n");
}
