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
 * out_dxf.c: write as Ascii DXF
 * written by Reini Urban
 */

/* Works for most r13-r2000 files, but not for many r2004+
TODO: down-conversions from unsupported entities on older DXF versions.

Since r13:
Entities: LWPOLYLINE, HATCH, SPLINE, LEADER, DIMENSION, MTEXT, IMAGE, BLOCK_RECORD.
Add CLASSES for those
*/

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
//#include <math.h>

#include "common.h"
#include "bits.h"
#include "dwg.h"
#include "decode.h"
#include "out_dxf.h"

#define DWG_LOGLEVEL DWG_LOGLEVEL_NONE
#include "logging.h"

/* the current version per spec block */
static unsigned int cur_ver = 0;
static char buf[255];

// private
static int
dxf_common_entity_handle_data(Bit_Chain *restrict dat, const Dwg_Object *restrict obj);
static int
dwg_dxf_object(Bit_Chain *restrict dat, const Dwg_Object *restrict obj);
static int dxf_3dsolid(Bit_Chain *restrict dat,
                       const Dwg_Object *restrict obj,
                       Dwg_Entity_3DSOLID *restrict _obj);

/*--------------------------------------------------------------------------------
 * MACROS
 */

#define ACTION dxf
#define IS_PRINT
#define IS_DXF

#define FIELD(name,type,dxf) VALUE(_obj->name,type,dxf)
#define FIELD_CAST(name,type,cast,dxf) FIELD(name,cast,dxf)
#define FIELD_TRACE(name,type)

#define VALUE_TV(value,dxf) \
  { GROUP(dxf); \
    fprintf(dat->fh, "%s\r\n", value); }
#ifdef HAVE_NATIVE_WCHAR2
# define VALUE_TU(value,dxf)\
  { GROUP(dxf); \
    fprintf(dat->fh, "%ls\r\n", value ? (wchar_t*)value : L""); }
#else
# define VALUE_TU(wstr,dxf) \
  { \
    BITCODE_TU ws = (BITCODE_TU)wstr; \
    uint16_t _c; \
    GROUP(dxf);\
    if (wstr) \
      while ((_c = *ws++)) { \
        fprintf(dat->fh, "%c", (char)(_c & 0xff)); \
      } \
    fprintf(dat->fh, "\r\n"); \
  }
#endif
#define VALUE_TFF(str,dxf)    VALUE_TV(str, dxf)
#define VALUE_BINARY(value,size,dxf) \
{ \
  long len = size; \
  do { \
    short j; \
    long l = len > 127 ? 127 : len; \
    GROUP(dxf); \
    if (value) \
      for (j=0; j < l; j++) { \
        fprintf(dat->fh, "%02X", value[j]); \
      } \
    fprintf(dat->fh, "\r\n"); \
    len -= 127; \
  } while (len > 127); \
}

#define FIELD_VALUE(name) _obj->name
#define ANYCODE -1
// the hex code
#define VALUE_HANDLE(value, handle_code, dxf) \
  if (dxf) { \
    fprintf(dat->fh, "%3i\r\n%lX\r\n", dxf, value ? value->absolute_ref : 0); \
  }
// the name in the table, referenced by the handle
// names on: 6 7 8. which else? there are more styles: plot, ...
// rather skip unknown handles
#define FIELD_HANDLE(name, handle_code, dxf) \
  if (dxf && _obj->name) { \
    if (dxf == 6) \
      FIELD_HANDLE_NAME(name, dxf, LTYPE) \
    else if (dxf == 7) \
      FIELD_HANDLE_NAME(name, dxf, STYLE) \
    else if (dxf == 8) \
      FIELD_HANDLE_NAME(name, dxf, LAYER) \
    else if (dat->version >= R_13) \
      fprintf(dat->fh, "%3i\r\n%lX\r\n", dxf, _obj->name->absolute_ref); \
  }
