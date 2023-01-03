// TODO debugging
#define DWG_TYPE DWG_TYPE_TABLE
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BL i, j;
  Dwg_LinkedData ldata;
  Dwg_LinkedTableData tdata;
  Dwg_FormattedTableData fdata;
  BITCODE_H tablestyle;

  BITCODE_RC unknown_rc;
  BITCODE_H unknown_h;
  BITCODE_BL unknown_bl;
  BITCODE_B unknown_b;
  BITCODE_BL unknown_bl1;
  BITCODE_3BD ins_pt;
  BITCODE_3BD scale;
  BITCODE_BB scale_flag;
  BITCODE_BD rotation;
  BITCODE_BE extrusion;
  BITCODE_B has_attribs;
  BITCODE_BL num_owned;
  BITCODE_BS flag_for_table_value;
  BITCODE_3BD horiz_direction;
  BITCODE_BL num_cols;
  BITCODE_BL num_rows;
  unsigned long num_cells;
  BITCODE_BD *col_widths;
  BITCODE_BD *row_heights;
  Dwg_TABLE_Cell *cells;
  BITCODE_BL num_attr_defs;
  Dwg_TABLE_AttrDef *attr_defs;
  BITCODE_B has_table_overrides;
  BITCODE_BL table_flag_override;
  BITCODE_B title_suppressed;
  BITCODE_B header_suppressed;
  BITCODE_BS flow_direction;
  BITCODE_BD horiz_cell_margin;
  BITCODE_BD vert_cell_margin;
  BITCODE_CMC title_row_color;
  BITCODE_CMC header_row_color;
  BITCODE_CMC data_row_color;
  BITCODE_B title_row_fill_none;
  BITCODE_B header_row_fill_none;
  BITCODE_B data_row_fill_none;
  BITCODE_CMC title_row_fill_color;
  BITCODE_CMC header_row_fill_color;
  BITCODE_CMC data_row_fill_color;
  BITCODE_BS title_row_alignment;
  BITCODE_BS header_row_alignment;
  BITCODE_BS data_row_alignment;
  BITCODE_H title_text_style;
  BITCODE_H header_text_style;
  BITCODE_H data_text_style;
  BITCODE_BD title_row_height;
  BITCODE_BD header_row_height;
  BITCODE_BD data_row_height;
  BITCODE_B has_border_color_overrides;
  BITCODE_BL border_color_overrides_flag;
  BITCODE_CMC title_horiz_top_color;
  BITCODE_CMC title_horiz_ins_color;
  BITCODE_CMC title_horiz_bottom_color;
  BITCODE_CMC title_vert_left_color;
  BITCODE_CMC title_vert_ins_color;
  BITCODE_CMC title_vert_right_color;
  BITCODE_CMC header_horiz_top_color;
  BITCODE_CMC header_horiz_ins_color;
  BITCODE_CMC header_horiz_bottom_color;
  BITCODE_CMC header_vert_left_color;
  BITCODE_CMC header_vert_ins_color;
  BITCODE_CMC header_vert_right_color;
  BITCODE_CMC data_horiz_top_color;
  BITCODE_CMC data_horiz_ins_color;
  BITCODE_CMC data_horiz_bottom_color;
  BITCODE_CMC data_vert_left_color;
  BITCODE_CMC data_vert_ins_color;
  BITCODE_CMC data_vert_right_color;

  BITCODE_B has_border_lineweight_overrides;
  BITCODE_BL border_lineweight_overrides_flag;
  BITCODE_BS title_horiz_top_linewt;
  BITCODE_BS title_horiz_ins_linewt;
  BITCODE_BS title_horiz_bottom_linewt;
  BITCODE_BS title_vert_left_linewt;
  BITCODE_BS title_vert_ins_linewt;
  BITCODE_BS title_vert_right_linewt;
  BITCODE_BS header_horiz_top_linewt;
  BITCODE_BS header_horiz_ins_linewt;
  BITCODE_BS header_horiz_bottom_linewt;
  BITCODE_BS header_vert_left_linewt;
  BITCODE_BS header_vert_ins_linewt;
  BITCODE_BS header_vert_right_linewt;
  BITCODE_BS data_horiz_top_linewt;
  BITCODE_BS data_horiz_ins_linewt;
  BITCODE_BS data_horiz_bottom_linewt;
  BITCODE_BS data_vert_left_linewt;
  BITCODE_BS data_vert_ins_linewt;
  BITCODE_BS data_vert_right_linewt;
  BITCODE_B has_border_visibility_overrides;
  BITCODE_BL border_visibility_overrides_flag;
  BITCODE_BS title_horiz_top_visibility;
  BITCODE_BS title_horiz_ins_visibility;
  BITCODE_BS title_horiz_bottom_visibility;
  BITCODE_BS title_vert_left_visibility;
  BITCODE_BS title_vert_ins_visibility;
  BITCODE_BS title_vert_right_visibility;
  BITCODE_BS header_horiz_top_visibility;
  BITCODE_BS header_horiz_ins_visibility;
  BITCODE_BS header_horiz_bottom_visibility;
  BITCODE_BS header_vert_left_visibility;
  BITCODE_BS header_vert_ins_visibility;
  BITCODE_BS header_vert_right_visibility;
  BITCODE_BS data_horiz_top_visibility;
  BITCODE_BS data_horiz_ins_visibility;
  BITCODE_BS data_horiz_bottom_visibility;
  BITCODE_BS data_vert_left_visibility;
  BITCODE_BS data_vert_ins_visibility;
  BITCODE_BS data_vert_right_visibility;
  BITCODE_H block_header;
  BITCODE_H first_attrib;
  BITCODE_H last_attrib;
  BITCODE_H *attribs;
  BITCODE_H seqend;
  BITCODE_H title_row_style_override;
  BITCODE_H header_row_style_override;
  BITCODE_H data_row_style_override;
  BITCODE_BS unknown_bs;
  BITCODE_3BD hor_dir;
  BITCODE_BL has_break_data;
  BITCODE_BL break_flag;
  BITCODE_BL break_flow_direction;
  BITCODE_BD break_spacing;
  BITCODE_BL break_unknown1;
  BITCODE_BL break_unknown2;
  BITCODE_BL num_break_heights;
  Dwg_TABLE_BreakHeight *break_heights;
  BITCODE_BL num_break_rows;
  Dwg_TABLE_BreakRow *break_rows;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_ent_table *table = dwg_object_to_TABLE (obj);

  if (dwg_version >= R_2010)
    {
      printf ("TABLECONTENT r2010+ nyi\n"); // TODO
      // return;
    }

  CHK_ENTITY_TYPE (table, TABLE, unknown_rc, RC);
  CHK_ENTITY_H (table, TABLE, unknown_h);
  CHK_ENTITY_TYPE (table, TABLE, unknown_bl, BL);
  CHK_ENTITY_TYPE (table, TABLE, unknown_b, B);
  CHK_ENTITY_TYPE (table, TABLE, unknown_bl1, BL);
  CHK_ENTITY_3RD (table, TABLE, ins_pt);
  CHK_ENTITY_3RD (table, TABLE, scale);
  CHK_ENTITY_TYPE (table, TABLE, scale_flag, BB);
  CHK_ENTITY_TYPE (table, TABLE, rotation, BD);
  CHK_ENTITY_MAX (table, TABLE, rotation, BD, MAX_ANGLE);
  CHK_ENTITY_3RD (table, TABLE, extrusion);
  CHK_ENTITY_TYPE (table, TABLE, has_attribs, B);
  CHK_ENTITY_TYPE (table, TABLE, num_owned, BL);

  CHK_ENTITY_H (table, TABLE, tablestyle);
  CHK_ENTITY_TYPE (table, TABLE, flag_for_table_value, BS);
  CHK_ENTITY_3RD (table, TABLE, horiz_direction);
  CHK_ENTITY_TYPE (table, TABLE, num_cols, BL);
  CHK_ENTITY_TYPE (table, TABLE, num_rows, BL);
  CHK_ENTITY_TYPE (table, TABLE, num_cells, BLL);
  // BITCODE_BD* col_widths;
  // BITCODE_BD* row_heights;
  if (!dwg_dynapi_entity_value (table, "TABLE", "cells", &cells, NULL))
    fail ("TABLE.cells");
  else
    for (i = 0; i < num_cells; i++)
      {
        CHK_SUBCLASS_TYPE (cells[i], TABLE_Cell, type, BS);
        CHK_SUBCLASS_TYPE (cells[i], TABLE_Cell, flags, RC);
        CHK_SUBCLASS_TYPE (cells[i], TABLE_Cell, is_merged_value, B);
        CHK_SUBCLASS_TYPE (cells[i], TABLE_Cell, is_autofit_flag, B);
        CHK_SUBCLASS_TYPE (cells[i], TABLE_Cell, merged_width_flag, BL);
        CHK_SUBCLASS_TYPE (cells[i], TABLE_Cell, merged_height_flag, BL);
        CHK_SUBCLASS_TYPE (cells[i], TABLE_Cell, rotation, BD);
        CHK_SUBCLASS_TYPE (cells[i], TABLE_Cell, additional_data_flag, B);
        if (cells[i].type == 1)
          {
            CHK_SUBCLASS_UTF8TEXT (cells[i], TABLE_Cell, text_value);
            CHK_SUBCLASS_H (cells[i], TABLE_Cell, text_style);
          }
        else
          {
            CHK_SUBCLASS_H (cells[i], TABLE_Cell, block_handle);
            CHK_SUBCLASS_TYPE (cells[i], TABLE_Cell, block_scale, BD);
            CHK_SUBCLASS_TYPE (cells[i], TABLE_Cell, num_attr_defs, BL);
            if (!dwg_dynapi_subclass_value (&cells[i], "TABLE_Cell",
                                            "attr_defs", &attr_defs, NULL))
              fail ("TABLE.attr_defs");
            for (j = 0; j < cells[i].num_attr_defs; j++)
              {
                CHK_SUBCLASS_H (attr_defs[j], TABLE_AttrDef, attdef);
                CHK_SUBCLASS_TYPE (attr_defs[j], TABLE_AttrDef, index, BS);
                CHK_SUBCLASS_UTF8TEXT (attr_defs[j], TABLE_AttrDef, text);
              }
          }
        CHK_SUBCLASS_TYPE (cells[i], TABLE_Cell, cell_flag_override, BL);
        CHK_SUBCLASS_TYPE (cells[i], TABLE_Cell, virtual_edge_flag, RC);
        CHK_SUBCLASS_TYPE (cells[i], TABLE_Cell, cell_alignment, RS);
        CHK_SUBCLASS_TYPE (cells[i], TABLE_Cell, bg_fill_none, B);
        CHK_SUBCLASS_CMC (cells[i], TABLE_Cell, bg_color);
        CHK_SUBCLASS_CMC (cells[i], TABLE_Cell, content_color);
        CHK_SUBCLASS_TYPE (cells[i], TABLE_Cell, text_height, BD);
        CHK_SUBCLASS_CMC (cells[i], TABLE_Cell, top_grid_color);
        CHK_SUBCLASS_TYPE (cells[i], TABLE_Cell, top_grid_linewt, BS);
        CHK_SUBCLASS_TYPE (cells[i], TABLE_Cell, top_visibility, BS);
        CHK_SUBCLASS_CMC (cells[i], TABLE_Cell, right_grid_color);
        CHK_SUBCLASS_TYPE (cells[i], TABLE_Cell, right_grid_linewt, BS);
        CHK_SUBCLASS_TYPE (cells[i], TABLE_Cell, right_visibility, BS);
        CHK_SUBCLASS_CMC (cells[i], TABLE_Cell, bottom_grid_color);
        CHK_SUBCLASS_TYPE (cells[i], TABLE_Cell, bottom_grid_linewt, BS);
        CHK_SUBCLASS_TYPE (cells[i], TABLE_Cell, bottom_visibility, BS);
        CHK_SUBCLASS_CMC (cells[i], TABLE_Cell, left_grid_color);
        CHK_SUBCLASS_TYPE (cells[i], TABLE_Cell, left_grid_linewt, BS);
        CHK_SUBCLASS_TYPE (cells[i], TABLE_Cell, left_visibility, BS);
        CHK_SUBCLASS_TYPE (cells[i], TABLE_Cell, unknown, BL);
        // Dwg_TABLE_value value;
      }
  CHK_ENTITY_TYPE (table, TABLE, has_table_overrides, B);
  CHK_ENTITY_TYPE (table, TABLE, table_flag_override, BL);
  CHK_ENTITY_TYPE (table, TABLE, title_suppressed, B);
  CHK_ENTITY_TYPE (table, TABLE, header_suppressed, B);
  CHK_ENTITY_TYPE (table, TABLE, flow_direction, BS);
  CHK_ENTITY_TYPE (table, TABLE, horiz_cell_margin, BD);
  CHK_ENTITY_TYPE (table, TABLE, vert_cell_margin, BD);
  CHK_ENTITY_CMC (table, TABLE, title_row_color);
  CHK_ENTITY_CMC (table, TABLE, header_row_color);
  CHK_ENTITY_CMC (table, TABLE, data_row_color);
  CHK_ENTITY_TYPE (table, TABLE, title_row_fill_none, B);
  CHK_ENTITY_TYPE (table, TABLE, header_row_fill_none, B);
  CHK_ENTITY_TYPE (table, TABLE, data_row_fill_none, B);
  CHK_ENTITY_CMC (table, TABLE, title_row_fill_color);
  CHK_ENTITY_CMC (table, TABLE, header_row_fill_color);
  CHK_ENTITY_CMC (table, TABLE, data_row_fill_color);
  CHK_ENTITY_TYPE (table, TABLE, title_row_alignment, BS);
  CHK_ENTITY_TYPE (table, TABLE, header_row_alignment, BS);
  CHK_ENTITY_TYPE (table, TABLE, data_row_alignment, BS);
  CHK_ENTITY_H (table, TABLE, title_text_style);
  CHK_ENTITY_H (table, TABLE, header_text_style);
  CHK_ENTITY_H (table, TABLE, data_text_style);
  CHK_ENTITY_TYPE (table, TABLE, title_row_height, BD);
  CHK_ENTITY_TYPE (table, TABLE, header_row_height, BD);
  CHK_ENTITY_TYPE (table, TABLE, data_row_height, BD);
  CHK_ENTITY_TYPE (table, TABLE, has_border_color_overrides, B);
  CHK_ENTITY_TYPE (table, TABLE, border_color_overrides_flag, BL);
  CHK_ENTITY_CMC (table, TABLE, title_horiz_top_color);
  CHK_ENTITY_CMC (table, TABLE, title_horiz_ins_color);
  CHK_ENTITY_CMC (table, TABLE, title_horiz_bottom_color);
  CHK_ENTITY_CMC (table, TABLE, title_vert_left_color);
  CHK_ENTITY_CMC (table, TABLE, title_vert_ins_color);
  CHK_ENTITY_CMC (table, TABLE, title_vert_right_color);
  CHK_ENTITY_CMC (table, TABLE, header_horiz_top_color);
  CHK_ENTITY_CMC (table, TABLE, header_horiz_ins_color);
  CHK_ENTITY_CMC (table, TABLE, header_horiz_bottom_color);
  CHK_ENTITY_CMC (table, TABLE, header_vert_left_color);
  CHK_ENTITY_CMC (table, TABLE, header_vert_ins_color);
  CHK_ENTITY_CMC (table, TABLE, header_vert_right_color);
  CHK_ENTITY_CMC (table, TABLE, data_horiz_top_color);
  CHK_ENTITY_CMC (table, TABLE, data_horiz_ins_color);
  CHK_ENTITY_CMC (table, TABLE, data_horiz_bottom_color);
  CHK_ENTITY_CMC (table, TABLE, data_vert_left_color);
  CHK_ENTITY_CMC (table, TABLE, data_vert_ins_color);
  CHK_ENTITY_CMC (table, TABLE, data_vert_right_color);

  CHK_ENTITY_TYPE (table, TABLE, has_border_lineweight_overrides, B);
  CHK_ENTITY_TYPE (table, TABLE, border_lineweight_overrides_flag, BL);
  CHK_ENTITY_TYPE (table, TABLE, title_horiz_top_linewt, BS);
  CHK_ENTITY_TYPE (table, TABLE, title_horiz_ins_linewt, BS);
  CHK_ENTITY_TYPE (table, TABLE, title_horiz_bottom_linewt, BS);
  CHK_ENTITY_TYPE (table, TABLE, title_vert_left_linewt, BS);
  CHK_ENTITY_TYPE (table, TABLE, title_vert_ins_linewt, BS);
  CHK_ENTITY_TYPE (table, TABLE, title_vert_right_linewt, BS);
  CHK_ENTITY_TYPE (table, TABLE, header_horiz_top_linewt, BS);
  CHK_ENTITY_TYPE (table, TABLE, header_horiz_ins_linewt, BS);
  CHK_ENTITY_TYPE (table, TABLE, header_horiz_bottom_linewt, BS);
  CHK_ENTITY_TYPE (table, TABLE, header_vert_left_linewt, BS);
  CHK_ENTITY_TYPE (table, TABLE, header_vert_ins_linewt, BS);
  CHK_ENTITY_TYPE (table, TABLE, header_vert_right_linewt, BS);
  CHK_ENTITY_TYPE (table, TABLE, data_horiz_top_linewt, BS);
  CHK_ENTITY_TYPE (table, TABLE, data_horiz_ins_linewt, BS);
  CHK_ENTITY_TYPE (table, TABLE, data_horiz_bottom_linewt, BS);
  CHK_ENTITY_TYPE (table, TABLE, data_vert_left_linewt, BS);
  CHK_ENTITY_TYPE (table, TABLE, data_vert_ins_linewt, BS);
  CHK_ENTITY_TYPE (table, TABLE, data_vert_right_linewt, BS);
  CHK_ENTITY_TYPE (table, TABLE, has_border_visibility_overrides, B);
  CHK_ENTITY_TYPE (table, TABLE, border_visibility_overrides_flag, BL);
  CHK_ENTITY_TYPE (table, TABLE, title_horiz_top_visibility, BS);
  CHK_ENTITY_TYPE (table, TABLE, title_horiz_ins_visibility, BS);
  CHK_ENTITY_TYPE (table, TABLE, title_horiz_bottom_visibility, BS);
  CHK_ENTITY_TYPE (table, TABLE, title_vert_left_visibility, BS);
  CHK_ENTITY_TYPE (table, TABLE, title_vert_ins_visibility, BS);
  CHK_ENTITY_TYPE (table, TABLE, title_vert_right_visibility, BS);
  CHK_ENTITY_TYPE (table, TABLE, header_horiz_top_visibility, BS);
  CHK_ENTITY_TYPE (table, TABLE, header_horiz_ins_visibility, BS);
  CHK_ENTITY_TYPE (table, TABLE, header_horiz_bottom_visibility, BS);
  CHK_ENTITY_TYPE (table, TABLE, header_vert_left_visibility, BS);
  CHK_ENTITY_TYPE (table, TABLE, header_vert_ins_visibility, BS);
  CHK_ENTITY_TYPE (table, TABLE, header_vert_right_visibility, BS);
  CHK_ENTITY_TYPE (table, TABLE, data_horiz_top_visibility, BS);
  CHK_ENTITY_TYPE (table, TABLE, data_horiz_ins_visibility, BS);
  CHK_ENTITY_TYPE (table, TABLE, data_horiz_bottom_visibility, BS);
  CHK_ENTITY_TYPE (table, TABLE, data_vert_left_visibility, BS);
  CHK_ENTITY_TYPE (table, TABLE, data_vert_ins_visibility, BS);
  CHK_ENTITY_TYPE (table, TABLE, data_vert_right_visibility, BS);
  CHK_ENTITY_H (table, TABLE, block_header);
  CHK_ENTITY_H (table, TABLE, first_attrib);
  CHK_ENTITY_H (table, TABLE, last_attrib);
  // CHK_ENTITY_TYPE (table, TABLE, attribs, H*, attribs);
  CHK_ENTITY_H (table, TABLE, seqend);
  CHK_ENTITY_H (table, TABLE, title_row_style_override);
  CHK_ENTITY_H (table, TABLE, header_row_style_override);
  CHK_ENTITY_H (table, TABLE, data_row_style_override);
  CHK_ENTITY_TYPE (table, TABLE, unknown_bs, BS);
  CHK_ENTITY_3RD (table, TABLE, hor_dir);
  CHK_ENTITY_TYPE (table, TABLE, has_break_data, BL);
  CHK_ENTITY_TYPE (table, TABLE, break_flag, BL);
  CHK_ENTITY_TYPE (table, TABLE, break_flow_direction, BL);
  CHK_ENTITY_TYPE (table, TABLE, break_spacing, BD);
  CHK_ENTITY_TYPE (table, TABLE, break_unknown1, BL);
  CHK_ENTITY_TYPE (table, TABLE, break_unknown2, BL);
  CHK_ENTITY_TYPE (table, TABLE, num_break_heights, BL);
  // Dwg_TABLE_BreakHeight *break_heights;
  CHK_ENTITY_TYPE (table, TABLE, num_break_rows, BL);
  // Dwg_TABLE_BreakRow *break_rows;

