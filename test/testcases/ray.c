#define DWG_TYPE DWG_TYPE_RAY
#include "common.c"
#include <dejagnu.h>

void
low_level_process (dwg_object * obj)
{
  dwg_ent_ray *ray = dwg_object_to_RAY (obj);

  printf ("points of ray : x = %f, y = %f, z = %f\n", ray->point.x,
          ray->point.y, ray->point.z);
  printf ("vector of ray : x = %f, y = %f, z = %f\n", ray->vector.x,
          ray->vector.y, ray->vector.z);
}

void
api_process (dwg_object * obj)
{
  int error;
  dwg_point_3d points, vector;
  dwg_ent_ray *ray = dwg_object_to_RAY (obj);


  dwg_ent_ray_get_point (ray, &points, &error);
  if (!error  && points.x == ray->point.x && points.y == ray->point.y && points.z == ray->point.z)      // error check
    pass ("Working Properly");
  else
    fail ("error in reading extrusion");


  dwg_ent_ray_get_vector (ray, &vector, &error);
  if (!error  && ray->vector.x == vector.x && ray->vector.y == vector.y && ray->vector.z == vector.z)   // error check
    pass ("Working Properly");
  else
    fail ("error in reading vector");
}
