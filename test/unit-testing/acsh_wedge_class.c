#define DWG_TYPE DWG_TYPE_ACSH_WEDGE_CLASS
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  // Dwg_EvalExpr evalexpr;
  // Dwg_ACSH_HistoryNode history_node;
  BITCODE_BD *trans;
  BITCODE_BL major;
  BITCODE_BL minor;
  BITCODE_BD length;
  BITCODE_BD width;
  BITCODE_BD height;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_acsh_wedge_class *_obj = dwg_object_to_ACSH_WEDGE_CLASS (obj);

  CHK_EVALEXPR (ACSH_WEDGE_CLASS);
  CHK_ACSH_HISTORYNODE ();

  CHK_ENTITY_TYPE (_obj, ACSH_WEDGE_CLASS, major, BL);
  CHK_ENTITY_TYPE (_obj, ACSH_WEDGE_CLASS, minor, BL);
  CHK_ENTITY_TYPE (_obj, ACSH_WEDGE_CLASS, length, BD);
  CHK_ENTITY_TYPE (_obj, ACSH_WEDGE_CLASS, width, BD);
  CHK_ENTITY_TYPE (_obj, ACSH_WEDGE_CLASS, height, BD);
}
