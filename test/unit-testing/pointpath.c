// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_POINTPATH
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_BS class_version;
  BITCODE_3BD point;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_pointpath *_obj = dwg_object_to_POINTPATH (obj);

  CHK_ENTITY_TYPE (_obj, POINTPATH, class_version, BS);
  CHK_ENTITY_MAX (_obj, POINTPATH, class_version, BS, 3);
  CHK_ENTITY_3RD (_obj, POINTPATH, point);
#endif
}
