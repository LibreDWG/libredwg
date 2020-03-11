#define DWG_TYPE DWG_TYPE_UNDERLAYDEFINITION
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_TV filename;
  BITCODE_TV name;

  dwg_obj_underlaydefinition *underlaydef = dwg_object_to_UNDERLAYDEFINITION (obj);

  CHK_ENTITY_UTF8TEXT (underlaydef, UNDERLAYDEFINITION, filename, filename);
  CHK_ENTITY_UTF8TEXT (underlaydef, UNDERLAYDEFINITION, name, name);
}
