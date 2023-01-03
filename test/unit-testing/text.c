#define DWG_TYPE DWG_TYPE_TEXT
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error = 0, isnew;
  BITCODE_RD elevation, thickness, rotation, height, oblique_angle,
      width_factor;
  BITCODE_BS generation, vert_alignment, horiz_alignment;
  BITCODE_RC dataflags;
  char *text_value;
  dwg_point_3d extrusion;
  dwg_point_2d ins_pt, alignment_pt;
  BITCODE_H style;

  Dwg_Version_Type version = obj->parent->header.version;
  dwg_ent_text *text = dwg_object_to_TEXT (obj);

  CHK_ENTITY_UTF8TEXT (text, TEXT, text_value);
#ifdef USE_DEPRECATED_API
  if (version < R_2007
      && (strcmp (dwg_ent_text_get_text (text, &error), text_value) || error))
    fail ("old API dwg_ent_text_get_text");
#endif

  CHK_ENTITY_2RD (text, TEXT, ins_pt);
  CHK_ENTITY_2RD (text, TEXT, alignment_pt);
  CHK_ENTITY_3RD_W_OLD (text, TEXT, extrusion);
  CHK_ENTITY_TYPE (text, TEXT, elevation, BD);
  CHK_ENTITY_TYPE (text, TEXT, dataflags, RC);
  CHK_ENTITY_TYPE_W_OLD (text, TEXT, height, RD);
  CHK_ENTITY_TYPE_W_OLD (text, TEXT, thickness, RD);
  CHK_ENTITY_TYPE_W_OLD (text, TEXT, rotation, RD);
  CHK_ENTITY_MAX (text, TEXT, rotation, RD, 6.284);
  CHK_ENTITY_TYPE (text, TEXT, oblique_angle, RD);
  CHK_ENTITY_MAX (text, TEXT, oblique_angle, RD, 6.284);
  CHK_ENTITY_TYPE (text, TEXT, width_factor, RD);
  CHK_ENTITY_TYPE (text, TEXT, generation, BS);

  CHK_ENTITY_TYPE_W_OLD (text, TEXT, vert_alignment, BS);
  CHK_ENTITY_TYPE_W_OLD (text, TEXT, horiz_alignment, BS);
  CHK_ENTITY_H (text, TEXT, style);
}
