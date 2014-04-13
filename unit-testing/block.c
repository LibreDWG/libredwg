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

  if (dwg_get_type(obj)== DWG_TYPE_BLOCK)
    {
      output_process(obj);
    }
}

void
low_level_process(dwg_object *obj)
{
  // casts dwg object to block
  dwg_ent_block *block = dwg_object_to_BLOCK(obj);

  // returns block name
  printf("block name : %s\t\n", block->name);
}

void
api_process(dwg_object *obj)
{
  int name_error;
  char * name;

  // casts dwg object to block
  dwg_ent_block *block = dwg_object_to_BLOCK(obj);

  // returns block name
  name = dwg_ent_block_get_name(block, &name_error);
  if(name_error == 0 )
    {
      printf("block name  : %s\t\n", name);
    }
  else
    {
      printf("error in reading name\n");
    }
 
} 
