// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_BLOCKFLIPACTION
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BLOCKACTION_fields;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_blockflipaction *_obj = dwg_object_to_BLOCKFLIPACTION (obj);

#endif
}
