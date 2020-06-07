// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_ACSH_WEDGE_CLASS
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
  BITCODE_BD *trans;
  // AcDbShWedge (same as box)
  BITCODE_BL bl90;       /*!< DXF 90 (33) */
  BITCODE_BL bl91;       /*!< DXF 91 (29) */
  BITCODE_BD length;
  BITCODE_BD width;
  BITCODE_BD height;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_acsh_wedge_class *_obj = dwg_object_to_ACSH_WEDGE_CLASS (obj);

  CHK_ENTITY_TYPE (_obj, ACSH_WEDGE_CLASS, class_version, BL); // 90
  CHK_ENTITY_TYPE (_obj, ACSH_WEDGE_CLASS, ee_bl98, BL); //33
  CHK_ENTITY_TYPE (_obj, ACSH_WEDGE_CLASS, ee_bl99, BL); //29 
  CHK_SUBCLASS_TYPE (_obj->history_node, ACSH_HistoryNode, bl90, BL); //33
  CHK_SUBCLASS_TYPE (_obj->history_node, ACSH_HistoryNode, bl91, BL);
  // last 16x nums 40-55
  if (!dwg_dynapi_entity_value (_obj, "ACSH_HistoryNode", "trans", &trans, NULL))
    fail ("ACSH_HistoryNode.trans");
  for (int i = 0; i < 16; i++)
    {
      if (trans[i] == _obj->history_node.trans[i]) // catches nan
        ok ("ACSH_HistoryNode.trans[%d]: %f", i, trans[i]);
      else
        fail ("ACSH_HistoryNode.trans[%d]: %f", i, trans[i]);
    }
  CHK_SUBCLASS_CMC (_obj->history_node, ACSH_HistoryNode, color);
  CHK_SUBCLASS_TYPE (_obj->history_node, ACSH_HistoryNode, bl92, BL);
  CHK_SUBCLASS_H (_obj->history_node, ACSH_HistoryNode, h347);

  CHK_ENTITY_TYPE (_obj, ACSH_WEDGE_CLASS, bl90, BL);
  CHK_ENTITY_TYPE (_obj, ACSH_WEDGE_CLASS, bl91, BL);
  CHK_ENTITY_TYPE (_obj, ACSH_WEDGE_CLASS, length, BD);
  CHK_ENTITY_TYPE (_obj, ACSH_WEDGE_CLASS, width, BD);
  CHK_ENTITY_TYPE (_obj, ACSH_WEDGE_CLASS, height, BD);
#endif
}
