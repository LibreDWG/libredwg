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
 * downconvert.c: DWG version conversion helpers shared by the DWG encoder
 * and the DXF/DXFB writers, which need some of them even in --disable-write
 * builds.
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
#include "classes.h"
#include "dynapi.h"
#include "free.h"
#include "decode.h"
#include "hash.h"

#define DWG_LOGLEVEL loglevel
#include "logging.h"

// Internal table lookup from dwg.c.
BITCODE_H
dwg_find_tablehandle_silent (Dwg_Data *restrict dwg, const char *restrict name,
                             const char *restrict table);

// Internal add API used while constructing conversion metadata.
void dwg_set_handle_size (Dwg_Handle *restrict hdl);
Dwg_Object_APPID *dwg_add_APPID (Dwg_Data *restrict dwg,
                                 const char *restrict name);
BITCODE_RLL dwg_obj_generic_handlevalue (void *_obj);
Dwg_Object_BLOCK_HEADER *dwg_add_BLOCK_HEADER (Dwg_Data *restrict dwg,
                                               const char *restrict name);
Dwg_Entity_BLOCK *dwg_add_BLOCK (Dwg_Object_BLOCK_HEADER *restrict hdr,
                                 const char *restrict name);
Dwg_Entity_ENDBLK *dwg_add_ENDBLK (Dwg_Object_BLOCK_HEADER *restrict hdr);
int dwg_insert_entity (Dwg_Object_BLOCK_HEADER *restrict hdr,
                       Dwg_Object *restrict obj);

/* Restore the linked list used by R13-R2000 after moving owned entities. */
static int
repeat_relink_block (Dwg_Data *restrict dwg,
                     Dwg_Object_BLOCK_HEADER *restrict hdr)
{
  BITCODE_BL i;
  hdr->first_entity = hdr->last_entity = NULL;
  for (i = 0; i < hdr->num_owned; i++)
    {
      Dwg_Object_Ref *ref = hdr->entities[i];
      Dwg_Object *obj = ref ? dwg_ref_object (dwg, ref) : NULL;
      BITCODE_RLL prev
          = i && hdr->entities[i - 1] ? hdr->entities[i - 1]->absolute_ref : 0;
      BITCODE_RLL next = i + 1 < hdr->num_owned && hdr->entities[i + 1]
                             ? hdr->entities[i + 1]->absolute_ref
                             : 0;
      if (!obj || obj->supertype != DWG_SUPERTYPE_ENTITY || !obj->tio.entity)
        return DWG_ERR_INVALIDHANDLE;
      if (!i)
        hdr->first_entity
            = dwg_add_handleref (dwg, 4, ref->absolute_ref, NULL);
      if (i + 1 == hdr->num_owned)
        hdr->last_entity = dwg_add_handleref (dwg, 4, ref->absolute_ref, NULL);
      obj->tio.entity->prev_entity = dwg_add_handleref (dwg, 4, prev, obj);
      obj->tio.entity->next_entity = dwg_add_handleref (dwg, 4, next, obj);
      obj->tio.entity->nolinks = 0;
    }
  return 0;
}

/* Turn legacy REPEAT spans into arrayed block inserts.  Nested spans are
   converted inside out; their inserts then become members of the outer block.
 */
