#define DWG_TYPE DWG_TYPE_DIMENSION_DIAMETER
#include "common.c"

void
low_level_process(dwg_object *obj)
{

  // casts a dwg object to diameter dimension
  dwg_ent_dim_diameter *dim_diameter = dwg_object_to_DIMENSION_DIAMETER(obj);

  // prints horizontal dir
  printf("horiz dir of dim_diameter : %f\n",
          dim_diameter->horiz_dir);

  // prints lspace factor
  printf("lspace factor of dim_diameter : %f\n", 
          dim_diameter->lspace_factor);

  // prints lspace style
  printf("lspace style of dim_diameter : " FORMAT_BS "\n",
          dim_diameter->lspace_style);

  // prints attachment point
  printf("attach point of dim_diameter : " FORMAT_BS "\n",
          dim_diameter->attachment_point);

  // prints radius of diameter
  printf("Radius of dim_diameter : %f\n",
          dim_diameter->elevation.ecs_11);

  // prints thickness
  printf("Thickness of dim_diameter : %f\n",
          dim_diameter->elevation.ecs_12);

  // prints extrusion
  printf("extrusion of dim_diameter : x = %f, y = %f, z = %f\n",
          dim_diameter->extrusion.x, dim_diameter->extrusion.y,
          dim_diameter->extrusion.z);

  // prints insert scale values
  printf("ins_scale of dim_diameter : x = %f, y = %f, z = %f\n",
          dim_diameter->ins_scale.x, dim_diameter->ins_scale.y,
          dim_diameter->ins_scale.z);

  // prints 10 point
  printf("pt10 of dim_diameter : x = %f, y = %f, z = %f\n",
          dim_diameter->_10_pt.x, dim_diameter->_10_pt.y,
          dim_diameter->_10_pt.z);

  // prints 15 point
  printf("pt15 of dim_diameter : x = %f, y = %f, z = %f\n",
          dim_diameter->_15_pt.x, dim_diameter->_15_pt.y,
          dim_diameter->_15_pt.z);

  // prints 12 point
  printf("pt12 of dim_diameter : x = %f, y = %f\n",
          dim_diameter->_12_pt.x, dim_diameter->_12_pt.y);

  // prints text mid point
  printf("text_mid_pt of dim_diameter : x = %f, y = %f\n",
          dim_diameter->text_midpt.x, dim_diameter->text_midpt.y);

  // prints user text value
  printf("user text of dim_diameter : %s\n", dim_diameter->user_text);

  // prints text rotation
  printf("text rotation of dim_diameter : %f\n", dim_diameter->text_rot);

  // prints rotation of insertion
  printf("ins rotation of dim_diameter : %f\n", dim_diameter->ins_rotation);

  // prints arrow 1
  printf("arrow1 of dim_diameter : " FORMAT_BS "\n", dim_diameter->flip_arrow1);

  // prints arrow 2
  printf("arrow2 of dim_diameter : " FORMAT_BS "\n", dim_diameter->flip_arrow2);

  // prints flags2
  printf("flags2 of dim_diameter : " FORMAT_BS "\n", dim_diameter->flags_1);

  // prints actual measurement
  printf("act_measurement of dim_diameter : %f\n",
          dim_diameter->act_measurement); 

  // prints leader length
  printf("leader length of dim diameter : %f\n", dim_diameter->leader_len);
}

