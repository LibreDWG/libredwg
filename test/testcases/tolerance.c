#define DWG_TYPE DWG_TYPE_TOLERANCE
#include "common.c"
#include <dejagnu.h>

void
low_level_process (dwg_object *obj)
{
  dwg_ent_tolerance *tolerance = dwg_object_to_TOLERANCE (obj);

  printf ("Radius of tolerance : %f\n", tolerance->height);
  printf ("Thickness of tolerance : %f\n", tolerance->dimgap);
  printf ("text string of tolerance : %s\n", tolerance->text_string);
  printf ("extrusion of tolerance : x = %f, y = %f, z = %f\n",
          tolerance->extrusion.x, tolerance->extrusion.y,
          tolerance->extrusion.z);
  printf ("ins_pt of tolerance : x = %f, y = %f, z = %f\n",
          tolerance->ins_pt.x, tolerance->ins_pt.y, tolerance->ins_pt.z);
  printf ("center of tolerance : x = %f, y = %f, z = %f\n",
          tolerance->x_direction.x, tolerance->x_direction.y,
          tolerance->x_direction.z);
}

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_BD height, dimgap;
  dwg_point_3d ins_pt, x_dir, ext; // 3d_points
  char *text_string;

  // casting object to tolerance entity
  dwg_ent_tolerance *tolerance = dwg_object_to_TOLERANCE (obj);

  height = dwg_ent_tolerance_get_height (tolerance, &error);
  if (!error && height == tolerance->height) // Error checking
    pass ("Working Properly");
  else
    fail ("error in reading height");

  dimgap = dwg_ent_tolerance_get_dimgap (tolerance, &error);
  if (!error && dimgap == tolerance->dimgap) // error checking
    pass ("Working Properly");
  else
    fail ("error in reading dimgao");

  text_string = dwg_ent_tolerance_get_text_string (tolerance, &error);
  if (!error
      && !strcmp (text_string, tolerance->text_string)) // error checking
    pass ("Working Properly");
  else
    fail ("error in reading text");

  dwg_ent_tolerance_get_ins_pt (tolerance, &ins_pt, &error);
  if (!error && ins_pt.x == tolerance->ins_pt.x
      && ins_pt.y == tolerance->ins_pt.y && ins_pt.z == tolerance->ins_pt.z)
    pass ("Working Properly");
  else
    fail ("error in reading ins_pt");

  dwg_ent_tolerance_get_extrusion (tolerance, &ext, &error);
  if (!error && ext.x == tolerance->extrusion.x
      && ext.y == tolerance->extrusion.y && ext.z == tolerance->extrusion.z)
    pass ("Working Properly");
  else
    fail ("error in reading extrusion");

  dwg_ent_tolerance_get_x_direction (tolerance, &x_dir, &error);
  if (!error && tolerance->x_direction.x == x_dir.x
      && tolerance->x_direction.y == x_dir.y
      && tolerance->x_direction.z == x_dir.z)
    pass ("Working Properly");
  else
    fail ("error in reading x direction");
}
