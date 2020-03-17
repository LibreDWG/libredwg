// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_ASSOCNETWORK
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  // ASSOCACTION
  BITCODE_BL solution_status; /* 90 */
  BITCODE_BL geometry_status; /* 90 */
  BITCODE_H readdep;          /* 330 */
  BITCODE_H writedep;         /* 360 */
  BITCODE_BL constraint_status; /* 90 */
  BITCODE_BL dof;               /* 90 */
  BITCODE_B  is_body_a_proxy;    /* 90 */

  BITCODE_BL unknown_assoc; // 90
  BITCODE_BL unknown_n1; // 90 0
  BITCODE_BL unknown_n2; // 90 1
  BITCODE_BL num_actions;// 90 1
  BITCODE_H* actions;    // 360

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_assocnetwork *_obj = dwg_object_to_ASSOCNETWORK (obj);

  CHK_ENTITY_TYPE (_obj, ASSOCNETWORK, solution_status, BL, solution_status); /* 90 */
  CHK_ENTITY_TYPE (_obj, ASSOCNETWORK, geometry_status, BL, geometry_status); /* 90 */
  CHK_ENTITY_H (_obj, ASSOCNETWORK, readdep, readdep);           /* 330 */
  CHK_ENTITY_H (_obj, ASSOCNETWORK, writedep, writedep);         /* 360 */
  CHK_ENTITY_TYPE (_obj, ASSOCNETWORK, constraint_status, BL, constraint_status); /* 90 */
  CHK_ENTITY_TYPE (_obj, ASSOCNETWORK, dof, BL, dof);               /* 90 */
  CHK_ENTITY_TYPE (_obj, ASSOCNETWORK, is_body_a_proxy, B, is_body_a_proxy);    /* 90 */

  CHK_ENTITY_TYPE (_obj, ASSOCNETWORK, unknown_assoc, BL, unknown_assoc);
  CHK_ENTITY_TYPE (_obj, ASSOCNETWORK, unknown_n1, BL, unknown_n1);
  CHK_ENTITY_TYPE (_obj, ASSOCNETWORK, unknown_n2, BL, unknown_n2);
  CHK_ENTITY_TYPE (_obj, ASSOCNETWORK, num_actions, BL, num_actions);
  if (!dwg_dynapi_entity_value (_obj, "ASSOCNETWORK", "actions", &actions, NULL))
    fail ("ASSOCNETWORK.actions");
  for (BITCODE_BL i = 0; i < num_actions; i++)
    {
      ok ("ASSOCNETWORK.actions[%d]: " FORMAT_REF, i, ARGS_REF (actions[i]));
    }
#endif
}
