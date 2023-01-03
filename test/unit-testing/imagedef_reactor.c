#define DWG_TYPE DWG_TYPE_IMAGEDEF_REACTOR
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_BL class_version;

  // Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_imagedef_reactor *_obj = dwg_object_to_IMAGEDEF_REACTOR (obj);

  CHK_ENTITY_TYPE (_obj, IMAGEDEF_REACTOR, class_version, BL);
}
