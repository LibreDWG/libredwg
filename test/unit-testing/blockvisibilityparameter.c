// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_BLOCKVISIBILITYPARAMETER
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_B is_initialized;
  BITCODE_B b2;
  BITCODE_T name;
  BITCODE_T desc;
  BITCODE_BL num_states;
  Dwg_BLOCKVISIBILITYPARAMETER_state *states;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_blockvisibilityparameter *_obj = dwg_object_to_BLOCKVISIBILITYPARAMETER (obj);

  CHK_ENTITY_TYPE (_obj, BLOCKVISIBILITYPARAMETER, is_initialized, B, is_initialized);
  CHK_ENTITY_TYPE (_obj, BLOCKVISIBILITYPARAMETER, b2, B, b2);
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKVISIBILITYPARAMETER, name, name);
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKVISIBILITYPARAMETER, desc, desc);
  CHK_ENTITY_TYPE (_obj, BLOCKVISIBILITYPARAMETER, num_states, BL, num_states);
  // ..
#endif
}
