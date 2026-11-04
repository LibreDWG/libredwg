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
char *ATTRIBUTE_MALLOC htmlutf8escape (const char *restrict utf8);
char *ATTRIBUTE_MALLOC mtext_plaintext (const char *restrict src);
char *ATTRIBUTE_MALLOC mtext_escape_line (const char *restrict line);
char *ATTRIBUTE_MALLOC mtext_wrap_text (const char *restrict text,
                                        double width, double text_height,
                                        double *num_lines);
const char *mtext_attachment_anchor (int attachment);
double mtext_attachment_first_offset (int attachment, double line_height,
                                      double num_lines);
double mtext_line_height (double text_height, int linespace_style,
                          double linespace_factor);
double mtext_svg_angle (const BITCODE_3BD x_axis_dir);

#endif
