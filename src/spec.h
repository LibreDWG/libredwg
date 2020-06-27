/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2018-2025 Free Software Foundation, Inc.                   */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

#ifndef SPEC_H
#  define SPEC_H

#  undef _GNU_SOURCE
#  define _GNU_SOURCE
#  include <string.h>
#  include "bits.h"
#  include "codepages.h"
#  include "decode.h"

#  define DECODER if (0)
#  define ENCODER if (0)
#  define PRINT if (0)
/* DECODER_ENCODER_OR_JSON really, or just NOT_DXF */
#  define DECODER_OR_ENCODER if (0)
#  define DXF_OR_PRINT if (0)
#  define DXF_OR_FREE if (0)
#  define DXF if (0)
#  define JSON if (0)
#  define FREE if (0)
#  define IF_FREE_OR_SINCE(x) SINCE (x)
#  define IF_FREE_OR_VERSIONS(x, y) VERSIONS (x, y)
#  ifndef IF_ENCODE_FROM_EARLIER
#    define IF_ENCODE_FROM_EARLIER if (0)
#    define IF_ENCODE_FROM_EARLIER_OR_DXF if (0)
#    define IF_ENCODE_FROM_PRE_R13 if (0)
#    define IF_ENCODE_FROM_PRE_R2000 if (0)
#  endif
#  ifndef IF_ENCODE_SINCE_R13
#    define IF_ENCODE_SINCE_R13 if (0)
#  endif
#  define IF_IS_ENCODER 0
#  define IF_IS_DECODER 0
#  define IF_IS_DXF 0
#  define IF_IS_FREE 0

#  ifndef ACTION
#    error ACTION define missing: decode, encode, dxf, json, ...
#  endif
// call either the object, the common object or its subclass/subent
// implementation
#  define _DWG_FUNC_N(ACTION, NAME) dwg_##ACTION##_##NAME
#  define DWG_FUNC_N(ACTION, NAME) _DWG_FUNC_N (ACTION, NAME)
#  define _DWG_COMMON_N(ACTION, NAME) dwg_##ACTION##_##NAME##_common
#  define DWG_COMMON_N(ACTION, NAME) _DWG_COMMON_N (ACTION, NAME)
#  define _DWG_IMPL_N(ACTION, NAME) dwg_##ACTION##_##NAME##_impl
#  define DWG_IMPL_N(ACTION, NAME) _DWG_IMPL_N (ACTION, NAME)
#  define _DWG_SUBENT_N(ACTION) dwg_##ACTION##_subent
#  define DWG_SUBENT_N(ACTION) _DWG_SUBENT_N (ACTION)

#  define SET_PARENT(field, obj)
#  define SET_PARENT_OBJ(field)
#  define SET_PARENT_FIELD(field, what_parent, obj)

#  ifndef ISFIRST
#    define ISFIRST
#    define SETFIRST
#    define CLEARFIRST
#  endif

// for compile-time range checks with n=3,10,1000,5000,10000,20000,100000
// #  define LOG2_APPROX(n) (size_t)((-0.344845 * (n) * (n)) + (2.024658 * (n))
//- 1.674873) #  define _IN_RANGE     (sizeof (_obj->field) >= LOG2_APPROX
//(maxvalue) / 8)
#  define _IN_RANGE(var, n)                                                   \
    ((sizeof (var) == 1 && n <= 0xff) || (sizeof (var) == 2 && n <= 0xffff)   \
     || (sizeof (var) >= 4))

#  ifndef IS_FREE
#    define VALUEOUTOFBOUNDS(field, maxvalue)                                 \
      if (_IN_RANGE (_obj->field, maxvalue) && _obj->field > maxvalue)        \
        {                                                                     \
          LOG_ERROR ("Invalid %s." #field " %lu", obj ? obj->name : "",       \
                     (unsigned long)_obj->field);                             \
          _obj->field = 0;                                                    \
          return DWG_ERR_VALUEOUTOFBOUNDS;                                    \
        }
#    define SUB_VALUEOUTOFBOUNDS(o, field, maxvalue)                          \
      if (_IN_RANGE (_obj->o.field, maxvalue) && _obj->o.field > maxvalue)    \
        {                                                                     \
          LOG_ERROR ("Invalid %s." #field " %lu", obj ? obj->name : "",       \
                     (unsigned long)_obj->o.field);                           \
          _obj->o.field = 0;                                                  \
          return DWG_ERR_VALUEOUTOFBOUNDS;                                    \
        }
#  else
#    define VALUEOUTOFBOUNDS(field, maxvalue)                                 \
      if (_IN_RANGE (_obj->field, maxvalue) && _obj->field > maxvalue)        \
        {                                                                     \
          return DWG_ERR_VALUEOUTOFBOUNDS;                                    \
        }
#    define SUB_VALUEOUTOFBOUNDS(o, field, maxvalue)                          \
      if (_IN_RANGE (_obj->o.field, maxvalue) && _obj->o.field > maxvalue)    \
        {                                                                     \
          return DWG_ERR_VALUEOUTOFBOUNDS;                                    \
        }
#  endif

#endif /* SPEC_H */

#define FIELD_VALUE(nam) _obj->nam
#define SUB_FIELD_VALUE(o, nam) _obj->o.nam

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
#  define VALUE_2BD(value, dxf) VALUE_2RD (value, dxf)
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
#ifndef VALUE_RSd
#  define VALUE_RSd(value, dxf)
#endif
#ifndef VALUE_BL
#  define VALUE_BL(value, dxf)
#endif
#ifndef VALUE_RLx
#  define VALUE_RLx(value, dxf) VALUE_RL (value, dxf)
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
#ifndef SUB_FIELD_RLLd
#  define SUB_FIELD_RLLd(o, nam, dxf) FIELD_RLLd (o.nam, dxf)
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
#ifndef SUB_FIELD_BLx
#  define SUB_FIELD_BLx(o, nam, dxf) FIELD_BLx (o.nam, dxf)
#endif
#ifndef SUB_FIELD_RC
#  define SUB_FIELD_RC(o, nam, dxf) SUB_FIELD (o, nam, RC, dxf)
#endif
#ifndef SUB_FIELD_BL
#  define SUB_FIELD_B(o, nam, dxf) FIELDG (o.nam, B, dxf)
#  define SUB_FIELD_BB(o, nam, dxf) FIELDG (o.nam, BB, dxf)
#  define SUB_FIELD_3B(o, nam, dxf) FIELDG (o.nam, 3B, dxf)
#  define SUB_FIELD_BS(o, nam, dxf) FIELDG (o.nam, BS, dxf)
#  define SUB_FIELD_BL(o, nam, dxf) FIELDG (o.nam, BL, dxf)
#  define SUB_FIELD_BLd(o, nam, dxf) FIELD_BLd (o.nam, dxf)
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
// # define SUB_FIELD_ENC(o,nam,dxf1,dxf2) FIELD_ENC(o.nam, dxf1,dxf2)
#endif

#ifndef SUB_HANDLE_VECTOR
#  define SUB_HANDLE_VECTOR(o, nam, sizefield, code, dxf)                     \
    if (_obj->o.sizefield && _obj->o.nam)                                     \
      {                                                                       \
        BITCODE_BL _size = _obj->o.sizefield;                                 \
        for (vcount = 0; vcount < _size; vcount++)                            \
          {                                                                   \
            SUB_FIELD_HANDLE (o, nam[vcount], code, dxf);                     \
          }                                                                   \
      }
