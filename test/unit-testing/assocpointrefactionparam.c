// TODO unstable
#define DWG_TYPE DWG_TYPE_ASSOCPOINTREFACTIONPARAM
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  ASSOCACTIONPARAM_fields;
  /* AssocCompoundActionParam */
  BITCODE_BS class_version;
  BITCODE_BS bs1;
  BITCODE_BL num_params;
  BITCODE_H *params;
  BITCODE_B has_child_param;
  BITCODE_BS child_status;
  BITCODE_BL child_id;
  BITCODE_H child_param;
  BITCODE_H h330_2;
  BITCODE_BL bl2;
  BITCODE_H h330_3;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  // #ifdef DEBUG_CLASSES
  dwg_obj_assocpointrefactionparam *_obj
      = dwg_object_to_ASSOCPOINTREFACTIONPARAM (obj);

  CHK_ENTITY_TYPE (_obj, ASSOCPOINTREFACTIONPARAM, is_r2013, BS);
  CHK_ENTITY_TYPE (_obj, ASSOCPOINTREFACTIONPARAM, aap_version, BL);
  CHK_ENTITY_UTF8TEXT (_obj, ASSOCPOINTREFACTIONPARAM, name);

  CHK_ENTITY_TYPE (_obj, ASSOCPOINTREFACTIONPARAM, class_version, BS);
  CHK_ENTITY_TYPE (_obj, ASSOCPOINTREFACTIONPARAM, bs1, BS);
  CHK_ENTITY_TYPE (_obj, ASSOCPOINTREFACTIONPARAM, num_params, BL);
  CHK_ENTITY_HV (_obj, ASSOCPOINTREFACTIONPARAM, params, num_params);
  CHK_ENTITY_TYPE (_obj, ASSOCPOINTREFACTIONPARAM, has_child_param, B);
  CHK_ENTITY_TYPE (_obj, ASSOCPOINTREFACTIONPARAM, child_status, BS);
  CHK_ENTITY_TYPE (_obj, ASSOCPOINTREFACTIONPARAM, child_id, BL);
  CHK_ENTITY_H (_obj, ASSOCPOINTREFACTIONPARAM, child_param);
  CHK_ENTITY_H (_obj, ASSOCPOINTREFACTIONPARAM, h330_2);
  CHK_ENTITY_TYPE (_obj, ASSOCPOINTREFACTIONPARAM, bl2, BL);
  CHK_ENTITY_H (_obj, ASSOCPOINTREFACTIONPARAM, h330_3);
  // #endif
}
