// TODO coverage
#define DWG_TYPE DWG_TYPE_BLOCKPOINTPARAMETER
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BL i;
  BLOCK1PTPARAMETER_fields;
  BITCODE_T position_name;  /* DXF 303 */
  BITCODE_T position_desc;  /* DXF 304 */
  BITCODE_3BD def_label_pt; /* DXF 1011 */

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  // #ifdef DEBUG_CLASSES
  dwg_obj_blockpointparameter *_obj = dwg_object_to_BLOCKPOINTPARAMETER (obj);

  CHK_EVALEXPR (BLOCKPOINTPARAMETER);
  // AcDbBlockElement
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKPOINTPARAMETER, name);
  CHK_ENTITY_TYPE (_obj, BLOCKPOINTPARAMETER, be_major, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKPOINTPARAMETER, be_minor, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKPOINTPARAMETER, eed1071, BL);
  // AcDbBlockParameter
  CHK_ENTITY_TYPE (_obj, BLOCKPOINTPARAMETER, show_properties, B);
  CHK_ENTITY_TYPE (_obj, BLOCKPOINTPARAMETER, chain_actions, B);
  // AcDbBlock1PtParameter
  CHK_ENTITY_3RD (_obj, BLOCKPOINTPARAMETER, def_pt);
  CHK_ENTITY_TYPE (_obj, BLOCKPOINTPARAMETER, num_propinfos, BL);
  CHK_ENTITY_MAX (_obj, BLOCKPOINTPARAMETER, num_propinfos, BL, 2);
  BLOCKPARAMETER_PropInfo (prop1) BLOCKPARAMETER_PropInfo (prop2)
      // AcDbBlockPointParameter
      CHK_ENTITY_UTF8TEXT (_obj, BLOCKPOINTPARAMETER, position_name);
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKPOINTPARAMETER, position_desc);
  CHK_ENTITY_3RD (_obj, BLOCKPOINTPARAMETER, def_label_pt);
  // #endif
}