#define HEADER_9(name) \
    GROUP(9);\
    fprintf (dat->fh, "$%s\r\n", #name)
#define VALUE_H(value, dxf) \
    if (dxf) \
      fprintf(dat->fh, "%3i\r\n%lX\r\n", dxf, value ? value->absolute_ref : 0)
#define HEADER_H(name,dxf) \
    HEADER_9(name);\
    VALUE_H(dwg->header_vars.name, dxf)

#define HEADER_VALUE(name, type, dxf, value) \
  if (dxf) {\
    GROUP(9);\
    fprintf (dat->fh, "$" #name "\r\n");\
    VALUE (value, type, dxf);\
  }
#define HEADER_VAR(name, type, dxf) \
  HEADER_VALUE(name, type, dxf, dwg->header_vars.name)

#define HEADER_3D(name)\
  HEADER_9(name);\
  POINT_3D (name, header_vars.name, 10, 20, 30)
#define HEADER_2D(name)\
  HEADER_9(name);\
  POINT_2D (name, header_vars.name, 10, 20)
#define HEADER_BLL(name, dxf) \
  HEADER_9(name);\
  VALUE_BLL(dwg->header_vars.name, dxf)

#define SECTION(section) fprintf(dat->fh, "  0\r\nSECTION\r\n  2\r\n" #section "\r\n")
#define ENDSEC()         fprintf(dat->fh, "  0\r\nENDSEC\r\n")
#define TABLE(table)     fprintf(dat->fh, "  0\r\nTABLE\r\n  2\r\n" #table "\r\n")
#define ENDTAB()         fprintf(dat->fh, "  0\r\nENDTAB\r\n")
#define RECORD(record)   fprintf(dat->fh, "  0\r\n" #record "\r\n")
#define SUBCLASS(text)   if (dat->from_version >= R_2000) { VALUE_TV(#text, 100); }

#define GROUP(dxf) \
    fprintf (dat->fh, "%3i\r\n", dxf)
/* avoid empty numbers, and fixup some bad %f libc formatting */
#define VALUE(value, type, dxf) \
  if (dxf) { \
    char *_s; \
    const char *_fmt = dxf_format (dxf); \
    GROUP(dxf); \
    GCC_DIAG_IGNORE(-Wformat-nonliteral) \
    snprintf(buf, 255, _fmt, value); \
    GCC_DIAG_RESTORE \
    /* not a string, empty num. must be zero */ \
    if (strcmp(_fmt, "%s") && !*buf) \
      strcpy(buf, "0"); \
    else if (90 <= dxf && dxf < 100) { \
      /* -Wpointer-to-int-cast */ \
      const int32_t _si = (int32_t)(intptr_t)(value); \
      snprintf(buf, 255, "%6i", _si); \
    } else if (!strcmp(_fmt, "%-16.14f")) {      \
      if (!strcmp(buf, "0.00000000000000")) \
        strcpy(buf, "0.0"); \
      else if ((_s = strstr(buf, ".00000000000000"))) \
        strcpy(_s, ".0"); \
      else if ((_s = strstr(buf, ".50000000000000"))) \
        strcpy(_s, ".5"); \
      else if ((_s = strstr(buf, ".12500000000000"))) \
        strcpy(_s, ".125"); \
    } \
    fprintf(dat->fh, "%s\r\n", buf); \
  }
#define VALUE_RD(value, dxf) \
  if (dxf && !bit_isnan(value)) { \
    GROUP(dxf); \
    if (value == 0.0 || value == 0) \
      fprintf(dat->fh, "0.0\r\n"); \
    else if (value == 0.5) \
      fprintf(dat->fh, "0.5\r\n"); \
    else if (value == 0.125) \
      fprintf(dat->fh, "0.125\r\n"); \
    else \
      fprintf(dat->fh, "%-16.14f\r\n", value); \
  }
#define VALUE_B(value, dxf) \
  if (dxf) { \
    GROUP(dxf); \
    if (value == 0) \
      fprintf(dat->fh, "     0\r\n"); \
    else \
      fprintf(dat->fh, "     1\r\n"); \
  }

#define FIELD_HANDLE_NAME(name, dxf, table) \
  {\
    Dwg_Object_Ref *ref = _obj->name;\
    Dwg_Object *o = ref ? ref->obj : NULL;\
    dxf_cvt_tablerecord(dat, o, o ? o->tio.object->tio.table->entry_name : (char*)"0", dxf); \
  }
#define HEADER_HANDLE_NAME(name, dxf, table) \
  HEADER_9(name); FIELD_HANDLE_NAME(name, dxf, table)

#define FIELD_DATAHANDLE(name, code, dxf) FIELD_HANDLE(name, code, dxf)

#define HEADER_RC(name,dxf)  HEADER_9(name); FIELD(name, RC, dxf)
#define HEADER_RS(name,dxf)  HEADER_9(name); FIELD(name, RS, dxf)
#define HEADER_RD(name,dxf)  HEADER_9(name); FIELD_RD(name, dxf)
#define HEADER_RL(name,dxf)  HEADER_9(name); FIELD(name, RL, dxf)
#define HEADER_RLL(name,dxf) HEADER_9(name); FIELD(name, RLL, dxf)
#define HEADER_TV(name,dxf)  HEADER_9(name); VALUE_TV(_obj->name, dxf)
#define HEADER_TU(name,dxf)  HEADER_9(name); VALUE_TU(_obj->name, dxf)
#define HEADER_T(name,dxf)   HEADER_9(name); VALUE_T((char*)_obj->name, dxf)
#define HEADER_B(name,dxf)   HEADER_9(name); FIELD_B(name, dxf)
#define HEADER_BS(name,dxf)  HEADER_9(name); FIELD(name, BS, dxf)
#define HEADER_BD(name,dxf)  HEADER_9(name); FIELD_BD(name, dxf)
#define HEADER_BL(name,dxf)  HEADER_9(name); FIELD(name, BL, dxf)

//#define VALUE_B(value,dxf)   VALUE(value, RC, dxf)
#define VALUE_BB(value,dxf)  VALUE(value, RC, dxf)
#define VALUE_3B(value,dxf)  VALUE(value, RC, dxf)
#define VALUE_BS(value,dxf)  VALUE(value, RS, dxf)
#define VALUE_BL(value,dxf)  VALUE(value, BL, dxf)
#define VALUE_BLL(value,dxf) VALUE(value, RLL, dxf)
#define VALUE_BD(value,dxf) \
  { if (dxf >= 50 && dxf < 55) value = rad2deg(value); \
    VALUE_RD(value, dxf); }
#define VALUE_RC(value,dxf)  VALUE(value, RC, dxf)
#define VALUE_RS(value,dxf)  VALUE(value, RS, dxf)
#define VALUE_RL(value,dxf)  VALUE(value, RL, dxf)
#define VALUE_RLL(value,dxf) VALUE(value, RLL, dxf)
#define VALUE_MC(value,dxf)  VALUE(value, MC, dxf)
#define VALUE_MS(value,dxf)  VALUE(value, MS, dxf)
#define VALUE_3BD(pt,dxf) { VALUE_RD(pt.x, dxf); VALUE_RD(pt.y, dxf+10); VALUE_RD(pt.z, dxf+20);}

#define FIELD_RD(name,dxf)  VALUE_RD(_obj->name, dxf)
#define FIELD_B(name,dxf)   VALUE_B(_obj->name, dxf)
#define FIELD_BB(name,dxf)  FIELD(name, BB, dxf)
#define FIELD_3B(name,dxf)  FIELD(name, 3B, dxf)
#define FIELD_BS(name,dxf)  FIELD(name, BS, dxf)
#define FIELD_BL(name,dxf)  FIELD(name, BL, dxf)
#define FIELD_BLL(name,dxf) FIELD(name, BLL, dxf)
#define FIELD_BD(name,dxf)  \
  { if (dxf >= 50 && dxf < 55) _obj->name = rad2deg(_obj->name); \
    FIELD_RD(name, dxf); }
#define FIELD_RC(name,dxf)  FIELD(name, RC, dxf)
#define FIELD_RS(name,dxf)  FIELD(name, RS, dxf)
#define FIELD_RL(name,dxf)  FIELD(name, RL, dxf)
#define FIELD_RLL(name,dxf) FIELD(name, RLL, dxf)
#define FIELD_MC(name,dxf)  FIELD(name, MC, dxf)
#define FIELD_MS(name,dxf)  FIELD(name, MS, dxf)
#define FIELD_TF(name,len,dxf)  VALUE_TV(_obj->name, dxf)
#define FIELD_TFF(name,len,dxf) VALUE_TV(_obj->name, dxf)
#define FIELD_TV(name,dxf) \
  if (dxf) { VALUE_TV(_obj->name,dxf); }
#define FIELD_TU(name,dxf) \
  if (dxf) { VALUE_TU((BITCODE_TU)_obj->name, dxf); }
#define FIELD_T(name,dxf) \
  { if (dat->from_version >= R_2007) { FIELD_TU(name, dxf); } \
    else                             { FIELD_TV(name, dxf); } }
#define VALUE_T(value,dxf) \
  { if (dat->from_version >= R_2007) { VALUE_TU(value, dxf); } \
    else                             { VALUE_TV(value, dxf); } }
#define FIELD_BT(name,dxf)     FIELD(name, BT, dxf);
#define FIELD_4BITS(name,dxf)  FIELD(name,4BITS,dxf)
#define FIELD_BE(name,dxf)     FIELD_3RD(name,dxf)
#define FIELD_DD(name, _default, dxf) FIELD_BD(name, dxf)
#define FIELD_2DD(name, d1, d2, dxf) { FIELD_DD(name.x, d1, dxf); FIELD_DD(name.y, d2, dxf+10); }
#define FIELD_3DD(name, def, dxf) { \
    FIELD_DD(name.x, FIELD_VALUE(def.x), dxf); \
    FIELD_DD(name.y, FIELD_VALUE(def.y), dxf+10); \
    FIELD_DD(name.z, FIELD_VALUE(def.z), dxf+20); }
#define FIELD_2RD(name,dxf) {FIELD_RD(name.x, dxf); FIELD_RD(name.y, dxf+10);}
#define FIELD_2BD(name,dxf) {FIELD_BD(name.x, dxf); FIELD_BD(name.y, dxf+10);}
#define FIELD_2BD_1(name,dxf) {FIELD_BD(name.x, dxf); FIELD_BD(name.y, dxf+1);}
#define FIELD_3RD(name,dxf) {FIELD_RD(name.x, dxf); FIELD_RD(name.y, dxf+10); FIELD_RD(name.z, dxf+20);}
#define FIELD_3BD(name,dxf) {FIELD_BD(name.x, dxf); FIELD_BD(name.y, dxf+10); FIELD_BD(name.z, dxf+20);}
#define FIELD_3BD_1(name,dxf) {FIELD_BD(name.x, dxf); FIELD_BD(name.y, dxf+1); FIELD_BD(name.z, dxf+2);}
#define FIELD_3DPOINT(name,dxf) FIELD_3BD(name,dxf)
#define FIELD_CMC(color,dxf1,dxf2) { \
  VALUE_RS(_obj->color.index, dxf1); \
  if (dat->version >= R_2004 && dxf2 && _obj->color.rgb) { \
    VALUE_RL(_obj->color.rgb, dxf2); \
  } \
}
#define HEADER_TIMEBLL(name, dxf) \
  HEADER_9(name); FIELD_TIMEBLL(name, dxf)
#define FIELD_TIMEBLL(name,dxf) \
  GROUP(dxf); fprintf(dat->fh, FORMAT_RL "." FORMAT_RL "\r\n", \
                      _obj->name.days, _obj->name.ms)
#define HEADER_CMC(name,dxf) \
    HEADER_9(name);\
    VALUE_RS(dwg->header_vars.name.index, dxf)

#define POINT_3D(name, var, c1, c2, c3)\
  {\
    VALUE_RD(dwg->var.x, c1);\
    VALUE_RD(dwg->var.y, c2);\
    VALUE_RD(dwg->var.z, c3);\
  }
#define POINT_2D(name, var, c1, c2) \
  {\
    VALUE_RD(dwg->var.x, c1);\
    VALUE_RD(dwg->var.y, c2);\
  }

//FIELD_VECTOR_N(name, type, size):
// reads data of the type indicated by 'type' 'size' times and stores
// it all in the vector called 'name'.
#define FIELD_VECTOR_N(name, type, size, dxf)\
  if (dxf && _obj->name)\
    {\
      for (vcount=0; vcount < (BITCODE_BL)size; vcount++)\
        {\
          VALUE_##type(_obj->name[vcount], dxf); \
        }\
    }
#define FIELD_VECTOR_T(name, size, dxf)\
  if (dxf && _obj->name) {\
    PRE (R_2007) {                                                   \
      for (vcount=0; vcount < (BITCODE_BL)_obj->size; vcount++)             \
        VALUE_TV(_obj->name[vcount], dxf);                           \
    } else {                                                         \
      for (vcount=0; vcount < (BITCODE_BL)_obj->size; vcount++)             \
        VALUE_TU(_obj->name[vcount], dxf);                           \
    }                                                                \
  }

