#define DWG_TYPE DWG_TYPE_RENDERSETTINGS
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BL class_version;
  BITCODE_T name;
  BITCODE_B fog_enabled;
  BITCODE_B fog_background_enabled;
  BITCODE_B backfaces_enabled;
  BITCODE_B environ_image_enabled;
  BITCODE_T environ_image_filename;
  BITCODE_T description;
  BITCODE_BL display_index;
  BITCODE_B has_predefined; /*!< DXF 290, r2013 only */

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_rendersettings *_obj = dwg_object_to_RENDERSETTINGS (obj);

  CHK_ENTITY_TYPE (_obj, RENDERSETTINGS, class_version, BL);
  CHK_ENTITY_UTF8TEXT (_obj, RENDERSETTINGS, name);
  CHK_ENTITY_TYPE (_obj, RENDERSETTINGS, fog_enabled, B);
  CHK_ENTITY_TYPE (_obj, RENDERSETTINGS, fog_background_enabled, B);
  CHK_ENTITY_TYPE (_obj, RENDERSETTINGS, backfaces_enabled, B);
  CHK_ENTITY_TYPE (_obj, RENDERSETTINGS, environ_image_enabled, B);
  CHK_ENTITY_UTF8TEXT (_obj, RENDERSETTINGS, environ_image_filename);
  CHK_ENTITY_UTF8TEXT (_obj, RENDERSETTINGS, description);
  CHK_ENTITY_TYPE (_obj, RENDERSETTINGS, display_index, BL);
  CHK_ENTITY_TYPE (_obj, RENDERSETTINGS, has_predefined, B);
}
