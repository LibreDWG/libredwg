#define DWG_TYPE DWG_TYPE_DIMENSION_ALIGNED
#include "common.c"
#include <dejagnu.h>

/* This function checks API functions for integrity
   @params dwg_object* obj
 */
void
api_process (dwg_object * obj)
{
  int error;
  double ext_line_rot;
  BITCODE_RC flags2;
  dwg_point_3d pt10, pt13, pt14;
  dwg_ent_dim_aligned *dim_aligned = dwg_object_to_DIMENSION_ALIGNED (obj);

  dwg_ent_dim_aligned_get_def_pt (dim_aligned, &pt10, &error);
  if (!error  && pt10.x == dim_aligned->def_pt.x && pt10.y == dim_aligned->def_pt.y && pt10.z == dim_aligned->def_pt.z)
    pass ("Working Properly");
  else
    fail ("error in reading def_pt");

  dwg_ent_dim_aligned_get_13_pt (dim_aligned, &pt13, &error);
  if (!error  && pt13.x == dim_aligned->_13_pt.x && pt13.y == dim_aligned->_13_pt.y && pt13.z == dim_aligned->_13_pt.z)
    pass ("Working Properly");
  else
    fail ("error in reading pt13");

  dwg_ent_dim_aligned_get_14_pt (dim_aligned, &pt14, &error);
  if (!error  && dim_aligned->_14_pt.x == pt14.x && dim_aligned->_14_pt.y == pt14.y && dim_aligned->_14_pt.z == pt14.z)
    pass ("Working Properly");
  else
    fail ("error in reading pt14");

  ext_line_rot = dwg_ent_dim_aligned_get_ext_line_rotation (dim_aligned, &error);
  if (!error  && ext_line_rot == dim_aligned->ext_line_rotation)
    pass ("Working Properly");
  else
    fail ("error in reading ext line rotation");

}