int
upgrade_REPEAT (Dwg_Data *restrict dwg, Bit_Chain *restrict dat,
                bool *restrict converted)
{
  typedef struct
  {
    BITCODE_BL first, last;
  } Repeat_Span;
  Repeat_Span *spans;
  BITCODE_BL *stack, depth = 0, count = 0, repeats = 0, i, n;
  int error = 0;
  *converted = false;

  if (dat->from_version >= R_13b1)
    return 0;
  n = dwg->num_objects;
  for (i = 0; i < n; i++)
    {
      Dwg_Object *obj = &dwg->object[i];
      if (obj->supertype != DWG_SUPERTYPE_ENTITY || !obj->tio.entity)
        continue;
      if (obj->fixedtype == DWG_TYPE_REPEAT)
        repeats++;
      else if (obj->fixedtype == DWG_TYPE_INSERT
               || obj->fixedtype == DWG_TYPE_MINSERT)
        *converted = true;
    }
  if (!repeats)
    return 0;
  *converted = true;
  spans = (Repeat_Span *)calloc (repeats, sizeof (*spans));
  stack = (BITCODE_BL *)calloc (repeats, sizeof (*stack));
  if (!spans || !stack)
    {
      free (spans);
      free (stack);
      return DWG_ERR_OUTOFMEM;
    }
  for (i = 0; i < n; i++)
    {
      Dwg_Object *obj = &dwg->object[i];
      if (obj->supertype != DWG_SUPERTYPE_ENTITY || !obj->tio.entity)
        continue;
      /* JSON/DXF imports of pre-R13 files carry no entity handles, but the
         block ownership lists below are keyed by handle. */
      if (!obj->handle.value)
        {
          obj->handle.value = dwg_next_handseed (dwg);
          dwg_add_handle (&obj->handle, 0, obj->handle.value, obj);
        }
      if (obj->fixedtype == DWG_TYPE_REPEAT)
        stack[depth++] = i;
      else if (obj->fixedtype == DWG_TYPE_ENDREP)
        {
          if (!depth)
            {
              LOG_ERROR ("ENDREP without REPEAT");
              error = DWG_ERR_INVALIDDWG;
              goto done;
            }
          spans[count].first = stack[--depth];
          spans[count++].last = i;
        }
    }
  if (depth)
    {
      LOG_ERROR ("REPEAT without ENDREP");
      error = DWG_ERR_INVALIDDWG;
      goto done;
    }
  /* R1.x DXF output has no BLOCKS section. R2.10 supports arrayed INSERT. */
  if (dat->version < R_2_0b)
    {
      LOG_WARN ("Upgrading DXF to R2.10 to preserve REPEAT arrays");
      dat->version = dwg->header.version = R_2_10;
    }
  for (i = 0; i < count; i++)
    {
      BITCODE_BL first = spans[i].first, last = spans[i].last, j, out;
      Dwg_Object *repeat = &dwg->object[first];
      Dwg_Object *end = &dwg->object[last];
      Dwg_Entity_ENDREP values;
      Dwg_Object *owner;
      Dwg_Object_BLOCK_HEADER *oldhdr, *newhdr;
      Dwg_Object_Entity *ent;
      BITCODE_RLL owner_handle, block_handle;
      char name[32];
      unsigned serial = first;
      int err;
      void *insert;
      char *insert_name, *insert_dxfname;
      BITCODE_H blockref;

      if (!repeat->tio.entity || !end->tio.entity
          || !end->tio.entity->tio.ENDREP)
        {
          error = DWG_ERR_INVALIDDWG;
          goto done;
        }
      values = *end->tio.entity->tio.ENDREP;
      ent = repeat->tio.entity;
      owner_handle = ent->ownerhandle ? ent->ownerhandle->absolute_ref : 0;
      owner = owner_handle ? dwg_resolve_handle (dwg, owner_handle)
                           : dwg_model_space_object (dwg);
      if (!owner || owner->fixedtype != DWG_TYPE_BLOCK_HEADER
          || !owner->tio.object || !owner->tio.object->tio.BLOCK_HEADER)
        {
          error = DWG_ERR_INVALIDDWG;
          goto done;
        }
      oldhdr = owner->tio.object->tio.BLOCK_HEADER;
      owner_handle = owner->handle.value;
      if (end->tio.entity->ownerhandle
          && end->tio.entity->ownerhandle->absolute_ref
          && end->tio.entity->ownerhandle->absolute_ref != owner_handle)
        {
          error = DWG_ERR_INVALIDDWG;
          goto done;
        }
      do
        snprintf (name, sizeof (name), "*U%u", serial++);
      while (dwg_find_tablehandle_silent (dwg, name, "BLOCK"));
      newhdr = dwg_add_BLOCK_HEADER (dwg, name);
      if (!newhdr)
        {
          error = DWG_ERR_OUTOFMEM;
          goto done;
        }
      newhdr->anonymous = 1;
      newhdr->flag |= 1;
      {
        Dwg_Object *ctrl = dwg_get_first_object (dwg, DWG_TYPE_BLOCK_CONTROL);
        if (ctrl && ctrl->tio.object && ctrl->tio.object->tio.BLOCK_CONTROL)
          dwg->block_control = *ctrl->tio.object->tio.BLOCK_CONTROL;
      }
      block_handle = dwg_obj_generic_handlevalue (newhdr);
      if (!block_handle || !dwg_add_BLOCK (newhdr, name))
        {
          error = DWG_ERR_OUTOFMEM;
          goto done;
        }
      /* dwg_add_BLOCK may have moved dwg->object. */
      for (j = first + 1; j < last; j++)
        {
          Dwg_Object *child = &dwg->object[j];
          Dwg_Object_Ref *ref;
          if (child->supertype != DWG_SUPERTYPE_ENTITY || !child->tio.entity
              || child->type == DWG_TYPE_FREED
              || child->fixedtype == DWG_TYPE_JUMP
              || dwg_obj_is_subentity (child))
            continue;
          ref = child->tio.entity->ownerhandle;
          if (ref && ref->absolute_ref && ref->absolute_ref != owner_handle)
            continue;
          child->tio.entity->entmode = 0;
          if (dwg_insert_entity (newhdr, child))
            {
              error = DWG_ERR_INVALIDDWG;
              goto done;
            }
        }
      if (!dwg_add_ENDBLK (newhdr))
        {
          error = DWG_ERR_OUTOFMEM;
          goto done;
        }
      /* INSERT and MINSERT may have the same size, so allocate the larger. */
      insert
          = calloc (1, sizeof (Dwg_Entity_MINSERT) > sizeof (Dwg_Entity_INSERT)
                           ? sizeof (Dwg_Entity_MINSERT)
                           : sizeof (Dwg_Entity_INSERT));
      insert_name
          = dat->version < R_13b1 ? (char *)"INSERT" : (char *)"MINSERT";
      if (dwg->opts & DWG_OPTS_INJSON)
        insert_name = strdup (insert_name);
      insert_dxfname
          = (dwg->opts & DWG_OPTS_IN) ? strdup ("INSERT") : (char *)"INSERT";
      blockref = dwg_add_handleref (dwg, 5, block_handle, NULL);
      if (!insert || !insert_name || !insert_dxfname || !blockref)
        {
          free (insert);
          if (dwg->opts & DWG_OPTS_INJSON)
            free (insert_name);
          if (dwg->opts & DWG_OPTS_IN)
            free (insert_dxfname);
          error = DWG_ERR_OUTOFMEM;
          goto done;
        }
      repeat = &dwg->object[first];
      ent = repeat->tio.entity;
      if (dwg->opts & DWG_OPTS_IN)
        free (repeat->dxfname);
      if (dwg->opts & DWG_OPTS_INJSON)
        free (repeat->name);
      free (ent->tio.REPEAT);
      repeat->name = insert_name;
      repeat->dxfname = insert_dxfname;
      if (dat->version < R_13b1)
        {
          Dwg_Entity_INSERT *ins = (Dwg_Entity_INSERT *)insert;
          ent->tio.INSERT = ins;
          ins->parent = ent;
          ins->scale.x = ins->scale.y = ins->scale.z = 1.0;
          ins->extrusion.z = 1.0;
          ins->num_cols = values.numcols;
          ins->num_rows = values.numrows;
          ins->col_spacing = values.colspacing;
          ins->row_spacing = values.rowspacing;
          ins->block_header = blockref;
          ent->opts_r11 = 16 | 32 | 64 | 128;
          repeat->type = DWG_TYPE_INSERT_r11;
          repeat->fixedtype = DWG_TYPE_INSERT;
        }
      else
        {
          Dwg_Entity_MINSERT *ins = (Dwg_Entity_MINSERT *)insert;
          ent->tio.MINSERT = ins;
          ins->parent = ent;
          ins->scale.x = ins->scale.y = ins->scale.z = 1.0;
          ins->extrusion.z = 1.0;
          ins->num_cols = values.numcols;
          ins->num_rows = values.numrows;
          ins->col_spacing = values.colspacing;
          ins->row_spacing = values.rowspacing;
          ins->block_header = blockref;
          repeat->type = repeat->fixedtype = DWG_TYPE_MINSERT;
        }
      /* Keep the INSERT in the original owner but remove its body and ENDREP.
       */
      for (j = out = 0; j < oldhdr->num_owned; j++)
        {
          Dwg_Object_Ref *ref = oldhdr->entities[j];
          Dwg_Object *o = ref ? dwg_ref_object (dwg, ref) : NULL;
          if (o
              && (o->index == last
                  || (o->index > first && o->index < last && o->tio.entity
                      && o->tio.entity->ownerhandle
                      && o->tio.entity->ownerhandle->absolute_ref
                             == block_handle)))
            continue;
          oldhdr->entities[out++] = ref;
        }
      oldhdr->num_owned = out;
      err = repeat_relink_block (dwg, oldhdr);
      err |= repeat_relink_block (dwg, newhdr);
      if (err)
        {
          error = DWG_ERR_INVALIDDWG;
          goto done;
        }
      end = &dwg->object[last];
      {
        Dwg_Version_Type version = dwg->header.version;
        dwg->header.version = dat->from_version;
        dwg_free_object (end);
        dwg->header.version = version;
      }
      dwg_resolve_objectrefs_silent (dwg);
    }
done:
  free (spans);
  free (stack);
  return error;
}

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
// class_version
void
downconvert_MLEADERSTYLE (Dwg_Object *restrict obj)
{
  Dwg_Data *dwg = obj->parent;
  Dwg_Object_MLEADERSTYLE *_obj;
  Dwg_Object_Object *oo;
  Dwg_Object_APPID *appid;
  BITCODE_H hdl;
  BITCODE_RLL eedhdl;
  BITCODE_BL oindex;
  unsigned int idx;

  if (!obj || obj->fixedtype != DWG_TYPE_MLEADERSTYLE || !obj->tio.object)
    {
      LOG_ERROR ("Invalid type %u for downconvert_MLEADERSTYLE",
                 obj ? obj->fixedtype : 0);
      return;
    }
  oindex = obj->index;
  hdl = dwg_find_tablehandle_silent (dwg, "ACAD_MLEADERVER", "APPID");
  if (hdl)
    {
      eedhdl = hdl->handleref.value;
      LOG_TRACE ("Use APPID.ACAD_MLEADERVER (" FORMAT_HV ")\n", eedhdl);
    }
  else
    {
      appid = dwg_add_APPID (dwg, "ACAD_MLEADERVER");
      eedhdl = dwg_obj_generic_handlevalue (appid);
      LOG_TRACE ("Added APPID.ACAD_MLEADERVER (" FORMAT_HV ")\n", eedhdl);
    }
  // obj may have moved, but dirty_refs is 0 (a dirty_objs is useless)
  obj = &dwg->object[oindex];
  if (obj->fixedtype != DWG_TYPE_MLEADERSTYLE || !obj->tio.object)
    {
      LOG_ERROR ("Invalid type %u for downconvert_MLEADERSTYLE",
                 obj ? obj->fixedtype : 0);
      return;
    }
  oo = obj->tio.object;
  idx = oo->num_eed;
  if (dwg_has_eed_appid (oo, eedhdl))
    return;
  oo->num_eed += 1;
  if (idx)
    oo->eed
        = (Dwg_Eed *)realloc (oo->eed, (oo->num_eed + 1) * sizeof (Dwg_Eed));
  else
    oo->eed = (Dwg_Eed *)calloc (2, sizeof (Dwg_Eed));
  dwg_add_handle (&oo->eed[idx].handle, 5, eedhdl, NULL);
  oo->eed[idx].size = 3;
  oo->eed[idx].raw = NULL;
#define EED_SZ(n) MAX(n, sizeof(Dwg_Eed_Data))
  oo->eed[idx].data = (Dwg_Eed_Data *)calloc (EED_SZ (3), 1);
  oo->eed[idx].data->code = 70;
  _obj = oo->tio.MLEADERSTYLE;
  oo->eed[idx].data->u.eed_70.rs
      = _obj->class_version ? _obj->class_version : 2;
  idx++;
  oo->eed[idx].size = 0;
  oo->eed[idx].raw = NULL;
  oo->eed[idx].data = NULL;
}

