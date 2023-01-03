#define DWG_TYPE DWG_TYPE_MLEADERSTYLE
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BS class_version;
  BITCODE_BS content_type;
  BITCODE_BS mleader_order;
  BITCODE_BS leader_order;
  BITCODE_BL max_points;
  BITCODE_BD first_seg_angle;
  BITCODE_BD second_seg_angle;
  BITCODE_BS type;
  BITCODE_CMC line_color;
  BITCODE_H line_type;
  BITCODE_BLd linewt;
  BITCODE_B has_landing;
  BITCODE_B has_dogleg;
  BITCODE_BD landing_gap;
  BITCODE_BD landing_dist;
  BITCODE_TV description;
  BITCODE_H arrow_head;
  BITCODE_BD arrow_head_size;
  BITCODE_TV text_default;
  BITCODE_H text_style;
  BITCODE_BS attach_left;
  BITCODE_BS attach_right;
  BITCODE_BS text_angle_type;
  BITCODE_BS text_align_type;
  BITCODE_CMC text_color;
  BITCODE_BD text_height;
  BITCODE_B has_text_frame;
  // BITCODE_B is_new_format; /* computed */
  BITCODE_B text_always_left;
  BITCODE_BD align_space;
  BITCODE_H block;
  BITCODE_CMC block_color;
  BITCODE_3BD block_scale;
  BITCODE_B use_block_scale;
  BITCODE_BD block_rotation;
  BITCODE_B use_block_rotation;
  BITCODE_BS block_connection;
  BITCODE_BD scale;
  BITCODE_B is_changed;
  BITCODE_B is_annotative;
  BITCODE_BD break_size;
  BITCODE_BS attach_dir;
  BITCODE_BS attach_top;
  BITCODE_BS attach_bottom;
  BITCODE_B text_extended;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_mleaderstyle *_obj = dwg_object_to_MLEADERSTYLE (obj);

  CHK_ENTITY_TYPE (_obj, MLEADERSTYLE, class_version, BS);
  CHK_ENTITY_TYPE (_obj, MLEADERSTYLE, content_type, BS);
  CHK_ENTITY_TYPE (_obj, MLEADERSTYLE, mleader_order, BS);
  CHK_ENTITY_TYPE (_obj, MLEADERSTYLE, leader_order, BS);
  CHK_ENTITY_TYPE (_obj, MLEADERSTYLE, max_points, BL);
  CHK_ENTITY_TYPE (_obj, MLEADERSTYLE, first_seg_angle, BD);
  CHK_ENTITY_TYPE (_obj, MLEADERSTYLE, second_seg_angle, BD);
  CHK_ENTITY_TYPE (_obj, MLEADERSTYLE, type, BS);
  CHK_ENTITY_CMC (_obj, MLEADERSTYLE, line_color);
  CHK_ENTITY_H (_obj, MLEADERSTYLE, line_type);
  CHK_ENTITY_TYPE (_obj, MLEADERSTYLE, linewt, BLd);
  CHK_ENTITY_TYPE (_obj, MLEADERSTYLE, has_landing, B);
  CHK_ENTITY_TYPE (_obj, MLEADERSTYLE, has_dogleg, B);
  CHK_ENTITY_TYPE (_obj, MLEADERSTYLE, landing_gap, BD);
  CHK_ENTITY_TYPE (_obj, MLEADERSTYLE, landing_dist, BD);
  CHK_ENTITY_UTF8TEXT (_obj, MLEADERSTYLE, description);
  CHK_ENTITY_H (_obj, MLEADERSTYLE, arrow_head);
  CHK_ENTITY_TYPE (_obj, MLEADERSTYLE, arrow_head_size, BD);
  CHK_ENTITY_UTF8TEXT (_obj, MLEADERSTYLE, text_default);
  CHK_ENTITY_H (_obj, MLEADERSTYLE, text_style);
  CHK_ENTITY_TYPE (_obj, MLEADERSTYLE, attach_left, BS);
  CHK_ENTITY_TYPE (_obj, MLEADERSTYLE, attach_right, BS);
  CHK_ENTITY_TYPE (_obj, MLEADERSTYLE, text_angle_type, BS);
  CHK_ENTITY_TYPE (_obj, MLEADERSTYLE, text_align_type, BS);
  CHK_ENTITY_CMC (_obj, MLEADERSTYLE, text_color);
  CHK_ENTITY_TYPE (_obj, MLEADERSTYLE, text_height, BD);
  CHK_ENTITY_TYPE (_obj, MLEADERSTYLE, has_text_frame, B);
  CHK_ENTITY_TYPE (_obj, MLEADERSTYLE, text_always_left, B);
  CHK_ENTITY_TYPE (_obj, MLEADERSTYLE, align_space, BD);
  CHK_ENTITY_H (_obj, MLEADERSTYLE, block);
  CHK_ENTITY_CMC (_obj, MLEADERSTYLE, block_color);
  CHK_ENTITY_3RD (_obj, MLEADERSTYLE, block_scale);
  CHK_ENTITY_TYPE (_obj, MLEADERSTYLE, use_block_scale, B);
  CHK_ENTITY_TYPE (_obj, MLEADERSTYLE, block_rotation, BD);
  CHK_ENTITY_TYPE (_obj, MLEADERSTYLE, use_block_rotation, B);
  CHK_ENTITY_TYPE (_obj, MLEADERSTYLE, block_connection, BS);
  CHK_ENTITY_TYPE (_obj, MLEADERSTYLE, scale, BD);
  CHK_ENTITY_TYPE (_obj, MLEADERSTYLE, is_changed, B);
  CHK_ENTITY_TYPE (_obj, MLEADERSTYLE, is_annotative, B);
  CHK_ENTITY_TYPE (_obj, MLEADERSTYLE, break_size, BD);
  CHK_ENTITY_TYPE (_obj, MLEADERSTYLE, attach_dir, BS);
  CHK_ENTITY_TYPE (_obj, MLEADERSTYLE, attach_top, BS);
  CHK_ENTITY_TYPE (_obj, MLEADERSTYLE, attach_bottom, BS);
  CHK_ENTITY_TYPE (_obj, MLEADERSTYLE, text_extended, B);
}
