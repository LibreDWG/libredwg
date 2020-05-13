// TODO ??
#define DWG_TYPE DWG_TYPE_PROXY_ENTITY
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

  dwg_ent_proxy *_obj = dwg_object_to_PROXY_ENTITY (obj);
  CHK_ENTITY_TYPE (_obj, PROXY_ENTITY, class_id, BL);
  if (class_id != 498)
    fail ("PROXY_ENTITY.class_id %d not 498", (int)class_id);
  CHK_ENTITY_TYPE (_obj, PROXY_ENTITY, version, BL);
  CHK_ENTITY_TYPE (_obj, PROXY_ENTITY, maint_version, BL);
  CHK_ENTITY_TYPE (_obj, PROXY_ENTITY, from_dxf, B);
  CHK_ENTITY_TYPE (_obj, PROXY_ENTITY, data_numbits, BL);
  CHK_ENTITY_TYPE (_obj, PROXY_ENTITY, data_size, BL);
  if (data_size * 8 < data_numbits || data_size > obj->size)
    fail ("Invalid PROXY_ENTITY.data_size %u * 8 < %u data_numbits",
          (unsigned)data_size, (unsigned)data_numbits);
  // BITCODE_RC *data;
  CHK_ENTITY_TYPE (_obj, PROXY_ENTITY, num_objids, BL);
  if (!dwg_dynapi_entity_value (_obj, "PROXY_ENTITY", "objids", &objids, NULL))
    fail ("PROXY_ENTITY.objids");
  for (i = 0; i < num_objids; i++)
    {
      if (_obj->objids[i] == objids[i])
        ok ("PROXY_ENTITY.objids[%d]: " FORMAT_REF, i, ARGS_REF (objids[i]));
      else
        fail ("PROXY_ENTITY.objids[%d]: " FORMAT_REF, i, ARGS_REF (objids[i]));
    }
}
