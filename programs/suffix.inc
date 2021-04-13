/* -*- c -*- */
/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2010 Thien-Thi Nguyen                                      */
/*  Copyright (C) 2014, 2018 Free Software Foundation, Inc.                  */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * suffix.c: avoid hardcoded input/output filenames
 * written by Thien-Thi Nguyen
 * modified by Reini Urban
 */

#include "config.h"
#ifdef __STDC_ALLOC_LIB__
# define __STDC_WANT_LIB_EXT2__ 1 /* for strdup */
#else
# define _USE_BSD 1
#endif
#ifndef _XOPEN_SOURCE /* for strdup, snprintf */
# define _XOPEN_SOURCE 700
#endif
#include <stdlib.h>
#include <stdio.h>
#ifdef HAVE_LIBGEN_H
#  include <libgen.h>
#else
  char *basename (char *);
#endif
#include <string.h>

char *
suffix (const char *filename, const char *ext);

/* Return a newly `malloc'ed string made from "re-suffixing" FILENAME with
   ".EXT" (note dot).  That is, when FILENAME has the form "STEM.dwg" the
   value is "STEM.EXT", otherwise the value is "FILENAME.EXT".

   Caller should `free' the returned string when done using it.  */
char *
suffix (const char *filename, const char *ext)
{
  char *copy = STRDUP (filename ? filename : "");
#ifdef HAVE_BASENAME
  char *base = basename (copy);
#else
  char *base = copy;
#endif
  size_t len = (base ? strlen (base) : 0) + 1 + strlen (ext) + 1;
  char *rv = (char *)MALLOC (len);
  char *dot;

  if (!base) // basename may fail
    {
      base = (char *)"";
    }
  else
    {
      if ((dot = strrchr (base, '.'))
          && dot + 4 < base + len
          //valid input extensions:
          && (!strncmp (1 + dot, "dwg", 3) || // strip known extensions
              !strncmp (1 + dot, "DWG", 3) ||
              !strncmp (1 + dot, "dxf", 3) || // TODO xml
              !strncmp (1 + dot, "DXF", 3) || // TODO xml
              !strncmp (1 + dot, "json", 4) ||
              !strncmp (1 + dot, "JSON", 4)))
        *dot = '\0';
    }
  if (strchr(ext, '.'))
    snprintf (rv, len, "%s%s", base, ext);
  else
    snprintf (rv, len, "%s.%s", base, ext);
  FREE (copy);
  return rv;
}

#define REQUIRE_INPUT_FILE_ARG(argc)            \
  do                                            \
    {                                           \
      if (1 == argc)                            \
        {                                       \
          puts ("No input file specified");     \
          return 1;                             \
        }                                       \
    }                                           \
  while (0)

/* suffix.c ends here */
