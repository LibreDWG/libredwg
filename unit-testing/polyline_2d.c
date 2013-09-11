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

  if (dwg_get_type(obj)== DWG_TYPE_POLYLINE_2D)
    {
      output_process(obj);
    }
}

void
low_level_process(dwg_object *obj)
{
  dwg_ent_polyline_2d *polyline_2d = dwg_object_to_POLYLINE_2D(obj);


}

void
api_process(dwg_object *obj)
{
  int radius_error, thickness_error, ext_error, center_error;
  float radius, thickness;
  dwg_point_3d center, ext;
  dwg_ent_polyline_2d *polyline_2d = dwg_object_to_POLYLINE_2D(obj);


}
