/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2009, 2018 Free Software Foundation, Inc.                  */
/*  Copyright (C) 2010 Thien-Thi Nguyen                                      */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * dwgread.c: read a DWG file, print verbose logging, and output to
 *            various formats.
 * written by Felipe Castro
 * modified by Felipe Corrêa da Silva Sances
 * modified by Rodrigo Rodrigues da Silva
 * modified by Thien-Thi Nguyen
 * modified by Reini Urban
 */

#include "../src/config.h"
#include <stdio.h>
#include <string.h>
// strings.h or string.h
#ifdef AX_STRCASECMP_HEADER
# include AX_STRCASECMP_HEADER
#endif

#include "dwg.h"
#include "../src/bits.h"
#include "suffix.inc"
#include "out_json.h"
#include "out_dxf.h"

static int help(void);
int verbosity(int argc, char **argv, int i, unsigned int *opts);
#include "common.inc"

static int usage(void) {
  printf("\nUsage: dwgread [-v[0-9]] [-O FMT] [-o OUTFILE] [DWGFILE|-]\n");
  return 1;
}
static int opt_version(void) {
  printf("dwgread %s\n", PACKAGE_VERSION);
  return 0;
}
static int help(void) {
  printf("\nUsage: dwgread [OPTION]... DWGFILE\n");
  printf("Reads the DWG into some optional output format to stdout or some file,\n"
         "and prints error, success or verbose internal progress to stderr.\n"
         "\n");
  printf("  -v[0-9], --verbose [0-9]  verbosity\n");
  printf("  -O fmt,  --format fmt     fmt: DXF, DXFB, JSON, GeoJSON\n");
  printf("           Planned output formats:  YAML, XML/OGR, GPX, SVG, PS\n");
  printf("  -o outfile                also defines the output fmt. Default: stdout\n");
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
  const char *outfile = NULL;
  int has_v = 0;

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
      has_v = 1;
      argc -= num_args;
      i += num_args;
    }
  if (argc > 2 &&
      (!strcmp(argv[i], "--format") ||
       !strncmp(argv[i], "-O", 2)))
    {
      int num_args;
      if (!strncmp(argv[i], "-O", 2) && strcmp(argv[i], "-O")) //-Ofmt
        {
          fmt = argv[i]+2;
          num_args = 1;
        }
      else //-O fmt | --format fmt
        {
          fmt = argv[i+1];
          num_args = 2;
        }
      if (!has_v) opts = 0;
      argc -= num_args;
      i += num_args;
    }
  if (argc > 2 && !strcmp(argv[i], "-o"))
    {
      outfile = argv[i+1];
      argc -= 2;
      i += 2;
      if (!fmt)
        {
          if (strstr(outfile, ".json") || strstr(outfile, ".JSON"))
            fmt = (char*)"json";
          else
          if (strstr(outfile, ".dxf") || strstr(outfile, ".DXF"))
            fmt = (char*)"dxf";
          else
          if (strstr(outfile, ".dxfb") || strstr(outfile, ".DXFB"))
            fmt = (char*)"dxfb";
          else
          if (strstr(outfile, ".geojson") || strstr(outfile, ".GeoJSON"))
            fmt = (char*)"geojson";
          else {
            fprintf(stderr, "Unknown output format for %s\n", outfile);
          }
        }
    }
  if (argc > 1 && !strcmp(argv[i], "--help"))
    return help();
  if (argc > 1 && !strcmp(argv[i], "--version"))
    return opt_version();

  REQUIRE_INPUT_FILE_ARG (argc);
  memset(&dwg, 0, sizeof(Dwg_Data));
  if (has_v || !fmt)
    dwg.opts = opts;
  error = dwg_read_file(argv[i], &dwg);
  if (!fmt)
    {
      if (error >= DWG_ERR_CRITICAL)
        printf("\nERROR 0x%x\n", error);
      else
        printf("\nSUCCESS 0x%x\n", error);
    }
  else
    {
      Bit_Chain dat;
      if (outfile)
        dat.fh = fopen(outfile, "w");
      else
        dat.fh = stdout;
      dat.version = dat.from_version = dwg.header.version;
      // TODO --as-rNNNN version? for now not.
      // we want the native dump, converters are seperate.

      if (!strcasecmp(fmt, "json"))
        error = dwg_write_json(&dat, &dwg);
      else if (!strcasecmp(fmt, "dxfb"))
        error = dwg_write_dxfb(&dat, &dwg);
      else if (!strcasecmp(fmt, "dxf"))
        error = dwg_write_dxf(&dat, &dwg);
      else if (!strcasecmp(fmt, "geojson"))
        error = dwg_write_geojson(&dat, &dwg);
      else {
        fprintf(stderr, "Invalid output format %s\n", fmt);
      }
      if (outfile)
        {
          fclose(dat.fh);
          if (error >= DWG_ERR_CRITICAL)
            printf("\nERROR 0x%x\n", error);
          else
            printf("\nSUCCESS 0x%x\n", error);
        }
    }
  if (dwg.header.version)
    dwg_free(&dwg);

  return error >= DWG_ERR_CRITICAL ? 1 : 0;
}
