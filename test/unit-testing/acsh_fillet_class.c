// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_ACSH_FILLET_CLASS
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  // AcDbEvalExpr
  BITCODE_BL class_version; // 90
  BITCODE_BL ee_bl98; //33
  BITCODE_BL ee_bl99; //29
  Dwg_ACSH_HistoryNode history_node;
  BITCODE_BD* trans;
  // AcDbShFillet
  BITCODE_BL bl90;       /*!< DXF 90 */
  BITCODE_BL bl91;       /*!< DXF 91 */
  BITCODE_BL bl92;	 /*!< DXF 92 */
  BITCODE_BL i, num_edges;	 /*!< DXF 93 */
  BITCODE_BL *edges;     /*!< DXF 94 */
  BITCODE_BL num_radiuses;	/*!< DXF 95 */
  BITCODE_BL num_startsetbacks;	/*!< DXF 96 */
  BITCODE_BL num_endsetbacks;	/*!< DXF 97 */
  BITCODE_BD *radiuses;	        /*!< DXF 41 */
  BITCODE_BD *startsetbacks;	/*!< DXF 42 */
  BITCODE_BD *endsetbacks;	/*!< DXF 43 */

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_acsh_fillet_class *_obj = dwg_object_to_ACSH_FILLET_CLASS (obj);

  CHK_ENTITY_TYPE (_obj, ACSH_FILLET_CLASS, class_version, BL); // 90
  CHK_ENTITY_TYPE (_obj, ACSH_FILLET_CLASS, ee_bl98, BL); //33
  CHK_ENTITY_TYPE (_obj, ACSH_FILLET_CLASS, ee_bl99, BL); //29
  CHK_SUBCLASS_TYPE (_obj->history_node, ACSH_HistoryNode, bl90, BL); //33
  CHK_SUBCLASS_TYPE (_obj->history_node, ACSH_HistoryNode, bl91, BL);
  // last 16x nums 40-55
  if (!dwg_dynapi_entity_value (_obj, "ACSH_HistoryNode", "trans", &trans, NULL))
    fail ("ACSH_HistoryNode.trans");
  for (i = 0; i < 16; i++)
    {
      if (trans[i] == _obj->history_node.trans[i]) // catches nan
        ok ("ACSH_HistoryNode.trans[%d]: %f", i, trans[i]);
      else
        fail ("ACSH_HistoryNode.trans[%d]: %f", i, trans[i]);
    }
  CHK_SUBCLASS_CMC (_obj->history_node, ACSH_HistoryNode, color);
  CHK_SUBCLASS_TYPE (_obj->history_node, ACSH_HistoryNode, bl92, BL);
  CHK_SUBCLASS_H (_obj->history_node, ACSH_HistoryNode, h347);

  CHK_ENTITY_TYPE (_obj, ACSH_FILLET_CLASS, bl90, BL);
  CHK_ENTITY_TYPE (_obj, ACSH_FILLET_CLASS, bl91, BL);
  CHK_ENTITY_TYPE (_obj, ACSH_FILLET_CLASS, num_edges, BL);
  CHK_ENTITY_VECTOR_TYPE (_obj, ACSH_FILLET_CLASS, edges, num_edges, BL);
  CHK_ENTITY_TYPE (_obj, ACSH_FILLET_CLASS, num_radiuses, BL);
  CHK_ENTITY_VECTOR_TYPE (_obj, ACSH_FILLET_CLASS, radiuses, num_radiuses, BD);
  CHK_ENTITY_TYPE (_obj, ACSH_FILLET_CLASS, num_startsetbacks, BL);
  CHK_ENTITY_VECTOR_TYPE (_obj, ACSH_FILLET_CLASS, startsetbacks, num_startsetbacks, BD);
  CHK_ENTITY_TYPE (_obj, ACSH_FILLET_CLASS, num_endsetbacks, BL);
  CHK_ENTITY_VECTOR_TYPE (_obj, ACSH_FILLET_CLASS, endsetbacks, num_endsetbacks, BD);
#endif
}
