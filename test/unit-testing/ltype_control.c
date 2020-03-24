#define DWG_TYPE DWG_TYPE_LTYPE_CONTROL
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BS i, num_entries;
  BITCODE_H *entries, *hdls;
  BITCODE_BL objid;
  BITCODE_H bylayer;
  BITCODE_H byblock;

  dwg_obj_ltype_control *ctrl = dwg_object_to_LTYPE_CONTROL (obj);

  CHK_ENTITY_TYPE (ctrl, LTYPE_CONTROL, num_entries, BS, num_entries);
  entries = dwg_object_tablectrl_get_entries (obj, &error);
  if (!dwg_dynapi_entity_value (ctrl, "LTYPE_CONTROL", "entries", &hdls, NULL))
    fail ("LTYPE_CONTROL.entries");
  for (i = 0; i < num_entries; i++)
    {
      if (hdls[i] == entries[i])
        ok ("LTYPE_CONTROL.entries[%d]: " FORMAT_REF, i, ARGS_REF (entries[i]));
      else
        fail ("LTYPE_CONTROL.entries[%d]: " FORMAT_REF, i, ARGS_REF (entries[i]));
    }
  CHK_ENTITY_TYPE (ctrl, LTYPE_CONTROL, objid, BL, objid);  
  CHK_ENTITY_H (ctrl, LTYPE_CONTROL, bylayer, bylayer);
  CHK_ENTITY_H (ctrl, LTYPE_CONTROL, byblock, byblock);
}
