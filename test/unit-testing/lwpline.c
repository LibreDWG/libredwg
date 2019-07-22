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
    {
      printf ("Error with old API dwg_ent_lwpline_get_flag\n");
      exit (1);
    }
  CHK_ENTITY_3RD (lwpline, LWPOLYLINE, extrusion, extrusion);
  dwg_ent_lwpline_get_extrusion (lwpline, &pt3d, &error);
  if (error || memcmp (&extrusion, &pt3d, sizeof (extrusion)))
    {
      printf ("Error with old API dwg_ent_lwpline_get_extrusion\n");
      exit (1);
    }

  CHK_ENTITY_TYPE (lwpline, LWPOLYLINE, const_width, BD, const_width);
  if (dwg_ent_lwpline_get_const_width (lwpline, &error) != const_width || error)
    {
      printf ("Error with old API dwg_ent_lwpline_get_const_width\n");
      exit (1);
    }
  CHK_ENTITY_TYPE (lwpline, LWPOLYLINE, thickness, BD, thickness);
  if (dwg_ent_lwpline_get_thickness (lwpline, &error) != thickness || error)
    {
      printf ("Error with old API dwg_ent_lwpline_get_thickness\n");
      exit (1);
    }

  CHK_ENTITY_TYPE (lwpline, LWPOLYLINE, elevation, BD, elevation);
  if (dwg_ent_lwpline_get_elevation (lwpline, &error) != elevation || error)
    {
      printf ("Error with old API dwg_ent_lwpline_get_elevation\n");
      exit (1);
    }

  CHK_ENTITY_TYPE (lwpline, LWPOLYLINE, num_widths, BL, num_widths);
  if (dwg_ent_lwpline_get_numwidths (lwpline, &error) != num_widths || error)
    {
      printf ("Error with old API dwg_ent_lwpline_get_numwidths\n");
      exit (1);
    }
  CHK_ENTITY_TYPE (lwpline, LWPOLYLINE, num_bulges, BL, num_bulges);
  if (dwg_ent_lwpline_get_numbulges (lwpline, &error) != num_bulges || error)
    {
      printf ("Error with old API dwg_ent_lwpline_get_numbulges\n");
      exit (1);
    }
  CHK_ENTITY_TYPE (lwpline, LWPOLYLINE, num_points, BL, num_points);
  if (dwg_ent_lwpline_get_numpoints (lwpline, &error) != num_points || error)
    {
      printf ("Error with old API dwg_ent_lwpline_get_numpoints\n");
      exit (1);
    }

  //CHK_ENTITY_TYPE (lwpline, LWPOLYLINE, bulges, BD*, bulges);
  bulges = dwg_ent_lwpline_get_bulges (lwpline, &error);
  if (!error)
    {
      for (i = 0; i < lwpline->num_bulges; i++)
        (void)bulges[i];
      //printf ("bulge[%d] of lwpline : %f\n", (int)i, bulges[i]);
      free (bulges);
    }
  else
    {
      printf ("Error in reading bulges \n");
      exit (1);
    }

  points = dwg_ent_lwpline_get_points (lwpline, &error);
  if (!error)
    {
      for (i = 0; i < lwpline->num_points; i++)
        (void)points[i].x;
        //printf ("point[%d] of lwpline : x = %f\ty = %f\n", (int)i, points[i].x,
        //        points[i].y);
      free (points);
    }
  else
    {
      printf ("Error in reading points \n");
      exit (1);
    }

  width = dwg_ent_lwpline_get_widths (lwpline, &error);
  if (!error)
    {
      for (i = 0; i < lwpline->num_widths; i++)
        (void)width[i].start;
        //printf ("widths[%d] of lwpline : x = %f\ty = %f\n", (int)i,
        //        width[i].start, width[i].end);
      free (width);
    }
  else
    {
      printf ("Error in reading widths \n");
      exit (1);
    }

  CHK_ENTITY_TYPE (lwpline, LWPOLYLINE, num_vertexids, BL, num_vertexids);
}
