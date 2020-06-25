// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_BLOCKLOOKUPACTION
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BLOCKACTION_fields;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_blocklookupaction *_obj = dwg_object_to_BLOCKLOOKUPACTION (obj);

#endif
}
