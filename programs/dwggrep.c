/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2018 Free Software Foundation, Inc.                        */
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
 *
 * written by Reini Urban
 */

#include "../src/config.h"
#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_STRCASESTR
# undef  __DARWIN_C_LEVEL
# define __DARWIN_C_LEVEL __DARWIN_C_FULL
# ifndef __USE_GNU
#   define __USE_GNU
# endif
# include <string.h>
#else
# include <string.h>
# include <ctype.h>
#endif
#include <getopt.h>
#ifdef HAVE_PCRE2_H
//use both, 8 and 16 (r2007+)
# define PCRE2_CODE_UNIT_WIDTH 0
# include <pcre2.h>
#endif

#include "dwg.h"
#include "logging.h"
#include "common.h"
#include "bits.h"
#include "dwg_api.h"

#ifndef HAVE_PCRE2_H
# define PCRE2_MULTILINE 1
# define PCRE2_CASELESS 2
# define PCRE2_EXTENDED 3
# define PCRE2_NO_AUTO_CAPTURE 4
# define PCRE2_NO_DOTSTAR_ANCHOR 5
#else
# define PCRE2_JIT_MATCH_OPTIONS \
   (PCRE2_NO_UTF_CHECK|PCRE2_NOTBOL|PCRE2_NOTEOL|PCRE2_NOTEMPTY|\
    PCRE2_NOTEMPTY_ATSTART)
# define PCRE2_JIT_COMPILE_OPTIONS \
   (PCRE2_JIT_COMPLETE)
#endif

char *pattern;
char buf[4096];
// partial to find substrings, not only complete matches
int options = PCRE2_MULTILINE|PCRE2_NO_AUTO_CAPTURE|PCRE2_NO_DOTSTAR_ANCHOR;
int opt_count = 0;
int opt_text = 0;
int opt_tables = 0;
int opt_filename = 1;
short numdxf = 0;
short numtype = 0;
static short dxf[10];  //ensure zero-fill
static char* type[10]; //ensure zero-fill

/* the current version per spec block */
static unsigned int cur_ver = 0;

#ifdef HAVE_PCRE2_H
# undef USE_MATCH_CONTEXT
/* pcre2_compile */
static pcre2_code_8 *ri8;
static pcre2_match_data_8 *match_data8;
static pcre2_match_context_8 *match_context8 = NULL;
# ifdef HAVE_PCRE2_16
static pcre2_code_16 *ri16;
static pcre2_match_data_16 *match_data16;
static pcre2_match_context_16 *match_context16 = NULL;
# endif

# ifdef USE_MATCH_CONTEXT
static pcre2_jit_stack_8 *jit_stack8 = NULL;
static pcre2_compile_context_8 *compile_context8 = NULL;
#  ifdef HAVE_PCRE2_16
static pcre2_jit_stack_16 *jit_stack16 = NULL;
static pcre2_compile_context_16 *compile_context16 = NULL;
#  endif
# endif
#endif

static int usage(void) {
  printf("\nUsage: dwggrep [-cRr] pattern *.dwg\n");
  return 1;
}
static int opt_version(void) {
  printf("dwggrep %s\n", PACKAGE_VERSION);
  return 0;
}
static int help(void) {
  printf("\nUsage: dwggrep [OPTIONS]... pattern files\n");
#ifdef HAVE_PCRE2_H
  printf("Search regex pattern in a list of DWGs.\n\n");
#else
  printf("Search string (no regex) in a list of DWGs.\n\n");
#endif
  printf("  -i                        Case-insensitive pattern\n");
#ifdef HAVE_PCRE2_H
  printf("  -x                        Extended regex pattern\n");
#endif
  printf("  -c, --count               Print only the count of matched elements.\n");
  printf("  -h, --no-filename         Print no filename.\n");
#if 0
  printf("  -R, -r, --recursive       Recursively search subdirectories listed.\n");
#endif
  printf("  -y, --type NAME           Search only NAME entities or objects.\n");
  printf("  -d, --dxf NUM             Search only DXF group NUM fields.\n");
  printf("  -t, --text                Search only in TEXT-like entities.\n");
#if 0
  printf("  -b, --tables              Search only in table names.\n");
#endif
#ifdef HAVE_GETOPT_LONG
  printf("      --help                Display this help and exit\n");
  printf("      --version             Output version information and exit\n"
         "\n");
#else
  printf("  -u                        Display this help and exit\n");
  printf("  -v                        Output version information and exit\n"
         "\n");
#endif
  printf("GNU LibreDWG online manual: <https://www.gnu.org/software/libredwg/>\n");
  return 0;
}

