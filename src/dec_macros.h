/*--------------------------------------------------------------------------------
 * Welcome to the dark side of the moon...
 * MACROS
 */

#ifdef HAVE_CTYPE_H
#include <ctype.h>
#endif

#define ACTION decode
#define IS_DECODER

#define VALUE(value,type,dxf) \
  LOG_TRACE(FORMAT_##type " [" #type " %d]\n", value, dxf)
#define VALUE_RC(value,dxf) VALUE(value, RC, dxf)
#define VALUE_RS(value,dxf) VALUE(value, RS, dxf)
#define VALUE_RL(value,dxf) VALUE(value, RL, dxf)
#define VALUE_RD(value,dxf) VALUE(value, RD, dxf)

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
#define LOG_TF(level,var,len) \
  { int _i; \
    for (_i=0; _i<(len); _i++) { \
      LOG(level,"%02x", (unsigned char)((char*)var)[_i]); \
    } \
    LOG(level,"\n"); \
    if (DWG_LOGLEVEL >= DWG_LOGLEVEL_INSANE) { \
      for (_i=0; _i<(len); _i++) { \
        unsigned char c = ((unsigned char*)var)[_i]; \
        LOG_INSANE("%-2c", isprint(c) ? c : ' '); \
      } \
      LOG_INSANE("\n"); \
    } \
  }
#define LOG_TRACE_TF(var,len)  LOG_TF(TRACE,var,len)
#define LOG_INSANE_TF(var,len) LOG_TF(INSANE,var,len)
#define FIELD_2PT_TRACE(name, type, dxf) \
  LOG_TRACE(#name ": (" FORMAT_BD ", " FORMAT_BD ") [" #type " %d]\n", \
            _obj->name.x, _obj->name.y, dxf)
#define FIELD_3PT_TRACE(name, type, dxf) \
  LOG_TRACE(#name ": (" FORMAT_BD ", " FORMAT_BD ", " FORMAT_BD ") [" #type " %d]\n", \
            _obj->name.x, _obj->name.y, _obj->name.z, dxf)

#define FIELD_VALUE(name) _obj->name

#define ANYCODE -1
#define VALUE_HANDLE(handleptr, name, handle_code, dxf) \
  { \
    if (handle_code >= 0) \
      {\
        handleptr = dwg_decode_handleref_with_code(hdl_dat, obj, dwg, handle_code);\
      }\
    else\
      {\
        handleptr = dwg_decode_handleref(hdl_dat, obj, dwg);\
      }\
    if (handleptr)\
      {\
        LOG_TRACE(#name ": HANDLE(%x.%d.%lX) absolute:%lX [%d]\n", \
                  handleptr->handleref.code, \
                  handleptr->handleref.size, \
                  handleptr->handleref.value,\
                  handleptr->absolute_ref, dxf) \
      }\
    else \
      {\
        LOG_TRACE(#name ": NULL HANDLE(%x) [%d]\n", handle_code, dxf); \
      }\
  }
#define FIELD_HANDLE(name, handle_code, dxf) VALUE_HANDLE(_obj->name, name, handle_code, dxf)

#define VALUE_HANDLE_N(handleptr, name, vcount, handle_code, dxf) \
  {\
    if (handle_code >= 0) \
      {\
        handleptr = dwg_decode_handleref_with_code(hdl_dat, obj, dwg, handle_code);\
      }\
    else\
      {\
        handleptr = dwg_decode_handleref(hdl_dat, obj, dwg);\
      }\
    if (handleptr)\
      {\
        LOG_TRACE(#name "[%d]: %d HANDLE(%x.%d.%lX) absolute:%lX [%d]\n",  \
              (int)vcount, handle_code,   \
              handleptr->handleref.code,  \
              handleptr->handleref.size,  \
              handleptr->handleref.value, \
              handleptr->absolute_ref, dxf) \
      }\
    else \
      {\
        LOG_TRACE(#name ": NULL HANDLE(%x) [%d]\n", handle_code, dxf); \
      }\
  }
#define FIELD_HANDLE_N(name, vcount, handle_code, dxf) \
  VALUE_HANDLE_N(_obj->name, name, vcount, handle_code, dxf)

#define FIELD_DATAHANDLE(name, handle_code, dxf) \
  { \
    _obj->name = dwg_decode_handleref(dat, obj, dwg);\
    LOG_TRACE(#name ": HANDLE(%x.%d.%lX) absolute:%lX [%d]\n",\
              _obj->name->handleref.code,  \
              _obj->name->handleref.size,  \
              _obj->name->handleref.value, \
              _obj->name->absolute_ref, dxf);\
  }

#define FIELD_B(name,dxf) FIELDG(name, B, dxf)
#define FIELD_BB(name,dxf) FIELDG(name, BB, dxf)
#define FIELD_3B(name,dxf) FIELDG(name, 3B, dxf)
#define FIELD_BS(name,dxf) FIELDG(name, BS, dxf)
#define FIELD_BL(name,dxf) FIELDG(name, BL, dxf)
#define FIELD_BLL(name,dxf) FIELDG(name, BLL, dxf)
#define FIELD_BD(name,dxf) { \
  _obj->name = bit_read_BD(dat); \
  if (bit_isnan(_obj->name)) { \
    FIELD_G_TRACE(name,BD,dxf); \
    LOG_ERROR("Invalid BD " #name); \
    return DWG_ERR_VALUEOUTOFBOUNDS; \
  } \
  if (dxf >= 50 && dxf < 54) { \
    LOG_TRACE(#name ": " FORMAT_BD " [BD %d]  %gº\n", \
              _obj->name, dxf, rad2deg(_obj->name)); \
  } else { \
    FIELD_G_TRACE(name,BD,dxf); \
  } \
}
#define FIELD_BLx(name,dxf) \
  { _obj->name = bit_read_BL(dat); \
    LOG_TRACE(#name ": 0x%x [BL %d]\n", (uint32_t)_obj->name, dxf); }
#define FIELD_RLx(name,dxf) \
  { _obj->name = bit_read_RL(dat); \
    LOG_TRACE(#name ": 0x%x [RL %d]\n", (uint32_t)_obj->name, dxf); }
#define FIELD_BSx(name,dxf) \
  { _obj->name = bit_read_BS(dat); \
    LOG_TRACE(#name ": 0x%x [BS %d]\n", _obj->name, dxf); }
#define FIELD_RC(name,dxf) FIELDG(name, RC, dxf)
#define FIELD_RCu(name,dxf) \
  { _obj->name = bit_read_RC(dat); \
    LOG_TRACE(#name ": %u [RC %d]\n", (unsigned)((unsigned char)_obj->name), dxf); }
#define FIELD_RCd(name,dxf) \
  { _obj->name = bit_read_RC(dat); \
    LOG_TRACE(#name ": %d [RC %d]\n", (int)((signed char)_obj->name), dxf); }
#define FIELD_RS(name,dxf) FIELDG(name, RS, dxf)
#define FIELD_RD(name,dxf) { \
  FIELDG(name, RD, dxf); \
  if (bit_isnan(_obj->name)) { \
    LOG_ERROR("Invalid RD " #name); \
    return DWG_ERR_VALUEOUTOFBOUNDS; \
  } \
}
#define FIELD_RL(name,dxf)  FIELDG(name, RL, dxf)
#define FIELD_RLL(name,dxf) FIELDG(name, RLL, dxf)
#define FIELD_MC(name,dxf)  FIELDG(name, MC, dxf)
#define FIELD_MS(name,dxf)  FIELDG(name, MS, dxf)
#define FIELD_TF(name,len,dxf) \
  { VECTOR_CHKCOUNT(name,TF,len) \
   _obj->name = bit_read_TF(dat,(int)len); \
    LOG_INSANE( #name ": [%d TF " #dxf "]\n", len); \
    LOG_INSANE_TF(FIELD_VALUE(name), (int)len); }
#define FIELD_TFF(name,len,dxf) \
  { VECTOR_CHKCOUNT(name,TF,len) \
    bit_read_fixed(dat,_obj->name,(int)len); \
    LOG_INSANE( #name ": [%d TFF " #dxf "]\n", len); \
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
#define FIELD_BT(name,dxf) FIELDG(name, BT, dxf)
#define FIELD_4BITS(name,dxf) \
    { _obj->name = bit_read_4BITS(dat); \
      FIELD_G_TRACE(name,4BITS,dxf); }

#define FIELD_BE(name,dxf) \
    bit_read_BE(dat, &_obj->name.x, &_obj->name.y, &_obj->name.z)
#define FIELD_DD(name, _default, dxf) { \
  FIELD_VALUE(name) = bit_read_DD(dat, _default); \
  if (bit_isnan(_obj->name)) { \
    LOG_ERROR("Invalid DD " #name); \
    return DWG_ERR_VALUEOUTOFBOUNDS; \
  } \
}
#define FIELD_2DD(name, d1, d2, dxf) { \
    FIELD_DD(name.x, d1, dxf); FIELD_DD(name.y, d2, dxf+10); \
    FIELD_2PT_TRACE(name, DD, dxf); }
#define FIELD_3DD(name, def, dxf) { \
    FIELD_DD(name.x, FIELD_VALUE(def.x), dxf); \
    FIELD_DD(name.y, FIELD_VALUE(def.y), dxf+10); \
    FIELD_DD(name.z, FIELD_VALUE(def.z), dxf+20); \
    FIELD_3PT_TRACE(name, DD, dxf); }
#define FIELD_3RD(name,dxf) \
  { _obj->name.x = bit_read_RD(dat); \
    _obj->name.y = bit_read_RD(dat); \
    _obj->name.z = bit_read_RD(dat); \
    if (bit_isnan(_obj->name.x) || bit_isnan(_obj->name.y) || bit_isnan(_obj->name.z)) { \
      LOG_ERROR("Invalid 3RD " #name); \
      return DWG_ERR_VALUEOUTOFBOUNDS; \
    } \
    FIELD_3PT_TRACE(name,RD,dxf); }
#define FIELD_3BD(name,dxf) \
  { _obj->name.x = bit_read_BD(dat); \
    _obj->name.y = bit_read_BD(dat); \
    _obj->name.z = bit_read_BD(dat); \
    if (bit_isnan(_obj->name.x) || bit_isnan(_obj->name.y) || bit_isnan(_obj->name.z)) { \
      LOG_ERROR("Invalid 3BD " #name); \
      return DWG_ERR_VALUEOUTOFBOUNDS; \
    } \
    FIELD_3PT_TRACE(name,BD,dxf); }
#define FIELD_2RD(name,dxf) \
  { _obj->name.x = bit_read_RD(dat); \
    _obj->name.y = bit_read_RD(dat); \
    if (bit_isnan(_obj->name.x) || bit_isnan(_obj->name.y)) { \
      LOG_ERROR("Invalid 2RD " #name); \
      return DWG_ERR_VALUEOUTOFBOUNDS; \
    } \
    FIELD_2PT_TRACE(name,RD,dxf); }
#define FIELD_2BD(name,dxf) \
  { _obj->name.x = bit_read_BD(dat); \
    _obj->name.y = bit_read_BD(dat); \
    if (bit_isnan(_obj->name.x) || bit_isnan(_obj->name.y)) { \
      LOG_ERROR("Invalid 2BD " #name); \
      return DWG_ERR_VALUEOUTOFBOUNDS; \
    } \
    FIELD_2PT_TRACE(name,BD,dxf); }
#define FIELD_2BD_1(name,dxf) \
  { _obj->name.x = bit_read_BD(dat); \
    _obj->name.y = bit_read_BD(dat); \
    if (bit_isnan(_obj->name.x) || bit_isnan(_obj->name.y)) { \
      LOG_ERROR("Invalid 2BD_1 " #name); \
      return DWG_ERR_VALUEOUTOFBOUNDS; \
    } \
    FIELD_2PT_TRACE(name,BD,dxf); }
// FIELDG(name.x, BD, dxf); FIELDG(name.y, BD, dxf+1);
#define FIELD_3BD_1(name,dxf) \
  { _obj->name.x = bit_read_BD(dat); \
    _obj->name.y = bit_read_BD(dat); \
    _obj->name.z = bit_read_BD(dat); \
    if (bit_isnan(_obj->name.x) || bit_isnan(_obj->name.y) || bit_isnan(_obj->name.z)) { \
      LOG_ERROR("Invalid 3BD_1 " #name); \
      return DWG_ERR_VALUEOUTOFBOUNDS; \
    } \
    FIELD_3PT_TRACE(name,BD,dxf); }
//    FIELDG(name.x, BD, dxf); FIELDG(name.y, BD, dxf+1);
//    FIELDG(name.z, BD, dxf+2); }
#define FIELD_3DPOINT(name,dxf)  FIELD_3BD(name,dxf)
#define FIELD_3DVECTOR(name,dxf) FIELD_3BD_1(name,dxf)
#define FIELD_TIMEBLL(name,dxf) \
  { _obj->name = bit_read_TIMEBLL(dat); \
    LOG_TRACE(#name ": %.8f  (" FORMAT_BL ", " FORMAT_BL ") [TIMEBLL %d]\n", \
              _obj->name.value, _obj->name.days, _obj->name.ms, dxf); }
#define FIELD_CMC(color,dxf1,dxf2) \
  { bit_read_CMC(dat, &_obj->color); \
    LOG_TRACE(#color ".index: %d [CMC.BS %d]\n", _obj->color.index, dxf1); \
    if (dat->version >= R_2004) { \
      LOG_TRACE(#color ".rgb: 0x%06x [CMC.BL %d]\n", (unsigned)_obj->color.rgb, dxf2); \
      LOG_TRACE(#color ".flag: 0x%x [CMC.RC]\n", (unsigned)_obj->color.flag); \
      if (_obj->color.flag & 1) \
        LOG_TRACE(#color ".name: %s [CMC.TV]\n", _obj->color.name); \
      if (_obj->color.flag & 2) \
        LOG_TRACE(#color ".bookname: %s [CMC.TV]\n", _obj->color.book_name); \
    }\
  }
#define FIELD_EMC(color,dxf1,dxf2) \
  { bit_read_EMC(dat, &_obj->color); \
    LOG_TRACE(#color ".index: %d [EMC.BS %d]\n", _obj->color.index, dxf1); \
    if (dat->version >= R_2004) { \
      if (_obj->color.flag) \
        LOG_TRACE(#color ".flag: 0x%x\n", (unsigned)_obj->color.flag); \
      if (_obj->color.flag & 0x20) \
        LOG_TRACE(#color ".alpha: 0x%06x [EMC.BL %d]\n", (unsigned)_obj->color.alpha, dxf2+20); \
      if (_obj->color.flag & 0x80) \
        LOG_TRACE(#color ".rgb: 0x%06x [EMC.BL %d]\n", (unsigned)_obj->color.rgb, dxf2); \
      if ((_obj->color.flag & 0x41) == 0x41) \
        LOG_TRACE(#color ".name: %s [EMC.TV %d]\n", _obj->color.name, dxf2+10);    \
      if ((_obj->color.flag & 0x42) == 0x42) \
        LOG_TRACE(#color ".bookname: %s [EMC.TV]\n", _obj->color.book_name); \
    }\
  }

#undef DEBUG_POS
#undef DEBUG_HERE
#undef DEBUG_POS_OBJ
#undef DEBUG_HERE_OBJ
#define DEBUG_POS_OBJ\
  if (DWG_LOGLEVEL >= DWG_LOGLEVEL_TRACE) { \
    LOG_TRACE("DEBUG_POS @%u.%u (%lu) %lu\n", (unsigned int)dat->byte, dat->bit, \
              bit_position(dat), obj ? bit_position(dat) - obj->address*8 : 0); \
  }
#define DEBUG_POS\
  if (DWG_LOGLEVEL >= DWG_LOGLEVEL_TRACE) { \
    LOG_TRACE("DEBUG_POS @%u.%u (%lu)\n", (unsigned int)dat->byte, dat->bit, \
              bit_position(dat)); \
  }
#define _DEBUG_HERE\
  if (DWG_LOGLEVEL >= DWG_LOGLEVEL_TRACE) { \
    Bit_Chain here = *dat; \
    int oldloglevel = loglevel; \
    char *tmp; BITCODE_BB bb = 0; BITCODE_RS rs; BITCODE_RL rl; double bd; \
    Dwg_Handle hdl; \
    tmp = bit_read_TF(dat, 24);\
    if (DWG_LOGLEVEL >= DWG_LOGLEVEL_INSANE) { \
      bit_fprint_bits(stderr, (unsigned char*)tmp, 68); fprintf(stderr,"\n"); \
    } \
    LOG_TRACE_TF(tmp, 24);\
    free(tmp); \
    SINCE(R_13) {\
      *dat = here;\
      LOG_TRACE("  B  :"FORMAT_B"\t", bit_read_B(dat));\
      *dat = here; bb = bit_read_BB(dat) & 0x3;\
      LOG_TRACE("  BB :"FORMAT_BB"\n", bb);\
    }\
    *dat = here; rs = bit_read_RS(dat);                \
    LOG_TRACE("  RS :"FORMAT_RS" / 0x%04x (16)\n", rs, rs); \
    SINCE(R_13) {\
      *dat = here; rs = bit_read_BS(dat); \
      LOG_TRACE("  BS :"FORMAT_BS" / 0x%04x (%ld)\t", rs, rs, \
                bit_position(dat)-bit_position(&here)); \
     }\
    SINCE(R_2007) {\
      *dat = here; rs = bit_read_MS(dat);              \
      LOG_TRACE("  MS :"FORMAT_RS" / 0x%04x (%ld)\n", rs, rs, \
                bit_position(dat)-bit_position(&here)); \
    } else LOG_TRACE("\n"); \
    *dat = here; rl = bit_read_RL(dat);  \
    LOG_TRACE("  RL :"FORMAT_RL " / 0x%08x (32)\n", rl, rl);\
    *dat = here;\
    LOG_TRACE("  RD :"FORMAT_RD " (64)\n", bit_read_RD(dat));\
    *dat = here; \
    SINCE(R_13) {\
      if (bb != 3) { rl = bit_read_BL(dat);                     \
        LOG_TRACE("  BL :"FORMAT_BL " / 0x%08x (%ld)\n", rl, rl,\
                  bit_position(dat)-bit_position(&here));       \
        *dat = here; bd = bit_read_BD(dat);                     \
        LOG_TRACE("  BD :"FORMAT_BD " (%ld)\n", bd,\
                  bit_position(dat)-bit_position(&here)); \
        *dat = here;                                            \
      }                                                         \
    }                                                           \
    if ((dat->chain[dat->byte] & 0xf) <= 4) {                   \
      loglevel = 0;                                             \
      if (!bit_read_H(dat, &hdl)) {                             \
        LOG_TRACE("  H :(%d.%d.%lX) (%ld)\n", hdl.code, hdl.size,\
                  hdl.value, bit_position(dat)-bit_position(&here)); \
      }                                                         \
      loglevel = oldloglevel;                                   \
    }                                                           \
    *dat = here;                                                \
  }
#define DEBUG_HERE_OBJ\
  DEBUG_POS_OBJ\
  _DEBUG_HERE
#define DEBUG_HERE\
  DEBUG_POS\
  _DEBUG_HERE

//check for overflow into next object (invalid num_elems)
#define AVAIL_BITS() \
  (obj ? (long)((obj->address + obj->size)*8 - bit_position(dat) + 20) \
       : 0xff00L)
#define TYPE_MAXELEMSIZE(type) (unsigned)dwg_bits_size[BITS_##type]
#define VECTOR_CHKCOUNT(name,type,size) \
  if ((long)((size)*TYPE_MAXELEMSIZE(type)) > AVAIL_BITS()) {           \
    LOG_ERROR("Invalid " #name " vcount %ld. Need min. %u bits for %s, have %ld for %s.", \
              (long)(size), (size)*TYPE_MAXELEMSIZE(type), #type, AVAIL_BITS(), obj?obj->dxfname:""); \
    return DWG_ERR_VALUEOUTOFBOUNDS; }
#define _VECTOR_CHKCOUNT(name,size,maxelemsize) \
  if ((long)(size)*(maxelemsize) > AVAIL_BITS()) { \
    LOG_ERROR("Invalid " #name " vcount %ld. Need min. %u bits, have %ld for %s.", \
              (long)(size), (unsigned)(size)*(maxelemsize), AVAIL_BITS(), obj?obj->dxfname:""); \
    size = 0; \
    return DWG_ERR_VALUEOUTOFBOUNDS; }
#define HANDLE_VECTOR_CHKCOUNT(name,size) \
  VECTOR_CHKCOUNT(name,HANDLE,size)


//FIELD_VECTOR_N(name, type, size):
// reads data of the type indicated by 'type' 'size' times and stores
// it all in the vector called 'name'.
#define FIELD_VECTOR_N(name, type, size, dxf) \
  if (size > 0) \
    { \
      VECTOR_CHKCOUNT(name,type,size) \
      _obj->name = (BITCODE_##type*) calloc(size, sizeof(BITCODE_##type)); \
      for (vcount=0; vcount<(BITCODE_BL)size; vcount++) \
        {\
          _obj->name[vcount] = bit_read_##type(dat); \
          LOG_INSANE(#name "[%ld]: " FORMAT_##type "\n", \
                     (long)vcount, _obj->name[vcount]) \
        } \
    }
#define FIELD_VECTOR_T(name, size, dxf) \
  if (_obj->size > 0) \
    { \
      _VECTOR_CHKCOUNT(name,_obj->size,dat->version>=R_2007 ? 18 : 2) \
      _obj->name = calloc(_obj->size, sizeof(char*)); \
      for (vcount=0; vcount<(BITCODE_BL)_obj->size; vcount++) \
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
#define FIELD_VECTOR_N1(name, type, size, dxf) \
  if (size > 0) \
    { \
      int _dxf = dxf;\
      VECTOR_CHKCOUNT(name,type,size) \
      _obj->name = (BITCODE_##type*) calloc(size, sizeof(BITCODE_##type)); \
      for (vcount=0; vcount<(BITCODE_BL)size; vcount++) \
        {\
          _obj->name[vcount] = bit_read_##type(dat); \
          LOG_INSANE(#name "[%ld]: " FORMAT_##type " [" #type " %d]\n", \
                     (long)vcount, _obj->name[vcount], _dxf++) \
        } \
    }

#define FIELD_VECTOR(name, type, size, dxf) FIELD_VECTOR_N(name, type, _obj->size, dxf)

#define FIELD_2RD_VECTOR(name, size, dxf) \
  VECTOR_CHKCOUNT(name,2RD,_obj->size) \
  _obj->name = (BITCODE_2RD *) calloc(_obj->size, sizeof(BITCODE_2RD)); \
  for (vcount=0; vcount< (BITCODE_BL)_obj->size; vcount++)\
    {\
      FIELD_2RD(name[vcount], dxf); \
    }

#define FIELD_2DD_VECTOR(name, size, dxf) \
  VECTOR_CHKCOUNT(name,2DD,_obj->size) \
    _obj->name = (BITCODE_2RD *) calloc(_obj->size, sizeof(BITCODE_2RD)); \
  FIELD_2RD(name[0], dxf); \
  for (vcount = 1; vcount < (BITCODE_BL)_obj->size; vcount++)\
    {\
      FIELD_DD(name[vcount].x, FIELD_VALUE(name[vcount - 1].x), dxf); \
      FIELD_DD(name[vcount].y, FIELD_VALUE(name[vcount - 1].x), dxf+10);\
      LOG_TRACE(#name "[%ld]: (" FORMAT_BD ", " FORMAT_BD ") [DD %d]\n", \
                (long)vcount, _obj->name[vcount].x, _obj->name[vcount].y, dxf) \
    }

#define FIELD_3DPOINT_VECTOR(name, size, dxf) \
  VECTOR_CHKCOUNT(name,3BD,_obj->size) \
  _obj->name = (BITCODE_3DPOINT *) calloc(_obj->size, sizeof(BITCODE_3DPOINT)); \
  for (vcount=0; vcount < (BITCODE_BL)_obj->size; vcount++) \
    {\
      FIELD_3DPOINT(name[vcount], dxf); \
    }

// shortest handle: 8 bit
#define HANDLE_VECTOR_N(name, size, code, dxf) \
  VECTOR_CHKCOUNT(name,HANDLE,size) \
    FIELD_VALUE(name) = (BITCODE_H*) calloc(size, sizeof(BITCODE_H)); \
  for (vcount=0; vcount < (BITCODE_BL)size; vcount++) \
    {\
      FIELD_HANDLE_N(name[vcount], vcount, code, dxf);  \
    }

#define HANDLE_VECTOR(name, sizefield, code, dxf) \
  HANDLE_VECTOR_N(name, FIELD_VALUE(sizefield), code, dxf)

//skip non-zero bytes and a terminating zero
#define FIELD_NUM_INSERTS(num_inserts, type, dxf) \
      FIELD_VALUE(num_inserts)=0; \
      while (bit_read_RC(dat)) \
        {\
          FIELD_VALUE(num_inserts)++;\
        }\
      FIELD_G_TRACE(num_inserts, type, dxf)

#define FIELD_XDATA(name, size)\
  _obj->name = dwg_decode_xdata(dat, _obj, _obj->size)

#define REACTORS(code)\
  obj->tio.object->reactors = calloc(obj->tio.object->num_reactors, sizeof(BITCODE_H)); \
  for (vcount=0; vcount < obj->tio.object->num_reactors; vcount++) \
    {\
      VALUE_HANDLE_N(obj->tio.object->reactors[vcount], reactors, vcount, code, 330); \
    }

#define ENT_REACTORS(code)\
  _ent->reactors = calloc(_ent->num_reactors, sizeof(BITCODE_H)); \
  for (vcount=0; vcount < _ent->num_reactors; vcount++)\
    {\
      VALUE_HANDLE_N(_ent->reactors[vcount], reactors, vcount, code, 330); \
    }

#define XDICOBJHANDLE(code)\
  SINCE(R_2004)\
    {\
      if (!obj->tio.object->xdic_missing_flag)\
        {\
          VALUE_HANDLE(obj->tio.object->xdicobjhandle, xdicobjhandle, code, 360);\
        }\
    }\
  else\
    {\
      SINCE(R_13)\
        {\
          VALUE_HANDLE(obj->tio.object->xdicobjhandle, xdicobjhandle, code, 360);\
        }\
    }

#define ENT_XDICOBJHANDLE(code)\
  SINCE(R_2004)\
    {\
      if (!_ent->xdic_missing_flag)\
        {\
          VALUE_HANDLE(_ent->xdicobjhandle, xdicobjhandle, code, 360); \
        }\
    }\
  else { \
    SINCE(R_13) \
    {\
      VALUE_HANDLE(_ent->xdicobjhandle, xdicobjhandle, code, 360); \
    }\
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
  if (dat->version >= R_2007) { \
    vcount = bit_position(dat); \
    if (obj->hdlpos != (unsigned long)vcount) { \
      bit_set_position(hdl_dat, obj->hdlpos); \
      LOG_HANDLE(" handle stream: %+ld @%lu.%u %s\n", obj->hdlpos - vcount, \
        dat->byte, dat->bit, \
        ((long)obj->hdlpos - (long)vcount) >= 8 ? "MISSING" \
          : ((long)obj->hdlpos < (long)vcount) ? "OVERSHOOT" : ""); \
    } \
  }

#define REPEAT_CHKCOUNT(name,times,type) \
  if (AVAIL_BITS() < 0) { \
    LOG_ERROR("Invalid " #name " in %s. No bytes left.\n", obj->dxfname); \
    return DWG_ERR_VALUEOUTOFBOUNDS; \
  } \
  LOG_INSANE("REPEAT_CHKCOUNT %s." #name " x %ld: %ld > %ld?\n", \
    obj->dxfname, (long)times, (long)((times)*sizeof(type)), AVAIL_BITS()); \
  if (dat->version >= R_2004 && (long)((times)*sizeof(type)) > AVAIL_BITS()) { \
    LOG_ERROR("Invalid %s." #name " x %ld\n", obj->dxfname, (long)times); \
    return DWG_ERR_VALUEOUTOFBOUNDS; }
#define REPEAT_CHKCOUNT_LVAL(name,times,type) \
  if (AVAIL_BITS() < 0) { \
    LOG_ERROR("Invalid %s." #name ". No bytes left.\n", obj->dxfname); \
    times = 0; \
    return DWG_ERR_VALUEOUTOFBOUNDS; \
  } \
  LOG_INSANE("REPEAT_CHKCOUNT_LVAL %s." #name " x %ld: %ld > %ld?\n", \
    obj->dxfname, (long)times, (long)((times)*sizeof(type)), AVAIL_BITS()); \
  if (dat->version >= R_2004 && (long)((times)*sizeof(type)) > AVAIL_BITS()) { \
    LOG_ERROR("Invalid %s." #name " x %ld\n", obj->dxfname, (long)times); \
    times = 0; \
    return DWG_ERR_VALUEOUTOFBOUNDS; }

// unchecked with a constant
#define REPEAT_CN(times, name, type) \
  if (times) _obj->name = (type *) calloc(times, sizeof(type)); \
  for (rcount1=0; rcount1<(BITCODE_BL)times; rcount1++)
#define REPEAT_N(times, name, type) \
  REPEAT_CHKCOUNT(name,times,type) \
  if (times) _obj->name = (type *) calloc(times, sizeof(type)); \
  for (rcount1=0; rcount1<(BITCODE_BL)times; rcount1++)

#define _REPEAT(times, name, type, idx) \
  REPEAT_CHKCOUNT_LVAL(name,_obj->times,type) \
  if (_obj->times) _obj->name = (type *) calloc(_obj->times, sizeof(type)); \
  for (rcount##idx=0; rcount##idx<(BITCODE_BL)_obj->times; rcount##idx++)
#define _REPEAT_C(times, name, type, idx) \
  REPEAT_CHKCOUNT_LVAL(name,_obj->times,type) \
  if (_obj->times) _obj->name = (type *) calloc(_obj->times, sizeof(type)); \
  for (rcount##idx=0; rcount##idx<(BITCODE_BL)_obj->times; rcount##idx++)
#define _REPEAT_N(times, name, type, idx) \
  if (_obj->name) \
    for (rcount##idx=0; rcount##idx<(BITCODE_BL)times; rcount##idx++)

#define REPEAT(times, name, type)   _REPEAT(times, name, type, 1)
#define REPEAT2(times, name, type)  _REPEAT(times, name, type, 2)
#define REPEAT3(times, name, type)  _REPEAT(times, name, type, 3)
#define REPEAT4(times, name, type)  _REPEAT(times, name, type, 4)

#define REPEAT_C(times, name, type)   _REPEAT_C(times, name, type, 1)
#define REPEAT2_C(times, name, type)  _REPEAT_C(times, name, type, 2)
#define REPEAT3_C(times, name, type)  _REPEAT_C(times, name, type, 3)
#define REPEAT4_C(times, name, type)  _REPEAT_C(times, name, type, 4)

#define COMMON_ENTITY_HANDLE_DATA \
  SINCE(R_13) {\
    START_HANDLE_STREAM; \
    error |= dwg_decode_common_entity_handle_data(dat, hdl_dat, obj); \
  }

/** Add the empty entity or object with its three structs to the DWG.
    All fields are zero'd. TODO: some are initialized with default values, as
    defined in dwg.spec.
    Returns 0 or DWG_ERR_OUTOFMEM.
*/

#define DWG_ENTITY(token) \
EXPORT int dwg_add_##token (Dwg_Object *obj) \
{ \
  Dwg_Object_Entity *_ent; \
  Dwg_Entity_##token *_obj; \
  LOG_INFO("Add entity " #token " ")\
  obj->parent->num_entities++;\
  obj->supertype = DWG_SUPERTYPE_ENTITY;\
  obj->fixedtype = DWG_TYPE_##token;\
  _ent = obj->tio.entity = calloc(1, sizeof(Dwg_Object_Entity));\
  if (!_ent) return DWG_ERR_OUTOFMEM; \
  _ent->tio.token = calloc(1, sizeof (Dwg_Entity_##token));\
  if (!_ent->tio.token) return DWG_ERR_OUTOFMEM; \
  obj->dxfname = (char*)#token; \
  _ent->dwg = obj->parent; \
  _ent->objid = obj->index; /* obj ptr itself might move */ \
  _ent->tio.token->parent = obj->tio.entity;\
  return 0; \
} \
/**Call dwg_add_##token and write the fields from the bitstream dat to the entity or object. */ \
static int dwg_decode_##token (Bit_Chain *restrict dat, Dwg_Object *restrict obj) \
{ \
  BITCODE_BL vcount, rcount1, rcount2, rcount3, rcount4; \
  Dwg_Entity_##token *ent, *_obj; \
  Dwg_Object_Entity *_ent; \
  Dwg_Data* dwg = obj->parent; \
  Bit_Chain* hdl_dat = dat; \
  Bit_Chain* str_dat; \
  int error = dwg_add_##token(obj); \
  if (error) return error; \
  if (dat->version >= R_2007) { \
    str_dat = calloc(1, sizeof(Bit_Chain)); /* separate string buffer */ \
    if (!str_dat) return DWG_ERR_OUTOFMEM; \
    *str_dat = *dat; \
  } else \
    str_dat = dat; \
  LOG_INFO("Decode entity " #token " ")\
  _ent = obj->tio.entity; \
  ent = obj->tio.entity->tio.token;\
  _obj = ent;\
  _ent->dwg = dwg; \
  _ent->objid = obj->index; /* obj ptr itself might move */ \
  _obj->parent = obj->tio.entity;\
  if (dat->version >= R_13) { \
    error = dwg_decode_entity(dat, hdl_dat, str_dat, _ent); \
  } else { \
    error = decode_entity_preR13(dat, obj, _ent); \
  } \
  if (error >= DWG_ERR_CRITICAL) return error;

// Does size include the CRC?
#define DWG_ENTITY_END \
  if (dat->version >= R_2007) { \
    free(str_dat); \
    vcount  = (obj->size+obj->address)*8 - bit_position(hdl_dat); \
  } else { \
    vcount  = (obj->size+obj->address)*8 - bit_position(dat); \
  } \
  if (vcount) \
    LOG_HANDLE(" padding: %+ld %s\n", (long)vcount, (BITCODE_BLd)vcount >= 8 \
               ? "MISSING" \
               : ((BITCODE_BLd)vcount < 0) ? "OVERSHOOT" : "");  \
  return error & ~DWG_ERR_UNHANDLEDCLASS; \
}

#define DWG_OBJECT(token) \
EXPORT int dwg_add_ ## token (Dwg_Object *obj) \
{ \
  Dwg_Object_##token *_obj;\
  LOG_INFO("Add object " #token " ")\
  obj->supertype = DWG_SUPERTYPE_OBJECT;\
  obj->fixedtype = DWG_TYPE_##token;\
  obj->tio.object = calloc (1, sizeof(Dwg_Object_Object)); \
  if (!obj->tio.object) return DWG_ERR_OUTOFMEM; \
  _obj = obj->tio.object->tio.token = calloc (1, sizeof(Dwg_Object_##token)); \
  if (!_obj) return DWG_ERR_OUTOFMEM; \
  obj->dxfname = (char*)#token; \
  _obj->parent = obj->tio.object; \
  obj->tio.object->dwg = obj->parent; \
  obj->tio.object->objid = obj->index; /* obj ptr itself might move */ \
  return 0; \
} \
static int dwg_decode_ ## token (Bit_Chain *restrict dat, Dwg_Object *restrict obj) \
{ \
  BITCODE_BL vcount, rcount1, rcount2, rcount3, rcount4; \
  Dwg_Object_##token *_obj;\
  Dwg_Data* dwg = obj->parent;\
  Bit_Chain* hdl_dat = dat; /* handle stream initially the same */ \
  Bit_Chain* str_dat; \
  int error = dwg_add_##token(obj); \
  if (error) return error; \
  if (dat->version >= R_2007) { \
    str_dat = calloc(1, sizeof(Bit_Chain)); /* separate string buffer */ \
    if (!str_dat) return DWG_ERR_OUTOFMEM; \
  } else \
    str_dat = dat; \
  LOG_INFO("Decode object " #token " ")\
  _obj = obj->tio.object->tio.token;\
  error |= dwg_decode_object(dat, hdl_dat, str_dat, obj->tio.object); \
  if (error >= DWG_ERR_CRITICAL) return error;

#define DWG_OBJECT_END DWG_ENTITY_END
