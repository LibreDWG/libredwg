#define DWG_TYPE DWG_TYPE_ELLIPSE
#include "common.c"

void
low_level_process (dwg_object *obj)
{
  dwg_ent_ellipse *ellipse = dwg_object_to_ELLIPSE (obj);

  printf ("Ratio of ellipse : %f\n", ellipse->axis_ratio);
  printf ("sm axis of ellipse : x = %f, y = %f, z = %f\n", ellipse->sm_axis.x,
          ellipse->sm_axis.y, ellipse->sm_axis.z);
  printf ("extrusion of ellipse : x = %f, y = %f, z = %f\n",
          ellipse->extrusion.x, ellipse->extrusion.y, ellipse->extrusion.z);
  printf ("center of ellipse : x = %f,y = %f,z = %f\n", ellipse->center.x,
          ellipse->center.y, ellipse->center.z);
  printf ("start angle of ellipse : %f\n", ellipse->start_angle);
  printf ("end angle of ellipse : %f\n", ellipse->end_angle);
}

void
api_process (dwg_object *obj)
{
  int error;
  double axis_ratio, start_angle, end_angle;
  dwg_point_3d center, sm, ext;
  dwg_ent_ellipse *ellipse = dwg_object_to_ELLIPSE (obj);

  axis_ratio = dwg_ent_ellipse_get_axis_ratio (ellipse, &error);
  if (!error)
    printf ("axis ratio of ellipse : %f\n", axis_ratio);
  else
    printf ("in reading axis_ratio \n");

  dwg_ent_ellipse_get_sm_axis (ellipse, &sm, &error);
  if (!error)
    {
      printf ("sm axis of ellipse : x = %f, y = %f, z = %f\n", sm.x, sm.y,
              sm.z);
    }
  else
    {
      printf ("error in reading sm axis \n");
    }

  dwg_ent_ellipse_get_extrusion (ellipse, &ext, &error);
  if (!error)
    {
      printf ("extrusion of ellipse : x = %f, y = %f, z = %f\n", ext.x, ext.y,
              ext.z);
    }
  else
    {
      printf ("error in reading extrusion \n");
    }

  dwg_ent_ellipse_get_center (ellipse, &center, &error);
  if (!error)
    {
      printf ("center of ellipse : x = %f, y = %f, z = %f\n", center.x,
              center.y, center.z);
    }
  else
    {
      printf ("error in reading center \n");
    }

  start_angle = dwg_ent_ellipse_get_start_angle (ellipse, &error);
  if (!error)
    printf ("start angle of ellipse : %f\n", start_angle);
  else
    printf ("in reading start angle\n");

  end_angle = dwg_ent_ellipse_get_end_angle (ellipse, &error);
  if (!error)
    printf ("End angle of ellipse : %f\n", end_angle);
  else
    printf ("in reading end angle \n");
}
