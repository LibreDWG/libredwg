// stable
#define DWG_TYPE DWG_TYPE_RENDERENVIRONMENT
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BL class_version;
  BITCODE_B fog_enabled;
  BITCODE_B fog_background_enabled;
  BITCODE_RC fog_color_r;
  BITCODE_RC fog_color_g;
  BITCODE_RC fog_color_b;
  BITCODE_BD fog_density_near;
  BITCODE_BD fog_density_far;
  BITCODE_BD fog_distance_near;
  BITCODE_BD fog_distance_far;
  BITCODE_B environ_image_enabled;
  BITCODE_T environ_image_filename;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_renderenvironment *_obj = dwg_object_to_RENDERENVIRONMENT (obj);

  CHK_ENTITY_TYPE (_obj, RENDERENVIRONMENT, class_version, BL);
  CHK_ENTITY_TYPE (_obj, RENDERENVIRONMENT, fog_enabled, B);
  CHK_ENTITY_TYPE (_obj, RENDERENVIRONMENT, fog_background_enabled, B);
  CHK_ENTITY_TYPE (_obj, RENDERENVIRONMENT, fog_color_r, RC);
  CHK_ENTITY_TYPE (_obj, RENDERENVIRONMENT, fog_color_g, RC);
  CHK_ENTITY_TYPE (_obj, RENDERENVIRONMENT, fog_color_b, RC);
  CHK_ENTITY_TYPE (_obj, RENDERENVIRONMENT, fog_density_near, BD);
  CHK_ENTITY_TYPE (_obj, RENDERENVIRONMENT, fog_density_far, BD);
  CHK_ENTITY_TYPE (_obj, RENDERENVIRONMENT, fog_distance_near, BD);
  CHK_ENTITY_TYPE (_obj, RENDERENVIRONMENT, fog_distance_far, BD);
  CHK_ENTITY_TYPE (_obj, RENDERENVIRONMENT, environ_image_enabled, B);
  CHK_ENTITY_UTF8TEXT (_obj, RENDERENVIRONMENT, environ_image_filename);
}
