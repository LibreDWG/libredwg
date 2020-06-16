/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2018-2020 Free Software Foundation, Inc.                   */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * importer.h: overwrite import-specific spec macros,
 *   for decode, in_json, in_dxf.
 * written by Reini Urban
 */

#ifndef IMPORTER_H
#define IMPORTER_H

// redeclare versions to be from, not target
#undef VERSION
#undef NOT_VERSION
#undef VERSIONS
#undef PRE
#undef SINCE
#undef UNTIL
#undef RESET_VER
#define VERSION(v)                                                            \
  cur_ver = v;                                                                \
  if (dat->from_version == v)
#define NOT_VERSION(v)                                                        \
  cur_ver = v;                                                                \
  if (dat->from_version != v)
#define VERSIONS(v1, v2)                                                      \
  cur_ver = v1;                                                               \
  if (dat->from_version >= v1 && dat->from_version <= v2)
#define OTHER_VERSIONS else
#define PRE(v)                                                                \
  cur_ver = v;                                                                \
  if (dat->from_version < v)
#define SINCE(v)                                                              \
  cur_ver = v;                                                                \
  if (dat->from_version >= v)
#define PRIOR_VERSIONS else
#define UNTIL(v)                                                              \
  cur_ver = v;                                                                \
  if (dat->from_version <= v)
#define LATER_VERSIONS else
#define RESET_VER cur_ver = dat->from_version;

#endif
