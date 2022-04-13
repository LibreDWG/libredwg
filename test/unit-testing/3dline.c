// preR13 only
#define DWG_TYPE DWG_TYPE__3DLINE
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_RD thickness;
  dwg_point_3d start, end, extrusion;

  dwg_ent__3dline *_3dline = dwg_object_to__3DLINE (obj);

  CHK_ENTITY_3RD (_3dline, 3DLINE, start);
  CHK_ENTITY_3RD (_3dline, 3DLINE, end);
  CHK_ENTITY_3RD (_3dline, 3DLINE, extrusion);
  CHK_ENTITY_TYPE (_3dline, 3DLINE, thickness, RD);
}
