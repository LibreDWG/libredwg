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

#ifndef SPEC_H
#  define SPEC_H

#  define _GNU_SOURCE
#  include <string.h>
#  include "decode.h"

#  define DECODER if (0)
#  define ENCODER if (0)
#  define PRINT if (0)
/* DECODER_ENCODER_OR_JSON really, or just NOT_DXF */
#  define DECODER_OR_ENCODER if (0)
#  define DXF_OR_PRINT if (0)
#  define DXF if (0)
#  define JSON if (0)
#  define FREE if (0)
#  define IF_FREE_OR_SINCE(x) SINCE (x)
#  define IF_FREE_OR_VERSIONS(x,y) VERSIONS(x, y)
#  define IF_ENCODE_FROM_EARLIER if (0)
#  define IF_ENCODE_FROM_EARLIER_OR_DXF if (0)
#  define IF_ENCODE_FROM_PRE_R13 if (0)
#  define IF_ENCODE_FROM_SINCE_R13 if (0)
#  define IF_IS_ENCODER 0
#  define IF_IS_DECODER 0
#  define IF_IS_DXF 0
#  define IF_IS_FREE 0

#  ifndef ACTION
#    error ACTION define missing: decode, encode, dxf, json, ...
#  endif
#  define _DWG_FUNC_N(ACTION, name) dwg_##ACTION##_##name
#  define DWG_FUNC_N(ACTION, name) _DWG_FUNC_N (ACTION, name)
#  define _DWG_PRIVATE_N(ACTION, name) dwg_##ACTION##_##name##_private
#  define DWG_PRIVATE_N(ACTION, name) _DWG_PRIVATE_N (ACTION, name)

#  define SET_PARENT(field, obj)
#  define SET_PARENT_OBJ(field)
#  define SET_PARENT_FIELD(field, what_parent, obj)

#  ifndef ISFIRST
#    define ISFIRST
#    define SETFIRST
#    define CLEARFIRST
#  endif

// for compile-time range checks with n=3,10,1000,5000,10000,20000,100000
//#  define LOG2_APPROX(n) (size_t)((-0.344845 * (n) * (n)) + (2.024658 * (n)) - 1.674873)
//#  define _IN_RANGE     (sizeof (_obj->field) >= LOG2_APPROX (maxvalue) / 8)
#  define _IN_RANGE(var, n)                                                   \
    ((sizeof (var) == 1 && n <= 0xff) || (sizeof (var) == 2 && n <= 0xffff)   \
     || (sizeof (var) >= 4))

#  ifndef IS_FREE
#    define VALUEOUTOFBOUNDS(field, maxvalue)                                 \
      if (_IN_RANGE (_obj->field, maxvalue)                                   \
          && _obj->field > maxvalue)                                          \
        {                                                                     \
          LOG_ERROR ("Invalid %s." #field " %lu", obj->name,                  \
                     (unsigned long)_obj->field);                             \
          _obj->field = 0;                                                    \
          return DWG_ERR_VALUEOUTOFBOUNDS;                                    \
        }
#    define SUB_VALUEOUTOFBOUNDS(o,field, maxvalue)                           \
      if (_IN_RANGE (_obj->o.field, maxvalue)                                 \
          && _obj->o.field > maxvalue)                                        \
        {                                                                     \
          LOG_ERROR ("Invalid %s." #field " %lu", obj->name,                  \
                     (unsigned long)_obj->o.field);                           \
          _obj->o.field = 0;                                                  \
          return DWG_ERR_VALUEOUTOFBOUNDS;                                    \
        }
#  else
#    define VALUEOUTOFBOUNDS(field, maxvalue)                                 \
      if (_IN_RANGE (_obj->field, maxvalue)                                   \
          && _obj->field > maxvalue)                                          \
        {                                                                     \
          return DWG_ERR_VALUEOUTOFBOUNDS;                                    \
        }
