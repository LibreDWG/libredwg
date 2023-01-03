#define DWG_TYPE DWG_TYPE_ACSH_BOOLEAN_CLASS
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  // Dwg_EvalExpr evalexpr;
  // Dwg_ACSH_HistoryNode history_node;
  BITCODE_BD *trans;
  // AcDbShBoolean
  BITCODE_BL major;      /*!< DXF 90 */
  BITCODE_BL minor;      /*!< DXF 91 */
  BITCODE_RCd operation; /*!< DXF 280 */
  BITCODE_BL operand1;   /*!< DXF 92 */
  BITCODE_BL operand2;   /*!< DXF 93 */

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_acsh_boolean_class *_obj = dwg_object_to_ACSH_BOOLEAN_CLASS (obj);

  CHK_EVALEXPR (ACSH_BOOLEAN_CLASS);
  CHK_ACSH_HISTORYNODE ();

  CHK_ENTITY_TYPE (_obj, ACSH_BOOLEAN_CLASS, major, BL);
  CHK_ENTITY_TYPE (_obj, ACSH_BOOLEAN_CLASS, minor, BL);
  CHK_ENTITY_TYPE (_obj, ACSH_BOOLEAN_CLASS, operation, RCd);
  CHK_ENTITY_TYPE (_obj, ACSH_BOOLEAN_CLASS, operand1, BL);
  CHK_ENTITY_TYPE (_obj, ACSH_BOOLEAN_CLASS, operand2, BL);
}
