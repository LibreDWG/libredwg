// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_BLOCKPOLARPARAMETER
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BLOCKPARAMETER_fields;
  // ..
  BLOCKPARAMVALUESET_fields;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_blockpolarparameter *_obj = dwg_object_to_BLOCKPOLARPARAMETER (obj);

#endif
}
