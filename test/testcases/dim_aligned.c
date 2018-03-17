#define DWG_TYPE DWG_TYPE_DIMENSION_ALIGNED
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
  dwg_ent_dim_aligned *dim_aligned = dwg_object_to_DIMENSION_ALIGNED (obj);


  horiz_dir = dwg_ent_dim_aligned_get_horiz_dir (dim_aligned, &error);
  if (!error  && horiz_dir == dim_aligned->horiz_dir)
    pass ("Working Properly");
  else
    fail ("error in reading horiz dir");


  lspace_factor = dwg_ent_dim_aligned_get_elevation_ecs11 (dim_aligned, &error);
  if (!error  && dim_aligned->lspace_factor == lspace_factor)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading lspace factor");


  lspace_style = dwg_ent_dim_aligned_get_elevation_ecs11 (dim_aligned, &error);
  if (!error  && lspace_style == dim_aligned->lspace_style)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading lspace style");


  attach_pt = dwg_ent_dim_aligned_get_elevation_ecs11 (dim_aligned, &error);
  if (!error  && attach_pt == dim_aligned->attachment_point)	// error checking
    pass ("Working Properly");
  else
    fail ("error in reading attach point");


  ecs11 = dwg_ent_dim_aligned_get_elevation_ecs11 (dim_aligned, &error);
  if (!error  && ecs11 == dim_aligned->elevation.ecs_11)
    pass ("Working Properly");
  else
    fail ("error in reading ecs11");


  ecs12 = dwg_ent_dim_aligned_get_elevation_ecs12 (dim_aligned, &error);
  if (!error  && ecs12 == dim_aligned->elevation.ecs_12)	// error checking 
    pass ("Working Properly");
  else
    fail ("error in reading ecs12");


  dwg_ent_dim_aligned_get_extrusion (dim_aligned, &ext, &error);
  if (!error  && ext.x == dim_aligned->extrusion.x
      && ext.y == dim_aligned->extrusion.y
      && ext.z == dim_aligned->extrusion.z)
    pass ("Working Properly");
  else
    fail ("error in reading extrusion");


  dwg_ent_dim_aligned_get_ins_scale (dim_aligned, &ins_scale, &error);
  if (!error  && dim_aligned->ins_scale.x == ins_scale.x && dim_aligned->ins_scale.y == ins_scale.y && dim_aligned->ins_scale.z == ins_scale.z)	// error checking
    pass ("Working Properly");
  else
    fail ("error in reading ins_scale");


  dwg_ent_dim_aligned_get_10_pt (dim_aligned, &pt10, &error);
  if (!error  && pt10.x == dim_aligned->_10_pt.x && pt10.y == dim_aligned->_10_pt.y && pt10.z == dim_aligned->_10_pt.z)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading pt10");


  dwg_ent_dim_aligned_get_13_pt (dim_aligned, &pt13, &error);
  if (!error  && pt13.x == dim_aligned->_13_pt.x && pt13.y == dim_aligned->_13_pt.y && pt13.z == dim_aligned->_13_pt.z)	//error checking
    pass ("Working Properly");
  else
    fail ("error in reading pt13");


  dwg_ent_dim_aligned_get_14_pt (dim_aligned, &pt14, &error);
  if (!error  && dim_aligned->_14_pt.x == pt14.x && dim_aligned->_14_pt.y == pt14.y && dim_aligned->_14_pt.z == pt14.z)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading pt14");


  dwg_ent_dim_aligned_get_12_pt (dim_aligned, &pt12, &error);
  if (!error  && dim_aligned->_12_pt.x == pt12.x && dim_aligned->_12_pt.y == pt12.y)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading pt12");


  dwg_ent_dim_aligned_get_text_mid_pt (dim_aligned, &text_mid_pt, &error);
  if (!error  && text_mid_pt.x == dim_aligned->text_midpt.x && text_mid_pt.y == dim_aligned->text_midpt.y)	// error checks
    pass ("Working Properly");
  else
    fail ("error in reading text_mid_pt");


  user_text =
    dwg_ent_dim_aligned_get_user_text (dim_aligned, &error);
  if (!error  && dim_aligned->user_text == user_text)
    pass ("Working Properly");
  else
    fail ("error in reading user_text");


  text_rot = dwg_ent_dim_aligned_get_text_rot (dim_aligned, &error);
  if (!error  && text_rot == dim_aligned->text_rot)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading text rotation");


  ins_rot =
    dwg_ent_dim_aligned_get_ins_rotation (dim_aligned, &error);
  if (!error  && ins_rot == dim_aligned->ins_rotation)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading ins rotation");


  flip_arrow1 = dwg_ent_dim_aligned_get_flip_arrow1 (dim_aligned, &error);
  if (!error  && flip_arrow1 == dim_aligned->flip_arrow1)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading arrow1");


  flip_arrow2 = dwg_ent_dim_aligned_get_flip_arrow2 (dim_aligned, &error);
  if (!error  && flip_arrow2 == dim_aligned->flip_arrow2)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading arrow1 \n");


  flags1 = dwg_ent_dim_aligned_get_flags1 (dim_aligned, &error);
  if (!error  && dim_aligned->flags_1 == flags1)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading flags1");


  act_measure = dwg_ent_dim_aligned_get_act_measurement (dim_aligned, &error);
  if (!error  && act_measure == dim_aligned->act_measurement)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading act_measurement");


  ext_line_rot = dwg_ent_dim_aligned_get_ext_line_rotation (dim_aligned, &error);
  if (!error  && ext_line_rot == dim_aligned->ext_line_rot)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading ext line rot");

}
