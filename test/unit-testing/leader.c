#define DWG_TYPE DWG_TYPE_LEADER
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  dwg_point_3d origin, extrusion, x_direction, inspt_offset, endptproj;
  dwg_point_3d *points;
  BITCODE_B unknown_bit_1;
  BITCODE_BS path_type;
  BITCODE_BS annot_type;
  BITCODE_BL i, num_points;
  BITCODE_BD dimgap;
  BITCODE_BD box_height;
  BITCODE_BD box_width;
  BITCODE_B hookline_dir;
  BITCODE_B hookline_on;
  BITCODE_B arrowhead_on;
  BITCODE_BS arrowhead_type;
  BITCODE_BD dimasz;
  BITCODE_B unknown_bit_2;
  BITCODE_B unknown_bit_3;
  BITCODE_BS unknown_short_1;
  BITCODE_BS byblock_color;
  BITCODE_B unknown_bit_4;
  BITCODE_B unknown_bit_5;
  BITCODE_H associated_annotation;
  BITCODE_H dimstyle;

  dwg_ent_leader *leader = dwg_object_to_LEADER (obj);

  CHK_ENTITY_3RD_W_OLD (leader, LEADER, origin);
  CHK_ENTITY_3RD_W_OLD (leader, LEADER, extrusion);
  CHK_ENTITY_3RD_W_OLD (leader, LEADER, x_direction);
  CHK_ENTITY_3RD_W_OLD (leader, LEADER, inspt_offset);
  CHK_ENTITY_3RD (leader, LEADER, endptproj);

  CHK_ENTITY_TYPE (leader, LEADER, unknown_bit_1, B);
  CHK_ENTITY_TYPE_W_OLD (leader, LEADER, path_type, BS);
  CHK_ENTITY_TYPE_W_OLD (leader, LEADER, annot_type, BS);
  CHK_ENTITY_TYPE_W_OLD (leader, LEADER, num_points, BL);
  if (!dwg_dynapi_entity_value (leader, "LEADER", "points", &points, NULL))
    fail ("LEADER.points");
  else
    for (i = 0; i < leader->num_points; i++)
      (void)points[i].z;
  CHK_ENTITY_TYPE_W_OLD (leader, LEADER, dimgap, BD);
  CHK_ENTITY_TYPE_W_OLD (leader, LEADER, box_height, BD);
  CHK_ENTITY_TYPE_W_OLD (leader, LEADER, box_width, BD);
  CHK_ENTITY_TYPE_W_OLD (leader, LEADER, hookline_dir, B);
  CHK_ENTITY_TYPE_W_OLD (leader, LEADER, arrowhead_on, B);
  CHK_ENTITY_TYPE_W_OLD (leader, LEADER, arrowhead_type, BS);
  CHK_ENTITY_TYPE_W_OLD (leader, LEADER, dimasz, BD);
  CHK_ENTITY_TYPE (leader, LEADER, unknown_bit_2, B);
  CHK_ENTITY_TYPE (leader, LEADER, unknown_bit_3, B);
  CHK_ENTITY_TYPE (leader, LEADER, unknown_short_1, BS);
  CHK_ENTITY_TYPE_W_OLD (leader, LEADER, byblock_color, BS);
  CHK_ENTITY_TYPE (leader, LEADER, hookline_on, B);
  CHK_ENTITY_TYPE (leader, LEADER, unknown_bit_4, B);
  CHK_ENTITY_TYPE (leader, LEADER, unknown_bit_5, B);

  CHK_ENTITY_H (leader, LEADER, associated_annotation);
  CHK_ENTITY_H (leader, LEADER, dimstyle);
}
