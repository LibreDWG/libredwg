#define DWG_TYPE DWG_TYPE_ASSOCARRAYMODIFYACTIONBODY
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  ASSOCARRAYACTIONBODY_fields;
  BITCODE_H *deps;
  BITCODE_BS status;
  BITCODE_BL num_items;
  Dwg_ARRAYITEMLOCATOR *items;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_assocarraymodifyactionbody *_obj = dwg_object_to_ASSOCARRAYMODIFYACTIONBODY (obj);
  bool good = 1;

  // ASSOCACTIONPARAM:
  CHK_ENTITY_TYPE (_obj, ASSOCARRAYMODIFYACTIONBODY, is_r2013, BS);
  CHK_ENTITY_TYPE (_obj, ASSOCARRAYMODIFYACTIONBODY, aap_version, BL);
  CHK_ENTITY_UTF8TEXT (_obj, ASSOCARRAYMODIFYACTIONBODY, name);
  CHK_ASSOCPARAMBASEDACTIONBODY (ASSOCARRAYMODIFYACTIONBODY);

  CHK_ENTITY_TYPE (_obj, ASSOCARRAYACTIONBODY, aaab_version, BL);
  CHK_ENTITY_UTF8TEXT (_obj, ASSOCARRAYACTIONBODY, aaab_paramblock);
  if (!dwg_dynapi_entity_value (_obj, "ASSOCARRAYACTIONBODY", "aaab_transmatrix[16]", &aaab_transmatrix, NULL))
    fail ("ASSOCARRAYACTIONBODY.aaab_transmatrix[16]");
  for (unsigned i = 0; i < 16; i += 4)
    {
      for (unsigned j = i; j < i + 4; j++)
        { /* catches nan */
          if (aaab_transmatrix[j] != _obj->aaab_transmatrix[j])
            good = 0;
        }
      if (good)
        ok ("ASSOCARRAYACTIONBODY.aaab_transmatrix[%d]: (%f, %f, %f, %f)", i, aaab_transmatrix[i],
            aaab_transmatrix[i + 1], aaab_transmatrix[i + 2], aaab_transmatrix[i + 3]);
      else
        fail ("ASSOCARRAYACTIONBODY.aaab_transmatrix[%d]: (%f, %f, %f, %f)", i, aaab_transmatrix[i],
              aaab_transmatrix[i + 1], aaab_transmatrix[i + 2], aaab_transmatrix[i + 3]);
    }

  CHK_ENTITY_TYPE (_obj, ASSOCARRAYMODIFYACTIONBODY, status, BS);
  CHK_ENTITY_TYPE (_obj, ASSOCARRAYMODIFYACTIONBODY, num_items, BL);
  if (!dwg_dynapi_entity_value (_obj, "ASSOCARRAYACTIONBODY", "items", &items, NULL))
    fail ("ASSOCARRAYACTIONBODY.items");
  for (unsigned i = 0; i < num_items; i++)
    {
      CHK_SUBCLASS_TYPE (items[i], ARRAYITEMLOCATOR, itemloc1, BL);
      CHK_SUBCLASS_TYPE (items[i], ARRAYITEMLOCATOR, itemloc2, BL);
      CHK_SUBCLASS_TYPE (items[i], ARRAYITEMLOCATOR, itemloc3, BL);
      // FIXME CHK_SUBCLASS_VECTOR_TYPE (items[i], ARRAYITEMLOCATOR, "itemloc[3]", 3, BL);
      //ok ("ASSOCARRAYACTIONBODY.items[%d].itemloc[0]: " FORMAT_BL, i, items[i].itemloc[0]);
      //ok ("ASSOCARRAYACTIONBODY.items[%d].itemloc[1]: " FORMAT_BL, i, items[i].itemloc[1]);
      //ok ("ASSOCARRAYACTIONBODY.items[%d].itemloc[2]: " FORMAT_BL, i, items[i].itemloc[2]);
    }  
#endif
}
