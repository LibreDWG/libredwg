#include "common.c"
#include <dejagnu.h>

void output_process (dwg_object * obj);

void
output_object (dwg_object * obj)
{
  if (!obj)
    {
      printf ("object is NULL\n");
      return;
    }

  if (dwg_get_type (obj) == DWG_TYPE_DIMENSION_DIAMETER)
    {
      output_process (obj);
    }
}

/* This function checks API functions for integrity
   @params dwg_object* obj 
 */
void
api_process (dwg_object * obj)
{
  int ecs11_error, ecs12_error, flags1_error, act_error, horiz_error,
    lspace_error, style_error, att_point_error, ext_error, user_text_error,
    text_rot_error, ins_rot_error, arrow1_error, arrow2_error, mid_pt_error,
    ins_scale_error, flags2_error, pt10_error, pt12_error, dim_rot_error,
    ext_line_rot_error, pt15_error, length_error;
  double ecs11, ecs12, act_measure, horiz_dir, lspace_factor, text_rot,
    ins_rot, ext_line_rot, dim_rot, length;
  unsigned int flags1, lspace_style, attach_pt, flip_arrow1, flip_arrow2,
    flags2;
  char *user_text;
  dwg_point_2d text_mid_pt, pt12, pt16;
  dwg_point_3d pt10, pt15, ext, ins_scale;
  dwg_ent_dim_diameter *dim_diameter = dwg_object_to_DIMENSION_DIAMETER (obj);

  // returns horiz dir
  horiz_dir = dwg_ent_dim_diameter_get_horiz_dir (dim_diameter, &horiz_error);
  if (horiz_error == 0 && dim_diameter->horiz_dir == horiz_dir)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading horiz dir");
    }

  // returns lspace factor
  lspace_factor = dwg_ent_dim_diameter_get_elevation_ecs11 (dim_diameter,
							    &lspace_error);
  if (lspace_error == 0 && dim_diameter->lspace_factor == lspace_factor)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading lspace factor");
    }

  // returns lspace style
  lspace_style = dwg_ent_dim_diameter_get_elevation_ecs11 (dim_diameter,
							   &style_error);
  if (style_error == 0 && dim_diameter->lspace_style == lspace_style)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading lspace style");
    }

  // returns attachment point
  attach_pt = dwg_ent_dim_diameter_get_elevation_ecs11 (dim_diameter,
							&att_point_error);
  if (att_point_error == 0 && attach_pt == dim_diameter->attachment_point)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading attach point");
    }

  // returns ecs11 elevation
  ecs11 =
    dwg_ent_dim_diameter_get_elevation_ecs11 (dim_diameter, &ecs11_error);
  if (ecs11_error == 0 && dim_diameter->elevation.ecs_11 == ecs11)
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading ecs11");
    }

  // returns ecs12 elevation
  ecs12 =
    dwg_ent_dim_diameter_get_elevation_ecs12 (dim_diameter, &ecs12_error);
  if (ecs12_error == 0 && dim_diameter->elevation.ecs_12 == ecs12)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading ecs12");
    }

  // returns extrusion
  dwg_ent_dim_diameter_get_extrusion (dim_diameter, &ext, &ext_error);
  if (ext_error == 0 && dim_diameter->extrusion.x == ext.x && dim_diameter->extrusion.y == ext.y && dim_diameter->extrusion.z == ext.z)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading extrusion");
    }

  // returns insert scale
  dwg_ent_dim_diameter_get_ins_scale (dim_diameter, &ins_scale,
				      &ins_scale_error);
  if (ins_scale_error == 0 && dim_diameter->ins_scale.x == ins_scale.x && dim_diameter->ins_scale.y == ins_scale.y && dim_diameter->ins_scale.z == ins_scale.z)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading ins_scale");
    }

  // returns 10 point
  dwg_ent_dim_diameter_get_10_pt (dim_diameter, &pt10, &pt10_error);
  if (pt10_error == 0 && dim_diameter->_10_pt.x == pt10.x && dim_diameter->_10_pt.y == pt10.y && dim_diameter->_10_pt.z == pt10.z)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading pt10");
    }

  // returns 15 point
  dwg_ent_dim_diameter_get_15_pt (dim_diameter, &pt15, &pt15_error);
  if (pt15_error == 0 && dim_diameter->_15_pt.x == pt15.x && dim_diameter->_15_pt.y == pt15.y && dim_diameter->_15_pt.z == pt15.z)	// error checking
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading pt14");
    }

  // return point 12
  dwg_ent_dim_diameter_get_12_pt (dim_diameter, &pt12, &pt12_error);
  if (pt12_error == 0 && dim_diameter->_12_pt.x == pt12.x && dim_diameter->_12_pt.y == pt12.y)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading pt12");
    }
  // returns text mid point
  dwg_ent_dim_diameter_get_text_mid_pt (dim_diameter, &text_mid_pt,
					&mid_pt_error);
  if (mid_pt_error == 0 && text_mid_pt.x == dim_diameter->text_midpt.x && text_mid_pt.y == dim_diameter->text_midpt.y)	// error checking
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading text_mid_pt");
    }

  // returns user text
  user_text =
    dwg_ent_dim_diameter_get_user_text (dim_diameter, &user_text_error);
  if (user_text_error == 0 && !strcmp (dim_diameter->user_text, user_text))	// error checking
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading user_text");
    }

  // returns text rotation
  text_rot =
    dwg_ent_dim_diameter_get_text_rot (dim_diameter, &text_rot_error);
  if (text_rot_error == 0 && text_rot == dim_diameter->text_rot)	// error checking
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading text rotation");
    }

  // returns insert rotation
  ins_rot =
    dwg_ent_dim_diameter_get_ins_rotation (dim_diameter, &ins_rot_error);
  if (ins_rot_error == 0 && dim_diameter->ins_rotation == ins_rot)	// error checking
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading ins rotation");
    }

  // returns flip arrow 1
  flip_arrow1 = dwg_ent_dim_diameter_get_flip_arrow1 (dim_diameter,
						      &arrow1_error);
  if (arrow1_error == 0 && flip_arrow1 == dim_diameter->flip_arrow1)
    {
      fail ("Working Properly");
    }
  else
    {
      fail ("error in reading arrow1");
    }

  // returns flip arrow 2
  flip_arrow2 = dwg_ent_dim_diameter_get_flip_arrow2 (dim_diameter,
						      &arrow2_error);
  if (arrow2_error == 0 && dim_diameter->flip_arrow2 == flip_arrow2)	// error reporting
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading arrow1");
    }

  // returns flags 1
  flags1 = dwg_ent_dim_diameter_get_flags1 (dim_diameter, &flags1_error);
  if (flags1_error == 0 && dim_diameter->flags_1 == flags1)
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading flags1");
    }

  // returns actual measurement
  act_measure = dwg_ent_dim_diameter_get_act_measurement (dim_diameter,
							  &act_error);
  if (act_error == 0 && dim_diameter->act_measurement == act_measure)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading act_measurement");
    }

  // returns leader length
  length =
    dwg_ent_dim_diameter_get_leader_length (dim_diameter, &length_error);
  if (length_error == 0 && length == dim_diameter->leader_len)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading leader length");
    }
}
