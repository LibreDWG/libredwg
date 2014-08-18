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

  if (dwg_get_type (obj) == DWG_TYPE_DIMENSION_ALIGNED)
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
    ins_scale_error, flags2_error, pt10_error, pt12_error, pt13_error,
    pt14_error, dim_rot_error, ext_line_rot_error;
  double ecs11, ecs12, act_measure, horiz_dir, lspace_factor, text_rot,
    ins_rot, ext_line_rot, dim_rot;
  unsigned int flags1, lspace_style, attach_pt, flip_arrow1, flip_arrow2,
    flags2;
  char *user_text;
  dwg_point_2d text_mid_pt, pt12;
  dwg_point_3d pt10, pt13, pt14, ext, ins_scale;
  dwg_ent_dim_aligned *dim_aligned = dwg_object_to_DIMENSION_ALIGNED (obj);

  // prints horiz direction value
  horiz_dir = dwg_ent_dim_aligned_get_horiz_dir (dim_aligned, &horiz_error);
  if (horiz_error == 0 && horiz_dir == dim_aligned->horiz_dir)
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading horiz dir");
    }

  // returns lspcae factor
  lspace_factor = dwg_ent_dim_aligned_get_elevation_ecs11 (dim_aligned,
							   &lspace_error);
  if (lspace_error == 0 && dim_aligned->lspace_factor == lspace_factor)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading lspace factor");
    }

  // returns lspace style
  lspace_style = dwg_ent_dim_aligned_get_elevation_ecs11 (dim_aligned,
							  &style_error);
  if (style_error == 0 && lspace_style == dim_aligned->lspace_style)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading lspace style");
    }

  // returns attachment point 
  attach_pt = dwg_ent_dim_aligned_get_elevation_ecs11 (dim_aligned,
						       &att_point_error);
  if (att_point_error == 0 && attach_pt == dim_aligned->attachment_point)	// error checking
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading attach point");
    }

  // returns ecs 11 of aligned dimension
  ecs11 = dwg_ent_dim_aligned_get_elevation_ecs11 (dim_aligned, &ecs11_error);
  if (ecs11_error == 0 && ecs11 == dim_aligned->elevation.ecs_11)
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading ecs11");
    }

  // returns ecs12 of aligned dimension
  ecs12 = dwg_ent_dim_aligned_get_elevation_ecs12 (dim_aligned, &ecs12_error);
  if (ecs12_error == 0 && ecs12 == dim_aligned->elevation.ecs_12)	// error checking 
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading ecs12");
    }

  // returns extrusion of aligned dimension
  dwg_ent_dim_aligned_get_extrusion (dim_aligned, &ext, &ext_error);
  if (ext_error == 0 && ext.x == dim_aligned->extrusion.x
      && ext.y == dim_aligned->extrusion.y
      && ext.z == dim_aligned->extrusion.z)
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading extrusion");
    }

  // returns insertion scale of aligned dimension
  dwg_ent_dim_aligned_get_ins_scale (dim_aligned, &ins_scale,
				     &ins_scale_error);
  if (ins_scale_error == 0 && dim_aligned->ins_scale.x == ins_scale.x && dim_aligned->ins_scale.y == ins_scale.y && dim_aligned->ins_scale.z == ins_scale.z)	// error checking
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading ins_scale");
    }

  // returns 10 point
  dwg_ent_dim_aligned_get_10_pt (dim_aligned, &pt10, &pt10_error);
  if (pt10_error == 0 && pt10.x == dim_aligned->_10_pt.x && pt10.y == dim_aligned->_10_pt.y && pt10.z == dim_aligned->_10_pt.z)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading pt10");
    }

  // returns 13 point 
  dwg_ent_dim_aligned_get_13_pt (dim_aligned, &pt13, &pt13_error);
  if (pt13_error == 0 && pt13.x == dim_aligned->_13_pt.x && pt13.y == dim_aligned->_13_pt.y && pt13.z == dim_aligned->_13_pt.z)	//error checking
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading pt13");
    }

  // returns 14 point
  dwg_ent_dim_aligned_get_14_pt (dim_aligned, &pt14, &pt14_error);
  if (pt14_error == 0 && dim_aligned->_14_pt.x == pt14.x && dim_aligned->_14_pt.y == pt14.y && dim_aligned->_14_pt.z == pt14.z)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading pt14");
    }

  // returns 12 point
  dwg_ent_dim_aligned_get_12_pt (dim_aligned, &pt12, &pt12_error);
  if (pt12_error == 0 && dim_aligned->_12_pt.x == pt12.x && dim_aligned->_12_pt.y == pt12.y)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading pt12");
    }

  // returns dimension mid point
  dwg_ent_dim_aligned_get_text_mid_pt (dim_aligned, &text_mid_pt,
				       &mid_pt_error);
  if (mid_pt_error == 0 && text_mid_pt.x == dim_aligned->text_midpt.x && text_mid_pt.y == dim_aligned->text_midpt.y)	// error checks
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading text_mid_pt");
    }

  // returns user text of dimension
  user_text =
    dwg_ent_dim_aligned_get_user_text (dim_aligned, &user_text_error);
  if (user_text_error == 0 && dim_aligned->user_text == user_text)
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading user_text");
    }

  // returns rotation of text
  text_rot = dwg_ent_dim_aligned_get_text_rot (dim_aligned, &text_rot_error);
  if (text_rot_error == 0 && text_rot == dim_aligned->text_rot)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading text rotation");
    }

  // returns rotation of insertion of dimension
  ins_rot =
    dwg_ent_dim_aligned_get_ins_rotation (dim_aligned, &ins_rot_error);
  if (ins_rot_error == 0 && ins_rot == dim_aligned->ins_rotation)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading ins rotation");
    }

  // returns flip arrow
  flip_arrow1 = dwg_ent_dim_aligned_get_flip_arrow1 (dim_aligned,
						     &arrow1_error);
  if (arrow1_error == 0 && flip_arrow1 == dim_aligned->flip_arrow1)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading arrow1");
    }

  // returns flip arrow2
  flip_arrow2 = dwg_ent_dim_aligned_get_flip_arrow2 (dim_aligned,
						     &arrow2_error);
  if (arrow2_error == 0 && flip_arrow2 == dim_aligned->flip_arrow2)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading arrow1 \n");
    }

  // returns flag 
  flags1 = dwg_ent_dim_aligned_get_flags1 (dim_aligned, &flags1_error);
  if (arrow2_error == 0 && dim_aligned->flags_1 == flags1)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading flags1");
    }

  // returns act measure
  act_measure = dwg_ent_dim_aligned_get_act_measurement (dim_aligned,
							 &act_error);
  if (act_error == 0 && act_measure == dim_aligned->act_measurement)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading act_measurement");
    }

  // returns ext line rotation
  ext_line_rot = dwg_ent_dim_aligned_get_ext_line_rotation (dim_aligned,
							    &ext_line_rot_error);
  if (ext_line_rot_error == 0 && ext_line_rot == dim_aligned->ext_line_rot)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading ext line rot");
    }

}