/* R13/R14 have the standard names upper case: BYBLOCK, BYLAYER, CONTINUOUS,
   STANDARD, *ACTIVE. AutoCAD writes them as ByBlock, ByLayer, Continuous,
   Standard and *Active since R2000, also as dictionary keys. */
static const char *
standard_name_r2000 (const char *name)
{
  if (!name)
    return NULL;
  if (strEQc (name, "BYBLOCK"))
    return "ByBlock";
  if (strEQc (name, "BYLAYER"))
    return "ByLayer";
  if (strEQc (name, "CONTINUOUS"))
    return "Continuous";
  if (strEQc (name, "STANDARD"))
    return "Standard";
  if (strEQc (name, "*ACTIVE"))
    return "*Active";
  return NULL;
}

// Write R2000 as AutoCAD does: MTEXT line breaks as \P, gradients as a
// solid fill in the first gradient color. R2000 has no gradients.
void
fix_mtext_hatch (Dwg_Data *restrict dwg)
{
  for (BITCODE_BL i = 0; i < dwg->num_objects; i++)
    {
      Dwg_Object *obj = &dwg->object[i];
      Dwg_Object_Entity *_ent;
      if (obj->supertype != DWG_SUPERTYPE_ENTITY || !obj->tio.entity)
        continue;
      _ent = obj->tio.entity;
      if (obj->fixedtype == DWG_TYPE_MTEXT)
        {
          if (IS_FROM_TU_DWG (dwg))
            {
              Dwg_Entity_MTEXT *mt = _ent->tio.MTEXT;
              uint16_t *old = (BITCODE_TU)mt->text;
              if (old && memchr (old, 0x0A, 2 * bit_wcs2len ((BITCODE_TU)old)))
                {
                  uint16_t *nw
                      = calloc (2 * bit_wcs2len ((BITCODE_TU)old) + 1, 2);
                  uint16_t *d = nw;
                  for (const uint16_t *q = old; *q; q++)
                    {
                      if (*q == 0x0A)
                        {
                          *d++ = '\\';
                          *d++ = 'P';
                        }
                      else
                        *d++ = *q;
                    }
                  *d = 0;
                  LOG_TRACE ("MTEXT.text: converted LF to \\P\n");
                  free (mt->text);
                  mt->text = (BITCODE_T)nw;
                }
            }
          else
            {
              Dwg_Entity_MTEXT *mt = _ent->tio.MTEXT;
              if (mt->text && strchr (mt->text, '\n'))
                {
                  char *old = (char *)mt->text;
                  char *nw = calloc (2 * strlen (old) + 1, 1);
                  char *d = nw;
                  for (const char *q = old; *q; q++)
                    {
                      if (*q == '\n')
                        {
                          *d++ = '\\';
                          *d++ = 'P';
                        }
                      else
                        *d++ = *q;
                    }
                  *d = 0;
                  LOG_TRACE ("MTEXT.text: converted LF to \\P\n");
                  free (old);
                  mt->text = nw;
                }
            }
        }
      else if (obj->fixedtype == DWG_TYPE_HATCH)
        {
          Dwg_Entity_HATCH *h = _ent->tio.HATCH;
          if (h->is_gradient_fill)
            {
              // R2000 has no gradients. A solid fill in the first gradient
              // color, as AutoCAD's own R2000 save.
              if (h->num_colors && h->colors)
                {
                  h->colors[0].color = h->colors[0].color;
                  // put the color into the gradient colors[0] slot, which the
                  // R2000 layout reads as the solid fill color.
                  h->is_gradient_fill = 0;
                  h->is_solid_fill = 1;
                  LOG_TRACE ("HATCH: gradient written as solid fill\n");
                }
              else
                h->is_gradient_fill = 0;
            }
        }
    }
}

/* R2004+ keep the owned VERTEXes and ATTRIBs of a POLYLINE or INSERT in a
   handle list, R13-R2000 in a chain with the first and last handle. Without
   this the VERTEXes and ATTRIBs are lost in the older versions. */
void
link_owned_entities (Dwg_Data *restrict dwg)
{
  static const struct
  {
    Dwg_Object_Type type;
    const char *list, *first, *last;
  } map[] = {
    { DWG_TYPE_POLYLINE_2D, "vertex", "first_vertex", "last_vertex" },
    { DWG_TYPE_POLYLINE_3D, "vertex", "first_vertex", "last_vertex" },
    { DWG_TYPE_POLYLINE_MESH, "vertex", "first_vertex", "last_vertex" },
    { DWG_TYPE_POLYLINE_PFACE, "vertex", "first_vertex", "last_vertex" },
    { DWG_TYPE_INSERT, "attribs", "first_attrib", "last_attrib" },
    { DWG_TYPE_MINSERT, "attribs", "first_attrib", "last_attrib" },
  };
  for (BITCODE_BL i = 0; i < dwg->num_objects; i++)
    {
      Dwg_Object *obj = &dwg->object[i];
      if (obj->supertype != DWG_SUPERTYPE_ENTITY || !obj->tio.entity)
        continue;
      for (unsigned m = 0; m < ARRAY_SIZE (map); m++)
        {
          BITCODE_BL num_owned = 0;
          BITCODE_H *list = NULL;
          BITCODE_H first = NULL, last = NULL;
          Dwg_Object_Entity *_ent = obj->tio.entity;
          if (obj->fixedtype != map[m].type)
            continue;
          if (!dwg_dynapi_entity_value (_ent->tio.INSERT, obj->name,
                                        "num_owned", &num_owned, NULL)
              || !num_owned
              || !dwg_dynapi_entity_value (_ent->tio.INSERT, obj->name,
                                           map[m].list, &list, NULL)
              || !list)
            break;
          dwg_dynapi_entity_value (_ent->tio.INSERT, obj->name, map[m].first,
                                   &first, NULL);
          if (first && first->absolute_ref)
            break; // already linked
          if (list[0] && list[num_owned - 1])
            {
              // absolute (code 4), not relative to the entity as with obj
              BITCODE_H lo
                  = dwg_add_handleref (dwg, 4, list[0]->absolute_ref, NULL);
              BITCODE_H hi = dwg_add_handleref (
                  dwg, 4, list[num_owned - 1]->absolute_ref, NULL);
              if (!lo || !hi)
                return;
              first = lo;
              last = hi;
              dwg_dynapi_entity_set_value (_ent->tio.INSERT, obj->name,
                                           map[m].first, &first, 0);
              dwg_dynapi_entity_set_value (_ent->tio.INSERT, obj->name,
                                           map[m].last, &last, 0);
              LOG_TRACE ("%s.%s = " FORMAT_REF ", %s = " FORMAT_REF "\n",
                         obj->name, map[m].first, ARGS_REF (first),
                         map[m].last, ARGS_REF (last));
            }
          break;
        }
    }
}

