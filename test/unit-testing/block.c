#define DWG_TYPE DWG_TYPE_BLOCK
#include "common.c"

void
low_level_process(dwg_object *obj)
{
  dwg_ent_block *block = dwg_object_to_BLOCK(obj);

  printf("block name : %s\n", block->name);
}

void
api_process(dwg_object *obj)
{
  int error;
  char *name;
  dwg_ent_block *block = dwg_object_to_BLOCK(obj);

  name = dwg_ent_block_get_name(block, &error);
  if ( !error )
      printf("block name  : %s\n", name);
  else
      printf("error in reading name\n");
} 
