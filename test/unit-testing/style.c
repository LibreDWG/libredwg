#define DWG_TYPE DWG_TYPE_STYLE
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_RC flag;
  BITCODE_T name;
  BITCODE_RSd used;
  BITCODE_B is_xref_ref;
  BITCODE_BS is_xref_resolved;
  BITCODE_B is_xref_dep;
  BITCODE_H xref;

  BITCODE_B is_shape;
  BITCODE_B is_vertical;
  BITCODE_BD text_size;
  BITCODE_BD width_factor;
  BITCODE_BD oblique_angle;
  BITCODE_RC generation;
  BITCODE_BD last_height;
  BITCODE_TV font_file;
  BITCODE_TV bigfont_file;
  // BITCODE_BL ttf_flags;
  // BITCODE_T ttf_typeface;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_style *_obj = dwg_object_to_STYLE (obj);

  CHK_ENTITY_TYPE (_obj, STYLE, flag, RC);
  CHK_ENTITY_UTF8TEXT (_obj, STYLE, name);
  CHK_ENTITY_TYPE (_obj, STYLE, used, RSd);
  CHK_ENTITY_TYPE (_obj, STYLE, is_xref_ref, B);
  CHK_ENTITY_TYPE (_obj, STYLE, is_xref_resolved, BS);
  CHK_ENTITY_TYPE (_obj, STYLE, is_xref_dep, B);
  CHK_ENTITY_H (_obj, STYLE, xref);

  CHK_ENTITY_TYPE (_obj, STYLE, is_vertical, B);
  CHK_ENTITY_TYPE (_obj, STYLE, is_shape, B);
  CHK_ENTITY_TYPE (_obj, STYLE, text_size, BD);
  CHK_ENTITY_TYPE (_obj, STYLE, width_factor, BD);
  CHK_ENTITY_TYPE (_obj, STYLE, oblique_angle, BD);
  CHK_ENTITY_TYPE (_obj, STYLE, generation, RC);
  CHK_ENTITY_TYPE (_obj, STYLE, last_height, BD);
  CHK_ENTITY_UTF8TEXT (_obj, STYLE, font_file);
  CHK_ENTITY_UTF8TEXT (_obj, STYLE, bigfont_file);
  // CHK_ENTITY_TYPE (_obj, STYLE, ttf_flags, BL);
  // CHK_ENTITY_UTF8TEXT (_obj, STYLE, ttf_typeface);
}
