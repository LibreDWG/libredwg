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
 * uses pcre2-8 (not yet pcre2-16, rather convert down to UTF-8)
 *
 * written by Reini Urban
 */

#include "../src/config.h"
#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_STRCASESTR
# undef  __DARWIN_C_LEVEL
# define __DARWIN_C_LEVEL __DARWIN_C_FULL
# include <string.h>
#else
# include <string.h>
# include <ctype.h>
#endif
#ifdef HAVE_PCRE2_H
//use both, 8 and 16 (r2007+)
# define PCRE2_CODE_UNIT_WIDTH 0
# include <pcre2.h>
#endif

#include "dwg.h"
#include "../src/logging.h"
#include "../src/common.h"
#include "../src/bits.h"
#include "suffix.inc"
static int help(void);
int verbosity(int argc, char **argv, int i, unsigned int *opts);
#include "common.inc"
#include "dwg_api.h"

#ifndef HAVE_PCRE2_H
# define PCRE2_DUPNAMES 0
# define PCRE2_CASELESS 1
# define PCRE2_EXTENDED 2
#endif

char *pattern;
char buf[4096];
int options = PCRE2_DUPNAMES;
int opt_count = 0;
int opt_text = 0;
int opt_tables = 0;
int opt_filename = 1;

/* the current version per spec block */
static unsigned int cur_ver = 0;

#ifdef HAVE_PCRE2_H
# undef USE_MATCH_CONTEXT
/* pcre2_compile */
static pcre2_code_8 *ri8;
static pcre2_match_data_8 *match_data8;
static pcre2_match_context_8 *match_context8 = NULL;
#ifdef HAVE_PCRE2_16
static pcre2_code_16 *ri16;
static pcre2_match_data_16 *match_data16;
static pcre2_match_context_16 *match_context16 = NULL;
#endif

# ifdef USE_MATCH_CONTEXT
static pcre2_jit_stack_8 *jit_stack8 = NULL;
static pcre2_compile_context_8 *compile_context8 = NULL;
#ifdef HAVE_PCRE2_16
static pcre2_jit_stack_16 *jit_stack16 = NULL;
static pcre2_compile_context_16 *compile_context16 = NULL;
#endif
# endif
# define PCRE2_JIT_MATCH_OPTIONS \
   (PCRE2_NO_UTF_CHECK|PCRE2_NOTBOL|PCRE2_NOTEOL|PCRE2_NOTEMPTY|\
    PCRE2_NOTEMPTY_ATSTART|PCRE2_PARTIAL_SOFT)
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
#if 0
  printf("  --type NAME               Search only NAME entities or objects.\n");
  printf("  --dxf NUM                 Search only DXF group NUM fields.\n");
#endif
  // for now only this:
  printf("  --text                    Search only in TEXT-like entities.\n");
  printf("  --tables                  Search only in table names.\n");
  printf("      --help                Display this help and exit\n");
  printf("      --version             Output version information and exit\n"
         "\n");
  printf("GNU LibreDWG online manual: <https://www.gnu.org/software/libredwg/>\n");
  return 0;
}

static int
do_match (int is16, char *filename, char *entity, int dxf, char* text)
{
#ifdef HAVE_PCRE2_H
  int found;
# ifdef HAVE_PCRE2_16
  if (is16)
    found = pcre2_jit_match_16(ri16, (PCRE2_SPTR16)text, PCRE2_ZERO_TERMINATED, 0,
                              PCRE2_JIT_MATCH_OPTIONS,
                              match_data16,     /* block for storing the result */
                              match_context16); /* disabled */
  else
# endif
  // converted to UTF-8 before
  found = pcre2_jit_match_8(ri8, (PCRE2_SPTR8)text, PCRE2_ZERO_TERMINATED, 0,
                                PCRE2_JIT_MATCH_OPTIONS,
                                match_data8,     /* block for storing the result */
                                match_context8); /* disabled */
  if (found >= 0) {
    if (!opt_count)
      printf("%s %s %d: %s\n", opt_filename ? filename : "", entity, dxf, text);
    return 1;
  }
#endif

#ifdef HAVE_PCRE2_16
  if (!is16)
  {
#endif
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
                  printf("%s %s %d: %s\n", opt_filename ? filename : "", entity, dxf, text);
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
            printf("%s %s %d: %s\n", opt_filename ? filename : "", entity, dxf, text);
          return 1;
        }
# endif
    }
  else
    {
      if (strstr(text, pattern)) {
        if (!opt_count)
          printf("%s %s %d: %s\n", opt_filename ? filename : "", entity, dxf, text);
        return 1;
      }
    }
