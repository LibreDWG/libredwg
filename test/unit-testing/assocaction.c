// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_ASSOCACTION
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

  BITCODE_BL num_deps;    /* 90 */
  //BITCODE_B is_actionevaluation_in_progress; // 90
  Dwg_ACTIONBODY body;
  BITCODE_BL status;
  BITCODE_H  actionbody;
  BITCODE_H  callback;
  BITCODE_H  owningnetwork;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_assocaction *_obj = dwg_object_to_ASSOCACTION (obj);

  CHK_ENTITY_TYPE (_obj, ASSOCACTION, solution_status, BL, solution_status); /* 90 */
  CHK_ENTITY_TYPE (_obj, ASSOCACTION, geometry_status, BL, geometry_status); /* 90 */
  CHK_ENTITY_H (_obj, ASSOCACTION, readdep, readdep);           /* 330 */
  CHK_ENTITY_H (_obj, ASSOCACTION, writedep, writedep);         /* 360 */
  CHK_ENTITY_TYPE (_obj, ASSOCACTION, constraint_status, BL, constraint_status); /* 90 */
  CHK_ENTITY_TYPE (_obj, ASSOCACTION, dof, BL, dof);               /* 90 */
  CHK_ENTITY_TYPE (_obj, ASSOCACTION, is_body_a_proxy, B, is_body_a_proxy);    /* 90 */

  CHK_ENTITY_TYPE (_obj, ASSOCACTION, num_deps, BL, num_deps);    /* 90 */
  //CHK_ENTITY_TYPE (_obj, ASSOCACTION, is_actionevaluation_in_progress, B, is_actionevaluation_in_progress); // 90
  CHK_SUBCLASS_TYPE (_obj->body, ACTIONBODY, evaluatorid, TV);
  CHK_SUBCLASS_TYPE (_obj->body, ACTIONBODY, expression, TV);
  CHK_SUBCLASS_TYPE (_obj->body, ACTIONBODY, value, BL);
  CHK_ENTITY_TYPE (_obj, ASSOCACTION, status, BL, status);
  CHK_ENTITY_H (_obj, ASSOCACTION, actionbody, actionbody);
  CHK_ENTITY_H (_obj, ASSOCACTION, callback, callback);
  CHK_ENTITY_H (_obj, ASSOCACTION, owningnetwork, owningnetwork);
#endif
}
