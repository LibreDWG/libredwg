// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_BLOCKLOOKUPACTION
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BL i;
  BLOCKACTION_fields;
  BITCODE_BL info_code93;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_blocklookupaction *_obj = dwg_object_to_BLOCKLOOKUPACTION (obj);

  // AcDbBlockAction
  CHK_ENTITY_3RD (_obj, BLOCKLOOKUPACTION, display_location);
  CHK_ENTITY_TYPE (_obj, BLOCKLOOKUPACTION, num_actions, BL);
  CHK_ENTITY_VECTOR_TYPE (_obj, BLOCKLOOKUPACTION, actions, num_actions, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKLOOKUPACTION, num_deps, BL);
  CHK_ENTITY_HV (_obj, BLOCKLOOKUPACTION, deps, num_deps);
  // AcDbBlockLookupAction
  CHK_SUBCLASS_TYPE     (_obj->conn_pt1, BLOCKACTION_connectionpts, code, BL);
  CHK_SUBCLASS_UTF8TEXT (_obj->conn_pt1, BLOCKACTION_connectionpts, name);
  CHK_ENTITY_TYPE (_obj, BLOCKMOVEACTION, info_code93, BL);
  // ..
#endif
}
