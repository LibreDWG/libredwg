// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_ASSOCOSNAPPOINTREFACTIONPARAM
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  // AcDbAssocActionParam
  BITCODE_RC unknown;  // 01010101
  BITCODE_B unknown1;  //
  BITCODE_BS status;   // 0-7
  BITCODE_T  name;     // 1 ""
  BITCODE_RS flags;    // 90 0
  // AcDbAssocCompoundActionParam
  BITCODE_BD unknown3; // 40 -1.0
  BITCODE_BS num_actions; // 90 1
  BITCODE_H* actions;
  BITCODE_H writedep;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_assocosnappointrefactionparam *_obj = dwg_object_to_ASSOCOSNAPPOINTREFACTIONPARAM (obj);

  CHK_ENTITY_TYPE (_obj, ASSOCOSNAPPOINTREFACTIONPARAM, unknown, RC);
  CHK_ENTITY_TYPE (_obj, ASSOCOSNAPPOINTREFACTIONPARAM, unknown1, B);
  CHK_ENTITY_TYPE (_obj, ASSOCOSNAPPOINTREFACTIONPARAM, status, BS);
  CHK_ENTITY_MAX (_obj, ASSOCOSNAPPOINTREFACTIONPARAM, status, BS, 7);
  CHK_ENTITY_UTF8TEXT (_obj, ASSOCOSNAPPOINTREFACTIONPARAM, name, name);
  CHK_ENTITY_TYPE (_obj, ASSOCOSNAPPOINTREFACTIONPARAM, flags, RS);
  CHK_ENTITY_TYPE (_obj, ASSOCOSNAPPOINTREFACTIONPARAM, unknown3, BD);
  CHK_ENTITY_TYPE (_obj, ASSOCOSNAPPOINTREFACTIONPARAM, num_actions, BS);
  CHK_ENTITY_MAX (_obj, ASSOCOSNAPPOINTREFACTIONPARAM, num_actions, BS, 1000);
  CHK_ENTITY_HV (_obj, ASSOCOSNAPPOINTREFACTIONPARAM, actions, actions, num_actions);
  CHK_ENTITY_H (_obj, ASSOCOSNAPPOINTREFACTIONPARAM, writedep, writedep);
#endif
}
