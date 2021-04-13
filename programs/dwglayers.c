/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2018-2019 Free Software Foundation, Inc.                   */
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
#include <assert.h>
#include "my_getopt.h"
#ifdef HAVE_VALGRIND_VALGRIND_H
#  include <valgrind/valgrind.h>
#endif

#include <dwg.h>
#include "common.h"
#include "bits.h"
#include "logging.h"

static int
usage (void)
{
  printf ("\nUsage: dwglayers [-f|--flags] [--on] <input_file.dwg>\n");
  return 1;
}
static int
opt_version (void)
{
  printf ("dwglayers %s\n", PACKAGE_VERSION);
  return 0;
}
static int
help (void)
{
  printf ("\nUsage: dwglayers [OPTION]... DWGFILE\n");
  printf ("Print list of layers.\n"
          "\n");
#ifdef HAVE_GETOPT_LONG
  printf ("  -x, --extnames            prints EXTNAMES (r13-r14 only)\n"
          "                i.e. space instead of _\n");
  printf ("  -f, --flags               prints also flags:\n"
          "                3 chars for: f for frozen, + or - for ON or OFF, l "
          "for locked\n");
  printf ("  -o, --on                  prints only ON layers\n");
  printf ("  -h, --help                display this help and exit\n");
  printf ("      --version             output version information and exit\n"
          "\n");
#else
  printf ("  -x            prints EXTNAMES (r13-r14 only)\n"
          "                i.e. space instead of _\n");
  printf ("  -f            prints also flags:\n"
          "                3 chars for: f for frozen, + or - for ON or OFF, l "
          "for locked\n");
  printf ("  -o            prints only ON layers\n");
  printf ("  -h            display this help and exit\n");
  printf ("  -i            output version information and exit\n"
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
  long i = 1;
  int flags = 0, on = 0, extnames = 0;
  char *filename_in;
  Dwg_Data dwg;
  Dwg_Object *obj;
  Dwg_Object_LAYER *layer;
  Dwg_Object_LAYER_CONTROL *_ctrl;
  int c;
#ifdef HAVE_GETOPT_LONG
  int option_index = 0;
  static struct option long_options[]
      = { { "flags", 0, 0, 'f' }, { "extnames", 0, 0, 'x' },
          { "on", 0, 0, 'o' },    { "help", 0, 0, 0 },
          { "version", 0, 0, 0 }, { NULL, 0, NULL, 0 } };
#endif

  GC_INIT ();
  while
#ifdef HAVE_GETOPT_LONG
      ((c = getopt_long (argc, argv, "xfoh", long_options, &option_index))
       != -1)
#else
      ((c = getopt (argc, argv, "xfohi")) != -1)
#endif
    {
      if (c == -1)
        break;
      switch (c)
        {
#ifdef HAVE_GETOPT_LONG
        case 0:
          if (!strcmp (long_options[option_index].name, "version"))
            return opt_version ();
          if (!strcmp (long_options[option_index].name, "help"))
            return help ();
          break;
#else
        case 'i':
          return opt_version ();
#endif
        case 'x':
          extnames = 1;
          break;
        case 'f':
          flags = 1;
          break;
        case 'o':
          on = 1;
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
  if (i >= argc)
    return usage ();

  filename_in = argv[i];
  memset (&dwg, 0, sizeof (Dwg_Data));
  error = dwg_read_file (filename_in, &dwg);
  if (error >= DWG_ERR_CRITICAL)
    {
      fprintf (stderr, "READ ERROR %s: 0x%x\n", filename_in, error);
      goto done;
    }

  obj = dwg_get_first_object (&dwg, DWG_TYPE_LAYER_CONTROL);
  if (!obj)
    {
      fprintf (stderr, "No layers found\n");
      goto done;
    }
  _ctrl = obj->tio.object->tio.LAYER_CONTROL;
  for (i = 0; i < _ctrl->num_entries; i++)
    {
      char *name;
      assert (_ctrl->entries);
      if (!_ctrl->entries[i]) // NULL BITCODE_H
        continue;
      obj = _ctrl->entries[i]->obj;
      if (!obj || obj->type != DWG_TYPE_LAYER) // can be DICTIONARY also
        continue;
      assert (_ctrl->entries[i]->obj->tio.object);
      layer = _ctrl->entries[i]->obj->tio.object->tio.LAYER;
      if (on && (layer->off || layer->frozen))
        continue;
      if (flags)
        printf ("%s%s%s\t", layer->frozen ? "f" : " ", layer->off ? "-" : "+",
                layer->locked ? "l" : " ");
      if (extnames && dwg.header.from_version >= R_13b1
          && dwg.header.from_version < R_2000)
        {
          if (!(name = dwg_find_table_extname (&dwg, obj)))
            name = layer->name;
        }
      else
        name = layer->name;
      if (!name)
        {
          LOG_ERROR ("Invalid layer " FORMAT_HV " ignored, empty name",
                     _ctrl->entries[i]->obj->handle.value)
        }
      else
        {
          // since r2007 unicode, converted to utf-8
          if (dwg.header.from_version >= R_2007)
            {
              char *utf8 = bit_convert_TU ((BITCODE_TU)name);
              printf ("%s\n", utf8);
              FREE (utf8);
            }
          else
            printf ("%s\n", name);
        }
      fflush (stdout);
    }

done:
  // forget about valgrind. really huge DWG's need endlessly here.
  if ((dwg.header.version && dwg.num_objects < 1000)
#if defined __SANITIZE_ADDRESS__ || __has_feature(address_sanitizer)
      || 1
#endif
#ifdef HAVE_VALGRIND_VALGRIND_H
      || (RUNNING_ON_VALGRIND)
#endif
  )
    dwg_free (&dwg);
  return error >= DWG_ERR_CRITICAL ? 1 : 0;
}
