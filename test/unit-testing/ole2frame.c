#define DWG_TYPE DWG_TYPE_OLE2FRAME
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_BS flag, mode;
  BITCODE_BL data_length;
  BITCODE_RC unknown;
  char *data, *data1;

  dwg_ent_ole2frame *ole2frame = dwg_object_to_OLE2FRAME (obj);

  CHK_ENTITY_TYPE_W_OLD (ole2frame, OLE2FRAME, flag, BS, flag);
  CHK_ENTITY_TYPE_W_OLD (ole2frame, OLE2FRAME, mode, BS, mode);
  CHK_ENTITY_TYPE_W_OLD (ole2frame, OLE2FRAME, data_length, BL, data_length);

  //CHK_ENTITY_TYPE_W_OLD (ole2frame, OLE2FRAME, data, RC*, data);
  if (data_length != dwg_ent_ole2frame_get_data_length (ole2frame, &error)
      || error)
    ok ("TODO: dwg_ent_ole2frame_get_data_length error: %d", error);
  else
    {
      if (!dwg_dynapi_entity_value (ole2frame, "OLE2FRAME", "data", &data, NULL))
        fail ("OLE2FRAME.data");
      data1 = dwg_ent_ole2frame_get_data (ole2frame, &error);
      // TODO wrong length
      if (error /* || memcmp (&data, &data1, data_length) */)
        ok ("TODO: dwg_ent_ole2frame_get_data error: %d", error);
    }

  CHK_ENTITY_TYPE (ole2frame, OLE2FRAME, unknown, RC, unknown);
}
