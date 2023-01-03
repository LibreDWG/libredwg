// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_MLEADEROBJECTCONTEXTDATA
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  ANNOTSCALEOBJECTCONTEXTDATA_fields;
  /*BITCODE_BS class_version; // r2010+ =3
  BITCODE_B is_default;
  BITCODE_B has_xdic;
  BITCODE_H scale; */

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_mleaderobjectcontextdata *_obj
      = dwg_object_to_MLEADEROBJECTCONTEXTDATA (obj);

  CHK_ENTITY_TYPE (_obj, MLEADEROBJECTCONTEXTDATA, class_version, BS);
  CHK_ENTITY_TYPE (_obj, MLEADEROBJECTCONTEXTDATA, is_default, B);
  CHK_ENTITY_H (_obj, MLEADEROBJECTCONTEXTDATA, scale);
  // ...
#endif
}
