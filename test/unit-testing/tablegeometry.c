#define DWG_TYPE DWG_TYPE_TABLEGEOMETRY
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_BL numrows;
  BITCODE_BL numcols;
  BITCODE_BL num_cells; /* = numrows * numcols */
  Dwg_TABLEGEOMETRY_Cell *cells;
  Dwg_CellContentGeometry *geometry;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_tablegeometry *_obj = dwg_object_to_TABLEGEOMETRY (obj);

  CHK_ENTITY_TYPE (_obj, TABLEGEOMETRY, numrows, BL);
  CHK_ENTITY_TYPE (_obj, TABLEGEOMETRY, numcols, BL);
  CHK_ENTITY_TYPE (_obj, TABLEGEOMETRY, num_cells, BL);
  if (!dwg_dynapi_entity_value (_obj, "TABLEGEOMETRY", "cells", &cells, NULL))
    fail ("TABLEGEOMETRY.cells");
  else
    for (BITCODE_BL i = 0; i < num_cells; i++)
      {
        CHK_SUBCLASS_TYPE (cells[i], TABLEGEOMETRY_Cell, geom_data_flag, BL);
        CHK_SUBCLASS_TYPE (cells[i], TABLEGEOMETRY_Cell, width_w_gap, BD);
        CHK_SUBCLASS_TYPE (cells[i], TABLEGEOMETRY_Cell, height_w_gap, BD);
        CHK_SUBCLASS_H (cells[i], TABLEGEOMETRY_Cell, tablegeometry);
        CHK_SUBCLASS_TYPE (cells[i], TABLEGEOMETRY_Cell, num_geometry, BL);
        if (!dwg_dynapi_subclass_value (&cells[i], "TABLEGEOMETRY_Cell",
                                        "geometry", &geometry, NULL))
          fail ("TABLEGEOMETRY.cells.geometry");
        else
          for (BITCODE_BL j = 0; j < cells[i].num_geometry; j++)
            {
              CHK_SUBCLASS_3RD (geometry[j], CellContentGeometry,
                                dist_top_left);
              CHK_SUBCLASS_3RD (geometry[j], CellContentGeometry, dist_center);
              CHK_SUBCLASS_TYPE (geometry[j], CellContentGeometry,
                                 content_width, BD);
              CHK_SUBCLASS_TYPE (geometry[j], CellContentGeometry,
                                 content_height, BD);
              CHK_SUBCLASS_TYPE (geometry[j], CellContentGeometry, width, BD);
              CHK_SUBCLASS_TYPE (geometry[j], CellContentGeometry, height, BD);
              CHK_SUBCLASS_TYPE (geometry[j], CellContentGeometry, unknown,
                                 BL);
            }
      }
}
