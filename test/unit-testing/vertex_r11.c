#define DWG_TYPE DWG_TYPE_VERTEX_R11
#include "common.c"

void
api_process (dwg_object *obj)
{
  double start_width, end_width, bulge, tangent_dir;
  BITCODE_RC flag;
  BITCODE_RS vertind[4];
  dwg_point_2d point;

  dwg_ent_vertex_r11 *vertex_r11 = dwg_object_to_VERTEX_R11 (obj);

  CHK_ENTITY_2RD_W_OLD (vertex_r11, VERTEX_R11, point);
  CHK_ENTITY_TYPE_W_OLD (vertex_r11, VERTEX_R11, start_width, RD);
  CHK_ENTITY_TYPE_W_OLD (vertex_r11, VERTEX_R11, end_width, RD);
  CHK_ENTITY_TYPE_W_OLD (vertex_r11, VERTEX_R11, bulge, RD);
  CHK_ENTITY_TYPE_W_OLD (vertex_r11, VERTEX_R11, flag, RC);
  CHK_ENTITY_TYPE_W_OLD (vertex_r11, VERTEX_R11, tangent_dir, RD);
  CHK_ENTITY_TYPE_W_OLD (vertex_r11, VERTEX_R11, vertind[0], RC);
  CHK_ENTITY_TYPE_W_OLD (vertex_r11, VERTEX_R11, vertind[1], RC);
  CHK_ENTITY_TYPE_W_OLD (vertex_r11, VERTEX_R11, vertind[2], RC);
  CHK_ENTITY_TYPE_W_OLD (vertex_r11, VERTEX_R11, vertind[3], RC);
}
