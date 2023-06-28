/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2009-2024 Free Software Foundation, Inc.                   */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * dec_macros.h: decoding macros (for encode, print, free in the C files)
 * written by Felipe Castro
 * modified by Felipe Corrêa da Silva Sances
 * modified by Rodrigo Rodrigues da Silva
 * modified by Reini Urban
 * modified by Denis Pruchkovsky
 */

#ifndef DEC_MACROS_H
#define DEC_MACROS_H

#include "config.h"
#if defined HAVE_CTYPE_H || defined _MSC_VER
#  include <ctype.h>
#endif

#ifdef IN_DXF_H
#  error in_dxf.h must be included after dec_macros.h because of FORMAT_BD
#endif

// needed by decode, decode_r2007 and decode_r11
#define ACTION decode
#define IS_DECODER

// redeclare versions to be from, not target
#include "importer.h"

// different to out_json
#define ARGS_HREF11(ref) ref->handleref.size, ref->r11_idx, ref->absolute_ref
#define FORMAT_HREF11 "[%u, %hd, " FORMAT_RLLx "]"

#undef LOG_POS
#define LOG_POS                                                               \
  LOG_INSANE (" @%" PRIuSIZE ".%u",                                           \
              obj ? dat->byte - obj->address : dat->byte, (unsigned)dat->bit) \
  LOG_TRACE ("\n")
#define LOG_POS_(level)                                                       \
  LOG_INSANE (" @%" PRIuSIZE ".%u",                                           \
              obj ? dat->byte - obj->address : dat->byte, (unsigned)dat->bit) \
  LOG (level, "\n")

#define LOG_RPOS                                                              \
  LOG_INSANE (" @%" PRIuSIZE ".%u", dat->byte, (unsigned)dat->bit)            \
  LOG_TRACE ("\n")
#define LOG_RPOS_(level)                                                      \
  LOG_INSANE (" @%" PRIuSIZE ".%u", dat->byte, (unsigned)dat->bit)            \
  LOG (level, "\n")
#define LOG_HPOS                                                              \
  LOG_INSANE (" @%" PRIuSIZE ".%u",                                           \
              obj && hdl_dat->byte > obj->address                             \
                  ? hdl_dat->byte - obj->address                              \
                  : hdl_dat->byte,                                            \
              (unsigned)hdl_dat->bit)                                         \
  LOG_TRACE ("\n")
#define LOG_HPOS_(level)                                                      \
  LOG_INSANE (" @%" PRIuSIZE ".%u",                                           \
              obj && hdl_dat->byte > obj->address                             \
                  ? hdl_dat->byte - obj->address                              \
                  : hdl_dat->byte,                                            \
              (unsigned)hdl_dat->bit)                                         \
  LOG (level, "\n")