#endif

#ifndef SUB_FIELD_VECTOR
#  define SUB_FIELD_VECTOR(o, nam, type, sizefield, dxf)                      \
    if (_obj->o.sizefield && _obj->o.nam)                                     \
      {                                                                       \
        BITCODE_BL _size = _obj->o.sizefield;                                 \
        for (vcount = 0; vcount < _size; vcount++)                            \
          {                                                                   \
            SUB_FIELD (o, nam[vcount], type, dxf);                            \
          }                                                                   \
      }
#endif
#ifndef SUB_FIELD_VECTOR_N
#  define SUB_FIELD_VECTOR_N(o, nam, type, size, dxf)                         \
    if (size > 0 && _obj->o.nam != NULL)                                      \
      {                                                                       \
        BITCODE_BL _size = (BITCODE_BL)size;                                  \
        for (vcount = 0; vcount < _size; vcount++)                            \
          {                                                                   \
            SUB_FIELD (o, nam[vcount], type, dxf);                            \
          }                                                                   \
      }
#endif
#ifndef FIELD_VECTOR_INL
#  define FIELD_VECTOR_INL(nam, type, size, dxf)                              \
    FIELD_VECTOR_N (nam, type, size, dxf)
#endif
#ifndef SUB_FIELD_VECTOR_INL
#  define SUB_FIELD_VECTOR_INL(o, nam, type, size, dxf)                       \
    SUB_FIELD_VECTOR_N (o, nam, type, size, dxf)
#endif
#ifndef SUB_FIELD_2RD_VECTOR
#  define SUB_FIELD_2RD_VECTOR(o, name, size, dxf)                            \
    if (_obj->o.size > 0)                                                     \
      {                                                                       \
        for (vcount = 0; vcount < (BITCODE_BL)_obj->o.size; vcount++)         \
          {                                                                   \
            SUB_FIELD_2RD (o, name[vcount], dxf);                             \
          }                                                                   \
      }
#endif
#ifndef SUB_FIELD_3BD_VECTOR
#  define SUB_FIELD_3BD_VECTOR(o, name, size, dxf)                            \
    if (_obj->o.size > 0)                                                     \
      {                                                                       \
        for (vcount = 0; vcount < (BITCODE_BL)_obj->o.size; vcount++)         \
          {                                                                   \
            SUB_FIELD_3BD (o, name[vcount], dxf);                             \
          }                                                                   \
      }
#endif
#ifndef FIELD_VECTOR_T1
#  define FIELD_VECTOR_T1(nam, type, size, dxf)                               \
    FIELD_VECTOR_T (nam, type, size, dxf)
#endif
#ifndef SUB_FIELD_VECTOR_TYPESIZE
#  define SUB_FIELD_VECTOR_TYPESIZE(o, nam, size, typesize, dxf)              \
    if (_obj->o.size && _obj->o.nam)                                          \
      {                                                                       \
        for (vcount = 0; vcount < (BITCODE_BL)_obj->o.size; vcount++)         \
          {                                                                   \
            switch (typesize)                                                 \
              {                                                               \
              case 0:                                                         \
                break;                                                        \
              case 1:                                                         \
                SUB_FIELD (o, nam[vcount], RC, dxf);                          \
                break;                                                        \
              case 2:                                                         \
                SUB_FIELD (o, nam[vcount], RS, dxf);                          \
                break;                                                        \
              case 4:                                                         \
                SUB_FIELD (o, nam[vcount], RL, dxf);                          \
                break;                                                        \
              case 8:                                                         \
                SUB_FIELD (o, nam[vcount], RLL, dxf);                         \
                break;                                                        \
              default:                                                        \
                LOG_ERROR ("Unknown SUB_FIELD_VECTOR_TYPE " #nam              \
                           " typesize %d",                                    \
                           typesize);                                         \
                break;                                                        \
              }                                                               \
          }                                                                   \
      }
#endif

#ifndef REPEAT_F
// not allocating versions checked
#  define _REPEAT_F(times, size, nam, type, idx)                              \
    if (_obj->times > (BITCODE_BL)size)                                       \
      {                                                                       \
        LOG_ERROR ("Invalid %s " FORMAT_BL " > %u", #nam,                     \
                   FIELD_VALUE (times), (unsigned)size);                      \
        FIELD_VALUE (times) = (BITCODE_BL)size;                               \
      }                                                                       \
    for (rcount##idx = 0; rcount##idx < (BITCODE_BL)_obj->times; rcount##idx++)
#  define REPEAT_F(times, size, nam, type)                                    \
    _REPEAT_F (times, size, nam, type, 1)
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
#  define FIELD_TFv(name, len, dxf) FIELD_TV (name, dxf)
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
#ifndef FIELD_RSd
#  define FIELD_RSd(name, dxf) FIELD_RS (name, dxf)
#endif
#ifndef FIELD_RLLd
#  define FIELD_RLLd(name, dxf) FIELD_RLL (name, dxf)
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
      if (_ver != dat->version)                                               \
        dat->version = _ver;                                                  \
    }
#endif
#ifndef SUB_FIELD_CMTC
#  define SUB_FIELD_CMTC(o, name, dxf)                                        \
    {                                                                         \
      Dwg_Version_Type _ver = dat->version;                                   \
      Dwg_Version_Type _fver = dat->from_version;                             \
      if (dat->version < R_2004)                                              \
        dat->version = R_2004;                                                \
      if (dat->from_version < R_2004)                                         \
        dat->from_version = R_2004;                                           \
      SUB_FIELD_CMC (o, name, dxf);                                           \
      if (_ver != dat->version)                                               \
        dat->version = _ver;                                                  \
      if (_fver != dat->from_version)                                         \
        dat->from_version = _fver;                                            \
    }
#endif
// on DXF skip if 0
#ifndef FIELD_BD0
#  define FIELD_2RD0(name, dxf) FIELD_2RD (name, dxf)
#  define FIELD_RD0(name, dxf) FIELD_RD (name, dxf)
#  define FIELD_RD1(name, dxf) FIELD_RD (name, dxf)
#  define FIELD_BD0(name, dxf) FIELD_BD (name, dxf)
#  define FIELD_BD1(name, dxf) FIELD_BD (name, dxf)
#  define FIELD_BL0(name, dxf) FIELD_BL (name, dxf)
#  define SUB_FIELD_BL0(o, name, dxf) SUB_FIELD_BL (o, name, dxf)
#  define FIELD_B0(name, dxf) FIELD_B (name, dxf)
#  define FIELD_B1(name, dxf) FIELD_B (name, dxf)
#  define FIELD_BS0(name, dxf) FIELD_BS (name, dxf)
#  define FIELD_BS1(name, dxf) FIELD_BS (name, dxf)
#  define FIELD_RC0(name, dxf) FIELD_RC (name, dxf)
#  define FIELD_RS0(name, dxf) FIELD_RS (name, dxf)
#  define FIELD_RL0(name, dxf) FIELD_RL (name, dxf)
#  define FIELD_BT0(name, dxf) FIELD_BT (name, dxf)
#  define VALUE_T0(name, dxf) VALUE_T (name, dxf)
#  define FIELD_TV0(name, dxf) FIELD_TV (name, dxf)
#  define FIELD_T0(name, dxf) FIELD_T (name, dxf)
#  define FIELD_CMC0(color, dxf) FIELD_CMC (color, dxf)
#  define FIELD_HANDLE0(name, code, dxf) FIELD_HANDLE (name, code, dxf)
#  define SUB_FIELD_HANDLE0(o, name, code, dxf)                               \
    SUB_FIELD_HANDLE (o, name, code, dxf)
