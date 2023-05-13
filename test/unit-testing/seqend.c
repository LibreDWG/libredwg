#define DWG_TYPE DWG_TYPE_SEQEND
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error1, error2;
  dwg_ent_generic *seqend = (dwg_ent_generic *)dwg_object_to_SEQEND (obj);
  dwg_obj_ent *parent = dwg_ent_generic_parent (seqend, &error1);
  dwg_object *obj2 = dwg_ent_generic_to_object (seqend, &error2);

  if (error1 || parent != obj->tio.entity)
    fail ("dwg_ent_generic_parent %p == %p", parent, obj->tio.entity);
  else
    ok ("dwg_ent_generic_parent");

  if (error2 || obj2->address != obj->address)
    fail ("dwg_ent_generic_to_object %zu == %zu", obj2->address, obj->address);
  else
    pass ();
}
