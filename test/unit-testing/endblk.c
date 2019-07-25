#define DWG_TYPE DWG_TYPE_ENDBLK
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error1, error2;
  dwg_ent_generic *endblk = (dwg_ent_generic *)dwg_object_to_ENDBLK (obj);
  dwg_obj_ent *parent = dwg_ent_generic_parent (endblk, &error1);
  dwg_object *obj2 = dwg_ent_generic_to_object (endblk, &error2);

  if (error1 || parent != obj->tio.entity)
    fail ("dwg_ent_generic_parent %p == %p", parent, obj->tio.entity);
  else
    ok ("dwg_ent_generic_parent");

  if (error2 || obj2 != obj)
    fail ("dwg_ent_generic_to_object %p == %p", obj2, obj);
  else
    pass ();
}
