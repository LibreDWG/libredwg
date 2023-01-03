// unstable, just some field names missing
#define DWG_TYPE DWG_TYPE_ACSH_CHAMFER_CLASS
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  // Dwg_EvalExpr evalexpr;
  // Dwg_ACSH_HistoryNode history_node;
  BITCODE_BD *trans;
  // AcDbShChamfer
  BITCODE_BL major;
  BITCODE_BL minor;
  BITCODE_BL bl92;       /*!< DXF 92 */
  BITCODE_BD base_dist;  /*!< DXF 41 */
  BITCODE_BD other_dist; /*!< DXF 42 */
  BITCODE_BL num_edges;  /*!< DXF 93 */
  BITCODE_BL *edges;     /*!< DXF 94 */
  BITCODE_BL bl95;       /*!< DXF 95 */

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_acsh_chamfer_class *_obj = dwg_object_to_ACSH_CHAMFER_CLASS (obj);

  CHK_EVALEXPR (ACSH_CHAMFER_CLASS);
  CHK_ACSH_HISTORYNODE ();

  CHK_ENTITY_TYPE (_obj, ACSH_CHAMFER_CLASS, major, BL);
  CHK_ENTITY_TYPE (_obj, ACSH_CHAMFER_CLASS, minor, BL);
  CHK_ENTITY_TYPE (_obj, ACSH_CHAMFER_CLASS, bl92, BL);
  CHK_ENTITY_TYPE (_obj, ACSH_CHAMFER_CLASS, base_dist, BD);
  CHK_ENTITY_TYPE (_obj, ACSH_CHAMFER_CLASS, other_dist, BD);
  CHK_ENTITY_TYPE (_obj, ACSH_CHAMFER_CLASS, num_edges, BL);
  {
    BITCODE_BL i;
    CHK_ENTITY_VECTOR_TYPE (_obj, ACSH_CHAMFER_CLASS, edges, num_edges, BL);
  }
  CHK_ENTITY_TYPE (_obj, ACSH_CHAMFER_CLASS, bl95, BL);
}
