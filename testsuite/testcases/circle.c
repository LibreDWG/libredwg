#include "common.c"
#include <dejagnu.h>

/// Output processing function
void output_process (dwg_object * obj);

/// Checks the respective DWG entity/object type and then calls the output_process()
void
output_object (dwg_object * obj)
{
  if (!obj)
    {
      printf ("object is NULL\n");
      return;
    }

  if (dwg_get_type (obj) == DWG_TYPE_CIRCLE)
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
  int radius_error, thickness_error, ext_error, center_error;	// Error reporting
  float radius, thickness;
  dwg_point_3d center, ext;	//3d_points 

  // casting object to circle entity
  dwg_ent_circle *circle = dwg_object_to_CIRCLE (obj);

  // returns circle radius
  radius = dwg_ent_circle_get_radius (circle, &radius_error);
  if (radius_error == 0 && radius == circle->radius)	// Error checking
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading radius");
    }

  // returns circle thickness
  thickness = dwg_ent_circle_get_thickness (circle, &thickness_error);
  if (thickness_error == 0 && thickness == circle->thickness)	// error checking
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading thickness");
    }

  // returns circle extrusion
  dwg_ent_circle_get_extrusion (circle, &ext, &ext_error);
  if (ext_error == 0 && ext.x == circle->extrusion.x && ext.y == circle->extrusion.y && ext.z == circle->extrusion.z)	// error checking
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading extrusion");
    }

  // return circle center points
  dwg_ent_circle_get_center (circle, &center, &center_error);
  if (center_error == 0 && circle->center.x == center.x && circle->center.y == center.y && circle->center.z == center.z)	// error checking
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading center");
    }

}
