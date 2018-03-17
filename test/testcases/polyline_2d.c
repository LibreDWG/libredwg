#define DWG_TYPE DWG_TYPE_POLYLINE_2D
#include "common.c"
#include <dejagnu.h>

void
low_level_process (dwg_object * obj)
{
  dwg_ent_polyline_2d *polyline_2d = dwg_object_to_POLYLINE_2D (obj);
}

void
api_process (dwg_object * obj)
{
  int error;
  BITCODE_BD start_width, end_width, elevation, thickness;
  BITCODE_BL owned_obj;
  unsigned int flags, curve_type;
  dwg_point_3d ext;
  dwg_ent_polyline_2d *polyline_2d = dwg_object_to_POLYLINE_2D (obj);


  dwg_ent_polyline_2d_get_extrusion (polyline_2d, &ext, &error);
  if (!error  && ext.x == polyline_2d->extrusion.x && ext.y == polyline_2d->extrusion.y && ext.z == polyline_2d->extrusion.z)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading extrusion");


  start_width =
    dwg_ent_polyline_2d_get_start_width (polyline_2d, &error);
  if (!error  && start_width == polyline_2d->start_width)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading start width");


  end_width = dwg_ent_polyline_2d_get_end_width (polyline_2d, &error);
  if (!error  && polyline_2d->end_width == end_width)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading end width");


  thickness =
    dwg_ent_polyline_2d_get_thickness (polyline_2d, &error);
  if (!error  && polyline_2d->thickness == thickness)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading thickness");


  elevation =
    dwg_ent_polyline_2d_get_elevation (polyline_2d, &error);
  if (!error  && polyline_2d->elevation == elevation)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading elevation");


  flags = dwg_ent_polyline_2d_get_flags (polyline_2d, &error);
  if (!error  && polyline_2d->flags == flags)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading flag");


  curve_type =
    dwg_ent_polyline_2d_get_curve_type (polyline_2d, &error);
  if (!error  && curve_type == polyline_2d->curve_type)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading curve type");


  owned_obj = dwg_ent_polyline_2d_get_owned_obj_count (polyline_2d,
						       &error);
  if (!error  &&
      owned_obj == polyline_2d->owned_obj_count)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading owned object count");
}
