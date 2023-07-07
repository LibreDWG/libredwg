// TODO unstable
#define DWG_TYPE DWG_TYPE_ASSOCVERTEXACTIONPARAM
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
  BITCODE_3BD pt;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  // #ifdef DEBUG_CLASSES
  dwg_obj_assocvertexactionparam *_obj
      = dwg_object_to_ASSOCVERTEXACTIONPARAM (obj);

  CHK_ENTITY_TYPE (_obj, ASSOCVERTEXACTIONPARAM, is_r2013, BS);
  CHK_ENTITY_TYPE (_obj, ASSOCVERTEXACTIONPARAM, aap_version, BL);
  CHK_ENTITY_UTF8TEXT (_obj, ASSOCVERTEXACTIONPARAM, name);

  CHK_ENTITY_TYPE (_obj, ASSOCVERTEXACTIONPARAM, asdap_class_version, BL);
  CHK_ENTITY_H (_obj, ASSOCVERTEXACTIONPARAM, dep);
  CHK_ENTITY_TYPE (_obj, ASSOCVERTEXACTIONPARAM, class_version, BL);
  CHK_ENTITY_3RD (_obj, ASSOCVERTEXACTIONPARAM, pt);
  // #endif
}
