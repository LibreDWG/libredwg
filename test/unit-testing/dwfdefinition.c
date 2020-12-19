#define DWG_TYPE DWG_TYPE_DWFDEFINITION
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_T filename;
  BITCODE_T name;

  dwg_obj_underlaydefinition *underlaydef = dwg_object_to_DWFDEFINITION (obj);

  CHK_ENTITY_UTF8TEXT (underlaydef, DWFDEFINITION, filename);
  CHK_ENTITY_UTF8TEXT (underlaydef, DWFDEFINITION, name);
}
