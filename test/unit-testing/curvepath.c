// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_CURVEPATH
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_BS class_version;
  BITCODE_H entity;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_curvepath *_obj = dwg_object_to_CURVEPATH (obj);

  CHK_ENTITY_TYPE (_obj, CURVEPATH, class_version, BS);
  CHK_ENTITY_MAX (_obj, CURVEPATH, class_version, BS, 3);
  CHK_ENTITY_H (_obj, CURVEPATH, entity);
#endif
}
