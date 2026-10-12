/* -*- c -*- */
/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2026 Free Software Foundation, Inc.                        */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * out_dxf_table.inc: ACAD_TABLE for DXF and DXFB, included by out_dxf.c
 * and out_dxfb.c with their VALUE_* macros, DXF_FN (name) for their
 * dxf_name or dxfb_name helpers, and DXF_TABLE_FIXUP_HANDLE (obj).
 */

/* A table color (CMTC, the R2004 color also in R2000 tables) as ACI: the
   index of an ACI color (method 0xc3, the index in the low byte as in the
   AutoCAD R2000 samples), 0 ByBlock, 256 ByLayer, 257 none. A true color
   is written as its index if any, else 256 (ByLayer). */
static void
DXF_FN (table_color) (Bit_Chain *restrict dat, const Dwg_Color *restrict color,
                      const int dxf)
{
  int method = color->method ? (int)color->method : (int)(color->rgb >> 24);
  int index;
  switch (method)
    {
    case 0xc3:
      index = color->rgb & 0xff;
      break;
    case 0xc1:
      index = 0;
      break;
    case 0xc0:
      index = 256;
      break;
    case 0xc8:
      index = 257;
      break;
    default:
      index = (color->index > 0 && color->index < 256) ? color->index : 256;
      break;
    }
  VALUE_RS (index, dxf);
}

/* The text of a pre-R2010 table cell: the cell text before R2007, else the
   formatted value string (302), else the string value. All in the string
   encoding of the input (TU from R2007+ DWG). */
static BITCODE_T
DXF_FN (table_cell_text) (Bit_Chain *restrict dat, const Dwg_TABLE_Cell *cell)
{
  if (!bit_empty_T (dat, cell->text_value))
    return cell->text_value;
  if (!bit_empty_T (dat, cell->value.value_string))
    return cell->value.value_string;
  if ((cell->value.data_type == 4 || cell->value.data_type == 512)
      && !bit_empty_T (dat, cell->value.data_string))
    return cell->value.data_string;
  return NULL;
}

/* ACAD_TABLE as AutoCAD writes it to all DXF versions (example_2000.dxf to
   example_2018.dxf, 2000/TS1.dxf): the INSERT part, then the pre-R2010 table
   data (20.4.96.1) in the DXF order 342 343 11 90 91 92 93-96, the row
   heights (141) before the column widths (142), and per cell 171-176, the
   override flag (177, 91 since R2007), 178 and 145, then the content: the
   text (1) before R2007, the value (CELL_VALUE ... ACVALUE_END) since R2007.
   R2010+ adds 280 0 before 342. A R2010+ table is converted from the table
   content as for R2000 DWG (downconvert_TABLE). The override groups follow
   the DWG field order with the DXF codes of the spec (no AutoCAD sample has
   any). The TABLECONTENT object itself is not written. */
