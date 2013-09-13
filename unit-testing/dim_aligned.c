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

  if (dwg_get_type(obj)== DWG_TYPE_DIMENSION_ALIGNED)
    {
      output_process(obj);
    }
}

void
low_level_process(dwg_object *obj)
{
  // retruns dwg_object to dim aligned entity
  dwg_ent_dim_aligned *dim_aligned = dwg_object_to_DIMENSION_ALIGNED(obj);

  // prints horiz dir of dim_aligned
  printf("horiz dir of dim_aligned : %f\t\n",
          dim_aligned->horiz_dir);

  // prints lspace factor
  printf("lspace factor of dim_aligned : %f\t\n", 
          dim_aligned->lspace_factor);

  // prints lspace style of dim aligned
  printf("lspace style of dim_aligned : %d\t\n",
          dim_aligned->lspace_style);

  // prints attach point of dim aligned
  printf("attach point of dim_aligned : %d\t\n",
          dim_aligned->attachment_point);

  // prints radius of dim aligned
  printf("Radius of dim_aligned : %f\t\n",
          dim_aligned->elevation.ecs_11);

  // prints elevation of dim aligned 
  printf("Thickness of dim_aligned : %f\t\n",
          dim_aligned->elevation.ecs_12);

  // prints extrusion of dim aligned
  printf("extrusion of dim_aligned : x = %f, y = %f, z = %f\t\n",
          dim_aligned->extrusion.x, dim_aligned->extrusion.y,
          dim_aligned->extrusion.z);

  // prints ins scale
  printf("ins_scale of dim_aligned : x = %f, y = %f, z = %f\t\n",
          dim_aligned->ins_scale.x, dim_aligned->ins_scale.y,
          dim_aligned->ins_scale.z);

  // prints point 10
  printf("pt10 of dim_aligned : x = %f, y = %f, z = %f\t\n",
          dim_aligned->_10_pt.x, dim_aligned->_10_pt.y,
          dim_aligned->_10_pt.z);

  // prints point 13
  printf("pt13 of dim_aligned : x = %f, y = %f, z = %f\t\n",
          dim_aligned->_13_pt.x, dim_aligned->_13_pt.y,
          dim_aligned->_13_pt.z);

  // prints point 14
  printf("pt14 of dim_aligned : x = %f, y = %f, z = %f\t\n",
          dim_aligned->_14_pt.x, dim_aligned->_14_pt.y,
          dim_aligned->_14_pt.z);

  // prints point 12
  printf("pt12 of dim_aligned : x = %f, y = %f\t\n",
          dim_aligned->_12_pt.x, dim_aligned->_12_pt.y);

  // prints mid point of text
  printf("text_mid_pt of dim_aligned : x = %f, y = %f\t\n",
          dim_aligned->text_midpt.x, dim_aligned->text_midpt.y);

  // prints user text
  printf("user text of dim_aligned : %s\t\n", dim_aligned->user_text);

  // prints text rotation 
  printf("text rotation of dim_aligned : %f\t\n", dim_aligned->text_rot);

  // prints ins rotation
  printf("ins rotation of dim_aligned : %f\t\n", dim_aligned->ins_rotation);

  // prints flip arrow1
  printf("arrow1 of dim_aligned : %d\t\n", dim_aligned->flip_arrow1);

  // prints flip arrow2
  printf("arrow2 of dim_aligned : %d\t\n", dim_aligned->flip_arrow2);

  // prints flags2
  printf("flags2 of dim_aligned : %d\t\n", dim_aligned->flags_1);

  // prints act measurement
  printf("act_measurement of dim_aligned : %f\t\n",
          dim_aligned->act_measurement);

  // prints ext line rot
  printf("ext line rot of dim_aligned : ",dim_aligned->ext_line_rot);

}

