#define DWG_TYPE DWG_TYPE_RAY
#include "common.c"

void
low_level_process (dwg_object *obj)
{
  dwg_ent_ray *ray = dwg_object_to_RAY (obj);

  printf ("points of ray : x = %f, y = %f, z = %f\n", ray->point.x,
          ray->point.y, ray->point.z);
  printf ("vector of ray : x = %f, y = %f, z = %f\n", ray->vector.x,
          ray->vector.y, ray->vector.z);
}

void
api_process (dwg_object *obj)
{
  int error;
  dwg_point_3d points, vector;
  dwg_ent_ray *ray = dwg_object_to_RAY (obj);

  dwg_ent_ray_get_point (ray, &points, &error);
  if (!error)
    printf ("points of ray : x = %f, y = %f, z = %f\n", points.x, points.y,
            points.z);
  else
    printf ("error in reading extrusion \n");

  dwg_ent_ray_get_vector (ray, &vector, &error);
  if (!error)
    printf ("vector of ray : x = %f, y = %f, z = %f\n", vector.x, vector.y,
            vector.z);
  else
    printf ("error in reading vector \n");
}
