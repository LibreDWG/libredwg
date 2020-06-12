// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_ACSH_CONE_CLASS
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  ACDBEVALEXPR_fields;
  Dwg_ACSH_HistoryNode history_node;
  BITCODE_BD *trans;
  BITCODE_BL major;
  BITCODE_BL minor;
  BITCODE_BD base_radius;     /*!< DXF 40 */
  BITCODE_BD top_major_radius;/*!< DXF 41 */
  BITCODE_BD top_minor_radius;/*!< DXF 42 */
  BITCODE_BD top_x_radius;    /*!< DXF 43 */

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_acsh_cone_class *_obj = dwg_object_to_ACSH_CONE_CLASS (obj);

  CHK_EVALEXPR (ACSH_CONE_CLASS);
  CHK_ACSH_HISTORYNODE();

  CHK_ENTITY_TYPE (_obj, ACSH_CONE_CLASS, major, BL);
  CHK_ENTITY_TYPE (_obj, ACSH_CONE_CLASS, minor, BL);
  CHK_ENTITY_TYPE (_obj, ACSH_CONE_CLASS, base_radius, BD);
  CHK_ENTITY_TYPE (_obj, ACSH_CONE_CLASS, top_major_radius, BD);
  CHK_ENTITY_TYPE (_obj, ACSH_CONE_CLASS, top_minor_radius, BD);
  CHK_ENTITY_TYPE (_obj, ACSH_CONE_CLASS, top_x_radius, BD);
#endif
}
