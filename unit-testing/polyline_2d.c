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
  int start_w_error, end_w_error, ext_error, thickness_error, elevation_error,
      curve_type_error, flags_error, owned_obj_error;
  float start_width, end_width, elevation, thickness;
  long owned_obj;
  unsigned int flags, curve_type;
  dwg_point_3d ext;
  dwg_ent_polyline_2d *polyline_2d = dwg_object_to_POLYLINE_2D(obj);

  // returns extrusion points
  dwg_ent_polyline_2d_get_extrusion(polyline_2d, &ext, &ext_error);
  if( ext_error == 0 ) // error check
  {
     printf("extrusion : x = %f, y = %f, z = %f\t\n",ext.x, ext.y, ext.z);
  }
  else
  {
    printf("error in reading extrusion");
  }

  // returns start width
  start_width = dwg_ent_polyline_2d_get_start_width(polyline_2d, &start_w_error);
  if( start_w_error == 0 ) // error check
  {
     printf("start width of polyline : %f\t\n", start_width);
  }
  else
  {
    printf("error in reading start width");
  }

  // returns end width
  end_width = dwg_ent_polyline_2d_get_end_width(polyline_2d, &end_w_error);
  if( end_w_error == 0 ) // error check
  {
     printf("end width of polyline : %f\t\n", end_width);
  }
  else
  {
    printf("error in reading end width");
  }

  // returns thickness
  thickness = dwg_ent_polyline_2d_get_thickness(polyline_2d, &thickness_error);
  if( thickness_error == 0 ) // error check
  {
     printf("thickness of polyline : %f\t\n", thickness);
  }
  else
  {
    printf("error in reading thickness");
  }

  // returns elevation
  elevation = dwg_ent_polyline_2d_get_elevation(polyline_2d, &elevation_error);
  if( elevation_error == 0 ) // error check
  {
     printf("elevation of polyline : %f\t\n", elevation);
  }
  else
  {
    printf("error in reading elevation");
  }

  // returns flags
  flags = dwg_ent_polyline_2d_get_flags(polyline_2d, &flags_error);
  if( flags_error == 0 ) // error check
  {
     printf("flag of polyline : %ud\t\n", flags);
  }
  else
  {
    printf("error in reading flag");
  }

  // returns curve type
  curve_type = dwg_ent_polyline_2d_get_curve_type(polyline_2d, &curve_type_error);
  if( curve_type_error == 0 ) // error check
  {
     printf("curve type of polyline : %ud\t\n", curve_type);
  }
  else
  {
    printf("error in reading curve type");
  }

  // returns owned object count
  owned_obj = dwg_ent_polyline_2d_get_owned_obj_count(polyline_2d,
               &owned_obj_error);
  if( owned_obj_error == 0 ) // error check
  {
     printf("owned object of polyline : %ld\t\n", owned_obj);
  }
  else
  {
    printf("error in reading owned object count");
  }


}
