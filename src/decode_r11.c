/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2022-2025 Free Software Foundation, Inc.                   */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * decode_r11.c: preR13 decoding
 * written by Reini Urban
 * modified by Michal Josef Špaček
 */

#include "config.h"
#ifndef _DEFAULT_SOURCE
#  define _DEFAULT_SOURCE 1 /* for byteswap.h */
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <limits.h>

#define IS_DECODER
#define _DECODE_R11_C
#ifdef USE_TRACING
/* This flag means we have checked the environment variable
   LIBREDWG_TRACE and set `loglevel' appropriately.  */
// static bool env_var_checked_p;
#endif /* USE_TRACING */
#define DWG_LOGLEVEL loglevel
#include "logging.h"
#include "dwg_api.h" // for the preR13 add API

#include "common.h"
#include "bits.h"
#include "dwg.h"
#include "decode.h"
#include "free.h"

/* The logging level for the read (decode) path.
 * Yes, this library is not thread-safe.
 */
static unsigned int loglevel;
/* the current version per spec block */
static int cur_ver = 0;
static BITCODE_BL rcount1 = 0, rcount2 = 0;

#ifdef DWG_ABORT
static unsigned int errors = 0;
#  ifndef DWG_ABORT_LIMIT
#    define DWG_ABORT_LIMIT 200
#  endif
#endif

#include "dynapi.h"
#include "dec_macros.h"
#include "decode_r11.h"

// need spec.h for the LOG_FLAG_TABLE*
#define ACTION decode
#include "spec.h"

void dwg_set_next_hdl (Dwg_Data *dwg, unsigned long value);

/*------------------------------------------------------------------------------
 * Private functions
 */

static int decode_preR13_section_hdr (const char *restrict name,
                                      Dwg_Section_Type_r11 id,
                                      Bit_Chain *restrict dat,
                                      Dwg_Data *restrict dwg);
static int decode_preR13_section (Dwg_Section_Type_r11 id,
                                  Bit_Chain *restrict dat,
                                  Dwg_Data *restrict dwg);

/*----------------------------------------------------------------------------
 * Public function definitions
 */

EXPORT Dwg_Object_Ref *
dwg_decode_preR13_handleref (Bit_Chain *restrict dat, int size,
                             Dwg_Data *restrict dwg)
{
  Dwg_Object_Ref *ref = dwg_new_ref (dwg);
  if (!ref)
    {
      LOG_ERROR ("Out of memory");
      return NULL;
    }
  // dwg->object_ref[dwg->num_object_refs++] = ref;
  // ref->handleref.is_global = 1;
  ref->handleref.size = size;
  if (size == 2)
    ref->r11_idx = (BITCODE_RSd)bit_read_RS (dat);
  else
    ref->r11_idx = (BITCODE_RCd)bit_read_RC (dat);
  return ref;
}

static int
decode_preR13_header_variables (Bit_Chain *dat, Dwg_Data *restrict dwg)
{
  Dwg_Header_Variables *_obj = &dwg->header_vars;
  Dwg_Object *obj = NULL;
  Bit_Chain *hdl_dat = dat;
  int error = 0;

  // clang-format off
  #include "header_variables_r11.spec"
  // clang-format on

  return error;
}

