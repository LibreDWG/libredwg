#define DWG_TYPE DWG_TYPE_RENDERENVIRONMENT
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BL class_version;
  BITCODE_B fog_enabled;
  BITCODE_B fog_background_enabled;
  BITCODE_CMC fog_color;
  BITCODE_BD fog_density_near;
  BITCODE_BD fog_density_far;
  BITCODE_BD fog_distance_near;
  BITCODE_BD fog_distance_far;
  BITCODE_B environ_image_enabled;
  BITCODE_T environ_image_filename;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_renderenvironment *_obj = dwg_object_to_RENDERENVIRONMENT (obj);

  CHK_ENTITY_TYPE (_obj, RENDERENVIRONMENT, class_version, BL, class_version);
  CHK_ENTITY_TYPE (_obj, RENDERENVIRONMENT, fog_enabled, B, fog_enabled);
  CHK_ENTITY_TYPE (_obj, RENDERENVIRONMENT, fog_background_enabled, B, fog_background_enabled);
  CHK_ENTITY_CMC (_obj, RENDERENVIRONMENT, fog_color, fog_color);
  CHK_ENTITY_TYPE (_obj, RENDERENVIRONMENT, fog_density_near, BD, fog_density_near);
  CHK_ENTITY_TYPE (_obj, RENDERENVIRONMENT, fog_density_far, BD, fog_density_far);
  CHK_ENTITY_TYPE (_obj, RENDERENVIRONMENT, fog_distance_near, BD, fog_distance_near);
  CHK_ENTITY_TYPE (_obj, RENDERENVIRONMENT, fog_distance_far, BD, fog_distance_far);
  CHK_ENTITY_TYPE (_obj, RENDERENVIRONMENT, environ_image_enabled, B, environ_image_enabled);
  CHK_ENTITY_UTF8TEXT (_obj, RENDERENVIRONMENT, environ_image_filename, environ_image_filename);
#endif
}
