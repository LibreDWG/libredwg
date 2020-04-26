/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2018-2020 Free Software Foundation, Inc.                   */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/* dxfwrite.c: write a DXF file from various input formats.
 * written by Reini Urban
 */

#include "../src/config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// strings.h or string.h
#ifdef AX_STRCASECMP_HEADER
#  include AX_STRCASECMP_HEADER
#endif
#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif
#include "my_stat.h"
#include "my_getopt.h"
#ifdef HAVE_VALGRIND_VALGRIND_H
#  include <valgrind/valgrind.h>
#endif

#include "dwg.h"
#include "common.h"
#include "bits.h"
#include "suffix.inc"
#include "decode.h"
#ifndef DISABLE_JSON
#  include "in_json.h"
#endif
#include "out_dxf.h"

// avoid the slow fork loop, for afl-clang-fast
#ifdef __AFL_COMPILER
static volatile const char *__afl_persistent_sig = "##SIG_AFL_PERSISTENT##";
#endif

static int opts = 1;

static int help (void);

static int
usage (void)
{
  printf ("\nUsage: dxfwrite [-b] [-v[0-9]] [-y] [--as rNNNN] [-I FMT] [-o "
          "DXFFILE] "
          "INFILE\n");
  return 1;
}
static int
opt_version (void)
{
  printf ("dxfwrite %s\n", PACKAGE_VERSION);
  return 0;
}
static int
help (void)
{
  printf ("\nUsage: dxfwrite [OPTION]... [-o DXFFILE] INFILE\n");
  printf ("Writes a DXF file from various input formats.\n"
          "\n");
#ifdef HAVE_GETOPT_LONG
  printf ("  -v[0-9], --verbose [0-9]  verbosity\n");
  printf ("  --as rNNNN                save as version\n");
  printf ("              Valid versions:\n");
  printf ("                r9, r10, r11, r12, r13, r14, r2000, r2004, r2007,"
          "                r2010, r2013, r2018, r2021\n");
#  ifndef DISABLE_JSON
  printf ("  -I fmt,  --format fmt     DWG, DXF, DXFB, JSON\n");
#  else
  printf ("  -I fmt,  --format fmt     DWG, DXF, DXFB\n");
#  endif
  printf ("           Planned input formats: GeoJSON, YAML, XML/OGR, GPX\n");
  printf ("  -o dxffile, --file        \n");
  printf ("  -m, --minimal             only $ACADVER, HANDSEED and "
          "ENTITIES\n");
  printf ("  -b, --binary              create a binary DXF\n");
  printf ("  -y, --overwrite           overwrite existing files\n");
  printf ("           --help           display this help and exit\n");
  printf ("           --version        output version information and exit\n"
          "\n");
#else
  printf ("  -v[0-9]     verbosity\n");
  printf ("  -a rNNNN    save as version\n");
  printf ("              Valid versions:\n");
  printf ("                r9, r10, r11, r12, r13, r14, r2000, r2004, r2007,"
          "                r2010, r2013, r2018, r2021\n");
#  ifndef DISABLE_JSON
  printf ("  -I fmt      fmt: DWG, DXF, DXFB, JSON\n");
#  else
  printf ("  -I fmt      fmt: DWG, DXF, DXFB\n");
#  endif
  printf (
      "              Planned input formats: GeoJSON, YAML, XML/OGR, GPX\n");
  printf ("  -o dxffile\n");
  printf ("  -m          minimal, only $ACADVER, HANDSEED and ENTITIES\n");
  printf ("  -b          create a binary DXF\n");
  printf ("  -y          overwrite existing files\n");
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
  const char *fmt = NULL;
  const char *infile = NULL;
  char *outfile = NULL;
  Bit_Chain dat = { 0 };
  const char *version = NULL;
  Dwg_Version_Type dwg_version = R_INVALID;
  int c;
  int overwrite = 0;
  int binary = 0;
  int minimal = 0;
  int force_free = 0;
  int free_outfile = 0;
#ifdef HAVE_GETOPT_LONG
  int option_index = 0;
  static struct option long_options[]
      = { { "verbose", 1, &opts, 1 }, // optional
          { "format", 1, 0, 'I' },    { "file", 1, 0, 'o' },
          { "as", 1, 0, 'a' },        { "minimal", 0, 0, 'm' },
          { "binary", 0, 0, 'b' },    { "overwrite", 0, 0, 'y' },
          { "version", 0, 0, 0 },     { "force-free", 0, 0, 0 },
          { "help", 0, 0, 0 },        { NULL, 0, NULL, 0 } };
#endif

  if (argc < 2)
    return usage ();

  while
#ifdef HAVE_GETOPT_LONG
      ((c = getopt_long (argc, argv, "ymba:v::I:o:h", long_options,
                         &option_index))
       != -1)
#else
      ((c = getopt (argc, argv, "ymba:v::I:o:hi")) != -1)
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
          if (!strcmp (long_options[option_index].name, "force-free"))
            force_free = 1;
          if (!strcmp (long_options[option_index].name, "binary"))
            binary = 1;
          break;
#else
        case 'i':
          return opt_version ();
#endif
        case 'I':
          fmt = optarg;
          break;
        case 'y':
          overwrite = 1;
          break;
        case 'b':
          binary = 1;
          break;
        case 'm':
          minimal = 1;
          break;
        case 'o':
          outfile = optarg;
          break;
        case 'a': // as
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

#ifdef __AFL_HAVE_MANUAL_CONTROL
  // llvm_mode deferred init
  __AFL_INIT ();
#endif

#ifdef __xxAFL_HAVE_MANUAL_CONTROL
  while (__AFL_LOOP (1000))
    { // llvm_mode persistent mode (currently broken)
#endif

      // get input format from INFILE, not outfile.
      // With stdin, should -I be mandatory, or try to autodetect the format?
      // With a file use the extension.
      if (optind < argc) // have arg
        {
          infile = argv[i];
          if (!fmt)
            {
              if (strstr (infile, ".dwg") || strstr (infile, ".DWG"))
                fmt = (char *)"dwg";
#ifndef DISABLE_DXF
#  ifndef DISABLE_JSON
              else if (strstr (infile, ".json") || strstr (infile, ".JSON"))
                fmt = (char *)"json";
#  endif
              else if (strstr (infile, ".dxfb") || strstr (infile, ".DXFB"))
                fmt = (char *)"dxfb";
              else if (strstr (infile, ".dxf") || strstr (infile, ".DXF"))
                fmt = (char *)"dxf";
              else
#endif
                fprintf (stderr, "Unknown input format for '%s'\n", infile);
            }
        }

      // allow stdin, but require -I|--format then
      memset (&dwg, 0, sizeof (Dwg_Data));
      dwg.opts = opts;
      if (version) // hint the importer
        dwg.header.version = dat.version = dwg_version;

      if (infile)
        {
          struct stat attrib;
          if (stat (infile, &attrib)) // not exists
            {
              fprintf (stderr, "Missing input file '%s'\n", infile);
              exit (1);
            }
          dat.fh = fopen (infile, "r");
          if (!dat.fh)
            {
              fprintf (stderr, "Could not read file '%s'\n", infile);
              exit (1);
            }
          dat.size = attrib.st_size;
        }
      else
        dat.fh = stdin;

      if ((fmt && !strcasecmp (fmt, "dwg"))
          || (infile && !strcasecmp (infile, ".dwg")))
        {
          if (opts > 1)
            fprintf (stderr, "Reading DWG file %s\n",
                     infile ? infile : "from stdin");
          error = dwg_read_file (infile ? infile : "-", &dwg);
        }
#ifndef DISABLE_DXF
#  ifndef DISABLE_JSON
      else if ((fmt && !strcasecmp (fmt, "json"))
               || (infile && !strcasecmp (infile, ".json")))
        {
          if (opts > 1)
            fprintf (stderr, "Reading JSON file %s\n",
                     infile ? infile : "from stdin");
          error = dwg_read_json (&dat, &dwg);
        }
#  endif
      else if ((fmt && !strcasecmp (fmt, "dxfb"))
               || (infile && !strcasecmp (infile, ".dxfb")))
        {
          if (opts > 1)
            fprintf (stderr, "Reading Binary DXF file %s\n",
                     infile ? infile : "from stdin");
          error = dxf_read_file (infile ? infile : "-", &dwg);
        }
      else if ((fmt && !strcasecmp (fmt, "dxf"))
               || (infile && !strcasecmp (infile, ".dxf")))
        {
          if (opts > 1)
            fprintf (stderr, "Reading DXF file %s\n",
                     infile ? infile : "from stdin");
          error = dxf_read_file (infile ? infile : "-", &dwg);
        }
      else
#endif
        {
          if (fmt)
            fprintf (stderr, "Invalid or unsupported input format '%s'\n",
                     fmt);
          else if (infile)
            fprintf (stderr, "Missing input format for '%s'\n", infile);
          else
            fprintf (stderr, "Missing input format\n");
          if (infile)
            fclose (dat.fh);
          free (dat.chain);
          exit (1);
        }

      free (dat.chain);
      dat.size = 0;
      if (infile && dat.fh)
        {
          fclose (dat.fh);
          dat.fh = NULL;
        }
      if (error >= DWG_ERR_CRITICAL)
        goto free;

      if (!version)
        dat.version = dwg.header.version = dwg.header.from_version;
      dat.from_version = dwg.header.from_version;
      if (minimal)
        dwg.opts |= DWG_OPTS_MINIMAL;
      dwg.opts |= opts;

      if (!outfile)
        {
          outfile = suffix (infile, "dxf");
          free_outfile = 1;
        }

      if (opts > 1)
        {
          fprintf (stderr, "Writing %s%sDXF file %s",
                   minimal ? "minimal " : "", binary ? "binary " : "",
                   outfile);
          if (version)
            fprintf (stderr, " (from %s to %s)\n",
                     dwg_version_type (dwg.header.from_version),
                     dwg_version_type (dwg.header.version));
          else
            fprintf (stderr, " (%s)\n", dwg_version_type (dwg.header.version));
        }

      {
        struct stat attrib;
        if (stat (outfile, &attrib))
          dat.fh = fopen (outfile, "wb");
        else // exists
          {
            if (!overwrite)
              {
                fprintf (stderr, "File not overwritten: %s, use -y.\n",
                         outfile);
                error |= DWG_ERR_IOERROR;
              }
            else
              {
                if (S_ISREG (attrib.st_mode) && // refuse to remove a directory
                    (access (outfile, W_OK) == 0) // writable
#ifndef _WIN32
                    // refuse to remove a symlink. even with overwrite.
                    // security
                    && !S_ISLNK (attrib.st_mode)
#endif
                )
                  {
                    unlink (outfile);
                    dat.fh = fopen (outfile, "wb");
                  }
                else if ( // for fuzzing mainly
#ifdef _WIN32
                    strEQc (outfile, "NUL")
#else
                strEQc (outfile, "/dev/null")
#endif
                )
                  {
                    dat.fh = fopen (outfile, "wb");
                  }
                else
                  {
                    fprintf (stderr, "Not writable file or symlink: %s\n",
                             outfile);
                    error |= DWG_ERR_IOERROR;
                  }
              }
          }
      }
      if (!dat.fh)
        {
          fprintf (stderr, "WRITE ERROR %s\n", outfile);
          error |= DWG_ERR_IOERROR;
        }
      else
        {
          error |= binary ? dwg_write_dxfb (&dat, &dwg)
                          : dwg_write_dxf (&dat, &dwg);
        }
      if (dat.fh)
        fclose (dat.fh);

    free:
#if defined __SANITIZE_ADDRESS__ || __has_feature(address_sanitizer)
      {
        char *asanenv = getenv ("ASAN_OPTIONS");
        if (!asanenv)
          force_free = 1;
        // detect_leaks is enabled by default. see if it's turned off
        else if (strstr (asanenv, "detect_leaks=0") == NULL) /* not found */
          force_free = 1;
      }
#endif
      // forget about leaks. really huge DWG's need endlessly here.
      if ((dwg.header.version && dwg.num_objects < 1) || force_free
#ifdef HAVE_VALGRIND_VALGRIND_H
          || (RUNNING_ON_VALGRIND)
#endif
      )
        {
          dwg_free (&dwg);
        }

#ifdef __xxAFL_HAVE_MANUAL_CONTROL
    } // __AFL_LOOP(1000) persistent mode
#endif

  if (error >= DWG_ERR_CRITICAL)
    {
      fprintf (stderr, "ERROR 0x%x\n", error);
      if (error && opts > 2)
        dwg_errstrings (error);
    }
  else
    {
      if (opts > 1)
        {
          fprintf (stderr, "SUCCESS 0x%x\n", error);
          if (error && opts > 2)
            dwg_errstrings (error);
        }
      else
        fprintf (stderr, "SUCCESS\n");
    }

  if (free_outfile)
    free (outfile);
  return error >= DWG_ERR_CRITICAL ? 1 : 0;
}
