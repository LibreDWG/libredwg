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
#include <getopt.h>
#ifdef HAVE_VALGRIND_VALGRIND_H
#include <valgrind/valgrind.h>
#endif

#include <dwg.h>
#include "common.h"
#include "bits.h"
#include "logging.h"

static int usage(void) {
  printf("\nUsage: dwglayers [-f|--flags] [--on] <input_file.dwg>\n");
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
#ifdef HAVE_GETOPT_LONG
  printf("  -f, --flags               prints also flags:\n"
         "                3 chars for: f for frozen, + or - for ON or OFF, l for locked\n");
  printf("      --on                  prints only ON layers\n");
  printf("      --help                display this help and exit\n");
  printf("      --version             output version information and exit\n"
         "\n");
#else
  printf("  -f            prints also flags:\n"
         "                3 chars for: f for frozen, + or - for ON or OFF, l for locked\n");
  printf("  -o            prints only ON layers\n");
  printf("  -h            display this help and exit\n");
  printf("  -i            output version information and exit\n"
         "\n");
#endif  
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
  Dwg_Object_LAYER *layer;
  int c;
#ifdef HAVE_GETOPT_LONG
  int option_index = 0;
  static struct option long_options[] = {
        {"flags",   0, 0, 'f'},
        {"on",      0, 0, 'o'},
        {"help",    0, 0, 0},
        {"version", 0, 0, 0},
        {NULL,      0, NULL, 0}
  };
#endif

  while
#ifdef HAVE_GETOPT_LONG
    ((c = getopt_long(argc, argv, "foh",
                      long_options, &option_index)) != -1)
#else
    ((c = getopt(argc, argv, "fohi")) != -1)
#endif
    {
      if (c == -1) break;
      switch (c) {
#ifdef HAVE_GETOPT_LONG
      case 0:
        if (!strcmp(long_options[option_index].name, "version"))
          return opt_version();
        if (!strcmp(long_options[option_index].name, "help"))
          return help();
        break;
#else
      case 'i':
        return opt_version();
#endif
      case 'f':
        flags = 1;
        break;
      case 'o':
        on = 1;
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
  if (i >= argc)
    return usage();
  
  filename_in = argv[i];
  memset(&dwg, 0, sizeof(Dwg_Data));
  error = dwg_read_file(filename_in, &dwg);
  if (error >= DWG_ERR_CRITICAL)
    fprintf(stderr, "READ ERROR %s: 0x%x\n", filename_in, error);

  for (i=0; i < dwg.layer_control.num_entries; i++)
    {
      Dwg_Object *obj = dwg.layer_control.layers[i]->obj;
      if (obj->type != DWG_TYPE_LAYER) //can be DICTIONARY also
        break;
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

  // forget about valgrind. really huge DWG's need endlessly here.
  if ((dwg.header.version && dwg.num_objects < 1000)
#ifdef HAVE_VALGRIND_VALGRIND_H
      || (RUNNING_ON_VALGRIND)
#endif
      )
    dwg_free(&dwg);
  return error >= DWG_ERR_CRITICAL ? 1 : 0;
}
