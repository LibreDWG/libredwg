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

  if (dwg_get_type(obj)== DWG_TYPE_XLINE)
    {
      output_process(obj);
    }
}

void
low_level_process(dwg_object *obj)
{
  dwg_ent_xline *xline = dwg_object_to_XLINE(obj);
  printf("points of xline : x = %f, y = %f, z = %f\t\n", xline->point.x,
          xline->point.y, xline->point.z);
  printf("vector of xline : x = %f, y = %f, z = %f\t\n", xline->vector.x,
          xline->vector.y, xline->vector.z);
}

void
api_process(dwg_object *obj)
{
  int vector_error, pt_error;
  dwg_point_3d points, vector;
  dwg_ent_xline *xline = dwg_object_to_XLINE(obj);

  dwg_ent_xline_get_point(xline, &points, &pt_error);
  if(pt_error == 0 )
    {
      printf("points of xline : x = %f, y = %f, z = %f\t\n",
              points.x, points.y, points.z);
    }
  else
    {
      printf("error in reading extrusion \n");
    }
  dwg_ent_xline_get_vector(xline, &vector, &vector_error);
  if(vector_error == 0 )
    {
      printf("vector of xline : x = %f, y = %f, z = %f\t\n",
              vector.x, vector.y, vector.z);
    }
  else
    {
      printf("error in reading vector \n");
    }
}
