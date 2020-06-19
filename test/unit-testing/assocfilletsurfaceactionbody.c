// TODO unstable
#define DWG_TYPE DWG_TYPE_ASSOCFILLETSURFACEACTIONBODY
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  ASSOCPATHBASEDSURFACEACTIONBODY_fields;
  // AcDbAssocFilletSurfaceActionBody
  BITCODE_BL class_version;
  BITCODE_BS status;
  BITCODE_2RD pt1, pt2;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_assocfilletsurfaceactionbody *_obj = dwg_object_to_ASSOCFILLETSURFACEACTIONBODY (obj);

  CHK_ENTITY_TYPE (_obj, ASSOCFILLETSURFACEACTIONBODY, aaab_version, BS);
  CHK_ENTITY_MAX  (_obj, ASSOCFILLETSURFACEACTIONBODY, aaab_version, BS, 2);
  CHK_ENTITY_H    (_obj, ASSOCFILLETSURFACEACTIONBODY, assoc_dep);
  CHK_ENTITY_H    (_obj, ASSOCFILLETSURFACEACTIONBODY, actionbody);

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
  
  CHK_ENTITY_TYPE (_obj, ASSOCFILLETSURFACEACTIONBODY, pab_status, BL);
  CHK_ENTITY_TYPE (_obj, ASSOCFILLETSURFACEACTIONBODY, pab_l2, BL);
  CHK_ENTITY_TYPE (_obj, ASSOCFILLETSURFACEACTIONBODY, num_deps, BL);
  CHK_ENTITY_TYPE (_obj, ASSOCFILLETSURFACEACTIONBODY, pab_l4, BL);
  CHK_ENTITY_TYPE (_obj, ASSOCFILLETSURFACEACTIONBODY, pab_l5, BL);
  CHK_ENTITY_HV (_obj, ASSOCFILLETSURFACEACTIONBODY, readdeps, num_deps);
  CHK_ENTITY_HV (_obj, ASSOCFILLETSURFACEACTIONBODY, writedeps, num_deps); 
  CHK_ENTITY_TYPE (_obj, ASSOCFILLETSURFACEACTIONBODY, sab_status, BL);
  CHK_ENTITY_TYPE (_obj, ASSOCFILLETSURFACEACTIONBODY, sab_b1, B);
  CHK_ENTITY_TYPE (_obj, ASSOCFILLETSURFACEACTIONBODY, sab_l2, BL);
  CHK_ENTITY_TYPE (_obj, ASSOCFILLETSURFACEACTIONBODY, sab_b2, B);
  CHK_ENTITY_TYPE (_obj, ASSOCFILLETSURFACEACTIONBODY, sab_s1, BS);
  CHK_ENTITY_TYPE (_obj, ASSOCFILLETSURFACEACTIONBODY, pbsab_status, BL);

  CHK_ENTITY_TYPE (_obj, ASSOCFILLETSURFACEACTIONBODY, class_version, BL);
  CHK_ENTITY_TYPE (_obj, ASSOCFILLETSURFACEACTIONBODY, status, BS);
  CHK_ENTITY_2RD  (_obj, ASSOCFILLETSURFACEACTIONBODY, pt1);
  CHK_ENTITY_2RD  (_obj, ASSOCFILLETSURFACEACTIONBODY, pt2);
}