#    define SUB_VALUEOUTOFBOUNDS(o,field, maxvalue)                           \
      if (_IN_RANGE (_obj->o.field, maxvalue)                                 \
          && _obj->o.field > maxvalue)                                        \
        {                                                                     \
          return DWG_ERR_VALUEOUTOFBOUNDS;                                    \
        }
#  endif

#endif /* SPEC_H */

#ifndef VALUE_HANDLE
#  define VALUE_HANDLE(value, nam, handle_code, dxf)
#endif
#ifndef VALUE_B
#  define VALUE_B(value, dxf)
#endif
#ifndef VALUE_TV
#  define VALUE_TV(value, dxf)
#endif
#ifndef VALUE_TF
#  define VALUE_TF(value, dxf)
#endif
#ifndef VALUE_TFF
#  define VALUE_TFF(value, dxf)
#endif
#ifndef VALUE_3BD
#  define VALUE_3BD(value, dxf)
#endif
#ifndef VALUE_2RD
#  define VALUE_2RD(value, dxf)
#endif
#ifndef VALUE_2BD
#  define VALUE_2BD(value, dxf) VALUE_2RD(value, dxf)
#endif
#ifndef VALUE_3RD
#  define VALUE_3RD(value, dxf) VALUE_3BD (value, dxf)
#endif
#ifndef VALUE_BS
#  define VALUE_BS(value, dxf)
#endif
#ifndef VALUE_BSd
#  define VALUE_BSd(value, dxf)
#endif
#ifndef VALUE_BL
#  define VALUE_BL(value, dxf)
#endif
#ifndef KEY
#  define KEY(nam)
#endif
#ifndef BLOCK_NAME
#  define BLOCK_NAME(nam, dxf) FIELD_T (nam, dxf)
#endif
// sub fields
#ifndef FIELDG
#  define FIELDG(nam, type, dxf) FIELD (nam, type)
#endif
#ifndef SUB_FIELD_BSd
#  define SUB_FIELD_BSd(o, nam, dxf) FIELD_BSd (o.nam, dxf)
#endif
#ifndef SUB_FIELD_BSx
#  define SUB_FIELD_BSx(o, nam, dxf) FIELD_BSx (o.nam, dxf)
#endif
#ifndef SUB_FIELD_RSx
#  define SUB_FIELD_RSx(o, nam, dxf) FIELD_RSx (o.nam, dxf)
#endif
#ifndef SUB_FIELD_TU
#  define SUB_FIELD_TU(o, nam, dxf) FIELD_TU (o.nam, dxf)
#endif
#ifndef SUB_FIELD_T
#  define SUB_FIELD_T(o, nam, dxf) FIELD_T (o.nam, dxf)
#endif
#ifndef SUB_FIELD_TV
#  define SUB_FIELD_TV(o, nam, dxf) FIELD_TV (o.nam, dxf)
#endif
#ifndef SUB_FIELD_TF
#  define SUB_FIELD_TF(o, nam, len, dxf) FIELD_TF (o.nam, _obj->o.len, dxf)
#endif
#ifndef SUB_FIELD_BL
#  define SUB_FIELD_B(o, nam, dxf) FIELDG (o.nam, B, dxf)
#  define SUB_FIELD_BB(o, nam, dxf) FIELDG (o.nam, BB, dxf)
#  define SUB_FIELD_3B(o, nam, dxf) FIELDG (o.nam, 3B, dxf)
#  define SUB_FIELD_BS(o, nam, dxf) FIELDG (o.nam, BS, dxf)
#  define SUB_FIELD_BL(o, nam, dxf) FIELDG (o.nam, BL, dxf)
#  define SUB_FIELD_BLx(o, nam, dxf) FIELD_BLx (o.nam, dxf)
#  define SUB_FIELD_BLd(o, nam, dxf) FIELD_BLd (o.nam, dxf)
#  define SUB_FIELD_RC(o, nam, dxf) FIELDG (o.nam, RC, dxf)
#  define SUB_FIELD_RS(o, nam, dxf) FIELDG (o.nam, RS, dxf)
#  define SUB_FIELD_RL(o, nam, dxf) FIELDG (o.nam, RL, dxf)
#  define SUB_FIELD_BLL(o, nam, dxf) FIELDG (o.nam, BLL, dxf)
#  define SUB_FIELD_RLL(o, nam, dxf) FIELDG (o.nam, RLL, dxf)
#  define SUB_FIELD_BD(o, nam, dxf) FIELD_BD (o.nam, dxf)
#  define SUB_FIELD_RD(o, nam, dxf) FIELD_RD (o.nam, dxf)
#  define SUB_FIELD_2RD(o, nam, dxf) FIELD_2RD (o.nam, dxf)
#  define SUB_FIELD_2BD(o, nam, dxf) FIELD_2BD (o.nam, dxf)
#  define SUB_FIELD_2BD_1(o, nam, dxf) FIELD_2BD_1 (o.nam, dxf)
#  define SUB_FIELD_3RD(o, nam, dxf) FIELD_3RD (o.nam, dxf)
#  define SUB_FIELD_3BD(o, nam, dxf) FIELD_3BD (o.nam, dxf)
#  define SUB_FIELD_3BD_inl(o, nam, dxf) FIELD_3BD (o, dxf)
#  define SUB_FIELD_3DPOINT(o, nam, dxf) FIELD_3BD (o.nam, dxf)
//# define SUB_FIELD_ENC(o,nam,dxf1,dxf2) FIELD_ENC(o.nam, dxf1,dxf2)
#endif
#ifndef SUB_HANDLE_VECTOR
#  define SUB_HANDLE_VECTOR(o, nam, sizefield, code, dxf)                     \
  if (_obj->o.sizefield && _obj->o.nam)                                       \
    {                                                                         \
      BITCODE_BL _size = _obj->o.sizefield;                                   \
      for (vcount = 0; vcount < _size; vcount++)                              \
        {                                                                     \
          SUB_FIELD_HANDLE (o, nam[vcount], code, dxf);                       \
        }                                                                     \
    }
