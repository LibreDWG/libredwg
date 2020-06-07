// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_ACSH_EXTRUSION_CLASS
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  // AcDbEvalExpr
  BITCODE_BL class_version; // 90
  BITCODE_BL ee_bl98; //33
  BITCODE_BL ee_bl99; //29
  Dwg_ACSH_HistoryNode history_node;
  BITCODE_BD* trans;
  // AcDbShSweepBase
  BITCODE_BL bl90;       /*!< DXF 90 */
  BITCODE_BL bl91;       /*!< DXF 91 */
  BITCODE_3BD direction;  /*!< DXF 10 */
  BITCODE_BL bl92;       /*!< DXF 92 */
  BITCODE_BL shsw_text_size;  /*!< DXF 90 */
  BITCODE_T shsw_text;       /*!< DXF 310 */
  BITCODE_BL shsw_bl93;       /*!< DXF 93 */
  BITCODE_BL shsw_text2_size; /*!< DXF 90 */
  BITCODE_T shsw_text2;      /*!< DXF 310 */
  BITCODE_BD draft_angle;       /*!< DXF 42 0.0 */
  BITCODE_BD start_draft_dist;  /*!< DXF 43 0.0 */
  BITCODE_BD end_draft_dist;    /*!< DXF 44 0.0 */
  BITCODE_BD scale_factor;      /*!< DXF 45 1.0 */
  BITCODE_BD twist_angle;       /*!< DXF 48 0.0 */
  BITCODE_BD align_angle;       /*!< DXF 49 0.0 */
  BITCODE_BD* sweepentity_transform; /*!< DXF 46 16x */
  BITCODE_BD* pathentity_transform;  /*!< DXF 47 16x */
  BITCODE_RC align_option;      /*!< DXF 70 2 */
  BITCODE_RC miter_option;      /*!< DXF 71 2 */
  BITCODE_B has_align_start;    /*!< DXF 290 1 */
  BITCODE_B bank;       /*!< DXF 292 1 */
  BITCODE_B check_intersections; /*!< DXF 293 0 */
  BITCODE_B shsw_b294;  /*!< DXF 294  1 */
  BITCODE_B shsw_b295;  /*!< DXF 295  1 */
  BITCODE_B shsw_b296;  /*!< DXF 296  1 */
  BITCODE_3BD pt2;      /*!< DXF 11 0,0,0 */
  // AcDbShExtrusion

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_acsh_extrusion_class *_obj = dwg_object_to_ACSH_EXTRUSION_CLASS (obj);

  CHK_ENTITY_TYPE (_obj, ACSH_EXTRUSION_CLASS, class_version, BL); // 90
  CHK_ENTITY_TYPE (_obj, ACSH_EXTRUSION_CLASS, ee_bl98, BL); //33
  CHK_ENTITY_TYPE (_obj, ACSH_EXTRUSION_CLASS, ee_bl99, BL); //29
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

  CHK_ENTITY_3RD (_obj, ACSH_EXTRUSION_CLASS, direction);         /*!< DXF 10 */
  CHK_ENTITY_TYPE (_obj, ACSH_EXTRUSION_CLASS, bl92, BL);       /*!< DXF 92 */
  CHK_ENTITY_TYPE (_obj, ACSH_EXTRUSION_CLASS, shsw_text_size, BL);  /*!< DXF 90 */
  //if (!dwg_dynapi_entity_value (_obj, "ACSH_EXTRUSION_CLASS", "shsw_text", &shsw_text, NULL))
  //  fail ("ACSH_EXTRUSION_CLASS.shsw_text");
  CHK_ENTITY_UTF8TEXT (_obj, ACSH_EXTRUSION_CLASS, shsw_text);       /*!< DXF 310 */
  CHK_ENTITY_TYPE (_obj, ACSH_EXTRUSION_CLASS, shsw_bl93, BL);       /*!< DXF 93 */
  CHK_ENTITY_TYPE (_obj, ACSH_EXTRUSION_CLASS, shsw_text2_size, BL); /*!< DXF 90 */
  //if (!dwg_dynapi_entity_value (_obj, "ACSH_EXTRUSION_CLASS", "shsw_text2", &shsw_text2, NULL))
  //  fail ("ACSH_EXTRUSION_CLASS.shsw_text");
  CHK_ENTITY_UTF8TEXT (_obj, ACSH_EXTRUSION_CLASS, shsw_text2);      /*!< DXF 310 */
  CHK_ENTITY_TYPE (_obj, ACSH_EXTRUSION_CLASS, draft_angle, BD);       /*!< DXF 42 0.0 */
  CHK_ENTITY_TYPE (_obj, ACSH_EXTRUSION_CLASS, start_draft_dist, BD);  /*!< DXF 43 0.0 */
  CHK_ENTITY_TYPE (_obj, ACSH_EXTRUSION_CLASS, end_draft_dist, BD);    /*!< DXF 44 0.0 */
  CHK_ENTITY_TYPE (_obj, ACSH_EXTRUSION_CLASS, scale_factor, BD);      /*!< DXF 45 1.0 */
  CHK_ENTITY_TYPE (_obj, ACSH_EXTRUSION_CLASS, twist_angle, BD);       /*!< DXF 48 0.0 */
  CHK_ENTITY_TYPE (_obj, ACSH_EXTRUSION_CLASS, align_angle, BD);       /*!< DXF 49 0.0 */
  if (!dwg_dynapi_entity_value (_obj, "ACSH_EXTRUSION_CLASS",
                                "sweepentity_transform",
                                &sweepentity_transform, NULL))
    fail ("ACSH_EXTRUSION_CLASS.sweepentity_transform");
  for (int i = 0; i < 16; i++)
    {
      ok ("ACSH_EXTRUSION_CLASS.sweepentity_transform[%d]: %f", i,
          sweepentity_transform[i]);
    }
  if (!dwg_dynapi_entity_value (_obj, "ACSH_EXTRUSION_CLASS",
                                "pathentity_transform",
                                &pathentity_transform, NULL))
    fail ("ACSH_EXTRUSION_CLASS.pathentity_transform");
  for (int i = 0; i < 16; i++)
    {
      ok ("ACSH_EXTRUSION_CLASS.pathentity_transform[%d]: %f", i,
          sweepentity_transform[i]);
    }
  CHK_ENTITY_TYPE (_obj, ACSH_EXTRUSION_CLASS, align_option, RC);      /*!< DXF 70 2 */
  CHK_ENTITY_TYPE (_obj, ACSH_EXTRUSION_CLASS, miter_option, RC);      /*!< DXF 71 2 */
  CHK_ENTITY_TYPE (_obj, ACSH_EXTRUSION_CLASS, has_align_start, B);    /*!< DXF 290 1 */
  CHK_ENTITY_TYPE (_obj, ACSH_EXTRUSION_CLASS, bank, B);       /*!< DXF 292 1 */
  CHK_ENTITY_TYPE (_obj, ACSH_EXTRUSION_CLASS, check_intersections, B); /*!< DXF 293 0 */
  CHK_ENTITY_TYPE (_obj, ACSH_EXTRUSION_CLASS, shsw_b294, B);  /*!< DXF 294  1 */
  CHK_ENTITY_TYPE (_obj, ACSH_EXTRUSION_CLASS, shsw_b295, B);  /*!< DXF 295  1 */
  CHK_ENTITY_TYPE (_obj, ACSH_EXTRUSION_CLASS, shsw_b296, B);  /*!< DXF 296  1 */
  CHK_ENTITY_3RD (_obj, ACSH_EXTRUSION_CLASS, pt2);      /*!< DXF 11 0,0,0 */
#endif
}
