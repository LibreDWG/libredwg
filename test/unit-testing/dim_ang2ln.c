#define DWG_TYPE DWG_TYPE_DIMENSION_ANG2LN
#include "common.c"

void
low_level_process(dwg_object *obj)
{
  dwg_ent_dim_ang2ln *dim_ang2ln = dwg_object_to_DIMENSION_ANG2LN(obj);

  printf("horiz dir of dim_ang2ln : %f\n",
          dim_ang2ln->horiz_dir);
  printf("lspace factor of dim_ang2ln : %f\n",
          dim_ang2ln->lspace_factor);
  printf("lspace style of dim_ang2ln : " FORMAT_BS "\n",
          dim_ang2ln->lspace_style);
  printf("attachment of dim_ang2ln : " FORMAT_BS "\n",
          dim_ang2ln->attachment);
  printf("elevation of dim_ang2ln : %f\n",
          dim_ang2ln->elevation);
  printf("extrusion of dim_ang2ln : x = %f, y = %f, z = %f\n",
          dim_ang2ln->extrusion.x, dim_ang2ln->extrusion.y,
          dim_ang2ln->extrusion.z);
  printf("ins_scale of dim_ang2ln : x = %f, y = %f, z = %f\n",
         dim_ang2ln->ins_scale.x, dim_ang2ln->ins_scale.y,
         dim_ang2ln->ins_scale.z);
  printf("pt10 of dim_ang2ln : x = %f, y = %f, z = %f\n",
          dim_ang2ln->def_pt.x, dim_ang2ln->def_pt.y,
          dim_ang2ln->def_pt.z);
  printf("pt13 of dim_ang2ln : x = %f, y = %f, z = %f\n",
          dim_ang2ln->_13_pt.x, dim_ang2ln->_13_pt.y,
          dim_ang2ln->_13_pt.z);
  printf("pt14 of dim_ang2ln : x = %f, y = %f, z = %f\n",
          dim_ang2ln->_14_pt.x, dim_ang2ln->_14_pt.y,
          dim_ang2ln->_14_pt.z);
  printf("pt15 of dim_ang2ln : x = %f, y = %f, z = %f\n",
          dim_ang2ln->first_arc_pt.x, dim_ang2ln->first_arc_pt.y,
          dim_ang2ln->first_arc_pt.z);
  printf("pt12 of dim_ang2ln : x = %f, y = %f\n",
          dim_ang2ln->clone_ins_pt.x, dim_ang2ln->clone_ins_pt.y);
  printf("pt16 of dim_ang2ln : x = %f, y = %f\n",
          dim_ang2ln->_16_pt.x, dim_ang2ln->_16_pt.y);
  printf("text_mid_pt of dim_ang2ln : x = %f, y = %f\n",
          dim_ang2ln->text_midpt.x, dim_ang2ln->text_midpt.y);
  printf("user text of dim_ang2ln : %s\n", dim_ang2ln->user_text);
  printf("text rotation of dim_ang2ln : %f\n", dim_ang2ln->text_rotation);
  printf("ins rotation of dim_ang2ln : %f\n", dim_ang2ln->ins_rotation);
  printf("arrow1 of dim_ang2ln : " FORMAT_B "\n", dim_ang2ln->flip_arrow1);
  printf("arrow2 of dim_ang2ln : " FORMAT_B "\n", dim_ang2ln->flip_arrow2);
  printf("flags1 of dim_ang2ln : " FORMAT_RC "\n", dim_ang2ln->flag1);
  printf("act_measurement of dim_ang2ln : %f\n",
            dim_ang2ln->act_measurement);
}

