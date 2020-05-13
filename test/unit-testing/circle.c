#define DWG_TYPE DWG_TYPE_CIRCLE
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  double radius, thickness;
  dwg_point_3d center, extrusion;

  dwg_ent_circle *circle = dwg_object_to_CIRCLE (obj);

  CHK_ENTITY_3RD_W_OLD (circle, CIRCLE, center);
  CHK_ENTITY_TYPE_W_OLD (circle, CIRCLE, radius, BD);
  CHK_ENTITY_TYPE_W_OLD (circle, CIRCLE, thickness, BD);
  CHK_ENTITY_3RD_W_OLD (circle, CIRCLE, extrusion);
}
