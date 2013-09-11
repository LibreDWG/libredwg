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

  if (dwg_get_type(obj)== DWG_TYPE_DIMENSION_DIAMETER)
    {
      output_process(obj);
    }
}

void
low_level_process(dwg_object *obj)
{
  dwg_ent_dim_diameter *dim_diameter = dwg_object_to_DIMENSION_DIAMETER(obj);
      printf("horiz dir of dim_diameter : %f\t\n",
              dim_diameter->horiz_dir);
      printf("lspace factor of dim_diameter : %f\t\n", 
              dim_diameter->lspace_factor);
      printf("lspace style of dim_diameter : %d\t\n",
              dim_diameter->lspace_style);
      printf("attach point of dim_diameter : %d\t\n",
              dim_diameter->attachment_point);
      printf("Radius of dim_diameter : %f\t\n",
              dim_diameter->elevation.ecs_11);
      printf("Thickness of dim_diameter : %f\t\n",
              dim_diameter->elevation.ecs_12);
      printf("extrusion of dim_diameter : x = %f, y = %f, z = %f\t\n",
              dim_diameter->extrusion.x, dim_diameter->extrusion.y,
              dim_diameter->extrusion.z);
      printf("ins_scale of dim_diameter : x = %f, y = %f, z = %f\t\n",
              dim_diameter->ins_scale.x, dim_diameter->ins_scale.y,
	      dim_diameter->ins_scale.z);
      printf("pt10 of dim_diameter : x = %f, y = %f, z = %f\t\n",
              dim_diameter->_10_pt.x, dim_diameter->_10_pt.y,
              dim_diameter->_10_pt.z);
      printf("pt15 of dim_diameter : x = %f, y = %f, z = %f\t\n",
              dim_diameter->_15_pt.x, dim_diameter->_15_pt.y,
              dim_diameter->_15_pt.z);
      printf("pt12 of dim_diameter : x = %f, y = %f\t\n",
              dim_diameter->_12_pt.x, dim_diameter->_12_pt.y);
      printf("text_mid_pt of dim_diameter : x = %f, y = %f\t\n",
              dim_diameter->text_midpt.x, dim_diameter->text_midpt.y);
      printf("user text of dim_diameter : %s\t\n", dim_diameter->user_text);
      printf("text rotation of dim_diameter : %f\t\n", dim_diameter->text_rot);
      printf("ins rotation of dim_diameter : %f\t\n", dim_diameter->ins_rotation);
      printf("arrow1 of dim_diameter : %d\t\n", dim_diameter->flip_arrow1);
      printf("arrow1 of dim_diameter : %d\t\n", dim_diameter->flip_arrow2);
      printf("flags2 of dim_diameter : %d\t\n", dim_diameter->flags_1);
      printf("act_measurement of dim_diameter : %f\t\n",
              dim_diameter->act_measurement); 
      printf("leader length of dim diameter : %f\t\n", dim_diameter->leader_len);
}

