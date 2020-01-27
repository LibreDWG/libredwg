/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2020 Free Software Foundation, Inc.                        */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * in_json.c: parse JSON via jsmn/jsmn.c
 * written by Reini Urban
 */

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "common.h"
#include "bits.h"
#include "dwg.h"
#include "decode.h"
#include "dynapi.h"
#include "in_dxf.h"
#include "in_json.h"

// not exported
#define JSMN_STATIC
// probably not needed in the final version (counting keys)
#define JSMN_PARENT_LINKS
// our files are bigger than 8000
// In strict mode primitive must be followed by "," or "}" or "]"; comma/object/array
// In strict mode an object or array can't become a key
// In strict mode primitives are: numbers and booleans
#undef JSMN_STRICT
#include "../jsmn/jsmn.h"

typedef struct jsmntokens {
  unsigned int index;
  jsmntok_t *tokens;
  unsigned int num_tokens;
} jsmntokens_t;

// synced with enum jsmntype_t
static const char *const t_typename[]
    = { "UNDEFINED", "OBJECT", "ARRAY", "STRING", "PRIMITIVE" };

static unsigned int loglevel;
#define DWG_LOGLEVEL loglevel
#include "logging.h"

/* the current version per spec block */
static unsigned int cur_ver = 0;
static char* created_by;
static Bit_Chain *g_dat;

#define MIN(X, Y) ((X) < (Y) ? (X) : (Y))

