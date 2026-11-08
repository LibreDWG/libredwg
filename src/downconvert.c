/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2009-2026 Free Software Foundation, Inc.                   */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * downconvert.c: TABLE/TABLESTYLE field conversion helpers shared by
 * the DWG encoder (encode.c) and the DXF/DXFB writers (out_dxf.c,
 * out_dxfb.c), which need them even in --disable-write builds.
 */

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "common.h"
#include "bits.h"
#include "dwg.h"
#include "encode.h"
#include "free.h"

#define DWG_LOGLEVEL loglevel
#include "logging.h"

// Duplicate a string for the TABLE cell text in the string flavour of the
// source: TU (UCS-2) for r2007+ DWGs, else TV.
static BITCODE_T
table_dup_T (const Dwg_Data *restrict dwg, const BITCODE_T s)
{
  if (!s)
    return NULL;
  if (IS_FROM_TU_DWG (dwg))
    return (BITCODE_T)bit_wcs2dup ((BITCODE_TU)s);
  return (BITCODE_T)strdup ((const char *)s);
}

// A string as TU (r2007+ DWGs) or TV from ASCII text.
static BITCODE_T
table_ascii_to_T (const Dwg_Data *restrict dwg, const char *restrict ascii)
{
  size_t len = strlen (ascii);
  if (IS_FROM_TU_DWG (dwg))
    {
      BITCODE_RS *tu = (BITCODE_RS *)calloc (len + 1, sizeof (BITCODE_RS));
      if (!tu)
        return NULL;
      for (size_t i = 0; i < len; i++)
        tu[i] = (unsigned char)ascii[i];
      return (BITCODE_T)tu;
    }
  return (BITCODE_T)strdup (ascii);
}

// The displayed text of a table cell value (20.4.99). Strings are stored as
// UTF-16LE bytes in data_date (data_size includes the terminating NUL),
// numbers use the formatted value_string, or are printed plain.
static BITCODE_T
table_value_text (const Dwg_Data *restrict dwg,
                  const Dwg_TABLE_value *restrict value)
{
  if ((value->data_type == 4 || value->data_type == 512) && value->data_date
      && value->data_size >= 2)
    {
      const unsigned char *b = (const unsigned char *)value->data_date;
      size_t n = value->data_size / 2;
      BITCODE_RS *tu = (BITCODE_RS *)calloc (n + 1, sizeof (BITCODE_RS));
      BITCODE_T s;
      if (!tu)
        return NULL;
      for (size_t i = 0; i < n; i++)
        tu[i] = (BITCODE_RS)(b[2 * i] | (b[2 * i + 1] << 8));
      if (IS_FROM_TU_DWG (dwg))
        return (BITCODE_T)tu;
      s = (BITCODE_T)bit_convert_TU ((BITCODE_TU)tu);
      free (tu);
      return s;
    }
  if (value->value_string && *value->value_string)
    return table_dup_T (dwg, value->value_string);
  if (value->data_type == 1 || value->data_type == 2)
    {
      char buf[48];
      if (value->data_type == 1)
        snprintf (buf, sizeof (buf), "%u", (unsigned)value->data_long);
      else
        snprintf (buf, sizeof (buf), "%g", value->data_double);
      return table_ascii_to_T (dwg, buf);
    }
  return NULL;
}

// R2007+ also needs the text as value, as AutoCAD writes it: format flags 6,
// type string with UTF-16LE data, and the same string as value string.
static void
table_cell_string_value (const Dwg_Data *restrict dwg,
                         Dwg_TABLE_Cell *restrict cell)
{
  BITCODE_TU tmp = NULL;
  const BITCODE_RS *tu;
  unsigned char *b;
  size_t n = 0;
  if (!cell->text_value)
    return;
  if (IS_FROM_TU_DWG (dwg))
    tu = (const BITCODE_RS *)cell->text_value;
  else
    {
      tmp = bit_utf8_to_TU ((char *)cell->text_value, 0);
      tu = (const BITCODE_RS *)tmp;
    }
  while (tu && tu[n])
    n++;
  if (!n)
    {
      free (tmp);
      return;
    }
  b = (unsigned char *)calloc (n * 2 + 2, 1);
  if (b)
    {
      for (size_t i = 0; i < n; i++)
        {
          b[2 * i] = tu[i] & 0xff;
          b[2 * i + 1] = tu[i] >> 8;
        }
      cell->value.data_date = (BITCODE_TF)b;
      cell->value.data_size = (BITCODE_BL)(n * 2 + 2);
      cell->value.format_flags = 6;
      cell->value.data_type = 4;
      cell->value.value_string = table_dup_T (dwg, cell->text_value);
    }
  free (tmp);
}

