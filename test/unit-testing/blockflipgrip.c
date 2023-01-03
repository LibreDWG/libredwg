#define DWG_TYPE DWG_TYPE_BLOCKFLIPGRIP
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BLOCKGRIP_fields;
  BITCODE_BL combined_state;
  BITCODE_3BD orientation;
  // BITCODE_BS upd_state;
  // BITCODE_BS state;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_blockflipgrip *_obj = dwg_object_to_BLOCKFLIPGRIP (obj);

  CHK_EVALEXPR (BLOCKFLIPGRIP);
  // AcDbBlockElement
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKFLIPGRIP, name);
  CHK_ENTITY_TYPE (_obj, BLOCKFLIPGRIP, be_major, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKFLIPGRIP, be_minor, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKFLIPGRIP, eed1071, BL);
  // AcDbBlockGrip
  CHK_ENTITY_TYPE (_obj, BLOCKFLIPGRIP, bg_bl91, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKFLIPGRIP, bg_bl92, BL);
  CHK_ENTITY_3RD (_obj, BLOCKFLIPGRIP, bg_location);
  CHK_ENTITY_TYPE (_obj, BLOCKFLIPGRIP, bg_insert_cycling, B);
  CHK_ENTITY_TYPE (_obj, BLOCKFLIPGRIP, bg_insert_cycling_weight, BLd);
  // AcDbBlockFlipGrip
  CHK_ENTITY_TYPE (_obj, BLOCKFLIPGRIP, combined_state, BL);
  CHK_ENTITY_3RD (_obj, BLOCKFLIPGRIP, orientation);
  // CHK_ENTITY_TYPE  (_obj, BLOCKFLIPGRIP, upd_state, BS);
  // CHK_ENTITY_TYPE  (_obj, BLOCKFLIPGRIP, state, BS);
}