void
upconvert_standard_names (Dwg_Data *restrict dwg)
{
  for (BITCODE_BL i = 0; i < dwg->num_objects; i++)
    {
      Dwg_Object *obj = &dwg->object[i];
      if (!obj->tio.object || obj->supertype != DWG_SUPERTYPE_OBJECT)
        continue;
      if (obj->fixedtype == DWG_TYPE_LTYPE || obj->fixedtype == DWG_TYPE_STYLE
          || obj->fixedtype == DWG_TYPE_DIMSTYLE
          || obj->fixedtype == DWG_TYPE_VPORT
          || obj->fixedtype == DWG_TYPE_MLINESTYLE)
        {
          BITCODE_T name = NULL;
          const char *newname;
          if (dwg_dynapi_entity_value (obj->tio.object->tio.APPID, obj->name,
                                       "name", &name, NULL)
              && (newname = standard_name_r2000 (name)))
            {
              LOG_TRACE ("%s.name %s => %s\n", obj->name, name, newname);
              dwg_dynapi_entity_set_value (obj->tio.object->tio.APPID,
                                           obj->name, "name", &newname, 0);
            }
        }
      else if (obj->fixedtype == DWG_TYPE_DICTIONARY
               && obj->tio.object->tio.DICTIONARY)
        {
          Dwg_Object_DICTIONARY *_dict = obj->tio.object->tio.DICTIONARY;
          for (BITCODE_BL j = 0; _dict->texts && j < _dict->numitems; j++)
            {
              const char *newname = standard_name_r2000 (_dict->texts[j]);
              if (newname)
                {
                  char *dup = strdup (newname);
                  if (!dup)
                    return;
                  LOG_TRACE ("DICTIONARY.texts[%u] %s => %s\n", j,
                             _dict->texts[j], newname);
                  free (_dict->texts[j]);
                  _dict->texts[j] = dup;
                }
            }
        }
    }
}

