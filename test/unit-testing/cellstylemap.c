#define DWG_TYPE DWG_TYPE_CELLSTYLEMAP
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BL i, j, num_cells;
  Dwg_TABLESTYLE_CellStyle *cells;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_cellstylemap *_obj = dwg_object_to_CELLSTYLEMAP (obj);

  CHK_ENTITY_TYPE (_obj, CELLSTYLEMAP, num_cells, BL);
  if (!dwg_dynapi_entity_value (_obj, "CELLSTYLEMAP", "cells", &cells, NULL))
    fail ("CELLSTYLEMAP.cells");
  else
    for (i = 0; i < num_cells; i++)
      {
        CHK_SUBCLASS_TYPE (cells[i], TABLESTYLE_CellStyle, id, BL);
        CHK_SUBCLASS_TYPE (cells[i], TABLESTYLE_CellStyle, type, BL);
        CHK_SUBCLASS_MAX (cells[i], TABLESTYLE_CellStyle, type, BL, 2);
        CHK_SUBCLASS_UTF8TEXT (cells[i], TABLESTYLE_CellStyle, name);
        CHK_SUBCLASS_TYPE (cells[i].cellstyle, CellStyle, type, BL);
        CHK_SUBCLASS_TYPE (cells[i].cellstyle, CellStyle, data_flags, BS);
        if (!cells[i].cellstyle.data_flags)
          continue;
        CHK_SUBCLASS_TYPE (cells[i].cellstyle, CellStyle,
                           property_override_flags, BLx);
        CHK_SUBCLASS_TYPE (cells[i].cellstyle, CellStyle, merge_flags, BLx);
        CHK_SUBCLASS_CMC (cells[i].cellstyle, CellStyle, bg_color);
        CHK_SUBCLASS_TYPE (cells[i].cellstyle, CellStyle, content_layout, BL);
        CHK_SUBCLASS_TYPE (cells[i].cellstyle.content_format, ContentFormat,
                           property_override_flags, BLx);
        CHK_SUBCLASS_TYPE (cells[i].cellstyle.content_format, ContentFormat,
                           property_override_flags, BLx);
        CHK_SUBCLASS_TYPE (cells[i].cellstyle.content_format, ContentFormat,
                           property_flags, BLx);
        CHK_SUBCLASS_TYPE (cells[i].cellstyle.content_format, ContentFormat,
                           value_data_type, BL);
        CHK_SUBCLASS_TYPE (cells[i].cellstyle.content_format, ContentFormat,
                           value_unit_type, BL);
        CHK_SUBCLASS_UTF8TEXT (cells[i].cellstyle.content_format,
                               ContentFormat, value_format_string);
        CHK_SUBCLASS_TYPE (cells[i].cellstyle.content_format, ContentFormat,
                           rotation, BD);
        CHK_SUBCLASS_TYPE (cells[i].cellstyle.content_format, ContentFormat,
                           block_scale, BD);
        CHK_SUBCLASS_TYPE (cells[i].cellstyle.content_format, ContentFormat,
                           cell_alignment, BLx);
        CHK_SUBCLASS_CMC (cells[i].cellstyle.content_format, ContentFormat,
                          content_color);
        CHK_SUBCLASS_H (cells[i].cellstyle.content_format, ContentFormat,
                        text_style);
        CHK_SUBCLASS_TYPE (cells[i].cellstyle.content_format, ContentFormat,
                           text_height, BD);
        CHK_SUBCLASS_TYPE (cells[i].cellstyle, CellStyle,
                           margin_override_flags, BS);
        if (cells[i].cellstyle.margin_override_flags)
          {
            CHK_SUBCLASS_TYPE (cells[i].cellstyle, CellStyle, vert_margin, BD);
            CHK_SUBCLASS_TYPE (cells[i].cellstyle, CellStyle, horiz_margin,
                               BD);
            CHK_SUBCLASS_TYPE (cells[i].cellstyle, CellStyle, bottom_margin,
                               BD);
            CHK_SUBCLASS_TYPE (cells[i].cellstyle, CellStyle, right_margin,
                               BD);
            CHK_SUBCLASS_TYPE (cells[i].cellstyle, CellStyle,
                               margin_horiz_spacing, BD);
            CHK_SUBCLASS_TYPE (cells[i].cellstyle, CellStyle,
                               margin_vert_spacing, BD);
          }
        CHK_SUBCLASS_TYPE (cells[i].cellstyle, CellStyle, num_borders, BL);
        for (j = 0; j < cells[i].cellstyle.num_borders; j++)
          {
            CHK_SUBCLASS_TYPE (cells[i].cellstyle.borders[j], GridFormat,
                               index_mask, BL);
            if (!cells[i].cellstyle.borders[j].index_mask)
              continue;
            CHK_SUBCLASS_TYPE (cells[i].cellstyle.borders[j], GridFormat,
                               border_overrides, BL);
            CHK_SUBCLASS_TYPE (cells[i].cellstyle.borders[j], GridFormat,
                               border_type, BL);
            CHK_SUBCLASS_CMC (cells[i].cellstyle.borders[j], GridFormat,
                              color);
            CHK_SUBCLASS_TYPE (cells[i].cellstyle.borders[j], GridFormat,
                               linewt, BLd);
            CHK_SUBCLASS_H (cells[i].cellstyle.borders[j], GridFormat, ltype);
            CHK_SUBCLASS_TYPE (cells[i].cellstyle.borders[j], GridFormat,
                               visible, B);
            CHK_SUBCLASS_TYPE (cells[i].cellstyle.borders[j], GridFormat,
                               double_line_spacing, BD);
          }
      }
}
