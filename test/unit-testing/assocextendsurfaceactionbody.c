#define DWG_TYPE DWG_TYPE_ASSOCEXTENDSURFACEACTIONBODY
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  ASSOCPATHBASEDSURFACEACTIONBODY_fields;
  BITCODE_H *deps;
  // AcDbAssocExtendSurfaceActionBody
  BITCODE_BL class_version;
  BITCODE_RC option;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_assocextendsurfaceactionbody *_obj
      = dwg_object_to_ASSOCEXTENDSURFACEACTIONBODY (obj);

  // ASSOCACTIONBODY:
  CHK_ENTITY_TYPE (_obj, ASSOCEXTENDSURFACEACTIONBODY, aab_version, BL);
  CHK_ASSOCPARAMBASEDACTIONBODY (ASSOCEXTENDSURFACEACTIONBODY);
  // AcDbAssocSurfaceActionBody
  CHK_SUBCLASS_TYPE (_obj->sab, ASSOCSURFACEACTIONBODY, version, BL);
  CHK_SUBCLASS_TYPE (_obj->sab, ASSOCSURFACEACTIONBODY, is_semi_assoc, B);
  CHK_SUBCLASS_TYPE (_obj->sab, ASSOCSURFACEACTIONBODY, l2, BL);
  CHK_SUBCLASS_TYPE (_obj->sab, ASSOCSURFACEACTIONBODY, is_semi_ovr, B);
  CHK_SUBCLASS_TYPE (_obj->sab, ASSOCSURFACEACTIONBODY, grip_status, BS);
  CHK_SUBCLASS_H (_obj->sab, ASSOCSURFACEACTIONBODY, assocdep);
  // AcDbAssocPathBasedSurfaceActionBody
  CHK_ENTITY_TYPE (_obj, ASSOCEXTENDSURFACEACTIONBODY, pbsab_status, BL);

  CHK_ENTITY_TYPE (_obj, ASSOCEXTENDSURFACEACTIONBODY, class_version, BL);
  CHK_ENTITY_TYPE (_obj, ASSOCEXTENDSURFACEACTIONBODY, option, RC);
}
