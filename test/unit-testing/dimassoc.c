// TODO unstable
#define DWG_TYPE DWG_TYPE_DIMASSOC
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_H dimensionobj;
  BITCODE_BL associativity;
  BITCODE_B trans_space_flag;
  BITCODE_RC rotated_type;
  Dwg_DIMASSOC_Ref *ref;
  // BITCODE_BS j;
  BITCODE_BL num_intsectobj, num_xrefs;
  BITCODE_H *intsectobj, *xrefs;
  // BITCODE_BL intsect_gsmarker;
  // BITCODE_H intsectxrefobj;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_dimassoc *dimassoc = dwg_object_to_DIMASSOC (obj);

  if (obj->fixedtype == DWG_TYPE_UNKNOWN_OBJ)
    return;

  CHK_ENTITY_TYPE (dimassoc, DIMASSOC, associativity, BLx);
  CHK_ENTITY_MAX (dimassoc, DIMASSOC, associativity, BL, 15);
  CHK_ENTITY_TYPE (dimassoc, DIMASSOC, trans_space_flag, B);
  CHK_ENTITY_TYPE (dimassoc, DIMASSOC, rotated_type, RCd);
  CHK_ENTITY_H (dimassoc, DIMASSOC, dimensionobj);
  if (!dwg_dynapi_entity_value (dimassoc, "DIMASSOC", "ref", &ref, NULL))
    fail ("DIMASSOC.ref");
  for (int i = 0; i < 4; i++)
    {
      // 0 1 2 3 => 1 2 4 8. skip unset bits
      if (!(associativity & (1 << i)))
        continue;
      CHK_SUBCLASS_UTF8TEXT (ref[i], DIMASSOC_Ref,
                             classname); // "AcDbOsnapPointRef"
      CHK_SUBCLASS_TYPE (ref[i], DIMASSOC_Ref, osnap_type, RC);
      CHK_SUBCLASS_MAX (ref[i], DIMASSOC_Ref, osnap_type, RC, 13);
      CHK_SUBCLASS_TYPE (ref[i], DIMASSOC_Ref, osnap_dist, BD);
      CHK_SUBCLASS_3RD (ref[i], DIMASSOC_Ref, osnap_pt);
      CHK_SUBCLASS_TYPE (ref[i], DIMASSOC_Ref, main_subent_type, BL);
      CHK_SUBCLASS_MAX (ref[i], DIMASSOC_Ref, main_subent_type, BL, 2);
      CHK_SUBCLASS_TYPE (ref[i], DIMASSOC_Ref, main_gsmarker, BL);
      CHK_SUBCLASS_TYPE (ref[i], DIMASSOC_Ref, num_intsectobj, BL);
      CHK_SUBCLASS_MAX (ref[i], DIMASSOC_Ref, num_intsectobj, BL, 100);
      num_intsectobj = ref[i].num_intsectobj;
      CHK_SUBCLASS_HV (ref[i], DIMASSOC_Ref, intsectobj, num_intsectobj);
      CHK_SUBCLASS_TYPE (ref[i], DIMASSOC_Ref, num_xrefs, BL);
      CHK_SUBCLASS_MAX (ref[i], DIMASSOC_Ref, num_xrefs, BL, 100);
      num_xrefs = ref[i].num_xrefs;
      CHK_SUBCLASS_HV (ref[i], DIMASSOC_Ref, xrefs, num_xrefs);
      CHK_SUBCLASS_TYPE (ref[i], DIMASSOC_Ref, has_lastpt_ref, B);
      CHK_SUBCLASS_3RD (ref[i], DIMASSOC_Ref, lastpt_ref);
    }
#endif
}
