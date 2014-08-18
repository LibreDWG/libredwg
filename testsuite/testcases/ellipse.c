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

  if (dwg_get_type (obj) == DWG_TYPE_ELLIPSE)
    {
      output_process (obj);
    }
}

void
low_level_process (dwg_object * obj)
{
  // casts dwg object to ellipse entity
  dwg_ent_ellipse *ellipse = dwg_object_to_ELLIPSE (obj);

  // prints ratio of ellipse
  printf ("Ratio of ellipse : %f\t\n", ellipse->axis_ratio);

  // prints sm axis of ellipse
  printf ("sm axis of ellipse : x = %f, y = %f, z = %f\t\n",
	  ellipse->sm_axis.x, ellipse->sm_axis.y, ellipse->sm_axis.z);

  // prints extrusion points
  printf ("extrusion of ellipse : x = %f, y = %f, z = %f\t\n",
	  ellipse->extrusion.x, ellipse->extrusion.y, ellipse->extrusion.z);

  // prints center points
  printf ("center of ellipse : x = %f,y = %f,z = %f\t\n",
	  ellipse->center.x, ellipse->center.y, ellipse->center.z);

  // prints start angle
  printf ("start angle of ellipse : %f\t\n", ellipse->start_angle);

  // prints end angle
  printf ("end angle of ellipse : %f\t\n", ellipse->end_angle);
}

void
api_process (dwg_object * obj)
{
  int ratio_error, sm_error, ext_error, center_error, start_error, end_error;
  float axis_ratio, start_angle, end_angle;
  dwg_point_3d center, sm, ext;
  dwg_ent_ellipse *ellipse = dwg_object_to_ELLIPSE (obj);

  // returns axis ratio
  axis_ratio = dwg_ent_ellipse_get_axis_ratio (ellipse, &ratio_error);
  if (ratio_error == 0 && ellipse->axis_ratio)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading axis_ratio");
    }

  // returns sm axis  
  dwg_ent_ellipse_get_sm_axis (ellipse, &sm, &sm_error);
  if (sm_error == 0 && sm.x == ellipse->sm_axis.x && sm.y == ellipse->sm_axis.y && sm.z == ellipse->sm_axis.z)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading sm axis");
    }

  // returns extrusion points
  dwg_ent_ellipse_get_extrusion (ellipse, &ext, &ext_error);
  if (ext_error == 0 && ext.x == ellipse->extrusion.x && ext.y == ellipse->extrusion.y && ext.z == ellipse->extrusion.z)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading extrusion");
    }

  // returns center
  dwg_ent_ellipse_get_center (ellipse, &center, &center_error);
  if (center_error == 0 && ellipse->center.x == center.x && ellipse->center.y == center.y && ellipse->center.z == center.z)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading center");
    }

  // returns start angle  
  start_angle = dwg_ent_ellipse_get_start_angle (ellipse, &start_error);
  if (start_error == 0 && ellipse->start_angle == start_angle)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading start angle");
    }

  // returns end angle
  end_angle = dwg_ent_ellipse_get_end_angle (ellipse, &end_error);
  if (end_error == 0 && ellipse->end_angle == end_angle)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading end angle");
    }

}
