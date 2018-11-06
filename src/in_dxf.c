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
 * in_dxf.c: read ascii DXF
 * written by Reini Urban
 */
/* TODO:
 * read whole spec and add fieldnames/group to some hash/array to search in.
 *   i.e. HEADER [ {HANDSEED, RS, 5}, ... ]
 * while looping over the DXF:
 *   read next object: border groups 0, 2
 *   map object to our struct type
 *     i.e. 0 SECTION 2 HEADER -> dwg->header_vars,
 *          0 TABLE 2 APPID => APPID_CONTROL, nth 0 APPID => nth APPID.
 *   find and fill fieldname/group in our struct. skip unknowns and missing names
 *
 * Alternate idea: already when reading the spec, mem-search the dxf (dat in memory)
 * to the known border for the field within the current object range,
 * border 0/2 => start - end. less memory, whole spec not needed in memory.
 * Problems: we might miss some DXF groups, as we random search within it,
 * and don't keep track what we used, and what not.
 *
 * But first check the EOL: CRLF or just LF. This speeds up the search.
 */

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <ctype.h>

#include "common.h"
#include "bits.h"
#include "dwg.h"
#include "in_dxf.h"
#include "out_dxf.h"
#include "decode.h"
#include "encode.h"

static unsigned int loglevel;
#define DWG_LOGLEVEL loglevel
#include "logging.h"

/* the current version per spec block */
static unsigned int cur_ver = 0;
static char buf[4096];
static long start, end; //stream offsets

typedef struct _dxf_pair {
  short code;
  enum RES_BUF_VALUE_TYPE type;
  union {
    int i;
    char *s;
    long l;
    double d;
  } value;
} Dxf_Pair;

static long num_dxf_objs;  // how many elements are added
static long size_dxf_objs; // how many elements are allocated
static Dxf_Objs* dxf_objs;

static inline void dxf_skip_ws(Bit_Chain *dat)
{
  for (; !dat->chain[dat->byte] || isspace(dat->chain[dat->byte]); dat->byte++) ;
}

static int dxf_read_code(Bit_Chain *dat)
{
  char *endptr;
  long num = strtol((char*)&dat->chain[dat->byte], &endptr, 10);
  dat->byte += (unsigned char *)endptr - &dat->chain[dat->byte];
  dxf_skip_ws(dat);
  if (num > INT_MAX)
    LOG_ERROR("%s: int overflow %ld (at %lu)", __FUNCTION__, num, dat->byte)
  return (int)num;
}

static int dxf_read_group(Bit_Chain *dat, int dxf)
{
  char *endptr;
  long num = strtol((char*)&dat->chain[dat->byte], &endptr, 10);
  if ((int)num == dxf) {
    LOG_HANDLE("group %d\n", dxf);
    dat->byte += (unsigned char *)endptr - &dat->chain[dat->byte];
    dxf_skip_ws(dat);
    return 1;
  }
  return 0;
}

static void dxf_read_string(Bit_Chain *dat, char **string)
{
  int i;
  dxf_skip_ws(dat);
  for (i = 0; !isspace(dat->chain[dat->byte]); dat->byte++)
    {
      buf[i++] = dat->chain[dat->byte];
    }
  buf[i] = '\0';
  //int i = sscanf(&dat->chain[dat->byte], "%s", buf);
  if (i) {
    dxf_skip_ws(dat);
    if (!string)
      return; // ignore
    if (!*string)
      *string = malloc(strlen(buf)+1);
    strcpy(*string, buf);
  }
}

#define STRADD(field, string) \
  field = malloc(strlen(string)+1); strcpy(field, string)

static void dxf_free_pair(Dxf_Pair* pair)
{
  if (pair->type == VT_STRING ||
      pair->type == VT_BINARY)
    {
      free(pair->value.s);
    }
  free(pair);
  pair = NULL;
}

static Dxf_Pair* dxf_read_pair(Bit_Chain *dat)
{
  Dxf_Pair *pair = calloc(1, sizeof(Dxf_Pair));
  pair->code = (short)dxf_read_code(dat);
  pair->type = get_base_value_type(pair->code);
  switch (pair->type)
    {
    case VT_STRING:
      dxf_read_string(dat, &pair->value.s);
      LOG_TRACE("dxf{%d, %s}\n", (int)pair->code, pair->value.s);
      SINCE(R_2007) {
        BITCODE_TU wstr = bit_utf8_to_TU(pair->value.s);
        free(pair->value.s);
        pair->value.s = (char*)wstr;
      }
      break;
    case VT_BOOL:
    case VT_INT8:
    case VT_INT16:
    case VT_INT32:
      pair->value.i = dxf_read_code(dat);
      LOG_TRACE("dxf{%d, %d}\n", (int)pair->code, pair->value.i);
      break;
    case VT_REAL:
    case VT_POINT3D:
      dxf_skip_ws(dat);
      sscanf((char*)&dat->chain[dat->byte], "%lf", &pair->value.d);
      LOG_TRACE("dxf{%d, %f}\n", pair->code, pair->value.d);
      break;
    case VT_BINARY:
      //read into buf only?
      dxf_read_string(dat, &pair->value.s);
      //TODO convert %02X to string
      LOG_TRACE("dxf{%d, %s}\n", (int)pair->code, pair->value.s);
      break;
    case VT_HANDLE:
    case VT_OBJECTID:
      dxf_read_string(dat, NULL);
      sscanf(buf, "%X", &pair->value.i);
      LOG_TRACE("dxf{%d,%X}\n", (int)pair->code, pair->value.i);
      break;
    case VT_INVALID:
    default:
      LOG_ERROR("Invalid DXF group code: %d", pair->code);
      return NULL;
    }
  return pair;
}

