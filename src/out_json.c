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

/*
 * out_json.c: write as JSON
 * written by Reini Urban
 */

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifdef HAVE_ALLOCA_H
#include <alloca.h>
#define HAVE_ALLOCA
#elif defined(_WIN32) && defined(HAVE_MALLOC_H)
#include <malloc.h>
#define alloca(s) _alloca(s)
#define HAVE_ALLOCA
#endif

#include "common.h"
#include "bits.h"
#include "dwg.h"
#include "decode.h"
#include "out_json.h"

#define DWG_LOGLEVEL DWG_LOGLEVEL_NONE
#include "logging.h"

/* the current version per spec block */
static unsigned int cur_ver = 0;
/* see also examples/unknown.c */
static char* cquote(char *restrict dest, const char *restrict src);
#ifdef HAVE_NATIVE_WCHAR2
static wchar_t* wcquote(wchar_t *restrict dest, const wchar_t *restrict src);
#endif
static void  print_wcquote(Bit_Chain *restrict dat, dwg_wchar_t *restrict wstr);

#ifndef HAVE_ALLOCA
static inline char* alloca(size_t size);
static inline char* alloca(size_t size) {
  return malloc(size);
}
#endif

/*--------------------------------------------------------------------------------
 * MACROS
 */

#define ACTION json
#define IS_PRINT
#define IS_JSON

#define PREFIX   _prefix(dat);
#define ARRAY    fprintf (dat->fh, "[\n"); dat->bit++
#define ENDARRAY fprintf (dat->fh, "\n"); dat->bit--; PREFIX fprintf (dat->fh, "],\n")
#define LASTENDARRAY fprintf (dat->fh, "\n"); dat->bit--; PREFIX fprintf (dat->fh, "]\n")
#define KEYs(nam) PREFIX fprintf (dat->fh, "\"%s\": ", nam)
#define KEY(nam) PREFIX fprintf (dat->fh, "\"%s\": ", #nam)
#define HASH     PREFIX fprintf (dat->fh, "{\n"); dat->bit++
#define ENDHASH  fprintf (dat->fh, "\n"); dat->bit--; PREFIX fprintf (dat->fh, "},\n")
#define LASTENDHASH fprintf (dat->fh, "\n"); dat->bit--; PREFIX fprintf (dat->fh, "}")
#define NOCOMMA  fseek(dat->fh, -2, SEEK_CUR)

#define TABLE(nam) KEY(nam); HASH
#define ENDTAB()    NOCOMMA; ENDHASH
// a namd hash
#define RECORD(nam) KEY(nam); HASH
#define ENDRECORD()  NOCOMMA; ENDHASH
// a named list
#define SECTION(nam) PREFIX fprintf (dat->fh, "\"%s\": [\n", #nam); dat->bit++;
#define ENDSEC()      ENDARRAY

