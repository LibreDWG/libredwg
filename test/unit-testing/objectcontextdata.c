// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_OBJECTCONTEXTDATA
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BS class_version; /*!< r2010+ =4 */
  BITCODE_B is_default;
  BITCODE_B has_xdic;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_objectcontextdata *_obj = dwg_object_to_OBJECTCONTEXTDATA (obj);

  CHK_ENTITY_TYPE (_obj, OBJECTCONTEXTDATA, class_version, BS);
  CHK_ENTITY_TYPE (_obj, OBJECTCONTEXTDATA, is_default, B);
  CHK_ENTITY_TYPE (_obj, OBJECTCONTEXTDATA, has_xdic, B);

#endif
}
