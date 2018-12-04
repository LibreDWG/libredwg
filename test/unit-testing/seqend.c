#define DWG_TYPE DWG_TYPE_SEQEND
#include "common.c"

void
low_level_process(dwg_object *obj)
{
  dwg_ent_seqend *seqend = dwg_object_to_SEQEND(obj);
  Dwg_Data *dwg = seqend->parent->dwg;

  if (seqend->parent != obj->tio.entity)
    printf("ERROR: obj_obj of seqend %p == %p", seqend->parent, obj->tio.entity);

  if (&dwg->object[seqend->parent->objid] != obj)
    printf("ERROR: obj of seqend %ld -> obj %p", (long)seqend->parent->objid, obj);
}

void
api_process(dwg_object *obj)
{
  int error1, error2;
  dwg_ent_generic *seqend = (dwg_ent_generic *)dwg_object_to_SEQEND(obj);
  dwg_obj_ent *parent = dwg_ent_generic_parent(seqend, &error1);
  dwg_object *obj2 = dwg_ent_generic_to_object(seqend, &error2);

  if (error1 || parent != obj->tio.entity)
    printf("ERROR: dwg_ent_generic_parent %p == %p", parent, obj->tio.entity);

  if (error2 || obj2->address != obj->address)
    printf("ERROR: dwg_ent_generic_to_object %lu == %lu", obj2->address, obj->address);
}
