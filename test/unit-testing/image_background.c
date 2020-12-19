// unstable
#define DWG_TYPE DWG_TYPE_IMAGE_BACKGROUND
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error = 0, isnew;
  BITCODE_BL class_version;
  BITCODE_T filename;
  BITCODE_B fit_to_screen;
  BITCODE_B maintain_aspect_ratio;
  BITCODE_B use_tiling;
  BITCODE_2BD offset;
  BITCODE_2BD scale;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_image_background *_obj = dwg_object_to_IMAGE_BACKGROUND (obj);

  CHK_ENTITY_TYPE (_obj, IMAGE_BACKGROUND, class_version, BL);
  CHK_ENTITY_UTF8TEXT (_obj, IMAGE_BACKGROUND, filename);
  CHK_ENTITY_TYPE (_obj, IMAGE_BACKGROUND, fit_to_screen, B);
  CHK_ENTITY_TYPE (_obj, IMAGE_BACKGROUND, maintain_aspect_ratio, B);
  CHK_ENTITY_TYPE (_obj, IMAGE_BACKGROUND, use_tiling, B);
  CHK_ENTITY_2RD (_obj, IMAGE_BACKGROUND, offset);
  CHK_ENTITY_2RD (_obj, IMAGE_BACKGROUND, scale);
}
