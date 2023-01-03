#define DWG_TYPE DWG_TYPE_ACSH_BOX_CLASS
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  // ACDBEVALEXPR_fields;
  // Dwg_ACSH_HistoryNode history_node;
  BITCODE_BD *trans;
  // AcDbShBox
  BITCODE_BL major;  /*!< DXF 90 (33) */
  BITCODE_BL minor;  /*!< DXF 91 (29) */
  BITCODE_BD length; /*!< DXF 40 1300.0 (length?) */
  BITCODE_BD width;  /*!< DXF 41 20.0 (width?) */
  BITCODE_BD height; /*!< DXF 42 420.0 (height?) */

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_acsh_box_class *_obj = dwg_object_to_ACSH_BOX_CLASS (obj);

  CHK_EVALEXPR (ACSH_BOX_CLASS);
  CHK_ACSH_HISTORYNODE ();

  CHK_ENTITY_TYPE (_obj, ACSH_BOX_CLASS, major, BL);
  CHK_ENTITY_TYPE (_obj, ACSH_BOX_CLASS, minor, BL);
  CHK_ENTITY_TYPE (_obj, ACSH_BOX_CLASS, length, BD);
  CHK_ENTITY_TYPE (_obj, ACSH_BOX_CLASS, width, BD);
  CHK_ENTITY_TYPE (_obj, ACSH_BOX_CLASS, height, BD);
}
