/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2020-2025 Free Software Foundation, Inc.                   */
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
#include <errno.h>
#include <limits.h>

#define IS_ENCODER
#define IS_JSON
#include "common.h"
#include "importer.h"
#include "bits.h"
#include "dwg.h"
#include "hash.h"
#include "decode.h"
#include "dynapi.h"
#include "classes.h"
#include "in_json.h"

static unsigned int loglevel;
#define DWG_LOGLEVEL loglevel
#include "logging.h"

#include "in_dxf.h"
#include "free.h"

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
#define SUB_FIELD_LONG(o, nam, type)                                          \
  else if (strEQc (key, #nam))                                                \
  {                                                                           \
    _obj->o.nam = (BITCODE_##type)json_long (dat, tokens);                    \
    LOG_TRACE (#nam ": " FORMAT_##type "\n", _obj->o.nam)                     \
  }
#define _FIELD_LONGLONG(nam, type)                                            \
  else if (strEQc (key, #nam))                                                \
  {                                                                           \
    _obj->nam = (BITCODE_##type)json_longlong (dat, tokens);                  \
    LOG_TRACE (#nam ": " FORMAT_##type "\n", _obj->nam)                       \
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
    if (s)                                                                    \
      {                                                                       \
        size_t slen = strlen (s);                                             \
        memcpy (&_obj->nam, s, MIN (len, slen));                              \
        LOG_TRACE (#nam ": \"%.*s\"\n", len, _obj->nam);                      \
        FREE (s);                                                             \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        LOG_TRACE (#nam ": NULL\n");                                          \
      }                                                                       \
  }
#define FIELD_TFFx(nam, len, dxf)                                             \
  else if (strEQc (key, #nam))                                                \
  {                                                                           \
    size_t slen;                                                              \
    unsigned char *s = json_binary (dat, tokens, #nam, &slen);                \
    if (s)                                                                    \
      {                                                                       \
        slen = MIN (len, slen);                                               \
        memcpy (&_obj->nam, s, slen);                                         \
        LOG_TRACE (#nam ": \"%.*s\"\n", (int)slen, _obj->nam);                \
        FREE (s);                                                             \
      }                                                                       \
  }
#define FIELD_BINARY(nam, lenf, dxf)                                          \
  else if (strEQc (key, #nam))                                                \
  {                                                                           \
    size_t slen;                                                              \
    _obj->nam = json_binary (dat, tokens, #nam, &slen);                       \
    _obj->lenf = slen & 0xFFFFFFFF;                                           \
    JSON_TOKENS_CHECK_OVERFLOW_ERR                                            \
  }
#define FIELD_T(nam, dxf)                                                     \
  else if (strEQc (key, #nam))                                                \
  {                                                                           \
    LOG_TRACE (#nam ": \"%.*s\"\n", t->end - t->start,                        \
               &dat->chain[t->start]);                                        \
    if (t->type == JSMN_STRING)                                               \
      {                                                                       \
        /*if (dwg->header.version >= R_2007)                                  \
          _obj->nam = (BITCODE_T)json_wstring (dat, tokens);                  \
        else*/                                                                \
        _obj->nam = json_string (dat, tokens);                                \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        _obj->nam = NULL;                                                     \
        json_advance_unknown (dat, tokens, t->type, 0);                       \
      }                                                                       \
    JSON_TOKENS_CHECK_OVERFLOW_ERR                                            \
  }
#define FIELD_T32(nam, dxf)                                                   \
  else if (strEQc (key, #nam))                                                \
  {                                                                           \
    LOG_TRACE (#nam ": \"%.*s\"\n", t->end - t->start,                        \
               &dat->chain[t->start]);                                        \
    if (t->type == JSMN_STRING)                                               \
      _obj->nam = (BITCODE_T32)json_string (dat, tokens);                     \
    else                                                                      \
      {                                                                       \
        _obj->nam = NULL;                                                     \
        json_advance_unknown (dat, tokens, t->type, 0);                       \
      }                                                                       \
    JSON_TOKENS_CHECK_OVERFLOW_ERR                                            \
  }
#define FIELD_T16(nam, dxf)                                                   \
  else if (strEQc (key, #nam))                                                \
  {                                                                           \
    LOG_TRACE (#nam ": \"%.*s\"\n", t->end - t->start,                        \
               &dat->chain[t->start]);                                        \
    if (t->type == JSMN_STRING)                                               \
      {                                                                       \
        _obj->nam = json_string (dat, tokens);                                \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        _obj->nam = NULL;                                                     \
        json_advance_unknown (dat, tokens, t->type, 0);                       \
      }                                                                       \
    JSON_TOKENS_CHECK_OVERFLOW_ERR                                            \
  }
#define FIELD_TU16(nam, dxf)                                                  \
  else if (strEQc (key, #nam))                                                \
  {                                                                           \
    LOG_TRACE (#nam ": \"%.*s\"\n", t->end - t->start,                        \
               &dat->chain[t->start]);                                        \
    if (t->type == JSMN_STRING)                                               \
      _obj->nam = json_wstring (dat, tokens);                                 \
    else                                                                      \
      {                                                                       \
        _obj->nam = NULL;                                                     \
        json_advance_unknown (dat, tokens, t->type, 0);                       \
      }                                                                       \
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
#define FIELD_RCd(nam, dxf) _FIELD_LONG (nam, RCd)
#define FIELD_RCx(nam, dxf) _FIELD_LONGT (nam, RC, RCx)
#define FIELD_RS(nam, dxf) _FIELD_LONG (nam, RS)
#define FIELD_RSx(nam, dxf) _FIELD_LONGT (nam, RS, RSx)
#define FIELD_RL(nam, dxf) _FIELD_LONG (nam, RL)
#define FIELD_RLx(nam, dxf) _FIELD_LONGT (nam, RL, RLx)
#define FIELD_RLd(nam, dxf) _FIELD_LONGT (nam, RL, RLd)
#define FIELD_RLL(nam, dxf) _FIELD_LONGLONG (nam, RLL)
#define FIELD_HV(nam, dxf) _FIELD_LONGLONG (nam, HV)
#define FIELD_MC(nam, dxf) _FIELD_LONG (nam, MC)
#define FIELD_MS(nam, dxf) _FIELD_LONG (nam, MS)

#define FIELD_BD(nam, dxf) _FIELD_FLOAT (nam, BD)
#define FIELD_RD(nam, dxf) _FIELD_FLOAT (nam, RD)

// NOTE: Only for int types for now, no float. There don't exist any inlined
// float vectors
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
    _obj->nam = (BITCODE_##typ *)CALLOC (t->size, sizeof (BITCODE_##typ));    \
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

#define JSON_TOKENS_CHECK_OVERFLOW(ret)                                       \
  if (tokens->index > (unsigned int)tokens->num_tokens)                       \
    {                                                                         \
      LOG_ERROR ("Unexpected end of JSON at %u of %ld tokens", tokens->index, \
                 tokens->num_tokens);                                         \
      ret;                                                                    \
    }
#define JSON_TOKENS_CHECK_OVERFLOW_LABEL(label)                               \
  JSON_TOKENS_CHECK_OVERFLOW (goto label)
#define JSON_TOKENS_CHECK_OVERFLOW_ERR                                        \
  JSON_TOKENS_CHECK_OVERFLOW (return DWG_ERR_INVALIDDWG)
#define JSON_TOKENS_CHECK_OVERFLOW_NULL                                       \
  JSON_TOKENS_CHECK_OVERFLOW (return NULL)
#define JSON_TOKENS_CHECK_OVERFLOW_VOID JSON_TOKENS_CHECK_OVERFLOW (return)

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
          if (t->type == JSMN_OBJECT)
            tokens->index++; // skip the key also
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
      LOG_ERROR ("Overlong JSON STRING len=%d >= 80", len);
      tokens->index++;
      JSON_TOKENS_CHECK_OVERFLOW_VOID
      return;
    }
  if (len > 0)
    {
      memcpy (key, &dat->chain[t->start], len);
      key[len] = '\0';
      tokens->index++;
      JSON_TOKENS_CHECK_OVERFLOW_VOID
    }
  else
    LOG_ERROR ("Empty JSON key");
  return;
}

ATTRIBUTE_MALLOC
static char *
json_string (Bit_Chain *restrict dat, jsmntokens_t *restrict tokens)
{
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  char *key = NULL;
  int len;
  JSON_TOKENS_CHECK_OVERFLOW_NULL;
  len = t->end - t->start;
  if (t->type != JSMN_STRING)
    {
      LOG_ERROR ("Expected JSON STRING");
      json_advance_unknown (dat, tokens, t->type, 0);
      JSON_TOKENS_CHECK_OVERFLOW_NULL
      return NULL;
    }
  // Unquote \", convert Unicode to \\U+xxxx as in bit_embed_TU
  // unquote \\ to \.
  if (memchr (&dat->chain[t->start], '\\', len))
    {
      len += 8;
      key = (char *)CALLOC (1, len);
      if (!key)
        goto outofmemory;
      dat->chain[t->end] = '\0';
      while (!bit_utf8_to_TV (key, &dat->chain[t->start], len,
                              t->end - t->start, 1, dat->codepage))
        {
          LOG_INSANE ("Not enough room in quoted string len=%d\n", len - 8)
          len += 8;
          if (len > 6 * (t->end - t->start))
            {
              LOG_ERROR ("bit_utf8_to_TV loop len=%d vs %d \"%.*s\"", len,
                         t->end - t->start, t->end - t->start,
                         &dat->chain[t->start]);
              len = t->end - t->start;
              FREE (key);
              goto normal;
            }
          key = (char *)REALLOC (key, len);
          if (!key)
            goto outofmemory;
        }
    }
  else
    {
    normal:
      key = (char *)MALLOC (len + 1);
      if (!key)
        goto outofmemory;
      memcpy (key, &dat->chain[t->start], len);
      key[len] = '\0';
    }
  tokens->index++;
  return key;
outofmemory:
  LOG_ERROR ("Out of memory");
  return NULL;
}

ATTRIBUTE_MALLOC
static char *
json_fixed_string (Bit_Chain *restrict dat, const int len,
                   jsmntokens_t *restrict tokens)
{
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  char *str = (char *)CALLOC (len + 1, 1);
  int l;
  JSON_TOKENS_CHECK_OVERFLOW_NULL;
  l = t->end - t->start;
  if (!str)
    goto outofmemory;
  if (t->type != JSMN_STRING)
    {
      LOG_ERROR ("Expected JSON STRING");
      json_advance_unknown (dat, tokens, t->type, 0);
      JSON_TOKENS_CHECK_OVERFLOW_NULL
      return NULL;
    }
  // Unquote \", convert Unicode to \\U+xxxx as in bit_embed_TU
  // unquote \\ to \.
  if (memchr (&dat->chain[t->start], '\\', l))
    {
      int dlen = len;
      dat->chain[t->end] = '\0';
      while (!bit_utf8_to_TV (str, &dat->chain[t->start], dlen, l, 1,
                              dat->codepage))
        {
          LOG_INSANE ("Not enough room in quoted string len=%d\n", len)
          dlen += 8;
          if (dlen > 6 * l)
            {
              LOG_ERROR ("bit_utf8_to_TV loop len=%d vs %d \"%.*s\"", len, l,
                         l, &dat->chain[t->start]);
              // len = t->end - t->start;
              FREE (str);
              str = NULL;
              goto normal;
            }
          str = (char *)REALLOC (str, dlen);
          if (!str)
            goto outofmemory;
        }
      str[len] = '\0';
    }
  else
    {
      char *p;
    normal:
      if (!str)
        return NULL;
      if (l > len)
        {
          memcpy (str, &dat->chain[t->start], len);
          LOG_WARN ("Overlarge JSON TF value \"%.*s\" stripped to \"%s\"", l,
                    &dat->chain[t->start], str);
          str[len] = '\0';
        }
      else
        {
          memcpy (str, &dat->chain[t->start], l);
          str[l] = '\0';
        }
    }
  tokens->index++;
  return str;
outofmemory:
  LOG_ERROR ("Out of memory");
  return NULL;
}

ATTRIBUTE_MALLOC
static unsigned char *
json_binary (Bit_Chain *restrict dat, jsmntokens_t *restrict tokens,
             const char *restrict key, size_t *lenp)
{
  // convert from hex
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  const size_t len = t->end - t->start;
  const char *str = (char *)&dat->chain[t->start];
  const size_t blen = len / 2;
  size_t read;
  unsigned char *buf = len ? (unsigned char *)MALLOC (blen + 1) : NULL;
  char *pos = (char *)str;
  char *old;

  *lenp = 0;
  if (t->type != JSMN_STRING || !pos)
    {
      LOG_ERROR ("Expected JSON STRING");
      json_advance_unknown (dat, tokens, t->type, 0);
      FREE (buf);
      JSON_TOKENS_CHECK_OVERFLOW_NULL
      return NULL;
    }
  if (!buf)
    {
      if (len)
        LOG_ERROR ("Out of memory");
      tokens->index++;
      return NULL;
    }
  if ((read = in_hex2bin (buf, pos, blen) != blen))
    LOG_ERROR ("json_binary in_hex2bin with key %s at pos %" PRIuSIZE
               " of %" PRIuSIZE,
               key, read, blen);
  if (buf)
    {
      buf[blen] = '\0';
      LOG_TRACE ("%s: '%.*s'... [BINARY %" PRIuSIZE "]\n", key,
                 MIN ((int)len, 60), str, len);
      *lenp = blen;
    }
  tokens->index++;
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
      JSON_TOKENS_CHECK_OVERFLOW (return (double)NAN)
      return (double)NAN;
    }
  JSON_TOKENS_CHECK_OVERFLOW (return (double)NAN)
  tokens->index++;
  return strtod ((char *)&dat->chain[t->start], NULL);
}

static uint32_t
json_long (Bit_Chain *restrict dat, jsmntokens_t *restrict tokens)
{
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  // int len = t->end - t->start;
  if (t->type != JSMN_PRIMITIVE)
    {
      LOG_ERROR ("Expected JSON PRIMITIVE");
      json_advance_unknown (dat, tokens, t->type, 0);
      JSON_TOKENS_CHECK_OVERFLOW (return 0)
      return 0;
    }
  JSON_TOKENS_CHECK_OVERFLOW (return 0)
  tokens->index++;
  return (uint32_t)strtol ((char *)&dat->chain[t->start], NULL, 10);
}

static uint64_t
json_longlong (Bit_Chain *restrict dat, jsmntokens_t *restrict tokens)
{
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  // int len = t->end - t->start;
  if (t->type != JSMN_PRIMITIVE)
    {
      LOG_ERROR ("Expected JSON PRIMITIVE");
      json_advance_unknown (dat, tokens, t->type, 0);
      JSON_TOKENS_CHECK_OVERFLOW (return 0)
      return 0;
    }
  JSON_TOKENS_CHECK_OVERFLOW (return 0)
  tokens->index++;
#ifdef HAVE_STRTOULL
  return strtoull ((char *)&dat->chain[t->start], NULL, 10);
#else
  return strtoll ((char *)&dat->chain[t->start], NULL, 10);
#endif
}

static void
json_3DPOINT (Bit_Chain *restrict dat, jsmntokens_t *restrict tokens,
              const char *restrict name, const char *restrict key,
              const char *restrict type, BITCODE_3DPOINT *restrict pt)
{
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  if (t->type != JSMN_ARRAY || t->size != 3)
    {
      // older DWG's often are only 2D
      if (t->type != JSMN_ARRAY || dat->from_version >= R_13b1)
        {
          LOG_ERROR ("JSON 3DPOINT must be ARRAY of size 3")
          return;
        }
    }
  tokens->index++;
  JSON_TOKENS_CHECK_OVERFLOW_VOID
  pt->x = json_float (dat, tokens);
  pt->y = json_float (dat, tokens);
  if (t->size == 3)
    {
      pt->z = json_float (dat, tokens);
      LOG_TRACE ("%s.%s: (%f, %f, %f) [%s]\n", name, key, pt->x, pt->y, pt->z,
                 type);
    }
  else
    {
      pt->z = 0.0;
      LOG_TRACE ("%s.%s: (%f, %f) [%s]\n", name, key, pt->x, pt->y, type);
    }
}

static void
json_2DPOINT (Bit_Chain *restrict dat, jsmntokens_t *restrict tokens,
              const char *restrict name, const char *restrict key,
              const char *restrict type, BITCODE_2DPOINT *restrict pt)
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
  LOG_TRACE ("%s.%s: (%f, %f) [%s]\n", name, key, pt->x, pt->y, type);
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
             jsmntokens_t *restrict tokens, const char *name, const char *key,
             const Dwg_Object *restrict obj, const int i)
{
  uint32_t code, size, r11_idx;
  BITCODE_RLL value, absref;
  BITCODE_H ref;
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  if (t->type != JSMN_ARRAY || t->size < 2 || t->size > 5)
    {
      LOG_ERROR ("JSON HANDLE must be ARRAY of [ code, value ] or "
                 "[ code, size, value, absref ] or [ size, r11_idx, absref ]")
      return NULL;
    }
  JSON_TOKENS_CHECK_OVERFLOW_NULL
  tokens->index++;
  code = json_long (dat, tokens);
  if (i < 0)
    {
      LOG_TRACE ("%s.%s: ", name, key);
    }
  else // H*
    {
      LOG_TRACE ("%s.%s[%d]: ", name, key, i);
    }
  if (t->size >= 4)
    {
      size = json_long (dat, tokens);
      value = json_longlong (dat, tokens);
      absref = json_longlong (dat, tokens);
      if (!code && strEQc (key, "sort_ents"))
        ref = dwg_add_handleref (dwg, code, absref, NULL);
      else
        ref = dwg_add_handleref (dwg, code, absref,
                                 (!code || code >= 6) ? obj : NULL);
      if (t->size > 4)
        ref->r11_idx = json_long (dat, tokens); // unused
      if ((BITCODE_RC)size != ref->handleref.size
          || value != ref->handleref.value)
        {
          // Wrong user-input: GH #346 mingw64, oss-fuzz #39755
          // but valid for !HANDLING preR13
          if (strEQ (name, "HEADER") && !dwg->header_vars.HANDLING
              && dat->from_version <= R_11
              && (strEQc (key, "HANDSEED") || strEQc (key, "UCSNAME")))
            ;
          else
            LOG_ERROR ("Invalid handle %.*s => " FORMAT_REF " code=" FORMAT_BL
                       " size=" FORMAT_BL " value=" FORMAT_RLL
                       " absref=" FORMAT_RLL,
                       t->end - t->start, &dat->chain[t->start],
                       ARGS_REF (ref), code, size, value, absref);
          ref->handleref.size = (BITCODE_RC)size;
          ref->handleref.value = value;
          ref->absolute_ref = absref;
        }
      if (t->size > 4)
        LOG_TRACE (FORMAT_REF11 " [H]\n", ARGS_REF11 (ref))
      else
        LOG_TRACE (FORMAT_REF " [H]\n", ARGS_REF (ref))
    }
  else if (dat->from_version >= R_13b1)
    {
      // code as 1st
      if (t->size == 3) // since 0.12.5.5543 [code size value]
        size = json_long (dat, tokens);
      absref = json_longlong (dat, tokens);
      ref = dwg_add_handleref (dwg, code, absref,
                               (!code || code >= 6) ? obj : NULL);
      LOG_TRACE (FORMAT_REF " [H]\n", ARGS_REF (ref))
    }
  else // r11 for HANDLING=1 and/or _CONTROL entries
    {
      // [size idx absref] or [0 size absref]
      size = code;
      code = 0;
      r11_idx = json_long (dat, tokens);
      if (t->size == 3)
        {
          if (size == 0) // since 0.12.5.5543 [0 size value]
            {
              size = r11_idx;
              r11_idx = 0;
            }
          else
            code = 3; // r11 control entries default to 3
          absref = json_longlong (dat, tokens);
        }
      else
        absref = r11_idx; // 2nd item, the value
      ref = dwg_add_handleref (dwg, code, absref, obj);
      if (t->size == 2) // [size absref]
        ref->handleref.value = absref;
      else
        ref->r11_idx = r11_idx;
      ref->handleref.size = size;
      LOG_TRACE (FORMAT_REF11 " [H]\n", ARGS_REF11 (ref))
    }
  return ref;
}

static void
json_CMC (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
          jsmntokens_t *restrict tokens, const char *name, const char *fname,
          Dwg_Color *restrict color)
{
  char key[80];
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  memset (color, 0, sizeof (Dwg_Color));
  if (t->type == JSMN_OBJECT)
    {                  // 2004+
      tokens->index++; // hash of index, rgb...
      for (int j = 0; j < t->size; j++)
        {
          JSON_TOKENS_CHECK_OVERFLOW_VOID
          json_fixed_key (key, dat, tokens);
          if (strEQc (key, "index"))
            {
              uint32_t num = json_long (dat, tokens);
              JSON_TOKENS_CHECK_OVERFLOW_VOID
              color->index = (BITCODE_BSd)num;
              LOG_TRACE ("%s.%s.index " FORMAT_BSd " [CMC]\n", name, fname,
                         color->index);
            }
          else if (strEQc (key, "rgb"))
            {
              char hex[80];
              json_fixed_key (hex, dat, tokens);
              JSON_TOKENS_CHECK_OVERFLOW_VOID
              sscanf (hex, "%x", &color->rgb);
              color->method = color->rgb >> 0x18;
              LOG_TRACE ("%s.%s.rgb %x (method %x) [CMC]\n", name, fname,
                         color->rgb, color->method);
            }
          else if (strEQc (key, "flag"))
            {
              uint32_t num = json_long (dat, tokens);
              JSON_TOKENS_CHECK_OVERFLOW_VOID
              LOG_TRACE ("%s.%s.flag %lu [CMC]\n", name, fname,
                         (unsigned long)num);
              color->flag = (BITCODE_BS)num;
            }
          else if (strEQc (key, "alpha"))
            {
              uint32_t num = json_long (dat, tokens);
              JSON_TOKENS_CHECK_OVERFLOW_VOID
              LOG_TRACE ("%s.%s.alpha %u [CMC]\n", name, fname, (unsigned)num);
              color->alpha = (BITCODE_RC)num;
              color->alpha_type = 3;
            }
          else if (strEQc (key, "handle")) // [4, value] ARRAY
            {
              color->handle
                  = json_HANDLE (dat, dwg, tokens, name, fname, NULL, -1);
              JSON_TOKENS_CHECK_OVERFLOW_VOID
            }
          else if (strEQc (key, "name"))
            {
              char *str = json_string (dat, tokens);
              JSON_TOKENS_CHECK_OVERFLOW_VOID
              LOG_TRACE ("%s.%s.name \"%s\" [CMC]\n", name, fname, str);
              color->name = str;
              color->flag |= 1;
            }
          else if (strEQc (key, "book_name"))
            {
              char *str = json_string (dat, tokens);
              JSON_TOKENS_CHECK_OVERFLOW_VOID
              LOG_TRACE ("%s.%s.book_name \"%s\" [CMC]\n", name, fname, str);
              color->book_name = str;
              color->flag |= 2;
            }
          else
            {
              LOG_WARN ("Unknown color key %s.%s.%s", name, fname, key);
              tokens->index++;
              JSON_TOKENS_CHECK_OVERFLOW_VOID
            }
        }
    }
  else if (t->type == JSMN_PRIMITIVE)
    { // pre 2004
      uint32_t num = json_long (dat, tokens);
      color->index = (BITCODE_BSd)num;
      LOG_TRACE ("%s.%s.index " FORMAT_BSd " [CMC]\n", name, fname,
                 color->index);
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
      date->value
          = date->days
            + (86400.0 * date->ms); // just for display, not calculations
    }
  else
    {
      double num;
      num = json_float (dat, tokens);
      JSON_TOKENS_CHECK_OVERFLOW_VOID
      date->value = num;
      date->days = (BITCODE_BL)trunc (num);
      date->ms = (BITCODE_BL)(86400.0 * (date->value - date->days));
    }
  LOG_TRACE ("%s: %.08f (%u, %u) [TIMEBLL]\n", name, date->value, date->days,
             date->ms);
}

// array of subclass objects
static void *
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

// of primitives only. may return NULL
static void *
json_vector (Bit_Chain *restrict dat, jsmntokens_t *restrict tokens,
             const char *key, const char *type, BITCODE_BL *nump)
{
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  BITCODE_BL *a_bl = NULL;
  BITCODE_RLL *a_rll = NULL;
  BITCODE_BD *a_bd = NULL;
  BITCODE_TV *a_tv = NULL;
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
      a_bl = len ? (BITCODE_BL *)CALLOC (len, size) : NULL;
      arr = a_bl;
    }
  else if (strEQc (type, "RLL"))
    {
      size = sizeof (BITCODE_RLL);
      a_rll = len ? (BITCODE_RLL *)CALLOC (len, size) : NULL;
      arr = a_rll;
    }
  else if (strEQc (type, "BD"))
    {
      is_float = 1;
      size = sizeof (BITCODE_BD);
      a_bd = len ? (BITCODE_BD *)CALLOC (len, size) : NULL;
      arr = a_bd;
    }
  else if (strEQc (type, "TV"))
    {
      is_str = 1;
      size = sizeof (BITCODE_TV);
      a_tv = len ? (BITCODE_TV *)CALLOC (len, size) : NULL;
      arr = a_tv;
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
      if (is_str && a_tv)
        {
          a_tv[k] = json_string (dat, tokens);
          JSON_TOKENS_CHECK_OVERFLOW_NULL;
          LOG_TRACE ("%s[%d]: %s [%s]\n", key, k, a_tv[k], type);
        }
      else if (is_float && a_bd)
        {
          a_bd[k] = json_float (dat, tokens);
          LOG_TRACE ("%s[%d]: %f [%s]\n", key, k, a_bd[k], type);
        }
      else if (strEQc (type, "RLL") && a_rll)
        {
          a_rll[k] = json_longlong (dat, tokens);
          LOG_TRACE ("%s[%d]: " FORMAT_RLL " [%s]\n", key, k, a_rll[k], type);
        }
      else if (strEQc (type, "BL") && a_bl)
        {
          a_bl[k] = json_long (dat, tokens);
          LOG_TRACE ("%s[%d]: " FORMAT_BL " [%s]\n", key, k, a_bl[k], type);
        }
    }
  // dwg_dynapi_field_set_value (dwg, _obj, f, &nums, 1);
  return (void *)arr;
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
  Dwg_Header *_obj = &dwg->header;
  Dwg_Object *obj = NULL;
  char version[80] = {0};
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
          version[0] = '\0';
          json_fixed_key (version, dat, tokens);
          dat->from_version = dwg->header.from_version
              = dwg_version_hdr_type (version);
          // is_tu = dat->from_version >= R_2007;
          LOG_TRACE ("FILEHEADER.from_version = %s (%s)\n", version,
                     dwg_version_type (dat->from_version));
          if (dat->from_version == R_INVALID)
            {
              LOG_ERROR ("Invalid FILEHEADER.version %s", version);
              return DWG_ERR_INVALIDDWG;
            }
        }
      // clang-format off
      FIELD_RC (is_maint, 0)
      FIELD_RC (zero_one_or_three, 0)
      // preR13 only
      FIELD_RS (numentity_sections, 0)
      FIELD_RL (sections, 0)
      FIELD_RS (numheader_vars, 0)
      FIELD_RLx (entities_start, 0)
      FIELD_RLx (entities_end, 0)
      FIELD_RLx (blocks_start, 0)
      FIELD_RLx (blocks_size, 0)
      FIELD_RLx (extras_start, 0)
      FIELD_RLx (extras_size, 0)

      FIELD_RL (thumbnail_address, 0) //@0x0d
      FIELD_RC (dwg_version, 0)
      FIELD_RC (maint_version, 0)
      FIELD_RS (codepage, 0) //@0x13: 29/30 for ANSI_1252, since r2007 UTF-16
      FIELD_RL (sections, 0) // until r2000
      // SINCE (R_2004a)
      FIELD_RC (unknown_0, 0)
      FIELD_RC (app_dwg_version, 0)
      FIELD_RC (app_maint_version, 0)
      FIELD_RL (security_type, 0)
      FIELD_RL (rl_1c_address, 0) /* mostly 0 */
      FIELD_RL (summaryinfo_address, 0)
      FIELD_RL (vbaproj_address, 0)
      FIELD_RL (r2004_header_address, 0) /* mostly 128/0x80 */

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
  // revised beta version
  {
    Dwg_Version_Type v = *version ?
      dwg_version_hdr_type2 (version, _obj->dwg_version) : R_INVALID;
    if (v != R_INVALID && v != dwg->header.from_version)
      {
        dat->from_version = dwg->header.from_version = v;
        LOG_TRACE ("FILEHEADER.from_version = %s (%s) via dwg_version\n",
                   version, dwg_version_type (v));
      }
  }
  tokens->index--;
  return 0;
}

static int
json_HEADER (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
             jsmntokens_t *restrict tokens)
{
  const char *section = "HEADER";
  const char *name = section;
  jsmntok_t *t = &tokens->tokens[tokens->index];
  // Dwg_Header_Variables *_obj = &dwg->header_vars;
  Dwg_Object *obj = NULL;
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
          if (t->type == JSMN_ARRAY && strEQc (key, "layer_colors"))
            {
              tokens->index++;
              for (int index = 0; index < MAX (t->size, 128); index++)
                {
                  dwg->header_vars.layer_colors[index]
                      = (BITCODE_RS)json_long (dat, tokens);
                  LOG_TRACE ("%s: " FORMAT_RS " [RS]\n", key,
                             dwg->header_vars.layer_colors[index]);
                }
              JSON_TOKENS_CHECK_OVERFLOW_ERR;
            }
          else
            {
              LOG_WARN ("Unknown key HEADER.%s", key)
              json_advance_unknown (dat, tokens, t->type, 0);
              continue;
            }
        }
      else if (t->type == JSMN_PRIMITIVE
               && (strEQc (f->type, "BD") || strEQc (f->type, "RD")))
        {
          double num = json_float (dat, tokens);
          LOG_TRACE ("%s: " FORMAT_RD " [%s]\n", key, num, f->type)
          dwg_dynapi_header_set_value (dwg, key, &num, 0);
        }
      else if (t->type == JSMN_PRIMITIVE && f->size <= 4 // not a RS[]
               && (strEQc (f->type, "RC") || strEQc (f->type, "B")
                   || strEQc (f->type, "BB") || strEQc (f->type, "RS")
                   || strEQc (f->type, "BS") || strEQc (f->type, "RL")
                   || strEQc (f->type, "BL") || strEQc (f->type, "RSd")
                   || strEQc (f->type, "BLd") || strEQc (f->type, "BSd")))
        {
          uint32_t num = json_long (dat, tokens);
          LOG_TRACE ("%s: " FORMAT_BL " [%s]\n", key, num, f->type)
          dwg_dynapi_header_set_value (dwg, key, &num, 0);
        }
      else if (t->type == JSMN_PRIMITIVE && f->size == 8 // not a RLL[]
               && (strEQc (f->type, "RLL") || strEQc (f->type, "BLL")))
        {
          uint64_t num = json_longlong (dat, tokens);
          LOG_TRACE ("%s: " FORMAT_RLL " [%s]\n", key, num, f->type)
          dwg_dynapi_header_set_value (dwg, key, &num, 0);
        }
      else if (t->type == JSMN_STRING
               && (strEQc (f->type, "TV") || strEQc (f->type, "T")))
        {
          size_t len;
          char *str = json_string (dat, tokens);
          LOG_TRACE ("%s: \"%s\" [%s]\n", key, str, f->type);
          if (dwg->header.version < R_13b1 && strEQc (key, "MENU")
              && (len = strlen (str) > 15))
            { // split into MENU + MENUEXT
              strncpy ((char *)dwg->header_vars.MENUEXT, &str[15], 45);
              str[15] = '\0';
              dwg->header_vars.MENU = STRDUP (str);
              dwg->header_vars.MENUEXT[45] = '\0';
            }
          else
            dwg_dynapi_header_set_value (dwg, key, &str, 1);
          FREE (str);
        }
      else if (t->type == JSMN_ARRAY
               && (strEQc (f->type, "3BD") || strEQc (f->type, "3RD")
                   || strEQc (f->type, "3DPOINT") || strEQc (f->type, "BE")
                   || strEQc (f->type, "3BD_1")))
        {
          BITCODE_3DPOINT pt;
          json_3DPOINT (dat, tokens, name, key, f->type, &pt);
          dwg_dynapi_header_set_value (dwg, key, &pt, 1);
        }
      else if (t->type == JSMN_ARRAY
               && (strEQc (f->type, "2BD") || strEQc (f->type, "2RD")
                   || strEQc (f->type, "2DPOINT")
                   || strEQc (f->type, "2BD_1")))
        {
          BITCODE_2DPOINT pt;
          json_2DPOINT (dat, tokens, name, key, f->type, &pt);
          dwg_dynapi_header_set_value (dwg, key, &pt, 1);
        }
      else if (t->type == JSMN_ARRAY
               && (strEQc (f->type, "BS") || strEQc (f->type, "RS")))
        {
          int size1 = t->size;
          BITCODE_BS *nums = (BITCODE_BS *)CALLOC (f->size, 1);
          tokens->index++;
          // fail if not malloced or inlined array (but json has an array)
          if (f->size <= 2 && size1 > 1)
            {
              LOG_ERROR (
                  "Invalid JSON: HEADER.%s array where primitive expected",
                  f->name);
              FREE (nums);
              tokens->index += size1;
              return 0;
            }
          for (int k = 0; k < size1; k++)
            {
              JSON_TOKENS_CHECK_OVERFLOW_ERR
              if (k < (f->size / 2))
                {
                  nums[k] = (BITCODE_BS)json_long (dat, tokens);
                  LOG_TRACE ("%s.%s[%d]: " FORMAT_BS " [%s]\n", name, key, k,
                             nums[k], f->type);
                }
              else
                {
                  LOG_WARN ("Ignored %s.%s[%d]: " FORMAT_BL " [%s]", name, key,
                            k, json_long (dat, tokens), f->type);
                }
            }
          if (!size1)
            LOG_TRACE ("%s.%s: [%s] empty\n", name, key, f->type);
          if (!f->is_malloc)
            {
              dwg_dynapi_header_set_value (dwg, key, nums, 1);
              FREE (nums);
            }
          else
            dwg_dynapi_header_set_value (dwg, key, &nums, 1);
        }
      else if (strEQc (f->type, "TIMEBLL") || strEQc (f->type, "TIMERLL"))
        {
          BITCODE_TIMEBLL date = { 0 };
          json_TIMEBLL (dat, tokens, key, &date);
          dwg_dynapi_header_set_value (dwg, key, &date, 0);
        }
      else if (strEQc (f->type, "CMC"))
        {
          BITCODE_CMC color = { 0 };
          json_CMC (dat, dwg, tokens, name, key, &color);
          dwg_dynapi_header_set_value (dwg, key, &color, 0);
        }
      else if (t->type == JSMN_ARRAY && strEQc (f->type, "H"))
        {
          BITCODE_H hdl
              = json_HANDLE (dat, dwg, tokens, section, key, NULL, -1);
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
    dwg->dwg_class = (Dwg_Class *)CALLOC (size, sizeof (Dwg_Class));
  else
    dwg->dwg_class = (Dwg_Class *)REALLOC (
        dwg->dwg_class, (dwg->num_classes + size) * sizeof (Dwg_Class));
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
          if (!oldkl->number || !oldkl->dxfname || !oldkl->appname
              || !oldkl->cppname || !oldkl->item_class_id)
            {
              klass = oldkl;
              i--;
              size--;
              dwg->num_classes--;
              LOG_ERROR (
                  "Illegal CLASS [%d]. Mandatory field missing, skipped", i)
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
              klass->number = (BITCODE_BS)json_long (dat, tokens) & 0xFFFF;
              LOG_TRACE ("\nCLASS[%d].number: " FORMAT_BS "\n", i,
                         klass->number)
              if (klass->number != i + 500)
                LOG_WARN ("Possibly illegal class number %d, expected %d",
                          klass->number, i + 500)
            }
          else if (strEQc (key, "dxfname"))
            {
              klass->dxfname = json_string (dat, tokens);
              if (dwg->header.version >= R_2007)
                klass->dxfname_u = bit_utf8_to_TU (klass->dxfname, 0);
              LOG_TRACE ("dxfname: \"%s\"\n", klass->dxfname);
            }
          else if (strEQc (key, "cppname"))
            {
              LOG_TRACE ("cppname: \"%.*s\"\n", t->end - t->start,
                         &dat->chain[t->start])
              /*if (dwg->header.version >= R_2007)
                klass->cppname = (char*)json_wstring (dat, tokens);
              else*/
              klass->cppname = json_string (dat, tokens);
            }
          else if (strEQc (key, "appname"))
            {
              LOG_TRACE ("appname \"%.*s\"\n", t->end - t->start,
                         &dat->chain[t->start])
              /*if (dwg->header.version >= R_2007)
                klass->appname = (char*)json_wstring (dat, tokens);
              else*/
              klass->appname = json_string (dat, tokens);
            }
          else if (strEQc (key, "proxyflag"))
            {
              klass->proxyflag = json_long (dat, tokens);
              LOG_TRACE ("proxyflag %u\n", klass->proxyflag)
            }
          else if (strEQc (key, "num_instances"))
            {
              klass->num_instances = json_long (dat, tokens);
              LOG_TRACE ("num_instances %u\n", klass->num_instances)
            }
          else if (strEQc (key, "is_zombie"))
            {
              klass->is_zombie = json_long (dat, tokens);
              LOG_TRACE ("is_zombie %d\n", klass->is_zombie)
            }
          else if (strEQc (key, "item_class_id"))
            {
              klass->item_class_id = json_long (dat, tokens);
              LOG_TRACE ("item_class_id %u\n", klass->item_class_id)
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

static bool
eed_need_size (Bit_Chain *restrict dat, Dwg_Eed *restrict eed,
               const unsigned int i, const int need, int *havep)
{
  if (need > *havep)
    {
      BITCODE_BS size;
      unsigned int isize;
      int diff = need - *havep;
      // find isize
      for (isize = i; !eed[isize].size && isize > 0; isize--)
        ;
      size = eed[isize].size;
      LOG_TRACE (" extend eed[%u].size %u +%d (have: %d, need: %d)\n", isize,
                 size, diff, *havep, need)
      eed[i].data = (Dwg_Eed_Data *)REALLOC (eed[i].data, size + diff);
      eed[isize].size += diff;
      *havep = size + diff - need;
      return true;
    }
  *havep -= need;
  return false;
}

static void
json_eed (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
          jsmntokens_t *restrict tokens, Dwg_Object_Object *restrict obj)
{
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  const char *name = "";
  int isize = -1;
  uint32_t size = 0;
  obj->eed = (Dwg_Eed *)CALLOC (t->size, sizeof (Dwg_Eed));
  obj->num_eed = t->size;
  LOG_TRACE ("num_eed: " FORMAT_BL " [BL]\n", obj->num_eed);
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
                  // see below: if does_cross_unicode_datversion (dat) we need
                  // to recalc
                  obj->eed[i].size = (BITCODE_BS)size & 0xFFFF;
                  LOG_TRACE ("eed[%u].size " FORMAT_BS "\n", i,
                             obj->eed[i].size);
                  have = size + 1; // we overallocate by 1 for the ending NUL
                  obj->eed[i].data = (Dwg_Eed_Data *)CALLOC (have, 1);
                  LOG_INSANE (" alloc eed[%u].data: %d\n", i, have)
                }
              else if (strEQc (key, "handle"))
                {
                  BITCODE_H hdl = json_HANDLE (dat, dwg, tokens, name,
                                               "eed[i].handles", NULL, -1);
                  memcpy (&obj->eed[i].handle, &hdl->handleref,
                          sizeof (Dwg_Handle));
                  if (isize != (int)i || isize > (int)obj->num_eed)
                    {
                      LOG_ERROR (
                          "Missing eed[%u].size field %d or overflow at %s", i,
                          isize, key)
                      break;
                    }
                }
              else if (strEQc (key, "code"))
                {
                  uint32_t code = json_long (dat, tokens);
                  if (isize != (int)i)
                    {
                      if (have > 0)
                        {
                          obj->eed[i - 1].data = (Dwg_Eed_Data *)REALLOC (
                              obj->eed[i - 1].data, size - have);
                          LOG_INSANE (" REALLOC eed[%u].data: %d\n", i - 1,
                                      (int)(size - have))
                        }
                      have = size - have - 1;
                      obj->eed[i].data = (Dwg_Eed_Data *)CALLOC (have, 1);
                      LOG_INSANE (" alloc eed[%u].data: %d\n", i, have)
                    }
                  have--;
                  obj->eed[i].data->code = (BITCODE_RC)code;
                  LOG_TRACE ("eed[%u].data.code " FORMAT_RC "\n", i, code);
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
                        /*PRE (R_2007a)*/
                        {
                          char *s = json_string (dat, tokens);
                          BITCODE_RS len = strlen (s) & 0xFFFF;
                          if (eed_need_size (dat, obj->eed, i, len + 1 + 5,
                                             &have))
                            data = obj->eed[i].data;
                          data->u.eed_0.is_tu = 0;
                          data->u.eed_0.length = len & 0xFFFF;
                          data->u.eed_0.codepage = dwg->header.codepage;
                          if (len)
                            memcpy (&data->u.eed_0.string, s,
                                    (len + 1) & 0xFFFF);
                          LOG_TRACE ("eed[%u].data.value \"%s\"\n", i, s);
                          have++; // ignore the ending NUL
                          FREE (s);
                          // with PRE (R_2007a), the size gets smaller
                          if (does_cross_unicode_datversion (dat))
                            {
                              int oldsize = (len * 2) + 5;
                              size = len + 5;
                              obj->eed[isize].size -= (oldsize - size);
                            }
                        }
                        /*
                      LATER_VERSIONS
                        {
                          BITCODE_TU s = json_wstring (dat, tokens);
                          int len = bit_wcs2len (s);
                          if (eed_need_size (dat, obj->eed, i, (len * 2) + 1 +
                      2, &have)) data = obj->eed[i].data;
                          data->u.eed_0_r2007.length = len;
                          if (len)
                            memcpy (&data->u.eed_0_r2007.string, s, (len + 1) *
                      2); have += 2; // ignore the ending NUL LOG_TRACE
                      ("eed[%u].data.value \"%.*s\"\n", i, t->end - t->start,
                                     &dat->chain[t->start]);
                          FREE (s);
                        }
                        */
                      }
                      break;
                    case 1:
                      if (eed_need_size (dat, obj->eed, i, 3, &have))
                        data = obj->eed[i].data;
                      data->u.eed_1.appid_index
                          = (BITCODE_RS)json_long (dat, tokens);
                      LOG_TRACE ("eed[%u].data.appid_index %d\n", i,
                                 (int)data->u.eed_1.appid_index);
                      break;
                    case 2:
                      if (eed_need_size (dat, obj->eed, i, 1, &have))
                        data = obj->eed[i].data;
                      data->u.eed_2.close
                          = (BITCODE_RC)json_long (dat, tokens);
                      LOG_TRACE ("eed[%u].data.close %d\n", i,
                                 data->u.eed_2.close);
                      break;
                    case 3:
                      eed_need_size (dat, obj->eed, i, 4, &have);
                      data->u.eed_3.layer = json_long (dat, tokens);
                      LOG_TRACE ("eed[%u].data.layer " FORMAT_RLL "\n", i,
                                 data->u.eed_3.layer);
                      break;
                    case 4:
                      {
                        size_t len;
                        unsigned char *s
                            = json_binary (dat, tokens, "eed", &len);
                        // FIXME wrong obj with ubsan
                        if (eed_need_size (dat, obj->eed, i,
                                           (len + 2) & INT_MAX, &have))
                          data = obj->eed[i].data;
                        memcpy (&data->u.eed_4.data, s, len & 0xFF);
                        data->u.eed_4.length = len & 0xFF;
                        FREE (s);
                        break;
                      }
                    case 5:
                      {
                        BITCODE_H hdl
                            = json_HANDLE (dat, dwg, tokens, name,
                                           "eed[i].u.eed_5.entity", NULL, -1);
                        JSON_TOKENS_CHECK_OVERFLOW_VOID
                        memcpy (&data->u.eed_5.entity, &hdl->handleref.value,
                                sizeof (hdl->handleref.value));
                        break;
                      }
                    case 10:
                    case 11:
                    case 12:
                    case 13:
                    case 14:
                    case 15:
                      {
                        BITCODE_3BD pt;
                        json_3DPOINT (dat, tokens, name, "eed", "3RD", &pt);
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
                      LOG_TRACE ("eed[%u].data.value %f\n", i,
                                 data->u.eed_40.real);
                      break;
                    case 70:
                      if (eed_need_size (dat, obj->eed, i, 2, &have))
                        data = obj->eed[i].data;
                      data->u.eed_70.rs = (BITCODE_RS)json_long (dat, tokens);
                      LOG_TRACE ("eed[%u].data.value %d\n", i,
                                 (int)data->u.eed_70.rs);
                      break;
                    case 71:
                      if (eed_need_size (dat, obj->eed, i, 4, &have))
                        data = obj->eed[i].data;
                      data->u.eed_71.rl = json_long (dat, tokens);
                      LOG_TRACE ("eed[%u].data.value %d\n", i,
                                 (int)data->u.eed_71.rl);
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
            jsmntokens_t *restrict tokens, Dwg_Object_XRECORD *restrict obj)
{
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  Dwg_Resbuf *rbuf;
  BITCODE_BL size = 0;
  const char *name = "XRECORD";
  obj->xdata = (Dwg_Resbuf *)CALLOC (1, sizeof (Dwg_Resbuf));
  rbuf = obj->xdata;
  obj->num_xdata = t->size;
  LOG_INSANE ("num_xdata: " FORMAT_BL "\n", obj->num_xdata);
  tokens->index++; // array of objects
  for (unsigned i = 0; i < obj->num_xdata; i++)
    {
      char key[80];
      JSON_TOKENS_CHECK_OVERFLOW_ERR
      t = &tokens->tokens[tokens->index];
      if (t->type == JSMN_ARRAY && t->size == 2) // of [ type, value ]
        {
          Dwg_Resbuf *old = rbuf;
          enum RESBUF_VALUE_TYPE vtype;

          tokens->index++;
          JSON_TOKENS_CHECK_OVERFLOW_ERR
          rbuf->type = (BITCODE_RS)json_long (dat, tokens);
          JSON_TOKENS_CHECK_OVERFLOW_ERR
          LOG_INSANE ("xdata[%u]: type %d\n", i, rbuf->type);
          size += 2;
          vtype = dwg_resbuf_value_type (rbuf->type);
          switch (vtype)
            {
            case DWG_VT_STRING:
              {
                char *s = json_string (dat, tokens);
                size_t len = strlen (s);
                JSON_TOKENS_CHECK_OVERFLOW_ERR
                if (len > 0xFFFF)
                  {
                    LOG_ERROR ("xdata string overflow len=%" PRIuSIZE, len);
                    return DWG_ERR_INVALIDDWG;
                  }
                rbuf->value.str.size = len & 0xFFFF;
                // here the xdata_size gets re-calculated from size
                PRE (R_2007a) // from version
                {
                  rbuf->value.str.is_tu = 0;
                  rbuf->value.str.u.data = s;
                  rbuf->value.str.codepage = dwg->header.codepage;
                  LOG_TRACE ("xdata[%u]: \"%s\" [TV %d]\n", i, s,
                             (int)rbuf->type);
                  size += (len & 0xFFFF) + 3;
                }
                LATER_VERSIONS
                {
                  rbuf->value.str.is_tu = 1;
                  rbuf->value.str.u.wdata = bit_utf8_to_TU (s, 0);
                  FREE (s);
                  LOG_TRACE_TU ("xdata", rbuf->value.str.u.wdata, rbuf->type);
                  size += ((len * 2) & 0xFFFF) + 2;
                }
              }
              break;
            case DWG_VT_BOOL:
            case DWG_VT_INT8:
              {
                uint32_t l = json_long (dat, tokens);
                JSON_TOKENS_CHECK_OVERFLOW_ERR
                rbuf->value.i8 = (BITCODE_RC)l;
                LOG_TRACE ("xdata[%u]: " FORMAT_RC " [RC %d]\n", i,
                           rbuf->value.i8, (int)rbuf->type);
                size += 1;
              }
              break;
            case DWG_VT_INT16:
              {
                uint32_t l = json_long (dat, tokens);
                JSON_TOKENS_CHECK_OVERFLOW_ERR
                rbuf->value.i16 = (BITCODE_RS)l;
                LOG_TRACE ("xdata[%u]: " FORMAT_BS " [RS %d]\n", i,
                           rbuf->value.i16, (int)rbuf->type);
                size += 2;
              }
              break;
            case DWG_VT_INT32:
              {
                uint32_t l = json_long (dat, tokens);
                JSON_TOKENS_CHECK_OVERFLOW_ERR
                rbuf->value.i32 = (BITCODE_RL)l;
                LOG_TRACE ("xdata[%u]: " FORMAT_BL " [RL %d]\n", i, l,
                           (int)rbuf->type);
                size += 4;
              }
              break;
            case DWG_VT_INT64:
              {
                uint64_t l = json_longlong (dat, tokens);
                JSON_TOKENS_CHECK_OVERFLOW_ERR
                rbuf->value.i64 = l;
                LOG_TRACE ("xdata[%u]: " FORMAT_RLL " [RLL %d]\n", i, l,
                           (int)rbuf->type);
                size += 8;
              }
              break;
            case DWG_VT_REAL:
              rbuf->value.dbl = json_float (dat, tokens);
              JSON_TOKENS_CHECK_OVERFLOW_ERR
              LOG_TRACE ("xdata[%u]: %f [RD %d]\n", i, rbuf->value.dbl,
                         (int)rbuf->type);
              size += 8;
              break;
            case DWG_VT_POINT3D:
              {
                BITCODE_3BD pt;
                json_3DPOINT (dat, tokens, name, "xdata", "3RD", &pt);
                JSON_TOKENS_CHECK_OVERFLOW_ERR
                memcpy (&rbuf->value.pt, &pt, 24);
                size += 24;
              }
              break;
            case DWG_VT_BINARY:
              {
                size_t len;
                unsigned char *s = json_binary (dat, tokens, "xdata", &len);
                JSON_TOKENS_CHECK_OVERFLOW_ERR
                rbuf->value.str.u.data = (char *)s;
                rbuf->value.str.size = len & 0xFFFF;
                size += (len + 1) & 0xFFFF;
                break;
              }
            case DWG_VT_HANDLE:
            case DWG_VT_OBJECTID:
              {
                uint32_t l = json_long (dat, tokens);
                JSON_TOKENS_CHECK_OVERFLOW_ERR
                rbuf->value.absref = (BITCODE_RLL)l;
                LOG_TRACE ("xdata[%u]: " FORMAT_HV " [H %d]\n", i,
                           rbuf->value.absref, (int)rbuf->type);
                size += 8;
              }
              break;
            case DWG_VT_INVALID:
            default:
              break;
            }
          rbuf = (Dwg_Resbuf *)CALLOC (1, sizeof (Dwg_Resbuf));
          old->nextrb = rbuf;
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
        LOG_WARN ("Changed XRECORD.xdata_size from %u to %u", obj->xdata_size,
                  size)
      else
        LOG_TRACE ("Changed XRECORD.xdata_size from %u to %u\n",
                   obj->xdata_size, size)
    }
  obj->xdata_size = size;
  return 0;
}

static int
json_acis_data (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                jsmntokens_t *restrict tokens, const Dwg_Object *restrict obj)
{
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  int len = 0;
  int alloc = t->end - t->start;
  int size = t->size;
  char *s = (char *)CALLOC (alloc, 1);
  Dwg_Entity__3DSOLID *_obj = obj->tio.entity->tio._3DSOLID;
  BITCODE_BS acis_version;

  dwg_dynapi_entity_value (_obj, obj->name, "version", &acis_version, NULL);
  LOG_INSANE ("acis lines: %d\n", t->size);
  tokens->index++; // array of strings with version 1
  for (int i = 0; i < size; i++)
    {
      JSON_TOKENS_CHECK_OVERFLOW_ERR
      t = &tokens->tokens[tokens->index];
      if (t->type == JSMN_STRING)
        {
          int l = t->end - t->start;
          // could be made faster, but those strings are short, and not that
          // often.
          if (acis_version == 1)
            {
              len += l + 1;
              if (len > alloc) // cannot happen, as we take the length of the
                               // json source array
                {
                  LOG_WARN ("Internal surprise, acis_data overshoot %d > %d",
                            len, alloc);
                  alloc += 60;
                  s = (char *)REALLOC (s, alloc);
                }
              strncat (s, (char *)&dat->chain[t->start], l);
              strcat (s, "\n");
            }
          else
            {
              len += l;
              if (len > alloc)
                {
                  alloc = len;
                  s = (char *)REALLOC (s, alloc);
                }
              if (i == 0) // first line is plain, second is binary
                strncat (s, (char *)&dat->chain[t->start], l);
              else
                {
                  // 15 is the length of the first line: "ACIS BinaryFile"
                  const unsigned blen = l / 2;
                  unsigned read;
                  char *pos = (char *)&dat->chain[t->start];
                  if ((len - l) != 15 || size != 2)
                    LOG_ERROR (
                        "Invalid %s ACIS %u json format. len %d, size %d",
                        obj->name, acis_version, len - l, size);
                  if (!pos)
                    {
                      LOG_ERROR ("Invalid %s ACIS %u json format. NULL string",
                                 obj->name, acis_version);
                      FREE (s);
                      return DWG_ERR_INVALIDTYPE;
                    }
                  if ((read = in_hex2bin ((unsigned char *)&s[15], pos, blen)
                              != blen))
                    LOG_ERROR ("in_hex2bin with key %s at pos %u of %u",
                               "acis_data", read, blen);
                  len = 15 + blen;
                }
            }
        }
      tokens->index++;
    }
  LOG_TRACE ("%s.acis_data: %s\n", obj->name, s);
  if (acis_version > 1)
    {
      BITCODE_BL num_blocks = 1;
      BITCODE_BL sab_size = len;
      BITCODE_BL *block_size = (BITCODE_BL *)CALLOC (2, sizeof (BITCODE_BL));
      block_size[0] = len;
      block_size[1] = 0;
      LOG_TRACE (" num_blocks: " FORMAT_BL " [BL]\n", num_blocks);
      LOG_TRACE (" sab_size: " FORMAT_BL " [BL]\n", sab_size);
      LOG_TRACE (" block_size[0]: %d [BL]\n", block_size[0]);
      dwg_dynapi_entity_set_value (_obj, obj->name, "num_blocks", &num_blocks,
                                   true);
      dwg_dynapi_entity_set_value (_obj, obj->name, "block_size", &block_size,
                                   true);
      dwg_dynapi_entity_set_value (_obj, obj->name, "sab_size", &sab_size,
                                   true);
    }
  // just keep this s ptr, no utf8
  return dwg_dynapi_entity_set_value (obj->tio.entity->tio._3DSOLID, obj->name,
                                      "acis_data", &s, false)
             ? 0
             : DWG_ERR_INVALIDTYPE;
}

static const Dwg_DYNAPI_field *
find_sizefield (const Dwg_DYNAPI_field *restrict fields,
                const char *restrict key)
{
  const Dwg_DYNAPI_field *f;
  char *s = (char *)MALLOC (strlen (key) + 12);
  strcpy (s, key);
  strcat (s, "_size");
  for (f = &fields[0]; f->name; f++)
    {
      if (strEQ (s, f->name))
        {
          long size = 0;
          FREE (s);
          return f;
        }
    }
  FREE (s);
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
  else if (strEQc (key, "transmatrix"))
    ; // ignore
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

static void
json_fixup_JUMP (Dwg_Entity_JUMP *_obj)
{
  if (_obj->jump_address_raw > 0xffffff)
    {
      _obj->jump_address &= 0xffffff;
      LOG_TRACE ("jump_address => " FORMAT_BLx "\n", _obj->jump_address);
      switch ((_obj->jump_address_raw & 0xff000000) >> 24)
        {
        case 0:
          LOG_TRACE ("jump_entity_section => DWG_ENTITIES_SECTION\n");
          break;
        case 0x40:
          _obj->jump_entity_section = DWG_BLOCKS_SECTION;
          LOG_TRACE ("jump_entity_section => DWG_BLOCKS_SECTION\n");
          break;
        case 0x80:
          _obj->jump_entity_section = DWG_EXTRA_SECTION;
          LOG_TRACE ("jump_entity_section => DWG_EXTRA_SECTION\n");
          break;
        default:
          LOG_ERROR ("Invalid jump_entity_section %x ignored",
                     (_obj->jump_address_raw & 0xff000000) >> 24)
        }
    }
  else
    LOG_TRACE ("jump_entity_section => DWG_ENTITIES_SECTION\n");
}

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
  const Dwg_DYNAPI_field *f = (Dwg_DYNAPI_field *)fields;
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  int error = 0;
  LOG_INSANE ("-search %s key %s: %s %.*s\n", name, key, t_typename[t->type],
              t->end - t->start, &dat->chain[t->start]);
  JSON_TOKENS_CHECK_OVERFLOW_ERR;
  for (; f->name; f++)
    {
      if (strEQ (f->name, key))
        break;
    }
  // Found common, subclass or entity key, check types
  if (f && f->name)
    {
      LOG_INSANE ("-found %s [%s] %s\n", f->name, f->type,
                  t_typename[t->type]);
      if (t->type == JSMN_PRIMITIVE
          && (strEQc (f->type, "BD") || strEQc (f->type, "RD")
              || strEQc (f->type, "BT")))
        {
          double num = json_float (dat, tokens);
          JSON_TOKENS_CHECK_OVERFLOW_ERR;
          LOG_TRACE ("%s.%s: %f [%s]\n", name, key, num, f->type);
          dwg_dynapi_field_set_value (dwg, _obj, f, &num, 0);
        }
      // all numfields are calculated from actual array sizes
      // for easier adding or deleting entries.
      else if (t->type == JSMN_PRIMITIVE && memBEGINc (key, "num_"))
        {
          tokens->index++;
          JSON_TOKENS_CHECK_OVERFLOW_ERR;
          LOG_TRACE ("%s.%s: %.*s (ignored)\n", name, key, t->end - t->start,
                     &dat->chain[t->start]);
        }
      else if (t->type == JSMN_PRIMITIVE
               && (strEQc (f->type, "RC") || strEQc (f->type, "B")
                   || strEQc (f->type, "BB") || strEQc (f->type, "RS")
                   || strEQc (f->type, "BS") || strEQc (f->type, "RL")
                   || strEQc (f->type, "BL") || strEQc (f->type, "BLd")
                   || strEQc (f->type, "BSd") || strEQc (f->type, "RCd")
                   || strEQc (f->type, "RSd") || strEQc (f->type, "4BITS")))
        {
          uint32_t num = json_long (dat, tokens);
          JSON_TOKENS_CHECK_OVERFLOW_ERR
          LOG_TRACE ("%s.%s: " FORMAT_BL " [%s]\n", name, key, num, f->type);
          dwg_dynapi_field_set_value (dwg, _obj, f, &num, 0);
          if (strEQc (name, "JUMP") && strEQc (key, "jump_address_raw"))
            {
              json_fixup_JUMP ((Dwg_Entity_JUMP *)_obj);
              LOG_TRACE ("%s.%s: " FORMAT_BLx " [RLx]\n", name, key, num);
            }
        }
      else if (t->type == JSMN_PRIMITIVE
               && (strEQc (f->type, "RLL") || strEQc (f->type, "BLL")))
        {
          uint64_t num = json_longlong (dat, tokens);
          JSON_TOKENS_CHECK_OVERFLOW_ERR
          LOG_TRACE ("%s.%s: " FORMAT_RLL " [%s]\n", name, key, num, f->type);
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
              size_t blen = len / 2;
              size_t read;
              unsigned char *buf
                  = len ? (unsigned char *)MALLOC (blen + 1) : NULL;
              char *pos = str;
              char *old;
              if (buf && pos)
                {
                  if ((read = in_hex2bin (buf, pos, blen) != blen))
                    LOG_ERROR ("in_hex2bin with key %s at pos %" PRIuSIZE
                               " of %" PRIuSIZE,
                               key, read, blen);
                  buf[blen] = '\0';
                  LOG_TRACE ("%s.%s: '%.*s'... [BINARY %" PRIuSIZE "]\n", name,
                             key, MIN ((int)len, 60), str, len);
                }
              FREE (str);
              json_set_sizefield (_obj, fields, key, blen);
              // set the ptr directly, no alloc, no conversion.
              old = &((char *)_obj)[f->offset];
              memcpy (old, &buf, f->size);
            }
          // is r11 fixed size string? (tables and controls,
          // marked as TV, not TF)
          else if (dwg->header.from_version <= R_12
                   && (dwg_obj_is_table (obj) || dwg_obj_is_control (obj)))
            {
              char *old;
              // sizes:
              // 16: DIMSTYLE.DIM*
              // 32: *.name
              // 48: LTYPE.description
              // 64: STYLE.font_file, bigfont_file
              // 66: DIMSTYLE.DIMBLK2_T
              const int k = strEQc (key, "name")               ? 32
                            : obj->fixedtype == DWG_TYPE_LTYPE ? 48
                            : obj->fixedtype == DWG_TYPE_DIMSTYLE
                                    && strEQc (key, "DIMBLK2_T")
                                ? 66
                            : obj->fixedtype == DWG_TYPE_DIMSTYLE ? 16
                                                                  : 64;
              tokens->index--;
              FREE (str);
              str = json_fixed_string (dat, k, tokens);
              LOG_TRACE ("%s.%s: \"%s\" [TF %d %d]\n", name, key, str, k,
                         f->dxf);
              old = &((char *)_obj)[f->offset];
              memcpy (old, &str, sizeof (char *));
            }
          else if (strEQc (f->type, "TF")) // oleclient, strings_area, ...
                                           // fixup size field
            {
              char *old;
              if (strEQc (key, "strings_area"))
                {
                  const size_t k
                      = dwg->header.from_version > R_2004 ? 512 : 256;
                  const size_t blen = len / 2;
                  // binary hexstring
                  BITCODE_TF buf = (BITCODE_TF)MALLOC (blen);
                  len = in_hex2bin (buf, str, blen);
                  if (len != blen)
                    LOG_ERROR ("in_hex2bin with key %s at pos %" PRIuSIZE
                               " of %" PRIuSIZE,
                               key, len, blen);
                  memcpy (str, buf, len);
                  FREE (buf);
                  if (len > k)
                    {
                      LOG_ERROR ("Illegal %s.%s length %" PRIuSIZE
                                 " > %" PRIuSIZE ", stripped",
                                 name, key, len, k);
                      len = k;
                    }
                  else if (len != k)
                    {
                      str = (char *)REALLOC (str, k);
                      memset (&str[len + 1], 0, k - len - 1);
                    }
                }
              else if (f->size > sizeof (char *))
                {
                  str = (char *)REALLOC (str, f->size);
                  memset (&str[len + 1], 0, f->size - len - 1);
                }
              LOG_TRACE ("%s.%s: \"%s\" [%s %d]\n", name, key, str, f->type,
                         f->size);
              if (strNE (key, "strings_area"))
                json_set_sizefield (_obj, fields, key, len);
              old = &((char *)_obj)[f->offset];
              memcpy (old, &str, sizeof (char *));
              // dwg_dynapi_field_set_value (dwg, _obj, f, &str, 1);
            }
          else
            {
              LOG_TRACE ("%s.%s: \"%s\" [%s] len=%d\n", name, key, str,
                         f->type, (int)len);
              dwg_dynapi_field_set_value (dwg, _obj, f, &str, 1);
              FREE (str);
            }
        }
      // arrays
      else if (t->type == JSMN_ARRAY
               && (strEQc (f->type, "3BD") || strEQc (f->type, "3RD")
                   || strEQc (f->type, "3DPOINT") || strEQc (f->type, "BE")
                   || strEQc (f->type, "3BD_1")))
        {
          BITCODE_3DPOINT pt;
          json_3DPOINT (dat, tokens, name, key, f->type, &pt);
          JSON_TOKENS_CHECK_OVERFLOW_ERR
          dwg_dynapi_field_set_value (dwg, _obj, f, &pt, 1);
        }
      else if (t->type == JSMN_ARRAY
               && (strEQc (f->type, "2BD") || strEQc (f->type, "2RD")
                   || strEQc (f->type, "2DPOINT")
                   || strEQc (f->type, "2BD_1")))
        {
          BITCODE_2DPOINT pt;
          json_2DPOINT (dat, tokens, name, key, f->type, &pt);
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
          BITCODE_CMC color = { 0, 0, 0, 0, 0, NULL, NULL, NULL, 0, 0, 0 };
          json_CMC (dat, dwg, tokens, name, key, &color);
          JSON_TOKENS_CHECK_OVERFLOW_ERR
          dwg_dynapi_field_set_value (dwg, _obj, f, &color, 1);
        }
      else if (t->type == JSMN_ARRAY && strEQc (f->type, "H"))
        {
          BITCODE_H hdl;
          hdl = json_HANDLE (dat, dwg, tokens, name, key, obj, -1);
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
            }
          hdls
              = size1 ? (BITCODE_H *)CALLOC (size1, sizeof (BITCODE_H)) : NULL;
          json_set_numfield (_obj, fields, key, (long)size1);
          tokens->index++;
          for (int k = 0; k < t->size; k++)
            {
              BITCODE_H hdl;
              JSON_TOKENS_CHECK_OVERFLOW_ERR
              hdl = json_HANDLE (dat, dwg, tokens, name, key, obj, k);
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
            LOG_TRACE ("%s.%s: [%s] empty\n", name, key, f->type);
          // memcpy (&((char *)_obj)[f->offset], &hdls, sizeof (hdls));
          dwg_dynapi_field_set_value (dwg, _obj, f, &hdls, 1);
        }
      else if (t->type == JSMN_ARRAY
               && (strEQc (f->type, "T*") || strEQc (f->type, "TV*")))
        {
          int skip = 0;
          BITCODE_BL size1 = t->size;
          BITCODE_T *elems;
          if (memBEGINc (name, "DICTIONARY") && strEQc (key, "texts"))
            {
              LOG_ERROR ("Illegal old json format");
              return DWG_ERR_INVALIDDWG;
            }
          elems
              = size1 ? (BITCODE_T *)CALLOC (size1, sizeof (BITCODE_T)) : NULL;
          json_set_numfield (_obj, fields, key, (long)size1);
          tokens->index++;
          for (int k = 0; k < t->size; k++)
            {
              JSON_TOKENS_CHECK_OVERFLOW_ERR
              if (k < (int)size1)
                {
                  elems[k] = json_string (dat, tokens);
                  LOG_TRACE ("%s.%s[%d]: \"%s\" [%s]\n", name, key, k,
                             elems[k], f->type);
                }
              else
                {
                  tokens->index++;
                  t = &tokens->tokens[tokens->index];
                  LOG_WARN ("%s.%s[%d]: \"%.*s\" [%s] ignored", name, key, k,
                            t->end - t->start, &dat->chain[t->start], f->type);
                }
            }
          if (!t->size)
            LOG_TRACE ("%s.%s: [%s] empty\n", name, key, f->type);
          dwg_dynapi_field_set_value (dwg, _obj, f, &elems, 1);
        }
      else if (t->type == JSMN_ARRAY
               && (strEQc (f->type, "3DPOINT*") || strEQc (f->type, "3BD*")))
        {
          int size1 = t->size;
          BITCODE_3DPOINT *pts
              = size1 ? (BITCODE_3BD *)CALLOC (size1, sizeof (BITCODE_3BD))
                      : NULL;
          json_set_numfield (_obj, fields, key, size1);
          tokens->index++;
          for (int k = 0; k < size1; k++)
            {
              JSON_TOKENS_CHECK_OVERFLOW_ERR;
              json_3DPOINT (dat, tokens, name, key, f->type, &pts[k]);
            }
          if (!size1)
            LOG_TRACE ("%s.%s: [%s] empty\n", name, key, f->type);
          dwg_dynapi_field_set_value (dwg, _obj, f, &pts, 1);
        }
      else if (t->type == JSMN_ARRAY && strEQc (f->type, "2RD*"))
        {
          const int size1 = t->size;
          BITCODE_2DPOINT *pts = size1 ? (BITCODE_2DPOINT *)CALLOC (
                                             size1, sizeof (BITCODE_2DPOINT))
                                       : NULL;
          json_set_numfield (_obj, fields, key, size1);
          tokens->index++;
          for (int k = 0; k < size1; k++)
            {
              JSON_TOKENS_CHECK_OVERFLOW_ERR;
              json_2DPOINT (dat, tokens, name, key, f->type, &pts[k]);
            }
          if (!size1)
            LOG_TRACE ("%s.%s: [%s] empty\n", name, key, f->type);
          dwg_dynapi_field_set_value (dwg, _obj, f, &pts, 1);
        }
      else if (t->type == JSMN_ARRAY
               && (strEQc (f->type, "BD*") || strEQc (f->type, "RD")))
        {
          const int size1 = t->size;
          const int max_k
              = !f->is_malloc ? (int)(f->size / sizeof (BITCODE_BD)) : size1;
          BITCODE_BD *nums;
          // fail if not malloced or inlined array (but json has an array)
          if (f->size <= 8 && size1 > 1 && !f->is_malloc)
            {
              LOG_ERROR ("Invalid JSON: %s.%s array where primitive expected",
                         name, f->name);
              tokens->index += size1;
              return 0;
            }
          if (f->is_malloc)
            {
              nums = size1 ? (BITCODE_BD *)CALLOC (size1, sizeof (BITCODE_BD))
                           : NULL;
              json_set_numfield (_obj, fields, key, size1);
            }
          else
            {
              nums = (BITCODE_BD *)CALLOC (f->size, 1);
            }
          tokens->index++;
          for (int k = 0; k < size1; k++)
            {
              JSON_TOKENS_CHECK_OVERFLOW_ERR
              if (k < max_k)
                {
                  nums[k] = json_float (dat, tokens);
                  LOG_TRACE ("%s.%s[%d]: %f [%s]\n", name, key, k, nums[k],
                             f->type);
                }
              else
                {
                  LOG_WARN ("Ignored %s.%s[%d]: %f [%s]", name, key, k,
                            json_float (dat, tokens), f->type);
                }
            }
          if (!size1)
            {
              LOG_TRACE ("%s.%s: [%s] empty\n", name, key, f->type);
            }
          else
            {
              if (f->is_malloc) // BD*, just copy the pointer
                dwg_dynapi_field_set_value (dwg, _obj, f, &nums, 1);
              else
                { // copy all values
                  dwg_dynapi_field_set_value (dwg, _obj, f, nums, 1);
                  FREE (nums);
                }
            }
        }
      else if (t->type == JSMN_ARRAY
               && (strEQc (f->type, "BL*") || strEQc (f->type, "BLd")))
        {
          const int size1 = t->size;
          const int max_k
              = !f->is_malloc ? (int)(f->size / sizeof (BITCODE_BL)) : size1;
          BITCODE_BL *nums;
          // fail if not malloced or inlined array (but json has an array)
          if (f->size <= 4 && size1 > 1)
            {
              LOG_ERROR ("Invalid JSON: %s.%s array where primitive expected",
                         name, f->name);
              tokens->index += size1;
              return 0;
            }
          if (f->is_malloc)
            {
              nums = size1 ? (BITCODE_BL *)CALLOC (size1, sizeof (BITCODE_BL))
                           : NULL;
              json_set_numfield (_obj, fields, key, size1);
            }
          else
            nums = (BITCODE_BL *)CALLOC (f->size, 1);
          tokens->index++;
          for (int k = 0; k < size1; k++)
            {
              JSON_TOKENS_CHECK_OVERFLOW_ERR
              if (k < max_k)
                {
                  nums[k] = json_long (dat, tokens);
                  if (strEQc (f->type, "BL*"))
                    {
                      LOG_TRACE ("%s.%s[%d]: " FORMAT_BL " [%s]\n", name, key,
                                 k, nums[k], f->type);
                    }
                  else
                    {
                      LOG_TRACE ("%s.%s[%d]: " FORMAT_BLd " [%s]\n", name, key,
                                 k, nums[k], f->type);
                    }
                }
              else
                {
                  tokens->index++;
                  LOG_TRACE ("%s.%s[%d]: [%s] ignored\n", name, key, k,
                             f->type);
                }
            }
          if (!size1)
            {
              LOG_TRACE ("%s.%s: [%s] empty\n", name, key, f->type);
            }
          else
            {
              if (f->is_malloc) // BL*
                dwg_dynapi_field_set_value (dwg, _obj, f, &nums, 1);
              else
                {
                  dwg_dynapi_field_set_value (dwg, _obj, f, nums, 1);
                  FREE (nums);
                }
            }
        }
      else if (t->type == JSMN_ARRAY
               && (strEQc (f->type, "BS") || strEQc (f->type, "BS*")
                   || strEQc (f->type, "RS")))
        {
          const int size1 = t->size;
          const int max_k
              = !f->is_malloc ? (int)(f->size / sizeof (BITCODE_BS)) : size1;
          BITCODE_BS *nums;
          // fail if not malloced or inlined array (but json has an array)
          if (f->size <= 2 && size1 > 1)
            {
              LOG_ERROR ("Invalid JSON: %s.%s array where primitive expected",
                         name, f->name);
              tokens->index += size1;
              return 0;
            }
          if (f->is_malloc)
            {
              nums = size1 ? (BITCODE_BS *)CALLOC (size1, sizeof (BITCODE_BS))
                           : NULL;
              json_set_numfield (_obj, fields, key, size1);
            }
          else
            nums = (BITCODE_BS *)CALLOC (f->size, 1);
          tokens->index++;
          for (int k = 0; k < size1; k++)
            {
              JSON_TOKENS_CHECK_OVERFLOW_ERR
              if (k < max_k)
                {
                  nums[k] = (BITCODE_BS)json_long (dat, tokens);
                  LOG_TRACE ("%s.%s[%d]: " FORMAT_BS " [%s]\n", name, key, k,
                             nums[k], f->type);
                }
              else
                {
                  tokens->index++;
                  LOG_TRACE ("%s.%s[%d]: [%s] ignored\n", name, key, k,
                             f->type);
                }
            }
          if (!size1)
            {
              LOG_TRACE ("%s.%s: [%s] empty\n", name, key, f->type);
            }
          else
            {
              if (f->is_malloc) // BS*
                dwg_dynapi_field_set_value (dwg, _obj, f, &nums, 1);
              else
                {
                  dwg_dynapi_field_set_value (dwg, _obj, f, nums, 1);
                  FREE (nums);
                }
            }
        }
      else if (t->type == JSMN_ARRAY && strEQc (key, "xdata")
               && strEQc (name, "XRECORD"))
        {
          error |= json_xdata (dat, dwg, tokens, (Dwg_Object_XRECORD *)_obj);
          JSON_TOKENS_CHECK_OVERFLOW_ERR
        }
      else if (t->type == JSMN_ARRAY && strEQc (key, "acis_data")
               && strEQc (f->type, "RC*"))
        {
          error |= json_acis_data (dat, dwg, tokens, obj);
          JSON_TOKENS_CHECK_OVERFLOW_ERR;
        }
      else if (t->type == JSMN_ARRAY && strEQc (key, "encr_sat_data")
               && strEQc (f->type, "char **"))
        {
          BITCODE_BL num_blocks = t->size;
          BITCODE_BL *block_size
              = (BITCODE_BL *)CALLOC (num_blocks + 1, sizeof (BITCODE_BL));
          unsigned char **data = (unsigned char **)CALLOC (
              num_blocks + 1, sizeof (unsigned char *));
          tokens->index++;
          LOG_TRACE ("num_blocks: " FORMAT_BL " [BL]\n", num_blocks);
          for (BITCODE_BL k = 0; k < num_blocks; k++)
            {
              size_t len;
              JSON_TOKENS_CHECK_OVERFLOW_ERR
              data[k] = json_binary (dat, tokens, "encr_sat_data", &len);
              block_size[k] = (BITCODE_BL)len;
              LOG_TRACE ("block_size[%d]: %" PRIuSIZE " [BL]\n", k, len);
            }
          block_size[num_blocks] = 0;
          LOG_TRACE ("block_size[%d]: 0 [BL]\n", num_blocks);
          dwg_dynapi_entity_set_value (_obj, obj->name, "num_blocks",
                                       &num_blocks, true);
          dwg_dynapi_entity_set_value (_obj, obj->name, "block_size",
                                       &block_size, true);
          dwg_dynapi_field_set_value (dwg, _obj, f, &data, true);
        }
      // TFFx (binary)
      else if (t->type == JSMN_STRING && strEQc (key, "revision_bytes"))
        {
          size_t len;
          unsigned char *s = json_binary (dat, tokens, "revision_bytes", &len);
          JSON_TOKENS_CHECK_OVERFLOW_ERR
          f = dwg_dynapi_entity_field (name, "revision_bytes");
          if (f && len == 8)
            {
              LOG_TRACE ("%s.%s: [%s]\n", name, key, s);
              dwg_dynapi_field_set_value (dwg, _obj, f, s, true);
            }
          FREE (s);
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
              FREE (subclass);
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
              elems = (char *)CALLOC (MAX (4, num_elems), size_elem);
              LOG_TRACE ("DIMASSOC num_refs = 4\n");
            }
          else
            elems = num_elems ? (char *)CALLOC (num_elems, size_elem) : NULL;
          dwg_dynapi_field_set_value (dwg, _obj, f, &elems, 1);
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
                  FREE (subclass);
                  json_advance_unknown (dat, tokens, t->type, 0);
                  JSON_TOKENS_CHECK_OVERFLOW_ERR
                  return DWG_ERR_INVALIDTYPE;
                }
              LOG_TRACE ("%s.%s[%d]:\n", name, key, k);
              keys = t->size;
              tokens->index++;
              for (int ki = 0; ki < keys; ki++)
                {
                  // separate subclass type loop
                  const Dwg_DYNAPI_field *f1;
                  char key1[80];
                  char *rest;
                  JSON_TOKENS_CHECK_OVERFLOW_ERR
                  json_fixed_key (key1, dat, tokens);
                  LOG_INSANE ("-search %s key: %s\n", subclass, key1);
                  f1 = dwg_dynapi_subclass_field (subclass, key1);
                  if (f1)
                    {
                      LOG_INSANE ("-found %s [%s]\n", f1->name, f1->type);
                      if (!_set_struct_field (dat, obj, tokens,
                                              &elems[k * size_elem], subclass,
                                              key1, sfields))
                        ++tokens->index;
                    }
                  else if ((rest = strchr (key1, '.'))) // embedded struct
                    {
                      *rest = '\0';
                      rest++;
                      f1 = dwg_dynapi_subclass_field (subclass, key1);
                      if (f1 && *rest)
                        {
                          char *sb1 = dwg_dynapi_subclass_name (f1->type);
                          const Dwg_DYNAPI_field *sfields1
                              = sb1 ? dwg_dynapi_subclass_fields (sb1) : NULL;
                          if (!sfields1
                              || !_set_struct_field (
                                  dat, obj, tokens,
                                  &elems[(k * size_elem) + f1->offset], sb1,
                                  rest, sfields1))
                            ++tokens->index;
                          FREE (sb1);
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
          FREE (subclass);
        }
      // subclass structs (embedded):
      else if (t->type == JSMN_OBJECT && memBEGINc (f->type, "Dwg_"))
        {
          int num_keys = t->size; // div by 2 really
          // int size_struct;
          const Dwg_DYNAPI_field *sfields;
          char *subclass = dwg_dynapi_subclass_name (f->type);
          if (!subclass)
            {
              LOG_ERROR ("Unknown subclass type %s", f->type);
              goto unknown_ent;
            }
          // size_struct = dwg_dynapi_fields_size (subclass);
          sfields = dwg_dynapi_subclass_fields (subclass);
          if (!sfields)
            {
              LOG_ERROR ("Unknown subclass name %s", subclass);
              FREE (subclass);
              goto unknown_ent;
            }
          LOG_TRACE ("embedded struct %s %s [%d keys]\n", subclass, key,
                     num_keys / 2);
          tokens->index++;
          // a single struct
          if (!num_keys)
            LOG_TRACE ("%s: [%s] empty\n", key, f->type);
          for (int k = 0; k < num_keys; k++)
            {
              const Dwg_DYNAPI_field *f1;
              char key1[80];
              char *rest;
              JSON_TOKENS_CHECK_OVERFLOW_ERR
              json_fixed_key (key1, dat, tokens);
              LOG_INSANE ("-search %s key %s\n", subclass, key1);
              f1 = dwg_dynapi_subclass_field (subclass, key1);
              if (f1)
                {
                  // subclass offset for _obj
                  void *off = &((char *)_obj)[f->offset + f1->offset];
                  if (!_set_struct_field (dat, obj, tokens, off, subclass,
                                          key1, sfields))
                    ++tokens->index;
                }
              else if ((rest = strchr (key1, '.'))) // embedded struct
                {
                  *rest = '\0';
                  rest++;
                  f1 = dwg_dynapi_subclass_field (subclass, key1);
                  if (f1 && *rest)
                    {
                      void *off = &((char *)_obj)[f->offset + f1->offset];
                      char *subclass1 = dwg_dynapi_subclass_name (f1->type);
                      const Dwg_DYNAPI_field *sfields1
                          = subclass1 ? dwg_dynapi_subclass_fields (subclass1)
                                      : NULL;
                      if (!sfields1
                          || !_set_struct_field (dat, obj, tokens, off,
                                                 subclass1, rest, sfields1))
                        ++tokens->index;
                      FREE (subclass1);
                    }
                }
              if (!f1 || !f1->name) // not found
                {
                  LOG_ERROR ("Unknown subclass field %s.%s", subclass, key1);
                  ++tokens->index;
                }
            }
          FREE (subclass);
        }
      else
        {
        unknown_ent:
          LOG_ERROR ("Unknown type for %s.%s %s with %s", name, key, f->type,
                     t_typename[t->type]);
          ++tokens->index;
          JSON_TOKENS_CHECK_OVERFLOW_ERR
        }
      return error | (f->name ? 1 : 0); // found or not
    }
  else // not found
    {  // maybe it's an embedded subclass. look for the dot(s)
      int found = 0;
      char *rest = strchr ((char *)key, '.');
      while (rest)
        {
          // Currently we have 3 known static arrays, and a few embedded
          // subclasses. Color e.g.
          const Dwg_DYNAPI_field *f1;
          const char *subclass = NULL;
          JSON_TOKENS_CHECK_OVERFLOW_ERR;
          *rest = '\0';
          rest++;
          f1 = dwg_dynapi_entity_field (name, key);
          if (f1 && *rest)
            {
              void *off = &((char *)_obj)[f1->offset];
              const char *subclass1 = dwg_dynapi_subclass_name (f1->type);
              const Dwg_DYNAPI_field *sfields1
                  = subclass1 ? dwg_dynapi_subclass_fields (subclass1) : NULL;
              if (!sfields1 && subclass1)
                sfields1 = dwg_dynapi_entity_fields (subclass1);
              if (!sfields1
                  || !_set_struct_field (dat, obj, tokens, off, subclass1,
                                         rest, sfields1))
                ++tokens->index;
              FREE ((char *)subclass1);
              return error | (f1->name ? 1 : 0); // found or not
            }
          f1 = dwg_dynapi_subclass_field (name, key);
          if (f1 && *rest)
            {
              void *off = &((char *)_obj)[f1->offset];
              const char *subclass1 = dwg_dynapi_subclass_name (f1->type);
              const Dwg_DYNAPI_field *sfields1
                  = subclass1 ? dwg_dynapi_subclass_fields (subclass1) : NULL;
              if (!sfields1 && subclass1)
                sfields1 = dwg_dynapi_entity_fields (subclass1);
              if (!sfields1
                  || !_set_struct_field (dat, obj, tokens, off, subclass1,
                                         rest, sfields1))
                ++tokens->index;
              FREE ((char *)subclass1);
              return error | (f1->name ? 1 : 0); // found or not
            }
          else
            {
              // failed_key.rest.nextfieldatteept
              *(rest - 1) = '.'; // unsuccessful search, set the dot back
              rest = strchr (rest, '.');
              if (rest)
                {
                  LOG_HANDLE ("Try next embedded struct with %s.%s\n", key,
                              rest);
                }
              else
                {
                  LOG_HANDLE ("No embedded struct with %s\n", key);
                }
            }
        }
    }
  return error;
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
      // need to leave the handles, just FREE H*
      FREE (_obj->itemhandles);
      _obj->itemhandles = NULL;
      _obj->numitems = 0;
    }
  if ((_obj->numitems || do_free) && !_obj->itemhandles)
    {
      LOG_ERROR ("reset DICTIONARY, no itemhandles");
      for (BITCODE_BL i = 0; i < _obj->numitems; i++)
        FREE (_obj->texts[i]);
      FREE (_obj->texts);
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
      // need to leave the handles, just FREE H*
      FREE (_obj->itemhandles);
      _obj->itemhandles = NULL;
      _obj->numitems = 0;
    }
  if ((_obj->numitems || do_free) && !_obj->itemhandles)
    {
      LOG_ERROR ("reset DICTIONARYWDFLT, no itemhandles");
      for (BITCODE_BL i = 0; i < _obj->numitems; i++)
        FREE (_obj->texts[i]);
      FREE (_obj->texts);
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
  int i, size;
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
      dwg->object = (Dwg_Object *)CALLOC (rounded, sizeof (Dwg_Object));
    }
  else
    dwg_add_object (dwg);
  if (!dwg->object)
    {
      LOG_ERROR ("Out of memory");
      return DWG_ERR_OUTOFMEM;
    }
  if (dwg->header.from_version < R_13b1)
    {
      dwg->header.section
          = (Dwg_Section *)CALLOC (SECTION_VX + 1, sizeof (Dwg_Section));
    }
  dwg->num_objects += size;
  for (i = 0; i < size; i++)
    {
      char name[80];
      int keys;
      int is_entity = 0;
      Dwg_Object *obj = &dwg->object[i];
      Dwg_Object_APPID *_obj = NULL;
      const Dwg_DYNAPI_field *fields = NULL, *cfields;
      const Dwg_DYNAPI_field *f;

      memset (name, 0, sizeof (name));
      JSON_TOKENS_CHECK_OVERFLOW (goto harderr)
      if (i > 0)
        {
          Dwg_Object *oldobj = &dwg->object[i - 1];
          if (dwg->header.from_version >= R_13b1 && !oldobj->handle.value)
            {
              LOG_ERROR ("Required %s.handle missing, skipped", oldobj->name)
              dwg_free_object (oldobj);
              obj = oldobj;
              i--;
            }
          else if (!oldobj->type)
            {
              if (dwg->header.from_version >= R_13b1
                  || (oldobj->fixedtype != DWG_TYPE_BLOCK
                      && oldobj->fixedtype != DWG_TYPE_ENDBLK))
                LOG_ERROR ("Required %s.type missing, skipped", oldobj->name)
              if (!oldobj->parent)
                oldobj->parent = dwg;
              dwg_free_object (oldobj);
              obj = oldobj;
              i--;
              size--;
            }
          else if (oldobj->fixedtype == DWG_TYPE_UNUSED)
            {
              LOG_ERROR ("Required %s.fixedtype missing, skipped",
                         oldobj->name);
              if (!oldobj->parent)
                oldobj->parent = dwg;
              dwg_free_object (oldobj);
              obj = oldobj;
              i--;
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
          JSON_TOKENS_CHECK_OVERFLOW (goto typeerr)
        }
      keys = t->size;
      LOG_HANDLE ("\n-keys: %d, object %d of %d\n", keys, i, size);

      tokens->index++;
      for (int j = 0; j < keys; j++)
        {
          bool saw_dxfname = false;
          char key[80];
          memset (key, 0, sizeof (key));
          LOG_INSANE ("[%d] ", j);
          JSON_TOKENS_CHECK_OVERFLOW (goto harderr)
          json_fixed_key (key, dat, tokens);
          JSON_TOKENS_CHECK_OVERFLOW (goto harderr)
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
                      JSON_TOKENS_CHECK_OVERFLOW (goto harderr)
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
                  LOG_ERROR ("Unknown object %s (no fields)", name);
                  // skip_object:
                  obj->type = obj->fixedtype = DWG_TYPE_DUMMY;
                  // exhaust the rest
                  for (; j < keys; j++)
                    {
                      json_advance_unknown (dat, tokens, t->type, 0); // value
                      tokens->index++; // next key
                      JSON_TOKENS_CHECK_OVERFLOW (goto harderr)
                    }
                  tokens->index--;
                  break;
                }
              // crashing acad import, but dxfin might be okay
              /*
              if (is_class_unstable (name) &&
                  (strEQc (name, "TABLEGEOMETRY") ||
                   strEQc (name, "WIPEOUT")))
                {
                  LOG_ERROR ("Unhandled object %s", name);
                  goto skip_object;
                }
              */
              LOG_TRACE ("\nnew object %s [%d] (size: %d)\n", name, i,
                         objsize);
              obj->tio.object = (Dwg_Object_Object *)CALLOC (
                  1, sizeof (Dwg_Object_Object));
              obj->tio.object->dwg = dwg;
              obj->tio.object->objid = i;
              // NEW_OBJECT (dwg, obj)
              // ADD_OBJECT loop?
              _obj = (Dwg_Object_APPID *)CALLOC (1, objsize);
              obj->tio.object->tio.APPID = _obj;
              obj->tio.object->tio.APPID->parent = obj->tio.object;
              FREE (obj->name);
              obj->name = STRDUP (name);
              // TODO alias
              FREE (obj->dxfname);
              obj->dxfname = STRDUP (name);
              tokens->index++;
              JSON_TOKENS_CHECK_OVERFLOW (goto harderr)
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
                      JSON_TOKENS_CHECK_OVERFLOW (goto harderr)
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
                  LOG_ERROR ("Unknown entity %s (no fields)", name);
                  obj->type = obj->fixedtype = DWG_TYPE_DUMMY;
                  // exhaust the rest
                  for (; j < keys; j++)
                    {
                      json_advance_unknown (dat, tokens, t->type, 0); // value
                      tokens->index++; // next key
                      JSON_TOKENS_CHECK_OVERFLOW (goto harderr)
                    }
                  tokens->index--;
                  break;
                }
              LOG_TRACE ("\nnew entity %s [%d] (size: %d)\n", name, i,
                         objsize);
              obj->tio.entity = (Dwg_Object_Entity *)CALLOC (
                  1, sizeof (Dwg_Object_Entity));
              obj->tio.entity->dwg = dwg;
              obj->tio.entity->objid = i;
              // NEW_ENTITY (dwg, obj)
              // ADD_ENTITY loop?
              _obj = (Dwg_Object_APPID *)CALLOC (1, objsize);
              obj->tio.entity->tio.POINT = (Dwg_Entity_POINT *)_obj;
              obj->tio.entity->tio.POINT->parent = obj->tio.entity;
              FREE (obj->name);
              obj->name = STRDUP (name);
              // if different, the alias is done via extra dxfname key (below)
              FREE (obj->dxfname);
              obj->dxfname = STRDUP (name);
              tokens->index++;
              JSON_TOKENS_CHECK_OVERFLOW (goto harderr)
            }
          else if (!obj || !fields)
            {
              LOG_ERROR ("Required object or entity key missing");
              json_advance_unknown (dat, tokens, t->type, 0);
              JSON_TOKENS_CHECK_OVERFLOW (goto harderr)
            }
          else if (strEQc (key, "dxfname"))
            {
              FREE (obj->dxfname);
              saw_dxfname = true;
              obj->dxfname = json_string (dat, tokens);
              LOG_TRACE ("dxfname: %s\n", obj->dxfname)
              if (!obj->dxfname)
                obj->dxfname = STRDUP (name);

              // Some objects have various subtypes under one name.
              // TODO OBJECTCONTEXTDATA, ...
            }
          else if (strEQc (key, "index")
                   && strNE (name, "TableCellContent_Attr"))
            {
              BITCODE_RL index = json_long (dat, tokens);
              if (dat->from_version < R_13b1)
                {
                  if (index > 5)
                    index = obj->index; // we added 2 mspace blocks (type 0)
                                        // in-between
                }
              if (obj->index != index)
                LOG_WARN ("Ignore wrong %s.index %d, expected %d", name, index,
                          obj->index)
              LOG_TRACE ("index: %d\n", obj->index)
            }
          else if (strEQc (key, "type") && !obj->type)
            {
              int isent;
              const char *dxfname;
              obj->type = (BITCODE_BS)json_long (dat, tokens);
              JSON_TOKENS_CHECK_OVERFLOW (goto harderr)

              if (!dwg_object_name (name, &dxfname, &obj->fixedtype, &isent,
                                    NULL))
                {
                  LOG_ERROR ("Unknown object %s failed dwg_object_name()",
                             name);
                  // exhaust the rest
                  for (; j < keys; j++)
                    {
                      json_advance_unknown (dat, tokens, t->type, 0); // value
                      tokens->index++; // next key
                      JSON_TOKENS_CHECK_OVERFLOW (goto harderr)
                    }
                  tokens->index--;
                  break;
                }
              else
                {
                  if (obj->dxfname && strNE (obj->dxfname, dxfname))
                    {
                      if (memBEGINc (dxfname, "UNKNOWN_") || !saw_dxfname)
                        LOG_TRACE ("Changed dxfname %s => %s\n", obj->dxfname,
                                   dxfname)
                      else
                        LOG_WARN ("Changed wrong dxfname %s => %s",
                                  obj->dxfname, dxfname)
                    }
                  FREE (obj->dxfname);
                  obj->dxfname = STRDUP (dxfname);
                  if (obj->type <= DWG_TYPE_LAYOUT
                      && obj->fixedtype != obj->type
                      && dwg->header.from_version >= R_13b1)
                    {
                      LOG_WARN ("Changed wrong type %d => %d", obj->type,
                                obj->fixedtype)
                      obj->type = obj->fixedtype;
                    }
                  if ((obj->supertype == DWG_SUPERTYPE_ENTITY && !isent)
                      || (obj->supertype == DWG_SUPERTYPE_OBJECT && isent))
                    {
                      LOG_ERROR ("Illegal object supertype for %s", name);
                      // exhaust the rest
                      for (; j < keys; j++)
                        {
                          json_advance_unknown (dat, tokens, t->type,
                                                0); // value
                          tokens->index++;          // next key
                          JSON_TOKENS_CHECK_OVERFLOW (goto harderr);
                        }
                      tokens->index--;
                      break;
                    }
                }
              LOG_TRACE ("type: %d,\tfixedtype: %d\n", obj->type,
                         obj->fixedtype);
              if (dwg->header.from_version < R_13b1 && dwg_obj_is_table (obj))
                {
                  Dwg_Section_Type_r11 id = SECTION_HEADER_R11;
                  switch (obj->fixedtype)
                    {
                    case DWG_TYPE_BLOCK_HEADER:
                      id = SECTION_BLOCK;
                      break;
                    case DWG_TYPE_LAYER:
                      id = SECTION_LAYER;
                      break;
                    case DWG_TYPE_STYLE:
                      id = SECTION_STYLE;
                      break;
                    case DWG_TYPE_LTYPE:
                      id = SECTION_LTYPE;
                      break;
                    case DWG_TYPE_VIEW:
                      id = SECTION_VIEW;
                      break;
                    case DWG_TYPE_VPORT:
                      id = SECTION_VPORT;
                      break;
                    case DWG_TYPE_APPID:
                      id = SECTION_APPID;
                      break;
                    case DWG_TYPE_DIMSTYLE:
                      id = SECTION_DIMSTYLE;
                      break;
                    case DWG_TYPE_VX_TABLE_RECORD:
                      id = SECTION_VX;
                      break;
                    default:
                      LOG_ERROR ("Invalid table type %s %u", obj->name,
                                 obj->fixedtype);
                      // assert (!obj->fixedtype);
                    }
                  if (id != SECTION_HEADER_R11)
                    {
                      // TODO: maybe add a missing CONTROL object here. GH #453
                      dwg->header.section[id].number++;
                    }
                }
            }
          // Note: also _obj->size
          else if (strEQc (key, "size") && !obj->size
                   && t->type == JSMN_PRIMITIVE)
            {
              obj->size = json_long (dat, tokens);
              JSON_TOKENS_CHECK_OVERFLOW (goto harderr)
              if (dwg->header.from_version >= R_13b1 && !obj->handle.value)
                {
                  LOG_ERROR ("Required %s.handle missing", name);
                  goto harderr;
                }
              LOG_TRACE ("%s.size: %d\n", obj->name, obj->size)
            }
          else if (strEQc (key, "bitsize") && !obj->bitsize)
            {
              obj->bitsize = json_long (dat, tokens);
              LOG_TRACE ("%s.bitsize: %d\n", obj->name, obj->bitsize)
              JSON_TOKENS_CHECK_OVERFLOW (goto harderr)
            }
          else if (strEQc (key, "address") && !obj->address)
            {
              obj->address = json_long (dat, tokens);
              LOG_TRACE ("%s.address: 0x%zx\n", obj->name, obj->address)
              JSON_TOKENS_CHECK_OVERFLOW (goto harderr)
            }
          else if (strEQc (key, "handle") && !obj->handle.value)
            {
              BITCODE_H hdl
                  = json_HANDLE (dat, dwg, tokens, name, key, obj, -1);
              JSON_TOKENS_CHECK_OVERFLOW (goto harderr)
              if (hdl)
                {
                  obj->handle.code = hdl->handleref.code;
                  obj->handle.size = hdl->handleref.size;
                  obj->handle.value = hdl->handleref.value;
                  if (!hdl->handleref.code)
                    {
                      hdl->obj = obj;
                      dwg_add_handle (&hdl->handleref, 0, hdl->handleref.value,
                                      obj);
                    }
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
              LOG_TRACE ("_subclass: %.*s\n", t->end - t->start,
                         &dat->chain[t->start]);
              tokens->index++;
              JSON_TOKENS_CHECK_OVERFLOW (goto harderr)
            }
          else if (strEQc (key, "num_unknown_bits"))
            {
              obj->num_unknown_bits = json_long (dat, tokens);
              JSON_TOKENS_CHECK_OVERFLOW (goto harderr)
              LOG_TRACE ("num_unknown_bits: %d\n", (int)obj->num_unknown_bits);
            }
          else if (strEQc (key, "unknown_bits"))
            {
              const int len = t->end - t->start;
              char *hex = json_string (dat, tokens);
              const unsigned blen = len / 2;
              unsigned read;
              BITCODE_TF buf = (BITCODE_TF)MALLOC (blen + 1);
              if ((read = in_hex2bin (buf, hex, blen) != blen))
                LOG_ERROR ("in_hex2bin with key %s at pos %u of %u", key, read,
                           blen);
              buf[blen] = '\0';
              FREE (hex);
              if (!obj->num_unknown_bits)
                obj->num_unknown_bits = blen * 8; // minus some padding bits
              if (obj->unknown_bits)
                FREE (obj->unknown_bits);
              obj->unknown_bits = buf;
              // LOG_TRACE ("%s: '%.*s' [%s] (binary)\n", key, blen, buf,
              //            f->type);
              LOG_TRACE ("unknown_bits: %.*s\n", t->end - t->start,
                         &dat->chain[t->start])
            }
          else if (strEQc (key, "num_unknown_rest"))
            {
              obj->num_unknown_rest = json_long (dat, tokens);
              JSON_TOKENS_CHECK_OVERFLOW (goto harderr)
              LOG_TRACE ("num_unknown_rest: %d\n", (int)obj->num_unknown_rest);
            }
          else if (strEQc (key, "unknown_rest"))
            {
              const int len = t->end - t->start;
              char *hex = json_string (dat, tokens);
              const unsigned blen = len / 2;
              unsigned read;
              BITCODE_TF buf = (BITCODE_TF)MALLOC (blen + 1);
              if (hex)
                {
                  if ((read = in_hex2bin (buf, hex, blen) != blen))
                    LOG_ERROR ("in_hex2bin with key %s at pos %u of %u", key, read,
                               blen);
                  buf[blen] = '\0';
                  FREE (hex);
                }
              else
                {
                  memset (buf, 0, blen);
                }
              if (!obj->num_unknown_rest)
                obj->num_unknown_rest = blen * 8; // minus some padding bits
              if (obj->unknown_rest)
                FREE (obj->unknown_rest);
              obj->unknown_rest = buf;
              // LOG_TRACE ("%s: '%.*s' [%s] (binary)\n", key, blen, buf,
              //            f->type);
              LOG_TRACE ("unknown_rest: %.*s\n", t->end - t->start,
                         &dat->chain[t->start])
            }
          else if (strEQc (key, "eed")
                   // obj->tio.object shares a common prefix with entity until eed
                   && !obj->tio.object->num_eed && t->type == JSMN_ARRAY)
            {
              json_eed (dat, dwg, tokens, obj->tio.object);
              JSON_TOKENS_CHECK_OVERFLOW (goto harderr)
            }
          else
            // search_field:
            {
              if (t->type == JSMN_STRING && is_entity && strEQc (key, "tag"))
                {
                  int sz = t->end - t->start;
                  char *tag = (char *)MALLOC (sz + 1);
                  memcpy (tag, &dat->chain[t->start], sz);
                  tag[sz] = '\0';
                  if (sz <= 0 || !dwg_is_valid_tag (tag))
                    {
                      LOG_WARN ("Invalid %s.tag: %s\n", obj->name, tag);
                    }
                  FREE (tag);
                }
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

              // This should now be handled in _set_struct_field, recursively.
              // esp for TABLE's
              // first the MLEADER_AnnotContext union:
              if (strEQc (name, "MULTILEADER"))
                {
                  // assert (0);
                  // embedded structs
                  if (memBEGINc (key, "ctx.content.txt."))
                    {
                      Dwg_Entity_MULTILEADER *_o
                          = (Dwg_Entity_MULTILEADER *)_obj;
                      Dwg_MLEADER_Content *cnt = &_o->ctx.content;
                      const Dwg_DYNAPI_field *sf = dwg_dynapi_subclass_fields (
                          "MLEADER_Content_MText");
                      if (sf
                          && _set_struct_field (
                              dat, obj, tokens, cnt, "MLEADER_Content",
                              &key[strlen ("ctx.content.")], sf))
                        continue;
                    }
                  // the rest
                  else if (memBEGINc (key, "ctx."))
                    {
                      Dwg_Entity_MULTILEADER *_o
                          = (Dwg_Entity_MULTILEADER *)_obj;
                      Dwg_MLEADER_AnnotContext *ctx = &_o->ctx;
                      const Dwg_DYNAPI_field *sf = dwg_dynapi_subclass_fields (
                          "MLEADER_AnnotContext");
                      if (sf
                          && _set_struct_field (dat, obj, tokens, ctx,
                                                "MLEADER_AnnotContext",
                                                &key[4], sf))
                        continue;
                    }
                }
              else if (t->type == JSMN_OBJECT && memBEGINc (name, "DICTIONARY")
                       && strEQc (key, "items"))
                {
                  Dwg_Object_DICTIONARY *o = obj->tio.object->tio.DICTIONARY;
                  o->numitems = t->size;
                  o->texts = o->numitems ? (BITCODE_T *)CALLOC (
                                               o->numitems, sizeof (BITCODE_T))
                                         : NULL;
                  o->itemhandles
                      = o->numitems ? (BITCODE_H *)CALLOC (o->numitems,
                                                           sizeof (BITCODE_H))
                                    : NULL;
                  tokens->index++;
                  for (int k = 0; k < (int)o->numitems; k++)
                    {
                      JSON_TOKENS_CHECK_OVERFLOW (goto harderr);
                      t = &tokens->tokens[tokens->index];
                      /*SINCE (R_2007a)
                        o->texts[k] = (BITCODE_T)json_wstring (dat, tokens);
                      else*/
                      o->texts[k] = json_string (dat, tokens);
                      LOG_TRACE ("texts[%d]: %.*s\t => ", k, t->end - t->start,
                                 &dat->chain[t->start]);
                      JSON_TOKENS_CHECK_OVERFLOW (goto harderr);
                      o->itemhandles[k] = json_HANDLE (dat, dwg, tokens, name,
                                                       "itemhandles", obj, k);
                    }
                  if (!o->numitems)
                    LOG_TRACE ("%s.%s empty\n", name, key);
                  continue;
                }
              LOG_ERROR ("Unknown %s.%s %.*s ignored", name, key,
                         t->end - t->start, &dat->chain[t->start]);
              json_advance_unknown (dat, tokens, t->type, 0);
              JSON_TOKENS_CHECK_OVERFLOW (goto harderr)
            }
        }
    }
  LOG_TRACE ("End of %s\n", section)
  tokens->index--;
  return 0;
harderr:
  dwg->num_objects = i;
  LOG_TRACE ("End of %s (hard error)\n", section)
  tokens->index--;
  return DWG_ERR_INVALIDDWG;
typeerr:
  dwg->num_objects = i;
  LOG_TRACE ("End of %s (type error)\n", section)
  tokens->index--;
  return DWG_ERR_INVALIDTYPE;
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
  // struct { uint32_t hdloff; int32_t offset } *omap = CALLOC (size, 8);
  for (int i = 0; i < size; i++)
    {
      tokens->index++;
      JSON_TOKENS_CHECK_OVERFLOW_ERR
      for (int k = 0; k < 2; k++)
        {
          uint32_t hdloff = json_long (dat, tokens);
          uint32_t offset = json_long (dat, tokens);
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
  uint32_t size1 = 0;
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
          size_t len;
          dwg->thumbnail.chain = json_binary (dat, tokens, key, &len);
          JSON_TOKENS_CHECK_OVERFLOW_ERR
          dwg->thumbnail.size = len;
          if (size1 > 0 && size1 != (uint32_t)len)
            LOG_WARN ("thumbnail size mismatch: binary len %" PRIuSIZE
                      " != size " FORMAT_BL,
                      len, size1);
          LOG_TRACE ("size: %" PRIuSIZE "\n", len);
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
json_R2007_Header (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                   jsmntokens_t *restrict tokens)
{
  const char *section = "R2007_Header";
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  Dwg_R2007_Header *_obj = &dwg->fhdr.r2007_file_header;
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
      FIELD_RLL (header_size, 0)
      FIELD_RLL (file_size, 0)
      FIELD_RLL (pages_map_crc_compressed, 0)
      FIELD_RLL (pages_map_correction, 0)
      FIELD_RLL (pages_map_crc_seed, 0)
      FIELD_RLL (pages_map2_offset, 0)
      FIELD_RLL (pages_map2_id, 0)
      FIELD_RLL (pages_map_offset, 0)
      FIELD_RLL (pages_map_id, 0)
      FIELD_RLL (header2_offset, 0)
      FIELD_RLL (pages_map_size_comp, 0)
      FIELD_RLL (pages_map_size_uncomp, 0)
      FIELD_RLL (pages_amount, 0)
      FIELD_RLL (pages_maxid, 0)
      FIELD_RLL (unknown1, 0)
      FIELD_RLL (unknown2, 0)
      FIELD_RLL (pages_map_crc_uncomp, 0)
      FIELD_RLL (unknown3, 0)
      FIELD_RLL (unknown4, 0)
      FIELD_RLL (unknown5, 0)
      FIELD_RLL (num_sections, 0)
      FIELD_RLL (sections_map_crc_uncomp, 0)
      FIELD_RLL (sections_map_size_comp, 0)
      FIELD_RLL (sections_map2_id, 0)
      FIELD_RLL (sections_map_id, 0)
      FIELD_RLL (sections_map_size_uncomp, 0)
      FIELD_RLL (sections_map_crc_comp, 0)
      FIELD_RLL (sections_map_correction, 0)
      FIELD_RLL (sections_map_crc_seed, 0)
      FIELD_RLL (stream_version, 0)
      FIELD_RLL (crc_seed, 0)
      FIELD_RLL (crc_seed_encoded, 0)
      FIELD_RLL (random_seed, 0)
      FIELD_RLL (header_crc, 0)
      // clang-format on
      // end of encrypted header
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
json_R2004_Header (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                   jsmntokens_t *restrict tokens)
{
  const char *section = "R2004_Header";
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  Dwg_R2004_Header *_obj = &dwg->fhdr.r2004_header;
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
          char *s = json_fixed_string (dat, 11, tokens);
          JSON_TOKENS_CHECK_OVERFLOW_ERR
          if (s)
            memcpy (&_obj->file_ID_string, s, 12);
          else
            {
              LOG_ERROR ("Invalid R2004_Header.file_ID_string")
              memcpy (&_obj->file_ID_string, "AcFssFcAJMB\0", 12);
            }
          LOG_TRACE ("file_ID_string: \"%.*s\"\n", 12, _obj->file_ID_string)
          FREE (s);
          s = NULL;
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
      FIELD_RLL (secondheader_address, 0)
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
  Dwg_AuxHeader *_obj = &dwg->auxheader;
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
      FIELD_RL (maint_version, 0)
      FIELD_RL (numsaves, 0)
      FIELD_RL (minus_1, 0)
      FIELD_RS (numsaves_1, 0)
      FIELD_RS (numsaves_2, 0)
      FIELD_RL (zero, 0)
      FIELD_RSx (dwg_version_1, 0)
      FIELD_RL (maint_version_1, 0)
      FIELD_RSx (dwg_version_2, 0)
      FIELD_RL (maint_version_2, 0)
      FIELD_VECTOR_INL (unknown_6rs, RS, 6, 0)
      FIELD_VECTOR_INL (unknown_5rl, RL, 5, 0)
      FIELD_TIMERLL (TDCREATE, 0)
      FIELD_TIMERLL (TDUPDATE, 0)
      FIELD_HV (HANDSEED, 0)
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
json_SecondHeader_Sections (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                            jsmntokens_t *restrict tokens,
                            Dwg_SecondHeader *_obj, int size)
{
  const char *section = "SecondHeader_Sections";
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  if (t->type != JSMN_ARRAY)
    {
      LOG_ERROR ("Unexpected %s at %u of %ld tokens, expected %s ARRAY",
                 t_typename[t->type], tokens->index, tokens->num_tokens,
                 section);
      json_advance_unknown (dat, tokens, t->type, 0);
      return DWG_ERR_INVALIDTYPE;
    }
  _obj->num_sections = MIN (size, 6);
  LOG_TRACE ("%s: %d\n", section, _obj->num_sections);
  for (int j = 0; j < _obj->num_sections; j++)
    {
      int keys;
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
      for (int k = 0; k < MIN (keys, 3); k++)
        {
          char key[80];
          JSON_TOKENS_CHECK_OVERFLOW_ERR
          json_fixed_key (key, dat, tokens);
          t = &tokens->tokens[tokens->index];
          // clang-format off
          if (0) ; // else
          SUB_FIELD_LONG (sections[j], nr, RCd)
          SUB_FIELD_LONG (sections[j], address, BLx)
          SUB_FIELD_LONG (sections[j], size, BL)
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
json_SecondHeader_Handles (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                           jsmntokens_t *restrict tokens,
                           Dwg_SecondHeader *_obj, int size)
{
  const char *section = "SecondHeader_Handles";
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  if (t->type != JSMN_ARRAY)
    {
      LOG_ERROR ("Unexpected %s at %u of %ld tokens, expected %s ARRAY",
                 t_typename[t->type], tokens->index, tokens->num_tokens,
                 section);
      json_advance_unknown (dat, tokens, t->type, 0);
      return DWG_ERR_INVALIDTYPE;
    }
  _obj->num_handles = MIN (size, 14);
  LOG_TRACE ("%s: %d\n", section, _obj->num_handles);
  for (int j = 0; j < _obj->num_handles; j++)
    {
      int keys;
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
      for (int k = 0; k < MIN (keys, 3); k++)
        {
          char key[80];
          JSON_TOKENS_CHECK_OVERFLOW_ERR
          json_fixed_key (key, dat, tokens);
          t = &tokens->tokens[tokens->index];
          // clang-format off
          if (0) ; // else
          SUB_FIELD_LONG (handles[j], num_hdl, RCd) // disabled
          SUB_FIELD_LONG (handles[j], nr, RCd)
          // SUB_FIELD_VECTOR_INL (handles[j], hdl, RC, num_hdl, 0)
          else if (strEQc (key, "hdl") && t->type == JSMN_ARRAY)
            {
              tokens->index++;
              JSON_TOKENS_CHECK_OVERFLOW_ERR
              if (t->size < 8)
                _obj->handles[j].num_hdl = t->size;
              for (int vcount = 0; vcount < t->size; vcount++)
                {
                  if (vcount >= 8)
                    {
                      tokens->index++;
                      continue;
                    }
                  _obj->handles[j].hdl[vcount] =
                    (BITCODE_RC)json_long (dat, tokens) & 0xFF;
                  JSON_TOKENS_CHECK_OVERFLOW_ERR
                  LOG_TRACE ("hdl[%d]: " FORMAT_RC " [RC %d]\n", vcount,
                               _obj->handles[j].hdl[vcount], 0);
                }
            }
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
json_SecondHeader (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                   jsmntokens_t *restrict tokens)
{
  const char *section = "SecondHeader";
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  Dwg_SecondHeader *_obj = &dwg->secondheader;
  int size, size1, error = 0;
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

      if (strEQc (key, "handles"))
        {
          if (t->type != JSMN_ARRAY) // of OBJECTs
            json_advance_unknown (dat, tokens, t->type, 0);
          else if (t->size)
            error
                |= json_SecondHeader_Handles (dat, dwg, tokens, _obj, t->size);
          else
            tokens->index++; // empty array
          if (error >= DWG_ERR_CRITICAL)
            return error;
        }
      else if (strEQc (key, "sections"))
        {
          if (t->type != JSMN_ARRAY) // of OBJECTs
            json_advance_unknown (dat, tokens, t->type, 0);
          else if (t->size)
            error |= json_SecondHeader_Sections (dat, dwg, tokens, _obj,
                                                 t->size);
          else
            tokens->index++; // empty array
          if (error >= DWG_ERR_CRITICAL)
            return error;
        }
      // clang-format off
      FIELD_RL (size, 0)
      FIELD_RL (address, 0)
      FIELD_TFF (version, 11, 0)
      FIELD_RC (is_maint, 0)
      FIELD_RC (zero_one_or_three, 0)
      FIELD_BS (dwg_versions, 0)
      FIELD_RS (codepage, 0)
      FIELD_BS (num_sections, 0)
      FIELD_BS (num_handles, 0)
      FIELD_RLL (junk_r14, 0)
      FIELD_RS (crc, 0)
      // clang-format on
      else
      {
        LOG_ERROR ("Unknown %s.%s ignored", section, key);
        tokens->index++;
      }
    }

  LOG_TRACE ("End of %s\n", section)
  tokens->index--;
  return error;
}

static int
json_SummaryInfo (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                  jsmntokens_t *restrict tokens)
{
  const char *section = "SummaryInfo";
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  Dwg_SummaryInfo *_obj = &dwg->summaryinfo;
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
          _obj->props = (Dwg_SummaryInfo_Property *)CALLOC (
              size1, sizeof (Dwg_SummaryInfo_Property));
          _obj->num_props = size1;
          tokens->index++;
          for (int j = 0; j < size1; j++)
            {
              JSON_TOKENS_CHECK_OVERFLOW_ERR
              t = &tokens->tokens[tokens->index];
              if (t->type != JSMN_OBJECT)
                {
                  LOG_ERROR ("Unexpected %s at %u of %ld tokens, expected "
                             "%s.%s OBJECT",
                             t_typename[t->type], tokens->index,
                             tokens->num_tokens, section, key);
                  json_advance_unknown (dat, tokens, t->type, 0);
                  return DWG_ERR_INVALIDTYPE;
                }
              tokens->index++; // OBJECT of 2: tag, value. TODO: array of 2
              json_fixed_key (key, dat, tokens);
              JSON_TOKENS_CHECK_OVERFLOW_ERR
              t = &tokens->tokens[tokens->index];
              if (t->type == JSMN_STRING) // CUSTOMPROPERTYTAG
                {
                  _obj->props[j].tag = json_string (dat, tokens);
                  LOG_TRACE ("props[%d] = (%.*s", j, t->end - t->start,
                             &dat->chain[t->start])
                }
              else if (t->type == JSMN_PRIMITIVE)
                tokens->index++;
              else
                json_advance_unknown (dat, tokens, t->type, 0);

              json_fixed_key (key, dat, tokens);
              JSON_TOKENS_CHECK_OVERFLOW_ERR
              t = &tokens->tokens[tokens->index];
              if (t->type == JSMN_STRING) // CUSTOMPROPERTY
                {
                  _obj->props[j].value = json_string (dat, tokens);
                  LOG_TRACE (",%.*s)", t->end - t->start,
                             &dat->chain[t->start])
                }
              else if (t->type == JSMN_PRIMITIVE)
                tokens->index++;
              else
                json_advance_unknown (dat, tokens, t->type, 0);
              LOG_TRACE ("\n")
            }
        }
      // clang-format off
      FIELD_T16 (TITLE, 1)
      FIELD_T16 (SUBJECT, 1)
      FIELD_T16 (AUTHOR, 1)
      FIELD_T16 (KEYWORDS, 1)
      FIELD_T16 (COMMENTS, 1)
      FIELD_T16 (LASTSAVEDBY, 1)
      FIELD_T16 (REVISIONNUMBER, 1)
      FIELD_T16 (HYPERLINKBASE, 1)
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
json_VBAProject (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                 jsmntokens_t *restrict tokens)
{
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  memset (&dwg->vbaproject, 0, sizeof (Dwg_VBAProject));
  LOG_WARN ("Ignore VBAProject");
  json_advance_unknown (dat, tokens, t->type, 0);
  return DWG_ERR_INVALIDTYPE;
}

static int
json_AppInfo (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
              jsmntokens_t *restrict tokens)
{
  const char *section = "AppInfo";
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  Dwg_AppInfo *_obj = &dwg->appinfo;
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
      // LOG_TRACE ("%s\n", key)
      t = &tokens->tokens[tokens->index];

      // clang-format off
      if (0) ;
      FIELD_RL (class_version, 0)
      FIELD_T16 (appinfo_name, 0)
      FIELD_TFFx (version_checksum, 16, 0)
      FIELD_T16 (version, 0)
      FIELD_TFFx (comment_checksum, 16, 0)
      FIELD_T16 (comment, 0)
      FIELD_TFFx (product_checksum, 16, 0)
      FIELD_T16 (product_info, 0)
      else
        {
          LOG_ERROR ("Unknown %s.%s ignored", section, key);
          json_advance_unknown (dat, tokens, t->type, 0);
        }
      // clang-format on
    }

  _obj->num_strings = 3;
  LOG_TRACE ("num_strings => 3\n");
  LOG_TRACE ("End of %s\n", section);
  tokens->index--;
  return 0;
}

static int
json_AppInfoHistory (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                     jsmntokens_t *restrict tokens)
{
  const char *section = "AppInfoHistory";
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  Dwg_AppInfoHistory *_obj = &dwg->appinfohistory;
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
      // LOG_TRACE ("%s\n", key)
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
                        jsmntokens_t *restrict tokens, Dwg_FileDepList *o,
                        int size)
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
  o->files
      = (Dwg_FileDepList_Files *)CALLOC (size, sizeof (Dwg_FileDepList_Files));
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
  Dwg_FileDepList *_obj = &dwg->filedeplist;
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
              _obj->features
                  = (BITCODE_TV *)CALLOC (size1, sizeof (BITCODE_TV));
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
  Dwg_Security *_obj = &dwg->security;
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
  Dwg_RevHistory *_obj = &dwg->revhistory;
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
  Dwg_ObjFreeSpace *_obj = &dwg->objfreespace;
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
      FIELD_RLL (numhandles, 0)
      FIELD_TIMERLL (TDUPDATE, 0)
      FIELD_RC (numnums, 0)
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
                        jsmntokens_t *restrict tokens, Dwg_AcDs *o, int size)
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
  o->segidx
      = (Dwg_AcDs_SegmentIndex *)CALLOC (size, sizeof (Dwg_AcDs_SegmentIndex));
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
                    jsmntokens_t *restrict tokens, Dwg_AcDs *o, int size)
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
  o->segments = (Dwg_AcDs_Segment *)CALLOC (size, sizeof (Dwg_AcDs_Segment));
  // o->num_segidx = size;
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
              o->datidx.di_unknown = json_long (dat, tokens);
              LOG_TRACE ("datidx.di_unknown_1: " FORMAT_RL "\n", o->datidx.di_unknown);
            }
          else if (strEQc (key, "si_unknown_1"))
            {
              o->schidx.si_unknown_1 = json_long (dat, tokens);
              LOG_TRACE ("schidx.unknown_1: " FORMAT_RL "\n", o->schidx.si_unknown_1);
            }
          else if (strEQc (key, "si_unknown_2"))
            {
              o->schidx.si_unknown_2 = json_long (dat, tokens);
              LOG_TRACE ("schidx.unknown_2: " FORMAT_RL "\n", o->schidx.si_unknown_2);
            }
          else if (strEQc (key, "si_tag"))
            {
              o->schidx.si_tag = (BITCODE_RLL)json_longlong (dat, tokens);
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
  Dwg_AcDs *_obj = &dwg->acds;
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
        // if (error >= DWG_ERR_CRITICAL)
        //   return error;
      }
      else if (strEQc (key, "segments"))
      {
        if (t->type != JSMN_ARRAY) // of OBJECTs
          json_advance_unknown (dat, tokens, t->type, 0);
        else if (t->size)
          error |= json_AcDs_Segments (dat, dwg, tokens, _obj, t->size);
        else
          tokens->index++; // empty array
        // if (error >= DWG_ERR_CRITICAL)
        //   return error;
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
  Dwg_Template *_obj = &dwg->Template;
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
      FIELD_T (description, 0)
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

static void
json_free_globals (jsmntokens_t *tokens)
{
  if (tokens)
    FREE (tokens->tokens);
  FREE (created_by);
}

EXPORT int
dwg_read_json (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  Dwg_Header *obj = &dwg->header;
  jsmn_parser parser;
  jsmntokens_t tokens;
  unsigned int i;
  int error = -1;
  created_by = NULL;

  dwg->opts |= (loglevel | DWG_OPTS_INJSON);
  dat->opts |= (loglevel | DWG_OPTS_INJSON);
  loglevel = dwg->opts & 0xf;
  if (dat->fh && (!dat->chain || !*dat->chain))
    {
      error = dat_read_stream (dat, dat->fh);
      if (error >= DWG_ERR_CRITICAL)
        return error;
      LOG_TRACE ("  json file size: %" PRIuSIZE "\n", dat->size);
    }
  g_dat = dat;

  jsmn_init (&parser);
  // How big will it be? This is the max. memory variant.
  // we could also use less, see jsmn/examples/jsondump.c for small devices.
  tokens.num_tokens
      = jsmn_parse (&parser, (char *)dat->chain, dat->size, NULL, 0);
  if (tokens.num_tokens <= 0)
    {
      const long remaining = (long)(dat->size - parser.pos);
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
                     parser.pos, (int)remaining, &dat->chain[parser.pos]);
        }
      return DWG_ERR_INVALIDDWG;
    }
  LOG_TRACE ("  num_tokens: %ld\n", tokens.num_tokens);
  tokens.tokens
      = (jsmntok_t *)CALLOC (tokens.num_tokens + 1024, sizeof (jsmntok_t));
  if (!tokens.tokens)
    return DWG_ERR_OUTOFMEM;

  dwg->object_map = hash_new (tokens.num_tokens / 50);
  if (!dwg->object_map) // we are obviously on a tiny system
    {
      dwg->object_map = hash_new (1024);
      if (!dwg->object_map)
        {
          LOG_ERROR ("Out of memory");
          json_free_globals (&tokens);
          return DWG_ERR_OUTOFMEM;
        }
    }
  dwg->dirty_refs = 1;
  // set the target version earlier (than e.g. in DXF)
  // we cannot write DWG >= R_2004 yet. avoid widestrings for now
  /* if (!dwg->header.version)
    dwg->header.version = dat->version = R_2000;
  */
  dat->version = R_2000;

  jsmn_init (&parser); // reset pos to 0
  error = jsmn_parse (&parser, (char *)dat->chain, dat->size, tokens.tokens,
                      (unsigned int)tokens.num_tokens);
  if (error < 0)
    {
      const long remaining = (long)(dat->size - parser.pos);
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
                     error, parser.toknext, parser.pos, (int)remaining,
                     &dat->chain[parser.pos]);
        }
      json_free_globals (&tokens);
      return DWG_ERR_INVALIDDWG;
    }

  if (tokens.tokens[0].type != JSMN_OBJECT)
    {
      fprintf (stderr, "First JSON element is not an object/hash\n");
      json_free_globals (&tokens);
      return DWG_ERR_INVALIDDWG;
    }

  // valid first level tokens:
  // created_by: string
  // section objects: FILEHEADER, HEADER, THUMBNAILIMAGE, R2004_Header,
  //                  SummaryInfo, AppInfo,
  //                  AppInfoHistory, FileDepList, Security, RevHistory,
  //                  ObjFreeSpace, Template,
  //                  AuxHeader, SecondHeader
  // section arrays: CLASSES, OBJECTS, HANDLES
  error = 0;
  for (tokens.index = 1; tokens.index < (unsigned int)tokens.num_tokens;
       tokens.index++)
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
          json_free_globals (&tokens);
          return DWG_ERR_INVALIDDWG;
        }
      if (len >= 80)
        {
          LOG_ERROR ("Unknown JSON key at %u of %ld tokens, len %d > 80",
                     tokens.index, tokens.num_tokens, len);
          json_free_globals (&tokens);
          return DWG_ERR_INVALIDDWG;
        }
      memcpy (key, &dat->chain[t->start], len);
      key[len] = '\0';
      tokens.index++;
      if (tokens.index > (unsigned int)tokens.num_tokens)
        {
          LOG_ERROR ("Unexpected end of JSON at %u of %ld tokens %s:%d",
                     tokens.index, tokens.num_tokens, __FILE__, __LINE__);
          json_free_globals (&tokens);
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
      else if (strEQc (key, "SecondHeader"))
        error |= json_SecondHeader (dat, dwg, &tokens);
      else if (strEQc (key, "R2004_Header"))
        error |= json_R2004_Header (dat, dwg, &tokens);
      else if (strEQc (key, "R2007_Header"))
        error |= json_R2007_Header (dat, dwg, &tokens);
      else if (strEQc (key, "SummaryInfo"))
        error |= json_SummaryInfo (dat, dwg, &tokens);
      else if (strEQc (key, "VBAProject"))
        error |= json_VBAProject (dat, dwg, &tokens);
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
          json_free_globals (&tokens);
          return error | DWG_ERR_INVALIDTYPE;
        }
      if (error >= DWG_ERR_CRITICAL)
        {
          LOG_TRACE ("\n")
          json_free_globals (&tokens);
          return error;
        }
    }

  LOG_TRACE ("\n")
  if (dat->version <= R_2000 && dwg->header.from_version > R_2000)
    dwg_fixup_BLOCKS_entities (dwg);

  json_free_globals (&tokens);
  created_by = NULL;
  return error;
}

#undef IS_ENCODER
#undef IS_JSON