// from >2010 to 2000: build the pre-R2010 ACAD_TABLE fields from the
// R2010+ TABLECONTENT (LinkedTableData).  Column widths, row heights, the
// cell text (or block, with its attribute texts) of the first content of
// each cell, and merged cells are kept. Cell styles, formats, borders and
// additional contents are lost.
void
downconvert_TABLE (Dwg_Object *restrict obj)
{
  Dwg_Data *dwg;
  Dwg_Entity_TABLE *_obj;
  Dwg_LinkedTableData *tdata;
  Dwg_FormattedTableData *fdata;
  unsigned long idx;
  if (!obj || obj->fixedtype != DWG_TYPE_TABLE || !obj->tio.entity
      || !obj->tio.entity->tio.TABLE)
    {
      LOG_ERROR ("Invalid type %u for downconvert_TABLE",
                 obj ? obj->fixedtype : 0);
      return;
    }
  dwg = obj->parent;
  _obj = obj->tio.entity->tio.TABLE;
  tdata = &_obj->tdata;
  fdata = &_obj->fdata;
  // already pre-R2010 content, e.g. read from DXF
  if (_obj->cells || _obj->col_widths || _obj->row_heights)
    return;
  if (!tdata->num_cols || !tdata->num_rows || !tdata->cols || !tdata->rows
      || tdata->num_cols > 5000 || tdata->num_rows > 5000)
    {
      LOG_WARN ("downconvert_TABLE: no table content to downconvert");
      return;
    }
  LOG_WARN ("Downconverting TABLE with losing information");
  _obj->num_cols = tdata->num_cols;
  _obj->num_rows = tdata->num_rows;
  _obj->num_cells = (unsigned long)tdata->num_cols * tdata->num_rows;
  _obj->col_widths
      = (BITCODE_BD *)calloc (tdata->num_cols, sizeof (BITCODE_BD));
  _obj->row_heights
      = (BITCODE_BD *)calloc (tdata->num_rows, sizeof (BITCODE_BD));
  _obj->cells
      = (Dwg_TABLE_Cell *)calloc (_obj->num_cells, sizeof (Dwg_TABLE_Cell));
  if (!_obj->col_widths || !_obj->row_heights || !_obj->cells)
    {
      LOG_ERROR ("Out of memory");
      free (_obj->col_widths);
      free (_obj->row_heights);
      free (_obj->cells);
      _obj->col_widths = NULL;
      _obj->row_heights = NULL;
      _obj->cells = NULL;
      _obj->num_cols = _obj->num_rows = 0;
      _obj->num_cells = 0;
      return;
    }
  for (BITCODE_BL c = 0; c < tdata->num_cols; c++)
    _obj->col_widths[c] = tdata->cols[c].width;
  for (BITCODE_BL r = 0; r < tdata->num_rows; r++)
    _obj->row_heights[r] = tdata->rows[r].height;
  for (idx = 0; idx < _obj->num_cells; idx++)
    {
      Dwg_TABLE_Cell *cell = &_obj->cells[idx];
      cell->parent = _obj;
      cell->type = 1; /* text cell */
      cell->merged_width_flag = 1;
      cell->merged_height_flag = 1;
    }
  for (BITCODE_BL r = 0; r < tdata->num_rows; r++)
    {
      Dwg_TableRow *row = &tdata->rows[r];
      BITCODE_BL ncols = row->num_cells < tdata->num_cols ? row->num_cells
                                                          : tdata->num_cols;
      for (BITCODE_BL c = 0; row->cells && c < ncols; c++)
        {
          Dwg_TableCell *tc = &row->cells[c];
          Dwg_TABLE_Cell *cell
              = &_obj->cells[(unsigned long)r * tdata->num_cols + c];
          Dwg_TableCellContent *cc;
          if (!tc->cell_contents || !tc->num_cell_contents)
            continue;
          cc = &tc->cell_contents[0];
          if (cc->type == 4 /* Block */)
            {
              cell->type = 2;
              cell->block_handle = cc->handle;
              cell->block_scale = 1.0;
              if ((cc->content_format.property_override_flags & 0x8)
                  && cc->content_format.block_scale != 0.0)
                cell->block_scale = cc->content_format.block_scale;
              if (cc->attrs && cc->num_attrs && cc->num_attrs < 1000)
                {
                  cell->attr_defs = (Dwg_TABLE_AttrDef *)calloc (
                      cc->num_attrs, sizeof (Dwg_TABLE_AttrDef));
                  if (cell->attr_defs)
                    {
                      cell->additional_data_flag = 1;
                      cell->num_attr_defs = cc->num_attrs;
                      for (BITCODE_BL i = 0; i < cc->num_attrs; i++)
                        {
                          Dwg_TABLE_AttrDef *ad = &cell->attr_defs[i];
                          ad->parent = cell;
                          ad->attdef = cc->attrs[i].attdef;
                          ad->index = (BITCODE_BS)cc->attrs[i].index;
                          ad->text = table_dup_T (dwg, cc->attrs[i].value);
                        }
                    }
                }
            }
          else
            {
              cell->text_value = table_value_text (dwg, &cc->value);
              table_cell_string_value (dwg, cell);
            }
        }
    }
  // Merged regions: the top left cell has the extent, covered cells are
  // marked as merged.
  for (BITCODE_BL i = 0; fdata->merged_cells && i < fdata->num_merged_cells;
       i++)
    {
      Dwg_FormattedTableMerged *m = &fdata->merged_cells[i];
      if (m->top_row > m->bottom_row || m->left_col > m->right_col
          || m->bottom_row >= tdata->num_rows
          || m->right_col >= tdata->num_cols)
        continue;
      for (BITCODE_BL r = m->top_row; r <= m->bottom_row; r++)
        for (BITCODE_BL c = m->left_col; c <= m->right_col; c++)
          _obj->cells[(unsigned long)r * tdata->num_cols + c].is_merged_value
              = 1;
      idx = (unsigned long)m->top_row * tdata->num_cols + m->left_col;
      _obj->cells[idx].is_merged_value = 0;
      _obj->cells[idx].merged_width_flag = m->right_col - m->left_col + 1;
      _obj->cells[idx].merged_height_flag = m->bottom_row - m->top_row + 1;
    }
  // horizontal direction from the rotation, 0x06: has block, 0x10: down
  if (_obj->horiz_direction.x == 0.0 && _obj->horiz_direction.y == 0.0
      && _obj->horiz_direction.z == 0.0)
    {
      _obj->horiz_direction.x = cos (_obj->rotation);
      _obj->horiz_direction.y = sin (_obj->rotation);
    }
  if (!_obj->flag_for_table_value)
    _obj->flag_for_table_value = 0x16;
  return;
}

