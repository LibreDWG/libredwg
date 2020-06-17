// TODO unstable
#define DWG_TYPE DWG_TYPE_ASSOCDEPENDENCY
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BS class_version;
  BITCODE_BL status;
  BITCODE_B is_read_dep;
  BITCODE_B is_write_dep;
  BITCODE_B is_attached_to_object;
  BITCODE_B is_delegating_to_owning_action;
  BITCODE_BLd order;
  BITCODE_H dep_on;
  BITCODE_B has_name;
  BITCODE_T name;
  BITCODE_BLd depbodyid;
  BITCODE_H readdep;
  BITCODE_H dep_body;
  BITCODE_H node;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_assocdependency *_obj = dwg_object_to_ASSOCDEPENDENCY (obj);

  CHK_ENTITY_TYPE (_obj, ASSOCDEPENDENCY, class_version, BL);
  CHK_ENTITY_MAX (_obj, ASSOCDEPENDENCY, class_version, BL, 2);
  CHK_ENTITY_TYPE (_obj, ASSOCDEPENDENCY, status, BL);
  CHK_ENTITY_TYPE (_obj, ASSOCDEPENDENCY, is_read_dep, B);
  CHK_ENTITY_TYPE (_obj, ASSOCDEPENDENCY, is_write_dep, B);
  CHK_ENTITY_TYPE (_obj, ASSOCDEPENDENCY, is_attached_to_object, B);
  CHK_ENTITY_TYPE (_obj, ASSOCDEPENDENCY, is_delegating_to_owning_action, B);
  CHK_ENTITY_TYPE (_obj, ASSOCDEPENDENCY, order, BLd);
  CHK_ENTITY_H (_obj, ASSOCDEPENDENCY, dep_on);
  CHK_ENTITY_TYPE (_obj, ASSOCDEPENDENCY, has_name, B);
  CHK_ENTITY_UTF8TEXT (_obj, ASSOCDEPENDENCY, name);
  CHK_ENTITY_TYPE (_obj, ASSOCDEPENDENCY, depbodyid, BLd);
  CHK_ENTITY_H (_obj, ASSOCDEPENDENCY, readdep);
  CHK_ENTITY_H (_obj, ASSOCDEPENDENCY, dep_body);
  CHK_ENTITY_H (_obj, ASSOCDEPENDENCY, node);
}
