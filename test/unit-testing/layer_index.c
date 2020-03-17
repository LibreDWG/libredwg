#define DWG_TYPE DWG_TYPE_LAYER_INDEX
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BL timestamp1;
  BITCODE_BL timestamp2;
  BITCODE_BL i, num_entries;
  Dwg_LAYER_entry* entries;
  BITCODE_H *layer_entries, *hdls;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_layer_index *_obj = dwg_object_to_LAYER_INDEX (obj);

  CHK_ENTITY_TYPE (_obj, LAYER_INDEX, timestamp1, BL, timestamp1);
  CHK_ENTITY_TYPE (_obj, LAYER_INDEX, timestamp2, BL, timestamp2);
  CHK_ENTITY_TYPE (_obj, LAYER_INDEX, num_entries, BL, num_entries);
  if (!dwg_dynapi_entity_value (_obj, "LAYER_INDEX", "entries", &entries, NULL))
    fail ("LAYER_INDEX.entries");
  if (!dwg_dynapi_entity_value (_obj, "LAYER_INDEX", "layer_entries", &layer_entries, NULL))
    fail ("LAYER_INDEX.layer_entries");
  hdls = _obj->layer_entries;
  for (i = 0; i < num_entries; i++)
    {
      CHK_SUBCLASS_TYPE (entries[i], LAYER_entry, idxlong, BL);
      CHK_SUBCLASS_UTF8TEXT (entries[i], LAYER_entry, layername);
      if (hdls[i] == layer_entries[i])
        ok ("LAYER_INDEX.layer_entries[%d]: " FORMAT_REF, i, ARGS_REF (layer_entries[i]));
      else
        fail ("LAYER_INDEX.layer_entries[%d]: " FORMAT_REF, i, ARGS_REF (layer_entries[i]));
    }
}
