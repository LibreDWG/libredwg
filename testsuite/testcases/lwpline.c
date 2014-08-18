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
  int flag_error, const_width_error, elevation_error, thickness_error,
    num_points_error, num_bulges_error, num_widths_error, normal_error,
    points_error, bulges_error, widths_error;
  long num_points, num_bulges, num_widths;
  dwg_point_3d normal;
  char flags;
  double const_width, elevation, thickness;
  double *bulges;
  dwg_point_2d *points;
  dwg_lwpline_widths *width;

  // returns normal points
  dwg_ent_lwpline_get_normal (lwpline, &normal, &normal_error);
  if (normal_error == 0 && lwpline->normal.x == normal.x && lwpline->normal.y == normal.y && lwpline->normal.z == normal.z)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading normal");
    }

  // returns constant width
  const_width = dwg_ent_lwpline_get_const_width (lwpline, &const_width_error);
  if (const_width_error == 0 && const_width == lwpline->const_width)	// error checking
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading const width");
    }

  // returns elevation
  elevation = dwg_ent_lwpline_get_elevation (lwpline, &elevation_error);
  if (elevation_error == 0 && elevation == lwpline->elevation)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading elevation");
    }

  // returns thickness
  thickness = dwg_ent_lwpline_get_thickness (lwpline, &thickness_error);
  if (thickness_error == 0 && lwpline->thickness == thickness)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading thickness");
    }

  // returns number of widths
  num_widths = dwg_ent_lwpline_get_num_widths (lwpline, &num_widths_error);
  if (num_widths_error == 0 && num_widths == lwpline->num_widths)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading num width");
    }

  // returns number of bulges
  num_bulges = dwg_ent_lwpline_get_num_bulges (lwpline, &num_bulges_error);
  if (num_bulges_error == 0 && num_bulges == lwpline->num_bulges)	// error checking
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading num bulge");
    }

  // returns number of points
  num_points = dwg_ent_lwpline_get_num_points (lwpline, &num_points_error);
  if (num_points_error == 0 && num_points == lwpline->num_points)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading num points");
    }

  // returns flags
  flags = dwg_ent_lwpline_get_flags (lwpline, &flag_error);
  if (flag_error == 0 && flags == lwpline->flags)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading flag");
    }

  // returns bulges
  bulges = dwg_ent_lwpline_get_bulges (lwpline, &bulges_error);
  if (bulges_error == 0)	// error check
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
	  pass ("Working Properly");
	}
      else
	{
	  fail ("error in reading bulges");
	}
    }
  else
    {
      fail ("error in reading bulges");
    }

  // returns points
  points = dwg_ent_lwpline_get_points (lwpline, &points_error);
  if (points_error == 0)	// error check
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
	  pass ("Working Properly");
	}
      else
	{
	  fail ("error in reading points");
	}
    }
  else
    {
      fail ("error in reading points");
    }

  // returns width
  width = dwg_ent_lwpline_get_widths (lwpline, &widths_error);
  if (widths_error == 0)	// error check
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
	  pass ("Working Properly");
	}
      else
	{
	  fail ("error in reading widths");
	}
    }
  else
    {
      fail ("error in reading widths");
    }
}
