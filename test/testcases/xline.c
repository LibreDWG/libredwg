#define DWG_TYPE DWG_TYPE_XLINE
#include "common.c"
#include <dejagnu.h>

void
api_process (dwg_object * obj)
{
  int error;
  dwg_point_3d points, vector;

  dwg_ent_xline *xline = dwg_object_to_XLINE (obj);


  dwg_ent_xline_get_point (xline, &points, &error);
  if (!error  && points.x == xline->point.x && points.y == xline->point.y && points.z == xline->point.z)        // error check
    pass ("Working Properly");
  else
    fail ("error in reading extrusion");

  dwg_ent_xline_get_vector (xline, &vector, &error);
  if (!error && vector.x == xline->vector.x && vector.y == xline->vector.y && vector.z == xline->vector.z)      // error check
    pass ("Working Properly");
  else
    fail ("error in reading vector");
}
