#define DWG_TYPE DWG_TYPE_DIMENSION_ANG3PT
#include "common.c"

void
low_level_process(dwg_object *obj)
{
  // casts dwg object to ang3pt
  dwg_ent_dim_ang3pt *dim_ang3pt = dwg_object_to_DIMENSION_ANG3PT(obj);

  // prints horiz dir
  printf("horiz dir of dim_ang3pt : %f\n",
          dim_ang3pt->horiz_dir);

  // prints lspace factor
  printf("lspace factor of dim_ang3pt : %f\n", 
          dim_ang3pt->lspace_factor);

  // prints lspace style
  printf("lspace style of dim_ang3pt : " FORMAT_BS "\n",
          dim_ang3pt->lspace_style);

  // prints attach point
  printf("attach point of dim_ang3pt : " FORMAT_BS "\n",
          dim_ang3pt->attachment_point);

  // prints radius
  printf("Radius of dim_ang3pt : %f\n",
          dim_ang3pt->elevation.ecs_11);

  // prints thickness
  printf("Thickness of dim_ang3pt : %f\n",
          dim_ang3pt->elevation.ecs_12);

  // prints extursion
  printf("extrusion of dim_ang3pt : x = %f, y = %f, z = %f\n",
          dim_ang3pt->extrusion.x, dim_ang3pt->extrusion.y,
          dim_ang3pt->extrusion.z);

  // prints insert scale
  printf("ins_scale of dim_ang3pt : x = %f, y = %f, z = %f\n",
          dim_ang3pt->ins_scale.x, dim_ang3pt->ins_scale.y,
          dim_ang3pt->ins_scale.z);

  // prints 10 point
  printf("pt10 of dim_ang3pt : x = %f, y = %f, z = %f\n",
          dim_ang3pt->_10_pt.x, dim_ang3pt->_10_pt.y,
          dim_ang3pt->_10_pt.z);

  // prints 13 pt
  printf("pt13 of dim_ang3pt : x = %f, y = %f, z = %f\n",
          dim_ang3pt->_13_pt.x, dim_ang3pt->_13_pt.y,
          dim_ang3pt->_13_pt.z);

  // prints 14 pt
  printf("pt14 of dim_ang3pt : x = %f, y = %f, z = %f\n",
          dim_ang3pt->_14_pt.x, dim_ang3pt->_14_pt.y,
          dim_ang3pt->_14_pt.z);

  // prints 15 pt
  printf("pt15 of dim_ang3pt : x = %f, y = %f, z = %f\n",
          dim_ang3pt->_15_pt.x, dim_ang3pt->_15_pt.y,
          dim_ang3pt->_15_pt.z);

  // prints 12 pt 
  printf("pt12 of dim_ang3pt : x = %f, y = %f\n",
          dim_ang3pt->_12_pt.x, dim_ang3pt->_12_pt.y);

  // prints text mid point
  printf("text_mid_pt of dim_ang3pt : x = %f, y = %f\n",
          dim_ang3pt->text_midpt.x, dim_ang3pt->text_midpt.y);

  // prints user text
  printf("user text of dim_ang3pt : %s\n", dim_ang3pt->user_text);

  // prints text rotation
  printf("text rotation of dim_ang3pt : %f\n", dim_ang3pt->text_rot);

  // prints insert rotation
  printf("ins rotation of dim_ang3pt : %f\n", dim_ang3pt->ins_rotation);

  // prints arrow 1
  printf("arrow1 of dim_ang3pt : " FORMAT_BS "\n", dim_ang3pt->flip_arrow1);

  // prints arrow 2
  printf("arrow2 of dim_ang3pt : " FORMAT_BS "\n", dim_ang3pt->flip_arrow2);

  // prints flags 2
  printf("flags2 of dim_ang3pt : " FORMAT_BS "\n", dim_ang3pt->flags_1);

  // prints act measurement
  printf("act_measurement of dim_ang3pt : %f\n",
          dim_ang3pt->act_measurement); 
}

