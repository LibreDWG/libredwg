#define DWG_TYPE DWG_TYPE_CONTEXTDATAMANAGER
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_H objectcontext;
  BITCODE_BL num_submgrs;
  Dwg_CONTEXTDATA_submgr *submgrs;
  Dwg_CONTEXTDATA_dict *entries;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_contextdatamanager *_obj = dwg_object_to_CONTEXTDATAMANAGER (obj);

  CHK_ENTITY_H (_obj, CONTEXTDATAMANAGER, objectcontext);
  CHK_ENTITY_TYPE (_obj, CONTEXTDATAMANAGER, num_submgrs, BL);
  if (!dwg_dynapi_entity_value (_obj, "CONTEXTDATAMANAGER", "submgrs",
                                &submgrs, NULL))
    fail ("CONTEXTDATAMANAGER.submgrs");
  else
    for (BITCODE_BL i = 0; i < num_submgrs; i++)
      {
        CHK_SUBCLASS_H (submgrs[i], CONTEXTDATA_submgr, handle);
        CHK_SUBCLASS_TYPE (submgrs[i], CONTEXTDATA_submgr, num_entries, BL);
        if (!dwg_dynapi_subclass_value (&submgrs[i], "CONTEXTDATA_submgr",
                                        "entries", &entries, NULL))
          fail ("CONTEXTDATA_submgr.entries");
        else
          for (BITCODE_BL j = 0; j < submgrs[i].num_entries; j++)
            {
              CHK_SUBCLASS_UTF8TEXT (submgrs[i].entries[j], CONTEXTDATA_dict,
                                     text);
              CHK_SUBCLASS_H (submgrs[i].entries[j], CONTEXTDATA_dict,
                              itemhandle);
            }
      }
}
