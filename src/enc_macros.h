#define ANYCODE -1

// need to define before spec.h already
#undef IF_ENCODE_FROM_EARLIER
#undef IF_ENCODE_FROM_EARLIER_OR_DXF
#undef IF_ENCODE_FROM_PRE_R13
#undef IF_ENCODE_FROM_PRE_2000
#undef IF_ENCODE_SINCE_R13
#define IF_ENCODE_FROM_EARLIER                                                \
  if (dat->from_version && dat->from_version < cur_ver)
#define IF_ENCODE_FROM_EARLIER_OR_DXF                                         \
  if ((dat->from_version && dat->from_version < cur_ver)                      \
      || dwg->opts & DWG_OPTS_INDXF)
#define IF_ENCODE_FROM_PRE_R13                                                \
  if (dat->from_version && dat->from_version < R_13b1)
#define IF_ENCODE_FROM_PRE_2000                                               \
  if (dat->from_version && dat->from_version < R_2000)
#define IF_ENCODE_SINCE_R13                                                   \
  if (dat->from_version && dat->from_version >= R_13b1)

#undef LOG_POS
#define LOG_POS                                                               \
  LOG_INSANE (" @%" PRIuSIZE ".%u",                                           \
              obj ? dat->byte - obj->address : dat->byte, dat->bit);          \
  LOG_TRACE ("\n");
#define LOG_RPOS                                                              \
  LOG_INSANE (" @%" PRIuSIZE ".%u", dat->byte, dat->bit);                     \
  LOG_TRACE ("\n");
#define LOG_HPOS                                                              \
  LOG_INSANE (" @%" PRIuSIZE ".%u",                                           \
              obj && hdl_dat->byte > obj->address                             \
                  ? hdl_dat->byte - obj->address                              \
                  : hdl_dat->byte,                                            \
              hdl_dat->bit);                                                  \
  LOG_TRACE ("\n");