// We put the 5 tables into sections.
// number is num_entries in the table. >=r13 it is the id.
static int
decode_preR13_section_hdr (const char *restrict name, Dwg_Section_Type_r11 id,
                           Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  // int error = 0;
  Dwg_Section *tbl = &dwg->header.section[id];
  unsigned long end_address;

  if (dat->byte + 10 > dat->size)
    {
      LOG_ERROR ("%s.size overflow @%" PRIuSIZE, name, dat->byte)
      return DWG_ERR_SECTIONNOTFOUND;
    }
  tbl->type = (Dwg_Section_Type)id;
  strncpy (tbl->name, name, sizeof (tbl->name) - 1);
  tbl->name[sizeof (tbl->name) - 1] = '\0';
  LOG_TRACE ("ptr table %s_CONTROL [%d]", tbl->name, id);
  LOG_RPOS;
  LOG_TRACE ("----------------------\n");
  tbl->size = (BITCODE_RL)bit_read_RS (dat);
  LOG_TRACE ("%s_CONTROL.size: " FORMAT_RS " [RS]", tbl->name,
             (BITCODE_RS)tbl->size);
  LOG_RPOS;
  // RC in r2000, RL in 2004
  tbl->number = (BITCODE_RLd)((BITCODE_RSd)bit_read_RS (dat));
  LOG_TRACE ("%s_CONTROL.number: " FORMAT_RS " [RS]", tbl->name,
             (BITCODE_RS)tbl->number);
  LOG_RPOS;
  tbl->flags_r11 = bit_read_RS (dat);
  LOG_TRACE ("%s_CONTROL.flags_r11: " FORMAT_RSx " [RS]", tbl->name,
             tbl->flags_r11);
  LOG_RPOS;
  tbl->address = bit_read_RL (dat);
  LOG_TRACE ("%s_CONTROL.address: " FORMAT_RLx " [RL] (%u)", tbl->name,
             (BITCODE_RL)tbl->address, (unsigned)tbl->address);
  LOG_RPOS;
  end_address = (unsigned long)(tbl->address + (tbl->number * tbl->size));
  LOG_TRACE ("ptr table end: 0x%lx (%lu)\n\n", end_address, end_address);

  switch (id)
    {
    case SECTION_BLOCK:
      {
        Dwg_Object *obj = dwg_get_first_object (dwg, DWG_TYPE_BLOCK_CONTROL);
        if (obj)
          {
            Dwg_Object_BLOCK_CONTROL *_obj
                = obj->tio.object->tio.BLOCK_CONTROL;
            obj->size = tbl->size;
            obj->address = tbl->address;
            _obj->flags_r11 = tbl->flags_r11;
            //  we cannot set _obj->num_entries, as we add BLOCK's via
            //  add_BLOCK_HEADER
            dwg->block_control = *_obj;
          }
      }
      break;

#define CASE_TBL(TBL)                                                         \
  case SECTION_##TBL:                                                         \
    {                                                                         \
      Dwg_Object *obj = dwg_get_first_object (dwg, DWG_TYPE_##TBL##_CONTROL); \
      if (obj)                                                                \
        {                                                                     \
          Dwg_Object_##TBL##_CONTROL *_obj                                    \
              = obj->tio.object->tio.TBL##_CONTROL;                           \
          obj->size = tbl->size;                                              \
          obj->address = tbl->address;                                        \
          _obj->flags_r11 = tbl->flags_r11;                                   \
        }                                                                     \
    }                                                                         \
    break

      CASE_TBL (LAYER);
      CASE_TBL (STYLE);
      CASE_TBL (LTYPE);
      CASE_TBL (VIEW);
      CASE_TBL (UCS); // since r10
      CASE_TBL (VPORT);
      CASE_TBL (DIMSTYLE);
      CASE_TBL (APPID); // since r11
      CASE_TBL (VX);
    default:
      LOG_ERROR ("Illegal section id %d", id);
    }
  if (tbl->number > 0 && tbl->size < 33)
    {
      LOG_ERROR ("Wrong %s.number " FORMAT_RLd " or size %u", tbl->name,
                 tbl->number, (unsigned)tbl->size)
      return 1; // DWG_ERR_SECTIONNOTFOUND;
    }
  if (tbl->number > 0
      && (tbl->address + (tbl->number * tbl->size) > dat->size))
    {
      LOG_ERROR ("%s.size overflow %" PRIuSIZE " > %" PRIuSIZE, tbl->name,
                 (size_t)(tbl->address + (tbl->number * tbl->size)),
                 dat->size);
      // VPORT.size bug in DWG, ignore it.
      return id == SECTION_VPORT ? 0 : 1;
    }
  return 0;
}

GCC30_DIAG_IGNORE (-Wformat-nonliteral)
// TABLES really
static int
decode_preR13_section (Dwg_Section_Type_r11 id, Bit_Chain *restrict dat,
                       Dwg_Data *restrict dwg)
{
  Dwg_Section *tbl = &dwg->header.section[id];
  // Bit_Chain *hdl_dat = dat;
  Dwg_Object *obj;
  int error = 0;
  BITCODE_RLd i;
  // BITCODE_RL vcount;
  BITCODE_RL num = dwg->num_objects;
  size_t pos = tbl->address;
  size_t oldpos;
  size_t real_start = pos;
  // BITCODE_TF name;
  // BITCODE_RSd used = -1;
  // BITCODE_RC flag;

  LOG_TRACE ("\ncontents table %-8s [%2d]: size:%-4u num:%-3ld (" FORMAT_RLL
             "-" FORMAT_RLL ")\n\n",
             tbl->name, id, tbl->size, (long)tbl->number, tbl->address,
             tbl->address + (tbl->number * tbl->size))

  // with sentinel in case of R11
  SINCE (R_11)
  {
    real_start -= 16; // the sentinel size
  }

  // report unknown data before table
  if (tbl->address && dat->byte != real_start)
    {
      LOG_WARN ("\n@0x%zx => start 0x%zx", dat->byte, real_start);
      if (dat->byte < real_start)
        {
          UNKNOWN_UNTIL (real_start);
        }
    }

  SINCE (R_11)
  {
#define DECODE_PRER13_SENTINEL(ID)                                            \
  error |= decode_preR13_sentinel (ID, #ID, dat, dwg);                        \
  if (error >= DWG_ERR_SECTIONNOTFOUND)                                       \
  return error

    switch (id)
      {
#define CASE_SENTINEL_BEGIN(id)                                               \
  case SECTION_##id:                                                          \
    DECODE_PRER13_SENTINEL (DWG_SENTINEL_R11_##id##_BEGIN);                   \
    break

        CASE_SENTINEL_BEGIN (BLOCK);
        CASE_SENTINEL_BEGIN (LAYER);
        CASE_SENTINEL_BEGIN (STYLE);
        CASE_SENTINEL_BEGIN (LTYPE);
        CASE_SENTINEL_BEGIN (VIEW);
        CASE_SENTINEL_BEGIN (UCS);
        CASE_SENTINEL_BEGIN (VPORT);
        CASE_SENTINEL_BEGIN (APPID);
        CASE_SENTINEL_BEGIN (DIMSTYLE);
        CASE_SENTINEL_BEGIN (VX);
#undef CASE_SENTINEL_BEGIN

      default:
        LOG_ERROR ("Internal error: Invalid section id %d", (int)id);
        return DWG_ERR_INTERNALERROR;
      }
  }

  oldpos = dat->byte;
  if (tbl->address)
    dat->byte = tbl->address;
  dat->bit = 0;
  if ((size_t)(tbl->number * tbl->size) > dat->size - dat->byte)
    {
      LOG_ERROR ("Overlarge table num_entries %ld or size %ld for %-8s [%2d]",
                 (long)tbl->number, (long)tbl->size, tbl->name, id);
      dat->byte = oldpos;
      return DWG_ERR_INVALIDDWG;
    }
  tbl->objid_r11 = num;
  if (dwg->num_alloced_objects < dwg->num_objects + tbl->number)
    {
      dwg->num_alloced_objects = dwg->num_objects + tbl->number;
      if (dwg->num_alloced_objects > dwg->num_objects + MAX_NUM)
        {
          LOG_ERROR ("Invalid num_alloced_objects " FORMAT_BL,
                     dwg->num_alloced_objects);
          return DWG_ERR_INVALIDDWG;
        }
      dwg->object = (Dwg_Object *)REALLOC (
          dwg->object, dwg->num_alloced_objects * sizeof (Dwg_Object));
      dwg->dirty_refs = 1;
    }

#define SET_CONTROL(token)                                                    \
  Dwg_Object *ctrl;                                                           \
  Dwg_Object_##token##_CONTROL *_ctrl = NULL;                                 \
  ctrl = dwg_get_first_object (dwg, DWG_TYPE_##token##_CONTROL);              \
  if (ctrl)                                                                   \
    {                                                                         \
      _ctrl = ctrl->tio.object->tio.token##_CONTROL;                          \
      ctrl->size = tbl->size;                                                 \
      if (tbl->number > _ctrl->num_entries)                                   \
        {                                                                     \
          if (_ctrl->entries)                                                 \
            {                                                                 \
              _ctrl->entries = (BITCODE_H *)REALLOC (                         \
                  _ctrl->entries, tbl->number * sizeof (BITCODE_H));          \
              memset (&_ctrl->entries[_ctrl->num_entries], 0,                 \
                      (tbl->number - _ctrl->num_entries)                      \
                          * sizeof (BITCODE_H));                              \
            }                                                                 \
          else                                                                \
            _ctrl->entries                                                    \
                = (BITCODE_H *)CALLOC (tbl->number, sizeof (BITCODE_H));      \
          _ctrl->num_entries = tbl->number;                                   \
          LOG_TRACE (#token "_CONTROL.num_entries = %u\n", tbl->number);      \
        }                                                                     \
    }

#define NEW_OBJECT                                                            \
  dwg_add_object (dwg);                                                       \
  if (dat->byte > dat->size)                                                  \
    return DWG_ERR_INVALIDDWG;                                                \
  obj = &dwg->object[num++];                                                  \
  obj->address = dat->byte;                                                   \
  obj->size = tbl->size;

#define ADD_CTRL_ENTRY                                                        \
  if (_ctrl)                                                                  \
    {                                                                         \
      BITCODE_H ref;                                                          \
      if (!obj->handle.value)                                                 \
        obj->handle.value = dwg_next_handseed (dwg);                          \
      ref = _ctrl->entries[i]                                                 \
          = dwg_add_handleref (dwg, 2, obj->handle.value, obj);               \
      ref->r11_idx = i;                                                       \
      LOG_TRACE ("%s.entries[%u] = " FORMAT_REF " [H 0]\n", ctrl->name, i,    \
                 ARGS_REF (ref));                                             \
    }                                                                         \
  else                                                                        \
    return error | DWG_ERR_INVALIDDWG

#define CHK_ENDPOS                                                            \
  SINCE (R_11)                                                                \
  {                                                                           \
    if (!bit_check_CRC (dat, obj->address, 0xC0C1))                           \
      error |= DWG_ERR_WRONGCRC;                                              \
  }                                                                           \
  pos = tbl->address + (long)((i + 1) * tbl->size);                           \
  if (pos != dat->byte)                                                       \
    {                                                                         \
      LOG_ERROR ("offset %ld", (long)(pos - dat->byte));                      \
      if (pos > dat->byte)                                                    \
        {                                                                     \
          BITCODE_RL offset = (BITCODE_RL)(pos - dat->byte);                  \
          obj->num_unknown_rest = 8 * offset;                                 \
          obj->unknown_rest = (BITCODE_TF)CALLOC (offset + 1, 1);             \
          if (obj->unknown_rest)                                              \
            {                                                                 \
              memcpy (obj->unknown_rest, &dat->chain[dat->byte], offset);     \
              LOG_TRACE_TF (obj->unknown_rest, offset);                       \
            }                                                                 \
          else                                                                \
            {                                                                 \
              LOG_ERROR ("Out of memory");                                    \
              obj->num_unknown_rest = 0;                                      \
            }                                                                 \
        }                                                                     \
      /* In the table header the size OR number can be wrong. */              \
      /* Here we catch the wrong number. */                                   \
      if (tbl->number > 0 && tbl->size < 33)                                  \
        return DWG_ERR_SECTIONNOTFOUND;                                       \
    }                                                                         \
  LOG_TRACE ("\n")                                                            \
  dat->byte = pos

  switch (id)
    {
    case SECTION_BLOCK:
      {
        SET_CONTROL (BLOCK);
        for (i = 0; i < tbl->number; i++)
          {
            NEW_OBJECT;
            error |= dwg_decode_BLOCK_HEADER (dat, obj);
            // PUSH_HV (_hdr, num_owned, entities, ref);
            ADD_CTRL_ENTRY;
            CHK_ENDPOS;
          }
      }
      break;

    case SECTION_LAYER:
      {
        SET_CONTROL (LAYER);
        for (i = 0; i < tbl->number; i++)
          {
            NEW_OBJECT;
            error |= dwg_decode_LAYER (dat, obj);
            ADD_CTRL_ENTRY;
            CHK_ENDPOS;
          }
      }
      break;

    // was a text STYLE table, became a STYLE object
    case SECTION_STYLE:
      {
        SET_CONTROL (STYLE);
        for (i = 0; i < tbl->number; i++)
          {
            NEW_OBJECT;
            error |= dwg_decode_STYLE (dat, obj);
            ADD_CTRL_ENTRY;
            CHK_ENDPOS;
          }
      }
      break;

    case SECTION_LTYPE:
      {
        SET_CONTROL (LTYPE);
        for (i = 0; i < tbl->number; i++)
          {
            NEW_OBJECT;
            error |= dwg_decode_LTYPE (dat, obj);
            ADD_CTRL_ENTRY;
            if (strEQc (tbl->name, "CONTINUOUS"))
              dwg->header_vars.LTYPE_CONTINUOUS = _ctrl->entries[i];
            CHK_ENDPOS;
          }
      }
      break;

    case SECTION_VIEW:
      {
        SET_CONTROL (VIEW);
        for (i = 0; i < tbl->number; i++)
          {
            NEW_OBJECT;
            error |= dwg_decode_VIEW (dat, obj);
            ADD_CTRL_ENTRY;
            CHK_ENDPOS;
          }
      }
      break;

    // SINCE R_11
    case SECTION_UCS:
      {
        SET_CONTROL (UCS);
        for (i = 0; i < tbl->number; i++)
          {
            NEW_OBJECT;
            error |= dwg_decode_UCS (dat, obj);
            ADD_CTRL_ENTRY;
            CHK_ENDPOS;
          }
      }
      break;

    // SINCE R_11
    case SECTION_VPORT:
      {
        SET_CONTROL (VPORT);
        for (i = 0; i < tbl->number; i++)
          {
            NEW_OBJECT;
            error |= dwg_decode_VPORT (dat, obj);
            ADD_CTRL_ENTRY;
            CHK_ENDPOS;
          }
      }
      break;

    // SINCE R_11
    case SECTION_APPID:
      {
        SET_CONTROL (APPID);
        for (i = 0; i < tbl->number; i++)
          {
            NEW_OBJECT;
            error |= dwg_decode_APPID (dat, obj);
            ADD_CTRL_ENTRY;
            CHK_ENDPOS;
          }
      }
      break;

    // SINCE R_11
    case SECTION_DIMSTYLE:
      {
        SET_CONTROL (DIMSTYLE);
        for (i = 0; i < tbl->number; i++)
          {
            NEW_OBJECT;
            error |= dwg_decode_DIMSTYLE (dat, obj);
            ADD_CTRL_ENTRY;
            CHK_ENDPOS;
          }
      }
      break;

    // SINCE R_11
    case SECTION_VX:
      {
        SET_CONTROL (VX);
        for (i = 0; i < tbl->number; i++)
          {
            NEW_OBJECT;
            error |= dwg_decode_VX_TABLE_RECORD (dat, obj);
            ADD_CTRL_ENTRY;
            CHK_ENDPOS;
          }
      }
      break;

    case SECTION_HEADER_R11:
    default:
      LOG_ERROR ("Invalid table id %d", id);
      tbl->number = 0;
      break;
    }

  if (tbl->address && tbl->number && tbl->size)
    dat->byte = tbl->address + (tbl->number * tbl->size);
  else
    dat->byte = oldpos;

  SINCE (R_11)
  {
    switch (id)
      {
#define CASE_SENTINEL_END(id)                                                 \
  case SECTION_##id:                                                          \
    DECODE_PRER13_SENTINEL (DWG_SENTINEL_R11_##id##_END);                     \
    break

        CASE_SENTINEL_END (BLOCK);
        CASE_SENTINEL_END (LAYER);
        CASE_SENTINEL_END (STYLE);
        CASE_SENTINEL_END (LTYPE);
        CASE_SENTINEL_END (VIEW);
        CASE_SENTINEL_END (UCS);
        CASE_SENTINEL_END (VPORT);
        CASE_SENTINEL_END (APPID);
        CASE_SENTINEL_END (DIMSTYLE);
        CASE_SENTINEL_END (VX);
      default:
        LOG_ERROR ("Internal error: Invalid section id %d", (int)id);
        return DWG_ERR_INTERNALERROR;
      }
  }
#undef DECODE_PRER13_SENTINEL
#undef CASE_SENTINEL_END

  return error;
}

EXPORT int
decode_entity_preR13 (Bit_Chain *restrict dat, Dwg_Object *restrict obj,
                      Dwg_Object_Entity *_ent)
{
  Dwg_Object_Entity *_obj = _ent;
  const bool is_block = obj->address >= 0x40000000;
  Bit_Chain *hdl_dat = NULL, *str_dat = NULL;
  Dwg_Data *dwg = _ent->dwg;
  int error = 0; // errors only with H or CMC. r11 has none of these

  obj->bitsize_pos = bit_position (dat);
  obj->address
      = dat->byte - 1; // already read the type. size includes the type
  LOG_INFO ("===========================\n"
            "Entity number: %d, Type: %d, Addr: %zx\n",
            obj->index, obj->type, obj->address);
  _obj->entmode = is_block ? 3 : 2; // ent or block

#include "common_entity_data.spec"

  if (!obj->handle.value) // can be set via FLAG_R11_HAS_HANDLING also
    {
      obj->handle.value = dwg_next_handle (dwg);
      dwg_add_handle (&obj->handle, 0, obj->handle.value, obj);
      LOG_TRACE ("=> handle: (0.%d." FORMAT_HV ")\n", obj->handle.size,
                 obj->handle.value);
    }
  obj->common_size = bit_position (dat) - obj->bitsize_pos;
  return error;
}

AFL_GCC_TOOBIG
EXPORT int
decode_preR13 (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  BITCODE_RL num_entities;
  BITCODE_RL blocks_start = 0, blocks_end = 0, blocks_size = 0;
  BITCODE_RL extras_start = 0, extras_end = 0, extras_size = 0;
  // BITCODE_RS rs2;
  Dwg_Object *obj = NULL;
  int error = 0;
  // Bit_Chain dat_save = *dat;

  loglevel = dat->opts & DWG_OPTS_LOGLEVEL;
  {
    int i;
    Dwg_Header *_obj = (Dwg_Header *)&dwg->header;
    // Bit_Chain *hdl_dat = dat;
    BITCODE_BL vcount;

    // clang-format off
    #include "header.spec"
    // clang-format on
  }
  LOG_TRACE ("@0x%zx\n", dat->byte); // 0x14
  SINCE (R_2_0b)
  {
    // Block entities
    blocks_start = dwg->header.blocks_start;
    blocks_size = dwg->header.blocks_size;
    if (blocks_size > 0xffffff)
      {
        blocks_size &= 0xffffff;
        LOG_TRACE ("blocks_size => " FORMAT_RLx "\n", blocks_size);
      }
    blocks_end = blocks_start + blocks_size;
    // Extra entities
    extras_start = dwg->header.extras_start;
    extras_size = dwg->header.extras_size;
    if (extras_size > 0xffffff)
      {
        extras_size &= 0xffffff;
        LOG_TRACE ("extras_size => " FORMAT_RLx "\n", extras_size);
      }
    extras_end = extras_start + extras_size;
  }

  // setup all the new control objects
  error |= dwg_add_Document (dwg, 0);

  SINCE (R_2_0b)
  {
    dwg->header.section[0].number = 0;
    dwg->header.section[0].type = (Dwg_Section_Type)SECTION_HEADER_R11;
    strcpy (dwg->header.section[0].name, "HEADER");

    // The 5 tables (num_sections always 5): 3 RS + 1 RL address
    LOG_INFO ("==========================================\n")
    // dat_save = *dat;
    error |= decode_preR13_section_hdr ("BLOCK", SECTION_BLOCK, dat, dwg);
    if (error >= DWG_ERR_CRITICAL)
      return error;
    error |= decode_preR13_section_hdr ("LAYER", SECTION_LAYER, dat, dwg);
    if (error >= DWG_ERR_CRITICAL)
      return error;
    error |= decode_preR13_section_hdr ("STYLE", SECTION_STYLE, dat, dwg);
    if (error >= DWG_ERR_CRITICAL)
      return error;
    error |= decode_preR13_section_hdr ("LTYPE", SECTION_LTYPE, dat, dwg);
    if (error >= DWG_ERR_CRITICAL)
      return error;
    error |= decode_preR13_section_hdr ("VIEW", SECTION_VIEW, dat, dwg);
    if (error >= DWG_ERR_CRITICAL)
      return error;
  }
  LOG_TRACE ("@0x%zx\n", dat->byte); // 0x5e
  if (dat->size < 0x1f0)             // AC1.50 0x1f9 74 vars
    {
      LOG_ERROR ("DWG too small %" PRIuSIZE, (size_t)dat->size)
      return DWG_ERR_INVALIDDWG;
    }

  LOG_TRACE ("==========================================\n")
  error |= decode_preR13_header_variables (dat, dwg);
  LOG_TRACE ("@0x%zx\n", dat->byte);
  if (error >= DWG_ERR_CRITICAL)
    return error;
  if (dat->byte + 2 >= dat->size)
    {
      LOG_ERROR ("post HEADER overflow")
      return error | DWG_ERR_CRITICAL;
    }
  SINCE (R_11)
  {
    // crc16 + DWG_SENTINEL_R11_ENTITIES_BEGIN
    BITCODE_RS crc, crcc;
    crcc = bit_calc_CRC (0xC0C1, &dat->chain[0], dat->byte); // from 0 to now
    crc = bit_read_RS (dat);
    LOG_TRACE ("crc: %04X [RSx] from 0-0x%zx\n", crc, dat->byte - 2);
    if (crc != crcc)
      {
        LOG_ERROR ("Header CRC mismatch %04X <=> %04X", crc, crcc);
        error |= DWG_ERR_WRONGCRC;
      }
  }

  PRE (R_10)
  {
    num_entities = dwg->header_vars.numentities;
  }
  LATER_VERSIONS
  {
    num_entities = 0;
  }
  PRE (R_2_0b)
  {
    dwg->header.entities_start = dat->byte & 0xFFFFFFFF;
    dwg->header.entities_end = dwg->header_vars.dwg_size;
  }

  // entities
  error |= decode_preR13_entities (
      dwg->header.entities_start, dwg->header.entities_end, num_entities,
      dwg->header.entities_end - dwg->header.entities_start, dat, dwg,
      ENTITIES_SECTION_INDEX);
  if (error >= DWG_ERR_CRITICAL)
    return error;

  PRE (R_2_0b)
  {
    // this has usually some slack at the end.
    return error;
  }

  error |= decode_preR13_section (SECTION_BLOCK, dat, dwg);
  if (error >= DWG_ERR_CRITICAL)
    return error;
  error |= decode_preR13_section (SECTION_LAYER, dat, dwg);
  if (error >= DWG_ERR_CRITICAL)
    return error;
  error |= decode_preR13_section (SECTION_STYLE, dat, dwg);
  if (error >= DWG_ERR_CRITICAL)
    return error;
  error |= decode_preR13_section (SECTION_LTYPE, dat, dwg);
  if (error >= DWG_ERR_CRITICAL)
    return error;
  error |= decode_preR13_section (SECTION_VIEW, dat, dwg);
  if (error >= DWG_ERR_CRITICAL)
    return error;
  if (dwg->header.num_sections >= SECTION_VPORT) // r10
    {
      error |= decode_preR13_section (SECTION_UCS, dat, dwg);
      if (error >= DWG_ERR_CRITICAL)
        return error;
      error |= decode_preR13_section (SECTION_VPORT, dat, dwg);
      if (error >= DWG_ERR_CRITICAL)
        return error;
    }
  if (dwg->header.num_sections >= SECTION_APPID) // r10
    {
      error |= decode_preR13_section (SECTION_APPID, dat, dwg);
      if (error >= DWG_ERR_CRITICAL)
        return error;
    }
  if (dwg->header.num_sections >= SECTION_VX) // r11
    {
      error |= decode_preR13_section (SECTION_DIMSTYLE, dat, dwg);
      if (error >= DWG_ERR_CRITICAL)
        return error;
      error |= decode_preR13_section (SECTION_VX, dat, dwg);
      if (error >= DWG_ERR_CRITICAL)
        return error;
    }

  // block entities
  error |= decode_preR13_entities (blocks_start, blocks_end, 0, blocks_size,
                                   dat, dwg, BLOCKS_SECTION_INDEX);
  if (error >= DWG_ERR_CRITICAL)
    return error;

  // extra entities
  error |= decode_preR13_entities (extras_start, extras_end, 0, extras_size,
                                   dat, dwg, EXTRAS_SECTION_INDEX);
  if (error >= DWG_ERR_CRITICAL)
    return error;

  // aux header
  SINCE (R_11)
  {
    error |= decode_r11_auxheader (dat, dwg);
    if (error >= DWG_ERR_CRITICAL)
      return error;
  }

  if (dat->byte < dat->size)
    {
      size_t len = dat->size - dat->byte;
      BITCODE_TF unknown = bit_read_TF (dat, len);
      LOG_TRACE ("unknown (%" PRIuSIZE "):", len);
      LOG_TRACE_TF (unknown, len);
      FREE (unknown);
    }

  if (dwg->dirty_refs)
    dwg_resolve_objectrefs_silent (dwg);
  return 0;
}
AFL_GCC_POP

#undef IS_DECODER
