#define DWG_TYPE DWG_TYPE_VBA_PROJECT
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_BL data_size;
  BITCODE_TF data;
  dwg_obj_vba_project *_obj = dwg_object_to_VBA_PROJECT (obj);

  CHK_ENTITY_TYPE (_obj, VBA_PROJECT, data_size, BL);
  CHK_ENTITY_TYPE (_obj, VBA_PROJECT, data, TF);
}
