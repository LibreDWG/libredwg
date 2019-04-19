#define DWG_TYPE DWG_TYPE_SEQEND
#include "common.c"
#include <dejagnu.h>

void
api_process (dwg_object *obj)
{
  int error1, error2;
  dwg_ent_seqend *seqend = dwg_object_to_SEQEND (obj);
  dwg_ent_generic *gen = (dwg_ent_generic *)seqend;
  dwg_obj_ent *parent = dwg_ent_generic_parent (gen, &error1);
  dwg_object *obj2 = dwg_ent_generic_to_object (gen, &error2);

  if (!error1 && seqend->parent == parent)
    pass ("dwg_ent_generic_parent ok");
  else
    fail ("dwg_ent_generic_parent SEQEND %p == %p", parent, seqend->parent);

  if (!error2 && obj2 == obj)
    pass ("dwg_ent_generic_to_object ok");
  else
    fail ("dwg_ent_generic_to_object SEQEND %p == %p", obj2, obj);
}
