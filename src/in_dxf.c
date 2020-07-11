/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2018-2025 Free Software Foundation, Inc.                   */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * in_dxf.c: read ascii and binary DXF
 * written by Reini Urban
 */

#include "config.h"
#ifdef __STDC_ALLOC_LIB__
#  define __STDC_WANT_LIB_EXT2__ 1 /* for strdup */
#else
#  define _USE_BSD 1
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <errno.h>
// #include <ctype.h>
#include <math.h>
// strings.h or string.h
#ifdef AX_STRCASECMP_HEADER
#  include AX_STRCASECMP_HEADER
#endif

#define IS_INDXF
#include "common.h"
#include "importer.h"
#include "codepages.h"
#include "bits.h"
#include "dwg.h"
#include "out_dxf.h"
#include "decode.h"
#include "encode.h"
#include "dynapi.h"
#include "hash.h"
#include "classes.h"
#include "free.h"

static unsigned int loglevel;
#define DWG_LOGLEVEL loglevel
#include "logging.h"

#include "in_dxf.h"

#ifndef _DWG_API_H_
// dwg_api.h is too big
Dwg_Object *dwg_obj_generic_to_object (const void *restrict obj,
                                       int *restrict error);
#endif
static void
dxf_set_DWGCODEPAGE (Dwg_Data *dwg);
// from dwg_api.c
Dwg_Object_DICTIONARY *
dwg_add_DICTIONARY (Dwg_Data *restrict dwg,
                    const char *restrict name, /* the NOD entry */
                    const char *restrict text, /* maybe NULL */
                    const BITCODE_RLL absolute_ref);
Dwg_Object_VX_TABLE_RECORD *
dwg_add_VX (Dwg_Data *restrict dwg, const char *restrict name /* maybe NULL */);
// from dwg.c
BITCODE_RLL
dwg_new_handseed (Dwg_Data *restrict dwg);
BITCODE_H
dwg_find_tablehandle_silent (Dwg_Data *restrict dwg, const char *restrict name,
                             const char *restrict table);

/* the current version per spec block */
static unsigned int cur_ver = 0;
static char buf[4096];
static long start, end; // stream offsets
static array_hdls *header_hdls = NULL;
static array_hdls *eed_hdls = NULL;
static array_hdls *obj_hdls = NULL;

// static long num_dxf_objs;  // how many elements are added
// static long size_dxf_objs; // how many elements are allocated
// static Dxf_Objs *dxf_objs;

#define EXPECT_DXF(nam, field, dxf)                                           \
  if (pair == NULL || pair->code != dxf)                                      \
    {                                                                         \
      LOG_ERROR ("%s: Unexpected DXF code %d, expected %d for %s", nam,       \
                 pair ? pair->code : -1, dxf, #field);                        \
      return pair;                                                            \
    }
#define EXPECT_INT_DXF(field, dxf, _type)                                     \
  EXPECT_DXF (obj->name, #field, dxf);                                        \
  dwg_dynapi_entity_set_value (o, obj->name, field, &pair->value, 1);         \
  LOG_TRACE ("%s.%s = %d [" #_type " %d]\n", obj->name, field, pair->value.i, \
             pair->code);                                                     \
  dxf_free_pair (pair);                                                       \
  pair = NULL;
#define EXPECT_UINT_DXF(field, dxf, _type)                                    \
  EXPECT_DXF (obj->name, #field, dxf);                                        \
  if (pair->value.l < 0)                                                      \
    {                                                                         \
      LOG_ERROR ("%s: Unexpected DXF value %ld for %s code %d", obj->name,    \
                 pair ? pair->value.l : 0, #field, dxf);                      \
      return pair;                                                            \
    }                                                                         \
  dwg_dynapi_entity_set_value (o, obj->name, field, &pair->value, 1);         \
  LOG_TRACE ("%s.%s = %d [" #_type " %d]\n", obj->name, field, pair->value.i, \
             pair->code);                                                     \
  dxf_free_pair (pair);                                                       \
  pair = NULL
#define EXPECT_DBL_DXF(field, dxf, _type)                                     \
  EXPECT_DXF (obj->name, #field, dxf);                                        \
  dwg_dynapi_entity_set_value (o, obj->name, field, &pair->value, 1);         \
  LOG_TRACE ("%s.%s = %f [" #_type " %d]\n", obj->name, field, pair->value.d, \
             pair->code);                                                     \
  dxf_free_pair (pair);                                                       \
  pair = NULL
#define EXPECT_H_DXF(field, htype, dxf, _type)                                \
  EXPECT_DXF (obj->name, #field, dxf);                                        \
  if (pair->value.u)                                                          \
    {                                                                         \
      BITCODE_H hdl = dwg_add_handleref (dwg, htype, pair->value.u, obj);     \
      dwg_dynapi_entity_set_value (o, obj->name, field, &hdl, 1);             \
      LOG_TRACE ("%s.%s = " FORMAT_REF " [H %d]\n", obj->name, field,         \
                 ARGS_REF (hdl), pair->code);                                 \
    }                                                                         \
  dxf_free_pair (pair);                                                       \
  pair = NULL
#define EXPECT_T_DXF(field, dxf)                                              \
  EXPECT_DXF (obj->name, #field, dxf);                                        \
  if (pair->value.s)                                                          \
    {                                                                         \
      dwg_dynapi_entity_set_value (o, obj->name, field, &pair->value.s, 1);   \
      LOG_TRACE ("%s.%s = \"%s\" [T %d]\n", obj->name, field, pair->value.s,  \
                 pair->code);                                                 \
    }                                                                         \
  dxf_free_pair (pair);                                                       \
  pair = NULL

#define SUB_FIELD_VALUE(sub, nam) o->sub.nam
#define EXPECT_SUB_INT_DXF(sub, field, dxf, _type)                            \
  EXPECT_DXF (obj->name, field, dxf);                                         \
  dwg_dynapi_subclass_set_value (dwg, o, f->type, field, &pair->value, 1);    \
  LOG_TRACE ("%s.%s.%s = %d [" #_type " %d]\n", obj->name, sub, field,        \
             pair->value.i, pair->code);                                      \
  dxf_free_pair (pair);                                                       \
  pair = NULL;
#define EXPECT_SUB_UINT_DXF(sub, field, dxf, _type)                           \
  EXPECT_DXF (obj->name, field, dxf);                                         \
  if (pair->value.l < 0)                                                      \
    {                                                                         \
      LOG_ERROR ("%s: Unexpected DXF value %ld for %s.%s code %d", obj->name, \
                 pair ? pair->value.l : 0, sub, field, dxf);                  \
      return pair;                                                            \
    }                                                                         \
  dwg_dynapi_subclass_set_value (dwg, o, #_type, field, &pair->value, 1);     \
  LOG_TRACE ("%s.%s.%s = %ld [" #_type " %d]\n", obj->name, sub, field,       \
             pair->value.l, pair->code);                                      \
  dxf_free_pair (pair);                                                       \
  pair = NULL;
#define EXPECT_SUB_DBL_DXF(sub, field, dxf, _type)                            \
  EXPECT_DXF (obj->name, field, dxf);                                         \
  dwg_dynapi_subclass_set_value (dwg, o, #_type, #field, &pair->value, 1);     \
  LOG_TRACE ("%s.%s.%s = %g [%s %d]\n", obj->name, sub, field,                \
             pair->value.d, #_type, pair->code);                              \
  dxf_free_pair (pair);                                                       \
  pair = NULL;
#define EXPECT_SUB_H_DXF(sub, field, htype, dxf, _type)                       \
  EXPECT_DXF (obj->name, field, dxf);                                         \
  if (pair->value.u)                                                          \
    {                                                                         \
      BITCODE_H hdl = dwg_add_handleref (dwg, htype, pair->value.u, obj);     \
      dwg_dynapi_subclass_set_value (dwg, o, f->type, field, &hdl, 1);        \
      LOG_TRACE ("%s.%s.%s = " FORMAT_REF " [H %d]\n", obj->name, sub, field, \
                 ARGS_REF (hdl), pair->code);                                 \
    }                                                                         \
  dxf_free_pair (pair);                                                       \
  pair = NULL
#define EXPECT_SUB_T_DXF(sub, field, dxf, _type)                              \
  EXPECT_DXF (obj->name, field, dxf);                                         \
  if (pair->value.s)                                                          \
    {                                                                         \
      dwg_dynapi_subclass_set_value (dwg, o, _type, field, &pair->value.s,    \
                                     1);                                      \
      LOG_TRACE ("%s.%s.%s = \"%s\" [T %d]\n", obj->name, sub, field,         \
                 pair->value.s, pair->code);                                  \
    }                                                                         \
  dxf_free_pair (pair);                                                       \
  pair = NULL

// stricter ordering for special subclasses:
#define FIELD_B(field, dxf)                                                   \
  if (dxf)                                                                    \
    {                                                                         \
      pair = dxf_read_pair (dat);                                             \
      EXPECT_INT_DXF (#field, dxf, B);                                        \
    }
#define SUB_FIELD_B(sub, field, dxf)                                          \
  if (dxf)                                                                    \
    {                                                                         \
      pair = dxf_read_pair (dat);                                             \
      EXPECT_SUB_INT_DXF (#sub, #field, dxf, B);                              \
    }
#define FIELD_RC(field, dxf)                                                  \
  if (dxf)                                                                    \
    {                                                                         \
      pair = dxf_read_pair (dat);                                             \
      EXPECT_INT_DXF (#field, dxf, RC);                                       \
    }
#define FIELD_BS(field, dxf)                                                  \
  if (dxf)                                                                    \
    {                                                                         \
      pair = dxf_read_pair (dat);                                             \
      EXPECT_INT_DXF (#field, dxf, BS);                                       \
    }
#define FIELD_BLd(field, dxf)                                                 \
  if (dxf)                                                                    \
    {                                                                         \
      pair = dxf_read_pair (dat);                                             \
      EXPECT_INT_DXF (#field, dxf, BLd);                                      \
    }
#define SUB_FIELD_BLd(sub, field, dxf)                                        \
  if (dxf)                                                                    \
    {                                                                         \
      pair = dxf_read_pair (dat);                                             \
      EXPECT_SUB_INT_DXF (#sub, #field, dxf, BLd);                            \
    }
#define FIELD_BL(field, dxf)                                                  \
  if (dxf)                                                                    \
    {                                                                         \
      pair = dxf_read_pair (dat);                                             \
      EXPECT_UINT_DXF (#field, dxf, BL);                                      \
    }
#define SUB_FIELD_BL(sub, field, dxf)                                         \
  if (dxf)                                                                    \
    {                                                                         \
      pair = dxf_read_pair (dat);                                             \
      EXPECT_SUB_UINT_DXF (#sub, #field, dxf, BL);                            \
    }
#define FIELD_BD(field, dxf)                                                  \
  if (dxf)                                                                    \
    {                                                                         \
      pair = dxf_read_pair (dat);                                             \
      EXPECT_DBL_DXF (#field, dxf, BD);                                       \
    }
#define FIELD_3BD(field, dxf)                                                 \
  if (dxf)                                                                    \
    {                                                                         \
      BITCODE_3BD pt;                                                         \
      pair = dxf_read_pair (dat);                                             \
      EXPECT_DXF (obj->name, #field, dxf);                                    \
      pt.x = pair->value.d;                                                   \
      dxf_free_pair (pair);                                                   \
                                                                              \
      pair = dxf_read_pair (dat);                                             \
      EXPECT_DXF (obj->name, #field, dxf + 10);                               \
      pt.y = pair->value.d;                                                   \
      dxf_free_pair (pair);                                                   \
                                                                              \
      pair = dxf_read_pair (dat);                                             \
      EXPECT_DXF (obj->name, #field, dxf + 20);                               \
      pt.z = pair->value.d;                                                   \
      dwg_dynapi_entity_set_value (o, obj->name, #field, &pt, 1);             \
      LOG_TRACE ("%s.%s = (%f, %f, %f) [3BD %d]\n", obj->name, #field, pt.x,  \
                 pt.y, pt.z, pair->code - 20);                                \
      dxf_free_pair (pair);                                                   \
    }
#define FIELD_3BD_1(field, dxf)                                               \
  if (dxf)                                                                    \
    {                                                                         \
      BITCODE_3BD pt;                                                         \
      pair = dxf_read_pair (dat);                                             \
      EXPECT_DXF (obj->name, #field, dxf);                                    \
      pt.x = pair->value.d;                                                   \
      dxf_free_pair (pair);                                                   \
                                                                              \
      pair = dxf_read_pair (dat);                                             \
      EXPECT_DXF (obj->name, #field, dxf + 1);                                \
      pt.y = pair->value.d;                                                   \
      dxf_free_pair (pair);                                                   \
                                                                              \
      pair = dxf_read_pair (dat);                                             \
      EXPECT_DXF (obj->name, #field, dxf + 2);                                \
      pt.z = pair->value.d;                                                   \
      dwg_dynapi_entity_set_value (o, obj->name, #field, &pt, 1);             \
      LOG_TRACE ("%s.%s = (%f, %f, %f) [3BD_1 %d]\n", obj->name, #field,      \
                 pt.x, pt.y, pt.z, pair->code - 2);                           \
      dxf_free_pair (pair);                                                   \
    }
#define SUB_FIELD_BD(sub, field, dxf, _type)                                  \
  if (dxf)                                                                    \
    {                                                                         \
      BITCODE_BD pt;                                                          \
      pair = dxf_read_pair (dat);                                             \
      EXPECT_SUB_DBL_DXF (sub, field, dxf, RD);                               \
      pt = pair->value.d;                                                     \
      dxf_free_pair (pair);                                                   \
    }
#define SUB_FIELD_2BD(sub, field, dxf)                                        \
  if (dxf)                                                                    \
    {                                                                         \
      BITCODE_2BD pt;                                                         \
      pair = dxf_read_pair (dat);                                             \
      EXPECT_SUB_DBL_DXF (sub, field.x, dxf, RD);                             \
      pt.x = pair->value.d;                                                   \
      dxf_free_pair (pair);                                                   \
                                                                              \
      pair = dxf_read_pair (dat);                                             \
      EXPECT_SUB_DBL_DXF (sub, field.y, dxf + 10, RD);                        \
      pt.y = pair->value.d;                                                   \
      dxf_free_pair (pair);                                                   \
    }
#define SUB_FIELD_3BD(sub, field, dxf)                                        \
  if (dxf)                                                                    \
    {                                                                         \
      BITCODE_3BD pt;                                                         \
      pair = dxf_read_pair (dat);                                             \
      EXPECT_SUB_DBL_DXF (sub, field.x, dxf, RD);                             \
      pt.x = pair->value.d;                                                   \
      dxf_free_pair (pair);                                                   \
                                                                              \
      pair = dxf_read_pair (dat);                                             \
      EXPECT_SUB_DBL_DXF (sub, field.y, dxf + 10, RD);                        \
      pt.y = pair->value.d;                                                   \
      dxf_free_pair (pair);                                                   \
                                                                              \
      pair = dxf_read_pair (dat);                                             \
      EXPECT_SUB_DBL_DXF (sub, field.z, dxf + 20, RD);                        \
      pt.z = pair->value.d;                                                   \
      dxf_free_pair (pair);                                                   \
    }
#define FIELD_HANDLE(field, code, dxf)                                        \
  if (dxf)                                                                    \
    {                                                                         \
      pair = dxf_read_pair (dat);                                             \
      EXPECT_H_DXF (#field, code, dxf, H);                                    \
    }
#define SUB_FIELD_HANDLE(sub, field, code, dxf)                               \
  if (dxf)                                                                    \
    {                                                                         \
      pair = dxf_read_pair (dat);                                             \
      EXPECT_SUB_H_DXF (#sub, #field, code, dxf, H);                          \
    }
#define FIELD_T(field, dxf)                                                   \
  if (dxf)                                                                    \
    {                                                                         \
      pair = dxf_read_pair (dat);                                             \
      EXPECT_T_DXF (#field, dxf);                                             \
    }
#define SUB_FIELD_T(sub, field, dxf)                                          \
  if (dxf)                                                                    \
    {                                                                         \
      pair = dxf_read_pair (dat);                                             \
      EXPECT_SUB_T_DXF (#sub, #field, dxf, "T");                              \
    }

static void *
xcalloc (size_t n, size_t s)
{
  void *p;
  if ((n * s) > INT32_MAX)
    goto err;
  p = calloc (n, s);
  if (!p)
    {
    err:
      LOG_ERROR ("Out of memory with calloc %ld * %ld\n", (long)n, (long)s);
      return NULL;
    }
  return p;
}

#ifndef DISABLE_DXF

/* With mips32 -O2 inline would fail. */
static void
dxf_skip_ws (Bit_Chain *dat)
{
  const int is_binary = dat->opts & DWG_OPTS_DXFB;
  if (is_binary)
    return;
  if (dat->byte >= dat->size)
    return;
  // clang-format off
  for (; (!dat->chain[dat->byte] ||
          dat->chain[dat->byte] == ' ' ||
          dat->chain[dat->byte] == '\t' ||
          dat->chain[dat->byte] == '\r');
       )
    // clang-format on
    {
      dat->byte++;
      if (dat->byte >= dat->size)
        return;
    }
}

#  define SAFER_STRTOL(num, rettype, ret)                                     \
    if (dat->byte + 3 >= dat->size                                            \
        || !memchr (&dat->chain[dat->byte], '\n', dat->size - dat->byte))     \
      {                                                                       \
        LOG_ERROR ("Premature DXF end");                                      \
        dat->byte = dat->size;                                                \
        return (rettype)ret;                                                  \
      }                                                                       \
    errno = 0;                                                                \
    num = strtol ((char *)&dat->chain[dat->byte], &endptr, 10);               \
    if (endptr)                                                               \
      {                                                                       \
        if (endptr == (char *)&dat->chain[dat->byte])                         \
          {                                                                   \
            LOG_ERROR ("Expected DXF integer value");                         \
            dat->byte = dat->size;                                            \
            return (rettype)ret;                                              \
          }                                                                   \
        dat->byte += (unsigned char *)endptr - &dat->chain[dat->byte];        \
      }                                                                       \
    if (errno == ERANGE)                                                      \
      return (rettype)num;                                                    \
    if (dat->byte + 1 >= dat->size)                                           \
    return (rettype)num

static BITCODE_RC
dxf_read_rc (Bit_Chain *dat)
{
  const int is_binary = dat->opts & DWG_OPTS_DXFB;
  if (is_binary)
    {
      return bit_read_RC (dat);
    }
  else
    {
      char *endptr;
      long num;
      // avoid overflow over dat->size
      SAFER_STRTOL (num, BITCODE_RC, 0);
      if (dat->chain[dat->byte] == '\r')
        dat->byte++;
      if (dat->chain[dat->byte] == '\n')
        dat->byte++;
      else
        {
          LOG_ERROR ("%s: \\n missing after %ld (at %" PRIuSIZE ")",
                     __FUNCTION__, num, dat->byte);
          dat->byte = dat->size;
        }
      if (num > UINT8_MAX)
        {
          LOG_ERROR ("%s: RC overflow %ld (at %" PRIuSIZE ")", __FUNCTION__,
                     num, dat->byte);
          dat->byte = dat->size;
        }
      return (BITCODE_RC)num;
    }
}

static BITCODE_RS
dxf_read_rs (Bit_Chain *dat)
{
  const int is_binary = dat->opts & DWG_OPTS_DXFB;
  if (is_binary)
    {
      return bit_read_RS (dat);
    }
  else
    {
      char *endptr;
      long num;
      SAFER_STRTOL (num, BITCODE_RS, 0);
      if (dat->chain[dat->byte] == '\r')
        dat->byte++;
      if (dat->chain[dat->byte] == '\n')
        dat->byte++;
      else
        {
          LOG_ERROR ("%s: \\n missing after %ld (at %" PRIuSIZE ")",
                     __FUNCTION__, num, dat->byte);
          dat->byte = dat->size;
        }
      if (num > UINT16_MAX)
        {
          LOG_ERROR ("%s: RS overflow %ld (at %" PRIuSIZE ")", __FUNCTION__,
                     num, dat->byte);
          dat->byte = dat->size;
        }
      return (BITCODE_RS)num;
    }
}

static BITCODE_RL
dxf_read_rl (Bit_Chain *dat)
{
  const int is_binary = dat->opts & DWG_OPTS_DXFB;
  if (is_binary)
    {
      return bit_read_RL (dat);
    }
  else
    {
      char *endptr;
      long num;
      // avoid overflow over dat->size
      SAFER_STRTOL (num, BITCODE_RL, 0);
      if (dat->chain[dat->byte] == '\r')
        dat->byte++;
      if (dat->chain[dat->byte] == '\n')
        dat->byte++;
      else
        {
          LOG_ERROR ("%s: \\n missing after %ld (at %" PRIuSIZE ")",
                     __FUNCTION__, num, dat->byte);
          dat->byte = dat->size;
        }
      /*
      if (num > (long)0xffffffff)
        LOG_ERROR ("%s: RL overflow %ld (at %" PRIuSIZE ")", __FUNCTION__, num,
                   dat->byte);
      */
      return (BITCODE_RL)num;
    }
}

static BITCODE_RLL
dxf_read_rll (Bit_Chain *dat)
{
  const int is_binary = dat->opts & DWG_OPTS_DXFB;
  if (is_binary)
    {
      return bit_read_RLL (dat);
    }
  else
    {
      char *endptr;
      BITCODE_RLL num;
      // avoid overflow over dat->size (need final "  0\nEOF")
      SAFER_STRTOL (num, BITCODE_RLL, 0UL);
      if (dat->chain[dat->byte] == '\r')
        dat->byte++;
      if (dat->chain[dat->byte] == '\n')
        dat->byte++;
      else
        {
          LOG_ERROR ("%s: \\n missing after %ld (at %" PRIuSIZE ")",
                     __FUNCTION__, (long)num, dat->byte);
          num = 0UL;
          dat->byte = dat->size;
        }
      return num;
    }
}

static BITCODE_RD
dxf_read_rd (Bit_Chain *dat)
{
  const int is_binary = dat->opts & DWG_OPTS_DXFB;
  if (is_binary)
    {
      return bit_read_RD (dat);
    }
  else
    {
      char *str, *endptr;
      BITCODE_RD num;
      dxf_skip_ws (dat);
      str = (char *)&dat->chain[dat->byte];
      // avoid overflow over dat->size
      if (dat->byte + 6 >= dat->size
          || !memchr (str, '\n', dat->size - dat->byte - 6))
        {
          LOG_ERROR ("Premature DXF end");
          dat->byte = dat->size;
          return (double)NAN;
        }
      else
        num = strtod (str, &endptr);
      if (endptr)
        dat->byte += endptr - str;
      if (errno == ERANGE)
        return (double)NAN;
      return num;
    }
}

#  if 0
// not yet needed. only with write2004
// ASCII: series of 310 HEX encoded
// BINARY: ??
static unsigned char *
dxf_read_binary (Bit_Chain *dat, unsigned char **p, size_t len)
{
  unsigned char *data;
  const char *pos = (char*)&dat->chain[dat->byte];
  const int is_binary = dat->opts & DWG_OPTS_DXFB;
  const size_t size = len / 2;
  size_t read;
  if (dat->byte + size >= dat->size)
    return NULL;
  //if (is_binary)
  data = p && *p ? (unsigned char *)realloc (*p, size) : (unsigned char *)malloc (size);
  if (!data)
    {
      LOG_ERROR ("Out of memory");
      return NULL;
    }
  LOG_TRACE ("binary[%u]: ", size);
  if ((read = in_hex2bin (data, pos, size) != size))
    LOG_ERROR ("in_hex2bin read only %" PRIuSIZE " of %" PRIuSIZE, read, size);
  dat->byte += read;
  if (p)
    *p = data;
  return data;
}
#  endif

// Target (dynapi) expects UTF8 strings.
// Unicode strings are UTF-8 with quoted \\U+
// Convert old asian MIF \\M+nxxxx to this \\U+XXXX repr. also.
// BINARY: no length prefixes, just zero-terminated strings
static void
dxf_read_string (Bit_Chain *dat, char **string)
{
  const int is_binary = dat->opts & DWG_OPTS_DXFB;
  if (dat->byte >= dat->size)
    return;
  if (is_binary)
    {
#  if 1
      size_t size = strlen ((char *)&dat->chain[dat->byte]) + 1;
      if (!string)
        {
          strncpy (buf, (char *)&dat->chain[dat->byte], 4095);
          if (size > 4095)
            buf[4095] = '\0';
        }
      else
        {
          *string = !*string ? (char *)malloc (size)
                             : (char *)realloc (*string, size);
          if (!*string)
            {
              LOG_ERROR ("Out of memory");
              return;
            }
          strcpy (*string, (char *)&dat->chain[dat->byte]);
        }
      dat->byte += size;
#  else
      size_t size = sscanf ((char *)&dat->chain[dat->byte], "%s", (char *)buf);
      buf[4095] = '\0';
      if (size != EOF)
        // FIXME: TFv fixed size strings
        size = strlen (buf) + 1;
      dat->byte += size;
      if (!string)
        {
          if (size > 4096)
            return;
          return; // ignore, just advanced dat
        }
      *string
          = !*string ? (char *)malloc (size) : (char *)realloc (*string, size);
      if (!*string)
        {
          LOG_ERROR ("Out of memory");
          return;
        }
      strcpy (*string, buf);
#  endif
    }
  else
    {
      int i;
      dxf_skip_ws (dat);
      if (dat->byte >= dat->size
          || !memchr (&dat->chain[dat->byte], '\n', dat->size - dat->byte))
        return;
      for (i = 0;
           dat->byte < dat->size && dat->chain[dat->byte] != '\n' && i < 4096;
           dat->byte++)
        {
          char *s = (char *)&dat->chain[dat->byte];
          if (memBEGINc (s, "\\M+") && s[3] >= '1' && s[3] <= '5')
            {
              const Dwg_Codepage mif_tbl[]
                  = { CP_UNDEFINED, CP_ANSI_932,  CP_ANSI_950,
                      CP_ANSI_949,  CP_ANSI_1361, CP_ANSI_936 };
              int n;
              Dwg_Codepage cp;
              sscanf (&s[3], "%d", &n);
              if (n >= 1 && n <= 5)
                {
                  uint16_t x = 0;
                  uint32_t uc;
                  cp = mif_tbl[n];
                  sscanf (&s[4], "%4hX", &x);
                  uc = dwg_codepage_uwc (cp, x);
                  snprintf (&buf[i], 4096 - i, "\\U+%04X", uc);
                  i += 7;
                  dat->byte += 7;
                }
            }
          else
            buf[i++] = *s;
        }
      if (dat->byte >= dat->size || i >= 4096)
        return;
      if (i && buf[i - 1] == '\r')
        buf[i - 1] = '\0';
      else
        buf[i] = '\0';
      dat->byte++;

      // dxf_skip_ws (dat);
      if (!string)
        return; // ignore, just advanced dat
      if (!*string)
        *string = (char *)malloc (strlen (buf) + 1);
      else
        *string = (char *)realloc (*string, strlen (buf) + 1);
      if (!*string)
        {
          LOG_ERROR ("Out of memory");
          return;
        }
      strcpy (*string, buf);
    }
}

static void
dxf_free_pair (Dxf_Pair *pair)
{
  if (!pair)
    return;
  if (pair->type == DWG_VT_STRING || pair->type == DWG_VT_BINARY)
    {
      free (pair->value.s);
      pair->value.s = NULL;
    }
  else if (pair->code == 0 || pair->code == 2)
    {
      free (pair->value.s);
      pair->value.s = NULL;
    }
  free (pair);
  pair = NULL;
}

static Dxf_Pair *ATTRIBUTE_MALLOC
dxf_read_pair (Bit_Chain *dat)
{
  Dxf_Pair *pair = (Dxf_Pair *)xcalloc (1, sizeof (Dxf_Pair));
  const int is_binary = dat->opts & DWG_OPTS_DXFB;
  if (!pair)
    return NULL;
  if (dat->size - dat->byte < 6) // at least 0\nEOF\n
    {
    err:
      LOG_ERROR ("Unexpected DXF end-of-file");
      free (pair);
      return NULL;
    }
  if (is_binary)
    LOG_HANDLE ("%4zx: ", dat->byte);
  pair->code = (short)dxf_read_rs (dat);
  if (dat->size - dat->byte < 4) // at least EOF\n
    goto err;
  pair->type = dwg_resbuf_value_type (pair->code);
  // if (pair->code == 280) // && strEQc (key, "ENDCAPS")
  //   pair->type = DWG_VT_INT16; // for HEADER.ENDCAPS - CEPSNTYPE
  switch (pair->type)
    {
    case DWG_VT_STRING:
      dxf_read_string (dat, &pair->value.s);
      if (!pair->value.s && pair->code != 0)
        pair->value.s = (char *)calloc (1, 1);
      if (!pair->value.s)
        {
          LOG_ERROR ("Out of memory");
          dxf_free_pair (pair);
          return NULL;
        }
      LOG_TRACE ("  dxf (%d, \"%s\")\n", (int)pair->code, pair->value.s);
      // dynapi_set_helper converts from utf-8 to unicode, not here.
      // we need to know the type of the target field, if TV or T
      break;
    case DWG_VT_BOOL:
    case DWG_VT_INT8:
      pair->value.i = dxf_read_rc (dat);
      LOG_TRACE ("  dxf (%d, %d)\n", (int)pair->code, pair->value.i);
      break;
    case DWG_VT_INT16:
      pair->value.i = dxf_read_rs (dat);
      LOG_TRACE ("  dxf (%d, %d)\n", (int)pair->code, pair->value.i);
      break;
    case DWG_VT_INT32:
      pair->value.l = dxf_read_rl (dat);
      LOG_TRACE ("  dxf (%d, %ld)\n", (int)pair->code, pair->value.l);
      break;
    case DWG_VT_INT64:
      pair->value.rll = dxf_read_rll (dat);
      LOG_TRACE ("  dxf (%d, " FORMAT_RLL ")\n", (int)pair->code,
                 pair->value.rll);
      break;
    case DWG_VT_REAL:
    case DWG_VT_POINT3D:
      dxf_skip_ws (dat);
      pair->value.d = dxf_read_rd (dat);
      LOG_TRACE ("  dxf (%d, %f)\n", pair->code, pair->value.d);
      break;
    case DWG_VT_BINARY:
      // zero-terminated. TODO hex decode here already?
      dxf_read_string (dat, &pair->value.s);
      if (!pair->value.s)
        pair->value.s = (char *)calloc (1, 1);
      if (!pair->value.s)
        {
          LOG_ERROR ("Out of memory");
          dxf_free_pair (pair);
          return NULL;
        }
      LOG_TRACE ("  dxf (%d, %s)\n", (int)pair->code, pair->value.s);
      break;
    case DWG_VT_HANDLE:
    case DWG_VT_OBJECTID:
      // BINARY: hex string without len
      dxf_read_string (dat, NULL);
      sscanf (buf, "%X", &pair->value.u);
      LOG_TRACE ("  dxf (%d, %X)\n", (int)pair->code, pair->value.u);
      break;
    case DWG_VT_INVALID:
    default:
      LOG_ERROR ("Invalid DXF group code: %d", pair->code);
      dxf_free_pair (pair);
      return NULL;
    }
  return pair;
}

#  define DXF_CHECK_EOF                                                       \
    if (dat->byte >= dat->size || (pair == NULL)                              \
        || (pair->code == 0 && !pair->value.s)                                \
        || (pair->code == 0 && strEQc (pair->value.s, "EOF")))                \
      {                                                                       \
        if (pair)                                                             \
          dxf_free_pair (pair);                                               \
        pair = NULL;                                                          \
        return 1;                                                             \
      }
#  define DXF_RETURN_EOF(what)                                                \
    if (dat->byte >= dat->size || (pair == NULL)                              \
        || (pair->code == 0 && !pair->value.s)                                \
        || (pair->code == 0 && strEQc (pair->value.s, "EOF")))                \
      {                                                                       \
        if (pair)                                                             \
          dxf_free_pair (pair);                                               \
        pair = NULL;                                                          \
        return what;                                                          \
      }
#  define DXF_BREAK_EOF                                                       \
    if (dat->byte >= dat->size || (pair == NULL)                              \
        || (pair->code == 0 && !pair->value.s)                                \
        || (pair->code == 0 && strEQc (pair->value.s, "EOF")))                \
    break

static Dxf_Pair *
dxf_skip_comment (Bit_Chain *dat, Dxf_Pair *pair)
{
  while (pair != NULL && pair->code == 999)
    {
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
      DXF_RETURN_EOF (pair);
    }
  return pair;
}

/*--------------------------------------------------------------------------------
 * MACROS
 */

#  define ACTION indxf
// #define IS_ENCODER
// #define IS_DXF

/* Store all handle fieldnames and string values into this flexarray.
   We need strdup'd copies, the dxf input will be freed.
 */
array_hdls *
array_push (array_hdls *restrict hdls, const char *restrict field,
            const char *restrict name, const int code)
{
  uint32_t i = hdls->nitems;
  if (i >= hdls->size)
    {
      hdls->size += 16;
      hdls = (array_hdls *)realloc (
          hdls, 8 + (hdls->size * sizeof (struct array_hdl)));
      if (!hdls)
        {
          LOG_ERROR ("Out of memory");
          return NULL;
        }
      // memset (hdls, 0, 8 + (hdls->size * sizeof (struct array_hdl));
    }
  hdls->nitems = i + 1;
  hdls->items[i].field = strdup (field);
  hdls->items[i].name = strdup (name);
  hdls->items[i].code = code;
  return hdls;
}

array_hdls *
new_array_hdls (uint32_t size)
{
  array_hdls *hdls
      = (array_hdls *)xcalloc (1, 8 + size * sizeof (struct array_hdl));
  if (!hdls)
    return NULL;
  hdls->size = size;
  return hdls;
}

void
free_array_hdls (array_hdls *hdls)
{
  for (uint32_t i = 0; i < hdls->nitems; i++)
    {
      free (hdls->items[i].field);
      free (hdls->items[i].name);
    }
  free (hdls);
}

#  define DXF_CHECK_ENDSEC                                                    \
    if (pair != NULL                                                          \
        && (dat->byte >= dat->size || (pair->code == 0 && !pair->value.s)     \
            || (pair->code == 0 && strEQc (pair->value.s, "ENDSEC"))))        \
    return 0
#  define DXF_BREAK_ENDSEC                                                    \
    if (pair != NULL                                                          \
        && (dat->byte >= dat->size || (pair->code == 0 && !pair->value.s)     \
            || (pair->code == 0 && strEQc (pair->value.s, "ENDSEC"))))        \
    break
#  define DXF_RETURN_ENDSEC(what)                                             \
    if (pair != NULL                                                          \
        && (dat->byte >= dat->size || (pair->code == 0 && !pair->value.s)     \
            || (pair->code == 0 && strEQc (pair->value.s, "ENDSEC"))))        \
      {                                                                       \
        dxf_free_pair (pair);                                                 \
        return what;                                                          \
      }

static Dxf_Pair *
dxf_expect_code (Bit_Chain *restrict dat, Dxf_Pair *restrict pair, int code)
{
  while (pair != NULL && pair->code != code)
    {
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
      pair = dxf_skip_comment (dat, pair);
      DXF_RETURN_EOF (pair);
      if (pair && pair->code != code)
        {
          LOG_ERROR ("Expecting DXF code %d, got %d (at %" PRIuSIZE ")", code,
                     pair->code, dat->byte);
        }
    }
  return pair;
}

static int
matches_type (Dxf_Pair *restrict pair, const Dwg_DYNAPI_field *restrict f)
{
  switch (pair->type)
    {
    case DWG_VT_STRING:
      if (f->is_string)
        return 1;
      if (f->type[0] == 'H')
        return 1; // handles can be just names
      break;
    case DWG_VT_INT64:
      // BLL or RLL
      if (f->size == 8 && f->type[1] == 'L' && f->type[2] == 'L')
        return 1;
      // fall through
    case DWG_VT_INT32:
      // BL or RL
      if (f->size == 4 && f->type[1] == 'L')
        return 1;
      // fall through
    case DWG_VT_INT16:
      // BS or RS or CMC
      if (f->size == 2 && f->type[1] == 'S')
        return 1;
      if (strEQc (f->type, "CMC"))
        return 1;
      if (strEQc (f->type, "BSd"))
        return 1;
      // fall through
    case DWG_VT_INT8:
      if (strEQc (f->type, "RC"))
        return 1;
      // fall through
    case DWG_VT_BOOL:
      if (strEQc (f->type, "B"))
        return 1;
      break;
    case DWG_VT_REAL:
      // BD or RD
      if (f->size == 8 && f->type[1] == 'D')
        return 1;
      if (strEQc (f->type, "TIMEBLL"))
        return 1;
      break;
    case DWG_VT_POINT3D:
      // 3BD or 3RD or 3DPOINT or BE
      if (f->size == 24 && (f->type[0] == '3' || strEQc (f->type, "BE")))
        return 1;
      // accept 2BD or 2RD or 2DPOINT also
      if (f->size == 16 && f->type[0] == '2')
        return 1;
      break;
    case DWG_VT_BINARY:
      if (f->is_string)
        return 1;
      break;
    case DWG_VT_HANDLE:
    case DWG_VT_OBJECTID:
      if (f->type[0] == 'H')
        return 1;
      break;
    case DWG_VT_INVALID:
    default:
      LOG_ERROR ("Invalid DXF group code: %d", pair->code);
    }
  return 0;
}

/* Also used by in_json */
const Dwg_DYNAPI_field *
find_numfield (const Dwg_DYNAPI_field *restrict fields,
               const char *restrict key)
{
  const Dwg_DYNAPI_field *f;
  char s[80];
  strcpy (s, "num_");
  strcat (s, key);
  // see gen-dynapi.pl:1102
  if (strEQc (key, "attribs"))
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
  else if (strEQc (key, "cellstyle.borders"))
    strcpy (s, "cellstyle.num_borders");
  else if (strEQc (key, "segs") || strEQc (key, "polyline_paths"))
    strcpy (s, "num_segs_or_paths");
  else if (strEQc (key, "txt.col_sizes"))
    strcpy (s, "txt.num_col_sizes");
search:
  for (f = &fields[0]; f->name; f++)
    {
      if (strEQ (s, f->name))
        return f;
      // but not HATCH
      else if (strEQc (key, "dashes") && strEQc (f->type, "Dwg_LTYPE_dash*"))
        {
          strcpy (s, "numdashes");
          goto search;
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
      strcpy (s, "num_segs_or_paths");
      goto search;
    }
  return NULL;
}

/* convert to flag */
BITCODE_RC
dxf_find_lweight (const int16_t lw)
{
  // See acdb.h: 100th of a mm, enum of
  const int lweights[] = { 0,
                           5,
                           9,
                           13,
                           15,
                           18,
                           20,
                           25,
                           30,
                           35,
                           40,
                           50,
                           53,
                           60,
                           70,
                           80,
                           90,
                           100,
                           106,
                           120,
                           140,
                           158,
                           200,
                           211,
                           /*illegal/reserved:*/ 0,
                           0,
                           0,
                           0,
                           0,
                           /*29:*/ -1, // BYLAYER
                           -2,         // BYBLOCK
                           -3 };       // BYLWDEFAULT
  for (int i = 0; i < 32; i++)
    {
      if (lweights[i] == lw)
        return i;
    }
  return 0;
}

// FIXME: support 430 (name), 440 (alpha)
static int
dxf_read_CMC (const Dwg_Data *restrict dwg, Bit_Chain *restrict dat,
              BITCODE_CMC *restrict color, const char *fieldname,
              const int dxf)
{
  int error = 1;
  size_t pos = bit_position (dat);
  Dxf_Pair *pair = dxf_read_pair (dat);
  if (!color || pair == NULL)
    {
      LOG_ERROR ("empty CMC field %s", fieldname);
      return 1;
    }
  if (pair->code < 90 && dxf == pair->code)
    {
      color->index = pair->value.i;
      if (pair->value.i == 256) // bylayer
        color->method = 0xc2;
      if (pair->value.i == 257) // none
        color->method = 0xc8;
      else if (dwg->header.version >= R_2004)
        {
          color->index = 256;
          color->rgb = pair->value.l;
          color->rgb |= 0xc2000000;
          LOG_TRACE ("%s.rgb = 0x%08x [%s %d]\n", fieldname, color->rgb, "CMC",
                     pair->code);
        }
      LOG_TRACE ("%s.index = %d [%s %d]\n", fieldname, color->index, "CMC",
                 pair->code);
      // optional 420, 430, 440 fields
      pos = bit_position (dat);
      error = dxf_read_CMC (dwg, dat, color, fieldname, dxf);
    }
  else if (pair->code < 430 && pair->code == (dxf + 420 - 62)) // truecolor
    {
      color->rgb = pair->value.l;
      color->rgb |= 0xc3000000;
      LOG_TRACE ("%s.rgb = 0x%08x [%s %d]\n", fieldname, color->rgb, "CMC",
                 pair->code);
      error = 0;
    }
  // TODO 430, 440
  else if (pair->code < 440 && pair->code == (dxf + 430 - 62)) // name
    {
      LOG_WARN ("%s.name %s ignored [%s %d]", fieldname, pair->value.s, "CMC",
                pair->code);
      error = 0;
    }
  else if (pair->code < 450 && pair->code == (dxf + 440 - 62)) // alpha
    {
      LOG_WARN ("%s.alpha %ld ignored [%s %d]", fieldname, pair->value.l,
                "CMC", pair->code);
      error = 0;
    }
  dxf_free_pair (pair);
  if (error)
    {
      LOG_TRACE ("no optional CMC, backup\n");
      bit_set_position (dat, pos);
    }
  return error;
}

static int
dxf_header_read (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  Dwg_Header_Variables *_obj = &dwg->header_vars;
  Dwg_Object *obj = NULL;
  const int is_binary = dat->opts & DWG_OPTS_DXFB;
  // const int minimal = dwg->opts & DWG_OPTS_MINIMAL;
  int is_tu = 1;
  int i = 0;
  Dxf_Pair *pair;

  // defaults, not often found in a DXF
  _obj->ISOLINES = 4;
  _obj->TEXTQLTY = 50;
  _obj->FACETRES = 0.5;

  // here SECTION (HEADER) was already consumed
  // read the first group 9, $field pair
  pair = dxf_read_pair (dat);
  while (pair != NULL && pair->code == 9 && pair->value.s)
    {
      char field[80];
      strncpy (field, pair->value.s, 79);
      field[79] = '\0';
      i = 0;

      // now read the code, value pair. for points it may be multiple (index i)
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
      if (!pair)
        {
          pair = dxf_read_pair (dat);
          if (!pair)
            return 1;
        }
      DXF_BREAK_ENDSEC;
    next_hdrvalue:
      if (is_binary && pair->code == 280
          && (strEQc (field, "$ENDCAPS") || strEQc (field, "$JOINSTYLE")))
        dat->byte++; // B => RS
      if (pair->code == 1 && strEQc (field, "$ACADVER")
          && pair->value.s != NULL)
        {
          int vi; // C++ quirks
          // Note: Here version is still R_INVALID, thus pair->value.s
          // is never TU.
          const char *version = pair->value.s;
          dat->from_version = dwg->header.from_version
              = dwg_version_hdr_type (version);
          is_tu = dat->from_version >= R_2007;
          LOG_TRACE ("HEADER.from_version = %s,\tdat->from_version = %s\n",
                     dwg_version_codes (dwg->header.from_version),
                     dwg_version_codes (dat->from_version));
          if (dat->from_version == R_INVALID)
            {
              LOG_ERROR ("Invalid HEADER: 9 %s, 1 %s", field, version)
              return DWG_ERR_INVALIDDWG;
            }
          if (is_tu && dwg->num_objects
              && dwg->object[0].fixedtype == DWG_TYPE_BLOCK_HEADER)
            {
              Dwg_Object_BLOCK_HEADER *o
                  = dwg->object[0].tio.object->tio.BLOCK_HEADER;
              free (o->name);
              o->name = (char *)bit_utf8_to_TU ((char *)"*Model_Space", 0);
            }
          // currently we can only encode DWGs to r13-r2000, but DXF's to
          // almost everything.
          if (dwg->header.from_version >= R_13b1
              && dwg->header.from_version <= R_2000)
            dwg->header.version = dat->version = dwg->header.from_version;
          LOG_TRACE ("HEADER.version = %s,\tdat->version = %s\n",
                     dwg_version_codes (dwg->header.version),
                     dwg_version_codes (dat->version));
        }
      else if (field[0] == '$')
        {
          const Dwg_DYNAPI_field *f = dwg_dynapi_header_field (&field[1]);
          if (!f)
            {
              if (strEQc (field, "$DWGCODEPAGE"))
                {
                  dwg->header_vars.DWGCODEPAGE = pair->value.s;
                  LOG_TRACE ("HEADER.%s %s [TV %d]\n", &field[1],
                             pair->value.s, (int)pair->code);
                  dxf_set_DWGCODEPAGE (dwg); // needed early to set the cp for all strings
                  dwg->header_vars.DWGCODEPAGE = SET_STR (pair->value.s);
                }
              else
              if (pair->code == 40 && strEQc (field, "$3DDWFPREC"))
                {
                  LOG_TRACE ("HEADER.%s [%s %d]\n", &field[1], "BD",
                             pair->code);
                  dwg->header_vars._3DDWFPREC = pair->value.d;
                }

#  define SUMMARY_T(name)                                                     \
    (pair->code == 1 && strEQc (field, "$" #name) && pair->value.s != NULL)   \
    {                                                                         \
      char dest[1024];                                                        \
      LOG_TRACE ("SUMMARY.%s = %s [T16 1]\n", &field[1], pair->value.s);      \
      bit_utf8_to_TV (dest, (unsigned char *)pair->value.s, 1024,             \
                            strlen (pair->value.s), 0, dat->codepage);        \
      dest[1023] = '\0';                                                      \
      dwg->summaryinfo.name = SET_STR (dest);                                 \
    }

              // clang-format off
              else if SUMMARY_T (TITLE)
              else if SUMMARY_T (AUTHOR)
              else if SUMMARY_T (SUBJECT)
              else if SUMMARY_T (KEYWORDS)
              else if SUMMARY_T (COMMENTS)
              else if SUMMARY_T (LASTSAVEDBY)
              else if (pair->code == 1
                       && strEQc (field, "$CUSTOMPROPERTYTAG")
                       && pair->value.s != NULL)
                // clang-format on
                {
                  char dest[1024];
                  BITCODE_BL j = dwg->summaryinfo.num_props;
                  dwg->summaryinfo.num_props++;
                  dwg->summaryinfo.props
                      = (Dwg_SummaryInfo_Property *)realloc (
                          dwg->summaryinfo.props,
                          (j + 1) * sizeof (Dwg_SummaryInfo_Property));
                  memset (dwg->summaryinfo.props + j, 0,
                          sizeof (Dwg_SummaryInfo_Property));
                  LOG_TRACE ("SUMMARY.props[%u].tag = %s [TU16 1]\n", j,
                             pair->value.s);
                  bit_utf8_to_TV (dest, (unsigned char *)pair->value.s, 1024,
                                  strlen (pair->value.s), 0, dat->codepage);
                  dest[1023] = '\0';
                  dwg->summaryinfo.props[j].tag = SET_STR (dest);
                }
              else if (pair->code == 1 && strEQc (field, "$CUSTOMPROPERTY")
                       && pair->value.s != NULL && dwg->summaryinfo.props
                       && dwg->summaryinfo.num_props > 0)
                {
                  char dest[1024];
                  BITCODE_BL j = dwg->summaryinfo.num_props - 1;
                  LOG_TRACE ("SUMMARY.props[%u].value = %s [TU16 1]\n", j,
                             pair->value.s);
                  bit_utf8_to_TV (dest, (unsigned char *)pair->value.s, 1024,
                                  strlen (pair->value.s), 0, dat->codepage);
                  dest[1023] = '\0';
                  dwg->summaryinfo.props[j].value = SET_STR (dest);
                }
              else
                LOG_ERROR ("skipping HEADER: 9 %s, unknown field with code %d",
                           field, pair->code);
            }
          else if (!matches_type (pair, f) && strNE (field, "$XCLIPFRAME")
                   && strNE (field, "$OSMODE") && strNE (field, "$TIMEZONE"))
            {
              // XCLIPFRAME is 280 RC or 290 B in dynapi.
              // TIMEZONE is BLd (signed)
              LOG_ERROR (
                  "skipping HEADER: 9 %s, wrong type code %d <=> field %s",
                  field, pair->code, f->type);
            }
          else if (pair->type == DWG_VT_POINT3D)
            {
              BITCODE_3BD pt = { 0.0, 0.0, 0.0 };
              if (i)
                dwg_dynapi_header_value (dwg, &field[1], &pt, NULL);
              if (i == 0)
                pt.x = pair->value.d;
              else if (i == 1)
                pt.y = pair->value.d;
              else if (i == 2)
                pt.z = pair->value.d;
              if (i > 2)
                {
                  LOG_ERROR ("skipping HEADER: 9 %s, too many point elements",
                             field);
                }
              else
                {
                  // yes, set it 2-3 times
                  LOG_TRACE ("HEADER.%s [%s %d][%d] = %f\n", &field[1],
                             f->type, pair->code, i, pair->value.d);
                  dwg_dynapi_header_set_value (dwg, &field[1], &pt, 1);
                  i++;
                }
            }
          else if (pair->type == DWG_VT_STRING && strEQc (f->type, "H"))
            {
              char *key, *str;
              if (pair->value.s && strlen (pair->value.s))
                {
                  LOG_TRACE ("HEADER.%s %s [%s %d] later\n", &field[1],
                             pair->value.s, f->type, (int)pair->code);
                  // name (which table?) => handle
                  // needs to be postponed, because we don't have the tables
                  // yet.
                  header_hdls = array_push (header_hdls, &field[1],
                                            pair->value.s, pair->code);
                }
              else
                {
                  BITCODE_H hdl = dwg_add_handleref (dwg, 5, 0, NULL);
                  LOG_TRACE ("HEADER.%s NULL 5 [H %d]\n", &field[1],
                             pair->code);
                  dwg_dynapi_header_set_value (dwg, &field[1], &hdl, 1);
                }
            }
          else if (strEQc (f->type, "H"))
            {
              BITCODE_H hdl;
              hdl = dwg_add_handleref (dwg, 4, pair->value.u, NULL);
              LOG_TRACE ("HEADER.%s %X [H %d]\n", &field[1], pair->value.u,
                         pair->code);
              dwg_dynapi_header_set_value (dwg, &field[1], &hdl, 1);
            }
          else if (strEQc (f->type, "CMC"))
            {
              static BITCODE_CMC color = { 0 };
              if (pair->code <= 70)
                {
                  LOG_TRACE ("HEADER.%s.index %d [CMC %d]\n", &field[1],
                             pair->value.i, pair->code);
                  color.index = pair->value.i;
                  dwg_dynapi_header_set_value (dwg, &field[1], &color, 0);
                }
            }
          else if (pair->type == DWG_VT_REAL && strEQc (f->type, "TIMEBLL"))
            {
              static BITCODE_TIMEBLL date = { 0, 0, 0 };
              date.value = pair->value.d;
              date.days = (BITCODE_BL)trunc (pair->value.d);
              date.ms = (BITCODE_BL)(86400000.0 * (date.value - date.days));
              LOG_TRACE ("HEADER.%s %.09f (" FORMAT_BL ", " FORMAT_BL
                         ") [TIMEBLL %d]\n",
                         &field[1], date.value, date.days, date.ms,
                         pair->code);
              dwg_dynapi_header_set_value (dwg, &field[1], &date, 0);
            }
          else if (pair->type == DWG_VT_STRING)
            {
              LOG_TRACE ("HEADER.%s [%s %d]\n", &field[1], f->type,
                         pair->code);
              // TODO need to free old string values
              dwg_dynapi_header_set_value (dwg, &field[1], &pair->value, 1);
            }
          else
            {
              LOG_TRACE ("HEADER.%s [%s %d]\n", &field[1], f->type,
                         pair->code);
              dwg_dynapi_header_set_value (dwg, &field[1], &pair->value, 1);
            }
        }
      else
        {
          LOG_ERROR ("skipping HEADER: 9 %s, missing the $", field);
        }

      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
      if (!pair)
        {
          pair = dxf_read_pair (dat);
          if (!pair)
            return 1;
        }
      DXF_BREAK_ENDSEC;
      if (pair->code != 9 /* && pair->code != 0 */)
        goto next_hdrvalue; // for mult. 10,20,30 values
    }

  VERSIONS (R_13, R_14)
  {
    _obj->unit1_ratio = 412148564080.0; // m to ??
    _obj->unit2_ratio = 6.162483e-14;
    _obj->unit3_ratio = 1.62263e+13;
    _obj->unit4_ratio = 2.63294e+26;
    _obj->unit1_name = dwg_add_u8_input (dwg, "meter");
    _obj->unit2_name = dwg_add_u8_input (dwg, "inch");
    _obj->unit3_name = dwg_add_u8_input (dwg, "inch");
    _obj->unit4_name = dwg_add_u8_input (dwg, "sq inch");
  }
  SINCE (R_2000b)
  {
    BITCODE_BSd celweight = dxf_revcvt_lweight (_obj->CELWEIGHT);
    // clang-format off
    _obj->FLAGS = (celweight & 0x1f)       |
          (_obj->ENDCAPS     ? 0x60   : 0) |
          (_obj->JOINSTYLE   ? 0x180  : 0) |
          (_obj->LWDISPLAY   ? 0 : 0x200)  |
          (_obj->XEDIT       ? 0 : 0x400)  |
          (_obj->EXTNAMES    ? 0x800  : 0) |
          (_obj->PSTYLEMODE  ? 0x2000 : 0) |
          (_obj->OLESTARTUP  ? 0x4000 : 0);
    // clang-format on
    LOG_TRACE ("HEADER.%s => 0x%x\n", "FLAGS", (unsigned)_obj->FLAGS);
    dwg->Template.MEASUREMENT = _obj->MEASUREMENT;
    LOG_TRACE ("TEMPLATE.MEASUREMENT = HEADER.MEASUREMENT %d\n",
               (int)_obj->MEASUREMENT);
  }

  dxf_free_pair (pair);
  return 0;
}

static void
dxf_set_default_DWGCODEPAGE (Bit_Chain *dat, Dwg_Data *dwg)
{
  Dwg_Header_Variables *vars = &dwg->header_vars;
  Dwg_Header *hdr = &dwg->header;

  if (vars->DWGCODEPAGE)
    return;
  if (hdr->version < R_2007)
    {
      vars->DWGCODEPAGE = strdup ("ANSI_1252");
      hdr->codepage = CP_ANSI_1252;
    }
  else
    {
      vars->DWGCODEPAGE = strdup ("UTF-16");
      hdr->codepage = CP_UTF16;
    }
  LOG_TRACE ("default HEADER.codepage = %d [%s]\n", hdr->codepage,
             vars->DWGCODEPAGE);
  dat->codepage = hdr->codepage;
}

static void
dxf_set_DWGCODEPAGE (Bit_Chain *dat, Dwg_Data *dwg)
{
  Dwg_Header_Variables *vars = &dwg->header_vars;
  Dwg_Header *hdr = &dwg->header;

  if (!vars->DWGCODEPAGE)
    return;
  // r11 usually has "undefined"
  if (hdr->from_version <= R_12 && strEQc (vars->DWGCODEPAGE, "undefined"))
    hdr->codepage = CP_UNDEFINED;
  else
    {
      hdr->codepage = dwg_codepage_int (vars->DWGCODEPAGE);
      if (hdr->codepage == CP_UNDEFINED)
        {
          LOG_ERROR ("Invalid DWGCODEPAGE %s", vars->DWGCODEPAGE);
        }
      else
        {
          LOG_TRACE ("HEADER.codepage = %u [%s]\n", hdr->codepage,
                     vars->DWGCODEPAGE);
          dat->codepage = hdr->codepage;
        }
    }
}

static void
dxf_fixup_header (Bit_Chain *dat, Dwg_Data *dwg)
{
  Dwg_Header_Variables *vars = &dwg->header_vars;
  Dwg_Header *hdr = &dwg->header;
  const struct dwg_versions *_verp
      = dwg->header.version == R_INVALID
            ? dwg_version_struct (R_2000b)
            : dwg_version_struct (dwg->header.version);
  // Dwg_AuxHeader *aux = &dwg->auxheader;
  LOG_TRACE ("dxf_fixup_header\n");
  if (dwg->header.version == R_INVALID)
    dwg->header.version = R_2000;
  if (dwg->header.from_version == R_INVALID)
    dwg->header.from_version = R_11;
  if (vars->HANDSEED)
    vars->HANDSEED->handleref.code = 0;
  if (vars->DWGCODEPAGE && !hdr->codepage)
    dxf_set_DWGCODEPAGE (dat, dwg);
  else
    dxf_set_default_DWGCODEPAGE (dat, dwg);

  // R_2007:
  // is_maint: 0x32 [RC 0]
  // zero_one_or_three: 0x3 [RC 0]
  // thumbnail_addr: 3360 [RL 0]
  // dwg_version: 0x1f [RC 0]
  // maint_version: 0x8 [RC 0]
  // codepage: 30 [RS 0]
  // R_2004+:
  // unknown_0: 0x0 [RC 0]
  // app_dwg_version: 0x1f [RC 0]
  // app_maint_version: 0x8 [RC 0]
  // security_type: 0 [RL 0]
  // rl_1c_address: 0 [RL 0]
  // summary_info_address: 3200 [RL 0]
  // vba_proj_address: 0 [RL 0]
  // r2004_header_address: 128 [RL 0]

  // R_2000:
  // is_maint: 0xf [RC 0]
  // zero_one_or_three: 0x1 [RC 0]
  // thumbnail_addr: 220 [RL 0]
  // dwg_version: 0x1f [RC 0]
  // maint_version: 0x8 [RC 0]
  // codepage: 30 [RS 0]

  if (_verp)
    hdr->dwg_version = _verp->dwg_version;
  if (hdr->version <= R_14)
    {
      hdr->is_maint = 0x0;
      if (hdr->version == R_13 && vars->PROXYGRAPHICS > 0)
        {
          hdr->dwg_version = R_13c3;
          hdr->is_maint = 0x5;
        }
    }
  else if (hdr->version <= R_2000)
    {
      hdr->is_maint = 0xf; // 0x6 - 0xf
      hdr->zero_one_or_three = 1;
      hdr->thumbnail_address = 220;
      if (!hdr->dwg_version)
        hdr->dwg_version = 0x21;
      hdr->maint_version = 0x8;
    }
  else if (hdr->version <= R_2004)
    hdr->is_maint = 0x68;
  else if (hdr->version <= R_2007)
    hdr->is_maint = 0x32;
  else if (hdr->version <= R_2010)
    hdr->is_maint = 0x6d;
  else if (hdr->version <= R_2013)
    hdr->is_maint = 0x7d;
  else if (hdr->version <= R_2018)
    hdr->is_maint = 0x4;

  if (!vars->FINGERPRINTGUID)
    {
      vars->FINGERPRINTGUID
          = dwg->header.from_version >= R_2007
                ? (BITCODE_TV)bit_utf8_to_TU (
                      (char *)"{00000000-0000-0000-0000-000000000000}", 0)
                : strdup ("{00000000-0000-0000-0000-000000000000}");
    }
  if (!vars->VERSIONGUID)
    {
      vars->VERSIONGUID
          = dwg->header.from_version >= R_2007
                ? (BITCODE_TV)bit_utf8_to_TU (
                      (char *)"{A6BF05D3-02A0-4EB8-9AEE-9443625E66B6}", 0)
                : (BITCODE_TV)strdup (
                      "{DE6A95C3-2D01-4A77-AC28-3C42FCFFF657}"); // R_2000
    }
}

static int
dxf_classes_read (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  BITCODE_BL i;
  Dxf_Pair *pair = dxf_read_pair (dat);
  Dwg_Class *klass;
  const int is_binary = dat->opts & DWG_OPTS_DXFB;
  const char *t_type = dat->version >= R_2007 ? "TU" : "TV";

  while (pair)
    { // read next class
      // add class (see decode)
      i = dwg->num_classes;
      if (i == 0)
        dwg->dwg_class = (Dwg_Class *)malloc (sizeof (Dwg_Class));
      else
        dwg->dwg_class = (Dwg_Class *)realloc (dwg->dwg_class,
                                               (i + 1) * sizeof (Dwg_Class));
      if (!dwg->dwg_class)
        {
          LOG_ERROR ("Out of memory");
          return DWG_ERR_OUTOFMEM;
        }
    restart:
      klass = &dwg->dwg_class[i];
      memset (klass, 0, sizeof (Dwg_Class));
      if (pair != NULL && pair->code == 0 && pair->value.s
          && (strEQc (pair->value.s, "CLASS") || strEQc (pair->value.s, "ENDSEC")))
        {
          if (strEQc (pair->value.s, "ENDSEC"))
            {
              dxf_free_pair (pair);
              return 0;
            }
          dxf_free_pair (pair);
          pair = dxf_read_pair (dat);
          if (!pair)
            return 1;
        }
      else
        {
          LOG_ERROR ("2 CLASSES must be followed by 0 CLASS or ENDSEC")
          DXF_RETURN_EOF (DWG_ERR_INVALIDDWG);
          return DWG_ERR_INVALIDDWG;
        }
      klass->number = 500 + i;
      while (pair != NULL && pair->code != 0)
        { // read until next 0 CLASS
          switch (pair->code)
            {
            case 1:
              if (pair->value.s)
                {
                  const char *n = strEQc (pair->value.s, "ACDBDATATABLE")
                                      ? "DATATABLE"
                                      : pair->value.s;
                  if (klass->dxfname)
                    {
                      LOG_ERROR ("Group 1 for CLASS %s already read",
                                 klass->dxfname);
                      break;
                    }
                  STRADD_TV (klass->dxfname, n);
                  if (dat->version >= R_2007)
                    klass->dxfname_u = bit_utf8_to_TU ((char *)n, 0);
                  LOG_TRACE ("CLASS[%d].dxfname = %s [TV 1]\n", i, n);
                }
              break;
            case 2:
              if (klass->cppname)
                {
                  LOG_ERROR ("Group 2 for CLASS %s already read",
                             klass->dxfname);
                  break;
                }
              if (pair->value.s)
                {
                  STRADD_T (klass->cppname, pair->value.s);
                }
              LOG_TRACE ("CLASS[%d].cppname = %s [%s 2]\n", i, pair->value.s,
                         t_type);
              break;
            case 3:
              if (klass->appname)
                {
                  LOG_ERROR ("Group 3 for CLASS %s already read",
                             klass->dxfname);
                  break;
                }
              if (pair->value.s)
                {
                  STRADD_T (klass->appname, pair->value.s);
                }
              LOG_TRACE ("CLASS[%d].appname = %s [%s 3]\n", i, pair->value.s,
                         t_type);
              break;
            case 90:
              klass->proxyflag = pair->value.l;
              LOG_TRACE ("CLASS[%d].proxyflag = %ld [BS 90]\n", i,
                         pair->value.l);
              break;
            case 91:
              klass->num_instances = pair->value.l;
              LOG_TRACE ("CLASS[%d].num_instances = %ld [BL 91]\n", i,
                         pair->value.l);
              break;
            case 280: // ie was_proxy
              klass->is_zombie = (BITCODE_B)pair->value.i;
              LOG_TRACE ("CLASS[%d].is_zombie = %d [B 280]\n", i,
                         pair->value.i);
              if (is_binary)
                dat->byte++; // B => RS
              break;
            case 281: // ie is_entity
              // 1f2 for entities, 1f3 for objects
              klass->item_class_id = pair->value.i ? 0x1f2 : 0x1f3;
              LOG_TRACE ("CLASS[%d].item_class_id = 0x%x [BSx 281] (%s)\n", i,
                         klass->item_class_id,
                         pair->value.i ? "is_entity" : "is_object");
              if (is_binary)
                dat->byte++; // B => RS
              break;
            default:
              LOG_WARN ("Unknown DXF code for class[%d].%d", i, pair->code);
              break;
            }
          dxf_free_pair (pair);
          pair = dxf_read_pair (dat);
          if (!pair)
            {
              pair = dxf_read_pair (dat);
              if (!pair)
                return 1;
            }
        }
      if (klass->dxfname && klass->cppname && klass->appname
          && klass->item_class_id)
        {
          dwg->num_classes++;
          DXF_RETURN_ENDSEC (0)
        }
      else
        {
          DXF_RETURN_ENDSEC (0);
          goto restart; // without alloc
        }
    }
  dxf_free_pair (pair);
  return 0;
}

static void
add_eed (Dwg_Object *restrict obj, const char *restrict name,
         Dxf_Pair *restrict pair)
{
  int code, size = 0, j;
  int i, prev = 0;
  Dwg_Eed *eed;
  Dwg_Data *dwg = obj->parent;

  i = obj->tio.object->num_eed; // same layout for Object and Entity
  eed = obj->tio.object->eed;

  // new eed pair
  if (pair->code < 1020 || pair->code > 1035) // no followup y and z pairs
    {
      if (i || eed)
        {
          eed = (Dwg_Eed *)realloc (eed, (i + 1) * sizeof (Dwg_Eed));
          if (!eed)
            {
              LOG_ERROR ("Out of memory");
              dwg_free_eed (obj);
              return;
            }
          if (i)
            memset (&eed[i], 0, sizeof (Dwg_Eed));
        }
      else
        {
          eed = (Dwg_Eed *)xcalloc (1, sizeof (Dwg_Eed));
          if (!eed)
            {
              LOG_ERROR ("Out of memory");
              dwg_free_eed (obj);
              return;
            }
        }
      obj->tio.object->eed = eed;
      obj->tio.object->num_eed++;
    }
  else // add to old eed
    i--;
  // search for previous size index
  for (j = i; j >= 0; j--)
    if (eed[j].handle.code)
      prev = j;
  if (!(prev >= 0 && prev <= i))
    {
      LOG_ERROR ("Invalid EED, no prev %d size 1000 code", prev);
      dwg_free_eed (obj);
      return;
    }
  assert (prev >= 0 && prev <= i);
  code = pair->code - 1000; // 1000
  if (code < 0 || code >= 1000)
    {
      LOG_ERROR ("Invalid DXF code %d", pair->code);
      dwg_free_eed (obj);
      return;
    }
  assert (code >= 0 && code < 100);
  LOG_TRACE ("EED[%d] code: %d ", i, code);
  switch (code)
    {
    case 0:
      {
        int len = pair->value.s ? strlen (pair->value.s) & INT_MAX : 0;
        if (dwg->header.version < R_2007)
          {
            /* code [RC] + len [RS] + cp [RS] + str[len] */
            size = 1 + 2 + 2 + len;
            eed[i].data = (Dwg_Eed_Data *)xcalloc (1, size + 1);
            if (!eed[i].data)
              {
                LOG_ERROR ("Out of memory");
                dwg_free_eed (obj);
                return;
              }
            eed[i].data->code = code; // 1000
            eed[i].data->u.eed_0.length = len & 0xFFFF;
            eed[i].data->u.eed_0.is_tu = 0;
            eed[i].data->u.eed_0.codepage = dwg->header.codepage;
            if (len && len < 256)
              {
                LOG_TRACE ("string: \"%s\" [TV %d]\n", pair->value.s,
                           size - 1);
                // FIXME buffer overflow with dwgwrite example_2010.dxf
                memcpy (eed[i].data->u.eed_0.string, pair->value.s, len + 1);
              }
          }
        else
          {
            /* code [RC] + length [RS] + 2*len [TU] */
            if (len && len < 32767)
              {
                BITCODE_TU tu = bit_utf8_to_TU (pair->value.s, 0);
                len = bit_wcs2len (tu) & 0x0FFFFFFF;
                size = 1 + 2 + 2 + (len * 2); // now with padding
                eed[i].data = (Dwg_Eed_Data *)xcalloc (1, size + 2);
                if (!eed[i].data)
                  {
                    LOG_ERROR ("Out of memory");
                    dwg_free_eed (obj);
                    return;
                  }
                eed[i].data->code = code;
                eed[i].data->u.eed_0_r2007.length = len & 0xFFFF;
                eed[i].data->u.eed_0.is_tu = 1;
                LOG_TRACE ("wstring: \"%s\" [TU %d]\n", pair->value.s, len);
                if (len)
                  memcpy (eed[i].data->u.eed_0_r2007.string, tu,
                          2 * (len + 1));
                free (tu);
              }
          }
        eed[i].size += size;
      }
      break;
    // 1001 is the name of the APPID handle, not part of size nor data
    case 1:
      obj->tio.object->num_eed--;
      prev = i;
      if (!pair->value.s || !*pair->value.s)
        {
          LOG_ERROR ("Invalid empty DXF code 1001");
          dwg_free_eed (obj);
          return;
        }
      if (strEQc (pair->value.s, "ACAD"))
        {
          dwg_add_handle (&eed[i].handle, 5, 0x12, NULL);
          LOG_TRACE ("handle: 5.1.12 [H] for APPID.%s\n", pair->value.s);
        }
      else
        {
          // search name in APPID table (if already added)
          BITCODE_H hdl;
          hdl = dwg_find_tablehandle_silent (dwg, pair->value.s, "APPID");
          if (hdl)
            {
              memcpy (&eed[i].handle, &hdl->handleref, sizeof (Dwg_Handle));
              eed[i].handle.code = 5;
              LOG_TRACE ("handle: " FORMAT_HV " [H] for APPID.%s\n",
                         hdl->absolute_ref, pair->value.s);
            }
          // needs to be postponed, because we don't have the tables yet
          else
            {
              char idx[12];
              snprintf (idx, 12, "%d", obj->index);
              eed[i].handle.code = 5;
              eed_hdls = array_push (eed_hdls, idx, pair->value.s, i);
              LOG_TRACE ("handle: ? [H} for APPID.%s later\n", pair->value.s);
            }
        }
      break;
    case 2: // 1002 . "{" => 0, or 1002 . "}" => 1
      /* code [RC] + close [RC] */
      size = 1 + 1;
      eed[i].data = (Dwg_Eed_Data *)xcalloc (1, size);
      if (!eed[i].data)
        {
          LOG_ERROR ("Out of memory");
          dwg_free_eed (obj);
          return;
        }
      eed[i].data->code = code;
      eed[i].data->u.eed_2.close = strEQc (pair->value.s, "{") ? 0 : 1;
      LOG_TRACE ("close: %d\n", eed[i].data->u.eed_2.close);
      eed[i].size += size;
      break;
    case 4:
      {
        // BINARY
        // const char *pos = pair->value.s;
        // const size_t len = strlen (pair->value.s);
        const size_t blen = strlen (pair->value.s) >> 1;
        size_t read;
        /* code [RC] + len+0 + length [RC] */
        size = 1 + (blen & INT_MAX) + 1 + 1;
        eed[i].data = (Dwg_Eed_Data *)xcalloc (1, size);
        if (!eed[i].data)
          {
            LOG_ERROR ("Out of memory");
            dwg_free_eed (obj);
            return;
          }
        eed[i].data->code = code; // 1004
        eed[i].data->u.eed_4.length = blen & 0xFF;
        LOG_TRACE ("binary[%" PRIuSIZE "]: ", blen);
        if ((read = in_hex2bin (eed[i].data->u.eed_4.data, pair->value.s, blen)
                    != blen))
          LOG_ERROR ("in_hex2bin read only %" PRIuSIZE " of %" PRIuSIZE, read,
                     blen);
        eed[i].size += size;
      }
      break;
    case 10: // DWG_VT_POINT3D
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
      /* code [RC] + 3*RD */
      size = 1 + (3 * 8);
      eed[i].data = (Dwg_Eed_Data *)xcalloc (1, size);
      if (!eed[i].data)
        {
          LOG_ERROR ("Out of memory");
          dwg_free_eed (obj);
          return;
        }
      eed[i].data->code = code;
      eed[i].data->u.eed_10.point.x = pair->value.d;
      eed[i].size += size;
      break;
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
      if (i < 0)
        return;
      if (!eed[i].data || eed[i].data->code != code - 10)
        {
          LOG_ERROR ("Wrong EED DXF code %d, expected %d", code + 1000,
                     eed[i].data ? eed[i].data->code + 1010 : 1020)
          return;
        }
      eed[i].data->u.eed_10.point.y = pair->value.d;
      break;
    case 30:
    case 31:
    case 32:
    case 33:
    case 34:
    case 35:
      if (i < 0)
        return;
      if (!eed[i].data || eed[i].data->code != code - 20)
        {
          LOG_ERROR ("Wrong EED DXF code %d, expected %d", code + 1000,
                     eed[i].data ? eed[i].data->code + 1020 : 1030)
          return;
        }
      eed[i].data->u.eed_10.point.z = pair->value.d;
      LOG_TRACE ("3dpoint: (%f,%f,%f)\n", eed[i].data->u.eed_10.point.x,
                 eed[i].data->u.eed_10.point.y, pair->value.d);
      break;
    case 40:
    case 41:
    case 42:
      /* code [RC] + 3*RD */
      size = 1 + 8;
      eed[i].data = (Dwg_Eed_Data *)xcalloc (1, size);
      if (!eed[i].data)
        {
          LOG_ERROR ("Out of memory");
          dwg_free_eed (obj);
          return;
        }
      eed[i].data->code = code; // 1071
      eed[i].data->u.eed_40.real = pair->value.d;
      LOG_TRACE ("real: %f\n", pair->value.d);
      eed[i].size += size;
      break;
    case 70:
      /* code [RC] + RS */
      size = 1 + 2;
      eed[i].data = (Dwg_Eed_Data *)xcalloc (1, size);
      if (!eed[i].data)
        {
          LOG_ERROR ("Out of memory");
          dwg_free_eed (obj);
          return;
        }
      eed[i].data->code = code; // 1071
      eed[i].data->u.eed_70.rs = pair->value.i;
      LOG_TRACE ("short: %d\n", pair->value.i);
      eed[i].size += size;
      break;
    case 71:
      /* code [RC] + RL */
      size = 1 + 4;
      eed[i].data = (Dwg_Eed_Data *)xcalloc (1, size);
      if (!eed[i].data)
        {
          LOG_ERROR ("Out of memory");
          dwg_free_eed (obj);
          return;
        }
      eed[i].data->code = code; // 1071
      eed[i].data->u.eed_71.rl = pair->value.l;
      LOG_TRACE ("long: %ld\n", pair->value.l);
      eed[i].size += size;
      break;
    case 5:
      {
        // HANDLE (absref)
        const char *pos = pair->value.s;
        BITCODE_RLL l = 0;
        /* code [RC] + RLL */
        size = 1 + 8;
        eed[i].data = (Dwg_Eed_Data *)xcalloc (1, size);
        if (!eed[i].data)
          {
            LOG_ERROR ("Out of memory");
            dwg_free_eed (obj);
            return;
          }
        eed[i].data->code = code; // 1005
        sscanf (pos, FORMAT_HV, &l);
        eed[i].data->u.eed_5.entity = l;
        LOG_TRACE ("entity: " FORMAT_HV " [RLL]\n", l);
        eed[i].size += size;
        break;
      }
    default:
      LOG_ERROR ("Not yet implemented EED[%d] code %d", i, pair->code);
    }
  // new size block or not?
  if (!eed[i].handle.code)
    {
      // add to prev. size
      if (i != prev)
        {
          eed[prev].size += eed[i].size;
          eed[i].size = 0;
        }
      LOG_TRACE ("EED[%d] size: %d\n", prev, eed[prev].size);
    }
  else if (eed[i].size)
    LOG_TRACE ("EED[%d] size: %d\n", i, eed[i].size);
  return;
}

int
is_table_name (const char *restrict name)
{
  return strEQc (name, "LTYPE") || strEQc (name, "VPORT")
         || strEQc (name, "VPORT") || strEQc (name, "APPID")
         || strEQc (name, "BLOCK") || strEQc (name, "LAYER")
         || strEQc (name, "DIMSTYLE") || strEQc (name, "STYLE")
         || strEQc (name, "VIEW") || strEQc (name, "VX")
         || strEQc (name, "UCS") || strEQc (name, "BLOCK_RECORD")
         || strEQc (name, "BLOCK_HEADER");
}

#  define CHK_dashes(i, array)                                                \
    if (i < 0 || i >= (int)o->num##array || !o->array)                        \
      return NULL;                                                            \
    assert (o->array);                                                        \
    assert (i >= 0 && i < (int)o->num##array)
#  define CHK_array(i, array)                                                 \
    if (i < 0 || i >= (int)o->num_##array || !o->array)                       \
      return NULL;                                                            \
    assert (o->array);                                                        \
    assert (i >= 0 && i < (int)o->num_##array)

static Dxf_Pair *
add_LTYPE_dashes (Dwg_Object *restrict obj, Bit_Chain *restrict dat,
                  Dxf_Pair *restrict pair)
{
  Dwg_Object_LTYPE *o = obj->tio.object->tio.LTYPE;
  Dwg_Data *dwg = obj->parent;
  int num_dashes = (int)o->numdashes;
  int is_tu = 0;

  o->dashes
      = (Dwg_LTYPE_dash *)xcalloc (o->numdashes, sizeof (Dwg_LTYPE_dash));
  if (!o->dashes)
    {
      o->numdashes = 0;
      return NULL;
    }
  for (int j = -1; j < num_dashes;)
    {
      if (!pair || pair->code == 0)
        return pair;
      else if (pair->code == 49)
        {
          j++;
          CHK_dashes (j, dashes);
          o->dashes[j].length = pair->value.d;
          LOG_TRACE ("LTYPE.dashes[%d].length = %f [BD 49]\n", j,
                     pair->value.d);
          PRE (R_13b1)
          {
            o->pattern_len += pair->value.d;
          }
        }
      else if (pair->code == 74)
        {
          CHK_dashes (j, dashes);
          o->dashes[j].shape_flag = pair->value.i;
          LOG_TRACE ("LTYPE.dashes[%d].shape_flag = %d [BS 74]\n", j,
                     pair->value.i);
          if (o->dashes[j].shape_flag & 2)
            o->has_strings_area = 1;
        }
      else if (pair->code == 75)
        {
          if (j < 0)
            j++;
          CHK_dashes (j, dashes);
          o->dashes[j].complex_shapecode = pair->value.i;
          LOG_TRACE ("LTYPE.dashes[%d].complex_shapecode = %d [BS 75]\n", j,
                     pair->value.i);
        }
      else if (pair->code == 340)
        {
          if (j < 0)
            j++;
          CHK_dashes (j, dashes);
          o->dashes[j].style
              = dwg_add_handleref (obj->parent, 5, pair->value.u, obj);
          LOG_TRACE ("LTYPE.dashes[%d].style = " FORMAT_REF " [H 340]\n", j,
                     ARGS_REF (o->dashes[j].style));
        }
      else if (pair->code == 44)
        {
          CHK_dashes (j, dashes);
          o->dashes[j].x_offset = pair->value.d;
          LOG_TRACE ("LTYPE.dashes[%d].x_offset = %f [BD 44]\n", j,
                     pair->value.d);
        }
      else if (pair->code == 45)
        {
          CHK_dashes (j, dashes);
          o->dashes[j].y_offset = pair->value.d;
          LOG_TRACE ("LTYPE.dashes[%d].y_offset = %f [BD 45]\n", j,
                     pair->value.d);
        }
      else if (pair->code == 46)
        {
          CHK_dashes (j, dashes);
          o->dashes[j].scale = pair->value.d;
          LOG_TRACE ("LTYPE.dashes[%d].scale = %f [BD 46]\n", j,
                     pair->value.d);
        }
      else if (pair->code == 50)
        {
          CHK_dashes (j, dashes);
          o->dashes[j].rotation = deg2rad (pair->value.d);
          LOG_TRACE ("LTYPE.dashes[%d].rotation = %f [BD 50]\n", j,
                     o->dashes[j].rotation);
        }
      else if (pair->code == 9)
        {
          static unsigned dash_i = 0;
          is_tu = obj->parent->header.version >= R_2007;
          CHK_dashes (j, dashes);
          o->dashes[j].text = dwg_add_u8_input (obj->parent, pair->value.s);
          LOG_TRACE ("LTYPE.dashes[%d].text = %s [T 9]\n", j, pair->value.s);
          // write into strings_area
          if (!o->strings_area)
            o->strings_area = (BITCODE_TF)xcalloc (is_tu ? 512 : 256, 1);
          if (is_tu)
            {
              bit_wcs2cpy ((BITCODE_TU)&o->strings_area[dash_i],
                           (BITCODE_TU)o->dashes[j].text);
              dash_i += ((strlen (pair->value.s) * 2) & UINT_MAX) + 2;
            }
          else
            {
              strcpy ((char *)&o->strings_area[dash_i], o->dashes[j].text);
              dash_i += (strlen (pair->value.s) & UINT_MAX) + 1;
            }
        }
      else
        break; // not a Dwg_LTYPE_dash

      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
    }
  return pair;
}

static Dxf_Pair *
add_MLINESTYLE_lines (Dwg_Object *restrict obj, Bit_Chain *restrict dat,
                      Dxf_Pair *restrict pair)
{
  int num_lines = pair->value.i;
  Dwg_Object_MLINESTYLE *o = obj->tio.object->tio.MLINESTYLE;
  Dwg_Data *dwg = obj->parent;
  o->num_lines = num_lines;
  LOG_TRACE ("MLINESTYLE.num_lines = %d [RC 71]\n", num_lines);
  o->lines = (Dwg_MLINESTYLE_line *)xcalloc (num_lines,
                                             sizeof (Dwg_MLINESTYLE_line));
  if (!o->lines)
    {
      o->num_lines = 0;
      return NULL;
    }

  for (int j = -1; j < (int)num_lines;)
    {
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
      if (pair == NULL || pair->code == 0)
        return pair;
      else if (pair->code == 49)
        {
          j++;
          CHK_array (j, lines);
          o->lines[j].offset = pair->value.d;
          LOG_TRACE ("MLINESTYLE.lines[%d].offset = %f [BD 49]\n", j,
                     pair->value.d);
        }
      else if (pair->code == 62)
        {
          if (j < 0)
            j++;
          CHK_array (j, lines);
          o->lines[j].color.index = pair->value.i;
          LOG_TRACE ("MLINESTYLE.lines[%d].color.index = %d [CMC 62]\n", j,
                     pair->value.i);
        }
      else if (pair->code == 420)
        {
          if (j < 0)
            j++;
          CHK_array (j, lines);
          o->lines[j].color.rgb = pair->value.u;
          LOG_TRACE ("MLINESTYLE.lines[%d].color.rgb = %08X [CMC 420]\n", j,
                     pair->value.u);
        }
      else if (pair->code == 6)
        {
          if (j < 0)
            j++;
          CHK_array (j, lines);
          o->lines[j].lt.index = 0;
          o->lines[j].lt.ltype = NULL;
          if (strEQc (pair->value.s, "BYLAYER")
              || strEQc (pair->value.s, "ByLayer"))
            {
              // TODO SHRT_MAX, but should be -1 really
              o->lines[j].lt.index = 32767;
              LOG_TRACE ("MLINESTYLE.lines[%d].lt.index = -1 [BSd 6]\n", j);
              if (dwg->header.from_version >= R_2018)
                goto mline_hdl;
            }
          else if (strEQc (pair->value.s, "BYBLOCK")
                   || strEQc (pair->value.s, "ByBlock"))
            {
              o->lines[j].lt.index = 32766;
              LOG_TRACE ("MLINESTYLE.lines[%d].lt.index = -2 [BSd 6]\n", j);
              if (dwg->header.from_version >= R_2018)
                goto mline_hdl;
            }
          else if (strEQc (pair->value.s, "CONTINUOUS")
                   || strEQc (pair->value.s, "Continuous"))
            {
              o->lines[j].lt.index = 0;
              LOG_TRACE ("MLINESTYLE.lines[%d].lt.index = 0 [BSd 6]\n", j);
              if (dwg->header.from_version >= R_2018)
                goto mline_hdl;
            }
          else // lookup on LTYPE_CONTROL list
          mline_hdl:
            {
              BITCODE_H hdl;
              o->lines[j].lt.index
                  = (BITCODE_BSd)strtol (pair->value.s, NULL, 10);
              if (o->lines[j].lt.index)
                LOG_TRACE ("MLINESTYLE.lines[%d].lt.index = %d [BSd 6]\n", j,
                           (int)o->lines[j].lt.index);
              if ((hdl = dwg_find_tablehandle_silent (dwg, pair->value.s,
                                                      "LTYPE")))
                {
                  hdl->handleref.code = 5;
                  o->lines[j].lt.ltype = hdl;
                  LOG_TRACE ("MLINESTYLE.lines[%d].lt.ltype %s => " FORMAT_REF
                             " [H 6]\n",
                             j, pair->value.s, ARGS_REF (hdl));
                }
            }
        }
      else
        break; // not a Dwg_MLINESTYLE_line
    }
  return pair;
}

static Dxf_Pair *
new_LWPOLYLINE (Dwg_Object *restrict obj, Bit_Chain *restrict dat,
                Dxf_Pair *restrict pair)
{
  BITCODE_BL num_points = pair->value.u;
  Dwg_Entity_LWPOLYLINE *o = obj->tio.entity->tio.LWPOLYLINE;
  int j = -1;

  o->num_points = num_points;
  LOG_TRACE ("LWPOLYLINE.num_points = %u [BS 90]\n", num_points);
  o->points = (BITCODE_2RD *)xcalloc (num_points, sizeof (BITCODE_2RD));
  if (!o->points)
    {
      o->num_points = 0;
      return NULL;
    }

  while (pair != NULL && pair->code != 0)
    {
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
      if (pair == NULL || pair->code == 0)
        {
          LOG_TRACE ("LWPOLYLINE.flag = %d [BS 70]\n", o->flag);
          return pair;
        }
      else if (pair->code == 43)
        {
          o->const_width = pair->value.d;
          if (pair->value.d != 0.0)
            o->flag |= 4;
          LOG_TRACE ("LWPOLYLINE.const_width = %f [BD 43]\n", pair->value.d);
        }
      else if (pair->code == 70)
        {
          if (pair->value.i & 1) /* only if closed or not */
            o->flag |= 512;
          else if (pair->value.i & 128) /* plinegen? */
            o->flag |= 256;
          LOG_TRACE ("LWPOLYLINE.flag = %d [BS 70]\n", o->flag);
        }
      else if (pair->code == 38)
        {
          o->elevation = pair->value.d;
          if (pair->value.d != 0.0)
            o->flag |= 8;
          LOG_TRACE ("LWPOLYLINE.elevation = %f [38 BD]\n", pair->value.d);
        }
      else if (pair->code == 39)
        {
          o->thickness = pair->value.d;
          if (pair->value.d != 0.0)
            o->flag |= 2;
          LOG_TRACE ("LWPOLYLINE.thickness = %f [BD 39]\n", pair->value.d);
        }
      else if (pair->code == 210)
        {
          o->extrusion.x = pair->value.d;
        }
      else if (pair->code == 220)
        {
          o->extrusion.y = pair->value.d;
        }
      else if (pair->code == 230)
        {
          o->extrusion.z = pair->value.d;
          if (o->extrusion.x == 0.0 && o->extrusion.y == 0.0)
            o->extrusion.z = (o->extrusion.z <= 0.0) ? -1.0 : 1.0;
          if (o->extrusion.x != 0.0 || o->extrusion.y != 0.0
              || o->extrusion.z != 1.0)
            o->flag |= 1;
          LOG_TRACE ("LWPOLYLINE.extrusion = (%f, %f, %f) [BE 210]\n",
                     o->extrusion.x, o->extrusion.y, o->extrusion.z);
        }
      else if (pair->code == 10)
        {
          j++; // we always start with 10 (I hope)

#  define CHK_points                                                          \
    if (j < 0 || j >= (int)o->num_points || !o->points)                       \
      return NULL;                                                            \
    assert (o->points);                                                       \
    assert (o->num_points > 0);                                               \
    assert (j >= 0 && j < (int)o->num_points)

          CHK_points;
          o->points[j].x = pair->value.d;
        }
      else if (pair->code == 20)
        {
          CHK_points;
          LOG_TRACE ("LWPOLYLINE.points[%d] = (%f, %f) [2RD 10]\n", j,
                     o->points[j].x, pair->value.d);
          o->points[j].y = pair->value.d;
        }
      else if (pair->code == 42)
        {
          if (!o->num_bulges)
            {
              o->bulges
                  = (BITCODE_BD *)xcalloc (num_points, sizeof (BITCODE_BD));
              if (!o->bulges)
                {
                  o->num_bulges = 0;
                  return NULL;
                }
              o->num_bulges = num_points;
              o->flag |= 16;
            }
          CHK_array (j, bulges);
          o->bulges[j] = pair->value.d;
          LOG_TRACE ("LWPOLYLINE.bulges[%d] = %f [BD 42]\n", j, pair->value.d);
        }
      else if (pair->code == 91)
        {
          if (!j)
            {
              o->vertexids
                  = (BITCODE_BL *)xcalloc (num_points, sizeof (BITCODE_BL));
              if (!o->vertexids)
                {
                  o->num_vertexids = 0;
                  return NULL;
                }
              o->num_vertexids = num_points;
            }
          CHK_array (j, vertexids);
          o->vertexids[j] = pair->value.i;
          LOG_TRACE ("LWPOLYLINE.vertexids[%d] = %d [BL 91]\n", j,
                     pair->value.i);
        }
      else if (pair->code == 40) // not const_width
        {
          if (!j)
            {
              o->widths = (Dwg_LWPOLYLINE_width *)xcalloc (
                  num_points, sizeof (Dwg_LWPOLYLINE_width));
              if (!o->widths)
                {
                  o->num_widths = 0;
                  return NULL;
                }
              o->flag |= 4;
              o->num_widths = num_points;
            }
          CHK_array (j, widths);
          o->widths[j].start = pair->value.d;
          LOG_TRACE ("LWPOLYLINE.widths[%d].start = %f [BD 40]\n", j,
                     pair->value.d);
        }
      else if (pair->code == 41 && (o->flag & 4)) // not const_width
        {
          CHK_array (j, widths);
          o->widths[j].end = pair->value.d;
          LOG_TRACE ("LWPOLYLINE.widths[%d].end = %f [BD 41]\n", j,
                     pair->value.d);
        }
      else if (pair->code >= 1000 && pair->code < 1999)
        {
          add_eed (obj, "LWPOLYLINE", pair);
        }
      else
        {
          LOG_ERROR ("Invalid DXF code %d for %s", pair->code, "LWPOLYLINE");
          return NULL;
        }
    }
  return pair;
}
#  undef CHK_points

// only code 1
static Dxf_Pair *
add_3DSOLID_encr (Dwg_Object *restrict obj, Bit_Chain *restrict dat,
                  Dxf_Pair *restrict pair)
{
  Dwg_Entity_3DSOLID *o = obj->tio.entity->tio._3DSOLID;
  size_t i = 0;
  size_t total = 0;
  o->num_blocks = 1;
  o->encr_sat_data = (char **)xcalloc (2, sizeof (char *));
  if (!o->encr_sat_data)
    {
      o->num_blocks = 0;
      return NULL;
    }
  o->encr_sat_data[0] = NULL;
  o->block_size = (BITCODE_BL *)xcalloc (2, sizeof (BITCODE_BL));
  if (!o->block_size)
    {
      o->num_blocks = 0;
      return NULL;
    }

  while (pair != NULL && (pair->code == 1 || pair->code == 3))
    {
      size_t len;
      if (!pair->value.s)
        {
          dxf_free_pair (pair);
          pair = dxf_read_pair (dat);
          continue;
        }
      len = strlen (pair->value.s) + 1; // + the \n
      if (len > 100000)                 // chunked into blocks of size 4096
        {
          LOG_ERROR ("Overlarge DXF string len %" PRIuSIZE ": %s", len,
                     pair->value.s);
          return NULL;
        }
      if (!total || !o->encr_sat_data[0])
        {
          total = len;
          o->encr_sat_data[0] = (char *)malloc (total + 1); // + the \0
          if (!o->encr_sat_data[0])
            {
              LOG_ERROR ("Out of memory");
              return NULL;
            }
          // memcpy (o->encr_sat_data[0], pair->value.s, len + 1);
          strcpy ((char *)o->encr_sat_data[0], pair->value.s);
        }
      else
        {
          total += len;
          o->encr_sat_data[0]
              = (char *)realloc (o->encr_sat_data[0], total + 1);
          if (!o->encr_sat_data[0])
            {
              LOG_ERROR ("Out of memory");
              return NULL;
            }
          strcat ((char *)o->encr_sat_data[0], pair->value.s);
        }
      if (pair->code == 1)
        {
          strcat ((char *)o->encr_sat_data[0], "\n");
          // For the replacement of "^ " with "^" see below
          i++;
        }

      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
    }
  LOG_TRACE ("%s.block_size[0]: %" PRIuSIZE "\n", obj->name, total);

  if (o->version == 1)
    {
      int idx = 0;
      o->unknown = 1; // ??
      o->acis_data = (BITCODE_RC *)xcalloc (1, total + 1);
      if (!o->acis_data)
        return NULL;
      for (i = 0; i < total; i++)
        {
          if (o->encr_sat_data[0][i] == '^' && i <= total
              && o->encr_sat_data[0][i + 1] == ' ')
            {
              o->acis_data[idx++] = 'A';
              i++;
            }
          else if (o->encr_sat_data[0][i] <= 32)
            o->acis_data[idx++] = o->encr_sat_data[0][i];
          else
            o->acis_data[idx++] = 159 - o->encr_sat_data[0][i];
        }
      o->acis_data[idx] = '\0';
      o->block_size[0] = idx;
      LOG_TRACE ("%s.acis_data:\n%s\n", obj->name, o->acis_data);
    }

  return pair;
}

static Dxf_Pair *
add_MESH (Dwg_Object *restrict obj, Bit_Chain *restrict dat,
          Dxf_Pair *restrict pair)
{
  Dwg_Entity_MESH *o = obj->tio.entity->tio.MESH;
  int j = 0;
  int vector = 0;

  // valid entry code: 91
  if (pair->code == 91)
    {
      vector = pair->code;
      o->num_subdiv_vertex = pair->value.u;
      LOG_TRACE ("MESH.num_subdiv_vertex = %u [BL 91]\n", pair->value.u);
      if (pair->value.u)
        {
          o->subdiv_vertex = (BITCODE_3BD *)xcalloc (o->num_subdiv_vertex,
                                                     sizeof (BITCODE_3BD));
          if (!o->subdiv_vertex)
            {
              o->num_subdiv_vertex = 0;
              return NULL;
            }
        }
    }

  while (pair->code != 0)
    {
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);

      if (pair == NULL || pair->code == 0)
        return pair;
      else if (pair->code == 92)
        {
          j = 0;
          vector = pair->code;
          o->num_vertex = pair->value.u;
          LOG_TRACE ("MESH.num_vertex = %u [BL 92]\n", pair->value.u);
          if (pair->value.u)
            {
              o->vertex = (BITCODE_3BD *)xcalloc (o->num_vertex,
                                                  sizeof (BITCODE_3BD));
              if (!o->vertex)
                {
                  o->num_vertex = 0;
                  return NULL;
                }
            }
        }
      else if (pair->code == 93)
        {
          j = 0;
          vector = pair->code;
          o->num_faces = pair->value.u;
          LOG_TRACE ("MESH.num_faces = %u [BL %d]\n", pair->value.u,
                     pair->code);
          if (pair->value.u)
            {
              o->faces
                  = (BITCODE_BL *)xcalloc (o->num_faces, sizeof (BITCODE_BL));
              if (!o->faces)
                {
                  o->num_faces = 0;
                  return NULL;
                }
            }
        }
      else if (pair->code == 94)
        {
          j = 0;
          vector = pair->code;
          o->num_edges = pair->value.u;
          LOG_TRACE ("MESH.num_edges = %u [BL %d]\n", pair->value.u,
                     pair->code);
          if (pair->value.u) // from face - to face
            {
              o->edges = (Dwg_MESH_edge *)xcalloc (o->num_edges,
                                                   sizeof (Dwg_MESH_edge));
              if (!o->edges)
                {
                  o->num_edges = 0;
                  return NULL;
                }
            }
        }
      else if (pair->code == 95)
        {
          j = 0;
          vector = pair->code;
          o->num_crease = pair->value.u;
          LOG_TRACE ("MESH.num_crease = %u [BL %d]\n", pair->value.u,
                     pair->code);
          if (pair->value.u)
            {
              o->crease
                  = (BITCODE_BD *)xcalloc (o->num_crease, sizeof (BITCODE_BD));
              if (!o->crease)
                {
                  o->num_crease = 0;
                  return NULL;
                }
            }
        }
      else if (pair->code == 10)
        {
          if (vector == 91)
            {
              CHK_array (j, subdiv_vertex);
              o->subdiv_vertex[j].x = pair->value.d;
            }
          else if (vector == 92)
            {
              CHK_array (j, vertex);
              o->vertex[j].x = pair->value.d;
            }
          else
            goto mesh_error;
        }
      else if (pair->code == 20)
        {
          if (vector == 91)
            {
              CHK_array (j, subdiv_vertex);
              o->subdiv_vertex[j].y = pair->value.d;
            }
          else if (vector == 92)
            {
              CHK_array (j, vertex);
              o->vertex[j].y = pair->value.d;
            }
          else
            goto mesh_error;
        }
      else if (pair->code == 30)
        {
          if (vector == 91)
            {
              CHK_array (j, subdiv_vertex);
              o->subdiv_vertex[j].z = pair->value.d;
              LOG_TRACE ("MESH.subdiv_vertex[%d] = (%f, %f, %f) [3BD 10]\n", j,
                         o->subdiv_vertex[j].x, o->subdiv_vertex[j].y,
                         o->subdiv_vertex[j].z);
              j++;
            }
          else if (vector == 92)
            {
              CHK_array (j, vertex);
              o->vertex[j].z = pair->value.d;
              LOG_TRACE ("MESH.vertex[%d] = (%f, %f, %f) [3BD 10]\n", j,
                         o->vertex[j].x, o->vertex[j].y, o->vertex[j].z);
              j++;
            }
          else
            goto mesh_error;
        }
      else if (pair->code == 90)
        {
          if (vector == 93)
            {
              CHK_array (j, faces);
              o->faces[j] = pair->value.u;
              LOG_TRACE ("MESH.faces[%d] = %u [BL %d]\n", j, pair->value.u,
                         pair->code);
              j++;
            }
          else if (vector == 94)
            {
              int i = j / 2;
              CHK_array (i, edges);
              assert (j < (int)(2 * o->num_edges));
              if (j % 2 == 0)
                {
                  o->edges[i].idxfrom = pair->value.u;
                }
              else
                {
                  o->edges[i].idxto = pair->value.u;
                  LOG_TRACE ("MESH.edges[%d] = (%u, %u) [2BL %d]\n", i,
                             o->edges[i].idxfrom, pair->value.u, pair->code);
                }
              j++;
            }
          else if (vector == 95)
            {
              o->num_crease = pair->value.u;
              LOG_TRACE ("MESH.num_crease = %u [BL %d]\n", pair->value.u,
                         pair->code);
              if (pair->value.u) // from face - to face
                {
                  o->crease
                      = (double *)xcalloc (o->num_crease, sizeof (double));
                  if (!o->crease)
                    {
                      o->num_crease = 0;
                      return NULL;
                    }
                }
            }
          else
            goto mesh_error;
        }
      else if (pair->code == 140)
        {
          if (vector == 95)
            {
              CHK_array (j, crease);
              o->crease[j] = pair->value.u;
              LOG_TRACE ("MESH.crease[%d] = %u [BD %d]\n", j, pair->value.u,
                         pair->code);
              j++;
            }
          else
            goto mesh_error;
        }
      else if (pair->code >= 1000 && pair->code < 1999)
        {
          add_eed (obj, "MESH", pair);
        }
      else
        {
        mesh_error:
          LOG_ERROR ("Unknown DXF code %d for %s", pair->code, "MESH");
        }
    }
  return pair;
}

static Dxf_Pair *
add_HATCH (Dwg_Object *restrict obj, Bit_Chain *restrict dat,
           Dxf_Pair *restrict pair)
{
  BITCODE_BL num_paths; // 91
  Dwg_Entity_HATCH *o = obj->tio.entity->tio.HATCH;
  int is_plpath = 0;
  int j = -1;
  int k = -1;
  int l = -1;
  int hdl_idx = -1;
  bool next_330_boundary_handles = false;

  // valid entry codes
  if (pair->code == 91)
    {
      o->num_paths = pair->value.u;
      LOG_TRACE ("HATCH.num_paths = %u [BS 91]\n", o->num_paths);
      o->paths
          = (Dwg_HATCH_Path *)xcalloc (o->num_paths, sizeof (Dwg_HATCH_Path));
      if (!o->paths)
        {
          o->num_paths = 0;
          dxf_free_pair (pair);
          return NULL;
        }
    }
  else if (pair->code == 78)
    {
      o->num_deflines = pair->value.l;
      LOG_TRACE ("HATCH.num_deflines = %ld [BS 78]\n", pair->value.l);
      o->deflines = (Dwg_HATCH_DefLine *)xcalloc (pair->value.l,
                                                  sizeof (Dwg_HATCH_DefLine));
      if (!o->deflines)
        {
          o->num_deflines = 0;
          dxf_free_pair (pair);
          return NULL;
        }
    }
  if (pair->code == 453)
    {
      o->num_colors = pair->value.l;
      LOG_TRACE ("HATCH.num_colors = %ld [BL 453]\n", pair->value.l);
      o->colors = (Dwg_HATCH_Color *)xcalloc (pair->value.l,
                                              sizeof (Dwg_HATCH_Color));
      if (!o->colors)
        {
          o->num_colors = 0;
          dxf_free_pair (pair);
          return NULL;
        }
    }

  while (pair->code != 0)
    {
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);

      if (pair == NULL || pair->code == 0 || pair->code == 75)
        return pair;
      else if (pair->code == 92)
        {
          j++;

#  define CHK_paths                                                           \
    if (!o->paths || j < 0 || j >= (int)o->num_paths)                         \
      {                                                                       \
        LOG_ERROR ("HATCH no paths or wrong j %u\n", j);                      \
        if (o->paths)                                                         \
          {                                                                   \
            free (o->paths);                                                  \
            o->paths = NULL;                                                  \
          }                                                                   \
        o->num_paths = 0;                                                     \
        dxf_free_pair (pair);                                                 \
        return NULL;                                                          \
      }                                                                       \
    assert (o->paths);                                                        \
    assert (j >= 0);                                                          \
    assert (j < (int)o->num_paths)

          CHK_paths;
          o->paths[j].flag = pair->value.u;
          LOG_TRACE ("HATCH.paths[%d].flag = %u [BL 92]\n", j, pair->value.u);
          is_plpath = pair->value.u & 2;
          o->has_derived = pair->value.u & 4;
          LOG_TRACE ("HATCH.has_derived = %d [B 0]\n", o->has_derived);
        }
      else if (pair->code == 93)
        {
          CHK_paths;
          o->paths[j].num_segs_or_paths = pair->value.u;
          LOG_TRACE ("HATCH.paths[%d].num_segs_or_paths = %u [BL 93]\n", j,
                     pair->value.u);
          k = -1;
          if (pair->value.u && !is_plpath)
            { /* segs */
              o->paths[j].segs = (Dwg_HATCH_PathSeg *)xcalloc (
                  pair->value.u, sizeof (Dwg_HATCH_PathSeg));
              if (!o->paths[j].segs)
                {
                  o->paths[j].num_segs_or_paths = 0;
                  dxf_free_pair (pair);
                  return NULL;
                }
            }
          else if (pair->value.u)
            { /* polyline path */
              o->paths[j].polyline_paths = (Dwg_HATCH_PolylinePath *)xcalloc (
                  pair->value.u, sizeof (Dwg_HATCH_PolylinePath));
              if (!o->paths[j].polyline_paths)
                {
                  o->paths[j].num_segs_or_paths = 0;
                  dxf_free_pair (pair);
                  return NULL;
                }
            }
        }
      else if (pair->code == 72)
        {
          CHK_paths;
#  define CHK_segs                                                            \
    if (!o->paths[j].segs || k < 0                                            \
        || k >= (int)o->paths[j].num_segs_or_paths)                           \
      {                                                                       \
        LOG_ERROR ("HATCH no paths[%d].segs or wrong k %d\n", j, k);          \
        if (o->paths && o->paths[j].segs)                                     \
          {                                                                   \
            free (o->paths[j].segs);                                          \
            o->paths[j].segs = NULL;                                          \
          }                                                                   \
        o->paths[j].num_segs_or_paths = 0;                                    \
        dxf_free_pair (pair);                                                 \
        return NULL;                                                          \
      }                                                                       \
    assert (o->paths[j].segs);                                                \
    assert (k >= 0);                                                          \
    assert (k < (int)o->paths[j].num_segs_or_paths)

          if (!is_plpath)
            {
              k++;
              CHK_segs;
              if (j < (int)o->num_paths)
                {
                  o->paths[j].segs[k].curve_type = pair->value.i;
                  LOG_TRACE (
                      "HATCH.paths[%d].segs[%d].curve_type = %d [RC 72]\n", j,
                      k, pair->value.i);
                }
            }
          else
            {
              o->paths[j].bulges_present = pair->value.i;
              LOG_TRACE ("HATCH.paths[%d].bulges_present = %d [RC 72]\n", j,
                         pair->value.i);
            }
        }
      else if (pair->code == 73 && is_plpath && pair->value.i)
        {
          CHK_paths;
          o->paths[j].closed = pair->value.i;
          LOG_TRACE ("HATCH.paths[%d].closed = %d [RC 73]\n", j,
                     pair->value.i);
        }
      else if (pair->code == 94 && !is_plpath && pair->value.l)
        {
          CHK_paths;
          CHK_segs;
          o->paths[j].segs[k].degree = pair->value.l;
          LOG_TRACE ("HATCH.paths[%d].segs[%d].degree = %ld [BL 94]\n", j, k,
                     pair->value.l);
        }
      else if (pair->code == 74 && !is_plpath)
        {
          CHK_paths;
          CHK_segs;
          o->paths[j].segs[k].is_periodic = pair->value.i;
          LOG_TRACE ("HATCH.paths[%d].segs[%d].is_periodic = %d [B 74]\n", j,
                     k, pair->value.i);
        }
      else if (pair->code == 95 && !is_plpath)
        {
          CHK_paths;
          CHK_segs;
          o->paths[j].segs[k].num_knots = pair->value.l;
          LOG_TRACE ("HATCH.paths[%d].segs[%d].num_knots = %ld [BL 95]\n", j,
                     k, pair->value.l);
          o->paths[j].segs[k].knots
              = (double *)xcalloc (pair->value.l, sizeof (double));
          if (!o->paths[j].segs[k].knots)
            {
              o->paths[j].segs[k].num_knots = 0;
              dxf_free_pair (pair);
              return NULL;
            }
          l = -1;
        }
      else if (pair->code == 96 && !is_plpath)
        {
          CHK_paths;
          CHK_segs;
          o->paths[j].segs[k].num_control_points = pair->value.l;
          LOG_TRACE (
              "HATCH.paths[%d].segs[%d].num_control_points = %ld [BL 96]\n", j,
              k, pair->value.l);
          o->paths[j].segs[k].control_points
              = (Dwg_HATCH_ControlPoint *)xcalloc (
                  pair->value.l, sizeof (Dwg_HATCH_ControlPoint));
          if (!o->paths[j].segs[k].control_points)
            {
              o->paths[j].segs[k].num_control_points = 0;
              dxf_free_pair (pair);
              return NULL;
            }
          l = -1;
        }
      else if (pair->code == 10 && !is_plpath && !o->num_seeds)
        {
          CHK_paths;
          CHK_segs;
          switch (o->paths[j].segs[k].curve_type)
            {
            case 1: /* LINE */
              o->paths[j].segs[k].first_endpoint.x = pair->value.d;
              // LOG_TRACE ("HATCH.paths[%d].segs[%d].first_endpoint.x = %f [10
              // 2BD]\n",
              //           j, k, pair->value.d);
              break;
            case 2: /* CIRCULAR ARC */
            case 3: /* ELLIPTICAL ARC */
              o->paths[j].segs[k].center.x = pair->value.d;
              // LOG_TRACE ("HATCH.paths[%d].segs[%d].center.x = %f [10
              // 2BD]\n",
              //           j, k, pair->value.d);
              break;
            case 4: /* SPLINE */
              l++;

#  define CHK_control_points                                                  \
    if (!o->paths[j].segs || l < 0                                            \
        || l >= (int)o->paths[j].segs[k].num_control_points)                  \
      {                                                                       \
        LOG_ERROR ("HATCH no paths[%d].segs or "                              \
                   "wrong l %d control_points index\n",                       \
                   j, l);                                                     \
        if (!o->paths[j].segs)                                                \
          o->paths[j].num_segs_or_paths = 0;                                  \
        else                                                                  \
          {                                                                   \
            free (o->paths[j].segs[k].control_points);                        \
            o->paths[j].segs[k].control_points = NULL;                        \
            o->paths[j].segs[k].num_control_points = 0;                       \
          }                                                                   \
        dxf_free_pair (pair);                                                 \
        return NULL;                                                          \
      }                                                                       \
    assert (l >= 0);                                                          \
    assert (l < (int)o->paths[j].segs[k].num_control_points)

#  define CHK_fitpts                                                          \
    if (!o->paths[j].segs || l < 0                                            \
        || l >= (int)o->paths[j].segs[k].num_fitpts)                          \
      {                                                                       \
        LOG_ERROR ("HATCH no paths[%d].segs or "                              \
                   "wrong l %d fitpts index\n",                               \
                   j, l);                                                     \
        dxf_free_pair (pair);                                                 \
        return NULL;                                                          \
      }                                                                       \
    assert (l >= 0);                                                          \
    assert (l < (int)o->paths[j].segs[k].num_fitpts)

              CHK_control_points;
              o->paths[j].segs[k].control_points[l].point.x = pair->value.d;
              // LOG_TRACE
              // ("HATCH.paths[%d].segs[%d].control_points[%d].point.x = %f [10
              // 2BD]\n",
              //           j, k, l, pair->value.d);
              break;
            default:
              LOG_WARN ("Unhandled HATCH.paths[%d].segs[%d].curve_type %d "
                        "for DXF %d",
                        j, k, o->paths[j].segs[k].curve_type, pair->code);
            }
        }
      else if (pair->code == 11 && !is_plpath && !o->num_seeds)
        {
          CHK_paths;
          CHK_segs;
          switch (o->paths[j].segs[k].curve_type)
            {
            case 1: /* LINE */
              o->paths[j].segs[k].second_endpoint.x = pair->value.d;
              // LOG_TRACE ("HATCH.paths[%d].segs[%d].second_endpoint.x = %f
              // [2BD 11]\n",
              //           j, k, pair->value.d);
              break;
            case 3: /* ELLIPTICAL ARC */
              o->paths[j].segs[k].endpoint.x = pair->value.d;
              // LOG_TRACE ("HATCH.paths[%d].segs[%d].endpoint.x = %f [
              // 2BD 11]\n",
              //           j, k, pair->value.d);
              break;
            case 4: /* SPLINE */
              l++;
              CHK_fitpts;
              if (!o->paths[j].segs[k].fitpts)
                {
                  o->paths[j].segs[k].num_fitpts = 0;
                  dxf_free_pair (pair);
                  return NULL;
                }
              o->paths[j].segs[k].fitpts[l].x = pair->value.d;
              // LOG_TRACE ("HATCH.paths[%d].segs[%d].fitpts[%d].x = %f [
              // 2RD 11]\n",
              //           j, k, l, pair->value.d);
              break;
            default:
              LOG_WARN ("Unhandled HATCH.paths[%d].segs[%d].curve_type %d "
                        "for DXF %d",
                        j, k, o->paths[j].segs[k].curve_type, pair->code);
            }
        }
      else if (pair->code == 20 && !is_plpath && !o->num_seeds)
        {
          CHK_paths;
          CHK_segs;
          switch (o->paths[j].segs[k].curve_type)
            {
            case 1: /* LINE */
              o->paths[j].segs[k].first_endpoint.y = pair->value.d;
              LOG_TRACE ("HATCH.paths[%d].segs[%d].first_endpoint = (%f, %f) "
                         "[2RD 10]\n",
                         j, k, o->paths[j].segs[k].first_endpoint.x,
                         pair->value.d);
              break;
            case 2: /* CIRCULAR ARC */
            case 3: /* ELLIPTICAL ARC */
              o->paths[j].segs[k].center.y = pair->value.d;
              LOG_TRACE (
                  "HATCH.paths[%d].segs[%d].center = (%f, %f) [2RD 10]\n", j,
                  k, o->paths[j].segs[k].center.x, pair->value.d);
              break;
            case 4: /* SPLINE */
              CHK_control_points;
              o->paths[j].segs[k].control_points[l].point.y = pair->value.d;
              LOG_TRACE ("HATCH.paths[%d].segs[%d].control_points[%d].point = "
                         "(%f, %f) [2RD 10]\n",
                         j, k, l,
                         o->paths[j].segs[k].control_points[l].point.x,
                         pair->value.d);
              break;
            default:
              LOG_WARN ("Unhandled HATCH.paths[%d].segs[%d].curve_type %d "
                        "for DXF %d",
                        j, k, o->paths[j].segs[k].curve_type, pair->code);
            }
        }
      else if (pair->code == 21 && !is_plpath && !o->num_seeds
               && pair->value.d != 0.0)
        {
          CHK_paths;
          CHK_segs;
          switch (o->paths[j].segs[k].curve_type)
            {
            case 1: /* LINE */
              o->paths[j].segs[k].second_endpoint.y = pair->value.d;
              LOG_TRACE ("HATCH.paths[%d].segs[%d].second_endpoint = (%f, %f) "
                         "[2RD 11]\n",
                         j, k, o->paths[j].segs[k].second_endpoint.x,
                         pair->value.d);
              break;
            case 3: /* ELLIPTICAL ARC */
              o->paths[j].segs[k].endpoint.y = pair->value.d;
              LOG_TRACE (
                  "HATCH.paths[%d].segs[%d].endpoint = (%f, %f) [2RD 11]\n", j,
                  k, o->paths[j].segs[k].endpoint.x, pair->value.d);
              break;
            case 4: /* SPLINE */
              CHK_fitpts;
              o->paths[j].segs[k].fitpts[l].y = pair->value.d;
              LOG_TRACE ("HATCH.paths[%d].segs[%d].fitpts[%d].y = (%f, %f) "
                         "[2RD 11]\n",
                         j, k, l, o->paths[j].segs[k].fitpts[l].x,
                         pair->value.d);
              break;
            default:
              LOG_WARN ("Unhandled HATCH.paths[%d].segs[%d].curve_type %d "
                        "for DXF %d",
                        j, k, o->paths[j].segs[k].curve_type, pair->code);
            }
        }
      else if (pair->code == 40 && !is_plpath)
        {
          CHK_paths;
          CHK_segs;
          switch (o->paths[j].segs[k].curve_type)
            {
            case 2: /* CIRCULAR ARC */
              o->paths[j].segs[k].radius = pair->value.d;
              LOG_TRACE ("HATCH.paths[%d].segs[%d].radius = %f [BD 40]\n", j,
                         k, pair->value.d);
              break;
            case 3: /* ELLIPTICAL ARC */
              o->paths[j].segs[k].minor_major_ratio = pair->value.d;
              LOG_TRACE (
                  "HATCH.paths[%d].segs[%d].minor_major_ratio = %f [BD 40]\n",
                  j, k, pair->value.d);
              break;
            case 4: /* SPLINE */
              if (l >= 0 && o->paths[j].segs[k].is_rational)
                {
                  CHK_control_points;
                  o->paths[j].segs[k].control_points[l].weight = pair->value.d;
                  LOG_TRACE ("HATCH.paths[%d].segs[%d].control_points[%d]."
                             "weight = %f [BD 40]\n",
                             j, k, l, pair->value.d);
                }
              else
                {
#  define CHK_knots                                                           \
    if (!o->paths[j].segs || !o->paths[j].segs[k].knots || l < 0              \
        || l >= (int)o->paths[j].segs[k].num_knots)                           \
      {                                                                       \
        LOG_ERROR ("HATCH no paths[%d].segs[%d].knots or "                    \
                   "wrong l %d knots index\n",                                \
                   j, k, l);                                                  \
        dxf_free_pair (pair);                                                 \
        return NULL;                                                          \
      }                                                                       \
    assert (l >= 0);                                                          \
    assert (l < (int)o->paths[j].segs[k].num_knots)

                  l++;
                  CHK_knots;
                  o->paths[j].segs[k].knots[l] = pair->value.d;
                  LOG_TRACE (
                      "HATCH.paths[%d].segs[%d].knots[%d] = %f [BD 40]\n", j,
                      k, l, pair->value.d);
                  if (l == (int)o->paths[j].segs[k].num_knots - 1) // last 40
                    l = -1;
                }
              break;
            default:
              LOG_WARN ("Unhandled HATCH.paths[%d].segs[%d].curve_type %d "
                        "for DXF %d",
                        j, k, o->paths[j].segs[k].curve_type, pair->code);
            }
        }
      else if (pair->code == 50 && !is_plpath)
        {
          CHK_paths;
          CHK_segs;
          switch (o->paths[j].segs[k].curve_type)
            {
            case 2: /* CIRCULAR ARC */
            case 3: /* ELLIPTICAL ARC */
              o->paths[j].segs[k].start_angle = deg2rad (pair->value.d);
              LOG_TRACE ("HATCH.paths[%d].segs[%d].start_angle = %f [BD 50]\n",
                         j, k, pair->value.d);
              break;
            default:
              LOG_WARN ("Unhandled HATCH.paths[%d].segs[%d].curve_type %d "
                        "for DXF %d",
                        j, k, o->paths[j].segs[k].curve_type, pair->code);
            }
        }
      else if (pair->code == 51 && !is_plpath)
        {
          CHK_paths;
          CHK_segs;
          switch (o->paths[j].segs[k].curve_type)
            {
            case 2: /* CIRCULAR ARC */
            case 3: /* ELLIPTICAL ARC */
              o->paths[j].segs[k].end_angle = deg2rad (pair->value.d);
              LOG_TRACE ("HATCH.paths[%d].segs[%d].end_angle = %f [BD 51]\n",
                         j, k, pair->value.d);
              break;
            default:
              LOG_WARN ("Unhandled HATCH.paths[%d].segs[%d].curve_type %d "
                        "for DXF %d",
                        j, k, o->paths[j].segs[k].curve_type, pair->code);
            }
        }
      else if (pair->code == 73 && !is_plpath)
        {
          CHK_paths;
          CHK_segs;
          switch (o->paths[j].segs[k].curve_type)
            {
            case 2: /* CIRCULAR ARC */
            case 3: /* ELLIPTICAL ARC */
              o->paths[j].segs[k].is_ccw = pair->value.i;
              LOG_TRACE ("HATCH.paths[%d].segs[%d].is_ccw = %d [B 73]\n", j, k,
                         pair->value.i);
              break;
            default: // SPLINE 4
              o->paths[j].segs[k].is_rational = pair->value.i;
              LOG_TRACE ("HATCH.paths[%d].segs[%d].is_rational = %d [B 73]\n",
                         j, k, pair->value.i);
            }
        }
      else if (pair->code == 10 && is_plpath && !o->num_seeds)
        {
          k++;
          CHK_paths;

#  define CHK_polyline_paths                                                  \
    if (!o->paths[j].polyline_paths || k < 0                                  \
        || k >= (int)o->paths[j].num_segs_or_paths)                           \
      {                                                                       \
        LOG_ERROR ("HATCH no paths[%d].polyline_paths or wrong k %d\n", j,    \
                   k);                                                        \
        dxf_free_pair (pair);                                                 \
        return NULL;                                                          \
      }                                                                       \
    assert (o->paths[j].polyline_paths);                                      \
    assert (k >= 0);                                                          \
    assert (k < (int)o->paths[j].num_segs_or_paths)

          CHK_polyline_paths;
          o->paths[j].polyline_paths[k].point.x = pair->value.d;
          // LOG_TRACE ("HATCH.paths[%d].polyline_paths[%d].point.x = %f [BD 10
          // ]\n",
          //           j, k, pair->value.d);
        }
      else if (pair->code == 20 && is_plpath && !o->num_seeds)
        {
          CHK_paths;
          CHK_polyline_paths;
          o->paths[j].polyline_paths[k].point.y = pair->value.d;
          LOG_TRACE (
              "HATCH.paths[%d].polyline_paths[%d].point = (%f, %f) [2RD 10]\n",
              j, k, o->paths[j].polyline_paths[k].point.x, pair->value.d);
        }
      else if (pair->code == 42 && is_plpath)
        {
          CHK_paths;
          CHK_polyline_paths;
          o->paths[j].polyline_paths[k].bulge = pair->value.d;
          LOG_TRACE ("HATCH.paths[%d].polyline_paths[%d].bulge = %f [BD 42]\n",
                     j, k, pair->value.d);
        }
      else if (pair->code == 97 && !is_plpath)
        {
          CHK_paths;
          CHK_segs;
          if (k < 0 || o->paths[j].segs[k].curve_type != 4)
            {
              next_330_boundary_handles = true;
              o->paths[j].num_boundary_handles = pair->value.l;
              // o->num_boundary_handles += pair->value.l;
              LOG_TRACE (
                  "HATCH.paths[%d].num_boundary_handles = %ld [BL 97]\n", j,
                  pair->value.l);
              k = 0;
            }
          else
            {
              next_330_boundary_handles = false;
              o->paths[j].segs[k].num_fitpts = pair->value.l;
              LOG_TRACE (
                  "HATCH.paths[%d].segs[%d].num_fitpts  = %ld [BL 97]\n", j, k,
                  pair->value.l);
              o->paths[j].segs[k].fitpts = (BITCODE_2RD *)xcalloc (
                  pair->value.l, sizeof (BITCODE_2RD));
            }
        }
      else if (pair->code == 97 && is_plpath)
        {
          CHK_paths;
          o->paths[j].num_boundary_handles = pair->value.l;
          next_330_boundary_handles = true;
          // o->num_boundary_handles += pair->value.l;
          LOG_TRACE (
              "HATCH.paths[%d].num_boundary_handles = %ld [BL 97] (1)\n", j,
              pair->value.l);
          k = 0;
        }
      else if (pair->code == 78)
        {
          o->num_deflines = pair->value.l;
          LOG_TRACE ("HATCH.num_deflines = %ld [BS 78]\n", pair->value.l);
          o->deflines = (Dwg_HATCH_DefLine *)xcalloc (
              pair->value.l, sizeof (Dwg_HATCH_DefLine));
          if (!o->deflines)
            {
              o->num_deflines = 0;
              dxf_free_pair (pair);
              return NULL;
            }
          j = -1;
        }
      else if (pair->code == 53 && o->num_deflines)
        {
          j++;

#  define CHK_deflines                                                        \
    if (!o->deflines || j < 0 || j >= (int)o->num_deflines)                   \
      {                                                                       \
        LOG_ERROR ("HATCH no deflines or wrong j %d", j);                     \
        dxf_free_pair (pair);                                                 \
        return NULL;                                                          \
      }                                                                       \
    assert (j >= 0);                                                          \
    assert (j < (int)o->num_deflines)

          CHK_deflines;
          o->deflines[j].angle = deg2rad (pair->value.d);
          LOG_TRACE ("HATCH.deflines[%d].angle = %f [BD 53]\n", j,
                     o->deflines[j].angle);
        }
      else if (pair->code == 43 && o->num_deflines)
        {
          CHK_deflines;
          o->deflines[j].pt0.x = pair->value.d;
          LOG_TRACE ("HATCH.deflines[%d].pt0.x = %f [BD 43]\n", j,
                     pair->value.d);
        }
      else if (pair->code == 44 && o->num_deflines)
        {
          CHK_deflines;
          o->deflines[j].pt0.y = pair->value.d;
          LOG_TRACE ("HATCH.deflines[%d].pt0.y = %f [BD 44]\n", j,
                     pair->value.d);
        }
      else if (pair->code == 45 && o->num_deflines)
        {
          CHK_deflines;
          o->deflines[j].offset.x = pair->value.d;
          LOG_TRACE ("HATCH.deflines[%d].offset.x = %f [BD 45]\n", j,
                     pair->value.d);
        }
      else if (pair->code == 46 && o->num_deflines)
        {
          CHK_deflines;
          o->deflines[j].offset.y = pair->value.d;
          LOG_TRACE ("HATCH.deflines[%d].offset.y = %f [BD 46]\n", j,
                     pair->value.d);
        }
      else if (pair->code == 79 && o->num_deflines)
        {
          CHK_deflines;
          o->deflines[j].num_dashes = pair->value.u;
          LOG_TRACE ("HATCH.deflines[%d].num_dashes = %u [BS 79]\n", j,
                     pair->value.u);
          if (pair->value.u)
            {
              o->deflines[j].dashes
                  = (BITCODE_BD *)xcalloc (pair->value.u, sizeof (BITCODE_BD));
              if (!o->deflines[j].dashes)
                {
                  o->deflines[j].num_dashes = 0;
                  dxf_free_pair (pair);
                  return NULL;
                }
            }
          k = -1;
        }
      else if (pair->code == 49 && o->num_deflines && j >= 0)
        {
          CHK_deflines;
          if (!o->deflines[j].dashes)
            {
              LOG_ERROR ("DXF 79 num_dashes missing for HATCH.dashes 49")
              goto unknown_HATCH;
            }
          k++;
          if (!o->deflines[j].dashes || k < 0
              || k >= (int)o->deflines[j].num_dashes)
            {
              LOG_ERROR ("add_HATCH dashes")
              dxf_free_pair (pair);
              return NULL;
            }
          assert (k >= 0);
          assert (k < (int)o->deflines[j].num_dashes);
          o->deflines[j].dashes[k] = pair->value.d;
          LOG_TRACE ("HATCH.deflines[%d].dashes[%d] = %f [BD 49]\n", j, k,
                     pair->value.d);
        }
      else if (pair->code == 47)
        {
          o->pixel_size = pair->value.d;
          LOG_TRACE ("HATCH.pixel_size = %f [BD 47]\n", pair->value.d);
        }
      else if (pair->code == 98)
        {
          o->num_seeds = pair->value.u;
          LOG_TRACE ("HATCH.num_seeds = %u [BL 98]\n", pair->value.u);
          if (pair->value.u)
            {
              o->seeds = (BITCODE_2RD *)xcalloc (pair->value.u,
                                                 sizeof (BITCODE_2RD));
              if (!o->seeds)
                {
                  o->num_seeds = 0;
                  dxf_free_pair (pair);
                  return NULL;
                }
            }
          k = -1;
        }
      else if (pair->code == 10 && o->num_seeds)
        {
          k++;

#  define CHK_seeds                                                           \
    if (!o->seeds || k < 0 || k >= (int)o->num_seeds)                         \
      {                                                                       \
        LOG_ERROR ("HATCH no seeds or wrong k %d", k);                        \
        dxf_free_pair (pair);                                                 \
        return NULL;                                                          \
      }                                                                       \
    assert (k >= 0);                                                          \
    assert (k < (int)o->num_seeds)

          CHK_seeds;
          o->seeds[k].x = pair->value.d;
          // LOG_TRACE ("HATCH.seeds[%d].x = %f [10 2RD]\n",
          //           k, pair->value.d);
        }
      else if (pair->code == 20 && o->num_seeds)
        {
          CHK_seeds;
          o->seeds[k].y = pair->value.d;
          LOG_TRACE ("HATCH.seeds[%d] = (%f, %f) [2RD 10]\n", k, o->seeds[k].x,
                     pair->value.d);
        }
      else if (pair->code == 330 && next_330_boundary_handles && j >= 0
               && j < (int)o->num_paths && o->paths[j].num_boundary_handles
               && (hdl_idx + 1) < (int)o->paths[j].num_boundary_handles)
        {
          BITCODE_H ref
              = dwg_add_handleref (obj->parent, 4, pair->value.u, obj);
          CHK_paths;
          hdl_idx++;
          if ((unsigned)hdl_idx > o->paths[j].num_boundary_handles)
            o->paths[j].num_boundary_handles = (unsigned)hdl_idx;
          if (!o->paths[j].boundary_handles)
            o->paths[j].boundary_handles = (BITCODE_H *)xcalloc (
                o->paths[j].num_boundary_handles, sizeof (BITCODE_H));
          else
            o->paths[j].boundary_handles = (BITCODE_H *)realloc (
                o->paths[j].boundary_handles,
                o->paths[j].num_boundary_handles * sizeof (BITCODE_H));
          LOG_TRACE ("HATCH.paths[%d].num_boundary_handles = %u\n", j,
                     (unsigned)o->paths[j].num_boundary_handles);
          if (!o->paths[j].boundary_handles)
            {
              o->paths[j].num_boundary_handles = 0;
              LOG_ERROR ("! HATCH.paths[%d].boundary_handles", j);
              dxf_free_pair (pair);
              return NULL;
            }
          o->paths[j].boundary_handles[hdl_idx] = ref;
          LOG_TRACE ("HATCH.paths[%d].boundary_handles[%d] = " FORMAT_REF
                     " [H 330]\n",
                     j, hdl_idx, ARGS_REF (ref));
        }
      else if (pair->code == 453)
        {
          o->num_colors = pair->value.u;
          LOG_TRACE ("HATCH.num_colors = %u [BL 453]\n", pair->value.u);
          if (pair->value.u)
            {
              o->colors = (Dwg_HATCH_Color *)xcalloc (
                  pair->value.u, sizeof (Dwg_HATCH_Color));
              if (!o->colors)
                {
                  o->num_colors = 0;
                  dxf_free_pair (pair);
                  return NULL;
                }
            }
          j = -1;
        }
      else if (pair->code == 463 && o->num_colors)
        {
          j++;
          if (!o->colors || j < 0 || j >= (int)o->num_colors)
            {
              LOG_ERROR ("HATCH.colors DXF %hd", pair->code);
              dxf_free_pair (pair);
              return NULL;
            }
          assert (j >= 0);
          assert (j < (int)o->num_colors);
          o->colors[j].shift_value = pair->value.d;
          LOG_TRACE ("HATCH.colors[%d].shift_value = %f [BD 463]\n", j,
                     pair->value.d);
        }
      else if (pair->code == 63 && o->num_colors)
        {
          if (!o->colors || j < 0 || j >= (int)o->num_colors)
            {
              LOG_ERROR ("HATCH.colors DXF %hd", pair->code);
              dxf_free_pair (pair);
              return NULL;
            }
          assert (j < (int)o->num_colors);
          o->colors[j].color.index = pair->value.i;
          LOG_TRACE ("HATCH.colors[%d].color.index = %u [CMC 63]\n", j,
                     pair->value.i);
        }
      else if (pair->code == 421 && o->num_colors)
        {
          if (!o->colors || j < 0 || j >= (int)o->num_colors)
            {
              LOG_ERROR ("HATCH.colors DXF %hd", pair->code);
              dxf_free_pair (pair);
              return NULL;
            }
          assert (j >= 0);
          assert (j < (int)o->num_colors);
          o->colors[j].color.rgb = pair->value.u;
          LOG_TRACE ("HATCH.colors[%d].color.rgb = %08X [CMC 421]\n", j,
                     pair->value.u);
        }
      else if (pair->code == 431 && o->num_colors)
        {
          if (!o->colors || j < 0 || j >= (int)o->num_colors)
            {
              LOG_ERROR ("HATCH.colors DXF %hd", pair->code);
              dxf_free_pair (pair);
              return NULL;
            }
          assert (j >= 0);
          assert (j < (int)o->num_colors);
          if (dat->version >= R_2007)
            o->colors[j].color.name
                = (BITCODE_T)bit_utf8_to_TU (pair->value.s, 0);
          else
            o->colors[j].color.name = strdup (pair->value.s);
          LOG_TRACE ("HATCH.colors[%d].color.name = %s [CMC 431]\n", j,
                     pair->value.s);
        }
      else if (pair->code == 470)
        {
          dwg_dynapi_entity_set_value (o, "HATCH", "gradient_name",
                                       &pair->value, 1);
          LOG_TRACE ("HATCH.gradient_name = %s [T 470]\n", pair->value.s);
        }
      else if (pair->code == 450)
        {
          o->is_gradient_fill = pair->value.u;
          LOG_TRACE ("HATCH.is_gradient_fill = %u [BL 450]\n", pair->value.u);
        }
      else if (pair->code == 451)
        {
          o->reserved = pair->value.u;
          LOG_TRACE ("HATCH.reserved = %u [BL 451]\n", pair->value.u);
        }
      else if (pair->code == 460)
        {
          o->gradient_angle = deg2rad (pair->value.d);
          LOG_TRACE ("HATCH.gradient_angle = %f [BD 460]\n", pair->value.d);
        }
      else if (pair->code == 461)
        {
          o->gradient_shift = pair->value.d;
          LOG_TRACE ("HATCH.gradient_shift = %f [BD 461]\n", pair->value.d);
        }
      else if (pair->code == 452)
        {
          o->single_color_gradient = pair->value.u;
          LOG_TRACE ("HATCH.single_color_gradient = %u [BL 452]\n",
                     pair->value.u);
        }
      else if (pair->code == 462)
        {
          o->gradient_tint = pair->value.d;
          LOG_TRACE ("HATCH.gradient_tint = %f [BD 462]\n", pair->value.d);
        }
      else if (pair->code >= 1000 && pair->code < 1999)
        {
          add_eed (obj, "HATCH", pair);
        }
      else
        {
        unknown_HATCH:
          LOG_ERROR ("Unknown DXF code %d for %s", pair->code, "HATCH");
        }
    }
  return pair;
}
#  undef CHK_paths
#  undef CHK_polyline_paths
#  undef CHK_deflines
#  undef CHK_seeds

static Dxf_Pair *
add_MULTILEADER_lines (Dwg_Object *restrict obj, Bit_Chain *restrict dat,
                       Dxf_Pair *restrict pair, Dwg_LEADER_Node *lnode)
{
  Dwg_Entity_MULTILEADER *o = obj->tio.entity->tio.MULTILEADER;
  if (!lnode)
    {
      LOG_ERROR ("MULTILEADER_lines");
      dxf_free_pair (pair);
      return NULL;
    }
  lnode->num_lines = 0;
  if (pair->code == 304 && strEQc (pair->value.s, "LEADER_LINE{"))
    {
      int i = -1, j = -1, k = -1;
      Dwg_MLEADER_AnnotContext *ctx = &o->ctx;
      lnode->lines = (Dwg_LEADER_Line *)xcalloc (1, sizeof (Dwg_LEADER_Line));
      if (!lnode->lines)
        {
          lnode->num_lines = 0;
          LOG_ERROR ("MULTILEADER.node.lines");
          dxf_free_pair (pair);
          return NULL;
        }

      // lines and breaks
      while (pair->code != 305 && pair->code != 0)
        {
          Dwg_LEADER_Line *lline = &lnode->lines[0];
          dxf_free_pair (pair);
          pair = dxf_read_pair (dat);
          if (!pair)
            return NULL;
          switch (pair->code)
            {
            case 10:
              i++;
              lnode->num_lines = i + 1;
              LOG_TRACE ("%s.leaders[].num_lines = %d\n", obj->name, i + 1);
              if (i > 0)
                {
                  lnode->lines = (Dwg_LEADER_Line *)realloc (
                      lnode->lines,
                      lnode->num_lines * sizeof (Dwg_LEADER_Line));
                  if (!lnode->lines)
                    {
                      lnode->num_lines = 0;
                      LOG_ERROR ("Out of memory");
                      dxf_free_pair (pair);
                      return NULL;
                    }
                }
              lline = &lnode->lines[i];
              memset (lline, 0, sizeof (Dwg_LEADER_Line));
              lline->num_breaks = 0;
              j++;
              lline->num_points = j + 1;
              lline->points = (BITCODE_3BD *)realloc (
                  lline->points, lline->num_points * sizeof (BITCODE_3BD));
              memset (&lline->points[j], 0, sizeof (BITCODE_3BD));
              lline->points[j].x = pair->value.d;
              LOG_TRACE ("%s.leaders[].lines[%d].points[%d].x = %f [BD %d]\n",
                         obj->name, i, j, pair->value.d, pair->code);
              break;
            case 20:
#  define CHK_points                                                          \
    if (j < 0 || j >= (int)lline->num_points)                                 \
      {                                                                       \
        LOG_ERROR ("MULTILEADER wrong j %d points index", j);                 \
        dxf_free_pair (pair);                                                 \
        return NULL;                                                          \
      }                                                                       \
    assert (j >= 0 && j < (int)lline->num_points)

#  define CHK_breaks                                                          \
    if (k < 0 || k >= (int)lline->num_breaks)                                 \
      {                                                                       \
        LOG_ERROR ("MULTILEADER wrong k %d breaks index", j);                 \
        dxf_free_pair (pair);                                                 \
        return NULL;                                                          \
      }                                                                       \
    assert (k >= 0 && k < (int)lline->num_breaks)

              CHK_points;
              lline->points[j].y = pair->value.d;
              LOG_TRACE ("%s.leaders[].lines[%d].points[%d].y = %f [BD %d]\n",
                         obj->name, i, j, pair->value.d, pair->code);
              break;
            case 30:
              CHK_points;
              lline->points[j].z = pair->value.d;
              LOG_TRACE ("%s.leaders[].lines[%d].points[%d].z = %f [BD %d]\n",
                         obj->name, i, j, pair->value.d, pair->code);
              break;
            case 11:
              CHK_points;
              k++;
              lline->num_breaks = k + 1;
              lline->breaks = (Dwg_LEADER_Break *)realloc (
                  lline->breaks, (k + 1) * sizeof (Dwg_LEADER_Break));
              memset (&ctx->leaders[k], 0, sizeof (Dwg_LEADER_Break));
              lline->breaks[k].start.x = pair->value.d;
              LOG_TRACE (
                  "%s.leaders[].lines[%d].breaks[%d].start.x = %f [3BD %d]\n",
                  obj->name, i, k, pair->value.d, pair->code);
              break;
            case 21:
              CHK_breaks;
              lline->breaks[k].start.y = pair->value.d;
              LOG_TRACE (
                  "%s.leaders[].lines[%d].breaks[%d].start.y = %f [3BD %d]\n",
                  obj->name, i, k, pair->value.d, pair->code);
              break;
            case 31:
              CHK_breaks;
              lline->breaks[k].start.z = pair->value.d;
              LOG_TRACE (
                  "%s.leaders[].lines[%d].breaks[%d].start.z = %f [3BD %d]\n",
                  obj->name, i, k, pair->value.d, pair->code);
              break;
            case 12:
              CHK_breaks;
              lline->breaks[k].end.x = pair->value.d;
              LOG_TRACE (
                  "%s.leaders[].lines[%d].breaks[%d].end.x = %f [3BD %d]\n",
                  obj->name, i, k, pair->value.d, pair->code);
              break;
            case 22:
              CHK_breaks;
              lline->breaks[k].end.y = pair->value.d;
              LOG_TRACE (
                  "%s.leaders[].lines[%d].breaks[%d].end.y = %f [3BD %d]\n",
                  obj->name, i, k, pair->value.d, pair->code);
              break;
            case 32:
              CHK_breaks;
              lline->breaks[k].end.z = pair->value.d;
              LOG_TRACE (
                  "%s.leaders[].lines[%d].breaks[%d].end.z = %f [3BD %d]\n",
                  obj->name, i, k, pair->value.d, pair->code);
              break;
            case 91:
              lline->line_index = pair->value.u;
              LOG_TRACE ("%s.leaders[].lines[%d].line_index = %u [BL %d]\n",
                         obj->name, i, pair->value.u, pair->code);
              break;
            case 170:
              lline->type = pair->value.i;
              LOG_TRACE ("%s.leaders[].lines[%d].line_index = %d [BS %d]\n",
                         obj->name, i, pair->value.i, pair->code);
              break;
            case 92:
              if (pair->value.u > 256)
                {
                  lline->color.index = 256;
                  lline->color.rgb = pair->value.u;
                  lline->color.method = pair->value.u >> 0x18;
                  LOG_TRACE (
                      "%s.leaders[].lines[%d].color.rgb = %08X [CMC %d]\n",
                      obj->name, i, pair->value.u, pair->code);
                }
              else
                {
                  lline->color.index = pair->value.i;
                  LOG_TRACE (
                      "%s.leaders[].lines[%d].color.index = %d [CMC %d]\n",
                      obj->name, i, pair->value.i, pair->code);
                }
              break;
            case 171:
              lline->linewt = dxf_find_lweight ((int16_t)pair->value.i);
              LOG_TRACE ("%s.leaders[].lines[%d].linewt = %d [BL %d]\n",
                         obj->name, i, lline->linewt, pair->code);
              break;
            case 40:
              lline->arrow_size = pair->value.d;
              LOG_TRACE ("%s.leaders[].lines[%d].arrow_size = %f [BD %d]\n",
                         obj->name, i, pair->value.d, pair->code);
              break;
            case 93:
              lline->flags = pair->value.i;
              LOG_TRACE ("%s.leaders[].lines[%d].line_index = %d [BL %d]\n",
                         obj->name, i, pair->value.i, pair->code);
              break;
            case 305: // end
              break;
            default:
              LOG_ERROR (
                  "Unknown DXF code %d for MULTILEADER.leaders[].lines[%d]",
                  pair->code, i);
              return pair;
            }
        }
    }
  if (!lnode->num_lines)
    {
      free (lnode->lines);
      lnode->lines = NULL;
    }
  return pair;
}

#  undef CHK_points
#  undef CHK_breaks

static Dxf_Pair *
add_MULTILEADER_leaders (Dwg_Object *restrict obj, Bit_Chain *restrict dat,
                         Dxf_Pair *restrict pair)
{
  Dwg_Entity_MULTILEADER *o = obj->tio.entity->tio.MULTILEADER;
  if (pair != NULL && pair->code == 302 && strEQc (pair->value.s, "LEADER{"))
    {
      int i = -1, j = -1;
      Dwg_MLEADER_AnnotContext *ctx = &o->ctx;
      ctx->num_leaders = 0;
      while (pair != NULL && pair->code != 303 && pair->code != 0)
        {
          Dwg_LEADER_Node *lnode = i >= 0 ? &ctx->leaders[i] : NULL;
          dxf_free_pair (pair);
          pair = dxf_read_pair (dat);
          if (!pair)
            return NULL;
          if (!lnode && pair->code != 290 && pair->code != 304
              && pair->code != 303)
            {
              LOG_ERROR ("Missing MULTILEADER.LEADER{ 290 start");
              return pair;
            }
          if (lnode)
            lnode->num_breaks = 0;
          switch (pair->code)
            {
            case 290:
              i++;
              ctx->num_leaders = i + 1;
              LOG_TRACE ("%s.ctx.num_leaders = %d\n", obj->name, i + 1);
              ctx->leaders = (Dwg_LEADER_Node *)realloc (
                  ctx->leaders, (i + 1) * sizeof (Dwg_LEADER_Node));
              if (!ctx->leaders)
                return NULL;
              memset (&ctx->leaders[i], 0, sizeof (Dwg_LEADER_Node));
              ctx->leaders[i].has_lastleaderlinepoint = pair->value.i;
              LOG_TRACE (
                  "%s.ctx.leaders[%d].has_lastleaderlinepoint = %d [B %d]\n",
                  obj->name, i, pair->value.i, pair->code);
              break;
            case 291:
              lnode->has_dogleg = pair->value.i;
              LOG_TRACE ("%s.ctx.leaders[%d].has_dogleg = %d [B %d]\n",
                         obj->name, i, pair->value.i, pair->code);
              break;
            case 90:
              lnode->branch_index = pair->value.u;
              LOG_TRACE ("%s.ctx.leaders[%d].branch_index = %u [BL %d]\n",
                         obj->name, i, pair->value.u, pair->code);
              break;
            case 271:
              lnode->attach_dir = pair->value.i;
              LOG_TRACE ("%s.ctx.leaders[%d].branch_index = %d [BS %d]\n",
                         obj->name, i, pair->value.i, pair->code);
              break;
            case 40:
              lnode->dogleg_length = pair->value.d;
              LOG_TRACE ("%s.ctx.leaders[%d].dogleg_length = %f [BD %d]\n",
                         obj->name, i, pair->value.d, pair->code);
              break;
            case 10:
              lnode->lastleaderlinepoint.x = pair->value.d;
              LOG_TRACE (
                  "%s.ctx.leaders[%d].lastleaderlinepoint.x = %f [3BD %d]\n",
                  obj->name, i, pair->value.d, pair->code);
              break;
            case 20:
              lnode->lastleaderlinepoint.y = pair->value.d;
              LOG_TRACE (
                  "%s.ctx.leaders[%d].lastleaderlinepoint.y = %f [3BD %d]\n",
                  obj->name, i, pair->value.d, pair->code);
              break;
            case 30:
              lnode->lastleaderlinepoint.z = pair->value.d;
              LOG_TRACE (
                  "%s.ctx.leaders[%d].lastleaderlinepoint.z = %f [BD %d]\n",
                  obj->name, i, pair->value.d, pair->code);
              break;
            case 11:
              if (lnode->has_dogleg)
                {
                  lnode->dogleg_vector.x = pair->value.d;
                  LOG_TRACE (
                      "%s.ctx.leaders[%d].dogleg_vector.x = %f [3BD %d]\n",
                      obj->name, i, pair->value.d, pair->code);
                }
              else
                {
                  j++;
                  lnode->num_breaks = j + 1;
                  LOG_TRACE ("%s.leaders[%d].num_breaks = %d\n", obj->name, i,
                             j + 1);
                  lnode->breaks = (Dwg_LEADER_Break *)realloc (
                      lnode->breaks, (j + 1) * sizeof (Dwg_LEADER_Break));
                  lnode->breaks[j].start.x = pair->value.d;
                  LOG_TRACE (
                      "%s.ctx.leaders[%d].breaks[%d].start.x = %f [3BD %d]\n",
                      obj->name, i, j, pair->value.d, pair->code);
                }
              break;
            case 21:
              if (lnode->has_dogleg)
                {
                  lnode->dogleg_vector.y = pair->value.d;
                  LOG_TRACE (
                      "%s.ctx.leaders[%d].dogleg_vector.y = %f [3BD %d]\n",
                      obj->name, i, pair->value.d, pair->code);
                }
              else
                {
#  define CHK_breaks                                                          \
    if (j < 0 || j >= (int)lnode->num_breaks)                                 \
      return NULL;                                                            \
    assert (j >= 0 && j < (int)lnode->num_breaks)

                  CHK_breaks;
                  lnode->breaks[j].start.y = pair->value.d;
                  LOG_TRACE (
                      "%s.ctx.leaders[%d].breaks[%d].start.y = %f [3BD %d]\n",
                      obj->name, i, j, pair->value.d, pair->code);
                }
              break;
            case 31:
              if (lnode->has_dogleg)
                {
                  lnode->dogleg_vector.z = pair->value.d;
                  LOG_TRACE (
                      "%s.ctx.leaders[%d].dogleg_vector.z = %f [3BD %d]\n",
                      obj->name, i, pair->value.d, pair->code);
                }
              else
                {
                  CHK_breaks;
                  lnode->breaks[j].start.z = pair->value.d;
                  LOG_TRACE (
                      "%s.ctx.leaders[%d].breaks[%d].start.z = %f [3BD %d]\n",
                      obj->name, i, j, pair->value.d, pair->code);
                }
              break;
            case 12:
              CHK_breaks;
              lnode->breaks[j].end.x = pair->value.d;
              LOG_TRACE ("%s.ctx.leaders[%d].breaks[%d].end.x = %f [3BD %d]\n",
                         obj->name, i, j, pair->value.d, pair->code);
              break;
            case 22:
              CHK_breaks;
              lnode->breaks[j].end.y = pair->value.d;
              LOG_TRACE ("%s.ctx.leaders[%d].breaks[%d].end.y = %f [3BD %d]\n",
                         obj->name, i, j, pair->value.d, pair->code);
              break;
            case 32:
              CHK_breaks;
              lnode->breaks[j].end.z = pair->value.d;
              LOG_TRACE ("%s.ctx.leaders[%d].breaks[%d].end.z = %f [3BD %d]\n",
                         obj->name, i, j, pair->value.d, pair->code);
              break;
            case 304:
              if (strEQc (pair->value.s, "LEADER_LINE{"))
                pair = add_MULTILEADER_lines (obj, dat, pair, lnode);
              break;
            case 303: // end
              break;
            default:
              LOG_ERROR ("Unknown DXF code %d for MULTILEADER.leaders[]",
                         pair->code);
              return pair;
            }
        }
    }
  return pair;
}
#  undef CHK_breaks

static Dxf_Pair *
add_MULTILEADER (Dwg_Object *restrict obj, Bit_Chain *restrict dat,
                 Dxf_Pair *restrict pair)
{
  Dwg_Entity_MULTILEADER *o = obj->tio.entity->tio.MULTILEADER;
  int i = -1, j = -1;

  if (pair->code == 300 && strEQc (pair->value.s, "CONTEXT_DATA{"))
    {
      // const Dwg_DYNAPI_field *fields
      // = dwg_dynapi_subclass_fields ("MLEADER_AnnotContext");
      Dwg_MLEADER_AnnotContext *ctx = &o->ctx;
      while (pair != NULL && pair->code != 301 && pair->code != 0)
        {
          switch (pair->code)
            {
            case 300:
              break;
            case 40:
              ctx->scale_factor = pair->value.d;
              LOG_TRACE ("%s.ctx.scale = %f [BD %d]\n", obj->name,
                         pair->value.d, pair->code);
              break;
            case 10:
              ctx->content_base.x = pair->value.d;
              break;
            case 20:
              ctx->content_base.y = pair->value.d;
              break;
            case 30:
              ctx->content_base.z = pair->value.d;
              LOG_TRACE ("%s.ctx.content_base = (%f, %f, %f) [10 3BD]\n",
                         obj->name, ctx->content_base.x, ctx->content_base.y,
                         ctx->content_base.z);
              break;
            case 41:
              ctx->text_height = pair->value.d;
              LOG_TRACE ("%s.ctx.text_height = %f [BD %d]\n", obj->name,
                         pair->value.d, pair->code);
              break;
            case 140:
              ctx->arrow_size = pair->value.d;
              LOG_TRACE ("%s.ctx.arrow_size = %f [BD %d]\n", obj->name,
                         pair->value.d, pair->code);
              break;
            case 145:
              ctx->landing_gap = pair->value.d;
              LOG_TRACE ("%s.ctx.landing_gap = %f [BD %d]\n", obj->name,
                         pair->value.d, pair->code);
              break;
            case 174:
              ctx->text_left = pair->value.i;
              LOG_TRACE ("%s.ctx.text_left = %d [BS %d]\n", obj->name,
                         pair->value.i, pair->code);
              break;
            case 175:
              ctx->text_right = pair->value.i;
              LOG_TRACE ("%s.ctx.text_right = %d [BS %d]\n", obj->name,
                         pair->value.i, pair->code);
              break;
            case 176:
              ctx->text_angletype = pair->value.i;
              LOG_TRACE ("%s.ctx.text_angletype = %d [BS %d]\n", obj->name,
                         pair->value.i, pair->code);
              break;
            case 177:
              ctx->text_alignment = pair->value.i;
              LOG_TRACE ("%s.ctx.text_alignment = %d [BS %d]\n", obj->name,
                         pair->value.i, pair->code);
              break;
            case 290:
              if (ctx->has_content_blk)
                goto unknown_mleader;
              ctx->has_content_txt = pair->value.i;
              LOG_TRACE ("%s.ctx.has_content_txt = %d [B %d]\n", obj->name,
                         pair->value.i, pair->code);
              break;
            case 302:
              if (strEQc (pair->value.s, "LEADER{"))
                pair = add_MULTILEADER_leaders (obj, dat, pair);
              break;
            case 304:
              if (ctx->has_content_txt)
                {
                  if (ctx->has_content_blk)
                    goto unknown_mleader;
                  if (dat->version >= R_2007)
                    ctx->content.txt.default_text
                        = (char *)bit_utf8_to_TU (pair->value.s, 0);
                  else
                    ctx->content.txt.default_text = strdup (pair->value.s);
                  LOG_TRACE ("%s.ctx.content.txt.default_text = %s [%d T]\n",
                             obj->name, pair->value.s, pair->code);
                }
              break;
            case 340:
              if (ctx->has_content_txt)
                {
                  if (ctx->has_content_blk)
                    goto unknown_mleader;
                  ctx->content.txt.style
                      = dwg_add_handleref (obj->parent, 5, pair->value.u, obj);
                  LOG_TRACE ("%s.ctx.content.txt.style = " FORMAT_REF
                             " [%d H]\n",
                             obj->name, ARGS_REF (ctx->content.txt.style),
                             pair->code);
                }
              break;
            case 11:
              if (!ctx->has_content_txt)
                goto unknown_mleader;
              ctx->content.txt.normal.x = pair->value.d;
              break;
            case 21:
              if (!ctx->has_content_txt)
                goto unknown_mleader;
              ctx->content.txt.normal.y = pair->value.d;
              break;
            case 31:
              if (!ctx->has_content_txt)
                goto unknown_mleader;
              ctx->content.txt.normal.z = pair->value.d;
              LOG_TRACE ("%s.ctx.content.txt.normal = (%f, %f, %f) [11 3BD]\n",
                         obj->name, ctx->content.txt.normal.x,
                         ctx->content.txt.normal.y, ctx->content.txt.normal.z);
              break;
            case 12:
              if (!ctx->has_content_txt)
                goto unknown_mleader;
              ctx->content.txt.location.x = pair->value.d;
              break;
            case 22:
              if (!ctx->has_content_txt)
                goto unknown_mleader;
              ctx->content.txt.location.y = pair->value.d;
              break;
            case 32:
              if (!ctx->has_content_txt)
                goto unknown_mleader;
              ctx->content.txt.location.z = pair->value.d;
              LOG_TRACE (
                  "%s.ctx.content.txt.location = (%f, %f, %f) [12 3BD]\n",
                  obj->name, ctx->content.txt.location.x,
                  ctx->content.txt.location.y, ctx->content.txt.location.z);
              break;
            case 13:
              if (!ctx->has_content_txt)
                goto unknown_mleader;
              ctx->content.txt.direction.x = pair->value.d;
              break;
            case 23:
              if (!ctx->has_content_txt)
                goto unknown_mleader;
              ctx->content.txt.direction.y = pair->value.d;
              break;
            case 33:
              if (!ctx->has_content_txt)
                goto unknown_mleader;
              ctx->content.txt.direction.z = pair->value.d;
              LOG_TRACE (
                  "%s.ctx.content.txt.direction = (%f, %f, %f) [13 3BD]\n",
                  obj->name, ctx->content.txt.direction.x,
                  ctx->content.txt.direction.y, ctx->content.txt.direction.z);
              break;
            case 42:
              if (!ctx->has_content_txt)
                goto unknown_mleader;
              ctx->content.txt.rotation = deg2rad (pair->value.d);
              LOG_TRACE ("%s.ctx.content.txt.rotation = %f [BD %d]\n",
                         obj->name, ctx->content.txt.rotation, pair->code);
              break;
            case 43:
              if (!ctx->has_content_txt)
                goto unknown_mleader;
              ctx->content.txt.width = pair->value.d;
              LOG_TRACE ("%s.ctx.content.txt.width = %f [BD %d]\n", obj->name,
                         pair->value.d, pair->code);
              break;
            case 44:
              if (!ctx->has_content_txt)
                goto unknown_mleader;
              ctx->content.txt.height = pair->value.d;
              LOG_TRACE ("%s.ctx.content.txt.height = %f [BD %d]\n", obj->name,
                         pair->value.d, pair->code);
              break;
            case 45:
              if (!ctx->has_content_txt)
                goto unknown_mleader;
              ctx->content.txt.line_spacing_factor = pair->value.d;
              LOG_TRACE (
                  "%s.ctx.content.txt.line_spacing_factor = %f [BD %d]\n",
                  obj->name, pair->value.d, pair->code);
              break;
            case 170:
              if (!ctx->has_content_txt)
                goto unknown_mleader;
              ctx->content.txt.line_spacing_style = pair->value.i;
              LOG_TRACE (
                  "%s.ctx.content.txt.line_spacing_style = %d [BS %d]\n",
                  obj->name, pair->value.i, pair->code);
              break;
            case 171:
              if (!ctx->has_content_txt)
                goto unknown_mleader;
              ctx->content.txt.alignment = pair->value.i;
              LOG_TRACE ("%s.ctx.content.txt.alignment = %d [BS %d]\n",
                         obj->name, pair->value.i, pair->code);
              break;
            case 172:
              if (!ctx->has_content_txt)
                break;
              ctx->content.txt.flow = pair->value.i;
              LOG_TRACE ("%s.ctx.content.txt.flow = %d [BS %d]\n", obj->name,
                         pair->value.i, pair->code);
              break;
            case 90:
              if (!ctx->has_content_txt)
                goto unknown_mleader;
              ctx->content.txt.color.index = pair->value.i;
              LOG_TRACE ("%s.ctx.content.txt.color.index = %d [BS %d]\n",
                         obj->name, pair->value.i, pair->code);
              break;
            case 91:
              if (!ctx->has_content_txt)
                goto unknown_mleader;
              ctx->content.txt.bg_color.index = pair->value.i;
              LOG_TRACE ("%s.ctx.content.txt.bg_color.index = %d [BS %d]\n",
                         obj->name, pair->value.i, pair->code);
              break;
            case 141:
              if (!ctx->has_content_txt)
                goto unknown_mleader;
              ctx->content.txt.bg_scale = pair->value.d;
              LOG_TRACE ("%s.ctx.content.txt.bg_scale = %f [BD %d]\n",
                         obj->name, pair->value.d, pair->code);
              break;
            case 142:
              if (!ctx->has_content_txt)
                goto unknown_mleader;
              ctx->content.txt.col_width = pair->value.d;
              LOG_TRACE ("%s.ctx.content.txt.col_width = %f [BD %d]\n",
                         obj->name, pair->value.d, pair->code);
              break;
            case 143:
              if (!ctx->has_content_txt)
                goto unknown_mleader;
              ctx->content.txt.col_gutter = pair->value.d;
              LOG_TRACE ("%s.ctx.content.txt.col_gutter = %f [BD %d]\n",
                         obj->name, pair->value.d, pair->code);
              break;
            case 92:
              if (!ctx->has_content_txt)
                goto unknown_mleader;
              ctx->content.txt.bg_transparency = pair->value.u;
              LOG_TRACE ("%s.ctx.content.txt.bg_transparency = %u [BL %d]\n",
                         obj->name, pair->value.u, pair->code);
              break;
            case 291:
              if (!ctx->has_content_txt)
                goto unknown_mleader;
              ctx->content.txt.is_bg_fill = pair->value.i;
              LOG_TRACE ("%s.ctx.content.txt.is_bg_fill = %i [B %d]\n",
                         obj->name, pair->value.i, pair->code);
              break;
            case 292:
              if (!ctx->has_content_txt)
                goto unknown_mleader;
              ctx->content.txt.is_bg_mask_fill = pair->value.i;
              LOG_TRACE ("%s.ctx.content.txt.is_bg_mask_fill = %i [B %d]\n",
                         obj->name, pair->value.i, pair->code);
              break;
            case 293:
              if (!ctx->has_content_txt)
                goto unknown_mleader;
              ctx->content.txt.is_height_auto = pair->value.i;
              LOG_TRACE ("%s.ctx.content.txt.is_height_auto = %i [B %d]\n",
                         obj->name, pair->value.i, pair->code);
              break;
            case 294:
              if (!ctx->has_content_txt)
                goto unknown_mleader;
              ctx->content.txt.is_col_flow_reversed = pair->value.i;
              LOG_TRACE (
                  "%s.ctx.content.txt.is_col_flow_reversed = %i [B %d]\n",
                  obj->name, pair->value.i, pair->code);
              break;
            case 295:
              if (!ctx->has_content_txt)
                goto unknown_mleader;
              ctx->content.txt.word_break = pair->value.i;
              LOG_TRACE ("%s.ctx.content.txt.word_break = %i [B %d]\n",
                         obj->name, pair->value.i, pair->code);
              break;
            case 173:
              if (!ctx->has_content_txt)
                goto unknown_mleader;
              ctx->content.txt.col_type = pair->value.i;
              LOG_TRACE ("%s.ctx.content.txt.col_type = %d [BS %d]\n",
                         obj->name, pair->value.i, pair->code);
              break;
            case 144:
              if (!ctx->has_content_txt)
                goto unknown_mleader;
              i++;
              ctx->content.txt.num_col_sizes = i + 1;
              ctx->content.txt.col_sizes = (double *)realloc (
                  ctx->content.txt.col_sizes, (i + 1) * sizeof (double));
              ctx->content.txt.col_sizes[i] = pair->value.d;
              LOG_TRACE ("%s.ctx.content.txt.col_sizes[%d] = %f [BD %d]\n",
                         obj->name, i, pair->value.d, pair->code);
              break;
            case 296:
              ctx->has_content_blk = pair->value.i;
              LOG_TRACE ("%s.ctx.has_content_blk = %i [B %d]\n", obj->name,
                         pair->value.i, pair->code);
              break;
            case 14: // has_block
              if (!ctx->has_content_blk)
                goto unknown_mleader;
              ctx->content.blk.normal.x = pair->value.d;
              LOG_TRACE ("%s.ctx.content.blk.normal.x = %f [3BD %d]\n",
                         obj->name, pair->value.d, pair->code);
              break;
            case 24:
              if (!ctx->has_content_blk)
                goto unknown_mleader;
              ctx->content.blk.normal.y = pair->value.d;
              LOG_TRACE ("%s.ctx.content.blk.normal.y = %f [3BD %d]\n",
                         obj->name, pair->value.d, pair->code);
              break;
            case 34:
              if (!ctx->has_content_blk)
                goto unknown_mleader;
              ctx->content.blk.normal.z = pair->value.d;
              LOG_TRACE ("%s.ctx.content.blk.normal.z = %f [3BD %d]\n",
                         obj->name, pair->value.d, pair->code);
              break;
            case 341:
              if (!ctx->has_content_blk)
                goto unknown_mleader;
              ctx->content.blk.block_table
                  = dwg_add_handleref (obj->parent, 4, pair->value.u, obj);
              LOG_TRACE ("%s.ctx.content.blk.block_table = " FORMAT_REF
                         " [%d H]\n",
                         obj->name, ARGS_REF (ctx->content.blk.block_table),
                         pair->code);
              break;
            case 15: // has_block
              if (!ctx->has_content_blk)
                goto unknown_mleader;
              ctx->content.blk.normal.x = pair->value.d;
              LOG_TRACE ("%s.ctx.content.blk.normal.x = %f [3BD %d]\n",
                         obj->name, pair->value.d, pair->code);
              break;
            case 25:
              if (!ctx->has_content_blk)
                goto unknown_mleader;
              ctx->content.blk.location.y = pair->value.d;
              break;
            case 35:
              if (!ctx->has_content_blk)
                goto unknown_mleader;
              ctx->content.blk.location.z = pair->value.d;
              LOG_TRACE (
                  "%s.ctx.content.blk.location = (%f, %f, %f) [3BD %d]\n",
                  obj->name, ctx->content.blk.location.x,
                  ctx->content.blk.location.y, ctx->content.blk.location.z,
                  pair->code);
              break;
            case 16: // has_block
              if (!ctx->has_content_blk)
                goto unknown_mleader;
              ctx->content.blk.scale.x = pair->value.d;
              break;
            case 26:
              if (!ctx->has_content_blk)
                goto unknown_mleader;
              ctx->content.blk.scale.y = pair->value.d;
              break;
            case 36:
              if (!ctx->has_content_blk)
                goto unknown_mleader;
              ctx->content.blk.scale.z = pair->value.d;
              LOG_TRACE ("%s.ctx.content.blk.scale = (%f, %f, %f) [3BD %d]\n",
                         obj->name, ctx->content.blk.scale.x,
                         ctx->content.blk.scale.y, ctx->content.blk.scale.z,
                         pair->code);
              break;
            case 46:
              if (!ctx->has_content_blk)
                goto unknown_mleader;
              ctx->content.blk.rotation = deg2rad (pair->value.d);
              LOG_TRACE ("%s.ctx.content.blk.rotation = %f [BD %d]\n",
                         obj->name, pair->value.d, pair->code);
              break;
            case 93:
              if (!ctx->has_content_blk)
                goto unknown_mleader;
              if (pair->value.u > 257)
                {
                  ctx->content.blk.color.index = 256;
                  ctx->content.blk.color.rgb = pair->value.u;
                  ctx->content.blk.color.method = pair->value.u >> 0x18;
                  LOG_TRACE (
                      "%s.leaders[].lines[%d].color.rgb = %08X [CMC %d]\n",
                      obj->name, i, pair->value.u, pair->code);
                }
              else
                {
                  ctx->content.blk.color.index = pair->value.i;
                  if (pair->value.i == 257)
                    {
                      ctx->content.blk.color.method = 0xc8;
                      ctx->content.blk.color.rgb = 0xc8000000;
                    }
                  LOG_TRACE (
                      "%s.leaders[].lines[%d].color.index = %d [CMC %d]\n",
                      obj->name, i, pair->value.i, pair->code);
                }
              break;
            case 47:
              if (!ctx->has_content_blk)
                goto unknown_mleader;
              j++;
              if (!j)
                {
                  ctx->content.blk.transform
                      = (double *)xcalloc (16, sizeof (double));
                  if (!ctx->content.blk.transform)
                    {
                      return NULL;
                    }
                }
              if (j >= 16)
                {
                  LOG_ERROR ("Too many %s.ctx.content.blk.transform[%d] "
                             "groups 47, max 16",
                             obj->name, j);
                  free (ctx->content.blk.transform);
                  ctx->content.blk.transform = NULL;
                  return NULL;
                }
              ctx->content.blk.transform[j] = pair->value.d;
              LOG_TRACE ("%s.ctx.content.blk.transform[%d] = %f [BD %d]\n",
                         obj->name, j, pair->value.d, pair->code);
              break;
            case 110:
              ctx->base.x = pair->value.d;
              break;
            case 120:
              ctx->base.y = pair->value.d;
              break;
            case 130:
              ctx->base.z = pair->value.d;
              LOG_TRACE ("%s.ctx.base = (%f, %f, %f) [3BD %d]\n", obj->name,
                         ctx->base.x, ctx->base.y, ctx->base.z, pair->code);
              break;
            case 111:
              ctx->base_dir.x = pair->value.d;
              break;
            case 121:
              ctx->base_dir.y = pair->value.d;
              break;
            case 131:
              ctx->base_dir.z = pair->value.d;
              LOG_TRACE ("%s.ctx.base = (%f, %f, %f) [3BD %d]\n", obj->name,
                         ctx->base_dir.x, ctx->base_dir.y, ctx->base_dir.z,
                         pair->code);
              break;
            case 112:
              ctx->base_vert.x = pair->value.d;
              break;
            case 122:
              ctx->base_vert.y = pair->value.d;
              break;
            case 132:
              ctx->base_vert.z = pair->value.d;
              LOG_TRACE ("%s.ctx.base = (%f, %f, %f) [3BD %d]\n", obj->name,
                         ctx->base_vert.x, ctx->base_vert.y, ctx->base_vert.z,
                         pair->code);
              break;
            case 297:
              ctx->is_normal_reversed = pair->value.i;
              LOG_TRACE ("%s.ctx.is_normal_reversed = %i [B %d]\n", obj->name,
                         pair->value.i, pair->code);
              break;
            case 273:
              ctx->text_top = pair->value.i;
              LOG_TRACE ("%s.ctx.text_top = %i [BS %d]\n", obj->name,
                         pair->value.i, pair->code);
              break;
            case 272:
              ctx->text_bottom = pair->value.i;
              LOG_TRACE ("%s.ctx.text_bottom = %i [BS %d]\n", obj->name,
                         pair->value.i, pair->code);
              break;

            case 301: // end ctx
              return pair;
            default:
            unknown_mleader:
              LOG_ERROR ("Unknown DXF code %d for %s", pair->code,
                         "MULTILEADER");
            }
          dxf_free_pair (pair);
          pair = dxf_read_pair (dat);
        }
    }
  return pair;
}

// returns with pair if not found, or NULL on error.
// only for geomesh_pts (num 93, 13, 14) and geomesh_faces (num 96, 97-99)
static Dxf_Pair *
add_GEODATA (Dwg_Object *restrict obj, Bit_Chain *restrict dat,
             Dxf_Pair *restrict pair)
{
  Dwg_Data *dwg = obj->parent;
  Dwg_Object_GEODATA *o = obj->tio.object->tio.GEODATA;
  int i = -1;

  while (pair != NULL)
    {
      switch (pair->code)
        {
        case 93:
          i = -1;
          o->num_geomesh_pts = pair->value.u;
          o->geomesh_pts = (Dwg_GEODATA_meshpt *)xcalloc (
              pair->value.u, sizeof (Dwg_GEODATA_meshpt));
          if (!o->geomesh_pts && o->num_geomesh_pts)
            return NULL;
          LOG_TRACE ("%s.num_geomesh_pts = %u [BL %d]\n", obj->name,
                     pair->value.u, pair->code);
          break;
        case 96:
          i = -1;
          o->num_geomesh_faces = pair->value.u;
          o->geomesh_faces = (Dwg_GEODATA_meshface *)xcalloc (
              pair->value.u, sizeof (Dwg_GEODATA_meshface));
          if (!o->geomesh_faces && o->num_geomesh_faces)
            return NULL;
          LOG_TRACE ("%s.num_geomesh_faces = %u [BL %d]\n", obj->name,
                     pair->value.u, pair->code);
          break;
        case 13:
          i++;
#  define CHK_geomesh_pts                                                     \
    if (i < 0 || i >= (int)o->num_geomesh_pts || !o->geomesh_pts)             \
      return NULL;                                                            \
    assert (i >= 0 && i < (int)o->num_geomesh_pts)

          CHK_geomesh_pts;
          o->geomesh_pts[i].source_pt.x = pair->value.d;
          break;
        case 23:
          CHK_geomesh_pts;
          o->geomesh_pts[i].source_pt.y = pair->value.d;
          LOG_TRACE ("%s.geomesh_pts[%d] = (%f, %f) [2RD %d]\n", obj->name, i,
                     o->geomesh_pts[i].source_pt.x, pair->value.d, 13);
          break;
        case 14:
          CHK_geomesh_pts;
          o->geomesh_pts[i].dest_pt.x = pair->value.d;
          break;
        case 24:
          CHK_geomesh_pts;
          o->geomesh_pts[i].dest_pt.y = pair->value.d;
          LOG_TRACE ("%s.geomesh_pts[%d].dest_pt = (%f, %f) [2RD %d]\n",
                     obj->name, i, o->geomesh_pts[i].dest_pt.x, pair->value.d,
                     13);
          break;
        case 97:
          i++;
#  define CHK_geomesh_faces                                                   \
    if (i < 0 || i >= (int)o->num_geomesh_faces || !o->geomesh_faces)         \
      return NULL;                                                            \
    assert (o->geomesh_faces);                                                \
    assert (i >= 0 && i < (int)o->num_geomesh_faces)

          CHK_geomesh_faces;
          o->geomesh_faces[i].face1 = pair->value.u;
          break;
        case 98:
          CHK_geomesh_faces;
          o->geomesh_faces[i].face2 = pair->value.u;
          break;
        case 99:
          CHK_geomesh_faces;
          o->geomesh_faces[i].face3 = pair->value.u;
          LOG_TRACE ("%s.geomesh_faces[%d] = (%u, %u, %u) [3*BL %d]\n",
                     obj->name, i, o->geomesh_faces[i].face1,
                     o->geomesh_faces[i].face2, pair->value.u, 97);
          break;
        case 0:
        default:
          return pair;
        }
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
    }
  return pair;
}
#  undef CHK_geomesh_pts
#  undef CHK_geomesh_faces

// returns with 0
// subclass for multiple objects.
// Only handle conflicts?, codes with multiple keys.
// 1, 309
static Dxf_Pair *
add_CellStyle (Dwg_Object *restrict obj, Dwg_CellStyle *o, const char *key,
               Bit_Chain *restrict dat, Dxf_Pair *restrict pair)
{
  Dwg_Data *dwg = obj->parent;
  BITCODE_H hdl;
  enum
  {
    NONE,
    TABLEFORMAT,
    CONTENTFORMAT,
    CELLMARGIN,
    GRIDFORMAT,
    CELLSTYLE
  } mode;
  int i = -1, j = -1;
  int grid = -1;
  mode = NONE;

  while (pair != NULL && pair->code != 0)
    {
      switch (pair->code)
        {
        case 0:
          break;
        case 1:
          if (strEQc (pair->value.s, "TABLEFORMAT_BEGIN"))
            mode = TABLEFORMAT;
          else if (strEQc (pair->value.s, "CONTENTFORMAT_BEGIN"))
            mode = CONTENTFORMAT;
          else if (strEQc (pair->value.s, "CELLMARGIN_BEGIN"))
            mode = CELLMARGIN;
          else if (strEQc (pair->value.s, "GRIDFORMAT_BEGIN"))
            mode = GRIDFORMAT;
          else if (strEQc (pair->value.s, "CELLSTYLE_BEGIN"))
            {
              dxf_free_pair (pair);
              return dxf_read_pair (dat);
            }
          else
            goto unknown_default;
          break;
        case 300:
          if (mode == CONTENTFORMAT)
            {
              if (obj->parent->header.version >= R_2007)
                o->content_format.value_format_string
                    = (BITCODE_T)bit_utf8_to_TU (pair->value.s, 0);
              else
                o->content_format.value_format_string = strdup (pair->value.s);
              LOG_TRACE (
                  "%s.%s.content_format.value_format_string = \"%s\" [T %d]\n",
                  obj->name, key, pair->value.s, pair->code);
            }
          else if (mode == TABLEFORMAT)
            {
              if (!strEQc (pair->value.s, "CONTENTFORMAT"))
                goto unknown_default;
            }
          else if (mode == CELLSTYLE)
            return pair;
          else
            goto unknown_default;
          break;
        case 301:
          // ignore MARGIN
          if (!strEQc (pair->value.s, "MARGIN"))
            goto unknown_default;
          break;
        case 302:
          // ignore GRIDFORMAT
          if (!strEQc (pair->value.s, "GRIDFORMAT"))
            goto unknown_default;
          break;
        case 309:
          if (strEQc (pair->value.s, "TABLEFORMAT_END")
              || strEQc (pair->value.s, "CELLSTYLE_END"))
            mode = NONE;
          else if (strEQc (pair->value.s, "CONTENTFORMAT_END")
                   || strEQc (pair->value.s, "CELLMARGIN_END")
                   || strEQc (pair->value.s, "GRIDFORMAT_END"))
            mode = TABLEFORMAT;
          else
            goto unknown_default;
          break;
        case 90:
          if (mode == TABLEFORMAT)
            {
              o->type = pair->value.u;
              LOG_TRACE ("%s.%s.type = " FORMAT_BL " [BL %d]\n", obj->name,
                         key, pair->value.u, pair->code);
            }
          else if (mode == CONTENTFORMAT)
            {
              o->content_format.property_override_flags = pair->value.u;
              LOG_TRACE (
                  "%s.%s.content_format.property_override_flags = " FORMAT_BLx
                  " [BLx %d]\n",
                  obj->name, key, pair->value.u, pair->code);
            }
          else if (mode == GRIDFORMAT)
            {
              if (grid < 0 || grid >= (int)o->num_borders)
                return NULL;
              o->borders[grid].border_overrides = pair->value.u;
              LOG_TRACE ("%s.%s.borders[%d].border_overrides = " FORMAT_BLx
                         " [BLx %d]\n",
                         obj->name, key, grid, pair->value.u, pair->code);
            }
          else if (mode == CELLSTYLE)
            return pair;
          else if (mode == NONE)
            return pair;
          else
            goto unknown_default;
          break;
        case 91:
          if (mode == TABLEFORMAT)
            {
              o->property_override_flags = pair->value.u;
              LOG_TRACE ("%s.%s.property_override_flags = " FORMAT_BLx
                         " [BLx %d]\n",
                         obj->name, key, pair->value.u, pair->code);
            }
          else if (mode == CONTENTFORMAT)
            {
              o->content_format.property_flags = pair->value.u;
              LOG_TRACE ("%s.%s.content_format.property_flags = " FORMAT_BLx
                         " [BLx %d]\n",
                         obj->name, key, pair->value.u, pair->code);
            }
          else if (mode == GRIDFORMAT)
            {
              if (grid < 0 || grid >= (int)o->num_borders)
                return NULL;
              o->borders[grid].border_type = pair->value.u;
              LOG_TRACE ("%s.%s.borders[%d].border_type = " FORMAT_BLx
                         " [BLx %d]\n",
                         obj->name, key, grid, pair->value.u, pair->code);
            }
          else if (mode == CELLSTYLE)
            return pair;
          else if (mode == NONE)
            return pair;
          else
            goto unknown_default;
          break;
        case 92:
          if (mode == TABLEFORMAT)
            {
              o->merge_flags = pair->value.u;
              LOG_TRACE ("%s.%s.merge_flags = " FORMAT_BLx " [BLx %d]\n",
                         obj->name, key, pair->value.u, pair->code);
            }
          else if (mode == CONTENTFORMAT)
            {
              o->content_format.value_data_type = pair->value.u;
              LOG_TRACE ("%s.%s.content_format.value_data_type = " FORMAT_BLx
                         " [BLx %d]\n",
                         obj->name, key, pair->value.u, pair->code);
            }
          else if (mode == GRIDFORMAT)
            {
              if (grid < 0 || grid >= (int)o->num_borders)
                return NULL;
              o->borders[grid].linewt = pair->value.i;
              LOG_TRACE ("%s.%s.borders[%d].linewt = %d [BSd %d]\n", obj->name,
                         key, grid, pair->value.i, pair->code);
            }
          else
            goto unknown_default;
          break;
        case 93:
          if (mode == TABLEFORMAT)
            {
              o->content_layout = pair->value.u;
              LOG_TRACE ("%s.%s.content_layout = " FORMAT_BL " [BL %d]\n",
                         obj->name, key, pair->value.u, pair->code);
            }
          else if (mode == CONTENTFORMAT)
            {
              o->content_format.value_unit_type = pair->value.u;
              LOG_TRACE ("%s.%s.content_format.value_unit_type = " FORMAT_BLx
                         " [BLx %d]\n",
                         obj->name, key, pair->value.u, pair->code);
            }
          else if (mode == GRIDFORMAT)
            {
              if (grid < 0 || grid >= (int)o->num_borders)
                return NULL;
              o->borders[grid].visible = pair->value.i;
              LOG_TRACE ("%s.%s.borders[%d].visible = %d [BL %d]\n", obj->name,
                         key, grid, pair->value.i, pair->code);
            }
          else
            goto unknown_default;
          break;
        case 94:
          if (mode == TABLEFORMAT)
            {
              o->num_borders = pair->value.u;
              j = 0;
              LOG_TRACE ("%s.%s.num_borders = " FORMAT_BL " [BL %d]\n",
                         obj->name, key, pair->value.u, pair->code);
              o->borders = (Dwg_GridFormat *)xcalloc (o->num_borders,
                                                      sizeof (Dwg_GridFormat));
              if (!o->borders)
                {
                  o->num_borders = 0;
                  return NULL;
                }
            }
          else if (mode == CONTENTFORMAT)
            {
              o->content_format.cell_alignment = pair->value.u;
              LOG_TRACE ("%s.%s.content_format.cell_alignment = " FORMAT_BL
                         " [BL %d]\n",
                         obj->name, key, pair->value.u, pair->code);
            }
          else
            goto unknown_default;
          break;
        case 95:
          if (mode == TABLEFORMAT)
            {
              grid++;
              if (grid < 0 || grid >= (int)o->num_borders)
                return NULL;
              o->borders[grid].index_mask = pair->value.u;
              LOG_TRACE ("%s.%s.borders[%d].index_mask = " FORMAT_BLx
                         " [BLx %d]\n",
                         obj->name, key, grid, pair->value.u, pair->code);
            }
          else
            goto unknown_default;
          break;
        case 62:
          if (mode == TABLEFORMAT)
            {
              o->bg_color.rgb = pair->value.u;
              o->bg_color.method = pair->value.u >> 0x18;
              if (pair->value.u == 257)
                {
                  o->bg_color.method = 0xc8;
                  o->bg_color.rgb = 0xc8000000;
                }
              else
                o->bg_color.index = dwg_find_color_index (pair->value.u);
              LOG_TRACE ("%s.%s.bg_color = %08x [CMTC %d]\n", obj->name, key,
                         pair->value.u, pair->code);
            }
          else if (mode == CONTENTFORMAT)
            {
              o->content_format.content_color.rgb = pair->value.u;
              o->content_format.content_color.method = pair->value.u >> 0x18;
              if (pair->value.u == 257)
                {
                  o->content_format.content_color.method = 0xc8;
                  o->content_format.content_color.rgb = 0xc8000000;
                }
              else
                o->content_format.content_color.index
                    = dwg_find_color_index (pair->value.u);
              LOG_TRACE (
                  "%s.%s.content_format.content_color = %08x [CMTC %d]\n",
                  obj->name, key, pair->value.u, pair->code);
            }
          else if (mode == GRIDFORMAT)
            {
              if (grid < 0 || grid >= (int)o->num_borders)
                return NULL;
              o->borders[grid].color.rgb = pair->value.d;
              o->borders[grid].color.method = pair->value.u >> 0x18;
              if (pair->value.u == 257)
                {
                  o->borders[grid].color.method = 0xc8;
                  o->borders[grid].color.rgb = 0xc8000000;
                }
              else
                o->borders[grid].color.index
                    = dwg_find_color_index (pair->value.u);
              LOG_TRACE ("%s.%s.borders[%d].color = %08x [CMTC %d]\n",
                         obj->name, key, grid, pair->value.u, pair->code);
            }
          else
            goto unknown_default;
          break;
        case 40:
          if (mode == CELLMARGIN)
            {
              i++;
              switch (i)
                {
                case 0:
                  o->vert_margin = pair->value.d;
                  LOG_TRACE ("%s.%s.vert_margin = %f [BD %d]\n", obj->name,
                             key, pair->value.d, pair->code);
                  break;
                case 1:
                  o->horiz_margin = pair->value.d;
                  LOG_TRACE ("%s.%s.horiz_margin = %f [BD %d]\n", obj->name,
                             key, pair->value.d, pair->code);
                  break;
                case 2:
                  o->bottom_margin = pair->value.d;
                  LOG_TRACE ("%s.%s.bottom_margin = %f [BD %d]\n", obj->name,
                             key, pair->value.d, pair->code);
                  break;
                case 3:
                  o->right_margin = pair->value.d;
                  LOG_TRACE ("%s.%s.right_margin = %f [BD %d]\n", obj->name,
                             key, pair->value.d, pair->code);
                  break;
                case 4:
                  o->margin_horiz_spacing = pair->value.d;
                  LOG_TRACE ("%s.%s.margin_horiz_spacing = %f [BD %d]\n",
                             obj->name, key, pair->value.d, pair->code);
                  break;
                case 5:
                  o->margin_vert_spacing = pair->value.d;
                  LOG_TRACE ("%s.%s.margin_vert_spacing = %f [BD %d]\n",
                             obj->name, key, pair->value.d, pair->code);
                  break;
                default:
                  LOG_ERROR ("Invalid CELLMARGIN 40 index %d", i);
                  goto unknown_default;
                }
            }
          else if (mode == CONTENTFORMAT)
            {
              o->content_format.rotation = pair->value.d;
              LOG_TRACE ("%s.%s.content_format.rotation = %f [BD %d]\n",
                         obj->name, key, pair->value.d, pair->code);
            }
          else if (mode == GRIDFORMAT)
            {
              if (grid < 0 || grid >= (int)o->num_borders)
                return NULL;
              o->borders[grid].double_line_spacing = pair->value.d;
              LOG_TRACE (
                  "%s.%s.borders[%d].double_line_spacing = %f [BD %d]\n",
                  obj->name, key, grid, pair->value.d, pair->code);
            }
          else
            goto unknown_default;
          break;
        case 140:
          if (mode == CONTENTFORMAT)
            {
              o->content_format.block_scale = pair->value.d;
              LOG_TRACE ("%s.%s.content_format.block_scale = %f [BD %d]\n",
                         obj->name, key, pair->value.d, pair->code);
            }
          else
            goto unknown_default;
          break;
        case 144:
          if (mode == CONTENTFORMAT)
            {
              o->content_format.text_height = pair->value.d;
              LOG_TRACE ("%s.%s.content_format.text_height = %f [BD %d]\n",
                         obj->name, key, pair->value.d, pair->code);
            }
          else
            goto unknown_default;
          break;
        case 170:
          if (mode == TABLEFORMAT)
            {
              o->data_flags = pair->value.u;
              LOG_TRACE ("%s.%s.data_flags = 0x%x [BSx %d]\n", obj->name, key,
                         pair->value.u, pair->code);
            }
          else
            goto unknown_default;
          break;
        case 171:
          if (mode == TABLEFORMAT)
            {
              o->margin_override_flags = pair->value.u;
              LOG_TRACE ("%s.%s.margin_override_flags = 0x%x [BSx %d]\n",
                         obj->name, key, pair->value.u, pair->code);
            }
          else
            goto unknown_default;
          break;
        case 340:
          if (mode == CONTENTFORMAT)
            {
              o->content_format.text_style
                  = dwg_add_handleref (dwg, 3, pair->value.u, NULL);
              LOG_TRACE ("%s.%s.content_format.text_style = " FORMAT_REF
                         " [H 3 %d]\n",
                         obj->name, key,
                         ARGS_REF (o->content_format.text_style), pair->code);
            }
          else if (mode == GRIDFORMAT)
            {
              if (grid < 0 || grid >= (int)o->num_borders)
                return NULL;
              o->borders[grid].ltype
                  = dwg_add_handleref (dwg, 3, pair->value.u, NULL);
              LOG_TRACE ("%s.%s.borders[%d].ltype = " FORMAT_REF " [H 3 %d]\n",
                         obj->name, key, grid,
                         ARGS_REF (o->borders[grid].ltype), pair->code);
            }
          else
            goto unknown_default;
          break;
        /*
        case 7:
          hdl = find_tablehandle (dwg, pair);
          assert (hdl);
          CHK_array (i, rowstyles);
          o->rowstyles[i].text_style = hdl;
          LOG_TRACE ("%s.rowstyles[%d].text_style = " FORMAT_REF " [H %d]\n",
                       obj->name, i, ARGS_REF(hdl), pair->code);
          break;
        case 1:
          CHK_array (i, rowstyles);
          o->rowstyles[i].format_string = bit_utf8_to_TU (pair->value.s, 0);
          LOG_TRACE ("%s.rowstyles[%d].format_string = %s [TU %d]\n",
                     obj->name, i, pair->value.s, pair->code);
          break;
        */
        default:
        unknown_default:
          LOG_ERROR ("Unknown DXF code %d for %s.%s", pair->code, obj->name,
                     key);
        }
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
    }
  return pair;
}

// returns with 0
static Dxf_Pair *
add_TABLESTYLE (Dwg_Object *restrict obj, Bit_Chain *restrict dat,
                Dxf_Pair *restrict pair)
{
  Dwg_Data *dwg = obj->parent;
  Dwg_Object_TABLESTYLE *o = obj->tio.object->tio.TABLESTYLE;
  BITCODE_H hdl;
  int i = -1, j = -1;

#  define CHK_rowstyles                                                       \
    if (!(i >= 0 && i < 3) || !o->num_rowstyles || !o->rowstyles)             \
      return NULL;                                                            \
    assert (i >= 0 && i < 3);                                                 \
    assert (o->num_rowstyles);                                                \
    assert (o->rowstyles)

#  define CHK_borders                                                         \
    if (!(j >= 0 && j <= 6) || !o->rowstyles[i].num_borders                   \
        || !o->rowstyles[i].borders)                                          \
      return NULL;                                                            \
    assert (j >= 0 && j < 6);                                                 \
    assert (o->rowstyles[i].num_borders);                                     \
    assert (o->rowstyles[i].borders)

  while (pair != NULL && pair->code != 0)
    {
      switch (pair->code)
        {
        case 0:
          break;
        case 7:
          i++;
          CHK_rowstyles;
          hdl = find_tablehandle (dwg, pair);
          if (!hdl)
            return NULL;
          assert (hdl);
          o->rowstyles[i].text_style = hdl;
          LOG_TRACE ("%s.rowstyles[%d].text_style = " FORMAT_REF " [H %d]\n",
                     obj->name, i, ARGS_REF (hdl), pair->code);
          break;
        case 140:
          CHK_rowstyles;
          o->rowstyles[i].text_height = pair->value.d;
          LOG_TRACE ("%s.rowstyles[%d].text_height = %f [BD %d]\n", obj->name,
                     i, pair->value.d, pair->code);
          break;
        case 170:
          CHK_rowstyles;
          o->rowstyles[i].text_alignment = pair->value.i;
          LOG_TRACE ("%s.rowstyles[%d].text_alignment = " FORMAT_BS
                     " [BS %d]\n",
                     obj->name, i, o->rowstyles[i].text_alignment, pair->code);
          break;
        case 62:
          CHK_rowstyles;
          o->rowstyles[i].text_color.index = pair->value.i;
          // TODO rgb with 420
          LOG_TRACE ("%s.rowstyles[%d].text_color.index = %d [CMC %d]\n",
                     obj->name, i, pair->value.i, pair->code);
          break;
        case 63:
          CHK_rowstyles;
          o->rowstyles[i].fill_color.index = pair->value.i;
          LOG_TRACE ("%s.rowstyles[%d].fill_color.index = %d [CMC %d]\n",
                     obj->name, i, pair->value.i, pair->code);
          break;
        case 283:
          CHK_rowstyles;
          o->rowstyles[i].has_bgcolor = pair->value.i;
          LOG_TRACE ("%s.rowstyles[%d].has_bgcolor = %d [B %d]\n", obj->name,
                     i, pair->value.i, pair->code);
          break;
        case 90:
          CHK_rowstyles;
          o->rowstyles[i].data_type = pair->value.i;
          LOG_TRACE ("%s.rowstyles[%d].data_type = %d [BL %d]\n", obj->name, i,
                     pair->value.i, pair->code);
          break;
        case 91:
          CHK_rowstyles;
          o->rowstyles[i].unit_type = pair->value.i;
          LOG_TRACE ("%s.rowstyles[%d].unit_type = %d [BL %d]\n", obj->name, i,
                     pair->value.i, pair->code);
          break;
        case 1:
          CHK_rowstyles;
          o->rowstyles[i].format_string = bit_utf8_to_TU (pair->value.s, 0);
          LOG_TRACE ("%s.rowstyles[%d].format_string = %s [TU %d]\n",
                     obj->name, i, pair->value.s, pair->code);
          break;
        case 274:
        case 275:
        case 276:
        case 277:
        case 278:
        case 279:
          CHK_rowstyles;
          j = pair->code - 274;
          if (j < 0 || j >= 6)
            return NULL;
          assert (j >= 0 && j <= 6);
          if (!o->rowstyles[i].borders)
            {
              o->rowstyles[i].borders = (Dwg_TABLESTYLE_border *)xcalloc (
                  6, sizeof (Dwg_TABLESTYLE_border));
              if (!o->rowstyles[i].borders)
                {
                  o->rowstyles[i].num_borders = 0;
                  return NULL;
                }
              o->rowstyles[i].num_borders = 6;
            }
          CHK_borders;
          assert (o->rowstyles[i].num_borders);
          o->rowstyles[i].borders[j].linewt = dxf_find_lweight ((int16_t)pair->value.i);
          LOG_TRACE ("%s.rowstyles[%d].borders[%d].linewt = %d [BSd %d]\n",
                     obj->name, i, j, o->rowstyles[i].borders[j].linewt,
                     pair->code);
          break;
        case 284:
        case 285:
        case 286:
        case 287:
        case 288:
        case 289:
          CHK_rowstyles;
          j = pair->code - 284;
          CHK_borders;
          o->rowstyles[i].borders[j].visible = pair->value.i;
          LOG_TRACE ("%s.rowstyles[%d].borders[%d].visible = %d [B %d]\n",
                     obj->name, i, j, pair->value.i, pair->code);
          break;
        case 64:
        case 65:
        case 66:
        case 67:
        case 68:
        case 69:
          CHK_rowstyles;
          j = pair->code - 64;
          CHK_borders;
          assert (j >= 0 && j <= 6);
          o->rowstyles[i].borders[j].color.index = pair->value.i;
          LOG_TRACE (
              "%s.rowstyles[%d].borders[%d].color.index = %d [CMC %d]\n",
              obj->name, i, j, pair->value.i, pair->code);
          break;
        default:
          if (is_type_stable (obj->fixedtype))
            LOG_ERROR ("Unknown DXF code %d for %s", pair->code, "TABLESTYLE")
          else
            LOG_WARN ("Unknown DXF code %d for %s", pair->code, "TABLESTYLE");
        }
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
    }
  return pair;
}

// returns with 0
static Dxf_Pair *
add_TABLEGEOMETRY_Cell (Dwg_Object *restrict obj, Bit_Chain *restrict dat,
                        Dxf_Pair *restrict pair)
{
  Dwg_Data *dwg = obj->parent;
  Dwg_Object_TABLEGEOMETRY *o = obj->tio.object->tio.TABLEGEOMETRY;
  BITCODE_H hdl;
  BITCODE_BL num_cells = o->num_cells;
  int i = -1, j = -1;

  if (num_cells < 1)
    {
      return NULL;
    }

  o->cells = (Dwg_TABLEGEOMETRY_Cell *)xcalloc (
      num_cells, sizeof (Dwg_TABLEGEOMETRY_Cell));
  if (!o->cells)
    {
      o->num_cells = 0;
      return NULL;
    }

  while (pair != NULL && pair->code != 0)
    {
      switch (pair->code)
        {
        case 0:
          break;
        case 93:
          i++; // the first
#  define CHK_cells                                                           \
    if (i < 0 || i >= (int)num_cells || !o->cells)                            \
      return NULL;                                                            \
    assert (i >= 0 && i < (int)num_cells);                                    \
    assert (o->cells)

          CHK_cells;
          o->cells[i].geom_data_flag = pair->value.i;
          LOG_TRACE ("%s.cells[%d].geom_data_flag = " FORMAT_BL " [BL %d]\n",
                     obj->name, i, o->cells[i].geom_data_flag, pair->code);
          break;
        case 40:
          CHK_cells;
          o->cells[i].width_w_gap = pair->value.d;
          LOG_TRACE ("%s.cells[%d].width_w_gap = %f [BD %d]\n", obj->name, i,
                     pair->value.d, pair->code);
          break;
        case 41:
          CHK_cells;
          o->cells[i].height_w_gap = pair->value.d;
          LOG_TRACE ("%s.cells[%d].height_w_gap = %f [BD %d]\n", obj->name, i,
                     pair->value.d, pair->code);
          break;
        case 330:
          CHK_cells;
          hdl = find_tablehandle (dwg, pair);
          if (hdl)
            {
              if (hdl->handleref.code != 4) // turn the 5 into a 4
                o->cells[i].tablegeometry
                    = dwg_add_handleref (dwg, 4, hdl->handleref.value, NULL);
              else
                o->cells[i].tablegeometry = hdl;
            }
          else
            o->cells[i].tablegeometry = dwg_add_handleref (dwg, 4, 0, NULL);
          LOG_TRACE ("%s.cells[%d].tablegeometry = " FORMAT_REF " [H %d]\n",
                     obj->name, i, ARGS_REF (o->cells[i].tablegeometry),
                     pair->code);
          break;
        case 94:
          CHK_cells;
          o->cells[i].num_geometry = pair->value.i;
          LOG_TRACE ("%s.cells[%d].num_geometry = " FORMAT_BL " [BL %d]\n",
                     obj->name, i, o->cells[i].num_geometry, pair->code);
          o->cells[i].geometry = (Dwg_CellContentGeometry *)xcalloc (
              pair->value.i, sizeof (Dwg_CellContentGeometry));
          if (!o->cells[i].geometry)
            {
              o->cells[i].num_geometry = 0;
              return NULL;
            }
          j = -1;
          break;
        case 10:
          CHK_cells;
          j++;

#  define CHK_geometry                                                        \
    if (j < 0 || j >= (int)o->cells[i].num_geometry || !o->cells[i].geometry) \
      return NULL;                                                            \
    assert (j >= 0 && j < (int)o->cells[i].num_geometry);                     \
    assert (o->cells[i].geometry)

          CHK_geometry;
          o->cells[i].geometry[j].dist_top_left.x = pair->value.d;
          LOG_TRACE (
              "%s.cells[%d].geometry[%d].dist_top_left.x = %f [BD %d]\n",
              obj->name, i, j, pair->value.d, pair->code);
          break;
        case 20:
          CHK_cells;
          CHK_geometry;
          o->cells[i].geometry[j].dist_top_left.y = pair->value.d;
          break;
        case 30:
          CHK_cells;
          CHK_geometry;
          o->cells[i].geometry[j].dist_top_left.z = pair->value.d;
          LOG_TRACE ("%s.cells[%d].geometry[%d].dist_top_left = ( %f, %f, %f) "
                     "[3BD 10]\n",
                     obj->name, i, j, o->cells[i].geometry[j].dist_top_left.x,
                     o->cells[i].geometry[j].dist_top_left.y, pair->value.d);
          break;
        case 11:
          CHK_cells;
          CHK_geometry;
          o->cells[i].geometry[j].dist_center.x = pair->value.d;
          LOG_TRACE ("%s.cells[%d].geometry[%d].dist_center.x = %f [BD %d]\n",
                     obj->name, i, j, pair->value.d, pair->code);
          break;
        case 21:
          CHK_cells;
          CHK_geometry;
          o->cells[i].geometry[j].dist_center.y = pair->value.d;
          break;
        case 31:
          CHK_cells;
          CHK_geometry;
          o->cells[i].geometry[j].dist_center.z = pair->value.d;
          LOG_TRACE ("%s.cells[%d].geometry[%d].dist_center = ( %f, %f, %f) "
                     "[3BD 10]\n",
                     obj->name, i, j, o->cells[i].geometry[j].dist_center.x,
                     o->cells[i].geometry[j].dist_center.y, pair->value.d);
          break;
        case 43:
          CHK_cells;
          CHK_geometry;
          o->cells[i].geometry[j].content_width = pair->value.d;
          LOG_TRACE ("%s.cells[%d].geometry[%d].content_width = %f [BD %d]\n",
                     obj->name, i, j, pair->value.d, pair->code);
          break;
        case 44:
          CHK_cells;
          CHK_geometry;
          o->cells[i].geometry[j].content_height = pair->value.d;
          LOG_TRACE ("%s.cells[%d].geometry[%d].content_height = %f [BD %d]\n",
                     obj->name, i, j, pair->value.d, pair->code);
          break;
        case 45:
          CHK_cells;
          CHK_geometry;
          o->cells[i].geometry[j].width = pair->value.d;
          LOG_TRACE ("%s.cells[%d].geometry[%d].width = %f [BD %d]\n",
                     obj->name, i, j, pair->value.d, pair->code);
          break;
        case 46:
          CHK_cells;
          CHK_geometry;
          o->cells[i].geometry[j].height = pair->value.d;
          LOG_TRACE ("%s.cells[%d].geometry[%d].height = %f [BD %d]\n",
                     obj->name, i, j, pair->value.d, pair->code);
          break;
        case 95:
          CHK_cells;
          CHK_geometry;
          o->cells[i].geometry[j].unknown = pair->value.i;
          LOG_TRACE ("%s.cells[%d].geometry[%d].unknown = %d [BL %d]\n",
                     obj->name, i, j, pair->value.i, pair->code);
          break;
        default:
          LOG_ERROR ("Unknown DXF code %d for %s", pair->code, "TABLESTYLE");
        }
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
    }
  return pair;
}

#  undef CHK_cells
#  undef CHK_geometry

// starts with 71 or 75
static Dxf_Pair *
add_DIMASSOC (Dwg_Object *restrict obj, Bit_Chain *restrict dat,
              Dxf_Pair *restrict pair)
{
  Dwg_Object_DIMASSOC *o = obj->tio.object->tio.DIMASSOC;
  Dwg_Data *dwg = obj->parent;
  int i = -1, j = -1;
  int have_rotated_type = 0;
  o->ref = (Dwg_DIMASSOC_Ref *)xcalloc (4, sizeof (Dwg_DIMASSOC_Ref));
  if (!o->ref)
    {
      return NULL;
    }

  while (pair != NULL && pair->code != 0)
    {
      switch (pair->code)
        {
        case 0:
          break;
        case 71: // always before each ref
          i++;
          while (!(o->associativity & (1 << i)) && i < 4) // popcount x
            i++;
          if (!(i >= 0 && i <= 3))
            {
              LOG_ERROR ("Invalid DIMASSOC_Ref index %d", i)
              return pair;
            }
          o->rotated_type = pair->value.i;
          have_rotated_type = 1; // early bump
          LOG_TRACE ("%s.rotated_type = %d [BS %d]\n", obj->name,
                     pair->value.i, pair->code);
          break;
        case 1:
          if (strNE (pair->value.s, "AcDbOsnapPointRef"))
            {
              LOG_ERROR ("Invalid DIMASSOC subclass %s", pair->value.s);
              return pair;
            }
          if (!(i >= 0 && i <= 3))
            {
              LOG_ERROR ("Invalid DIMASSOC_Ref index %d", i)
              return pair;
            }
          if (!have_rotated_type) // not already bumped
            {
              i++;
              while (!(o->associativity & (1 << i)) && i < 4) // popcount x
                i++;
              if (i > 3)
                i = 3;
              assert (i >= 0 && i <= 3);
            }
          o->ref[i].classname = dwg_add_u8_input (dwg, pair->value.s);
          LOG_TRACE ("%s.ref[%d].classname = %s [T %d]\n", obj->name, i,
                     pair->value.s, pair->code);
          have_rotated_type = 0;
          break;
        case 72:
          if (i < 0 || i > 3)
            break;
          o->ref[i].osnap_type = pair->value.i;
          LOG_TRACE ("%s.ref[%d].osnap_type = %d [RC %d]\n", obj->name, i,
                     pair->value.i, pair->code);
          break;
        // FIXME: 301 xrefpaths[j]
        case 331:
          {
            BITCODE_BS n;
            if (i < 0 || i > 3)
              break;
            n = o->ref[i].num_xrefs;
            o->ref[i].xrefs = (BITCODE_H *)realloc (
                o->ref[i].xrefs, (n + 1) * sizeof (BITCODE_H));
            o->ref[i].xrefs[n]
                = dwg_add_handleref (dwg, 5, pair->value.u, obj);
            LOG_TRACE ("%s.ref[%d].xrefs[%d] = " FORMAT_REF " [H* %d]\n",
                       obj->name, i, n, ARGS_REF (o->ref[i].xrefs[n]),
                       pair->code);
            o->ref[i].num_xrefs++;
          }
          break;
        case 74:
          if (i < 0 || i > 3)
            break;
          o->ref[i].num_intsectobj = pair->value.i;
          o->ref[i].intsectobj
              = (BITCODE_H *)xcalloc (pair->value.i, sizeof (BITCODE_H));
          if (!o->ref[i].intsectobj)
            {
              o->ref[i].num_intsectobj = 0;
              return NULL;
            }
          j = 0;
          LOG_TRACE ("%s.ref[%d].num_intsectobj = %d [BS %d]\n", obj->name, i,
                     pair->value.i, pair->code);
          break;
        case 332:
          if (i < 0 || j < 0 || i > 3 || j >= (int)o->ref[i].num_intsectobj)
            break;
          o->ref[i].intsectobj[j]
              = dwg_add_handleref (dwg, 5, pair->value.u, obj);
          LOG_TRACE ("%s.ref[%d].intsectobj[%d] = " FORMAT_REF " [H %d]\n",
                     obj->name, i, j, ARGS_REF (o->ref[i].intsectobj[j]),
                     pair->code);
          j++;
          break;
        case 73:
          if (i < 0)
            break;
          o->ref[i].main_subent_type = pair->value.i;
          LOG_TRACE ("%s.ref[%d].main_subent_type = %d [BS %d]\n", obj->name,
                     i, pair->value.i, pair->code);
          break;
        case 75:
          if (i < 0)
            break;
          o->ref[i].has_lastpt_ref = pair->value.i;
          LOG_TRACE ("%s.ref[%d].has_lastpt_ref = %d [B %d]\n", obj->name, i,
                     pair->value.i, pair->code);
          break;
        case 91:
          if (i < 0)
            break;
          o->ref[i].main_gsmarker = pair->value.i;
          LOG_TRACE ("%s.ref[%d].main_gsmarker = %d [BL %d]\n", obj->name, i,
                     pair->value.i, pair->code);
          break;
        case 40:
          if (i < 0)
            break;
          o->ref[i].osnap_dist = pair->value.d;
          LOG_TRACE ("%s.ref[%d].osnap_dist = %f [BD %d]\n", obj->name, i,
                     pair->value.d, pair->code);
          break;
        case 10:
          if (i < 0)
            break;
          o->ref[i].osnap_pt.x = pair->value.d;
          break;
        case 20:
          if (i < 0)
            break;
          o->ref[i].osnap_pt.y = pair->value.d;
          break;
        case 30:
          if (i < 0)
            break;
          o->ref[i].osnap_pt.z = pair->value.d;
          LOG_TRACE ("%s.ref[%d].osnap_pt = (%f, %f, %f) [3BD 10]\n",
                     obj->name, i, o->ref[i].osnap_pt.x, o->ref[i].osnap_pt.y,
                     pair->value.d);
          break;
        default:
          LOG_ERROR ("Unknown DXF code %d for %s", pair->code, "DIMASSOC");
        }
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
    }
  return pair;
}

// starting with 90 or 8
static Dxf_Pair *
add_LAYER_entry (Dwg_Object *restrict obj, Bit_Chain *restrict dat,
                 Dxf_Pair *restrict pair)
{
  Dwg_Object_LAYER_INDEX *o = obj->tio.object->tio.LAYER_INDEX;
  Dwg_Data *dwg = obj->parent;
  int i = 0;
  o->entries = (Dwg_LAYER_entry *)xcalloc (1, sizeof (Dwg_LAYER_entry));
  o->num_entries = 1;
  if (!o->entries)
    {
      o->num_entries = 0;
      return NULL;
    }
  if (pair->code == 90 && !pair->value.i)
    {
      LOG_TRACE ("skip first %s.entries[%d].numlayers = %d [BL %d]\n",
                 obj->name, i, pair->value.i, pair->code);
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
    }
  while (pair != NULL && pair->code != 0)
    {
      switch (pair->code)
        {
        case 0:
          break;
        case 8:
          o->entries[i].name = dwg_add_u8_input (dwg, pair->value.s);
          LOG_TRACE ("%s.entries[%d].name = %s [T %d]\n", obj->name, i,
                     pair->value.s, pair->code);
          break;
        case 360:
          o->entries[i].handle
              = dwg_add_handleref (dwg, 5, pair->value.u, obj);
          LOG_TRACE ("%s.entries[%d].handle = " FORMAT_REF " [H %d]\n",
                     obj->name, i, ARGS_REF (o->entries[i].handle),
                     pair->code);
          break;
        case 90:
          o->entries[i].numlayers = pair->value.i;
          LOG_TRACE ("%s.entries[%d].numlayers = %d [BL %d]\n", obj->name, i,
                     pair->value.i, pair->code);
          o->num_entries++;
          i++;
          o->entries = (Dwg_LAYER_entry *)realloc (
              o->entries, o->num_entries * sizeof (Dwg_LAYER_entry));
          if (!o->entries)
            {
              o->num_entries = 0;
              return NULL;
            }
          else
            {
              memset (&o->entries[o->num_entries - 1], 0, sizeof (Dwg_LAYER_entry));
            }
          break;
        default:
          o->entries = NULL;
          o->num_entries = 0;
          LOG_ERROR ("Unknown DXF code %d for %s", pair->code, "LAYER_INDEX");
          return NULL;
        }
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
    }
  o->num_entries--;
  LOG_TRACE ("%s.num_entries = %d [BL]\n", obj->name, o->num_entries);
  return pair;
}

static Dxf_Pair *
add_EVALVARIANT (Dwg_Data *restrict dwg, Bit_Chain *restrict dat,
                 Dwg_EvalVariant *value)
{
  Dxf_Pair *pair = dxf_read_pair (dat);
  EXPECT_DXF ("EvalVariant", code, 70);
  value->code = pair->value.i;
  LOG_TRACE ("%s.%s = %d [BL %d]\n", "EvalVariant", "code", pair->value.i,
             pair->code);
  dxf_free_pair (pair);

  pair = dxf_read_pair (dat);
  if (!pair || pair->code == 0)
    return pair;
  switch (dwg_resbuf_value_type (pair->value.i))
    {
    case DWG_VT_REAL:
      value->u.bd = pair->value.d;
      LOG_TRACE ("%s.%s = %f [BD %d]\n", "EvalVariant", "value", pair->value.d,
                 pair->code);
      break;
    case DWG_VT_INT32:
      value->u.bl = pair->value.u;
      LOG_TRACE ("%s.%s = %u [BL %d]\n", "EvalVariant", "value", pair->value.u,
                 pair->code);
      break;
    case DWG_VT_INT16:
      value->u.bs = pair->value.i;
      LOG_TRACE ("%s.%s = %d [BS %d]\n", "EvalVariant", "value", pair->value.i,
                 pair->code);
      break;
    case DWG_VT_INT8:
      value->u.rc = pair->value.i;
      LOG_TRACE ("%s.%s = %d [RC %d]\n", "EvalVariant", "value", pair->value.i,
                 pair->code);
      break;
    case DWG_VT_STRING:
      value->u.text = dwg_add_u8_input (dwg, pair->value.s);
      LOG_TRACE ("%s.%s = %s [T %d]\n", "EvalVariant", "value", pair->value.s,
                 pair->code);
      break;
    case DWG_VT_HANDLE:
      value->u.handle = dwg_add_handleref (dwg, 5, pair->value.u, NULL);
      LOG_TRACE ("%s.%s = " FORMAT_REF " [H %d]\n", "EvalVariant", "value",
                 ARGS_REF (value->u.handle), pair->code);
      break;
    case DWG_VT_BINARY:
    case DWG_VT_OBJECTID:
    case DWG_VT_POINT3D:
    case DWG_VT_INVALID:
    case DWG_VT_INT64:
    case DWG_VT_BOOL:
    default:
      LOG_ERROR ("Invalid EvalVariant.value.type %d", pair->code)
      break;
    }
  dxf_free_pair (pair);
  return NULL;
}

static int
add_VALUEPARAMs (Dwg_Data *restrict dwg, Bit_Chain *restrict dat,
                 Dwg_VALUEPARAM *value)
{
  Dxf_Pair *pair = dxf_read_pair (dat);
  if (pair == NULL || pair->code != 90)
    {
      LOG_ERROR ("%s: Unexpected DXF code %d, expected %d for %s",
                 "VALUEPARAM", pair ? pair->code : -1, 90, "class_version");
      return 0;
    }
  value->class_version = pair->value.u;
  LOG_TRACE ("%s.%s = %d [BL %d]\n", "VALUEPARAM", "class_version",
             pair->value.i, pair->code);
  dxf_free_pair (pair);

  pair = dxf_read_pair (dat);
  if (pair == NULL || pair->code != 1)
    {
      LOG_ERROR ("%s: Unexpected DXF code %d, expected %d for %s",
                 "VALUEPARAM", pair ? pair->code : -1, 1, "name");
      return 0;
    }
  value->name = dwg_add_u8_input (dwg, pair->value.s);
  LOG_TRACE ("%s.%s = %s [BL %d]\n", "VALUEPARAM", "name", pair->value.s,
             pair->code);
  dxf_free_pair (pair);

  pair = dxf_read_pair (dat);
  if (pair == NULL || pair->code != 90)
    {
      LOG_ERROR ("%s: Unexpected DXF code %d, expected %d for %s",
                 "VALUEPARAM", pair ? pair->code : -1, 90, "unit_type");
      return 0;
    }
  value->unit_type = pair->value.u;
  LOG_TRACE ("%s.%s = %d [BL %d]\n", "VALUEPARAM", "unit_type", pair->value.i,
             pair->code);
  dxf_free_pair (pair);

  pair = dxf_read_pair (dat);
  if (pair == NULL || pair->code != 90)
    {
      LOG_ERROR ("%s: Unexpected DXF code %d, expected %d for %s",
                 "VALUEPARAM", pair ? pair->code : -1, 90, "num_vars");
      return 0;
    }
  value->num_vars = pair->value.u;
  LOG_TRACE ("%s.%s = %d [BL %d]\n", "VALUEPARAM", "num_vars", pair->value.i,
             pair->code);
  value->vars = (Dwg_VALUEPARAM_vars *)xcalloc (value->num_vars,
                                                sizeof (Dwg_VALUEPARAM_vars));
  if (!value->vars)
    {
      value->num_vars = 0;
      return 0;
    }
  for (unsigned j = 0; j < value->num_vars; j++)
    {
      pair = add_EVALVARIANT (dwg, dat, &value->vars[j].value);
      if (pair)
        return 0;
      pair = dxf_read_pair (dat);
      if (pair == NULL || pair->code != 330)
        {
          LOG_ERROR ("%s: Unexpected DXF code %d, expected %d for %s",
                     "VALUEPARAM", pair ? pair->code : -1, 330, "handle");
          return 0;
        }
      value->vars[j].handle = dwg_add_handleref (dwg, 4, pair->value.u, NULL);
      LOG_TRACE ("%s.vars[%u].handle = " FORMAT_REF " [H %d]\n", "VALUEPARAM",
                 j, ARGS_REF (value->vars[j].handle), pair->code);
      dxf_free_pair (pair);
    }
  return 1;
}

static Dxf_Pair *
add_EVAL_Edge (Dwg_Object *restrict obj, Bit_Chain *restrict dat,
               Dxf_Pair *restrict pair)
{
  Dwg_Object_EVALUATION_GRAPH *o = obj->tio.object->tio.EVALUATION_GRAPH;
  Dwg_Data *dwg = obj->parent;
  int i = -1;
  o->edges = (Dwg_EVAL_Edge *)xcalloc (1, sizeof (Dwg_EVAL_Edge));
  o->num_edges = 0;
  if (!o->edges)
    return NULL;

  while (pair != NULL && pair->code == 92)
    {
      i++;
      o->edges = (Dwg_EVAL_Edge *)realloc (
          o->edges, o->num_edges * sizeof (Dwg_EVAL_Edge));
      if (!o->edges)
        {
          o->num_edges = 0;
          return NULL;
        }
      o->num_edges++;
      o->edges[i].id = pair->value.i; // 92
      LOG_TRACE ("%s.edges[%d].id = %d [BL %d]\n", obj->name, i, pair->value.i,
                 pair->code);
      dxf_free_pair (pair);

      pair = dxf_read_pair (dat);
      if (!pair || pair->code != 93)
        {
          LOG_ERROR ("Unknown DXF code %d for %s", pair->code, obj->name);
          return pair;
        }
      o->edges[i].nextid = pair->value.i; // 93
      LOG_TRACE ("%s.edges[%d].nextid = %d [BL %d]\n", obj->name, i,
                 pair->value.i, pair->code);
      dxf_free_pair (pair);

      pair = dxf_read_pair (dat);
      if (!pair || pair->code != 94)
        {
          LOG_ERROR ("Unknown DXF code %d for %s", pair->code, obj->name);
          return pair;
        }
      o->edges[i].e1 = pair->value.i; // 94
      LOG_TRACE ("%s.edges[%d].e1 = %d [BL %d]\n", obj->name, i, pair->value.i,
                 pair->code);
      dxf_free_pair (pair);

      pair = dxf_read_pair (dat);
      if (!pair || pair->code != 91)
        {
          LOG_ERROR ("Unknown DXF code %d for %s", pair->code, obj->name);
          return pair;
        }
      o->edges[i].e2 = pair->value.i; // 91
      LOG_TRACE ("%s.edges[%d].e2 = %d [BL %d]\n", obj->name, i, pair->value.i,
                 pair->code);
      dxf_free_pair (pair);

      pair = dxf_read_pair (dat);
      if (!pair || pair->code != 91)
        {
          LOG_ERROR ("Unknown DXF code %d for %s", pair->code, obj->name);
          return pair;
        }
      o->edges[i].e3 = pair->value.i; // 91
      LOG_TRACE ("%s.edges[%d].e3 = %d [BL %d]\n", obj->name, i, pair->value.i,
                 pair->code);
      dxf_free_pair (pair);

      for (int j = 0; j < 5; j++)
        {
          pair = dxf_read_pair (dat);
          if (!pair || pair->code != 92)
            {
              LOG_ERROR ("Unknown DXF code %d for %s", pair->code, obj->name);
              return pair;
            }
          o->edges[i].out_edge[j] = pair->value.i; // 92
          LOG_TRACE ("%s.edges[%d].out_edge[%d] = %d [BL %d]\n", obj->name, i,
                     j, pair->value.i, pair->code);
          dxf_free_pair (pair);
        }

      pair = dxf_read_pair (dat);
    }
  LOG_TRACE ("%s.num_edges = %d [BL]\n", obj->name, o->num_edges);
  return pair;
}

/* Starts with first AcDbEvalGraph 91. FIXME edges */
static Dxf_Pair *
add_EVAL_Node (Dwg_Object *restrict obj, Bit_Chain *restrict dat,
               Dxf_Pair *restrict pair)
{
  Dwg_Object_EVALUATION_GRAPH *o = obj->tio.object->tio.EVALUATION_GRAPH;
  Dwg_Data *dwg = obj->parent;
  int i = -1, j = 0;
  o->nodes = (Dwg_EVAL_Node *)xcalloc (1, sizeof (Dwg_EVAL_Node));
  o->num_nodes = 1;
  if (!o->nodes)
    {
      o->num_nodes = 0;
      return NULL;
    }
  while (pair != NULL && pair->code != 0)
    {
      switch (pair->code)
        {
        case 91:
          i++;
          o->nodes = (Dwg_EVAL_Node *)realloc (
              o->nodes, (i + 1) * sizeof (Dwg_EVAL_Node));
          if (!o->nodes)
            break;
          o->num_nodes = i + 1;
          o->nodes[i].id = pair->value.i;
          LOG_TRACE ("%s.nodes[%d].id = %d [BL %d]\n", obj->name, i,
                     pair->value.i, pair->code);
          break;
        case 93: // must be 32
          o->nodes[i].edge_flags = pair->value.i;
          LOG_TRACE ("%s.nodes[%d].edge_flags = %d [BL %d]\n", obj->name, i,
                     pair->value.i, pair->code);
          break;
        case 95:
          o->nodes[i].edge_flags = pair->value.i;
          if (pair->value.i != 32)
            LOG_WARN ("%s.nodes[%d].edge_flags = %d [BL %d] != 32", obj->name,
                      i, pair->value.i, pair->code)
          else
            LOG_TRACE ("%s.nodes[%d].edge_flags = %d [BL %d]\n", obj->name, i,
                       pair->value.i, pair->code)
          break;
        case 360:
          o->nodes[i].evalexpr
              = dwg_add_handleref (dwg, 5, pair->value.u, obj);
          LOG_TRACE ("%s.nodes[%d].evalexpr = " FORMAT_REF " [H %d]\n",
                     obj->name, i, ARGS_REF (o->nodes[i].evalexpr),
                     pair->code);
          break;
        case 92:
          if (j > 3) // 0 - 3
            {        // list of edges
              return add_EVAL_Edge (obj, dat, pair);
            }
          o->nodes[i].node[j] = pair->value.i;
          LOG_TRACE ("%s.nodes[%d].edge[%d] = %d [BL %d]\n", obj->name, i, j,
                     pair->value.i, pair->code);
          j++;
          break;
        default:
          LOG_ERROR ("Unknown DXF code %d for %s", pair->code, obj->name);
          return NULL;
        }
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
    }
  LOG_TRACE ("%s.num_nodes = %d [BL]\n", obj->name, o->num_nodes);
  return pair;
}

static Dxf_Pair *
add_ASSOCNETWORK (Dwg_Object *restrict obj, Bit_Chain *restrict dat,
                  Dxf_Pair *restrict pair)
{
  Dwg_Object_ASSOCNETWORK *o = obj->tio.object->tio.ASSOCNETWORK;
  Dwg_Data *dwg = obj->parent;
  BITCODE_BL num;
  BITCODE_H *hv = NULL;
  Dwg_ASSOCACTION_Deps *deps;

  if (pair == NULL || pair->code != 90)
    return pair;
  EXPECT_INT_DXF ("network_version", 90, BS);
  FIELD_BL (network_action_index, 90);
  FIELD_BL (num_actions, 90);
  num = o->num_actions;

  deps = (Dwg_ASSOCACTION_Deps *)xcalloc (num, sizeof (Dwg_ASSOCACTION_Deps));
  if (!deps)
    {
      o->num_actions = 0;
      return NULL;
    }
  for (unsigned i = 0; i < num; i++)
    {
      BITCODE_H hdl;
      int is_owned, code;
      pair = dxf_read_pair (dat);
      if (pair && (pair->code == 360 || pair->code == 330))
        deps[i].is_owned = pair->code == 360;
      else
        {
          LOG_ERROR ("Invalid ASSOCACTION.deps[%d].is_owned DXF code %d", i,
                     pair ? pair->code : 0);
          return NULL;
        }
      code = deps[i].is_owned ? DWG_HDL_HARDOWN : DWG_HDL_SOFTPTR;
      deps[i].dep = dwg_add_handleref (dwg, code, pair->value.u, obj);
      LOG_TRACE ("%s.%s[%u] = " FORMAT_REF " [H %d]\n", obj->name, "deps", i,
                 ARGS_REF (deps[i].dep), pair->code);
      dxf_free_pair (pair);
    }
  dwg_dynapi_entity_set_value (o, obj->name, "actions", &deps, 1);

  pair = dxf_read_pair (dat);
  num = pair ? pair->value.u : 0;
  EXPECT_INT_DXF ("num_owned_actions", 90, BL);
  if (num)
    {
      hv = (BITCODE_H *)xcalloc (num, sizeof (BITCODE_H));
      if (!hv)
        return NULL;
    }
  for (unsigned i = 0; i < num; i++)
    {
      BITCODE_H hdl;
      pair = dxf_read_pair (dat);
      if (!pair || pair->code != 330)
        {
          LOG_ERROR ("Invalid ASSOCNETWORK.owned_actions[%d] DXF code %d", i,
                     pair ? pair->code : 0);
          return NULL;
        }
      hdl = dwg_add_handleref (dwg, 3, pair->value.u, obj);
      LOG_TRACE ("%s.%s[%d] = " FORMAT_REF " [H %d]\n", obj->name,
                 "owned_actions", i, ARGS_REF (hdl), pair->code);
      hv[i] = hdl;
      dxf_free_pair (pair);
    }
  if (num)
    dwg_dynapi_entity_set_value (o, obj->name, "owned_actions", &hv, 1);
  return NULL;
}

// with ASSOC2DCONSTRAINTGROUP, ASSOCNETWORK, ASSOCACTION
static Dxf_Pair *
add_ASSOCACTION (Dwg_Object *restrict obj, Bit_Chain *restrict dat,
                 Dxf_Pair *restrict pair)
{
  Dwg_Object_ASSOCACTION *o = obj->tio.object->tio.ASSOCACTION;
  Dwg_Data *dwg = obj->parent;
  BITCODE_BL num;
  BITCODE_H *hv = NULL;
  Dwg_VALUEPARAM *values = NULL;
  Dwg_ASSOCACTION_Deps *deps;
  unsigned class_version;

  if (pair == NULL || pair->code != 90)
    return pair;
  class_version = pair->value.u;
  EXPECT_INT_DXF ("class_version", 90, BS);
  FIELD_BL (geometry_status, 90);
  FIELD_HANDLE (owningnetwork, 4, 330);
  FIELD_HANDLE (actionbody, 3, 360);
  FIELD_BL (action_index, 90);
  FIELD_BL (max_assoc_dep_index, 90);

  FIELD_BL (num_deps, 90);
  num = o->num_deps;
  deps = (Dwg_ASSOCACTION_Deps *)xcalloc (num, sizeof (Dwg_ASSOCACTION_Deps));
  if (!deps)
    {
      o->num_deps = 0;
      return NULL;
    }
  for (unsigned i = 0; i < num; i++)
    {
      BITCODE_H hdl;
      int is_owned, code;
      pair = dxf_read_pair (dat);
      if (pair && (pair->code == 360 || pair->code == 330))
        deps[i].is_owned = pair->code == 360;
      else
        {
          LOG_ERROR ("Invalid ASSOCACTION.deps[%d].is_owned DXF code %d", i,
                     pair ? pair->code : 0);
          return NULL;
        }
      code = deps[i].is_owned ? DWG_HDL_HARDOWN : DWG_HDL_SOFTPTR;
      deps[i].dep = dwg_add_handleref (dwg, code, pair->value.u, obj);
      LOG_TRACE ("%s.%s[%u] = " FORMAT_REF " [H %d]\n", obj->name, "deps", i,
                 ARGS_REF (deps[i].dep), pair->code);
      dxf_free_pair (pair);
    }
  dwg_dynapi_entity_set_value (o, obj->name, "deps", &deps, 1);

  if (class_version > 1)
    {
      pair = dxf_read_pair (dat);
      num = pair ? pair->value.u : 0;
      EXPECT_INT_DXF ("num_owned_params", 90, BL);
      if (num)
        {
          hv = (BITCODE_H *)xcalloc (num, sizeof (BITCODE_H));
          if (!hv)
            return NULL;
        }
      for (unsigned i = 0; i < num; i++)
        {
          BITCODE_H hdl;
          pair = dxf_read_pair (dat);
          if (!pair || pair->type != DWG_VT_HANDLE)
            {
              LOG_ERROR ("Invalid ASSOCACTION.owned_params[%d] DXF code %d", i,
                         pair ? pair->code : 0);
              return NULL;
            }
          hdl = dwg_add_handleref (dwg, 3, pair->value.u, obj);
          LOG_TRACE ("%s.%s = " FORMAT_REF " [H %d]\n", obj->name,
                     "owned_params", ARGS_REF (hdl), pair->code);
          hv[i] = hdl;
          dxf_free_pair (pair);
        }
      if (num)
        dwg_dynapi_entity_set_value (o, obj->name, "owned_params", &hv, 1);

      pair = dxf_read_pair (dat);
      if (pair == NULL || pair->code != 90)
        return pair;
      // ignore the ValueParams class_version
      dxf_free_pair (pair);

      pair = dxf_read_pair (dat);
      num = pair ? pair->value.u : 0;
      EXPECT_INT_DXF ("num_values", 90, BL);
      if (num)
        {
          values = (Dwg_VALUEPARAM *)xcalloc (num, sizeof (Dwg_VALUEPARAM));
          if (!values)
            return NULL;
        }
      for (unsigned i = 0; i < num; i++)
        {
          if (!add_VALUEPARAMs (dwg, dat, &values[i]))
            return NULL;
        }
      if (num)
        dwg_dynapi_entity_set_value (o, obj->name, "values", &values, 1);
    }

  return NULL;
}

static Dxf_Pair *
add_RENDERENVIRONMENT (Dwg_Object *restrict obj, Bit_Chain *restrict dat)
{
  Dwg_Object_RENDERENVIRONMENT *o = obj->tio.object->tio.RENDERENVIRONMENT;
  Dwg_Data *dwg = obj->parent;
  Dxf_Pair *pair;
  BITCODE_RC r, g, b;

  FIELD_BL (class_version, 90);
  FIELD_B (fog_enabled, 290);
  FIELD_B (fog_background_enabled, 290);
  FIELD_RC (fog_color_r, 280);
  FIELD_RC (fog_color_g, 280);
  FIELD_RC (fog_color_b, 280);
  FIELD_BD (fog_density_near, 40); /* default 100.0 (opaque fog) */
  FIELD_BD (fog_density_far, 40);
  /* default 100.0 (at the far clipping plane) */
  FIELD_BD (fog_distance_near, 40);
  FIELD_BD (fog_distance_far, 40);
  FIELD_B (environ_image_enabled, 290);
  FIELD_T (environ_image_filename, 1);
  return NULL;
}
static Dxf_Pair *
add_RENDERSETTINGS (Dwg_Object *restrict obj, Bit_Chain *restrict dat)
{
  Dwg_Object_RENDERSETTINGS *o = obj->tio.object->tio.RENDERSETTINGS;
  Dwg_Data *dwg = obj->parent;
  Dxf_Pair *pair;

  FIELD_BL (class_version, 90);
  FIELD_T (name, 1);
  FIELD_B (fog_enabled, 290);
  FIELD_B (fog_background_enabled, 290);
  FIELD_B (backfaces_enabled, 290);
  FIELD_B (environ_image_enabled, 290);
  FIELD_T (environ_image_filename, 1);
  FIELD_T (description, 1);
  FIELD_BL (display_index, 90);
  VERSION (R_2013)
  {
    FIELD_B (has_predefined, 290);
  }
  return NULL;
}

static Dxf_Pair *
add_RENDERGLOBAL (Dwg_Object *restrict obj, Bit_Chain *restrict dat)
{
  Dwg_Object_RENDERGLOBAL *o = obj->tio.object->tio.RENDERGLOBAL;
  Dwg_Data *dwg = obj->parent;
  Dxf_Pair *pair;

  FIELD_BL (class_version, 90);
  FIELD_BL (procedure, 90);   /*!< 0 view, 1 crop, 2 selection */
  FIELD_BL (destination, 90); /*!< 0 window, 1 viewport */
  FIELD_B (save_enabled, 290);
  FIELD_T (save_filename, 1);
  FIELD_BL (image_width, 90);
  FIELD_BL (image_height, 90);
  FIELD_B (predef_presets_first, 290);
  FIELD_B (highlevel_info, 290);
  return NULL;
}

static Dxf_Pair *
add_RENDERENTRY (Dwg_Object *restrict obj, Bit_Chain *restrict dat)
{
  Dwg_Object_RENDERENTRY *o = obj->tio.object->tio.RENDERENTRY;
  Dwg_Data *dwg = obj->parent;
  Dxf_Pair *pair;

  FIELD_BL (class_version, 90);
  FIELD_T (image_file_name, 1);
  FIELD_T (preset_name, 1);
  FIELD_T (view_name, 1);
  FIELD_BL (dimension_x, 90);
  FIELD_BL (dimension_y, 90);
  FIELD_BS (start_year, 70);
  FIELD_BS (start_month, 70);
  FIELD_BS (start_day, 70);
  FIELD_BS (start_minute, 70);
  FIELD_BS (start_second, 70);
  FIELD_BS (start_msec, 70);
  FIELD_BD (render_time, 40);
  FIELD_BL (memory_amount, 90);
  FIELD_BL (material_count, 90);
  FIELD_BL (light_count, 90);
  FIELD_BL (triangle_count, 90);
  FIELD_BL (display_index, 90);
  return NULL;
}
// more Dynblocks:
static Dxf_Pair *
add_AcDbBlockParameter (Dwg_Object *restrict obj, Bit_Chain *restrict dat)
{
  Dwg_Object_BLOCKALIGNMENTGRIP *o = obj->tio.object->tio.BLOCKALIGNMENTGRIP;
  Dwg_Data *dwg = obj->parent;
  Dxf_Pair *pair;
  // SUBCLASS (AcDbBlockParameter);
  FIELD_B (show_properties, 280);
  FIELD_B (chain_actions, 281);
  return NULL;
}
#  if 0
static Dxf_Pair *
add_AcDbBlockGrip (Dwg_Object *restrict obj, Bit_Chain *restrict dat)
{
  Dwg_Object_BLOCKALIGNMENTGRIP *o = obj->tio.object->tio.BLOCKALIGNMENTGRIP;
  Dwg_Data *dwg = obj->parent;
  Dxf_Pair *pair;
  // SUBCLASS (AcDbBlockGrip)
  FIELD_BL (bg_bl91, 91);
  FIELD_BL (bg_bl92, 92);
  FIELD_3BD (bg_location, 1010);
  FIELD_B (bg_insert_cycling, 280);
  FIELD_BLd (bg_insert_cycling_weight, 93);
  return NULL;
}
#  endif
static Dxf_Pair *
add_AcDbBlockGripExpr (Dwg_Object *restrict obj, Bit_Chain *restrict dat)
{
  Dwg_Object_BLOCKALIGNMENTGRIP *o = obj->tio.object->tio.BLOCKALIGNMENTGRIP;
  Dwg_Data *dwg = obj->parent;
  Dxf_Pair *pair;
  // SUBCLASS (AcDbBlockGripExpr)
  FIELD_BL (grip_type, 91); /* ?? */
  FIELD_T (grip_expr, 300)
  return NULL;
}
// also for BLOCKLINEARGRIP
static Dxf_Pair *
add_BLOCKALIGNMENTGRIP (Dwg_Object *restrict obj, Bit_Chain *restrict dat)
{
  Dwg_Object_BLOCKALIGNMENTGRIP *o = obj->tio.object->tio.BLOCKALIGNMENTGRIP;
  Dwg_Data *dwg = obj->parent;
  Dxf_Pair *pair;
  FIELD_3BD_1 (orientation, 140);
  return NULL;
}
static Dxf_Pair *
add_BLOCKFLIPGRIP (Dwg_Object *restrict obj, Bit_Chain *restrict dat)
{
  Dwg_Object_BLOCKFLIPGRIP *o = obj->tio.object->tio.BLOCKFLIPGRIP;
  Dwg_Data *dwg = obj->parent;
  Dxf_Pair *pair;
  FIELD_3BD_1 (orientation, 140);
  FIELD_BL (combined_state, 93);
  return NULL;
}

static Dxf_Pair *
add_PERSUBENTMGR (Dwg_Object *restrict obj, Bit_Chain *restrict dat,
                  Dxf_Pair *restrict pair)
{
  Dwg_Object_PERSUBENTMGR *o = obj->tio.object->tio.PERSUBENTMGR;
  Dwg_Data *dwg = obj->parent;

  EXPECT_UINT_DXF ("class_version", 90, BL);
  FIELD_BL (unknown_0, 90);
  FIELD_BL (unknown_2, 90);
  FIELD_BL (numassocsteps, 90);
  FIELD_BL (numassocsubents, 90);
  FIELD_BL (num_steps, 90);
  if (o->num_steps > 0)
    {
      o->steps = (BITCODE_BL *)xcalloc (o->num_steps, sizeof (BITCODE_BL));
      if (!o->steps)
        {
          o->num_steps = 0;
          return pair;
        }
      for (unsigned i = 0; i < o->num_steps; i++)
        {
          pair = dxf_read_pair (dat);
          if (!pair || pair->code != 90)
            return pair;
          o->steps[i] = pair->value.u;
          LOG_TRACE ("%s.steps[%d] = %u [BL %d]\n", obj->name, i,
                     pair->value.u, pair->code);
          dxf_free_pair (pair);
        }
    }
  return NULL;
}

static Dxf_Pair *
add_ASSOCDEPENDENCY (Dwg_Object *restrict obj, Bit_Chain *restrict dat)
{
  Dwg_Object_ASSOCDEPENDENCY *o = obj->tio.object->tio.ASSOCDEPENDENCY;
  Dwg_Data *dwg = obj->parent;
  Dxf_Pair *pair;

  FIELD_BL (class_version, 90);
  FIELD_BL (status, 90);
  FIELD_B (is_read_dep, 290);
  FIELD_B (is_write_dep, 290);
  FIELD_B (is_attached_to_object, 290);
  FIELD_B (is_delegating_to_owning_action, 290);
  FIELD_BLd (order, 90); /* -1 or 0 */
  FIELD_HANDLE (dep_on, 3, 330);
  FIELD_B (has_name, 290);
  if (o->has_name)
    {
      FIELD_T (name, 1);
    }
  FIELD_HANDLE (readdep, 4, 330);
  FIELD_HANDLE (node, 3, 330);
  FIELD_HANDLE (dep_body, 4, 360);
  FIELD_BLd (depbodyid, 90);
  return NULL;
}

// ASSOCGEOMDEPENDENCY or ASSOCVALUEDEPENDENCY subclass
static Dxf_Pair *
add_sub_ASSOCDEPENDENCY (Dwg_Object *restrict obj, Bit_Chain *restrict dat)
{
  Dwg_Data *dwg = obj->parent;
  Dxf_Pair *pair;
  const Dwg_DYNAPI_field *f = dwg_dynapi_entity_field (obj->name, "assocdep");
  char *o = &((char *)obj->tio.object->tio.ASSOCGEOMDEPENDENCY)[f->offset];
  BITCODE_B has_name;

  SUB_FIELD_BL (assocdep, class_version, 90);
  SUB_FIELD_BL (assocdep, status, 90);
  SUB_FIELD_B (assocdep, is_read_dep, 290);
  SUB_FIELD_B (assocdep, is_write_dep, 290);
  SUB_FIELD_B (assocdep, is_attached_to_object, 290);
  SUB_FIELD_B (assocdep, is_delegating_to_owning_action, 290);
  SUB_FIELD_BLd (assocdep, order, 90); /* -1 or 0 */
  SUB_FIELD_HANDLE (assocdep, dep_on, 3, 330);
  SUB_FIELD_B (assocdep, has_name, 290);
  if (dwg_dynapi_subclass_value (o, "Dwg_Object_ASSOCDEPENDENCY", "has_name",
                                 &has_name, 0)
      && has_name)
    {
      SUB_FIELD_T (assocdep, name, 1);
    }
  SUB_FIELD_HANDLE (assocdep, readdep, 4, 330);
  SUB_FIELD_HANDLE (assocdep, node, 3, 330);
  SUB_FIELD_HANDLE (assocdep, dep_body, 4, 360);
  SUB_FIELD_BLd (assocdep, depbodyid, 90);
  return NULL;
}

// need fixed input, or we get a duplicate group 90, num_childs
static Dxf_Pair *
add_FIELD (Dwg_Object *restrict obj, Bit_Chain *restrict dat)
{
  Dwg_Object_FIELD *o = obj->tio.object->tio.FIELD;
  Dwg_Data *dwg = obj->parent;
  Dxf_Pair *pair;
  BITCODE_BL i;

  // SUBCLASS (AcDbField)
  FIELD_T (id, 1);
  FIELD_T (code, 2); // and code 3 for subsequent >255 chunks
  pair = dxf_read_pair (dat);
  if (pair->code == 4) // optional
    {
      o->format = strdup (pair->value.s);
      LOG_TRACE ("%s.format = %s [T 4]\n", obj->name, o->format);
      dxf_free_pair (pair);
      FIELD_BL (num_childs, 90);
    }
  else
    {
      EXPECT_UINT_DXF ("num_childs", 90, BL);
      dxf_free_pair (pair);
    }
  if (o->num_childs)
    {
      o->childs = (BITCODE_H *)xcalloc (o->num_childs, sizeof (BITCODE_H));
      if (!o->childs)
        {
          o->num_childs = 0;
          return pair;
        }
      for (i = 0; i < o->num_childs; i++)
        {
          pair = dxf_read_pair (dat);
          EXPECT_DXF (obj->name, o->childs[i], 360);
          o->childs[i] = dwg_add_handleref (dwg, 3, pair->value.u, obj);
          LOG_TRACE ("%s.childs[%d] = " FORMAT_REF " [H 360]\n", obj->name, i,
                     ARGS_REF (o->childs[i]));
          dxf_free_pair (pair);
        }
    }
  FIELD_BL (num_objects, 97);
  if (o->num_objects)
    {
      o->objects = (BITCODE_H *)xcalloc (o->num_objects, sizeof (BITCODE_H));
      if (!o->objects)
        {
          o->num_objects = 0;
          return pair;
        }
      for (i = 0; i < o->num_objects; i++)
        {
          pair = dxf_read_pair (dat);
          EXPECT_DXF (obj->name, o->objects[i], 331);
          o->objects[i] = dwg_add_handleref (dwg, 5, pair->value.u, obj);
          LOG_TRACE ("%s.objects[%u] = " FORMAT_REF " [H 331]\n", obj->name, i,
                     ARGS_REF (o->objects[i]));
          dxf_free_pair (pair);
        }
    }
  //PRE (R_2007a) {
  //  FIELD_T (format, 4);
  //}
  FIELD_BL (evaluation_option, 91);
  FIELD_BL (filing_option, 92);
  FIELD_BL (field_state, 94);
  FIELD_BL (evaluation_status, 95);
  FIELD_BL (evaluation_error_code, 96);
  FIELD_T (evaluation_error_msg, 300);

  FIELD_BL (num_childval, 93);
  if (o->num_childval)
    {
      o->childval = (Dwg_FIELD_ChildValue *)xcalloc (o->num_childval, sizeof (Dwg_FIELD_ChildValue));
      if (!o->childval)
        {
          o->num_childval = 0;
          return pair;
        }
      for (i = 0; i < o->num_childval; i++)
        {
          pair = dxf_read_pair (dat);
          EXPECT_DXF (obj->name, childval[i].key, 6);
          o->childval[i].key = strdup (pair->value.s);
          dxf_free_pair (pair);
        next_field:
          pair = dxf_read_pair (dat);
          EXPECT_DXF (obj->name, childval[i].value.data_type, 90);
          LOG_TRACE ("FIELD.childval[%u].value.data_type = %u [RL 90]\n", i,
                     pair->value.u);
          o->childval[i].value.data_type = pair->value.u;
          dxf_free_pair (pair);
          /*
            90
            140
            7  ACFD_FIELD_VALUE
            90
            91
          */
          switch (o->childval[i].value.data_type)
            {
            case 0: /* kUnknown */
              SUB_FIELD_BL (childval[i],value.data_long, 91); // string length
              break;
            case 1: /* kLong */
              SUB_FIELD_BL (childval[i],value.data_long, 91);
              break;
            case 2: /* kDouble */
              //SUB_FIELD_BD (childval[i],value.data_double, 140, RD);
              pair = dxf_read_pair (dat);
              EXPECT_DXF (obj->name, childval[i].value.data_double, 140);
              o->childval[i].value.data_double = pair->value.d;
              LOG_TRACE ("FIELD.childval[%u].value.data_double = %f [RD 140]\n", i,
                         pair->value.d);
              dxf_free_pair (pair);
              break;
            case 4:                           /* kString */
              SUB_FIELD_T (childval[i],value.data_string, 1); /* and 2. TODO multiple lines */
              break;
            case 8: /* kDate */
              SUB_FIELD_BL (childval[i],value.data_size, 92);
              //TODO SUB_FIELD_BINARY (childval[i],value.data_date, SUB_FIELD_VALUE (childval[i],value.data_size), 310);
              break;
            case 16: /* kPoint */
              //SUB_FIELD_2BD (childval[i],value.data_point, 11);
              pair = dxf_read_pair (dat);
              EXPECT_DXF (obj->name, childval[i].value.data_point.x, 11);
              o->childval[i].value.data_point.x = pair->value.d;
              dxf_free_pair (pair);

              pair = dxf_read_pair (dat);
              EXPECT_DXF (obj->name, childval[i].value.data_point.y, 21);
              o->childval[i].value.data_point.y = pair->value.d;
              dxf_free_pair (pair);
              LOG_TRACE (
                  "FIELD.childval[%u].value.data_point = (%f, %f) [2RD 11]\n",
                  i, o->childval[i].value.data_point.x,
                  o->childval[i].value.data_point.y);
              break;
            case 32: /* k3dPoint */
              pair = dxf_read_pair (dat);
              EXPECT_DXF (obj->name, childval[i].value.data_3dpoint.x, 11);
              o->childval[i].value.data_3dpoint.x = pair->value.d;
              dxf_free_pair (pair);
              pair = dxf_read_pair (dat);
              EXPECT_DXF (obj->name, childval[i].value.data_3dpoint.y, 21);
              o->childval[i].value.data_3dpoint.y = pair->value.d;
              dxf_free_pair (pair);
              pair = dxf_read_pair (dat);
              EXPECT_DXF (obj->name, childval[i].value.data_3dpoint.z, 31);
              o->childval[i].value.data_3dpoint.z = pair->value.d;
              dxf_free_pair (pair);
              LOG_TRACE (
                  "FIELD.childval[%u].value.data_3dpoint = (%f, %f, %f) [3RD 11]\n",
                  i, o->childval[i].value.data_3dpoint.x,
                  o->childval[i].value.data_3dpoint.y,
                  o->childval[i].value.data_3dpoint.z);
              break;
            case 64: /* kObjectId */
              pair = dxf_read_pair (dat);
              EXPECT_DXF (obj->name, childval[i].value.data_handle, 330);
              o->childval[i].value.data_handle
                  = dwg_add_handleref (dwg, 3, pair->value.u, obj);
              dxf_free_pair (pair);
              LOG_TRACE ("FIELD.childval[%u].value.data_handle = " FORMAT_REF
                         " [H %d]\n",
                         i, ARGS_REF (o->childval[i].value.data_handle), 330);
              // SUB_FIELD_HANDLE (childval[i],value.data_handle, -1, 330);
              break;
            case 128: /* kBuffer */
              LOG_ERROR ("Unknown data type in FIELD: \"kBuffer\".\n")
              break;
            case 256: /* kResBuf */
            case 512: /* kGeneral since r2007*/
            default:
              LOG_ERROR ("Unknown data type in FIELD: \"kResBuf\".\n")
              break;
            // case 512: /* kGeneral since r2007*/
              //SINCE (R_2007a) { SUB_FIELD_BL (childval[i],value.data_size, 0); }
              //else
              //  {
              //    LOG_ERROR (
              //               "Unknown data type in FIELD: \"kGeneral before "
              //               "R_2007\".\n")
              //  }
              break;
            }
        }
      // optional 7 Key (evaluated cache); hard-coded as ACFD_FIELD_VALUE
      pair = dxf_read_pair (dat);
      if (pair->code == 7)
        {
          dxf_free_pair (pair);
          goto next_field; // one more cached field
        }
      else if (i + 1 == o->num_childval) {
        FIELD_T (value_string, 301);
      }
    }
  if (pair->code != 301) {
    FIELD_T (value_string, 301); // TODO: and 9 for subsequent >255 chunks
  }
  FIELD_BL (value_string_length, 98); //ODA bug TV

  return NULL;
}


/* if it has an absolute ownerhandle:
   1. all entities are relative
   2. all early table records are relative
   3. all early objects are absolute (<GROUP),
   4. DICTIONARY, LAYER may be both
 */
static int
is_obj_absowner (Dwg_Object *obj)
{
  if (obj->supertype == DWG_SUPERTYPE_ENTITY)
  if (dwg_obj_is_table (obj))
    return obj->parent->header.version >= R_2004;
  /* With DICTIONARY it may vary, mostly absolute */
  if (obj->type < DWG_TYPE_GROUP) // needs to be absolute 4.1.X
    return 1;
  // DICTIONARY: D-17 rel. >= 19 abs
  if (obj->fixedtype == DWG_TYPE_DICTIONARY || obj->fixedtype == DWG_TYPE_DICTIONARYVAR)
    return obj->handle.value > 0x19;
  if (obj->fixedtype == DWG_TYPE_LAYOUT
      || obj->fixedtype == DWG_TYPE_SCALE
      || obj->fixedtype == DWG_TYPE_SORTENTSTABLE
      || obj->fixedtype == DWG_TYPE_MATERIAL
      // || obj->fixedtype == DWG_TYPE_DICTIONARY
      // || obj->fixedtype == DWG_TYPE_DICTIONARYVAR
      || obj->fixedtype == DWG_TYPE_DICTIONARYWDFLT
      // || obj->fixedtype == DWG_TYPE_XRECORD
      )
    return 1;
  else // may have relative ref: 8.0.0
    return 0;
}

static Dxf_Pair *
new_table_control (const char *restrict name, Bit_Chain *restrict dat,
                   Dwg_Data *restrict dwg)
{
  // VPORT_CONTROL.num_entries
  // VPORT_CONTROL.entries[num_entries] handles
  Dwg_Object *obj;
  Dxf_Pair *pair = NULL;
  Dwg_Object_LTYPE_CONTROL *_obj = NULL; // the largest
  int j = 0;
  int is_tu = dwg->header.version >= R_2007 ? 1 : 0;
  char *fieldname;
  char ctrlname[80];
  char *dxfname;
  BITCODE_B is_xref_ref; // referenceable

  NEW_OBJECT (dwg, obj);

  if (strEQc (name, "BLOCK_RECORD"))
    strcpy (ctrlname, "BLOCK_CONTROL");
  else if (strEQc (name, "VX_TABLE_RECORD"))
    strcpy (ctrlname, "VX_CONTROL");
  else
    {
      const char _control[] = "_CONTROL";
      // -Wstringop-truncation bug:
      // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=88780
      GCC80_DIAG_IGNORE (-Wstringop-truncation)
      strncpy (ctrlname, name, sizeof (ctrlname) - sizeof (_control) - 1);
      ctrlname[sizeof (ctrlname) - sizeof (_control)] = '\0';
      strncat (ctrlname, _control, sizeof (ctrlname) - 1);
      GCC80_DIAG_RESTORE
    }
  LOG_TRACE ("add %s\n", ctrlname);
  dxfname = strdup (ctrlname);

  // clang-format off
  ADD_TABLE_IF1 (LTYPE, LTYPE_CONTROL)
  else
  ADD_TABLE_IF1 (VPORT, VPORT_CONTROL)
  else
  ADD_TABLE_IF1 (APPID, APPID_CONTROL)
  else
  ADD_TABLE_IF1 (BLOCK_RECORD, BLOCK_CONTROL)
  else
  ADD_TABLE_IF1 (DIMSTYLE, DIMSTYLE_CONTROL)
  else
  ADD_TABLE_IF1 (LAYER, LAYER_CONTROL)
  else
  ADD_TABLE_IF1 (STYLE, STYLE_CONTROL)
  else
  ADD_TABLE_IF1 (UCS, UCS_CONTROL)
  else
  ADD_TABLE_IF1 (VIEW, VIEW_CONTROL)
  else
  // ADD_TABLE_IF1 (VX, VX_CONTROL)
  // else
  ADD_TABLE_IF1 (BLOCK_RECORD, BLOCK_CONTROL)
  else
  // clang-format on
  {
    // obj->name = "UNKNOWN_OBJ";
    obj->fixedtype = DWG_TYPE_UNKNOWN_OBJ;
    // undo NEW_OBJECT
    free (dxfname);
    free (obj->tio.object);
    dwg->num_objects--;
    LOG_ERROR ("Unknown DXF TABLE %s nor %s_CONTROL", name, name);
    dxf_free_pair (pair);
    return NULL;
  }
  if (!_obj)
    {
      // obj->name = "UNKNOWN_OBJ";
      obj->fixedtype = DWG_TYPE_UNKNOWN_OBJ;
      // undo NEW_OBJECT
      free (obj->tio.object);
      dwg->num_objects--;
      LOG_ERROR ("Empty _obj at DXF TABLE %s nor %s_CONTROL", name, name);
      dxf_free_pair (pair);
      return NULL;
    }
  obj->tio.object->objid = obj->index;
  is_xref_ref = 1;
  if (dwg_dynapi_entity_field (obj->name, "is_xref_ref"))
    dwg_dynapi_entity_set_value (_obj, obj->name, "is_xref_ref", &is_xref_ref,
                                 1);
  // default is_xdic_missing
  if (dwg->header.version >= R_2004)
    obj->tio.object->is_xdic_missing = 1;

  pair = dxf_read_pair (dat);
  // read common table until next 0 table or endtab
  while (pair != NULL && pair->code != 0)
    {
      switch (pair->code)
        {
        case 0:
          goto do_return;
        case 5:
        case 105: // for DIMSTYLE
          {
            Dwg_Object_Ref *ref;
            char ctrlobj[80];
            const char _object[] = "_OBJECT";
            dwg_add_handle (&obj->handle, 0, pair->value.u, obj);
            ref = dwg_add_handleref (dwg, 3, pair->value.u, obj);
            LOG_TRACE ("%s.handle = " FORMAT_H " [H %d]\n", ctrlname,
                       ARGS_H (obj->handle), pair->code);
            // also set the matching HEADER.*_CONTROL_OBJECT
            // -Wstringop-truncation bug:
            // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=88780
            GCC80_DIAG_IGNORE (-Wstringop-truncation)
            strncpy (ctrlobj, ctrlname,
                     sizeof (ctrlobj) - sizeof (_object) - 1);
            ctrlname[sizeof (ctrlobj) - sizeof (_object)] = '\0';
            strncat (ctrlobj, _object, sizeof (ctrlobj) - 1);
            GCC80_DIAG_RESTORE
            dwg_dynapi_header_set_value (dwg, ctrlobj, &ref, 0);
            LOG_TRACE ("HEADER.%s = " FORMAT_REF " [H 0]\n", ctrlobj,
                       ARGS_REF (ref));
          }
          break;
        case 100: // AcDbSymbolTableRecord, ... ignore
          break;
        case 102: // TODO {ACAD_XDICTIONARY {ACAD_REACTORS {BLKREFS
          break;
        case 330: // TODO: most likely {ACAD_REACTORS
          {
            BITCODE_H owh;
            if (!obj->tio.object->ownerhandle)
              {
                if (is_obj_absowner (obj)) // table record
                  owh = dwg_add_handleref (dwg, 4, pair->value.u, NULL);
                else // relative
                  owh = dwg_add_handleref (dwg, 4, pair->value.u, obj);
                obj->tio.object->ownerhandle = owh;
                LOG_TRACE ("%s.ownerhandle = " FORMAT_REF " [H 330]\n", ctrlname,
                           ARGS_REF (owh));
              }
          }
          break;
        case 340:
          if (pair->value.u && strEQc (ctrlname, "DIMSTYLE_CONTROL"))
            {
              Dwg_Object_DIMSTYLE_CONTROL *o
                  = (Dwg_Object_DIMSTYLE_CONTROL *)_obj;
              if (!o->num_morehandles || j < 0 || j >= o->num_morehandles)
                {
                  LOG_ERROR ("Invalid DIMSTYLE_CONTROL.num_morehandles %d or "
                             "index %d",
                             o->num_morehandles, j)
                  break;
                }
              assert (o->morehandles);
              assert (j >= 0);
              assert (j < o->num_morehandles);
              o->morehandles[j]
                  = dwg_add_handleref (dwg, 5, pair->value.u, NULL);
              LOG_TRACE ("%s.morehandles[%d] = " FORMAT_REF " [H 340]\n",
                         ctrlname, j, ARGS_REF (o->morehandles[j]));
              j++;
            }
          break;
        case 360: // {ACAD_XDICTIONARY TODO
          obj->tio.object->xdicobjhandle
              = dwg_add_handleref (dwg, 3, pair->value.u, obj);
          obj->tio.object->is_xdic_missing = 0;
          LOG_TRACE ("%s.xdicobjhandle = " FORMAT_REF " [H 360]\n", ctrlname,
                     ARGS_REF (obj->tio.object->xdicobjhandle));
          break;
        case 70:
          if (pair->value.u)
            {
              BITCODE_H *hdls;
              // can be -1
              BITCODE_BL num_entries = pair->value.i < 0 ? 0 : pair->value.i;
              if (num_entries > INT32_MAX // BS overflow
                  && obj->fixedtype != DWG_TYPE_BLOCK_CONTROL
                  && obj->fixedtype != DWG_TYPE_LAYER_CONTROL
                  && obj->fixedtype != DWG_TYPE_VIEW_CONTROL
                  && obj->fixedtype != DWG_TYPE_STYLE_CONTROL)
                {
                  LOG_ERROR ("%s.num_entries BS overflow", obj->name);
                  num_entries = 0;
                }
              hdls = (BITCODE_H *)xcalloc (num_entries, sizeof (BITCODE_H));
              if (!hdls)
                num_entries = 0;
              dwg_dynapi_entity_set_value (_obj, obj->name, "num_entries",
                                           &num_entries, 1);
              LOG_TRACE ("%s.num_entries = %u [BL 70]\n", ctrlname,
                         num_entries);
              dwg_dynapi_entity_set_value (_obj, obj->name, "entries", &hdls,
                                           0);
              LOG_TRACE ("Add %d %s.%s\n", num_entries, ctrlname, "entries");
            }
          break;
        case 71:
          if (strEQc (ctrlname, "DIMSTYLE_CONTROL"))
            {
              if (pair->value.u)
                {
                  BITCODE_H *hdls;
                  hdls = (BITCODE_H *)xcalloc (pair->value.u,
                                               sizeof (BITCODE_H));
                  if (!hdls)
                    pair->value.u = 0;
                  dwg_dynapi_entity_set_value (
                      _obj, obj->name, "num_morehandles", &pair->value, is_tu);
                  LOG_TRACE ("%s.num_morehandles = %u [BL 71]\n", ctrlname,
                             pair->value.u);
                  dwg_dynapi_entity_set_value (_obj, obj->name, "morehandles",
                                               &hdls, 0);
                  LOG_TRACE ("Add %s.morehandles[%d]\n", ctrlname,
                             pair->value.u);
                }
              else
                {
                  LOG_TRACE ("%s.num_morehandles = %u [BL 71]\n", ctrlname,
                             pair->value.u)
                }
              break;
            }
          // fall through
        default:
          if (pair->code >= 1000 && pair->code < 1999)
            {
              add_eed (obj, obj->name, pair);
            }
          else
            LOG_ERROR ("Unknown DXF code %d for %s", pair->code, ctrlname);
        }
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
    }
do_return:
  // A minimal DXF will have no handle values, assign them then
  if (!obj->handle.value)
    {
      BITCODE_RLL next_handle = dwg_next_handle (dwg);
      dwg_add_handle (&obj->handle, 0, next_handle, NULL);
      // adds header_vars->CONTROL ref
      (void)dwg_ctrl_table (dwg, name);
      LOG_TRACE ("%s.handle = (0.%d." FORMAT_HV ")\n", obj->name,
                 obj->handle.size, obj->handle.value);
    }
  // default NULL handle
  if (!obj->tio.object->xdicobjhandle)
    {
      obj->tio.object->is_xdic_missing = 1;
      if (dwg->header.version >= R_13b1 && dwg->header.version < R_2004)
        obj->tio.object->xdicobjhandle = dwg_add_handleref (dwg, 3, 0, obj);
    }
  return pair;
}

/* by name or by ref.
   Note that we don't get the ref->obj here, as it may still move
   by realloc dwg->object[].
*/
BITCODE_H
find_tablehandle (Dwg_Data *restrict dwg, Dxf_Pair *restrict pair)
{
  BITCODE_H ref = NULL;
  if (pair->code == 8)
    ref = dwg_find_tablehandle_silent (dwg, pair->value.s, "LAYER");
  else if (pair->code == 1) // $DIMBLK
    ref = dwg_find_tablehandle_silent (dwg, pair->value.s, "BLOCK");
  // some name: $DIMSTYLE, $UCSBASE, $UCSORTHOREF, $CMLSTYLE
  // not enough info, decide later
  else if (pair->code == 2)
    ;
  else if (pair->code == 3)
    ref = dwg_find_tablehandle_silent (dwg, pair->value.s, "DIMSTYLE");
  // what is/was 4 and 5? VIEW? VX?
  else if (pair->code == 6)
    ref = dwg_find_tablehandle_silent (dwg, pair->value.s, "LTYPE");
  else if (pair->code == 7)
    ref = dwg_find_tablehandle_silent (dwg, pair->value.s, "STYLE");
  else if (pair->code == 345 || pair->code == 346)
    ref = dwg_find_tablehandle_silent (dwg, pair->value.s, "UCS");

  if (ref) // turn a 2 (hardowner) into a 5 (softref)
    return dwg_add_handleref (dwg, 5, ref->absolute_ref, NULL);
  /* I think all these >300 are given by hex value, not by name */
  if (!ref && pair->code > 300)
    {
      for (BITCODE_BL i = 0; i < dwg->num_object_refs; i++)
        {
          Dwg_Object_Ref *refi = dwg->object_ref[i];
          if (refi->absolute_ref == (BITCODE_BL)pair->value.u)
            {
              // no relative offset
              ref = dwg_add_handleref (dwg, 5, pair->value.u, NULL);
              break;
            }
        }
      if (!ref)
        {
          // no relative offset
          ref = dwg_add_handleref (dwg, 5, pair->value.u, NULL);
        }
    }
#  if 0
  else if (pair->code == 331)
    ref = dwg_find_tablehandle_silent (dwg, pair->value.s, "VPORT");
  else if (pair->code == 390)
    ref = dwg_find_tablehandle_silent (dwg, pair->value.s, "PLOTSTYLENAME");
  else if (pair->code == 347)
    ref = dwg_find_tablehandle_silent (dwg, pair->value.s, "MATERIAL");
  else if (pair->code == 345 || pair->code == 346)
    ref = dwg_find_tablehandle_silent (dwg, pair->value.s, "UCS");
  else if (pair->code == 361) // SUN
    ref = dwg_find_tablehandle_silent (dwg, pair->value.s, "SHADOW");
  else if (pair->code == 340) // or TABLESTYLE or LAYOUT or MLINESTYLE ...
    ref = dwg_find_tablehandle_silent (dwg, pair->value.s, "STYLE");
  else if (pair->code == 342 || pair->code == 343)
    ref = dwg_find_tablehandle_silent (dwg, pair->value.s, "STYLE");
  else if (pair->code == 348)
    ref = dwg_find_tablehandle_silent (dwg, pair->value.s, "VISUALSTYLE");
  else if (pair->code == 332)
    ref = dwg_find_tablehandle_silent (dwg, pair->value.s, "BACKGROUND");
#  endif
  return ref;
}

// add pair to XRECORD
static Dxf_Pair *
add_xdata (Bit_Chain *restrict dat, Dwg_Object *restrict obj,
           Dxf_Pair *restrict pair)
{
  BITCODE_BL num_xdata, xdata_size;
  // add pairs to xdata linked list
  Dwg_Resbuf *rbuf;
  Dwg_Object_XRECORD *_obj = obj->tio.object->tio.XRECORD;

  num_xdata = _obj->num_xdata;
  xdata_size = _obj->xdata_size;
  rbuf = (Dwg_Resbuf *)calloc (1, sizeof (Dwg_Resbuf));
  if (!rbuf)
    {
      LOG_ERROR ("Out of memory");
      return NULL;
    }
  if (num_xdata && _obj->xdata)
    {
      Dwg_Resbuf *xdata, *prev;
      // add to end, not front
      prev = xdata = _obj->xdata;
      while (xdata)
        {
          prev = xdata;
          xdata = xdata->nextrb;
        }
      prev->nextrb = rbuf;
    }
  else
    _obj->xdata = rbuf;

  xdata_size += 2; // RS
  rbuf->type = pair->code;
  switch (dwg_resbuf_value_type (rbuf->type))
    {
    case DWG_VT_STRING:
      if (!pair->value.s)
        goto invalid;
      PRE (R_2007a) // TODO: nice would be the proper target version.
                    // dat->version
      {
        Dwg_Data *dwg = obj->parent;
        size_t length = strlen (pair->value.s);
        rbuf->value.str.size = length & 0xFFFF;
        rbuf->value.str.codepage = dwg->header.codepage;
        rbuf->value.str.is_tu = 0;
        rbuf->value.str.u.data = strdup (pair->value.s);
        LOG_TRACE ("xdata[%d]: \"%s\" [%d]\n", num_xdata,
                   rbuf->value.str.u.data, rbuf->type);
        xdata_size += 3 + rbuf->value.str.size;
      }
      LATER_VERSIONS
      {
        size_t length = strlen (pair->value.s);
        rbuf->value.str.size = length & 0xFFFF;
        if (length > 0)
          rbuf->value.str.u.wdata = bit_utf8_to_TU (pair->value.s, 0);
        rbuf->value.str.is_tu = 1;
        LOG_TRACE ("xdata[%d]: \"%s\" [TU %d]\n", num_xdata, pair->value.s,
                   rbuf->type);
        xdata_size += 2 + 2 * rbuf->value.str.size;
      }
      break;
    case DWG_VT_REAL:
      rbuf->value.dbl = pair->value.d;
      LOG_TRACE ("xdata[%d]: %f [%d]\n", num_xdata, rbuf->value.dbl,
                 rbuf->type);
      xdata_size += 8;
      break;
    case DWG_VT_BOOL:
    case DWG_VT_INT8:
      rbuf->value.i8 = pair->value.i;
      LOG_TRACE ("xdata[%d]: %d [%d]\n", num_xdata, (int)rbuf->value.i8,
                 rbuf->type);
      xdata_size += 1;
      break;
    case DWG_VT_INT16:
      rbuf->value.i16 = pair->value.i;
      LOG_TRACE ("xdata[%d]: %d [%d]\n", num_xdata, (int)rbuf->value.i16,
                 rbuf->type);
      xdata_size += 2;
      break;
    case DWG_VT_INT32:
      rbuf->value.i32 = pair->value.l;
      LOG_TRACE ("xdata[%d]: %ld [%d]\n", num_xdata, (long)rbuf->value.i32,
                 rbuf->type);
      xdata_size += 4;
      break;
    case DWG_VT_INT64:
      rbuf->value.i64 = (BITCODE_BLL)pair->value.rll;
      LOG_TRACE ("xdata[%d]: " FORMAT_RLL " [%d]\n", num_xdata,
                 rbuf->value.i64, rbuf->type);
      xdata_size += 8;
      break;
    case DWG_VT_POINT3D:
      rbuf->value.pt[0] = pair->value.d;
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
      if (!pair)
        return NULL;
      rbuf->value.pt[1] = pair->value.d;
      dxf_free_pair (pair);
      xdata_size += 24;
      { // if 30
        size_t pos = bit_position (dat);
        pair = dxf_read_pair (dat);
        if (!pair)
          return NULL;
        if (dwg_resbuf_value_type (pair->code) == DWG_VT_POINT3D)
          {
            rbuf->value.pt[2] = pair->value.d;
            LOG_TRACE ("xdata[%d]: (%f,%f,%f) [%d]\n", num_xdata,
                       rbuf->value.pt[0], rbuf->value.pt[1], rbuf->value.pt[2],
                       rbuf->type);
          }
        else
          {
            bit_set_position (dat, pos); // reset stream
            rbuf->value.pt[2] = 0;
            LOG_TRACE ("xdata[%d]: (%f,%f) [%d]\n", num_xdata,
                       rbuf->value.pt[0], rbuf->value.pt[1], rbuf->type);
          }
      }
      break;
    case DWG_VT_BINARY:
      // convert from hex
      if (!pair->value.s)
        goto invalid;
      {
        size_t len = strlen (pair->value.s);
        size_t blen = len / 2;
        size_t read;
        unsigned char *s = (unsigned char *)malloc (blen);
        // const char *pos = pair->value.s;
        rbuf->value.str.u.data = (char *)s;
        rbuf->value.str.size = blen & 0xFFFF;
        if ((read = in_hex2bin (s, pair->value.s, blen) != blen))
          LOG_ERROR ("in_hex2bin read only %" PRIuSIZE " of %" PRIuSIZE, read,
                     blen);
        xdata_size += 1 + (blen & 0xFFFF);
        LOG_TRACE ("xdata[%d]: ", num_xdata);
        // LOG_TRACE_TF (rbuf->value.str.u.data, rbuf->value.str.size);
      }
      break;
    case DWG_VT_HANDLE:
    case DWG_VT_OBJECTID:
      xdata_size += 8;
      rbuf->value.absref = pair->value.rll;
      LOG_TRACE ("xdata[%d]: " FORMAT_HV " [H %d]\n", num_xdata,
                 rbuf->value.absref, rbuf->type);
      break;
    case DWG_VT_INVALID:
    default:
    invalid:
      LOG_ERROR ("Invalid group code in rbuf: %d", rbuf->type)
    }

  num_xdata++;
  _obj->num_xdata = num_xdata;
  _obj->xdata_size = xdata_size;
  return pair;
}

// 350 or 360
static __nonnull_all void
add_dictionary_itemhandles (Dwg_Object *restrict obj, Dxf_Pair *restrict pair,
                            char *restrict text)
{
  // but not DICTIONARYVAR
  Dwg_Object_DICTIONARY *_obj
      = obj->tio.object->tio.DICTIONARY; // also DICTIONARYWDFLT
  Dwg_Data *dwg = obj->parent;
  BITCODE_BL num;
  BITCODE_H hdl;

  if (pair->code == 360)
    _obj->is_hardowner = 1;
  num = _obj->numitems;
  hdl = dwg_add_handleref (dwg, 2, pair->value.u, obj);
  LOG_TRACE ("%s.itemhandles[%d] = " FORMAT_REF " [H* %d]\n", obj->name, num,
             ARGS_REF (hdl), pair->code);
  _obj->itemhandles = (BITCODE_H *)realloc (_obj->itemhandles,
                                            (num + 1) * sizeof (BITCODE_H));
  _obj->texts
      = (BITCODE_TV *)realloc (_obj->texts, (num + 1) * sizeof (BITCODE_TV));
  if (!_obj->itemhandles || !_obj->texts)
    {
      LOG_ERROR ("Out of memory");
      return;
    }
  _obj->itemhandles[num] = hdl;
  _obj->texts[num] = dwg_add_u8_input (dwg, text);
  LOG_TRACE ("%s.texts[%d] = %s [T* 3]\n", obj->name, num, text);
  _obj->numitems = num + 1;
}

/* read to ent->preview, r2010+ code 160, earlier code 92.
   like WIPEOUT, LIGHT, MULTILEADER, ARC_DIMENSION or PROXY GRAPHICS vector
   data.
   Also handle empty preview_size, such as with MULTILEADER r2000.
*/
static Dxf_Pair *
add_ent_preview (Dwg_Object *restrict obj, Bit_Chain *restrict dat,
                 Dxf_Pair *restrict pair)
{
  Dwg_Object_Entity *ent = obj->tio.entity;
  size_t written = 0;

  if (obj->supertype != DWG_SUPERTYPE_ENTITY)
    {
      LOG_ERROR ("%s is no entity for a preview", obj->name);
      return pair;
    }
  ent->preview_size = pair->code == 160  ? pair->value.rll
                      : pair->code == 92 ? pair->value.u
                                         : 0;
  if (!ent->preview_size && pair->code != 310)
    {
      dxf_free_pair (pair);
      return dxf_read_pair (dat);
    }
  if (ent->preview_size >= MAX_SIZE_BUF)
    {
      LOG_ERROR ("Invalid %s.preview_size", obj->name);
      dxf_free_pair (pair);
      return NULL;
    }
  ent->preview
      = (BITCODE_TF)calloc (ent->preview_size ? ent->preview_size : 127, 1);
  if (!ent->preview)
    {
      LOG_ERROR ("Out of memory");
      return NULL;
    }
  LOG_TRACE ("%s.preview_size = " FORMAT_BLL " [BLL %d]\n", obj->name,
             ent->preview_size, pair->code);

  if (pair->code != 310)
    {
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
    }
  while (pair != NULL && pair->code == 310 && pair->value.s)
    {
      size_t read;
      size_t len = strlen (pair->value.s);
      size_t blen = len / 2;
      // const char *pos = pair->value.s;
      BITCODE_TF s;

      if (!ent->preview_size)
        ent->preview = (BITCODE_TF)realloc (ent->preview, written + blen);
      else if (blen + written > ent->preview_size)
        {
          LOG_ERROR ("%s.preview overflow: %" PRIuSIZE " + written %" PRIuSIZE
                     " > size: " FORMAT_BLL,
                     obj->name, blen, written, ent->preview_size);
          return pair;
        }
      s = &ent->preview[written];
      if ((read = in_hex2bin (s, pair->value.s, blen) != blen))
        LOG_ERROR ("in_hex2bin read only %" PRIuSIZE " of %" PRIuSIZE, read,
                   blen);
      written += read;
      LOG_TRACE ("%s.preview += %" PRIuSIZE " (%" PRIuSIZE "/" FORMAT_BLL
                 ")\n",
                 obj->name, blen, written, ent->preview_size);

      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
    }
  if (!ent->preview_size)
    ent->preview_size = written;
  if (ent->preview_size)
    ent->preview_exists = 1;
  return pair;
}

// read to BLOCK_HEADER.preview (310), filling in the size
static Dxf_Pair *
add_block_preview (Dwg_Object *restrict obj, Bit_Chain *restrict dat,
                   Dxf_Pair *restrict pair)
{
  Dwg_Object_BLOCK_HEADER *_obj = obj->tio.object->tio.BLOCK_HEADER;
  size_t written = 0;

  if (obj->type != DWG_TYPE_BLOCK_HEADER)
    {
      LOG_ERROR ("%s is no BLOCK_HEADER for a preview", obj->name);
      return pair;
    }
  if (pair->code != 310)
    {
      LOG_ERROR ("Invalid %s.preview code %d, need 310", obj->name,
                 pair->code);
      return pair;
    }
  while (pair != NULL && pair->code == 310)
    {
      const char *pos = pair->value.s;
      const size_t len = pos ? strlen (pos) : 0;
      const size_t blen = len / 2;
      size_t read;
      BITCODE_TF s;

      if (len)
        {
          _obj->preview = (BITCODE_TF)realloc (_obj->preview, written + blen);
          s = &_obj->preview[written];
          if ((read = in_hex2bin (s, pair->value.s, blen) != blen))
            LOG_ERROR ("in_hex2bin read only %" PRIuSIZE " of %" PRIuSIZE,
                       read, blen);
          written += read;
          LOG_TRACE ("BLOCK_HEADER.preview += %" PRIuSIZE " (%" PRIuSIZE ")\n",
                     blen, written);
        }
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
    }
  _obj->preview_size = written & 0xFFFFFFFF;
  LOG_TRACE ("BLOCK_HEADER.preview_size = %" PRIuSIZE " [BL 0]\n", written);
  return pair;
}

// only need to process conflicting SPLINE DXF codes here. the rest is done via
// dynapi.
// TODO: also for HELIX (where SPLINE is a subclass), pass obj then.
static int
add_SPLINE (Dwg_Entity_SPLINE *restrict o, Bit_Chain *restrict dat,
            Dxf_Pair *restrict pair, int *restrict jp,
            BITCODE_RS *restrict flagp)
{
  int j = *jp;
  if (pair->code == 210 || pair->code == 220 || pair->code == 230)
    return 0; // ignore extrusion in the dwg (planar only)
  else if (pair->code == 70)
    {
      BITCODE_RS flag = *flagp;
      *flagp = flag = pair->value.i;
      o->flag = flag;
      LOG_TRACE ("SPLINE.flag = %d [70]\n", flag);
      if (flag & 1)
        {
          o->closed_b = 1;
          LOG_TRACE ("=> SPLINE.closed_b = 1 [B 0] (bit 0)\n");
        }
      if (flag & 2)
        {
          o->periodic = 1;
          LOG_TRACE ("=> SPLINE.periodic = 1 [B 0] (bit 1)\n");
        }
      if (flag & 4)
        {
          o->rational = 1;
          LOG_TRACE ("=> SPLINE.rational = 1 [B 0] (bit 2)\n");
        }
      if (flag & 16)
        {
          o->weighted = 1;
          LOG_TRACE ("=> SPLINE.weighted = 1 [B 0] (bit 4)\n");
        }
      if (flag & 32)
        o->scenario = 2; // bezier: planar
      else
        o->scenario = 1;
      LOG_TRACE ("=> SPLINE.scenario = %d [BL 0]\n", o->scenario);
      return 1; // found
    }
  else if (pair->code == 71)
    {
      o->degree = pair->value.i;
      LOG_TRACE ("SPLINE.degree = %d [BL 71]\n", o->degree);
      return 1; // found
    }
  else if (pair->code == 72)
    {
      o->num_knots = pair->value.i;
      *jp = 0;
      o->knots = (BITCODE_BD *)xcalloc (o->num_knots, sizeof (BITCODE_BD));
      if (!o->knots)
        {
          o->num_knots = 0;
          return 0;
        }
      LOG_TRACE ("SPLINE.num_knots = %d [BS 72]\n", o->num_knots);
      return 1; // found
    }
  else if (pair->code == 73)
    {
      o->num_ctrl_pts = pair->value.i;
      *jp = 0;
      o->ctrl_pts = (Dwg_SPLINE_control_point *)xcalloc (
          o->num_ctrl_pts, sizeof (Dwg_SPLINE_control_point));
      if (!o->ctrl_pts)
        {
          o->num_ctrl_pts = 0;
          return 0;
        }
      LOG_TRACE ("SPLINE.num_ctrl_pts = %d [BS 73]\n", o->num_ctrl_pts);
      return 1; // found
    }
  else if (pair->code == 74)
    {
      o->num_fit_pts = pair->value.i;
      *jp = 0;
      o->fit_pts
          = (BITCODE_3BD *)xcalloc (o->num_fit_pts, sizeof (BITCODE_3BD));
      if (!o->fit_pts)
        {
          o->num_fit_pts = 0;
          return 0;
        }
      j = 0;
      // o->scenario = 2;
      o->flag |= 1024;
      LOG_TRACE ("SPLINE.num_fit_pts = %d [BS 74]\n", o->num_fit_pts);
      return 1; // found
    }
  else if (pair->code == 40) // knots[] BD*
    {
      if (!o->knots || j >= (int)o->num_knots)
        {
          LOG_ERROR ("SPLINE.knots[%d] overflow, max %d", *jp, o->num_knots);
          return 1; // found
        }
      o->knots[j] = pair->value.d;
      LOG_TRACE ("SPLINE.knots[%d] = %f [BD* 40]\n", *jp, pair->value.d);
      j++;
      *jp = j;
      if (j == (int)o->num_knots)
        *jp = 0;
      return 1; // found
    }
  else if (pair->code == 10) // ctrl_pts[].x 3BD
    {
      if (!o->ctrl_pts || j >= (int)o->num_ctrl_pts)
        {
          LOG_ERROR ("SPLINE.ctrl_pts[%d] overflow, max %d", *jp,
                     o->num_ctrl_pts);
          return 1; // found
        }
      o->ctrl_pts[j].parent = o;
      o->ctrl_pts[j].x = pair->value.d;
      return 1; // found
    }
  else if (pair->code == 20) // ctrl_pts[].y 3BD
    {
      if (!o->ctrl_pts || j >= (int)o->num_ctrl_pts)
        {
          LOG_ERROR ("SPLINE.ctrl_pts[%d] overflow, max %d", j,
                     o->num_ctrl_pts);
          return 1; // found
        }
      o->ctrl_pts[j].y = pair->value.d;
      return 1; // found
    }
  else if (pair->code == 30) // ctrl_pts[].z 3BD
    {
      if (!o->ctrl_pts || j >= (int)o->num_ctrl_pts)
        {
          LOG_ERROR ("SPLINE.ctrl_pts[%d] overflow, max %d", j,
                     o->num_ctrl_pts);
          return 1; // found
        }
      o->ctrl_pts[j].z = pair->value.d;
      LOG_TRACE ("SPLINE.ctrl_pts[%d] = (%f, %f, %f) [3BD* 10]\n", *jp,
                 o->ctrl_pts[j].x, o->ctrl_pts[j].y, o->ctrl_pts[j].z);
      j++;
      *jp = j;
      if (j == (int)o->num_ctrl_pts)
        *jp = 0;
      return 1; // found
    }
  else if (pair->code == 41) // ctrl_pts[].w 3BD
    {
      if (j == 0)
        j = (int)o->num_ctrl_pts;
      j--;
      if (!o->ctrl_pts || j >= (int)o->num_ctrl_pts)
        {
          LOG_ERROR ("SPLINE.ctrl_pts[%d] overflow, max %d", j,
                     o->num_ctrl_pts);
          return 1; // found
        }
      o->ctrl_pts[j].w = pair->value.d;
      LOG_TRACE ("SPLINE.ctrl_pts[%d].w = %f [BD* 41]\n", *jp,
                 o->ctrl_pts[j].w);
      j++;
      *jp = j;
      if (j == (int)o->num_ctrl_pts)
        *jp = 0;
      return 1; // found
    }
  else if (pair->code == 11) // fit_pts[].x 3BD
    {
      if (!o->fit_pts || j >= o->num_fit_pts)
        {
          LOG_ERROR ("SPLINE.fit_pts[%d] overflow, max %d", j, o->num_fit_pts);
          return 1; // found
        }
      o->fit_pts[j].x = pair->value.d;
      return 1; // found
    }
  else if (pair->code == 21) // fit_pts[].y 3BD
    {
      if (!o->fit_pts || j >= o->num_fit_pts)
        {
          LOG_ERROR ("SPLINE.fit_pts[%d] overflow, max %d", j, o->num_fit_pts);
          return 1; // found
        }
      o->fit_pts[j].y = pair->value.d;
      return 1; // found
    }
  else if (pair->code == 31) // fit_pts[].z 3BD
    {
      if (!o->fit_pts || j >= o->num_fit_pts)
        {
          LOG_ERROR ("SPLINE.fit_pts[%d] overflow, max %d", j, o->num_fit_pts);
          return 1; // found
        }
      o->fit_pts[j].z = pair->value.d;
      LOG_TRACE ("SPLINE.fit_pts[%d] = (%f, %f, %f) [3BD* 11]\n", *jp,
                 o->fit_pts[j].x, o->fit_pts[j].y, o->fit_pts[j].z);
      j++;
      *jp = j;
      if (j == (int)o->num_fit_pts)
        *jp = 0;
      return 1; // found
    }
  return 0;
}

static int
add_MLINE (Dwg_Object *restrict obj, Bit_Chain *restrict dat,
           Dxf_Pair *restrict pair, int *restrict jp, int *restrict kp,
           int *restrict lp)
{
  Dwg_Entity_MLINE *o = obj->tio.entity->tio.MLINE;
  int found = 1;
  int j = *jp;
  int k = *kp;
  int l = *lp;

  if (pair->code == 72)
    {
      if (o->num_verts)
        {
          LOG_ERROR ("MLINE.num_verts already set");
          return 2;
        }
      o->num_verts = pair->value.i;
      o->parent = obj->tio.entity;
      o->verts = (Dwg_MLINE_vertex *)xcalloc (o->num_verts,
                                              sizeof (Dwg_MLINE_vertex));
      if (!o->verts)
        {
          o->num_verts = 0;
          return 0;
        }
      if (o->num_lines)
        {
          for (int _j = 0; _j < o->num_verts; _j++)
            {
              o->verts[_j].lines = (Dwg_MLINE_line *)xcalloc (
                  o->num_lines, sizeof (Dwg_MLINE_line));
              if (!o->verts[_j].lines)
                {
                  o->num_lines = 0;
                  return 2;
                }
            }
        }
      LOG_TRACE ("MLINE.num_verts = %d [BS 72]\n", o->num_verts);
      *jp = 0;
    }
  else if (pair->code == 73)
    {
      if (o->num_lines)
        {
          LOG_ERROR ("MLINE.num_lines already set");
          return 2;
        }
      o->num_lines = pair->value.i;
      if (o->num_verts && !o->verts[0].lines)
        {
          for (int _j = 0; _j < o->num_verts; _j++)
            {
              o->verts[_j].lines = (Dwg_MLINE_line *)xcalloc (
                  o->num_lines, sizeof (Dwg_MLINE_line));
              if (!o->verts[_j].lines)
                {
                  o->num_lines = 0;
                  return 2;
                }
            }
        }
      LOG_TRACE ("MLINE.num_lines = %d [BS 73]\n", o->num_lines);
      *kp = 0;
    }
  else if (pair->code == 11 && o->num_verts)
    {

#  define CHK_verts                                                           \
    if (!o->verts || j < 0 || j >= o->num_verts)                              \
      {                                                                       \
        LOG_ERROR ("MLINE.verts[%d] out of bounds", j);                       \
        return 2;                                                             \
      }                                                                       \
    assert (o->verts);                                                        \
    assert (j >= 0);                                                          \
    assert (j < o->num_verts)

      CHK_verts;
      o->verts[j].parent = o;
      o->verts[j].vertex.x = pair->value.d;
    }
  else if (pair->code == 21 && o->num_verts)
    {
      CHK_verts;
      o->verts[j].vertex.y = pair->value.d;
    }
  else if (pair->code == 31 && o->num_verts)
    {
      CHK_verts;
      o->verts[j].vertex.z = pair->value.d;
      LOG_TRACE ("MLINE.verts[%d] = (%f, %f, %f) [3BD* 11]\n", j,
                 o->verts[j].vertex.x, o->verts[j].vertex.y,
                 o->verts[j].vertex.z);
    }
  else if (pair->code == 12 && o->num_verts)
    {
      CHK_verts;
      o->verts[j].vertex_direction.x = pair->value.d;
    }
  else if (pair->code == 22 && o->num_verts)
    {
      CHK_verts;
      o->verts[j].vertex_direction.y = pair->value.d;
    }
  else if (pair->code == 32 && o->num_verts)
    {
      CHK_verts;
      o->verts[j].vertex_direction.z = pair->value.d;
      LOG_TRACE ("MLINE.vertex_direction[%d] = (%f, %f, %f) [3BD* 12]\n", j,
                 o->verts[j].vertex_direction.x,
                 o->verts[j].vertex_direction.y,
                 o->verts[j].vertex_direction.z);
    }
  else if (pair->code == 13 && o->num_verts)
    {
      CHK_verts;
      o->verts[j].miter_direction.x = pair->value.d;
    }
  else if (pair->code == 23 && o->num_verts)
    {
      CHK_verts;
      o->verts[j].miter_direction.y = pair->value.d;
    }
  else if (pair->code == 33 && o->num_verts)
    {
      CHK_verts;
      o->verts[j].miter_direction.z = pair->value.d;
      LOG_TRACE ("MLINE.miter_direction[%d] = (%f, %f, %f) [3BD* 13]\n", j,
                 o->verts[j].miter_direction.x, o->verts[j].miter_direction.y,
                 o->verts[j].miter_direction.z);
      // if (j != o->num_verts - 1)
      //  j++; // not the last
      *kp = 0;
    }
  else if (pair->code == 74 && o->num_lines)
    {
      CHK_verts;

#  define CHK_lines                                                           \
    if (k < 0 || k >= (int)o->num_lines || !o->verts[j].lines)                \
      {                                                                       \
        LOG_ERROR ("MLINE.verts[%d].lines[%d] out of bounds", j, k);          \
        return 2;                                                             \
      }                                                                       \
    assert (o->verts[j].lines);                                               \
    assert (k >= 0 && k < (int)o->num_lines)

      CHK_lines;
      o->verts[j].lines[k].parent = &o->verts[j];
      o->verts[j].lines[k].num_segparms = pair->value.i;
      o->verts[j].lines[k].segparms
          = (BITCODE_BD *)xcalloc (pair->value.i, sizeof (BITCODE_BD));
      if (!o->verts[j].lines[k].segparms)
        {
          o->verts[j].lines[k].num_segparms = 0;
          return 2;
        }
      LOG_TRACE ("MLINE.v[%d].l[%d].num_segparms = %d [BS 74]\n", j, k,
                 pair->value.i);
      *lp = 0;
    }
  else if (pair->code == 41 && o->num_lines)
    {
      CHK_verts;
      CHK_lines;
      if (l < 0 || l >= o->verts[j].lines[k].num_segparms
          || !o->verts[j].lines[k].segparms)
        return 2;
      assert (l >= 0);
      assert (o->verts[j].lines);
      assert (l < o->verts[j].lines[k].num_segparms);

      o->verts[j].lines[k].segparms[l] = pair->value.d;
      LOG_TRACE ("MLINE.v[%d].l[%d].segparms[%d] = %f [BD 41]\n", j, k, l,
                 pair->value.d);
      l++;
      *lp = l;
    }
  else if (pair->code == 75 && o->num_lines)
    {
      CHK_verts;
      CHK_lines;
      o->verts[j].lines[k].num_areafillparms = pair->value.i;
      LOG_TRACE ("MLINE.v[%d].l[%d].num_areafillparms = %d [BS 75]\n", j, k,
                 pair->value.i);
      if (!pair->value.i)
        {
          k++; // next line
          if (k == o->num_lines)
            {
              j++; // next vertex
              *jp = j;
              k = 0;
            }
          *kp = k;
        }
      *lp = 0;
    }
  else if (pair->code == 42 && o->num_lines)
    {
      CHK_verts;
      CHK_lines;
      if (l < 0 || l >= o->verts[j].lines[k].num_areafillparms
          || !o->verts[j].lines[k].areafillparms)
        return 2;
      assert (l >= 0);
      assert (l < o->verts[j].lines[k].num_areafillparms);

      o->verts[j].lines[k].areafillparms[l] = pair->value.d;
      LOG_TRACE ("MLINE.v[%d].l[%d].areafillparms[%d] = %f [BD 42]\n", j, k, l,
                 pair->value.d);
      l++;
      *lp = l;
      if (l == o->verts[j].lines[k].num_areafillparms)
        {
          l = 0;
          *lp = l;
          k++; // next line
          if (k == o->num_lines)
            {
              j++; // next vertex
              *jp = j;
              k = 0;
            }
          *kp = k;
        }
    }
  else
    found = 0;

  return found;
}

#  undef CHK_verts
#  undef CHK_lines

static Dxf_Pair *
add_AcDbEvalExpr (Dwg_Object *restrict obj, char *_obj,
                  Bit_Chain *restrict dat, Dxf_Pair *restrict pair)
{
  Dwg_EvalExpr *ee;
  const char *const evalexpr = "evalexpr";
  Dwg_Data *dwg = obj->parent;
  const Dwg_DYNAPI_field *f1 = dwg_dynapi_entity_field (obj->name, evalexpr);
  if (!f1)
    return pair;
  ee = (Dwg_EvalExpr *)&(_obj)[f1->offset];
  ee->parentid = -1;
  while (pair && pair->code != 100)
    {
      if (pair->code == 90)
        {
          ee->nodeid = pair->value.u;
          LOG_TRACE ("%s.%s.%s = %u [BL %d]\n", obj->name, evalexpr, "nodeid",
                     pair->value.u, pair->code);
        }
      else if (pair->code == 98)
        {
          ee->minor = pair->value.u;
          LOG_TRACE ("%s.%s.%s = %u [BL %d]\n", obj->name, evalexpr, "major",
                     pair->value.u, pair->code);
        }
      else if (pair->code == 99)
        {
          ee->minor = pair->value.u;
          LOG_TRACE ("%s.%s.%s = %u [BL %d]\n", obj->name, evalexpr, "minor",
                     pair->value.u, pair->code);
        }
      else if (pair->code == 70 && !ee->value_code)
        {
          ee->value_code = pair->value.i;
          LOG_TRACE ("%s.%s.%s = %d [BSd %d]\n", obj->name, evalexpr,
                     "value_code", pair->value.i, pair->code);
        }
      else if (pair->code == 40 || pair->code == 140)
        {
          ee->value.num40 = pair->value.d;
          LOG_TRACE ("%s.%s.%s = %f [BD %d]\n", obj->name, evalexpr,
                     "value.num40", pair->value.d, pair->code);
        }
      else if (pair->code == 10)
        {
          ee->value.pt2d.x = pair->value.d;
        }
      else if (pair->code == 20)
        {
          ee->value.pt2d.y = pair->value.d;
          LOG_TRACE ("%s.%s.%s = (%f, %f) [2RD %d]\n", obj->name, evalexpr,
                     "value.pt2d", ee->value.pt2d.x, pair->value.d,
                     pair->code);
        }
      else if (pair->code == 11)
        {
          ee->value.pt3d.x = pair->value.d;
        }
      else if (pair->code == 21)
        {
          ee->value.pt3d.y = pair->value.d;
        }
      else if (pair->code == 31)
        {
          ee->value.pt3d.z = pair->value.d;
          LOG_TRACE ("%s.%s.%s = (%f, %f, %f) [3RD %d]\n", obj->name, evalexpr,
                     "value.pt3d", ee->value.pt3d.x, ee->value.pt3d.y,
                     pair->value.d, pair->code);
        }
      else if (pair->code == 1)
        {
          ee->value.text1 = strdup (pair->value.s);
          LOG_TRACE ("%s.%s.%s = %s [T %d]\n", obj->name, evalexpr,
                     "value.text1", pair->value.s, pair->code);
        }
      else if (pair->code == 70 && ee->value_code)
        {
          ee->value.short70 = pair->value.i;
          LOG_TRACE ("%s.%s.%s = %d [BSd %d]\n", obj->name, evalexpr,
                     "value.short70", pair->value.i, pair->code);
        }
      else if (pair->code == 91)
        {
          ee->value.handle91 = dwg_add_handleref (dwg, 5, pair->value.u, obj);
          LOG_TRACE ("%s.%s.%s = " FORMAT_REF " [H %d]\n", obj->name, evalexpr,
                     "value.handle91", ARGS_REF (ee->value.handle91),
                     pair->code);
        }
      else
        {
          LOG_ERROR ("Invalid DXF code %d for %s", pair->code, "AcDbEvalExpr")
          return pair;
        }
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
    }
  return pair;
}

// starts with T 300
// returns NULL on success
static Dxf_Pair *
add_AcDbBlockElement (Dwg_Object *restrict obj, char *o,
                      Bit_Chain *restrict dat, Dxf_Pair *restrict pair)
{
  Dwg_Data *dwg = obj->parent;
  EXPECT_T_DXF ("name", 300);
  FIELD_BL (be_major, 98);
  FIELD_BL (be_minor, 99);
  FIELD_BL (eed1071, 1071);
  return NULL;
}

// starts with 100
// returns NULL on success
static Dxf_Pair *
add_AcDbBlockAction (Dwg_Object *restrict obj, Bit_Chain *restrict dat)
{
  Dwg_Data *dwg = obj->parent;
  // all with the same offset
  Dwg_Object_BLOCKMOVEACTION *o = obj->tio.object->tio.BLOCKMOVEACTION;
  Dxf_Pair *pair;

  FIELD_BL (num_actions, 70);
  if (o->num_actions)
    {
      // FIELD_VECTOR (actions, BL, num_actions, 91);
      o->actions = (BITCODE_BL *)xcalloc (o->num_actions, sizeof (BITCODE_BL));
      if (!o->actions)
        {
          o->num_actions = 0;
          return pair;
        }
      for (unsigned i = 0; i < o->num_actions; i++)
        {
          pair = dxf_read_pair (dat);
          EXPECT_DXF (obj->name, o->actions[i], 91);
          o->actions[i] = pair->value.u;
          LOG_TRACE ("%s.actions[%d] = %u [BL 91]\n", obj->name, i,
                     o->actions[i]);
          dxf_free_pair (pair);
        }
    }
  FIELD_BL (num_deps, 71);
  // HANDLE_VECTOR (deps, num_deps, 5, 330);
  if (o->num_deps)
    {
      o->deps = (BITCODE_H *)xcalloc (o->num_deps, sizeof (BITCODE_H));
      if (!o->deps)
        {
          o->num_deps = 0;
          return pair;
        }
      for (unsigned i = 0; i < o->num_deps; i++)
        {
          pair = dxf_read_pair (dat);
          EXPECT_DXF (obj->name, o->deps[i], 330);
          o->deps[i] = dwg_add_handleref (dwg, 5, pair->value.u, obj);
          LOG_TRACE ("%s.deps[%d] = " FORMAT_REF " [H 330]\n", obj->name, i,
                     ARGS_REF (o->deps[i]));
          dxf_free_pair (pair);
        }
    }
  FIELD_3BD (display_location, 1010);
  return NULL;
}

// returns NULL on success
static Dxf_Pair *
add_BlockAction_ConnectionPts (Dwg_Object *restrict obj,
                               Bit_Chain *restrict dat, const int first,
                               const int repeat, const int bl_code,
                               const int t_code)
{
  Dwg_Data *dwg = obj->parent;
  Dwg_Object_BLOCKFLIPACTION *o = obj->tio.object->tio.BLOCKFLIPACTION;
  Dxf_Pair *pair;
  Dwg_BLOCKACTION_connectionpts conn_pts[6];
  const char *const field = "conn_pts";
  const Dwg_DYNAPI_field *f = dwg_dynapi_entity_field (obj->name, field);

  if (!f)
    return (Dxf_Pair *)-1;
  if (first)
    dwg_dynapi_field_get_value (o, f, &conn_pts);
  for (int i = first; i < (first + repeat); i++)
    {
      pair = dxf_read_pair (dat);
      EXPECT_DXF (obj->name, "conn_pts[i].code", bl_code + i - first);
      conn_pts[i].code = pair->value.u;
      LOG_TRACE ("%s.conn_pts[%d].code = %u [BL %d]\n", obj->name, i,
                 pair->value.u, bl_code + i - first);
      dxf_free_pair (pair);
    }

  for (int i = first; i < (first + repeat); i++)
    {
      pair = dxf_read_pair (dat);
      EXPECT_DXF (obj->name, "conn_pts[].name", t_code + i - first);
      conn_pts[i].name = strdup (pair->value.s);
      LOG_TRACE ("%s.conn_pts[%d].name = %s [BL %d]\n", obj->name, i,
                 pair->value.s, t_code + i - first);
      dxf_free_pair (pair);
    }
  // memcpy'ing back the content
  dwg_dynapi_field_set_value (dwg, o, f, &conn_pts, 0);
  return NULL;
}

// returns NULL on success
static Dxf_Pair *
add_AcDbBlockActionWithBasePt (Dwg_Object *restrict obj,
                               Bit_Chain *restrict dat)
{
  Dwg_Data *dwg = obj->parent;
  Dwg_Object_BLOCKROTATEACTION *o = obj->tio.object->tio.BLOCKROTATEACTION;
  Dxf_Pair *pair;
  pair = add_BlockAction_ConnectionPts (obj, dat, 0, 2, 92, 301);
  if (pair)
    return pair;
  FIELD_3BD (offset, 1011);
  FIELD_B (dependent, 280);
  FIELD_3BD (base_pt, 1012);
  return NULL;
}

// starts with 100
// returns NULL on success
static Dxf_Pair *
add_AcDbBlockFlipAction (Dwg_Object *restrict obj, Bit_Chain *restrict dat)
{
  Dwg_Data *dwg = obj->parent;
  Dxf_Pair *pair;
  pair = add_BlockAction_ConnectionPts (obj, dat, 0, 4, 92, 301);
  if (pair)
    return pair;
  return NULL;
}

// starts with 100
// returns NULL on success
static Dxf_Pair *
add_AcDbBlockRotationAction (Dwg_Object *restrict obj, Bit_Chain *restrict dat)
{
  Dwg_Data *dwg = obj->parent;
  Dxf_Pair *pair;
  pair = add_BlockAction_ConnectionPts (obj, dat, 2, 1, 94, 303);
  if (pair)
    return pair;
  return NULL;
}

// starts with 100
// returns NULL on success
static Dxf_Pair *
add_AcDbBlockScaleAction (Dwg_Object *restrict obj, Bit_Chain *restrict dat)
{
  Dwg_Data *dwg = obj->parent;
  Dxf_Pair *pair;
  pair = add_BlockAction_ConnectionPts (obj, dat, 2, 3, 94, 303);
  if (pair)
    return pair;
  return NULL;
}

// starts with 100
// returns NULL on success
static Dxf_Pair *
add_AcDbBlockMoveAction (Dwg_Object *restrict obj, Bit_Chain *restrict dat)
{
  Dwg_Data *dwg = obj->parent;
  Dwg_Object_BLOCKMOVEACTION *o = obj->tio.object->tio.BLOCKMOVEACTION;
  Dxf_Pair *pair;

  pair = dxf_read_pair (dat);
  EXPECT_DXF (obj->name, conn_pts[0].code, 92);
  o->conn_pts[0].code = pair->value.u;
  dxf_free_pair (pair);
  pair = dxf_read_pair (dat);
  EXPECT_DXF (obj->name, conn_pts[0].name, 301);
  o->conn_pts[0].name = strdup (pair->value.s);
  LOG_TRACE ("%s.conn_pts[0] = (%u, %s)\n", obj->name, o->conn_pts[0].code,
             o->conn_pts[0].name);
  dxf_free_pair (pair);
  pair = dxf_read_pair (dat);
  EXPECT_DXF (obj->name, conn_pts[0].code, 93);
  o->conn_pts[1].code = pair->value.u;
  dxf_free_pair (pair);
  pair = dxf_read_pair (dat);
  EXPECT_DXF (obj->name, conn_pts[0].name, 302);
  o->conn_pts[1].name = strdup (pair->value.s);
  LOG_TRACE ("%s.conn_pts[1] = (%u, %s)\n", obj->name, o->conn_pts[1].code,
             o->conn_pts[1].name);
  dxf_free_pair (pair);

  // AcDbBlockAction_doubles
  FIELD_BD (action_offset_x, 140);
  FIELD_BD (action_offset_y, 141);
  FIELD_BD (angle_offset, 0);
  /* VALUE_RC (1, 280); Action XY type. 1? */
  pair = dxf_read_pair (dat);
  dxf_free_pair (pair);
  return NULL;
}

// starts with 100
// returns NULL on success
static Dxf_Pair *
add_AcDbBlockStretchAction (Dwg_Object *restrict obj, Bit_Chain *restrict dat)
{
  Dwg_Data *dwg = obj->parent;
  Dwg_Object_BLOCKSTRETCHACTION *o = obj->tio.object->tio.BLOCKSTRETCHACTION;
  Dxf_Pair *pair;

  pair = dxf_read_pair (dat);
  EXPECT_DXF (obj->name, conn_pts[0].code, 92);
  o->conn_pts[0].code = pair->value.u;
  dxf_free_pair (pair);
  pair = dxf_read_pair (dat);
  EXPECT_DXF (obj->name, conn_pts[0].name, 301);
  o->conn_pts[0].name = strdup (pair->value.s);
  LOG_TRACE ("%s.conn_pts[0] = (%u, %s)\n", obj->name, o->conn_pts[0].code,
             o->conn_pts[0].name);
  dxf_free_pair (pair);
  pair = dxf_read_pair (dat);
  EXPECT_DXF (obj->name, conn_pts[0].code, 93);
  o->conn_pts[1].code = pair->value.u;
  dxf_free_pair (pair);
  pair = dxf_read_pair (dat);
  EXPECT_DXF (obj->name, conn_pts[0].name, 302);
  o->conn_pts[1].name = strdup (pair->value.s);
  LOG_TRACE ("%s.conn_pts[1] = (%u, %s)\n", obj->name, o->conn_pts[1].code,
             o->conn_pts[1].name);
  dxf_free_pair (pair);

  FIELD_BL (num_pts, 72);
  // FIELD_2RD_VECTOR (pts, num_pts, 10);
  if (o->num_pts)
    {
      o->pts = (BITCODE_2RD *)xcalloc (o->num_pts, sizeof (BITCODE_2RD));
      if (!o->pts)
        {
          o->num_pts = 0;
          return pair;
        }
      for (unsigned i = 0; i < o->num_pts; i++)
        {
          pair = dxf_read_pair (dat);
          EXPECT_DXF (obj->name, o->pts[i], 1011);
          o->pts[i].x = pair->value.d;
          dxf_free_pair (pair);

          pair = dxf_read_pair (dat);
          EXPECT_DXF (obj->name, o->pts[i], 1021);
          o->pts[i].y = pair->value.d;
          dxf_free_pair (pair);
          LOG_TRACE ("%s.pts[%d] = (%f, %f) [2RD 1011]\n", obj->name, i,
                     o->pts[i].x, o->pts[i].y);
        }
    }
  FIELD_BL (num_hdls, 73);
  if (o->num_hdls)
    {
      o->hdls = (Dwg_BLOCKSTRETCHACTION_handles *)xcalloc (
          o->num_hdls, sizeof (Dwg_BLOCKSTRETCHACTION_handles));
      if (!o->hdls)
        {
          o->num_hdls = 0;
          return pair;
        }
      for (unsigned i = 0; i < o->num_hdls; i++)
        {
          pair = dxf_read_pair (dat);
          EXPECT_DXF (obj->name, o->hdls[i], 331);
          o->hdls[i].hdl = dwg_add_handleref (dwg, 5, pair->value.u, obj);
          LOG_TRACE ("%s.hdls[%d] = " FORMAT_REF " [H 331]\n", obj->name, i,
                     ARGS_REF (o->hdls[i].hdl));
          dxf_free_pair (pair);

          pair = dxf_read_pair (dat);
          EXPECT_DXF (obj->name, o->hdls[i].num_indexes, 74);
          o->hdls[i].num_indexes = pair->value.i;
          LOG_TRACE ("%s.hdls[%d].num_indexes = %u [BS 74]\n", obj->name, i,
                     (unsigned)o->hdls[i].num_indexes);
          dxf_free_pair (pair);

          if (o->hdls[i].num_indexes)
            {
              for (unsigned j = 0; j < o->hdls[i].num_indexes; j++)
                {
                  pair = dxf_read_pair (dat);
                  EXPECT_DXF (obj->name, o->hdls[i].indexes[j], 94);
                  o->hdls[i].indexes[j] = pair->value.u;
                  LOG_TRACE ("%s.hdls[%d].indexes[%d] = %u [BL 94]\n",
                             obj->name, i, j, (unsigned)o->hdls[i].indexes[j]);
                  dxf_free_pair (pair);
                }
            }
        }
    }

  FIELD_BL (num_codes, 75);
  if (o->num_codes)
    {
      o->codes = (Dwg_BLOCKSTRETCHACTION_codes *)xcalloc (
          o->num_codes, sizeof (Dwg_BLOCKSTRETCHACTION_codes));
      if (!o->codes)
        {
          o->num_codes = 0;
          return pair;
        }
      for (unsigned i = 0; i < o->num_codes; i++)
        {
          pair = dxf_read_pair (dat);
          EXPECT_DXF (obj->name, o->codes[i].bl95, 95);
          o->codes[i].bl95 = pair->value.i;
          LOG_TRACE ("%s.codes[%d].bl95 = %d [BL 95]\n", obj->name, i,
                     o->codes[i].bl95);
          dxf_free_pair (pair);

          pair = dxf_read_pair (dat);
          EXPECT_DXF (obj->name, o->codes[i].num_indexes, 76);
          o->codes[i].num_indexes = pair->value.i;
          LOG_TRACE ("%s.codes[%d].num_indexes = %d [BS 76]\n", obj->name, i,
                     o->codes[i].num_indexes);
          dxf_free_pair (pair);

          if (o->codes[i].num_indexes)
            {
              for (unsigned j = 0; j < o->hdls[i].num_indexes; j++)
                {
                  pair = dxf_read_pair (dat);
                  EXPECT_DXF (obj->name, o->codes[i].indexes[j], 94);
                  o->codes[i].indexes[j] = pair->value.i;
                  LOG_TRACE ("%s.codes[%d].indexes[%d] = %d [BL 94]\n",
                             obj->name, i, j, o->codes[i].indexes[j]);
                  dxf_free_pair (pair);
                }
            }
        }
    }

  // AcDbBlockAction_doubles
  FIELD_BD (action_offset_x, 140);
  FIELD_BD (action_offset_y, 141);
  FIELD_BD (angle_offset, 0);
  /* VALUE_RC (1, 280); Action XY type. 1? */
  pair = dxf_read_pair (dat);
  dxf_free_pair (pair);
  return NULL;
}

// starts with BL 91
// returns NULL on success
static Dxf_Pair *
add_AcDbBlockGrip (Dwg_Object *restrict obj, char *o, Bit_Chain *restrict dat,
                   Dxf_Pair *restrict pair)
{
  Dwg_Data *dwg = obj->parent;
  EXPECT_INT_DXF ("bg_bl91", 91, BL);
  FIELD_BL (bg_bl92, 92);
  FIELD_3BD (bg_location, 1010);
  FIELD_B (bg_insert_cycling, 280);
  FIELD_BLd (bg_insert_cycling_weight, 93);
  return NULL;
}

// starts with BL 93
// returns NULL on success
static Dxf_Pair *
add_AcDbBlockVisibilityParameter (Dwg_Object *restrict obj,
                                  Dwg_Object_BLOCKVISIBILITYPARAMETER *o,
                                  Bit_Chain *restrict dat,
                                  Dxf_Pair *restrict pair)
{
  Dwg_Data *dwg = obj->parent;
  EXPECT_INT_DXF ("num_blocks", 93, BL);
  if (o->num_blocks)
    {
      o->blocks = (BITCODE_H *)xcalloc (o->num_blocks, sizeof (BITCODE_H));
      if (!o->blocks)
        {
          o->num_blocks = 0;
          return pair;
        }
      for (unsigned i = 0; i < o->num_blocks; i++)
        {
          pair = dxf_read_pair (dat);
          EXPECT_DXF (obj->name, o->blocks[i], 331);
          o->blocks[i] = dwg_add_handleref (dwg, 4, pair->value.u, NULL);
          LOG_TRACE ("%s.blocks[%d] = " FORMAT_REF " [H 331]\n", obj->name, i,
                     ARGS_REF (o->blocks[i]));
          dxf_free_pair (pair);
        }
    }

  FIELD_BL (num_states, 92);
  if (o->num_states)
    {
      o->states = (Dwg_BLOCKVISIBILITYPARAMETER_state *)xcalloc (
          o->num_states, sizeof (Dwg_BLOCKVISIBILITYPARAMETER_state));
      if (!o->states)
        {
          o->num_states = 0;
          return pair;
        }
      for (unsigned i = 0; i < o->num_states; i++)
        {
          pair = dxf_read_pair (dat);
          EXPECT_DXF (obj->name, o->states[i].name, 303);
          o->states[i].name = strdup (pair->value.s);
          LOG_TRACE ("%s.states[%d].name = %s [T 303]\n", obj->name, i,
                     o->states[i].name);
          dxf_free_pair (pair);

          pair = dxf_read_pair (dat);
          EXPECT_DXF (obj->name, o->states[i].num_blocks, 94);
          o->states[i].num_blocks = pair->value.u;
          LOG_TRACE ("%s.states[%d].num_blocks = %u [BL 94]\n", obj->name, i,
                     o->states[i].num_blocks);
          dxf_free_pair (pair);

          if (o->states[i].num_blocks)
            {
              o->states[i].blocks = (BITCODE_H *)xcalloc (
                  o->states[i].num_blocks, sizeof (BITCODE_H));
              if (!o->states[i].blocks)
                {
                  o->states[i].num_blocks = 0;
                  return pair;
                }
              for (unsigned j = 0; j < o->states[i].num_blocks; j++)
                {
                  pair = dxf_read_pair (dat);
                  EXPECT_DXF (obj->name, o->states[i].blocks[j], 332);
                  o->states[i].blocks[j]
                      = dwg_add_handleref (dwg, 4, pair->value.u, NULL);
                  LOG_TRACE (
                      "%s.states[%d].blocks[%d] = " FORMAT_REF " [H 332]\n",
                      obj->name, i, j, ARGS_REF (o->states[i].blocks[j]));
                  dxf_free_pair (pair);
                }
            }

          pair = dxf_read_pair (dat);
          EXPECT_DXF (obj->name, o->states[i].num_params, 95);
          o->states[i].num_params = pair->value.u;
          LOG_TRACE ("%s.states[%d].num_params = %u [BL 95]\n", obj->name, i,
                     o->states[i].num_params);
          dxf_free_pair (pair);

          if (o->states[i].num_params)
            {
              o->states[i].params = (BITCODE_H *)xcalloc (
                  o->states[i].num_params, sizeof (BITCODE_H));
              if (!o->states[i].params)
                {
                  o->states[i].num_params = 0;
                  return pair;
                }
              for (unsigned j = 0; j < o->states[i].num_params; j++)
                {
                  pair = dxf_read_pair (dat);
                  EXPECT_DXF (obj->name, o->states[i].params[j], 333);
                  o->states[i].params[j]
                      = dwg_add_handleref (dwg, 4, pair->value.u, NULL);
                  LOG_TRACE (
                      "%s.states[%d].params[%d] = " FORMAT_REF " [H 333]\n",
                      obj->name, i, j, ARGS_REF (o->states[i].params[j]));
                  dxf_free_pair (pair);
                }
            }
        }
    }
  return NULL;
}

// starts empty
// returns NULL on success
static Dxf_Pair *
add_BlockParam_PropInfo (Dwg_Object *restrict obj, Bit_Chain *restrict dat,
                         Dwg_BLOCKPARAMETER_PropInfo *prop, const int i,
                         const int num_code, const int d_code,
                         const int t_code)
{
  Dwg_Data *dwg = obj->parent;
  Dxf_Pair *pair;

  pair = dxf_read_pair (dat);
  EXPECT_DXF (obj->name, prop->num_connections, num_code);
  prop->num_connections = pair->value.u;
  LOG_TRACE ("%s.prop%d.num_connections = %u [BL %d]\n", obj->name, i,
             pair->value.u, num_code);
  dxf_free_pair (pair);
  if (prop->num_connections)
    {
      prop->connections = (Dwg_BLOCKPARAMETER_connection *)xcalloc (
          prop->num_connections, sizeof (Dwg_BLOCKPARAMETER_connection));
      if (!prop->connections)
        {
          prop->num_connections = 0;
          return pair;
        }
      for (unsigned j = 0; j < prop->num_connections; j++)
        {
          pair = dxf_read_pair (dat);
          EXPECT_DXF (obj->name, prop->connections[j].code, d_code);
          prop->connections[j].code = pair->value.u;
          LOG_TRACE ("%s.prop[%d].connections[%u].code = %u [BL %d]\n",
                     obj->name, i, j, pair->value.u, d_code);
          dxf_free_pair (pair);

          pair = dxf_read_pair (dat);
          EXPECT_DXF (obj->name, prop->connections[j].name, t_code);
          prop->connections[j].name = strdup (pair->value.s);
          LOG_TRACE ("%s.prop[%d].connections[%u].name = %s [T %d]\n",
                     obj->name, i, j, pair->value.s, t_code);
          dxf_free_pair (pair);
        }
    }
  return NULL;
}

// starts empty (on 100 . subclass)
// returns NULL on success
static Dxf_Pair *
add_AcDbBlock1PtParameter (Dwg_Object *restrict obj, Bit_Chain *restrict dat)
{
  Dwg_Object_BLOCKBASEPOINTPARAMETER *o
      = obj->tio.object->tio.BLOCKBASEPOINTPARAMETER;
  Dwg_Data *dwg = obj->parent;
  Dxf_Pair *pair;

  pair = dxf_read_pair (dat);
  EXPECT_DXF (obj->name, "def_pt.x", 1010);
  o->def_pt.x = pair->value.d;
  dxf_free_pair (pair);
  pair = dxf_read_pair (dat);
  EXPECT_DXF (obj->name, "def_pt.y", 1020);
  o->def_pt.y = pair->value.d;
  dxf_free_pair (pair);
  pair = dxf_read_pair (dat);
  EXPECT_DXF (obj->name, "def_pt.z", 1030);
  o->def_pt.z = pair->value.d;
  LOG_TRACE ("%s.def_pt = (%f, %f, %f) [3BD 1010]\n", obj->name, o->def_pt.x,
             o->def_pt.y, pair->value.d);
  dxf_free_pair (pair);

  pair = dxf_read_pair (dat);
  EXPECT_DXF (obj->name, "num_propinfos", 93); // 2
  o->num_propinfos = pair->value.u;
  LOG_TRACE ("%s.num_propinfos = %u [BL 93]\n", obj->name, pair->value.u);
  dxf_free_pair (pair);

  pair = add_BlockParam_PropInfo (obj, dat, &o->prop1, 1, 170, 91, 301);
  if (pair)
    return pair;
  pair = add_BlockParam_PropInfo (obj, dat, &o->prop2, 2, 171, 92, 302);
  if (pair)
    return pair;

  return NULL;
}

// starts empty (on 100 . subclass)
// returns NULL on success
static Dxf_Pair *
add_AcDbBlock2PtParameter (Dwg_Object *restrict obj, Bit_Chain *restrict dat)
{
  Dwg_Object_BLOCKALIGNMENTPARAMETER *o
      = obj->tio.object->tio.BLOCKALIGNMENTPARAMETER;
  Dwg_Data *dwg = obj->parent;
  Dxf_Pair *pair;

  pair = dxf_read_pair (dat);
  EXPECT_DXF (obj->name, "def_basept.x", 1010);
  o->def_basept.x = pair->value.d;
  dxf_free_pair (pair);
  pair = dxf_read_pair (dat);
  EXPECT_DXF (obj->name, "def_basept.y", 1020);
  o->def_basept.y = pair->value.d;
  dxf_free_pair (pair);
  pair = dxf_read_pair (dat);
  EXPECT_DXF (obj->name, "def_basept.z", 1030);
  o->def_basept.z = pair->value.d;
  LOG_TRACE ("%s.def_basept = (%f, %f, %f) [3BD 1010]\n", obj->name,
             o->def_basept.x, o->def_basept.y, pair->value.d);
  dxf_free_pair (pair);

  pair = dxf_read_pair (dat);
  EXPECT_DXF (obj->name, "def_endpt.x", 1011);
  o->def_endpt.x = pair->value.d;
  dxf_free_pair (pair);
  pair = dxf_read_pair (dat);
  EXPECT_DXF (obj->name, "def_endpt.y", 1021);
  o->def_endpt.y = pair->value.d;
  dxf_free_pair (pair);
  pair = dxf_read_pair (dat);
  EXPECT_DXF (obj->name, "def_endpt.z", 1031);
  o->def_endpt.z = pair->value.d;
  LOG_TRACE ("%s.def_endpt = (%f, %f, %f) [3BD 1011]\n", obj->name,
             o->def_endpt.x, o->def_endpt.y, pair->value.d);
  dxf_free_pair (pair);

  pair = dxf_read_pair (dat);
  EXPECT_DXF (obj->name, "num_prop_states", 170);
  LOG_TRACE ("%s.num_prop_states = %d [BL 170]\n", obj->name, pair->value.u);
  dxf_free_pair (pair);
  o->prop_states = (BITCODE_BL *)xcalloc (4, sizeof (BITCODE_BL));
  if (!o->prop_states)
    return dxf_read_pair (dat);
  for (unsigned i = 0; i < 4; i++)
    {
      pair = dxf_read_pair (dat);
      EXPECT_DXF (obj->name, prop_states[i], 91);
      o->prop_states[i] = pair->value.u;
      LOG_TRACE ("%s.prop_states[%d] = %u [BL 91]\n", obj->name, i,
                 pair->value.u);
      dxf_free_pair (pair);
    }

  pair = add_BlockParam_PropInfo (obj, dat, &o->prop1, 1, 171, 92, 301);
  if (pair)
    return pair;
  pair = add_BlockParam_PropInfo (obj, dat, &o->prop2, 2, 172, 93, 302);
  if (pair)
    return pair;
  pair = add_BlockParam_PropInfo (obj, dat, &o->prop3, 3, 173, 94, 303);
  if (pair)
    return pair;
  pair = add_BlockParam_PropInfo (obj, dat, &o->prop4, 4, 174, 95, 304);
  if (pair)
    return pair;

  FIELD_BS (parameter_base_location, 177);
  return NULL;
}

typedef int t_codes[4];

// varying dxf codes per object
// returns NULL on success
static Dxf_Pair *
add_AcDbBlockParamValueSet (Dwg_Object *restrict obj,
                            Dwg_BLOCKPARAMVALUESET *o, Bit_Chain *restrict dat,
                            Dxf_Pair *restrict pair)
{
  Dwg_Data *dwg = obj->parent;
  // i_code, d_code, s_code, t_code
  const t_codes codes[] = {
    { 96, 128, 175,
      307 }, // 0
             // AcDbBlock{Linear,Diametric,Radial,Angular}ConstraintParameter{,Entity}
    { 96, 141, 175, 307 }, // 1 BLOCKLINEARPARAMETER.value_set
    { 96, 141, 175, 307 }, // 2 BLOCKROTATIONPARAMETER.angle_value_set
    { 97, 146, 176, 309 }, // 3 BLOCKXYPARAMETER.y_value_set
    { 96, 142, 175, 410 }, // 4 BLOCKXYPARAMETER.x_value_set
    { 96, 142, 175, 410 }, // 5 BLOCKPOLARPARAMETER.angle_value_set
    { 97, 146, 176, 309 }, // 6 BLOCKPOLARPARAMETER.distance_value_set
  };
  const t_codes *code
      = &codes
            [0]; // AcDbBlock{Linear,Diametric,Radial,Angular}ConstraintParameter{,Entity}

  // T at first
  if (pair->code == 307)
    {
      if (strEQc (obj->name, "BLOCKLINEARPARAMETER"))
        code = &codes[1];
      else if (strEQc (obj->name, "BLOCKROTATIONPARAMETER"))
        code = &codes[2];
    }
  else if (pair->code == 309)
    {
      if (strEQc (obj->name, "BLOCKXYPARAMETER"))
        code = &codes[3];
      else if (strEQc (obj->name, "BLOCKPOLARPARAMETER"))
        code = &codes[6];
    }
  else if (pair->code == 410)
    {
      if (strEQc (obj->name, "BLOCKXYPARAMETER"))
        code = &codes[4];
      else if (strEQc (obj->name, "BLOCKPOLARPARAMETER"))
        code = &codes[5];
    }
  // subclass not object
  EXPECT_DXF ("BlockParamValueSet", "desc", (*code)[3]); // t_code
  o->desc = strdup (pair->value.s);
  LOG_TRACE ("%s.value_set.desc = \"%s\"\n", obj->name, pair->value.s);
  dxf_free_pair (pair);

  pair = dxf_read_pair (dat);
  EXPECT_DXF ("BlockParamValueSet", "flags", (*code)[0]); // i_code
  o->flags = pair->value.i;
  LOG_TRACE ("%s.value_set.flags = %d\n", obj->name, pair->value.i);
  dxf_free_pair (pair);

  pair = dxf_read_pair (dat);
  EXPECT_DXF ("BlockParamValueSet", "minimum", (*code)[1]); // d_code
  o->minimum = pair->value.d;
  LOG_TRACE ("%s.value_set.minimum = %f\n", obj->name, pair->value.d);
  dxf_free_pair (pair);

  pair = dxf_read_pair (dat);
  EXPECT_DXF ("BlockParamValueSet", "maximum", (*code)[1] + 1);
  o->maximum = pair->value.d;
  LOG_TRACE ("%s.value_set.maximum = %f\n", obj->name, pair->value.d);
  dxf_free_pair (pair);

  pair = dxf_read_pair (dat);
  EXPECT_DXF ("BlockParamValueSet", "increment", (*code)[1] + 2);
  o->increment = pair->value.d;
  LOG_TRACE ("%s.value_set.increment = %f\n", obj->name, pair->value.d);
  dxf_free_pair (pair);

  pair = dxf_read_pair (dat);
  EXPECT_DXF ("BlockParamValueSet", "num_valuelist", (*code)[2]); // s_code
  o->num_valuelist = pair->value.i;
  LOG_TRACE ("%s.value_set.num_valuelist = %d\n", obj->name, pair->value.i);
  dxf_free_pair (pair);

  // FIELD_VECTOR (valuelist, num_valuelist, BD, code[1]+3);
  if (!o->num_valuelist)
    return NULL;
  o->valuelist = (double *)xcalloc (o->num_valuelist, sizeof (double));
  if (!o->valuelist)
    {
      o->num_valuelist = 0;
      return dxf_read_pair (dat);
    }

  for (unsigned i = 0; i < o->num_valuelist; i++)
    {
      pair = dxf_read_pair (dat);
      EXPECT_DXF ("BLOCKPARAMVALUESET", valuelist[i], (*code)[1] + 3);
      o->valuelist[i] = pair->value.d;
      LOG_TRACE ("%s.value_set.valuelist[%d] = %f [BD %d]\n", obj->name, i,
                 pair->value.d, pair->code);
      dxf_free_pair (pair);
    }
  return NULL;
}

// starts with 100
// returns NULL on success
static Dxf_Pair *
add_AcDbBlockRotationParameter (Dwg_Object *restrict obj,
                                Bit_Chain *restrict dat)
{
  Dwg_Data *dwg = obj->parent;
  Dwg_Object_BLOCKROTATIONPARAMETER *o
      = obj->tio.object->tio.BLOCKROTATIONPARAMETER;
  Dwg_BLOCKPARAMVALUESET *value_set = &o->angle_value_set;
  Dxf_Pair *pair;

  FIELD_T (angle_name, 305);
  FIELD_T (angle_desc, 306);
  FIELD_3BD (def_base_angle_pt, 1011);
  FIELD_BD (angle, 140);
  pair = dxf_read_pair (dat);
  EXPECT_DXF (obj->name, angle_value_set, 307);
  pair = add_AcDbBlockParamValueSet (obj, value_set, dat, pair);
  if (pair)
    return pair;
  return NULL;
}

#  define FIELD_CMC(field, dxf) dxf_read_CMC (dwg, dat, &o->field, #field, dxf)
#  define FIELD_CMC2004(field, dxf)                                           \
    SINCE (R_2004a)                                                           \
    {                                                                         \
      FIELD_CMC (field, dxf);                                                 \
    }

// starts with 71 . 0
// returns NULL on success
static Dxf_Pair *
add_AcDbSectionViewStyle (Dwg_Object *restrict obj, Bit_Chain *restrict dat)
{
  Dwg_Data *dwg = obj->parent;
  Dwg_Object_SECTIONVIEWSTYLE *o = obj->tio.object->tio.SECTIONVIEWSTYLE;
  BITCODE_BD *av;
  // starting with 71 . 0 (skipped)
  Dxf_Pair *pair;
  FIELD_BL (flags, 90);
  pair = dxf_read_pair (dat); // skip 71 . 1
  dxf_free_pair (pair);
  FIELD_HANDLE (identifier_style, 5, 340); // textstyle
  FIELD_CMC2004 (identifier_color, 62);
  FIELD_BD (identifier_height, 40); // 5.0
  FIELD_HANDLE (arrow_start_symbol, 5, 340);
  FIELD_HANDLE (arrow_end_symbol, 5, 340);
  FIELD_CMC2004 (arrow_symbol_color, 62);
  FIELD_BD (arrow_symbol_size, 40);
  FIELD_T (identifier_exclude_characters, 300); // I, O, Q, S, X, Z
  // 40 90 40 90 71
  FIELD_BD (arrow_symbol_extension_length, 40);
  FIELD_BLd (identifier_position, 90);
  FIELD_BD (identifier_offset, 40);
  FIELD_BLd (arrow_position, 90);
  pair = dxf_read_pair (dat); // skip 71 . 2
  dxf_free_pair (pair);
  FIELD_HANDLE (plane_ltype, 5, 340); // ltype
  FIELD_BLd (plane_linewt, 90);
  FIELD_CMC2004 (plane_line_color, 62);
  FIELD_HANDLE (bend_ltype, 5, 340); // ltype
  FIELD_BLd (bend_linewt, 90);
  FIELD_CMC2004 (bend_line_color, 62);
  FIELD_BD (bend_line_length, 40);
  FIELD_BD (end_line_overshoot, 40);
  FIELD_BD (end_line_length, 40);
  pair = dxf_read_pair (dat); // skip 71 . 3
  dxf_free_pair (pair);
  FIELD_HANDLE (viewlabel_text_style, 5, 340); // textstyle
  FIELD_CMC2004 (viewlabel_text_color, 62);
  FIELD_BD (viewlabel_text_height, 40);
  FIELD_BL (viewlabel_attachment, 90);
  FIELD_BD (viewlabel_offset, 40); // 5.0
  FIELD_BL (viewlabel_alignment, 90);
  FIELD_T (viewlabel_pattern, 300);
  pair = dxf_read_pair (dat); // skip 71 . 4
  dxf_free_pair (pair);
  FIELD_CMC2004 (hatch_color, 62);
  FIELD_CMC2004 (hatch_bg_color, 62);
  FIELD_T (hatch_pattern, 300);
  FIELD_BD (hatch_scale, 40);
  FIELD_BLd (hatch_transparency, 90);
  FIELD_B (unknown_b1, 290);
  FIELD_B (unknown_b2, 290);
  FIELD_BL (num_hatch_angles, 90);
  if (o->num_hatch_angles)
    {
      o->hatch_angles = (BITCODE_BD *)xcalloc (o->num_hatch_angles, 8);
      if (!o->hatch_angles)
        {
          o->num_hatch_angles = 0;
          return dxf_read_pair (dat);
        }
      for (unsigned i = 0; i < o->num_hatch_angles; i++)
        {
          // FIELD_BD (hatch_angles[i], 40);
          pair = dxf_read_pair (dat);
          EXPECT_DXF (obj->name, hatch_angles, 40);
          o->hatch_angles[i] = pair->value.d;
          LOG_TRACE ("%s.%s[%d] = %f [BD %d]\n", obj->name, "hatch_angles", i,
                     pair->value.d, pair->code);
          dxf_free_pair (pair);
        }
    }
  return NULL;
}

static Dxf_Pair *
add_AcDbDetailViewStyle (Dwg_Object *restrict obj, Bit_Chain *restrict dat)
{
  Dwg_Data *dwg = obj->parent;
  Dwg_Object_DETAILVIEWSTYLE *o = obj->tio.object->tio.DETAILVIEWSTYLE;
  // starting with 71 . 0
  Dxf_Pair *pair;
  FIELD_BL (flags, 90);
  pair = dxf_read_pair (dat); // skip 71 . 1
  dxf_free_pair (pair);
  FIELD_HANDLE (identifier_style, 5, 340); // textstyle
  FIELD_CMC2004 (identifier_color, 62);
  FIELD_BD (identifier_height, 40); // 5.0
  FIELD_HANDLE (arrow_symbol, 5, 340);
  FIELD_CMC2004 (arrow_symbol_color, 62);
  FIELD_BD (arrow_symbol_size, 40);
  FIELD_T (identifier_exclude_characters, 300);
  FIELD_BD (identifier_offset, 40);
  FIELD_RC (identifier_placement, 280);
  pair = dxf_read_pair (dat); // skip 71 . 2
  dxf_free_pair (pair);
  FIELD_HANDLE (boundary_ltype, 5, 340); // ltype
  FIELD_BLd (boundary_linewt, 90);
  FIELD_CMC2004 (boundary_line_color, 62);
  pair = dxf_read_pair (dat); // skip 71 . 3
  dxf_free_pair (pair);
  FIELD_HANDLE (viewlabel_text_style, 5, 340); // textstyle
  FIELD_CMC2004 (viewlabel_text_color, 62);
  FIELD_BD (viewlabel_text_height, 40);
  FIELD_BL (viewlabel_attachment, 90);
  FIELD_BD (viewlabel_offset, 40); // 5.0
  FIELD_BL (viewlabel_alignment, 90);
  FIELD_T (viewlabel_pattern, 300);
  pair = dxf_read_pair (dat); // skip 71 . 4
  dxf_free_pair (pair);
  FIELD_HANDLE (connection_ltype, 5, 340); // ltype
  FIELD_BLd (connection_linewt, 90);
  FIELD_CMC2004 (connection_line_color, 62);
  FIELD_HANDLE (borderline_ltype, 5, 340);
  FIELD_BLd (borderline_linewt, 90);
  FIELD_CMC2004 (borderline_color, 62);
  FIELD_RC (model_edge, 280); // type, origin, direction
  return NULL;
}

static Dxf_Pair *
add_VISUALSTYLE_props (Dwg_Object *restrict obj, Bit_Chain *restrict dat)
{
  Dwg_Data *dwg = obj->parent;
  Dwg_Object_VISUALSTYLE *o = obj->tio.object->tio.VISUALSTYLE;
  // starting with 70 . 58 num_props
  Dxf_Pair *pair;
  FIELD_BS (num_props, 70); // 58
  if (o->num_props != 58)
    LOG_ERROR ("Invalid VISUALSTYLE.num_props %u != 58",
               (unsigned)o->num_props);
  FIELD_B (b_prop1c, 290);
  FIELD_BS (b_prop1c_int, 176);
  FIELD_B (b_prop1d, 290);
  FIELD_BS (b_prop1d_int, 176);
  FIELD_B (b_prop1e, 290);
  FIELD_BS (b_prop1e_int, 176);
  FIELD_B (b_prop1f, 90);
  FIELD_BS (b_prop1f_int, 176);
  FIELD_B (b_prop20, 290);
  FIELD_BS (b_prop20_int, 176);
  FIELD_B (b_prop21, 90);
  FIELD_BS (b_prop21_int, 176);
  FIELD_B (b_prop22, 290);
  FIELD_BS (b_prop22_int, 176);
  FIELD_B (b_prop23, 290);
  FIELD_BS (b_prop23_int, 176);
  FIELD_B (b_prop24, 290);
  FIELD_BS (b_prop24_int, 176);
  FIELD_BL (bl_prop25, 90);
  FIELD_BS (bl_prop25_int, 176);
  FIELD_BD (bd_prop26, 40);
  FIELD_BS (bd_prop26_int, 176);
  FIELD_BD (bd_prop27, 40);
  FIELD_BS (bd_prop27_int, 176);
  FIELD_BL (bl_prop28, 90);
  FIELD_BS (bl_prop28_int, 176);
  FIELD_CMC (c_prop29, 62);
  FIELD_BS (c_prop29_int, 176);
  FIELD_BL (bl_prop2a, 90);
  FIELD_BS (bl_prop2a_int, 176);
  FIELD_BL (bl_prop2b, 90);
  FIELD_BS (bl_prop2b_int, 176);
  FIELD_CMC (c_prop2c, 62);
  FIELD_BS (c_prop2c_int, 176);
  /*FIELD_B (b_prop2d, 0);*/ FIELD_BS (b_prop2d_int, 176);
  FIELD_BL (bl_prop2e, 290);
  FIELD_BS (bl_prop2e_int, 176);
  FIELD_BL (bl_prop2f, 90);
  FIELD_BS (bl_prop2f_int, 176);
  FIELD_BL (bl_prop30, 90);
  FIELD_BS (bl_prop30_int, 176);
  FIELD_B (b_prop31, 290);
  FIELD_BS (b_prop31_int, 176);
  FIELD_BL (bl_prop32, 90);
  FIELD_BS (bl_prop32_int, 176);
  FIELD_CMC (c_prop33, 62);
  FIELD_BS (c_prop33_int, 176);
  FIELD_BD (bd_prop34, 40);
  FIELD_BS (bd_prop34_int, 176);
  FIELD_BL (edge_wiggle, 90);
  FIELD_BS (edge_wiggle_int, 176); // prop 0x35
  FIELD_T (strokes, 1);
  FIELD_BS (strokes_int, 176); // prop 0x36
  FIELD_B (b_prop37, 290);
  FIELD_BS (b_prop37_int, 176);
  FIELD_BD (bd_prop38, 40);
  FIELD_BS (bd_prop38_int, 176);
  FIELD_BD (bd_prop39, 40);
  FIELD_BS (bd_prop39_int, 176);
  return NULL;
}

#endif // DISABLE_DXF

// needed for encode

#ifndef DISABLE_DXF

// see GH #138. add vertices / attribs
static void
dxf_postprocess_SEQEND (Dwg_Object *restrict obj)
{
  Dwg_Data *dwg = obj->parent;
  Dwg_Entity_SEQEND *o = obj->tio.entity->tio.SEQEND;
  Dwg_Object *owner = dwg_ref_object (dwg, obj->tio.entity->ownerhandle);
  Dwg_Entity_POLYLINE_2D *ow;
  BITCODE_BL i, j, num_owned = 0;
  BITCODE_H seqend;
  BITCODE_H *owned = NULL;
  const char *owhdls;

  loglevel = dwg->opts & DWG_OPTS_LOGLEVEL;
  LOG_TRACE ("dxf_postprocess_SEQEND:\n");
  // r12 and earlier: search for owner backwards
  if (dwg->header.from_version < R_13b1 && !owner
      && !obj->tio.entity->ownerhandle)
    {
      for (i = obj->index - 1; i > 0; i--)
        {
          Dwg_Object *_o = &dwg->object[i];
          if (_o->type == DWG_TYPE_INSERT || _o->type == DWG_TYPE_MINSERT
              || _o->type == DWG_TYPE_POLYLINE_2D
              || _o->type == DWG_TYPE_POLYLINE_3D
              || _o->type == DWG_TYPE_POLYLINE_PFACE
              || _o->type == DWG_TYPE_POLYLINE_MESH)
            {
              owner = _o;
              obj->tio.entity->ownerhandle
                  = dwg_add_handleref (dwg, 4, _o->handle.value, obj);
              LOG_TRACE ("SEQEND.owner = " FORMAT_H " (%s) [H* 0]\n",
                         ARGS_H (_o->handle), _o->name);
              break;
            }
        }
    }
  if (!owner)
    {
      if (obj->tio.entity->ownerhandle)
        LOG_WARN ("Missing owner (" FORMAT_HV ") from " FORMAT_REF
                  " [H 330]",
                  obj->handle.value, ARGS_REF (obj->tio.entity->ownerhandle))
      else
        LOG_WARN ("Missing owner (" FORMAT_HV ")", obj->handle.value)
      return;
    }
  obj->tio.entity->ownerhandle->obj = NULL;
  owhdls = memBEGINc (owner->name, "POLYLINE_") ? "vertex" : "attribs";
  ow = owner->tio.entity->tio.POLYLINE_2D;

  seqend = dwg_add_handleref (dwg, 3, obj->handle.value, owner);
  dwg_dynapi_entity_set_value (ow, owner->name, "seqend", &seqend, 0);
  LOG_TRACE ("%s.seqend = " FORMAT_REF " [H 0]\n", owner->name,
             ARGS_REF (seqend));
  // num_owned is not properly stored in a DXF
  // collect children hdls. all objects from owner to here
  for (j = 0, i = owner->index + 1; i < obj->index; i++, j++)
    {
      Dwg_Object *_o = &dwg->object[i];
      num_owned = j + 1;
      if (dwg->header.from_version >= R_13b1)
        {
          owned = (BITCODE_H *)realloc (owned, num_owned * sizeof (BITCODE_H));
          owned[j] = dwg_add_handleref (dwg, 3, _o->handle.value, owner);
          LOG_TRACE ("%s.%s[%d] = " FORMAT_REF " [H* 0]\n", owner->name,
                     owhdls, j, ARGS_REF (owned[j]));
        }
    }
  if (!num_owned)
    return;
  dwg_dynapi_entity_set_value (ow, owner->name, "num_owned", &num_owned, 0);
  LOG_TRACE ("%s.num_owned = " FORMAT_BL " [BL 0]\n", owner->name, num_owned);

  dwg_dynapi_entity_set_value (ow, owner->name, owhdls, &owned, 0);
  in_postprocess_SEQEND (obj, num_owned, owned);
}

static void
dxf_postprocess_LAYOUT (Dwg_Object *restrict obj)
{
  Dwg_Data *dwg = obj->parent;
  Dwg_Object_Object *_o = obj->tio.object;
  Dwg_Object_LAYOUT *_obj = obj->tio.object->tio.LAYOUT;

  // fix LAYOUT 330 confusion. block_header vs ownerhandle
  if (_o->ownerhandle && _o->num_reactors &&
      _o->reactors[0]->absolute_ref != _o->ownerhandle->absolute_ref)
    {
      // must not be a BLOCK_HEADER, but a DICTIONARY
      _obj->block_header = _o->ownerhandle;
      LOG_TRACE ("LAYOUT.block_header = " FORMAT_REF " (fixup)\n",
                 ARGS_REF (_obj->block_header));
      _o->ownerhandle->absolute_ref = _o->ownerhandle->handleref.value =
        _o->reactors[0]->absolute_ref;
      LOG_TRACE ("LAYOUT.ownerhandle = " FORMAT_REF " (fixup)\n",
                 ARGS_REF (_o->ownerhandle));
    }
  if (dwg->header.version < R_2004)
    {
      _obj->plotsettings.plotview = dwg_find_tablehandle (
          dwg, _obj->plotsettings.plotview_name, "PLOTSETTINGS");
      /*
      if (!_obj->plotsettings.plotview)
        _obj->plotsettings.plotview = dwg_add_handleref (dwg, 0, 0, NULL);
      */
    }
  else
    _obj->plotsettings.plotview_name
        = dwg_handle_name (dwg, "PLOTSETTINGS", _obj->plotsettings.plotview);
}

static void
dxf_postprocess_MLINESTYLE (Dwg_Object *restrict obj)
{
  Dwg_Data *dwg = obj->parent;
  Dwg_Object_Object *_o = obj->tio.object;
  Dwg_Object_MLINESTYLE *_obj = obj->tio.object->tio.MLINESTYLE;

  // fix wrong ownerhandle
  if (_o->ownerhandle && _o->num_reactors &&
      _o->reactors[0]->absolute_ref != _o->ownerhandle->absolute_ref)
    {
      // must not be a BLOCK_HEADER, but a DICTIONARY
      _o->ownerhandle = _o->reactors[0];
      LOG_TRACE ("MLINESTYLE.ownerhandle = " FORMAT_REF " (fixup)\n",
                 ARGS_REF (_o->ownerhandle));
    }
}

static void
dxf_postprocess_PLOTSETTINGS (Dwg_Object *restrict obj)
{
  Dwg_Data *dwg = obj->parent;
  Dwg_Object_PLOTSETTINGS *_obj = obj->tio.object->tio.PLOTSETTINGS;

  if (dwg->header.version < R_2004)
    {
      _obj->plotview = dwg_find_tablehandle (dwg, _obj->plotview_name, "VIEW");
      /*
      if (!_obj->plotview)
        _obj->plotview = dwg_add_handleref (dwg, 0, 0, NULL);
      */
    }
  else
    _obj->plotview_name = dwg_handle_name (dwg, "VIEW", _obj->plotview);
}

// separate model_space and paper_space into its own fields, out of entries[]
static int
move_out_BLOCK_CONTROL (Dwg_Object *restrict obj,
                        Dwg_Object_BLOCK_CONTROL *restrict _ctrl,
                        const char *f)
{
  // move out this entry
  for (BITCODE_BL j = 0; j < _ctrl->num_entries; j++)
    {
      if (_ctrl->entries && _ctrl->entries[j]
          && _ctrl->entries[j]->absolute_ref == obj->handle.value)
        {
          LOG_TRACE ("remove %s from entries[%d]: " FORMAT_H "\n", f, j,
                     ARGS_H (obj->handle));
          _ctrl->num_entries--;
          LOG_TRACE ("BLOCK_CONTROL.num_entries = " FORMAT_BL "\n",
                     _ctrl->num_entries);
          if (j < _ctrl->num_entries) // if last, skip move, realloc is enough
            /* 1 < 4 (was 5, i.e 0-4): 1, 2, 4-1-1: 2 */
            memmove (&_ctrl->entries[j], &_ctrl->entries[j + 1],
                     (_ctrl->num_entries - j - 1) * sizeof (BITCODE_H));
          _ctrl->entries = (BITCODE_H *)realloc (
              _ctrl->entries, _ctrl->num_entries * sizeof (BITCODE_H));
          return 1;
        }
    }
  return 0;
}

static int
move_out_LTYPE_CONTROL (Dwg_Object *restrict obj,
                        Dwg_Object_LTYPE_CONTROL *restrict _ctrl,
                        const char *f)
{
  // move out this entry (byblock, bylayer, continuous)
  for (BITCODE_BL j = 0; j < _ctrl->num_entries; j++)
    {
      if (_ctrl->entries && _ctrl->entries[j]
          && _ctrl->entries[j]->absolute_ref == obj->handle.value)
        {
          LOG_TRACE ("remove %s from entries[%d]: " FORMAT_H "\n", f, j,
                     ARGS_H (obj->handle));
          _ctrl->num_entries--;
          LOG_TRACE ("LTYPE_CONTROL.num_entries = " FORMAT_BL "\n",
                     _ctrl->num_entries);
          if (j < _ctrl->num_entries)
            memmove (&_ctrl->entries[j], &_ctrl->entries[j + 1],
                     (_ctrl->num_entries - j - 1) * sizeof (BITCODE_H));
          if (_ctrl->num_entries)
            _ctrl->entries = (BITCODE_H *)realloc (
                _ctrl->entries, _ctrl->num_entries * sizeof (BITCODE_H));
          else
            {
              free (_ctrl->entries);
              _ctrl->entries = NULL;
            }
          return 1;
        }
    }
  return 0;
}

static void
postprocess_TEXTlike (Dwg_Object *obj)
{
  BITCODE_RC dataflags;
  BITCODE_2RD ins_pt, alignment_pt;
  BITCODE_RD elevation, oblique_angle, rotation, width_factor;
  BITCODE_BS generation, horiz_alignment, vert_alignment;
  BITCODE_H style;
  Dwg_Entity_TEXT *_obj = obj->tio.entity->tio.TEXT;

  dwg_dynapi_entity_value (_obj, obj->name, "dataflags", &dataflags, NULL);
  dwg_dynapi_entity_value (_obj, obj->name, "elevation", &elevation, NULL);
  dwg_dynapi_entity_value (_obj, obj->name, "ins_pt", &ins_pt, NULL);
  dwg_dynapi_entity_value (_obj, obj->name, "alignment_pt", &alignment_pt,
                           NULL);
  dwg_dynapi_entity_value (_obj, obj->name, "oblique_angle", &oblique_angle,
                           NULL);
  dwg_dynapi_entity_value (_obj, obj->name, "rotation", &rotation, NULL);
  dwg_dynapi_entity_value (_obj, obj->name, "width_factor", &width_factor,
                           NULL);
  dwg_dynapi_entity_value (_obj, obj->name, "generation", &generation, NULL);
  dwg_dynapi_entity_value (_obj, obj->name, "horiz_alignment",
                           &horiz_alignment, NULL);
  dwg_dynapi_entity_value (_obj, obj->name, "vert_alignment", &vert_alignment,
                           NULL);
  dwg_dynapi_entity_value (_obj, obj->name, "style", &style, NULL);

  if (elevation != 0.0)
    dataflags |= 1;
  if (alignment_pt.x != ins_pt.x || alignment_pt.y != ins_pt.y)
    dataflags |= 2;
  if (oblique_angle != 0.0)
    dataflags |= 4;
  if (rotation != 0.0)
    dataflags |= 8;
  if (width_factor != 1.0)
    dataflags |= 0x10;
  if (generation != 0)
    dataflags |= 0x20;
  if (horiz_alignment != 0)
    dataflags |= 0x40;
  if (vert_alignment != 0)
    dataflags |= 0x80;

  if (!style)
    {
      Dwg_Data *dwg = obj->parent;
      // set style to Standard (5.1.11)
      style = dwg_find_tablehandle_silent (dwg, "Standard", "STYLE");
      if (style)
        {
          if (style->handleref.code != 5)
            style = dwg_add_handleref (dwg, 5, style->absolute_ref, NULL);
          dwg_dynapi_entity_set_value (_obj, obj->name, "style", &style, 0);
          LOG_TRACE ("%s.style = " FORMAT_REF "\n", obj->name,
                     ARGS_REF (style));
        }
    }
  dwg_dynapi_entity_set_value (_obj, obj->name, "dataflags", &dataflags, 0);
  LOG_TRACE ("%s.dataflags => 0x%x\n", obj->name, dataflags);
}

int
is_textlike (Dwg_Object *restrict obj)
{
  // has dataflags and common text fields
  return obj->fixedtype == DWG_TYPE_TEXT || obj->fixedtype == DWG_TYPE_ATTDEF
         || obj->fixedtype == DWG_TYPE_ATTRIB;
}

#endif // DISABLE_DXF

#ifndef DISABLE_DXF

#  define GET_NUMFIELD(type)                                                  \
    {                                                                         \
      BITCODE_##type _size;                                                   \
      memcpy (&_size, &((char *)_obj)[f->offset], f->size);                   \
      LOG_TRACE ("%s = " FORMAT_##type "\n", f->name, _size);                 \
      return (long)_size;                                                     \
    }                                                                         \
    break;

static long
get_numfield_value (void *restrict _obj, const Dwg_DYNAPI_field *restrict f)
{
  long num = 0;
  if (f)
    {
      switch (f->size)
        {
        case 1:
          GET_NUMFIELD (B)
        case 2:
          GET_NUMFIELD (BS)
        case 4:
          GET_NUMFIELD (BL)
        case 8:
          GET_NUMFIELD (BLL)
        default:
          LOG_ERROR ("Unknown %s dynapi size %d", f->name, f->size);
        }
    }
  else if (strEQc (f->name, "transmatrix"))
    num = 16 * 8; // ignore
  else if (strEQc (f->name, "ref"))
    {
      if (f->size != 4) // fixed size
        LOG_WARN ("Need 4 ref array elements, have %ld", num)
      else
        LOG_TRACE ("Check ref[] 4 ok\n")
    }
  else
    LOG_ERROR ("Unknown num_%s field", f->name);
  return 0;
}
#  undef GET_NUMFIELD

/* For tables, entities and objects.
 */
static __nonnull ((1, 2, 3, 4))
Dxf_Pair *new_object (
    char *restrict name, char *restrict dxfname, Bit_Chain *restrict dat,
    Dwg_Data *restrict dwg, BITCODE_BL ctrl_id, BITCODE_BL *i_p)
{
  const int is_tu = 1;
  Dwg_Object *obj;
  Dxf_Pair *pair = dxf_read_pair (dat);
  Dwg_Object_APPID *_obj = NULL; // the smallest
  // we'd really need a Dwg_Object_TABLE or Dwg_Object_Generic type
  char ctrlname[80];
  char subclass[80];
  char text[256]; // FIXME
  int in_xdict = 0;
  int in_reactors = 0;
  int in_blkrefs = 0;
  int in_embedobj = 0;
  int is_entity = is_dwg_entity (name) || strEQc (name, "DIMENSION");
  // BITCODE_BL rcount1, rcount2, rcount3, vcount;
  // Bit_Chain *hdl_dat, *str_dat;
  int j = 0, k = 0, l = 0, error = 0;
  BITCODE_BL i = i_p ? *i_p : 0;
  int cur_cell = -1;
  size_t written = 0;
  BITCODE_RL curr_inserts = 0;
  BITCODE_RS flag = 0;
  BITCODE_BB scale_flag;
  BITCODE_3BD pt;
  Dwg_Object *ctrl;
  const Dwg_DYNAPI_field *prev_vstyle = NULL;
  subclass[0] = '\0';

  if (ctrl_id || i)
    {
      LOG_TRACE ("add %s [%d]\n", name, i)
    }
  else
    {
      if (strcmp (name, dxfname) != 0)
        LOG_TRACE ("add %s (%s)\n", name, dxfname)
      else
        LOG_TRACE ("add %s\n", name)
    }

  if (is_entity)
    {
      NEW_ENTITY (dwg, obj);

      obj->tio.entity->is_xdic_missing = 1;
      obj->tio.entity->color.index = 256; // ByLayer
      obj->tio.entity->ltype_scale = 1.0;
      if (strEQc (name, "SEQEND") || memBEGINc (name, "VERTEX"))
        obj->tio.entity->linewt = 0x1c;
      else
        obj->tio.entity->linewt = 0x1d;

      if (*name == '3')
        {
          // Looks dangerous but name[80] is big enough
          memmove (&name[1], &name[0], strlen (name) + 1);
          *name = '_';
        }
      if (strEQc (name, "DIMENSION"))
        { // the biggest
          ADD_ENTITY (DIMENSION_ANG2LN);
        }
      // broken (causes acad to hang on audit redraw)
      /*
      else if (is_class_unstable (name)
               && strEQc (name, "WIPEOUT"))
        {
          LOG_ERROR ("Unhandled DXF entity %s", name);
          name = (char*)"UNKNOWN_ENT";
          ADD_ENTITY (UNKNOWN_ENT);
          return pair;
        }
      */
      else
        {
          // clang-format off
          // ADD_ENTITY by name
          // check all objects
          #undef DWG_ENTITY
          #define DWG_ENTITY(token)             \
          if (strEQc (name, #token))            \
            {                                   \
              ADD_ENTITY (token);               \
              goto found_ent;                   \
            }                                   \
          else

          #include "objects.inc"
          //final else
          LOG_WARN ("Unknown object %s", name);

          #undef DWG_ENTITY
          #define DWG_ENTITY(token)
          // clang-format on
        found_ent:;
        }
    }
  else
    {
      NEW_OBJECT (dwg, obj);

      obj->tio.object->is_xdic_missing = 1;
      if (!ctrl_id) // no table
        {
          // clang-format off
          // ADD_OBJECT by name
          // check all objects
          #undef DWG_OBJECT
          #define DWG_OBJECT(token)         \
              if (strEQc (name, #token))    \
                {                           \
                  ADD_OBJECT (token);       \
                  goto found_obj;           \
                }

          #include "objects.inc"

          #undef DWG_OBJECT
          #define DWG_OBJECT(token)

        found_obj:
          ;
          // clang-format on
        }
      else // a table
        {
          if (strEQc (name, "BLOCK_RECORD"))
            {
              // strcpy (name, "BLOCK_HEADER");
              strcpy (ctrlname, "BLOCK_CONTROL");
            }
          else
            {
              strncpy (ctrlname, name, 70);
              ctrlname[69] = '\0';
              strcat (ctrlname, "_CONTROL");
            }

          // clang-format off
          ADD_TABLE_IF (LTYPE, LTYPE)
          else
          ADD_TABLE_IF (VPORT, VPORT)
          else
          ADD_TABLE_IF (APPID, APPID)
          else
          ADD_TABLE_IF (DIMSTYLE, DIMSTYLE)
          else
          ADD_TABLE_IF (LAYER, LAYER)
          else
          ADD_TABLE_IF (STYLE, STYLE)
          else
          ADD_TABLE_IF (UCS, UCS)
          else
          ADD_TABLE_IF (VIEW, VIEW)
          else
          ADD_TABLE_IF (BLOCK_RECORD, BLOCK_HEADER)
          else
          ADD_TABLE_IF (VX_TABLE_RECORD, VX_TABLE_RECORD)
          else
          // clang-format on
          {
            dwg->num_objects--;
            LOG_ERROR ("Unknown DXF AcDbSymbolTableRecord %s, skipping", name);
            return pair;
          }
        }
    }

  if (!_obj)
    {
      dwg->num_objects--;
      LOG_ERROR ("Empty _obj at DXF AcDbSymbolTableRecord %s, skipping", name);
      return pair;
    }
  ctrl = &dwg->object[ctrl_id];

  {
    const Dwg_DYNAPI_field *f1;
    BITCODE_B is_xref_ref = 1; // TODO only if it used by some entity <r14
    // set defaults not in dxf:
    if (dwg_dynapi_entity_field (obj->name, "is_xref_ref"))
      dwg_dynapi_entity_set_value (_obj, obj->name, "is_xref_ref",
                                   &is_xref_ref, 0);
    if ((f1 = dwg_dynapi_entity_field (obj->name, "scale_flag"))
        && (memBEGINc (f1->type, "BB")))
      {
        scale_flag = 3;
        dwg_dynapi_entity_set_value (_obj, obj->name, "scale_flag",
                                     &scale_flag, 0);
        LOG_TRACE ("%s.scale_flag = 3 (default)\n", obj->name);
      }
    if ((f1 = dwg_dynapi_entity_field (obj->name, "width_factor"))
        && (memBEGINc (f1->type, "RD") || memBEGINc (f1->type, "BD")))
      {
        BITCODE_BD width_factor = 1.0;
        dwg_dynapi_entity_set_value (_obj, obj->name, "width_factor",
                                     &width_factor, 0);
        LOG_TRACE ("%s.width_factor = 1.0 (default)\n", obj->name);
      }
    if ((f1 = dwg_dynapi_entity_field (obj->name, "scale"))
        && (memBEGINc (f1->type, "3BD")))
      {
        pt.x = pt.y = pt.z = 1.0;
        dwg_dynapi_entity_set_value (_obj, obj->name, "scale", &pt, 0);
        LOG_TRACE ("%s.scale = (1,1,1) (default)\n", obj->name);
        pt.x = pt.y = pt.z = 0.0;
      }
    if ((f1 = dwg_dynapi_entity_field (obj->name, "extrusion"))
        && (memBEGINc (f1->type, "BE") || memBEGINc (f1->type, "3BD")))
      {
        pt.x = pt.y = 0.0;
        pt.z = 1.0;
        dwg_dynapi_entity_set_value (_obj, obj->name, "extrusion", &pt, 0);
        LOG_TRACE ("%s.extrusion = (0,0,1) (default)\n", obj->name);
        pt.z = 0.0;
      }
  }
  // more DXF defaults
  if (obj->fixedtype == DWG_TYPE_LAYOUT)
    {
      Dwg_Object_LAYOUT *o = obj->tio.object->tio.LAYOUT;
      o->plotsettings.paper_units = 1.0; // default
    }
  else if (obj->fixedtype == DWG_TYPE_PLOTSETTINGS)
    {
      Dwg_Object_PLOTSETTINGS *o = obj->tio.object->tio.PLOTSETTINGS;
      o->paper_units = 1.0; // default
    }
  else if (obj->fixedtype == DWG_TYPE_DIMSTYLE)
    {
      Dwg_Object_DIMSTYLE *o = obj->tio.object->tio.DIMSTYLE;
      o->DIMSCALE = o->DIMLFAC = o->DIMTFAC = 1.0; // default
      o->DIMALTU = o->DIMLUNIT = 2;                // default
      o->DIMFIT = 3;
      o->DIMLWD = o->DIMLWE = -2;
    }
  else if (obj->fixedtype == DWG_TYPE_TABLESTYLE)
    {
      Dwg_Object_TABLESTYLE *o = obj->tio.object->tio.TABLESTYLE;
      o->num_rowstyles = 3;
      o->rowstyles = (Dwg_TABLESTYLE_rowstyles *)xcalloc (
          3, sizeof (Dwg_TABLESTYLE_rowstyles));
      if (!o->rowstyles)
        {
          o->num_rowstyles = 0;
          goto invalid_dxf;
        }
      for (j = 0; j < 3; j++)
        {
          o->rowstyles[j].borders = (Dwg_TABLESTYLE_border *)xcalloc (
              6, sizeof (Dwg_TABLESTYLE_border));
          o->rowstyles[j].num_borders = 6;
          for (k = 0; k < 3; k++) // defaults: ByLayer
            {
              o->rowstyles[j].borders[k].visible = 1;
              o->rowstyles[j].borders[k].linewt = 29;
              o->rowstyles[j].borders[k].color.index = 256;
            }
        }
      k = 0;
      j = 0;
    }
  /*
  else if (is_textlike (obj))
    {
      BITCODE_RC dataflags = 0x2 + 0x4 + 0x8;
      dwg_dynapi_entity_set_value (_obj, obj->name, "dataflags",
                                   &dataflags, 0);
    }
  */
  else if (obj->fixedtype == DWG_TYPE_MTEXT)
    {
      BITCODE_H style;
      Dwg_Entity_MTEXT *o = obj->tio.entity->tio.MTEXT;
      if (!o->x_axis_dir.x)
        o->x_axis_dir.x = 1.0;
      // set style to Standard (5.1.11)
      style = dwg_find_tablehandle_silent (dwg, "Standard", "STYLE");
      if (style)
        {
          if (style->handleref.code != 5)
            style = dwg_add_handleref (dwg, 5, style->absolute_ref, NULL);
          o->style = style;
        }
    }
  else if (obj->fixedtype == DWG_TYPE_VERTEX_2D)
    {
      // TODO better the layer of its owner
      obj->tio.entity->layer = dwg->header_vars.CLAYER; // default
    }
  // Some objects have various subtypes under one name, like DIMENSION.
  // TODO OBJECTCONTEXTDATA, ...

  // read table fields until next 0 table or 0 ENDTAB
  while (pair != NULL && pair->code != 0)
    {
    start_loop:
      if (pair == NULL)
        {
          pair = dxf_read_pair (dat);
          DXF_RETURN_EOF (pair);
        }
#  if 0
      // don't set defaults. TODO but needed to reset counters j, k, l
      if ((pair->type == DWG_VT_INT8 || pair->type == DWG_VT_INT16 || pair->type == DWG_VT_BOOL) &&
          pair->value.i == 0)
        goto next_pair;
      else if (pair->type == DWG_VT_REAL && pair->value.d == 0.0)
        goto next_pair;
      else if ((pair->type == DWG_VT_INT32 || pair->type == DWG_VT_INT64) &&
               pair->value.l == 0L)
        goto next_pair;
#  endif
      // start_switch:
      switch (pair->code)
        { // common flags: name, xref
        case 0: // dead code. disabled in loop above
          if (strEQc (name, "SEQEND"))
            dxf_postprocess_SEQEND (obj);
          else if (strEQc (name, "LAYOUT"))
            dxf_postprocess_LAYOUT (obj);
          else if (strEQc (name, "PLOTSETTINGS"))
            dxf_postprocess_PLOTSETTINGS (obj);
          else if (strEQc (name, "MLINESTYLE"))  // FIXME. not triggered
            dxf_postprocess_MLINESTYLE (obj);
          return pair;
        case 105: /* DIMSTYLE only for 5 */
          if (strNE (name, "DIMSTYLE"))
            goto object_default;
          // fall through
        case 5:
          {
            // check for existing BLOCK_HEADER.*Model_Space
            if (obj->fixedtype == DWG_TYPE_BLOCK_HEADER
                && dwg->object[0].handle.value == pair->value.u
                && obj->tio.object->tio.BLOCK_HEADER
                       != dwg->object[0].tio.object->tio.BLOCK_HEADER
                && dwg->num_objects)
              {
                dwg->num_objects--;
                free (obj->tio.object->tio.BLOCK_HEADER);
                obj->tio.object->tio.BLOCK_HEADER = NULL;
                obj = &dwg->object[0];
                _obj = obj->tio.object->tio.APPID;
                LOG_TRACE ("Reuse existing BLOCK_HEADER.*Model_Space %X [0]\n",
                           pair->value.u)
              }
          // special-case VIEWPORT -> VX.
          if (strEQc (name, "VIEWPORT") && dwg->header.version < R_2004
              && dwg->header.version > R_11 && pair->code == 5)
            {
              Dwg_Object_VX_TABLE_RECORD *vx = dwg_add_VX (dwg, "");
              Dwg_Object *vxobj = dwg_obj_generic_to_object (vx, &error);
              Dwg_Entity_VIEWPORT *_vobj = obj->tio.entity->tio.VIEWPORT;
              if (dwg->header_vars.HANDSEED)
                {
                  vxobj->handle.value = dwg_new_handseed (dwg);
                }
              // vx->is_on = 1;
              vx->viewport
                  = dwg_add_handleref (dwg, 4, pair->value.u, NULL);
              LOG_TRACE ("VX_TABLE_RECORD.viewport = " FORMAT_REF " [H 4]\n",
                         ARGS_REF (vx->viewport));
              _vobj->vport_entity_header
                  = dwg_add_handleref (dwg, 5, vxobj->handle.value, NULL);
              LOG_TRACE ("VIEWPORT.vport_entity_header = " FORMAT_REF " [H 5]\n",
                         ARGS_REF (_vobj->vport_entity_header));
            }
            if (strNE (name, "DIMSTYLE") || pair->code == 105)
              {
                obj->handle.value = pair->value.u;
                dwg_add_handle (&obj->handle, 0, pair->value.u, obj);
                LOG_TRACE ("%s.handle = " FORMAT_H " [H 5]\n", name,
                           ARGS_H (obj->handle));
                if (ctrl_id)
                  {
                    // add to ctrl "entries" HANDLE_VECTOR.
                    // mspace and pspace are filtered out later
                    Dwg_Object_BLOCK_CONTROL *_ctrl
                        = dwg->object[ctrl_id].tio.object->tio.BLOCK_CONTROL;
                    BITCODE_H *hdls = NULL;
                    BITCODE_BL num_entries = 0;

                    if ((int)i < 0)
                      i = 0;
                    dwg_dynapi_entity_value (_ctrl, ctrlname, "num_entries",
                                             &num_entries, NULL);
                    if (i >= num_entries)
                      {
                        // DXF often lies about num_entries, skipping defaults
                        // e.g. BLOCK_CONTROL contains mspace+pspace in DXF,
                        // but in the DWG they are extra. But this is fixed at
                        // case 2, not here.
                        LOG_TRACE (
                            "Misleading %s.num_entries %d for %dth entry\n",
                            ctrlname, num_entries, i);
                        i = num_entries;
                        num_entries++;
                        dwg_dynapi_entity_set_value (
                            _ctrl, ctrlname, "num_entries", &num_entries, 0);
                        LOG_TRACE ("%s.num_entries = %d [BL 70]\n", ctrlname,
                                   num_entries);
                      }
                    dwg_dynapi_entity_value (_ctrl, ctrlname, "entries", &hdls,
                                             NULL);
                    if (!hdls)
                      hdls = (BITCODE_H *)xcalloc (num_entries,
                                                   sizeof (Dwg_Object_Ref *));
                    else
                      hdls = (BITCODE_H *)realloc (
                          hdls, num_entries * sizeof (Dwg_Object_Ref *));
                    if (pair->value.u && !hdls)
                      goto invalid_dxf;
                    hdls[i] = dwg_add_handleref (dwg, 2, pair->value.u, obj);
                    dwg_dynapi_entity_set_value (_ctrl, ctrlname, "entries",
                                                 &hdls, 0);
                    LOG_TRACE ("%s.%s[%d] = " FORMAT_REF " [H* 0]\n", ctrlname,
                               "entries", i, ARGS_REF (hdls[i]));
                  }
              }
            else if (pair->value.u && *buf)
              {
                // DIMSTYLE 5: DIMBLK name -> handle
                BITCODE_H handle
                    = dwg_find_tablehandle_silent (dwg, buf, "BLOCK");
                if (!handle)
                  {
                    obj_hdls = array_push (obj_hdls, "DIMBLK", buf,
                                           obj->tio.object->objid);
                    LOG_TRACE ("DIMSTYLE.DIMBLK: name %s -> H later\n", buf);
                  }
              }
          }
          break;
        case 8:
          if (is_entity && pair->value.s)
            {
              BITCODE_H handle = find_tablehandle (dwg, pair);
              if (!handle)
                {
                  obj_hdls = array_push (obj_hdls, "layer", pair->value.s,
                                         obj->tio.object->objid);
                  LOG_TRACE ("%s.layer: name %s -> H later\n", obj->name,
                             pair->value.s)
                }
              else
                {
                  dwg_dynapi_common_set_value (_obj, "layer", &handle, 1);
                  LOG_TRACE ("%s.layer = %s " FORMAT_REF " [H 8]\n", name,
                             pair->value.s, ARGS_REF (handle));
                }
              break;
            }
          // fall through
        case 100: // for nested structs
          if (pair->code == 100 && pair->value.s)
            {
              strncpy (subclass, pair->value.s, 79);
              subclass[79] = '\0';
              // set the real objname
              if (strEQc (obj->name, "DIMENSION_ANG2LN")
                  || strEQc (obj->name, "DIMENSION"))
                {
                  // we rather checked the flag before
                  if (strEQc (subclass, "AcDbRotatedDimension"))
                    {
                      obj->type = obj->fixedtype = DWG_TYPE_DIMENSION_LINEAR;
                      obj->name = (char *)"DIMENSION_LINEAR";
                      free (obj->dxfname);
                      obj->dxfname = strdup (obj->name);
                      strcpy (name, obj->name);
                      LOG_TRACE ("change type to %s\n", name);
                    }
                  else if (strEQc (subclass, "AcDbAlignedDimension"))
                    {
                      // could be DIMENSION_LINEAR also. changed later on those
                      // new pairs
                      obj->type = obj->fixedtype = DWG_TYPE_DIMENSION_ALIGNED;
                      obj->name = (char *)"DIMENSION_ALIGNED";
                      free (obj->dxfname);
                      obj->dxfname = strdup (obj->name);
                      strcpy (name, obj->name);
                      LOG_TRACE ("change type to %s\n", name);
                    }
                  else if (strEQc (subclass, "AcDbOrdinateDimension"))
                    {
                      obj->type = obj->fixedtype = DWG_TYPE_DIMENSION_ORDINATE;
                      obj->name = (char *)"DIMENSION_ORDINATE";
                      free (obj->dxfname);
                      obj->dxfname = strdup (obj->name);
                      strcpy (name, obj->name);
                      LOG_TRACE ("change type to %s\n", name);
                    }
                  else if (strEQc (subclass, "AcDbDiametricDimension"))
                    {
                      obj->type = obj->fixedtype = DWG_TYPE_DIMENSION_DIAMETER;
                      obj->name = (char *)"DIMENSION_DIAMETER";
                      free (obj->dxfname);
                      obj->dxfname = strdup (obj->name);
                      strcpy (name, obj->name);
                      LOG_TRACE ("change type to %s\n", name);
                    }
                  else if (strEQc (subclass, "AcDbRadialDimension"))
                    {
                      UPGRADE_ENTITY (DIMENSION_ANG2LN, DIMENSION_RADIUS)
                    }
                  else if (strEQc (subclass, "AcDb3PointAngularDimension"))
                    {
                      UPGRADE_ENTITY (DIMENSION_ANG2LN, DIMENSION_ANG3PT)
                    }
                }
              if (strEQc (obj->name, "DIMENSION_ALIGNED")
                  && strEQc (subclass, "AcDbRotatedDimension"))
                {
                  UPGRADE_ENTITY (DIMENSION_ALIGNED, DIMENSION_LINEAR)
                }
              // set the real objname
              else if (strEQc (obj->name, "POLYLINE_2D"))
                {
                  if (strEQc (subclass, "AcDb3dPolyline"))
                    {
                      UPGRADE_ENTITY (POLYLINE_2D, POLYLINE_3D)
                    }
                  else if (strEQc (subclass, "AcDbPolyFaceMesh"))
                    {
                      UPGRADE_ENTITY (POLYLINE_2D, POLYLINE_PFACE)
                    }
                  else if (strEQc (subclass, "AcDbPolygonMesh"))
                    {
                      UPGRADE_ENTITY (POLYLINE_2D, POLYLINE_MESH)
                    }
                }
              else if (strEQc (obj->name, "VERTEX_2D"))
                {
                  Dwg_Object_Ref *owner = obj->tio.entity->ownerhandle;
                  Dwg_Object *parent = dwg_ref_object (dwg, owner);
                  if (parent && obj->tio.entity->layer
                      && parent->supertype == DWG_SUPERTYPE_ENTITY
                      && parent->tio.entity->layer
                      && obj->tio.entity->layer->absolute_ref
                             != parent->tio.entity->layer->absolute_ref)
                    {
                      obj->tio.entity->layer = parent->tio.entity->layer;
                      LOG_TRACE ("=> VERTEX.layer = " FORMAT_REF "\n",
                                 ARGS_REF (obj->tio.entity->layer));
                    }
                  if (strEQc (subclass, "AcDb3dPolylineVertex"))
                    {
                      UPGRADE_ENTITY (VERTEX_2D, VERTEX_3D)
                    }
                  else if (strEQc (subclass, "AcDbPolyFaceMeshVertex"))
                    { // _MESH or _PFACE:
                      if (parent
                          && parent->fixedtype == DWG_TYPE_POLYLINE_PFACE)
                        {
                          UPGRADE_ENTITY (VERTEX_2D, VERTEX_PFACE)
                        }
                      else
                        { // AcDbPolygonMesh
                          UPGRADE_ENTITY (VERTEX_2D, VERTEX_MESH)
                        }
                    }
                  else if (strEQc (subclass, "AcDbFaceRecord"))
                    {
                      UPGRADE_ENTITY (VERTEX_2D, VERTEX_PFACE_FACE)
                    }
                }
              else if (strEQc (obj->name, "INSERT")
                       && strEQc (subclass, "AcDbMInsertBlock"))
                {
                  UPGRADE_ENTITY (INSERT, MINSERT)
                }

              // When we have all proper types, check proper subclasses.
              // If the subclass is allowed in this object.
              if (!dwg_has_subclass (obj->name, subclass))
                {
                  if (is_type_stable (obj->fixedtype))
                    {
                      LOG_ERROR ("Illegal subclass %s in object %s", subclass,
                                 obj->name);
                      return NULL;
                    }
                  else
                    {
                      LOG_WARN ("Illegal subclass %s in object %s", subclass,
                                obj->name);
                      *subclass = '\0';
                    }
                }
              if (strEQc (subclass, "AcDbField"))
                {
                  dxf_free_pair (pair);
                  pair = add_FIELD (obj, dat); // NULL for success
                  if (!pair)
                    goto next_pair;
                  else
                    goto start_loop; /* failure */
                }
              if (strEQc (subclass, "AcDbDetailViewStyle")
                  && obj->fixedtype != DWG_TYPE_DETAILVIEWSTYLE)
                {
                  LOG_ERROR ("Invalid subclass %s in object %s", subclass,
                             obj->name);
                  return NULL;
                }

              // with PERSUBENTMGR
              if (obj->fixedtype == DWG_TYPE_PERSUBENTMGR
                  && strEQc (subclass, "AcDbPersSubentManager"))
                {
                  dxf_free_pair (pair);
                  pair = dxf_read_pair (dat);
                  pair = add_PERSUBENTMGR (obj, dat, pair); // NULL for success
                  if (!pair)
                    goto next_pair;
                  else
                    goto start_loop; /* failure */
                }
              // with ASSOCDEPENDENCY or ACDBASSOCGEOMDEPENDENCY
              else if (strEQc (obj->name, "ASSOCDEPENDENCY")
                       && strEQc (subclass, "AcDbAssocDependency"))
                {
                  dxf_free_pair (pair);
                  pair = add_ASSOCDEPENDENCY (obj, dat); // NULL for success
                  if (!pair)
                    goto next_pair;
                  else
                    goto start_loop; /* failure */
                }
              // with ASSOCVALUEDEPENDENCY or ACDBASSOCGEOMDEPENDENCY
              else if (strstr (obj->name, "ASSOC")
                       && strstr (obj->name, "DEPENDENCY")
                       && strEQc (subclass, "AcDbAssocDependency"))
                {
                  dxf_free_pair (pair);
                  pair
                      = add_sub_ASSOCDEPENDENCY (obj, dat); // NULL for success
                  if (!pair)
                    goto next_pair;
                  else
                    goto start_loop; /* failure */
                }
              // with ASSOC2DCONSTRAINTGROUP, ASSOCNETWORK, ASSOCACTION
              else if (strstr (obj->name, "ASSOC")
                       && strEQc (subclass, "AcDbAssocAction"))
                {
                  dxf_free_pair (pair);
                  pair = dxf_read_pair (dat);
                  pair = add_ASSOCACTION (obj, dat, pair); // NULL for success
                  if (!pair)
                    {
                      // TODO: yet unsupported
                      if (strEQc (name, "ASSOC2DCONSTRAINTGROUP"))
                        return dxf_read_pair (dat);
                      else
                        goto next_pair;
                    }
                  else
                    goto start_loop; /* failure */
                }
              else if (strstr (obj->name, "ASSOC")
                       && strEQc (subclass, "AcDbAssocNetwork"))
                {
                  dxf_free_pair (pair);
                  pair = dxf_read_pair (dat);
                  LOG_TRACE ("add_ASSOCNETWORK\n")
                  pair = add_ASSOCNETWORK (obj, dat, pair); // NULL for success
                  if (!pair)
                    goto next_pair;
                  else
                    goto start_loop; /* failure */
                }
                // strict subclasses (functable?)
#  define CHK_SUBCLASS(cppname, addmethod)                                    \
    if (strEQc (subclass, #cppname))                                          \
      {                                                                       \
        dxf_free_pair (pair);                                                 \
        LOG_TRACE ("add_" #addmethod "\n")                                    \
        pair = add_##addmethod (obj, dat); /* NULL for success */             \
        if (!pair)                                                            \
          goto next_pair;                                                     \
        else                                                                  \
          goto start_loop; /* failure */                                      \
      }

              // clang-format off
              else CHK_SUBCLASS (AcDbBlockParameter, AcDbBlockParameter)
              else CHK_SUBCLASS (AcDbBlockGripExpr, AcDbBlockGripExpr)
              else CHK_SUBCLASS (AcDbBlockAlignmentGrip, BLOCKALIGNMENTGRIP)
              else CHK_SUBCLASS (AcDbBlockLinearGrip, BLOCKALIGNMENTGRIP)
              else CHK_SUBCLASS (AcDbBlockFlipGrip, BLOCKFLIPGRIP)
              else CHK_SUBCLASS (AcDbRenderEnvironment, RENDERENVIRONMENT)
              else CHK_SUBCLASS (AcDbRenderGlobal, RENDERGLOBAL)
              else CHK_SUBCLASS (AcDbRenderEntry, RENDERENTRY)
              else CHK_SUBCLASS (AcDbRenderSettings, RENDERSETTINGS)
              // clang-format on

              // more DYNBLOCKs
#  define else_do_strict_subclass(SUBCLASS)                                   \
    else if (strEQc (subclass, #SUBCLASS))                                    \
    {                                                                         \
      dxf_free_pair (pair);                                                   \
      LOG_TRACE ("add_" #SUBCLASS "\n")                                       \
      pair = add_##SUBCLASS (obj, dat);                                       \
      if (!pair) /* NULL for success */                                       \
        goto next_pair;                                                       \
      else                                                                    \
        goto start_loop; /* failure */                                        \
    }

                  // clang-format off
              else_do_strict_subclass (AcDbBlock1PtParameter)
              else_do_strict_subclass (AcDbBlock2PtParameter)
              else_do_strict_subclass (AcDbBlockAction)
              else_do_strict_subclass (AcDbBlockActionWithBasePt)
              else_do_strict_subclass (AcDbBlockFlipAction)
              else_do_strict_subclass (AcDbBlockMoveAction)
              else_do_strict_subclass (AcDbBlockRotationAction)
              else_do_strict_subclass (AcDbBlockScaleAction)
              else_do_strict_subclass (AcDbBlockStretchAction)
              else_do_strict_subclass (AcDbBlockRotationParameter)
              // clang-format on
            }
          break;
        case 101:
          if (pair->value.s && strEQc (pair->value.s, "Embedded Object"))
            in_embedobj = 1;
          break;
        case 102:
          if (pair->value.s && strEQc (pair->value.s, "{ACAD_XDICTIONARY"))
            in_xdict = 1;
          else if (pair->value.s && strEQc (pair->value.s, "{ACAD_REACTORS"))
            in_reactors = 1;
          else if (ctrl_id && pair->value.s
                   && strEQc (pair->value.s, "{BLKREFS"))
            in_blkrefs = 1; // unique handle 331
          else if (pair->value.s && strEQc (pair->value.s, "}"))
            in_reactors = in_xdict = in_blkrefs = 0;
          else if (pair->value.s && strEQc (name, "XRECORD"))
            pair = add_xdata (dat, obj, pair);
          else
            LOG_WARN ("Unknown DXF code 102 %s in %s", pair->value.s, name)
          break;
        case 331:
          if (ctrl_id && in_blkrefs) // BLKREFS TODO
            {
              BITCODE_H *inserts = NULL;
              BITCODE_H hdl;
              BITCODE_RL num_inserts;
              dwg_dynapi_entity_value (_obj, obj->name, "num_inserts",
                                       &num_inserts, 0);
              if (curr_inserts)
                dwg_dynapi_entity_value (_obj, obj->name, "inserts", &inserts,
                                         0);
              if (curr_inserts + 1 > num_inserts)
                {
                  LOG_HANDLE ("  extending %s.num_inserts %d < %d\n",
                              obj->name, num_inserts, curr_inserts + 1);
                  num_inserts = curr_inserts + 1;
                  dwg_dynapi_entity_set_value (_obj, obj->name, "num_inserts",
                                               &num_inserts, 0);
                }
              if (inserts)
                inserts = (BITCODE_H *)realloc (
                    inserts, num_inserts * sizeof (BITCODE_H));
              else
                inserts
                    = (BITCODE_H *)xcalloc (num_inserts, sizeof (BITCODE_H));
              if (num_inserts && !inserts)
                goto invalid_dxf;
              dwg_dynapi_entity_set_value (_obj, obj->name, "inserts",
                                           &inserts, 0);
              hdl = dwg_add_handleref (dwg, 4, pair->value.u,
                                       NULL); // absolute
              LOG_TRACE ("%s.inserts[%d] = " FORMAT_REF " [H* 331]\n",
                         obj->name, curr_inserts, ARGS_REF (hdl));
              inserts[curr_inserts++] = hdl;
              break;
            }
          else if (pair->code == 331 && obj->fixedtype == DWG_TYPE_LAYOUT)
            {
              Dwg_Object_LAYOUT *o = obj->tio.object->tio.LAYOUT;
              o->active_viewport
                  = dwg_add_handleref (dwg, 4, pair->value.u, NULL);
              LOG_TRACE ("%s.active_viewport = " FORMAT_REF " [H 331]\n",
                         obj->name, ARGS_REF (o->active_viewport));
              break;
            }
          // fall through
        case 330:
          if (in_reactors)
            {
              BITCODE_BL num = is_entity ? obj->tio.entity->num_reactors
                                         : obj->tio.object->num_reactors;
              BITCODE_H reactor = dwg_add_handleref (dwg, 4, pair->value.u,
                                                     NULL); // always abs
              LOG_TRACE ("%s.reactors[%d] = " FORMAT_REF " [H* 330]\n", name,
                         num, ARGS_REF (reactor));
              if (is_entity)
                {
                  obj->tio.entity->reactors
                      = (BITCODE_H *)realloc (obj->tio.entity->reactors,
                                              (num + 1) * sizeof (BITCODE_H));
                  obj->tio.entity->reactors[num] = reactor;
                  obj->tio.entity->num_reactors++;
                }
              else
                {
                  obj->tio.object->reactors
                      = (BITCODE_H *)realloc (obj->tio.object->reactors,
                                              (num + 1) * sizeof (BITCODE_H));
                  obj->tio.object->reactors[num] = reactor;
                  obj->tio.object->num_reactors++;
                  if (num == 0 && !dwg_obj_is_table(obj) && !obj->tio.object->ownerhandle)
                    {
                      obj->tio.object->ownerhandle = reactor;
                      LOG_TRACE ("%s.ownerhandle = " FORMAT_REF "\n",
                                 obj->name,
                                 ARGS_REF (obj->tio.object->ownerhandle));
                    }
                }
            }
          else if (pair->code == 330 && obj->fixedtype == DWG_TYPE_LAYOUT
                   && obj->tio.object->ownerhandle)
            {
              Dwg_Object_LAYOUT *o = obj->tio.object->tio.LAYOUT;
              o->block_header
                  = dwg_add_handleref (dwg, 4, pair->value.u, NULL);
              LOG_TRACE ("%s.block_header = " FORMAT_REF " [H 330]\n",
                         obj->name, ARGS_REF (o->block_header));
            }
          // valid ownerhandle, if not XRECORD with an ownerhandle already
          else if (pair->code == 330
                   && (obj->fixedtype != DWG_TYPE_XRECORD
                       || !obj->tio.object->ownerhandle))
            {
              BITCODE_H owh;
              if (is_obj_absowner (obj))
                owh = dwg_add_handleref (dwg, 4, pair->value.u, NULL);
              else // relative
                owh = dwg_add_handleref (dwg, 4, pair->value.u, obj);
              if (is_entity)
                obj->tio.entity->ownerhandle = owh;
              else
                obj->tio.object->ownerhandle = owh;
              LOG_TRACE ("%s.ownerhandle = " FORMAT_REF " [H 330]\n", name,
                         ARGS_REF (owh));
            }
          break;
        case 350: // DICTIONARY softhandle
        case 360: // {ACAD_XDICTIONARY or some hardowner
          if (pair->code == 360 && in_xdict)
            {
              BITCODE_H xdic = dwg_add_handleref (dwg, 3, pair->value.u, obj);
              if (is_entity)
                {
                  obj->tio.entity->xdicobjhandle = xdic;
                  obj->tio.entity->is_xdic_missing = 0;
                }
              else
                {
                  obj->tio.object->xdicobjhandle = xdic;
                  obj->tio.object->is_xdic_missing = 0;
                }
              LOG_TRACE ("%s.xdicobjhandle = " FORMAT_REF " [H 360]\n", name,
                         ARGS_REF (xdic));
              break;
            }
          // DICTIONARY or DICTIONARYWDFLT, but not DICTIONARYVAR
          else if (memBEGINc (name, "DICTIONARY")
                   && strNE (name, "DICTIONARYVAR") && *text)
            {
              add_dictionary_itemhandles (obj, pair, text);
              break;
            }
          else if (pair->code == 360 && // hardowner, not soft
                   (obj->fixedtype == DWG_TYPE_IMAGE
                    || obj->fixedtype == DWG_TYPE_WIPEOUT))
            {
              BITCODE_H ref = dwg_add_handleref (dwg, 3, pair->value.u, obj);
              dwg_dynapi_entity_set_value (_obj, obj->name, "imagedefreactor",
                                           &ref, 0);
              LOG_TRACE ("%s.imagedefreactor = " FORMAT_REF " [H %d]\n", name,
                         ARGS_REF (ref), pair->code);
              break;
            }
          // fall through
        case 340:
          if (pair->code == 340 && strEQc (name, "GROUP"))
            {
              Dwg_Object_GROUP *o = obj->tio.object->tio.GROUP;
              BITCODE_H hdl = dwg_add_handleref (dwg, 5, pair->value.u, obj);
              LOG_TRACE ("GROUP.groups[%d] = " FORMAT_REF " [H* 340]\n",
                         o->num_groups, ARGS_REF (hdl));
              o->groups = (BITCODE_H *)realloc (
                  o->groups, (o->num_groups + 1) * sizeof (BITCODE_H));
              o->groups[o->num_groups] = hdl;
              o->num_groups++;
              break;
            }
          // fall through
        case 341:
          if (pair->code == 341 && strEQc (name, "VIEWPORT"))
            {
              Dwg_Entity_VIEWPORT *o = obj->tio.entity->tio.VIEWPORT;
              int code = dwg->header.version >= R_2004 ? 4 : 5;
              BITCODE_H hdl
                  = dwg_add_handleref (dwg, code, pair->value.u, obj);
              LOG_TRACE ("VIEWPORT.frozen_layers[%d] = " FORMAT_REF
                         " [H* 341]\n",
                         o->num_frozen_layers, ARGS_REF (hdl));
              o->frozen_layers = (BITCODE_H *)realloc (
                  o->frozen_layers,
                  (o->num_frozen_layers + 1) * sizeof (BITCODE_H));
              o->frozen_layers[o->num_frozen_layers] = hdl;
              o->num_frozen_layers++;
              break;
            }
          // fall through
        case 2:
          if (ctrl_id && pair->code == 2)
            {
              dwg_dynapi_entity_set_value (_obj, obj->name, "name",
                                           &pair->value, is_tu);
              LOG_TRACE ("%s.name = %s [T 2]\n", name, pair->value.s);
              if (!pair->value.s)
                break;
              assert (i_p); // needs ctrl_id
              *i_p = i + 1;
              if (strEQc (name, "BLOCK_RECORD"))
                {
                  // separate mspace and pspace into its own fields
                  Dwg_Object_BLOCK_CONTROL *_ctrl
                      = ctrl->tio.object->tio.BLOCK_CONTROL;
                  if (!strcasecmp (pair->value.s, "*Paper_Space"))
                    {
                      const char *f = "paper_space";
                      _ctrl->paper_space
                          = dwg_add_handleref (dwg, 3, obj->handle.value, obj);
                      LOG_TRACE ("%s.%s = " FORMAT_REF " [H 0]\n", ctrlname, f,
                                 ARGS_REF (_ctrl->paper_space));
                      dwg->header_vars.BLOCK_RECORD_PSPACE
                          = dwg_add_handleref (dwg, 5, obj->handle.value, obj);
                      // move out of entries
                      if (move_out_BLOCK_CONTROL (obj, _ctrl, f))
                        *i_p = i;
                    }
                  else if (!strcasecmp (pair->value.s, "*Model_Space"))
                    {
                      const char *f = "model_space";
                      _ctrl->model_space
                          = dwg_add_handleref (dwg, 3, obj->handle.value, obj);
                      LOG_TRACE ("%s.%s = " FORMAT_REF " [H 0]\n", ctrlname, f,
                                 ARGS_REF (_ctrl->model_space));
                      dwg->header_vars.BLOCK_RECORD_MSPACE
                          = dwg_add_handleref (dwg, 5, obj->handle.value, obj);
                      // move out of entries
                      if (move_out_BLOCK_CONTROL (obj, _ctrl, f))
                        *i_p = i;
                    }
                }
              else if (strEQc (name, "LTYPE"))
                {
                  // separate bylayer and byblock into its own fields
                  Dwg_Object_LTYPE_CONTROL *_ctrl
                      = ctrl->tio.object->tio.LTYPE_CONTROL;
                  if (!strcasecmp (pair->value.s, "ByLayer"))
                    {
                      const char *f = "bylayer";
                      _ctrl->bylayer
                          = dwg_add_handleref (dwg, 3, obj->handle.value, obj);
                      LOG_TRACE ("%s.%s = " FORMAT_REF " [H 0]\n", ctrlname, f,
                                 ARGS_REF (_ctrl->bylayer));
                      dwg->header_vars.LTYPE_BYLAYER
                          = dwg_add_handleref (dwg, 5, obj->handle.value, obj);
                      // move out of entries
                      if (move_out_LTYPE_CONTROL (obj, _ctrl, f))
                        *i_p = i;
                    }
                  else if (!strcasecmp (pair->value.s, "ByBlock"))
                    {
                      const char *f = "byblock";
                      _ctrl->byblock
                          = dwg_add_handleref (dwg, 3, obj->handle.value, obj);
                      LOG_TRACE ("%s.%s = " FORMAT_REF " [H 0]\n", ctrlname, f,
                                 ARGS_REF (_ctrl->byblock));
                      dwg->header_vars.LTYPE_BYBLOCK
                          = dwg_add_handleref (dwg, 5, obj->handle.value, obj);
                      // move out of entries
                      if (move_out_LTYPE_CONTROL (obj, _ctrl, f))
                        *i_p = i;
                    }
                }
              break;
            }
          if (pair->code == 2 && strEQc (name, "MLINE"))
            {
              // ignore name of mlinestyle, already set by ->mlinestyle
              break;
            }
          if (pair->code == 2 && dwg_obj_is_3dsolid (obj))
            {
              BITCODE_BL revision_major;
              BITCODE_BS revision_minor1;
              BITCODE_BS revision_minor2;
              BITCODE_RC revision_bytes[9];
              // no malloc, it is copied into the dwg. but it needs to be large
              // enough, pair->value.s might be smaller on corrupt DXF's. Also
              // we null-terminate it.
              char revision_guid[39];
              char *p = &revision_guid[0];
              unsigned u[3];
              if (!pair->value.s)
                {
                  LOG_ERROR ("Invalid %s.revision_guid %s", obj->name,
                             revision_guid);
                  break;
                }
              // "{00000100-0100-00CA-D300-80010A7B10C3}"
              strncpy (revision_guid, pair->value.s, 38);
              revision_guid[38] = '\0';
              if (!dwg_dynapi_entity_set_value (
                      _obj, obj->name, "revision_guid[39]", revision_guid, 0))
                break;
              if (revision_guid[0] != '{' ||  /* 8 */
                  revision_guid[9] != '-' ||  /* 4 */
                  revision_guid[14] != '-' || /* 4 */
                  revision_guid[19] != '-' || /* 4 */
                  revision_guid[24] != '-' || /* 12 */
                  revision_guid[37] != '}')
                {
                  LOG_ERROR ("Invalid %s.revision_guid %s", obj->name,
                             revision_guid);
                  break;
                }
              sscanf (p, "{%8" PRIx32 "-%4X-%4X-%4X-", &revision_major, &u[0],
                      &u[1], &u[2]);
              revision_minor1 = (BITCODE_BS)u[0];
              revision_minor2 = (BITCODE_BS)u[1];
              p += 20;
              sscanf (p, SCANF_2X, &revision_bytes[0]);
              p += 2;
              sscanf (p, SCANF_2X, &revision_bytes[1]);
              p += 3;
              for (int _i = 2; _i < 8; _i++)
                {
                  sscanf (p, SCANF_2X, &revision_bytes[_i]);
                  p += 2;
                }
              // insist on an ending 0 byte, even if never used.
              revision_bytes[8] = '\0';
              dwg_dynapi_entity_set_value (_obj, obj->name, "revision_major",
                                           &revision_major, 0);
              dwg_dynapi_entity_set_value (_obj, obj->name, "revision_minor1",
                                           &revision_minor1, 0);
              dwg_dynapi_entity_set_value (_obj, obj->name, "revision_minor2",
                                           &revision_minor2, 0);
              dwg_dynapi_entity_set_value (
                  _obj, obj->name, "revision_bytes[9]", revision_bytes, 0);
              break;
            }
          // fall through
        case 70:
          if (ctrl_id && pair->code == 70)
            {
              BITCODE_B bit;
              ;
              flag = pair->value.i | 64;
              dwg_dynapi_entity_set_value (_obj, obj->name, "flag", &flag, 1);
              LOG_TRACE ("%s.flag = %d [RC 70]\n", name, pair->value.i);
              if (obj->fixedtype == DWG_TYPE_STYLE)
                {

#  define SET_CTRL_BIT(b, bnam)                                               \
    bit = flag & b ? 1 : 0;                                                   \
    if (bit)                                                                  \
      {                                                                       \
        dwg_dynapi_entity_set_value (_obj, obj->name, #bnam, &bit, 1);        \
        LOG_TRACE ("%s.%s = %d [B]\n", name, #bnam, bit);                     \
      }

                  SET_CTRL_BIT (1, is_vertical);
                  SET_CTRL_BIT (4, is_shape);
                }
              else if (obj->fixedtype == DWG_TYPE_LAYER)
                {
                  SINCE (R_2000b)
                  {
                    SET_CTRL_BIT (1, frozen);
                    SET_CTRL_BIT (2, off);
                    SET_CTRL_BIT (4, frozen_in_new);
                    SET_CTRL_BIT (8, locked);
                    SET_CTRL_BIT (32768, plotflag);
                  }
                  else
                  {
                    SET_CTRL_BIT (1, frozen);
                    SET_CTRL_BIT (2, frozen_in_new);
                    SET_CTRL_BIT (4, locked);
                  }
                }
              else if (obj->fixedtype == DWG_TYPE_BLOCK_HEADER)
                {
                  SET_CTRL_BIT (1, anonymous);
                  SET_CTRL_BIT (2, hasattrs);
                  SET_CTRL_BIT (4, blkisxref);
                  SET_CTRL_BIT (8, xrefoverlaid);
                  SET_CTRL_BIT (32, loaded_bit);
                }
              else if (obj->fixedtype == DWG_TYPE_VIEW)
                {
                  SET_CTRL_BIT (1, is_pspace);
                }
              else if (obj->fixedtype == DWG_TYPE_DIMSTYLE)
                {
                  SET_CTRL_BIT (1, flag0);
                }
              else if (obj->fixedtype == DWG_TYPE_VX_TABLE_RECORD)
                {
                  // also set via 290
                  SET_CTRL_BIT (2, is_on);
                }
              break;
            }
          else if (pair->code == 70 && obj->fixedtype == DWG_TYPE_LAYOUT)
            {
              Dwg_Object_LAYOUT *o = obj->tio.object->tio.LAYOUT;
              if (strEQc (subclass,
                          "AcDbPlotSettings")) // todo: embedded struct
                {
                  o->plotsettings.plot_flags = pair->value.i;
                  LOG_TRACE ("LAYOUT.plotsettings.plot_flags = 0x%x [BSx 70]",
                             pair->value.i);
                }
              else if (strEQc (subclass, "AcDbLayout"))
                {
                  o->layout_flags = pair->value.u;
                  LOG_TRACE ("LAYOUT.layout_flags = 0x%x [BSx 70]",
                             pair->value.u);
                }
              else
                {
                  LOG_WARN ("Unhandled LAYOUT.70 in subclass %s", subclass);
                  o->layout_flags = pair->value.u;
                  LOG_TRACE ("LAYOUT.layout_flags = 0x%x [BSx 70]",
                             pair->value.u);
                }
              break;
            }
          else if (pair->code == 70 && obj->fixedtype == DWG_TYPE_LWPOLYLINE)
            {
              Dwg_Entity_LWPOLYLINE *o = obj->tio.entity->tio.LWPOLYLINE;
              o->flag = pair->value.i;
              // 1 => 512 closed
              // 128: plinegen
              if (o->flag & 1)
                o->flag = (o->flag - 1) + 512;
              LOG_TRACE ("LWPOLYLINE.flag => %d [BS 70]\n", flag);
              break;
            }
          else if (pair->code == 70
                   && strEQc (subclass, "AcDbModelDocViewStyle"))
            {
              BITCODE_BS ver = pair->value.i;
              LOG_TRACE ("%s.mdoc_class_version = %d [BS %d]\n", name,
                         pair->value.i, pair->code);
              dwg_dynapi_entity_set_value (_obj, obj->name,
                                           "mdoc_class_version", &ver, is_tu);
              break;
            }
          else if (pair->code == 70
                   && (strEQc (subclass, "AcDbSectionViewStyle")
                       || strEQc (subclass, "AcDbDetailViewStyle")))
            {
              BITCODE_BS ver = pair->value.i;
              LOG_TRACE ("%s.class_version = %d [BS %d]\n", name,
                         pair->value.i, pair->code);
              dwg_dynapi_entity_set_value (_obj, obj->name, "class_version",
                                           &ver, is_tu);
              break;
            }
          else if (pair->code == 70
                   && obj->fixedtype == DWG_TYPE_DIMENSION_ANG2LN)
            {
              Dwg_Entity_DIMENSION_ANG2LN *o
                  = obj->tio.entity->tio.DIMENSION_ANG2LN;
              o->flag = o->flag1 = pair->value.i;
              LOG_TRACE ("DIMENSION.flag = %d [RC 70]\n", pair->value.i);
              o->flag1 &= 0xE0; /* clear the upper flag bits, and fix them: */
              o->flag1 = (o->flag1 & 0x80) ? o->flag1 & 0x7F : o->flag1 | 1;
              o->flag1 = (o->flag1 & 0x20) ? o->flag1 | 2 : o->flag1 & 0xDF;
              LOG_TRACE ("DIMENSION.flag1 => %d [RC]\n", o->flag1);
              // Skip this flag logic, it is unreliable. Detecting subclasses
              // is far better.
              switch (o->flag & 31)
                {
                case 0: // rotated, horizontal or vertical
                  LOG_TRACE ("Looks like %s\n", "DIMENSION_LINEAR");
                  // UPGRADE_ENTITY (DIMENSION_ANG2LN, DIMENSION_LINEAR);
                  break;
                case 1:
                  LOG_TRACE ("Looks like %s\n", "DIMENSION_ALIGNED");
                  // UPGRADE_ENTITY (DIMENSION_ANG2LN, DIMENSION_ALIGNED);
                  break;
                case 2: // already?
                  LOG_TRACE ("Looks like %s\n", "DIMENSION_ANG2LN");
                  // UPGRADE_ENTITY (DIMENSION_ANG2LN, DIMENSION_ANG2LN);
                  break;
                case 3:
                  LOG_TRACE ("Looks like %s\n", "DIMENSION_DIAMETER");
                  // UPGRADE_ENTITY (DIMENSION_ANG2LN, DIMENSION_DIAMETER);
                  break;
                case 4:
                  LOG_TRACE ("Looks like %s\n", "DIMENSION_RADIUS");
                  // UPGRADE_ENTITY (DIMENSION_ANG2LN, DIMENSION_RADIUS);
                  break;
                case 5:
                  LOG_TRACE ("Looks like %s\n", "DIMENSION_ANG3PT");
                  // UPGRADE_ENTITY (DIMENSION_ANG2LN, DIMENSION_ANG3PT);
                  break;
                case 6:
                  LOG_TRACE ("Looks like DIMENSION_LINEAR\n");
                  // UPGRADE_ENTITY (DIMENSION_ANG2LN, DIMENSION_ORDINATE);
                  break;
                default:
                  LOG_ERROR ("Invalid DIMENSION.flag %d", o->flag & 31);
                  error |= DWG_ERR_INVALIDTYPE;
                  break;
                }
              break;
            }
          // fall through
        case 420: // color.rgb's
        case 421:
        case 422:
        case 423:
        case 424:
        case 425:
        case 426:
        case 427:
        case 428:
          // note that there is a DIMSTYLE.DIMTFILL with rgb 428. so far I only
          // found index 70, not rgb 428
          if (pair->code >= 420 && pair->code <= 428)
            {
              const char *fname = NULL;
              if (pair->code == 420 && strEQc (name, "LAYER"))
                fname = "color";
              else if (pair->code == 421 && strEQc (name, "LIGHT"))
                fname = "color";
              else if (pair->code == 421
                       && (strEQc (name, "VPORT") || strEQc (name, "VIEWPORT")
                           || strEQc (name, "VIEW")))
                fname = "ambient_color";
              else if (pair->code == 421 && strEQc (name, "MTEXT"))
                fname = "bg_fill_color";
              else if (pair->code == 420 && strEQc (name, "MLINESTYLE"))
                // TODO or lines[].color
                fname = "fill_color";
              else if (pair->code == 420 && strEQc (name, "VISUALSTYLE"))
                break; // ignore. always index 5
              else if (pair->code == 421 && strEQc (name, "VISUALSTYLE"))
                fname = "face_mono_color";
              else if (pair->code == 422 && strEQc (name, "VISUALSTYLE"))
                fname = "edge_intersection_color";
              else if (pair->code == 423 && strEQc (name, "VISUALSTYLE"))
                fname = "edge_obscured_color";
              else if (pair->code == 424 && strEQc (name, "VISUALSTYLE"))
                fname = "edge_color";
              else if (pair->code == 425 && strEQc (name, "VISUALSTYLE"))
                fname = "edge_silhouette_color";
              else if (pair->code == 428 && strEQc (name, "DIMSTYLE"))
                fname = "DIMTFILL";
              else if (strEQc (name, "TABLE"))
                {
                  BITCODE_BL table_flag
                      = obj->tio.entity->tio.TABLE->table_flag_override;
                  BITCODE_BL border_color = obj->tio.entity->tio.TABLE
                                                ->border_color_overrides_flag;
                  if (pair->code == 421)
                    {
                      if (table_flag & 0x0800)
                        fname = "title_row_fill_color";
                      else if (table_flag & 0x01000)
                        fname = "header_row_fill_color";
                      else if (table_flag & 0x02000)
                        fname = "data_row_fill_color";
                      else if (border_color & 0x0008)
                        fname = "title_vert_left_color";
                      else if (border_color & 0x0200)
                        fname = "header_vert_left_color";
                      else if (border_color & 0x8000)
                        fname = "data_vert_left_color";
                    }
                  else if (pair->code == 422)
                    {
                      if (table_flag & 0x0020)
                        fname = "title_row_color";
                      else if (table_flag & 0x0040)
                        fname = "header_row_color";
                      else if (table_flag & 0x0080)
                        fname = "data_row_color";
                      else if (border_color & 0x0001)
                        fname = "title_horiz_top_color";
                      else if (border_color & 0x0040)
                        fname = "header_horiz_top_color";
                      else if (border_color & 0x1000)
                        fname = "data_horiz_top_color";
                    }
                  else if (pair->code == 423)
                    {
                      if (border_color & 0x0002)
                        fname = "title_horiz_ins_color";
                      else if (border_color & 0x0080)
                        fname = "header_horiz_ins_color";
                      else if (border_color & 0x2000)
                        fname = "data_horiz_ins_color";
                    }
                  else if (pair->code == 424)
                    {
                      if (border_color & 0x0004)
                        fname = "title_horiz_bottom_color";
                      else if (border_color & 0x0100)
                        fname = "header_horiz_bottom_color";
                      else if (border_color & 0x4000)
                        fname = "data_horiz_bottom_color";
                    }
                  else if (pair->code == 426)
                    {
                      if (border_color & 0x0010)
                        fname = "title_vert_ins_color";
                      else if (border_color & 0x0400)
                        fname = "header_vert_ins_color";
                      else if (border_color & 0x10000)
                        fname = "data_vert_ins_color";
                    }
                  else if (pair->code == 427)
                    {
                      if (border_color & 0x0020)
                        fname = "title_vert_right_color";
                      else if (border_color & 0x0800)
                        fname = "header_vert_right_color";
                      else if (border_color & 0x20000)
                        fname = "data_vert_right_color";
                    }
                }

              if (fname)
                {
                  BITCODE_CMC color;
                  dwg_dynapi_entity_value (_obj, obj->name, fname, &color,
                                           NULL);
                  color.method = 0xc2;
                  color.rgb = pair->value.l;
                  color.rgb |= 0xc2000000;
                  LOG_TRACE ("%s.%s.rgb = %08X [CMC %d]\n", name, fname,
                             color.rgb, pair->code);
                  dwg_dynapi_entity_set_value (_obj, obj->name, fname, &color,
                                               is_tu);
                  break;
                }
            }
          // fall through
        default:
        object_default:
          if (pair->code >= 1000 && pair->code < 1999)
            add_eed (obj, obj->name, pair);
          else if (pair->code != 280 && strEQc (name, "XRECORD"))
            pair = add_xdata (dat, obj, pair);
          else if (pair->code == 310 && strEQc (obj->name, "BLOCK_HEADER"))
            {
              pair = add_block_preview (obj, dat, pair);
              goto start_loop;
            }
          else if (pair->code == 90 && obj->fixedtype == DWG_TYPE_OLE2FRAME)
            {
              Dwg_Entity_OLE2FRAME *o = obj->tio.entity->tio.OLE2FRAME;
              o->data_size = pair->value.l;
              o->data = (BITCODE_RC *)xcalloc (pair->value.l, 1);
              if (!o->data)
                {
                  o->data_size = 0;
                  goto invalid_dxf;
                }
              LOG_TRACE ("OLE2FRAME.data_size = %ld [BL 90]\n", pair->value.l);
            }
          else if (pair->code == 90 && obj->fixedtype == DWG_TYPE_PERSUBENTMGR)
            {
              pair = add_PERSUBENTMGR (obj, dat, pair); // NULL for success
              if (!pair)
                goto next_pair;
              else
                goto start_loop; /* failure */
            }
          else if ((pair->code == 71 || pair->code == 75 || pair->code == 1)
                   && obj->fixedtype == DWG_TYPE_DIMASSOC)
            {
              pair = add_DIMASSOC (obj, dat, pair);
              // returns with 0
              if (pair != NULL && pair->code == 0)
                goto start_loop;
              else
                goto search_field;
            }
          else if ((pair->code == 8 || pair->code == 90)
                   && obj->fixedtype == DWG_TYPE_LAYER_INDEX)
            {
              pair = add_LAYER_entry (obj, dat, pair);
              // returns with 0
              if (pair != NULL && pair->code == 0)
                goto start_loop;
              else
                goto search_field;
            }
          else if (pair->code == 91
                   && obj->fixedtype == DWG_TYPE_EVALUATION_GRAPH)
            {
              pair = add_EVAL_Node (obj, dat, pair);
              // returns with 0
              if (pair != NULL && pair->code == 0)
                goto start_loop;
              else
                goto search_field;
            }
          else if (pair->code == 310 && obj->fixedtype == DWG_TYPE_OLE2FRAME)
            {
              Dwg_Entity_OLE2FRAME *o = obj->tio.entity->tio.OLE2FRAME;
              size_t len = strlen (pair->value.s);
              size_t blen = len / 2;
              size_t read;
              // const char *pos = pair->value.s;
              unsigned char *s = (unsigned char *)&o->data[written];
              if (!o->data)
                {
                  written = 0;
                  o->data = (unsigned char *)xcalloc (blen, 1);
                  s = o->data;
                }
              assert (o->data);
              if (blen + written > o->data_size)
                {
                  LOG_ERROR ("OLE2FRAME.data overflow: %" PRIuSIZE
                             " + written %" PRIuSIZE " > "
                             "data_size: %u",
                             blen, written, o->data_size);
                  goto invalid_dxf;
                }
              if ((read = in_hex2bin (s, pair->value.s, blen) != blen))
                LOG_ERROR ("in_hex2bin read only %" PRIuSIZE " of %" PRIuSIZE,
                           read, blen);
              written += read;
              LOG_TRACE ("OLE2FRAME.data += %" PRIuSIZE " (%" PRIuSIZE
                         "/%u) [TF 310]\n",
                         blen, written, o->data_size);
            }
          else if (pair->code == 1
                   && ((strEQc (name, "_3DSOLID") || strEQc (name, "BODY")
                        || strEQc (name, "REGION"))
                       || strEQc (subclass, "AcDbModelerGeometry")))
            {
              j = 0;
              k = 0;
              // check if the object is valid
              if (!dwg_obj_is_3dsolid (obj))
                {
                  LOG_ERROR ("%s not a 3DSOLID", name);
                  goto invalid_dxf;
                }
              pair = add_3DSOLID_encr (obj, dat, pair);
              goto start_loop;
            }
          else if (pair->code == 1 && obj->fixedtype == DWG_TYPE_LAYOUT)
            {
              Dwg_Object_LAYOUT *o = obj->tio.object->tio.LAYOUT;
              if (strEQc (subclass, "AcDbPlotSettings"))
                {
                  const Dwg_DYNAPI_field *f = dwg_dynapi_entity_field (
                      "PLOTSETTINGS", "printer_cfg_file");
                  dwg_dynapi_field_set_value (dwg, &o->plotsettings, f,
                                              &pair->value, 1);
                  LOG_TRACE ("%s.plotsettings.printer_cfg_file = %s [T 1]\n",
                             obj->name, pair->value.s);
                }
              else if (strEQc (subclass, "AcDbLayout"))
                {
                  dwg_dynapi_entity_set_value (_obj, obj->name, "layout_name",
                                               &pair->value, 1);
                  LOG_TRACE ("%s.layout_name = %s [T 1]\n", obj->name,
                             pair->value.s);
                }
              else
                LOG_WARN ("Unhandled LAYOUT.1 in subclass %s", subclass);
              goto next_pair;
            }
          else if (pair->code == 3 && obj->fixedtype == DWG_TYPE_MTEXT)
            {
              Dwg_Entity_MTEXT *o = obj->tio.entity->tio.MTEXT;
              size_t len = strlen (pair->value.s);
              if (!o->text)
                {
                  o->text = strdup (pair->value.s);
                  written = len;
                  LOG_TRACE ("MTEXT.text = %s (%" PRIuSIZE ") [TV 3]\n",
                             pair->value.s, len);
                }
              else
                {
                  assert (o->text);
                  if (strlen (o->text) < len)
                    o->text = (char *)realloc (o->text, len + 1);
                  strcpy (o->text, pair->value.s);
                  written += len;
                  LOG_TRACE ("MTEXT.text += %" PRIuSIZE "/%" PRIuSIZE
                             " [TV 3]\n",
                             len, written);
                }
            }
          /*
          else if (pair->code == 2 && obj->fixedtype == DWG_TYPE_LAYOUT)
            {
              Dwg_Object_LAYOUT *o = obj->tio.object->tio.LAYOUT;
              const Dwg_DYNAPI_field *f = dwg_dynapi_entity_field
          ("PLOTSETTINGS", "paper_size"); dwg_dynapi_field_set_value (dwg,
          &o->plotsettings, f, &pair->value, 1); LOG_TRACE
          ("%s.plotsettings.paper_size = %s [T 2]\n", obj->name,
          pair->value.s); goto next_pair;
            }
          */
          else if (pair->code == 370 && obj->fixedtype == DWG_TYPE_LAYER)
            {
              Dwg_Object_LAYER *layer = obj->tio.object->tio.LAYER;
              layer->linewt = dxf_find_lweight ((int16_t)pair->value.i);
              LOG_TRACE ("LAYER.linewt = %d\n", layer->linewt);
              layer->flag |= layer->linewt << 5;
              LOG_TRACE ("LAYER.flag = 0x%x [BS 70]\n", layer->flag);
              goto next_pair;
            }
          else if (pair->code == 370 && obj->supertype == DWG_SUPERTYPE_ENTITY
                   && strEQc (subclass, "AcDbEntity"))
            {
              BITCODE_RC linewt = dxf_find_lweight ((int16_t)pair->value.i);
              dwg_dynapi_common_set_value (_obj, "linewt", &linewt, 0);
              LOG_TRACE ("COMMON.linewt => %d [RC 370]\n", linewt);
              goto next_pair;
            }
          else if (pair->code == 48 && obj->supertype == DWG_SUPERTYPE_ENTITY
                   && strEQc (subclass, "AcDbEntity"))
            {
              dwg_dynapi_common_set_value (_obj, "ltype_scale", &pair->value.d,
                                           0);
              LOG_TRACE ("COMMON.ltype_scale = %f [BD 48]\n", pair->value.d);
              goto next_pair;
            }
          else if (pair->code == 390 && obj->supertype == DWG_SUPERTYPE_ENTITY
                   && strEQc (subclass, "AcDbEntity"))
            {
              BITCODE_BB flags = 3;
              BITCODE_H hdl = dwg_add_handleref (dwg, 5, pair->value.u, obj);
              dwg_dynapi_common_set_value (_obj, "plotstyle", &hdl, 0);
              dwg_dynapi_common_set_value (_obj, "plotstyle_flags", &flags, 0);
              LOG_TRACE ("COMMON.plotstyle => " FORMAT_REF " [H 390]\n",
                         ARGS_REF (hdl));
              goto next_pair;
            }
          else if (pair->code == 347 && obj->supertype == DWG_SUPERTYPE_ENTITY
                   && strEQc (subclass, "AcDbEntity"))
            {
              BITCODE_BB flags = 3;
              BITCODE_H hdl = dwg_add_handleref (dwg, 5, pair->value.u, obj);
              dwg_dynapi_common_set_value (_obj, "material", &hdl, 0);
              dwg_dynapi_common_set_value (_obj, "material_flags", &flags, 0);
              LOG_TRACE ("COMMON.material => " FORMAT_REF " [H 390]\n",
                         ARGS_REF (hdl));
              goto next_pair;
            }
          else if (pair->code == 348 && obj->supertype == DWG_SUPERTYPE_ENTITY
                   && strEQc (subclass, "AcDbEntity"))
            {
              BITCODE_H hdl = dwg_add_handleref (dwg, 5, pair->value.u, obj);
              // FIXME: which of the 3 visualstyle types? full, face, or edge
              dwg_dynapi_common_set_value (_obj, "full_visualstyle", &hdl, 0);
              LOG_TRACE ("COMMON.full_visualstyle => " FORMAT_REF " [H 348]\n",
                         ARGS_REF (hdl));
              goto next_pair;
            }
          else if (pair->code == 49 && obj->fixedtype == DWG_TYPE_LTYPE)
            {
              pair = add_LTYPE_dashes (obj, dat, pair);
              if (pair != NULL && pair->code == 0)
                return pair;
              goto next_pair;
            }
          else if (pair->code == 71 && obj->fixedtype == DWG_TYPE_MLINESTYLE
                   && pair->value.i != 0)
            {
              pair = add_MLINESTYLE_lines (obj, dat, pair);
              if (pair != NULL && pair->code == 0)
                return pair;
              goto next_pair;
            }
          else if (pair->code == 74 && obj->fixedtype == DWG_TYPE_VPORT)
            {
              Dwg_Object_VPORT *o = obj->tio.object->tio.VPORT;
              BITCODE_RC ucsicon = pair->value.i;
              if (ucsicon == 1) // 1 and 2 are swapped in DXF
                ucsicon = 2;
              else if (ucsicon == 2)
                ucsicon = 1;
              o->UCSICON = ucsicon;
              LOG_TRACE ("VPORT.UCSICON = %d [BB 74]\n", o->UCSICON)
              goto next_pair;
            }
          else if (pair->code == 65 && obj->fixedtype == DWG_TYPE_VPORT)
            {
              Dwg_Object_VPORT *o = obj->tio.object->tio.VPORT;
              o->UCSVP = pair->value.i;
              o->UCSFOLLOW = o->VIEWMODE & 4 ? 1 : 0;
              o->VIEWMODE |= o->UCSVP;
              LOG_TRACE ("VPORT.UCSVP = %d [B 65]\n", o->UCSVP)
              LOG_TRACE ("VPORT.UCSFOLLOW => %d [B 0] (calc)\n", o->UCSFOLLOW)
              LOG_TRACE ("VPORT.VIEWMODE => %d [4BITS 71] (calc)\n",
                         o->VIEWMODE)
              goto next_pair;
            }
          else if (pair->code == 90 && obj->fixedtype == DWG_TYPE_LWPOLYLINE)
            {
              pair = new_LWPOLYLINE (obj, dat, pair);
              if (pair != NULL && pair->code == 0)
                return pair;
              goto next_pair;
            }
          else if (pair->code == 350 && strEQc (subclass, "AcDb3dSolid")
                   && dwg_obj_is_3dsolid (obj))
            {
              Dwg_Entity__3DSOLID *o = obj->tio.entity->tio._3DSOLID;
              BITCODE_H hdl = dwg_add_handleref (dwg, 5, pair->value.u, obj);
              LOG_TRACE ("%s.history_id = " FORMAT_REF " [H 350]\n", obj->name,
                         ARGS_REF (hdl));
              o->history_id = hdl;
            }
          else if (obj->fixedtype == DWG_TYPE_MLINE)
            {
              int status = add_MLINE (obj, dat, pair, &j, &k, &l);
              if (status == 0)
                goto search_field;
              else if (status == 2)
                break;
            }
          else if (strEQc (name, "VERTEX_PFACE_FACE") && pair->code >= 71
                   && pair->code <= 74)
            {
              Dwg_Entity_VERTEX_PFACE_FACE *o
                  = (Dwg_Entity_VERTEX_PFACE_FACE *)_obj;
              j = pair->code - 71;
              o->vertind[j] = pair->value.i;
              LOG_TRACE ("VERTEX_PFACE_FACE.vertind[%d] = %d [BS %d]\n", j,
                         pair->value.i, pair->code);
            }
          else if (obj->fixedtype == DWG_TYPE_SPLINE)
            {
              if (pair->code == 210 || pair->code == 220 || pair->code == 230)
                break; // ignore extrusion in the dwg (planar only)
              if (add_SPLINE (obj->tio.entity->tio.SPLINE, dat, pair, &j,
                              &flag))
                goto next_pair;
              else
                goto search_field;
            }
          else if (obj->fixedtype == DWG_TYPE_HATCH)
            {
              if ((pair->code == 10 || pair->code == 20)
                  && pair->value.d == 0.0)
                break; // elevation
              else if (pair->code == 91 || pair->code == 78
                       || pair->code == 453)
                {
                  pair = add_HATCH (obj, dat, pair);
                  if (!pair || pair->code == 0) // end or unknown
                    return pair;
                  goto search_field;
                }
              else
                goto search_field;
            }
          else if (is_textlike (obj))
            {
              BITCODE_RC dataflags;
              if (pair->code == 10 || pair->code == 20)
                goto search_field;
              else if (pair->code == 2
                       && (obj->fixedtype == DWG_TYPE_ATTRIB
                           || obj->fixedtype == DWG_TYPE_ATTDEF)
                       && !dwg_is_valid_tag (pair->value.s))
                {
                  LOG_ERROR ("Invalid %s.tag %s\n", obj->name, pair->value.s);
                }
              else if (pair->code == 30 && pair->value.d == 0.0)
                {
                  dwg_dynapi_entity_value (_obj, obj->name, "dataflags",
                                           &dataflags, NULL);
                  dataflags |= 1;
                  LOG_TRACE ("%s.elevation 0.0 => dataflags = 0x%x\n",
                             obj->name, dataflags);
                  dwg_dynapi_entity_set_value (_obj, obj->name, "dataflags",
                                               &dataflags, 0);
                }
              else
                goto search_field;
            }
          else if (obj->fixedtype == DWG_TYPE_MESH)
            {
              if (pair->code == 91)
                {
                  pair = add_MESH (obj, dat, pair);
                  if (!pair || pair->code == 0) // end or unknown
                    return pair;
                  goto search_field;
                }
              else
                goto search_field;
            }
          else if (obj->fixedtype == DWG_TYPE_MLEADERSTYLE)
            {
              Dwg_Object_MLEADERSTYLE *o = obj->tio.object->tio.MLEADERSTYLE;
              if (pair->code == 47)
                o->block_scale.x = pair->value.d;
              else if (pair->code == 49)
                o->block_scale.y = pair->value.d;
              else if (pair->code == 140)
                {
                  o->block_scale.z = pair->value.d;
                  LOG_TRACE (
                      "MLEADERSTYLE.block_scale = (%f, %f, %f) [3BD 47]\n",
                      o->block_scale.x, o->block_scale.y, o->block_scale.z);
                }
              else if (pair->code == 297 && o->class_version < 2)
                {
                  LOG_TRACE ("MLEADERSTYLE.text_always_left = %d [B 297] => "
                             "class_version 2\n",
                             pair->value.i);
                  o->text_always_left = pair->value.i;
                  o->class_version = 2;
                }
              else
                goto search_field;
            }
          else if (obj->fixedtype == DWG_TYPE_MTEXT && pair->code == 46)
            {
              Dwg_Entity_MTEXT *o = obj->tio.entity->tio.MTEXT;
              if (!o->num_column_heights)
                o->num_column_heights = 1;
              if (!j)
                {
                  o->column_heights = (BITCODE_BD *)xcalloc (
                      o->num_column_heights, sizeof (BITCODE_BD));
                  if (!o->column_heights)
                    {
                      o->num_column_heights = 0;
                      goto invalid_dxf;
                    }
                }
              if (j < 0 || j >= (int)o->num_column_heights
                  || !o->column_heights)
                goto invalid_dxf;
              assert (j < (int)o->num_column_heights);
              o->column_heights[j] = pair->value.d;
              LOG_TRACE ("MTEXT.column_heights[%d] = %f [BD* 46]\n", j,
                         pair->value.d);
            }
          else if (obj->fixedtype == DWG_TYPE_GEODATA)
            {
              pair = add_GEODATA (obj, dat, pair);
              if (pair && pair->code != 0)
                goto search_field;
              else
                return pair;
            }
          else if (pair->code == 300 && obj->fixedtype == DWG_TYPE_CELLSTYLEMAP
                   && strEQc (pair->value.s, "CELLSTYLE"))
            {
              Dwg_Object_CELLSTYLEMAP *o = obj->tio.object->tio.CELLSTYLEMAP;
              cur_cell++;
              if (cur_cell < 0 || cur_cell >= (int)o->num_cells)
                goto invalid_dxf;
            }
          else if (pair->code == 1 && pair->value.s
                   && strEQc (pair->value.s, "TABLEFORMAT_BEGIN")
                   && (obj->fixedtype == DWG_TYPE_CELLSTYLEMAP
                       || obj->fixedtype == DWG_TYPE_TABLE
                       || obj->fixedtype == DWG_TYPE_TABLESTYLE
                       || obj->fixedtype == DWG_TYPE_TABLECONTENT))
            {
              Dwg_CellStyle *csty = NULL;
              Dwg_TABLESTYLE_CellStyle *tbl_sty = NULL;
              char key[80];
              if (obj->fixedtype == DWG_TYPE_CELLSTYLEMAP)
                {
                  Dwg_Object_CELLSTYLEMAP *o
                      = obj->tio.object->tio.CELLSTYLEMAP;
                  if (cur_cell < 0 || cur_cell >= (int)o->num_cells)
                    goto invalid_dxf;
                  if (cur_cell == 0 && !o->cells)
                    o->cells = (Dwg_TABLESTYLE_CellStyle *)xcalloc (
                        o->num_cells, sizeof (Dwg_TABLESTYLE_CellStyle));
                  if (!o->cells)
                    goto invalid_dxf;
                  tbl_sty = &o->cells[cur_cell];
                  sprintf (key, "cells[%d]", cur_cell);
                  csty = &tbl_sty->cellstyle;
                }
              else if (obj->fixedtype == DWG_TYPE_TABLESTYLE)
                {
                  // TODO ovr
                  Dwg_Object_TABLESTYLE *o = obj->tio.object->tio.TABLESTYLE;
                  tbl_sty = &o->sty;
                  csty = &o->sty.cellstyle;
                }
              if (csty)
                pair = add_CellStyle (obj, csty, &key[0], dat, pair);
              if (pair && pair->code != 0)
                {
                  if (tbl_sty && pair->code == 90)
                    {
                      tbl_sty->id = pair->value.u;
                      LOG_TRACE ("%s.%s.id = " FORMAT_BL " [BL %d]\n",
                                 obj->name, key, pair->value.u, pair->code);
                      dxf_free_pair (pair);
                      pair = dxf_read_pair (dat);
                    }
                  if (tbl_sty && pair && pair->code == 91)
                    {
                      tbl_sty->type = pair->value.u;
                      LOG_TRACE ("%s.%s.type = " FORMAT_BL " [BL %d]\n",
                                 obj->name, key, pair->value.u, pair->code);
                      dxf_free_pair (pair);
                      pair = dxf_read_pair (dat);
                    }
                  if (tbl_sty && pair && pair->code == 300)
                    {
                      tbl_sty->name = dwg_add_u8_input (dwg, pair->value.s);
                      LOG_TRACE ("%s.%s.name = \"%s\" [BL %d]\n", obj->name,
                                 key, pair->value.s, pair->code);
                      dxf_free_pair (pair);
                      pair = dxf_read_pair (dat);
                    }
                  if (tbl_sty && pair && pair->code == 309
                      && strEQc (pair->value.s, "CELLSTYLE_END"))
                    goto next_pair;
                  else
                    goto search_field;
                }
              else
                return pair;
            }
          else if (obj->fixedtype == DWG_TYPE_VPORT && pair->code == 41)
            {
              Dwg_Object_VPORT *o = obj->tio.object->tio.VPORT;
              o->aspect_ratio = pair->value.d;
              o->view_width = o->aspect_ratio * o->VIEWSIZE;
              LOG_TRACE ("VPORT.aspect_ratio = %f [BD 41]\n", o->aspect_ratio);
              LOG_TRACE ("VPORT.view_width = %f [BD 0]\n", o->view_width);
            }
          else if (strEQc (subclass, "AcDbShHistoryNode"))
            {
              // add_AcDbShHistoryNode (obj, _obj)
              Dwg_ACSH_HistoryNode *hn;
              const Dwg_DYNAPI_field *f1
                  = dwg_dynapi_entity_field (obj->name, "history_node");
              if (!f1)
                goto search_field;
              hn = (Dwg_ACSH_HistoryNode *)&((char *)_obj)[f1->offset];
              if (pair->code == 90)
                {
                  hn->major = pair->value.u;
                  LOG_TRACE ("%s.%s.%s = %u [BL %d]\n", name, "history_node",
                             "major", pair->value.u, pair->code);
                }
              else if (pair->code == 91)
                {
                  hn->minor = pair->value.u;
                  LOG_TRACE ("%s.%s.%s = %u [BL %d]\n", name, "history_node",
                             "minor", pair->value.u, pair->code);
                }
              else if (pair->code == 92)
                {
                  hn->step_id = pair->value.u;
                  LOG_TRACE ("%s.%s.%s = %u [BL %d]\n", name, "history_node",
                             "step_id", pair->value.u, pair->code);
                }
              else if (pair->code == 62)
                {
                hn_color:
                  hn->color.index = pair->value.l;
                  LOG_TRACE ("%s.%s.%s.index = %u [CMC %d]\n", name,
                             "history_node", "color", pair->value.u,
                             pair->code);
                }
              else if (pair->code == 347)
                {
                  hn->material
                      = dwg_add_handleref (dwg, 5, pair->value.u, obj);
                  LOG_TRACE ("%s.%s.%s = " FORMAT_REF " [H %d]\n", name,
                             "history_node", "material",
                             ARGS_REF (hn->material), pair->code);
                }
              else if (hn && pair->code == 40) // VECTOR_N1
                {
                  hn->trans = (BITCODE_BD *)xcalloc (16, sizeof (BITCODE_BD));
                  if (!hn->trans)
                    return NULL;
                  // BD* starting at 40-55
                  for (j = 0; j < 16; j++)
                    {
                      hn->trans[j] = pair->value.d;
                      LOG_TRACE ("%s.history_node.trans[%d] = %f [BD %d]\n",
                                 obj->name, j, pair->value.d, j + 40);
                      dxf_free_pair (pair);
                      pair = dxf_read_pair (dat);
                      if (!pair || pair->code == 0)
                        return pair;
                      if (pair->code == 62)
                        goto hn_color;
                      if (pair->code != j + 41)
                        goto search_field;
                    }
                }
            }
          else if (dwg_obj_is_acsh (obj) && memBEGINc (subclass, "AcDbSh")
                   && (pair->code == 90 || pair->code == 91))
            {
              const char *_key = pair->code == 90 ? "major" : "minor";
              const Dwg_DYNAPI_field *f1
                  = dwg_dynapi_entity_field (obj->name, _key);
              if (!f1)
                goto search_field;
              dwg_dynapi_field_set_value (dwg, _obj, f1, &pair->value, 0);
              LOG_TRACE ("%s.%s = %u [BL %d]\n", name, _key, pair->value.u,
                         pair->code);
            }
          else if (strEQc (subclass, "AcDbEvalExpr"))
            {
              pair = add_AcDbEvalExpr (obj, (char *)_obj, dat, pair);
              if (pair && pair->code == 100) // success
                goto start_loop;
              else
                goto search_field;
            }
          else if (obj->fixedtype == DWG_TYPE_DIMSTYLE && pair->code == 295)
            { // ODA has this documented as 205, netDXF as 294
              Dwg_Object_DIMSTYLE *o = obj->tio.object->tio.DIMSTYLE;
              o->DIMTXTDIRECTION = pair->value.i & 1;
              LOG_TRACE ("%s.DIMTXTDIRECTION = %u [B %d]\n", name,
                         pair->value.u, pair->code);
            }
          else if (obj->fixedtype == DWG_TYPE_LEADER
                   && (pair->code == 10 || pair->code == 20
                       || pair->code == 30))
            {
              Dwg_Entity_LEADER *o = obj->tio.entity->tio.LEADER;
              if (!j && pair->code == 10)
                {
                  o->points = (BITCODE_3BD *)xcalloc (o->num_points,
                                                      sizeof (BITCODE_3BD));
                  if (!o->points)
                    {
                      o->num_points = 0;
                      goto invalid_dxf;
                    }
                }
              if (j < 0 || j >= (int)o->num_points || !o->points)
                goto invalid_dxf;
              assert (j >= 0);
              assert (j < (int)o->num_points);
              assert (o->points);

              if (pair->code == 10)
                o->points[j].x = pair->value.d;
              else if (pair->code == 20)
                o->points[j].y = pair->value.d;
              else if (pair->code == 30)
                {
                  o->points[j].z = pair->value.d;
                  LOG_TRACE ("LEADER.points[%d] = (%f, %f, %f) [3BD* 10]\n", j,
                             o->points[j].x, o->points[j].y, o->points[j].z);
                  j++;
                }
            }
          else if (pair->code == 71
                   && strEQc (subclass, "AcDbSectionViewStyle"))
            {
              pair = add_AcDbSectionViewStyle (obj, dat);
              if (!pair) // success
                goto start_loop;
              else
                goto search_field;
            }
          else if (pair->code == 71 && strEQc (subclass, "AcDbDetailViewStyle")
                   && obj->fixedtype == DWG_TYPE_DETAILVIEWSTYLE)
            {
              pair = add_AcDbDetailViewStyle (obj, dat);
              if (pair && pair->code == 100) // success
                goto start_loop;
              else
                goto search_field;
            }
          else if (pair->code == 300 && strEQc (subclass, "AcDbBlockElement"))
            {
              pair = add_AcDbBlockElement (obj, (char *)_obj, dat, pair);
              if (!pair) // success
                goto start_loop;
              else
                goto search_field;
            }
          else if (pair->code == 91 && strEQc (subclass, "AcDbBlockgrip"))
            {
              pair = add_AcDbBlockGrip (obj, (char *)_obj, dat, pair);
              if (!pair) // success
                goto start_loop;
              else
                goto search_field;
            }
          else if (pair->code == 93
                   && strEQc (subclass, "AcDbBlockVisibilityParameter"))
            {
              pair = add_AcDbBlockVisibilityParameter (
                  obj, (Dwg_Object_BLOCKVISIBILITYPARAMETER *)_obj, dat, pair);
              if (!pair) // success
                goto start_loop;
              else
                goto search_field;
            }
          else if (pair->code == 307
                   && strEQc (subclass, "AcDbBlockLinearConstraintParameter"))
            {
              Dwg_BLOCKPARAMVALUESET *value_set
                  = &((Dwg_Object_BLOCKALIGNEDCONSTRAINTPARAMETER *)_obj)
                         ->value_set;
              pair = add_AcDbBlockParamValueSet (obj, value_set, dat, pair);
              if (!pair) // success
                goto start_loop;
              else
                goto search_field;
            }
          else if (pair->code == 307
                   && strEQc (obj->name, "BLOCKLINEARPARAMETER"))
            {
              Dwg_BLOCKPARAMVALUESET *value_set
                  = &((Dwg_Object_BLOCKLINEARPARAMETER *)_obj)->value_set;
              pair = add_AcDbBlockParamValueSet (obj, value_set, dat, pair);
              if (!pair) // success
                goto start_loop;
              else
                goto search_field;
            }
          else if (pair->code == 307
                   && strEQc (obj->name, "BLOCKROTATIONPARAMETER"))
            {
              Dwg_BLOCKPARAMVALUESET *value_set
                  = &((Dwg_Object_BLOCKROTATIONPARAMETER *)_obj)
                         ->angle_value_set;
              pair = add_AcDbBlockParamValueSet (obj, value_set, dat, pair);
              if (!pair) // success
                goto start_loop;
              else
                goto search_field;
            }
          else if (pair->code == 307
                   && strEQc (obj->name, "BLOCKANGULARCONSTRAINTPARAMETER"))
            {
              Dwg_BLOCKPARAMVALUESET *value_set
                  = &((Dwg_Object_BLOCKANGULARCONSTRAINTPARAMETER *)_obj)
                         ->value_set;
              pair = add_AcDbBlockParamValueSet (obj, value_set, dat, pair);
              if (!pair) // success
                goto start_loop;
              else
                goto search_field;
            }
          else if (pair->code == 307
                   && strEQc (obj->name, "BLOCKDIAMETRICCONSTRAINTPARAMETER"))
            {
              Dwg_BLOCKPARAMVALUESET *value_set
                  = &((Dwg_Object_BLOCKDIAMETRICCONSTRAINTPARAMETER *)_obj)
                         ->value_set;
              pair = add_AcDbBlockParamValueSet (obj, value_set, dat, pair);
              if (!pair) // success
                goto start_loop;
              else
                goto search_field;
            }
          else if (pair->code == 307
                   && strEQc (obj->name, "BLOCKRADIALCONSTRAINTPARAMETER"))
            {
              Dwg_BLOCKPARAMVALUESET *value_set
                  = &((Dwg_Object_BLOCKRADIALCONSTRAINTPARAMETER *)_obj)
                         ->value_set;
              pair = add_AcDbBlockParamValueSet (obj, value_set, dat, pair);
              if (!pair) // success
                goto start_loop;
              else
                goto search_field;
            }
          else if (pair->code == 309 && strEQc (obj->name, "BLOCKXYPARAMETER"))
            {
              Dwg_BLOCKPARAMVALUESET *value_set
                  = &((Dwg_Object_BLOCKXYPARAMETER *)_obj)->y_value_set;
              pair = add_AcDbBlockParamValueSet (obj, value_set, dat, pair);
              if (!pair) // success
                goto start_loop;
              else
                goto search_field;
            }
          else if (pair->code == 309
                   && strEQc (obj->name, "BLOCKPOLARPARAMETER"))
            {
              Dwg_BLOCKPARAMVALUESET *value_set
                  = &((Dwg_Object_BLOCKPOLARPARAMETER *)_obj)
                         ->distance_value_set;
              pair = add_AcDbBlockParamValueSet (obj, value_set, dat, pair);
              if (!pair) // success
                goto start_loop;
              else
                goto search_field;
            }
          else if (pair->code == 410 && strEQc (obj->name, "BLOCKXYPARAMETER"))
            {
              Dwg_BLOCKPARAMVALUESET *value_set
                  = &((Dwg_Object_BLOCKXYPARAMETER *)_obj)->x_value_set;
              pair = add_AcDbBlockParamValueSet (obj, value_set, dat, pair);
              if (!pair) // success
                goto start_loop;
              else
                goto search_field;
            }
          else if (pair->code == 410
                   && strEQc (obj->name, "BLOCKPOLARPARAMETER"))
            {
              Dwg_BLOCKPARAMVALUESET *value_set
                  = &((Dwg_Object_BLOCKPOLARPARAMETER *)_obj)->angle_value_set;
              pair = add_AcDbBlockParamValueSet (obj, value_set, dat, pair);
              if (!pair) // success
                goto start_loop;
              else
                goto search_field;
            }
          else if (pair->code == 8 && obj->fixedtype == DWG_TYPE_LAYERFILTER
                   && strEQc (subclass, "AcDbLayerFilter"))
            {
              // num_names
              Dwg_Object_LAYERFILTER *o = obj->tio.object->tio.LAYERFILTER;
              if (!o->names)
                {
                  o->names = (BITCODE_T *)xcalloc (1, sizeof (BITCODE_T));
                  j = 0;
                }
              else
                o->names = (BITCODE_T *)realloc (
                    o->names, (o->num_names + 1) * sizeof (BITCODE_T));
              if (!o->names || j < 0 || j >= (int)o->num_names)
                goto invalid_dxf;
              assert (j >= 0 && j < (int)o->num_names && o->names);
              o->names[j] = dwg_add_u8_input (dwg, pair->value.s);
              LOG_TRACE ("%s.%s[%d] = %s [%s %d]\n", name, "names", j,
                         pair->value.s, "T", pair->code);
              j++;
              o->num_names = j;
              goto next_pair;
            }
          else
          search_field:
            { // search all specific fields and common fields for the DXF
              const Dwg_DYNAPI_field *f;
              const Dwg_DYNAPI_field *fields
                  = dwg_dynapi_entity_fields (obj->name);
              if (!pair || pair->code == 0)
                break;
              if (!fields)
                {
                  LOG_ERROR ("Illegal object name %s, no dynapi fields",
                             obj->name);
                  break;
                }
              for (f = &fields[0]; f->name; f++)
                {
                  LOG_INSANE ("-%s.%s [%d %s] vs %d\n", obj->name, f->name,
                              f->dxf, f->type, pair->code)
                  // VECTORs. need to be malloced, and treated specially
                  if (pair->code != 3 && f->is_malloc && !f->is_string
                      && strNE (f->name, "parent")) // parent set in NEW_OBJECT
                    {
                      const Dwg_DYNAPI_field *num_f;

                      // FIELD_2RD_VECTOR (clip_verts, num_clip_verts, 11|14);
                      if ((pair->code == 91 || pair->code == 70)
                          && strEQc (f->name, "num_clip_verts"))
                        {
                          BITCODE_2RD *clip_verts;
                          BITCODE_BL num_clip_verts = pair->value.u;
                          if (pair->code == 91)
                            dwg_dynapi_entity_set_value (
                                _obj, obj->name, f->name, &num_clip_verts, 0);
                          else
                            {
                              BITCODE_BS num = pair->value.i; // 70
                              dwg_dynapi_entity_set_value (_obj, obj->name,
                                                           f->name, &num, 0);
                            }
                          dwg_dynapi_entity_value (_obj, obj->name,
                                                   "clip_verts", &clip_verts,
                                                   NULL);
                          clip_verts = (BITCODE_2RD *)realloc (
                              clip_verts,
                              num_clip_verts * sizeof (BITCODE_2RD));
                          if (!clip_verts)
                            goto invalid_dxf;
                          dwg_dynapi_entity_set_value (
                              _obj, obj->name, f->name, &clip_verts, 0);
                          LOG_TRACE ("%s.num_clip_verts = %d [%s %d]\n", name,
                                     num_clip_verts, f->type, pair->code);
                        }
                      else if (pair->code >= 10 && pair->code <= 24
                               && strEQc (f->name, "clip_verts")) // 11 or 14
                        {
                          BITCODE_BL num_clip_verts = 0;
                          BITCODE_2RD *clip_verts;
                          // 11 has no num_clip_verts: realloc. clip_inverts
                          // has.
                          if (pair->code == 14 || pair->code == 24)
                            {
                              // FIXME: num_clip_verts must match clip_verts[]
                              // But num_clip_verts can be set elsewhere,
                              // without reallocing the array.
                              dwg_dynapi_entity_value (_obj, obj->name,
                                                       "num_clip_verts",
                                                       &num_clip_verts, NULL);
                              LOG_INSANE ("%s.num_clip_verts = %d, j = %d\n",
                                          name, num_clip_verts, j);
                            }
                          if (!num_clip_verts && obj->fixedtype == DWG_TYPE_IMAGE)
                            num_clip_verts = 2;
                          dwg_dynapi_entity_value (_obj, obj->name,
                                                   "clip_verts", &clip_verts,
                                                   NULL);
                          // assert (j == 0 || j < (int)num_clip_verts);
                          if (pair->code < 20)
                            {
                              // no need to realloc
                              if (!j && pair->code == 14)
                                {
                                  clip_verts = (BITCODE_2RD *)xcalloc (
                                      num_clip_verts, sizeof (BITCODE_2RD));
                                  if (!clip_verts)
                                    goto invalid_dxf;
                                  dwg_dynapi_entity_set_value (_obj, obj->name,
                                                               f->name,
                                                               &clip_verts, 0);
                                }
                              else if (pair->code == 11)
                                {
                                  clip_verts = (BITCODE_2RD *)realloc (
                                      clip_verts,
                                      (j + 1) * sizeof (BITCODE_2RD));
                                  memset (&clip_verts[j], 0,
                                          sizeof (BITCODE_2RD));
                                  dwg_dynapi_entity_set_value (_obj, obj->name,
                                                               f->name,
                                                               &clip_verts, 0);
                                }
                              if (j >= 0 && j < (int)num_clip_verts
                                  && clip_verts)
                                clip_verts[j].x = pair->value.d;
                            }
                          else if (pair->code < 30)
                            {
                              if (j >= 0 && j < (int)num_clip_verts
                                  && clip_verts)
                                {
                                  clip_verts[j].y = pair->value.d;
                                  LOG_TRACE (
                                      "%s.%s[%d] = (%f, %f) [2RD* %d]\n", name,
                                      "clip_verts", j, clip_verts[j].x,
                                      clip_verts[j].y, pair->code - 10);
                                  j++;
                                }
                              if (pair->code == 21)
                                {
                                  dwg_dynapi_entity_set_value (
                                      _obj, obj->name, "num_clip_verts", &j,
                                      0);
                                  LOG_TRACE ("%s.num_clip_verts = %d\n", name,
                                             j);
                                }
                            }
                          goto next_pair;
                        }
                      // point vectors with known num_field (not 210)
                      else if ((*f->type == '2' || *f->type == '3')
                               && (f->type[2] == 'D'
                                   || strEQc (&f->type[1], "DPOINT*"))
                               && pair->code >= 10 && pair->code <= 34
                               && (num_f = find_numfield (fields, f->name)))
                        {
                          // how many points
                          long size = get_numfield_value (_obj, num_f);
                          double *pts;
                          int is2d = *f->type == '2';
                          if (!size)
                            {
                              LOG_TRACE ("Ignore empty %s.%s VECTOR [%s %d]\n",
                                         name, f->name, f->type, pair->code);
                              goto next_pair;
                            }
                          else if (j == 0 && pair->code < 20)
                            {
                              pts = (double *)xcalloc (size, is2d ? 16 : 24);
                              if (!pts)
                                return NULL;
                              LOG_TRACE ("%s.%s size: %ld\n", name, f->name,
                                         size);
                              pts[0] = pair->value.d;
                              dwg_dynapi_entity_set_value (_obj, obj->name,
                                                           f->name, &pts, 0);
                            }
                          else if (j > 0 && j < size)
                            {
                              int _i = is2d ? j * 2 : j * 3;
                              dwg_dynapi_entity_value (_obj, obj->name,
                                                       f->name, &pts, NULL);
                              if (pair->code < 20 && pts != NULL)
                                {
                                  pts[_i] = pair->value.d;
                                }
                              else if (pair->code < 30 && pts != NULL)
                                {
                                  if (is2d)
                                    LOG_TRACE (
                                        "%s.%s[%d] = (%f, %f) [%s %d]\n", name,
                                        f->name, j, pts[_i], pair->value.d,
                                        f->type, pair->code);
                                  pts[_i + 1] = pair->value.d;
                                }
                              else if (*f->type == '3' && pts)
                                {
                                  LOG_TRACE (
                                      "%s.%s[%d] = (%f, %f, %f) [%s %d]\n",
                                      name, f->name, j, pts[_i], pts[_i + 1],
                                      pair->value.d, f->type, pair->code);
                                  pts[_i + 2] = pair->value.d;
                                  if (j == size - 1)
                                    j = 0; // restart
                                }
                            }
                          else if (j > size)
                            LOG_ERROR ("%s.%s overflow %d > %ld", name,
                                       num_f->name, j, size)
                        }
                      else if (f->dxf == pair->code)
                        {
                          LOG_WARN ("Ignore %s.%s VECTOR [%s %d]", name,
                                    f->name, f->type, pair->code);
                          goto next_pair;
                        }
                    }
                  else if (obj->fixedtype == DWG_TYPE_VISUALSTYLE
                           && dwg->header.from_version >= R_2010
                           && pair->code == 176 && prev_vstyle)
                    {
                      // which 176 of the many? the one after the previous
                      // field
                      char fieldname[40];
                      strcpy (fieldname, prev_vstyle->name);
                      strcat (fieldname, "_int");
                      f = prev_vstyle + 1;
                      if (strEQc (fieldname, "display_brightness_bl_int"))
                        {
                          strcpy (fieldname, "display_brightness_int");
                          f++;
                        }
                      if (strEQ (fieldname, f->name))
                        {
                          LOG_HANDLE ("found %s.%s:\n", name, fieldname);
                          if (strEQc (f->name, "display_shadow_type_int")
                              && dwg->header.from_version >= R_2013)
                            {
                              Dwg_Object_VISUALSTYLE *o
                                  = obj->tio.object->tio.VISUALSTYLE;
                              o->display_shadow_type_int = pair->value.i;
                              pair = add_VISUALSTYLE_props (obj, dat);
                              if (!pair) // success
                                goto start_loop;
                              else // or better advance to the next 0
                                goto search_field;
                            }
                          else
                            goto matching_pair;
                        }
                      else
                        LOG_WARN ("%s.%s [BS 176] not found in dynapi", name,
                                  fieldname);
                    }
                  else if (f->dxf == pair->code) // matching DXF code
                    {
                    matching_pair:
                      if (obj->fixedtype == DWG_TYPE_VISUALSTYLE
                          && dwg->header.from_version >= R_2010
                          && pair->code != 176)
                        {
                          prev_vstyle = f;
                        }
                      // exceptions, where there's another field 92:
                      if (pair->code == 92 && is_entity
                          && dwg->header.from_version < R_2010
                          && strEQc (subclass, "AcDbEntity"))
                        // not MULTILEADER.text_color, nor MESH.num_vertex
                        {
                          pair = add_ent_preview (obj, dat, pair);
                          goto start_loop;
                        }
                      else if (pair->code == 3 && pair->value.s
                               && memBEGINc (obj->name, "DICTIONARY")
                               && strNE (obj->name, "DICTIONARYVAR"))
                        {
                          strncpy (text, pair->value.s, 254);
                          text[255] = '\0';
                          goto next_pair; // skip setting texts TV*
                        }
                      // convert angle to radians
                      else if (pair->code >= 50 && pair->code <= 55)
                        {
                          BITCODE_BD ang;
                          if (pair->value.d == 0.0)
                            goto next_pair;
                          ang = deg2rad (pair->value.d);
                          dwg_dynapi_entity_set_value (_obj, obj->name,
                                                       f->name, &ang, 0);
                          LOG_TRACE ("%s.%s = %f (from DEG %f°) [%s %d]\n",
                                     name, f->name, ang, pair->value.d,
                                     f->type, pair->code);
                          goto next_pair; // found
                        }
                      // convert double to text (e.g. ATEXT)
                      else if (strEQc (f->type, "D2T")
                               && pair->type == DWG_VT_REAL)
                        {
                          // TODO: for now we need to do double-conversion
                          // (str->dbl->str), because we don't have the initial
                          // dat->byte position.
                          char *d2t = (char *)xcalloc (128, 1);
                          snprintf (d2t, 128, "%f", pair->value.d);
                          dwg_dynapi_entity_set_value (_obj, obj->name,
                                                       f->name, &d2t, 1);
                          LOG_TRACE ("%s.%s = %s (from %f°) [%s %d]\n", name,
                                     f->name, d2t, pair->value.d, f->type,
                                     pair->code);
                          free (d2t);
                          goto next_pair; // found
                        }
                      // resolve handle, by name or ref
                      else if (strEQc (f->type, "H"))
                        {
                          BITCODE_H ref = find_tablehandle (dwg, pair);
                          if (!ref)
                            {
                              if (pair->code > 300)
                                {
                                  int code = 5; // default: soft pointer
                                  if (obj->fixedtype == DWG_TYPE_VIEWPORT)
                                    {
                                      switch (pair->code)
                                        {
                                        case 340:
                                        case 332:
                                        case 333:
                                          code = 4;
                                          break;
                                        case 361:
                                          code = 3;
                                          break;
                                        default:
                                          break;
                                        }
                                    }
                                  else if (strEQc (f->name, "history_id"))
                                    code = 4;
                                  else if (strEQc (f->name, "background"))
                                    code = 4;
                                  else if (strEQc (f->name, "dimensionobj"))
                                    code = 4;
                                  else if (strEQc (f->name, "active_viewport")
                                           || strEQc (f->name, "host_block"))
                                    code = 4;
                                  else if (strEQc (f->name, "writedep")
                                           || strEQc (f->name, "readdep"))
                                    code = 4;
                                  else if (strEQc (f->name, "imagedefreactor"))
                                    code = 3;
                                  else if (strEQc (f->name, "table_style"))
                                    code = 3;
                                  ref = dwg_add_handleref (dwg, code,
                                                           pair->value.u, obj);
                                  LOG_TRACE ("%s.%s = " FORMAT_REF " [H %d]\n",
                                             name, f->name, ARGS_REF (ref),
                                             pair->code);
                                }
                              else if (pair->type == DWG_VT_INT32
                                       && pair->value.u)
                                {
                                  ref = dwg_add_handleref (dwg, 5,
                                                           pair->value.u, obj);
                                  LOG_TRACE ("%s.%s = " FORMAT_REF " [H %d]\n",
                                             name, f->name, ARGS_REF (ref),
                                             pair->code);
                                }
                              else if ((pair->type == DWG_VT_STRING
                                        || pair->type == DWG_VT_HANDLE)
                                       && pair->value.s)
                                {
                                  obj_hdls = array_push (
                                      obj_hdls, f->name, pair->value.s,
                                      obj->tio.object->objid);
                                  LOG_TRACE ("%s.%s: name %s -> H for code "
                                             "%d later\n",
                                             name, f->name, pair->value.s,
                                             pair->code);
                                }
                            }
                          else
                            {
                              dwg_dynapi_entity_set_value (_obj, obj->name,
                                                           f->name, &ref, 1);
                              LOG_TRACE ("%s.%s = " FORMAT_REF " [H %d]\n",
                                         name, f->name, ARGS_REF (ref),
                                         pair->code);
                            }
                          goto next_pair; // found
                        }
                      // only 2D or 3D points .x
                      else if (f->size > 8
                               && (strchr (f->type, '2')
                                   || strchr (f->type, '3')
                                   || strEQc (f->type, "BE")))
                        {
                          // pt.x = 0.0;
                          // if (pair->value.d == 0.0) // ignore defaults
                          //  goto next_pair;
                          pt.x = pair->value.d;
                          dwg_dynapi_entity_set_value (_obj, obj->name,
                                                       f->name, &pt, 1);
                          LOG_TRACE ("%s.%s.x = %f [%s %d]\n", name, f->name,
                                     pair->value.d, f->type, pair->code);
                          goto next_pair; // found
                        }
                      else if (pair->type == DWG_VT_REAL
                               && strEQc (f->type, "TIMEBLL"))
                        {
                          static BITCODE_TIMEBLL date = { 0, 0, 0 };
                          date.value = pair->value.d;
                          date.days = (BITCODE_BL)trunc (pair->value.d);
                          date.ms = (BITCODE_BL)(86400000.0
                                                 * (date.value - date.days));
                          LOG_TRACE ("%s.%s %.09f (" FORMAT_BL ", " FORMAT_BL
                                     ") [TIMEBLL %d]\n",
                                     name, f->name, date.value, date.days,
                                     date.ms, pair->code);
                          dwg_dynapi_entity_set_value (_obj, obj->name,
                                                       f->name, &date, 1);
                          goto next_pair;
                        }
                      else if (f->size > 8 && strEQc (f->type, "CMC"))
                        {
                          BITCODE_CMC color;
                          dwg_dynapi_entity_value (_obj, obj->name, f->name,
                                                   &color, NULL);
                          if (pair->code < 90)
                            {
                              color.index = pair->value.i;
                              if (pair->value.i == 256)
                                color.method = 0xc2;
                              else if (pair->value.i == 257)
                                color.method = 0xc8;
                              else if (pair->value.i < 256
                                       && dat->from_version >= R_2004)
                                {
                                  color.method = 0xc3;
                                  color.rgb = 0xc3000000 | color.index;
                                  color.index = 256;
                                }
                              LOG_TRACE ("%s.%s.index = %d [%s %d]\n", name,
                                         f->name, color.index, "CMC",
                                         pair->code);
                              if (color.rgb)
                                LOG_TRACE ("%s.%s.rgb = 0x%08x [%s %d]\n",
                                           name, f->name, color.rgb, "CMC",
                                           pair->code);
                            }
                          else if (pair->code < 430)
                            {
                              color.rgb = pair->value.l;
                              color.method = pair->value.l >> 0x18;
                              if (pair->value.l == 257)
                                {
                                  color.method = 0xc8;
                                  color.rgb = 0xc8000000;
                                }
                              // color.alpha = (pair->value.l & 0xFF000000) >>
                              // 24; if (color.alpha)
                              //  color.alpha_type = 3;
                              LOG_TRACE ("%s.%s.rgb = %08X [%s %d]\n", name,
                                         f->name, pair->value.u, "CMC",
                                         pair->code);
                            }
                          else if (pair->code < 440)
                            {
                              color.flag |= 0x10;
                              color.name
                                  = dwg_add_u8_input (dwg, pair->value.s);
                              LOG_TRACE ("%s.%s.name = %s [%s %d]\n", name,
                                         f->name, pair->value.s, "CMC",
                                         pair->code);
                            }
                          else if (pair->code < 450)
                            {
                              color.alpha = (pair->value.l & 0xFF000000) >> 24;
                              if (color.alpha)
                                color.alpha_type = 3;
                              LOG_TRACE ("%s.%s.alpha = %08X [%s %d]\n", name,
                                         f->name, pair->value.u, "CMC",
                                         pair->code);
                            }
                          dwg_dynapi_entity_set_value (_obj, obj->name,
                                                       f->name, &color, 1);
                          goto next_pair; // found, early exit
                        }
                      // num_ field with associated vector
                      else if (memBEGINc (f->name, "num_")
                               && (strEQc (f->type, "BL")
                                   || strEQc (f->type, "BS")
                                   || strEQc (f->type, "RL")
                                   || strEQc (f->type, "RS")
                                   || strEQc (f->type, "RC")))
                        {
                          long old = get_numfield_value (_obj, f);
                          if (old)
                            {
                              LOG_ERROR ("%s.%s already set to %ld", obj->name,
                                         f->name, old);
                              if (is_class_stable (obj->name))
                                {
                                  error |= DWG_ERR_INVALIDDWG;
                                  goto invalid_dxf;
                                }
                              else
                                goto next_pair;
                            }
                          else
                            dwg_dynapi_entity_set_value (
                                _obj, obj->name, f->name, &pair->value, 1);
                        }
                      else
                        dwg_dynapi_entity_set_value (_obj, obj->name, f->name,
                                                     &pair->value, 1);
                      if (f->is_string)
                        {
                          LOG_TRACE ("%s.%s = %s [%s %d]\n", name, f->name,
                                     pair->value.s, f->type, pair->code);
                        }
                      else if (strchr (&f->type[1], 'D'))
                        {
                          LOG_TRACE ("%s.%s = %f [%s %d]\n", name, f->name,
                                     pair->value.d, f->type, pair->code);
                        }
                      else
                        {
                          LOG_TRACE ("%s.%s = %ld [%s %d]\n", name, f->name,
                                     pair->value.l, f->type, pair->code);
                        }
                      j = 0; // not a point nor vector member, so reset
                      goto next_pair; // found, early exit
                    }
                  // wrong code, maybe a point .y or .z
                  else if ((*f->type == '3' || *f->type == '2'
                            || strEQc (f->type, "BE"))
                           && f->dxf
                           && (strstr (f->type, "_1")
                                   ? f->dxf + 1 == pair->code // 2BD_1
                                   : f->dxf + 10 == pair->code))
                    {
                      // pt.y = 0.0;
                      // if (pair->value.d == 0.0) // ignore defaults
                      //  goto next_pair;
                      dwg_dynapi_entity_value (_obj, obj->name, f->name, &pt,
                                               NULL);
                      pt.y = pair->value.d;
                      dwg_dynapi_entity_set_value (_obj, obj->name, f->name,
                                                   &pt, 1);
                      LOG_TRACE ("%s.%s.y = %f [%s %d]\n", name, f->name,
                                 pair->value.d, f->type, pair->code);
                      goto next_pair; // found, early exit
                    }
                  else if ((*f->type == '3' || *f->type == '2'
                            || strEQc (f->type, "BE"))
                           && f->dxf
                           && (strstr (f->type, "_1")
                                   ? f->dxf + 2 == pair->code // 2BD_1
                                   : f->dxf + 20 == pair->code))
                    {
                      pt.z = 0.0;
                      // can ignore z or 0.0? e.g. no VPORT.view_target
                      if (strNE (name, "_3DFACE") && strNE (f->name, "scale")
                          && *f->type == '2')
                        goto next_pair;
                      dwg_dynapi_entity_value (_obj, obj->name, f->name, &pt,
                                               NULL);
                      pt.z = pair->value.d;
                      dwg_dynapi_entity_set_value (_obj, obj->name, f->name,
                                                   &pt, 0);
                      LOG_TRACE ("%s.%s.z = %f [%s %d]\n", name, f->name,
                                 pair->value.d, f->type, pair->code);

                      // 3DD scale
                      if (strEQc (f->name, "scale")
                          && dwg->header.version >= R_2000
                          && dwg_dynapi_entity_field (obj->name, "scale_flag")
                          && dwg_dynapi_entity_value (_obj, obj->name,
                                                      "scale_flag",
                                                      &scale_flag, NULL))
                        { // set scale_flag
                          scale_flag = 0;
                          if (pt.x == 1.0 && pt.y == 1.0 && pt.z == 1.0)
                            scale_flag = 3;
                          else if (pt.x == 1.0)
                            scale_flag = 1;
                          else if (pt.x == pt.y && pt.x == pt.z)
                            scale_flag = 2;
                          dwg_dynapi_entity_set_value (
                              _obj, obj->name, "scale_flag", &scale_flag, 0);
                          LOG_TRACE ("%s.scale_flag = %d [BB 0]\n", name,
                                     scale_flag);
                        }
                      // 3DFACE.z_is_zero
                      else if (strEQc (name, "_3DFACE")
                               && strEQc (f->name, "corner1")
                               && dwg->header.version >= R_2000 && pt.z == 0.0)
                        {
                          BITCODE_B z_is_zero = 1;
                          dwg_dynapi_entity_set_value (
                              _obj, obj->name, "z_is_zero", &z_is_zero, 0);
                          LOG_TRACE ("%s.z_is_zero = 1 [B 0]\n", name);
                        }

                      goto next_pair; // found, early exit
                    }
                  // FIELD_VECTOR_N BITCODE_BD transmatrix[16]:
                  else if (strEQc (f->type, "BD*")
                           && (strEQc (name, "EXTRUDEDSURFACE")
                               || strEQc (name, "LOFTEDSURFACE")
                               || strEQc (name, "SWEPTSURFACE")
                               || strEQc (name, "REVOLVEDSURFACE")
                               || strEQc (name, "MATERIAL")
                               || strEQc (name, "SPATIAL_FILTER")
                               || /* max 12 */
                               strEQc (name, "ACSH_SWEEP_CLASS"))
                           && ((pair->code >= 40 && pair->code <= 49)
                               || (pair->code <= 142 && pair->code <= 147)))
                    {
                      // 16x BD, via j
                      BITCODE_BD *matrix;
                      dwg_dynapi_entity_value (_obj, obj->name, f->name,
                                               &matrix, NULL);
                      if (!matrix)
                        {
                          matrix = (BITCODE_BD *)xcalloc (16,
                                                          sizeof (BITCODE_BD));
                          if (!matrix)
                            goto invalid_dxf;
                          j = 0;
                        }
                      if (j < 0 || j >= 16 || !matrix)
                        goto invalid_dxf;
                      assert (j >= 0 && j < 16 && matrix);
                      matrix[j] = pair->value.d;
                      dwg_dynapi_entity_set_value (_obj, obj->name, f->name,
                                                   &matrix, 0);
                      LOG_TRACE ("%s.%s[%d] = %f [%s %d]\n", name, f->name, j,
                                 pair->value.d, f->type, pair->code);
                      j++;
                      goto next_pair;
                    }
                }
              LOG_INSANE ("----\n");
              if (*subclass) // embedded subclasses/objects
                {
                  if (obj->fixedtype == DWG_TYPE_LAYOUT
                      && strEQc (subclass, "AcDbPlotSettings"))
                    {
                      Dwg_Object_LAYOUT *o = obj->tio.object->tio.LAYOUT;
                      int unique;
                      static double pt_x;
                      static const Dwg_DYNAPI_field *pt_f = NULL;
                      if (pair->code == 6 && *pair->value.s)
                        {
                          if (dwg->header.version < R_2004)
                            {
                              f = dwg_dynapi_entity_field ("PLOTSETTINGS",
                                                           "plotview_name");
                              dwg_dynapi_field_set_value (
                                  dwg, &o->plotsettings, f, &pair->value, 1);
                            }
                          else
                            {
                              BITCODE_H ref = find_tablehandle (dwg, pair);
                              f = dwg_dynapi_entity_field ("PLOTSETTINGS",
                                                           "plotview");
                              dwg_dynapi_field_set_value (
                                  dwg, &o->plotsettings, f, &ref, 1);
                            }
                          goto next_pair;
                        }
                      fields = dwg_dynapi_entity_fields ("PLOTSETTINGS");
                      f = dwg_dynapi_field_dxf (fields, pair->code, &unique);
                      if (f && unique)
                        {
                          if (*f->type == '2') // 2D points
                            {
                              pt_f = f;
                              pt_x = pair->value.d;
                              goto next_pair;
                            }
                          else if (strEQc (f->type, "H"))
                            {
                              BITCODE_H ref = find_tablehandle (dwg, pair);
                              if (!ref)
                                {
                                  if (pair->code > 300)
                                    {
                                      int code = 4;
                                      ref = dwg_add_handleref (
                                          dwg, code, pair->value.u, obj);
                                      LOG_TRACE (
                                          "%s.plotsettings.%s = " FORMAT_REF
                                          " [H %d]\n",
                                          name, f->name, ARGS_REF (ref),
                                          pair->code);
                                    }
                                  goto next_pair; // found
                                }
                              else
                                {
                                  dwg_dynapi_field_set_value (
                                      dwg, &o->plotsettings, f, &ref, 1);
                                  LOG_TRACE (
                                      "set %s.plotsettings.%s " FORMAT_REF
                                      " [H %d]\n",
                                      obj->name, f->name, ARGS_REF (ref),
                                      pair->code);
                                  goto next_pair; // found
                                }
                            }
                          else
                            {
                              LOG_TRACE ("set %s.plotsettings.%s [%s %d]\n",
                                         obj->name, f->name, f->type,
                                         pair->code);
                              dwg_dynapi_field_set_value (
                                  dwg, &o->plotsettings, f, &pair->value, 1);
                              goto next_pair;
                            }
                        }
                      else if (pt_f
                               && (pair->code == 47 || pair->code == 49
                                   || pair->code == 141 || pair->code == 149))
                        {
                          BITCODE_2BD pt2;
                          pt2.x = pt_x;
                          pt2.y = pair->value.d;
                          LOG_TRACE ("set %s.plotsettings.%s [%s %d]\n",
                                     obj->name, pt_f->name, pt_f->type,
                                     pt_f->dxf);
                          dwg_dynapi_field_set_value (dwg, &o->plotsettings,
                                                      pt_f, &pt2, 1);
                          goto next_pair;
                        }
                      else
                        LOG_WARN ("Unknown DXF code %d for %s", pair->code,
                                  subclass);
                    }
                }
              fields = is_entity ? dwg_dynapi_common_entity_fields ()
                                 : dwg_dynapi_common_object_fields ();
              for (f = &fields[0]; f->name; f++)
                {
                  LOG_INSANE ("-%s.%s [%d %s] vs %d\n",
                              is_entity ? "ENTITY" : "OBJECT", f->name, f->dxf,
                              f->type, pair->code)
                  if ((pair->code == 62 || pair->code == 420
                       || pair->code == 430 || pair->code == 440)
                      && (f->size > 8
                          && strEQc (f->type, "CMC"))) // alt. color fields
                    {
                      BITCODE_CMC color;
                      dwg_dynapi_common_value (_obj, f->name, &color, NULL);
                      if (pair->code == 62)
                        {
                          color.index = pair->value.i;
                          LOG_TRACE ("COMMON.%s.index = %d [%s %d]\n", f->name,
                                     pair->value.i, "CMC", pair->code);
                        }
                      else if (pair->code == 420)
                        {
                          color.rgb = pair->value.l;
                          color.method = pair->value.l >> 0x18;
                          if (pair->value.l == 257)
                            {
                              color.method = 0xc8;
                              color.rgb = 0xc8000000;
                            }
                          // color.alpha = (pair->value.l & 0xFF000000) >> 24;
                          // if (color.alpha)
                          //  color.alpha_type = 3;
                          LOG_TRACE ("COMMON.%s.rgb = %08X [%s %d]\n", f->name,
                                     pair->value.u, "CMC", pair->code);
                        }
                      else if (pair->code == 440)
                        {
                          color.flag |= 0x20;
                          color.alpha = (pair->value.l & 0xFF000000) >> 24;
                          color.alpha_type = pair->value.u >> 8;
                          if (color.alpha && !color.alpha_type)
                            color.alpha_type = 3;
                          LOG_TRACE ("COMMON.%s.alpha = %08X [%s %d]\n",
                                     f->name, pair->value.u, "CMC",
                                     pair->code);
                        }
                      else if (pair->code == 430)
                        {
                          color.flag |= 0x10;
                          color.name = dwg_add_u8_input (dwg, pair->value.s);
                          // TODO: book_name or name?
                          LOG_TRACE ("COMMON.%s.name = %s [%s %d]\n", f->name,
                                     pair->value.s, "CMC", pair->code);
                        }
                      dwg_dynapi_common_set_value (_obj, f->name, &color,
                                                   is_tu);
                      goto next_pair; // found, early exit
                    }
                  else if (f->dxf == pair->code)
                    {
                      /// resolve handle (table entry) given by name or ref
                      if (strEQc (f->type, "H"))
                        {
                          BITCODE_H handle = find_tablehandle (dwg, pair);
                          if (!handle)
                            {
                              if (pair->code > 300)
                                {
                                  handle = dwg_add_handleref (
                                      dwg, 5, pair->value.u, obj);
                                  dwg_dynapi_common_set_value (_obj, f->name,
                                                               &handle, 0);
                                  LOG_TRACE ("COMMON.%s = %X [H %d]\n",
                                             f->name, pair->value.u,
                                             pair->code)
                                }
                              else
                                {
                                  LOG_WARN ("TODO resolve common handle "
                                            "name %s %s",
                                            f->name, pair->value.s)
                                }
                            }
                          else
                            {
                              if (pair->code > 300)
                                LOG_TRACE (
                                    "COMMON.%s = " FORMAT_HV " [H %d]\n",
                                    f->name, pair->value.rll, pair->code)
                              else
                                LOG_TRACE ("COMMON.%s = %s [H %d]\n", f->name,
                                           pair->value.s, pair->code)
                              dwg_dynapi_common_set_value (_obj, f->name,
                                                           &handle, 0);
                            }
                          if (is_entity && pair->code == 6 && pair->value.s
                              && dwg->header.version >= R_2000)
                            {
                              BITCODE_BB flags = 3;
                              if (!strcasecmp (pair->value.s, "BYLAYER"))
                                flags = 0;
                              if (!strcasecmp (pair->value.s, "BYBLOCK"))
                                flags = 1;
                              if (!strcasecmp (pair->value.s, "CONTINUOUS"))
                                flags = 2;
                              dwg_dynapi_common_set_value (_obj, "ltype_flags",
                                                           &flags, 0);
                              LOG_TRACE ("COMMON.%s = %d [BB 0]\n",
                                         "ltype_flags", flags);
                            }
                          if (is_entity && pair->code == 390
                              && dwg->header.version >= R_2000)
                            {
                              BITCODE_BB flags = 3;
                              // eg. plotstyle: (5.2.765) abs:765 [H 390]
                              handle = dwg_add_handleref (dwg, 5,
                                                          pair->value.u, obj);
                              dwg_dynapi_common_set_value (_obj, "plotstyle",
                                                           &handle, 0);
                              dwg_dynapi_common_set_value (
                                  _obj, "plotstyle_flags", &flags, 0);
                              LOG_TRACE ("COMMON.%s = %d [BB 0]\n",
                                         "plotstyle_flags", flags);
                            }
                          if (is_entity && pair->code == 347
                              && dwg->header.version >= R_2007)
                            {
                              BITCODE_BB flags = 3;
                              handle = dwg_add_handleref (dwg, 5,
                                                          pair->value.u, obj);
                              dwg_dynapi_common_set_value (_obj, "material",
                                                           &handle, 0);
                              dwg_dynapi_common_set_value (
                                  _obj, "material_flags", &flags, 0);
                              LOG_TRACE ("COMMON.%s = %d [BB 0]\n",
                                         "material_flags", flags);
                            }
                          goto next_pair; // found, early exit
                        }
                      else if (pair->code == 310 && is_entity
                               && !obj->tio.entity->preview_size
                               && obj->fixedtype > DWG_TYPE_LAYOUT
                               && strEQc (subclass, "AcDbEntity"))
                        {
                          pair = add_ent_preview (obj, dat, pair);
                          goto start_loop;
                        }
                      else if (pair->code == 310 && is_entity
                               && obj->tio.entity->preview_size
                               && obj->fixedtype > DWG_TYPE_LAYOUT
                               && strEQc (subclass, "AcDbEntity"))
                        {
                          // This would corrupt the previous preview chain,
                          // don't append
                          LOG_ERROR ("Skip duplicate/interrupted %s.preview",
                                     obj->name)
                          goto next_pair;
                        }
                      else
                        {
                          // Don't write a ptr twice. This will fuckup the
                          // num_ counter. Just add to 310 preview, when
                          // prefixed by 92
                          if (f->is_malloc || f->is_string)
                            {
                              char *ptr = NULL;
                              if (dwg_dynapi_common_value (_obj, f->name, &ptr,
                                                           NULL)
                                  && ptr != NULL)
                                {
                                  LOG_ERROR ("Skip duplicate %s.%s [%s %d]",
                                             obj->name, f->name, f->type,
                                             pair->code)
                                  goto next_pair;
                                }
                            }
                          if (f->is_malloc)
                            {
                              char *str = strdup (pair->value.s);
                              dwg_dynapi_common_set_value (_obj, f->name, &str,
                                                           1);
                            }
                          else
                            {
                              dwg_dynapi_common_set_value (_obj, f->name,
                                                           &pair->value, 1);
                            }
                          if (f->is_string || f->type[0] == 'T')
                            {
                              LOG_TRACE ("COMMON.%s = \"%s\" [%s %d]\n",
                                         f->name, pair->value.s, f->type,
                                         pair->code)
                            }
                          else
                            {
                              if (is_entity && pair->code == 160
                                  && dwg->header.from_version >= R_2010)
                                {
                                  pair = add_ent_preview (obj, dat, pair);
                                  goto start_loop; // already fresh pair
                                }
                              if (strchr (f->type, 'D'))
                                LOG_TRACE ("COMMON.%s = %f [%s %d]\n", f->name,
                                           pair->value.d, f->type, pair->code)
                              else
                                LOG_TRACE ("COMMON.%s = %ld [%s %d]\n",
                                           f->name, pair->value.l, f->type,
                                           pair->code)
                            }
                          goto next_pair; // found, early exit
                        }
                    }
                }
              LOG_INSANE ("----\n")
              // still needed? already handled above
              // not in dynapi: 92 as 310 size prefix for PROXY vector preview
              // FIXME 92 is just for pre-r2010 entities. r2010+ is 160
              if (pair->code == 92 && is_entity
                  && dwg->header.from_version < R_2010
                  && (strEQc (subclass, "AcDbEntity")
                      || strEQc (subclass, "AcDbProxyEntity")
                      || strstr (subclass, "Surface")))
                {
                  pair = add_ent_preview (obj, dat, pair);
                  goto start_loop;
                }
              else if (strEQc (name, "SUN") && pair->code == 421)
                {
                  BITCODE_CMC color;
                  color.rgb = pair->value.l;
                  color.method = pair->value.l >> 0x18;
                  if (pair->value.l == 257)
                    {
                      color.method = 0xc8;
                      color.rgb = 0xc8000000;
                    }
                  // color.alpha = (pair->value.l & 0xFF000000) >> 24;
                  // if (color.alpha)
                  //  color.alpha_type = 3;
                  LOG_TRACE ("SUN.color.rgb = %08X [%s %d]\n", pair->value.u,
                             "CMC", pair->code);
                  goto next_pair;
                }
              else if (strEQc (name, "MULTILEADER"))
                {
                  // for the unknown subfields: 300, 140, 145, 302, 304, ...
                  pair = add_MULTILEADER (obj, dat, pair);
                  // returns with 0 or 301
                  if (pair && pair->code == 301)
                    goto next_pair;
                }
              else if (obj->fixedtype == DWG_TYPE_TABLESTYLE)
                {
                  // for the unknown subfields: 7, 140, ...
                  pair = add_TABLESTYLE (obj, dat, pair);
                  // returns with 0
                  if (pair && pair->code == 0)
                    goto start_loop;
                }
              else if (obj->fixedtype == DWG_TYPE_TABLEGEOMETRY)
                {
                  // for the unknown subfields: 93, 40, ...
                  pair = add_TABLEGEOMETRY_Cell (obj, dat, pair);
                  // returns with 0
                  if (pair && pair->code == 0)
                    goto start_loop;
                }
              else if (strEQc (name, "BLOCK")
                       && (pair->code == 70 || pair->code == 10
                           || pair->code == 20 || pair->code == 30
                           || pair->code == 3 || pair->code == 1
                           || pair->code == 4))
                ; // ignore those BLOCK fields. DXF artifacts
              else if (strEQc (name, "DIMENSION")
                       && (pair->code == 2 || pair->code == 210
                           || pair->code == 220 || pair->code == 230))
                ; // ignore the POLYLINE elevation.x,y. DXF artifacts
              else if (strEQc (name, "HATCH")
                       && (pair->code == 10 || pair->code == 20))
                ; // ignore the whole PLINE and VERTEX_PFACE_FACE 3BD 10
              else if ((strEQc (name, "VERTEX_PFACE_FACE")
                        || strEQc (name, "POLYLINE_3D")
                        || strEQc (name, "POLYLINE_2D")
                        || strEQc (name, "POLYLINE_MESH")
                        || strEQc (name, "POLYLINE_PFACE"))
                       && (pair->code == 10 || pair->code == 20
                           || pair->code == 30))
                ; // ignore the POLYLINE_PFACE flag 70
              else if (pair->code == 70 && strEQc (name, "POLYLINE_PFACE"))
                ;
              // always OLE
              else if (pair->code == 1 && strEQc (name, "OLE2FRAME")
                       && strEQc (pair->value.s, "OLE"))
                ;
              // the STYLE name, which is already defined by code 7
              else if (pair->code == 2 && strEQc (name, "SHAPE"))
                ;
              else if ((pair->code == 290 || pair->code == 2)
                       && ((obj->fixedtype == DWG_TYPE_REGION)
                           || (obj->fixedtype == DWG_TYPE_BODY)
                           || (obj->fixedtype == DWG_TYPE__3DSOLID)))
                LOG_TRACE ("Unknown DXF code %d for %s\n", pair->code, name)
              else if (obj->fixedtype == DWG_TYPE_PROXY_ENTITY
                       && pair->code == 92)
                {
                  pair = add_ent_preview (obj, dat, pair);
                  goto start_loop;
                }
              else if (obj->fixedtype == DWG_TYPE_PROXY_ENTITY
                       && (pair->code == 90 || pair->code == 91
                           || pair->code == 70 || pair->code == 93
                           || pair->code == 95 || pair->code == 330 // objids
                           || pair->code == 340                     // objids
                           || pair->code == 350                     // objids
                           || pair->code == 360                     // objids
                           || pair->code == 94)) // end of objids, ignore
                {
                  Dwg_Object_Ref *ref;
                  Dwg_Entity_PROXY_ENTITY *o
                      = obj->tio.entity->tio.PROXY_ENTITY;
                  if (pair->code == 90)
                    o->proxy_id = pair->value.i;
                  else if (pair->code == 91)
                    o->class_id = pair->value.i;
                  else if (pair->code == 93 && dwg->header.version < R_2010)
                    o->data_size = pair->value.i;
                  else if (pair->code == 95)
                    {
                      o->dwg_version = pair->value.u >> 16;
                      o->maint_version = pair->value.u & 0xff;
                    }
                  else if (pair->code == 70)
                    o->from_dxf = pair->value.i;
                  else if (pair->code == 330)
                    {
                      ref = dwg_add_handleref (dwg, 2, pair->value.u, obj);
                      PUSH_HV (o, num_objids, objids, ref);
                    }
                  else if (pair->code == 340)
                    {
                      ref = dwg_add_handleref (dwg, 3, pair->value.u, obj);
                      PUSH_HV (o, num_objids, objids, ref);
                    }
                  else if (pair->code == 350)
                    {
                      ref = dwg_add_handleref (dwg, 4, pair->value.u, obj);
                      PUSH_HV (o, num_objids, objids, ref);
                    }
                  else if (pair->code == 360)
                    {
                      ref = dwg_add_handleref (dwg, 5, pair->value.u, obj);
                      PUSH_HV (o, num_objids, objids, ref);
                    }
                }
              else if (obj->fixedtype == DWG_TYPE_PROXY_OBJECT
                       && (pair->code == 90 || pair->code == 91
                           || pair->code == 70 || pair->code == 93
                           || pair->code == 95 || pair->code == 330 // objids
                           || pair->code == 340                     // objids
                           || pair->code == 350                     // objids
                           || pair->code == 360                     // objids
                           || pair->code == 94)) // end of objids, ignore
                {
                  Dwg_Object_Ref *ref;
                  Dwg_Object_PROXY_OBJECT *o
                      = obj->tio.object->tio.PROXY_OBJECT;
                  if (pair->code == 90)
                    o->proxy_id = pair->value.i;
                  else if (pair->code == 91)
                    o->class_id = pair->value.i;
                  else if (pair->code == 93 && dwg->header.version < R_2010)
                    o->data_size = pair->value.i;
                  else if (pair->code == 95)
                    {
                      o->dwg_version = pair->value.u >> 16;
                      o->maint_version = pair->value.u & 0xff;
                    }
                  else if (pair->code == 70)
                    o->from_dxf = pair->value.i;
                  else if (pair->code == 330)
                    {
                      ref = dwg_add_handleref (dwg, 2, pair->value.u, obj);
                      PUSH_HV (o, num_objids, objids, ref);
                    }
                  else if (pair->code == 340)
                    {
                      ref = dwg_add_handleref (dwg, 3, pair->value.u, obj);
                      PUSH_HV (o, num_objids, objids, ref);
                    }
                  else if (pair->code == 350)
                    {
                      ref = dwg_add_handleref (dwg, 4, pair->value.u, obj);
                      PUSH_HV (o, num_objids, objids, ref);
                    }
                  else if (pair->code == 360)
                    {
                      ref = dwg_add_handleref (dwg, 5, pair->value.u, obj);
                      PUSH_HV (o, num_objids, objids, ref);
                    }
                }
              else if (obj->fixedtype == DWG_TYPE_LAYER
                       && ((pair->code == 348) || (pair->code == 420)
                           || (pair->code == 430) | (pair->code == 440)))
                {
                  Dwg_Object_LAYER *o = obj->tio.object->tio.LAYER;
                  if (pair->code == 420)
                    {
                      o->color.rgb = pair->value.l;
                      o->color.method = pair->value.l >> 0x18;
                      if (pair->value.l == 257)
                        {
                          o->color.method = 0xc8;
                          o->color.rgb = 0xc8000000;
                        }
                      // o->color.alpha = (pair->value.l & 0xFF000000) >> 24;
                      // if (o->color.alpha)
                      //  o->color.alpha_type = 3;
                      LOG_TRACE ("%s.color.rgb = %08X [%s %d]\n", name,
                                 pair->value.u, "CMC", pair->code);
                    }
                  else if (pair->code == 440)
                    {
                      o->color.flag |= 0x20; // ???
                      o->color.alpha = (pair->value.l & 0xFF000000) >> 24;
                      o->color.alpha_type = pair->value.u >> 8;
                      if (o->color.alpha && !o->color.alpha_type)
                        o->color.alpha_type = 3;
                      LOG_TRACE ("%s.color.alpha = %08X [%s %d]\n", name,
                                 pair->value.u, "CMC", pair->code);
                    }
                  else if (pair->code == 430)
                    {
                      char *x;
                      o->color.book_name = strdup (pair->value.s);
                      x = strchr (o->color.book_name, '$');
                      if (!x) // name only
                        {
                          o->color.name = o->color.book_name;
                          o->color.flag = 1;
                          o->color.book_name = NULL;
                          LOG_TRACE ("%s.color.name = %s [%s %d]\n", name,
                                     pair->value.s, "CMC", pair->code);
                          if (dwg->header.version >= R_2007)
                            {
                              char *tmp = o->color.name;
                              o->color.name = (BITCODE_T)bit_utf8_to_TU (
                                  o->color.name, 0);
                              free (tmp);
                            }
                        }
                      else
                        { // book with name
                          o->color.flag = 3;
                          o->color.name = strdup (x + 1);
                          x[0] = '\0';
                          if (dwg->header.version >= R_2007)
                            {
                              char *tmp = o->color.book_name;
                              o->color.book_name = (BITCODE_T)bit_utf8_to_TU (
                                  o->color.book_name, 0);
                              free (tmp);
                              tmp = o->color.name;
                              o->color.name = (BITCODE_T)bit_utf8_to_TU (
                                  o->color.name, 0);
                              free (tmp);
                            }
                          LOG_TRACE ("%s.color.book+name = %s [%s %d]\n", name,
                                     pair->value.s, "CMC", pair->code);
                        }
                    }
                  else if (pair->code == 348)
                    LOG_TRACE ("Unknown DXF code %d for %s\n", pair->code,
                               name);
                  goto next_pair;
                }
              else if (obj->fixedtype == DWG_TYPE_DIMSTYLE
                       && pair->code == 287)
                {
                  // <= r14
                  Dwg_Object_DIMSTYLE *o = obj->tio.object->tio.DIMSTYLE;
                  o->DIMFIT = pair->value.i;
                  LOG_TRACE ("%s.DIMFIT = %d [%s %d]\n", name, pair->value.i,
                             "RC", pair->code);
                }
              else if (obj->fixedtype == DWG_TYPE_DIMENSION_ALIGNED
                       && pair->code == 52)
                {
                  BITCODE_BD ang = deg2rad (pair->value.d);
                  UPGRADE_ENTITY (DIMENSION_ALIGNED, DIMENSION_LINEAR)
                  dwg_dynapi_entity_set_value (_obj, "DIMENSION_LINEAR",
                                               "oblique_angle", &ang, 1);
                  LOG_TRACE ("%s.%s = %f (from DEG %f°) [%s %d]\n", name,
                             "oblique_angle", ang, pair->value.d, "BD", 52);
                }
              else if (obj->fixedtype == DWG_TYPE_DIMENSION_ALIGNED
                       && pair->code == 50)
                {
                  BITCODE_BD ang = deg2rad (pair->value.d);
                  UPGRADE_ENTITY (DIMENSION_ALIGNED, DIMENSION_LINEAR)
                  dwg_dynapi_entity_set_value (_obj, "DIMENSION_LINEAR",
                                               "dim_rotation", &ang, 1);
                  LOG_TRACE ("%s.%s = %f (from DEG %f°) [%s %d]\n", name,
                             "dim_rotation", ang, pair->value.d, "BD", 50);
                }
              // accept wrong colors
              else if (is_dxf_class_importable (obj->name)
                       && (pair->code < 60 || pair->code > 68))
                {
                  goto invalid_dxf;
                }
              else
                LOG_WARN ("Unknown DXF code %d for %s", pair->code,
                          name); // Debugging or Unhandled class
            }
        }
    next_pair:
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
      DXF_RETURN_EOF (pair);
    }

  if (obj->fixedtype == DWG_TYPE_SEQEND)
    dxf_postprocess_SEQEND (obj);
  else if (obj->fixedtype == DWG_TYPE_LAYOUT)
    dxf_postprocess_LAYOUT (obj);
  else if (obj->fixedtype == DWG_TYPE_MLINESTYLE)
    dxf_postprocess_MLINESTYLE (obj); // FIXME. not triggered
  else if (obj->fixedtype == DWG_TYPE_PLOTSETTINGS)
    dxf_postprocess_PLOTSETTINGS (obj);
  // set defaults not in dxf:
  else if (obj->type == DWG_TYPE__3DFACE && dwg->header.from_version >= R_2000)
    {
      Dwg_Entity__3DFACE *o = obj->tio.entity->tio._3DFACE;
      o->has_no_flags = 1;
      LOG_TRACE ("_3DFACE.has_no_flags = 1 [B]\n");
    }
  else if (is_textlike (obj))
    postprocess_TEXTlike (obj);

  return pair;

invalid_dxf:
  LOG_ERROR ("Invalid DXF code %d for %s", pair->code, name)
  dxf_free_pair (pair);
  return NULL;
}

static int
dxf_tables_read (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  char table[80];
  Dxf_Pair *pair;

  pair = dxf_read_pair (dat);
  table[0] = '\0'; // init
  while (pair)     // read next 0 TABLE
    {
      if (pair->code == 0 && pair->value.s) // TABLE or ENDTAB
        {
          if (strEQc (pair->value.s, "TABLE"))
            table[0] = '\0'; // new table coming up
          else if (strEQc (pair->value.s, "BLOCK_RECORD"))
            {
              strncpy (table, pair->value.s, 79);
              table[79] = '\0';
            }
          else if (strEQc (pair->value.s, "ENDTAB"))
            {
              table[0] = '\0'; // close table
            }
          else if (strEQc (pair->value.s, "ENDSEC"))
            {
              dxf_free_pair (pair);
              return 0;
            }
          else
            {
              LOG_ERROR ("Unknown 0 %s (%s)", pair->value.s, "tables");
              dxf_free_pair (pair);
              return 1;
            }
        }
      else if (pair->code == 2 && pair->value.s && strlen (pair->value.s) < 80
               && is_table_name (pair->value.s)) // new table NAME
        {
          long i = 0;
          BITCODE_BL ctrl_id;
          strncpy (table, pair->value.s, 79);
          table[79] = '\0';
          dxf_free_pair (pair);
          pair = new_table_control (table, dat, dwg); // until 0 table
          ctrl_id = dwg->num_objects - 1;             // dwg->object might move
          while (pair && pair->code == 0 && pair->value.s
                 && strEQ (pair->value.s, table))
            {
              Dwg_Object *obj;
              Dwg_Object *ctrl;
              char *dxfname = strdup (pair->value.s);
              BITCODE_BL idx = dwg->num_objects;
              BITCODE_H ref;
              dxf_free_pair (pair);
              // until 0 table or 0 ENDTAB
              pair = new_object (table, dxfname, dat, dwg, ctrl_id,
                                 (BITCODE_BL *)&i);
              obj = &dwg->object[idx];
              ctrl = &dwg->object[ctrl_id];
              if (!pair)
                {
                  free (dxfname);
                  if (idx != dwg->num_objects)
                    obj->dxfname = NULL;
                  return DWG_ERR_INVALIDDWG;
                }
              // A minimal DXF will have no handle values
              if (!obj->handle.value)
                {
                  BITCODE_RLL next_handle = dwg_next_handle (dwg);
                  dwg_add_handle (&obj->handle, 0, next_handle, NULL);
                  // ref = dwg_add_handleref (dwg, 3, next_handle, ctrl);
                  LOG_TRACE ("%s.handle = (0.%d." FORMAT_HV ")\n", obj->name,
                             obj->handle.size, obj->handle.value);
                }
              // set ownerhandles
              if (dat->version >= R_13 && dat->version < R_14)
                {
                  if (obj->tio.object && !obj->tio.object->ownerhandle)
                    {
                      obj->tio.object->ownerhandle
                        = dwg_add_handleref (dwg, 4, ctrl->handle.value, NULL);
                      LOG_TRACE ("%s.ownerhandle = (4.%d." FORMAT_HV ")\n", obj->name,
                                 ctrl->handle.size, ctrl->handle.value);
                    }
                }
              {
                Dwg_Object_BLOCK_CONTROL *_ctrl
                    = ctrl->tio.object->tio.BLOCK_CONTROL;
                // dont add mspace, pspace entries, nor some LTYPE_CONTROL.entries
                if (strEQc (table, "BLOCK_RECORD"))
                  {
                    if (_ctrl->model_space
                        && obj->handle.value
                               == _ctrl->model_space->absolute_ref)
                      ;
                    else if (_ctrl->paper_space
                             && obj->handle.value
                                    == _ctrl->paper_space->absolute_ref)
                      ;
                    else if (find_hv (_ctrl->entries, _ctrl->num_entries, obj->handle.value) < 0)
                      {
                        ref = dwg_add_handleref (dwg, 2, obj->handle.value,
                                                 NULL);
                        PUSH_HV (_ctrl, num_entries, entries, ref);
                      }
                  }
                else if (strEQc (table, "LTYPE"))
                  {
                    Dwg_Object_LTYPE *_obj = obj->tio.object->tio.LTYPE;
                    Dwg_Object_LTYPE_CONTROL *_lctrl
                      = ctrl->tio.object->tio.LTYPE_CONTROL;
                    if (_lctrl->bylayer
                        && obj->handle.value == _lctrl->bylayer->absolute_ref)
                      ;
                    else if (_lctrl->byblock
                             && obj->handle.value
                             == _lctrl->byblock->absolute_ref)
                      ;
                    // already exists?
                    else if (find_hv (_lctrl->entries, _lctrl->num_entries, obj->handle.value) < 0)
                      ;
                    else
                      {
                        ref = dwg_add_handleref (dwg, 2, obj->handle.value,
                                                 NULL);
                        PUSH_HV (_ctrl, num_entries, entries, ref);
                      }
                    if (dwg->header.from_version > R_2004 && _obj->name
                        && _obj->has_strings_area)
                      {
                        _obj->strings_area = (BITCODE_TF)xcalloc (512, 1);
                        if (!_obj->strings_area)
                          {
                            free (dxfname);
                            goto outofmem;
                          }
                      }
                    if (dwg->header.from_version <= R_2004)
                      {
                        _obj->strings_area = (BITCODE_TF)xcalloc (256, 1);
                        if (!_obj->strings_area)
                          {
                            free (dxfname);
                            goto outofmem;
                          }
                      }
                  }
                else
                  {
                    // if it not already exists
                    if (find_hv (_ctrl->entries, _ctrl->num_entries, obj->handle.value) < 0)
                      {
                        ref = dwg_add_handleref (dwg, 2, obj->handle.value,
                                                 NULL);
                        PUSH_HV (_ctrl, num_entries, entries, ref);
                      }
                  }
              }
            }
          // next table
          // fixup entries vs num_entries (no NULL entries)
          {
            Dwg_Object *ctrl = &dwg->object[ctrl_id];
            Dwg_Object_BLOCK_CONTROL *_ctrl
                = ctrl->tio.object->tio.BLOCK_CONTROL;
            int at_end = 1;
            unsigned num_entries = 0;
            if (!dwg_obj_is_control (ctrl))
              {
                LOG_ERROR ("Missing CONTROL object");
                dxf_free_pair (pair);
                return 1;
              }
            // A minimal DXF will have no handle values, assign them then
            if (!ctrl->handle.value)
              {
                BITCODE_RLL next_handle = dwg_next_handle (dwg);
                dwg_add_handle (&ctrl->handle, 0, next_handle, NULL);
                // adds header_vars->CONTROL ref
                (void)dwg_ctrl_table (dwg, table);
                // ref = dwg_add_handleref (dwg, 3, next_handle, ctrl);
                LOG_TRACE ("%s.handle = (0.%d." FORMAT_HV ")\n", ctrl->name,
                           ctrl->handle.size, ctrl->handle.value);
              }
            num_entries = _ctrl->num_entries;
            if (_ctrl && ctrl->fixedtype == DWG_TYPE_BLOCK_CONTROL)
              {
                for (int j = num_entries - 1; j >= 0; j--)
                  {
                    BITCODE_H ref = _ctrl->entries ? _ctrl->entries[j] : NULL;
                    if (!ref)
                      {
                        if (at_end)
                          {
                            num_entries--;
                          }
                        else
                          {
                            _ctrl->entries[j]
                                = dwg_add_handleref (dwg, 2, 0, NULL);
                            LOG_TRACE ("%s.entries[%d] = (2.0.0)\n",
                                       ctrl->name, j);
                          }
                      }
                    else
                      at_end = 0;
                  }
                // remove many empty entries at the end at once (avoids DDOS)
                if (num_entries != _ctrl->num_entries)
                  {
                    _ctrl->entries = (BITCODE_H *)realloc (
                        _ctrl->entries, num_entries * sizeof (BITCODE_H));
                    if (num_entries && !_ctrl->entries)
                      goto outofmem;
                    _ctrl->num_entries = num_entries;
                    LOG_TRACE ("%s.num_entries => %d\n", ctrl->name,
                               _ctrl->num_entries);
                  }
                // leave room for one active entry
                if (_ctrl->num_entries == 1 && !_ctrl->entries[0])
                  {
                    _ctrl->entries[0] = dwg_add_handleref (dwg, 2, 0, NULL);
                    LOG_TRACE ("%s.entries[0] = (2.0.0)\n", ctrl->name);
                  }
              }
          }
        }
      DXF_RETURN_ENDSEC (0) // next TABLE or ENDSEC
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
      DXF_CHECK_EOF;
    } // while (pair)
  dxf_free_pair (pair);
  return 0;

outofmem:
  dxf_free_pair (pair);
  return DWG_ERR_OUTOFMEM;
}

static int
dxf_blocks_read (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  char name[80];
  Dxf_Pair *pair = dxf_read_pair (dat);
  Dwg_Object *obj;

  name[0] = '\0'; // init
  while (pair)    // read next 0 TABLE
    {
      if (pair != NULL && pair->code == 0 && pair->value.s)
        {
          BITCODE_BL i = 0;
          BITCODE_BB entmode = 0;
          while (pair != NULL && pair->code == 0 && pair->value.s
                 && strNE (pair->value.s, "ENDSEC"))
            {
              Dwg_Object *blkhdr = NULL;
              BITCODE_BL idx = dwg->num_objects;
              char *dxfname = strdup (pair->value.s);
              strncpy (name, dxfname, 79);
              name[79] = '\0';
              entity_alias (name);
              dxf_free_pair (pair);
              // complete old obj
              obj = &dwg->object[idx - 1];
              if (idx && !obj->handle.value)
                {
                  BITCODE_RLL next_handle = dwg_next_handle (dwg);
                  dwg_add_handle (&obj->handle, 0, next_handle, NULL);
                  LOG_TRACE ("%s.handle = (0.%d." FORMAT_HV ")\n", obj->name,
                             obj->handle.size, obj->handle.value);
                }
              pair = new_object (name, dxfname, dat, dwg, 0, &i);
              obj = &dwg->object[idx];
              if (!pair)
                {
                  free (dxfname);
                  if (idx != dwg->num_objects)
                    obj->dxfname = NULL;
                  return DWG_ERR_INVALIDDWG;
                }
              if (obj->type == DWG_TYPE_BLOCK)
                {
                  Dwg_Object_Entity *ent = obj->tio.entity;
                  Dwg_Entity_BLOCK *_obj = obj->tio.entity->tio.BLOCK;
                  i = 0;
                  if (ent->ownerhandle
                      && (blkhdr = dwg_ref_object (dwg, ent->ownerhandle)))
                    {
                      if (blkhdr->fixedtype == DWG_TYPE_BLOCK_HEADER)
                        {
                          Dwg_Object_BLOCK_HEADER *_hdr
                              = blkhdr->tio.object->tio.BLOCK_HEADER;
                          ent->ownerhandle->obj = NULL; // still dirty
                          _hdr->block_entity = dwg_add_handleref (
                              dwg, 3, obj->handle.value, blkhdr);
                          LOG_TRACE ("BLOCK_HEADER.block_entity = " FORMAT_REF
                                     " [H] (blocks)\n",
                                     ARGS_REF (_hdr->block_entity));
                          _hdr->base_pt.x = _obj->base_pt.x;
                          _hdr->base_pt.y = _obj->base_pt.y;
                          LOG_TRACE ("BLOCK_HEADER.base_pt = BLOCK.base_pt\n");
                        }
                      else if (blkhdr->fixedtype == DWG_TYPE_BLOCK_CONTROL)
                        {
                          Dwg_Object_BLOCK_CONTROL *_ctrl
                              = blkhdr->tio.object->tio.BLOCK_CONTROL;
                          ent->ownerhandle->obj = NULL; // still dirty
                          if (!_ctrl->model_space && _obj->name
                              && bit_eq_T (dat, _obj->name, "*Model_Space"))
                            {
                              _ctrl->model_space = dwg_add_handleref (
                                  dwg, 3, obj->handle.value, blkhdr);
                              LOG_TRACE (
                                  "BLOCK_CONTROL.model_space = " FORMAT_REF
                                  " [H] (blocks)\n",
                                  ARGS_REF (_ctrl->model_space));
                            }
                          else if (!_ctrl->paper_space && _obj->name
                                   && bit_eq_T (dat, _obj->name,
                                                "*Paper_Space"))
                            {
                              _ctrl->paper_space = dwg_add_handleref (
                                  dwg, 3, obj->handle.value, blkhdr);
                              LOG_TRACE (
                                  "BLOCK_CONTROL.paper_space = " FORMAT_REF
                                  " [H] (blocks)\n",
                                  ARGS_REF (_ctrl->paper_space));
                            }
                        }
                    }
                  else
                    blkhdr = NULL;
                  if (!_obj || !_obj->name)
                    ;
                  else if (bit_eq_T (dat, _obj->name, "*Model_Space")
                           || bit_eq_T (dat, _obj->name, "*MODEL_SPACE"))
                    entmode = ent->entmode = 2;
                  else if (bit_eq_T (dat, _obj->name, "*Paper_Space")
                           || bit_eq_T (dat, _obj->name, "*PAPER_SPACE"))
                    entmode = ent->entmode = 1;
                  else
                    entmode = 0;
                  if (!ent->isbylayerlt && !ent->ltype_flags && !ent->ltype)
                    ent->isbylayerlt = 1;
                }
              else if (obj->type == DWG_TYPE_ENDBLK)
                {
                  Dwg_Object_Entity *ent = obj->tio.entity;
                  Dwg_Entity_BLOCK *_obj = obj->tio.entity->tio.BLOCK;
                  if (!ent->isbylayerlt && !ent->ltype_flags && !ent->ltype)
                    ent->isbylayerlt = 1;
                  ent->entmode = entmode;
                  LOG_TRACE ("%s.entmode = %d [BB] (blocks)\n", obj->name,
                             entmode);
                  entmode = 0;
                  // set BLOCK_HEADER.endblk_entity handle
                  if ((blkhdr = dwg_ref_object (dwg, ent->ownerhandle))
                      && blkhdr->fixedtype == DWG_TYPE_BLOCK_HEADER)
                    {
                      Dwg_Object_BLOCK_HEADER *_hdr
                          = blkhdr->tio.object->tio.BLOCK_HEADER;
                      ent->ownerhandle->obj = NULL; // still dirty
                      _hdr->endblk_entity = dwg_add_handleref (
                          dwg, 3, obj->handle.value, blkhdr);
                      LOG_TRACE ("BLOCK_HEADER.endblk_entity = " FORMAT_REF
                                 " [H] (blocks)\n",
                                 ARGS_REF (_hdr->endblk_entity));
                      if (strcasecmp (_hdr->name, "*PAPER_SPACE") == 0)
                        ent->entmode = 1;
                      if (_hdr->last_entity)
                        {
                          LOG_TRACE ("BLOCK_HEADER.last_entity = " FORMAT_REF
                                     " [H] (blocks)\n",
                                     ARGS_REF (_hdr->last_entity));
                        }
                    }
                }
              // normal entity
              else if (obj->supertype == DWG_SUPERTYPE_ENTITY)
                {
                  Dwg_Object_Entity *ent = obj->tio.entity;
                  Dwg_Object_BLOCK_HEADER *_hdr;
                  ent->entmode = entmode;
                  LOG_TRACE ("%s.entmode = %d [BB] (blocks)\n", obj->name,
                             entmode);
                  if (!ent->isbylayerlt && !ent->ltype_flags && !ent->ltype)
                    ent->isbylayerlt = 1;
                  // add to entities
                  if (ent->ownerhandle
                      && (blkhdr = dwg_ref_object (dwg, ent->ownerhandle))
                      && blkhdr->fixedtype == DWG_TYPE_BLOCK_HEADER
                      && (_hdr = blkhdr->tio.object->tio.BLOCK_HEADER))
                    {
                      BITCODE_H ref = dwg_add_handleref (
                          dwg, 3, obj->handle.value, NULL);
                      PUSH_HV (_hdr, num_owned, entities, ref)
                    }
                  if (ent->ownerhandle
                      && (dwg->header.version >= R_13
                          || dwg->header.version == R_INVALID)
                      /* requires target version being set */
                      && dwg->header.version < R_2004
                      && (blkhdr = dwg_ref_object (dwg, ent->ownerhandle))
                      && blkhdr->fixedtype == DWG_TYPE_BLOCK_HEADER
                      && (_hdr = blkhdr->tio.object->tio.BLOCK_HEADER))
                    {
                      _hdr->last_entity = dwg_add_handleref (
                          dwg, 4, obj->handle.value, NULL);

                      if (!_hdr->first_entity)
                        {
                          _hdr->first_entity = _hdr->last_entity;

                          LOG_TRACE ("BLOCK_HEADER.first_entity = " FORMAT_REF
                                     " [H] (blocks)\n",
                                     ARGS_REF (_hdr->first_entity));
                        }
                    }
                }
            }
          DXF_RETURN_ENDSEC (0) // next BLOCK or ENDSEC
        }
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
      DXF_CHECK_EOF;
    }
  obj = &dwg->object[dwg->num_objects - 1];
  if (dwg->num_objects && !obj->handle.value)
    {
      BITCODE_RLL next_handle = dwg_next_handle (dwg);
      dwg_add_handle (&obj->handle, 0, next_handle, NULL);
      LOG_TRACE ("%s.handle = (0.%d." FORMAT_HV ")\n", obj->name,
                 obj->handle.size, obj->handle.value);
    }
  dxf_free_pair (pair);
  return 0;
}

// register this block entity, e.g. ModelSpace with the BLOCK_HEADER
// TODO use this also in the BLOCKS section, not just ENTITIES
static void
add_to_BLOCK_HEADER (Dwg_Object *restrict obj,
                     Dwg_Object_Ref *restrict ownerhandle)
{
  Dwg_Data *dwg = obj->parent;
  Dwg_Object_BLOCK_HEADER *_ctrl;
  Dwg_Object *ctrl = dwg_ref_object (dwg, ownerhandle);

  if (!ctrl || ctrl->type != DWG_TYPE_BLOCK_HEADER)
    return;
  _ctrl = ctrl->tio.object->tio.BLOCK_HEADER;
  if (obj->supertype != DWG_SUPERTYPE_ENTITY)
    return;
  LOG_TRACE ("add_to_BLOCK_HEADER %s: %s [" FORMAT_HV "]\n", _ctrl->name,
             obj->name, obj->handle.value);
  if (obj->type == DWG_TYPE_ENDBLK)
    {
      if (!_ctrl->endblk_entity)
        _ctrl->endblk_entity
            = dwg_add_handleref (dwg, 3, obj->handle.value, ctrl);
      return;
    }
  if (obj->type == DWG_TYPE_BLOCK)
    {
      if (!_ctrl->block_entity)
        _ctrl->block_entity
            = dwg_add_handleref (dwg, 3, obj->handle.value, ctrl);
      return;
    }
  if (!_ctrl->first_entity)
    _ctrl->last_entity = _ctrl->first_entity
        = dwg_add_handleref (dwg, 4, obj->handle.value, NULL);
  else
    // always overwrite. and it is global, so we can reuse it.
    _ctrl->last_entity = dwg_add_handleref (dwg, 4, obj->handle.value, NULL);
  PUSH_HV (_ctrl, num_owned, entities, _ctrl->last_entity);
}

static int
dxf_entities_read (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  Dxf_Pair *pair = dxf_read_pair (dat);
  char name[80];
  BITCODE_RLL mspace = dwg->header_vars.BLOCK_RECORD_MSPACE
                           ? dwg->header_vars.BLOCK_RECORD_MSPACE->absolute_ref
                           : 0x1F;
  BITCODE_RLL pspace = dwg->header_vars.BLOCK_RECORD_PSPACE
                           ? dwg->header_vars.BLOCK_RECORD_PSPACE->absolute_ref
                           : 0UL;
  BITCODE_H mspace_ref = dwg_model_space_ref (dwg);

  while (pair != NULL && pair->code == 0 && pair->value.s)
    {
      strncpy (name, pair->value.s, 79);
      name[79] = '\0';
      entity_alias (name);
      // until 0 ENDSEC
      while (pair != NULL && pair->code == 0 && pair->value.s
             && (is_dwg_entity (name) || strEQc (name, "DIMENSION")))
        {
          char *dxfname = strdup (pair->value.s);
          BITCODE_BL idx = dwg->num_objects;
          // LOG_HANDLE ("dxfname = strdup (%s)\n", dxfname);
          if (idx)
            {
              Dwg_Object *obj = &dwg->object[dwg->num_objects - 1];
              if (!obj->handle.value)
                {
                  BITCODE_RLL next_handle = dwg_next_handle (dwg);
                  dwg_add_handle (&obj->handle, 0, next_handle, NULL);
                  LOG_TRACE ("%s.handle = (0.%d." FORMAT_HV ")\n", obj->name,
                             obj->handle.size, obj->handle.value);
                }
            }
          dxf_free_pair (pair);
          pair = new_object (name, dxfname, dat, dwg, 0, NULL);
          if (!pair)
            {
              Dwg_Object *obj = &dwg->object[idx];
              free (dxfname);
              if (idx != dwg->num_objects)
                obj->dxfname = NULL;
              return DWG_ERR_INVALIDDWG;
            }
          if (pair->code == 0 && pair->value.s)
            {
              BITCODE_BL last_ent = dwg->num_objects - 1;
              Dwg_Object *obj = &dwg->object[last_ent];
              Dwg_Object_Entity *ent = obj->tio.entity;
              // FIXUP hack for added VX and VX_CONTROL
              while (obj->supertype != DWG_SUPERTYPE_ENTITY)
                {
                  last_ent--;
                  obj = &dwg->object[last_ent];
                  ent = obj->tio.entity;
                }
              if (ent->ownerhandle)
                {
                  if (ent->ownerhandle->absolute_ref == mspace)
                    ent->entmode = 2;
                  else if (pspace && ent->ownerhandle->absolute_ref == pspace)
                    ent->entmode = 1;
                  add_to_BLOCK_HEADER (obj, ent->ownerhandle);
                }
              else
                {
                  ent->entmode = 2;
                  add_to_BLOCK_HEADER (obj, mspace_ref);
                }

              in_postprocess_handles (obj);
              strncpy (name, pair->value.s, 79);
              name[79] = '\0';
              entity_alias (name);
            }
        }
      if (dwg->num_objects)
        {
          Dwg_Object *obj = &dwg->object[dwg->num_objects - 1];
          if (!obj->handle.value)
            {
              BITCODE_RLL next_handle = dwg_next_handle (dwg);
              dwg_add_handle (&obj->handle, 0, next_handle, NULL);
              LOG_TRACE ("%s.handle = (0.%d." FORMAT_HV ")\n", obj->name,
                         obj->handle.size, obj->handle.value);
            }
        }
      DXF_RETURN_ENDSEC (0)
      else LOG_WARN ("Unhandled 0 %s (%s)", name, "entities");
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
      DXF_CHECK_EOF;
    }
  dxf_free_pair (pair);
  return 0;
}

static int
dxf_objects_read (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  char name[80];
  Dxf_Pair *pair = dxf_read_pair (dat);
  while (pair != NULL)
    {
      while (pair != NULL && pair->code == 0 && pair->value.s)
        {
          BITCODE_BL idx = dwg->num_objects;
          strncpy (name, pair->value.s, 79);
          name[79] = '\0';
          object_alias (name);
          if (is_dwg_object (name))
            {
              char *dxfname = strdup (pair->value.s);
              // LOG_HANDLE ("dxfname = strdup (%s)\n", dxfname);
              dxf_free_pair (pair);
              pair = new_object (name, dxfname, dat, dwg, 0, NULL);
              if (!pair)
                {
                  Dwg_Object *obj = &dwg->object[idx];
                  free (dxfname);
                  if (idx != dwg->num_objects)
                    obj->dxfname = NULL;
                  return DWG_ERR_INVALIDDWG;
                }
            }
          else
            {
              DXF_RETURN_ENDSEC (0);
              LOG_WARN ("Unhandled 0 %s (%s)", name, "objects");
              dxf_free_pair (pair);
              pair = dxf_read_pair (dat);
              DXF_CHECK_EOF;
            }
        }
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
      DXF_CHECK_EOF;
    }
  dxf_free_pair (pair);
  return 0;
}

// redirected from ACDSDATA for now
static int
dxf_unknownsection_read (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  Dxf_Pair *pair = dxf_read_pair (dat);
  // until 0 ENDSEC
  while (pair)
    {
      while (pair && pair->code == 0 && pair->value.s)
        {
          DXF_RETURN_ENDSEC (0);
          LOG_WARN ("Unhandled 0 %s (%s)", pair->value.s, "unknownsection");
          dxf_free_pair (pair);
          pair = dxf_read_pair (dat);
          DXF_CHECK_EOF;
        }
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
      DXF_CHECK_EOF;
    }
  dxf_free_pair (pair);
  return DWG_ERR_INVALIDTYPE;
}

// read to THUMBNAIL dwg->thumbnail, size 90. not entity->preview
static int
dxf_thumbnail_read (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  Dxf_Pair *pair = dxf_read_pair (dat);
  size_t written = 0;

  while (pair)
    {
      switch (pair->code)
        {
        case 0: // ENDSEC
          dxf_free_pair (pair);
          return 0;
        case 90:
          dwg->thumbnail.size = pair->value.l; // INT32 => long
          dwg->thumbnail.chain
              = (unsigned char *)calloc (dwg->thumbnail.size, 1);
          if (!dwg->thumbnail.chain)
            {
              dxf_free_pair (pair);
              LOG_ERROR ("Out of memory");
              return DWG_ERR_OUTOFMEM;
            }
          LOG_TRACE ("PREVIEW.size = %" PRIuSIZE "\n", dwg->thumbnail.size);
          break;
        case 310:
          if (pair->value.s)
            {
              size_t len = strlen (pair->value.s);
              size_t blen = len / 2;
              size_t read;
              // const char *pos = pair->value.s;
              unsigned char *s = &dwg->thumbnail.chain[written];
              if (blen + written > dwg->thumbnail.size)
                {
                  dxf_free_pair (pair);
                  LOG_ERROR ("PREVIEW.size overflow: %" PRIuSIZE
                             " + written %" PRIuSIZE " > "
                             "size: %" PRIuSIZE,
                             blen, written, dwg->thumbnail.size);
                  return 1;
                }
              if ((read = in_hex2bin (s, pair->value.s, blen) != blen))
                LOG_ERROR ("in_hex2bin read only %" PRIuSIZE " of %" PRIuSIZE,
                           read, blen);
              written += read;
              LOG_TRACE ("PREVIEW.chain += %" PRIuSIZE " (%" PRIuSIZE
                         "/%" PRIuSIZE ")\n",
                         blen, written, dwg->thumbnail.size);
            }
          break;
        default:
          LOG_ERROR ("Unknown DXF code %d for %s", pair->code,
                     "THUMBNAILIMAGE");
          break;
        }
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
      if (!pair)
        break;
    }
  dxf_free_pair (pair);
  return 0;
}

// read to AcDs, TODO
// SECTION ACDSDATA: 0 ACDSSCHEMA, 0 ACDSRECORD
static int
dxf_acds_read (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  // Dxf_Pair *pair = dxf_read_pair (dat);
  // unsigned written = 0;
  LOG_WARN ("SECTION %s ignored for now", "ACDSDATA");
  dxf_unknownsection_read (dat, dwg);
  return 0;
}

static void
resolve_postponed_header_refs (Dwg_Data *restrict dwg)
{
  Dwg_Header_Variables *vars = &dwg->header_vars;
  uint32_t i;
  LOG_TRACE ("resolve %d postponed header ref names:\n", header_hdls->nitems);
  for (i = 0; i < header_hdls->nitems; i++)
    {
      char *field = header_hdls->items[i].field;
      Dxf_Pair p = { 0, DWG_VT_STRING, { 0 } };
      BITCODE_H hdl = NULL;
      p.value.s = header_hdls->items[i].name;
      if (!p.value.s || !*p.value.s)
        {
          LOG_WARN ("HEADER.%s empty dxf:%d", field, (int)p.code);
          continue;
        }
      p.code = header_hdls->items[i].code;
      if (strEQc (field, "DIMSTYLE"))
        p.code = 3; // special handling in find_tablehandle()
      else if (strstr (field, "UCS"))
        p.code = 345;
      hdl = find_tablehandle (dwg, &p);
      if (hdl)
        {
          if (hdl->handleref.code != 5)
            hdl = dwg_add_handleref (dwg, 5, hdl->absolute_ref, NULL);
          dwg_dynapi_header_set_value (dwg, field, &hdl, 1);
          LOG_TRACE ("HEADER.%s %s => " FORMAT_REF " [H %d]\n", field,
                     p.value.s, ARGS_REF (hdl), (int)p.code);
        }
      else if (strEQc (field, "CMLSTYLE"))
        {
          hdl = dwg_find_tablehandle_silent (dwg, p.value.s, "MLINESTYLE");
          if (hdl)
            {
              if (hdl->handleref.code != 5)
                hdl = dwg_add_handleref (dwg, 5, hdl->absolute_ref, NULL);
              dwg_dynapi_header_set_value (dwg, field, &hdl, 1);
              LOG_TRACE ("HEADER.%s %s => " FORMAT_REF " [H %d]\n", field,
                         p.value.s, ARGS_REF (hdl), (int)p.code)
            }
          else
            LOG_WARN ("Unknown HEADER.%s %s dxf:%d", field, p.value.s,
                      (int)p.code)
        }
      else
        LOG_WARN ("Unknown HEADER.%s %s dxf:%d", field, p.value.s, (int)p.code)
    }
}

// i.e. layer or block name
static void
resolve_postponed_object_refs (Dwg_Data *restrict dwg)
{
  uint32_t i;
  LOG_TRACE ("resolve %d postponed object ref names:\n", obj_hdls->nitems);
  for (i = 0; i < obj_hdls->nitems; i++)
    {
      char *field = obj_hdls->items[i].field;
      Dxf_Pair p = { 0, DWG_VT_STRING, { 0 } };
      BITCODE_H hdl = NULL;
      int objid = obj_hdls->items[i].code;
      Dwg_Object *obj = &dwg->object[objid];
      int is_entity = obj->supertype == DWG_SUPERTYPE_ENTITY;

      p.value.s = obj_hdls->items[i].name;
      if (!p.value.s || !*p.value.s)
        {
          LOG_WARN ("%s.%s empty", obj->name, field);
          continue;
        }
      // TODO find field type => dxf code
      if (strEQc (field, "block_header"))
        hdl = dwg_find_tablehandle_silent (dwg, p.value.s, "BLOCK");
      else if (strEQc (field, "style"))
        {
          if (is_entity)
            p.code = 7;
          hdl = dwg_find_tablehandle_silent (dwg, p.value.s, "STYLE");
        }
      else if (strEQc (field, "dimstyle"))
        {
          if (is_entity)
            p.code = 3;
          hdl = dwg_find_tablehandle_silent (dwg, p.value.s, "DIMSTYLE");
        }
      else if (strEQc (field, "layer"))
        {
          if (is_entity)
            p.code = 8;
          hdl = find_tablehandle (dwg, &p);
        }
      else if (strEQc (field, "ltype"))
        {
          if (is_entity)
            p.code = 6;
          hdl = find_tablehandle (dwg, &p);
        }
      else if (strEQc (field, "material")
               && (is_entity || obj->fixedtype == DWG_TYPE_LAYER))
        {
          if (is_entity)
            p.code = 347;
          hdl = dwg_find_tablehandle_silent (dwg, p.value.s, "MATERIAL");
        }
      else if (is_entity && strEQc (field, "shadow"))
        {
          p.code = 361;
          hdl = dwg_find_tablehandle_silent (dwg, p.value.s, "SHADOW");
        }
      else if (strEQc (field, "plotstyle")
               && (is_entity || obj->fixedtype == DWG_TYPE_LAYER))
        {
          if (is_entity)
            p.code = 390;
          hdl = dwg_find_tablehandle_silent (dwg, p.value.s, "PLOTSTYLENAME");
        }
      else if (is_entity && strEQc (field, "full_visualstyle"))
        {
          p.code = 348;
          hdl = dwg_find_tablehandle_silent (dwg, p.value.s, "VISUALSTYLE");
        }
      else if (is_entity && strEQc (field, "face_visualstyle"))
        {
          p.code = 348;
          hdl = dwg_find_tablehandle_silent (dwg, p.value.s, "VISUALSTYLE");
        }
      else if (is_entity && strEQc (field, "edge_visualstyle"))
        {
          p.code = 348;
          hdl = dwg_find_tablehandle_silent (dwg, p.value.s, "VISUALSTYLE");
        }
      else if (obj->fixedtype == DWG_TYPE_LAYER
               && strEQc (field, "visualstyle"))
        {
          hdl = dwg_find_tablehandle_silent (dwg, p.value.s, "VISUALSTYLE");
        }
      // TODO: check if DXF by name:
      // background, named_ucs, base_ucs, shadeplot, sun, livesection (VIEW,
      // VIEWPORT)
      else
        LOG_WARN ("missing code for %s", field)
      if (hdl)
        {
          Dwg_Object_APPID *_obj = obj->tio.object->tio.APPID;
          if (hdl->handleref.code != 5)
            hdl = dwg_add_handleref (dwg, 5, hdl->absolute_ref, NULL);
          if (p.code > 0)
            dwg_dynapi_common_set_value (_obj, field, &hdl, 0);
          else
            dwg_dynapi_entity_set_value (_obj, obj->name, field, &hdl, 0);
          LOG_TRACE ("%s.%s %s => " FORMAT_REF " [H %d]\n", obj->name, field,
                     p.value.s, ARGS_REF (hdl), (int)p.code);
        }
      else
        LOG_WARN ("Unknown %s.%s %s", obj->name, field, p.value.s)
    }
}

#  define CHECK_DICTIONARY_HDR(name)                                          \
    if (!vars->DICTIONARY_##name)                                             \
      {                                                                       \
        vars->DICTIONARY_##name = dwg_find_dictionary (dwg, #name);           \
        if (vars->DICTIONARY_##name)                                          \
          LOG_TRACE ("HEADER.DICTIONARY_" #name " = " FORMAT_REF "\n",        \
                     ARGS_REF (vars->DICTIONARY_##name))                      \
        else if ((vars->DICTIONARY_##name                                     \
                  = dwg_find_dictionary (dwg, "ACAD_" #name)))                \
          LOG_TRACE ("HEADER.DICTIONARY_" #name " = " FORMAT_REF "\n",        \
                     ARGS_REF (vars->DICTIONARY_##name));                     \
      }                                                                       \
    /* set owner to NOD 4.1.C */                                              \
    obj = dwg_ref_object (dwg, vars->DICTIONARY_##name);                      \
    if (obj && obj->tio.object && obj->fixedtype == DWG_TYPE_DICTIONARY)      \
      {                                                                       \
        Dwg_Object_Object *_obj = obj->tio.object;                            \
        if (!_obj->ownerhandle || !_obj->ownerhandle->absolute_ref)           \
          {                                                                   \
            Dwg_Object *nod                                                   \
                = dwg_get_first_object (dwg, DWG_TYPE_DICTIONARY);            \
            BITCODE_RLL nod_hdl = nod ? nod->handle.value : UINT64_C (0xC);   \
            bool is_abs = obj->handle.value >= 0x19;                          \
            _obj->ownerhandle                                                 \
                = dwg_add_handleref (dwg, 4, nod_hdl, is_abs ? NULL : obj);   \
          }                                                                   \
      }

static void
resolve_header_dicts (Dwg_Data *restrict dwg)
{
  Dwg_Header_Variables *vars = &dwg->header_vars;
  Dwg_Object *obj;

  if (!vars->DICTIONARY_NAMED_OBJECT)
    {
      Dwg_Object_DICTIONARY *nod;
      BITCODE_RLL hdl = UINT64_C(0xC);
      obj = dwg_get_first_object (dwg, DWG_TYPE_DICTIONARY); // the NOD
      if (!obj)
        {
          int error;
          nod = dwg_add_DICTIONARY (dwg, NULL, (const BITCODE_T) "NAMED_OBJECT",
                                    0UL);
          obj = dwg_obj_generic_to_object (nod, &error);
          if (obj)
            hdl = obj->handle.value;
        }
      else
        hdl = obj->handle.value;
      vars->DICTIONARY_NAMED_OBJECT = dwg_add_handleref (dwg, 3, hdl, NULL);
    }
  // only possible after OBJECTS
  CHECK_DICTIONARY_HDR (ACAD_GROUP)
  CHECK_DICTIONARY_HDR (ACAD_MLINESTYLE)
  if (dwg->header.from_version >= R_2000)
    {
      CHECK_DICTIONARY_HDR (LAYOUT)
      CHECK_DICTIONARY_HDR (PLOTSETTINGS)
      CHECK_DICTIONARY_HDR (PLOTSTYLENAME)
    }
  if (dwg->header.from_version >= R_2004)
    {
      CHECK_DICTIONARY_HDR (MATERIAL)
      CHECK_DICTIONARY_HDR (COLOR)
    }
  if (dwg->header.from_version >= R_2007)
    {
      CHECK_DICTIONARY_HDR (VISUALSTYLE)
    }
}
#  undef CHECK_DICTIONARY_HDR

static void
resolve_postponed_eed_refs (Dwg_Data *restrict dwg)
{
  LOG_TRACE ("resolve %d postponed eed APPID refs\n", eed_hdls->nitems);
  for (uint32_t i = 0; i < eed_hdls->nitems; i++)
    {
      char *name = eed_hdls->items[i].name;
      BITCODE_H ref
          = dwg_find_tablehandle (dwg, eed_hdls->items[i].name, "APPID");
      if (ref)
        {
          // copy to eed[i].handle. need: objid + eed[i]
          BITCODE_RL objid;
          Dwg_Eed *eed;
          int j = (int)eed_hdls->items[i].code;
          sscanf (eed_hdls->items[i].field, "%d", &objid);
          if (objid <= 0 || objid >= dwg->num_objects)
            {
              LOG_ERROR ("Skip invalid postponed eed APPID %s",
                         eed_hdls->items[i].field);
              continue;
            }
          eed = dwg->object[objid].tio.object->eed;
          if (!eed || j >= (int)dwg->object[objid].tio.object->num_eed)
            {
              LOG_ERROR ("Skip invalid postponed APPID eed[%d]", j);
              continue;
            }
          memcpy (&eed[j].handle, &ref->handleref, sizeof (Dwg_Handle));
          eed[j].handle.code = 5;
          LOG_TRACE (
              "postponed %s[%d]->eed[%d].handle for APPID.%s => " FORMAT_H
              " [H]\n",
              dwg->object[objid].name, objid, j, name, ARGS_H (eed[j].handle));
        }
      else
        LOG_WARN ("Unknown eed[].handle for APPID.%s", name)
    }
}

EXPORT int
dwg_read_dxf (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  // const int minimal = dwg->opts & DWG_OPTS_MINIMAL;
  Dxf_Pair *pair = NULL;
  int error = 0;

  loglevel = dwg->opts & DWG_OPTS_LOGLEVEL;
  if (!dat->chain && dat->fh)
    {
      error = dat_read_stream (dat, dat->fh);
      if (error >= DWG_ERR_CRITICAL)
        return error;
      if (dat->size >= 22
          && !memcmp (dat->chain, "AutoCAD Binary DXF",
                      sizeof ("AutoCAD Binary DXF") - 1))
        {
          dat->opts |= DWG_OPTS_DXFB;
          dat->byte = 22;
        }
    }
  if (dat->size < 256)
    {
      LOG_ERROR ("DXF input too small, %" PRIuSIZE " byte.\n", dat->size);
      return DWG_ERR_IOERROR;
    }
  /* Fail early on DWG */
  if (!memcmp (dat->chain, "AC10", 4) || !memcmp (dat->chain, "AC1.", 4)
      || !memcmp (dat->chain, "AC2.10", 4) || !memcmp (dat->chain, "MC0.0", 4))
    {
      LOG_ERROR ("This is a DWG, not a DXF\n");
      return DWG_ERR_INVALIDDWG;
    }
  dat->opts |= DWG_OPTS_INDXF;
  dwg->opts |= DWG_OPTS_INDXF;
  // num_dxf_objs = 0;
  // size_dxf_objs = 1000;
  // dxf_objs = (Dxf_Objs *)malloc (1000 * sizeof (Dxf_Objs));
  if (!dwg->object_map)
    dwg->object_map = hash_new (dat->size / 1000);
  // cannot rely on ref->obj during realloc's
  dwg->dirty_refs = 1;

  header_hdls = new_array_hdls (16);
  eed_hdls = new_array_hdls (16);
  obj_hdls = new_array_hdls (16);

  // start with the BLOCK_HEADER at objid 0
  if (!dwg->num_objects)
    {
      Dwg_Object *obj;
      Dwg_Object_BLOCK_HEADER *_obj;
      char *dxfname = strdup ((char *)"BLOCK_HEADER");
      NEW_OBJECT (dwg, obj);
      ADD_OBJECT1 (BLOCK_HEADER, BLOCK_HEADER);
      // dwg->header.version probably here still unknown. <r2000: 0x17
      // later fixed up when reading $ACADVER and the BLOCK_HEADER.name
      _obj->name = dwg_add_u8_input (dwg, "*Model_Space");
      LOG_TRACE ("%s.name = %s [TV 2]\n", dxfname, _obj->name);
      _obj->is_xref_ref = 1;
      obj->tio.object->is_xdic_missing = 1;
      dwg_add_handle (&obj->handle, 0, 0x1F, obj);
      LOG_TRACE ("%s.handle = (0.%d." FORMAT_HV ")\n", obj->name,
                 obj->handle.size, obj->handle.value);
      obj->tio.object->ownerhandle = dwg_add_handleref (dwg, 4, 1, NULL);
      LOG_TRACE ("%s.ownerhandle = " FORMAT_REF " [H 330]\n", dxfname,
                 ARGS_REF (obj->tio.object->ownerhandle));
    }

  while (dat->byte < dat->size)
    {
      pair = dxf_read_pair (dat);
      DXF_BREAK_EOF;
      pair = dxf_expect_code (dat, pair, 0);
      DXF_BREAK_EOF;
      if (pair->type == DWG_VT_STRING && strEQc (pair->value.s, "SECTION"))
        {
          dxf_free_pair (pair);
          pair = dxf_read_pair (dat);
          DXF_BREAK_EOF;
          pair = dxf_expect_code (dat, pair, 2);
          DXF_BREAK_EOF;
          if (!pair->value.s)
            {
              LOG_ERROR ("Expected SECTION string code 2, got code %d",
                         pair->code);
              dxf_free_pair (pair);
              pair = NULL;
              break;
            }
          else if (strEQc (pair->value.s, "HEADER"))
            {
              dxf_free_pair (pair);
              pair = NULL;
              error = dxf_header_read (dat, dwg);
              if (error > DWG_ERR_CRITICAL)
                goto error;
              dxf_fixup_header (dat, dwg);
              // skip minimal DXF
              /*
              if (!dwg->header_vars.DIMPOST) // T in all versions
                {
                  LOG_ERROR ("Unsupported minimal DXF");
                  return DWG_ERR_INVALIDDWG;
                }
              */
            }
          else if (strEQc (pair->value.s, "CLASSES"))
            {
              dxf_free_pair (pair);
              pair = NULL;
              if (dwg->header.from_version == R_INVALID)
                dxf_fixup_header (dat, dwg);
              error = dxf_classes_read (dat, dwg);
              if (error > DWG_ERR_CRITICAL)
                return error;
            }
          else if (strEQc (pair->value.s, "TABLES"))
            {
              BITCODE_H hdl;
              dxf_free_pair (pair);
              pair = NULL;
              error = dxf_tables_read (dat, dwg);
              if (error > DWG_ERR_CRITICAL)
                goto error;

              resolve_postponed_header_refs (dwg);
              resolve_postponed_eed_refs (dwg);
              if (dwg->header.from_version == R_INVALID)
                dxf_fixup_header (dat, dwg);

              // should not happen
              if (!dwg->header_vars.LTYPE_BYLAYER
                  && (hdl = dwg_find_tablehandle_silent (
                          dwg, (char *)"ByLayer", "LTYPE")))
                dwg->header_vars.LTYPE_BYLAYER
                    = dwg_add_handleref (dwg, 5, hdl->handleref.value, NULL);
              // should not happen
              if (!dwg->header_vars.LTYPE_BYBLOCK
                  && (hdl = dwg_find_tablehandle_silent (
                          dwg, (char *)"ByBlock", "LTYPE")))
                dwg->header_vars.LTYPE_BYBLOCK
                    = dwg_add_handleref (dwg, 5, hdl->handleref.value, NULL);
              // but this is needed
              if (!dwg->header_vars.LTYPE_CONTINUOUS
                  && (hdl = dwg_find_tablehandle_silent (
                          dwg, (char *)"Continuous", "LTYPE")))
                dwg->header_vars.LTYPE_CONTINUOUS
                    = dwg_add_handleref (dwg, 5, hdl->handleref.value, NULL);
            }
          else if (strEQc (pair->value.s, "BLOCKS"))
            {
              BITCODE_H hdl;
              dxf_free_pair (pair);
              pair = NULL;
              if (dwg->header.from_version == R_INVALID)
                dxf_fixup_header (dat, dwg);
              error = dxf_blocks_read (dat, dwg);
              if (error > DWG_ERR_CRITICAL)
                goto error;

              // resolve_postponed_header_refs (dwg);
              if (!dwg->header_vars.BLOCK_RECORD_PSPACE
                  && (hdl = dwg_find_tablehandle_silent (
                          dwg, (char *)"*Paper_Space", "BLOCK")))
                dwg->header_vars.BLOCK_RECORD_PSPACE
                    = dwg_add_handleref (dwg, 5, hdl->handleref.value, NULL);
              if (!dwg->header_vars.BLOCK_RECORD_MSPACE
                  && (hdl = dwg_find_tablehandle_silent (
                          dwg, (char *)"*Model_Space", "BLOCK")))
                dwg->header_vars.BLOCK_RECORD_MSPACE
                    = dwg_add_handleref (dwg, 5, hdl->handleref.value, NULL);
            }
          else if (strEQc (pair->value.s, "ENTITIES"))
            {
              dxf_free_pair (pair);
              pair = NULL;
              resolve_postponed_object_refs (dwg);
              error = dxf_entities_read (dat, dwg);
              if (error > DWG_ERR_CRITICAL)
                goto error;
            }
          else if (strEQc (pair->value.s, "OBJECTS"))
            {
              dxf_free_pair (pair);
              pair = NULL;
              error = dxf_objects_read (dat, dwg);
              if (error > DWG_ERR_CRITICAL)
                goto error;
              resolve_header_dicts (dwg);
            }
          else if (strEQc (pair->value.s, "THUMBNAILIMAGE"))
            {
              dxf_free_pair (pair);
              pair = NULL;
              error = dxf_thumbnail_read (dat, dwg);
            }
          else if (strEQc (pair->value.s, "ACDSDATA"))
            {
              dxf_free_pair (pair);
              pair = NULL;
              error = dxf_acds_read (dat, dwg);
            }
          else // if (strEQc (pair->value.s, ""))
            {
              LOG_WARN ("SECTION %s ignored for now", pair->value.s);
              dxf_free_pair (pair);
              pair = NULL;
              error = dxf_unknownsection_read (dat, dwg);
            }
        }
      else
        {
          LOG_ERROR ("Expected string SECTION");
          dxf_free_pair (pair);
        }
    }
  if (pair != NULL && pair->code == 0 && pair->value.s != NULL
      && strEQc (pair->value.s, "EOF"))
    ;
  else if (dat->byte >= dat->size || (pair == NULL))
    error |= DWG_ERR_IOERROR;
  dxf_free_pair (pair);
  resolve_postponed_header_refs (dwg);
  resolve_postponed_object_refs (dwg);
  LOG_HANDLE ("Resolving pointers from ObjectRef vector:\n");
  dwg_resolve_objectrefs_silent (dwg);
  free_array_hdls (header_hdls);
  free_array_hdls (eed_hdls);
  free_array_hdls (obj_hdls);
  if (dwg->header.version <= R_2000 && dwg->header.from_version > R_2000)
    dwg_fixup_BLOCKS_entities (dwg);
  LOG_TRACE ("import from DXF\n");
  if (error > DWG_ERR_CRITICAL)
    return error;
  else
    return dwg->num_objects ? 1 : 0;

error:
  dwg->dirty_refs = 0;
  free_array_hdls (header_hdls);
  free_array_hdls (eed_hdls);
  free_array_hdls (obj_hdls);
  return error;
}

EXPORT int
dwg_read_dxfb (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  dwg->opts |= DWG_OPTS_DXFB; // binary
  dat->opts |= DWG_OPTS_DXFB;
  if (dat->size >= 22 && dat->byte < 22
      && !memcmp (dat->chain, "AutoCAD Binary DXF",
                  sizeof ("AutoCAD Binary DXF") - 1))
    {
      dat->byte = 22;
    }
  return dwg_read_dxf (dat, dwg);
}

#endif // DISABLE_DXF

#undef IS_INDXF