#endif
#ifndef VALUE_TV0
#  define VALUE_TV0(name, dxf) VALUE_TV (name, dxf)
#endif

#ifndef FIELD_2RD_1
#  define FIELD_2RD_1(nam, dxf) FIELD_2RD (nam, dxf)
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
#  define SET_PARENT(field, to) _obj->field.parent = to
#  define SET_PARENT_OBJ(field) SET_PARENT (field, _obj)
#  define SET_PARENT_FIELD(field, what_parent, to) _obj->field.what_parent = to
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
#  undef DXF_OR_FREE
#  define DXF_OR_FREE if (1)
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
#  undef DXF_OR_FREE
#  define DXF_OR_FREE if (1)
#  undef IF_IS_FREE
#  define IF_IS_FREE 1
#  undef IF_FREE_OR_SINCE
#  define IF_FREE_OR_SINCE(x) if (1)
#  undef IF_FREE_OR_VERSIONS
#  define IF_FREE_OR_VERSIONS(x, y) if (1)
#else
#  ifndef END_REPEAT
#    define END_REPEAT(field)
#  endif
#endif

#ifndef END_REPEAT_F
#  define END_REPEAT_F(field)
#endif

#ifndef R11OPTS
#  define R11OPTS(b) (_ent->opts_r11 & (b))
#  define R11FLAG(b) (_ent->flag_r11 & (b))
#  define R11EXTRA(b) (_ent->extra_r11 & (b))
#endif

#if defined IS_JSON
#  define HANDLE_UNKNOWN_BITS                                                 \
    {                                                                         \
      unsigned num_bytes = obj->num_unknown_bits / 8;                         \
      if (obj->num_unknown_bits & 8)                                          \
        num_bytes++;                                                          \
      KEY (num_unknown_bits);                                                 \
      VALUE_RL (obj->num_unknown_bits, 0);                                    \
      KEY (unknown_bits);                                                     \
      VALUE_BINARY (obj->unknown_bits, num_bytes, 0);                         \
    }
#elif defined IS_DECODER
#  define HANDLE_UNKNOWN_BITS                                                 \
    dwg_decode_unknown_bits (dat, (Dwg_Object *restrict)obj)
#elif defined IS_ENCODER
#  define HANDLE_UNKNOWN_BITS                                                 \
    if (dwg_encode_unknown_bits (dat, (Dwg_Object *restrict)obj))             \
      {                                                                       \
        if (hdl_dat != dat && hdl_dat->chain != dat->chain)                   \
          bit_chain_free (hdl_dat);                                           \
        return error;                                                         \
      }
#elif defined IS_FREE
#  define HANDLE_UNKNOWN_BITS VALUE_TF (obj->unknown_bits, 0)
#elif defined IS_PRINT
#  define HANDLE_UNKNOWN_BITS                                                 \
    LOG_TRACE ("unknown_bits: %u [TF]\n", (unsigned)obj->num_unknown_bits);   \
    LOG_TRACE_TF (obj->unknown_bits, obj->num_unknown_bits)
#else
#  define HANDLE_UNKNOWN_BITS
#endif

#if defined IS_JSON
#  define UNKNOWN_BITS_REST                                                   \
    {                                                                         \
      unsigned num_bytes = obj->num_unknown_rest / 8;                         \
      if (obj->num_unknown_rest & 8)                                          \
        num_bytes++;                                                          \
      KEY (num_unknown_rest);                                                 \
      VALUE_RL (obj->num_unknown_rest, 0);                                    \
      KEY (unknown_bits);                                                     \
      VALUE_BINARY (obj->unknown_rest, num_bytes, 0);                         \
    }
#elif defined IS_DECODER
#  define UNKNOWN_BITS_REST                                                   \
    dwg_decode_unknown_rest (dat, (Dwg_Object *restrict)obj)
#elif defined IS_ENCODER
#  define UNKNOWN_BITS_REST                                                   \
    dwg_encode_unknown_rest (dat, (Dwg_Object *restrict)obj)
#elif defined IS_FREE
#  define UNKNOWN_BITS_REST VALUE_TF (obj->unknown_rest, 0)
#else
#  define UNKNOWN_BITS_REST
#endif

#ifndef START_OBJECT_HANDLE_STREAM
#  define START_OBJECT_HANDLE_STREAM                                          \
    START_HANDLE_STREAM;                                                      \
    assert (obj->supertype == DWG_SUPERTYPE_OBJECT)
#endif

#ifndef CONTROL_HANDLE_STREAM
#  define CONTROL_HANDLE_STREAM                                               \
    assert (obj->supertype == DWG_SUPERTYPE_OBJECT);                          \
    PRE (R_2007a)                                                             \
    {                                                                         \
      hdl_dat->byte = dat->byte;                                              \
      hdl_dat->bit = dat->bit;                                                \
    }                                                                         \
    SINCE (R_13b1)                                                            \
    {                                                                         \
      VALUE_HANDLE (obj->tio.object->ownerhandle, ownerhandle, 4, 0);         \
      REACTORS (4)                                                            \
      XDICOBJHANDLE (3)                                                       \
    }
#endif

