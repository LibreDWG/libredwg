//unstable
#define DWG_TYPE DWG_TYPE_SUN
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_BL class_version;
  BITCODE_B is_on;
  BITCODE_CMC color;
  BITCODE_BD intensity;
  BITCODE_BL julian_day;
  BITCODE_BL msecs;
  BITCODE_B  is_dst;
  BITCODE_B has_shadow;
  BITCODE_BL shadow_type;
  BITCODE_BS shadow_mapsize;
  BITCODE_RC shadow_softness;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_sun *_obj = dwg_object_to_SUN (obj);

  CHK_ENTITY_TYPE (_obj, SUN, class_version, BL, class_version);
  CHK_ENTITY_TYPE (_obj, SUN, is_on, B, is_on);
  CHK_ENTITY_CMC (_obj, SUN, color, color);
  CHK_ENTITY_TYPE (_obj, SUN, intensity, BD, intensity);
  CHK_ENTITY_TYPE (_obj, SUN, julian_day, BL, julian_day);
  CHK_ENTITY_TYPE (_obj, SUN, msecs, BL, msecs);
  CHK_ENTITY_TYPE (_obj, SUN, is_dst, B, is_dst);
  CHK_ENTITY_TYPE (_obj, SUN, has_shadow, B, has_shadow);
  CHK_ENTITY_TYPE (_obj, SUN, shadow_type, BL, shadow_type);
  CHK_ENTITY_TYPE (_obj, SUN, shadow_mapsize, BS, shadow_mapsize);
  CHK_ENTITY_TYPE (_obj, SUN, shadow_softness, RC, shadow_softness);
}
