#define DWG_TYPE DWG_TYPE_ENDBLK
#include "common.c"
#include <dejagnu.h>

void
api_process (dwg_object *obj)
{
  int error1, error2;
  dwg_ent_endblk *endblk = dwg_object_to_ENDBLK (obj);
  dwg_ent_generic *gen = (dwg_ent_generic *)endblk;
  dwg_obj_ent *parent = dwg_ent_generic_parent (gen, &error1);
  dwg_object *obj2 = dwg_ent_generic_to_object (gen, &error2);

  if (!error1 && endblk->parent == parent)
    pass ("dwg_ent_generic_parent ok");
  else
    fail ("dwg_ent_generic_parent ENDBLK %p == %p", parent, endblk->parent);

  if (!error2 && obj2 == obj)
    pass ("dwg_ent_generic_to_object ok");
  else
    fail ("dwg_ent_generic_to_object ENDBLK %p == %p", obj2, obj);
}
