// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_ACSH_HISTORY_CLASS
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  //Dwg_EvalExpr evalexpr;
  //Dwg_ACSH_HistoryNode history_node;
  BITCODE_BD *trans;
  BITCODE_BL major;
  BITCODE_BL minor;
  BITCODE_H owner;
  BITCODE_BL h_nodeid;
  BITCODE_B b280;
  BITCODE_B b281;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_acsh_history_class *_obj = dwg_object_to_ACSH_HISTORY_CLASS (obj);

  CHK_EVALEXPR (ACSH_HISTORY_CLASS);
  if (0) { //crashes
    CHK_ACSH_HISTORYNODE();
  }

  CHK_ENTITY_TYPE (_obj, ACSH_HISTORY_CLASS, major, BL); //33
  CHK_ENTITY_TYPE (_obj, ACSH_HISTORY_CLASS, minor, BL); //29
  CHK_ENTITY_H (_obj, ACSH_HISTORY_CLASS, owner);
  CHK_ENTITY_TYPE (_obj, ACSH_HISTORY_CLASS, h_nodeid, BL);
  CHK_ENTITY_TYPE (_obj, ACSH_HISTORY_CLASS, b280, B);
  CHK_ENTITY_TYPE (_obj, ACSH_HISTORY_CLASS, b281, B);
  
#endif
}
