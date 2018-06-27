#define DWG_TYPE DWG_TYPE_DIMENSION_ORDINATE
#include "common.c"

void
low_level_process(dwg_object *obj)
{
  dwg_ent_dim_ordinate *dim_ordinate = dwg_object_to_DIMENSION_ORDINATE(obj);

  printf("horiz dir of dim_ordinate : %f\n",
          dim_ordinate->horiz_dir);
  printf("lspace factor of dim_ordinate : %f\n", 
          dim_ordinate->lspace_factor);
  printf("lspace style of dim_ordinate : " FORMAT_BS "\n",
          dim_ordinate->lspace_style);
  printf("attach point of dim_ordinate : " FORMAT_BS "\n",
          dim_ordinate->attachment);
  printf("elevation of dim_ordinate : %f\n",
          dim_ordinate->elevation);
  printf("extrusion of dim_ordinate : x = %f, y = %f, z = %f\n",
          dim_ordinate->extrusion.x, dim_ordinate->extrusion.y,
          dim_ordinate->extrusion.z);
  printf("ins_scale of dim_ordinate : x = %f, y = %f, z = %f\n",
          dim_ordinate->ins_scale.x, dim_ordinate->ins_scale.y,
          dim_ordinate->ins_scale.z);
  printf("ucsorigin_pt of dim_ordinate : x = %f, y = %f, z = %f\n",
          dim_ordinate->ucsorigin_pt.x, dim_ordinate->ucsorigin_pt.y,
          dim_ordinate->ucsorigin_pt.z);
  printf("feature_location_pt of dim_ordinate : x = %f, y = %f, z = %f\n",
          dim_ordinate->feature_location_pt.x, dim_ordinate->feature_location_pt.y,
          dim_ordinate->feature_location_pt.z);
  printf("leader_endpt of dim_ordinate : x = %f, y = %f, z = %f\n",
          dim_ordinate->leader_endpt.x, dim_ordinate->leader_endpt.y,
          dim_ordinate->leader_endpt.z);
  printf("clone_ins_pt of dim_ordinate : x = %f, y = %f\n",
          dim_ordinate->clone_ins_pt.x, dim_ordinate->clone_ins_pt.y);
  printf("text_mid_pt of dim_ordinate : x = %f, y = %f\n",
          dim_ordinate->text_midpt.x, dim_ordinate->text_midpt.y);
  printf("user text of dim_ordinate : %s\n", dim_ordinate->user_text);
  printf("text rotation of dim_ordinate : %f\n", dim_ordinate->text_rotation);
  printf("ins rotation of dim_ordinate : %f\n", dim_ordinate->ins_rotation);
  printf("arrow1 of dim_ordinate : " FORMAT_B "\n", dim_ordinate->flip_arrow1);
  printf("arrow2 of dim_ordinate : " FORMAT_B "\n", dim_ordinate->flip_arrow2);
  printf("flags1 of dim_ordinate : " FORMAT_RC "\n", dim_ordinate->flag1);
  printf("flags2 of dim_ordinate : " FORMAT_RC "\n", dim_ordinate->flag2);
  printf("act_measurement of dim_ordinate : %f\n",
          dim_ordinate->act_measurement);

}

