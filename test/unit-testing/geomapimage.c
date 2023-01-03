// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_GEOMAPIMAGE
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BL class_version;
  BITCODE_3BD pt0;
  BITCODE_2RD size;
  BITCODE_BS display_props;
  BITCODE_B clipping;
  BITCODE_RC brightness;
  BITCODE_RC contrast;
  BITCODE_RC fade;
  BITCODE_BD rotation;
  // BITCODE_3BD origin;
  BITCODE_BD image_width;
  BITCODE_BD image_height;
  BITCODE_T name;
  BITCODE_BD image_file;
  BITCODE_BD image_visibility;
  BITCODE_BS transparency;
  BITCODE_BD height;
  BITCODE_BD width;
  BITCODE_B show_rotation;
  BITCODE_BD scale_factor;
  BITCODE_BS geoimage_brightness;
  BITCODE_BS geoimage_contrast;
  BITCODE_BS geoimage_fade;
  BITCODE_BS geoimage_position;
  BITCODE_BS geoimage_width;
  BITCODE_BS geoimage_height;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_geomapimage *_obj = dwg_object_to_GEOMAPIMAGE (obj);

  CHK_ENTITY_TYPE (_obj, GEOMAPIMAGE, class_version, BL);
  CHK_ENTITY_3RD (_obj, GEOMAPIMAGE, pt0);
  CHK_ENTITY_2RD (_obj, GEOMAPIMAGE, size);
  CHK_ENTITY_TYPE (_obj, GEOMAPIMAGE, display_props, BS);
  CHK_ENTITY_TYPE (_obj, GEOMAPIMAGE, clipping, B);
  CHK_ENTITY_TYPE (_obj, GEOMAPIMAGE, brightness, RC);
  CHK_ENTITY_TYPE (_obj, GEOMAPIMAGE, contrast, RC);
  CHK_ENTITY_TYPE (_obj, GEOMAPIMAGE, fade, RC);
  CHK_ENTITY_TYPE (_obj, GEOMAPIMAGE, rotation, BD);
  // CHK_ENTITY_3RD (_obj, GEOMAPIMAGE, origin);
  CHK_ENTITY_TYPE (_obj, GEOMAPIMAGE, image_width, BD);
  CHK_ENTITY_TYPE (_obj, GEOMAPIMAGE, image_height, BD);
  CHK_ENTITY_UTF8TEXT (_obj, GEOMAPIMAGE, name);
  CHK_ENTITY_TYPE (_obj, GEOMAPIMAGE, image_file, BD);
  CHK_ENTITY_TYPE (_obj, GEOMAPIMAGE, image_visibility, BD);
  CHK_ENTITY_TYPE (_obj, GEOMAPIMAGE, transparency, BS);
  CHK_ENTITY_TYPE (_obj, GEOMAPIMAGE, height, BD);
  CHK_ENTITY_TYPE (_obj, GEOMAPIMAGE, width, BD);
  CHK_ENTITY_TYPE (_obj, GEOMAPIMAGE, show_rotation, B);
  CHK_ENTITY_TYPE (_obj, GEOMAPIMAGE, scale_factor, BD);
  CHK_ENTITY_TYPE (_obj, GEOMAPIMAGE, geoimage_brightness, BS);
  CHK_ENTITY_TYPE (_obj, GEOMAPIMAGE, geoimage_contrast, BS);
  CHK_ENTITY_TYPE (_obj, GEOMAPIMAGE, geoimage_fade, BS);
  CHK_ENTITY_TYPE (_obj, GEOMAPIMAGE, geoimage_position, BS);
  CHK_ENTITY_TYPE (_obj, GEOMAPIMAGE, geoimage_width, BS);
  CHK_ENTITY_TYPE (_obj, GEOMAPIMAGE, geoimage_height, BS);
#endif
}
