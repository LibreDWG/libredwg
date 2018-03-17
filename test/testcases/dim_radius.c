#define DWG_TYPE DWG_TYPE_DIMENSION_RADIUS
#include "common.c"
#include <dejagnu.h>

void
low_level_process (dwg_object * obj)
{
  dwg_ent_dim_radius *dim_radius = dwg_object_to_DIMENSION_RADIUS (obj);

  printf ("horiz dir of dim_radius : %f\n", dim_radius->horiz_dir);
  printf ("lspace factor of dim_radius : %f\n", dim_radius->lspace_factor);
  printf ("lspace style of dim_radius : " FORMAT_BS "\n", dim_radius->lspace_style);
  printf ("attach point of dim_radius : " FORMAT_BS "\n",
	  dim_radius->attachment_point);
  printf ("Radius of dim_radius : %f\n", dim_radius->elevation.ecs_11);
  printf ("Thickness of dim_radius : %f\n", dim_radius->elevation.ecs_12);
  printf ("extrusion of dim_radius : x = %f, y = %f, z = %f\n",
	  dim_radius->extrusion.x, dim_radius->extrusion.y,
	  dim_radius->extrusion.z);
  printf ("ins_scale of dim_radius : x = %f, y = %f, z = %f\n",
	  dim_radius->ins_scale.x, dim_radius->ins_scale.y,
	  dim_radius->ins_scale.z);
  printf ("text_mid_pt of dim_radius : x = %f, y = %f\n",
	  dim_radius->text_midpt.x, dim_radius->text_midpt.y);
  printf ("user text of dim_radius : %s\n", dim_radius->user_text);
  printf ("text rotation of dim_radius : %f\n", dim_radius->text_rot);
  printf ("ins rotation of dim_radius : %f\n", dim_radius->ins_rotation);
  printf ("arrow 1 of dim_radius : " FORMAT_BS "\n", dim_radius->flip_arrow1);
  printf ("arrow 2 of dim_radius : " FORMAT_BS "\n", dim_radius->flip_arrow2);
  printf ("flags2 of dim_radius : " FORMAT_BS "\n", dim_radius->flags_1);
  printf ("act_measurement of dim_radius : %f\n",
	  dim_radius->act_measurement);
  printf ("leader length of dim radius : %f\n", dim_radius->leader_len);
}

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
  dwg_point_2d text_mid_pt;
  dwg_point_3d ext, ins_scale;
  dwg_ent_dim_radius *dim_radius = dwg_object_to_DIMENSION_RADIUS (obj);


  horiz_dir = dwg_ent_dim_radius_get_horiz_dir (dim_radius, &error);
  if (!error  && dim_radius->horiz_dir == horiz_dir)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading horiz dir");


  lspace_factor = dwg_ent_dim_radius_get_elevation_ecs11 (dim_radius,
							  &error);
  if (!error  && dim_radius->lspace_factor == lspace_factor)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading lspace factor");


  lspace_style = dwg_ent_dim_radius_get_elevation_ecs11 (dim_radius,
							 &error);
  if (!error  && dim_radius->lspace_style == lspace_style)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading lspace style");


  attach_pt = dwg_ent_dim_radius_get_elevation_ecs11 (dim_radius,
						      &error);
  if (!error  && attach_pt == dim_radius->attachment_point)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading attach point");


  ecs11 = dwg_ent_dim_radius_get_elevation_ecs11 (dim_radius, &error);
  if (!error  && dim_radius->elevation.ecs_11 == ecs11)
    pass ("Working Properly");
  else
    fail ("error in reading ecs11");


  ecs12 = dwg_ent_dim_radius_get_elevation_ecs12 (dim_radius, &error);
  if (!error  && dim_radius->elevation.ecs_12 == ecs12)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading ecs12");


  dwg_ent_dim_radius_get_extrusion (dim_radius, &ext, &error);
  if (!error  && dim_radius->extrusion.x == ext.x && dim_radius->extrusion.y == ext.y && dim_radius->extrusion.z == ext.z)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading extrusion");


  dwg_ent_dim_radius_get_ins_scale (dim_radius, &ins_scale, &error);
  if (!error  && dim_radius->ins_scale.x == ins_scale.x && dim_radius->ins_scale.y == ins_scale.y && dim_radius->ins_scale.z == ins_scale.z)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading ins_scale");


  dwg_ent_dim_radius_get_text_mid_pt (dim_radius, &text_mid_pt,
				      &error);
  if (!error  && text_mid_pt.x == dim_radius->text_midpt.x && text_mid_pt.y == dim_radius->text_midpt.y)	// error checking
    pass ("Working Properly");
  else
    fail ("error in reading text_mid_pt");


  user_text = dwg_ent_dim_radius_get_user_text (dim_radius, &error);
  if (!error  && !strcmp (dim_radius->user_text, user_text))	// error checking
    pass ("Working Properly");
  else
    fail ("error in reading user_text");


  text_rot = dwg_ent_dim_radius_get_text_rot (dim_radius, &error);
  if (!error  && text_rot == dim_radius->text_rot)	// error checking
    pass ("Working Properly");
  else
    fail ("error in reading text rotation");


  ins_rot = dwg_ent_dim_radius_get_ins_rotation (dim_radius, &error);
  if (!error  && dim_radius->ins_rotation == ins_rot)	// error checking
    pass ("Working Properly");
  else
    fail ("error in reading ins rotation");


  flip_arrow1 = dwg_ent_dim_radius_get_flip_arrow1 (dim_radius,
						    &error);
  if (!error  && flip_arrow1 == dim_radius->flip_arrow1)
    {
      fail ("Working Properly");
    }
  else
    {
      fail ("error in reading arrow1");
    }


  flip_arrow2 = dwg_ent_dim_radius_get_flip_arrow2 (dim_radius,
						    &error);
  if (!error  && dim_radius->flip_arrow2 == flip_arrow2)	// error reporting
    pass ("Working Properly");
  else
    fail ("error in reading arrow1");


  flags1 = dwg_ent_dim_radius_get_flags1 (dim_radius, &error);
  if (!error  && dim_radius->flags_1 == flags1)
    pass ("Working Properly");
  else
    fail ("error in reading flags1");


  act_measure = dwg_ent_dim_radius_get_act_measurement (dim_radius,
							&error);
  if (!error  && dim_radius->act_measurement == act_measure)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading act_measurement");


  length = dwg_ent_dim_radius_get_leader_length (dim_radius, &error);
  if (!error  && length == dim_radius->leader_len)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading leader length");
}