void
downconvert_DIMSTYLE (Bit_Chain *restrict dat, Dwg_Object *restrict obj)
{
  Dwg_Data *dwg = obj->parent;
  Dwg_Object_DIMSTYLE *_obj;
  Dwg_Object_Object *oo;
  Dwg_Object_APPID *appid;
  BITCODE_RLL eedhdl1, eedhdl2, eedhdl3;
  BITCODE_BL oindex;
  BITCODE_H hdl;
  unsigned int idx;

  if (!obj || obj->fixedtype != DWG_TYPE_DIMSTYLE || !obj->tio.object)
    {
      LOG_ERROR ("Invalid type %u for downconvert_DIMSTYLE",
                 obj ? obj->fixedtype : 0);
      return;
    }
  _obj = obj->tio.object->tio.DIMSTYLE;
  if (!bit_eq_T (dat, _obj->name, "Annotative"))
    return;

  oindex = obj->index;
  hdl = dwg_find_tablehandle_silent (dwg, "AcadAnnotative", "APPID");
  if (hdl)
    {
      eedhdl1 = hdl->handleref.value;
      LOG_TRACE ("Use APPID.AcadAnnotative (" FORMAT_HV ")\n", eedhdl1);
    }
  else
    {
      appid = dwg_add_APPID (dwg, "AcadAnnotative");
      eedhdl1 = dwg_obj_generic_handlevalue (appid);
      LOG_TRACE ("Added APPID.AcadAnnotative (" FORMAT_HV ")\n", eedhdl1);
    }
  hdl = dwg_find_tablehandle_silent (dwg, "ACAD_DSTYLE_DIMJAG", "APPID");
  if (hdl)
    {
      eedhdl2 = hdl->handleref.value;
      LOG_TRACE ("Use APPID.ACAD_DSTYLE_DIMJAG (" FORMAT_HV ")\n", eedhdl2);
    }
  else
    {
      appid = dwg_add_APPID (dwg, "ACAD_DSTYLE_DIMJAG");
      eedhdl2 = dwg_obj_generic_handlevalue (appid);
      LOG_TRACE ("Added APPID.ACAD_DSTYLE_DIMJAG (" FORMAT_HV ")\n", eedhdl2);
    }
  hdl = dwg_find_tablehandle_silent (dwg, "ACAD_DSTYLE_DIMTALN", "APPID");
  if (hdl)
    {
      eedhdl3 = hdl->handleref.value;
      LOG_TRACE ("Use APPID.ACAD_DSTYLE_DIMTALN (" FORMAT_HV ")\n", eedhdl3);
    }
  else
    {
      appid = dwg_add_APPID (dwg, "ACAD_DSTYLE_DIMTALN");
      eedhdl3 = dwg_obj_generic_handlevalue (appid);
      LOG_TRACE ("Added APPID.ACAD_DSTYLE_DIMTALN (" FORMAT_HV ")\n", eedhdl3);
    }
  // obj may have moved, but dirty_refs is 0 (a dirty_objs is useless)
  obj = &dwg->object[oindex];
  if (obj->fixedtype != DWG_TYPE_DIMSTYLE || !obj->tio.object)
    {
      LOG_ERROR ("Invalid type %u for downconvert_DIMSTYLE",
                 obj ? obj->fixedtype : 0);
      return;
    }
  oo = obj->tio.object;
  idx = oo->num_eed;
  if (!dwg_has_eed_appid (oo, eedhdl1))
    {
      LOG_TRACE ("Add EED for AcadAnnotative to " FORMAT_HV "\n",
                 obj->handle.value);
      oo->num_eed += 5;
      if (idx)
        oo->eed = (Dwg_Eed *)realloc (oo->eed,
                                      (oo->num_eed + 1) * sizeof (Dwg_Eed));
      else
        oo->eed = (Dwg_Eed *)calloc (6, sizeof (Dwg_Eed));
      // AnnotativeData
      dwg_add_handle (&oo->eed[idx].handle, 5, eedhdl1, NULL);
      oo->eed[idx].size = 28;
      oo->eed[idx].raw = NULL;
      oo->eed[idx].data = (Dwg_Eed_Data *)calloc (EED_SZ (20), 1);
      oo->eed[idx].data->code = 0;
      oo->eed[idx].data->u.eed_0.length = 14; // sizeof ("AnnotativeData") - 1;
      oo->eed[idx].data->u.eed_0.codepage = 30;
      memcpy (oo->eed[idx].data->u.eed_0.string, "AnnotativeData", 15);
      idx++;
      oo->eed[idx].size = 0;
      oo->eed[idx].raw = NULL;
      oo->eed[idx].data = (Dwg_Eed_Data *)calloc (EED_SZ (2), 1);
      oo->eed[idx].data->code = 2; // open
      idx++;
      oo->eed[idx].size = 0;
      oo->eed[idx].raw = NULL;
      oo->eed[idx].data = (Dwg_Eed_Data *)calloc (EED_SZ (3), 1);
      oo->eed[idx].data->code = 70;
      oo->eed[idx].data->u.eed_70.rs = 1;
      idx++;
      oo->eed[idx].size = 0;
      oo->eed[idx].raw = NULL;
      oo->eed[idx].data = (Dwg_Eed_Data *)calloc (EED_SZ (3), 1);
      oo->eed[idx].data->code = 70;
      oo->eed[idx].data->u.eed_70.rs = 1;
      idx++;
      oo->eed[idx].size = 0;
      oo->eed[idx].raw = NULL;
      oo->eed[idx].data = (Dwg_Eed_Data *)calloc (EED_SZ (2), 1);
      oo->eed[idx].data->code = 2;
      oo->eed[idx].data->u.eed_2.close = 1;
      idx++;
      oo->eed[idx].size = 0;
      oo->eed[idx].data = NULL;
      oo->eed[idx].raw = NULL;
    }

  if (!dwg_has_eed_appid (oo, eedhdl2))
    {
      LOG_TRACE ("Add EED for ACAD_DSTYLE_DIMJAG to " FORMAT_HV "\n",
                 obj->handle.value);
      oo->num_eed += 2;
      if (idx)
        oo->eed = (Dwg_Eed *)realloc (oo->eed,
                                      (oo->num_eed + 1) * sizeof (Dwg_Eed));
      else
        oo->eed = (Dwg_Eed *)calloc (3, sizeof (Dwg_Eed));
      // DIMJAG
      dwg_add_handle (&oo->eed[idx].handle, 5, eedhdl2, NULL);
      oo->eed[idx].size = 12;
      oo->eed[idx].raw = NULL;
      oo->eed[idx].data = (Dwg_Eed_Data *)calloc (EED_SZ (3), 1);
      oo->eed[idx].data->code = 70;
      //_obj = oo->tio.DIMSTYLE;
      oo->eed[idx].data->u.eed_70.rs = 388; // FIXME Which value?
      idx++;
      oo->eed[idx].size = 0;
      oo->eed[idx].raw = NULL;
      oo->eed[idx].data = (Dwg_Eed_Data *)calloc (EED_SZ (9), 1);
      oo->eed[idx].data->code = 40;
      oo->eed[idx].data->u.eed_40.real = 1.5; // FIXME Which value?
      idx++;
      oo->eed[idx].size = 0;
      oo->eed[idx].data = NULL;
      oo->eed[idx].raw = NULL;
    }
  if (!dwg_has_eed_appid (oo, eedhdl3))
    {
      LOG_TRACE ("Add EED for ACAD_DSTYLE_DIMTALN to " FORMAT_HV "\n",
                 obj->handle.value);
      oo->num_eed += 2;
      if (idx)
        oo->eed = (Dwg_Eed *)realloc (oo->eed,
                                      (oo->num_eed + 1) * sizeof (Dwg_Eed));
      else
        oo->eed = (Dwg_Eed *)calloc (3, sizeof (Dwg_Eed));
      // DIMTALN
      dwg_add_handle (&oo->eed[idx].handle, 5, eedhdl2, NULL);
      oo->eed[idx].size = 6;
      oo->eed[idx].raw = NULL;
      oo->eed[idx].data = (Dwg_Eed_Data *)calloc (EED_SZ (3), 1);
      oo->eed[idx].data->code = 70;
      //_obj = oo->tio.DIMSTYLE;
      oo->eed[idx].data->u.eed_70.rs = 392; // FIXME Which value?
      idx++;
      oo->eed[idx].size = 0;
      oo->eed[idx].raw = NULL;
      oo->eed[idx].data = (Dwg_Eed_Data *)calloc (EED_SZ (3), 1);
      oo->eed[idx].data->code = 70;
      oo->eed[idx].data->u.eed_70.rs = 0; // FIXME Which value?
      idx++;
      oo->eed[idx].size = 0;
      oo->eed[idx].data = NULL;
      oo->eed[idx].raw = NULL;
    }
#undef EED_SZ
  if (idx != oo->num_eed)
    {
      // eg. when the EED already had AcadAnnotative
      LOG_WARN ("Already DIMSTYLE(" FORMAT_HV ") eed idx %u vs num_eed %u\n",
                obj->handle.value, idx, oo->num_eed);
      oo->eed[idx].data = NULL;
      oo->num_eed = idx;
      oo->eed = (Dwg_Eed *)realloc (oo->eed, oo->num_eed * sizeof (Dwg_Eed));
    }
}
/* from 2018 to earlier */
void
dwg_downgrade_MLINESTYLE (Dwg_Object_MLINESTYLE *o)
{
  BITCODE_BSd lt_index
      = strEQc (o->name, "CONTINUOUS") || strEQc (o->name, "Continuous")
            ? 0
            : 32767; // or 32766 for BYBLOCK
  for (BITCODE_RC j = 0; j < o->num_lines; j++)
    {
      // TODO lookup lt_ltype
      o->lines[j].lt_index = lt_index;
    }
}

/* to 2018 */
void
dwg_upgrade_MLINESTYLE (Dwg_Data *restrict dwg,
                        Dwg_Object_MLINESTYLE *restrict o)
{
  // lookup on LTYPE_CONTROL list
  for (BITCODE_RC j = 0; j < o->num_lines; j++)
    {
      BITCODE_BSd lt_index = o->lines[j].lt_index;
      LOG_TRACE ("MLINESTYLE.lines[%d].lt_index = %d [BSd 6]\n", j,
                 (int)lt_index);
      if (lt_index == 0)
        o->lines[j].lt_ltype = dwg->header_vars.LTYPE_CONTINUOUS;
      else if (lt_index == 32767)
        o->lines[j].lt_ltype = dwg->header_vars.LTYPE_BYLAYER;
      else if (lt_index == 32766)
        o->lines[j].lt_ltype = dwg->header_vars.LTYPE_BYBLOCK;
      else if (lt_index > 0)
        {
          BITCODE_H hdl
              = dwg_find_tablehandle_index (dwg, (int)lt_index, "LTYPE");
          o->lines[j].lt_ltype
              = dwg_add_handleref (dwg, 5, hdl ? hdl->absolute_ref : 0, NULL);
          if (hdl)
            LOG_TRACE ("MLINESTYLE.lines[%d].lt_ltype %s => " FORMAT_REF
                       " [H]\n",
                       j, o->name, ARGS_REF (hdl));
        }
      else
        o->lines[j].lt_ltype = dwg_add_handleref (dwg, 5, 0, NULL);
    }
}

/* Public add API, see dwg_add_POLYLINE_2D() in dwg_api.h. The layout of
   dwg_point_2d is that of BITCODE_2RD. */
