/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2018-2020 Free Software Foundation, Inc.                   */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * dwggrep.c: search a string in all text values in a DWG
 * TODO scan the dwg.spec for all text DXF codes, per object.
 * TODO --recursive filewalker
 *
 * written by Reini Urban
 */

#define _GNU_SOURCE
#include "../src/config.h"
#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_STRCASESTR
#  undef __DARWIN_C_LEVEL
#  define __DARWIN_C_LEVEL __DARWIN_C_FULL
#  ifndef __USE_GNU
#    define __USE_GNU
#  endif
#  ifndef __BSD_VISIBLE
#    define __BSD_VISIBLE 1
#  endif
#  include <string.h>
#else
#  include <string.h>
#  include <ctype.h>
#endif
#include "my_getopt.h"
#ifdef HAVE_PCRE2_H
// use both, 8 and 16 (r2007+)
#  define PCRE2_CODE_UNIT_WIDTH 0
#  include <pcre2.h>
#endif

static const int verbose = 0;

#include "dwg.h"
#include "logging.h"
#include "dwg_api.h"
#include "common.h"
#include "bits.h"

#ifndef HAVE_PCRE2_H
#  define PCRE2_MULTILINE 1
#  define PCRE2_CASELESS 2
#  define PCRE2_EXTENDED 3
#  define PCRE2_NO_AUTO_CAPTURE 4
#  define PCRE2_NO_DOTSTAR_ANCHOR 5
#else
#  define PCRE2_JIT_MATCH_OPTIONS                                             \
    (PCRE2_NO_UTF_CHECK | PCRE2_NOTBOL | PCRE2_NOTEOL | PCRE2_NOTEMPTY        \
     | PCRE2_NOTEMPTY_ATSTART)
#  define PCRE2_JIT_COMPILE_OPTIONS (PCRE2_JIT_COMPLETE)
#endif

char *pattern;
char buf[4096];
// partial to find substrings, not only complete matches
int options
    = PCRE2_MULTILINE | PCRE2_NO_AUTO_CAPTURE | PCRE2_NO_DOTSTAR_ANCHOR;
int opt_count = 0;
int opt_text = 0;
int opt_blocks = 0;
int opt_tables = 0;
int opt_filename = 1;
short numdxf = 0;
short numtype = 0;
static short dxf[10];  // ensure zero-fill
static char *type[10]; // ensure zero-fill

/* the current version per spec block */
// static unsigned int cur_ver = 0;

#ifdef HAVE_PCRE2_H
#  undef USE_MATCH_CONTEXT
/* pcre2_compile */
static pcre2_code_8 *ri8;
static pcre2_match_data_8 *match_data8;
static pcre2_match_context_8 *match_context8 = NULL;
#  ifdef HAVE_PCRE2_16
static pcre2_code_16 *ri16;
static pcre2_match_data_16 *match_data16;
static pcre2_match_context_16 *match_context16 = NULL;
#  endif

#  ifdef USE_MATCH_CONTEXT
static pcre2_jit_stack_8 *jit_stack8 = NULL;
static pcre2_compile_context_8 *compile_context8 = NULL;
#    ifdef HAVE_PCRE2_16
static pcre2_jit_stack_16 *jit_stack16 = NULL;
static pcre2_compile_context_16 *compile_context16 = NULL;
#    endif
#  endif
#endif

static int
usage (void)
{
  printf ("\nUsage: dwggrep [-bcihntx] [--type TYPE] [--dxf NUM] [--help] "
          "pattern *.dwg\n");
  return 1;
}
static int
opt_version (void)
{
  printf ("dwggrep %s\n", PACKAGE_VERSION);
  return 0;
}
static int
help (void)
{
  printf ("\nUsage: dwggrep [OPTIONS]... pattern files\n");
#ifdef HAVE_PCRE2_H
  printf ("Search regex pattern in a list of DWGs.\n\n");
#else
  printf ("Search string (no regex) in a list of DWGs.\n\n");
#endif
  printf ("  -i                        Case-insensitive pattern\n");
#ifdef HAVE_PCRE2_H
  printf ("  -x                        Extended regex pattern\n");
#endif
  printf ("  -c, --count               Print only the count of matched "
          "elements.\n");
  printf ("  -h, --no-filename         Print no filename.\n");
#if 0
  printf("  -R, -r, --recursive       Recursively search subdirectories listed.\n");
#endif
  printf (
      "  -y, --type NAME           Search only NAME entities or objects.\n");
  printf ("  -d, --dxf NUM             Search only DXF group NUM fields.\n");
  printf ("  -t, --text                Search only in TEXT-like entities.\n");
  printf (
      "  -b, --blocks              Search also in all block definitions.\n");
  printf ("  -n, --tables              Search only in table names.\n");
#ifdef HAVE_GETOPT_LONG
  printf ("      --help                Display this help and exit\n");
  printf ("      --version             Output version information and exit\n"
          "\n");
#else
  printf ("  -u                        Display this help and exit\n");
  printf ("  -v                        Output version information and exit\n"
          "\n");
#endif
  printf ("GNU LibreDWG online manual: "
          "<https://www.gnu.org/software/libredwg/>\n");
  return 0;
}

static void
print_match (const int is16, const char *restrict filename,
             const char *restrict entity, const int dxfgroup,
             char *restrict text)
{
  if (is16)
    text = bit_TU_to_utf8 ((BITCODE_TU)text);
  printf ("%s %s %d: %s\n", opt_filename ? filename : "", entity, dxfgroup,
          text);
  if (is16)
    free (text);
}

static int
do_match (const int is16, const char *restrict filename,
          const char *restrict entity, const int dxfgroup, char *restrict text)
{
#ifdef HAVE_PCRE2_H
  int rc;
#  ifdef HAVE_PCRE2_16
  if (is16)
    rc = pcre2_match_16 (ri16, (PCRE2_SPTR16)text, PCRE2_ZERO_TERMINATED, 0,
                         PCRE2_JIT_MATCH_OPTIONS,
                         match_data16,     /* block for storing the result */
                         match_context16); /* disabled */
  else
#  endif
    // already converted to UTF-8 before
    rc = pcre2_match_8 (ri8, (PCRE2_SPTR8)text, PCRE2_ZERO_TERMINATED, 0,
                        PCRE2_JIT_MATCH_OPTIONS,
                        match_data8,     /* block for storing the result */
                        match_context8); /* disabled */
  if (rc >= 0)
    {
      if (!opt_count)
        print_match (is16, filename, entity, dxfgroup, text);
      return 1;
    }
  else if (rc < -2)
    { // not PCRE2_ERROR_NOMATCH nor PCRE2_ERROR_PARTIAL
      pcre2_get_error_message_8 (rc, (PCRE2_UCHAR8 *)buf, 4096);
      LOG_WARN ("pcre2 match error %s with %s", buf, pattern);
    }
  return 0;

#else

  if (options & PCRE2_CASELESS)
    {
#  ifndef HAVE_STRCASESTR
      size_t i, len;
      size_t dmax;
      char *dest = text;
      size_t dlen = dmax = strlen (text);
      char *src = pattern;
      size_t slen = strlen (pattern);

      while (*dest && dmax)
        {
          i = 0;
          len = slen;
          dlen = dmax;
          while (dest[i] && dlen)
            {
              if (toupper ((unsigned char)dest[i])
                  != toupper ((unsigned char)src[i]))
                {
                  break;
                }
              /* move to the next char */
              i++;
              len--;
              dlen--;

              if (src[i] == '\0' || !len)
                {
                  if (!opt_count)
                    print_match (0, filename, entity, dxfgroup, text);
                  return 1;
                }
            }
          dest++;
          dmax--;
        }
#  else
      if (strcasestr (text, pattern))
        {
          if (!opt_count)
            print_match (0, filename, entity, dxfgroup, text);
          return 1;
        }
#  endif
    }
  else
    {
      if (strstr (text, pattern))
        {
          if (!opt_count)
            print_match (0, filename, entity, dxfgroup, text);
          return 1;
        }
    }
  return 0;
#endif
}

// check matching dxfgroup first to avoid costly utf8 conversions
#define MATCH_DXF(type, ENTITY, text_field, dxfgroup)                         \
  if (numdxf)                                                                 \
    {                                                                         \
      int dxfok = 0;                                                          \
      for (int _i = 0; _i < numdxf; _i++)                                     \
        {                                                                     \
          if (dxf[_i] == dxfgroup)                                            \
            {                                                                 \
              dxfok = 1;                                                      \
              break;                                                          \
            }                                                                 \
        }                                                                     \
      if (dxfok)                                                              \
        {                                                                     \
          MATCH_TYPE (type, ENTITY, text_field, dxfgroup);                    \
        }                                                                     \
    }                                                                         \
  else                                                                        \
    {                                                                         \
      MATCH_TYPE (type, ENTITY, text_field, dxfgroup);                        \
    }

