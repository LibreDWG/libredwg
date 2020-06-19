// TODO debugging
#define DWG_TYPE DWG_TYPE_ASSOCALIGNEDDIMACTIONBODY
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  ASSOCANNOTATIONACTIONBODY_fields;
  Dwg_ASSOCPARAMBASEDACTIONBODY pab;
  BITCODE_BS class_version;
  BITCODE_H  d_node;
  BITCODE_H  r_node;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_assocaligneddimactionbody *_obj = dwg_object_to_ASSOCALIGNEDDIMACTIONBODY (obj);

  CHK_ENTITY_TYPE (_obj, ASSOCALIGNEDDIMACTIONBODY, aaab_version, BS);
  CHK_ENTITY_MAX  (_obj, ASSOCALIGNEDDIMACTIONBODY, aaab_version, BS, 2);
  CHK_ENTITY_H    (_obj, ASSOCALIGNEDDIMACTIONBODY, assoc_dep);
  CHK_ENTITY_H    (_obj, ASSOCALIGNEDDIMACTIONBODY, actionbody);
  
  CHK_SUBCLASS_TYPE (_obj->pab, ASSOCPARAMBASEDACTIONBODY, aab_version, BL);
  CHK_SUBCLASS_TYPE (_obj->pab, ASSOCPARAMBASEDACTIONBODY, status, BL);
  CHK_SUBCLASS_TYPE (_obj->pab, ASSOCPARAMBASEDACTIONBODY, l2, BL);
  CHK_SUBCLASS_TYPE (_obj->pab, ASSOCPARAMBASEDACTIONBODY, num_deps, BL);
  CHK_SUBCLASS_HV   (_obj->pab, ASSOCPARAMBASEDACTIONBODY, deps, _obj->pab.num_deps);
  CHK_SUBCLASS_TYPE (_obj->pab, ASSOCPARAMBASEDACTIONBODY, l4, BL);
  CHK_SUBCLASS_TYPE (_obj->pab, ASSOCPARAMBASEDACTIONBODY, l5, BL);
  CHK_SUBCLASS_H    (_obj->pab, ASSOCPARAMBASEDACTIONBODY, assoc_dep);
  CHK_SUBCLASS_TYPE (_obj->pab, ASSOCPARAMBASEDACTIONBODY, num_values, BL);
  for (unsigned i=0; i<_obj->pab.num_values; i++)
    {
      CHK_SUBCLASS_H (_obj->pab.values[i], VALUEPARAM, handle);
      switch (_obj->pab.values[i].value.type)
        {
        case 1:
          CHK_SUBCLASS_TYPE (_obj->pab.values[i].value.bd, EvalVariant, BD);
          break;
        case 2:
          CHK_SUBCLASS_TYPE (_obj->pab.values[i].value.bd, EvalVariant, BL);
          break;
        case 3:
          CHK_SUBCLASS_TYPE (_obj->pab.values[i].value.bd, EvalVariant, BS);
          break;
        case 5:
          CHK_SUBCLASS_UTF8TEXT (_obj->pab.values[i].value.text, EvalVariant);
          break;
        case 11:
          CHK_SUBCLASS_H (_obj->pab.values[i].value.handle, EvalVariant);
          break;
        }
    }

  CHK_ENTITY_TYPE (_obj, ASSOCALIGNEDDIMACTIONBODY, class_version, BS);
  CHK_ENTITY_H    (_obj, ASSOCALIGNEDDIMACTIONBODY, d_node);
  CHK_ENTITY_H    (_obj, ASSOCALIGNEDDIMACTIONBODY, r_node);
#endif
}
