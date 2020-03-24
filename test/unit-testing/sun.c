#define DWG_TYPE DWG_TYPE_SUN
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BL class_version;
  BITCODE_B is_on;
  BITCODE_BS unknown;
  BITCODE_CMC color;
  BITCODE_BD intensity;
  //BITCODE_3BD direction; //calculated?
  //BITCODE_BD altitude;   //calculated?
  //BITCODE_BD azimuth;    //calculated?
  BITCODE_BL julian_day;
  BITCODE_BL time;
  BITCODE_B  is_dst;
  BITCODE_B has_shadow;
  BITCODE_BS shadow_type;
  BITCODE_BS shadow_mapsize;
  BITCODE_BS shadow_softness;
  BITCODE_H skyparams;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_sun *_obj = dwg_object_to_SUN (obj);

  CHK_ENTITY_TYPE (_obj, SUN, class_version, BL, class_version);
  CHK_ENTITY_TYPE (_obj, SUN, is_on, B, is_on);
  CHK_ENTITY_TYPE (_obj, SUN, unknown, BS, unknown);
  CHK_ENTITY_CMC (_obj, SUN, color, color);
  CHK_ENTITY_TYPE (_obj, SUN, intensity, BD, intensity);
  CHK_ENTITY_TYPE (_obj, SUN, julian_day, BL, julian_day);
  CHK_ENTITY_TYPE (_obj, SUN, time, BL, time);
  CHK_ENTITY_TYPE (_obj, SUN, is_dst, B, is_dst);
  CHK_ENTITY_TYPE (_obj, SUN, has_shadow, B, has_shadow);
  CHK_ENTITY_TYPE (_obj, SUN, shadow_type, BS, shadow_type);
  CHK_ENTITY_TYPE (_obj, SUN, shadow_mapsize, BS, shadow_mapsize);
  CHK_ENTITY_TYPE (_obj, SUN, shadow_softness, BS, shadow_softness);
  CHK_ENTITY_H (_obj, SUN, skyparams, skyparams);
#endif
}