#define VALUE(value, type, dxf)                                               \
  {                                                                           \
    bit_write_##type (dat, value);                                            \
    LOG_TRACE (FORMAT_##type " [" #type " %d]", (BITCODE_##type)value, dxf);  \
    LOG_POS                                                                   \
  }
#define VALUE_RC(value, dxf) VALUE (value, RC, dxf)
#define VALUE_RS(value, dxf) VALUE (value, RS, dxf)
#define VALUE_RL(value, dxf) VALUE (value, RL, dxf)
#define VALUE_RLx(value, dxf) VALUE (value, RL, dxf)
#define VALUE_BS(value, dxf) VALUE (value, BS, dxf)
#define VALUE_BL(value, dxf) VALUE (value, BL, dxf)
#define VALUE_RD(value, dxf) VALUE (value, RD, dxf)
#define VALUE_BD(value, dxf) VALUE (value, BD, dxf)

#define FIELD(nam, type)                                                      \
  {                                                                           \
    bit_write_##type (dat, _obj->nam);                                        \
    FIELD_TRACE (nam, type);                                                  \
  }
#define FIELDG(nam, type, dxf)                                                \
  {                                                                           \
    bit_write_##type (dat, _obj->nam);                                        \
    FIELD_G_TRACE (nam, type, dxf);                                           \
  }
#define FIELD_TRACE(nam, type)                                                \
  LOG_TRACE (#nam ": " FORMAT_##type, _obj->nam);                             \
  LOG_POS
#define FIELD_G_TRACE(nam, type, dxfgroup)                                    \
  LOG_TRACE (#nam ": " FORMAT_##type " [" #type " " #dxfgroup "]",            \
             (BITCODE_##type)_obj->nam);                                      \
  LOG_POS
#define FIELD_CAST(nam, type, cast, dxf)                                      \
  {                                                                           \
    bit_write_##type (dat, (BITCODE_##type)_obj->nam);                        \
    LOG_TRACE (#nam ": " FORMAT_##cast " [" #cast " " #dxf "]",               \
               (BITCODE_##cast)_obj->nam);                                    \
    LOG_POS                                                                   \
  }
#define SUB_FIELD(o, nam, type, dxf) FIELD (o.nam, type)
#define SUB_FIELD_CAST(o, nam, type, cast, dxf)                               \
  {                                                                           \
    bit_write_##type (dat, (BITCODE_##type)_obj->o.nam);                      \
    FIELD_G_TRACE (o.nam, cast, dxf);                                         \
  }

#define FIELD_VALUE(nam) _obj->nam

#define FIELD_B(nam, dxf) FIELDG (nam, B, dxf)
#define FIELD_BB(nam, dxf) FIELDG (nam, BB, dxf)
#define FIELD_3B(nam, dxf) FIELDG (nam, 3B, dxf)
#define FIELD_BS(nam, dxf) FIELDG (nam, BS, dxf)
#define FIELD_RCd(nam, dxf) FIELD_CAST (nam, RC, RCd, dxf)
#define FIELD_BSd(nam, dxf) FIELD_CAST (nam, BS, BSd, dxf)
#define FIELD_RSx(nam, dxf) FIELD_CAST (nam, RS, RSx, dxf)
#define FIELD_RSd(nam, dxf) FIELD_CAST (nam, RS, RSd, dxf)
#define FIELD_RLx(nam, dxf) FIELD_CAST (nam, RL, RLx, dxf)
#define FIELD_BLx(nam, dxf) FIELD_CAST (nam, BL, BLx, dxf)
#define FIELD_BLd(nam, dxf) FIELD_CAST (nam, BL, BLd, dxf)
#define FIELD_RLd(nam, dxf) FIELD_CAST (nam, RL, RLd, dxf)
#define FIELD_BL(nam, dxf) FIELDG (nam, BL, dxf)
#define FIELD_BLL(nam, dxf) FIELDG (nam, BLL, dxf)
#define FIELD_HV(nam, dxf) FIELD_CAST (nam, RLL, HV, dxf)
#define FIELD_BD(nam, dxf) FIELDG (nam, BD, dxf)
#define FIELD_RC(nam, dxf) FIELDG (nam, RC, dxf)
#define FIELD_RS(nam, dxf) FIELDG (nam, RS, dxf)
#define FIELD_RD(nam, dxf) FIELDG (nam, RD, dxf)
#define FIELD_RL(nam, dxf) FIELDG (nam, RL, dxf)
#define FIELD_RLL(nam, dxf) FIELDG (nam, RLL, dxf)
#define FIELD_RLLd(nam, dxf) FIELD_CAST (nam, RLL, RLLd, dxf)
#define FIELD_RLLu(nam, dxf)                                                  \
  {                                                                           \
    bit_write_RLL (dat, _obj->nam);                                           \
    FIELD_G_TRACE (nam, BLL, dxf);                                            \
  }
#define SUB_FIELD_RCd(o, nam, dxf) SUB_FIELD_CAST (o, nam, RC, RCd, dxf)
#define FIELD_MC(nam, dxf) FIELDG (nam, MC, dxf)
#define FIELD_MS(nam, dxf) FIELDG (nam, MS, dxf)
#define FIELD_TV(nam, dxf)                                                    \
  {                                                                           \
    IF_ENCODE_FROM_EARLIER                                                    \
    {                                                                         \
      if (!_obj->nam)                                                         \
        _obj->nam = strdup ("");                                              \
    }                                                                         \
    bit_write_TV (dat, _obj->nam);                                            \
    LOG_TRACE (#nam ": \"%s\" [TV %d]", _obj->nam, dxf);                      \
    LOG_POS                                                                   \
  }
// may need to convert from/to TV<=>TU
#define FIELD_T(nam, dxf)                                                     \
  {                                                                           \
    if (dat->version < R_2007) {                                              \
      bit_write_T (dat, _obj->nam);                                           \
      if (IS_FROM_TU (dat)) {                                                 \
        LOG_TRACE_TU_AS (#nam, _obj->nam, TV, dxf);                           \
      } else {                                                                \
        LOG_TRACE (#nam ": \"%s\" [TV %d]", _obj->nam ? _obj->nam : "", dxf); \
        LOG_POS                                                               \
      }                                                                       \
    } else {                                                                  \
      bit_write_T (str_dat, _obj->nam);                                       \
      if (IS_FROM_TU (dat)) {                                                 \
        LOG_TRACE_TU (#nam, _obj->nam, dxf);                                  \
      } else {                                                                \
        LOG_TRACE (#nam ": \"%s\" [TU %d]", _obj->nam ? _obj->nam : "", dxf); \
        LOG_POS                                                               \
      }                                                                       \
    }                                                                         \
  }
#define FIELD_TF(nam, len, dxf)                                               \
  {                                                                           \
    LOG_TRACE (#nam ": %s [TF %d %d]\n", _obj->nam, (int)len, dxf);           \
    if (len > 0 && len < MAX_SIZE_TF)                                         \
      {                                                                       \
        if (!_obj->nam)                                                       \
          { /* empty field, write zeros */                                    \
            for (int _i = 0; _i < (int)(len); _i++)                           \
              bit_write_RC (dat, 0);                                          \
          } /* The source might not be long enough. or it is, just with a zero */ /* Luckily TFF's are only preR13 */                                   \
        else                                                                  \
          {                                                                   \
            bit_write_TF (dat, (BITCODE_TF)_obj->nam, len);                   \
          }                                                                   \
      }                                                                       \
    LOG_TRACE_TF (FIELD_VALUE (nam), (int)len);                               \
  }
// clang-format off
// zero-terminated fixed buffer, which might be shorter
#define FIELD_TFv(nam, len, dxf)                                              \
  {                                                                           \
    if (len > 0 && len < MAX_SIZE_TF)                                         \
      {                                                                       \
        if (!_obj->nam)                                                       \
          { /* empty field, write zeros */                                    \
            for (int _i = 0; _i < (int)(len); _i++)                           \
              bit_write_RC (dat, 0);                                          \
          }                                                                   \
        else                                                                  \
          {                                                                   \
            bit_write_TFv (dat, (BITCODE_TF)_obj->nam, len);                  \
          }                                                                   \
      }                                                                       \
    LOG_TRACE (#nam ": \"%s\" [TFv %d %d]", _obj->nam, (int)len, dxf);        \
    LOG_POS                                                                   \
    /*LOG_TRACE_TFv (FIELD_VALUE (nam), (int)len);*/                          \
  }
#define FIELD_BINARY(nam, len, dxf)                                           \
  {                                                                           \
    LOG_TRACE (#nam ": %s [TF %d %d]\n", _obj->nam, (int)len, dxf);           \
    if (len > 0)                                                              \
      {                                                                       \
        if (!_obj->nam)                                                       \
          { /* empty field, write zeros */                                    \
            for (int _i = 0; _i < (int)(len); _i++)                           \
              bit_write_RC (dat, 0);                                          \
          }                                                                   \
         /* The source might not be long enough. or it is, just with a zero */\
         /* Luckily TFF's are only preR13 */                                  \
        else                                                                  \
          {                                                                   \
            bit_write_TF (dat, (BITCODE_TF)_obj->nam, len);                   \
          }                                                                   \
      }                                                                       \
    LOG_INSANE_TF (FIELD_VALUE (nam), (int)len);                              \
  }
// clang-format on

#define FIELD_TFF(nam, len, dxf) FIELD_TF (nam, len, dxf)
#define FIELD_TU(nam, dxf)                                                    \
  {                                                                           \
    if (_obj->nam)                                                            \
      bit_write_TU (str_dat, (BITCODE_TU)_obj->nam);                          \
    LOG_TRACE_TU (#nam, (BITCODE_TU)_obj->nam, dxf);                          \
  }
#define FIELD_TU16(nam, dxf)                                                  \
  {                                                                           \
    if (_obj->nam)                                                            \
      bit_write_TU16 (str_dat, _obj->nam);                                    \
    LOG_TRACE_TU (#nam, (BITCODE_TU)_obj->nam, dxf);                          \
  }
#define FIELD_T16(nam, dxf)                                                   \
  {                                                                           \
    if (!IS_FROM_TU (dat))                                                    \
      {                                                                       \
        bit_write_T16 (str_dat, _obj->nam);                                   \
        LOG_TRACE (#nam ": \"%s\" [T16 %d]\n", _obj->nam, dxf);               \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        bit_write_TU16 (str_dat, (BITCODE_TU)_obj->nam);                      \
        LOG_TRACE_TU (#nam, _obj->nam, dxf);                                  \
      }                                                                       \
  }
#define FIELD_T32(nam, dxf)                                                   \
  {                                                                           \
    if (_obj->nam)                                                            \
      bit_write_T32 (str_dat, _obj->nam);                                     \
    if (!IS_FROM_TU (dat))                                                    \
      LOG_TRACE (#nam ": \"%s\" [T32 %d]\n", _obj->nam, dxf);                 \
    else                                                                      \
      LOG_TRACE_TU (#nam, (BITCODE_TU)_obj->nam, dxf);                        \
  }
#define FIELD_TU32(nam, dxf)                                                  \
  {                                                                           \
    if (_obj->nam)                                                            \
      bit_write_TU32 (str_dat, _obj->nam);                                    \
    if (!IS_FROM_TU (dat))                                                    \
      LOG_TRACE (#nam ": \"%s\" [TU32 %d]\n", _obj->nam, dxf);                \
    else                                                                      \
      LOG_TRACE_TU (#nam, (BITCODE_TU)_obj->nam, dxf);                        \
  }
#define FIELD_BT(nam, dxf) FIELDG (nam, BT, dxf);

#define _FIELD_DD(nam, _default, dxf)                                         \
  bit_write_DD (dat, FIELD_VALUE (nam), _default);
#define FIELD_DD(nam, _default, dxf)                                          \
  {                                                                           \
    BITCODE_BB b1 = _FIELD_DD (nam, _default, dxf);                           \
    if (b1 == 3)                                                              \
      LOG_TRACE (#nam ": %f [DD %d]", _obj->nam, dxf);                        \
    else                                                                      \
      LOG_TRACE (#nam ": %f [DD/%d %d]", _obj->nam, b1, dxf);                 \
    LOG_POS                                                                   \
  }
#define FIELD_2DD(nam, def, dxf)                                              \
  {                                                                           \
    BITCODE_BB b2, b1 = _FIELD_DD (nam.x, FIELD_VALUE (def.x), dxf);          \
    b2 = _FIELD_DD (nam.y, FIELD_VALUE (def.y), dxf + 10);                    \
    if (b1 == 3 && b2 == 3)                                                   \
      LOG_TRACE (#nam ": (%f, %f) [2DD %d]", _obj->nam.x, _obj->nam.y, dxf);  \
    else                                                                      \
      LOG_TRACE (#nam ": (%f, %f) [2DD/%d%d %d]", _obj->nam.x, _obj->nam.y,   \
                 b1, b2, dxf);                                                \
    LOG_POS                                                                   \
  }
#define FIELD_3DD(nam, def, dxf)                                              \
  {                                                                           \
    _FIELD_DD (nam.x, FIELD_VALUE (def.x), dxf);                              \
    _FIELD_DD (nam.y, FIELD_VALUE (def.y), dxf + 10);                         \
    _FIELD_DD (nam.z, FIELD_VALUE (def.z), dxf + 20);                         \
    LOG_TRACE (#nam ": (%f, %f, %f) [3DD %d]", _obj->nam.x, _obj->nam.y,      \
               _obj->nam.z, dxf);                                             \
    LOG_POS                                                                   \
  }
#define FIELD_2RD(nam, dxf)                                                   \
  {                                                                           \
    bit_write_RD (dat, _obj->nam.x);                                          \
    bit_write_RD (dat, _obj->nam.y);                                          \
    LOG_TRACE (#nam ": (%f, %f) [2RD %d]", _obj->nam.x, _obj->nam.y, dxf);    \
    LOG_POS                                                                   \
  }
#define FIELD_2BD(nam, dxf)                                                   \
  {                                                                           \
    bit_write_BD (dat, _obj->nam.x);                                          \
    bit_write_BD (dat, _obj->nam.y);                                          \
    LOG_TRACE (#nam ": (%f, %f) [2BD %d]", _obj->nam.x, _obj->nam.y, dxf);    \
    LOG_POS                                                                   \
  }
#define FIELD_2BD_1(nam, dxf) FIELD_2BD (nam, dxf)
#define FIELD_3RD(nam, dxf)                                                   \
  {                                                                           \
    bit_write_RD (dat, _obj->nam.x);                                          \
    bit_write_RD (dat, _obj->nam.y);                                          \
    bit_write_RD (dat, _obj->nam.z);                                          \
    LOG_TRACE (#nam ": (%f, %f, %f) [3RD %d]", _obj->nam.x, _obj->nam.y,      \
               _obj->nam.z, dxf);                                             \
    LOG_POS                                                                   \
  }
#define FIELD_3BD(nam, dxf)                                                   \
  {                                                                           \
    bit_write_BD (dat, _obj->nam.x);                                          \
    bit_write_BD (dat, _obj->nam.y);                                          \
    bit_write_BD (dat, _obj->nam.z);                                          \
    LOG_TRACE (#nam ": (%f, %f, %f) [3BD %d]", _obj->nam.x, _obj->nam.y,      \
               _obj->nam.z, dxf);                                             \
    LOG_POS                                                                   \
  }
#define FIELD_3BD_1(nam, dxf) FIELD_3BD (nam, dxf)
#define FIELD_3DPOINT(nam, dxf) FIELD_3BD (nam, dxf)
#define FIELD_4BITS(nam, dxf)                                                 \
  {                                                                           \
    unsigned char _b = (unsigned char)_obj->nam;                              \
    bit_write_4BITS (dat, _b);                                                \
    LOG_TRACE (#nam ": b%d%d%d%d [4BITS %d]", _b & 8, _b & 4, _b & 2, _b & 1, \
               dxf);                                                          \
    LOG_POS                                                                   \
  }
#define FIELD_TIMEBLL(nam, dxf)                                               \
  {                                                                           \
    bit_write_TIMEBLL (dat, _obj->nam);                                       \
    LOG_TRACE (#nam ": " FORMAT_BL "." FORMAT_BL " [TIMEBLL %d]",             \
               _obj->nam.days, _obj->nam.ms, dxf);                            \
    LOG_POS                                                                   \
  }
#define FIELD_TIMERLL(nam, dxf)                                               \
  {                                                                           \
    bit_write_TIMERLL (dat, _obj->nam);                                       \
    LOG_TRACE (#nam ": " FORMAT_RL "." FORMAT_RL " [TIMERLL %d]",             \
               _obj->nam.days, _obj->nam.ms, dxf);                            \
    LOG_POS                                                                   \
  }

#define FIELD_CMC(color, dxf)                                                 \
  {                                                                           \
    bit_write_CMC (dat, str_dat, &_obj->color);                               \
    LOG_TRACE (#color ".index: %d [CMC.%s %d]", _obj->color.index,            \
               dat->version < R_13b1 ? "RS" : "BS", dxf);                     \
    LOG_POS                                                                   \
    if (dat->version >= R_2004)                                               \
      {                                                                       \
        LOG_TRACE (#color ".rgb: 0x%08x [CMC.BL %d]\n",                       \
                   (unsigned)_obj->color.rgb, dxf + 420 - 62);                \
        LOG_TRACE (#color ".flag: 0x%x [CMC.RC]\n",                           \
                   (unsigned)_obj->color.flag);                               \
        if (_obj->color.flag & 1)                                             \
          LOG_TRACE (#color ".name: %s [CMC.T]\n", _obj->color.name);         \
        if (_obj->color.flag & 2)                                             \
          LOG_TRACE (#color ".bookname: %s [CMC.T]\n",                        \
                     _obj->color.book_name);                                  \
        LOG_INSANE (" @%" PRIuSIZE ".%u\n",                                   \
                    obj ? dat->byte - obj->address : dat->byte, dat->bit);    \
      }                                                                       \
  }
#define SUB_FIELD_CMC(o, color, dxf)                                          \
  {                                                                           \
    bit_write_CMC (dat, str_dat, &_obj->o.color);                             \
    LOG_TRACE (#color ".index: %d [CMC.BS %d]\n", _obj->o.color.index, dxf);  \
    LOG_INSANE (" @%" PRIuSIZE ".%u\n",                                       \
                obj ? dat->byte - obj->address : dat->byte, dat->bit);        \
    if (dat->version >= R_2004)                                               \
      {                                                                       \
        LOG_TRACE (#color ".rgb: 0x%06x [CMC.BL %d]\n",                       \
                   (unsigned)_obj->o.color.rgb, dxf + 420 - 62);              \
        LOG_TRACE (#color ".flag: 0x%x [CMC.RC]\n",                           \
                   (unsigned)_obj->o.color.flag);                             \
        if (_obj->o.color.flag & 1)                                           \
          LOG_TRACE (#color ".name: %s [CMC.T]\n", _obj->o.color.name);       \
        if (_obj->o.color.flag & 2)                                           \
          LOG_TRACE (#color ".bookname: %s [CMC.T]\n",                        \
                     _obj->o.color.book_name);                                \
        LOG_INSANE (" @%" PRIuSIZE ".%u\n",                                   \
                    obj ? dat->byte - obj->address : dat->byte, dat->bit);    \
      }                                                                       \
  }

#define LOG_TF(level, var, len)                                               \
  if (var)                                                                    \
    {                                                                         \
      int _i;                                                                 \
      for (_i = 0; _i < (int)(len); _i++)                                     \
        {                                                                     \
          LOG (level, "%02X", (unsigned char)((char *)var)[_i]);              \
        }                                                                     \
      LOG (level, "\n");                                                      \
      if (DWG_LOGLEVEL >= DWG_LOGLEVEL_INSANE)                                \
        {                                                                     \
          for (_i = 0; _i < (int)(len); _i++)                                 \
            {                                                                 \
              unsigned char c = ((unsigned char *)var)[_i];                   \
              LOG_INSANE ("%-2c", isprint (c) ? c : ' ');                     \
            }                                                                 \
          LOG_INSANE ("\n");                                                  \
        }                                                                     \
    }
#define LOG_TFv(level, var, len)                                              \
  if (var)                                                                    \
    {                                                                         \
      int _i;                                                                 \
      int _size = (int)MIN ((size_t)len, strlen (var));                       \
      for (_i = 0; _i < _size; _i++)                                          \
        {                                                                     \
          LOG (level, "%02X", (unsigned char)((char *)var)[_i]);              \
        }                                                                     \
      LOG (level, "\n");                                                      \
      if (DWG_LOGLEVEL >= DWG_LOGLEVEL_INSANE)                                \
        {                                                                     \
          for (_i = 0; _i < _size; _i++)                                      \
            {                                                                 \
              unsigned char c = ((unsigned char *)var)[_i];                   \
              LOG_INSANE ("%-2c", isprint (c) ? c : ' ');                     \
            }                                                                 \
          LOG_INSANE ("\n");                                                  \
        }                                                                     \
    }
#define LOG_TRACE_TF(var, len) LOG_TF (TRACE, var, len);
#define LOG_INSANE_TF(var, len) LOG_TF (INSANE, var, len);
#define LOG_TRACE_TFv(var, len) LOG_TFv (TRACE, var, len);
#define LOG_INSANE_TFv(var, len) LOG_TFv (INSANE, var, len);

#define FIELD_BE(nam, dxf)                                                    \
  {                                                                           \
    bit_write_BE (dat, FIELD_VALUE (nam.x), FIELD_VALUE (nam.y),              \
                  FIELD_VALUE (nam.z));                                       \
    if (dat->version >= R_2000 && FIELD_VALUE (nam.x) == 0.0                  \
        && FIELD_VALUE (nam.y) == 0.0 && FIELD_VALUE (nam.z) == 1.0)          \
      LOG_TRACE (#nam ": default 0,0,1 [B %d]", dxf);                         \
    else                                                                      \
      LOG_TRACE (#nam ": (%f, %f, %f) [BE %d]", _obj->nam.x, _obj->nam.y,     \
                   _obj->nam.z, dxf);                                         \
    LOG_POS                                                                   \
  }

#define OVERFLOW_CHECK(nam, size)                                             \
  if ((long)(size) > 0xff00L || (!_obj->nam && size) || (_obj->nam && !size)) \
    {                                                                         \
      LOG_ERROR ("Invalid " #nam " %ld", (long)size);                         \
      return DWG_ERR_VALUEOUTOFBOUNDS;                                        \
    }
#define OVERFLOW_CHECK_LV(nam, size)                                          \
  if ((long)(size) > 0xff00L)                                                 \
    {                                                                         \
      LOG_ERROR ("Invalid " #nam " %ld, set to 0", (long)size);               \
      size = 0;                                                               \
      return DWG_ERR_VALUEOUTOFBOUNDS;                                        \
    }
#define OVERFLOW_NULL_CHECK_LV(nam, size)                                     \
  if ((size) > MAX_NUM || (!_obj->nam && size) || (_obj->nam && !size))       \
    {                                                                         \
      LOG_ERROR ("Invalid " #nam " %ld, set to 0", (long)size);               \
      size = 0;                                                               \
      return DWG_ERR_VALUEOUTOFBOUNDS;                                        \
    }

#define FIELD_2RD_VECTOR(nam, size, dxf)                                      \
  OVERFLOW_NULL_CHECK_LV (nam, _obj->size)                                    \
  for (vcount = 0; vcount < (BITCODE_BL)_obj->size; vcount++)                 \
    {                                                                         \
      FIELD_2RD (nam[vcount], dxf);                                           \
    }

#define FIELD_2DD_VECTOR(nam, size, dxf)                                      \
  OVERFLOW_NULL_CHECK_LV (nam, _obj->size)                                    \
  if (_obj->size)                                                             \
    FIELD_2RD (nam[0], dxf);                                                  \
  for (vcount = 1; vcount < (BITCODE_BL)_obj->size; vcount++)                 \
    {                                                                         \
      FIELD_2DD (nam[vcount], nam[vcount - 1], dxf);                          \
    }

#define FIELD_3DPOINT_VECTOR(nam, size, dxf)                                  \
  OVERFLOW_NULL_CHECK_LV (nam, _obj->size)                                    \
  for (vcount = 0; vcount < (BITCODE_BL)_obj->size; vcount++)                 \
    {                                                                         \
      FIELD_3DPOINT (nam[vcount], dxf);                                       \
    }

#define REACTORS(code)                                                        \
  if (obj->tio.object->reactors)                                              \
    {                                                                         \
      OVERFLOW_CHECK_LV (num_reactors, obj->tio.object->num_reactors)         \
      SINCE (R_13b1)                                                          \
      {                                                                       \
        for (vcount = 0; vcount < (BITCODE_BL)obj->tio.object->num_reactors;  \
             vcount++)                                                        \
          {                                                                   \
            VALUE_HANDLE (obj->tio.object->reactors[vcount], reactors, code,  \
                          330);                                               \
          }                                                                   \
      }                                                                       \
    }

#define XDICOBJHANDLE(code)                                                   \
  RESET_VER                                                                   \
  SINCE (R_2004a)                                                             \
  {                                                                           \
    if (!obj->tio.object->is_xdic_missing)                                    \
      {                                                                       \
        VALUE_HANDLE (obj->tio.object->xdicobjhandle, xdicobjhandle, code,    \
                      360);                                                   \
      }                                                                       \
  }                                                                           \
  else                                                                        \
  {                                                                           \
    SINCE (R_13b1)                                                            \
    {                                                                         \
      VALUE_HANDLE (obj->tio.object->xdicobjhandle, xdicobjhandle, code,      \
                    360);                                                     \
    }                                                                         \
  }                                                                           \
  RESET_VER

#define ENT_XDICOBJHANDLE(code)                                               \
  RESET_VER                                                                   \
  SINCE (R_2004a)                                                             \
  {                                                                           \
    if (!obj->tio.entity->is_xdic_missing)                                    \
      {                                                                       \
        VALUE_HANDLE (obj->tio.entity->xdicobjhandle, xdicobjhandle, 3, 360); \
      }                                                                       \
  }                                                                           \
  else                                                                        \
  {                                                                           \
    SINCE (R_13b1)                                                            \
    {                                                                         \
      VALUE_HANDLE (obj->tio.entity->xdicobjhandle, xdicobjhandle, 3, 360);   \
    }                                                                         \
  }                                                                           \
  RESET_VER

// FIELD_VECTOR_N(nam, type, size, dxf):
// writes a 'size' elements vector of data of the type indicated by 'type'
#define FIELD_VECTOR_N(nam, type, size, dxf)                                  \
  if (size > 0 && _obj->nam)                                                  \
    {                                                                         \
      OVERFLOW_CHECK (nam, size)                                              \
      for (vcount = 0; vcount < (BITCODE_BL)size; vcount++)                   \
        {                                                                     \
          bit_write_##type (dat, _obj->nam[vcount]);                          \
          LOG_TRACE (#nam "[%ld]: " FORMAT_##type " [%s %d]", (long)vcount,   \
                     _obj->nam[vcount], #type, dxf);                          \
          LOG_POS                                                             \
        }                                                                     \
    }
#define FIELD_VECTOR_T(nam, type, size, dxf)                                  \
  if (_obj->size > 0 && _obj->nam)                                            \
    {                                                                         \
      OVERFLOW_CHECK_LV (nam, _obj->size)                                     \
      for (vcount = 0; vcount < (BITCODE_BL)_obj->size; vcount++)             \
        {                                                                     \
          if (dat->version != dat->from_version                               \
              || dat->version >= R_2007)                                      \
            {                                                                 \
              FIELD_##type (nam[vcount], dxf)                                 \
            }                                                                 \
          else if (!IS_FROM_TU (dat))                                         \
            {                                                                 \
              bit_write_TV (dat, (BITCODE_TV)_obj->nam[vcount]);              \
              LOG_TRACE (#nam "[%d]: \"%s\" [TV %d]", (int)vcount,            \
                         _obj->nam[vcount], dxf);                             \
              LOG_POS                                                         \
            }                                                                 \
          else                                                                \
            {                                                                 \
              bit_write_##type (dat, _obj->nam[vcount]);                      \
              if (IS_FROM_TU (dat)) {                                         \
                LOG_TRACE_TU (#nam, _obj->nam[vcount], dxf);                  \
              } else {                                                        \
                LOG_TRACE (#nam "[%d]: \"%s\" [TU %d]", (int)vcount,          \
                           _obj->nam[vcount] ? _obj->nam[vcount] : "", dxf);  \
                LOG_POS                                                       \
              }                                                               \
            }                                                                 \
        }                                                                     \
      RESET_VER                                                               \
    }
#define FIELD_VECTOR_T1(nam, type, size, dxf)                                 \
  if (_obj->size > 0 && _obj->nam)                                            \
    {                                                                         \
      OVERFLOW_CHECK_LV (nam, _obj->size)                                     \
      for (vcount = 0; vcount < (BITCODE_BL)_obj->size; vcount++)             \
        {                                                                     \
          if (!IS_FROM_TU (dat))                                              \
            {                                                                 \
              size_t _len = strlen (_obj->nam[vcount]);                       \
              bit_write_BS (dat, _len & 0xFFFFFFFF);                          \
              bit_write_TF (dat, (BITCODE_TF)_obj->nam[vcount], _len);        \
              LOG_TRACE (#nam "[%d]: \"%s\" [TV1 %d]", (int)vcount,           \
                         _obj->nam[vcount], dxf);                             \
              LOG_POS                                                         \
            }                                                                 \
          else                                                                \
            {                                                                 \
              bit_write_##type (dat, _obj->nam[vcount]);                      \
              if (IS_FROM_TU (dat)) {                                         \
                LOG_TRACE_TU (#nam, _obj->nam[vcount], dxf);                  \
              } else {                                                        \
                LOG_TRACE (#nam "[%d]: \"%s\" [TU %d]", (int)vcount,          \
                           _obj->nam[vcount] ? _obj->nam[vcount] : "", dxf);  \
                LOG_POS                                                       \
              }                                                               \
            }                                                                 \
        }                                                                     \
      RESET_VER                                                               \
    }

#define FIELD_VECTOR(nam, type, size, dxf)                                    \
  FIELD_VECTOR_N (nam, type, _obj->size, dxf)
#define FIELD_VECTOR_INL(nam, type, size, dxf)                                \
  FIELD_VECTOR_N (nam, type, size, dxf)

#define SUB_FIELD_VECTOR_TYPESIZE(o, nam, size, typesize, dxf)                \
  if (_obj->o.size > 0 && _obj->o.nam)                                        \
    {                                                                         \
      OVERFLOW_CHECK (nam, _obj->o.size)                                      \
      for (vcount = 0; vcount < (BITCODE_BL)_obj->o.size; vcount++)           \
        {                                                                     \
          bit_write_##type (dat, _obj->nam[vcount]);                          \
          switch (typesize)                                                   \
            {                                                                 \
            case 0:                                                           \
              break;                                                          \
            case 1:                                                           \
              bit_write_RC (dat, _obj->o.name[vcount]);                       \
              break;                                                          \
            case 2:                                                           \
              bit_write_RS (dat, _obj->o.name[vcount]);                       \
              break;                                                          \
            case 4:                                                           \
              bit_write_RL (dat, _obj->o.name[vcount]);                       \
              break;                                                          \
            case 8:                                                           \
              bit_write_RLL (dat, _obj->o.name[vcount]);                      \
              break;                                                          \
            default:                                                          \
              LOG_ERROR ("Unknown SUB_FIELD_VECTOR_TYPE " #nam                \
                         " typesize %d",                                      \
                         typesize);                                           \
              break;                                                          \
            }                                                                 \
          LOG_TRACE (#nam "[%u]: %d\n", vcount, _obj->nam[vcount]);           \
        }                                                                     \
    }

// clang-format off
#define VALUE_HANDLE(hdlptr, nam, handle_code, dxf)                           \
  {                                                                           \
    PRE (R_13b1)                                                              \
    {                                                                         \
      short idx = (hdlptr) ? (hdlptr)->r11_idx : -1;                          \
      short size = (hdlptr)           ? hdlptr->handleref.size                \
                   : handle_code == 1 ? 1                                     \
                                      : 2; /* = handle_size really, not code */                                    \
      if (size == 1)                                                          \
        bit_write_RC (dat, idx);                                              \
      else if (size == 2)                                                     \
        bit_write_RS (dat, idx);                                              \
      else if (size == 8)                                                     \
        bit_write_RLL (dat, (hdlptr)->handleref.value);                       \
      else                                                                    \
        LOG_ERROR (#nam ": Invalid size %d %hd [H %d]", size, idx, dxf);      \
      LOG_TRACE (#nam ": %hd [%s %d]", idx,                                   \
                 size == 1   ? "RC"                                           \
                 : size == 2 ? "RSd"                                          \
                             : "RLL",                                         \
                 dxf);                                                        \
      LOG_POS                                                                 \
    }                                                                         \
    IF_ENCODE_SINCE_R13                                                       \
    {                                                                         \
      RESET_VER                                                               \
      if (!hdlptr)                                                            \
        {                                                                     \
          Dwg_Handle null_handle = { 0, 0, 0UL, 0 };                          \
          null_handle.code = handle_code;                                     \
          bit_write_H (hdl_dat, &null_handle);                                \
          LOG_TRACE (#nam ": (%d.0.0) abs:0 [H %d]", handle_code, dxf);       \
          LOG_HPOS                                                            \
        }                                                                     \
      else                                                                    \
        {                                                                     \
          if (handle_code != ANYCODE                                          \
              && (hdlptr)->handleref.code != handle_code                      \
              && (handle_code == 4 && (hdlptr)->handleref.code < 6))          \
            {                                                                 \
              LOG_WARN ("Expected a CODE %d handle, got a %d", handle_code,   \
                        (hdlptr)->handleref.code);                            \
            } /*else if (dat->version <= R_2000 &&                            \
                         dat->from_version > R_2000 &&                        \
                         (hdlptr)->handleref.code > 5 && handle_code == 4)    \
            {                                                                 \
              downconvert_relative_handle (hdlptr, obj);                      \
            }*/                                                               \
          bit_write_H (hdl_dat, &(hdlptr)->handleref);                        \
          LOG_TRACE (#nam ": " FORMAT_REF " [H %d]", ARGS_REF (hdlptr), dxf); \
          LOG_HPOS                                                            \
        }                                                                     \
    }                                                                         \
  }
// clang-format on
// for obj->handle 0.x.x only, DXF 5
#define VALUE_H(hdl, dxf)                                                     \
  {                                                                           \
    PRE (R_13b1)                                                              \
    {                                                                         \
      bit_write_H (dat, &hdl);                                                \
    }                                                                         \
    else                                                                      \
    {                                                                         \
      bit_write_H (hdl_dat, &hdl);                                            \
    }                                                                         \
    LOG_TRACE ("handle: " FORMAT_H " [H %d]", ARGS_H (hdl), dxf);             \
    LOG_RPOS                                                                  \
  }

#define FIELD_HANDLE(nam, handle_code, dxf)                                   \
  VALUE_HANDLE (_obj->nam, nam, handle_code, dxf)
#define SUB_FIELD_HANDLE(o, nam, handle_code, dxf)                            \
  VALUE_HANDLE (_obj->o.nam, nam, handle_code, dxf)
#define FIELD_DATAHANDLE(nam, handle_code, dxf)                               \
  {                                                                           \
    bit_write_H (dat, _obj->nam ? &_obj->nam->handleref : NULL);              \
  }

#define FIELD_HANDLE_N(nam, vcount, handle_code, dxf)                         \
  IF_ENCODE_SINCE_R13                                                         \
  {                                                                           \
    RESET_VER                                                                 \
    if (!_obj->nam)                                                           \
      {                                                                       \
        bit_write_H (hdl_dat, NULL);                                          \
        LOG_TRACE (#nam "[%d]: NULL %d [H* %d]", (int)vcount, handle_code,    \
                   dxf);                                                      \
        LOG_HPOS                                                              \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        if (handle_code != ANYCODE                                            \
            && _obj->nam->handleref.code != handle_code                       \
            && (handle_code == 4 && _obj->nam->handleref.code < 6))           \
          {                                                                   \
            LOG_WARN ("Expected a CODE %x handle, got a %x", handle_code,     \
                      _obj->nam->handleref.code);                             \
          }                                                                   \
        bit_write_H (hdl_dat, &_obj->nam->handleref);                         \
        LOG_TRACE (#nam "[%d]: " FORMAT_REF " [H* %d]", (int)vcount,          \
                   ARGS_REF (_obj->nam), dxf);                                \
        LOG_HPOS                                                              \
      }                                                                       \
  }

#define HANDLE_VECTOR_N(nam, size, code, dxf)                                 \
  if (size > 0 && _obj->nam)                                                  \
    {                                                                         \
      OVERFLOW_CHECK (nam, size)                                              \
      for (vcount = 0; vcount < (BITCODE_BL)size; vcount++)                   \
        {                                                                     \
          if (_obj->nam[vcount])                                              \
            {                                                                 \
              FIELD_HANDLE_N (nam[vcount], vcount, code, dxf);                \
            }                                                                 \
        }                                                                     \
    }

#define FIELD_NUM_INSERTS(num_inserts, type, dxf)                             \
  for (vcount = 0; vcount < FIELD_VALUE (num_inserts); vcount++)              \
    {                                                                         \
      bit_write_RC (dat, 1);                                                  \
    }                                                                         \
  bit_write_RC (dat, 0);                                                      \
  LOG_TRACE ("num_inserts: %d [RC* 0]", FIELD_VALUE (num_inserts));           \
  LOG_POS

#define HANDLE_VECTOR(nam, sizefield, code, dxf)                              \
  HANDLE_VECTOR_N (nam, FIELD_VALUE (sizefield), code, dxf)

#define FIELD_XDATA(nam, size)                                                \
  error |= dwg_encode_xdata (dat, _obj, _obj->size)

#define COMMON_ENTITY_HANDLE_DATA                                             \
  SINCE (R_13b1)                                                              \
  {                                                                           \
    START_HANDLE_STREAM;                                                      \
  }                                                                           \
  RESET_VER

#define START_OBJECT_HANDLE_STREAM START_HANDLE_STREAM
#define CONTROL_HANDLE_STREAM START_HANDLE_STREAM

#define SECTION_STRING_STREAM                                                 \
  {                                                                           \
    Bit_Chain *sav_dat = dat;                                                 \
    dat = str_dat;

/* TODO: dump all TU strings here */
#define START_STRING_STREAM                                                   \
  bit_write_B (dat, obj->has_strings);                                        \
  RESET_VER                                                                   \
  if (obj->has_strings)                                                       \
    {                                                                         \
      Bit_Chain sav_dat = *dat;                                               \
      obj_string_stream (dat, obj, dat);

#define END_STRING_STREAM                                                     \
  dat = sav_dat;                                                              \
  }
#define ENCODE_COMMON_HANDLES                                                 \
  if (obj->supertype == DWG_SUPERTYPE_OBJECT && dat->version >= R_13b1)       \
    {                                                                         \
      VALUE_HANDLE (obj->tio.object->ownerhandle, ownerhandle, 4, 330);       \
      REACTORS (4);                                                           \
      XDICOBJHANDLE (3);                                                      \
    }                                                                         \
  else if (obj->supertype == DWG_SUPERTYPE_ENTITY && dat->version >= R_13b1)  \
    {                                                                         \
      error |= dwg_encode_common_entity_handle_data (dat, hdl_dat, obj);      \
    }

// clang-format off
#define START_HANDLE_STREAM                                                   \
  LOG_INSANE ("HANDLE_STREAM @%" PRIuSIZE ".%u\n", dat->byte - obj->address,  \
              dat->bit);                                                      \
  /* R2007+ object string stream footer */                                    \
  if (dat->version >= R_2007 && str_dat != dat) {                             \
    size_t _str_bits = bit_position (str_dat);                                \
    LOG_INSANE ("-str_dat: %" PRIuSIZE " bits, chain %p, dat chain %p\n",     \
                _str_bits, (void*)str_dat->chain, (void*)dat->chain);         \
    if (_str_bits) {                                                          \
      BITCODE_RS _data_size = (BITCODE_RS)_str_bits;                          \
      bit_copy_chain (dat, str_dat);                                          \
      if (_data_size & 0x8000) {                                              \
        bit_write_RS (dat, (_data_size >> 15) & 0x7FFF);                      \
        bit_write_RS (dat, (_data_size & 0x7FFF) | 0x8000);                   \
      } else                                                                  \
        bit_write_RS (dat, _data_size);                                       \
      bit_write_B (dat, 1);                                                   \
      obj->has_strings = 1;                                                   \
      LOG_TRACE ("-obj string footer: data_size %u, has_strings 1\n",         \
                 (unsigned)_data_size);                                        \
    } else {                                                                  \
      bit_write_B (dat, 0);                                                   \
      obj->has_strings = 0;                                                   \
    }                                                                         \
  }                                                                           \
  if (1 || /* has floats */                                                   \
      !obj->bitsize || /* DD sizes can vary, but let unknown_bits asis */     \
      has_entity_DD (obj) || /* strings may be zero-terminated or not */      \
      obj_has_strings (obj)                                                   \
      || (dwg->header.version != dwg->header.from_version                     \
          && obj->fixedtype != DWG_TYPE_UNKNOWN_OBJ                           \
          && obj->fixedtype != DWG_TYPE_UNKNOWN_ENT))                         \
    {                                                                         \
      obj->bitsize = (bit_position (dat) - (obj->address * 8)) & 0xFFFFFFFF;  \
      LOG_TRACE ("-bitsize calc from HANDLE_STREAM " FORMAT_RL " @%" PRIuSIZE \
                 ".%u (%" PRIuSIZE ")\n",                                     \
                 obj->bitsize, dat->byte - obj->address, dat->bit,            \
                 obj->address);                                               \
      obj->was_bitsize_set = 1;                                               \
    }                                                                         \
  if (!obj->hdlpos)                                                           \
    obj->hdlpos = bit_position (dat);                                         \
  {                                                                           \
    size_t _hpos = bit_position (hdl_dat);                                    \
    if (_hpos > 0)                                                            \
      { /* save away special accumulated hdls, need to write common first */  \
        Bit_Chain dat1 = *hdl_dat;                                            \
        Bit_Chain dat2 = { 0 };                                               \
        bit_chain_init_dat (&dat2, 12, dat);                                  \
        hdl_dat = &dat2;                                                      \
        ENCODE_COMMON_HANDLES /* owner, xdic, reactors */                     \
        obj_flush_hdlstream (obj, dat, hdl_dat); /* common */                 \
        /* special accumulated (e.g. xref) */                                 \
        obj_flush_hdlstream (obj, dat, &dat1);                                \
        bit_chain_free (&dat1);                                               \
        bit_chain_free (&dat2);                                               \
        *hdl_dat = *dat;                                                      \
        hdl_dat = dat;                                                        \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        if (hdl_dat != dat)                                                   \
          bit_chain_free (hdl_dat);                                           \
        hdl_dat = dat;                                                        \
        ENCODE_COMMON_HANDLES                                                 \
      }                                                                       \
  }                                                                           \
  RESET_VER
// clang-format on

static void
obj_flush_hdlstream (Dwg_Object *restrict obj, Bit_Chain *restrict dat,
                     Bit_Chain *restrict hdl_dat)
{
  size_t datpos = bit_position (dat);
  size_t hdlpos = bit_position (hdl_dat);
  size_t objpos = obj->address * 8;
#if 0
  unsigned char* oldchain = dat->chain;
#endif
  LOG_TRACE ("Flush handle stream of size %" PRIuSIZE " (@%" PRIuSIZE
             ".%u) to @%" PRIuSIZE ".%" PRIuSIZE "\n",
             hdlpos, hdl_dat->byte, hdl_dat->bit, (datpos - objpos) / 8,
             (datpos - objpos) % 8);
  // This might change dat->chain
  bit_copy_chain (dat, hdl_dat);
}
