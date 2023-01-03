// TODO
#define DWG_TYPE DWG_TYPE_ASSOCEDGEFILLETACTIONBODY
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  ASSOCPATHBASEDSURFACEACTIONBODY_fields;
  BITCODE_H *deps;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_assocedgefilletactionbody *_obj
      = dwg_object_to_ASSOCEDGEFILLETACTIONBODY (obj);

  // ASSOCACTIONBODY
  CHK_ENTITY_TYPE (_obj, ASSOCEDGEFILLETACTIONBODY, aab_version, BL);
  CHK_ENTITY_MAX (_obj, ASSOCEDGEFILLETACTIONBODY, aab_version, BL, 2);
  CHK_ASSOCPARAMBASEDACTIONBODY (ASSOCEDGEFILLETACTIONBODY);
  // AcDbAssocSurfaceActionBody
  CHK_SUBCLASS_TYPE (_obj->sab, ASSOCSURFACEACTIONBODY, version, BL);
  CHK_SUBCLASS_TYPE (_obj->sab, ASSOCSURFACEACTIONBODY, is_semi_assoc, B);
  CHK_SUBCLASS_TYPE (_obj->sab, ASSOCSURFACEACTIONBODY, l2, BL);
  CHK_SUBCLASS_TYPE (_obj->sab, ASSOCSURFACEACTIONBODY, is_semi_ovr, B);
  CHK_SUBCLASS_TYPE (_obj->sab, ASSOCSURFACEACTIONBODY, grip_status, BS);
  CHK_SUBCLASS_H (_obj->sab, ASSOCSURFACEACTIONBODY, assocdep);
  // AcDbAssocPathBasedSurfaceActionBody
  CHK_ENTITY_TYPE (_obj, ASSOCEDGEFILLETACTIONBODY, pbsab_status, BL);
#endif
}
