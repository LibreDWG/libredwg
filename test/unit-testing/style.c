#define DWG_TYPE DWG_TYPE_STYLE
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_RS flag;
  BITCODE_T name;
  BITCODE_RS used;
  BITCODE_B is_xref_ref;
  BITCODE_BS is_xref_resolved;
  BITCODE_B is_xref_dep;
  BITCODE_H xref;

  BITCODE_B is_shape;
  BITCODE_B is_vertical;
  BITCODE_BD text_size;
  BITCODE_BD width_factor;
  BITCODE_BD oblique_ang;
  BITCODE_RC generation;
  BITCODE_BD last_height;
  BITCODE_TV font_file;
  BITCODE_TV bigfont_file;
  BITCODE_BL ttf_flags;
  BITCODE_T ttf_typeface;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_style *_obj = dwg_object_to_STYLE (obj);

  CHK_ENTITY_TYPE (_obj, STYLE, flag, RC, flag);
  CHK_ENTITY_UTF8TEXT (_obj, STYLE, name, name);
  CHK_ENTITY_TYPE (_obj, STYLE, used, RS, used);
  CHK_ENTITY_TYPE (_obj, STYLE, is_xref_ref, B, is_xref_ref);
  CHK_ENTITY_TYPE (_obj, STYLE, is_xref_resolved, BS, is_xref_resolved);
  CHK_ENTITY_TYPE (_obj, STYLE, is_xref_dep, B, is_xref_dep);
  CHK_ENTITY_H (_obj, STYLE, xref, xref);

  CHK_ENTITY_TYPE (_obj, STYLE, is_vertical, B, is_vertical);
  CHK_ENTITY_TYPE (_obj, STYLE, is_shape, B, is_shape);
  CHK_ENTITY_TYPE (_obj, STYLE, text_size, BD, text_size);
  CHK_ENTITY_TYPE (_obj, STYLE, width_factor, BD, width_factor);
  CHK_ENTITY_TYPE (_obj, STYLE, oblique_ang, BD, oblique_ang);
  CHK_ENTITY_TYPE (_obj, STYLE, generation, RC, generation);
  CHK_ENTITY_TYPE (_obj, STYLE, last_height, BD, last_height);
  CHK_ENTITY_UTF8TEXT (_obj, STYLE, font_file, font_file);
  CHK_ENTITY_UTF8TEXT (_obj, STYLE, bigfont_file, bigfont_file);
  CHK_ENTITY_TYPE (_obj, STYLE, ttf_flags, BL, ttf_flags);
  CHK_ENTITY_UTF8TEXT (_obj, STYLE, ttf_typeface, ttf_typeface);
}