/* r2007+ sources keep T fields as UTF-16, where bit_write_T would scan an
   ASCII literal as wide chars, past its end.  Shallow like those. */
static DWGCHAR cellstyle_name_table[] = { 'T', 'a', 'b', 'l', 'e', 0 };
static DWGCHAR cellstyle_name_title[] = { '_', 'T', 'I', 'T', 'L', 'E', 0 };
static DWGCHAR cellstyle_name_header[]
    = { '_', 'H', 'E', 'A', 'D', 'E', 'R', 0 };
static DWGCHAR cellstyle_name_data[] = { '_', 'D', 'A', 'T', 'A', 0 };

// The cell style borders by edge flags 1: top, 2: right, 4: bottom, 8: left,
// 0x10: inside vertical, 0x20: inside horizontal. The pre-R2010 rowstyle
// borders are: top, horizontal inside, bottom, left, vertical inside, right.
static const BITCODE_BL tablestyle_border_edge[6] = { 1, 0x20, 4, 8, 0x10, 2 };

// A color without its strings and handle, so that it needs no extra free.
static void
tablestyle_copy_color (Dwg_Color *restrict dest, const Dwg_Color *restrict src)
{
  *dest = *src;
  dest->name = NULL;
  dest->book_name = NULL;
  dest->handle = NULL;
}