#ifdef HAVE_PCRE2_16
  }
#endif

  return 0;
}

#ifdef HAVE_PCRE2_16
#define MATCH_TYPE(type,ENTITY,text_field,dxf)  \
  text = obj->tio.type->tio.ENTITY->text_field; \
  found += do_match(obj->parent->header.version >= R_2007, filename, #ENTITY, dxf, text)
#else
#define MATCH_TYPE(type,ENTITY,text_field,dxf)  \
  text = obj->tio.type->tio.ENTITY->text_field; \
  if (obj->parent->header.version >= R_2007) \
    text = bit_convert_TU((BITCODE_TU)text); \
  found += do_match(obj->parent->header.version >= R_2007, filename, #ENTITY, dxf, text); \
  if (obj->parent->header.version >= R_2007) \
    free(text)
#endif

#define MATCH_ENTITY(ENTITY,text_field,dxf) \
  MATCH_TYPE(entity,ENTITY,text_field,dxf)
#define MATCH_OBJECT(ENTITY,text_field,dxf) \
  MATCH_TYPE(object,ENTITY,text_field,dxf)
#define MATCH_TABLE(ENTITY, handle, TABLE, dxf) {}

static
int match_TEXT(char* filename, Dwg_Object* obj)
{
  char *text;
  int found = 0;
  MATCH_ENTITY (TEXT, text_value, 1);
  if (!opt_text)
    MATCH_TABLE (TEXT, style, STYLE, 7);
  return found;
}

static
int match_ATTRIB(char* filename, Dwg_Object* obj)
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
int match_ATTDEF(char* filename, Dwg_Object* obj)
{
  char *text;
  int found = 0;
  MATCH_ENTITY (ATTDEF, default_value, 1);
  MATCH_ENTITY (ATTDEF, tag, 2);
  MATCH_ENTITY (ATTDEF, prompt, 3);
  return found;
}

static
int match_BLOCK(char* filename, Dwg_Object* obj)
{
  char *text;
  int found = 0;
  MATCH_ENTITY (BLOCK, name, 2);
  return found;
}

static
int match_MTEXT(char* filename, Dwg_Object* obj)
{
  char *text;
  int found = 0;
  MATCH_ENTITY (MTEXT, text, 1);
  return found;
}

static
int match_DICTIONARY(char* filename, Dwg_Object* obj)
{
  char *text;
  int found = 0, i;
  Dwg_Object_DICTIONARY *_obj = obj->tio.object->tio.DICTIONARY;

  for (i=0; i<_obj->numitems; i++)
    {
      MATCH_OBJECT (DICTIONARY, text[i], 3);
    }
  return found;
}

static
int match_DICTIONARYVAR(char* filename, Dwg_Object* obj)
{
  char *text;
  int found = 0, textlen;
  MATCH_OBJECT (DICTIONARYVAR, str, 1);
  return found;
}

static
int match_IMAGEDEF(char* filename, Dwg_Object* obj)
{
  char *text;
  int found = 0, textlen;
  MATCH_OBJECT (IMAGEDEF, file_path, 1);
  return found;
}

static
int match_LAYOUT(char* filename, Dwg_Object* obj)
{
  char *text;
  int found = 0, i;
  Dwg_Object_LAYOUT *_obj = obj->tio.object->tio.LAYOUT;

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
int match_BLOCK_HEADER(char* filename, Dwg_Object_Ref* ref)
{
  int found = 0;
  Dwg_Object* obj;
  //Dwg_Object_BLOCK_HEADER* hdr;

  if (!ref || !ref->obj || !ref->obj->tio.object)
    return 0;
  //hdr = ref->obj->tio.object->tio.BLOCK_HEADER;
  obj = get_first_owned_object(ref->obj);
  while (obj)
    {
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
              else if (obj->fixedtype == DWG_TYPE_DICTIONARY)
                found += match_DICTIONARY(filename, obj);
              else if (obj->fixedtype == DWG_TYPE_DICTIONARYVAR)
                found += match_DICTIONARYVAR(filename, obj);
              else if (obj->type == DWG_TYPE_IMAGEDEF)
                found += match_IMAGEDEF(filename, obj);
              else if (obj->type == DWG_TYPE_LAYOUT)
                found += match_LAYOUT(filename, obj);
            }
        }
      if (!opt_text)
        {
          if (obj->supertype == DWG_SUPERTYPE_ENTITY)
            {
              //common entity names
              MATCH_TABLE (ENTITY, layer, LAYER, 8);
              MATCH_TABLE (ENTITY, ltype, LTYPE, 8);
              //r2000+
              MATCH_TABLE (ENTITY, plotstyle, PLOTSTYLE, 8);
              //r2007+
              MATCH_TABLE (ENTITY, material, MATERIAL, 8);
              MATCH_TABLE (ENTITY, shadow, DICTIONARY, 8);
              //r2010+
              MATCH_TABLE (ENTITY, full_visualstyle, VISUALSTYLE, 8);
              MATCH_TABLE (ENTITY, face_visualstyle, VISUALSTYLE, 8);
              MATCH_TABLE (ENTITY, edge_visualstyle, VISUALSTYLE, 8);
            }
        }
      obj = get_next_owned_object(ref->obj, obj);
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
  short dxf[10];
  char* objtype[10];
  short numdxf = 0;
  short numtype = 0;
  int plen;
  int errcode;
#ifdef HAVE_PCRE2_H
  PCRE2_SIZE erroffset;
  /* pcre_compile */
  int have_jit;
# ifdef HAVE_PCRE2_16
  BITCODE_TU pattern16;
# endif
#endif
  int opt_recurse = 0;
  int count = 0;

  // check args
  if (argc < 2)
    return usage();

  memset(dxf, 0, 10*sizeof(short));
  if (i < argc && !strcmp(argv[i], "-i"))
    {
      options |= PCRE2_CASELESS;
      i++;
    }
#ifdef HAVE_PCRE2_H
  if (i < argc && !strcmp(argv[i], "-x"))
    {
      options |= PCRE2_EXTENDED;
      i++;
    }
#endif
  if (i < argc && (!strcmp(argv[i], "--count") || !strcmp(argv[i], "-c")))
    {
      opt_count = 1;
      i++;
    }
  if (i < argc && (!strcmp(argv[i], "--no-filename") || !strcmp(argv[i], "-h")))
    {
      opt_filename = 0;
      i++;
    }
  if (i < argc && (!strcmp(argv[i], "--recursive") ||
                   !strcmp(argv[i], "-R") ||
                   !strcmp(argv[i], "-r")))
    {
      opt_recurse = 1;
      i++;
    }
  if (i < argc-1 && !strcmp(argv[i], "--type"))
    {
      if (numtype >= 10) return usage(); //too many
      objtype[numtype++] = argv[i+1];
      i += 2;
    }
  if (i < argc-1 && !strcmp(argv[i], "--dxf"))
    {
      if (numdxf >= 10) return usage(); // too many
      dxf[numdxf++] = (short)strtol(argv[i+1], NULL, 10);
      i += 2;
    }
  if (i < argc && !strcmp(argv[i], "--text"))
    {
      opt_text = 1;
      i++;
    }
  if (i < argc && !strcmp(argv[i], "--tables"))
    {
      opt_tables = 1;
      i++;
    }
  if (i < argc && !strcmp(argv[i], "--help"))
    return help();
  if (i < argc && !strcmp(argv[i], "--version"))
    return opt_version();

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
  match_data8 = pcre2_match_data_create_from_pattern_8(ri8, NULL);
  pcre2_jit_compile_8(ri8, PCRE2_JIT_COMPLETE); /* no partial matches */

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
  match_data16 = pcre2_match_data_create_from_pattern_16(ri16, NULL);
  pcre2_jit_compile_16(ri16, PCRE2_JIT_COMPLETE); /* no partial matches */
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

      for (k=0; k < dwg.block_control.num_entries; k++)
        {
          count += match_BLOCK_HEADER(filename, dwg.block_control.block_headers[k]);
        }
      count += match_BLOCK_HEADER(filename, dwg.block_control.model_space);
      count += match_BLOCK_HEADER(filename, dwg.block_control.paper_space);

      if (j < argc)
        dwg_free(&dwg); //skip the last free
    }
  if (opt_count)
    printf("%d\n", count);

  return count ? 0 : 1;
}
