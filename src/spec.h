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

#include <string.h>
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
#define IF_IS_DXF 0
#define IF_IS_FREE 0

#ifndef ACTION
# error ACTION define missing: decode, encode, dxf, ...
#endif
#define _DWG_FUNC_N(ACTION,name) dwg_ ## ACTION ## _ ## name
#define DWG_FUNC_N(ACTION,name) _DWG_FUNC_N(ACTION,name)

#define SET_PARENT(field, obj)
#define SET_PARENT_OBJ(field)
#define SET_PARENT_FIELD(field, what_parent, obj)

#ifndef M_PI_2
# define M_PI_2      1.57079632679489661923132169163975144
#endif
#define rad2deg(ang) (ang)*90.0/M_PI_2
#define deg2rad(ang) (ang) ? M_PI_2/((ang)*90.0) : 0.0

#ifndef VALUE_HANDLE
# define VALUE_HANDLE(value, handle_code, dxf)
#endif
#ifndef VALUE_TV
# define VALUE_TV(value, dxf)
#endif
#ifndef VALUE_TF
# define VALUE_TF(value, dxf)
#endif
#ifndef VALUE_TFF
# define VALUE_TFF(value, dxf)
#endif
#ifndef VALUE_3BD
# define VALUE_3BD(value, dxf)
#endif
#ifndef VALUE_BL
# define VALUE_BL(value, dxf)
#endif
// logging format overrides
#ifndef FIELD_RLx
# define FIELD_RLx(name, dxf) FIELD_RL(name, dxf)
#endif
#ifndef FIELD_BLx
# define FIELD_BLx(name, dxf) FIELD_BL(name, dxf)
# define FIELD_BSx(name, dxf) FIELD_BS(name, dxf)
#endif
#ifndef FIELD_RCu
# define FIELD_RCu(name, dxf) FIELD_RC(name, dxf)
#endif
#ifndef FIELD_RCd
# define FIELD_RCd(name, dxf) FIELD_RC(name, dxf)
#endif
#ifndef SUBCLASS
# define SUBCLASS(text)
#endif
#ifndef DXF_3DSOLID
# define DXF_3DSOLID
#endif
#ifndef FIELD_2PT_TRACE
#define FIELD_2PT_TRACE(name, type, dxf) \
  LOG_TRACE(#name ": (" FORMAT_BD ", " FORMAT_BD ") [" #type " %d]\n", \
            _obj->name.x, _obj->name.y, dxf)
#define FIELD_3PT_TRACE(name, type, dxf) \
  LOG_TRACE(#name ": (" FORMAT_BD ", " FORMAT_BD ", " FORMAT_BD ") [" #type " %d]\n", \
            _obj->name.x, _obj->name.y, _obj->name.z, dxf)
#endif
#ifndef LOG_TRACE_TF
#define LOG_TRACE_TF(var,len)
#define LOG_INSANE_TF(var,len)
#endif
#ifndef FIELD_EMC
#define FIELD_EMC(a,b,c) FIELD_CMC(a,b,c)
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
#undef DECODER
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
#undef  IF_IS_DXF
#define DXF   if (1)
#define IF_IS_DXF 1
#endif

#if defined(IS_FREE)
#undef FREE
#define FREE    if (1)
#undef IF_IS_FREE
#define IF_IS_FREE 1
#else
#define END_REPEAT(field)
#endif

#define R11OPTS(b) _ent->opts_r11 & b
#define R11FLAG(b) _ent->flag_r11 & b

#define DECODE_UNKNOWN_BITS \
  DECODER { dwg_decode_unknown(dat, (Dwg_Object *restrict)obj); } \
  FREE { VALUE_TF(obj->unknown_bits, 0); }

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
#endif


#ifndef FIELD_VECTOR_N1
# define FIELD_VECTOR_N1(name, type, size, dxf) FIELD_VECTOR_N(name, type, size, dxf)
#endif

#ifndef REPEAT

#define REPEAT_CN(times, name, type) \
  if (_obj->name) \
    for (rcount1=0; rcount1<(BITCODE_BL)times; rcount1++)
#define REPEAT_N(times, name, type) \
  if (dat->version >= R_2000 && times > 0x1000) { \
    fprintf(stderr, "Invalid rcount1 %ld", (long)times); return DWG_ERR_VALUEOUTOFBOUNDS; } \
  if (_obj->name) \
    for (rcount1=0; rcount1<(BITCODE_BL)times; rcount1++)

#define _REPEAT(times, name, type, idx) \
  if (dat->version >= R_2000 && _obj->times > 0x1000) { \
    fprintf(stderr, "Invalid rcount " #idx " %ld", (long)_obj->times); return DWG_ERR_VALUEOUTOFBOUNDS; } \
  if (_obj->name) \
    for (rcount##idx=0; rcount##idx<(BITCODE_BL)_obj->times; rcount##idx++)
#define _REPEAT_C(times, name, type, idx) \
  if (_obj->name) \
    for (rcount##idx=0; rcount##idx<(BITCODE_BL)_obj->times; rcount##idx++)
#define _REPEAT_N(times, name, type, idx) \
  if (_obj->name) \
    for (rcount##idx=0; rcount##idx<(BITCODE_BL)times; rcount##idx++)
#define REPEAT(times, name, type)  _REPEAT(times, name, type, 1)
#define REPEAT2(times, name, type) _REPEAT(times, name, type, 2)
#define REPEAT3(times, name, type) _REPEAT(times, name, type, 3)
#define REPEAT4(times, name, type) _REPEAT(times, name, type, 4)
#define REPEAT_C(times, name, type)  _REPEAT_C(times, name, type, 1)
#define REPEAT2_C(times, name, type) _REPEAT_C(times, name, type, 2)
#define REPEAT3_C(times, name, type) _REPEAT_C(times, name, type, 3)
#define REPEAT4_C(times, name, type) _REPEAT_C(times, name, type, 4)

#endif

#define SPLINE_CONTROL_POINT_VECTOR(name, size, weighted) \
  REPEAT(size, name, Dwg_SPLINE_control_point) \
    { \
      FIELD_3BD(name[rcount1], 10); \
      if (!FIELD_VALUE(weighted)) \
        FIELD_VALUE(name[rcount1].w) = 0; \
      else \
        FIELD_BD(name[rcount1].w, 41); \
    } \
  END_REPEAT(name);

#define _VECTOR_CHKCOUNT(name,size,maxelemsize) {}

#endif /* SPEC_H */
