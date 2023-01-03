// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_ACSH_LOFT_CLASS
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  // Dwg_EvalExpr evalexpr;
  // Dwg_ACSH_HistoryNode history_node;
  BITCODE_BD *trans;
  BITCODE_BL major; /*!< DXF 90 */
  BITCODE_BL minor; /*!< DXF 91 */
  BITCODE_BL num_crosssects;
  BITCODE_H *crosssects;
  BITCODE_BL num_guides;
  BITCODE_H *guides;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_acsh_loft_class *_obj = dwg_object_to_ACSH_LOFT_CLASS (obj);

  CHK_EVALEXPR (ACSH_LOFT_CLASS);
  CHK_ACSH_HISTORYNODE ();

  CHK_ENTITY_TYPE (_obj, ACSH_LOFT_CLASS, major, BL);
  CHK_ENTITY_TYPE (_obj, ACSH_LOFT_CLASS, minor, BL);
  CHK_ENTITY_TYPE (_obj, ACSH_LOFT_CLASS, num_crosssects, BL);
  {
    BITCODE_BL i;
    CHK_ENTITY_HV (_obj, ACSH_LOFT_CLASS, crosssects, num_crosssects);
    CHK_ENTITY_TYPE (_obj, ACSH_LOFT_CLASS, num_guides, BL);
    CHK_ENTITY_HV (_obj, ACSH_LOFT_CLASS, guides, num_guides);
  }
#endif
}
