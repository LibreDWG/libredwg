#define DWG_TYPE DWG_TYPE_ACSH_FILLET_CLASS
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  // Dwg_EvalExpr evalexpr;
  // Dwg_ACSH_HistoryNode history_node;
  BITCODE_BD *trans;
  // AcDbShFillet
  BITCODE_BL major;             /*!< DXF 90 */
  BITCODE_BL minor;             /*!< DXF 91 */
  BITCODE_BL bl92;              /*!< DXF 92 */
  BITCODE_BL num_edges;         /*!< DXF 93 */
  BITCODE_BL *edges;            /*!< DXF 94 */
  BITCODE_BL num_radiuses;      /*!< DXF 95 */
  BITCODE_BL num_startsetbacks; /*!< DXF 96 */
  BITCODE_BL num_endsetbacks;   /*!< DXF 97 */
  BITCODE_BD *radiuses;         /*!< DXF 41 */
  BITCODE_BD *startsetbacks;    /*!< DXF 42 */
  BITCODE_BD *endsetbacks;      /*!< DXF 43 */

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_acsh_fillet_class *_obj = dwg_object_to_ACSH_FILLET_CLASS (obj);

  CHK_EVALEXPR (ACSH_FILLET_CLASS);
  CHK_ACSH_HISTORYNODE ();

  {
    BITCODE_BL i;
    CHK_ENTITY_TYPE (_obj, ACSH_FILLET_CLASS, major, BL);
    CHK_ENTITY_TYPE (_obj, ACSH_FILLET_CLASS, minor, BL);
    CHK_ENTITY_TYPE (_obj, ACSH_FILLET_CLASS, num_edges, BL);
    CHK_ENTITY_VECTOR_TYPE (_obj, ACSH_FILLET_CLASS, edges, num_edges, BL);
    CHK_ENTITY_TYPE (_obj, ACSH_FILLET_CLASS, num_radiuses, BL);
    CHK_ENTITY_VECTOR_TYPE (_obj, ACSH_FILLET_CLASS, radiuses, num_radiuses,
                            BD);
    CHK_ENTITY_TYPE (_obj, ACSH_FILLET_CLASS, num_startsetbacks, BL);
    CHK_ENTITY_VECTOR_TYPE (_obj, ACSH_FILLET_CLASS, startsetbacks,
                            num_startsetbacks, BD);
    CHK_ENTITY_TYPE (_obj, ACSH_FILLET_CLASS, num_endsetbacks, BL);
    CHK_ENTITY_VECTOR_TYPE (_obj, ACSH_FILLET_CLASS, endsetbacks,
                            num_endsetbacks, BD);
  }
}