#if 0
  if (!dwg_dynapi_entity_value (table, "TABLE", "rowstyles", &rowstyles, NULL))
    fail ("TABLE.rowstyles");
  for (i = 0; i < num_rowstyles; i++)
    {
      CHK_SUBCLASS_H (rowstyles[i], TABLE_rowstyles, text_style);
      CHK_SUBCLASS_TYPE (rowstyles[i], TABLE_rowstyles, text_height, BD);
      CHK_SUBCLASS_TYPE (rowstyles[i], TABLE_rowstyles, text_alignment, BS);
      CHK_SUBCLASS_CMC (rowstyles[i], TABLE_rowstyles, text_color);
      CHK_SUBCLASS_CMC (rowstyles[i], TABLE_rowstyles, fill_color);
      CHK_SUBCLASS_TYPE (rowstyles[i], TABLE_rowstyles, has_bgcolor, B);
      if (rowstyles[i].num_borders != 6)
        fail ("TABLE.rowstyles[%d].num_borders %d != 6", i, rowstyles[i].num_borders);
      for (int j = 0; j < 6; j++)
        {
          CHK_SUBCLASS_TYPE (rowstyles[i].borders[j], TABLE_border, linewt, BSd);
          CHK_SUBCLASS_TYPE (rowstyles[i].borders[j], TABLE_border, visible, B);
          CHK_SUBCLASS_CMC (rowstyles[i].borders[j], TABLE_border, color);
        }
      if (dwg_version >= R_2007)
        {
          CHK_SUBCLASS_TYPE (rowstyles[i], TABLE_rowstyles, data_type, BL);
          CHK_SUBCLASS_TYPE (rowstyles[i], TABLE_rowstyles, unit_type, BL);
          CHK_SUBCLASS_UTF8TEXT (rowstyles[i], TABLE_rowstyles, format_string);
        }
    }
#endif
}
