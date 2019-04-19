#define DWG_TYPE DWG_TYPE_LINE
#include "common.c"

void
low_level_process (dwg_object *obj)
{
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
  double thickness;
  dwg_point_3d ext, start, end;
  dwg_ent_line *line = dwg_object_to_LINE (obj);

  dwg_ent_line_get_start_point (line, &start, &error);
  if (!error)
    {
      printf ("start points of line : x = %f, y = %f\n", start.x, start.y);
    }
  else
    {
      printf ("error in reading start points \n");
    }

  dwg_ent_line_get_end_point (line, &end, &error);
  if (!error)
    {
      printf ("end points of line : x = %f, y = %f\n", end.x, end.y);
    }
  else
    {
      printf ("error in reading end points \n");
    }

  thickness = dwg_ent_line_get_thickness (line, &error);
  if (!error)
    printf ("Thickness of line : %f\n", thickness);
  else
    printf ("in reading thickness \n");

  dwg_ent_line_get_extrusion (line, &ext, &error);
  if (!error)
    {
      printf ("extrusion of line : x = %f, y = %f, z = %f\n", ext.x, ext.y,
              ext.z);
    }
  else
    {
      printf ("error in reading extrusion \n");
    }
}
