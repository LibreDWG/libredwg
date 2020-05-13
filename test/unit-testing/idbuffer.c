#define DWG_TYPE DWG_TYPE_IDBUFFER
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_RC unknown;
  BITCODE_BL i, num_obj_ids;
  BITCODE_H *obj_ids, *hdls;

  dwg_obj_idbuffer *_obj = dwg_object_to_IDBUFFER (obj);

  CHK_ENTITY_TYPE (_obj, IDBUFFER, unknown, RC);
  CHK_ENTITY_TYPE (_obj, IDBUFFER, num_obj_ids, BL);
  if (!dwg_dynapi_entity_value (_obj, "IDBUFFER", "obj_ids", &obj_ids, NULL))
    fail ("IDBUFFER.obj_ids");
  hdls = _obj->obj_ids;
  for (i = 0; i < num_obj_ids; i++)
    {
      if (hdls[i] == obj_ids[i])
        ok ("IDBUFFER.obj_ids[%d]: " FORMAT_REF, i, ARGS_REF (obj_ids[i]));
      else
        fail ("IDBUFFER.obj_ids[%d]: " FORMAT_REF, i, ARGS_REF (obj_ids[i]));
    }
}
