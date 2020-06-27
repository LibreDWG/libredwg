// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_BLOCKSCALEACTION
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BLOCKACTION_fields;
  BITCODE_BL i;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_blockscaleaction *_obj = dwg_object_to_BLOCKSCALEACTION (obj);

  // AcDbBlockAction
  CHK_ENTITY_3RD (_obj, BLOCKSCALEACTION, display_location);
  CHK_ENTITY_TYPE (_obj, BLOCKSCALEACTION, num_actions, BL);
  CHK_ENTITY_VECTOR_TYPE (_obj, BLOCKSCALEACTION, actions, num_actions, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKSCALEACTION, num_deps, BL);
  CHK_ENTITY_HV (_obj, BLOCKSCALEACTION, deps, num_deps);
  // AcDbBlockScaleAction
  CHK_SUBCLASS_TYPE     (_obj->conn_pt1, BLOCKACTION_connectionpts, code, BL);
  CHK_SUBCLASS_UTF8TEXT (_obj->conn_pt1, BLOCKACTION_connectionpts, name);
  CHK_SUBCLASS_TYPE     (_obj->conn_pt2, BLOCKACTION_connectionpts, code, BL);
  CHK_SUBCLASS_UTF8TEXT (_obj->conn_pt2, BLOCKACTION_connectionpts, name);
  CHK_SUBCLASS_TYPE     (_obj->conn_pt3, BLOCKACTION_connectionpts, code, BL);
  CHK_SUBCLASS_UTF8TEXT (_obj->conn_pt3, BLOCKACTION_connectionpts, name);
  // ..
#endif
}
