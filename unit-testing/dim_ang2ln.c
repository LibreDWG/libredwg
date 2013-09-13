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

  if (dwg_get_type(obj)== DWG_TYPE_DIMENSION_ANG2LN)
    {
      output_process(obj);
    }
}

void
low_level_process(dwg_object *obj)
{

  // casts dwg object to ang2ln dimension entity
  dwg_ent_dim_ang2ln *dim_ang2ln = dwg_object_to_DIMENSION_ANG2LN(obj);

  // prints horiz dir
  printf("horiz dir of dim_ang2ln : %f\t\n",
          dim_ang2ln->horiz_dir);

  // prints lspace factor 
  printf("lspace factor of dim_ang2ln : %f\t\n", 
          dim_ang2ln->lspace_factor);

  // prints lspace style
  printf("lspace style of dim_ang2ln : %d\t\n",
          dim_ang2ln->lspace_style);

  // prints attach point
  printf("attach point of dim_ang2ln : %d\t\n",
          dim_ang2ln->attachment_point);

  // print radius of ang2ln dimension
  printf("Radius of dim_ang2ln : %f\t\n",
          dim_ang2ln->elevation.ecs_11);

  // prints thickness
  printf("Thickness of dim_ang2ln : %f\t\n",
          dim_ang2ln->elevation.ecs_12);

  // prints extrusion point
  printf("extrusion of dim_ang2ln : x = %f, y = %f, z = %f\t\n",
          dim_ang2ln->extrusion.x, dim_ang2ln->extrusion.y,
          dim_ang2ln->extrusion.z);

  // prints insertion scale
  printf("ins_scale of dim_ang2ln : x = %f, y = %f, z = %f\t\n",
         dim_ang2ln->ins_scale.x, dim_ang2ln->ins_scale.y,
         dim_ang2ln->ins_scale.z);

  // prints 10 point 
  printf("pt10 of dim_ang2ln : x = %f, y = %f, z = %f\t\n",
          dim_ang2ln->_10_pt.x, dim_ang2ln->_10_pt.y,
          dim_ang2ln->_10_pt.z);

  // prints  13 point
  printf("pt13 of dim_ang2ln : x = %f, y = %f, z = %f\t\n",
          dim_ang2ln->_13_pt.x, dim_ang2ln->_13_pt.y,
          dim_ang2ln->_13_pt.z);

  // prints 14 point
  printf("pt14 of dim_ang2ln : x = %f, y = %f, z = %f\t\n",
          dim_ang2ln->_14_pt.x, dim_ang2ln->_14_pt.y,
          dim_ang2ln->_14_pt.z);

  // prints 15 point
  printf("pt15 of dim_ang2ln : x = %f, y = %f, z = %f\t\n",
          dim_ang2ln->_15_pt.x, dim_ang2ln->_15_pt.y,
          dim_ang2ln->_15_pt.z);

  // print 12 point
  printf("pt12 of dim_ang2ln : x = %f, y = %f\t\n",
          dim_ang2ln->_12_pt.x, dim_ang2ln->_12_pt.y);

  // print 16 point
  printf("pt16 of dim_ang2ln : x = %f, y = %f\t\n",
          dim_ang2ln->_16_pt.x, dim_ang2ln->_16_pt.y);

  // prints mid point of text
  printf("text_mid_pt of dim_ang2ln : x = %f, y = %f\t\n",
          dim_ang2ln->text_midpt.x, dim_ang2ln->text_midpt.y);

  // prints text
  printf("user text of dim_ang2ln : %s\t\n", dim_ang2ln->user_text);

  // prints text rotation 
  printf("text rotation of dim_ang2ln : %f\t\n", dim_ang2ln->text_rot);

  // prints insert rotation
  printf("ins rotation of dim_ang2ln : %f\t\n", dim_ang2ln->ins_rotation);

  // prints arrow1
  printf("arrow1 of dim_ang2ln : %d\t\n", dim_ang2ln->flip_arrow1);

  // prints arrow2
  printf("arrow2 of dim_ang2ln : %d\t\n", dim_ang2ln->flip_arrow2);

  // prints flag2
  printf("flags2 of dim_ang2ln : %d\t\n", dim_ang2ln->flags_1);

  // prints actual measurement
  printf("act_measurement of dim_ang2ln : %f\t\n",
            dim_ang2ln->act_measurement); 
}

