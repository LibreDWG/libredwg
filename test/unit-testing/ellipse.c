#define DWG_TYPE DWG_TYPE_ELLIPSE
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  double axis_ratio, start_angle, end_angle;
  dwg_point_3d center, sm, ext;

  dwg_ent_ellipse *ellipse = dwg_object_to_ELLIPSE (obj);

  CHK_ENTITY_TYPE_W_OLD (ellipse, ELLIPSE, axis_ratio, RD, axis_ratio);
  CHK_ENTITY_3RD_W_OLD (ellipse, ELLIPSE, sm_axis, sm);
  CHK_ENTITY_3RD_W_OLD (ellipse, ELLIPSE, extrusion, ext);
  CHK_ENTITY_3RD_W_OLD (ellipse, ELLIPSE, center, center);
  CHK_ENTITY_TYPE_W_OLD (ellipse, ELLIPSE, start_angle, BD, start_angle);
  CHK_ENTITY_TYPE_W_OLD (ellipse, ELLIPSE, end_angle, BD, end_angle);
}