#endif

#ifndef SUB_FIELD_VECTOR
#  define SUB_FIELD_VECTOR(o, nam, sizefield, type, dxf)                      \
  if (_obj->o.sizefield && _obj->o.nam)                                       \
    {                                                                         \
      BITCODE_BL _size = _obj->o.sizefield;                                   \
      for (vcount = 0; vcount < _size; vcount++)                              \
        {                                                                     \
          SUB_FIELD (o, nam[vcount], type, dxf);                              \
        }                                                                     \
    }
#endif
#ifndef SUB_FIELD_VECTOR_N
#  define SUB_FIELD_VECTOR_N(o, nam, type, size, dxf)                         \
  if (size > 0 && _obj->o.nam != NULL)                                        \
    {                                                                         \
      BITCODE_BL _size = (BITCODE_BL)size;                                    \
      for (vcount = 0; vcount < _size; vcount++)                              \
        {                                                                     \
          SUB_FIELD (o, nam[vcount], type, dxf);                              \
        }                                                                     \
    }
#endif
#ifndef FIELD_VECTOR_INL
#  define FIELD_VECTOR_INL(o, nam, type, size, dxf)                           \
  FIELD_VECTOR_N(o, nam, type, size, dxf)
#endif
#ifndef SUB_FIELD_VECTOR_INL
#  define SUB_FIELD_VECTOR_INL(o, nam, type, size, dxf)                       \
  SUB_FIELD_VECTOR_N(o, nam, type, size, dxf)
#endif
#ifndef SUB_FIELD_2RD_VECTOR
#  define SUB_FIELD_2RD_VECTOR(o,name, size, dxf)                             \
  if (_obj->o.size > 0)                                                       \
    {                                                                         \
      for (vcount = 0; vcount < (BITCODE_BL)_obj->o.size; vcount++)           \
        {                                                                     \
          SUB_FIELD_2RD (o,name[vcount], dxf);                                \
        }                                                                     \
    }
