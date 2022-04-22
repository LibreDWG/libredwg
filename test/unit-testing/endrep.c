// preR13 only
#define DWG_TYPE DWG_TYPE_ENDREP
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_RS num_cols, num_rows;
  BITCODE_RD col_spacing, row_spacing;

  dwg_ent_endrep *_obj = dwg_object_to_ENDREP (obj);

  CHK_ENTITY_TYPE (_obj, ENDREP, num_cols, RS);
  CHK_ENTITY_TYPE (_obj, ENDREP, num_rows, RS);
  CHK_ENTITY_TYPE (_obj, ENDREP, col_spacing, RD);
  CHK_ENTITY_TYPE (_obj, ENDREP, row_spacing, RD);
}
