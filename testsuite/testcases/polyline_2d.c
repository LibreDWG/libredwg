#include "common.c"
#include <dejagnu.h>
void output_process (dwg_object * obj);

void
output_object (dwg_object * obj)
{
  if (!obj)
    {
      printf ("object is NULL\n");
      return;
    }

  if (dwg_get_type (obj) == DWG_TYPE_POLYLINE_2D)
    {
      output_process (obj);
    }
}

void
low_level_process (dwg_object * obj)
{
  dwg_ent_polyline_2d *polyline_2d = dwg_object_to_POLYLINE_2D (obj);


}

void
api_process (dwg_object * obj)
{
  int start_w_error, end_w_error, ext_error, thickness_error, elevation_error,
    curve_type_error, flags_error, owned_obj_error;
  float start_width, end_width, elevation, thickness;
  long owned_obj;
  unsigned int flags, curve_type;
  dwg_point_3d ext;
  dwg_ent_polyline_2d *polyline_2d = dwg_object_to_POLYLINE_2D (obj);

  // returns extrusion points
  dwg_ent_polyline_2d_get_extrusion (polyline_2d, &ext, &ext_error);
  if (ext_error == 0 && ext.x == polyline_2d->extrusion.x && ext.y == polyline_2d->extrusion.y && ext.z == polyline_2d->extrusion.z)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading extrusion");
    }

  // returns start width
  start_width =
    dwg_ent_polyline_2d_get_start_width (polyline_2d, &start_w_error);
  if (start_w_error == 0 && start_width == polyline_2d->start_width)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading start width");
    }

  // returns end width
  end_width = dwg_ent_polyline_2d_get_end_width (polyline_2d, &end_w_error);
  if (end_w_error == 0 && polyline_2d->end_width == end_width)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading end width");
    }

  // returns thickness
  thickness =
    dwg_ent_polyline_2d_get_thickness (polyline_2d, &thickness_error);
  if (thickness_error == 0 && polyline_2d->thickness == thickness)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading thickness");
    }

  // returns elevation
  elevation =
    dwg_ent_polyline_2d_get_elevation (polyline_2d, &elevation_error);
  if (elevation_error == 0 && polyline_2d->elevation == elevation)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading elevation");
    }

  // returns flags
  flags = dwg_ent_polyline_2d_get_flags (polyline_2d, &flags_error);
  if (flags_error == 0 && polyline_2d->flags == flags)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading flag");
    }

  // returns curve type
  curve_type =
    dwg_ent_polyline_2d_get_curve_type (polyline_2d, &curve_type_error);
  if (curve_type_error == 0 && curve_type == polyline_2d->curve_type)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading curve type");
    }

  // returns owned object count
  owned_obj = dwg_ent_polyline_2d_get_owned_obj_count (polyline_2d,
						       &owned_obj_error);
  if (owned_obj_error == 0 && owned_obj == polyline_2d->owned_obj_count)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading owned object count");
    }


}
