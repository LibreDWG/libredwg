/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2010-2019 Free Software Foundation, Inc.                   */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * logging.h: logging macros
 * written by Rodrigo Rodrigues da Silva
 * modified by Reini Urban
 */

// Reduce logging code through macros. In the future, this file can be used as
// an interface to use more sophisticated logging libraries such as gnu nana

#ifndef LOGGING_H
#define LOGGING_H

#include <stdio.h>
#include <string.h>

#ifdef IN_DXF_H
#  error in_dxf.h must be included after logging.h because of FORMAT_BD
#endif

/*
 * If more logging levels are necessary, put them in the right place and
 * update the numbering, keeping it a 0,1,...n sequence, where n corresponds
 * to LOGLEVEL_ALL. If LOGLEVEL is set to k, all messages with LOGLEVEL < k
 * will be displayed
 */

#define DWG_LOGLEVEL_NONE 0   // no log
#define DWG_LOGLEVEL_ERROR 1  // only error messages
#define DWG_LOGLEVEL_INFO 2   // only general info and object codes/names
#define DWG_LOGLEVEL_TRACE 3  // eg for each field value parsed
#define DWG_LOGLEVEL_HANDLE 4 // print all referenced objects (handles)
#define DWG_LOGLEVEL_INSANE 5 // print all vector data (string content)
// #define LOGLEVEL_FOO ..     //if more codes are necessary
#define DWG_LOGLEVEL_ALL 9

#ifndef DWG_LOGLEVEL
#  define DWG_LOGLEVEL DWG_LOGLEVEL_ERROR
#endif

#define HANDLER fprintf
#define OUTPUT stderr

#define LOG(level, ...)                                                   \
  {                                                                           \
    if (DWG_LOGLEVEL >= DWG_LOGLEVEL_##level)                                 \
      {                                                                       \
        HANDLER (OUTPUT, ##__VA_ARGS__);                                               \
      }                                                                       \
  }
#define LOG_ERROR(...)                                                    \
  {                                                                           \
    if (DWG_LOGLEVEL >= DWG_LOGLEVEL_ERROR)                                   \
      {                                                                       \
        HANDLER (OUTPUT, "ERROR: ");                                          \
        LOG (ERROR, ##__VA_ARGS__)                                                     \
        HANDLER (OUTPUT, "\n");                                               \
      }                                                                       \
  }
#define LOG_WARN(...)                                                     \
  {                                                                           \
    if (DWG_LOGLEVEL >= DWG_LOGLEVEL_ERROR)                                   \
      {                                                                       \
        HANDLER (OUTPUT, "Warning: ");                                        \
        LOG (ERROR, ##__VA_ARGS__)                                                     \
        HANDLER (OUTPUT, "\n");                                               \
      }                                                                       \
  }

// speed up fuzzing, avoid unnecessary branches
#ifdef __AFL_COMPILER
#  undef DWG_LOGLEVEL
#  define DWG_LOGLEVEL DWG_LOGLEVEL_NONE
#  undef LOG
#  undef LOG_WARN
#  undef LOG_ERROR
#  define LOG(args...)                                                        \
    {                                                                         \
    }
#  define LOG_WARN(args...)                                                   \
    {                                                                         \
    }
#  define LOG_ERROR(args...)                                                  \
    {                                                                         \
    }
#endif

#define LOG_INFO(...) LOG (INFO, ##__VA_ARGS__)
#define LOG_TRACE(...) LOG (TRACE, ##__VA_ARGS__)
#define LOG_HANDLE(...) LOG (HANDLE, ##__VA_ARGS__)
#define LOG_INSANE(...) LOG (INSANE, ##__VA_ARGS__)
#define LOG_ALL(...) LOG (ALL, ##__VA_ARGS__)

#ifndef LOG_POS
#  define LOG_POS                                                             \
    LOG_INSANE (" @%lu.%u", dat->byte, dat->bit)                              \
    LOG_TRACE ("\n")
#endif
#ifdef HAVE_NATIVE_WCHAR2
#  define LOG_TRACE_TU(s, wstr, dxf)                                          \
    LOG_TRACE ("%s: \"%ls\" [TU %d]", s, (wchar_t *)wstr, dxf)                \
    LOG_POS
#  define LOG_TRACE_TU_I(s, i, wstr, type, dxf)                               \
    LOG_TRACE ("%s[%d]: \"%ls\" [%s %d]", s, (int)i, (wchar_t *)wstr, #type,  \
               dxf)                                                           \
    LOG_POS
#  define LOG_TEXT_UNICODE(level, ...) LOG (level, ##__VA_ARGS__)
#else
#  define LOG_TRACE_TU(s, wstr, dxf)                                          \
    {                                                                         \
      LOG_TRACE ("%s: \"", s)                                                 \
      LOG_TEXT_UNICODE (TRACE, (BITCODE_TU)wstr)                              \
      LOG_TRACE ("\" [TU %d]", dxf)                                           \
      LOG_POS;                                                                \
    }
#  define LOG_TRACE_TU_I(s, i, wstr, type, dxf)                               \
    {                                                                         \
      LOG_TRACE ("%s[%d]: \"", s, (int)i)                                     \
      LOG_TEXT_UNICODE (TRACE, (BITCODE_TU)wstr)                              \
      LOG_TRACE ("\" [" #type " %d]", dxf)                                    \
      LOG_POS;                                                                \
    }
#  define LOG_TEXT_UNICODE(level, wstr)                                       \
    {                                                                         \
      if (DWG_LOGLEVEL >= DWG_LOGLEVEL_##level && wstr)                       \
        {                                                                     \
          char *_u8 = bit_convert_TU (wstr);                                  \
          HANDLER (OUTPUT, "%s", _u8);                                        \
          free (_u8);                                                         \
        }                                                                     \
    }
#endif
#define LOG_TRACE_TW(s, wstr, dxf)                                            \
  LOG_TRACE ("%s: \"", s)                                                     \
  LOG_TEXT32 (TRACE, (BITCODE_TW)wstr)                                        \
  LOG_TRACE ("\" [TW %d]", dxf)                                               \
  LOG_POS
#define LOG_TEXT32(level, wstr)                                               \
  {                                                                           \
    if (DWG_LOGLEVEL >= DWG_LOGLEVEL_##level && wstr)                         \
      {                                                                       \
        char *_u8 = bit_convert_TU (wstr);                                    \
        HANDLER (OUTPUT, "%s", _u8);                                          \
        free (_u8);                                                           \
      }                                                                       \
  }

#endif
