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

#include "common.h"
#include "bits.h"
#include "dwg.h"
#include "out_json.h"

#define DWG_LOGLEVEL DWG_LOGLEVEL_NONE
#include "logging.h"

/* the current version per spec block */
static unsigned int cur_ver = 0;

extern void
obj_string_stream(Bit_Chain *dat, BITCODE_RL bitsize, Bit_Chain *str);

/*--------------------------------------------------------------------------------
 * MACROS
 * TODO: use a dat field as indent level, i.e. bit.
 */

#define IS_PRINT

#define PREFIX for (int _i=0; _i<dat->bit; _i++) { fprintf (dat->fh, "  "); }
#define ARRAY    PREFIX fprintf (dat->fh, "[\n"); dat->bit++
#define ENDARRAY dat->bit--; PREFIX fprintf (dat->fh, "]\n")
#define HASH     PREFIX fprintf (dat->fh, "{\n"); dat->bit++
#define ENDHASH  dat->bit--; PREFIX fprintf (dat->fh, "}\n")
#define SECTION(name) PREFIX fprintf (dat->fh, "\"%s\": [\n", #name); dat->bit++;
#define ENDSEC()   ENDARRAY;

#define FIELD(name,type,dxf) \
    PREFIX fprintf(dat->fh, "\"" #name "\": " FORMAT_##type ",\n", _obj->name)
#define FIELD_CAST(name,type,cast,dxf) FIELD(name,cast,dxf)
#define FIELD_TRACE(name,type)
#define FIELD_TEXT(name,str) \
    PREFIX fprintf(dat->fh, "\"" #name "\": \"%s\",\n", str)
#ifdef HAVE_NATIVE_WCHAR2
# define FIELD_TEXT_TU(name,wstr) \
    PREFIX fprintf(dat->fh, "\"" #name "\": \"%ls\",\n", wstr)
#else
# define FIELD_TEXT_TU(name,wstr) \
  { \
    BITCODE_TU ws = (BITCODE_TU)wstr;\
    uint16_t _c; PREFIX \
    fprintf(dat->fh, "\"" #name "\": \""); \
    while ((_c = *ws++)) { \
      fprintf(dat->fh, "%c", (char)(_c & 0xff)); \
    } \
    fprintf(dat->fh, "\",\n"); \
  }
#endif

#define FIELD_VALUE(name) _obj->name
#define ANYCODE -1
// todo: only the name, not the ref
#define FIELD_HANDLE(name, handle_code, dxf)    \
    PREFIX if (_obj->name) { \
    fprintf(dat->fh, "\"%s\": \"HANDLE(%d.%d.%lu) absolute:%lu\",\n", #name, \
           _obj->name->handleref.code,                     \
           _obj->name->handleref.size,                     \
           _obj->name->handleref.value,                    \
           _obj->name->absolute_ref);                      \
  }
#define FIELD_DATAHANDLE(name, code, dxf) FIELD_HANDLE(name, code, dxf)
#define FIELD_HANDLE_N(name, vcount, handle_code, dxf) \
    PREFIX if (_obj->name) { \
    fprintf(dat->fh, "\"HANDLE(%d.%d.%lu) absolute:%lu\",\n",\
           _obj->name->handleref.code,                     \
           _obj->name->handleref.size,                     \
           _obj->name->handleref.value,                    \
           _obj->name->absolute_ref);                      \
  } else {\
    fprintf(dat->fh, "\"\",\n"); \
  }

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
#define FIELD_TF(name,len,dxf)  FIELD_TEXT(name, _obj->name)
#define FIELD_TFF(name,len,dxf) FIELD_TEXT(name, _obj->name)
#define FIELD_TV(name,dxf)      FIELD_TEXT(name, _obj->name)
#define FIELD_TU(name,dxf)      FIELD_TEXT_TU(name, (BITCODE_TU)_obj->name)
#define FIELD_T(name,dxf) \
  { if (dat->version >= R_2007) { FIELD_TU(name, dxf); } \
    else                        { FIELD_TV(name, dxf); } }
#define FIELD_BT(name,dxf)    FIELD(name, BT, dxf);
#define FIELD_4BITS(name,dxf) FIELD(name,4BITS,dxf)
#define FIELD_BE(name,dxf)    FIELD_3RD(name,dxf)
#define FIELD_DD(name, _default, dxf) \
    PREFIX fprintf(dat->fh, "\"" #name "\": " FORMAT_DD ", default: " FORMAT_DD ",\n", \
            _obj->name, _default)
#define FIELD_2DD(name, d1, d2, dxf) { \
    FIELD_DD(name.x, d1, dxf); \
    FIELD_DD(name.y, d2, dxf+10); }
#define FIELD_3DD(name, def, dxf) { \
    FIELD_DD(name.x, FIELD_VALUE(def.x), dxf); \
    FIELD_DD(name.y, FIELD_VALUE(def.y), dxf+10); \
    FIELD_DD(name.z, FIELD_VALUE(def.z), dxf+20); }
#define FIELD_2RD(name,dxf) {FIELD(name.x, RD, dxf); FIELD(name.y, RD, dxf+10);}
#define FIELD_2BD(name,dxf) {FIELD(name.x, BD, dxf); FIELD(name.y, BD, dxf+10);}
#define FIELD_2BD_1(name,dxf) {FIELD(name.x, BD, dxf); FIELD(name.y, BD, dxf+1);}
#define FIELD_3RD(name,dxf) {FIELD(name.x, RD, dxf); FIELD(name.y, RD, dxf+10); \
    FIELD(name.z, RD, dxf+20);}
#define FIELD_3BD(name,dxf) {FIELD(name.x, BD, dxf); FIELD(name.y, BD, dxf+10); \
    FIELD(name.z, BD, dxf+20);}
