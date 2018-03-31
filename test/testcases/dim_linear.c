#define DWG_TYPE DWG_TYPE_DIMENSION_LINEAR
#include "common.c"
#include <dejagnu.h>

/* This function checks API functions for integrity
   @params dwg_object* obj 
 */
void
api_process (dwg_object * obj)
{
  int error;
  double ext_line_rot, dim_rot;
  BITCODE_RC flags2;
  dwg_point_3d pt10, pt13, pt14;
  dwg_ent_dim_linear *dim_linear = dwg_object_to_DIMENSION_LINEAR (obj);

  dwg_ent_dim_linear_get_def_pt (dim_linear, &pt10, &error);
  if (!error  && dim_linear->def_pt.x == pt10.x && dim_linear->def_pt.y == pt10.y && dim_linear->def_pt.z == pt10.z)
    pass ("Working Properly");
  else
    fail ("error in reading pt10");

  dwg_ent_dim_linear_get_13_pt (dim_linear, &pt13, &error);
  if (!error  && dim_linear->_13_pt.x == pt13.x && dim_linear->_13_pt.y == pt13.y && dim_linear->_13_pt.z == pt13.z)
    pass ("Working Properly");
  else
    fail ("error in reading pt13");

  dwg_ent_dim_linear_get_14_pt (dim_linear, &pt14, &error);
  if (!error  && dim_linear->_14_pt.x == pt14.x && dim_linear->_14_pt.y == pt14.y && dim_linear->_14_pt.z == pt14.z)
    pass ("Working Properly");
  else
    fail ("error in reading pt14");

  ext_line_rot = dwg_ent_dim_linear_get_ext_line_rotation (dim_linear, &error);
  if (!error  && ext_line_rot == dim_linear->ext_line_rot)
    pass ("Working Properly");
  else
    fail ("error in reading ext line rot");

  dim_rot = dwg_ent_dim_linear_get_dim_rot (dim_linear, &error);
  if (!error  && dim_linear->dim_rot == dim_rot)
    pass ("Working Properly");
  else
    fail ("error in reading dim rot \n");
}
