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

  if (dwg_get_type (obj) == DWG_TYPE_DIMENSION_ORDINATE)
    {
      output_process (obj);
    }
}

void
low_level_process (dwg_object * obj)
{

  // casts dwg object to ordinate
  dwg_ent_dim_ordinate *dim_ordinate = dwg_object_to_DIMENSION_ORDINATE (obj);

  // prints horiz dir
  printf ("horiz dir of dim_ordinate : %f\t\n", dim_ordinate->horiz_dir);

  // prints lspace factor
  printf ("lspace factor of dim_ordinate : %f\t\n",
	  dim_ordinate->lspace_factor);

  // prints lspace styke
  printf ("lspace style of dim_ordinate : %d\t\n",
	  dim_ordinate->lspace_style);

  // prints attach point
  printf ("attach point of dim_ordinate : %d\t\n",
	  dim_ordinate->attachment_point);

  // prints radius
  printf ("Radius of dim_ordinate : %f\t\n", dim_ordinate->elevation.ecs_11);

  // prints thickness
  printf ("Thickness of dim_ordinate : %f\t\n",
	  dim_ordinate->elevation.ecs_12);

  // prints extrusion points
  printf ("extrusion of dim_ordinate : x = %f, y = %f, z = %f\t\n",
	  dim_ordinate->extrusion.x, dim_ordinate->extrusion.y,
	  dim_ordinate->extrusion.z);

  // prints scale
  printf ("ins_scale of dim_ordinate : x = %f, y = %f, z = %f\t\n",
	  dim_ordinate->ins_scale.x, dim_ordinate->ins_scale.y,
	  dim_ordinate->ins_scale.z);

  // prints 10 point
  printf ("pt10 of dim_ordinate : x = %f, y = %f, z = %f\t\n",
	  dim_ordinate->_10_pt.x, dim_ordinate->_10_pt.y,
	  dim_ordinate->_10_pt.z);

  // prints 13point
  printf ("pt13 of dim_ordinate : x = %f, y = %f, z = %f\t\n",
	  dim_ordinate->_13_pt.x, dim_ordinate->_13_pt.y,
	  dim_ordinate->_13_pt.z);

  // prints 14 point
  printf ("pt14 of dim_ordinate : x = %f, y = %f, z = %f\t\n",
	  dim_ordinate->_14_pt.x, dim_ordinate->_14_pt.y,
	  dim_ordinate->_14_pt.z);

  // prints 12 point
  printf ("pt12 of dim_ordinate : x = %f, y = %f\t\n",
	  dim_ordinate->_12_pt.x, dim_ordinate->_12_pt.y);

  // prints mid point
  printf ("text_mid_pt of dim_ordinate : x = %f, y = %f\t\n",
	  dim_ordinate->text_midpt.x, dim_ordinate->text_midpt.y);

  // prints user text
  printf ("user text of dim_ordinate : %s\t\n", dim_ordinate->user_text);

  // prints text rotation
  printf ("text rotation of dim_ordinate : %f\t\n", dim_ordinate->text_rot);

  // prints insert rotation
  printf ("ins rotation of dim_ordinate : %f\t\n",
	  dim_ordinate->ins_rotation);

  // prints dimension arrow1
  printf ("arrow1 of dim_ordinate : %d\t\n", dim_ordinate->flip_arrow1);

  // prints dimension arrow 2
  printf ("arrow1 of dim_ordinate : %d\t\n", dim_ordinate->flip_arrow2);

  // prints flags 2
  printf ("flags2 of dim_ordinate : %d\t\n", dim_ordinate->flags_2);

  // prints flag 1
  printf ("flags1 of dim_ordinate : %d\t\n", dim_ordinate->flags_1);

  // prints act measurement
  printf ("act_measurement of dim_ordinate : %f\t\n",
	  dim_ordinate->act_measurement);

}

