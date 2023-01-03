#define DWG_TYPE DWG_TYPE_ASSOCMLEADERACTIONBODY
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  ASSOCANNOTATIONACTIONBODY_fields;
  Dwg_ASSOCPARAMBASEDACTIONBODY pab;
  BITCODE_H *deps;
  BITCODE_BL class_version;
  BITCODE_BL num_actions;
  Dwg_ASSOCACTIONBODY_action *actions;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_assocmleaderactionbody *_obj
      = dwg_object_to_ASSOCMLEADERACTIONBODY (obj);

  // ASSOCANNOTATIONACTIONBODY
  CHK_ENTITY_TYPE (_obj, , aaab_version, BS);
  CHK_ENTITY_MAX (_obj, ASSOCMLEADERACTIONBODY, aaab_version, BS, 2);
  CHK_ENTITY_H (_obj, ASSOCMLEADERACTIONBODY, assoc_dep);
  CHK_ENTITY_H (_obj, ASSOCMLEADERACTIONBODY, actionbody);

  UNTIL (R_2010)
  {
    if (actionbody)
      {
        CHK_ASSOCPARAMBASEDACTIONBODY (ASSOCMLEADERACTIONBODY);
      }
  }

  CHK_ENTITY_TYPE (_obj, ASSOCMLEADERACTIONBODY, class_version, BL);
  CHK_ENTITY_TYPE (_obj, ASSOCMLEADERACTIONBODY, num_actions, BL);
  for (unsigned i = 0; i < num_actions; i++)
    {
      CHK_SUBCLASS_TYPE (_obj->actions[i], ASSOCACTIONBODY_action, depid, BL);
      CHK_SUBCLASS_H (_obj->actions[i], ASSOCACTIONBODY_action, dep);
    }
}
