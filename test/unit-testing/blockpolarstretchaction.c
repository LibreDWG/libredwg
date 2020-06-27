// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_BLOCKPOLARSTRETCHACTION
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BL i;
  BLOCKACTION_fields;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_blockpolarstretchaction *_obj = dwg_object_to_BLOCKPOLARSTRETCHACTION (obj);

  // AcDbBlockAction
  CHK_ENTITY_3RD (_obj, BLOCKPOLARSTRETCHACTION, ba_pt);
  CHK_ENTITY_TYPE (_obj, BLOCKPOLARSTRETCHACTION, num_actions, BL);
  CHK_ENTITY_VECTOR_TYPE (_obj, BLOCKPOLARSTRETCHACTION, actions, num_actions, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKPOLARSTRETCHACTION, num_deps, BL);
  CHK_ENTITY_HV (_obj, BLOCKPOLARSTRETCHACTION, deps, num_deps);
  // AcDbBlockPolarStretchAction
  // ..
#endif
}
