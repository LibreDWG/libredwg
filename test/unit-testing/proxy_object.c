// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_PROXY_OBJECT
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_BL proxy_id;
  BITCODE_BL class_id;
  BITCODE_BL dwg_version;
  BITCODE_BL maint_version;
  BITCODE_B from_dxf;
  BITCODE_BL data_numbits;
  BITCODE_BL data_size;
  BITCODE_RC *data;
  BITCODE_BL i, num_objids;
  BITCODE_H *objids;

  dwg_obj_proxy *_obj = dwg_object_to_PROXY_OBJECT (obj);
  CHK_ENTITY_TYPE (_obj, PROXY_OBJECT, proxy_id, BL);
  if (proxy_id != 499)
    fail ("PROXY_OBJECT.proxy_id %d not 499", (int)proxy_id);
  CHK_ENTITY_TYPE (_obj, PROXY_OBJECT, class_id, BL); // index in CLASSES
  CHK_ENTITY_TYPE (_obj, PROXY_OBJECT, dwg_version, BL);
  CHK_ENTITY_TYPE (_obj, PROXY_OBJECT, maint_version, BL);
  CHK_ENTITY_TYPE (_obj, PROXY_OBJECT, from_dxf, B);
  CHK_ENTITY_TYPE (_obj, PROXY_OBJECT, data_numbits, BL);
  CHK_ENTITY_TYPE (_obj, PROXY_OBJECT, data_size, BL);
  if (data_size * 8 < data_numbits || data_size > obj->size)
    fail ("Invalid PROXY_ENTITY.data_size %u * 8 < %u data_numbits",
          (unsigned)data_size, (unsigned)data_numbits);
  CHK_ENTITY_BINARY (_obj, PROXY_OBJECT, data, data_size);
  CHK_ENTITY_TYPE (_obj, PROXY_OBJECT, num_objids, BL);
  if (!dwg_dynapi_entity_value (_obj, "PROXY_OBJECT", "objids", &objids, NULL))
    fail ("PROXY_OBJECT.objids");
  for (i = 0; i < num_objids; i++)
    {
      if (!objids[i]) // can be NULL
        {
          if (!_obj->objids[i])
            ok ("PROXY_OBJECT.objids[%d]: NULL", i);
          else
            fail ("PROXY_OBJECT.objids[%d]: NULL", i);
        }
      else
        {
          if (_obj->objids[i] == objids[i])
            ok ("PROXY_OBJECT.objids[%d]: " FORMAT_REF, i,
                ARGS_REF (objids[i]));
          else
            fail ("PROXY_OBJECT.objids[%d]: " FORMAT_REF, i,
                  ARGS_REF (objids[i]));
        }
    }
}