#define DXF_CHECK_EOF           \
  if (dat->byte >= dat->size || \
      (pair->code == 0 && !strcmp(pair->value.s, "EOF"))) \
    return 1

static int dxf_skip_comment(Bit_Chain *dat, Dxf_Pair *pair)
{
  while (pair->code == 999)
    {
      dxf_free_pair(pair);
      pair = dxf_read_pair(dat);
      DXF_CHECK_EOF;
    }
  return 0;
}

static int dxf_check_code(Bit_Chain *dat, Dxf_Pair *pair, int code)
{
  if (pair->code == code)
    {
      dxf_skip_comment(dat, pair);
      DXF_CHECK_EOF;
      dxf_free_pair(pair);
      return 1;
    }
  return 0;
}

/*--------------------------------------------------------------------------------
 * MACROS
 */

#define ACTION indxf
#define IS_ENCODER
#define IS_DXF

//for sscanf with BD we need to use %lf not %g
#undef FORMAT_BD
#define FORMAT_BD "%lf"

#define FIELD(name,type,dxf) dxf_add_field(obj, #name, #type, dxf)
#define FIELD_CAST(name,type,cast,dxf) FIELD(name,cast,dxf)
#define FIELD_TRACE(name,type)
#define VALUE_TV(value, dxf)  dxf_read_string(dat, (char**)&value)
#define SUBCLASS(text) VALUE_TV(buf, 100);

#define VALUE_TU(value,dxf) \
  { BITCODE_TU wstr; \
    VALUE_TV(buf, dxf); \
    wstr = bit_utf8_to_TU(buf); \
    /* TODO get TU length, copy to &value */ \
  }

#define FIELD_VALUE(name) _obj->name
#define ANYCODE -1
#define VALUE_HANDLE(hdlptr, handle_code, dxf) \
  if (dxf && hdlptr) { \
    if (GROUP(dxf)) { \
      int i = sscanf((char*)&dat->chain[dat->byte], "%lX", \
                     &hdlptr->absolute_ref); \
      dat->byte += i; \
    } \
  }
#define FIELD_HANDLE(name, handle_code, dxf) VALUE_HANDLE(_obj->name, handle_code, dxf)
#define HEADER_9(name) \
    GROUP(9)
