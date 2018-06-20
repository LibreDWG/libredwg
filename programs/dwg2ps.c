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
 * dwg_ps.c: create a PostScript file of lines from a DWG
 * TODO: more 2D elements, see dwg2SVG
 * written by Felipe Castro
 * modified by Felipe Corrêa da Silva Sances
 * modified by Rodrigo Rodrigues da Silva
 * modified by Thien-Thi Nguyen
 * modified by Reini Urban
 */

#include "../src/config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libps/pslib.h>

#include <dwg.h>
#include <dwg_api.h>
#include "../src/common.h"
//#include "../src/bits.h" //bit_convert_TU
#include "suffix.inc"
static int help(void);
int verbosity(int argc, char **argv, int i, unsigned int *opts);
#include "common.inc"

static int usage(void) {
  printf("\nUsage: dwg2ps [-v[0-9]] DWGFILE [PSFILE]\n");
  return 1;
}
static int opt_version(void) {
  printf("dwg2ps %s\n", PACKAGE_VERSION);
  return 0;
}
static int help(void) {
  printf("\nUsage: dwg2ps [OPTION]... DWGFILE [PSFILE]\n");
  printf("Converts some 2D elements of the DWG to a Postscript file.\n"
         "\n");
  printf("  -v[0-9], --verbose [0-9]  verbosity\n");
  printf("           --help           display this help and exit\n");
  printf("           --version        output version information and exit\n"
         "\n");
  printf("GNU LibreDWG online manual: <https://www.gnu.org/software/libredwg/>\n");
  return 0;
}

static void
create_postscript(Dwg_Data *dwg, char *output)
{
  double dx;
  double dy;
  double scale_x;
  double scale_y;
  double scale;
  BITCODE_BL i;
  //FILE *fh;
  PSDoc *ps;

  /* Initialization
   */
  PS_boot();
  ps = PS_new();
  if (PS_open_file(ps, output) < 0)
    {
      puts("Cannot write PostScript file");
      return;
    }

  PS_set_info(ps, "Creator", "dwg2ps " PACKAGE_VERSION);
  //TODO LASTSAVEDBY
  //PS_set_info(ps, "Author", bit_convert_TU(dwg->header_vars.LASTSAVEDBY));
  PS_set_info(ps, "Title", output);
  //PS_set_info(ps, "Keywords", "dwg, postscript, conversion, CAD, plot");

  /* First page: Model Space (?)
   */
  dx = (dwg_model_x_max(dwg) - dwg_model_x_min(dwg));
  dy = (dwg_model_y_max(dwg) - dwg_model_y_min(dwg));
  scale_x = dx / (dwg_model_x_max(dwg) - dwg_model_x_min(dwg));
  scale_y = dy / (dwg_model_y_max(dwg) - dwg_model_y_min(dwg));
  scale = 25.4 / 72; // pt:mm
  PS_begin_page(ps, dx / scale, dy / scale);
  scale *= (scale_x > scale_y ? scale_x : scale_y);
  PS_scale(ps, (float)scale, (float)scale);
  PS_translate(ps, (float)-dwg_model_x_min(dwg), (float)-dwg_model_y_min(dwg));
  if (dwg->opts)
    {
      fprintf (stderr, "Limits: %f, %f\n", dx, dy);
      fprintf (stderr, "Scale: %f (%f, %f)\n", scale, scale_x, scale_y);
    }

  /* Mark the origin with a crossed circle
   */
#define H 1
  PS_circle(ps, 0, 0, H);
  PS_moveto(ps, 0, H);
  PS_lineto(ps, 0, -H);
  PS_moveto(ps, -H, 0);
  PS_lineto(ps, H, 0);
  PS_stroke(ps);

  /* Iterate all entities
   */
  for (i = 0; i < dwg->num_objects; i++)
    {
      Dwg_Object *obj = &dwg->object[i];
      if (obj->supertype == DWG_SUPERTYPE_UNKNOWN) // unknown
        continue;
      if (obj->type == DWG_SUPERTYPE_OBJECT) // not entity
        continue;
      //if (obj->tio.entity->entity_mode == 0) // belongs to block
      //  continue;
      if (obj->type == DWG_TYPE_LINE)
        {
          Dwg_Entity_LINE* line;
          line = obj->tio.entity->tio.LINE;
          PS_moveto(ps, (float)line->start.x, (float)line->start.y);
          PS_lineto(ps, (float)line->end.x, (float)line->end.y);
          PS_stroke(ps);
        }
      else if (obj->type == DWG_TYPE_POLYLINE_2D)
        {
          int error;
          BITCODE_RL j, numpts = dwg_obj_polyline_2d_get_numpoints(obj, &error);
          dwg_point_2d *pts = dwg_obj_polyline_2d_get_points(obj, &error);
          if (numpts && !error)
            {
              PS_moveto(ps, (float)pts[0].x, (float)pts[0].y);
              for (j=1; j<numpts; j++)
                {
                  PS_lineto(ps, (float)pts[j].x, (float)pts[j].y);
                  PS_stroke(ps);
                }
            }
        }
      else if (obj->type == DWG_TYPE_ARC)
        {
          Dwg_Entity_ARC* arc = obj->tio.entity->tio.ARC;
          PS_arc(ps, (float)arc->center.x, (float)arc->center.y, (float)arc->radius,
                     (float)arc->start_angle, (float)arc->end_angle);
        }
      else if (obj->type == DWG_TYPE_CIRCLE)
        {
          Dwg_Entity_CIRCLE* cir = obj->tio.entity->tio.CIRCLE;
          PS_circle(ps, (float)cir->center.x, (float)cir->center.y, (float)cir->radius);
        }
    }

  /* End Model Space */
  PS_end_page(ps);

  PS_close(ps);
  PS_delete(ps);
  PS_shutdown();
}

int
main(int argc, char *argv[])
{
  int error;
  char *outfile;
  int i = 1;
  unsigned int opts = 1; //loglevel 1
  Dwg_Data dwg;

  if (argc < 2)
    return usage();
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
  if (argc > 1 && !strcmp(argv[i], "--help"))
    return help();
  if (argc > 1 && !strcmp(argv[i], "--version"))
    return opt_version();

  REQUIRE_INPUT_FILE_ARG (argc);
  memset(&dwg, 0, sizeof(Dwg_Data));
  dwg.opts = opts;
  error = dwg_read_file(argv[i], &dwg);
  if (error >= DWG_ERR_CRITICAL)
    {
      fprintf(stderr, "READ ERROR 0x%x\n", error);
      dwg_free(&dwg);
      return 1;
    }

  outfile = suffix (argv[i], "ps");
  create_postscript(&dwg, outfile);

  printf ("Success! See the file '%s'\n", outfile);
  free (outfile);
  dwg_free(&dwg);
  return 0;
}