#endif
#ifndef SUB_FIELD_3BD_VECTOR
#  define SUB_FIELD_3BD_VECTOR(o,name, size, dxf)                             \
  if (_obj->o.size > 0)                                                       \
    {                                                                         \
      for (vcount = 0; vcount < (BITCODE_BL)_obj->o.size; vcount++)           \
        {                                                                     \
          SUB_FIELD_3BD (o,name[vcount], dxf);                                \
        }                                                                     \
    }
#endif
#ifndef SUB_FIELD_VECTOR_TYPESIZE
#  define SUB_FIELD_VECTOR_TYPESIZE(o, nam, size, typesize, dxf)              \
  if (_obj->o.size && _obj->o.nam)                                            \
    {                                                                         \
      for (vcount = 0; vcount < (BITCODE_BL)_obj->o.size; vcount++)           \
      {                                                                       \
        switch (typesize)                                                     \
          {                                                                   \
          case 0:                                                             \
            break;                                                            \
          case 1:                                                             \
            SUB_FIELD (o, nam[vcount], RC, dxf);                              \
            break;                                                            \
          case 2:                                                             \
            SUB_FIELD (o, nam[vcount], RS, dxf);                              \
            break;                                                            \
          case 4:                                                             \
            SUB_FIELD (o, nam[vcount], RL, dxf);                              \
            break;                                                            \
          case 8:                                                             \
            SUB_FIELD (o, nam[vcount], RLL, dxf);                             \
            break;                                                            \
          default:                                                            \
            LOG_ERROR ("Unkown SUB_FIELD_VECTOR_TYPE " #nam " typesize %d",   \
                       typesize);                                             \
            break;                                                            \
          }                                                                   \
      }                                                                       \
    }
#endif

// logging format overrides
#ifndef FIELD_RLx
#  define FIELD_RLx(name, dxf) FIELD_RL (name, dxf)
#endif
#ifndef FIELD_RSx
#  define FIELD_RSx(name, dxf) FIELD_RS (name, dxf)
#endif
#ifndef FIELD_RCx
#  define FIELD_RCx(name, dxf) FIELD_RC (name, dxf)
#endif
#ifndef FIELD_BLx
#  define FIELD_BLx(name, dxf) FIELD_BL (name, dxf)
#endif
#ifndef FIELD_TFv
#  define FIELD_TFv(name, len, dxf) FIELD_TF (name, len, dxf)
#endif
#ifndef FIELD_TFFx
#  define FIELD_TFFx(name, len, dxf) FIELD_TFF (name, len, dxf)
#endif
#ifndef FIELD_TU32
#  define FIELD_TU32(name, dxf) FIELD_TV (name, dxf)
#endif
#ifndef FIELD_RLd
#  define FIELD_RLd(name, dxf) FIELD_RL (name, dxf)
#endif
#ifndef FIELD_BLd
#  define FIELD_BLd(name, dxf) FIELD_BL (name, dxf)
#endif
#ifndef FIELD_BSx
#  define FIELD_BSx(name, dxf) FIELD_BS (name, dxf)
#endif
#ifndef FIELD_BSd
#  define FIELD_BSd(name, dxf) FIELD_BS (name, dxf)
#endif
#ifndef FIELD_RCu
#  define FIELD_RCu(name, dxf) FIELD_RC (name, dxf)
#endif
#ifndef FIELD_RCd
#  define FIELD_RCd(name, dxf) FIELD_RC (name, dxf)
#endif
#ifndef VALUE_BINARY
#  define VALUE_BINARY(value, len, dxf)
#endif
#ifndef FIELD_BINARY
#  define FIELD_BINARY(name, len, dxf) FIELD_TF (name, len, dxf)
#endif
// force truecolor
#ifndef FIELD_CMTC
#  define FIELD_CMTC(name, dxf)                                               \
    {                                                                         \
      Dwg_Version_Type _ver = dat->version;                                   \
      if (dat->version < R_2004)                                              \
        dat->version = R_2004;                                                \
      FIELD_CMC (name, dxf);                                                  \
      dat->version = _ver;                                                    \
    }
