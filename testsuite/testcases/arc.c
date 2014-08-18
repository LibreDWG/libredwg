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

  if (dwg_get_type (obj) == DWG_TYPE_ARC)
    {
      output_process (obj);
    }
}

/* This function checks API functions for integrity
   @params dwg_object* obj 
 */
void
api_process (dwg_object * obj)
{
  int radius_error, thickness_error, ext_error, center_error, start_error,
    end_error;
  float radius, thickness, start_angle, end_angle;
  dwg_point_3d center, ext;

  // casts an object to arc entity 
  dwg_ent_arc *arc = dwg_object_to_ARC (obj);

  // returns radius of arc
  radius = dwg_ent_arc_get_radius (arc, &radius_error);
  if (radius_error == 0 && radius == arc->radius)	// error checking 
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading radius");
    }

  // returns thickness of arc
  thickness = dwg_ent_arc_get_thickness (arc, &thickness_error);
  if (thickness_error == 0 && thickness == arc->thickness)	// error checking
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading thickness");
    }

  // Returns arc extrusion
  dwg_ent_arc_get_extrusion (arc, &ext, &ext_error);
  if (ext_error == 0 && ext.x == arc->extrusion.x && ext.y == arc->extrusion.y && ext.z == arc->extrusion.z)	// error checking 
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading extrusion");
    }

  // returns center of arc
  dwg_ent_arc_get_center (arc, &center, &center_error);
  if (center_error == 0 && center.x == arc->center.x && center.y == arc->center.y && center.z == arc->center.z)	// arc checking
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading center");
    }

  // returns start angle  
  start_angle = dwg_ent_arc_get_start_angle (arc, &start_error);
  if (start_error == 0 && start_angle == arc->start_angle)	// error checking
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading start angle");
    }

  // returns thickness angle
  thickness = dwg_ent_arc_get_end_angle (arc, &end_error);
  if (end_error == 0 && thickness == arc->thickness)
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading end angle");
    }
}
