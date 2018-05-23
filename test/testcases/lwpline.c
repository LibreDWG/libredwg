#define DWG_TYPE DWG_TYPE_LWPLINE
#include "common.c"
#include <dejagnu.h>

void
low_level_process (dwg_object * obj)
{
  BITCODE_BL i;
  dwg_ent_lwpline *lwpline = dwg_object_to_LWPLINE (obj);

  printf ("normal of lwpline : x = %f, y = %f, z = %f\n",
	  lwpline->normal.x, lwpline->normal.y, lwpline->normal.z);
  printf ("const width of lwpline : %f\n", lwpline->const_width);
  printf ("elevation of lwpline : %f\n", lwpline->elevation);
  printf ("thickness of lwpline : %f\n", lwpline->thickness);
  printf ("num width of lwpline : " FORMAT_BL "\n", lwpline->num_widths);
  printf ("num bulges of lwpline : " FORMAT_BL "\n", lwpline->num_bulges);
  printf ("num points of lwpline : " FORMAT_BL "\n", lwpline->num_points);
  printf ("flag of lwpline : " FORMAT_BS "\n", lwpline->flag);

  for (i = 0; i < lwpline->num_bulges; i++)
    printf ("bulges[%u] of lwpline : %f\n", i, lwpline->bulges[i]);

  for (i = 0; i < lwpline->num_points; i++)
    printf ("points[%u] of lwpline : x =%f\ty = %f\n",
            i, lwpline->points[i].x, lwpline->points[i].y);

  for (i = 0; i < lwpline->num_widths; i++)
    printf ("points[%u] of lwpline : x =%f\ty = %f\n",
            i, lwpline->widths[i].start, lwpline->widths[i].end);
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
  dwg_lwpline_widths *width;

  dwg_ent_lwpline *lwpline = dwg_object_to_LWPLINE (obj);

  dwg_ent_lwpline_get_normal (lwpline, &normal, &error);
  if (error == 0 && lwpline->normal.x == normal.x &&
      lwpline->normal.y == normal.y && lwpline->normal.z == normal.z)
    {
      pass ("lwpline_get_normal");
    }
  else
    {
      fail ("lwpline_get_normal");
    }


  const_width = dwg_ent_lwpline_get_const_width (lwpline, &error);
  if (error == 0 && const_width == lwpline->const_width)
    {
      pass ("lwpline_get_const_width");
    }
  else
    {
      fail ("lwpline_get_const_width");
    }


  elevation = dwg_ent_lwpline_get_elevation (lwpline, &error);
  if (error == 0 && elevation == lwpline->elevation)
    {
      pass ("lwpline_get_elevation");
    }
  else
    {
      fail ("lwpline_get_elevation");
    }


  thickness = dwg_ent_lwpline_get_thickness (lwpline, &error);
  if (error == 0 && lwpline->thickness == thickness)
    {
      pass ("lwpline_get_thickness");
    }
  else
    {
      fail ("lwpline_get_thickness");
    }


  num_widths = dwg_ent_lwpline_get_numwidths (lwpline, &error);
  if (error == 0 && num_widths == lwpline->num_widths)
    {
      pass ("lwpline_get_num_widths");
    }
  else
    {
      fail ("lwpline_get_num_widths");
    }


  num_bulges = dwg_ent_lwpline_get_numbulges (lwpline, &error);
  if (error == 0 && num_bulges == lwpline->num_bulges)
    {
      pass ("lwpline_get_num_bulges");
    }
  else
    {
      fail ("lwpline_get_num_bulges");
    }


  num_points = dwg_ent_lwpline_get_numpoints (lwpline, &error);
  if (error == 0 && num_points == lwpline->num_points)
    {
      pass ("lwpline_get_num_points");
    }
  else
    {
      fail ("lwpline_get_num_points");
    }


  flags = dwg_ent_lwpline_get_flag (lwpline, &error);
  if (error == 0 && flags == lwpline->flag)	// error check
    {
      pass ("lwpline_get_flags");
    }
  else
    {
      fail ("lwpline_get_flags error=%d flags=%x lwpline->flags=%x",
            error, flags, lwpline->flag);
    }


  bulges = dwg_ent_lwpline_get_bulges (lwpline, &error);
  if (error == 0)
    {
      unsigned int i, matches = 1;
      for (i = 0; i < lwpline->num_bulges; i++)
	{
	  if (lwpline->bulges[i] != bulges[i])
	    {
	      matches = 0;
	    }
	}
      if (matches)
	{
	  pass ("lwpline_get_bulges");
	}
      else
	{
	  fail ("lwpline_get_bulges match");
	}
    }
  else
    {
      fail ("lwpline_get_bulges error=1");
    }


  points = dwg_ent_lwpline_get_points (lwpline, &error);
  if (error == 0)	// error check
    {
      unsigned int i, matches = 1;
      for (i = 0; i < lwpline->num_points; i++)
	{
	  if (lwpline->points[i].x != points[i].x
	      || lwpline->points[i].y != points[i].y)
	    {
	      matches = 0;
	    }
	}
      if (matches)
	{
	  pass ("lwpline_get_points");
	}
      else
	{
	  fail ("lwpline_get_points match");
	}
    }
  else
    {
      fail ("lwpline_get_points error=1");
    }


  width = dwg_ent_lwpline_get_widths (lwpline, &error);
  if (error == 0)
    {
      unsigned int i, matches = 1;
      for (i = 0; i < lwpline->num_widths; i++)
	{
	  if (lwpline->widths[i].start != width[i].start
	      && lwpline->widths[i].end != width[i].end)
	    {
	      matches = 0;
	    }
	}
      if (matches)
	{
	  pass ("lwpline_get_widths");
	}
      else
	{
	  fail ("lwpline_get_widths matches");
	}
    }
  else
    {
      fail ("lwpline_get_widths error=1");
    }
}
