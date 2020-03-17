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
  BITCODE_BS status;   // 90: 0 uptodate, 1 changed_directly, 2 changed_transitive,
                       // 3 ChangedNoDifference, 4 FailedToEvaluate, 5 Erased, 6 Suppressed
                       // 7 Unresolved
  BITCODE_T  name;     // 1 ""
  BITCODE_RS flags;    // 90 0
  // AcDbAssocCompoundActionParam
  BITCODE_BD unknown3; // 40 -1.0
  BITCODE_BS num_params; // 90 1
  BITCODE_H actionparam;  // 330
  BITCODE_H writedep; // 360

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_assocosnappointrefactionparam *_obj = dwg_object_to_ASSOCOSNAPPOINTREFACTIONPARAM (obj);

  CHK_ENTITY_TYPE (_obj, ASSOCOSNAPPOINTREFACTIONPARAM, unknown, RC, unknown);
  CHK_ENTITY_TYPE (_obj, ASSOCOSNAPPOINTREFACTIONPARAM, unknown1, B, unknown1);
  CHK_ENTITY_TYPE (_obj, ASSOCOSNAPPOINTREFACTIONPARAM, status, BS, status);
  CHK_ENTITY_MAX (_obj, ASSOCOSNAPPOINTREFACTIONPARAM, status, BS, 7);
  CHK_ENTITY_UTF8TEXT (_obj, ASSOCOSNAPPOINTREFACTIONPARAM, name, name);
  CHK_ENTITY_TYPE (_obj, ASSOCOSNAPPOINTREFACTIONPARAM, flags, RS, flags);
  CHK_ENTITY_TYPE (_obj, ASSOCOSNAPPOINTREFACTIONPARAM, unknown3, BD, unknown3);
  CHK_ENTITY_TYPE (_obj, ASSOCOSNAPPOINTREFACTIONPARAM, num_params, BS, num_params);
  CHK_ENTITY_H (_obj, ASSOCOSNAPPOINTREFACTIONPARAM, actionparam, actionparam);
  CHK_ENTITY_H (_obj, ASSOCOSNAPPOINTREFACTIONPARAM, writedep, writedep);
#endif
}
