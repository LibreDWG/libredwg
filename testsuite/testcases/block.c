#define DWG_TYPE DWG_TYPE_BLOCK
#include "common.c"
#include <dejagnu.h>

/* This function checks API functions for integrity
   @params dwg_object* obj 
 */
void
api_process (dwg_object * obj)
{
  int name_error;
  char *name;

  // casts dwg object to block
  dwg_ent_block *block = dwg_object_to_BLOCK (obj);

  // returns block name
  name = dwg_ent_block_get_name (block, &name_error);
  if (name_error == 0 && block->name == name)
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading name");
    }

}
