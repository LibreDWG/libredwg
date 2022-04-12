// debugging
#define DWG_TYPE DWG_TYPE_BREAKDATA
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BL num_pointrefs;
  BITCODE_H *pointrefs;
  BITCODE_H dimref;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_breakdata *_obj = dwg_object_to_BREAKDATA (obj);

  CHK_ENTITY_TYPE (_obj, BREAKDATA, num_pointrefs, BL);
  CHK_ENTITY_HV (_obj, BREAKDATA, pointrefs, num_pointrefs);
  CHK_ENTITY_H (_obj, BREAKDATA, dimref);
#endif
}
