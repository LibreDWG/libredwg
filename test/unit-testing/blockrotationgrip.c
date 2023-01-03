#define DWG_TYPE DWG_TYPE_BLOCKROTATIONGRIP
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BLOCKGRIP_fields;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_blockrotationgrip *_obj = dwg_object_to_BLOCKROTATIONGRIP (obj);

  CHK_EVALEXPR (BLOCKROTATIONGRIP);
  // AcDbBlockElement
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKROTATIONGRIP, name);
  CHK_ENTITY_TYPE (_obj, BLOCKROTATIONGRIP, be_major, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKROTATIONGRIP, be_minor, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKROTATIONGRIP, eed1071, BL);
  // AcDbBlockGrip
  CHK_ENTITY_TYPE (_obj, BLOCKROTATIONGRIP, bg_bl91, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKROTATIONGRIP, bg_bl92, BL);
  CHK_ENTITY_3RD (_obj, BLOCKROTATIONGRIP, bg_location);
  CHK_ENTITY_TYPE (_obj, BLOCKROTATIONGRIP, bg_insert_cycling, B);
  CHK_ENTITY_TYPE (_obj, BLOCKROTATIONGRIP, bg_insert_cycling_weight, BLd);
}
