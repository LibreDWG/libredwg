#define DWG_TYPE DWG_TYPE_POINT
#include "common.c"
#include <dejagnu.h>

void
low_level_process (dwg_object * obj)
{
  dwg_ent_point *point = dwg_object_to_POINT (obj);

  printf ("points of point : x = %f, y = %f, z = %f\n",
	  point->x, point->y, point->z);
  printf ("extrusion of point : x = %f, y = %f, z = %f\n",
	  point->extrusion.x, point->extrusion.y, point->extrusion.z);
  printf ("thickness of point : %f\n", point->thickness);
}

void
api_process (dwg_object * obj)
{
  int error;
  BITCODE_BD thickness;
  dwg_point_3d ext, points;
  dwg_ent_point *point = dwg_object_to_POINT (obj);


  dwg_ent_point_get_point (point, &points, &error);
  if (!error  && point->x == points.x && point->y == points.y && point->z == points.z)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading point");


  dwg_ent_point_get_extrusion (point, &ext, &error);
  if (!error  && ext.x == point->extrusion.x && ext.y == point->extrusion.y && ext.z == point->extrusion.z)	// error checking
    pass ("Working Properly");
  else
    fail ("error in reading extrusion");


  thickness = dwg_ent_point_get_thickness (point, &error);
  if (!error  && thickness == point->thickness)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading thickness");
}
