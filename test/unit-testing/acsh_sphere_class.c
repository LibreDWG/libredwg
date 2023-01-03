#define DWG_TYPE DWG_TYPE_ACSH_SPHERE_CLASS
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
  BITCODE_BD radius; /*!< DXF 40 */

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_acsh_sphere_class *_obj = dwg_object_to_ACSH_SPHERE_CLASS (obj);

  CHK_EVALEXPR (ACSH_SPHERE_CLASS);
  CHK_ACSH_HISTORYNODE ();

  CHK_ENTITY_TYPE (_obj, ACSH_SPHERE_CLASS, major, BL);
  CHK_ENTITY_TYPE (_obj, ACSH_SPHERE_CLASS, minor, BL);
  CHK_ENTITY_TYPE (_obj, ACSH_SPHERE_CLASS, radius, BD);
}
