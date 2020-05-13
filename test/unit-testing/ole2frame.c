#define DWG_TYPE DWG_TYPE_OLE2FRAME
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_BS type, mode;
  BITCODE_BL data_size;
  BITCODE_RC lock_aspect;
  BITCODE_TF data;
  BITCODE_TF data1;

  dwg_ent_ole2frame *ole2frame = dwg_object_to_OLE2FRAME (obj);

  CHK_ENTITY_TYPE_W_OLD (ole2frame, OLE2FRAME, type, BS);
  CHK_ENTITY_TYPE_W_OLD (ole2frame, OLE2FRAME, mode, BS);
  CHK_ENTITY_TYPE (ole2frame, OLE2FRAME, lock_aspect, RC);

  CHK_ENTITY_TYPE_W_OLD (ole2frame, OLE2FRAME, data_size, BL);
  CHK_ENTITY_TYPE_W_OLD (ole2frame, OLE2FRAME, data, TF);
}