void
api_process(dwg_object *obj)
{
  int error;
  double elevation, act_measure, horiz_dir, lspace_factor, text_rot, 
    ins_rot;
  BITCODE_B flip_arrow1, flip_arrow2;
  BITCODE_RC flag1, flag2;
  BITCODE_BS lspace_style, attachment;
  char * user_text;
  dwg_point_2d text_mid_pt, pt12;
  dwg_point_3d pt10, pt13, pt14, ext, ins_scale;

  dwg_ent_dim_ordinate *dim_ordinate = dwg_object_to_DIMENSION_ORDINATE(obj);
  dwg_ent_dim *dim = dwg_object_to_DIMENSION(obj);

  horiz_dir = dwg_ent_dim_get_horiz_dir(dim, &error);
  if ( !error )
    printf("horiz dir of dim_ordinate : %f\n", horiz_dir);
  else
    printf("error in reading horiz dir \n");

  lspace_factor = dwg_ent_dim_get_lspace_factor(dim, &error);
  if ( !error )
    printf("lspace factor of dim_ordinate : %f\n", lspace_factor);
  else	
    printf("error in reading lspace factor \n");

  lspace_style = dwg_ent_dim_get_elevation(dim, &error);
  if ( !error )
    printf("lspace style of dim_ordinate : " FORMAT_BS "\n", lspace_style);
  else
    printf("error in reading lspace style \n");

  attachment = dwg_ent_dim_get_attachment(dim, &error);
  if ( !error )
    printf("attachment of dim_ordinate : " FORMAT_BS "\n", attachment);
  else
    printf("error in reading attachment \n");

  elevation = dwg_ent_dim_get_elevation(dim, &error);
  if ( !error )
    printf("elevation of dim_ordinate : %f\n", elevation);
  else
    printf("error in reading elevation \n");

  dwg_ent_dim_get_extrusion(dim, &ext, &error);
  if ( !error )
    printf("extrusion of dim_ordinate : x = %f, y = %f, z = %f\n",
           ext.x, ext.y, ext.z);
  else
    printf("error in reading extrusion \n");

  dwg_ent_dim_get_ins_scale(dim, &ins_scale, &error);
  if ( !error )
    printf("ins_scale of dim_ordinate : x = %f, y = %f, z = %f\n",
           ins_scale.x, ins_scale.y, ins_scale.z);
  else
    printf("error in reading ins_scale \n");

  dwg_ent_dim_ordinate_get_ucsorigin_pt(dim_ordinate, &pt10, &error);
  if ( !error )
    printf("ucsorigin_pt of dim_ordinate : x = %f, y = %f, z = %f\n",
           pt10.x, pt10.y, pt10.z);
  else
    printf("error in reading pt10 \n");

  dwg_ent_dim_ordinate_get_feature_location_pt(dim_ordinate, &pt13, &error);
  if ( !error )
    printf("feature_location_pt of dim_ordinate : x = %f, y = %f, z = %f\n",
           pt13.x, pt13.y, pt13.z);
  else
    printf("error in reading feature_location_pt\n");

  dwg_ent_dim_ordinate_get_leader_endpt(dim_ordinate, &pt14, &error);
  if ( !error )
    printf("leader_endpt of dim_ordinate : x = %f, y = %f, z = %f\n",
           pt14.x, pt14.y, pt14.z);
  else
    printf("error in reading leader_endpt\n");

  dwg_ent_dim_get_clone_ins_pt(dim, &pt12, &error);
  if ( !error )
    printf("clone_ins_pt of dim_ordinate : x = %f, y = %f\n",
           pt12.x, pt12.y);
  else
    printf("error in reading clone_ins_pt\n");

  dwg_ent_dim_get_text_midpt(dim, &text_mid_pt, &error);
  if ( !error )
    printf("text_mid_pt of dim_ordinate : x = %f, y = %f\n",
           text_mid_pt.x, text_mid_pt.y);
  else
    printf("error in reading text_mid_pt \n");

  user_text = dwg_ent_dim_get_user_text(dim, &error);
  if ( !error )
    printf("user text of dim_ordinate : %s\n", user_text);
  else
    printf("error in reading user_text \n");

  text_rot = dwg_ent_dim_get_text_rotation(dim, &error);
  if ( !error )
    printf(" text rotation of dim_ordinate : %f\n", text_rot);
  else
    printf("in reading text rotation \n");

  ins_rot = dwg_ent_dim_get_ins_rotation(dim, &error);
  if ( !error )
    printf("ins rotation of dim_ordinate : %f\n", ins_rot);
  else
    printf("in reading ins rotation \n");

  flip_arrow1 = dwg_ent_dim_get_flip_arrow1(dim, &error);
  if ( !error )
    printf("arrow1 of dim_ordinate : " FORMAT_B "\n", flip_arrow1);
  else
    printf("in reading arrow1 \n");

  flip_arrow2 = dwg_ent_dim_get_flip_arrow2(dim, &error);
  if ( !error )
    printf("arrow1 of dim_ordinate : " FORMAT_B "\n", flip_arrow2);
  else
    printf("in reading arrow1 \n");

  flag1 = dwg_ent_dim_get_flag1(dim, &error);
  if ( !error )
    printf("flag1 of dim_ordinate : " FORMAT_RC "\n", flag1);
  else
    printf("in reading flag1 \n");

  flag2 = dwg_ent_dim_ordinate_get_flag2(dim_ordinate, &error);
  if ( !error )
    printf("flag2 of dim_ordinate : " FORMAT_RC "\n", flag2);
  else
    printf("in reading flag2 \n");

  act_measure = dwg_ent_dim_get_act_measurement(dim, &error);
  if ( !error )
    printf("act_measurement of dim_ordinate : %f\n", act_measure);
  else
    printf("in reading act_measurement \n");
 
}
