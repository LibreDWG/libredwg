#define DWG_TYPE DWG_TYPE_SOLID
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  double thickness, elevation;
  dwg_point_3d extrusion;
  dwg_point_2d corner1, corner2, corner3, corner4;

  dwg_ent_solid *solid = dwg_object_to_SOLID (obj);

  CHK_ENTITY_TYPE_W_OLD (solid, SOLID, thickness, BD);
  CHK_ENTITY_TYPE_W_OLD (solid, SOLID, elevation, BD);
  CHK_ENTITY_2RD_W_OLD (solid, SOLID, corner1);
  CHK_ENTITY_2RD_W_OLD (solid, SOLID, corner2);
  CHK_ENTITY_2RD_W_OLD (solid, SOLID, corner3);
  CHK_ENTITY_2RD_W_OLD (solid, SOLID, corner4);
  CHK_ENTITY_3RD_W_OLD (solid, SOLID, extrusion);
}
