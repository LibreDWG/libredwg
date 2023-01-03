#define DWG_TYPE DWG_TYPE_DICTIONARYWDFLT
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BL i, numitems;
  BITCODE_BS cloning;
  BITCODE_RC is_hardowner;
  BITCODE_TV *texts;
  BITCODE_H *itemhandles, *hdls;
  BITCODE_H defaultid;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_dictionarywdflt *_obj = dwg_object_to_DICTIONARYWDFLT (obj);

  CHK_ENTITY_TYPE (_obj, DICTIONARYWDFLT, numitems, BL);
  CHK_ENTITY_TYPE (_obj, DICTIONARYWDFLT, cloning, BS);
  CHK_ENTITY_TYPE (_obj, DICTIONARYWDFLT, is_hardowner, RC);
  if (!dwg_dynapi_entity_value (_obj, "DICTIONARYWDFLT", "texts", &texts,
                                NULL))
    fail ("DICTIONARYWDFLT.texts");
  if (!dwg_dynapi_entity_value (_obj, "DICTIONARYWDFLT", "itemhandles",
                                &itemhandles, NULL))
    fail ("DICTIONARYWDFLT.itemhandles");
  hdls = _obj->itemhandles;
  for (i = 0; i < numitems; i++)
    {
      ok ("DICTIONARY.texts[%d]: %s", i, texts[i]);
      if (hdls[i] == itemhandles[i])
        ok ("DICTIONARY.itemhandles[%d]: " FORMAT_REF, i,
            ARGS_REF (itemhandles[i]));
      else
        fail ("DICTIONARY.itemhandles[%d]: " FORMAT_REF, i,
              ARGS_REF (itemhandles[i]));
    }

  CHK_ENTITY_H (_obj, DICTIONARYWDFLT, defaultid);
}
