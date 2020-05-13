#define DWG_TYPE DWG_TYPE_TRACE
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  double thickness, elevation;
  dwg_point_3d extrusion;
  dwg_point_2d corner1, corner2, corner3, corner4;

  dwg_ent_trace *trace = dwg_object_to_TRACE (obj);

  CHK_ENTITY_TYPE_W_OLD (trace, TRACE, thickness, BD);
  CHK_ENTITY_TYPE_W_OLD (trace, TRACE, elevation, BD);
  CHK_ENTITY_2RD_W_OLD (trace, TRACE, corner1);
  CHK_ENTITY_2RD_W_OLD (trace, TRACE, corner2);
  CHK_ENTITY_2RD_W_OLD (trace, TRACE, corner3);
  CHK_ENTITY_2RD_W_OLD (trace, TRACE, corner4);
  CHK_ENTITY_3RD_W_OLD (trace, TRACE, extrusion);
}