#undef  FORMAT_RC
#define FORMAT_RC "%d"
#define VALUE(value,type,dxf) \
  fprintf(dat->fh, FORMAT_##type ",\n", value)
#define VALUE_B(value,dxf)  VALUE(value, B, dxf)
#define VALUE_RC(value,dxf) VALUE(value, RC, dxf)
#define VALUE_RS(value,dxf) VALUE(value, RS, dxf)
#define VALUE_RL(value,dxf) VALUE(value, RL, dxf)
#define VALUE_RD(value,dxf) VALUE(value, RD, dxf)
#define VALUE_2RD(pt,dxf) \
    fprintf(dat->fh, "[ %f, %f ],\n", pt.x, pt.y)
#define VALUE_2DD(pt,d1,d2,dxf) VALUE_2RD(pt,dxf)
#define VALUE_3RD(pt,dxf) \
  fprintf(dat->fh, "[ %f, %f, %f ],\n", pt.x, pt.y, pt.z)
#define VALUE_3BD(pt,dxf) VALUE_3RD(pt,dxf)
#define VALUE_TV(nam,dxf)

#define FIELD(nam,type,dxf) \
  { PREFIX fprintf(dat->fh, "\"" #nam "\": " FORMAT_##type ",\n", _obj->nam); }
#define _FIELD(nam,type,value) \
  { PREFIX fprintf(dat->fh, "\"" #nam "\": " FORMAT_##type ",\n", obj->nam); }
#define ENT_FIELD(nam,type,value) \
  { PREFIX fprintf(dat->fh, "\"" #nam "\": " FORMAT_##type ",\n", _ent->nam); }
#define SUB_FIELD(o,nam,type,dxf) \
  { PREFIX fprintf(dat->fh, "\"" #nam "\": " FORMAT_##type ",\n", _obj->o.nam); }

#define FIELD_CAST(nam,type,cast,dxf) FIELD(nam,cast,dxf)
#define FIELD_TRACE(nam,type)
#define FIELD_G_TRACE(nam,type,dxf)
#ifdef HAVE_ALLOCA
#define FIELD_TEXT(nam,str) \
  { \
    PREFIX \
    if (str && (strchr(str, '"') || strchr(str, '\\'))) { \
      char *_buf = alloca(2*strlen(str)); \
      fprintf(dat->fh, "\"" #nam "\": \"%s\",\n", cquote(_buf, str)); \
    } else { \
      fprintf(dat->fh, "\"" #nam "\": \"%s\",\n", str ? str : ""); \
    } \
  }
#else
#define FIELD_TEXT(nam,str) \
  { \
    PREFIX \
    if (str && (strchr(str, '"') || strchr(str, '\\'))) { \
      char *_buf = malloc(2*strlen(str)); \
      fprintf(dat->fh, "\"" #nam "\": \"%s\",\n", cquote(_buf, str)); \
      free(_buf); \
    } else { \
      fprintf(dat->fh, "\"" #nam "\": \"%s\",\n", str ? str : ""); \
    } \
  }
#endif

#ifdef HAVE_NATIVE_WCHAR2
# define VALUE_TEXT_TU(wstr) \
    if (wstr && (wcschr(wstr, L'"') || wcschr(wstr, L'\\'))) { \
      wchar_t *_buf = malloc(2*wcslen(wstr)); \
      fprintf(dat->fh, "\"%ls\",\n", wcquote(_buf, wstr)); \
      free(_buf); \
    } else { \
      fprintf(dat->fh, "\"%ls\",\n", wstr ? (wchar_t*)wstr : L""); \
    }
#else
# define VALUE_TEXT_TU(wstr) print_wcquote(dat, (BITCODE_TU)wstr)
#endif
#define FIELD_TEXT_TU(nam, wstr) \
  KEY(nam); VALUE_TEXT_TU((BITCODE_TU)wstr)

#define FIELD_VALUE(nam) _obj->nam
#define ANYCODE -1
// todo: only the name, not the ref
#define VALUE_HANDLE(hdlptr, nam, handle_code, dxf)     \
  if (hdlptr) { \
    fprintf(dat->fh, "\"%d.%d.%lu\",\n", \
            hdlptr->handleref.code, \
            hdlptr->handleref.size, \
            hdlptr->handleref.value); \
  } else { fprintf(dat->fh, "\"0.0.0\",\n"); }
#define VALUE_H(hdl, dxf) \
  fprintf(dat->fh, "\"%d.%d.%lu\",\n", \
            (hdl).code, \
            (hdl).size, \
            (hdl).value)
#define FIELD_HANDLE(nam, handle_code, dxf) \
  PREFIX if (_obj->nam) { \
    fprintf(dat->fh, "\"" #nam "\": \"%d.%d.%lu\",\n", \
            _obj->nam->handleref.code, \
            _obj->nam->handleref.size, \
            _obj->nam->handleref.value); \
  } else { fprintf(dat->fh, "\"" #nam "\": \"0.0.0\",\n"); }
#define SUB_FIELD_HANDLE(o,nam,handle_code, dxf) \
  PREFIX if (_obj->o.nam) { \
    fprintf(dat->fh, "\"" #nam "\": \"%d.%d.%lu\",\n", \
            _obj->o.nam->handleref.code, \
            _obj->o.nam->handleref.size, \
            _obj->o.nam->handleref.value); \
  } else { fprintf(dat->fh, "\"" #nam "\": \"0.0.0\",\n"); }
#define FIELD_DATAHANDLE(nam, code, dxf) FIELD_HANDLE(nam, code, dxf)
#define FIELD_HANDLE_N(nam, vcount, handle_code, dxf) \
  PREFIX if (_obj->nam) { \
    fprintf(dat->fh, "\"%d.%d.%lu\",\n", \
            _obj->nam->handleref.code, \
            _obj->nam->handleref.size, \
            _obj->nam->handleref.value); \
  } else {\
    fprintf(dat->fh, "\"0.0.0\",\n"); \
  }
#define FIELD_BINARY(nam,size,dxf) \
{ \
  long len = size; \
  KEY(nam); \
  fprintf(dat->fh, "\""); \
  if (_obj->nam) { \
    for (long j=0; j < len; j++) { \
      fprintf(dat->fh, "%02X", _obj->nam[j]); \
    } \
  } \
  fprintf(dat->fh, "\",\n"); \
}

#define FIELD_B(nam,dxf)   FIELD(nam, B, dxf)
#define FIELD_BB(nam,dxf)  FIELD(nam, BB, dxf)
#define FIELD_3B(nam,dxf)  FIELD(nam, 3B, dxf)
#define FIELD_BS(nam,dxf)  FIELD(nam, BS, dxf)
#define FIELD_BL(nam,dxf)  FIELD(nam, BL, dxf)
#define FIELD_BLL(nam,dxf) FIELD(nam, BLL, dxf)
#define FIELD_BD(nam,dxf)  FIELD(nam, BD, dxf)
#define FIELD_RC(nam,dxf)  FIELD(nam, RC, dxf)
#define FIELD_RS(nam,dxf)  FIELD(nam, RS, dxf)
#define FIELD_RD(nam,dxf)  FIELD(nam, RD, dxf)
#define FIELD_RL(nam,dxf)  FIELD(nam, RL, dxf)
#define FIELD_RLL(nam,dxf) FIELD(nam, RLL, dxf)
#define FIELD_MC(nam,dxf)  FIELD(nam, MC, dxf)
#define FIELD_MS(nam,dxf)  FIELD(nam, MS, dxf)
#define FIELD_TF(nam,len,dxf)  FIELD_TEXT(nam, _obj->nam)
#define FIELD_TFF(nam,len,dxf) FIELD_TEXT(nam, _obj->nam)
#define FIELD_TV(nam,dxf)      FIELD_TEXT(nam, _obj->nam)
#define FIELD_TU(nam,dxf)      FIELD_TEXT_TU(nam, (BITCODE_TU)_obj->nam)
#define FIELD_T(nam,dxf) \
  { if (dat->version >= R_2007) { FIELD_TU(nam, dxf); } \
    else                        { FIELD_TV(nam, dxf); } }
#define FIELD_BT(nam,dxf)    FIELD(nam, BT, dxf);
#define FIELD_4BITS(nam,dxf) FIELD(nam,4BITS,dxf)
#define FIELD_BE(nam,dxf)    FIELD_3RD(nam,dxf)
#define FIELD_DD(nam, _default, dxf) \
    PREFIX fprintf(dat->fh, "\"" #nam "\": " FORMAT_DD ",\n", _obj->nam)
#define FIELD_2DD(nam, d1, d2, dxf) FIELD_2RD(nam,dxf)
#define FIELD_3DD(nam, def, dxf)    FIELD_3RD(nam,dxf)
#define FIELD_2RD(nam,dxf) { \
    PREFIX fprintf(dat->fh, "\"" #nam "\": [ %f, %f ],\n", \
                   _obj->nam.x, _obj->nam.y); }
#define FIELD_2BD(nam,dxf)   FIELD_2RD(nam,dxf)
#define FIELD_2BD_1(nam,dxf) FIELD_2RD(nam,dxf)
#define FIELD_3RD(nam,dxf) { \
    PREFIX fprintf(dat->fh, "\"" #nam "\": [ %f, %f, %f ],\n", \
                   _obj->nam.x, _obj->nam.y, _obj->nam.z); }
#define FIELD_3BD(nam,dxf)     FIELD_3RD(nam,dxf)
#define FIELD_3BD_1(nam,dxf)   FIELD_3RD(nam,dxf)
#define FIELD_3DPOINT(nam,dxf) FIELD_3BD(nam,dxf)

#define SUB_FIELD_T(o,nam,dxf) \
  { if (dat->version >= R_2007) { KEY(nam); VALUE_TEXT_TU((BITCODE_TU)_obj->o.nam); } \
    else                        { PREFIX; fprintf(dat->fh, "\"" #nam "\": \"%s\",\n", \
                                                    _obj->o.nam); } }
#define SUB_FIELD_B(o,nam,dxf)   SUB_FIELD(o, nam, B, dxf)
#define SUB_FIELD_BB(o,nam,dxf)  SUB_FIELD(o, nam, BB, dxf)
#define SUB_FIELD_3B(o,nam,dxf)  SUB_FIELD(o, nam, 3B, dxf)
#define SUB_FIELD_BS(o,nam,dxf)  SUB_FIELD(o, nam, BS, dxf)
#define SUB_FIELD_BL(o,nam,dxf)  SUB_FIELD(o, nam, BL, dxf)
#define SUB_FIELD_BLx(o,nam,dxf) SUB_FIELD(o, nam, BLx, dxf)
#define SUB_FIELD_BD(o,nam,dxf)  SUB_FIELD(o, nam, BD, dxf)
#define SUB_FIELD_RC(o,nam,dxf)  SUB_FIELD(o, nam, RC, dxf)
#define SUB_FIELD_RS(o,nam,dxf)  SUB_FIELD(o, nam, RS, dxf)
#define SUB_FIELD_RD(o,nam,dxf)  SUB_FIELD(o, nam, RD, dxf)
#define SUB_FIELD_RL(o,nam,dxf)  SUB_FIELD(o, nam, RL, dxf)
#define SUB_FIELD_BLL(o,nam,dxf) SUB_FIELD(o, nam, BLL, dxf)
#define SUB_FIELD_RLL(o,nam,dxf) SUB_FIELD(o, nam, RLL, dxf)
#define SUB_FIELD_3BD_inl(o,nam,dxf) KEY(nam); VALUE_3RD(_obj->o, dxf)
#define SUB_FIELD_2BD_1(o,nam,dxf) KEY(nam); VALUE_2RD(_obj->o.nam, dxf)
#define SUB_FIELD_2RD(o,nam,dxf)   KEY(nam); VALUE_2RD(_obj->o.nam, dxf)
#define SUB_FIELD_3RD(o,nam,dxf)   KEY(nam); VALUE_3RD(_obj->o.nam, dxf)
#define SUB_FIELD_3BD(o,nam,dxf)   KEY(nam); VALUE_3RD(_obj->o.nam, dxf)
#define SUB_FIELD_3DPOINT(o,nam,dxf) KEY(nam); VALUE_3RD(_obj->o.nam, dxf)

#define FIELD_CMC(color,dxf1,dxf2) { \
  if (dat->version >= R_2004) { \
    RECORD(color); \
    PREFIX fprintf(dat->fh, "\"index\": %d,\n", _obj->color.index); \
    PREFIX fprintf(dat->fh, "\"" #color ".rgb\": \"%06x\",\n", (unsigned)_obj->color.rgb); \
    /*if (_obj->color.flag & 1) { \
      PREFIX fprintf(dat->fh, "\"" #color ".name\": \"%s\",\n", _obj->color.name); } \
    if (_obj->color.flag & 2) { \
      PREFIX fprintf(dat->fh, "\"" #color ".bookname\": \"%s\",\n", _obj->color.book_name); } */\
    ENDRECORD(); \
  } else { \
    PREFIX fprintf(dat->fh, "\"" #color "\": %d,\n", _obj->color.index); \
  } \
}
#define SUB_FIELD_CMC(o,color,dxf1,dxf2) {    \
  if (dat->version >= R_2004) { \
    RECORD(color); \
    PREFIX fprintf(dat->fh, "\"index\": %d,\n", _obj->o.color.index); \
    PREFIX fprintf(dat->fh, "\"" #color ".rgb\": \"%06x\",\n", (unsigned)_obj->o.color.rgb); \
    /*if (_obj->color.flag & 1) { \
      PREFIX fprintf(dat->fh, "\"" #color ".name\": \"%s\",\n", _obj->o.color.name); } \
    if (_obj->color.flag & 2) { \
      PREFIX fprintf(dat->fh, "\"" #color ".bookname\": \"%s\",\n", _obj->o.color.book_name); } */\
    ENDRECORD(); \
  } else { \
    PREFIX fprintf(dat->fh, "\"" #color "\": %d,\n", _obj->o.color.index); \
  } \
}
#define FIELD_TIMEBLL(nam,dxf) \
    PREFIX fprintf(dat->fh, "\"" #nam "\": " FORMAT_BL "." FORMAT_BL ",\n", \
            _obj->nam.days, _obj->nam.ms)

//FIELD_VECTOR_N(nam, type, size):
// reads data of the type indicated by 'type' 'size' times and stores
// it all in the vector called 'nam'.
#define FIELD_VECTOR_N(nam, type, size, dxf)\
    KEY(nam); ARRAY; \
    if (_obj->nam) { \
      for (vcount=0; vcount < (BITCODE_BL)size; vcount++)\
        {\
          PREFIX fprintf(dat->fh, FORMAT_##type ",\n", _obj->nam[vcount]); \
        }\
      if (size) NOCOMMA;\
    } \
    ENDARRAY;
#define FIELD_VECTOR_T(nam, size, dxf)\
    KEY(nam); ARRAY; \
    if (_obj->nam) { \
      PRE (R_2007) { \
        for (vcount=0; vcount < (BITCODE_BL)_obj->size; vcount++) { \
          PREFIX fprintf(dat->fh, "\"%s\",\n", _obj->nam[vcount]); \
        } \
      } else { \
        for (vcount=0; vcount < (BITCODE_BL)_obj->size; vcount++) { \
          PREFIX VALUE_TEXT_TU(_obj->nam[vcount]); \
        } \
      } \
      if (_obj->size) NOCOMMA;\
    } \
    ENDARRAY;

#define FIELD_VECTOR(nam, type, size, dxf) FIELD_VECTOR_N(nam, type, _obj->size, dxf)

#define FIELD_2RD_VECTOR(nam, size, dxf)\
  KEY(nam); ARRAY;\
  for (vcount=0; vcount < (BITCODE_BL)_obj->size; vcount++)\
    {\
      VALUE_2RD(FIELD_VALUE(nam[vcount]), dxf); \
    }\
  if (_obj->size) NOCOMMA;\
  ENDARRAY;

#define FIELD_2DD_VECTOR(nam, size, dxf)\
  KEY(nam); ARRAY;\
  for (vcount = 0; vcount < (BITCODE_BL)_obj->size; vcount++)\
    {\
      PREFIX VALUE_2RD(_obj->nam[vcount], dxf); \
    }\
  if (_obj->size) NOCOMMA;\
  ENDARRAY;

#define FIELD_3DPOINT_VECTOR(nam, size, dxf)\
  KEY(nam); ARRAY;\
  for (vcount=0; vcount < (BITCODE_BL)_obj->size; vcount++)\
    {\
      PREFIX VALUE_3BD(FIELD_VALUE(nam[vcount]), dxf);\
    }\
  if (_obj->size) NOCOMMA;\
  ENDARRAY;

#define HANDLE_VECTOR_N(nam, size, code, dxf) \
  KEY(nam); \
  ARRAY;\
  for (vcount=0; vcount < (BITCODE_BL)size; vcount++)\
    {\
      FIELD_HANDLE_N(nam[vcount], vcount, code, dxf);\
    }\
  if (size) NOCOMMA; \
  ENDARRAY;

#define HANDLE_VECTOR(nam, sizefield, code, dxf) \
  HANDLE_VECTOR_N(nam, FIELD_VALUE(sizefield), code, dxf)

#define REPEAT_CN(times, nam, type) \
  if (_obj->nam) { KEY(nam); ARRAY; } \
  if (_obj->nam) \
    for (rcount1=0; rcount1<(BITCODE_BL)times; rcount1++)
#define REPEAT_N(times, nam, type) \
  if (_obj->nam) { KEY(nam); ARRAY; } \
  if (_obj->nam) \
    for (rcount1=0; rcount1<(BITCODE_BL)times; rcount1++)
#define _REPEAT_N(times, nam, type, idx) \
  if (_obj->nam) { KEY(nam); ARRAY; } \
  if (_obj->nam) \
    for (rcount##idx=0; rcount##idx<(BITCODE_BL)times; rcount##idx++)
#define _REPEAT_C(times, nam, type, idx) _REPEAT_N(_obj->times, nam, type, idx)
#define _REPEAT(times, nam, type, idx)   _REPEAT_N(_obj->times, nam, type, idx)
#define REPEAT(times, nam, type)  _REPEAT(times, nam, type, 1)
#define REPEAT2(times, nam, type) _REPEAT(times, nam, type, 2)
#define REPEAT3(times, nam, type) _REPEAT(times, nam, type, 3)
#define REPEAT4(times, nam, type) _REPEAT(times, nam, type, 4)
#define REPEAT_C(times, nam, type)  _REPEAT_C(times, nam, type, 1)
#define REPEAT2_C(times, nam, type) _REPEAT_C(times, nam, type, 2)
#define REPEAT3_C(times, nam, type) _REPEAT_C(times, nam, type, 3)
#define REPEAT4_C(times, nam, type) _REPEAT_C(times, nam, type, 4)

#undef  REPEAT_BLOCK
#define REPEAT_BLOCK     { HASH;
#undef  END_REPEAT_BLOCK
#define END_REPEAT_BLOCK NOCOMMA; ENDHASH; }
#undef  END_REPEAT
#define END_REPEAT(nam) if (_obj->nam) { NOCOMMA; ENDARRAY; }

#define FIELD_NUM_INSERTS(num_inserts, type, dxf) \
  FIELD(num_inserts, type, dxf)

#define FIELD_XDATA(nam, size)

#define REACTORS(code)\
  KEY(reactors); ARRAY; \
  if (obj->tio.object->reactors) { \
    for (vcount=0; vcount < obj->tio.object->num_reactors; vcount++)    \
      {                                                                 \
        PREFIX VALUE_HANDLE(obj->tio.object->reactors[vcount], reactors, code, 330); \
      }                                                                 \
    NOCOMMA; \
  } \
  ENDARRAY;

#define XDICOBJHANDLE(code)\
  SINCE(R_2004)\
    {\
      if (!obj->tio.object->xdic_missing_flag) {    \
        KEY(xdicobjhandle); \
        VALUE_HANDLE(obj->tio.object->xdicobjhandle, xdicobjhandle, code, -3); \
      } \
    }\
  PRIOR_VERSIONS\
    {\
      KEY(xdicobjhandle); \
      VALUE_HANDLE(obj->tio.object->xdicobjhandle, xdicobjhandle, code, -3); \
    }

#define COMMON_ENTITY_HANDLE_DATA
#define SECTION_STRING_STREAM
#define START_STRING_STREAM
#define END_STRING_STREAM
#define START_HANDLE_STREAM


static void
_prefix(Bit_Chain* dat) {
  for (int _i=0; _i < dat->bit; _i++) { fprintf (dat->fh, "  "); }
}


#define DWG_ENTITY(token) \
static int \
dwg_json_##token (Bit_Chain *restrict dat, Dwg_Object *restrict obj) \
{\
  BITCODE_BL vcount, rcount1, rcount2, rcount3, rcount4; \
  int error = 0; \
  Bit_Chain *str_dat = dat;\
  Bit_Chain *hdl_dat = dat;\
  Dwg_Data* dwg = obj->parent; \
  Dwg_Entity_##token *ent, *_obj;\
  Dwg_Object_Entity *_ent;\
  LOG_INFO("Entity " #token ":\n")\
  _ent = obj->tio.entity;\
  _obj = ent = _ent->tio.token;\
  FIELD_TEXT(entity, #token);\
  if (strcmp(obj->dxfname, #token)) \
    FIELD_TEXT(dxfname,obj->dxfname);\
  _FIELD(type,RL,0);\
  KEY(handle);VALUE_H(obj->handle, 5);\
  _FIELD(size,RL,0);\
  _FIELD(bitsize,BL,0);\
  if (_ent->picture_exists) ENT_FIELD(picture_exists,B,0);\
  SINCE(R_13) { \
    KEY(owner); VALUE_HANDLE (obj->parent->header_vars.BLOCK_RECORD_MSPACE,owner,0,330); \
    error |= json_common_entity_handle_data(dat, obj); \
  }

#define DWG_ENTITY_END return 0; }

#define DWG_OBJECT(token) \
static int \
dwg_json_ ##token (Bit_Chain *restrict dat, Dwg_Object *restrict obj) \
{ \
  BITCODE_BL vcount, rcount1, rcount2, rcount3, rcount4;\
  int error = 0;\
  Bit_Chain *hdl_dat = dat;\
  Dwg_Data* dwg = obj->parent; \
  Dwg_Object_##token *_obj;\
  LOG_INFO("Object " #token ":\n")\
  _obj = obj->tio.object->tio.token;\
  FIELD_TEXT(object, #token);\
  if (strcmp(obj->dxfname, #token)) \
    FIELD_TEXT(dxfname,obj->dxfname);\
  _FIELD(type,RL,0);\
  KEY(handle);VALUE_H(obj->handle, 5);\
  _FIELD(size,RL,0);\
  _FIELD(bitsize,BL,0);

#define DWG_OBJECT_END return 0; }

static int
json_common_entity_handle_data(Bit_Chain *restrict dat, Dwg_Object *restrict obj)
{
  (void)dat; (void)obj;
  return 0;
}

#include "dwg.spec"

static void
print_wcquote(Bit_Chain *restrict dat, dwg_wchar_t *restrict wstr) {
  BITCODE_TU ws = (BITCODE_TU)wstr;
  uint16_t c;
  fprintf(dat->fh, "\"");
  if (ws) {
    while ((c = *ws++)) {
      if      (c == L'"')  { fprintf(dat->fh, "\\\""); }
      else if (c == L'\\') { fprintf(dat->fh, "\\\\"); }
      else if (c == L'\n') { fprintf(dat->fh, "\\n"); }
      else if (c == L'\r') { fprintf(dat->fh, "\\r"); }
      else if (c < 0x1f || c > 0xff) {
        fprintf(dat->fh, "\\u%04x", c);
      }
      else
        fprintf(dat->fh, "%c", (char)(c & 0xff));
    }
  }
  fprintf(dat->fh, "\",\n");
}

static char*
cquote(char *restrict dest, const char *restrict src) {
  char c;
  char *d = dest;
  char *s = (char*)src;
  while ((c = *s++)) {
    if      (c == '"')  { *dest++ = '\\'; *dest++ = c; }
    else if (c == '\\') { *dest++ = '\\'; *dest++ = c; }
    else if (c == '\n') { *dest++ = '\\'; *dest++ = 'n'; }
    else if (c == '\r') { *dest++ = '\\'; *dest++ = 'r'; }
    else if (c < 0x1f)  { *dest++ = '\\'; *dest++ = 'u';
                          *dest++ = '0';  *dest++ = '0';
                          *dest++ = c < 0x10 ? '0' : '1';
                          *dest++ = (c % 16 > 10 ? 'a' + (c%16) : '0' + (c%16)); }
    else                                  *dest++ = c;
  }
  *dest = 0; //add final delim, skipped above
  return d;
}

#ifdef HAVE_NATIVE_WCHAR2
static wchar_t*
wcquote(wchar_t *restrict dest, const wchar_t *restrict src) {
  wchar_t c;
  wchar_t *d = dest;
  wchar_t *s = (wchar_t*)src;
  while ((c = *s++)) {
    if      (c == L'"')  { *dest++ = L'\\'; *dest++ = c; }
    else if (c == L'\\') { *dest++ = L'\\'; *dest++ = c; }
    else if (c == L'\n') { *dest++ = L'\\'; *dest++ = L'n'; }
    else if (c == L'\r') { *dest++ = L'\\'; *dest++ = L'r'; }
    else if (c < 0x1f)   { *dest++ = L'\\'; *dest++ = L'u';
                          *dest++ = L'0';  *dest++ = L'0';
                          *dest++ = c < 0x10 ? L'0' : L'1';
                          *dest++ = (c % 16 > 10 ? L'a' + (c%16) : L'0' + (c%16)); }
    else                                  *dest++ = c;
  }
  *dest = 0; //add final delim, skipped above
  return d;
}
#endif

/* returns 0 on success
 */
static int
dwg_json_variable_type(Dwg_Data *restrict dwg, Bit_Chain *restrict dat,
                       Dwg_Object *restrict  obj)
{
  int i;
  Dwg_Class *klass;
  int is_entity;

  i = obj->type - 500;
  if (i < 0 || i >= (int)dwg->num_classes)
    return DWG_ERR_INVALIDTYPE;

  klass = &dwg->dwg_class[i];
  if (!klass || !klass->dxfname)
    return DWG_ERR_INTERNALERROR;
  // almost always false
  is_entity = dwg_class_is_entity(klass);

  #include "classes.inc"

  return DWG_ERR_UNHANDLEDCLASS;
}

static int
dwg_json_object(Bit_Chain *restrict dat, Dwg_Object *restrict obj)
{
  switch (obj->type)
    {
    case DWG_TYPE_TEXT:
      return dwg_json_TEXT(dat, obj);
    case DWG_TYPE_ATTRIB:
      return dwg_json_ATTRIB(dat, obj);
    case DWG_TYPE_ATTDEF:
      return dwg_json_ATTDEF(dat, obj);
    case DWG_TYPE_BLOCK:
      return dwg_json_BLOCK(dat, obj);
    case DWG_TYPE_ENDBLK:
      return dwg_json_ENDBLK(dat, obj);
    case DWG_TYPE_SEQEND:
      return dwg_json_SEQEND(dat, obj);
    case DWG_TYPE_INSERT:
      return dwg_json_INSERT(dat, obj);
    case DWG_TYPE_MINSERT:
      return dwg_json_MINSERT(dat, obj);
    case DWG_TYPE_VERTEX_2D:
      return dwg_json_VERTEX_2D(dat, obj);
    case DWG_TYPE_VERTEX_3D:
      return dwg_json_VERTEX_3D(dat, obj);
    case DWG_TYPE_VERTEX_MESH:
      return dwg_json_VERTEX_MESH(dat, obj);
    case DWG_TYPE_VERTEX_PFACE:
      return dwg_json_VERTEX_PFACE(dat, obj);
    case DWG_TYPE_VERTEX_PFACE_FACE:
      return dwg_json_VERTEX_PFACE_FACE(dat, obj);
    case DWG_TYPE_POLYLINE_2D:
      return dwg_json_POLYLINE_2D(dat, obj);
    case DWG_TYPE_POLYLINE_3D:
      return dwg_json_POLYLINE_3D(dat, obj);
    case DWG_TYPE_ARC:
      return dwg_json_ARC(dat, obj);
    case DWG_TYPE_CIRCLE:
      return dwg_json_CIRCLE(dat, obj);
    case DWG_TYPE_LINE:
      return dwg_json_LINE(dat, obj);
    case DWG_TYPE_DIMENSION_ORDINATE:
      return dwg_json_DIMENSION_ORDINATE(dat, obj);
    case DWG_TYPE_DIMENSION_LINEAR:
      return dwg_json_DIMENSION_LINEAR(dat, obj);
    case DWG_TYPE_DIMENSION_ALIGNED:
      return dwg_json_DIMENSION_ALIGNED(dat, obj);
    case DWG_TYPE_DIMENSION_ANG3PT:
      return dwg_json_DIMENSION_ANG3PT(dat, obj);
    case DWG_TYPE_DIMENSION_ANG2LN:
      return dwg_json_DIMENSION_ANG2LN(dat, obj);
    case DWG_TYPE_DIMENSION_RADIUS:
      return dwg_json_DIMENSION_RADIUS(dat, obj);
    case DWG_TYPE_DIMENSION_DIAMETER:
      return dwg_json_DIMENSION_DIAMETER(dat, obj);
    case DWG_TYPE_POINT:
      return dwg_json_POINT(dat, obj);
    case DWG_TYPE__3DFACE:
      return dwg_json__3DFACE(dat, obj);
    case DWG_TYPE_POLYLINE_PFACE:
      return dwg_json_POLYLINE_PFACE(dat, obj);
    case DWG_TYPE_POLYLINE_MESH:
      return dwg_json_POLYLINE_MESH(dat, obj);
    case DWG_TYPE_SOLID:
      return dwg_json_SOLID(dat, obj);
    case DWG_TYPE_TRACE:
      return dwg_json_TRACE(dat, obj);
    case DWG_TYPE_SHAPE:
      return dwg_json_SHAPE(dat, obj);
    case DWG_TYPE_VIEWPORT:
      return dwg_json_VIEWPORT(dat, obj);
    case DWG_TYPE_ELLIPSE:
      return dwg_json_ELLIPSE(dat, obj);
    case DWG_TYPE_SPLINE:
      return dwg_json_SPLINE(dat, obj);
    case DWG_TYPE_REGION:
      return dwg_json_REGION(dat, obj);
    case DWG_TYPE__3DSOLID:
      return dwg_json__3DSOLID(dat, obj);
    case DWG_TYPE_BODY:
      return dwg_json_BODY(dat, obj);
    case DWG_TYPE_RAY:
      return dwg_json_RAY(dat, obj);
    case DWG_TYPE_XLINE:
      return dwg_json_XLINE(dat, obj);
    case DWG_TYPE_DICTIONARY:
      return dwg_json_DICTIONARY(dat, obj);
    case DWG_TYPE_MTEXT:
      return dwg_json_MTEXT(dat, obj);
    case DWG_TYPE_LEADER:
      return dwg_json_LEADER(dat, obj);
    case DWG_TYPE_TOLERANCE:
      return dwg_json_TOLERANCE(dat, obj);
    case DWG_TYPE_MLINE:
      return dwg_json_MLINE(dat, obj);
    case DWG_TYPE_BLOCK_CONTROL:
      return dwg_json_BLOCK_CONTROL(dat, obj);
    case DWG_TYPE_BLOCK_HEADER:
      return dwg_json_BLOCK_HEADER(dat, obj);
    case DWG_TYPE_LAYER_CONTROL:
      return dwg_json_LAYER_CONTROL(dat, obj);
    case DWG_TYPE_LAYER:
      return dwg_json_LAYER(dat, obj);
    case DWG_TYPE_STYLE_CONTROL:
      return dwg_json_STYLE_CONTROL(dat, obj);
    case DWG_TYPE_STYLE:
      return dwg_json_STYLE(dat, obj);
    case DWG_TYPE_LTYPE_CONTROL:
      return dwg_json_LTYPE_CONTROL(dat, obj);
    case DWG_TYPE_LTYPE:
      return dwg_json_LTYPE(dat, obj);
    case DWG_TYPE_VIEW_CONTROL:
      return dwg_json_VIEW_CONTROL(dat, obj);
    case DWG_TYPE_VIEW:
      return dwg_json_VIEW(dat, obj);
    case DWG_TYPE_UCS_CONTROL:
      return dwg_json_UCS_CONTROL(dat, obj);
    case DWG_TYPE_UCS:
      return dwg_json_UCS(dat, obj);
    case DWG_TYPE_VPORT_CONTROL:
      return dwg_json_VPORT_CONTROL(dat, obj);
    case DWG_TYPE_VPORT:
      return dwg_json_VPORT(dat, obj);
    case DWG_TYPE_APPID_CONTROL:
      return dwg_json_APPID_CONTROL(dat, obj);
    case DWG_TYPE_APPID:
      return dwg_json_APPID(dat, obj);
    case DWG_TYPE_DIMSTYLE_CONTROL:
      return dwg_json_DIMSTYLE_CONTROL(dat, obj);
    case DWG_TYPE_DIMSTYLE:
      return dwg_json_DIMSTYLE(dat, obj);
    case DWG_TYPE_VPORT_ENTITY_CONTROL:
      return dwg_json_VPORT_ENTITY_CONTROL(dat, obj);
    case DWG_TYPE_VPORT_ENTITY_HEADER:
      return dwg_json_VPORT_ENTITY_HEADER(dat, obj);
    case DWG_TYPE_GROUP:
      return dwg_json_GROUP(dat, obj);
    case DWG_TYPE_MLINESTYLE:
      return dwg_json_MLINESTYLE(dat, obj);
    case DWG_TYPE_OLE2FRAME:
      return dwg_json_OLE2FRAME(dat, obj);
    case DWG_TYPE_DUMMY:
      return dwg_json_DUMMY(dat, obj);
    case DWG_TYPE_LONG_TRANSACTION:
      return dwg_json_LONG_TRANSACTION(dat, obj);
    case DWG_TYPE_LWPOLYLINE:
      return dwg_json_LWPOLYLINE(dat, obj);
    case DWG_TYPE_HATCH:
      return dwg_json_HATCH(dat, obj);
    case DWG_TYPE_XRECORD:
      return dwg_json_XRECORD(dat, obj);
    case DWG_TYPE_PLACEHOLDER:
      return dwg_json_PLACEHOLDER(dat, obj);
    case DWG_TYPE_PROXY_ENTITY:
      return dwg_json_PROXY_ENTITY(dat, obj);
    case DWG_TYPE_OLEFRAME:
      return dwg_json_OLEFRAME(dat, obj);
    case DWG_TYPE_VBA_PROJECT:
      LOG_ERROR("Unhandled Object VBA_PROJECT. Has its own section\n");
      //dwg_json_VBA_PROJECT(dat, obj);
      break;
    case DWG_TYPE_LAYOUT:
      return dwg_json_LAYOUT(dat, obj);
    default:
      if (obj->type == obj->parent->layout_number)
        {
          return dwg_json_LAYOUT(dat, obj);
        }
      /* > 500 */
      else if (DWG_ERR_UNHANDLEDCLASS &
               dwg_json_variable_type(obj->parent, dat, obj))
        {
          Dwg_Data *dwg = obj->parent;
          int is_entity;
          int i = obj->type - 500;
          Dwg_Class *klass = NULL;

          if (i >= 0 && i < (int)dwg->num_classes)
            {
              klass = &dwg->dwg_class[i];
              is_entity = dwg_class_is_entity(klass);
            }
          // properly dwg_decode_object/_entity for eed, reactors, xdic
          if (klass && !is_entity)
            {
              return dwg_json_UNKNOWN_OBJ(dat, obj);
            }
          else if (klass)
            {
              return dwg_json_UNKNOWN_ENT(dat, obj);
            }
          else // not a class
            {
              LOG_WARN("Unknown object, skipping eed/reactors/xdic");
              SINCE(R_2000)
                {
                  LOG_INFO("Object bitsize: %u\n", obj->bitsize)
                }
              LOG_INFO("Object handle: %d.%d.%lX\n",
                       obj->handle.code, obj->handle.size, obj->handle.value);
            }
        }
    }
  return DWG_ERR_INVALIDTYPE;
}

static int
json_fileheader_write(Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  struct Dwg_Header* _obj = &dwg->header;
  Dwg_Object* obj = NULL;
  int i;

  RECORD(FILEHEADER); // single hash
  KEY(version); fprintf(dat->fh, "\"%s\",\n", version_codes[dwg->header.version]);

  #include "header.spec"

  ENDRECORD();
  return 0;
}

static int
json_header_write(Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  Dwg_Header_Variables* _obj = &dwg->header_vars;
  Dwg_Object* obj = NULL;
  //const int minimal = 0;
  char buf[4096];
  double ms;
  const char* codepage =
    (dwg->header.codepage == 30 || dwg->header.codepage == 0)
    ? "ANSI_1252"
    : (dwg->header.version >= R_2007)
      ? "UTF-8"
      : "ANSI_1252";

  RECORD(HEADER); // single hash
  #include "header_variables.spec"
  ENDRECORD();
  return 0;
}

static int
json_classes_write (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  BITCODE_BL i;

  SECTION(CLASSES); //list of classes
  for (i=0; i < dwg->num_classes; i++)
    {
      Dwg_Class *_obj = &dwg->dwg_class[i];
      HASH;
      FIELD_BS (number, 0);
      FIELD_TV (dxfname, 1);
      FIELD_T (cppname, 2);
      FIELD_T (appname, 3);
      FIELD_BS (proxyflag, 90);
      FIELD_BL (num_instances, 91);
      FIELD_B  (wasazombie, 280);
      FIELD_BS (item_class_id, 281);
      NOCOMMA;
      ENDHASH;
    }
  NOCOMMA;
  ENDSEC();
  return 0;
}

static int
json_objects_write (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  BITCODE_BL i;

  SECTION(OBJECTS);
  for (i=0; i < dwg->num_objects; i++)
    {
      Dwg_Object *obj = &dwg->object[i];
      HASH;
      dwg_json_object(dat, obj);
      NOCOMMA;
      ENDHASH;
    }
  NOCOMMA;
  ENDSEC();
  return 0;
}

/* The object map/handles section */
static int
json_handles_write (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  BITCODE_BL j;
  SECTION(HANDLES);
  for (j = 0; j < dwg->num_objects; j++)
    {
      Dwg_Object *obj = &dwg->object[j];
      // handle => abs. offset
      PREFIX; fprintf(dat->fh, "[ %lu, %lu ],\n", obj->handle.value, obj->address);
    }
  NOCOMMA;
  ENDSEC();
  return 0;
}

static int
json_preview_write (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  Bit_Chain *_obj = (Bit_Chain*) &dwg->picture;
  if (_obj->chain && _obj->size && _obj->size > 10)
    {
      KEY(THUMBNAILIMAGE); HASH;
      PREFIX fprintf(dat->fh, "\"size\": %lu,\n", _obj->size);
      FIELD_BINARY(chain, _obj->size, 310);
      NOCOMMA;
      ENDHASH;
    }
  return 0;
}

EXPORT int
dwg_write_json(Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  const int minimal = dwg->opts & 0x10;
  struct Dwg_Header *obj = &dwg->header;

  fprintf (dat->fh, "{\n"
                     "  \"created_by\": \"%s\",\n", PACKAGE_STRING);
  dat->bit++; // ident

  if (!minimal) {
    json_fileheader_write (dat, dwg);
    // TODO: 3-5 sections:
    // auxheader.spec
    // r2004_file_header.spec
  }

  // A minimal HEADER requires only $ACADVER, $HANDSEED, and then ENTITIES
  json_header_write (dat, dwg);

  if (!minimal && dat->version >= R_13)
    {
      if (json_classes_write (dat, dwg) >= DWG_ERR_CRITICAL)
        goto fail;
    }

  if (json_objects_write (dat, dwg) >= DWG_ERR_CRITICAL)
    goto fail;

  if (!minimal && dat->version >= R_2000) {
    if (json_preview_write (dat, dwg) >= DWG_ERR_CRITICAL)
      goto fail;
  }

  /* object map */
  if (!minimal && dat->version >= R_13) {
    if (json_handles_write (dat, dwg) >= DWG_ERR_CRITICAL)
      goto fail;
  }

  NOCOMMA;
  dat->bit--;
  fprintf (dat->fh, "}\n");
  return 0;
 fail:
  return 1;
}

#undef IS_PRINT
