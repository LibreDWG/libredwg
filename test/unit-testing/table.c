// TODO debugging
#define DWG_TYPE DWG_TYPE_TABLE
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BL i;
  Dwg_LinkedData ldata;
  Dwg_LinkedTableData tdata;
  Dwg_FormattedTableData fdata;
  BITCODE_H table_style;

  BITCODE_RC unknown_rc;
  BITCODE_H unknown_h;
  BITCODE_BL unknown_bl;
  BITCODE_B unknown_b;
  BITCODE_BL unknown_bl1;
  BITCODE_3BD insertion_point;
  BITCODE_3BD scale;
  BITCODE_BB data_flags;
  BITCODE_BD rotation;
  BITCODE_BE extrusion;
  BITCODE_B has_attribs;
  BITCODE_BL num_owned;
  BITCODE_BS flag_for_table_value;
  BITCODE_3BD horiz_direction;
  BITCODE_BL num_cols;
  BITCODE_BL num_rows;
  unsigned long num_cells;
  BITCODE_BD* col_widths;
  BITCODE_BD* row_heights;
  Dwg_TABLE_Cell* cells;
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
  BITCODE_H* attrib_handles;
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
      printf ("TABLECONTENT r2010+"); // TODO
      return;
    }

  CHK_ENTITY_TYPE (table, TABLE, unknown_rc, RC, unknown_rc);
  CHK_ENTITY_H (table, TABLE, unknown_h, unknown_h);
  CHK_ENTITY_TYPE (table, TABLE, unknown_bl, BL, unknown_bl);
  CHK_ENTITY_TYPE (table, TABLE, unknown_b, B, unknown_b);
  CHK_ENTITY_TYPE (table, TABLE, unknown_bl1, BL, unknown_bl1);
  CHK_ENTITY_3RD (table, TABLE, insertion_point, insertion_point);
  CHK_ENTITY_3RD (table, TABLE, scale, scale);
  CHK_ENTITY_TYPE (table, TABLE, data_flags, BB, data_flags);
  CHK_ENTITY_TYPE (table, TABLE, rotation, BD, rotation);
  CHK_ENTITY_MAX (table, TABLE, rotation, BD, 6.284);
  CHK_ENTITY_3RD (table, TABLE, extrusion, extrusion);
  CHK_ENTITY_TYPE (table, TABLE, has_attribs, B, has_attribs);
  CHK_ENTITY_TYPE (table, TABLE, num_owned, BL, num_owned);
  CHK_ENTITY_TYPE (table, TABLE, flag_for_table_value, BS, flag_for_table_value);
  CHK_ENTITY_3RD (table, TABLE, horiz_direction, horiz_direction);
  CHK_ENTITY_TYPE (table, TABLE, num_cols, BL, num_cols);
  CHK_ENTITY_TYPE (table, TABLE, num_rows, BL, num_rows);
  //CHK_ENTITY_TYPE (table, TABLE, num_cells, BL, num_cells);
  //BITCODE_BD* col_widths;
  //BITCODE_BD* row_heights;
  //Dwg_TABLE_Cell* cells;
  CHK_ENTITY_TYPE (table, TABLE, has_table_overrides, B, has_table_overrides);
  CHK_ENTITY_TYPE (table, TABLE, table_flag_override, BL, table_flag_override);
  CHK_ENTITY_TYPE (table, TABLE, title_suppressed, B, title_suppressed);
  CHK_ENTITY_TYPE (table, TABLE, header_suppressed, B, header_suppressed);
  CHK_ENTITY_TYPE (table, TABLE, flow_direction, BS, flow_direction);
  CHK_ENTITY_TYPE (table, TABLE, horiz_cell_margin, BD, horiz_cell_margin);
  CHK_ENTITY_TYPE (table, TABLE, vert_cell_margin, BD, vert_cell_margin);
  CHK_ENTITY_CMC (table, TABLE, title_row_color, title_row_color);
  CHK_ENTITY_CMC (table, TABLE, header_row_color, header_row_color);
  CHK_ENTITY_CMC (table, TABLE, data_row_color, data_row_color);
  CHK_ENTITY_TYPE (table, TABLE, title_row_fill_none, B, title_row_fill_none);
  CHK_ENTITY_TYPE (table, TABLE, header_row_fill_none, B, header_row_fill_none);
  CHK_ENTITY_TYPE (table, TABLE, data_row_fill_none, B, data_row_fill_none);
  CHK_ENTITY_CMC (table, TABLE, title_row_fill_color, title_row_fill_color);
  CHK_ENTITY_CMC (table, TABLE, header_row_fill_color, header_row_fill_color);
  CHK_ENTITY_CMC (table, TABLE, data_row_fill_color, data_row_fill_color);
  CHK_ENTITY_TYPE (table, TABLE, title_row_alignment, BS, title_row_alignment);
  CHK_ENTITY_TYPE (table, TABLE, header_row_alignment, BS, header_row_alignment);
  CHK_ENTITY_TYPE (table, TABLE, data_row_alignment, BS, data_row_alignment);
  CHK_ENTITY_H (table, TABLE, title_text_style, title_text_style);
  CHK_ENTITY_H (table, TABLE, header_text_style, header_text_style);
  CHK_ENTITY_H (table, TABLE, data_text_style, data_text_style);
  CHK_ENTITY_TYPE (table, TABLE, title_row_height, BD, title_row_height);
  CHK_ENTITY_TYPE (table, TABLE, header_row_height, BD, header_row_height);
  CHK_ENTITY_TYPE (table, TABLE, data_row_height, BD, data_row_height);
  CHK_ENTITY_TYPE (table, TABLE, has_border_color_overrides, B, has_border_color_overrides);
  CHK_ENTITY_TYPE (table, TABLE, border_color_overrides_flag, BL, border_color_overrides_flag);
  CHK_ENTITY_CMC (table, TABLE, title_horiz_top_color, title_horiz_top_color);
  CHK_ENTITY_CMC (table, TABLE, title_horiz_ins_color, title_horiz_ins_color);
  CHK_ENTITY_CMC (table, TABLE, title_horiz_bottom_color, title_horiz_bottom_color);
  CHK_ENTITY_CMC (table, TABLE, title_vert_left_color, title_vert_left_color);
  CHK_ENTITY_CMC (table, TABLE, title_vert_ins_color, title_vert_ins_color);
  CHK_ENTITY_CMC (table, TABLE, title_vert_right_color, title_vert_right_color);
  CHK_ENTITY_CMC (table, TABLE, header_horiz_top_color, header_horiz_top_color);
  CHK_ENTITY_CMC (table, TABLE, header_horiz_ins_color, header_horiz_ins_color);
  CHK_ENTITY_CMC (table, TABLE, header_horiz_bottom_color, header_horiz_bottom_color);
  CHK_ENTITY_CMC (table, TABLE, header_vert_left_color, header_vert_left_color);
  CHK_ENTITY_CMC (table, TABLE, header_vert_ins_color, header_vert_ins_color);
  CHK_ENTITY_CMC (table, TABLE, header_vert_right_color, header_vert_right_color);
  CHK_ENTITY_CMC (table, TABLE, data_horiz_top_color, data_horiz_top_color);
  CHK_ENTITY_CMC (table, TABLE, data_horiz_ins_color, data_horiz_ins_color);
  CHK_ENTITY_CMC (table, TABLE, data_horiz_bottom_color, data_horiz_bottom_color);
  CHK_ENTITY_CMC (table, TABLE, data_vert_left_color, data_vert_left_color);
  CHK_ENTITY_CMC (table, TABLE, data_vert_ins_color, data_vert_ins_color);
  CHK_ENTITY_CMC (table, TABLE, data_vert_right_color, data_vert_right_color);

  CHK_ENTITY_TYPE (table, TABLE, has_border_lineweight_overrides, B, has_border_lineweight_overrides);
  CHK_ENTITY_TYPE (table, TABLE, border_lineweight_overrides_flag, BL, border_lineweight_overrides_flag);
  CHK_ENTITY_TYPE (table, TABLE, title_horiz_top_linewt, BS, title_horiz_top_linewt);
  CHK_ENTITY_TYPE (table, TABLE, title_horiz_ins_linewt, BS, title_horiz_ins_linewt);
  CHK_ENTITY_TYPE (table, TABLE, title_horiz_bottom_linewt, BS, title_horiz_bottom_linewt);
  CHK_ENTITY_TYPE (table, TABLE, title_vert_left_linewt, BS, title_vert_left_linewt);
  CHK_ENTITY_TYPE (table, TABLE, title_vert_ins_linewt, BS, title_vert_ins_linewt);
  CHK_ENTITY_TYPE (table, TABLE, title_vert_right_linewt, BS, title_vert_right_linewt);
  CHK_ENTITY_TYPE (table, TABLE, header_horiz_top_linewt, BS, header_horiz_top_linewt);
  CHK_ENTITY_TYPE (table, TABLE, header_horiz_ins_linewt, BS, header_horiz_ins_linewt);
  CHK_ENTITY_TYPE (table, TABLE, header_horiz_bottom_linewt, BS, header_horiz_bottom_linewt);
  CHK_ENTITY_TYPE (table, TABLE, header_vert_left_linewt, BS, header_vert_left_linewt);
  CHK_ENTITY_TYPE (table, TABLE, header_vert_ins_linewt, BS, header_vert_ins_linewt);
  CHK_ENTITY_TYPE (table, TABLE, header_vert_right_linewt, BS, header_vert_right_linewt);
  CHK_ENTITY_TYPE (table, TABLE, data_horiz_top_linewt, BS, data_horiz_top_linewt);
  CHK_ENTITY_TYPE (table, TABLE, data_horiz_ins_linewt, BS, data_horiz_ins_linewt);
  CHK_ENTITY_TYPE (table, TABLE, data_horiz_bottom_linewt, BS, data_horiz_bottom_linewt);
  CHK_ENTITY_TYPE (table, TABLE, data_vert_left_linewt, BS, data_vert_left_linewt);
  CHK_ENTITY_TYPE (table, TABLE, data_vert_ins_linewt, BS, data_vert_ins_linewt);
  CHK_ENTITY_TYPE (table, TABLE, data_vert_right_linewt, BS, data_vert_right_linewt);
  CHK_ENTITY_TYPE (table, TABLE, has_border_visibility_overrides, B, has_border_visibility_overrides);
  CHK_ENTITY_TYPE (table, TABLE, border_visibility_overrides_flag, BL, border_visibility_overrides_flag);
  CHK_ENTITY_TYPE (table, TABLE, title_horiz_top_visibility, BS, title_horiz_top_visibility);
  CHK_ENTITY_TYPE (table, TABLE, title_horiz_ins_visibility, BS, title_horiz_ins_visibility);
  CHK_ENTITY_TYPE (table, TABLE, title_horiz_bottom_visibility, BS, title_horiz_bottom_visibility);
  CHK_ENTITY_TYPE (table, TABLE, title_vert_left_visibility, BS, title_vert_left_visibility);
  CHK_ENTITY_TYPE (table, TABLE, title_vert_ins_visibility, BS, title_vert_ins_visibility);
  CHK_ENTITY_TYPE (table, TABLE, title_vert_right_visibility, BS, title_vert_right_visibility);
  CHK_ENTITY_TYPE (table, TABLE, header_horiz_top_visibility, BS, header_horiz_top_visibility);
  CHK_ENTITY_TYPE (table, TABLE, header_horiz_ins_visibility, BS, header_horiz_ins_visibility);
  CHK_ENTITY_TYPE (table, TABLE, header_horiz_bottom_visibility, BS, header_horiz_bottom_visibility);
  CHK_ENTITY_TYPE (table, TABLE, header_vert_left_visibility, BS, header_vert_left_visibility);
  CHK_ENTITY_TYPE (table, TABLE, header_vert_ins_visibility, BS, header_vert_ins_visibility);
  CHK_ENTITY_TYPE (table, TABLE, header_vert_right_visibility, BS, header_vert_right_visibility);
  CHK_ENTITY_TYPE (table, TABLE, data_horiz_top_visibility, BS, data_horiz_top_visibility);
  CHK_ENTITY_TYPE (table, TABLE, data_horiz_ins_visibility, BS, data_horiz_ins_visibility);
  CHK_ENTITY_TYPE (table, TABLE, data_horiz_bottom_visibility, BS, data_horiz_bottom_visibility);
  CHK_ENTITY_TYPE (table, TABLE, data_vert_left_visibility, BS, data_vert_left_visibility);
  CHK_ENTITY_TYPE (table, TABLE, data_vert_ins_visibility, BS, data_vert_ins_visibility);
  CHK_ENTITY_TYPE (table, TABLE, data_vert_right_visibility, BS, data_vert_right_visibility);
  CHK_ENTITY_H (table, TABLE, block_header, block_header);
  CHK_ENTITY_H (table, TABLE, first_attrib, first_attrib);
  CHK_ENTITY_H (table, TABLE, last_attrib, last_attrib);
  //CHK_ENTITY_TYPE (table, TABLE, attrib_handles, H*, attrib_handles);
  CHK_ENTITY_H (table, TABLE, seqend, seqend);
  CHK_ENTITY_H (table, TABLE, title_row_style_override, title_row_style_override);
  CHK_ENTITY_H (table, TABLE, header_row_style_override, header_row_style_override);
  CHK_ENTITY_H (table, TABLE, data_row_style_override, data_row_style_override);
  CHK_ENTITY_TYPE (table, TABLE, unknown_bs, BS, unknown_bs);
  CHK_ENTITY_3RD (table, TABLE, hor_dir, hor_dir);
  CHK_ENTITY_TYPE (table, TABLE, has_break_data, BL, has_break_data);
  CHK_ENTITY_TYPE (table, TABLE, break_flag, BL, break_flag);
  CHK_ENTITY_TYPE (table, TABLE, break_flow_direction, BL, break_flow_direction);
  CHK_ENTITY_TYPE (table, TABLE, break_spacing, BD, break_spacing);
  CHK_ENTITY_TYPE (table, TABLE, break_unknown1, BL, break_unknown1);
  CHK_ENTITY_TYPE (table, TABLE, break_unknown2, BL, break_unknown2);
  CHK_ENTITY_TYPE (table, TABLE, num_break_heights, BL, num_break_heights);
  //Dwg_TABLE_BreakHeight *break_heights;
  CHK_ENTITY_TYPE (table, TABLE, num_break_rows, BL, num_break_rows);
  //Dwg_TABLE_BreakRow *break_rows;

