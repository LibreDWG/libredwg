#define DWG_TYPE DWG_TYPE_DIMENSION_ANG3PT
#include "common.c"
#include <dejagnu.h>

/* This function checks API functions for integrity
   @params dwg_object* obj 
 */
void
api_process (dwg_object * obj)
{
  int error;
  double act_measure, horiz_dir, lspace_factor, text_rot,
    ins_rot, ext_line_rot, dim_rot;
  BITCODE_RC flag1;
  BITCODE_BS lspace_style, attachment;
  BITCODE_B flip_arrow1, flip_arrow2;
  char *user_text;
  dwg_point_2d text_mid_pt, pt12;
  dwg_point_3d pt10, pt13, pt14, pt15, extrusion, ins_scale;
  dwg_ent_dim_ang3pt *dim_ang3pt = dwg_object_to_DIMENSION_ANG3PT (obj);
  dwg_ent_dim *dim = dwg_object_to_DIMENSION(obj);

  dwg_ent_dim_ang3pt_get_def_pt (dim_ang3pt, &pt10, &error);
  if (!error  && dim_ang3pt->def_pt.x == pt10.x && dim_ang3pt->def_pt.y == pt10.y && dim_ang3pt->def_pt.z == pt10.z)
    pass ("Working Properly");
  else
    fail ("error in reading def_pt");

  dwg_ent_dim_ang3pt_get_13_pt (dim_ang3pt, &pt13, &error);
  if (!error  && dim_ang3pt->_13_pt.x == pt13.x && dim_ang3pt->_13_pt.y == pt13.y && dim_ang3pt->_13_pt.z == pt13.z)
    pass ("Working Properly");
  else
    fail ("error in reading pt13");


  dwg_ent_dim_ang3pt_get_14_pt (dim_ang3pt, &pt14, &error);
  if (!error  && dim_ang3pt->_14_pt.x == pt14.x && dim_ang3pt->_14_pt.y == pt14.y && dim_ang3pt->_14_pt.z == pt14.z)
    pass ("Working Properly");
  else
    fail ("error in reading pt14");


  dwg_ent_dim_ang3pt_get_first_arc_pt (dim_ang3pt, &pt15, &error);
  if (!error  && dim_ang3pt->first_arc_pt.x == pt15.x && dim_ang3pt->first_arc_pt.y == pt15.y && dim_ang3pt->first_arc_pt.z == pt15.z)
    pass ("Working Properly");
  else
    fail ("error in reading first_arc_p");

  dwg_ent_dim_get_clone_ins_pt (dim, &pt12, &error);
  if (!error  && dim->clone_ins_pt.x == pt12.x && dim->clone_ins_pt.y == pt12.y)
    pass ("Working Properly");
  else
    fail ("error in reading pt12");

  dwg_ent_dim_get_text_midpt (dim, &text_mid_pt, &error);
  if (!error && text_mid_pt.x == dim->text_midpt.x && text_mid_pt.y == dim->text_midpt.y)
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
