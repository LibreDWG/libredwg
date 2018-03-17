#define DWG_TYPE DWG_TYPE_XLINE
#include "common.c"

void
low_level_process(dwg_object *obj)
{
  dwg_ent_xline *xline = dwg_object_to_XLINE(obj);

  printf("points of xline : x = %f, y = %f, z = %f\n", xline->point.x,
          xline->point.y, xline->point.z);
  printf("vector of xline : x = %f, y = %f, z = %f\n", xline->vector.x,
          xline->vector.y, xline->vector.z);
}

void
api_process(dwg_object *obj)
{
  int error;
  dwg_point_3d points, vector;

  dwg_ent_xline *xline = dwg_object_to_XLINE(obj);

  dwg_ent_xline_get_point(xline, &points,
                          &error);
  if ( !error )
      printf("points of xline : x = %f, y = %f, z = %f\n",
              points.x, points.y, points.z);
  else
      printf("error in reading extrusion \n");


  dwg_ent_xline_get_vector(xline, &vector,
                           &error);
  if ( !error )
      printf("vector of xline : x = %f, y = %f, z = %f\n",
              vector.x, vector.y, vector.z);
  else
      printf("error in reading vector \n");
}
