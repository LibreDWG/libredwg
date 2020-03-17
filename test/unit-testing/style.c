#define DWG_TYPE DWG_TYPE_STYLE
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_RC flag;
  BITCODE_TV name;
  BITCODE_RS used;
  BITCODE_B xrefref;
  BITCODE_BS xrefindex_plus1;
  BITCODE_B xrefdep;
  BITCODE_B vertical;
  BITCODE_B shape_file;
  BITCODE_BD fixed_height;
  BITCODE_BD width_factor;
  BITCODE_BD oblique_ang;
  BITCODE_RC generation;
  BITCODE_BD last_height;
  BITCODE_TV font_name;
  BITCODE_TV bigfont_name;
  BITCODE_H extref;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_style *_obj = dwg_object_to_STYLE (obj);

  CHK_ENTITY_TYPE (_obj, STYLE, flag, RC, flag);
  CHK_ENTITY_UTF8TEXT (_obj, STYLE, name, name);
  CHK_ENTITY_TYPE (_obj, STYLE, used, RS, used);
  CHK_ENTITY_TYPE (_obj, STYLE, xrefref, B, xrefref);
  CHK_ENTITY_TYPE (_obj, STYLE, xrefindex_plus1, BS, xrefindex_plus1);
  CHK_ENTITY_TYPE (_obj, STYLE, xrefdep, B, xrefdep);
  CHK_ENTITY_TYPE (_obj, STYLE, vertical, B, vertical);
  CHK_ENTITY_TYPE (_obj, STYLE, shape_file, B, shape_file);
  CHK_ENTITY_TYPE (_obj, STYLE, fixed_height, BD, fixed_height);
  CHK_ENTITY_TYPE (_obj, STYLE, width_factor, BD, width_factor);
  CHK_ENTITY_TYPE (_obj, STYLE, oblique_ang, BD, oblique_ang);
  CHK_ENTITY_TYPE (_obj, STYLE, generation, RC, generation);
  CHK_ENTITY_TYPE (_obj, STYLE, last_height, BD, last_height);
  CHK_ENTITY_UTF8TEXT (_obj, STYLE, font_name, font_name);
  CHK_ENTITY_UTF8TEXT (_obj, STYLE, bigfont_name, bigfont_name);
  CHK_ENTITY_H (_obj, STYLE, extref, extref);  
}
