#define DWG_TYPE DWG_TYPE_BODY
#include "common.c"

void
low_level_process (dwg_object *obj)
{
  dwg_ent_solid *solid = dwg_object_to_SOLID (obj);

  printf ("elevation of solid : %f\n", solid->elevation);
  printf ("thickness of solid : %f\n", solid->thickness);
  printf ("extrusion of solid : x = %f, y = %f, z = %f\n", solid->extrusion.x,
          solid->extrusion.y, solid->extrusion.z);
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
api_process (dwg_object *obj)
{
  int error;
  double thickness, elevation;
  dwg_point_3d ext;
  dwg_point_2d c1, c2, c3, c4;
  dwg_ent_solid *solid = dwg_object_to_SOLID (obj);

  elevation = dwg_ent_solid_get_elevation (solid, &error);
  if (!error)
    printf ("elevation of solid : %f\n", elevation);
  else
    printf ("in reading elevation\n");

  thickness = dwg_ent_solid_get_thickness (solid, &error);
  if (!error)
    printf ("thickness of solid : %f\n", thickness);
  else
    printf ("in reading thickness\n");

  dwg_ent_solid_get_extrusion (solid, &ext, &error);
  if (!error)
    {
      printf ("extrusion of solid : x = %f, y = %f, z = %f\n", ext.x, ext.y,
              ext.z);
    }
  else
    {
      printf ("error in reading extrusion \n");
    }

  dwg_ent_solid_get_corner1 (solid, &c1, &error);
  if (!error)
    printf ("corner 1 of solid : x = %f, y = %f\n", c1.x, c1.y);
  else
    printf ("in reading corner 1 \n");

  dwg_ent_solid_get_corner2 (solid, &c2, &error);
  if (!error)
    printf ("corner 2 of solid : x = %f, y = %f\n", c2.x, c2.y);
  else
    printf ("in reading corner 2 \n");

  dwg_ent_solid_get_corner3 (solid, &c3, &error);
  if (!error)
    printf ("corner 3 of solid : x = %f, y = %f\n", c3.x, c3.y);
  else
    printf ("in reading corner 3 \n");

  dwg_ent_solid_get_corner4 (solid, &c4, &error);
  if (!error)
    printf ("solids of solid : x = %f, y = %f\n", c4.x, c4.y);
  else
    printf ("in reading corner 4 \n");
}
