// TODO unstable
#define DWG_TYPE DWG_TYPE_ASSOCTRIMSURFACEACTIONBODY
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  ASSOCPATHBASEDSURFACEACTIONBODY_fields;
  BITCODE_H *deps;
  BITCODE_BL class_version;
  BITCODE_B b1;
  BITCODE_B b2;
  BITCODE_BD distance;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_assoctrimsurfaceactionbody *_obj
      = dwg_object_to_ASSOCTRIMSURFACEACTIONBODY (obj);

  // ASSOCACTIONBODY
  CHK_ENTITY_TYPE (_obj, ASSOCTRIMSURFACEACTIONBODY, aab_version, BL);
  CHK_ENTITY_MAX (_obj, ASSOCTRIMSURFACEACTIONBODY, aab_version, BL, 2);
  CHK_ASSOCPARAMBASEDACTIONBODY (ASSOCTRIMSURFACEACTIONBODY);
  // AcDbAssocSurfaceActionBody
  CHK_SUBCLASS_TYPE (_obj->sab, ASSOCSURFACEACTIONBODY, version, BL);
  CHK_SUBCLASS_TYPE (_obj->sab, ASSOCSURFACEACTIONBODY, is_semi_assoc, B);
  CHK_SUBCLASS_TYPE (_obj->sab, ASSOCSURFACEACTIONBODY, l2, BL);
  CHK_SUBCLASS_TYPE (_obj->sab, ASSOCSURFACEACTIONBODY, is_semi_ovr, B);
  CHK_SUBCLASS_TYPE (_obj->sab, ASSOCSURFACEACTIONBODY, grip_status, BS);
  CHK_SUBCLASS_H (_obj->sab, ASSOCSURFACEACTIONBODY, assocdep);
  // AcDbAssocPathBasedSurfaceActionBody
  CHK_ENTITY_TYPE (_obj, ASSOCTRIMSURFACEACTIONBODY, pbsab_status, BL);

  CHK_ENTITY_TYPE (_obj, ASSOCTRIMSURFACEACTIONBODY, class_version, BL);
  CHK_ENTITY_TYPE (_obj, ASSOCTRIMSURFACEACTIONBODY, b1, B);
  CHK_ENTITY_TYPE (_obj, ASSOCTRIMSURFACEACTIONBODY, b2, B);
  CHK_ENTITY_TYPE (_obj, ASSOCTRIMSURFACEACTIONBODY, distance, BD);
}
