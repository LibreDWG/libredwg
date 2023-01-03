// TODO debugging
#define DWG_TYPE DWG_TYPE_PERSUBENTMGR
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_BL class_version;
  BITCODE_BL unknown_0;
  BITCODE_BL unknown_2;
  BITCODE_BL numassocsteps;
  BITCODE_BL numassocsubents;
  BITCODE_BL i, num_steps;
  BITCODE_BL *steps;
  BITCODE_BL num_subents;
  BITCODE_BL *subents;

#ifdef DEBUG_CLASSES
  dwg_obj_persubentmgr *_obj = dwg_object_to_PERSUBENTMGR (obj);

  CHK_ENTITY_TYPE (_obj, PERSUBENTMGR, class_version, BL);
  CHK_ENTITY_MAX (_obj, PERSUBENTMGR, class_version, BL, 2);
  CHK_ENTITY_TYPE (_obj, PERSUBENTMGR, unknown_0, BL);
  CHK_ENTITY_TYPE (_obj, PERSUBENTMGR, unknown_2, BL);
  CHK_ENTITY_TYPE (_obj, PERSUBENTMGR, numassocsteps, BL);
  CHK_ENTITY_TYPE (_obj, PERSUBENTMGR, numassocsubents, BL);
  CHK_ENTITY_TYPE (_obj, PERSUBENTMGR, num_steps, BL);
  CHK_ENTITY_MAX (_obj, PERSUBENTMGR, num_steps, BL, 500);
  CHK_ENTITY_VECTOR_TYPE (_obj, PERSUBENTMGR, steps, num_steps, BL);
  CHK_ENTITY_TYPE (_obj, PERSUBENTMGR, num_subents, BL);
  CHK_ENTITY_MAX (_obj, PERSUBENTMGR, num_subents, BL, 100);
  CHK_ENTITY_VECTOR_TYPE (_obj, PERSUBENTMGR, subents, num_subents, BL);
#endif
}
