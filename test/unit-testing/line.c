#define DWG_TYPE DWG_TYPE_LINE
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  double thickness;
  dwg_point_3d pt3d, ext, start, end;
  BITCODE_B z_is_zero;

  dwg_ent_line *line = dwg_object_to_LINE (obj);

  CHK_ENTITY_3RD (line, LINE, start, start);
  dwg_ent_line_get_start_point (line, &pt3d, &error);
  if (error || memcmp (&start, &pt3d, sizeof (dwg_point_3d)))
    fail ("old API dwg_ent_line_get_start_point");

  CHK_ENTITY_3RD (line, LINE, end, end);
  dwg_ent_line_get_end_point (line, &pt3d, &error);
  if (error || memcmp (&end, &pt3d, sizeof (dwg_point_3d)))
    fail ("old API dwg_ent_line_get_end_point");

  CHK_ENTITY_TYPE_W_OLD (line, LINE, thickness, RD, thickness);
  CHK_ENTITY_3RD_W_OLD (line, LINE, extrusion, ext);

  CHK_ENTITY_TYPE (line, LINE, z_is_zero, B, z_is_zero);
}
