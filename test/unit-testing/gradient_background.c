// unstable
#define DWG_TYPE DWG_TYPE_GRADIENT_BACKGROUND
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error = 0, isnew;
  BITCODE_BL class_version;
  BITCODE_BLx color_top;
  BITCODE_BLx color_middle;
  BITCODE_BLx color_bottom;
  BITCODE_BD horizon;
  BITCODE_BD height;
  BITCODE_BD rotation;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_gradient_background *_obj = dwg_object_to_GRADIENT_BACKGROUND (obj);

  CHK_ENTITY_TYPE (_obj, GRADIENT_BACKGROUND, class_version, BL);
  CHK_ENTITY_TYPE (_obj, GRADIENT_BACKGROUND, color_top, BLx);
  CHK_ENTITY_TYPE (_obj, GRADIENT_BACKGROUND, color_middle, BLx);
  CHK_ENTITY_TYPE (_obj, GRADIENT_BACKGROUND, color_bottom, BLx);
  CHK_ENTITY_TYPE (_obj, GRADIENT_BACKGROUND, horizon, BD);
  CHK_ENTITY_TYPE (_obj, GRADIENT_BACKGROUND, height, BD);
  CHK_ENTITY_TYPE (_obj, GRADIENT_BACKGROUND, rotation, BD);
}