#endif
#ifndef SUB_FIELD_CMTC
#  define SUB_FIELD_CMTC(o, name, dxf)                                        \
    {                                                                         \
      Dwg_Version_Type _ver = dat->version;                                   \
      if (dat->version < R_2004)                                              \
        dat->version = R_2004;                                                \
      SUB_FIELD_CMC (o, name, dxf);                                           \
      dat->version = _ver;                                                    \
    }
#endif
// on DXF skip if 0
#ifndef FIELD_BD0
#  define FIELD_BD0(name, dxf) FIELD_BD (name, dxf)
#  define FIELD_BL0(name, dxf) FIELD_BL (name, dxf)
#  define SUB_FIELD_BL0(o, name, dxf) SUB_FIELD_BL (o, name, dxf)
#  define FIELD_BS0(name, dxf) FIELD_BS (name, dxf)
#  define FIELD_B0(name, dxf) FIELD_B (name, dxf)
#  define FIELD_RC0(name, dxf) FIELD_RC (name, dxf)
#  define FIELD_BT0(name, dxf) FIELD_BT (name, dxf)
#  define FIELD_T0(name, dxf) FIELD_T (name, dxf)
#  define FIELD_HANDLE0(name, code, dxf) FIELD_HANDLE (name, code, dxf)
#  define SUB_FIELD_HANDLE0(o, name, code, dxf) SUB_FIELD_HANDLE (o, name, code, dxf)
#endif

// double to text
#ifndef FIELD_D2T
#  define FIELD_D2T(name, dxf) FIELD_TV (name, dxf)
#endif
#ifndef LOG_TRACE_TF
#  define LOG_TRACE_TF(var, len)
#  define LOG_INSANE_TF(var, len)
#endif
#ifndef SUBCLASS
#  define SUBCLASS(text)
#endif
#ifndef DXF_3DSOLID
#  define DXF_3DSOLID
#endif
#ifndef JSON_3DSOLID
#  define JSON_3DSOLID
#endif
#ifndef FIELD_2PT_TRACE
#  define FIELD_2PT_TRACE(name, type, dxf)                                    \
    LOG_TRACE (#name ": (" FORMAT_BD ", " FORMAT_BD ") [" #type " %d]\n",     \
               _obj->name.x, _obj->name.y, dxf)
#  define FIELD_3PT_TRACE(name, type, dxf)                                    \
    LOG_TRACE (#name ": (" FORMAT_BD ", " FORMAT_BD ", " FORMAT_BD            \
                     ") [" #type " %d]\n",                                    \
               _obj->name.x, _obj->name.y, _obj->name.z, dxf)
#endif
#ifndef FIELD_ENC
#  define FIELD_ENC(a, b, c) FIELD_CMC (a, b, c)
#endif
#ifndef SUB_FIELD_ENC
#  define SUB_FIELD_ENC(a, b, c, d) SUB_FIELD_CMC (a, b, c, d)
#endif

#ifdef IS_ENCODER
#  undef ENCODER
#  undef IF_IS_ENCODER
#  define IF_IS_ENCODER 1
#  define ENCODER if (1)
#  undef DECODER_OR_ENCODER
#  define DECODER_OR_ENCODER if (1)
// when writing, check also rewriting from an earlier version and fill in
// defaults then
#  undef IF_ENCODE_FROM_EARLIER
#  undef IF_ENCODE_FROM_EARLIER_OR_DXF
#  undef IF_ENCODE_FROM_PRE_R13
#  undef IF_ENCODE_FROM_SINCE_R13
#  define IF_ENCODE_FROM_EARLIER                                              \
    if (dat->from_version && dat->from_version < cur_ver)
