#define DWG_TYPE DWG_TYPE_POLYLINE_3D
#include "common.c"
#include <dejagnu.h>

void
low_level_process (dwg_object * obj)
{
  dwg_ent_polyline_3d *polyline_3d = dwg_object_to_POLYLINE_3D (obj);

  printf ("flag1 of polyline : " FORMAT_RC "\n", polyline_3d->flag);
  printf ("flag2 of polyline : " FORMAT_RC "\n", polyline_3d->flag2);
  printf("numpoints of polyline_3d (r2004+): " FORMAT_BL "\n", polyline_3d->owned_obj_count);
}

void
api_process (dwg_object * obj)
{
  int error;
  BITCODE_BL numpoints;
  BITCODE_RC flags1, flags2;
  dwg_ent_polyline_3d *polyline_3d = dwg_object_to_POLYLINE_3D (obj);

  flags1 = dwg_ent_polyline_3d_get_flag (polyline_3d, &error);
  if (!error  && flags1 == polyline_3d->flag)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading flag1");

  flags2 = dwg_ent_polyline_3d_get_flag2 (polyline_3d, &error);
  if (!error  && flags2 == polyline_3d->flag2)
    pass ("Working Properly");
  else
    fail ("error in reading flag2");

  numpoints = dwg_obj_polyline_3d_get_numpoints(obj, &error);
  //FIXME: only valid for r2004+
  if (!error && polyline_3d->owned_obj_count == numpoints)
    pass ("Working Properly");
  else
    fail ("error in reading numpoints");


}
