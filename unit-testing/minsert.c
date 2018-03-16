#include "common.c"

void
output_object(dwg_object* obj)
{
  if (!obj)
    {
      printf("object is NULL\n");
      return;
    }

  if (dwg_get_type(obj)== DWG_TYPE_MINSERT)
    {
      output_process(obj);
    }
}

void
low_level_process(dwg_object *obj)
{
  // casts dwg onject to minsert
  dwg_ent_minsert *minsert = dwg_object_to_MINSERT(obj);

  // prints inserion points
  printf("minsert points : x = %f, y = %f, z = %f\n",
	  minsert->ins_pt.x, minsert->ins_pt.y, minsert->ins_pt.z);

  // prints scale flag
  printf("scale flag for minsert : " FORMAT_BS "\n", minsert->scale_flag);

  // prints scale points
  printf("scale points : x = %f, y = %f, z = %f\n",
	  minsert->scale.x, minsert->scale.y, minsert->scale.z);

  // prints rotation angle
  printf("angle for minsert : %f\n", minsert->rotation_ang);

  // prints extrusion point
  printf("extrusion points : x = %f, y = %f, z = %f\n",
	  minsert->extrusion.x, minsert->extrusion.y, minsert->extrusion.z);

  // prints has_attributes
  printf("attribs for minsert : " FORMAT_BS "\n", minsert->has_attribs);

  // prints owned object counts
  printf("object count for minsert : " FORMAT_BL "\n", minsert->owned_obj_count);

  // prints num rows
  printf("number of rows for minsert : %u\n", minsert->numrows);

  // prints num cols
  printf("number of columns for minsert : %u\n", minsert->numcols);

  // prints col space
  printf("col space for minsert : %f\n", minsert->row_spacing);

  // prints rows space
  printf("row space for minsert : %f\n", minsert->col_spacing);
 
}

void
api_process(dwg_object *obj)
{
  int error;
  dwg_point_3d ins_pt, ext, scale;
  double rot_angle;
  char scale_flag, attribs;
  double col_space, row_space;
  BITCODE_BL obj_count, num_rows, num_cols;

  dwg_ent_minsert *minsert = dwg_object_to_MINSERT(obj);

  // returns insertion point
  dwg_ent_minsert_get_ins_pt(minsert, &ins_pt,
                             &error);
  if ( !error )
    {
      printf("minsert points : x = %f, y = %f, z = %f\n",
	     ins_pt.x, ins_pt.y, ins_pt.z);
    }
  else
    {
      printf("error in reading minsertion point\n");
    }

  // returns scale flag
  scale_flag = dwg_ent_minsert_get_scale_flag(minsert, &error); 
  if ( !error )
    {
      printf("scale flag for minsert : " FORMAT_BS "\n", scale_flag);
    }
  else
    {
      printf("error in reading scale flag\n");
    } 

  // returns scale
  dwg_ent_minsert_get_scale(minsert, &scale,
                            &error); 
  if ( !error )
    {
      printf("scale points : x = %f, y = %f, z = %f\n",
	     scale.x, scale.y, scale.z);
    }
  else
    {
      printf("error in reading scale \n");
    }

  // returns rotation angle
  rot_angle = dwg_ent_minsert_get_rotation_angle(minsert, &error); 
  if ( !error )
    {
      printf("angle for minsert : %f\n", rot_angle);
    }
  else
    {
      printf("error in reading rotation angle\n");
    } 

  // returns extrusion
  dwg_ent_minsert_get_extrusion(minsert, &ext,
                                &error); 
  if ( !error )
    {
      printf("extrusion points : x = %f, y = %f, z = %f\n",
	     ext.x, ext.y, ext.z);
    }
  else
    {
      printf("error in reading extrusion \n");
    } 

  // returns attribute
  attribs = dwg_ent_minsert_get_has_attribs(minsert, &error); 
  if ( !error )
    {
      printf("attribs for minsert : " FORMAT_BS "\n", attribs);
    }
  else
    {
      printf("error in reading attribs\n");
    } 

  // returns owned object counts
  obj_count = dwg_ent_minsert_get_owned_obj_count(minsert, &error); 
  if ( !error )
    {
      printf("object count for minsert : " FORMAT_BL "\n", obj_count);
    }
  else
    {
      printf("error in reading object counts\n");
    } 

  // returns num rows
  num_rows = dwg_ent_minsert_get_numrows(minsert, &error); 
  if ( !error )
    {
      printf("number of rows for minsert : " FORMAT_BL "\n", num_rows);
    }
  else
    {
      printf("error in reading number of rows\n");
    } 

  // returns number of columns
  num_cols = dwg_ent_minsert_get_numcols(minsert, &error); 
  if ( !error )
    {
      printf("number of columns for minsert : " FORMAT_BL "\n", num_cols);
    }
  else
    {
      printf("error in reading number of columns\n");
    } 

  // returns row space
  row_space= dwg_ent_minsert_get_row_spacing(minsert, &error); 
  if ( !error )
    {
      printf("row space for minsert : %f\n", row_space);
    }
  else
    {
      printf("error in reading numspace\n");
    } 

  // returns col space
  col_space = dwg_ent_minsert_get_col_spacing(minsert, &error); 
  if ( !error )
    {
      printf("col space for minsert : %f\n", col_space);
    }
  else
    {
      printf("error in reading col_space\n");
    } 
} 
