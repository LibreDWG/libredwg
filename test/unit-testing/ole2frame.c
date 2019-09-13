#define DWG_TYPE DWG_TYPE_OLE2FRAME
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_BS type, mode;
  BITCODE_BL data_length;
  BITCODE_RC lock_aspect;
  char *data, *data1;

  dwg_ent_ole2frame *ole2frame = dwg_object_to_OLE2FRAME (obj);

  CHK_ENTITY_TYPE_W_OLD (ole2frame, OLE2FRAME, type, BS, type);
  CHK_ENTITY_TYPE_W_OLD (ole2frame, OLE2FRAME, mode, BS, mode);
  CHK_ENTITY_TYPE (ole2frame, OLE2FRAME, lock_aspect, RC, lock_aspect);

  CHK_ENTITY_TYPE_W_OLD (ole2frame, OLE2FRAME, data_length, BL, data_length);
  // CHK_ENTITY_TYPE_W_OLD (ole2frame, OLE2FRAME, data, RC*, data);
  if (!dwg_dynapi_entity_value (ole2frame, "OLE2FRAME", "data", &data, NULL))
    fail ("OLE2FRAME.data");
  data1 = dwg_ent_ole2frame_get_data (ole2frame, &error);
  // TODO wrong dynapi with TF?
  if (error || data != data1
#if 0
      || memcmp (&data, &data1, data_length)
#endif
  )
    fail ("TODO: dynapi data: %p, old api data: %p", data, data1);
  else
    ok ("OLE2FRAME.data: %p == old api data: %p", data, data1);
}
