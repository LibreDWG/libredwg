// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_ASSOCPATHACTIONPARAM
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BS class_version;
  ASSOCACTIONPARAM_fields;
  /* AcDbAssocSingleDependencyActionParam */ 
  BITCODE_BL asdap_class_version; // 0
  BITCODE_H dep;
  /* AssocCompoundActionParam */
  BITCODE_BS class_version;
  BITCODE_BS bs1;
  BITCODE_BL num_params;
  BITCODE_H *params;
  BITCODE_B has_child_param;
  BITCODE_BS child_bs2;
  BITCODE_BL child_bl1;
  BITCODE_H child_param;
  BITCODE_H h330_2;
  BITCODE_BL bl2;
  BITCODE_H h330_3;
  /* AcDbAssocPathActionParam */
  BITCODE_BL status;	/*!< DXF 90 */

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_assocpathactionparam *_obj = dwg_object_to_ASSOCPATHACTIONPARAM (obj);

  CHK_ENTITY_TYPE (_obj, ASSOCPATHACTIONPARAM, is_r2013, BS);
  CHK_ENTITY_TYPE (_obj, ASSOCPATHACTIONPARAM, aap_version, BL);
  CHK_ENTITY_UTF8TEXT (_obj, ASSOCPATHACTIONPARAM, name);

  CHK_ENTITY_TYPE (_obj, ASSOCPATHACTIONPARAM, asdap_class_version, BL);
  CHK_ENTITY_H    (_obj, ASSOCPATHACTIONPARAM, dep);
  CHK_ENTITY_TYPE (_obj, ASSOCPATHACTIONPARAM, class_version, BS);
  CHK_ENTITY_TYPE (_obj, ASSOCPATHACTIONPARAM, bs1, BS);
  CHK_ENTITY_TYPE (_obj, ASSOCPATHACTIONPARAM, num_params, BL);
  CHK_ENTITY_HV   (_obj, ASSOCPATHACTIONPARAM, params, num_params);
  CHK_ENTITY_TYPE (_obj, ASSOCPATHACTIONPARAM, has_child_param, B);
  CHK_ENTITY_TYPE (_obj, ASSOCPATHACTIONPARAM, child_bs2, BS);
  CHK_ENTITY_TYPE (_obj, ASSOCPATHACTIONPARAM, child_bl1, BL);
  CHK_ENTITY_H    (_obj, ASSOCPATHACTIONPARAM, child_param);
  CHK_ENTITY_H    (_obj, ASSOCPATHACTIONPARAM, h330_1);
  CHK_ENTITY_TYPE (_obj, ASSOCPATHACTIONPARAM, bl2, BL);
  CHK_ENTITY_H    (_obj, ASSOCPATHACTIONPARAM, h330_3);

  CHK_ENTITY_TYPE (_obj, ASSOCPATHACTIONPARAM, status, BL);
#endif
}
