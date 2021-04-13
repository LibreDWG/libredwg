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

/*
 * dxf2dwg.c: save a DXF as DWG. Detect ascii/binary. No minimal, only full.
 * Optionally as a different version. Only r2000 encode-support so far.
 *
 * written by Reini Urban
 */

#include "../src/config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif
#include "my_stat.h"
#include "my_getopt.h"
#ifdef HAVE_VALGRIND_VALGRIND_H
#  include <valgrind/valgrind.h>
#endif

#include <dwg.h>
#include "common.h"
#include "bits.h"
#include "logging.h"
#include "suffix.inc"

#ifdef __AFL_COMPILER
#  include "decode.h"
#  include "encode.h"
#  include "in_dxf.h"
#endif

static int help (void);

static int opts = 1;
int minimal = 0;
int binary = 0;
int overwrite = 0;
char buf[4096];
/* the current version per spec block */
static unsigned int cur_ver = 0;

static int
usage (void)
{
  printf ("\nUsage: dxf2dwg [-v[N]] [-y] [--as rNNNN] [-o DWG] DXFFILES...\n");
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
          "Existing files are not overwritten, unless -y is given.\n"
          "Encoding currently only works for R13-R2000.\n"
          "\n");
#ifdef HAVE_GETOPT_LONG
  printf ("  -v[0-9], --verbose [0-9]  verbosity\n");
  printf ("  --as rNNNN                save as version\n");
  printf ("           Valid versions:\n");
  printf ("             r12, r14, r2000 (default)\n");
  printf ("           Planned versions:\n");
  printf ("             r9, r10, r11, r2004, r2007, r2010, r2013, r2018\n");
  printf ("  -o outfile, --file        optional, only valid with one single "
          "DXFFILE\n");
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
  printf ("  -o dwgfile  optional, only valid with one single DXFFILE\n");
  printf ("  -h          display this help and exit\n");
  printf ("  -i          output version information and exit\n"
          "\n");
#endif
  printf ("GNU LibreDWG online manual: "
          "<https://www.gnu.org/software/libredwg/>\n");
  return 0;
}

// lsan/valgrind leaks still TODO. GH #151
#if defined __SANITIZE_ADDRESS__ || __has_feature(address_sanitizer)
const char *__asan_default_options (void);
const char *
__asan_default_options (void)
{
  return "detect_leaks=0";
}
#endif

#ifdef __AFL_COMPILER
__AFL_FUZZ_INIT ();
// fastest mode via shared mem
#  define AFL_SHARED_MEM

int
main (int argc, char *argv[])
{
  Dwg_Data dwg;
  Bit_Chain dat = { NULL, 0, 0, 0, 0 };
  Bit_Chain out_dat = { NULL, 0, 0, 0, 0 };
  FILE *fp;
  struct stat attrib;

  GC_INIT ();
  __AFL_INIT ();
  // dat.opts = 3;
#  ifdef AFL_SHARED_MEM
  dat.chain = __AFL_FUZZ_TESTCASE_BUF;
#  endif

  while (__AFL_LOOP (10000))
    { // llvm_mode persistent, non-forking mode
#  ifdef AFL_SHARED_MEM
      dat.size = __AFL_FUZZ_TESTCASE_LEN;
      printf ("Fuzzing in_dxf + encode from shmem (%lu)\n", dat.size);
#  elif 0 // still 10x faster than the old file-forking fuzzer.
      /* from stdin: */
      dat.size = 0;
      // dat.chain = NULL;
      dat_read_stream (&dat, stdin);
      printf ("Fuzzing in_dxf + encode from stdin (%lu)\n", dat.size);
#  else
      /* else from file */
      fp = fopen (argv[1], "rb");
      if (!fp)
        return 0;
      dat.size = 0;
      dat_read_file (&dat, fp, argv[1]);
      fclose (fp);
      printf ("Fuzzing in_dxf + encode from file (%lu)\n", dat.size);
#  endif

      if (dat.size < 100)
        continue; // useful minimum input length
      if (dwg_read_dxf (&dat, &dwg) <= DWG_ERR_CRITICAL)
        {
          memset (&out_dat, 0, sizeof (out_dat));
          bit_chain_set_version (&out_dat, &dat);
          dat.codepage = dwg.header.codepage;
          out_dat.version = R_2000;
          dwg_encode (&dwg, &out_dat);
          FREE (out_dat.chain);
          dwg_free (&dwg);
        }
    }
  dwg_free (&dwg);
}
#  define main orig_main
int orig_main (int argc, char *argv[]);
#endif

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
  int do_free = 0;
  int need_free = 0;
  int c;
