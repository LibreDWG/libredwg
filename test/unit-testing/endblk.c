#define DWG_TYPE DWG_TYPE_ENDBLK
#include "common.c"

void
low_level_process(dwg_object *obj)
{
  dwg_ent_endblk *endblk = dwg_object_to_ENDBLK(obj);

  printf("dummy value of endblk : %d", endblk->dummy);
}

void
api_process(dwg_object *obj)
{
  int error;
  char dummy;
  dwg_ent_endblk *endblk = dwg_object_to_ENDBLK(obj);


  dummy = dwg_ent_endblk_get_dummy(endblk, &error);
  printf("dummy value of endblk : %d", dummy);
}
