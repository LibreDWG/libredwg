#define DWG_TYPE DWG_TYPE_DIMSTYLE_CONTROL
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BS i, num_entries;
  BITCODE_H *entries, *hdls;
  BITCODE_RC num_morehandles;
  BITCODE_H *morehandles;

  dwg_obj_dimstyle_control *ctrl = dwg_object_to_DIMSTYLE_CONTROL (obj);

  CHK_ENTITY_TYPE (ctrl, DIMSTYLE_CONTROL, num_entries, BS);
  entries = dwg_object_tablectrl_get_entries (obj, &error);
  if (!dwg_dynapi_entity_value (ctrl, "DIMSTYLE_CONTROL", "entries", &hdls,
                                NULL))
    fail ("DIMSTYLE_CONTROL.entries");
  for (i = 0; i < num_entries; i++)
    {
      if (hdls[i] == entries[i])
        ok ("DIMSTYLE_CONTROL.entries[%d]: " FORMAT_REF, i,
            ARGS_REF (entries[i]));
      else
        fail ("DIMSTYLE_CONTROL.entries[%d]: " FORMAT_REF, i,
              ARGS_REF (entries[i]));
    }
  CHK_ENTITY_TYPE (ctrl, DIMSTYLE_CONTROL, num_morehandles, RC);
  if (!dwg_dynapi_entity_value (ctrl, "DIMSTYLE_CONTROL", "morehandles",
                                &morehandles, NULL))
    fail ("DIMSTYLE_CONTROL.morehandles");
  for (i = 0; i < num_morehandles; i++)
    {
      ok ("DIMSTYLE_CONTROL.morehandles[%d]: " FORMAT_REF, i,
          ARGS_REF (morehandles[i]));
    }
}
