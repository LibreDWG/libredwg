// TODO unstable
#define DWG_TYPE DWG_TYPE_ASSOCPATCHSURFACEACTIONBODY
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  ASSOCPATHBASEDSURFACEACTIONBODY_fields;
  BITCODE_H *deps;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_assocpatchsurfaceactionbody *_obj
      = dwg_object_to_ASSOCPATCHSURFACEACTIONBODY (obj);

  // ASSOCACTIONBODY
  CHK_ENTITY_TYPE (_obj, ASSOCPATCHSURFACEACTIONBODY, aab_version, BL);
  CHK_ENTITY_MAX (_obj, ASSOCPATCHSURFACEACTIONBODY, aab_version, BL, 2);
  CHK_ASSOCPARAMBASEDACTIONBODY (ASSOCPATCHSURFACEACTIONBODY);
  // AcDbAssocSurfaceActionBody
  CHK_SUBCLASS_TYPE (_obj->sab, ASSOCSURFACEACTIONBODY, version, BL);
  CHK_SUBCLASS_TYPE (_obj->sab, ASSOCSURFACEACTIONBODY, is_semi_assoc, B);
  CHK_SUBCLASS_TYPE (_obj->sab, ASSOCSURFACEACTIONBODY, l2, BL);
  CHK_SUBCLASS_TYPE (_obj->sab, ASSOCSURFACEACTIONBODY, is_semi_ovr, B);
  CHK_SUBCLASS_TYPE (_obj->sab, ASSOCSURFACEACTIONBODY, grip_status, BS);
  CHK_SUBCLASS_H (_obj->sab, ASSOCSURFACEACTIONBODY, assocdep);
  // AcDbAssocPathBasedSurfaceActionBody
  CHK_ENTITY_TYPE (_obj, ASSOCPATCHSURFACEACTIONBODY, pbsab_status, BL);
}
