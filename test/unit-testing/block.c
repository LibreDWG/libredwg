#define DWG_TYPE DWG_TYPE_BLOCK
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  char *name;
  dwg_ent_block *block = dwg_object_to_BLOCK (obj);
  int isnew;

  CHK_ENTITY_UTF8TEXT_W_OLD (block, BLOCK, name);
}
