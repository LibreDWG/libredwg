#define DWG_TYPE DWG_TYPE_BLOCK_CONTROL
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BS i, num_entries;
  BITCODE_H *entries, *hdls;
  BITCODE_H model_space;
  BITCODE_H paper_space;

  dwg_obj_block_control *block_control = dwg_object_to_BLOCK_CONTROL (obj);

  CHK_ENTITY_TYPE_W_OBJ (block_control, BLOCK_CONTROL, num_entries, BS);
  entries = dwg_object_tablectrl_get_entries (obj, &error);
  if (!dwg_dynapi_entity_value (block_control, "BLOCK_CONTROL", "entries",
                                &hdls, NULL))
    fail ("BLOCK_CONTROL.entries");
  for (i = 0; i < num_entries; i++)
    {
      if (hdls[i] == entries[i])
        ok ("BLOCK_CONTROL.entries[%d]: " FORMAT_REF, i,
            ARGS_REF (entries[i]));
      else
        fail ("BLOCK_CONTROL.entries[%d]: " FORMAT_REF, i,
              ARGS_REF (entries[i]));
    }
  CHK_ENTITY_H (block_control, BLOCK_CONTROL, model_space);
  CHK_ENTITY_H (block_control, BLOCK_CONTROL, model_space);
}
