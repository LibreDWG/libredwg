#define DWG_TYPE DWG_TYPE_VERTEX_2D
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  double start_width, end_width, bulge, tangent_dir;
  BITCODE_RC flag;
  BITCODE_BL id;
  dwg_point_3d point;

  dwg_ent_vertex_2d *vertex_2d = dwg_object_to_VERTEX_2D (obj);

  CHK_ENTITY_TYPE_W_OLD (vertex_2d, VERTEX_2D, flag, RC);
  CHK_ENTITY_3RD_W_OLD (vertex_2d, VERTEX_2D, point);
  CHK_ENTITY_TYPE_W_OLD (vertex_2d, VERTEX_2D, start_width, BD);
  CHK_ENTITY_TYPE_W_OLD (vertex_2d, VERTEX_2D, end_width, BD);
  CHK_ENTITY_TYPE_W_OLD (vertex_2d, VERTEX_2D, bulge, BD);
  CHK_ENTITY_TYPE_W_OLD (vertex_2d, VERTEX_2D, tangent_dir, BD);
  CHK_ENTITY_TYPE (vertex_2d, VERTEX_2D, id, BL);
}
