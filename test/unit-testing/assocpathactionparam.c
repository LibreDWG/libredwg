// TODO unstable
#define DWG_TYPE DWG_TYPE_ASSOCPATHACTIONPARAM
#include "tests_common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  ASSOCACTIONPARAM_fields;
  ASSOCCOMPOUNDACTIONPARAM_fields;
  /* AcDbAssocPathActionParam */
  BITCODE_BL apap_version; /*!< DXF 90 */

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  // #ifdef DEBUG_CLASSES
  dwg_obj_assocpathactionparam *_obj
      = dwg_object_to_ASSOCPATHACTIONPARAM (obj);

  CHK_ENTITY_TYPE (_obj, ASSOCPATHACTIONPARAM, is_r2013, BS);
  CHK_ENTITY_TYPE (_obj, ASSOCPATHACTIONPARAM, aap_version, BL);
  CHK_ENTITY_UTF8TEXT (_obj, ASSOCPATHACTIONPARAM, name);

  CHK_ENTITY_TYPE (_obj, ASSOCPATHACTIONPARAM, apap_version, BL);
  // #endif
}