#ifdef HAVE_GETOPT_LONG
  int option_index = 0;
  static struct option long_options[]
      = { { "verbose", 1, &opts, 1 }, // optional
          { "file", 1, 0, 'o' },      { "as", 1, 0, 'a' },
          { "overwrite", 0, 0, 'y' }, { "help", 0, 0, 0 },
          { "force-free", 0, 0, 0 },  { "version", 0, 0, 0 },
          { NULL, 0, NULL, 0 } };
#endif

  if (argc < 2)
    return usage ();

  while
#ifdef HAVE_GETOPT_LONG
      ((c = getopt_long (argc, argv, "ya:v::o:h", long_options, &option_index))
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
          if (!strcmp (long_options[option_index].name, "force-free"))
            do_free = 1;
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
        case 'y':
          overwrite = 1;
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

  if (filename_out != NULL && (i + 1) < argc)
    {
      fprintf (stderr, "%s: no -o with multiple input files\n", argv[0]);
      return usage ();
    }
  do_free |= (i + 1) < argc; // if more than one file

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
          if (filename_out != argv[i - 1])
            FREE (filename_out);
          return usage ();
        }

      dwg.opts = opts;
      dwg.header.version = dwg_version;
      printf ("Reading DXF file %s\n", filename_in);
      error = dxf_read_file (filename_in, &dwg);
      if (error >= DWG_ERR_CRITICAL)
        {
          fprintf (stderr, "READ ERROR 0x%x %s\n", error, filename_in);
          if (need_free)
            FREE (filename_out);
          if (do_free
#ifdef HAVE_VALGRIND_VALGRIND_H
              || (RUNNING_ON_VALGRIND)
#endif
          )
            dwg_free (&dwg);
          continue;
        }

      dwg.opts |= opts;
      printf ("Writing DWG file %s", filename_out);
      if (version)
        {
          printf (" as %s\n", version);
          dwg.header.version = dwg_version;
          if (dwg_version > R_2000)
            printf ("Warning: encode currently only works for R13-R2000.\n");
          if (dwg.header.from_version == R_INVALID)
            dwg.header.from_version = dwg.header.version;
        }
      else
        {
          // FIXME: for now only R_13b1 - R_2000. later remove this line.
          if (dwg.header.from_version < R_13b1
              || dwg.header.from_version >= R_2004)
            dwg.header.version = dwg_version;
          if (dwg.header.from_version == R_INVALID)
            dwg.header.from_version = dwg.header.version;
          if (dwg.header.version == R_INVALID)
            dwg.header.version = dwg.header.from_version;
          printf ("\n");
        }

#ifdef USE_WRITE
      {
        struct stat attrib;
        if (!stat (filename_out, &attrib)) // exists
          {
            if (!overwrite)
              {
                LOG_ERROR ("File not overwritten: %s, use -y.\n",
                           filename_out);
                error |= DWG_ERR_IOERROR;
              }
            else
              {
                if (S_ISREG (attrib.st_mode) && // refuse to remove a directory
                    (access (filename_out, W_OK) == 0) // writable
#  ifndef _WIN32
                    // refuse to remove a symlink. even with overwrite.
                    // security
                    && !S_ISLNK (attrib.st_mode)
#  endif
                )
                  {
                    unlink (filename_out);
                    error = dwg_write_file (filename_out, &dwg);
                  }
                else if (
#  ifdef _WIN32
                    strEQc (filename_out, "NUL")
#  else
                    strEQc (filename_out, "/dev/null")
#  endif
                )
                  {
                    error = dwg_write_file (filename_out, &dwg);
                  }
                else
                  {
                    LOG_ERROR ("Not writable file or symlink: %s\n",
                               filename_out);
                    error |= DWG_ERR_IOERROR;
                  }
              }
          }
        else
          error = dwg_write_file (filename_out, &dwg);
      }
#else
      error = DWG_ERR_IOERROR;
#  error no DWG write support
#endif
      if (error)
        fprintf (stderr, "WRITE ERROR 0x%x %s\n", error, filename_out);

#if defined __SANITIZE_ADDRESS__ || __has_feature(address_sanitizer)
      {
        char *asanenv = getenv ("ASAN_OPTIONS");
        if (!asanenv)
          do_free = 1;
        // detect_leaks is enabled by default. see if it's turned off
        else if (strstr (asanenv, "detect_leaks=0") == NULL) /* not found */
          do_free = 1;
      }
#endif
      // forget about leaks. really huge DWG's need endlessly here.
      if (do_free
#ifdef HAVE_VALGRIND_VALGRIND_H
          || (RUNNING_ON_VALGRIND)
#endif
      )
        {
          dwg_free (&dwg);
          if (need_free)
            FREE (filename_out);
        }
      filename_out = NULL;
    }

  // but only the result of the last conversion
  return error >= DWG_ERR_CRITICAL ? 1 : 0;
}
