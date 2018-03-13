#include "common.c"
#include <dejagnu.h>
void output_process (dwg_object * obj);

void
output_object (dwg_object * obj)
{
  if (!obj)
    {
      printf ("object is NULL\n");
      return;
    }

  if (dwg_get_type (obj) == DWG_TYPE_LWPLINE)
    {
      output_process (obj);
    }
}

void
low_level_process (dwg_object * obj)
{
  // casts dwg object to lwpline entity
  dwg_ent_lwpline *lwpline = dwg_object_to_LWPLINE (obj);

  // prints normal points
  printf ("normal of lwpline : x = %f, y = %f, z = %f\t\n",
	  lwpline->normal.x, lwpline->normal.y, lwpline->normal.z);

  // prints constant width
  printf ("const width of lwpline : %f\t\n", lwpline->const_width);

  // prints elevation
  printf ("elevation of lwpline : %f\t\n", lwpline->elevation);

  // prints thickness
  printf ("thickness of lwpline : %f\t\n", lwpline->thickness);

  // prints width number
  printf ("num width of lwpline : %ld\t\n", lwpline->num_widths);

  // prints number of bulges
  printf ("num bulges of lwpline : %ld\t\n", lwpline->num_bulges);

  // prints number of points
  printf ("num points of lwpline : %ld\t\n", lwpline->num_points);

  // prints flag
  printf ("flag of lwpline : %d\t\n", lwpline->flags);

  // prints bulges
  int i;
  for (i = 0; i < lwpline->num_bulges; i++)
    {
      printf ("bulges of lwpline : %f\t\n", lwpline->bulges[i]);
    }

  // prints points
  for (i = 0; i < lwpline->num_points; i++)
    {
      printf ("points of lwpline : x =%f\ty = %f\t\n",
	      lwpline->points[i].x, lwpline->points[i].y);
    }

  // prints widths
  for (i = 0; i < lwpline->num_widths; i++)
    {
      printf ("points of lwpline : x =%f\ty = %f\t\n",
	      lwpline->widths[i].start, lwpline->widths[i].end);
    }
}

void
api_process (dwg_object * obj)
{
  dwg_ent_lwpline *lwpline = dwg_object_to_LWPLINE (obj);
  int error;
  BITCODE_BL num_points, num_bulges, num_widths;
  dwg_point_3d normal;
  BITCODE_BS flags;
  BITCODE_BD const_width, elevation, thickness;
  BITCODE_BD *bulges;
  dwg_point_2d *points;
  dwg_lwpline_widths *width;

  // returns normal points
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

  // returns constant width
  const_width = dwg_ent_lwpline_get_const_width (lwpline, &error);
  if (error == 0 && const_width == lwpline->const_width)
    {
      pass ("lwpline_get_const_width");
    }
  else
    {
      fail ("lwpline_get_const_width");
    }

  // returns elevation
  elevation = dwg_ent_lwpline_get_elevation (lwpline, &error);
  if (error == 0 && elevation == lwpline->elevation)
    {
      pass ("lwpline_get_elevation");
    }
  else
    {
      fail ("lwpline_get_elevation");
    }

  // returns thickness
  thickness = dwg_ent_lwpline_get_thickness (lwpline, &error);
  if (error == 0 && lwpline->thickness == thickness)
    {
      pass ("lwpline_get_thickness");
    }
  else
    {
      fail ("lwpline_get_thickness");
    }

  // returns number of widths
  num_widths = dwg_ent_lwpline_get_num_widths (lwpline, &error);
  if (error == 0 && num_widths == lwpline->num_widths)
    {
      pass ("lwpline_get_num_widths");
    }
  else
    {
      fail ("lwpline_get_num_widths");
    }

  // returns number of bulges
  num_bulges = dwg_ent_lwpline_get_num_bulges (lwpline, &error);
  if (error == 0 && num_bulges == lwpline->num_bulges)
    {
      pass ("lwpline_get_num_bulges");
    }
  else
    {
      fail ("lwpline_get_num_bulges");
    }

  // returns number of points
  num_points = dwg_ent_lwpline_get_num_points (lwpline, &error);
  if (error == 0 && num_points == lwpline->num_points)
    {
      pass ("lwpline_get_num_points");
    }
  else
    {
      fail ("lwpline_get_num_points");
    }

  // returns flags
  flags = dwg_ent_lwpline_get_flags (lwpline, &error);
  if (error == 0 && flags == lwpline->flags)	// error check
    {
      pass ("lwpline_get_flags");
    }
  else
    {
      fail ("lwpline_get_flags error=%d flags=%x lwpline->flags=%x",
            error, flags, lwpline->flags);
    }

  // returns bulges
  bulges = dwg_ent_lwpline_get_bulges (lwpline, &error);
  if (error == 0)
    {
      int i, matches = 1;
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

  // returns points
  points = dwg_ent_lwpline_get_points (lwpline, &error);
  if (error == 0)	// error check
    {
      int i, matches = 1;
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

  // returns width
  width = dwg_ent_lwpline_get_widths (lwpline, &error);
  if (error == 0)
    {
      int i, matches = 1;
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