#if 0  
  CHK_ENTITY_TYPE (table, TABLE, class_version, BL, class_version);
  CHK_ENTITY_UTF8TEXT (table, TABLE, name, name);
  CHK_ENTITY_TYPE (table, TABLE, flow_direction, BS, flow_direction);
  CHK_ENTITY_TYPE (table, TABLE, horiz_cell_margin, BD, horiz_cell_margin);
  CHK_ENTITY_TYPE (table, TABLE, vert_cell_margin, BD, vert_cell_margin);
  CHK_ENTITY_TYPE (table, TABLE, title_suppressed, B, title_suppressed);
  CHK_ENTITY_TYPE (table, TABLE, header_suppressed, B, header_suppressed);
  CHK_ENTITY_TYPE (table, TABLE, num_rowstyles, BL, num_rowstyles);
  if (num_rowstyles != 3)
    fail ("TABLE.num_rowstyles %d != 3", num_rowstyles);
  CHK_ENTITY_TYPE (table, TABLE, num_cells, BL, num_cells);
  if (!dwg_dynapi_entity_value (table, "TABLE", "rowstyles", &rowstyles, NULL))
    fail ("TABLE.rowstyles");
  if (!dwg_dynapi_entity_value (table, "TABLE", "cells", &cells, NULL))
    fail ("TABLE.cells");
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
