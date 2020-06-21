#define DWG_TYPE DWG_TYPE_ASSOCARRAYACTIONBODY
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  ASSOCARRAYACTIONBODY_fields;
  BITCODE_H *deps;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_assocarrayactionbody *_obj = dwg_object_to_ASSOCARRAYACTIONBODY (obj);

  // ASSOCACTIONPARAM:
  CHK_ENTITY_TYPE (_obj, ASSOCARRAYMODIFYACTIONBODY, is_r2013, BS);
  CHK_ENTITY_TYPE (_obj, ASSOCARRAYMODIFYACTIONBODY, aap_version, BL);
  CHK_ENTITY_UTF8TEXT (_obj, ASSOCARRAYMODIFYACTIONBODY, name);
  CHK_ASSOCPARAMBASEDACTIONBODY (ASSOCARRAYACTIONBODY);

  CHK_ENTITY_TYPE (_obj, ASSOCARRAYACTIONBODY, aaab_version, BL);
  CHK_ENTITY_UTF8TEXT (_obj, ASSOCARRAYACTIONBODY, aaab_paramblock);
  if (!dwg_dynapi_entity_value (_obj, "ASSOCARRAYACTIONBODY", "aaab_transmatrix[16]", &aaab_transmatrix, NULL))
    fail ("ASSOCARRAYACTIONBODY.aaab_transmatrix[16]");
  for (unsigned i = 0; i < 16; i++)
    {
      ok ("ASSOCARRAYACTIONBODY.aaab_transmatrix[%u]: %f", i, aaab_transmatrix[i]);
    }
#endif
}
