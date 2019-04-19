#define DWG_TYPE DWG_TYPE_LINE
#include "common.c"
#include <dejagnu.h>

void
low_level_process (dwg_object *obj)
{

  // casts dwg object to line entity
  dwg_ent_line *line = dwg_object_to_LINE (obj);

  printf ("start points of line : x = %f,y = %f\n", line->start.x,
          line->start.y);
  printf ("end of line : x = %f,y = %f\n", line->end.x, line->end.y);
  printf ("Thickness of line : %f\n", line->thickness);
  printf ("extrusion of line : x = %f, y = %f, z = %f\n", line->extrusion.x,
          line->extrusion.y, line->extrusion.z);
}

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_BD thickness;
  dwg_point_3d ext, start, end;
  dwg_ent_line *line = dwg_object_to_LINE (obj);

  dwg_ent_line_get_start_point (line, &start, &error);
  if (!error && start.x == line->start.x
      && start.y == line->start.y) // error check
    pass ("Working Properly");
  else
    fail ("error in reading start points");

  dwg_ent_line_get_end_point (line, &end, &error);
  if (!error && line->end.x == end.x && line->end.y == end.y) // error check
    pass ("Working Properly");
  else
    fail ("error in reading end points");

  thickness = dwg_ent_line_get_thickness (line, &error);
  if (!error && line->thickness == thickness) // error check
    pass ("Working Properly");
  else
    fail ("error in reading thickness");

  dwg_ent_line_get_extrusion (line, &ext, &error);
  if (!error && line->extrusion.x == ext.x && line->extrusion.y == ext.y
      && line->extrusion.z == ext.z) // error check
    pass ("Working Properly");
  else
    fail ("error in reading extrusion");
}
