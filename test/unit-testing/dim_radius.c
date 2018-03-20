#define DWG_TYPE DWG_TYPE_DIMENSION_RADIUS
#include "common.c"

void
low_level_process(dwg_object *obj)
{
  dwg_ent_dim_radius *dim_radius = dwg_object_to_DIMENSION_RADIUS(obj);

  printf("horiz dir of dim_radius : %f\n",
          dim_radius->horiz_dir);
  printf("lspace factor of dim_radius : %f\n", 
          dim_radius->lspace_factor);
  printf("lspace style of dim_radius : " FORMAT_BS "\n",
          dim_radius->lspace_style);
  printf("attach point of dim_radius : " FORMAT_BS "\n",
          dim_radius->attachment_point);
  printf("Radius of dim_radius : %f\n",
          dim_radius->elevation.ecs_11);
  printf("Thickness of dim_radius : %f\n",
          dim_radius->elevation.ecs_12);
  printf("extrusion of dim_radius : x = %f, y = %f, z = %f\n",
          dim_radius->extrusion.x, dim_radius->extrusion.y,
          dim_radius->extrusion.z);
  printf("ins_scale of dim_radius : x = %f, y = %f, z = %f\n",
          dim_radius->ins_scale.x, dim_radius->ins_scale.y,
         dim_radius->ins_scale.z);
  printf("text_mid_pt of dim_radius : x = %f, y = %f\n",
          dim_radius->text_midpt.x, dim_radius->text_midpt.y);
  printf("user text of dim_radius : %s\n", dim_radius->user_text);
  printf("text rotation of dim_radius : %f\n", dim_radius->text_rot);
  printf("ins rotation of dim_radius : %f\n", dim_radius->ins_rotation);
  printf("arrow 1 of dim_radius : " FORMAT_B "\n", dim_radius->flip_arrow1);
  printf("arrow 2 of dim_radius : " FORMAT_B "\n", dim_radius->flip_arrow2);
  printf("flags1 of dim_radius : "  FORMAT_RC "\n", dim_radius->flags_1);
  printf("act_measurement of dim_radius : %f\n",
          dim_radius->act_measurement); 
  printf("leader length of dim radius : %f\n", dim_radius->leader_len);
}

void
api_process(dwg_object *obj)
{
  int error;
  double ecs11, ecs12, act_measure, horiz_dir, lspace_factor, text_rot, 
    ins_rot, ext_line_rot, dim_rot, length ;
  BITCODE_B flip_arrow1, flip_arrow2;
  BITCODE_RC flags1;
  BITCODE_BS lspace_style, attach_pt;
  char * user_text;
  dwg_point_2d text_mid_pt;
  dwg_point_3d ext, ins_scale;

  dwg_ent_dim_radius *dim_radius = dwg_object_to_DIMENSION_RADIUS(obj);

  horiz_dir = dwg_ent_dim_radius_get_horiz_dir(dim_radius, &error);
  if ( !error )
    printf("horiz dir of dim_radius : %f\n", horiz_dir);
  else
    printf("in reading horiz dir \n");


  lspace_factor = dwg_ent_dim_radius_get_elevation_ecs11(dim_radius, 
                                                         &error);
  if ( !error )
    printf("lspace factor of dim_radius : %f\n", lspace_factor);
  else
    printf("in reading lspace factor \n");


  lspace_style = dwg_ent_dim_radius_get_elevation_ecs11(dim_radius, 
                                                        &error);
  if ( !error )
    printf("lspace style of dim_radius : " FORMAT_BS "\n", lspace_style);
  else
    printf("in reading lspace style \n");


  attach_pt = dwg_ent_dim_radius_get_elevation_ecs11(dim_radius, 
                                                     &error);
  if ( !error )
    printf("attach point of dim_radius : " FORMAT_BS "\n", attach_pt);
  else
    printf("in reading attach point \n");


  ecs11 = dwg_ent_dim_radius_get_elevation_ecs11(dim_radius, &error);
  if ( !error )
    printf("Radius of dim_radius : %f\n", ecs11);
  else
    printf("in reading ecs11 \n");


  ecs12 = dwg_ent_dim_radius_get_elevation_ecs12(dim_radius, &error);
  if ( !error )
    printf("Thickness of dim_radius : %f\n", ecs12);
  else
    printf("in reading ecs12 \n");


  dwg_ent_dim_radius_get_extrusion(dim_radius, &ext,
                                   &error);
  if ( !error )
    printf("extrusion of dim_radius : x = %f, y = %f, z = %f\n",
           ext.x, ext.y, ext.z);
  else
    printf("error in reading extrusion \n");

  dwg_ent_dim_radius_get_ins_scale(dim_radius, &ins_scale,
                                   &error);
  if ( !error )
    printf("ins_scale of dim_radius : x = %f, y = %f, z = %f\n",
           ins_scale.x, ins_scale.y, ins_scale.z);
  else
    printf("error in reading ins_scale \n");

  dwg_ent_dim_radius_get_text_mid_pt(dim_radius, &text_mid_pt,
                                     &error);
  if ( !error )
    printf("text_mid_pt of dim_radius : x = %f, y = %f\n",
           text_mid_pt.x, text_mid_pt.y);
  else
    printf("error in reading text_mid_pt \n");

  user_text = dwg_ent_dim_radius_get_user_text(dim_radius, &error);
  if ( !error )
    printf("user text of dim_radius : %s\n", user_text);
  else
    printf("in reading user_text \n");


  text_rot = dwg_ent_dim_radius_get_text_rot(dim_radius, &error);
  if ( !error )
    printf(" text rotation of dim_radius : %f\n", text_rot);
  else
    printf("in reading text rotation \n");


  ins_rot = dwg_ent_dim_radius_get_ins_rotation(dim_radius, &error);
  if ( !error )
    printf("ins rotation of dim_radius : %f\n", ins_rot);
  else
    printf("in reading ins rotation \n");


  flip_arrow1 = dwg_ent_dim_radius_get_flip_arrow1(dim_radius,
                                                   &error);
  if ( !error )
    printf("arrow1 of dim_radius : " FORMAT_B "\n", flip_arrow1);
  else
    printf("in reading arrow1 \n");


  flip_arrow2 = dwg_ent_dim_radius_get_flip_arrow2(dim_radius,
                                                   &error);
  if ( !error )
    printf("arrow1 of dim_radius : " FORMAT_B "\n", flip_arrow2);
  else
    printf("in reading arrow1 \n");


  flags1 = dwg_ent_dim_radius_get_flags1(dim_radius,
                                         &error);
  if ( !error )
    printf("flags1 of dim_radius : " FORMAT_RC "\n", flags1);
  else
    printf("in reading flags1 \n");

  act_measure = dwg_ent_dim_radius_get_act_measurement(dim_radius,
                                                       &error);
  if ( !error )
    printf("act_measurement of dim_radius : %f\n", act_measure);
  else
    printf("in reading act_measurement \n");


  length = dwg_ent_dim_radius_get_leader_length(dim_radius, &error);
  if ( !error )
    printf("leader length of dim radius : %f\n", length);
  else
    printf("error in reading leader length \n");
}
