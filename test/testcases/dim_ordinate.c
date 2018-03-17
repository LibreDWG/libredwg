#define DWG_TYPE DWG_TYPE_DIMENSION_ORDINATE
#include "common.c"
#include <dejagnu.h>

void
low_level_process (dwg_object * obj)
{

  // casts dwg object to ordinate
  dwg_ent_dim_ordinate *dim_ordinate = dwg_object_to_DIMENSION_ORDINATE (obj);


  printf ("horiz dir of dim_ordinate : %f\n", dim_ordinate->horiz_dir);
  printf ("lspace factor of dim_ordinate : %f\n",
	  dim_ordinate->lspace_factor);
  printf ("lspace style of dim_ordinate : " FORMAT_BS "\n",
	  dim_ordinate->lspace_style);
  printf ("attach point of dim_ordinate : " FORMAT_BS "\n",
	  dim_ordinate->attachment_point);
  printf ("Radius of dim_ordinate : %f\n", dim_ordinate->elevation.ecs_11);
  printf ("Thickness of dim_ordinate : %f\n",
	  dim_ordinate->elevation.ecs_12);
  printf ("extrusion of dim_ordinate : x = %f, y = %f, z = %f\n",
	  dim_ordinate->extrusion.x, dim_ordinate->extrusion.y,
	  dim_ordinate->extrusion.z);
  printf ("ins_scale of dim_ordinate : x = %f, y = %f, z = %f\n",
	  dim_ordinate->ins_scale.x, dim_ordinate->ins_scale.y,
	  dim_ordinate->ins_scale.z);
  printf ("pt10 of dim_ordinate : x = %f, y = %f, z = %f\n",
	  dim_ordinate->_10_pt.x, dim_ordinate->_10_pt.y,
	  dim_ordinate->_10_pt.z);
  printf ("pt13 of dim_ordinate : x = %f, y = %f, z = %f\n",
	  dim_ordinate->_13_pt.x, dim_ordinate->_13_pt.y,
	  dim_ordinate->_13_pt.z);
  printf ("pt14 of dim_ordinate : x = %f, y = %f, z = %f\n",
	  dim_ordinate->_14_pt.x, dim_ordinate->_14_pt.y,
	  dim_ordinate->_14_pt.z);
  printf ("pt12 of dim_ordinate : x = %f, y = %f\n",
	  dim_ordinate->_12_pt.x, dim_ordinate->_12_pt.y);
  printf ("text_mid_pt of dim_ordinate : x = %f, y = %f\n",
	  dim_ordinate->text_midpt.x, dim_ordinate->text_midpt.y);
  printf ("user text of dim_ordinate : %s\n", dim_ordinate->user_text);
  printf ("text rotation of dim_ordinate : %f\n", dim_ordinate->text_rot);
  printf ("ins rotation of dim_ordinate : %f\n",
	  dim_ordinate->ins_rotation);
  printf ("arrow1 of dim_ordinate : " FORMAT_BS "\n", dim_ordinate->flip_arrow1);
  printf ("arrow1 of dim_ordinate : " FORMAT_BS "\n", dim_ordinate->flip_arrow2);
  printf ("flags2 of dim_ordinate : " FORMAT_BS "\n", dim_ordinate->flags_2);
  printf ("flags1 of dim_ordinate : " FORMAT_BS "\n", dim_ordinate->flags_1);
  printf ("act_measurement of dim_ordinate : %f\n",
	  dim_ordinate->act_measurement);

}

