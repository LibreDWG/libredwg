#define DWG_TYPE DWG_TYPE_OLEFRAME
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_BS flag;
  BITCODE_BS mode;
  BITCODE_BL data_size;
  BITCODE_TF data;

  dwg_ent_oleframe *oleframe = dwg_object_to_OLEFRAME (obj);

  CHK_ENTITY_TYPE (oleframe, OLEFRAME, flag, BS);
  CHK_ENTITY_TYPE (oleframe, OLEFRAME, mode, BS);
  CHK_ENTITY_TYPE (oleframe, OLEFRAME, data_size, BL);
  CHK_ENTITY_TYPE (oleframe, OLEFRAME, data, TF);
}