void
api_process (dwg_object * obj)
{
  int ecs11_error, ecs12_error, flags1_error, act_error, horiz_error,
    lspace_error, style_error, att_point_error, ext_error, user_text_error,
    text_rot_error, ins_rot_error, arrow1_error, arrow2_error, mid_pt_error,
    ins_scale_error, flags2_error, pt10_error, pt12_error, pt13_error,
    pt14_error;
  double ecs11, ecs12, act_measure, horiz_dir, lspace_factor, text_rot,
    ins_rot;
  unsigned int flags1, lspace_style, attach_pt, flip_arrow1, flip_arrow2,
    flags2;
  char *user_text;
  dwg_point_2d text_mid_pt, pt12;
  dwg_point_3d pt10, pt13, pt14, ext, ins_scale;
  dwg_ent_dim_ordinate *dim_ordinate = dwg_object_to_DIMENSION_ORDINATE (obj);

  // returns horiz dir
  horiz_dir = dwg_ent_dim_ordinate_get_horiz_dir (dim_ordinate, &horiz_error);
  if (horiz_error == 0 && horiz_dir == dim_ordinate->horiz_dir)
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading horiz dir");
    }

  // returns lspcae factor
  lspace_factor = dwg_ent_dim_ordinate_get_elevation_ecs11 (dim_ordinate,
							    &lspace_error);
  if (lspace_error == 0 && dim_ordinate->lspace_factor == lspace_factor)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading lspace factor");
    }

  // returns lspace style
  lspace_style = dwg_ent_dim_ordinate_get_elevation_ecs11 (dim_ordinate,
							   &style_error);
  if (style_error == 0 && lspace_style == dim_ordinate->lspace_style)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading lspace style");
    }

  // returns attachment point 
  attach_pt = dwg_ent_dim_ordinate_get_elevation_ecs11 (dim_ordinate,
							&att_point_error);
  if (att_point_error == 0 && attach_pt == dim_ordinate->attachment_point)	// error checking
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading attach point");
    }

  // returns ecs 11 of ordinate dimension
  ecs11 =
    dwg_ent_dim_ordinate_get_elevation_ecs11 (dim_ordinate, &ecs11_error);
  if (ecs11_error == 0 && ecs11 == dim_ordinate->elevation.ecs_11)
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading ecs11");
    }

  // returns ecs12 of ordinate dimension
  ecs12 =
    dwg_ent_dim_ordinate_get_elevation_ecs12 (dim_ordinate, &ecs12_error);
  if (ecs12_error == 0 && ecs12 == dim_ordinate->elevation.ecs_12)	// error checking 
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading ecs12");
    }

  // returns extrusion of ordinate dimension
  dwg_ent_dim_ordinate_get_extrusion (dim_ordinate, &ext, &ext_error);
  if (ext_error == 0 && ext.x == dim_ordinate->extrusion.x
      && ext.y == dim_ordinate->extrusion.y
      && ext.z == dim_ordinate->extrusion.z)
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading extrusion");
    }

  // returns insertion scale of ordinate dimension
  dwg_ent_dim_ordinate_get_ins_scale (dim_ordinate, &ins_scale,
				      &ins_scale_error);
  if (ins_scale_error == 0 && dim_ordinate->ins_scale.x == ins_scale.x && dim_ordinate->ins_scale.y == ins_scale.y && dim_ordinate->ins_scale.z == ins_scale.z)	// error checking
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading ins_scale");
    }

  // returns 10 point
  dwg_ent_dim_ordinate_get_10_pt (dim_ordinate, &pt10, &pt10_error);
  if (pt10_error == 0 && pt10.x == dim_ordinate->_10_pt.x && pt10.y == dim_ordinate->_10_pt.y && pt10.z == dim_ordinate->_10_pt.z)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading pt10");
    }

  // returns 13 point 
  dwg_ent_dim_ordinate_get_13_pt (dim_ordinate, &pt13, &pt13_error);
  if (pt13_error == 0 && pt13.x == dim_ordinate->_13_pt.x && pt13.y == dim_ordinate->_13_pt.y && pt13.z == dim_ordinate->_13_pt.z)	//error checking
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading pt13");
    }

  // returns 14 point
  dwg_ent_dim_ordinate_get_14_pt (dim_ordinate, &pt14, &pt14_error);
  if (pt14_error == 0 && dim_ordinate->_14_pt.x == pt14.x && dim_ordinate->_14_pt.y == pt14.y && dim_ordinate->_14_pt.z == pt14.z)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading pt14");
    }

  // returns 12 point
  dwg_ent_dim_ordinate_get_12_pt (dim_ordinate, &pt12, &pt12_error);
  if (pt12_error == 0 && dim_ordinate->_12_pt.x == pt12.x && dim_ordinate->_12_pt.y == pt12.y)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading pt12");
    }

  // returns dimension mid point
  dwg_ent_dim_ordinate_get_text_mid_pt (dim_ordinate, &text_mid_pt,
					&mid_pt_error);
  if (mid_pt_error == 0 && text_mid_pt.x == dim_ordinate->text_midpt.x && text_mid_pt.y == dim_ordinate->text_midpt.y)	// error checks
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading text_mid_pt");
    }

  // returns user text of dimension
  user_text =
    dwg_ent_dim_ordinate_get_user_text (dim_ordinate, &user_text_error);
  if (user_text_error == 0 && dim_ordinate->user_text == user_text)
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading user_text");
    }

  // returns rotation of text
  text_rot =
    dwg_ent_dim_ordinate_get_text_rot (dim_ordinate, &text_rot_error);
  if (text_rot_error == 0 && text_rot == dim_ordinate->text_rot)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading text rotation");
    }

  // returns rotation of insertion of dimension
  ins_rot =
    dwg_ent_dim_ordinate_get_ins_rotation (dim_ordinate, &ins_rot_error);
  if (ins_rot_error == 0 && ins_rot == dim_ordinate->ins_rotation)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading ins rotation");
    }

  // returns flip arrow
  flip_arrow1 = dwg_ent_dim_ordinate_get_flip_arrow1 (dim_ordinate,
						      &arrow1_error);
  if (arrow1_error == 0 && flip_arrow1 == dim_ordinate->flip_arrow1)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading arrow1");
    }

  // returns flip arrow2
  flip_arrow2 = dwg_ent_dim_ordinate_get_flip_arrow2 (dim_ordinate,
						      &arrow2_error);
  if (arrow2_error == 0 && flip_arrow2 == dim_ordinate->flip_arrow2)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading arrow1 \n");
    }

  // returns flag 
  flags1 = dwg_ent_dim_ordinate_get_flags1 (dim_ordinate, &flags1_error);
  if (arrow2_error == 0 && dim_ordinate->flags_1 == flags1)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading flags1");
    }

  // returns act measure
  act_measure = dwg_ent_dim_ordinate_get_act_measurement (dim_ordinate,
							  &act_error);
  if (act_error == 0 && act_measure == dim_ordinate->act_measurement)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading act_measurement");
    }

}
