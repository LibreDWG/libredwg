// looks stable
#define DWG_TYPE DWG_TYPE_BLOCKVISIBILITYGRIP
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BLOCKGRIP_fields;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_blockvisibilitygrip *_obj = dwg_object_to_BLOCKVISIBILITYGRIP (obj);

  CHK_EVALEXPR (BLOCKVISIBILITYGRIP);
  // AcDbBlockElement
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKVISIBILITYGRIP, name);
  CHK_ENTITY_TYPE (_obj, BLOCKVISIBILITYGRIP, be_major, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKVISIBILITYGRIP, be_minor, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKVISIBILITYGRIP, eed1071, BL);
  // AcDbBlockGrip
  CHK_ENTITY_TYPE (_obj, BLOCKVISIBILITYGRIP, bg_bl91, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKVISIBILITYGRIP, bg_bl92, BL);
  CHK_ENTITY_3RD (_obj, BLOCKVISIBILITYGRIP, bg_location);
  CHK_ENTITY_TYPE (_obj, BLOCKVISIBILITYGRIP, bg_insert_cycling, B);
  CHK_ENTITY_TYPE (_obj, BLOCKVISIBILITYGRIP, bg_insert_cycling_weight, BLd);
}
