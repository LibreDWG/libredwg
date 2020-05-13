#define DWG_TYPE DWG_TYPE_DICTIONARYVAR
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_RC intval;
  BITCODE_TV str;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_dictionaryvar *_obj = dwg_object_to_DICTIONARYVAR (obj);

  CHK_ENTITY_TYPE (_obj, DICTIONARYVAR, intval, RC);
  CHK_ENTITY_UTF8TEXT (_obj, DICTIONARYVAR, str, str);
}
