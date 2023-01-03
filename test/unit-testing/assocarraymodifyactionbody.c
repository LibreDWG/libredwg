// TODO coverage
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
  dwg_obj_assocarraymodifyactionbody *_obj
      = dwg_object_to_ASSOCARRAYMODIFYACTIONBODY (obj);
  bool good = 1;

  // ASSOCACTIONBODY:
  CHK_ENTITY_TYPE (_obj, ASSOCARRAYMODIFYACTIONBODY, aab_version, BL);
  CHK_ASSOCPARAMBASEDACTIONBODY (ASSOCARRAYMODIFYACTIONBODY);

  CHK_ENTITY_TYPE (_obj, ASSOCARRAYMODIFYACTIONBODY, aaab_version, BL);
  CHK_ENTITY_UTF8TEXT (_obj, ASSOCARRAYMODIFYACTIONBODY, paramblock);
  if (!dwg_dynapi_entity_value (_obj, "ASSOCARRAYMODIFYACTIONBODY",
                                "transmatrix", &transmatrix, NULL))
    fail ("ASSOCARRAYMODIFYACTIONBODY.transmatrix");
  for (unsigned i = 0; i < 16; i += 4)
    {
      for (unsigned j = i; j < i + 4; j++)
        { /* catches nan */
          if (transmatrix[j] != _obj->transmatrix[j])
            good = 0;
        }
      if (good)
        ok ("ASSOCARRAYMODIFYACTIONBODY.transmatrix[%d]: (%f, %f, %f, %f)", i,
            transmatrix[i], transmatrix[i + 1], transmatrix[i + 2],
            transmatrix[i + 3]);
      else
        fail ("ASSOCARRAYMODIFYACTIONBODY.transmatrix[%d]: (%f, %f, %f, %f)",
              i, transmatrix[i], transmatrix[i + 1], transmatrix[i + 2],
              transmatrix[i + 3]);
    }

  CHK_ENTITY_TYPE (_obj, ASSOCARRAYMODIFYACTIONBODY, status, BS);
  CHK_ENTITY_TYPE (_obj, ASSOCARRAYMODIFYACTIONBODY, num_items, BL);
  if (!dwg_dynapi_entity_value (_obj, "ASSOCARRAYMODIFYACTIONBODY", "items",
                                &items, NULL))
    fail ("ASSOCARRAYMODIFYACTIONBODY.items");
  for (unsigned i = 0; i < num_items; i++)
    {
      CHK_SUBCLASS_TYPE (items[i], ARRAYITEMLOCATOR, itemloc1, BL);
      CHK_SUBCLASS_TYPE (items[i], ARRAYITEMLOCATOR, itemloc2, BL);
      CHK_SUBCLASS_TYPE (items[i], ARRAYITEMLOCATOR, itemloc3, BL);
      // FIXME CHK_SUBCLASS_VECTOR_TYPE (items[i], ARRAYITEMLOCATOR,
      // "itemloc[3]", 3, BL);
      // ok ("ASSOCARRAYMODIFYACTIONBODY.items[%d].itemloc[0]: " FORMAT_BL, i,
      // items[i].itemloc[0]); ok
      // ("ASSOCARRAYMODIFYACTIONBODY.items[%d].itemloc[1]: " FORMAT_BL, i,
      // items[i].itemloc[1]); ok
      // ("ASSOCARRAYMODIFYACTIONBODY.items[%d].itemloc[2]: " FORMAT_BL, i,
      // items[i].itemloc[2]);
    }
#endif
}
