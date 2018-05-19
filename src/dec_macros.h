/*--------------------------------------------------------------------------------
 * Welcome to the dark side of the moon...
 * MACROS
 */

#ifdef HAVE_CTYPE_H
#include <ctype.h>
#endif

#define IS_DECODER

#define VALUE(value,type,dxf) \
  LOG_TRACE(FORMAT_##type " [" #type " %d]\n", value, dxf)
#define VALUE_RC(value,dxf) VALUE(value, RC, dxf)

#define FIELDG(name,type,dxf) \
  { _obj->name = bit_read_##type(dat); \
    FIELD_G_TRACE(name,type,dxf); }

#define FIELD(name,type) \
  { _obj->name = bit_read_##type(dat); \
    FIELD_TRACE(name,type); }

#define FIELD_CAST(name,type,cast,dxf)\
  { _obj->name = (BITCODE_##cast)bit_read_##type(dat); \
    FIELD_G_TRACE(name,cast,dxf); }

#define FIELD_G_TRACE(name,type,dxfgroup) \
  LOG_TRACE(#name ": " FORMAT_##type " [" #type " %d]\n", _obj->name, dxfgroup)
#define FIELD_TRACE(name,type) \
  LOG_TRACE(#name ": " FORMAT_##type " " #type "\n", _obj->name)
#define LOG_TF(level,var,len)                   \
  { int _i; \
    for (_i=0; _i<len; _i++) { \
      LOG(level,"%02x ", (unsigned char)((char*)var)[_i]);      \
    } \
    LOG(level,"\n"); \
    for (_i=0; _i<len; _i++) { \
      char c = ((char*)var)[_i]; \
      LOG(level,"%2c ", isprint(c) ? c : ' ');  \
    } \
    LOG(level,"\n"); \
  }
#define LOG_TRACE_TF(var,len) LOG_TF(TRACE,var,len)
#define LOG_INSANE_TF(var,len) LOG_TF(INSANE,var,len)

#define FIELD_VALUE(name) _obj->name

#define ANYCODE -1
#define FIELD_HANDLE(name, handle_code, dxf) \
  { \
    if (handle_code >= 0) \
      {\
        _obj->name = dwg_decode_handleref_with_code(hdl_dat, obj, dwg, handle_code);\
      }\
    else\
      {\
        _obj->name = dwg_decode_handleref(hdl_dat, obj, dwg);\
      }\
    if (_obj->name)\
      {\
        LOG_TRACE(#name ": HANDLE(%d.%d.%lu) absolute:%lu [%d]\n", \
                  _obj->name->handleref.code, \
                  _obj->name->handleref.size, \
                  _obj->name->handleref.value,\
                  _obj->name->absolute_ref, dxf) \
      }\
  }
#define FIELD_HANDLE_N(name, vcount, handle_code, dxf)  \
  {\
    if (handle_code>=0) \
      {\
        _obj->name = dwg_decode_handleref_with_code(hdl_dat, obj, dwg, handle_code);\
      }\
    else\
      {\
        _obj->name = dwg_decode_handleref(hdl_dat, obj, dwg);\
      }\
    if (_obj->name)\
      {\
        LOG_TRACE(#name "[%d]: HANDLE(%d.%d.%lu) absolute:%lu [%d]\n",  \
              (int)vcount,                          \
              _obj->name->handleref.code,           \
              _obj->name->handleref.size,           \
              _obj->name->handleref.value,          \
              _obj->name->absolute_ref, dxf)        \
      }\
  }
#define FIELD_DATAHANDLE(name, handle_code, dxf) \
  { \
    _obj->name = dwg_decode_handleref(dat, obj, dwg);\
    LOG_TRACE(#name ": HANDLE(%d.%d.%lu) absolute:%lu\n",\
          _obj->name->handleref.code,\
          _obj->name->handleref.size,\
          _obj->name->handleref.value,\
          _obj->name->absolute_ref);\
  }

#define FIELD_B(name,dxf) FIELDG(name, B, dxf)
#define FIELD_BB(name,dxf) FIELDG(name, BB, dxf)
#define FIELD_3B(name,dxf) FIELDG(name, 3B, dxf)
#define FIELD_BS(name,dxf) FIELDG(name, BS, dxf)
#define FIELD_BL(name,dxf) FIELDG(name, BL, dxf)
#define FIELD_BLL(name,dxf) FIELDG(name, BLL, dxf)
#define FIELD_BD(name,dxf) FIELDG(name, BD, dxf)
#define FIELD_RC(name,dxf) FIELDG(name, RC, dxf)
#define FIELD_RS(name,dxf) FIELDG(name, RS, dxf)
#define FIELD_RD(name,dxf) FIELDG(name, RD, dxf)
#define FIELD_RL(name,dxf) FIELDG(name, RL, dxf)
#define FIELD_RLL(name,dxf) FIELDG(name, RLL, dxf)
#define FIELD_MC(name,dxf) FIELDG(name, MC, dxf)
#define FIELD_MS(name,dxf) FIELDG(name, MS, dxf)
#define FIELD_TF(name,len,dxf) \
  { _obj->name = bit_read_TF(dat,(int)len); \
    /* FIELD_G_TRACE(name, TF, dxf); */ \
    LOG_INSANE_TF(FIELD_VALUE(name), (int)len); }
#define FIELD_TFF(name,len,dxf) \
  { bit_read_fixed(dat,_obj->name,(int)len); \
    /* FIELD_G_TRACE(name, TF, dxf); */ \
    LOG_INSANE_TF(FIELD_VALUE(name), (int)len); }
#define FIELD_TV(name,dxf) FIELDG(name, TV, dxf)
#define FIELD_TU(name,dxf) \
  { _obj->name = (char*)bit_read_TU(str_dat); \
    LOG_TRACE_TU(#name, (BITCODE_TU)FIELD_VALUE(name), dxf); }
#define FIELD_T(name,dxf) \
  { if (dat->version < R_2007) { \
      FIELD_TV(name,dxf) \
    } else { \
      if (obj->has_strings) { \
        FIELD_TU(name,dxf) \
      } else { \
        LOG_TRACE_TU(#name, L"", dxf); \
      } \
    } \
  }
#define FIELD_BT(name,dxf) FIELDG(name, BT, dxf);
#define FIELD_4BITS(name,dxf) _obj->name = bit_read_4BITS(dat);

#define FIELD_BE(name,dxf) bit_read_BE(dat, &_obj->name.x, &_obj->name.y, &_obj->name.z);
#define FIELD_DD(name, _default, dxf) FIELD_VALUE(name) = bit_read_DD(dat, _default);
#define FIELD_2DD(name, d1, d2, dxf) { FIELD_DD(name.x, d1, dxf); FIELD_DD(name.y, d2, dxf+10); }
#define FIELD_3DD(name, def, dxf) { \
    FIELD_DD(name.x, FIELD_VALUE(def.x), dxf); \
    FIELD_DD(name.y, FIELD_VALUE(def.y), dxf+10); \
    FIELD_DD(name.z, FIELD_VALUE(def.z), dxf+20); }
#define FIELD_2RD(name,dxf) { FIELDG(name.x, RD, dxf); FIELDG(name.y, RD, dxf+10); }
#define FIELD_2BD(name,dxf) { FIELDG(name.x, BD, dxf); FIELDG(name.y, BD, dxf+10); }
#define FIELD_2BD_1(name,dxf) { FIELDG(name.x, BD, dxf); FIELDG(name.y, BD, dxf+1); }
#define FIELD_3RD(name,dxf) { FIELDG(name.x, RD, dxf); FIELDG(name.y, RD, dxf+10); \
                              FIELDG(name.z, RD,dxf+20); }
#define FIELD_3BD(name,dxf) { FIELDG(name.x, BD, dxf); FIELDG(name.y, BD, dxf+10); \
                              FIELDG(name.z, BD, dxf+20); }
#define FIELD_3BD_1(name,dxf) { FIELDG(name.x, BD, dxf); FIELDG(name.y, BD, dxf+1); \
                                FIELDG(name.z, BD, dxf+2); }
#define FIELD_3DPOINT(name,dxf) FIELD_3BD(name,dxf)
#define FIELD_TIMEBLL(name,dxf) \
  { _obj->name = bit_read_TIMEBLL(dat);                                  \
    LOG_TRACE(#name ": " FORMAT_BL "." FORMAT_BL "\n", _obj->name.days, _obj->name.ms); }
#define FIELD_CMC(name,dxf) \
  { bit_read_CMC(dat, &_obj->name); \
    LOG_TRACE(#name ": index %d\n", _obj->name.index); }

#undef DEBUG_HERE
#define DEBUG_HERE()\
  if (DWG_LOGLEVEL >= DWG_LOGLEVEL_TRACE) { \
    Bit_Chain here = *dat; \
    char *tmp; BITCODE_BB bb = 0; BITCODE_RS rs; BITCODE_RL rl;\
    LOG_TRACE("DEBUG_HERE @%u.%u / 0x%x\n  24RC: ", (unsigned int)dat->byte, dat->bit, \
              (unsigned int)dat->byte); \
    tmp = bit_read_TF(dat, 24);\
    LOG_TRACE_TF(tmp, 24);\
    SINCE(R_13) {\
      *dat = here;\
      LOG_TRACE("  B  :"FORMAT_B"\n", bit_read_B(dat));\
      *dat = here; bb = bit_read_BB(dat) & 0x3;\
      LOG_TRACE("  BB :"FORMAT_BB"\n", bb);\
    }\
    *dat = here; rs = bit_read_RS(dat);                \
    LOG_TRACE("  RS :"FORMAT_RS" / 0x%04x\n", rs, rs); \
    SINCE(R_13) {\
      *dat = here; rs = bit_read_BS(dat); \
      LOG_TRACE("  BS :"FORMAT_BS" / 0x%04x\n", rs, rs);\
     }\
    SINCE(R_2007) {\
      *dat = here; rs = bit_read_MS(dat);              \
      LOG_TRACE("  MS :"FORMAT_RS" / 0x%04x\n", rs, rs); \
    }\
    *dat = here; rl = bit_read_RL(dat);  \
    LOG_TRACE("  RL :"FORMAT_RL " / 0x%08x\n", rl, rl);\
    *dat = here;\
    LOG_TRACE("  RD :"FORMAT_RD "\n", bit_read_RD(dat));\
    *dat = here; \
    SINCE(R_13) {\
      if (bb != 3) { rl = bit_read_BL(dat);                     \
        LOG_TRACE("  BL :"FORMAT_BL " / 0x%08x\n", rl, rl);     \
        *dat = here;                                            \
        LOG_TRACE("  BD :"FORMAT_BD "\n", bit_read_BD(dat));    \
        *dat = here;                                            \
      }                                                         \
    }\
  }

//FIELD_VECTOR_N(name, type, size):
// reads data of the type indicated by 'type' 'size' times and stores
// it all in the vector called 'name'.
#define FIELD_VECTOR_N(name, type, size, dxf) \
  if (size > 0) \
    { \
      _obj->name = (BITCODE_##type*) malloc(size * sizeof(BITCODE_##type));\
      for (vcount=0; vcount<(long)size; vcount++) \
        {\
          _obj->name[vcount] = bit_read_##type(dat); \
          LOG_INSANE(#name "[%ld]: " FORMAT_##type "\n", \
                     (long)vcount, _obj->name[vcount]) \
        } \
    }
#define FIELD_VECTOR_T(name, size, dxf) \
  if (_obj->size > 0) \
    { \
      _obj->name = (char**) malloc(_obj->size * sizeof(char*)); \
      for (vcount=0; vcount<(long)_obj->size; vcount++) \
        {\
          PRE (R_2007) { \
            _obj->name[vcount] = bit_read_TV(dat); \
            LOG_INSANE(#name "[%ld]: %s\n", \
                       (long)vcount, _obj->name[vcount]) \
          } LATER_VERSIONS { \
            _obj->name[vcount] = (char*)bit_read_TU(dat); \
            LOG_TRACE_TU_I(#name, vcount, _obj->name[vcount], dxf) \
          } \
        } \
    }

#define FIELD_VECTOR(name, type, size, dxf) FIELD_VECTOR_N(name, type, _obj->size, dxf)

#define FIELD_2RD_VECTOR(name, size, dxf)                                   \
  _obj->name = (BITCODE_2RD *) malloc(_obj->size * sizeof(BITCODE_2RD));\
  for (vcount=0; vcount< (long)_obj->size; vcount++)\
    {\
      FIELD_2RD(name[vcount], dxf); \
    }

#define FIELD_2DD_VECTOR(name, size, dxf)                                   \
  _obj->name = (BITCODE_2RD *) malloc(_obj->size * sizeof(BITCODE_2RD));\
  FIELD_2RD(name[0], dxf);                                                  \
  for (vcount = 1; vcount < (long)_obj->size; vcount++)\
    {\
      FIELD_2DD(name[vcount], FIELD_VALUE(name[vcount - 1].x), FIELD_VALUE(name[vcount - 1].y), dxf); \
    }

#define FIELD_3DPOINT_VECTOR(name, size, dxf)                               \
  _obj->name = (BITCODE_3DPOINT *) malloc(_obj->size * sizeof(BITCODE_3DPOINT));\
  for (vcount=0; vcount < (long)_obj->size; vcount++) \
    {\
      FIELD_3DPOINT(name[vcount], dxf); \
    }

#define HANDLE_VECTOR_N(name, size, code, dxf) \
  FIELD_VALUE(name) = (BITCODE_H*) malloc(sizeof(BITCODE_H) * size);\
  for (vcount=0; vcount < (long)size; vcount++) \
    {\
      FIELD_HANDLE_N(name[vcount], vcount, code, dxf);  \
    }

#define HANDLE_VECTOR(name, sizefield, code, dxf) \
  HANDLE_VECTOR_N(name, FIELD_VALUE(sizefield), code, dxf)

//skip non-zero bytes and a terminating zero
#define FIELD_INSERT_COUNT(insert_count, type, dxf) \
      FIELD_VALUE(insert_count)=0; \
      while (bit_read_RC(dat)) \
        {\
          FIELD_VALUE(insert_count)++;\
        }\
      FIELD_G_TRACE(insert_count, type, dxf)

#define FIELD_XDATA(name, size)\
  _obj->name = dwg_decode_xdata(dat, _obj, _obj->size)

#define REACTORS(code)\
  FIELD_VALUE(reactors) = (BITCODE_H*) malloc(sizeof(BITCODE_H) * obj->tio.object->num_reactors);\
  for (vcount=0; vcount < (long)obj->tio.object->num_reactors; vcount++) \
    {\
      FIELD_HANDLE_N(reactors[vcount], vcount, code, -5);  \
    }

#define ENT_REACTORS(code)\
  FIELD_VALUE(reactors) = (BITCODE_H*) malloc(sizeof(BITCODE_H) * obj->tio.entity->num_reactors);\
  for (vcount=0; vcount < obj->tio.entity->num_reactors; vcount++)\
    {\
      FIELD_HANDLE_N(reactors[vcount], vcount, code, -5);  \
    }

#define XDICOBJHANDLE(code)\
  SINCE(R_2004)\
    {\
      if (!obj->tio.object->xdic_missing_flag)\
        {\
          FIELD_HANDLE(xdicobjhandle, code, 0);\
        }\
    }\
  PRIOR_VERSIONS\
    {\
      FIELD_HANDLE(xdicobjhandle, code, 0); \
    }

#define ENT_XDICOBJHANDLE(code)\
  SINCE(R_2004)\
    {\
      if (!obj->tio.entity->xdic_missing_flag)\
        {\
          FIELD_HANDLE(xdicobjhandle, code, 0); \
        }\
    }\
  PRIOR_VERSIONS\
    {\
      FIELD_HANDLE(xdicobjhandle, code, 0); \
    }

#define SECTION_STRING_STREAM \
  { \
    Bit_Chain sav_dat = *dat; \
    dat = str_dat;

#define START_STRING_STREAM \
  obj->has_strings = bit_read_B(dat); \
  if (obj->has_strings) { \
    Bit_Chain sav_dat = *dat; \
    obj_string_stream(dat, obj, dat);

#define END_STRING_STREAM \
    *dat = sav_dat; \
  }
/* just skip the has_strings bit */
#define START_HANDLE_STREAM \
  *hdl_dat = *dat; \
  if (dat->version >= R_2007) bit_set_position(hdl_dat, obj->hdlpos)

//TODO unify REPEAT macros
#define REPEAT_N(times, name, type) \
  if (times) _obj->name = (type *) calloc(times, sizeof(type)); \
  for (rcount=0; rcount<(long)times; rcount++)

#define REPEAT(times, name, type) \
  if (_obj->times) _obj->name = (type *) calloc(_obj->times, sizeof(type)); \
  for (rcount=0; rcount<(long)_obj->times; rcount++)

#define REPEAT2(times, name, type) \
  if (_obj->times) _obj->name = (type *) calloc(_obj->times, sizeof(type)); \
  for (rcount2=0; rcount2<(long)_obj->times; rcount2++)

#define REPEAT3(times, name, type) \
  if (_obj->times) _obj->name = (type *) calloc(_obj->times, sizeof(type)); \
  for (rcount3=0; rcount3<(long)_obj->times; rcount3++)

#define REPEAT4(times, name, type) \
  if (_obj->times) _obj->name = (type *) calloc(_obj->times, sizeof(type)); \
  for (rcount4=0; rcount4<(long)_obj->times; rcount4++)

#define COMMON_ENTITY_HANDLE_DATA \
  SINCE(R_13) {\
    START_HANDLE_STREAM; \
    dwg_decode_common_entity_handle_data(dat, hdl_dat, obj); \
  }

#define DWG_ENTITY(token) static void \
dwg_decode_##token (Bit_Chain* dat, Dwg_Object* obj)\
{\
  long vcount, rcount, rcount2, rcount3, rcount4;\
  Dwg_Entity_##token *ent, *_obj;\
  Dwg_Object_Entity *_ent;\
  Dwg_Data* dwg = obj->parent;\
  Bit_Chain* hdl_dat = dat; \
  Bit_Chain* str_dat; \
  if (dat->version >= R_2007) { \
    str_dat = malloc(sizeof(Bit_Chain)); /* seperate string buffer */ \
    *str_dat = *dat; \
  } else \
    str_dat = dat; \
  LOG_INFO("Entity " #token " ")\
  dwg->num_entities++;\
  obj->supertype = DWG_SUPERTYPE_ENTITY;\
  _ent = obj->tio.entity = (Dwg_Object_Entity*)calloc(1, sizeof(Dwg_Object_Entity));\
  obj->tio.entity->tio.token = (Dwg_Entity_##token *)calloc(1, sizeof (Dwg_Entity_##token));\
  ent = obj->tio.entity->tio.token;\
  _obj = ent;\
  _ent->object = obj;\
  if (dwg_decode_entity(dat, hdl_dat, str_dat, _ent)) return;

#define DWG_ENTITY_END }

#define DWG_OBJECT(token) static void \
dwg_decode_ ## token (Bit_Chain* dat, Dwg_Object* obj) \
{ \
  long vcount, rcount, rcount2, rcount3, rcount4; \
  Dwg_Object_##token *_obj;\
  Dwg_Data* dwg = obj->parent;\
  Bit_Chain* hdl_dat = dat; /* handle stream initially the same */ \
  Bit_Chain* str_dat; \
  if (dat->version >= R_2007) { \
    str_dat = calloc(1, sizeof(Bit_Chain)); /* seperate string buffer */ \
  } else \
    str_dat = dat; \
  LOG_INFO("Object " #token " ")\
  obj->supertype = DWG_SUPERTYPE_OBJECT;\
  obj->tio.object = (Dwg_Object_Object*)calloc (1, sizeof(Dwg_Object_Object)); \
  obj->tio.object->tio.token = (Dwg_Object_##token *)calloc (1, sizeof(Dwg_Object_##token)); \
  obj->tio.object->object = obj;\
  if (dwg_decode_object(dat, hdl_dat, str_dat, obj->tio.object)) return; \
  _obj = obj->tio.object->tio.token;

#define DWG_OBJECT_END }

