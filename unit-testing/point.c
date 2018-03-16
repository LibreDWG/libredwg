#include "common.c"

void
output_object(dwg_object* obj)
{
  if (!obj)
    {
      printf("object is NULL\n");
      return;
    }

  if (dwg_get_type(obj)== DWG_TYPE_POINT)
    {
      output_process(obj);
    }
}

void
low_level_process(dwg_object *obj)
{
  // casts object to point entity
  dwg_ent_point *point = dwg_object_to_POINT(obj);

  // prints point of point
  printf("points of point : x = %f, y = %f, z = %f\n", 
          point->x, point->y, point->z);

  // prints extrusion of point
  printf("extrusion of point : x = %f, y = %f, z = %f\n",
          point->extrusion.x, point->extrusion.y, point->extrusion.z);

  // prints thickness
  printf("thickness of point : %f\n", point->thickness);
}

void
api_process(dwg_object *obj)
{
  int error;
  double thickness;
  dwg_point_3d ext, points;
  dwg_ent_point *point = dwg_object_to_POINT(obj);
 
  // returns point values
  dwg_ent_point_get_point(point, &points,
                          &error);
  if ( !error )
    {
      printf("points of point : x = %f, y = %f, z = %f\n",
              points.x, points.y, points.z);
    }
  else
    {
      printf("error in reading point \n");
    }

  // returns extrusion
  dwg_ent_point_get_extrusion(point, &ext,
                              &error);
  if ( !error )
    {
      printf("extrusion of point : x = %f, y = %f, z = %f\n",
              ext.x, ext.y, ext.z);
    }
  else
    {
      printf("error in reading extrusion \n");
    }

  // returns thickness
  thickness = dwg_ent_point_get_thickness(point, &error);
  if ( !error )
    {
      printf("thickness of point : %f\n", thickness);
    }
  else
    {
      printf("error in reading thickness\n");
    }
}
