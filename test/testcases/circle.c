#define DWG_TYPE DWG_TYPE_CIRCLE
#include "common.c"
#include <dejagnu.h>

/* This function checks API functions for integrity
   @params dwg_object* obj
 */
void
api_process (dwg_object * obj)
{
  int error;    // Error reporting
  BITCODE_BD radius, thickness;
  dwg_point_3d center, ext;     //3d_points

  // casting object to circle entity
  dwg_ent_circle *circle = dwg_object_to_CIRCLE (obj);


  radius = dwg_ent_circle_get_radius (circle, &error);
  if (!error  && radius == circle->radius)      // Error checking
    pass ("Working Properly");
  else
    fail ("error in reading radius");


  thickness = dwg_ent_circle_get_thickness (circle, &error);
  if (!error  && thickness == circle->thickness)        // error checking
    pass ("Working Properly");
  else
    fail ("error in reading thickness");


  dwg_ent_circle_get_extrusion (circle, &ext, &error);
  if (!error  && ext.x == circle->extrusion.x && ext.y == circle->extrusion.y && ext.z == circle->extrusion.z)  // error checking
    pass ("Working Properly");
  else
    fail ("error in reading extrusion");

  // return circle center points
  dwg_ent_circle_get_center (circle, &center, &error);
  if (!error  && circle->center.x == center.x && circle->center.y == center.y && circle->center.z == center.z)  // error checking
    pass ("Working Properly");
  else
    fail ("error in reading center");

}
