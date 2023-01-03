#define DWG_TYPE DWG_TYPE_ASSOCDIMDEPENDENCYBODY
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BS adb_version;
  BITCODE_BS dimbase_version;
  BITCODE_T name;
  BITCODE_BS class_version;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_assocdimdependencybody *_obj
      = dwg_object_to_ASSOCDIMDEPENDENCYBODY (obj);

  // AcDbAssocDependencyBody:
  CHK_ENTITY_TYPE (_obj, ASSOCDIMDEPENDENCYBODY, adb_version, BL);
  CHK_ENTITY_MAX (_obj, ASSOCDIMDEPENDENCYBODY, adb_version, BL, 1);
  // AcDbImpAssocDimDependencyBodyBase
  CHK_ENTITY_TYPE (_obj, ASSOCDIMDEPENDENCYBODY, dimbase_version, BL);
  CHK_ENTITY_MAX (_obj, ASSOCDIMDEPENDENCYBODY, dimbase_version, BL, 1);
  CHK_ENTITY_UTF8TEXT (_obj, ASSOCDIMDEPENDENCYBODY, name);
  // AcDbAssocDimDependencyBody
  CHK_ENTITY_TYPE (_obj, ASSOCDIMDEPENDENCYBODY, class_version, BL);
  CHK_ENTITY_MAX (_obj, ASSOCDIMDEPENDENCYBODY, class_version, BL, 1);
}
