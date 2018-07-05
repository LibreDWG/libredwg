/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2018 Free Software Foundation, Inc.                        */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * dwg2dxf.c: save a DWG as DXF.
 * optionally as a different version.
 *
 * written by Reini Urban
 */

#include "../src/config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <getopt.h>

#include <dwg.h>
#include "common.h"
#include "bits.h"
#include "logging.h"
#include "suffix.inc"
#include "out_dxf.h"

static int opts = 1;
int minimal = 0;
int binary = 0;
char buf[4096];
/* the current version per spec block */
static unsigned int cur_ver = 0;

static int usage(void) {
  printf("\nUsage: dwg2dxf [-v[N]] [--as rNNNN] [-m|--minimal] [-b|--binary] DWGFILES...\n");
  return 1;
}
static int opt_version(void) {
  printf("dwg2dxf %s\n", PACKAGE_VERSION);
  return 0;
}
static int help(void) {
  printf("\nUsage: dwg2dxf [OPTION]... DWGFILES...\n");
  printf("Converts DWG files to DXF.\n");
  printf("Default DXFFILE: DWGFILE with .dxf extension in the current directory.\n"
         "Existing files are silently overwritten.\n"
         "\n");
#ifdef HAVE_GETOPT_LONG
  printf("  -v[0-9], --verbose [0-9]  verbosity\n");
  printf("  --as rNNNN                save as version\n");
  printf("           Valid versions:\n");
  printf("             r12, r14, r2000, r2004, r2007, r2010, r2013\n");
  printf("           Planned versions:\n");
  printf("             r9, r10, r11, r2018\n");
  printf("  -m, --minimal             only $ACADVER, HANDSEED and ENTITIES\n");
  printf("  -b, --binary              save as binary DXF\n");
  printf("  -o outfile, --file        only valid with one single DWGFILE\n");
  printf("      --help                display this help and exit\n");
  printf("      --version             output version information and exit\n"
         "\n");
#else
  printf("  -v[0-9]     verbosity\n");
  printf("  -a rNNNN    save as version\n");
  printf("              Valid versions:\n");
  printf("                r12, r14, r2000 (default)\n");
  printf("              Planned versions:\n");
  printf("                r9, r10, r11, r2004, r2007, r2010, r2013, r2018\n");
  printf("  -m          minimal, only $ACADVER, HANDSEED and ENTITIES\n");
  printf("  -b          save as binary DXF\n");
  printf("  -o dwgfile\n");
  printf("  -h          display this help and exit\n");
  printf("  -i          output version information and exit\n"
         "\n");
#endif
  printf("GNU LibreDWG online manual: <https://www.gnu.org/software/libredwg/>\n");
  return 0;
}

