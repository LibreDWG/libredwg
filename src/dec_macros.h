/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2009-2019 Free Software Foundation, Inc.                   */
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
#ifdef HAVE_CTYPE_H
#  include <ctype.h>
#endif

// needed by decode and decode_r2007
#define ACTION decode
#define IS_DECODER

#define VALUE(value, type, dxf)                                               \
  LOG_TRACE (FORMAT_##type " [" #type " %d]\n", value, dxf)
#define VALUE_RC(value, dxf) VALUE (value, RC, dxf)
#define VALUE_RS(value, dxf) VALUE (value, RS, dxf)
#define VALUE_RL(value, dxf) VALUE (value, RL, dxf)
#define VALUE_RD(value, dxf) VALUE (value, RD, dxf)

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
        LOG_TRACE (#nam ": " FORMAT_##type " [" #type " %d]", _obj->nam,      \
                   dxfgroup);                                                 \
      LOG_INSANE (" @%lu.%u", dat->byte, dat->bit)                            \
      LOG_TRACE ("\n")                                                        \
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
    if (DWG_LOGLEVEL >= DWG_LOGLEVEL_##level || len <= 256)                   \
      {                                                                       \
        for (int _i = 0; _i < (len); _i++)                                    \
          {                                                                   \
            LOG (level, "%02X", (unsigned char)((char *)var)[_i]);            \
          }                                                                   \
        LOG (level, "\n");                                                    \
      }                                                                       \
    if (DWG_LOGLEVEL >= DWG_LOGLEVEL_INSANE)                                  \
      {                                                                       \
        for (int _i = 0; _i < (len); _i++)                                    \
          {                                                                   \
            unsigned char c = ((unsigned char *)var)[_i];                     \
            LOG_INSANE ("%-2c", isprint (c) ? c : ' ');                       \
          }                                                                   \
        LOG_INSANE ("\n");                                                    \
      }                                                                       \
  }
#define LOG_TRACE_TF(var, len) LOG_TF (TRACE, var, len)
#define LOG_INSANE_TF(var, len) LOG_TF (INSANE, var, len)

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
      LOG_INSANE (" @%lu.%u", dat->byte, dat->bit)                            \
      LOG_TRACE ("\n")                                                        \
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
                     ") [" #type " %d]",                                      \
                     _obj->nam.x, _obj->nam.y, _obj->nam.z, dxf)              \
        }                                                                     \
      LOG_INSANE (" @%lu.%u", dat->byte, dat->bit)                            \
      LOG_TRACE ("\n")                                                        \
    }

#define FIELD_VALUE(nam) _obj->nam

