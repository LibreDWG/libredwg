#define DWG_TYPE DWG_TYPE_POLYLINE_R11
#include "common.c"

void
api_process (dwg_object *obj)
{
  double start_width, end_width;
  BITCODE_RS flag, num_m_verts, num_n_verts, m_density, n_density, curve_type;
  dwg_point_3d extrusion;

  dwg_ent_polyline_r11 *polyline_r11 = dwg_object_to_POLYLINE_R11 (obj);

  CHK_ENTITY_TYPE_W_OLD (polyline_r11, POLYLINE_R11, flag, RS);
  CHK_ENTITY_TYPE_W_OLD (polyline_r11, POLYLINE_R11, start_width, RD);
  CHK_ENTITY_TYPE_W_OLD (polyline_r11, POLYLINE_R11, end_width, RD);
  CHK_ENTITY_3RD_W_OLD (polyline_r11, POLYLINE_R11, extrusion);
  CHK_ENTITY_TYPE_W_OLD (polyline_r11, POLYLINE_R11, num_m_verts, RS);
  CHK_ENTITY_TYPE_W_OLD (polyline_r11, POLYLINE_R11, num_n_verts, RS);
  CHK_ENTITY_TYPE_W_OLD (polyline_r11, POLYLINE_R11, m_density, RS);
  CHK_ENTITY_TYPE_W_OLD (polyline_r11, POLYLINE_R11, n_density, RS);
  CHK_ENTITY_TYPE_W_OLD (polyline_r11, POLYLINE_R11, curve_type, RS);
}