#define DWG_TYPE DWG_TYPE_RENDERGLOBAL
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BL class_version;
  BITCODE_BL procedure;
  BITCODE_BL destination;
  BITCODE_B save_enabled;
  BITCODE_T save_filename;
  BITCODE_BL image_width;
  BITCODE_BL image_height;
  BITCODE_B predef_presets_first;
  BITCODE_B highlevel_info;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_renderglobal *_obj = dwg_object_to_RENDERGLOBAL (obj);

  CHK_ENTITY_TYPE (_obj, RENDERGLOBAL, class_version, BL);
  CHK_ENTITY_TYPE (_obj, RENDERGLOBAL, procedure, BL);
  CHK_ENTITY_TYPE (_obj, RENDERGLOBAL, destination, BL);
  CHK_ENTITY_TYPE (_obj, RENDERGLOBAL, save_enabled, B);
  CHK_ENTITY_UTF8TEXT (_obj, RENDERGLOBAL, save_filename);
  CHK_ENTITY_TYPE (_obj, RENDERGLOBAL, image_width, BL);
  CHK_ENTITY_TYPE (_obj, RENDERGLOBAL, image_height, BL);
  CHK_ENTITY_TYPE (_obj, RENDERGLOBAL, predef_presets_first, B);
  CHK_ENTITY_TYPE (_obj, RENDERGLOBAL, highlevel_info, B);
}
