#define DWG_TYPE DWG_TYPE_VIEW_CONTROL
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BS i, num_entries;
  BITCODE_H *entries, *hdls;

  dwg_obj_view_control *ctrl = dwg_object_to_VIEW_CONTROL (obj);

  CHK_ENTITY_TYPE (ctrl, VIEW_CONTROL, num_entries, BS);
  entries = dwg_object_tablectrl_get_entries (obj, &error);
  if (!dwg_dynapi_entity_value (ctrl, "VIEW_CONTROL", "entries", &hdls, NULL))
    fail ("VIEW_CONTROL.entries");
  for (i = 0; i < num_entries; i++)
    {
      if (hdls[i] == entries[i])
        ok ("VIEW_CONTROL.entries[%d]: " FORMAT_REF, i, ARGS_REF (entries[i]));
      else
        fail ("VIEW_CONTROL.entries[%d]: " FORMAT_REF, i,
              ARGS_REF (entries[i]));
    }
}
