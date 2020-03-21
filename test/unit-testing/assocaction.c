// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_ASSOCACTION
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BL solution_status;
  BITCODE_BL geometry_status;
  BITCODE_H readdep;
  BITCODE_H writedep;
  BITCODE_BL constraint_status;
  BITCODE_BL dof;
  BITCODE_B  is_body_a_proxy;

  BITCODE_BL num_deps;
  //BITCODE_B is_actionevaluation_in_progress;
  Dwg_ACTIONBODY body;
  BITCODE_BL status;
  BITCODE_H  actionbody;
  BITCODE_H  callback;
  BITCODE_H  owningnetwork;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_assocaction *_obj = dwg_object_to_ASSOCACTION (obj);

  CHK_ENTITY_TYPE (_obj, ASSOCACTION, solution_status, BL, solution_status);
  CHK_ENTITY_MAX (_obj, ASSOCACTION, solution_status, BL, 6);
  CHK_ENTITY_TYPE (_obj, ASSOCACTION, geometry_status, BL, geometry_status);
  CHK_ENTITY_MAX (_obj, ASSOCACTION, geometry_status, BL, 10);
  CHK_ENTITY_H (_obj, ASSOCACTION, readdep, readdep);
  CHK_ENTITY_H (_obj, ASSOCACTION, writedep, writedep);
  CHK_ENTITY_TYPE (_obj, ASSOCACTION, constraint_status, BL, constraint_status);
  CHK_ENTITY_MAX (_obj, ASSOCACTION, constraint_status, BL, 10);
  CHK_ENTITY_TYPE (_obj, ASSOCACTION, dof, BL, dof);
  CHK_ENTITY_MAX (_obj, ASSOCACTION, dof, BL, 10);
  CHK_ENTITY_TYPE (_obj, ASSOCACTION, is_body_a_proxy, B, is_body_a_proxy);

  CHK_ENTITY_TYPE (_obj, ASSOCACTION, num_deps, BL, num_deps);
  //CHK_ENTITY_TYPE (_obj, ASSOCACTION, is_actionevaluation_in_progress, B, is_actionevaluation_in_progress);
  CHK_SUBCLASS_UTF8TEXT (_obj->body, ACTIONBODY, evaluatorid);
  CHK_SUBCLASS_UTF8TEXT (_obj->body, ACTIONBODY, expression);
  CHK_SUBCLASS_TYPE (_obj->body, ACTIONBODY, value, BL);
  CHK_ENTITY_TYPE (_obj, ASSOCACTION, status, BL, status);
  CHK_ENTITY_H (_obj, ASSOCACTION, actionbody, actionbody);
  CHK_ENTITY_H (_obj, ASSOCACTION, callback, callback);
  CHK_ENTITY_H (_obj, ASSOCACTION, owningnetwork, owningnetwork);
#endif
}
