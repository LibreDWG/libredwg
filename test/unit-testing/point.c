#define DWG_TYPE DWG_TYPE_POINT
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  double thickness, x, y, z, x_ang;
  dwg_point_3d extrusion;

  dwg_ent_point *point = dwg_object_to_POINT (obj);

  CHK_ENTITY_TYPE (point, POINT, x, BD);
  CHK_ENTITY_TYPE (point, POINT, y, BD);
  CHK_ENTITY_TYPE (point, POINT, z, BD);
  CHK_ENTITY_TYPE_W_OLD (point, POINT, thickness, BD);
  CHK_ENTITY_3RD_W_OLD (point, POINT, extrusion);
  CHK_ENTITY_TYPE (point, POINT, x_ang, BD);
}
