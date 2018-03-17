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
  double ecs11, ecs12, act_measure, horiz_dir, lspace_factor, text_rot,
    ins_rot, ext_line_rot, dim_rot, length;
  BITCODE_RC flags1, flags2;
  BITCODE_BS lspace_style, attach_pt;
  BITCODE_B flip_arrow1, flip_arrow2;
  char *user_text;
  dwg_point_2d text_mid_pt, pt12, pt16;
  dwg_point_3d pt10, pt15, ext, ins_scale;
  dwg_ent_dim_diameter *dim_diameter = dwg_object_to_DIMENSION_DIAMETER (obj);


  horiz_dir = dwg_ent_dim_diameter_get_horiz_dir (dim_diameter, &error);
  if (!error  && dim_diameter->horiz_dir == horiz_dir)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading horiz dir");


  lspace_factor = dwg_ent_dim_diameter_get_elevation_ecs11 (dim_diameter,
							    &error);
  if (!error  && dim_diameter->lspace_factor == lspace_factor)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading lspace factor");


  lspace_style = dwg_ent_dim_diameter_get_elevation_ecs11 (dim_diameter,
							   &error);
  if (!error  && dim_diameter->lspace_style == lspace_style)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading lspace style");


  attach_pt = dwg_ent_dim_diameter_get_elevation_ecs11 (dim_diameter,
							&error);
  if (!error  && attach_pt == dim_diameter->attachment_point)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading attach point");


  ecs11 =
    dwg_ent_dim_diameter_get_elevation_ecs11 (dim_diameter, &error);
  if (!error  && dim_diameter->elevation.ecs_11 == ecs11)
    pass ("Working Properly");
  else
    fail ("error in reading ecs11");


  ecs12 =
    dwg_ent_dim_diameter_get_elevation_ecs12 (dim_diameter, &error);
  if (!error  && dim_diameter->elevation.ecs_12 == ecs12)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading ecs12");


  dwg_ent_dim_diameter_get_extrusion (dim_diameter, &ext, &error);
  if (!error  && dim_diameter->extrusion.x == ext.x && dim_diameter->extrusion.y == ext.y && dim_diameter->extrusion.z == ext.z)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading extrusion");


  dwg_ent_dim_diameter_get_ins_scale (dim_diameter, &ins_scale,
				      &error);
  if (!error  && dim_diameter->ins_scale.x == ins_scale.x && dim_diameter->ins_scale.y == ins_scale.y && dim_diameter->ins_scale.z == ins_scale.z)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading ins_scale");


  dwg_ent_dim_diameter_get_10_pt (dim_diameter, &pt10, &error);
  if (!error  && dim_diameter->_10_pt.x == pt10.x && dim_diameter->_10_pt.y == pt10.y && dim_diameter->_10_pt.z == pt10.z)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading pt10");


  dwg_ent_dim_diameter_get_15_pt (dim_diameter, &pt15, &error);
  if (!error  && dim_diameter->_15_pt.x == pt15.x && dim_diameter->_15_pt.y == pt15.y && dim_diameter->_15_pt.z == pt15.z)	// error checking
    pass ("Working Properly");
  else
    fail ("error in reading pt14");

  // return point 12
  dwg_ent_dim_diameter_get_12_pt (dim_diameter, &pt12, &error);
  if (!error  && dim_diameter->_12_pt.x == pt12.x && dim_diameter->_12_pt.y == pt12.y)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading pt12");

  dwg_ent_dim_diameter_get_text_mid_pt (dim_diameter, &text_mid_pt,
					&error);
  if (!error  && text_mid_pt.x == dim_diameter->text_midpt.x && text_mid_pt.y == dim_diameter->text_midpt.y)	// error checking
    pass ("Working Properly");
  else
    fail ("error in reading text_mid_pt");


  user_text =
    dwg_ent_dim_diameter_get_user_text (dim_diameter, &error);
  if (!error  && !strcmp (dim_diameter->user_text, user_text))	// error checking
    pass ("Working Properly");
  else
    fail ("error in reading user_text");


  text_rot =
    dwg_ent_dim_diameter_get_text_rot (dim_diameter, &error);
  if (!error  && text_rot == dim_diameter->text_rot)	// error checking
    pass ("Working Properly");
  else
    fail ("error in reading text rotation");


  ins_rot =
    dwg_ent_dim_diameter_get_ins_rotation (dim_diameter, &error);
  if (!error  && dim_diameter->ins_rotation == ins_rot)	// error checking
    pass ("Working Properly");
  else
    fail ("error in reading ins rotation");


  flip_arrow1 = dwg_ent_dim_diameter_get_flip_arrow1 (dim_diameter,
						      &error);
  if (!error  && flip_arrow1 == dim_diameter->flip_arrow1)
    {
      fail ("Working Properly");
    }
  else
    {
      fail ("error in reading arrow1");
    }


  flip_arrow2 = dwg_ent_dim_diameter_get_flip_arrow2 (dim_diameter,
						      &error);
  if (!error  && dim_diameter->flip_arrow2 == flip_arrow2)	// error reporting
    pass ("Working Properly");
  else
    fail ("error in reading arrow1");


  flags1 = dwg_ent_dim_diameter_get_flags1 (dim_diameter, &error);
  if (!error  && dim_diameter->flags_1 == flags1)
    pass ("Working Properly");
  else
    fail ("error in reading flags1");


  act_measure = dwg_ent_dim_diameter_get_act_measurement (dim_diameter,
							  &error);
  if (!error  && dim_diameter->act_measurement == act_measure)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading act_measurement");


  length =
    dwg_ent_dim_diameter_get_leader_length (dim_diameter, &error);
  if (!error  && length == dim_diameter->leader_len)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading leader length");
}
