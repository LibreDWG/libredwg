// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_ACSH_REVOLVE_CLASS
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
  BITCODE_3BD axis_pt;
  BITCODE_2RD direction;
  BITCODE_BD revolve_angle;
  BITCODE_BD start_angle;
  BITCODE_BD draft_angle;
  BITCODE_BD bd44;
  BITCODE_BD bd45;
  BITCODE_BD twist_angle;
  BITCODE_B b290;
  BITCODE_B is_close_to_axis;
  struct _dwg_object_entity *sweep_entity;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_acsh_revolve_class *_obj = dwg_object_to_ACSH_REVOLVE_CLASS (obj);

  CHK_EVALEXPR (ACSH_REVOLVE_CLASS);
  CHK_ACSH_HISTORYNODE ();

  CHK_ENTITY_TYPE (_obj, ACSH_REVOLVE_CLASS, major, BL);
  CHK_ENTITY_TYPE (_obj, ACSH_REVOLVE_CLASS, minor, BL);
  CHK_ENTITY_3RD (_obj, ACSH_REVOLVE_CLASS, axis_pt);
  CHK_ENTITY_2RD (_obj, ACSH_REVOLVE_CLASS, direction);
  CHK_ENTITY_TYPE (_obj, ACSH_REVOLVE_CLASS, revolve_angle, BD);
  CHK_ENTITY_TYPE (_obj, ACSH_REVOLVE_CLASS, start_angle, BD);
  CHK_ENTITY_TYPE (_obj, ACSH_REVOLVE_CLASS, draft_angle, BD);
  CHK_ENTITY_TYPE (_obj, ACSH_REVOLVE_CLASS, bd44, BD);
  CHK_ENTITY_TYPE (_obj, ACSH_REVOLVE_CLASS, bd45, BD);
  CHK_ENTITY_TYPE (_obj, ACSH_REVOLVE_CLASS, twist_angle, BD);
  CHK_ENTITY_TYPE (_obj, ACSH_REVOLVE_CLASS, b290, B);
  CHK_ENTITY_TYPE (_obj, ACSH_REVOLVE_CLASS, is_close_to_axis, B);
#endif
}
