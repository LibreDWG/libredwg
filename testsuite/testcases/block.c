#include "common.c"
#include <dejagnu.h>

void output_process (dwg_object * obj);

void
output_object (dwg_object * obj)
{
  if (!obj)
    {
      printf ("object is NULL\n");
      return;
    }

  if (dwg_get_type (obj) == DWG_TYPE_BLOCK)
    {
      output_process (obj);
    }
}

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