#define FIELD_VECTOR(name, type, size, dxf) FIELD_VECTOR_N(name, type, _obj->size, dxf)

#define FIELD_2RD_VECTOR(name, size, dxf)\
  if (dxf && _obj->name) {\
    for (vcount=0; vcount < (BITCODE_BL)_obj->size; vcount++)    \
      {\
        FIELD_2RD(name[vcount], dxf);\
      }\
  }

#define FIELD_2DD_VECTOR(name, size, dxf)\
  FIELD_2RD(name[0], dxf);\
  if (dxf && _obj->name) {\
    for (vcount = 1; vcount < (BITCODE_BL)_obj->size; vcount++)\
      {\
        FIELD_2DD(name[vcount], FIELD_VALUE(name[vcount - 1].x), FIELD_VALUE(name[vcount - 1].y), dxf);\
      }\
  }

#define FIELD_3DPOINT_VECTOR(name, size, dxf)\
  if (dxf) {\
    for (vcount=0; vcount < (BITCODE_BL)_obj->size; vcount++)\
      {\
        FIELD_3DPOINT(name[vcount], dxf);\
      }\
    }

#define VALUE_HANDLE_N(hdlptr, name, vcount, handle_code, dxf) \
  if (dxf && hdlptr && size) {\
    for (vcount=0; vcount < (BITCODE_BL)size; vcount++)\
      {\
        VALUE_HANDLE(hdlptr[vcount], handle_code, dxf);\
      }\
    }
#define FIELD_HANDLE_N(name, size, handle_code, dxf) \
  VALUE_HANDLE(_obj->name, handle_code, dxf)
#define HANDLE_VECTOR_N(name, size, code, dxf) \
  if (dxf && _obj->name && size) {\
    for (vcount=0; vcount < (BITCODE_BL)size; vcount++)\
      {\
        FIELD_HANDLE(name[vcount], code, dxf);\
      }\
    }

#define HANDLE_VECTOR(name, sizefield, code, dxf) \
  HANDLE_VECTOR_N(name, FIELD_VALUE(sizefield), code, dxf)

#define FIELD_NUM_INSERTS(num_inserts, type, dxf) \
  FIELD(num_inserts, type, dxf)

#define FIELD_XDATA(name, size) \
  dxf_write_xdata(dat, _obj->name, _obj->size)

#define _XDICOBJHANDLE(code) \
  if (dat->version >= R_13 && \
      obj->tio.object->xdicobjhandle && \
      obj->tio.object->xdicobjhandle->absolute_ref) \
  { \
    fprintf(dat->fh, "102\r\n{ACAD_XDICTIONARY\r\n");\
    VALUE_HANDLE(obj->tio.object->xdicobjhandle, code, 360); \
    fprintf(dat->fh, "102\r\n}\r\n");\
  }
#define _REACTORS(code)\
  if (dat->version >= R_13 && \
      obj->tio.object->num_reactors && \
      obj->tio.object->reactors) \
  { \
    fprintf(dat->fh, "102\r\n{ACAD_REACTORS\r\n");\
    for (vcount=0; vcount < obj->tio.object->num_reactors; vcount++)\
      { /* soft ptr */ \
        VALUE_HANDLE(obj->tio.object->reactors[vcount], code, 330); \
      }\
    fprintf(dat->fh, "102\r\n}\r\n");\
  }
#define ENT_REACTORS(code)\
  if (dat->version >= R_13 && _obj->num_reactors && _obj->reactors) {\
    fprintf(dat->fh, "102\r\n{ACAD_REACTORS\r\n");\
    for (vcount=0; vcount < _obj->num_reactors; vcount++)\
      {\
        VALUE_HANDLE(_obj->reactors[vcount], code, 330); \
      }\
    fprintf(dat->fh, "102\r\n}\r\n");\
  }
#define REACTORS(code)
#define XDICOBJHANDLE(code)
#define ENT_XDICOBJHANDLE(code) \
  if (dat->version >= R_13 && \
      obj->tio.entity->xdicobjhandle && \
      obj->tio.entity->xdicobjhandle->absolute_ref) \
  { \
    fprintf(dat->fh, "102\r\n{ACAD_XDICTIONARY\r\n");\
    VALUE_HANDLE(obj->tio.entity->xdicobjhandle, code, 360); \
    fprintf(dat->fh, "102\r\n}\r\n");\
  }

#define COMMON_ENTITY_HANDLE_DATA
#define SECTION_STRING_STREAM
#define START_STRING_STREAM
#define END_STRING_STREAM
#define START_HANDLE_STREAM

#ifndef DEBUG_CLASSES
static int
dwg_dxf_TABLECONTENT (Bit_Chain *restrict dat, const Dwg_Object *restrict obj)
{
  (void)dat; (void)obj;
  return 0;
}
#else
static int
dwg_dxf_TABLECONTENT (Bit_Chain *restrict dat, const Dwg_Object *restrict obj);
#endif

