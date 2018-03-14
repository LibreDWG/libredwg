#define DWG_TYPE DWG_TYPE_ENDBLK
#include "common.c"

void
low_level_process(dwg_object *obj)
{
  // casts object to endblk
  dwg_ent_endblk *endblk = dwg_object_to_ENDBLK(obj);

  // prints dummy value
  printf("dummy value of endblk : %d", endblk->dummy);
}

void
api_process(dwg_object *obj)
{
  int dummy_error;
  char dummy;
  dwg_ent_endblk *endblk = dwg_object_to_ENDBLK(obj);

  // returns dummy value
  dummy = dwg_ent_endblk_get_dummy(endblk, &dummy_error);
  printf("dummy value of endblk : %d", dummy);
}
