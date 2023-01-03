#define DWG_TYPE DWG_TYPE_SORTENTSTABLE
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_BL i, num_ents;
  BITCODE_H *sort_ents;
  BITCODE_H block_owner;
  BITCODE_H *ents, *hdls;

  dwg_obj_sortentstable *_obj = dwg_object_to_SORTENTSTABLE (obj);

  CHK_ENTITY_H (_obj, SORTENTSTABLE, block_owner);
  CHK_ENTITY_TYPE (_obj, SORTENTSTABLE, num_ents, BL);
  if (!dwg_dynapi_entity_value (_obj, "SORTENTSTABLE", "ents", &ents, NULL))
    fail ("SORTENTSTABLE.ents");
  if (!dwg_dynapi_entity_value (_obj, "SORTENTSTABLE", "sort_ents", &sort_ents,
                                NULL))
    fail ("SORTENTSTABLE.sort_ents");
  hdls = _obj->ents;
  for (i = 0; i < num_ents; i++)
    {
      if (hdls[i] == ents[i])
        {
          if (ents[i])
            ok ("SORTENTSTABLE.ents[%d]: " FORMAT_REF, i, ARGS_REF (ents[i]));
          else
            ok ("SORTENTSTABLE.ents[%d]: NULL", i);
        }
      else
        fail ("SORTENTSTABLE.ents[%d]: " FORMAT_REF, i, ARGS_REF (ents[i]));
      if (sort_ents[i])
        ok ("SORTENTSTABLE.sort_ents[%d]: " FORMAT_REF, i,
            ARGS_REF (sort_ents[i]));
      else
        ok ("SORTENTSTABLE.sort_ents[%d]: NULL", i);
    }
}
