// TODO DEBUGGING
// needed at all? or just a subclass?
#define DWG_TYPE DWG_TYPE_ANNOTSCALEOBJECTCONTEXTDATA
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  ANNOTSCALEOBJECTCONTEXTDATA_fields;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  // #ifdef DEBUG_CLASSES
  dwg_obj_annotscaleobjectcontextdata *_obj
      = dwg_object_to_ANNOTSCALEOBJECTCONTEXTDATA (obj);

  CHK_ENTITY_TYPE (_obj, ANNOTSCALEOBJECTCONTEXTDATA, class_version, BS);
  CHK_ENTITY_TYPE (_obj, ANNOTSCALEOBJECTCONTEXTDATA, is_default, B);
  CHK_ENTITY_H (_obj, ANNOTSCALEOBJECTCONTEXTDATA, scale);
  // #endif
}
