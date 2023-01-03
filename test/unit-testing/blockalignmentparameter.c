#define DWG_TYPE DWG_TYPE_BLOCKALIGNMENTPARAMETER
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BL i;
  BLOCK2PTPARAMETER_fields;
  BITCODE_B align_perpendicular;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_blockalignmentparameter *_obj
      = dwg_object_to_BLOCKALIGNMENTPARAMETER (obj);

  CHK_EVALEXPR (BLOCKALIGNMENTPARAMETER);
  // AcDbBlockElement
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKALIGNMENTPARAMETER, name);
  CHK_ENTITY_TYPE (_obj, BLOCKALIGNMENTPARAMETER, be_major, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKALIGNMENTPARAMETER, be_minor, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKALIGNMENTPARAMETER, eed1071, BL);
  // AcDbBlockParameter
  CHK_ENTITY_TYPE (_obj, BLOCKALIGNMENTPARAMETER, show_properties, B);
  CHK_ENTITY_TYPE (_obj, BLOCKALIGNMENTPARAMETER, chain_actions, B);
  // AcDbBlock2PtParameter
  CHK_ENTITY_3RD (_obj, BLOCKALIGNMENTPARAMETER, def_basept);
  CHK_ENTITY_3RD (_obj, BLOCKALIGNMENTPARAMETER, def_endpt);
  BLOCKPARAMETER_PropInfo (prop1) BLOCKPARAMETER_PropInfo (prop2)
      BLOCKPARAMETER_PropInfo (prop3) BLOCKPARAMETER_PropInfo (prop4)
          CHK_ENTITY_VECTOR_TYPE (_obj, BLOCKALIGNMENTPARAMETER, prop_states,
                                  4, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKALIGNMENTPARAMETER, parameter_base_location, BS);
  // AcDbBlockAlignmentParameter
  CHK_ENTITY_TYPE (_obj, BLOCKALIGNMENTPARAMETER, align_perpendicular, B);
}
