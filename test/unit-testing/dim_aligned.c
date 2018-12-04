#define DWG_TYPE DWG_TYPE_DIMENSION_ALIGNED
#include "common.c"

void
low_level_process(dwg_object *obj)
{
  // returns dwg_object to dim aligned entity
  dwg_ent_dim_aligned *dim_aligned = dwg_object_to_DIMENSION_ALIGNED(obj);

  printf("horiz dir of dim_aligned : %f\n",
          dim_aligned->horiz_dir);
  printf("lspace factor of dim_aligned : %f\n",
          dim_aligned->lspace_factor);
  printf("lspace style of dim_aligned : " FORMAT_BS "\n",
          dim_aligned->lspace_style);
  printf("attachment of dim_aligned : " FORMAT_BS "\n",
          dim_aligned->attachment);
  printf("elevation of dim_aligned : %f\n",
          dim_aligned->elevation);
  printf("extrusion of dim_aligned : x = %f, y = %f, z = %f\n",
          dim_aligned->extrusion.x, dim_aligned->extrusion.y,
          dim_aligned->extrusion.z);
  printf("ins_scale of dim_aligned : x = %f, y = %f, z = %f\n",
          dim_aligned->ins_scale.x, dim_aligned->ins_scale.y,
          dim_aligned->ins_scale.z);
  printf("def_pt of dim_aligned : x = %f, y = %f, z = %f\n",
          dim_aligned->def_pt.x, dim_aligned->def_pt.y,
          dim_aligned->def_pt.z);
  printf("pt13 of dim_aligned : x = %f, y = %f, z = %f\n",
          dim_aligned->_13_pt.x, dim_aligned->_13_pt.y,
          dim_aligned->_13_pt.z);
  printf("pt14 of dim_aligned : x = %f, y = %f, z = %f\n",
          dim_aligned->_14_pt.x, dim_aligned->_14_pt.y,
          dim_aligned->_14_pt.z);
  printf("clone_ins_pt of dim_aligned : x = %f, y = %f\n",
          dim_aligned->clone_ins_pt.x, dim_aligned->clone_ins_pt.y);
  printf("text_mid_pt of dim_aligned : x = %f, y = %f\n",
          dim_aligned->text_midpt.x, dim_aligned->text_midpt.y);
  printf("user text of dim_aligned : %s\n", dim_aligned->user_text);
  printf("text rotation of dim_aligned : %f\n", dim_aligned->text_rotation);
  printf("ins rotation of dim_aligned : %f\n", dim_aligned->ins_rotation);
  printf("arrow1 of dim_aligned : " FORMAT_B "\n", dim_aligned->flip_arrow1);
  printf("arrow2 of dim_aligned : " FORMAT_B "\n", dim_aligned->flip_arrow2);
  printf("flag1 of dim_aligned : " FORMAT_RC "\n", dim_aligned->flag1);
  printf("act_measurement of dim_aligned : %f\n",
          dim_aligned->act_measurement);
  printf("ext line rotation of dim_aligned : %f\n", dim_aligned->ext_line_rotation);
}

