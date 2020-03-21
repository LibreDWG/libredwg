// TODO unstable
#define DWG_TYPE DWG_TYPE_ASSOCALIGNEDDIMACTIONBODY
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_BL aab_status;
  BITCODE_BL pab_status;
  BITCODE_BL pab_l2;
  BITCODE_BL pab_l3;
  BITCODE_H  writedep;
  BITCODE_BL pab_l4;
  BITCODE_BL pab_l5;
  BITCODE_BL pab_l6;
  BITCODE_H  readdep;
  BITCODE_BL dcm_status;
  BITCODE_H  d_node;
  BITCODE_H  r_node;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_assocaligneddimactionbody *_obj = dwg_object_to_ASSOCALIGNEDDIMACTIONBODY (obj);

  CHK_ENTITY_TYPE (_obj, ASSOCALIGNEDDIMACTIONBODY, aab_status, BL, aab_status);
  CHK_ENTITY_MAX (_obj, ASSOCALIGNEDDIMACTIONBODY, aab_status, BL, 2);
  CHK_ENTITY_TYPE (_obj, ASSOCALIGNEDDIMACTIONBODY, pab_status, BL, pab_status);
  CHK_ENTITY_TYPE (_obj, ASSOCALIGNEDDIMACTIONBODY, pab_l2, BL, pab_l2);
  CHK_ENTITY_TYPE (_obj, ASSOCALIGNEDDIMACTIONBODY, pab_l3, BL, pab_l3);
  CHK_ENTITY_TYPE (_obj, ASSOCALIGNEDDIMACTIONBODY, pab_l4, BL, pab_l4);
  CHK_ENTITY_TYPE (_obj, ASSOCALIGNEDDIMACTIONBODY, pab_l5, BL, pab_l5);
  CHK_ENTITY_TYPE (_obj, ASSOCALIGNEDDIMACTIONBODY, pab_l6, BL, pab_l6);
  CHK_ENTITY_TYPE (_obj, ASSOCALIGNEDDIMACTIONBODY, dcm_status, BL, dcm_status);
  CHK_ENTITY_H (_obj, ASSOCALIGNEDDIMACTIONBODY, readdep, readdep);
  CHK_ENTITY_H (_obj, ASSOCALIGNEDDIMACTIONBODY, writedep, writedep);
  CHK_ENTITY_H (_obj, ASSOCALIGNEDDIMACTIONBODY, d_node, d_node);
  CHK_ENTITY_H (_obj, ASSOCALIGNEDDIMACTIONBODY, r_node, r_node);
}
