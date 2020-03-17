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
  //BITCODE_3BD origin;
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

  CHK_ENTITY_TYPE (_obj, GEOMAPIMAGE, class_version, BL, class_version);
  CHK_ENTITY_3RD (_obj, GEOMAPIMAGE, pt0, pt0);
  CHK_ENTITY_2RD (_obj, GEOMAPIMAGE, size, size);
  CHK_ENTITY_TYPE (_obj, GEOMAPIMAGE, display_props, BS, display_props);
  CHK_ENTITY_TYPE (_obj, GEOMAPIMAGE, clipping, B, clipping);
  CHK_ENTITY_TYPE (_obj, GEOMAPIMAGE, brightness, RC, brightness);
  CHK_ENTITY_TYPE (_obj, GEOMAPIMAGE, contrast, RC, contrast);
  CHK_ENTITY_TYPE (_obj, GEOMAPIMAGE, fade, RC, fade);
  CHK_ENTITY_TYPE (_obj, GEOMAPIMAGE, rotation, BD, rotation);
  //CHK_ENTITY_3RD (_obj, GEOMAPIMAGE, origin, origin);
  CHK_ENTITY_TYPE (_obj, GEOMAPIMAGE, image_width, BD, image_width);
  CHK_ENTITY_TYPE (_obj, GEOMAPIMAGE, image_height, BD, image_height);
  CHK_ENTITY_UTF8TEXT (_obj, GEOMAPIMAGE, name, name);
  CHK_ENTITY_TYPE (_obj, GEOMAPIMAGE, image_file, BD, image_file);
  CHK_ENTITY_TYPE (_obj, GEOMAPIMAGE, image_visibility, BD, image_visibility);
  CHK_ENTITY_TYPE (_obj, GEOMAPIMAGE, transparency, BS, transparency);
  CHK_ENTITY_TYPE (_obj, GEOMAPIMAGE, height, BD, height);
  CHK_ENTITY_TYPE (_obj, GEOMAPIMAGE, width, BD, width);
  CHK_ENTITY_TYPE (_obj, GEOMAPIMAGE, show_rotation, B, show_rotation);
  CHK_ENTITY_TYPE (_obj, GEOMAPIMAGE, scale_factor, BD, scale_factor);
  CHK_ENTITY_TYPE (_obj, GEOMAPIMAGE, geoimage_brightness, BS, geoimage_brightness);
  CHK_ENTITY_TYPE (_obj, GEOMAPIMAGE, geoimage_contrast, BS, geoimage_contrast);
  CHK_ENTITY_TYPE (_obj, GEOMAPIMAGE, geoimage_fade, BS, geoimage_fade);
  CHK_ENTITY_TYPE (_obj, GEOMAPIMAGE, geoimage_position, BS, geoimage_position);
  CHK_ENTITY_TYPE (_obj, GEOMAPIMAGE, geoimage_width, BS, geoimage_width);
  CHK_ENTITY_TYPE (_obj, GEOMAPIMAGE, geoimage_height, BS, geoimage_height);
#endif
}
