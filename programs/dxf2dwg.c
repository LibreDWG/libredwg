/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2018-2019 Free Software Foundation, Inc.                   */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * dxf2dwg.c: save a DXF as DWG. Detect ascii/binary, minimal/full.
 * Optionally as a different version. WIP
 *
 * written by Reini Urban
 */

#include "../src/config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <getopt.h>
#ifdef HAVE_VALGRIND_VALGRIND_H
#  include <valgrind/valgrind.h>
#endif

#include <dwg.h>
#include "common.h"
#include "bits.h"
#include "logging.h"
#include "suffix.inc"

static int help (void);

static int opts = 1;
int minimal = 0;
int binary = 0;
char buf[4096];
/* the current version per spec block */
static unsigned int cur_ver = 0;

static int
usage (void)
{
  printf ("\nUsage: dxf2dwg [-v[N]] [--as rNNNN] <input_file.dxf> "
          "[<output_file.dwg>]\n");
  return 1;
}
static int
opt_version (void)
{
  printf ("dxf2dwg %s\n", PACKAGE_VERSION);
  return 0;
}
static int
help (void)
{
  printf ("\nUsage: dxf2dwg [OPTION]... DXFFILES ...\n");
  printf ("Converts the DXF to a DWG. Accepts ascii and binary DXF.\n");
  printf ("Default DWGFILE: DXFFILE with .dwg extension in the current "
          "directory.\n"
          "Existing files are silently overwritten.\n"
          "\n");
#ifdef HAVE_GETOPT_LONG
  printf ("  -v[0-9], --verbose [0-9]  verbosity\n");
  printf ("  --as rNNNN                save as version\n");
  printf ("           Valid versions:\n");
  printf ("             r12, r14, r2000 (default)\n");
  printf ("           Planned versions:\n");
  printf ("             r9, r10, r11, r2004, r2007, r2010, r2013, r2018\n");
  printf ("  -o outfile, --file        only valid with one single DXFFILE\n");
  printf ("       --help               display this help and exit\n");
  printf ("       --version            output version information and exit\n"
          "\n");
#else
  printf ("  -v[0-9]     verbosity\n");
  printf ("  -a rNNNN    save as version\n");
  printf ("              Valid versions:\n");
  printf ("                r12, r14, r2000 (default)\n");
  printf ("              Planned versions:\n");
  printf ("                r9, r10, r11, r2004, r2007, r2010, r2013, r2018\n");
  printf ("  -o dwgfile  only valid with one single DXFFILE\n");
  printf ("  -h          display this help and exit\n");
  printf ("  -i          output version information and exit\n"
          "\n");
#endif
  printf ("GNU LibreDWG online manual: "
          "<https://www.gnu.org/software/libredwg/>\n");
  return 0;
}

