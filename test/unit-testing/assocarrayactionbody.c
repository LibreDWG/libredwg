#define DWG_TYPE DWG_TYPE_ASSOCARRAYACTIONBODY
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  ASSOCARRAYACTIONBODY_fields;
  BITCODE_H *deps;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  // #ifdef DEBUG_CLASSES
  dwg_obj_assocarrayactionbody *_obj
      = dwg_object_to_ASSOCARRAYACTIONBODY (obj);

  // ASSOCACTIONBODY
  CHK_ENTITY_TYPE (_obj, ASSOCARRAYACTIONBODY, aab_version, BL);
  CHK_ASSOCPARAMBASEDACTIONBODY (ASSOCARRAYACTIONBODY);

  CHK_ENTITY_TYPE (_obj, ASSOCARRAYACTIONBODY, aaab_version, BL);
  CHK_ENTITY_UTF8TEXT (_obj, ASSOCARRAYACTIONBODY, paramblock);
  if (!dwg_dynapi_entity_value (_obj, "ASSOCARRAYACTIONBODY", "transmatrix",
                                &transmatrix, NULL))
    fail ("ASSOCARRAYACTIONBODY.transmatrix[16]");
  for (unsigned i = 0; i < 16; i++)
    {
      ok ("ASSOCARRAYACTIONBODY.transmatrix[%u]: %f", i, transmatrix[i]);
    }
  // #endif
}
