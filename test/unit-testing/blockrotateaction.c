// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_BLOCKROTATEACTION
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BLOCKACTION_fields;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_blockrotateaction *_obj = dwg_object_to_BLOCKROTATEACTION (obj);

#endif
}
