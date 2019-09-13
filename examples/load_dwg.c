/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2009 Free Software Foundation, Inc.                        */
/*  Copyright (C) 2010 Thien-Thi Nguyen                                      */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * load_dwg.c: load a DWG, get lines, text and circles
 * written by Felipe Castro
 * modified by Felipe Corrêa da Silva Sances
 * modified by Thien-Thi Nguyen
 * modified by Reini Urban
 */

#include "config.h"
#ifdef __STDC_ALLOC_LIB__          /* for strdup */
#  define __STDC_WANT_LIB_EXT2__ 1 /* for strdup */
#else
#  define _USE_BSD 1
#endif
#ifndef _XOPEN_SOURCE /* for strdup, snprintf */
#  define _XOPEN_SOURCE 700
#endif
#include <stdio.h>
#include "dwg.h"

#include "../programs/suffix.inc"
static int help (void);
int verbosity (int argc, char **argv, int i, unsigned int *opts);
#include "../programs/common.inc"

static int
usage (void)
{
  printf ("\nUsage: load_dwg [-v[0-9]] DWGFILE\n");
  return 1;
}
static int
opt_version (void)
{
  printf ("load_dwg %s\n", PACKAGE_VERSION);
  return 0;
}
static int
help (void)
{
  printf ("\nUsage: load_dwg [OPTION]... DWGFILE\n");
  printf ("Example to add fingerprint elements to a DWG.\n"
          "\n");
  printf ("  -v[0-9], --verbose [0-9]  verbosity\n");
  printf ("           --help           display this help and exit\n");
  printf ("           --version        output version information and exit\n"
          "\n");
  printf ("GNU LibreDWG online manual: "
          "<https://www.gnu.org/software/libredwg/>\n");
  return 0;
}

static void
add_line (double x1, double y1, double x2, double y2)
{
  // Make something with that
  (void)x1;
  (void)y1;
  (void)x2;
  (void)y2;
}

static void
add_circle (double x, double y, double R)
{
  // Make something with that
  (void)x;
  (void)y;
  (void)R;
}

static void
add_text (double x, double y, char *txt)
{
  // Make something with that
  (void)x;
  (void)y;
  (void)txt;
}

static int
load_dwg (char *filename, unsigned int opts)
{
  BITCODE_BL i;
  int success;
  Dwg_Data dwg;

  memset (&dwg, 0, sizeof (Dwg_Data));
  dwg.opts = opts;
  success = dwg_read_file (filename, &dwg);
  for (i = 0; i < dwg.num_objects; i++)
    {
      Dwg_Entity_LINE *line;
      Dwg_Entity_CIRCLE *circle;
      Dwg_Entity_TEXT *text;

      switch (dwg.object[i].type)
        {
        case DWG_TYPE_LINE:
          line = dwg.object[i].tio.entity->tio.LINE;
          add_line (line->start.x, line->end.x, line->start.y, line->end.y);
          break;
        case DWG_TYPE_CIRCLE:
          circle = dwg.object[i].tio.entity->tio.CIRCLE;
          add_circle (circle->center.x, circle->center.y, circle->radius);
          break;
        case DWG_TYPE_TEXT:
          text = dwg.object[i].tio.entity->tio.TEXT;
          add_text (text->insertion_pt.x, text->insertion_pt.y,
                    text->text_value);
          break;
        default:
          break;
        }
    }
  dwg_free (&dwg);
  return success;
}

int
main (int argc, char *argv[])
{
  int i = 1;
  unsigned int opts = 1;

  if (argc < 2)
    return usage ();
#if defined(USE_TRACING) && defined(HAVE_SETENV)
  setenv ("LIBREDWG_TRACE", "1", 0);
#endif
  if (argc > 2
      && (!strcmp (argv[i], "--verbose") || !strncmp (argv[i], "-v", 2)))
    {
      int num_args = verbosity (argc, argv, i, &opts);
      argc -= num_args;
      i += num_args;
    }
  if (argc > 1 && !strcmp (argv[i], "--help"))
    return help ();
  if (argc > 1 && !strcmp (argv[i], "--version"))
    return opt_version ();

  REQUIRE_INPUT_FILE_ARG (argc);
  load_dwg (argv[i], opts);
  return 0;
}