// 8bit only
#define MATCH_NO16(type, ENTITY, text_field, dxfgroup)                        \
  text = (char *)obj->tio.type->tio.ENTITY->text_field;                       \
  if (text && numdxf)                                                         \
    {                                                                         \
      int dxfok = 0;                                                          \
      for (int i = 0; i < numdxf; i++)                                        \
        {                                                                     \
          if (dxf[i] == dxfgroup)                                             \
            {                                                                 \
              dxfok = 1;                                                      \
              break;                                                          \
            }                                                                 \
        }                                                                     \
      if (dxfok)                                                              \
        {                                                                     \
          found += do_match (0, filename, #ENTITY, dxfgroup, text);           \
        }                                                                     \
    }                                                                         \
  else if (text)                                                              \
    {                                                                         \
      found += do_match (0, filename, #ENTITY, dxfgroup, text);               \
    }

#ifdef HAVE_PCRE2_16
#  define MATCH_TYPE(type, ENTITY, text_field, dxfgroup)                      \
    text = (char *)obj->tio.type->tio.ENTITY->text_field;                     \
    if (text)                                                                 \
      found += do_match (obj->parent->header.version >= R_2007, filename,     \
                       #ENTITY, dxfgroup, text)
#else
#  define MATCH_TYPE(type, ENTITY, text_field, dxfgroup)                      \
    text = (char *)obj->tio.type->tio.ENTITY->text_field;                     \
    if (text)                                                                 \
      {                                                                       \
        if (obj->parent->header.version >= R_2007)                            \
          text = bit_TU_to_utf8 ((BITCODE_TU)text);                           \
        found += do_match (obj->parent->header.version >= R_2007, filename,   \
                           #ENTITY, dxfgroup, text);                          \
        if (obj->parent->header.version >= R_2007)                            \
          free (text);                                                        \
      }
#endif

#define MATCH_ENTITY(ENTITY, text_field, dxf)                                 \
  MATCH_DXF (entity, ENTITY, text_field, dxf)
#define MATCH_OBJECT(ENTITY, text_field, dxf)                                 \
  MATCH_DXF (object, ENTITY, text_field, dxf)
#define MATCH_TABLE(ENTITY, handle, TABLE, dxf)                               \
  {                                                                           \
  }

static int
match_TEXT (const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  MATCH_ENTITY (TEXT, text_value, 1);
  if (!opt_text)
    {
      MATCH_TABLE (TEXT, style, STYLE, 7);
    }
  return found;
}

static int
match_ARCALIGNEDTEXT (const char *restrict filename,
                      const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  MATCH_ENTITY (ARCALIGNEDTEXT, text_value, 1);
  if (!opt_text)
    {
      // ignore the various sizes stored as text
      MATCH_ENTITY (ARCALIGNEDTEXT, style, 7);
    }
  return found;
}

static int
match_ATTRIB (const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  // printf("--ATTRIB %lX %s\n", obj->handle.value, filename);
  MATCH_ENTITY (ATTRIB, text_value, 1);
  MATCH_ENTITY (ATTRIB, tag, 2);
  if (!opt_text)
    {
      MATCH_TABLE (ATTRIB, style, STYLE, 7);
    }
  return found;
}

static int
match_ATTDEF (const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  MATCH_ENTITY (ATTDEF, default_value, 1);
  MATCH_ENTITY (ATTDEF, tag, 2);
  MATCH_ENTITY (ATTDEF, prompt, 3);
  return found;
}

static int
match_MTEXT (const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  MATCH_ENTITY (MTEXT, text, 1);
  return found;
}

static int
match_BLOCK (const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  MATCH_ENTITY (BLOCK, name, 2);
  return found;
}

static int
match_DIMENSION (const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  // int is16 = obj->parent->header.version >= R_2007;

  text = obj->tio.entity->tio.DIMENSION_ORDINATE->user_text;
  if (text)
    found += do_match (0, filename, "DIMENSION", 1, text);
  return found;
}

static int
match_VIEWPORT (const char *restrict filename, const Dwg_Object *restrict obj)
{
  int found = 0;
  char *text;
  MATCH_ENTITY (VIEWPORT, style_sheet, 1);
  return found;
}

static int
match_MULTILEADER (const char *restrict filename,
                   const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  const Dwg_Entity_MULTILEADER *_obj = obj->tio.entity->tio.MULTILEADER;
  if (_obj->ctx.has_content_txt)
    {
      MATCH_ENTITY (MULTILEADER, ctx.content.txt.default_text, 304);
    }
  // SUB_FIELD_T (blocklabels[rcount1],label_text, 302);
  return found;
}

static int
match_3DSOLID (const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text = NULL;
  int found = 0;
  Dwg_Entity_3DSOLID *_obj;

  if (!obj || !obj->tio.entity)
    return 0;
  _obj = obj->tio.entity->tio._3DSOLID;
  if (!_obj)
    return 0;
  if (_obj->acis_data)
    {
      MATCH_NO16 (entity, _3DSOLID, acis_data, 1);
      // MATCH_ENTITY (_3DSOLID, acis_data, 1);
      // found += do_match(0, filename, "3DSOLID", 1, (char*)_obj->acis_data);
    }
  /*
  if (!_obj->encr_sat_data) return 0;
  for (BITCODE_BL j=0; j<_obj->num_blocks; j++)
    {
      //text = _obj->encr_sat_data[j];
      //if (text)
      //  found += do_match(0, filename, "3DSOLID", 301, text);
      MATCH_NO16 (entity, _3DSOLID, encr_sat_data[j], 301);
    }
  */
  return found;
}

static int
match_DICTIONARY (const char *restrict filename,
                  const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  BITCODE_BL i;
  Dwg_Object_DICTIONARY *_obj = obj->tio.object->tio.DICTIONARY;

  for (i = 0; i < _obj->numitems; i++)
    {
      MATCH_OBJECT (DICTIONARY, texts[i], 3);
    }
  return found;
}

static int
match_STYLE (const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  MATCH_OBJECT (STYLE, name, 2);
  if (!opt_tables)
    {
      MATCH_OBJECT (STYLE, font_file, 3);
      MATCH_OBJECT (STYLE, bigfont_file, 4);
    }
  return found;
}

static int
match_LTYPE (const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  MATCH_OBJECT (LTYPE, name, 2);
  MATCH_OBJECT (LTYPE, description, 3);
  if (!opt_tables)
    {
      MATCH_OBJECT (LTYPE, strings_area, 3);
    }
  return found;
}
static int
match_LAYER (const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  MATCH_OBJECT (LAYER, name, 2);
  return found;
}
static int
match_VIEW (const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  MATCH_OBJECT (VIEW, name, 2);
  return found;
}
static int
match_VPORT (const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  MATCH_OBJECT (VPORT, name, 2);
  return found;
}
static int
match_UCS (const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  MATCH_OBJECT (UCS, name, 2);
  return found;
}
static int
match_VX_TABLE_RECORD (const char *restrict filename,
                       const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  MATCH_OBJECT (VX_TABLE_RECORD, name, 2);
  return found;
}

static int
match_DIMSTYLE (const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  MATCH_OBJECT (DIMSTYLE, name, 2);
  if (!opt_tables)
    {
      MATCH_OBJECT (DIMSTYLE, DIMPOST, 3);
      MATCH_OBJECT (DIMSTYLE, DIMAPOST, 4);
      MATCH_OBJECT (DIMSTYLE, DIMBLK_T, 5);
      MATCH_OBJECT (DIMSTYLE, DIMBLK1_T, 6);
      MATCH_OBJECT (DIMSTYLE, DIMBLK2_T, 7);
      MATCH_OBJECT (DIMSTYLE, DIMMZS, 0);
      MATCH_OBJECT (DIMSTYLE, DIMALTMZS, 0);
    }
  return found;
}

static int
match_GROUP (const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  MATCH_OBJECT (GROUP, name, 3);
  return found;
}

static int
match_MLINESTYLE (const char *restrict filename,
                  const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  MATCH_OBJECT (MLINESTYLE, name, 2);
  MATCH_OBJECT (MLINESTYLE, description, 3);
  return found;
}

static int
match_DICTIONARYVAR (const char *restrict filename,
                     const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  MATCH_OBJECT (DICTIONARYVAR, strvalue, 1);
  return found;
}

static int
match_HATCH (const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  MATCH_ENTITY (HATCH, name, 2);
  MATCH_ENTITY (HATCH, gradient_name, 470);
  return found;
}
static int
match_TOLERANCE (const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  MATCH_ENTITY (TOLERANCE, text_value, 1);
  return found;
}

static int
match_IMAGEDEF (const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  MATCH_OBJECT (IMAGEDEF, file_path, 1);
  return found;
}

static int
match_SCALE (const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  // const Dwg_Object_SCALE *_obj = obj->tio.object->tio.SCALE;
  MATCH_OBJECT (SCALE, name, 1);
  return found;
}

static int
match_LAYER_INDEX (const char *restrict filename,
                   const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  const Dwg_Object_LAYER_INDEX *_obj = obj->tio.object->tio.LAYER_INDEX;
  for (BITCODE_BL i = 0; i < _obj->num_entries; i++)
    {
      MATCH_OBJECT (LAYER_INDEX, entries[i].name, 8);
    }
  return found;
}

static int
match_LAYOUT (const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  BITCODE_BL i;
  const Dwg_Object_LAYOUT *_obj = obj->tio.object->tio.LAYOUT;

  MATCH_OBJECT (LAYOUT, plotsettings.printer_cfg_file, 1);
  MATCH_OBJECT (LAYOUT, plotsettings.paper_size, 2);
  MATCH_OBJECT (LAYOUT, plotsettings.canonical_media_name, 4);
  MATCH_TABLE (LAYOUT, plotsettings.plotview, VIEW, 6);
  MATCH_OBJECT (LAYOUT, plotsettings.plotview_name, 6);
  MATCH_OBJECT (LAYOUT, plotsettings.stylesheet, 7);

  MATCH_OBJECT (LAYOUT, layout_name, 1);
  MATCH_TABLE (LAYOUT, block_header, BLOCK, 330);
  MATCH_TABLE (LAYOUT, active_viewport, VIEWPORT, 331);
  MATCH_TABLE (LAYOUT, shadeplot, VISUALSTYLE, 333);
  MATCH_TABLE (LAYOUT, base_ucs, UCS, 346);
  MATCH_TABLE (LAYOUT, named_ucs, UCS, 345);
  for (i = 0; i < _obj->num_viewports; i++)
    {
      MATCH_TABLE (LAYOUT, viewports[i], VPORT, 0);
    }
  return found;
}

static int
match_FIELD (const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  BITCODE_BL i;
  const Dwg_Object_FIELD *_obj = obj->tio.object->tio.FIELD;

  MATCH_OBJECT (FIELD, format, 4);
  MATCH_OBJECT (FIELD, evaluation_error_msg, 300);
  MATCH_OBJECT (FIELD, value.format_string, 300);
  MATCH_OBJECT (FIELD, value.value_string, 300);
  MATCH_OBJECT (FIELD, value_string, 301);
  for (i = 0; i < _obj->num_childval; i++)
    {
      MATCH_OBJECT (FIELD, childval[i].key, 6);
      MATCH_OBJECT (FIELD, childval[i].value.format_string, 300);
      MATCH_OBJECT (FIELD, childval[i].value.value_string, 302);
    }
  return found;
}

static int
match_TABLE (const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  BITCODE_BL i, j;
  const Dwg_Entity_TABLE *_obj = obj->tio.entity->tio.TABLE;

  for (i = 0; i < _obj->num_cells; i++)
    {
      if (_obj->cells[i].type == 1)
        {
          MATCH_ENTITY (TABLE, cells[i].text_value, 1);
        }
      else if (_obj->cells[i].type == 2
               && _obj->cells[i].additional_data_flag == 1
               && _obj->cells[i].num_attr_defs)
        {
          for (j = 0; j < _obj->cells[i].num_attr_defs; j++)
            {
              MATCH_ENTITY (TABLE, cells[i].attr_defs[j].text, 300);
            }
        }
    }
  return found;
}

static int
match_TABLECONTENT (const char *restrict filename,
                    const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  BITCODE_BL i, j, k;
  const Dwg_Object_TABLECONTENT *_obj = obj->tio.object->tio.TABLECONTENT;

  MATCH_OBJECT (TABLECONTENT, ldata.name, 1);
  MATCH_OBJECT (TABLECONTENT, ldata.description, 300);
  for (i = 0; i < _obj->tdata.num_cols; i++)
    {
      MATCH_OBJECT (TABLECONTENT, tdata.cols[i].name, 300);
    }
  for (i = 0; i < _obj->tdata.num_rows; i++)
    {
      for (j = 0; j < _obj->tdata.rows[i].num_cells; j++)
        {
          MATCH_OBJECT (TABLECONTENT, tdata.rows[i].cells[j].tooltip, 300);
          for (k = 0; k < _obj->tdata.rows[i].cells[j].num_customdata_items;
               k++)
            {
#define _custom tdata.rows[i].cells[j].customdata_items[k]
              MATCH_OBJECT (TABLECONTENT, _custom.name, 300);
              if (_obj->_custom.value.data_type == 4)
                {
                  MATCH_OBJECT (TABLECONTENT, _custom.value.data_string, 302);
                }
#undef _custom
            }
          for (k = 0; k < _obj->tdata.rows[i].cells[j].num_cell_contents; k++)
            {
#define _content tdata.rows[i].cells[j].cell_contents[k]
              if (_obj->_content.type == 1
                  && _obj->_content.value.data_type == 4)
                {
                  MATCH_OBJECT (TABLECONTENT, _content.value.data_string, 302);
                }
#undef _content
            }
        }
    }
  return found;
}

static int
match_GEODATA (const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  // const Dwg_Object_GEODATA *_obj = obj->tio.object->tio.GEODATA;

  MATCH_OBJECT (GEODATA, coord_system_def, 0);
  MATCH_OBJECT (GEODATA, geo_rss_tag, 302);
  MATCH_OBJECT (GEODATA, observation_from_tag, 305);
  MATCH_OBJECT (GEODATA, observation_to_tag, 306);
  MATCH_OBJECT (GEODATA, observation_coverage_tag, 0);
  // obsolete
  MATCH_OBJECT (GEODATA, coord_system_datum, 0);
  MATCH_OBJECT (GEODATA, coord_system_wkt, 0);
  return found;
}

static int
match_GEOPOSITIONMARKER (const char *restrict filename,
                         const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  // const Dwg_Entity_GEOPOSITIONMARKER *_obj =
  // obj->tio.entity->tio.GEOPOSITIONMARKER;

  MATCH_ENTITY (GEOPOSITIONMARKER, notes, 1);
  // if enabled
  // MATCH_ENTITY (GEOPOSITIONMARKER, mtext->tio.entity->tio.MTEXT->text, 3);
  return found;
}

static int
match_UNDERLAYDEFINITION (const char *restrict filename,
                          const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  // const Dwg_Object_PDFDEFINITION *_obj = obj->tio.object->tio.PDFDEFINITION;

  MATCH_OBJECT (PDFDEFINITION, filename, 1);
  MATCH_OBJECT (PDFDEFINITION, name, 2);
  return found;
}

static int
match_VISUALSTYLE (const char *restrict filename,
                   const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  // const Dwg_Object_VISUALSTYLE *_obj = obj->tio.object->tio.VISUALSTYLE;
  MATCH_OBJECT (VISUALSTYLE, description, 1);
  return found;
}

static int
match_TABLESTYLE (const char *restrict filename,
                  const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  // const Dwg_Object_TABLESTYLE *_obj = obj->tio.object->tio.TABLESTYLE;
  MATCH_OBJECT (TABLESTYLE, name, 2);
  return found;
}
static int
match_LIGHT (const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  // const Dwg_Entity_LIGHT *_obj = obj->tio.entity->tio.LIGHT;
  MATCH_ENTITY (LIGHT, name, 1);
  // MATCH_ENTITY (LIGHT, web_file, 1);
  return found;
}

static int
match_SUNSTUDY (const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  // const Dwg_Object_SUNSTUDY *_obj = obj->tio.object->tio.SUNSTUDY;
  MATCH_OBJECT (SUNSTUDY, setup_name, 1);
  MATCH_OBJECT (SUNSTUDY, description, 2);
  MATCH_OBJECT (SUNSTUDY, sheet_set_name, 3);
  return found;
}

static int
match_LIGHTLIST (const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  const Dwg_Object_LIGHTLIST *_obj = obj->tio.object->tio.LIGHTLIST;

  for (BITCODE_BL i = 0; i < _obj->num_lights; i++)
    {
      MATCH_OBJECT (LIGHTLIST, lights[i].name, 1);
    }
  return found;
}

static int
match_DBCOLOR (const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  // const Dwg_Object_DBCOLOR *_obj = obj->tio.object->tio.DBCOLOR;
  MATCH_OBJECT (DBCOLOR, color.name, 430);
  MATCH_OBJECT (DBCOLOR, color.book_name, 430);
  return found;
}

static int
match_MATERIAL (const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  // const Dwg_Object_MATERIAL *_obj = obj->tio.object->tio.MATERIAL;
  MATCH_OBJECT (MATERIAL, name, 1);
  MATCH_OBJECT (MATERIAL, description, 2);
  MATCH_OBJECT (MATERIAL, diffusemap.filename, 3);
  MATCH_OBJECT (MATERIAL, specularmap.filename, 4);
  MATCH_OBJECT (MATERIAL, reflectionmap.filename, 6);
  MATCH_OBJECT (MATERIAL, opacitymap.filename, 7);
  MATCH_OBJECT (MATERIAL, bumpmap.filename, 8);
  MATCH_OBJECT (MATERIAL, refractionmap.filename, 9);
  // MATCH_OBJECT (MATERIAL, normalmap.filename, 3);
  // MATCH_OBJECT (MATERIAL, genprocname, 300);
  // MATCH_OBJECT (MATERIAL, genprocvaltext, 301);
  // MATCH_OBJECT (MATERIAL, genprocvalcolorname, 430);
  return found;
}

static int
match_PLOTSETTINGS (const char *restrict filename,
                    const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  // const Dwg_Object_PLOTSETTINGS *_obj = obj->tio.object->tio.PLOTSETTINGS;
  MATCH_OBJECT (PLOTSETTINGS, printer_cfg_file, 1);
  MATCH_OBJECT (PLOTSETTINGS, paper_size, 2);
  MATCH_OBJECT (PLOTSETTINGS, canonical_media_name, 4);
  MATCH_OBJECT (PLOTSETTINGS, plotview_name, 6);
  MATCH_TABLE (PLOTSETTINGS, VIEW, plotview, 6);
  MATCH_OBJECT (PLOTSETTINGS, stylesheet, 7);
  MATCH_TABLE (PLOTSETTINGS, VISUALSTYLE, shadeplot, 333);
  return found;
}
static int
match_DIMASSOC (const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  const Dwg_Object_DIMASSOC *_obj = obj->tio.object->tio.DIMASSOC;
  for (BITCODE_BL i = 0; i < 4; i++)
    {
      if (_obj->ref[i].classname)
        {
          MATCH_OBJECT (DIMASSOC, ref[i].classname, 0);
        }
    }
  return found;
}

static int
match_ASSOCOSNAPPOINTREFACTIONPARAM (const char *restrict filename,
                                     const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  MATCH_OBJECT (ASSOCOSNAPPOINTREFACTIONPARAM, name, 1);
  return found;
}
static int
match_ASSOCACTIONPARAM (const char *restrict filename,
                        const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  MATCH_OBJECT (ASSOCACTIONPARAM, name, 1);
  return found;
}
static int
match_ASSOCEDGEACTIONPARAM (const char *restrict filename,
                            const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  MATCH_OBJECT (ASSOCEDGEACTIONPARAM, name, 1);
  return found;
}
static int
match_ASSOCFACEACTIONPARAM (const char *restrict filename,
                            const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  MATCH_OBJECT (ASSOCFACEACTIONPARAM, name, 1);
  return found;
}
static int
match_ASSOCOBJECTACTIONPARAM (const char *restrict filename,
                              const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  MATCH_OBJECT (ASSOCOBJECTACTIONPARAM, name, 1);
  return found;
}
static int
match_ASSOCPATHACTIONPARAM (const char *restrict filename,
                            const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  MATCH_OBJECT (ASSOCPATHACTIONPARAM, name, 1);
  return found;
}
static int
match_ASSOCVERTEXACTIONPARAM (const char *restrict filename,
                              const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  MATCH_OBJECT (ASSOCVERTEXACTIONPARAM, name, 1);
  return found;
}

// TODO match on its subclasses which holds the text:
//  ASSOCVARIABLE, EvalVariant

#define MATCH_AcDbAssocParamBasedActionBody(_type)                            \
  for (unsigned i = 0; i < _obj->pab.num_values; i++)                         \
    {                                                                         \
      MATCH_OBJECT (_type, pab.values[i].name, 1);                            \
      for (unsigned j = 0; j < _obj->pab.values[i].num_vars; j++)             \
        {                                                                     \
          int _dxf = _obj->pab.values[i].vars[j].value.code;                  \
          if (dwg_resbuf_value_type (_dxf) == DWG_VT_STRING)                  \
            {                                                                 \
              MATCH_OBJECT (_type, pab.values[i].vars[j].value.u.text, _dxf); \
            }                                                                 \
        }                                                                     \
    }

static int
match_ASSOCMLEADERACTIONBODY (const char *restrict filename,
                              const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  const Dwg_Object_ASSOCMLEADERACTIONBODY *_obj
      = obj->tio.object->tio.ASSOCMLEADERACTIONBODY;
  MATCH_AcDbAssocParamBasedActionBody (ASSOCMLEADERACTIONBODY) return found;
}
static int
match_ASSOC3POINTANGULARDIMACTIONBODY (const char *restrict filename,
                                       const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  const Dwg_Object_ASSOC3POINTANGULARDIMACTIONBODY *_obj
      = obj->tio.object->tio.ASSOC3POINTANGULARDIMACTIONBODY;
  MATCH_AcDbAssocParamBasedActionBody (
      ASSOC3POINTANGULARDIMACTIONBODY) return found;
}
static int
match_ASSOCALIGNEDDIMACTIONBODY (const char *restrict filename,
                                 const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  const Dwg_Object_ASSOCALIGNEDDIMACTIONBODY *_obj
      = obj->tio.object->tio.ASSOCALIGNEDDIMACTIONBODY;
  MATCH_AcDbAssocParamBasedActionBody (ASSOCALIGNEDDIMACTIONBODY) return found;
}

static int
match_ASSOCORDINATEDIMACTIONBODY (const char *restrict filename,
                                  const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  const Dwg_Object_ASSOCORDINATEDIMACTIONBODY *_obj
      = obj->tio.object->tio.ASSOCORDINATEDIMACTIONBODY;
  MATCH_AcDbAssocParamBasedActionBody (
      ASSOCORDINATEDIMACTIONBODY) return found;
}

static int
match_ASSOCROTATEDDIMACTIONBODY (const char *restrict filename,
                                 const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  const Dwg_Object_ASSOCROTATEDDIMACTIONBODY *_obj
      = obj->tio.object->tio.ASSOCROTATEDDIMACTIONBODY;
  MATCH_AcDbAssocParamBasedActionBody (ASSOCROTATEDDIMACTIONBODY) return found;
}

static int
match_ASSOCPATCHSURFACEACTIONBODY (const char *restrict filename,
                                   const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  const Dwg_Object_ASSOCPATCHSURFACEACTIONBODY *_obj
      = obj->tio.object->tio.ASSOCPATCHSURFACEACTIONBODY;
  MATCH_AcDbAssocParamBasedActionBody (
      ASSOCPATCHSURFACEACTIONBODY) return found;
}
static int
match_ASSOCPLANESURFACEACTIONBODY (const char *restrict filename,
                                   const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  const Dwg_Object_ASSOCPLANESURFACEACTIONBODY *_obj
      = obj->tio.object->tio.ASSOCPLANESURFACEACTIONBODY;
  MATCH_AcDbAssocParamBasedActionBody (
      ASSOCPLANESURFACEACTIONBODY) return found;
}
static int
match_ASSOCEXTENDSURFACEACTIONBODY (const char *restrict filename,
                                    const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  const Dwg_Object_ASSOCEXTENDSURFACEACTIONBODY *_obj
      = obj->tio.object->tio.ASSOCEXTENDSURFACEACTIONBODY;
  MATCH_AcDbAssocParamBasedActionBody (
      ASSOCEXTENDSURFACEACTIONBODY) return found;
}
static int
match_ASSOCEXTRUDEDSURFACEACTIONBODY (const char *restrict filename,
                                      const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  const Dwg_Object_ASSOCEXTRUDEDSURFACEACTIONBODY *_obj
      = obj->tio.object->tio.ASSOCEXTRUDEDSURFACEACTIONBODY;
  MATCH_AcDbAssocParamBasedActionBody (
      ASSOCEXTRUDEDSURFACEACTIONBODY) return found;
}
static int
match_ASSOCFILLETSURFACEACTIONBODY (const char *restrict filename,
                                    const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  const Dwg_Object_ASSOCFILLETSURFACEACTIONBODY *_obj
      = obj->tio.object->tio.ASSOCFILLETSURFACEACTIONBODY;
  MATCH_AcDbAssocParamBasedActionBody (
      ASSOCFILLETSURFACEACTIONBODY) return found;
}
static int
match_ASSOCLOFTEDSURFACEACTIONBODY (const char *restrict filename,
                                    const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  const Dwg_Object_ASSOCLOFTEDSURFACEACTIONBODY *_obj
      = obj->tio.object->tio.ASSOCLOFTEDSURFACEACTIONBODY;
  MATCH_AcDbAssocParamBasedActionBody (
      ASSOCLOFTEDSURFACEACTIONBODY) return found;
}
static int
match_ASSOCNETWORKSURFACEACTIONBODY (const char *restrict filename,
                                     const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  const Dwg_Object_ASSOCNETWORKSURFACEACTIONBODY *_obj
      = obj->tio.object->tio.ASSOCNETWORKSURFACEACTIONBODY;
  MATCH_AcDbAssocParamBasedActionBody (
      ASSOCNETWORKSURFACEACTIONBODY) return found;
}
static int
match_ASSOCOFFSETSURFACEACTIONBODY (const char *restrict filename,
                                    const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  const Dwg_Object_ASSOCOFFSETSURFACEACTIONBODY *_obj
      = obj->tio.object->tio.ASSOCOFFSETSURFACEACTIONBODY;
  MATCH_AcDbAssocParamBasedActionBody (
      ASSOCOFFSETSURFACEACTIONBODY) return found;
}
static int
match_ASSOCREVOLVEDSURFACEACTIONBODY (const char *restrict filename,
                                      const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  const Dwg_Object_ASSOCREVOLVEDSURFACEACTIONBODY *_obj
      = obj->tio.object->tio.ASSOCREVOLVEDSURFACEACTIONBODY;
  MATCH_AcDbAssocParamBasedActionBody (
      ASSOCREVOLVEDSURFACEACTIONBODY) return found;
}
static int
match_ASSOCSWEPTSURFACEACTIONBODY (const char *restrict filename,
                                   const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  const Dwg_Object_ASSOCSWEPTSURFACEACTIONBODY *_obj
      = obj->tio.object->tio.ASSOCSWEPTSURFACEACTIONBODY;
  MATCH_AcDbAssocParamBasedActionBody (
      ASSOCSWEPTSURFACEACTIONBODY) return found;
}
static int
match_ASSOCTRIMSURFACEACTIONBODY (const char *restrict filename,
                                  const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  const Dwg_Object_ASSOCTRIMSURFACEACTIONBODY *_obj
      = obj->tio.object->tio.ASSOCTRIMSURFACEACTIONBODY;
  MATCH_AcDbAssocParamBasedActionBody (
      ASSOCTRIMSURFACEACTIONBODY) return found;
}

static int
match_BLOCKPARAMDEPENDENCYBODY (const char *restrict filename,
                                const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  MATCH_OBJECT (BLOCKPARAMDEPENDENCYBODY, name, 1);
  return found;
}
static int
match_BLOCKMOVEACTION (const char *restrict filename,
                       const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  MATCH_OBJECT (BLOCKMOVEACTION, conn_pts[0].name, 301);
  MATCH_OBJECT (BLOCKMOVEACTION, conn_pts[1].name, 302);
  return found;
}
static int
match_BLOCKSTRETCHACTION (const char *restrict filename,
                          const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  MATCH_OBJECT (BLOCKSTRETCHACTION, conn_pts[0].name, 301);
  MATCH_OBJECT (BLOCKSTRETCHACTION, conn_pts[1].name, 302);
  return found;
}
static int
match_BLOCKROTATEACTION (const char *restrict filename,
                         const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  MATCH_OBJECT (BLOCKROTATEACTION, conn_pts[0].name, 301);
  MATCH_OBJECT (BLOCKROTATEACTION, conn_pts[1].name, 302);
  MATCH_OBJECT (BLOCKROTATEACTION, conn_pts[2].name, 303);
  return found;
}
static int
match_BLOCKVISIBILITYGRIP (const char *restrict filename,
                           const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  Dwg_Object_BLOCKVISIBILITYGRIP *_obj
      = obj->tio.object->tio.BLOCKVISIBILITYGRIP;

  if (_obj->evalexpr.value_code == 1)
    {
      MATCH_OBJECT (BLOCKVISIBILITYGRIP, evalexpr.value.text1, 1);
    }
  MATCH_OBJECT (BLOCKVISIBILITYGRIP, name, 1);
  return found;
}
static int
match_BLOCKGRIPLOCATIONCOMPONENT (const char *restrict filename,
                                  const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  Dwg_Object_BLOCKGRIPLOCATIONCOMPONENT *_obj
      = obj->tio.object->tio.BLOCKGRIPLOCATIONCOMPONENT;

  if (_obj->evalexpr.value_code == 1)
    {
      MATCH_OBJECT (BLOCKGRIPLOCATIONCOMPONENT, evalexpr.value.text1, 1);
    }
  MATCH_OBJECT (BLOCKGRIPLOCATIONCOMPONENT, grip_expr, 91);
  return found;
}
static int
match_BLOCKBASEPOINTPARAMETER (const char *restrict filename,
                               const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  Dwg_Object_BLOCKBASEPOINTPARAMETER *_obj
      = obj->tio.object->tio.BLOCKBASEPOINTPARAMETER;

  if (_obj->evalexpr.value_code == 1)
    {
      MATCH_OBJECT (BLOCKBASEPOINTPARAMETER, evalexpr.value.text1, 1);
    }
  MATCH_OBJECT (BLOCKBASEPOINTPARAMETER, name, 1);
  for (unsigned i = 0; i < _obj->prop1.num_connections; i++)
    {
      MATCH_OBJECT (BLOCKROTATIONPARAMETER, prop1.connections[i].name, 301);
    }
  for (unsigned i = 0; i < _obj->prop2.num_connections; i++)
    {
      MATCH_OBJECT (BLOCKROTATIONPARAMETER, prop2.connections[i].name, 302);
    }
  return found;
}
static int
match_BLOCKLINEARPARAMETER (const char *restrict filename,
                            const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  Dwg_Object_BLOCKLINEARPARAMETER *_obj
      = obj->tio.object->tio.BLOCKLINEARPARAMETER;

  if (_obj->evalexpr.value_code == 1)
    {
      MATCH_OBJECT (BLOCKLINEARPARAMETER, evalexpr.value.text1, 1);
    }
  MATCH_OBJECT (BLOCKLINEARPARAMETER, name, 1);
  for (unsigned i = 0; i < _obj->prop1.num_connections; i++)
    {
      MATCH_OBJECT (BLOCKLINEARPARAMETER, prop1.connections[i].name, 301);
    }
  for (unsigned i = 0; i < _obj->prop2.num_connections; i++)
    {
      MATCH_OBJECT (BLOCKLINEARPARAMETER, prop2.connections[i].name, 302);
    }
  for (unsigned i = 0; i < _obj->prop3.num_connections; i++)
    {
      MATCH_OBJECT (BLOCKLINEARPARAMETER, prop3.connections[i].name, 303);
    }
  for (unsigned i = 0; i < _obj->prop4.num_connections; i++)
    {
      MATCH_OBJECT (BLOCKLINEARPARAMETER, prop4.connections[i].name, 304);
    }
  MATCH_OBJECT (BLOCKLINEARPARAMETER, distance_name, 305);
  MATCH_OBJECT (BLOCKLINEARPARAMETER, distance_desc, 306);
  MATCH_OBJECT (BLOCKLINEARPARAMETER, value_set.desc, 307);
  return found;
}
static int
match_BLOCKFLIPPARAMETER (const char *restrict filename,
                          const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  Dwg_Object_BLOCKFLIPPARAMETER *_obj
      = obj->tio.object->tio.BLOCKFLIPPARAMETER;

  if (_obj->evalexpr.value_code == 1)
    {
      MATCH_OBJECT (BLOCKFLIPPARAMETER, evalexpr.value.text1, 1);
    }
  MATCH_OBJECT (BLOCKFLIPPARAMETER, name, 1);
  for (unsigned i = 0; i < _obj->prop1.num_connections; i++)
    {
      MATCH_OBJECT (BLOCKFLIPPARAMETER, prop1.connections[i].name, 301);
    }
  for (unsigned i = 0; i < _obj->prop2.num_connections; i++)
    {
      MATCH_OBJECT (BLOCKFLIPPARAMETER, prop2.connections[i].name, 302);
    }
  for (unsigned i = 0; i < _obj->prop3.num_connections; i++)
    {
      MATCH_OBJECT (BLOCKFLIPPARAMETER, prop3.connections[i].name, 303);
    }
  for (unsigned i = 0; i < _obj->prop4.num_connections; i++)
    {
      MATCH_OBJECT (BLOCKFLIPPARAMETER, prop4.connections[i].name, 304);
    }
  MATCH_OBJECT (BLOCKFLIPPARAMETER, flip_label, 305);
  MATCH_OBJECT (BLOCKFLIPPARAMETER, flip_label_desc, 306);
  MATCH_OBJECT (BLOCKFLIPPARAMETER, base_state_label, 307);
  MATCH_OBJECT (BLOCKFLIPPARAMETER, flipped_state_label, 308);
  MATCH_OBJECT (BLOCKFLIPPARAMETER, tooltip, 309);
  return found;
}
static int
match_BLOCKROTATIONPARAMETER (const char *restrict filename,
                              const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  Dwg_Object_BLOCKROTATIONPARAMETER *_obj
      = obj->tio.object->tio.BLOCKROTATIONPARAMETER;

  if (_obj->evalexpr.value_code == 1)
    {
      MATCH_OBJECT (BLOCKROTATIONPARAMETER, evalexpr.value.text1, 1);
    }
  MATCH_OBJECT (BLOCKROTATIONPARAMETER, name, 1);
  for (unsigned i = 0; i < _obj->prop1.num_connections; i++)
    {
      MATCH_OBJECT (BLOCKROTATIONPARAMETER, prop1.connections[i].name, 301);
    }
  for (unsigned i = 0; i < _obj->prop2.num_connections; i++)
    {
      MATCH_OBJECT (BLOCKROTATIONPARAMETER, prop2.connections[i].name, 302);
    }
  for (unsigned i = 0; i < _obj->prop3.num_connections; i++)
    {
      MATCH_OBJECT (BLOCKROTATIONPARAMETER, prop3.connections[i].name, 303);
    }
  for (unsigned i = 0; i < _obj->prop4.num_connections; i++)
    {
      MATCH_OBJECT (BLOCKROTATIONPARAMETER, prop4.connections[i].name, 304);
    }
  MATCH_OBJECT (BLOCKROTATIONPARAMETER, angle_name, 305);
  MATCH_OBJECT (BLOCKROTATIONPARAMETER, angle_desc, 306);
  MATCH_OBJECT (BLOCKROTATIONPARAMETER, angle_value_set.desc, 307);
  return found;
}
static int
match_BLOCKXYPARAMETER (const char *restrict filename,
                        const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  Dwg_Object_BLOCKXYPARAMETER *_obj = obj->tio.object->tio.BLOCKXYPARAMETER;

  if (_obj->evalexpr.value_code == 1)
    {
      MATCH_OBJECT (BLOCKXYPARAMETER, evalexpr.value.text1, 1);
    }
  MATCH_OBJECT (BLOCKXYPARAMETER, name, 1);
  for (unsigned i = 0; i < _obj->prop1.num_connections; i++)
    {
      MATCH_OBJECT (BLOCKXYPARAMETER, prop1.connections[i].name, 301);
    }
  for (unsigned i = 0; i < _obj->prop2.num_connections; i++)
    {
      MATCH_OBJECT (BLOCKXYPARAMETER, prop2.connections[i].name, 302);
    }
  for (unsigned i = 0; i < _obj->prop3.num_connections; i++)
    {
      MATCH_OBJECT (BLOCKXYPARAMETER, prop3.connections[i].name, 303);
    }
  for (unsigned i = 0; i < _obj->prop4.num_connections; i++)
    {
      MATCH_OBJECT (BLOCKXYPARAMETER, prop4.connections[i].name, 304);
    }
  MATCH_OBJECT (BLOCKXYPARAMETER, x_label, 305);
  MATCH_OBJECT (BLOCKXYPARAMETER, x_label_desc, 306);
  MATCH_OBJECT (BLOCKXYPARAMETER, y_label, 307);
  MATCH_OBJECT (BLOCKXYPARAMETER, y_label_desc, 308);
  MATCH_OBJECT (BLOCKXYPARAMETER, x_value_set.desc, 410);
  MATCH_OBJECT (BLOCKXYPARAMETER, y_value_set.desc, 309);
  return found;
}
static int
match_BLOCKVISIBILITYPARAMETER (const char *restrict filename,
                                const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  Dwg_Object_BLOCKVISIBILITYPARAMETER *_obj
      = obj->tio.object->tio.BLOCKVISIBILITYPARAMETER;

  if (_obj->evalexpr.value_code == 1)
    {
      MATCH_OBJECT (BLOCKVISIBILITYPARAMETER, evalexpr.value.text1, 1);
    }
  MATCH_OBJECT (BLOCKVISIBILITYPARAMETER, name, 1);
  for (unsigned i = 0; i < _obj->prop1.num_connections; i++)
    {
      MATCH_OBJECT (BLOCKVISIBILITYPARAMETER, prop1.connections[i].name, 301);
    }
  for (unsigned i = 0; i < _obj->prop2.num_connections; i++)
    {
      MATCH_OBJECT (BLOCKVISIBILITYPARAMETER, prop2.connections[i].name, 302);
    }
  MATCH_OBJECT (BLOCKVISIBILITYPARAMETER, blockvisi_name, 301);
  MATCH_OBJECT (BLOCKVISIBILITYPARAMETER, blockvisi_desc, 302);
  for (unsigned i = 0; i < _obj->num_states; i++)
    {
      MATCH_OBJECT (BLOCKVISIBILITYPARAMETER, states[i].name, 303);
    }
  return found;
}
static int
match_NAVISWORKSMODELDEF (const char *restrict filename,
                          const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  MATCH_OBJECT (NAVISWORKSMODELDEF, path, 1);
  return found;
}

static int
match_OBJECTS (const char *restrict filename, Dwg_Data *restrict dwg)
{
  int found = 0;
  // char *text;

  if (!dwg)
    return 0;
  for (BITCODE_BL i = 0; i < dwg->num_objects; i++)
    {
      const Dwg_Object *obj = &dwg->object[i];
      if (obj->supertype != DWG_SUPERTYPE_OBJECT)
        continue;
      // processed later, --tables finds BLOCK
      if (obj->fixedtype == DWG_TYPE_BLOCK_HEADER)
        continue;
      if (numtype) // search for allowed --type and skip if not
        {
          int typeok = 0;
          for (int j = 0; j < numtype; j++)
            {
              if (obj->dxfname && !strcmp (type[j], obj->dxfname))
                {
                  typeok = 1;
                  break;
                }
            }
          if (!typeok) // next obj
            continue;
        }

#define ELSEMATCH(OBJECT)                                                     \
  else if (obj->fixedtype == DWG_TYPE_##OBJECT) found                         \
      += match_##OBJECT (filename, obj);

      if (obj->fixedtype == DWG_TYPE_LAYER)
        found += match_LAYER (filename, obj);
      ELSEMATCH (LTYPE)
      ELSEMATCH (STYLE)
      ELSEMATCH (VIEW)
      ELSEMATCH (VPORT)
      ELSEMATCH (DIMSTYLE)
      ELSEMATCH (UCS)
      ELSEMATCH (VX_TABLE_RECORD)
      if (opt_tables)
        continue;

      if (obj->fixedtype == DWG_TYPE_DICTIONARY
          || obj->fixedtype == DWG_TYPE_DICTIONARYWDFLT)
        found += match_DICTIONARY (filename, obj);
      ELSEMATCH (GROUP)
      ELSEMATCH (MLINESTYLE)
      ELSEMATCH (DICTIONARYVAR)
      ELSEMATCH (IMAGEDEF)
      ELSEMATCH (LAYER_INDEX)
      ELSEMATCH (LAYOUT)
      ELSEMATCH (PLOTSETTINGS)
      ELSEMATCH (SCALE)
      ELSEMATCH (FIELD)
      ELSEMATCH (TABLECONTENT)
      ELSEMATCH (GEODATA)
      else if (obj->fixedtype == DWG_TYPE_PDFDEFINITION) found
          += match_UNDERLAYDEFINITION (filename, obj);
      else if (obj->fixedtype == DWG_TYPE_DGNDEFINITION) found
          += match_UNDERLAYDEFINITION (filename, obj);
      else if (obj->fixedtype == DWG_TYPE_DWFDEFINITION) found
          += match_UNDERLAYDEFINITION (filename, obj);
      ELSEMATCH (VISUALSTYLE)
      ELSEMATCH (TABLESTYLE)
      ELSEMATCH (SUNSTUDY)
      ELSEMATCH (LIGHTLIST)
      ELSEMATCH (DBCOLOR)
      ELSEMATCH (MATERIAL)
      ELSEMATCH (DIMASSOC)
      ELSEMATCH (ASSOCOSNAPPOINTREFACTIONPARAM)
      ELSEMATCH (ASSOCACTIONPARAM)
      ELSEMATCH (ASSOCEDGEACTIONPARAM)
      ELSEMATCH (ASSOCFACEACTIONPARAM)
      ELSEMATCH (ASSOCOBJECTACTIONPARAM)
      ELSEMATCH (ASSOCPATHACTIONPARAM)
      ELSEMATCH (ASSOCVERTEXACTIONPARAM)
      ELSEMATCH (ASSOCPATCHSURFACEACTIONBODY)
      ELSEMATCH (ASSOCPLANESURFACEACTIONBODY)
      ELSEMATCH (ASSOCEXTENDSURFACEACTIONBODY)
      ELSEMATCH (ASSOCEXTRUDEDSURFACEACTIONBODY)
      ELSEMATCH (ASSOCFILLETSURFACEACTIONBODY)
      ELSEMATCH (ASSOCLOFTEDSURFACEACTIONBODY)
      ELSEMATCH (ASSOCNETWORKSURFACEACTIONBODY)
      ELSEMATCH (ASSOCOFFSETSURFACEACTIONBODY)
      ELSEMATCH (ASSOCREVOLVEDSURFACEACTIONBODY)
      ELSEMATCH (ASSOCSWEPTSURFACEACTIONBODY)
      ELSEMATCH (ASSOCTRIMSURFACEACTIONBODY)
      ELSEMATCH (ASSOCMLEADERACTIONBODY)
      ELSEMATCH (ASSOC3POINTANGULARDIMACTIONBODY)
      ELSEMATCH (ASSOCALIGNEDDIMACTIONBODY)
      ELSEMATCH (ASSOCORDINATEDIMACTIONBODY)
      ELSEMATCH (ASSOCROTATEDDIMACTIONBODY)
      ELSEMATCH (BLOCKPARAMDEPENDENCYBODY)
      ELSEMATCH (BLOCKBASEPOINTPARAMETER)
      ELSEMATCH (BLOCKFLIPPARAMETER)
      ELSEMATCH (BLOCKLINEARPARAMETER)
      ELSEMATCH (BLOCKROTATIONPARAMETER)
      ELSEMATCH (BLOCKXYPARAMETER)
      ELSEMATCH (BLOCKVISIBILITYPARAMETER)
      ELSEMATCH (BLOCKMOVEACTION)
      ELSEMATCH (BLOCKSTRETCHACTION)
      ELSEMATCH (BLOCKROTATEACTION)
      ELSEMATCH (BLOCKVISIBILITYGRIP)
      ELSEMATCH (BLOCKGRIPLOCATIONCOMPONENT)
      ELSEMATCH (NAVISWORKSMODELDEF)
    }
  return found;
}

static int
match_preR13_entities (const char *restrict filename,
                       const Dwg_Data *restrict dwg, const bool blocks)
{
  int found = 0;
  // char *text;

  // TODO skip block entities for now
  if (blocks)
    return found;
  for (unsigned j = 0; j < dwg->num_objects; j++)
    {
      const Dwg_Object *obj = &dwg->object[j];
      if (obj->fixedtype == DWG_TYPE_UNUSED)
        continue;
      if (verbose)
        fprintf (stderr, "%s [%d]\n", obj->name, obj->index);
      if (numtype) // search for allowed --type and skip if not
        {
          int typeok = 0;
          for (int i = 0; i < numtype; i++)
            {
              if (obj->dxfname && !strcmp (type[i], obj->dxfname))
                {
                  typeok = 1;
                  break;
                }
            }
          if (!typeok) // next obj
            continue;
        }

      if (obj->fixedtype == DWG_TYPE_TEXT)
        found += match_TEXT (filename, obj);
#ifdef WITH_SUBENTS
      ELSEMATCH (ATTRIB)
#endif
      ELSEMATCH (ATTDEF)
      if (!opt_text)
        {
          if (obj->type == DWG_TYPE_DIMENSION_r11)
            found += match_DIMENSION (filename, obj);
          ELSEMATCH (VIEWPORT)
          ELSEMATCH (BLOCK)
        }
      if (!opt_text)
        {
          // common entity names
          MATCH_TABLE (ENTITY, layer, LAYER, 8);
          MATCH_TABLE (ENTITY, ltype, LTYPE, 8);
        }
    }
  return found;
}

static int
match_BLOCK_HEADER (const char *restrict filename,
                    Dwg_Object_Ref *restrict ref)
{
  int found = 0;
  const Dwg_Object *hdr;
  const Dwg_Object *obj;
  char *text;

  if (!ref)
    return 0;
  obj = hdr = ref->obj;
  if (!hdr || hdr->supertype != DWG_SUPERTYPE_OBJECT
      || hdr->type != DWG_TYPE_BLOCK_HEADER)
    return 0;

  MATCH_OBJECT (BLOCK_HEADER, name, 2);
  if (opt_tables)
    return found;
  MATCH_OBJECT (BLOCK_HEADER, xref_pname, 1);
  MATCH_OBJECT (BLOCK_HEADER, description, 4);

  if (verbose)
    fprintf (stderr, "HDR: %d, HANDLE: " FORMAT_HV "\n", hdr->index,
             hdr->handle.value);
  for (obj = get_first_owned_entity (hdr); obj;
       obj = get_next_owned_entity (hdr, obj)) // without subentities
    {
      if (!obj)
        break;
      if (verbose)
        fprintf (stderr, "%s [%d], HANDLE: " FORMAT_HV "\n", obj->name,
                 obj->index, obj->handle.value);
      if (numtype) // search for allowed --type and skip if not
        {
          int typeok = 0;
          for (int i = 0; i < numtype; i++)
            {
              if (obj->dxfname && !strcmp (type[i], obj->dxfname))
                {
                  typeok = 1;
                  break;
                }
            }
          if (!typeok) // next obj
            continue;
        }

      if (obj->fixedtype == DWG_TYPE_TEXT)
        found += match_TEXT (filename, obj);
#ifdef WITH_SUBENTS
      ELSEMATCH (ATTRIB)
#endif
      ELSEMATCH (ATTDEF)
      ELSEMATCH (MTEXT)
      ELSEMATCH (ARCALIGNEDTEXT)
      else if (obj->fixedtype == DWG_TYPE_INSERT)
      {
#ifndef WITH_SUBENTS
        const Dwg_Data *dwg = obj->parent;
        Dwg_Entity_INSERT *_obj = obj->tio.entity->tio.INSERT;
        if (_obj->has_attribs)
          {
            if (dwg->header.version >= R_13b1 && dwg->header.version <= R_2000)
              {
                Dwg_Object *last_attrib = _obj->last_attrib->obj;
                Dwg_Object *o
                    = _obj->first_attrib ? _obj->first_attrib->obj : NULL;
                while (o && o->type == DWG_TYPE_ATTRIB)
                  {
                    found += match_ATTRIB (filename, o);
                    o = dwg_next_entity (o);
                    if (o == last_attrib)
                      break;
                  }
              }
            else if (dwg->header.version >= R_2004)
              {
                Dwg_Object *o;
                for (BITCODE_BL j = 0; j < _obj->num_owned; j++)
                  {
                    o = _obj->attribs[j] ? _obj->attribs[j]->obj : NULL;
                    if (o && o->type == DWG_TYPE_ATTRIB)
                      found += match_ATTRIB (filename, o);
                  }
              }
          }
#endif
      }
      else if (obj->fixedtype == DWG_TYPE_MINSERT)
      {
#ifndef WITH_SUBENTS
        const Dwg_Data *dwg = obj->parent;
        Dwg_Entity_MINSERT *_obj = obj->tio.entity->tio.MINSERT;
        if (_obj->has_attribs)
          {
            if (dwg->header.version >= R_13b1 && dwg->header.version <= R_2000)
              {
                Dwg_Object *last_attrib = _obj->last_attrib->obj;
                Dwg_Object *o
                    = _obj->first_attrib ? _obj->first_attrib->obj : NULL;
                while (o && o->type == DWG_TYPE_ATTRIB)
                  {
                    found += match_ATTRIB (filename, o);
                    o = dwg_next_entity (o);
                    if (o == last_attrib)
                      break;
                  }
              }
            else if (dwg->header.version >= R_2004)
              {
                Dwg_Object *o;
                for (BITCODE_BL j = 0; j < _obj->num_owned; j++)
                  {
                    o = _obj->attribs[j] ? _obj->attribs[j]->obj : NULL;
                    if (o && o->type == DWG_TYPE_ATTRIB)
                      found += match_ATTRIB (filename, o);
                  }
              }
          }
#endif
      }
      if (!opt_text)
        {
          if (obj->fixedtype == DWG_TYPE_DIMENSION_ORDINATE
              || obj->fixedtype == DWG_TYPE_DIMENSION_LINEAR
              || obj->fixedtype == DWG_TYPE_DIMENSION_ALIGNED
              || obj->fixedtype == DWG_TYPE_DIMENSION_ANG3PT
              || obj->fixedtype == DWG_TYPE_DIMENSION_ANG2LN
              || obj->fixedtype == DWG_TYPE_DIMENSION_RADIUS
              || obj->fixedtype == DWG_TYPE_DIMENSION_DIAMETER)
            found += match_DIMENSION (filename, obj);
          ELSEMATCH (VIEWPORT)
          else if (obj->fixedtype == DWG_TYPE__3DSOLID
                   || obj->fixedtype == DWG_TYPE_BODY
                   || obj->fixedtype == DWG_TYPE_REGION) found
              += match_3DSOLID (filename, obj);

          ELSEMATCH (BLOCK)
          ELSEMATCH (HATCH)
          ELSEMATCH (TOLERANCE)
          ELSEMATCH (TABLE)
          ELSEMATCH (GEOPOSITIONMARKER)
          ELSEMATCH (MULTILEADER)
          ELSEMATCH (LIGHT)
        }

      if (!opt_text)
        {
          // common entity names
          MATCH_TABLE (ENTITY, layer, LAYER, 8);
          MATCH_TABLE (ENTITY, ltype, LTYPE, 8);
          if (obj->parent->header.version >= R_2000)
            {
              MATCH_TABLE (ENTITY, plotstyle, PLOTSTYLE, 8);
            }
          if (obj->parent->header.version >= R_2007)
            {
              MATCH_TABLE (ENTITY, material, MATERIAL, 8);
              MATCH_TABLE (ENTITY, shadow, DICTIONARY, 8);
            }
          if (obj->parent->header.version >= R_2010)
            {
              MATCH_TABLE (ENTITY, full_visualstyle, VISUALSTYLE, 8);
              MATCH_TABLE (ENTITY, face_visualstyle, VISUALSTYLE, 8);
              MATCH_TABLE (ENTITY, edge_visualstyle, VISUALSTYLE, 8);
            }
        }
    }
  return found;
}

int
main (int argc, char *argv[])
{
  int error = 0;
  int i = 1, j;
  char *filename;
  Dwg_Data dwg;
#ifdef HAVE_PCRE2_H
  int errcode;
  size_t plen;
  PCRE2_SIZE erroffset;
  int have_jit;
#  ifdef HAVE_PCRE2_16
  BITCODE_TU pattern16;
#  endif
#endif
#if 0
  int opt_recurse = 0;
#endif
  int count = 0;
  int c;
#ifdef HAVE_GETOPT_LONG
  int option_index = 0;
  static struct option long_options[]
      = { { "case", 0, 0, 'i' },      { "extended", 0, 0, 'x' },
          { "count", 0, 0, 'c' },     { "no-filename", 0, 0, 'h' },
          { "recursive", 0, 0, 'r' }, { "recursive", 0, 0, 'R' },
          { "type", 1, 0, 'y' },      { "dxf", 1, 0, 'd' },
          { "text", 0, 0, 't' },      { "blocks", 0, 0, 'b' },
          { "tables", 0, 0, 'n' },    { "help", 0, 0, 0 },
          { "version", 0, 0, 0 },     { NULL, 0, NULL, 0 } };
#endif

  // check args
  if (argc < 2)
    return usage ();
  memset (dxf, 0, 10 * sizeof (short));

  while
#ifdef HAVE_GETOPT_LONG
      ((c = getopt_long (argc, argv, "ixchrRy:d:tbn", long_options,
                         &option_index))
       != -1)
#else
      ((c = getopt (argc, argv, "ixchrRy:d:tbnvu")) != -1)
#endif
    {
      if (c == -1)
        break;
      switch (c)
        {
#ifdef HAVE_GETOPT_LONG
        case 0:
          if (!strcmp (long_options[option_index].name, "help"))
            return help ();
          if (!strcmp (long_options[option_index].name, "version"))
            return opt_version ();
          break;
#else
        case 'v':
          return opt_version ();
#endif
#ifdef HAVE_PCRE2_H
        case 'x':
          options |= PCRE2_EXTENDED;
          break;
#endif
        case 'i':
          options |= PCRE2_CASELESS;
          break;
        case 'c':
          opt_count = 1;
          break;
        case 'h':
          opt_filename = 0;
          break;
#if 0
        case 'r':
        case 'R':
          opt_recurse = 1;
          break;
#endif
        case 't':
          opt_text = 1;
          break;
        case 'b':
          opt_blocks = 1;
          break;
        case 'n':
          opt_tables = 1;
          break;
        case 'y':
          if (numtype >= 10)
            return usage ();        // too many
          type[numtype++] = optarg; // a string
          break;
        case 'd':
          if (numdxf >= 10)
            return usage (); // too many
          // a integer group
          dxf[numdxf++] = (short)strtol (optarg, NULL, 10);
          break;

        case 'u':
          return help ();
        case '?':
          fprintf (stderr, "%s: invalid option '-%c' ignored\n", argv[0],
                   optopt);
          break;
        default:
          return usage ();
        }
    }
  i = optind;
  if (i > argc - 2) // need 2 more args. TODO: unless -R given
    return usage ();

  pattern = argv[i];
#ifdef HAVE_PCRE2_H
  plen = strlen (pattern);
  pcre2_config_8 (PCRE2_CONFIG_JIT, &have_jit);
  ri8 = pcre2_compile_8 ((PCRE2_SPTR8)pattern, /* pattern */
                         plen & 0xFFFFFFFF,    /* uint32_t */
                         options,              /* options */
                         &errcode,             /* errors */
                         &erroffset,           /* error offset */
#  ifdef USE_MATCH_CONTEXT
                         compile_context
#  else
                         NULL
#  endif
  );
  if (errcode != 0 && errcode != 100)
    {
      pcre2_get_error_message_8 (errcode, (PCRE2_UCHAR8 *)buf, 4096);
      // cppcheck-suppress preprocessorErrorDirective
      LOG_ERROR ("pcre2_compile_8 error %d: %s with %s", errcode, buf,
                 pattern);
      return 1;
    }
  match_data8 = pcre2_match_data_create_from_pattern_8 (ri8, NULL);
  if (have_jit)
    pcre2_jit_compile_8 (ri8, PCRE2_JIT_COMPILE_OPTIONS);

#  ifdef HAVE_PCRE2_16
  pcre2_config_16 (PCRE2_CONFIG_JIT, &have_jit);
  pattern16 = bit_utf8_to_TU (pattern, 0);
  ri16 = pcre2_compile_16 ((PCRE2_SPTR16)pattern16, /* pattern */
                           plen & 0xFFFFFFFF,       /* uint32_t */
                           options,                 /* options */
                           &errcode,                /* errors */
                           &erroffset,              /* error offset */
#    ifdef USE_MATCH_CONTEXT
                           compile_context
#    else
                           NULL
#    endif
  );
  free (pattern16);
  if (errcode != 0 && errcode != 100)
    {
      pcre2_get_error_message_8 (errcode, (PCRE2_UCHAR8 *)buf, 4096);
      LOG_ERROR ("pcre2_compile_16 error %d: %s with %s", errcode, buf,
                 pattern);
      return 1;
    }
  match_data16 = pcre2_match_data_create_from_pattern_16 (ri16, NULL);
  if (have_jit)
    pcre2_jit_compile_16 (ri16, PCRE2_JIT_COMPILE_OPTIONS);
#  endif
#endif

  // for all filenames...
  for (j = i + 1; j < argc; j++)
    {
      Dwg_Object_Ref *mspace_ref = NULL;
      filename = argv[j];
      memset (&dwg, 0, sizeof (Dwg_Data));
      dwg.opts = 0;
      error = dwg_read_file (filename, &dwg);
      if (error > DWG_ERR_CRITICAL)
        {
          fprintf (stderr, "Error: Could not read DWG file %s, error: 0x%x\n",
                   filename, error);
          continue;
        }

      if (!opt_text)
        count += match_OBJECTS (filename, &dwg);
      if (dwg.header.version < R_13b1)
        { // FIXME hack
          // mspace_ref = (Dwg_Object_Ref *)calloc (1, sizeof
          // (Dwg_Object_Ref)); mspace_ref->obj = &dwg.object[0];
          count += match_preR13_entities (filename, &dwg, false);
        }
      else
        mspace_ref = dwg_model_space_ref (&dwg);

      if (!opt_tables)
        count += match_BLOCK_HEADER (filename, mspace_ref);
      if (opt_blocks)
        {
          if (dwg.header.version < R_13b1)
            count += match_preR13_entities (filename, &dwg, true);
          else
            for (long k = 0; k < dwg.block_control.num_entries; k++)
              {
                count += match_BLOCK_HEADER (filename,
                                             dwg.block_control.entries[k]);
              }
        }
      if (!opt_tables)
        count += match_BLOCK_HEADER (filename, dwg_paper_space_ref (&dwg));

      fflush (stdout);
      // if (dwg.header.version < R_13b1)
      //   free (mspace_ref);
      if (j < argc)
        dwg_free (&dwg); // skip the last free
    }
  if (opt_count)
    printf ("%d\n", count);

  return count ? 0 : 1;
}
