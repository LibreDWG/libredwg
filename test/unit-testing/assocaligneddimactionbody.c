// crashes
#define DWG_TYPE DWG_TYPE_ASSOCALIGNEDDIMACTIONBODY
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
  dwg_obj_assocaligneddimactionbody *_obj
      = dwg_object_to_ASSOCALIGNEDDIMACTIONBODY (obj);

  // ASSOCANNOTATIONACTIONBODY
  CHK_ENTITY_TYPE (_obj, ASSOCALIGNEDDIMACTIONBODY, aaab_version, BS);
  CHK_ENTITY_MAX (_obj, ASSOCALIGNEDDIMACTIONBODY, aaab_version, BS, 2);
  CHK_ENTITY_H (_obj, ASSOCALIGNEDDIMACTIONBODY, assoc_dep);
  CHK_ENTITY_H (_obj, ASSOCALIGNEDDIMACTIONBODY, actionbody);

  UNTIL (R_2010)
  {
    if (actionbody)
      {
        CHK_ASSOCPARAMBASEDACTIONBODY (ASSOCALIGNEDDIMACTIONBODY);
      }
  }

  CHK_ENTITY_TYPE (_obj, ASSOCALIGNEDDIMACTIONBODY, class_version, BS);
  CHK_ENTITY_H (_obj, ASSOCALIGNEDDIMACTIONBODY, d_node);
  CHK_ENTITY_H (_obj, ASSOCALIGNEDDIMACTIONBODY, r_node);
}
