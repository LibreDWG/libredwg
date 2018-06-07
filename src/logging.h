/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2010, 2018 Free Software Foundation, Inc.                  */
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

//Reduce logging code through macros. In the future, this file can be used as
//an interface to use more sophisticated logging libraries such as gnu nana

#ifndef LOGGING_H
#define LOGGING_H

#include <stdio.h>
#include <string.h>

/*
 * If more logging levels are necessary, put them in the right place and
 * update the numbering, keeping it a 0,1,...n sequence, where n corresponds
 * to LOGLEVEL_ALL. If LOGLEVEL is set to k, all messages with LOGLEVEL < k
 * will be displayed
 */

#define DWG_LOGLEVEL_NONE    0 //no log
#define DWG_LOGLEVEL_ERROR   1 //only error messages
#define DWG_LOGLEVEL_INFO    2 //only general info and object codes/names
#define DWG_LOGLEVEL_TRACE   3 //eg for each field value parsed
#define DWG_LOGLEVEL_HANDLE  4 //print all referenced objects (handles)
#define DWG_LOGLEVEL_INSANE  5 //print all vector data (string content)
// #define LOGLEVEL_FOO ..     //if more codes are necessary
#define DWG_LOGLEVEL_ALL     9

#ifndef DWG_LOGLEVEL
# define DWG_LOGLEVEL DWG_LOGLEVEL_ERROR
#endif

#define HANDLER fprintf
#define OUTPUT stderr

#define LOG(level, args...) \
          if (DWG_LOGLEVEL >= DWG_LOGLEVEL_##level) { \
            HANDLER(OUTPUT, args); \
          }

#define LOG_ERROR(args...) \
          if (DWG_LOGLEVEL >= DWG_LOGLEVEL_ERROR) { \
              HANDLER(OUTPUT, "ERROR: "); \
              LOG(ERROR, args) \
              HANDLER(OUTPUT, "\n");          \
          }
#define LOG_WARN(args...) \
          if (DWG_LOGLEVEL >= DWG_LOGLEVEL_ERROR) { \
              HANDLER(OUTPUT, "Warning: "); \
              LOG(ERROR, args) \
              HANDLER(OUTPUT, "\n"); \
          }

#define LOG_INFO(args...) LOG(INFO, args)
#define LOG_TRACE(args...) LOG(TRACE, args)
#define LOG_HANDLE(args...) LOG(HANDLE, args)
#define LOG_INSANE(args...) LOG(INSANE, args)
#define LOG_ALL(args...) LOG(ALL, args)

#ifdef HAVE_NATIVE_WCHAR2
# define LOG_TRACE_TU(s, wstr, dxf) \
   LOG_TRACE("%s: \"%ls\" [TU %d]", s, (wchar_t*)wstr, dxf)
# define LOG_TRACE_TU_I(s, i, wstr, dxf) \
   LOG_TRACE("%s[%d]: \"%ls\" [TU %d]", s, (int)i, (wchar_t*)wstr, dxf)
# define LOG_TEXT_UNICODE(level, args) LOG(level, args)
#else
# define LOG_TRACE_TU(s, wstr, dxf) \
   LOG_TRACE("%s: \"", s) \
   LOG_TEXT_UNICODE(TRACE, (BITCODE_TU)wstr) \
   LOG_TRACE("\" [TU %d]\n", dxf)
# define LOG_TRACE_TU_I(s, i, wstr, dxf) \
   LOG_TRACE("%s[%d]: \"", s, (int)i) \
   LOG_TEXT_UNICODE(TRACE, (BITCODE_TU)wstr) \
   LOG_TRACE("\" [TU %d]\n", dxf)
# define LOG_TEXT_UNICODE(level, wstr) \
  if (DWG_LOGLEVEL >= DWG_LOGLEVEL_##level && wstr) { \
    BITCODE_TU ws = wstr;                             \
    uint16_t _c;                                      \
    while ((_c = *ws++)) {                            \
      HANDLER(OUTPUT, "%c", (char)(_c & 0xff));       \
    }                                                 \
  }
#endif

#endif
