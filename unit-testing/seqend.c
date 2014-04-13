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

  if (dwg_get_type(obj)== DWG_TYPE_SEQEND)
    {
      output_process(obj);
    }
}

void
low_level_process(dwg_object *obj)
{

  // casts a dwg object to seqend entity
  dwg_ent_seqend *seqend = dwg_object_to_SEQEND(obj);

  // prints dummy value
  printf("dummy value of seqend : %d", seqend->dummy);
}

void
api_process(dwg_object *obj)
{
  int dummy_error;
  char dummy;
  dwg_ent_seqend *seqend = dwg_object_to_SEQEND(obj);

  // returns dummy value
  dummy = dwg_ent_seqend_get_dummy(seqend, &dummy_error);
  printf("dummy value of seqend : %d", dummy);
}