#  define IF_ENCODE_FROM_EARLIER_OR_DXF                                       \
    if ((dat->from_version && dat->from_version < cur_ver) || dwg->opts & DWG_OPTS_INDXF)
#  define IF_ENCODE_FROM_PRE_R13                                              \
    if (dat->from_version && dat->from_version < R_13)
#  define IF_ENCODE_SINCE_R13                                                 \
    if (dat->from_version && dat->from_version >= R_13)
#endif

#ifdef IS_DECODER
#  undef DECODER
#  undef IF_IS_DECODER
#  undef DECODER_OR_ENCODER
#  define IF_IS_DECODER 1
#  define DECODER if (1)
#  define DECODER_OR_ENCODER if (1)
#  undef SET_PARENT
#  undef SET_PARENT_OBJ
#  undef SET_PARENT_FIELD
#  define SET_PARENT(field, to)                                               \
    if (_obj->field)                                                          \
      _obj->field->parent = to;
#  define SET_PARENT_OBJ(field) SET_PARENT (field, _obj);
#  define SET_PARENT_FIELD(field, what_parent, to)                            \
    if (_obj->field)                                                          \
      _obj->field->what_parent = to;
#else
#  define TRACE_DD
#endif

#if defined(IS_PRINT)
#  undef PRINT
#  define PRINT if (1)
#  undef DXF_OR_PRINT
#  define DXF_OR_PRINT if (1)
#endif

#if defined(IS_DXF)
#  undef DXF
#  define DXF if (1)
#  undef DXF_OR_PRINT
#  define DXF_OR_PRINT if (1)
#  undef IF_IS_DXF
#  define IF_IS_DXF 1
#endif

#if defined(IS_JSON)
#  undef JSON
#  define JSON if (1)
#  undef DXF_OR_PRINT
#  define DXF_OR_PRINT if (1)
#  undef DECODER_OR_ENCODER
#  define DECODER_OR_ENCODER if (1)
#endif

#if defined(IS_FREE)
#  undef FREE
#  define FREE if (1)
#  undef IF_IS_FREE
#  define IF_IS_FREE 1
#  undef IF_FREE_OR_SINCE
#  define IF_FREE_OR_SINCE(x) if (1)
#  undef IF_FREE_OR_VERSIONS
#  define IF_FREE_OR_VERSIONS(x,y) if (1)
#else
#  ifndef END_REPEAT
#    define END_REPEAT(field)
#  endif
#endif

#ifndef R11OPTS
#  define R11OPTS(b) _ent->opts_r11 &b
#  define R11FLAG(b) _ent->flag_r11 &b
#endif

#define DECODE_UNKNOWN_BITS                                                   \
  DECODER { dwg_decode_unknown (dat, (Dwg_Object * restrict) obj); }          \
  FREE { VALUE_TF (obj->unknown_bits, 0); }

#ifndef START_OBJECT_HANDLE_STREAM
#  define START_OBJECT_HANDLE_STREAM                                          \
    START_HANDLE_STREAM;                                                      \
    assert (obj->supertype == DWG_SUPERTYPE_OBJECT)
#endif

#ifndef CONTROL_HANDLE_STREAM
#  define CONTROL_HANDLE_STREAM                                               \
    assert (obj->supertype == DWG_SUPERTYPE_OBJECT);                          \
    PRE (R_2007) {                                                            \
      hdl_dat->byte = dat->byte;                                              \
      hdl_dat->bit = dat->bit;                                                \
    }                                                                         \
    SINCE (R_13) {                                                            \
      VALUE_HANDLE (obj->tio.object->ownerhandle, ownerhandle, 4, 0);         \
      REACTORS (4)                                                            \
      XDICOBJHANDLE (3)                                                       \
    }
#endif

