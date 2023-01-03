// TODO unstable
#define DWG_TYPE DWG_TYPE_TABLESTYLE
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BS class_version;
  BITCODE_BS flags;
  BITCODE_BS flow_direction;
  BITCODE_BD horiz_cell_margin;
  BITCODE_BD vert_cell_margin;
  BITCODE_B is_title_suppressed;
  BITCODE_B is_header_suppressed;
  BITCODE_BL i, j, num_rowstyles;
  Dwg_TABLESTYLE_rowstyles *rowstyles;
  Dwg_TABLESTYLE_CellStyle sty;
  BITCODE_RC unknown_rc;
  BITCODE_BL unknown_bl1;
  BITCODE_BL unknown_bl2;
  BITCODE_H cellstyle;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_tablestyle *tblstyle = dwg_object_to_TABLESTYLE (obj);

  CHK_ENTITY_TYPE (tblstyle, TABLESTYLE, class_version, BL);
  {
    BITCODE_T name;
    CHK_ENTITY_UTF8TEXT (tblstyle, TABLESTYLE, name);
  }
  CHK_ENTITY_TYPE (tblstyle, TABLESTYLE, flow_direction, BS);
  CHK_ENTITY_TYPE (tblstyle, TABLESTYLE, horiz_cell_margin, BD);
  CHK_ENTITY_TYPE (tblstyle, TABLESTYLE, vert_cell_margin, BD);
  CHK_ENTITY_TYPE (tblstyle, TABLESTYLE, is_title_suppressed, B);
  CHK_ENTITY_TYPE (tblstyle, TABLESTYLE, is_header_suppressed, B);
  CHK_ENTITY_TYPE (tblstyle, TABLESTYLE, num_rowstyles, BL);
  if (num_rowstyles != 3)
    fail ("TABLESTYLE.num_rowstyles %d != 3", num_rowstyles);
  if (!dwg_dynapi_entity_value (tblstyle, "TABLESTYLE", "rowstyles",
                                &rowstyles, NULL))
    fail ("TABLESTYLE.rowstyles");

#ifndef DEBUG_CLASSS
  if (dwg_version >= R_2010)
    {
      printf ("skip: TABLESTYLE r2010+ not yet implemented\n");
    }
