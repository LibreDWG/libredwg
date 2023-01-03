#define DWG_TYPE DWG_TYPE_DICTIONARY
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_BL i, numitems;
  BITCODE_BS cloning;
  BITCODE_RC is_hardowner;
  BITCODE_TV *texts;
  BITCODE_H *itemhandles, *hdls;
  BITCODE_RC cloning_r14;

  dwg_obj_dictionary *dictionary = dwg_object_to_DICTIONARY (obj);

  CHK_ENTITY_TYPE (dictionary, DICTIONARY, numitems, BL);
  CHK_ENTITY_TYPE (dictionary, DICTIONARY, cloning, BS);
  CHK_ENTITY_TYPE (dictionary, DICTIONARY, is_hardowner, RC);
  if (!dwg_dynapi_entity_value (dictionary, "DICTIONARY", "texts", &texts,
                                NULL))
    fail ("DICTIONARY.texts");
  if (!dwg_dynapi_entity_value (dictionary, "DICTIONARY", "itemhandles",
                                &itemhandles, NULL))
    fail ("DICTIONARY.itemhandles");
  hdls = dictionary->itemhandles;
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
}
