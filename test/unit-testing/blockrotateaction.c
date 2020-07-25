#define DWG_TYPE DWG_TYPE_BLOCKROTATEACTION
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BLOCKACTION_WITHBASEPT_fields;
  BITCODE_BL i;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_blockrotateaction *_obj = dwg_object_to_BLOCKROTATEACTION (obj);

  // AcDbBlockAction
  CHK_ENTITY_3RD (_obj, BLOCKROTATEACTION, display_location);
  CHK_ENTITY_TYPE (_obj, BLOCKROTATEACTION, num_actions, BL);
  CHK_ENTITY_VECTOR_TYPE (_obj, BLOCKROTATEACTION, actions, num_actions, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKROTATEACTION, num_deps, BL);
  CHK_ENTITY_HV (_obj, BLOCKROTATEACTION, deps, num_deps);
  // AcDbBlockActionWithBasePt
  CHK_ENTITY_3RD (_obj, BLOCKSCALEACTION, offset);
  CHK_SUBCLASS_TYPE     (_obj->conn_pt1, BLOCKACTION_connectionpts, code, BL);
  CHK_SUBCLASS_UTF8TEXT (_obj->conn_pt1, BLOCKACTION_connectionpts, name);
  CHK_SUBCLASS_TYPE     (_obj->conn_pt2, BLOCKACTION_connectionpts, code, BL);
  CHK_SUBCLASS_UTF8TEXT (_obj->conn_pt2, BLOCKACTION_connectionpts, name);
  CHK_ENTITY_TYPE (_obj, BLOCKSCALEACTION, dependent, B);
  CHK_ENTITY_3RD (_obj, BLOCKSCALEACTION, base_pt);
  CHK_ENTITY_3RD (_obj, BLOCKSCALEACTION, stretch_pt);
  // AcDbBlockRotateAction
  CHK_SUBCLASS_TYPE     (_obj->conn_pt3, BLOCKACTION_connectionpts, code, BL);
  CHK_SUBCLASS_UTF8TEXT (_obj->conn_pt3, BLOCKACTION_connectionpts, name);
}
