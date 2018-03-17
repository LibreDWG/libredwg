#define DWG_TYPE DWG_TYPE_ENDBLK
#include "common.c"
#include <dejagnu.h>

void
api_process (dwg_object * obj)
{
  int error;
  char dummy;
  dwg_ent_endblk *endblk = dwg_object_to_ENDBLK (obj);


  dummy = dwg_ent_endblk_get_dummy (endblk, &error);
  if (!error  && dummy == endblk->dummy)
    pass ("Working Properly");
  else
    fail ("dwg_ent_endblk_get_dummy");
}
