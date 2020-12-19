// unstable
#define DWG_TYPE DWG_TYPE_SOLID_BACKGROUND
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error = 0, isnew;
  BITCODE_BL class_version;
  BITCODE_BLx color;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_solid_background *_obj = dwg_object_to_SOLID_BACKGROUND (obj);

  CHK_ENTITY_TYPE (_obj, SOLID_BACKGROUND, class_version, BL);
  CHK_ENTITY_TYPE (_obj, SOLID_BACKGROUND, color, BLx);
}
