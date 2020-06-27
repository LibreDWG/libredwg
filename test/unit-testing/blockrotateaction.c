// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_BLOCKROTATEACTION
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BLOCKACTION_fields;
  BITCODE_BL i, info_num1;
  BITCODE_T info_text1;
  BITCODE_BL info_num2;
  BITCODE_T info_text2;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_blockrotateaction *_obj = dwg_object_to_BLOCKROTATEACTION (obj);

  // AcDbBlockAction
  CHK_ENTITY_3RD (_obj, BLOCKROTATEACTION, ba_pt);
  CHK_ENTITY_TYPE (_obj, BLOCKROTATEACTION, num_actions, BL);
  CHK_ENTITY_VECTOR_TYPE (_obj, BLOCKROTATEACTION, actions, num_actions, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKROTATEACTION, num_deps, BL);
  CHK_ENTITY_HV (_obj, BLOCKROTATEACTION, deps, num_deps);
  // AcDbBlockRotateAction
  CHK_SUBCLASS_TYPE     (_obj->conn_pt1, BLOCKACTION_connectionpts, code, BL);
  CHK_SUBCLASS_UTF8TEXT (_obj->conn_pt1, BLOCKACTION_connectionpts, name);
#endif
}
