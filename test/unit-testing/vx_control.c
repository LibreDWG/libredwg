#define DWG_TYPE DWG_TYPE_VX_CONTROL
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BS i, num_entries;
  BITCODE_H *entries, *hdls;

  dwg_obj_vx_control *ctrl = dwg_object_to_VX_CONTROL (obj);

  CHK_ENTITY_TYPE (ctrl, VX_CONTROL, num_entries, BS);
  entries = dwg_object_tablectrl_get_entries (obj, &error);
  if (!dwg_dynapi_entity_value (ctrl, "VX_CONTROL", "entries", &hdls, NULL))
    fail ("VX_CONTROL.entries");
  for (i = 0; i < num_entries; i++)
    {
      if (hdls[i] == entries[i])
        ok ("VX_CONTROL.entries[%d]: " FORMAT_REF, i, ARGS_REF (entries[i]));
      else
        fail ("VX_CONTROL.entries[%d]: " FORMAT_REF, i, ARGS_REF (entries[i]));
    }
}
