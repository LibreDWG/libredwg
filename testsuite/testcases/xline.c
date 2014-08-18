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

  if (dwg_get_type (obj) == DWG_TYPE_XLINE)
    {
      output_process (obj);
    }
}


void
api_process (dwg_object * obj)
{
  int vector_error, pt_error;
  dwg_point_3d points, vector;

  dwg_ent_xline *xline = dwg_object_to_XLINE (obj);


  // returns xline points
  dwg_ent_xline_get_point (xline, &points, &pt_error);
  if (pt_error == 0 && points.x == xline->point.x && points.y == xline->point.y && points.z == xline->point.z)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading extrusion");
    }

  // returns xline vectors
  dwg_ent_xline_get_vector (xline, &vector, &vector_error);
  if (vector_error == 0 && vector.x == xline->vector.x && vector.y == xline->vector.y && vector.z == xline->vector.z)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading vector");
    }
}
