#include "common.c"

void
output_process(dwg_object *obj);

void
output_object(dwg_object* obj){
  if (!obj)
    {
      printf("object is NULL\n");
      return;
    }

  if (dwg_get_type(obj)== DWG_TYPE_DIMENSION_LINEAR)
    {
      output_process(obj);
    }
}

void
low_level_process(dwg_object *obj)
{

  // casts dwg object to linear dimension entity
  dwg_ent_dim_linear *dim_linear = dwg_object_to_DIMENSION_LINEAR(obj);

  // prints horiz dir
  printf("horiz dir of dim_linear : %f\t\n",
          dim_linear->horiz_dir);
  
  // prints lspace factor
  printf("lspace factor of dim_linear : %f\t\n", 
          dim_linear->lspace_factor);

  // prints lspace style
  printf("lspace style of dim_linear : %d\t\n",
          dim_linear->lspace_style);

  // prints attach point
  printf("attach point of dim_linear : %d\t\n",
          dim_linear->attachment_point);

  // prints radius
  printf("Radius of dim_linear : %f\t\n",
          dim_linear->elevation.ecs_11);

  // prints thickness
  printf("Thickness of dim_linear : %f\t\n",
          dim_linear->elevation.ecs_12);

  // prints thickness
  printf("extrusion of dim_linear : x = %f, y = %f, z = %f\t\n",
          dim_linear->extrusion.x, dim_linear->extrusion.y,
          dim_linear->extrusion.z);

  // prints isnertion scale
  printf("ins_scale of dim_linear : x = %f, y = %f, z = %f\t\n",
          dim_linear->ins_scale.x, dim_linear->ins_scale.y,
          dim_linear->ins_scale.z);

  // prints 10 point
  printf("pt10 of dim_linear : x = %f, y = %f, z = %f\t\n",
          dim_linear->_10_pt.x, dim_linear->_10_pt.y,
          dim_linear->_10_pt.z);

  // prints 13 point
  printf("pt13 of dim_linear : x = %f, y = %f, z = %f\t\n",
          dim_linear->_13_pt.x, dim_linear->_13_pt.y,
          dim_linear->_13_pt.z);

  // prints point 14
  printf("pt14 of dim_linear : x = %f, y = %f, z = %f\t\n",
         dim_linear->_14_pt.x, dim_linear->_14_pt.y,
         dim_linear->_14_pt.z);

  // prints 12 point
  printf("pt12 of dim_linear : x = %f, y = %f\t\n",
          dim_linear->_12_pt.x, dim_linear->_12_pt.y);

  // prints mid point of text
  printf("text_mid_pt of dim_linear : x = %f, y = %f\t\n",
          dim_linear->text_midpt.x, dim_linear->text_midpt.y);

  // prints user text
  printf("user text of dim_linear : %s\t\n", dim_linear->user_text);

  // prints text rotation
  printf("text rotation of dim_linear : %f\t\n", dim_linear->text_rot);

  // prints insertion rotation
  printf("ins rotation of dim_linear : %f\t\n", dim_linear->ins_rotation);

  // prints arrow 1
  printf("arrow1 of dim_linear : %d\t\n", dim_linear->flip_arrow1);

  // prints arrow 2
  printf("arrow2 of dim_linear : %d\t\n", dim_linear->flip_arrow2);

  // prints flags2
  printf("flags2 of dim_linear : %d\t\n", dim_linear->flags_1);

  // prints actual measurement
  printf("act_measurement of dim_linear : %f\t\n",
           dim_linear->act_measurement);

  // prints ext line rotation
  printf("ext line rot of dim_linear : %f\t\n",dim_linear->ext_line_rot);

  // prints dim rotation
  printf("dim rot of dim linear : %f\t\n", dim_linear->dim_rot);

}

