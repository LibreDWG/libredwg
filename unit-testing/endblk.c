#include "common.c"

void
output_process(dwg_object *obj);

void
output_object(dwg_object* obj){
  if (!obj)
    {
      printf("object is NULL\n");
      return;
    }

  if (dwg_get_type(obj)== DWG_TYPE_ENDBLK)
    {
      output_process(obj);
    }
}

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
