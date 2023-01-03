// TODO unstable
#define DWG_TYPE DWG_TYPE_ASSOCOFFSETSURFACEACTIONBODY
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  ASSOCPATHBASEDSURFACEACTIONBODY_fields;
  BITCODE_H *deps;
  BITCODE_BL class_version;
  BITCODE_B b1;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_assocoffsetsurfaceactionbody *_obj
      = dwg_object_to_ASSOCOFFSETSURFACEACTIONBODY (obj);

  // ASSOCACTIONBODY
  CHK_ENTITY_TYPE (_obj, ASSOCOFFSETSURFACEACTIONBODY, aab_version, BL);
  CHK_ENTITY_MAX (_obj, ASSOCOFFSETSURFACEACTIONBODY, aab_version, BL, 2);
  CHK_ASSOCPARAMBASEDACTIONBODY (ASSOCOFFSETSURFACEACTIONBODY);
  // AcDbAssocSurfaceActionBody
  CHK_SUBCLASS_TYPE (_obj->sab, ASSOCSURFACEACTIONBODY, version, BL);
  CHK_SUBCLASS_TYPE (_obj->sab, ASSOCSURFACEACTIONBODY, is_semi_assoc, B);
  CHK_SUBCLASS_TYPE (_obj->sab, ASSOCSURFACEACTIONBODY, l2, BL);
  CHK_SUBCLASS_TYPE (_obj->sab, ASSOCSURFACEACTIONBODY, is_semi_ovr, B);
  CHK_SUBCLASS_TYPE (_obj->sab, ASSOCSURFACEACTIONBODY, grip_status, BS);
  CHK_SUBCLASS_H (_obj->sab, ASSOCSURFACEACTIONBODY, assocdep);
  // AcDbAssocPathBasedSurfaceActionBody
  CHK_ENTITY_TYPE (_obj, ASSOCOFFSETSURFACEACTIONBODY, pbsab_status, BL);

  CHK_ENTITY_TYPE (_obj, ASSOCOFFSETSURFACEACTIONBODY, class_version, BL);
  CHK_ENTITY_TYPE (_obj, ASSOCOFFSETSURFACEACTIONBODY, b1, B);
}
