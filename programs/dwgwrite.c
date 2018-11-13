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

/* TODO: format readers, outfile logic
 * dwgwrite.c: write a DWG file from various input formats.
 * written by Reini Urban
 */

#include "../src/config.h"
#include <stdio.h>
#include <string.h>
// strings.h or string.h
#ifdef AX_STRCASECMP_HEADER
# include AX_STRCASECMP_HEADER
#endif
#include <getopt.h>

#include "dwg.h"
#include "bits.h"
#include "suffix.inc"
#include "in_json.h"
#include "in_dxf.h"

static int opts = 1;

static int help(void);

static int usage(void) {
  printf("\nUsage: dwgwrite [-v[0-9]] [--as rNNNN] [-I FMT] [-o DWGFILE] INFILE\n");
  return 1;
}
static int opt_version(void) {
  printf("dwgwrite %s\n", PACKAGE_VERSION);
  return 0;
}
static int help(void) {
  printf("\nUsage: dwgwrite [OPTION]... [-o DWGFILE] INFILE\n");
  printf("Writes a DWG file from various input formats. Only r2000 for now\n"
         "\n");
#ifdef HAVE_GETOPT_LONG
  printf("  -v[0-9], --verbose [0-9]  verbosity\n");
  printf("  --as rNNNN                save as version\n");
  printf("           Valid versions:\n");
  printf("             r12, r14, r2000 (default)\n");
  printf("           Planned versions:\n");
  printf("             r9, r10, r11, r2004, r2007, r2010, r2013, r2018\n");
  printf("  -I fmt,  --format fmt     DXF, DXFB\n");
  printf("           Planned input formats: JSON, GeoJSON, YAML, XML/OGR, GPX\n");
  printf("  -o dwgfile, --file        \n");
  printf("           --help           display this help and exit\n");
  printf("           --version        output version information and exit\n"
         "\n");
#else
  printf("  -v[0-9]     verbosity\n");
  printf("  -a rNNNN    save as version\n");
  printf("              Valid versions:\n");
  printf("                r12, r14, r2000 (default)\n");
  printf("              Planned versions:\n");
  printf("                r9, r10, r11, r2004, r2007, r2010, r2013, r2018\n");
  printf("  -I fmt      fmt: DXF, DXFB\n");
  printf("              Planned input formats: JSON, GeoJSON, YAML, XML/OGR, GPX\n");
  printf("  -o dwgfile\n");
  printf("  -h          display this help and exit\n");
  printf("  -i          output version information and exit\n"
         "\n");
#endif
  printf("GNU LibreDWG online manual: <https://www.gnu.org/software/libredwg/>\n");
  return 0;
}

int
main(int argc, char *argv[])
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
#ifdef HAVE_GETOPT_LONG
  int option_index = 0;
  static struct option long_options[] = {
        {"verbose", 1, &opts, 1}, //optional
        {"format",  1, 0, 'I'},
        {"file",    1, 0, 'o'},
        {"as",      1, 0, 'a'},
        {"help",    0, 0, 0},
        {"version", 0, 0, 0},
        {NULL,      0, NULL, 0}
  };
#endif

  if (argc < 2)
    return usage();

  while
#ifdef HAVE_GETOPT_LONG
    ((c = getopt_long(argc, argv, ":a:v::I:o:h",
                      long_options, &option_index)) != -1)
#else
    ((c = getopt(argc, argv, ":a:v::I:o:hi")) != -1)
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
      case 'I':
        fmt = optarg;
        break;
      case 'o':
        outfile = optarg;
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

  //get input format from INFILE, not outfile.
  //With stdin, should -I be mandatory, or try to autodetect the format?
  //With a file use the extension.
  if (optind < argc) //have arg
    {
      infile = argv[i];
      if (!fmt)
        {
          if (strstr(infile, ".json") || strstr(infile, ".JSON"))
            fmt = (char*)"json";
          else
          if (strstr(infile, ".dxfb") || strstr(infile, ".DXFB"))
            fmt = (char*)"dxfb";
          else if (strstr(infile, ".dxf") || strstr(infile, ".DXF"))
            fmt = (char*)"dxf";
          else
            fprintf(stderr, "Unknown input format for '%s'\n", infile);
        }
    }

  //allow stdin, but require -I|--format then
  memset(&dwg, 0, sizeof(Dwg_Data));
  dwg.opts = opts;
  if (infile)
    dat.fh = fopen(infile, "r");
  else
    dat.fh = stdin;

  /*if ((fmt && !strcasecmp(fmt, "json")) ||
        (infline && !strcasecmp(infile, ".json")))
    error = dwg_read_json(&dat, &dwg);
  else */
  if ((fmt && !strcasecmp(fmt, "dxfb")) ||
      (infile && !strcasecmp(infile, ".dxfb")))
    error = dwg_read_dxfb(&dat, &dwg);
  else if ((fmt && !strcasecmp(fmt, "dxf")) ||
           (infile && !strcasecmp(infile, ".dxf")))
    error = dxf_read_file(infile, &dwg); // ascii or binary
  else {
    if (fmt)
      fprintf(stderr, "Invalid input format '%s'\n", fmt);
    else if (infile)
      fprintf(stderr, "Missing input format for '%s'\n", infile);
    else
      fprintf(stderr, "Missing input format\n");
    if (argc)
      fclose(dat.fh);
    exit(1);
  }
  if (argc)
    fclose(dat.fh);

  if (dwg.header.from_version != dwg.header.version)
    dwg.header.from_version = dwg.header.version;
  if (version) {
    dat.version = dwg.header.version = dwg_version;
  }
  else {
    dat.version = dwg.header.version = R_2000;
  }
  if (!outfile)
    outfile = suffix (infile, "dwg");
  error |= dwg_write_file(outfile, &dwg);
  // forget about valgrind. really huge DWG's need endlessly here.
  //dwg_free(&dwg);

  return error >= DWG_ERR_CRITICAL ? 1 : 0;
}
