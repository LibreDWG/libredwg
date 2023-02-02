/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2022-2023 Free Software Foundation, Inc.                   */
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
//#ifdef __STDC_ALLOC_LIB__
//#  define __STDC_WANT_LIB_EXT2__ 1 /* for strdup */
//#else
//#  define _USE_BSD 1
//#endif
#ifndef _DEFAULT_SOURCE
#  define _DEFAULT_SOURCE 1 /* for byteswap.h */
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#define IS_DECODER
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

#ifdef USE_TRACING
/* This flag means we have checked the environment variable
   LIBREDWG_TRACE and set `loglevel' appropriately.  */
static bool env_var_checked_p;
#endif /* USE_TRACING */
#define DWG_LOGLEVEL loglevel

#include "logging.h"
#include "dwg_api.h" // for preR13 only
#include "dynapi.h"
#include "dec_macros.h"
#include "decode_r11.h"

// need spec.h for the LOG_FLAG_TABLE*
#define ACTION decode
#include "spec.h"

//#undef LOG_POS
//#define LOG_POS LOG_INSANE (" @%lu.%u\n", dat->byte, dat->bit)

// This needs the add API
#ifdef USE_WRITE

void dwg_set_next_hdl (Dwg_Data *dwg, unsigned long value);

/*------------------------------------------------------------------------------
 * Private functions
 */

static int decode_preR13_section_hdr (const char *restrict name,
                                      Dwg_Section_Type_r11 id,
                                      Bit_Chain *restrict dat,
                                      Dwg_Data *restrict dwg);
static void decode_preR13_section_chk (Dwg_Section_Type_r11 id,
                                       Bit_Chain *restrict dat,
                                       Dwg_Data *restrict dwg);
static int decode_preR13_section (Dwg_Section_Type_r11 id,
                                  Bit_Chain *restrict dat,
                                  Dwg_Data *restrict dwg);

/*----------------------------------------------------------------------------
 * Public function definitions
 */

EXPORT Dwg_Object_Ref *
dwg_decode_preR13_handleref (Bit_Chain *restrict dat, int size)
{
  Dwg_Object_Ref *ref = (Dwg_Object_Ref *)calloc (1, sizeof (Dwg_Object_Ref));
  if (!ref)
    {
      LOG_ERROR ("Out of memory");
      return NULL;
    }
  if (size == 2)
    ref->r11_idx = bit_read_RS (dat);
  else
    ref->r11_idx = (BITCODE_RS)bit_read_RC (dat);
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
  int error = 0;
  Dwg_Section *tbl = &dwg->header.section[id];

  if (dat->byte + 10 > dat->size)
    {
      LOG_ERROR ("%s.size overflow @%lu", name, dat->byte)
      return DWG_ERR_SECTIONNOTFOUND;
    }
  tbl->size = bit_read_RS (dat);
  tbl->number = bit_read_RS (dat);
  tbl->flags = bit_read_RS (dat);
  tbl->address = bit_read_RL (dat);
  strncpy (tbl->name, name, sizeof (tbl->name) - 1);
  tbl->name[63] = '\0';
  LOG_TRACE (
      "ptr table %-8s [%2d]: size:%-4u num:%-2d (0x%lx-0x%lx) flags:0x%x\n",
      tbl->name, id, tbl->size, tbl->number, (unsigned long)tbl->address,
      (unsigned long)(tbl->address + (tbl->number * tbl->size)), tbl->flags)

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
            // obj->flag_r11 = tbl->flags;
            //  we cannot set _obj->num_entries, as we add BLOCK's via
            //  add_BLOCK_HEADER
            dwg->block_control = *_obj;
          }
      }
      break;

