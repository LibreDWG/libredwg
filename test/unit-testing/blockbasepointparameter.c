// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_BLOCKBASEPOINTPARAMETER
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
  dwg_obj_blockbasepointparameter *_obj = dwg_object_to_BLOCKBASEPOINTPARAMETER (obj);

#endif
}
