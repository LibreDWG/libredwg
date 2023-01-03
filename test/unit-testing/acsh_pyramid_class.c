// unstable
#define DWG_TYPE DWG_TYPE_ACSH_PYRAMID_CLASS
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
  BITCODE_BD height;    /*!< DXF 40 */
  BITCODE_BL sides;     /*!< DXF 92 */
  BITCODE_BD radius;    /*!< DXF 41 */
  BITCODE_BD topradius; /*!< DXF 42 */

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_acsh_pyramid_class *_obj = dwg_object_to_ACSH_PYRAMID_CLASS (obj);

  CHK_EVALEXPR (ACSH_PYRAMID_CLASS);
  CHK_ACSH_HISTORYNODE ();

  CHK_ENTITY_TYPE (_obj, ACSH_PYRAMID_CLASS, major, BL);
  CHK_ENTITY_TYPE (_obj, ACSH_PYRAMID_CLASS, minor, BL);
  CHK_ENTITY_TYPE (_obj, ACSH_PYRAMID_CLASS, sides, BL);
  CHK_ENTITY_TYPE (_obj, ACSH_PYRAMID_CLASS, height, BD);
  CHK_ENTITY_TYPE (_obj, ACSH_PYRAMID_CLASS, radius, BD);
  CHK_ENTITY_TYPE (_obj, ACSH_PYRAMID_CLASS, topradius, BD);
}