void
api_process(dwg_object *obj)
{
  int error;
  double ecs11, ecs12, act_measure, horiz_dir, lspace_factor, text_rot, 
         ins_rot, ext_line_rot, dim_rot;
  unsigned int flags1, lspace_style, attach_pt, flip_arrow1, flip_arrow2, 
               flags2;
  char * user_text;
  dwg_point_2d text_mid_pt, pt12;
  dwg_point_3d pt10, pt13, pt14, ext, ins_scale;
  dwg_ent_dim_linear *dim_linear = dwg_object_to_DIMENSION_LINEAR(obj);

  // returns horiz dir
  horiz_dir = dwg_ent_dim_linear_get_horiz_dir(dim_linear,
              &error);
  if (!error)
    {  
      printf("horiz dir of dim_linear : %f\t\n", horiz_dir);
    }
  else
    {
      printf("error in reading horiz dir \n");
    }

  // returns lspace factor
  lspace_factor = dwg_ent_dim_linear_get_elevation_ecs11(dim_linear, 
              &error);
  if (!error)
    {  
      printf("lspace factor of dim_linear : %f\t\n", lspace_factor);
    }
  else
    {
      printf("error in reading lspace factor \n");
    }

  // returns lspace style
  lspace_style = dwg_ent_dim_linear_get_elevation_ecs11(dim_linear, 
              &error);
  if (!error)
    {  
      printf("lspace style of dim_linear : %d\t\n", lspace_style);
    }
  else
    {
      printf("error in reading lspace style \n");
    }

  // returns attachment point
  attach_pt = dwg_ent_dim_linear_get_elevation_ecs11(dim_linear, 
              &error);
  if (!error)
    {  
      printf("attach point of dim_linear : %d\t\n", attach_pt);
    }
  else
    {
      printf("error in reading attach point \n");
    }

  // returns ecs11 elevation
  ecs11 = dwg_ent_dim_linear_get_elevation_ecs11(dim_linear,
              &error);
  if (!error)
    {  
      printf("Radius of dim_linear : %f\t\n",ecs11);
    }
  else
    {
      printf("error in reading ecs11 \n");
    }

  // returns ecs 12 elevation
  ecs12 = dwg_ent_dim_linear_get_elevation_ecs12(dim_linear,
              &error);
  if (!error)
    {
      printf("Thickness of dim_linear : %f\t\n",ecs12);
    }
  else
    {
      printf("error in reading ecs12 \n");
    }

  // returns extrusion points
  dwg_ent_dim_linear_get_extrusion(dim_linear, &ext,
              &error);
  if (!error)
    {
      printf("extrusion of dim_linear : x = %f, y = %f, z = %f\t\n",
              ext.x, ext.y, ext.z);
    }
  else
    {
      printf("error in reading extrusion \n");
    }

  // returns insert scale
  dwg_ent_dim_linear_get_ins_scale(dim_linear, &ins_scale,
              &error);
  if (!error)
    {
      printf("ins_scale of dim_linear : x = %f, y = %f, z = %f\t\n",
              ins_scale.x, ins_scale.y, ins_scale.z);
    }
  else
    {
      printf("error in reading ins_scale \n");
    }

  // returns 10 point
  dwg_ent_dim_linear_get_10_pt(dim_linear, &pt10,
              &error);
  if (!error)
    {
      printf("pt10 of dim_linear : x = %f, y = %f, z = %f\t\n",
              pt10.x, pt10.y, pt10.z);
    }
  else
    {
      printf("error in reading pt10 \n");
    }

  // returns 13 point
  dwg_ent_dim_linear_get_13_pt(dim_linear, &pt13,
              &error);
  if (!error)
    {
      printf("pt13 of dim_linear : x = %f, y = %f, z = %f\t\n",
              pt13.x, pt13.y, pt13.z);
    }
  else
    {
      printf("error in reading pt13 \n");
    }

  // returns 14 point
  dwg_ent_dim_linear_get_14_pt(dim_linear, &pt14,
              &error);
  if (!error)
    {
      printf("pt14 of dim_linear : x = %f, y = %f, z = %f\t\n",
              pt14.x, pt14.y, pt14.z);
    }
  else
    {
      printf("error in reading pt14 \n");
    }

  // returns 12 point
  dwg_ent_dim_linear_get_12_pt(dim_linear, &pt12,
              &error);
  if (!error)
    {
      printf("pt12 of dim_linear : x = %f, y = %f\t\n",
              pt12.x, pt12.y);
    }
  else
    {
      printf("error in reading pt12 \n");
    }

  // returns mid point of text
  dwg_ent_dim_linear_get_text_mid_pt(dim_linear, &text_mid_pt,
              &error);
  if (!error)
    {
      printf("text_mid_pt of dim_linear : x = %f, y = %f\t\n",
              text_mid_pt.x, text_mid_pt.y);
    }
  else
    {
      printf("error in reading text_mid_pt \n");
    }

  // returns user text
  user_text = dwg_ent_dim_linear_get_user_text(dim_linear,
              &error);
  if (!error)
    {  
      printf("user text of dim_linear : %s\t\n",user_text);
    }
  else
    {
      printf("error in reading user_text \n");
    }

  // returns text rotation
  text_rot = dwg_ent_dim_linear_get_text_rot(dim_linear,
              &error);
  if (!error)
    {  
      printf(" text rotation of dim_linear : %f\t\n", text_rot);
    }
  else
    {
      printf("error in reading text rotation \n");
    }

  // returns insert rotation
  ins_rot = dwg_ent_dim_linear_get_ins_rotation(dim_linear,
              &error);
  if (!error)
    {  
      printf("ins rotation of dim_linear : %f\t\n",ins_rot);
    }
  else
    {
      printf("error in reading ins rotation \n");
    }

  // returns flip arrow 1
  flip_arrow1 = dwg_ent_dim_linear_get_flip_arrow1(dim_linear,
              &error);
  if (!error)
    {  
      printf("arrow1 of dim_linear : %d\t\n",flip_arrow1);
    }
  else
    {
      printf("error in reading arrow1 \n");
    }
 
  // returns flip arrow 2
  flip_arrow2 = dwg_ent_dim_linear_get_flip_arrow2(dim_linear,
              &error);
  if (!error)
    {  
      printf("arrow2 of dim_linear : %d\t\n",flip_arrow2);
    }
  else
    {
      printf("error in reading arrow1 \n");
    }

  // returns flags1
  flags1 = dwg_ent_dim_linear_get_flags1(dim_linear,
              &error);
  if (!error)
    {  
      printf("flags1 of dim_linear : %d\t\n",flags1);
    }
  else
    {
      printf("error in reading flags1 \n");
    }

  // returns act measurement
  act_measure = dwg_ent_dim_linear_get_act_measurement(dim_linear,
              &error);
  if (!error)
    {  
      printf("act_measurement of dim_linear : %f\t\n",act_measure);
    }
  else
    {
      printf("error in reading act_measurement \n");
    }

  // returns ext line rot 
  ext_line_rot = dwg_ent_dim_linear_get_ext_line_rotation(dim_linear,
              &error);
  if (!error)
    {  
      printf("ext line rot of dim_linear : %f\t\n", ext_line_rot);
    }
  else
    {
      printf("error in reading ext line rot \n");
    }

  // returns dim rot 
  dim_rot = dwg_ent_dim_linear_get_dim_rot(dim_linear,
              &error);
  if (!error)
    {  
      printf("dim rot of dim linear : %f\t\n", dim_rot);
    }
  else
    {
      printf("error in reading dim rot \n");
    }

}
