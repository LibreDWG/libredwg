#define DWG_TYPE DWG_TYPE_ASSOCRESTOREENTITYSTATEACTIONBODY
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  ASSOCACTIONBODY_fields;
  BITCODE_BL class_version;
  BITCODE_H entity;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_assocrestoreentitystateactionbody *_obj
      = dwg_object_to_ASSOCRESTOREENTITYSTATEACTIONBODY (obj);

  // ASSOCACTIONBODY
  CHK_ENTITY_TYPE (_obj, ASSOCRESTOREENTITYSTATEACTIONBODY, aab_version, BS);

  CHK_ENTITY_TYPE (_obj, ASSOCRESTOREENTITYSTATEACTIONBODY, class_version, BL);
  CHK_ENTITY_H (_obj, ASSOCRESTOREENTITYSTATEACTIONBODY, entity);
}
