// TODO coverage
#define DWG_TYPE DWG_TYPE_BLOCKXYGRIP
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BLOCKGRIP_fields;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  // #ifdef DEBUG_CLASSES
  dwg_obj_blockxygrip *_obj = dwg_object_to_BLOCKXYGRIP (obj);

  CHK_EVALEXPR (BLOCKXYGRIP);
  // AcDbBlockElement
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKXYGRIP, name);
  CHK_ENTITY_TYPE (_obj, BLOCKXYGRIP, be_major, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKXYGRIP, be_minor, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKXYGRIP, eed1071, BL);
  // AcDbBlockGrip
  CHK_ENTITY_TYPE (_obj, BLOCKXYGRIP, bg_bl91, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKXYGRIP, bg_bl92, BL);
  CHK_ENTITY_3RD (_obj, BLOCKXYGRIP, bg_location);
  CHK_ENTITY_TYPE (_obj, BLOCKXYGRIP, bg_insert_cycling, B);
  CHK_ENTITY_TYPE (_obj, BLOCKXYGRIP, bg_insert_cycling_weight, BLd);
  // #endif
}