#define VALUE_H(value,dxf) \
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
    char *headername; \
    if (GROUP(9)) { \
      dxf_read_string(dat, &headername); \
      LOG_TRACE("9 %s:\n", #name); \
      VALUE (value, type, dxf); \
    } \
    else { \
      FIELD(name,type,dxf); \
    } \
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
#define HEADER_TIMEBLL(name, dxf) \
  HEADER_9(name);\
  FIELD_TIMEBLL(name, dxf)

#define SECTION(section) RECORD(SECTION); PAIR(2, section)
#define ENDSEC()       RECORD(ENDSEC)
#define TABLE(table)   RECORD(TABLE); PAIR(2, table)
#define ENDTAB()       RECORD(ENDTAB)
#define PAIR(n, record) \
  { GROUP(n); \
    dxf_read_string(dat, NULL); }
#define RECORD(record) PAIR(0, record)
#define GROUP(dxf) dxf_read_group(dat, dxf)

#define VALUE(value, type, dxf) {}

#define HEADER_HANDLE_NAME(name, dxf, section) \
  HEADER_9(name);\
  {\
    Dwg_Object_Ref *ref = dwg->header_vars.name;\
    pair = dxf_read_pair(dat); \
    DXF_CHECK_EOF; \
    if (ref && ref->obj && pair->type == VT_HANDLE) { \
      /* TODO: set the table handle */ \
      ;/*ref->obj->handle.absolute_ref = pair->value.i; */ \
      /*ref->obj->tio.object->tio.section->entry_name = strdup(pair->value.s);*/ \
    } \
  }
//FIXME
#define HANDLE_NAME(id, dxf) \
  { \
    Dwg_Object_Ref *ref = id;\
    char *tmp; \
    Dwg_Object *o = ref ? ref->obj : NULL;\
    VALUE_TV(tmp, dxf); \
}

#define FIELD_DATAHANDLE(name, code, dxf) FIELD_HANDLE(name, code, dxf)
#define FIELD_HANDLE_N(name, vcount, handle_code, dxf) FIELD_HANDLE(name, handle_code, dxf)

#define HEADER_RC(name,dxf)  HEADER_9(name); FIELD(name, RC, dxf)
#define HEADER_RS(name,dxf)  HEADER_9(name); FIELD(name, RS, dxf)
#define HEADER_RL(name,dxf)  HEADER_9(name); FIELD(name, RL, dxf)
#define HEADER_RD(name,dxf)  HEADER_9(name); FIELD(name, RD, dxf)
#define HEADER_RLL(name,dxf) HEADER_9(name); FIELD(name, RLL, dxf)
#define HEADER_TV(name,dxf)  HEADER_9(name); VALUE_TV(_obj->name,dxf)
#define HEADER_TU(name,dxf)  HEADER_9(name); VALUE_TU(_obj->name,dxf)
#define HEADER_T(name,dxf)   HEADER_9(name); VALUE_T(_obj->name, dxf)
#define HEADER_B(name,dxf)   HEADER_9(name); FIELD(name, B, dxf)
#define HEADER_BS(name,dxf)  HEADER_9(name); FIELD(name, BS, dxf)
#define HEADER_BL(name,dxf)  HEADER_9(name); FIELD(name, BL, dxf)
#define HEADER_BD(name,dxf)  HEADER_9(name); FIELD(name, BD, dxf)

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
  if (_obj->name != NULL && dxf != 0) { GROUP(dxf); VALUE_TV(_obj->name, dxf); }
#define FIELD_TU(name,dxf) \
  if (_obj->name != NULL && dxf != 0) { GROUP(dxf); VALUE_TU((BITCODE_TU)_obj->name, dxf); }
#define FIELD_T(name,dxf) \
  { if (dat->version >= R_2007) { FIELD_TU(name, dxf); } \
    else                        { FIELD_TV(name, dxf); } }
#define VALUE_T(value,dxf) \
  { if (dat->version >= R_2007) { VALUE_TU(value, dxf); } \
    else                        { VALUE_TV(value, dxf); } }
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
#define FIELD_CMC(color,dxf1,dxf2) \
  VALUE_RS(_obj->color.index, dxf1)
// TODO: rgb
#define FIELD_TIMEBLL(name,dxf) \
  GROUP(dxf);\
  sscanf((char*)&dat->chain[dat->byte], "%u.%u", \
        &_obj->name.days, &_obj->name.ms)
#define HEADER_CMC(name,dxf) \
    HEADER_9(name);\
    VALUE_RS(dwg->header_vars.name.index, dxf)

#define POINT_3D(name, var, c1, c2, c3)\
  {\
    pair = dxf_read_pair(dat); \
    DXF_CHECK_EOF; \
    if (pair && pair->code == c1) { \
      dwg->var.x = pair->value.d; \
      dxf_free_pair(pair); \
      pair = dxf_read_pair(dat); \
      if (pair && pair->code == c2) \
        dwg->var.y = pair->value.d; \
      dxf_free_pair(pair); \
      pair = dxf_read_pair(dat); \
      if (pair && pair->code == c3) \
        dwg->var.z = pair->value.d; \
      dxf_free_pair(pair); \
    } \
  }
#define POINT_2D(name, var, c1, c2) \
  {\
    pair = dxf_read_pair(dat); \
    DXF_CHECK_EOF; \
    if (pair && pair->code == c1) { \
      dwg->var.x = pair->value.d; \
      dxf_free_pair(pair); \
      pair = dxf_read_pair(dat); \
      if (pair && pair->code == c2) \
        dwg->var.y = pair->value.d; \
      dxf_free_pair(pair); \
    } \
  }

//FIELD_VECTOR_N(name, type, size):
// reads data of the type indicated by 'type' 'size' times and stores
// it all in the vector called 'name'.
#define FIELD_VECTOR_N(name, type, size, dxf)\
  if (dxf)\
    {\
      for (vcount=0; vcount < (BITCODE_BL)size; vcount++)\
        {\
          sscanf((char*)&dat->chain[dat->byte], #name ": " FORMAT_##type ",\n", \
                 &_obj->name[vcount]);\
        }\
    }

#define FIELD_VECTOR_T(name, size, dxf)\
  if (dxf) {\
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
  if (dxf) {\
    for (vcount=0; vcount < (BITCODE_BL)_obj->size; vcount++)    \
      {\
        FIELD_2RD(name[vcount], dxf);\
      }\
  }

#define FIELD_2DD_VECTOR(name, size, dxf)\
  FIELD_2RD(name[0], dxf);\
  for (vcount = 1; vcount < (BITCODE_BL)_obj->size; vcount++)\
    {\
      FIELD_2DD(name[vcount], FIELD_VALUE(name[vcount - 1].x), FIELD_VALUE(name[vcount - 1].y), dxf);\
    }\

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

#define FIELD_XDATA(name, size)

#define REACTORS(code)\
  pair = dxf_read_pair(dat); \
  if (dxf_check_code(dat, pair, 102)) { /* {ACAD_REACTORS */ \
    dxf_free_pair(pair); vcount = 0; \
    while (dxf_check_code(dat, pair, 330)) { \
      vcount++; obj->tio.object->num_reactors++; \
      VALUE_HANDLE(obj->tio.object->reactors[vcount], code, 330); \
    } \
    dxf_check_code(dat, pair, 102); \
  }

#define ENT_REACTORS(code)\
  pair = dxf_read_code(dat); \
  if (dxf_check_code(dat, pair, 102)) { /* {ACAD_REACTORS */ \
    dxf_free_pair(pair); vcount = 0; \
    while (dxf_check_code(dat, pair, 330)) { \
      vcount++; _obj->num_reactors++; \
      VALUE_HANDLE(obj->tio.entity->reactors[vcount], code, 330); \
    } \
    dxf_check_code(dat, pair, 102); \
  }

#define XDICOBJHANDLE(code)
#define ENT_XDICOBJHANDLE(code)

#define COMMON_ENTITY_HANDLE_DATA \
  SINCE(R_13) { \
    error |= dxf_common_entity_handle_data(dat, obj); \
  }
#define SECTION_STRING_STREAM
#define START_STRING_STREAM
#define END_STRING_STREAM
#define START_HANDLE_STREAM

#define DWG_ENTITY(token) \
static int \
dwg_indxf_##token (Bit_Chain *restrict dat, Dwg_Object *restrict obj) \
{\
  BITCODE_BL vcount, rcount1, rcount2, rcount3, rcount4; \
  Dwg_Entity_##token *ent, *_obj;\
  Bit_Chain *hdl_dat = dat;\
  Dwg_Data* dwg = obj->parent;\
  Dwg_Object_Entity *_ent;\
  Dxf_Pair *pair; \
  int error = 0;\
  LOG_INFO("Entity " #token ":\n")\
  _ent = obj->tio.entity;\
  _obj = ent = _ent->tio.token;\
  obj->fixedtype = DWG_TYPE_##token;\
  LOG_TRACE("Entity handle: %d.%d.%lX\n",\
    obj->handle.code,\
    obj->handle.size,\
    obj->handle.value)

#define DWG_ENTITY_END return error; }

#define DWG_OBJECT(token) \
static int \
dwg_indxf_ ##token (Bit_Chain *restrict dat, Dwg_Object *restrict obj) \
{ \
  BITCODE_BL vcount, rcount1, rcount2, rcount3, rcount4;\
  Bit_Chain *hdl_dat = dat;\
  Dwg_Data* dwg = obj->parent;\
  Dwg_Object_##token *_obj;\
  Dxf_Pair *pair; \
  int error = 0; \
  obj->fixedtype = DWG_TYPE_##token;\
  LOG_INFO("Object " #token ":\n")\
  _obj = obj->tio.object->tio.token;\
  LOG_TRACE("Object handle: %d.%d.%lX\n",\
    obj->handle.code,\
    obj->handle.size,\
    obj->handle.value)

#define DWG_OBJECT_END return error; }

//TODO: we have only one obj per DXF context/section. simplify
void dxf_add_field(Dwg_Object *restrict obj, const char *restrict name,
                   const char *restrict type, int dxf)
{
  int i;
  Dxf_Objs *found = NULL;
  Dxf_Field *field;

  //array of [obj -> [fields], ...]
  if (num_dxf_objs >= size_dxf_objs)
    {
      size_dxf_objs += 1000;
      dxf_objs = realloc(dxf_objs, size_dxf_objs*sizeof(Dxf_Objs));
      memset(&dxf_objs[num_dxf_objs], 0, 1000*sizeof(Dxf_Objs));
    }
  //search obj ptr in array
  for (i=0; i<num_dxf_objs; i++)
    {
      if (dxf_objs[i].obj == obj)
        {
          found = &dxf_objs[i];
          break;
        }
    }
  if (!found) // new object (first field)
    {
      found = &dxf_objs[num_dxf_objs];
      found->obj = obj;
      found->num_fields = 1;
      found->size_fields = 16;
      found->fields = (Dxf_Field*)calloc(16, sizeof(Dxf_Field));
      num_dxf_objs++;
    }
  /*if (!found->fields)
    {
      found->num_fields = 1;
      found->size_fields = 16;
      found->fields = calloc(found->size_fields, sizeof(Dxf_Field));
    }
    else */
  if (found->num_fields > found->size_fields)
    {
      found->size_fields += 16;
      found->fields = realloc(found->fields, found->size_fields*sizeof(Dxf_Field));
      memset(&found->fields[found->num_fields], 0, 16*sizeof(Dxf_Field));
    }

  // fill the new field
  field = &found->fields[found->num_fields];
  field->name = malloc(strlen(name)+1);
  strcpy(field->name, (char*)name);
  field->type = malloc(strlen(type)+1);
  strcpy(field->type, (char*)type);
  field->dxf = dxf;
  found->num_fields++;
}

//TODO: we have only one obj per DXF context/section. simplify
Dxf_Field* dxf_search_field(Dwg_Object *restrict obj, const char *restrict name,
                            const char *restrict type, int dxf)
{
  int i;
  Dxf_Objs *found = NULL;

  // first search obj ptr in array
  for (i=0; i<num_dxf_objs; i++)
    {
      if (dxf_objs[i].obj == obj)
        {
          found = &dxf_objs[i];
          break;
        }
    }
  if (!found)
    {
      LOG_ERROR("obj not found\n");
      return NULL;
    }
  // then search field
  for (i=0; i<found->num_fields; i++)
    {
      if (!strcmp(found->fields[i].name, name))
        return &found->fields[i];
    }
  return NULL;
}

static int
dxf_common_entity_handle_data(Bit_Chain *dat, Dwg_Object* obj)
{
  (void)dat; (void)obj;
  return 0;
}

#include "dwg.spec"

/* returns 0 if object was handled,
   DWG_ERR_UNHANDLEDCLASS or some other error otherwise
 */
static int
dwg_indxf_variable_type(Dwg_Data * dwg, Bit_Chain *dat, Dwg_Object* obj)
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
  // almost always false
  is_entity = dwg_class_is_entity(klass);

  #include "classes.inc"

  return DWG_ERR_UNHANDLEDCLASS;
}

static int
dwg_indxf_object(Bit_Chain *restrict dat, Dwg_Object *restrict obj)
{
  int error = 0;
  if (!obj || !obj->parent)
    return DWG_ERR_INTERNALERROR;

  switch (obj->type)
    {
    case DWG_TYPE_TEXT:
      return dwg_indxf_TEXT(dat, obj);
    case DWG_TYPE_ATTRIB:
      return dwg_indxf_ATTRIB(dat, obj);
    case DWG_TYPE_ATTDEF:
      return dwg_indxf_ATTDEF(dat, obj);
    case DWG_TYPE_BLOCK:
      return dwg_indxf_BLOCK(dat, obj);
    case DWG_TYPE_ENDBLK:
      return dwg_indxf_ENDBLK(dat, obj);
    case DWG_TYPE_SEQEND:
      return dwg_indxf_SEQEND(dat, obj);
    case DWG_TYPE_INSERT:
      return dwg_indxf_INSERT(dat, obj);
    case DWG_TYPE_MINSERT:
      return dwg_indxf_MINSERT(dat, obj);
    case DWG_TYPE_VERTEX_2D:
      return dwg_indxf_VERTEX_2D(dat, obj);
    case DWG_TYPE_VERTEX_3D:
      return dwg_indxf_VERTEX_3D(dat, obj);
    case DWG_TYPE_VERTEX_MESH:
      return dwg_indxf_VERTEX_MESH(dat, obj);
    case DWG_TYPE_VERTEX_PFACE:
      return dwg_indxf_VERTEX_PFACE(dat, obj);
    case DWG_TYPE_VERTEX_PFACE_FACE:
      return dwg_indxf_VERTEX_PFACE_FACE(dat, obj);
    case DWG_TYPE_POLYLINE_2D:
      return dwg_indxf_POLYLINE_2D(dat, obj);
    case DWG_TYPE_POLYLINE_3D:
      return dwg_indxf_POLYLINE_3D(dat, obj);
    case DWG_TYPE_ARC:
      return dwg_indxf_ARC(dat, obj);
    case DWG_TYPE_CIRCLE:
      return dwg_indxf_CIRCLE(dat, obj);
    case DWG_TYPE_LINE:
      return dwg_indxf_LINE(dat, obj);
    case DWG_TYPE_DIMENSION_ORDINATE:
      return dwg_indxf_DIMENSION_ORDINATE(dat, obj);
    case DWG_TYPE_DIMENSION_LINEAR:
      return dwg_indxf_DIMENSION_LINEAR(dat, obj);
    case DWG_TYPE_DIMENSION_ALIGNED:
      return dwg_indxf_DIMENSION_ALIGNED(dat, obj);
    case DWG_TYPE_DIMENSION_ANG3PT:
      return dwg_indxf_DIMENSION_ANG3PT(dat, obj);
    case DWG_TYPE_DIMENSION_ANG2LN:
      return dwg_indxf_DIMENSION_ANG2LN(dat, obj);
    case DWG_TYPE_DIMENSION_RADIUS:
      return dwg_indxf_DIMENSION_RADIUS(dat, obj);
    case DWG_TYPE_DIMENSION_DIAMETER:
      return dwg_indxf_DIMENSION_DIAMETER(dat, obj);
    case DWG_TYPE_POINT:
      return dwg_indxf_POINT(dat, obj);
    case DWG_TYPE__3DFACE:
      return dwg_indxf__3DFACE(dat, obj);
    case DWG_TYPE_POLYLINE_PFACE:
      return dwg_indxf_POLYLINE_PFACE(dat, obj);
    case DWG_TYPE_POLYLINE_MESH:
      return dwg_indxf_POLYLINE_MESH(dat, obj);
    case DWG_TYPE_SOLID:
      return dwg_indxf_SOLID(dat, obj);
    case DWG_TYPE_TRACE:
      return dwg_indxf_TRACE(dat, obj);
    case DWG_TYPE_SHAPE:
      return dwg_indxf_SHAPE(dat, obj);
    case DWG_TYPE_VIEWPORT:
      return dwg_indxf_VIEWPORT(dat, obj);
    case DWG_TYPE_ELLIPSE:
      return dwg_indxf_ELLIPSE(dat, obj);
    case DWG_TYPE_SPLINE:
      return dwg_indxf_SPLINE(dat, obj);
    case DWG_TYPE_REGION:
      return dwg_indxf_REGION(dat, obj);
    case DWG_TYPE__3DSOLID:
      return dwg_indxf__3DSOLID(dat, obj);
    case DWG_TYPE_BODY:
      return dwg_indxf_BODY(dat, obj);
    case DWG_TYPE_RAY:
      return dwg_indxf_RAY(dat, obj);
    case DWG_TYPE_XLINE:
      return dwg_indxf_XLINE(dat, obj);
    case DWG_TYPE_DICTIONARY:
      return dwg_indxf_DICTIONARY(dat, obj);
    case DWG_TYPE_MTEXT:
      return dwg_indxf_MTEXT(dat, obj);
    case DWG_TYPE_LEADER:
      return dwg_indxf_LEADER(dat, obj);
    case DWG_TYPE_TOLERANCE:
      return dwg_indxf_TOLERANCE(dat, obj);
    case DWG_TYPE_MLINE:
      return dwg_indxf_MLINE(dat, obj);
    case DWG_TYPE_BLOCK_CONTROL:
      return dwg_indxf_BLOCK_CONTROL(dat, obj);
    case DWG_TYPE_BLOCK_HEADER:
      return dwg_indxf_BLOCK_HEADER(dat, obj);
    case DWG_TYPE_LAYER_CONTROL:
      return dwg_indxf_LAYER_CONTROL(dat, obj);
    case DWG_TYPE_LAYER:
      return dwg_indxf_LAYER(dat, obj);
    case DWG_TYPE_STYLE_CONTROL:
      return dwg_indxf_STYLE_CONTROL(dat, obj);
    case DWG_TYPE_STYLE:
      return dwg_indxf_STYLE(dat, obj);
    case DWG_TYPE_LTYPE_CONTROL:
      return dwg_indxf_LTYPE_CONTROL(dat, obj);
    case DWG_TYPE_LTYPE:
      return dwg_indxf_LTYPE(dat, obj);
    case DWG_TYPE_VIEW_CONTROL:
      return dwg_indxf_VIEW_CONTROL(dat, obj);
    case DWG_TYPE_VIEW:
      return dwg_indxf_VIEW(dat, obj);
    case DWG_TYPE_UCS_CONTROL:
      return dwg_indxf_UCS_CONTROL(dat, obj);
    case DWG_TYPE_UCS:
      return dwg_indxf_UCS(dat, obj);
    case DWG_TYPE_VPORT_CONTROL:
      return dwg_indxf_VPORT_CONTROL(dat, obj);
    case DWG_TYPE_VPORT:
      return dwg_indxf_VPORT(dat, obj);
    case DWG_TYPE_APPID_CONTROL:
      return dwg_indxf_APPID_CONTROL(dat, obj);
    case DWG_TYPE_APPID:
      return dwg_indxf_APPID(dat, obj);
    case DWG_TYPE_DIMSTYLE_CONTROL:
      return dwg_indxf_DIMSTYLE_CONTROL(dat, obj);
    case DWG_TYPE_DIMSTYLE:
      return dwg_indxf_DIMSTYLE(dat, obj);
    case DWG_TYPE_VPORT_ENTITY_CONTROL:
      return dwg_indxf_VPORT_ENTITY_CONTROL(dat, obj);
    case DWG_TYPE_VPORT_ENTITY_HEADER:
      return dwg_indxf_VPORT_ENTITY_HEADER(dat, obj);
    case DWG_TYPE_GROUP:
      return dwg_indxf_GROUP(dat, obj);
    case DWG_TYPE_MLINESTYLE:
      return dwg_indxf_MLINESTYLE(dat, obj);
    case DWG_TYPE_OLE2FRAME:
      return dwg_indxf_OLE2FRAME(dat, obj);
    case DWG_TYPE_DUMMY:
      return 0; //dwg_indxf_DUMMY(dat, obj);
    case DWG_TYPE_LONG_TRANSACTION:
      return dwg_indxf_LONG_TRANSACTION(dat, obj);
    case DWG_TYPE_LWPOLYLINE:
      return dwg_indxf_LWPOLYLINE(dat, obj);
    case DWG_TYPE_HATCH:
      return dwg_indxf_HATCH(dat, obj);
    case DWG_TYPE_XRECORD:
      return dwg_indxf_XRECORD(dat, obj);
    case DWG_TYPE_PLACEHOLDER:
      return dwg_indxf_PLACEHOLDER(dat, obj);
    case DWG_TYPE_OLEFRAME:
      return dwg_indxf_OLEFRAME(dat, obj);
    case DWG_TYPE_VBA_PROJECT:
      LOG_ERROR("Unhandled Object VBA_PROJECT. Has its own section\n");
      return DWG_ERR_INVALIDTYPE;
      //dwg_indxf_VBA_PROJECT(dat, obj);
    case DWG_TYPE_LAYOUT:
      return dwg_indxf_LAYOUT(dat, obj);
    case DWG_TYPE_PROXY_ENTITY:
      return dwg_indxf_PROXY_ENTITY(dat, obj);
    case DWG_TYPE_PROXY_OBJECT:
      return dwg_indxf_PROXY_OBJECT(dat, obj);
    default:
      if (obj->type == obj->parent->layout_number)
        {
          return dwg_indxf_LAYOUT(dat, obj);
        }
      /* > 500 */
      if (DWG_ERR_UNHANDLEDCLASS &
          (error = dwg_indxf_variable_type(obj->parent, dat, obj)))
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
              return error; //dwg_indxf_UNKNOWN_OBJ(dat, obj);
            }
          else if (klass)
            {
              return error; //dwg_indxf_UNKNOWN_ENT(dat, obj);
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
  return error;
}

#define DXF_CHECK_ENDSEC \
  if (dat->byte >= dat->size || pair->code == 0) \
    return 0
#define DXF_BREAK_ENDSEC \
  if (dat->byte >= dat->size || \
      (pair->code == 0 && !strcmp(pair->value.s, "ENDSEC"))) \
    break
#define DXF_RETURN_ENDSEC(what) \
  if (dat->byte >= dat->size || \
      (pair->code == 0 && !strcmp(pair->value.s, "ENDSEC"))) { \
    dxf_free_pair(pair); \
    return what; \
  }

static int dxf_expect_code(Bit_Chain *dat, Dxf_Pair *pair, int code)
{
  while (pair->code != code)
    {
      dxf_free_pair(pair);
      pair = dxf_read_pair(dat);
      dxf_skip_comment(dat, pair);
      DXF_CHECK_EOF;
      if (pair->code != code) {
        LOG_ERROR("Expecting DXF code %d, got %d (at %lu)",
                  code, pair->code, dat->byte);
      }
    }
  return 0;
}

static int
dxf_header_read(Bit_Chain *dat, Dwg_Data* dwg)
{
  Dwg_Header_Variables* _obj = &dwg->header_vars;
  Dwg_Object* obj = NULL;
  const int minimal = dwg->opts & 0x10;
  double ms;
  char* codepage;
  Dxf_Pair *pair = dxf_read_pair(dat);

  // define fields (unordered)
  #include "header_variables_dxf.spec"

  while (pair->code != 0) {
    pair = dxf_read_pair(dat);
    DXF_BREAK_ENDSEC;

    //TODO find name in header struct and set value

    dxf_free_pair(pair);
  }
  dxf_free_pair(pair);

  // TODO: convert DWGCODEPAGE string to header.codepage number
  if (!strcmp(_obj->DWGCODEPAGE, "ANSI_1252"))
      dwg->header.codepage = 30;

  return 0;
}

static int
dxf_classes_read (Bit_Chain *dat, Dwg_Data * dwg)
{
  BITCODE_BL i;
  Dxf_Pair *pair = dxf_read_pair(dat);
  Dwg_Class *klass;

  while (1) { // read next class
    // add class (see decode)
    i = dwg->num_classes;
    if (i == 0)
      dwg->dwg_class = malloc(sizeof(Dwg_Class));
    else
      dwg->dwg_class = realloc(dwg->dwg_class, (i + 1) * sizeof(Dwg_Class));
    if (!dwg->dwg_class) { LOG_ERROR("Out of memory"); return DWG_ERR_OUTOFMEM; }

    klass = &dwg->dwg_class[i];
    memset(klass, 0, sizeof(Dwg_Class));

    while (pair->code != 0) { // read until next 0 CLASS
      pair = dxf_read_pair(dat);
      switch (pair->code) {
      case 1: STRADD(klass->dxfname, pair->value.s); break;
      case 2: STRADD(klass->cppname, pair->value.s); break;
      case 3: STRADD(klass->appname, pair->value.s); break;
      case 90: klass->proxyflag = pair->value.l; break;
      case 91: klass->num_instances = pair->value.l; break;
      case 280: klass->wasazombie = (BITCODE_B)pair->value.i; break;
      case 281: klass->item_class_id = pair->value.i ? 0x1f3 : 0x1f2; break;
      default: LOG_WARN("Unknown DXF code for class[%d].%d", i, pair->code);
               break;
      }
      dxf_free_pair(pair);
    }
    DXF_RETURN_ENDSEC(0); // next class or ENDSEC
    if (strcmp(pair->value.s, "CLASS")) { // or something else
      LOG_ERROR("Unexpexted DXF 0 %s at class[%d]", pair->value.s, i);
      return DWG_ERR_CLASSESNOTFOUND;
    }
    dwg->num_classes++;
  }
  return 0;
}

static int
dxf_tables_read (Bit_Chain *dat, Dwg_Data * dwg)
{
  (void)dwg;

  SECTION(TABLES);
  TABLE(VPORT);
  //...
  ENDTAB();
  ENDSEC();
  return 0;
}

static int
dxf_blocks_read (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  (void)dwg;

  SECTION(BLOCKS);
  //...
  ENDSEC();
  return 0;
}

static int
dxf_entities_read (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  Dxf_Pair *pair;
  Dwg_Object *obj = NULL;
  (void)dwg;

  SECTION(ENTITIES);
  while (dat->byte < dat->size) {
    pair = dxf_read_pair(dat);
    dxf_expect_code(dat, pair, 0);
    DXF_CHECK_EOF;
    dwg_indxf_object(dat, obj); //TODO obj must be already created here
  }
  ENDSEC();
  return 0;
}

static int
dxf_objects_read (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  Dxf_Pair *pair;
  Dwg_Object *obj = NULL;
  (void)dwg;

  SECTION(OBJECTS);
  while (dat->byte < dat->size) {
    pair = dxf_read_pair(dat);
    dxf_expect_code(dat, pair, 0);
    DXF_CHECK_EOF;
    dwg_indxf_object(dat, obj); //TODO obj must be already created here
  }
  ENDSEC();
  return 0;
}

static int
dxf_preview_read (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  (void)dwg;
  SECTION(THUMBNAILIMAGE);
  //VALUE_RL(pic->size, 90);
  //VALUE_BINARY(pic->chain, pic->size, 310);
  ENDSEC();
  return 0;
}

int
dwg_read_dxf(Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  const int minimal = dwg->opts & 0x10;
  Dxf_Pair *pair;
  //warn if minimal != 0
  //struct Dwg_Header *obj = &dwg->header;
  loglevel = dwg->opts & 0xf;

  num_dxf_objs = 0;
  size_dxf_objs = 1000;
  dxf_objs = malloc(1000*sizeof(Dxf_Objs));

  while (dat->byte < dat->size) {
    pair = dxf_read_pair(dat);
    dxf_expect_code(dat, pair, 0);
    DXF_CHECK_EOF;
    if (!strcmp(pair->value.s, "SECTION"))
      {
        dxf_free_pair(pair);
        pair = dxf_read_pair(dat);
        dxf_expect_code(dat, pair, 2);
        DXF_CHECK_EOF;
        if (!strcmp(pair->value.s, "HEADER"))
          {
            dxf_free_pair(pair);
            dxf_header_read (dat, dwg);
          }
        else if (!strcmp(pair->value.s, "CLASSES"))
          {
            dxf_free_pair(pair);
            dxf_classes_read (dat, dwg);
          }
        else if (!strcmp(pair->value.s, "TABLES"))
          {
            dxf_free_pair(pair);
            dxf_tables_read (dat, dwg);
          }
        else if (!strcmp(pair->value.s, "BLOCKS"))
          {
            dxf_free_pair(pair);
            dxf_blocks_read (dat, dwg);
          }
        else if (!strcmp(pair->value.s, "ENTITIES"))
          {
            dxf_free_pair(pair);
            dxf_entities_read (dat, dwg);
          }
        else if (!strcmp(pair->value.s, "OBJECTS"))
          {
            dxf_free_pair(pair);
            dxf_objects_read (dat, dwg);
          }
        if (!strcmp(pair->value.s, "THUMBNAIL"))
          {
            dxf_free_pair(pair);
            dxf_preview_read (dat, dwg);
          }
      }
  }
  return dwg->num_objects ? 1 : 0;
}

#undef IS_ENCODE
#undef IS_DXF
