#define DWG_TYPE DWG_TYPE_DIMENSION_RADIUS
#include "common.c"

void
low_level_process(dwg_object *obj)
{
  // casts dwg object to dimension entity
  dwg_ent_dim_radius *dim_radius = dwg_object_to_DIMENSION_RADIUS(obj);

  // prints horiz dir
  printf("horiz dir of dim_radius : %f\n",
          dim_radius->horiz_dir);

  // prints lspace factor
  printf("lspace factor of dim_radius : %f\n", 
          dim_radius->lspace_factor);

  // prints lspace style
  printf("lspace style of dim_radius : " FORMAT_BS "\n",
          dim_radius->lspace_style);

  // prints attach point
  printf("attach point of dim_radius : " FORMAT_BS "\n",
          dim_radius->attachment_point);

  // prints radius
  printf("Radius of dim_radius : %f\n",
          dim_radius->elevation.ecs_11);

  // prints thickness
  printf("Thickness of dim_radius : %f\n",
          dim_radius->elevation.ecs_12);

  // prints extrusion points
  printf("extrusion of dim_radius : x = %f, y = %f, z = %f\n",
          dim_radius->extrusion.x, dim_radius->extrusion.y,
          dim_radius->extrusion.z);
 
  // prints insertion scale points
  printf("ins_scale of dim_radius : x = %f, y = %f, z = %f\n",
          dim_radius->ins_scale.x, dim_radius->ins_scale.y,
         dim_radius->ins_scale.z);

  // prints text mid point
  printf("text_mid_pt of dim_radius : x = %f, y = %f\n",
          dim_radius->text_midpt.x, dim_radius->text_midpt.y);

  // prints user text
  printf("user text of dim_radius : %s\n", dim_radius->user_text);

  // prints text rotation
  printf("text rotation of dim_radius : %f\n", dim_radius->text_rot);

  // prints insert rotation
  printf("ins rotation of dim_radius : %f\n", dim_radius->ins_rotation);

  // prints arrow 1
  printf("arrow 1 of dim_radius : " FORMAT_BS "\n", dim_radius->flip_arrow1);

  // prints arrow 2
  printf("arrow 2 of dim_radius : " FORMAT_BS "\n", dim_radius->flip_arrow2);

  // prints flags 2
  printf("flags2 of dim_radius : " FORMAT_BS "\n", dim_radius->flags_1);

  // prints actual measurement
  printf("act_measurement of dim_radius : %f\n",
          dim_radius->act_measurement); 

  // prints leader length
  printf("leader length of dim radius : %f\n", dim_radius->leader_len);
}

