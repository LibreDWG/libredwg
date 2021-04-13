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

  CHK_ENTITY_TYPE (lwpline, LWPOLYLINE, flag, BS);
#ifdef USE_DEPRECATED_API
  if (dwg_ent_lwpline_get_flag (lwpline, &error) != flag || error)
    fail ("old API dwg_ent_lwpline_get_flag");
#endif
  CHK_ENTITY_3RD (lwpline, LWPOLYLINE, extrusion);
#ifdef USE_DEPRECATED_API
  dwg_ent_lwpline_get_extrusion (lwpline, &pt3d, &error);
  if (error || memcmp (&extrusion, &pt3d, sizeof (extrusion)))
    fail ("old API dwg_ent_lwpline_get_extrusion");
#endif

  CHK_ENTITY_TYPE (lwpline, LWPOLYLINE, const_width, BD);
#ifdef USE_DEPRECATED_API
  if (dwg_ent_lwpline_get_const_width (lwpline, &error) != const_width
      || error)
    fail ("old API dwg_ent_lwpline_get_const_width");
#endif
  CHK_ENTITY_TYPE (lwpline, LWPOLYLINE, thickness, BD);
#ifdef USE_DEPRECATED_API
  if (dwg_ent_lwpline_get_thickness (lwpline, &error) != thickness || error)
    fail ("old API dwg_ent_lwpline_get_thickness");
#endif
  CHK_ENTITY_TYPE (lwpline, LWPOLYLINE, elevation, BD);
#ifdef USE_DEPRECATED_API
  if (dwg_ent_lwpline_get_elevation (lwpline, &error) != elevation || error)
    fail ("old API dwg_ent_lwpline_get_elevation");
#endif
  CHK_ENTITY_TYPE (lwpline, LWPOLYLINE, num_widths, BL);
#ifdef USE_DEPRECATED_API
  if (dwg_ent_lwpline_get_numwidths (lwpline, &error) != num_widths || error)
    fail ("old API dwg_ent_lwpline_get_numwidths");
#endif
  CHK_ENTITY_TYPE (lwpline, LWPOLYLINE, num_bulges, BL);
#ifdef USE_DEPRECATED_API
  if (dwg_ent_lwpline_get_numbulges (lwpline, &error) != num_bulges || error)
    fail ("old API dwg_ent_lwpline_get_numbulges");
#endif
  CHK_ENTITY_TYPE (lwpline, LWPOLYLINE, num_points, BL);
#ifdef USE_DEPRECATED_API
  if (dwg_ent_lwpline_get_numpoints (lwpline, &error) != num_points || error)
    fail ("old API dwg_ent_lwpline_get_numpoints");
#endif

  // CHK_ENTITY_TYPE (lwpline, LWPOLYLINE, bulges, BD*, bulges);
#ifdef USE_DEPRECATED_API
  bulges = dwg_ent_lwpline_get_bulges (lwpline, &error);
#else
  bulges = lwpline->bulges;
  error = 0;
#endif
  if (!error)
    {
      for (i = 0; i < lwpline->num_bulges; i++)
        (void)bulges[i];
      // printf ("bulge[%d] of lwpline : %f\n", (int)i, bulges[i]);
#ifdef USE_DEPRECATED_API
      FREE (bulges);
#endif
    }
  else
    fail ("bulges");

#ifdef USE_DEPRECATED_API
  points = dwg_ent_lwpline_get_points (lwpline, &error);
#else
  points = (dwg_point_2d *)lwpline->points;
#endif
  if (!error)
    {
      for (i = 0; i < lwpline->num_points; i++)
        (void)points[i].x;
      // printf ("point[%d] of lwpline : x = %f\ty = %f\n", (int)i,
      // points[i].x,
      //        points[i].y);
#ifdef USE_DEPRECATED_API
      FREE (points);
#endif
    }
  else
    fail ("points");

#ifdef USE_DEPRECATED_API
  width = dwg_ent_lwpline_get_widths (lwpline, &error);
#else
  width = (dwg_lwpline_widths *)lwpline->widths;
#endif
  if (!error)
    {
      for (i = 0; i < lwpline->num_widths; i++)
        (void)width[i].start;
      // printf ("widths[%d] of lwpline : x = %f\ty = %f\n", (int)i,
      //        width[i].start, width[i].end);
#ifdef USE_DEPRECATED_API
      FREE (width);
#endif
    }
  else
    fail ("widths");

  CHK_ENTITY_TYPE (lwpline, LWPOLYLINE, num_vertexids, BL);
}