Dwg_Entity_POLYLINE_2D *
dwg_add_POLYLINE_2D (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                     const int num_pts, const BITCODE_2RD *restrict pts);

/* Copy the common entity properties which the pre-R13 formats know. */
static void
copy_entity_props (Dwg_Object_Entity *restrict dst,
                   const Dwg_Object_Entity *restrict src)
{
  dst->layer = src->layer;
  dst->ltype = src->ltype;
  dst->color = src->color;
  dst->ltype_scale = src->ltype_scale;
  dst->ltype_flags = src->ltype_flags;
  dst->isbylayerlt = src->isbylayerlt;
  dst->linewt = src->linewt;
  dst->invisible = src->invisible;
}

/* Convert the LWPOLYLINE at dwg->object[idx] to a POLYLINE_2D with its
   VERTEX_2Ds and SEQEND, at the same position. */
static int
lwpolyline_to_polyline (Dwg_Data *restrict dwg, const BITCODE_BL idx)
{
  Dwg_Object *lw = &dwg->object[idx];
  Dwg_Object_Entity *ent = lw->tio.entity;
  Dwg_Entity_LWPOLYLINE *_lw = ent->tio.LWPOLYLINE;
  Dwg_Object *owner = ent->ownerhandle ? dwg_ref_object (dwg, ent->ownerhandle)
                                       : NULL;
  Dwg_Object_BLOCK_HEADER *hdr;
  Dwg_Entity_POLYLINE_2D *pl;
  const BITCODE_RLL lw_handle = lw->handle.value;
  BITCODE_BL first_new, m, c, j, i, n = _lw->num_points;
  BITCODE_BL num_objects = dwg->num_objects;
  Dwg_Object *tmp;

  if (!owner || owner->fixedtype != DWG_TYPE_BLOCK_HEADER)
    owner = dwg_model_space_object (dwg);
  /* dwg_insert_entity() requires the model space. A fuzzed DWG may have none. */
  if (!owner || !owner->tio.object || !owner->tio.object->tio.BLOCK_HEADER
      || !dwg_model_space_ref (dwg) || !dwg_paper_space_ref (dwg))
    return DWG_ERR_INVALIDDWG;
  hdr = owner->tio.object->tio.BLOCK_HEADER;
  if (n && !_lw->points)
    n = 0;

  first_new = dwg->num_objects;
  /* With the pre-R13 target version the API would set the r11 types, which
     the free functions and the encoder do not expect for an R13+ source. */
  {
    Dwg_Version_Type version = dwg->header.version;
    dwg->header.version = dwg->header.from_version;
    pl = n ? dwg_add_POLYLINE_2D (hdr, (int)n, _lw->points) : NULL;
    dwg->header.version = version;
  }
  /* The add API may have moved dwg->object. */
  lw = &dwg->object[idx];
  ent = lw->tio.entity;
  _lw = ent->tio.LWPOLYLINE;
  if (n && !pl)
    return DWG_ERR_OUTOFMEM;

  if (pl)
    {
      Dwg_Object *pobj = &dwg->object[first_new];
      m = dwg->num_objects - first_new; /* POLYLINE, n VERTEX, SEQEND */
      if (m != n + 2)
        return DWG_ERR_INVALIDDWG;
      copy_entity_props (pobj->tio.entity, ent);
      pobj->tio.entity->entmode = ent->entmode;
      pl->flag &= ~1;
      if (_lw->flag & 512)
        pl->flag |= 1; /* closed */
      if (_lw->flag & 4)
        pl->start_width = pl->end_width = _lw->const_width;
      pl->thickness = _lw->thickness;
      pl->elevation = _lw->elevation;
      pl->extrusion = _lw->extrusion;
      for (j = 0; j < m; j++)
        {
          Dwg_Object *o = &dwg->object[first_new + j];
          if (j)
            {
              copy_entity_props (o->tio.entity, ent);
              o->tio.entity->entmode = 0; /* owned by the POLYLINE */
            }
          if (j >= 1 && j <= n)
            {
              Dwg_Entity_VERTEX_2D *v = o->tio.entity->tio.VERTEX_2D;
              v->point.z = _lw->elevation;
              if (j - 1 < _lw->num_bulges && _lw->bulges)
                v->bulge = _lw->bulges[j - 1];
              if (j - 1 < _lw->num_widths && _lw->widths)
                {
                  v->start_width = _lw->widths[j - 1].start;
                  v->end_width = _lw->widths[j - 1].end;
                }
            }
        }
      /* keep the order in the owner */
      for (j = 0; j < hdr->num_owned; j++)
        if (hdr->entities && hdr->entities[j]
            && hdr->entities[j]->absolute_ref == lw_handle)
          {
            if (hdr->num_owned > 1 && hdr->entities[hdr->num_owned - 1])
              {
                hdr->entities[j] = hdr->entities[hdr->num_owned - 1];
                hdr->num_owned--;
              }
            break;
          }
    }
  else
    m = 0; /* no points, just drop it */

  /* Free the LWPOLYLINE. The free functions depend on the source version. */
  {
    Dwg_Version_Type version = dwg->header.version;
    dwg->header.version = dwg->header.from_version;
    dwg_free_object (lw);
    dwg->header.version = version;
  }
  /* New order: [0, idx), the new objects, (idx, first_new) */
  c = first_new - idx - 1;
  if (m)
    {
      tmp = (Dwg_Object *)malloc (m * sizeof (Dwg_Object));
      if (!tmp)
        return DWG_ERR_OUTOFMEM;
      memcpy (tmp, &dwg->object[first_new], m * sizeof (Dwg_Object));
      memmove (&dwg->object[idx + m], &dwg->object[idx + 1],
               c * sizeof (Dwg_Object));
      memcpy (&dwg->object[idx], tmp, m * sizeof (Dwg_Object));
      free (tmp);
    }
  else
    memmove (&dwg->object[idx], &dwg->object[idx + 1],
             c * sizeof (Dwg_Object));
  dwg->num_objects = num_objects - 1 + m;
  /* Both the objects and the handle map need the new indices. */
  for (i = idx; i < dwg->num_objects; i++)
    {
      Dwg_Object *o = &dwg->object[i];
      o->index = i;
      if (o->supertype == DWG_SUPERTYPE_ENTITY && o->tio.entity)
        o->tio.entity->objid = i;
      else if (o->supertype == DWG_SUPERTYPE_OBJECT && o->tio.object)
        o->tio.object->objid = i;
      if (o->handle.value && dwg->object_map)
        hash_set (dwg->object_map, o->handle.value, i);
    }
  /* The refs point to the old positions. */
  dwg_resolve_objectrefs_silent (dwg);
  return 0;
}

/* Pre-R13 has no LWPOLYLINE. Convert them to POLYLINE_2D, instead of writing
   them as invalid objects. */
int
downconvert_LWPOLYLINE (Dwg_Data *restrict dwg)
{
  int error = 0;
  for (BITCODE_BL k = dwg->num_objects; k > 0; k--)
    {
      Dwg_Object *obj = &dwg->object[k - 1];
      if (obj->fixedtype == DWG_TYPE_LWPOLYLINE
          && obj->supertype == DWG_SUPERTYPE_ENTITY && obj->tio.entity
          && obj->tio.entity->tio.LWPOLYLINE)
        {
          error |= lwpolyline_to_polyline (dwg, k - 1);
          if (error >= DWG_ERR_CRITICAL)
            break;
        }
    }
  return error;
}

