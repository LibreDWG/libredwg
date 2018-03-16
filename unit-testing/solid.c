#define DWG_TYPE DWG_TYPE_BODY
#include "common.c"

void
low_level_process(dwg_object *obj)
{
  // casts object to solid entity
  dwg_ent_solid *solid = dwg_object_to_SOLID(obj);

  // prints elevation
  printf("elevation of solid : %f\n", solid->elevation);

  // prints thickness
  printf("thickness of solid : %f\n", solid->thickness);

  // prints extrusion
  printf("extrusion of solid : x = %f, y = %f, z = %f\n",
          solid->extrusion.x, solid->extrusion.y, solid->extrusion.z);

  // prints corner 1
  printf("corner1 of solid : x = %f, y = %f\n", solid->corner1.x,
          solid->corner1.y);

  // prints corner 2
  printf("corner2 of solid : x = %f, y = %f\n", solid->corner2.x,
          solid->corner2.y);

  // prints corner 3
  printf("corner3 of solid : x = %f, y = %f\n", solid->corner3.x,
          solid->corner3.y);

  // prints corner 4
  printf("corner4 of solid : x = %f, y = %f\n", solid->corner4.x,
          solid->corner4.y);

}

void
api_process(dwg_object *obj)
{
  int error;
  double thickness, elevation;
  dwg_point_3d ext;
  dwg_point_2d c1,c2,c3,c4;
  dwg_ent_solid *solid = dwg_object_to_SOLID(obj);

  // prints elevation
  elevation  = dwg_ent_solid_get_elevation(solid, &error);
  if ( !error )
    {
      printf("elevation of solid : %f\n", elevation);
    }
  else
    {
      printf("error in reading elevation\n");
    }
 
  // returns thickness
  thickness = dwg_ent_solid_get_thickness(solid, &error);
  if ( !error )
    {
      printf("thickness of solid : %f\n", thickness);
    }
  else
    {
      printf("error in reading thickness\n");
    }

  // returns extrusion
  dwg_ent_solid_get_extrusion(solid, &ext,
                              &error);
  if ( !error )
    {
      printf("extrusion of solid : x = %f, y = %f, z = %f\n",
              ext.x, ext.y, ext.z);
    }
  else
    {
      printf("error in reading extrusion \n");
    }

  // returns corner 1
  dwg_ent_solid_get_corner1(solid, &c1,
                            &error);
  if ( !error )
    {
      printf("corner 1 of solid : x = %f, y = %f\n", c1.x, c1.y);
    }
  else
    {
      printf("error in reading corner 1 \n");
    }

  // returns corner 2
  dwg_ent_solid_get_corner2(solid, &c2,
                            &error);
  if ( !error )
    {
      printf("corner 2 of solid : x = %f, y = %f\n", c2.x, c2.y);
    }
  else
    {
      printf("error in reading corner 2 \n");
    }

  // returns corner 3
  dwg_ent_solid_get_corner3(solid, &c3,
                            &error);
  if ( !error )
    {
      printf("corner 3 of solid : x = %f, y = %f\n", c3.x, c3.y);
    }
  else
    {
      printf("error in reading corner 3 \n");
    }

  // returns corner 4
  dwg_ent_solid_get_corner4(solid, &c4,
                            &error);
  if ( !error )
    {
      printf("solids of solid : x = %f, y = %f\n", c4.x, c4.y);
    }
  else
    {
      printf("error in reading corner 4 \n");
    }

}
