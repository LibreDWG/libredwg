/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2023 Free Software Foundation, Inc.                        */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * odaversion.c: prints the ODA compatible DWG version
 * modified by Reini Urban
   oda understands those versions:
    "ACAD9","ACAD10","ACAD12",
    "ACAD13","ACAD14",
    "ACAD2000","ACAD2004",
    "ACAD2007","ACAD2010",
    "ACAD2013","ACAD2018"
*/

#include <stdio.h>

int
main (int argc, char *argv[])
{
  FILE *f;
  char buf[7];
  size_t size;

  if (argc != 2)
    return 1;
  f = fopen (argv[1], "rb");
  if (!f)
    return 1;
  size = fread (buf, 1, 6, f);
  if (size != 6)
    return 1;
  buf[6] = '\0';
  if (buf[0] == 'A' && buf[1] == 'C')
    {
      int v;
      if (1 != sscanf (buf, "AC%4d", &v))
        return 1;
      if (v < 1004) // <r9
        printf ("\n");
      else if (v <= 1005)
        printf ("9\n");
      else if (v <= 1006)
        printf ("10\n");
      else if (v <= 1009)
        printf ("11\n");
      else if (v <= 1012)
        printf ("13\n");
      else if (v <= 1014)
        printf ("14\n");
      else if (v <= 1016)
        printf ("2000\n");
      else if (v <= 1018)
        printf ("2004\n");
      else if (v <= 1021)
        printf ("2007\n");
      else if (v <= 1024)
        printf ("2010\n");
      else if (v < 1028)
        printf ("2013\n");
      else if (v < 1033)
        printf ("2018\n");
      else
        printf ("\n");
    }
  fclose (f);
  return 0;
}
