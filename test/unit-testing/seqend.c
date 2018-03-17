#define DWG_TYPE DWG_TYPE_SEQEND
#include "common.c"

void
low_level_process(dwg_object *obj)
{
  dwg_ent_seqend *seqend = dwg_object_to_SEQEND(obj);

  printf("dummy value of seqend : %d", seqend->dummy);
}

void
api_process(dwg_object *obj)
{
  int error;
  char dummy;
  dwg_ent_seqend *seqend = dwg_object_to_SEQEND(obj);

  dummy = dwg_ent_seqend_get_dummy(seqend, &error);
  printf("dummy value of seqend : %d", dummy);
}
