#define DWG_TYPE DWG_TYPE_UCS_CONTROL
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BS i, num_entries;
  BITCODE_H *entries, *hdls;

  dwg_obj_ucs_control *ctrl = dwg_object_to_UCS_CONTROL (obj);

  CHK_ENTITY_TYPE (ctrl, UCS_CONTROL, num_entries, BS);
  entries = dwg_object_tablectrl_get_entries (obj, &error);
  if (!dwg_dynapi_entity_value (ctrl, "UCS_CONTROL", "entries", &hdls, NULL))
    fail ("UCS_CONTROL.entries");
  for (i = 0; i < num_entries; i++)
    {
      if (hdls[i] == entries[i])
        ok ("UCS_CONTROL.entries[%d]: " FORMAT_REF, i, ARGS_REF (entries[i]));
      else
        fail ("UCS_CONTROL.entries[%d]: " FORMAT_REF, i,
              ARGS_REF (entries[i]));
    }
}
