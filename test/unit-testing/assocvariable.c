// TODO unstable
#define DWG_TYPE DWG_TYPE_ASSOCVARIABLE
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BS av_class_version;
  ASSOCACTION_fields;
  BITCODE_T name;
  BITCODE_T t58;
  BITCODE_T evaluator;
  BITCODE_T desc;
  // BITCODE_BSd value_type;
  Dwg_EvalVariant value;
  BITCODE_B has_t78;
  BITCODE_T t78;
  BITCODE_B b290;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  // #ifdef DEBUG_CLASSES
  dwg_obj_assocvariable *_obj = dwg_object_to_ASSOCVARIABLE (obj);

  CHK_ENTITY_TYPE (_obj, ASSOCVARIABLE, av_class_version, BS);

  // ASSOCACTION
  CHK_ENTITY_TYPE (_obj, ASSOCVARIABLE, class_version, BS);
  CHK_ENTITY_TYPE (_obj, ASSOCVARIABLE, geometry_status, BL);
  CHK_ENTITY_MAX (_obj, ASSOCVARIABLE, geometry_status, BL, 10);
  CHK_ENTITY_H (_obj, ASSOCVARIABLE, owningnetwork);
  CHK_ENTITY_H (_obj, ASSOCVARIABLE, actionbody);
  CHK_ENTITY_TYPE (_obj, ASSOCVARIABLE, action_index, BL);
  CHK_ENTITY_TYPE (_obj, ASSOCVARIABLE, max_assoc_dep_index, BL);
  CHK_ENTITY_TYPE (_obj, ASSOCVARIABLE, num_deps, BL);
  for (unsigned i = 0; i < num_deps; i++)
    {
      CHK_SUBCLASS_TYPE (_obj->deps[i], ASSOCACTION_Deps, is_owned, B);
      CHK_SUBCLASS_H (_obj->deps[i], ASSOCACTION_Deps, dep);
    }
  CHK_ENTITY_TYPE (_obj, ASSOCVARIABLE, num_owned_params, BL);
  CHK_ENTITY_HV (_obj, ASSOCVARIABLE, owned_params, num_owned_params);
  CHK_ENTITY_TYPE (_obj, ASSOCNETWORK, num_values, BL);
  CHK_VALUEPARAM (num_values, values);

  // ASSOCVARIABLE
  CHK_ENTITY_UTF8TEXT (_obj, ASSOCVARIABLE, name);
  CHK_ENTITY_UTF8TEXT (_obj, ASSOCVARIABLE, t58);
  CHK_ENTITY_UTF8TEXT (_obj, ASSOCVARIABLE, evaluator);
  CHK_ENTITY_UTF8TEXT (_obj, ASSOCVARIABLE, desc);
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
  CHK_ENTITY_TYPE (_obj, ASSOCVARIABLE, has_t78, B);
  CHK_ENTITY_UTF8TEXT (_obj, ASSOCVARIABLE, t78);
  CHK_ENTITY_TYPE (_obj, ASSOCVARIABLE, b290, B);
  // #endif
}