void
api_process(dwg_object *obj)
{
  int ecs11_error, ecs12_error, flags1_error, act_error, horiz_error, lspace_error,
      style_error, att_point_error, ext_error, user_text_error, text_rot_error, 
      ins_rot_error, arrow1_error, arrow2_error, mid_pt_error, ins_scale_error, 
      flags2_error, pt10_error, pt12_error, dim_rot_error, ext_line_rot_error,
      pt15_error, length_error;
  double ecs11, ecs12, act_measure, horiz_dir, lspace_factor, text_rot, 
         ins_rot, ext_line_rot, dim_rot, length;
  unsigned int flags1, lspace_style, attach_pt, flip_arrow1, flip_arrow2, 
               flags2;
  char * user_text;
  dwg_point_2d text_mid_pt, pt12, pt16;
  dwg_point_3d pt10, pt15, ext, ins_scale;
  dwg_ent_dim_diameter *dim_diameter = dwg_object_to_DIMENSION_DIAMETER(obj);

  horiz_dir = dwg_ent_dim_diameter_get_horiz_dir(dim_diameter, &horiz_error);
  if(horiz_error == 0 )
    {  
      printf("horiz dir of dim_diameter : %f\t\n", horiz_dir);
    }
  else
    {
      printf("error in reading horiz dir \n");
    }

  lspace_factor = dwg_ent_dim_diameter_get_elevation_ecs11(dim_diameter, 
                  &lspace_error);
  if(lspace_error == 0 )
    {  
      printf("lspace factor of dim_diameter : %f\t\n", lspace_factor);
    }
  else
    {
      printf("error in reading lspace factor \n");
    }

  lspace_style = dwg_ent_dim_diameter_get_elevation_ecs11(dim_diameter, 
                  &style_error);
  if(style_error == 0 )
    {  
      printf("lspace style of dim_diameter : %d\t\n", lspace_style);
    }
  else
    {
      printf("error in reading lspace style \n");
    }

  attach_pt = dwg_ent_dim_diameter_get_elevation_ecs11(dim_diameter, 
              &att_point_error);
  if(att_point_error == 0 )
    {  
      printf("attach point of dim_diameter : %d\t\n", attach_pt);
    }
  else
    {
      printf("error in reading attach point \n");
    }

  ecs11 = dwg_ent_dim_diameter_get_elevation_ecs11(dim_diameter, &ecs11_error);
  if(ecs11_error == 0 )
    {  
      printf("Radius of dim_diameter : %f\t\n",ecs11);
    }
  else
    {
      printf("error in reading ecs11 \n");
    }

  ecs12 = dwg_ent_dim_diameter_get_elevation_ecs12(dim_diameter, &ecs12_error);
  if(ecs12_error == 0 )
    {
      printf("Thickness of dim_diameter : %f\t\n",ecs12);
    }
  else
    {
      printf("error in reading ecs12 \n");
    }

  dwg_ent_dim_diameter_get_extrusion(dim_diameter, &ext,&ext_error);
  if(ext_error == 0 )
    {
      printf("extrusion of dim_diameter : x = %f, y = %f, z = %f\t\n",
              ext.x, ext.y, ext.z);
    }
  else
    {
      printf("error in reading extrusion \n");
    }

  dwg_ent_dim_diameter_get_ins_scale(dim_diameter, &ins_scale,&ins_scale_error);
  if(ins_scale_error == 0 )
    {
      printf("ins_scale of dim_diameter : x = %f, y = %f, z = %f\t\n",
              ins_scale.x, ins_scale.y, ins_scale.z);
    }
  else
    {
      printf("error in reading ins_scale \n");
    }
  dwg_ent_dim_diameter_get_10_pt(dim_diameter, &pt10,&pt10_error);
  if(pt10_error == 0 )
    {
      printf("pt10 of dim_diameter : x = %f, y = %f, z = %f\t\n",
              pt10.x, pt10.y, pt10.z);
    }
  else
    {
      printf("error in reading pt10 \n");
    }
 dwg_ent_dim_diameter_get_15_pt(dim_diameter, &pt15, &pt15_error);
  if(pt15_error == 0 )
    {
      printf("pt15 of dim_diameter : x = %f, y = %f, z = %f\t\n",
              pt15.x, pt15.y, pt15.z);
    }
  else
    {
      printf("error in reading pt14 \n");
    }
   dwg_ent_dim_diameter_get_12_pt(dim_diameter, &pt12,&pt12_error);
  if(pt12_error == 0 )
    {
      printf("pt12 of dim_diameter : x = %f, y = %f\t\n",
              pt12.x, pt12.y);
    }
  else
    {
      printf("error in reading pt12 \n");
    }
  dwg_ent_dim_diameter_get_text_mid_pt(dim_diameter, &text_mid_pt,
                                          &mid_pt_error);
  if(mid_pt_error == 0 )
    {
      printf("text_mid_pt of dim_diameter : x = %f, y = %f\t\n",
              text_mid_pt.x, text_mid_pt.y);
    }
  else
    {
      printf("error in reading text_mid_pt \n");
    }
  user_text = dwg_ent_dim_diameter_get_user_text(dim_diameter, &user_text_error);
  if(user_text_error == 0 )
    {  
      printf("user text of dim_diameter : %s\t\n",user_text);
    }
  else
    {
      printf("error in reading user_text \n");
    }
  text_rot = dwg_ent_dim_diameter_get_text_rot(dim_diameter, &text_rot_error);
  if(text_rot_error == 0 )
    {  
      printf(" text rotation of dim_diameter : %f\t\n", text_rot);
    }
  else
    {
      printf("error in reading text rotation \n");
    }
  ins_rot = dwg_ent_dim_diameter_get_ins_rotation(dim_diameter, &ins_rot_error);
  if(ins_rot_error == 0 )
    {  
      printf("ins rotation of dim_diameter : %f\t\n",ins_rot);
    }
  else
    {
      printf("error in reading ins rotation \n");
    }
  flip_arrow1 = dwg_ent_dim_diameter_get_flip_arrow1(dim_diameter,
                &arrow1_error);
  if(arrow1_error == 0 )
    {  
      printf("arrow1 of dim_diameter : %d\t\n",flip_arrow1);
    }
  else
    {
      printf("error in reading arrow1 \n");
    }
  flip_arrow2 = dwg_ent_dim_diameter_get_flip_arrow2(dim_diameter,
                &arrow2_error);
  if(arrow2_error == 0 )
    {  
      printf("arrow1 of dim_diameter : %d\t\n",flip_arrow2);
    }
  else
    {
      printf("error in reading arrow1 \n");
    }
 flags1 = dwg_ent_dim_diameter_get_flags1(dim_diameter,
                &flags1_error);
  if(arrow2_error == 0 )
    {  
      printf("flags1 of dim_diameter : %d\t\n",flags1);
    }
  else
    {
      printf("error in reading flags1 \n");
    }
  act_measure = dwg_ent_dim_diameter_get_act_measurement(dim_diameter,
                &act_error);
  if(act_error == 0 )
    {  
      printf("act_measurement of dim_diameter : %f\t\n",act_measure);
    }
  else
    {
      printf("error in reading act_measurement \n");
    }
  length = dwg_ent_dim_diameter_get_leader_length(dim_diameter, &length_error);
  if(length_error == 0 )
    {  
       printf("leader length of dim diameter : %f\t\n", length);
    }
  else
    {
      printf("error in reading leader length \n");
    } 
}