void
api_process(dwg_object *obj)
{
  int error;
  double elevation, act_measure, horiz_dir, lspace_factor, rotation;
  BITCODE_B flip_arrow1, flip_arrow2;
  BITCODE_RC flags1, flags2;
  BITCODE_BS lspace_style, attachment;
  char * user_text;
  dwg_point_2d text_midpt, pt12;
  dwg_point_3d pt10, pt13, pt14, ext, ins_scale;

  dwg_ent_dim_aligned *dim_aligned = dwg_object_to_DIMENSION_ALIGNED(obj);
  dwg_ent_dim *dim = dwg_object_to_DIMENSION(obj);

  horiz_dir = dwg_ent_dim_get_horiz_dir(dim, &error);
  if (!error)
    printf("horiz dir of dim_aligned : %f\n", horiz_dir);
  else
    printf("error in reading horiz dir \n");

  lspace_factor = dwg_ent_dim_get_lspace_factor(dim, &error);
  if (!error)
    printf("lspace factor of dim_aligned : %f\n", lspace_factor);
  else
    printf("error in reading lspace factor \n");

  lspace_style = dwg_ent_dim_get_lspace_style(dim, &error);
  if (!error)
    printf("lspace style of dim_aligned : " FORMAT_BS "\n", lspace_style);
  else
    printf("error in reading lspace style \n");

  attachment = dwg_ent_dim_get_attachment(dim, &error);
  if (!error)
    printf("attachment of dim_aligned : " FORMAT_BS "\n", attachment);
  else
    printf("error in reading attachment \n");

  elevation = dwg_ent_dim_get_elevation(dim, &error);
  if (!error)
    printf("elevation of dim_aligned : %f\n", elevation);
  else
    printf("error in reading elevation \n");

  dwg_ent_dim_get_extrusion(dim, &ext, &error);
  if (!error)
    printf("extrusion of dim_aligned : x = %f, y = %f, z = %f\n",
           ext.x, ext.y, ext.z);
  else
    printf("error in reading extrusion \n");

  dwg_ent_dim_get_ins_scale(dim, &ins_scale, &error);
  if (!error)
    printf("ins_scale of dim_aligned : x = %f, y = %f, z = %f\n",
           ins_scale.x, ins_scale.y, ins_scale.z);
  else
    printf("error in reading ins_scale \n");

  dwg_ent_dim_aligned_get_def_pt(dim_aligned, &pt10, &error);
  if (!error)
    printf("def_pt of dim_aligned : x = %f, y = %f, z = %f\n",
           pt10.x, pt10.y, pt10.z);
  else
    printf("error in reading pt10 \n");

  dwg_ent_dim_aligned_get_13_pt(dim_aligned, &pt13, &error);
  if (!error)
    printf("pt13 of dim_aligned : x = %f, y = %f, z = %f\n",
           pt13.x, pt13.y, pt13.z);
  else
    printf("error in reading pt13 \n");

  dwg_ent_dim_aligned_get_14_pt(dim_aligned, &pt14, &error);
  if (!error)
    printf("pt14 of dim_aligned : x = %f, y = %f, z = %f\n",
           pt14.x, pt14.y, pt14.z);
  else
    printf("error in reading pt14 \n");

  dwg_ent_dim_get_clone_ins_pt(dim, &pt12, &error);
  if (!error)
    printf("clone_ins_pt of dim_aligned : x = %f, y = %f\n",
           pt12.x, pt12.y);
  else
    printf("error in reading pt12 \n");

  dwg_ent_dim_get_text_midpt(dim, &text_midpt, &error);
  if (!error)
    printf("text_mid_pt of dim_aligned : x = %f, y = %f\n",
           text_midpt.x, text_midpt.y);
  else
    printf("error in reading text_mid_pt \n");

  user_text = dwg_ent_dim_get_user_text(dim, &error);
  if (!error)
    printf("user text of dim_aligned : %s\n",user_text);
  else
    printf("error in reading user_text \n");

  rotation = dwg_ent_dim_get_text_rotation(dim, &error);
  if (!error)
    printf(" text rotation of dim_aligned : %f\n", rotation);
  else
    printf("error in reading text rotation \n");

  rotation = dwg_ent_dim_get_ins_rotation(dim, &error);
  if (!error)
    printf("ins rotation of dim : %f\n", rotation);
  else
    printf("error in reading ins rotation \n");

  flip_arrow1 = dwg_ent_dim_get_flip_arrow1(dim, &error);
  if (!error)
    printf("arrow1 of dim : " FORMAT_B "\n", flip_arrow1);
  else
    printf("error in reading arrow1 \n");

  flip_arrow2 = dwg_ent_dim_get_flip_arrow2(dim, &error);
  if (!error)
    printf("arrow1 of dim : " FORMAT_B "\n", flip_arrow2);
  else
    printf("error in reading arrow1 \n");

  flags1 = dwg_ent_dim_get_flag1(dim, &error);
  if (!error)
    printf("flag1 of dim : " FORMAT_RC "\n", flags1);
  else
    printf("error in reading flags1 \n");

  act_measure = dwg_ent_dim_get_act_measurement(dim, &error);
  if (!error)
    printf("act_measurement of dim : %f\n", act_measure);
  else
    printf("error in reading act_measurement \n");

  rotation = dwg_ent_dim_aligned_get_ext_line_rotation(dim_aligned, &error);
  if (!error)
    printf("ext line rotation of dim_aligned : %f\n", rotation);
  else
    printf("error in reading ext line rotation \n");
}
