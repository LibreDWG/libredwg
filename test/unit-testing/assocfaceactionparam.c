// TODO unstable
#define DWG_TYPE DWG_TYPE_ASSOCFACEACTIONPARAM
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  ASSOCACTIONPARAM_fields;
  /* AcDbAssocSingleDependencyActionParam */
  BITCODE_BL asdap_class_version; // 0
  BITCODE_H dep;
  /* AcDbAssocFaceActionParam */
  BITCODE_BL class_version;
  BITCODE_BL index;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  // #ifdef DEBUG_CLASSES
  dwg_obj_assocfaceactionparam *_obj
      = dwg_object_to_ASSOCFACEACTIONPARAM (obj);

  CHK_ENTITY_TYPE (_obj, ASSOCFACEACTIONPARAM, is_r2013, BS);
  CHK_ENTITY_TYPE (_obj, ASSOCFACEACTIONPARAM, aap_version, BL);
  CHK_ENTITY_UTF8TEXT (_obj, ASSOCFACEACTIONPARAM, name);

  CHK_ENTITY_TYPE (_obj, ASSOCFACEACTIONPARAM, asdap_class_version, BL);
  CHK_ENTITY_H (_obj, ASSOCFACEACTIONPARAM, dep);
  CHK_ENTITY_TYPE (_obj, ASSOCFACEACTIONPARAM, class_version, BL);
  CHK_ENTITY_TYPE (_obj, ASSOCFACEACTIONPARAM, index, BL);
  // #endif
}
