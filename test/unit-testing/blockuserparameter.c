// TODO coverage
#define DWG_TYPE DWG_TYPE_BLOCKUSERPARAMETER
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BL i;
  BLOCK1PTPARAMETER_fields;
  BITCODE_BS flag;         /*!< DXF 90 */
  BITCODE_H assocvariable; /*!< DXF 305 */
  BITCODE_T expr;          /*!< DXF 301 */
  Dwg_EvalVariant value;
  BITCODE_BS type; /*!< DXF 170 */

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  // #ifdef DEBUG_CLASSES
  dwg_obj_blockuserparameter *_obj = dwg_object_to_BLOCKUSERPARAMETER (obj);

  CHK_EVALEXPR (BLOCKUSERPARAMETER);
  // AcDbBlockElement
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKUSERPARAMETER, name);
  CHK_ENTITY_TYPE (_obj, BLOCKUSERPARAMETER, be_major, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKUSERPARAMETER, be_minor, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKUSERPARAMETER, eed1071, BL);
  // AcDbBlockParameter
  CHK_ENTITY_TYPE (_obj, BLOCKUSERPARAMETER, show_properties, B);
  CHK_ENTITY_TYPE (_obj, BLOCKUSERPARAMETER, chain_actions, B);
  // AcDbBlock1PtParameter
  CHK_ENTITY_3RD (_obj, BLOCKUSERPARAMETER, def_pt);
  CHK_ENTITY_TYPE (_obj, BLOCKUSERPARAMETER, num_propinfos, BL);
  CHK_ENTITY_MAX (_obj, BLOCKUSERPARAMETER, num_propinfos, BL, 2);
  BLOCKPARAMETER_PropInfo (prop1) BLOCKPARAMETER_PropInfo (prop2)
      // AcDbBlockUserParameter
      CHK_ENTITY_TYPE (_obj, BLOCKUSERPARAMETER, flag, BS);
  CHK_ENTITY_H (_obj, BLOCKUSERPARAMETER, assocvariable);
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKUSERPARAMETER, expr);

  CHK_SUBCLASS_TYPE (_obj->value, EvalVariant, code, BS);
  switch (dwg_resbuf_value_type (_obj->value.code))
    {
    case DWG_VT_REAL:
      CHK_SUBCLASS_TYPE (_obj->value, EvalVariant, u.bd, BD);
      break;
    case DWG_VT_INT32:
      CHK_SUBCLASS_TYPE (_obj->value, EvalVariant, u.bl, BL);
      break;
    case DWG_VT_INT16:
      CHK_SUBCLASS_TYPE (_obj->value, EvalVariant, u.bs, BS);
      break;
    case DWG_VT_STRING:
      CHK_SUBCLASS_UTF8TEXT (_obj->value, EvalVariant, u.text);
      break;
    case DWG_VT_HANDLE:
      CHK_SUBCLASS_H (_obj->value, EvalVariant, u.handle);
      break;
    case DWG_VT_BINARY:
    case DWG_VT_OBJECTID:
    case DWG_VT_POINT3D:
    case DWG_VT_INVALID:
    case DWG_VT_INT64:
    case DWG_VT_INT8:
    case DWG_VT_BOOL:
    default:
      fail ("Unknown ASSOCVARIABLE.value.code %u", _obj->value.code);
    }

  CHK_ENTITY_TYPE (_obj, BLOCKUSERPARAMETER, type, BS);
  // #endif
}
