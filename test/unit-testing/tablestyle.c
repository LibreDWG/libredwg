// TODO unstable
#define DWG_TYPE DWG_TYPE_TABLESTYLE
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  // BITCODE_BL class_version;
  BITCODE_BS flags;
  BITCODE_BS flow_direction;
  BITCODE_BD horiz_cell_margin;
  BITCODE_BD vert_cell_margin;
  BITCODE_B is_title_suppressed;
  BITCODE_B is_header_suppressed;
  BITCODE_RC unknown_rc;
  BITCODE_BL i, j, unknown_bl1;
  BITCODE_BL unknown_bl2;
  BITCODE_H cellstyle_handle;
  Dwg_TABLE_Cell cell;
  Dwg_TABLESTYLE_cellstyle cellstyle; // embedded structs
  Dwg_TABLESTYLE_rowstyle rowstyle;
  Dwg_TABLESTYLE_border border;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_tablestyle *tblstyle = dwg_object_to_TABLESTYLE (obj);

  // CHK_ENTITY_TYPE (tblstyle, TABLESTYLE, class_version, BL);
  {
    BITCODE_T name;
    CHK_ENTITY_UTF8TEXT (tblstyle, TABLESTYLE, name);
  }
  CHK_ENTITY_TYPE (tblstyle, TABLESTYLE, flow_direction, BS);
  CHK_ENTITY_TYPE (tblstyle, TABLESTYLE, horiz_cell_margin, BD);
  CHK_ENTITY_TYPE (tblstyle, TABLESTYLE, vert_cell_margin, BD);
  CHK_ENTITY_TYPE (tblstyle, TABLESTYLE, is_title_suppressed, B);
  CHK_ENTITY_TYPE (tblstyle, TABLESTYLE, is_header_suppressed, B);
  if (dwg_version >= R_2010)
    {
      CHK_ENTITY_TYPE (tblstyle, TABLESTYLE, unknown_rc, RC);
      CHK_ENTITY_TYPE (tblstyle, TABLESTYLE, unknown_bl1, BL);
      CHK_ENTITY_TYPE (tblstyle, TABLESTYLE, unknown_bl2, BL);
      CHK_ENTITY_H (tblstyle, TABLESTYLE, cellstyle_handle);
      if (!dwg_dynapi_entity_value (tblstyle, "TABLESTYLE", "cellstyle",
                                    &cellstyle, NULL))
        fail ("TABLESTYLE.cellstyle");
      CHK_SUBCLASS_TYPE (cellstyle, TABLESTYLE_cellstyle, id, BL);
      CHK_SUBCLASS_TYPE (cellstyle, TABLESTYLE_cellstyle, type, BL);
      CHK_SUBCLASS_UTF8TEXT (cellstyle, TABLESTYLE_cellstyle, name);
      if (!dwg_dynapi_entity_value (&cellstyle, "TABLESTYLE_cellstyle", "cell",
                                    &cell, NULL))
        fail ("TABLESTYLE.cellstyle.cell");
    }

    /* FIXME: border is no subclass. There's no proper parent id to get to the
     * obj */
