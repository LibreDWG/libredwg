#define DWG_TYPE DWG_TYPE_TRACE
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  double thickness, elevation;
  dwg_point_3d ext;
  dwg_point_2d c1, c2, c3, c4;

  dwg_ent_trace *trace = dwg_object_to_TRACE (obj);

  CHK_ENTITY_TYPE_W_OLD (trace, TRACE, thickness, BD, thickness);
  CHK_ENTITY_TYPE_W_OLD (trace, TRACE, elevation, BD, elevation);
  CHK_ENTITY_2RD_W_OLD (trace, TRACE, corner1, c1);
  CHK_ENTITY_2RD_W_OLD (trace, TRACE, corner2, c2);
  CHK_ENTITY_2RD_W_OLD (trace, TRACE, corner3, c3);
  CHK_ENTITY_2RD_W_OLD (trace, TRACE, corner4, c4);
  CHK_ENTITY_3RD_W_OLD (trace, TRACE, extrusion, ext);
}
