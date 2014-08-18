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

  if (dwg_get_type (obj) == DWG_TYPE_RAY)
    {
      output_process (obj);
    }
}

void
low_level_process (dwg_object * obj)
{

  // casts dwg object to ray entity
  dwg_ent_ray *ray = dwg_object_to_RAY (obj);

  // prints points of a ray
  printf ("points of ray : x = %f, y = %f, z = %f\t\n", ray->point.x,
	  ray->point.y, ray->point.z);

  // prints vectors of a ray
  printf ("vector of ray : x = %f, y = %f, z = %f\t\n", ray->vector.x,
	  ray->vector.y, ray->vector.z);
}

void
api_process (dwg_object * obj)
{
  int vector_error, pt_error;
  dwg_point_3d points, vector;
  dwg_ent_ray *ray = dwg_object_to_RAY (obj);

  // returns point
  dwg_ent_ray_get_point (ray, &points, &pt_error);
  if (pt_error == 0 && points.x == ray->point.x && points.y == ray->point.y && points.z == ray->point.z)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading extrusion");
    }

  // returns vector
  dwg_ent_ray_get_vector (ray, &vector, &vector_error);
  if (vector_error == 0 && ray->vector.x == vector.x && ray->vector.y == vector.y && ray->vector.z == vector.z)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading vector");
    }
}
