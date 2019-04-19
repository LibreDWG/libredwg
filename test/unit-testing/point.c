#define DWG_TYPE DWG_TYPE_POINT
#include "common.c"

void
low_level_process (dwg_object *obj)
{
  dwg_ent_point *point = dwg_object_to_POINT (obj);

  printf ("points of point : x = %f, y = %f, z = %f\n", point->x, point->y,
          point->z);
  printf ("extrusion of point : x = %f, y = %f, z = %f\n", point->extrusion.x,
          point->extrusion.y, point->extrusion.z);
  printf ("thickness of point : %f\n", point->thickness);
}

void
api_process (dwg_object *obj)
{
  int error;
  double thickness;
  dwg_point_3d ext, points;
  dwg_ent_point *point = dwg_object_to_POINT (obj);

  dwg_ent_point_get_point (point, &points, &error);
  if (!error)
    {
      printf ("points of point : x = %f, y = %f, z = %f\n", points.x, points.y,
              points.z);
    }
  else
    {
      printf ("error in reading point \n");
    }

  dwg_ent_point_get_extrusion (point, &ext, &error);
  if (!error)
    {
      printf ("extrusion of point : x = %f, y = %f, z = %f\n", ext.x, ext.y,
              ext.z);
    }
  else
    {
      printf ("error in reading extrusion \n");
    }

  thickness = dwg_ent_point_get_thickness (point, &error);
  if (!error)
    {
      printf ("thickness of point : %f\n", thickness);
    }
  else
    {
      printf ("error in reading thickness\n");
    }
}