static void print_match(const int is16, const char *restrict filename,
                        const char *restrict entity,
                        const int dxfgroup, char *restrict text)
{
  if (is16)
    text = bit_convert_TU((BITCODE_TU)text);
  printf("%s %s %d: %s\n", opt_filename ? filename : "", entity, dxfgroup, text);
  if (is16)
    free(text);
}

static int
do_match (const int is16, const char *restrict filename,
          const char *restrict entity,
          const int dxfgroup, char *restrict text)
{
#ifdef HAVE_PCRE2_H
  int rc;
# ifdef HAVE_PCRE2_16
  if (is16)
    rc = pcre2_match_16(ri16, (PCRE2_SPTR16)text, PCRE2_ZERO_TERMINATED, 0,
                           PCRE2_JIT_MATCH_OPTIONS,
                           match_data16,     /* block for storing the result */
                           match_context16); /* disabled */
  else
# endif
    // already converted to UTF-8 before
    rc = pcre2_match_8(ri8, (PCRE2_SPTR8)text, PCRE2_ZERO_TERMINATED, 0,
                       PCRE2_JIT_MATCH_OPTIONS,
                       match_data8,     /* block for storing the result */
                       match_context8); /* disabled */
  if (rc >= 0) {
    if (!opt_count)
      print_match(is16, filename, entity, dxfgroup, text);
    return 1;
  } else if (rc < -2) { //not PCRE2_ERROR_NOMATCH nor PCRE2_ERROR_PARTIAL
    pcre2_get_error_message_8(rc, (PCRE2_UCHAR8 *)buf, 4096);
    LOG_WARN("pcre2 match error %s with %s", buf, pattern);
  }
  return 0;

#else

  if (options & PCRE2_CASELESS)
    {
# ifndef HAVE_STRCASESTR
      int i, len, dmax;
      char *dest = text;
      int dlen = dmax = strlen(text);
      char *src = pattern;
      int slen = strlen(pattern);

      while (*dest && dmax)
        {
          i = 0;
          len = slen;
          dlen = dmax;
          while (dest[i] && dlen)
            {
              if (toupper((unsigned char)dest[i]) != toupper((unsigned char)src[i])) {
                break;
              }
              /* move to the next char */
              i++;
              len--;
              dlen--;

              if (src[i] == '\0' || !len) {
                if (!opt_count)
                  print_match(is16, filename, entity, dxfgroup, text);
                return 1;
              }
            }
          dest++;
          dmax--;
        }
# else
      if (strcasestr(text, pattern))
        {
          if (!opt_count)
            print_match(is16, filename, entity, dxfgroup, text);
          return 1;
        }
# endif
    }
  else
    {
      if (strstr(text, pattern)) {
        if (!opt_count)
          print_match(is16, filename, entity, dxfgroup, text);
        return 1;
      }
    }
  return 0;
#endif
}

// check matching dxfgroup first to avoid costly utf8 conversions
#define MATCH_DXF(type,ENTITY,text_field,dxfgroup)  \
  if (numdxf) { \
    int dxfok = 0; \
    for (int _i=0; _i<numdxf; _i++) { \
      if (dxf[_i] == dxfgroup) { dxfok = 1; break; } \
    } \
    if (dxfok) { MATCH_TYPE(type,ENTITY,text_field,dxfgroup); } \
  } \
  else { \
    MATCH_TYPE(type,ENTITY,text_field,dxfgroup); \
  }