void
api_process(dwg_object *obj)
{
  int ecs11_error, ecs12_error, flags1_error, act_error, horiz_error, lspace_error,
      style_error, att_point_error, ext_error, user_text_error, text_rot_error, 
      ins_rot_error, arrow1_error, arrow2_error, mid_pt_error, ins_scale_error, 
      flags2_error, pt10_error, pt12_error, pt13_error, pt14_error, 
      dim_rot_error, ext_line_rot_error, pt15_error, pt16_error;
  double ecs11, ecs12, act_measure, horiz_dir, lspace_factor, text_rot, 
         ins_rot, ext_line_rot, dim_rot;
  unsigned int flags1, lspace_style, attach_pt, flip_arrow1, flip_arrow2, 
               flags2;
  char * user_text;
  dwg_point_2d text_mid_pt, pt12, pt16;
  dwg_point_3d pt10, pt13, pt14, pt15, ext, ins_scale;
  dwg_ent_dim_ang2ln *dim_ang2ln = dwg_object_to_DIMENSION_ANG2LN(obj);


  // returns horizontal direction
  horiz_dir = dwg_ent_dim_ang2ln_get_horiz_dir(dim_ang2ln, &horiz_error);
  if(horiz_error == 0 ) // error check
    {  
      printf("horiz dir of dim_ang2ln : %f\t\n", horiz_dir);
    }
  else
    {
      printf("error in reading horiz dir \n");
    }

  // returns lspace factor
  lspace_factor = dwg_ent_dim_ang2ln_get_elevation_ecs11(dim_ang2ln, 
                  &lspace_error);
  if(lspace_error == 0 ) // error check
    {  
      printf("lspace factor of dim_ang2ln : %f\t\n", lspace_factor);
    }
  else
    {
      printf("error in reading lspace factor \n");
    }

  // returns lspace style
  lspace_style = dwg_ent_dim_ang2ln_get_elevation_ecs11(dim_ang2ln, 
                  &style_error);
  if(style_error == 0 ) // error check
    {  
      printf("lspace style of dim_ang2ln : %d\t\n", lspace_style);
    }
  else
    {
      printf("error in reading lspace style \n");
    }

  // returns attachment point
  attach_pt = dwg_ent_dim_ang2ln_get_elevation_ecs11(dim_ang2ln, 
              &att_point_error);
  if(att_point_error == 0 ) // error check
    {  
      printf("attach point of dim_ang2ln : %d\t\n", attach_pt);
    }
  else
    {
      printf("error in reading attach point \n");
    }

  // returns ecs11 elevation
  ecs11 = dwg_ent_dim_ang2ln_get_elevation_ecs11(dim_ang2ln, &ecs11_error);
  if(ecs11_error == 0 )
    {  
      printf("Radius of dim_ang2ln : %f\t\n",ecs11);
    }
  else
    {
      printf("error in reading ecs11 \n");
    }

  // returns ecs12 elevation
  ecs12 = dwg_ent_dim_ang2ln_get_elevation_ecs12(dim_ang2ln, &ecs12_error);
  if(ecs12_error == 0 ) // error check
    {
      printf("Thickness of dim_ang2ln : %f\t\n",ecs12);
    }
  else
    {
      printf("error in reading ecs12 \n");
    }

  // returns extrusion
  dwg_ent_dim_ang2ln_get_extrusion(dim_ang2ln, &ext,&ext_error);
  if(ext_error == 0 ) // error check
    {
      printf("extrusion of dim_ang2ln : x = %f, y = %f, z = %f\t\n",
              ext.x, ext.y, ext.z);
    }
  else
    {
      printf("error in reading extrusion \n");
    }

  // returns insert scale
  dwg_ent_dim_ang2ln_get_ins_scale(dim_ang2ln, &ins_scale,&ins_scale_error);
  if(ins_scale_error == 0 ) // error check
    {
      printf("ins_scale of dim_ang2ln : x = %f, y = %f, z = %f\t\n",
              ins_scale.x, ins_scale.y, ins_scale.z);
    }
  else
    {
      printf("error in reading ins_scale \n");
    }

  // returns point 10
  dwg_ent_dim_ang2ln_get_10_pt(dim_ang2ln, &pt10,&pt10_error);
  if(pt10_error == 0 ) // error check
    {
      printf("pt10 of dim_ang2ln : x = %f, y = %f, z = %f\t\n",
              pt10.x, pt10.y, pt10.z);
    }
  else
    {
      printf("error in reading pt10 \n");
    }

  // returns point 13
  dwg_ent_dim_ang2ln_get_13_pt(dim_ang2ln, &pt13,&pt13_error);
  if(pt13_error == 0 ) // error check
    {
      printf("pt13 of dim_ang2ln : x = %f, y = %f, z = %f\t\n",
              pt13.x, pt13.y, pt13.z);
    }
  else
    {
      printf("error in reading pt13 \n");
    }

  // returns point 14
  dwg_ent_dim_ang2ln_get_14_pt(dim_ang2ln, &pt14,&pt14_error);
  if(pt14_error == 0 ) // error check
    {
      printf("pt14 of dim_ang2ln : x = %f, y = %f, z = %f\t\n",
              pt14.x, pt14.y, pt14.z);
    }
  else
    {
      printf("error in reading pt14 \n");
    }

  // returns 15 point
  dwg_ent_dim_ang2ln_get_15_pt(dim_ang2ln, &pt15, &pt15_error);
  if(pt15_error == 0 ) // error checking
    {
      printf("pt15 of dim_ang2ln : x = %f, y = %f, z = %f\t\n",
              pt15.x, pt15.y, pt15.z);
    }
  else
    {
      printf("error in reading pt14 \n");
    }

  // return point 12
  dwg_ent_dim_ang2ln_get_12_pt(dim_ang2ln, &pt12,&pt12_error);
  if(pt12_error == 0 ) // error check
    {
      printf("pt12 of dim_ang2ln : x = %f, y = %f\t\n",
              pt12.x, pt12.y);
    }
  else
    {
      printf("error in reading pt12 \n");
    }

  // returns point 16
  dwg_ent_dim_ang2ln_get_16_pt(dim_ang2ln, &pt16, &pt16_error);
  if(pt16_error == 0 ) // error check
    {
      printf("pt16 of dim_ang2ln : x = %f, y = %f\t\n",
              pt16.x, pt16.y);
    }
  else
    {
      printf("error in reading pt16 \n");
    }
  
  // returns text mid point
  dwg_ent_dim_ang2ln_get_text_mid_pt(dim_ang2ln, &text_mid_pt,
                                          &mid_pt_error);
  if(mid_pt_error == 0 ) // error checking
    {
      printf("text_mid_pt of dim_ang2ln : x = %f, y = %f\t\n",
              text_mid_pt.x, text_mid_pt.y);
    }
  else
    {
      printf("error in reading text_mid_pt \n");
    }

  // returns user text
  user_text = dwg_ent_dim_ang2ln_get_user_text(dim_ang2ln, &user_text_error);
  if(user_text_error == 0 ) // error checking
    {  
      printf("user text of dim_ang2ln : %s\t\n",user_text);
    }
  else
    {
      printf("error in reading user_text \n");
    }

  // returns text rotation
  text_rot = dwg_ent_dim_ang2ln_get_text_rot(dim_ang2ln, &text_rot_error);
  if(text_rot_error == 0 ) // error checking
    {  
      printf(" text rotation of dim_ang2ln : %f\t\n", text_rot);
    }
  else
    {
      printf("error in reading text rotation \n");
    }

  // returns insert rotation
  ins_rot = dwg_ent_dim_ang2ln_get_ins_rotation(dim_ang2ln, &ins_rot_error);
  if(ins_rot_error == 0 ) // error checking
    {  
      printf("ins rotation of dim_ang2ln : %f\t\n",ins_rot);
    }
  else
    {
      printf("error in reading ins rotation \n");
    }

  // returns flip arrow 1
  flip_arrow1 = dwg_ent_dim_ang2ln_get_flip_arrow1(dim_ang2ln,
                &arrow1_error);
  if(arrow1_error == 0 )
    {  
      printf("arrow1 of dim_ang2ln : %d\t\n",flip_arrow1);
    }
  else
    {
      printf("error in reading arrow1 \n");
    }

  // returns flip arrow 2
  flip_arrow2 = dwg_ent_dim_ang2ln_get_flip_arrow2(dim_ang2ln,
                &arrow2_error);
  if(arrow2_error == 0 ) // error reporting
    {  
      printf("arrow1 of dim_ang2ln : %d\t\n",flip_arrow2);
    }
  else
    {
      printf("error in reading arrow1 \n");
    }

  // returns flags 1
 flags1 = dwg_ent_dim_ang2ln_get_flags1(dim_ang2ln,
                &flags1_error);
  if(flags1_error == 0 )
    {  
      printf("flags1 of dim_ang2ln : %d\t\n",flags1);
    }
  else
    {
      printf("error in reading flags1 \n");
    }

  // returns actual measurement
  act_measure = dwg_ent_dim_ang2ln_get_act_measurement(dim_ang2ln,
                &act_error);
  if(act_error == 0 ) // error check
    {  
      printf("act_measurement of dim_ang2ln : %f\t\n",act_measure);
    }
  else
    {
      printf("error in reading act_measurement \n");
    }
}
