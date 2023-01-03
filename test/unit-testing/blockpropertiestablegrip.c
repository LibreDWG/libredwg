// TODO coverage
#define DWG_TYPE DWG_TYPE_BLOCKPROPERTIESTABLEGRIP
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BLOCKGRIP_fields;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_blockpropertiestablegrip *_obj
      = dwg_object_to_BLOCKPROPERTIESTABLEGRIP (obj);

  CHK_EVALEXPR (BLOCKPROPERTIESTABLEGRIP);
  // AcDbBlockElement
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKPROPERTIESTABLEGRIP, name);
  CHK_ENTITY_TYPE (_obj, BLOCKPROPERTIESTABLEGRIP, be_major, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKPROPERTIESTABLEGRIP, be_minor, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKPROPERTIESTABLEGRIP, eed1071, BL);
  // AcDbBlockGrip
  CHK_ENTITY_TYPE (_obj, BLOCKPROPERTIESTABLEGRIP, bg_bl91, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKPROPERTIESTABLEGRIP, bg_bl92, BL);
  CHK_ENTITY_3RD (_obj, BLOCKPROPERTIESTABLEGRIP, bg_location);
  CHK_ENTITY_TYPE (_obj, BLOCKPROPERTIESTABLEGRIP, bg_insert_cycling, B);
  CHK_ENTITY_TYPE (_obj, BLOCKPROPERTIESTABLEGRIP, bg_insert_cycling_weight,
                   BLd);
#endif
}
