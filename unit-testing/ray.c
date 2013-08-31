#include "common.c"

void
output_process(dwg_object *obj);

void
output_object(dwg_object* obj){
  if (!obj)
    {
      printf("object is NULL\n");
      return;
    }

  if (dwg_get_type(obj)== DWG_TYPE_RAY)
    {
      output_process(obj);
    }
}

void
low_level_process(dwg_object *obj)
{
  dwg_ent_ray *ray = dwg_object_to_RAY(obj);
  printf("points of ray : x = %f, y = %f, z = %f\t\n", ray->point.x,
          ray->point.y, ray->point.z);
  printf("vector of ray : x = %f, y = %f, z = %f\t\n", ray->vector.x,
          ray->vector.y, ray->vector.z);
}

void
api_process(dwg_object *obj)
{
  int vector_error, pt_error;
  dwg_point_3d points, vector;
  dwg_ent_ray *ray = dwg_object_to_RAY(obj);

  dwg_ent_ray_get_point(ray, &points, &pt_error);
  if(pt_error == 0 )
    {
      printf("points of ray : x = %f, y = %f, z = %f\t\n",
              points.x, points.y, points.z);
    }
  else
    {
      printf("error in reading extrusion \n");
    }
  dwg_ent_ray_get_vector(ray, &vector, &vector_error);
  if(vector_error == 0 )
    {
      printf("vector of ray : x = %f, y = %f, z = %f\t\n",
              vector.x, vector.y, vector.z);
    }
  else
    {
      printf("error in reading vector \n");
    }
}
