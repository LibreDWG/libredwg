#define DWG_TYPE DWG_TYPE_VERTEX_PFACE
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_RC flag;
  dwg_point_3d point;

  dwg_ent_vertex_pface *vertex_pface = dwg_object_to_VERTEX_PFACE (obj);

  CHK_ENTITY_TYPE (vertex_pface, VERTEX_PFACE, flag, RC);
  CHK_ENTITY_3RD (vertex_pface, VERTEX_PFACE, point);
}