#ifndef LOG_FLAG_W
#  define LOG_FLAG_W(token, w)                                                \
    if (_obj->flag & FLAG_##token##_##w)                                      \
    LOG_TRACE (#w " (%d) ", FLAG_##token##_##w)
#  define LOG_FLAG_TABLE_W(w)                                                 \
    if (_obj->flag & FLAG_TABLE_##w)                                          \
    LOG_TRACE (#w " (%d) ", FLAG_TABLE_##w)
#  define LOG_FLAG_TABLE_MAX(v)                                               \
    if (_obj->flag > v)                                                       \
    LOG_WARN ("Unknown flag (%d)", _obj->flag)
#  define LOG_FLAG_TABLE_COMMON                                               \
    if (_obj->flag)                                                           \
      {                                                                       \
        LOG_TRACE ("      ");                                                 \
        LOG_FLAG_TABLE_W (IS_XREF_REF);                                       \
        LOG_FLAG_TABLE_W (IS_XREF_RESOLVED);                                  \
        LOG_FLAG_TABLE_W (IS_XREF_DEP);                                       \
        LOG_FLAG_TABLE_W (IS_REMOVED);                                        \
        LOG_FLAG_TABLE_MAX (255);                                             \
        LOG_TRACE ("\n");                                                     \
      }
#  define LOG_FLAG_BLOCK_W(w)                                                 \
    if (_obj->flag & FLAG_BLOCK_##w)                                          \
    LOG_TRACE (#w " (%d) ", FLAG_BLOCK_##w)
#  define LOG_FLAG_Block                                                      \
    if (_obj->flag)                                                           \
      {                                                                       \
        LOG_TRACE ("      ");                                                 \
        LOG_FLAG_W (BLOCK, ANONYMOUS);                                        \
        LOG_FLAG_W (BLOCK, HAS_ATTRIBS);                                      \
        LOG_FLAG_W (BLOCK, IS_EXT_REF);                                       \
        LOG_FLAG_W (BLOCK, IS_XREF_OVERLAY);                                  \
        LOG_FLAG_TABLE_W (IS_XREF_REF);                                       \
        LOG_FLAG_TABLE_W (IS_XREF_RESOLVED);                                  \
        LOG_FLAG_TABLE_W (IS_XREF_DEP);                                       \
        LOG_FLAG_TABLE_W (IS_REMOVED);                                        \
        LOG_FLAG_TABLE_MAX (255);                                             \
        LOG_TRACE ("\n");                                                     \
      }
#  define LOG_FLAG_Layer                                                      \
    if (_obj->flag)                                                           \
      {                                                                       \
        LOG_TRACE ("      ");                                                 \
        LOG_FLAG_W (LAYER, FROZEN);                                           \
        LOG_FLAG_W (LAYER, FROZEN_IN_NEW);                                    \
        LOG_FLAG_W (LAYER, LOCKED);                                           \
        LOG_FLAG_W (LAYER, PLOTFLAG);                                         \
        LOG_FLAG_TABLE_W (IS_XREF_REF);                                       \
        LOG_FLAG_TABLE_W (IS_XREF_RESOLVED);                                  \
        LOG_FLAG_TABLE_W (IS_XREF_DEP);                                       \
        LOG_FLAG_TABLE_W (IS_REMOVED);                                        \
        LOG_FLAG_TABLE_MAX (255);                                             \
        LOG_TRACE ("\n");                                                     \
      }
#  define LOG_FLAG_TextStyle                                                  \
    if (_obj->flag)                                                           \
      {                                                                       \
        LOG_TRACE ("      ");                                                 \
        LOG_FLAG_W (STYLE, SHAPE);                                            \
        LOG_FLAG_W (STYLE, VERTICAL_TEXT);                                    \
        LOG_FLAG_TABLE_W (IS_XREF_REF);                                       \
        LOG_FLAG_TABLE_W (IS_XREF_RESOLVED);                                  \
        LOG_FLAG_TABLE_W (IS_XREF_DEP);                                       \
        LOG_FLAG_TABLE_W (IS_REMOVED);                                        \
        LOG_FLAG_TABLE_MAX (255);                                             \
        LOG_TRACE ("\n");                                                     \
      }
#  define LOG_FLAG_View                                                       \
    if (_obj->flag)                                                           \
      {                                                                       \
        LOG_TRACE ("      ");                                                 \
        LOG_FLAG_W (VIEW, PSPACE);                                            \
        LOG_FLAG_TABLE_W (IS_XREF_REF);                                       \
        LOG_FLAG_TABLE_W (IS_XREF_RESOLVED);                                  \
        LOG_FLAG_TABLE_W (IS_XREF_DEP);                                       \
        LOG_FLAG_TABLE_W (IS_REMOVED);                                        \
        LOG_FLAG_TABLE_MAX (255);                                             \
        LOG_TRACE ("\n");                                                     \
      }
#  define LOG_FLAG_VX                                                         \
    if (_obj->flag)                                                           \
      {                                                                       \
        LOG_TRACE ("      ");                                                 \
        LOG_FLAG_W (VX, IS_ON);                                               \
        LOG_FLAG_TABLE_W (IS_XREF_REF);                                       \
        LOG_FLAG_TABLE_W (IS_XREF_RESOLVED);                                  \
        LOG_FLAG_TABLE_W (IS_XREF_DEP);                                       \
        LOG_FLAG_TABLE_W (IS_REMOVED);                                        \
        LOG_FLAG_TABLE_MAX (255);                                             \
        LOG_TRACE ("\n");                                                     \
      }
#  define LOG_FLAG_Viewport LOG_FLAG_TABLE_COMMON
#  define LOG_FLAG_RegApp LOG_FLAG_TABLE_COMMON
#  define LOG_FLAG_DimStyle LOG_FLAG_TABLE_COMMON
#  define LOG_FLAG_Linetype LOG_FLAG_TABLE_COMMON
#  define LOG_FLAG_UCS LOG_FLAG_TABLE_COMMON
#endif

#ifndef COMMON_TABLE_FLAGS
#  define COMMON_TABLE_FLAGS(acdbname)                                            \
    assert (obj->supertype == DWG_SUPERTYPE_OBJECT);                              \
    PRE (R_13b1)                                                                  \
    {                                                                             \
      if (strcmp (#acdbname, "Layer") == 0)                                       \
        {                                                                         \
          FIELD_CAST (flag, RC, RS, 70);                                          \
        }                                                                         \
      else                                                                        \
        {                                                                         \
          FIELD_CAST (flag, RC, RC, 70);                                          \
        }                                                                         \
      /* clang-format off */                                                      \
      DECODER_OR_ENCODER                                                          \
        {                                                                         \
          LOG_FLAG_##acdbname                                                     \
        }                                                                         \
      FIELD_TFv (name, 32, 2);                                                    \
      VERSION (R_11)                                                              \
        FIELD_RSd (used, 0);                                                      \
      /* clang-format on */                                                       \
    }                                                                             \
    LATER_VERSIONS                                                                \
    {                                                                             \
      FIELD_T (name, 2);                                                          \
      UNTIL (R_2004)                                                              \
      {                                                                           \
        FIELD_B (is_xref_ref, 0);       /* always 1, 70 bit 6 */                  \
        FIELD_BS (is_xref_resolved, 0); /* 0 or 256 */                            \
        FIELD_B (is_xref_dep, 0);       /* 70 bit 4 */                            \
      }                                                                           \
      LATER_VERSIONS                                                              \
      {                                                                           \
        FIELD_VALUE (is_xref_ref) = 1;                                            \
        FIELD_BS (is_xref_resolved, 0); /* 0 or 256 */                            \
        if (FIELD_VALUE (is_xref_resolved) == 256)                                \
          FIELD_VALUE (is_xref_dep) = 1;                                          \
      }                                                                           \
      FIELD_HANDLE (xref, 5, 0); /* NULLHDL without is_xref_dep */                \
      FIELD_VALUE (flag)                                                          \
          |= FIELD_VALUE (is_xref_dep) << 4 | FIELD_VALUE (is_xref_ref) << 6;     \
      DECODER_OR_ENCODER                                                          \
        {                                                                         \
           LOG_TRACE ("=> flag %u [BL 70]\n", FIELD_VALUE (flag));                \
        }                                                                         \
    }                                                                             \
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
  static int DWG_IMPL_N (ACTION, parenttype##_##subtype)                      \
    (Bit_Chain *dat,                                                          \
    Bit_Chain *hdl_dat,                                                       \
    Bit_Chain *str_dat,                                                       \
    Dwg_Object *restrict obj,                                                 \
    Dwg_Object_##parenttype *restrict _obj,                                   \

#define DWG_ENT_SUBCLASS_DECL(parenttype, subtype)                            \
  static int DWG_COMMON_N (ACTION, parenttype##_##subtype) (                  \
      Dwg_Entity_##parenttype *restrict _obj, Bit_Chain * dat,                \
      Bit_Chain * hdl_dat, Bit_Chain * str_dat, Dwg_Object *restrict obj)

#define DWG_SUBCLASS(parenttype, subtype)                                     \
  static int DWG_IMPL_N (ACTION, parenttype##_##subtype)                      \
    (Bit_Chain *dat,                                                          \
     Bit_Chain *hdl_dat,                                                      \
     Bit_Chain *str_dat,                                                      \
     Dwg_Object *restrict obj,                                                \
     Dwg_Object_##parenttype *restrict _obj)                                  \
  {                                                                           \
    BITCODE_BL vcount, rcount3, rcount4;                                      \
    Dwg_Data *dwg = obj->parent;                                              \
    int error = 0;                                                            \
    subtype##_fields;                                                         \
    return error;                                                             \
  }

#define DWG_ENT_SUBCLASS(parenttype, subtype)                                 \
  static int DWG_COMMON_N (ACTION, parenttype##_##subtype) (                  \
      Dwg_Entity_##parenttype *restrict _obj, Bit_Chain * dat,                \
      Bit_Chain * hdl_dat, Bit_Chain * str_dat, Dwg_Object *restrict obj)     \
  {                                                                           \
    BITCODE_BL vcount, rcount3, rcount4;                                      \
    Dwg_Data *dwg = obj->parent;                                              \
    int error = 0;                                                            \
    subtype##_fields;                                                         \
    return error;                                                             \
  }

#define CALL_SUBCLASS(_xobj, parenttype, subtype)                             \
  error |= DWG_COMMON_N (ACTION, parenttype##_##subtype) (                    \
// call the common and subclass
#define CALL_COMMON(_xobj, parenttype, subtype)                               \
  error |= DWG_COMMON_N (ACTION, parenttype##_##subtype) (                    \
      _xobj, dat, hdl_dat, str_dat, (Dwg_Object *)obj)
// if the name is compile-time known
#define CALL_ENTITY(name, xobj)                                               \
  error |= DWG_IMPL_N (ACTION, name) (dat, hdl_dat, str_dat,                  \
                                         (Dwg_Object *)xobj)
// TODO: dispatch on the type
// if the type is compile-time known, call the impl subclass directly
#define CALL_SUBENT_TYPE(nam, xobj)                                           \
  {                                                                           \
    if ((Dwg_Object *)xobj->fixedtype == DWG_TYPE_##nam)                      \
      error |= DWG_IMPL_N (ACTION, nam) (dat, hdl_dat, str_dat,               \
                                         (Dwg_Object *)xobj);                 \
  }
// dispatch dynamically on the type
#define CALL_SUBENT(hdl, dxf)
{
  Dwg_Object *o = dwg_ref_object (dwg, hdl);
  if (o)
    error |= DWG_IMPL_N (ACTION, nam) (dat, hdl_dat, str_dat, o);
}

#ifndef UNKNOWN_UNTIL
#  define UNKNOWN_UNTIL(pos)                                                  \
    LOG_TRACE ("unknown (%ld): ", pos - dat->byte);                           \
    dat->byte = pos
#endif

#define LOG_FLAG_MAX(value, w)                                                \
  if (value > w)                                                              \
  LOG_WARN ("Unknown flag (0x%x)", value & ~(w))

#ifndef LOG_LAYER_FLAG
#  define LOG_LAYER_FLAG(w)                                                   \
    DECODER_OR_ENCODER                                                        \
    {                                                                         \
      if (_obj->w)                                                            \
        LOG_TRACE ("       %s: 0x%x\n", #w, _obj->w);                         \
    }
#  define LOG_LAYER_FLAG_REV(w)                                               \
    DECODER_OR_ENCODER                                                        \
    {                                                                         \
      if (!_obj->w)                                                           \
        LOG_TRACE ("       %s: 0x%x\n", #w, _obj->w);                         \
    }
#endif
#ifndef LOG_TEXT_GENERATION
#  define LOG_TEXT_GENERATION_W(w)                                            \
    if (_obj->generation & TEXT_GENERATION_##w)                               \
    LOG_TRACE (#w "(0x%x) ", TEXT_GENERATION_##w)
#  define LOG_TEXT_GENERATION                                                 \
    DECODER_OR_ENCODER                                                        \
    {                                                                         \
      if (_obj->generation)                                                   \
        {                                                                     \
          LOG_TRACE ("            ");                                         \
          LOG_TEXT_GENERATION_W (BACKWARDS);                                  \
          LOG_TEXT_GENERATION_W (UPSIDE_DOWN);                                \
          LOG_FLAG_MAX (_obj->generation, 7);                                 \
          LOG_TRACE ("\n");                                                   \
        }                                                                     \
    }
#endif

#ifndef LOG_LTYPE_SHAPE_FLAG
#  define LOG_LTYPE_SHAPE_FLAG_W(w)                                           \
    if (_obj->dashes[rcount1].shape_flag & LTYPE_SHAPE_FLAG_##w)              \
    LOG_TRACE (#w "(0x%x) ", LTYPE_SHAPE_FLAG_##w)
#  define LOG_LTYPE_SHAPE_FLAG                                                \
    DECODER_OR_ENCODER                                                        \
    {                                                                         \
      if (_obj->dashes[rcount1].shape_flag)                                   \
        {                                                                     \
          LOG_TRACE ("                      ");                               \
          LOG_LTYPE_SHAPE_FLAG_W (ABS_ROTATION);                              \
          LOG_LTYPE_SHAPE_FLAG_W (IS_TEXT);                                   \
          LOG_LTYPE_SHAPE_FLAG_W (IS_SHAPE);                                  \
          LOG_FLAG_MAX (_obj->dashes[rcount1].shape_flag, 5);                 \
          LOG_TRACE ("\n");                                                   \
        }                                                                     \
    }
#endif

#ifndef LOG_HORIZ_ALIGNMENT
#  define LOG_HORIZ_ALIGNMENT_W(w)                                            \
    if (_obj->horiz_alignment == HORIZ_ALIGNMENT_##w)                         \
    LOG_TRACE (#w "(0x%x) ", HORIZ_ALIGNMENT_##w)
#  define LOG_HORIZ_ALIGNMENT                                                 \
    DECODER_OR_ENCODER                                                        \
    {                                                                         \
      if (_obj->horiz_alignment)                                              \
        {                                                                     \
          LOG_TRACE ("                 ");                                    \
          LOG_HORIZ_ALIGNMENT_W (LEFT);                                       \
          LOG_HORIZ_ALIGNMENT_W (CENTER);                                     \
          LOG_HORIZ_ALIGNMENT_W (RIGHT);                                      \
          LOG_HORIZ_ALIGNMENT_W (ALIGNED);                                    \
          LOG_HORIZ_ALIGNMENT_W (MIDDLE);                                     \
          LOG_HORIZ_ALIGNMENT_W (FIT);                                        \
          LOG_FLAG_MAX (_obj->horiz_alignment, HORIZ_ALIGNMENT_FIT);          \
          LOG_TRACE ("\n");                                                   \
        }                                                                     \
    }
#endif

#ifndef LOG_VERT_ALIGNMENT
#  define LOG_VERT_ALIGNMENT_W(w)                                             \
    if (_obj->vert_alignment == VERT_ALIGNMENT_##w)                           \
    LOG_TRACE (#w "(0x%x) ", VERT_ALIGNMENT_##w)
#  define LOG_VERT_ALIGNMENT                                                  \
    DECODER_OR_ENCODER                                                        \
    {                                                                         \
      if (_obj->vert_alignment)                                               \
        {                                                                     \
          LOG_TRACE ("                ");                                     \
          LOG_VERT_ALIGNMENT_W (BASELINE);                                    \
          LOG_VERT_ALIGNMENT_W (BOTTOM);                                      \
          LOG_VERT_ALIGNMENT_W (MIDDLE);                                      \
          LOG_VERT_ALIGNMENT_W (TOP);                                         \
          LOG_FLAG_MAX (_obj->vert_alignment, VERT_ALIGNMENT_TOP);            \
          LOG_TRACE ("\n");                                                   \
        }                                                                     \
    }
#endif

#ifndef LOG_FLAG_ATTDEF
#  define LOG_FLAG_ATTDEF_W(w)                                                \
    if (_obj->flags & FLAG_ATTDEF_##w)                                        \
    LOG_TRACE (#w "(0x%x) ", FLAG_ATTDEF_##w)
#  define LOG_FLAG_ATTDEF                                                     \
    DECODER_OR_ENCODER                                                        \
    {                                                                         \
      if (_obj->flags)                                                        \
        {                                                                     \
          LOG_TRACE ("       ");                                              \
          LOG_FLAG_ATTDEF_W (INVISIBLE);                                      \
          LOG_FLAG_ATTDEF_W (CONSTANT);                                       \
          LOG_FLAG_ATTDEF_W (VERIFY);                                         \
          LOG_FLAG_ATTDEF_W (PRESET);                                         \
          LOG_FLAG_MAX (_obj->flags, 15);                                     \
          LOG_TRACE ("\n");                                                   \
        }                                                                     \
    }
#endif

#ifndef LOG_FLAG_ATTRIB
#  define LOG_FLAG_ATTRIB_W(w)                                                \
    if (_obj->flags & FLAG_ATTRIB_##w)                                        \
    LOG_TRACE (#w "(0x%x) ", FLAG_ATTRIB_##w)
#  define LOG_FLAG_ATTRIB                                                     \
    DECODER_OR_ENCODER                                                        \
    {                                                                         \
      if (_obj->flags)                                                        \
        {                                                                     \
          LOG_TRACE ("       ");                                              \
          LOG_FLAG_ATTRIB_W (INVISIBLE);                                      \
          LOG_FLAG_ATTRIB_W (CONSTANT);                                       \
          LOG_FLAG_ATTRIB_W (VERIFY);                                         \
          LOG_FLAG_ATTRIB_W (PRESET);                                         \
          LOG_FLAG_MAX (_obj->flags, 15);                                     \
          LOG_TRACE ("\n");                                                   \
        }                                                                     \
    }
#endif

#ifndef LOG_FLAG_LWPOLYLINE
#  define LOG_FLAG_LWPOLYLINE_W(w)                                            \
    if (_obj->flag & FLAG_LWPOLYLINE_##w)                                     \
    LOG_TRACE (#w "(0x%x) ", FLAG_LWPOLYLINE_##w)
#  define LOG_FLAG_LWPOLYLINE                                                 \
    DECODER_OR_ENCODER                                                        \
    {                                                                         \
      if (_obj->flag)                                                         \
        {                                                                     \
          LOG_TRACE ("      ");                                               \
          LOG_FLAG_LWPOLYLINE_W (HAS_EXTRUSION);                              \
          LOG_FLAG_LWPOLYLINE_W (HAS_THICKNESS);                              \
          LOG_FLAG_LWPOLYLINE_W (HAS_CONSTWIDTH);                             \
          LOG_FLAG_LWPOLYLINE_W (HAS_ELEVATION);                              \
          LOG_FLAG_LWPOLYLINE_W (HAS_NUM_BULGES);                             \
          LOG_FLAG_LWPOLYLINE_W (HAS_NUM_WIDTHS);                             \
          LOG_FLAG_LWPOLYLINE_W (UNKNOWN_64);                                 \
          LOG_FLAG_LWPOLYLINE_W (UNKNOWN_128);                                \
          LOG_FLAG_LWPOLYLINE_W (PLINEGEN);                                   \
          LOG_FLAG_LWPOLYLINE_W (CLOSED);                                     \
          LOG_FLAG_LWPOLYLINE_W (VERTEXIDCOUNT);                              \
          LOG_FLAG_MAX (_obj->flag, 2047);                                    \
          LOG_TRACE ("\n");                                                   \
        }                                                                     \
    }
#endif

#ifndef LOG_FLAG_POLYLINE
#  define LOG_FLAG_POLYLINE_W(w)                                              \
    if (_obj->flag & FLAG_POLYLINE_##w)                                       \
    LOG_TRACE (#w "(0x%x) ", FLAG_POLYLINE_##w)
#  define LOG_FLAG_POLYLINE                                                   \
    DECODER_OR_ENCODER                                                        \
    {                                                                         \
      if (_obj->flag)                                                         \
        {                                                                     \
          LOG_TRACE ("      ");                                               \
          LOG_FLAG_POLYLINE_W (CLOSED);                                       \
          LOG_FLAG_POLYLINE_W (CURVE_FIT);                                    \
          LOG_FLAG_POLYLINE_W (SPLINE_FIT);                                   \
          LOG_FLAG_POLYLINE_W (3D);                                           \
          LOG_FLAG_POLYLINE_W (MESH);                                         \
          LOG_FLAG_POLYLINE_W (MESH_CLOSED);                                  \
          LOG_FLAG_POLYLINE_W (PFACE_MESH);                                   \
          LOG_FLAG_POLYLINE_W (LT_PATTERN_CONTINUES);                         \
          LOG_FLAG_MAX (_obj->flag, 255);                                     \
          LOG_TRACE ("\n");                                                   \
        }                                                                     \
    }
#endif

#ifndef LOG_FLAG_VERTEX
#  define LOG_FLAG_VERTEX_W(w)                                                \
    if (_obj->flag & FLAG_VERTEX_##w)                                         \
    LOG_TRACE (#w "(0x%x) ", FLAG_VERTEX_##w)
#  define LOG_FLAG_VERTEX                                                     \
    DECODER_OR_ENCODER                                                        \
    {                                                                         \
      if (_obj->flag)                                                         \
        {                                                                     \
          LOG_TRACE ("      ");                                               \
          LOG_FLAG_VERTEX_W (EXTRA_VERTEX);                                   \
          LOG_FLAG_VERTEX_W (CURVE_FIT);                                      \
          LOG_FLAG_VERTEX_W (UNUSED_4);                                       \
          LOG_FLAG_VERTEX_W (SPLINE_FIT);                                     \
          LOG_FLAG_VERTEX_W (SPLINE_FRAME_CONTROL_POINT);                     \
          LOG_FLAG_VERTEX_W (3D);                                             \
          LOG_FLAG_VERTEX_W (MESH);                                           \
          LOG_FLAG_VERTEX_W (PFACE_MESH);                                     \
          LOG_FLAG_MAX (_obj->flag, 255);                                     \
          LOG_TRACE ("\n");                                                   \
        }                                                                     \
    }
#endif

#ifndef LOG_POLYLINE_CURVETYPE
#  define LOG_POLYLINE_CURVETYPE_W(w)                                         \
    if (_obj->curve_type == POLYLINE_CURVETYPE_##w)                           \
    LOG_TRACE (#w "(0x%x) ", POLYLINE_CURVETYPE_##w)
#  define LOG_POLYLINE_CURVETYPE                                              \
    DECODER_OR_ENCODER                                                        \
    {                                                                         \
      if (_obj->curve_type)                                                   \
        {                                                                     \
          LOG_TRACE ("            ");                                         \
          LOG_POLYLINE_CURVETYPE_W (DEFAULT);                                 \
          LOG_POLYLINE_CURVETYPE_W (QUADR_BSPLINE);                           \
          LOG_POLYLINE_CURVETYPE_W (CUBIC_BSPLINE);                           \
          LOG_POLYLINE_CURVETYPE_W (BEZIER_SURFACE);                          \
          LOG_FLAG_MAX (_obj->curve_type, 8);                                 \
          LOG_TRACE ("\n");                                                   \
        }                                                                     \
    }
#endif

#ifndef LOG_LEADER_PATHTYPE
#  define LOG_LEADER_PATHTYPE_W(w)                                            \
    if (_obj->path_type == LEADER_PATHTYPE_##w)                               \
    LOG_TRACE (#w "(0x%x) ", LEADER_PATHTYPE_##w)
#  define LOG_LEADER_PATHTYPE                                                 \
    DECODER_OR_ENCODER                                                        \
    {                                                                         \
      if (_obj->path_type)                                                    \
        {                                                                     \
          LOG_TRACE ("           ");                                          \
          LOG_LEADER_PATHTYPE_W (STRAIGHT);                                   \
          LOG_LEADER_PATHTYPE_W (SPLINE);                                     \
          LOG_FLAG_MAX (_obj->path_type, 3);                                  \
          LOG_TRACE ("\n");                                                   \
        }                                                                     \
    }
#endif

#ifndef LOG_LEADER_ANNOTTYPE
#  define LOG_LEADER_ANNOTTYPE_W(w)                                           \
    if (_obj->annot_type == LEADER_ANNOTTYPE_##w)                             \
    LOG_TRACE (#w "(0x%x) ", LEADER_ANNOTTYPE_##w)
#  define LOG_LEADER_ANNOTTYPE                                                \
    DECODER_OR_ENCODER                                                        \
    {                                                                         \
      if (_obj->annot_type)                                                   \
        {                                                                     \
          LOG_TRACE ("            ");                                         \
          LOG_LEADER_ANNOTTYPE_W (MTEXT);                                     \
          LOG_LEADER_ANNOTTYPE_W (TOLERANCE);                                 \
          LOG_LEADER_ANNOTTYPE_W (INSERT);                                    \
          LOG_LEADER_ANNOTTYPE_W (NO_ANNOT);                                  \
          LOG_FLAG_MAX (_obj->annot_type, 15);                                \
          LOG_TRACE ("\n");                                                   \
        }                                                                     \
    }
#endif

#ifndef LOG_MLINE_FLAGS
#  define LOG_MLINE_FLAGS_W(w)                                                \
    if (_obj->flags & MLINE_FLAGS_##w)                                        \
    LOG_TRACE (#w "(0x%x) ", MLINE_FLAGS_##w)
#  define LOG_MLINE_FLAGS                                                     \
    DECODER_OR_ENCODER                                                        \
    {                                                                         \
      if (_obj->flags)                                                        \
        {                                                                     \
          LOG_TRACE ("     ");                                                \
          LOG_MLINE_FLAGS_W (HAS_VERTEX);                                     \
          LOG_MLINE_FLAGS_W (CLOSED);                                         \
          LOG_MLINE_FLAGS_W (SUPPRESS_START_CAPS);                            \
          LOG_MLINE_FLAGS_W (SUPPRESS_END_CAPS);                              \
          LOG_FLAG_MAX (_obj->flags, 15);                                     \
          LOG_TRACE ("\n");                                                   \
        }                                                                     \
    }
#endif

#ifndef LOG_MLINESTYLE_FLAG
#  define LOG_MLINESTYLE_FLAG_W(w)                                            \
    if (_obj->flag & MLINESTYLE_FLAG_##w)                                     \
    LOG_TRACE (#w "(0x%x) ", MLINESTYLE_FLAG_##w)
#  define LOG_MLINESTYLE_FLAG                                                 \
    DECODER_OR_ENCODER                                                        \
    {                                                                         \
      if (_obj->flag)                                                         \
        {                                                                     \
          LOG_TRACE ("      ");                                               \
          LOG_MLINESTYLE_FLAG_W (FILL);                                       \
          LOG_MLINESTYLE_FLAG_W (MITERS);                                     \
          LOG_MLINESTYLE_FLAG_W (UNKNOWN_4);                                  \
          LOG_MLINESTYLE_FLAG_W (UNKNOWN_8);                                  \
          LOG_MLINESTYLE_FLAG_W (START_SQUARE_END);                           \
          LOG_MLINESTYLE_FLAG_W (START_INNER_ARC);                            \
          LOG_MLINESTYLE_FLAG_W (START_ROUND);                                \
          LOG_MLINESTYLE_FLAG_W (UNKNOWN_128);                                \
          LOG_MLINESTYLE_FLAG_W (END_SQUARE);                                 \
          LOG_MLINESTYLE_FLAG_W (END_INNER_ARC);                              \
          LOG_MLINESTYLE_FLAG_W (END_ROUND);                                  \
          LOG_FLAG_MAX (_obj->flag, 2047);                                    \
          LOG_TRACE ("\n");                                                   \
        }                                                                     \
    }
#endif

#ifndef LOG_MLINE_JUSTIFICATION
#  define LOG_MLINE_JUSTIFICATION_W(w)                                        \
    if (_obj->justification == MLINE_JUSTIFICATION_##w)                       \
    LOG_TRACE (#w "(0x%x) ", MLINE_JUSTIFICATION_##w)
#  define LOG_MLINE_JUSTIFICATION                                             \
    DECODER_OR_ENCODER                                                        \
    {                                                                         \
      if (_obj->justification)                                                \
        {                                                                     \
          LOG_TRACE ("             ");                                        \
          LOG_MLINE_JUSTIFICATION_W (TOP);                                    \
          LOG_MLINE_JUSTIFICATION_W (MIDDLE);                                 \
          LOG_MLINE_JUSTIFICATION_W (BOTTOM);                                 \
          LOG_FLAG_MAX (_obj->justification, 3);                              \
          LOG_TRACE ("\n");                                                   \
        }                                                                     \
    }
#endif

#ifndef LOG_SPLINE_SCENARIO
#  define LOG_SPLINE_SCENARIO_W(w)                                            \
    if (_obj->scenario == SPLINE_SCENARIO_##w)                                \
    LOG_TRACE (#w "(0x%x) ", SPLINE_SCENARIO_##w)
#  define LOG_SPLINE_SCENARIO                                                 \
    DECODER_OR_ENCODER                                                        \
    {                                                                         \
      if (_obj->scenario)                                                     \
        {                                                                     \
          LOG_TRACE ("          ");                                           \
          LOG_SPLINE_SCENARIO_W (SPLINE);                                     \
          LOG_SPLINE_SCENARIO_W (BEZIER);                                     \
          LOG_FLAG_MAX (_obj->scenario, 2);                                   \
          LOG_TRACE ("\n");                                                   \
        }                                                                     \
    }
#endif
#ifndef LOG_SPLINE_SPLINEFLAGS
#  define LOG_SPLINE_SPLINEFLAGS_W(w)                                         \
    if (_obj->splineflags & SPLINE_SPLINEFLAGS_##w)                           \
    LOG_TRACE (#w "(0x%x) ", SPLINE_SPLINEFLAGS_##w)
#  define LOG_SPLINE_SPLINEFLAGS                                              \
    DECODER_OR_ENCODER                                                        \
    {                                                                         \
      if (_obj->splineflags)                                                  \
        {                                                                     \
          LOG_TRACE ("            ");                                         \
          LOG_SPLINE_SPLINEFLAGS_W (METHOD_FIT_POINTS);                       \
          LOG_SPLINE_SPLINEFLAGS_W (CV_FRAME_SHOW);                           \
          LOG_SPLINE_SPLINEFLAGS_W (CLOSED);                                  \
          LOG_FLAG_MAX (_obj->splineflags, 7);                                \
          LOG_TRACE ("\n");                                                   \
        }                                                                     \
    }
#endif
#ifndef LOG_SPLINE_KNOTPARAM
#  define LOG_SPLINE_KNOTPARAM_W(w)                                           \
    if (_obj->knotparam == SPLINE_KNOTPARAM_##w)                              \
    LOG_TRACE (#w "(0x%x) ", SPLINE_KNOTPARAM_##w)
#  define LOG_SPLINE_KNOTPARAM                                                \
    DECODER_OR_ENCODER                                                        \
    {                                                                         \
      if (_obj->knotparam)                                                    \
        {                                                                     \
          LOG_TRACE ("          ");                                           \
          LOG_SPLINE_KNOTPARAM_W (CHORD);                                     \
          LOG_SPLINE_KNOTPARAM_W (SQUARE_ROOT);                               \
          LOG_SPLINE_KNOTPARAM_W (UNIFORM);                                   \
          LOG_SPLINE_KNOTPARAM_W (CUSTOM);                                    \
          LOG_FLAG_MAX (_obj->knotparam, 15);                                 \
          LOG_TRACE ("\n");                                                   \
        }                                                                     \
    }
#endif

#ifndef LOG_LIGHT_TYPE
#  define LOG_LIGHT_TYPE_W(w)                                                 \
    if (_obj->type == LIGHT_TYPE_##w)                                         \
    LOG_TRACE (#w "(0x%x) ", LIGHT_TYPE_##w)
#  define LOG_LIGHT_TYPE                                                      \
    DECODER_OR_ENCODER                                                        \
    {                                                                         \
      if (_obj->type)                                                         \
        {                                                                     \
          LOG_TRACE ("      ");                                               \
          LOG_LIGHT_TYPE_W (DISTANT);                                         \
          LOG_LIGHT_TYPE_W (POINT);                                           \
          LOG_LIGHT_TYPE_W (SPOT);                                            \
          LOG_FLAG_MAX (_obj->type, 3);                                       \
          LOG_TRACE ("\n");                                                   \
        }                                                                     \
    }
#endif

#ifndef LOG_LIGHT_ATTENUATION_TYPE
#  define LOG_LIGHT_ATTENUATION_W(w)                                          \
    if (_obj->attenuation_type == LIGHT_ATTENUATION_TYPE_##w)                 \
    LOG_TRACE (#w "(0x%x) ", LIGHT_ATTENUATION_TYPE_##w)
#  define LOG_LIGHT_ATTENUATION_TYPE                                          \
    DECODER_OR_ENCODER                                                        \
    {                                                                         \
      LOG_TRACE ("      ");                                                   \
      LOG_LIGHT_ATTENUATION_W (NONE);                                         \
      LOG_LIGHT_ATTENUATION_W (INV_LINEAR);                                   \
      LOG_LIGHT_ATTENUATION_W (INV_SQUARE);                                   \
      LOG_FLAG_MAX (_obj->attenuation_type, 2);                               \
      LOG_TRACE ("\n");                                                       \
    }
#endif

#ifndef LOG_LIGHT_EXTLIGHT_SHAPE
#  define LOG_EXTLIGHT_SHAPE_W(w)                                             \
    if (_obj->extlight_shape == LIGHT_EXTLIGHT_SHAPE_##w)                     \
    LOG_TRACE (#w "(0x%x) ", LIGHT_EXTLIGHT_SHAPE_##w)
#  define LOG_LIGHT_EXTLIGHT_SHAPE                                            \
    DECODER_OR_ENCODER                                                        \
    {                                                                         \
      LOG_TRACE ("      ");                                                   \
      LOG_EXTLIGHT_SHAPE_W (LINEAR);                                          \
      LOG_EXTLIGHT_SHAPE_W (RECT);                                            \
      LOG_EXTLIGHT_SHAPE_W (DISK);                                            \
      LOG_EXTLIGHT_SHAPE_W (CYLINDER);                                        \
      LOG_EXTLIGHT_SHAPE_W (SPHERE);                                          \
      LOG_FLAG_MAX (_obj->extlight_shape, 4);                                 \
      LOG_TRACE ("\n");                                                       \
    }
#endif

#ifndef LOG_3DFACE_INVISIBLE
#  define LOG_INVISIBLE_W(w)                                                  \
    if (_obj->invis_flags & _3DFACE_INVISIBLE_##w)                            \
    LOG_TRACE (#w "(0x%x) ", _3DFACE_INVISIBLE_##w)
#  define LOG_3DFACE_INVISIBLE                                                \
    DECODER_OR_ENCODER                                                        \
    {                                                                         \
      if (_obj->invis_flags)                                                  \
        {                                                                     \
          LOG_TRACE ("             ");                                        \
          LOG_INVISIBLE_W (EDGE1);                                            \
          LOG_INVISIBLE_W (EDGE2);                                            \
          LOG_INVISIBLE_W (EDGE3);                                            \
          LOG_INVISIBLE_W (EDGE4);                                            \
          LOG_FLAG_MAX (_obj->invis_flags, 15);                               \
          LOG_TRACE ("\n");                                                   \
        }                                                                     \
    }
#endif

#ifdef IS_DECODER
#  define PRER13_SECTION_HDR(name)                                            \
    if (decode_preR13_section_hdr (#name, SECTION_##name, dat, dwg))          \
    return DWG_ERR_SECTIONNOTFOUND
#elif defined IS_ENCODER
#  define PRER13_SECTION_HDR(name)                                            \
    encode_preR13_section_hdr (#name, SECTION_##name, dat, dwg)
#  define DWG_TABLE(token) DWG_OBJECT (token)
#else
#  define PRER13_SECTION_HDR(name)
#  define DWG_TABLE(token) DWG_OBJECT (token)
#endif

#ifndef _DEBUG_HERE
#  define _DEBUG_HERE(x)
#endif
