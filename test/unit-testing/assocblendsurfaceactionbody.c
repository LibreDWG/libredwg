// unstable. no coverage
#define DWG_TYPE DWG_TYPE_ASSOCBLENDSURFACEACTIONBODY
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  ASSOCPATHBASEDSURFACEACTIONBODY_fields;
  BITCODE_H *deps;
  // AcDbAssocBlendSurfaceActionBody
  BITCODE_BL class_version;
  BITCODE_B b1;
  BITCODE_B b2;
  BITCODE_B b3;
  BITCODE_B b4;
  BITCODE_B b5;
  BITCODE_BS blend_options;
  BITCODE_BS bs2;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_assocblendsurfaceactionbody *_obj
      = dwg_object_to_ASSOCBLENDSURFACEACTIONBODY (obj);

  // ASSOCACTIONBODY:
  CHK_ENTITY_TYPE (_obj, ASSOCBLENDSURFACEACTIONBODY, aab_version, BL);
  CHK_ASSOCPARAMBASEDACTIONBODY (ASSOCBLENDSURFACEACTIONBODY);
  // AcDbAssocSurfaceActionBody
  CHK_SUBCLASS_TYPE (_obj->sab, ASSOCSURFACEACTIONBODY, version, BL);
  CHK_SUBCLASS_TYPE (_obj->sab, ASSOCSURFACEACTIONBODY, is_semi_assoc, B);
  CHK_SUBCLASS_TYPE (_obj->sab, ASSOCSURFACEACTIONBODY, l2, BL);
  CHK_SUBCLASS_TYPE (_obj->sab, ASSOCSURFACEACTIONBODY, is_semi_ovr, B);
  CHK_SUBCLASS_TYPE (_obj->sab, ASSOCSURFACEACTIONBODY, grip_status, BS);
  CHK_SUBCLASS_H (_obj->sab, ASSOCSURFACEACTIONBODY, assocdep);
  // AcDbAssocPathBasedSurfaceActionBody
  CHK_ENTITY_TYPE (_obj, ASSOCBLENDSURFACEACTIONBODY, pbsab_status, BL);

  CHK_ENTITY_TYPE (_obj, ASSOCBLENDSURFACEACTIONBODY, class_version, BL);
  CHK_ENTITY_TYPE (_obj, ASSOCBLENDSURFACEACTIONBODY, b1, B);
  CHK_ENTITY_TYPE (_obj, ASSOCBLENDSURFACEACTIONBODY, b2, B);
  CHK_ENTITY_TYPE (_obj, ASSOCBLENDSURFACEACTIONBODY, b3, B);
  CHK_ENTITY_TYPE (_obj, ASSOCBLENDSURFACEACTIONBODY, blend_options, BS);
  CHK_ENTITY_TYPE (_obj, ASSOCBLENDSURFACEACTIONBODY, b4, B);
  CHK_ENTITY_TYPE (_obj, ASSOCBLENDSURFACEACTIONBODY, b5, B);
  CHK_ENTITY_TYPE (_obj, ASSOCBLENDSURFACEACTIONBODY, bs2, BS);
}
