//debugging
#define DWG_TYPE DWG_TYPE_TVDEVICEPROPERTIES
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_BL flags;
  BITCODE_BS max_regen_threads;
  BITCODE_BL use_lut_palette;
  BITCODE_BLL alt_hlt;
  BITCODE_BLL alt_hltcolor;
  BITCODE_BLL geom_shader_usage;
  BITCODE_BL blending_mode;
  BITCODE_BD antialiasing_level;
  BITCODE_BD bd2;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_tvdeviceproperties *_obj = dwg_object_to_TVDEVICEPROPERTIES (obj);

  CHK_ENTITY_TYPE (_obj, TVDEVICEPROPERTIES, flags, BL, flags);
  CHK_ENTITY_TYPE (_obj, TVDEVICEPROPERTIES, max_regen_threads, BS, max_regen_threads);
  CHK_ENTITY_TYPE (_obj, TVDEVICEPROPERTIES, use_lut_palette, BL, use_lut_palette);
  CHK_ENTITY_TYPE (_obj, TVDEVICEPROPERTIES, alt_hlt, BLL, alt_hlt);
  CHK_ENTITY_TYPE (_obj, TVDEVICEPROPERTIES, alt_hltcolor, BLL, alt_hltcolor);
  CHK_ENTITY_TYPE (_obj, TVDEVICEPROPERTIES, geom_shader_usage, BLL, geom_shader_usage);
  CHK_ENTITY_TYPE (_obj, TVDEVICEPROPERTIES, blending_mode, BL, blending_mode);
  CHK_ENTITY_TYPE (_obj, TVDEVICEPROPERTIES, antialiasing_level, BD, antialiasing_level);
  CHK_ENTITY_TYPE (_obj, TVDEVICEPROPERTIES, bd2, BD, bd2);
#endif
}
