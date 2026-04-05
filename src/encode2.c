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
 * encode2.c: second part of encoding functions (dwg2.spec entities/objects)
 * split from encode.c to reduce compile times.
 * written by Reini Urban
 */

#define HAVE_COMPRESS_R2004_SECTION
#define ENCODE_PATCH_RSSIZE

#include "config.h"
#ifdef __STDC_ALLOC_LIB__
#  define __STDC_WANT_LIB_EXT2__ 1 /* for strdup */
#else
#  define _USE_BSD 1
#endif
#ifndef _BSD_SOURCE
#  define _BSD_SOURCE 1
#endif
#ifndef _DEFAULT_SOURCE
#  define _DEFAULT_SOURCE 1
#endif
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#if defined HAVE_CTYPE_H || defined _MSC_VER
#  include <ctype.h>
#endif
#ifdef HAVE_WCTYPE_H
#  include <wctype.h>
#endif

#define DWG_MAX_OBJSIZE 0xf00000

#include "common.h"
#include "bits.h"
#include "dwg.h"
#include "encode.h"
#include "decode.h"
#include "classes.h"
#include "dynapi.h"
#include "free.h"

/* the current version per spec block */
static Dwg_Version_Type cur_ver = R_INVALID;
static BITCODE_BL rcount1 = 0, rcount2 = 0;

#define DWG_LOGLEVEL loglevel

#include "logging.h"

/*--------------------------------------------------------------------------------
 * spec MACROS - same as encode.c
 */

#define ACTION encode
#define IS_ENCODER

#include "enc_macros.h"

/* Forward declarations for non-static functions generated below */
GCC46_DIAG_IGNORE (-Wredundant-decls)
#define DWG_ENTITY(token)                                                     \
  int dwg_encode_##token (Bit_Chain *restrict dat,                            \
                          Dwg_Object *restrict obj);
#define DWG_ENTITY_END
#define DWG_OBJECT(token)                                                     \
  int dwg_encode_##token (Bit_Chain *restrict dat,                            \
                          Dwg_Object *restrict obj);
#define DWG_OBJECT_END
#include "objects.inc"
GCC46_DIAG_RESTORE
#undef DWG_ENTITY
#undef DWG_ENTITY_END
#undef DWG_OBJECT
#undef DWG_OBJECT_END

// clang-format off
#define DWG_ENTITY(token)                                                     \
  static int dwg_encode_##token##_private (                                   \
      Bit_Chain *dat, Bit_Chain *hdl_dat, Bit_Chain *str_dat,                 \
      Dwg_Object *restrict obj);                                              \
  int dwg_encode_##token (Bit_Chain *restrict dat,                            \
                          Dwg_Object *restrict obj)                           \
  {                                                                           \
    int error;                                                                \
    Bit_Chain _hdl_dat = { 0 };                                               \
    Bit_Chain _str_dat = { 0 };                                               \
    Bit_Chain *hdl_dat = &_hdl_dat; /* a new copy */                          \
    Bit_Chain *str_dat;                                                       \
    LOG_INFO ("Encode entity " #token "\n");                                  \
    bit_chain_init_dat (hdl_dat, 128, dat);                                   \
    if (dat->version >= R_2007) {                                             \
      bit_chain_init_dat (&_str_dat, 128, dat);                               \
      str_dat = &_str_dat;                                                    \
    } else {                                                                  \
      str_dat = dat;                                                          \
    }                                                                         \
    error = dwg_encode_entity (obj, dat, hdl_dat, str_dat);                   \
    if (error)                                                                \
      {                                                                       \
        LOG_HANDLE ("Early DWG_ENTITY exit\n");                               \
        if (hdl_dat != dat && hdl_dat->chain != dat->chain)                   \
          bit_chain_free (hdl_dat);                                           \
        if (str_dat != dat && str_dat->chain)                                 \
          bit_chain_free (str_dat);                                           \
        return error;                                                         \
      }                                                                       \
    error = dwg_encode_##token##_private (dat, hdl_dat, str_dat, obj);        \
    if (error & DWG_ERR_VALUEOUTOFBOUNDS && hdl_dat != dat                    \
        && hdl_dat->chain != dat->chain)                                      \
      {                                                                       \
        LOG_HANDLE ("VALUEOUTOFBOUNDS bypassed DWG_ENTITY_END\n");            \
        /* bit_chain_free (hdl_dat); */                                       \
      }                                                                       \
    dwg_encode_unknown_rest (dat, obj);                                       \
    return error;                                                             \
  }                                                                           \
  static int dwg_encode_##token##_private (                                   \
      Bit_Chain *dat, Bit_Chain *hdl_dat, Bit_Chain *str_dat,                 \
      Dwg_Object *restrict obj)                                               \
  {                                                                           \
    int error = 0;                                                            \
    BITCODE_BL vcount, rcount3, rcount4;                                      \
    Dwg_Data *dwg = obj->parent;                                              \
    Dwg_Object_Entity *_ent = obj->tio.entity;                                \
    Dwg_Entity_##token *_obj = _ent->tio.token;
// clang-format on

#define DWG_ENTITY_END                                                        \
  if (hdl_dat->byte > dat->byte)                                              \
    {                                                                         \
      dat->byte = hdl_dat->byte;                                              \
      dat->bit = hdl_dat->bit;                                                \
    }                                                                         \
  if (hdl_dat != dat && hdl_dat->chain != dat->chain)                         \
    bit_chain_free (hdl_dat);                                                 \
  if (str_dat != dat && str_dat->chain)                                       \
    bit_chain_free (str_dat);                                                 \
  return error;                                                               \
  }

