#define DWG_TYPE DWG_TYPE_MTEXT
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  double rect_height, rect_width, text_height, extents_height, extents_width,
      linespace_factor;
  BITCODE_BS attachment, flow_dir, linespace_style, class_version;
  BITCODE_B unknown_b0, is_not_annotative, default_flag;
  BITCODE_BL bg_fill_flag, bg_fill_scale, bg_fill_trans;
  BITCODE_CMC bg_fill_color;
  char *text;
  dwg_point_3d ins_pt, extrusion, x_axis_dir;
  BITCODE_H appid, style;
  BITCODE_BL ignore_attachment;
  BITCODE_BL numfragments;
  BITCODE_BD column_width;
  BITCODE_BD gutter;
  BITCODE_B auto_height;
  BITCODE_B flow_reversed;
  BITCODE_BS column_type;
  BITCODE_BL i, num_column_heights;
  BITCODE_BD *column_heights;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_ent_mtext *mtext = dwg_object_to_MTEXT (obj);

  CHK_ENTITY_UTF8TEXT_W_OLD (mtext, MTEXT, text);
  CHK_ENTITY_3RD (mtext, MTEXT, ins_pt);
  CHK_ENTITY_3RD_W_OLD (mtext, MTEXT, extrusion);
  CHK_ENTITY_3RD_W_OLD (mtext, MTEXT, x_axis_dir);
  CHK_ENTITY_TYPE_W_OLD (mtext, MTEXT, rect_width, BD);
  CHK_ENTITY_TYPE_W_OLD (mtext, MTEXT, rect_height, BD);
  CHK_ENTITY_TYPE_W_OLD (mtext, MTEXT, text_height, BD);
  CHK_ENTITY_TYPE_W_OLD (mtext, MTEXT, attachment, BS);
  CHK_ENTITY_MAX (mtext, MTEXT, attachment, BS, 9);
  CHK_ENTITY_TYPE (mtext, MTEXT, flow_dir, BS); // was drawing_dir in dwg_api
  CHK_ENTITY_MAX (mtext, MTEXT, flow_dir, BS, 5);
  CHK_ENTITY_TYPE_W_OLD (mtext, MTEXT, extents_height, BD);
  CHK_ENTITY_TYPE_W_OLD (mtext, MTEXT, extents_width, BD);
  CHK_ENTITY_H (mtext, MTEXT, style);
  SINCE (R_2000)
  {
    CHK_ENTITY_TYPE_W_OLD (mtext, MTEXT, linespace_style, BS);
    CHK_ENTITY_TYPE_W_OLD (mtext, MTEXT, linespace_factor, BD);
    CHK_ENTITY_TYPE (mtext, MTEXT, unknown_b0, B);
  }
  SINCE (R_2004)
  {
    CHK_ENTITY_TYPE (mtext, MTEXT, bg_fill_flag, BL);
    CHK_ENTITY_MAX (mtext, MTEXT, bg_fill_flag, BL, 31);
    CHK_ENTITY_TYPE (mtext, MTEXT, bg_fill_scale, BL);
    CHK_ENTITY_CMC (mtext, MTEXT, bg_fill_color);
    CHK_ENTITY_TYPE (mtext, MTEXT, bg_fill_trans, BL);
  }
  SINCE (R_2018)
  {
    CHK_ENTITY_TYPE (mtext, MTEXT, is_not_annotative, B);
    CHK_ENTITY_TYPE (mtext, MTEXT, class_version, BS);
    CHK_ENTITY_MAX (mtext, MTEXT, class_version, BS, 10);
    CHK_ENTITY_TYPE (mtext, MTEXT, default_flag, B);
    CHK_ENTITY_H (mtext, MTEXT, appid);
    CHK_ENTITY_TYPE (mtext, MTEXT, column_type, BS);
    CHK_ENTITY_MAX (mtext, MTEXT, column_type, BS, 2);
    CHK_ENTITY_TYPE (mtext, MTEXT, ignore_attachment, BL);
    if ((BITCODE_BL)attachment != ignore_attachment)
      fprintf (stderr,
               "attachment " FORMAT_BS " != ignore_attachment " FORMAT_BL "\n",
               attachment, ignore_attachment);
    CHK_ENTITY_TYPE (mtext, MTEXT, numfragments,
                     BL); // only for column_type == 1
    CHK_ENTITY_TYPE (mtext, MTEXT, num_column_heights, BL);
    if (column_type == 1)
      {
        if (num_column_heights)
          fail ("num_column_heights with column_type == 1");
      }
    else
      {
        if (numfragments)
          fail ("numfragments with column_type != 1");
      }
    CHK_ENTITY_TYPE (mtext, MTEXT, column_width, BD);
    CHK_ENTITY_TYPE (mtext, MTEXT, gutter, BD);
    CHK_ENTITY_TYPE (mtext, MTEXT, auto_height, B);
    CHK_ENTITY_TYPE (mtext, MTEXT, flow_reversed, B);
    CHK_ENTITY_VECTOR_TYPE (mtext, MTEXT, column_heights, num_column_heights,
                            BD);
  }
}