#ifndef COMMON_TABLE_FLAGS
#  define COMMON_TABLE_FLAGS(acdbname)                                        \
    assert (obj->supertype == DWG_SUPERTYPE_OBJECT);                          \
    PRE (R_13)                                                                \
    {                                                                         \
      if (strcmp (#acdbname, "Layer") == 0)                                   \
        {                                                                     \
        FIELD_CAST (flag, RC, RS, 70);                                        \
        }                                                                     \
      else                                                                    \
        {                                                                     \
          FIELD_RC (flag, 70);                                                \
        }                                                                     \
      FIELD_TFv (name, 32, 2);                                                \
      FIELD_RS (used, 0);                                                     \
    }                                                                         \
    LATER_VERSIONS                                                            \
    {                                                                         \
      FIELD_T (name, 2);                                                      \
      UNTIL (R_2004)                                                          \
      {                                                                       \
        FIELD_B (is_xref_ref, 0);       /* always 1, 70 bit 6 */              \
        FIELD_BS (is_xref_resolved, 0); /* 0 or 256 */                        \
        FIELD_B (is_xref_dep, 0);       /* 70 bit 4 */                        \
      }                                                                       \
      LATER_VERSIONS                                                          \
      {                                                                       \
        FIELD_VALUE (is_xref_ref) = 1;                                        \
        FIELD_BS (is_xref_resolved, 0); /* 0 or 256 */                        \
        if (FIELD_VALUE (is_xref_resolved) == 256)                            \
          FIELD_VALUE (is_xref_dep) = 1;                                      \
      }                                                                       \
      FIELD_HANDLE (xref, 5, 0); /* NULLHDL without is_xref_dep */            \
      FIELD_VALUE (flag) |= FIELD_VALUE (is_xref_dep) << 4                    \
                          | FIELD_VALUE (is_xref_ref) << 6;                   \
    }                                                                         \
    RESET_VER
#endif

#ifndef FIELD_VECTOR_N1
#  define FIELD_VECTOR_N1(name, type, size, dxf)                              \
    FIELD_VECTOR_N (name, type, size, dxf)
#endif

#ifndef REPEAT_BLOCK
#  define REPEAT_BLOCK {
#  define END_REPEAT_BLOCK }
#endif

/* REPEAT names:
  _ adds idx
  C does no checks
  N does constant times (else _obj->times)
  F does not calloc/free
*/

// unchecked with constant times
#ifndef REPEAT
#  define REPEAT_CN(times, name, type)                                        \
    if (_obj->name != NULL)                                                   \
      for (rcount1 = 0; rcount1 < (BITCODE_BL)times; rcount1++)
// checked with constant times
#  define REPEAT_N(times, name, type)                                         \
    if (dat->version >= R_2000 && (BITCODE_BL)times > 20000)                  \
      {                                                                       \
        LOG_ERROR ("Invalid %s." #name " rcount1 %ld", SAFEDXFNAME,           \
                   (long)times);                                              \
        return DWG_ERR_VALUEOUTOFBOUNDS;                                      \
      }                                                                       \
    if (_obj->name != NULL)                                                   \
      for (rcount1 = 0; rcount1 < (BITCODE_BL)times; rcount1++)

// checked with var. times
#  define _REPEAT(times, name, type, idx)                                     \
    if (dat->version >= R_2000 && (BITCODE_BL)_obj->times > 20000)            \
      {                                                                       \
        LOG_ERROR ("Invalid %s." #name " rcount" #idx " %ld", SAFEDXFNAME,    \
                   (long)_obj->times);                                        \
        return DWG_ERR_VALUEOUTOFBOUNDS;                                      \
      }                                                                       \
    if (_obj->times > 0 && _obj->name != NULL)                                \
      for (rcount##idx = 0; rcount##idx < (BITCODE_BL)_obj->times;            \
           rcount##idx++)
// unchecked with var. times
#  ifndef _REPEAT_C
#    define _REPEAT_C(times, name, type, idx)                                 \
      if (_obj->times > 0 && _obj->name != NULL)                              \
        for (rcount##idx = 0; rcount##idx < (BITCODE_BL)_obj->times;          \
             rcount##idx++)
#  endif
#  define REPEAT(times, name, type) _REPEAT (times, name, type, 1)
#  define REPEAT2(times, name, type) _REPEAT (times, name, type, 2)
#  define REPEAT3(times, name, type) _REPEAT (times, name, type, 3)
#  define REPEAT4(times, name, type) _REPEAT (times, name, type, 4)
#  define REPEAT_C(times, name, type) _REPEAT_C (times, name, type, 1)
#  define REPEAT2_C(times, name, type) _REPEAT_C (times, name, type, 2)
#  define REPEAT3_C(times, name, type) _REPEAT_C (times, name, type, 3)
#  define REPEAT4_C(times, name, type) _REPEAT_C (times, name, type, 4)
#endif
// unchecked with constant times
#ifndef _REPEAT_CN
#  define _REPEAT_CN(times, name, type, idx)                                  \
    if (_obj->name != NULL)                                                   \
      for (rcount##idx = 0; rcount##idx < (BITCODE_BL)times; rcount##idx++)
#endif
// not allocating versions:
// unchecked
#ifndef _REPEAT_CNF
#  define _REPEAT_CNF(times, name, type, idx)                                 \
    if (_obj->name != NULL)                                                   \
      for (rcount##idx = 0; rcount##idx < (BITCODE_BL)times; rcount##idx++)
#endif
#ifndef _REPEAT_NF
// checked
#  define _REPEAT_NF(times, name, type, idx)                                  \
    if (dat->version >= R_2000 && times > 0x7ff)                              \
      {                                                                       \
        LOG_ERROR ("Invalid %s." #name " rcount" #idx " %ld", SAFEDXFNAME,    \
                   (long)times);                                              \
        return DWG_ERR_VALUEOUTOFBOUNDS;                                      \
      }                                                                       \
    if (_obj->name != NULL)                                                   \
      for (rcount##idx = 0; rcount##idx < (BITCODE_BL)times; rcount##idx++)
#endif

#define DWG_SUBCLASS_DECL(parenttype, subtype)                                \
  static int DWG_PRIVATE_N (ACTION, parenttype##_##subtype)                   \
    (Dwg_Object_##parenttype *restrict _obj, Bit_Chain *dat,                  \
     Bit_Chain *hdl_dat,                                                      \
     Bit_Chain *str_dat, Dwg_Object *restrict obj)                            \

#define DWG_SUBCLASS(parenttype, subtype)                                     \
  static int DWG_PRIVATE_N (ACTION, parenttype##_##subtype)                   \
    (Dwg_Object_##parenttype *restrict _obj, Bit_Chain *dat,                  \
     Bit_Chain *hdl_dat,                                                      \
     Bit_Chain *str_dat, Dwg_Object *restrict obj)                            \
  {                                                                           \
    BITCODE_BL vcount, rcount3, rcount4;                                      \
    Dwg_Data *dwg = obj->parent;                                              \
    int error = 0;                                                            \
    subtype##_fields;                                                         \
    return error;                                                             \
  }

#define CALL_SUBCLASS(_xobj, parenttype, subtype)                             \
  error |= DWG_PRIVATE_N (ACTION, parenttype##_##subtype) (_xobj, dat,        \
               hdl_dat, str_dat, (Dwg_Object *)obj)
// if the name is compile-time known
#define CALL_ENTITY(name, xobj)                                               \
  error |= DWG_PRIVATE_N (ACTION, name) (dat, hdl_dat, str_dat,               \
                                             (Dwg_Object *)xobj)
// TODO: dispatch on the type
#define CALL_SUBENT(hdl, dxf)
//error |= DWG_PRIVATE_N (ACTION, xobj->fixedtype) (dat, hdl_dat, str_dat, (Dwg_Object *)xobj)
#define CALL_SUBCURVE(hdl, curvetype)