//The strcmp is being optimized away at compile-time! https://godbolt.org/g/AqkhwL
#define DWG_ENTITY(token) \
static int \
dwg_dxf_##token (Bit_Chain *restrict dat, const Dwg_Object *restrict obj) \
{\
  BITCODE_BL vcount, rcount1, rcount2, rcount3, rcount4; \
  int error = 0; \
  Dwg_Data* dwg = obj->parent; \
  Dwg_Entity_##token *ent, *_obj;\
  Dwg_Object_Entity *_ent;\
  if (!strcmp(#token, "GEOPOSITIONMARKER"))\
    RECORD(POSITIONMARKER);\
  else if (dat->version < R_13 && strlen(#token) == 10 && !strcmp(#token, "LWPOLYLINE")) \
    RECORD(POLYLINE);\
  else if (strlen(#token) > 10 && !memcmp(#token, "DIMENSION_", 10)) \
    RECORD(DIMENSION);\
  else if (strlen(#token) > 9 && !memcmp(#token,  "POLYLINE_", 9)) \
    RECORD(POLYLINE);\
  else if (strlen(#token) > 7 && !memcmp(#token, "VERTEX_", 7)) \
    RECORD(VERTEX);\
  else if (dat->version >= R_2010 && !strcmp(#token, "TABLE")) { \
    RECORD(ACAD_TABLE);\
    return dwg_dxf_TABLECONTENT(dat, obj); \
  } \
  else if (obj->type >= 500 && obj->dxfname) \
    fprintf(dat->fh, "  0\r\n%s\r\n", obj->dxfname); \
  else\
    RECORD(token);\
  _ent = obj->tio.entity;\
  _obj = ent = _ent->tio.token;\
  LOG_INFO("Entity " #token ":\n")\
  SINCE(R_11) { \
    LOG_TRACE("Entity handle: %d.%d.%lX\n",\
              obj->handle.code,\
              obj->handle.size,\
              obj->handle.value); \
    fprintf(dat->fh, "%3i\r\n%lX\r\n", 5, obj->handle.value); \
  } \
  SINCE(R_13) { \
    VALUE_HANDLE (obj->parent->header_vars.BLOCK_RECORD_MSPACE, 5, 330); \
    error |= dxf_common_entity_handle_data(dat, obj); \
  }

#define DWG_ENTITY_END return error; \
}

#define DWG_OBJECT(token) \
static int \
dwg_dxf_ ##token (Bit_Chain *restrict dat, const Dwg_Object *restrict obj) \
{ \
  BITCODE_BL vcount, rcount1, rcount2, rcount3, rcount4;\
  int error = 0; \
  Bit_Chain *hdl_dat = dat;\
  Dwg_Data* dwg = obj->parent; \
  Dwg_Object_##token *_obj;\
  LOG_INFO("Object " #token ":\n")\
  _obj = obj->tio.object->tio.token;\
  if (!dwg_obj_is_control(obj)) { \
    if (obj->fixedtype == DWG_TYPE_TABLE) \
      ; \
    else if (obj->type >= 500 && obj->dxfname)        \
      fprintf(dat->fh, "  0\r\n%s\r\n", obj->dxfname); \
    else if (obj->type == DWG_TYPE_PLACEHOLDER) \
      RECORD(ACDBPLACEHOLDER); \
    else if (obj->type != DWG_TYPE_BLOCK_HEADER) \
      RECORD(token);                             \
    SINCE(R_13) { \
      int dxf = 5; \
      if (obj->type == DWG_TYPE_DIMSTYLE) dxf = 105; \
      fprintf(dat->fh, "%3i\r\n%lX\r\n", dxf, obj->handle.value); \
      _XDICOBJHANDLE(3); \
      _REACTORS(4); \
    } \
  } \
  LOG_TRACE("Object handle: %d.%d.%lX\n",\
            obj->handle.code,   \
            obj->handle.size,   \
            obj->handle.value)

//then 330, SUBCLASS

#define DWG_OBJECT_END return error; \
}

static int
dxf_write_xdata(Bit_Chain *restrict dat, Dwg_Resbuf *restrict rbuf, BITCODE_BL size)
{
  Dwg_Resbuf *tmp;
  int i;

  while (rbuf)
    {
      int dxftype = rbuf->type;
      const char* fmt = dxf_format(rbuf->type);
      short type = get_base_value_type(rbuf->type);
      if (!strcmp(fmt, "(unknown code)"))
        {
          if (type == VT_INVALID) {
            LOG_WARN("Invalid xdata code %d", dxftype);
          } else {
            LOG_WARN("Unknown xdata code %d => %d", dxftype, (BITCODE_BL)type);
          }
        }

      tmp = rbuf->next;
      switch (type)
        {
        case VT_STRING:
          UNTIL(R_2007) {
            VALUE_TV(rbuf->value.str.u.data, dxftype);
          } LATER_VERSIONS {
            VALUE_TU(rbuf->value.str.u.wdata, dxftype);
          }
          break;
        case VT_REAL:
          VALUE_RD(rbuf->value.dbl, dxftype);
          break;
        case VT_BOOL:
        case VT_INT8:
          VALUE_RC(rbuf->value.i8, dxftype);
          break;
        case VT_INT16:
          VALUE_RS(rbuf->value.i16, dxftype);
          break;
        case VT_INT32:
          VALUE_RL(rbuf->value.i32, dxftype);
          break;
        case VT_POINT3D:
          VALUE_RD(rbuf->value.pt[0], dxftype);
          VALUE_RD(rbuf->value.pt[1], dxftype+1);
          VALUE_RD(rbuf->value.pt[2], dxftype+2);
          break;
        case VT_BINARY:
          VALUE_BINARY(rbuf->value.str.u.data, rbuf->value.str.size, dxftype);
          break;
        case VT_HANDLE:
        case VT_OBJECTID:
          fprintf(dat->fh, "%3i\r\n%lX\r\n", dxftype,
                  (unsigned long)*(uint64_t*)rbuf->value.hdl);
          break;
        case VT_INVALID:
          break; //skip
        default:
          fprintf(dat->fh, "%3i\r\n\r\n", dxftype);
          break;
        }
      rbuf = tmp;
    }
  return 0;
}

#undef DXF_3DSOLID
#define DXF_3DSOLID dxf_3dsolid(dat, obj, (Dwg_Entity_3DSOLID*)_obj);

// r13+ converts STANDARD to Standard, BYLAYER to ByLayer, BYBLOCK to ByBlock
static void
dxf_cvt_tablerecord(Bit_Chain *restrict dat, const Dwg_Object *restrict obj,
                 char *restrict entry_name, const int dxf)
{
  if (obj && obj->supertype == DWG_SUPERTYPE_OBJECT && entry_name)
    {
      if (dat->version >= R_2007) // r2007+ unicode names
        {
          entry_name = bit_convert_TU((BITCODE_TU)entry_name);
        }
      if (dat->from_version >= R_13 && dat->version < R_13)
        { // convert the other way round, from newer to older
          if (!strcmp(entry_name, "Standard"))
            fprintf(dat->fh, "%3i\r\nSTANDARD\r\n", dxf);
          else if (!strcmp(entry_name, "ByLayer"))
            fprintf(dat->fh, "%3i\r\nBYLAYER\r\n", dxf);
          else if (!strcmp(entry_name, "ByBlock"))
            fprintf(dat->fh, "%3i\r\nBYBLOCK\r\n", dxf);
          else if (!strcmp(entry_name, "*Active"))
            fprintf(dat->fh, "%3i\r\n*ACTIVE\r\n", dxf);
          else
            fprintf(dat->fh, "%3i\r\n%s\r\n", dxf, entry_name);
        }
      else
        { // convert some standard names
          if (dat->version >= R_13 && !strcmp(entry_name, "STANDARD"))
            fprintf(dat->fh, "%3i\r\nStandard\r\n", dxf);
          else if (dat->version >= R_13 && !strcmp(entry_name, "BYLAYER"))
            fprintf(dat->fh, "%3i\r\nByLayer\r\n", dxf);
          else if (dat->version >= R_13 && !strcmp(entry_name, "BYBLOCK"))
            fprintf(dat->fh, "%3i\r\nByBlock\r\n", dxf);
          else if (dat->version >= R_13 && !strcmp(entry_name, "*ACTIVE"))
            fprintf(dat->fh, "%3i\r\n*Active\r\n", dxf);
          else
            fprintf(dat->fh, "%3i\r\n%s\r\n", dxf, entry_name);
        }
    }
  else {
    fprintf(dat->fh, "%3i\r\n\r\n", dxf);
  }
}

// 5 written here first
#define COMMON_TABLE_CONTROL_FLAGS \
  if (ctrl) { \
    SINCE(R_13) { \
      fprintf(dat->fh, "%3i\r\n%lX\r\n", 5, ctrl->handle.value);\
    } \
    SINCE(R_14) { \
      VALUE_H (_ctrl->null_handle, 330); \
    } \
  } \
  SINCE(R_2000) {                        \
    VALUE_TV ("AcDbSymbolTable", 100);   \
  }

//TODO add 340
#define COMMON_TABLE_FLAGS(owner, acdbname) \
    SINCE(R_14) { \
      FIELD_HANDLE (owner, 4, 330); \
    } \
    SINCE(R_2000) { \
      VALUE_TV ("AcDbSymbolTableRecord", 100); \
      VALUE_TV ("AcDb" #acdbname "TableRecord", 100); \
    }\
    if (_obj->entry_name) dxf_cvt_tablerecord(dat, obj, _obj->entry_name, 2); \
    FIELD_RC (flag, 70)

#define LAYER_TABLE_FLAGS(owner, acdbname) \
    SINCE(R_14) { \
      FIELD_HANDLE (owner, 4, 330); \
    } \
    SINCE(R_2000) { \
      VALUE_TV ("AcDbSymbolTableRecord", 100); \
      VALUE_TV ("AcDb" #acdbname "TableRecord", 100); \
    } \
    if (_obj->entry_name) dxf_cvt_tablerecord(dat, obj, _obj->entry_name, 2); \
    FIELD_RS (flag, 70)

#include "dwg.spec"

static int dxf_3dsolid(Bit_Chain *restrict dat,
                       const Dwg_Object *restrict obj,
                       Dwg_Entity_3DSOLID *restrict _obj)
{
  Dwg_Data* dwg = obj->parent;
  unsigned long j;
  BITCODE_BL vcount, rcount1, rcount2;
  BITCODE_BL i;
  int error = 0;
  int index;
  int total_size = 0;
  int num_blocks = 0;

  COMMON_ENTITY_HANDLE_DATA;

  FIELD_B (acis_empty, 0);
  if (!FIELD_VALUE(acis_empty))
    {
      FIELD_B (unknown, 0);
      FIELD_BS (version, 70);
      if (FIELD_VALUE(version) == 1)
        {
          for (i=0; i<FIELD_VALUE(num_blocks); i++)
            {
              char *s = FIELD_VALUE(encr_sat_data[i]);
              int len = strlen(s);
              // FIELD_BL (block_size[i], 0);
              // DXF 1 + 3 if >255
              while (len > 0) {
                char *n = strchr(s, '\n');
                int l = len > 255 ? 255 : len;
                if (n && (n-s < len))
                  l = n-s;
                if (l) {
                  if (l < 255)
                    GROUP(1);
                  else
                    GROUP(3);
                  if (s[l-1] == '\r')
                    fprintf(dat->fh, "%.*s\n", l, s);
                  else
                    fprintf(dat->fh, "%.*s\r\n", l, s);
                  l++;
                  len -= l;
                  s += l;
                } else {
                  len--;
                  s++;
                }
              }
            }
          //LOG_TRACE("acis_data [1]:\n%s\n", FIELD_VALUE (acis_data));
        }
      else //if (FIELD_VALUE(version)==2)
        {
          //TODO
          LOG_ERROR("TODO: Implement parsing of SAT file (version 2) "
                    "in entities 37,38 and 39.\n");
        }
/*
      FIELD_B (wireframe_data_present, 0);
      if (FIELD_VALUE(wireframe_data_present))
        {
          FIELD_B (point_present, 0);
          if (FIELD_VALUE(point_present))
            {
              FIELD_3BD (point, 0);
            }
          FIELD_BL (num_isolines, 0);
          FIELD_B (isoline_present, 0);
          if (FIELD_VALUE(isoline_present))
            {
              FIELD_BL (num_wires, 0);
              REPEAT(num_wires, wires, Dwg_3DSOLID_wire)
                {
                  PARSE_WIRE_STRUCT(wires[rcount1])
                }
              END_REPEAT(wires);
              FIELD_BL (num_silhouettes, 0);
              REPEAT(num_silhouettes, silhouettes, Dwg_3DSOLID_silhouette)
                {
                  FIELD_BL (silhouettes[rcount1].vp_id, 0);
                  FIELD_3BD (silhouettes[rcount1].vp_target, 0);
                  FIELD_3BD (silhouettes[rcount1].vp_dir_from_target, 0);
                  FIELD_3BD (silhouettes[rcount1].vp_up_dir, 0);
                  FIELD_B (silhouettes[rcount1].vp_perspective, 0);
                  FIELD_BL (silhouettes[rcount1].num_wires, 0);
                  REPEAT2(silhouettes[rcount1].num_wires, silhouettes[rcount1].wires,
                          Dwg_3DSOLID_wire)
                    {
                      PARSE_WIRE_STRUCT(silhouettes[rcount1].wires[rcount2])
                    }
                  END_REPEAT(silhouettes[rcount1].wires);
                }
              END_REPEAT(silhouettes);
            }
        }
*/
      FIELD_B (acis_empty_bit, 0);
      if (!FIELD_VALUE(acis_empty_bit))
        {
          LOG_ERROR("TODO: Implement parsing of ACIS data at the end "
                    "of 3dsolid object parsing (acis_empty_bit==0).\n");
        }

      SINCE(R_2007) {
          FIELD_BL (unknown_2007, 0);
          FIELD_HANDLE (history_id, ANYCODE, 350);
      }
    }
  return error;
}

/* returns 0 on success
 */
static int
dwg_dxf_variable_type(const Dwg_Data *restrict dwg, Bit_Chain *restrict dat,
                      Dwg_Object *restrict obj)
{
  int i;
  Dwg_Class *klass;
  int is_entity;

  i = obj->type - 500;
  if (i < 0 || i >= dwg->num_classes)
    return DWG_ERR_INVALIDTYPE;

  klass = &dwg->dwg_class[i];
  if (!klass || ! klass->dxfname)
    return DWG_ERR_INTERNALERROR;
  is_entity = dwg_class_is_entity(klass);

  //if (!is_entity)
  //  fprintf(dat->fh, "  0\r\n%s\r\n", dxfname);

  #include "classes.inc"

  return DWG_ERR_UNHANDLEDCLASS;
}

static int
dwg_dxf_object(Bit_Chain *restrict dat, const Dwg_Object *restrict obj)
{
  int error = 0;
  int minimal;
  if (!obj || !obj->parent)
    return DWG_ERR_INTERNALERROR;
  minimal = obj->parent->opts & 0x10;

  if (obj->supertype == DWG_SUPERTYPE_UNKNOWN)
    return 0;

  switch (obj->type)
    {
    case DWG_TYPE_TEXT:
      return dwg_dxf_TEXT(dat, obj);
    case DWG_TYPE_ATTRIB:
      return dwg_dxf_ATTRIB(dat, obj);
    case DWG_TYPE_ATTDEF:
      return dwg_dxf_ATTDEF(dat, obj);
    case DWG_TYPE_BLOCK:
      return dwg_dxf_BLOCK(dat, obj);
    case DWG_TYPE_ENDBLK:
      return dwg_dxf_ENDBLK(dat, obj);
    case DWG_TYPE_SEQEND:
      return dwg_dxf_SEQEND(dat, obj);
    case DWG_TYPE_INSERT:
      return dwg_dxf_INSERT(dat, obj);
    case DWG_TYPE_MINSERT:
      return dwg_dxf_MINSERT(dat, obj);
    case DWG_TYPE_VERTEX_2D:
      return dwg_dxf_VERTEX_2D(dat, obj);
    case DWG_TYPE_VERTEX_3D:
      return dwg_dxf_VERTEX_3D(dat, obj);
    case DWG_TYPE_VERTEX_MESH:
      return dwg_dxf_VERTEX_MESH(dat, obj);
    case DWG_TYPE_VERTEX_PFACE:
      return dwg_dxf_VERTEX_PFACE(dat, obj);
    case DWG_TYPE_VERTEX_PFACE_FACE:
      return dwg_dxf_VERTEX_PFACE_FACE(dat, obj);
    case DWG_TYPE_POLYLINE_2D:
      return dwg_dxf_POLYLINE_2D(dat, obj);
    case DWG_TYPE_POLYLINE_3D:
      return dwg_dxf_POLYLINE_3D(dat, obj);
    case DWG_TYPE_ARC:
      return dwg_dxf_ARC(dat, obj);
    case DWG_TYPE_CIRCLE:
      return dwg_dxf_CIRCLE(dat, obj);
    case DWG_TYPE_LINE:
      return dwg_dxf_LINE(dat, obj);
    case DWG_TYPE_DIMENSION_ORDINATE:
      return dwg_dxf_DIMENSION_ORDINATE(dat, obj);
    case DWG_TYPE_DIMENSION_LINEAR:
      return dwg_dxf_DIMENSION_LINEAR(dat, obj);
    case DWG_TYPE_DIMENSION_ALIGNED:
      return dwg_dxf_DIMENSION_ALIGNED(dat, obj);
    case DWG_TYPE_DIMENSION_ANG3PT:
      return dwg_dxf_DIMENSION_ANG3PT(dat, obj);
    case DWG_TYPE_DIMENSION_ANG2LN:
      return dwg_dxf_DIMENSION_ANG2LN(dat, obj);
    case DWG_TYPE_DIMENSION_RADIUS:
      return dwg_dxf_DIMENSION_RADIUS(dat, obj);
    case DWG_TYPE_DIMENSION_DIAMETER:
      return dwg_dxf_DIMENSION_DIAMETER(dat, obj);
    case DWG_TYPE_POINT:
      return dwg_dxf_POINT(dat, obj);
    case DWG_TYPE__3DFACE:
      return dwg_dxf__3DFACE(dat, obj);
    case DWG_TYPE_POLYLINE_PFACE:
      return dwg_dxf_POLYLINE_PFACE(dat, obj);
    case DWG_TYPE_POLYLINE_MESH:
      return dwg_dxf_POLYLINE_MESH(dat, obj);
    case DWG_TYPE_SOLID:
      return dwg_dxf_SOLID(dat, obj);
    case DWG_TYPE_TRACE:
      return dwg_dxf_TRACE(dat, obj);
    case DWG_TYPE_SHAPE:
      return dwg_dxf_SHAPE(dat, obj);
    case DWG_TYPE_VIEWPORT:
      return minimal ? 0 : dwg_dxf_VIEWPORT(dat, obj);
    case DWG_TYPE_ELLIPSE:
      return dwg_dxf_ELLIPSE(dat, obj);
    case DWG_TYPE_SPLINE:
      return dwg_dxf_SPLINE(dat, obj);
    case DWG_TYPE_REGION:
      return dwg_dxf_REGION(dat, obj);
    case DWG_TYPE__3DSOLID:
      return dwg_dxf__3DSOLID(dat, obj);
    case DWG_TYPE_BODY:
      return dwg_dxf_BODY(dat, obj);
    case DWG_TYPE_RAY:
      return dwg_dxf_RAY(dat, obj);
    case DWG_TYPE_XLINE:
      return dwg_dxf_XLINE(dat, obj);
    case DWG_TYPE_DICTIONARY:
      return minimal ? 0 : dwg_dxf_DICTIONARY(dat, obj);
    case DWG_TYPE_MTEXT:
      return dwg_dxf_MTEXT(dat, obj);
    case DWG_TYPE_LEADER:
      return dwg_dxf_LEADER(dat, obj);
    case DWG_TYPE_TOLERANCE:
      return dwg_dxf_TOLERANCE(dat, obj);
    case DWG_TYPE_MLINE:
#ifdef DEBUG_CLASSES
      // TODO: looks good, but acad import crashes
      return dwg_dxf_MLINE(dat, obj);
#else
      LOG_WARN("Unhandled Entity MLINE in out_dxf %u/%lX", obj->index, obj->handle.value)
      if (0) dwg_dxf_MLINE(dat, obj);
      return DWG_ERR_UNHANDLEDCLASS;
#endif
    case DWG_TYPE_BLOCK_CONTROL:
    case DWG_TYPE_BLOCK_HEADER:
    case DWG_TYPE_LAYER_CONTROL:
    case DWG_TYPE_LAYER:
    case DWG_TYPE_STYLE_CONTROL:
    case DWG_TYPE_STYLE:
    case DWG_TYPE_LTYPE_CONTROL:
    case DWG_TYPE_LTYPE:
    case DWG_TYPE_VIEW_CONTROL:
    case DWG_TYPE_VIEW:
    case DWG_TYPE_UCS_CONTROL:
    case DWG_TYPE_UCS:
    case DWG_TYPE_VPORT_CONTROL:
    case DWG_TYPE_VPORT:
    case DWG_TYPE_APPID_CONTROL:
    case DWG_TYPE_APPID:
    case DWG_TYPE_DIMSTYLE_CONTROL:
    case DWG_TYPE_DIMSTYLE:
    case DWG_TYPE_VPORT_ENTITY_CONTROL:
    case DWG_TYPE_VPORT_ENTITY_HEADER:
      break;
    case DWG_TYPE_GROUP:
      return dwg_dxf_GROUP(dat, obj);
    case DWG_TYPE_MLINESTYLE:
      return minimal ? 0 : dwg_dxf_MLINESTYLE(dat, obj);
    case DWG_TYPE_OLE2FRAME:
      return minimal ? 0 : dwg_dxf_OLE2FRAME(dat, obj);
    case DWG_TYPE_DUMMY:
      return 0;
    case DWG_TYPE_LONG_TRANSACTION:
      return minimal ? 0 : dwg_dxf_LONG_TRANSACTION(dat, obj);
    case DWG_TYPE_LWPOLYLINE:
      return dwg_dxf_LWPOLYLINE(dat, obj);
    case DWG_TYPE_HATCH:
      return dwg_dxf_HATCH(dat, obj);
    case DWG_TYPE_XRECORD:
      return minimal ? 0 : dwg_dxf_XRECORD(dat, obj);
    case DWG_TYPE_PLACEHOLDER:
      return minimal ? 0 : dwg_dxf_PLACEHOLDER(dat, obj);
    case DWG_TYPE_PROXY_ENTITY:
      //TODO dwg_dxf_PROXY_ENTITY(dat, obj);
      return DWG_ERR_UNHANDLEDCLASS;
    case DWG_TYPE_OLEFRAME:
      return minimal ? 0 : dwg_dxf_OLEFRAME(dat, obj);
    case DWG_TYPE_VBA_PROJECT:
      if (!minimal) {
        LOG_ERROR("Unhandled Object VBA_PROJECT");
        //dwg_dxf_VBA_PROJECT(dat, obj);
        return DWG_ERR_UNHANDLEDCLASS;
      }
      return 0;
    case DWG_TYPE_LAYOUT:
      return minimal ? 0 : dwg_dxf_LAYOUT(dat, obj);
    default:
      if (obj->type == obj->parent->layout_number)
        {
          return minimal ? 0 : dwg_dxf_LAYOUT(dat, obj);
        }
      /* > 500 */
      else if ((error = dwg_dxf_variable_type(obj->parent, dat, (Dwg_Object*)obj))
               & DWG_ERR_UNHANDLEDCLASS)
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
          if (!klass)
            {
              LOG_WARN("Unknown object, skipping eed/reactors/xdic");
              return DWG_ERR_INVALIDTYPE;
            }
          return error;
        }
    }
  return DWG_ERR_UNHANDLEDCLASS;
}

static int
dxf_common_entity_handle_data(Bit_Chain *restrict dat, const Dwg_Object *restrict obj)
{
  Dwg_Object_Entity *ent;
  //Dwg_Data *dwg = obj->parent;
  Dwg_Object_Entity *_obj;
  int error = 0;
  BITCODE_BL vcount = 0;
  ent = obj->tio.entity;
  _obj = ent;

  #include "common_entity_handle_data.spec"

#if 1
  #include "common_entity_data.spec"
#else
  if (ent->picture_exists && ent->picture_size >= 0 && ent->picture_size < 210210) {
    FIELD_RL(picture_size, 160);
    VALUE_BINARY(ent->picture, ent->picture_size, 310); //chunked hex encoding
  }
  SINCE(R_2004) {
    if (ent->color.index > 0)
      FIELD_BL(color.index, 62);
    if (ent->color.flag & 0x80)
      FIELD_BL(color.rgb & 0x00ffffff, 420);
    if (ent->color.flag & 0x20) {
      if (ent->color.transparency_type == 0)
        VALUE_TV("ByLayer", 440)
      else if (ent->color.transparency_type == 1)
        VALUE_TV("ByBlock", 440)
      else if (ent->color.transparency_type == 3)
        FIELD_BL(color.alpha, 440)
    }
    if ((ent->color.flag & 0x41) == 0x41)
      FIELD_TV(color.name, 430)
    if ((ent->color.flag  & 0x42) == 0x42)
      FIELD_TV(color.book_name, 430)
  }
  if (ent->linetype_scale > 0.0)
    FIELD_BD (linetype_scale, 48)

  SINCE(R_2000)
    {
      switch (ent->linetype_flags) {
      case 1: VALUE_TV("ByBlock", 7); break;
      case 2: VALUE_TV("CONTINUOUS", 7); break;
      //case 3: HANDLE_NAME(LTYPE, 7); break;
      }
      // 00 BYLAYER, 01 BYBLOCK, 10 CONTINUOUS, 11 plotstyle handle
      FIELD_BB (plotstyle_flags, 0);
    }
  SINCE(R_2007)
    {
      FIELD_BB (material_flags, 0); //if not BYLAYER 00: 347 material handle
      FIELD_RC (shadow_flags, 284);
    }
#endif

  return error;
}

const char *
dxf_format (int code)
{
  if (0 <= code && code < 5)
    return "%s";
  if (code == 5 || code == -5)
    return "%X";
  if (5 < code && code < 10)
    return "%s";
  if (code < 60)
    return "%-16.14f";
  if (code < 80)
    return "%6i";
  if (80 <= code && code <= 99) //BL int32 lgtm [cpp/constant-comparison]
    return "%9li";
  if (code == 100)
    return "%s";
  if (code == 102)
    return "%s";
  if (code == 105)
    return "%X";
  if (110 <= code && code <= 149)
    return "%-16.14f";
  if (160 <= code && code <= 169)
    return "%12li";
  if (170 <= code && code <= 179)
    return "%6i";
  if (210 <= code && code <= 239)
    return "%-16.14f";
  if (270 <= code && code <= 289)
    return "%6i";
  if (290 <= code && code <= 299)
    return "%6i"; // boolean
  if (300 <= code && code <= 319)
    return "%s";
  if (320 <= code && code <= 369)
    return "%X";
  if (370 <= code && code <= 389)
    return "%6i";
  if (390 <= code && code <= 399)
    return "%X";
  if (400 <= code && code <= 409)
    return "%6i";
  if (410 <= code && code <= 419)
    return "%s";
  if (420 <= code && code <= 429)
    return "%9li"; //int32_t
  if (430 <= code && code <= 439)
    return "%s";
  if (440 <= code && code <= 449)
    return "%9li"; //int32_t
  if (450 <= code && code <= 459)
    return "%12li"; //long
  if (460 <= code && code <= 469)
    return "%-16.14f";
  if (470 <= code && code <= 479)
    return "%s";
  if (480 <= code && code <= 481)
    return "%X";
  if (code == 999)
    return "%s";
  if (1000 <= code && code <= 1009)
    return "%s";
  if (1010 <= code && code <= 1059)
    return "%-16.14f";
  if (1060 <= code && code <= 1070)
    return "%6i";
  if (code == 1071)
    return "%9li"; //int32_t

  return "(unknown code)";
}

const char* dxf_codepage (int code, Dwg_Data* dwg)
{
  if (code == 30 || code == 0)
    return "ANSI_1252";
  else if (code == 29)
    return "ANSI_1251";
  else if (dwg->header.version >= R_2007)
    return "UTF-8"; // dwg internally: UCS-16, for DXF: UTF-8
  else
    return "ANSI_1252";
}

// see https://www.autodesk.com/techpubs/autocad/acad2000/dxf/header_section_group_codes_dxf_02.htm
static int
dxf_header_write(Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  Dwg_Header_Variables* _obj = &dwg->header_vars;
  Dwg_Object* obj = NULL;
  double ms;
  const int minimal = dwg->opts & 0x10;
  const char* codepage = dxf_codepage(dwg->header.codepage, dwg);

  if (dwg->header.codepage != 30 &&
      dwg->header.codepage != 29 &&
      dwg->header.codepage != 0 &&
      dwg->header.version < R_2007) {
    // some asian or eastern-european codepage
    // see https://github.com/mozman/ezdxf/blob/master/docs/source/dxfinternals/fileencoding.rst
    LOG_WARN("Unknown codepage %d, assuming ANSI_1252", dwg->header.codepage);
  }

  #include "header_variables_dxf.spec"

  return 0;
}

// only called since r2000. but not really needed, unless referenced
static int
dxf_classes_write (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  unsigned int i;

  SECTION (CLASSES);
  LOG_TRACE("num_classes: %u\n", dwg->num_classes);
  for (i=0; i < dwg->num_classes; i++)
    {
      RECORD (CLASS);
      VALUE_TV (dwg->dwg_class[i].dxfname, 1);
      VALUE_T (dwg->dwg_class[i].cppname, 2);
      VALUE_T (dwg->dwg_class[i].appname, 3);
      VALUE_RS (dwg->dwg_class[i].proxyflag, 90);
      SINCE (R_2004) {
        VALUE_RC (dwg->dwg_class[i].num_instances, 91);
      }
      VALUE_RC (dwg->dwg_class[i].wasazombie, 280);
      // Is-an-entity. 1f2 for entities, 1f3 for objects
      VALUE_RC (dwg->dwg_class[i].item_class_id == 0x1F2 ? 1 : 0, 281);
    }
  ENDSEC();
  return 0;
}

static int
dxf_tables_write (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  int error = 0;
  unsigned int i;

  SECTION(TABLES);
  {
    Dwg_Object_VPORT_CONTROL *_ctrl = &dwg->vport_control;
    Dwg_Object *ctrl = &dwg->object[_ctrl->objid];
    if (ctrl)
      {
        TABLE(VPORT);
        // add handle 5 here at first
        COMMON_TABLE_CONTROL_FLAGS;
        error |= dwg_dxf_VPORT_CONTROL(dat, ctrl);
        //TODO how far back can DXF read 1000?
        if (dat->version != dat->from_version && dat->from_version >= R_2000)
          {
            /* if saved from newer version, eg. AC1032: */
            VALUE_TV ("ACAD", 1001);
            VALUE_TV ("DbSaveVer", 1000);
            VALUE_RS ((dat->from_version * 3) + 15, 1071); // so that 69 is R_2018
          }
        for (i=0; i<dwg->vport_control.num_entries; i++)
          {
            Dwg_Object *obj = dwg_ref_object(dwg, _ctrl->vports[i]);
            if (obj && obj->type == DWG_TYPE_VPORT) {
              //reordered in the DXF: 2,70,10,11,12,13,14,15,16,...
              //special-cased in the spec
              error |= dwg_dxf_VPORT(dat, obj);
            }
          }
        ENDTAB();
      }
  }
  {
    Dwg_Object_LTYPE_CONTROL *_ctrl = &dwg->ltype_control;
    Dwg_Object *obj;
    Dwg_Object *ctrl = &dwg->object[_ctrl->objid];
    if (ctrl)
      {
        TABLE(LTYPE);
        COMMON_TABLE_CONTROL_FLAGS;
        error |= dwg_dxf_LTYPE_CONTROL(dat, ctrl);
        // first the 2 builtin ltypes: ByBlock, ByLayer
        if ((obj  = dwg_ref_object(dwg, dwg->header_vars.LTYPE_BYBLOCK))) {
          dwg_dxf_LTYPE(dat, obj);
        }
        if ((obj  = dwg_ref_object(dwg, dwg->header_vars.LTYPE_BYLAYER))) {
          error |= dwg_dxf_LTYPE(dat, obj);
        }
        // here LTYPE_CONTINUOUS is already included
        for (i=0; i<dwg->ltype_control.num_entries; i++)
          {
            obj = dwg_ref_object(dwg, _ctrl->linetypes[i]);
            if (obj && obj->type == DWG_TYPE_LTYPE) {
              error |= dwg_dxf_LTYPE(dat, obj);
            }
          }
        ENDTAB();
      }
  }
  {
    Dwg_Object_LAYER_CONTROL *_ctrl = &dwg->layer_control;
    Dwg_Object *ctrl = &dwg->object[_ctrl->objid];
    if (ctrl)
      {
        TABLE(LAYER);
        COMMON_TABLE_CONTROL_FLAGS;
        error |= dwg_dxf_LAYER_CONTROL(dat, ctrl);
        for (i=0; i<dwg->layer_control.num_entries; i++)
          {
            Dwg_Object *obj = dwg_ref_object(dwg, _ctrl->layers[i]);
            if (obj && obj->type == DWG_TYPE_LAYER)
              error |= dwg_dxf_LAYER(dat, obj);
            //else if (obj && obj->type == DWG_TYPE_DICTIONARY)
            //  error |= dwg_dxf_DICTIONARY(dat, obj);
          }
        ENDTAB();
      }
  }  
  {
    Dwg_Object_STYLE_CONTROL *_ctrl = &dwg->style_control;
    Dwg_Object *ctrl = &dwg->object[_ctrl->objid];
    if (ctrl)
      {
        TABLE(STYLE);
        COMMON_TABLE_CONTROL_FLAGS;
        error |= dwg_dxf_STYLE_CONTROL(dat, ctrl);
        for (i=0; i<dwg->style_control.num_entries; i++)
          {
            Dwg_Object *obj = dwg_ref_object(dwg, _ctrl->styles[i]);
            if (obj && obj->type == DWG_TYPE_STYLE) {
              error |= dwg_dxf_STYLE(dat, obj);
            }
          }
        ENDTAB();
      }
  }
  {
    Dwg_Object_VIEW_CONTROL *_ctrl = &dwg->view_control;
    Dwg_Object *ctrl = &dwg->object[_ctrl->objid];
    TABLE(VIEW);
    COMMON_TABLE_CONTROL_FLAGS;
    error |= dwg_dxf_VIEW_CONTROL(dat, ctrl);
    for (i=0; i<dwg->view_control.num_entries; i++)
      {
        Dwg_Object *obj = dwg_ref_object(dwg, _ctrl->views[i]);
        //FIXME implement the other two
        if (obj && obj->type == DWG_TYPE_VIEW)
          error |= dwg_dxf_VIEW(dat, obj);
        /*
        else if (obj && obj->fixedtype == DWG_TYPE_SECTIONVIEWSTYLE)
          error |= dwg_dxf_SECTIONVIEWSTYLE(dat, obj);
        if (obj && obj->fixedtype == DWG_TYPE_DETAILVIEWSTYLE) {
          error |= dwg_dxf_DETAILVIEWSTYLE(dat, obj);
        */
      }
    ENDTAB();
  }
  {
    Dwg_Object_UCS_CONTROL *_ctrl = &dwg->ucs_control;
    Dwg_Object *ctrl = &dwg->object[_ctrl->objid];
    TABLE(UCS);
    COMMON_TABLE_CONTROL_FLAGS;
    error |= dwg_dxf_UCS_CONTROL(dat, ctrl);
    for (i=0; i<dwg->ucs_control.num_entries; i++)
      {
        Dwg_Object *obj = dwg_ref_object(dwg, _ctrl->ucs[i]);
        if (obj && obj->type == DWG_TYPE_UCS) {
          error |= dwg_dxf_UCS(dat, obj);
        }
      }
    ENDTAB();
  }
  SINCE (R_13)
  {
    Dwg_Object_APPID_CONTROL *_ctrl = &dwg->appid_control;
    Dwg_Object *ctrl = &dwg->object[_ctrl->objid];
    TABLE(APPID);
    COMMON_TABLE_CONTROL_FLAGS;
    error |= dwg_dxf_APPID_CONTROL(dat, ctrl);
    for (i=0; i<dwg->appid_control.num_entries; i++)
      {
        Dwg_Object *obj = dwg_ref_object(dwg, _ctrl->apps[i]);
        if (obj && obj->type == DWG_TYPE_APPID) {
          error |= dwg_dxf_APPID(dat, obj);
        }
      }
    ENDTAB();
  }
  {
    Dwg_Object_DIMSTYLE_CONTROL *_ctrl = &dwg->dimstyle_control;
    Dwg_Object *ctrl = &dwg->object[_ctrl->objid];
    TABLE(DIMSTYLE);
    COMMON_TABLE_CONTROL_FLAGS;
    error |= dwg_dxf_DIMSTYLE_CONTROL(dat, ctrl);
    //ignoring morehandles
    for (i=0; i<dwg->dimstyle_control.num_entries; i++)
      {
        Dwg_Object *obj = dwg_ref_object(dwg, _ctrl->dimstyles[i]);
        if (obj && obj->type == DWG_TYPE_DIMSTYLE) {
          error |= dwg_dxf_DIMSTYLE(dat, obj);
        }
      }
    ENDTAB();
  }
  // fool the warnings. this table is nowhere to be found in the wild. maybe pre-R_11
  if (0 && dwg->vport_entity_control.num_entries)
    {
      Dwg_Object_VPORT_ENTITY_CONTROL *_ctrl = &dwg->vport_entity_control;
      Dwg_Object *ctrl = &dwg->object[_ctrl->objid];
      TABLE(VPORT_ENTITY);
      COMMON_TABLE_CONTROL_FLAGS;
      error |= dwg_dxf_VPORT_ENTITY_CONTROL(dat, ctrl);
      for (i=0; i<dwg->vport_entity_control.num_entries; i++)
        {
          Dwg_Object *obj = dwg_ref_object(dwg, _ctrl->vport_entity_headers[i]);
          if (obj && obj->type == DWG_TYPE_VPORT_ENTITY_HEADER) {
            error |= dwg_dxf_VPORT_ENTITY_HEADER(dat, obj);
          }
        }
      // avoid unused warnings
      dwg_dxf_PROXY_ENTITY(dat, &dwg->object[0]);
      ENDTAB();
    }
  SINCE (R_13)
  {
    Dwg_Object_BLOCK_CONTROL *_ctrl = &dwg->block_control;
    Dwg_Object *ctrl = &dwg->object[_ctrl->objid];
    Dwg_Object *obj = dwg_ref_object(dwg, _ctrl->model_space);
    Dwg_Object *mspace = NULL, *pspace = NULL;

    TABLE(BLOCK_RECORD);
    COMMON_TABLE_CONTROL_FLAGS;
    error |= dwg_dxf_BLOCK_CONTROL(dat, ctrl);
    if (obj && obj->type == DWG_TYPE_BLOCK_HEADER) {
      mspace = obj;
      RECORD(BLOCK_RECORD);
      error |= dwg_dxf_BLOCK_HEADER(dat, obj);
    }
    if (_ctrl->paper_space) {
      obj = dwg_ref_object(dwg, _ctrl->paper_space);
      if (obj && obj->type == DWG_TYPE_BLOCK_HEADER) {
        pspace = obj;
        RECORD(BLOCK_RECORD);
        error |= dwg_dxf_BLOCK_HEADER(dat, obj);
      }
    }
    for (i=0; i<dwg->block_control.num_entries; i++)
      {
        obj = dwg_ref_object(dwg, dwg->block_control.block_headers[i]);
        if (obj && obj->type == DWG_TYPE_BLOCK_HEADER &&
            obj != mspace && obj != pspace)
          {
            RECORD(BLOCK_RECORD);
            error |= dwg_dxf_BLOCK_HEADER(dat, obj);
          }
      }
    ENDTAB();
  }
  ENDSEC();
  return 0;
}

static int
dxf_blocks_write (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  int error = 0;
  //unsigned int i;
  Dwg_Object_BLOCK_CONTROL *_ctrl = &dwg->block_control;
  Dwg_Object *ctrl = &dwg->object[_ctrl->objid];
  /* let's see if this control block is correct... */
  Dwg_Object_Ref *msref = dwg->header_vars.BLOCK_RECORD_MSPACE;
  Dwg_Object_Ref *psref = dwg->header_vars.BLOCK_RECORD_PSPACE;
  Dwg_Object *hdr, *obj;

  // The modelspace header needs to have an block_entity.
  // There are cases (r2010 AEC dwgs) where they don't have one.
  if (msref && msref->obj &&
      msref->obj->type == DWG_TYPE_BLOCK_HEADER &&
      msref->obj->tio.object->tio.BLOCK_HEADER->block_entity)
    hdr = msref->obj;
  else
    hdr = _ctrl->model_space->obj; // these two really should be the same

  // If there's no *Model_Space block skip this BLOCKS section.
  // Or try handle 1F with r2000+, 17 with r14
  obj = get_first_owned_block(hdr);
  if (!obj)
    obj = dwg_resolve_handle(dwg, dwg->header.version >= R_2000 ? 0x1f : 0x17);
  if (!obj)
    return 1;

  SECTION(BLOCKS);
  while (obj)
    {
      error |= dwg_dxf_object(dat, obj);
      obj = get_next_owned_block(hdr, obj);
      if (obj && obj->type == DWG_TYPE_ENDBLK)
        {
          error |= dwg_dxf_ENDBLK(dat, obj);
          obj = NULL;
        }
    }

  if (psref && psref->obj && psref->obj->tio.object->tio.BLOCK_HEADER->block_entity)
    hdr = psref->obj;
  else if (_ctrl->paper_space)
    hdr = _ctrl->paper_space->obj;
  else
    hdr = NULL;

  if (hdr) {
      obj = get_first_owned_block(hdr);
      while (obj)
        {
          error |= dwg_dxf_object(dat, obj);
          obj = get_next_owned_block(hdr, obj);
          if (obj && obj->type == DWG_TYPE_ENDBLK)
            {
              error |= dwg_dxf_ENDBLK(dat, obj);
              obj = NULL;
            }
        }
    }
  ENDSEC();
  return error;
}

static int
dxf_entities_write (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  int error = 0;
  BITCODE_BL i;

  SECTION(ENTITIES);
  for (i=0; i<dwg->num_objects; i++)
    {
      if (dwg->object[i].supertype == DWG_SUPERTYPE_ENTITY &&
          dwg->object[i].type != DWG_TYPE_BLOCK &&
          dwg->object[i].type != DWG_TYPE_ENDBLK)
        error |= dwg_dxf_object(dat, &dwg->object[i]);
    }
  ENDSEC();
  return error;
}

static int
dxf_objects_write (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  int error = 0;
  BITCODE_BL i;

  SECTION(OBJECTS);
  for (i=0; i<dwg->num_objects; i++)
    {
      if (dwg->object[i].supertype == DWG_SUPERTYPE_OBJECT)
        error |= dwg_dxf_object(dat, &dwg->object[i]);
    }
  ENDSEC();
  return error;
}

//TODO: Beware, there's also a new ACDSDATA section, with ACDSSCHEMA elements
// and the Thumbnail_Data (per block?)
static int
dxf_preview_write (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  Bit_Chain *pic = (Bit_Chain*) &dwg->picture;
  if (pic->chain && pic->size && pic->size > 10)
    {
      SECTION(THUMBNAILIMAGE);
      VALUE_RL(pic->size, 90);
      VALUE_BINARY(pic->chain, pic->size, 310);
      ENDSEC();
    }
  return 0;
}

int
dwg_write_dxf(Bit_Chain *dat, Dwg_Data * dwg)
{
  const int minimal = dwg->opts & 0x10;
  struct Dwg_Header *obj = &dwg->header;

  if (dat->from_version == R_INVALID)
    dat->from_version = dat->version;

  VALUE_TV(PACKAGE_STRING, 999);

  // A minimal header requires only $ACADVER, $HANDSEED, and then ENTITIES
  // see https://pythonhosted.org/ezdxf/dxfinternals/filestructure.html
  dxf_header_write (dat, dwg);

  if (!minimal) {
    // if downgraded from r2000 to r14, but we still have classes, keep the classes
    if ((dat->from_version >= R_2000 && dwg->num_classes) ||
        dat->version >= R_2000) {
      if (dxf_classes_write (dat, dwg) >= DWG_ERR_CRITICAL)
        goto fail;
    }

    if (dxf_tables_write (dat, dwg) >= DWG_ERR_CRITICAL)
      goto fail;

    if (dxf_blocks_write (dat, dwg) >= DWG_ERR_CRITICAL)
      goto fail;
  }

  if (dxf_entities_write (dat, dwg) >= DWG_ERR_CRITICAL)
    goto fail;

  if (!minimal) {
    SINCE(R_13) {
      if (dxf_objects_write (dat, dwg) >= DWG_ERR_CRITICAL)
        goto fail;
    }
    SINCE(R_2000) {
      if (dxf_preview_write (dat, dwg) >= DWG_ERR_CRITICAL)
        goto fail;
    }
  }
  RECORD(EOF);

  return 0;
 fail:
  return 1;
}

#undef IS_PRINT
#undef IS_DXF
