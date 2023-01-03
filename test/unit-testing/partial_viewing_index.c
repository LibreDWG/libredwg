// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_PARTIAL_VIEWING_INDEX
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BL num_entries, i;
  BITCODE_B has_entries;
  Dwg_PARTIAL_VIEWING_INDEX_Entry *entries;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_partial_viewing_index *_obj
      = dwg_object_to_PARTIAL_VIEWING_INDEX (obj);

  CHK_ENTITY_TYPE (_obj, PARTIAL_VIEWING_INDEX, num_entries, BL);
  CHK_ENTITY_TYPE (_obj, PARTIAL_VIEWING_INDEX, has_entries, B);

  if (!dwg_dynapi_entity_value (_obj, "PARTIAL_VIEWING_INDEX", "entries",
                                &entries, NULL))
    fail ("dynapi PARTIAL_VIEWING_INDEX.entries");
  if (num_entries)
    {
      if (!entries)
        fail ("empty PARTIAL_VIEWING_INDEX.entries");
      else
        for (i = 0; i < num_entries; i++)
          {
            CHK_SUBCLASS_2RD (entries[i], PARTIAL_VIEWING_INDEX_Entry,
                              extents_min);
            CHK_SUBCLASS_2RD (entries[i], PARTIAL_VIEWING_INDEX_Entry,
                              extents_max);
            CHK_SUBCLASS_H (entries[i], PARTIAL_VIEWING_INDEX_Entry, object);
          }
    }
#endif
}
