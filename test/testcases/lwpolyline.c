#define DWG_TYPE DWG_TYPE_LWPOLYLINE
#include "common.c"
#include <dejagnu.h>

void
low_level_process (dwg_object * obj)
{
  BITCODE_BL i;
  dwg_ent_lwpolyline *lwpolyline = dwg_object_to_LWPOLYLINE (obj);

  printf ("normal of lwpolyline : x = %f, y = %f, z = %f\n",
	  lwpolyline->normal.x, lwpolyline->normal.y, lwpolyline->normal.z);
  printf ("const width of lwpolyline : %f\n", lwpolyline->const_width);
  printf ("elevation of lwpolyline : %f\n", lwpolyline->elevation);
  printf ("thickness of lwpolyline : %f\n", lwpolyline->thickness);
  printf ("num width of lwpolyline : " FORMAT_BL "\n", lwpolyline->num_widths);
  printf ("num bulges of lwpolyline : " FORMAT_BL "\n", lwpolyline->num_bulges);
  printf ("num points of lwpolyline : " FORMAT_BL "\n", lwpolyline->num_points);
  printf ("flag of lwpolyline : " FORMAT_BS "\n", lwpolyline->flag);

  for (i = 0; i < lwpolyline->num_bulges; i++)
    printf ("bulges[%u] of lwpolyline : %f\n", i, lwpolyline->bulges[i]);

  for (i = 0; i < lwpolyline->num_points; i++)
    printf ("points[%u] of lwpolyline : x =%f\ty = %f\n",
            i, lwpolyline->points[i].x, lwpolyline->points[i].y);

  for (i = 0; i < lwpolyline->num_widths; i++)
    printf ("points[%u] of lwpolyline : x =%f\ty = %f\n",
            i, lwpolyline->widths[i].start, lwpolyline->widths[i].end);
}

void
api_process (dwg_object * obj)
{
  int error;
  BITCODE_BL num_points, num_bulges, num_widths;
  dwg_point_3d normal;
  BITCODE_BS flags;
  BITCODE_BD const_width, elevation, thickness;
  BITCODE_BD *bulges;
  dwg_point_2d *points;
  dwg_lwpolyline_widths *width;

  dwg_ent_lwpolyline *lwpolyline = dwg_object_to_LWPOLYLINE (obj);

  dwg_ent_lwpolyline_get_normal (lwpolyline, &normal, &error);
  if (error == 0 && lwpolyline->normal.x == normal.x &&
      lwpolyline->normal.y == normal.y && lwpolyline->normal.z == normal.z)
    {
      pass ("lwpolyline_get_normal");
    }
  else
    {
      fail ("lwpolyline_get_normal");
    }


  const_width = dwg_ent_lwpolyline_get_const_width (lwpolyline, &error);
  if (error == 0 && const_width == lwpolyline->const_width)
    {
      pass ("lwpolyline_get_const_width");
    }
  else
    {
      fail ("lwpolyline_get_const_width");
    }


  elevation = dwg_ent_lwpolyline_get_elevation (lwpolyline, &error);
  if (error == 0 && elevation == lwpolyline->elevation)
    {
      pass ("lwpolyline_get_elevation");
    }
  else
    {
      fail ("lwpolyline_get_elevation");
    }


  thickness = dwg_ent_lwpolyline_get_thickness (lwpolyline, &error);
  if (error == 0 && lwpolyline->thickness == thickness)
    {
      pass ("lwpolyline_get_thickness");
    }
  else
    {
      fail ("lwpolyline_get_thickness");
    }


  num_widths = dwg_ent_lwpolyline_get_numwidths (lwpolyline, &error);
  if (error == 0 && num_widths == lwpolyline->num_widths)
    {
      pass ("lwpolyline_get_num_widths");
    }
  else
    {
      fail ("lwpolyline_get_num_widths");
    }


  num_bulges = dwg_ent_lwpolyline_get_numbulges (lwpolyline, &error);
  if (error == 0 && num_bulges == lwpolyline->num_bulges)
    {
      pass ("lwpolyline_get_num_bulges");
    }
  else
    {
      fail ("lwpolyline_get_num_bulges");
    }


  num_points = dwg_ent_lwpolyline_get_numpoints (lwpolyline, &error);
  if (error == 0 && num_points == lwpolyline->num_points)
    {
      pass ("lwpolyline_get_num_points");
    }
  else
    {
      fail ("lwpolyline_get_num_points");
    }


  flags = dwg_ent_lwpolyline_get_flag (lwpolyline, &error);
  if (error == 0 && flags == lwpolyline->flag)	// error check
    {
      pass ("lwpolyline_get_flags");
    }
  else
    {
      fail ("lwpolyline_get_flags error=%d flags=%x lwpolyline->flags=%x",
            error, flags, lwpolyline->flag);
    }


  bulges = dwg_ent_lwpolyline_get_bulges (lwpolyline, &error);
  if (error == 0)
    {
      unsigned int i, matches = 1;
      for (i = 0; i < lwpolyline->num_bulges; i++)
	{
	  if (lwpolyline->bulges[i] != bulges[i])
	    {
	      matches = 0;
	    }
	}
      if (matches)
	{
	  pass ("lwpolyline_get_bulges");
	}
      else
	{
	  fail ("lwpolyline_get_bulges match");
	}
    }
  else
    {
      fail ("lwpolyline_get_bulges error=1");
    }


  points = dwg_ent_lwpolyline_get_points (lwpolyline, &error);
  if (error == 0)	// error check
    {
      unsigned int i, matches = 1;
      for (i = 0; i < lwpolyline->num_points; i++)
	{
	  if (lwpolyline->points[i].x != points[i].x
	      || lwpolyline->points[i].y != points[i].y)
	    {
	      matches = 0;
	    }
	}
      if (matches)
	{
	  pass ("lwpolyline_get_points");
	}
      else
	{
	  fail ("lwpolyline_get_points match");
	}
    }
  else
    {
      fail ("lwpolyline_get_points error=1");
    }


  width = dwg_ent_lwpolyline_get_widths (lwpolyline, &error);
  if (error == 0)
    {
      unsigned int i, matches = 1;
      for (i = 0; i < lwpolyline->num_widths; i++)
	{
	  if (lwpolyline->widths[i].start != width[i].start
	      && lwpolyline->widths[i].end != width[i].end)
	    {
	      matches = 0;
	    }
	}
      if (matches)
	{
	  pass ("lwpolyline_get_widths");
	}
      else
	{
	  fail ("lwpolyline_get_widths matches");
	}
    }
  else
    {
      fail ("lwpolyline_get_widths error=1");
    }
}
