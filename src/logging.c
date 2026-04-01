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
 * logging.c: logging functions
 * written by Reini Urban
 */

/* Reduce logging code size by using functions instead of inline macros. */

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "common.h"
#include "bits.h"
#include "logging.h"

/* The central logging level for all modules. */
// #if !defined COMMON_TEST_C && !defined DECODE_TEST_C && !defined
// ENCODE_TEST_C
EXPORT unsigned int loglevel;
// #endif

void ATTRIBUTE_FORMAT (2, 3)
    logger (const unsigned int minlevel, const char *restrict fmt, ...)
{
  if (loglevel >= minlevel)
    {
      va_list args;
      va_start (args, fmt);
      vfprintf (OUTPUT, fmt, args);
      va_end (args);
    }
}

void ATTRIBUTE_FORMAT (1, 2) log_error (const char *restrict fmt, ...)
{
  if (loglevel >= DWG_LOGLEVEL_ERROR)
    {
      va_list args;
      fputs ("ERROR: ", OUTPUT);
      va_start (args, fmt);
      vfprintf (OUTPUT, fmt, args);
      va_end (args);
      fputc ('\n', OUTPUT);
    }
}

void ATTRIBUTE_FORMAT (1, 2) log_warn (const char *restrict fmt, ...)
{
  if (loglevel >= DWG_LOGLEVEL_ERROR)
    {
      va_list args;
      fputs ("Warning: ", OUTPUT);
      va_start (args, fmt);
      vfprintf (OUTPUT, fmt, args);
      va_end (args);
      fputc ('\n', OUTPUT);
    }
}

void
log_text32 (const unsigned int minlevel, const BITCODE_TU wstr)
{
  if (loglevel >= minlevel && wstr)
    {
      char *_u8 = bit_convert_TU (wstr);
      HANDLER (OUTPUT, "%s", _u8);
      free (_u8);
    }
}
