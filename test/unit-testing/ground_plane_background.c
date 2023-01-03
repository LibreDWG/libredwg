// unstable
#define DWG_TYPE DWG_TYPE_GROUND_PLANE_BACKGROUND
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error = 0, isnew;
  BITCODE_BL class_version;
  BITCODE_BLx color_sky_zenith;
  BITCODE_BLx color_sky_horizon;
  BITCODE_BLx color_underground_horizon;
  BITCODE_BLx color_underground_azimuth;
  BITCODE_BLx color_near;
  BITCODE_BLx color_far;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_ground_plane_background *_obj
      = dwg_object_to_GROUND_PLANE_BACKGROUND (obj);

  CHK_ENTITY_TYPE (_obj, GROUND_PLANE_BACKGROUND, class_version, BL);
  CHK_ENTITY_TYPE (_obj, GROUND_PLANE_BACKGROUND, color_sky_zenith, BLx);
  CHK_ENTITY_TYPE (_obj, GROUND_PLANE_BACKGROUND, color_sky_horizon, BLx);
  CHK_ENTITY_TYPE (_obj, GROUND_PLANE_BACKGROUND, color_underground_horizon,
                   BLx);
  CHK_ENTITY_TYPE (_obj, GROUND_PLANE_BACKGROUND, color_underground_azimuth,
                   BLx);
  CHK_ENTITY_TYPE (_obj, GROUND_PLANE_BACKGROUND, color_near, BLx);
  CHK_ENTITY_TYPE (_obj, GROUND_PLANE_BACKGROUND, color_far, BLx);
}
