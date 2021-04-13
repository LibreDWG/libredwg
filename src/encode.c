/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2009-2010,2018-2026 Free Software Foundation, Inc.         */
/*  Copyright (C) 2010 Thien-Thi Nguyen                                      */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * encode.c: encoding functions to write a DWG
 * written by Felipe Castro
 * modified by Felipe Corrêa da Silva Sances
 * modified by Rodrigo Rodrigues da Silva
 * modified by Thien-Thi Nguyen
 * modified by Till Heuschmann
 * modified by Anderson Pierre Cardoso
 * modified and rewritten by Reini Urban
 */

#define HAVE_COMPRESS_R2004_SECTION
#define ENCODE_PATCH_RSSIZE

#include "config.h"
#ifdef __STDC_ALLOC_LIB__
#  define __STDC_WANT_LIB_EXT2__ 1 /* for strdup */
#else
#  define _USE_BSD 1
#endif
#ifndef _DEFAULT_SOURCE
#  define _DEFAULT_SOURCE 1 /* for__USE_MISC byteswap.h */
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

// from dwg_api
BITCODE_T dwg_add_u8_input (Dwg_Data *restrict dwg,
                            const char *restrict u8str);
Dwg_Object_APPID *dwg_add_APPID (Dwg_Data *restrict dwg,
                                 const char *restrict name);
Dwg_Object_VX_TABLE_RECORD *dwg_add_VX (Dwg_Data *restrict dwg,
                                        const char *restrict name);
BITCODE_RLL dwg_obj_generic_handlevalue (void *_obj);
Dwg_Object *dwg_obj_generic_to_object (const void *restrict obj,
                                       int *restrict error);

static int encode_preR13_section (const Dwg_Section_Type_r11 id,
                                  Bit_Chain *restrict dat,
                                  Dwg_Data *restrict dwg);
// static void downconvert_relative_handle (BITCODE_H handle,
//                                          Dwg_Object *restrict obj);
void dwg_downgrade_MLINESTYLE (Dwg_Object_MLINESTYLE *o);
void dwg_upgrade_MLINESTYLE (Dwg_Data *restrict dwg,
                             Dwg_Object_MLINESTYLE *restrict o);

/* The logging level for the write (encode) path.  */
static unsigned int loglevel;
/* the current version per spec block */
static Dwg_Version_Type cur_ver = R_INVALID;
static BITCODE_BL rcount1 = 0, rcount2 = 0;

/* section_order: A static array of section types.
   SECTION_R13_SIZE is the size and the sentinel.
 */
#define SECTION_R13_SIZE 7U
static Dwg_Section_Type_r13 section_order[SECTION_R13_SIZE]
#ifndef __cplusplus
    = { 0 }
#endif
;

#ifdef USE_TRACING
/* This flag means we have checked the environment variable
   LIBREDWG_TRACE and set `loglevel' appropriately.  */
static bool env_var_checked_p;
#endif /* USE_TRACING */
#define DWG_LOGLEVEL loglevel

#include "logging.h"

/*--------------------------------------------------------------------------------
 * spec MACROS
 */

#define ACTION encode
#define IS_ENCODER

#include "enc_macros.h"

