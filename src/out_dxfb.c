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
 * out_dxfb.c: write as Binary DXF
 * written by Reini Urban
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
static char buf[4096];

//private
static int
dxfb_common_entity_handle_data(Bit_Chain *restrict dat,
                               const Dwg_Object *restrict obj);

/*--------------------------------------------------------------------------------
 * MACROS
 */

#define ACTION dxfb
#define IS_PRINT
#define IS_DXF

#define FIELD(name,type,dxf) \
    if (dxf) { FIELD_##type(name, dxf); }

#define HEADER_VALUE(name, type, dxf, value) \
    GROUP(9);\
    fprintf (dat->fh, "$%s%c", #name, 0);\
    VALUE_##type(value, dxf)

#define HEADER_VAR(name, type, dxf) \
  HEADER_VALUE(name, type, dxf, dwg->header_vars.name)

#define FIELD_CAST(name,type,cast,dxf) FIELD(name,cast,dxf)
#define FIELD_TRACE(name,type)
//TODO length?
#define VALUE_TV(value,dxf) \
  { GROUP(dxf); \
    fprintf(dat->fh, "%s%c", value, 0); }
#ifdef HAVE_NATIVE_WCHAR2
# define VALUE_TU(value,dxf)\
  { GROUP(dxf); \
    fprintf(dat->fh, "%ls%c%c", (wchar_t*)value, 0, 0); }
#else
# define VALUE_TU(wstr,dxf) \
  { \
    BITCODE_TU ws = (BITCODE_TU)wstr; \
    uint16_t _c; \
    GROUP(dxf);\
    while ((_c = *ws++)) { \
      fprintf(dat->fh, "%c", (char)(_c & 0xff)); \
    } \
    fprintf(dat->fh, "%c%c", 0, 0); \
  }
#endif
#define VALUE_TFF(str,dxf)  VALUE_TV(str, dxf)
#define VALUE_BINARY(value,size,dxf) \
{ \
  long len = size; \
  do { \
    short j; \
    long l = len > 127 ? 127 : len; \
    GROUP(dxf); \
    if (value) \
      for (j=0; j < l; j++) { \
        fprintf(dat->fh, "%c", value[j]); \
      } \
    fprintf(dat->fh, "%c", '\0'); \
    len -= 127; \
  } while (len > 127); \
}

#define FIELD_VALUE(name) _obj->name
#define ANYCODE -1
//TODO
#define VALUE_HANDLE(hdlptr, handle_code, dxf) \
  { \
     uint32_t _j = (uint32_t)hdlptr->absolute_ref; \
     GROUP(dxf); \
     fwrite(&_j, sizeof(uint32_t), 1, dat->fh); \
  }
#define FIELD_HANDLE(name, handle_code, dxf) VALUE_HANDLE(_obj->name, handle_code, dxf)

#define GROUP(code)                  \
    {                                \
    if (dat->version < R_14) {       \
      unsigned char icode = (unsigned char)code; \
      fwrite(&icode, sizeof(unsigned char), 1, dat->fh); \
    } else {                         \
      short icode = code;            \
      fwrite(&icode, sizeof(short), 1, dat->fh); \
    }                                \
  }
#define FIELD_TV(name,dxf) \
  if (_obj->name != NULL && dxf != 0) { VALUE_TV(_obj->name,dxf) }
#define FIELD_TU(name,dxf) \
  if (_obj->name != NULL && dxf != 0) { VALUE_TU(_obj->name, dxf) }
#define VALUE_T(value,dxf) \
  { if (dat->version >= R_2007) VALUE_TU(value, dxf) \
    else                        VALUE_TV(value, dxf) }
#define FIELD_T(name,dxf) \
  { if (dat->version >= R_2007) { FIELD_TU(name, dxf) } \
    else                        { FIELD_TV(name, dxf) } }
#define FIELD_TF(name,len,dxf)  VALUE_TV(_obj->name, dxf)
#define FIELD_TFF(name,len,dxf) VALUE_TV(_obj->name, dxf)

#define HEADER_TV(name,dxf) \
    HEADER_9(name);\
    VALUE_TV(dwg->header_vars.name, dxf)
#define HEADER_TU(name,dxf) \
    HEADER_9(name);\
    VALUE_TU(dwg->header_vars.name, dxf)
#define HEADER_T(name,dxf) \
    HEADER_9(name);\
    VALUE_T(dwg->header_vars.name, dxf)
#define POINT_3D(name, var, c1, c2, c3)\
    VALUE_RD(dwg->var.x, c1);\
    VALUE_RD(dwg->var.y, c2);\
    VALUE_RD(dwg->var.z, c3)
#define POINT_2D(name, var, c1, c2) \
    VALUE_RD(dwg->var.x, c1);\
    VALUE_RD(dwg->var.x, c2)
#define HEADER_3D(name)\
    HEADER_9(name);\
    POINT_3D (name, header_vars.name, 10, 20, 30)
#define HEADER_2D(name)\
    HEADER_9(name);\
    POINT_2D (name, header_vars.name, 10, 20)
#define SECTION(token) \
    VALUE_TV("SECTION", 0);\
    VALUE_TV(#token, 2)
#define ENDSEC()        VALUE_TV("ENDSEC", 0)
#define TABLE(token) \
    VALUE_TV("TABLE", 0);\
    VALUE_TV(#token, 2)
#define ENDTAB()        VALUE_TV("ENDTAB", 0)
#define RECORD(record)  VALUE_TV(#record, 0)
#define SUBCLASS(text)  if (dat->from_version >= R_2000) { VALUE_TV(#text, 100) }

/*
#define VALUE(code, value)                   \
  {\
    GCC_DIAG_IGNORE(-Wformat-nonliteral) \
    snprintf (buf, 4096, "%3i\n%s\n", code, dxfb_format (code));\
    fprintf(dat->fh, buf, value);\
    GCC_DIAG_RESTORE \
  }
 */

#define FIELD_DATAHANDLE(name, code, dxf) FIELD_HANDLE(name, code, dxf)
#define FIELD_HANDLE_N(name, vcount, handle_code, dxf) FIELD_HANDLE(name, handle_code, dxf)

#define FIELD_B(name,dxf)   FIELD_RC(name, dxf)
#define FIELD_BB(name,dxf)  FIELD_RC(name, dxf)
#define FIELD_3B(name,dxf)  FIELD_RC(name, dxf)
#define FIELD_BS(name,dxf)  FIELD_RS(name, dxf)
#define FIELD_BL(name,dxf)  FIELD_RL(name, dxf)
#define HEADER_BLL(name,dxf) HEADER_RLL(name, dxf)
#define FIELD_BD(name,dxf)  FIELD_RD(name, dxf)

#define HEADER_9(name) \
    GROUP(9);\
    fprintf (dat->fh, "$%s%c", #name, 0)
#define VALUE(value,type,dxf) VALUE_##type(value,dxf)
#define VALUE_B(value,dxf) VALUE_RC(value,dxf)
#define VALUE_BB(value,dxf) VALUE_RC(value,dxf)
#define VALUE_3B(value,dxf) VALUE_RC(value,dxf)
#define VALUE_BS(value,dxf) VALUE_RS(value,dxf)
#define VALUE_BL(value,dxf) VALUE_RL(value,dxf)
#define VALUE_BD(value,dxf) VALUE_RD(value,dxf)
#define VALUE_RC(value,dxf) \
  {\
    BITCODE_RC c = (value); \
    GROUP(dxf);\
    fwrite(&c, sizeof(BITCODE_RC), 1, dat->fh); \
  }
#define FIELD_RC(name,dxf) VALUE_RC(_obj->name, dxf)
#define HEADER_RC(name,dxf) \
    HEADER_9(name);\
    VALUE_RC(dwg->header_vars.name, dxf)
#define HEADER_B(name,dxf) HEADER_RC(name,dxf)

#define VALUE_RS(value,dxf) \
  {\
    BITCODE_RS s = (value); \
    GROUP(dxf);\
    fwrite(&s, sizeof(BITCODE_RS), 1, dat->fh);\
  }
#define FIELD_RS(name,dxf) VALUE_RS(_obj->name, dxf)
#define HEADER_RS(name,dxf) \
    HEADER_9(name);\
    VALUE_RS(dwg->header_vars.name, dxf)

#define VALUE_RD(value,dxf)\
  {\
    double d = (value);\
    GROUP(dxf);\
    fwrite(&d, sizeof(double), 1, dat->fh); \
  }
#define FIELD_RD(name,dxf) VALUE_RD(_obj->name,dxf)
#define HEADER_RD(name,dxf) \
    HEADER_9(name);\
    VALUE_RD(dwg->header_vars.name, dxf)

#define VALUE_RL(value,dxf)\
  {\
    BITCODE_RL _s = value;\
    GROUP(dxf);\
    fwrite(&_s, sizeof(BITCODE_RL), 1, dat->fh);\
  }
#define FIELD_RL(name,dxf) VALUE_RL(_obj->name,dxf)
#define HEADER_RL(name,dxf) \
    HEADER_9(name);\
    VALUE_RL(dwg->header_vars.name, dxf)

#define HEADER_B(name,dxf)   HEADER_RC(name,dxf)
#define HEADER_BS(name,dxf)  HEADER_RS(name,dxf)
#define HEADER_BD(name,dxf)  HEADER_RD(name,dxf)
#define HEADER_BL(name,dxf)  HEADER_RL(name, dxf)

#define VALUE_H(value,dxf) \
  {\
    Dwg_Object_Ref *ref = value;\
    if (ref && ref->obj) {VALUE_RL(ref->absolute_ref, dxf);}\
    else {VALUE_RL(0, dxf);} \
  }
#define HEADER_H(name,dxf)\
    HEADER_9(name);\
    VALUE_H(dwg->header_vars.name, dxf)

#define HANDLE_NAME(name, code, table) \
  VALUE_HANDLE_NAME(dwg->header_vars.name, dxf, table)
//TODO: convert STANDARD to Standard?
#define VALUE_HANDLE_NAME(value,dxf,table)\
  {\
    Dwg_Object_Ref *ref = value;\
    if (ref && ref->obj && ref->obj->supertype == DWG_SUPERTYPE_OBJECT) { \
      VALUE_TV(ref->obj->tio.object->tio.table->entry_name, dxf) } \
    else VALUE_TV("", dxf) \
  }
#define FIELD_HANDLE_NAME(name,dxf,table) VALUE_HANDLE_NAME(_obj->name,dxf,table)
#define HEADER_HANDLE_NAME(name,dxf,table)\
    HEADER_9(name);\
    VALUE_HANDLE_NAME(dwg->header_vars.name,dxf,table)

#define HEADER_RLL(name,dxf) \
  {\
    BITCODE_RLL s = _obj->name;\
    GROUP(9);\
    fprintf (dat->fh, "$%s%c", #name, 0);\
    GROUP(dxf);\
    fwrite(&s, sizeof(BITCODE_RLL), 1, dat->fh);\
  }
  
#define FIELD_MC(name,dxf) FIELD_RC(name,dxf)
#define FIELD_MS(name,dxf)  FIELD_RS(name,dxf)
#define FIELD_BT(name,dxf)    FIELD_BD(name, dxf);
#define FIELD_4BITS(name,dxf) FIELD_RC(name, dxf)
#define FIELD_BE(name,dxf)    FIELD_3RD(name,dxf)
#define FIELD_DD(name, _default, dxf) FIELD_RD(name,dxf)
#define FIELD_2DD(name, d1, d2, dxf) { FIELD_DD(name.x, d1, dxf); FIELD_DD(name.y, d2, dxf+10); }
#define FIELD_3DD(name, def, dxf) { \
    FIELD_DD(name.x, FIELD_VALUE(def.x), dxf); \
    FIELD_DD(name.y, FIELD_VALUE(def.y), dxf+10); \
    FIELD_DD(name.z, FIELD_VALUE(def.z), dxf+20); }
#define FIELD_2RD(name,dxf) {FIELD_RD(name.x, dxf); FIELD_RD(name.y, dxf+10);}
#define FIELD_2BD(name,dxf) FIELD_2RD(name,dxf)
#define FIELD_2BD_1(name,dxf) {FIELD_RD(name.x, dxf); FIELD_RD(name.y, dxf+1);}
#define FIELD_3RD(name,dxf) {FIELD_RD(name.x, dxf); FIELD_RD(name.y, dxf+10); FIELD_RD(name.z, dxf+20);}
#define FIELD_3BD(name,dxf) FIELD_3RD(name,dxf)
#define FIELD_3BD_1(name,dxf) {FIELD_RD(name.x,dxf); FIELD_RD(name.y,dxf+1); FIELD_RD(name.z,dxf+2);}
#define FIELD_3DPOINT(name,dxf) FIELD_3RD(name,dxf)
#define FIELD_CMC(color,dxf1,dxf2) { \
  VALUE_RS(_obj->color.index, dxf1); \
  if (dat->version >= R_2004 && dxf2 && _obj->color.rgb) { \
    VALUE_RL(_obj->color.rgb, dxf2); \
  } \
}
#define HEADER_CMC(name,dxf) \
    HEADER_9(name); VALUE_RS(dwg->header_vars.name.index, dxf)
#define HEADER_TIMEBLL(name,dxf) \
    HEADER_9(name); FIELD_TIMEBLL(name,dxf)
#define FIELD_TIMEBLL(name,dxf) VALUE_RD(_obj->name.value, dxf)

//FIELD_VECTOR_N(name, type, size):
// reads data of the type indicated by 'type' 'size' times and stores
// it all in the vector called 'name'.
#define FIELD_VECTOR_N(name, type, size, dxf)\
  if (dxf) {\
    for (vcount=0; vcount < (BITCODE_BL)size; vcount++) \
      VALUE (_obj->name[vcount], type, dxf); \
  }
#define FIELD_VECTOR_T(name, size, dxf)\
  if (dxf) {\
    PRE (R_2007) { \
      for (vcount=0; vcount < (BITCODE_BL)_obj->size; vcount++) \
        VALUE_TV (_obj->name[vcount], dxf) \
    } else { \
      for (vcount=0; vcount < (BITCODE_BL)_obj->size; vcount++) \
        VALUE_TU (_obj->name[vcount], dxf) \
    }\
  }

#define FIELD_VECTOR(name, type, size, dxf) \
  FIELD_VECTOR_N(name, type, _obj->size, dxf)

#define FIELD_2RD_VECTOR(name, size, dxf)\
  if (dxf) {\
    for (vcount=0; vcount < (BITCODE_BL)_obj->size; vcount++)\
      {\
        FIELD_2RD(name[vcount], dxf);\
      }\
    }

#define FIELD_2DD_VECTOR(name, size, dxf)\
  if (dxf) {\
    FIELD_2RD(name[0], 0);\
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

#define HANDLE_VECTOR_N(name, size, code, dxf) \
  if (dxf) {\
    for (vcount=0; vcount < (BITCODE_BL)size; vcount++)\
      {\
        FIELD_HANDLE_N(name[vcount], vcount, code, dxf);\
      }\
    }

#define HANDLE_VECTOR(name, sizefield, code, dxf) \
  HANDLE_VECTOR_N(name, FIELD_VALUE(sizefield), code, dxf)

#define FIELD_NUM_INSERTS(num_inserts, type, dxf) \
  FIELD(num_inserts, type, dxf)

#define FIELD_XDATA(name, size) \
  dxfb_write_xdata(dat, _obj->name, _obj->size)

#define _XDICOBJHANDLE(code) \
  if (dat->version >= R_13 && \
      obj->tio.object->xdicobjhandle && \
      obj->tio.object->xdicobjhandle->absolute_ref) \
  { \
    VALUE_TV("{ACAD_XDICTIONARY", 102);\
    VALUE_HANDLE(obj->tio.object->xdicobjhandle, code, 360); \
    VALUE_TV("}", 102);\
  }
#define _REACTORS(code)\
  if (dat->version >= R_13 && \
      obj->tio.object->num_reactors && \
      obj->tio.object->reactors) \
  { \
    VALUE_TV("{ACAD_REACTORS", 102) \
    for (vcount=0; vcount < obj->tio.object->num_reactors; vcount++)\
      {\
        VALUE_HANDLE(obj->tio.object->reactors[vcount], code, 330);\
      }\
    VALUE_TV("}", 102) \
  }
#define ENT_REACTORS(code)\
  if (dat->version >= R_13 && _obj->num_reactors && _obj->reactors) {\
    VALUE_TV("{ACAD_REACTORS", 102) \
    for (vcount=0; vcount < _obj->num_reactors; vcount++)\
      {\
        VALUE_HANDLE(_obj->reactors[vcount], code, 330);\
      }\
    VALUE_TV("}", 102) \
  }

#define REACTORS(code)
#define XDICOBJHANDLE(code)
#define ENT_XDICOBJHANDLE(code) \
  if (dat->version >= R_13 && \
      obj->tio.entity->xdicobjhandle && \
      obj->tio.entity->xdicobjhandle->absolute_ref) \
  { \
    VALUE_TV("{ACAD_XDICTIONARY", 102);\
    VALUE_HANDLE(obj->tio.entity->xdicobjhandle, code, 360); \
    VALUE_TV("}", 102);\
  }

#define COMMON_ENTITY_HANDLE_DATA
#define SECTION_STRING_STREAM
#define START_STRING_STREAM
#define END_STRING_STREAM
#define START_HANDLE_STREAM

#ifndef DEBUG_CLASSES
static int
dwg_dxfb_TABLECONTENT (Bit_Chain *restrict dat, const Dwg_Object *restrict obj)
{
  (void)dat; (void)obj;
  return 0;
}
#else
static int
dwg_dxfb_TABLECONTENT (Bit_Chain *restrict dat, const Dwg_Object *restrict obj);
#endif

#define DWG_ENTITY(token) \
static int \
dwg_dxfb_##token (Bit_Chain *restrict dat, const Dwg_Object *restrict obj) \
{\
  BITCODE_BL vcount, rcount1, rcount2, rcount3, rcount4; \
  Dwg_Data* dwg = obj->parent; \
  Dwg_Entity_##token *ent, *_obj;\
  Dwg_Object_Entity *_ent;\
  int error = 0; \
  if (!strcmp(#token, "GEOPOSITIONMARKER"))\
    RECORD(POSITIONMARKER)\
  else if (dat->version < R_13 && strlen(#token) == 10 && !strcmp(#token, "LWPOLYLINE")) \
    RECORD(POLYLINE)\
  else if (strlen(#token) > 11 && !memcmp(#token, "DIMENSION_", 11)) \
    RECORD(DIMENSION)\
  else if (strlen(#token) > 9 && !memcmp(#token, "POLYLINE_", 9)) \
    RECORD(POLYLINE)\
  else if (strlen(#token) > 7 && !memcmp(#token, "VERTEX_", 7)) \
    RECORD(VERTEX)\
  else if (dat->version >= R_2010 && !strcmp(#token, "TABLE")) { \
    RECORD(ACAD_TABLE);\
    return dwg_dxfb_TABLECONTENT(dat, obj); \
  } \
  else\
    RECORD(token);\
  LOG_INFO("Entity " #token ":\n")\
  _ent = obj->tio.entity;\
  _obj = ent = _ent->tio.token;\
  SINCE(R_11) { \
    uint32_t i = (uint32_t)obj->handle.value;\
    LOG_TRACE("Entity handle: %d.%d.%lX\n",\
      obj->handle.code,\
      obj->handle.size,\
      obj->handle.value) \
    GROUP(330); \
    fwrite(&i, sizeof(uint32_t), 1, dat->fh); \
  } \
  SINCE(R_13) { \
    VALUE_HANDLE_NAME (obj->parent->header_vars.BLOCK_RECORD_MSPACE, 330, BLOCK_HEADER); \
    error |= dxfb_common_entity_handle_data(dat, obj); \
  }

#define DWG_ENTITY_END return 0; }

#define DWG_OBJECT(token) \
static int \
dwg_dxfb_ ##token (Bit_Chain *restrict dat, const Dwg_Object *restrict obj) \
{ \
  BITCODE_BL vcount, rcount1, rcount2, rcount3, rcount4;\
  Bit_Chain *hdl_dat = dat;\
  Dwg_Data* dwg = obj->parent;\
  Dwg_Object_##token *_obj;\
  int error = 0; \
  LOG_INFO("Object " #token ":\n")\
  _obj = obj->tio.object->tio.token;\
  if (!dwg_obj_is_control(obj)) { \
    if (obj->fixedtype == DWG_TYPE_TABLE) \
      ; \
    else if (obj->type >= 500 && obj->dxfname) \
      VALUE_TV(obj->dxfname, 0) \
    else if (obj->type == DWG_TYPE_PLACEHOLDER) \
      RECORD(ACDBPLACEHOLDER) \
    else if (obj->type != DWG_TYPE_BLOCK_HEADER) \
      RECORD(token)                              \
    SINCE(R_13) { \
      uint32_t _i = (uint32_t)obj->handle.value; \
      int dxf = 5; \
      if (obj->type == DWG_TYPE_DIMSTYLE) dxf = 105; \
      GROUP(dxf);\
      fwrite(&_i, sizeof(uint32_t), 1, dat->fh); \
      _XDICOBJHANDLE(3); \
      _REACTORS(4); \
    } \
  }

#define DWG_OBJECT_END return 0; }

static void
dxfb_write_xdata(Bit_Chain *restrict dat, Dwg_Resbuf *restrict rbuf, BITCODE_BL size)
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
            VALUE_TV(rbuf->value.str.u.data, rbuf->type)
          } LATER_VERSIONS {
            VALUE_TU(rbuf->value.str.u.wdata, rbuf->type)
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
}

// r2000+ converts STANDARD to Standard, BYLAYER to ByLayer, BYBLOCK to ByBlock
//TODO: binary
static void
dxfb_cvt_tablerecord(Bit_Chain *restrict dat, const Dwg_Object *restrict obj,
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
            VALUE_TV ("STANDARD", dxf)
          else if (!strcmp(entry_name, "ByLayer"))
            VALUE_TV ("BYLAYER", dxf)
          else if (!strcmp(entry_name, "ByBlock"))
            VALUE_TV ("BYBLOCK", dxf)
          else if (!strcmp(entry_name, "*Active"))
            VALUE_TV ("*ACTIVE", dxf)
          else
            VALUE_TV (entry_name, dxf)
        }
      else
        { // convert some standard names
          if (dat->version >= R_2000 && !strcmp(entry_name, "STANDARD"))
            VALUE_TV ("Standard", dxf)
          else if (dat->version >= R_2000 && !strcmp(entry_name, "BYLAYER"))
            VALUE_TV ("ByLayer", dxf)
          else if (dat->version >= R_2000 && !strcmp(entry_name, "BYBLOCK"))
            VALUE_TV ("ByBlock", dxf)
          else if (dat->version >= R_2000 && !strcmp(entry_name, "*ACTIVE"))
            VALUE_TV ("*Active", dxf)
          else
            VALUE_TV (entry_name, dxf)
        }
    }
  else {
    fprintf(dat->fh, "%3i\r\n\r\n", dxf);
  }
}

// 5 written here first
#define COMMON_TABLE_CONTROL_FLAGS \
  SINCE(R_13) { \
    uint32_t _i = (uint32_t)ctrl->handle.value; \
    GROUP(5); \
    fwrite(&_i, sizeof(uint32_t), 1, dat->fh); \
  } \
  SINCE(R_14) \
    VALUE_H (_ctrl->null_handle, 330); \
  SINCE(R_2000) \
    VALUE_TV ("AcDbSymbolTable", 100)

#define COMMON_TABLE_FLAGS(owner, acdbname) \
  SINCE(R_14) \
    FIELD_HANDLE (owner, 4, 330); \
  SINCE(R_2000) { \
    VALUE_TV ("AcDbSymbolTableRecord", 100) \
    VALUE_TV ("AcDb" #acdbname "TableRecord", 100) \
  } \
  if (_obj->entry_name) dxfb_cvt_tablerecord(dat, obj, _obj->entry_name, 2); \
  FIELD_RC (flag, 70)

#define LAYER_TABLE_FLAGS(owner, acdbname) \
  SINCE(R_14) \
    FIELD_HANDLE (owner, 4, 330); \
  SINCE(R_2000) { \
    VALUE_TV ("AcDbSymbolTableRecord", 100) \
    VALUE_TV ("AcDb" #acdbname "TableRecord", 100) \
  } \
  if (_obj->entry_name) dxfb_cvt_tablerecord(dat, obj, _obj->entry_name, 2); \
  FIELD_RS (flag, 70)

#include "dwg.spec"


/* returns 0 on success
 */
static int
dwg_dxfb_variable_type(const Dwg_Data *restrict dwg, Bit_Chain *restrict dat,
                       Dwg_Object *restrict obj)
{
  int i;
  Dwg_Class *klass;
  int is_entity;

  i = obj->type - 500;
  if (i < 0 || i >= dwg->num_classes)
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
dwg_dxfb_object(Bit_Chain *restrict dat, const Dwg_Object *restrict obj)
{
  int error = 0;
  const int minimal = obj->parent->opts & 0x10;

  if (obj->supertype == DWG_SUPERTYPE_UNKNOWN)
    return 0;

  switch (obj->type)
    {
    case DWG_TYPE_TEXT:
      return dwg_dxfb_TEXT(dat, obj);
    case DWG_TYPE_ATTRIB:
      return dwg_dxfb_ATTRIB(dat, obj);
    case DWG_TYPE_ATTDEF:
      return dwg_dxfb_ATTDEF(dat, obj);
    case DWG_TYPE_BLOCK:
      return dwg_dxfb_BLOCK(dat, obj);
    case DWG_TYPE_ENDBLK:
      return dwg_dxfb_ENDBLK(dat, obj);
    case DWG_TYPE_SEQEND:
      return dwg_dxfb_SEQEND(dat, obj);
    case DWG_TYPE_INSERT:
      return dwg_dxfb_INSERT(dat, obj);
    case DWG_TYPE_MINSERT:
      return dwg_dxfb_MINSERT(dat, obj);
    case DWG_TYPE_VERTEX_2D:
      return dwg_dxfb_VERTEX_2D(dat, obj);
    case DWG_TYPE_VERTEX_3D:
      return dwg_dxfb_VERTEX_3D(dat, obj);
    case DWG_TYPE_VERTEX_MESH:
      return dwg_dxfb_VERTEX_MESH(dat, obj);
    case DWG_TYPE_VERTEX_PFACE:
      return dwg_dxfb_VERTEX_PFACE(dat, obj);
    case DWG_TYPE_VERTEX_PFACE_FACE:
      return dwg_dxfb_VERTEX_PFACE_FACE(dat, obj);
    case DWG_TYPE_POLYLINE_2D:
      return dwg_dxfb_POLYLINE_2D(dat, obj);
    case DWG_TYPE_POLYLINE_3D:
      return dwg_dxfb_POLYLINE_3D(dat, obj);
    case DWG_TYPE_ARC:
      return dwg_dxfb_ARC(dat, obj);
    case DWG_TYPE_CIRCLE:
      return dwg_dxfb_CIRCLE(dat, obj);
    case DWG_TYPE_LINE:
      return dwg_dxfb_LINE(dat, obj);
    case DWG_TYPE_DIMENSION_ORDINATE:
      return dwg_dxfb_DIMENSION_ORDINATE(dat, obj);
    case DWG_TYPE_DIMENSION_LINEAR:
      return dwg_dxfb_DIMENSION_LINEAR(dat, obj);
    case DWG_TYPE_DIMENSION_ALIGNED:
      return dwg_dxfb_DIMENSION_ALIGNED(dat, obj);
    case DWG_TYPE_DIMENSION_ANG3PT:
      return dwg_dxfb_DIMENSION_ANG3PT(dat, obj);
    case DWG_TYPE_DIMENSION_ANG2LN:
      return dwg_dxfb_DIMENSION_ANG2LN(dat, obj);
    case DWG_TYPE_DIMENSION_RADIUS:
      return dwg_dxfb_DIMENSION_RADIUS(dat, obj);
    case DWG_TYPE_DIMENSION_DIAMETER:
      return dwg_dxfb_DIMENSION_DIAMETER(dat, obj);
    case DWG_TYPE_POINT:
      return dwg_dxfb_POINT(dat, obj);
    case DWG_TYPE__3DFACE:
      return dwg_dxfb__3DFACE(dat, obj);
    case DWG_TYPE_POLYLINE_PFACE:
      return dwg_dxfb_POLYLINE_PFACE(dat, obj);
    case DWG_TYPE_POLYLINE_MESH:
      return dwg_dxfb_POLYLINE_MESH(dat, obj);
    case DWG_TYPE_SOLID:
      return dwg_dxfb_SOLID(dat, obj);
    case DWG_TYPE_TRACE:
      return dwg_dxfb_TRACE(dat, obj);
    case DWG_TYPE_SHAPE:
      return dwg_dxfb_SHAPE(dat, obj);
    case DWG_TYPE_VIEWPORT:
      return minimal ? 0 : dwg_dxfb_VIEWPORT(dat, obj);
    case DWG_TYPE_ELLIPSE:
      return dwg_dxfb_ELLIPSE(dat, obj);
    case DWG_TYPE_SPLINE:
      return dwg_dxfb_SPLINE(dat, obj);
    case DWG_TYPE_REGION:
      return dwg_dxfb_REGION(dat, obj);
    case DWG_TYPE__3DSOLID:
      return dwg_dxfb__3DSOLID(dat, obj);
    case DWG_TYPE_BODY:
      return dwg_dxfb_BODY(dat, obj);
    case DWG_TYPE_RAY:
      return dwg_dxfb_RAY(dat, obj);
    case DWG_TYPE_XLINE:
      return dwg_dxfb_XLINE(dat, obj);
    case DWG_TYPE_DICTIONARY:
      return minimal ? 0 : dwg_dxfb_DICTIONARY(dat, obj);
    case DWG_TYPE_MTEXT:
      return dwg_dxfb_MTEXT(dat, obj);
    case DWG_TYPE_LEADER:
      return dwg_dxfb_LEADER(dat, obj);
    case DWG_TYPE_TOLERANCE:
      return dwg_dxfb_TOLERANCE(dat, obj);
    case DWG_TYPE_MLINE:
#ifdef DEBUG_CLASSES
      // TODO: looks good, but acad import crashes
      return dwg_dxfb_MLINE(dat, obj);
#else
      LOG_WARN("Unhandled Entity MLINE in out_dxfb %u/%lX", obj->index, obj->handle.value)
      if (0) dwg_dxfb_MLINE(dat, obj);
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
      return dwg_dxfb_GROUP(dat, obj);
    case DWG_TYPE_MLINESTYLE:
      return minimal ? 0 : dwg_dxfb_MLINESTYLE(dat, obj);
    case DWG_TYPE_OLE2FRAME:
      return minimal ? 0 : dwg_dxfb_OLE2FRAME(dat, obj);
    case DWG_TYPE_DUMMY:
      return 0;
    case DWG_TYPE_LONG_TRANSACTION:
      return minimal ? 0 : dwg_dxfb_LONG_TRANSACTION(dat, obj);
    case DWG_TYPE_LWPOLYLINE:
      return dwg_dxfb_LWPOLYLINE(dat, obj);
    case DWG_TYPE_HATCH:
      return dwg_dxfb_HATCH(dat, obj);
    case DWG_TYPE_XRECORD:
      return minimal ? 0 : dwg_dxfb_XRECORD(dat, obj);
    case DWG_TYPE_PLACEHOLDER:
      return minimal ? 0 : dwg_dxfb_PLACEHOLDER(dat, obj);
    case DWG_TYPE_PROXY_ENTITY:
      //TODO dwg_dxfb_PROXY_ENTITY(dat, obj);
      return DWG_ERR_UNHANDLEDCLASS;
    case DWG_TYPE_OLEFRAME:
      return minimal ? 0 : dwg_dxfb_OLEFRAME(dat, obj);
    case DWG_TYPE_VBA_PROJECT:
      if (!minimal) {
        LOG_ERROR("Unhandled Object VBA_PROJECT");
        //dwg_dxfb_VBA_PROJECT(dat, obj);
        return DWG_ERR_UNHANDLEDCLASS;
      }
      return 0;
    case DWG_TYPE_LAYOUT:
      return minimal ? 0 : dwg_dxfb_LAYOUT(dat, obj);
    default:
      if (obj->type == obj->parent->layout_number)
        {
          return minimal ? 0 : dwg_dxfb_LAYOUT(dat, obj);
        }
      /* > 500 */
      else if (DWG_ERR_UNHANDLEDCLASS &
               (error = dwg_dxfb_variable_type(obj->parent, dat, (Dwg_Object*)obj)))
        {
          Dwg_Data *dwg = obj->parent;
          int is_entity;
          int i = obj->type - 500;
          Dwg_Class *klass = NULL;

          if (i >= 0 && i < (int)dwg->num_classes)
            klass = &dwg->dwg_class[i];
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
dxfb_common_entity_handle_data(Bit_Chain *restrict dat, const Dwg_Object *restrict obj)
{
  Dwg_Object_Entity *ent;
  //Dwg_Data *dwg = obj->parent;
  Dwg_Object_Entity *_obj;
  BITCODE_BL vcount = 0;
  int error = 0;
  ent = obj->tio.entity;
  _obj = ent;

  #include "common_entity_handle_data.spec"

  return error;
}

// see https://www.autodesk.com/techpubs/autocad/acad2000/dxf/header_section_group_codes_dxf_02.htm
static int
dxfb_header_write(Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
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

static int
dxfb_classes_write (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
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
dxfb_tables_write (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  int error = 0;
  unsigned int i;

  SECTION(TABLES);
  {
    Dwg_Object_VPORT_CONTROL *_ctrl = &dwg->vport_control;
    Dwg_Object *ctrl = &dwg->object[_ctrl->objid];
    TABLE(VPORT);
    // add handle 5 here at first
    COMMON_TABLE_CONTROL_FLAGS;
    error |= dwg_dxfb_VPORT_CONTROL(dat, ctrl);
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
          error |= dwg_dxfb_VPORT(dat, obj);
        }
      }
    ENDTAB();
  }
  {
    Dwg_Object_LTYPE_CONTROL *_ctrl = &dwg->ltype_control;
    Dwg_Object *ctrl = &dwg->object[_ctrl->objid];
    Dwg_Object *obj;

    TABLE(LTYPE);
    COMMON_TABLE_CONTROL_FLAGS;
    error |= dwg_dxfb_LTYPE_CONTROL(dat, ctrl);
    // first the 2 builtin ltypes: ByBlock, ByLayer
    if ((obj  = dwg_ref_object(dwg, dwg->header_vars.LTYPE_BYBLOCK))) {
      dwg_dxfb_LTYPE(dat, obj);
    }
    if ((obj  = dwg_ref_object(dwg, dwg->header_vars.LTYPE_BYLAYER))) {
      error |= dwg_dxfb_LTYPE(dat, obj);
    }
    // here LTYPE_CONTINUOUS is already included
    for (i=0; i<dwg->ltype_control.num_entries; i++)
      {
        obj = dwg_ref_object(dwg, _ctrl->linetypes[i]);
        if (obj && obj->type == DWG_TYPE_LTYPE) {
          error |= dwg_dxfb_LTYPE(dat, obj);
        }
      }
    ENDTAB();
  }
  {
    Dwg_Object_LAYER_CONTROL *_ctrl = &dwg->layer_control;
    Dwg_Object *ctrl = &dwg->object[_ctrl->objid];
    TABLE(LAYER);
    COMMON_TABLE_CONTROL_FLAGS;
    error |= dwg_dxfb_LAYER_CONTROL(dat, ctrl);
    for (i=0; i<dwg->layer_control.num_entries; i++)
      {
        Dwg_Object *obj = dwg_ref_object(dwg, _ctrl->layers[i]);
        if (obj && obj->type == DWG_TYPE_LAYER) {
          error |= dwg_dxfb_LAYER(dat, obj);
        }
      }
    ENDTAB();
  }
  {
    Dwg_Object_STYLE_CONTROL *_ctrl = &dwg->style_control;
    Dwg_Object *ctrl = &dwg->object[_ctrl->objid];
    TABLE(STYLE);
    COMMON_TABLE_CONTROL_FLAGS;
    error |= dwg_dxfb_STYLE_CONTROL(dat, ctrl);
    for (i=0; i<dwg->style_control.num_entries; i++)
      {
        Dwg_Object *obj = dwg_ref_object(dwg, _ctrl->styles[i]);
        if (obj && obj->type == DWG_TYPE_STYLE) {
          error |= dwg_dxfb_STYLE(dat, obj);
        }
      }
    ENDTAB();
  }
  {
    Dwg_Object_VIEW_CONTROL *_ctrl = &dwg->view_control;
    Dwg_Object *ctrl = &dwg->object[_ctrl->objid];
    TABLE(VIEW);
    COMMON_TABLE_CONTROL_FLAGS;
    error |= dwg_dxfb_VIEW_CONTROL(dat, ctrl);
    for (i=0; i<dwg->view_control.num_entries; i++)
      {
        Dwg_Object *obj = dwg_ref_object(dwg, _ctrl->views[i]);
        //FIXME implement the other two
        if (obj && obj->type == DWG_TYPE_VIEW)
          error |= dwg_dxfb_VIEW(dat, obj);
        /*
        else if (obj && obj->fixedtype == DWG_TYPE_SECTIONVIEWSTYLE)
          error |= dwg_dxfb_SECTIONVIEWSTYLE(dat, obj);
        if (obj && obj->fixedtype == DWG_TYPE_DETAILVIEWSTYLE) {
          error |= dwg_dxfb_DETAILVIEWSTYLE(dat, obj);
        */
      }
    ENDTAB();
  }
  {
    Dwg_Object_UCS_CONTROL *_ctrl = &dwg->ucs_control;
    Dwg_Object *ctrl = &dwg->object[_ctrl->objid];
    TABLE(UCS);
    COMMON_TABLE_CONTROL_FLAGS;
    error |= dwg_dxfb_UCS_CONTROL(dat, ctrl);
    for (i=0; i<dwg->ucs_control.num_entries; i++)
      {
        Dwg_Object *obj = dwg_ref_object(dwg, _ctrl->ucs[i]);
        if (obj && obj->type == DWG_TYPE_UCS) {
          error |= dwg_dxfb_UCS(dat, obj);
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
    error |= dwg_dxfb_APPID_CONTROL(dat, ctrl);
    for (i=0; i<dwg->appid_control.num_entries; i++)
      {
        Dwg_Object *obj = dwg_ref_object(dwg, _ctrl->apps[i]);
        if (obj && obj->type == DWG_TYPE_APPID) {
          error |= dwg_dxfb_APPID(dat, obj);
        }
      }
    ENDTAB();
  }
  {
    Dwg_Object_DIMSTYLE_CONTROL *_ctrl = &dwg->dimstyle_control;
    Dwg_Object *ctrl = &dwg->object[_ctrl->objid];
    TABLE(DIMSTYLE);
    COMMON_TABLE_CONTROL_FLAGS;
    dwg_dxfb_DIMSTYLE_CONTROL(dat, ctrl);
    //ignoring morehandles
    for (i=0; i<dwg->dimstyle_control.num_entries; i++)
      {
        Dwg_Object *obj = dwg_ref_object(dwg, _ctrl->dimstyles[i]);
        if (obj && obj->type == DWG_TYPE_DIMSTYLE) {
          error |= dwg_dxfb_DIMSTYLE(dat, obj);
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
      error |= dwg_dxfb_VPORT_ENTITY_CONTROL(dat, ctrl);
      for (i=0; i<dwg->vport_entity_control.num_entries; i++)
        {
          Dwg_Object *obj = dwg_ref_object(dwg, _ctrl->vport_entity_headers[i]);
          if (obj && obj->type == DWG_TYPE_VPORT_ENTITY_HEADER) {
            error |= dwg_dxfb_VPORT_ENTITY_HEADER(dat, obj);
          }
        }
      // avoid unused warnings
      dwg_dxfb_PROXY_ENTITY(dat, &dwg->object[0]);
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
    error |= dwg_dxfb_BLOCK_CONTROL(dat, ctrl);
    if (obj && obj->type == DWG_TYPE_BLOCK_HEADER) {
      mspace = obj;
      RECORD(BLOCK_RECORD);
      error |= dwg_dxfb_BLOCK_HEADER(dat, obj);
    }
    if (_ctrl->paper_space) {
      obj = dwg_ref_object(dwg, _ctrl->paper_space);
      if (obj && obj->type == DWG_TYPE_BLOCK_HEADER) {
        pspace = obj;
        RECORD(BLOCK_RECORD);
        error |= dwg_dxfb_BLOCK_HEADER(dat, obj);
      }
    }
    for (i=0; i<dwg->block_control.num_entries; i++)
      {
        Dwg_Object *_o = dwg_ref_object(dwg, dwg->block_control.block_headers[i]);
        if (_o && _o->type == DWG_TYPE_BLOCK_HEADER &&
            _o != mspace && _o != pspace)
          {
            RECORD(BLOCK_RECORD);
            error |= dwg_dxfb_BLOCK_HEADER(dat, _o);
          }
      }
    ENDTAB();
  }

  ENDSEC();
  return 0;
}

static int
dxfb_blocks_write (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  int error = 0;
  Dwg_Object_BLOCK_CONTROL *_ctrl = &dwg->block_control;
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
      error |= dwg_dxfb_object(dat, obj);
      obj = get_next_owned_block(hdr, obj);
      if (obj && obj->type == DWG_TYPE_ENDBLK)
        {
          error |= dwg_dxfb_ENDBLK(dat, obj);
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
          error |= dwg_dxfb_object(dat, obj);
          obj = get_next_owned_block(hdr, obj);
          if (obj && obj->type == DWG_TYPE_ENDBLK)
            {
              error |= dwg_dxfb_ENDBLK(dat, obj);
              obj = NULL;
            }
        }
    }
  ENDSEC();
  return error;
}

static int
dxfb_entities_write (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  int error = 0;
  BITCODE_BL i;

  SECTION(ENTITIES);
  for (i=0; i<dwg->num_objects; i++)
    {
      if (dwg->object[i].supertype == DWG_SUPERTYPE_ENTITY &&
          dwg->object[i].type != DWG_TYPE_BLOCK &&
          dwg->object[i].type != DWG_TYPE_ENDBLK)
        error |= dwg_dxfb_object(dat, &dwg->object[i]);
    }
  ENDSEC();
  return error;
}

static int
dxfb_objects_write (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  int error = 0;
  BITCODE_BL i;

  SECTION(OBJECTS);
  for (i=0; i<dwg->num_objects; i++)
    {
      if (dwg->object[i].supertype == DWG_SUPERTYPE_OBJECT)
        error |= dwg_dxfb_object(dat, &dwg->object[i]);
    }
  ENDSEC();
  return error;
}

static int
dxfb_preview_write (Bit_Chain *dat, Dwg_Data * dwg)
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
dwg_write_dxfb(Bit_Chain *dat, Dwg_Data * dwg)
{
  const int minimal = dwg->opts & 0x10;

  if (dat->from_version == R_INVALID)
    dat->from_version = dat->version;
  fprintf(dat->fh, "AutoCAD Binary DXF\r\n%c%c", 0x1a, 0);
  //VALUE_TV(PACKAGE_STRING, 999);

  // a minimal header requires only $ACADVER, $HANDSEED, and then ENTITIES
  // see https://pythonhosted.org/ezdxf/dxfinternals/filestructure.html
  dxfb_header_write (dat, dwg);

  if (!minimal) {
    // if downgraded from r2000 to r14, but we still have classes, keep the classes
    if ((dat->from_version >= R_2000 && dwg->num_classes) ||
        dat->version >= R_2000) {
      if (dxfb_classes_write (dat, dwg) >= DWG_ERR_CRITICAL)
        goto fail;
    }

    if (dxfb_tables_write (dat, dwg) >= DWG_ERR_CRITICAL)
      goto fail;

    if (dxfb_blocks_write (dat, dwg) >= DWG_ERR_CRITICAL)
      goto fail;
  }

  if (dxfb_entities_write (dat, dwg) >= DWG_ERR_CRITICAL)
    goto fail;

  if (!minimal) {
    SINCE(R_13) {
      if (dxfb_objects_write (dat, dwg) >= DWG_ERR_CRITICAL)
        goto fail;
    }
    SINCE(R_2000) {
      if (dxfb_preview_write (dat, dwg) >= DWG_ERR_CRITICAL)
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