int
main (int argc, char *argv[])
{
  int i = 1;
  int error = 0;
  Dwg_Data dwg;
  char *filename_in;
  const char *version = NULL;
  char *filename_out = NULL;
  Dwg_Version_Type dwg_version = R_2000;
  int do_free;
  int need_free = 0;
  int c;
#ifdef HAVE_GETOPT_LONG
  int option_index = 0;
  static struct option long_options[]
      = { { "verbose", 1, &opts, 1 }, // optional
          { "file", 1, 0, 'o' },      { "as", 1, 0, 'a' },
          { "minimal", 0, 0, 'm' },   { "binary", 0, 0, 'b' },
          { "help", 0, 0, 0 },        { "version", 0, 0, 0 },
          { NULL, 0, NULL, 0 } };
#endif

  if (argc < 2)
    return usage ();

  while
#ifdef HAVE_GETOPT_LONG
      ((c = getopt_long (argc, argv, ":a:v::o:h", long_options, &option_index))
       != -1)
#else
      ((c = getopt (argc, argv, ":a:v::o:hi")) != -1)
#endif
    {
      if (c == -1)
        break;
      switch (c)
        {
        case ':': // missing arg
          if (optarg && !strcmp (optarg, "v"))
            {
              opts = 1;
              break;
            }
          fprintf (stderr, "%s: option '-%c' requires an argument\n", argv[0],
                   optopt);
          break;
#ifdef HAVE_GETOPT_LONG
        case 0:
          /* This option sets a flag */
          if (!strcmp (long_options[option_index].name, "verbose"))
            {
              if (opts < 0 || opts > 9)
                return usage ();
#  if defined(USE_TRACING) && defined(HAVE_SETENV)
              {
                char v[2];
                *v = opts + '0';
                *(v + 1) = 0;
                setenv ("LIBREDWG_TRACE", v, 1);
              }
#  endif
              break;
            }
          if (!strcmp (long_options[option_index].name, "version"))
            return opt_version ();
          if (!strcmp (long_options[option_index].name, "help"))
            return help ();
          break;
#else
        case 'i':
          return opt_version ();
#endif
        case 'o':
          filename_out = optarg;
          break;
        case 'a':
          dwg_version = dwg_version_as (optarg);
          if (dwg_version == R_INVALID)
            {
              fprintf (stderr, "Invalid version '%s'\n", argv[1]);
              return usage ();
            }
          version = optarg;
          break;
        case 'v': // support -v3 and -v
          i = (optind > 0 && optind < argc) ? optind - 1 : 1;
          if (!memcmp (argv[i], "-v", 2))
            {
              opts = argv[i][2] ? argv[i][2] - '0' : 1;
            }
          if (opts < 0 || opts > 9)
            return usage ();
#if defined(USE_TRACING) && defined(HAVE_SETENV)
          {
            char v[2];
            *v = opts + '0';
            *(v + 1) = 0;
            setenv ("LIBREDWG_TRACE", v, 1);
          }
#endif
          break;
        case 'h':
          return help ();
        case '?':
          fprintf (stderr, "%s: invalid option '-%c' ignored\n", argv[0],
                   optopt);
          break;
        default:
          return usage ();
        }
    }
  i = optind;

  if (filename_out && i + 1 < argc)
    {
      fprintf (stderr, "%s: no -o with multiple input files\n", argv[0]);
      return usage ();
    }
  do_free = i + 1 < argc;

  while (i < argc)
    {
      filename_in = argv[i];
      i++;
      if (!filename_out)
        {
          need_free = 1;
          filename_out = suffix (filename_in, "dwg");
        }

      if (strEQ (filename_in, filename_out))
        {
          if (filename_out != argv[i-1])
            free (filename_out);
          return usage ();
        }

      dwg.opts = opts;
      printf ("Reading DXF file %s\n", filename_in);
      error = dxf_read_file (filename_in, &dwg);
      if (error >= DWG_ERR_CRITICAL)
        {
          fprintf (stderr, "READ ERROR 0x%x %s\n", error, filename_in);
          continue;
        }
      printf ("TODO: fixing up post-DXF not yet done\n");
      // sections, ...

      printf ("Writing DWG file %s", filename_out);
      if (version)
        {
          printf (" as %s\n", version);
          if (dwg.header.from_version != dwg.header.version)
            dwg.header.from_version = dwg.header.version;
          // else keep from_version = 0
          dwg.header.version = dwg_version;
        }
      else
        {
          // FIXME: for now only R_2000
          dwg.header.version = dwg_version;
          printf ("\n");
        }
#ifdef USE_WRITE
      error = dwg_write_file (filename_out, &dwg);
#else
      error = DWG_ERR_IOERROR;
#  error no DWG write support
#endif
      if (error)
        fprintf (stderr, "WRITE ERROR 0x%x %s\n", error, filename_out);

      // forget about valgrind. really huge DWG's need endlessly here.
      if (do_free
#if defined __SANITIZE_ADDRESS__ || __has_feature (address_sanitizer)
          || 1
#endif
#ifdef HAVE_VALGRIND_VALGRIND_H
          || (RUNNING_ON_VALGRIND)
#endif
      )
        {
          dwg_free (&dwg);
          if (need_free)
            free (filename_out);
        }
      filename_out = NULL;
    }

  // but only the result of the last conversion
  return error >= DWG_ERR_CRITICAL ? 1 : 0;
}
