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

  if (dwg_get_type(obj)== DWG_TYPE_DIMENSION_RADIUS)
    {
      output_process(obj);
    }
}

void
low_level_process(dwg_object *obj)
{
  dwg_ent_dim_radius *dim_radius = dwg_object_to_DIMENSION_RADIUS(obj);
      printf("horiz dir of dim_radius : %f\t\n",
              dim_radius->horiz_dir);
      printf("lspace factor of dim_radius : %f\t\n", 
              dim_radius->lspace_factor);
      printf("lspace style of dim_radius : %d\t\n",
              dim_radius->lspace_style);
      printf("attach point of dim_radius : %d\t\n",
              dim_radius->attachment_point);
      printf("Radius of dim_radius : %f\t\n",
              dim_radius->elevation.ecs_11);
      printf("Thickness of dim_radius : %f\t\n",
              dim_radius->elevation.ecs_12);
      printf("extrusion of dim_radius : x = %f, y = %f, z = %f\t\n",
              dim_radius->extrusion.x, dim_radius->extrusion.y,
              dim_radius->extrusion.z);
      printf("ins_scale of dim_radius : x = %f, y = %f, z = %f\t\n",
              dim_radius->ins_scale.x, dim_radius->ins_scale.y,
	      dim_radius->ins_scale.z);
      printf("text_mid_pt of dim_radius : x = %f, y = %f\t\n",
              dim_radius->text_midpt.x, dim_radius->text_midpt.y);
      printf("user text of dim_radius : %s\t\n", dim_radius->user_text);
      printf("text rotation of dim_radius : %f\t\n", dim_radius->text_rot);
      printf("ins rotation of dim_radius : %f\t\n", dim_radius->ins_rotation);
      printf("arrow1 of dim_radius : %d\t\n", dim_radius->flip_arrow1);
      printf("arrow1 of dim_radius : %d\t\n", dim_radius->flip_arrow2);
      printf("flags2 of dim_radius : %d\t\n", dim_radius->flags_1);
      printf("act_measurement of dim_radius : %f\t\n",
              dim_radius->act_measurement); 
      printf("leader length of dim radius : %f\t\n", dim_radius->leader_len);
}