#define CHK_BORDER(row, name)                                                 \
  if (!dwg_dynapi_subclass_value (&rowstyle, "TABLESTYLE_rowstyle", #name,    \
                                  &border, NULL))                             \
    fail ("TABLESTYLE." #row "_rowstyle." #name "_border");                   \
  CHK_SUBCLASS_TYPE (border, TABLESTYLE_border, linewt, BSd);                 \
  CHK_SUBCLASS_TYPE (border, TABLESTYLE_border, visible, B);                  \
  CHK_SUBCLASS_CMC (border, TABLESTYLE_border, color)

  if (!dwg_dynapi_entity_value (tblstyle, "TABLESTYLE", "data_rowstyle",
                                &rowstyle, NULL))
    fail ("TABLESTYLE.data_rowstyle");
  CHK_SUBCLASS_H (rowstyle, TABLESTYLE_rowstyle, text_style);
  CHK_SUBCLASS_TYPE (rowstyle, TABLESTYLE_rowstyle, text_height, BD);
  CHK_SUBCLASS_TYPE (rowstyle, TABLESTYLE_rowstyle, text_alignment, BS);
  CHK_SUBCLASS_CMC (rowstyle, TABLESTYLE_rowstyle, text_color);
  CHK_SUBCLASS_CMC (rowstyle, TABLESTYLE_rowstyle, fill_color);
  CHK_SUBCLASS_TYPE (rowstyle, TABLESTYLE_rowstyle, has_bgcolor, B);
  CHK_BORDER (data, top);
  CHK_BORDER (data, hor);
  CHK_BORDER (data, bot);
  CHK_BORDER (data, left);
  CHK_BORDER (data, vert);
  CHK_BORDER (data, right);
  if (dwg_version >= R_2007)
    {
      CHK_SUBCLASS_TYPE (rowstyle, TABLESTYLE_rowstyle, data_type, BL);
      CHK_SUBCLASS_TYPE (rowstyle, TABLESTYLE_rowstyle, unit_type, BL);
      CHK_SUBCLASS_UTF8TEXT (rowstyle, TABLESTYLE_rowstyle, format_string);
    }

  if (!dwg_dynapi_entity_value (tblstyle, "TABLESTYLE", "title_rowstyle",
                                &rowstyle, NULL))
    fail ("TABLESTYLE.title_rowstyle");
  CHK_SUBCLASS_H (rowstyle, TABLESTYLE_rowstyle, text_style);
  CHK_SUBCLASS_TYPE (rowstyle, TABLESTYLE_rowstyle, text_height, BD);
  CHK_SUBCLASS_TYPE (rowstyle, TABLESTYLE_rowstyle, text_alignment, BS);
  CHK_SUBCLASS_CMC (rowstyle, TABLESTYLE_rowstyle, text_color);
  CHK_SUBCLASS_CMC (rowstyle, TABLESTYLE_rowstyle, fill_color);
  CHK_SUBCLASS_TYPE (rowstyle, TABLESTYLE_rowstyle, has_bgcolor, B);
  CHK_BORDER (title, top);
  CHK_BORDER (title, hor);
  CHK_BORDER (title, bot);
  CHK_BORDER (title, left);
  CHK_BORDER (title, vert);
  CHK_BORDER (title, right);
  if (dwg_version >= R_2007)
    {
      CHK_SUBCLASS_TYPE (rowstyle, TABLESTYLE_rowstyle, data_type, BL);
      CHK_SUBCLASS_TYPE (rowstyle, TABLESTYLE_rowstyle, unit_type, BL);
      CHK_SUBCLASS_UTF8TEXT (rowstyle, TABLESTYLE_rowstyle, format_string);
    }

  if (!dwg_dynapi_entity_value (tblstyle, "TABLESTYLE", "header_rowstyle",
                                &rowstyle, NULL))
    fail ("TABLESTYLE.header_rowstyle");
  CHK_SUBCLASS_H (rowstyle, TABLESTYLE_rowstyle, text_style);
  CHK_SUBCLASS_TYPE (rowstyle, TABLESTYLE_rowstyle, text_height, BD);
  CHK_SUBCLASS_TYPE (rowstyle, TABLESTYLE_rowstyle, text_alignment, BS);
  CHK_SUBCLASS_CMC (rowstyle, TABLESTYLE_rowstyle, text_color);
  CHK_SUBCLASS_CMC (rowstyle, TABLESTYLE_rowstyle, fill_color);
  CHK_SUBCLASS_TYPE (rowstyle, TABLESTYLE_rowstyle, has_bgcolor, B);
  CHK_BORDER (header, top);
  CHK_BORDER (header, hor);
  CHK_BORDER (header, bot);
  CHK_BORDER (header, left);
  CHK_BORDER (header, vert);
  CHK_BORDER (header, right);
  if (dwg_version >= R_2007)
    {
      CHK_SUBCLASS_TYPE (rowstyle, TABLESTYLE_rowstyle, data_type, BL);
      CHK_SUBCLASS_TYPE (rowstyle, TABLESTYLE_rowstyle, unit_type, BL);
      CHK_SUBCLASS_UTF8TEXT (rowstyle, TABLESTYLE_rowstyle, format_string);
    }
}