// clang-format off
#define DWG_ENTITY(token)                                                     \
  static int dwg_encode_##token##_private (                                   \
      Bit_Chain *dat, Bit_Chain *hdl_dat, Bit_Chain *str_dat,                 \
      Dwg_Object *restrict obj);                                              \
  static int dwg_encode_##token (Bit_Chain *restrict dat,                     \
                                 Dwg_Object *restrict obj)                    \
  {                                                                           \
    int error;                                                                \
    Bit_Chain _hdl_dat = { 0 };                                               \
    Bit_Chain *hdl_dat = &_hdl_dat; /* a new copy */                          \
    Bit_Chain *str_dat = dat; /* a ref */                               \
    LOG_INFO ("Encode entity " #token "\n");                                  \
    bit_chain_init_dat (hdl_dat, 128, dat);                                   \
    error = dwg_encode_entity (obj, dat, hdl_dat, str_dat);                   \
    if (error)                                                                \
      {                                                                       \
        LOG_HANDLE ("Early DWG_ENTITY exit\n");                               \
        if (hdl_dat != dat && hdl_dat->chain != dat->chain)                   \
          bit_chain_free (hdl_dat);                                           \
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
  static int dwg_encode_##token (Bit_Chain *restrict dat,                     \
                                 Dwg_Object *restrict obj)                    \
  {                                                                           \
    int error;                                                                \
    Bit_Chain _hdl_dat = { 0 };                                               \
    Bit_Chain *hdl_dat = &_hdl_dat; /* a new copy */                          \
    Bit_Chain *str_dat = dat; /* a ref */                               \
    LOG_INFO ("Encode object " #token "\n");                                  \
    bit_chain_init_dat (hdl_dat, 128, dat);                                   \
    error = dwg_encode_object (obj, dat, hdl_dat, str_dat);                   \
    if (error)                                                                \
      {                                                                       \
        if (hdl_dat != dat)                                                   \
          bit_chain_free (hdl_dat);                                           \
        return error;                                                         \
      }                                                                       \
    error = dwg_encode_##token##_private (dat, hdl_dat, str_dat, obj);        \
    if (error & DWG_ERR_VALUEOUTOFBOUNDS && hdl_dat != dat                    \
        && hdl_dat->chain != dat->chain)                                      \
      bit_chain_free (hdl_dat);                                               \
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
  return error;                                                               \
  }

#define ENT_REACTORS(code)                                                    \
  if (dat->version >= R_13b1 && _obj->num_reactors > 0x1000)                  \
    {                                                                         \
      LOG_ERROR ("Invalid num_reactors: %ld\n", (long)_obj->num_reactors);    \
      return DWG_ERR_VALUEOUTOFBOUNDS;                                        \
    }                                                                         \
  SINCE (R_13b1)                                                              \
  {                                                                           \
    if (_obj->num_reactors && !_obj->reactors)                                \
      {                                                                       \
        LOG_ERROR ("NULL entity.reactors");                                   \
        return DWG_ERR_VALUEOUTOFBOUNDS;                                      \
      }                                                                       \
    for (vcount = 0; vcount < _obj->num_reactors; vcount++)                   \
      {                                                                       \
        FIELD_HANDLE_N (reactors[vcount], vcount, code, 330);                 \
      }                                                                       \
  }

#undef DEBUG_POS
#define DEBUG_POS                                                             \
  if (DWG_LOGLEVEL >= DWG_LOGLEVEL_TRACE)                                     \
    {                                                                         \
      LOG_TRACE ("DEBUG_POS @%" PRIuSIZE ".%u / 0x%zx (%" PRIuSIZE ")\n",     \
                 dat->byte, dat->bit, dat->byte, bit_position (dat));         \
    }

/*--------------------------------------------------------------------------------*/
typedef struct
{
  BITCODE_RLL handle;
  size_t address;
  BITCODE_BL index;
} Object_Map;

/*--------------------------------------------------------------------------------
 * Private functions prototypes
 */
static BITCODE_RL encode_preR13_entities (EntitySectionIndexR11 section,
                                          Bit_Chain *restrict dat,
                                          Dwg_Data *restrict dwg,
                                          int *restrict error);
static int encode_preR13_header_variables (Bit_Chain *dat,
                                           Dwg_Data *restrict dwg);
static int dwg_encode_header_variables (Bit_Chain *dat, Bit_Chain *hdl_dat,
                                        Bit_Chain *str_dat,
                                        Dwg_Data *restrict dwg);
static int dwg_encode_variable_type (Dwg_Data *restrict dwg,
                                     Bit_Chain *restrict dat,
                                     Dwg_Object *restrict obj);
void dwg_encode_handleref (Bit_Chain *hdl_dat, Dwg_Object *restrict obj,
                           Dwg_Data *restrict dwg,
                           Dwg_Object_Ref *restrict ref);
void dwg_encode_handleref_with_code (Bit_Chain *hdl_dat,
                                     Dwg_Object *restrict obj,
                                     Dwg_Data *restrict dwg,
                                     Dwg_Object_Ref *restrict ref,
                                     unsigned int code);
int dwg_encode_add_object (Dwg_Object *restrict obj, Bit_Chain *restrict dat,
                           size_t address);

static BITCODE_RLL add_LibreDWG_APPID (Dwg_Data *dwg);
static BITCODE_BL add_DUMMY_eed (Dwg_Object *obj);
static void fixup_NOD (Dwg_Data *restrict dwg, Dwg_Object *restrict obj);
static void downconvert_MLEADERSTYLE (Dwg_Object *restrict obj);
static void downconvert_DIMSTYLE (Bit_Chain *restrict dat,
                                  Dwg_Object *restrict obj);

/* Imported */
BITCODE_H
dwg_find_tablehandle_silent (Dwg_Data *restrict dwg, const char *restrict name,
                             const char *restrict table);
void dwg_set_handle_size (Dwg_Handle *restrict hdl);

/*--------------------------------------------------------------------------------
 * Public functions
 */

#define write_sentinel(dat, sentinel_id)                                      \
  SINCE (R_11)                                                                \
  {                                                                           \
    bit_write_TF (dat, (BITCODE_TF)dwg_sentinel (sentinel_id), 16);           \
    LOG_TRACE (#sentinel_id " [16]");                                         \
    LOG_RPOS                                                                  \
  }

static BITCODE_RL
encode_patch_RLsize (Bit_Chain *dat, size_t size_adr)
{
  size_t pos;
  BITCODE_RL size;
  if (dat->bit) // padding
    {
      dat->bit = 0;
      dat->byte++;
    }
  assert (size_adr);
  if (size_adr < 4 || dat->byte < (size_adr - 4))
    return 0;
  size = (size_t)(dat->byte - size_adr - 4) & 0xFFFFFFFF; // minus the RL size
  if (size > dat->byte)                                   // 32-bit overflow
    return 0;
  pos = bit_position (dat);
  bit_set_position (dat, size_adr * 8);
  bit_write_RL (dat, size);
  LOG_TRACE ("size: " FORMAT_RL " [RL] @%" PRIuSIZE "\n", size, size_adr);
  bit_set_position (dat, pos);
  return size;
}

#ifdef ENCODE_PATCH_RSSIZE

static BITCODE_RS
encode_patch_RSsize (Bit_Chain *dat, size_t size_adr)
{
  size_t pos;
  BITCODE_RS size;
  if (dat->bit) // padding
    {
      dat->bit = 0;
      dat->byte++;
    }
  size = (dat->byte - size_adr) & 0xFFFF;
  pos = bit_position (dat);
  bit_set_position (dat, size_adr * 8);
  bit_write_RS_BE (dat, size);
  LOG_TRACE ("Size: " FORMAT_RS " [RS_BE] @%" PRIuSIZE "\n", size, size_adr);
  bit_set_position (dat, pos);
  return size;
}
#endif

/* if an error in this section should immediately return with a critical error,
 * like INVALIDDWG */
#if 0
static bool
is_section_critical (Dwg_Section_Type i)
{
  return (i == SECTION_OBJECTS || i == SECTION_HEADER || i == SECTION_CLASSES
          || i == SECTION_HANDLES) ? true : false;
}
#endif
static bool
is_section_r13_critical (Dwg_Section_Type_r13 i)
{
  return i <= SECTION_HANDLES_R13 ? true : false;
}

/* Limitations: */

/* when downconverting to r2000.
   fixes ODA Recovery error: AcDbRegAppTableRecord can't be cast to AcDbEntity
   GH #817 */
static void
remove_EXEMPT_FROM_CAD_STANDARDS_APPID (Bit_Chain *restrict dat,
                                        Dwg_Data *restrict dwg)
{
  BITCODE_H appid
      = dwg_find_tablehandle (dwg, "ACAD_EXEMPT_FROM_CAD_STANDARDS", "APPID");
  BITCODE_H appctl;
  Dwg_Object_APPID_CONTROL *_ctl;
  Dwg_Object *ctl, *obj;
  if (!appid)
    {
      LOG_INSANE ("APPID.EXEMPT_FROM_CAD_STANDARDS %s", "not found, good.\n");
      return;
    }
  if (!(appctl = dwg->header_vars.APPID_CONTROL_OBJECT))
    appctl = dwg_find_table_control (dwg, "APPID_CONTROL");
  if (!appctl)
    {
      LOG_ERROR ("APPID_CONTROL not found")
      return;
    }
  ctl = dwg_ref_object (dwg, appctl);
  if (!ctl || ctl->fixedtype != DWG_TYPE_APPID_CONTROL)
    {
      LOG_ERROR ("APPID_CONTROL not found")
      return;
    }
  _ctl = ctl->tio.object->tio.APPID_CONTROL;
  obj = dwg_ref_object (dwg, appid);
  if (!obj || obj->fixedtype != DWG_TYPE_APPID)
    {
      LOG_ERROR ("APPID.EXEMPT_FROM_CAD_STANDARDS %s", "not found");
    }
  else
    {
      LOG_TRACE ("APPID.EXEMPT_FROM_CAD_STANDARDS " FORMAT_REF " %s",
                 ARGS_REF (appid), "deleted\n");
      dwg_free_object (obj);
      // obj->fixedtype = DWG_TYPE_UNUSED;
      // obj->type = DWG_TYPE_UNUSED;
    }
  // removing appid from CONTROL HV
  for (BITCODE_BS i = 0; i < _ctl->num_entries; i++)
    {
      BITCODE_H ref = _ctl->entries[i];
      if (ref && ref->absolute_ref == appid->absolute_ref)
        {
          LOG_TRACE ("APPID_CONTROL[%u] removed " FORMAT_REF " [H*]\n", i,
                     ARGS_REF (ref));
          delete_hv (_ctl->entries, &_ctl->num_entries, i);
          return;
        }
    }
  // TODO: removing appid handle from all EED's
}

static BITCODE_RLL
add_LibreDWG_APPID (Dwg_Data *dwg)
{
  BITCODE_H appid = dwg_find_tablehandle (dwg, "LibreDWG", "APPID");
  Dwg_Object_APPID *_obj;

  if (appid)
    {
      LOG_INSANE ("APPID.LibreDWG found " FORMAT_HV "\n", appid->absolute_ref);
      return appid->absolute_ref;
    }
  LOG_INSANE ("no APPID.LibreDWG found\n");

  // This breaks json.test roundtrips tests as it adds a new object.
  // But sooner or later we want to delete yet unsupported objects
  // (Dictionaries, MATERIAL, VISUALSTYLE, dynblocks, surfaces, assoc*, ...)

  // add APPID (already searched above)
  _obj = dwg_add_APPID (dwg, "LibreDWG");
  return dwg_obj_generic_handlevalue (_obj);
}

static BITCODE_BL
add_DUMMY_eed (Dwg_Object *obj)
{
  Dwg_Object_Entity *ent = obj->tio.entity;
  // const int is_entity = obj->supertype == DWG_SUPERTYPE_ENTITY;
  const BITCODE_BL num_eed = ent->num_eed; // same offset for object
  Dwg_Data *dwg = obj->parent;
  char *name = obj->dxfname;
  BITCODE_H appid;
  Dwg_Eed_Data *data;
  size_t len;
  BITCODE_BS size;
  int i = 1, off = 0;
  const bool is_tu = dwg->header.version >= R_2007;

  // FIXME
#ifdef _WIN

  return 0;

#else

#  ifdef HAVE_STDDEF_H /* windows (mingw32,cygwin) not */
  assert (offsetof (Dwg_Object_Object, num_eed)
          == offsetof (Dwg_Object_Entity, num_eed));
  assert (offsetof (Dwg_Object_Object, eed)
          == offsetof (Dwg_Object_Entity, eed));
#  endif

  if (num_eed) // replace it
    dwg_free_eed (obj);
  appid = dwg_find_tablehandle_silent (dwg, "LibreDWG", "APPID");
  if (!appid)
    {
      LOG_WARN ("APPID LibreDWG not found, no EED added");
      ent->num_eed = 0;
      return 0;
    }
  ent->num_eed = 1;
  ent->eed = (Dwg_Eed *)CALLOC (2, sizeof (Dwg_Eed));
  len = (int)strlen (name);
  size = is_tu ? 1 + 2 + (((len + 1) & 0xFFFF) * 2) // RC + RS_BE + wstr
               : 1 + 3 + (len & 0xFF) + 1;          // RC + RC+RS + str
  data = ent->eed[0].data = (Dwg_Eed_Data *)CALLOC (size + 3, 1);
  ent->eed[0].size = size;
  dwg_add_handle (&ent->eed[0].handle, 5, appid->absolute_ref, NULL);
  data->code = 0; // RC
  if (is_tu)      // probably never used, write DUMMY placeholder to R_2007
    {
      BITCODE_TU wstr = bit_utf8_to_TU (name, 0);
      data->u.eed_0_r2007.is_tu = 1;
      data->u.eed_0_r2007.length = len & 0xFFFF; // RS
      memcpy (data->u.eed_0_r2007.string, wstr, len * 2);
    }
  else
    {
      data->u.eed_0.is_tu = 0;
      data->u.eed_0.length = len & 0xFF; // RC
      data->u.eed_0.codepage = 30;       // RS_BE
      memcpy (data->u.eed_0.string, name, len);
    }
  LOG_TRACE ("-EED[0]: code: 0, string: %s (len: %" PRIuSIZE ")\n", name, len);

  if (!obj->num_unknown_bits)
    return 1;
  // unknown_bits in chunks of 255 (length stored in RC)
  len = obj->num_unknown_bits / 8;
  if (obj->num_unknown_bits % 8)
    len++;
  // EED size is stored in a 16-bit field. Truncate overly large payloads.
  {
    const size_t max_eed_size = 0x7FFF;
    const size_t code0_size = size;
    size_t max_len = (size_t)len;
    size_t chunks = (max_len + 254) / 255;
    size_t total = code0_size + max_len + (2 * chunks);
    if (total > max_eed_size)
      {
        max_len
            = (max_eed_size > code0_size) ? (max_eed_size - code0_size) : 0;
        while (max_len)
          {
            chunks = (max_len + 254) / 255;
            total = code0_size + max_len + (2 * chunks);
            if (total <= max_eed_size)
              break;
            if (max_len > 255)
              max_len -= 255;
            else
              max_len = 0;
          }
        if (max_len < (size_t)len)
          {
            LOG_WARN ("EED payload too large (%zu bytes), truncating to %zu",
                      len, max_len);
            len = max_len;
            obj->num_unknown_bits = (BITCODE_BL)(max_len * 8);
          }
      }
  }
  size = (BITCODE_BS)((((len + 254) / 255) + 1) & 0xffff);
  if (size > 1) // we already reserved for two eeds
    {
      ent->eed = (Dwg_Eed *)REALLOC (ent->eed, (1 + size) * sizeof (Dwg_Eed));
      memset (&ent->eed[1], 0, size * sizeof (Dwg_Eed));
    }
  do
    {
      unsigned l = len > 255 ? 255 : (unsigned)len;
      ent->num_eed++;
      ent->eed[i].size = 0;
      ent->eed[0].size += l + 2;
      data = ent->eed[i].data = (Dwg_Eed_Data *)CALLOC (l + 2, 1);
      data->code = 4;           // RC
      data->u.eed_4.length = l; // also just an RC. max 256, how odd
      memcpy (data->u.eed_4.data, &obj->unknown_bits[off],
              data->u.eed_4.length);
      LOG_TRACE ("-EED[%d]: code: 4, unknown_bits: %u\n", i,
                 data->u.eed_4.length);
      if (len > 255)
        {
          len -= 255;
          off += 255;
          i++;
        }
      else
        break;
    }
  while (1);
  return i;
#endif
}

#ifdef ENCODE_UNKNOWN_AS_DUMMY

/** We cannot write unknown bits into another version. Also with indxf we don't
 * have that luxury. Write a DUMMY/PLACEHOLDER or POINT instead. Later maybe
 * PROXY. This leaks and is controversial. But it silences many ACAD import
 * warnings, and preserves information.
 */
static void
encode_unknown_as_dummy (Bit_Chain *restrict dat, Dwg_Object *restrict obj,
                         BITCODE_BS placeholder_type)
{
  Dwg_Data *dwg = obj->parent;
  int is_entity = obj->supertype == DWG_SUPERTYPE_ENTITY;

  obj->size = 0;
  obj->bitsize = 0;

  if (is_entity)
    { // POINT is better than DUMMY to preserve the next_entity chain.
      // TODO much better would be PROXY_ENTITY
      Dwg_Entity_POINT *_obj = obj->tio.entity->tio.POINT;
      LOG_WARN ("fixup unsupported %s " FORMAT_HV " as POINT", obj->dxfname,
                obj->handle.value);
      if (!obj->tio.entity->xdicobjhandle)
        obj->tio.entity->xdicobjhandle = dwg_add_handleref (dwg, 3, 0, NULL);
      /*
      if (obj->tio.entity->num_reactors)
        {
          FREE (obj->tio.entity->reactors);
          obj->tio.entity->num_reactors = 0;
          obj->tio.entity->reactors = NULL;
        }
      */
      if (dwg->opts & DWG_OPTS_INJSON)
        {
          dwg_free_eed (obj);
          if (obj->tio.entity)
            {
              if (obj->tio.entity->preview)
                {
                  free (obj->tio.entity->preview);
                  obj->tio.entity->preview = NULL;
                }
              obj->tio.entity->preview_size = 0;
              obj->tio.entity->preview_exists = 0;
            }
        }
      if (dwg_supports_eed (dwg) && !(dwg->opts & DWG_OPTS_INJSON))
        add_DUMMY_eed (obj); // broken on windows
      dwg_free_object_private (obj);
      FREE (obj->unknown_bits);
      obj->tio.entity->tio.POINT = _obj
          = (Dwg_Entity_POINT *)REALLOC (_obj, sizeof (Dwg_Entity_POINT));
      // memset (_obj, 0, sizeof (Dwg_Entity_POINT)); // asan cries
      _obj->parent = obj->tio.entity;
      _obj->x = 0.0;
      _obj->y = 0.0;
      _obj->z = 0.0;
      _obj->thickness = 1e25; // let it stand out
      _obj->extrusion.x = 0.0;
      _obj->extrusion.y = 0.0;
      _obj->extrusion.z = 1.0;
      _obj->x_ang = 0.0;
      obj->type = DWG_TYPE_POINT;
      obj->fixedtype = DWG_TYPE_POINT;
      if (dwg->opts & DWG_OPTS_INJSON)
        {
          FREE (obj->name);
          obj->name = STRDUP ("POINT");
        }
      else
        obj->name = (char *)"POINT";
      if (dwg->opts & DWG_OPTS_IN)
        {
          FREE (obj->dxfname);
          obj->dxfname = STRDUP ("POINT");
        }
      else
        obj->dxfname = (char *)"POINT";
    }
  else
    {
      const char *name;
      const char *dxfname;

      if (dwg->opts & DWG_OPTS_INJSON)
        dwg_free_eed (obj);
      if (dwg_supports_eed (dwg) && !(dwg->opts & DWG_OPTS_INJSON))
        add_DUMMY_eed (obj); // broken on windows
      dwg_free_object_private (obj);
      // if PLACEHOLDER is available, or even PROXY_OBJECT.
      // PLOTSETTINGS uses PLACEHOLDER though
      if (placeholder_type)
        {
          obj->type = placeholder_type;
          obj->fixedtype = DWG_TYPE_PLACEHOLDER;
          name = "PLACEHOLDER";
          dxfname = "ACDBPLACEHOLDER";
        }
      else
        {
          obj->type = DWG_TYPE_DUMMY;
          obj->fixedtype = DWG_TYPE_DUMMY;
          name = "DUMMY";
          dxfname = "DUMMY";
        }
      LOG_INFO ("fixup unsupported %s " FORMAT_HV " as %s, Type %d\n",
                obj->dxfname, obj->handle.value, name, obj->type);
      if (!obj->tio.object->xdicobjhandle)
        obj->tio.object->xdicobjhandle = dwg_add_handleref (dwg, 3, 0, NULL);
      // keep owner, xdicobj, reactors
      if (dwg->opts & DWG_OPTS_INJSON)
        {
          FREE (obj->name);
          obj->name = STRDUP (name);
        }
      else
        obj->name = (char *)name;
      if (dwg->opts & DWG_OPTS_IN)
        {
          FREE (obj->dxfname);
          obj->dxfname = STRDUP (dxfname);
        }
      else
        obj->dxfname = (char *)dxfname;
      FREE (obj->unknown_bits);
    }
  obj->hdlpos = 0;
}
#endif

#ifndef DEBUG_CLASSES
// delete this NOD entry
// only needed until we can write all object types (at least the ones from the
// NOD)
static void
remove_NOD_item (Dwg_Object_DICTIONARY *_obj, const int i, const char *name)
{
  int last = _obj->numitems - 1;
  if (i < (int)_obj->numitems && _obj->itemhandles[i] != NULL)
    {
      LOG_TRACE ("Disable link to " FORMAT_REF " for NOD.%s\n",
                 ARGS_REF (_obj->itemhandles[i]), name);
    }
  else
    return;
  if (i < last && _obj->itemhandles != NULL && _obj->texts != NULL)
    {
      FREE (_obj->texts[i]);
      if (!_obj->itemhandles[i]->handleref.is_global)
        FREE (_obj->itemhandles[i]);
      memmove (&_obj->texts[i], &_obj->texts[i + 1],
               (last - i) * sizeof (BITCODE_T));
      memmove (&_obj->itemhandles[i], &_obj->itemhandles[i + 1],
               (last - i) * sizeof (BITCODE_H));
    }
  if (_obj->numitems)
    _obj->numitems--;
  return;
}
#endif // !DEBUG_CLASSES

// NOD: ACAD_TABLESTYLE => DICT name[0] - itemhandles[0] => TABLESTYLE
// (Unstable) AcDbVariableDictionary: CTABLESTYLE => DICTVAR str only needed
// until we can write all object types (at least the ones from the NOD)
static void
fixup_NOD (Dwg_Data *restrict dwg,
           Dwg_Object *restrict obj) // named object dict
{
  Dwg_Object_DICTIONARY *_obj;
  int is_tu = dwg->header.version >= R_2007;
  if (obj->handle.value != 0xC)
    return;
  _obj = obj->tio.object->tio.DICTIONARY;
  // => DICTIONARY with name of current style, and link to it.
  // If the link target is disabled (unstable, unhandled or such), remove it
  // from the NOD.
#define DISABLE_NODSTYLE(name)                                                \
  if (!is_type_stable (DWG_TYPE_##name))                                      \
    {                                                                         \
      if (is_tu)                                                              \
        {                                                                     \
          char *u8 = bit_convert_TU ((BITCODE_TU)_obj->texts[i]);             \
          if (u8 && strEQc (u8, "ACAD_" #name))                               \
            remove_NOD_item (_obj, i, "ACAD_" #name);                         \
          FREE (u8);                                                          \
        }                                                                     \
      else if (_obj->texts[i] && strEQc (_obj->texts[i], "ACAD_" #name))      \
        remove_NOD_item (_obj, i, "ACAD_" #name);                             \
    }

#ifndef DEBUG_CLASSES
  for (BITCODE_BL i = 0; i < _obj->numitems; i++)
    {
      DISABLE_NODSTYLE (ASSOCPERSSUBENTMANAGER)
      // else DISABLE_NODSTYLE (ASSOCNETWORK)
      else DISABLE_NODSTYLE (DETAILVIEWSTYLE)
      else DISABLE_NODSTYLE (MATERIAL)
      else DISABLE_NODSTYLE (MLEADERSTYLE)
      else DISABLE_NODSTYLE (MLINESTYLE)
      else DISABLE_NODSTYLE (PERSUBENTMGR)
      else DISABLE_NODSTYLE (PLOTSETTINGS)
      // else DISABLE_NODSTYLE (PLOTSTYLENAME)
      else DISABLE_NODSTYLE (SECTIONVIEWSTYLE)
      else DISABLE_NODSTYLE (TABLESTYLE)
      else DISABLE_NODSTYLE (VISUALSTYLE)
    }
#endif
#undef DISABLE_NODSTYLE
}

/* Copy the decomp buffer uncompressed into dat of a DWG r2004+ file. Sets
 * comp_data_size. */
static int
copy_R2004_section (Bit_Chain *restrict dat, BITCODE_RC *restrict decomp,
                    uint32_t decomp_data_size, uint32_t *comp_data_size)
{
  if (dat->size < dat->byte + decomp_data_size)
    bit_chain_alloc_size (dat, decomp_data_size);
  assert (!dat->bit);
  memcpy (&dat->chain[dat->byte], decomp, decomp_data_size);
  dat->byte += decomp_data_size;
  *comp_data_size = decomp_data_size;
  return 0;
}

/* 1 for yes, 0 for no */
static int
section_encrypted (const Dwg_Data *dwg, const Dwg_Section_Type id)
{
  switch (id)
    {
    case SECTION_SECURITY: //??
    case SECTION_FILEDEPLIST:
    case SECTION_APPINFO:
      return 1;
    case SECTION_UNKNOWN:
    case SECTION_HEADER:
    case SECTION_REVHISTORY:
    case SECTION_OBJECTS:
    case SECTION_OBJFREESPACE:
    case SECTION_TEMPLATE:
    case SECTION_HANDLES:
    case SECTION_CLASSES:
    case SECTION_AUXHEADER:
    case SECTION_SUMMARYINFO:
    case SECTION_PREVIEW:
    case SECTION_APPINFOHISTORY:
    case SECTION_VBAPROJECT:
    case SECTION_SIGNATURE:
    case SECTION_ACDS:
    case SECTION_SYSTEM_MAP:
    case SECTION_INFO:
    default:
      return 0;
    }
}

#if 0  /* unused, LZ compressor not yet ODA-compatible */
/* 1 for yes, 0 for no */
static int
section_compressed (const Dwg_Data *dwg, const Dwg_Section_Type id)
{
  switch (id)
    {
    case SECTION_UNKNOWN:
    case SECTION_HEADER:
    case SECTION_REVHISTORY:
    case SECTION_OBJECTS:
    case SECTION_OBJFREESPACE:
    case SECTION_TEMPLATE:
    case SECTION_HANDLES:
    case SECTION_CLASSES:
    case SECTION_AUXHEADER:
    case SECTION_SYSTEM_MAP:
    case SECTION_INFO:
      return 1;
    case SECTION_SUMMARYINFO:
    case SECTION_PREVIEW:
    case SECTION_APPINFO:
    case SECTION_APPINFOHISTORY:
    case SECTION_FILEDEPLIST:
    case SECTION_SECURITY:
    case SECTION_VBAPROJECT:
    case SECTION_SIGNATURE:
    case SECTION_ACDS:
    default:
      return 0;
    }
}
#endif /* unused */

static int
filedeplist_is_empty (const Dwg_FileDepList *obj)
{
  return obj->num_features == 0 && obj->num_files == 0;
}

static int
security_is_empty (const Dwg_Security *obj)
{
  return obj->unknown_1 == 0 && obj->unknown_2 == 0 && obj->unknown_3 == 0
         && obj->crypto_id == 0 && (!obj->crypto_name || !obj->crypto_name[0])
         && obj->algo_id == 0 && obj->key_len == 0 && obj->encr_size == 0
         && (!obj->encr_buffer || !obj->encr_buffer[0]);
}

/* r2004 compressed sections, LZ77 WIP */

#define MIN_COMPRESSED_SECTION 19
#define COMPRESSION_BUFFER_SIZE 0x400
#define COMPRESSION_WINDOW_SIZE 0x800

static void write_length (Bit_Chain *dat, uint32_t u1, uint32_t match,
                          uint32_t u2);

/* R2004 Write literal length
 */
static unsigned char
write_literal_length (Bit_Chain *restrict dat, BITCODE_RC *restrict buf,
                      uint32_t len)
{
#if 0
  if (len <= (0x0F + 3)) // single byte, opcode 0
    {
      bit_write_RC (dat, len - 3);
      return 0;
    }
  else if (len < 0xf0)
    {
      bit_write_RC (dat, len);
      return length & 0xff;
    }
  else
    {
      uint32_t total = 0x0f;
      while (leng >= 0xf0)
        {
          bit_write_RC (dat, 0);
          len -= 0xFF;
          total += 0xFF;
        }
      bit_write_RC (dat, len - 3); // ??
      return 0;
    }
#else
  if (len)
    {
      if (len > 3)
        {
          write_length (dat, 0, len - 1, 0x11);
        }
      LOG_INSANE ("LIT %x\n", len)
      bit_write_TF (dat, buf, len);
    }
  return 0;
#endif
}

/* Write raw data wrapped in valid LZ "store" framing so that
   decompress_R2004_section can decode it: literal_length + data + 0x11.
   Used for system sections (SECTION_INFO, SECTION_SYSTEM_MAP). */
static int
store_R2004_section (Bit_Chain *restrict dat, BITCODE_RC *restrict decomp,
                     uint32_t decomp_data_size, uint32_t *comp_data_size)
{
  size_t start = dat->byte;
  assert (decomp_data_size > 3); // system sections always > 3 bytes
  // worst-case overhead: length header + data + terminator
  if (dat->size < dat->byte + decomp_data_size + 20)
    bit_chain_alloc_size (dat, decomp_data_size + 20);
  assert (!dat->bit);
  write_literal_length (dat, decomp, decomp_data_size);
  bit_write_RC (dat, 0x11); // end of stream
  *comp_data_size = (uint32_t)(dat->byte - start);
  return 0;
}

/* Return the on-disk size of a system section written via
   store_R2004_section without emitting it. */
static uint32_t
stored_R2004_section_size (uint32_t decomp_data_size)
{
  uint32_t hdr_size = 1; // terminator 0x11

  if (decomp_data_size > 3)
    {
      hdr_size++; // initial literal-length opcode byte
      if (decomp_data_size > 18)
        {
          uint32_t offset = decomp_data_size - 18;
          while (offset > 0xff)
            {
              hdr_size++;
              offset -= 0xff;
            }
          hdr_size++; // final offset byte
        }
    }
  return decomp_data_size + hdr_size;
}

/* R2004 Long Compression Offset
 */
static void
write_long_compression_offset (Bit_Chain *dat, uint32_t offset)
{
  while (offset > 0xff)
    {
      bit_write_RC (dat, 0);
      offset -= 0xff;
    }
  LOG_INSANE (">O 00 %x", offset)
  bit_write_RC (dat, (unsigned char)offset);
}

static void
write_length (Bit_Chain *dat, uint32_t u1, uint32_t match, uint32_t u2)
{
  if (u2 < match)
    {
      LOG_INSANE (">L %x ", u1 & 0xff)
      bit_write_RC (dat, u1 & 0xff);
      write_long_compression_offset (dat, match - u2);
      LOG_INSANE ("\n")
    }
  else
    {
      LOG_INSANE (">L %x\n", (u1 | (match - 2)) & 0xff);
      bit_write_RC (dat, (u1 | (match - 2)) & 0xff);
    }
}

/* R2004 Two Byte Offset
 */
#if 0
static unsigned int
write_two_byte_offset (Bit_Chain *restrict dat, uint32_t offset)
{
  BITCODE_RC b1, b2;
  b1 = offset << 2;
  b2 = offset >> 6;
  //offset = (firstByte >> 2) | (secondByte << 6);
  bit_write_RC (dat, b1);
  bit_write_RC (dat, b2);
  //*lit_length = (firstByte & 0x03);
  return b1 & 0x03;
}
#endif

/* Hash-based match finder for LZ77 compression (ODA section 4.7).
   Uses a 0x8000-entry hash table for O(1) match lookup.
   Returns match length (>= 3) or 0. Sets *dist_p to backward distance.
   Based on the ODA specs and the ACadSharp DwgLZ77AC18Compressor. */
static int
compress_find_match (BITCODE_RC *restrict src, uint32_t src_size, uint32_t pos,
                     int32_t *hash_table, uint32_t *dist_p)
{
  int match_len = 0;
  uint32_t v1, v2, v3, v4;
  int idx;
  int32_t prev;
  uint32_t dist;

  if (pos + 3 >= src_size)
    return 0;

  /* 4-byte rolling hash */
  v1 = (uint32_t)src[pos + 3] << 6;
  v2 = v1 ^ src[pos + 2];
  v3 = (v2 << 5) ^ src[pos + 1];
  v4 = (v3 << 5) ^ src[pos];
  idx = (int)((v4 + (v4 >> 5)) & 0x7FFF);

  prev = hash_table[idx];
  if (prev >= 0)
    dist = pos - (uint32_t)prev;
  else
    dist = 0;

  if (prev >= 0 && dist <= 0xBFFF)
    {
      if (dist > 0x400 && src[pos + 3] != src[(uint32_t)prev + 3])
        {
          /* Try secondary hash slot */
          idx = (idx & 0x7FF) ^ 0x401F;
          prev = hash_table[idx];
          if (prev >= 0)
            dist = pos - (uint32_t)prev;
          if (prev < 0 || dist > 0xBFFF
              || (dist > 0x400 && src[pos + 3] != src[(uint32_t)prev + 3]))
            {
              hash_table[idx] = (int32_t)pos;
              return 0;
            }
        }
      if (src[pos] == src[(uint32_t)prev]
          && src[pos + 1] == src[(uint32_t)prev + 1]
          && src[pos + 2] == src[(uint32_t)prev + 2])
        {
          uint32_t p = (uint32_t)prev + 3;
          uint32_t c = pos + 3;
          match_len = 3;
          while (c < src_size && src[p] == src[c])
            {
              p++;
              c++;
              match_len++;
            }
        }
    }

  hash_table[idx] = (int32_t)pos;
  *dist_p = dist;
  return match_len >= 3 ? match_len : 0;
}

/* Write the LZ77 match encoding (two-byte offset + optional length opcode).
   match_dist:    backward distance (1-based, max 0xBFFF)
   match_len:     number of matching bytes (>= 3)
   trailing_lits: number of literal bytes that follow (0-3 in low bits,
                  >= 4 means low bits are 0, separate literal_length follows)
 */
static void
compress_write_match (Bit_Chain *restrict dat, uint32_t match_dist,
                      uint32_t match_len, uint32_t trailing_lits)
{
  uint32_t b1, b2;

  LOG_INSANE ("WM dist=%u len=%u lits=%u: ", match_dist, match_len,
              trailing_lits)
  if (match_len < 0x0F && match_dist <= 0x400)
    {
      /* Compact encoding: 2 bytes total, decompressor opcode >= 0x40 */
      uint32_t d = match_dist - 1;
      b1 = ((match_len + 1) << 4) | ((d & 3) << 2);
      b2 = d >> 2;
    }
  else if (match_dist <= 0x4000)
    {
      /* Medium encoding: length opcode 0x20..0x3F + two-byte offset */
      uint32_t d = match_dist - 1;
      write_length (dat, 0x20, match_len, 0x21);
      b1 = (d & 0xFF) << 2;
      b2 = d >> 6;
    }
  else
    {
      /* Long encoding: length opcode 0x10..0x1F + two-byte offset */
      uint32_t d = match_dist - 0x4000;
      write_length (dat, ((d >> 11) & 8) | 0x10, match_len, 9);
      b1 = (d & 0xFF) << 2;
      b2 = d >> 6;
    }

  if (trailing_lits < 4)
    b1 |= trailing_lits;

  LOG_INSANE ("> %x %x\n", b1 & 0xFF, b2 & 0xFF)
  bit_write_RC (dat, b1 & 0xFF);
  bit_write_RC (dat, b2 & 0xFF);
}

/* Compress the decomp buffer into dat of a DWG r2004+ file. Sets
   comp_data_size. LZ77 variant, ODA section 4.7.
   Based on ACadSharp DwgLZ77AC18Compressor.
   Format: [literal_length + data]* [match [literal_length + data]]* 0x11
*/
static int
compress_R2004_section (Bit_Chain *restrict dat, BITCODE_RC *restrict decomp,
                        uint32_t decomp_data_size, uint32_t *comp_data_size)
{
#ifndef HAVE_COMPRESS_R2004_SECTION
  return store_R2004_section (dat, decomp, decomp_data_size, comp_data_size);
#else
  size_t start = dat->byte;
  int32_t *hash_table;
  uint32_t curr_offset = 0;    /* start of pending literal run */
  uint32_t pos = 4;            /* current scan position (skip first 4 bytes) */
  uint32_t prev_match_len = 0; /* saved match length from previous iter */
  uint32_t prev_match_dist = 0;
  uint32_t lits, match_len, match_dist;

  assert (!dat->bit);
  /* Small inputs: encode as a pure literal stream. */
  if (decomp_data_size <= 0x18)
    {
      if (dat->size < dat->byte + decomp_data_size + 20)
        bit_chain_alloc_size (dat, decomp_data_size + 20);
      write_literal_length (dat, decomp, decomp_data_size);
      bit_write_RC (dat, 0x11); /* end of stream */
      *comp_data_size = (uint32_t)(dat->byte - start);
      return 0;
    }

  /* Ensure enough output space */
  if (dat->size < dat->byte + decomp_data_size + 20)
    bit_chain_alloc_size (dat, decomp_data_size + 20);

  hash_table = (int32_t *)calloc (0x8000, sizeof (int32_t));
  if (!hash_table)
    {
      /* Valid LZ stream fallback if memory is tight. */
      write_literal_length (dat, decomp, decomp_data_size);
      bit_write_RC (dat, 0x11); /* end of stream */
      *comp_data_size = (uint32_t)(dat->byte - start);
      return 0;
    }
  memset (hash_table, -1, 0x8000 * sizeof (int32_t));

  while (pos < decomp_data_size - 0x13)
    {
      match_len = (uint32_t)compress_find_match (decomp, decomp_data_size, pos,
                                                 hash_table, &match_dist);
      if (match_len < 3)
        {
          pos++;
          continue;
        }

      /* Number of literal bytes between last match end and this match */
      lits = pos - curr_offset;

      /* Write previous match (delayed) with trailing literal count */
      if (prev_match_len)
        compress_write_match (dat, prev_match_dist, prev_match_len, lits);

      /* Write literal bytes */
      write_literal_length (dat, &decomp[curr_offset], lits);

      pos += match_len;
      curr_offset = pos;
      prev_match_len = match_len;
      prev_match_dist = match_dist;
    }

  /* Final trailing literals */
  lits = decomp_data_size - curr_offset;
  if (prev_match_len)
    compress_write_match (dat, prev_match_dist, prev_match_len, lits);
  write_literal_length (dat, &decomp[curr_offset], lits);

  bit_write_RC (dat, 0x11); /* end of stream */
  *comp_data_size = (uint32_t)(dat->byte - start);

  free (hash_table);
  return 0;
#endif
}

static Dwg_Section_Info *
find_section_info_type (const Dwg_Data *restrict dwg, Dwg_Section_Type type)
{
  for (unsigned i = 0; i < dwg->header.section_infohdr.num_desc; i++)
    {
      Dwg_Section_Info *info = &dwg->header.section_info[i];
      if (info->fixedtype == type)
        return info;
    }
  return NULL;
}

/* Ordering of r13-r2000 sections 0-6 */
static void
section_order_trace (const Dwg_Data *dwg, const BITCODE_BL numsections,
                     Dwg_Section_Type_r13 *psection_order)
{
  BITCODE_BL num = numsections;
  LOG_TRACE ("section_order:");
  if (numsections > SECTION_R13_SIZE)
    {
      LOG_WARN ("numsections %u => %u", numsections, SECTION_R13_SIZE);
      num = SECTION_R13_SIZE;
    }
  for (BITCODE_BL i = 0; i < num; i++)
    {
      LOG_TRACE (" %u", psection_order[i]);
    }
  LOG_TRACE ("\n[");
  for (BITCODE_BL i = 0; i < num; i++)
    {
      LOG_TRACE ("%s ", dwg_section_name (dwg, psection_order[i]));
    }
  LOG_TRACE ("]\n");
}

static int
section_move_top (Dwg_Section_Type_r13 *psection_order, BITCODE_RL *pnum,
                  Dwg_Section_Type_r13 sec_id)
{
  Dwg_Section_Type_r13 old_first = psection_order[0];
  assert (*pnum <= SECTION_R13_SIZE);
  if (psection_order[0] == sec_id)
    {
      LOG_TRACE ("section_move_top %u (already)\n", sec_id);
      return 0;
    }

  for (unsigned i = 1; i < *pnum; i++)
    {
      // f x x i y y y
      if (psection_order[i] == (unsigned)sec_id)
        { // found at i
          psection_order[0] = sec_id;
          // move x'n right by 1
          if (i > 1)
            memmove (&psection_order[2], &psection_order[1],
                     (i - 1) * sizeof (Dwg_Section_Type_r13));
          psection_order[1] = old_first;
          // i f x x y y y
          LOG_TRACE ("section_move_top %u (re-order)\n", sec_id);
          return 0;
        }
    }
  // not found: insert
  psection_order[0] = sec_id;
  // move x'n right by 1
  // f x x x y y y
  memmove (&psection_order[2], &psection_order[1],
           (*pnum - 1) * sizeof (Dwg_Section_Type_r13));
  psection_order[1] = old_first;
  LOG_TRACE ("section_move_top %u (inserted)\n", sec_id);
  (*pnum)++;
  assert (*pnum <= SECTION_R13_SIZE);
  return 1;
}

static unsigned
section_find (Dwg_Section_Type_r13 *psection_order, BITCODE_RL num,
              Dwg_Section_Type_r13 id)
{
  LOG_TRACE ("section_find %u\n", (unsigned)id);
  for (unsigned i = 0; i < num; i++)
    {
      if (psection_order[i] == id)
        { // found at i
          return i;
        }
    }
  return SECTION_R13_SIZE;
}

static int
section_remove (Dwg_Section_Type_r13 *psection_order, BITCODE_RL *pnum,
                Dwg_Section_Type_r13 id)
{
  unsigned i = section_find (psection_order, *pnum, id);
  LOG_TRACE ("section_remove %u [%u]\n", (unsigned)id, i);
  if (i >= *pnum) // not found
    return 0;
  // move left
  assert (*pnum > 0);
  (*pnum)--;
  memmove (&psection_order[i], &psection_order[i + 1],
           (*pnum - i) * sizeof (Dwg_Section_Type_r13));
  psection_order[*pnum]
      = (Dwg_Section_Type_r13)SECTION_R13_SIZE; // sentinel (invalid)
  return 1;
}

static int
section_move_before (Dwg_Section_Type_r13 *psection_order, BITCODE_RL *pnum,
                     Dwg_Section_Type_r13 id, Dwg_Section_Type_r13 before)
{
  int ret = 0;
  unsigned b;
  unsigned id_pos;
  Dwg_Section_Type_r13 old_before;
  LOG_TRACE ("section_move_before %u %u\n", (unsigned)id, (unsigned)before);
  b = section_find (psection_order, *pnum, before);
  // find before
  if (b >= SECTION_R13_SIZE) // not found
    return 0;
  // x x b y y
  old_before = psection_order[b];
  assert (*pnum + 1 <= SECTION_R13_SIZE);
  memmove (&psection_order[b + 1], &psection_order[b],
           (*pnum - b) * sizeof (Dwg_Section_Type_r13));
  (*pnum)++;
  if (!section_remove (psection_order, pnum, id)) // if not exist: insert
    ret = 1;
  psection_order[b] = id;
  // x x i b y
  return ret;
}

/* header.section pointers changed, rebuild all info->sections */
static void
section_info_rebuild (Dwg_Data *dwg, Dwg_Section_Type lasttype)
{
  (void)lasttype;
  for (unsigned idx = 0; idx < dwg->header.section_infohdr.num_desc; idx++)
    {
      Dwg_Section_Info *info = &dwg->header.section_info[idx];
      if (info->sections && info->num_sections)
        {
          unsigned ssi = 0;
          for (unsigned i = 0; i < dwg->header.num_sections; i++)
            {
              Dwg_Section *sec = &dwg->header.section[i];
              if (sec->type == info->fixedtype)
                {
                  info->sections[ssi] = sec;
                  ssi++;
                  if (ssi >= info->num_sections)
                    break;
                }
            }
        }
    }
}

static void
calc_preR13_ctrl_flags_r11 (Dwg_Data *restrict dwg, Dwg_Section *tbl, int i)
{
  const Dwg_Version_Type ver = dwg->header.version;
  /* TODO Saved by R_2_0 is unknown */
  /* TODO Drawings from AutoCAD release distribution had increasing numbers */
  if (ver <= R_2_10)
    {
      /* Different numbers 0x800f, 0x8010, 0x8011 all same from saved drawing
       * by r2.17/r2.18 */
      tbl->flags_r11 = 0x800f;
    }
  else if (ver < R_9)
    {
      /* Increasing values (0x8008 - 0x800c), saved by R2.6 */
      tbl->flags_r11 = 0x8007 + i;
    }
  else
    {
      /* Saved from R9, R10 and R11 */
      tbl->flags_r11 = 0;
    }
}

// see below for the elements
static void
calc_preR13_ctrl_size (Dwg_Data *restrict dwg, Dwg_Object *obj)
{
  const Dwg_Version_Type ver = dwg->header.version;
  switch (obj->fixedtype)
    {
    case DWG_TYPE_BLOCK_CONTROL:
      if (ver == R_11)
        obj->size = 45;
      else if (ver <= R_10)
        obj->size = 37;
      break;
    case DWG_TYPE_LAYER_CONTROL:
      if (ver == R_11)
        obj->size = 41;
      else if (ver <= R_10)
        obj->size = 37;
      break;
    case DWG_TYPE_STYLE_CONTROL:
      if (ver == R_11)
        obj->size = 198;
      else if (ver == R_2_10)
        obj->size = 130;
      else if (ver <= R_10) // also r2.4, r2.5
        obj->size = 194;
      break;
    case DWG_TYPE_LTYPE_CONTROL:
      if (ver == R_11)
        obj->size = 191;
      else if (ver <= R_10)
        obj->size = 187;
      break;
    case DWG_TYPE_VIEW_CONTROL:
      if (ver <= R_11)
        obj->size = 153;
      else if (ver == R_10)
        obj->size = 149;
      else if (ver <= R_9)
        obj->size = 91;
      break;
    case DWG_TYPE_UCS_CONTROL:
      if (ver == R_11)
        obj->size = 109;
      else if (ver == R_10)
        obj->size = 105;
      break;
    case DWG_TYPE_VPORT_CONTROL:
      if (ver == R_11)
        obj->size = 253;
      else if (ver == R_10)
        obj->size = 249;
      break;
    case DWG_TYPE_APPID_CONTROL:
      obj->size = 37;
      break;
    case DWG_TYPE_DIMSTYLE_CONTROL:
      obj->size = 324;
      break;
    case DWG_TYPE_VX_CONTROL:
      obj->size = 43;
      break;
    default:
      break;
    }
}

// needed on upgrades or <r2.0b
static void
encode_check_num_sections (Dwg_Section_Type_r11 id, Dwg_Data *restrict dwg)
{
  BITCODE_RL num_sections;
  if (!dwg->header.sections || !dwg->header.num_sections)
    dwg_sections_init (dwg);
  // starts at 1 and adds thumbnail
  num_sections = dwg->header.num_sections + 2;
  if (!id)
    {
      id = (Dwg_Section_Type_r11)num_sections;
      dwg->header.sections = dwg->header.num_sections;
    }
  if ((BITCODE_RL)id >= num_sections)
    {
      // create empty default section on upgrade
      LOG_TRACE ("Not enough sections " FORMAT_RL " for id %u\n", num_sections,
                 (unsigned)id);
      dwg->header.section = (Dwg_Section *)REALLOC (
          dwg->header.section, sizeof (Dwg_Section) * (id + 2));
      memset (&dwg->header.section[num_sections - 1], 0,
              (id + 1 - num_sections) * sizeof (Dwg_Section));
      dwg->header.num_sections = (BITCODE_RL)id;
    }
}

static void
encode_preR13_section_hdr (const char *restrict name,
                           const Dwg_Section_Type_r11 id,
                           Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  static BITCODE_BL addr = 0;
  Dwg_Section *tbl;
  int i;
  unsigned long end_address;

  assert (id <= SECTION_VX);
  encode_check_num_sections (id, dwg);
  tbl = &dwg->header.section[id];
  i = id < 5 ? id : id - 1;
  if (id == SECTION_BLOCK)
    addr = dwg->header.entities_end + (dat->version >= R_11 ? 0x20 : 0);
  // SECTION_BLOCK = 1,
  // SECTION_LAYER = 2,
  // SECTION_STYLE = 3,
  // SECTION_LTYPE = 5,
  // SECTION_VIEW  = 6,
  if (tbl && !tbl->size)
    {
      switch (id)
        {

#define ENCODE_CTRL_TO_TABLE(idtoken, ctrltoken)                              \
  case SECTION_##idtoken:                                                     \
    {                                                                         \
      Dwg_Object *obj = dwg_get_first_object (dwg, DWG_TYPE_##ctrltoken);     \
      if (obj)                                                                \
        {                                                                     \
          Dwg_Object_##ctrltoken *_obj = obj->tio.object->tio.ctrltoken;      \
          strncpy (tbl->name, obj->name, sizeof (tbl->name) - 1);             \
          tbl->name[sizeof (tbl->name) - 1] = '\0';                           \
          if (!obj->size)                                                     \
            calc_preR13_ctrl_size (dwg, obj);                                 \
          tbl->size = obj->size;                                              \
          tbl->number = _obj->num_entries;                                    \
          if (!tbl->flags_r11)                                                \
            calc_preR13_ctrl_flags_r11 (dwg, tbl, i);                         \
          tbl->address = addr;                                                \
          addr += tbl->size * tbl->number;                                    \
          if (dat->version >= R_11)                                           \
            addr += 0x20;                                                     \
        }                                                                     \
      else                                                                    \
        LOG_WARN (#ctrltoken " hdr not found")                                \
      break;                                                                  \
    }
        case SECTION_HEADER_R11:
        default:
          break;
          ENCODE_CTRL_TO_TABLE (BLOCK, BLOCK_CONTROL)
          ENCODE_CTRL_TO_TABLE (LAYER, LAYER_CONTROL)
          ENCODE_CTRL_TO_TABLE (STYLE, STYLE_CONTROL)
          ENCODE_CTRL_TO_TABLE (LTYPE, LTYPE_CONTROL)
          ENCODE_CTRL_TO_TABLE (VIEW, VIEW_CONTROL)
          ENCODE_CTRL_TO_TABLE (UCS, UCS_CONTROL)
          ENCODE_CTRL_TO_TABLE (VPORT, VPORT_CONTROL)
          ENCODE_CTRL_TO_TABLE (APPID, APPID_CONTROL)
          ENCODE_CTRL_TO_TABLE (DIMSTYLE, DIMSTYLE_CONTROL)
          ENCODE_CTRL_TO_TABLE (VX, VX_CONTROL)
#undef ENCODE_CTRL_TO_TABLE
        }
    }
  LOG_TRACE ("ptr table %s [%d]", tbl->name, id);
  LOG_RPOS;
  LOG_TRACE ("----------------------\n");
  bit_write_RS (dat, tbl->size & 0xFFFF); // calculated
  LOG_TRACE ("%s.size: " FORMAT_RS " [RS]", tbl->name, (BITCODE_RS)tbl->size);
  LOG_RPOS;
  bit_write_RS (dat, (BITCODE_RS)tbl->number);
  LOG_TRACE ("%s.number: " FORMAT_RS " [RS]", tbl->name,
             (BITCODE_RS)tbl->number);
  LOG_RPOS;
  bit_write_RS (dat, tbl->flags_r11);
  LOG_TRACE ("%s.flags_r11: " FORMAT_RSx " [RS]", tbl->name, tbl->flags_r11);
  LOG_RPOS;
  tbl->address = dat->byte;
  bit_write_RL (dat, 0xDEADBEAF); // patched later
  LOG_TRACE ("%s.address: 0xDEADBEAF [RL] (patch addr)", tbl->name);
  LOG_RPOS;
  end_address = (unsigned long)(tbl->address + (tbl->number * tbl->size));
  LOG_TRACE ("ptr table end: 0x%lx (%lu)\n\n", end_address, end_address);
}

// only in R11
static void
encode_preR13_section_chk (const Dwg_Section_Type_r11 id,
                           Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  Dwg_Section *tbl;
  // if (!dwg->header.num_sections)
  //   dwg->header.num_sections = SECTION_VX; // r11
  encode_check_num_sections (id, dwg);
  tbl = &dwg->header.section[id];
  if (tbl)
    {
      bit_write_RS (dat, (BITCODE_RS)id);
      bit_write_RS (dat, tbl->size);
      bit_write_RS (dat, tbl->number);
      bit_write_RL (dat, tbl->address);
      LOG_TRACE ("chk table %-8s [%2d]: size:%-4u nr:%-3ld (0x%zx)\n",
                 tbl->name, id, tbl->size, (long)tbl->number,
                 (size_t)tbl->address)
    }
}

// only in R11
static int
encode_r11_auxheader (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  int error = 0;
  Dwg_AuxHeader *_obj = &dwg->auxheader;
  Dwg_Object *obj = NULL;

  LOG_TRACE ("\nAUXHEADER: @0x%zx\n", dat->byte);
  _obj->auxheader_address = dat->byte & 0xFFFFFFFF;
  if (!_obj->num_auxheader_variables)
    {
      _obj->num_auxheader_variables = 16;
      _obj->auxheader_size = 138;
      _obj->num_aux_tables = 10;
      _obj->entities_start = dwg->header.entities_start;
      _obj->entities_end = dwg->header.entities_end;
      _obj->blocks_start = dwg->header.blocks_start;
      _obj->extras_start = dwg->header.extras_start;
      _obj->R11_HANDLING = dwg->header_vars.HANDLING;
    }
  write_sentinel (dat, DWG_SENTINEL_R11_AUXHEADER_BEGIN);
  FIELD_RS (num_auxheader_variables, 0);
  FIELD_RS (auxheader_size, 0);
  FIELD_RLx (entities_start, 0);
  if (_obj->entities_start != dwg->header.entities_start)
    {
      LOG_WARN ("entities_start %x/%x", _obj->entities_start,
                dwg->header.entities_start);
    }
  FIELD_RLx (entities_end, 0);
  if (_obj->entities_end != dwg->header.entities_end)
    {
      LOG_WARN ("entities_end %x/%x", _obj->entities_end,
                dwg->header.entities_end);
    }
  FIELD_RLx (blocks_start, 0);
  if (_obj->blocks_start != dwg->header.blocks_start)
    {
      LOG_WARN ("blocks_start %x/%x", _obj->blocks_start,
                dwg->header.blocks_start);
    }
  FIELD_RLx (extras_start, 0);
  if (_obj->extras_start != dwg->header.extras_start)
    {
      LOG_WARN ("extras_start %x/%x", _obj->extras_start,
                dwg->header.extras_start);
    }
  _obj->R11_HANDLING = dwg->header_vars.HANDLING;
  FIELD_RS (R11_HANDLING, 0);
  {
    // always use the header_vars.HANDSEED
    _obj->HANDSEED = dwg->header_vars.HANDSEED->handleref.value & 0xFFFFFFFF;
    bit_write_RL_BE (dat, _obj->HANDSEED);
    LOG_TRACE ("HANDSEED: " FORMAT_RLx "\n", _obj->HANDSEED);
  }
  FIELD_RS (num_aux_tables, 0);
  encode_preR13_section_chk (SECTION_BLOCK, dat, dwg);
  encode_preR13_section_chk (SECTION_LAYER, dat, dwg);
  encode_preR13_section_chk (SECTION_STYLE, dat, dwg);
  encode_preR13_section_chk (SECTION_LTYPE, dat, dwg);
  encode_preR13_section_chk (SECTION_VIEW, dat, dwg);
  if (dwg->header.num_sections >= SECTION_VPORT)
    {
      encode_preR13_section_chk (SECTION_UCS, dat, dwg);
      encode_preR13_section_chk (SECTION_VPORT, dat, dwg);
    }
  if (dwg->header.num_sections >= SECTION_APPID)
    {
      encode_preR13_section_chk (SECTION_APPID, dat, dwg);
    }
  if (dwg->header.num_sections >= SECTION_VX)
    {
      encode_preR13_section_chk (SECTION_DIMSTYLE, dat, dwg);
      encode_preR13_section_chk (SECTION_VX, dat, dwg);
    }
  FIELD_RLx (auxheader_address, 0);
  bit_write_CRC (dat, _obj->auxheader_address + 16, 0xC0C1);
  write_sentinel (dat, DWG_SENTINEL_R11_AUXHEADER_END);
  LOG_TRACE ("\n");

  return error;
}

// r13c3 - r2000
static int
encode_objfreespace_private (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  Dwg_ObjFreeSpace *_obj = &dwg->objfreespace;
  Dwg_Object *obj = NULL;
  int error = 0;
  // clang-format off
  #include "objfreespace.spec"
  // clang-format on
  return error;
}

// r13c3 - r2000
static int
encode_secondheader_private (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  Bit_Chain *str_dat = dat;
  Dwg_SecondHeader *_obj = &dwg->secondheader;
  // for error logging only:
#ifndef __cplusplus
  Dwg_Object *obj = &(Dwg_Object){ .name = (char *)"2NDHEADER" };
#else
  Dwg_Object xobj;
  xobj.name = (char *)"2NDHEADER";
  Dwg_Object *obj = &xobj;
#endif
  int error = 0;
  BITCODE_BL vcount;
  if (!dat->chain || !dat->size)
    return 1;

    // clang-format off
  #include "2ndheader.spec"
  // clang-format on

  return error;
}

static int
encode_template (Dwg_Data *restrict dwg, Bit_Chain *restrict dat)
{
  int error = 0;
  struct _dwg_template *_obj = &dwg->Template;
  Dwg_Section_Type sec_id;
  SINCE (R_2004a)
  sec_id = SECTION_TEMPLATE;
  else sec_id = (Dwg_Section_Type)SECTION_TEMPLATE_R13;

  if ((int)dwg->header.num_sections <= (int)sec_id)
    dwg->header.section = (Dwg_Section *)REALLOC (
        dwg->header.section, (sec_id + 1) * sizeof (Dwg_Section));
  LOG_INFO ("\n=======> Template: @%4zu\n", dat->byte);
  dwg->header.section[sec_id].number = 4;
  dwg->header.section[sec_id].address = dat->byte;
  dwg->header.section[sec_id].size = 4; // always empty description
  // Template description
  bit_write_T16 (dat, _obj->description);
  LOG_TRACE ("_obj->description: %s [T16]\n", _obj->description);
  // 0 - English, 1- Metric
  bit_write_RS (dat, dwg->header_vars.MEASUREMENT);
  LOG_TRACE ("HEADER.MEASUREMENT: %d [RS]\n", dwg->header_vars.MEASUREMENT);
  return error;
}

static int
encode_auxheader (Dwg_Data *restrict dwg, Bit_Chain *restrict dat)
{
  Bit_Chain *str_dat = dat;
  Dwg_AuxHeader *_obj = &dwg->auxheader;
  Dwg_Object *obj = NULL;
  BITCODE_BL vcount;
  int error = 0;
  const BITCODE_RL olds
      = dwg->secondheader.sections[SECTION_AUXHEADER_R2000].size;
  BITCODE_RL news = dwg->header.section[SECTION_AUXHEADER_R2000].size;

  assert (!dat->bit);
  LOG_INFO ("\n=======> AuxHeader: %8zu\n", dat->byte); // size: 123

  dwg->header.section[SECTION_AUXHEADER_R2000].number = 5;
  dwg->header.section[SECTION_AUXHEADER_R2000].address = dat->byte;

  if (!_obj->dwg_version) // todo: needed?
    {
      BITCODE_RS def_unknown_6rs[] = { 4, 0x565, 0, 0, 2, 1 };
      LOG_TRACE ("Use AuxHeader defaults...\n");
      FIELD_VALUE (aux_intro[0]) = 0xff;
      FIELD_VALUE (aux_intro[1]) = 0x77;
      FIELD_VALUE (aux_intro[2]) = 0x01;
      FIELD_VALUE (minus_1) = -1;
      FIELD_VALUE (dwg_version) = dwg->header.dwg_version;
      FIELD_VALUE (maint_version) = dwg->header.maint_version;
      FIELD_VALUE (dwg_version_1) = dwg->header.dwg_version;
      FIELD_VALUE (dwg_version_2) = dwg->header.dwg_version;
      FIELD_VALUE (maint_version_1) = dwg->header.maint_version;
      FIELD_VALUE (maint_version_2) = dwg->header.maint_version;
      memcpy (FIELD_VALUE (unknown_6rs), def_unknown_6rs,
              sizeof (def_unknown_6rs));
      memcpy (&_obj->TDCREATE, &dwg->header_vars.TDCREATE,
              sizeof (BITCODE_TIMERLL));
      memcpy (&_obj->TDUPDATE, &dwg->header_vars.TDUPDATE,
              sizeof (BITCODE_TIMERLL));
      if (dwg->header_vars.HANDSEED)
        FIELD_VALUE (HANDSEED) = dwg->header_vars.HANDSEED->absolute_ref;
    }

    // clang-format off
  #include "auxheader.spec"
  // clang-format on

  news = dwg->header.section[SECTION_AUXHEADER_R2000].size
      = (BITCODE_RL)(dat->byte
                     - dwg->header.section[SECTION_AUXHEADER_R2000].address);
  // maybe we need padding
  if (dat->version == dat->from_version && news < olds)
    {
      for (unsigned i = 0; i < (olds - news); i++)
        bit_write_RC (dat, 0);
    }

  assert (!dat->bit);
  return error;
}

/*------------------------------------------------------------
 * THUMBNAIL preview pictures
 */
static int
encode_r13_thumbnail (Dwg_Data *restrict dwg, Bit_Chain *restrict dat,
                      const size_t header_crc_address)
{
  int error = 0;
  // When adding Thumbnail after MEASUREMENT, patchup the address later.
  VERSIONS (R_13b1, R_2000)
  {
    if (dwg->header.thumbnail_address != (dat->byte & 0xFFFFFFFF))
      {
        // patchup header.thumbnail_address
        size_t oldpos = dat->byte;
        dwg->header.thumbnail_address = dat->byte & 0xFFFFFFFF;
        dat->byte = 0x0D;
        bit_write_RL (dat, dwg->header.thumbnail_address);
        LOG_TRACE ("header.thumbnail_address => " FORMAT_RL " [RL] @0x0d\n",
                   dwg->header.thumbnail_address);
        assert (header_crc_address);
        if (header_crc_address)
          {
            dat->byte = header_crc_address;
            bit_write_CRC (dat, 0, 0xC0C1);
            LOG_TRACE ("header.CRC updated [RSx] @%" PRIuSIZE "\n",
                       header_crc_address)
          }
        else
          LOG_WARN ("header.CRC not updated!\n")
        dat->byte = oldpos;
      }
  }
  SINCE (R_13b1)
  {
    dat->bit = 0;
    LOG_TRACE ("\n=======> Thumbnail:       %4zu\n", dat->byte);
    // dwg->thumbnail.size = 0; // to disable
    write_sentinel (dat, DWG_SENTINEL_THUMBNAIL_BEGIN);
    if (dwg->thumbnail.size == 0)
      {
        bit_write_RL (dat, 5); // overall size
        LOG_TRACE ("Thumbnail size: 5 [RL]\n");
        bit_write_RC (dat, 0); // num_pictures
        LOG_TRACE ("Thumbnail num_pictures: 0 [RC]\n");
      }
    else
      {
        bit_write_TF (dat, dwg->thumbnail.chain, dwg->thumbnail.size);
      }
    write_sentinel (dat, DWG_SENTINEL_THUMBNAIL_END);
    {
      BITCODE_RL bmpsize;
      BITCODE_RC type;
      dwg_bmp (dwg, &bmpsize, &type);
      if (bmpsize > dwg->thumbnail.size)
        LOG_ERROR ("thumbnail size overflow: %i > %" PRIuSIZE "\n", bmpsize,
                   dwg->thumbnail.size);
    }
    LOG_TRACE ("         Thumbnail (end): %4zu\n", dat->byte);
  }
  return error;
}

/*------------------------------------------------------------
 * Header Variables
 */
static int
encode_header_vars (Dwg_Data *restrict dwg, Bit_Chain *restrict dat,
                    Dwg_Version_Type orig_from_version)
{
  int error;
  size_t size_adr;
  Dwg_Section_Type sec_id;
  SINCE (R_2004a)
  sec_id = SECTION_HEADER;
  else sec_id = (Dwg_Section_Type)SECTION_HEADER_R13;
  assert (!dat->bit);
  LOG_INFO ("\n=======> Header Variables:   %4zu\n", dat->byte);
  if (!dwg->header.section)
    {
      LOG_ERROR ("Empty header.section");
      return DWG_ERR_OUTOFMEM;
    }
  dwg->header.section[sec_id].number = 0;
  dwg->header.section[sec_id].address = dat->byte;
  write_sentinel (dat, DWG_SENTINEL_VARIABLE_BEGIN);
  size_adr = dat->byte;
  bit_write_RL (dat, 540); // Size placeholder
  error = dwg_encode_header_variables (dat, dat, dat, dwg);
  // undo minimal HEADER hack
  if (dat->from_version != orig_from_version)
    dat->from_version = orig_from_version;
  encode_patch_RLsize (dat, size_adr);
  bit_write_CRC (dat, size_adr, 0xC0C1);
  write_sentinel (dat, DWG_SENTINEL_VARIABLE_END);
  assert ((int64_t)dat->byte > (int64_t)dwg->header.section[0].address);
  dwg->header.section[0].size
      = ((int64_t)dat->byte - (int64_t)dwg->header.section[0].address)
        & 0xFFFFFFFF;
  LOG_TRACE ("         Header Variables (end): %4zu\n", dat->byte);
  return error;
}

/*------------------------------------------------------------
 * Classes
 */
static int
encode_classes (Dwg_Data *restrict dwg, Bit_Chain *restrict dat)
{
  int error = 0;
  BITCODE_BL j;
  BITCODE_BS max_num = 499;
  Dwg_Section_Type sec_id;
  size_t size_adr;
  SINCE (R_2004a)
  sec_id = SECTION_CLASSES;
  else sec_id = (Dwg_Section_Type)SECTION_CLASSES_R13;
  LOG_INFO ("\n=======> Classes: %4zu (%d)\n", dat->byte, dwg->num_classes);
  if (dwg->num_classes > UINT16_MAX - 500)
    {
      LOG_ERROR ("Invalid dwg->num_classes " FORMAT_BS, dwg->num_classes);
      dwg->num_classes = 0;
      error |= DWG_ERR_VALUEOUTOFBOUNDS | DWG_ERR_CLASSESNOTFOUND;
    }
  dwg->header.section[sec_id].number = 1;
  dwg->header.section[sec_id].address = dat->byte; // FIXME
  write_sentinel (dat, DWG_SENTINEL_CLASS_BEGIN);
  size_adr = dat->byte;  // Size position
  bit_write_RL (dat, 0); // Size placeholder

  SINCE (R_2004a)
  {
    for (j = 0; j < dwg->num_classes; j++)
      if (dwg->dwg_class[j].number > max_num)
        max_num = dwg->dwg_class[j].number;
    bit_write_BS (dat, max_num);
    LOG_TRACE ("max_num: " FORMAT_BS " [BS]\n", max_num);
    bit_write_RS (dat, 0);
    LOG_TRACE ("rs_zero: " FORMAT_RS " [RS]\n", 0);
    bit_write_B (dat, 1);
    LOG_TRACE ("btrue: " FORMAT_B " [B]\n", 1);
  }
  for (j = 0; j < dwg->num_classes; j++)
    {
      Dwg_Class *klass;
      klass = &dwg->dwg_class[j];
      bit_write_BS (dat, klass->number);
      bit_write_BS (dat, klass->proxyflag);
      SINCE (R_2007a)
      {
        bit_write_T (dat, klass->appname);
        bit_write_T (dat, klass->cppname);
      }
      else
      {
        bit_write_TV (dat, klass->appname);
        bit_write_TV (dat, klass->cppname);
      }
      SINCE (R_2007a) // only when we have it. like not for 2004 => 2007
      // conversions
      {
        if (klass->dxfname_u)
          bit_write_TU (dat, klass->dxfname_u);
        else
          bit_write_T (dat, klass->dxfname);
      }
      else // we always have this one
          bit_write_TV (dat, klass->dxfname);
      bit_write_B (dat, klass->is_zombie);
      bit_write_BS (dat, klass->item_class_id);
      LOG_TRACE ("Class %d 0x%x %s\n"
                 " %s \"%s\" %d 0x%x\n",
                 klass->number, klass->proxyflag, klass->dxfname,
                 klass->cppname, klass->appname, klass->is_zombie,
                 klass->item_class_id);

      SINCE (R_2004a)
      {
        if (!klass->dwg_version)
          {
            // defaults
            klass->dwg_version = (BITCODE_BL)dwg->header.dwg_version;
            klass->maint_version = (BITCODE_BL)dwg->header.maint_version;
            // TODO num_instances
          }
        bit_write_BL (dat, klass->num_instances);
        bit_write_BS (dat, klass->dwg_version);
        bit_write_BS (dat, klass->maint_version);
        bit_write_BL (dat, klass->unknown_1);
        bit_write_BL (dat, klass->unknown_2);
        LOG_TRACE (" %d %d\n", (int)klass->num_instances,
                   (int)klass->dwg_version);
      }
    }

  /* Patch the section size at its beginning
   */
  assert (size_adr);
  encode_patch_RLsize (dat, size_adr);
  bit_write_CRC (dat, size_adr, 0xC0C1);
  write_sentinel (dat, DWG_SENTINEL_CLASS_END);
  dwg->header.section[sec_id].size
      = (dat->byte - dwg->header.section[sec_id].address) & 0xFFFFFFFF;
  LOG_TRACE ("       Classes (end): %4zu\n", dat->byte);
  // FIXME: for all versions?
  bit_write_RL (dat, 0x0DCA);
  LOG_TRACE ("unknown: %04X [RL]\n", 0x0DCA);
  return error;
}

/*------------------------------------------------------------
 * Classes
 */
static int
encode_objects_handles (Dwg_Data *restrict dwg, Bit_Chain *restrict dat,
                        Bit_Chain *restrict sec_dat)
{
  int error = 0;
  int ckr_missing = 1;
  BITCODE_BL i, j;
  Dwg_Section_Type sec_id;
  size_t size_adr;
  size_t last_offset;
  BITCODE_RLL last_handle;
  Bit_Chain *old_dat = NULL, *str_dat, *hdl_dat;
  Object_Map *restrict omap;

  old_dat = dat;
  /*------------------------------------------------------------
   * Objects
   */
  SINCE (R_2004a)
  {
    sec_id = SECTION_OBJECTS;
    bit_chain_alloc (&sec_dat[sec_id]);
    str_dat = hdl_dat = dat = &sec_dat[sec_id];
    bit_chain_set_version (dat, old_dat);
    dat->byte += 4; // r2004 object map offsets start after a 4-byte prefix
  }
  LOG_INFO ("\n=======> Objects: %4zu\n", dat->byte);
  size_adr = dat->byte;

  /* Sort object-map by ascending handles
   */
  LOG_TRACE ("num_objects: %i\n", dwg->num_objects);
  LOG_TRACE ("num_object_refs: %i\n", dwg->num_object_refs);
  omap = (Object_Map *)CALLOC (dwg->num_objects, sizeof (Object_Map));
  if (!omap)
    {
      LOG_ERROR ("Out of memory");
      return DWG_ERR_OUTOFMEM;
    }
  if (DWG_LOGLEVEL >= DWG_LOGLEVEL_HANDLE)
    {
      LOG_HANDLE ("\nSorting objects...\n");
      for (i = 0; i < dwg->num_objects; i++)
        fprintf (OUTPUT, "Object(%3i): " FORMAT_HV " / idx: %u\n", i,
                 dwg->object[i].handle.value, dwg->object[i].index);
    }
  // init unsorted
  for (i = 0; i < dwg->num_objects; i++)
    {
      Dwg_Object *obj = &dwg->object[i];
      if (obj->type == DWG_TYPE_UNUSED)
        {
          LOG_TRACE ("Skip unused object %s " FORMAT_BL " " FORMAT_HV "\n",
                     obj->name ? obj->name : "", i, obj->handle.value)
          continue;
        }
      if (obj->type == DWG_TYPE_FREED)
        {
          LOG_TRACE ("Skip freed object %s " FORMAT_BL " " FORMAT_HV "\n",
                     obj->name ? obj->name : "", i, obj->handle.value)
          continue;
        }
      omap[i].index = i; // i.e. dwg->object[j].index
      omap[i].handle = dwg->object[i].handle.value;
    }
  // insertion sort
  for (i = 0; i < dwg->num_objects; i++)
    {
      Object_Map tmap;
      j = i;
      tmap = omap[i];
      while (j > 0 && omap[j - 1].handle > tmap.handle)
        {
          omap[j] = omap[j - 1];
          j--;
        }
      omap[j] = tmap;
    }
  if (DWG_LOGLEVEL >= DWG_LOGLEVEL_HANDLE)
    {
      LOG_HANDLE ("\nSorted handles:\n");
      for (i = 0; i < dwg->num_objects; i++)
        fprintf (OUTPUT, "Handle(%3i): " FORMAT_HV " / idx: " FORMAT_BL "\n",
                 i, omap[i].handle, omap[i].index);
    }

  UNTIL (R_2002)
  {
    dwg->objfreespace.objects_address = dat->byte & UINT32_MAX;
  }
  /* Write the sorted objects
   */
  for (i = 0; i < dwg->num_objects; i++)
    {
      Dwg_Object *obj;
      BITCODE_BL index = omap[i].index;
      BITCODE_UMC hdloff = omap[i].handle - (i ? omap[i - 1].handle : 0);
      BITCODE_MC off = (dat->byte - (i ? omap[i - 1].address : 0)) & INT32_MAX;
      size_t end_address;
      if (!index && !omap[i].handle)
        continue; // skipped objects
      LOG_TRACE ("\n> Next object: " FORMAT_BL " Handleoff: " FORMAT_UMC
                 " [UMC] Offset: " FORMAT_MC " [MC] @%" PRIuSIZE "\n"
                 "==========================================\n",
                 i, hdloff, off, dat->byte);
      omap[i].address = dat->byte;
      if (index > dwg->num_objects)
        {
          LOG_ERROR ("Invalid object map index " FORMAT_BL ", max " FORMAT_BL
                     ". Skipping",
                     index, dwg->num_objects)
          error |= DWG_ERR_VALUEOUTOFBOUNDS;
          continue;
        }
      obj = &dwg->object[index];
      if (obj->type == DWG_TYPE_UNUSED || obj->type == DWG_TYPE_FREED)
        {
          continue;
        }
        // change the address to the linearly sorted one
#ifndef NDEBUG
      PRE (R_2004a)
      {
        assert (dat->byte);
      }
#endif
      if (!obj->parent)
        obj->parent = dwg;
      error |= dwg_encode_add_object (obj, dat, dat->byte);

#ifndef NDEBUG
      // check if this object overwrote at address 0. but with r2004 it starts
      // fresh.
      if (dwg->header.version >= R_1_2 && dwg->header.version < R_2004)
        {
          if (dat->size < 6 || dat->chain[0] != 'A' || dat->chain[1] != 'C')
            {
              LOG_ERROR ("Encode overwrite pos 0, invalid DWG magic");
              return DWG_ERR_INVALIDDWG;
            }
          assert (dat->size > 6);
          assert (dat->chain[0] == 'A');
          assert (dat->chain[1] == 'C');
        }
#endif
      end_address = omap[i].address + (size_t)obj->size; // from RL
      if (end_address > dat->size)
        {
          assert (obj->size < DWG_MAX_OBJSIZE);
          bit_chain_alloc_size (dat, end_address - dat->size);
        }
    }

  if (DWG_LOGLEVEL >= DWG_LOGLEVEL_HANDLE)
    {
      LOG_HANDLE ("\nSorted objects:\n");
      for (i = 0; i < dwg->num_objects; i++)
        LOG_HANDLE ("Object(%d): " FORMAT_HV " / Address: %" PRIuSIZE
                    " / Idx: " FORMAT_BL "\n",
                    i, omap[i].handle, omap[i].address, omap[i].index);
    }
  bit_write_CRC (dat, size_adr, 0xC0C1);

  /*------------------------------------------------------------
   * Object-map
   * split into chunks of max. 2030
   */
  LOG_INFO ("\n=======> Object Map: %4zu\n", dat->byte);
  SINCE (R_2004a)
  {
    sec_id = SECTION_HANDLES;
    bit_chain_init_dat (&sec_dat[sec_id], (8 * dwg->num_objects) + 32, dat);
    str_dat = hdl_dat = dat = &sec_dat[sec_id];
    size_adr = 0;
    dat->byte += 2; // reserve initial RS_BE page size
  }
  else
  {
    sec_id = (Dwg_Section_Type)SECTION_HANDLES_R13;
    dwg->header.section[sec_id].number = 2;
    dwg->header.section[sec_id].address = dat->byte;
    size_adr
        = dat->byte; // Correct value of section size must be written later
    dat->byte += 2;
  }

  last_offset = 0;
  last_handle = 0;
  for (i = 0; i < dwg->num_objects; i++)
    {
      BITCODE_BL index;
      BITCODE_UMC handleoff;
      BITCODE_MC offset;

      index = omap[i].index;
      if (!index && !omap[i].handle)
        continue; // skipped objects
      handleoff = omap[i].handle - last_handle;
      bit_write_UMC (dat, handleoff);
      LOG_HANDLE ("Handleoff(%3i): " FORMAT_UMC " [UMC] (" FORMAT_HV "), ",
                  index, handleoff, omap[i].handle)
      last_handle = omap[i].handle;

      offset = (omap[i].address - last_offset) & INT32_MAX;
      bit_write_MC (dat, offset);
      last_offset = omap[i].address;
      LOG_HANDLE ("Offset: " FORMAT_MC " [MC] @%" PRIuSIZE "\n", offset,
                  last_offset);

      ckr_missing = 1;
      if (dat->byte - size_adr > 2030) // 2029
        {
          ckr_missing = 0;
          assert (size_adr || dwg->header.version >= R_2004);
#ifdef ENCODE_PATCH_RSSIZE
          encode_patch_RSsize (dat, size_adr);
#else
          sec_size = (dat->byte - size_adr) & UINT_MAX;
          dat->chain[size_adr] = sec_size >> 8;
          dat->chain[size_adr + 1] = sec_size & 0xFF;
          LOG_TRACE ("Handles page size: %u [RS_BE] @%" PRIuSIZE "\n",
                     sec_size, size_adr);
#endif
          bit_write_CRC_BE (dat, size_adr, 0xC0C1);

          size_adr = dat->byte;
          dat->byte += 2;
          last_offset = 0;
          last_handle = 0;
        }
    }
  // printf ("Obj size: %u\n", i);
  if (ckr_missing)
    {
#ifndef NDEBUG
      PRE (R_2004a)
      {
        assert (size_adr);
      }
#endif
#ifdef ENCODE_PATCH_RSSIZE
      encode_patch_RSsize (dat, size_adr);
#else
      sec_size = (dat->byte - size_adr) & UINT_MAX;
      dat->chain[size_adr] = sec_size >> 8;
      dat->chain[size_adr + 1] = sec_size & 0xFF;
      LOG_TRACE ("Handles page size: %u [RS_BE] @%" PRIuSIZE "\n", sec_size,
                 size_adr);
#endif
      bit_write_CRC_BE (dat, size_adr, 0xC0C1);
    }
#ifndef NDEBUG
  if (dwg->header.version >= R_1_2 && dwg->header.version < R_2004)
    {
      if (dat->size < 4 || dat->chain[0] != 'A' || dat->chain[1] != 'C')
        {
          LOG_ERROR ("Encode overwrite pos 0");
          return DWG_ERR_INVALIDDWG;
        }
      assert (dat->chain[0] == 'A');
      assert (dat->chain[1] == 'C');
    }
  PRE (R_2004a)
  {
    assert (dat->byte);
  }
#endif
  size_adr = dat->byte;
  bit_write_RS_BE (dat, 2); // last section_size 2
  LOG_TRACE ("Handles page size: %u [RS_BE] @%" PRIuSIZE "\n", 2, size_adr);
  bit_write_CRC_BE (dat, size_adr, 0xC0C1);

  /* Calculate and write the size of the object map
   */
  dwg->header.section[sec_id].size
      = (dat->byte - dwg->header.section[sec_id].address) & 0xFFFFFFFF;
  FREE (omap);
  return error;
}

static int
encode_objfreespace_2ndheader (Dwg_Data *restrict dwg, Bit_Chain *restrict dat)
{
  int error = 0;

  /*------------------------------------------------------------
   * ObjFreeSpace and Second header - r13-r2000 only.
   * Note: partially also since r2004.
   */
  if (dwg->header.version >= R_13 && dwg->header.version < R_2004
      && dwg->header.num_sections > 3)
    {
      assert (dat->byte);
      dwg->header.section[SECTION_OBJFREESPACE_R13].number = 3;
      if (dwg->objfreespace.numnums)
        {
          LOG_INFO ("\n=======> ObjFreeSpace 3 (start): %4u\n",
                    (unsigned)dat->byte);
          dwg->header.section[SECTION_OBJFREESPACE_R13].address = dat->byte;
          dwg->header.section[SECTION_OBJFREESPACE_R13].size = 53;
          error |= encode_objfreespace_private (dat, dwg);
          LOG_INFO ("=======> ObjFreeSpace 3 (end): %4u\n",
                    (unsigned)dat->byte);
        }
    }

  if (dwg->header.version >= R_13 && dwg->header.version < R_2004
      && dwg->secondheader.codepage)
    {
      struct _dwg_secondheader *_obj = &dwg->secondheader;
      Dwg_Object *obj = NULL;
      size_t size_adr;
      BITCODE_BL i;

      LOG_INFO ("\n=======> Second Header: %4zu\n", dat->byte + 16);
      write_sentinel (dat, DWG_SENTINEL_2NDHEADER_BEGIN);
      size_adr = dat->byte;
      dwg->secondheader.address = (BITCODE_RL)(size_adr - 16) & UINT32_MAX;
      dwg->fhdr.r2004_header.secondheader_address = size_adr - 16;
      if (!_obj->sections[SECTION_TEMPLATE_R13].address
          && section_find (section_order, dwg->header.num_sections,
                           SECTION_TEMPLATE_R13))
        {
          // Template address may change when this size changes. But in ACAD
          // dwg's, the template address is just zeroed.
          dwg->header.section[SECTION_TEMPLATE_R13].number
              = _obj->sections[SECTION_TEMPLATE_R13].nr = 4;
          dwg->header.section[SECTION_TEMPLATE_R13].address
              = _obj->sections[SECTION_TEMPLATE_R13].address = 0;
          dwg->header.section[SECTION_TEMPLATE_R13].size
              = _obj->sections[SECTION_TEMPLATE_R13].size = 0;
        }
      if (!_obj->size && !_obj->num_sections)
        {
          const char *code = dwg_version_codes (dwg->header.version);
          LOG_TRACE ("Use secondheader defaults...\n");
          strcpy ((char *)&_obj->version[0], &code[0]);
          memset (&_obj->version[7], 0, 4);
          // TODO detect what it is
          _obj->junk_r14 = UINT64_C (0x989543D074AE8021);
        }
      _obj->is_maint = dwg->header.is_maint;
      _obj->zero_one_or_three = dwg->header.zero_one_or_three;
      _obj->dwg_versions
          = (BITCODE_RS)(((BITCODE_RS)dwg->header.maint_version << 8)
                         | dwg->header.dwg_version);
      _obj->codepage = dwg->header.codepage;
      // always recompute sections, even with dwgrewrite
      if (dwg->header.sections > 6)
        dwg->header.sections = 6;
      _obj->num_sections = dwg->header.sections;
      for (i = 0; i < MIN (dwg->header.sections, 7U); i++)
        {
          _obj->sections[i].nr = dwg->header.section[i].number;
          _obj->sections[i].address = dwg->header.section[i].address;
          _obj->sections[i].size = dwg->header.section[i].size;
        }
      // always set handles from the header vars
      if (!_obj->num_handles)
        _obj->num_handles = 14;

#define SET_HDL(i, NAM)                                                       \
  _obj->handles[i].nr = i;                                                    \
  if (dwg->header_vars.NAM                                                    \
      && _obj->handles[i].num_hdl != dwg->header_vars.NAM->handleref.size)    \
    {                                                                         \
      _obj->handles[i].num_hdl                                                \
          = MAX (dwg->header_vars.NAM->handleref.size, 8);                    \
    }                                                                         \
  if (dwg->header_vars.NAM)                                                   \
    {                                                                         \
      unsigned char chain[8];                                                 \
      Bit_Chain hdat                                                          \
          = { chain, 8L, 0L, 0, 0, R_INVALID, R_INVALID, NULL, 30 };          \
      bit_H_to_dat (&hdat, &dwg->header_vars.NAM->handleref);                 \
      _obj->handles[i].name = #NAM;                                           \
      for (int k = 0; k < MIN ((int)_obj->handles[i].num_hdl, 8); k++)        \
        _obj->handles[i].hdl[k] = hdat.chain[k];                              \
    }

      SET_HDL (0, HANDSEED);
      SET_HDL (1, BLOCK_CONTROL_OBJECT);
      SET_HDL (2, LAYER_CONTROL_OBJECT);
      SET_HDL (3, STYLE_CONTROL_OBJECT);
      SET_HDL (4, LTYPE_CONTROL_OBJECT);
      SET_HDL (5, VIEW_CONTROL_OBJECT);
      SET_HDL (6, UCS_CONTROL_OBJECT);
      SET_HDL (7, VPORT_CONTROL_OBJECT);
      SET_HDL (8, APPID_CONTROL_OBJECT);
      SET_HDL (9, DIMSTYLE_CONTROL_OBJECT);
      SET_HDL (10, VX_CONTROL_OBJECT);
      SET_HDL (11, DICTIONARY_NAMED_OBJECT);
      SET_HDL (12, DICTIONARY_ACAD_MLINESTYLE);
      SET_HDL (13, DICTIONARY_ACAD_GROUP);

      encode_secondheader_private (dat, dwg);
      dwg->secondheader.size
          = (BITCODE_RL)(dat->byte - size_adr + 16) & UINT32_MAX;
      encode_patch_RLsize (dat, size_adr);
      bit_write_CRC (dat, size_adr, 0xC0C1);
      VERSIONS (R_14, R_2000)
      {
        FIELD_RLL (junk_r14, 0);
      }
      write_sentinel (dat, DWG_SENTINEL_2NDHEADER_END);
    }
  return error;
}

/**
 * dwg_encode(): the current generic encoder entry point.
 *
 * TODO: 2007 maps.
 * 2010+ uses the 2004 format.
 * Returns a summary bitmask of all errors.
 * Skip all MATERIAL objects, DICTS and common properties
 */
AFL_GCC_TOOBIG
EXPORT int
dwg_encode (Dwg_Data *restrict dwg, Bit_Chain *restrict dat)
{
  int error = 0;
  BITCODE_BL i, j;
  size_t section_address, header_crc_address = 0;
  size_t size_adr;
  unsigned int sec_size = 0;
  Bit_Chain *old_dat = NULL, *str_dat, *hdl_dat;
  Dwg_Section_Type sec_id;
  Dwg_Version_Type orig_from_version = dwg->header.from_version;
  Bit_Chain sec_dat[SECTION_SYSTEM_MAP + 1]
      = { 0 }; // to encode each r2004 section

  dwg->cur_index = 0;
  if (dwg->opts)
    loglevel = dwg->opts & DWG_OPTS_LOGLEVEL;
#ifdef USE_TRACING
  /* Before starting, set the logging level, but only do so once.  */
  if (!env_var_checked_p)
    {
      char *probe = getenv ("LIBREDWG_TRACE");
      if (probe)
        loglevel = atoi (probe);
      env_var_checked_p = true;
    }
#endif /* USE_TRACING */

  if (dwg->header.version != dwg->header.from_version)
    LOG_TRACE ("Encode version %s (%s) from version %s (%s)\n",
               dwg_version_codes (dwg->header.version),
               dwg_version_type (dwg->header.version),
               dwg_version_codes (dwg->header.from_version),
               dwg_version_type (dwg->header.from_version))
  else
    LOG_TRACE ("Encode version %s (%s)\n",
               dwg_version_codes (dwg->header.version),
               dwg_version_type (dwg->header.version));

#ifdef ENCODE_UNKNOWN_AS_DUMMY
  // We cannot write unknown_bits into another version, or when it's coming
  // from DXF. Same-version JSON imports keep the raw unknown_bits dump.
  // Otherwise write a PLACEHOLDER/DUMMY or POINT instead. Later maybe PROXY.
  // This is controversial and breaks roundtrip tests, but helps
  // ACAD imports.
  if (dwg_supports_eed (dwg)
      && (dwg->header.version != dwg->header.from_version
          || (dwg->opts & DWG_OPTS_INDXF)))
    {
      int fixup = 0;
      // Scan for invalid/unstable/unsupported objects and entities
      // and eliminate or replace them with placeholders.
      LOG_TRACE ("Scan for unsupported objects\n");
      for (i = 0; i < dwg->num_objects; i++)
        {
          Dwg_Object *obj = &dwg->object[i];
          if (obj->fixedtype == DWG_TYPE_UNKNOWN_OBJ
              || obj->fixedtype == DWG_TYPE_UNKNOWN_ENT
          // WIPEOUT causes hang, TABLEGEOMETRY crash, MATERIAL causes ODA
          // errors. Keep same-version JSON imports untouched.
#  ifndef DEBUG_CLASSES
              || (dwg->opts & DWG_OPTS_INDXF
                  && (/*obj->fixedtype == DWG_TYPE_WIPEOUT (GH #244) || */
                      obj->fixedtype == DWG_TYPE_TABLEGEOMETRY
                      || obj->fixedtype == DWG_TYPE_MATERIAL))
#  endif
          )
            {
              fixup++;
              break;
            }
        }
      if (fixup)
        {
          BITCODE_RLL new_appid;
          BITCODE_BS placeholder_type = 0;
          LOG_TRACE ("Found unsupported objects, add APPID LibreDWG\n");
          new_appid = add_LibreDWG_APPID (dwg);
          if (new_appid)
            {
              fixup = 0;
              // if not found leaves placeholder_type at 0 to use DUMMY
              LOG_HANDLE ("find_class ACDBPLACEHOLDER\n");
              dwg_find_class (dwg, "ACDBPLACEHOLDER", &placeholder_type);
              if (placeholder_type < 500)
                {
                  LOG_ERROR ("Invalid class_id %d for ACDBPLACEHOLDER",
                             (int)placeholder_type);
                  placeholder_type = 0;
                }
              for (i = 0; i < dwg->num_objects; i++)
                {
                  Dwg_Object *obj = &dwg->object[i];
                  if (obj->fixedtype == DWG_TYPE_UNKNOWN_OBJ
                      || obj->fixedtype == DWG_TYPE_UNKNOWN_ENT
#  ifndef DEBUG_CLASSES
                      || (dwg->opts & DWG_OPTS_INDXF
                          && (/*obj->fixedtype == DWG_TYPE_WIPEOUT (GH #244) ||
                               */
                              obj->fixedtype == DWG_TYPE_TABLEGEOMETRY
                              || obj->fixedtype == DWG_TYPE_MATERIAL))
#  endif
                  )
                    {
                      fixup++;
                      // replace entities with points, objects with
                      // placeholders
                      LOG_HANDLE ("encode_unknown_as_dummy\n");
                      encode_unknown_as_dummy (dat, obj, placeholder_type);
                    }
                  // what to do with links to MATERIAL/...
                  if (dwg->header.version >= R_13b1 && obj->handle.value == 0xC
                      && obj->fixedtype == DWG_TYPE_DICTIONARY)
                    fixup_NOD (dwg, obj); // named object dict
                }
              LOG_TRACE ("Fixed %d unsupported objects\n\n", fixup);
            }
        }
    }
#endif
  if (dwg_supports_eed (dwg) && dwg->header.version < R_2010
      && dwg->header.from_version >= R_2010)
    {
      // Scan for objects with EED for class_version fields.
      LOG_TRACE ("Scan for downconverting objects to EED\n");
      for (i = 0; i < dwg->num_objects; i++)
        {
          Dwg_Object *obj = &dwg->object[i];
          if (obj->fixedtype == DWG_TYPE_MLEADERSTYLE)
            downconvert_MLEADERSTYLE (obj);
          else if (obj->fixedtype == DWG_TYPE_DIMSTYLE)
            downconvert_DIMSTYLE (dat, obj); // so far Annotative only
        }
    }

  bit_chain_alloc (dat);
  hdl_dat = dat; // split later in objects/entities
  if (!dat->version)
    {
      dat->version = dwg->header.version;
      dat->from_version = dwg->header.from_version;
      dat->opts = dwg->opts;
    }
  dat->codepage = dwg->header.codepage;

  if (dwg->header.from_version > R_2000 && dwg->header.version <= R_2000)
    {
      remove_EXEMPT_FROM_CAD_STANDARDS_APPID (dat, dwg);
    }

  /*------------------------------------------------------------
   * Header
   */
  strcpy ((char *)dat->chain,
          dwg_version_codes (dwg->header.version)); // Chain version
  dat->byte += 11;

  {
    BITCODE_BL vcount;
    // set dwg_version from release
    const struct dwg_versions *_verp
        = dwg_version_struct (dwg->header.version);
    Dwg_Header *_obj = &dwg->header;
    Dwg_Object *obj = NULL;
    if ((dwg->header.version < R_13b1 && !_obj->numheader_vars) // ie from DXF
        || (dwg->header.version >= R_13b1 && !_obj->dwg_version))
      {
        _obj->zero_one_or_three = 1;
        if (_verp)
          _obj->dwg_version = _verp->dwg_version;
        if (dwg->header.version > R_13b1)
          {
            // can be improved with r2004 by another lookup table
            _obj->is_maint = 0xf;
            _obj->maint_version = 29;
          }
        /*
        switch (dwg->header.version)
          {
          case R_9:
            _obj->dwg_version = 0x0b;
            break;
          case R_10:
            _obj->dwg_version = 0x0d;
            break;
          case R_11:
            _obj->dwg_version = 0x10;
            break;
          case R_13:
            _obj->dwg_version = 0x13;
            break;
          case R_13c3:
            _obj->dwg_version = 0x14;
            break;
          case R_14:
            _obj->dwg_version = 0x15;
            break;
          case R_2000:
            _obj->dwg_version = 0x17;
            _obj->is_maint = 0xf;
            break;
          case R_2004:
            _obj->dwg_version = 0x19; // or 0x18/0x1a
            _obj->is_maint = 0x68;
            break;
          case R_2007:
            _obj->dwg_version = 0x1b;
            _obj->is_maint = 0x32;
            break;
          case R_2010:
            _obj->dwg_version = 0x1d;
            _obj->is_maint = 0x6d;
            break;
          case R_2013:
            _obj->dwg_version = 0x1f;
            _obj->is_maint = 0x7d;
            break;
          case R_2018:
            _obj->dwg_version = 0x21;
            _obj->is_maint = 0x1d;
            break;
          case R_INVALID:
          case R_AFTER:
          case R_1_1:
          case R_1_2:
          case R_1_3:
          case R_1_4:
          case R_2_0:
          case R_2_1:
          case R_2_21:
          case R_2_22:
          case R_2_4:
          case R_2_5:
          case R_2_6:
          case R_9c1:
          case R_11b1:
          case R_11b2:
          case R_12:
          default:
            break;
          }
          */
        if (!_obj->app_dwg_version)
          _obj->app_dwg_version = _obj->dwg_version;
      }
    if (!_obj->codepage)
      _obj->codepage = dat->codepage;
    if (!_obj->blocks_size)
      _obj->blocks_size = 0x40000000;
    if (!_obj->extras_size)
      _obj->extras_size = 0x80000000;
    VERSIONS (R_2_0b, R_13b1)
    {
      _obj->numentity_sections = 3;
    }
    SINCE (R_2004a)
    {
      if (!_obj->r2004_header_address)
        _obj->r2004_header_address = 128;
    }
    if (!_obj->num_sections || !_obj->sections)
      dwg_sections_init (dwg);

      // clang-format off
    #include "header.spec"
    // clang-format on
  }
  section_address = dat->byte;

#define WE_CAN                                                                \
  "This version of LibreDWG is only capable of encoding "                     \
  "versions r1.1-r2004 (code: MC0.0-AC1018) DWG files.\n"

  PRE (R_13b1)
  {
    BITCODE_RL numentities, addr;
    size_t hdr_offset, hdr_end;
    BITCODE_BL last_entity_idx, end_idx;
    BITCODE_BLd first_entity_idx = 0;
    Dwg_Object *first_block;

    if (dwg->header.version == R_INVALID
        || dwg->header.from_version == R_INVALID)
      {
        LOG_ERROR (WE_CAN "Invalid or missing FILEHEADER.version");
        return DWG_ERR_INVALIDDWG;
      }

    SINCE (R_2_0b)
    {
      if (!dwg->header.section
          || dwg->header.version != dwg->header.from_version)
        {
          dwg_sections_init (dwg);
        }
      // get the tables from the CONTROL objects
      encode_preR13_section_hdr ("BLOCK", SECTION_BLOCK, dat, dwg);
      encode_preR13_section_hdr ("LAYER", SECTION_LAYER, dat, dwg);
      encode_preR13_section_hdr ("STYLE", SECTION_STYLE, dat, dwg);
      encode_preR13_section_hdr ("LTYPE", SECTION_LTYPE, dat, dwg);
      encode_preR13_section_hdr ("VIEW", SECTION_VIEW, dat, dwg);
      /* The rest is embedded into header_variables_r11.spec below */
    }

    hdr_offset = dat->byte;
    encode_preR13_header_variables (dat, dwg);
    hdr_end = dat->byte;
    // the sentinel starts 16 before entities_start
    SINCE (R_11)
    {
      if (!dwg->header.num_sections)
        dwg->header.num_sections = SECTION_VX;
      bit_write_RS (dat, 0); // crc placeholder
      LOG_TRACE ("crc 0 [RSx]\n");
      write_sentinel (dat, DWG_SENTINEL_R11_ENTITIES_BEGIN);
    }
    else if (!dwg->header.num_sections) dwg->header.num_sections
        = dwg->header.numheader_vars <= 129   ? SECTION_VIEW
          : dwg->header.numheader_vars <= 158 ? SECTION_VPORT
          : dwg->header.numheader_vars <= 160 ? SECTION_APPID
                                              : SECTION_VX;
    dwg->header.entities_start = dat->byte & 0xFFFFFFFF;
    // LOG_TRACE ("\nentities 0x%x:\n", dwg->header.entities_start);
    dwg->cur_index = 0;
    // collect all entities (non-blocks)
    numentities
        = encode_preR13_entities (ENTITIES_SECTION_INDEX, dat, dwg, &error);
    dwg->cur_index += numentities;
    dwg->header.entities_end = dat->byte & 0xFFFFFFFF;
    LOG_TRACE ("\nentities %u 0x%x - 0x%x\n", numentities,
               dwg->header.entities_start, dwg->header.entities_end);
    write_sentinel (dat, DWG_SENTINEL_R11_ENTITIES_END);

    // patch all the section tbl->address
    addr = dwg->header.entities_end + (dat->version >= R_11 ? 0x20 : 0);
    encode_check_num_sections ((Dwg_Section_Type_r11)dwg->header.num_sections,
                               dwg);
    if (dwg->header.from_version >= R_13b1)
      {
        /* r2000 has e.g.
         * section 0: header vars
         *         1: class section
         *         2: object map
         *         3: optional: ObjFreeSpace
         *            optional: SecondHeader
         *         4: and Template
         *         5: optional: AuxHeader (no sentinels, since R13c3)
         */
        LOG_ERROR ("FIXME convert sections from CONTROL objects to tables");
      }
    if (dwg->header.from_version < R_13b1 && dwg->header.section)
      {
        for (int id = (int)SECTION_BLOCK; id <= (int)dwg->header.num_sections;
             id++)
          {
            Dwg_Section *tbl = &dwg->header.section[id];
            BITCODE_RL pos = dat->byte & 0xFFFFFFFF;
            if (!tbl || !tbl->address)
              continue;
            dat->byte = tbl->address;
            LOG_TRACE ("%s.address => " FORMAT_RLx " [RLx] @%u.0\n", tbl->name,
                       addr, (unsigned)tbl->address);
            bit_write_RL (dat, addr);
            tbl->address = addr;
            addr += tbl->size * tbl->number;
            if (dat->version >= R_11)
              addr += 0x20;
            dat->byte = pos;
          }
      }

    PRE (R_2_0b)
    {
      // patch these numbers into the header
      BITCODE_RL dwg_size = dat->byte & 0xFFFFFFFF;
      dat->byte = 0x0c + 24;
      if (dwg_size != dwg->header_vars.dwg_size)
        LOG_TRACE ("-dwg_size: %u [RL] @%" PRIuSIZE ".0\n", (unsigned)dwg_size,
                   dat->byte);
      bit_write_RL (dat, dwg_size);
      if (numentities != dwg->header_vars.numentities)
        LOG_TRACE ("-numentities: %u [RS] @%" PRIuSIZE ".0\n",
                   (unsigned)numentities, dat->byte);
      bit_write_RS (dat, numentities & 0xFFFF);
      dat->byte = dwg_size;
      LOG_TRACE ("Wrote %u bytes\n", (unsigned)dwg_size);
      return error;
    }
    SINCE (R_2_0b)
    {
      BITCODE_RL num_block_entities, num_extra_entities, blocks_end,
          extras_end, jump_index;
      BITCODE_RL endblk_index = dwg->num_objects - 1;
      Dwg_Object *last_endblk;

      error |= encode_preR13_section (SECTION_BLOCK, dat, dwg);
      error |= encode_preR13_section (SECTION_LAYER, dat, dwg);
      error |= encode_preR13_section (SECTION_STYLE, dat, dwg);
      error |= encode_preR13_section (SECTION_LTYPE, dat, dwg);
      error |= encode_preR13_section (SECTION_VIEW, dat, dwg);

      if (dwg->header.num_sections >= SECTION_VPORT)
        {
          error |= encode_preR13_section (SECTION_UCS, dat, dwg);
          error |= encode_preR13_section (SECTION_VPORT, dat, dwg);
        }
      if (dwg->header.num_sections >= SECTION_APPID)
        error |= encode_preR13_section (SECTION_APPID, dat, dwg);
      if (dwg->header.num_sections >= SECTION_VX)
        {
          error |= encode_preR13_section (SECTION_DIMSTYLE, dat, dwg);
          error |= encode_preR13_section (SECTION_VX, dat, dwg);
        }
      // encode block entities
      write_sentinel (dat, DWG_SENTINEL_R11_BLOCK_ENTITIES_BEGIN);
      dwg->header.blocks_start = dat->byte & 0xFFFFFFFF;
      if (dat->version >= R_2_0b
          && (first_block = dwg_find_first_type (dwg, DWG_TYPE_BLOCK)))
        {
          num_block_entities = encode_preR13_entities (BLOCKS_SECTION_INDEX,
                                                       dat, dwg, &error);
          dwg->header.blocks_size
              = (dat->byte - dwg->header.blocks_start) & 0xFFFFFFFF;
        }
      else
        {
          num_block_entities = 0;
          dwg->header.blocks_size = 0;
        }
      write_sentinel (dat, DWG_SENTINEL_R11_BLOCK_ENTITIES_END);
      if (dwg->header.version > R_2_22)
        dwg->header.blocks_size += 0x40000000;
      LOG_TRACE ("\nblock_entities %u 0x%x - 0x%x (0x%x)\n",
                 num_block_entities, dwg->header.blocks_start,
                 dwg->header.blocks_start
                     + (dwg->header.blocks_size & 0xffffff),
                 dwg->header.blocks_size);

      // encode extra entities.
      write_sentinel (dat, DWG_SENTINEL_R11_EXTRA_ENTITIES_BEGIN);
      dwg->header.extras_start = dat->byte & 0xFFFFFFFF;
      if (dwg->num_objects)
        {
          num_extra_entities = encode_preR13_entities (EXTRAS_SECTION_INDEX,
                                                       dat, dwg, &error);
          dwg->header.extras_size
              = 0x80000000
                + ((dat->byte - dwg->header.extras_start) & 0x8FFFFFFF);
        }
      else
        num_extra_entities = 0;
      write_sentinel (dat, DWG_SENTINEL_R11_EXTRA_ENTITIES_END);
      LOG_TRACE ("\nextra_entities %u 0x%x - 0x%x (0x%x)\n",
                 num_extra_entities, dwg->header.extras_start,
                 dwg->header.extras_start
                     + (dwg->header.extras_size & 0xffffff),
                 dwg->header.extras_size);
      PRE (R_10)
      {
        if (dwg->header_vars.numentities != numentities + num_block_entities)
          {
            dwg->header_vars.numentities = numentities + num_block_entities;
            LOG_WARN ("Fixed numentities: %u\n", dwg->header_vars.numentities);
          }
      }
      SINCE (R_11)
      {
        error |= encode_r11_auxheader (dat, dwg);
      }
      addr = dat->byte & 0xFFFFFFFF;
      LOG_TRACE ("@0x%x -> ", addr);
      if (error >= DWG_ERR_CRITICAL)
        return error;
      // patch these numbers into the header
      dat->byte = 0x14; // header section_address
      dat->bit = 0;
      bit_write_RL (dat, dwg->header.entities_start);
      LOG_TRACE ("0x14:\nentities_start: 0x%x [RLx]\n",
                 dwg->header.entities_start);
      bit_write_RL (dat, dwg->header.entities_end);
      LOG_TRACE ("entities_end: 0x%x [RLx]\n", dwg->header.entities_end);
      bit_write_RL (dat, dwg->header.blocks_start);
      LOG_TRACE ("blocks_start: 0x%x [RLx]\n", dwg->header.blocks_start);
      bit_write_RL (dat, dwg->header.blocks_size);
      LOG_TRACE ("blocks_size: 0x%x [RLx]\n", dwg->header.blocks_size);
      bit_write_RL (dat, dwg->header.extras_start);
      LOG_TRACE ("extras_start: 0x%x [RLx]\n", dwg->header.extras_start);
      bit_write_RL (dat, dwg->header.extras_size);
      LOG_TRACE ("extras_size: 0x%x [RLx]\n", dwg->header.extras_size);
      SINCE (R_11)
      {
        BITCODE_RS crc;
        dat->byte = dwg->header.entities_start - 18;
        assert (dat->byte == hdr_end);
        crc = bit_calc_CRC (0xC0C1, &dat->chain[0], dat->byte);
        LOG_TRACE ("crc: %04X [RSx] from 0-0x%zx\n", crc, dat->byte); // -0x6bd
        bit_write_RS (dat, crc);
      }
    }
    VERSIONS (R_2_0b, R_9c1)
    {
      LOG_TRACE ("@0x%zx ->", dat->byte);
      dat->byte = hdr_offset + (3 * 8);
      LOG_TRACE ("0x%zx", dat->byte);
      bit_write_RS (dat, dwg->header_vars.numentities);
      LOG_TRACE ("numentities: %u [RS]\n", dwg->header_vars.numentities);
    }
    dat->byte = addr;
    LOG_TRACE ("Wrote %" PRIuSIZE " bytes\n", dat->byte);
    dat->size = addr;
    return error;
  }

  VERSIONS (R_13b1, R_2004)
  {
    /* compute the r2000 section order:
     * section 0: Header vars
     *         1: Classes
                  Template (r13 only, optional)
                  padding (r13c3+)
                  THUMBNAIL
     *         2: Handles
     *         3: ObjFreeSpace (r13c3+, optional)
                  + 2NDHEADER (r13-r2000)
     *         4: Template (r14-r2000, optional)
     *         5: AuxHeader (r2000, no sentinels)
     *         6: THUMBNAIL (r13c3+, not a section)
     */

    /* Usually 3-5, max 6 */
    if (!dwg->header.num_sections
        || (dat->from_version >= R_2004 && dwg->header.num_sections > 6))
      {
        if (dwg->header.version <= R_2000)
          {
            if (dwg->secondheader.num_sections)
              dwg->header.num_sections = dwg->secondheader.num_sections;
            else
              dwg->header.num_sections
                  = dwg->auxheader.dwg_version && dwg->header.version == R_2000
                        ? 6
                        : 5;
          }
        else
          dwg->header.num_sections = 6;
        // minimal DXF:
        // if (dwg->opts & (DWG_OPTS_INDXF | DWG_OPTS_MINIMAL)
        //    && (!dwg->header_vars.HANDSEED ||
        //    !dwg->header_vars.TDCREATE.days))
        //  {
        //    dwg->header.num_sections = 5;
        //    // hack to trigger IF_ENCODE_FROM_EARLIER defaults. undone after
        //    // HEADER
        //    dat->from_version = R_11;
        //    if (dat->version <= dat->from_version)
        //      dat->from_version = (Dwg_Version_Type)((int)dat->version - 1);
        //  }
      }
    error |= dwg_sections_init (dwg);
    if (error >= DWG_ERR_CRITICAL)
      return error;
    dat->byte += (dwg->header.sections * 9); /* RC + 2*RL */
    header_crc_address = dat->byte;
    bit_write_CRC (dat, 0, 0xC0C1);
    write_sentinel (dat, DWG_SENTINEL_HEADER_END);

    VERSIONS (R_13b1, R_2000)
    {
      section_order[0] = SECTION_HEADER_R13;
      section_order[1] = SECTION_CLASSES_R13;
      section_order[2] = SECTION_HANDLES_R13;
      if (dwg->header.sections > 3)
        {
          section_order[3] = SECTION_OBJFREESPACE_R13;
          section_order[4] = SECTION_TEMPLATE_R13;
          if (dwg->header.sections > 5)
            {
              section_order[5] = SECTION_AUXHEADER_R2000;
              section_order[6] = SECTION_THUMBNAIL_R13;
            }
          else
            {
              section_order[5] = SECTION_THUMBNAIL_R13;
            }
        }
      else
        section_order[3] = SECTION_THUMBNAIL_R13;

      if (DWG_LOGLEVEL >= DWG_LOGLEVEL_TRACE)
        section_order_trace (dwg, dwg->header.num_sections,
                             (Dwg_Section_Type_r13 *)&section_order);
    }

    if (dwg->header.thumbnail_address)
      {
        if (dwg->header.section[SECTION_HEADER_R13].address
            && dwg->header.thumbnail_address
                   < dwg->header.section[SECTION_HEADER_R13].address)
          {
            section_move_top ((Dwg_Section_Type_r13 *)&section_order,
                              &dwg->header.num_sections,
                              SECTION_THUMBNAIL_R13);
            // thumbnail_position = before_header; // r13 if empty
          }
        else if (dwg->secondheader.sections[SECTION_HEADER_R13].address)
          {
            if (dwg->header.thumbnail_address
                < dwg->secondheader.sections[SECTION_HEADER_R13].address)
              {
                section_move_top ((Dwg_Section_Type_r13 *)&section_order,
                                  &dwg->header.num_sections,
                                  SECTION_THUMBNAIL_R13);
                // thumbnail_position = before_header; // r13 if empty
              }
            else if (dwg->secondheader.sections[SECTION_HANDLES_R13].address
                     && dwg->header.thumbnail_address
                            < dwg->secondheader.sections[SECTION_HANDLES_R13]
                                  .address)
              {
                section_move_before ((Dwg_Section_Type_r13 *)&section_order,
                                     &dwg->header.num_sections,
                                     SECTION_THUMBNAIL_R13,
                                     SECTION_HANDLES_R13);
              }
          }
      }
    else
      {
        PRE (R_13c3)
        {
          section_move_before ((Dwg_Section_Type_r13 *)&section_order,
                               &dwg->header.num_sections,
                               SECTION_THUMBNAIL_R13, SECTION_HANDLES_R13);
        }
        SINCE (R_2004a)
        {
          section_move_top ((Dwg_Section_Type_r13 *)&section_order,
                            &dwg->header.num_sections, SECTION_THUMBNAIL_R13);
        }
      }
    VERSIONS (R_13b1, R_2000)
    {
      // auxheader before thumbnail?
      if (dwg->header.sections > 5
          && dwg->secondheader.sections[SECTION_AUXHEADER_R2000].address
          && dwg->header.thumbnail_address
                 > dwg->secondheader.sections[SECTION_AUXHEADER_R2000].address)
        {
          section_move_top ((Dwg_Section_Type_r13 *)&section_order,
                            &dwg->header.num_sections,
                            SECTION_AUXHEADER_R2000);
        }
    }
    VERSION (R_13)
    {
      section_move_before ((Dwg_Section_Type_r13 *)&section_order,
                           &dwg->header.num_sections, SECTION_TEMPLATE_R13,
                           SECTION_HANDLES_R13);
    }
    if (DWG_LOGLEVEL >= DWG_LOGLEVEL_TRACE)
      section_order_trace (dwg, dwg->header.num_sections,
                           (Dwg_Section_Type_r13 *)&section_order);
    LOG_TRACE ("num_sections => " FORMAT_RL "\n", dwg->header.num_sections);

    // on downconvert or DXF import add the missing VX_CONTROL object
    // (DXF never contains VX tables nor table records)
    if (dwg->header.version < R_2004 && !dwg->header_vars.VX_CONTROL_OBJECT)
      {
        Dwg_Object *obj = dwg_find_first_type (dwg, DWG_TYPE_VX_CONTROL);
        if (!obj)
          {
            dwg_add_VX (dwg, NULL);
            obj = dwg_find_first_type (dwg, DWG_TYPE_VX_CONTROL);
            if (obj)
              {
                obj->handle.value = 0xB;
                LOG_TRACE ("adding VX_CONTROL object " FORMAT_RLL "\n",
                           obj->handle.value);
                dwg->header_vars.VX_TABLE_RECORD
                    = dwg_add_handleref (dwg, 5, 0, NULL);
              }
          }
        if (obj)
          dwg->header_vars.VX_CONTROL_OBJECT
              = dwg_add_handleref (dwg, 3, obj->handle.value, obj);
      }
    VERSIONS (R_13b1, R_2000)
    {
      for (unsigned id = 0; id < dwg->header.num_sections; id++)
        {
          switch (section_order[id])
            {
            case SECTION_HEADER_R13:
              error |= encode_header_vars (dwg, dat, orig_from_version);
              break;
            case SECTION_CLASSES_R13:
              error |= encode_classes (dwg, dat);
              break;
            case SECTION_HANDLES_R13:
              error |= encode_objects_handles (dwg, dat, sec_dat);
              break;
            case SECTION_OBJFREESPACE_R13:
              error |= encode_objfreespace_2ndheader (dwg, dat);
              break;
            case SECTION_TEMPLATE_R13:
              error |= encode_template (dwg, dat);
              break;
            case SECTION_AUXHEADER_R2000:
              error |= encode_auxheader (dwg, dat);
              break;
            case SECTION_THUMBNAIL_R13:
              error |= encode_r13_thumbnail (dwg, dat, header_crc_address);
              break;
            default:
              LOG_WARN ("Unhandled section %u [%u]", section_order[id], id);
              break;
            }
        }
    }
    if (dwg->header.sections == 3 && dwg->secondheader.codepage)
      {
        error |= encode_objfreespace_2ndheader (dwg, dat);
      }
  } // VERSIONS (R_13b1, R_2004)

  VERSIONS (R_2007a, R_2007)
  {
    LOG_ERROR (WE_CAN "We don't encode r2007 yet");
    // dat->version = dwg->header.version = R_2010; // rather do 2010
    return DWG_ERR_NOTYETSUPPORTED;
  }

  /* r2004 file header (compressed + encrypted) */
  SINCE (R_2004a)
  {
    LOG_INFO ("\n");
    SINCE (R_2007a) // r2010 is close though
    {
      LOG_ERROR (WE_CAN "Writing >r2004 not yet supported");
      return DWG_ERR_NOTYETSUPPORTED;
    }

    // Preserve sec_dat entries already populated by the VERSIONS block
    // (SECTION_OBJECTS and SECTION_HANDLES from encode_objects_handles).
    {
      Bit_Chain saved_objects = sec_dat[SECTION_OBJECTS];
      Bit_Chain saved_handles = sec_dat[SECTION_HANDLES];
      memset (&sec_dat, 0, (SECTION_SYSTEM_MAP + 1) * sizeof (Bit_Chain));
      sec_dat[SECTION_OBJECTS] = saved_objects;
      sec_dat[SECTION_HANDLES] = saved_handles;
    }
    if (dwg->header.section_infohdr.num_desc && !dwg->header.section_info)
      dwg->header.section_info = (Dwg_Section_Info *)CALLOC (
          dwg->header.section_infohdr.num_desc, sizeof (Dwg_Section_Info));
    LOG_TRACE ("\n#### r2004 File Header ####\n");
    if (dat->byte + 0x80 >= dat->size - 1)
      {
        dwg->header.num_sections = 28; // room for some object pages
        dwg->header.section = (Dwg_Section *)CALLOC (28, sizeof (Dwg_Section));
      }
    if (!dwg->header.section_info)
      {
        dwg->header.section_infohdr.num_desc = SECTION_SYSTEM_MAP + 1;
        dwg->header.section_info = (Dwg_Section_Info *)CALLOC (
            SECTION_SYSTEM_MAP + 1, sizeof (Dwg_Section_Info));
      }
  }
  else
      /* End of the file */
      dat->size
      = dat->byte;

  SINCE (R_2004a)
  {
#ifdef __cplusplus
    int type;
#else
    Dwg_Section_Type type;
#endif
    Dwg_Object *obj = NULL;
    BITCODE_BL vcount, rcount3;
    size_t size;
    unsigned total_size = 0;
    old_dat = dat;

    // write section data into sec_dat[type] chains
    for (type = SECTION_HEADER; type < SECTION_SYSTEM_MAP; type++)
      {
        if (type != SECTION_OBJECTS && type != SECTION_PREVIEW
            && type != SECTION_UNKNOWN)
          LOG_TRACE ("\n=== Section %s ===\n", dwg_section_name (dwg, type))
        switch (type)
          {
          case SECTION_HEADER:
            bit_chain_init_dat (&sec_dat[type], sizeof (Dwg_Header) + 64, dat);
            dat = &sec_dat[type];
            error |= encode_header_vars (dwg, dat, orig_from_version);
            break;
          case SECTION_AUXHEADER:
            {
              bit_chain_alloc (&sec_dat[type]);
              str_dat = hdl_dat = dat = &sec_dat[type];
              bit_chain_set_version (dat, old_dat);
              error |= encode_auxheader (dwg, dat);
              LOG_TRACE ("-size: %" PRIuSIZE "\n", dat->byte);
            }
            break;
          case SECTION_HANDLES:
          case SECTION_OBJECTS:
            // Already populated by encode_objects_handles in VERSIONS block
            if (sec_dat[type].chain && sec_dat[type].byte)
              break;
            if (type == SECTION_HANDLES)
              {
                // Don't bit_chain_init_dat here; encode_objects_handles
                // allocates sec_dat[SECTION_HANDLES] itself. Just set
                // version so it can be copied to SECTION_OBJECTS.
                bit_chain_set_version (&sec_dat[type], dat);
                str_dat = hdl_dat = dat = &sec_dat[type];
                error |= encode_objects_handles (dwg, dat, sec_dat);
              }
            break;
          case SECTION_CLASSES:
            bit_chain_init_dat (&sec_dat[type],
                                (sizeof (Dwg_Class) * dwg->num_classes) + 32,
                                dat);
            str_dat = hdl_dat = dat = &sec_dat[type];
            error |= encode_classes (dwg, dat);
            break;
          case SECTION_TEMPLATE:
            bit_chain_init_dat (&sec_dat[type], 16, dat);
            str_dat = hdl_dat = dat = &sec_dat[type];
            error |= encode_template (dwg, dat);
            LOG_TRACE ("-size: %" PRIuSIZE "\n", dat->byte);
            break;
          case SECTION_PREVIEW:
            {
              bit_chain_alloc (&sec_dat[type]);
              str_dat = hdl_dat = dat = &sec_dat[type];
              bit_chain_set_version (dat, old_dat);
              error |= encode_r13_thumbnail (dwg, dat, header_crc_address);
              LOG_TRACE ("-size: %" PRIuSIZE "\n", dat->byte);
            }
            break;
          case SECTION_UNKNOWN: // deferred
          case SECTION_INFO:
          case SECTION_SYSTEM_MAP:
            break;
          case SECTION_OBJFREESPACE:
            {
              bit_chain_alloc (&sec_dat[type]);
              hdl_dat = dat = &sec_dat[type];
              bit_chain_set_version (dat, old_dat);
              error |= encode_objfreespace_private (dat, dwg);
              LOG_TRACE ("-size: %" PRIuSIZE "\n", dat->byte);
            }
            break;
          case SECTION_REVHISTORY:
            {
              Dwg_RevHistory *_obj = &dwg->revhistory;
              bit_chain_alloc (&sec_dat[type]);
              str_dat = hdl_dat = dat = &sec_dat[type];
              bit_chain_set_version (dat, old_dat);
#include "revhistory.spec"
              LOG_TRACE ("-size: %" PRIuSIZE "\n", dat->byte)
            }
            break;
          case SECTION_SUMMARYINFO:
            {
              Dwg_SummaryInfo *_obj = &dwg->summaryinfo;
              bit_chain_alloc (&sec_dat[type]);
              str_dat = hdl_dat = dat = &sec_dat[type];
              bit_chain_set_version (dat, old_dat);
#include "summaryinfo.spec"
              LOG_TRACE ("-size: %" PRIuSIZE "\n", dat->byte)
            }
            break;
          case SECTION_APPINFO:
            {
              Dwg_AppInfo *_obj = &dwg->appinfo;
              bit_chain_alloc (&sec_dat[type]);
              str_dat = hdl_dat = dat = &sec_dat[type];
              bit_chain_set_version (dat, old_dat);
              if (_obj->size && _obj->unknown_bits)
                bit_write_TF (dat, _obj->unknown_bits, _obj->size);
              else
                {
#include "appinfo.spec"
                }
              LOG_TRACE ("-size: %" PRIuSIZE "\n", dat->byte)
            }
            break;
          case SECTION_APPINFOHISTORY:
            {
              Dwg_AppInfoHistory *_obj = &dwg->appinfohistory;
              if (!_obj->size || !_obj->unknown_bits)
                break;
              bit_chain_alloc (&sec_dat[type]);
              str_dat = hdl_dat = dat = &sec_dat[type];
              bit_chain_set_version (dat, old_dat);
              bit_write_TF (dat, _obj->unknown_bits, _obj->size);
              LOG_TRACE ("-size: %" PRIuSIZE "\n", dat->byte)
            }
            break;
          case SECTION_FILEDEPLIST:
            {
              Dwg_FileDepList *_obj = &dwg->filedeplist;
              if (filedeplist_is_empty (_obj))
                break;
              bit_chain_alloc (&sec_dat[type]);
              str_dat = hdl_dat = dat = &sec_dat[type];
              bit_chain_set_version (dat, old_dat);
#include "filedeplist.spec"
              LOG_TRACE ("-size: %" PRIuSIZE "\n", dat->byte)
            }
            break;
          case SECTION_SECURITY:
            {
              Dwg_Security *_obj = &dwg->security;
              if (security_is_empty (_obj))
                break;
              bit_chain_alloc (&sec_dat[type]);
              str_dat = hdl_dat = dat = &sec_dat[type];
              bit_chain_set_version (dat, old_dat);
#include "security.spec"
              LOG_TRACE ("-size: %" PRIuSIZE "\n", dat->byte)
            }
            break;
          case SECTION_SIGNATURE:
            {
#if 0
              Dwg_Signature *_obj = &dwg->signature;
              bit_chain_alloc (&sec_dat[type]);
              str_dat = hdl_dat = dat = &sec_dat[type];
              bit_chain_set_version (dat, old_dat);
              {
#  include "signature.spec"
              }
              LOG_TRACE ("-size: %" PRIuSIZE "\n", dat->byte)
#endif
            }
            break;
          case SECTION_ACDS:
            {
#if 0
              Dwg_AcDs *_obj = &dwg->acds;
              bit_chain_alloc (&sec_dat[type]);
              str_dat = hdl_dat = dat = &sec_dat[type];
              bit_chain_set_version (dat, old_dat);
              {
#  include "acds.spec"
              }
              LOG_TRACE ("-size: %" PRIuSIZE "\n", dat->byte)
#endif
            }
            break;
          case SECTION_VBAPROJECT: // nyi
          default:
            break;
          }
      }
    // and write system and data section maps.
    dat = old_dat;

    /*-------------------------------------------------------------------------
     * Section map and info
     */
    // no gaps, so header->numsections == r2004_header->numsections
    // get together all the section sizes, and set the addresses
    {
      int ssize;
      int si, info_id;
      unsigned address;

      const Dwg_Section_Type section_map_order[] = {
        // R2004_Header
        SECTION_UNKNOWN, // the empty section 128-256
        SECTION_SECURITY,       SECTION_FILEDEPLIST, SECTION_ACDS,
        SECTION_VBAPROJECT,
        SECTION_APPINFOHISTORY, //? at least before AppInfo
        SECTION_APPINFO,        SECTION_PREVIEW,
        SECTION_SUMMARYINFO, // sometimes this is before Preview
        SECTION_REVHISTORY,     SECTION_OBJECTS,     SECTION_OBJFREESPACE,
        SECTION_TEMPLATE,       SECTION_HANDLES,     SECTION_CLASSES,
        SECTION_AUXHEADER,      SECTION_HEADER,      SECTION_SIGNATURE,

        SECTION_INFO,           SECTION_SYSTEM_MAP
      };

      // not the order in the system map, but the order in the dat stream.
      // clang-format off
      const Dwg_Section_Type stream_order[] = {
        // R2004_Header
        SECTION_UNKNOWN, // the empty section 128-256
        SECTION_SUMMARYINFO, SECTION_PREVIEW,        SECTION_VBAPROJECT,
        SECTION_APPINFO,     SECTION_APPINFOHISTORY, SECTION_FILEDEPLIST,
        SECTION_ACDS,        SECTION_REVHISTORY,     SECTION_SECURITY,
        SECTION_OBJECTS,     SECTION_OBJFREESPACE,   SECTION_TEMPLATE,
        SECTION_HANDLES,     SECTION_CLASSES,        SECTION_AUXHEADER,
        SECTION_HEADER,      SECTION_SIGNATURE,

        SECTION_INFO,        SECTION_SYSTEM_MAP
      };
      // clang-format on

      dwg->fhdr.r2004_header.numsections = 0;
      dwg->fhdr.r2004_header.numgaps = 0;

      // sec_dat[SECTION_UNKNOWN].byte = 0;
      sec_dat[SECTION_INFO].byte = 10
                                   + (dwg->header.section_infohdr.num_desc
                                      * sizeof (Dwg_Section_Info));
      // only a guess, reserve at least one page
      sec_dat[SECTION_SYSTEM_MAP].byte = (4 * 20 * sizeof (Dwg_Section));

      section_address = 0x100;
      // first all the data pages, than a number gap of 1, and last the two
      // system page maps, info and system_map the data_pages (system_map
      // sections) can include multiple pages of the same type.
      LOG_TRACE ("\n=== Section map and info page sizes ===\n");
      for (si = 0, info_id = 0, i = 0; i < ARRAY_SIZE (stream_order); i++)
        {
          type = stream_order[i];
          if (sec_dat[type].byte)
            {
              const unsigned int max_decomp_size
                  = section_max_decomp_size (dwg, (Dwg_Section_Type)type);
              const char *name = dwg_section_name (dwg, type);
              Dwg_Section_Info *info;
              if (sec_dat[type].bit)
                {
                  LOG_WARN ("Unpadded section %d", type);
                  sec_dat[type].byte++;
                }
              ssize = (int)sec_dat[type].byte;
              sec_dat[type].size = ssize;
              if (info_id >= (int)dwg->header.section_infohdr.num_desc)
                {
                  dwg->header.section_infohdr.num_desc = info_id + 1;
                  dwg->header.section_info = (Dwg_Section_Info *)REALLOC (
                      dwg->header.section_info,
                      (info_id + 1) * sizeof (Dwg_Section_Info));
                }
              info = &dwg->header.section_info[info_id];
              info->fixedtype = (Dwg_Section_Type)type;
              info->type = type;
              info->unknown = 1;
              if (name && type < SECTION_INFO) // not UNKNOWN and the last two
                strcpy (info->name, name);
              else
                memset (info->name, 0, 64);
              info->size = ssize;
              info->max_decomp_size = max_decomp_size;
              info->encrypted
                  = section_encrypted (dwg, (Dwg_Section_Type)type);
              // always uncompressed for now, LZ compressor not yet
              // ODA-compatible. TODO: re-enable when fixed.
              info->compressed = 1;
              // pre-calc numsections for both
              if ((unsigned)ssize <= max_decomp_size)
                info->num_sections = 1;
              else
                {
                  info->num_sections = (unsigned)ssize / max_decomp_size;
                  if ((unsigned)ssize % max_decomp_size)
                    info->num_sections++;
                }
              info->sections = (Dwg_Section **)CALLOC (info->num_sections,
                                                       sizeof (Dwg_Section *));
              // enough sections?
              if (si + info->num_sections > dwg->header.num_sections)
                {
                  Dwg_Section *oldsecs = dwg->header.section;
                  dwg->header.num_sections = si + info->num_sections;
                  dwg->header.section = (Dwg_Section *)REALLOC (
                      dwg->header.section,
                      dwg->header.num_sections * sizeof (Dwg_Section));
                  if (dwg->header.section != oldsecs)
                    // need to rebuild all info->sections
                    section_info_rebuild (dwg, (Dwg_Section_Type)type);
                }
              {
                int ssi = 0;
                // For single-page sections store actual content size, not
                // the theoretical max_decomp_size
                if (info->num_sections == 1)
                  info->max_decomp_size
                      = MIN (max_decomp_size, (unsigned)ssize);
                while (ssize > 0)
                  {
                    // actual content for this page (last page may be partial)
                    unsigned page_content = MIN (
                        max_decomp_size, (unsigned)(ssize > 0 ? ssize : 0));
                    Dwg_Section *sec = &dwg->header.section[si];
                    total_size += ssize;
                    sec->number = si + 1; // index starting at 1
                    if (type < SECTION_INFO)
                      {
                        // data section: on-disk = 32-byte encrypted page
                        // header + actual content (no padding to max_decomp)
                        sec->size = 32 + page_content;
                        sec->decomp_data_size = page_content;
                      }
                    else
                      {
                        // system section: on-disk = 20-byte header + content
                        // size will be updated during stream write
                        sec->size = 20 + page_content;
                        sec->decomp_data_size = page_content;
                      }
                    sec->type = (Dwg_Section_Type)type;
                    sec->compression_type = info->compressed;
                    info->sections[ssi] = sec;
                    LOG_TRACE ("section[%d] %s[%d].sections[%d]: number=%d "
                               "size=%d\n",
                               si, dwg_section_name (dwg, type), info_id, ssi,
                               sec->number, (int)sec->size);
                    ssize -= (int)page_content;
                    ssi++; // info->sections index
                    si++;  // section index
                  }
              }
              info_id++;
            }
          else
            LOG_TRACE ("section_info %s is empty, skipped. size=0\n",
                       dwg_section_name (dwg, type));
        }
      // r2004_header.numsections counts all real section-map entries.
      // section_info_id and section_map_id then live just above that range.
      dwg->fhdr.r2004_header.numsections = si;
      // fix num_desc to actual count of written descriptors
      dwg->header.section_infohdr.num_desc = info_id;
      // section_info and section_map are the two last already added.
      if ((unsigned)si > dwg->header.num_sections) // needed?
        {
          Dwg_Section *oldsecs = dwg->header.section;
          dwg->header.num_sections = si;
          dwg->header.section = (Dwg_Section *)REALLOC (
              dwg->header.section, si * sizeof (Dwg_Section));
          if (dwg->header.section != oldsecs)
            section_info_rebuild (dwg, SECTION_SYSTEM_MAP);
        }
      dwg->fhdr.r2004_header.section_info_id
          = dwg->fhdr.r2004_header.numsections + 1;
      dwg->fhdr.r2004_header.section_map_id
          = dwg->fhdr.r2004_header.numsections + 2;
      dwg->fhdr.r2004_header.section_array_size
          = dwg->fhdr.r2004_header.numsections + 2;
      dwg->fhdr.r2004_header.last_section_id
          = dwg->fhdr.r2004_header.section_map_id;
      dwg->header.section[si - 2].number
          = dwg->fhdr.r2004_header.section_info_id;
      dwg->header.section[si - 1].number
          = dwg->fhdr.r2004_header.section_map_id;

      LOG_TRACE ("\n=== Section Info %d in map order ===\n",
                 dwg->fhdr.r2004_header.section_info_id);
      // write into sec_dat[type] first, then compress
      sec_id = SECTION_INFO;
      sec_dat[sec_id].size = sec_dat[sec_id].byte;
      bit_chain_alloc (&sec_dat[sec_id]);
      dat = &sec_dat[sec_id];
      bit_chain_set_version (dat, old_dat);
      bit_set_position (dat, 0); // so far we faked the content. now write it

      {
        Dwg_Section_InfoHdr *_obj = &dwg->header.section_infohdr;
        Dwg_Section *sec = &dwg->header.section[si - 2];
        Dwg_Section_Info *info
            = find_section_info_type (dwg, (Dwg_Section_Type)sec_id);
        // index starting at 1
        sec->number = dwg->fhdr.r2004_header.section_info_id;
        sec->size = MIN (0x7400, sec->size);
        sec->decomp_data_size = sec->size;
        sec->type = (Dwg_Section_Type)type;
        if (info)
          {
            sec->compression_type = info->compressed;
            // very unlikely, more than 1 page
            info->sections[0] = sec;
          }
        _obj->compressed = 2;
        _obj->max_size = 0x7400;
        _obj->encrypted = 0;
        _obj->num_desc2 = _obj->num_desc;
        LOG_HANDLE ("InfoHdr @%" PRIuSIZE ".0\n", dat->byte);
        FIELD_RL (num_desc, 0);
        FIELD_RL (compressed, 0);
        FIELD_RL (max_size, 0);
        FIELD_RL (encrypted, 0);
        FIELD_RL (num_desc2, 0);
      }
      for (i = 0; i < ARRAY_SIZE (section_map_order); i++)
        {
          Dwg_Section_Info *_obj;
          type = section_map_order[i];
          _obj = find_section_info_type (dwg, (Dwg_Section_Type)type);
          if (_obj)
            {
              assert (type == _obj->fixedtype);
              LOG_TRACE ("\nSection_Info %s [%d]\n",
                         dwg_section_name (dwg, type), i);
              FIELD_RLLu (size, 0);
              FIELD_RL (num_sections, 0);
              FIELD_RL (max_decomp_size, 0);
              FIELD_RL (unknown, 0);
              FIELD_RL (compressed, 0);
              FIELD_RL (type, 0);
              FIELD_RL (encrypted, 0);
              bit_write_TF (dat, (unsigned char *)_obj->name, 64);
              LOG_TRACE ("name: %s\n", *_obj->name ? _obj->name : "");
              // write page entries: number RL, size RL, address RLL
              for (j = 0; j < _obj->num_sections; j++)
                {
                  Dwg_Section *sec = _obj->sections[j];
                  if (sec)
                    {
                      uint64_t page_offset
                          = (uint64_t)j * _obj->max_decomp_size;
                      bit_write_RL (dat, sec->number);
                      bit_write_RL (dat, sec->decomp_data_size);
                      bit_write_RLL (dat, page_offset);
                      LOG_TRACE (
                          "  Page: %d size: %u address: 0x%" PRIx64 "\n",
                          sec->number, sec->decomp_data_size, page_offset);
                    }
                }
            }
        }

      LOG_TRACE ("\n=== Section System Map %d in map order ===\n",
                 dwg->fhdr.r2004_header.section_map_id);
      sec_id = SECTION_SYSTEM_MAP;
      type = SECTION_SYSTEM_MAP;
      {
        // Dwg_Section_InfoHdr *_obj = &dwg->header.section_infohdr;
        Dwg_Section *sec = &dwg->header.section[si - 1];
        Dwg_Section_Info *info
            = find_section_info_type (dwg, (Dwg_Section_Type)type);
        if (!info || !info->sections)
          {
            LOG_ERROR ("SECTION_SYSTEM_MAP not found");
            return DWG_ERR_SECTIONNOTFOUND;
          }

        sec_dat[sec_id].size = sec_dat[sec_id].byte;
        bit_chain_alloc (&sec_dat[sec_id]);
        str_dat = hdl_dat = dat = &sec_dat[sec_id];
        bit_chain_set_version (dat, old_dat);
        bit_set_position (dat, 0); // so far we faked the content. now write it

        // index starting at 1
        sec->number = dwg->fhdr.r2004_header.section_map_id;
        sec->size = MIN (0x7400, sec->size);
        sec->decomp_data_size = sec->size;
        sec->type = (Dwg_Section_Type)type;
        sec->compression_type = info->compressed;
        // very unlikely, more than 1 page
        info->sections[0] = sec;
      }

      // preliminary section map (in enum order): will be rebuilt in stream
      // order after data sections are written, with correct addresses. Section
      // map format: (number RL, size RL) per entry = 8 bytes. Address is NOT
      // stored; the decoder computes it from accumulated sizes.
      for (i = 0; i < dwg->header.num_sections; i++)
        {
          Dwg_Section *_obj = &dwg->header.section[i];
          FIELD_RL (number, 0);
          FIELD_RL (size, 0);
          if (_obj->number < 0) // gap (unused, we deleted all gaps)
            {
              FIELD_RL (parent, 0);
              FIELD_RL (left, 0);
              FIELD_RL (right, 0);
              FIELD_RL (x00, 0);
            }
        }
      dwg->fhdr.r2004_header.decomp_data_size
          = dat->byte & 0xFFFFFFFF; // system_map_size (preliminary)
      LOG_TRACE ("-size: %" PRIuSIZE "\n", dat->byte);

      dat = old_dat;
#ifndef NDEBUG
      if (dwg->header.version >= R_1_2)
        {
          if (dat->size < 4 || dat->chain[0] != 'A' || dat->chain[1] != 'C')
            {
              LOG_ERROR ("Encode overwrite pos 0");
              return DWG_ERR_INVALIDDWG;
            }
          assert (dat->chain[0] == 'A');
          assert (dat->chain[1] == 'C');
          PRE (R_2004a)
          {
            assert (dat->byte <= 0x100);
          }
          LATER_VERSIONS
          {
            assert (dat->byte <= 0x140);
          }
        }
#endif

      // now write all the sections in the stream order
      LOG_TRACE ("\n=== Write sections in stream order ===\n");
      size
          = total_size
            + (8 * ((dwg->fhdr.r2004_header.numsections + 2) * 24)); // no gaps
      assert (section_address);
      dat->byte = section_address;
      if (dat->byte + size > dat->size)
        bit_chain_alloc_size (dat, size);
      LOG_HANDLE ("@%" PRIuSIZE ".0\n", dat->byte);
      for (i = 0; i < ARRAY_SIZE (stream_order); i++)
        {
          Dwg_Section_Info *info;
          unsigned max_decomp_size;
          type = stream_order[i];
          info = find_section_info_type (dwg, (Dwg_Section_Type)type);
          if (!info)
            continue;
          max_decomp_size
              = section_max_decomp_size (dwg, (Dwg_Section_Type)type);

          // Before SECTION_SYSTEM_MAP: rebuild the section map in stream order
          // with correct addresses (accumulating from actual stream
          // positions).
          if (type == SECTION_SYSTEM_MAP)
            {
              Bit_Chain *smap = &sec_dat[SECTION_SYSTEM_MAP];
              uint32_t smap_data_size;
              // SECTION_INFO sec->size was already set correctly (20 +
              // comp_data_size) when it was written in the previous iteration.
              // rebuild section map content (number, size) in stream order
              bit_set_position (smap, 0);
              for (int si2 = 0; si2 < (int)ARRAY_SIZE (stream_order); si2++)
                {
                  Dwg_Section_Info *info2;
                  int type2 = stream_order[si2];
                  if (type2 == SECTION_INFO || type2 == SECTION_SYSTEM_MAP)
                    break; // stop before INFO and SYSTEM_MAP (written
                           // explicitly)
                  info2
                      = find_section_info_type (dwg, (Dwg_Section_Type)type2);
                  if (!info2)
                    continue;
                  for (unsigned k2 = 0; k2 < info2->num_sections; k2++)
                    {
                      Dwg_Section *sec2 = info2->sections[k2];
                      if (!sec2)
                        continue;
                      bit_write_RL (smap, sec2->number);
                      bit_write_RL (smap, sec2->size);
                    }
                }
              // write SECTION_INFO entry
              {
                Dwg_Section *sec_inf = &dwg->header.section[si - 2];
                bit_write_RL (smap, sec_inf->number);
                bit_write_RL (smap, sec_inf->size);
              }
              // compute SECTION_SYSTEM_MAP disk size and write its entry
              smap_data_size = (uint32_t)smap->byte + 8; // +8 for this entry
              {
                Dwg_Section *sec_smap = &dwg->header.section[si - 1];
                sec_smap->size
                    = 20 + stored_R2004_section_size (smap_data_size);
                bit_write_RL (smap, sec_smap->number);
                bit_write_RL (smap, sec_smap->size);
              }
              // update sizes in r2004_header
              dwg->fhdr.r2004_header.decomp_data_size = smap_data_size;
            }

          LOG_TRACE ("Write %s pages @%" PRIuSIZE " (%u/%" PRIuSIZE ")\n",
                     dwg_section_name (dwg, type), dat->byte,
                     info->num_sections, sec_dat[type].size);
          for (unsigned k = 0; k < info->num_sections; k++)
            {
              Dwg_Section *sec = info->sections[k];
              uint32_t content_size;
              if (!sec)
                {
                  LOG_ERROR ("empty info->sections[%u]", k);
                  continue;
                }
              if (!sec_dat[type].chain)
                {
                  LOG_ERROR ("empty %s.chain", dwg_section_name (dwg, type));
                  continue;
                }
#ifndef NDEBUG
              if (info->fixedtype < SECTION_INFO)
                assert (info->fixedtype == sec->type);
#endif
              sec->address = dat->byte; // actual stream position

              if (info->fixedtype == SECTION_SUMMARYINFO)
                dwg->header.summaryinfo_address = (uint32_t)sec->address;
              else if (info->fixedtype == SECTION_PREVIEW)
                dwg->header.thumbnail_address
                    = (uint32_t)sec->address + 32; // after page header
              else if (info->fixedtype == SECTION_VBAPROJECT)
                dwg->header.vbaproj_address = (uint32_t)sec->address;

              if (type >= SECTION_INFO)
                {
                  // System section: 20-byte header + LZ-stored content
                  // section_type: 0x41630e3b (SYSTEM_MAP) or 0x4163003b (INFO)
                  uint32_t section_magic
                      = (type == SECTION_SYSTEM_MAP) ? 0x41630e3b : 0x4163003b;
                  size_t checksum_pos, comp_data_pos;
                  content_size = (uint32_t)sec_dat[type].byte;
                  sec->decomp_data_size = content_size;

                  if (dat->byte + 20 + content_size + 20 > dat->size)
                    bit_chain_alloc_size (dat, 20 + content_size + 20);

                  bit_write_RL (dat, section_magic);
                  bit_write_RL (dat, content_size); // decomp_data_size
                  comp_data_pos = dat->byte;
                  bit_write_RL (dat, 0); // comp_data_size placeholder
                  bit_write_RL (dat, 2); // compression_type = 2 (LZ)
                  checksum_pos = dat->byte;
                  bit_write_RL (dat, 0); // checksum placeholder

                  // write section content as compressed stream.
                  // For SYSTEM_MAP use store (not compress) so the size
                  // matches stored_R2004_section_size used in the self-entry.
                  if (type == SECTION_SYSTEM_MAP)
                    store_R2004_section (dat, sec_dat[type].chain,
                                         content_size, &sec->comp_data_size);
                  else
                    compress_R2004_section (dat, sec_dat[type].chain,
                                            content_size,
                                            &sec->comp_data_size);
                  sec->size = 20 + sec->comp_data_size;

                  // patch comp_data_size in the header
                  dat->chain[comp_data_pos] = sec->comp_data_size & 0xFF;
                  dat->chain[comp_data_pos + 1]
                      = (sec->comp_data_size >> 8) & 0xFF;
                  dat->chain[comp_data_pos + 2]
                      = (sec->comp_data_size >> 16) & 0xFF;
                  dat->chain[comp_data_pos + 3]
                      = (sec->comp_data_size >> 24) & 0xFF;

                  if (type == SECTION_SYSTEM_MAP)
                    {
                      dwg->fhdr.r2004_header.section_map_address
                          = (BITCODE_RLL)sec->address - 0x100;
                      dwg->fhdr.r2004_header.last_section_address
                          = (BITCODE_RLL)sec->address + sec->size - 0x100;
                      // secondheader_address is set after all sections
                      dwg->fhdr.r2004_header.comp_data_size
                          = sec->comp_data_size;
                    }

                  // compute and patch checksum
                  {
                    uint32_t cs1, cs2;
                    Bit_Chain cs_dat = *dat;
                    cs_dat.byte = sec->address;
                    cs1 = dwg_section_page_checksum (0, &cs_dat, 20, true);
                    cs_dat.byte = sec->address + 20;
                    cs2 = dwg_section_page_checksum (
                        cs1, &cs_dat, (int32_t)sec->comp_data_size, false);
                    dat->chain[checksum_pos] = cs2 & 0xFF;
                    dat->chain[checksum_pos + 1] = (cs2 >> 8) & 0xFF;
                    dat->chain[checksum_pos + 2] = (cs2 >> 16) & 0xFF;
                    dat->chain[checksum_pos + 3] = (cs2 >> 24) & 0xFF;
                    LOG_TRACE ("%s checksum: 0x%08x\n",
                               dwg_section_name (dwg, type), cs2);
                  }
                }
              else
                {
                  // Data section: 32-byte encrypted page header + data padded
                  // to max_decomp_size.
                  BITCODE_RC *chain_page;
                  uint32_t page_hdr[8];
                  uint32_t sec_mask;
                  size_t page_hdr_pos;

                  content_size = sec->decomp_data_size; // = max_decomp_size
                  chain_page
                      = sec_dat[type].chain + (size_t)k * max_decomp_size;
                  if (dat->byte + 32 + content_size + 32 > dat->size)
                    bit_chain_alloc_size (dat, 32 + content_size + 32);

                  // reserve encrypted page header, write payload, then patch
                  // header with actual compressed data size.
                  page_hdr_pos = dat->byte;
                  dat->byte += 32;

                  // always use raw copy for data section pages,
                  // as the LZ compressor output is not yet ODA-compatible
                  copy_R2004_section (dat, chain_page, content_size,
                                      &sec->comp_data_size);

                  sec->size = 32 + sec->comp_data_size;

                  // build unencrypted page header
                  page_hdr[0] = 0x4163043b;          // page_type
                  page_hdr[1] = info->type;          // section_type
                  page_hdr[2] = sec->comp_data_size; // compressed bytes
                  page_hdr[3] = content_size;        // decompressed bytes
                  page_hdr[4] = k * max_decomp_size; // start offset in decomp
                  page_hdr[5] = 0;                   // unknown
                  page_hdr[6] = 0;                   // page_header_crc
                  page_hdr[7] = 0;                   // data_crc

                  {
                    Bit_Chain page_dat = *dat;
                    Bit_Chain hdr_dat = { 0 };
                    uint32_t data_crc, page_hdr_crc;

                    page_dat.byte = page_hdr_pos + 32;
                    data_crc = dwg_section_page_checksum (
                        0, &page_dat, (int32_t)sec->comp_data_size, false);

                    // Build clear-text header checksums before applying mask.
                    page_hdr[6] = 0;        // page_header_crc placeholder
                    page_hdr[7] = data_crc; // data_crc
                    hdr_dat.chain = (unsigned char *)page_hdr;
                    hdr_dat.size = 32;
                    hdr_dat.byte = 0;
                    page_hdr_crc = dwg_section_page_checksum (
                        data_crc, &hdr_dat, 32, false);
                    page_hdr[6] = page_hdr_crc;

                    // encrypt: XOR with sec_mask
                    sec_mask = 0x4164536b ^ (uint32_t)sec->address;
                    for (int n = 0; n < 8; n++)
                      page_hdr[n] = htole32 (le32toh (htole32 (page_hdr[n]))
                                             ^ sec_mask);
                    memcpy (&dat->chain[page_hdr_pos], page_hdr, 32);
                  }

                  LOG_HANDLE ("Write page %s[%u] @" FORMAT_RLL
                              " size=%u mask=0x%x\n",
                              dwg_section_name (dwg, type), k, sec->address,
                              content_size, sec_mask);
                }
            }
          bit_chain_free (&sec_dat[type]);
        }
    }

    // Patchup thumbnail_address at 0x0D for R2004+
    {
      size_t oldpos = dat->byte;
      dat->byte = 0x0D;
      bit_write_RL (dat, dwg->header.thumbnail_address);
      LOG_TRACE ("header.thumbnail_address => " FORMAT_RL " [RL] @0x0d\n",
                 dwg->header.thumbnail_address);
      dat->byte = oldpos;
    }

    // Reserve space for secondheader (108 bytes, copy of encrypted
    // R2004_Header without padding)
    {
      size_t secondheader_pos = dat->byte;
      dwg->fhdr.r2004_header.secondheader_address
          = (BITCODE_RLL)secondheader_pos - 0x100;
      if (dat->byte + 108 > dat->size)
        bit_chain_alloc_size (dat, 108);
      dat->byte += 108;
      LOG_TRACE ("secondheader reserved @0x%" PRIX64 "\n",
                 (uint64_t)secondheader_pos);
    }

    {
      Dwg_R2004_Header *_obj = &dwg->fhdr.r2004_header;
      Bit_Chain file_dat = {
        NULL, sizeof (Dwg_R2004_Header), 0UL, 0, 0, R_INVALID, R_INVALID, NULL,
        30
      };
      BITCODE_RC overlap_hdr[sizeof (Dwg_R2004_Header)];
      Bit_Chain *orig_dat = dat;
      /* "AcFssFcAJMB" encrypted: 6840F8F7922AB5EF18DD0BF1 */
      const char enc_file_ID_string[]
          = "\x68\x40\xF8\xF7\x92\x2A\xB5\xEF\x18\xDD\x0B\xF1";
      uint32_t checksum;

      file_dat.chain = (unsigned char *)CALLOC (1, sizeof (Dwg_R2004_Header));
      dat = &file_dat;
      LOG_TRACE ("\nSection R2004_Header @0x100\n");
      memcpy (_obj->file_ID_string, "AcFssFcAJMB", 12);
      // The encrypted file header overlaps the first 12 bytes of the first
      // data page at 0x100. Preserve those ciphertext bytes so the final
      // header encryption does not clobber the SummaryInfo page header.
      decrypt_R2004_header (overlap_hdr, &orig_dat->chain[0x80],
                            sizeof (Dwg_R2004_Header));

      checksum = _obj->crc32;
      LOG_HANDLE ("old crc32: 0x%x\n", _obj->crc32);
      _obj->crc32 = 0;
      // recalc the CRC32, without the padding, but the crc32 as 0
      _obj->crc32
          = bit_calc_CRC32 (0, (unsigned char *)&dwg->fhdr.r2004_header, 0x6c);
      LOG_HANDLE ("calc crc32: 0x%x\n", _obj->crc32);

      // clang-format off
      #include "r2004_file_header.spec"
      // clang-format on
      // The encrypted file header overlaps bytes 0x100..0x10b (12 bytes) of
      // the first data page. Preserve exactly those overlapped plaintext
      // fields (comp_data_size, compression_type, checksum) from the
      // pre-existing ciphertext so we don't clobber the first page header.
      memcpy (&file_dat.chain[offsetof (Dwg_R2004_Header, comp_data_size)],
              &overlap_hdr[offsetof (Dwg_R2004_Header, comp_data_size)],
              sizeof (_obj->comp_data_size) + sizeof (_obj->compression_type)
                  + sizeof (_obj->checksum));

      // go back and encrypt it
      dat = orig_dat;
      decrypt_R2004_header (&dat->chain[0x80], file_dat.chain,
                            sizeof (Dwg_R2004_Header));
      bit_chain_free (&file_dat);
      LOG_HANDLE ("encrypted R2004_Header:\n");
      LOG_TF (HANDLE, &dat->chain[0x80], (int)sizeof (Dwg_R2004_Header));
      if (memcmp (&dat->chain[0x80], enc_file_ID_string,
                  sizeof (enc_file_ID_string) - 1))
        {
          LOG_ERROR ("r2004_file_header encryption error");
          return error | DWG_ERR_INVALIDDWG;
        }
      // Write secondheader: copy of encrypted R2004_Header (108 bytes,
      // without the 12-byte padding)
      {
        size_t secondheader_pos = (size_t)_obj->secondheader_address + 0x100;
        memcpy (&dat->chain[secondheader_pos], &dat->chain[0x80], 108);
        LOG_TRACE ("secondheader written @0x%" PRIX64 "\n",
                   (uint64_t)secondheader_pos);
      }
    } // R2004_Header
  } // R_2004

  assert (!dat->bit);
  dat->size = dat->byte;
  LOG_INFO ("\nFinal DWG size: %u\n", (unsigned)dat->size);

  UNTIL (R_2002)
  {
    /* Patch section addresses */
    assert (section_address);
    dat->byte = section_address;
    dat->bit = 0;
    LOG_INFO ("\n=======> section addresses: %4zu\n", dat->byte);
    for (j = 0; j < dwg->header.sections; j++)
      {
        LOG_TRACE ("section[%u].number: %4d [RC] %s\n", j,
                   (int)dwg->header.section[j].number,
                   j < 6 ? dwg_section_name (dwg, j) : "");
        LOG_TRACE ("section[%u].offset: %4lu [RL]\n", j,
                   (unsigned long)dwg->header.section[j].address);
        LOG_TRACE ("section[%u].size:   %4lu [RL]\n", j,
                   (unsigned long)dwg->header.section[j].size);
        if ((size_t)dwg->header.section[j].address
                + dwg->header.section[j].size
            > dat->size)
          {
            if (is_section_r13_critical ((Dwg_Section_Type_r13)j))
              {
                LOG_ERROR ("section[%u] %s address or size overflow", j,
                           j < 6 ? dwg_section_name (dwg, j) : "");
                return DWG_ERR_INVALIDDWG;
              }
            else
              {
                LOG_WARN ("section[%u] %s address or size overflow, skipped",
                          j, j < 6 ? dwg_section_name (dwg, j) : "");
                dwg->header.section[j].address = 0;
                dwg->header.section[j].size = 0;
              }
          }
        bit_write_RC (dat, dwg->header.section[j].number);
        bit_write_RL (dat, dwg->header.section[j].address);
        bit_write_RL (dat, dwg->header.section[j].size);
      }

    // assert (dat->byte == header_crc_address);
    bit_write_CRC (dat, 0, 0xC0C1);
    dat->byte = dat->size;
  }

  return 0;
}
// clang-format off
AFL_GCC_POP

/* utf-8 string without lowercase letters, space or ! */
static BITCODE_T
fixup_invalid_tag (const Bit_Chain *restrict dat, char *restrict tag)
{
  size_t len;
  int changed = 0;
  BITCODE_TV newtag;
  BITCODE_TU wstr;
  if (!tag)
    return NULL;
  if (IS_FROM_TU (dat))
    wstr = (BITCODE_TU)tag;
  else
    wstr = bit_utf8_to_TU ((char*)tag, 0);
  len = bit_wcs2len (wstr);
  for (size_t i = 0; i < len; i++)
    {
      if (i > 256)
        {
          wstr[i] = 0;
          changed++;
          break;
        }
#ifdef HAVE_WCTYPE_H
      if (iswlower (wstr[i]))
        {
          wstr[i] = towupper (wstr[i]);
          changed++;
        }
#endif
      if (wstr[i] == L' ' || wstr[i] == L'!')
        {
          wstr[i] = L'_';
          changed++;
        }
    }
  if (changed && dat->version < R_2007)
    {
      newtag = bit_convert_TU (wstr);
      FREE (wstr);
      return newtag;
    }
  else
    return (BITCODE_T)tag;
}

#include "dwg.spec"
// clang-format on

/* Forward declarations for functions generated from dwg2.spec in encode2.c.
   Uses objects.inc which also covers first-half (static) functions, so
   suppress the redundant-decls warning for those. */
#undef DWG_ENTITY
#undef DWG_ENTITY_END
#undef DWG_OBJECT
#undef DWG_OBJECT_END
GCC46_DIAG_IGNORE (-Wredundant-decls)
#define DWG_ENTITY(token)                                                     \
  extern int dwg_encode_##token (Bit_Chain *restrict dat,                     \
                                 Dwg_Object *restrict obj);
#define DWG_ENTITY_END
#define DWG_OBJECT(token)                                                     \
  extern int dwg_encode_##token (Bit_Chain *restrict dat,                     \
                                 Dwg_Object *restrict obj);
#define DWG_OBJECT_END
#include "objects.inc"
GCC46_DIAG_RESTORE

static int
encode_preR13_section (const Dwg_Section_Type_r11 id, Bit_Chain *restrict dat,
                       Dwg_Data *restrict dwg)
{
  Dwg_Section *tbl = &dwg->header.section[id];
  int i = 0;
  int error = 0;
  BITCODE_BL vcount;
  int tblnum = tbl->number;
  BITCODE_RL num = tbl->objid_r11; // from decode_r11
  Bit_Chain *hdl_dat = dat;
  Dwg_Object *ctrl;
  assert (id <= SECTION_VX);

#define PREP_CTRL(token)                                                      \
  if (!num) /* don't trust tbl. from json or dxf */                           \
    {                                                                         \
      Dwg_Object_##token *_ctrl;                                              \
      Dwg_Object *ref = NULL;                                                 \
      ctrl = dwg_get_first_object (dwg, DWG_TYPE_##token);                    \
      if (!ctrl)                                                              \
        {                                                                     \
          LOG_ERROR ("No " #token " found");                                  \
          return DWG_ERR_INVALIDTYPE;                                         \
        }                                                                     \
      _ctrl = ctrl->tio.object->tio.token;                                    \
      tblnum = _ctrl->num_entries;                                            \
      if (tblnum)                                                             \
        ref = dwg_ref_object (dwg, _ctrl->entries[0]);                        \
      num = ref ? ref->index : 0;                                             \
    }                                                                         \
  LOG_TRACE ("\nctrl " #token " [%d]: num:%u\n", num, tblnum)
  if (num + tblnum >= dwg->num_objects)
    {
      LOG_ERROR ("Invalid num %u + tblnum %u", (unsigned)num,
                 (unsigned)tblnum);
      return DWG_ERR_INVALIDTYPE;
    }

#define PREP_TABLE(token)                                                     \
  size_t size_adr = dat->byte;                                                \
  Dwg_Object *obj = dwg_get_next_object (dwg, DWG_TYPE_##token, num + i);     \
  Dwg_Object_##token *_obj;                                                   \
  if (!obj)                                                                   \
    {                                                                         \
      LOG_ERROR ("No table %s after pos %u found", #token, num + i);          \
      if (num + i > dwg->num_objects)                                         \
        num = 0;                                                              \
      continue;                                                               \
    }                                                                         \
  _obj = obj->tio.object->tio.token;                                          \
  LOG_TRACE ("contents table " #token " [%d]: (0x%zx, 0x%zx)\n", i,           \
             obj->address, dat->byte);                                        \
  if (obj->fixedtype != DWG_TYPE_##token)                                     \
    {                                                                         \
      LOG_ERROR ("Wrong type %s at [%d], expected %s",                        \
                 dwg_type_name (obj->fixedtype), num + i,                     \
                 "DWG_TYPE_" #token);                                         \
      continue;                                                               \
    }

#define CHK_ENDPOS                                                            \
  dwg->cur_index += tblnum;                                                   \
  SINCE (R_11)                                                                \
  {                                                                           \
    bit_write_CRC (dat, size_adr, 0xC0C1);                                    \
  }

  switch (id)
    {
    case SECTION_BLOCK:
      write_sentinel (dat, DWG_SENTINEL_R11_BLOCK_BEGIN);
      PREP_CTRL (BLOCK_CONTROL)
      for (i = 0; i < tblnum; i++)
        {
          PREP_TABLE (BLOCK_HEADER);
          if (_obj->name && strEQc (_obj->name, "*MODEL_SPACE"))
            {
              LOG_TRACE ("Skip *MODEL_SPACE\n");
              continue;
            }
          error |= dwg_encode_BLOCK_HEADER (dat, obj);
          CHK_ENDPOS;
        }
      write_sentinel (dat, DWG_SENTINEL_R11_BLOCK_END);
      break;

    case SECTION_LAYER:
      write_sentinel (dat, DWG_SENTINEL_R11_LAYER_BEGIN);
      PREP_CTRL (LAYER_CONTROL)
      for (i = 0; i < tblnum; i++)
        {
          PREP_TABLE (LAYER);
          error |= dwg_encode_LAYER (dat, obj);
          CHK_ENDPOS;
        }
      write_sentinel (dat, DWG_SENTINEL_R11_LAYER_END);
      break;

    // was a text STYLE table, became a STYLE object
    case SECTION_STYLE:
      write_sentinel (dat, DWG_SENTINEL_R11_STYLE_BEGIN);
      PREP_CTRL (STYLE_CONTROL)
      for (i = 0; i < tblnum; i++)
        {
          PREP_TABLE (STYLE);
          error |= dwg_encode_STYLE (dat, obj);
          CHK_ENDPOS;
        }
      write_sentinel (dat, DWG_SENTINEL_R11_STYLE_END);
      break;

    case SECTION_LTYPE:
      write_sentinel (dat, DWG_SENTINEL_R11_LTYPE_BEGIN);
      PREP_CTRL (LTYPE_CONTROL)
      for (i = 0; i < tblnum; i++)
        {
          PREP_TABLE (LTYPE);
          error |= dwg_encode_LTYPE (dat, obj);
          CHK_ENDPOS;
        }
      write_sentinel (dat, DWG_SENTINEL_R11_LTYPE_END);
      break;

    case SECTION_VIEW:
      write_sentinel (dat, DWG_SENTINEL_R11_VIEW_BEGIN);
      PREP_CTRL (VIEW_CONTROL)
      for (i = 0; i < tblnum; i++)
        {
          PREP_TABLE (VIEW);
          error |= dwg_encode_VIEW (dat, obj);
          CHK_ENDPOS;
        }
      write_sentinel (dat, DWG_SENTINEL_R11_VIEW_END);
      break;

    case SECTION_UCS:
      write_sentinel (dat, DWG_SENTINEL_R11_UCS_BEGIN);
      PREP_CTRL (UCS_CONTROL)
      for (i = 0; i < tblnum; i++)
        {
          PREP_TABLE (UCS);
          // check ucs_elevation. maybe set flag_r11 & HAS_ELEVATION
          if (FIELD_VALUE (ucs_elevation) != 0.0)
            FIELD_VALUE (ucsorg.z) = FIELD_VALUE (ucs_elevation);
          // if (FIELD_VALUE (ucsorg.z) != 0.0)
          //   {
          //     obj->tio.object->flag_r11 |= FLAG_R11_HAS_ELEVATION;
          //     obj->tio.object->elevation_r11 = FIELD_VALUE (ucsorg.z);
          //   }
          error |= dwg_encode_UCS (dat, obj);
          CHK_ENDPOS;
        }
      write_sentinel (dat, DWG_SENTINEL_R11_UCS_END);
      break;

    case SECTION_VPORT:
      write_sentinel (dat, DWG_SENTINEL_R11_VPORT_BEGIN);
      PREP_CTRL (VPORT_CONTROL)
      for (i = 0; i < tblnum; i++)
        {
          PREP_TABLE (VPORT);
          error |= dwg_encode_VPORT (dat, obj);
          CHK_ENDPOS;
        }
      write_sentinel (dat, DWG_SENTINEL_R11_VPORT_END);
      break;

    case SECTION_APPID:
      write_sentinel (dat, DWG_SENTINEL_R11_APPID_BEGIN);
      PREP_CTRL (APPID_CONTROL)
      for (i = 0; i < tblnum; i++)
        {
          PREP_TABLE (APPID);
          error |= dwg_encode_APPID (dat, obj);
          CHK_ENDPOS;
        }
      write_sentinel (dat, DWG_SENTINEL_R11_APPID_END);
      break;

    case SECTION_DIMSTYLE:
      write_sentinel (dat, DWG_SENTINEL_R11_DIMSTYLE_BEGIN);
      PREP_CTRL (DIMSTYLE_CONTROL)
      for (i = 0; i < tblnum; i++)
        {
          PREP_TABLE (DIMSTYLE); // d1f
          error |= dwg_encode_DIMSTYLE (dat, obj);
          CHK_ENDPOS; //-e63
        }
      write_sentinel (dat, DWG_SENTINEL_R11_DIMSTYLE_END);
      break;

    case SECTION_VX:
      write_sentinel (dat, DWG_SENTINEL_R11_VX_BEGIN);
      if (tblnum)
        {
          LOG_WARN ("VX table ignored");
          // error |= dwg_encode_VX (dat, obj);
          tblnum = 0;
        }
      write_sentinel (dat, DWG_SENTINEL_R11_VX_END);
      break;

    case SECTION_HEADER_R11:
    default:
      LOG_ERROR ("Invalid table id %d", id);
      tbl->number = 0;
      break;
    }
  // dat->byte = tbl->address + (tbl->number * tbl->size);
  return error;
}

#ifndef DEBUG_CLASSES
static void
disable_3DSOLID_materials (Dwg_Object *obj)
{
  BITCODE_BL num_materials = 0;
  // or some child of it
  Dwg_Entity_BODY *_obj = obj->tio.entity->tio.BODY;
  if (dwg_dynapi_entity_value (_obj, obj->name, "num_materials",
                               &num_materials, NULL)
      && num_materials)
    {
      num_materials = 0;
      dwg_dynapi_entity_set_value (_obj, obj->name, "num_materials",
                                   &num_materials, false);
    }
}
#endif

/*
static int
encode_preR13_POLYLINE (Bit_Chain *restrict dat, Dwg_Object *restrict obj)
{
  int error = 0;
  switch (obj->fixedtype)
    {
    case DWG_TYPE_VERTEX_2D:
      error = dwg_encode_VERTEX_2D (dat, obj);
      break;
    case DWG_TYPE_VERTEX_3D:
      error = dwg_encode_VERTEX_3D (dat, obj);
      break;
    case DWG_TYPE_VERTEX_MESH:
      error = dwg_encode_VERTEX_MESH (dat, obj);
      break;
    case DWG_TYPE_VERTEX_PFACE:
      error = dwg_encode_VERTEX_PFACE (dat, obj);
      break;
    case DWG_TYPE_VERTEX_PFACE_FACE:
      error = dwg_encode_VERTEX_PFACE_FACE (dat, obj);
      break;
    case DWG_TYPE_POLYLINE_2D:
      error = dwg_encode_POLYLINE_2D (dat, obj);
      break;
    case DWG_TYPE_POLYLINE_3D:
      error = dwg_encode_POLYLINE_3D (dat, obj);
      break;
    case DWG_TYPE_POLYLINE_PFACE:
      error = dwg_encode_POLYLINE_PFACE (dat, obj);
      break;
    case DWG_TYPE_POLYLINE_MESH:
      error = dwg_encode_POLYLINE_MESH (dat, obj);
      break;
    default:
      LOG_ERROR ("Wrong preR13 POLYLINE/VERTEX type %u (flag %u)",
                 obj->fixedtype, flag_r11);
      error |= DWG_ERR_VALUEOUTOFBOUNDS;
    }
  return error;
}
*/

// blocks might be mixed in-between normal entities. from BLOCK to ENDBLK
// extras begin with a jump, until a jump back
static BITCODE_RL
encode_preR13_entities (EntitySectionIndexR11 section, Bit_Chain *restrict dat,
                        Dwg_Data *restrict dwg, int *restrict error)
{
  BITCODE_RL numentities = 0;
  bool in_blocks = false;
  bool past_blocks = false;
  LOG_INFO ("===========================\n"
            "%s from 0x%zx\n",
            section == ENTITIES_SECTION_INDEX ? "Entities"
            : section == BLOCKS_SECTION_INDEX ? "Blocks"
                                              : "Extras",
            dat->byte);
  for (unsigned index = 0; index < dwg->num_objects; index++)
    {
      Dwg_Object *obj = &dwg->object[index];
      size_t size_pos = 0UL;
      // skip table objects or uninitialized entities
      if (obj->supertype != DWG_SUPERTYPE_ENTITY || !obj->tio.entity)
        {
          if (obj->index && obj->fixedtype)
            LOG_HANDLE ("Skip object %s, number: %d, Fixedtype: %d, Addr: %zx "
                        "(0x%zx)\n",
                        obj->name, obj->index, obj->fixedtype, obj->address,
                        dat->byte);
          continue;
        }
      // deleted, i.e. moved to a BLOCK
      if (obj->fixedtype == DWG_TYPE_UNUSED)
        {
          LOG_TRACE (
              "Deleted entity %s, number: %d, type: %d, Addr: %zx (0x%zx)\n",
              obj->name, obj->index, obj->type, obj->address, dat->byte);
        }
      // skip first 2 mspace block/endblk entities
      if (obj->type == DWG_TYPE_UNUSED_r11 && obj->index < 13)
        {
          LOG_HANDLE ("Unused entity %s, number: %d, Fixedtype: %d, Addr: %zx "
                      "(0x%zx)\n",
                      obj->name, obj->index, obj->fixedtype, obj->address,
                      dat->byte);
          continue;
        }
      // check if block/extras or entity member
      if (dat->version >= R_2_0b && section == ENTITIES_SECTION_INDEX)
        {
          // but belongs to a block
          if (obj->fixedtype == DWG_TYPE_BLOCK)
            {
              Dwg_Object *next_endblk
                  = dwg_get_next_object (dwg, DWG_TYPE_ENDBLK, obj->index);
              if (next_endblk && next_endblk->index > obj->index)
                {
                  index += (next_endblk->index - obj->index);
                  past_blocks = true;
                  LOG_TRACE ("BLOCK: advance to %d. ENDBLK: %d, we are at %d "
                             "Addr: %zx (0x%zx)\n",
                             index + 1, next_endblk->index, obj->index,
                             obj->address, dat->byte);
                  continue;
                }
              else
                {
                  LOG_TRACE ("in BLOCK\n");
                  in_blocks = true;
                }
            }
          if (obj->tio.entity->entmode == 3)
            {
              past_blocks = true;
              LOG_TRACE ("Skip %s in entities section belonging to a block, "
                         "number: %d, "
                         "type: %d, Addr: %zx (0x%zx)\n",
                         obj->name, obj->index, obj->type, obj->address,
                         dat->byte);
              continue;
            }
          // extras entities come after all block entities
          if (past_blocks)
            {
              LOG_TRACE ("Skip extras %s in entities section, "
                         "number: %d, type: %d, Addr: %zx (0x%zx)\n",
                         obj->name, obj->index, obj->type, obj->address,
                         dat->byte);
              continue;
            }
          if (in_blocks)
            {
              if (obj->fixedtype == DWG_TYPE_ENDBLK)
                {
                  LOG_TRACE ("out of BLOCK\n");
                  in_blocks = false;
                  continue;
                }
              else
                {
                  LOG_TRACE (
                      "Skip in block %s in entities section, number: %d, "
                      "type: %d, Addr: %zx (0x%zx)\n",
                      obj->name, obj->index, obj->type, obj->address,
                      dat->byte);
                  continue;
                }
            }
        }
      else if (dat->version >= R_2_0b && section == BLOCKS_SECTION_INDEX)
        {
          if (dat->version < R_2_0b || obj->tio.entity->entmode != 3)
            {
              LOG_TRACE ("Skip entity %s not in block section, number: %d, "
                         "type: %d, Addr: %zx (0x%zx)\n",
                         obj->name, obj->index, obj->type, obj->address,
                         dat->byte);
              continue;
            }
          if (obj->fixedtype == DWG_TYPE_BLOCK)
            {
              Dwg_Object *next_endblk
                  = dwg_get_next_object (dwg, DWG_TYPE_ENDBLK, obj->index);
              if (next_endblk && obj->tio.entity && obj->tio.entity->tio.BLOCK
                  && obj->tio.entity->tio.BLOCK->name
                  && strEQc (obj->tio.entity->tio.BLOCK->name, "*MODEL_SPACE"))
                {
                  // skip model_space entities
                  int adv = next_endblk->index - obj->index;
                  LOG_TRACE (
                      "Skip %d *MODEL_SPACE block entities, number: %d, "
                      "type: %d, Addr: %zx (0x%zx)\n",
                      adv, obj->index, obj->type, obj->address, dat->byte);
                  index += adv;
                  continue;
                }
              // skip empty BLOCKS, i.e. MODEL_SPACE
              if (next_endblk && next_endblk->index == obj->index + 1)
                {
                  LOG_TRACE ("Skip empty block, number: %d, "
                             "type: %d, Addr: %zx (0x%zx)\n",
                             obj->index, obj->type, obj->address, dat->byte);
                  continue;
                }
              else
                in_blocks = true;
            }
          if (!in_blocks)
            {
              LOG_TRACE ("Skip entity %s in block section, number: %d, "
                         "type: %d, Addr: %zx (0x%zx)\n",
                         obj->name, obj->index, obj->type, obj->address,
                         dat->byte);
              continue;
            }
        }
      // extras entities come after all block entities in the object list
      if (section == EXTRAS_SECTION_INDEX)
        {
          if (obj->tio.entity->entmode == 3)
            {
              past_blocks = true;
              continue; // skip block entities
            }
          if (!past_blocks)
            continue; // skip main entities (before blocks)
        }

      if (dat->byte + obj->size < dat->size)
        bit_chain_alloc_size (dat, obj->size);
      numentities++;
      obj->address = dat->byte;
      LOG_INFO ("===========================\n"
                "Entity %s, number: %d, Addr: %" PRIuSIZE " (0x%zx)\n",
                obj->name, obj->index, obj->address, dat->byte);
      PRE (R_2_0b)
      {
        bit_write_RS (dat, obj->type);
        LOG_INFO ("type: %d [RS]\n", obj->type)
        if (obj->type > 64)
          LOG_INFO ("deleted\n")
      }
      LATER_VERSIONS
      {
        bit_write_RC (dat, obj->type);
        size_pos = dat->byte + 1; // past the flag
        LOG_INFO ("type: %d [RC]\n", obj->type)
      }

#define CASE_ENCODE_TYPE(ty)                                                  \
  case DWG_TYPE_##ty:                                                         \
    *error |= dwg_encode_##ty (dat, obj);                                     \
    break

      switch (obj->fixedtype)
        {
          CASE_ENCODE_TYPE (LINE);
          CASE_ENCODE_TYPE (POINT);
          CASE_ENCODE_TYPE (CIRCLE);
          CASE_ENCODE_TYPE (SHAPE);
          CASE_ENCODE_TYPE (REPEAT);
          CASE_ENCODE_TYPE (ENDREP);
          CASE_ENCODE_TYPE (TEXT);
          CASE_ENCODE_TYPE (ARC);
          CASE_ENCODE_TYPE (TRACE);
          CASE_ENCODE_TYPE (LOAD); /* convert from pre r2.0 */
          CASE_ENCODE_TYPE (SOLID);
          CASE_ENCODE_TYPE (BLOCK);
          CASE_ENCODE_TYPE (ENDBLK);
          CASE_ENCODE_TYPE (INSERT);
          CASE_ENCODE_TYPE (ATTDEF);
          CASE_ENCODE_TYPE (ATTRIB);
          CASE_ENCODE_TYPE (SEQEND);
          CASE_ENCODE_TYPE (JUMP);
          CASE_ENCODE_TYPE (POLYLINE_2D);
          CASE_ENCODE_TYPE (POLYLINE_3D);
          CASE_ENCODE_TYPE (POLYLINE_PFACE);
          CASE_ENCODE_TYPE (POLYLINE_MESH);
          CASE_ENCODE_TYPE (VERTEX_2D);
          CASE_ENCODE_TYPE (VERTEX_3D);
          CASE_ENCODE_TYPE (VERTEX_MESH);
          CASE_ENCODE_TYPE (VERTEX_PFACE);
          CASE_ENCODE_TYPE (VERTEX_PFACE_FACE);
          CASE_ENCODE_TYPE (_3DLINE);
          CASE_ENCODE_TYPE (_3DFACE);
          CASE_ENCODE_TYPE (DIMENSION_ORDINATE);
          CASE_ENCODE_TYPE (DIMENSION_LINEAR);
          CASE_ENCODE_TYPE (DIMENSION_ALIGNED);
          CASE_ENCODE_TYPE (DIMENSION_ANG3PT);
          CASE_ENCODE_TYPE (DIMENSION_ANG2LN);
          CASE_ENCODE_TYPE (DIMENSION_RADIUS);
          CASE_ENCODE_TYPE (DIMENSION_DIAMETER);
          CASE_ENCODE_TYPE (VIEWPORT);
        default:
          DEBUG_HERE;
          LOG_ERROR ("Unknown object type %d", obj->type)
          break;
        }

      if (in_blocks && obj->fixedtype == DWG_TYPE_ENDBLK)
        in_blocks = false;
      else if (!in_blocks && obj->fixedtype == DWG_TYPE_BLOCK)
        in_blocks = true;

      SINCE (R_2_0)
      {
        // patchup size
        if (!obj->size)
          {
            size_t pos = dat->byte;
            obj->size = (dat->byte - obj->address) & 0xFFFFFFFF;
            SINCE (R_11)
            {
              obj->size += 2; // crc16
            }
            dat->byte = size_pos;
            bit_write_RS (dat, obj->size);
            LOG_TRACE ("-size: %u [RL] (@%" PRIuSIZE ".%u)\n", obj->size,
                       dat->byte, dat->bit)
            dat->byte = pos;
          }
        SINCE (R_11)
        {
          BITCODE_RS crc = bit_calc_CRC (0xC0C1, &dat->chain[obj->address],
                                         obj->size - 2);
          LOG_TRACE ("crc: %04X [RSx] from 0x%zx-0x%zx\n", crc, obj->address,
                     dat->byte);
          bit_write_RS (dat, crc);
        }
      }
    }
  return numentities;
}

// expand aliases: name => CLASSES.dxfname
static const char *
dxf_encode_alias (char *restrict name)
{
  if (strEQc (name, "DICTIONARYWDFLT"))
    return "ACDBDICTIONARYWDFLT";
  else if (strEQc (name, "SECTIONVIEWSTYLE"))
    return "ACDBSECTIONVIEWSTYLE";
  else if (strEQc (name, "PLACEHOLDER"))
    return "ACDBPLACEHOLDER";
  else if (strEQc (name, "DETAILVIEWSTYLE"))
    return "ACDBDETAILVIEWSTYLE";
  else if (strEQc (name, "ASSOCPERSSUBENTMANAGER"))
    return "ACDBASSOCPERSSUBENTMANAGER";
  else if (strEQc (name, "EVALUATION_GRAPH"))
    return "ACAD_EVALUATION_GRAPH";
  else if (strEQc (name, "ASSOCACTION"))
    return "ACDBASSOCACTION";
  else if (strEQc (name, "ASSOCALIGNEDDIMACTIONBODY"))
    return "ACDBASSOCALIGNEDDIMACTIONBODY";
  else if (strEQc (name, "ASSOCOSNAPPOINTREFACTIONPARAM"))
    return "ACDBASSOCOSNAPPOINTREFACTIONPARAM";
  else if (strEQc (name, "ASSOCVERTEXACTIONPARAM"))
    return "ACDBASSOCVERTEXACTIONPARAM";
  else if (strEQc (name, "ASSOCGEOMDEPENDENCY"))
    return "ACDBASSOCGEOMDEPENDENCY";
  else if (strEQc (name, "ASSOCDEPENDENCY"))
    return "ACDBASSOCDEPENDENCY";
  else if (strEQc (name, "TABLE"))
    return "ACAD_TABLE";
  else
    return NULL;
}

Dwg_Class *
dwg_encode_get_class (Dwg_Data *dwg, Dwg_Object *obj)
{
  int i;
  Dwg_Class *klass = NULL;
  if (!dwg || !dwg->dwg_class)
    return NULL;
  // indxf has a different class order
  if (obj->dxfname) // search class by name, not offset
    {
      int invalid_klass = 0;
      for (i = 0; i < dwg->num_classes; i++)
        {
          klass = &dwg->dwg_class[i];
          if (!klass->dxfname)
            {
              invalid_klass++;
              continue;
            }
          if (strEQ (obj->dxfname, klass->dxfname))
            {
              obj->type = 500 + i;
              break;
            }
          else
            {
              // alias DICTIONARYWDFLT => ACDBDICTIONARYWDFLT
              const char *alias = dxf_encode_alias (obj->dxfname);
              if (alias && klass->dxfname && strEQ (alias, klass->dxfname))
                {
                  // a static string, which cannot be FREE'd. important for
                  // indxf
                  if (dwg->opts & DWG_OPTS_IN)
                    obj->dxfname = STRDUP ((char *)alias);
                  else
                    obj->dxfname = (char *)alias;
                  obj->type = 500 + i;
                  break;
                }
              klass = NULL; // inefficient

              if (invalid_klass > 2 && !(dwg->opts & DWG_OPTS_IN))
                goto search_by_index;
            }
        }
    }
  else // search by index
    {
    search_by_index:
      i = obj->type - 500;
      if (i < 0 || i >= (int)dwg->num_classes)
        {
          LOG_WARN ("Invalid object type %d, only %u classes", obj->type,
                    dwg->num_classes);
          return NULL;
        }

      klass = &dwg->dwg_class[i];
      if (!klass->dxfname)
        return NULL;
      if (dwg->opts & DWG_OPTS_IN)
        obj->dxfname = STRDUP (klass->dxfname);
      else
        obj->dxfname = klass->dxfname;
    }
  return klass;
}

/** dwg_encode_variable_type
 * Encode object by class name, not type. if type > 500.
 * Returns 0 on success, else some Dwg_Error.
 */
static int
dwg_encode_variable_type (Dwg_Data *restrict dwg, Bit_Chain *restrict dat,
                          Dwg_Object *restrict obj)
{
  int is_entity;
  BITCODE_BS oldtype = obj->type;
  Dwg_Class *klass = dwg_encode_get_class (dwg, obj);
  if (!klass)
    return DWG_ERR_INVALIDTYPE;
  is_entity = dwg_class_is_entity (klass);
  // check if it really was an entity
  if ((is_entity && obj->supertype == DWG_SUPERTYPE_OBJECT)
      || (!is_entity && obj->supertype == DWG_SUPERTYPE_ENTITY))
    {
      if (is_dwg_object (obj->name))
        {
          if (is_entity)
            {
              LOG_INFO ("Fixup Class %s item_class_id to %s for %s\n",
                        klass->dxfname, "OBJECT", obj->name);
              klass->item_class_id = 0x1f2;
              if (!klass->dxfname || strNE (klass->dxfname, obj->dxfname))
                {
                  FREE (klass->dxfname);
                  klass->dxfname = STRDUP (obj->dxfname);
                }
              is_entity = 0;
            }
          else
            {
              LOG_INFO ("Fixup %s.supertype to %s\n", obj->name, "OBJECT");
              obj->supertype = DWG_SUPERTYPE_OBJECT;
            }
        }
      else if (is_dwg_entity (obj->name))
        {
          if (!is_entity)
            {
              LOG_INFO ("Fixup Class %s item_class_id to %s for %s\n",
                        klass->dxfname, "ENTITY", obj->name);
              klass->item_class_id = 0x1f3;
              if (!klass->dxfname || strNE (klass->dxfname, obj->dxfname))
                {
                  FREE (klass->dxfname);
                  klass->dxfname = STRDUP (obj->dxfname);
                }
              is_entity = 1;
            }
          else
            {
              LOG_INFO ("Fixup %s.supertype to %s", obj->name, "ENTITY");
              obj->supertype = DWG_SUPERTYPE_ENTITY;
            }
        }
      else
        {
          LOG_ERROR ("Illegal Class %s is_%s item_class_id for %s",
                     klass->dxfname, is_entity ? "entity" : "object",
                     obj->name);
          return DWG_ERR_INVALIDTYPE;
        }
    }

  if (dwg->opts & DWG_OPTS_IN) // DXF or JSON import
    {
      size_t pos = bit_position (dat);
      /* Should not be triggered. Only when undef ENCODE_UNKNOWN_AS_DUMMY */
      if (is_type_unstable (obj->fixedtype)
          && (/*obj->fixedtype == DWG_TYPE_WIPEOUT || */
              obj->fixedtype == DWG_TYPE_TABLEGEOMETRY))
        {
          LOG_WARN ("Skip broken %s", obj->name); // acad crashes still
          obj->type = is_entity ? DWG_TYPE_UNKNOWN_ENT : DWG_TYPE_PLACEHOLDER;
          klass->dxfname = STRDUP (is_entity ? "UNKNOWN_ENT" : "UNKNOWN_OBJ");
        }
      if (oldtype != obj->type)
        {
          dat->byte = obj->address;
          dat->bit = 0;
          LOG_TRACE ("fixup Type: %d [BS] @%" PRIuSIZE "\n", obj->type,
                     obj->address);
          bit_write_BS (dat, obj->type); // fixup wrong type
          bit_set_position (dat, pos);
        }
    }

    // clang-format off
  #include "classes.inc"
  // clang-format on

  LOG_WARN ("Unknown Class %s %d %s (0x%x%s)", is_entity ? "entity" : "object",
            klass->number, klass->dxfname, klass->proxyflag,
            klass->is_zombie ? "is_zombie" : "")

#undef WARN_UNHANDLED_CLASS
#undef WARN_UNSTABLE_CLASS

  return DWG_ERR_UNHANDLEDCLASS;
}

bool
dwg_encode_unknown_bits (Bit_Chain *restrict dat, Dwg_Object *restrict obj)
{
  Dwg_Data *restrict dwg = obj->parent;
  if (dwg->header.version == dwg->header.from_version && obj->unknown_bits
      && obj->num_unknown_bits) // cannot calculate
    {
      int len = obj->num_unknown_bits / 8;
      const int mod = obj->num_unknown_bits % 8;
      bit_write_TF (dat, obj->unknown_bits, len);
      if (mod)
        {
          const BITCODE_TF tail = obj->unknown_bits + len;
          for (int i = 0; i < mod; i++)
            bit_write_B (dat, (tail[0] >> i) & 1);
          len++;
        }
      LOG_TRACE ("unknown_bits: %d/%u [TF]\n", len,
                 (unsigned)obj->num_unknown_bits);
      LOG_TRACE_TF (obj->unknown_bits, len);
      return true;
    }
  else
    return false;
}

static int
dwg_encode_raw_UNKNOWN_ENT (Bit_Chain *restrict dat, Dwg_Object *restrict obj)
{
  int error;
  Bit_Chain _hdl_dat = { 0 };
  Bit_Chain *hdl_dat = &_hdl_dat;
  Bit_Chain *str_dat = dat;

  LOG_INFO ("Encode entity UNKNOWN_ENT\n");
  bit_chain_init_dat (hdl_dat, 128, dat);
  error = dwg_encode_entity (obj, dat, hdl_dat, str_dat);
  if (error)
    {
      if (hdl_dat != dat && hdl_dat->chain != dat->chain)
        bit_chain_free (hdl_dat);
      return error;
    }
  if (!dwg_encode_unknown_bits (dat, obj))
    {
      if (hdl_dat != dat && hdl_dat->chain != dat->chain)
        bit_chain_free (hdl_dat);
      return DWG_ERR_UNHANDLEDCLASS;
    }
  dwg_encode_unknown_rest (dat, obj);
  if (hdl_dat != dat && hdl_dat->chain != dat->chain)
    bit_chain_free (hdl_dat);
  return 0;
}

static int
dwg_encode_raw_UNKNOWN_OBJ (Bit_Chain *restrict dat, Dwg_Object *restrict obj)
{
  int error;
  Bit_Chain _hdl_dat = { 0 };
  Bit_Chain *hdl_dat = &_hdl_dat;
  Bit_Chain *str_dat = dat;

  LOG_INFO ("Encode object UNKNOWN_OBJ\n");
  bit_chain_init_dat (hdl_dat, 128, dat);
  error = dwg_encode_object (obj, dat, hdl_dat, str_dat);
  if (error)
    {
      if (hdl_dat != dat && hdl_dat->chain != dat->chain)
        bit_chain_free (hdl_dat);
      return error;
    }
  if (!dwg_encode_unknown_bits (dat, obj))
    {
      if (hdl_dat != dat && hdl_dat->chain != dat->chain)
        bit_chain_free (hdl_dat);
      return DWG_ERR_UNHANDLEDCLASS;
    }
  dwg_encode_unknown_rest (dat, obj);
  if (hdl_dat != dat && hdl_dat->chain != dat->chain)
    bit_chain_free (hdl_dat);
  return 0;
}

int
dwg_encode_add_object (Dwg_Object *restrict obj, Bit_Chain *restrict dat,
                       size_t address)
{
  int error = 0;
  size_t end_address = address + obj->size;
  Dwg_Data *dwg = obj->parent;

  PRE (R_2004a)
  {
    if (!address)
      return DWG_ERR_INVALIDDWG;
    assert (address);
  }
  dat->byte = address;
  dat->bit = 0;

  LOG_INFO ("Object number: %lu", (unsigned long)obj->index);
  if (obj->size > DWG_MAX_OBJSIZE)
    {
      LOG_ERROR ("Object size %u overflow, skipped", obj->size);
      // limit the size (oss-fuzz #41021)
      obj->size = 0;
      return DWG_ERR_VALUEOUTOFBOUNDS;
    }
  if (!dwg_supports_obj (dwg, obj))
    {
      if (dwg->header.version < R_13b1)
        {
          LOG_ERROR ("\nObject %s unsupported, skipped", obj->name);
          return 0;
        }
      else
        {
          LOG_WARN ("\nObject %s unsupported", obj->name);
        }
    }
  if (dat->byte + obj->size < dat->size)
    bit_chain_alloc_size (dat, obj->size);

  // First write an approximate size here.
  // Then calculate size from the fields. Either <0x7fff or more.
  // Patch it afterwards and check old<>new size if enough space allocated.
  bit_write_MS (dat, obj->size);
  obj->address = dat->byte;
  PRE (R_2010b)
  {
    bit_write_BS (dat, obj->type);
    LOG_INFO (", Size: " FORMAT_MS " [MS], Type: " FORMAT_BS
              " [BS], Address: %" PRIuSIZE "\n",
              obj->size, obj->type, obj->address)
  }
  LATER_VERSIONS
  {
    if (!obj->handlestream_size && obj->bitsize)
      obj->handlestream_size = obj->size * 8 - obj->bitsize;
    bit_write_UMC (dat, obj->handlestream_size);
    obj->address = dat->byte;
    bit_write_BOT (dat, obj->type);
    LOG_INFO (", Size: " FORMAT_MS " [MS], Hdlsize: " FORMAT_UMC
              " [UMC], Type: %d [BOT], Address: %" PRIuSIZE "\n",
              obj->size, obj->handlestream_size, obj->type, obj->address)
  }

  /* Write the specific type to dat */
  switch (obj->fixedtype)
    {
    case DWG_TYPE_TEXT:
      error = dwg_encode_TEXT (dat, obj);
      break;
    case DWG_TYPE_ATTRIB:
      error = dwg_encode_ATTRIB (dat, obj);
      break;
    case DWG_TYPE_ATTDEF:
      error = dwg_encode_ATTDEF (dat, obj);
      break;
    case DWG_TYPE_BLOCK:
      error = dwg_encode_BLOCK (dat, obj);
      break;
    case DWG_TYPE_ENDBLK:
      error = dwg_encode_ENDBLK (dat, obj);
      break;
    case DWG_TYPE_SEQEND:
      error = dwg_encode_SEQEND (dat, obj);
      break;
    case DWG_TYPE_INSERT:
      error = dwg_encode_INSERT (dat, obj);
      break;
    case DWG_TYPE_MINSERT:
      error = dwg_encode_MINSERT (dat, obj);
      break;
    case DWG_TYPE_VERTEX_2D:
      error = dwg_encode_VERTEX_2D (dat, obj);
      break;
    case DWG_TYPE_VERTEX_3D:
      error = dwg_encode_VERTEX_3D (dat, obj);
      break;
    case DWG_TYPE_VERTEX_MESH:
      error = dwg_encode_VERTEX_MESH (dat, obj);
      break;
    case DWG_TYPE_VERTEX_PFACE:
      error = dwg_encode_VERTEX_PFACE (dat, obj);
      break;
    case DWG_TYPE_VERTEX_PFACE_FACE:
      error = dwg_encode_VERTEX_PFACE_FACE (dat, obj);
      break;
    case DWG_TYPE_POLYLINE_2D:
      error = dwg_encode_POLYLINE_2D (dat, obj);
      break;
    case DWG_TYPE_POLYLINE_3D:
      error = dwg_encode_POLYLINE_3D (dat, obj);
      break;
    case DWG_TYPE_ARC:
      error = dwg_encode_ARC (dat, obj);
      break;
    case DWG_TYPE_CIRCLE:
      error = dwg_encode_CIRCLE (dat, obj);
      break;
    case DWG_TYPE_LINE:
      error = dwg_encode_LINE (dat, obj);
      break;
    case DWG_TYPE_DIMENSION_ORDINATE:
      error = dwg_encode_DIMENSION_ORDINATE (dat, obj);
      break;
    case DWG_TYPE_DIMENSION_LINEAR:
      error = dwg_encode_DIMENSION_LINEAR (dat, obj);
      break;
    case DWG_TYPE_DIMENSION_ALIGNED:
      error = dwg_encode_DIMENSION_ALIGNED (dat, obj);
      break;
    case DWG_TYPE_DIMENSION_ANG3PT:
      error = dwg_encode_DIMENSION_ANG3PT (dat, obj);
      break;
    case DWG_TYPE_DIMENSION_ANG2LN:
      error = dwg_encode_DIMENSION_ANG2LN (dat, obj);
      break;
    case DWG_TYPE_DIMENSION_RADIUS:
      error = dwg_encode_DIMENSION_RADIUS (dat, obj);
      break;
    case DWG_TYPE_DIMENSION_DIAMETER:
      error = dwg_encode_DIMENSION_DIAMETER (dat, obj);
      break;
    case DWG_TYPE_POINT:
      error = dwg_encode_POINT (dat, obj);
      break;
    case DWG_TYPE__3DFACE:
      error = dwg_encode__3DFACE (dat, obj);
      break;
    case DWG_TYPE_POLYLINE_PFACE:
      error = dwg_encode_POLYLINE_PFACE (dat, obj);
      break;
    case DWG_TYPE_POLYLINE_MESH:
      error = dwg_encode_POLYLINE_MESH (dat, obj);
      break;
    case DWG_TYPE_SOLID:
      error = dwg_encode_SOLID (dat, obj);
      break;
    case DWG_TYPE_TRACE:
      error = dwg_encode_TRACE (dat, obj);
      break;
    case DWG_TYPE_SHAPE:
      error = dwg_encode_SHAPE (dat, obj);
      break;
    case DWG_TYPE_VIEWPORT:
      error = dwg_encode_VIEWPORT (dat, obj);
      break;
    case DWG_TYPE_ELLIPSE:
      error = dwg_encode_ELLIPSE (dat, obj);
      break;
    case DWG_TYPE_SPLINE:
      error = dwg_encode_SPLINE (dat, obj);
      break;
    case DWG_TYPE_REGION:
#ifndef DEBUG_CLASSES
      disable_3DSOLID_materials (obj);
#endif
      error = dwg_encode_REGION (dat, obj);
      break;
    case DWG_TYPE__3DSOLID:
#ifndef DEBUG_CLASSES
      disable_3DSOLID_materials (obj);
#endif
      error = dwg_encode__3DSOLID (dat, obj);
      break;
    case DWG_TYPE_BODY:
#ifndef DEBUG_CLASSES
      disable_3DSOLID_materials (obj);
#endif
      error = dwg_encode_BODY (dat, obj);
      break;
    case DWG_TYPE_RAY:
      error = dwg_encode_RAY (dat, obj);
      break;
    case DWG_TYPE_XLINE:
      error = dwg_encode_XLINE (dat, obj);
      break;
    case DWG_TYPE_DICTIONARY:
      error = dwg_encode_DICTIONARY (dat, obj);
      break;
    case DWG_TYPE_MTEXT:
      error = dwg_encode_MTEXT (dat, obj);
      break;
    case DWG_TYPE_LEADER:
      error = dwg_encode_LEADER (dat, obj);
      break;
    case DWG_TYPE_TOLERANCE:
      error = dwg_encode_TOLERANCE (dat, obj);
      break;
    case DWG_TYPE_MLINE:
      error = dwg_encode_MLINE (dat, obj);
      break;
    case DWG_TYPE_BLOCK_CONTROL:
      error = dwg_encode_BLOCK_CONTROL (dat, obj);
      break;
    case DWG_TYPE_BLOCK_HEADER:
      error = dwg_encode_BLOCK_HEADER (dat, obj);
      break;
    case DWG_TYPE_LAYER_CONTROL:
      error = dwg_encode_LAYER_CONTROL (dat, obj);
      break;
    case DWG_TYPE_LAYER:
      error = dwg_encode_LAYER (dat, obj);
      break;
    case DWG_TYPE_STYLE_CONTROL:
      error = dwg_encode_STYLE_CONTROL (dat, obj);
      break;
    case DWG_TYPE_STYLE:
      error = dwg_encode_STYLE (dat, obj);
      break;
    case DWG_TYPE_LTYPE_CONTROL:
      error = dwg_encode_LTYPE_CONTROL (dat, obj);
      break;
    case DWG_TYPE_LTYPE:
      error = dwg_encode_LTYPE (dat, obj);
      break;
    case DWG_TYPE_VIEW_CONTROL:
      error = dwg_encode_VIEW_CONTROL (dat, obj);
      break;
    case DWG_TYPE_VIEW:
      error = dwg_encode_VIEW (dat, obj);
      break;
    case DWG_TYPE_UCS_CONTROL:
      error = dwg_encode_UCS_CONTROL (dat, obj);
      break;
    case DWG_TYPE_UCS:
      error = dwg_encode_UCS (dat, obj);
      break;
    case DWG_TYPE_VPORT_CONTROL:
      error = dwg_encode_VPORT_CONTROL (dat, obj);
      break;
    case DWG_TYPE_VPORT:
      error = dwg_encode_VPORT (dat, obj);
      break;
    case DWG_TYPE_APPID_CONTROL:
      error = dwg_encode_APPID_CONTROL (dat, obj);
      break;
    case DWG_TYPE_APPID:
      error = dwg_encode_APPID (dat, obj);
      break;
    case DWG_TYPE_DIMSTYLE_CONTROL:
      error = dwg_encode_DIMSTYLE_CONTROL (dat, obj);
      break;
    case DWG_TYPE_DIMSTYLE:
      error = dwg_encode_DIMSTYLE (dat, obj);
      break;
    case DWG_TYPE_VX_CONTROL:
      error = dwg_encode_VX_CONTROL (dat, obj);
      break;
    case DWG_TYPE_VX_TABLE_RECORD:
      error = dwg_encode_VX_TABLE_RECORD (dat, obj);
      break;
    case DWG_TYPE_GROUP:
      error = dwg_encode_GROUP (dat, obj);
      break;
    case DWG_TYPE_MLINESTYLE:
      if (dat->version >= R_2018 && dat->from_version < R_2018)
        dwg_upgrade_MLINESTYLE (dwg, obj->tio.object->tio.MLINESTYLE);
      else if (dat->version < R_2018 && dat->from_version > +R_2018)
        dwg_downgrade_MLINESTYLE (obj->tio.object->tio.MLINESTYLE);
      error = dwg_encode_MLINESTYLE (dat, obj);
      (void)dwg_encode_get_class (dwg, obj);
      break;
    case DWG_TYPE_OLE2FRAME:
      error = dwg_encode_OLE2FRAME (dat, obj);
      (void)dwg_encode_get_class (dwg, obj);
      break;
    case DWG_TYPE_DUMMY:
      error = dwg_encode_DUMMY (dat, obj);
      break;
    case DWG_TYPE_LONG_TRANSACTION:
      error = dwg_encode_LONG_TRANSACTION (dat, obj);
      break;
    case DWG_TYPE_LWPOLYLINE:
      error = dwg_encode_LWPOLYLINE (dat, obj);
      (void)dwg_encode_get_class (dwg, obj);
      break;
    case DWG_TYPE_HATCH:
      error = dwg_encode_HATCH (dat, obj);
      (void)dwg_encode_get_class (dwg, obj);
      break;
    case DWG_TYPE_XRECORD:
      error = dwg_encode_XRECORD (dat, obj);
      (void)dwg_encode_get_class (dwg, obj);
      break;
    case DWG_TYPE_PLACEHOLDER:
      error = dwg_encode_PLACEHOLDER (dat, obj);
      (void)dwg_encode_get_class (dwg, obj);
      break;
    case DWG_TYPE_OLEFRAME:
      error = dwg_encode_OLEFRAME (dat, obj);
      (void)dwg_encode_get_class (dwg, obj);
      break;
    case DWG_TYPE_VBA_PROJECT:
      // LOG_ERROR ("Unhandled Object VBA_PROJECT. Has its own AcDb::VBAProject
      // section");
      error = dwg_encode_VBA_PROJECT (dat, obj);
      (void)dwg_encode_get_class (dwg, obj);
      break;
    case DWG_TYPE_LAYOUT:
      error |= dwg_encode_LAYOUT (dat, obj);
      (void)dwg_encode_get_class (dwg, obj);
      break;
    case DWG_TYPE_PROXY_ENTITY:
      error = dwg_encode_PROXY_ENTITY (dat, obj);
      break;
    case DWG_TYPE_PROXY_OBJECT:
      error = dwg_encode_PROXY_OBJECT (dat, obj);
      break;
    case DWG_TYPE_UNKNOWN_ENT:
      if ((dwg->opts & DWG_OPTS_INJSON)
          && dwg->header.version == dwg->header.from_version)
        error = dwg_encode_raw_UNKNOWN_ENT (dat, obj);
      else
        error = DWG_ERR_UNHANDLEDCLASS;
      break;
    case DWG_TYPE_UNKNOWN_OBJ:
      if ((dwg->opts & DWG_OPTS_INJSON)
          && dwg->header.version == dwg->header.from_version)
        error = dwg_encode_raw_UNKNOWN_OBJ (dat, obj);
      else
        error = DWG_ERR_UNHANDLEDCLASS;
      break;
    /*
    case DWG_TYPE_REPEAT:
      error = dwg_encode_REPEAT (dat, obj);
      break;
    case DWG_TYPE_ENDREP:
      error = dwg_encode_ENDREP (dat, obj);
      break;
    case DWG_TYPE__3DLINE:
      error = dwg_encode__3DLINE (dat, obj);
      break;
    */
    default:
#ifndef DEBUG_CLASSES
      if (dwg && dwg_dynapi_entity_field (obj->name, "num_materials"))
        disable_3DSOLID_materials (obj);
#endif
      if (dwg && obj->type == dwg->layout_type
          && obj->fixedtype == DWG_TYPE_LAYOUT)
        {
          error = dwg_encode_LAYOUT (dat, obj);
          (void)dwg_encode_get_class (dwg, obj);
        }
      else if (dwg != NULL
               && (error = dwg_encode_variable_type (dwg, dat, obj))
                      & DWG_ERR_UNHANDLEDCLASS)
        {
          int is_entity;
          Dwg_Class *klass = dwg_encode_get_class (dwg, obj);
          if (klass)
            is_entity = klass->item_class_id == 0x1f2
                        && obj->supertype == DWG_SUPERTYPE_ENTITY;
          else
            is_entity = obj->supertype == DWG_SUPERTYPE_ENTITY;

          assert (address);
          dat->byte = address; // restart and write into the UNKNOWN_OBJ object
          dat->bit = 0;

          bit_write_MS (dat, obj->size); // unknown blobs have a known size
          if (dat->version >= R_2010)
            {
              bit_write_UMC (dat, obj->handlestream_size);
              bit_write_BOT (dat, obj->type);
            }
          else
            bit_write_BS (dat, obj->type);

          // from json and dwg can write to these. from dxf not.
          if (is_entity)
            {
              if (obj->bitsize
                  && dwg->header.version == dwg->header.from_version)
                obj->was_bitsize_set = 1;
              error = dwg_encode_UNKNOWN_ENT (dat, obj);
            }
          else
            {
              // skip START_OBJECT_HANDLE_STREAM (see DWG_OBJECT_END)
              // unknown_bits already includes that.
              if (!obj->hdlpos)
                {
                  if (obj->bitsize)
                    {
                      obj->hdlpos = (obj->address * 8) + obj->bitsize;
                      if (dwg->header.version == dwg->header.from_version)
                        obj->was_bitsize_set = 1;
                    }
                  else
                    obj->hdlpos = (obj->address * 8) + obj->num_unknown_bits;
                }
              error = dwg_encode_UNKNOWN_OBJ (dat, obj);
            }
        }
    }

  /* ADD/DXF/JSON/RW across versions: patchup size and bitsize */
  /* Across versions size+bitsize must be recalculated.
     Sizes are unreliable when changing versions. */
  if (!obj->size || dwg->header.from_version != dwg->header.version
      || obj->was_bitsize_set)
    {
      size_t pos = bit_position (dat);
      BITCODE_RL old_size = obj->size;
      if (dwg->header.version < R_2004 || obj->index)
        {
          if (!address)
            return DWG_ERR_INVALIDDWG;
          assert (address);
        }
      if (dat->byte > obj->address)
        {
          // The size and CRC fields are not included in the obj->size
          obj->size = (dat->byte - obj->address) & 0xFFFFFFFF;
          if (dat->bit)
            obj->size++;
        }
      while (dat->byte >= dat->size)
        bit_chain_alloc (dat);
      // assert (obj->bitsize); // on errors
      if (!obj->bitsize
          || (dwg->header.from_version != dwg->header.version
              // and not calculated from HANDLE_STREAM or via unknown_bits
              // already
              && !obj->was_bitsize_set))
        {
          LOG_TRACE ("-bitsize calc from address (no handle) @%" PRIuSIZE
                     ".%u\n",
                     dat->byte - obj->address, dat->bit);
          obj->bitsize = (pos - (obj->address * 8)) & 0xFFFFFFFF;
        }
      bit_set_position (dat, address * 8);
      if (obj->size > UINT32_C (0x7fff) && old_size <= UINT32_C (0x7fff))
        {
          // FIXME with overlarge sizes >0x7fff memmove dat right by 2, one
          // more CRC RS added.
          LOG_INFO (
              "overlarge MS size %lu > 0x7fff (was %lu) @%" PRIuSIZE "\n",
              (unsigned long)obj->size, (unsigned long)old_size, dat->byte);
          if (dat->byte + obj->size + 4 > dat->size)
            bit_chain_alloc_size (dat,
                                  (dat->byte + obj->size + 4) - dat->size);
          memmove (&dat->chain[dat->byte + 2], &dat->chain[dat->byte],
                   obj->size + 2);
          // obj->size += 2;
          // obj->bitsize += 16;
          obj->address += 2;
          obj->bitsize_pos += 16;
          pos += 16;
        }
      if (obj->size <= UINT32_C (0x7fff) && old_size > UINT32_C (0x7fff))
        {
          // with old overlarge sizes >0x7fff memmove dat left by 2, one RS
          // removed.
          LOG_INFO ("was overlarge MS size %lu => %lu @%" PRIuSIZE "\n",
                    (unsigned long)old_size, (unsigned long)obj->size,
                    dat->byte);
          memmove (&dat->chain[dat->byte], &dat->chain[dat->byte + 2],
                   obj->size + 2);
          // obj->size -= 2;
          // obj->bitsize -= 16;
          obj->address -= 2;
          obj->bitsize_pos -= 16;
          pos -= 16;
        }
      bit_write_MS (dat, obj->size);
      LOG_TRACE ("-size: %u [MS] @%" PRIuSIZE "\n", obj->size, address);
      SINCE (R_2013b)
      {
        if (!obj->handlestream_size && obj->bitsize)
          obj->handlestream_size = (obj->size * 8) - obj->bitsize;
        bit_write_UMC (dat, obj->handlestream_size);
        LOG_TRACE ("-handlestream_size: " FORMAT_UMC " [UMC]\n",
                   obj->handlestream_size);
      }
      VERSIONS (R_13b1, R_2007)
      {
        if (obj->bitsize_pos && obj->bitsize)
          {
            bit_set_position (dat, obj->bitsize_pos);
            bit_write_RL (dat, obj->bitsize);
            LOG_TRACE ("-bitsize: %u [RL] @%" PRIuSIZE ".%u\n", obj->bitsize,
                       obj->bitsize_pos / 8, (unsigned)obj->bitsize_pos % 8);
          }
      }
      bit_set_position (dat, pos);
    }

  /* Now 1 padding bits until next byte, and then a RS CRC */
  if (dat->bit)
    LOG_TRACE ("padding: +%d [*B]\n", 8 - dat->bit)
  while (dat->bit)
    bit_write_B (dat, 1);
  end_address = obj->address + obj->size;
  if (end_address != dat->byte)
    {
      if (obj->size)
        {
          LOG_ERROR ("Wrong object size: %" PRIuSIZE " + %u = %" PRIuSIZE
                     " != %" PRIuSIZE ": %ld off",
                     address, obj->size, end_address, dat->byte,
                     (long)(end_address - dat->byte));
          // dat->byte = end_address;
        }
    }
  assert (!dat->bit);
  bit_write_CRC (dat, address, 0xC0C1);
  return error;
}

int
dwg_encode_unknown_rest (Bit_Chain *restrict dat, Dwg_Object *restrict obj)
{
  if (obj->num_unknown_rest)
    {
      unsigned len = obj->num_unknown_rest / 8;
      const int mod = obj->num_unknown_rest % 8;
      bit_write_TF (dat, obj->unknown_rest, len);
      if (mod)
        {
          const BITCODE_TF tail = obj->unknown_rest + len;
          for (int i = 0; i < mod; i++)
            bit_write_B (dat, (tail[0] >> i) & 1);
          len++;
        }
      LOG_TRACE ("unknown_rest: %u/%u [TF]\n", len,
                 (unsigned)obj->num_unknown_rest);
      LOG_TRACE_TF (obj->unknown_rest, len);
    }
  return 0;
}

/** writes the data part, if there's no raw.
 */
static int
dwg_encode_eed_data (Bit_Chain *restrict dat, Dwg_Eed_Data *restrict data,
                     const int i)
{
  size_t pos = bit_position (dat);
  size_t size;
  if (dat->byte + 24 < dat->size)
    bit_chain_alloc_size (dat, 24);
  bit_write_RC (dat, data->code);
  LOG_TRACE ("EED[%d] code: %d [RC] ", i, data->code);
  switch (data->code)
    {
    case 0:
      {
        PRE (R_13b1)
        {
          // TU only if from r2007+ DWG, not JSON, DXF
          BITCODE_RS length = data->u.eed_0.is_tu ? data->u.eed_0.length
                                                  : data->u.eed_0_r2007.length;
          char *s;
          if (length + 1 + dat->byte < dat->size)
            bit_chain_alloc_size (dat, (length + 1 + dat->byte) - dat->size);
          if (length > 255)
            {
              LOG_ERROR ("eed: overlong string %d stripped", (int)length);
              length = 255;
            }
          if (data->u.eed_0.is_tu)
            {
              char *dest = bit_embed_TU_size (
                  (BITCODE_RS *)&data->u.eed_0_r2007.string, length);
              s = dest;
            }
          else
            {
              s = (char *)data->u.eed_0.string;
            }
          bit_write_RC (dat, length & 0xff);
          bit_write_TF (dat, (unsigned char *)s, length);
          LOG_TRACE ("string: len=" FORMAT_RC " [RC] \"%s\" [TF]",
                     (BITCODE_RC)(length & 0xff), s);
          if (data->u.eed_0.is_tu)
            FREE (s);
        }
        VERSIONS (R_13b1, R_2007)
        {
          // only if from r2007+ DWG, not JSON, DXF
          if (data->u.eed_0.is_tu)
            {
              BITCODE_RS length = data->u.eed_0_r2007.length;
              BITCODE_RS *s = (BITCODE_RS *)&data->u.eed_0_r2007.string;
              char *dest;
              if (length + 5 + dat->byte < dat->size)
                bit_chain_alloc_size (dat,
                                      (length + 5 + dat->byte) - dat->size);
              if (length > 255)
                {
                  LOG_ERROR ("eed: overlong string %d stripped", (int)length);
                  length = 255;
                }
              dest = bit_embed_TU_size (s, length);
              bit_write_RC (dat, length);
              bit_write_RS (dat, dat->codepage);
              bit_write_TF (dat, (unsigned char *)dest, length);
              LOG_TRACE ("string: len=" FORMAT_RS " [RC] cp=" FORMAT_RS
                         " [RS] \"%s\" [TF]",
                         length, dat->codepage, dest);
              FREE (dest);
            }
          else
            {
              if (!*data->u.eed_0.string)
                data->u.eed_0.length = 0;
              if (data->u.eed_0.length + 5 + dat->byte < dat->size)
                bit_chain_alloc_size (dat, data->u.eed_0.length + 5);
              bit_write_RC (dat, data->u.eed_0.length);
              bit_write_RS_BE (dat, data->u.eed_0.codepage);
              bit_write_TF (dat, (BITCODE_TF)data->u.eed_0.string,
                            data->u.eed_0.length);
              LOG_TRACE ("string: len=" FORMAT_RS " [RC] cp=" FORMAT_RS
                         " [RS_BE] \"%s\" [TF]",
                         data->u.eed_0.length, data->u.eed_0.codepage,
                         data->u.eed_0.string);
            }
        }
        SINCE (R_2007a)
        {
          // from ASCII DWG or JSON, DXF
          if (!data->u.eed_0.is_tu)
            {
              BITCODE_RS length = data->u.eed_0.length;
              BITCODE_TU dest = bit_utf8_to_TU (data->u.eed_0.string, 0);
              if ((length * 2) + 5 + dat->byte < dat->size)
                bit_chain_alloc_size (dat, (length * 2) + 5 + dat->byte);
              bit_write_RS (dat, length);
              for (int j = 0; j < length; j++)
                bit_write_RS (dat, *dest++);
              data->u.eed_0_r2007.length = length;
              LOG_TRACE ("wstring: len=%d [RS] \"%s\" [TU]", (int)length,
                         data->u.eed_0.string);
            }
          else
            {
              BITCODE_RS length = data->u.eed_0_r2007.length;
              BITCODE_RS *s = (BITCODE_RS *)&data->u.eed_0_r2007.string;
              if ((length * 2) + 5 + dat->byte < dat->size)
                bit_chain_alloc_size (dat, (length * 2) + 5);
              bit_write_RS (dat, length);
              for (int j = 0; j < length; j++)
                bit_write_RS (dat, *s++);
#ifdef _WIN32
              LOG_TRACE ("wstring: len=%d [RS] \"" FORMAT_TU "\" [TU]",
                         (int)data->u.eed_0_r2007.length,
                         data->u.eed_0_r2007.string);
#else
              if (DWG_LOGLEVEL >= DWG_LOGLEVEL_TRACE)
                {
                  char *u8 = bit_TU_to_utf8_len (data->u.eed_0_r2007.string,
                                                 data->u.eed_0_r2007.length);
                  LOG_TRACE ("wstring: len=%d [RS] \"%s\" [TU]",
                             (int)data->u.eed_0_r2007.length, u8);
                  FREE (u8);
                }
#endif
            }
        }
      }
      break;
    case 1:
      bit_write_RS (dat, data->u.eed_1.appid_index);
      LOG_TRACE ("appid_index: " FORMAT_RS " [RS]", data->u.eed_1.appid_index);
      break;
    case 2:
      bit_write_RC (dat, data->u.eed_2.close);
      if (data->u.eed_2.close)
        {
          LOG_TRACE ("close: " FORMAT_RC " [RC]", data->u.eed_2.close);
        }
      else
        {
          LOG_TRACE ("open: " FORMAT_RC " [RC]", data->u.eed_2.close);
        }
      break;
    case 3:
      PRE (R_13b1)
      {
        bit_write_RS (dat, (BITCODE_RS)data->u.eed_3.layer);
        LOG_TRACE ("layer: " FORMAT_RS " [RS]",
                   (BITCODE_RS)data->u.eed_3.layer);
      }
      LATER_VERSIONS
      {
        bit_write_RLL (dat, data->u.eed_3.layer);
        LOG_TRACE ("layer: " FORMAT_RLL " [RLL]", data->u.eed_3.layer);
      }
      break;
    case 4:
      if (data->u.eed_4.length + 1 + dat->byte < dat->size)
        bit_chain_alloc_size (dat, data->u.eed_4.length + 1);
      bit_write_RC (dat, data->u.eed_4.length);
      bit_write_TF (dat, (BITCODE_TF)data->u.eed_4.data, data->u.eed_4.length);
      LOG_TRACE ("binary: ");
      LOG_TRACE_TF (data->u.eed_4.data, data->u.eed_4.length);
      break;
    case 5:
      bit_write_RLL_BE (dat, data->u.eed_5.entity);
      LOG_TRACE ("entity: " FORMAT_HV " [RLL_BE]", data->u.eed_5.entity);
      break;
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
      bit_write_RD (dat, data->u.eed_10.point.x);
      bit_write_RD (dat, data->u.eed_10.point.y);
      bit_write_RD (dat, data->u.eed_10.point.z);
      LOG_TRACE ("3dpoint: (%f, %f, %f) [3RD]", data->u.eed_10.point.x,
                 data->u.eed_10.point.y, data->u.eed_10.point.z);
      break;
    case 40:
    case 41:
    case 42:
      bit_write_RD (dat, data->u.eed_40.real);
      LOG_TRACE ("real: %f [RD]", data->u.eed_40.real);
      break;
    case 70:
      bit_write_RS (dat, data->u.eed_70.rs);
      LOG_TRACE ("short: " FORMAT_RS " [RS]", data->u.eed_70.rs);
      break;
    case 71:
      bit_write_RL (dat, data->u.eed_71.rl);
      LOG_TRACE ("long: " FORMAT_RL " [RL]", data->u.eed_71.rl);
      break;
    default:
      dat->byte--;
      LOG_ERROR ("unknown EED code %d", data->code);
    }
  size = bit_position (dat) - pos;
  return (size % 8) ? (int)(size / 8) + 1 : (int)(size / 8);
}

#define dat_flush(orig, dat) bit_copy_chain (orig, dat)

/** Either writes the raw part.
    Only members with size have raw and a handle.
    Otherwise (indxf) defer to dwg_encode_eed_data.
    On does_cross_unicode_datversion skip raw, and recalc the sizes.
    TODO downconvert: strip eed and HAS_EED flag for versions which cannot do
   EED.
 */
static int
dwg_encode_eed (Bit_Chain *restrict dat, Dwg_Object *restrict obj)
{
  // size_t off = obj->address;
  // unsigned dat_size = 0;
  Dwg_Handle *last_handle = NULL;
  Bit_Chain dat1 = { 0 };
  int i, num_eed = obj->tio.object->num_eed;
  BITCODE_BS size = 0;
  BITCODE_RS last_size = 0;
  BITCODE_RS new_size = 0;
  int did_raw = 0;
  int need_recalc = does_cross_unicode_datversion (dat);

  bit_chain_init (&dat1, 1024);
  dat1.from_version = dat->from_version;
  dat1.version = dat->version;
  dat1.opts = dat->opts;

  // Skip DICTIONARY AE3 AcDsRecords/AcDsSchemas 1070 . 2, wrong ACIS version
  if (dat->opts & DWG_OPTS_INDXF && dat->version < R_2007
      && obj->fixedtype == DWG_TYPE_DICTIONARY && num_eed == 1)
    {
      Dwg_Eed *eed = &obj->tio.object->eed[0];
      if (eed->handle.value == 0x12 && eed->data->code == 70
          && eed->data->u.eed_70.rs > 1)
        {
          LOG_TRACE ("skip AcDs DICTIONARY EED to use ACIS ver 2\n");
          num_eed = 0;
        }
    }

  for (i = 0; i < num_eed; i++)
    {
      Dwg_Eed *eed = &obj->tio.object->eed[i];
      if (eed->size) // start of a new EED appid section
        {
          size = eed->size;
          if (eed->raw && !need_recalc)
            {
              did_raw = 1;
              if (dat->version > R_12)
                {
                  bit_write_BS (dat, size);
                  LOG_TRACE ("EED[%d] size: " FORMAT_BS " [BS]", i, size);
                  LOG_POS
                  bit_write_H (dat, &eed->handle);
                  LOG_TRACE ("EED[%d] handle: " FORMAT_H " [H]", i,
                             ARGS_H (eed->handle));
                  LOG_POS
                }
              else
                {
                  bit_write_RS (dat, size);
                  LOG_TRACE ("EED[%d] size: " FORMAT_RS " [RS]", i, size);
                  LOG_POS
                }
              LOG_TRACE ("EED[%d] raw [TF %d]\n", i, size);
              bit_write_TF (dat, eed->raw, size);
              LOG_TRACE_TF (eed->raw, size);
              new_size = 0;
            }
          // indxf
          else if (eed->data)
            {
              did_raw = 0;
              if (new_size) // flush old
                {

// FIXME DXF import of ACAD EED crashes (GH #244)
// on BLOCK_HEADER with 0 . "DesignCenter Data"
#define EED_ALLOWED                                                           \
  !(dat->opts & DWG_OPTS_INDXF) || last_handle->value != 0x12                 \
      || obj->fixedtype != DWG_TYPE_BLOCK_HEADER

                  if (EED_ALLOWED)
                    {
                      eed->size = new_size;
                      if (dat->version <= R_12)
                        {
                          bit_write_RS (dat, new_size);
                          LOG_TRACE ("EED[%d] size: %d [RS]", last_size,
                                     new_size);
                        }
                      else
                        {
                          bit_write_BS (dat, new_size);
                          LOG_TRACE ("EED[%d] size: %d [BS]", last_size,
                                     new_size);
                          LOG_POS;
                          bit_write_H (dat, last_handle);
                          LOG_TRACE ("EED[%d] handle: " FORMAT_H " [H]",
                                     last_size, ARGS_H (*last_handle));
                        }
                      LOG_POS;
                      LOG_TRACE ("flush eed_data %" PRIuSIZE ".%d\n",
                                 dat1.byte, dat1.bit);
                      dat_flush (dat, &dat1);
                    }
                  else
                    {
                      LOG_WARN ("skip EED[%d] handle: " FORMAT_H
                                " [H] for DesignCenter Data",
                                last_size, ARGS_H (*last_handle));
                      LOG_POS;
                      dat1.byte = 0;
                    }
                  new_size = 0;
                }
              new_size = dwg_encode_eed_data (&dat1, eed->data, i);
              LOG_POS;
            }
          last_size = i;
          last_handle = &eed->handle;
        }
      // and if not already written by the previous raw (this has size=0)
      else if (!did_raw && eed->data)
        {
          new_size += dwg_encode_eed_data (&dat1, eed->data, i);
          LOG_POS;
        }
    }
  if (new_size && last_handle) // flush remaining rest
    {
      // FIXME HACK, see above
      if (EED_ALLOWED)
        {
          if (dat->version <= R_12)
            {
              bit_write_RS (dat, new_size);
              LOG_TRACE ("EED[%d] size: %d [RS]", last_size, new_size);
              LOG_POS;
            }
          else
            {
              bit_write_BS (dat, new_size);
              LOG_TRACE ("EED[%d] size: %d [BS]", last_size, new_size);
              LOG_POS;
              bit_write_H (dat, last_handle);
              LOG_TRACE ("EED[%d] handle: " FORMAT_H " [H]", last_size,
                         ARGS_H (*last_handle));
              LOG_POS;
            }
          last_handle = NULL;
        }
      else
        {
          LOG_TRACE ("skip EED[%d] handle: " FORMAT_H
                     " [H] for DesignCenter Data",
                     last_size, ARGS_H (*last_handle));
          LOG_POS;
          dat1.byte = 0;
        }
    }
  if (dat1.byte)
    LOG_TRACE ("flush eed_data %" PRIuSIZE ".%d\n", dat1.byte, dat1.bit);
  dat_flush (dat, &dat1);
  if (dat->version > R_12)
    {
      bit_write_BS (dat, 0);
      if (i)
        LOG_TRACE ("EED[%d] size: 0 [BS] (end)\n", i);
    }
  LOG_TRACE ("num_eed: %d\n", num_eed);
  bit_chain_free (&dat1);
  return 0;
}

/* The first common part of every entity.

   The last common part is common_entity_handle_data.spec
   which is read from the hdl stream.
   See DWG_SUPERTYPE_ENTITY in dwg_encode().
 */
int
dwg_encode_entity (Dwg_Object *restrict obj, Bit_Chain *dat,
                   Bit_Chain *restrict hdl_dat, Bit_Chain *str_dat)
{
  int error = 0;
  Dwg_Object_Entity *_ent = obj->tio.entity;
  Dwg_Object_Entity *_obj = _ent;
  Dwg_Data *dwg = _ent->dwg;

  if (!obj || !dat || !_ent)
    return DWG_ERR_INVALIDDWG;

  hdl_dat->from_version = dat->from_version;
  hdl_dat->version = dat->version;
  hdl_dat->opts = dat->opts;

  SINCE (R_2007a)
  {
    *str_dat = *dat;
  }
  VERSIONS (R_2000, R_2007)
  {
    obj->bitsize_pos = bit_position (dat);
    bit_write_RL (dat, obj->bitsize);
    LOG_TRACE ("bitsize: %u [RL] (@%" PRIuSIZE ".%" PRIuSIZE ")\n",
               obj->bitsize, obj->bitsize_pos / 8, obj->bitsize_pos % 8);
  }
  obj->was_bitsize_set = 0;
  if (obj->bitsize)
    {
      obj->hdlpos = (obj->address * 8) + obj->bitsize;
    }
  SINCE (R_2007a)
  {
    // The handle stream offset, i.e. end of the object, right after
    // the has_strings bit.
    SINCE (R_2010b)
    {
      if (obj->bitsize)
        {
          obj->hdlpos += 8;
          // LOG_HANDLE ("(bitsize: " FORMAT_RL ", ", obj->bitsize);
          LOG_HANDLE ("hdlpos: %" PRIuSIZE "\n", obj->hdlpos);
        }
    }
    // and set the string stream (restricted to size)
    error |= obj_string_stream (dat, obj, str_dat);
  }

  SINCE (R_13b1)
  {
    bit_write_H (dat, &obj->handle);
    LOG_TRACE ("handle: " FORMAT_H " [H 5]", ARGS_H (obj->handle))
    LOG_INSANE (" @%" PRIuSIZE ".%u", dat->byte - obj->address, dat->bit)
    LOG_TRACE ("\n")

    error |= dwg_encode_eed (dat, obj);
  }

  // if (error & (DWG_ERR_INVALIDTYPE|DWG_ERR_VALUEOUTOFBOUNDS))
  //   return error;

  // clang-format off
  #include "common_entity_data.spec"
  // clang-format on

  return error;
}

int
dwg_encode_common_entity_handle_data (Bit_Chain *dat, Bit_Chain *hdl_dat,
                                      Dwg_Object *restrict obj)
{
  Dwg_Object_Entity *_ent;
  // Dwg_Data *dwg = obj->parent;
  Dwg_Object_Entity *_obj;
  BITCODE_BL vcount;
  int error = 0;
  _ent = obj->tio.entity;
  _obj = _ent;

  // deferred from common_entity_data, which writes to the wrong hdl stream
  if (dat->version >= R_2004 && _ent->color.flag & 0x40)
    FIELD_HANDLE (color.handle, 0, 430);

    // clang-format off
  #include "common_entity_handle_data.spec"
  // clang-format on

  return error;
}

#if 0
// converted from TV to possibly longer TFv
void
downgrade_preR13_header_variables (Bit_Chain *dat, Dwg_Data *restrict dwg)
{
  if (dat->from_version >= R_13b1)
    {
      // downgrade TV to TFv
      dwg->header_vars.MENU = REALLOC (dwg->header_vars.MENU, 15);
      dwg->header_vars.DIMBLK_T = REALLOC (dwg->header_vars.DIMBLK_T, 33);
      if (dwg->header.numheader_vars > 114)
        {
          dwg->header_vars.DIMPOST = REALLOC (dwg->header_vars.DIMPOST, 16);
          dwg->header_vars.DIMAPOST = REALLOC (dwg->header_vars.DIMAPOST, 16);
        }
      if (dwg->header.numheader_vars > 129)
        {
          dwg->header_vars.DIMBLK1_T
              = REALLOC (dwg->header_vars.DIMBLK1_T, 33);
          dwg->header_vars.DIMBLK2_T
              = REALLOC (dwg->header_vars.DIMBLK2_T, 33);
          dwg->header_vars.unknown_string
              = REALLOC (dwg->header_vars.unknown_string, 33);
        }
      if (dwg->header.numheader_vars > 160)
        {
          dwg->header_vars.unit1_name
              = REALLOC (dwg->header_vars.unit1_name, 33);
          dwg->header_vars.unit2_name
              = REALLOC (dwg->header_vars.unit2_name, 33);
          dwg->header_vars.unit3_name
              = REALLOC (dwg->header_vars.unit3_name, 33);
          dwg->header_vars.unit4_name
              = REALLOC (dwg->header_vars.unit4_name, 33);
        }
    }
}
#endif

static int
encode_preR13_header_variables (Bit_Chain *dat, Dwg_Data *restrict dwg)
{
  Dwg_Header_Variables *_obj = &dwg->header_vars;
  Dwg_Object *obj = NULL;
  Bit_Chain *hdl_dat = dat;
  int error = 0;

  // clang-format off
// PRE (R_13b1)
// {
//   if (dat->from_version >= R_13b1)
//     downgrade_preR13_header_variables (dat, dwg);
// }
  #include "header_variables_r11.spec"
  // clang-format on

  return error;
}

void
dwg_encode_handleref (Bit_Chain *hdl_dat, Dwg_Object *restrict obj,
                      Dwg_Data *restrict dwg, Dwg_Object_Ref *restrict ref)
{
  // this function should receive a Object_Ref without an abs_ref, calculate it
  // and return a Dwg_Handle. This should be a higher level function not sure
  // if the prototype is correct
  assert (obj);
}

/**
 * code:
 *  TYPEDOBJHANDLE:
 *   2 Soft owner
 *   3 Hard owner
 *   4 Soft pointer
 *   5 Hard pointer
 *  OFFSETOBJHANDLE for soft owners or pointers:
 *   6 ref + 1
 *   8 ref - 1
 *   a ref + offset
 *   c ref - offset
 */
void
dwg_encode_handleref_with_code (Bit_Chain *hdl_dat, Dwg_Object *restrict obj,
                                Dwg_Data *restrict dwg,
                                Dwg_Object_Ref *restrict ref,
                                unsigned int code)
{
  // XXX fixme. create the handle, then check the code. allow relative handle
  // soft codes.
  dwg_encode_handleref (hdl_dat, obj, dwg, ref);
  if (ref->absolute_ref == 0 && ref->handleref.code != code)
    {
      /*
       * With TYPEDOBJHANDLE 2-5 the code indicates the type of ownership.
       * With OFFSETOBJHANDLE >5 the handle is stored as an offset from some
       * other handle.
       */
      switch (ref->handleref.code)
        {
        case 0x06:
          ref->absolute_ref = (obj->handle.value + 1);
          break;
        case 0x08:
          ref->absolute_ref = (obj->handle.value - 1);
          break;
        case 0x0A:
          ref->absolute_ref = (obj->handle.value + ref->handleref.value);
          break;
        case 0x0C:
          ref->absolute_ref = (obj->handle.value - ref->handleref.value);
          break;
        case 2:
        case 3:
        case 4:
        case 5:
          ref->absolute_ref = ref->handleref.value;
          break;
        case 0: // ignore (ANYCODE)
          ref->absolute_ref = ref->handleref.value;
          break;
        default:
          LOG_WARN ("Invalid handle pointer code %d", ref->handleref.code);
          break;
        }
    }
}

/* The first common part of every object.

   There is no COMMON_ENTITY_DATA for objects, handles are deferred and flushed
   later. See DWG_SUPERTYPE_OBJECT in dwg_encode().
*/
int
dwg_encode_object (Dwg_Object *restrict obj, Bit_Chain *dat,
                   Bit_Chain *restrict hdl_dat, Bit_Chain *str_dat)
{
  int error = 0;
  // BITCODE_BL vcount;

  hdl_dat->from_version = dat->from_version;
  hdl_dat->version = dat->version;
  hdl_dat->opts = dat->opts;

  {
    Dwg_Object *_obj = obj;
    VERSIONS (R_2000, R_2007)
    {
      obj->bitsize_pos = bit_position (dat);
      FIELD_RL (bitsize, 0);
    }
    obj->was_bitsize_set = 0;
    if (obj->bitsize)
      // the handle stream offset
      obj->hdlpos = bit_position (dat) + obj->bitsize;
    SINCE (R_2007a)
    {
      obj_string_stream (dat, obj, str_dat);
    }
    if (!_obj || !obj->tio.object)
      return DWG_ERR_INVALIDDWG;

    SINCE (R_13b1)
    {
      bit_write_H (dat, &obj->handle);
      LOG_TRACE ("handle: " FORMAT_H " [H 5]\n", ARGS_H (obj->handle));
      error |= dwg_encode_eed (dat, obj);
    }
    VERSIONS (R_13b1, R_14)
    {
      obj->bitsize_pos = bit_position (dat);
      FIELD_RL (bitsize, 0);
    }
  }

  SINCE (R_13b1)
  {
    Dwg_Object_Object *_obj = obj->tio.object;
    FIELD_BL (num_reactors, 0);
    SINCE (R_2004a)
    {
      FIELD_B (is_xdic_missing, 0);
    }
    SINCE (R_2013b)
    {
      FIELD_B (has_ds_data, 0);
    } // AcDs DATA
  }
  return error;
}

AFL_GCC_TOOBIG
static int
dwg_encode_header_variables (Bit_Chain *dat, Bit_Chain *hdl_dat,
                             Bit_Chain *str_dat, Dwg_Data *restrict dwg)
{
  Dwg_Header_Variables *_obj = &dwg->header_vars;
  Dwg_Object *obj = NULL;
  Dwg_Version_Type old_from = dat->from_version;
  BITCODE_H last_hdl;
  BITCODE_RLL seed = 0;

  if (!_obj->HANDSEED) // minimal or broken DXF
    {
      dwg->opts |= DWG_OPTS_MINIMAL;
      dat->from_version = (Dwg_Version_Type)((int)dat->version - 1);
      LOG_TRACE ("encode from minimal DXF\n");
      _obj->HANDSEED = (Dwg_Object_Ref *)CALLOC (1, sizeof (Dwg_Object_Ref));
    }
  // check the object map for the next available handle
  last_hdl = dwg->num_object_refs ? dwg->object_ref[dwg->num_object_refs - 1]
                                  : NULL;
  if (last_hdl)
    {
      // find the largest handle
      seed = last_hdl->absolute_ref;
      LOG_TRACE ("compute HANDSEED " FORMAT_HV " ", seed);
      for (unsigned i = 0; i < dwg->num_object_refs; i++)
        {
          Dwg_Object_Ref *ref = dwg->object_ref[i];
          if (ref->absolute_ref > seed)
            seed = ref->absolute_ref;
        }
      _obj->HANDSEED->absolute_ref = seed + 1;
      LOG_TRACE ("-> " FORMAT_HV "\n", seed);
    }
  else
    _obj->HANDSEED->absolute_ref = 0x72E;

    // clang-format off
  #include "header_variables.spec"
  // clang-format on

  dat->from_version = old_from;
  return 0;
}
AFL_GCC_POP

int
dwg_encode_xdata (Bit_Chain *restrict dat, Dwg_Object_XRECORD *restrict _obj,
                  unsigned xdata_size)
{
  Dwg_Resbuf *rbuf = _obj->xdata;
  enum RESBUF_VALUE_TYPE type;
  int error = 0;
  int i;
  unsigned j = 0;
  size_t start = dat->byte, end = start + xdata_size;
  Dwg_Data *dwg = _obj->parent->dwg;
  Dwg_Object *obj = &dwg->object[_obj->parent->objid];

  // BITCODE_BL num_xdata = _obj->num_xdata;
  if ((dat->opts & DWG_OPTS_INJSON) && xdata_size > 0x7FFF)
    {
      LOG_WARN ("xdata_size %u too large, truncating to 0x7FFF", xdata_size);
      xdata_size = 0x7FFF;
      _obj->xdata_size = xdata_size;
    }

  if (dat->opts
      & DWG_OPTS_IN) // loosen the overflow checks on dxf/json imports
    end += xdata_size;

  while (rbuf)
    {
      bit_write_RS (dat, rbuf->type);
      LOG_INSANE ("xdata[%u] type: " FORMAT_RS " [RS] @%" PRIuSIZE ".%u\n", j,
                  rbuf->type, dat->byte - obj->address, dat->bit)
      type = dwg_resbuf_value_type (rbuf->type);
      switch (type)
        {
        case DWG_VT_STRING:
          PRE (R_2007a)
          {
            if (dat->byte + 3 + rbuf->value.str.size > end)
              break;
            // from TU DWG only
            if (rbuf->value.str.size && rbuf->value.str.is_tu)
              {
                BITCODE_TV news = bit_embed_TU_size (rbuf->value.str.u.wdata,
                                                     rbuf->value.str.size);
                unsigned len = (unsigned)strlen (news);
                bit_write_RS (dat, len & 0xFFFF);
                bit_write_RC (dat, rbuf->value.str.codepage);
                if (rbuf->value.str.u.data)
                  bit_write_TF (dat, (BITCODE_TF)news, len);
                else
                  bit_write_TF (dat, (BITCODE_TF) "", 0);
                LOG_TRACE ("xdata[%u]: \"%s\" [TF %u %d]", j,
                           rbuf->value.str.u.data, len, rbuf->type);
                FREE (news);
              }
            else
              {
                // do we need to convert UTF-8 to codepage?
                if (rbuf->value.str.size > 0 && dat->opts & DWG_OPTS_INJSON)
                  {
                    BITCODE_RS destlen = rbuf->value.str.size * 2;
                    char *dest = (char *)MALLOC (destlen);
                    while (!bit_utf8_to_TV (
                        dest, (BITCODE_TF)rbuf->value.str.u.data, destlen,
                        rbuf->value.str.size, 0, rbuf->value.str.codepage))
                      {
                        destlen *= 2;
                        dest = (char *)REALLOC (dest, destlen);
                      }
                    destlen = (BITCODE_RS)strlen (dest);
                    bit_write_RS (dat, destlen);
                    bit_write_RC (dat, rbuf->value.str.codepage);
                    if (dest)
                      bit_write_TF (dat, (BITCODE_TF)dest, destlen);
                    else
                      bit_write_TF (dat, (BITCODE_TF) "", 0);
                    FREE (dest);
                  }
                else
                  {
                    bit_write_RS (dat, rbuf->value.str.size);
                    bit_write_RC (dat, rbuf->value.str.codepage);
                    if (rbuf->value.str.u.data)
                      bit_write_TF (dat, (BITCODE_TF)rbuf->value.str.u.data,
                                    rbuf->value.str.size);
                    else
                      bit_write_TF (dat, (BITCODE_TF) "", 0);
                  }
                LOG_TRACE ("xdata[%u]: \"%s\" [TF %d %d]", j,
                           rbuf->value.str.u.data, rbuf->value.str.size,
                           rbuf->type);
              }
            LOG_POS;
          }
          LATER_VERSIONS
          {
            if (dat->byte + 2 + (2 * rbuf->value.str.size) > end)
              break;
            if (rbuf->value.str.size && !rbuf->value.str.is_tu)
              {
                // TODO: same len when converted to TU? normally yes
                BITCODE_TU news = bit_utf8_to_TU (rbuf->value.str.u.data, 0);
                bit_write_RS (dat, rbuf->value.str.size);
                for (i = 0; i < rbuf->value.str.size; i++)
                  bit_write_RS (dat, news[i]);
                LOG_TRACE_TU ("xdata", news, rbuf->type);
                FREE (news);
              }
            else
              {
                bit_write_RS (dat, rbuf->value.str.size);
                for (i = 0; i < rbuf->value.str.size; i++)
                  bit_write_RS (dat, rbuf->value.str.u.wdata[i]);
                LOG_TRACE_TU ("xdata", rbuf->value.str.u.wdata, rbuf->type);
              }
            LOG_POS;
          }
          break;
        case DWG_VT_REAL:
          if (dat->byte + 8 > end)
            break;
          bit_write_RD (dat, rbuf->value.dbl);
          LOG_TRACE ("xdata[%u]: %f [RD %d]", j, rbuf->value.dbl, rbuf->type);
          LOG_POS;
          break;
        case DWG_VT_BOOL:
        case DWG_VT_INT8:
          bit_write_RC (dat, rbuf->value.i8);
          LOG_TRACE ("xdata[%u]: %d [RC %d]", j, (int)rbuf->value.i8,
                     rbuf->type);
          LOG_POS;
          break;
        case DWG_VT_INT16:
          if (dat->byte + 2 > end)
            break;
          bit_write_RS (dat, rbuf->value.i16);
          LOG_TRACE ("xdata[%u]: " FORMAT_RLd " [RSd %d]", j, rbuf->value.i16,
                     rbuf->type);
          LOG_POS;
          break;
        case DWG_VT_INT32:
          if (dat->byte + 4 > end)
            break;
          bit_write_RL (dat, rbuf->value.i32);
          LOG_TRACE ("xdata[%d]: " FORMAT_RLd " [RLd %d]", j, rbuf->value.i32,
                     rbuf->type);
          LOG_POS;
          break;
        case DWG_VT_INT64:
          if (dat->byte + 8 > end)
            break;
          bit_write_RLL (dat, rbuf->value.i64);
          LOG_TRACE ("xdata[%u]: " FORMAT_RLLd " [RLLd %d]", j,
                     rbuf->value.i64, rbuf->type);
          LOG_POS;
          break;
        case DWG_VT_POINT3D:
          if (dat->byte + 24 > end)
            break;
          bit_write_RD (dat, rbuf->value.pt[0]);
          bit_write_RD (dat, rbuf->value.pt[1]);
          bit_write_RD (dat, rbuf->value.pt[2]);
          LOG_TRACE ("xdata[%u]: (%f,%f,%f) [3RD %d]", j, rbuf->value.pt[0],
                     rbuf->value.pt[1], rbuf->value.pt[2], rbuf->type);
          LOG_POS;
          break;
        case DWG_VT_BINARY:
          if (dat->byte + rbuf->value.str.size > end)
            break;
          // 128 is a tradeoff, which of both data is wrong or right.
          if (!rbuf->value.str.u.data && rbuf->value.str.size > 128)
            {
              LOG_ERROR ("Empty xdata string. Write size %u as 0",
                         (unsigned)rbuf->value.str.size);
              bit_write_RC (dat, 0);
            }
          else
            bit_write_RC (dat, rbuf->value.str.size);
          if (rbuf->value.str.size)
            bit_write_TF (dat, (BITCODE_TF)rbuf->value.str.u.data,
                          rbuf->value.str.size);
          LOG_TRACE ("xdata[%u]: [TF %d %d] ", j, rbuf->value.str.size,
                     rbuf->type);
          LOG_TRACE_TF (rbuf->value.str.u.data, rbuf->value.str.size);
          LOG_POS;
          break;
        case DWG_VT_HANDLE:
        case DWG_VT_OBJECTID:
          if (dat->byte + 8 > end)
            break;
          bit_write_RLL (dat, rbuf->value.absref);
          LOG_TRACE ("xdata[%u]: " FORMAT_HV " [H %d]", j, rbuf->value.absref,
                     rbuf->type);
          LOG_POS;
          break;
        case DWG_VT_INVALID:
        default:
          LOG_ERROR ("Invalid group code in xdata: %d", rbuf->type);
          error = DWG_ERR_INVALIDEED;
          break;
        }
      rbuf = rbuf->nextrb;
      j++;
      if (j >= _obj->num_xdata)
        break;
      if (dat->byte >= end)
        {
          LOG_WARN ("xdata overflow %u", xdata_size);
          break;
        }
    }
  if (_obj->xdata_size != ((dat->byte - start) & 0xFFFFFFFF))
    {
      if (dat->opts & DWG_OPTS_IN) // imprecise xdata_size: calculate
        {
          _obj->xdata_size = (dat->byte - start) & 0xFFFFFFFF;
          LOG_TRACE ("-xdata_size: " FORMAT_BL " (calculated)\n",
                     _obj->xdata_size);
          return error;
        }
      else
        {
          LOG_WARN ("xdata Written %" PRIuSIZE ", expected " FORMAT_BL,
                    dat->byte - start, _obj->xdata_size);
          _obj->xdata_size = (dat->byte - start) & 0xFFFFFFFF;
          return error ? error : 1;
        }
    }
  return 0;
}

Dwg_Object *
find_prev_entity (Dwg_Object *obj)
{
  Dwg_Data *dwg = obj->parent;
  if (obj->supertype != DWG_SUPERTYPE_ENTITY)
    return NULL;
  if (dwg->prev_entity_index < obj->index
      && dwg->prev_entity_index < dwg->num_objects)
    {
      Dwg_Object *prev = &dwg->object[dwg->prev_entity_index];
      if (prev->supertype == DWG_SUPERTYPE_ENTITY
          && prev->tio.entity->entmode == obj->tio.entity->entmode)
        {
          if (prev->fixedtype == DWG_TYPE_SEQEND
              || prev->fixedtype == DWG_TYPE_ENDBLK)
            return NULL;
          else
            return prev;
        }
    }
  /*
  for (BITCODE_BL i = obj->index - 1; i > 0; i--)
    {
      Dwg_Object *prev = &dwg->object[i];
      if (prev->supertype == DWG_SUPERTYPE_ENTITY
          && prev->tio.entity->entmode == obj->tio.entity->entmode)
        {
          if (prev->fixedtype == DWG_TYPE_SEQEND
              || prev->fixedtype == DWG_TYPE_ENDBLK)
            return NULL;
          else
            return prev;
        }
    }
  */
  return NULL;
}

// return a relative softptr (4 handle) to the prev_ref handle, relative to
// obj.
static Dwg_Object_Ref *
dwg_link_prev (Dwg_Object_Ref *restrict prev_ref, Dwg_Object *restrict obj)
{
  Dwg_Object *prev;
  Dwg_Data *dwg = obj ? obj->parent : NULL;
  if (!prev_ref)
    return dwg_add_handleref (dwg, 4, 0, NULL);
  if (!obj)
    return NULL;
  prev = dwg_ref_object (dwg, prev_ref);
  if (!prev || prev->supertype != DWG_SUPERTYPE_ENTITY)
    return NULL;
  if (obj && obj->supertype == DWG_SUPERTYPE_ENTITY)
    obj->tio.entity->nolinks = 1;
  return dwg_add_handleref (dwg, 4, prev->handle.value, obj);
}

// return a relative softptr (4 handle) to the next_ref handle, relative to
// obj. sets obj nolinks to 1 or 0.
static Dwg_Object_Ref *
dwg_link_next (Dwg_Object_Ref *restrict next_ref, Dwg_Object *restrict obj)
{
  Dwg_Object *next;
  Dwg_Object_Ref *prev_ref;
  Dwg_Data *dwg = obj ? obj->parent : NULL;
  if (!next_ref)
    return dwg_add_handleref (dwg, 4, 0, NULL);
  if (!obj || obj->supertype != DWG_SUPERTYPE_ENTITY)
    return NULL;
  next = dwg_ref_object (dwg, next_ref);
  if (!next || next->supertype != DWG_SUPERTYPE_ENTITY)
    return NULL;
  if (next->index == obj->index + 1)
    {
      prev_ref = obj->tio.entity->prev_entity;
      // check if nolinks can be set
      if (prev_ref && prev_ref->handleref.code == 8
          && prev_ref->handleref.value == 0)
        {
          obj->tio.entity->nolinks = 1;
          LOG_TRACE ("%s.nolinks = 1\n", obj->name);
          return NULL;
        }
    }
  obj->tio.entity->nolinks = 0;
  return dwg_add_handleref (dwg, 4, next->handle.value, obj);
}

// Also exported to in_json and dwg_api, where we have no target version still.
// To set to linked list of children in POLYLINE_*/*INSERT
// similar to dwg_fixup_BLOCKS_entities()
void
in_postprocess_SEQEND (Dwg_Object *restrict obj, BITCODE_BL num_owned,
                       BITCODE_H *owned)
{
  Dwg_Data *dwg;
  // Dwg_Entity_SEQEND *o = obj->tio.entity->tio.SEQEND;
  Dwg_Object *owner;
  Dwg_Entity_POLYLINE_2D *ow;
  const char *owhdls; // the name of the H*
  const char *firstfield;
  const char *lastfield;

  if (!obj || !obj->parent || obj->fixedtype != DWG_TYPE_SEQEND
      || !obj->tio.entity)
    {
      LOG_ERROR ("wrong in_postprocess_SEQEND obj");
      return;
    }
  dwg = obj->parent;
  loglevel = dwg->opts & DWG_OPTS_LOGLEVEL;
  LOG_TRACE ("in_postprocess_SEQEND (%u):\n", (unsigned)num_owned);
  owner = dwg_ref_object (dwg, obj->tio.entity->ownerhandle);
  // r12 and earlier: search for owner backwards
  if (dwg->header.from_version < R_13b1 && !owner
      && !obj->tio.entity->ownerhandle)
    {
      for (BITCODE_BL i = obj->index - 1; i > 0; i--)
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
  if (!owner || !owner->tio.entity || !owner->name)
    {
      if (obj->tio.entity->ownerhandle)
        LOG_WARN ("Missing owner (" FORMAT_HV ") from " FORMAT_REF " [H 330]",
                  obj->handle.value, ARGS_REF (obj->tio.entity->ownerhandle))
      else
        LOG_WARN ("Missing owner (" FORMAT_HV ")", obj->handle.value)
      return;
    }

  obj->tio.entity->ownerhandle->obj = NULL;
  owhdls = memBEGINc (owner->name, "POLYLINE_") ? "vertex" : "attribs";
  // not the same layout for all possible owners
  ow = owner->tio.entity->tio.POLYLINE_2D;
  if (!num_owned || !owned)
    {
      dwg_dynapi_entity_value (ow, owner->name, "num_owned", &num_owned, 0);
      dwg_dynapi_entity_value (ow, owner->name, owhdls, &owned, 0);
    }
  if (!num_owned)
    return;

  if (memBEGINc (owner->name, "POLYLINE_"))
    {
      firstfield = "first_vertex";
      lastfield = "last_vertex";
    }
  else
    {
      firstfield = "first_attrib";
      lastfield = "last_attrib";
    }
  // store all these fields, or just the ones for the requested version?
  // this is also called from IN_JSON and DWG_API which has not target version
  // yet
  if ((dwg->header.from_version > R_2000 || dwg->opts & DWG_OPTS_INDXF)
      && owned)
    {
      Dwg_Object *owned_obj;
      Dwg_Object_Entity *ent;
      Dwg_Object_Ref *hdl;
      // need to turn code 3 into absolute 4.
      if (owned[0])
        {
          hdl = dwg_add_handleref (dwg, 4, owned[0]->handleref.value, NULL);
          dwg_dynapi_entity_set_value (ow, owner->name, firstfield, &hdl, 0);
          LOG_TRACE ("%s[0].%s = " FORMAT_REF "[H 0]\n", owner->name,
                     firstfield, ARGS_REF (hdl));
        }
      if (owned[num_owned - 1])
        {
          hdl = dwg_add_handleref (
              dwg, 4, owned[num_owned - 1]->handleref.value, NULL);
          dwg_dynapi_entity_set_value (ow, owner->name, lastfield, &hdl, 0);
          LOG_TRACE ("%s[%u].%s = " FORMAT_REF "[H 0]\n", owner->name,
                     num_owned - 1, lastfield, ARGS_REF (hdl));
        }
      // link the list, because the children have entmode, different to the
      // owner.
      owned_obj = dwg_ref_object (dwg, owned[0]);
      if (!owned_obj || owned_obj->supertype != DWG_SUPERTYPE_ENTITY)
        return;
      ent = owned_obj->tio.entity;
      ent->prev_entity = dwg_link_prev (NULL, owned_obj);
      if (ent->prev_entity)
        {
          LOG_TRACE ("%s[0].prev_entity = " FORMAT_REF "[H 0]\n",
                     owned_obj->name, ARGS_REF (ent->prev_entity));
        }
      else
        ent->nolinks = 0;
      ent->next_entity
          = dwg_link_next (num_owned > 1 ? owned[1] : NULL, owned_obj);
      if (ent->next_entity)
        LOG_TRACE ("%s[0].next_entity = " FORMAT_REF "[H 0]\n",
                   owned_obj->name, ARGS_REF (ent->next_entity))
      else
        ent->nolinks = 0;
      if (ent->nolinks == 1 && num_owned == 1)
        {
          ent->nolinks = 0;
          LOG_TRACE ("%s[0].nolinks = 0\n", owned_obj->name);
        }
      for (unsigned i = 1; i < num_owned; i++)
        {
          owned_obj = dwg_ref_object (dwg, owned[i]);
          if (!owned_obj || owned_obj->supertype != DWG_SUPERTYPE_ENTITY)
            continue;
          ent = owned_obj->tio.entity;
          ent->prev_entity = dwg_link_prev (owned[i - 1], owned_obj);
          if (ent->prev_entity)
            {
              LOG_TRACE ("%s[%u].prev_entity = " FORMAT_REF "[H 0]\n",
                         owned_obj->name, i, ARGS_REF (ent->prev_entity));
            }
          if (i == num_owned - 1) // the last
            {
              if (ent->nolinks == 1)
                {
                  ent->nolinks = 0;
                  LOG_TRACE ("%s[%u].nolinks = 0\n", owned_obj->name, i);
                }
            }
          else
            {
              ent->next_entity = dwg_link_next (owned[i + 1], owned_obj);
              if (ent->next_entity)
                LOG_TRACE ("%s[%u].next_entity = " FORMAT_REF "[H 0]\n",
                           owned_obj->name, i, ARGS_REF (ent->next_entity));
            }
        }
    }
  else if ((dwg->header.from_version <= R_2000 || dwg->opts & DWG_OPTS_INDXF)
           && !owned)
    {
      BITCODE_H first, last, ref;
      unsigned i = 0;
      owned = (BITCODE_H *)CALLOC (1, sizeof (BITCODE_H));
      dwg_dynapi_entity_value (ow, owner->name, firstfield, &first, 0);
      dwg_dynapi_entity_value (ow, owner->name, lastfield, &last, 0);
      ref = first;
      if (!first || !last || !last->absolute_ref)
        {
          num_owned = 0;
          owned[0] = first;
        }
      else if (first->absolute_ref == last->absolute_ref)
        {
          num_owned = 1;
          owned[0] = first;
        }
      else
        while (ref && ref->absolute_ref
               && ref->absolute_ref != last->absolute_ref)
          {
            Dwg_Object *ref_obj = dwg_ref_object (dwg, ref);
            if (!ref_obj || ref_obj->supertype != DWG_SUPERTYPE_ENTITY
                || !ref_obj->tio.entity)
              continue;
            owned[i] = ref;
            if (ref)
              LOG_TRACE ("%s.%s[%u] = " FORMAT_REF "[H 0]\n", owner->name,
                         owhdls, i, ARGS_REF (ref));
            ref = ref_obj->tio.entity->next_entity;
            i++;
            if (i > 1)
              {
                num_owned = i;
                owned = (BITCODE_H *)REALLOC (owned, i * sizeof (BITCODE_H));
              }
          }
      dwg_dynapi_entity_set_value (ow, owner->name, "num_owned", &num_owned,
                                   0);
      dwg_dynapi_entity_set_value (ow, owner->name, owhdls, &owned, 0);
    }
}

void
in_postprocess_handles (Dwg_Object *restrict obj)
{
  Dwg_Data *dwg = obj->parent;
#ifndef __AFL_COMPILER
  const char *name = obj->name;
#endif
  int is_entity = obj->supertype == DWG_SUPERTYPE_ENTITY;

  // common_entity_handle_data:
  // set is_xdic_missing and xdicobjhandle if <2004
  if (is_entity ? !obj->tio.entity->xdicobjhandle
                : !obj->tio.object->xdicobjhandle)
    {
      if (dwg->header.version >= R_2004)
        {
          if (is_entity)
            obj->tio.entity->is_xdic_missing = 1;
          else
            obj->tio.object->is_xdic_missing = 1;
        }
      else if (dwg->header.version >= R_13b1 && !is_entity)
        obj->tio.object->xdicobjhandle = dwg_add_handleref (dwg, 3, 0, obj);
      else if (dwg->header.version >= R_13b1 && is_entity)
        obj->tio.entity->xdicobjhandle = dwg_add_handleref (dwg, 3, 0, obj);
    }

  if (is_entity)
    {
      Dwg_Object_Entity *ent = obj->tio.entity;
      if (dwg->header.version >= R_13b1 && dwg->header.version <= R_14)
        {
          if (ent->ltype_flags < 3)
            ent->isbylayerlt = 1;
        }
      if (dwg->header.version >= R_13b1 && dwg->header.version <= R_2000
          && obj->type != DWG_TYPE_SEQEND && obj->type != DWG_TYPE_ENDBLK)
        {
          Dwg_Object *prev = find_prev_entity (obj);
          ent->next_entity = NULL; // temp.
          if (prev)
            {
              // if (prev->tio.entity->prev_entity)
              //  prev->tio.entity->nolinks = 0;
              if (prev->index + 1 != obj->index)
                {
                  prev->tio.entity->nolinks = 0;
                  prev->tio.entity->next_entity
                      = dwg_add_handleref (dwg, 4, obj->handle.value, prev);
                  LOG_TRACE ("prev %s(" FORMAT_HV ").next_entity = " FORMAT_REF
                             "\n",
                             prev->name, prev->handle.value,
                             ARGS_REF (prev->tio.entity->next_entity));
                  ent->nolinks = 0;
                  ent->prev_entity
                      = dwg_add_handleref (dwg, 4, prev->handle.value, obj);
                  LOG_TRACE ("%s.prev_entity = " FORMAT_REF "\n", name,
                             ARGS_REF (ent->prev_entity));
                }
              else
                {
                  LOG_TRACE ("%s.prev_entity = NULL HANDLE 4\n", name);
                  ent->prev_entity = NULL;
                  ent->nolinks = 1;
                }
            }
          else if (obj->type == DWG_TYPE_BLOCK)
            {
              ent->nolinks = 0;
              ent->prev_entity = dwg_add_handleref (dwg, 4, 0, NULL);
              ent->next_entity = dwg_add_handleref (dwg, 4, 0, NULL);
              LOG_TRACE ("%s.prev_entity = next_entity = " FORMAT_REF "\n",
                         name, ARGS_REF (ent->prev_entity));
            }
          else
            {
              LOG_TRACE ("%s.prev_entity = NULL HANDLE 4\n", name);
              ent->prev_entity = NULL;
              ent->nolinks = 1;
            }
        }
      else if (obj->type != DWG_TYPE_SEQEND && obj->type != DWG_TYPE_ENDBLK)
        ent->nolinks = 1;
    }
}

// when updating TEXT/ATTDEF/ATTRIB to R_2000
void
dwg_set_dataflags (Dwg_Object *obj)
{
  if (obj->fixedtype == DWG_TYPE_TEXT)
    {
      Dwg_Entity_TEXT *_obj = obj->tio.entity->tio.TEXT;

#define _SET_DATAFLAGS                                                        \
  _obj->dataflags = 0xff;                                                     \
  if (_obj->elevation != 0.0)                                                 \
    _obj->dataflags &= ~1;                                                    \
  if (_obj->alignment_pt.x != _obj->ins_pt.x                                  \
      || _obj->alignment_pt.y != _obj->ins_pt.y)                              \
    _obj->dataflags &= ~2;                                                    \
  if (_obj->oblique_angle != 0.0)                                             \
    _obj->dataflags &= ~4;                                                    \
  if (_obj->rotation != 0.0)                                                  \
    _obj->dataflags &= ~8;                                                    \
  if (_obj->width_factor != 1.0)                                              \
    _obj->dataflags &= ~0x10;                                                 \
  if (_obj->generation != 0)                                                  \
    _obj->dataflags &= ~0x20;                                                 \
  if (_obj->horiz_alignment != 0)                                             \
    _obj->dataflags &= ~0x40;                                                 \
  if (_obj->vert_alignment != 0)                                              \
    _obj->dataflags &= ~0x80

      _SET_DATAFLAGS;
    }
  else if (obj->fixedtype == DWG_TYPE_ATTRIB)
    {
      Dwg_Entity_ATTRIB *_obj = obj->tio.entity->tio.ATTRIB;
      _SET_DATAFLAGS;
    }
  else if (obj->fixedtype == DWG_TYPE_ATTDEF)
    {
      Dwg_Entity_ATTDEF *_obj = obj->tio.entity->tio.ATTDEF;
      _SET_DATAFLAGS;
    }
#undef _SET_DATAFLAGS
}

#if 0
// from >2000 to 2000-r13, no relative refs,. FIXME: except prev,next links.
// only with older objects, not class objects.
static void
downconvert_relative_handle (BITCODE_H ref, Dwg_Object *restrict obj)
{
  if (obj && obj->type >= 500)
    return;
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

// from >2007 to 2000
void
downconvert_TABLESTYLE (Dwg_Object *restrict obj)
{
  // Dwg_Data *dwg = obj->parent;
  Dwg_Object_TABLESTYLE *_obj
      = obj->tio.object ? obj->tio.object->tio.TABLESTYLE : NULL;
  if (!obj || obj->fixedtype != DWG_TYPE_TABLESTYLE)
    {
      LOG_ERROR ("Invalid type %u for downconvert_TABLESTYLE",
                 obj ? obj->fixedtype : 0);
      return;
    }
  LOG_WARN ("Downconverting TABLESTYLE with loosing information")
  if (!_obj->num_rowstyles)
    {
      _obj->num_rowstyles = 3;
      _obj->rowstyles = (Dwg_TABLESTYLE_rowstyles *)CALLOC (
          3, sizeof (Dwg_TABLESTYLE_rowstyles));
    }
  // 0: data, 1: title, 2: header
  // assert (strEQc (_obj->sty.name, "Table"));
  LOG_TRACE ("TABLESTYLE.sty.name: %s\n", _obj->sty.name);
  _obj->rowstyles[0].text_style
      = _obj->sty.cellstyle.content_format.text_style;
  _obj->rowstyles[0].text_height
      = _obj->sty.cellstyle.content_format.text_height;
  _obj->rowstyles[0].text_alignment
      = _obj->sty.cellstyle.content_format.cell_alignment;
  _obj->rowstyles[0].text_color
      = _obj->sty.cellstyle.content_format.content_color;
  _obj->rowstyles[0].fill_color = _obj->sty.cellstyle.bg_color;
  _obj->rowstyles[0].has_bgcolor = _obj->sty.cellstyle.bg_color.method != 0xc8;
  _obj->rowstyles[0].data_type
      = _obj->sty.cellstyle.content_format.value_data_type;
  _obj->rowstyles[0].unit_type
      = _obj->sty.cellstyle.content_format.value_unit_type;
#if 0
  {
    size_t destlen;
    char *u8 = bit_convert_TU ((BITCODE_TU)_obj->sty.cellstyle
                               .content_format.value_format_string);
    if (u8 && (destlen = strlen (u8)))
      {
        char *dest = MALLOC (destlen + 1);
        _obj->rowstyles[0].format_string = (BITCODE_TU)bit_utf8_to_TV (
            dest, (unsigned char *)u8, destlen, destlen - 1, 0,
            dwg->header.codepage);
      }
    FREE (u8);
  }
#endif
  if (!_obj->rowstyles[0].num_borders)
    {
      _obj->rowstyles[0].num_borders = 6;
      _obj->rowstyles[0].borders = (Dwg_TABLESTYLE_border *)CALLOC (
          6, sizeof (Dwg_TABLESTYLE_border));
    }
  // borders/grid: top, horizontal inside, bottom, left, vertical inside, right
  if (_obj->sty.cellstyle.borders)
    {
      for (unsigned i = 0; i < 6; i++)
        {
          if (i >= _obj->sty.cellstyle.num_borders)
            break;
          _obj->rowstyles[0].borders[i].linewt
              = _obj->sty.cellstyle.borders[i].linewt;
          _obj->rowstyles[0].borders[i].visible
              = _obj->sty.cellstyle.borders[i].visible;
          _obj->rowstyles[0].borders[i].color
              = _obj->sty.cellstyle.borders[i].color;
        }
      FREE (_obj->sty.cellstyle.borders);
      _obj->sty.cellstyle.borders = NULL;
      _obj->sty.cellstyle.num_borders = 0;
    }
  // title
  if (!_obj->rowstyles[1].num_borders)
    {
      _obj->rowstyles[1].num_borders = 6;
      _obj->rowstyles[1].borders = (Dwg_TABLESTYLE_border *)CALLOC (
          6, sizeof (Dwg_TABLESTYLE_border));
      _obj->rowstyles[1].text_color.method = 0xc1;
      _obj->rowstyles[1].fill_color.method = 0xc1;
    }
  if (_obj->ovr.type == 1)
    {
      // assert (strEQc (_obj->ovr.name, "_TITLE"));
      LOG_TRACE ("TABLESTYLE.ovr.name: %s\n", _obj->ovr.name);
      _obj->rowstyles[0].text_style
          = _obj->ovr.cellstyle.content_format.text_style;
      _obj->rowstyles[0].text_height
          = _obj->ovr.cellstyle.content_format.text_height;
      _obj->rowstyles[0].text_alignment
          = _obj->ovr.cellstyle.content_format.cell_alignment;
      _obj->rowstyles[0].text_color
          = _obj->ovr.cellstyle.content_format.content_color;
      _obj->rowstyles[0].fill_color = _obj->ovr.cellstyle.bg_color;
      if (_obj->ovr.cellstyle.borders)
        {
          for (unsigned i = 0; i < 6; i++)
            {
              if (i >= _obj->ovr.cellstyle.num_borders)
                break;
              _obj->rowstyles[0].borders[i].linewt
                  = _obj->ovr.cellstyle.borders[i].linewt;
              _obj->rowstyles[0].borders[i].visible
                  = _obj->ovr.cellstyle.borders[i].visible;
              _obj->rowstyles[0].borders[i].color
                  = _obj->ovr.cellstyle.borders[i].color;
            }
          // FREE (_obj->ovr.cellstyle.borders);
          //_obj->ovr.cellstyle.borders = NULL;
          //_obj->ovr.cellstyle.num_borders = 0;
        }
    }
  // header
  if (!_obj->rowstyles[2].num_borders)
    {
      _obj->rowstyles[2].num_borders = 6;
      _obj->rowstyles[2].borders = (Dwg_TABLESTYLE_border *)CALLOC (
          6, sizeof (Dwg_TABLESTYLE_border));
      _obj->rowstyles[2].text_color.method = 0xc1;
      _obj->rowstyles[2].fill_color.method = 0xc1;
      if (_obj->ovr.cellstyle.borders)
        {
          for (unsigned i = 0; i < _obj->rowstyles[2].num_borders; i++)
            {
              _obj->rowstyles[2].borders[i].linewt
                  = _obj->ovr.cellstyle.borders[i].linewt;
              _obj->rowstyles[2].borders[i].visible
                  = _obj->ovr.cellstyle.borders[i].visible;
              _obj->rowstyles[2].borders[i].color
                  = _obj->ovr.cellstyle.borders[i].color;
            }
        }
    }
}

// from >2007 to 2000, need to add a EED with the APPID.ACAD_MLEADERVER
// class_version
static void
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
        = (Dwg_Eed *)REALLOC (oo->eed, (oo->num_eed + 1) * sizeof (Dwg_Eed));
  else
    oo->eed = (Dwg_Eed *)CALLOC (2, sizeof (Dwg_Eed));
  dwg_add_handle (&oo->eed[idx].handle, 5, eedhdl, NULL);
  oo->eed[idx].size = 3;
  oo->eed[idx].data = (Dwg_Eed_Data *)CALLOC (3, 1);
  oo->eed[idx].data->code = 70;
  _obj = oo->tio.MLEADERSTYLE;
  oo->eed[idx].data->u.eed_70.rs
      = _obj->class_version ? _obj->class_version : 2;
  idx++;
  oo->eed[idx].size = 0;
  oo->eed[idx].raw = NULL;
  oo->eed[idx].data = NULL;
}

static void
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
        oo->eed = (Dwg_Eed *)REALLOC (oo->eed,
                                      (oo->num_eed + 1) * sizeof (Dwg_Eed));
      else
        oo->eed = (Dwg_Eed *)CALLOC (6, sizeof (Dwg_Eed));
      // AnnotativeData
      dwg_add_handle (&oo->eed[idx].handle, 5, eedhdl1, NULL);
      oo->eed[idx].size = 28;
      oo->eed[idx].raw = NULL;
      oo->eed[idx].data = (Dwg_Eed_Data *)CALLOC (20, 1);
      oo->eed[idx].data->code = 0;
      oo->eed[idx].data->u.eed_0.length = 14; // sizeof ("AnnotativeData") - 1;
      oo->eed[idx].data->u.eed_0.codepage = 30;
      memcpy (oo->eed[idx].data->u.eed_0.string, "AnnotativeData", 15);
      idx++;
      oo->eed[idx].size = 0;
      oo->eed[idx].raw = NULL;
      oo->eed[idx].data = (Dwg_Eed_Data *)CALLOC (2, 1);
      oo->eed[idx].data->code = 2; // open
      idx++;
      oo->eed[idx].size = 0;
      oo->eed[idx].raw = NULL;
      oo->eed[idx].data = (Dwg_Eed_Data *)CALLOC (3, 1);
      oo->eed[idx].data->code = 70;
      oo->eed[idx].data->u.eed_70.rs = 1;
      idx++;
      oo->eed[idx].size = 0;
      oo->eed[idx].raw = NULL;
      oo->eed[idx].data = (Dwg_Eed_Data *)CALLOC (3, 1);
      oo->eed[idx].data->code = 70;
      oo->eed[idx].data->u.eed_70.rs = 1;
      idx++;
      oo->eed[idx].size = 0;
      oo->eed[idx].raw = NULL;
      oo->eed[idx].data = (Dwg_Eed_Data *)CALLOC (2, 1);
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
        oo->eed = (Dwg_Eed *)REALLOC (oo->eed,
                                      (oo->num_eed + 1) * sizeof (Dwg_Eed));
      else
        oo->eed = (Dwg_Eed *)CALLOC (3, sizeof (Dwg_Eed));
      // DIMJAG
      dwg_add_handle (&oo->eed[idx].handle, 5, eedhdl2, NULL);
      oo->eed[idx].size = 12;
      oo->eed[idx].raw = NULL;
      oo->eed[idx].data = (Dwg_Eed_Data *)CALLOC (3, 1);
      oo->eed[idx].data->code = 70;
      //_obj = oo->tio.DIMSTYLE;
      oo->eed[idx].data->u.eed_70.rs = 388; // FIXME Which value?
      idx++;
      oo->eed[idx].size = 0;
      oo->eed[idx].raw = NULL;
      oo->eed[idx].data = (Dwg_Eed_Data *)CALLOC (9, 1);
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
        oo->eed = (Dwg_Eed *)REALLOC (oo->eed,
                                      (oo->num_eed + 1) * sizeof (Dwg_Eed));
      else
        oo->eed = (Dwg_Eed *)CALLOC (3, sizeof (Dwg_Eed));
      // DIMTALN
      dwg_add_handle (&oo->eed[idx].handle, 5, eedhdl2, NULL);
      oo->eed[idx].size = 6;
      oo->eed[idx].raw = NULL;
      oo->eed[idx].data = (Dwg_Eed_Data *)CALLOC (3, 1);
      oo->eed[idx].data->code = 70;
      //_obj = oo->tio.DIMSTYLE;
      oo->eed[idx].data->u.eed_70.rs = 392; // FIXME Which value?
      idx++;
      oo->eed[idx].size = 0;
      oo->eed[idx].raw = NULL;
      oo->eed[idx].data = (Dwg_Eed_Data *)CALLOC (3, 1);
      oo->eed[idx].data->code = 70;
      oo->eed[idx].data->u.eed_70.rs = 0; // FIXME Which value?
      idx++;
      oo->eed[idx].size = 0;
      oo->eed[idx].data = NULL;
      oo->eed[idx].raw = NULL;
    }
  if (idx != oo->num_eed)
    {
      // eg. when the EED already had AcadAnnotative
      LOG_WARN ("Already DIMSTYLE(" FORMAT_HV ") eed idx %u vs num_eed %u\n",
                obj->handle.value, idx, oo->num_eed);
      oo->eed[idx].data = NULL;
      oo->num_eed = idx;
      oo->eed = (Dwg_Eed *)REALLOC (oo->eed, oo->num_eed * sizeof (Dwg_Eed));
    }
}

// up or downconvert from/to 256/512 bytes
void
dwg_convert_LTYPE_strings_area (const Dwg_Data *restrict dwg,
                                Dwg_Object_LTYPE *restrict _obj)
{
  if (dwg->header.from_version <= R_2004 && dwg->header.version > R_2004)
    {
      // upconvert to 512
      BITCODE_TF old = _obj->strings_area;
      if (!old)
        {
          _obj->has_strings_area = 0;
          return;
        }
      _obj->strings_area = (BITCODE_TF)CALLOC (1, 512);
      if (!_obj->strings_area)
        {
          _obj->has_strings_area = 0;
          FREE (old);
          return;
        }
      _obj->has_strings_area = 1;
      for (int i = 0; i < 256; i++)
        {
          _obj->strings_area[i * 2] = old[i];
        }
      FREE (old);
    }
  else if (dwg->header.from_version > R_2004 && dwg->header.version <= R_2004)
    {
      // downconvert to 256
      BITCODE_TF old = _obj->strings_area;
      if (!old)
        _obj->has_strings_area = 0;
      _obj->strings_area = (BITCODE_TF)CALLOC (1, 256);
      if (!_obj->strings_area || !old)
        { // all empty
          if (old)
            FREE (old);
          if (_obj->strings_area)
            FREE (_obj->strings_area);
          _obj->strings_area = NULL;
          return;
        }
      _obj->has_strings_area = 1;
      for (int i = 0; i < 256; i++)
        {
          _obj->strings_area[i] = old[i * 2];
        }
      FREE (old);
    }
}

#undef IS_ENCODER
