#define DWG_TYPE DWG_TYPE_ELLIPSE
#include "common.c"
#include <dejagnu.h>

void
low_level_process (dwg_object * obj)
{
  // casts dwg object to ellipse entity
  dwg_ent_ellipse *ellipse = dwg_object_to_ELLIPSE (obj);


  printf ("Ratio of ellipse : %f\n", ellipse->axis_ratio);
  printf ("sm axis of ellipse : x = %f, y = %f, z = %f\n",
          ellipse->sm_axis.x, ellipse->sm_axis.y, ellipse->sm_axis.z);
  printf ("extrusion of ellipse : x = %f, y = %f, z = %f\n",
          ellipse->extrusion.x, ellipse->extrusion.y, ellipse->extrusion.z);
  printf ("center of ellipse : x = %f, y = %f, z = %f\n",
          ellipse->center.x, ellipse->center.y, ellipse->center.z);
  printf ("start angle of ellipse : %f\n", ellipse->start_angle);
  printf ("end angle of ellipse : %f\n", ellipse->end_angle);
}

void
api_process (dwg_object * obj)
{
  int error;
  BITCODE_BD axis_ratio, start_angle, end_angle;
  dwg_point_3d center, sm, ext;
  dwg_ent_ellipse *ellipse = dwg_object_to_ELLIPSE (obj);


  axis_ratio = dwg_ent_ellipse_get_axis_ratio (ellipse, &error);
  if (!error  && ellipse->axis_ratio)   // error check
    pass ("Working Properly");
  else
    fail ("error in reading axis_ratio");


  dwg_ent_ellipse_get_sm_axis (ellipse, &sm, &error);
  if (!error  && sm.x == ellipse->sm_axis.x && sm.y == ellipse->sm_axis.y && sm.z == ellipse->sm_axis.z)        // error check
    pass ("Working Properly");
  else
    fail ("error in reading sm axis");


  dwg_ent_ellipse_get_extrusion (ellipse, &ext, &error);
  if (!error  && ext.x == ellipse->extrusion.x && ext.y == ellipse->extrusion.y && ext.z == ellipse->extrusion.z)       // error check
    pass ("Working Properly");
  else
    fail ("error in reading extrusion");


  dwg_ent_ellipse_get_center (ellipse, &center, &error);
  if (!error  && ellipse->center.x == center.x && ellipse->center.y == center.y && ellipse->center.z == center.z)       // error check
    pass ("Working Properly");
  else
    fail ("error in reading center");


  start_angle = dwg_ent_ellipse_get_start_angle (ellipse, &error);
  if (!error  && ellipse->start_angle == start_angle)   // error check
    pass ("Working Properly");
  else
    fail ("error in reading start angle");


  end_angle = dwg_ent_ellipse_get_end_angle (ellipse, &error);
  if (!error  && ellipse->end_angle == end_angle)       // error check
    pass ("Working Properly");
  else
    fail ("error in reading end angle");

}
