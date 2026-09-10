/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2019 Free Software Foundation, Inc.                        */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * escape.h: SVG helpers
 * written by Reini Urban
 */

#ifndef OUT_ESCAPE_H
#define OUT_ESCAPE_H

#include "dwg.h"
#include "common.h"
#include "codepages.h"

char *ATTRIBUTE_MALLOC htmlescape (const char *restrict src,
                                   const Dwg_Codepage codepage);
char *ATTRIBUTE_MALLOC htmlwescape (BITCODE_TU wsrc);
char *ATTRIBUTE_MALLOC htmlutf8escape (const char *restrict src);
char *ATTRIBUTE_MALLOC mtext_escape_line (const char *line);
char *ATTRIBUTE_MALLOC mtext_wrap_text (const char *src, double rect_width,
                                        double text_height,
                                        double width_factor);
const char *mtext_attachment_anchor (BITCODE_BS attachment);
double mtext_svg_angle (double x_axis_x, double x_axis_y);
double mtext_line_height (double text_height, double linespace_factor);
double mtext_attachment_first_offset (BITCODE_BS attachment,
                                      double text_height,
                                      double line_height,
                                      unsigned int num_lines);

#endif
