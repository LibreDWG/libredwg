// TODO coverage
#define DWG_TYPE DWG_TYPE_POINTCLOUDDEFEX
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BL i;
  BITCODE_BL class_version;
  BITCODE_T source_filename;
  BITCODE_B is_loaded;
  BITCODE_RLL numpoints;
  BITCODE_3BD extents_min;
  BITCODE_3BD extents_max;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  // #ifdef DEBUG_CLASSES
  dwg_obj_pointclouddefex *_obj = dwg_object_to_POINTCLOUDDEFEX (obj);

  CHK_ENTITY_TYPE (_obj, POINTCLOUDDEFEX, class_version, BL);
  CHK_ENTITY_MAX (_obj, POINTCLOUDDEFEX, class_version, BL, 3);
  CHK_ENTITY_UTF8TEXT (_obj, POINTCLOUDDEFEX, source_filename);
  CHK_ENTITY_TYPE (_obj, POINTCLOUDDEFEX, is_loaded, B);
  CHK_ENTITY_TYPE (_obj, POINTCLOUDDEFEX, numpoints, RLL);
  CHK_ENTITY_3RD (_obj, POINTCLOUDDEFEX, extents_min);
  CHK_ENTITY_3RD (_obj, POINTCLOUDDEFEX, extents_max);
  // #endif
}
