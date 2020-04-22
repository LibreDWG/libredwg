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
 * in_json.c: parse JSON via jsmn/jsmn.h
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
#include "hash.h"
#include "decode.h"
#include "dynapi.h"
#include "in_json.h"

static unsigned int loglevel;
#define DWG_LOGLEVEL loglevel
#include "logging.h"

#include "in_dxf.h"

// not exported
#define JSMN_STATIC
// probably not needed in the final version (counting keys)
#undef JSMN_PARENT_LINKS
// our files are bigger than 8000
// In strict mode primitive must be followed by "," or "}" or "]";
// comma/object/array
// In strict mode an object or array can't become a key
// In strict mode primitives are: numbers and booleans
#undef JSMN_STRICT
#include "../jsmn/jsmn.h"

typedef struct jsmntokens
{
  unsigned int index;
  jsmntok_t *tokens;
  long num_tokens;
} jsmntokens_t;

// synced with enum jsmntype_t
static const char *const t_typename[]
    = { "UNDEFINED", "OBJECT", "ARRAY", "STRING", "PRIMITIVE" };

/* the current version per spec block */
static unsigned int cur_ver = 0;
static char *created_by;
static Bit_Chain *g_dat;

#define json_expect(tokens, typ)                                              \
  if (tokens->tokens[tokens->index].type != JSMN_##typ)                       \
  return DWG_ERR_INVALIDTYPE

/*--------------------------------------------------------------------------------
 * MACROS
 */

#define ACTION injson
#define IS_ENCODER
#define IS_JSON

/******************************************************************/
#define _FIELD_FLOAT(nam, type)                                               \
  else if (strEQc (key, #nam))                                                \
  {                                                                           \
    _obj->nam = (BITCODE_##type)json_float (dat, tokens);                     \
    LOG_TRACE (#nam ": " FORMAT_##type "\n", _obj->nam)                       \
  }
#define _FIELD_LONG(nam, type)                                                \
  else if (strEQc (key, #nam))                                                \
  {                                                                           \
    _obj->nam = (BITCODE_##type)json_long (dat, tokens);                      \
    LOG_TRACE (#nam ": " FORMAT_##type "\n", _obj->nam)                       \
  }
#define SUB_FIELD_LONG(o,nam, type)                                           \
  else if (strEQc (key, #nam))                                                \
  {                                                                           \
    _obj->o.nam = (BITCODE_##type)json_long (dat, tokens);                    \
    LOG_TRACE (#nam ": " FORMAT_##type "\n", _obj->o.nam)                     \
  }
#define _FIELD_LONGT(nam, type, fmt)                                          \
  else if (strEQc (key, #nam))                                                \
  {                                                                           \
    _obj->nam = (BITCODE_##type)json_long (dat, tokens);                      \
    LOG_TRACE (#nam ": " FORMAT_##fmt "\n", _obj->nam)                        \
  }
#define FIELD_TFF(nam, len, dxf)                                              \
  else if (strEQc (key, #nam))                                                \
  {                                                                           \
    char *s = json_string (dat, tokens);                                      \
    int slen = strlen (s);                                                    \
    memcpy (&_obj->nam, s, MIN (len, slen));                                  \
    LOG_TRACE (#nam ": \"%.*s\"\n", len, _obj->nam);                          \
    free (s);                                                                 \
  }
#define FIELD_TFFx(nam, len, dxf)                                             \
  else if (strEQc (key, #nam))                                                \
  {                                                                           \
    unsigned long slen;                                                       \
    char *s = json_binary (dat, tokens, #nam, &slen);                         \
    memcpy (&_obj->nam, s, slen);                                             \
    LOG_TRACE (#nam ": \"%.*s\"\n", (int)slen, _obj->nam);                    \
    free (s);                                                                 \
  }
#define FIELD_BINARY(nam, lenf, dxf)                                          \
  else if (strEQc (key, #nam))                                                \
  {                                                                           \
    unsigned long slen;                                                       \
    _obj->nam = (BITCODE_TF)json_binary (dat, tokens, #nam, &slen);           \
    _obj->lenf = slen;                                                        \
    JSON_TOKENS_CHECK_OVERFLOW_ERR                                            \
  }
#define FIELD_T(nam, dxf)                                                     \
  else if (strEQc (key, #nam))                                                \
  {                                                                           \
    LOG_TRACE (#nam ": \"%.*s\"\n", t->end - t->start,                        \
               &dat->chain[t->start]);                                        \
    if (t->type == JSMN_STRING)                                               \
      {                                                                       \
        if (dwg->header.version >= R_2007)                                    \
          _obj->nam = (BITCODE_T)json_wstring (dat, tokens);                  \
        else                                                                  \
          _obj->nam = json_string (dat, tokens);                              \
      }                                                                       \
    else                                                                      \
      json_advance_unknown (dat, tokens, t->type, 0);                         \
    JSON_TOKENS_CHECK_OVERFLOW_ERR                                            \
  }
#define FIELD_T32(nam, dxf)                                                   \
  else if (strEQc (key, #nam))                                                \
  {                                                                           \
    LOG_TRACE (#nam ": \"%.*s\"\n", t->end - t->start,                        \
               &dat->chain[t->start]);                                        \
    if (t->type == JSMN_STRING)                                               \
      {                                                                       \
        _obj->nam = (BITCODE_T32)json_wstring (dat, tokens);                  \
      }                                                                       \
    else                                                                      \
      json_advance_unknown (dat, tokens, t->type, 0);                         \
    JSON_TOKENS_CHECK_OVERFLOW_ERR                                            \
  }
#define FIELD_TU16(nam, dxf)                                                  \
  else if (strEQc (key, #nam))                                                \
  {                                                                           \
    LOG_TRACE (#nam ": \"%.*s\"\n", t->end - t->start,                        \
               &dat->chain[t->start]);                                        \
    if (t->type == JSMN_STRING)                                               \
      {                                                                       \
        _obj->nam = (BITCODE_TU)json_wstring (dat, tokens);                   \
      }                                                                       \
    else                                                                      \
      json_advance_unknown (dat, tokens, t->type, 0);                         \
    JSON_TOKENS_CHECK_OVERFLOW_ERR                                            \
  }
#define FIELD_TIMERLL(nam, dxf)                                               \
  else if (strEQc (key, #nam))                                                \
  {                                                                           \
    json_TIMERLL (dat, tokens, #nam, "TIMERLL", &_obj->nam);                  \
    JSON_TOKENS_CHECK_OVERFLOW_ERR                                            \
  }

#define FIELD_B(nam, dxf) _FIELD_LONG (nam, B)
#define FIELD_BB(nam, dxf) _FIELD_LONG (nam, BB)
#define FIELD_3B(nam, dxf) _FIELD_LONG (nam, 3B)
#define FIELD_BS(nam, dxf) _FIELD_LONG (nam, BS)
#define FIELD_BL(nam, dxf) _FIELD_LONG (nam, BL)
#define FIELD_BLL(nam, dxf) _FIELD_LONG (nam, BLL)
#define FIELD_RC(nam, dxf) _FIELD_LONG (nam, RC)
#define FIELD_RCx(nam, dxf) _FIELD_LONGT (nam, RC, RCx)
#define FIELD_RS(nam, dxf) _FIELD_LONG (nam, RS)
#define FIELD_RSx(nam, dxf) _FIELD_LONGT (nam, RS, RSx)
#define FIELD_RL(nam, dxf) _FIELD_LONG (nam, RL)
#define FIELD_RLx(nam, dxf) _FIELD_LONGT (nam, RL, RLx)
#define FIELD_RLd(nam, dxf) _FIELD_LONGT (nam, RL, RLd)
#define FIELD_RLL(nam, dxf) _FIELD_LONG (nam, RLL)
#define FIELD_MC(nam, dxf) _FIELD_LONG (nam, MC)
#define FIELD_MS(nam, dxf) _FIELD_LONG (nam, MS)

#define FIELD_BD(nam, dxf) _FIELD_FLOAT (nam, BD)
#define FIELD_RD(nam, dxf) _FIELD_FLOAT (nam, RD)

// NOTE: Only for int types for now, no float. There don't exist any inlined float vectors
#define FIELD_VECTOR_INL(nam, typ, _size, dxf)                                \
  else if (strEQc (key, #nam) && t->type == JSMN_ARRAY && t->size == _size)   \
  {                                                                           \
    tokens->index++;                                                          \
    JSON_TOKENS_CHECK_OVERFLOW_ERR                                            \
    for (int vcount = 0; vcount < _size; vcount++)                            \
      {                                                                       \
        _obj->nam[vcount] = (BITCODE_##typ)json_long (dat, tokens);           \
        JSON_TOKENS_CHECK_OVERFLOW_ERR                                        \
        LOG_TRACE (#nam "[%d]: " FORMAT_##typ " [" #typ " %d]\n", vcount,     \
                   _obj->nam[vcount], dxf);                                   \
      }                                                                       \
  }
// e.g. FIELD_VECTOR (histories, RL, num_histories, 0)
#define FIELD_VECTOR(nam, typ, _sizef, dxf)                                   \
  else if (strEQc (key, #nam) && t->type == JSMN_ARRAY)                       \
  {                                                                           \
    _obj->nam = (BITCODE_##typ *)calloc (t->size, sizeof (BITCODE_##typ));    \
    _obj->_sizef = t->size;                                                   \
    tokens->index++;                                                          \
    JSON_TOKENS_CHECK_OVERFLOW_ERR                                            \
    for (int vcount = 0; vcount < (int)t->size; vcount++)                     \
      {                                                                       \
        _obj->nam[vcount] = (BITCODE_##typ)json_long (dat, tokens);           \
        JSON_TOKENS_CHECK_OVERFLOW_ERR                                        \
        LOG_TRACE (#nam "[%d]: " FORMAT_##typ " [" #typ " %d]\n", vcount,     \
                   _obj->nam[vcount], dxf);                                   \
      }                                                                       \
  }

#define JSON_TOKENS_CHECK_OVERFLOW(retval)                                    \
  if (tokens->index >= (unsigned int)tokens->num_tokens)                      \
    {                                                                         \
      LOG_ERROR ("Unexpected end of JSON at %u of %ld tokens", tokens->index, \
                 tokens->num_tokens);                                         \
      return retval;                                                          \
    }
#define JSON_TOKENS_CHECK_OVERFLOW_VOID                                       \
  if (tokens->index >= (unsigned int)tokens->num_tokens)                      \
    {                                                                         \
      LOG_ERROR ("Unexpected end of JSON at %u of %ld tokens", tokens->index, \
                 tokens->num_tokens);                                         \
      return;                                                                 \
    }
#define JSON_TOKENS_CHECK_OVERFLOW_ERR  JSON_TOKENS_CHECK_OVERFLOW (DWG_ERR_INVALIDDWG)
#define JSON_TOKENS_CHECK_OVERFLOW_NULL JSON_TOKENS_CHECK_OVERFLOW (NULL)

// advance until next known first-level type
// on OBJECT to end of OBJECT
// on ARRAY to end of ARRAY
// on STRING (key) get next
static int
json_advance_unknown (Bit_Chain *restrict dat, jsmntokens_t *restrict tokens,
                      jsmntype_t type, int depth)
{
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  int error = 0;
  JSON_TOKENS_CHECK_OVERFLOW_ERR
  if (depth > 25)
    {
      LOG_ERROR ("JSON recursion limit");
      return DWG_ERR_INVALIDDWG;
    }
  if (!depth || ((dat->opts & DWG_OPTS_LOGLEVEL) >= 3))
    LOG_TRACE ("Skip JSON %s %.*s at %u of %ld tokens\n", t_typename[t->type],
               t->end - t->start, &dat->chain[t->start], tokens->index,
               tokens->num_tokens);
  switch (t->type)
    {
    case JSMN_OBJECT:
    case JSMN_ARRAY:
      tokens->index++;
      JSON_TOKENS_CHECK_OVERFLOW_ERR
      for (int i = 0; i < t->size; i++)
        {
          if (t->type == JSMN_OBJECT) tokens->index++; // skip the key also
          error |= json_advance_unknown (dat, tokens, t->type, depth + 1);
        }
      return error;
    case JSMN_STRING:
    case JSMN_PRIMITIVE:
      tokens->index++;
      JSON_TOKENS_CHECK_OVERFLOW_ERR
      return error;
    case JSMN_UNDEFINED:
    default:
      return error;
    }
  return error;
}

static void
json_fixed_key (char *key, Bit_Chain *restrict dat,
                jsmntokens_t *restrict tokens)
{
  const jsmntok_t *t;
  int len;
  JSON_TOKENS_CHECK_OVERFLOW_VOID
  t = &tokens->tokens[tokens->index];
  len = t->end - t->start;
  *key = 0;
  if (t->type != JSMN_STRING)
    {
      LOG_ERROR ("Expected JSON STRING");
      json_advance_unknown (dat, tokens, t->type, 0);
      JSON_TOKENS_CHECK_OVERFLOW_VOID
      return;
    }
  if (len >= 80)
    {
      LOG_ERROR ("Expected JSON STRING");
      tokens->index++;
      JSON_TOKENS_CHECK_OVERFLOW_VOID
      return;
    }
  memcpy (key, &dat->chain[t->start], len);
  key[len] = '\0';
  tokens->index++;
  JSON_TOKENS_CHECK_OVERFLOW_VOID
  return;
}

// which would require a different size, need to recalc.
static inline bool
does_cross_unicode_datversion (Bit_Chain *restrict dat)
{
  if ((dat->version < R_2007 && dat->from_version >= R_2007)
      || (dat->version >= R_2007 && dat->from_version < R_2007))
    return true;
  else
    return false;
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
      json_advance_unknown (dat, tokens, t->type, 0);
      JSON_TOKENS_CHECK_OVERFLOW_NULL
      return NULL;
    }
  // Unquote \", convert Unicode to \\U+xxxx as in bit_embed_TU
  if (memchr (&dat->chain[t->start], '\\', len))
    {
      len += 8;
      key = malloc (len);
      dat->chain[t->end] = '\0';
      while (!bit_utf8_to_TV (key, &dat->chain[t->start], len))
        {
          LOG_INSANE ("Not enough room in quoted string len=%d\n", len-8)
          len += 8;
          if (len > 6 * (t->end - t->start))
            {
              LOG_ERROR ("bit_utf8_to_TV loop len=%d vs %d \"%.*s\"", len,
                         t->end - t->start, t->end - t->start,
                         &dat->chain[t->start]);
              len = t->end - t->start;
              goto normal;
            }
          key = realloc (key, len);
        }
    }
  else
    {
    normal:
      key = malloc (len + 1);
      memcpy (key, &dat->chain[t->start], len);
      key[len] = '\0';
    }
  tokens->index++;
  JSON_TOKENS_CHECK_OVERFLOW(key)
  return key;
}

ATTRIBUTE_MALLOC
static BITCODE_TU
json_wstring (Bit_Chain *restrict dat, jsmntokens_t *restrict tokens)
{
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  if (t->type != JSMN_STRING)
    {
      LOG_ERROR ("Expected JSON STRING");
      json_advance_unknown (dat, tokens, t->type, 0);
      return NULL;
    }
  tokens->index++;
  JSON_TOKENS_CHECK_OVERFLOW_NULL
  dat->chain[t->end] = '\0';
  return bit_utf8_to_TU ((char *)&dat->chain[t->start]);
}

ATTRIBUTE_MALLOC
static char *
json_binary (Bit_Chain *restrict dat, jsmntokens_t *restrict tokens,
             const char *restrict key, unsigned long *lenp)
{
  // convert from hex
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  char *str = json_string (dat, tokens);
  size_t len = strlen (str);
  unsigned long blen = len / 2;
  char *buf = len ? malloc (blen + 1) : NULL;
  char *pos = str;
  char *old;

  *lenp = 0;
  if (t->type != JSMN_STRING)
    {
      LOG_ERROR ("Expected JSON STRING");
      json_advance_unknown (dat, tokens, t->type, 0);
      JSON_TOKENS_CHECK_OVERFLOW_NULL
      return NULL;
    }
  for (unsigned i = 0; i < blen; i++)
    {
      sscanf (pos, "%2hhX", &buf[i]);
      pos += 2;
    }
  if (buf)
    {
      buf[blen] = '\0';
      LOG_TRACE ("%s: '%.*s'... [BINARY]\n", key, (int)len / 10, str);
      *lenp = blen;
    }
  free (str);
  return buf;
}

static double
json_float (Bit_Chain *restrict dat, jsmntokens_t *restrict tokens)
{
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  // int len = t->end - t->start;
  if (t->type != JSMN_PRIMITIVE)
    {
      LOG_ERROR ("Expected JSON PRIMITIVE");
      json_advance_unknown (dat, tokens, t->type, 0);
      JSON_TOKENS_CHECK_OVERFLOW((double)NAN)
      return (double)NAN;
    }
  JSON_TOKENS_CHECK_OVERFLOW((double)NAN)
  tokens->index++;
  return strtod ((char *)&dat->chain[t->start], NULL);
}

static long
json_long (Bit_Chain *restrict dat, jsmntokens_t *restrict tokens)
{
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  // int len = t->end - t->start;
  if (t->type != JSMN_PRIMITIVE)
    {
      LOG_ERROR ("Expected JSON PRIMITIVE");
      json_advance_unknown (dat, tokens, t->type, 0);
      JSON_TOKENS_CHECK_OVERFLOW(0)
      return 0;
    }
  JSON_TOKENS_CHECK_OVERFLOW(0)
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
  JSON_TOKENS_CHECK_OVERFLOW_VOID
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
  JSON_TOKENS_CHECK_OVERFLOW_VOID
  tokens->index++;
  pt->x = json_float (dat, tokens);
  pt->y = json_float (dat, tokens);
  LOG_TRACE ("%s (%f, %f) [%s]\n", name, pt->x, pt->y, type);
}

static void
json_TIMERLL (Bit_Chain *restrict dat, jsmntokens_t *restrict tokens,
              const char *restrict name, const char *restrict type,
              BITCODE_TIMERLL *restrict tl)
{
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  if (t->type != JSMN_ARRAY || t->size != 2)
    {
      LOG_ERROR ("JSON TIMERLL must be ARRAY of size 2")
      return;
    }
  JSON_TOKENS_CHECK_OVERFLOW_VOID
  tokens->index++;
  tl->days = json_long (dat, tokens);
  tl->ms = json_long (dat, tokens);
  LOG_TRACE ("%s (%u, %u) [%s]\n", name, (unsigned)tl->days, (unsigned)tl->ms,
             type);
}

ATTRIBUTE_MALLOC
static BITCODE_H
json_HANDLE (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
             jsmntokens_t *restrict tokens, const char *name,
             const Dwg_Object *restrict obj, const int i)
{
  long code, size, value, absref;
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  BITCODE_H ref;
  if (t->type != JSMN_ARRAY || (t->size != 2 && t->size != 4))
    {
      LOG_ERROR ("JSON HANDLE must be ARRAY of [ code, value ] or [ code, size, value, absref ]")
      return NULL;
    }
  JSON_TOKENS_CHECK_OVERFLOW_NULL
  tokens->index++;
  code = json_long (dat, tokens);
  if (t->size == 4)
    {
      size = json_long (dat, tokens);
      value = json_long (dat, tokens);
      absref = json_long (dat, tokens);
      ref = dwg_add_handleref (dwg, code, absref, (!code || code >= 6) ? obj : NULL);
      if ((BITCODE_RC)size != ref->handleref.size || (unsigned long)value != ref->handleref.value)
        {
          // FIXME internal in_json problem only
          LOG_INFO ("dwg_add_handle(%.*s) inconsistency => " FORMAT_REF "\n",
                    t->end - t->start, &dat->chain[t->start], ARGS_REF (ref));
          ref->handleref.size = (BITCODE_RC)size;
          ref->handleref.value = (unsigned long)value;
          ref->absolute_ref = (unsigned long)absref;
        }
    }
  else
    {
      absref = json_long (dat, tokens);
      ref = dwg_add_handleref (dwg, code, absref, (!code || code >= 6) ? obj : NULL);
    }
  if (i < 0)
    LOG_TRACE ("%s: " FORMAT_REF " [H]\n", name, ARGS_REF (ref))
  else // H*
    LOG_TRACE ("%s[%d]: " FORMAT_REF " [H]\n", name, i, ARGS_REF (ref))
  return ref;
}

static void
json_CMC (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
          jsmntokens_t *restrict tokens, const char *name,
          Dwg_Color *restrict color)
{
  char key[80];
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  if (t->type == JSMN_OBJECT)
    {                  // 2004+
      tokens->index++; // hash of index, rgb...
      for (int j = 0; j < t->size; j++)
        {
          JSON_TOKENS_CHECK_OVERFLOW_VOID
          json_fixed_key (key, dat, tokens);
          if (strEQc (key, "index"))
            {
              long num = json_long (dat, tokens);
              JSON_TOKENS_CHECK_OVERFLOW_VOID
              LOG_TRACE ("%s.index %ld [CMC]\n", name, num);
              color->index = (BITCODE_BSd)num;
            }
          else if (strEQc (key, "rgb"))
            {
              char hex[80];
              json_fixed_key (hex, dat, tokens);
              JSON_TOKENS_CHECK_OVERFLOW_VOID
              LOG_TRACE ("%s.rgb %s [CMC]\n", name, hex);
              sscanf (hex, "%x", &color->rgb);
            }
          else if (strEQc (key, "flag"))
            {
              long num = json_long (dat, tokens);
              JSON_TOKENS_CHECK_OVERFLOW_VOID
              LOG_TRACE ("%s.flag %u [CMC]\n", name, (unsigned)num);
              color->flag = (BITCODE_BS)num;
            }
          else if (strEQc (key, "alpha"))
            {
              long num = json_long (dat, tokens);
              JSON_TOKENS_CHECK_OVERFLOW_VOID
              LOG_TRACE ("%s.alpha %u [CMC]\n", name, (unsigned)num);
              color->alpha = (BITCODE_RC)num;
              color->alpha_type = 3;
            }
          else if (strEQc (key, "handle")) // [4, value] ARRAY
            {
              color->handle = json_HANDLE (dat, dwg, tokens, name, NULL, -1);
              JSON_TOKENS_CHECK_OVERFLOW_VOID
            }
          else if (strEQc (key, "name"))
            {
              char *str = json_string (dat, tokens);
              JSON_TOKENS_CHECK_OVERFLOW_VOID
              LOG_TRACE ("%s.name \"%s\" [CMC]\n", name, str);
              color->name = str;
            }
          else if (strEQc (key, "book_name"))
            {
              char *str = json_string (dat, tokens);
              JSON_TOKENS_CHECK_OVERFLOW_VOID
              LOG_TRACE ("%s.book_name \"%s\" [CMC]\n", name, str);
              color->book_name = str;
            }
          else
            {
              LOG_WARN ("Unknown key color.%s", key);
              tokens->index++;
              JSON_TOKENS_CHECK_OVERFLOW_VOID
            }
        }
    }
  else if (t->type == JSMN_PRIMITIVE)
    { // pre 2004
      long num = json_long (dat, tokens);
      LOG_TRACE ("%s.index %ld [CMC]\n", name, num);
      color->index = (BITCODE_BSd)num;
      JSON_TOKENS_CHECK_OVERFLOW_VOID
    }
}

static void
json_TIMEBLL (Bit_Chain *restrict dat, jsmntokens_t *restrict tokens,
              const char *name, BITCODE_TIMEBLL *date)
{
  double ms;
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  // the double format looses the last two digits 6-8
  if (t->type == JSMN_ARRAY && t->size == 2)
    {
      tokens->index++;
      JSON_TOKENS_CHECK_OVERFLOW_VOID
      date->days = json_long (dat, tokens);
      JSON_TOKENS_CHECK_OVERFLOW_VOID
      date->ms = json_long (dat, tokens);
      JSON_TOKENS_CHECK_OVERFLOW_VOID
      date->value = date->days + (86400.0 * date->ms); // just for display, not calculations
    }
  else
    {
      double num;
      num = json_float (dat, tokens);
      JSON_TOKENS_CHECK_OVERFLOW_VOID
      date->value = num;
      date->days = (BITCODE_BL)trunc (num);
      date->ms = (BITCODE_BL) (86400.0 * (date->value - date->days));
    }
  LOG_TRACE ("%s: %.08f (%u, %u) [TIMEBLL]\n", name, date->value, date->days,
             date->ms);
}

// array of subclass objects
static void*
json_records (Bit_Chain *restrict dat, jsmntokens_t *restrict tokens,
              void *_obj, const char *subclass, BITCODE_BL *nump)
{
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  if (t->type != JSMN_ARRAY)
    {
      LOG_ERROR ("Expected %s ARRAY of OBJECTs", subclass);
      json_advance_unknown (dat, tokens, t->type, 0);
      JSON_TOKENS_CHECK_OVERFLOW_NULL
      *nump = 0;
      return NULL;
    }
  // see subclass arrays, _set_struct_field
  json_advance_unknown (dat, tokens, t->type, 0);
  JSON_TOKENS_CHECK_OVERFLOW_NULL
  return NULL;
}

// of primitives only
static void*
json_vector (Bit_Chain *restrict dat, jsmntokens_t *restrict tokens,
             const char *key, const char *type, BITCODE_BL *nump)
{
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  BITCODE_BL *a_bl;
  BITCODE_RLL *a_rll;
  BITCODE_BD *a_bd;
  BITCODE_TV *a_tv;
  void *arr;
  int len = t->size;
  int size, is_str = 0, is_float = 0;

  if (t->type != JSMN_ARRAY)
    {
      LOG_ERROR ("Expected %s ARRAY", "key");
      json_advance_unknown (dat, tokens, t->type, 0);
      JSON_TOKENS_CHECK_OVERFLOW_NULL
      *nump = 0;
      return NULL;
    }
  if (strEQc (type, "BL"))
    {
      size = sizeof (BITCODE_BL);
      a_bl = arr = len ? calloc (len, size) : NULL;
    }
  else if (strEQc (type, "RLL"))
    {
      size = sizeof (BITCODE_RLL);
      a_rll = arr = len ? calloc (len, size) : NULL;
    }
  else if (strEQc (type, "BD"))
    {
      is_float = 1;
      size = sizeof (BITCODE_BD);
      a_bd = arr = len ? calloc (len, size) : NULL;
    }
  else if (strEQc (type, "TV"))
    {
      is_str = 1;
      size = sizeof (BITCODE_TV);
      a_tv = arr = len ? calloc (len, size) : NULL;
    }
  else
    {
      LOG_ERROR ("Unhandled json_vector type %s", type);
      return NULL;
    }
  // json_set_numfield (_obj, fields, key, size1);
  *nump = len;
  if (!len)
    LOG_TRACE ("%s: [%s] empty\n", key, type);
  tokens->index++;
  for (int k = 0; k < len; k++)
    {
      JSON_TOKENS_CHECK_OVERFLOW_NULL;
      t = &tokens->tokens[tokens->index];
      if (t->type != JSMN_PRIMITIVE)
        {
          LOG_ERROR ("Expected %s PRIMITIVE", "key");
          json_advance_unknown (dat, tokens, t->type, 0);
          return arr;
        }
      if (is_str)
        {
          a_tv[k] = json_string (dat, tokens);
          JSON_TOKENS_CHECK_OVERFLOW_NULL;
          LOG_TRACE ("%s[%d]: %s [%s]\n", key, k, a_tv[k], type);
        }
      else if (is_float)
        {
          a_bd[k] = json_float (dat, tokens);
          LOG_TRACE ("%s[%d]: %f [%s]\n", key, k, a_bd[k], type);
        }
      else if (strEQc (type, "RLL"))
        {
          a_rll[k] = json_long (dat, tokens);
          LOG_TRACE ("%s[%d]: " FORMAT_RLL " [%s]\n", key, k, a_rll[k], type);
        }
      else if (strEQc (type, "BL"))
        {
          a_bl[k] = json_long (dat, tokens);
          LOG_TRACE ("%s[%d]: " FORMAT_BL " [%s]\n", key, k, a_bl[k], type);
        }
    }
  //dwg_dynapi_field_set_value (dwg, _obj, f, &nums, 1);
  return (void*)arr;
}

static int
json_created_by (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                 jsmntokens_t *restrict tokens)
{
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  (void)dwg;
  if (t->type != JSMN_STRING)
    {
      LOG_ERROR ("Expected %s STRING", "created_by");
      json_advance_unknown (dat, tokens, t->type, 0);
      return DWG_ERR_INVALIDTYPE;
    }
  created_by = json_string (dat, tokens);
  JSON_TOKENS_CHECK_OVERFLOW_ERR;
  LOG_TRACE ("created_by %s\n", created_by);
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
      LOG_ERROR ("Unexpected %s at %u of %ld tokens, expected %s OBJECT",
                 t_typename[t->type], tokens->index, tokens->num_tokens,
                 section);
      json_advance_unknown (dat, tokens, t->type, 0);
      return DWG_ERR_INVALIDTYPE;
    }
  LOG_TRACE ("\n%s pos:%d [%d keys]\n--------------------\n", section,
             tokens->index, t->size);
  tokens->index++;
  JSON_TOKENS_CHECK_OVERFLOW_ERR
  // t = &tokens->tokens[tokens->index];
  // json_expect(tokens, STRING);
  // FIELD_TV (version, 0);
  for (int i = 0; i < size; i++)
    {
      char key[80];
      json_fixed_key (key, dat, tokens);
      JSON_TOKENS_CHECK_OVERFLOW_ERR
      t = &tokens->tokens[tokens->index];
      if (strEQc (key, "version"))
        {
          Dwg_Version_Type v;
          version[0] = '\0';
          json_fixed_key (version, dat, tokens);
          // set version's
          for (v = 0; v <= R_AFTER; v++)
            {
              if (strEQ (version, version_codes[v]))
                {
                  dat->from_version = dwg->header.from_version = v;
                  // is_tu = dat->version >= R_2007;
                  LOG_TRACE ("FILEHEADER.from_version = %s (%s)\n",
                             version, dwg_version_type (v));
                  if (!dwg->header.version) // not already set
                    dwg->header.version = dat->version = dat->from_version;
                  else
                    {
                      dat->version = dwg->header.version;
                      LOG_TRACE ("FILEHEADER.version = %s (%s)\n", version_codes[dat->version],
                                 dwg_version_type (dat->version));
                    }
                  break;
                }
            }
          if (v == R_AFTER)
            {
              LOG_ERROR ("Invalid FILEHEADER.version %s", version);
              exit (1);
            }
        }
      // FIELD_VECTOR_INL (zero_5, RL, 5, 0)
      else if (strEQc (key, "zero_5") && t->type == JSMN_ARRAY)
        {
          tokens->index++;
          for (int j = 0; j < MIN (5, t->size); j++)
            {
              _obj->zero_5[j] = json_long (dat, tokens);
            }
        }
      // clang-format off
      FIELD_RC (is_maint, 0)
      FIELD_RC (zero_one_or_three, 0)
      FIELD_RL (thumbnail_address, 0) //@0x0d
      FIELD_RC (dwg_version, 0)
      FIELD_RC (maint_version, 0)
      FIELD_RS (codepage, 0) //@0x13: 29/30 for ANSI_1252, since r2007 UTF-16
      // SINCE (R_2004)
      FIELD_RC (unknown_0, 0)
      FIELD_RC (app_dwg_version, 0)
      FIELD_RC (app_maint_version, 0)
      FIELD_RL (security_type, 0)
      FIELD_RL (rl_1c_address, 0) /* mostly 0 */
      FIELD_RL (summaryinfo_address, 0)
      FIELD_RL (vbaproj_address, 0)
      FIELD_RL (rl_28_80, 0) /* mostly 128/0x80 */
      // clang-format on

      else if (strEQc (key, "HEADER"))
      {
        LOG_WARN ("Unexpected next section %s", key)
        tokens->index--;
        tokens->index--;
        return 0;
      }
      else
      {
        LOG_ERROR ("Unknown %s.%s ignored", section, key);
        tokens->index++;
      }
    }
  LOG_TRACE ("End of %s\n", section)
  tokens->index--;
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
      LOG_ERROR ("Unexpected %s at %u of %ld tokens, expected %s OBJECT",
                 t_typename[t->type], tokens->index, tokens->num_tokens,
                 section);
      json_advance_unknown (dat, tokens, t->type, 0);
      return DWG_ERR_INVALIDTYPE;
    }
  LOG_TRACE ("\n%s pos:%d [%d keys]\n--------------------\n", section,
             tokens->index, t->size);
  tokens->index++;
  for (int i = 0; i < size; i++)
    {
      char key[80];
      Dwg_DYNAPI_field *f;

      json_fixed_key (key, dat, tokens);
      JSON_TOKENS_CHECK_OVERFLOW_ERR
      t = &tokens->tokens[tokens->index];
      f = (Dwg_DYNAPI_field *)dwg_dynapi_header_field (key);
      if (!f)
        {
          LOG_WARN ("Unknown key HEADER.%s", key)
          json_advance_unknown (dat, tokens, t->type, 0);
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
          free (str);
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
      else if (strEQc (f->type, "TIMEBLL") || strEQc (f->type, "TIMERLL"))
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
          BITCODE_H hdl = json_HANDLE (dat, dwg, tokens, key, NULL, -1);
          if (hdl)
            dwg_dynapi_header_set_value (dwg, key, &hdl, 0);
        }
      //...
      else if (t->type == JSMN_OBJECT && strEQc (key, "CLASSES"))
        {
          LOG_WARN ("Unexpected next section %s", key)
          tokens->index--;
          tokens->index--;
          return 0;
        }
      else
        {
          LOG_WARN ("Unhandled %s [%s] with %s", key, f->type,
                    t_typename[t->type])
          tokens->index++;
          continue;
        }
    }
  LOG_TRACE ("End of %s\n", section)
  // the key
  tokens->index--;
  return 0;
}

static int
json_CLASSES (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
              jsmntokens_t *restrict tokens)
{
  const char *section = "CLASSES";
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  int size;
  if (t->type != JSMN_ARRAY || dwg->num_classes)
    {
      LOG_ERROR ("Unexpected %s at %u of %ld tokens, expected %s ARRAY",
                 t_typename[t->type], tokens->index, tokens->num_tokens,
                 section);
      json_advance_unknown (dat, tokens, t->type, 0);
      return DWG_ERR_INVALIDTYPE;
    }
  size = t->size;
  LOG_TRACE ("\n%s pos:%d [%d members]\n--------------------\n", section,
             tokens->index, size);
  tokens->index++;
  if (dwg->num_classes == 0)
    dwg->dwg_class = calloc (size, sizeof (Dwg_Class));
  else
    dwg->dwg_class = realloc (dwg->dwg_class,
                              (dwg->num_classes + size) * sizeof (Dwg_Class));
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
      if (i > 0)
        {
          Dwg_Class *oldkl = &dwg->dwg_class[i - 1];
          if (!oldkl->number || !oldkl->dxfname || !oldkl->appname ||
              !oldkl->cppname || !oldkl->item_class_id)
            {
              klass = oldkl; i--; size--; dwg->num_classes--;
              LOG_ERROR ("Illegal CLASS [%d]. Mandatory field missing, skipped", i)
            }
        }

      JSON_TOKENS_CHECK_OVERFLOW_ERR
      t = &tokens->tokens[tokens->index];
      if (t->type != JSMN_OBJECT)
        {
          LOG_ERROR (
              "Unexpected %s at %u of %ld tokens, expected %s OBJECT. %s:%d",
              t_typename[t->type], tokens->index, tokens->num_tokens, section,
              __FUNCTION__, __LINE__);
          json_advance_unknown (dat, tokens, t->type, 0);
          return DWG_ERR_INVALIDTYPE;
        }
      keys = t->size;
      tokens->index++;
      LOG_HANDLE ("\n-keys: %d\n", keys);
      for (int j = 0; j < keys; j++)
        {
          char key[80];
          JSON_TOKENS_CHECK_OVERFLOW_ERR
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
              if (dwg->header.version >= R_2007)
                klass->dxfname_u = bit_utf8_to_TU (klass->dxfname);
              LOG_TRACE ("dxfname: \"%s\"\n", klass->dxfname);
            }
          else if (strEQc (key, "cppname"))
            {
              LOG_TRACE ("cppname: \"%.*s\"\n", t->end - t->start,
                         &dat->chain[t->start])
              if (dwg->header.version >= R_2007)
                klass->cppname = (char*)json_wstring (dat, tokens);
              else
                klass->cppname = json_string (dat, tokens);
            }
          else if (strEQc (key, "appname"))
            {
              LOG_TRACE ("appname \"%.*s\"\n",  t->end - t->start,
                         &dat->chain[t->start])
              if (dwg->header.version >= R_2007)
                klass->appname = (char*)json_wstring (dat, tokens);
              else
                klass->appname = json_string (dat, tokens);
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
              LOG_WARN ("Unknown CLASS key %s %.*s", key, t->end - t->start,
                         &dat->chain[t->start])
              json_advance_unknown (dat, tokens, t->type, 0);
            }
        }
    }
  // also check the last one
  if (dwg->num_classes > 0)
    {
      Dwg_Class *oldkl = &dwg->dwg_class[dwg->num_classes - 1];
      if (!oldkl->number || !oldkl->dxfname || !oldkl->appname
          || !oldkl->cppname || !oldkl->item_class_id)
        {
          dwg->num_classes--;
          LOG_ERROR ("Illegal CLASS [%d]. Mandatory field missing, skipped",
                     dwg->num_classes)
        }
    }
  LOG_TRACE ("End of %s\n", section)
  tokens->index--;
  return 0;
}

static inline bool
eed_need_size (Bit_Chain *restrict dat, Dwg_Eed *restrict eed, const unsigned int i,
               const int need, int *havep)
{
  if (need > *havep)
    {
      int size;
      unsigned int isize;
      int diff = need - *havep;
      // find isize
      for (isize = i; !eed[isize].size && isize > 0; isize--)
        ;
      size = eed[isize].size;
      LOG_TRACE (" extend eed[%u].size to %d (+%d)\n", isize, size, diff)
      eed[i].data = realloc (eed[i].data, size + diff);
      eed[isize].size += diff;
      *havep = size + diff - need;
      return true;
    }
  *havep -= need;
  return false;
}

static void
json_eed (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
          jsmntokens_t *restrict tokens,
          Dwg_Object_Object *restrict obj)
{
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  int isize = -1;
  long size = 0;
  obj->eed = calloc (t->size, sizeof (Dwg_Eed));
  obj->num_eed = t->size;
  LOG_TRACE ("num_eed: " FORMAT_BL" [BL]\n", obj->num_eed);
  tokens->index++; // array of objects
  JSON_TOKENS_CHECK_OVERFLOW_VOID
  for (unsigned i = 0; i < obj->num_eed; i++)
    {
      char key[80];
      int have = 0;
      t = &tokens->tokens[tokens->index];
      if (t->type == JSMN_OBJECT)
        {
          const int osize = t->size;
          tokens->index++; // hash of size, handle, code, value
          for (int j = 0; j < osize; j++)
            {
              JSON_TOKENS_CHECK_OVERFLOW_VOID
              json_fixed_key (key, dat, tokens);
              t = &tokens->tokens[tokens->index];
              if (strEQc (key, "size"))
                {
                  size = json_long (dat, tokens);
                  isize = (int)i;
                  LOG_TRACE ("eed[%u].size %ld\n", i, size);
                  // see below: if does_cross_unicode_datversion (dat) we need to recalc
                  obj->eed[i].size = (BITCODE_BS)size;
                  have = size + 1; // we overallocate by 1 for the ending NUL
                  obj->eed[i].data = calloc (have, 1);
                  LOG_INSANE (" alloc eed[%u].data: %d\n", i, have)
               }
              else if (strEQc (key, "handle"))
                {
                  BITCODE_H hdl = json_HANDLE (dat, dwg, tokens, "eed[i].handle", NULL, -1);
                  memcpy (&obj->eed[i].handle, &hdl->handleref, sizeof (Dwg_Handle));
                  if (isize != (int)i || isize > (int)obj->num_eed)
                    {
                      LOG_ERROR ("Missing eed[%u].size field %d or overflow at %s", i, isize, key)
                      break;
                    }
                }
              else if (strEQc (key, "code"))
                {
                  long code = json_long (dat, tokens);
                  if (isize != (int)i)
                    {
                      if (have > 0)
                        {
                          obj->eed[i - 1].data = realloc (obj->eed[i - 1].data, size - have);
                          LOG_INSANE (" realloc eed[%u].data: %d\n", i-1, (int)(size - have))
                        }
                      have = size - have - 1;
                      obj->eed[i].data = calloc (have, 1);
                      LOG_INSANE (" alloc eed[%u].data: %d\n", i, have)
                    }
                  have--;
                  obj->eed[i].data->code = (BITCODE_RC)code;
                  LOG_TRACE ("eed[%u].data.code %ld\n", i, code);
                }
              else if (strEQc (key, "value"))
                {
                  Dwg_Eed_Data *data = obj->eed[i].data;
                  if (!data)
                    {
                      LOG_ERROR ("Missing eed[%u].code field", i)
                      break;
                    }
                  switch (data->code)
                    {
                    case 0:
                      {
                        // in work: wstring case, needed for dxfwrite
                        SINCE (R_2007)
                          {
                            BITCODE_TU s = json_wstring (dat, tokens);
                            int len = bit_wcs2len (s);
                            if (eed_need_size (dat, obj->eed, i, (len * 2) + 1 + 2, &have))
                              data = obj->eed[i].data;
                            data->u.eed_0_r2007.length = len;
                            memcpy (&data->u.eed_0_r2007.string, s, (len + 1) * 2);
                            have += 2; // ignore the ending NUL
                            LOG_TRACE ("eed[%u].data.value \"%.*s\"\n", i,
                                       t->end - t->start,
                                       &dat->chain[t->start]);
                            free (s);
                          }
                        else
                          {
                            char *s = json_string (dat, tokens);
                            int len = strlen (s);
                            if (eed_need_size (dat, obj->eed, i, len + 1 + 1 + 2, &have))
                              data = obj->eed[i].data;
                            data->u.eed_0.length = len;
                            data->u.eed_0.codepage = dwg->header.codepage;
                            memcpy (&data->u.eed_0.string, s, len + 1);
                            LOG_TRACE ("eed[%u].data.value \"%s\"\n", i, s);
                            have++; // ignore the ending NUL
                            free (s);
                            // with PRE (R_2007), the size gets smaller
                            if (does_cross_unicode_datversion (dat))
                              {
                                int oldsize = (len * 2) + 2;
                                size = len + 3;
                                obj->eed[isize].size -= (oldsize - size);
                              }
                          }
                      }
                      break;
                    case 2:
                      if (eed_need_size (dat, obj->eed, i, 1, &have))
                        data = obj->eed[i].data;
                      data->u.eed_2.byte = (BITCODE_RC)json_long (dat, tokens);
                      LOG_TRACE ("eed[%u].data.value %d\n", i, data->u.eed_2.byte);
                      break;
                    case 3:
                      eed_need_size (dat, obj->eed, i, 4, &have);
                      data->u.eed_3.layer = json_long (dat, tokens);
                      LOG_TRACE ("eed[%u].data.value %d\n", i, data->u.eed_3.layer);
                      break;
                    case 4:
                      {
                        unsigned long len;
                        char *s = json_binary (dat, tokens, "eed", &len);
                        if (eed_need_size (dat, obj->eed, i, len + 2, &have))
                          data = obj->eed[i].data;
                        memcpy (&data->u.eed_4.data, s, len);
                        data->u.eed_4.length = len;
                        //LOG_TRACE ("eed[%u].data.value \"%s\"\n", i, s);
                        free (s);
                        break;
                      }
                    case 5:
                      if (eed_need_size (dat, obj->eed, i, 8, &have))
                        data = obj->eed[i].data;
                      data->u.eed_5.entity = (BITCODE_RLL)json_long (dat, tokens);
                      LOG_TRACE ("eed[%u].data.value " FORMAT_RLL "\n", i,
                                 (BITCODE_RLL)data->u.eed_5.entity);
                      break;
                    case 10:
                    case 11:
                    case 12:
                    case 13:
                    case 14:
                    case 15:
                      {
                        BITCODE_3BD pt;
                        json_3DPOINT (dat, tokens, "eed", "3RD", &pt);
                        if (eed_need_size (dat, obj->eed, i, 24, &have))
                          data = obj->eed[i].data;
                        memcpy (&data->u.eed_10.point, &pt, 24);
                      }
                      break;
                    case 40:
                    case 41:
                    case 42:
                      if (eed_need_size (dat, obj->eed, i, 8, &have))
                        data = obj->eed[i].data;
                      data->u.eed_40.real = json_float (dat, tokens);
                      LOG_TRACE ("eed[%u].data.value %f\n", i, data->u.eed_40.real);
                      break;
                    case 70:
                      if (eed_need_size (dat, obj->eed, i, 2, &have))
                        data = obj->eed[i].data;
                      data->u.eed_70.rs = (BITCODE_RS)json_long (dat, tokens);
                      LOG_TRACE ("eed[%u].data.value %d\n", i, (int)data->u.eed_70.rs);
                      break;
                    case 71:
                      if (eed_need_size (dat, obj->eed, i, 4, &have))
                        data = obj->eed[i].data;
                      data->u.eed_71.rl = (BITCODE_RL)json_long (dat, tokens);
                      LOG_TRACE ("eed[%u].data.value %d\n", i, (int)data->u.eed_71.rl);
                      break;
                    default:
                      break;
                    }
                }
            }
        }
    }
  return;
}

static int
json_xdata (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
            jsmntokens_t *restrict tokens,
            Dwg_Object_XRECORD *restrict obj)
{
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  Dwg_Resbuf *rbuf;
  BITCODE_BL size = 0;
  obj->xdata = (Dwg_Resbuf *)calloc (1, sizeof (Dwg_Resbuf));
  rbuf = obj->xdata;
  obj->num_xdata = t->size;
  LOG_INSANE ("num_xdata: " FORMAT_BL"\n", obj->num_xdata);
  tokens->index++; // array of objects
  for (unsigned i = 0; i < obj->num_xdata; i++)
    {
      char key[80];
      JSON_TOKENS_CHECK_OVERFLOW_ERR
      t = &tokens->tokens[tokens->index];
      if (t->type == JSMN_ARRAY && t->size == 2) // of [ type, value ]
        {
          Dwg_Resbuf *old = rbuf;
          enum RES_BUF_VALUE_TYPE vtype;

          tokens->index++;
          JSON_TOKENS_CHECK_OVERFLOW_ERR
          rbuf->type = (BITCODE_RS)json_long (dat, tokens);
          JSON_TOKENS_CHECK_OVERFLOW_ERR
          LOG_INSANE ("xdata[%u]: type %d\n", i, rbuf->type);
          size += 2;
          vtype = get_base_value_type (rbuf->type);
          switch (vtype)
            {
            case VT_STRING:
              {
                char *s = json_string (dat, tokens);
                int len = strlen (s);
                JSON_TOKENS_CHECK_OVERFLOW_ERR
                rbuf->value.str.size = len;
                // here the xdata_size gets re-calculated from size
                PRE (R_2007) // target version
                {
                  rbuf->value.str.u.data = s;
                  rbuf->value.str.codepage = dwg->header.codepage;
                  LOG_TRACE ("xdata[%u]: \"%s\" [TV %d]\n", i, s,
                             (int)rbuf->type);
                  size += len + 3;
                }
                LATER_VERSIONS
                {
                  rbuf->value.str.u.wdata = bit_utf8_to_TU (s);
                  free (s);
                  LOG_TRACE_TU ("xdata", rbuf->value.str.u.wdata, rbuf->type);
                  size += (len * 2) + 2;
                }
              }
              break;
            case VT_BOOL:
            case VT_INT8:
              {
                long l = json_long (dat, tokens);
                JSON_TOKENS_CHECK_OVERFLOW_ERR
                rbuf->value.i8 = (BITCODE_RC)l;
                LOG_TRACE ("xdata[%u]: %ld [RC %d]\n", i, l, (int)rbuf->type);
                size += 1;
              }
              break;
            case VT_INT16:
              {
                long l = json_long (dat, tokens);
                JSON_TOKENS_CHECK_OVERFLOW_ERR
                rbuf->value.i16 = (BITCODE_RS)l;
                LOG_TRACE ("xdata[%u]: %ld [RS %d]\n", i, l, (int)rbuf->type);
                size += 2;
              }
              break;
            case VT_INT32:
              {
                long l = json_long (dat, tokens);
                JSON_TOKENS_CHECK_OVERFLOW_ERR
                rbuf->value.i32 = (BITCODE_RL)l;
                LOG_TRACE ("xdata[%u]: %ld [RL %d]\n", i, l, (int)rbuf->type);
                size += 4;
              }
              break;
            case VT_INT64:
              {
                long l = json_long (dat, tokens);
                JSON_TOKENS_CHECK_OVERFLOW_ERR
                rbuf->value.i64 = (BITCODE_BLL)l;
                LOG_TRACE ("xdata[%u]: %ld [BLL %d]\n", i, l, (int)rbuf->type);
                size += 8;
              }
              break;
            case VT_REAL:
              rbuf->value.dbl = json_float (dat, tokens);
              JSON_TOKENS_CHECK_OVERFLOW_ERR
              LOG_TRACE ("xdata[%u]: %f [RD %d]\n", i, rbuf->value.dbl,
                         (int)rbuf->type);
              size += 8;
              break;
            case VT_POINT3D:
              {
                BITCODE_3BD pt;
                json_3DPOINT (dat, tokens, "xdata", "3RD", &pt);
                JSON_TOKENS_CHECK_OVERFLOW_ERR
                memcpy (&rbuf->value.pt, &pt, 24);
                size += 24;
              }
              break;
            case VT_BINARY:
              {
                unsigned long len;
                char *s = json_binary (dat, tokens, "xdata", &len);
                JSON_TOKENS_CHECK_OVERFLOW_ERR
                rbuf->value.str.u.data = s;
                rbuf->value.str.size = len;
                size += len + 1;
                break;
              }
            case VT_HANDLE:
            case VT_OBJECTID:
              {
                BITCODE_H hdl;
                hdl = json_HANDLE (dat, dwg, tokens, key, NULL, -1);
                JSON_TOKENS_CHECK_OVERFLOW_ERR
                memcpy (&rbuf->value.h, &hdl->handleref,
                        sizeof (hdl->handleref));
                size += 8;
              }
              break;
            case VT_INVALID:
            default:
              break;
            }
          rbuf = (Dwg_Resbuf *)calloc (1, sizeof (Dwg_Resbuf));
          old->next = rbuf;
        }
      else
        {
          json_advance_unknown (dat, tokens, t->type, 0);
          JSON_TOKENS_CHECK_OVERFLOW_ERR
        }
    }
  if (obj->xdata_size != size)
    {
      if (!does_cross_unicode_datversion (dat))
        LOG_WARN ("Changed XRECORD.xdata_size from %u to %u", obj->xdata_size, size)
      else
        LOG_TRACE ("Changed XRECORD.xdata_size from %u to %u\n", obj->xdata_size, size)
    }
  obj->xdata_size = size;
  return 0;
}

static int
json_acis_data (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                jsmntokens_t *restrict tokens,
                const Dwg_Object *restrict obj)
{
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  int len = 0;
  int alloc = t->end - t->start;
  int size = t->size;
  char *s = calloc (alloc, 1);
  LOG_INSANE ("acis lines: %d\n", t->size);
  tokens->index++; // array of strings
  for (int i = 0; i < size; i++)
    {
      JSON_TOKENS_CHECK_OVERFLOW_ERR
      t = &tokens->tokens[tokens->index];
      if (t->type == JSMN_STRING)
        {
          int l = t->end - t->start;
          len += l + 1;
          if (len > alloc) // cannot happen, as we take the length of the json source array
            {
              LOG_WARN ("Internal surprise, acis_data overshoot %d > %d", len, alloc);
              alloc += 60;
              s = realloc (s, alloc);
            }
          // could be made faster, but those strings are short, and not that often.
          strncat (s, (char*)&dat->chain[t->start], l);
          strcat (s, "\n");
        }
      tokens->index++;
    }
  LOG_TRACE ("acis_data: %s\n", s);
  // just keep this s ptr, no utf8
  return dwg_dynapi_entity_set_value (obj->tio.entity->tio._3DSOLID, obj->name,
                                      "acis_data", &s, false)
             ? 0
             : DWG_ERR_INVALIDTYPE;
}

static const Dwg_DYNAPI_field *
find_numfield (const Dwg_DYNAPI_field *restrict fields,
               const char *restrict key)
{
  const Dwg_DYNAPI_field *f;
  char *s = malloc (strlen (key) + 12);
  strcpy (s, "num_");
  strcat (s, key);
  // see gen-dynapi.pl:1102
  if (strEQc (key, "attrib_handles"))
    strcpy (s, "num_owned");
  else if (strEQc (key, "attribs"))
    strcpy (s, "num_owned");
  else if (strEQc (key, "items"))
    strcpy (s, "numitems");
  else if (strEQc (key, "entities"))
    strcpy (s, "num_owned");
  else if (strEQc (key, "sort_ents"))
    strcpy (s, "num_ents");
  else if (strEQc (key, "attr_def_id"))
    strcpy (s, "num_attr_defs");
  else if (strEQc (key, "layer_entries"))
    strcpy (s, "num_entries");
  else if (strEQc (key, "readdeps"))
    strcpy (s, "num_deps");
  else if (strEQc (key, "writedeps"))
    strcpy (s, "num_deps");
  else if (strEQc (key, "encr_sat_data"))
    strcpy (s, "num_blocks");
  else if (strEQc (key, "styles")) // conflicts? only for LTYPE
    strcpy (s, "num_dashes");
  else if (strEQc (key, "segs") || strEQc (key, "polyline_paths"))
    {
      s = realloc (s, strlen ("num_segs_or_paths") + 1);
      strcpy (s, "num_segs_or_paths");
    }
  else if (strEQc (key, "txt.col_sizes"))
    {
      strcpy (s, "txt.num_col_sizes");
    }
search:
  for (f = &fields[0]; f->name; f++)
    {
      if (strEQ (s, f->name))
        {
          free (s);
          return f;
        }
    }
  // or num_owner
  if (strEQc (key, "vertex"))
    {
      strcpy (s, "num_owned");
      goto search;
    }
  // there are two of them
  if (strEQc (key, "paths") && strNE (s, "num_segs_or_paths"))
    {
      s = realloc (s, strlen ("num_segs_or_paths") + 1);
      strcpy (s, "num_segs_or_paths");
      goto search;
    }
  free (s);
  return NULL;
}

static const Dwg_DYNAPI_field *
find_sizefield (const Dwg_DYNAPI_field *restrict fields,
                const char *restrict key)
{
  const Dwg_DYNAPI_field *f;
  char *s = malloc (strlen (key) + 12);
  strcpy (s, key);
  strcat (s, "_size");
  for (f = &fields[0]; f->name; f++)
    {
      if (strEQ (s, f->name))
        {
          long size = 0;
          free (s);
          return f;
        }
    }
  free (s);
  return NULL;
}

#define FIXUP_NUMFIELD(type)                                                  \
  {                                                                           \
    BITCODE_##type num;                                                       \
    BITCODE_##type _size = (BITCODE_##type)size;                              \
    LOG_TRACE ("%s = " FORMAT_##type "\n", f->name, _size);                   \
    memcpy (&((char *)_obj)[f->offset], &_size, f->size);                     \
  }                                                                           \
  break;

static void
json_set_numfield (void *restrict _obj,
                   const Dwg_DYNAPI_field *restrict fields,
                   const char *restrict key, const long size)
{
  const Dwg_DYNAPI_field *f = find_numfield (fields, key);
  if (f)
    {
      switch (f->size)
        {
        case 1:
          FIXUP_NUMFIELD (B)
        case 2:
          FIXUP_NUMFIELD (BS)
        case 4:
          FIXUP_NUMFIELD (BL)
        case 8:
          FIXUP_NUMFIELD (BLL)
        default:
          LOG_ERROR ("Unknown %s dynapi size %d", key, f->size);
        }
    }
  else if (strEQc (key, "ref"))
    {
      if (size != 4) // fixed size
        LOG_WARN ("Need 4 ref array elements, have %ld", size)
      else
        LOG_TRACE ("Check ref[] 4 ok\n")
    }
  else
    LOG_ERROR ("Unknown num_%s field", key);
}
#undef FIXUP_NUMFIELD

#define FIXUP_SIZEFIELD(type)                                                 \
  {                                                                           \
    const BITCODE_##type _size = (BITCODE_##type)len;                         \
    LOG_TRACE ("%s = " FORMAT_##type "\n", f->name, _size);                   \
    memcpy (&((char *)_obj)[f->offset], &_size, f->size);                     \
  }                                                                           \
  break;

// e.g. for TF strings: preview + preview_size.
static void
json_set_sizefield (void *restrict _obj,
                    const Dwg_DYNAPI_field *restrict fields,
                    const char *restrict key, const size_t len)
{
  const Dwg_DYNAPI_field *f = find_sizefield (fields, key);
  if (f)
    {
      switch (f->size)
        {
        case 1:
          FIXUP_SIZEFIELD (B)
        case 2:
          FIXUP_SIZEFIELD (BS)
        case 4:
          FIXUP_SIZEFIELD (BL)
        case 8:
          FIXUP_SIZEFIELD (BLL)
        default:
          LOG_ERROR ("Unknown %s dynapi size %d", key, f->size);
        }
    }
  else
    {
      LOG_ERROR ("Unknown %s size field", key);
    }
}
#undef FIXUP_SIZE

// needs to be recursive, for search in subclasses
static int
_set_struct_field (Bit_Chain *restrict dat, const Dwg_Object *restrict obj,
                   jsmntokens_t *restrict tokens, void *restrict _obj,
                   const char *restrict name, const char *restrict key,
                   const Dwg_DYNAPI_field *restrict fields)
{
  Dwg_Data *restrict dwg = obj->parent;
  const Dwg_DYNAPI_field *f;
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  int error = 0;
  LOG_INSANE ("-search %s.%s: %s %.*s\n", name, key, t_typename[t->type],
              t->end - t->start, &dat->chain[t->start]);
  for (f = &fields[0]; f->name; f++) // linear search, not binary for now
    {
      // LOG_INSANE ("-%s.%s [%s]\n", name, f->name, f->type);
      // common and entity dynapi, check types
      JSON_TOKENS_CHECK_OVERFLOW_ERR
      if (strEQ (f->name, key)) // found
        {
          LOG_INSANE ("-found %s [%s] %s\n", f->name, f->type,
                      t_typename[t->type]);
          if (t->type == JSMN_PRIMITIVE
              && (strEQc (f->type, "BD") || strEQc (f->type, "RD")
                  || strEQc (f->type, "BT")))
            {
              double num = json_float (dat, tokens);
              JSON_TOKENS_CHECK_OVERFLOW_ERR
              LOG_TRACE ("%s: " FORMAT_RD " [%s]\n", key, num, f->type);
              dwg_dynapi_field_set_value (dwg, _obj, f, &num, 0);
            }
          // all numfields are calculated from actual array sizes
          // for easier adding or deleting entries.
          else if (t->type == JSMN_PRIMITIVE && (memBEGINc (key, "num_")))
            {
              tokens->index++;
              JSON_TOKENS_CHECK_OVERFLOW_ERR;
              /*
              if (strEQc (key, "numitems"))
                {
                  int32_t num;
                  dwg_dynapi_field_get_value (_obj, f, &num);
                  if (num) // may not exist
                    {
                      LOG_ERROR ("%s.numitems %d may not exist", name, (int)num);
                      return 0;
                    }
                  else
                    num = -1;
                  dwg_dynapi_field_set_value (dwg, _obj, f, &num, 0);
                  LOG_TRACE ("%s: %.*s => -1 (init)\n", key, t->end - t->start, &dat->chain[t->start]);
                }
              else
              */
              LOG_TRACE ("%s: %.*s (ignored)\n", key, t->end - t->start, &dat->chain[t->start]);
            }
          else if (t->type == JSMN_PRIMITIVE
                   && (strEQc (f->type, "RC") || strEQc (f->type, "B")
                       || strEQc (f->type, "BB") || strEQc (f->type, "RS")
                       || strEQc (f->type, "BS") || strEQc (f->type, "RL")
                       || strEQc (f->type, "BL") || strEQc (f->type, "RLL")
                       || strEQc (f->type, "BLd") || strEQc (f->type, "BSd")
                       || strEQc (f->type, "BLL")
                       || strEQc (f->type, "4BITS")))
            {
              long num = json_long (dat, tokens);
              JSON_TOKENS_CHECK_OVERFLOW_ERR
              LOG_TRACE ("%s: %ld [%s]\n", key, num, f->type);
              dwg_dynapi_field_set_value (dwg, _obj, f, &num, 0);
            }
          // TFF not yet in dynapi.c
          else if (t->type == JSMN_STRING
                   && (strEQc (f->type, "TV") || strEQc (f->type, "T")
                       || strEQc (f->type, "TF") || strEQc (f->type, "TU")))
            {
              char *str = json_string (dat, tokens);
              size_t len = strlen (str);
              JSON_TOKENS_CHECK_OVERFLOW_ERR
              if (f->dxf == 310) // is BINARY. TODO: TABLE/FIELD *.data_date
                {
                  // convert from hex
                  unsigned blen = len / 2;
                  char *buf = len ? malloc (blen + 1) : NULL;
                  char *pos = str;
                  char *old;
                  for (unsigned i = 0; i < blen; i++)
                    {
                      sscanf (pos, "%2hhX", &buf[i]);
                      pos += 2;
                    }
                  if (buf)
                    {
                      buf[blen] = '\0';
                      LOG_TRACE ("%s: '%.*s' [%s] (binary)\n", key, blen, buf,
                                 f->type);
                    }
                  free (str);
                  json_set_sizefield (_obj, fields, key, blen);
                  // set the ptr directly, no alloc, no conversion.
                  old = &((char *)_obj)[f->offset];
                  memcpy (old, &buf, f->size);
                }
              else if (strEQc (f->type, "TF")) // oleclient, strings_area, ...
                                               // fixup size field
                {
                  char *old;
                  if (strEQc (key, "strings_area"))
                    {
                      const int k = dwg->header.version > R_2004 ? 512 : 256;
                      if (len > (size_t)k)
                        {
                          LOG_ERROR ("Illegal %s.%s length %lu > %d, stripped", name,
                                     key, (unsigned long)len, k);
                          len = (size_t)k;
                        }
                      else if (len != (size_t)k)
                        {
                          str = realloc (str, k);
                          memset (&str[len + 1], 0, k - len - 1);
                        }
                    }
                  else if (f->size > sizeof (char *))
                    {
                      str = realloc (str, f->size);
                      memset (&str[len + 1], 0, f->size - len - 1);
                    }
                  LOG_TRACE ("%s: \"%s\" [%s %d]\n", key, str, f->type,
                             f->size);
                  if (strNE (key, "strings_area"))
                    json_set_sizefield (_obj, fields, key, len);
                  old = &((char *)_obj)[f->offset];
                  memcpy (old, &str, sizeof (char *));
                  // dwg_dynapi_field_set_value (dwg, _obj, f, &str, 1);
                }
              else
                {
                  LOG_TRACE ("%s: \"%s\" [%s] len=%d\n", key, str, f->type, (int)len);
                  dwg_dynapi_field_set_value (dwg, _obj, f, &str, 1);
                  free (str);
                }
            }
          else if (t->type == JSMN_ARRAY
                   && (strEQc (f->type, "3BD") || strEQc (f->type, "3RD")
                       || strEQc (f->type, "3DPOINT") || strEQc (f->type, "BE")
                       || strEQc (f->type, "3BD_1")))
            {
              BITCODE_3DPOINT pt;
              json_3DPOINT (dat, tokens, key, f->type, &pt);
              JSON_TOKENS_CHECK_OVERFLOW_ERR
              dwg_dynapi_field_set_value (dwg, _obj, f, &pt, 1);
            }
          else if (t->type == JSMN_ARRAY
                   && (strEQc (f->type, "2BD") || strEQc (f->type, "2RD")
                       || strEQc (f->type, "2DPOINT")
                       || strEQc (f->type, "2BD_1")))
            {
              BITCODE_2DPOINT pt;
              json_2DPOINT (dat, tokens, key, f->type, &pt);
              JSON_TOKENS_CHECK_OVERFLOW_ERR
              dwg_dynapi_field_set_value (dwg, _obj, f, &pt, 1);
            }
          else if (strEQc (f->type, "TIMEBLL") || strEQc (f->type, "TIMERLL"))
            {
              static BITCODE_TIMEBLL date = { 0, 0, 0 };
              json_TIMEBLL (dat, tokens, key, &date);
              JSON_TOKENS_CHECK_OVERFLOW_ERR
              dwg_dynapi_field_set_value (dwg, _obj, f, &date, 1);
            }
          else if (strEQc (f->type, "CMC"))
            {
              static BITCODE_CMC color = { 0, 0, 0 };
              json_CMC (dat, dwg, tokens, key, &color);
              JSON_TOKENS_CHECK_OVERFLOW_ERR
              dwg_dynapi_field_set_value (dwg, _obj, f, &color, 1);
            }
          else if (t->type == JSMN_ARRAY && strEQc (f->type, "H"))
            {
              BITCODE_H hdl;
              hdl = json_HANDLE (dat, dwg, tokens, key, obj, -1);
              JSON_TOKENS_CHECK_OVERFLOW_ERR
              if (hdl)
                dwg_dynapi_field_set_value (dwg, _obj, f, &hdl, 1);
            }
          else if (t->type == JSMN_ARRAY && strEQc (f->type, "H*"))
            {
              BITCODE_BL size1 = t->size;
              BITCODE_H *hdls;
              if (memBEGINc (name, "DICTIONARY") && strEQc (key, "itemhandles"))
                {
                  LOG_ERROR ("Illegal old json format");
                  return DWG_ERR_INVALIDDWG;
                  /*
                  BITCODE_BL numitems; // check against existing texts[] size
                  const Dwg_DYNAPI_field *numf = find_numfield (fields, key);
                  memcpy (&numitems, &((char *)_obj)[numf->offset], numf->size);
                  if (numitems != (BITCODE_BL)-1 && numitems != size1)
                    {
                      // already have texts. if less we leak them
                      LOG_WARN ("Skip some itemhandles, only accept %d of %d",
                                numitems, (int)size1)
                      size1 = numitems;
                    }
                  */
                }
              hdls = size1 ? calloc (size1, sizeof (BITCODE_H)) : NULL;
              json_set_numfield (_obj, fields, key, (long)size1);
              tokens->index++;
              for (int k = 0; k < t->size; k++)
                {
                  BITCODE_H hdl;
                  JSON_TOKENS_CHECK_OVERFLOW_ERR
                  hdl = json_HANDLE (dat, dwg, tokens, key, obj, k);
                  if (k < (int)size1)
                    {
                      if (hdl)
                        hdls[k] = hdl;
                      else
                        hdls[k] = dwg_add_handleref (dwg, 0, 0, NULL);
                    }
                  else
                    LOG_WARN ("ignored");
                }
              if (!size1)
                LOG_TRACE ("%s: [%s] empty\n", key, f->type);
              //memcpy (&((char *)_obj)[f->offset], &hdls, sizeof (hdls));
              dwg_dynapi_field_set_value (dwg, _obj, f, &hdls, 1);
            }
          else if (t->type == JSMN_ARRAY
                   && (strEQc (f->type, "T*") || strEQc (f->type, "TV*")))
            {
              int skip = 0;
              BITCODE_BL size1 = t->size;
              BITCODE_TV *elems;
              if (memBEGINc (name, "DICTIONARY") && strEQc (key, "texts"))
                {
                  LOG_ERROR ("Illegal old json format");
                  return DWG_ERR_INVALIDDWG;
                  /*
                  BITCODE_BL numitems;
                  const Dwg_DYNAPI_field *numf = find_numfield (fields, key);
                  memcpy (&numitems, &((char *)_obj)[numf->offset], numf->size);
                  if (numitems != (BITCODE_BL)-1)
                    {
                      // have less itemhandles. leak the texts
                      LOG_WARN ("%s.texts must be before itemhandles", name)
                      size1 = numitems;
                    }
                  else
                    size1 = t->size;
                  */
                }
              elems = size1 ? calloc (size1, sizeof (BITCODE_T)) : NULL;
              json_set_numfield (_obj, fields, key, (long)size1);
              tokens->index++;
              for (int k = 0; k < t->size; k++)
                {
                  JSON_TOKENS_CHECK_OVERFLOW_ERR
                  if (k < (int)size1)
                    {
                      elems[k] = json_string (dat, tokens);
                      LOG_TRACE ("%s[%d]: \"%s\" [%s]\n", key, k, elems[k],
                                 f->type);
                    }
                  else
                    {
                      tokens->index++;
                      t = &tokens->tokens[tokens->index];
                      LOG_WARN ("%s[%d]: \"%.*s\" [%s] ignored", key, k,
                                t->end - t->start, &dat->chain[t->start], f->type);
                    }
                }
              if (!t->size)
                LOG_TRACE ("%s: [%s] empty\n", key, f->type);
              dwg_dynapi_field_set_value (dwg, _obj, f, &elems, 1);
            }
          else if (t->type == JSMN_ARRAY && strEQc (f->type, "3DPOINT*"))
            {
              int size1 = t->size;
              BITCODE_3DPOINT *pts = size1 ? calloc (size1, sizeof (BITCODE_3BD)) : NULL;
              json_set_numfield (_obj, fields, key, size1);
              tokens->index++;
              for (int k = 0; k < size1; k++)
                {
                  JSON_TOKENS_CHECK_OVERFLOW_ERR
                  json_3DPOINT (dat, tokens, key, f->type, &pts[k]);
                }
              if (!size1)
                LOG_TRACE ("%s: [%s] empty\n", key, f->type);
              dwg_dynapi_field_set_value (dwg, _obj, f, &pts, 1);
            }
          else if (t->type == JSMN_ARRAY && strEQc (f->type, "2RD*"))
            {
              int size1 = t->size;
              BITCODE_2DPOINT *pts = size1 ? calloc (size1, sizeof (BITCODE_2RD)) : NULL;
              json_set_numfield (_obj, fields, key, size1);
              tokens->index++;
              for (int k = 0; k < size1; k++)
                {
                  JSON_TOKENS_CHECK_OVERFLOW_ERR
                  json_2DPOINT (dat, tokens, key, f->type, &pts[k]);
                }
              if (!size1)
                LOG_TRACE ("%s: [%s] empty\n", key, f->type);
              dwg_dynapi_field_set_value (dwg, _obj, f, &pts, 1);
            }
          else if (t->type == JSMN_ARRAY && strEQc (f->type, "BD*"))
            {
              int size1 = t->size;
              BITCODE_BD *nums = size1 ? calloc (size1, sizeof (BITCODE_BD)) : NULL;
              json_set_numfield (_obj, fields, key, size1);
              tokens->index++;
              for (int k = 0; k < size1; k++)
                {
                  JSON_TOKENS_CHECK_OVERFLOW_ERR
                  nums[k] = json_float (dat, tokens);
                  LOG_TRACE ("%s[%d]: %f [%s]\n", key, k, nums[k], f->type);
                }
              if (!size1)
                LOG_TRACE ("%s: [%s] empty\n", key, f->type);
              dwg_dynapi_field_set_value (dwg, _obj, f, &nums, 1);
            }
          else if (t->type == JSMN_ARRAY && strEQc (f->type, "BL*"))
            {
              int size1 = t->size;
              BITCODE_BL *nums = size1 ? calloc (size1, sizeof (BITCODE_BL)) : NULL;
              json_set_numfield (_obj, fields, key, size1);
              tokens->index++;
              for (int k = 0; k < size1; k++)
                {
                  JSON_TOKENS_CHECK_OVERFLOW_ERR
                  nums[k] = (BITCODE_BL)json_long (dat, tokens);
                  LOG_TRACE ("%s[%d]: " FORMAT_BL " [BL]\n", key, k, nums[k]);
                }
              if (!size1)
                LOG_TRACE ("%s: [%s] empty\n", key, f->type);
              dwg_dynapi_field_set_value (dwg, _obj, f, &nums, 1);
            }
          else if (t->type == JSMN_ARRAY && strEQc (key, "xdata") && strEQc (name, "XRECORD"))
            {
              error |= json_xdata (dat, dwg, tokens, _obj);
              JSON_TOKENS_CHECK_OVERFLOW_ERR
            }
          else if (t->type == JSMN_ARRAY && strEQc (key, "acis_data") && strEQc (f->type, "RC*"))
            {
              error |= json_acis_data (dat, dwg, tokens, obj);
              JSON_TOKENS_CHECK_OVERFLOW_ERR
            }
          else if (t->type == JSMN_ARRAY && strEQc (key, "encr_sat_data") && strEQc (f->type, "char **"))
            {
              BITCODE_BL num_blocks = t->size;
              BITCODE_BL *block_size = calloc (num_blocks + 1, sizeof (BITCODE_BL));
              char **data = calloc (num_blocks + 1, sizeof (char*));
              tokens->index++;
              LOG_TRACE ("num_blocks: " FORMAT_BL " [BL]\n", num_blocks);
              for (BITCODE_BL k = 0; k < num_blocks; k++)
                {
                  unsigned long len;
                  JSON_TOKENS_CHECK_OVERFLOW_ERR
                  data[k] = json_binary (dat, tokens, "encr_sat_data", &len);
                  block_size[k] = (BITCODE_BL)len;
                  LOG_TRACE ("block_size[%d]: %lu [BL]\n", k, len);
                }
              block_size[num_blocks] = 0;
              LOG_TRACE ("block_size[%d]: 0 [BL]\n", num_blocks);
              dwg_dynapi_entity_set_value (_obj, obj->name, "num_blocks", &num_blocks, true);
              dwg_dynapi_entity_set_value (_obj, obj->name, "block_size", &block_size, true);
              dwg_dynapi_field_set_value (dwg, _obj, f, &data, true);
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
              LOG_TRACE ("new subclass %s %s [%d elems with size %d]\n", name,
                         subclass, num_elems, size_elem);
              json_set_numfield (_obj, fields, key, num_elems);
              if (strEQc (subclass, "MLINE_line") && num_elems)
                {
                  Dwg_Entity_MLINE *o = obj->tio.entity->tio.MLINE;
                  o->num_lines = num_elems;
                  LOG_TRACE ("MLINE.num_lines = %d\n", num_elems);
                }
              if (strEQc (subclass, "DIMASSOC_Ref") && num_elems != 4)
                {
                  elems = calloc (MAX (4, num_elems), size_elem);
                  LOG_TRACE ("DIMASSOC num_refs = 4\n");
                }
              else
                elems = num_elems ? calloc (num_elems, size_elem) : NULL;
              tokens->index++;
              // array of structs
              if (!num_elems)
                LOG_TRACE ("%s: [%s] empty\n", key, f->type);
              for (int k = 0; k < num_elems; k++)
                {
                  int keys;
                  JSON_TOKENS_CHECK_OVERFLOW_ERR
                  t = &tokens->tokens[tokens->index];
                  if (t->type != JSMN_OBJECT)
                    {
                      LOG_ERROR ("Unexpected %s at %u of %ld tokens, expected "
                                 "%s OBJECT. %s:%d",
                                 t_typename[t->type], tokens->index,
                                 tokens->num_tokens, subclass, __FUNCTION__,
                                 __LINE__);
                      json_advance_unknown (dat, tokens, t->type, 0);
                      JSON_TOKENS_CHECK_OVERFLOW_ERR
                      return DWG_ERR_INVALIDTYPE;
                    }
                  LOG_TRACE ("%s.%s[%d]:\n", name, key, k);
                  keys = t->size;
                  tokens->index++;
                  for (int ki = 0; ki < keys; ki++)
                    {
                      // seperate subclass type loop
                      const Dwg_DYNAPI_field *f1;
                      char key1[80];
                      JSON_TOKENS_CHECK_OVERFLOW_ERR
                      json_fixed_key (key1, dat, tokens);
                      LOG_INSANE ("-search %s.%s\n", subclass, key1);
                      f1 = dwg_dynapi_subclass_field (subclass, key1);
                      if (f1)
                        {
                          LOG_INSANE ("-found %s [%s]\n", f1->name,
                                      f1->type);
                          if (!_set_struct_field (dat, obj, tokens,
                                                  &elems[k * size_elem],
                                                  subclass, key1, sfields))
                            ++tokens->index;
                        }
                      else if (strEQc (key1, "lt.index"))
                        {
                          if (!_set_struct_field (dat, obj, tokens,
                                                  &elems[k * size_elem],
                                                  subclass, "lt", sfields))
                            ++tokens->index;
                        }
                      else if (strchr (key1, '.')) // embedded struct
                        {
                          char *rest = strchr (key1, '.');
                          *rest = '\0';
                          rest++;
                          f1 = dwg_dynapi_subclass_field (subclass, key1);
                          if (f1 && *rest)
                            {
                              char *sb1 = dwg_dynapi_subclass_name (f1->type);
                              const Dwg_DYNAPI_field *sfields1
                                  = sb1 ? dwg_dynapi_subclass_fields (sb1)
                                        : NULL;
                              if (!sfields1
                                  || !_set_struct_field (
                                      dat, obj, tokens,
                                      &elems[(k * size_elem) + f1->offset],
                                      sb1, rest, sfields1))
                                ++tokens->index;
                              free (sb1);
                            }
                        }
                      if (!f1 || !f1->name) // not found
                        {
                          LOG_ERROR ("Unknown subclass field %s.%s", subclass,
                                     key1);
                          ++tokens->index;
                        }
                    }
                }
              if (dwg_dynapi_field_set_value (dwg, _obj, f, &elems, 1))
                LOG_TRACE ("subclass %s.%s done\n", name, key);
              free (subclass);
            }
          else
            {
            unknown_ent:
              LOG_ERROR ("Unknown type for %s.%s %s with %s", name, key,
                         f->type, t_typename[t->type]);
              ++tokens->index;
              JSON_TOKENS_CHECK_OVERFLOW_ERR
            }
          break;
        }
      else
        {
          // Currently we have 3 known static arrays
          // TODO: => vertind BS[4]
          JSON_TOKENS_CHECK_OVERFLOW_ERR
          if (t->type == JSMN_PRIMITIVE && memBEGINc (key, "vertind[")
              && strEQc (f->name, "vertind[4]"))
            {
              BITCODE_BS arr[4];
              int index;
              sscanf (key, "vertind[%d]", &index);
              if (index >= 0 && index < 4)
                {
                  dwg_dynapi_field_get_value (_obj, f, &arr);
                  arr[index] = json_long (dat, tokens);
                  LOG_TRACE ("%s: %d [%s]\n", key, (int)arr[index], f->type);
                  dwg_dynapi_field_set_value (dwg, _obj, f, &arr, 0);
                }
              else
                {
                  tokens->index++;
                }
              JSON_TOKENS_CHECK_OVERFLOW_ERR
              break;
            }
          else if (t->type == JSMN_PRIMITIVE && memBEGINc (key, "edge[")
                   && strEQc (f->name, "edge[4]"))
            {
              BITCODE_BL arr[4];
              int index;
              sscanf (key, "edge[%d]", &index);
              if (index >= 0 && index < 4)
                {
                  dwg_dynapi_field_get_value (_obj, f, &arr);
                  arr[index] = json_long (dat, tokens);
                  LOG_TRACE ("%s: %d [%s]\n", key, (int)arr[index], f->type);
                  dwg_dynapi_field_set_value (dwg, _obj, f, &arr, 0);
                }
              else
                {
                  tokens->index++;
                }
              JSON_TOKENS_CHECK_OVERFLOW_ERR
              break;
            }
          else if (t->type == JSMN_ARRAY && memBEGINc (key, "workplane[")
                   && strEQc (f->name, "workplane[3]"))
            {
              BITCODE_3BD arr[3];
              int index;
              sscanf (key, "workplane[%d]", &index);
              if (index >= 0 && index < 3)
                {
                  dwg_dynapi_field_get_value (_obj, f, &arr);
                  json_3DPOINT (dat, tokens, key, f->type, &arr[index]);
                  dwg_dynapi_field_set_value (dwg, _obj, f, &arr, 0);
                }
              else
                json_advance_unknown (dat, tokens, t->type, 0);
              JSON_TOKENS_CHECK_OVERFLOW_ERR
              break;
            }
        }
    }
  return error | (f->name ? 1 : 0); // found or not
}

/*
// check both texts[] and itemhandles[]
static void
in_postprocess_DICTIONARY (Dwg_Object *obj)
{
  Dwg_Object_DICTIONARY *_obj = obj->tio.object->tio.DICTIONARY;
  int do_free = 0;
  if (_obj->numitems == (BITCODE_BL)-1)
    {
      _obj->numitems = 0;
      do_free = 1;
      LOG_ERROR ("reset DICTIONARY, no numitems");
    }
  if ((_obj->numitems || do_free) && !_obj->texts)
    {
      LOG_ERROR ("reset DICTIONARY, no texts");
      // need to leave the handles, just free H*
      free (_obj->itemhandles);
      _obj->itemhandles = NULL;
      _obj->numitems = 0;
    }
  if ((_obj->numitems || do_free) && !_obj->itemhandles)
    {
      LOG_ERROR ("reset DICTIONARY, no itemhandles");
      for (BITCODE_BL i = 0; i < _obj->numitems; i++)
        free (_obj->texts[i]);
      free (_obj->texts);
      _obj->texts = NULL;
      _obj->numitems = 0;
    }
}
// check both texts[] and itemhandles[]
static void
in_postprocess_DICTIONARYWDFLT (Dwg_Object *obj)
{
  Dwg_Object_DICTIONARYWDFLT *_obj = obj->tio.object->tio.DICTIONARYWDFLT;
  int do_free = 0;
  if (_obj->numitems == (BITCODE_BL)-1)
    {
      _obj->numitems = 0;
      do_free = 1;
      LOG_ERROR ("reset DICTIONARYWDFLT, no numitems");
    }
  if ((_obj->numitems || do_free) && !_obj->texts)
    {
      LOG_ERROR ("reset DICTIONARYWDFLT, no texts");
      // need to leave the handles, just free H*
      free (_obj->itemhandles);
      _obj->itemhandles = NULL;
      _obj->numitems = 0;
    }
  if ((_obj->numitems || do_free) && !_obj->itemhandles)
    {
      LOG_ERROR ("reset DICTIONARYWDFLT, no itemhandles");
      for (BITCODE_BL i = 0; i < _obj->numitems; i++)
        free (_obj->texts[i]);
      free (_obj->texts);
      _obj->texts = NULL;
      _obj->numitems = 0;
    }
}
*/

static int
json_OBJECTS (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
              jsmntokens_t *restrict tokens)
{
  const char *section = "OBJECTS";
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  int size;
  if (t->type != JSMN_ARRAY || dwg->num_objects)
    {
      LOG_ERROR ("Unexpected %s at %u of %ld tokens, expected %s ARRAY",
                 t_typename[t->type], tokens->index, tokens->num_tokens,
                 section);
      json_advance_unknown (dat, tokens, t->type, 0);
      return DWG_ERR_INVALIDTYPE;
    }
  size = t->size;
  LOG_TRACE ("\n%s pos:%d [%d members]\n--------------------\n", section,
             tokens->index, size);
  tokens->index++;
  JSON_TOKENS_CHECK_OVERFLOW_ERR
  if (dwg->num_objects == 0)
    {
      // faster version of dwg_add_object()
      // round up to next REFS_PER_REALLOC
      int rounded = size;
      if (rounded % REFS_PER_REALLOC)
        rounded += REFS_PER_REALLOC - (rounded % REFS_PER_REALLOC);
      dwg->object = calloc (rounded, sizeof (Dwg_Object));
    }
  else
    dwg_add_object (dwg);
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
      const Dwg_DYNAPI_field *fields = NULL, *cfields;
      const Dwg_DYNAPI_field *f;

      JSON_TOKENS_CHECK_OVERFLOW_ERR
      if (i > 0)
        {
          Dwg_Object *oldobj = &dwg->object[i - 1];
          if (!oldobj->handle.value)
            {
              LOG_ERROR ("Required %s.handle missing, skipped", oldobj->name)
              dwg_free_object (obj);
              obj = oldobj; i--; size--;
            }
          else if (!oldobj->type)
            {
              LOG_ERROR ("Required %s.type missing, skipped", oldobj->name)
              dwg_free_object (obj);
              obj = oldobj; i--; size--;
            }
          else if (oldobj->fixedtype == DWG_TYPE_UNUSED)
            {
              LOG_ERROR ("Required %s.fixedtype missing, skipped", oldobj->name);
              dwg_free_object (obj);
              obj = oldobj; i--; size--;
            }
          if (oldobj->fixedtype == DWG_TYPE_SEQEND)
            {
              in_postprocess_SEQEND (oldobj, 0, NULL);
            }
          /*
          else if (oldobj->fixedtype == DWG_TYPE_DICTIONARY)
            {
              in_postprocess_DICTIONARY (oldobj);
            }
          else if (oldobj->fixedtype == DWG_TYPE_DICTIONARYWDFLT)
            {
              in_postprocess_DICTIONARYWDFLT (oldobj);
            }
          */
        }

      memset (obj, 0, sizeof (Dwg_Object));
      t = &tokens->tokens[tokens->index];
      if (t->type != JSMN_OBJECT)
        {
          LOG_ERROR (
              "Unexpected %s at %u of %ld tokens, expected %s OBJECT. %s:%d",
              t_typename[t->type], tokens->index, tokens->num_tokens, section,
              __FUNCTION__, __LINE__);
          json_advance_unknown (dat, tokens, t->type, 0);
          JSON_TOKENS_CHECK_OVERFLOW_ERR
          return DWG_ERR_INVALIDTYPE;
        }
      keys = t->size;
      LOG_HANDLE ("\n-keys: %d\n", keys);

      tokens->index++;
      for (int j = 0; j < keys; j++)
        {
          char key[80];
          LOG_INSANE ("[%d] ", j);
          JSON_TOKENS_CHECK_OVERFLOW_ERR
          json_fixed_key (key, dat, tokens);
          JSON_TOKENS_CHECK_OVERFLOW_ERR
          t = &tokens->tokens[tokens->index];
          if (strEQc (key, "object") && t->type == JSMN_STRING
              && i < (int)dwg->num_objects && !dwg->object[i].type)
            {
              int len = t->end - t->start;
              int objsize = 16;
              obj->supertype = DWG_SUPERTYPE_OBJECT;
              obj->parent = dwg;
              obj->index = i;

              if (len >= 80)
                {
                  LOG_ERROR ("Illegal %s name %.*s", key, len,
                             &dat->chain[t->start])
                  obj->type = obj->fixedtype = DWG_TYPE_DUMMY;
                  // exhaust the rest
                  for (; j < keys; j++)
                    {
                      json_advance_unknown (dat, tokens, t->type, 0); // value
                      tokens->index++; // next key
                      JSON_TOKENS_CHECK_OVERFLOW_ERR
                    }
                  tokens->index--;
                  break;
                }
              memcpy (name, &dat->chain[t->start], len);
              name[len] = '\0';
              is_entity = 0;
              fields = dwg_dynapi_entity_fields (name);
              objsize = dwg_dynapi_fields_size (name);
              if (!fields || !objsize || !is_dwg_object (name))
                {
                  LOG_ERROR ("Unknown object %s", name);
                  obj->type = obj->fixedtype = DWG_TYPE_DUMMY;
                  // exhaust the rest
                  for (; j < keys; j++)
                    {
                      json_advance_unknown (dat, tokens, t->type, 0); // value
                      tokens->index++; // next key
                      JSON_TOKENS_CHECK_OVERFLOW_ERR
                    }
                  tokens->index--;
                  break;
                }
              LOG_TRACE ("\nnew object %s [%d] (size: %d)\n", name, i,
                         objsize);
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
              JSON_TOKENS_CHECK_OVERFLOW_ERR
            }
          else if (strEQc (key, "entity") && t->type == JSMN_STRING
                   && i < (int)dwg->num_objects && !dwg->object[i].type)
            {
              int len = t->end - t->start;
              int objsize;
              obj->supertype = DWG_SUPERTYPE_ENTITY;
              obj->parent = dwg;
              obj->index = i;

              if (len >= 80)
                {
                  LOG_ERROR ("Illegal %s name %.*s", key, len,
                             &dat->chain[t->start])
                  obj->type = obj->fixedtype = DWG_TYPE_DUMMY;
                  // exhaust the rest
                  for (; j < keys; j++)
                    {
                      json_advance_unknown (dat, tokens, t->type, 0); // value
                      tokens->index++; // next key
                      JSON_TOKENS_CHECK_OVERFLOW_ERR
                    }
                  tokens->index--;
                  break;
                }
              memcpy (name, &dat->chain[t->start], len);
              name[len] = '\0';
              is_entity = 1;
              fields = dwg_dynapi_entity_fields (name);
              objsize = dwg_dynapi_fields_size (name);
              if (!fields || !objsize || !is_dwg_entity (name))
                {
                  LOG_ERROR ("Unknown entity %s", name);
                  obj->type = obj->fixedtype = DWG_TYPE_DUMMY;
                  // exhaust the rest
                  for (; j < keys; j++)
                    {
                      json_advance_unknown (dat, tokens, t->type, 0); // value
                      tokens->index++; // next key
                      JSON_TOKENS_CHECK_OVERFLOW_ERR
                    }
                  tokens->index--;
                  break;
                }
              LOG_TRACE ("\nnew entity %s [%d] (size: %d)\n", name, i,
                         objsize);
              obj->tio.entity = calloc (1, sizeof (Dwg_Object_Entity));
              obj->tio.entity->dwg = dwg;
              obj->tio.entity->objid = i;
              // NEW_ENTITY (dwg, obj)
              // ADD_ENTITY loop?
              _obj = calloc (1, objsize);
              obj->tio.entity->tio.POINT = (Dwg_Entity_POINT *)_obj;
              obj->tio.entity->tio.POINT->parent = obj->tio.entity;
              obj->name = strdup (name);
              // if different, the alias is done via extra dxfname key (below)
              obj->dxfname = strdup (name);
              tokens->index++;
              JSON_TOKENS_CHECK_OVERFLOW_ERR
            }
          else if (!obj || !fields)
            {
              LOG_ERROR ("Required object or entity key missing");
              json_advance_unknown (dat, tokens, t->type, 0);
              return DWG_ERR_INVALIDDWG;
            }
          else if (strEQc (key, "dxfname"))
            {
              free (obj->dxfname);
              obj->dxfname = json_string (dat, tokens);
              LOG_TRACE ("dxfname: %s\n", obj->dxfname)
              if (!obj->dxfname)
                obj->dxfname = strdup (name);
            }
          else if (strEQc (key, "index") && strNE (name, "TableCellContent_Attr"))
            {
              BITCODE_RL index = (BITCODE_RL)json_long (dat, tokens);
              if (obj->index != index)
                LOG_WARN ("Ignore wrong %s.index %d, expected %d", name, index,
                          obj->index)
              LOG_TRACE ("index: %d\n", obj->index)
            }
          else if (strEQc (key, "type") && !obj->type)
            {
              obj->type = json_long (dat, tokens);
              JSON_TOKENS_CHECK_OVERFLOW_ERR
              // clang-format off
              // ADD_ENTITY by name
              // check all objects

              // if in CLASSES keep a high type
#undef DWG_OBJECT
#define DWG_OBJECT(token)                       \
  if (strEQc (name, #token))                    \
    {                                           \
      obj->fixedtype = DWG_TYPE_##token;        \
      if ((obj->fixedtype <= DWG_TYPE_OLE2FRAME \
           || obj->type < 500)                  \
          && obj->fixedtype != obj->type)       \
        {                                       \
          LOG_WARN ("Fixup wrong type %d for %s to %d", \
                    obj->type, name, obj->fixedtype)    \
          obj->type = obj->fixedtype;           \
          break;                                \
        }                                       \
      goto found_ent;                           \
    }                                           \
  else
#undef DWG_ENTITY
#define DWG_ENTITY(token) DWG_OBJECT(token)

              #include "objects.inc"
              //final else
              if (strEQc (name, "3DFACE"))
                obj->fixedtype = DWG_TYPE__3DFACE;
              else if (strEQc (name, "3DSOLID"))
                obj->fixedtype = DWG_TYPE__3DSOLID;
              else if (strEQc (name, "VERTEX_PFACE"))
                obj->fixedtype = DWG_TYPE_VERTEX_PFACE;
              else
                LOG_WARN ("Unknown object %s", name);

#undef DWG_OBJECT
#undef DWG_ENTITY
              // clang-format on
            found_ent:
              LOG_TRACE ("type: %d,\tfixedtype: %d\n", obj->type,
                         obj->fixedtype)
            }
          // Note: also _obj->size
          else if (strEQc (key, "size") && !obj->size
                   && t->type == JSMN_PRIMITIVE)
            {
              obj->size = json_long (dat, tokens);
              JSON_TOKENS_CHECK_OVERFLOW_ERR
              if (!obj->handle.value)
                {
                  LOG_ERROR ("Required %s.handle missing", name)
                  return DWG_ERR_INVALIDDWG;
                }
              LOG_TRACE ("size: %d\n", obj->size)
            }
          else if (strEQc (key, "bitsize") && !obj->bitsize)
            {
              obj->bitsize = json_long (dat, tokens);
              LOG_TRACE ("bitsize: %d\n", obj->bitsize)
              JSON_TOKENS_CHECK_OVERFLOW_ERR
            }
          else if (strEQc (key, "handle") && !obj->handle.value)
            {
              BITCODE_H hdl = json_HANDLE (dat, dwg, tokens, key, obj, -1);
              JSON_TOKENS_CHECK_OVERFLOW_ERR
              if (hdl)
                {
                  obj->handle.code = hdl->handleref.code;
                  obj->handle.size = hdl->handleref.size;
                  obj->handle.value = hdl->handleref.value;
                }
              if (!obj->type) // TODO: We could eventually relax this
                {
                  LOG_ERROR ("Required %s.type missing", name)
                  return DWG_ERR_INVALIDDWG;
                }
            }
          // ignore subclass markers
          else if (t->type == JSMN_STRING && strEQc (key, "_subclass"))
            {
              LOG_TRACE ("_subclass: %.*s\n", t->end - t->start, &dat->chain[t->start]);
              tokens->index++;
              JSON_TOKENS_CHECK_OVERFLOW_ERR
            }
          else if (strEQc (key, "num_unknown_bits")
                   && memBEGINc (obj->name, "UNKNOWN_"))
            {
              obj->num_unknown_bits = json_long (dat, tokens);
              JSON_TOKENS_CHECK_OVERFLOW_ERR
              LOG_TRACE ("num_unknown_bits: %d\n", (int)obj->num_unknown_bits);
            }
          else if (strEQc (key, "unknown_bits")
                   && memBEGINc (obj->name, "UNKNOWN_"))
            {
              int len = t->end - t->start;
              char *hex = json_string (dat, tokens);
              unsigned blen = len / 2;
              BITCODE_TF buf = malloc (blen + 1);
              char *pos = hex;
              char *old;
              for (unsigned k = 0; k < blen; k++)
                {
                  sscanf (pos, "%2hhX", &buf[k]);
                  pos += 2;
                }
              buf[blen] = '\0';
              free (hex);
              if (!obj->num_unknown_bits)
                obj->num_unknown_bits = blen * 8; // minus some padding bits
              obj->unknown_bits = buf;
              // LOG_TRACE ("%s: '%.*s' [%s] (binary)\n", key, blen, buf,
              // f->type);
              LOG_TRACE ("unknown_bits: %.*s\n", t->end - t->start,
                         &dat->chain[t->start])
            }
          else if (strEQc (key, "eed") && !obj->tio.object->num_eed
                   && t->type == JSMN_ARRAY)
            {
              json_eed (dat, dwg, tokens, obj->tio.object);
              JSON_TOKENS_CHECK_OVERFLOW_ERR
            }
          else
            // search_field:
            {
              if (_set_struct_field (dat, obj, tokens, _obj, name, key,
                                     fields))
                continue;
              if (is_entity)
                {
                  if (_set_struct_field (dat, obj, tokens, obj->tio.entity,
                                         name, key,
                                         dwg_dynapi_common_entity_fields ()))
                    continue;
                }
              else
                {
                  if (_set_struct_field (dat, obj, tokens, obj->tio.object,
                                         name, key,
                                         dwg_dynapi_common_object_fields ()))
                    continue;
                }
              // first the MLEADER_AnnotContext union
              if (strEQc (name, "MULTILEADER"))
                {
                  // embedded structs
                  if (memBEGINc (key, "ctx.content.txt.")
                      || memBEGINc (key, "ctx.content.blk."))
                    {
                      Dwg_Entity_MULTILEADER *_o
                          = (Dwg_Entity_MULTILEADER *)_obj;
                      Dwg_MLEADER_Content *cnt = &_o->ctx.content;
                      const Dwg_DYNAPI_field *sf
                          = dwg_dynapi_subclass_fields ("MLEADER_Content");
                      if (sf
                          && _set_struct_field (
                              dat, obj, tokens, cnt, "MLEADER",
                              &key[strlen ("ctx.content.")], sf))
                        continue;
                    }
                  else if (memBEGINc (key, "ctx."))
                    {
                      Dwg_Entity_MULTILEADER *_o
                          = (Dwg_Entity_MULTILEADER *)_obj;
                      Dwg_MLEADER_AnnotContext *ctx = &_o->ctx;
                      const Dwg_DYNAPI_field *sf = dwg_dynapi_subclass_fields (
                          "MLEADER_AnnotContext");
                      if (sf
                          && _set_struct_field (dat, obj, tokens, ctx,
                                                "MLEADER", &key[4], sf))
                        continue;
                    }
                }
              else if (t->type == JSMN_OBJECT && memBEGINc (name, "DICTIONARY") && strEQc (key, "items"))
                {
                  Dwg_Object_DICTIONARY *o = obj->tio.object->tio.DICTIONARY;
                  o->numitems = t->size;
                  o->texts = o->numitems ? calloc (o->numitems, sizeof (BITCODE_T)) : NULL;
                  o->itemhandles = o->numitems ? calloc (o->numitems, sizeof (BITCODE_H)) : NULL;
                  tokens->index++;
                  for (int k = 0; k < (int)o->numitems; k++)
                    {
                      JSON_TOKENS_CHECK_OVERFLOW_ERR;
                      t = &tokens->tokens[tokens->index];
                      SINCE (R_2007)
                        o->texts[k] = (BITCODE_T)json_wstring (dat, tokens);
                      else
                        o->texts[k] = json_string (dat, tokens);
                      LOG_TRACE ("texts[%d]: %.*s\t => ", k, t->end - t->start, &dat->chain[t->start]);
                      JSON_TOKENS_CHECK_OVERFLOW_ERR;
                      o->itemhandles[k] = json_HANDLE (dat, dwg, tokens, "itemhandles", obj, k);
                    }
                  if (!o->numitems)
                    LOG_TRACE ("%s.%s empty\n", name, key);
                  continue;
                }
              // or starts with an embedded subclass, like TABLE_value "value.xxx"
              if (strchr (key, '.'))
                {
                  const Dwg_DYNAPI_field *f1;
                  char *rest = strchr (key, '.');
                  *rest = '\0';
                  rest++;
                  // find field for key prefix, like value.
                  f1 = dwg_dynapi_entity_field (name, key);
                  if (f1 && *rest)
                    {
                      void *off = &((char *)_obj)[f1->offset];
                      char *subclass = dwg_dynapi_subclass_name (f1->type);
                      const Dwg_DYNAPI_field *sfields
                          = subclass ? dwg_dynapi_subclass_fields (subclass)
                                     : NULL;
                      // subclass offset for _obj
                      if (sfields
                          && _set_struct_field (dat, obj, tokens, off,
                                                subclass, rest, sfields))
                        {
                          free (subclass);
                          continue;
                        }
                      free (subclass);
                    }
                }
              LOG_ERROR ("Unknown %s.%s %.*s ignored\n", name, key,
                         t->end - t->start, &dat->chain[t->start]);
              json_advance_unknown (dat, tokens, t->type, 0);
              JSON_TOKENS_CHECK_OVERFLOW_ERR
            }
        }
    }
  LOG_TRACE ("End of %s\n", section)
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
      LOG_ERROR ("Unexpected %s at %u of %ld tokens, expected %s OBJECT",
                 t_typename[t->type], tokens->index, tokens->num_tokens,
                 section);
      json_advance_unknown (dat, tokens, t->type, 0);
      return DWG_ERR_INVALIDTYPE;
    }
  size = t->size;
  LOG_TRACE ("\n%s pos:%d [%d members]\n--------------------\n", section,
             tokens->index, size);
  // Maybe create dwg->header.section[SECTION_HANDLES_R13] omap here.
  // struct { uint32_t hdloff; int32_t offset } *omap = calloc (size, 8);
  for (int i = 0; i < size; i++)
    {
      tokens->index++;
      JSON_TOKENS_CHECK_OVERFLOW_ERR
      for (int k = 0; k < 2; k++)
        {
          long hdloff = json_long (dat, tokens);
          long offset = json_long (dat, tokens);
          tokens->index++;
          JSON_TOKENS_CHECK_OVERFLOW_ERR
        }
    }
  return 0;
}

static int
json_THUMBNAILIMAGE (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                     jsmntokens_t *restrict tokens)
{
  const char *section = "THUMBNAILIMAGE";
  const jsmntok_t *restrict t = &tokens->tokens[tokens->index];
  Dwg_Chain *restrict _obj = &dwg->thumbnail;
  int size;
  long size1 = 0;
  if (t->type != JSMN_OBJECT)
    {
      LOG_ERROR ("Unexpected %s at %u of %ld tokens, expected %s OBJECT",
                 t_typename[t->type], tokens->index, tokens->num_tokens,
                 section);
      json_advance_unknown (dat, tokens, t->type, 0);
      return DWG_ERR_INVALIDTYPE;
    }
  size = t->size;
  LOG_TRACE ("\n%s pos:%d [%d keys]\n--------------------\n", section,
             tokens->index, size);
  tokens->index++;
  for (int i = 0; i < size; i++)
    {
      char key[80];
      JSON_TOKENS_CHECK_OVERFLOW_ERR
      json_fixed_key (key, dat, tokens);
      t = &tokens->tokens[tokens->index];
      if (strEQc (key, "size"))
        size1 = json_long (dat, tokens);
      else if (strEQc (key, "chain"))
        {
          unsigned long len;
          dwg->thumbnail.chain
              = (unsigned char *)json_binary (dat, tokens, key, &len);
          JSON_TOKENS_CHECK_OVERFLOW_ERR
          dwg->thumbnail.size = len;
          if (size1 > 0 && size1 != (long)len)
            LOG_WARN ("thumbnail size mismatch: binary len %lu != size %ld",
                      len, size1);
          LOG_TRACE ("size: %lu\n", len);
        }
      else
        {
          LOG_TRACE ("%s\n", key)
          json_advance_unknown (dat, tokens, t->type, 0);
          JSON_TOKENS_CHECK_OVERFLOW_ERR
        }
    }

  LOG_TRACE ("End of %s\n", section)
  tokens->index--;
  return 0;
}

static int
json_R2004_Header (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                   jsmntokens_t *restrict tokens)
{
  const char *section = "R2004_Header";
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  Dwg_R2004_Header *_obj = &dwg->r2004_header;
  int size;
  if (t->type != JSMN_OBJECT)
    {
      LOG_ERROR ("Unexpected %s at %u of %ld tokens, expected %s OBJECT",
                 t_typename[t->type], tokens->index, tokens->num_tokens,
                 section);
      json_advance_unknown (dat, tokens, t->type, 0);
      return DWG_ERR_INVALIDTYPE;
    }
  size = t->size;
  LOG_TRACE ("\n%s pos:%d [%d keys]\n--------------------\n", section,
             tokens->index, size);
  tokens->index++;
  for (int i = 0; i < size; i++)
    {
      char key[80];
      JSON_TOKENS_CHECK_OVERFLOW_ERR
      json_fixed_key (key, dat, tokens);
      t = &tokens->tokens[tokens->index];

      // FIELD_TFFx (file_ID_string, 12, 0) //pre-allocated
      if (strEQc (key, "file_ID_string"))
        {
          unsigned long slen;
#if 0
          char *s = json_binary (dat, tokens, key, &slen);
#else
          char *s = json_string (dat, tokens);
          slen = strlen (s);
#endif
          JSON_TOKENS_CHECK_OVERFLOW_ERR
          if (slen == 12)
            memcpy (&_obj->file_ID_string, s, 12);
          else
            {
              LOG_ERROR ("Invalid R2004_Header.file_ID_string fixed")
              memcpy (&_obj->file_ID_string, "AcFssFcAJMB\0", 12);
            }
          LOG_TRACE ("file_ID_string: \"%.*s\"\n", 12, _obj->file_ID_string)
          free (s);
        }
      else if (strEQc (key, "padding"))
        {
          // just zeros encrypted */
          LOG_TRACE ("padding: \"%.*s\" (ignored)\n", t->end - t->start,
                     &dat->chain[t->start])
          tokens->index++;
        }
      // clang-format off
      FIELD_RLx (header_address, 0)
      FIELD_RL (header_size, 0)
      FIELD_RL (x04, 0)
      FIELD_RLd (root_tree_node_gap, 0)
      FIELD_RLd (lowermost_left_tree_node_gap, 0)
      FIELD_RLd (lowermost_right_tree_node_gap, 0)
      FIELD_RL (unknown_long, 0)
      FIELD_RL (last_section_id, 0)
      FIELD_RLL (last_section_address, 0)
      FIELD_RLL (second_header_address, 0)
      FIELD_RL (numgaps, 0)
      FIELD_RL (numsections, 0)
      FIELD_RL (x20, 0)
      FIELD_RL (x80, 0)
      FIELD_RL (x40, 0)
      FIELD_RL (section_map_id, 0)
      FIELD_RLL (section_map_address, 0)
      FIELD_RL (section_info_id, 0)
      FIELD_RL (section_array_size, 0)
      FIELD_RL (gap_array_size, 0)
      FIELD_RLx (crc32, 0)
      // clang-format on
      // end of encrypted 0x6c header
      else
        {
          LOG_ERROR ("Unknown %s.%s ignored", section, key);
          tokens->index++;
        }
    }

  LOG_TRACE ("End of %s\n", section)
  tokens->index--;
  return 0;
}

static int
json_AuxHeader (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                jsmntokens_t *restrict tokens)
{
  const char *section = "AuxHeader";
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  struct Dwg_AuxHeader *_obj = &dwg->auxheader;
  int size;
  if (t->type != JSMN_OBJECT)
    {
      LOG_ERROR ("Unexpected %s at %u of %ld tokens, expected %s OBJECT",
                 t_typename[t->type], tokens->index, tokens->num_tokens,
                 section);
      json_advance_unknown (dat, tokens, t->type, 0);
      return DWG_ERR_INVALIDTYPE;
    }
  size = t->size;
  LOG_TRACE ("\n%s pos:%d [%d keys]\n--------------------\n", section,
             tokens->index, size);
  tokens->index++;
  for (int i = 0; i < size; i++)
    {
      char key[80];
      JSON_TOKENS_CHECK_OVERFLOW_ERR
      json_fixed_key (key, dat, tokens);
      t = &tokens->tokens[tokens->index];

      // clang-format off
      if (0) ; // else
      FIELD_VECTOR_INL (aux_intro, RC, 3, 0)
      FIELD_RSx (dwg_version, 0)
      FIELD_RS (maint_version, 0)
      FIELD_RL (numsaves, 0)
      FIELD_RL (minus_1, 0)
      FIELD_RS (numsaves_1, 0)
      FIELD_RS (numsaves_2, 0)
      FIELD_RL (zero, 0)
      FIELD_RSx (dwg_version_1, 0)
      FIELD_RS (maint_version_1, 0)
      FIELD_RSx (dwg_version_2, 0)
      FIELD_RS (maint_version_2, 0)
      FIELD_VECTOR_INL (unknown_6rs, RS, 6, 0)
      FIELD_VECTOR_INL (unknown_5rl, RL, 5, 0)
      FIELD_RD (TDCREATE, 0)
      FIELD_RD (TDUPDATE, 0)
      FIELD_RLx (HANDSEED, 0)
      FIELD_RL (plot_stamp, 0)
      FIELD_RS (zero_1, 0)
      FIELD_RS (numsaves_3, 0)
      FIELD_RL (zero_2, 0)
      FIELD_RL (zero_3, 0)
      FIELD_RL (zero_4, 0)
      FIELD_RL (numsaves_4, 0)
      FIELD_RL (zero_5, 0)
      FIELD_RL (zero_6, 0)
      FIELD_RL (zero_7, 0)
      FIELD_RL (zero_8, 0)
      FIELD_VECTOR_INL (zero_18, RS, 3, 0)
      // clang-format on
      else
        {
          LOG_ERROR ("Unknown %s.%s ignored", section, key);
          tokens->index++;
        }
    }

  LOG_TRACE ("End of %s\n", section)
  tokens->index--;
  return 0;
}

static int
json_SummaryInfo (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                  jsmntokens_t *restrict tokens)
{
  const char *section = "SummaryInfo";
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  struct Dwg_SummaryInfo *_obj = &dwg->summaryinfo;
  int size;
  if (t->type != JSMN_OBJECT)
    {
      LOG_ERROR ("Unexpected %s at %u of %ld tokens, expected %s OBJECT",
                 t_typename[t->type], tokens->index, tokens->num_tokens,
                 section);
      json_advance_unknown (dat, tokens, t->type, 0);
      return DWG_ERR_INVALIDTYPE;
    }
  size = t->size;
  LOG_TRACE ("\n%s pos:%d [%d keys]\n--------------------\n", section,
             tokens->index, size);
  tokens->index++;
  for (int i = 0; i < size; i++)
    {
      int size1;
      char key[80];
      JSON_TOKENS_CHECK_OVERFLOW_ERR
      json_fixed_key (key, dat, tokens);
      t = &tokens->tokens[tokens->index];

      if (strEQc (key, "num_props"))
        {
          if (t->type == JSMN_PRIMITIVE)
            tokens->index++;
          else
            json_advance_unknown (dat, tokens, t->type, 0);
        }
      else if (strEQc (key, "props"))
        {
          if (t->type != JSMN_ARRAY)
            {
              LOG_ERROR (
                  "Unexpected %s at %u of %ld tokens, expected %s.%s ARRAY",
                  t_typename[t->type], tokens->index, tokens->num_tokens,
                  section, key);
              json_advance_unknown (dat, tokens, t->type, 0);
              return DWG_ERR_INVALIDTYPE;
            }
          size1 = t->size;
          LOG_TRACE ("\n%s pos:%d [%d members]\n--------------------\n",
                     "SummaryInfo_Property", tokens->index, size);
          _obj->props = calloc (size1, sizeof (Dwg_SummaryInfo_Property));
          _obj->num_props = size1;
          tokens->index++;
          for (int j = 0; j < size1; j++)
            {
              JSON_TOKENS_CHECK_OVERFLOW_ERR
              t = &tokens->tokens[tokens->index];
              if (t->type != JSMN_OBJECT)
                {
                  LOG_ERROR ("Unexpected %s at %u of %ld tokens, expected %s.%s OBJECT",
                             t_typename[t->type], tokens->index, tokens->num_tokens,
                             section, key);
                  json_advance_unknown (dat, tokens, t->type, 0);
                  return DWG_ERR_INVALIDTYPE;
                }
              tokens->index++; // OBJECT of 2: key, value. TODO: array of 2
              json_fixed_key (key, dat, tokens);
              JSON_TOKENS_CHECK_OVERFLOW_ERR
              t = &tokens->tokens[tokens->index];
              if (t->type == JSMN_STRING) // key
                _obj->props[j].key = json_string (dat, tokens);
              else if (t->type == JSMN_PRIMITIVE)
                tokens->index++;
              else
                json_advance_unknown (dat, tokens, t->type, 0);

              json_fixed_key (key, dat, tokens);
              JSON_TOKENS_CHECK_OVERFLOW_ERR
              t = &tokens->tokens[tokens->index];
              if (t->type == JSMN_STRING) // value
                _obj->props[j].value = json_string (dat, tokens);
              else if (t->type == JSMN_PRIMITIVE)
                tokens->index++;
              else
                json_advance_unknown (dat, tokens, t->type, 0);
              if (_obj->props[j].key || _obj->props[j].value)
                LOG_TRACE ("props[%d] = (%s,%s)\n", j, _obj->props[j].key,
                           _obj->props[j].value)
            }
        }
      // clang-format off
      FIELD_T (TITLE, 1)
      FIELD_T (SUBJECT, 1)
      FIELD_T (AUTHOR, 1)
      FIELD_T (KEYWORDS, 1)
      FIELD_T (COMMENTS, 1)
      FIELD_T (LASTSAVEDBY, 1)
      FIELD_T (REVISIONNUMBER, 1)
      FIELD_T (HYPERLINKBASE, 1)
      FIELD_TIMERLL (TDINDWG, 0)
      FIELD_TIMERLL (TDCREATE, 0)
      FIELD_TIMERLL (TDUPDATE, 0)
      FIELD_RL (unknown1, 0)
      FIELD_RL (unknown2, 0)
      else
        {
          LOG_ERROR ("Unknown %s.%s ignored", section, key);
          json_advance_unknown (dat, tokens, t->type, 0);
        }
      // clang-format on
    }

  LOG_TRACE ("End of %s\n", section)
  tokens->index--;
  return 0;
}

static int
json_AppInfo (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
              jsmntokens_t *restrict tokens)
{
  const char *section = "AppInfo";
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  struct Dwg_AppInfo *_obj = &dwg->appinfo;
  int size;
  if (t->type != JSMN_OBJECT)
    {
      LOG_ERROR ("Unexpected %s at %u of %ld tokens, expected %s OBJECT",
                 t_typename[t->type], tokens->index, tokens->num_tokens,
                 section);
      json_advance_unknown (dat, tokens, t->type, 0);
      return DWG_ERR_INVALIDTYPE;
    }
  size = t->size;
  LOG_TRACE ("\n%s pos:%d [%d keys]\n--------------------\n", section,
             tokens->index, size);
  tokens->index++;
  for (int i = 0; i < size; i++)
    {
      char key[80];
      JSON_TOKENS_CHECK_OVERFLOW_ERR
      json_fixed_key (key, dat, tokens);
      //LOG_TRACE ("%s\n", key)
      t = &tokens->tokens[tokens->index];

      // clang-format off
      if (0) ;
      FIELD_RL (class_version, 0)
      FIELD_TU16 (appinfo_name, 0)
      FIELD_RL (num_strings, 0)
      FIELD_TFFx (version_checksum, 16, 0)
      FIELD_TU16 (version, 0)
      FIELD_TFFx (comment_checksum, 16, 0)
      FIELD_TU16 (comment, 0)
      FIELD_TFFx (product_checksum, 16, 0)
      FIELD_TU16 (product_info, 0)
      else
        {
          LOG_ERROR ("Unknown %s.%s ignored", section, key);
          json_advance_unknown (dat, tokens, t->type, 0);
        }
      // clang-format on
    }

  LOG_TRACE ("End of %s\n", section)
  tokens->index--;
  return 0;
}

static int
json_AppInfoHistory (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                     jsmntokens_t *restrict tokens)
{
  const char *section = "AppInfoHistory";
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  struct Dwg_AppInfoHistory *_obj = &dwg->appinfohistory;
  int size;
  if (t->type != JSMN_OBJECT)
    {
      LOG_ERROR ("Unexpected %s at %u of %ld tokens, expected %s OBJECT",
                 t_typename[t->type], tokens->index, tokens->num_tokens,
                 section);
      json_advance_unknown (dat, tokens, t->type, 0);
      return DWG_ERR_INVALIDTYPE;
    }
  size = t->size;
  LOG_TRACE ("\n%s pos:%d [%d keys]\n--------------------\n", section,
             tokens->index, size);
  tokens->index++;
  for (int i = 0; i < size; i++)
    {
      char key[80];
      JSON_TOKENS_CHECK_OVERFLOW_ERR
      json_fixed_key (key, dat, tokens);
      //LOG_TRACE ("%s\n", key)
      t = &tokens->tokens[tokens->index];
      // clang-format off
      if (0) ;
      FIELD_RL (size, 0)
      FIELD_BINARY (unknown_bits, size, 0)
      else
        {
          LOG_ERROR ("Unknown %s.%s ignored", section, key);
          json_advance_unknown (dat, tokens, t->type, 0);
        }
      // clang-format on
    }

  LOG_TRACE ("End of %s\n", section)
  tokens->index--;
  return 0;
}

static int
json_FileDepList_Files (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                        jsmntokens_t *restrict tokens,
                        struct Dwg_FileDepList *o, int size)
{
  const char *section = "FileDepList_Files";
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  if (t->type != JSMN_ARRAY)
    {
      LOG_ERROR ("Unexpected %s at %u of %ld tokens, expected %s ARRAY",
                 t_typename[t->type], tokens->index, tokens->num_tokens,
                 section);
      json_advance_unknown (dat, tokens, t->type, 0);
      return DWG_ERR_INVALIDTYPE;
    }
  o->files = calloc (size, sizeof (Dwg_FileDepList_Files));
  o->num_files = size;
  for (int j = 0; j < size; j++)
    {
      int keys;
      Dwg_FileDepList_Files *_obj = &o->files[j];
      tokens->index++;
      JSON_TOKENS_CHECK_OVERFLOW_ERR
      t = &tokens->tokens[tokens->index];
      keys = t->size;
      if (t->type != JSMN_OBJECT)
        {
          LOG_ERROR ("Unexpected %s at %u of %ld tokens, expected %s OBJECT",
                     t_typename[t->type], tokens->index, tokens->num_tokens,
                     section);
          json_advance_unknown (dat, tokens, t->type, 0);
          return DWG_ERR_INVALIDTYPE;
        }
      assert (t->type == JSMN_OBJECT);
      tokens->index++;
      for (int k = 0; k < keys; k++)
        {
          char key[80];
          JSON_TOKENS_CHECK_OVERFLOW_ERR
          json_fixed_key (key, dat, tokens);
          t = &tokens->tokens[tokens->index];
          // clang-format off
          if (0) ; // else
          FIELD_T32 (filename, 0)
          FIELD_T32 (filepath, 0)
          FIELD_T32 (fingerprint, 0)
          FIELD_T32 (version, 0)
          FIELD_RL (feature_index, 0)
          FIELD_RL(timestamp, 0)
          FIELD_RL (filesize, 0)
          FIELD_RL (affects_graphics, 0)
          FIELD_RL (refcount, 0)
          else
            {
              LOG_ERROR ("Unknown %s.%s ignored", section, key);
              json_advance_unknown (dat, tokens, t->type, 0);
            }
          // clang-format on
        }
      tokens->index--;
    }
  tokens->index++;
  return 0;
}

static int
json_FileDepList (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                  jsmntokens_t *restrict tokens)
{
  int error = 0;
  const char *section = "FileDepList";
  struct Dwg_FileDepList *_obj = &dwg->filedeplist;
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  int size;

  if (t->type != JSMN_OBJECT)
    {
      LOG_ERROR ("Unexpected %s at %u of %ld tokens, expected %s OBJECT",
                 t_typename[t->type], tokens->index, tokens->num_tokens,
                 section);
      json_advance_unknown (dat, tokens, t->type, 0);
      return DWG_ERR_INVALIDTYPE;
    }
  size = t->size;
  LOG_TRACE ("\n%s pos:%d [%d keys]\n--------------------\n", section,
             tokens->index, size);
  tokens->index++;
  for (int i = 0; i < size; i++)
    {
      char key[80];
      JSON_TOKENS_CHECK_OVERFLOW_ERR
      json_fixed_key (key, dat, tokens);
      LOG_TRACE ("%s\n", key)
      t = &tokens->tokens[tokens->index];
      if (strEQc (key, "features")) // TV[]
        {
          if (t->type != JSMN_ARRAY)
            json_advance_unknown (dat, tokens, t->type, 0);
          else
            {
              int size1 = t->size;
              _obj->features = calloc (size1, sizeof (BITCODE_TV));
              _obj->num_features = size1;
              tokens->index++;
              for (int j = 0; j < size1; j++)
                {
                  JSON_TOKENS_CHECK_OVERFLOW_ERR
                  t = &tokens->tokens[tokens->index];
                  if (t->type == JSMN_STRING)
                    {
                      _obj->features[j] = json_string (dat, tokens);
                      LOG_TRACE ("  %s\n", _obj->features[j]);
                    }
                  else if (t->type == JSMN_PRIMITIVE)
                    tokens->index++;
                  else
                    json_advance_unknown (dat, tokens, t->type, 0);
                }
            }
        }
      else if (strEQc (key, "files"))
        {
          if (t->type != JSMN_ARRAY) // of OBJECTs
            json_advance_unknown (dat, tokens, t->type, 0);
          else if (t->size)
            error |= json_FileDepList_Files (dat, dwg, tokens, _obj, t->size);
          else
            tokens->index++; // empty array
          if (error >= DWG_ERR_CRITICAL)
            return error;
        }
      else
        {
          LOG_ERROR ("Unknown %s.%s ignored", section, key);
          json_advance_unknown (dat, tokens, t->type, 0);
        }
    }

  LOG_TRACE ("End of %s\n", section)
  tokens->index--;
  return 0;
}

static int
json_Security (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
               jsmntokens_t *restrict tokens)
{
  const char *section = "Security";
  struct Dwg_Security *_obj = &dwg->security;
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  int size;
  if (t->type != JSMN_OBJECT)
    {
      LOG_ERROR ("Unexpected %s at %u of %ld tokens, expected %s OBJECT",
                 t_typename[t->type], tokens->index, tokens->num_tokens,
                 section);
      json_advance_unknown (dat, tokens, t->type, 0);
      return DWG_ERR_INVALIDTYPE;
    }
  size = t->size;
  LOG_TRACE ("\n%s pos:%d [%d keys]\n--------------------\n", section,
             tokens->index, size);
  tokens->index++;
  for (int i = 0; i < size; i++)
    {
      char key[80];
      JSON_TOKENS_CHECK_OVERFLOW_ERR
      json_fixed_key (key, dat, tokens);
      t = &tokens->tokens[tokens->index];
      // clang-format off
      if (0) ;
      FIELD_RLx (unknown_1, 0)
      FIELD_RLx (unknown_2, 0)
      FIELD_RLx (unknown_3, 0)
      FIELD_RL (crypto_id, 0)
      FIELD_T32 (crypto_name, 0)
      FIELD_RL (algo_id, 0)
      FIELD_RL (key_len, 0)
      FIELD_RL (encr_size, 0)
      FIELD_BINARY (encr_buffer, encr_size, 0)
      else
        {
          LOG_ERROR ("Unknown %s.%s ignored", section, key);
          json_advance_unknown (dat, tokens, t->type, 0);
        }
      // clang-format on
    }

  LOG_TRACE ("End of %s\n", section)
  tokens->index--;
  return 0;
}

static int
json_RevHistory (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                 jsmntokens_t *restrict tokens)
{
  const char *section = "RevHistory";
  struct Dwg_RevHistory *_obj = &dwg->revhistory;
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  int size;
  if (t->type != JSMN_OBJECT)
    {
      LOG_ERROR ("Unexpected %s at %u of %ld tokens, expected %s OBJECT",
                 t_typename[t->type], tokens->index, tokens->num_tokens,
                 section);
      json_advance_unknown (dat, tokens, t->type, 0);
      return DWG_ERR_INVALIDTYPE;
    }
  size = t->size;
  LOG_TRACE ("\n%s pos:%d [%d keys]\n--------------------\n", section,
             tokens->index, size);
  tokens->index++;
  for (int i = 0; i < size; i++)
    {
      char key[80];
      JSON_TOKENS_CHECK_OVERFLOW_ERR
      json_fixed_key (key, dat, tokens);
      t = &tokens->tokens[tokens->index];
      // clang-format off
      if (0) ;
      FIELD_RL (class_version, 0)
      FIELD_RL (class_minor, 0)
      FIELD_RL (num_histories, 0)
      FIELD_VECTOR (histories, RL, num_histories, 0)
      else
        {
          LOG_ERROR ("Unknown %s.%s ignored", section, key);
          json_advance_unknown (dat, tokens, t->type, 0);
        }
      // clang-format on
    }

  LOG_TRACE ("End of %s\n", section)
  tokens->index--;
  return 0;
}

static int
json_ObjFreeSpace (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                   jsmntokens_t *restrict tokens)
{
  const char *section = "ObjFreeSpace";
  struct Dwg_ObjFreeSpace *_obj = &dwg->objfreespace;
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  int size;
  if (t->type != JSMN_OBJECT)
    {
      LOG_ERROR ("Unexpected %s at %u of %ld tokens, expected %s OBJECT",
                 t_typename[t->type], tokens->index, tokens->num_tokens,
                 section);
      json_advance_unknown (dat, tokens, t->type, 0);
      return DWG_ERR_INVALIDTYPE;
    }
  size = t->size;
  LOG_TRACE ("\n%s pos:%d [%d keys]\n--------------------\n", section,
             tokens->index, size);
  tokens->index++;
  for (int i = 0; i < size; i++)
    {
      char key[80];
      JSON_TOKENS_CHECK_OVERFLOW_ERR
      json_fixed_key (key, dat, tokens);
      // t = &tokens->tokens[tokens->index];
      // clang-format off
      if (0) ;
      FIELD_RLL (zero, 0)
      FIELD_RLL (num_handles, 0)
      FIELD_TIMERLL (TDUPDATE, 0)
      FIELD_RC (num_nums, 0)
      FIELD_RLL (max32, 0)
      FIELD_RLL (max32_hi, 0)
      FIELD_RLL (max64, 0)
      FIELD_RLL (max64_hi, 0)
      FIELD_RLL (maxtbl, 0)
      FIELD_RLL (maxtbl_hi, 0)
      FIELD_RLL (maxrl, 0)
      FIELD_RLL (maxrl_hi, 0)
      FIELD_RL (objects_address, 0)
      else
        {
          LOG_ERROR ("Unknown %s.%s ignored", section, key);
          json_advance_unknown (dat, tokens, t->type, 0);
        }
      // clang-format on
    }

  LOG_TRACE ("End of %s\n", section)
  tokens->index--;
  return 0;
}

static int
json_AcDs_SegmentIndex (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                        jsmntokens_t *restrict tokens,
                        struct Dwg_AcDs *o, int size)
{
  const char *section = "AcDs_SegmentIndex";
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  if (t->type != JSMN_ARRAY)
    {
      LOG_ERROR ("Unexpected %s at %u of %ld tokens, expected %s ARRAY",
                 t_typename[t->type], tokens->index, tokens->num_tokens,
                 section);
      json_advance_unknown (dat, tokens, t->type, 0);
      return DWG_ERR_INVALIDTYPE;
    }
  o->segidx = calloc (size, sizeof (Dwg_AcDs_SegmentIndex));
  o->num_segidx = size;
  for (int j = 0; j < size; j++)
    {
      int keys;
      Dwg_AcDs_SegmentIndex *_obj = &o->segidx[j];
      tokens->index++;
      JSON_TOKENS_CHECK_OVERFLOW_ERR
      t = &tokens->tokens[tokens->index];
      keys = t->size;
      if (t->type != JSMN_OBJECT)
        {
          LOG_ERROR ("Unexpected %s at %u of %ld tokens, expected %s OBJECT",
                     t_typename[t->type], tokens->index, tokens->num_tokens,
                     section);
          json_advance_unknown (dat, tokens, t->type, 0);
          return DWG_ERR_INVALIDTYPE;
        }
      assert (t->type == JSMN_OBJECT);
      tokens->index++;
      LOG_TRACE ("segidx[%d]:\n", j);
      for (int k = 0; k < keys; k++)
        {
          char key[80];
          JSON_TOKENS_CHECK_OVERFLOW_ERR
          json_fixed_key (key, dat, tokens);
          t = &tokens->tokens[tokens->index];
          // clang-format off
          if (strEQc (key, "index"))
            tokens->index++; // ignore
          FIELD_RLL (offset, 0)
          FIELD_RL (size, 0)
          else
            {
              LOG_ERROR ("Unknown %s.%s ignored", section, key);
              json_advance_unknown (dat, tokens, t->type, 0);
            }
          // clang-format on
        }
      tokens->index--;
    }
  tokens->index++;
  return 0;
}

static int
json_AcDs_Segments (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                    jsmntokens_t *restrict tokens,
                    struct Dwg_AcDs *o, int size)
{
  const char *section = "AcDs_Segment";
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  if (t->type != JSMN_ARRAY)
    {
      LOG_ERROR ("Unexpected %s at %u of %ld tokens, expected %s ARRAY",
                 t_typename[t->type], tokens->index, tokens->num_tokens,
                 section);
      json_advance_unknown (dat, tokens, t->type, 0);
      return DWG_ERR_INVALIDTYPE;
    }
  o->segments = calloc (size, sizeof (Dwg_AcDs_Segment));
  //o->num_segidx = size;
  for (int j = 0; j < size; j++)
    {
      int keys;
      Dwg_AcDs_Segment *_obj = &o->segments[j];
      tokens->index++;
      JSON_TOKENS_CHECK_OVERFLOW_ERR
      t = &tokens->tokens[tokens->index];
      keys = t->size;
      if (t->type != JSMN_OBJECT)
        {
          LOG_ERROR ("Unexpected %s at %u of %ld tokens, expected %s OBJECT",
                     t_typename[t->type], tokens->index, tokens->num_tokens,
                     section);
          json_advance_unknown (dat, tokens, t->type, 0);
          return DWG_ERR_INVALIDTYPE;
        }
      assert (t->type == JSMN_OBJECT);
      tokens->index++;
      LOG_TRACE ("segments[%d]:\n", j);
      for (int k = 0; k < keys; k++)
        {
          char key[80];
          JSON_TOKENS_CHECK_OVERFLOW_ERR
          json_fixed_key (key, dat, tokens);
          t = &tokens->tokens[tokens->index];
          // clang-format off
          if (strEQc (key, "index"))
            tokens->index++; // ignore
          FIELD_RLx (signature, 0)
          FIELD_TFF (name, 6, 0)
          FIELD_RL (type, 0)
          FIELD_RL (segment_idx, 0)
          FIELD_RL (is_blob01, 0)
          FIELD_RL (segsize, 0)
          FIELD_RL (unknown_2, 0)
          FIELD_RL (ds_version, 0)
          FIELD_RL (unknown_3, 0)
          FIELD_RL (data_algn_offset, 0)
          FIELD_RL (objdata_algn_offset, 0)
          FIELD_TFF (padding, 8, 0)
          else if (strEQc (key, "datidx.entries"))
            {
              o->datidx.entries = (Dwg_AcDs_DataIndex_Entry*)json_records (dat, tokens,
                  o, "AcDs_DataIndex_Entry", &o->datidx.num_entries);
            }
          else if (strEQc (key, "di_unknown"))
            {
              o->datidx.di_unknown = (BITCODE_RL)json_long (dat, tokens);
              LOG_TRACE ("datidx.di_unknown_1: " FORMAT_RL "\n", o->datidx.di_unknown);
            }
          else if (strEQc (key, "si_unknown_1"))
            {
              o->schidx.si_unknown_1 = (BITCODE_RL)json_long (dat, tokens);
              LOG_TRACE ("schidx.unknown_1: " FORMAT_RL "\n", o->schidx.si_unknown_1);
            }
          else if (strEQc (key, "si_unknown_2"))
            {
              o->schidx.si_unknown_2 = (BITCODE_RL)json_long (dat, tokens);
              LOG_TRACE ("schidx.unknown_2: " FORMAT_RL "\n", o->schidx.si_unknown_2);
            }
          else if (strEQc (key, "si_tag"))
            {
              o->schidx.si_tag = (BITCODE_RLL)json_long (dat, tokens);
              LOG_TRACE ("schidx.si_tag: " FORMAT_RLL "\n", o->schidx.si_tag);
            }
          else if (strEQc (key, "schidx.props"))
            {
              o->schidx.props = (Dwg_AcDs_SchemaIndex_Prop*)json_records (dat, tokens,
                                    o, "AcDs_SchemaIndex_Prop", &o->schidx.num_props);
            }
          else if (strEQc (key, "schidx.prop_entries"))
            {
              o->schidx.prop_entries = (Dwg_AcDs_SchemaIndex_Prop*)json_records (dat, tokens,
                                           o, "AcDs_SchemaIndex_Prop", &o->schidx.num_prop_entries);
            }
          else if (strEQc (key, "schdat.uprops"))
            {
              o->schdat.uprops = (Dwg_AcDs_SchemaData_UProp*)json_records (dat, tokens,
                                     o, "AcDs_SchemaData_UProp", &o->schdat.num_uprops);
            }
          else if (strEQc (key, "schdat.schemas"))
            {
              o->schdat.schemas = (Dwg_AcDs_Schema*)json_records (dat, tokens,
                                      o, "AcDs_Schema", &o->schdat.num_schemas);
            }
          else if (strEQc (key, "schdat.propnames"))
            {
              o->schdat.propnames = (BITCODE_TV*)json_vector (dat, tokens,
                                      key, "TV", &o->schdat.num_propnames);
            }
          else if (strEQc (key, "search.search"))
            {
              o->search.search = (Dwg_AcDs_Search_Data*)json_records (dat, tokens,
                                     o, "AcDs_Search_Data", &o->search.num_search);
            }
          /*
          else if (strEQc (key, "schema.index"))
            {
              o->schdat.schema.index = (BITCODE_RLL*)json_vector (dat, tokens,
                                    o, key, "RLL", &o->schdat.schemas[i].num_index);
            }
          else if (strEQc (key, "schema.props"))
            {
              o->schdat.schemas[i].props = (Dwg_AcDs_SchemaIndex_Prop*)json_records (dat, tokens,
                                      o, "AcDs_SchemaIndex_Prop", &o->schdat.schemas[i].num_props);
            }
          */
          // todo: support more types
          else
            {
              LOG_ERROR ("Unknown %s.%s ignored", section, key);
              json_advance_unknown (dat, tokens, t->type, 0);
            }
          // clang-format on
        }
      tokens->index--;
    }
  tokens->index++;
  return 0;
}

static int
json_AcDs (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                   jsmntokens_t *restrict tokens)
{
  const char *section = "AcDs";
  struct Dwg_AcDs *_obj = &dwg->acds;
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  int error = 0;
  int size;
  if (t->type != JSMN_OBJECT)
    {
      LOG_ERROR ("Unexpected %s at %u of %ld tokens, expected %s OBJECT",
                 t_typename[t->type], tokens->index, tokens->num_tokens,
                 section);
      json_advance_unknown (dat, tokens, t->type, 0);
      return DWG_ERR_INVALIDTYPE;
    }
  size = t->size;
  LOG_TRACE ("\n%s pos:%d [%d keys]\n--------------------\n", section,
             tokens->index, size);
  tokens->index++;
  for (int i = 0; i < size; i++)
    {
      char key[80];
      JSON_TOKENS_CHECK_OVERFLOW_ERR
      json_fixed_key (key, dat, tokens);
      t = &tokens->tokens[tokens->index];
      // clang-format off
      if (0) ;
      FIELD_RLx (file_signature, 0)
      FIELD_RLd (file_header_size, 0)
      FIELD_RLd (unknown_1, 0)
      FIELD_RLd (version, 0)
      FIELD_RLd (unknown_2, 0)
      FIELD_RLd (ds_version, 0)
      FIELD_RLd (segidx_offset, 0)
      FIELD_RLd (segidx_unknown, 0)
      FIELD_RLd (num_segidx, 0)
      FIELD_RLd (schidx_segidx, 0)
      FIELD_RLd (datidx_segidx, 0)
      FIELD_RLd (search_segidx, 0)
      FIELD_RLd (prvsav_segidx, 0)
      FIELD_RL (file_size, 0)
      // clang-format on
      else if (strEQc (key, "segidx"))
        {
          if (t->type != JSMN_ARRAY) // of OBJECTs
            json_advance_unknown (dat, tokens, t->type, 0);
          else if (t->size)
            error |= json_AcDs_SegmentIndex (dat, dwg, tokens, _obj, t->size);
          else
            tokens->index++; // empty array
          //if (error >= DWG_ERR_CRITICAL)
          //  return error;
        }
      else if (strEQc (key, "segments"))
        {
          if (t->type != JSMN_ARRAY) // of OBJECTs
            json_advance_unknown (dat, tokens, t->type, 0);
          else if (t->size)
            error |= json_AcDs_Segments (dat, dwg, tokens, _obj, t->size);
          else
            tokens->index++; // empty array
          //if (error >= DWG_ERR_CRITICAL)
          //  return error;
        }
      else
        {
          LOG_ERROR ("Unknown %s.%s ignored", section, key);
          json_advance_unknown (dat, tokens, t->type, 0);
        }
    }

  LOG_TRACE ("End of %s\n", section)
  tokens->index--;
  return 0;
}

static int
json_Template (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
               jsmntokens_t *restrict tokens)
{
  const char *section = "Template";
  struct Dwg_Template *_obj = &dwg->template;
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  int size;
  if (t->type != JSMN_OBJECT)
    {
      LOG_ERROR ("Unexpected %s OBJECT at %u of %ld tokens, got %s", section,
                 tokens->index, tokens->num_tokens, t_typename[t->type]);
      json_advance_unknown (dat, tokens, t->type, 0);
      return DWG_ERR_INVALIDTYPE;
    }
  size = t->size;
  LOG_TRACE ("\n%s pos:%d [%d keys]\n--------------------\n", section,
             tokens->index, size);
  tokens->index++;
  for (int i = 0; i < size; i++)
    {
      char key[80];
      JSON_TOKENS_CHECK_OVERFLOW_ERR
      json_fixed_key (key, dat, tokens);
      t = &tokens->tokens[tokens->index];
      if (strEQc (key, "MEASUREMENT"))
        {
          _obj->MEASUREMENT = (BITCODE_BS)json_long (dat, tokens);
          dwg->header_vars.MEASUREMENT = _obj->MEASUREMENT;
          LOG_TRACE ("%s: %d\n", key, (int)_obj->MEASUREMENT)
        }
      else if (strEQc (key, "description"))
        {
          LOG_TRACE ("%s: \"%.*s\" (ignored)\n", key, t->end - t->start, &dat->chain[t->start])
          tokens->index++;
        }
      else
        {
          LOG_TRACE ("%s\n", key);
          json_advance_unknown (dat, tokens, t->type, 0);
        }
    }

  LOG_TRACE ("End of %s\n", section)
  tokens->index--;
  return 0;
}

EXPORT int
dwg_read_json (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  struct Dwg_Header *obj = &dwg->header;
  jsmn_parser parser;
  jsmntokens_t tokens;
  unsigned int i;
  int error = -1;

  dwg->opts |= (loglevel | DWG_OPTS_INJSON);
  loglevel = dwg->opts & 0xf;
  if (!dat->chain && dat->fh)
    {
      error = dat_read_stream (dat, dat->fh);
      if (error >= DWG_ERR_CRITICAL)
        return error;
      LOG_TRACE ("  json file size: %lu\n", dat->size);
    }
  g_dat = dat;

  jsmn_init (&parser);
  // How big will it be? This is the max. memory variant.
  // we could also use less, see jsmn/examples/jsondump.c for small devices.
  tokens.num_tokens
      = jsmn_parse (&parser, (char *)dat->chain, dat->size, NULL, 0);
  if (tokens.num_tokens <= 0)
    {
      const int remaining = dat->size - parser.pos;
      if (parser.pos > 10 && remaining > 10)
        {
          char err[21];
          memcpy (&err, &dat->chain[parser.pos - 10], 20);
          err[20] = 0;
          LOG_ERROR ("Invalid json. jsmn error at pos: %u (... %s ...)",
                     parser.pos, err);
        }
      else
        {
          LOG_ERROR ("Invalid json. jsmn error at pos: %u (%.*s ...)",
                     parser.pos, remaining, &dat->chain[parser.pos]);
        }
      return DWG_ERR_INVALIDDWG;
    }
  LOG_TRACE ("  num_tokens: %ld\n", tokens.num_tokens);
  tokens.tokens = calloc (tokens.num_tokens + 1024, sizeof (jsmntok_t));
  if (!tokens.tokens)
    return DWG_ERR_OUTOFMEM;

  dwg->object_map = hash_new (tokens.num_tokens / 50);
  if (!dwg->object_map) // we are obviously on a tiny system
    {
      dwg->object_map = hash_new (1024);
      if (!dwg->object_map)
        {
          LOG_ERROR ("Out of memory");
          return DWG_ERR_OUTOFMEM;
        }
    }
  dwg->dirty_refs = 1;
  // set the target version earlier (than e.g. in DXF)
  // we cannot write DWG >= R_2004 yet. avoid widestrings for now
  /* if (!dwg->header.version)
    dwg->header.version = dat->version = R_2000;
  */

  jsmn_init (&parser); // reset pos to 0
  error = jsmn_parse (&parser, (char *)dat->chain, dat->size, tokens.tokens,
                      (unsigned int)tokens.num_tokens);
  if (error < 0)
    {
      const int remaining = dat->size - parser.pos;
      if (parser.pos > 10 && remaining > 10)
        {
          char err[21];
          memcpy (&err, &dat->chain[parser.pos - 10], 20);
          err[20] = 0;
          LOG_ERROR ("Invalid json. jsmn error %d at the %u-th token, pos: %u "
                     "(... %s ...)",
                     error, parser.toknext, parser.pos, err);
        }
      else
        {
          LOG_ERROR ("Invalid json. jsmn error %d at the %u-th token, pos: %u "
                     "(%.*s ...)",
                     error, parser.toknext, parser.pos, remaining,
                     &dat->chain[parser.pos]);
        }
      free (tokens.tokens);
      return DWG_ERR_INVALIDDWG;
    }

  if (tokens.tokens[0].type != JSMN_OBJECT)
    {
      fprintf (stderr, "First JSON element is not an object/hash\n");
      free (tokens.tokens);
      exit (1);
    }

  // valid first level tokens:
  // created_by: string
  // section objects: FILEHEADER, HEADER, THUMBNAILIMAGE, R2004_Header,
  //                  SummaryInfo, AppInfo,
  //                  AppInfoHistory, FileDepList, Security, RevHistory,
  //                  ObjFreeSpace, Template
  // section arrays: CLASSES, OBJECTS, HANDLES
  error = 0;
  for (tokens.index = 1; tokens.index < (unsigned int)tokens.num_tokens; tokens.index++)
    {
      char key[80];
      const jsmntok_t *t = &tokens.tokens[tokens.index];
      const int len = t->end - t->start;

      if (t->type == JSMN_UNDEFINED)
        break;
      if (t->type != JSMN_STRING)
        {
          LOG_ERROR ("Unexpected JSON key at %u of %ld tokens, got %s",
                     tokens.index, tokens.num_tokens, t_typename[t->type]);
          free (tokens.tokens);
          return DWG_ERR_INVALIDDWG;
        }
      if (len >= 80)
        {
          LOG_ERROR ("Unknown JSON key at %u of %ld tokens, len %d > 80",
                     tokens.index, tokens.num_tokens, len);
          free (tokens.tokens);
          return DWG_ERR_INVALIDDWG;
        }
      memcpy (key, &dat->chain[t->start], len);
      key[len] = '\0';
      tokens.index++;
      if (tokens.index >= (unsigned int)tokens.num_tokens)
        {
          LOG_ERROR ("Unexpected end of JSON at %u of %ld tokens %s:%d",
                     tokens.index, tokens.num_tokens, __FILE__, __LINE__);
          free (tokens.tokens);
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
      else if (strEQc (key, "AuxHeader"))
        error |= json_AuxHeader (dat, dwg, &tokens);
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
      else if (strEQc (key, "AcDs"))
        error |= json_AcDs (dat, dwg, &tokens);
      else if (strEQc (key, "Template"))
        error |= json_Template (dat, dwg, &tokens);
      /* Only in json early versions <0.11 */
      else if (strEQc (key, "HANDLES"))
        error |= json_HANDLES (dat, dwg, &tokens);
      else
        {
          LOG_ERROR ("Unexpected JSON key %s at %u of %ld tokens. %s:%d", key,
                     tokens.index, tokens.num_tokens, __FUNCTION__, __LINE__);
          LOG_TRACE ("\n")
          free (tokens.tokens);
          return error | DWG_ERR_INVALIDTYPE;
        }
      if (error >= DWG_ERR_CRITICAL)
        {
          LOG_TRACE ("\n")
          free (tokens.tokens);
          return error;
        }
    }

  LOG_TRACE ("\n")
  if (dwg->header.version <= R_2000 && dwg->header.from_version > R_2000)
    dwg_fixup_BLOCKS_entities (dwg);

  free (tokens.tokens);
  return error;
}

#undef IS_ENCODER
#undef IS_JSON