/* R13-R2000 DWGs only have the entity chain, not the BLOCK_HEADER.entities
   list which the pre-R13 DXF output walks. Build it from the chain, without
   the sub-entities (VERTEX, ATTRIB) and SEQEND. */
int
fill_owned_entities (Dwg_Data *restrict dwg)
{
  for (BITCODE_BL i = 0; i < dwg->num_objects; i++)
    {
      Dwg_Object *obj = &dwg->object[i];
      Dwg_Object_BLOCK_HEADER *hdr;
      Dwg_Object *o;
      BITCODE_BL steps = 0;
      if (obj->fixedtype != DWG_TYPE_BLOCK_HEADER || !obj->tio.object
          || !(hdr = obj->tio.object->tio.BLOCK_HEADER) || hdr->num_owned
          || hdr->entities || !hdr->first_entity || !hdr->last_entity
          || !hdr->last_entity->absolute_ref)
        continue;
      o = dwg_ref_object (dwg, hdr->first_entity);
      while (o && o->supertype == DWG_SUPERTYPE_ENTITY && o->tio.entity
             && steps++ <= dwg->num_objects)
        {
          if (!is_attrib_vertex (o->fixedtype)
              && o->fixedtype != DWG_TYPE_SEQEND)
            {
              BITCODE_H *tmp = (BITCODE_H *)realloc (
                  hdr->entities, (hdr->num_owned + 1) * sizeof (BITCODE_H));
              if (!tmp)
                return DWG_ERR_OUTOFMEM;
              hdr->entities = tmp;
              hdr->entities[hdr->num_owned++]
                  = dwg_add_handleref (dwg, 4, o->handle.value, NULL);
            }
          if (o->handle.value == hdr->last_entity->absolute_ref)
            break;
          o = dwg_next_entity (o);
        }
    }
  return 0;
}

#if 0
// from >2000 to 2000-r13, no relative refs,. FIXME: except prev,next links.
// only with older objects, not class objects.
static void
downconvert_relative_handle (BITCODE_H ref, Dwg_Object *restrict obj)
{
  if (!obj)
    return;
  /*
  if (obj->type >= 500
      && obj->fixedtype != DWG_TYPE_DICTIONARYWDFLT
      && obj->fixedtype != DWG_TYPE_PLACEHOLDER
      && obj->fixedtype != DWG_TYPE_LAYOUT
      && obj->fixedtype != DWG_TYPE_MATERIAL
      && obj->fixedtype != DWG_TYPE_TABLESTYLE
      && obj->fixedtype != DWG_TYPE_DICTIONARYVAR)
    return;
  */
  assert (ref->handleref.code > 5);
  if (ref->absolute_ref)
    {
      ref->handleref.value = ref->absolute_ref;
      ref->handleref.code = 4;
      dwg_set_handle_size (&ref->handleref);
      return;
    }
  else
    {
      assert (obj);
      switch (ref->handleref.code)
        {
        case 6:
          ref->handleref.value = obj->handle.value + 1;
          dwg_set_handle_size (&ref->handleref);
          break;
        case 8:
          ref->handleref.value = obj->handle.value - 1;
          break;
        case 10:
          ref->handleref.value = obj->handle.value + ref->handleref.value;
          break;
        case 12:
          ref->handleref.value = (obj->handle.value - ref->handleref.value);
          break;
        case 14: // eg 2007 REGION.history_id (some very high number)
          ref->handleref.value = obj->handle.value;
          break;
        default:
          return;
        }
      ref->handleref.code = 4;
      dwg_set_handle_size (&ref->handleref);
    }
}
#endif


// Add n empty EED items and the terminating one. Returns the first new item,
// or NULL on out of memory.
static Dwg_Eed *
eed_extend (Dwg_Eed *restrict *restrict eedp, BITCODE_BL *restrict num_eed,
            BITCODE_BL n)
{
  BITCODE_BL idx = *num_eed;
  Dwg_Eed *eed = (Dwg_Eed *)realloc (*eedp, (idx + n + 1) * sizeof (Dwg_Eed));
  if (!eed)
    return NULL;
  memset (&eed[idx], 0, (n + 1) * sizeof (Dwg_Eed));
  *eedp = eed;
  *num_eed = idx + n;
  return &eed[idx];
}

static bool
eed_item (Dwg_Eed *restrict eed, const BITCODE_RC code, const size_t size)
{
  eed->data = (Dwg_Eed_Data *)calloc (1, size);
  if (!eed->data)
    return false;
  eed->data->code = code;
  return true;
}

static bool
eed_item_70 (Dwg_Eed *restrict eed, const BITCODE_RS value)
{
  if (!eed_item (eed, 70, 3))
    return false;
  eed->data->u.eed_70.rs = value;
  return true;
}

static bool
eed_item_11 (Dwg_Eed *restrict eed, const double x, const double y,
             const double z)
{
  if (!eed_item (eed, 11, 1 + 3 * sizeof (double)))
    return false;
  eed->data->u.eed_10.point.x = x;
  eed->data->u.eed_10.point.y = y;
  eed->data->u.eed_10.point.z = z;
  return true;
}

// The handle of the APPID, added when missing. This may move the objects.
static BITCODE_RLL
eed_appid_handle (Dwg_Data *restrict dwg, const char *restrict name)
{
  BITCODE_H hdl = dwg_find_tablehandle_silent (dwg, name, "APPID");
  if (hdl)
    {
      LOG_TRACE ("Use APPID.%s (" FORMAT_RLLx ")\n", name,
                 (BITCODE_RLL)hdl->handleref.value);
      return hdl->handleref.value;
    }
  else
    {
      Dwg_Object_APPID *appid = dwg_add_APPID (dwg, name);
      BITCODE_RLL value = dwg_obj_generic_handlevalue (appid);
      LOG_TRACE ("Added APPID.%s (" FORMAT_RLLx ")\n", name, value);
      return value;
    }
}

// Compare a name of the source, TU for r2007+ DWGs, else TV, with an ASCII one.
static bool
material_name_is (const Dwg_Data *restrict dwg, const BITCODE_T name,
                  const char *restrict ascii)
{
  if (!name)
    return false;
  if (IS_FROM_TU_DWG (dwg))
    {
      const BITCODE_RS *tu = (const BITCODE_RS *)name;
      size_t i;
      for (i = 0; ascii[i]; i++)
        if (tu[i] != (unsigned char)ascii[i])
          return false;
      return tu[i] == 0;
    }
  return strEQ ((const char *)name, ascii);
}

static bool
is_default_material (const Dwg_Data *restrict dwg, const BITCODE_T name)
{
  return material_name_is (dwg, name, "ByLayer")
         || material_name_is (dwg, name, "ByBlock");
}

