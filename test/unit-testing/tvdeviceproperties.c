// debugging
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

  CHK_ENTITY_TYPE (_obj, TVDEVICEPROPERTIES, flags, BL);
  CHK_ENTITY_TYPE (_obj, TVDEVICEPROPERTIES, max_regen_threads, BS);
  CHK_ENTITY_TYPE (_obj, TVDEVICEPROPERTIES, use_lut_palette, BL);
  CHK_ENTITY_TYPE (_obj, TVDEVICEPROPERTIES, alt_hlt, BLL);
  CHK_ENTITY_TYPE (_obj, TVDEVICEPROPERTIES, alt_hltcolor, BLL);
  CHK_ENTITY_TYPE (_obj, TVDEVICEPROPERTIES, geom_shader_usage, BLL);
  CHK_ENTITY_TYPE (_obj, TVDEVICEPROPERTIES, blending_mode, BL);
  CHK_ENTITY_TYPE (_obj, TVDEVICEPROPERTIES, antialiasing_level, BD);
  CHK_ENTITY_TYPE (_obj, TVDEVICEPROPERTIES, bd2, BD);
#endif
}