static void
tablestyle_set_color (Dwg_Color *restrict c, const BITCODE_BSd index,
                      const BITCODE_BL rgb)
{
  memset (c, 0, sizeof (Dwg_Color));
  c->index = index;
  c->rgb = rgb;
  c->method = (Dwg_Color_Method)(rgb >> 24);
}

// The owner of a handle ref frees non-global ones, so do not share these.
static BITCODE_H
tablestyle_copy_ref (const BITCODE_H ref)
{
  if (!ref || ref->handleref.is_global)
    return ref;
  return dwg_add_handleref_free (ref->handleref.code, ref->absolute_ref);
}

static Dwg_TABLESTYLE_CellStyle *
tablestyle_find_cellstyle (const Dwg_Object_TABLESTYLE *restrict _obj,
                           const BITCODE_BL id)
{
  for (BITCODE_BL i = 0; _obj->cellstyles && i < _obj->num_cellstyles; i++)
    if (_obj->cellstyles[i].id == id)
      return &_obj->cellstyles[i];
  return NULL;
}

static void
rowstyle_from_cellstyle (Dwg_TABLESTYLE_rowstyles *restrict rs,
                         const Dwg_CellStyle *restrict cs)
{
  const Dwg_ContentFormat *cf = &cs->content_format;
  rs->text_style = tablestyle_copy_ref (cf->text_style);
  rs->text_height = cf->text_height;
  rs->text_alignment = cf->cell_alignment;
  tablestyle_copy_color (&rs->text_color, &cf->content_color);
  // no background: AutoCAD writes an indexed white as fill color
  rs->has_bgcolor = cs->bg_color.rgb && (cs->bg_color.rgb >> 24) != 0xc8;
  if (rs->has_bgcolor)
    tablestyle_copy_color (&rs->fill_color, &cs->bg_color);
  else
    tablestyle_set_color (&rs->fill_color, 256, 0xc3000007);
  rs->data_type = cf->value_data_type;
  rs->unit_type = cf->value_unit_type;
  rs->num_borders = 6;
  rs->borders = (Dwg_TABLESTYLE_border *)calloc (6, sizeof (*rs->borders));
  if (!rs->borders)
    {
      rs->num_borders = 0;
      return;
    }
  for (unsigned i = 0; i < 6; i++)
    {
      rs->borders[i].linewt = -2; // ByBlock
      rs->borders[i].visible = 1;
      tablestyle_set_color (&rs->borders[i].color, 0, 0xc1000000);
    }
  for (BITCODE_BL j = 0; cs->borders && j < cs->num_borders; j++)
    for (unsigned i = 0; i < 6; i++)
      if (cs->borders[j].index_mask == tablestyle_border_edge[i])
        {
          rs->borders[i].linewt = cs->borders[j].linewt;
          rs->borders[i].visible = cs->borders[j].visible ? 0 : 1;
          tablestyle_copy_color (&rs->borders[i].color, &cs->borders[j].color);
        }
}

