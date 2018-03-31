#define DWG_TYPE DWG_TYPE_POLYLINE_2D
#include "common.c"

void
low_level_process(dwg_object *obj)
{
  dwg_ent_polyline_2d *polyline_2d = dwg_object_to_POLYLINE_2D(obj);
}

void
api_process(dwg_object *obj)
{
  int error;
  double start_width, end_width, elevation, thickness;
  BITCODE_BL owned_obj;
  unsigned int flags, curve_type;
  dwg_point_3d ext;
  dwg_ent_polyline_2d *polyline_2d = dwg_object_to_POLYLINE_2D(obj);


  dwg_ent_polyline_2d_get_extrusion(polyline_2d, &ext,
                                    &error);
  if ( !error )
  {
     printf("extrusion : x = %f, y = %f, z = %f\n",ext.x, ext.y, ext.z);
  }
  else
  {
    printf("error in reading extrusion");
  }


  start_width = dwg_ent_polyline_2d_get_start_width(polyline_2d, &error);
  if ( !error )
  {
     printf("start width of polyline : %f\n", start_width);
  }
  else
  {
    printf("error in reading start width");
  }


  end_width = dwg_ent_polyline_2d_get_end_width(polyline_2d, &error);
  if ( !error )
  {
     printf("end width of polyline : %f\n", end_width);
  }
  else
  {
    printf("error in reading end width");
  }


  thickness = dwg_ent_polyline_2d_get_thickness(polyline_2d, &error);
  if ( !error )
  {
     printf("thickness of polyline : %f\n", thickness);
  }
  else
  {
    printf("error in reading thickness");
  }


  elevation = dwg_ent_polyline_2d_get_elevation(polyline_2d, &error);
  if ( !error )
  {
     printf("elevation of polyline : %f\n", elevation);
  }
  else
  {
    printf("error in reading elevation");
  }


  flags = dwg_ent_polyline_2d_get_flag(polyline_2d, &error);
  if ( !error )
  {
     printf("flag of polyline : %ud\n", flags);
  }
  else
  {
    printf("error in reading flag");
  }


  curve_type = dwg_ent_polyline_2d_get_curve_type(polyline_2d, &error);
  if ( !error )
  {
     printf("curve type of polyline : %ud\n", curve_type);
  }
  else
  {
    printf("error in reading curve type");
  }


  owned_obj = dwg_ent_polyline_2d_get_owned_obj_count(polyline_2d,
               &error);
  if ( !error )
  {
     printf("owned object of polyline : " FORMAT_BL "\n", owned_obj);
  }
  else
  {
    printf("error in reading owned object count");
  }


}
