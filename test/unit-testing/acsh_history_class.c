// TODO unhandled
#define DWG_TYPE DWG_TYPE_ACSH_HISTORY_CLASS
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  // AcDbEvalExpr
  BITCODE_BL class_version; // 90
  BITCODE_BL ee_bl98; //33
  BITCODE_BL ee_bl99; //29
  // AcDbShHistoryNode
  BITCODE_BL shhn_bl90; //33
  BITCODE_BL shhn_bl91; //29
  BITCODE_BD* shhn_pts; //last 16x nums 40-55
  BITCODE_CMC color; /*!< DXF 62 */
  BITCODE_B  shhn_b92; /*!< DXF 92 */
  BITCODE_BL shhn_bl347; /*!< DXF 347 */

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_acsh_history_class *_obj = dwg_object_to_ACSH_HISTORY_CLASS (obj);

  CHK_ENTITY_TYPE (_obj, ACSH_HISTORY_CLASS, class_version, BL); // 90
  CHK_ENTITY_TYPE (_obj, ACSH_HISTORY_CLASS, ee_bl98, BL); //33
  CHK_ENTITY_TYPE (_obj, ACSH_HISTORY_CLASS, ee_bl99, BL); //29
  CHK_ENTITY_TYPE (_obj, ACSH_HISTORY_CLASS, shhn_bl90, BL); //33
  CHK_ENTITY_TYPE (_obj, ACSH_HISTORY_CLASS, shhn_bl91, BL); //29
  //FIXME
#if 0
  // last 16x nums 40-55
  if (!dwg_dynapi_entity_value (_obj, "ACSH_HISTORY_CLASS", "shhn_pts", &shhn_pts, NULL))
    fail ("ACSH_HISTORY_CLASS.shhn_pts");
  for (int i = 0; i < 16; i++)
    {
      ok ("ACSH_HISTORY_CLASS.shhn_pts[%d]: %f", i, shhn_pts[i]);
    }
  CHK_ENTITY_CMC (_obj, ACSH_HISTORY_CLASS, color);
  CHK_ENTITY_TYPE (_obj, ACSH_HISTORY_CLASS, shhn_b92, B);
  CHK_ENTITY_TYPE (_obj, ACSH_HISTORY_CLASS, shhn_bl347, BL);
  // AcDbShPrimitive
#endif
#endif
}
