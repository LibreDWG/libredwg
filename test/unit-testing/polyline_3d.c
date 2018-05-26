#define DWG_TYPE DWG_TYPE_POLYLINE_3D
#include "common.c"

void
low_level_process(dwg_object *obj)
{
  dwg_ent_polyline_3d *polyline_3d = dwg_object_to_POLYLINE_3D(obj); 

  printf("flag1 of polyline_3d : " FORMAT_RC "\n", polyline_3d->flag);
  printf("flag2 of polyline_3d : " FORMAT_RC "\n", polyline_3d->flag2);
  printf("numpoints of polyline_3d (r2004+): " FORMAT_BL "\n", polyline_3d->num_owned);
}

void
api_process(dwg_object *obj)
{
  int error;
  BITCODE_BL numpoints;
  BITCODE_RC flags1, flags2;

  dwg_ent_polyline_3d *polyline_3d = dwg_object_to_POLYLINE_3D(obj);

  flags1 = dwg_ent_polyline_3d_get_flag(polyline_3d, &error);
  if ( !error )
     printf("flag1 of polyline : " FORMAT_RC "\n", flags1);
  else
    printf("error in reading flag1");

  flags2 = dwg_ent_polyline_3d_get_flag2(polyline_3d, &error);
  if ( !error )
     printf("flag2 of polyline : " FORMAT_RC "\n", flags2);
  else
    printf("error in reading flag2");

  numpoints = dwg_obj_polyline_3d_get_numpoints(obj, &error);
  if ( !error )
     printf("numpoints of polyline_3d : " FORMAT_BL "\n", numpoints);
  else
    printf("error in reading numpoints");

}