void
api_process(dwg_object *obj)
{
  int error;
  double ecs11, ecs12, act_measure, horiz_dir, lspace_factor, text_rot, 
         ins_rot, ext_line_rot, dim_rot;
  BITCODE_RC flags1, flip_arrow1, flip_arrow2, flags2;
  BITCODE_BS lspace_style, attach_pt;
  char * user_text;
  dwg_point_2d text_mid_pt, pt12;
  dwg_point_3d pt10, pt13, pt14, pt15, ext, ins_scale;
  dwg_ent_dim_ang3pt *dim_ang3pt = dwg_object_to_DIMENSION_ANG3PT(obj);

  horiz_dir = dwg_ent_dim_ang3pt_get_horiz_dir(dim_ang3pt, &error);
  if ( !error )
    printf("horiz dir of dim_ang3pt : %f\n", horiz_dir);
  else
    printf("error in reading horiz dir \n");

  // returns lspace factor
  lspace_factor = dwg_ent_dim_ang3pt_get_elevation_ecs11(dim_ang3pt, 
                  &error);
  if ( !error )
    {  
      printf("lspace factor of dim_ang3pt : %f\n", lspace_factor);
    }
  else
    {
      printf("error in reading lspace factor \n");
    }

  // returns lspace style
  lspace_style = dwg_ent_dim_ang3pt_get_elevation_ecs11(dim_ang3pt, 
                  &error);
  if ( !error )
    {  
      printf("lspace style of dim_ang3pt : " FORMAT_BS "\n", lspace_style);
    }
  else
    {
      printf("error in reading lspace style \n");
    }

  // returns attach point
  attach_pt = dwg_ent_dim_ang3pt_get_elevation_ecs11(dim_ang3pt, 
              &error);
  if ( !error )
    {  
      printf("attach point of dim_ang3pt : " FORMAT_BS "\n", attach_pt);
    }
  else
    {
      printf("error in reading attach point \n");
    }

  // returns elevation ecs 11
  ecs11 = dwg_ent_dim_ang3pt_get_elevation_ecs11(dim_ang3pt, &error);
  if ( !error )
    {  
      printf("Radius of dim_ang3pt : %f\n",ecs11);
    }
  else
    {
      printf("error in reading ecs11 \n");
    }

  // returns elevation ecs 12
  ecs12 = dwg_ent_dim_ang3pt_get_elevation_ecs12(dim_ang3pt, &error);
  if ( !error )
    {
      printf("Thickness of dim_ang3pt : %f\n",ecs12);
    }
  else
    {
      printf("error in reading ecs12 \n");
    }

  // returns extrusion points
  dwg_ent_dim_ang3pt_get_extrusion(dim_ang3pt, &ext,
                                   &error);
  if ( !error )
    {
      printf("extrusion of dim_ang3pt : x = %f, y = %f, z = %f\n",
              ext.x, ext.y, ext.z);
    }
  else
    {
      printf("error in reading extrusion \n");
    }

  dwg_ent_dim_ang3pt_get_ins_scale(dim_ang3pt, &ins_scale,
                                   &error);
  if ( !error )
    {
      printf("ins_scale of dim_ang3pt : x = %f, y = %f, z = %f\n",
              ins_scale.x, ins_scale.y, ins_scale.z);
    }
  else
    {
      printf("error in reading ins_scale \n");
    }

  // returns 10 point
  dwg_ent_dim_ang3pt_get_10_pt(dim_ang3pt, &pt10,
                               &error);
  if ( !error )
    {
      printf("pt10 of dim_ang3pt : x = %f, y = %f, z = %f\n",
              pt10.x, pt10.y, pt10.z);
    }
  else
    {
      printf("error in reading pt10 \n");
    }

  // returns 13 point
  dwg_ent_dim_ang3pt_get_13_pt(dim_ang3pt, &pt13,
                               &error);
  if ( !error )
    {
      printf("pt13 of dim_ang3pt : x = %f, y = %f, z = %f\n",
              pt13.x, pt13.y, pt13.z);
    }
  else
    {
      printf("error in reading pt13 \n");
    }

  // returns 14 point
  dwg_ent_dim_ang3pt_get_14_pt(dim_ang3pt, &pt14,
                               &error);
  if ( !error )
    {
      printf("pt14 of dim_ang3pt : x = %f, y = %f, z = %f\n",
              pt14.x, pt14.y, pt14.z);
    }
  else
    {
      printf("error in reading pt14 \n");
    }

  // returns 15 point
  dwg_ent_dim_ang3pt_get_15_pt(dim_ang3pt, &pt15,
                               &error);
  if ( !error )
    {
      printf("pt15 of dim_ang3pt : x = %f, y = %f, z = %f\n",
              pt15.x, pt15.y, pt15.z);
    }
  else
    {
      printf("error in reading pt14 \n");
    }

  // returns 12 point
  dwg_ent_dim_ang3pt_get_12_pt(dim_ang3pt, &pt12,
                               &error);
  if ( !error )
    {
      printf("pt12 of dim_ang3pt : x = %f, y = %f\n",
              pt12.x, pt12.y);
    }
  else
    {
      printf("error in reading pt12 \n");
    }

  // returns text mid point
  dwg_ent_dim_ang3pt_get_text_mid_pt(dim_ang3pt, &text_mid_pt,
                                          &error);
  if ( !error )
    {
      printf("text_mid_pt of dim_ang3pt : x = %f, y = %f\n",
              text_mid_pt.x, text_mid_pt.y);
    }
  else
    {
      printf("error in reading text_mid_pt \n");
    }

  // returns user text
  user_text = dwg_ent_dim_ang3pt_get_user_text(dim_ang3pt, &error);
  if ( !error )
    {  
      printf("user text of dim_ang3pt : %s\n",user_text);
    }
  else
    {
      printf("error in reading user_text \n");
    }

  // returns text rotation
  text_rot = dwg_ent_dim_ang3pt_get_text_rot(dim_ang3pt, &error);
  if ( !error )
    {  
      printf(" text rotation of dim_ang3pt : %f\n", text_rot);
    }
  else
    {
      printf("error in reading text rotation \n");
    }

  // returns insert rotation
  ins_rot = dwg_ent_dim_ang3pt_get_ins_rotation(dim_ang3pt, &error);
  if ( !error )
    {
      printf("ins rotation of dim_ang3pt : %f\n",ins_rot);
    }
  else
    {
      printf("error in reading ins rotation \n");
    }

  // returns flip arrow 1
  flip_arrow1 = dwg_ent_dim_ang3pt_get_flip_arrow1(dim_ang3pt,
                &error);
  if ( !error )
    {  
      printf("arrow1 of dim_ang3pt : " FORMAT_BS "\n",flip_arrow1);
    }
  else
    {
      printf("error in reading arrow1 \n");
    }

  // returns flip arrow 2
  flip_arrow2 = dwg_ent_dim_ang3pt_get_flip_arrow2(dim_ang3pt,
                &error);
  if ( !error )
    {  
      printf("arrow2 of dim_ang3pt : " FORMAT_BS "\n",flip_arrow2);
    }
  else
    {
      printf("error in reading arrow1 \n");
    }

  // return flag1 
  flags1 = dwg_ent_dim_ang3pt_get_flags1(dim_ang3pt,
                &error);
  if ( !error )
    {  
      printf("flags1 of dim_ang3pt : " FORMAT_BS "\n",flags1);
    }
  else
    {
      printf("error in reading flags1 \n");
    }

  // returns actual measurement
  act_measure = dwg_ent_dim_ang3pt_get_act_measurement(dim_ang3pt,
                &error);
  if ( !error )
    {  
      printf("act_measurement of dim_ang3pt : %f\n",act_measure);
    }
  else
    {
      printf("error in reading act_measurement \n");
    }
}
