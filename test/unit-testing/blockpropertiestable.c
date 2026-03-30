// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_BLOCKPROPERTIESTABLE
#include "tests_common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_blockpropertiestable *_obj
      = dwg_object_to_BLOCKPROPERTIESTABLE (obj);

#endif
}
