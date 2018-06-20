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
 * dwglayers.c: print list of layers in a DWG
 *
 * written by Reini Urban
 */

#include "../src/config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dwg.h>
#include "../src/common.h"
#include "../src/bits.h"
#include "../src/logging.h"
#include "suffix.inc"
static int help(void);
int verbosity(int argc, char **argv, int i, unsigned int *opts);
#include "common.inc"

int minimal = 0;
int binary = 0;
char buf[4096];
/* the current version per spec block */
static unsigned int cur_ver = 0;

static int usage(void) {
  printf("\nUsage: dwglayers [-f|--flags] <input_file.dwg>\n");
  return 1;
}
static int opt_version(void) {
  printf("dwglayers %s\n", PACKAGE_VERSION);
  return 0;
}
static int help(void) {
  printf("\nUsage: dwglayers [OPTION]... DWGFILE\n");
  printf("Print list of layers.\n"
         "\n");
  printf("  -f, --flags               prints also flags:\n"
         "                3 chars for: f for frozen, + or - for ON or OFF, l for locked\n");
  printf("      --on                  prints only ON layers\n");
  printf("      --help                display this help and exit\n");
  printf("      --version             output version information and exit\n"
         "\n");
  printf("GNU LibreDWG online manual: <https://www.gnu.org/software/libredwg/>\n");
  return 0;
}

int
main (int argc, char *argv[])
{
  int error;
  long i = 1;
  int flags = 0, on = 0;
  char* filename_in;
  Dwg_Data dwg;
  Bit_Chain dat;
  Dwg_Object_LAYER *layer;

  // check args
  if (argc < 2)
    return usage();

  if (argc > 2 && (!strcmp(argv[i], "-f") || !strcmp(argv[i], "--flags")))
    {
      flags = 1;
      argc--; i++;
    }
  if (argc > 2 && !strcmp(argv[i], "--on"))
    {
      on = 1;
      argc--; i++;
    }
  if (argc > 1 && !strcmp(argv[i], "--help"))
    return help();
  if (argc > 1 && !strcmp(argv[i], "--version"))
    return opt_version();
  if (argc < 2)
    return usage();
  
  filename_in = argv[i];
  memset(&dwg, 0, sizeof(Dwg_Data));
  dwg.opts = 0;
  error = dwg_read_file(filename_in, &dwg);
  if (error >= DWG_ERR_CRITICAL)
    fprintf(stderr, "READ ERROR %s: 0x%x\n", filename_in, error);

  for (i=0; i < dwg.layer_control.num_entries; i++)
    {
      layer = dwg.layer_control.layers[i]->obj->tio.object->tio.LAYER;
      layer->on = layer->color.index > 0;
      if (on && (!layer->on || layer->frozen))
        continue;
      if (flags)
        printf("%s%s%s\t",
               layer->frozen ? "f" : " ",
               layer->on ?     "+" : "-",
               layer->locked ? "l" : " ");
      // since r2007 unicode, converted to utf-8
      if (dwg.header.version >= R_2007) {
        char *utf8 = bit_convert_TU((BITCODE_TU)layer->entry_name);
        printf("%s\n", utf8);
        free(utf8);
      }
      else
        printf("%s\n", layer->entry_name);
    }

  dwg_free(&dwg);
  return error >= DWG_ERR_CRITICAL ? 1 : 0;
}
