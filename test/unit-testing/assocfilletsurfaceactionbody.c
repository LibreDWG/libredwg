// TODO unstable
#define DWG_TYPE DWG_TYPE_ASSOCFILLETSURFACEACTIONBODY
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  ASSOCPATHBASEDSURFACEACTIONBODY_fields;
  BITCODE_H *deps;
  // AcDbAssocFilletSurfaceActionBody
  BITCODE_BL class_version;
  BITCODE_BS status;
  BITCODE_2RD pt1, pt2;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_assocfilletsurfaceactionbody *_obj
      = dwg_object_to_ASSOCFILLETSURFACEACTIONBODY (obj);

  // ASSOCACTIONBODY
  CHK_ENTITY_TYPE (_obj, ASSOCFILLETSURFACEACTIONBODY, aab_version, BL);
  CHK_ENTITY_MAX (_obj, ASSOCFILLETSURFACEACTIONBODY, aab_version, BL, 2);
  CHK_ASSOCPARAMBASEDACTIONBODY (ASSOCFILLETSURFACEACTIONBODY);
  // AcDbAssocSurfaceActionBody
  CHK_SUBCLASS_TYPE (_obj->sab, ASSOCSURFACEACTIONBODY, version, BL);
  CHK_SUBCLASS_TYPE (_obj->sab, ASSOCSURFACEACTIONBODY, is_semi_assoc, B);
  CHK_SUBCLASS_TYPE (_obj->sab, ASSOCSURFACEACTIONBODY, l2, BL);
  CHK_SUBCLASS_TYPE (_obj->sab, ASSOCSURFACEACTIONBODY, is_semi_ovr, B);
  CHK_SUBCLASS_TYPE (_obj->sab, ASSOCSURFACEACTIONBODY, grip_status, BS);
  CHK_SUBCLASS_H (_obj->sab, ASSOCSURFACEACTIONBODY, assocdep);
  // AcDbAssocPathBasedSurfaceActionBody
  CHK_ENTITY_TYPE (_obj, ASSOCFILLETSURFACEACTIONBODY, pbsab_status, BL);

  CHK_ENTITY_TYPE (_obj, ASSOCFILLETSURFACEACTIONBODY, class_version, BL);
  CHK_ENTITY_TYPE (_obj, ASSOCFILLETSURFACEACTIONBODY, status, BS);
  CHK_ENTITY_2RD (_obj, ASSOCFILLETSURFACEACTIONBODY, pt1);
  CHK_ENTITY_2RD (_obj, ASSOCFILLETSURFACEACTIONBODY, pt2);
}