// from >2007 to 2000, no Texture_diffusemap, translucence, ...
// The ACAD EED of ODA's r2000 MATERIALs is: 70 -1, 70 3, 70 x, 0 "", 71 y, 70 0
// ByLayer and ByBlock have none, "Global" has x=0, y=0. Other materials have
// x in {0, 1, 4} and y in {0, 8, 64, 72}, but what they mean is not known.
// We use the most common for user materials: x=1, y=0.
void
dwg_downgrade_MATERIAL (Dwg_Object *obj)
{
  Dwg_Data *dwg = obj->parent;
  Dwg_Object_Object *oo;
  Dwg_Eed *eed;
  BITCODE_RLL eedhdl;
  BITCODE_BL oindex;
  BITCODE_T name;

  if (!obj || obj->fixedtype != DWG_TYPE_MATERIAL || !obj->tio.object
      || !obj->tio.object->tio.MATERIAL)
    {
      LOG_ERROR ("Invalid type %s (%u) for dwg_downgrade_MATERIAL",
                 obj ? dwg_type_name (obj->fixedtype) : "",
                 obj ? obj->fixedtype : 0);
      return;
    }
  name = obj->tio.object->tio.MATERIAL->name;
  if (is_default_material (dwg, name))
    return;
  oindex = obj->index;
  eedhdl = eed_appid_handle (dwg, "ACAD");
  // obj may have moved, but dirty_refs is 0 (a dirty_objs is useless)
  obj = &dwg->object[oindex];
  if (obj->fixedtype != DWG_TYPE_MATERIAL || !obj->tio.object)
    {
      LOG_ERROR ("Invalid type %s (%u) for dwg_downgrade_MATERIAL",
                 dwg_type_name (obj->fixedtype), obj->fixedtype);
      return;
    }
  oo = obj->tio.object;
  if (dwg_has_eed_appid (oo, eedhdl))
    return;
  LOG_TRACE ("Downgrade MATERIAL " FORMAT_RLLx
             ". Add ACAD EED from %d to %d values\n",
             obj->handle.value, oo->num_eed, oo->num_eed + 6);
  eed = eed_extend (&oo->eed, &oo->num_eed, 6);
  if (!eed)
    return;
  dwg_add_handle (&eed[0].handle, 5, eedhdl, NULL);
  eed[0].size = 21;
  eed_item_70 (&eed[0], 65535); // -1
  eed_item_70 (&eed[1], 3);
  eed_item_70 (&eed[2], material_name_is (dwg, name, "Global") ? 0 : 1);
  // an empty string: length and codepage 0
  eed_item (&eed[3], 0, 6);
  if (eed_item (&eed[4], 71, 5))
    eed[4].data->u.eed_71.rl = 0;
  eed_item_70 (&eed[5], 0);
}

// from >2007 to 2000: an entity with a material (not ByLayer) gets the
// default mapper EEDs, as ODA does:
// ACAD_MATERIAL_MAPPER: 70 2, 70 0, 70 0, 4x 11 (an identity matrix 4x3)
// ACAD_MATERIAL_MAPPER_2: 70 0, 70 0
// There is no "RTMaterial" EED in any ODA sample, the link to the material
// is lost in r2000.
void
dwg_downgrade_entity_material (Dwg_Data *restrict dwg, const BITCODE_BL oindex)
{
  Dwg_Object *obj = &dwg->object[oindex];
  Dwg_Object_Entity *ent;
  Dwg_Object *mat;
  Dwg_Eed *eed;
  BITCODE_RLL hdl1, hdl2;

  if (obj->supertype != DWG_SUPERTYPE_ENTITY || !(ent = obj->tio.entity)
      || !ent->material || !ent->material->absolute_ref)
    return;
  mat = dwg_ref_object_silent (dwg, ent->material);
  if (!mat || mat->fixedtype != DWG_TYPE_MATERIAL || !mat->tio.object
      || !mat->tio.object->tio.MATERIAL
      || is_default_material (dwg, mat->tio.object->tio.MATERIAL->name))
    return;
  hdl1 = eed_appid_handle (dwg, "ACAD_MATERIAL_MAPPER");
  hdl2 = eed_appid_handle (dwg, "ACAD_MATERIAL_MAPPER_2");
  // the objects may have moved
  obj = &dwg->object[oindex];
  ent = obj->tio.entity;
  if (!ent)
    return;
  for (BITCODE_BL i = 0; i < ent->num_eed; i++)
    if (ent->eed[i].handle.value == hdl1)
      return;
  LOG_TRACE ("Add MATERIAL_MAPPER EED to %s " FORMAT_RLLx "\n", obj->name,
             obj->handle.value);
  eed = eed_extend (&ent->eed, &ent->num_eed, 7 + 2);
  if (!eed)
    return;
  dwg_add_handle (&eed[0].handle, 5, hdl1, NULL);
  eed[0].size = 3 * 3 + 4 * (1 + 3 * sizeof (double));
  eed_item_70 (&eed[0], 2);
  eed_item_70 (&eed[1], 0);
  eed_item_70 (&eed[2], 0);
  eed_item_11 (&eed[3], 0.0, 0.0, 0.0);
  eed_item_11 (&eed[4], 1.0, 0.0, 0.0);
  eed_item_11 (&eed[5], 0.0, 1.0, 0.0);
  eed_item_11 (&eed[6], 0.0, 0.0, 1.0);
  dwg_add_handle (&eed[7].handle, 5, hdl2, NULL);
  eed[7].size = 2 * 3;
  eed_item_70 (&eed[7], 0);
  eed_item_70 (&eed[8], 0);
}

// up or downconvert from/to 256/512 bytes
void
dwg_convert_LTYPE_strings_area (const Dwg_Data *restrict dwg,
                                Dwg_Object_LTYPE *restrict _obj)
{
  // add_LTYPE_dashes builds strings_area for DXF input sized by the source
  // (header.version during the read): exactly 256 for a <=r2004 source, 512
  // for a >=r2007 source. The <=r2004 -> r2007 upconvert reads old[0..255] and
  // writes new[i*2], safe on a 256-byte buffer, and is required so a <=r2004
  // source is not written past its 256-byte allocation for a r2007+ target
  // (GH #1486). The r2007 -> <=r2004 downconvert instead reads old[i*2] up to
  // index 510, which would overread a 256-byte DXF buffer, so it stays skipped
  // for DXF input.
  if ((dwg->opts & DWG_OPTS_INDXF)
      && !(dwg->header.from_version <= R_2004 && dwg->header.version > R_2004))
    return;
  if (dwg->header.from_version <= R_2004 && dwg->header.version > R_2004)
    {
      // upconvert to 512
      BITCODE_TF old = _obj->strings_area;
      if (!old)
        {
          _obj->has_strings_area = 0;
          return;
        }
      _obj->strings_area = (BITCODE_TF)calloc (1, 512);
      if (!_obj->strings_area)
        {
          _obj->has_strings_area = 0;
          free (old);
          return;
        }
      _obj->has_strings_area = 1;
      for (int i = 0; i < 256; i++)
        {
          _obj->strings_area[i * 2] = old[i];
        }
      free (old);
    }
  else if (dwg->header.from_version > R_2004 && dwg->header.version <= R_2004)
    {
      // downconvert to 256
      BITCODE_TF old = _obj->strings_area;
      if (!old)
        _obj->has_strings_area = 0;
      _obj->strings_area = (BITCODE_TF)calloc (1, 256);
      if (!_obj->strings_area || !old)
        { // all empty
          if (old)
            free (old);
          if (_obj->strings_area)
            free (_obj->strings_area);
          _obj->strings_area = NULL;
          return;
        }
      _obj->has_strings_area = 1;
      for (int i = 0; i < 256; i++)
        {
          _obj->strings_area[i] = old[i * 2];
        }
      free (old);
    }
}
