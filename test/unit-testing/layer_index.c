#define DWG_TYPE DWG_TYPE_LAYER_INDEX
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_TIMEBLL last_updated;
  BITCODE_BL i, num_entries;
  Dwg_LAYER_entry *entries;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_layer_index *_obj = dwg_object_to_LAYER_INDEX (obj);

  CHK_ENTITY_TIMEBLL (_obj, LAYER_INDEX, last_updated);
  CHK_ENTITY_TYPE (_obj, LAYER_INDEX, num_entries, BL);
  if (!dwg_dynapi_entity_value (_obj, "LAYER_INDEX", "entries", &entries,
                                NULL))
    fail ("LAYER_INDEX.entries");
  for (i = 0; i < num_entries; i++)
    {
      CHK_SUBCLASS_UTF8TEXT (entries[i], LAYER_entry, name);
      CHK_SUBCLASS_H (entries[i], LAYER_entry, handle);
      CHK_SUBCLASS_TYPE (entries[i], LAYER_entry, numlayers, BL);
    }
}