void
api_process (dwg_object * obj)
{
  int error;
  double ecs11, ecs12, act_measure, horiz_dir, lspace_factor, text_rot,
    ins_rot;
  BITCODE_RC flags1, flags2;
  BITCODE_BS lspace_style, attach_pt;
  BITCODE_B flip_arrow1, flip_arrow2;
  char *user_text;
  dwg_point_2d text_mid_pt, pt12;
  dwg_point_3d pt10, pt13, pt14, ext, ins_scale;
  dwg_ent_dim_ordinate *dim_ordinate = dwg_object_to_DIMENSION_ORDINATE (obj);


  horiz_dir = dwg_ent_dim_ordinate_get_horiz_dir (dim_ordinate, &error);
  if (!error  && horiz_dir == dim_ordinate->horiz_dir)
    pass ("Working Properly");
  else
    fail ("error in reading horiz dir");


  lspace_factor = dwg_ent_dim_ordinate_get_elevation_ecs11 (dim_ordinate,
							    &error);
  if (!error  && dim_ordinate->lspace_factor == lspace_factor)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading lspace factor");


  lspace_style = dwg_ent_dim_ordinate_get_elevation_ecs11 (dim_ordinate,
							   &error);
  if (!error  && lspace_style == dim_ordinate->lspace_style)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading lspace style");


  attach_pt = dwg_ent_dim_ordinate_get_elevation_ecs11 (dim_ordinate,
							&error);
  if (!error  && attach_pt == dim_ordinate->attachment_point)	// error checking
    pass ("Working Properly");
  else
    fail ("error in reading attach point");


  ecs11 =
    dwg_ent_dim_ordinate_get_elevation_ecs11 (dim_ordinate, &error);
  if (!error  && ecs11 == dim_ordinate->elevation.ecs_11)
    pass ("Working Properly");
  else
    fail ("error in reading ecs11");


  ecs12 =
    dwg_ent_dim_ordinate_get_elevation_ecs12 (dim_ordinate, &error);
  if (!error  && ecs12 == dim_ordinate->elevation.ecs_12)	// error checking 
    pass ("Working Properly");
  else
    fail ("error in reading ecs12");


  dwg_ent_dim_ordinate_get_extrusion (dim_ordinate, &ext, &error);
  if (!error  && ext.x == dim_ordinate->extrusion.x
      && ext.y == dim_ordinate->extrusion.y
      && ext.z == dim_ordinate->extrusion.z)
    pass ("Working Properly");
  else
    fail ("error in reading extrusion");


  dwg_ent_dim_ordinate_get_ins_scale (dim_ordinate, &ins_scale,
				      &error);
  if (!error  && dim_ordinate->ins_scale.x == ins_scale.x && dim_ordinate->ins_scale.y == ins_scale.y && dim_ordinate->ins_scale.z == ins_scale.z)	// error checking
    pass ("Working Properly");
  else
    fail ("error in reading ins_scale");


  dwg_ent_dim_ordinate_get_10_pt (dim_ordinate, &pt10, &error);
  if (!error  && pt10.x == dim_ordinate->_10_pt.x && pt10.y == dim_ordinate->_10_pt.y && pt10.z == dim_ordinate->_10_pt.z)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading pt10");


  dwg_ent_dim_ordinate_get_13_pt (dim_ordinate, &pt13, &error);
  if (!error  && pt13.x == dim_ordinate->_13_pt.x && pt13.y == dim_ordinate->_13_pt.y && pt13.z == dim_ordinate->_13_pt.z)	//error checking
    pass ("Working Properly");
  else
    fail ("error in reading pt13");


  dwg_ent_dim_ordinate_get_14_pt (dim_ordinate, &pt14, &error);
  if (!error  && dim_ordinate->_14_pt.x == pt14.x && dim_ordinate->_14_pt.y == pt14.y && dim_ordinate->_14_pt.z == pt14.z)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading pt14");


  dwg_ent_dim_ordinate_get_12_pt (dim_ordinate, &pt12, &error);
  if (!error  && dim_ordinate->_12_pt.x == pt12.x && dim_ordinate->_12_pt.y == pt12.y)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading pt12");


  dwg_ent_dim_ordinate_get_text_mid_pt (dim_ordinate, &text_mid_pt,
					&error);
  if (!error  && text_mid_pt.x == dim_ordinate->text_midpt.x && text_mid_pt.y == dim_ordinate->text_midpt.y)	// error checks
    pass ("Working Properly");
  else
    fail ("error in reading text_mid_pt");


  user_text =
    dwg_ent_dim_ordinate_get_user_text (dim_ordinate, &error);
  if (!error  && dim_ordinate->user_text == user_text)
    pass ("Working Properly");
  else
    fail ("error in reading user_text");


  text_rot =
    dwg_ent_dim_ordinate_get_text_rot (dim_ordinate, &error);
  if (!error  && text_rot == dim_ordinate->text_rot)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading text rotation");


  ins_rot =
    dwg_ent_dim_ordinate_get_ins_rotation (dim_ordinate, &error);
  if (!error  && ins_rot == dim_ordinate->ins_rotation)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading ins rotation");


  flip_arrow1 = dwg_ent_dim_ordinate_get_flip_arrow1 (dim_ordinate,
						      &error);
  if (!error  && flip_arrow1 == dim_ordinate->flip_arrow1)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading arrow1");


  flip_arrow2 = dwg_ent_dim_ordinate_get_flip_arrow2 (dim_ordinate,
						      &error);
  if (!error  && flip_arrow2 == dim_ordinate->flip_arrow2)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading arrow1 \n");


  flags1 = dwg_ent_dim_ordinate_get_flags1 (dim_ordinate, &error);
  if (!error  && dim_ordinate->flags_1 == flags1)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading flags1");


  act_measure = dwg_ent_dim_ordinate_get_act_measurement (dim_ordinate,
							  &error);
  if (!error  && act_measure == dim_ordinate->act_measurement)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading act_measurement");

}
