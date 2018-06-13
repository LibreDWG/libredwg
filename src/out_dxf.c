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

/* TODO: down-conversions from unsupported entities on older DXF versions:
Since r13: Entities: LWPOLYLINE, HATCH, SPLINE, LEADER, DIMENSION, MTEXT, IMAGE,
             BLOCK_RECORD.
           Table BLOCK_RECORD.
           add CLASSES for those
           handle 5, 100, 105
           non-all caps table names.
Since r14: handle 330

*/

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

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
static char buf1[255];

// private
static int
dxf_common_entity_handle_data(Bit_Chain *restrict dat, const Dwg_Object *restrict obj);
static int
dwg_dxf_object(Bit_Chain *restrict dat, const Dwg_Object *restrict obj);

/*--------------------------------------------------------------------------------
 * MACROS
 */

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
  if (dxf && value) { \
    fprintf(dat->fh, "%3i\r\n%lX\r\n", dxf, value->absolute_ref); \
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
    else if (!minimal) \
      fprintf(dat->fh, "%3i\r\n%lX\r\n", dxf, _obj->name->absolute_ref); \
  }
#define HEADER_9(name) \
    GROUP(9);\
    fprintf (dat->fh, "$%s\r\n", #name)
#define VALUE_H(value, dxf) \
  {\
    Dwg_Object_Ref *ref = value;\
    if (ref && ref->obj) { VALUE_RS(ref->absolute_ref, dxf); }\
    else { VALUE_RS(0, dxf); } \
  }
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
/* avoid empty numbers, and fixup some bad  %f formatting */
#define VALUE(value, type, dxf) \
  if (dxf) { \
    char *s; \
    const char *fmt = dxf_format (dxf);\
    GROUP(dxf);\
    snprintf (buf1, 255, "%s\r\n", fmt);\
    GCC_DIAG_IGNORE(-Wformat-nonliteral) \
    snprintf(buf, 255, buf1, value); \
    if (strcmp(fmt, "%s") && !strcmp(buf, "\r\n")) \
      snprintf (buf, 255, "0\r\n"); \
    if (!strcmp(fmt, "%-16.14f")) { \
      if (!strcmp(buf, "0.00000000000000\r\n")) \
        strcpy(buf, "0.0\r\n"); \
      else if ((s = strstr(buf, ".00000000000000\r\n"))) \
        strcpy(s, ".0\r\n"); \
      else if ((s = strstr(buf, ".50000000000000\r\n"))) \
        strcpy(s, ".5\r\n"); \
    }\
    fprintf(dat->fh, buf, value);\
    GCC_DIAG_RESTORE \
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
#define FIELD_HANDLE_N(name, vcount, handle_code, dxf) FIELD_HANDLE(name, handle_code, dxf)

#define HEADER_RC(name,dxf)  HEADER_9(name); FIELD(name, RC, dxf)
#define HEADER_RS(name,dxf)  HEADER_9(name); FIELD(name, RS, dxf)
#define HEADER_RD(name,dxf)  HEADER_9(name); FIELD(name, RD, dxf)
#define HEADER_RL(name,dxf)  HEADER_9(name); FIELD(name, RL, dxf)
#define HEADER_RLL(name,dxf) HEADER_9(name); FIELD(name, RLL, dxf)
#define HEADER_TV(name,dxf)  HEADER_9(name); VALUE_TV(_obj->name,dxf)
#define HEADER_T(name,dxf)   HEADER_9(name); VALUE_T(_obj->name, dxf)

#define VALUE_B(value,dxf)   VALUE(value, RC, dxf)
#define VALUE_BB(value,dxf)  VALUE(value, RC, dxf)
#define VALUE_3B(value,dxf)  VALUE(value, RC, dxf)
#define VALUE_BS(value,dxf)  VALUE(value, RS, dxf)
#define VALUE_BL(value,dxf)  VALUE(value, BL, dxf)
#define VALUE_BLL(value,dxf) VALUE(value, RLL, dxf)
#define VALUE_BD(value,dxf)  VALUE(value, RD, dxf)
#define VALUE_RC(value,dxf)  VALUE(value, RC, dxf)
#define VALUE_RS(value,dxf)  VALUE(value, RS, dxf)
#define VALUE_RD(value,dxf)  VALUE(value, RD, dxf)
#define VALUE_RL(value,dxf)  VALUE(value, RL, dxf)
#define VALUE_RLL(value,dxf) VALUE(value, RLL, dxf)
#define VALUE_MC(value,dxf)  VALUE(value, MC, dxf)
#define VALUE_MS(value,dxf)  VALUE(value, MS, dxf)
#define FIELD_B(name,dxf)   FIELD(name, B, dxf)
#define FIELD_BB(name,dxf)  FIELD(name, BB, dxf)
#define FIELD_3B(name,dxf)  FIELD(name, 3B, dxf)
#define FIELD_BS(name,dxf)  FIELD(name, BS, dxf)
#define FIELD_BL(name,dxf)  FIELD(name, BL, dxf)
#define FIELD_BLL(name,dxf) FIELD(name, BLL, dxf)
#define FIELD_BD(name,dxf)  FIELD(name, BD, dxf)
#define FIELD_RC(name,dxf)  FIELD(name, RC, dxf)
#define FIELD_RS(name,dxf)  FIELD(name, RS, dxf)
#define FIELD_RD(name,dxf)  FIELD(name, RD, dxf)
#define FIELD_RL(name,dxf)  FIELD(name, RL, dxf)
#define FIELD_RLL(name,dxf) FIELD(name, RLL, dxf)
#define FIELD_MC(name,dxf)  FIELD(name, MC, dxf)
#define FIELD_MS(name,dxf)  FIELD(name, MS, dxf)
#define FIELD_TF(name,len,dxf)  VALUE_TV(_obj->name, dxf)
#define FIELD_TFF(name,len,dxf) VALUE_TV(_obj->name, dxf)
#define FIELD_TV(name,dxf) \
  if (dxf != 0) { VALUE_TV(_obj->name,dxf); }
#define FIELD_TU(name,dxf) \
  if (dxf != 0) { VALUE_TU((BITCODE_TU)_obj->name, dxf); }
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
#define FIELD_2RD(name,dxf) {FIELD(name.x, RD, dxf); FIELD(name.y, RD, dxf+10);}
#define FIELD_2BD(name,dxf) {FIELD(name.x, BD, dxf); FIELD(name.y, BD, dxf+10);}
#define FIELD_2BD_1(name,dxf) {FIELD(name.x, BD, dxf); FIELD(name.y, BD, dxf+1);}
#define FIELD_3RD(name,dxf) {FIELD(name.x, RD, dxf); FIELD(name.y, RD, dxf+10); FIELD(name.z, RD, dxf+20);}
#define FIELD_3BD(name,dxf) {FIELD(name.x, BD, dxf); FIELD(name.y, BD, dxf+10); FIELD(name.z, BD, dxf+20);}
#define FIELD_3BD_1(name,dxf) {FIELD(name.x, BD, dxf); FIELD(name.y, BD, dxf+1); FIELD(name.z, BD, dxf+2);}
#define FIELD_3DPOINT(name,dxf) FIELD_3BD(name,dxf)
#define FIELD_CMC(name,dxf)\
  VALUE_RS(_obj->name.index, dxf)
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
    VALUE(dwg->var.x, BD, c1);\
    VALUE(dwg->var.y, BD, c2);\
    VALUE(dwg->var.z, BD, c3);\
  }
#define POINT_2D(name, var, c1, c2) \
  {\
    VALUE(dwg->var.x, BD, c1);\
    VALUE(dwg->var.y, BD, c2);\
  }

//FIELD_VECTOR_N(name, type, size):
// reads data of the type indicated by 'type' 'size' times and stores
// it all in the vector called 'name'.
#define FIELD_VECTOR_N(name, type, size, dxf)\
  if (dxf)\
    {\
      for (vcount=0; vcount < (int)size; vcount++)\
        {\
          VALUE_##type(_obj->name[vcount], dxf); \
        }\
    }
#define FIELD_VECTOR_T(name, size, dxf)\
  if (dxf) {\
    PRE (R_2007) {                                                   \
      for (vcount=0; vcount < (int)_obj->size; vcount++)             \
        VALUE_TV(_obj->name[vcount], dxf);                           \
    } else {                                                         \
      for (vcount=0; vcount < (int)_obj->size; vcount++)             \
        VALUE_TU(_obj->name[vcount], dxf);                           \
    }                                                                \
  }

#define FIELD_VECTOR(name, type, size, dxf) FIELD_VECTOR_N(name, type, _obj->size, dxf)

#define FIELD_2RD_VECTOR(name, size, dxf)\
  if (dxf) {\
    for (vcount=0; vcount < (int)_obj->size; vcount++)    \
      {\
        FIELD_2RD(name[vcount], dxf);\
      }\
  }

#define FIELD_2DD_VECTOR(name, size, dxf)\
  FIELD_2RD(name[0], dxf);\
  for (vcount = 1; vcount < (int)_obj->size; vcount++)\
    {\
      FIELD_2DD(name[vcount], FIELD_VALUE(name[vcount - 1].x), FIELD_VALUE(name[vcount - 1].y), dxf);\
    }\

#define FIELD_3DPOINT_VECTOR(name, size, dxf)\
  if (dxf) {\
    for (vcount=0; vcount < (int)_obj->size; vcount++)\
      {\
        FIELD_3DPOINT(name[vcount], dxf);\
      }\
    }

#define HANDLE_VECTOR_N(name, size, code, dxf) \
  if (dxf) {\
    for (vcount=0; vcount < (int)size; vcount++)\
      {\
        FIELD_HANDLE_N(name[vcount], vcount, code, dxf);\
      }\
    }

#define HANDLE_VECTOR(name, sizefield, code, dxf) \
  HANDLE_VECTOR_N(name, FIELD_VALUE(sizefield), code, dxf)

#define FIELD_NUM_INSERTS(num_inserts, type, dxf) \
  FIELD(num_inserts, type, dxf)

#define FIELD_XDATA(name, size) \
  dxf_write_xdata(dat, _obj->name, _obj->size)

#define REACTORS(code)\
  if (obj->tio.object->num_reactors) {\
    fprintf(dat->fh, "102\r\n{ACAD_REACTORS\r\n");\
    for (vcount=0; vcount < (int)obj->tio.object->num_reactors; vcount++)\
      { /* soft ptr */ \
        fprintf(dat->fh, "330\r\n"); \
        FIELD_HANDLE_N(reactors[vcount], vcount, code, -5);\
      }\
    fprintf(dat->fh, "102\r\n}\r\n");\
  }
#define ENT_REACTORS(code)\
  if (_obj->num_reactors) {\
    fprintf(dat->fh, "102\r\n{ACAD_REACTORS\r\n");\
    for (vcount=0; vcount < _obj->num_reactors; vcount++)\
      {\
        fprintf(dat->fh, "330\r\n"); \
        FIELD_HANDLE_N(reactors[vcount], vcount, code, -5);\
      }\
    fprintf(dat->fh, "102\r\n}\r\n");\
  }

#define XDICOBJHANDLE(code)
#define ENT_XDICOBJHANDLE(code)

#define COMMON_ENTITY_HANDLE_DATA
#define SECTION_STRING_STREAM
#define START_STRING_STREAM
#define END_STRING_STREAM
#define START_HANDLE_STREAM

//The strcmp is being optimized away at compile-time! https://godbolt.org/g/AqkhwL
#define DWG_ENTITY(token) \
static int \
dwg_dxf_##token (Bit_Chain *restrict dat, const Dwg_Object *restrict obj) \
{\
  int error = 0; \
  long vcount, rcount1, rcount2, rcount3, rcount4; \
  Dwg_Entity_##token *ent, *_obj;\
  Dwg_Object_Entity *_ent;\
  const int minimal = obj->parent->opts & 0x10;\
  if (!strcmp(#token, "GEOPOSITIONMARKER"))\
    RECORD(POSITIONMARKER);\
  else if (dat->version < R_13 && !strcmp(#token, "LWPOLYLINE")) \
    RECORD(POLYLINE);\
  else\
    RECORD(token);\
  _ent = obj->tio.entity;\
  _obj = ent = _ent->tio.token;\
  LOG_INFO("Entity " #token ":\n")\
  LOG_TRACE("Entity handle: %d.%d.%lX\n",\
            obj->handle.code,\
            obj->handle.size,\
            obj->handle.value); \
  SINCE(R_2000) { \
    fprintf(dat->fh, "%3i\r\n%lX\r\n", 5, obj->handle.value); \
    VALUE_HANDLE (obj->parent->header_vars.BLOCK_RECORD_MSPACE, 5, 330); \
    VALUE_TV ("AcDbEntity", 100); \
   } \
  SINCE(R_13) { \
    error |= dxf_common_entity_handle_data(dat, obj); \
  }

#define DWG_ENTITY_END return error; \
}

#define DWG_OBJECT(token) \
static int \
dwg_dxf_ ##token (Bit_Chain *restrict dat, const Dwg_Object *restrict obj) \
{ \
  int error = 0; \
  long vcount, rcount1, rcount2, rcount3, rcount4;\
  Bit_Chain *hdl_dat = dat;\
  Dwg_Object_##token *_obj;\
  const int minimal = obj->parent->opts & 0x10;\
  LOG_INFO("Object " #token ":\n")\
  _obj = obj->tio.object->tio.token;\
  if (!dwg_obj_is_control(obj)) { \
    int dxf = 5; \
    if (obj->type != DWG_TYPE_BLOCK_HEADER) RECORD(token); \
    if (obj->type == DWG_TYPE_DIMSTYLE) dxf = 105; \
    fprintf(dat->fh, "%3i\r\n%lX\r\n", dxf, obj->handle.value); \
  } \
  LOG_TRACE("Object handle: %d.%d.%lX\n",\
            obj->handle.code,   \
            obj->handle.size,   \
            obj->handle.value)

#define DWG_OBJECT_END return error; \
}

static int
dxf_write_xdata(Bit_Chain *restrict dat, Dwg_Resbuf *restrict rbuf, BITCODE_BL size)
{
  Dwg_Resbuf *tmp;
  int i;

  while (rbuf)
    {
      //const char* fmt = dxf_format(rbuf->type);
      short type = get_base_value_type(rbuf->type);

      tmp = rbuf->next;
      switch (type)
        {
        case VT_STRING:
          UNTIL(R_2007) {
            VALUE_TV(rbuf->value.str.u.data, rbuf->type);
          } LATER_VERSIONS {
            VALUE_TU(rbuf->value.str.u.wdata, rbuf->type);
          }
          break;
        case VT_REAL:
          VALUE_RD(rbuf->value.dbl, rbuf->type);
          break;
        case VT_BOOL:
        case VT_INT8:
          VALUE_RC(rbuf->value.i8, rbuf->type);
          break;
        case VT_INT16:
          VALUE_RS(rbuf->value.i16, rbuf->type);
          break;
        case VT_INT32:
          VALUE_RL(rbuf->value.i32, rbuf->type);
          break;
        case VT_POINT3D:
          VALUE_RD(rbuf->value.pt[0], rbuf->type);
          VALUE_RD(rbuf->value.pt[1], rbuf->type+1);
          VALUE_RD(rbuf->value.pt[2], rbuf->type+2);
          break;
        case VT_BINARY:
          VALUE_BINARY(rbuf->value.str.u.data, rbuf->value.str.size, rbuf->type);
          break;
        case VT_HANDLE:
        case VT_OBJECTID:
          fprintf(dat->fh, "%lX\r\n", (unsigned long)*(uint64_t*)rbuf->value.hdl);
          break;
        case VT_INVALID:
        default:
          fprintf(dat->fh, "\r\n");
          break;
        }
      rbuf = tmp;
    }
  return 0;
}

// r2000+ converts STANDARD to Standard, BYLAYER to ByLayer, BYBLOCK to ByBlock
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
      if (dat->from_version >= R_2000 && dat->version < R_2000)
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
          if (dat->version >= R_2000 && !strcmp(entry_name, "STANDARD"))
            fprintf(dat->fh, "%3i\r\nStandard\r\n", dxf);
          else if (dat->version >= R_2000 && !strcmp(entry_name, "BYLAYER"))
            fprintf(dat->fh, "%3i\r\nByLayer\r\n", dxf);
          else if (dat->version >= R_2000 && !strcmp(entry_name, "BYBLOCK"))
            fprintf(dat->fh, "%3i\r\nByBlock\r\n", dxf);
          else if (dat->version >= R_2000 && !strcmp(entry_name, "*ACTIVE"))
            fprintf(dat->fh, "%3i\r\n*Active\r\n", dxf);
          else
            fprintf(dat->fh, "%3i\r\n%s\r\n", dxf, entry_name);
        }
    }
  else {
    fprintf(dat->fh, "%3i\r\n\r\n", dxf);
  }
}

//TODO: 5 330 100 70. have: 330 100 5 70
#define COMMON_TABLE_CONTROL_FLAGS(owner, acdbname) \
    fprintf(dat->fh, "%3i\r\n%lX\r\n", 5, ctrl->handle.value); \
    VALUE_H (_ctrl->null_handle, 330); \
    if (dat->from_version >= R_2000) { \
      VALUE_TV ("AcDbSymbolTable", 100); \
    }

#define COMMON_TABLE_FLAGS(owner, acdbname) \
  if (!minimal) { \
    FIELD_HANDLE (owner, 4, 330); \
    if (dat->from_version >= R_2000) { \
      VALUE_TV ("AcDbSymbolTableRecord", 100); \
      VALUE_TV ("AcDb" #acdbname "TableRecord", 100); \
    }\
  } \
  if (_obj->entry_name) dxf_cvt_tablerecord(dat, obj, _obj->entry_name, 2); \
  FIELD_RC (flag, 70);

#define LAYER_TABLE_FLAGS(owner, acdbname) \
  if (!minimal) { \
    FIELD_HANDLE (owner, 4, 330); \
    if (dat->from_version >= R_2000) { \
      VALUE_TV ("AcDbSymbolTableRecord", 100); \
      VALUE_TV ("AcDb" #acdbname "TableRecord", 100); \
    }\
  } \
  if (_obj->entry_name) dxf_cvt_tablerecord(dat, obj, _obj->entry_name, 2); \
  FIELD_RS (flag, 70);

#include "dwg.spec"


/* returns 0 on success
 */
static int
dwg_dxf_variable_type(const Dwg_Data *restrict dwg, Bit_Chain *restrict dat,
                      Dwg_Object *restrict obj)
{
  int i;
  char *dxfname;
  Dwg_Class *klass;
  int is_entity;

  i = obj->type - 500;
  if (i < 0 || i >= dwg->num_classes)
    return DWG_ERR_INVALIDTYPE;

  klass = &dwg->dwg_class[i];
  if (!klass || ! klass->dxfname)
    return DWG_ERR_INTERNALERROR;
  dxfname = klass->dxfname;
  // almost always false
  is_entity = dwg_class_is_entity(klass);

  //if (!is_entity)
  //  fprintf(dat->fh, "  0\r\n%s\r\n", dxfname);

  #define action dxf
  #include "classes.inc"

  return DWG_ERR_UNHANDLEDCLASS;
}

static int
dwg_dxf_object(Bit_Chain *restrict dat, const Dwg_Object *restrict obj)
{
  int error = 0;
  const int minimal = obj->parent->opts & 0x10;

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
      return dwg_dxf_MLINE(dat, obj);
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
      return dwg_dxf_MLINESTYLE(dat, obj);
    case DWG_TYPE_OLE2FRAME:
      return minimal ? 0 : dwg_dxf_OLE2FRAME(dat, obj);
    case DWG_TYPE_DUMMY:
      break;
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
      //dwg_dxf_PROXY_ENTITY(dat, obj);
      return 0;
    case DWG_TYPE_OLEFRAME:
      return minimal ? 0 : dwg_dxf_OLEFRAME(dat, obj);
    case DWG_TYPE_VBA_PROJECT:
      if (!minimal) {
        LOG_ERROR("Unhandled Object VBA_PROJECT. Has its own section\n");
        //dwg_dxf_VBA_PROJECT(dat, obj);
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
              SINCE(R_2000)
                {
                  LOG_INFO("Object bitsize: %u\n", obj->bitsize)
                }
              LOG_INFO("Object handle: %d.%d.%lX\n",
                       obj->handle.code, obj->handle.size, obj->handle.value);
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
  int i, error = 0;
  long unsigned int vcount = 0;
  ent = obj->tio.entity;
  _obj = ent;

  #include "common_entity_handle_data.spec"

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
  if (90 <= code && code <= 99) //BL
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
  const int minimal = 0; //dwg->opts & 0x10;

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
  const int minimal = 0; //dwg->opts & 0x10;

  SECTION(TABLES);
  {
    Dwg_Object_VPORT_CONTROL *_ctrl = &dwg->vport_control;
    Dwg_Object *ctrl = &dwg->object[_ctrl->objid];
    TABLE(VPORT);
    // add handle 5 here at first
    COMMON_TABLE_CONTROL_FLAGS(null_handle, Viewport);
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
        Dwg_Object *obj = dwg_ref_get_object(dwg, _ctrl->vports[i]);
        if (obj) {
          //reordered in the DXF: 2,70,10,11,12,13,14,15,16,...
          //special-cased in the spec
          error |= dwg_dxf_VPORT(dat, obj);
        }
      }
    ENDTAB();
  }
  {
    Dwg_Object_LTYPE_CONTROL *_ctrl = &dwg->ltype_control;
    Dwg_Object *ctrl = &dwg->object[_ctrl->objid];
    Dwg_Object *obj;
    TABLE(LTYPE);
    COMMON_TABLE_CONTROL_FLAGS(null_handle, Linetype);
    error |= dwg_dxf_LTYPE_CONTROL(dat, ctrl);
    // first the 2 builtin ltypes: ByBlock, ByLayer
    if ((obj  = dwg_ref_get_object(dwg, dwg->header_vars.LTYPE_BYBLOCK))) {
      dwg_dxf_LTYPE(dat, obj);
    }
    if ((obj  = dwg_ref_get_object(dwg, dwg->header_vars.LTYPE_BYLAYER))) {
      error |= dwg_dxf_LTYPE(dat, obj);
    }
    // here LTYPE_CONTINUOUS is already included
    for (i=0; i<dwg->ltype_control.num_entries; i++)
      {
        obj = dwg_ref_get_object(dwg, _ctrl->linetypes[i]);
        if (obj && obj->supertype == DWG_SUPERTYPE_OBJECT) {
          error |= dwg_dxf_LTYPE(dat, obj);
        }
      }
    ENDTAB();
  }
  {
    Dwg_Object_LAYER_CONTROL *_ctrl = &dwg->layer_control;
    Dwg_Object *ctrl = &dwg->object[_ctrl->objid];
    TABLE(LAYER);
    COMMON_TABLE_CONTROL_FLAGS(null_handle, Layer);
    error |= dwg_dxf_LAYER_CONTROL(dat, ctrl);
    for (i=0; i<dwg->layer_control.num_entries; i++)
      {
        Dwg_Object *obj = dwg_ref_get_object(dwg, _ctrl->layers[i]);
        if (obj && obj->supertype == DWG_SUPERTYPE_OBJECT) {
          error |= dwg_dxf_LAYER(dat, obj);
        }
      }
    ENDTAB();
  }  
  {
    Dwg_Object_STYLE_CONTROL *_ctrl = &dwg->style_control;
    Dwg_Object *ctrl = &dwg->object[_ctrl->objid];
    TABLE(STYLE);
    COMMON_TABLE_CONTROL_FLAGS(null_handle, TextStyle);
    error |= dwg_dxf_STYLE_CONTROL(dat, ctrl);
    for (i=0; i<dwg->style_control.num_entries; i++)
      {
        Dwg_Object *obj = dwg_ref_get_object(dwg, _ctrl->styles[i]);
        if (obj && obj->supertype == DWG_SUPERTYPE_OBJECT) {
          error |= dwg_dxf_STYLE(dat, obj);
        }
      }
    ENDTAB();
  }
  {
    Dwg_Object_VIEW_CONTROL *_ctrl = &dwg->view_control;
    Dwg_Object *ctrl = &dwg->object[_ctrl->objid];
    TABLE(VIEW);
    COMMON_TABLE_CONTROL_FLAGS(null_handle, View);
    error |= dwg_dxf_VIEW_CONTROL(dat, ctrl);
    for (i=0; i<dwg->view_control.num_entries; i++)
      {
        Dwg_Object *obj = dwg_ref_get_object(dwg, _ctrl->views[i]);
        //FIXME ignore ACDBSECTIONVIEWSTYLE and ACDBDETAILVIEWSTYLE
        if (obj && obj->supertype == DWG_SUPERTYPE_OBJECT) {
          error |= dwg_dxf_VIEW(dat, obj);
        }
      }
    ENDTAB();
  }
  {
    Dwg_Object_UCS_CONTROL *_ctrl = &dwg->ucs_control;
    Dwg_Object *ctrl = &dwg->object[_ctrl->objid];
    TABLE(UCS);
    COMMON_TABLE_CONTROL_FLAGS(null_handle, UCS);
    error |= dwg_dxf_UCS_CONTROL(dat, ctrl);
    for (i=0; i<dwg->ucs_control.num_entries; i++)
      {
        Dwg_Object *obj = dwg_ref_get_object(dwg, _ctrl->ucs[i]);
        if (obj && obj->supertype == DWG_SUPERTYPE_OBJECT) {
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
    COMMON_TABLE_CONTROL_FLAGS(null_handle, RegApp);
    error |= dwg_dxf_APPID_CONTROL(dat, ctrl);
    for (i=0; i<dwg->appid_control.num_entries; i++)
      {
        Dwg_Object *obj = dwg_ref_get_object(dwg, _ctrl->apps[i]);
        if (obj && obj->supertype == DWG_SUPERTYPE_OBJECT) {
          error |= dwg_dxf_APPID(dat, obj);
        }
      }
    ENDTAB();
  }
  {
    Dwg_Object_DIMSTYLE_CONTROL *_ctrl = &dwg->dimstyle_control;
    Dwg_Object *ctrl = &dwg->object[_ctrl->objid];
    TABLE(DIMSTYLE);
    COMMON_TABLE_CONTROL_FLAGS(null_handle, DimStyle);
    error |= dwg_dxf_DIMSTYLE_CONTROL(dat, ctrl);
    //ignoring morehandles
    for (i=0; i<dwg->dimstyle_control.num_entries; i++)
      {
        Dwg_Object *obj = dwg_ref_get_object(dwg, _ctrl->dimstyles[i]);
        if (obj && obj->supertype == DWG_SUPERTYPE_OBJECT) {
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
      COMMON_TABLE_CONTROL_FLAGS(null_handle, Viewport);
      error |= dwg_dxf_VPORT_ENTITY_CONTROL(dat, ctrl);
      for (i=0; i<dwg->vport_entity_control.num_entries; i++)
        {
          Dwg_Object *obj = dwg_ref_get_object(dwg, _ctrl->vport_entity_headers[i]);
          if (obj && obj->supertype == DWG_SUPERTYPE_OBJECT) {
            error |= dwg_dxf_VPORT_ENTITY_HEADER(dat, obj);
          }
        }
      // avoid unused warnings
      dwg_dxf_PROXY_ENTITY(dat, &dwg->object[0]);
      dwg_dxf_UNKNOWN_ENT(dat, &dwg->object[0]);
      dwg_dxf_UNKNOWN_OBJ(dat, &dwg->object[0]);
      ENDTAB();
    }
  SINCE (R_13)
  {
    Dwg_Object_BLOCK_CONTROL *_ctrl = &dwg->block_control;
    Dwg_Object *ctrl = &dwg->object[_ctrl->objid];
    Dwg_Object *obj = dwg_ref_get_object(dwg, _ctrl->model_space);
    TABLE(BLOCK_RECORD);
    COMMON_TABLE_CONTROL_FLAGS(null_handle, BlockTable);
    error |= dwg_dxf_BLOCK_CONTROL(dat, ctrl);
    if (obj && obj->supertype == DWG_SUPERTYPE_OBJECT) {
      RECORD(BLOCK_RECORD);
      error |= dwg_dxf_BLOCK_HEADER(dat, obj);
    }
    if (_ctrl->paper_space) {
      obj = dwg_ref_get_object(dwg, _ctrl->paper_space);
      if (obj && obj->supertype == DWG_SUPERTYPE_OBJECT) {
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
  unsigned int i;
  Dwg_Object *mspace = NULL, *pspace = NULL;
  Dwg_Object_BLOCK_CONTROL *_ctrl = &dwg->block_control;
  Dwg_Object *ctrl = &dwg->object[_ctrl->objid];
  const int minimal = 0; //dwg->opts & 0x10;

  SECTION(BLOCKS);
  COMMON_TABLE_CONTROL_FLAGS(null_handle, Block);
  dwg_dxf_BLOCK_CONTROL(dat, ctrl);
  if (_ctrl->model_space)
    {
      Dwg_Object *obj = dwg_ref_get_object(dwg, _ctrl->model_space);
      if (obj) {
        mspace = obj;
        assert(obj->type == DWG_TYPE_BLOCK_HEADER);
        RECORD (BLOCK);
        error |= dwg_dxf_BLOCK_HEADER(dat, obj);
      }
    }
  if (dwg->block_control.paper_space)
    {
      Dwg_Object *obj = dwg_ref_get_object(dwg, dwg->block_control.paper_space);
      if (obj) {
        pspace = obj;
        assert(obj->type == DWG_TYPE_BLOCK_HEADER);
        RECORD (BLOCK);
        error |= dwg_dxf_BLOCK_HEADER(dat, obj);
      }
    }
  for (i=0; i<dwg->block_control.num_entries; i++)
    {
      Dwg_Object *obj = dwg_ref_get_object(dwg, dwg->block_control.block_headers[i]);
      if (obj && obj != mspace && obj != pspace)
        {
          assert(obj->type == DWG_TYPE_BLOCK_HEADER);
          RECORD (BLOCK);
          error |= dwg_dxf_BLOCK_HEADER(dat, obj);
        }
    }
  ENDSEC();
  return 0;
}

static int
dxf_entities_write (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  int error = 0;
  long unsigned int i;

  SECTION(ENTITIES);
  for (i=0; i<dwg->num_objects; i++)
    {
      if (dwg->object[i].supertype == DWG_SUPERTYPE_ENTITY &&
          dwg->object[i].type != DWG_TYPE_BLOCK &&
          dwg->object[i].type != DWG_TYPE_ENDBLK)
        error |= dwg_dxf_object(dat, &dwg->object[i]);
    }
  ENDSEC();
  return 0;
}

static int
dxf_objects_write (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  int error = 0;
  long unsigned int i;

  SECTION(OBJECTS);
  for (i=0; i<dwg->num_objects; i++)
    {
      if (dwg->object[i].supertype == DWG_SUPERTYPE_OBJECT)
        error |= dwg_dxf_object(dat, &dwg->object[i]);
    }
  ENDSEC();
  return 0;
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
      if (dxf_classes_write (dat, dwg))
        goto fail;
    }

    if (dxf_tables_write (dat, dwg))
      goto fail;

    if (dxf_blocks_write (dat, dwg))
      goto fail;
  }

  if (dxf_entities_write (dat, dwg))
    goto fail;

  if (!minimal) {
    SINCE(R_13) {
      if (dxf_objects_write (dat, dwg))
        goto fail;
    }
    SINCE(R_2000) {
      if (dxf_preview_write (dat, dwg))
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
