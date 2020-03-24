#define DWG_TYPE DWG_TYPE_STYLE_CONTROL
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BS i, num_entries;
  BITCODE_H *entries, *hdls;
  BITCODE_BL objid;

  dwg_obj_style_control *ctrl = dwg_object_to_STYLE_CONTROL (obj);

  CHK_ENTITY_TYPE (ctrl, STYLE_CONTROL, num_entries, BS, num_entries);
  entries = dwg_object_tablectrl_get_entries (obj, &error);
  if (!dwg_dynapi_entity_value (ctrl, "STYLE_CONTROL", "entries", &hdls, NULL))
    fail ("STYLE_CONTROL.entries");
  for (i = 0; i < num_entries; i++)
    {
      if (hdls[i] == entries[i])
        ok ("STYLE_CONTROL.entries[%d]: " FORMAT_REF, i, ARGS_REF (entries[i]));
      else
        fail ("STYLE_CONTROL.entries[%d]: " FORMAT_REF, i, ARGS_REF (entries[i]));
    }
}
