// stable
#define DWG_TYPE DWG_TYPE_ASSOCNETWORK
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  // ASSOCACTION
  BITCODE_BS class_version;
  BITCODE_BL geometry_status;
  BITCODE_H owningnetwork;
  BITCODE_H actionbody;
  BITCODE_BL action_index;
  BITCODE_BL max_assoc_dep_index;
  BITCODE_BL num_deps;
  BITCODE_BL num_owned_params;
  BITCODE_H *owned_params;
  BITCODE_BL num_values;
  Dwg_VALUEPARAM *values;
  // ASSOCNETWORK
  BITCODE_BS network_version;
  BITCODE_BL network_action_index;
  BITCODE_BL num_actions;
  Dwg_ASSOCACTION_Deps *actions;
  BITCODE_BL num_owned_actions;
  BITCODE_H *owned_actions;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_assocnetwork *_obj = dwg_object_to_ASSOCNETWORK (obj);

  // ASSOCACTION
  CHK_ENTITY_TYPE (_obj, ASSOCNETWORK, class_version, BS);
  CHK_ENTITY_TYPE (_obj, ASSOCNETWORK, geometry_status, BL);
  CHK_ENTITY_MAX (_obj, ASSOCNETWORK, geometry_status, BL, 10);
  CHK_ENTITY_H (_obj, ASSOCNETWORK, owningnetwork);
  CHK_ENTITY_H (_obj, ASSOCNETWORK, actionbody);
  CHK_ENTITY_TYPE (_obj, ASSOCNETWORK, action_index, BL);
  CHK_ENTITY_TYPE (_obj, ASSOCNETWORK, max_assoc_dep_index, BL);
  CHK_ENTITY_TYPE (_obj, ASSOCNETWORK, num_deps, BL);
  for (unsigned i = 0; i < num_deps; i++)
    {
      CHK_SUBCLASS_TYPE (_obj->deps[i], ASSOCACTION_Deps, is_owned, B);
      CHK_SUBCLASS_H (_obj->deps[i], ASSOCACTION_Deps, dep);
    }
  CHK_ENTITY_TYPE (_obj, ASSOCNETWORK, num_owned_params, BL);
  CHK_ENTITY_HV (_obj, ASSOCNETWORK, owned_params, num_owned_params);
  CHK_ENTITY_TYPE (_obj, ASSOCNETWORK, num_values, BL);
  CHK_VALUEPARAM (num_values, values);

  // ASSOCNETWORK
  CHK_ENTITY_TYPE (_obj, ASSOCNETWORK, network_version, BL);
  CHK_ENTITY_TYPE (_obj, ASSOCNETWORK, network_action_index, BL);
  CHK_ENTITY_TYPE (_obj, ASSOCNETWORK, num_actions, BL);
  for (unsigned i = 0; i < num_actions; i++)
    {
      CHK_SUBCLASS_TYPE (_obj->actions[i], ASSOCACTION_Deps, is_owned, B);
      CHK_SUBCLASS_H (_obj->actions[i], ASSOCACTION_Deps, dep);
    }
  CHK_ENTITY_TYPE (_obj, ASSOCNETWORK, num_owned_actions, BL);
  CHK_ENTITY_HV (_obj, ASSOCNETWORK, owned_actions, num_owned_actions);
}
