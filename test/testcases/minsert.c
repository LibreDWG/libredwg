#define DWG_TYPE DWG_TYPE_MINSERT
#include "common.c"
#include <dejagnu.h>

void
low_level_process (dwg_object * obj)
{
  // casts dwg onject to minsert
  dwg_ent_minsert *minsert = dwg_object_to_MINSERT (obj);

  printf ("minsert points : x = %f, y = %f, z = %f\n",
	  minsert->ins_pt.x, minsert->ins_pt.y, minsert->ins_pt.z);
  printf ("scale flag for minsert : " FORMAT_BB "\n", minsert->scale_flag);
  printf ("scale points : x = %f, y = %f, z = %f\n",
	  minsert->scale.x, minsert->scale.y, minsert->scale.z);
  printf ("angle for minsert : %f\n", minsert->rotation);
  printf ("extrusion points : x = %f, y = %f, z = %f\n",
	  minsert->extrusion.x, minsert->extrusion.y, minsert->extrusion.z);
  printf ("attribs for minsert : " FORMAT_B "\n", minsert->has_attribs);
  printf ("object count for minsert : " FORMAT_BL "\n", minsert->num_owned);
  printf ("number of rows for minsert : " FORMAT_BS "\n", minsert->num_rows);
  printf ("number of columns for minsert : " FORMAT_BS "\n", minsert->num_cols);
  printf ("col space for minsert : %f\n", minsert->row_spacing);
  printf ("row space for minsert : %f\n", minsert->col_spacing);
}

void
api_process (dwg_object * obj)
{
  int error;
  dwg_point_3d ins_pt, ext, scale;
  double rot_angle;
  char has_attribs;
  BITCODE_BD col_space, row_space;
  BITCODE_BL obj_count, num_rows, num_cols;

  dwg_ent_minsert *minsert = dwg_object_to_MINSERT (obj);


  dwg_ent_minsert_get_ins_pt (minsert, &ins_pt, &error);
  if (!error  && ins_pt.x == minsert->ins_pt.x &&
      ins_pt.y == minsert->ins_pt.y && ins_pt.z == minsert->ins_pt.z)
    pass ("Working Properly");
  else
    fail ("error in reading insertion point");

  dwg_ent_minsert_get_scale (minsert, &scale, &error);
  if (!error  && scale.x == minsert->scale.x && scale.y == minsert->scale.y && scale.z == minsert->scale.z)
    pass ("Working Properly");
  else
    fail ("error in reading scale");


  rot_angle = dwg_ent_minsert_get_rotation (minsert, &error);
  if (!error  && rot_angle == minsert->rotation)
    pass ("Working Properly");
  else
    fail ("error in reading rotation angle");


  dwg_ent_minsert_get_extrusion (minsert, &ext, &error);
  if (!error  && ext.x == minsert->extrusion.x && ext.y == minsert->extrusion.y && ext.z == minsert->extrusion.z)
    pass ("Working Properly");
  else
    fail ("error in reading extrusion");


  has_attribs = dwg_ent_minsert_has_attribs (minsert, &error);
  if (!error  && has_attribs == minsert->has_attribs)
    pass ("Working Properly");
  else
    fail ("error in reading has_attribs");


  obj_count = dwg_ent_minsert_get_num_owned (minsert, &error);
  if (!error  && obj_count == minsert->num_owned)
    pass ("Working Properly");
  else
    fail ("error in reading object counts");


  num_rows = dwg_ent_minsert_get_num_rows (minsert, &error);
  if (!error  && num_rows == minsert->num_rows)
    pass ("Working Properly");
  else
    fail ("error in reading number of rows");


  num_cols = dwg_ent_minsert_get_num_cols (minsert, &error);
  if (!error  && num_cols == minsert->num_cols)
    pass ("Working Properly");
  else
    fail ("error in reading number of columns");


  row_space = dwg_ent_minsert_get_row_spacing (minsert, &error);
  if (!error  && row_space == minsert->row_spacing)
    pass ("Working Properly");
  else
    fail ("error in reading numspace");


  col_space = dwg_ent_minsert_get_col_spacing (minsert, &error);
  if (!error  && col_space == minsert->col_spacing)
    pass ("Working Properly");
  else
    fail ("error in reading col_space");
}
