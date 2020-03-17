// TODO unstable
#define DWG_TYPE DWG_TYPE_DIMASSOC
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BL associativity;
  BITCODE_RC trans_space_flag;
  Dwg_DIMASSOC_Ref *ref;
  BITCODE_BL intsect_gsmarker;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_dimassoc *dimassoc = dwg_object_to_DIMASSOC (obj);

  if (obj->fixedtype == DWG_TYPE_UNKNOWN_OBJ)
    return;

  CHK_ENTITY_TYPE (dimassoc, DIMASSOC, associativity, BL, associativity);
  CHK_ENTITY_TYPE (dimassoc, DIMASSOC, trans_space_flag, RC, trans_space_flag);
  //CHK_ENTITY_TYPE (dimassoc, DIMASSOC, intsect_gsmarker, BL, intsect_gsmarker);
  if (!dwg_dynapi_entity_value (dimassoc, "DIMASSOC", "ref", &ref, NULL))
    fail ("DIMASSOC.ref");
  for (int i = 0; i < 4; i++)
    {
      // 0 1 2 3 => 1 2 4 8. skip unset bits
      if (!(associativity & (1<<i)))
        continue;
      CHK_SUBCLASS_TYPE (ref[i], DIMASSOC_Ref, rotated_type, BS);
      CHK_SUBCLASS_TYPE (ref[i], DIMASSOC_Ref, osnap_type, RC);
      CHK_SUBCLASS_UTF8TEXT (ref[i], DIMASSOC_Ref, classname); // "AcDbOsnapPointRef"
      CHK_SUBCLASS_TYPE (ref[i], DIMASSOC_Ref, main_subent_type, BS);
      CHK_SUBCLASS_TYPE (ref[i], DIMASSOC_Ref, intsect_subent_type, BS);
      CHK_SUBCLASS_TYPE (ref[i], DIMASSOC_Ref, main_gsmarker, BL);
      CHK_SUBCLASS_TYPE (ref[i], DIMASSOC_Ref, osnap_dist, BD);
      CHK_SUBCLASS_3RD (ref[i], DIMASSOC_Ref, osnap_pt);
      CHK_SUBCLASS_TYPE (ref[i], DIMASSOC_Ref, has_lastpt_ref, B);
      CHK_SUBCLASS_H (ref[i], DIMASSOC_Ref, mainobj);
      CHK_SUBCLASS_H (ref[i], DIMASSOC_Ref, intsectobj);
    }
}
