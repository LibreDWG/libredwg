// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_ASSOCEXTRUDEDSURFACEACTIONBODY
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
  // AcDbAssocExtrudedSurfaceActionBody
  BITCODE_BL esab_status;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_assocextrudedsurfaceactionbody *_obj = dwg_object_to_ASSOCEXTRUDEDSURFACEACTIONBODY (obj);

  CHK_ENTITY_TYPE (_obj, ASSOCEXTRUDEDSURFACEACTIONBODY, aab_version, BL);
  CHK_ENTITY_TYPE (_obj, ASSOCEXTRUDEDSURFACEACTIONBODY, pab_status, BL);
  CHK_ENTITY_TYPE (_obj, ASSOCEXTRUDEDSURFACEACTIONBODY, pab_l2, BL);
  CHK_ENTITY_TYPE (_obj, ASSOCEXTRUDEDSURFACEACTIONBODY, num_deps, BL);
  CHK_ENTITY_TYPE (_obj, ASSOCEXTRUDEDSURFACEACTIONBODY, pab_l4, BL);
  CHK_ENTITY_TYPE (_obj, ASSOCEXTRUDEDSURFACEACTIONBODY, pab_l5, BL);
  CHK_ENTITY_HV (_obj, ASSOCEXTRUDEDSURFACEACTIONBODY, readdeps, readdeps, num_deps);
  CHK_ENTITY_HV (_obj, ASSOCEXTRUDEDSURFACEACTIONBODY, writedeps, writedeps, num_deps); 
  CHK_ENTITY_TYPE (_obj, ASSOCEXTRUDEDSURFACEACTIONBODY, sab_status, BL);
  CHK_ENTITY_TYPE (_obj, ASSOCEXTRUDEDSURFACEACTIONBODY, sab_b1, B);
  CHK_ENTITY_TYPE (_obj, ASSOCEXTRUDEDSURFACEACTIONBODY, sab_l2, BL);
  CHK_ENTITY_TYPE (_obj, ASSOCEXTRUDEDSURFACEACTIONBODY, sab_b2, B);
  CHK_ENTITY_TYPE (_obj, ASSOCEXTRUDEDSURFACEACTIONBODY, sab_s1, BS);
  CHK_ENTITY_TYPE (_obj, ASSOCEXTRUDEDSURFACEACTIONBODY, pbsab_status, BL);

  CHK_ENTITY_TYPE (_obj, ASSOCEXTRUDEDSURFACEACTIONBODY, esab_status, BL);
#endif
}
