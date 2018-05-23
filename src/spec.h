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

#ifndef SPEC_H
#define SPEC_H

#define DECODER if (0)
#define ENCODER if (0)
#define PRINT   if (0)
#define DXF     if (0)
#define FREE    if (0)
#define IF_ENCODE_FROM_EARLIER   if (0)
#define IF_ENCODE_FROM_PRE_R13   if (0)
#define IF_ENCODE_FROM_SINCE_R13 if (0)
#define IF_IS_ENCODER 0
#define IF_IS_DECODER 0

#ifdef IS_ENCODER
#undef ENCODER
#undef IF_IS_ENCODER
#define IF_IS_ENCODER 1
#define ENCODER if (1)
// when writing, check also rewriting from an earlier version and fill in a default then
#undef IF_ENCODE_FROM_EARLIER
#undef IF_ENCODE_FROM_PRE_R13
#undef IF_ENCODE_FROM_SINCE_R13
#define IF_ENCODE_FROM_EARLIER \
  if (dat->from_version && dat->from_version < cur_ver)
#define IF_ENCODE_FROM_PRE_R13 \
  if (dat->from_version && dat->from_version < R_13)
#define IF_ENCODE_SINCE_R13 \
  if (dat->from_version && dat->from_version >= R_13)
#endif

#ifdef IS_DECODER
#undef  DECODER
#undef IF_IS_DECODER
#define IF_IS_DECODER 1
#define DECODER if (1)
#endif

#if defined(IS_PRINT)
#undef  PRINT
#define PRINT   if (1)
#endif

#if defined(IS_FREE)
#undef FREE
#define FREE    if (1)
#else
#define END_REPEAT(field)
#endif

#define R11OPTS(b) _ent->opts_r11 & b

#ifndef COMMON_TABLE_FLAGS
#define COMMON_TABLE_FLAGS(owner, acdbname) \
  PRE (R_13) \
  { \
    FIELD_RC (flag, 70); \
    FIELD_TF (entry_name, 32, 2); \
    FIELD_RS (used, 0); \
  } \
  LATER_VERSIONS \
  { \
    FIELD_T (entry_name, 2); \
    FIELD_B (xrefref, 0); /* 70 bit 7 */ \
    PRE (R_2007) \
    { \
      FIELD_BS (xrefindex_plus1, 0); \
      FIELD_B (xrefdep, 0); \
    } \
    LATER_VERSIONS \
    { \
      FIELD_B (xrefdep, 0); \
      if (FIELD_VALUE(xrefdep)) { \
        FIELD_BS (xrefindex_plus1, 0); \
      } \
    } \
  }\
  RESET_VER
#endif

#endif /* SPEC_H */
