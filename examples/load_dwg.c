/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2009, 2021, 2023 Free Software Foundation, Inc.            */
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
#include <time.h>
#ifndef DISABLE_WRITE
#  include <sys/stat.h>
#  include <unistd.h>
#endif
#include "dwg.h"
#include "dwg_api.h"
#include "bits.h"
#include "common.h"

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

static int
add_fingerprint (Dwg_Data *dwg, dwg_point_3d *pt)
{
  int error = 0;
  char text[128];
  double height = dwg->header_vars.TEXTSIZE;
  time_t t = time (NULL);
  Dwg_Object_BLOCK_HEADER *hdr = dwg_get_block_header (dwg, &error);
  Dwg_Entity_TEXT *ent;
  if (error)
    return 1;

#ifdef HAVE_WFORMAT_Y2K
  GCC46_DIAG_IGNORE (-Wformat-y2k)
#endif
  strftime (text, sizeof (text), "Last updated: %c", localtime (&t));
#ifdef HAVE_WFORMAT_Y2K
  GCC46_DIAG_RESTORE
#endif

#ifdef USE_WRITE
  if ((ent = dwg_add_TEXT (hdr, text, pt, height)))
    {
      ent->horiz_alignment = HORIZ_ALIGNMENT_RIGHT;
      return 0;
    }
  else
#endif
    return 1;
}

static int
change_fingerprint (Dwg_Data *dwg, Dwg_Entity_TEXT *_obj)
{
  char text[128];
  double height = dwg->header_vars.TEXTSIZE;
  time_t t = time (NULL);

#ifdef HAVE_WFORMAT_Y2K
  GCC46_DIAG_IGNORE (-Wformat-y2k)
#endif
  strftime (text, sizeof (text), "Last updated: %c", localtime (&t));
#ifdef HAVE_WFORMAT_Y2K
  GCC46_DIAG_RESTORE
#endif

  if (dwg->header.version < R_2007)
    {
      if (strlen (text) < strlen (_obj->text_value))
        strcpy (_obj->text_value, text);
      else
        {
          FREE (_obj->text_value);
          _obj->text_value = STRDUP (text);
        }
    }
  else
    {
      FREE (_obj->text_value);
      _obj->text_value = (BITCODE_TV)bit_utf8_to_TU (text, 0);
    }
  return 0;
}

static int
load_dwg (char *filename, unsigned int opts)
{
  BITCODE_BL i;
  int success, found = 0;
  Dwg_Data dwg;
  dwg_point_3d pt;

#ifdef USE_WRITE
  char *new_filename = (char *)MALLOC (strlen (filename) + 4);
  char *fn = STRDUP (filename);
  char *base = basename (fn);
  char *p;
  struct stat st;

  if ((p = strrchr (base, '.')))
    *p = '\0';
  sprintf (new_filename, "%s_new.dwg", base);
  FREE (fn);
#endif

  memset (&dwg, 0, sizeof (Dwg_Data));
  dwg.opts = opts;
  success = dwg_read_file (filename, &dwg);
  // get the insertion point for our fingerprint
  pt.x = dwg.header_vars.LIMMAX.x;
  pt.y = dwg.header_vars.LIMMAX.y;
  pt.z = 0.0;

  // check if a fingerprint already exists there.
  // if so update it. if not add it
  for (i = 0; i < dwg.num_objects; i++)
    {
      if (dwg.object[i].fixedtype == DWG_TYPE_TEXT)
        {
          Dwg_Entity_TEXT *_obj = dwg.object[i].tio.entity->tio.TEXT;
          if (pt.x == _obj->ins_pt.x && pt.y == _obj->ins_pt.y
              && _obj->horiz_alignment == HORIZ_ALIGNMENT_RIGHT)
            {
              found++;
              change_fingerprint (&dwg, _obj);
              fprintf (stderr, "fingerprint updated at (%f, %f)\n", pt.x,
                       pt.y);
            }
        }
    }

  if (!found)
    {
      add_fingerprint (&dwg, &pt);
      fprintf (stderr, "fingerprint added at (%f, %f)\n", pt.x, pt.y);
    }

#ifdef USE_WRITE
  if (0 == stat (new_filename, &st))
    unlink (new_filename);
  if (dwg.header.version > R_2000)
    dwg.header.version = R_2000;
  success = dwg_write_file (new_filename, &dwg);
  FREE (new_filename);
#endif

  dwg_free (&dwg);
  return success;
}

int
main (int argc, char *argv[])
{
  int i = 1;
  unsigned int opts = 0;

  GC_INIT ();
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
