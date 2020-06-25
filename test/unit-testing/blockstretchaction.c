// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_BLOCKSTRETCHACTION
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BLOCKACTION_fields;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_blockstretchaction *_obj = dwg_object_to_BLOCKSTRETCHACTION (obj);

#endif
}
