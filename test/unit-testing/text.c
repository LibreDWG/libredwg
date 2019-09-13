#define DWG_TYPE DWG_TYPE_TEXT
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error = 0;
  BITCODE_RD elevation, thickness, rotation, height, oblique_ang, width_factor,
      rdvalue;
  BITCODE_BS generation, vert_align, horiz_align, bsvalue;
  BITCODE_RC dataflags, rcvalue;
  char *text_value;
  dwg_point_3d ext;
  dwg_point_2d pt2d, ins_pt, alignment_pt;
  BITCODE_H style;

  Dwg_Version_Type version = obj->parent->header.version;
  dwg_ent_text *text = dwg_object_to_TEXT (obj);

  CHK_ENTITY_UTF8TEXT (text, TEXT, text_value, text_value);
  if (version < R_2007
      && (strcmp (dwg_ent_text_get_text (text, &error), text_value) || error))
    fail ("old API dwg_ent_text_get_text");

  CHK_ENTITY_2RD (text, TEXT, insertion_pt, ins_pt);
  dwg_ent_text_get_insertion_point (text, &pt2d, &error);
  if (error || memcmp (&ins_pt, &pt2d, sizeof (ins_pt)))
    fail ("old API dwg_ent_text_get_insertion_point");
  CHK_ENTITY_2RD (text, TEXT, alignment_pt, alignment_pt);
  CHK_ENTITY_3RD_W_OLD (text, TEXT, extrusion, ext);
  CHK_ENTITY_TYPE (text, TEXT, elevation, BD, elevation);
  CHK_ENTITY_TYPE (text, TEXT, dataflags, RC, dataflags);
  CHK_ENTITY_TYPE_W_OLD (text, TEXT, height, RD, height);
  CHK_ENTITY_TYPE_W_OLD (text, TEXT, thickness, RD, thickness);
  CHK_ENTITY_TYPE_W_OLD (text, TEXT, rotation, RD, rotation);
  CHK_ENTITY_TYPE (text, TEXT, oblique_ang, RD, oblique_ang);
  CHK_ENTITY_TYPE (text, TEXT, width_factor, RD, width_factor);
  CHK_ENTITY_TYPE (text, TEXT, generation, BS, generation);

  CHK_ENTITY_TYPE_W_OLD (text, TEXT, vert_alignment, BS, vert_align);
  CHK_ENTITY_TYPE_W_OLD (text, TEXT, horiz_alignment, BS, horiz_align);
  CHK_ENTITY_H (text, TEXT, style, style);
}