#define VALUE(value, type, dxf)                                               \
  (void)bit_read_##type (dat);                                                \
  LOG_TRACE (FORMAT_##type " [" #type " %d]\n", (BITCODE_##type)value, dxf)
#define VALUE_RC(value, dxf) VALUE (value, RC, dxf)
#define VALUE_BS(value, dxf) VALUE (value, BS, dxf)
#define VALUE_BL(value, dxf) VALUE (value, BL, dxf)
#define VALUE_RS(value, dxf) VALUE (value, RS, dxf)
#define VALUE_RL(value, dxf) VALUE (value, RL, dxf)
#define VALUE_RLx(value, dxf) VALUE (value, RL, dxf)
#define VALUE_RD(value, dxf) VALUE (value, RD, dxf)
#define VALUE_BD(value, dxf) VALUE (value, BD, dxf)

#define FIELDG(nam, type, dxf)                                                \
  {                                                                           \
    _obj->nam = bit_read_##type (dat);                                        \
    FIELD_G_TRACE (nam, type, dxf);                                           \
  }

#define FIELD(nam, type)                                                      \
  {                                                                           \
    _obj->nam = bit_read_##type (dat);                                        \
    FIELD_TRACE (nam, type);                                                  \
  }
#define SUB_FIELD(o, nam, type, dxf)                                          \
  {                                                                           \
    _obj->o.nam = bit_read_##type (dat);                                      \
    FIELD_G_TRACE (o.nam, type, dxf);                                         \
  }

#define FIELD_CAST(nam, type, cast, dxf)                                      \
  {                                                                           \
    _obj->nam = (BITCODE_##cast)bit_read_##type (dat);                        \
    FIELD_G_TRACE (nam, cast, dxf);                                           \
  }
#define SUB_FIELD_CAST(o, nam, type, cast, dxf)                               \
  {                                                                           \
    _obj->o.nam = (BITCODE_##cast)bit_read_##type (dat);                      \
    FIELD_G_TRACE (o.nam, cast, dxf);                                         \
  }

#define FIELD_G_TRACE(nam, type, dxfgroup)                                    \
  if (DWG_LOGLEVEL >= DWG_LOGLEVEL_TRACE)                                     \
    {                                                                         \
      char *s1 = strrplc (#nam, "[rcount1]", "[%d]");                         \
      if (s1)                                                                 \
        {                                                                     \
          char *s2 = strrplc (s1, "[rcount2]", "[%d]");                       \
          if (s2)                                                             \
            {                                                                 \
              GCC46_DIAG_IGNORE (-Wformat-nonliteral)                         \
              LOG_TRACE (strcat (s2, ": " FORMAT_##type " [" #type " %d]"),   \
                         rcount1, rcount2, _obj->nam, dxfgroup);              \
              GCC46_DIAG_RESTORE                                              \
              free (s2);                                                      \
              free (s1);                                                      \
            }                                                                 \
          else                                                                \
            {                                                                 \
              GCC46_DIAG_IGNORE (-Wformat-nonliteral)                         \
              LOG_TRACE (strcat (s1, ": " FORMAT_##type " [" #type " %d]"),   \
                         rcount1, _obj->nam, dxfgroup);                       \
              GCC46_DIAG_RESTORE                                              \
              free (s1);                                                      \
            }                                                                 \
        }                                                                     \
      else                                                                    \
        LOG_TRACE (#nam ": " FORMAT_##type " [" #type " %d]",                 \
                   (BITCODE_##type)_obj->nam, dxfgroup);                      \
      LOG_RPOS                                                                \
    }

#define FIELD_G_TRACE_ANGLE(nam, type, dxfgroup)                              \
  if (DWG_LOGLEVEL >= DWG_LOGLEVEL_TRACE)                                     \
    {                                                                         \
      char *s1 = strrplc (#nam, "[rcount1]", "[%d]");                         \
      if (s1)                                                                 \
        {                                                                     \
          char *s2 = strrplc (s1, "[rcount2]", "[%d]");                       \
          if (s2)                                                             \
            {                                                                 \
              GCC46_DIAG_IGNORE (-Wformat-nonliteral)                         \
              LOG_TRACE (strcat (s2, ": " FORMAT_##type " [" #type            \
                                 " %d] %gº"),                                 \
                         rcount1, rcount2, _obj->nam, dxfgroup,               \
                         rad2deg (_obj->nam));                                \
              GCC46_DIAG_RESTORE                                              \
              free (s2);                                                      \
              free (s1);                                                      \
            }                                                                 \
          else                                                                \
            {                                                                 \
              GCC46_DIAG_IGNORE (-Wformat-nonliteral)                         \
              LOG_TRACE (strcat (s1, ": " FORMAT_##type " [" #type            \
                                 " %d] %gº"),                                 \
                         rcount1, _obj->nam, dxfgroup, rad2deg (_obj->nam));  \
              GCC46_DIAG_RESTORE                                              \
              free (s1);                                                      \
            }                                                                 \
        }                                                                     \
      else                                                                    \
        LOG_TRACE (#nam ": " FORMAT_##type " [" #type " %d] %gº",             \
                   (BITCODE_##type)_obj->nam, dxfgroup, rad2deg (_obj->nam)); \
      LOG_RPOS                                                                \
    }

#define FIELD_TRACE(nam, type)                                                \
  if (DWG_LOGLEVEL >= DWG_LOGLEVEL_TRACE)                                     \
    {                                                                         \
      char *s1 = strrplc (#nam, "[rcount1]", "[%d]");                         \
      if (s1)                                                                 \
        {                                                                     \
          char *s2 = strrplc (s1, "[rcount2]", "[%d]");                       \
          if (s2)                                                             \
            {                                                                 \
              GCC46_DIAG_IGNORE (-Wformat-nonliteral)                         \
              LOG_TRACE (strcat (s2, ": " FORMAT_##type " " #type "\n"),      \
                         rcount1, rcount2, _obj->nam)                         \
              GCC46_DIAG_RESTORE                                              \
              free (s2);                                                      \
              free (s1);                                                      \
            }                                                                 \
          else                                                                \
            {                                                                 \
              GCC46_DIAG_IGNORE (-Wformat-nonliteral)                         \
              LOG_TRACE (strcat (s1, ": " FORMAT_##type " " #type "\n"),      \
                         rcount1, _obj->nam)                                  \
              GCC46_DIAG_RESTORE                                              \
              free (s1);                                                      \
            }                                                                 \
        }                                                                     \
      else                                                                    \
        {                                                                     \
          LOG_TRACE (#nam ": " FORMAT_##type " [" #type "]\n", _obj->nam)     \
        }                                                                     \
    }
#define LOG_TF(level, var, len)                                               \
  {                                                                           \
    if (var && (DWG_LOGLEVEL >= DWG_LOGLEVEL_##level || len <= 256))          \
      {                                                                       \
        for (unsigned _i = 0; _i < (unsigned)(len); _i++)                     \
          {                                                                   \
            LOG (level, "%02X", (unsigned char)((char *)var)[_i]);            \
          }                                                                   \
        LOG (level, "\n");                                                    \
      }                                                                       \
    if (var && (DWG_LOGLEVEL >= DWG_LOGLEVEL_INSANE))                         \
      {                                                                       \
        for (unsigned _i = 0; _i < (unsigned)(len); _i++)                     \
          {                                                                   \
            unsigned char c = ((unsigned char *)var)[_i];                     \
            LOG_INSANE ("%-2c", isprint (c) ? c : ' ');                       \
          }                                                                   \
        LOG_INSANE ("\n");                                                    \
      }                                                                       \
  }
#define LOG_TRACE_TF(var, len) LOG_TF (TRACE, var, len)
#define LOG_INSANE_TF(var, len) LOG_TF (INSANE, var, len)

#define FIELD_VEC_TRACE_N(nam, type, vcount, value, dxf)                      \
  if (DWG_LOGLEVEL >= DWG_LOGLEVEL_TRACE)                                     \
    {                                                                         \
      char *s1 = strrplc (#nam, "[rcount1]", "[%d]");                         \
      if (s1)                                                                 \
        {                                                                     \
          char *s2 = strrplc (s1, "[rcount2]", "[%d]");                       \
          if (s2)                                                             \
            {                                                                 \
              GCC46_DIAG_IGNORE (-Wformat-nonliteral)                         \
              LOG_TRACE (                                                     \
                  strcat (s2, "[%ld]: " FORMAT_##type " [" #type " %d]"),     \
                  rcount1, rcount2, vcount, value, dxf)                       \
              GCC46_DIAG_RESTORE                                              \
              free (s2);                                                      \
              free (s1);                                                      \
            }                                                                 \
          else                                                                \
            {                                                                 \
              GCC46_DIAG_IGNORE (-Wformat-nonliteral)                         \
              LOG_TRACE (                                                     \
                  strcat (s1, "[%ld]: " FORMAT_##type " [" #type " %d]"),     \
                  rcount1, vcount, value, dxf)                                \
              GCC46_DIAG_RESTORE                                              \
              free (s1);                                                      \
            }                                                                 \
        }                                                                     \
      else                                                                    \
        {                                                                     \
          LOG_TRACE (#nam "[%ld]: " FORMAT_##type " [" #type " %d]", vcount,  \
                     value, dxf)                                              \
        }                                                                     \
      LOG_POS;                                                                \
    }
#define FIELD_2PT_TRACE(nam, type, dxf)                                       \
  if (DWG_LOGLEVEL >= DWG_LOGLEVEL_TRACE)                                     \
    {                                                                         \
      char *s1 = strrplc (#nam, "[rcount1]", "[%d]");                         \
      if (s1)                                                                 \
        {                                                                     \
          char *s2 = strrplc (s1, "[rcount2]", "[%d]");                       \
          if (s2)                                                             \
            {                                                                 \
              GCC46_DIAG_IGNORE (-Wformat-nonliteral)                         \
              LOG_TRACE (strcat (s2, ": (" FORMAT_BD ", " FORMAT_BD           \
                                     ") [" #type " %d]"),                     \
                         rcount1, rcount2, _obj->nam.x, _obj->nam.y, dxf)     \
              GCC46_DIAG_RESTORE                                              \
              free (s2);                                                      \
              free (s1);                                                      \
            }                                                                 \
          else                                                                \
            {                                                                 \
              GCC46_DIAG_IGNORE (-Wformat-nonliteral)                         \
              LOG_TRACE (strcat (s1, ": (" FORMAT_BD ", " FORMAT_BD           \
                                     ") [" #type " %d]"),                     \
                         rcount1, _obj->nam.x, _obj->nam.y, dxf)              \
              GCC46_DIAG_RESTORE                                              \
              free (s1);                                                      \
            }                                                                 \
        }                                                                     \
      else                                                                    \
        {                                                                     \
          LOG_TRACE (#nam ": (" FORMAT_BD ", " FORMAT_BD ") [" #type " %d]",  \
                     _obj->nam.x, _obj->nam.y, dxf)                           \
        }                                                                     \
      LOG_POS;                                                                \
    }
#define FIELD_3PT_TRACE(nam, type, dxf)                                       \
  if (DWG_LOGLEVEL >= DWG_LOGLEVEL_TRACE)                                     \
    {                                                                         \
      char *s1 = strrplc (#nam, "[rcount1]", "[%d]");                         \
      if (s1)                                                                 \
        {                                                                     \
          char *s2 = strrplc (s1, "[rcount2]", "[%d]");                       \
          if (s2)                                                             \
            {                                                                 \
              GCC46_DIAG_IGNORE (-Wformat-nonliteral)                         \
              LOG_TRACE (strcat (s2, ": (" FORMAT_BD ", " FORMAT_BD           \
                                     ", " FORMAT_BD ") [" #type " %d]"),      \
                         rcount1, rcount2, _obj->nam.x, _obj->nam.y,          \
                         _obj->nam.z, dxf)                                    \
              GCC46_DIAG_RESTORE                                              \
              free (s2);                                                      \
              free (s1);                                                      \
            }                                                                 \
          else                                                                \
            {                                                                 \
              GCC46_DIAG_IGNORE (-Wformat-nonliteral)                         \
              LOG_TRACE (strcat (s1, ": (" FORMAT_BD ", " FORMAT_BD           \
                                     ", " FORMAT_BD ") [" #type " %d]"),      \
                         rcount1, _obj->nam.x, _obj->nam.y, _obj->nam.z, dxf) \
              GCC46_DIAG_RESTORE                                              \
              free (s1);                                                      \
            }                                                                 \
        }                                                                     \
      else                                                                    \
        {                                                                     \
          LOG_TRACE (#nam ": (" FORMAT_BD ", " FORMAT_BD ", " FORMAT_BD       \
                          ") [" #type " %d]",                                 \
                     _obj->nam.x, _obj->nam.y, _obj->nam.z, dxf)              \
        }                                                                     \
      LOG_POS;                                                                \
    }

#define ANYCODE -1
#define VALUE_HANDLE(ref, nam, code, dxf)                                     \
  {                                                                           \
    PRE (R_13b1)                                                              \
    {                                                                         \
      size_t _pos = bit_position (dat);                                       \
      if (ref)                                                                \
        free (ref);                                                           \
      ref = dwg_decode_preR13_handleref (dat, code /*as size */, dwg);        \
      LOG_TRACE (#nam ": %hd [H(%s) %d]", (short)ref->r11_idx,                \
                 code == 1 ? "RC" : "RSd", dxf)                               \
      LOG_INSANE (" @%" PRIuSIZE ".%u", _pos / 8, (unsigned)(_pos % 8));      \
      LOG_TRACE ("\n");                                                       \
    }                                                                         \
    LATER_VERSIONS                                                            \
    {                                                                         \
      size_t _pos = bit_position (hdl_dat);                                   \
      if (code >= 0)                                                          \
        ref = dwg_decode_handleref_with_code (hdl_dat, obj, dwg, code);       \
      else                                                                    \
        ref = dwg_decode_handleref (hdl_dat, obj, dwg);                       \
      if (DWG_LOGLEVEL >= DWG_LOGLEVEL_TRACE)                                 \
        {                                                                     \
          if (ref)                                                            \
            {                                                                 \
              LOG_TRACE (#nam ": " FORMAT_REF " [H %d]", ARGS_REF (ref),      \
                         dxf);                                                \
              if (dwg_ref_object_silent (dwg, ref)                            \
                  && DWG_LOGLEVEL > DWG_LOGLEVEL_TRACE)                       \
                {                                                             \
                  int alloced;                                                \
                  const char *u8 = dwg_ref_tblname (dwg, ref, &alloced);      \
                  if (u8 && *u8)                                              \
                    HANDLER (OUTPUT, " => %s %s", dwg_ref_objname (dwg, ref), \
                             u8);                                             \
                  if (u8 && alloced)                                          \
                    free ((void *)u8);                                        \
                }                                                             \
            }                                                                 \
          else                                                                \
            {                                                                 \
              HANDLER (OUTPUT, #nam ": NULL %d [H %d]", code, dxf);           \
            }                                                                 \
          LOG_INSANE (" @%" PRIuSIZE ".%u", _pos / 8, (unsigned)(_pos % 8));  \
          HANDLER (OUTPUT, "\n");                                             \
        }                                                                     \
    }                                                                         \
  }
#define FIELD_HANDLE(nam, code, dxf) VALUE_HANDLE (_obj->nam, nam, code, dxf)
#define SUB_FIELD_HANDLE(o, nam, code, dxf)                                   \
  VALUE_HANDLE (_obj->o.nam, o.nam, code, dxf)
#define VALUE_H(hdl, dxf)                                                     \
  {                                                                           \
    size_t pos;                                                               \
    PRE (R_13b1)                                                              \
    {                                                                         \
      pos = bit_position (dat);                                               \
      error |= bit_read_H (dat, &hdl);                                        \
    }                                                                         \
    else                                                                      \
    {                                                                         \
      pos = bit_position (hdl_dat);                                           \
      error |= bit_read_H (hdl_dat, &hdl);                                    \
    }                                                                         \
    LOG_TRACE ("handle: " FORMAT_H " [H %d]", ARGS_H (hdl), dxf);             \
    LOG_INSANE (" @%" PRIuSIZE ".%u", pos / 8, (unsigned)(pos % 8));          \
    LOG_TRACE ("\n");                                                         \
  }

#define VALUE_HANDLE_N(ref, nam, vcount, code, dxf)                           \
  {                                                                           \
    PRE (R_13b1)                                                              \
    {                                                                         \
      size_t pos = bit_position (dat);                                        \
      ref = dwg_decode_preR13_handleref (dat, code, dwg);                     \
      LOG_TRACE (#nam "[%d]: " FORMAT_RS " [RS %d]", (int)vcount,             \
                 ref->r11_idx, dxf);                                          \
      LOG_INSANE (" @%" PRIuSIZE ".%u", pos / 8, (unsigned)(pos % 8));        \
      LOG_TRACE ("\n");                                                       \
    }                                                                         \
    LATER_VERSIONS                                                            \
    {                                                                         \
      size_t pos = bit_position (hdl_dat);                                    \
      if (code >= 0)                                                          \
        ref = dwg_decode_handleref_with_code (hdl_dat, obj, dwg, code);       \
      else                                                                    \
        ref = dwg_decode_handleref (hdl_dat, obj, dwg);                       \
      if (DWG_LOGLEVEL >= DWG_LOGLEVEL_TRACE)                                 \
        {                                                                     \
          if (ref)                                                            \
            {                                                                 \
              char *_name = strrplc (#nam, "[vcount]", "");                   \
              LOG_TRACE ("%s[%d]: " FORMAT_REF " [H* %d]",                    \
                         _name ? _name : #nam, (int)vcount, ARGS_REF (ref),   \
                         dxf);                                                \
              if (_name)                                                      \
                free (_name);                                                 \
              if (dwg_ref_object_silent (dwg, ref)                            \
                  && DWG_LOGLEVEL > DWG_LOGLEVEL_TRACE)                       \
                {                                                             \
                  int alloced;                                                \
                  const char *u8 = dwg_ref_tblname (dwg, ref, &alloced);      \
                  HANDLER (OUTPUT, " => %s %s", dwg_ref_objname (dwg, ref),   \
                           u8);                                               \
                  if (alloced)                                                \
                    free ((void *)u8);                                        \
                }                                                             \
            }                                                                 \
          else                                                                \
            {                                                                 \
              LOG_TRACE (#nam "[%d]: NULL %d [H* %d]", (int)vcount, code,     \
                         dxf);                                                \
            }                                                                 \
          LOG_INSANE (" @%" PRIuSIZE ".%u", pos / 8, (unsigned)(pos % 8));    \
          LOG_TRACE ("\n");                                                   \
        }                                                                     \
    }                                                                         \
  }
#define FIELD_HANDLE_N(nam, vcount, code, dxf)                                \
  VALUE_HANDLE_N (_obj->nam, nam, vcount, code, dxf)

#define FIELD_DATAHANDLE(nam, code, dxf)                                      \
  {                                                                           \
    _obj->nam = dwg_decode_handleref (dat, obj, dwg);                         \
    if (_obj->nam)                                                            \
      {                                                                       \
        LOG_TRACE (#nam ": " FORMAT_H " [H %d]",                              \
                   ARGS_H (_obj->nam->handleref), dxf);                       \
        LOG_RPOS                                                              \
      }                                                                       \
  }

#define FIELD_B(nam, dxf) FIELDG (nam, B, dxf)
#define FIELD_BB(nam, dxf) FIELDG (nam, BB, dxf)
#define FIELD_3B(nam, dxf) FIELDG (nam, 3B, dxf)
#define FIELD_BS(nam, dxf) FIELDG (nam, BS, dxf)
#define FIELD_BL(nam, dxf) FIELDG (nam, BL, dxf)
#define FIELD_BLL(nam, dxf) FIELDG (nam, BLL, dxf)
#define FIELD_BD(nam, dxf)                                                    \
  {                                                                           \
    _obj->nam = bit_read_BD (dat);                                            \
    if (bit_isnan (_obj->nam))                                                \
      {                                                                       \
        FIELD_G_TRACE (nam, BD, dxf);                                         \
        LOG_ERROR ("Invalid BD " #nam);                                       \
        return DWG_ERR_VALUEOUTOFBOUNDS;                                      \
      }                                                                       \
    if (dxf >= 50 && dxf < 54)                                                \
      {                                                                       \
        FIELD_G_TRACE_ANGLE (nam, BD, dxf);                                   \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        FIELD_G_TRACE (nam, BD, dxf);                                         \
      }                                                                       \
  }
#define FIELD_BLx(nam, dxf) FIELD_CAST (nam, BL, BLx, dxf)
#define SUB_FIELD_BLx(o, nam, dxf) SUB_FIELD_CAST (o, nam, BL, BLx, dxf)
#define SUB_FIELD_RC(o, nam, dxf) SUB_FIELD (o, nam, RC, dxf)
#define FIELD_RCx(nam, dxf) FIELD_CAST (nam, RC, RCx, dxf)
#define FIELD_RCd(nam, dxf) FIELD_CAST (nam, RC, RCd, dxf)
#define FIELD_RLx(nam, dxf) FIELD_CAST (nam, RL, RLx, dxf)
#define FIELD_BSx(nam, dxf) FIELD_CAST (nam, BS, BSx, dxf)
#define FIELD_RSd(nam, dxf) FIELD_CAST (nam, RS, RSd, dxf)
#define FIELD_BSd(nam, dxf) FIELD_CAST (nam, BS, BSd, dxf)
#define SUB_FIELD_BSd(o, nam, dxf) SUB_FIELD_CAST (o, nam, BS, BSd, dxf)
#define FIELD_BLd(nam, dxf) FIELD_CAST (nam, BL, BLd, dxf)
#define FIELD_RLd(nam, dxf) FIELD_CAST (nam, RL, RLd, dxf)
#define FIELD_RC(nam, dxf) FIELDG (nam, RC, dxf)
#define FIELD_RCu(nam, dxf)                                                   \
  {                                                                           \
    _obj->nam = bit_read_RC (dat);                                            \
    LOG_TRACE (#nam ": %u [RC %d]", (unsigned)((unsigned char)_obj->nam),     \
               dxf);                                                          \
    LOG_RPOS                                                                  \
  }
#define SUB_FIELD_RCd(o, nam, dxf) SUB_FIELD_CAST (o, nam, RC, RCd, dxf)
#define FIELD_RS(nam, dxf) FIELDG (nam, RS, dxf)
#define FIELD_RSx(nam, dxf) FIELD_CAST (nam, RS, RSx, dxf)
#define FIELD_RD(nam, dxf)                                                    \
  {                                                                           \
    _obj->nam = bit_read_RD (dat);                                            \
    if (bit_isnan (_obj->nam))                                                \
      {                                                                       \
        LOG_ERROR ("Invalid RD " #nam);                                       \
        return DWG_ERR_VALUEOUTOFBOUNDS;                                      \
      }                                                                       \
    if (dxf >= 50 && dxf < 54)                                                \
      {                                                                       \
        LOG_TRACE (#nam ": " FORMAT_RD " [RD %d]  %gº", _obj->nam, dxf,       \
                   rad2deg (_obj->nam));                                      \
        LOG_RPOS                                                              \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        FIELD_G_TRACE (nam, RD, dxf);                                         \
      }                                                                       \
  }
#define FIELD_RL(nam, dxf) FIELDG (nam, RL, dxf)
#define FIELD_RLL(nam, dxf) FIELDG (nam, RLL, dxf)
#define FIELD_RLLd(nam, dxf) FIELDG (nam, RLLd, dxf)
#define FIELD_MC(nam, dxf) FIELDG (nam, MC, dxf)
#define FIELD_MS(nam, dxf) FIELDG (nam, MS, dxf)
/* preR13 we have no obj->address and obj->size yet, skip VECTOR_CHKCOUNT */
// clang-format off
#define FIELD_TF(nam, len, dxf)                                               \
  {                                                                           \
    SINCE (R_13b1) { _obj->nam = NULL; VECTOR_CHKCOUNT (nam, TF, len, dat) }  \
    _obj->nam = bit_read_TF (dat, (size_t)len);                               \
    LOG_TRACE (#nam ": \"%s\" [TF %" PRIuSIZE " " #dxf "]", _obj->nam, (size_t)len);    \
    if (!_obj->nam)                                                           \
      return DWG_ERR_VALUEOUTOFBOUNDS;                                        \
    LOG_INSANE (" @%" PRIuSIZE ".%u", dat->byte, (unsigned)dat->bit)                    \
    LOG_TRACE ("\n")                                                          \
    LOG_INSANE_TF (FIELD_VALUE (nam), (int)len);                              \
  }
#define FIELD_TFv(nam, len, dxf)                                              \
  {                                                                           \
    /* if (_obj->nam) free (_obj->nam); // preR13 add_Document defaults */    \
    SINCE (R_13b1) { _obj->nam = NULL; VECTOR_CHKCOUNT (nam, TF, len, dat) }  \
    _obj->nam = (BITCODE_TV)bit_read_TF (dat, (size_t)len);                   \
    LOG_TRACE (#nam ": \"%s\" [TFv %" PRIuSIZE " " #dxf "]", _obj->nam,       \
               (size_t)len);                                                  \
    if (!_obj->nam)                                                           \
      return DWG_ERR_VALUEOUTOFBOUNDS;                                        \
    LOG_RPOS                                                                  \
    /* LOG_TRACE_TF (FIELD_VALUE (nam), (int)len); */                         \
  }
#define FIELD_TFF(nam, len, dxf)                                              \
  {                                                                           \
    SINCE (R_13b1) { _VECTOR_CHKCOUNT_STATIC (nam, len, 8, dat) }             \
    bit_read_fixed (dat, _obj->nam, (size_t)len);                             \
    LOG_TRACE (#nam ": \"%.*s\" [TFF %" PRIuSIZE " " #dxf "]", (int)len,      \
               _obj->nam, (size_t)len);                                       \
    LOG_RPOS                                                                  \
    LOG_TRACE_TF (FIELD_VALUE (nam), (size_t)len);                            \
  }
#define FIELD_TFFx(nam, len, dxf)                                             \
  {                                                                           \
    SINCE (R_13b1) { _VECTOR_CHKCOUNT_STATIC (nam, len, 8, dat) }             \
    bit_read_fixed (dat, (BITCODE_RC*)_obj->nam, (int)len);                   \
    LOG_TRACE (#nam ": [TFFx %d " #dxf "]", (int)len);                        \
    LOG_RPOS                                                                  \
    LOG_TRACE_TF ((BITCODE_RC*)_obj->nam, (int)len);                          \
  }
#define FIELD_T16(nam, dxf)                                                   \
    if (dat->from_version < R_2007)                                           \
      {                                                                       \
        _obj->nam = (BITCODE_T16)bit_read_T16 (dat);                          \
        LOG_TRACE (#nam ": %s [T16 " #dxf "]", _obj->nam);                    \
        LOG_RPOS                                                              \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        _obj->nam = (BITCODE_T16)bit_read_TU16 (dat);                         \
        LOG_TRACE_TU (#nam, FIELD_VALUE (nam), dxf);                          \
      }
#define FIELD_T32(nam, dxf)                                                   \
  {                                                                           \
    if (dat->from_version < R_2007)                                           \
      {                                                                       \
        FIELDG (nam, T32, dxf);                                               \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        _obj->nam = bit_read_TU32 (dat);                                      \
        LOG_TRACE_TU (#nam, FIELD_VALUE (nam), dxf)                           \
      }                                                                       \
  }
#define FIELD_TU16(nam, dxf)                                                  \
  {                                                                           \
    _obj->nam = bit_read_TU16 (dat);                                          \
    LOG_TRACE_TU (#nam, FIELD_VALUE (nam), dxf);                              \
  }
#define FIELD_TU32(nam, dxf)                                                  \
  {                                                                           \
    _obj->nam = bit_read_TU32 (dat);                                          \
    if (dat->from_version < R_2007)                                           \
      {                                                                       \
        LOG_TRACE (#nam ": \"%s\" [TU32 %d]", _obj->nam, dxf)                 \
        LOG_RPOS                                                              \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        LOG_TRACE_TU (#nam, FIELD_VALUE (nam), dxf)                           \
      }                                                                       \
  }
#define FIELD_TV(nam, dxf)                                                    \
  {                                                                           \
    _obj->nam = bit_read_TV (dat);                                            \
    LOG_TRACE_TV (#nam ": \"%s\" [TV %d]", _obj->nam, dxf);                   \
  }
#define FIELD_TU(nam, dxf)                                                    \
  {                                                                           \
    _obj->nam = (BITCODE_TU)bit_read_TU (str_dat);                            \
    LOG_TRACE_TU (#nam, (BITCODE_TU)FIELD_VALUE (nam), dxf);                  \
  }
// clang-format on
#define FIELD_T(nam, dxf)                                                     \
  {                                                                           \
    if (dat->from_version < R_2007)                                           \
      {                                                                       \
        _obj->nam = bit_read_TV (dat);                                        \
        LOG_TRACE_TV (#nam ": \"%s\" [T %d]", _obj->nam, dxf)                 \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        if (!obj || obj->has_strings) /* header_vars */                       \
          {                                                                   \
            _obj->nam = (BITCODE_T)bit_read_TU (str_dat);                     \
            LOG_TRACE_TU (#nam, (BITCODE_TU)FIELD_VALUE (nam), dxf)           \
          }                                                                   \
        else                                                                  \
          {                                                                   \
            LOG_TRACE_TU (#nam, L"", dxf);                                    \
            LOG_INSANE (" !has_strings\n")                                    \
          }                                                                   \
      }                                                                       \
  }
#define FIELD_BT(nam, dxf) FIELDG (nam, BT, dxf)
#define FIELD_4BITS(nam, dxf)                                                 \
  {                                                                           \
    int _b = _obj->nam = bit_read_4BITS (dat);                                \
    LOG_TRACE (#nam ": b%d%d%d%d [4BITS %d]", _b & 8, _b & 4, _b & 2, _b & 1, \
               dxf);                                                          \
    LOG_RPOS                                                                  \
  }

#define FIELD_BE(nam, dxf)                                                    \
  {                                                                           \
    bit_read_BE (dat, &_obj->nam.x, &_obj->nam.y, &_obj->nam.z);              \
    if (dat->version >= R_2000 && FIELD_VALUE (nam.x) == 0.0                  \
        && FIELD_VALUE (nam.y) == 0.0 && FIELD_VALUE (nam.z) == 1.0)          \
      {                                                                       \
        LOG_TRACE (#nam ": default 0,0,1 [B %d]", dxf)                        \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        LOG_TRACE (#nam ": (%f, %f, %f) [BE %d]", _obj->nam.x, _obj->nam.y,   \
                   _obj->nam.z, dxf)                                          \
      }                                                                       \
    LOG_POS;                                                                  \
  }
#define TRACE_DD                                                              \
  {                                                                           \
    BITCODE_BB result;                                                        \
    BITCODE_RC byte;                                                          \
    if ((dat->byte * 8) + 8 + dat->bit > dat->size * 8)                       \
      {                                                                       \
        loglevel = dat->opts & DWG_OPTS_LOGLEVEL;                             \
        LOG_ERROR ("%s FIELD_DD buffer overflow at pos %" PRIuSIZE            \
                   ".%u, size %" PRIuSIZE ","                                 \
                   " advance by 8",                                           \
                   __FUNCTION__, dat->byte, dat->bit, dat->size);             \
      }                                                                       \
    byte = dat->chain[dat->byte];                                             \
    if (dat->bit < 7)                                                         \
      result = (byte & (0xc0 >> dat->bit)) >> (6 - dat->bit);                 \
    else                                                                      \
      {                                                                       \
        result = (byte & 0x01) << 1;                                          \
        if (dat->byte < dat->size - 1)                                        \
          {                                                                   \
            byte = dat->chain[dat->byte + 1];                                 \
            result |= (byte & 0x80) >> 7;                                     \
          }                                                                   \
      }                                                                       \
    LOG_HANDLE ("DD code %u\n", result)                                       \
  }
#define FIELD_DD(nam, _default, dxf)                                          \
  {                                                                           \
    TRACE_DD                                                                  \
    FIELD_VALUE (nam) = bit_read_DD (dat, _default);                          \
    if (bit_isnan (_obj->nam))                                                \
      {                                                                       \
        LOG_ERROR ("Invalid DD " #nam);                                       \
        return DWG_ERR_VALUEOUTOFBOUNDS;                                      \
      }                                                                       \
  }
#define FIELD_2DD(nam, def, dxf)                                              \
  {                                                                           \
    FIELD_DD (nam.x, FIELD_VALUE (def.x), dxf);                               \
    FIELD_DD (nam.y, FIELD_VALUE (def.y), dxf + 10);                          \
    FIELD_2PT_TRACE (nam, 2DD, dxf);                                          \
  }
#define FIELD_3DD(nam, def, dxf)                                              \
  {                                                                           \
    FIELD_DD (nam.x, FIELD_VALUE (def.x), dxf);                               \
    FIELD_DD (nam.y, FIELD_VALUE (def.y), dxf + 10);                          \
    FIELD_DD (nam.z, FIELD_VALUE (def.z), dxf + 20);                          \
    FIELD_3PT_TRACE (nam, 3DD, dxf);                                          \
  }
#define FIELD_3RD(nam, dxf)                                                   \
  {                                                                           \
    _obj->nam.x = bit_read_RD (dat);                                          \
    _obj->nam.y = bit_read_RD (dat);                                          \
    _obj->nam.z = bit_read_RD (dat);                                          \
    if (bit_isnan (_obj->nam.x) || bit_isnan (_obj->nam.y)                    \
        || bit_isnan (_obj->nam.z))                                           \
      {                                                                       \
        LOG_ERROR ("Invalid 3RD " #nam);                                      \
        return DWG_ERR_VALUEOUTOFBOUNDS;                                      \
      }                                                                       \
    FIELD_3PT_TRACE (nam, 3RD, dxf);                                          \
  }
#define FIELD_3BD(nam, dxf)                                                   \
  {                                                                           \
    _obj->nam.x = bit_read_BD (dat);                                          \
    _obj->nam.y = bit_read_BD (dat);                                          \
    _obj->nam.z = bit_read_BD (dat);                                          \
    if (bit_isnan (_obj->nam.x) || bit_isnan (_obj->nam.y)                    \
        || bit_isnan (_obj->nam.z))                                           \
      {                                                                       \
        LOG_ERROR ("Invalid 3BD " #nam);                                      \
        return DWG_ERR_VALUEOUTOFBOUNDS;                                      \
      }                                                                       \
    FIELD_3PT_TRACE (nam, 3BD, dxf);                                          \
  }
#define FIELD_2RD(nam, dxf)                                                   \
  {                                                                           \
    _obj->nam.x = bit_read_RD (dat);                                          \
    _obj->nam.y = bit_read_RD (dat);                                          \
    if (bit_isnan (_obj->nam.x) || bit_isnan (_obj->nam.y))                   \
      {                                                                       \
        LOG_ERROR ("Invalid 2RD " #nam);                                      \
        return DWG_ERR_VALUEOUTOFBOUNDS;                                      \
      }                                                                       \
    FIELD_2PT_TRACE (nam, 2RD, dxf);                                          \
  }
// FIXME: does this even exist? normal 2DPOINT is 2RD
//   HATCH.deflines.pt0,offset LAYOUT.plot_origin,plot_window_{ll,ur},
//   LAYOUT.paper_image_origin GEODATA.northdir, GEODATA.refpt0,1 .zero1, zero2
//   BACKGROUND.offset,scale
#define FIELD_2BD(nam, dxf)                                                   \
  {                                                                           \
    _obj->nam.x = bit_read_BD (dat);                                          \
    _obj->nam.y = bit_read_BD (dat);                                          \
    if (bit_isnan (_obj->nam.x) || bit_isnan (_obj->nam.y))                   \
      {                                                                       \
        LOG_ERROR ("Invalid 2BD " #nam);                                      \
        return DWG_ERR_VALUEOUTOFBOUNDS;                                      \
      }                                                                       \
    FIELD_2PT_TRACE (nam, 2BD, dxf);                                          \
  }
#define FIELD_2BD_1(nam, dxf)                                                 \
  {                                                                           \
    _obj->nam.x = bit_read_BD (dat);                                          \
    _obj->nam.y = bit_read_BD (dat);                                          \
    if (bit_isnan (_obj->nam.x) || bit_isnan (_obj->nam.y))                   \
      {                                                                       \
        LOG_ERROR ("Invalid 2BD_1 " #nam);                                    \
        return DWG_ERR_VALUEOUTOFBOUNDS;                                      \
      }                                                                       \
    FIELD_2PT_TRACE (nam, 2BD_1, dxf);                                        \
  }
// FIELDG(nam.x, BD, dxf); FIELDG(nam.y, BD, dxf+1);
#define FIELD_3BD_1(nam, dxf)                                                 \
  {                                                                           \
    _obj->nam.x = bit_read_BD (dat);                                          \
    _obj->nam.y = bit_read_BD (dat);                                          \
    _obj->nam.z = bit_read_BD (dat);                                          \
    if (bit_isnan (_obj->nam.x) || bit_isnan (_obj->nam.y)                    \
        || bit_isnan (_obj->nam.z))                                           \
      {                                                                       \
        LOG_ERROR ("Invalid 3BD_1 " #nam);                                    \
        return DWG_ERR_VALUEOUTOFBOUNDS;                                      \
      }                                                                       \
    FIELD_3PT_TRACE (nam, 3BD_1, dxf);                                        \
  }
//    FIELDG(nam.x, BD, dxf); FIELDG(nam.y, BD, dxf+1);
//    FIELDG(nam.z, BD, dxf+2); }
#define FIELD_2DPOINT(nam, dxf) FIELD_2RD (nam, dxf)
#define FIELD_3DPOINT(nam, dxf) FIELD_3BD (nam, dxf)
#define FIELD_3DVECTOR(nam, dxf) FIELD_3BD_1 (nam, dxf)
#define FIELD_TIMEBLL(nam, dxf)                                               \
  {                                                                           \
    _obj->nam = bit_read_TIMEBLL (dat);                                       \
    if (DWG_LOGLEVEL >= DWG_LOGLEVEL_TRACE)                                   \
      {                                                                       \
        struct tm tm;                                                         \
        char _buf[60] = "";                                                   \
        cvt_TIMEBLL (&tm, _obj->nam);                                         \
        if (strEQc (#nam, "TDINDWG") || strEQc (#nam, "TDUSRTIMER"))          \
          strftime (_buf, 60, STRFTIME_DURATION, &tm);                        \
        else if (_obj->nam.days)                                              \
          strftime (_buf, 60, STRFTIME_DATE, &tm);                            \
        else                                                                  \
          strftime (_buf, 60, STRFTIME_TIME, &tm);                            \
        LOG_TRACE (#nam ": [" FORMAT_BL ", " FORMAT_BL "] %s [TIMEBLL %d]",   \
                   _obj->nam.days, _obj->nam.ms, _buf, dxf);                  \
        LOG_RPOS                                                              \
      }                                                                       \
  }
#define FIELD_TIMERLL(nam, dxf)                                               \
  {                                                                           \
    _obj->nam = bit_read_TIMERLL (dat);                                       \
    if (DWG_LOGLEVEL >= DWG_LOGLEVEL_TRACE)                                   \
      {                                                                       \
        struct tm tm;                                                         \
        char _buf[60] = "";                                                   \
        if (_obj->nam.days)                                                   \
          strftime (_buf, 60, STRFTIME_DATE, cvt_TIMEBLL (&tm, _obj->nam));   \
        else                                                                  \
          strftime (_buf, 60, STRFTIME_TIME, cvt_TIMEBLL (&tm, _obj->nam));   \
        LOG_TRACE (#nam ": [" FORMAT_RL ", " FORMAT_RL "] %s [TIMERLL %d]",   \
                   _obj->nam.days, _obj->nam.ms, _buf, dxf);                  \
        LOG_RPOS                                                              \
      }                                                                       \
  }
#define FIELD_CMC(color, dxf)                                                 \
  {                                                                           \
    error |= bit_read_CMC (dat, str_dat, &_obj->color);                       \
    LOG_TRACE (#color ".index: %d [CMC.%s %d]", _obj->color.index,            \
               dat->from_version < R_13b1 ? "RS" : "BS", dxf);                \
    LOG_RPOS                                                                  \
    if (dat->from_version >= R_2004)                                          \
      {                                                                       \
        LOG_TRACE (#color ".rgb: 0x%06x [CMC.BL %d]\n",                       \
                   (unsigned)_obj->color.rgb, dxf + 420 - 62);                \
        LOG_TRACE (#color ".method: 0x%x %s\n", _obj->color.method,           \
                   dwg_color_method_name (_obj->color.method));               \
        LOG_TRACE (#color ".flag: 0x%x [CMC.RC]\n",                           \
                   (unsigned)_obj->color.flag);                               \
        if (_obj->color.flag & 1)                                             \
          {                                                                   \
            LOG_TRACE (#color ".name: %s [CMC.%s]\n", _obj->color.name,       \
                       dat->from_version >= R_2007 ? "T" : "TV");             \
          }                                                                   \
        if (_obj->color.flag & 2)                                             \
          {                                                                   \
            LOG_TRACE (#color ".book_name: %s [CMC.%s]\n",                    \
                       _obj->color.book_name,                                 \
                       dat->from_version >= R_2007 ? "T" : "TV");             \
          }                                                                   \
      }                                                                       \
  }
#define SUB_FIELD_CMC(o, color, dxf)                                          \
  {                                                                           \
    error |= bit_read_CMC (dat, str_dat, &_obj->o.color);                     \
    LOG_TRACE (#o "." #color ".index: %d [CMC.BS %d]", _obj->o.color.index,   \
               dxf);                                                          \
    LOG_RPOS                                                                  \
    if (dat->from_version >= R_2004)                                          \
      {                                                                       \
        LOG_TRACE (#o "." #color ".rgb: 0x%06x [CMC.BL %d]\n",                \
                   (unsigned)_obj->o.color.rgb, dxf + 420 - 62);              \
        LOG_TRACE (#color ".method: 0x%x %s\n", _obj->o.color.method,         \
                   dwg_color_method_name (_obj->o.color.method));             \
        LOG_TRACE (#o "." #color ".flag: 0x%x [CMC.RC]\n",                    \
                   (unsigned)_obj->o.color.flag);                             \
        if (_obj->o.color.flag & 1)                                           \
          {                                                                   \
            LOG_TRACE (#o "." #color ".name: %s [CMC.%s]\n",                  \
                       _obj->o.color.name,                                    \
                       dat->from_version >= R_2007 ? "T" : "TV");             \
          }                                                                   \
        if (_obj->o.color.flag & 2)                                           \
          {                                                                   \
            LOG_TRACE (#o "." #color ".book_name: %s [CMC.%s]\n",             \
                       _obj->o.color.book_name,                               \
                       dat->from_version >= R_2007 ? "T" : "TV");             \
          }                                                                   \
      }                                                                       \
  }
// force truecolor
#define FIELD_CMTC(name, dxf)                                                 \
  {                                                                           \
    Dwg_Version_Type _ver = dat->from_version;                                \
    if (dat->from_version < R_2004)                                           \
      dat->from_version = R_2004;                                             \
    FIELD_CMC (name, dxf);                                                    \
    dat->from_version = _ver;                                                 \
  }
#define SUB_FIELD_CMTC(o, name, dxf)                                          \
  {                                                                           \
    Dwg_Version_Type _ver = dat->from_version;                                \
    if (dat->from_version < R_2004)                                           \
      dat->from_version = R_2004;                                             \
    SUB_FIELD_CMC (o, name, dxf);                                             \
    dat->from_version = _ver;                                                 \
  }
#define FIELD_ENC(color, dxf)                                                 \
  {                                                                           \
    bit_read_ENC (dat, hdl_dat, str_dat, &_obj->color);                       \
    LOG_TRACE (#color ".index: %d [ENC.BS %d]", _obj->color.index, dxf);      \
    LOG_POS                                                                   \
    if (dat->from_version >= R_2004)                                          \
      {                                                                       \
        if (_obj->color.flag)                                                 \
          LOG_TRACE (#color ".flag: 0x%x\n", (unsigned)_obj->color.flag);     \
        if (_obj->color.flag & 0x20)                                          \
          LOG_TRACE (#color ".alpha: %d [ENC.BL %d]\n",                       \
                     (int)_obj->color.alpha, dxf + 440 - 62);                 \
        if (_obj->color.flag & 0x80)                                          \
          LOG_TRACE (#color ".rgb: 0x%06x [ENC.BL %d]\n",                     \
                     (unsigned)_obj->color.rgb, dxf + 420 - 62);              \
        if (_obj->color.flag & 0x40 && _obj->color.handle)                    \
          LOG_TRACE (#color ".handle: %X [ENC.H %d]\n",                       \
                     _obj->color.handle->handleref.value, dxf + 430 - 62);    \
      }                                                                       \
  }
#define SUB_FIELD_ENC(o, color, dxf)                                          \
  {                                                                           \
    bit_read_ENC (dat, hdl_dat, str_dat, &_obj->o.color);                     \
    LOG_TRACE (#o "." #color ".index: %d [ENC.BS %d]", _obj->o.color.index,   \
               dxf);                                                          \
    LOG_POS                                                                   \
    if (dat->from_version >= R_2004)                                          \
      {                                                                       \
        if (_obj->o.color.flag)                                               \
          LOG_TRACE (#o "." #color ".flag: 0x%x\n",                           \
                     (unsigned)_obj->o.color.flag);                           \
        if (_obj->o.color.flag & 0x20)                                        \
          LOG_TRACE (#o "." #color ".alpha: %d [ENC.BL %d]\n",                \
                     (unsigned)_obj->o.color.alpha, dxf + 440 - 62);          \
        if (_obj->o.color.flag & 0x80)                                        \
          LOG_TRACE (#o "." #color ".rgb: 0x%06x [ENC.BL %d]\n",              \
                     (unsigned)_obj->o.color.rgb, dxf + 420 - 62);            \
        if (_obj->o.color.flag & 0x40 && _obj->o.color.handle)                \
          LOG_TRACE (#o "." #color ".handle: " FORMAT_RLLx " [ENC.H %d]\n",   \
                     _obj->o.color.handle->handleref.value, dxf + 430 - 62);  \
      }                                                                       \
  }

#undef DEBUG_POS
#undef DEBUG_HERE
#undef DEBUG_POS_OBJ
#undef DEBUG_HERE_OBJ
#define DEBUG_POS_OBJ                                                         \
  if (DWG_LOGLEVEL >= DWG_LOGLEVEL_TRACE)                                     \
    {                                                                         \
      LOG_TRACE ("DEBUG_POS @%" PRIuSIZE ".%u (%" PRIuSIZE ") %" PRIuSIZE     \
                 "\n",                                                        \
                 dat->byte, (unsigned)dat->bit, bit_position (dat),           \
                 obj ? bit_position (dat) - obj->address * 8 : 0);            \
    }
#define DEBUG_POS                                                             \
  if (DWG_LOGLEVEL >= DWG_LOGLEVEL_TRACE)                                     \
    {                                                                         \
      LOG_TRACE ("DEBUG_POS @%" PRIuSIZE ".%u (%" PRIuSIZE ")\n", dat->byte,  \
                 (unsigned)dat->bit, bit_position (dat));                     \
    }
#define _DEBUG_HERE(objsize)                                                  \
  if (dat->size < dat->byte)                                                  \
    return DWG_ERR_VALUEOUTOFBOUNDS;                                          \
  if (DWG_LOGLEVEL >= DWG_LOGLEVEL_TRACE)                                     \
    {                                                                         \
      Bit_Chain here = *dat;                                                  \
      int oldloglevel = loglevel;                                             \
      BITCODE_TF tmp;                                                         \
      BITCODE_BB bb = 0;                                                      \
      BITCODE_RS rs;                                                          \
      BITCODE_RL rl;                                                          \
      double bd;                                                              \
      Dwg_Handle hdl;                                                         \
      rs = 24;                                                                \
      if ((objsize) > 0 && dat->byte + rs > (objsize))                        \
        rs = ((objsize) - dat->byte) & 0xFFFF;                                \
      if (dat->byte + rs > dat->size)                                         \
        rs = (dat->size - dat->byte - 1) & 0xFFFF;                            \
      tmp = bit_read_TF (dat, (size_t)rs);                                    \
      if (tmp)                                                                \
        {                                                                     \
          if (DWG_LOGLEVEL >= DWG_LOGLEVEL_INSANE)                            \
            {                                                                 \
              bit_fprint_bits (OUTPUT, tmp, rs * 8);                          \
              HANDLER (OUTPUT, "\n");                                         \
            }                                                                 \
          LOG_TRACE_TF (tmp, rs);                                             \
          free (tmp);                                                         \
        }                                                                     \
      SINCE (R_13b1)                                                          \
      {                                                                       \
        *dat = here;                                                          \
        LOG_TRACE ("  B  :" FORMAT_B "\t", bit_read_B (dat));                 \
        *dat = here;                                                          \
        bb = bit_read_BB (dat) & 0x3;                                         \
        LOG_TRACE ("  BB :" FORMAT_BB "\n", bb);                              \
      }                                                                       \
      *dat = here;                                                            \
      rs = bit_read_RS (dat);                                                 \
      LOG_TRACE ("  RS :" FORMAT_RS " / 0x%04x (16)\n", rs, rs);              \
      SINCE (R_13b1)                                                          \
      {                                                                       \
        *dat = here;                                                          \
        rs = bit_read_BS (dat);                                               \
        LOG_TRACE ("  BS :" FORMAT_BS " / 0x%04x (%" PRIuSIZE ")\t", rs, rs,  \
                   bit_position (dat) - bit_position (&here));                \
      }                                                                       \
      SINCE (R_2007a)                                                         \
      {                                                                       \
        *dat = here;                                                          \
        rs = bit_read_MS (dat);                                               \
        LOG_TRACE ("  MS :" FORMAT_RS " / 0x%04x (%" PRIuSIZE ")\n", rs, rs,  \
                   bit_position (dat) - bit_position (&here));                \
      }                                                                       \
      else LOG_TRACE ("\n");                                                  \
      *dat = here;                                                            \
      rl = bit_read_RL (dat);                                                 \
      LOG_TRACE ("  RL :" FORMAT_RL " / 0x%08x (32)\n", rl, rl);              \
      *dat = here;                                                            \
      LOG_TRACE ("  RD :" FORMAT_RD " (64)\n", bit_read_RD (dat));            \
      *dat = here;                                                            \
      SINCE (R_13b1)                                                          \
      {                                                                       \
        if (bb != 3)                                                          \
          {                                                                   \
            rl = bit_read_BL (dat);                                           \
            LOG_TRACE ("  BL :" FORMAT_BL " / 0x%08x (%" PRIuSIZE ")\n", rl,  \
                       rl, bit_position (dat) - bit_position (&here));        \
            *dat = here;                                                      \
            bd = bit_read_BD (dat);                                           \
            LOG_TRACE ("  BD :" FORMAT_BD " (%" PRIuSIZE ")\n", bd,           \
                       bit_position (dat) - bit_position (&here));            \
            *dat = here;                                                      \
          }                                                                   \
      }                                                                       \
      if ((dat->chain[dat->byte] & 0xf) <= 4)                                 \
        {                                                                     \
          loglevel = 0;                                                       \
          if (!bit_read_H (dat, &hdl))                                        \
            {                                                                 \
              LOG_TRACE ("  H : " FORMAT_H " (%" PRIuSIZE ")\n",              \
                         ARGS_H (hdl),                                        \
                         bit_position (dat) - bit_position (&here));          \
            }                                                                 \
          loglevel = oldloglevel;                                             \
        }                                                                     \
      *dat = here;                                                            \
    }
#define DEBUG_HERE_OBJ                                                        \
  DEBUG_POS_OBJ                                                               \
  _DEBUG_HERE (obj->size)
#define DEBUG_HERE                                                            \
  DEBUG_POS                                                                   \
  _DEBUG_HERE (0UL)

// check for overflow into next object (invalid num_elems)
#define AVAIL_BITS(dat) (int64_t) ((dat->size * 8) - bit_position (dat))
#define TYPE_MAXELEMSIZE(type) dwg_bits_size[BITS_##type]
#define VECTOR_CHKCOUNT(nam, type, size, dat)                                 \
  if ((int64_t)(size) > AVAIL_BITS (dat)                                      \
      || (int64_t)((size) * TYPE_MAXELEMSIZE (type)) > AVAIL_BITS (dat))      \
    {                                                                         \
      LOG_ERROR ("Invalid " #nam " size %" PRId64 ". Need min. %" PRId64      \
                 " bits for " #type ", have %" PRId64 " for %s.",             \
                 (int64_t)(size), (int64_t)(size) * TYPE_MAXELEMSIZE (type),  \
                 AVAIL_BITS (dat), SAFEDXFNAME);                              \
      if (_obj->nam)                                                          \
        free (_obj->nam);                                                     \
      return DWG_ERR_VALUEOUTOFBOUNDS;                                        \
    }
#define SUB_VECTOR_CHKCOUNT(o, nam, type, size, dat)                          \
  if ((int64_t)(size) > AVAIL_BITS (dat)                                      \
      || (int64_t)((size) * TYPE_MAXELEMSIZE (type)) > AVAIL_BITS (dat))      \
    {                                                                         \
      LOG_ERROR ("Invalid " #nam " size %" PRId64 ". Need min. %" PRId64      \
                 " bits for " #type ", have %" PRId64 " at @%" PRIuSIZE ".%u "\
                 " for %s.",                                                  \
                 (int64_t)(size), (int64_t)(size) * TYPE_MAXELEMSIZE (type),  \
                 AVAIL_BITS (dat), dat->byte, (unsigned)dat->bit, SAFEDXFNAME);\
      if (_obj->o.nam)                                                        \
        free (_obj->o.nam);                                                   \
      return DWG_ERR_VALUEOUTOFBOUNDS;                                        \
    }
#define VECTOR_CHKCOUNT_LV(nam, type, size, dat)                              \
  if ((int64_t)(size) > AVAIL_BITS (dat)                                      \
      || (int64_t)((size) * TYPE_MAXELEMSIZE (type)) > AVAIL_BITS (dat))      \
    {                                                                         \
      LOG_ERROR ("Invalid " #nam " size %" PRId64 ". Need min. %" PRId64      \
                 " bits for " #type ", have %" PRId64 " at @%" PRIuSIZE ".%u "\
                 "for %s.",                                                   \
                 (int64_t)(size), (int64_t)(size) * TYPE_MAXELEMSIZE (type),  \
                 AVAIL_BITS (dat), dat->byte, (unsigned)dat->bit, SAFEDXFNAME);\
      if (_obj->nam)                                                          \
        free (_obj->nam);                                                     \
      size = 0;                                                               \
      /* return DWG_ERR_VALUEOUTOFBOUNDS; */                                  \
    }
// for static TFF types with a size field
#define _VECTOR_CHKCOUNT(nam, size, maxelemsize, dat)                         \
  if ((int64_t)(size) > AVAIL_BITS (dat)                                      \
      || (int64_t)((size) * (maxelemsize)) > AVAIL_BITS (dat))                \
    {                                                                         \
      LOG_ERROR ("Invalid " #nam " size %" PRId64 ". Need min. %" PRId64      \
                 " bits, have %" PRId64 " for %s. "                           \
                 "Set " #size " to 0",                                        \
                 (int64_t)(size), (int64_t)(size) * (maxelemsize),            \
                 AVAIL_BITS (dat), SAFEDXFNAME);                              \
      size = 0;                                                               \
      return DWG_ERR_VALUEOUTOFBOUNDS;                                        \
    }
// for static TFF types with fixed size
#define _VECTOR_CHKCOUNT_STATIC(nam, siz, maxelemsize, dat)                   \
  if ((int64_t)(8 * siz) > AVAIL_BITS (dat)                                   \
      || (int64_t)((siz) * (maxelemsize)) > AVAIL_BITS (dat)                  \
      || dat->byte + (siz) > dat->size)                                       \
    {                                                                         \
      LOG_ERROR ("Invalid " #nam " size %" PRId64 ". Need min. %" PRId64      \
                 " bits, have %" PRId64 " for %s.",                           \
                 (int64_t)(siz), (int64_t)(siz) * (maxelemsize),              \
                 AVAIL_BITS (dat), SAFEDXFNAME);                              \
      return DWG_ERR_VALUEOUTOFBOUNDS;                                        \
    }
#define HANDLE_VECTOR_CHKCOUNT(nam, size)                                     \
  _VECTOR_CHKCOUNT (nam, size, TYPE_MAXELEMSIZE (HANDLE), hdl_dat)

// FIELD_VECTOR_N(name, type, size):
// reads data of the type indicated by 'type' 'size' times and stores
// it all in the vector called 'name'.
#define FIELD_VECTOR_N(name, type, size, dxf)                                 \
  if (size > 0)                                                               \
    {                                                                         \
      VECTOR_CHKCOUNT (name, type, size, dat)                                 \
      _obj->name = (BITCODE_##type *)calloc (size, sizeof (BITCODE_##type));  \
      if (!_obj->name)                                                        \
        return DWG_ERR_OUTOFMEM;                                              \
      for (vcount = 0; vcount < (BITCODE_BL)size; vcount++)                   \
        {                                                                     \
          _obj->name[vcount] = bit_read_##type (dat);                         \
          FIELD_VEC_TRACE_N (name, type, (long)vcount, _obj->name[vcount],    \
                             dxf)                                             \
        }                                                                     \
    }
#define SUB_FIELD_VECTOR_N(o, nam, type, csize, dxf)                          \
  if (csize > 0)                                                              \
    {                                                                         \
      SUB_VECTOR_CHKCOUNT (o, nam, type, csize, dat)                          \
      _obj->o.nam                                                             \
          = (BITCODE_##type *)calloc (csize, sizeof (BITCODE_##type));        \
      if (!_obj->o.nam)                                                       \
        return DWG_ERR_OUTOFMEM;                                              \
      for (vcount = 0; vcount < (BITCODE_BL)csize; vcount++)                  \
        {                                                                     \
          _obj->o.nam[vcount] = bit_read_##type (dat);                        \
          LOG_TRACE (#nam "[%ld]: " FORMAT_##type " [" #type " %d]",          \
                     (long)vcount, _obj->o.nam[vcount], dxf)                  \
          LOG_POS                                                             \
        }                                                                     \
    }
#define SUB_FIELD_VECTOR(o, name, type, sizefield, dxf)                       \
  if (_obj->o.sizefield > 0)                                                  \
    {                                                                         \
      SUB_VECTOR_CHKCOUNT (o, name, type, _obj->o.sizefield, dat)             \
      _obj->o.name = (BITCODE_##type *)calloc (_obj->o.sizefield,             \
                                               sizeof (BITCODE_##type));      \
      if (!_obj->o.name)                                                      \
        return DWG_ERR_OUTOFMEM;                                              \
      LOG_TRACE (#name ": { ")                                                \
      for (vcount = 0; vcount < (BITCODE_BL)_obj->o.sizefield; vcount++)      \
        {                                                                     \
          _obj->o.name[vcount] = bit_read_##type (dat);                       \
          LOG_TRACE (FORMAT_##type " ", _obj->o.name[vcount])                 \
        }                                                                     \
      if (dxf)                                                                \
        LOG_TRACE ("} [*" #type " %d]", dxf)                                  \
      else                                                                    \
        LOG_TRACE ("} [*" #type "]")                                          \
      LOG_POS                                                                 \
    }
// inlined, with const size and without malloc
#define FIELD_VECTOR_INL(name, type, size, dxf)                               \
  if (size > 0)                                                               \
    {                                                                         \
      SINCE (R_13b1)                                                          \
      {                                                                       \
        _VECTOR_CHKCOUNT_STATIC (name, size, TYPE_MAXELEMSIZE (type), dat)    \
      } LOG_TRACE (#name ": { ") for (vcount = 0; vcount < (BITCODE_BL)size;  \
                                      vcount++)                               \
      {                                                                       \
        _obj->name[vcount] = bit_read_##type (dat);                           \
        LOG_TRACE (FORMAT_##type " ", _obj->name[vcount])                     \
      }                                                                       \
      if (dxf)                                                                \
        LOG_TRACE ("} [*" #type " %d]", dxf)                                  \
      else                                                                    \
        LOG_TRACE ("} [*" #type "]")                                          \
      LOG_POS                                                                 \
    }
// inlined, with const size and without malloc
#define SUB_FIELD_VECTOR_INL(o, nam, type, csize, dxf)                        \
  if (csize > 0)                                                              \
    {                                                                         \
      _VECTOR_CHKCOUNT_STATIC (nam, csize, TYPE_MAXELEMSIZE (type), dat)      \
      for (vcount = 0; vcount < (BITCODE_BL)csize; vcount++)                  \
        {                                                                     \
          _obj->o.nam[vcount] = bit_read_##type (dat);                        \
          LOG_TRACE (#nam "[%ld]: " FORMAT_##type " [" #type " %d]",          \
                     (long)vcount, _obj->o.nam[vcount], dxf)                  \
          LOG_POS                                                             \
        }                                                                     \
    }
#define FIELD_VECTOR_T(name, type, size, dxf)                                 \
  if (_obj->size > 0)                                                         \
    {                                                                         \
      _VECTOR_CHKCOUNT (name, _obj->size,                                     \
                        dat->from_version >= R_2007 ? 18 : 2, dat)            \
      _obj->name = (char **)calloc (_obj->size, sizeof (char *));             \
      if (!_obj->name)                                                        \
        return DWG_ERR_OUTOFMEM;                                              \
      for (vcount = 0; vcount < (BITCODE_BL)_obj->size; vcount++)             \
        {                                                                     \
          PRE (R_2007a)                                                       \
          {                                                                   \
            _obj->name[vcount] = bit_read_TV (dat);                           \
            LOG_TRACE (#name "[%d]: \"%s\" [TV %d]", (int)vcount,             \
                       _obj->name[vcount], dxf)                               \
            LOG_POS                                                           \
            if (!_obj->name[vcount])                                          \
              return DWG_ERR_VALUEOUTOFBOUNDS;                                \
          }                                                                   \
          LATER_VERSIONS                                                      \
          {                                                                   \
            _obj->name[vcount] = (char *)bit_read_##type (dat);               \
            LOG_TRACE_TU_I (#name, vcount, _obj->name[vcount], type, dxf)     \
            if (!_obj->name[vcount])                                          \
              return DWG_ERR_VALUEOUTOFBOUNDS;                                \
          }                                                                   \
        }                                                                     \
    }
#define FIELD_VECTOR_N1(name, type, size, dxf)                                \
  if (size > 0)                                                               \
    {                                                                         \
      int _dxf = dxf;                                                         \
      VECTOR_CHKCOUNT (name, type, size, dat)                                 \
      _obj->name = (BITCODE_##type *)calloc (size, sizeof (BITCODE_##type));  \
      if (!_obj->name)                                                        \
        return DWG_ERR_OUTOFMEM;                                              \
      for (vcount = 0; vcount < (BITCODE_BL)size; vcount++)                   \
        {                                                                     \
          _obj->name[vcount] = bit_read_##type (dat);                         \
          LOG_TRACE (#name "[%d]: " FORMAT_##type " [" #type " %d]",          \
                     (int)vcount, _obj->name[vcount], _dxf++)                 \
          LOG_POS                                                             \
        }                                                                     \
    }

#define FIELD_VECTOR(name, type, size, dxf)                                   \
  FIELD_VECTOR_N (name, type, _obj->size, dxf)

#define SUB_FIELD_VECTOR_TYPESIZE(o, name, size, typesize, dxf)               \
  if (_obj->o.size > 0)                                                       \
    {                                                                         \
      _obj->o.name = (BITCODE_RC *)calloc (_obj->o.size, typesize);           \
      if (!_obj->o.name)                                                      \
        return DWG_ERR_OUTOFMEM;                                              \
      for (vcount = 0; vcount < (BITCODE_BL)_obj->o.size; vcount++)           \
        {                                                                     \
          switch (typesize)                                                   \
            {                                                                 \
            case 0:                                                           \
              break;                                                          \
            case 1:                                                           \
              _obj->o.name[vcount] = bit_read_RC (dat);                       \
              break;                                                          \
            case 2:                                                           \
              _obj->o.name[vcount] = bit_read_RS (dat);                       \
              break;                                                          \
            case 4:                                                           \
              _obj->o.name[vcount] = bit_read_RL (dat);                       \
              break;                                                          \
            case 8:                                                           \
              _obj->o.name[vcount] = bit_read_RLL (dat);                      \
              break;                                                          \
            default:                                                          \
              LOG_ERROR ("Unknown FIELD_VECTOR_TYPE " #name " typesize %d",   \
                         typesize);                                           \
              break;                                                          \
            }                                                                 \
          LOG_TRACE (#name "[%u]: %d", vcount, (int)_obj->o.name[vcount])     \
          LOG_POS                                                             \
        }                                                                     \
    }

#define FIELD_2RD_VECTOR(name, size, dxf)                                     \
  VECTOR_CHKCOUNT_LV (name, 2RD, _obj->size, dat)                             \
  if (_obj->size > 0)                                                         \
    {                                                                         \
      _obj->name = (BITCODE_2RD *)calloc (_obj->size, sizeof (BITCODE_2RD));  \
      if (!_obj->name)                                                        \
        return DWG_ERR_OUTOFMEM;                                              \
      for (vcount = 0; vcount < (BITCODE_BL)_obj->size; vcount++)             \
        {                                                                     \
          FIELD_2RD (name[vcount], dxf);                                      \
        }                                                                     \
    }                                                                         \
  else                                                                        \
    return DWG_ERR_VALUEOUTOFBOUNDS;

#define SUB_FIELD_2RD_VECTOR(o, name, size, dxf)                              \
  VECTOR_CHKCOUNT_LV (o.name, 2RD, _obj->o.size, dat)                         \
  if (_obj->o.size > 0)                                                       \
    {                                                                         \
      _obj->o.name                                                            \
          = (BITCODE_2RD *)calloc (_obj->o.size, sizeof (BITCODE_2RD));       \
      if (!_obj->o.name)                                                      \
        return DWG_ERR_OUTOFMEM;                                              \
      for (vcount = 0; vcount < (BITCODE_BL)_obj->o.size; vcount++)           \
        {                                                                     \
          SUB_FIELD_2RD (o, name[vcount], dxf);                               \
        }                                                                     \
    }                                                                         \
  else                                                                        \
    return DWG_ERR_VALUEOUTOFBOUNDS;

#define FIELD_2DD_VECTOR(name, size, dxf)                                     \
  VECTOR_CHKCOUNT_LV (name, 2DD, _obj->size, dat)                             \
  if (_obj->size > 0)                                                         \
    {                                                                         \
      _obj->name = (BITCODE_2RD *)calloc (_obj->size, sizeof (BITCODE_2RD));  \
      if (!_obj->name)                                                        \
        return DWG_ERR_OUTOFMEM;                                              \
      FIELD_2RD (name[0], dxf);                                               \
      for (vcount = 1; vcount < (BITCODE_BL)_obj->size; vcount++)             \
        {                                                                     \
          BITCODE_BB b2, b1 = bit_read_BB_noadv (dat);                        \
          FIELD_DD (name[vcount].x, FIELD_VALUE (name[vcount - 1].x), dxf);   \
          b2 = bit_read_BB_noadv (dat);                                       \
          FIELD_DD (name[vcount].y, FIELD_VALUE (name[vcount - 1].y),         \
                    dxf + 10);                                                \
          if (b1 == 3 && b2 == 3)                                             \
            LOG_TRACE (#name "[%ld]: (" FORMAT_BD ", " FORMAT_BD              \
                             ") [2DD %d]",                                    \
                       (long)vcount, _obj->name[vcount].x,                    \
                       _obj->name[vcount].y, dxf)                             \
          else                                                                \
            LOG_TRACE (#name "[%ld]: (" FORMAT_BD ", " FORMAT_BD              \
                             ") [2DD/%d%d %d]",                               \
                       (long)vcount, _obj->name[vcount].x,                    \
                       _obj->name[vcount].y, b1, b2, dxf)                     \
          LOG_POS                                                             \
        }                                                                     \
    }                                                                         \
  else                                                                        \
    return DWG_ERR_VALUEOUTOFBOUNDS;

#define FIELD_3DPOINT_VECTOR(name, size, dxf)                                 \
  VECTOR_CHKCOUNT_LV (name, 3BD, _obj->size, dat)                             \
  if (_obj->size > 0)                                                         \
    {                                                                         \
      _obj->name                                                              \
          = (BITCODE_3DPOINT *)calloc (_obj->size, sizeof (BITCODE_3DPOINT)); \
      if (!_obj->name)                                                        \
        return DWG_ERR_OUTOFMEM;                                              \
      for (vcount = 0; vcount < (BITCODE_BL)_obj->size; vcount++)             \
        {                                                                     \
          FIELD_3DPOINT (name[vcount], dxf);                                  \
        }                                                                     \
    }
#define SUB_FIELD_3BD_VECTOR(o, name, size, dxf)                              \
  VECTOR_CHKCOUNT_LV (o.name, 3BD, _obj->o.size, dat)                         \
  if (_obj->o.size > 0)                                                       \
    {                                                                         \
      _obj->o.name                                                            \
          = (BITCODE_3BD *)calloc (_obj->o.size, sizeof (BITCODE_3BD));       \
      if (!_obj->o.name)                                                      \
        return DWG_ERR_OUTOFMEM;                                              \
      for (vcount = 0; vcount < (BITCODE_BL)_obj->o.size; vcount++)           \
        {                                                                     \
          SUB_FIELD_3BD (o, name[vcount], dxf);                               \
        }                                                                     \
    }                                                                         \
  else                                                                        \
    return DWG_ERR_VALUEOUTOFBOUNDS;

// shortest handle: 8 bit
#define HANDLE_VECTOR_N(nam, size, code, dxf)                                 \
  if (size > 0)                                                               \
    {                                                                         \
      FIELD_VALUE (nam) = (BITCODE_H *)calloc (size, sizeof (BITCODE_H));     \
      if (!_obj->nam)                                                         \
        return DWG_ERR_OUTOFMEM;                                              \
      for (vcount = 0; vcount < (BITCODE_BL)size; vcount++)                   \
        {                                                                     \
          FIELD_HANDLE_N (nam[vcount], vcount, code, dxf);                    \
        }                                                                     \
    }

#define HANDLE_VECTOR(nam, sizefield, code, dxf)                              \
  VECTOR_CHKCOUNT_LV (nam, HANDLE, FIELD_VALUE (sizefield), hdl_dat)          \
  HANDLE_VECTOR_N (nam, FIELD_VALUE (sizefield), code, dxf)

#define SUB_HANDLE_VECTOR(o, nam, sizefield, code, dxf)                       \
  if (_obj->o.sizefield > 0)                                                  \
    {                                                                         \
      BITCODE_BL _size = _obj->o.sizefield;                                   \
      HANDLE_VECTOR_CHKCOUNT (nam, _size)                                     \
      _obj->o.nam = (BITCODE_H *)calloc (_size, sizeof (BITCODE_H));          \
      if (!_obj->o.nam)                                                       \
        return DWG_ERR_OUTOFMEM;                                              \
      for (vcount = 0; vcount < _size; vcount++)                              \
        {                                                                     \
          SUB_FIELD_HANDLE (o, nam[vcount], code, dxf);                       \
        }                                                                     \
    }

// count 1 bytes, until non-1 bytes or a terminating zero
#define FIELD_NUM_INSERTS(num_inserts, type, dxf)                             \
  FIELD_VALUE (num_inserts) = 0;                                              \
  vcount = 0;                                                                 \
  while ((vcount = (BITCODE_RC)bit_read_RC (dat)))                            \
    {                                                                         \
      FIELD_VALUE (num_inserts)++;                                            \
      LOG_INSANE ("num_inserts [RC " FORMAT_RL "]: %d\n",                     \
                  FIELD_VALUE (num_inserts), (unsigned char)vcount)           \
      if (vcount != 1)                                                        \
        {                                                                     \
          LOG_WARN ("num_inserts [RC " FORMAT_RL "]: %d != 1",                \
                    FIELD_VALUE (num_inserts), (unsigned char)vcount)         \
          bit_advance_position (dat, -8);                                     \
          break;                                                              \
        }                                                                     \
    }                                                                         \
  LOG_TRACE ("num_inserts: %d [RC* 0]\n", FIELD_VALUE (num_inserts))

#define FIELD_XDATA(name, xdata_size)                                         \
  _obj->name = dwg_decode_xdata (dat, _obj, _obj->xdata_size)

#define REACTORS(code)                                                        \
  if (obj->tio.object->num_reactors > 0)                                      \
    {                                                                         \
      HANDLE_VECTOR_CHKCOUNT (reactors, obj->tio.object->num_reactors)        \
      obj->tio.object->reactors = (BITCODE_H *)calloc (                       \
          obj->tio.object->num_reactors, sizeof (BITCODE_H));                 \
      if (!obj->tio.object->reactors)                                         \
        return DWG_ERR_OUTOFMEM;                                              \
      for (vcount = 0; vcount < obj->tio.object->num_reactors; vcount++)      \
        {                                                                     \
          VALUE_HANDLE_N (obj->tio.object->reactors[vcount], reactors,        \
                          vcount, code, 330);                                 \
        }                                                                     \
    }

#define ENT_REACTORS(code)                                                    \
  if (_ent->num_reactors > 0)                                                 \
    {                                                                         \
      HANDLE_VECTOR_CHKCOUNT (reactors, _ent->num_reactors)                   \
      _ent->reactors                                                          \
          = (BITCODE_H *)calloc (_ent->num_reactors, sizeof (BITCODE_H));     \
      if (!_ent->reactors)                                                    \
        return DWG_ERR_OUTOFMEM;                                              \
      for (vcount = 0; vcount < _ent->num_reactors; vcount++)                 \
        {                                                                     \
          VALUE_HANDLE_N (_ent->reactors[vcount], reactors, vcount, code,     \
                          330);                                               \
        }                                                                     \
    }

#define XDICOBJHANDLE(code)                                                   \
  SINCE (R_2004a)                                                             \
  {                                                                           \
    if (!obj->tio.object->is_xdic_missing)                                    \
      {                                                                       \
        VALUE_HANDLE (obj->tio.object->xdicobjhandle, xdicobjhandle, code,    \
                      360);                                                   \
        if (!obj->tio.object->xdicobjhandle)                                  \
          obj->tio.object->is_xdic_missing = 1;                               \
      }                                                                       \
  }                                                                           \
  else                                                                        \
  {                                                                           \
    SINCE (R_13b1)                                                            \
    {                                                                         \
      VALUE_HANDLE (obj->tio.object->xdicobjhandle, xdicobjhandle, code,      \
                    360);                                                     \
    }                                                                         \
  }

#define ENT_XDICOBJHANDLE(code)                                               \
  SINCE (R_2004a)                                                             \
  {                                                                           \
    if (!_ent->is_xdic_missing)                                               \
      {                                                                       \
        VALUE_HANDLE (_ent->xdicobjhandle, xdicobjhandle, code, 360);         \
      }                                                                       \
  }                                                                           \
  else                                                                        \
  {                                                                           \
    SINCE (R_13b1)                                                            \
    {                                                                         \
      VALUE_HANDLE (_ent->xdicobjhandle, xdicobjhandle, code, 360);           \
    }                                                                         \
  }

#define UNKNOWN_UNTIL(pos)                                                    \
  if (dat->byte < (size_t)(pos))                                              \
    {                                                                         \
      long len = (long)((pos) - dat->byte);                                   \
      BITCODE_TF unknown = bit_read_TF (dat, (size_t)len);                    \
      LOG_TRACE ("unknown (%ld): ", len);                                     \
      LOG_TRACE_TF (unknown, len);                                            \
      free (unknown);                                                         \
    }                                                                         \
  dat->byte = (size_t)(pos)

#define SECTION_STRING_STREAM                                                 \
  {                                                                           \
    Bit_Chain sav_dat = *dat;                                                 \
    dat = str_dat;

// TODO: unused
#define START_STRING_STREAM                                                   \
  obj->has_strings = bit_read_B (dat);                                        \
  if (obj->has_strings)                                                       \
    {                                                                         \
      Bit_Chain sav_dat = *dat;                                               \
      obj_string_stream (dat, obj, dat);

#define END_STRING_STREAM                                                     \
  *dat = sav_dat;                                                             \
  }
/* just checking. skip the has_strings bit. hdl_dat is already set */
#define START_HANDLE_STREAM                                                   \
  {                                                                           \
    size_t _pos = bit_position (dat);                                         \
    if (dat->from_version >= R_2007)                                          \
      _pos++; /* has_strings bit */                                           \
    if (obj->hdlpos != _pos)                                                  \
      {                                                                       \
        LOG_HANDLE (                                                          \
            " handle stream: %+ld @%" PRIuSIZE ".%u %s (@%" PRIuSIZE ".%u "   \
            " @%" PRIuSIZE ".%u)\n",                                          \
            (long)(obj->hdlpos - _pos), dat->byte, (unsigned)dat->bit,        \
            (long)(obj->hdlpos - _pos) >= 8 ? "MISSING"                       \
            : (long)(obj->hdlpos < _pos)    ? "OVERSHOOT"                     \
                                            : "",                                \
            obj->hdlpos / 8, (unsigned)obj->hdlpos % 8, hdl_dat->byte,        \
            (unsigned)hdl_dat->bit);                                          \
        bit_set_position (dat, obj->hdlpos);                                  \
      }                                                                       \
  }

#define REPEAT_CHKCOUNT(name, times, type)                                    \
  if (AVAIL_BITS (dat) < 0)                                                   \
    {                                                                         \
      LOG_ERROR ("Invalid " #name " in %s. No bytes left.\n", SAFEDXFNAME);   \
      if (_obj->name)                                                         \
        {                                                                     \
          free (_obj->name);                                                  \
          _obj->name = NULL;                                                  \
        }                                                                     \
      return DWG_ERR_VALUEOUTOFBOUNDS;                                        \
    }                                                                         \
  LOG_INSANE ("REPEAT_CHKCOUNT %s." #name " x %lu: %lu > %" PRId64 "?\n",     \
              SAFEDXFNAME, (unsigned long)times,                              \
              (unsigned long)((times) * sizeof (type)), AVAIL_BITS (dat));    \
  if ((int64_t)((times) * sizeof (type)) > AVAIL_BITS (dat))                  \
    {                                                                         \
      LOG_ERROR ("Invalid %s." #name " x %lu\n", SAFEDXFNAME,                 \
                 (unsigned long)times);                                       \
      if (_obj->name)                                                         \
        {                                                                     \
          free (_obj->name);                                                  \
          _obj->name = NULL;                                                  \
        }                                                                     \
      return DWG_ERR_VALUEOUTOFBOUNDS;                                        \
    }
#define REPEAT_CHKCOUNT_LVAL(name, times, type)                               \
  if (AVAIL_BITS (dat) < 0)                                                   \
    {                                                                         \
      LOG_ERROR ("Invalid %s." #name ". No bytes left.\n", SAFEDXFNAME);      \
      times = 0;                                                              \
      if (_obj->name)                                                         \
        {                                                                     \
          free (_obj->name);                                                  \
          _obj->name = NULL;                                                  \
        }                                                                     \
      return DWG_ERR_VALUEOUTOFBOUNDS;                                        \
    }                                                                         \
  LOG_INSANE ("REPEAT_CHKCOUNT_LVAL %s." #name " x %lu: %lu > %" PRId64       \
              "?\n",                                                          \
              SAFEDXFNAME, (unsigned long)times,                              \
              (unsigned long)((times) * sizeof (type)), AVAIL_BITS (dat));    \
  if ((int64_t)((times) * sizeof (type)) > AVAIL_BITS (dat)                   \
      || (sizeof (times) > 4 && times > 0xc0000000 / sizeof (type)))          \
    {                                                                         \
      LOG_ERROR ("Invalid %s." #name " x %ld\n", SAFEDXFNAME, (long)times);   \
      times = 0;                                                              \
      if (_obj->name)                                                         \
        {                                                                     \
          free (_obj->name);                                                  \
          _obj->name = NULL;                                                  \
        }                                                                     \
      return DWG_ERR_VALUEOUTOFBOUNDS;                                        \
    }

/* REPEAT names:
  _ adds idx
  C does no checks
  N does constant times (else _obj->times)
  F does not calloc/free
*/

// unchecked with constant times
#define REPEAT_CN(times, name, type)                                          \
  if (times > 0)                                                              \
    {                                                                         \
      _obj->name = (type *)calloc (times, sizeof (type));                     \
      if (!_obj->name)                                                        \
        return DWG_ERR_OUTOFMEM;                                              \
    }                                                                         \
  for (rcount1 = 0; rcount1 < (BITCODE_BL)times; rcount1++)
// checked with constant times
#define REPEAT_N(times, name, type)                                           \
  REPEAT_CHKCOUNT (name, times, type)                                         \
  if (times > 0)                                                              \
    {                                                                         \
      _obj->name = (type *)calloc (times, sizeof (type));                     \
      if (!_obj->name)                                                        \
        return DWG_ERR_OUTOFMEM;                                              \
    }                                                                         \
  for (rcount1 = 0; rcount1 < (BITCODE_BL)times; rcount1++)

// checked with var. times
#define _REPEAT(times, nam, type, idx)                                        \
  REPEAT_CHKCOUNT_LVAL (nam, _obj->times, type)                               \
  if (_obj->times > 0)                                                        \
    {                                                                         \
      _obj->nam = (type *)calloc (_obj->times, sizeof (type));                \
      if (!_obj->nam)                                                         \
        return DWG_ERR_OUTOFMEM;                                              \
    }                                                                         \
  for (rcount##idx = 0; rcount##idx < (BITCODE_BL)_obj->times; rcount##idx++)
// unchecked with var. times
#define _REPEAT_C(times, name, type, idx)                                     \
  if (_obj->times > 0)                                                        \
    _obj->name = (type *)calloc (_obj->times, sizeof (type));                 \
  for (rcount##idx = 0; rcount##idx < (BITCODE_BL)_obj->times; rcount##idx++)
// unchecked with constant times
#define _REPEAT_CN(times, name, type, idx)                                    \
  if (times > 0)                                                              \
    {                                                                         \
      _obj->name = (type *)calloc (times, sizeof (type));                     \
      if (!_obj->name)                                                        \
        return DWG_ERR_OUTOFMEM;                                              \
    }                                                                         \
  if (_obj->name)                                                             \
    for (rcount##idx = 0; rcount##idx < (BITCODE_BL)times; rcount##idx++)

// not allocating versions unchecked: _REPEAT_CNF
// not allocating versions checked: _REPEAT_NF

#define REPEAT(times, name, type) _REPEAT (times, name, type, 1)
#define REPEAT2(times, name, type) _REPEAT (times, name, type, 2)
#define REPEAT3(times, name, type) _REPEAT (times, name, type, 3)
#define REPEAT4(times, name, type) _REPEAT (times, name, type, 4)

#define REPEAT_C(times, name, type) _REPEAT_C (times, name, type, 1)
#define REPEAT2_C(times, name, type) _REPEAT_C (times, name, type, 2)
#define REPEAT3_C(times, name, type) _REPEAT_C (times, name, type, 3)
#define REPEAT4_C(times, name, type) _REPEAT_C (times, name, type, 4)

#define COMMON_ENTITY_HANDLE_DATA                                             \
  SINCE (R_13b1)                                                              \
  {                                                                           \
    START_HANDLE_STREAM;                                                      \
  }

/** Add the empty entity or object with its three structs to the DWG.
    All fields are zero'd. TODO: some are initialized with default values, as
    defined in dwg.spec.
    Returns 0 or DWG_ERR_OUTOFMEM.
*/

#define DWG_ENTITY(token)                                                     \
  EXPORT int dwg_setup_##token (Dwg_Object *obj)                              \
  {                                                                           \
    Dwg_Object_Entity *_ent;                                                  \
    Dwg_Entity_##token *_obj;                                                 \
    if (strEQc (#token, "DIMENSION_ANG2LN")                                   \
        && obj->parent->header.version < R_13b1)                              \
      LOG_INFO ("Add entity DIMENSION [%d] ", obj->index)                     \
    else                                                                      \
      LOG_INFO ("Add entity " #token " [%d] ", obj->index)                    \
    obj->parent->num_entities++;                                              \
    obj->supertype = DWG_SUPERTYPE_ENTITY;                                    \
    if (!(int)obj->fixedtype)                                                 \
      obj->fixedtype = DWG_TYPE_##token;                                      \
    if (!(int)obj->type && obj->fixedtype <= DWG_TYPE_LAYOUT)                 \
      obj->type = DWG_TYPE_##token;                                           \
    if (!obj->dxfname)                                                        \
      {                                                                       \
        if (strlen (#token) > 3 && !memcmp (#token, "_3D", 3))                \
          obj->name = obj->dxfname = (char *)&#token[1];                      \
        else if (strEQc (#token, "PROXY_ENTITY"))                             \
          {                                                                   \
            obj->dxfname = (char *)"ACAD_PROXY_ENTITY";                       \
            obj->name = (char *)#token;                                       \
          }                                                                   \
        else                                                                  \
          obj->name = obj->dxfname = (char *)#token;                          \
      }                                                                       \
    else if (!obj->name)                                                      \
      {                                                                       \
        if (strlen (#token) > 3 && !memcmp (#token, "_3D", 3))                \
          obj->name = (char *)&#token[1];                                     \
        else                                                                  \
          obj->name = (char *)#token;                                         \
      }                                                                       \
    if (obj->parent && obj->parent->opts & DWG_OPTS_IN)                       \
      {                                                                       \
        obj->dxfname = strdup (obj->dxfname);                                 \
        if (obj->parent->opts & DWG_OPTS_INJSON)                              \
          obj->name = strdup (obj->name);                                     \
      }                                                                       \
    _ent = obj->tio.entity                                                    \
        = (Dwg_Object_Entity *)calloc (1, sizeof (Dwg_Object_Entity));        \
    if (!_ent)                                                                \
      return DWG_ERR_OUTOFMEM;                                                \
    _ent->tio.token                                                           \
        = (Dwg_Entity_##token *)calloc (1, sizeof (Dwg_Entity_##token));      \
    if (!_ent->tio.token)                                                     \
      return DWG_ERR_OUTOFMEM;                                                \
    _ent->dwg = obj->parent;                                                  \
    _ent->objid = obj->index; /* obj ptr itself might move */                 \
    _ent->tio.token->parent = obj->tio.entity;                                \
    return 0;                                                                 \
  }                                                                           \
                                                                              \
  static int dwg_decode_##token##_private (                                   \
      Bit_Chain *dat, Bit_Chain *hdl_dat, Bit_Chain *str_dat,                 \
      Dwg_Object *restrict obj);                                              \
                                                                              \
  /**Call dwg_setup_##token and write the fields from the bitstream dat to    \
   * the entity or object. */                                                 \
  static int dwg_decode_##token (Bit_Chain *restrict dat,                     \
                                 Dwg_Object *restrict obj)                    \
  {                                                                           \
    int error = dwg_setup_##token (obj);                                      \
    Bit_Chain hdl_dat = *dat;                                                 \
    if (error)                                                                \
      return error;                                                           \
    SINCE (R_2007a)                                                           \
    {                                                                         \
      Bit_Chain obj_dat = *dat, str_dat = *dat;                               \
      error                                                                   \
          = dwg_decode_##token##_private (&obj_dat, &hdl_dat, &str_dat, obj); \
    }                                                                         \
    else                                                                      \
    {                                                                         \
      error = dwg_decode_##token##_private (dat, &hdl_dat, dat, obj);         \
    }                                                                         \
    return error;                                                             \
  }                                                                           \
                                                                              \
  GCC30_DIAG_IGNORE (-Wformat-nonliteral)                                     \
  static int dwg_decode_##token##_private (                                   \
      Bit_Chain *dat, Bit_Chain *hdl_dat, Bit_Chain *str_dat,                 \
      Dwg_Object *restrict obj)                                               \
  {                                                                           \
    BITCODE_BL vcount, rcount3, rcount4;                                      \
    int error = 0;                                                            \
    Dwg_Entity_##token *ent, *_obj;                                           \
    Dwg_Object_Entity *_ent;                                                  \
    Dwg_Data *dwg = obj->parent;                                              \
    LOG_INFO ("Decode entity " #token "\n")                                   \
    _ent = obj->tio.entity;                                                   \
    ent = obj->tio.entity->tio.token;                                         \
    _obj = ent;                                                               \
    _ent->dwg = dwg;                                                          \
    _ent->objid = obj->index; /* obj ptr itself might move */                 \
    _obj->parent = obj->tio.entity;                                           \
    SINCE (R_13b1)                                                            \
    {                                                                         \
      error = dwg_decode_entity (dat, hdl_dat, str_dat, _ent);                \
    }                                                                         \
    else                                                                      \
    {                                                                         \
      error = decode_entity_preR13 (dat, obj, _ent);                          \
    }                                                                         \
    if (error >= DWG_ERR_CRITICAL || dat->byte > dat->size)                   \
      return error;

// Does size include the CRC?
#define DWG_ENTITY_END                                                        \
  SINCE (R_13b1)                                                              \
  {                                                                           \
    size_t pos = obj_stream_position (dat, hdl_dat, str_dat);                 \
    int64_t padding = (obj->size * 8) - pos;                                  \
    bit_set_position (dat, pos);                                              \
    if (padding)                                                              \
      LOG_HANDLE (" padding: %+ld %s\n", (long)padding,                       \
                  padding >= 8    ? "MISSING"                                 \
                  : (padding < 0) ? "OVERSHOOT"                               \
                                  : "");                                      \
  }                                                                           \
  return error & ~DWG_ERR_UNHANDLEDCLASS;                                     \
  }

#define DWG_OBJECT(token)                                                     \
  EXPORT int dwg_setup_##token (Dwg_Object *obj)                              \
  {                                                                           \
    Dwg_Object_##token *_obj;                                                 \
    LOG_INFO ("Add object " #token " [%d] ", obj->index)                      \
    obj->supertype = DWG_SUPERTYPE_OBJECT;                                    \
    obj->tio.object                                                           \
        = (Dwg_Object_Object *)calloc (1, sizeof (Dwg_Object_Object));        \
    if (!obj->tio.object)                                                     \
      return DWG_ERR_OUTOFMEM;                                                \
    _obj = obj->tio.object->tio.token                                         \
        = (Dwg_Object_##token *)calloc (1, sizeof (Dwg_Object_##token));      \
    if (!_obj)                                                                \
      {                                                                       \
        free (obj->tio.object);                                               \
        obj->tio.object = NULL;                                               \
        obj->fixedtype = DWG_TYPE_FREED;                                      \
        return DWG_ERR_OUTOFMEM;                                              \
      }                                                                       \
    if (!(int)obj->fixedtype)                                                 \
      obj->fixedtype = DWG_TYPE_##token;                                      \
    if (!obj->name)                                                           \
      obj->name = (char *)#token;                                             \
    if (!(int)obj->type && obj->fixedtype <= DWG_TYPE_LAYOUT)                 \
      obj->type = DWG_TYPE_##token;                                           \
    if (!obj->dxfname)                                                        \
      {                                                                       \
        if (strEQc (#token, "PLACEHOLDER"))                                   \
          obj->dxfname = (char *)"ACDBPLACEHOLDER";                           \
        else if (strEQc (#token, "PROXY_OBJECT"))                             \
          obj->dxfname = (char *)"ACAD_PROXY_OBJECT";                         \
        else                                                                  \
          obj->dxfname = (char *)#token;                                      \
      }                                                                       \
    if (obj->parent && obj->parent->opts & DWG_OPTS_IN)                       \
      {                                                                       \
        obj->dxfname = strdup (obj->dxfname);                                 \
        if (obj->parent->opts & DWG_OPTS_INJSON)                              \
          obj->name = strdup (obj->name);                                     \
      }                                                                       \
    _obj->parent = obj->tio.object;                                           \
    obj->tio.object->dwg = obj->parent;                                       \
    obj->tio.object->objid = obj->index; /* obj ptr itself might move */      \
    return 0;                                                                 \
  }                                                                           \
  static int dwg_decode_##token##_private (                                   \
      Bit_Chain *obj_dat, Bit_Chain *hdl_dat, Bit_Chain *str_dat,             \
      Dwg_Object *restrict obj);                                              \
                                                                              \
  static int dwg_decode_##token (Bit_Chain *restrict dat,                     \
                                 Dwg_Object *restrict obj)                    \
  {                                                                           \
    int error = dwg_setup_##token (obj);                                      \
    Bit_Chain hdl_dat = *dat;                                                 \
    if (error)                                                                \
      return error;                                                           \
    SINCE (R_2007a)                                                           \
    {                                                                         \
      Bit_Chain obj_dat = *dat, str_dat = *dat;                               \
      error                                                                   \
          = dwg_decode_##token##_private (&obj_dat, &hdl_dat, &str_dat, obj); \
    }                                                                         \
    else                                                                      \
    {                                                                         \
      error = dwg_decode_##token##_private (dat, &hdl_dat, dat, obj);         \
    }                                                                         \
    return error;                                                             \
  }                                                                           \
                                                                              \
  GCC30_DIAG_IGNORE (-Wformat-nonliteral)                                     \
  static int dwg_decode_##token##_private (                                   \
      Bit_Chain *dat, Bit_Chain *hdl_dat, Bit_Chain *str_dat,                 \
      Dwg_Object *restrict obj)                                               \
  {                                                                           \
    BITCODE_BL vcount, rcount3, rcount4;                                      \
    int error = 0;                                                            \
    Dwg_Object_##token *_obj = NULL;                                          \
    Dwg_Data *dwg = obj->parent;                                              \
    LOG_INFO ("Decode object " #token "\n")                                   \
    if (strNE (#token, "TABLECONTENT") || obj->fixedtype != DWG_TYPE_TABLE)   \
      {                                                                       \
        _obj = obj->tio.object->tio.token;                                    \
        error = dwg_decode_object (dat, hdl_dat, str_dat, obj->tio.object);   \
        if (error >= DWG_ERR_CRITICAL || dat->byte > dat->size)               \
          return error;                                                       \
      }

#define DWG_OBJECT_END DWG_ENTITY_END

// only for exported BLOCK_HEADER, LAYER, STYLE, LTYPE, VIEW, UCS, VPORT,
// APPID, DIMSTYLE, VX_TABLE_RECORD table records, needed for the r11 add API.
#define DWG_TABLE(token)                                                      \
  EXPORT int dwg_setup_##token (Dwg_Object *obj)                              \
  {                                                                           \
    Dwg_Object_##token *_obj;                                                 \
    LOG_INFO ("Add table record " #token " [%d] ", obj->index)                \
    obj->supertype = DWG_SUPERTYPE_OBJECT;                                    \
    obj->tio.object                                                           \
        = (Dwg_Object_Object *)calloc (1, sizeof (Dwg_Object_Object));        \
    if (!obj->tio.object)                                                     \
      return DWG_ERR_OUTOFMEM;                                                \
    _obj = obj->tio.object->tio.token                                         \
        = (Dwg_Object_##token *)calloc (1, sizeof (Dwg_Object_##token));      \
    if (!_obj)                                                                \
      {                                                                       \
        free (obj->tio.object);                                               \
        obj->tio.object = NULL;                                               \
        obj->fixedtype = DWG_TYPE_FREED;                                      \
        return DWG_ERR_OUTOFMEM;                                              \
      }                                                                       \
    if (!(int)obj->fixedtype)                                                 \
      obj->fixedtype = DWG_TYPE_##token;                                      \
    if (!obj->name)                                                           \
      obj->name = (char *)#token;                                             \
    if (!(int)obj->type && obj->fixedtype <= DWG_TYPE_LAYOUT)                 \
      obj->type = DWG_TYPE_##token;                                           \
    obj->dxfname = (char *)#token;                                            \
    if (obj->parent->opts & DWG_OPTS_IN)                                      \
      {                                                                       \
        obj->dxfname = strdup (obj->dxfname);                                 \
        if (obj->parent->opts & DWG_OPTS_INJSON)                              \
          obj->name = strdup (obj->name);                                     \
      }                                                                       \
    _obj->parent = obj->tio.object;                                           \
    obj->tio.object->dwg = obj->parent;                                       \
    obj->tio.object->objid = obj->index; /* obj ptr itself might move */      \
    return 0;                                                                 \
  }                                                                           \
  static int dwg_decode_##token##_private (                                   \
      Bit_Chain *obj_dat, Bit_Chain *hdl_dat, Bit_Chain *str_dat,             \
      Dwg_Object *restrict obj);                                              \
                                                                              \
  int dwg_decode_##token (Bit_Chain *restrict dat, Dwg_Object *restrict obj)  \
  {                                                                           \
    int error = dwg_setup_##token (obj);                                      \
    Bit_Chain hdl_dat = *dat;                                                 \
    if (error)                                                                \
      return error;                                                           \
    SINCE (R_2007a)                                                           \
    {                                                                         \
      Bit_Chain obj_dat = *dat, str_dat = *dat;                               \
      error                                                                   \
          = dwg_decode_##token##_private (&obj_dat, &hdl_dat, &str_dat, obj); \
    }                                                                         \
    else                                                                      \
    {                                                                         \
      error = dwg_decode_##token##_private (dat, &hdl_dat, dat, obj);         \
    }                                                                         \
    return error;                                                             \
  }                                                                           \
                                                                              \
  GCC30_DIAG_IGNORE (-Wformat-nonliteral)                                     \
  static int dwg_decode_##token##_private (                                   \
      Bit_Chain *dat, Bit_Chain *hdl_dat, Bit_Chain *str_dat,                 \
      Dwg_Object *restrict obj)                                               \
  {                                                                           \
    BITCODE_BL vcount, rcount3, rcount4;                                      \
    int error = 0;                                                            \
    Dwg_Object_##token *_obj = NULL;                                          \
    Dwg_Data *dwg = obj->parent;                                              \
    LOG_INFO ("Decode table record " #token "\n")                             \
    _obj = obj->tio.object->tio.token;                                        \
    error = dwg_decode_object (dat, hdl_dat, str_dat, obj->tio.object);       \
    if (error >= DWG_ERR_CRITICAL || dat->byte > dat->size)                   \
      return error;

#endif
