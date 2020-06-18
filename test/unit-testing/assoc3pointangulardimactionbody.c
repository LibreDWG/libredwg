// TODO debugging
#define DWG_TYPE DWG_TYPE_ASSOC3POINTANGULARDIMACTIONBODY
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  ASSOCANNOTATIONACTIONBODY_fields;
  struct _dwg_ASSOCPARAMBASEDACTIONBODY pab;
  BITCODE_BS class_version;       /*!< DXF 90  */
  BITCODE_H h1;		/*!< DXF 330  */
  BITCODE_H h2;		/*!< DXF 330  */
  BITCODE_H h3;		/*!< DXF 330  */

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_ASSOC3POINTANGULARDIMACTIONBODY *_obj = dwg_object_to_ASSOC3POINTANGULARDIMACTIONBODY (obj);

  CHK_ENTITY_TYPE (_obj, ASSOC3POINTANGULARDIMACTIONBODY, aaab_version, BS);
  CHK_ENTITY_MAX (_obj, ASSOC3POINTANGULARDIMACTIONBODY, aaab_version, BS, 2);
  CHK_ENTITY_H (_obj, ASSOC3POINTANGULARDIMACTIONBODY, assoc_dep);
  CHK_ENTITY_H (_obj, ASSOC3POINTANGULARDIMACTIONBODY, actionbody);
  
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

  //CHK_ENTITY_TYPE (_obj, ASSOC3POINTANGULARDIMACTIONBODY, dcm_status, BL);
  CHK_ENTITY_H (_obj, ASSOC3POINTANGULARDIMACTIONBODY, h1);
  CHK_ENTITY_H (_obj, ASSOC3POINTANGULARDIMACTIONBODY, h2);
  CHK_ENTITY_H (_obj, ASSOC3POINTANGULARDIMACTIONBODY, h3);
#endif
}
