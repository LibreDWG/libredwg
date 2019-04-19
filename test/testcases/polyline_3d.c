#define DWG_TYPE DWG_TYPE_POLYLINE_3D
#include "common.c"
#include <dejagnu.h>

void
low_level_process (dwg_object *obj)
{
  dwg_ent_polyline_3d *polyline_3d = dwg_object_to_POLYLINE_3D (obj);

  printf ("flag of polyline : " FORMAT_RC "\n", polyline_3d->flag);
  printf ("curve_type of polyline : " FORMAT_RC "\n", polyline_3d->curve_type);
  printf ("numpoints of polyline_3d (r2004+): " FORMAT_BL "\n",
          polyline_3d->num_owned);
}

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_BL numpoints;
  BITCODE_RC flag, curve_type;
  dwg_ent_polyline_3d *polyline_3d = dwg_object_to_POLYLINE_3D (obj);

  flag = dwg_ent_polyline_3d_get_flag (polyline_3d, &error);
  if (!error && flag == polyline_3d->flag)
    pass ("Working Properly");
  else
    fail ("error in reading flag");

  curve_type = dwg_ent_polyline_3d_get_curve_type (polyline_3d, &error);
  if (!error && curve_type == polyline_3d->curve_type)
    pass ("Working Properly");
  else
    fail ("error in reading curve_type");

  numpoints = dwg_object_polyline_3d_get_numpoints (obj, &error);
  // FIXME: only valid for r2004+
  if (!error && polyline_3d->num_owned == numpoints)
    pass ("Working Properly");
  else
    fail ("error in reading numpoints");
}
