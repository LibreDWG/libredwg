// TODO coverage
#define DWG_TYPE DWG_TYPE_BLOCKPOLARGRIP
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BLOCKGRIP_fields;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_blockpolargrip *_obj = dwg_object_to_BLOCKPOLARGRIP (obj);

  CHK_EVALEXPR (BLOCKPOLARGRIP);
  // AcDbBlockElement
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKPOLARGRIP, name);
  CHK_ENTITY_TYPE (_obj, BLOCKPOLARGRIP, be_major, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKPOLARGRIP, be_minor, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKPOLARGRIP, eed1071, BL);
  // AcDbBlockGrip
  CHK_ENTITY_TYPE (_obj, BLOCKPOLARGRIP, bg_bl91, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKPOLARGRIP, bg_bl92, BL);
  CHK_ENTITY_3RD (_obj, BLOCKPOLARGRIP, bg_location);
  CHK_ENTITY_TYPE (_obj, BLOCKPOLARGRIP, bg_insert_cycling, B);
  CHK_ENTITY_TYPE (_obj, BLOCKPOLARGRIP, bg_insert_cycling_weight, BLd);
}
