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
#include "out_dxfb.h"

#define DWG_LOGLEVEL DWG_LOGLEVEL_TRACE
#include "logging.h"

/* the current version per spec block */
static unsigned int cur_ver = 0;
static char buf[4096];

// imported
extern const char* dxf_codepage (int code, Dwg_Data* dwg);
//extern void obj_string_stream(Bit_Chain *dat, BITCODE_RL bitsize, Bit_Chain *str);
//extern const char *dxf_format (int code);

//private
static void dxfb_common_entity_handle_data(Bit_Chain *dat, Dwg_Object* obj);

/*--------------------------------------------------------------------------------
 * MACROS
 */

#define IS_PRINT

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

#define FIELD_VALUE(name) _obj->name
#define ANYCODE -1
//TODO
#define FIELD_HANDLE(name, handle_code, dxf) \
  if (_obj->name) { \
    fprintf(dat->fh, #name ": \"HANDLE(%d.%d.%lu) absolute:%lu\",\n",\
           _obj->name->handleref.code,                     \
           _obj->name->handleref.size,                     \
           _obj->name->handleref.value,                    \
           _obj->name->absolute_ref);                      \
  }

#define GROUP(code)                  \
    {                                \
    if (dat->version < R_14) {       \
      unsigned char icode = (unsigned char)code; \
      fwrite(&icode, 1, 1, dat->fh); \
    } else {                         \
      short icode = code;            \
      fwrite(&icode, 2, 1, dat->fh); \
    }                                \
  }
#define FIELD_TV(name,dxf) \
  if (_obj->name != NULL && dxf != 0) { VALUE_TV(_obj->name,dxf); }
#define FIELD_TU(name,dxf) \
  if (_obj->name != NULL && dxf != 0) { VALUE_TU(_obj->name, dxf); }
#define VALUE_T(value,dxf) \
  { if (dat->version >= R_2007) { VALUE_TU(value, dxf); } \
    else                        { VALUE_TV(value, dxf); } }
#define FIELD_T(name,dxf) \
  { if (dat->version >= R_2007) { FIELD_TU(name, dxf); } \
    else                        { FIELD_TV(name, dxf); } }
#define FIELD_TF(name,len,dxf)  VALUE_TV(_obj->name, dxf)
#define FIELD_TFF(name,len,dxf) VALUE_TV(_obj->name, dxf)

#define HEADER_TV(name,dxf) \
    HEADER_9(name);\
    VALUE_TV(dwg->header_vars.name, dxf)
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
    BITCODE_RC c = value;\
    GROUP(dxf);\
    fwrite(&c, 1, 1, dat->fh); \
  }
#define FIELD_RC(name,dxf) VALUE_RC(_obj->name, dxf)
#define HEADER_RC(name,dxf) \
    HEADER_9(name);\
    VALUE_RC(dwg->header_vars.name, dxf)
#define HEADER_B(name,dxf) HEADER_RC(name,dxf)

#define VALUE_RS(value,dxf) \
  {\
    BITCODE_RS s = value;\
    GROUP(dxf);\
    fwrite(&s, 2, 1, dat->fh);\
  }
#define FIELD_RS(name,dxf) VALUE_RS(_obj->name, dxf)
#define HEADER_RS(name,dxf) \
    HEADER_9(name);\
    VALUE_RS(dwg->header_vars.name, dxf)

#define VALUE_RD(value,dxf)\
  {\
    double d = value;\
    GROUP(dxf);\
    fwrite(&d, 1, 8, dat->fh); \
  }
#define FIELD_RD(name,dxf) VALUE_RD(_obj->name,dxf)
#define HEADER_RD(name,dxf) \
    HEADER_9(name);\
    VALUE_RD(dwg->header_vars.name, dxf)

#define VALUE_RL(value,dxf)\
  {\
    BITCODE_RL s = value;\
    GROUP(dxf);\
    fwrite(&s, 4, 1, dat->fh);\
  }
#define FIELD_RL(name,dxf) VALUE_RL(_obj->name,dxf)
#define HEADER_RL(name,dxf) \
    HEADER_9(name);\
    VALUE_RL(dwg->header_vars.name, dxf)

#define VALUE_H(value,dxf) \
  {\
    Dwg_Object_Ref *ref = value;\
    if (ref && ref->obj) {VALUE_RS(ref->absolute_ref, dxf);}\
    else {VALUE_RS(0, dxf);}                                \
  }
