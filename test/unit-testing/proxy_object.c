// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_PROXY_OBJECT
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_BL class_id;
  BITCODE_BL version;
  BITCODE_BL maint_version;
  BITCODE_B from_dxf;
  BITCODE_BL data_numbits;
  BITCODE_BL data_size;
  BITCODE_RC *data;
  BITCODE_BL i, num_objids;
  BITCODE_H *objids;

  dwg_obj_proxy *_obj = dwg_object_to_PROXY_OBJECT (obj);
  CHK_ENTITY_TYPE (_obj, PROXY_OBJECT, class_id, BL, class_id);
  if (class_id != 499)
    fail ("PROXY_OBJECT.class_id %d not 499", (int)class_id);
  CHK_ENTITY_TYPE (_obj, PROXY_OBJECT, version, BL, version);
  CHK_ENTITY_TYPE (_obj, PROXY_OBJECT, maint_version, BL, maint_version);
  CHK_ENTITY_TYPE (_obj, PROXY_OBJECT, from_dxf, B, from_dxf);
  CHK_ENTITY_TYPE (_obj, PROXY_OBJECT, data_numbits, BL, data_numbits);
  CHK_ENTITY_TYPE (_obj, PROXY_OBJECT, data_size, BL, data_size);
  if (data_size * 8 < data_numbits || data_size > obj->size)
    fail ("Invalid PROXY_ENTITY.data_size %u * 8 < %u data_numbits",
          (unsigned)data_size, (unsigned)data_numbits);
  //BITCODE_RC *data;
  CHK_ENTITY_TYPE (_obj, PROXY_OBJECT, num_objids, BL, num_objids);
  if (!dwg_dynapi_entity_value (_obj, "PROXY_OBJECT", "objids", &objids, NULL))
    fail ("PROXY_OBJECT.objids");
  for (i = 0; i < num_objids; i++)
    {
      if (_obj->objids[i] == objids[i])
        ok ("PROXY_OBJECT.objids[%d]: " FORMAT_REF, i, ARGS_REF (objids[i]));
      else
        fail ("PROXY_OBJECT.objids[%d]: " FORMAT_REF, i, ARGS_REF (objids[i]));
    }
}