#define ANYCODE -1
#define VALUE_HANDLE(ref, nam, code, dxf)                                     \
  {                                                                           \
    unsigned long pos = bit_position (hdl_dat);                               \
    if (code >= 0)                                                            \
      ref = dwg_decode_handleref_with_code (hdl_dat, obj, dwg, code);         \
    else                                                                      \
      ref = dwg_decode_handleref (hdl_dat, obj, dwg);                         \
    if (DWG_LOGLEVEL >= DWG_LOGLEVEL_TRACE)                                   \
      {                                                                       \
        if (ref)                                                              \
          {                                                                   \
            LOG_TRACE (#nam ": " FORMAT_REF " [H %d]", ARGS_REF (ref), dxf);  \
            if (dwg_ref_object_silent (dwg, ref)                              \
                && DWG_LOGLEVEL > DWG_LOGLEVEL_TRACE)                         \
              {                                                               \
                const char *u8 = dwg_ref_tblname (dwg, ref);                  \
                HANDLER (OUTPUT, " => %s %s", dwg_ref_objname (dwg, ref), u8);\
                if (dwg->header.version >= R_2007 && u8 && *u8)               \
                  free ((void*)u8);                                           \
              }                                                               \
          }                                                                   \
        else                                                                  \
          {                                                                   \
            LOG_TRACE (#nam ": NULL %d [H %d]", code, dxf);                   \
          }                                                                   \
        LOG_INSANE (" @%lu.%u", pos / 8, (unsigned)(pos % 8));                \
        LOG_TRACE ("\n");                                                     \
      }                                                                       \
  }
#define FIELD_HANDLE(nam, code, dxf) VALUE_HANDLE (_obj->nam, nam, code, dxf)
#define SUB_FIELD_HANDLE(o, nam, code, dxf)                                   \
  VALUE_HANDLE (_obj->o.nam, o.nam, code, dxf)

#define VALUE_HANDLE_N(ref, nam, vcount, code, dxf)                           \
  {                                                                           \
    unsigned long pos = bit_position (hdl_dat);                               \
    if (code >= 0)                                                            \
      ref = dwg_decode_handleref_with_code (hdl_dat, obj, dwg, code);         \
    else                                                                      \
      ref = dwg_decode_handleref (hdl_dat, obj, dwg);                         \
    if (DWG_LOGLEVEL >= DWG_LOGLEVEL_TRACE)                                   \
      {                                                                       \
        if (ref)                                                              \
          {                                                                   \
            LOG_TRACE (#nam "[%d]: " FORMAT_REF " [H* %d]", (int)vcount,      \
                       ARGS_REF (ref), dxf);                                  \
            if (dwg_ref_object_silent (dwg, ref)                              \
                && DWG_LOGLEVEL > DWG_LOGLEVEL_TRACE)                         \
              {                                                               \
                const char *u8 = dwg_ref_tblname (dwg, ref);                  \
                HANDLER (OUTPUT, " => %s %s", dwg_ref_objname (dwg, ref), u8);\
                if (dwg->header.version >= R_2007 && u8 && *u8)               \
                  free ((void*)u8);                                           \
              }                                                               \
          }                                                                   \
        else                                                                  \
          {                                                                   \
            LOG_TRACE (#nam "[%d]: NULL %d [H* %d]", (int)vcount, code, dxf); \
          }                                                                   \
        LOG_INSANE (" @%lu.%u", pos / 8, (unsigned)(pos % 8));                \
        LOG_TRACE ("\n");                                                     \
      }                                                                       \
  }
#define FIELD_HANDLE_N(nam, vcount, code, dxf)                                \
  VALUE_HANDLE_N (_obj->nam, nam, vcount, code, dxf)

#define FIELD_DATAHANDLE(nam, code, dxf)                                      \
  {                                                                           \
    _obj->nam = dwg_decode_handleref (dat, obj, dwg);                         \
    if (_obj->nam)                                                            \
      {                                                                       \
        LOG_TRACE (#nam ": " FORMAT_H " [H %d]\n",                            \
                   ARGS_H (_obj->nam->handleref), dxf);                       \
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
        LOG_TRACE (#nam ": " FORMAT_BD " [BD %d]  %gº\n", _obj->nam, dxf,     \
                   rad2deg (_obj->nam));                                      \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        FIELD_G_TRACE (nam, BD, dxf);                                         \
      }                                                                       \
  }
#define FIELD_BLx(nam, dxf)                                                   \
  {                                                                           \
    _obj->nam = bit_read_BL (dat);                                            \
    LOG_TRACE (#nam ": 0x%x [BLx %d]\n", (uint32_t)_obj->nam, dxf);           \
  }
#define FIELD_RLx(nam, dxf)                                                   \
  {                                                                           \
    _obj->nam = bit_read_RL (dat);                                            \
    LOG_TRACE (#nam ": 0x%x [RLx %d]\n", (uint32_t)_obj->nam, dxf);           \
  }
#define FIELD_BSx(nam, dxf)                                                   \
  {                                                                           \
    _obj->nam = bit_read_BS (dat);                                            \
    LOG_TRACE (#nam ": 0x%x [BSx %d]\n", _obj->nam, dxf);                     \
  }
#define FIELD_BSd(nam, dxf)                                                   \
  {                                                                           \
    _obj->nam = bit_read_BS (dat);                                            \
    LOG_TRACE (#nam ": " FORMAT_BSd " [BSd %d]\n", _obj->nam, dxf);           \
  }
#define SUB_FIELD_BSd(o, nam, dxf)                                            \
  {                                                                           \
    _obj->o.nam = (BITCODE_BSd)bit_read_BS (dat);                             \
    LOG_TRACE (#o "." #nam ": " FORMAT_BSd " [BSd %d]\n", _obj->o.nam, dxf);  \
  }
#define FIELD_BLd(nam, dxf)                                                   \
  {                                                                           \
    _obj->nam = bit_read_BL (dat);                                            \
    LOG_TRACE (#nam ": " FORMAT_BLd " [BLd %d]\n", _obj->nam, dxf);           \
  }
#define FIELD_RLd(nam, dxf)                                                   \
  {                                                                           \
    _obj->nam = bit_read_RL (dat);                                            \
    LOG_TRACE (#nam ": " FORMAT_RLd " [RLd %d]\n", _obj->nam, dxf);           \
  }
#define FIELD_RC(nam, dxf) FIELDG (nam, RC, dxf)
#define FIELD_RCu(nam, dxf)                                                   \
  {                                                                           \
    _obj->nam = bit_read_RC (dat);                                            \
    LOG_TRACE (#nam ": %u [RC %d]\n", (unsigned)((unsigned char)_obj->nam),   \
               dxf);                                                          \
  }
#define FIELD_RCx(nam, dxf)                                                   \
  {                                                                           \
    _obj->nam = bit_read_RC (dat);                                            \
    LOG_TRACE (#nam ": 0x%x [RCx %d]\n", (unsigned)_obj->nam, dxf);           \
  }
#define FIELD_RCd(nam, dxf)                                                   \
  {                                                                           \
    _obj->nam = bit_read_RC (dat);                                            \
    LOG_TRACE (#nam ": %d [RC %d]\n", (int)((signed char)_obj->nam), dxf);    \
  }
#define FIELD_RS(nam, dxf) FIELDG (nam, RS, dxf)
#define FIELD_RSx(nam, dxf)                                                   \
  {                                                                           \
    _obj->nam = bit_read_RS (dat);                                            \
    LOG_TRACE (#nam ": " FORMAT_RSx " [RSx %d]\n", _obj->nam, dxf);           \
  }
#define FIELD_RD(nam, dxf)                                                    \
  {                                                                           \
    FIELDG (nam, RD, dxf);                                                    \
    if (bit_isnan (_obj->nam))                                                \
      {                                                                       \
        LOG_ERROR ("Invalid RD " #nam);                                       \
        return DWG_ERR_VALUEOUTOFBOUNDS;                                      \
      }                                                                       \
  }
#define FIELD_RL(nam, dxf) FIELDG (nam, RL, dxf)
#define FIELD_RLL(nam, dxf) FIELDG (nam, RLL, dxf)
#define FIELD_MC(nam, dxf) FIELDG (nam, MC, dxf)
#define FIELD_MS(nam, dxf) FIELDG (nam, MS, dxf)
/* preR13 we have no obj->address and obj->size yet, skip VECTOR_CHKCOUNT */
// clang-format off
#define FIELD_TF(nam, len, dxf)                                               \
  {                                                                           \
    SINCE (R_13) { _obj->nam = NULL; VECTOR_CHKCOUNT (nam, TF, len, dat) }    \
    _obj->nam = bit_read_TF (dat, (int)len);                                  \
    LOG_TRACE (#nam ": \"%s\" [TF %lu " #dxf "]\n", _obj->nam,                \
               (unsigned long)len);                                           \
    LOG_TRACE_TF (FIELD_VALUE (nam), (int)len);                               \
  }
#define FIELD_TFv(nam, len, dxf)                                              \
  {                                                                           \
    SINCE (R_13) { _obj->nam = NULL; VECTOR_CHKCOUNT (nam, TF, len, dat) }    \
    _obj->nam = (BITCODE_TV)bit_read_TF (dat, (int)len);                      \
    LOG_TRACE (#nam ": \"%s\" [TF %lu " #dxf "]\n", _obj->nam,                \
               (unsigned long)len);                                           \
    LOG_TRACE_TF (FIELD_VALUE (nam), (int)len);                               \
  }
#define FIELD_TFF(nam, len, dxf)                                              \
  {                                                                           \
    SINCE (R_13) { VECTOR_CHKCOUNT (nam, TF, len, dat) }                      \
    bit_read_fixed (dat, _obj->nam, (int)len);                                \
    LOG_TRACE (#nam ": \"%s\" [TFF %d " #dxf "]\n", _obj->nam, (int)len);     \
    LOG_TRACE_TF (FIELD_VALUE (nam), (int)len);                               \
  }
#define FIELD_TFFx(nam, len, dxf)                                             \
  {                                                                           \
    SINCE (R_13) { VECTOR_CHKCOUNT (nam, TF, len, dat) }                      \
    bit_read_fixed (dat, (BITCODE_RC*)_obj->nam, (int)len);                   \
    LOG_TRACE (#nam ": [TFFx %d " #dxf "]\n  ", (int)len);                    \
    LOG_TRACE_TF ((BITCODE_RC*)_obj->nam, (int)len);                          \
  }
#define FIELD_T16(nam, dxf) FIELDG (nam, T16, dxf)
#define FIELD_TU16(nam, dxf)                                                  \
  {                                                                           \
    _obj->nam = bit_read_TU16 (dat);                                          \
    LOG_TRACE_TU (#nam, FIELD_VALUE (nam), dxf);                              \
  }
#define FIELD_TU32(nam, dxf)                                                  \
  {                                                                           \
    _obj->nam = bit_read_TU32 (dat);                                          \
    if (dat->version < R_2007)                                                \
      {                                                                       \
        LOG_TRACE (#nam ": \"%s\" [TU32 %d]\n", _obj->nam, dxf)               \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        LOG_TRACE_TU (#nam, FIELD_VALUE (nam), dxf)                           \
      }                                                                       \
  }
#define FIELD_T32(nam, dxf)                                                   \
  {                                                                           \
    _obj->nam = bit_read_T32 (dat);                                           \
    if (dat->version < R_2007)                                                \
      {                                                                       \
        LOG_TRACE (#nam ": \"%s\" [T32 %d]\n", _obj->nam, dxf);               \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        LOG_TRACE_TU (#nam, FIELD_VALUE (nam), dxf)                           \
      }                                                                       \
  }
#define FIELD_TV(nam, dxf)                                                    \
  {                                                                           \
    _obj->nam = bit_read_TV (dat);                                            \
    LOG_TRACE (#nam ": \"%s\" [TV %d]", _obj->nam, dxf);                      \
    LOG_INSANE (" @%lu.%u", dat->byte, dat->bit)                              \
    LOG_TRACE ("\n")                                                          \
  }
#define FIELD_TU(nam, dxf)                                                    \
  {                                                                           \
    _obj->nam = (BITCODE_TU)bit_read_TU (str_dat);                            \
    LOG_TRACE_TU (#nam, (BITCODE_TU)FIELD_VALUE (nam), dxf);                  \
  }
// clang-format on
#define FIELD_T(nam, dxf)                                                     \
  {                                                                           \
    if (dat->version < R_2007)                                                \
      {                                                                       \
        _obj->nam = bit_read_TV (dat);                                        \
        LOG_TRACE (#nam ": \"%s\" [T %d]", _obj->nam, dxf);                   \
        LOG_INSANE (" @%lu.%u", dat->byte, dat->bit)                          \
        LOG_TRACE ("\n")                                                      \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        if (!obj || obj->has_strings) /* header_vars */                       \
          {                                                                   \
            _obj->nam = (BITCODE_T)bit_read_TU (str_dat);                     \
            LOG_TRACE_TU (#nam, (BITCODE_TU)FIELD_VALUE (nam), dxf);          \
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
    LOG_TRACE (#nam ": b%d%d%d%d [4BITS %d]\n", _b & 8, _b & 4, _b & 2,       \
               _b & 1, dxf);                                                  \
  }

#define FIELD_BE(nam, dxf)                                                    \
  {                                                                           \
    bit_read_BE (dat, &_obj->nam.x, &_obj->nam.y, &_obj->nam.z);              \
    if (_obj->nam.z != 1.0)                                                   \
      {                                                                       \
        LOG_TRACE (#nam ": (%f, %f, %f) [BE %d]\n", _obj->nam.x, _obj->nam.y, \
                   _obj->nam.z, dxf);                                         \
      }                                                                       \
  }
#define TRACE_DD                                                              \
  {                                                                           \
    BITCODE_BB result;                                                        \
    BITCODE_RC byte = dat->chain[dat->byte];                                  \
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
#define FIELD_2DD(nam, dx, dy, dxf)                                           \
  {                                                                           \
    FIELD_DD (nam.x, dx, dxf);                                                \
    FIELD_DD (nam.y, dy, dxf + 10);                                           \
    FIELD_2PT_TRACE (nam, 2DD, dxf);                                           \
  }
#define FIELD_3DD(nam, def, dxf)                                              \
  {                                                                           \
    FIELD_DD (nam.x, FIELD_VALUE (def.x), dxf);                               \
    FIELD_DD (nam.y, FIELD_VALUE (def.y), dxf + 10);                          \
    FIELD_DD (nam.z, FIELD_VALUE (def.z), dxf + 20);                          \
    FIELD_3PT_TRACE (nam, 3DD, dxf);                                           \
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
    FIELD_3PT_TRACE (nam, 3RD, dxf);                                           \
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
    FIELD_3PT_TRACE (nam, 3BD, dxf);                                           \
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
    FIELD_2PT_TRACE (nam, 2RD, dxf);                                           \
  }
#define FIELD_2BD(nam, dxf)                                                   \
  {                                                                           \
    _obj->nam.x = bit_read_BD (dat);                                          \
    _obj->nam.y = bit_read_BD (dat);                                          \
    if (bit_isnan (_obj->nam.x) || bit_isnan (_obj->nam.y))                   \
      {                                                                       \
        LOG_ERROR ("Invalid 2BD " #nam);                                      \
        return DWG_ERR_VALUEOUTOFBOUNDS;                                      \
      }                                                                       \
    FIELD_2PT_TRACE (nam, 2BD, dxf);                                           \
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
    FIELD_2PT_TRACE (nam, 2BD_1, dxf);                                           \
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
    FIELD_3PT_TRACE (nam, 3BD_1, dxf);                                           \
  }
//    FIELDG(nam.x, BD, dxf); FIELDG(nam.y, BD, dxf+1);
//    FIELDG(nam.z, BD, dxf+2); }
#define FIELD_3DPOINT(nam, dxf) FIELD_3BD (nam, dxf)
#define FIELD_3DVECTOR(nam, dxf) FIELD_3BD_1 (nam, dxf)
#define FIELD_TIMEBLL(nam, dxf)                                               \
  {                                                                           \
    _obj->nam = bit_read_TIMEBLL (dat);                                       \
    LOG_TRACE (#nam ": %.8f  (" FORMAT_BL ", " FORMAT_BL ") [TIMEBLL %d]",    \
               _obj->nam.value, _obj->nam.days, _obj->nam.ms, dxf);           \
    LOG_INSANE (" @%lu.%u", dat->byte, dat->bit)                              \
    LOG_TRACE ("\n")                                                          \
  }
#define FIELD_TIMERLL(nam, dxf)                                               \
  {                                                                           \
    _obj->nam = bit_read_TIMERLL (dat);                                       \
    LOG_TRACE (#nam ": %.8f  (" FORMAT_RL ", " FORMAT_RL ") [TIMERLL %d]",    \
               _obj->nam.value, _obj->nam.days, _obj->nam.ms, dxf);           \
    LOG_INSANE (" @%lu.%u", dat->byte, dat->bit)                              \
    LOG_TRACE ("\n")                                                          \
  }
#define FIELD_CMC(color, dxf1, dxf2)                                          \
  {                                                                           \
    bit_read_CMC (dat, &_obj->color);                                         \
    LOG_TRACE (#color ".index: %d [CMC.BS %d]", _obj->color.index, dxf1);     \
    LOG_INSANE (" @%lu.%u", dat->byte, dat->bit);                             \
    LOG_TRACE ("\n");                                                         \
    if (dat->version >= R_2004)                                               \
      {                                                                       \
        LOG_TRACE (#color ".rgb: 0x%06x [CMC.BL %d]\n",                       \
                   (unsigned)_obj->color.rgb, dxf2);                          \
        LOG_TRACE (#color ".flag: 0x%x [CMC.RC]\n",                           \
                   (unsigned)_obj->color.flag);                               \
        if (_obj->color.flag & 1)                                             \
          {                                                                   \
            LOG_TRACE (#color ".name: %s [CMC.TV]\n", _obj->color.name);      \
          }                                                                   \
        if (_obj->color.flag & 2)                                             \
          {                                                                   \
            LOG_TRACE (#color ".bookname: %s [CMC.TV]\n",                     \
                     _obj->color.book_name);                                  \
          }                                                                   \
      }                                                                       \
  }
#define SUB_FIELD_CMC(o, color, dxf1, dxf2)                                   \
  {                                                                           \
    bit_read_CMC (dat, &_obj->o.color);                                       \
    LOG_TRACE (#o "." #color ".index: %d [CMC.BS %d]\n", _obj->o.color.index, dxf1); \
    LOG_INSANE (" @%lu.%u\n", dat->byte, dat->bit)                            \
    if (dat->version >= R_2004)                                               \
      {                                                                       \
        LOG_TRACE (#o "." #color ".rgb: 0x%06x [CMC.BL %d]\n",                \
                   (unsigned)_obj->o.color.rgb, dxf2);                        \
        LOG_TRACE (#o "." #color ".flag: 0x%x [CMC.RC]\n",                    \
                   (unsigned)_obj->o.color.flag);                             \
        if (_obj->o.color.flag & 1)                                           \
          {                                                                   \
            LOG_TRACE (#o "." #color ".name: %s [CMC.TV]\n", _obj->o.color.name);\
          }                                                                   \
        if (_obj->o.color.flag & 2)                                           \
          {                                                                   \
            LOG_TRACE (#o "." #color ".bookname: %s [CMC.TV]\n",              \
                     _obj->o.color.book_name);                                \
          }                                                                   \
      }                                                                       \
  }
#define FIELD_ENC(color, dxf1, dxf2)                                          \
  {                                                                           \
    bit_read_ENC (dat, hdl_dat, str_dat, &_obj->color);                       \
    LOG_TRACE (#color ".index: %d [ENC.BS %d]\n", _obj->color.index, dxf1);   \
    if (dat->version >= R_2004)                                               \
      {                                                                       \
        if (_obj->color.flag)                                                 \
          LOG_TRACE (#color ".flag: 0x%x\n", (unsigned)_obj->color.flag);     \
        if (_obj->color.flag & 0x20)                                          \
          LOG_TRACE (#color ".alpha: %d [ENC.BL %d]\n",                       \
                     (int)_obj->color.alpha, dxf2 + 20);                      \
        if (_obj->color.flag & 0x80)                                          \
          LOG_TRACE (#color ".rgb: 0x%06x [ENC.BL %d]\n",                     \
                     (unsigned)_obj->color.rgb, dxf2);                        \
        if (_obj->color.flag & 0x40 && _obj->color.handle)                    \
          LOG_TRACE (#color ".handle: %X [ENC.H %d]\n",                       \
                     _obj->color.handle->handleref.value, dxf2 + 10);         \
      }                                                                       \
  }
#define SUB_FIELD_ENC(o, color, dxf1, dxf2)                                   \
  {                                                                           \
    bit_read_ENC (dat, hdl_dat, str_dat, &_obj->o.color);                     \
    LOG_TRACE (#o "." #color ".index: %d [ENC.BS %d]\n", _obj->o.color.index, dxf1); \
    if (dat->version >= R_2004)                                               \
      {                                                                       \
        if (_obj->o.color.flag)                                               \
          LOG_TRACE (#o "." #color ".flag: 0x%x\n", (unsigned)_obj->o.color.flag);   \
        if (_obj->o.color.flag & 0x20)                                        \
          LOG_TRACE (#o "." #color ".alpha: %d [ENC.BL %d]\n",                       \
                     (unsigned)_obj->o.color.alpha, dxf2 + 20);               \
        if (_obj->o.color.flag & 0x80)                                        \
          LOG_TRACE (#o "." #color ".rgb: 0x%06x [ENC.BL %d]\n",                     \
                     (unsigned)_obj->o.color.rgb, dxf2);                      \
        if (_obj->o.color.flag & 0x40 && _obj->o.color.handle)                \
          LOG_TRACE (#o "." #color ".handle: %lX [ENC.H %d]\n",               \
                     _obj->o.color.handle->handleref.value, dxf2 + 10);       \
      }                                                                       \
  }


#undef DEBUG_POS
#undef DEBUG_HERE
#undef DEBUG_POS_OBJ
#undef DEBUG_HERE_OBJ
#define DEBUG_POS_OBJ                                                         \
  if (DWG_LOGLEVEL >= DWG_LOGLEVEL_TRACE)                                     \
    {                                                                         \
      LOG_TRACE ("DEBUG_POS @%u.%u (%lu) %lu\n", (unsigned int)dat->byte,     \
                 dat->bit, bit_position (dat),                                \
                 obj ? bit_position (dat) - obj->address * 8 : 0);            \
    }
#define DEBUG_POS                                                             \
  if (DWG_LOGLEVEL >= DWG_LOGLEVEL_TRACE)                                     \
    {                                                                         \
      LOG_TRACE ("DEBUG_POS @%u.%u (%lu)\n", (unsigned int)dat->byte,         \
                 dat->bit, bit_position (dat));                               \
    }
#define _DEBUG_HERE(objsize)                                                  \
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
      if (objsize && dat->byte + 24 > objsize)                                \
        rs = objsize - dat->byte;                                             \
      tmp = bit_read_TF (dat, rs);                                            \
      if (DWG_LOGLEVEL >= DWG_LOGLEVEL_INSANE)                                \
        {                                                                     \
          bit_fprint_bits (OUTPUT, tmp, rs*8);                                \
          HANDLER (OUTPUT, "\n");                                             \
        }                                                                     \
      LOG_TRACE_TF (tmp, rs);                                                 \
      free (tmp);                                                             \
      SINCE (R_13)                                                            \
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
      SINCE (R_13)                                                            \
      {                                                                       \
        *dat = here;                                                          \
        rs = bit_read_BS (dat);                                               \
        LOG_TRACE ("  BS :" FORMAT_BS " / 0x%04x (%ld)\t", rs, rs,            \
                   bit_position (dat) - bit_position (&here));                \
      }                                                                       \
      SINCE (R_2007)                                                          \
      {                                                                       \
        *dat = here;                                                          \
        rs = bit_read_MS (dat);                                               \
        LOG_TRACE ("  MS :" FORMAT_RS " / 0x%04x (%ld)\n", rs, rs,            \
                   bit_position (dat) - bit_position (&here));                \
      }                                                                       \
      else LOG_TRACE ("\n");                                                  \
      *dat = here;                                                            \
      rl = bit_read_RL (dat);                                                 \
      LOG_TRACE ("  RL :" FORMAT_RL " / 0x%08x (32)\n", rl, rl);              \
      *dat = here;                                                            \
      LOG_TRACE ("  RD :" FORMAT_RD " (64)\n", bit_read_RD (dat));            \
      *dat = here;                                                            \
      SINCE (R_13)                                                            \
      {                                                                       \
        if (bb != 3)                                                          \
          {                                                                   \
            rl = bit_read_BL (dat);                                           \
            LOG_TRACE ("  BL :" FORMAT_BL " / 0x%08x (%ld)\n", rl, rl,        \
                       bit_position (dat) - bit_position (&here));            \
            *dat = here;                                                      \
            bd = bit_read_BD (dat);                                           \
            LOG_TRACE ("  BD :" FORMAT_BD " (%ld)\n", bd,                     \
                       bit_position (dat) - bit_position (&here));            \
            *dat = here;                                                      \
          }                                                                   \
      }                                                                       \
      if ((dat->chain[dat->byte] & 0xf) <= 4)                                 \
        {                                                                     \
          loglevel = 0;                                                       \
          if (!bit_read_H (dat, &hdl))                                        \
            {                                                                 \
              LOG_TRACE ("  H : " FORMAT_H " (%ld)\n", ARGS_H (hdl),          \
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
  _DEBUG_HERE (0)

// check for overflow into next object (invalid num_elems)
#define AVAIL_BITS(dat)                                                       \
  (obj ? (long long)((obj->size * 8) - bit_position (dat) + 20) : 0xff00LL)
#define TYPE_MAXELEMSIZE(type) dwg_bits_size[BITS_##type]
#define VECTOR_CHKCOUNT(nam, type, size, dat)                                 \
  if ((long long)(size) > AVAIL_BITS (dat) ||                                 \
      (long long)((size)*TYPE_MAXELEMSIZE (type)) > AVAIL_BITS (dat))         \
    {                                                                         \
      LOG_ERROR ("Invalid " #nam " size %ld. Need min. %u bits for " #type    \
                 ", have %lld for %s.",                                       \
                 (long)(size), (unsigned)(size)*TYPE_MAXELEMSIZE (type),      \
                 AVAIL_BITS (dat), SAFEDXFNAME);                              \
      if (_obj->nam) free (_obj->nam);                                        \
      return DWG_ERR_VALUEOUTOFBOUNDS;                                        \
    }
#define VECTOR_CHKCOUNT_LV(nam, type, size, dat)                              \
  if ((long long)(size) > AVAIL_BITS (dat) ||                                 \
      (long long)((size)*TYPE_MAXELEMSIZE (type)) > AVAIL_BITS (dat))         \
    {                                                                         \
      LOG_ERROR ("Invalid " #nam " size %ld. Need min. %u bits for " #type    \
                 ", have %lld for %s. Set " #size " to 0.",                   \
                 (long)(size), (unsigned)(size)*TYPE_MAXELEMSIZE (type),      \
                 AVAIL_BITS (dat), SAFEDXFNAME);                              \
      if (_obj->nam) free (_obj->nam);                                        \
      size = 0;                                                               \
      /* return DWG_ERR_VALUEOUTOFBOUNDS; */                                  \
    }
#define _VECTOR_CHKCOUNT(nam, size, maxelemsize, dat)                         \
  if ((long long)(size) > AVAIL_BITS (dat) ||                                 \
      (long long)((size) * (maxelemsize)) > AVAIL_BITS (dat))                 \
    {                                                                         \
      LOG_ERROR ("Invalid " #nam                                              \
                 " size %ld. Need min. %u bits, have %lld for %s. "           \
                 "Set " #size " to 0",                                        \
                 (long)(size), (unsigned)(size) * (maxelemsize),              \
                 AVAIL_BITS (dat), SAFEDXFNAME);                              \
      size = 0;                                                               \
      return DWG_ERR_VALUEOUTOFBOUNDS;                                        \
    }
#define HANDLE_VECTOR_CHKCOUNT(nam, size)                                     \
  _VECTOR_CHKCOUNT (nam, size, TYPE_MAXELEMSIZE(HANDLE), hdl_dat)

// FIELD_VECTOR_N(name, type, size):
// reads data of the type indicated by 'type' 'size' times and stores
// it all in the vector called 'name'.
#define FIELD_VECTOR_N(name, type, size, dxf)                                 \
  if (size > 0)                                                               \
    {                                                                         \
      VECTOR_CHKCOUNT (name, type, size, dat)                                 \
      _obj->name = (BITCODE_##type *)calloc (size, sizeof (BITCODE_##type));  \
      for (vcount = 0; vcount < (BITCODE_BL)size; vcount++)                   \
        {                                                                     \
          _obj->name[vcount] = bit_read_##type (dat);                         \
          LOG_TRACE (#name "[%ld]: " FORMAT_##type "\n", (long)vcount,        \
                      _obj->name[vcount])                                     \
        }                                                                     \
    }
// inlined, with const size and without malloc
#define FIELD_VECTOR_INL(name, type, size, dxf)                               \
  if (size > 0)                                                               \
    {                                                                         \
      VECTOR_CHKCOUNT (name, type, size, dat)                                 \
      for (vcount = 0; vcount < (BITCODE_BL)size; vcount++)                   \
        {                                                                     \
          _obj->name[vcount] = bit_read_##type (dat);                         \
          LOG_TRACE (#name "[%ld]: " FORMAT_##type "\n", (long)vcount,        \
                      _obj->name[vcount])                                     \
        }                                                                     \
    }
#define FIELD_VECTOR_T(name, type, size, dxf)                                 \
  if (_obj->size > 0)                                                         \
    {                                                                         \
      _VECTOR_CHKCOUNT (name, _obj->size,                                     \
                        dat->version >= R_2007 ? 18 : 2, dat)                 \
      _obj->name = calloc (_obj->size, sizeof (char *));                      \
      for (vcount = 0; vcount < (BITCODE_BL)_obj->size; vcount++)             \
        {                                                                     \
          PRE (R_2007)                                                        \
          {                                                                   \
            _obj->name[vcount] = bit_read_TV (dat);                           \
            LOG_TRACE (#name "[%d]: \"%s\" [%s %d]", (int)vcount,             \
                       _obj->name[vcount], #type, dxf)                        \
            LOG_INSANE (" @%lu.%u", dat->byte, dat->bit)                      \
            LOG_TRACE ("\n")                                                  \
          }                                                                   \
          LATER_VERSIONS                                                      \
          {                                                                   \
            _obj->name[vcount] = (char *)bit_read_##type (dat);               \
            LOG_TRACE_TU_I (#name, vcount, _obj->name[vcount], type, dxf)     \
          }                                                                   \
        }                                                                     \
    }
#define FIELD_VECTOR_N1(name, type, size, dxf)                                \
  if (size > 0)                                                               \
    {                                                                         \
      int _dxf = dxf;                                                         \
      VECTOR_CHKCOUNT (name, type, size, dat)                                 \
      _obj->name = (BITCODE_##type *)calloc (size, sizeof (BITCODE_##type));  \
      for (vcount = 0; vcount < (BITCODE_BL)size; vcount++)                   \
        {                                                                     \
          _obj->name[vcount] = bit_read_##type (dat);                         \
          LOG_INSANE (#name "[%d]: " FORMAT_##type " [" #type " %d]\n",       \
                      (int)vcount, _obj->name[vcount], _dxf++)                \
        }                                                                     \
    }

#define FIELD_VECTOR(name, type, size, dxf)                                   \
  FIELD_VECTOR_N (name, type, _obj->size, dxf)

#define SUB_FIELD_VECTOR_TYPESIZE(o, name, size, typesize, dxf)               \
  if (_obj->o.size > 0)                                                       \
    {                                                                         \
      _obj->o.name = calloc (_obj->o.size, typesize);                         \
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
              LOG_ERROR ("Unkown FIELD_VECTOR_TYPE " #name " typesize %d",    \
                         typesize);                                           \
              break;                                                          \
            }                                                                 \
          LOG_TRACE (#name "[%u]: %d\n", vcount, (int)_obj->o.name[vcount])   \
        }                                                                     \
    }

#define FIELD_2RD_VECTOR(name, size, dxf)                                     \
  VECTOR_CHKCOUNT_LV (name, 2RD, _obj->size, dat)                             \
  if (_obj->size > 0)                                                         \
    {                                                                         \
      _obj->name = (BITCODE_2RD *)calloc (_obj->size, sizeof (BITCODE_2RD));  \
      for (vcount = 0; vcount < (BITCODE_BL)_obj->size; vcount++)             \
        {                                                                     \
          FIELD_2RD (name[vcount], dxf);                                      \
        }                                                                     \
    }                                                                         \
  else                                                                        \
    return DWG_ERR_VALUEOUTOFBOUNDS;

#define FIELD_2DD_VECTOR(name, size, dxf)                                     \
  VECTOR_CHKCOUNT_LV (name, 2DD, _obj->size, dat)                             \
  if (_obj->size > 0)                                                         \
    {                                                                         \
      _obj->name = (BITCODE_2RD *)calloc (_obj->size, sizeof (BITCODE_2RD));  \
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
          LOG_INSANE (" @%lu.%u", dat->byte, dat->bit)                        \
          LOG_TRACE ("\n")                                                    \
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
      for (vcount = 0; vcount < (BITCODE_BL)_obj->size; vcount++)             \
        {                                                                     \
          FIELD_3DPOINT (name[vcount], dxf);                                  \
        }                                                                     \
    }                                                                         \
  else                                                                        \
    return DWG_ERR_VALUEOUTOFBOUNDS;

// shortest handle: 8 bit
#define HANDLE_VECTOR_N(nam, size, code, dxf)                                 \
  if (size > 0)                                                               \
    {                                                                         \
      FIELD_VALUE (nam) = (BITCODE_H *)calloc (size, sizeof (BITCODE_H));     \
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
      obj->tio.object->reactors                                               \
          = calloc (obj->tio.object->num_reactors, sizeof (BITCODE_H));       \
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
      _ent->reactors = calloc (_ent->num_reactors, sizeof (BITCODE_H));       \
      for (vcount = 0; vcount < _ent->num_reactors; vcount++)                 \
        {                                                                     \
          VALUE_HANDLE_N (_ent->reactors[vcount], reactors, vcount, code,     \
                          330);                                               \
        }                                                                     \
    }

#define XDICOBJHANDLE(code)                                                   \
  SINCE (R_2004)                                                              \
  {                                                                           \
    if (!obj->tio.object->xdic_missing_flag)                                  \
      {                                                                       \
        VALUE_HANDLE (obj->tio.object->xdicobjhandle, xdicobjhandle, code,    \
                      360);                                                   \
        if (!obj->tio.object->xdicobjhandle)                                  \
          obj->tio.object->xdic_missing_flag = 1;                             \
      }                                                                       \
  }                                                                           \
  else                                                                        \
  {                                                                           \
    SINCE (R_13)                                                              \
    {                                                                         \
      VALUE_HANDLE (obj->tio.object->xdicobjhandle, xdicobjhandle, code,      \
                    360);                                                     \
    }                                                                         \
  }

#define ENT_XDICOBJHANDLE(code)                                               \
  SINCE (R_2004)                                                              \
  {                                                                           \
    if (!_ent->xdic_missing_flag)                                             \
      {                                                                       \
        VALUE_HANDLE (_ent->xdicobjhandle, xdicobjhandle, code, 360);         \
      }                                                                       \
  }                                                                           \
  else                                                                        \
  {                                                                           \
    SINCE (R_13)                                                              \
    {                                                                         \
      VALUE_HANDLE (_ent->xdicobjhandle, xdicobjhandle, code, 360);           \
    }                                                                         \
  }

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
    {                                                                         \
      vcount = bit_position (dat);                                            \
      if (dat->from_version >= R_2007) vcount++; /* has_strings bit */        \
      if (obj->hdlpos != (unsigned long)vcount)                               \
        {                                                                     \
          LOG_HANDLE (" handle stream: %+ld @%lu.%u %s (@%lu.%u "             \
                      " @%lu.%u)\n",                                          \
                      (long)obj->hdlpos - (long)vcount, dat->byte, dat->bit,  \
                      ((long)obj->hdlpos - (long)vcount) >= 8                 \
                          ? "MISSING"                                         \
                          : ((long)obj->hdlpos < (long)vcount) ? "OVERSHOOT"  \
                                                               : "",          \
                      obj->hdlpos / 8, (unsigned)obj->hdlpos % 8,             \
                      hdl_dat->byte, hdl_dat->bit);                           \
          bit_set_position (dat, obj->hdlpos);                                \
        }                                                                     \
    }

#define REPEAT_CHKCOUNT(name, times, type)                                    \
  if (AVAIL_BITS (dat) < 0)                                                   \
    {                                                                         \
      LOG_ERROR ("Invalid " #name " in %s. No bytes left.\n", SAFEDXFNAME);   \
      if (_obj->name) { free (_obj->name); _obj->name = NULL; }               \
      return DWG_ERR_VALUEOUTOFBOUNDS;                                        \
    }                                                                         \
  LOG_INSANE ("REPEAT_CHKCOUNT %s." #name " x %ld: %lld > %lld?\n",           \
              SAFEDXFNAME, (long)times, (long long)((times) * sizeof (type)), \
              AVAIL_BITS (dat));                                              \
  if ((long long)((times) * sizeof (type)) > AVAIL_BITS (dat))                \
    {                                                                         \
      LOG_ERROR ("Invalid %s." #name " x %ld\n", SAFEDXFNAME, (long)times);   \
      if (_obj->name) { free (_obj->name); _obj->name = NULL; }               \
      return DWG_ERR_VALUEOUTOFBOUNDS;                                        \
    }
#define REPEAT_CHKCOUNT_LVAL(name, times, type)                               \
  if (AVAIL_BITS (dat) < 0)                                                   \
    {                                                                         \
      LOG_ERROR ("Invalid %s." #name ". No bytes left.\n", SAFEDXFNAME);      \
      times = 0;                                                              \
      if (_obj->name) { free (_obj->name); _obj->name = NULL; }               \
      return DWG_ERR_VALUEOUTOFBOUNDS;                                        \
    }                                                                         \
  LOG_INSANE ("REPEAT_CHKCOUNT_LVAL %s." #name " x %ld: %lld > %lld?\n",      \
              SAFEDXFNAME, (long)times, (long long)((times) * sizeof (type)), \
              AVAIL_BITS (dat));                                              \
  if ((long long)((times) * sizeof (type)) > AVAIL_BITS (dat))                \
    {                                                                         \
      LOG_ERROR ("Invalid %s." #name " x %ld\n", SAFEDXFNAME, (long)times);   \
      times = 0;                                                              \
      if (_obj->name) { free (_obj->name); _obj->name = NULL; }               \
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
    _obj->name = (type *)calloc (times, sizeof (type));                       \
  for (rcount1 = 0; rcount1 < (BITCODE_BL)times; rcount1++)
// checked with constant times
#define REPEAT_N(times, name, type)                                           \
  REPEAT_CHKCOUNT (name, times, type)                                         \
  if (times > 0)                                                              \
    _obj->name = (type *)calloc (times, sizeof (type));                       \
  for (rcount1 = 0; rcount1 < (BITCODE_BL)times; rcount1++)

// checked with var. times
#define _REPEAT(times, name, type, idx)                                       \
  REPEAT_CHKCOUNT_LVAL (name, _obj->times, type)                              \
  if (_obj->times > 0)                                                        \
    _obj->name = (type *)calloc (_obj->times, sizeof (type));                 \
  for (rcount##idx = 0; rcount##idx < (BITCODE_BL)_obj->times; rcount##idx++)
// unchecked with var. times
#define _REPEAT_C(times, name, type, idx)                                     \
  if (_obj->times > 0)                                                        \
    _obj->name = (type *)calloc (_obj->times, sizeof (type));                 \
  for (rcount##idx = 0; rcount##idx < (BITCODE_BL)_obj->times; rcount##idx++)
// unchecked with constant times
#define _REPEAT_CN(times, name, type, idx)                                    \
  if (times > 0)                                                              \
    _obj->name = (type *)calloc (times, sizeof (type));                       \
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
  SINCE (R_13)                                                                \
  {                                                                           \
    START_HANDLE_STREAM;                                                      \
  }

/** Add the empty entity or object with its three structs to the DWG.
    All fields are zero'd. TODO: some are initialized with default values, as
    defined in dwg.spec.
    Returns 0 or DWG_ERR_OUTOFMEM.
*/

#define DWG_ENTITY(token)                                                     \
  EXPORT int dwg_add_##token (Dwg_Object *obj)                                \
  {                                                                           \
    Dwg_Object_Entity *_ent;                                                  \
    Dwg_Entity_##token *_obj;                                                 \
    LOG_INFO ("Add entity " #token " [%d] ", obj->index)                      \
    obj->parent->num_entities++;                                              \
    obj->supertype = DWG_SUPERTYPE_ENTITY;                                    \
    if (!(int)obj->fixedtype)                                                 \
      {                                                                       \
        obj->fixedtype = DWG_TYPE_##token;                                    \
      }                                                                       \
    if (!obj->dxfname)                                                        \
      {                                                                       \
        if (strlen (#token) > 3 && !memcmp (#token, "_3D", 3))                \
          obj->name = obj->dxfname = (char *)&#token[1];                      \
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
    _ent = obj->tio.entity = calloc (1, sizeof (Dwg_Object_Entity));          \
    if (!_ent)                                                                \
      return DWG_ERR_OUTOFMEM;                                                \
    _ent->tio.token = calloc (1, sizeof (Dwg_Entity_##token));                \
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
  /**Call dwg_add_##token and write the fields from the bitstream dat to the  \
   * entity or object. */                                                     \
  static int dwg_decode_##token (Bit_Chain *restrict dat,                     \
                                 Dwg_Object *restrict obj)                    \
  {                                                                           \
    int error = dwg_add_##token (obj);                                        \
    Bit_Chain hdl_dat = *dat;                                                 \
    if (error)                                                                \
      return error;                                                           \
    SINCE (R_2007)                                                            \
      {                                                                       \
        Bit_Chain obj_dat = *dat, str_dat = *dat;                             \
        error = dwg_decode_##token##_private (&obj_dat, &hdl_dat, &str_dat,   \
                                              obj);                           \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        error = dwg_decode_##token##_private (dat, &hdl_dat, dat, obj);       \
      }                                                                       \
    return error;                                                             \
  }                                                                           \
                                                                              \
  GCC30_DIAG_IGNORE (-Wformat-nonliteral)                                     \
  static int dwg_decode_##token##_private (                                   \
      Bit_Chain *dat, Bit_Chain *hdl_dat, Bit_Chain *str_dat,\
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
    SINCE (R_13) { error = dwg_decode_entity (dat, hdl_dat, str_dat, _ent); } \
    else { error = decode_entity_preR13 (dat, obj, _ent); }                   \
    if (error >= DWG_ERR_CRITICAL)                                            \
      return error;

// Does size include the CRC?
#define DWG_ENTITY_END                                                        \
    {                                                                         \
      unsigned long pos = obj_stream_position (dat, hdl_dat, str_dat);        \
      int64_t padding = (obj->size * 8) - pos;                                \
      bit_set_position (dat, pos);                                            \
      if (padding)                                                            \
        LOG_HANDLE (" padding: %+ld %s\n", (long)padding,                     \
                    padding >= 8 ? "MISSING"                                  \
                               : (padding < 0) ? "OVERSHOOT" : "");           \
    }                                                                         \
    return error & ~DWG_ERR_UNHANDLEDCLASS;                                   \
  }

#define DWG_OBJECT(token)                                                     \
  EXPORT int dwg_add_##token (Dwg_Object *obj)                                \
  {                                                                           \
    Dwg_Object_##token *_obj;                                                 \
    LOG_INFO ("Add object " #token " [%d] ", obj->index)                      \
    obj->supertype = DWG_SUPERTYPE_OBJECT;                                    \
    obj->tio.object = calloc (1, sizeof (Dwg_Object_Object));                 \
    if (!obj->tio.object)                                                     \
      return DWG_ERR_OUTOFMEM;                                                \
    _obj = obj->tio.object->tio.token                                         \
        = calloc (1, sizeof (Dwg_Object_##token));                            \
    if (!_obj)                                                                \
      {                                                                       \
        free (obj->tio.object);                                               \
        obj->tio.object = NULL;                                               \
        obj->fixedtype = DWG_TYPE_FREED;                                      \
        return DWG_ERR_OUTOFMEM;                                              \
      }                                                                       \
    if (!(int)obj->fixedtype)                                                 \
      {                                                                       \
        obj->fixedtype = DWG_TYPE_##token;                                    \
        obj->name = (char *)#token;                                           \
      }                                                                       \
    if (!obj->dxfname)                                                        \
      {                                                                       \
        if (!strcmp (#token, "PLACEHOLDER"))                                  \
          obj->dxfname = (char *)"ACDBPLACEHOLDER";                           \
        else                                                                  \
          obj->dxfname = (char *)#token;                                      \
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
    int error = dwg_add_##token (obj);                                        \
    Bit_Chain hdl_dat = *dat;                                                 \
    if (error)                                                                \
      return error;                                                           \
    SINCE (R_2007)                                                            \
      {                                                                       \
        Bit_Chain obj_dat = *dat, str_dat = *dat;                             \
        error = dwg_decode_##token##_private (&obj_dat, &hdl_dat, &str_dat,   \
                                              obj);                           \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        error = dwg_decode_##token##_private (dat, &hdl_dat, dat, obj);       \
      }                                                                       \
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
    LOG_INFO ("Decode object " #token "\n")                                 \
    if (strNE (#token, "TABLECONTENT") || obj->fixedtype != DWG_TYPE_TABLE)   \
      {                                                                       \
        _obj = obj->tio.object->tio.token;                                    \
        error = dwg_decode_object (dat, hdl_dat, str_dat, obj->tio.object);   \
        if (error >= DWG_ERR_CRITICAL)                                        \
          return error;                                                       \
      }

#define DWG_OBJECT_END DWG_ENTITY_END

#endif
