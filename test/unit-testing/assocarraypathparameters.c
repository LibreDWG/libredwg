// unstable
#define DWG_TYPE DWG_TYPE_ASSOCARRAYPATHPARAMETERS
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  ASSOCARRAYPARAMETERS_fields;
  BITCODE_BL numitems;
  BITCODE_BL numrows;
  BITCODE_BL numlevels;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  // #ifdef DEBUG_CLASSES
  dwg_obj_assocarraypathparameters *_obj
      = dwg_object_to_ASSOCARRAYPATHPARAMETERS (obj);

  CHK_ENTITY_TYPE (_obj, ASSOCARRAYPATHPARAMETERS, aap_version, BL);
  CHK_ENTITY_TYPE (_obj, ASSOCARRAYPATHPARAMETERS, num_items, BL);
  if (!dwg_dynapi_entity_value (_obj, "ASSOCARRAYPATHPARAMETERS", "items",
                                &items, NULL))
    fail ("ASSOCARRAYPATHPARAMETERS.items");
  else
    for (unsigned i = 0; i < num_items; i++)
      {
        BITCODE_BL itemloc[3];
        CHK_SUBCLASS_TYPE (items[i], ASSOCARRAYITEM, class_version, BL);
        if (dwg_dynapi_subclass_value (&items[i], "ASSOCARRAYITEM",
                                       "itemloc[3]", &itemloc, NULL))
          {
            ok ("ASSOCARRAYITEM.itemloc[0]: " FORMAT_BL, itemloc[0]);
            ok ("ASSOCARRAYITEM.itemloc[1]: " FORMAT_BL, itemloc[1]);
            ok ("ASSOCARRAYITEM.itemloc[2]: " FORMAT_BL, itemloc[2]);
          }
        else
          fail ("ASSOCARRAYITEM.itemloc[3]");
        CHK_SUBCLASS_TYPE (items[i], ASSOCARRAYITEM, flags, BL);
        CHK_SUBCLASS_VECTOR_TYPE (items[i], ASSOCARRAYITEM, transmatrix, 16,
                                  BD);
        CHK_SUBCLASS_VECTOR_TYPE (items[i], ASSOCARRAYITEM, rel_transform, 16,
                                  BD);
        CHK_SUBCLASS_H (items[i], ASSOCARRAYITEM, h1);
        CHK_SUBCLASS_H (items[i], ASSOCARRAYITEM, h2);
      }
  CHK_ENTITY_TYPE (_obj, ASSOCARRAYPATHPARAMETERS, numitems, BL);
  CHK_ENTITY_TYPE (_obj, ASSOCARRAYPATHPARAMETERS, numrows, BL);
  CHK_ENTITY_TYPE (_obj, ASSOCARRAYPATHPARAMETERS, numlevels, BL);
  // #endif
}
