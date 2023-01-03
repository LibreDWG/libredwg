#define DWG_TYPE DWG_TYPE_BLOCKLOOKUPGRIP
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BLOCKGRIP_fields;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_blocklookupgrip *_obj = dwg_object_to_BLOCKLOOKUPGRIP (obj);

  CHK_EVALEXPR (BLOCKLOOKUPGRIP);
  // AcDbBlockElement
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKLOOKUPGRIP, name);
  CHK_ENTITY_TYPE (_obj, BLOCKLOOKUPGRIP, be_major, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKLOOKUPGRIP, be_minor, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKLOOKUPGRIP, eed1071, BL);
  // AcDbBlockGrip
  CHK_ENTITY_TYPE (_obj, BLOCKLOOKUPGRIP, bg_bl91, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKLOOKUPGRIP, bg_bl92, BL);
  CHK_ENTITY_3RD (_obj, BLOCKLOOKUPGRIP, bg_location);
  CHK_ENTITY_TYPE (_obj, BLOCKLOOKUPGRIP, bg_insert_cycling, B);
  CHK_ENTITY_TYPE (_obj, BLOCKLOOKUPGRIP, bg_insert_cycling_weight, BLd);
}
