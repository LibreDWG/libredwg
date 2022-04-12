// debugging
#define DWG_TYPE DWG_TYPE_BREAKPOINTREF
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_breakpointref *_obj = dwg_object_to_BREAKPOINTREF (obj);
  // ... XrefFullSubendPath
#endif
}
