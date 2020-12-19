// unstable, with coverage
#define DWG_TYPE DWG_TYPE_SKYLIGHT_BACKGROUND
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error = 0, isnew;
  BITCODE_BL class_version;
  BITCODE_H sunid;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_skylight_background *_obj = dwg_object_to_SKYLIGHT_BACKGROUND (obj);

  CHK_ENTITY_TYPE (_obj, SKYLIGHT_BACKGROUND, class_version, BL);
  CHK_ENTITY_H (_obj, SKYLIGHT_BACKGROUND, sunid);
}
