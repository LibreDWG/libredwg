#define DWG_TYPE DWG_TYPE_MTEXT
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  double rect_height, rect_width, text_height, extents_ht, extents_wid,
      linespace_factor;
  BITCODE_BS attachment, drawing_dir, linespace_style, class_version;
  BITCODE_B unknown_bit, annotative, default_flag;
  BITCODE_BL bg_fill_flag, bg_fill_scale, bg_fill_trans, column_type;
  char *text;
  dwg_point_3d ins_pt, ext, x_axis_dir, pt3d;
  BITCODE_H appid, style;

  dwg_ent_mtext *mtext = dwg_object_to_MTEXT (obj);
  Dwg_Version_Type version = obj->parent->header.version;

  printf ("MTEXT.text: \"%s\"\n", mtext->text);
  if (!dwg_dynapi_entity_utf8text (mtext, "MTEXT", "text", &text, NULL))
    {
      fail ("dynapi for MTEXT.text");
    }
  else
    {
      printf ("MTEXT.text: \"%s\" (utf8)\n", text);
    }
  if (strcmp (dwg_ent_mtext_get_text (mtext, &error), mtext->text))
    fail ("old API dwg_ent_mtext_get_text");
  CHK_ENTITY_3RD_W_OLD (mtext, MTEXT, insertion_pt, ins_pt);
  CHK_ENTITY_3RD_W_OLD (mtext, MTEXT, extrusion, ext);
  CHK_ENTITY_3RD_W_OLD (mtext, MTEXT, x_axis_dir, x_axis_dir);
  CHK_ENTITY_TYPE_W_OLD (mtext, MTEXT, rect_width, BD, rect_width);
  CHK_ENTITY_TYPE_W_OLD (mtext, MTEXT, rect_height, BD, rect_height);
  CHK_ENTITY_TYPE_W_OLD (mtext, MTEXT, text_height, BD, text_height);
  CHK_ENTITY_TYPE_W_OLD (mtext, MTEXT, attachment, BS, attachment);
  CHK_ENTITY_TYPE_W_OLD (mtext, MTEXT, drawing_dir, BS, drawing_dir);
  CHK_ENTITY_TYPE_W_OLD (mtext, MTEXT, extents_height, BD, extents_ht);
  CHK_ENTITY_TYPE_W_OLD (mtext, MTEXT, extents_width, BD, extents_wid);
  if (version >= R_2000)
    {
      CHK_ENTITY_TYPE_W_OLD (mtext, MTEXT, linespace_style, BS,
                             linespace_style);
      CHK_ENTITY_TYPE_W_OLD (mtext, MTEXT, linespace_factor, BD,
                             linespace_factor);
      CHK_ENTITY_TYPE (mtext, MTEXT, unknown_bit, B, unknown_bit);
    }
  if (version >= R_2004)
    {
      CHK_ENTITY_TYPE (mtext, MTEXT, bg_fill_flag, BL, bg_fill_flag);
      CHK_ENTITY_TYPE (mtext, MTEXT, bg_fill_scale, BL, bg_fill_scale);
      CHK_ENTITY_TYPE (mtext, MTEXT, bg_fill_trans, BL, bg_fill_trans);
    }
  if (version >= R_2018)
    {
      CHK_ENTITY_TYPE (mtext, MTEXT, annotative, B, annotative);
      CHK_ENTITY_TYPE (mtext, MTEXT, class_version, BS, class_version);
      CHK_ENTITY_TYPE (mtext, MTEXT, default_flag, B, default_flag);
      CHK_ENTITY_H (mtext, MTEXT, appid, appid);
      CHK_ENTITY_TYPE (mtext, MTEXT, column_type, BL, column_type);
    }
  CHK_ENTITY_H (mtext, MTEXT, style, style);
}
