#define DWG_TYPE DWG_TYPE_SEQEND
#include "common.c"
#include <dejagnu.h>

void
low_level_process (dwg_object * obj)
{
  dwg_ent_seqend *seqend = dwg_object_to_SEQEND (obj);

  printf ("dummy value of seqend : " FORMAT_BS, seqend->dummy);
}

void
api_process (dwg_object * obj)
{
  int error;
  char dummy;
  dwg_ent_seqend *seqend = dwg_object_to_SEQEND (obj);

  dummy = dwg_ent_seqend_get_dummy (seqend, &error);
  if (!error  && dummy == seqend->dummy)
    pass ("Working Properly");
  else
    fail ("dwg_ent_seqend_get_dummy");

}