void
api_process(dwg_object *obj)
{
  int error;
  double ecs11, ecs12, act_measure, horiz_dir, lspace_factor, text_rot, 
         ins_rot, ext_line_rot, dim_rot, length;
  BITCODE_RC flags1, flags2, flip_arrow1, flip_arrow2;
  BITCODE_BS lspace_style, attach_pt;
  char * user_text;
  dwg_point_2d text_mid_pt, pt12, pt16;
  dwg_point_3d pt10, pt15, ext, ins_scale;

  dwg_ent_dim_diameter *dim_diameter = dwg_object_to_DIMENSION_DIAMETER(obj);

  // returns horiz dir
  horiz_dir = dwg_ent_dim_diameter_get_horiz_dir(dim_diameter, &error);
  if ( !error )
    {  
      printf("horiz dir of dim_diameter : %f\n", horiz_dir);
    }
  else
    {
      printf("error in reading horiz dir \n");
    }

  // returns lspace factor
  lspace_factor = dwg_ent_dim_diameter_get_elevation_ecs11(dim_diameter, 
                  &error);
  if ( !error )
    {  
      printf("lspace factor of dim_diameter : %f\n", lspace_factor);
    }
  else
    {
      printf("error in reading lspace factor \n");
    }

  // returns lspace style
  lspace_style = dwg_ent_dim_diameter_get_elevation_ecs11(dim_diameter, 
                  &error);
  if ( !error )
    {  
      printf("lspace style of dim_diameter : " FORMAT_BS "\n", lspace_style);
    }
  else
    {
      printf("error in reading lspace style \n");
    }

  // returns attachment point
  attach_pt = dwg_ent_dim_diameter_get_elevation_ecs11(dim_diameter, 
              &error);
  if ( !error )
    {  
      printf("attach point of dim_diameter : " FORMAT_BS "\n", attach_pt);
    }
  else
    {
      printf("error in reading attach point \n");
    }

  // returns ecs 11 elevation
  ecs11 = dwg_ent_dim_diameter_get_elevation_ecs11(dim_diameter, &error);
  if ( !error )
    {  
      printf("Radius of dim_diameter : %f\n",ecs11);
    }
  else
    {
      printf("error in reading ecs11 \n");
    }

  // returns ecs 12 elevation
  ecs12 = dwg_ent_dim_diameter_get_elevation_ecs12(dim_diameter, &error);
  if ( !error )
    {
      printf("Thickness of dim_diameter : %f\n",ecs12);
    }
  else
    {
      printf("error in reading ecs12 \n");
    }

  // returns extrusion points
  dwg_ent_dim_diameter_get_extrusion(dim_diameter, &ext,
                                     &error);
  if ( !error )
    {
      printf("extrusion of dim_diameter : x = %f, y = %f, z = %f\n",
              ext.x, ext.y, ext.z);
    }
  else
    {
      printf("error in reading extrusion \n");
    }

  // returns insertion scale 
  dwg_ent_dim_diameter_get_ins_scale(dim_diameter, &ins_scale,
                                     &error);
  if ( !error )
    {
      printf("ins_scale of dim_diameter : x = %f, y = %f, z = %f\n",
              ins_scale.x, ins_scale.y, ins_scale.z);
    }
  else
    {
      printf("error in reading ins_scale \n");
    }

  // returns 10 point
  dwg_ent_dim_diameter_get_10_pt(dim_diameter, &pt10,
                                 &error);
  if ( !error )
    {
      printf("pt10 of dim_diameter : x = %f, y = %f, z = %f\n",
              pt10.x, pt10.y, pt10.z);
    }
  else
    {
      printf("error in reading pt10 \n");
    }

  // returns 15 point
  dwg_ent_dim_diameter_get_15_pt(dim_diameter, &pt15,
                                 &error);
  if ( !error )
    {
      printf("pt15 of dim_diameter : x = %f, y = %f, z = %f\n",
              pt15.x, pt15.y, pt15.z);
    }
  else
    {
      printf("error in reading pt14 \n");
    }

  // returns 12 point
  dwg_ent_dim_diameter_get_12_pt(dim_diameter, &pt12,
                                 &error);
  if ( !error )
    {
      printf("pt12 of dim_diameter : x = %f, y = %f\n",
              pt12.x, pt12.y);
    }
  else
    {
      printf("error in reading pt12 \n");
    }

  // returns text mid point
  dwg_ent_dim_diameter_get_text_mid_pt(dim_diameter, &text_mid_pt,
                                          &error);
  if ( !error )
    {
      printf("text_mid_pt of dim_diameter : x = %f, y = %f\n",
              text_mid_pt.x, text_mid_pt.y);
    }
  else
    {
      printf("error in reading text_mid_pt \n");
    }

  // returns user text
  user_text = dwg_ent_dim_diameter_get_user_text(dim_diameter, &error);
  if ( !error )
    {  
      printf("user text of dim_diameter : %s\n",user_text);
    }
  else
    {
      printf("error in reading user_text \n");
    }

  // returns text rotation
  text_rot = dwg_ent_dim_diameter_get_text_rot(dim_diameter, &error);
  if ( !error )
    {  
      printf(" text rotation of dim_diameter : %f\n", text_rot);
    }
  else
    {
      printf("error in reading text rotation \n");
    }

  // returns insert rotation
  ins_rot = dwg_ent_dim_diameter_get_ins_rotation(dim_diameter, &error);
  if ( !error )
    {  
      printf("ins rotation of dim_diameter : %f\n",ins_rot);
    }
  else
    {
      printf("error in reading ins rotation \n");
    }

  // returns flip arrow 1
  flip_arrow1 = dwg_ent_dim_diameter_get_flip_arrow1(dim_diameter,
                &error);
  if ( !error )
    {  
      printf("arrow1 of dim_diameter : " FORMAT_BS "\n",flip_arrow1);
    }
  else
    {
      printf("error in reading arrow1 \n");
    }

  // returns flip arrow 2
  flip_arrow2 = dwg_ent_dim_diameter_get_flip_arrow2(dim_diameter,
                &error);
  if ( !error )
    {  
      printf("arrow1 of dim_diameter : " FORMAT_BS "\n",flip_arrow2);
    }
  else
    {
      printf("error in reading arrow1 \n");
    }

  // returns flags1
 flags1 = dwg_ent_dim_diameter_get_flags1(dim_diameter,
                &error);
  if ( !error )
    {  
      printf("flags1 of dim_diameter : " FORMAT_BS "\n",flags1);
    }
  else
    {
      printf("error in reading flags1 \n");
    }

  // returns actual measurement
  act_measure = dwg_ent_dim_diameter_get_act_measurement(dim_diameter,
                &error);
  if ( !error )
    {  
      printf("act_measurement of dim_diameter : %f\n",act_measure);
    }
  else
    {
      printf("error in reading act_measurement \n");
    }

  // returns leader length
  length = dwg_ent_dim_diameter_get_leader_length(dim_diameter, &error);
  if ( !error )
    {  
       printf("leader length of dim diameter : %f\n", length);
    }
  else
    {
      printf("error in reading leader length \n");
    } 
}
