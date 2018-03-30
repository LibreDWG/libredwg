#define DWG_TYPE DWG_TYPE_MINSERT
#include "common.c"

void
low_level_process(dwg_object *obj)
{
  dwg_ent_minsert *minsert = dwg_object_to_MINSERT(obj);

  printf("minsert points : x = %f, y = %f, z = %f\n",
	  minsert->ins_pt.x, minsert->ins_pt.y, minsert->ins_pt.z);
  printf("scale flag for minsert : " FORMAT_BB "\n", minsert->scale_flag);
  printf("scale points : x = %f, y = %f, z = %f\n",
	  minsert->scale.x, minsert->scale.y, minsert->scale.z);
  printf("angle for minsert : %f\n", minsert->rotation);
  printf("extrusion points : x = %f, y = %f, z = %f\n",
	  minsert->extrusion.x, minsert->extrusion.y, minsert->extrusion.z);
  printf("attribs for minsert : " FORMAT_B "\n", minsert->has_attribs);
  printf("object count for minsert : " FORMAT_BL "\n", minsert->owned_obj_count);
  printf("number of rows for minsert : %u\n", minsert->numrows);
  printf("number of columns for minsert : %u\n", minsert->numcols);
  printf("col space for minsert : %f\n", minsert->row_spacing);
  printf("row space for minsert : %f\n", minsert->col_spacing);
}

void
api_process(dwg_object *obj)
{
  int error;
  dwg_point_3d ins_pt, ext, scale;
  double rot_angle;
  BITCODE_BB scale_flag;
  BITCODE_B  has_attribs;
  double col_space, row_space;
  BITCODE_BL obj_count, num_rows, num_cols;

  dwg_ent_minsert *minsert = dwg_object_to_MINSERT(obj);


  dwg_ent_minsert_get_ins_pt(minsert, &ins_pt,
                             &error);
  if ( !error )
    printf("minsert points : x = %f, y = %f, z = %f\n",
           ins_pt.x, ins_pt.y, ins_pt.z);
  else
    printf("error in reading minsertion point\n");


  scale_flag = dwg_ent_minsert_get_scale_flag(minsert, &error); 
  if ( !error )
    printf("scale flag for minsert : " FORMAT_BB "\n", scale_flag);
  else
    printf("in reading scale flag\n"); 


  dwg_ent_minsert_get_scale(minsert, &scale,
                            &error); 
  if ( !error )
    printf("scale points : x = %f, y = %f, z = %f\n",
           scale.x, scale.y, scale.z);
  else
    printf("error in reading scale \n");


  rot_angle = dwg_ent_minsert_get_rotation(minsert, &error); 
  if ( !error )
    printf("angle for minsert : %f\n", rot_angle);
  else
    printf("in reading rotation angle\n"); 


  dwg_ent_minsert_get_extrusion(minsert, &ext,
                                &error); 
  if ( !error )
    printf("extrusion points : x = %f, y = %f, z = %f\n",
           ext.x, ext.y, ext.z);
  else
    printf("error in reading extrusion \n");


  has_attribs = dwg_ent_minsert_get_has_attribs(minsert, &error); 
  if ( !error )
    printf("attribs for minsert : " FORMAT_B "\n", has_attribs);
  else
    printf("in reading attribs\n"); 


  obj_count = dwg_ent_minsert_get_owned_obj_count(minsert, &error); 
  if ( !error )
    printf("object count for minsert : " FORMAT_BL "\n", obj_count);
  else
    printf("in reading object counts\n"); 


  num_rows = dwg_ent_minsert_get_numrows(minsert, &error); 
  if ( !error )
    printf("number of rows for minsert : " FORMAT_BL "\n", num_rows);
  else
    printf("in reading number of rows\n"); 


  num_cols = dwg_ent_minsert_get_numcols(minsert, &error); 
  if ( !error )
    printf("number of columns for minsert : " FORMAT_BL "\n", num_cols);
  else
    printf("in reading number of columns\n"); 


  row_space= dwg_ent_minsert_get_row_spacing(minsert, &error); 
  if ( !error )
    printf("row space for minsert : %f\n", row_space);
  else
    printf("in reading numspace\n"); 


  col_space = dwg_ent_minsert_get_col_spacing(minsert, &error); 
  if ( !error )
    printf("col space for minsert : %f\n", col_space);
  else
    printf("in reading col_space\n"); 
} 
