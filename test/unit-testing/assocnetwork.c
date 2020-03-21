// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_ASSOCNETWORK
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  // ASSOCACTION
  BITCODE_BL solution_status;
  BITCODE_BL geometry_status;
  BITCODE_H readdep;
  BITCODE_H writedep;
  BITCODE_BL constraint_status;
  BITCODE_BL dof;
  BITCODE_B  is_body_a_proxy;

  BITCODE_BL unknown_assoc;
  BITCODE_BL unknown_n1;
  BITCODE_BL unknown_n2;
  BITCODE_BL num_actions;
  BITCODE_H* actions;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_assocnetwork *_obj = dwg_object_to_ASSOCNETWORK (obj);

  CHK_ENTITY_TYPE (_obj, ASSOCNETWORK, solution_status, BL, solution_status);
  CHK_ENTITY_MAX (_obj, ASSOCNETWORK, solution_status, BL, 6);
  CHK_ENTITY_TYPE (_obj, ASSOCNETWORK, geometry_status, BL, geometry_status);
  CHK_ENTITY_MAX (_obj, ASSOCNETWORK, geometry_status, BL, 10);
  CHK_ENTITY_H (_obj, ASSOCNETWORK, readdep, readdep);
  CHK_ENTITY_H (_obj, ASSOCNETWORK, writedep, writedep);
  CHK_ENTITY_TYPE (_obj, ASSOCNETWORK, constraint_status, BL, constraint_status);
  CHK_ENTITY_MAX (_obj, ASSOCNETWORK, constraint_status, BL, 10);
  CHK_ENTITY_TYPE (_obj, ASSOCNETWORK, dof, BL, dof);
  CHK_ENTITY_MAX (_obj, ASSOCNETWORK, dof, BL, 6);
  CHK_ENTITY_TYPE (_obj, ASSOCNETWORK, is_body_a_proxy, B, is_body_a_proxy);

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
