#include "common.c"
#include <dejagnu.h>
void output_process (dwg_object * obj);

void
output_object (dwg_object * obj)
{
  if (!obj)
    {
      printf ("object is NULL\n");
      return;
    }

  if (dwg_get_type (obj) == DWG_TYPE_MINSERT)
    {
      output_process (obj);
    }
}

void
low_level_process (dwg_object * obj)
{
  // casts dwg onject to minsert
  dwg_ent_minsert *minsert = dwg_object_to_MINSERT (obj);

  // prints inserion points
  printf ("minsert points : x = %f, y = %f, z = %f\t\n",
	  minsert->ins_pt.x, minsert->ins_pt.y, minsert->ins_pt.z);

  // prints scale flag
  printf ("scale flag for minsert : %d\t\n", minsert->scale_flag);

  // prints scale points
  printf ("scale points : x = %f, y = %f, z = %f\t\n",
	  minsert->scale.x, minsert->scale.y, minsert->scale.z);

  // prints rotation angle
  printf ("angle for minsert : %f\t\n", minsert->rotation_ang);

  // prints extrusion point
  printf ("extrusion points : x = %f, y = %f, z = %f\t\n",
	  minsert->extrusion.x, minsert->extrusion.y, minsert->extrusion.z);

  // prints has_attributes
  printf ("attribs for minsert : %d\t\n", minsert->has_attribs);

  // prints owned object counts
  printf ("object count for minsert : %ld\t\n", minsert->owned_obj_count);

  // prints num rows
  printf ("number of rows for minsert : %ld\t\n", minsert->numrows);

  // prints num cols
  printf ("number of columns for minsert : %ld\t\n", minsert->numcols);

  // prints col space
  printf ("col space for minsert : %f\t\n", minsert->row_spacing);

  // prints rows space
  printf ("row space for minsert : %f\t\n", minsert->col_spacing);

}

void
api_process (dwg_object * obj)
{
  int ins_pt_error, ext_error, scale_error, ang_error, flag_error, attr_error,
    count_error, num_col_error, num_row_error, colspace_error, rowspace_error;
  dwg_point_3d ins_pt, ext, scale;
  double rot_angle;
  char scale_flag, attribs;
  float col_space, row_space;
  long obj_count, num_rows, num_cols;
  dwg_ent_minsert *minsert = dwg_object_to_MINSERT (obj);


  // returns insertion point
  dwg_ent_minsert_get_ins_pt (minsert, &ins_pt, &ins_pt_error);
  if (ins_pt_error == 0 && ins_pt.x == minsert->ins_pt.x && ins_pt.y == minsert->ins_pt.y && ins_pt.z == minsert->ins_pt.z)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading insertion point");
    }

  // returns scale flag
  scale_flag = dwg_ent_minsert_get_scale_flag (minsert, &flag_error);
  if (flag_error == 0 && scale_flag == minsert->scale_flag)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading scale flag");
    }

  // returns scale points
  dwg_ent_minsert_get_scale (minsert, &scale, &scale_error);
  if (scale_error == 0 && scale.x == minsert->scale.x && scale.y == minsert->scale.y && scale.z == minsert->scale.z)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading scale");
    }

  // returns rotation angle
  rot_angle = dwg_ent_minsert_get_rotation_angle (minsert, &ang_error);
  if (ang_error == 0 && rot_angle == minsert->rotation_ang)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading rotation angle");
    }

  // returns extrusion points
  dwg_ent_minsert_get_extrusion (minsert, &ext, &ext_error);
  if (ext_error == 0 && ext.x == minsert->extrusion.x && ext.y == minsert->extrusion.y && ext.z == minsert->extrusion.z)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading extrusion");
    }

  // returns attriute
  attribs = dwg_ent_minsert_get_has_attribs (minsert, &attr_error);
  if (attr_error == 0 && attribs == minsert->has_attribs)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading attribs");
    }

  // returns owned object count
  obj_count = dwg_ent_minsert_get_owned_obj_count (minsert, &count_error);
  if (count_error == 0 && obj_count == minsert->owned_obj_count)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading object counts");
    }

  // returns num rows
  num_rows = dwg_ent_minsert_get_numrows (minsert, &num_row_error);
  if (num_row_error == 0 && num_rows == minsert->numrows)	// error checks
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading number of rows");
    }

  // returns number of columns
  num_cols = dwg_ent_minsert_get_numcols (minsert, &num_col_error);
  if (num_col_error == 0 && num_cols == minsert->numcols)	// error checking
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading number of columns");
    }

  // returns row space
  row_space = dwg_ent_minsert_get_row_spacing (minsert, &rowspace_error);
  if (rowspace_error == 0 && row_space == minsert->row_spacing)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading numspace");
    }

  // returns col space
  col_space = dwg_ent_minsert_get_col_spacing (minsert, &colspace_error);
  if (colspace_error == 0 && col_space == minsert->col_spacing)	// error checking
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading col_space");
    }
}
