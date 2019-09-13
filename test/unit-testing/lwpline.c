#define DWG_TYPE DWG_TYPE_LWPOLYLINE
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_BS flag;
  BITCODE_BL num_points, num_bulges, num_widths;
  dwg_point_3d extrusion, pt3d;
  char flags;
  double const_width, elevation, thickness;
  double *bulges;
  dwg_point_2d *points;
  dwg_lwpline_widths *width;
  BITCODE_BL *vertexids;
  BITCODE_BL num_vertexids, i;

  dwg_ent_lwpline *lwpline = dwg_object_to_LWPOLYLINE (obj);

  CHK_ENTITY_TYPE (lwpline, LWPOLYLINE, flag, BS, flag);
  if (dwg_ent_lwpline_get_flag (lwpline, &error) != flag || error)
    fail ("old API dwg_ent_lwpline_get_flag");
  CHK_ENTITY_3RD (lwpline, LWPOLYLINE, extrusion, extrusion);
  dwg_ent_lwpline_get_extrusion (lwpline, &pt3d, &error);
  if (error || memcmp (&extrusion, &pt3d, sizeof (extrusion)))
    fail ("old API dwg_ent_lwpline_get_extrusion");

  CHK_ENTITY_TYPE (lwpline, LWPOLYLINE, const_width, BD, const_width);
  if (dwg_ent_lwpline_get_const_width (lwpline, &error) != const_width
      || error)
    fail ("old API dwg_ent_lwpline_get_const_width");
  CHK_ENTITY_TYPE (lwpline, LWPOLYLINE, thickness, BD, thickness);
  if (dwg_ent_lwpline_get_thickness (lwpline, &error) != thickness || error)
    fail ("old API dwg_ent_lwpline_get_thickness");
  CHK_ENTITY_TYPE (lwpline, LWPOLYLINE, elevation, BD, elevation);
  if (dwg_ent_lwpline_get_elevation (lwpline, &error) != elevation || error)
    fail ("old API dwg_ent_lwpline_get_elevation");
  CHK_ENTITY_TYPE (lwpline, LWPOLYLINE, num_widths, BL, num_widths);
  if (dwg_ent_lwpline_get_numwidths (lwpline, &error) != num_widths || error)
    fail ("old API dwg_ent_lwpline_get_numwidths");
  CHK_ENTITY_TYPE (lwpline, LWPOLYLINE, num_bulges, BL, num_bulges);
  if (dwg_ent_lwpline_get_numbulges (lwpline, &error) != num_bulges || error)
    fail ("old API dwg_ent_lwpline_get_numbulges");
  CHK_ENTITY_TYPE (lwpline, LWPOLYLINE, num_points, BL, num_points);
  if (dwg_ent_lwpline_get_numpoints (lwpline, &error) != num_points || error)
    fail ("old API dwg_ent_lwpline_get_numpoints");

  // CHK_ENTITY_TYPE (lwpline, LWPOLYLINE, bulges, BD*, bulges);
  bulges = dwg_ent_lwpline_get_bulges (lwpline, &error);
  if (!error)
    {
      for (i = 0; i < lwpline->num_bulges; i++)
        (void)bulges[i];
      // printf ("bulge[%d] of lwpline : %f\n", (int)i, bulges[i]);
      free (bulges);
    }
  else
    fail ("bulges");

  points = dwg_ent_lwpline_get_points (lwpline, &error);
  if (!error)
    {
      for (i = 0; i < lwpline->num_points; i++)
        (void)points[i].x;
      // printf ("point[%d] of lwpline : x = %f\ty = %f\n", (int)i,
      // points[i].x,
      //        points[i].y);
      free (points);
    }
  else
    fail ("points");

  width = dwg_ent_lwpline_get_widths (lwpline, &error);
  if (!error)
    {
      for (i = 0; i < lwpline->num_widths; i++)
        (void)width[i].start;
      // printf ("widths[%d] of lwpline : x = %f\ty = %f\n", (int)i,
      //        width[i].start, width[i].end);
      free (width);
    }
  else
    fail ("widths");

  CHK_ENTITY_TYPE (lwpline, LWPOLYLINE, num_vertexids, BL, num_vertexids);
}