// from >2007 to 2000: the three cell styles (title, header, data) become the
// rowstyles 0: data, 1: title, 2: header, the margins come from the table
// cell style. Custom cell styles, content formats and borders with line type
// or double lines are lost.
void
downconvert_TABLESTYLE (Dwg_Object *restrict obj)
{
  Dwg_Object_TABLESTYLE *_obj
      = obj && obj->tio.object ? obj->tio.object->tio.TABLESTYLE : NULL;
  const Dwg_CellStyle *data, *title, *header;
  Dwg_TABLESTYLE_CellStyle *cs;
  if (!obj || obj->fixedtype != DWG_TYPE_TABLESTYLE || !_obj)
    {
      LOG_ERROR ("Invalid type %u for downconvert_TABLESTYLE",
                 obj ? obj->fixedtype : 0);
      return;
    }
  // already pre-R2010 content, e.g. read from DXF
  if (_obj->rowstyles)
    return;
  LOG_WARN ("Downconverting TABLESTYLE with loosing information");
  _obj->rowstyles = (Dwg_TABLESTYLE_rowstyles *)calloc (
      3, sizeof (Dwg_TABLESTYLE_rowstyles));
  if (!_obj->rowstyles)
    return;
  _obj->num_rowstyles = 3;
  data = (cs = tablestyle_find_cellstyle (_obj, 3)) ? &cs->cellstyle
                                                     : &_obj->sty.cellstyle;
  title = (cs = tablestyle_find_cellstyle (_obj, 1)) ? &cs->cellstyle : data;
  header = (cs = tablestyle_find_cellstyle (_obj, 2)) ? &cs->cellstyle : data;
  for (unsigned i = 0; i < 3; i++)
    _obj->rowstyles[i].parent = _obj;
  rowstyle_from_cellstyle (&_obj->rowstyles[0], data);
  rowstyle_from_cellstyle (&_obj->rowstyles[1], title);
  rowstyle_from_cellstyle (&_obj->rowstyles[2], header);
  if (_obj->sty.cellstyle.margin_override_flags & 1)
    {
      _obj->horiz_cell_margin = _obj->sty.cellstyle.horiz_margin;
      _obj->vert_cell_margin = _obj->sty.cellstyle.vert_margin;
    }
}

// Fill a R2010 cell style from a pre-R2010 rowstyle.
static void
cellstyle_from_rowstyle (Dwg_TABLESTYLE_CellStyle *restrict cs,
                         Dwg_Object_TABLESTYLE *restrict parent,
                         const Dwg_TABLESTYLE_rowstyles *restrict rs,
                         const BITCODE_BL id, const BITCODE_BL class,
                         const BITCODE_BL merge_flags, DWGCHAR *restrict name,
                         const char *restrict name_tv, const int is_tu,
                         const BITCODE_BD spacing)
{
  Dwg_CellStyle *c = &cs->cellstyle;
  cs->parent = parent;
  cs->id = id;
  cs->type = class;
  cs->name = is_tu ? (char *)name : (char *)name_tv;
  c->type = 5; // as AutoCAD does
  c->data_flags = 1;
  c->merge_flags = merge_flags;
  c->content_layout = 1; // flow
  if (rs->has_bgcolor)
    tablestyle_copy_color (&c->bg_color, &rs->fill_color);
  else
    tablestyle_set_color (&c->bg_color, 0, 0xc8000000); // none
  c->content_format.value_data_type = rs->data_type ? rs->data_type : 0x200;
  c->content_format.value_unit_type = rs->unit_type;
  c->content_format.block_scale = 1.0;
  c->content_format.cell_alignment = rs->text_alignment;
  tablestyle_copy_color (&c->content_format.content_color, &rs->text_color);
  c->content_format.text_style = rs->text_style;
  c->content_format.text_height = rs->text_height;
  c->margin_override_flags = 1;
  c->vert_margin = c->bottom_margin = parent->vert_cell_margin;
  c->horiz_margin = c->right_margin = parent->horiz_cell_margin;
  c->margin_horiz_spacing = c->margin_vert_spacing = spacing;
  c->borders = (Dwg_GridFormat *)calloc (6, sizeof (Dwg_GridFormat));
  if (!c->borders)
    return;
  c->num_borders = 6;
  for (unsigned i = 0; i < 6; i++)
    {
      Dwg_GridFormat *b = &c->borders[i];
      b->parent = c;
      b->index_mask = 1U << i;
      b->border_type = 1; // single
      b->double_line_spacing = 1.125;
      b->linewt = -2;
      b->visible = 0;
      b->ltype = dwg_add_handleref_free (5, 0); // NULL, as AutoCAD
      tablestyle_set_color (&b->color, 0, 0xc1000000);
      for (unsigned k = 0; k < 6 && rs->borders && k < rs->num_borders; k++)
        if (tablestyle_border_edge[k] == b->index_mask)
          {
            b->linewt = rs->borders[k].linewt;
            b->visible = rs->borders[k].visible ? 0 : 1;
            tablestyle_copy_color (&b->color, &rs->borders[k].color);
          }
    }
}

