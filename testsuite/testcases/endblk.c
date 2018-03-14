#define DWG_TYPE DWG_TYPE_ENDBLK
#include "common.c"
#include <dejagnu.h>

void
api_process (dwg_object * obj)
{
  int dummy_error;
  char dummy;
  dwg_ent_endblk *endblk = dwg_object_to_ENDBLK (obj);

  // returns dummy value
  dummy = dwg_ent_endblk_get_dummy (endblk, &dummy_error);
  if (dummy_error == 0 && dummy == endblk->dummy)
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("dwg_ent_endblk_get_dummy");
    }
}
