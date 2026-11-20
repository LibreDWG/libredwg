#define DWG_TYPE DWG_TYPE_ACSH_REVOLVE_CLASS
#include "tests_common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  // Dwg_EvalExpr evalexpr;
  // Dwg_ACSH_HistoryNode history_node;
  BITCODE_BD *trans;
  BITCODE_BL major;
  BITCODE_BL minor;
  BITCODE_3BD axis_pt;
  BITCODE_3BD direction;
  BITCODE_BD revolve_angle;
  BITCODE_BD start_angle;
  BITCODE_BD draft_angle;
  BITCODE_BD draft_start_distance;
  BITCODE_BD draft_end_distance;
  BITCODE_BD twist_angle;
  BITCODE_B solid;
  BITCODE_B is_close_to_axis;
  BITCODE_BL method;
  BITCODE_BL bindata_size;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_acsh_revolve_class *_obj = dwg_object_to_ACSH_REVOLVE_CLASS (obj);

  CHK_EVALEXPR (ACSH_REVOLVE_CLASS);
  CHK_ACSH_HISTORYNODE ();

  CHK_ENTITY_TYPE (_obj, ACSH_REVOLVE_CLASS, major, BL);
  CHK_ENTITY_TYPE (_obj, ACSH_REVOLVE_CLASS, minor, BL);
  CHK_ENTITY_3RD (_obj, ACSH_REVOLVE_CLASS, axis_pt);
  CHK_ENTITY_3RD (_obj, ACSH_REVOLVE_CLASS, direction);
  CHK_ENTITY_TYPE (_obj, ACSH_REVOLVE_CLASS, revolve_angle, BD);
  CHK_ENTITY_TYPE (_obj, ACSH_REVOLVE_CLASS, start_angle, BD);
  CHK_ENTITY_TYPE (_obj, ACSH_REVOLVE_CLASS, draft_angle, BD);
  CHK_ENTITY_TYPE (_obj, ACSH_REVOLVE_CLASS, draft_start_distance, BD);
  CHK_ENTITY_TYPE (_obj, ACSH_REVOLVE_CLASS, draft_end_distance, BD);
  CHK_ENTITY_TYPE (_obj, ACSH_REVOLVE_CLASS, twist_angle, BD);
  CHK_ENTITY_TYPE (_obj, ACSH_REVOLVE_CLASS, solid, B);
  CHK_ENTITY_TYPE (_obj, ACSH_REVOLVE_CLASS, is_close_to_axis, B);
  CHK_ENTITY_TYPE (_obj, ACSH_REVOLVE_CLASS, method, BL);
  CHK_ENTITY_TYPE (_obj, ACSH_REVOLVE_CLASS, bindata_size, BL);
}
