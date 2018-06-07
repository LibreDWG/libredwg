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
 * into examples/alldwg.inc.
 * with the available likely fields try permutations of most likely types.
 */

#include "config.h"
#include <stdio.h>
#include "dwg.h"
#include "bits.h"

static struct _unknown {
  const char *name;
  const char *bytes;
  const char *bits;
  const char *source;
  const long address; const int bitsize;
} unknowns[] =
  {
    { "MATERIAL", "0e0101000f010100", "10", "150DSC4_AO-46050_QDC-fixed_2000.log", 400137, 1426 },
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

    { 0, NULL, "", "", 0L, 0L }
};

int
main (int argc, char *argv[])
{
  int i;
  for (i=0; unknowns[i].name; i++)
    {
      //TODO: find the shortest objects
      printf("%s: %d (%s)\n", unknowns[i].name, unknowns[i].bitsize, unknowns[i].source);
      //TODO: try likely field combinations and print the top 3.
      //there are various heuristics, like the handle stream at the end
    }

  return 0;
}
