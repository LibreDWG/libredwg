#define DWG_TYPE DWG_TYPE_ARC
#include "common.c"
#include <dejagnu.h>

/* This function checks API functions for integrity
   @params dwg_object* obj 
 */
void
api_process (dwg_object * obj)
{
  int error;
  BITCODE_BD radius, thickness, start_angle, end_angle;
  dwg_point_3d center, ext;

  // casts an object to arc entity 
  dwg_ent_arc *arc = dwg_object_to_ARC (obj);

  // returns radius of arc
  radius = dwg_ent_arc_get_radius (arc, &error);
  if (error == 0 && radius == arc->radius)
    {
      pass ("arc_get_radius");
    }
  else
    {
      fail ("arc_get_radius error=%d radius=%f arc->radius=%f",
            error, radius, arc->radius);
    }

  // returns thickness of arc
  thickness = dwg_ent_arc_get_thickness (arc, &error);
  if (error == 0 && thickness == arc->thickness)
    {
      pass ("arc_get_thickness");
    }
  else
    {
      fail ("arc_get_thickness");
    }

  // Returns arc extrusion
  dwg_ent_arc_get_extrusion (arc, &ext, &error);
  if (error == 0 && ext.x == arc->extrusion.x &&
      ext.y == arc->extrusion.y && ext.z == arc->extrusion.z)
    {
      pass ("arc_get_extrusion");
    }
  else
    {
      fail ("arc_get_extrusion error=%d ext.x=%f arc->extrusion.x=%f",
            error, ext.x, arc->extrusion.x);
    }

  // returns center of arc
  dwg_ent_arc_get_center (arc, &center, &error);
  if (error == 0 && center.x == arc->center.x &&
      center.y == arc->center.y && center.z == arc->center.z)	// arc checking
    {
      pass ("arc_get_center");
    }
  else
    {
      fail ("arc_get_center error=%d center.x=%f arc->center.x=%f",
            error, center.x, arc->center.x);
    }

  // returns start angle  
  start_angle = dwg_ent_arc_get_start_angle (arc, &error);
  if (error == 0 && start_angle == arc->start_angle)
    {
      pass ("arc_get_start_angle");
    }
  else
    {
      fail ("arc_get_start_angle error=%d start_angle=%f arc->start_angle=%f",
            error, start_angle, arc->start_angle);
    }

  // returns end_angle
  end_angle = dwg_ent_arc_get_end_angle (arc, &error);
  if (error == 0 && end_angle == arc->end_angle)
    {
      pass ("arc_get_end_angle");
    }
  else
    {
      fail ("arc_get_end_angle error=%d end_angle=%f arc->end_angle=%f",
            error, end_angle, arc->end_angle);
    }
}
