// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_ASSOCSWEPTSURFACEACTIONBODY
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  // AcDbAssocActionBody
  BITCODE_BL aab_version;
  // AcDbAssocParamBasedActionBody
  BITCODE_BL pab_status;
  BITCODE_BL pab_l2;
  BITCODE_BL num_deps;
  BITCODE_H  *writedeps;
  BITCODE_BL pab_l4;
  BITCODE_BL pab_l5;
  BITCODE_H  *readdeps;
  BITCODE_T  *descriptions;
  // AcDbAssocSurfaceActionBody
  BITCODE_BL sab_status;
  BITCODE_B sab_b1;
  BITCODE_BL sab_l2;
  BITCODE_B sab_b2;
  BITCODE_BS sab_s1;
  // AcDbAssocPathBasedSurfaceActionBody
  BITCODE_BL pbsab_status;
  // AcDbAssocSweptSurfaceActionBody
  BITCODE_BL ssab_status;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_assocsweptsurfaceactionbody *_obj = dwg_object_to_ASSOCSWEPTSURFACEACTIONBODY (obj);

  CHK_ENTITY_TYPE (_obj, ASSOCSWEPTSURFACEACTIONBODY, aab_version, BL);
  CHK_ENTITY_TYPE (_obj, ASSOCSWEPTSURFACEACTIONBODY, pab_status, BL);
  CHK_ENTITY_TYPE (_obj, ASSOCSWEPTSURFACEACTIONBODY, pab_l2, BL);
  CHK_ENTITY_TYPE (_obj, ASSOCSWEPTSURFACEACTIONBODY, num_deps, BL);
  CHK_ENTITY_TYPE (_obj, ASSOCSWEPTSURFACEACTIONBODY, pab_l4, BL);
  CHK_ENTITY_TYPE (_obj, ASSOCSWEPTSURFACEACTIONBODY, pab_l5, BL);
  CHK_ENTITY_HV (_obj, ASSOCSWEPTSURFACEACTIONBODY, readdeps, readdeps, num_deps);
  CHK_ENTITY_HV (_obj, ASSOCSWEPTSURFACEACTIONBODY, writedeps, writedeps, num_deps); 
  CHK_ENTITY_TYPE (_obj, ASSOCSWEPTSURFACEACTIONBODY, sab_status, BL);
  CHK_ENTITY_TYPE (_obj, ASSOCSWEPTSURFACEACTIONBODY, sab_b1, B);
  CHK_ENTITY_TYPE (_obj, ASSOCSWEPTSURFACEACTIONBODY, sab_l2, BL);
  CHK_ENTITY_TYPE (_obj, ASSOCSWEPTSURFACEACTIONBODY, sab_b2, B);
  CHK_ENTITY_TYPE (_obj, ASSOCSWEPTSURFACEACTIONBODY, sab_s1, BS);
  CHK_ENTITY_TYPE (_obj, ASSOCSWEPTSURFACEACTIONBODY, pbsab_status, BL);

  CHK_ENTITY_TYPE (_obj, ASSOCSWEPTSURFACEACTIONBODY, ssab_status, BL);
#endif
}