void
api_process(dwg_object *obj)
{
  int error;
  double ecs11, ecs12, act_measure, horiz_dir, lspace_factor, text_rot, 
         ins_rot, ext_line_rot, dim_rot, length ;
  BITCODE_RC flags1, flags2, flip_arrow1, flip_arrow2;
  BITCODE_BS lspace_style, attach_pt;
  char * user_text;
  dwg_point_2d text_mid_pt;
  dwg_point_3d ext, ins_scale;
  dwg_ent_dim_radius *dim_radius = dwg_object_to_DIMENSION_RADIUS(obj);

  // returns horiz dir 
  horiz_dir = dwg_ent_dim_radius_get_horiz_dir(dim_radius, &error);
  if ( !error )
    {
      printf("horiz dir of dim_radius : %f\n", horiz_dir);
    }
  else
    {
      printf("error in reading horiz dir \n");
    }

  // returns lspace factor
  lspace_factor = dwg_ent_dim_radius_get_elevation_ecs11(dim_radius, 
                  &error);
  if ( !error )
    {  
      printf("lspace factor of dim_radius : %f\n", lspace_factor);
    }
  else
    {
      printf("error in reading lspace factor \n");
    }

  // returns lspace style
  lspace_style = dwg_ent_dim_radius_get_elevation_ecs11(dim_radius, 
                  &error);
  if ( !error )
    {  
      printf("lspace style of dim_radius : " FORMAT_BS "\n", lspace_style);
    }
  else
    {
      printf("error in reading lspace style \n");
    }

  // returns attach point
  attach_pt = dwg_ent_dim_radius_get_elevation_ecs11(dim_radius, 
              &error);
  if ( !error )
    {  
      printf("attach point of dim_radius : " FORMAT_BS "\n", attach_pt);
    }
  else
    {
      printf("error in reading attach point \n");
    }

  // returns ecs 11 elevation
  ecs11 = dwg_ent_dim_radius_get_elevation_ecs11(dim_radius, &error);
  if ( !error )
    {  
      printf("Radius of dim_radius : %f\n",ecs11);
    }
  else
    {
      printf("error in reading ecs11 \n");
    }

  // returns ecs 12 elevation
  ecs12 = dwg_ent_dim_radius_get_elevation_ecs12(dim_radius, &error);
  if ( !error )
    {
      printf("Thickness of dim_radius : %f\n",ecs12);
    }
  else
    {
      printf("error in reading ecs12 \n");
    }

  // returns extrusion points
  dwg_ent_dim_radius_get_extrusion(dim_radius, &ext,
                                   &error);
  if ( !error )
    {
      printf("extrusion of dim_radius : x = %f, y = %f, z = %f\n",
              ext.x, ext.y, ext.z);
    }
  else
    {
      printf("error in reading extrusion \n");
    }

  // returns insertion scale
  dwg_ent_dim_radius_get_ins_scale(dim_radius, &ins_scale,
                                   &error);
  if ( !error )
    {
      printf("ins_scale of dim_radius : x = %f, y = %f, z = %f\n",
              ins_scale.x, ins_scale.y, ins_scale.z);
    }
  else
    {
      printf("error in reading ins_scale \n");
    }

  // returns text mid point
  dwg_ent_dim_radius_get_text_mid_pt(dim_radius, &text_mid_pt,
                                          &error);
  if ( !error )
    {
      printf("text_mid_pt of dim_radius : x = %f, y = %f\n",
              text_mid_pt.x, text_mid_pt.y);
    }
  else
    {
      printf("error in reading text_mid_pt \n");
    }

  // returns user text
  user_text = dwg_ent_dim_radius_get_user_text(dim_radius, &error);
  if ( !error )
    {
      printf("user text of dim_radius : %s\n",user_text);
    }
  else
    {
      printf("error in reading user_text \n");
    }

  // returns text rotation
  text_rot = dwg_ent_dim_radius_get_text_rot(dim_radius, &error);
  if ( !error )
    {
      printf(" text rotation of dim_radius : %f\n", text_rot);
    }
  else
    {
      printf("error in reading text rotation \n");
    }

  // returns insert rotation
  ins_rot = dwg_ent_dim_radius_get_ins_rotation(dim_radius, &error);
  if ( !error )
    {  
      printf("ins rotation of dim_radius : %f\n",ins_rot);
    }
  else
    {
      printf("error in reading ins rotation \n");
    }

  // returns flip arrow 1
  flip_arrow1 = dwg_ent_dim_radius_get_flip_arrow1(dim_radius,
                &error);
  if ( !error )
    {  
      printf("arrow1 of dim_radius : " FORMAT_BS "\n",flip_arrow1);
    }
  else
    {
      printf("error in reading arrow1 \n");
    }

  // returns flip arrow 2
  flip_arrow2 = dwg_ent_dim_radius_get_flip_arrow2(dim_radius,
                &error);
  if ( !error )
    {  
      printf("arrow1 of dim_radius : " FORMAT_BS "\n",flip_arrow2);
    }
  else
    {
      printf("error in reading arrow1 \n");
    }

  // returns flag 1
  flags1 = dwg_ent_dim_radius_get_flags1(dim_radius,
                &error);
  if ( !error )
    {  
      printf("flags1 of dim_radius : " FORMAT_BS "\n",flags1);
    }
  else
    {
      printf("error in reading flags1 \n");
    }

  // returns actual measurement
  act_measure = dwg_ent_dim_radius_get_act_measurement(dim_radius,
                &error);
  if ( !error )
    {  
      printf("act_measurement of dim_radius : %f\n",act_measure);
    }
  else
    {
      printf("error in reading act_measurement \n");
    }

  // returns length
  length = dwg_ent_dim_radius_get_leader_length(dim_radius, &error);
  if ( !error )
    {  
       printf("leader length of dim radius : %f\n", length);
    }
  else
    {
      printf("error in reading leader length \n");
    } 
}