static int
DXF_FN (ACAD_TABLE) (Bit_Chain *restrict dat, const Dwg_Object *restrict obj)
{
  int error = 0;
  const Dwg_Data *dwg = obj->parent;
  Dwg_Entity_TABLE *_obj = obj->tio.entity->tio.TABLE;
  BITCODE_BL ci, ai;

#ifdef USE_WRITE
  if (!_obj->cells && _obj->tdata.num_rows && _obj->tdata.num_cols)
    downconvert_TABLE ((Dwg_Object *)obj);
#endif
  if (_obj->num_cells != (unsigned long)_obj->num_rows * _obj->num_cols
      || (_obj->num_cells && !_obj->cells)
      || (_obj->num_rows && !_obj->row_heights)
      || (_obj->num_cols && !_obj->col_widths))
    {
      LOG_ERROR ("Skip invalid ACAD_TABLE " FORMAT_H, ARGS_H (obj->handle));
      return DWG_ERR_INVALIDDWG;
    }

  RECORD (ACAD_TABLE);
  LOG_INFO ("Entity TABLE\n");
  DXF_TABLE_FIXUP_HANDLE (obj);
  VALUE_H (obj->handle.value, 5);
  error |= DXF_FN (common_entity_handle_data) (dat, obj);

  SUBCLASS (AcDbBlockReference)
  FIELD_HANDLE (block_header, 5, 2);
  FIELD_3BD (ins_pt, 10);
  if (_obj->scale.x != 1.0 || _obj->scale.y != 1.0 || _obj->scale.z != 1.0)
    FIELD_3BD_1 (scale, 41);
  if (_obj->rotation != 0.0)
    VALUE_BD (_obj->rotation, 50);
  FIELD_BE (extrusion, 210);

  SUBCLASS (AcDbTable)
  SINCE (R_2010b)
  {
    VALUE_BS (0, 280);
  }
  FIELD_HANDLE (tablestyle, 5, 342);
  FIELD_HANDLE (block_header, 5, 343);
  VALUE_3BD (_obj->horiz_direction, 11);
  VALUE_BL (_obj->flag_for_table_value, 90);
  VALUE_BL (_obj->num_rows, 91);
  VALUE_BL (_obj->num_cols, 92);
  {
    const BITCODE_BL f93
        = _obj->has_table_overrides ? _obj->table_flag_override : 0;
    const BITCODE_BL f94 = _obj->has_border_color_overrides
                               ? _obj->border_color_overrides_flag
                               : 0;
    const BITCODE_BL f95 = _obj->has_border_lineweight_overrides
                               ? _obj->border_lineweight_overrides_flag
                               : 0;
    const BITCODE_BL f96 = _obj->has_border_visibility_overrides
                               ? _obj->border_visibility_overrides_flag
                               : 0;
    VALUE_BL (f93, 93);
    VALUE_BL (f94, 94);
    VALUE_BL (f95, 95);
    VALUE_BL (f96, 96);
  }
  for (ci = 0; ci < _obj->num_rows; ci++)
    VALUE_RD (_obj->row_heights[ci], 141);
  for (ci = 0; ci < _obj->num_cols; ci++)
    VALUE_RD (_obj->col_widths[ci], 142);

  for (ci = 0; ci < _obj->num_cells; ci++)
    {
      Dwg_TABLE_Cell *cell = &_obj->cells[ci];
      BITCODE_BL flag
          = cell->additional_data_flag ? cell->cell_flag_override : 0;
      // 177 is a 16-bit group: without 0x40000 (no groups; AutoCAD writes
      // 0 to 177 where the DWG cell has 0x40000: example_2000), and the
      // visibility bits 0x8000-0x20000 are given by their groups 285, 286
      // and 288 only (in_dxf sets them again; ACadSharp cannot read
      // sample_AC1015 tables without them).
      PRE (R_2007a)
      {
        flag &= 0x3FFFF;
      }
      VALUE_BS (cell->type, 171);
      VALUE_BS (cell->flags, 172);
      VALUE_B (cell->is_merged_value, 173);
      VALUE_B (cell->is_autofit_flag, 174);
      VALUE_BL (cell->merged_width_flag, 175);
      VALUE_BL (cell->merged_height_flag, 176);
      PRE (R_2007a)
      {
        VALUE_BS ((flag & 0x7FFF), 177);
      }
      LATER_VERSIONS
      {
        VALUE_BL (flag, 91);
      }
      VALUE_BS (cell->virtual_edge_flag, 178);
      VALUE_RD (cell->rotation, 145);
      if (cell->type == 2)
        {
          if (cell->block_handle && cell->block_handle->absolute_ref)
            VALUE_HANDLE (cell->block_handle, block_handle, 3, 340);
          VALUE_RD (cell->block_scale, 144);
          if (cell->has_attr_defs && cell->num_attr_defs && cell->attr_defs)
            {
              VALUE_BS (cell->num_attr_defs, 179);
              for (ai = 0; ai < cell->num_attr_defs; ai++)
                {
                  Dwg_TABLE_AttrDef *attr = &cell->attr_defs[ai];
                  VALUE_HANDLE (attr->attdef, attdef, 4, 331);
                  VALUE_T (attr->text, 300);
                }
            }
        }
      else
        {
          int has_field = cell->text_style && cell->text_style->absolute_ref;
          if (has_field)
            VALUE_HANDLE (cell->text_style, text_style, 3, 344);
          PRE (R_2007a)
          {
            BITCODE_T text = DXF_FN (table_cell_text) (dat, cell);
            if (!has_field && cell->type == 1)
              {
                if (text)
                  VALUE_T (text, 1)
                else
                  VALUE_TFF ("", 1)
              }
          }
        }
      SINCE (R_2007a)
      {
        // the value (20.4.99). Without one (from pre-R2007 or converted
        // R2010+ tables) a string value from the cell text.
        const Dwg_TABLE_value *v = &cell->value;
        VALUE_BL (cell->unknown, 92);
        VALUE_TFF ("CELL_VALUE", 301);
        if (v->format_flags || v->data_type || v->value_string)
          {
            VALUE_BL (v->format_flags, 93);
            VALUE_BL (v->data_type, 90);
            if (!(v->format_flags & 1))
              switch (v->data_type)
                {
                case 0:
                case 1:
                  VALUE_BL (v->data_long, 91);
                  break;
                case 2:
                  VALUE_RD (v->data_double, 140);
                  break;
                case 4:
                case 512:
                  if (v->data_string)
                    VALUE_T (v->data_string, 1)
                  else
                    VALUE_TFF ("", 1)
                  break;
                case 16:
                  VALUE_RD (v->data_point.x, 11);
                  VALUE_RD (v->data_point.y, 21);
                  break;
                case 32:
                  VALUE_3BD (v->data_3dpoint, 11);
                  break;
                case 64:
                  VALUE_HANDLE (v->data_handle, data_handle, 5, 330);
                  break;
                default:
                  break;
                }
            VALUE_BL (v->unit_type, 94);
            if (v->format_string)
              VALUE_T (v->format_string, 300)
            else
              VALUE_TFF ("", 300)
            if (v->value_string)
              VALUE_T (v->value_string, 302)
            else
              VALUE_TFF ("", 302)
          }
        else
          {
            BITCODE_T text = cell->type == 1
                                 ? DXF_FN (table_cell_text) (dat, cell)
                                 : NULL;
            VALUE_BL ((BITCODE_BL)(text ? 6 : 7), 93);
            VALUE_BL ((BITCODE_BL)(text ? 4 : 0), 90);
            if (text)
              VALUE_T (text, 1);
            VALUE_BL (0, 94);
            VALUE_TFF ("", 300);
            if (text)
              VALUE_T (text, 302)
            else
              VALUE_TFF ("", 302)
          }
        VALUE_TFF ("ACVALUE_END", 304);
      }
      // the cell overrides
      if (flag & 0x01)
        VALUE_BS (cell->cell_alignment, 170);
      if (flag & 0x02)
        VALUE_B (cell->bg_fill_none, 283);
      if (flag & 0x04)
        DXF_FN (table_color) (dat, &cell->bg_color, 63);
      if (flag & 0x08)
        DXF_FN (table_color) (dat, &cell->content_color, 64);
      if (flag & 0x10)
        {
          Dwg_Object_Ref *ref = cell->text_style_override;
          Dwg_Object *o = ref ? dwg_ref_object ((Dwg_Data *)dwg, ref) : NULL;
          if (o && o->fixedtype == DWG_TYPE_STYLE)
            DXF_FN (cvt_tablerecord)
            (dat, o, o->tio.object->tio.STYLE->name, 7);
          else
            VALUE_TFF ("", 7);
        }
      if (flag & 0x20)
        VALUE_RD (cell->text_height, 140);
      if (flag & 0x00040)
        DXF_FN (table_color) (dat, &cell->top_grid_color, 69);
      if (flag & 0x00400)
        VALUE_BS (cell->top_grid_linewt, 279);
      if (flag & 0x04000)
        VALUE_BS (cell->top_visibility, 289);
      if (flag & 0x00080)
        DXF_FN (table_color) (dat, &cell->right_grid_color, 65);
      if (flag & 0x00800)
        VALUE_BS (cell->right_grid_linewt, 275);
      if (flag & 0x08000)
        VALUE_BS (cell->right_visibility, 285);
      if (flag & 0x00100)
        DXF_FN (table_color) (dat, &cell->bottom_grid_color, 66);
      if (flag & 0x01000)
        VALUE_BS (cell->bottom_grid_linewt, 276);
      if (flag & 0x10000)
        VALUE_BS (cell->bottom_visibility, 286);
      if (flag & 0x00200)
        DXF_FN (table_color) (dat, &cell->left_grid_color, 68);
      if (flag & 0x02000)
        VALUE_BS (cell->left_grid_linewt, 278);
      if (flag & 0x20000)
        VALUE_BS (cell->left_visibility, 288);
    }

  // the table overrides, in the DWG order
  if (_obj->has_table_overrides)
    {
      const BITCODE_BL f = _obj->table_flag_override;
      if (f & 0x0001)
        VALUE_B (_obj->title_suppressed, 280);
      VALUE_B (_obj->header_suppressed, 281);
      if (f & 0x0004)
        VALUE_BS (_obj->flow_direction, 70);
      if (f & 0x0008)
        VALUE_RD (_obj->horiz_cell_margin, 40);
      if (f & 0x0010)
        VALUE_RD (_obj->vert_cell_margin, 41);
      if (f & 0x0020)
        DXF_FN (table_color) (dat, &_obj->title_row_color, 64);
      if (f & 0x0040)
        DXF_FN (table_color) (dat, &_obj->header_row_color, 64);
      if (f & 0x0080)
        DXF_FN (table_color) (dat, &_obj->data_row_color, 64);
      if (f & 0x0100)
        VALUE_B (_obj->title_row_fill_none, 283);
      if (f & 0x0200)
        VALUE_B (_obj->header_row_fill_none, 283);
      if (f & 0x0400)
        VALUE_B (_obj->data_row_fill_none, 283);
      if (f & 0x0800)
        DXF_FN (table_color) (dat, &_obj->title_row_fill_color, 63);
      if (f & 0x1000)
        DXF_FN (table_color) (dat, &_obj->header_row_fill_color, 63);
      if (f & 0x2000)
        DXF_FN (table_color) (dat, &_obj->data_row_fill_color, 63);
      if (f & 0x4000)
        VALUE_BS (_obj->title_row_alignment, 170);
      if (f & 0x8000)
        VALUE_BS (_obj->header_row_alignment, 170);
      if (f & 0x10000)
        VALUE_BS (_obj->data_row_alignment, 170);
      if (f & 0x20000)
        FIELD_HANDLE (title_text_style, 5, 7);
      if (f & 0x40000)
        FIELD_HANDLE (header_text_style, 5, 7);
      if (f & 0x80000)
        FIELD_HANDLE (data_text_style, 5, 7);
      if (f & 0x100000)
        VALUE_RD (_obj->title_row_height, 140);
      if (f & 0x200000)
        VALUE_RD (_obj->header_row_height, 140);
      if (f & 0x400000)
        VALUE_RD (_obj->data_row_height, 140);
    }
  if (_obj->has_border_color_overrides)
    {
      const BITCODE_BL f = _obj->border_color_overrides_flag;
      Dwg_Color *c[18]
          = { &_obj->title_horiz_top_color,     &_obj->title_horiz_ins_color,
              &_obj->title_horiz_bottom_color,  &_obj->title_vert_left_color,
              &_obj->title_vert_ins_color,      &_obj->title_vert_right_color,
              &_obj->header_horiz_top_color,    &_obj->header_horiz_ins_color,
              &_obj->header_horiz_bottom_color, &_obj->header_vert_left_color,
              &_obj->header_vert_ins_color,     &_obj->header_vert_right_color,
              &_obj->data_horiz_top_color,      &_obj->data_horiz_ins_color,
              &_obj->data_horiz_bottom_color,   &_obj->data_vert_left_color,
              &_obj->data_vert_ins_color,       &_obj->data_vert_right_color };
      static const int code[6] = { 64, 65, 66, 63, 68, 69 };
      for (int k = 0; k < 18; k++)
        if (f & (1U << k))
          DXF_FN (table_color) (dat, c[k], code[k % 6]);
    }
  if (_obj->has_border_lineweight_overrides)
    {
      const BITCODE_BL f = _obj->border_lineweight_overrides_flag;
      BITCODE_BS *w[18] = {
        &_obj->title_horiz_top_linewt,     &_obj->title_horiz_ins_linewt,
        &_obj->title_horiz_bottom_linewt,  &_obj->title_vert_left_linewt,
        &_obj->title_vert_ins_linewt,      &_obj->title_vert_right_linewt,
        &_obj->header_horiz_top_linewt,    &_obj->header_horiz_ins_linewt,
        &_obj->header_horiz_bottom_linewt, &_obj->header_vert_left_linewt,
        &_obj->header_vert_ins_linewt,     &_obj->header_vert_right_linewt,
        &_obj->data_horiz_top_linewt,      &_obj->data_horiz_ins_linewt,
        &_obj->data_horiz_bottom_linewt,   &_obj->data_vert_left_linewt,
        &_obj->data_vert_ins_linewt,       &_obj->data_vert_right_linewt
      };
      for (int k = 0; k < 18; k++)
        if (f & (1U << k))
          VALUE_BS (*w[k], 274 + k % 6);
    }
  if (_obj->has_border_visibility_overrides)
    {
      const BITCODE_BL f = _obj->border_visibility_overrides_flag;
      BITCODE_BS *v[18] = { &_obj->title_horiz_top_visibility,
                            &_obj->title_horiz_ins_visibility,
                            &_obj->title_horiz_bottom_visibility,
                            &_obj->title_vert_left_visibility,
                            &_obj->title_vert_ins_visibility,
                            &_obj->title_vert_right_visibility,
                            &_obj->header_horiz_top_visibility,
                            &_obj->header_horiz_ins_visibility,
                            &_obj->header_horiz_bottom_visibility,
                            &_obj->header_vert_left_visibility,
                            &_obj->header_vert_ins_visibility,
                            &_obj->header_vert_right_visibility,
                            &_obj->data_horiz_top_visibility,
                            &_obj->data_horiz_ins_visibility,
                            &_obj->data_horiz_bottom_visibility,
                            &_obj->data_vert_left_visibility,
                            &_obj->data_vert_ins_visibility,
                            &_obj->data_vert_right_visibility };
      for (int k = 0; k < 18; k++)
        if (f & (1U << k))
          VALUE_BS (*v[k], 284 + k % 6);
    }
  error |= DXF_FN (write_eed) (dat, obj->tio.object);
  return error;
}
