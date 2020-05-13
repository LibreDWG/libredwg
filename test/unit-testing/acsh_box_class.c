// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_ACSH_BOX_CLASS
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
  BITCODE_CMC color;      /*!< DXF 62 */
  BITCODE_B  shhn_b92;    /*!< DXF 92 */
  BITCODE_BL shhn_bl347;  /*!< DXF 347 */
  // AcDbShPrimitive
  // AcDbShBox
  BITCODE_BL shb_bl90;       /*!< DXF 90 (33) */
  BITCODE_BL shb_bl91;       /*!< DXF 91 (29) */
  BITCODE_BD shb_bd40;       /*!< DXF 40 1300.0 (length?) */
  BITCODE_BD shb_bd41;       /*!< DXF 41 20.0 (width?) */
  BITCODE_BD shb_bd42;       /*!< DXF 42 420.0 (height?) */

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_acsh_box_class *_obj = dwg_object_to_ACSH_BOX_CLASS (obj);

  CHK_ENTITY_TYPE (_obj, ACSH_BOX_CLASS, class_version, BL); // 90
  CHK_ENTITY_TYPE (_obj, ACSH_BOX_CLASS, ee_bl98, BL); //33
  CHK_ENTITY_TYPE (_obj, ACSH_BOX_CLASS, ee_bl99, BL); //29
  CHK_ENTITY_TYPE (_obj, ACSH_BOX_CLASS, shhn_bl90, BL); //33
  CHK_ENTITY_TYPE (_obj, ACSH_BOX_CLASS, shhn_bl91, BL); //29
  // last 16x nums 40-55
  if (!dwg_dynapi_entity_value (_obj, "ACSH_BOX_CLASS", "shhn_pts", &shhn_pts, NULL))
    fail ("ACSH_BOX_CLASS.shhn_pts");
  for (int i = 0; i < 16; i++)
    {
      ok ("ACSH_BOX_CLASS.shhn_pts[%d]: %f", i, shhn_pts[i]);
    }
  CHK_ENTITY_CMC (_obj, ACSH_BOX_CLASS, color, color);
  CHK_ENTITY_TYPE (_obj, ACSH_BOX_CLASS, shhn_b92, B);
  CHK_ENTITY_TYPE (_obj, ACSH_BOX_CLASS, shhn_bl347, BL);
  CHK_ENTITY_TYPE (_obj, ACSH_BOX_CLASS, shb_bl90, BL);
  CHK_ENTITY_TYPE (_obj, ACSH_BOX_CLASS, shb_bl91, BL);
  CHK_ENTITY_TYPE (_obj, ACSH_BOX_CLASS, shb_bd40, BD);       /*!< DXF 40 1300.0 (length?) */
  CHK_ENTITY_TYPE (_obj, ACSH_BOX_CLASS, shb_bd41, BD);       /*!< DXF 41 20.0 (width?) */
  CHK_ENTITY_TYPE (_obj, ACSH_BOX_CLASS, shb_bd42, BD);       /*!< DXF 42 420.0 (height?) */
#endif
}
