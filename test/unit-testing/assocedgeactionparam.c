// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_ASSOCEDGEACTIONPARAM
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  ASSOCACTIONPARAM_fields;
  /* AcDbAssocSingleDependencyActionParam */
  BITCODE_BL asdap_class_version; // 0
  BITCODE_H dep;
  /* AcDbAssocEdgeActionParam */
  BITCODE_BL class_version;
  BITCODE_H param;
  BITCODE_B has_action;
  BITCODE_BL action_type;
  BITCODE_H subent;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_assocedgeactionparam *_obj
      = dwg_object_to_ASSOCEDGEACTIONPARAM (obj);

  CHK_ENTITY_TYPE (_obj, ASSOCEDGEACTIONPARAM, is_r2013, BS);
  CHK_ENTITY_TYPE (_obj, ASSOCEDGEACTIONPARAM, aap_version, BL);
  CHK_ENTITY_UTF8TEXT (_obj, ASSOCEDGEACTIONPARAM, name);
  CHK_ENTITY_TYPE (_obj, ASSOCEDGEACTIONPARAM, asdap_class_version, BL);
  CHK_ENTITY_H (_obj, ASSOCEDGEACTIONPARAM, dep);

  CHK_ENTITY_TYPE (_obj, ASSOCEDGEACTIONPARAM, class_version, BL);
  CHK_ENTITY_H (_obj, ASSOCEDGEACTIONPARAM, param);
  CHK_ENTITY_TYPE (_obj, ASSOCEDGEACTIONPARAM, has_action, B);
  CHK_ENTITY_TYPE (_obj, ASSOCEDGEACTIONPARAM, action_type, BL);
  CHK_ENTITY_H (_obj, ASSOCEDGEACTIONPARAM, subent);
#endif
}
