#define DWG_TYPE DWG_TYPE_LEADER
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  dwg_point_3d origin, ext, x_direction, offset_to_block_ins_pt, endptproj;
  dwg_point_3d *points;
  BITCODE_B unknown_bit_1;
  BITCODE_BS path_type;
  BITCODE_BS annot_type;
  BITCODE_BL num_points;
  BITCODE_BD dimgap;
  BITCODE_BD box_height;
  BITCODE_BD box_width;
  BITCODE_B hookline_dir;
  BITCODE_B arrowhead_on;
  BITCODE_BS arrowhead_type;
  BITCODE_BD dimasz;
  BITCODE_B unknown_bit_2;
  BITCODE_B unknown_bit_3;
  BITCODE_BS unknown_short_1;
  BITCODE_BS byblock_color;
  BITCODE_B hookline_on;
  BITCODE_B unknown_bit_5;
  BITCODE_H associated_annotation;
  BITCODE_H dimstyle;

  dwg_ent_leader *leader = dwg_object_to_LEADER (obj);

  CHK_ENTITY_3RD_W_OLD (leader, LEADER, origin, origin);
  CHK_ENTITY_3RD_W_OLD (leader, LEADER, extrusion, ext);
  CHK_ENTITY_3RD_W_OLD (leader, LEADER, x_direction, x_direction);
  CHK_ENTITY_3RD_W_OLD (leader, LEADER, offset_to_block_ins_pt, offset_to_block_ins_pt);
  CHK_ENTITY_3RD (leader, LEADER, endptproj, endptproj);

  CHK_ENTITY_TYPE (leader, LEADER, unknown_bit_1, B, unknown_bit_1);
  CHK_ENTITY_TYPE_W_OLD (leader, LEADER, path_type, BS, path_type);
  CHK_ENTITY_TYPE_W_OLD (leader, LEADER, annot_type, BS, annot_type);
  CHK_ENTITY_TYPE_W_OLD (leader, LEADER, num_points, BL, num_points);
  CHK_ENTITY_TYPE_W_OLD (leader, LEADER, dimgap, BD, dimgap);
  CHK_ENTITY_TYPE_W_OLD (leader, LEADER, box_height, BD, box_height);
  CHK_ENTITY_TYPE_W_OLD (leader, LEADER, box_width, BD, box_width);
  CHK_ENTITY_TYPE_W_OLD (leader, LEADER, hookline_dir, B, hookline_dir);
  CHK_ENTITY_TYPE_W_OLD (leader, LEADER, arrowhead_on, B, arrowhead_on);
  CHK_ENTITY_TYPE_W_OLD (leader, LEADER, arrowhead_type, BS, arrowhead_type);
  CHK_ENTITY_TYPE_W_OLD (leader, LEADER, dimasz, BD, dimasz);
  CHK_ENTITY_TYPE (leader, LEADER, unknown_bit_2, B, unknown_bit_2);
  CHK_ENTITY_TYPE (leader, LEADER, unknown_bit_3, B, unknown_bit_3);
  CHK_ENTITY_TYPE (leader, LEADER, unknown_short_1, BS, unknown_short_1);
  CHK_ENTITY_TYPE_W_OLD (leader, LEADER, byblock_color, BS, byblock_color);
  CHK_ENTITY_TYPE (leader, LEADER, hookline_on, B, hookline_on);
  CHK_ENTITY_TYPE (leader, LEADER, unknown_bit_5, B, unknown_bit_5);

  CHK_ENTITY_H (leader, LEADER, associated_annotation, associated_annotation);
  CHK_ENTITY_H (leader, LEADER, dimstyle, dimstyle);
}
