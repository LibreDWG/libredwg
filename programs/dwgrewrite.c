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
 * dwgrewrite.c: load a DWG file and rewrite it,
 * optionally as a different version.
 *
 * written by Anderson Pierre Cardoso
 * modified by Reini Urban
 */

#include "../src/config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <getopt.h>

#include <dwg.h>
#include "../src/common.h"
#include "suffix.inc"

static int opts = 1;

static int help (void);
// int verbosity(int argc, char **argv, int i, unsigned int *opts);
//#include "common.inc"

static int
usage (void)
{
  printf ("\nUsage: dwgrewrite [-v[N]] [--as rNNNN] <dwg_input_file.dwg> "
          "[<dwg_output_file.dwg>]\n");
  return 1;
}
static int
opt_version (void)
{
  printf ("dwgrewrite %s\n", PACKAGE_VERSION);
  return 0;
}
static int
help (void)
{
  printf ("\nUsage: dwgrewrite [OPTION]... INFILE [OUTFILE]\n");
  printf ("Rewrites the DWG as another DWG.\n");
  printf ("Default OUTFILE: INFILE with <-rewrite.dwg> appended.\n"
          "\n");
#ifdef HAVE_GETOPT_LONG
  printf ("  -v[0-9], --verbose [0-9]  verbosity\n");
  printf ("  --as rNNNN                save as version\n");
  printf ("           Valid versions:\n");
  printf ("             r12, r14, r2000\n");
  printf ("           Planned versions:\n");
  printf ("             r9, r10, r11, r2004, r2007, r2010, r2013, r2018\n");
  printf ("  -o dwgfile, --file        \n");
  printf ("           --help           display this help and exit\n");
  printf ("           --version        output version information and exit\n"
          "\n");
#else
  printf ("  -v[0-9]     verbosity\n");
  printf ("  -a rNNNN    save as version\n");
  printf ("              Valid versions:\n");
  printf ("                r12, r14, r2000 (default)\n");
  printf ("              Planned versions:\n");
  printf ("                r9, r10, r11, r2004, r2007, r2010, r2013, r2018\n");
  printf ("  -o dwgfile\n");
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
  int error;
  int i = 1;
  Dwg_Data dwg;
  char *filename_in;
  const char *version = NULL;
  char *filename_out = NULL;
  Dwg_Version_Type dwg_version;
  BITCODE_BL num_objects;
  int c;
#ifdef HAVE_GETOPT_LONG
  int option_index = 0;
  static struct option long_options[]
      = { { "verbose", 1, &opts, 1 }, // optional
          { "file", 1, 0, 'o' },      { "as", 1, 0, 'a' }, { "help", 0, 0, 0 },
          { "version", 0, 0, 0 },     { NULL, 0, NULL, 0 } };
#endif

  // check args
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

  memset (&dwg, 0, sizeof (Dwg_Data));
  dwg.opts = opts;

  filename_in = argv[i];
  if (!filename_in)
    {
      puts ("No input file specified");
      return 1;
    }
  if (!filename_out)
    {
      if (argc > i + 1)
        filename_out = argv[i + 1];
      else
        filename_out = suffix (filename_in, "-rewrite.dwg");
    }
  if (!filename_out || !strcmp (filename_in, filename_out))
    {
      if (filename_out != argv[i + 1])
        free (filename_out);
      return usage ();
    }

  /*
   * some very simple testing
   */
  printf ("Reading DWG file %s\n", filename_in);
  error = dwg_read_file (filename_in, &dwg);
  if (error >= DWG_ERR_CRITICAL)
    fprintf (stderr, "READ ERROR 0x%x\n", error);
  num_objects = dwg.num_objects;
  if (!num_objects)
    {
      printf ("Read 0 objects\n");
      if (error >= DWG_ERR_CRITICAL)
        {
          if (filename_out != argv[i + 1])
            free (filename_out);
          return error;
        }
    }

  if (opts)
    printf ("\n");
  printf ("Writing DWG file %s", filename_out);
#ifndef USE_WRITE
  error = 1;
#else
  if (version)
    { // forced -as-rXXX
      printf (" as %s\n", version);
      if (dwg.header.from_version != dwg.header.version)
        dwg.header.from_version = dwg.header.version;
      // else keep from_version
      dwg.header.version = dwg_version;
    }
  else if (dwg.header.version < R_13 || dwg.header.version > R_2000)
    {
      // we cannot yet write pre-r13 or 2004+
      printf (" as r2000\n");
      dwg.header.version = R_2000;
    }
  else
    {
      printf ("\n");
    }

  {
    struct stat attrib;
    if (!stat (filename_out, &attrib)) // exists
      {
        if (S_ISREG (attrib.st_mode) &&        // refuse to remove a directory
            (access (filename_out, W_OK) == 0) // is writable
#ifndef _WIN32
            // refuse to remove a symlink. even with overwrite. security
            && !S_ISLNK (attrib.st_mode)
#endif
            )
          unlink (filename_out);
        else
          {
            fprintf (stderr, "Not writable file or symlink: %s\n",
                     filename_out);
            error |= DWG_ERR_IOERROR;
          }
      }
  }
  error = dwg_write_file (filename_out, &dwg);
#endif

  if (error >= DWG_ERR_CRITICAL)
    {
      printf ("WRITE ERROR 0x%x\n", error);
      if (filename_out != argv[i + 1])
        free (filename_out);
      return error;
    }
  dwg_free (&dwg); // this is slow, but on needed on low memory systems

#ifdef USE_WRITE
  // try to read again
  if (opts)
    printf ("\n");
  printf ("Re-reading created file %s\n", filename_out);
  error = dwg_read_file (filename_out, &dwg);
  if (error >= DWG_ERR_CRITICAL)
    printf ("re-READ ERROR 0x%x\n", error);
  if (num_objects && (num_objects != dwg.num_objects))
    printf ("re-READ num_objects: %lu, should be %lu\n",
            (unsigned long)dwg.num_objects, (unsigned long)num_objects);
  dwg_free (&dwg);
#endif
  if (filename_out != argv[i + 1])
    free (filename_out);
  return error >= DWG_ERR_CRITICAL ? error : 0;
}
