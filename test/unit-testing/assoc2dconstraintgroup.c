#define DWG_TYPE DWG_TYPE_ASSOC2DCONSTRAINTGROUP
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  ASSOCACTION_fields;

  BITCODE_BL version;
  BITCODE_B b1;
  BITCODE_3BD workplane[3];
  BITCODE_H h1;
  BITCODE_BL num_actions;
  BITCODE_H *actions;
  BITCODE_BL num_nodes;
  Dwg_CONSTRAINTGROUPNODE *nodes;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  // #ifdef DEBUG_CLASSES
  dwg_obj_assoc2dconstraintgroup *_obj
      = dwg_object_to_ASSOC2DCONSTRAINTGROUP (obj);
  // ASSOCACTION
  CHK_ENTITY_TYPE (_obj, ASSOC2DCONSTRAINTGROUP, class_version, BS);
  CHK_ENTITY_TYPE (_obj, ASSOC2DCONSTRAINTGROUP, geometry_status, BL);
  CHK_ENTITY_MAX (_obj, ASSOC2DCONSTRAINTGROUP, geometry_status, BL, 10);
  CHK_ENTITY_H (_obj, ASSOC2DCONSTRAINTGROUP, owningnetwork);
  CHK_ENTITY_H (_obj, ASSOC2DCONSTRAINTGROUP, actionbody);
  CHK_ENTITY_TYPE (_obj, ASSOC2DCONSTRAINTGROUP, action_index, BL);
  CHK_ENTITY_TYPE (_obj, ASSOC2DCONSTRAINTGROUP, max_assoc_dep_index, BL);
  CHK_ENTITY_TYPE (_obj, ASSOC2DCONSTRAINTGROUP, num_deps, BL);
  for (unsigned i = 0; i < num_deps; i++)
    {
      CHK_SUBCLASS_TYPE (_obj->deps[i], ASSOCACTION_Deps, is_owned, B);
      CHK_SUBCLASS_H (_obj->deps[i], ASSOCACTION_Deps, dep);
    }
  CHK_ENTITY_TYPE (_obj, ASSOC2DCONSTRAINTGROUP, num_owned_params, BL);
  CHK_ENTITY_HV (_obj, ASSOC2DCONSTRAINTGROUP, owned_params, num_owned_params);
  CHK_ENTITY_TYPE (_obj, ASSOCACTION, num_values, BL);
  CHK_VALUEPARAM (num_values, values);

  // ASSOC2DCONSTRAINTGROUP
  CHK_ENTITY_TYPE (_obj, ASSOC2DCONSTRAINTGROUP, version, BL);
  CHK_ENTITY_TYPE (_obj, ASSOC2DCONSTRAINTGROUP, b1, B);
  // 3x10 workplane
  // CHK_ENTITY_3RD (_obj, ASSOC2DCONSTRAINTGROUP, workplane[3], &workplane[3][0]);
  CHK_ENTITY_H (_obj, ASSOC2DCONSTRAINTGROUP, h1);
  CHK_ENTITY_TYPE (_obj, ASSOC2DCONSTRAINTGROUP, num_actions, BL);
  CHK_ENTITY_HV (_obj, ASSOC2DCONSTRAINTGROUP, actions, num_actions);
  CHK_ENTITY_TYPE (_obj, ASSOC2DCONSTRAINTGROUP, num_nodes, BL);
  CHK_ENTITY_MAX (_obj, ASSOC2DCONSTRAINTGROUP, num_nodes, BL, 200);
  for (unsigned i = 0; i < num_nodes; i++)
    {
      CHK_SUBCLASS_TYPE (_obj->nodes[i], CONSTRAINTGROUPNODE, nodeid, BL);
      CHK_SUBCLASS_TYPE (_obj->nodes[i], CONSTRAINTGROUPNODE, status, RC);
      CHK_SUBCLASS_TYPE (_obj->nodes[i], CONSTRAINTGROUPNODE, num_connections,
                         BL);
      CHK_SUBCLASS_MAX (_obj->nodes[i], CONSTRAINTGROUPNODE, num_connections,
                        BL, _obj->num_actions);
      if (_obj->nodes[i].num_connections > _obj->num_actions)
        fail ("Invalid nodes[%u].num_connections", i);
      else if (!_obj->nodes[i].connections)
        fail ("Empty nodes[%u].connections", i);
      else
        for (unsigned j = 0;
             j < MIN (_obj->num_actions, _obj->nodes[i].num_connections); j++)
          {
            if (!_obj->nodes[i].connections[j])
              fail("Invalid connections[%u]", j);
            else
              ok ("CONSTRAINTGROUPNODE[%d].connections[%d]: %d", i, j,
                  (int)_obj->nodes[i].connections[j]);
          }
    }
  // #endif
}
