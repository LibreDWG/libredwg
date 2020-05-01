// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_ASSOCREVOLVEDSURFACEACTIONBODY
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
  // AcDbAssocRevolvedSurfaceActionBody
  BITCODE_BL rsab_status;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_assocrevolvedsurfaceactionbody *_obj = dwg_object_to_ASSOCREVOLVEDSURFACEACTIONBODY (obj);

  CHK_ENTITY_TYPE (_obj, ASSOCREVOLVEDSURFACEACTIONBODY, aab_version, BL, aab_version);
  CHK_ENTITY_TYPE (_obj, ASSOCREVOLVEDSURFACEACTIONBODY, pab_status, BL, pab_status);
  CHK_ENTITY_TYPE (_obj, ASSOCREVOLVEDSURFACEACTIONBODY, pab_l2, BL, pab_l2);
  CHK_ENTITY_TYPE (_obj, ASSOCEXTRUDEDSURFACEACTIONBODY, num_deps, BL, num_deps);
  CHK_ENTITY_TYPE (_obj, ASSOCEXTRUDEDSURFACEACTIONBODY, pab_l4, BL, pab_l4);
  CHK_ENTITY_TYPE (_obj, ASSOCEXTRUDEDSURFACEACTIONBODY, pab_l5, BL, pab_l5);
  CHK_ENTITY_HV (_obj, ASSOCEXTRUDEDSURFACEACTIONBODY, readdeps, readdeps, num_deps);
  CHK_ENTITY_HV (_obj, ASSOCEXTRUDEDSURFACEACTIONBODY, writedeps, writedeps, num_deps); 
  CHK_ENTITY_TYPE (_obj, ASSOCREVOLVEDSURFACEACTIONBODY, sab_status, BL, sab_status);
  CHK_ENTITY_TYPE (_obj, ASSOCREVOLVEDSURFACEACTIONBODY, sab_b1, B, sab_b1);
  CHK_ENTITY_TYPE (_obj, ASSOCREVOLVEDSURFACEACTIONBODY, sab_l2, BL, sab_l2);
  CHK_ENTITY_TYPE (_obj, ASSOCREVOLVEDSURFACEACTIONBODY, sab_b2, B, sab_b2);
  CHK_ENTITY_TYPE (_obj, ASSOCREVOLVEDSURFACEACTIONBODY, sab_s1, BS, sab_s1);
  CHK_ENTITY_TYPE (_obj, ASSOCREVOLVEDSURFACEACTIONBODY, pbsab_status, BL, pbsab_status);

  CHK_ENTITY_TYPE (_obj, ASSOCREVOLVEDSURFACEACTIONBODY, rsab_status, BL, rsab_status);
#endif
}
