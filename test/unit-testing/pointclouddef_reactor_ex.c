// TODO coverage
#define DWG_TYPE DWG_TYPE_POINTCLOUDDEF_REACTOR_EX
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BL class_version;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  // #ifdef DEBUG_CLASSES
  dwg_obj_pointclouddef_reactor_ex *_obj
      = dwg_object_to_POINTCLOUDDEF_REACTOR_EX (obj);

  CHK_ENTITY_TYPE (_obj, POINTCLOUDDEF_REACTOR_EX, class_version, BL);
  CHK_ENTITY_MAX (_obj, POINTCLOUDDEF_REACTOR_EX, class_version, BL, 2);
  // #endif
}
