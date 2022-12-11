// pre2.0 only
#define DWG_TYPE DWG_TYPE_LOAD
#include "common.c"

void
api_process (dwg_object *obj)
{
#ifdef USE_WRITE
  int error;
  char *file_name;
  int isnew;
  dwg_ent_load *load = dwg_object_to_LOAD (obj);

  CHK_ENTITY_UTF8TEXT (load, LOAD, file_name);
#endif
}