#define FIELD_3BD_1(name,dxf) {FIELD(name.x, BD, dxf); FIELD(name.y, BD, dxf+1); \
    FIELD(name.z, BD, dxf+2);}
#define FIELD_3DPOINT(name,dxf) FIELD_3BD(name,dxf)
#define FIELD_CMC(name,dxf)\
    PREFIX fprintf(dat->fh, "\"" #name "\": %d,\n", _obj->name.index)
#define FIELD_TIMEBLL(name,dxf) \
    PREFIX fprintf(dat->fh, "\"" #name "\": " FORMAT_BL "." FORMAT_BL ",\n", \
            _obj->name.days, _obj->name.ms)

//FIELD_VECTOR_N(name, type, size):
// reads data of the type indicated by 'type' 'size' times and stores
// it all in the vector called 'name'.
#define FIELD_VECTOR_N(name, type, size, dxf)\
    ARRAY; \
    for (vcount=0; vcount < (int)size; vcount++)\
      {\
        PREFIX fprintf(dat->fh, "\"" #name "\": " FORMAT_##type ",\n", _obj->name[vcount]); \
      }\
    ENDARRAY;
#define FIELD_VECTOR_T(name, size, dxf)\
    ARRAY; \
    PRE (R_2007) { \
      for (vcount=0; vcount < (int)_obj->size; vcount++) { \
        PREFIX fprintf(dat->fh, "\"" #name "\": \"%s\",\n", _obj->name[vcount]); \
      }\
    } else { \
      for (vcount=0; vcount < (int)_obj->size; vcount++)\
        FIELD_TEXT_TU(name, _obj->name[vcount]); \
    } \
    ENDARRAY;

#define FIELD_VECTOR(name, type, size, dxf) FIELD_VECTOR_N(name, type, _obj->size, dxf)

#define FIELD_2RD_VECTOR(name, size, dxf)\
  ARRAY;\
  for (vcount=0; vcount < (int)_obj->size; vcount++)\
    {\
      FIELD_2RD(name[vcount], dxf);\
    }\
  ENDARRAY;

#define FIELD_2DD_VECTOR(name, size, dxf)\
  ARRAY;\
  FIELD_2RD(name[0], 0);\
  for (vcount = 1; vcount < (int)_obj->size; vcount++)\
    {\
      FIELD_2DD(name[vcount], FIELD_VALUE(name[vcount - 1].x), FIELD_VALUE(name[vcount - 1].y), dxf);\
    }\
  ENDARRAY;

#define FIELD_3DPOINT_VECTOR(name, size, dxf)\
  ARRAY;\
  for (vcount=0; vcount < (int)_obj->size; vcount++)\
    {\
      FIELD_3DPOINT(name[vcount], dxf);\
    }\
  ENDARRAY;

#define HANDLE_VECTOR_N(name, size, code, dxf) \
  ARRAY;\
  for (vcount=0; vcount < (int)size; vcount++)\
    {\
      FIELD_HANDLE_N(name[vcount], vcount, code, dxf);\
    }\
  ENDARRAY;

#define HANDLE_VECTOR(name, sizefield, code, dxf) \
  HANDLE_VECTOR_N(name, FIELD_VALUE(sizefield), code, dxf)

#define FIELD_INSERT_COUNT(insert_count, type, dxf) \
  FIELD(insert_count, type, dxf)

#define FIELD_XDATA(name, size)

#define REACTORS(code)\
  PREFIX; \
  fprintf(dat->fh, "\"reactors\":"); ARRAY; \
  for (vcount=0; vcount < (int)obj->tio.object->num_reactors; vcount++)\
    {\
      FIELD_HANDLE_N(reactors[vcount], vcount, code, dxf);\
    }\
  ENDARRAY;

#define XDICOBJHANDLE(code)\
  SINCE(R_2004)\
    {\
      if (!obj->tio.object->xdic_missing_flag)\
        FIELD_HANDLE(xdicobjhandle, code, dxf);\
    }\
  PRIOR_VERSIONS\
    {\
      FIELD_HANDLE(xdicobjhandle, code, dxf);\
    }

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

#define COMMON_ENTITY_HANDLE_DATA
#define SECTION_STRING_STREAM
#define START_STRING_STREAM
#define END_STRING_STREAM
#define START_HANDLE_STREAM

#define DWG_ENTITY(token) \
static void \
dwg_json_##token (Bit_Chain *dat, Dwg_Object * obj) \
{\
  int vcount, rcount, rcount2, rcount3, rcount4; \
  Dwg_Entity_##token *ent, *_obj;\
  Dwg_Object_Entity *_ent;\
  LOG_INFO("Entity " #token ":\n")\
  _ent = obj->tio.entity;\
  _obj = ent = _ent->tio.token;

#define DWG_ENTITY_END }

#define DWG_OBJECT(token) \
static void \
dwg_json_ ##token (Bit_Chain *dat, Dwg_Object * obj) \
{ \
  int vcount, rcount, rcount2, rcount3, rcount4;\
  Bit_Chain *hdl_dat = dat;\
  Dwg_Object_##token *_obj;\
  LOG_INFO("Object " #token ":\n")\
  _obj = obj->tio.object->tio.token;

#define DWG_OBJECT_END }

#include "dwg.spec"

/* returns 1 if object could be printd and 0 otherwise
 */
static int
dwg_json_variable_type(Dwg_Data * dwg, Bit_Chain *dat, Dwg_Object* obj)
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
      dwg_json_DICTIONARYWDLFT(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "DICTIONARYVAR"))
    {
      assert(!is_entity);
      dwg_json_DICTIONARYVAR(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "HATCH"))
    {
      assert(!is_entity);
      dwg_json_HATCH(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "FIELDLIST"))
    {
      UNTESTED_CLASS;
      assert(!is_entity);
      dwg_json_FIELDLIST(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "GROUP"))
    {
      UNTESTED_CLASS;
      assert(!is_entity);
      dwg_json_GROUP(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "IDBUFFER"))
    {
      dwg_json_IDBUFFER(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "IMAGE"))
    {
      dwg_json_IMAGE(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "IMAGEDEF"))
    {
      dwg_json_IMAGEDEF(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "IMAGEDEF_REACTOR"))
    {
      dwg_json_IMAGEDEF_REACTOR(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "LAYER_INDEX"))
    {
      dwg_json_LAYER_INDEX(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "LAYOUT"))
    {
      dwg_json_LAYOUT(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "LWPLINE"))
    {
      dwg_json_LWPLINE(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "MULTILEADER"))
    {
#ifdef DEBUG_MULTILEADER
      UNTESTED_CLASS; //broken Leader_Line's/Points
      dwg_json_MULTILEADER(dat, obj);
      return 1;
#else
      UNHANDLED_CLASS;
      return 0;
#endif
    }
  if (!strcmp(dxfname, "MLEADERSTYLE"))
    {
      dwg_json_MLEADERSTYLE(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "OLE2FRAME"))
    {
      dwg_json_OLE2FRAME(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "OBJECTCONTEXTDATA")
      || strcmp(klass->cppname, "AcDbObjectContextData"))
    {
      dwg_json_OBJECTCONTEXTDATA(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "ACDBPLACEHOLDER"))
    {
      dwg_json_PLACEHOLDER(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "PROXY"))
    {
      dwg_json_PROXY(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "RASTERVARIABLES"))
    {
      dwg_json_RASTERVARIABLES(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "SCALE"))
    {
      dwg_json_SCALE(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "SORTENTSTABLE"))
    {
      dwg_json_SORTENTSTABLE(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "SPATIAL_FILTER"))
    {
      dwg_json_SPATIAL_FILTER(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "SPATIAL_INDEX"))
    {
      dwg_json_SPATIAL_INDEX(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "TABLE"))
    {
      UNTESTED_CLASS;
      dwg_json_TABLE(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "WIPEOUTVARIABLE"))
    {
      UNTESTED_CLASS;
      dwg_json_WIPEOUTVARIABLE(dat, obj);
      return 0;
    }
  if (!strcmp(dxfname, "WIPEOUT"))
    {
      dwg_json_WIPEOUT(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "FIELDLIST"))
    {
      UNTESTED_CLASS;
      dwg_json_FIELDLIST(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "VBA_PROJECT"))
    {
#ifdef DEBUG_VBA_PROJECT
      UNTESTED_CLASS;
      dwg_json_VBA_PROJECT(dat, obj);
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
      dwg_json_CELLSTYLEMAP(dat, obj);
      return 1;
#else
      UNHANDLED_CLASS;
      return 0;
#endif
    }
  if (!strcmp(dxfname, "VISUALSTYLE"))
    {
      dwg_json_VISUALSTYLE(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "AcDbField")) //?
    {
      UNTESTED_CLASS;
      dwg_json_FIELD(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "TABLECONTENT"))
    {
      UNTESTED_CLASS;
      dwg_json_TABLECONTENT(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "TABLEGEOMETRY"))
    {
      UNTESTED_CLASS;
      dwg_json_TABLEGEOMETRY(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "GEODATA"))
    {
      UNTESTED_CLASS;
      dwg_json_GEODATA(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "XRECORD"))
    {
      dwg_json_XRECORD(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "ARCALIGNEDTEXT"))
    {
      UNHANDLED_CLASS;
      //assert(!is_entity);
      //dwg_json_ARCALIGNEDTEXT(dat, obj);
      return 0;
    }
  if (!strcmp(dxfname, "DIMASSOC"))
    {
      UNHANDLED_CLASS;
      assert(!is_entity);
      //dwg_json_DIMASSOC(dat, obj);
      return 0;
    }
  if (!strcmp(dxfname, "MATERIAL"))
    {
      UNHANDLED_CLASS;
      assert(!is_entity);
      //dwg_json_MATERIAL(dat, obj);
      return 0;
    }
  if (!strcmp(dxfname, "TABLESTYLE"))
    {
      UNHANDLED_CLASS;
      assert(!is_entity);
      //dwg_json_TABLESTYLE(dat, obj);
      return 0;
    }
  if (!strcmp(dxfname, "DBCOLOR"))
    {
      UNHANDLED_CLASS;
      assert(!is_entity);
      //dwg_json_DBCOLOR(dat, obj);
      return 0;
    }
  if (!strcmp(dxfname, "ACDBSECTIONVIEWSTYLE"))
    {
      UNHANDLED_CLASS;
      assert(!is_entity);
      //dwg_json_SECTIONVIEWSTYLE(dat, obj);
      return 0;
    }
  if (!strcmp(dxfname, "ACDBDETAILVIEWSTYLE"))
    {
      UNHANDLED_CLASS;
      assert(!is_entity);
      //dwg_json_DETAILVIEWSTYLE(dat, obj);
      return 0;
    }
  if (!strcmp(dxfname, "ACDBASSOCNETWORK"))
    {
      UNHANDLED_CLASS;
      assert(!is_entity);
      //dwg_json_ASSOCNETWORK(dat, obj);
      return 0;
    }
  if (!strcmp(dxfname, "ACDBASSOC2DCONSTRAINTGROUP"))
    {
      UNHANDLED_CLASS;
      assert(!is_entity);
      //dwg_json_ASSOC2DCONSTRAINTGROUP(dat, obj);
      return 0;
    }
  if (!strcmp(dxfname, "ACDBASSOCGEOMDEPENDENCY"))
    {
      UNHANDLED_CLASS;
      assert(!is_entity);
      //dwg_json_ASSOCGEOMDEPENDENCY(dat, obj);
      return 0;
    }
  if (!strcmp(dxfname, "ACDB_LEADEROBJECTCONTEXTDATA_CLASS"))
    {
      //UNHANDLED_CLASS;
      //dwg_json_LEADEROBJECTCONTEXTDATA(dat, obj);
      return 0;
    }

  return 0;
}

static void
dwg_json_object(Bit_Chain *dat, Dwg_Object *obj)
{
  switch (obj->type)
    {
    case DWG_TYPE_TEXT:
      dwg_json_TEXT(dat, obj);
      break;
    case DWG_TYPE_ATTRIB:
      dwg_json_ATTRIB(dat, obj);
      break;
    case DWG_TYPE_ATTDEF:
      dwg_json_ATTDEF(dat, obj);
      break;
    case DWG_TYPE_BLOCK:
      dwg_json_BLOCK(dat, obj);
      break;
    case DWG_TYPE_ENDBLK:
      dwg_json_ENDBLK(dat, obj);
      break;
    case DWG_TYPE_SEQEND:
      dwg_json_SEQEND(dat, obj);
      break;
    case DWG_TYPE_INSERT:
      dwg_json_INSERT(dat, obj);
      break;
    case DWG_TYPE_MINSERT:
      dwg_json_MINSERT(dat, obj);
      break;
    case DWG_TYPE_VERTEX_2D:
      dwg_json_VERTEX_2D(dat, obj);
      break;
    case DWG_TYPE_VERTEX_3D:
      dwg_json_VERTEX_3D(dat, obj);
      break;
    case DWG_TYPE_VERTEX_MESH:
      dwg_json_VERTEX_MESH(dat, obj);
      break;
    case DWG_TYPE_VERTEX_PFACE:
      dwg_json_VERTEX_PFACE(dat, obj);
      break;
    case DWG_TYPE_VERTEX_PFACE_FACE:
      dwg_json_VERTEX_PFACE_FACE(dat, obj);
      break;
    case DWG_TYPE_POLYLINE_2D:
      dwg_json_POLYLINE_2D(dat, obj);
      break;
    case DWG_TYPE_POLYLINE_3D:
      dwg_json_POLYLINE_3D(dat, obj);
      break;
    case DWG_TYPE_ARC:
      dwg_json_ARC(dat, obj);
      break;
    case DWG_TYPE_CIRCLE:
      dwg_json_CIRCLE(dat, obj);
      break;
    case DWG_TYPE_LINE:
      dwg_json_LINE(dat, obj);
      break;
    case DWG_TYPE_DIMENSION_ORDINATE:
      dwg_json_DIMENSION_ORDINATE(dat, obj);
      break;
    case DWG_TYPE_DIMENSION_LINEAR:
      dwg_json_DIMENSION_LINEAR(dat, obj);
      break;
    case DWG_TYPE_DIMENSION_ALIGNED:
      dwg_json_DIMENSION_ALIGNED(dat, obj);
      break;
    case DWG_TYPE_DIMENSION_ANG3PT:
      dwg_json_DIMENSION_ANG3PT(dat, obj);
      break;
    case DWG_TYPE_DIMENSION_ANG2LN:
      dwg_json_DIMENSION_ANG2LN(dat, obj);
      break;
    case DWG_TYPE_DIMENSION_RADIUS:
      dwg_json_DIMENSION_RADIUS(dat, obj);
      break;
    case DWG_TYPE_DIMENSION_DIAMETER:
      dwg_json_DIMENSION_DIAMETER(dat, obj);
      break;
    case DWG_TYPE_POINT:
      dwg_json_POINT(dat, obj);
      break;
    case DWG_TYPE__3DFACE:
      dwg_json__3DFACE(dat, obj);
      break;
    case DWG_TYPE_POLYLINE_PFACE:
      dwg_json_POLYLINE_PFACE(dat, obj);
      break;
    case DWG_TYPE_POLYLINE_MESH:
      dwg_json_POLYLINE_MESH(dat, obj);
      break;
    case DWG_TYPE_SOLID:
      dwg_json_SOLID(dat, obj);
      break;
    case DWG_TYPE_TRACE:
      dwg_json_TRACE(dat, obj);
      break;
    case DWG_TYPE_SHAPE:
      dwg_json_SHAPE(dat, obj);
      break;
    case DWG_TYPE_VIEWPORT:
      dwg_json_VIEWPORT(dat, obj);
      break;
    case DWG_TYPE_ELLIPSE:
      dwg_json_ELLIPSE(dat, obj);
      break;
    case DWG_TYPE_SPLINE:
      dwg_json_SPLINE(dat, obj);
      break;
    case DWG_TYPE_REGION:
      dwg_json_REGION(dat, obj);
      break;
    case DWG_TYPE_3DSOLID:
      dwg_json__3DSOLID(dat, obj);
      break; /* Check the type of the object
              */
    case DWG_TYPE_BODY:
      dwg_json_BODY(dat, obj);
      break;
    case DWG_TYPE_RAY:
      dwg_json_RAY(dat, obj);
      break;
    case DWG_TYPE_XLINE:
      dwg_json_XLINE(dat, obj);
      break;
    case DWG_TYPE_DICTIONARY:
      dwg_json_DICTIONARY(dat, obj);
      break;
    case DWG_TYPE_MTEXT:
      dwg_json_MTEXT(dat, obj);
      break;
    case DWG_TYPE_LEADER:
      dwg_json_LEADER(dat, obj);
      break;
    case DWG_TYPE_TOLERANCE:
      dwg_json_TOLERANCE(dat, obj);
      break;
    case DWG_TYPE_MLINE:
      dwg_json_MLINE(dat, obj);
      break;
    case DWG_TYPE_BLOCK_CONTROL:
      dwg_json_BLOCK_CONTROL(dat, obj);
      break;
    case DWG_TYPE_BLOCK_HEADER:
      dwg_json_BLOCK_HEADER(dat, obj);
      break;
    case DWG_TYPE_LAYER_CONTROL:
      dwg_json_LAYER_CONTROL(dat, obj);
      break;
    case DWG_TYPE_LAYER:
      dwg_json_LAYER(dat, obj);
      break;
    case DWG_TYPE_SHAPEFILE_CONTROL:
      dwg_json_SHAPEFILE_CONTROL(dat, obj);
      break;
    case DWG_TYPE_SHAPEFILE:
      dwg_json_SHAPEFILE(dat, obj);
      break;
    case DWG_TYPE_LTYPE_CONTROL:
      dwg_json_LTYPE_CONTROL(dat, obj);
      break;
    case DWG_TYPE_LTYPE:
      dwg_json_LTYPE(dat, obj);
      break;
    case DWG_TYPE_VIEW_CONTROL:
      dwg_json_VIEW_CONTROL(dat, obj);
      break;
    case DWG_TYPE_VIEW:
      dwg_json_VIEW(dat, obj);
      break;
    case DWG_TYPE_UCS_CONTROL:
      dwg_json_UCS_CONTROL(dat, obj);
      break;
    case DWG_TYPE_UCS:
      dwg_json_UCS(dat, obj);
      break;
    case DWG_TYPE_VPORT_CONTROL:
      dwg_json_VPORT_CONTROL(dat, obj);
      break;
    case DWG_TYPE_VPORT:
      dwg_json_VPORT(dat, obj);
      break;
    case DWG_TYPE_APPID_CONTROL:
      dwg_json_APPID_CONTROL(dat, obj);
      break;
    case DWG_TYPE_APPID:
      dwg_json_APPID(dat, obj);
      break;
    case DWG_TYPE_DIMSTYLE_CONTROL:
      dwg_json_DIMSTYLE_CONTROL(dat, obj);
      break;
    case DWG_TYPE_DIMSTYLE:
      dwg_json_DIMSTYLE(dat, obj);
      break;
    case DWG_TYPE_VP_ENT_HDR_CONTROL:
      dwg_json_VP_ENT_HDR_CONTROL(dat, obj);
      break;
    case DWG_TYPE_VP_ENT_HDR:
      dwg_json_VP_ENT_HDR(dat, obj);
      break;
    case DWG_TYPE_GROUP:
      dwg_json_GROUP(dat, obj);
      break;
    case DWG_TYPE_MLINESTYLE:
      dwg_json_MLINESTYLE(dat, obj);
      break;
    case DWG_TYPE_OLE2FRAME:
      dwg_json_OLE2FRAME(dat, obj);
      break;
    case DWG_TYPE_DUMMY:
      dwg_json_DUMMY(dat, obj);
      break;
    case DWG_TYPE_LONG_TRANSACTION:
      dwg_json_LONG_TRANSACTION(dat, obj);
      break;
    case DWG_TYPE_LWPLINE:
      dwg_json_LWPLINE(dat, obj);
      break;
    case DWG_TYPE_HATCH:
      dwg_json_HATCH(dat, obj);
      break;
    case DWG_TYPE_XRECORD:
      dwg_json_XRECORD(dat, obj);
      break;
    case DWG_TYPE_PLACEHOLDER:
      dwg_json_PLACEHOLDER(dat, obj);
      break;
    case DWG_TYPE_PROXY_ENTITY:
      dwg_json_PROXY_ENTITY(dat, obj);
      break;
    case DWG_TYPE_OLEFRAME:
      dwg_json_OLEFRAME(dat, obj);
      break;
    case DWG_TYPE_VBA_PROJECT:
      LOG_ERROR("Unhandled Object VBA_PROJECT. Has its own section\n");
      //dwg_json_VBA_PROJECT(dat, obj);
      break;
    case DWG_TYPE_LAYOUT:
      dwg_json_LAYOUT(dat, obj);
      break;
    default:
      if (obj->type == obj->parent->layout_number)
        {
          dwg_json_LAYOUT(dat, obj);
        }
      /* > 500:
         TABLE, DICTIONARYWDLFT, IDBUFFER, IMAGE, IMAGEDEF, IMAGEDEF_REACTOR,
         LAYER_INDEX, OLE2FRAME, PROXY, RASTERVARIABLES, SORTENTSTABLE, SPATIAL_FILTER,
         SPATIAL_INDEX
      */
      else if (!dwg_json_variable_type(obj->parent, dat, obj))
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
              dwg_json_UNKNOWN_OBJ(dat, obj);
            }
          else if (klass)
            {
              dwg_json_UNKNOWN_ENT(dat, obj);
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

/*
static void
json_common_entity_handle_data(Bit_Chain *dat, Dwg_Object* obj)
{
  (void)dat; (void)obj;
}
*/

static void
json_header_write(Bit_Chain *dat, Dwg_Data* dwg)
{
  Dwg_Header_Variables* _obj = &dwg->header_vars;
  Dwg_Object* obj = NULL;
  const int minimal = dwg->opts & 0x10;
  char buf[4096];
  double ms;
  const char* codepage =
    (dwg->header.codepage == 30 || dwg->header.codepage == 0)
    ? "ANSI_1252"
    : (dwg->header.version >= R_2007)
      ? "UTF-8"
      : "ANSI_1252";

  SECTION(HEADER);
  #include "header_variables.spec"
  ENDSEC();
}

static int
json_classes_write (Bit_Chain *dat, Dwg_Data * dwg)
{
  unsigned int i;

  SECTION(CLASSES);
  for (i=0; i < dwg->num_classes; i++)
    {
      Dwg_Class *_obj = &dwg->dwg_class[i];
      HASH;
      FIELD_BS (number, 0);
      FIELD_T (dxfname, 1);
      FIELD_T (cppname, 2);
      FIELD_T (appname, 3);
      FIELD_BS (proxyflag, 90);
      FIELD_BL (instance_count, 91);
      FIELD_B  (wasazombie, 280);
      FIELD_BS (item_class_id, 281);
      // Is-an-entity. 1f2 for entities, 1f3 for objects
      //VALUE (281, dwg->dwg_class[i].item_class_id == 0x1F2 ? 1 : 0);
      ENDHASH;
    }
  ENDSEC();
  return 0;
}

static int
json_tables_write (Bit_Chain *dat, Dwg_Data * dwg)
{
  (void)dwg;

  SECTION(TABLES);
  //...
  ENDSEC();
  return 0;
}

static int
json_blocks_write (Bit_Chain *dat, Dwg_Data * dwg)
{
  (void)dwg;

  SECTION(BLOCKS);
  //...
  ENDSEC();
  return 0;
}

static int
json_entities_write (Bit_Chain *dat, Dwg_Data * dwg)
{
  unsigned int i;

  SECTION(ENTITIES);
  for (i=0; i < dwg->num_objects; i++)
    {
      Dwg_Object *obj = &dwg->object[i];
      HASH;
      dwg_json_object(dat, obj);
      ENDHASH;
    }
  ENDSEC();
  return 0;
}

/* The object map: we skip this
static int
json_objects_write (Bit_Chain *dat, Dwg_Data * dwg)
{
  unsigned int i;

  SECTION(OBJECTS);
  for (j = 0; j < dwg->num_objects; j++)
    {
    }
  ENDSEC();
  return 0;
}
*/

static int
json_preview_write (Bit_Chain *dat, Dwg_Data * dwg)
{
  (void)dat; (void)dwg;
  //...
  return 0;
}

int
dwg_write_json(Bit_Chain *dat, Dwg_Data * dwg)
{
  const int minimal = dwg->opts & 0x10;
  struct Dwg_Header *obj = &dwg->header;

  fprintf (dat->fh, "{\n"
                     "  \"created_by\": \"%s\",\n", PACKAGE_STRING);
  dat->bit++;
  // a minimal header requires only $ACADVER, $HANDSEED, and then ENTITIES
  // see https://pythonhosted.org/ezdxf/dxfinternals/filestructure.html
  json_header_write (dat, dwg);

  if (!minimal && dat->version >= R_13)
    {
      SINCE(R_2000) {
        if (json_classes_write (dat, dwg))
          goto fail;
      }

      if (json_tables_write (dat, dwg))
        goto fail;
      
      if (json_blocks_write (dat, dwg))
        goto fail;
    }

  if (json_entities_write (dat, dwg))
    goto fail;

  /* only the object map
  SINCE(R_13) {
    if (json_objects_write (dat, dwg))
      goto fail;
  }*/

  if (!minimal && dat->version >= R_2000) {
    if (json_preview_write (dat, dwg))
      goto fail;
  }

  dat->bit--;
  fprintf (dat->fh, "}\n");
  return 0;
 fail:
  return 1;
}

#undef IS_PRINT