void
api_process(dwg_object *obj)
{
  int ecs11_error, ecs12_error, flags1_error, act_error, horiz_error, lspace_error,
      style_error, att_point_error, ext_error, user_text_error, text_rot_error, 
      ins_rot_error, arrow1_error, arrow2_error, mid_pt_error, ins_scale_error, 
      flags2_error, pt10_error, pt12_error, pt13_error, pt14_error, 
      dim_rot_error, ext_line_rot_error;
  double ecs11, ecs12, act_measure, horiz_dir, lspace_factor, text_rot, 
         ins_rot, ext_line_rot, dim_rot;
  unsigned int flags1, lspace_style, attach_pt, flip_arrow1, flip_arrow2, 
               flags2;
  char * user_text;
  dwg_point_2d text_mid_pt, pt12;
  dwg_point_3d pt10, pt13, pt14, ext, ins_scale;
  dwg_ent_dim_aligned *dim_aligned = dwg_object_to_DIMENSION_ALIGNED(obj);

  // prints horiz direction value
  horiz_dir = dwg_ent_dim_aligned_get_horiz_dir(dim_aligned, &horiz_error);
  if(horiz_error == 0 )
    {  
      printf("horiz dir of dim_aligned : %f\t\n", horiz_dir);
    }
  else
    {
      printf("error in reading horiz dir \n");
    }

  // returns lspcae factor
  lspace_factor = dwg_ent_dim_aligned_get_elevation_ecs11(dim_aligned, 
                  &lspace_error);
  if(lspace_error == 0 ) // error check
    {  
      printf("lspace factor of dim_aligned : %f\t\n", lspace_factor);
    }
  else
    {
      printf("error in reading lspace factor \n");
    }

  // returns lspace style
  lspace_style = dwg_ent_dim_aligned_get_elevation_ecs11(dim_aligned, 
                  &style_error);
  if(style_error == 0 ) // error check
    {  
      printf("lspace style of dim_aligned : %d\t\n", lspace_style);
    }
  else
    {
      printf("error in reading lspace style \n");
    }

  // returns attachment point 
  attach_pt = dwg_ent_dim_aligned_get_elevation_ecs11(dim_aligned, 
              &att_point_error);
  if(att_point_error == 0 ) // error checking
    {  
      printf("attach point of dim_aligned : %d\t\n", attach_pt);
    }
  else
    {
      printf("error in reading attach point \n");
    }

  // returns ecs 11 of aligned dimension
  ecs11 = dwg_ent_dim_aligned_get_elevation_ecs11(dim_aligned, &ecs11_error);
  if(ecs11_error == 0 )
    {  
      printf("Radius of dim_aligned : %f\t\n",ecs11);
    }
  else
    {
      printf("error in reading ecs11 \n");
    }

  // returns ecs12 of aligned dimension
  ecs12 = dwg_ent_dim_aligned_get_elevation_ecs12(dim_aligned, &ecs12_error);
  if(ecs12_error == 0 ) // error checking 
    {
      printf("Thickness of dim_aligned : %f\t\n",ecs12);
    }
  else
    {
      printf("error in reading ecs12 \n");
    }

  // returns extrusion of aligned dimension
  dwg_ent_dim_aligned_get_extrusion(dim_aligned, &ext,&ext_error);
  if(ext_error == 0 )
    {
      printf("extrusion of dim_aligned : x = %f, y = %f, z = %f\t\n",
              ext.x, ext.y, ext.z);
    }
  else
    {
      printf("error in reading extrusion \n");
    }

  // returns insertion scale of aligned dimension
  dwg_ent_dim_aligned_get_ins_scale(dim_aligned, &ins_scale,&ins_scale_error);
  if(ins_scale_error == 0 ) // error checking
    {
      printf("ins_scale of dim_aligned : x = %f, y = %f, z = %f\t\n",
              ins_scale.x, ins_scale.y, ins_scale.z);
    }
  else
    {
      printf("error in reading ins_scale \n");
    }

  // returns 10 point
  dwg_ent_dim_aligned_get_10_pt(dim_aligned, &pt10,&pt10_error);
  if(pt10_error == 0 )  // error check
    {
      printf("pt10 of dim_aligned : x = %f, y = %f, z = %f\t\n",
              pt10.x, pt10.y, pt10.z);
    }
  else
    {
      printf("error in reading pt10 \n");
    }

  // returns 13 point 
  dwg_ent_dim_aligned_get_13_pt(dim_aligned, &pt13,&pt13_error);
  if(pt13_error == 0 ) //error checking
    {
      printf("pt13 of dim_aligned : x = %f, y = %f, z = %f\t\n",
              pt13.x, pt13.y, pt13.z);
    }
  else
    {
      printf("error in reading pt13 \n");
    }

  // returns 14 point
  dwg_ent_dim_aligned_get_14_pt(dim_aligned, &pt14,&pt14_error);
  if(pt14_error == 0 ) // error check
    {
      printf("pt14 of dim_aligned : x = %f, y = %f, z = %f\t\n",
              pt14.x, pt14.y, pt14.z);
    }
  else
    {
      printf("error in reading pt14 \n");
    }

  // returns 12 point
  dwg_ent_dim_aligned_get_12_pt(dim_aligned, &pt12,&pt12_error);
  if(pt12_error == 0 ) // error check
    {
      printf("pt12 of dim_aligned : x = %f, y = %f\t\n",
              pt12.x, pt12.y);
    }
  else
    {
      printf("error in reading pt12 \n");
    }

  // returns dimension mid point
  dwg_ent_dim_aligned_get_text_mid_pt(dim_aligned, &text_mid_pt,
                                          &mid_pt_error);
  if(mid_pt_error == 0 ) // error checks
    {
      printf("text_mid_pt of dim_aligned : x = %f, y = %f\t\n",
              text_mid_pt.x, text_mid_pt.y);
    }
  else
    {
      printf("error in reading text_mid_pt \n");
    }

  // returns user text of dimension
  user_text = dwg_ent_dim_aligned_get_user_text(dim_aligned, &user_text_error);
  if(user_text_error == 0 )
    {  
      printf("user text of dim_aligned : %s\t\n",user_text);
    }
  else
    {
      printf("error in reading user_text \n");
    }

  // returns rotation of text
  text_rot = dwg_ent_dim_aligned_get_text_rot(dim_aligned, &text_rot_error);
  if(text_rot_error == 0 ) // error check
    {  
      printf(" text rotation of dim_aligned : %f\t\n", text_rot);
    }
  else
    {
      printf("error in reading text rotation \n");
    }

  // returns rotation of insertion of dimension
  ins_rot = dwg_ent_dim_aligned_get_ins_rotation(dim_aligned, &ins_rot_error);
  if(ins_rot_error == 0 ) // error check
    {  
      printf("ins rotation of dim_aligned : %f\t\n",ins_rot);
    }
  else
    {
      printf("error in reading ins rotation \n");
    }
  
  // returns flip arrow
  flip_arrow1 = dwg_ent_dim_aligned_get_flip_arrow1(dim_aligned,
                &arrow1_error);
  if(arrow1_error == 0 ) // error check
    {  
      printf("arrow1 of dim_aligned : %d\t\n",flip_arrow1);
    }
  else
    {
      printf("error in reading arrow1 \n");
    }

  // returns flip arrow2
  flip_arrow2 = dwg_ent_dim_aligned_get_flip_arrow2(dim_aligned,
                &arrow2_error);
  if(arrow2_error == 0 ) // error check
    {  
      printf("arrow1 of dim_aligned : %d\t\n",flip_arrow2);
    }
  else
    {
      printf("error in reading arrow1 \n");
    }

  // returns flag 
  flags1 = dwg_ent_dim_aligned_get_flags1(dim_aligned,
                &flags1_error);
  if(arrow2_error == 0 ) // error check
    {  
      printf("flags1 of dim_aligned : %d\t\n",flags1);
    }
  else
    {
      printf("error in reading flags1 \n");
    }

  // returns act measure
  act_measure = dwg_ent_dim_aligned_get_act_measurement(dim_aligned,
                &act_error);
  if(act_error == 0 ) // error check
    {  
      printf("act_measurement of dim_aligned : %f\t\n",act_measure);
    }
  else
    {
      printf("error in reading act_measurement \n");
    }

  // returns ext line rotation
  ext_line_rot = dwg_ent_dim_aligned_get_ext_line_rotation(dim_aligned,
                &ext_line_rot_error);
  if(ext_line_rot_error == 0 ) // error check
    {  
      printf("ext line rot of dim_aligned : ", ext_line_rot);
    }
  else
    {
      printf("error in reading ext line rot \n");
    }

}
