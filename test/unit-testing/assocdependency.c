// TODO unstable
#define DWG_TYPE DWG_TYPE_ASSOCDEPENDENCY
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_BL class_version;
  BITCODE_BL status;
  BITCODE_B isread_dep;
  BITCODE_B iswrite_dep;
  BITCODE_B isobjectstate_dep;
  BITCODE_B unknown_b4;
  BITCODE_BL order;
  BITCODE_B unknown_b5;
  BITCODE_BL depbodyid;
  BITCODE_H owner;
  BITCODE_H readdep;
  BITCODE_H writedep;
  BITCODE_H node;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_assocdependency *_obj = dwg_object_to_ASSOCDEPENDENCY (obj);

  CHK_ENTITY_TYPE (_obj, ASSOCDEPENDENCY, class_version, BL);
  CHK_ENTITY_MAX (_obj, ASSOCDEPENDENCY, class_version, BL, 2);
  CHK_ENTITY_TYPE (_obj, ASSOCDEPENDENCY, status, BL);
  CHK_ENTITY_TYPE (_obj, ASSOCDEPENDENCY, isread_dep, B);
  CHK_ENTITY_TYPE (_obj, ASSOCDEPENDENCY, iswrite_dep, B);
  CHK_ENTITY_TYPE (_obj, ASSOCDEPENDENCY, isobjectstate_dep, B);
  CHK_ENTITY_TYPE (_obj, ASSOCDEPENDENCY, unknown_b4, B);
  CHK_ENTITY_TYPE (_obj, ASSOCDEPENDENCY, order, BL);
  CHK_ENTITY_TYPE (_obj, ASSOCDEPENDENCY, unknown_b5, B);
  CHK_ENTITY_TYPE (_obj, ASSOCDEPENDENCY, depbodyid, BL);
  CHK_ENTITY_H (_obj, ASSOCDEPENDENCY, owner);
  CHK_ENTITY_H (_obj, ASSOCDEPENDENCY, readdep);
  CHK_ENTITY_H (_obj, ASSOCDEPENDENCY, writedep);
  CHK_ENTITY_H (_obj, ASSOCDEPENDENCY, node);
}
