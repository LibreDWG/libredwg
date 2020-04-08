// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_SECTION
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BL state;
  BITCODE_BL flags;
  BITCODE_T name;
  BITCODE_3BD vert_dir;
  BITCODE_BD top_height;
  BITCODE_BD bottom_height;
  BITCODE_RC indicator_alpha;
  BITCODE_CMC indicator_color;
  BITCODE_BL i, num_verts;
  BITCODE_3BD *verts;
  BITCODE_BL num_blverts;
  BITCODE_3BD *blverts;
  BITCODE_H geomsetting;

  dwg_ent_section *_obj = dwg_object_to_SECTION (obj);

#ifdef DEBUG_CLASSES
  CHK_ENTITY_TYPE (_obj, SECTION, state, BL, state);
  //CHK_ENTITY_MAX (_obj, SECTION, state, BL, 10;
  CHK_ENTITY_TYPE (_obj, SECTION, flags, BL, flags);
  //CHK_ENTITY_MAX (_obj, SECTION, flags, BL, 128);
  CHK_ENTITY_UTF8TEXT (_obj, SECTION, name, name);
  CHK_ENTITY_3RD (_obj, SECTION, vert_dir, vert_dir);
  CHK_ENTITY_TYPE (_obj, SECTION, top_height, BD, top_height);
  CHK_ENTITY_TYPE (_obj, SECTION, bottom_height, BD, bottom_height);
  CHK_ENTITY_TYPE (_obj, SECTION, indicator_alpha, RC, indicator_alpha);
  CHK_ENTITY_CMC (_obj, SECTION, indicator_color, indicator_color);
  CHK_ENTITY_TYPE (_obj, SECTION, num_verts, BL, num_verts);
  CHK_ENTITY_3DPOINTS (_obj, SECTION, verts, verts, num_verts);
  CHK_ENTITY_TYPE (_obj, SECTION, num_blverts, BL, num_blverts);
  CHK_ENTITY_3DPOINTS (_obj, SECTION, blverts, blverts, num_blverts);
  CHK_ENTITY_H (_obj, SECTION, geomsetting, geomsetting);
#endif
}
