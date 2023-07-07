#define DWG_TYPE DWG_TYPE_ASSOCOBJECTACTIONPARAM
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  ASSOCACTIONPARAM_fields;
  /* AcDbAssocSingleDependencyActionParam */
  BITCODE_BL asdap_class_version; // 0
  BITCODE_H dep;
  /* AcDbAssocObjectActionParam */
  BITCODE_BS class_version; /*!< DXF 90 */

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  // #ifdef DEBUG_CLASSES
  dwg_obj_assocobjectactionparam *_obj
      = dwg_object_to_ASSOCOBJECTACTIONPARAM (obj);

  CHK_ENTITY_TYPE (_obj, ASSOCOBJECTACTIONPARAM, is_r2013, BS);
  CHK_ENTITY_TYPE (_obj, ASSOCOBJECTACTIONPARAM, aap_version, BL);
  CHK_ENTITY_UTF8TEXT (_obj, ASSOCOBJECTACTIONPARAM, name);

  CHK_ENTITY_TYPE (_obj, ASSOCOBJECTACTIONPARAM, asdap_class_version, BL);
  CHK_ENTITY_H (_obj, ASSOCOBJECTACTIONPARAM, dep);

  CHK_ENTITY_TYPE (_obj, ASSOCOBJECTACTIONPARAM, class_version, BS);
  // #endif
}
