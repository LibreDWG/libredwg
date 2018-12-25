#define DWG_TYPE DWG_TYPE_POLYLINE_3D
#include "common.c"

void
low_level_process(dwg_object *obj)
{
  dwg_ent_polyline_3d *polyline_3d = dwg_object_to_POLYLINE_3D(obj);

  printf("flag of polyline_3d : " FORMAT_RC "\n", polyline_3d->flag);
  printf("curve_type of polyline_3d : " FORMAT_RC "\n", polyline_3d->curve_type);
  printf("numpoints of polyline_3d (r2004+): " FORMAT_BL "\n", polyline_3d->num_owned);
}

void
api_process(dwg_object *obj)
{
  int error;
  BITCODE_BL numpoints;
  BITCODE_RC flag, curve_type;

  dwg_ent_polyline_3d *polyline_3d = dwg_object_to_POLYLINE_3D(obj);

  flag = dwg_ent_polyline_3d_get_flag(polyline_3d, &error);
  if ( !error )
     printf("flag of polyline : " FORMAT_RC "\n", flag);
  else
    printf("error in reading flag");

  curve_type = dwg_ent_polyline_3d_get_curve_type(polyline_3d, &error);
  if ( !error )
     printf("curve_type of polyline : " FORMAT_RC "\n", curve_type);
  else
    printf("error in reading curve_type");

  numpoints = dwg_object_polyline_3d_get_numpoints(obj, &error);
  if ( !error )
     printf("numpoints of polyline_3d : " FORMAT_BL "\n", numpoints);
  else
    printf("error in reading numpoints");

}
