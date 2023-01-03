#define DWG_TYPE DWG_TYPE_BLOCKLINEARGRIP
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BLOCKGRIP_fields;
  BITCODE_3BD orientation;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_blocklineargrip *_obj = dwg_object_to_BLOCKLINEARGRIP (obj);

  CHK_EVALEXPR (BLOCKLINEARGRIP);
  // AcDbBlockElement
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKLINEARGRIP, name);
  CHK_ENTITY_TYPE (_obj, BLOCKLINEARGRIP, be_major, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKLINEARGRIP, be_minor, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKLINEARGRIP, eed1071, BL);
  // AcDbBlockGrip
  CHK_ENTITY_TYPE (_obj, BLOCKLINEARGRIP, bg_bl91, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKLINEARGRIP, bg_bl92, BL);
  CHK_ENTITY_3RD (_obj, BLOCKLINEARGRIP, bg_location);
  CHK_ENTITY_TYPE (_obj, BLOCKLINEARGRIP, bg_insert_cycling, B);
  CHK_ENTITY_TYPE (_obj, BLOCKLINEARGRIP, bg_insert_cycling_weight, BLd);
  // AcDbBlockLinearGrip
  CHK_ENTITY_3RD (_obj, BLOCKLINEARGRIP, orientation);
}
