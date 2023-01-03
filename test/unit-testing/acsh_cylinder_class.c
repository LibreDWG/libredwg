#define DWG_TYPE DWG_TYPE_ACSH_CYLINDER_CLASS
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
  BITCODE_BD height;       /*!< DXF 40 */
  BITCODE_BD major_radius; /*!< DXF 41 */
  BITCODE_BD minor_radius; /*!< DXF 42 */
  BITCODE_BD x_radius;     /*!< DXF 43 */

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_acsh_cylinder_class *_obj = dwg_object_to_ACSH_CYLINDER_CLASS (obj);

  CHK_EVALEXPR (ACSH_CYLINDER_CLASS);
  CHK_ACSH_HISTORYNODE ();

  CHK_ENTITY_TYPE (_obj, ACSH_CYLINDER_CLASS, major, BL);
  CHK_ENTITY_TYPE (_obj, ACSH_CYLINDER_CLASS, minor, BL);
  CHK_ENTITY_TYPE (_obj, ACSH_CYLINDER_CLASS, height, BD);
  CHK_ENTITY_TYPE (_obj, ACSH_CYLINDER_CLASS, major_radius, BD);
  CHK_ENTITY_TYPE (_obj, ACSH_CYLINDER_CLASS, minor_radius, BD);
  CHK_ENTITY_TYPE (_obj, ACSH_CYLINDER_CLASS, x_radius, BD);
}
