#define DWG_TYPE DWG_TYPE_ARC
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  double radius, thickness, start_angle, end_angle;
  dwg_point_3d center, ext;

  dwg_ent_arc *arc = dwg_object_to_ARC (obj);

  CHK_ENTITY_TYPE_W_OLD (arc, ARC, radius, BD, radius);
  CHK_ENTITY_TYPE_W_OLD (arc, ARC, thickness, BD, thickness);
  CHK_ENTITY_3RD_W_OLD (arc, ARC, extrusion, ext);
  CHK_ENTITY_3RD_W_OLD (arc, ARC, center, center);
  CHK_ENTITY_TYPE_W_OLD (arc, ARC, start_angle, BD, start_angle);
  CHK_ENTITY_TYPE_W_OLD (arc, ARC, end_angle, BD, end_angle);
}
