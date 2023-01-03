#define DWG_TYPE DWG_TYPE_BLOCKALIGNMENTGRIP
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BLOCKGRIP_fields;
  BITCODE_3BD orientation;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_blockalignmentgrip *_obj = dwg_object_to_BLOCKALIGNMENTGRIP (obj);

  CHK_EVALEXPR (BLOCKALIGNMENTGRIP);
  // AcDbBlockElement
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKALIGNMENTGRIP, name);
  CHK_ENTITY_TYPE (_obj, BLOCKALIGNMENTGRIP, be_major, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKALIGNMENTGRIP, be_minor, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKALIGNMENTGRIP, eed1071, BL);
  // AcDbBlockGrip
  CHK_ENTITY_TYPE (_obj, BLOCKALIGNMENTGRIP, bg_bl91, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKALIGNMENTGRIP, bg_bl92, BL);
  CHK_ENTITY_3RD (_obj, BLOCKALIGNMENTGRIP, bg_location);
  CHK_ENTITY_TYPE (_obj, BLOCKALIGNMENTGRIP, bg_insert_cycling, B);
  CHK_ENTITY_TYPE (_obj, BLOCKALIGNMENTGRIP, bg_insert_cycling_weight, BLd);
  // AcDbBlockAlignmentGrip
  CHK_ENTITY_3RD (_obj, BLOCKALIGNMENTGRIP, orientation);
}
