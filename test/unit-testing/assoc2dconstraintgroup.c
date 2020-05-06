// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_ASSOC2DCONSTRAINTGROUP
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
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

  BITCODE_BL l5; //90 1
  BITCODE_B  b1; //70 0
  BITCODE_3BD workplane[3]; //3x10 workplane
  BITCODE_H h1; // 360
  BITCODE_BL num_actions; //90 2
  BITCODE_H* actions;    // 360
  BITCODE_BL l7; //90 9
  BITCODE_BL l8; //90 9
  BITCODE_T t1; // AcConstrainedCircle
  BITCODE_H h2; // 330
  BITCODE_BL cl1; //90 1
  BITCODE_RC cs1; //70 1
  BITCODE_BL cl2; //90 1
  BITCODE_BL cl3; //90 3
  BITCODE_H h3; // 330
  BITCODE_BL cl4; //90 0
  BITCODE_3BD c1; //10 @134
  BITCODE_3BD c2; //10
  BITCODE_3BD c3; //10
  BITCODE_BD w1; //40
  BITCODE_BD w2; //40
  BITCODE_BD w3; //40
  BITCODE_T t2; // AcConstrainedImplicitPoint

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_assoc2dconstraintgroup *_obj = dwg_object_to_ASSOC2DCONSTRAINTGROUP (obj);

  CHK_ENTITY_TYPE (_obj, ASSOC2DCONSTRAINTGROUP, class_version, BS, class_version);
  CHK_ENTITY_TYPE (_obj, ASSOC2DCONSTRAINTGROUP, geometry_status, BL, geometry_status);
  CHK_ENTITY_MAX (_obj, ASSOC2DCONSTRAINTGROUP, geometry_status, BL, 10);
  CHK_ENTITY_H (_obj, ASSOC2DCONSTRAINTGROUP, owningnetwork, owningnetwork);
  CHK_ENTITY_H (_obj, ASSOC2DCONSTRAINTGROUP, actionbody, actionbody);
  CHK_ENTITY_TYPE (_obj, ASSOC2DCONSTRAINTGROUP, action_index, BL, action_index);
  CHK_ENTITY_TYPE (_obj, ASSOC2DCONSTRAINTGROUP, max_assoc_dep_index, BL, max_assoc_dep_index);
  CHK_ENTITY_TYPE (_obj, ASSOC2DCONSTRAINTGROUP, num_deps, BL, num_deps);
  //Dwg_ASSOCACTION_Deps *deps;
  CHK_ENTITY_TYPE (_obj, ASSOC2DCONSTRAINTGROUP, num_owned_params, BL, num_owned_params);
  CHK_ENTITY_HV (_obj, ASSOC2DCONSTRAINTGROUP, owned_params, owned_params, num_owned_params);
  CHK_ENTITY_TYPE (_obj, ASSOC2DCONSTRAINTGROUP, num_owned_value_param_names,
                   BL, num_owned_value_param_names);
  CHK_ENTITY_HV (_obj, ASSOC2DCONSTRAINTGROUP, owned_value_param_names,
                 owned_value_param_names, num_owned_value_param_names);

  CHK_ENTITY_TYPE (_obj, ASSOC2DCONSTRAINTGROUP, l5, BL, l5);
  CHK_ENTITY_TYPE (_obj, ASSOC2DCONSTRAINTGROUP, b1, B , b1);
  //CHK_ENTITY_3RD (_obj, ASSOC2DCONSTRAINTGROUP, workplane[3], workplane[3]); //3x10 workplane
  CHK_ENTITY_H (_obj, ASSOC2DCONSTRAINTGROUP, h1, h1);
  CHK_ENTITY_TYPE (_obj, ASSOC2DCONSTRAINTGROUP, num_actions, BL, num_actions);
  CHK_ENTITY_HV (_obj, ASSOC2DCONSTRAINTGROUP, actions, actions, num_actions);
  CHK_ENTITY_TYPE (_obj, ASSOC2DCONSTRAINTGROUP, l7, BL, l7);
  CHK_ENTITY_TYPE (_obj, ASSOC2DCONSTRAINTGROUP, l8, BL, l8);
  CHK_ENTITY_UTF8TEXT (_obj, ASSOC2DCONSTRAINTGROUP, t1, t1);
  CHK_ENTITY_H (_obj, ASSOC2DCONSTRAINTGROUP, h2, h2);
  CHK_ENTITY_TYPE (_obj, ASSOC2DCONSTRAINTGROUP, cl1, BL, cl1);
  CHK_ENTITY_TYPE (_obj, ASSOC2DCONSTRAINTGROUP, cs1, RC, cs1);
  CHK_ENTITY_TYPE (_obj, ASSOC2DCONSTRAINTGROUP, cl2, BL, cl2);
  CHK_ENTITY_TYPE (_obj, ASSOC2DCONSTRAINTGROUP, cl3, BL, cl3);
  CHK_ENTITY_H (_obj, ASSOC2DCONSTRAINTGROUP, h3, h3);
  CHK_ENTITY_TYPE (_obj, ASSOC2DCONSTRAINTGROUP, cl4, BL, cl4);
  CHK_ENTITY_3RD (_obj, ASSOC2DCONSTRAINTGROUP, c1, c1);
  CHK_ENTITY_3RD (_obj, ASSOC2DCONSTRAINTGROUP, c2, c2);
  CHK_ENTITY_3RD (_obj, ASSOC2DCONSTRAINTGROUP, c3, c3);
  CHK_ENTITY_TYPE (_obj, ASSOC2DCONSTRAINTGROUP, w1, BD, w1);
  CHK_ENTITY_TYPE (_obj, ASSOC2DCONSTRAINTGROUP, w2, BD, w2);
  CHK_ENTITY_TYPE (_obj, ASSOC2DCONSTRAINTGROUP, w3, BD, w3);
  CHK_ENTITY_UTF8TEXT (_obj, ASSOC2DCONSTRAINTGROUP, t2, t2);
#endif
}
