#define DWG_TYPE DWG_TYPE_DGNDEFINITION
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_T filename;
  BITCODE_T name;

  dwg_obj_dgndefinition *def = dwg_object_to_DGNDEFINITION (obj);

  CHK_ENTITY_UTF8TEXT (def, DGNDEFINITION, filename);
  CHK_ENTITY_UTF8TEXT (def, DGNDEFINITION, name);
}
