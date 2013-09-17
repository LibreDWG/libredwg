/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2010 Thien-Thi Nguyen                                      */
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
 */

#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include <stdlib.h>

/* Return a newly `malloc'ed string made from "re-suffixing" FILENAME with
   ".EXT" (note dot).  That is, when FILENAME has the form "STEM.dwg" the
   value is "STEM.EXT", otherwise the value is "FILENAME.EXT".

   Caller should `free' the returned string when done using it.  */
char *
suffix (const char *filename, const char *ext)
{
  char *copy = strdup (filename);
  char *base = basename (copy);
  int len = strlen (base) + 1 + strlen (ext) + 1;
  char *rv = malloc (len);
  char *dot;

  if ((dot = strrchr (base, '.'))
      && dot + 4 < base + len
      && !strncmp (1 + dot, "dwg", 3))
    *dot = '\0';
  snprintf (rv, len, "%s.%s", base, ext);
  free (copy);
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
