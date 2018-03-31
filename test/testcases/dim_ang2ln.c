#define DWG_TYPE DWG_TYPE_DIMENSION_ANG2LN
#include "common.c"
#include <dejagnu.h>

void
api_process (dwg_object * obj)
{
  int error;
  BITCODE_RC flags2;
  dwg_point_2d pt16;
  dwg_point_3d pt10, pt13, pt14, pt15, ext, ins_scale;
  dwg_ent_dim_ang2ln *dim_ang2ln = dwg_object_to_DIMENSION_ANG2LN (obj);

  dwg_ent_dim_ang2ln_get_def_pt (dim_ang2ln, &pt10, &error);
  if (!error  && dim_ang2ln->def_pt.x == pt10.x && dim_ang2ln->def_pt.y == pt10.y && dim_ang2ln->def_pt.z == pt10.z)
    pass ("Working Properly");
  else
    fail ("error in reading def_pt");

  dwg_ent_dim_ang2ln_get_13_pt (dim_ang2ln, &pt13, &error);
  if (!error  && dim_ang2ln->_13_pt.x == pt13.x && dim_ang2ln->_13_pt.y == pt13.y && dim_ang2ln->_13_pt.z == pt13.z)
    pass ("Working Properly");
  else
    fail ("error in reading pt13");

  dwg_ent_dim_ang2ln_get_14_pt (dim_ang2ln, &pt14, &error);
  if (!error  && dim_ang2ln->_14_pt.x == pt14.x && dim_ang2ln->_14_pt.y == pt14.y && dim_ang2ln->_14_pt.z == pt14.z)
    pass ("Working Properly");
  else
    fail ("error in reading pt14");

  dwg_ent_dim_ang2ln_get_first_arc_pt (dim_ang2ln, &pt15, &error);
  if (!error  && dim_ang2ln->first_arc_pt.x == pt15.x && dim_ang2ln->first_arc_pt.y == pt15.y && dim_ang2ln->first_arc_pt.z == pt15.z)
    pass ("Working Properly");
  else
    fail ("error in reading first_arc_pt");

  dwg_ent_dim_ang2ln_get_16_pt (dim_ang2ln, &pt16, &error);
  if (!error  && dim_ang2ln->_16_pt.x == pt16.x && dim_ang2ln->_16_pt.y == pt16.y)
    pass ("Working Properly");
  else
    fail ("error in reading pt16");

}