#define HEADER_H(name,dxf) \
    HEADER_9(name);\
    VALUE_H(dwg->header_vars.name, dxf)

#define HANDLE_NAME(name, code, section) \
  VALUE_HANDLE_NAME(dwg->header_vars.name, dxf, section)
//TODO: convert STANDARD to Standard?
#define VALUE_HANDLE_NAME(value,dxf,section)\
  {\
    Dwg_Object_Ref *ref = value;\
    if (ref && ref->obj && ref->obj->supertype == DWG_SUPERTYPE_OBJECT) { \
      VALUE_TV(ref->obj->tio.object->tio.section->entry_name, dxf);} \
    else VALUE_TV("", dxf);\
  }
#define FIELD_HANDLE_NAME(name,dxf,section) VALUE_HANDLE_NAME(_obj->name,dxf,section)
#define HEADER_HANDLE_NAME(name,dxf,section)\
    HEADER_9(name);\
    VALUE_HANDLE_NAME(dwg->header_vars.name,dxf,section)

#define HEADER_RLL(name,dxf) \
  {\
    BITCODE_RLL s = _obj->name;\
    GROUP(9);\
    fprintf (dat->fh, "$%s%c", #name, 0);\
    GROUP(dxf);\
    fwrite(&s, 8, 1, dat->fh);\
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
#define FIELD_CMC(name,dxf) VALUE_RS(_obj->name.index, dxf)
#define HEADER_CMC(name,dxf) \
    HEADER_9(name);\
    VALUE_RS(dwg->header_vars.name.index, dxf)
#define FIELD_TIMEBLL(name,dxf) \
  VALUE_RL(_obj->name.days, dxf); VALUE_RL(_obj->name.ms, dxf)

//FIELD_VECTOR_N(name, type, size):
// reads data of the type indicated by 'type' 'size' times and stores
// it all in the vector called 'name'.
#define FIELD_VECTOR_N(name, type, size, dxf)\
  if (dxf) {\
    for (vcount=0; vcount < (int)size; vcount++) \
      VALUE (_obj->name[vcount], type, dxf); \
  }
#define FIELD_VECTOR_T(name, size, dxf)\
  if (dxf) {\
    PRE (R_2007) { \
      for (vcount=0; vcount < (int)_obj->size; vcount++) \
        VALUE_TV (_obj->name[vcount], dxf) \
    } else { \
      for (vcount=0; vcount < (int)_obj->size; vcount++) \
        VALUE_TU (_obj->name[vcount], dxf) \
    }\
  }

#define FIELD_VECTOR(name, type, size, dxf) \
  FIELD_VECTOR_N(name, type, _obj->size, dxf)

#define FIELD_2RD_VECTOR(name, size, dxf)\
  if (dxf) {\
    for (vcount=0; vcount < (int)_obj->size; vcount++)\
      {\
        FIELD_2RD(name[vcount], dxf);\
      }\
    }

#define FIELD_2DD_VECTOR(name, size, dxf)\
  if (dxf) {\
    FIELD_2RD(name[0], 0);\
    for (vcount = 1; vcount < (int)_obj->size; vcount++)\
      {\
        FIELD_2DD(name[vcount], FIELD_VALUE(name[vcount - 1].x), FIELD_VALUE(name[vcount - 1].y), dxf);\
      }\
    }

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

#define FIELD_XDATA(name, size)

#define REACTORS(code)\
  if (obj->tio.object->num_reactors) {\
    VALUE_TV("{ACAD_REACTORS", 102);\
    for (vcount=0; vcount < (int)obj->tio.object->num_reactors; vcount++)\
      {\
        FIELD_HANDLE_N(reactors[vcount], vcount, code, 330);\
      }\
    VALUE_TV("}", 102);\
  }
#define ENT_REACTORS(code)\
  if (_obj->num_reactors) {\
    VALUE_TV("{ACAD_REACTORS", 102);\
    for (vcount=0; vcount < (int)_obj->num_reactors; vcount++)\
      {\
        FIELD_HANDLE_N(reactors[vcount], vcount, code, 330);\
      }\
    VALUE_TV("}", 102);\
  }

#define XDICOBJHANDLE(code)
#define ENT_XDICOBJHANDLE(code)

#define REPEAT_N(times, name, type) \
  for (rcount=0; rcount<(int)times; rcount++)

#define REPEAT(times, name, type) \
  for (rcount=0; rcount<(int)_obj->times; rcount++)

#define REPEAT2(times, name, type) \
  for (rcount2=0; rcount2<(int)_obj->times; rcount2++)

#define REPEAT3(times, name, type) \
  for (rcount3=0; rcount3<(int)_obj->times; rcount3++)

#define REPEAT4(times, name, type) \
  for (rcount4=0; rcount4<(int)_obj->times; rcount4++)

#define COMMON_ENTITY_HANDLE_DATA \
  SINCE(R_13) { \
    dxfb_common_entity_handle_data(dat, obj); \
  }
#define SECTION_STRING_STREAM
#define START_STRING_STREAM
#define END_STRING_STREAM
#define START_HANDLE_STREAM

#define DWG_ENTITY(token) \
static void \
dwg_dxfb_##token (Bit_Chain *dat, Dwg_Object * obj) \
{\
  int vcount, rcount, rcount2, rcount3, rcount4; \
  Dwg_Entity_##token *ent, *_obj;\
  Dwg_Object_Entity *_ent;\
  LOG_INFO("Entity " #token ":\n")\
  _ent = obj->tio.entity;\
  _obj = ent = _ent->tio.token;\
  LOG_TRACE("Entity handle: %d.%d.%lu\n",\
    obj->handle.code,\
    obj->handle.size,\
    obj->handle.value)

#define DWG_ENTITY_END }

#define DWG_OBJECT(token) \
static void \
dwg_dxfb_ ##token (Bit_Chain *dat, Dwg_Object * obj) \
{ \
  int vcount, rcount, rcount2, rcount3, rcount4;\
  Bit_Chain *hdl_dat = dat;\
  Dwg_Object_##token *_obj;\
  LOG_INFO("Object " #token ":\n")\
  _obj = obj->tio.object->tio.token;\
  LOG_TRACE("Object handle: %d.%d.%lu\n",\
    obj->handle.code,\
    obj->handle.size,\
    obj->handle.value)

#define DWG_OBJECT_END }

#include "dwg.spec"

/* returns 1 if object could be printd and 0 otherwise
 */
static int
dwg_dxfb_variable_type(Dwg_Data * dwg, Bit_Chain *dat, Dwg_Object* obj)
{
  int i;
  char *dxfname;
  Dwg_Class *klass;
  int is_entity;

  if ((obj->type - 500) > dwg->num_classes)
    return 0;

  i = obj->type - 500;
  klass = &dwg->dwg_class[i];
  dxfname = klass->dxfname;
  // almost always false
  is_entity = dwg_class_is_entity(klass);

#define UNHANDLED_CLASS \
      LOG_WARN("Unhandled Class %s %d %s (0x%x%s)", is_entity ? "entity" : "object",\
               klass->number, dxfname, klass->proxyflag,\
               klass->wasazombie ? " was proxy" : "")
#define UNTESTED_CLASS \
      LOG_WARN("Untested Class %s %d %s (0x%x%s)", is_entity ? "entity" : "object",\
               klass->number, dxfname, klass->proxyflag,\
               klass->wasazombie ? " was proxy" : "")
  
  if (!strcmp(dxfname, "ACDBDICTIONARYWDFLT"))
    {
      assert(!is_entity);
      dwg_dxfb_DICTIONARYWDLFT(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "DICTIONARYVAR"))
    {
      assert(!is_entity);
      dwg_dxfb_DICTIONARYVAR(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "HATCH"))
    {
      assert(!is_entity);
      dwg_dxfb_HATCH(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "FIELDLIST"))
    {
      UNTESTED_CLASS;
      assert(!is_entity);
      dwg_dxfb_FIELDLIST(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "GROUP"))
    {
      UNTESTED_CLASS;
      assert(!is_entity);
      dwg_dxfb_GROUP(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "IDBUFFER"))
    {
      dwg_dxfb_IDBUFFER(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "IMAGE"))
    {
      dwg_dxfb_IMAGE(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "IMAGEDEF"))
    {
      dwg_dxfb_IMAGEDEF(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "IMAGEDEF_REACTOR"))
    {
      dwg_dxfb_IMAGEDEF_REACTOR(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "LAYER_INDEX"))
    {
      dwg_dxfb_LAYER_INDEX(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "LAYOUT"))
    {
      dwg_dxfb_LAYOUT(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "LWPOLYLINE"))
    {
      dwg_dxfb_LWPOLYLINE(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "MULTILEADER"))
    {
#ifdef DEBUG_MULTILEADER
      UNTESTED_CLASS; //broken Leader_Line's/Points
      dwg_dxfb_MULTILEADER(dat, obj);
      return 1;
#else
      UNHANDLED_CLASS;
      return 0;
#endif
    }
  if (!strcmp(dxfname, "MLEADERSTYLE"))
    {
      dwg_dxfb_MLEADERSTYLE(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "OLE2FRAME"))
    {
      dwg_dxfb_OLE2FRAME(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "OBJECTCONTEXTDATA")
      || strcmp(klass->cppname, "AcDbObjectContextData"))
    {
      dwg_dxfb_OBJECTCONTEXTDATA(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "ACDBPLACEHOLDER"))
    {
      dwg_dxfb_PLACEHOLDER(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "PROXY"))
    {
      dwg_dxfb_PROXY_OBJECT(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "RASTERVARIABLES"))
    {
      dwg_dxfb_RASTERVARIABLES(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "SCALE"))
    {
      dwg_dxfb_SCALE(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "SORTENTSTABLE"))
    {
      dwg_dxfb_SORTENTSTABLE(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "SPATIAL_FILTER"))
    {
      dwg_dxfb_SPATIAL_FILTER(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "SPATIAL_INDEX"))
    {
      dwg_dxfb_SPATIAL_INDEX(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "TABLE"))
    {
      UNTESTED_CLASS;
      dwg_dxfb_TABLE(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "WIPEOUTVARIABLE"))
    {
      UNTESTED_CLASS;
      dwg_dxfb_WIPEOUTVARIABLE(dat, obj);
      return 0;
    }
  if (!strcmp(dxfname, "WIPEOUT"))
    {
      dwg_dxfb_WIPEOUT(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "FIELDLIST"))
    {
      UNTESTED_CLASS;
      dwg_dxfb_FIELDLIST(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "VBA_PROJECT"))
    {
#ifdef DEBUG_VBA_PROJECT
      UNTESTED_CLASS;
      dwg_dxfb_VBA_PROJECT(dat, obj);
      return 1;
#else
      UNHANDLED_CLASS;
      return 0;
#endif
    }
  if (!strcmp(dxfname, "CELLSTYLEMAP"))
    {
#ifdef DEBUG_CELLSTYLEMAP
      UNTESTED_CLASS;
      dwg_dxfb_CELLSTYLEMAP(dat, obj);
      return 1;
#else
      UNHANDLED_CLASS;
      return 0;
#endif
    }
  if (!strcmp(dxfname, "VISUALSTYLE"))
    {
      dwg_dxfb_VISUALSTYLE(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "AcDbField")) //?
    {
      UNTESTED_CLASS;
      dwg_dxfb_FIELD(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "TABLECONTENT"))
    {
      UNTESTED_CLASS;
      dwg_dxfb_TABLECONTENT(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "TABLEGEOMETRY"))
    {
      UNTESTED_CLASS;
      dwg_dxfb_TABLEGEOMETRY(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "GEODATA"))
    {
      UNTESTED_CLASS;
      dwg_dxfb_GEODATA(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "XRECORD"))
    {
      dwg_dxfb_XRECORD(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "ARCALIGNEDTEXT"))
    {
      UNHANDLED_CLASS;
      //assert(!is_entity);
      //dwg_dxfb_ARCALIGNEDTEXT(dat, obj);
      return 0;
    }
  if (!strcmp(dxfname, "DIMASSOC"))
    {
      UNHANDLED_CLASS;
      assert(!is_entity);
      //dwg_dxfb_DIMASSOC(dat, obj);
      return 0;
    }
  if (!strcmp(dxfname, "MATERIAL"))
    {
      UNHANDLED_CLASS;
      assert(!is_entity);
      //dwg_dxfb_MATERIAL(dat, obj);
      return 0;
    }
  if (!strcmp(dxfname, "TABLESTYLE"))
    {
      UNHANDLED_CLASS;
      assert(!is_entity);
      //dwg_dxfb_TABLESTYLE(dat, obj);
      return 0;
    }
  if (!strcmp(dxfname, "DBCOLOR"))
    {
      UNHANDLED_CLASS;
      assert(!is_entity);
      //dwg_dxfb_DBCOLOR(dat, obj);
      return 0;
    }
  if (!strcmp(dxfname, "ACDBSECTIONVIEWSTYLE"))
    {
      UNHANDLED_CLASS;
      assert(!is_entity);
      //dwg_dxfb_SECTIONVIEWSTYLE(dat, obj);
      return 0;
    }
  if (!strcmp(dxfname, "ACDBDETAILVIEWSTYLE"))
    {
      UNHANDLED_CLASS;
      assert(!is_entity);
      //dwg_dxfb_DETAILVIEWSTYLE(dat, obj);
      return 0;
    }
  if (!strcmp(dxfname, "ACDBASSOCNETWORK"))
    {
      UNHANDLED_CLASS;
      assert(!is_entity);
      //dwg_dxfb_ASSOCNETWORK(dat, obj);
      return 0;
    }
  if (!strcmp(dxfname, "ACDBASSOC2DCONSTRAINTGROUP"))
    {
      UNHANDLED_CLASS;
      assert(!is_entity);
      //dwg_dxfb_ASSOC2DCONSTRAINTGROUP(dat, obj);
      return 0;
    }
  if (!strcmp(dxfname, "ACDBASSOCGEOMDEPENDENCY"))
    {
      UNHANDLED_CLASS;
      assert(!is_entity);
      //dwg_dxfb_ASSOCGEOMDEPENDENCY(dat, obj);
      return 0;
    }
  if (!strcmp(dxfname, "ACDB_LEADEROBJECTCONTEXTDATA_CLASS"))
    {
      //UNHANDLED_CLASS;
      //dwg_dxfb_LEADEROBJECTCONTEXTDATA(dat, obj);
      return 0;
    }

  return 0;
}

void
dwg_dxfb_object(Bit_Chain *dat, Dwg_Object *obj)
{

  switch (obj->type)
    {
    case DWG_TYPE_TEXT:
      dwg_dxfb_TEXT(dat, obj);
      break;
    case DWG_TYPE_ATTRIB:
      dwg_dxfb_ATTRIB(dat, obj);
      break;
    case DWG_TYPE_ATTDEF:
      dwg_dxfb_ATTDEF(dat, obj);
      break;
    case DWG_TYPE_BLOCK:
      dwg_dxfb_BLOCK(dat, obj);
      break;
    case DWG_TYPE_ENDBLK:
      dwg_dxfb_ENDBLK(dat, obj);
      break;
    case DWG_TYPE_SEQEND:
      dwg_dxfb_SEQEND(dat, obj);
      break;
    case DWG_TYPE_INSERT:
      dwg_dxfb_INSERT(dat, obj);
      break;
    case DWG_TYPE_MINSERT:
      dwg_dxfb_MINSERT(dat, obj);
      break;
    case DWG_TYPE_VERTEX_2D:
      dwg_dxfb_VERTEX_2D(dat, obj);
      break;
    case DWG_TYPE_VERTEX_3D:
      dwg_dxfb_VERTEX_3D(dat, obj);
      break;
    case DWG_TYPE_VERTEX_MESH:
      dwg_dxfb_VERTEX_MESH(dat, obj);
      break;
    case DWG_TYPE_VERTEX_PFACE:
      dwg_dxfb_VERTEX_PFACE(dat, obj);
      break;
    case DWG_TYPE_VERTEX_PFACE_FACE:
      dwg_dxfb_VERTEX_PFACE_FACE(dat, obj);
      break;
    case DWG_TYPE_POLYLINE_2D:
      dwg_dxfb_POLYLINE_2D(dat, obj);
      break;
    case DWG_TYPE_POLYLINE_3D:
      dwg_dxfb_POLYLINE_3D(dat, obj);
      break;
    case DWG_TYPE_ARC:
      dwg_dxfb_ARC(dat, obj);
      break;
    case DWG_TYPE_CIRCLE:
      dwg_dxfb_CIRCLE(dat, obj);
      break;
    case DWG_TYPE_LINE:
      dwg_dxfb_LINE(dat, obj);
      break;
    case DWG_TYPE_DIMENSION_ORDINATE:
      dwg_dxfb_DIMENSION_ORDINATE(dat, obj);
      break;
    case DWG_TYPE_DIMENSION_LINEAR:
      dwg_dxfb_DIMENSION_LINEAR(dat, obj);
      break;
    case DWG_TYPE_DIMENSION_ALIGNED:
      dwg_dxfb_DIMENSION_ALIGNED(dat, obj);
      break;
    case DWG_TYPE_DIMENSION_ANG3PT:
      dwg_dxfb_DIMENSION_ANG3PT(dat, obj);
      break;
    case DWG_TYPE_DIMENSION_ANG2LN:
      dwg_dxfb_DIMENSION_ANG2LN(dat, obj);
      break;
    case DWG_TYPE_DIMENSION_RADIUS:
      dwg_dxfb_DIMENSION_RADIUS(dat, obj);
      break;
    case DWG_TYPE_DIMENSION_DIAMETER:
      dwg_dxfb_DIMENSION_DIAMETER(dat, obj);
      break;
    case DWG_TYPE_POINT:
      dwg_dxfb_POINT(dat, obj);
      break;
    case DWG_TYPE__3DFACE:
      dwg_dxfb__3DFACE(dat, obj);
      break;
    case DWG_TYPE_POLYLINE_PFACE:
      dwg_dxfb_POLYLINE_PFACE(dat, obj);
      break;
    case DWG_TYPE_POLYLINE_MESH:
      dwg_dxfb_POLYLINE_MESH(dat, obj);
      break;
    case DWG_TYPE_SOLID:
      dwg_dxfb_SOLID(dat, obj);
      break;
    case DWG_TYPE_TRACE:
      dwg_dxfb_TRACE(dat, obj);
      break;
    case DWG_TYPE_SHAPE:
      dwg_dxfb_SHAPE(dat, obj);
      break;
    case DWG_TYPE_VIEWPORT:
      dwg_dxfb_VIEWPORT(dat, obj);
      break;
    case DWG_TYPE_ELLIPSE:
      dwg_dxfb_ELLIPSE(dat, obj);
      break;
    case DWG_TYPE_SPLINE:
      dwg_dxfb_SPLINE(dat, obj);
      break;
    case DWG_TYPE_REGION:
      dwg_dxfb_REGION(dat, obj);
      break;
    case DWG_TYPE__3DSOLID:
      dwg_dxfb__3DSOLID(dat, obj);
      break; /* Check the type of the object
              */
    case DWG_TYPE_BODY:
      dwg_dxfb_BODY(dat, obj);
      break;
    case DWG_TYPE_RAY:
      dwg_dxfb_RAY(dat, obj);
      break;
    case DWG_TYPE_XLINE:
      dwg_dxfb_XLINE(dat, obj);
      break;
    case DWG_TYPE_DICTIONARY:
      dwg_dxfb_DICTIONARY(dat, obj);
      break;
    case DWG_TYPE_MTEXT:
      dwg_dxfb_MTEXT(dat, obj);
      break;
    case DWG_TYPE_LEADER:
      dwg_dxfb_LEADER(dat, obj);
      break;
    case DWG_TYPE_TOLERANCE:
      dwg_dxfb_TOLERANCE(dat, obj);
      break;
    case DWG_TYPE_MLINE:
      dwg_dxfb_MLINE(dat, obj);
      break;
    case DWG_TYPE_BLOCK_CONTROL:
      dwg_dxfb_BLOCK_CONTROL(dat, obj);
      break;
    case DWG_TYPE_BLOCK_HEADER:
      dwg_dxfb_BLOCK_HEADER(dat, obj);
      break;
    case DWG_TYPE_LAYER_CONTROL:
      dwg_dxfb_LAYER_CONTROL(dat, obj);
      break;
    case DWG_TYPE_LAYER:
      dwg_dxfb_LAYER(dat, obj);
      break;
    case DWG_TYPE_STYLE_CONTROL:
      dwg_dxfb_STYLE_CONTROL(dat, obj);
      break;
    case DWG_TYPE_STYLE:
      dwg_dxfb_STYLE(dat, obj);
      break;
    case DWG_TYPE_LTYPE_CONTROL:
      dwg_dxfb_LTYPE_CONTROL(dat, obj);
      break;
    case DWG_TYPE_LTYPE:
      dwg_dxfb_LTYPE(dat, obj);
      break;
    case DWG_TYPE_VIEW_CONTROL:
      dwg_dxfb_VIEW_CONTROL(dat, obj);
      break;
    case DWG_TYPE_VIEW:
      dwg_dxfb_VIEW(dat, obj);
      break;
    case DWG_TYPE_UCS_CONTROL:
      dwg_dxfb_UCS_CONTROL(dat, obj);
      break;
    case DWG_TYPE_UCS:
      dwg_dxfb_UCS(dat, obj);
      break;
    case DWG_TYPE_VPORT_CONTROL:
      dwg_dxfb_VPORT_CONTROL(dat, obj);
      break;
    case DWG_TYPE_VPORT:
      dwg_dxfb_VPORT(dat, obj);
      break;
    case DWG_TYPE_APPID_CONTROL:
      dwg_dxfb_APPID_CONTROL(dat, obj);
      break;
    case DWG_TYPE_APPID:
      dwg_dxfb_APPID(dat, obj);
      break;
    case DWG_TYPE_DIMSTYLE_CONTROL:
      dwg_dxfb_DIMSTYLE_CONTROL(dat, obj);
      break;
    case DWG_TYPE_DIMSTYLE:
      dwg_dxfb_DIMSTYLE(dat, obj);
      break;
    case DWG_TYPE_VPORT_ENTITY_CONTROL:
      dwg_dxfb_VPORT_ENTITY_CONTROL(dat, obj);
      break;
    case DWG_TYPE_VPORT_ENTITY_HEADER:
      dwg_dxfb_VPORT_ENTITY_HEADER(dat, obj);
      break;
    case DWG_TYPE_GROUP:
      dwg_dxfb_GROUP(dat, obj);
      break;
    case DWG_TYPE_MLINESTYLE:
      dwg_dxfb_MLINESTYLE(dat, obj);
      break;
    case DWG_TYPE_OLE2FRAME:
      dwg_dxfb_OLE2FRAME(dat, obj);
      break;
    case DWG_TYPE_DUMMY:
      dwg_dxfb_DUMMY(dat, obj);
      break;
    case DWG_TYPE_LONG_TRANSACTION:
      dwg_dxfb_LONG_TRANSACTION(dat, obj);
      break;
    case DWG_TYPE_LWPOLYLINE:
      dwg_dxfb_LWPOLYLINE(dat, obj);
      break;
    case DWG_TYPE_HATCH:
      dwg_dxfb_HATCH(dat, obj);
      break;
    case DWG_TYPE_XRECORD:
      dwg_dxfb_XRECORD(dat, obj);
      break;
    case DWG_TYPE_PLACEHOLDER:
      dwg_dxfb_PLACEHOLDER(dat, obj);
      break;
    case DWG_TYPE_PROXY_ENTITY:
      dwg_dxfb_PROXY_ENTITY(dat, obj);
      break;
    case DWG_TYPE_OLEFRAME:
      dwg_dxfb_OLEFRAME(dat, obj);
      break;
    case DWG_TYPE_VBA_PROJECT:
      LOG_ERROR("Unhandled Object VBA_PROJECT. Has its own section\n");
      //dwg_dxfb_VBA_PROJECT(dat, obj);
      break;
    case DWG_TYPE_LAYOUT:
      dwg_dxfb_LAYOUT(dat, obj);
      break;
    default:
      if (obj->type == obj->parent->layout_number)
        {
          dwg_dxfb_LAYOUT(dat, obj);
        }
      /* > 500:
         TABLE, DICTIONARYWDLFT, IDBUFFER, IMAGE, IMAGEDEF, IMAGEDEF_REACTOR,
         LAYER_INDEX, OLE2FRAME, PROXY, RASTERVARIABLES, SORTENTSTABLE, SPATIAL_FILTER,
         SPATIAL_INDEX
      */
      else if (!dwg_dxfb_variable_type(obj->parent, dat, obj))
        {
          Dwg_Data *dwg = obj->parent;
          int is_entity;
          int i = obj->type - 500;
          Dwg_Class *klass = NULL;

          if (i <= (int)dwg->num_classes)
            {
              klass = &dwg->dwg_class[i];
              is_entity = dwg_class_is_entity(klass);
            }
          // properly dwg_decode_object/_entity for eed, reactors, xdic
          if (klass && !is_entity)
            {
              dwg_dxfb_UNKNOWN_OBJ(dat, obj);
            }
          else if (klass)
            {
              dwg_dxfb_UNKNOWN_ENT(dat, obj);
            }
          else // not a class
            {
              LOG_WARN("Unknown object, skipping eed/reactors/xdic");
              SINCE(R_2000)
                {
                  LOG_INFO("Object bitsize: %u\n", obj->bitsize)
                }
              LOG_INFO("Object handle: %d.%d.%lu\n",
                       obj->handle.code, obj->handle.size, obj->handle.value);
            }
        }
    }
}

static void
dxfb_common_entity_handle_data(Bit_Chain *dat, Dwg_Object* obj)
{
  (void)dat; (void)obj;
}

// see https://www.autodesk.com/techpubs/autocad/acad2000/dxf/header_section_group_codes_dxf_02.htm
void
dxfb_header_write(Bit_Chain *dat, Dwg_Data* dwg)
{
  Dwg_Header_Variables* _obj = &dwg->header_vars;
  Dwg_Object* obj = NULL;
  double ms;
  const int minimal = dwg->opts & 1;
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

  return;
}

static int
dxfb_classes_write (Bit_Chain *dat, Dwg_Data * dwg)
{
  unsigned int i;

  SECTION(HEADER);
  for (i=0; i < dwg->num_classes; i++)
    {
      RECORD(CLASS);
      VALUE_T (dwg->dwg_class[i].dxfname, 1);
      VALUE_T (dwg->dwg_class[i].cppname, 2);
      VALUE_T (dwg->dwg_class[i].appname, 3);
      VALUE_RS (dwg->dwg_class[i].proxyflag, 90);
      VALUE_RC (dwg->dwg_class[i].wasazombie, 280);
      // Is-an-entity. 1f2 for entities, 1f3 for objects
      VALUE_RC (dwg->dwg_class[i].item_class_id == 0x1F2 ? 1 : 0, 281);
    }
  ENDSEC();
  return 0;
}

static int
dxfb_tables_write (Bit_Chain *dat, Dwg_Data * dwg)
{
  (void)dwg;

  SECTION(TABLES);
  //...
  ENDSEC();
  return 0;
}

static int
dxfb_blocks_write (Bit_Chain *dat, Dwg_Data * dwg)
{
  (void)dwg;

  SECTION(BLOCKS);
  //...
  ENDSEC();
  return 0;
}

static int
dxfb_entities_write (Bit_Chain *dat, Dwg_Data * dwg)
{
  (void)dwg;

  SECTION(ENTITIES);
  //...
  ENDSEC();
  return 0;
}

static int
dxfb_objects_write (Bit_Chain *dat, Dwg_Data * dwg)
{
  (void)dwg;

  SECTION(OBJECTS);
  //...
  ENDSEC();
  return 0;
}

static int
dxfb_preview_write (Bit_Chain *dat, Dwg_Data * dwg)
{
  (void)dat; (void)dwg;
  //...
  return 0;
}

int
dwg_write_dxfb(Bit_Chain *dat, Dwg_Data * dwg)
{
  const int minimal = dwg->opts & 1;
  struct Dwg_Header *obj = &dwg->header;

  fprintf(dat->fh, "AutoCAD Binary DXF%s", "\r\n\0x1a\0");
  //VALUE(999, PACKAGE_STRING);

  // a minimal header requires only $ACADVER, $HANDSEED, and then ENTITIES
  // see https://pythonhosted.org/ezdxf/dxfinternals/filestructure.html
  SINCE(R_13)
  {
    dxfb_header_write (dat, dwg);

    SINCE(R_2000) {
      if (dxfb_classes_write (dat, dwg))
        goto fail;
    }

    if (dxfb_tables_write (dat, dwg))
      goto fail;

    if (dxfb_blocks_write (dat, dwg))
      goto fail;
  }

  if (dxfb_entities_write (dat, dwg))
    goto fail;

  SINCE(R_13) {
    if (dxfb_objects_write (dat, dwg))
      goto fail;
  }

  if (dwg->header.version >= R_2000 && !minimal) {
    if (dxfb_preview_write (dat, dwg))
      goto fail;
  }

  return 0;
 fail:
  return 1;
}

#undef IS_PRINT
