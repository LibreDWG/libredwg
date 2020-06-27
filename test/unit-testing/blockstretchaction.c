// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_BLOCKSTRETCHACTION
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BL i;
  BLOCKACTION_fields;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_blockstretchaction *_obj = dwg_object_to_BLOCKSTRETCHACTION (obj);

  // AcDbBlockAction
  CHK_ENTITY_3RD (_obj, BLOCKSTRETCHACTION, ba_pt);
  CHK_ENTITY_TYPE (_obj, BLOCKSTRETCHACTION, num_actions, BL);
  CHK_ENTITY_VECTOR_TYPE (_obj, BLOCKSTRETCHACTION, actions, num_actions, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKSTRETCHACTION, num_deps, BL);
  CHK_ENTITY_HV (_obj, BLOCKSTRETCHACTION, deps, num_deps);
  // AcDbBlockStretchAction
  // ..
#endif
}
