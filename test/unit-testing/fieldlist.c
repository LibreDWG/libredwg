#define DWG_TYPE DWG_TYPE_FIELDLIST
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_B unknown;
  BITCODE_BL i, num_fields;
  BITCODE_H *fields, *hdls;

  dwg_obj_fieldlist *_obj = dwg_object_to_FIELDLIST (obj);

  CHK_ENTITY_TYPE (_obj, FIELDLIST, unknown, B);
  CHK_ENTITY_TYPE (_obj, FIELDLIST, num_fields, BL);
  if (!dwg_dynapi_entity_value (_obj, "FIELDLIST", "fields", &fields, NULL))
    fail ("FIELDLIST.fields");
  hdls = _obj->fields;
  for (i = 0; i < num_fields; i++)
    {
      if (hdls[i] == fields[i])
        ok ("FIELDLIST.fields[%d]: " FORMAT_REF, i, ARGS_REF (fields[i]));
      else
        fail ("FIELDLIST.fields[%d]: " FORMAT_REF, i, ARGS_REF (fields[i]));
    }
}