//8bit only
#define MATCH_NO16(type,ENTITY,text_field,dxfgroup) \
  text = (char*)obj->tio.type->tio.ENTITY->text_field; \
  if (text && numdxf) { \
    int dxfok = 0; \
    for (int i=0; i<numdxf; i++) { \
      if (dxf[i] == dxfgroup) { dxfok = 1; break; } \
    } \
    if (dxfok) { \
      found += do_match(0, filename, #ENTITY, dxfgroup, text); \
    } \
  } \
  else if (text) { \
    found += do_match(0, filename, #ENTITY, dxfgroup, text); \
  }

#ifdef HAVE_PCRE2_16
#define MATCH_TYPE(type,ENTITY,text_field,dxfgroup)  \
  text = (char*)obj->tio.type->tio.ENTITY->text_field; \
  if (text) \
    found += do_match(obj->parent->header.version >= R_2007, filename, #ENTITY, dxfgroup, text)
#else
#define MATCH_TYPE(type,ENTITY,text_field,dxfgroup)  \
  text = (char*)obj->tio.type->tio.ENTITY->text_field; \
  if (text) { \
    if (obj->parent->header.version >= R_2007) \
      text = bit_convert_TU((BITCODE_TU)text); \
    found += do_match(obj->parent->header.version >= R_2007, filename, #ENTITY, dxfgroup, text); \
    if (obj->parent->header.version >= R_2007) \
      free(text); \
  }
#endif

#define MATCH_ENTITY(ENTITY,text_field,dxf) \
  MATCH_DXF(entity,ENTITY,text_field,dxf)
#define MATCH_OBJECT(ENTITY,text_field,dxf) \
  MATCH_DXF(object,ENTITY,text_field,dxf)
#define MATCH_TABLE(ENTITY, handle, TABLE, dxf) {}

static
int match_TEXT(const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  MATCH_ENTITY (TEXT, text_value, 1);
  if (!opt_text)
    MATCH_TABLE (TEXT, style, STYLE, 7);
  return found;
}

static
int match_ATTRIB(const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  MATCH_ENTITY (ATTRIB, text_value, 1);
  MATCH_ENTITY (ATTRIB, tag, 2);
  if (!opt_text)
    MATCH_TABLE (ATTRIB, style, STYLE, 7);
  return found;
}

static
int match_ATTDEF(const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  MATCH_ENTITY (ATTDEF, default_value, 1);
  MATCH_ENTITY (ATTDEF, tag, 2);
  MATCH_ENTITY (ATTDEF, prompt, 3);
  return found;
}

static
int match_MTEXT(const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  MATCH_ENTITY (MTEXT, text, 1);
  return found;
}

static
int match_BLOCK(const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  MATCH_ENTITY (BLOCK, name, 2);
  return found;
}

static
int match_DIMENSION(const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  //int is16 = obj->parent->header.version >= R_2007;

  text = obj->tio.entity->tio.DIMENSION_ORDINATE->user_text;
  if (text)
    found += do_match(0, filename, "DIMENSION", 1, text);
  return found;
}

static
int match_VIEWPORT(const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  MATCH_ENTITY (VIEWPORT, style_sheet, 1);
  return found;
}

static
int match_3DSOLID(const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text = NULL;
  int found = 0;
  BITCODE_BL j;
  Dwg_Entity_3DSOLID *_obj = obj->tio.entity->tio._3DSOLID;

  if (!_obj || !obj->tio.entity) return 0;
  if (_obj->acis_data) {
    MATCH_NO16 (entity, _3DSOLID, acis_data, 1);
    //MATCH_ENTITY (_3DSOLID, acis_data, 1);
    //found += do_match(0, filename, "3DSOLID", 1, (char*)_obj->acis_data);
  }
  /*
  if (!_obj->encr_sat_data) return 0;
  for (j=0; j<_obj->num_blocks; j++)
    {
      //text = _obj->encr_sat_data[j];
      //if (text)
      //  found += do_match(0, filename, "3DSOLID", 301, text);
      MATCH_NO16 (entity, _3DSOLID, encr_sat_data[j], 301);
    }
  */
  return found;
}

static
int match_DICTIONARY(const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  BITCODE_BL i;
  Dwg_Object_DICTIONARY *_obj = obj->tio.object->tio.DICTIONARY;

  for (i=0; i<_obj->numitems; i++)
    {
      MATCH_OBJECT (DICTIONARY, text[i], 3);
    }
  return found;
}

static
int match_STYLE(const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  MATCH_OBJECT (STYLE, font_name, 3);
  MATCH_OBJECT (STYLE, bigfont_name, 4);
  return found;
}

static
int match_LTYPE(const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  MATCH_OBJECT (LTYPE, description, 3);
  MATCH_OBJECT (LTYPE, strings_area, 3);
  return found;
}

static
int match_DIMSTYLE(const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  MATCH_OBJECT (DIMSTYLE, DIMPOST, 3);
  MATCH_OBJECT (DIMSTYLE, DIMAPOST, 4);
  MATCH_OBJECT (DIMSTYLE, DIMBLK_T, 5);
  MATCH_OBJECT (DIMSTYLE, DIMBLK1_T, 6);
  MATCH_OBJECT (DIMSTYLE, DIMBLK2_T, 7);
  MATCH_OBJECT (DIMSTYLE, DIMMZS, 0);
  MATCH_OBJECT (DIMSTYLE, DIMALTMZS, 0);
  return found;
}

static
int match_GROUP(const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  MATCH_OBJECT (GROUP, name, 3);
  return found;
}

static
int match_MLINESTYLE(const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  MATCH_OBJECT (MLINESTYLE, entry_name, 2);
  MATCH_OBJECT (MLINESTYLE, desc, 3);
  return found;
}

static
int match_DICTIONARYVAR(const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  MATCH_OBJECT (DICTIONARYVAR, str, 1);
  return found;
}

static
int match_HATCH(const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  MATCH_ENTITY (HATCH, name, 2);
  MATCH_ENTITY (HATCH, gradient_name, 470);
  return found;
}

static
int match_IMAGEDEF(const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  MATCH_OBJECT (IMAGEDEF, file_path, 1);
  return found;
}

static
int match_SCALE(const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0, i;
  const Dwg_Object_SCALE *_obj = obj->tio.object->tio.SCALE;

  MATCH_OBJECT (SCALE, name, 1);
  return found;
}

static
int match_LAYER_INDEX(const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  BITCODE_BL i;
  const Dwg_Object_LAYER_INDEX *_obj = obj->tio.object->tio.LAYER_INDEX;

  for (i=0; i<_obj->num_entries; i++)
    {
      MATCH_OBJECT (LAYER_INDEX, entries[i].layer, 8);
    }
  return found;
}

static
int match_LAYOUT(const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  BITCODE_BL i;
  const Dwg_Object_LAYOUT *_obj = obj->tio.object->tio.LAYOUT;

  MATCH_OBJECT (LAYOUT, page_setup_name, 1);
  MATCH_OBJECT (LAYOUT, printer_or_config, 2);
  MATCH_OBJECT (LAYOUT, paper_size, 4);
  MATCH_OBJECT (LAYOUT, plot_view_name, 6);
  MATCH_OBJECT (LAYOUT, current_style_sheet, 7);
  MATCH_OBJECT (LAYOUT, layout_name, 1);
  MATCH_TABLE (LAYOUT, plot_view, ??, 6);
  MATCH_TABLE (LAYOUT, visual_style, ??, 0);
  MATCH_TABLE (LAYOUT, base_ucs, UCS, 346);
  MATCH_TABLE (LAYOUT, named_ucs, UCS, 345);
  for (i=0; i<_obj->num_viewports; i++)
    {
      MATCH_TABLE (LAYOUT, viewports[i], VPORT, 0);
    }
  return found;
}

static
int match_FIELD(const char *restrict filename, const Dwg_Object *restrict obj)
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
  for (i=0; i<_obj->num_childval; i++)
    {
      MATCH_OBJECT (FIELD, childval[i].key, 6);
      MATCH_OBJECT (FIELD, childval[i].value.format_string, 300);
      MATCH_OBJECT (FIELD, childval[i].value.value_string, 302);
    }
  return found;
}

static
int match_TABLE(const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  BITCODE_BL i;
  const Dwg_Entity_TABLE *_obj = obj->tio.entity->tio.TABLE;

  for (i=0; i<_obj->num_cells; i++)
    {
      if (_obj->cells[i].type == 1) {
        MATCH_ENTITY (TABLE, cells[i].text_string, 1);
      } else if (_obj->cells[i].type == 2 && _obj->cells[i].additional_data_flag == 1) {
        MATCH_ENTITY (TABLE, cells[i].attr_def_text, 300);
      }
    }
  return found;
}

static
int match_TABLECONTENT(const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  BITCODE_BL i, j, k;
  const Dwg_Object_TABLECONTENT *_obj = obj->tio.object->tio.TABLECONTENT;

  MATCH_OBJECT (TABLECONTENT, ldata.name, 1);
  MATCH_OBJECT (TABLECONTENT, ldata.desc, 300);
  for (i=0; i<_obj->tdata.num_cols; i++)
    {
      MATCH_OBJECT (TABLECONTENT, tdata.cols[i].name, 300);
    }
  for (i=0; i<_obj->tdata.num_rows; i++)
    {
      for (j=0; j<_obj->tdata.rows[i].num_cells; j++)
        {
          MATCH_OBJECT (TABLECONTENT, tdata.rows[i].cells[j].tooltip, 300);
          for (k=0; k<_obj->tdata.rows[i].cells[j].num_customdata_items; k++)
            {
              #define _custom tdata.rows[i].cells[j].customdata_items[k]
              MATCH_OBJECT (TABLECONTENT, _custom.name, 300);
              if (_obj->_custom.value.data_type == 4)
                {
                  MATCH_OBJECT (TABLECONTENT, _custom.value.data_string, 302);
                }
              #undef _custom
            }
          for (k=0; k<_obj->tdata.rows[i].cells[j].num_cell_contents; k++)
            {
              #define _content tdata.rows[i].cells[j].cell_contents[k]
              if (_obj->_content.type == 1 && _obj->_content.value.data_type == 4)
                {
                  MATCH_OBJECT (TABLECONTENT, _content.value.data_string, 302);
                }
              #undef _content
            }
        }
    }
  return found;
}

static
int match_GEODATA(const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  const Dwg_Object_GEODATA *_obj = obj->tio.object->tio.GEODATA;

  MATCH_OBJECT (GEODATA, coord_system_def, 0);
  MATCH_OBJECT (GEODATA, geo_rss_tag, 302);
  MATCH_OBJECT (GEODATA, observation_from_tag, 305);
  MATCH_OBJECT (GEODATA, observation_to_tag, 306);
  MATCH_OBJECT (GEODATA, observation_coverage_tag, 0);
  //obsolete
  MATCH_OBJECT (GEODATA, coord_system_datum, 0);
  MATCH_OBJECT (GEODATA, coord_system_wkt, 0);
  return found;
}

static
int match_GEOPOSITIONMARKER(const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  const Dwg_Entity_GEOPOSITIONMARKER *_obj = obj->tio.entity->tio.GEOPOSITIONMARKER;

  MATCH_ENTITY (GEOPOSITIONMARKER, text, 1);
  MATCH_ENTITY (GEOPOSITIONMARKER, notes, 3);
  return found;
}

static
int match_UNDERLAYDEFINITION(const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  const Dwg_Object_UNDERLAYDEFINITION *_obj = obj->tio.object->tio.UNDERLAYDEFINITION;

  MATCH_OBJECT (UNDERLAYDEFINITION, filename, 1);
  MATCH_OBJECT (UNDERLAYDEFINITION, name, 2);
  return found;
}

static
int match_VISUALSTYLE(const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0, i;
  const Dwg_Object_VISUALSTYLE *_obj = obj->tio.object->tio.VISUALSTYLE;

  MATCH_OBJECT (VISUALSTYLE, desc, 1);
  return found;
}

static
int match_LIGHT(const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  const Dwg_Entity_LIGHT *_obj = obj->tio.entity->tio.LIGHT;

  MATCH_ENTITY (LIGHT, name, 1);
  //MATCH_ENTITY (LIGHT, web_file, 1);
  return found;
}

static
int match_SUNSTUDY(const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  const Dwg_Object_SUNSTUDY *_obj = obj->tio.object->tio.SUNSTUDY;

  MATCH_OBJECT (SUNSTUDY, setup_name, 1);
  MATCH_OBJECT (SUNSTUDY, desc, 2);
  MATCH_OBJECT (SUNSTUDY, sheet_set_name, 3);
  return found;
}

static
int match_LIGHTLIST(const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0, i;
  const Dwg_Object_LIGHTLIST *_obj = obj->tio.object->tio.LIGHTLIST;

  for (i=0; i<_obj->num_lights; i++)
    {
      //MATCH_OBJECT (LIGHTLIST, lights[i].name, 1);
    }
  return found;
}

static
int match_DBCOLOR(const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  const Dwg_Object_DBCOLOR *_obj = obj->tio.object->tio.DBCOLOR;

  MATCH_OBJECT (DBCOLOR, name, 430);
  MATCH_OBJECT (DBCOLOR, catalog, 430);
  return found;
}

static
int match_MATERIAL(const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  const Dwg_Object_MATERIAL *_obj = obj->tio.object->tio.MATERIAL;

  MATCH_OBJECT (MATERIAL, name, 1);
  MATCH_OBJECT (MATERIAL, desc, 2);
  MATCH_OBJECT (MATERIAL, diffusemap_filename, 3);
  MATCH_OBJECT (MATERIAL, specularmap_filename, 4);
  MATCH_OBJECT (MATERIAL, reflectionmap_filename, 6);
  MATCH_OBJECT (MATERIAL, opacitymap_filename, 7);
  MATCH_OBJECT (MATERIAL, bumpmap_filename, 8);
  MATCH_OBJECT (MATERIAL, refractionmap_filename, 9);
  MATCH_OBJECT (MATERIAL, normalmap_filename, 3);
  MATCH_OBJECT (MATERIAL, genprocname, 300);
  MATCH_OBJECT (MATERIAL, genprocvaltext, 301);
  MATCH_OBJECT (MATERIAL, genprocvalcolorname, 430);
  return found;
}

static
int match_PLOTSETTINGS(const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  const Dwg_Object_PLOTSETTINGS *_obj = obj->tio.object->tio.PLOTSETTINGS;

  MATCH_OBJECT (PLOTSETTINGS, page_setup_name, 1);
  MATCH_OBJECT (PLOTSETTINGS, printer_cfg_file, 2);
  MATCH_OBJECT (PLOTSETTINGS, paper_size, 4);
  return found;
}

static
int match_ASSOCACTION(const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  const Dwg_Object_ASSOCACTION *_obj = obj->tio.object->tio.ASSOCACTION;
  MATCH_OBJECT (ASSOCACTION, body.evaluatorid, 0);
  MATCH_OBJECT (ASSOCACTION, body.expression, 0);
  return found;
}
static
int match_ASSOCOSNAPPOINTREFACTIONPARAM(const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  const Dwg_Object_ASSOCOSNAPPOINTREFACTIONPARAM *_obj = obj->tio.object->tio.ASSOCOSNAPPOINTREFACTIONPARAM;
  MATCH_OBJECT (ASSOCOSNAPPOINTREFACTIONPARAM, name, 1);
  return found;
}
static
int match_ACDBNAVISWORKSMODELDEF(const char *restrict filename, const Dwg_Object *restrict obj)
{
  char *text;
  int found = 0;
  const Dwg_Object_ACDBNAVISWORKSMODELDEF *_obj = obj->tio.object->tio.ACDBNAVISWORKSMODELDEF;
  MATCH_OBJECT (ACDBNAVISWORKSMODELDEF, path, 1);
  return found;
}

static
int match_BLOCK_HEADER(const char *restrict filename, Dwg_Object_Ref *restrict ref)
{
  int found = 0;
  Dwg_Object *hdr;
  Dwg_Object *obj;
  char *text;

  if (!ref)
    return 0;
  obj = hdr = ref->obj;
  if (!hdr ||
      hdr->supertype != DWG_SUPERTYPE_OBJECT ||
      hdr->type != DWG_TYPE_BLOCK_HEADER)
    return 0;

  MATCH_OBJECT (BLOCK_HEADER, xref_pname, 1);
  MATCH_OBJECT (BLOCK_HEADER, description, 4);

  //fprintf(stderr, "HDR: %d, HANDLE: %X\n", hdr->address, hdr->handle.value);
  for (obj = get_first_owned_object(hdr);
       obj;
       obj = get_next_owned_object(hdr, obj))
    {
      if (numtype) //search for allowed --type and skip if not
        {
          int typeok = 0;
          for (int i=0; i<numtype; i++) {
            if (obj->dxfname && !strcmp(type[i], obj->dxfname)) {
              typeok = 1;
              break;
            }
          }
          if (!typeok) //next obj
            continue;
        }
      if (!opt_tables)
        { // opt_text:
          if (obj->type == DWG_TYPE_TEXT)
            found += match_TEXT(filename, obj);
          else if (obj->type == DWG_TYPE_ATTRIB)
            found += match_ATTRIB(filename, obj);
          else if (obj->type == DWG_TYPE_ATTDEF)
            found += match_ATTDEF(filename, obj);
          else if (obj->type == DWG_TYPE_MTEXT)
            found += match_MTEXT(filename, obj);
          if (!opt_text)
            {
              if (obj->type == DWG_TYPE_BLOCK)
                found += match_BLOCK(filename, obj);
              else if (obj->type == DWG_TYPE_DIMENSION_ORDINATE ||
                       obj->type == DWG_TYPE_DIMENSION_LINEAR ||
                       obj->type == DWG_TYPE_DIMENSION_ALIGNED ||
                       obj->type == DWG_TYPE_DIMENSION_ANG3PT ||
                       obj->type == DWG_TYPE_DIMENSION_ANG2LN ||
                       obj->type == DWG_TYPE_DIMENSION_RADIUS ||
                       obj->type == DWG_TYPE_DIMENSION_DIAMETER)
                found += match_DIMENSION(filename, obj);
              else if (obj->type == DWG_TYPE_VIEWPORT)
                found += match_VIEWPORT(filename, obj);
              else if (obj->type == DWG_TYPE__3DSOLID ||
                       obj->type == DWG_TYPE_BODY ||
                       obj->type == DWG_TYPE_REGION)
                found += match_3DSOLID(filename, obj);
              //tables??
              else if (obj->type == DWG_TYPE_STYLE)
                found += match_STYLE(filename, obj);
              else if (obj->type == DWG_TYPE_LTYPE)
                found += match_LTYPE(filename, obj);
              else if (obj->type == DWG_TYPE_DIMSTYLE)
                found += match_DIMSTYLE(filename, obj);
              else if (obj->fixedtype == DWG_TYPE_DICTIONARY)
                found += match_DICTIONARY(filename, obj);

              else if (obj->fixedtype == DWG_TYPE_GROUP)
                found += match_GROUP(filename, obj);
              else if (obj->fixedtype == DWG_TYPE_MLINESTYLE)
                found += match_MLINESTYLE(filename, obj);
              else if (obj->fixedtype == DWG_TYPE_DICTIONARYVAR)
                found += match_DICTIONARYVAR(filename, obj);
              else if (obj->fixedtype == DWG_TYPE_HATCH)
                found += match_HATCH(filename, obj);
              else if (obj->type == DWG_TYPE_IMAGEDEF)
                found += match_IMAGEDEF(filename, obj);
              else if (obj->type == DWG_TYPE_LAYER_INDEX)
                found += match_LAYER_INDEX(filename, obj);
              else if (obj->type == DWG_TYPE_LAYOUT)
                found += match_LAYOUT(filename, obj);
              else if (obj->fixedtype == DWG_TYPE_SCALE)
                found += match_SCALE(filename, obj);
              else if (obj->fixedtype == DWG_TYPE_FIELD)
                found += match_FIELD(filename, obj);
              else if (obj->fixedtype == DWG_TYPE_TABLE)
                found += match_TABLE(filename, obj);
              else if (obj->fixedtype == DWG_TYPE_TABLECONTENT)
                found += match_TABLECONTENT(filename, obj);
              else if (obj->fixedtype == DWG_TYPE_GEODATA)
                found += match_GEODATA(filename, obj);
              else if (obj->fixedtype == DWG_TYPE_GEOPOSITIONMARKER)
                found += match_GEOPOSITIONMARKER(filename, obj);
              else if (obj->fixedtype == DWG_TYPE_UNDERLAYDEFINITION)
                found += match_UNDERLAYDEFINITION(filename, obj);
              else if (obj->fixedtype == DWG_TYPE_VISUALSTYLE)
                found += match_VISUALSTYLE(filename, obj);
              else if (obj->fixedtype == DWG_TYPE_LIGHT)
                found += match_LIGHT(filename, obj);
              else if (obj->fixedtype == DWG_TYPE_SUNSTUDY)
                found += match_SUNSTUDY(filename, obj);
              else if (obj->fixedtype == DWG_TYPE_LIGHTLIST)
                found += match_LIGHTLIST(filename, obj);
              else if (obj->fixedtype == DWG_TYPE_DBCOLOR)
                found += match_DBCOLOR(filename, obj);
              else if (obj->fixedtype == DWG_TYPE_MATERIAL)
                found += match_MATERIAL(filename, obj);
              else if (obj->fixedtype == DWG_TYPE_PLOTSETTINGS)
                found += match_PLOTSETTINGS(filename, obj);
              else if (obj->fixedtype == DWG_TYPE_ASSOCACTION)
                found += match_ASSOCACTION(filename, obj);
              else if (obj->fixedtype == DWG_TYPE_ASSOCOSNAPPOINTREFACTIONPARAM)
                found += match_ASSOCOSNAPPOINTREFACTIONPARAM(filename, obj);
              else if (obj->fixedtype == DWG_TYPE_ACDBNAVISWORKSMODELDEF)
                found += match_ACDBNAVISWORKSMODELDEF(filename, obj);
            }
        }
      if (!opt_text)
        {
          if (obj->supertype == DWG_SUPERTYPE_ENTITY)
            {
              //common entity names
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
    }
  return found;
}

int
main (int argc, char *argv[])
{
  int error = 0;
  int i = 1, j;
  char* filename;
  Dwg_Data dwg;
  Bit_Chain dat;
  int plen;
  int errcode;
#ifdef HAVE_PCRE2_H
  PCRE2_SIZE erroffset;
  int have_jit;
# ifdef HAVE_PCRE2_16
  BITCODE_TU pattern16;
# endif
#endif
  int opt_recurse = 0;
  int count = 0;
  int c;
#ifdef HAVE_GETOPT_LONG
  int option_index = 0;
  static struct option long_options[] = {
        {"case",      0, 0, 'i'},
        {"extended",  0, 0, 'x'},
        {"count",     0, 0, 'c'},
        {"no-filename",0, 0, 'h'},
        {"recursive", 0, 0, 'r'},
        {"recursive", 0, 0, 'R'},
        {"type",      1, 0, 'y'},
        {"dxf",       1, 0, 'd'},
        {"text",      0, 0, 't'},
        {"tables",    0, 0, 'b'},
        {"help",      0, 0, 0},
        {"version",   0, 0, 0},
        {NULL,        0, NULL, 0}
  };
#endif

  // check args
  if (argc < 2)
    return usage();
  memset(dxf, 0, 10*sizeof(short));

  while
#ifdef HAVE_GETOPT_LONG
    ((c = getopt_long(argc, argv, "ixchrRy:d:tb",
                      long_options, &option_index)) != -1)
#else
    ((c = getopt(argc, argv, "ixchrRy:d:tbvu")) != -1)
#endif
    {
      if (c == -1) break;
      switch (c) {
#ifdef HAVE_GETOPT_LONG
      case 0:
        if (!strcmp(long_options[option_index].name, "help"))
          return help();
        if (!strcmp(long_options[option_index].name, "version"))
          return opt_version();
        break;
#else
      case 'v':
        return opt_version();
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
      case 'r':
      case 'R':
        opt_recurse = 1;
        break;
      case 't':
        opt_text = 1;
        break;
      case 'b':
        opt_tables = 1;
        break;
      case 'y':
        if (numtype >= 10) return usage(); //too many
        type[numtype++] = optarg; // a string
        break;
      case 'd':
        if (numdxf >= 10) return usage(); // too many
        // a integer group
        dxf[numdxf++] = (short)strtol(optarg, NULL, 10);
        break;

      case 'u':
        return help();
      case '?':
        fprintf(stderr, "%s: invalid option '-%c' ignored\n",
                argv[0], optopt);
        break;
      default:
        return usage();
      }
    }
  i = optind;
  if (i > argc-2) // need 2 more args. TODO: unless -R given
    return usage();

  pattern = argv[i]; plen = strlen(pattern);
#ifdef HAVE_PCRE2_H
  pcre2_config_8(PCRE2_CONFIG_JIT, &have_jit);
  ri8 = pcre2_compile_8(
     (PCRE2_SPTR8)pattern, plen, /* pattern */
     options,      /* options */
     &errcode,     /* errors */
     &erroffset,   /* error offset */
# ifdef USE_MATCH_CONTEXT
     compile_context
# else
     NULL
# endif
    );
  if (errcode != 0 && errcode != 100) {
    pcre2_get_error_message_8(errcode, (PCRE2_UCHAR8 *)buf, 4096);
    LOG_ERROR("pcre2_compile_8 error %d: %s with %s", errcode, buf, pattern);
    return 1;
  }
  match_data8 = pcre2_match_data_create_from_pattern_8(ri8, NULL);
  if (have_jit)
    pcre2_jit_compile_8(ri8, PCRE2_JIT_COMPILE_OPTIONS);
  
# ifdef HAVE_PCRE2_16
  pcre2_config_16(PCRE2_CONFIG_JIT, &have_jit);
  pattern16 = bit_utf8_to_TU(pattern);
  ri16 = pcre2_compile_16(
     (PCRE2_SPTR16)pattern16, plen, /* pattern */
     options,      /* options */
     &errcode,     /* errors */
     &erroffset,   /* error offset */
#  ifdef USE_MATCH_CONTEXT
     compile_context
#  else
     NULL
#  endif
    );
  if (errcode != 0 && errcode != 100) {
    pcre2_get_error_message_8(errcode, (PCRE2_UCHAR8 *)buf, 4096);
    LOG_ERROR("pcre2_compile_16 error %d: %s with %s", errcode, buf, pattern);
    return 1;
  }
  match_data16 = pcre2_match_data_create_from_pattern_16(ri16, NULL);
  if (have_jit)
    pcre2_jit_compile_16(ri16, PCRE2_JIT_COMPILE_OPTIONS);
# endif
#endif

  //for all filenames...
  for (j=i+1; j<argc; j++)
    {
      long k;

      filename = argv[j];
      memset(&dwg, 0, sizeof(Dwg_Data));
      dwg.opts = 0;
      error = dwg_read_file(filename, &dwg);
      if (error > DWG_ERR_CRITICAL)
        {
          fprintf(stderr, "Error: Could not read DWG file %s, error: 0x%x\n",
                  filename, error);
          continue;
        }

      count += match_BLOCK_HEADER(filename, dwg_model_space_ref(&dwg));
      for (k=0; k < dwg.block_control.num_entries; k++)
        {
          count += match_BLOCK_HEADER(filename, dwg.block_control.block_headers[k]);
        }
      count += match_BLOCK_HEADER(filename, dwg_paper_space_ref(&dwg));

      if (j < argc)
        dwg_free(&dwg); //skip the last free
    }
  if (opt_count)
    printf("%d\n", count);
  
  return count ? 0 : 1;
}
