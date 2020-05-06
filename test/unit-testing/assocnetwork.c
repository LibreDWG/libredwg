// TODO DEBUGGING
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
  Dwg_ASSOCACTION_Deps *deps;
  BITCODE_BL num_owned_params;
  BITCODE_H *owned_params;
  BITCODE_BL num_owned_value_param_names;
  BITCODE_H *owned_value_param_names;

  BITCODE_BL unknown_assoc;
  BITCODE_BL unknown_n1;
  BITCODE_BL unknown_n2;
  BITCODE_BL num_actions;
  BITCODE_H* actions;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_assocnetwork *_obj = dwg_object_to_ASSOCNETWORK (obj);

  CHK_ENTITY_TYPE (_obj, ASSOCNETWORK, class_version, BS, class_version);
  CHK_ENTITY_TYPE (_obj, ASSOCNETWORK, geometry_status, BL, geometry_status);
  CHK_ENTITY_MAX (_obj, ASSOCNETWORK, geometry_status, BL, 10);
  CHK_ENTITY_H (_obj, ASSOCNETWORK, owningnetwork, owningnetwork);
  CHK_ENTITY_H (_obj, ASSOCNETWORK, actionbody, actionbody);
  CHK_ENTITY_TYPE (_obj, ASSOCNETWORK, action_index, BL, action_index);
  CHK_ENTITY_TYPE (_obj, ASSOCNETWORK, max_assoc_dep_index, BL, max_assoc_dep_index);
  CHK_ENTITY_TYPE (_obj, ASSOCNETWORK, num_deps, BL, num_deps);
  //Dwg_ASSOCACTION_Deps *deps;
  CHK_ENTITY_TYPE (_obj, ASSOCNETWORK, num_owned_params, BL, num_owned_params);
  CHK_ENTITY_HV (_obj, ASSOCNETWORK, owned_params, owned_params, num_owned_params);
  CHK_ENTITY_TYPE (_obj, ASSOCNETWORK, num_owned_value_param_names,
                   BL, num_owned_value_param_names);
  CHK_ENTITY_HV (_obj, ASSOCNETWORK, owned_value_param_names,
                 owned_value_param_names, num_owned_value_param_names);

  CHK_ENTITY_TYPE (_obj, ASSOCNETWORK, unknown_assoc, BL, unknown_assoc);
  CHK_ENTITY_TYPE (_obj, ASSOCNETWORK, unknown_n1, BL, unknown_n1);
  CHK_ENTITY_TYPE (_obj, ASSOCNETWORK, unknown_n2, BL, unknown_n2);
  CHK_ENTITY_TYPE (_obj, ASSOCNETWORK, num_actions, BL, num_actions);
  CHK_ENTITY_MAX (_obj, ASSOCNETWORK, num_actions, BL, 5000);
  if (!dwg_dynapi_entity_value (_obj, "ASSOCNETWORK", "actions", &actions, NULL))
    fail ("ASSOCNETWORK.actions");
  for (BITCODE_BL i = 0; i < num_actions; i++)
    {
      ok ("ASSOCNETWORK.actions[%d]: " FORMAT_REF, i, ARGS_REF (actions[i]));
    }
#endif
}
