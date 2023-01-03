// unstable
#define DWG_TYPE DWG_TYPE_BLOCKBASEPOINTPARAMETER
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BL i;
  BLOCK1PTPARAMETER_fields;
  BITCODE_3BD pt;
  BITCODE_3BD base_pt;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_blockbasepointparameter *_obj
      = dwg_object_to_BLOCKBASEPOINTPARAMETER (obj);

  CHK_EVALEXPR (BLOCKBASEPOINTPARAMETER);
  // AcDbBlockElement
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKBASEPOINTPARAMETER, name);
  CHK_ENTITY_TYPE (_obj, BLOCKBASEPOINTPARAMETER, be_major, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKBASEPOINTPARAMETER, be_minor, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKBASEPOINTPARAMETER, eed1071, BL);
  // AcDbBlockParameter
  CHK_ENTITY_TYPE (_obj, BLOCKBASEPOINTPARAMETER, show_properties, B);
  CHK_ENTITY_TYPE (_obj, BLOCKBASEPOINTPARAMETER, chain_actions, B);
  // AcDbBlock1PtParameter
  CHK_ENTITY_3RD (_obj, BLOCKBASEPOINTPARAMETER, def_pt);
  CHK_ENTITY_TYPE (_obj, BLOCKBASEPOINTPARAMETER, num_propinfos, BL);
  CHK_ENTITY_MAX (_obj, BLOCKBASEPOINTPARAMETER, num_propinfos, BL, 2);
  BLOCKPARAMETER_PropInfo (prop1) BLOCKPARAMETER_PropInfo (prop2)
      // AcDbBlockBasepointParameter
      CHK_ENTITY_3RD (_obj, BLOCKBASEPOINTPARAMETER, pt);
  CHK_ENTITY_3RD (_obj, BLOCKBASEPOINTPARAMETER, base_pt);
}
