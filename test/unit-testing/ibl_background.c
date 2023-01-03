// unstable. RAPIDRTRENDERENVIRONMENT
#define DWG_TYPE DWG_TYPE_IBL_BACKGROUND
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error = 0, isnew;
  BITCODE_BL class_version;
  BITCODE_B enable;
  BITCODE_T name;
  BITCODE_BD rotation;
  BITCODE_B display_image;
  BITCODE_H secondary_background;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_ibl_background *_obj = dwg_object_to_IBL_BACKGROUND (obj);

  CHK_ENTITY_TYPE (_obj, IBL_BACKGROUND, class_version, BL);
  CHK_ENTITY_TYPE (_obj, IBL_BACKGROUND, enable, B);
  CHK_ENTITY_UTF8TEXT (_obj, IBL_BACKGROUND, name);
  CHK_ENTITY_TYPE (_obj, IBL_BACKGROUND, rotation,
                   BD); // in degree, not radian
  CHK_ENTITY_TYPE (_obj, IBL_BACKGROUND, display_image, B);
  CHK_ENTITY_H (_obj, IBL_BACKGROUND, secondary_background);
}