#  define CASE_TBL(TBL)                                                       \
  case SECTION_##TBL:                                                         \
    {                                                                         \
      Dwg_Object *obj = dwg_get_first_object (dwg, DWG_TYPE_##TBL##_CONTROL); \
      if (obj)                                                                \
        {                                                                     \
          Dwg_Object_##TBL##_CONTROL *_obj                                    \
              = obj->tio.object->tio.TBL##_CONTROL;                           \
          obj->size = tbl->size;                                              \
          obj->address = tbl->address;                                        \
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
      LOG_ERROR ("Yet unhandled section id %d", id);
    }
  if (tbl->number && (tbl->address + (tbl->number * tbl->size) > dat->size))
    {
      LOG_ERROR ("%s.size overflow", tbl->name)
      return DWG_ERR_INVALIDDWG;
    }
  return 0;
}

static void
decode_preR13_section_chk (Dwg_Section_Type_r11 id, Bit_Chain *restrict dat,
                           Dwg_Data *restrict dwg)
{
  Dwg_Section *tbl = &dwg->header.section[id];
  BITCODE_RS id1, size;
  BITCODE_RL address;
  BITCODE_RLd number;

#define CMP(f, type)                                                        \
  if (tbl->f != f)                                                          \
    LOG_ERROR ("decode_preR13_section_chk %s %s", tbl->name, #f)

  if (id > dwg->header.num_sections)
    {
      LOG_ERROR ("decode_preR13_section_chk: Invalid table %d, have only %d", id,
                 dwg->header.num_sections)
      return;
    }
  id1 = bit_read_RS (dat);
  size = bit_read_RS (dat);
  CMP (size, RS);
  number = (BITCODE_RLd)bit_read_RS (dat);
  CMP (number, RL);
  address = bit_read_RL (dat);
  CMP (address, RL)
#  undef CMP
  LOG_TRACE ("chk table %-8s [%2d]: size:%-4u nr:%-3ld (0x%x)\n", tbl->name,
             id1, size, (long)number, address)
}

GCC30_DIAG_IGNORE (-Wformat-nonliteral)
// TABLES really
static int
decode_preR13_section (Dwg_Section_Type_r11 id, Bit_Chain *restrict dat,
                       Dwg_Data *restrict dwg)
{
  Dwg_Section *tbl = &dwg->header.section[id];
  Bit_Chain *hdl_dat = dat;
  int i;
  BITCODE_BL vcount;
  int error = 0;
  long unsigned int num = dwg->num_objects;
  long unsigned int pos = tbl->address;
  BITCODE_RC flag;
  BITCODE_TF name;
  BITCODE_RSd used = -1;
  unsigned long oldpos = dat->byte;

  LOG_TRACE ("contents table %-8s [%2d]: size:%-4u num:%-3ld (0x%lx-0x%lx)\n",
             tbl->name, id, tbl->size, (long)tbl->number,
             (unsigned long)tbl->address,
             (unsigned long)(tbl->address
                             + ((unsigned long long)tbl->number * tbl->size)))
  dat->byte = tbl->address;
  dat->bit = 0;
  if ((unsigned long)(tbl->number * tbl->size) > dat->size - dat->byte)
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
      dwg->object = (Dwg_Object *)realloc (
          dwg->object, dwg->num_alloced_objects * sizeof (Dwg_Object));
      dwg->dirty_refs = 1;
    }

    // TODO: use the dwg.spec instead
    // MAYBE: move to a spec dwg_r11.spec, and dwg_decode_r11_NAME
#  define PREP_TABLE(token)                                                   \
    Dwg_Object *obj;                                                          \
    Dwg_Object_##token *_obj;                                                 \
    Dwg_Object *ctrl                                                          \
        = dwg_get_first_object (dwg, DWG_TYPE_##token##_CONTROL);             \
    Dwg_Object_##token##_CONTROL *_ctrl                                       \
        = ctrl ? ctrl->tio.object->tio.token##_CONTROL : NULL;                \
    if (!ctrl || dat->byte > dat->size || (num + i) > dwg->num_objects)       \
      return error | DWG_ERR_INVALIDDWG;                                      \
    flag = bit_read_RC (dat);                                                 \
    name = bit_read_TF (dat, 32);                                             \
    if (!name)                                                                \
      return error | DWG_ERR_INVALIDDWG;                                      \
    _obj = dwg_add_##token (dwg, (const char *)name);                         \
    obj = dwg_obj_generic_to_object (_obj, &error);                           \
    _ctrl->entries[i] = dwg_add_handleref (dwg, 2, obj->handle.value, obj);   \
    obj->size = tbl->size;                                                    \
    obj->address = pos;                                                       \
    _obj->flag = flag;                                                        \
    LOG_TRACE ("\n-- table entry " #token " [%d]: 0x%lx\n", i, pos);          \
    LOG_TRACE ("flag: %u [RC 70]\n", flag);                                   \
    LOG_FLAG_##token;                                                         \
    LOG_TRACE ("name: \"%s\" [TF 32 2]\n", name);                             \
    free (name);                                                              \
    if (dwg->header.version == R_11)                                          \
      FIELD_RSd (used, 0);

#  define LOG_FLAG_BLOCK LOG_FLAG_Block
#  define LOG_FLAG_LAYER LOG_FLAG_Layer
#  define LOG_FLAG_VIEW  LOG_FLAG_View
#  define LOG_FLAG_STYLE LOG_FLAG_TextStyle
#  define LOG_FLAG_APPID LOG_FLAG_RegApp
#  define LOG_FLAG_DIMSTYLE LOG_FLAG_DimStyle
#  define LOG_FLAG_LTYPE LOG_FLAG_Linetype
#  define LOG_FLAG_VPORT LOG_FLAG_Viewport

#  define CHK_ENDPOS                                                          \
    SINCE (R_11)                                                              \
    {                                                                         \
      BITCODE_RS crc16 = bit_read_RS (dat);                                   \
      LOG_TRACE ("crc16: %04X\n", crc16);                                     \
    }                                                                         \
    pos = tbl->address + (long)((i + 1) * tbl->size);                         \
    if (pos != dat->byte)                                                     \
      {                                                                       \
        LOG_ERROR ("offset %ld", pos - dat->byte);                            \
        /*return DWG_ERR_SECTIONNOTFOUND;*/                                   \
      }                                                                       \
    dat->byte = pos

  switch (id)
    {
    case SECTION_BLOCK:
      for (i = 0; i < tbl->number; i++)
        {
          Dwg_Object *obj;
          Dwg_Object_BLOCK_HEADER *_obj;
          Dwg_Object *ctrl;
          Dwg_Object_BLOCK_CONTROL *_ctrl;
          if (dat->byte > dat->size || (num + i) > dwg->num_objects)
            return DWG_ERR_INVALIDDWG;
          flag = bit_read_RC (dat);
          name = bit_read_TF (dat, 32);
          if (!name)
            return DWG_ERR_INVALIDDWG;
          _obj = dwg_add_BLOCK_HEADER (dwg, (const char *)name);
          _obj->flag = flag;
          LOG_TRACE ("\n-- table entry BLOCK_HEADER [%d]: 0x%lx\n", i, pos);
          LOG_TRACE ("flag: %u [RC 70]\n", flag);
          LOG_FLAG_BLOCK
          LOG_TRACE ("name: \"%s\" [TF 32 2]\n", name);
          free (name);
          obj = dwg_obj_generic_to_object (_obj, &error);
          if (obj)
            {
              obj->size = tbl->size;
              obj->address = pos;
            }
          ctrl = dwg_get_first_object (dwg, DWG_TYPE_BLOCK_CONTROL);
          if (ctrl)
            {
              _ctrl = ctrl->tio.object->tio.BLOCK_CONTROL;
              _ctrl->entries[i]
                  = dwg_add_handleref (dwg, 2, obj->handle.value, obj);
            }

          // TODO move to => dwg.spec
          SINCE (R_11)
          {
            FIELD_RSd (used, 0); // -1
          }
          FIELD_RLx (block_offset_r11, 0);
          if (tbl->size == 38)
            FIELD_RC (unknown_r11, 0);
          SINCE (R_11)
          {
            FIELD_HANDLE (block_entity, 2, 0);
            FIELD_RC (flag2, 0);
            FIELD_RC (unknown_r11, 0);
          }
          CHK_ENDPOS;
        }
      break;

    case SECTION_LAYER:
      for (i = 0; i < tbl->number; i++)
        {
          Bit_Chain *str_dat = dat;
          PREP_TABLE (LAYER);
          FIELD_CMC (color, 62); // off if negative
          FIELD_HANDLE (ltype, 2, 6);
          if (tbl->size == 38)
            FIELD_RC (flag0, 0);
          CHK_ENDPOS;
        }
      break;

    // was a text STYLE table, became a STYLE object
    case SECTION_STYLE:
      for (i = 0; i < tbl->number; i++)
        {
          PREP_TABLE (STYLE);
          FIELD_RD (text_size, 40); // ok
          FIELD_RD (width_factor, 41);
          FIELD_RD (oblique_angle, 50);
          FIELD_RC (generation, 71);
          LOG_TEXT_GENERATION
          FIELD_RD (last_height, 42);
          FIELD_TFv (font_file, 64, 3)                       // 8ed
          SINCE (R_2_4)
            FIELD_TFv (bigfont_file, 64, 4); // 92d
          CHK_ENDPOS;
        }
      break;

    case SECTION_LTYPE:
      {
        for (i = 0; i < tbl->number; i++)
          {
            Bit_Chain abs_dat = *dat;
            bit_reset_chain (dat);
            {
              PREP_TABLE (LTYPE);
              FIELD_TFv (description, 48, 3);
              FIELD_RC (alignment, 72);
              FIELD_RCu (num_dashes, 73); //
              FIELD_RD (pattern_len, 40);
#  ifndef IS_JSON
              FIELD_VECTOR_INL (dashes_r11, RD, 12, 49);
#  else
              FIELD_VECTOR_N (dashes_r11, RD, 12, 49);
#  endif
              if (dwg->header.version < R_11 && tbl->size > 187)
                FIELD_RC (unknown_r11, 0);
            }
            pos = dat->byte;
            *dat = abs_dat;
            dat->byte += pos;
            CHK_ENDPOS;
          }
      }
      break;

    case SECTION_VIEW:
      {
        for (i = 0; i < tbl->number; i++)
          {
            PREP_TABLE (VIEW);
            FIELD_RD (VIEWSIZE, 40);
            FIELD_2RD (VIEWCTR, 10);
            if (obj->size == 58)
              FIELD_RC (unknown_r11, 0);
            if (tbl->size > 58)
              FIELD_RD (view_width, 41);
            if (tbl->size > 66)
              FIELD_3RD (VIEWDIR, 11);
            if (tbl->size > 89)
              FIELD_RS (flag_3d, 0);
            if (tbl->size == 66 || tbl->size == 92)
              FIELD_RC (unknown_r2, 0);
            SINCE (R_10)
            {
              FIELD_3RD (view_target, 12);
              FIELD_CAST (VIEWMODE, RS, 4BITS, 71);
              FIELD_RD (lens_length, 42);
              FIELD_RD (front_clip_z, 43);
              FIELD_RD (back_clip_z, 44);
              FIELD_RD (twist_angle, 50);
            }
            CHK_ENDPOS;
          }
      }
      break;

    // SINCE R_11
    case SECTION_UCS:
      {
        for (i = 0; i < tbl->number; i++)
          {
            Dwg_Object *obj;
            Dwg_Object_UCS *_obj;
            dwg_point_3d ucsorg, ucsxdir, ucsydir;
            Dwg_Object *ctrl
                = dwg_get_first_object (dwg, DWG_TYPE_UCS_CONTROL);
            Dwg_Object_UCS_CONTROL *_ctrl
                = ctrl ? ctrl->tio.object->tio.UCS_CONTROL : NULL;
            if (!ctrl || dat->byte > dat->size || (num + i) > dwg->num_objects)
              return DWG_ERR_INVALIDDWG;
            LOG_TRACE ("\n-- table entry UCS [%d]: 0x%lx\n", i, pos);
            flag = bit_read_RC (dat);
            name = bit_read_TF (dat, 32);
            if (!name)
              return DWG_ERR_INVALIDDWG;
            SINCE (R_11)
              used = (BITCODE_RSd)bit_read_RS (dat);
            ucsorg.x = bit_read_RD (dat);
            ucsorg.y = bit_read_RD (dat);
            ucsorg.z = bit_read_RD (dat);
            ucsxdir.x = bit_read_RD (dat);
            ucsxdir.y = bit_read_RD (dat);
            ucsxdir.z = bit_read_RD (dat);
            ucsydir.x = bit_read_RD (dat);
            ucsydir.y = bit_read_RD (dat);
            ucsydir.z = bit_read_RD (dat);
            _obj = dwg_add_UCS (dwg, &ucsorg, &ucsxdir, &ucsydir,
                                (const char *)name);
            obj = dwg_obj_generic_to_object (_obj, &error);
            _ctrl->entries[i]
                = dwg_add_handleref (dwg, 2, obj->handle.value, obj);
            obj->size = tbl->size;
            obj->address = pos;
            _obj->flag = flag;
            _obj->used = used;
            LOG_TRACE ("flag: %u [RC 70]\n", flag);
            LOG_FLAG_UCS
            LOG_TRACE ("name: \"%s\" [TF 32 2]\n", name);
            LOG_TRACE ("used: " FORMAT_RSd " [RSd %d]\n", _obj->used, 0);
            LOG_TRACE ("ucsorg: (%f %f %f) [3RD]\n", ucsorg.x, ucsorg.y, ucsorg.z);
            LOG_TRACE ("ucsxdir: (%f %f %f) [3RD]\n", ucsxdir.x, ucsxdir.y, ucsxdir.z);
            LOG_TRACE ("ucsydir: (%f %f %f) [3RD]\n", ucsydir.x, ucsydir.y, ucsydir.z);
            free (name);

            CHK_ENDPOS;
          }
        break;

      // SINCE R_11
      case SECTION_VPORT:
        {
          for (i = 0; i < tbl->number; i++)
            {
              PREP_TABLE (VPORT);
              FIELD_2RD (lower_left, 10);
              FIELD_2RD (upper_right, 11);
              FIELD_3RD (view_target, 17);
              FIELD_3RD (VIEWDIR, 16);
              FIELD_RD (view_twist, 51);
              FIELD_RD (VIEWSIZE, 40);
              FIELD_2RD (VIEWCTR, 12);
              FIELD_RD (aspect_ratio, 41);
              DECODER {
                FIELD_VALUE (view_width) = FIELD_VALUE (aspect_ratio) * FIELD_VALUE (VIEWSIZE);
                LOG_TRACE ("view_width: %f (calc)\n", FIELD_VALUE (view_width))
              }
              FIELD_RD (lens_length, 42);
              FIELD_RD (front_clip_z, 43);
              FIELD_RD (back_clip_z, 44);
              FIELD_RS (UCSFOLLOW, 71);
              FIELD_RS (circle_zoom, 72); //circle sides
              FIELD_RS (FASTZOOM, 73);
              FIELD_RS (UCSICON, 74);
              FIELD_RS (SNAPMODE, 75);
              FIELD_RS (GRIDMODE, 76);
              FIELD_RS (SNAPSTYLE, 77);
              FIELD_RS (SNAPISOPAIR, 78);
              FIELD_RD (SNAPANG, 50);
              FIELD_2RD (SNAPBASE, 13);
              FIELD_2RD (SNAPUNIT, 14);
              FIELD_2RD (GRIDUNIT, 15);
              CHK_ENDPOS;
            }
        }
        break;

      // SINCE R_11
      case SECTION_APPID:
        {
          for (i = 0; i < tbl->number; i++)
            {
              PREP_TABLE (APPID);
              CHK_ENDPOS;
            }
        }
        break;

      // SINCE R_11
      case SECTION_DIMSTYLE:
        {
          for (i = 0; i < tbl->number; i++)
            {
              // unsigned long off;
              PREP_TABLE (DIMSTYLE);   // d1f
              FIELD_RD (DIMSCALE, 40); // d42
              FIELD_RD (DIMASZ, 41);
              FIELD_RD (DIMEXO, 42);
              FIELD_RD (DIMDLI, 43);
              FIELD_RD (DIMEXE, 44);
              FIELD_RD (DIMRND, 45);
              FIELD_RD (DIMDLE, 46);
              FIELD_RD (DIMTP, 47);
              FIELD_RD (DIMTM, 48); // ok
              FIELD_RD (DIMTXT, 140);
              FIELD_RD (DIMCEN, 141); // ok
              FIELD_RD (DIMTSZ, 142);
              FIELD_RD (DIMALTF, 143);
              FIELD_RD (DIMLFAC, 144);
              FIELD_RD (DIMTVP, 145); // db2
              FIELD_RC (DIMTOL, 71);  // dba
              FIELD_RC (DIMLIM, 72);  // dbb
              FIELD_RC (DIMTIH, 73);
              FIELD_RC (DIMTOH, 74);
              FIELD_RC (DIMSE1, 75);
              FIELD_RC (DIMSE2, 76);
              FIELD_CAST (DIMTAD, RC, RS, 77); // ok
              FIELD_CAST (DIMZIN, RC, BS, 78); // dc1
              FIELD_RC (DIMALT, 170);
              FIELD_CAST (DIMALTD, RC, BS, 171); // ok
              FIELD_RC (DIMTOFL, 172);           // ok
              FIELD_RC (DIMSAH, 173);            // ok
              FIELD_RC (DIMTIX, 174);            // ok
              FIELD_RC (DIMSOXD, 175);           // ok
              FIELD_TFv (DIMPOST, 16, 3);        // ok dc8
              FIELD_TFv (DIMAPOST, 16, 4);       // dd8
              FIELD_TFv (DIMBLK_T, 16, 5);       //?? unsupported by ODA
              FIELD_TFv (DIMBLK1_T, 16, 6);      //?? unsupported by ODA
              FIELD_TFv (DIMBLK2_T, 66, 7);      //?? unsupported by ODA
              // DEBUG_HERE; //e18
              // dat->byte += 50; //unknown: DIMSHO, DIMASO (global)
              FIELD_RS (DIMCLRD_N, 176); // e4a
              FIELD_RS (DIMCLRE_N, 177);
              FIELD_RS (DIMCLRT_N, 178); // e4e
              FIELD_RC (DIMUPT, 0);      //??
              FIELD_RD (DIMTFAC, 146);   // e51
              FIELD_RD (DIMGAP, 147);    // e59
              CHK_ENDPOS;                //-e63
            }
        }
        break;

#undef LOG_FLAG_W
#undef LOG_FLAG_TABLE_W
#undef LOG_FLAG_TABLE_MAX

      // SINCE R_11
      case SECTION_VX:
        {
          if (tbl->number)
            {
              LOG_WARN ("VX table ignored");
              tbl->number = 0;
            }
        }
        break;

      case SECTION_HEADER_R11:
      default:
        LOG_ERROR ("Invalid table id %d", id);
        tbl->number = 0;
        break;
      }
    }
  if (tbl->address && tbl->number && tbl->size)
    dat->byte = tbl->address + (tbl->number * tbl->size);
  else
    dat->byte = oldpos;
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
  int error = 0;

  obj->bitsize_pos = bit_position (dat);
  obj->address
      = dat->byte - 1; // already read the type. size includes the type
  LOG_INFO ("===========================\n"
            "Entity number: %d, Type: %d, Addr: %lx\n",
            obj->index, obj->type, obj->address);
  _obj->entmode = is_block ? 3 : 2; // ent or block

#  include "common_entity_data.spec"

  obj->common_size = bit_position (dat) - obj->bitsize_pos;
  return 0;
}

#else  // USE_WRITE
EXPORT int
decode_entity_preR13 (Bit_Chain *restrict dat, Dwg_Object *restrict obj,
                      Dwg_Object_Entity *_ent)
{
  return DWG_ERR_INTERNALERROR;
}
EXPORT Dwg_Object_Ref *
dwg_decode_preR13_handleref (Bit_Chain *restrict dat, int size)
{
  return NULL;
}
#endif // USE_WRITE

AFL_GCC_TOOBIG
EXPORT int
decode_preR13 (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  BITCODE_RL rl1, rl2, num_entities;
  BITCODE_RL blocks_start = 0, blocks_end = 0;
  BITCODE_RS rs2;
  Dwg_Object *obj = NULL;
  int tbl_id;
  int error = 0;
  Bit_Chain dat_save = *dat;

  loglevel = dat->opts & DWG_OPTS_LOGLEVEL;
#ifndef USE_WRITE
  fprintf (stderr, "Cannot create pre-R13 documents with --disable-write\n");
  return DWG_ERR_INTERNALERROR;
#else
  {
    int i;
    Dwg_Header *_obj = (Dwg_Header *)&dwg->header;
    Bit_Chain *hdl_dat = dat;
    dat->byte = 0x06;
    // clang-format off
    #include "header.spec"
    // clang-format on
  }
  LOG_TRACE ("@0x%lx\n", dat->byte); // 0x14

  // setup all the new control objects
  error |= dwg_add_Document (dwg, 0);
  if (error >= DWG_ERR_CRITICAL)
    return error;

  SINCE (R_2_0b)
  {
    blocks_start = dwg->header.blocks_start;
    blocks_end = dwg->header.blocks_end;
    if (blocks_start > 0xffff)
      {
        blocks_start &= 0xffff;
        LOG_TRACE ("blocks_start => " FORMAT_RLx "\n", blocks_start);
      }
    if (dwg->header.blocks_end == 0 && dwg->header.blocks_size != 0
      && dwg->header.blocks_size != 0x40000000)
      {
        blocks_end = blocks_start + dwg->header.blocks_size;
        LOG_TRACE ("blocks_end => " FORMAT_RLx "\n", blocks_end);
      }

    tbl_id = 0;
    dwg->header.section[0].number = 0;
    dwg->header.section[0].type = (Dwg_Section_Type)SECTION_HEADER_R11;
    strcpy (dwg->header.section[0].name, "HEADER");

    // The 5 tables (num_sections always 5): 3 RS + 1 RL address
    LOG_INFO ("==========================================\n")
    dat_save = *dat;
    if (decode_preR13_section_hdr ("BLOCK", SECTION_BLOCK, dat, dwg)
        || decode_preR13_section_hdr ("LAYER", SECTION_LAYER, dat, dwg)
        || decode_preR13_section_hdr ("STYLE", SECTION_STYLE, dat, dwg)
        || decode_preR13_section_hdr ("LTYPE", SECTION_LTYPE, dat, dwg)
        || decode_preR13_section_hdr ("VIEW", SECTION_VIEW, dat, dwg))
      return DWG_ERR_SECTIONNOTFOUND;
  }
  LOG_TRACE ("@0x%lx\n", dat->byte); // 0x5e
  if (dat->size < 0x1f0)             // AC1.50 0x1f9 74 vars
    {
      LOG_ERROR ("DWG too small %zu", (size_t)dat->size)
      return DWG_ERR_INVALIDDWG;
    }

  LOG_INFO ("==========================================\n")
  error |= decode_preR13_header_variables (dat, dwg);
  LOG_TRACE ("@0x%lx\n", dat->byte);
  if (error >= DWG_ERR_CRITICAL)
    return error;
  if (dat->byte + 2 >= dat->size)
    {
      LOG_ERROR ("post HEADER overflow")
      return error | DWG_ERR_CRITICAL;
    }
  SINCE (R_11)
  {
    // crc16 + DWG_SENTINEL_R11_HEADER_END
    BITCODE_RS crc, crcc;
    BITCODE_TF r11_sentinel;
    crcc = bit_calc_CRC (0xC0C1, &dat->chain[0], dat->byte); // from 0 to now
    crc = bit_read_RS (dat);
    LOG_TRACE ("crc: %04X [RSx] from 0-0x%lx\n", crc, dat->byte - 2);
    if (crc != crcc)
      {
        LOG_ERROR ("Header CRC mismatch %04X <=> %04X", crc, crcc);
        error |= DWG_ERR_WRONGCRC;
      }
    r11_sentinel = bit_read_TF (dat, 16);
    if (!r11_sentinel)
      return error | DWG_ERR_INVALIDDWG;
    LOG_TRACE ("r11_sentinel: ");
    LOG_TRACE_TF (r11_sentinel, 16) // == C46E6854F86E3330633EC1852ADC9401
    if (memcmp (r11_sentinel, dwg_sentinel (DWG_SENTINEL_R11_HEADER_END), 16))
      {
        LOG_ERROR ("DWG_SENTINEL_R11_HEADER_END mismatch");
        error |= DWG_ERR_WRONGCRC;
      }
    free (r11_sentinel);
  }

  PRE (R_10)
    num_entities = dwg->header_vars.numentities;
  else
    num_entities = 0;
  PRE (R_2_0b)
  {
    dwg->header.entities_start = dat->byte;
    dwg->header.entities_end = dwg->header_vars.dwg_size;
  }

  // entities
  if (dat->byte != dwg->header.entities_start)
    {
      LOG_WARN ("@0x%lx => entities_start 0x%x", dat->byte,
                dwg->header.entities_start);
      if (dat->byte < dwg->header.entities_start)
        {
          _DEBUG_HERE (dat->byte - dwg->header.entities_start)
        }
      dat->byte = dwg->header.entities_start;
    }
  error |= decode_preR13_entities (
      dwg->header.entities_start, dwg->header.entities_end, num_entities,
      dwg->header.entities_end - dwg->header.entities_start, 0, dat, dwg);
  if (error >= DWG_ERR_CRITICAL)
    return error;
  if (dat->byte != dwg->header.entities_end)
    {
      LOG_WARN ("@0x%lx => entities_end 0x%x", dat->byte, dwg->header.entities_end);
      dat->byte = dwg->header.entities_end;
    }
  PRE (R_2_0b)
  {
    // this has usually some slack at the end.
    return error;
  }
  LOG_INFO ("==========================================\n")
  // dat->byte += 20; /* crc + sentinel? 20 byte */
  if (!dwg->next_hdl)
    dwg_set_next_hdl (dwg, 0x22);
  dat_save = *dat;
  if (decode_preR13_section (SECTION_BLOCK, dat, dwg)
      || decode_preR13_section (SECTION_LAYER, dat, dwg)
      || decode_preR13_section (SECTION_STYLE, dat, dwg)
      || decode_preR13_section (SECTION_LTYPE, dat, dwg)
      || decode_preR13_section (SECTION_VIEW, dat, dwg))
    {
      *dat = dat_save;
      return DWG_ERR_SECTIONNOTFOUND;
    }
  if (dwg->header.num_sections >= SECTION_VPORT) // r10
    {
      dat_save = *dat;
      if (decode_preR13_section (SECTION_UCS, dat, dwg)
          || decode_preR13_section (SECTION_VPORT, dat, dwg))
        {
          *dat = dat_save;
          return DWG_ERR_SECTIONNOTFOUND;
        }
    }
  if (dwg->header.num_sections >= SECTION_APPID) // r10
    {
      dat_save = *dat;
      if (decode_preR13_section (SECTION_APPID, dat, dwg))
        {
          *dat = dat_save;
          return DWG_ERR_SECTIONNOTFOUND;
        }
    }
  if (dwg->header.num_sections >= SECTION_VX) // r11
    {
      dat_save = *dat;
      if (decode_preR13_section (SECTION_DIMSTYLE, dat, dwg)
          || decode_preR13_section (SECTION_VX, dat, dwg))
        {
          *dat = dat_save;
          return DWG_ERR_SECTIONNOTFOUND;
        }
    }
  if (error >= DWG_ERR_CRITICAL)
    return error;

  // block entities
  if (dat->byte != blocks_start)
    {
      BITCODE_TF unknown;
      int len = blocks_start - dat->byte;
      LOG_WARN ("\n@0x%lx => blocks_start 0x%x", dat->byte, blocks_start);
      if (dat->byte < blocks_start)
        {
          unknown = bit_read_TF (dat, len);
          LOG_TRACE ("unknown (%d):", len);
          LOG_TRACE_TF (unknown, len);
          free (unknown);
        }
      dat->byte = blocks_start;
    }
  if (dwg->header.blocks_size != 0 && dwg->header.blocks_size != 0x40000000)
    {
      if (dwg->header.version == R_11 && blocks_start + 32 < blocks_end)
        dwg->header.blocks_end -= 32; // ?? some sentinel probably
      error |= decode_preR13_entities (blocks_start, blocks_end, 0,
                                       dwg->header.blocks_size & 0x3FFFFFFF,
                                       dwg->header.blocks_max, dat, dwg);
      if (error >= DWG_ERR_CRITICAL)
        return error;
    }

  PRE (R_11) { return error; }
  // only since r11 (AC1009)
  LOG_TRACE ("AUXHEADER: @0x%lx\n", dat->byte);
  // 36 byte: 9x long
  rl1 = bit_read_RL (dat);
  rl2 = bit_read_RL (dat);
  LOG_TRACE ("?2long: 0x%x 0x%x %f\n", rl1, rl2,
             (double)dat->chain[dat->byte - 8]);
  rl1 = bit_read_RL (dat);
  rl2 = bit_read_RL (dat);
  LOG_TRACE ("?2long: 0x%x 0x%x %f\n", rl1, rl2,
             (double)dat->chain[dat->byte - 8]);
  rl1 = bit_read_RL (dat);
  rl2 = bit_read_RL (dat);
  LOG_TRACE ("?2long: 0x%x 0x%x %f\n", rl1, rl2,
             (double)dat->chain[dat->byte - 8]);
  rl1 = bit_read_RL (dat);
  rl2 = bit_read_RL (dat);
  LOG_TRACE ("?2long: 0x%x 0x%x %f\n", rl1, rl2,
             (double)dat->chain[dat->byte - 8]);
  rl1 = bit_read_RL (dat);
  LOG_TRACE ("?1long: 0x%x\n", rl1);

  LOG_TRACE ("@0x%lx: 4 block ptrs chk\n", dat->byte);
  if ((rl1 = bit_read_RL (dat)) != dwg->header.entities_start)
    {
      LOG_WARN ("entities_start %x/%x", rl1, dwg->header.entities_start);
    }
  if ((rl1 = bit_read_RL (dat)) != dwg->header.entities_end)
    {
      LOG_WARN ("entities_end %x/%x", rl1, dwg->header.entities_end);
    }
  if ((rl1 = bit_read_RL (dat)) != dwg->header.blocks_start)
    {
      LOG_WARN ("blocks_start %x/%x", rl1, dwg->header.blocks_start);
    }
  if ((rl1 = bit_read_RL (dat)) != dwg->header.blocks_end)
    {
      LOG_WARN ("blocks_end %x/%x", rl1, dwg->header.blocks_end);
    }
  // 12 byte
  LOG_TRACE ("@0x%lx\n", dat->byte);
  rl1 = bit_read_RL (dat);
  rl2 = bit_read_RL (dat);
  LOG_TRACE ("?2long: 0x%x 0x%x\n", rl1, rl2);
  rl1 = bit_read_RL (dat);
  LOG_TRACE ("?1long: 0x%x\n", rl1);

  rl1 = dwg->header.blocks_end + 36 + 4 * 4 + 12; // ??
  DEBUG_HERE
  UNKNOWN_UNTIL (rl1);
  LOG_TRACE ("@0x%lx\n", dat->byte);
  decode_preR13_section_chk (SECTION_BLOCK, dat, dwg);
  decode_preR13_section_chk (SECTION_LAYER, dat, dwg);
  decode_preR13_section_chk (SECTION_STYLE, dat, dwg);
  decode_preR13_section_chk (SECTION_LTYPE, dat, dwg);
  decode_preR13_section_chk (SECTION_VIEW, dat, dwg);
  if (dwg->header.num_sections >= SECTION_VPORT)
    {
      decode_preR13_section_chk (SECTION_UCS, dat, dwg);
      decode_preR13_section_chk (SECTION_VPORT, dat, dwg);
    }
  if (dwg->header.num_sections >= SECTION_APPID)
    {
      decode_preR13_section_chk (SECTION_APPID, dat, dwg);
    }
  if (dwg->header.num_sections >= SECTION_VX)
    {
      decode_preR13_section_chk (SECTION_DIMSTYLE, dat, dwg);
      decode_preR13_section_chk (SECTION_VX, dat, dwg);
    }
  rl1 = bit_read_RL (dat);
  LOG_TRACE ("long 0x%x\n", rl1); // address
  if (dat->byte < dat->size)
    {
      int len = dat->size - dat->byte;
      BITCODE_TF unknown = bit_read_TF (dat, len);
      LOG_TRACE ("unknown (%d):", len);
      LOG_TRACE_TF (unknown, len);
      free (unknown);
    }
  if (dwg->dirty_refs)
    dwg_resolve_objectrefs_silent (dwg);
  return 0;
#endif // USE_WRITE
}
AFL_GCC_POP

#undef IS_DECODER
