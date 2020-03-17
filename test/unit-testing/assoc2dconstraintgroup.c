// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_ASSOC2DCONSTRAINTGROUP
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

  BITCODE_BL l5; //90 1
  BITCODE_B  b1; //70 0
  BITCODE_3BD workplane[3]; //3x10 workplane
  // 360
  BITCODE_BL l6; //90 2
  // 360 360
  BITCODE_BL l7; //90 9
  BITCODE_BL l8; //90 9
  BITCODE_BL cl1; //90 1
  BITCODE_RC cs1; //70 1
  BITCODE_BL cl2; //90 1
  BITCODE_BL cl3; //90 3
  BITCODE_BL cl4; //90 0
  BITCODE_3BD c1; //10 @134
  BITCODE_3BD c2; //10
  BITCODE_3BD c3; //10
  BITCODE_BD w1; //40
  BITCODE_BD w2; //40
  BITCODE_BD w3; //40

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_assoc2dconstraintgroup *_obj = dwg_object_to_ASSOC2DCONSTRAINTGROUP (obj);

  CHK_ENTITY_TYPE (_obj, ASSOC2DCONSTRAINTGROUP, solution_status, BL, solution_status); /* 90 */
  CHK_ENTITY_TYPE (_obj, ASSOC2DCONSTRAINTGROUP, geometry_status, BL, geometry_status); /* 90 */
  CHK_ENTITY_H (_obj, ASSOC2DCONSTRAINTGROUP, readdep, readdep);           /* 330 */
  CHK_ENTITY_H (_obj, ASSOC2DCONSTRAINTGROUP, writedep, writedep);         /* 360 */
  CHK_ENTITY_TYPE (_obj, ASSOC2DCONSTRAINTGROUP, constraint_status, BL, constraint_status); /* 90 */
  CHK_ENTITY_TYPE (_obj, ASSOC2DCONSTRAINTGROUP, dof, BL, dof);               /* 90 */
  CHK_ENTITY_TYPE (_obj, ASSOC2DCONSTRAINTGROUP, is_body_a_proxy, B, is_body_a_proxy);    /* 90 */

  CHK_ENTITY_TYPE (_obj, ASSOC2DCONSTRAINTGROUP, l5, BL, l5); //90 1
  CHK_ENTITY_TYPE (_obj, ASSOC2DCONSTRAINTGROUP, b1, B , b1); //70 0
  //CHK_ENTITY_3RD (_obj, ASSOC2DCONSTRAINTGROUP, workplane[3], workplane[3]); //3x10 workplane
  CHK_ENTITY_TYPE (_obj, ASSOC2DCONSTRAINTGROUP, l6, BL, l6); //90 2
  CHK_ENTITY_TYPE (_obj, ASSOC2DCONSTRAINTGROUP, l7, BL, l7); //90 9
  CHK_ENTITY_TYPE (_obj, ASSOC2DCONSTRAINTGROUP, l8, BL, l8); //90 9
  CHK_ENTITY_TYPE (_obj, ASSOC2DCONSTRAINTGROUP, cl1, BL, cl1); //90 1
  CHK_ENTITY_TYPE (_obj, ASSOC2DCONSTRAINTGROUP, cs1, RC, cs1); //70 1
  CHK_ENTITY_TYPE (_obj, ASSOC2DCONSTRAINTGROUP, cl2, BL, cl2); //90 1
  CHK_ENTITY_TYPE (_obj, ASSOC2DCONSTRAINTGROUP, cl3, BL, cl3); //90 3
  CHK_ENTITY_TYPE (_obj, ASSOC2DCONSTRAINTGROUP, cl4, BL, cl4); //90 0
  CHK_ENTITY_3RD (_obj, ASSOC2DCONSTRAINTGROUP, c1, c1); //10 @134
  CHK_ENTITY_3RD (_obj, ASSOC2DCONSTRAINTGROUP, c2, c2); //10
  CHK_ENTITY_3RD (_obj, ASSOC2DCONSTRAINTGROUP, c3, c3); //10
  CHK_ENTITY_TYPE (_obj, ASSOC2DCONSTRAINTGROUP, w1, BD, w1); //40
  CHK_ENTITY_TYPE (_obj, ASSOC2DCONSTRAINTGROUP, w2, BD, w2); //40
  CHK_ENTITY_TYPE (_obj, ASSOC2DCONSTRAINTGROUP, w3, BD, w3); //40
#endif
}
