// TODO debugging
#define DWG_TYPE DWG_TYPE_ASSOCPERSSUBENTMANAGER
#include "tests_common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_BL class_version;
  BITCODE_BL unknown_3;
  BITCODE_BL unknown_0;
  BITCODE_BL unknown_2;
  BITCODE_BL unknown_bl1;
  BITCODE_BL unknown_bl2;
  BITCODE_BL num_steps;
  BITCODE_BL *steps;
  BITCODE_BL i;
  BITCODE_BL num_subents;
  BITCODE_BL *subents;
  BITCODE_BL unknown_bl3;
  BITCODE_B unknown_b1;

#ifdef DEBUG_CLASSES
  dwg_obj_assocperssubentmanager *_obj
      = dwg_object_to_ASSOCPERSSUBENTMANAGER (obj);
  if (obj->fixedtype == DWG_TYPE_UNKNOWN_OBJ)
    return;

  CHK_ENTITY_TYPE (_obj, ASSOCPERSSUBENTMANAGER, class_version, BL);
  CHK_ENTITY_MAX (_obj, ASSOCPERSSUBENTMANAGER, class_version, BL, 3);
  CHK_ENTITY_TYPE (_obj, ASSOCPERSSUBENTMANAGER, unknown_3, BL);
  CHK_ENTITY_TYPE (_obj, ASSOCPERSSUBENTMANAGER, unknown_0, BL);
  CHK_ENTITY_TYPE (_obj, ASSOCPERSSUBENTMANAGER, unknown_2, BL);
  CHK_ENTITY_TYPE (_obj, ASSOCPERSSUBENTMANAGER, unknown_bl1, BL);
  CHK_ENTITY_TYPE (_obj, ASSOCPERSSUBENTMANAGER, unknown_bl2, BL);
  CHK_ENTITY_TYPE (_obj, ASSOCPERSSUBENTMANAGER, num_steps, BL);
  CHK_ENTITY_VECTOR_TYPE (_obj, ASSOCPERSSUBENTMANAGER, steps, num_steps, BL);
  CHK_ENTITY_TYPE (_obj, ASSOCPERSSUBENTMANAGER, num_subents, BL);
  CHK_ENTITY_VECTOR_TYPE (_obj, ASSOCPERSSUBENTMANAGER, subents, num_subents,
                          BL);
  CHK_ENTITY_TYPE (_obj, ASSOCPERSSUBENTMANAGER, unknown_bl3, BL);
  CHK_ENTITY_TYPE (_obj, ASSOCPERSSUBENTMANAGER, unknown_b1, B);
#endif
}
