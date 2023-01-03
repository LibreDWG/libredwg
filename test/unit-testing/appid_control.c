#define DWG_TYPE DWG_TYPE_APPID_CONTROL
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BS i, num_entries;
  BITCODE_H *entries, *hdls;

  dwg_obj_appid_control *ctrl = dwg_object_to_APPID_CONTROL (obj);

  CHK_ENTITY_TYPE (ctrl, APPID_CONTROL, num_entries, BS);
  entries = dwg_object_tablectrl_get_entries (obj, &error);
  if (!dwg_dynapi_entity_value (ctrl, "APPID_CONTROL", "entries", &hdls, NULL))
    fail ("APPID_CONTROL.entries");
  for (i = 0; i < num_entries; i++)
    {
      if (hdls[i] == entries[i])
        {
          if (entries[i])
            ok ("APPID_CONTROL.entries[%d]: " FORMAT_REF, i,
                ARGS_REF (entries[i]));
          else
            ok ("APPID_CONTROL.entries[%d]: NULL", i);
        }
      else
        {
          if (entries[i])
            fail ("APPID_CONTROL.entries[%d]: " FORMAT_REF, i,
                  ARGS_REF (entries[i]));
          else
            fail ("APPID_CONTROL.entries[%d]: NULL", i);
        }
    }
}
