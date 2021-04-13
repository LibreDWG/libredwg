/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2010-2023 Free Software Foundation, Inc.                   */
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
#include "my_getopt.h"
#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif
#include "my_stat.h"

#include <dwg.h>
#include "../src/common.h"
#include "suffix.inc"

// avoid the slow fork loop, for afl-clang-fast
#ifdef __AFL_COMPILER
static volatile const char *__afl_persistent_sig = "##SIG_AFL_PERSISTENT##";
#endif

static int opts = 1;
static int help (void);

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
  printf ("             r1.4, r2.6, r2.10, r9, r10, r11, r13, r14, r2000 "
          "(default)\n");
  printf ("           Planned versions:\n");
  printf ("             r2004, r2007, r2010, r2013, r2018\n");
  printf ("  -o dwgfile, --file        \n");
  printf ("           --help           display this help and exit\n");
  printf ("           --version        output version information and exit\n"
          "\n");
#else
  printf ("  -v[0-9]     verbosity\n");
  printf ("  -a rNNNN    save as version\n");
  printf ("              Valid versions:\n");
  printf ("                r1.4-r11, r13, r14, r2000 (default)\n");
  printf ("              Planned versions:\n");
  printf ("                r2004-r2018\n");
  printf ("  -o dwgfile\n");
  printf ("  -h          display this help and exit\n");
  printf ("  -i          output version information and exit\n"
          "\n");
#endif
  printf ("GNU LibreDWG online manual: "
          "<https://www.gnu.org/software/libredwg/>\n");
  return 0;
}

#ifdef __AFL_COMPILER
#  include "bits.h"
#  include "decode.h"
#  include "encode.h"
__AFL_FUZZ_INIT ();
int
main (int argc, char *argv[])
{
  Dwg_Data dwg;
  Bit_Chain dat = { NULL, 0, 0, 0, 0 };
  Bit_Chain out_dat = { NULL, 0, 0, 0, 0 };
  FILE *fp;

  GC_INIT ();
  __AFL_INIT ();
  printf ("Fuzzing decode + encode + decode from shared memory\n");

  while (__AFL_LOOP (10000))
    {   // llvm_mode persistent, non-forking mode
#  if 1 // fastest mode via shared mem (crashes still)
      dat.chain = __AFL_FUZZ_TESTCASE_BUF;
      dat.size = __AFL_FUZZ_TESTCASE_LEN;
      // printf ("size: %lu\n", dat.size);
#  elif 1 // still 1000x faster than the old file-forking fuzzer.
      /* from stdin: */
      dat.size = 0;
      // dat.chain = NULL;
      dat_read_stream (&dat, stdin);
#  else
      /* else from file */
      fp = fopen (argv[1], "rb");
      if (!fp)
        return 0;
      dat.size = 0;
      dat_read_file (&dat, fp, argv[1]);
      fclose (fp);
#  endif
      if (dat.size < 100)
        continue; // useful minimum input length
      // dwg in only
      if (dwg_decode (&dat, &dwg) <= DWG_ERR_CRITICAL)
        {
          memset (&out_dat, 0, sizeof (out_dat));
          bit_chain_set_version (&out_dat, &dat);
          out_dat.version = R_2000;
          if (dwg_encode (&dwg, &out_dat) >= DWG_ERR_CRITICAL)
            exit (0);
          dwg_free (&dwg);
          dwg_decode (&out_dat, &dwg);
          FREE (out_dat.chain);
        }
      else
        exit (0);
    }
  dwg_free (&dwg);
}
#  define main orig_main
int orig_main (int argc, char *argv[]);
#endif

int
main (int argc, char *argv[])
{
  int error;
  int i = 1;
  Dwg_Data dwg;
  char *filename_in;
  const char *version = NULL;
  char *filename_out = NULL;
  int free_fnout = 0;
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
#ifdef __AFL_HAVE_MANUAL_CONTROL
  __AFL_INIT ();
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
  dwg.opts = opts & 0xf;

#ifdef __AFL_HAVE_MANUAL_CONTROL
  while (__AFL_LOOP (1000))
    {
#endif

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
            {
              free_fnout = 1;
              filename_out = suffix (filename_in, "-rewrite.dwg");
            }
        }
      if (!filename_out || !strcmp (filename_in, filename_out))
        {
          if (free_fnout)
            FREE (filename_out);
          return usage ();
        }

      /*
       * some very simple testing
       */
      printf ("Reading DWG file %s\n", filename_in);
      error = dwg_read_file (filename_in, &dwg); /* 1st read */
      if (error >= DWG_ERR_CRITICAL)
        fprintf (stderr, "READ ERROR 0x%x\n", error);
      num_objects = dwg.num_objects;
      if (!num_objects)
        {
          printf ("Read 0 objects\n");
          if (error >= DWG_ERR_CRITICAL)
            {
              if (free_fnout)
                FREE (filename_out);
              dwg_free (&dwg);
              return error;
            }
        }

      // if (opts)
      //   printf ("\n");
      printf ("Writing DWG file %s", filename_out);
      if (version)
        { // forced -as-rXXX
          printf (" as %s\n", version);
          if (dwg.header.from_version != dwg.header.version)
            dwg.header.from_version = dwg.header.version;
          // else keep from_version
          dwg.header.version = dwg_version;
        }
      else if (dwg.header.version > R_2000)
        {
          // we cannot yet write 2004+
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
            if (S_ISREG (attrib.st_mode) && // refuse to remove a directory
                (access (filename_out, W_OK) == 0) // is writable
#ifndef _WIN32
                // refuse to remove a symlink. even with overwrite. security
                && !S_ISLNK (attrib.st_mode)
#endif
            )
              unlink (filename_out);
            else
              {
                fprintf (stderr, "ERROR: Not writable file or symlink: %s\n",
                         filename_out);
                error |= DWG_ERR_IOERROR;
              }
          }
      }

      if (opts)
        fprintf (stderr, "\n==========================================\n");
      error = dwg_write_file (filename_out, &dwg);
      if (error >= DWG_ERR_CRITICAL)
        {
          printf ("WRITE ERROR 0x%x\n", error);
#ifndef IS_RELEASE
          // try to read the halfway written r2004 file.
          if (!(version && error == DWG_ERR_SECTIONNOTFOUND))
#endif
            {
              if (free_fnout)
                FREE (filename_out);
              dwg_free (&dwg);
              return error;
            }
        }
      dwg_free (&dwg);

      // try to read again
      // if (opts)
      //  printf ("\n");
      printf ("Re-reading created file %s\n", filename_out);
      if (opts)
        fprintf (stderr, "\n==========================================\n");
      error = dwg_read_file (filename_out, &dwg); /* 2nd read */
      if (error >= DWG_ERR_CRITICAL)
        printf ("re-READ ERROR 0x%x\n", error);
      if (num_objects && (num_objects != dwg.num_objects))
        printf ("re-READ num_objects: %lu, should be %lu\n",
                (unsigned long)dwg.num_objects, (unsigned long)num_objects);
      dwg_free (&dwg);

#ifdef __AFL_HAVE_MANUAL_CONTROL
    }
#endif

  if (free_fnout)
    FREE (filename_out);
  return error >= DWG_ERR_CRITICAL ? error : 0;
}
