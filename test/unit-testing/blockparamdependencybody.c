#define DWG_TYPE DWG_TYPE_BLOCKPARAMDEPENDENCYBODY
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
  // #ifdef DEBUG_CLASSES
  dwg_obj_blockparamdependencybody *_obj
      = dwg_object_to_BLOCKPARAMDEPENDENCYBODY (obj);

  // AcDbAssocDependencyBody:
  CHK_ENTITY_TYPE (_obj, BLOCKPARAMDEPENDENCYBODY, adb_version, BL);
  CHK_ENTITY_MAX (_obj, BLOCKPARAMDEPENDENCYBODY, adb_version, BL, 1);
  // AcDbImpAssocDimDependencyBodyBase
  CHK_ENTITY_TYPE (_obj, BLOCKPARAMDEPENDENCYBODY, dimbase_version, BL);
  CHK_ENTITY_MAX (_obj, BLOCKPARAMDEPENDENCYBODY, dimbase_version, BL, 1);
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKPARAMDEPENDENCYBODY, name);
  // AcDbBlockParameterDependencyBody
  CHK_ENTITY_TYPE (_obj, BLOCKPARAMDEPENDENCYBODY, class_version, BL);
  CHK_ENTITY_MAX (_obj, BLOCKPARAMDEPENDENCYBODY, class_version, BL, 0);
  // #endif
}
