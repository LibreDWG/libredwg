#define DWG_TYPE DWG_TYPE_ASSOCROTATEDDIMACTIONBODY
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  ASSOCANNOTATIONACTIONBODY_fields;
  Dwg_ASSOCPARAMBASEDACTIONBODY pab;
  BITCODE_H *deps;
  BITCODE_BS class_version;
  BITCODE_H d_node;
  BITCODE_H r_node;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_assocrotateddimactionbody *_obj
      = dwg_object_to_ASSOCROTATEDDIMACTIONBODY (obj);

  // ASSOCANNOTATIONACTIONBODY
  CHK_ENTITY_TYPE (_obj, ASSOCROTATEDDIMACTIONBODY, aaab_version, BS);
  CHK_ENTITY_MAX (_obj, ASSOCROTATEDDIMACTIONBODY, aaab_version, BS, 2);
  CHK_ENTITY_H (_obj, ASSOCROTATEDDIMACTIONBODY, assoc_dep);
  CHK_ENTITY_H (_obj, ASSOCROTATEDDIMACTIONBODY, actionbody);

  UNTIL (R_2010)
  {
    if (actionbody)
      {
        CHK_ASSOCPARAMBASEDACTIONBODY (ASSOCROTATEDDIMACTIONBODY);
      }
  }

  CHK_ENTITY_TYPE (_obj, ASSOCROTATEDDIMACTIONBODY, class_version, BS);
  CHK_ENTITY_H (_obj, ASSOCROTATEDDIMACTIONBODY, d_node);
  CHK_ENTITY_H (_obj, ASSOCROTATEDDIMACTIONBODY, r_node);
}
