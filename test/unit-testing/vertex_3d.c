#define DWG_TYPE DWG_TYPE_VERTEX_3D
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_RC flag;
  dwg_point_3d point;

  dwg_ent_vertex_3d *vertex_3d = dwg_object_to_VERTEX_3D (obj);

  CHK_ENTITY_TYPE (vertex_3d, VERTEX_3D, flag, RC);
  CHK_ENTITY_3RD (vertex_3d, VERTEX_3D, point);
}
