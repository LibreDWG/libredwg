#define DWG_TYPE DWG_TYPE_ACSH_TORUS_CLASS
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BD *trans;
  BITCODE_BL major;
  BITCODE_BL minor;
  BITCODE_BD major_radius; /*!< DXF 40 */
  BITCODE_BD minor_radius; /*!< DXF 41 */

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_acsh_torus_class *_obj = dwg_object_to_ACSH_TORUS_CLASS (obj);

  CHK_EVALEXPR (ACSH_TORUS_CLASS);
  CHK_ACSH_HISTORYNODE ();

  CHK_ENTITY_TYPE (_obj, ACSH_TORUS_CLASS, major, BL);
  CHK_ENTITY_TYPE (_obj, ACSH_TORUS_CLASS, minor, BL);
  CHK_ENTITY_TYPE (_obj, ACSH_TORUS_CLASS, major_radius, BD);
  CHK_ENTITY_TYPE (_obj, ACSH_TORUS_CLASS, minor_radius, BD);
}
