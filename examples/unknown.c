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
 * unknown.c: decode unknown bitstreams
 * written by Reini Urban
 *
 * A sample program to find the most likely
 * fields for all unknown dwg entities and objects.
 * gather all binary raw data from all unknown dwg entities and objects
 * into examples/alldwg.inc and examples/alldxf.inc
 * with the available likely fields try permutations of most likely types.
 */

#include "config.h"
#include <stdio.h>
#include <string.h>
#include "dwg.h"
#include "bits.h"

struct _unknown_field {
  int code;
  char *value;
  int type;
  int pos;
};
static struct _unknown_dxf {
  const char *name;
  const char *dxf;
  const unsigned int handle;
  const char *bytes;
  const char *bits;
  const struct _unknown_field *fields;
} unknown_dxf[] = {
    // see log_unknown_dxf.pl
    #include "alldxf_0.inc"
    { NULL, NULL, 0, "", "", NULL }
};
#include "alldxf_1.inc"  

/* not needed for the solver, only to check against afterwards */
#if 0
static struct _unknown {
  const char *name;
  const char *bytes;
  const char *bits;
  const char *log; const char *dxf;
  const unsigned int handle; const int bitsize;
} unknowns[] =
  {
   { "MATERIAL", "0e0101000f010100", "10", "150DSC4_AO-46050_QDC-fixed_2000.log", NULL, 400137, 1426 },
    /* the following types:
      5 ACDBASSOCGEOMDEPENDENCY
      3 ACDBASSOCNETWORK
     53 ACDBDETAILVIEWSTYLE
     53 ACDBSECTIONVIEWSTYLE
      5 ACDB_LEADEROBJECTCONTEXTDATA_CLASS
     63 CELLSTYLEMAP
    245 MATERIAL
      1 MULTILEADER
      1 SUN
     97 TABLESTYLE
    */
    // see log_unknown.pl
    #include "alldwg.inc"

    { 0, NULL, "", "", NULL, 0L, 0L }
};
#endif

int
main (int argc, char *argv[])
{
  int i, j;
  #include "alldxf_2.inc"
  for (i=0; unknown_dxf[i].name; i++)
    {
      int num_fields;
      const struct _unknown_field *g = unknown_dxf[i].fields;
      int len = strlen(unknown_dxf[i].bytes);
      //TODO offline: find the shortest objects.
      printf("\n%s: %X %s (%d)\n", unknown_dxf[i].name, unknown_dxf[i].handle,
             len < 200 ? unknown_dxf[i].bytes : "...", len);
      for (j=0; g[j].code; j++)
        {
          if (g[j].code == 100)
            printf("%d: %s\n", g[j].code, g[j].value);
        }
      num_fields = j;

      //TODO: try likely field combinations and print the top 3.
      //there are various heuristics, like the handle stream at the end
    }

  return 0;
}
