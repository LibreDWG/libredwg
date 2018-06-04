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

#include "decode.h"

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

#define SET_PARENT(field, obj)
#define SET_PARENT_OBJ(field)
#define SET_PARENT_FIELD(field, what_parent, obj)

#ifndef VALUE_HANDLE
#define VALUE_HANDLE(value, handle_code, dxf)
#endif
#ifndef VALUE_TV
#define VALUE_TV(value, dxf)
#endif
#ifndef SUBCLASS
#define SUBCLASS(text)
#endif
#ifndef FIELD_2PT_TRACE
#define FIELD_2PT_TRACE(name, type, dxf) \
  LOG_TRACE(#name ": (" FORMAT_BD ", " FORMAT_BD ") [" #type " %d]\n", \
            _obj->name.x, _obj->name.y, dxf)
#define FIELD_3PT_TRACE(name, type, dxf) \
  LOG_TRACE(#name ": (" FORMAT_BD ", " FORMAT_BD ", " FORMAT_BD ") [" #type " %d]\n", \
            _obj->name.x, _obj->name.y, _obj->name.z, dxf)
#endif

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
#undef SET_PARENT
#undef SET_PARENT_OBJ
#undef SET_PARENT_FIELD
#define SET_PARENT(field, to) if (_obj->field) _obj->field->parent = to;
#define SET_PARENT_OBJ(field) SET_PARENT(field, _obj);
#define SET_PARENT_FIELD(field, what_parent, to) \
    if (_obj->field) _obj->field->what_parent = to;
#endif

#if defined(IS_PRINT)
#undef  PRINT
#define PRINT   if (1)
#endif
#if defined(IS_DXF)
#undef  DXF
#define DXF   if (1)
#endif

#if defined(IS_FREE)
#undef FREE
#define FREE    if (1)
#else
#define END_REPEAT(field)
#endif

#define R11OPTS(b) _ent->opts_r11 & b
#define R11FLAG(b) _ent->flag_r11 & b

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
    FIELD_VALUE(flag) = FIELD_VALUE(flag) | \
                        FIELD_VALUE(xrefdep) << 4 | \
                        FIELD_VALUE(xrefref) << 6; \
  }\
  RESET_VER
#endif

// Same as above. just _dwg_object_LAYER::flags is short, not RC
#define LAYER_TABLE_FLAGS(owner, acdbname) \
  PRE (R_13) \
  { \
    FIELD_CAST (flag, RC, RS, 70); \
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
    FIELD_VALUE(flag) = FIELD_VALUE(flag) | \
                        FIELD_VALUE(xrefdep) << 4 | \
                        FIELD_VALUE(xrefref) << 6; \
  }\
  RESET_VER

#ifndef REPEAT

#define REPEAT_CN(times, name, type)                    \
  for (rcount1=0; rcount<(long)times; rcount1++)
#define REPEAT_N(times, name, type) \
  if (dat->version >= R_2000 && times > 0x1000) { \
    fprintf(stderr, "Invalid rcount1 %ld", (long)times); return; } \
  for (rcount1=0; rcount1<(long)times; rcount1++)

#define _REPEAT(times, name, type, idx) \
  if (dat->version >= R_2000 && _obj->times > 0x1000) { \
    fprintf(stderr, "Invalid rcount " #idx " %ld", (long)_obj->times); return; } \
  for (rcount##idx=0; rcount##idx<(long)_obj->times; rcount##idx++)
#define _REPEAT_C(times, name, type, idx) \
  for (rcount##idx=0; rcount##idx<(long)_obj->times; rcount##idx++)
#define REPEAT(times, name, type)  _REPEAT(times, name, type, 1)
#define REPEAT2(times, name, type) _REPEAT(times, name, type, 2)
#define REPEAT3(times, name, type) _REPEAT(times, name, type, 3)
#define REPEAT4(times, name, type) _REPEAT(times, name, type, 4)
#define REPEAT_C(times, name, type)  _REPEAT_C(times, name, type, 1)
#define REPEAT2_C(times, name, type) _REPEAT_C(times, name, type, 2)
#define REPEAT3_C(times, name, type) _REPEAT_C(times, name, type, 3)
#define REPEAT4_C(times, name, type) _REPEAT_C(times, name, type, 4)

#endif


#endif /* SPEC_H */
