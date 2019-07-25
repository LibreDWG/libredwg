#define DWG_TYPE DWG_TYPE_BLOCK
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  char *name;
  dwg_ent_block *block = dwg_object_to_BLOCK (obj);

  CHK_ENTITY_UTF8TEXT (block, BLOCK, name, name);
  if (strcmp (dwg_ent_block_get_name (block, &error), name))
    fail ("old API dwg_ent_block_get_name");
}
