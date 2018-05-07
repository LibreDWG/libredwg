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

#include <stdio.h>
#include "../src/config.h"
// strings.h or string.h
#ifdef AX_STRCASECMP_HEADER
# include AX_STRCASECMP_HEADER
#endif

#include <dwg.h>
#include "../src/bits.h"
#include "suffix.inc"
#include "in_json.h"
#include "in_dxf.h"
#include "in_dxfb.h"
static int help(void);
int verbosity(int argc, char **argv, int i, unsigned int *opts);
#include "common.inc"

static int usage(void) {
  printf("\nUsage: dwgwrite [-v[0-9]] [-I FMT] [-o DWGFILE] INFILE\n");
  return 1;
}
static int opt_version(void) {
  printf("dwgwrite %s\n", PACKAGE_VERSION);
  return 0;
}
static int help(void) {
  printf("\nUsage: dwgwrite [OPTION]... [-o DWGFILE] INFILE\n");
  printf("Writes a DWG file (not yet functional)\n"
         "\n");
  printf("  -v[0-9], --verbose [0-9]  verbosity\n");
  printf("  -I fmt,  --format fmt     fmt: JSON, DXF, DXFB\n"); //TODO: YAML, XML, SVG, PS, ...
  printf("  -o dwgfile                \n");
  printf("           --help           display this help and exit\n");
  printf("           --version        output version information and exit\n"
         "\n");
  printf("GNU LibreDWG online manual: <https://www.gnu.org/software/libredwg/>\n");
  return 0;
}

int
main(int argc, char *argv[])
{
  unsigned int opts = 1; //loglevel 1
  int i = 1;
  int error;
  Dwg_Data dwg;
  const char *fmt = NULL;
  const char *infile = NULL;
  char *outfile = NULL;
  Bit_Chain dat;

  if (argc < 2)
    {
      return usage();
    }
#if defined(USE_TRACING) && defined(HAVE_SETENV)
  setenv("LIBREDWG_TRACE", "1", 0);
#endif
  if (argc > 2 &&
      (!strcmp(argv[i], "--verbose") ||
       !strncmp(argv[i], "-v", 2)))
    {
      int num_args = verbosity(argc, argv, i, &opts);
      argc -= num_args;
      i += num_args;
    }
  if (argc > 2 &&
      (!strcmp(argv[i], "--format") ||
       !strncmp(argv[i], "-I", 2)))
    {
      int num_args;
      if (!strncmp(argv[i], "-I", 2) && strcmp(argv[i], "-I")) //-Ifmt
        {
          fmt = argv[i]+2;
          num_args = 1;
        }
      else //-I fmt | --format fmt
        {
          fmt = argv[i+1];
          num_args = 2;
        }
      argc -= num_args;
      i += num_args;
    }
  if (argc > 2 && !strcmp(argv[i], "-o"))
    {
      outfile = argv[i+1];
      argc -= 2;
      i += 2;
    }

  if (argc > 1 && !strcmp(argv[i], "--help"))
    return help();
  if (argc > 1 && !strcmp(argv[i], "--version"))
    return opt_version();

  //TODO: get input format from INFILE, not outfile.
  //With stdin, should -I be mandatory, or try to autodetect the format?
  //With a file use the extension.
  if (argc > 1)
    {
      infile = argv[i];
      argc -= 1;
      i += 1;
      if (!fmt)
        {
          if (strstr(infile, ".json") || strstr(infile, ".JSON"))
            fmt = (char*)"json";
          else
          if (strstr(infile, ".dxf") || strstr(infile, ".DXF"))
            fmt = (char*)"dxf";
          else
          if (strstr(infile, ".dxfb") || strstr(infile, ".DXFB"))
            fmt = (char*)"dxfb";
          else {
            fprintf(stderr, "Unknown input format for %s\n", infile);
          }
        }
    }

  //TODO: allow stdin, but require -I|--format then
  //REQUIRE_INPUT_FILE_ARG (argc);
  dwg.opts = opts;
  if (infile)
    dat.fh = fopen(infile, "w");
  else
    dat.fh = stdin;

  /*if (!strcasecmp(fmt, "json"))
    error = dwg_read_json(&dat, &dwg);
  else if (!strcasecmp(fmt, "dxfb"))
    error = dwg_read_dxfb(&dat, &dwg);
  else if (!strcasecmp(fmt, "dxf"))
    error = dwg_read_dxf(&dat, &dwg);
  else */ {
    fprintf(stderr, "Invalid input format %s\n", fmt);
    if (argc)
      fclose(dat.fh);
    exit(1);
  }
  if (argc)
    fclose(dat.fh);

  //dat.version = dat.from_version = dwg.header.version;
  error = dwg_write_file(outfile, &dwg);
  if (error)
    printf("\nERROR\n");
  else
    printf("\nSUCCESS\n");
  dwg_free(&dwg);

  return error;
}
