// unstable
#define DWG_TYPE DWG_TYPE_TEXTOBJECTCONTEXTDATA
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  ANNOTSCALEOBJECTCONTEXTDATA_fields;
  /*BITCODE_BS class_version; // r2010+ =3
  BITCODE_B is_default;
  BITCODE_B has_xdic;
  BITCODE_H scale; */
  BITCODE_BS horizontal_mode;
  BITCODE_BD rotation;
  BITCODE_2BD ins_pt;
  BITCODE_2BD alignment_pt;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_textobjectcontextdata *_obj
      = dwg_object_to_TEXTOBJECTCONTEXTDATA (obj);

  CHK_ENTITY_TYPE (_obj, TEXTOBJECTCONTEXTDATA, class_version, BS);
  CHK_ENTITY_TYPE (_obj, TEXTOBJECTCONTEXTDATA, is_default, B);
  CHK_ENTITY_H (_obj, TEXTOBJECTCONTEXTDATA, scale);
  CHK_ENTITY_TYPE (_obj, TEXTOBJECTCONTEXTDATA, horizontal_mode, BS);
  CHK_ENTITY_TYPE (_obj, TEXTOBJECTCONTEXTDATA, rotation, BD);
  CHK_ENTITY_2RD (_obj, TEXTOBJECTCONTEXTDATA, ins_pt);
  CHK_ENTITY_2RD (_obj, TEXTOBJECTCONTEXTDATA, alignment_pt);
}
