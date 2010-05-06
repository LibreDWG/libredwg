/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2009 Free Software Foundation, Inc.                        */
/*  Copyright (C) 2010 Thien-Thi Nguyen                                      */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * load_dwg.c: load a DWG, get lines, text and circles
 * written by Felipe Castro
 * modified by Felipe Corrêa da Silva Sances
 * modified by Thien-Thi Nguyen
 */

#include <dwg.h>
#include "suffix.c"

void
add_line(double x1, double y1, double x2, double y2)
{
  // Make something with that
}

void
add_circle(double x, double y, double R)
{
  // Make something with that
}

void
add_text(double x, double y, char *txt)
{
  // Make something with that
}

int
load_dwg(char *filename)
{
  unsigned int i;
  int success;
  Dwg_Data dwg;

  dwg.num_objects = 0;
  success = dwg_read_file(filename, &dwg);
  for (i = 0; i < dwg.num_objects; i++)
    {
      Dwg_Entity_LINE *line;
      Dwg_Entity_CIRCLE *circle;
      Dwg_Entity_TEXT *text;

      switch (dwg.object[i].type)
        {
      case DWG_TYPE_LINE:
        line = dwg.object[i].tio.entity->tio.LINE;
        add_line(line->start.x, line->end.x, line->start.y, line->end.y);
        break;
      case DWG_TYPE_CIRCLE:
        circle = dwg.object[i].tio.entity->tio.CIRCLE;
        add_circle(circle->center.x, circle->center.y, circle->radius);
        break;
      case DWG_TYPE_TEXT:
        text = dwg.object[i].tio.entity->tio.TEXT;
        add_text(text->insertion_pt.x, text->insertion_pt.y, text->text_value);
        break;
        }
    }
  dwg_free(&dwg);
  return success;
}

int
main (int argc, char *argv[])
{
  REQUIRE_INPUT_FILE_ARG (argc);
  load_dwg (argv[1]);
  return 0;
}