/** Returns -1 if not added, else returns the new objid.
   Does a complete handleref rescan to invalidate and resolve
   all internal obj pointers after a object[] realloc.
*/
#define DWG_OBJECT(token)                                                     \
  static int dwg_encode_##token##_private (                                   \
      Bit_Chain *dat, Bit_Chain *hdl_dat, Bit_Chain *str_dat,                 \
      Dwg_Object *restrict obj);                                              \
  int dwg_encode_##token (Bit_Chain *restrict dat,                            \
                          Dwg_Object *restrict obj)                           \
  {                                                                           \
    int error;                                                                \
    Bit_Chain _hdl_dat = { 0 };                                               \
    Bit_Chain _str_dat = { 0 };                                               \
    Bit_Chain *hdl_dat = &_hdl_dat; /* a new copy */                          \
    Bit_Chain *str_dat;                                                       \
    LOG_INFO ("Encode object " #token "\n");                                  \
    bit_chain_init_dat (hdl_dat, 128, dat);                                   \
    if (dat->version >= R_2007) {                                             \
      bit_chain_init_dat (&_str_dat, 128, dat);                               \
      str_dat = &_str_dat;                                                    \
    } else {                                                                  \
      str_dat = dat;                                                          \
    }                                                                         \
    error = dwg_encode_object (obj, dat, hdl_dat, str_dat);                   \
    if (error)                                                                \
      {                                                                       \
        if (hdl_dat != dat)                                                   \
          bit_chain_free (hdl_dat);                                           \
        if (str_dat != dat && str_dat->chain)                                 \
          bit_chain_free (str_dat);                                           \
        return error;                                                         \
      }                                                                       \
    error = dwg_encode_##token##_private (dat, hdl_dat, str_dat, obj);        \
    if (error & DWG_ERR_VALUEOUTOFBOUNDS && hdl_dat != dat                    \
        && hdl_dat->chain != dat->chain)                                      \
      bit_chain_free (hdl_dat);                                               \
    if (str_dat != dat && str_dat->chain)                                     \
      bit_chain_free (str_dat);                                               \
    dwg_encode_unknown_rest (dat, obj);                                       \
    return error;                                                             \
  }                                                                           \
  static int dwg_encode_##token##_private (                                   \
      Bit_Chain *dat, Bit_Chain *hdl_dat, Bit_Chain *str_dat,                 \
      Dwg_Object *restrict obj)                                               \
  {                                                                           \
    int error = 0;                                                            \
    BITCODE_BL vcount, rcount3, rcount4;                                      \
    Dwg_Data *dwg = obj->parent;                                              \
    Dwg_Object_##token *_obj = obj->tio.object->tio.token;

// some objects specs forgot about the common streams, so add it here
#define DWG_OBJECT_END                                                        \
  if (!obj->hdlpos)                                                           \
    {                                                                         \
      START_OBJECT_HANDLE_STREAM                                              \
    }                                                                         \
  if (hdl_dat->byte > dat->byte)                                              \
    {                                                                         \
      dat->byte = hdl_dat->byte;                                              \
      dat->bit = hdl_dat->bit;                                                \
    }                                                                         \
  if (hdl_dat != dat && hdl_dat->chain != dat->chain)                         \
    bit_chain_free (hdl_dat);                                                 \
  if (str_dat != dat && str_dat->chain)                                       \
    bit_chain_free (str_dat);                                                 \
  return error;                                                               \
  }

#define HANDLE_STREAM_ERROR_CLEANUP
#include "dwg2.spec"
#undef HANDLE_STREAM_ERROR_CLEANUP
// clang-format on
