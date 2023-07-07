// TODO coverage
#define DWG_TYPE DWG_TYPE_BLOCKLOOKUPPARAMETER
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BL i;
  BLOCK1PTPARAMETER_fields;
  BITCODE_T lookup_name; /*!< DXF 305 */
  BITCODE_T lookup_desc; /*!< DXF 306 */
  BITCODE_BL index;      /*!< DXF 94 ?? */
  BITCODE_T unknown_t;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  // #ifdef DEBUG_CLASSES
  dwg_obj_blocklookupparameter *_obj
      = dwg_object_to_BLOCKLOOKUPPARAMETER (obj);

  CHK_EVALEXPR (BLOCKLOOKUPPARAMETER);
  // AcDbBlockElement
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKLOOKUPPARAMETER, name);
  CHK_ENTITY_TYPE (_obj, BLOCKLOOKUPPARAMETER, be_major, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKLOOKUPPARAMETER, be_minor, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKLOOKUPPARAMETER, eed1071, BL);
  // AcDbBlockParameter
  CHK_ENTITY_TYPE (_obj, BLOCKLOOKUPPARAMETER, show_properties, B);
  CHK_ENTITY_TYPE (_obj, BLOCKLOOKUPPARAMETER, chain_actions, B);
  // AcDbBlock1PtParameter
  CHK_ENTITY_3RD (_obj, BLOCKLOOKUPPARAMETER, def_pt);
  CHK_ENTITY_TYPE (_obj, BLOCKLOOKUPPARAMETER, num_propinfos, BL);
  CHK_ENTITY_MAX (_obj, BLOCKLOOKUPPARAMETER, num_propinfos, BL, 2);
  BLOCKPARAMETER_PropInfo (prop1) BLOCKPARAMETER_PropInfo (prop2)
      // AcDbBlockLookupParameter
      CHK_ENTITY_UTF8TEXT (_obj, BLOCKLOOKUPPARAMETER, lookup_name);
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKLOOKUPPARAMETER, lookup_desc);
  CHK_ENTITY_TYPE (_obj, BLOCKLOOKUPPARAMETER, index, BL);
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKLOOKUPPARAMETER, unknown_t);
  // #endif
}
