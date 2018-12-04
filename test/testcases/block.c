#define DWG_TYPE DWG_TYPE_BLOCK
#include "common.c"
#include <dejagnu.h>

/* This function checks API functions for integrity
   @params dwg_object* obj
 */
void
api_process (dwg_object * obj)
{
  int error;
  char *name;

  // casts dwg object to block
  dwg_ent_block *block = dwg_object_to_BLOCK (obj);


  name = dwg_ent_block_get_name (block, &error);
  if (!error  && block->name == name)
    pass ("Working Properly");
  else
    fail ("error in reading name");

}
