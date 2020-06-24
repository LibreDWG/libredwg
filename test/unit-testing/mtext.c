#define DWG_TYPE DWG_TYPE_MTEXT
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  double rect_height, rect_width, text_height, extents_height, extents_width,
      linespace_factor;
  BITCODE_BS attachment, flow_dir, linespace_style, class_version;
  BITCODE_B unknown_bit, annotative, default_flag;
  BITCODE_BL bg_fill_flag, bg_fill_scale, bg_fill_trans, column_type;
  BITCODE_CMC bg_fill_color;
  char *text;
  dwg_point_3d insertion_pt, extrusion, x_axis_dir;
  BITCODE_H appid, style;

  dwg_ent_mtext *mtext = dwg_object_to_MTEXT (obj);

  CHK_ENTITY_UTF8TEXT_W_OLD (mtext, MTEXT, text);
  CHK_ENTITY_3RD_W_OLD (mtext, MTEXT, insertion_pt);
  CHK_ENTITY_3RD_W_OLD (mtext, MTEXT, extrusion);
  CHK_ENTITY_3RD_W_OLD (mtext, MTEXT, x_axis_dir);
  CHK_ENTITY_TYPE_W_OLD (mtext, MTEXT, rect_width, BD);
  CHK_ENTITY_TYPE_W_OLD (mtext, MTEXT, rect_height, BD);
  CHK_ENTITY_TYPE_W_OLD (mtext, MTEXT, text_height, BD);
  CHK_ENTITY_TYPE_W_OLD (mtext, MTEXT, attachment, BS);
  CHK_ENTITY_MAX  (mtext, MTEXT, attachment, BS, 9);
  CHK_ENTITY_TYPE (mtext, MTEXT, flow_dir, BS); // was drawing_dir in dwg_api
  CHK_ENTITY_MAX  (mtext, MTEXT, flow_dir, BS, 5);
  CHK_ENTITY_TYPE_W_OLD (mtext, MTEXT, extents_height, BD);
  CHK_ENTITY_TYPE_W_OLD (mtext, MTEXT, extents_width, BD);
  CHK_ENTITY_H (mtext, MTEXT, style);
  SINCE (R_2000)
    {
      CHK_ENTITY_TYPE_W_OLD (mtext, MTEXT, linespace_style, BS);
      CHK_ENTITY_TYPE_W_OLD (mtext, MTEXT, linespace_factor, BD);
      CHK_ENTITY_TYPE (mtext, MTEXT, unknown_bit, B);
    }
  SINCE (R_2004)
    {
      CHK_ENTITY_TYPE (mtext, MTEXT, bg_fill_flag, BL);
      CHK_ENTITY_TYPE (mtext, MTEXT, bg_fill_scale, BL);
      CHK_ENTITY_CMC (mtext, MTEXT, bg_fill_color);
      CHK_ENTITY_TYPE (mtext, MTEXT, bg_fill_trans, BL);
    }
  SINCE (R_2018)
    {
      CHK_ENTITY_TYPE (mtext, MTEXT, annotative, B);
      CHK_ENTITY_TYPE (mtext, MTEXT, class_version, BS);
      CHK_ENTITY_TYPE (mtext, MTEXT, default_flag, B);
      CHK_ENTITY_H (mtext, MTEXT, appid);
      CHK_ENTITY_TYPE (mtext, MTEXT, column_type, BL);
    }
}
