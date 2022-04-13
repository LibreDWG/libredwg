// preR13 only
#define DWG_TYPE DWG_TYPE_ENDREP
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_RS columns;
  BITCODE_RS rows;
  BITCODE_2RD start;

  dwg_ent_endrep *_obj = dwg_object_to_ENDREP (obj);

  CHK_ENTITY_TYPE (_obj, ENDREP, columns, RS);
  CHK_ENTITY_TYPE (_obj, ENDREP, rows, RS);
  CHK_ENTITY_2RD (_obj, ENDREP, start);
}