int
main (int argc, char *argv[])
{
  int i = 1;
  int error;
  Dwg_Data dwg;
  char* filename_in;
  const char *version = NULL;
  char* filename_out = NULL;
  Dwg_Version_Type dwg_version;
  Bit_Chain dat;

  int c;
#ifdef HAVE_GETOPT_LONG
  int option_index = 0;
  static struct option long_options[] = {
        {"verbose", 1, &opts, 1}, //optional
        {"file",    1, 0, 'o'},
        {"as",      1, 0, 'a'},
        {"minimal", 0, 0, 'm'},
        {"binary",  0, 0, 'b'},
        {"help",    0, 0, 0},
        {"version", 0, 0, 0},
        {NULL,      0, NULL, 0}
  };
#endif

  if (argc < 2)
    return usage();  

  while
#ifdef HAVE_GETOPT_LONG
    ((c = getopt_long(argc, argv, ":mba:v::o:h",
                      long_options, &option_index)) != -1)
#else
    ((c = getopt(argc, argv, ":mba:v::o:hi")) != -1)
#endif
    {
      if (c == -1) break;
      switch (c) {
      case ':': // missing arg
        if (optarg && !strcmp(optarg, "v")) {
          opts = 1;
          break;
        }
        fprintf(stderr, "%s: option '-%c' requires an argument\n",
                argv[0], optopt);
        break;
#ifdef HAVE_GETOPT_LONG
      case 0:
        /* This option sets a flag */
        if (!strcmp(long_options[option_index].name, "verbose"))
          {
            if (opts < 0 || opts > 9)
              return usage();
# if defined(USE_TRACING) && defined(HAVE_SETENV)
            {
              char v[2];
              *v = opts + '0';
              *(v+1) = 0;
              setenv("LIBREDWG_TRACE", v, 1);
            }
# endif
            break;
          }
        if (!strcmp(long_options[option_index].name, "version"))
          return opt_version();
        if (!strcmp(long_options[option_index].name, "help"))
          return help();
        break;
#else
      case 'i':
        return opt_version();
#endif
      case 'm':
        minimal = 1;
        break;
      case 'b':
        binary = 1;
        break;
      case 'o':
        filename_out = optarg;
        break;
      case 'a':
        dwg_version = dwg_version_as(optarg);
        if (dwg_version == R_INVALID)
          {
            fprintf(stderr, "Invalid version '%s'\n", argv[1]);
            return usage();
          }
        version = optarg;
        break;
      case 'v': // support -v3 and -v
        i = (optind > 0 && optind < argc) ? optind-1 : 1;
        if (!memcmp(argv[i], "-v", 2))
          {
            opts = argv[i][2] ? argv[i][2] - '0' : 1;
          }
        if (opts < 0 || opts > 9)
          return usage();
#if defined(USE_TRACING) && defined(HAVE_SETENV)
        {
          char v[2];
          *v = opts + '0';
          *(v+1) = 0;
          setenv("LIBREDWG_TRACE", v, 1);
        }
#endif
        break;
      case 'h':
        return help();
      case '?':
        fprintf(stderr, "%s: invalid option '-%c' ignored\n",
                argv[0], optopt);
        break;
      default:
        return usage();
      }
    }
  i = optind;

  if (filename_out && i+1 < argc)
    {
      fprintf(stderr, "%s: no -o with multiple input files\n",
              argv[0]);
      return usage();
    }

  while (i < argc)
    {
      filename_in = argv[i];
      i++;
      if (!filename_out)
        filename_out = suffix (filename_in, "dxf");

      memset(&dwg, 0, sizeof(Dwg_Data));
      dwg.opts = opts;
      fprintf(stderr, "Reading DWG file %s\n", filename_in);
      error = dwg_read_file (filename_in, &dwg);
      if (error >= DWG_ERR_CRITICAL) {
        fprintf(stderr, "READ ERROR 0x%x\n", error);
        continue;
      }

      printf("Writing DXF file %s", filename_out);
      if (version)
        {
          printf(" as %s\n", version);
          if (dwg.header.from_version != dwg.header.version)
            dwg.header.from_version = dwg.header.version;
          //else keep from_version = 0
          dwg.header.version = dwg_version;
        }
      else
        {
          printf("\n");
        }
      dat.version = dwg.header.version;
      dat.from_version = dwg.header.from_version;

      if (minimal)
        dwg.opts |= 0x10;
      dat.fh = fopen (filename_out, "wb");
      if (!dat.fh) {
        fprintf (stderr, "WRITE ERROR %s\n", filename_out);
        error = DWG_ERR_IOERROR;
      }
      else {
        error = binary ? dwg_write_dxfb (&dat, &dwg) : dwg_write_dxf (&dat, &dwg);
      }

      if (error >= DWG_ERR_CRITICAL)
        fprintf (stderr, "WRITE ERROR %s\n", filename_out);

      if (dat.fh)
        fclose (dat.fh);

      // forget about valgrind. really huge DWG's need endlessly here.
      if (argc > 1) {
        dwg_free(&dwg);
        free (filename_out);
      }
      filename_out = NULL;
    }

  // but only the result of the last conversion
  return error >= DWG_ERR_CRITICAL ? 1 : 0;
}
