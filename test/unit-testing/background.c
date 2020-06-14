//DEBUGGING
#define DWG_TYPE DWG_TYPE_BACKGROUND
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error = 0, isnew;
  BITCODE_BL class_version;
  BITCODE_RC type;
  Dwg_BACKGROUND_Sky sky;			// 1
  Dwg_BACKGROUND_Image image;			// 2
  Dwg_BACKGROUND_Solid solid;			// 3
  Dwg_BACKGROUND_IBL ibl;			// 4
  Dwg_BACKGROUND_GroundPlane groundplane;	// 5
  Dwg_BACKGROUND_Gradient gradient; 		// 6
  
  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_background *_obj = dwg_object_to_BACKGROUND (obj);

  CHK_ENTITY_TYPE (_obj, BACKGROUND, class_version, BL);
  CHK_ENTITY_TYPE (_obj, BACKGROUND, type, RC);
  switch (type)
    {
    case 1:
      CHK_SUBCLASS_H (_obj->u.sky, BACKGROUND_Sky, sunid);
      break;
    case 2:
      CHK_SUBCLASS_UTF8TEXT (_obj->u.image, BACKGROUND_Image, filename);
      CHK_SUBCLASS_TYPE (_obj->u.image, BACKGROUND_Image, fit_to_screen, B);
      CHK_SUBCLASS_TYPE (_obj->u.image, BACKGROUND_Image, maintain_aspect_ratio, B);
      CHK_SUBCLASS_TYPE (_obj->u.image, BACKGROUND_Image, use_tiling, B);
      CHK_SUBCLASS_2RD (_obj->u.image, BACKGROUND_Image, offset);
      CHK_SUBCLASS_2RD (_obj->u.image, BACKGROUND_Image, scale);
      break;
    case 3:
      CHK_SUBCLASS_TYPE (_obj->u.solid, BACKGROUND_Solid, color, BLx);
      break;
    case 4:
      CHK_SUBCLASS_TYPE (_obj->u.ibl, BACKGROUND_IBL, enable, B);
      CHK_SUBCLASS_UTF8TEXT (_obj->u.ibl, BACKGROUND_IBL, name);
      CHK_SUBCLASS_TYPE (_obj->u.ibl, BACKGROUND_IBL, rotation, BD); // in degree, not radian
      CHK_SUBCLASS_TYPE (_obj->u.ibl, BACKGROUND_IBL, display_image, B);
      CHK_SUBCLASS_H (_obj->u.ibl, BACKGROUND_IBL, secondary_background);
      break;
    case 5:
      CHK_SUBCLASS_TYPE (_obj->u.ground_plane, BACKGROUND_GroundPlane, color_sky_zenith, BLx);
      CHK_SUBCLASS_TYPE (_obj->u.ground_plane, BACKGROUND_GroundPlane, color_sky_horizon, BLx);
      CHK_SUBCLASS_TYPE (_obj->u.ground_plane, BACKGROUND_GroundPlane, color_underground_horizon, BLx);
      CHK_SUBCLASS_TYPE (_obj->u.ground_plane, BACKGROUND_GroundPlane, color_underground_azimuth, BLx);
      CHK_SUBCLASS_TYPE (_obj->u.ground_plane, BACKGROUND_GroundPlane, color_near, BLx);
      CHK_SUBCLASS_TYPE (_obj->u.ground_plane, BACKGROUND_GroundPlane, color_far, BLx);
      break;
    case 6:
      CHK_SUBCLASS_TYPE (_obj->u.gradient, BACKGROUND_Gradient, color_top, BLx);
      CHK_SUBCLASS_TYPE (_obj->u.gradient, BACKGROUND_Gradient, color_middle, BLx);
      CHK_SUBCLASS_TYPE (_obj->u.gradient, BACKGROUND_Gradient, color_bottom, BLx);
      CHK_SUBCLASS_TYPE (_obj->u.gradient, BACKGROUND_Gradient, horizon, BD);
      CHK_SUBCLASS_TYPE (_obj->u.gradient, BACKGROUND_Gradient, height, BD);
      CHK_SUBCLASS_TYPE (_obj->u.gradient, BACKGROUND_Gradient, rotation, BD);
      break;
    default:
      break;
    }
}
