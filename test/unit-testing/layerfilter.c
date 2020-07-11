#define DWG_TYPE DWG_TYPE_LAYERFILTER
#include "common.c"
#include "bits.h"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BL i, num_names;
  BITCODE_T *names;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_layerfilter *_obj = dwg_object_to_LAYERFILTER (obj);

  CHK_ENTITY_TYPE (_obj, LAYERFILTER, num_names, BL);
  if (!dwg_dynapi_entity_value (_obj, "LAYERFILTER", "names", &names, NULL))
    fail ("LAYERFILTER.names");
  for (i = 0; i < num_names; i++)
    {
      if (dwg_version >= R_2007)
        {
          char *name = bit_TU_to_utf8 ((BITCODE_TU)names[i]);
          ok ("LAYERFILTER.names[%d]: %s", i, name);
          free (name);
        }
      else
        {
          ok ("LAYERFILTER.names[%d]: %s", i, names[i]);
        }
    }
}