void
api_process(dwg_object *obj)
{
  int error;
  double elevation, act_measure, horiz_dir, lspace_factor, text_rot,
         ins_rot, ext_line_rot, dim_rot;
  BITCODE_B flip_arrow1, flip_arrow2;
  BITCODE_RC flags1;
  BITCODE_BS lspace_style, attachment;
  char * user_text;
  dwg_point_2d text_mid_pt, pt12, pt16;
  dwg_point_3d pt10, pt13, pt14, pt15, ext, ins_scale;

  dwg_ent_dim_ang2ln *dim_ang2ln = dwg_object_to_DIMENSION_ANG2LN(obj);
  dwg_ent_dim *dim = dwg_object_to_DIMENSION(obj);

  horiz_dir = dwg_ent_dim_get_horiz_dir(dim, &error);
  if ( !error )
      printf("horiz dir of dim : %f\n", horiz_dir);
  else
      printf("error in reading horiz dir \n");

  lspace_factor = dwg_ent_dim_get_lspace_factor(dim, &error);
  if ( !error )
    printf("lspace factor of dim : %f\n", lspace_factor);
  else
    printf("error in reading lspace factor \n");

  lspace_style = dwg_ent_dim_get_lspace_style(dim, &error);
  if ( !error )
    printf("lspace style of dim : " FORMAT_BS "\n", lspace_style);
  else
    printf("error in reading lspace style \n");

  attachment = dwg_ent_dim_get_attachment(dim, &error);
  if ( !error )
      printf("attachment of dim : " FORMAT_BS "\n", attachment);
  else
      printf("error in reading attachment \n");

  elevation = dwg_ent_dim_get_elevation(dim, &error);
  if ( !error )
    printf("Radius of dim : %f\n", elevation);
  else
    printf("error in reading elevation \n");

  dwg_ent_dim_get_extrusion(dim, &ext, &error);
  if ( !error )
    printf("extrusion of dim : x = %f, y = %f, z = %f\n",
           ext.x, ext.y, ext.z);
  else
    printf("error in reading extrusion \n");

  dwg_ent_dim_get_ins_scale(dim, &ins_scale, &error);
  if ( !error )
    printf("ins_scale of dim : x = %f, y = %f, z = %f\n",
           ins_scale.x, ins_scale.y, ins_scale.z);
  else
    printf("error in reading ins_scale \n");

  dwg_ent_dim_ang2ln_get_def_pt(dim_ang2ln, &pt10, &error);
  if ( !error )
    printf("pt10 of dim_ang2ln : x = %f, y = %f, z = %f\n",
           pt10.x, pt10.y, pt10.z);
  else
    printf("error in reading pt10 \n");

  dwg_ent_dim_ang2ln_get_13_pt(dim_ang2ln, &pt13, &error);
  if ( !error )
    printf("pt13 of dim_ang2ln : x = %f, y = %f, z = %f\n",
           pt13.x, pt13.y, pt13.z);
  else
    printf("error in reading pt13 \n");

  dwg_ent_dim_ang2ln_get_14_pt(dim_ang2ln, &pt14, &error);
  if ( !error )
    printf("pt14 of dim_ang2ln : x = %f, y = %f, z = %f\n",
           pt14.x, pt14.y, pt14.z);
  else
    printf("error in reading pt14 \n");

  dwg_ent_dim_ang2ln_get_first_arc_pt(dim_ang2ln, &pt15, &error);
  if ( !error )
    printf("pt15 of dim_ang2ln : x = %f, y = %f, z = %f\n",
           pt15.x, pt15.y, pt15.z);
  else
    printf("error in reading pt14 \n");

  dwg_ent_dim_get_clone_ins_pt(dim, &pt12, &error);
  if ( !error )
      printf("pt12 of dim : x = %f, y = %f\n",
              pt12.x, pt12.y);
  else
      printf("error in reading pt12 \n");

  dwg_ent_dim_ang2ln_get_16_pt(dim_ang2ln, &pt16, &error);
  if ( !error )
    printf("pt16 of dim_ang2ln : x = %f, y = %f\n",
           pt16.x, pt16.y);
  else
    printf("error in reading pt16 \n");

  dwg_ent_dim_get_text_midpt(dim, &text_mid_pt, &error);
  if ( !error )
    printf("text_mid_pt of dim : x = %f, y = %f\n",
           text_mid_pt.x, text_mid_pt.y);
  else
    printf("error in reading text_mid_pt \n");

  user_text = dwg_ent_dim_get_user_text(dim, &error);
  if ( !error )
      printf("user text of dim : %s\n", user_text);
  else
      printf("error in reading user_text \n");

  text_rot = dwg_ent_dim_get_text_rotation(dim, &error);
  if ( !error )
      printf(" text rotation of dim : %f\n", text_rot);
  else
      printf("error in reading text rotation \n");

  ins_rot = dwg_ent_dim_get_ins_rotation(dim, &error);
  if ( !error )
      printf("ins rotation of dim : %f\n", ins_rot);
  else
      printf("error in reading ins rotation \n");

  flip_arrow1 = dwg_ent_dim_get_flip_arrow1(dim, &error);
  if ( !error )
      printf("arrow1 of dim : " FORMAT_B "\n", flip_arrow1);
  else
      printf("error in reading arrow1 \n");

  flip_arrow2 = dwg_ent_dim_get_flip_arrow2(dim, &error);
  if ( !error )
    printf("arrow2 of dim : " FORMAT_B "\n", flip_arrow2);
  else
    printf("error in reading arrow2 \n");

  flags1 = dwg_ent_dim_get_flag1(dim, &error);
  if ( !error )
    printf("flags1 of dim : " FORMAT_RC "\n", flags1);
  else
    printf("error in reading flags1 \n");

  act_measure = dwg_ent_dim_get_act_measurement(dim, &error);
  if ( !error )
    printf("act_measurement of dim : %f\n", act_measure);
  else
    printf("error in reading act_measurement \n");
}
