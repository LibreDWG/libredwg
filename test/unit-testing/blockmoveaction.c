// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_BLOCKMOVEACTION
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BL i;
  BLOCKACTION_fields;
  BLOCKACTION_doubles_fields;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_blockmoveaction *_obj = dwg_object_to_BLOCKMOVEACTION (obj);

  // AcDbBlockAction
  CHK_ENTITY_3RD (_obj, BLOCKMOVEACTION, ba_pt);
  CHK_ENTITY_TYPE (_obj, BLOCKMOVEACTION, num_actions, BL);
  CHK_ENTITY_VECTOR_TYPE (_obj, BLOCKMOVEACTION, actions, num_actions, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKMOVEACTION, num_deps, BL);
  CHK_ENTITY_HV (_obj, BLOCKMOVEACTION, deps, num_deps);
  // AcDbBlockMoveAction
  CHK_SUBCLASS_TYPE     (_obj->conn_pt1, BLOCKACTION_connectionpts, code, BL);
  CHK_SUBCLASS_UTF8TEXT (_obj->conn_pt1, BLOCKACTION_connectionpts, name);
  CHK_SUBCLASS_TYPE     (_obj->conn_pt2, BLOCKACTION_connectionpts, code, BL);
  CHK_SUBCLASS_UTF8TEXT (_obj->conn_pt2, BLOCKACTION_connectionpts, name);
  CHK_ENTITY_TYPE (_obj, BLOCKMOVEACTION, action_offset_x, BD);
  CHK_ENTITY_TYPE (_obj, BLOCKMOVEACTION, action_offset_y, BD);
  CHK_ENTITY_TYPE (_obj, BLOCKMOVEACTION, angle_offset, BD);
#endif
}