#else
  if (dwg_version >= R_2010)
    {
      BITCODE_T name;
      CHK_ENTITY_TYPE (tblstyle, TABLESTYLE, unknown_rc, RC);
      CHK_ENTITY_TYPE (tblstyle, TABLESTYLE, unknown_bl1, BL);
      CHK_ENTITY_TYPE (tblstyle, TABLESTYLE, unknown_bl2, BL);
      CHK_ENTITY_H (tblstyle, TABLESTYLE, cellstyle);

      if (!dwg_dynapi_entity_value (tblstyle, "TABLESTYLE", "sty", &sty, NULL))
        fail ("TABLESTYLE.sty");
      CHK_SUBCLASS_TYPE (sty, TABLESTYLE_CellStyle, id, BL);
      CHK_SUBCLASS_TYPE (sty, TABLESTYLE_CellStyle, type, BL);
      CHK_SUBCLASS_MAX (sty, TABLESTYLE_CellStyle, type, BL, 2);
      // CHK_SUBCLASS_UTF8TEXT (sty, TABLESTYLE_CellStyle, name);

      CHK_SUBCLASS_TYPE (sty.cellstyle, CellStyle, type, BL);
      CHK_SUBCLASS_TYPE (sty.cellstyle, CellStyle, data_flags, BS);
      if (sty.cellstyle.data_flags)
        {
          CHK_SUBCLASS_TYPE (sty.cellstyle, CellStyle, property_override_flags,
                             BL);
          CHK_SUBCLASS_TYPE (sty.cellstyle, CellStyle, merge_flags, BL);
          CHK_SUBCLASS_CMC (sty.cellstyle, CellStyle, bg_color);
          CHK_SUBCLASS_TYPE (sty.cellstyle, CellStyle, content_layout, BL);
          CHK_SUBCLASS_TYPE (sty.cellstyle.content_format, ContentFormat,
                             property_override_flags, BL);
          CHK_SUBCLASS_TYPE (sty.cellstyle.content_format, ContentFormat,
                             property_override_flags, BL);
          CHK_SUBCLASS_TYPE (sty.cellstyle.content_format, ContentFormat,
                             property_flags, BL);
          CHK_SUBCLASS_TYPE (sty.cellstyle.content_format, ContentFormat,
                             value_data_type, BL);
          CHK_SUBCLASS_TYPE (sty.cellstyle.content_format, ContentFormat,
                             value_unit_type, BL);
          CHK_SUBCLASS_UTF8TEXT (sty.cellstyle.content_format, ContentFormat,
                                 value_format_string);
          CHK_SUBCLASS_TYPE (sty.cellstyle.content_format, ContentFormat,
                             rotation, BD);
          CHK_SUBCLASS_TYPE (sty.cellstyle.content_format, ContentFormat,
                             block_scale, BD);
          CHK_SUBCLASS_TYPE (sty.cellstyle.content_format, ContentFormat,
                             cell_alignment, BL);
          CHK_SUBCLASS_CMC (sty.cellstyle.content_format, ContentFormat,
                            content_color);
          CHK_SUBCLASS_H (sty.cellstyle.content_format, ContentFormat,
                          text_style);
          CHK_SUBCLASS_TYPE (sty.cellstyle.content_format, ContentFormat,
                             text_height, BD);
          CHK_SUBCLASS_TYPE (sty.cellstyle, CellStyle, margin_override_flags,
                             BS);
          if (sty.cellstyle.margin_override_flags)
            {
              CHK_SUBCLASS_TYPE (sty.cellstyle, CellStyle, vert_margin, BD);
              CHK_SUBCLASS_TYPE (sty.cellstyle, CellStyle, horiz_margin, BD);
              CHK_SUBCLASS_TYPE (sty.cellstyle, CellStyle, bottom_margin, BD);
              CHK_SUBCLASS_TYPE (sty.cellstyle, CellStyle, right_margin, BD);
              CHK_SUBCLASS_TYPE (sty.cellstyle, CellStyle,
                                 margin_horiz_spacing, BD);
              CHK_SUBCLASS_TYPE (sty.cellstyle, CellStyle, margin_vert_spacing,
                                 BD);
            }
        }
      CHK_SUBCLASS_TYPE (sty.cellstyle, CellStyle, num_borders, BL);
      for (j = 0; j < sty.cellstyle.num_borders; j++)
        {
          CHK_SUBCLASS_TYPE (sty.cellstyle.borders[j], GridFormat, index_mask,
                             BL);
          if (!sty.cellstyle.borders[j].index_mask)
            continue;
          CHK_SUBCLASS_TYPE (sty.cellstyle.borders[j], GridFormat,
                             border_overrides, BL);
          CHK_SUBCLASS_TYPE (sty.cellstyle.borders[j], GridFormat, border_type,
                             BL);
          CHK_SUBCLASS_CMC (sty.cellstyle.borders[j], GridFormat, color);
          CHK_SUBCLASS_TYPE (sty.cellstyle.borders[j], GridFormat, linewt,
                             BLd);
          CHK_SUBCLASS_H (sty.cellstyle.borders[j], GridFormat, ltype);
          CHK_SUBCLASS_TYPE (sty.cellstyle.borders[j], GridFormat, visible, B);
          CHK_SUBCLASS_TYPE (sty.cellstyle.borders[j], GridFormat,
                             double_line_spacing, BD);
        }
    }
#endif

  for (i = 0; i < num_rowstyles; i++)
    {
      CHK_SUBCLASS_H (rowstyles[i], TABLESTYLE_rowstyles, text_style);
      CHK_SUBCLASS_TYPE (rowstyles[i], TABLESTYLE_rowstyles, text_height, BD);
      CHK_SUBCLASS_TYPE (rowstyles[i], TABLESTYLE_rowstyles, text_alignment,
                         BS);
      CHK_SUBCLASS_CMC (rowstyles[i], TABLESTYLE_rowstyles, text_color);
      CHK_SUBCLASS_CMC (rowstyles[i], TABLESTYLE_rowstyles, fill_color);
      CHK_SUBCLASS_TYPE (rowstyles[i], TABLESTYLE_rowstyles, has_bgcolor, B);
      if (rowstyles[i].num_borders != 6)
        fail ("TABLESTYLE.rowstyles[%d].num_borders %d != 6", i,
              rowstyles[i].num_borders);
      if (rowstyles[i].borders)
        for (j = 0; j < 6; j++)
          {
            CHK_SUBCLASS_TYPE (rowstyles[i].borders[j], TABLESTYLE_border,
                               linewt, BSd);
            CHK_SUBCLASS_TYPE (rowstyles[i].borders[j], TABLESTYLE_border,
                               visible, B);
            CHK_SUBCLASS_CMC (rowstyles[i].borders[j], TABLESTYLE_border,
                              color);
          }
      if (dwg_version >= R_2007)
        {
          CHK_SUBCLASS_TYPE (rowstyles[i], TABLESTYLE_rowstyles, data_type,
                             BL);
          CHK_SUBCLASS_TYPE (rowstyles[i], TABLESTYLE_rowstyles, unit_type,
                             BL);
          CHK_SUBCLASS_UTF8TEXT (rowstyles[i], TABLESTYLE_rowstyles,
                                 format_string);
        }
    }
}
