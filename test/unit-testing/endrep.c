// preR13 only
#define DWG_TYPE DWG_TYPE_ENDREP
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_RS numcols, numrows;
  BITCODE_RD colspacing, rowspacing;

  dwg_ent_endrep *_obj = dwg_object_to_ENDREP (obj);

  CHK_ENTITY_TYPE (_obj, ENDREP, numcols, RS);
  CHK_ENTITY_TYPE (_obj, ENDREP, numrows, RS);
  CHK_ENTITY_TYPE (_obj, ENDREP, colspacing, RD);
  CHK_ENTITY_TYPE (_obj, ENDREP, rowspacing, RD);
}
