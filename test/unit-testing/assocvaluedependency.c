// stable, just indxf missing
#define DWG_TYPE DWG_TYPE_ASSOCVALUEDEPENDENCY
#include "tests_common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  Dwg_Object_ASSOCVALUEDEPENDENCY assocdep;
  BITCODE_BL class_version;
  BITCODE_T name;
  Dwg_EvalVariant value;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_assocvaluedependency *_obj
      = dwg_object_to_ASSOCVALUEDEPENDENCY (obj);

  CHK_SUBCLASS_TYPE (_obj->assocdep, ASSOCDEPENDENCY, class_version, BS);
  CHK_SUBCLASS_TYPE (_obj->assocdep, ASSOCDEPENDENCY, status, BL);
  CHK_SUBCLASS_TYPE (_obj->assocdep, ASSOCDEPENDENCY, is_read_dep, B);
  CHK_SUBCLASS_TYPE (_obj->assocdep, ASSOCDEPENDENCY, is_write_dep, B);
  CHK_SUBCLASS_TYPE (_obj->assocdep, ASSOCDEPENDENCY, is_attached_to_object,
                     B);
  CHK_SUBCLASS_TYPE (_obj->assocdep, ASSOCDEPENDENCY,
                     is_delegating_to_owning_action, B);
  CHK_SUBCLASS_TYPE (_obj->assocdep, ASSOCDEPENDENCY, order, BLd);
  CHK_SUBCLASS_H (_obj->assocdep, ASSOCDEPENDENCY, dep_on);
  CHK_SUBCLASS_TYPE (_obj->assocdep, ASSOCDEPENDENCY, has_name, B);
  CHK_SUBCLASS_UTF8TEXT (_obj->assocdep, ASSOCDEPENDENCY, name);
  CHK_SUBCLASS_TYPE (_obj->assocdep, ASSOCDEPENDENCY, depbodyid, BLd);
  CHK_SUBCLASS_H (_obj->assocdep, ASSOCDEPENDENCY, readdep);
  CHK_SUBCLASS_H (_obj->assocdep, ASSOCDEPENDENCY, dep_body);
  CHK_SUBCLASS_H (_obj->assocdep, ASSOCDEPENDENCY, node);
  // AcDbAssocValueDependency
  CHK_ENTITY_TYPE (_obj, ASSOCVALUEDEPENDENCY, class_version, BL);
  CHK_ENTITY_UTF8TEXT (_obj, ASSOCVALUEDEPENDENCY, name);
  CHK_SUBCLASS_TYPE (_obj->value, EvalVariant, code, BSd);
}
