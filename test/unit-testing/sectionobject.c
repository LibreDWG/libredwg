#define DWG_TYPE DWG_TYPE_SECTIONOBJECT
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
  BITCODE_BS indicator_alpha;
  BITCODE_CMC indicator_color;
  BITCODE_BL i, num_verts;
  BITCODE_3BD *verts;
  BITCODE_BL num_blverts;
  BITCODE_3BD *blverts;
  BITCODE_H section_settings;

  dwg_ent_sectionobject *_obj = dwg_object_to_SECTIONOBJECT (obj);

  CHK_ENTITY_TYPE (_obj, SECTIONOBJECT, state, BL);
  CHK_ENTITY_MAX (_obj, SECTIONOBJECT, state, BL, 4);
  CHK_ENTITY_TYPE (_obj, SECTIONOBJECT, flags, BL);
  CHK_ENTITY_MAX (_obj, SECTIONOBJECT, flags, BL, 256); //??
  CHK_ENTITY_UTF8TEXT (_obj, SECTIONOBJECT, name);
  CHK_ENTITY_3RD (_obj, SECTIONOBJECT, vert_dir);
  CHK_ENTITY_TYPE (_obj, SECTIONOBJECT, top_height, BD);
  CHK_ENTITY_TYPE (_obj, SECTIONOBJECT, bottom_height, BD);
  CHK_ENTITY_TYPE (_obj, SECTIONOBJECT, indicator_alpha, BS);
  CHK_ENTITY_CMC (_obj, SECTIONOBJECT, indicator_color);
  CHK_ENTITY_TYPE (_obj, SECTIONOBJECT, num_verts, BL);
  CHK_ENTITY_3DPOINTS (_obj, SECTIONOBJECT, verts, num_verts);
  CHK_ENTITY_TYPE (_obj, SECTIONOBJECT, num_blverts, BL);
  CHK_ENTITY_3DPOINTS (_obj, SECTIONOBJECT, blverts, num_blverts);
  CHK_ENTITY_H (_obj, SECTIONOBJECT, section_settings);
}