// from <=2007 to >2007, need to populate sty.cellstyle and the three cell
// styles from rowstyles. The cell styles are freed in free_TABLESTYLE_r2010,
// which free() needs, because it follows from_version.
void
upconvert_TABLESTYLE (Dwg_Object *restrict obj)
{
  Dwg_Object_TABLESTYLE *_obj
      = obj->tio.object ? obj->tio.object->tio.TABLESTYLE : NULL;
  int is_tu;
  if (!obj || obj->fixedtype != DWG_TYPE_TABLESTYLE || !_obj)
    {
      LOG_ERROR ("Invalid type %u for upconvert_TABLESTYLE",
                 obj ? obj->fixedtype : 0);
      return;
    }
  LOG_TRACE ("Upconverting TABLESTYLE\n");
  is_tu = IS_FROM_TU_DWG (obj->parent);

  // sty: the table cell style
  if (!_obj->sty.id)
    {
      _obj->sty.id = 4;   // table
      _obj->sty.type = 2; // label
      _obj->sty.name = is_tu ? (char *)cellstyle_name_table : (char *)"Table";
    }
  if (_obj->num_rowstyles >= 3 && _obj->rowstyles)
    {
      Dwg_TABLESTYLE_rowstyles *data = &_obj->rowstyles[0];
      Dwg_CellStyle *sty = &_obj->sty.cellstyle;
      sty->type = 5; // table
      sty->data_flags = 1;
      sty->property_override_flags = 0;
      sty->merge_flags = 0;
      sty->content_layout = 1; // flow
      // the defaults of the table cell style as written by AutoCAD, the
      // title, header and data cell styles have the actual values
      tablestyle_set_color (&sty->bg_color, 0, 0xc8000000); // none
      sty->content_format.value_data_type = 0x200;
      sty->content_format.block_scale = 1.0;
      sty->content_format.text_style = dwg_add_handleref_free (5, 0); // NULL
      sty->content_format.cell_alignment = 1;
      tablestyle_copy_color (&sty->content_format.content_color,
                             &data->text_color);
      sty->content_format.text_height = 0.18;
      sty->margin_horiz_spacing = sty->margin_vert_spacing = 0.18;
      sty->margin_override_flags = 1;
      sty->vert_margin = sty->bottom_margin = _obj->vert_cell_margin;
      sty->horiz_margin = sty->right_margin = _obj->horiz_cell_margin;
      // borders: populate if already allocated (don't allocate new ones,
      // the free path won't handle them since spec macros own the lifecycle)
      if (sty->borders)
        {
          for (unsigned i = 0; i < sty->num_borders && i < data->num_borders;
               i++)
            {
              sty->borders[i].linewt = data->borders[i].linewt;
              sty->borders[i].visible = data->borders[i].visible;
              sty->borders[i].color = data->borders[i].color;
            }
        }
    }
  // the non-custom cell styles: title, header, data
  if (_obj->num_rowstyles >= 3 && _obj->rowstyles && !_obj->cellstyles)
    {
      _obj->cellstyles = (Dwg_TABLESTYLE_CellStyle *)calloc (
          3, sizeof (Dwg_TABLESTYLE_CellStyle));
      if (!_obj->cellstyles)
        return;
      _obj->num_cellstyles = 3;
      cellstyle_from_rowstyle (&_obj->cellstyles[0], _obj, &_obj->rowstyles[1],
                               1, 1, 0x8000, cellstyle_name_title, "_TITLE",
                               is_tu, _obj->rowstyles[0].text_height);
      cellstyle_from_rowstyle (&_obj->cellstyles[1], _obj, &_obj->rowstyles[2],
                               2, 1, 0, cellstyle_name_header, "_HEADER",
                               is_tu, _obj->rowstyles[0].text_height);
      cellstyle_from_rowstyle (&_obj->cellstyles[2], _obj, &_obj->rowstyles[0],
                               3, 2, 0, cellstyle_name_data, "_DATA", is_tu,
                               _obj->rowstyles[0].text_height);
    }
}
