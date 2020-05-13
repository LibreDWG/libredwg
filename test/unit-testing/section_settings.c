// DEBUGGING
#define DWG_TYPE DWG_TYPE_SECTION_SETTINGS
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BS type;
  BITCODE_BS geometry;
  BITCODE_BS generation;
  BITCODE_BL num_sources;
  BITCODE_H *sources;
  BITCODE_H destblock;
  BITCODE_T destfile;
  BITCODE_B visibility;
  BITCODE_CMC color;
  BITCODE_H layer;
  BITCODE_H ltype;
  BITCODE_BD ltype_scale;
  BITCODE_H plotstyle;
  BITCODE_RC linewt;
  BITCODE_BS face_transparency;
  BITCODE_BS edge_transparency;
  BITCODE_B hatch_visibility;
  BITCODE_H hatch_pattern;
  BITCODE_BD hatch_angle;
  BITCODE_BD hatch_spacing;
  BITCODE_BD hatch_scale;
  BITCODE_B hidden_line;
  BITCODE_B division_lines;

#ifdef DEBUG_CLASSES
  dwg_obj_section_settings *_obj = dwg_object_to_SECTION_SETTINGS (obj);

  CHK_ENTITY_TYPE (_obj, SECTION_SETTINGS, type, BS);
  CHK_ENTITY_MAX (_obj, SECTION_SETTINGS, type, BS, 8);
  CHK_ENTITY_TYPE (_obj, SECTION_SETTINGS, num_sources, BL);
  CHK_ENTITY_MAX (_obj, SECTION_SETTINGS, num_sources, BL, 2000);
  CHK_ENTITY_HV (_obj, SECTION_SETTINGS, sources, sources, num_sources);
  // ...
#endif
}
