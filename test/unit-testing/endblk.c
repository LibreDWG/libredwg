#define DWG_TYPE DWG_TYPE_ENDBLK
#include "common.c"

void
low_level_process(dwg_object *obj)
{
  dwg_ent_endblk *endblk = dwg_object_to_ENDBLK(obj);
  Dwg_Data *dwg = endblk->parent->dwg;

  if (endblk->parent != obj->tio.entity)
    printf("ERROR: obj_obj of endblk %p == %p", endblk->parent, obj->tio.entity);

  if (&dwg->object[endblk->parent->objid] != obj)
    printf("ERROR: obj of endblk %lu -> obj %p", endblk->parent->objid, obj);
}

void
api_process(dwg_object *obj)
{
  int error1, error2;
  dwg_ent_generic *endblk = (dwg_ent_generic *)dwg_object_to_ENDBLK(obj);
  dwg_obj_ent *parent = dwg_ent_generic_parent(endblk, &error1);
  dwg_object *obj2 = dwg_ent_generic_to_object(endblk, &error2);
  
  if (error1 || parent != obj->tio.entity)
    printf("ERROR: dwg_ent_generic_parent %p == %p", parent, obj->tio.entity);
  if (error2 || obj2 != obj)
    printf("ERROR: dwg_ent_generic_to_object %p == %p", obj2, obj);
}