void
api_process(dwg_object *obj)
{
  int ecs11_error, ecs12_error, flags1_error, act_error, horiz_error, lspace_error,
      style_error, att_point_error, ext_error, user_text_error, text_rot_error, 
      ins_rot_error, arrow1_error, arrow2_error, mid_pt_error, ins_scale_error, 
      flags2_error, dim_rot_error, ext_line_rot_error, length_error;
  double ecs11, ecs12, act_measure, horiz_dir, lspace_factor, text_rot, 
         ins_rot, ext_line_rot, dim_rot, length ;
  unsigned int flags1, lspace_style, attach_pt, flip_arrow1, flip_arrow2, 
               flags2;
  char * user_text;
  dwg_point_2d text_mid_pt;
  dwg_point_3d ext, ins_scale;
  dwg_ent_dim_radius *dim_radius = dwg_object_to_DIMENSION_RADIUS(obj);

  horiz_dir = dwg_ent_dim_radius_get_horiz_dir(dim_radius, &horiz_error);
  if(horiz_error == 0 )
    {  
      printf("horiz dir of dim_radius : %f\t\n", horiz_dir);
    }
  else
    {
      printf("error in reading horiz dir \n");
    }

  lspace_factor = dwg_ent_dim_radius_get_elevation_ecs11(dim_radius, 
                  &lspace_error);
  if(lspace_error == 0 )
    {  
      printf("lspace factor of dim_radius : %f\t\n", lspace_factor);
    }
  else
    {
      printf("error in reading lspace factor \n");
    }

  lspace_style = dwg_ent_dim_radius_get_elevation_ecs11(dim_radius, 
                  &style_error);
  if(style_error == 0 )
    {  
      printf("lspace style of dim_radius : %d\t\n", lspace_style);
    }
  else
    {
      printf("error in reading lspace style \n");
    }

  attach_pt = dwg_ent_dim_radius_get_elevation_ecs11(dim_radius, 
              &att_point_error);
  if(att_point_error == 0 )
    {  
      printf("attach point of dim_radius : %d\t\n", attach_pt);
    }
  else
    {
      printf("error in reading attach point \n");
    }

  ecs11 = dwg_ent_dim_radius_get_elevation_ecs11(dim_radius, &ecs11_error);
  if(ecs11_error == 0 )
    {  
      printf("Radius of dim_radius : %f\t\n",ecs11);
    }
  else
    {
      printf("error in reading ecs11 \n");
    }

  ecs12 = dwg_ent_dim_radius_get_elevation_ecs12(dim_radius, &ecs12_error);
  if(ecs12_error == 0 )
    {
      printf("Thickness of dim_radius : %f\t\n",ecs12);
    }
  else
    {
      printf("error in reading ecs12 \n");
    }

  dwg_ent_dim_radius_get_extrusion(dim_radius, &ext,&ext_error);
  if(ext_error == 0 )
    {
      printf("extrusion of dim_radius : x = %f, y = %f, z = %f\t\n",
              ext.x, ext.y, ext.z);
    }
  else
    {
      printf("error in reading extrusion \n");
    }

  dwg_ent_dim_radius_get_ins_scale(dim_radius, &ins_scale,&ins_scale_error);
  if(ins_scale_error == 0 )
    {
      printf("ins_scale of dim_radius : x = %f, y = %f, z = %f\t\n",
              ins_scale.x, ins_scale.y, ins_scale.z);
    }
  else
    {
      printf("error in reading ins_scale \n");
    }
  dwg_ent_dim_radius_get_text_mid_pt(dim_radius, &text_mid_pt,
                                          &mid_pt_error);
  if(mid_pt_error == 0 )
    {
      printf("text_mid_pt of dim_radius : x = %f, y = %f\t\n",
              text_mid_pt.x, text_mid_pt.y);
    }
  else
    {
      printf("error in reading text_mid_pt \n");
    }
  user_text = dwg_ent_dim_radius_get_user_text(dim_radius, &user_text_error);
  if(user_text_error == 0 )
    {  
      printf("user text of dim_radius : %s\t\n",user_text);
    }
  else
    {
      printf("error in reading user_text \n");
    }
  text_rot = dwg_ent_dim_radius_get_text_rot(dim_radius, &text_rot_error);
  if(text_rot_error == 0 )
    {  
      printf(" text rotation of dim_radius : %f\t\n", text_rot);
    }
  else
    {
      printf("error in reading text rotation \n");
    }
  ins_rot = dwg_ent_dim_radius_get_ins_rotation(dim_radius, &ins_rot_error);
  if(ins_rot_error == 0 )
    {  
      printf("ins rotation of dim_radius : %f\t\n",ins_rot);
    }
  else
    {
      printf("error in reading ins rotation \n");
    }
  flip_arrow1 = dwg_ent_dim_radius_get_flip_arrow1(dim_radius,
                &arrow1_error);
  if(arrow1_error == 0 )
    {  
      printf("arrow1 of dim_radius : %d\t\n",flip_arrow1);
    }
  else
    {
      printf("error in reading arrow1 \n");
    }
  flip_arrow2 = dwg_ent_dim_radius_get_flip_arrow2(dim_radius,
                &arrow2_error);
  if(arrow2_error == 0 )
    {  
      printf("arrow1 of dim_radius : %d\t\n",flip_arrow2);
    }
  else
    {
      printf("error in reading arrow1 \n");
    }
 flags1 = dwg_ent_dim_radius_get_flags1(dim_radius,
                &flags1_error);
  if(arrow2_error == 0 )
    {  
      printf("flags1 of dim_radius : %d\t\n",flags1);
    }
  else
    {
      printf("error in reading flags1 \n");
    }
  act_measure = dwg_ent_dim_radius_get_act_measurement(dim_radius,
                &act_error);
  if(act_error == 0 )
    {  
      printf("act_measurement of dim_radius : %f\t\n",act_measure);
    }
  else
    {
      printf("error in reading act_measurement \n");
    }
  length = dwg_ent_dim_radius_get_leader_length(dim_radius, &length_error);
  if(length_error == 0 )
    {  
       printf("leader length of dim radius : %f\t\n", length);
    }
  else
    {
      printf("error in reading leader length \n");
    } 
}
