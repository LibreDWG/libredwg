// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_CELLSTYLEMAP
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BL i, num_cells;
  Dwg_CELLSTYLEMAP_Cell* cells;

#ifdef DEBUG_CLASSES
  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_cellstylemap *_obj = dwg_object_to_CELLSTYLEMAP (obj);

  CHK_ENTITY_TYPE (_obj, CELLSTYLEMAP, num_cells, BL, num_cells);
  if (!dwg_dynapi_entity_value (_obj, "CELLSTYLEMAP", "cells", &cells, NULL))
    fail ("CELLSTYLEMAP.cells");
  else
    for (i = 0; i < num_cells; i++)
      {
        CHK_SUBCLASS_TYPE (cells[i], CELLSTYLEMAP_Cell, id, BL);
        CHK_SUBCLASS_TYPE (cells[i], CELLSTYLEMAP_Cell, type, BL);
        CHK_SUBCLASS_UTF8TEXT (cells[i], CELLSTYLEMAP_Cell, name);
      }
#endif
}
