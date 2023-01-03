// unstable
#define DWG_TYPE DWG_TYPE_BLOCKFLIPPARAMETER
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BLOCK2PTPARAMETER_fields;
  BITCODE_T flip_label;
  BITCODE_T flip_label_desc;
  BITCODE_T base_state_label;
  BITCODE_T flipped_state_label;
  BITCODE_3BD def_label_pt;
  BITCODE_BL i, bl96;
  BITCODE_T tooltip;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_blockflipparameter *_obj = dwg_object_to_BLOCKFLIPPARAMETER (obj);

  CHK_EVALEXPR (BLOCKFLIPPARAMETER);
  // AcDbBlockElement
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKFLIPPARAMETER, name);
  CHK_ENTITY_TYPE (_obj, BLOCKFLIPPARAMETER, be_major, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKFLIPPARAMETER, be_minor, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKFLIPPARAMETER, eed1071, BL);
  // AcDbBlockParameter
  CHK_ENTITY_TYPE (_obj, BLOCKFLIPPARAMETER, show_properties, B);
  CHK_ENTITY_TYPE (_obj, BLOCKFLIPPARAMETER, chain_actions, B);
  // AcDbBlock2PtParameter
  CHK_ENTITY_3RD (_obj, BLOCKFLIPPARAMETER, def_basept);
  CHK_ENTITY_3RD (_obj, BLOCKFLIPPARAMETER, def_endpt);
  BLOCKPARAMETER_PropInfo (prop1) BLOCKPARAMETER_PropInfo (
      prop2) BLOCKPARAMETER_PropInfo (prop3) BLOCKPARAMETER_PropInfo (prop4)
      CHK_ENTITY_VECTOR_TYPE (_obj, BLOCKFLIPPARAMETER, prop_states, 4, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKFLIPPARAMETER, parameter_base_location, BS);
  CHK_ENTITY_3RD (_obj, BLOCKFLIPPARAMETER, upd_basept);
  CHK_ENTITY_3RD (_obj, BLOCKFLIPPARAMETER, basept);
  CHK_ENTITY_3RD (_obj, BLOCKFLIPPARAMETER, upd_endpt);
  CHK_ENTITY_3RD (_obj, BLOCKFLIPPARAMETER, endpt);

  // AcDbBlockFlipParameter
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKFLIPPARAMETER, flip_label);
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKFLIPPARAMETER, flip_label_desc);
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKFLIPPARAMETER, base_state_label);
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKFLIPPARAMETER, flipped_state_label);
  CHK_ENTITY_3RD (_obj, BLOCKFLIPPARAMETER, def_label_pt);
  CHK_ENTITY_TYPE (_obj, BLOCKFLIPPARAMETER, bl96, BL);
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKFLIPPARAMETER, tooltip);
}
