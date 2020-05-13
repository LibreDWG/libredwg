// TODO unstable
#define DWG_TYPE DWG_TYPE_DBCOLOR
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_CMC color;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_dbcolor *dbcolor = dwg_object_to_DBCOLOR (obj);

  CHK_ENTITY_CMC (dbcolor, DBCOLOR, color);
}
