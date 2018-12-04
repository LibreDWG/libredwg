#define DWG_TYPE DWG_TYPE_SOLID
#include "common.c"
#include <dejagnu.h>

void
low_level_process (dwg_object * obj)
{
  dwg_ent_solid *solid = dwg_object_to_SOLID (obj);

  printf ("elevation of solid : %f\n", solid->elevation);
  printf ("thickness of solid : %f\n", solid->thickness);
  printf ("extrusion of solid : x = %f, y = %f, z = %f\n",
          solid->extrusion.x, solid->extrusion.y, solid->extrusion.z);
  printf ("corner1 of solid : x = %f, y = %f\n", solid->corner1.x,
          solid->corner1.y);
  printf ("corner2 of solid : x = %f, y = %f\n", solid->corner2.x,
          solid->corner2.y);
  printf ("corner3 of solid : x = %f, y = %f\n", solid->corner3.x,
          solid->corner3.y);
  printf ("corner4 of solid : x = %f, y = %f\n", solid->corner4.x,
          solid->corner4.y);
}

void
api_process (dwg_object * obj)
{
  int error;
  BITCODE_BD thickness, elevation;
  dwg_point_3d ext;
  dwg_point_2d c1, c2, c3, c4;
  dwg_ent_solid *solid = dwg_object_to_SOLID (obj);


  elevation = dwg_ent_solid_get_elevation (solid, &error);
  if (!error  && solid->elevation == elevation) // error check
    pass ("Working Properly");
  else
    fail ("error in reading elevation");


  thickness = dwg_ent_solid_get_thickness (solid, &error);
  if (!error  && thickness == solid->thickness) // error check
    pass ("Working Properly");
  else
    fail ("error in reading thickness");


  dwg_ent_solid_get_extrusion (solid, &ext, &error);
  if (!error  && ext.x == solid->extrusion.x && ext.y == solid->extrusion.y && ext.z == solid->extrusion.z)     // error checks
    pass ("Working Properly");
  else
    fail ("error in reading extrusion");


  dwg_ent_solid_get_corner1 (solid, &c1, &error);
  if (!error  && solid->corner1.x == c1.x && solid->corner1.y == c1.y)  // error check
    pass ("Working Properly");
  else
    fail ("error in reading corner 1");


  dwg_ent_solid_get_corner2 (solid, &c2, &error);
  if (!error  && solid->corner2.x == c2.x && solid->corner2.y == c2.y)  // error check
    pass ("Working Properly");
  else
    fail ("error in reading corner 2");


  dwg_ent_solid_get_corner3 (solid, &c3, &error);
  if (!error  && solid->corner3.x == c3.x && solid->corner3.y == c3.y)  // error check
    pass ("Working Properly");
  else
    fail ("error in reading corner 3");


  dwg_ent_solid_get_corner4 (solid, &c4, &error);
  if (!error  && solid->corner4.x == c4.x && solid->corner4.y == c4.y)  // error check
    pass ("Working Properly");
  else
    fail ("error in reading corner 4 \n");

}