#define json_expect(tokens, typ) \
  if (tokens->tokens[tokens->index].type != JSMN_##typ) \
    return DWG_ERR_INVALIDTYPE

/*--------------------------------------------------------------------------------
 * MACROS
 */

#define ACTION injson
#define IS_ENCODE
#define IS_JSON

/******************************************************************/
#define _FIELD_FLOAT(nam, type)                                  \
      else if (strEQc (key, #nam))                               \
        {                                                        \
          _obj->nam = (BITCODE_##type)json_float (dat, tokens);  \
          LOG_TRACE (#nam ": " FORMAT_##type "\n", _obj->nam)    \
        }
#define _FIELD_LONG(nam, type)                                   \
      else if (strEQc (key, #nam))                               \
        {                                                        \
          _obj->nam = (BITCODE_##type)json_long (dat, tokens);   \
          LOG_TRACE (#nam ": " FORMAT_##type "\n", _obj->nam)    \
        }

#define FIELD_B(nam, dxf)   _FIELD_LONG (nam, B)
#define FIELD_BB(nam, dxf)  _FIELD_LONG (nam, BB)
#define FIELD_3B(nam, dxf)  _FIELD_LONG (nam, 3B)
#define FIELD_BS(nam, dxf)  _FIELD_LONG (nam, BS)
#define FIELD_BL(nam, dxf)  _FIELD_LONG (nam, BL)
#define FIELD_BLL(nam, dxf) _FIELD_LONG (nam, BLL)
#define FIELD_RC(nam, dxf)  _FIELD_LONG (nam, RC)
#define FIELD_RS(nam, dxf)  _FIELD_LONG (nam, RS)
#define FIELD_RL(nam, dxf)  _FIELD_LONG (nam, RL)
#define FIELD_RLL(nam, dxf) _FIELD_LONG (nam, RLL)
#define FIELD_MC(nam, dxf)  _FIELD_LONG (nam, MC)
#define FIELD_MS(nam, dxf)  _FIELD_LONG (nam, MS)

#define FIELD_BD(nam, dxf) _FIELD_FLOAT (nam, BD)
#define FIELD_RD(nam, dxf) _FIELD_FLOAT (nam, RD)

// advance until next known first-level type
// on OBJECT to end of OBJECT
// on ARRAY to end of ARRAY
// on STRING (key) get next
static int
json_advance_unknown (Bit_Chain *restrict dat, jsmntokens_t *restrict tokens,
                      int depth)
{
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  int error = 0;
  if (tokens->index >= tokens->num_tokens)
    {
      LOG_ERROR ("Unexpected end of JSON at %u of %u tokens",
                 tokens->index, tokens->num_tokens);
      return DWG_ERR_INVALIDDWG;
    }
  if (depth > 300)
    {
      LOG_ERROR ("JSON recursion limit");
      return DWG_ERR_INVALIDDWG;
    }
  LOG_TRACE ("Skip JSON %s %.*s at %u of %u tokens\n",
             t_typename[t->type], t->end - t->start, &dat->chain[t->start],
             tokens->index, tokens->num_tokens);
  switch (t->type)
    {
    case JSMN_OBJECT:
    case JSMN_ARRAY:
      tokens->index++;
      for (int i = 0; i < t->size; i++)
        {
          error |= json_advance_unknown (dat, tokens, depth + 1);
        }
      return error;
    case JSMN_STRING:
    case JSMN_PRIMITIVE:
      tokens->index++;
      return error;
    case JSMN_UNDEFINED:
    default:
      return error;
    }
  return error;
}

static void
json_fixed_key (char *key, Bit_Chain *restrict dat, jsmntokens_t *restrict tokens)
{
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  int len = t->end - t->start;
  *key = 0;
  if (t->type != JSMN_STRING)
    {
      LOG_ERROR ("Expected JSON STRING");
      json_advance_unknown (dat, tokens, 0);
      return;
    }
  if (len >= 80)
    {
      LOG_ERROR ("Expected JSON STRING");
      tokens->index++;
      return;
    }
  memcpy (key, &dat->chain[t->start], len);
  key[len] = '\0';
  tokens->index++;
  return;
}

ATTRIBUTE_MALLOC
static char *
json_string (Bit_Chain *restrict dat, jsmntokens_t *restrict tokens)
{
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  char *key;
  int len = t->end - t->start;
  if (t->type != JSMN_STRING)
    {
      LOG_ERROR ("Expected JSON STRING");
      json_advance_unknown (dat, tokens, 0);
      return NULL;
    }
  key = malloc (len + 1);
  memcpy (key, &dat->chain[t->start], len);
  key[len] = '\0';
  tokens->index++;
  return key;
}

static double
json_float (Bit_Chain *restrict dat, jsmntokens_t *restrict tokens)
{
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  //int len = t->end - t->start;
  if (t->type != JSMN_PRIMITIVE)
    {
      LOG_ERROR ("Expected JSON PRIMITIVE");
      json_advance_unknown (dat, tokens, 0);
      return (double)NAN;
    }
  tokens->index++;
  return strtod ((char *)&dat->chain[t->start], NULL);
}
static long
json_long (Bit_Chain *restrict dat, jsmntokens_t *restrict tokens)
{
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  //int len = t->end - t->start;
  if (t->type != JSMN_PRIMITIVE)
    {
      LOG_ERROR ("Expected JSON PRIMITIVE");
      json_advance_unknown (dat, tokens, 0);
      return 0;
    }
  tokens->index++;
  return strtol ((char *)&dat->chain[t->start], NULL, 10);
}

static void
json_3DPOINT (Bit_Chain *restrict dat, jsmntokens_t *restrict tokens,
              const char *restrict name, const char *restrict type,
              BITCODE_3DPOINT *restrict pt)
{
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  if (t->type != JSMN_ARRAY || t->size != 3)
    {
      LOG_ERROR ("JSON 3DPOINT must be ARRAY of size 3")
      return;
    }
  tokens->index++;
  pt->x = json_float (dat, tokens);
  pt->y = json_float (dat, tokens);
  pt->z = json_float (dat, tokens);
  LOG_TRACE ("%s (%f, %f, %f) [%s]\n", name, pt->x, pt->y, pt->z, type);
}

static void
json_2DPOINT (Bit_Chain *restrict dat, jsmntokens_t *restrict tokens,
              const char *restrict name, const char *restrict type,
              BITCODE_2DPOINT *restrict pt)
{
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  if (t->type != JSMN_ARRAY || t->size != 2)
    {
      LOG_ERROR ("JSON 2DPOINT must be ARRAY of size 2")
      return;
    }
  tokens->index++;
  pt->x = json_float (dat, tokens);
  pt->y = json_float (dat, tokens);
  LOG_TRACE ("%s (%f, %f) [%s]\n", name, pt->x, pt->y, type);
}

ATTRIBUTE_MALLOC
static BITCODE_H
json_HANDLE (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
             jsmntokens_t *restrict tokens, const char *name,
             const Dwg_Object *restrict obj)
{
  long code, value;
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  if (t->type != JSMN_ARRAY || t->size != 2)
    {
      LOG_ERROR ("JSON HANDLE must be ARRAY of [ code, value ]")
      return NULL;
    }
  tokens->index++;
  code = json_long (dat, tokens);
  value = json_long (dat, tokens);
  LOG_TRACE ("%s [%u, %u] [H]\n", name, (unsigned)code, (unsigned)value);
  return dwg_add_handleref (dwg, code, value, obj);
}

static void
json_CMC (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
          jsmntokens_t *restrict tokens, const char *name,
          Dwg_Color *restrict color)
{
  char key[80];
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  if (t->type == JSMN_OBJECT)
    { // 2004+
      tokens->tokens++; // hash of index, rgb...
      for (int j = 0; j < t->size; j++)
        {
          json_fixed_key (key, dat, tokens);
          if (strEQc (key, "index"))
            {
              long num = json_long (dat, tokens);
              LOG_TRACE ("%s.index %ld [CMC]\n", name, num);
              color->index = (BITCODE_BSd)num;
            }
          else if (strEQc (key, "rgb"))
            {
              char hex[80];
              json_fixed_key (hex, dat, tokens);
              LOG_TRACE ("%s.rgb %s [CMC]\n", name, hex);
              sscanf (hex, "%x", &color->rgb);
            }
          else if (strEQc (key, "flag"))
            {
              long num = json_long (dat, tokens);
              LOG_TRACE ("%s.flag %u [CMC]\n", name, (unsigned)num);
              color->flag = (BITCODE_BS)num;
            }
          else if (strEQc (key, "alpha"))
            {
              long num = json_long (dat, tokens);
              LOG_TRACE ("%s.alpha %u [CMC]\n", name, (unsigned)num);
              color->alpha = (BITCODE_RC)num;
              color->alpha_type = 3;
            }
          else if (strEQc (key, "handle")) // [4, value] ARRAY
            {
              color->handle = json_HANDLE (dat, dwg, tokens, name, NULL);
            }
          else if (strEQc (key, "name"))
            {
              char *str = json_string (dat, tokens);
              LOG_TRACE ("%s.name \"%s\" [CMC]\n", name, str);
              color->name = str;
            }
          else if (strEQc (key, "book_name"))
            {
              char *str = json_string (dat, tokens);
              LOG_TRACE ("%s.book_name \"%s\" [CMC]\n", name, str);
              color->book_name = str;
            }
          else
            {
              LOG_WARN ("Unknown key color.%s", key);
              tokens->index++;
            }
        }
    }
  else if (t->type == JSMN_PRIMITIVE)
    { // pre 2004
      long num = json_long (dat, tokens);
      LOG_TRACE ("%s.index %ld [CMC]\n", name, num);
      color->index = (BITCODE_BSd)num;
    }
}

static void
json_TIMEBLL (Bit_Chain *restrict dat,
              jsmntokens_t *restrict tokens, const char *name,
              BITCODE_TIMEBLL *date)
{
  unsigned long j = 1;
  double ms;
  double num = json_float (dat, tokens);
  date->value = num;
  date->days = (BITCODE_BL)trunc (num);
  ms = date->value;
  while (ms > 1.0)
    {
      j *= 10;
      ms /= 10.0;
    }
  date->ms = (BITCODE_BL) (j / 10 * (date->value - date->days));
  LOG_TRACE ("%s %f (%u, %u) [TIMEBLL]\n", name,
             date->value, date->days, date->ms);
}

static int
json_created_by (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                 jsmntokens_t *restrict tokens)
{
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  (void)dat; (void)dwg;
  if (t->type != JSMN_STRING)
    {
      LOG_ERROR ("Expected %s STRING", "created_by");
      json_advance_unknown (dat, tokens, 0);
      return DWG_ERR_INVALIDTYPE;
    }
  created_by = json_string (dat, tokens);
  tokens->index--; // advanced by the loop
  return 0;
}

static int
json_FILEHEADER (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                 jsmntokens_t *restrict tokens)
{
  const char *section = "FILEHEADER";
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  struct Dwg_Header *_obj = &dwg->header;
  Dwg_Object *obj = NULL;
  char version[80];
  int size = t->size;

  if (t->type != JSMN_OBJECT)
    {
      LOG_ERROR ("Expected %s OBJECT at %u of %u tokens, got %s", section,
                 tokens->index, tokens->num_tokens, t_typename[t->type]);
      json_advance_unknown (dat, tokens, 0);
      return DWG_ERR_INVALIDTYPE;
    }
  LOG_TRACE ("\n%s pos:%d [%d keys]\n--------------------\n", section, tokens->index, t->size);
  tokens->tokens++;
  // t = &tokens->tokens[tokens->index];
  // json_expect(tokens, STRING);
  // FIELD_TV (version, 0);
  for (int i = 0; i < size; i++)
    {
      char key[80];
      json_fixed_key (key, dat, tokens);
      t = &tokens->tokens[tokens->index];
      if (strEQc (key, "version"))
        {
          version[0] = '\0';
          json_fixed_key (version, dat, tokens);
          // set version's
          for (Dwg_Version_Type v = 0; v <= R_AFTER; v++)
            {
              if (strEQ (version, version_codes[v]))
                {
                  dwg->header.version = dwg->header.from_version = v;
                  dat->version = dat->from_version = dwg->header.version;
                  //is_utf = dat->version >= R_2007;
                  LOG_TRACE ("HEADER.version = dat->version = %s\n", version);
                  /*
                    if (is_utf && dwg->num_objects && dwg->object[0].fixedtype == DWG_TYPE_BLOCK_HEADER)
                    {
                    Dwg_Object_BLOCK_HEADER *_o = dwg->object[0].tio.object->tio.BLOCK_HEADER;
                    free (_o->name);
                    _o->name = (char*)bit_utf8_to_TU ((char*)"*Model_Space");
                    }
                  */
                  break;
                }
              if (v == R_AFTER)
                LOG_ERROR ("Invalid FILEHEADER.version %s", version);
            }
        }
      else if (strEQc (key, "zero_5") && t->type == JSMN_ARRAY)
        {
          tokens->index++;
          for (int j = 0; j < MIN(5, t->size); j++)
            {
              _obj->zero_5[j] = json_long (dat, tokens);
            }
        }

      FIELD_RC (is_maint, 0)
      FIELD_RC (zero_one_or_three, 0)
      FIELD_RL (thumbnail_address, 0) //@0x0d
      FIELD_RC (dwg_version, 0)
      FIELD_RC (maint_version, 0)
      FIELD_RS (codepage, 0) //@0x13: 29/30 for ANSI_1252, since r2007 UTF-16
      //SINCE (R_2004)
      FIELD_RC (unknown_0, 0)
      FIELD_RC (app_dwg_version, 0)
      FIELD_RC (app_maint_version, 0)
      FIELD_RL (security_type, 0)
      FIELD_RL (rl_1c_address, 0) /* mostly 0 */
      FIELD_RL (summaryinfo_address, 0)
      FIELD_RL (vbaproj_address, 0)
      FIELD_RL (rl_28_80, 0) /* mostly 128/0x80 */

      else if (strEQc (key, "HEADER"))
        {
          LOG_WARN ("Unexpected next section %s", key)
          tokens->tokens--;
          tokens->tokens--;
          return 0;
        }
      else
        {
          LOG_ERROR ("Unknown %s.%s ignored", section, key);
          tokens->index++;
        }
    }
  tokens->tokens--;
  return 0;
}

static int
json_HEADER (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
             jsmntokens_t *restrict tokens)
{
  const char *section = "HEADER";
  jsmntok_t *t = &tokens->tokens[tokens->index];
  Dwg_Header_Variables *_obj = &dwg->header_vars;
  Dwg_Object *obj = NULL;
  int size = t->size;
  int is_utf = 1;

  if (t->type != JSMN_OBJECT)
    {
      LOG_ERROR ("Expected %s OBJECT at %u of %u tokens, got %s", section,
                 tokens->index, tokens->num_tokens, t_typename[t->type]);
      json_advance_unknown (dat, tokens, 0);
      return DWG_ERR_INVALIDTYPE;
    }
  LOG_TRACE ("\n%s pos:%d [%d keys]\n--------------------\n", section, tokens->index, t->size);
  tokens->tokens++;
  for (int i = 0; i < size; i++)
    {
      char key[80];
      Dwg_DYNAPI_field *f;
      json_fixed_key (key, dat, tokens);
      t = &tokens->tokens[tokens->index];
      f = (Dwg_DYNAPI_field *)dwg_dynapi_header_field (key);
      if (!f)
        {
          LOG_WARN ("Unknown key HEADER.%s", key)
          json_advance_unknown (dat, tokens, 0);
          continue;
        }
      else if (t->type == JSMN_PRIMITIVE
               && (strEQc (f->type, "BD") || strEQc (f->type, "RD")))
        {
          double num = json_float (dat, tokens);
          LOG_TRACE ("%s: " FORMAT_RD " [%s]\n", key, num, f->type)
          dwg_dynapi_header_set_value (dwg, key, &num, 0);
        }
      else if (t->type == JSMN_PRIMITIVE
               && (strEQc (f->type, "RC") || strEQc (f->type, "B")
                   || strEQc (f->type, "BB") || strEQc (f->type, "RS")
                   || strEQc (f->type, "BS") || strEQc (f->type, "RL")
                   || strEQc (f->type, "BL") || strEQc (f->type, "RLL")
                   || strEQc (f->type, "BLd") || strEQc (f->type, "BSd")
                   || strEQc (f->type, "BLL")))
        {
          long num = json_long (dat, tokens);
          LOG_TRACE ("%s: %ld [%s]\n", key, num, f->type)
          dwg_dynapi_header_set_value (dwg, key, &num, 0);
        }
      else if (t->type == JSMN_STRING
               && (strEQc (f->type, "TV") || strEQc (f->type, "T")))
        {
          char *str = json_string (dat, tokens);
          LOG_TRACE ("%s: \"%s\" [%s]\n", key, str, f->type)
          dwg_dynapi_header_set_value (dwg, key, &str, 1);
        }
      else if (t->type == JSMN_ARRAY
               && (strEQc (f->type, "3BD") || strEQc (f->type, "3RD")
                   || strEQc (f->type, "3DPOINT") || strEQc (f->type, "BE")
                   || strEQc (f->type, "3BD_1")))
        {
          BITCODE_3DPOINT pt;
          json_3DPOINT (dat, tokens, key, f->type, &pt);
          dwg_dynapi_header_set_value (dwg, key, &pt, 1);
        }
      else if (t->type == JSMN_ARRAY
               && (strEQc (f->type, "2BD") || strEQc (f->type, "2RD")
                   || strEQc (f->type, "2DPOINT")
                   || strEQc (f->type, "2BD_1")))
        {
          BITCODE_2DPOINT pt;
          json_2DPOINT (dat, tokens, key, f->type, &pt);
          dwg_dynapi_header_set_value (dwg, key, &pt, 1);
        }
      else if (t->type == JSMN_PRIMITIVE && strEQc (f->type, "TIMEBLL"))
        {
          static BITCODE_TIMEBLL date = { 0, 0, 0 };
          json_TIMEBLL (dat, tokens, key, &date);
          dwg_dynapi_header_set_value (dwg, key, &date, 0);
        }
      else if (strEQc (f->type, "CMC"))
        {
          static BITCODE_CMC color = { 0, 0, 0 };
          json_CMC (dat, dwg, tokens, key, &color);
          dwg_dynapi_header_set_value (dwg, key, &color, 0);
        }
      else if (t->type == JSMN_ARRAY && strEQc (f->type, "H"))
        {
          BITCODE_H hdl = json_HANDLE (dat, dwg, tokens, key, NULL);
          dwg_dynapi_header_set_value (dwg, key, &hdl, 0);
        }
      //...
      else if (t->type == JSMN_OBJECT && strEQc (key, "CLASSES"))
        {
          LOG_WARN ("Unexpected next section %s", key)
          tokens->tokens--;
          tokens->tokens--;
          return 0;
        }
      else
        {
          LOG_WARN ("Unhandled %s [%s] with %s", key, f->type, t_typename[t->type])
          tokens->tokens++;
          continue;
        }
    }
  // the key
  tokens->tokens--;
  return 0;
}

static int
json_CLASSES (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
             jsmntokens_t *restrict tokens)
{
  const char *section = "CLASSES";
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  int size;
  if (t->type != JSMN_ARRAY)
    {
      LOG_ERROR ("Expected %s ARRAY at %u of %u tokens, got %s", section,
                 tokens->index, tokens->num_tokens, t_typename[t->type]);
      json_advance_unknown (dat, tokens, 0);
      return DWG_ERR_INVALIDTYPE;
    }
  size = t->size;
  LOG_TRACE ("\n%s pos:%d [%d members]\n--------------------\n", section, tokens->index, size);
  tokens->tokens++;
  if (dwg->num_classes == 0)
    dwg->dwg_class = calloc (size, sizeof (Dwg_Class));
  else
    dwg->dwg_class = realloc (dwg->dwg_class, (dwg->num_classes + size)
                                                  * sizeof (Dwg_Class));
  if (!dwg->dwg_class)
    {
      LOG_ERROR ("Out of memory");
      return DWG_ERR_OUTOFMEM;
    }
  dwg->num_classes += size;
  for (int i = 0; i < size; i++)
    {
      int keys;
      Dwg_Class *klass = &dwg->dwg_class[i];
      memset (klass, 0, sizeof (Dwg_Class));
      t = &tokens->tokens[tokens->index];
      if (t->type != JSMN_OBJECT)
        {
          LOG_ERROR ("Expected %s OBJECT at %u of %u tokens, got %s. %s:%d", section,
                     tokens->index, tokens->num_tokens, t_typename[t->type],
                     __FUNCTION__, __LINE__);
          json_advance_unknown (dat, tokens, 0);
          return DWG_ERR_INVALIDTYPE;
        }
      keys = t->size;
      tokens->index++;
      for (int j = 0; j < keys; j++)
        {
          char key[80];
          json_fixed_key (key, dat, tokens);
          t = &tokens->tokens[tokens->index];
          if (strEQc (key, "number"))
            {
              klass->number = json_long (dat, tokens);
              LOG_TRACE ("\nCLASS[%d].number: %d\n", i, klass->number)
              if (klass->number != i + 500)
                LOG_WARN ("Possibly illegal class number %d, expected %d",
                          klass->number, i + 500)
            }
          else if (strEQc (key, "dxfname"))
            {
              klass->dxfname = json_string (dat, tokens);
              LOG_TRACE ("dxfname: \"%s\"\n", klass->dxfname)
            }
          else if (strEQc (key, "cppname"))
            {
              klass->cppname = json_string (dat, tokens);
              LOG_TRACE ("cppname: \"%s\"\n", klass->cppname)
            }
          else if (strEQc (key, "appname"))
            {
              klass->appname = json_string (dat, tokens);
              LOG_TRACE ("appname \"%s\"\n", klass->appname)
            }
          else if (strEQc (key, "proxyflag"))
            {
              klass->proxyflag = json_long (dat, tokens);
              LOG_TRACE ("proxyflag %d\n", klass->proxyflag)
            }
          else if (strEQc (key, "num_instances"))
            {
              klass->num_instances = json_long (dat, tokens);
              LOG_TRACE ("num_instances %d\n", klass->num_instances)
            }
          else if (strEQc (key, "is_zombie"))
            {
              klass->is_zombie = json_long (dat, tokens);
              LOG_TRACE ("is_zombie %d\n", klass->is_zombie)
            }
          else if (strEQc (key, "item_class_id"))
            {
              klass->item_class_id = json_long (dat, tokens);
              LOG_TRACE ("item_class_id %d\n", klass->item_class_id)
            }
          else
            {
              LOG_TRACE ("Unknown CLASS key %s %.*s\n", key, t->end - t->start,
                         &dat->chain[t->start])
              ++tokens->index;
            }
        }
    }
  tokens->index--;
  return 0;
}

// needs to be recursive, for search in subclasses
static int
_set_struct_field (Bit_Chain *restrict dat, const Dwg_Object *restrict obj,
                   jsmntokens_t *restrict tokens,
                   void *restrict _obj,
                   const char *restrict name, const char *restrict key,
                   const Dwg_DYNAPI_field *restrict fields)
{
  Dwg_Data *restrict dwg = obj->parent;
  const Dwg_DYNAPI_field *f;
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  LOG_INSANE ("-search %s.%s: %s %.*s\n", name, key, t_typename[t->type],
              t->end - t->start, &dat->chain[t->start]);
  for (f = &fields[0]; f->name; f++) // linear search, not binary for now
    {
      //LOG_INSANE ("-%s.%s [%s]\n", name, f->name, f->type);
      // common and entity dynapi, check types
      if (strEQ (f->name, key)) // found
        {
          LOG_INSANE ("-found %s [%s] %s\n", f->name, f->type, t_typename[t->type]);
          if (t->type == JSMN_PRIMITIVE
               && (strEQc (f->type, "BD") || strEQc (f->type, "RD")
                   || strEQc (f->type, "BT")))
            {
              double num = json_float (dat, tokens);
              LOG_TRACE ("%s: " FORMAT_RD " [%s]\n", key, num, f->type);
              dwg_dynapi_field_set_value (dwg, _obj, f, &num, 0);
            }
          else if (t->type == JSMN_PRIMITIVE
                   && (strEQc (f->type, "RC") || strEQc (f->type, "B")
                       || strEQc (f->type, "BB") || strEQc (f->type, "RS")
                       || strEQc (f->type, "BS") || strEQc (f->type, "RL")
                       || strEQc (f->type, "BL") || strEQc (f->type, "RLL")
                       || strEQc (f->type, "BLd") || strEQc (f->type, "BSd")
                       || strEQc (f->type, "BLL")))
            {
              long num = json_long (dat, tokens);
              LOG_TRACE ("%s: %ld [%s]\n", key, num, f->type);
              dwg_dynapi_field_set_value (dwg, _obj, f, &num, 0);
            }
          else if (t->type == JSMN_STRING
                   && (strEQc (f->type, "TV") || strEQc (f->type, "T")
                       || strEQc (f->type, "TF") || strEQc (f->type, "TU")))
            {
              char *str = json_string (dat, tokens);
              int len;
              if (f->dxf == 310 && (len = strlen (str))) // is BINARY
                {
                  // convert from hex
                  unsigned blen = len / 2;
                  char *buf = malloc (blen + 1);
                  char *pos = str;
                  char *old;
                  for (unsigned i = 0; i < blen; i++)
                    {
                      sscanf (pos, "%2hhX", &buf[i]);
                      pos += 2;
                    }
                  buf[blen] = '\0';
                  LOG_TRACE ("%s: '%.*s' [%s] (binary)\n", key, blen, buf, f->type);
                  // set the ptr directly, no alloc, no conversion.
                  old = &((char*)_obj)[f->offset];
                  memcpy (old, &buf, f->size);
                }
              else
                {
                  LOG_TRACE ("%s: \"%s\" [%s]\n", key, str, f->type);
                  dwg_dynapi_field_set_value (dwg, _obj, f, &str, 1);
                }
              free (str);
            }
          else if (t->type == JSMN_ARRAY
                   && (strEQc (f->type, "3BD") || strEQc (f->type, "3RD")
                       || strEQc (f->type, "3DPOINT") || strEQc (f->type, "BE")
                       || strEQc (f->type, "3BD_1")))
            {
              BITCODE_3DPOINT pt;
              json_3DPOINT (dat, tokens, key, f->type, &pt);
              dwg_dynapi_field_set_value (dwg, _obj, f, &pt, 1);
            }
          else if (t->type == JSMN_ARRAY
                   && (strEQc (f->type, "2BD") || strEQc (f->type, "2RD")
                       || strEQc (f->type, "2DPOINT")
                       || strEQc (f->type, "2BD_1")))
            {
              BITCODE_2DPOINT pt;
              json_2DPOINT (dat, tokens, key, f->type, &pt);
              dwg_dynapi_field_set_value (dwg, _obj, f, &pt, 1);
            }
          else if (t->type == JSMN_PRIMITIVE && strEQc (f->type, "TIMEBLL"))
            {
              static BITCODE_TIMEBLL date = { 0, 0, 0 };
              json_TIMEBLL (dat, tokens, key, &date);
              dwg_dynapi_field_set_value (dwg, _obj, f, &date, 1);
            }
          else if (strEQc (f->type, "CMC"))
            {
              static BITCODE_CMC color = { 0, 0, 0 };
              json_CMC (dat, dwg, tokens, key, &color);
              dwg_dynapi_field_set_value (dwg, _obj, f, &color, 1);
            }
          else if (t->type == JSMN_ARRAY && strEQc (f->type, "H"))
            {
              BITCODE_H hdl = json_HANDLE (dat, dwg, tokens, key, obj);
              dwg_dynapi_field_set_value (dwg, _obj, f, &hdl, 1);
            }
          else if (t->type == JSMN_ARRAY && strEQc (f->type, "H*"))
            {
              int size1 = t->size;
              BITCODE_H *hdls = calloc (size1, sizeof (BITCODE_H));
              tokens->index++;
              for (int k = 0; k < size1; k++)
                {
                  hdls[k] = json_HANDLE (dat, dwg, tokens, key, obj);
                }
              dwg_dynapi_field_set_value (dwg, _obj, f, &hdls, 1);
            }
          else if (t->type == JSMN_ARRAY && strEQc (f->type, "3DPOINT*"))
            {
              int size1 = t->size;
              BITCODE_3DPOINT *pts = calloc (size1, sizeof (BITCODE_3BD));
              tokens->index++;
              for (int k = 0; k < size1; k++)
                {
                  json_3DPOINT (dat, tokens, key, f->type, &pts[k]);
                }
              dwg_dynapi_field_set_value (dwg, _obj, f, &pts, 1);
            }
          else if (t->type == JSMN_ARRAY && strEQc (f->type, "2RD*"))
            {
              int size1 = t->size;
              BITCODE_2DPOINT *pts = calloc (size1, sizeof (BITCODE_2RD));
              tokens->index++;
              for (int k = 0; k < size1; k++)
                {
                  json_2DPOINT (dat, tokens, key, f->type, &pts[k]);
                }
              dwg_dynapi_field_set_value (dwg, _obj, f, &pts, 1);
            }
          else if (t->type == JSMN_ARRAY && strEQc (f->type, "BD*"))
            {
              int size1 = t->size;
              BITCODE_BD *nums = calloc (size1, sizeof (BITCODE_BD));
              tokens->index++;
              for (int k = 0; k < size1; k++)
                {
                  nums[k] = json_float (dat, tokens);
                }
              dwg_dynapi_field_set_value (dwg, _obj, f, &nums, 1);
            }
          else if (t->type == JSMN_ARRAY && strEQc (f->type, "TV*"))
            {
              int size1 = t->size;
              BITCODE_TV *elems = calloc (size1, sizeof (BITCODE_TV));
              tokens->index++;
              for (int k = 0; k < size1; k++)
                {
                  elems[k] = json_string (dat, tokens);
                }
              dwg_dynapi_field_set_value (dwg, _obj, f, &elems, 1);
            }
          // subclass arrays:
          else if (t->type == JSMN_ARRAY && memBEGINc (f->type, "Dwg_"))
            {
              int num_elems = t->size;
              int size_elem;
              char *elems;
              const Dwg_DYNAPI_field *sfields;
              // strip off Dwg_ and final *
              // e.g. 'Dwg_MLINESTYLE_line*' => 'MLINESTYLE_line'
              char *subclass = dwg_dynapi_subclass_name (f->type);
              if (!subclass)
                {
                  LOG_ERROR ("Unknown subclass type %s", f->type);
                  goto unknown_ent;
                }
              size_elem = dwg_dynapi_fields_size (subclass);
              sfields = dwg_dynapi_subclass_fields (subclass);
              if (!size_elem || !sfields)
                {
                  LOG_ERROR ("Unknown subclass name %s", subclass);
                  free (subclass);
                  goto unknown_ent;
                }
              elems = calloc (num_elems, size_elem);
              tokens->index++;
              // array of structs
              for (int k = 0; k < num_elems; k++)
                {
                  int keys;
                  t = &tokens->tokens[tokens->index];
                  if (t->type != JSMN_OBJECT)
                    {
                      LOG_ERROR ("Expected %s OBJECT at %u of %u tokens, got %s. %s:%d", subclass,
                                 tokens->index, tokens->num_tokens, t_typename[t->type],
                                 __FUNCTION__, __LINE__);
                      json_advance_unknown (dat, tokens, 0);
                      return DWG_ERR_INVALIDTYPE;
                    }
                  keys = t->size;
                  tokens->index++;
                  for (int ki = 0; ki < keys; ki++)
                    {
                      // seperate subclass type loop
                      const Dwg_DYNAPI_field *f1;
                      char key1[80];
                      json_fixed_key (key1, dat, tokens);
                      LOG_INSANE ("-search %s.%s\n", subclass, key1);
                      for (f1 = &sfields[0]; f1->name; f1++) // linear search, not binary
                        {
                          //LOG_INSANE ("-%s.%s [%s]\n", subclass, f1->name, f1->type);
                          if (strEQ (f1->name, key1)) // found
                            {
                              LOG_INSANE ("-found %s [%s]\n", f1->name, f1->type);
                              if (_set_struct_field (dat, obj, tokens, &elems[k], subclass, key1,
                                                     sfields))
                                break;
                            }
                        }
                      if (!f1->name && strEQc (key, "lt.index"))
                        {
                          if (!_set_struct_field (dat, obj, tokens, &elems[k], subclass, "lt",
                                                 sfields))
                            ++tokens->index;
                        }
                      else if (!f1->name) // not found
                        {
                          LOG_ERROR ("Unknown subclass field %s.%s", subclass, key1);
                          ++tokens->index;
                        }
                    }
                }
              free (subclass);
              dwg_dynapi_field_set_value (dwg, _obj, f, &elems, 1);
            }
          else
            {
            unknown_ent:
              LOG_ERROR ("Unknown type for %s.%s %s with %s", name, key, f->type,
                         t_typename[t->type]);
              ++tokens->index;
            }
          break;
        }
    }
  return f->name ? 1 : 0; // found or not
}

static int
json_OBJECTS (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
             jsmntokens_t *restrict tokens)
{
  const char *section = "OBJECTS";
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  int size;
  if (t->type != JSMN_ARRAY)
    {
      LOG_ERROR ("Expected %s ARRAY at %u of %u tokens, got %s", section,
                 tokens->index, tokens->num_tokens, t_typename[t->type]);
      json_advance_unknown (dat, tokens, 0);
      return DWG_ERR_INVALIDTYPE;
    }
  size = t->size;
  LOG_TRACE ("\n%s pos:%d [%d members]\n--------------------\n", section, tokens->index, size);
  tokens->tokens++;
  if (dwg->num_objects == 0)
    dwg->object = calloc (size, sizeof (Dwg_Object));
  else
    {
      dwg->object = realloc (dwg->object, (dwg->num_objects + size)
                             * sizeof (Dwg_Object));
      memset (&dwg->object[dwg->num_objects], 0, size);
    }
  if (!dwg->object)
    {
      LOG_ERROR ("Out of memory");
      return DWG_ERR_OUTOFMEM;
    }
  dwg->num_objects += size;
  for (int i = 0; i < size; i++)
    {
      char name[80];
      int keys;
      int is_entity = 0;
      Dwg_Object *obj = &dwg->object[i];
      Dwg_Object_APPID *_obj = NULL;
      const Dwg_DYNAPI_field *fields, *cfields;
      const Dwg_DYNAPI_field *f;

      memset (obj, 0, sizeof (Dwg_Object));
      t = &tokens->tokens[tokens->index];
      if (t->type != JSMN_OBJECT)
        {
          LOG_ERROR ("Expected %s OBJECT at %u of %u tokens, got %s. %s:%d", section,
                     tokens->index, tokens->num_tokens, t_typename[t->type],
                     __FUNCTION__, __LINE__);
          json_advance_unknown (dat, tokens, 0);
          return DWG_ERR_INVALIDTYPE;
        }
      keys = t->size;
      LOG_INSANE ("-keys: %d\n", keys);

      tokens->index++;
      for (int j = 0; j < keys; j++)
        {
          char key[80];
          json_fixed_key (key, dat, tokens);
          t = &tokens->tokens[tokens->index];
          if (strEQc (key, "object") && t->type == JSMN_STRING
              && i < (int)dwg->num_objects && !dwg->object[i].type)
            {
              int len = t->end - t->start;
              int objsize = 16;
              memcpy (name, &dat->chain[t->start], len);
              name[len] = '\0';
              is_entity = 0;
              fields = dwg_dynapi_entity_fields (name);
              // TODO: add tests that all calculated sizes are correct
              objsize = dwg_dynapi_fields_size (name);
              if (!fields || !objsize)
                {
                  LOG_ERROR ("Unknown object or entity %s", name);
                  json_advance_unknown (dat, tokens, 0);
                  break;
                }
              LOG_TRACE ("\nnew object %s [%d] (size: %d)\n", name, i, objsize);
              obj->supertype = DWG_SUPERTYPE_OBJECT;
              obj->parent = dwg;
              obj->index = i;
              obj->tio.object = calloc (1, sizeof (Dwg_Object_Object));
              obj->tio.object->dwg = dwg;
              obj->tio.object->objid = i;
              // NEW_OBJECT (dwg, obj)
              // ADD_OBJECT loop?
              _obj = calloc (1, objsize);
              obj->tio.object->tio.APPID = _obj;
              obj->tio.object->tio.APPID->parent = obj->tio.object;
              obj->name = strdup (name);
              // TODO alias
              obj->dxfname = strdup (name);
              tokens->index++;
            }
          else if (strEQc (key, "entity") && t->type == JSMN_STRING
                   && i < (int)dwg->num_objects && !dwg->object[i].type)
            {
              int len = t->end - t->start;
              int objsize;
              memcpy (name, &dat->chain[t->start], len);
              name[len] = '\0';
              is_entity = 1;
              fields = dwg_dynapi_entity_fields (name);
              objsize = dwg_dynapi_fields_size (name);
              if (!fields || !objsize)
                {
                  LOG_ERROR ("Unknown object or entity %s", name);
                  json_advance_unknown (dat, tokens, 0);
                  break;
                }
              LOG_TRACE ("\nnew entity %s [%d] (size: %d)\n", name, i, objsize);
              obj->supertype = DWG_SUPERTYPE_ENTITY;
              obj->parent = dwg;
              obj->index = i;
              obj->tio.entity = calloc (1, sizeof (Dwg_Object_Entity));
              obj->tio.entity->dwg = dwg;
              obj->tio.entity->objid = i;
              // NEW_ENTITY (dwg, obj)
              // ADD_ENTITY loop?
              _obj = calloc (1, objsize);
              obj->tio.entity->tio.POINT = (Dwg_Entity_POINT*)_obj;
              obj->tio.entity->tio.POINT->parent = obj->tio.entity;
              obj->name = strdup (name);
              // if different, the alias is done via extra dxfname key (below)
              obj->dxfname = strdup (name);
              tokens->index++;
            }
          else if (!obj || !fields)
            {
              LOG_ERROR ("Required object or entity key missing");
              json_advance_unknown (dat, tokens, 0);
              return DWG_ERR_INVALIDDWG;
            }
          else if (strEQc (key, "dxfname"))
            {
              free (obj->dxfname);
              obj->dxfname = json_string (dat, tokens);
              LOG_TRACE ("dxfname: %s\n", obj->dxfname)
            }
          else if (strEQc (key, "index")) // TODO not TableCellContent_Attr
            {
              BITCODE_RL index = (BITCODE_RL)json_long (dat, tokens);
              if (obj->index != index)
                LOG_WARN ("Unexpected %s.index %d, expected %d", name, index, obj->index)
              obj->index = index;
              if (is_entity)
                obj->tio.entity->objid = obj->index;
              else
                obj->tio.object->objid = obj->index;
              LOG_TRACE ("index: %d\n", obj->index)
            }
          else if (strEQc (key, "type") && !obj->type)
            {
              obj->type = json_long (dat, tokens);
              if (obj->type < 500)
                obj->fixedtype = obj->type;
              else
                {
                  // clang-format off
                  // ADD_ENTITY by name
                  // check all objects
#undef DWG_OBJECT
#define DWG_OBJECT(token)                       \
  if (strEQc (name, #token))                    \
    {                                           \
      obj->fixedtype = DWG_TYPE_##token;        \
      goto found_ent;                           \
    }                                           \
  else
#undef DWG_ENTITY
#define DWG_ENTITY(token) DWG_OBJECT(token)

                  #include "objects.inc"
                  //final else
                  LOG_WARN ("Unknown object %s", name);

#undef DWG_OBJECT
#undef DWG_ENTITY
                  // clang-format on
                }
            found_ent:
              LOG_TRACE ("type: %d,\tfixedtype: %d\n", obj->type, obj->fixedtype)
            }
          // Note: also _obj->size
          else if (strEQc (key, "size") && !obj->size && t->type == JSMN_PRIMITIVE)
            {
              obj->size = json_long (dat, tokens);
              LOG_TRACE ("size: %d\n", obj->size)
            }
          else if (strEQc (key, "bitsize") && !obj->bitsize)
            {
              obj->bitsize = json_long (dat, tokens);
              LOG_TRACE ("bitsize: %d\n", obj->bitsize)
            }
          else if (strEQc (key, "handle") && !obj->handle.value)
            {
              BITCODE_H hdl = json_HANDLE (dat, dwg, tokens, key, obj);
              obj->handle.code = hdl->handleref.code;
              obj->handle.size = hdl->handleref.size;
              obj->handle.value = hdl->handleref.value;
            }
          else if (strEQc (key, "unknown") && memBEGINc (obj->name, "UNKNOWN_"))
            {
              obj->unknown_bits = json_string (dat, tokens);
              LOG_TRACE ("unknown_bits: %.*s\n", t->end - t->start, &dat->chain[t->start])
            }
          else
            //search_field:
            {
              if (_set_struct_field (dat, obj, tokens, _obj, name, key, fields))
                continue;
              if (is_entity)
                {
                  if (_set_struct_field (dat, obj, tokens, obj->tio.entity, name, key,
                                         dwg_dynapi_common_entity_fields ()))
                    continue;
                }
              else
                {
                  if (_set_struct_field (dat, obj, tokens, obj->tio.object, name, key,
                                         dwg_dynapi_common_object_fields ()))
                    continue;
                }
              if (strEQc (name, "MULTILEADER"))
                {
                  // embedded structs
                  if (memBEGINc (key, "ctx.content."))
                    {
                      Dwg_Entity_MULTILEADER *_o = (Dwg_Entity_MULTILEADER*)_obj;
                      Dwg_MLEADER_Content *cnt = &_o->ctx.content;
                      // FIXME: add to dynapi: txt.* and blk.* MLEADER_Content union fields
                      const Dwg_DYNAPI_field *sf
                        = dwg_dynapi_subclass_fields ("MLEADER_Content");
                      if (sf && _set_struct_field (dat, obj, tokens, cnt, "MLEADER",
                                                   &key[strlen("ctx.content.")], sf))
                        continue;
                    }
                  else if (memBEGINc (key, "ctx."))
                    {
                      Dwg_Entity_MULTILEADER *_o = (Dwg_Entity_MULTILEADER*)_obj;
                      Dwg_MLEADER_AnnotContext *ctx = &_o->ctx;
                      const Dwg_DYNAPI_field *sf
                        = dwg_dynapi_subclass_fields ("MLEADER_AnnotContext");
                      if (sf && _set_struct_field (dat, obj, tokens, ctx, "MLEADER",
                                                   &key[4], sf))
                        continue;
                    }
                }
              LOG_TRACE ("Unknown %s.%s %.*s\n", name, key,
                         t->end - t->start, &dat->chain[t->start]);
              json_advance_unknown (dat, tokens, 0);
            }
        }
    }
  tokens->index--;
  return 0;
}

static int
json_HANDLES (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
              jsmntokens_t *restrict tokens)
{
  const char *section = "HANDLES";
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  int size;
  if (t->type != JSMN_ARRAY)
    {
      LOG_ERROR ("Expected %s ARRAY at %u of %u tokens, got %s", section,
                 tokens->index, tokens->num_tokens, t_typename[t->type]);
      json_advance_unknown (dat, tokens, 0);
      return DWG_ERR_INVALIDTYPE;
    }
  size = t->size;
  LOG_TRACE ("\n%s pos:%d [%d members]\n--------------------\n", section,
             tokens->index, size);
  // Maybe create dwg->header.section[SECTION_HANDLES_R13] omap here.
  // struct { uint32_t hdloff; int32_t offset } *omap = calloc (size, 8);
  for (int i = 0; i < size; i++)
    {
      tokens->tokens++;
      for (int k = 0; k < 2; k++)
        {
          long hdloff = json_long (dat, tokens);
          long offset = json_long (dat, tokens);
          tokens->tokens++;
        }
    }
  return 0;
}

static int
json_THUMBNAILIMAGE (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                   jsmntokens_t *restrict tokens)
{
  const char *section = "THUMBNAILIMAGE";
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  (void)dat; (void)dwg;
  if (t->type != JSMN_OBJECT)
    {
      LOG_ERROR ("Expected %s OBJECT at %u of %u tokens, got %s", section,
                 tokens->index, tokens->num_tokens, t_typename[t->type]);
      json_advance_unknown (dat, tokens, 0);
      return DWG_ERR_INVALIDTYPE;
    }
  // ... dynapi
  return -1;
}

static int
json_R2004_Header (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                   jsmntokens_t *restrict tokens)
{
  const char *section = "R2004_Header";
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  (void)dat; (void)dwg;
  if (t->type != JSMN_OBJECT)
    {
      LOG_ERROR ("Expected %s OBJECT at %u of %u tokens, got %s", section,
                 tokens->index, tokens->num_tokens, t_typename[t->type]);
      json_advance_unknown (dat, tokens, 0);
      return DWG_ERR_INVALIDTYPE;
    }
  // ... dynapi
  return -1;
}

static int
json_SummaryInfo (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                  jsmntokens_t *restrict tokens)
{
  const char *section = "SummaryInfo";
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  (void)dat; (void)dwg;
  if (t->type != JSMN_OBJECT)
    {
      LOG_ERROR ("Expected %s OBJECT at %u of %u tokens, got %s", section,
                 tokens->index, tokens->num_tokens, t_typename[t->type]);
      json_advance_unknown (dat, tokens, 0);
      return DWG_ERR_INVALIDTYPE;
    }
  // ... dynapi
  return -1;
}

static int
json_AppInfo (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                  jsmntokens_t *restrict tokens)
{
  const char *section = "AppInfo";
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  (void)dat; (void)dwg;
  if (t->type != JSMN_OBJECT)
    {
      LOG_ERROR ("Expected %s OBJECT at %u of %u tokens, got %s", section,
                 tokens->index, tokens->num_tokens, t_typename[t->type]);
      json_advance_unknown (dat, tokens, 0);
      return DWG_ERR_INVALIDTYPE;
    }
  // ... dynapi
  return -1;
}

static int
json_AppInfoHistory (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                  jsmntokens_t *restrict tokens)
{
  const char *section = "AppInfoHistory";
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  (void)dat; (void)dwg;
  if (t->type != JSMN_OBJECT)
    {
      LOG_ERROR ("Expected %s OBJECT at %u of %u tokens, got %s", section,
                 tokens->index, tokens->num_tokens, t_typename[t->type]);
      json_advance_unknown (dat, tokens, 0);
      return DWG_ERR_INVALIDTYPE;
    }
  // ... dynapi
  return -1;
}

static int
json_FileDepList (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                  jsmntokens_t *restrict tokens)
{
  const char *section = "FileDepList";
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  (void)dat; (void)dwg;
  if (t->type != JSMN_OBJECT)
    {
      LOG_ERROR ("Expected %s OBJECT at %u of %u tokens, got %s", section,
                 tokens->index, tokens->num_tokens, t_typename[t->type]);
      json_advance_unknown (dat, tokens, 0);
      return DWG_ERR_INVALIDTYPE;
    }
  // ... dynapi
  return -1;
}

static int
json_Security (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                  jsmntokens_t *restrict tokens)
{
  const char *section = "Security";
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  (void)dat; (void)dwg;
  if (t->type != JSMN_OBJECT)
    {
      LOG_ERROR ("Expected %s OBJECT at %u of %u tokens, got %s", section,
                 tokens->index, tokens->num_tokens, t_typename[t->type]);
      json_advance_unknown (dat, tokens, 0);
      return DWG_ERR_INVALIDTYPE;
    }
  // ... dynapi
  return -1;
}

static int
json_RevHistory (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                  jsmntokens_t *restrict tokens)
{
  const char *section = "RevHistory";
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  (void)dat; (void)dwg;
  if (t->type != JSMN_OBJECT)
    {
      LOG_ERROR ("Expected %s OBJECT at %u of %u tokens, got %s", section,
                 tokens->index, tokens->num_tokens, t_typename[t->type]);
      json_advance_unknown (dat, tokens, 0);
      return DWG_ERR_INVALIDTYPE;
    }
  // ... dynapi
  return -1;
}

static int
json_ObjFreeSpace (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                  jsmntokens_t *restrict tokens)
{
  const char *section = "ObjFreeSpace";
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  (void)dat; (void)dwg;
  if (t->type != JSMN_OBJECT)
    {
      LOG_ERROR ("Expected %s OBJECT at %u of %u tokens, got %s", section,
                 tokens->index, tokens->num_tokens, t_typename[t->type]);
      json_advance_unknown (dat, tokens, 0);
      return DWG_ERR_INVALIDTYPE;
    }
  // ... dynapi
  return -1;
}

static int
json_Template (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
               jsmntokens_t *restrict tokens)
{
  const char *section = "Template";
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  (void)dat; (void)dwg;
  if (t->type != JSMN_OBJECT)
    {
      LOG_ERROR ("Expected %s OBJECT at %u of %u tokens, got %s", section,
                 tokens->index, tokens->num_tokens, t_typename[t->type]);
      json_advance_unknown (dat, tokens, 0);
      return DWG_ERR_INVALIDTYPE;
    }
  // ... dynapi
  return -1;
}

EXPORT int
dwg_read_json (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  struct Dwg_Header *obj = &dwg->header;
  jsmn_parser parser;
  //int num_tokens;
  //jsmntok_t *tokens;
  jsmntokens_t tokens;
  unsigned int i;
  int error = -1;

  loglevel = dwg->opts & 0xf;
  if (!dat->chain && dat->fh)
    {
      error = dat_read_stream (dat, dat->fh);
      if (error > DWG_ERR_CRITICAL)
        return error;
      LOG_TRACE ("  json file size: %lu\n", dat->size);
    }
  g_dat = dat;

  jsmn_init (&parser);
  // How big will it be? This is the max. memory variant.
  // we could also use less, see jsmn/examples/jsondump.c for small devices.
  tokens.num_tokens = jsmn_parse (&parser, (char *)dat->chain, dat->size, NULL, 0);
  if (tokens.num_tokens <= 0)
    {
      char err[21];
      memcpy (&err, &dat->chain[parser.pos - 10], 20);
      err[20] = 0;
      LOG_ERROR ("Invalid json. jsmn error at pos: %u (...%s...)", parser.pos,
                 err);
      return DWG_ERR_INVALIDDWG;
    }
  LOG_TRACE ("  num_tokens: %d\n", tokens.num_tokens);
  tokens.tokens = calloc (tokens.num_tokens + 1024, sizeof (jsmntok_t));
  if (!tokens.tokens)
    return DWG_ERR_OUTOFMEM;

  jsmn_init (&parser); // reset pos to 0
  error = jsmn_parse (&parser, (char *)dat->chain, dat->size, tokens.tokens,
                      tokens.num_tokens);
  if (error < 0)
    {
      char err[21];
      memcpy (&err, &dat->chain[parser.pos - 10], 20);
      err[20] = 0;
      LOG_ERROR (
          "Invalid json. jsmn error %d at the %u-th token, pos: %u (...%s...)",
          error, parser.toknext, parser.pos, err);
      return DWG_ERR_INVALIDDWG;
    }

  if (tokens.tokens[0].type != JSMN_OBJECT)
    {
      fprintf (stderr, "First JSON element is not an object/hash\n");
      exit (1);
    }

  // valid first level tokens:
  // created_by: string
  // section objects: FILEHEADER, HEADER, THUMBNAILIMAGE, R2004_Header, SummaryInfo, AppInfo,
  //                  AppInfoHistory, FileDepList, Security, RevHistory, ObjFreeSpace, Template
  // section arrays: CLASSES, OBJECTS, HANDLES
  for (tokens.index = 1; tokens.index < tokens.num_tokens; tokens.index++)
    {
      char key[80];
      const jsmntok_t *t = &tokens.tokens[tokens.index];
      if (t->type != JSMN_STRING)
        {
          LOG_ERROR ("Expected JSON key at %u of %u tokens, got %s",
                     tokens.index, tokens.num_tokens, t_typename[t->type]);
          return DWG_ERR_INVALIDDWG;
        }
        /*
        case JSMN_OBJECT:
          {
            //int j;
#ifdef JSMN_PARENT_LINKS
            printf ("parent: [%d]\n", t->parent); // always i-1 for arrays and objects
#endif
            // one of: FILEHEADER, HEADER, THUMBNAILIMAGE, R2004_Header, SummaryInfo, AppInfo, AppInfoHistory
            printf ("OBJECT[%d](%d) %.*s\n", i, t->size, t->end - t->start, &dat->chain[t->start]);
            // check keys
          }
          break;
        case JSMN_PRIMITIVE:
          printf ("[%d] %.*s\n", i, t->end - t->start, &dat->chain[t->start]);
          break;
        case JSMN_STRING:
          */
          {
            int len = t->end - t->start;
            if (len < 80)
              {
                memcpy (key, &dat->chain[t->start], len);
                key[len] = '\0';
                tokens.index++;
                if (tokens.index >= tokens.num_tokens)
                  {
                    LOG_ERROR ("Unexpected end of JSON at %u of %u tokens",
                               tokens.index, tokens.num_tokens);
                    return DWG_ERR_INVALIDDWG;
                  }
                if (strEQc (key, "created_by"))
                  error |= json_created_by (dat, dwg, &tokens);
                else if (strEQc (key, "FILEHEADER"))
                  error |= json_FILEHEADER (dat, dwg, &tokens);
                else if (strEQc (key, "HEADER"))
                  error |= json_HEADER (dat, dwg, &tokens);
                else if (strEQc (key, "CLASSES"))
                  error |= json_CLASSES (dat, dwg, &tokens);
                else if (strEQc (key, "OBJECTS"))
                  error |= json_OBJECTS (dat, dwg, &tokens);
                else if (strEQc (key, "THUMBNAILIMAGE"))
                  error |= json_THUMBNAILIMAGE (dat, dwg, &tokens);
                else if (strEQc (key, "R2004_Header"))
                  error |= json_R2004_Header (dat, dwg, &tokens);
                else if (strEQc (key, "SummaryInfo"))
                  error |= json_SummaryInfo (dat, dwg, &tokens);
                else if (strEQc (key, "AppInfo"))
                  error |= json_AppInfo (dat, dwg, &tokens);
                else if (strEQc (key, "AppInfoHistory"))
                  error |= json_AppInfoHistory (dat, dwg, &tokens);
                else if (strEQc (key, "FileDepList"))
                  error |= json_FileDepList (dat, dwg, &tokens);
                else if (strEQc (key, "Security"))
                  error |= json_Security (dat, dwg, &tokens);
                else if (strEQc (key, "RevHistory"))
                  error |= json_RevHistory (dat, dwg, &tokens);
                else if (strEQc (key, "ObjFreeSpace"))
                  error |= json_ObjFreeSpace (dat, dwg, &tokens);
                else if (strEQc (key, "Template"))
                  error |= json_Template (dat, dwg, &tokens);
                /* Only in json early versions <0.11 */
                else if (strEQc (key, "HANDLES"))
                  error |= json_HANDLES (dat, dwg, &tokens);
                else
                  {
                    LOG_ERROR ("Unexpected JSON key %s at %u of %u tokens. %s:%d", key, tokens.index,
                               tokens.num_tokens, __FUNCTION__, __LINE__);
                    return DWG_ERR_INVALIDDWG;
                  }
              }
            //printf ("[%d] \"%.*s\"\n", i, t->end - t->start, &dat->chain[t->start]);
          }
          /*
          break;
        case JSMN_ARRAY:
#ifdef JSMN_PARENT_LINKS
          printf ("parent: [%d]\n", t->parent);
#endif
          printf ("ARRAY[%d] %.*s\n", i, t->end - t->start, &dat->chain[t->start]);
          break;
        case JSMN_UNDEFINED:
        default:
          LOG_ERROR ("Invalid json token type %d", t->type);
          return DWG_ERR_INVALIDDWG;
        }

      // TODO walk the sections and keys
      // json_header_read   (tokens, num_tokens, dwg);
      // json_classes_read  (tokens, num_tokens, dwg);
      // json_tables_read   (tokens, num_tokens, dwg);
      // json_blocks_read   (tokens, num_tokens, dwg);
      // json_entities_read (tokens, num_tokens, dwg);
      // json_objects_read  (tokens, num_tokens, dwg);
      // json_preview_read  (tokens, num_tokens, dwg);
      */
    }

  return 0;
}

#undef IS_ENCODE
#undef IS_JSON
