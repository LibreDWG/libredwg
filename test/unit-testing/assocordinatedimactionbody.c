#define DWG_TYPE DWG_TYPE_ASSOCORDINATEDIMACTIONBODY
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  ASSOCANNOTATIONACTIONBODY_fields;
  Dwg_ASSOCPARAMBASEDACTIONBODY pab;
  BITCODE_H *deps;
  BITCODE_BL class_version;
  BITCODE_H d_node;
  BITCODE_H r_node;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_assocordinatedimactionbody *_obj
      = dwg_object_to_ASSOCORDINATEDIMACTIONBODY (obj);

  // ASSOCANNOTATIONACTIONBODY
  CHK_ENTITY_TYPE (_obj, ASSOCORDINATEDIMACTIONBODY, aaab_version, BS);
  CHK_ENTITY_MAX (_obj, ASSOCORDINATEDIMACTIONBODY, aaab_version, BS, 2);
  CHK_ENTITY_H (_obj, ASSOCORDINATEDIMACTIONBODY, assoc_dep);
  CHK_ENTITY_H (_obj, ASSOCORDINATEDIMACTIONBODY, actionbody);

  UNTIL (R_2010)
  {
    if (actionbody)
      {
        CHK_ASSOCPARAMBASEDACTIONBODY (ASSOCORDINATEDIMACTIONBODY);
      }
  }

  CHK_ENTITY_TYPE (_obj, ASSOCORDINATEDIMACTIONBODY, class_version, BL);
  CHK_ENTITY_H (_obj, ASSOCORDINATEDIMACTIONBODY, d_node);
  CHK_ENTITY_H (_obj, ASSOCORDINATEDIMACTIONBODY, r_node);
}
