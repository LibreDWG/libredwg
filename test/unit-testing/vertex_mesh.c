#define DWG_TYPE DWG_TYPE_VERTEX_MESH
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_RC flag;
  dwg_point_3d point;

  dwg_ent_vertex_mesh *vertex_mesh = dwg_object_to_VERTEX_MESH (obj);

  CHK_ENTITY_TYPE (vertex_mesh, VERTEX_MESH, flag, RC);
  CHK_ENTITY_3RD (vertex_mesh, VERTEX_MESH, point);
}
