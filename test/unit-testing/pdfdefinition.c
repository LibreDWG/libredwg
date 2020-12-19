#define DWG_TYPE DWG_TYPE_PDFDEFINITION
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_T filename;
  BITCODE_T name;

  dwg_obj_underlaydefinition *underlaydef = dwg_object_to_PDFDEFINITION (obj);

  CHK_ENTITY_UTF8TEXT (underlaydef, PDFDEFINITION, filename);
  CHK_ENTITY_UTF8TEXT (underlaydef, PDFDEFINITION, name);
}
