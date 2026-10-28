#define DWG_TYPE DWG_TYPE_LAYER_FILTER
#include "tests_common.c"
#include "bits.h"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BL i, num_names;
  BITCODE_T *names;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_layer_filter *_obj = dwg_object_to_LAYER_FILTER (obj);

  CHK_ENTITY_TYPE (_obj, LAYER_FILTER, num_names, BL);
  if (!dwg_dynapi_entity_value (_obj, "LAYER_FILTER", "names", &names, NULL))
    fail ("LAYER_FILTER.names");
  for (i = 0; i < num_names; i++)
    {
      if (dwg_version >= R_2007)
        {
          char *name = bit_convert_TU ((BITCODE_TU)names[i]);
          ok ("LAYER_FILTER.names[%d]: %s", i, name);
          free (name);
        }
      else
        {
          ok ("LAYER_FILTER.names[%d]: %s", i, names[i]);
        }
    }
}
