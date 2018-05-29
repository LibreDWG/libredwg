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
 * out_svg.c: write as SVG
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
#include "out_svg.h"

#define DWG_LOGLEVEL DWG_LOGLEVEL_TRACE
#include "logging.h"

/* the current version per spec block */
static unsigned int cur_ver = 0;
static char buf[4096];

extern void
obj_string_stream(Bit_Chain *dat, BITCODE_RL bitsize, Bit_Chain *str);

/*--------------------------------------------------------------------------------
 * MACROS
 */

#define IS_PRINT

#define FIELD(name,type,svg) \
    fprintf(dat->fh, #name ": " FORMAT_##type ",\n", _obj->name)
#define FIELD_CAST(name,type,cast,svg) FIELD(name,cast,svg)
#define FIELD_TRACE(name,type)
#define FIELD_TEXT(name,str) \
    fprintf(dat->fh, #name ": \"%s\",\n", str)
#ifdef HAVE_NATIVE_WCHAR2
# define FIELD_TEXT_TU(name,wstr) \
    fprintf(dat->fh, #name ": \"%ls\",\n", wstr)
#else
# define FIELD_TEXT_TU(name,wstr) \
  { \
    BITCODE_TU ws = (BITCODE_TU)wstr; \
    uint16_t _c; \
    fprintf(dat->fh, #name ": \""); \
    while ((_c = *ws++)) { \
      fprintf(dat->fh, "%c", (char)(_c & 0xff)); \
    } \
    fprintf(dat->fh, "\",\n"); \
  }
#endif
#define FIELD_VALUE(name) _obj->name
#define ANYCODE -1
#define FIELD_HANDLE(name, handle_code, svg) \
  if (_obj->name) { \
    fprintf(dat->fh, #name ": \"HANDLE(%d.%d.%lu) absolute:%lu\",\n",\
           _obj->name->handleref.code,                     \
           _obj->name->handleref.size,                     \
           _obj->name->handleref.value,                    \
           _obj->name->absolute_ref);                      \
  }

#define VAR(name, code, value)                  \
  {\
    fprintf (dat->fh, "  9\n$" #name "\n%3i\n", code);      \
    GCC_DIAG_IGNORE(-Wformat-nonliteral) \
    snprintf (buf, 4096, "%s\n", svg_format (code));\
    fprintf (dat->fh, buf, value);\
    GCC_DIAG_RESTORE \
  }
#define HEADER_VALUE(name, code)\
  {\
    fprintf (dat->fh, "  9\n$" #name "\n%3i\n", code);\
    GCC_DIAG_IGNORE(-Wformat-nonliteral) \
    snprintf (buf, 4096, "%s\n", svg_format (code));\
    fprintf (dat->fh, buf, dwg->header_vars.name);\
    GCC_DIAG_RESTORE \
  }
#define POINT_3D(name, var, c1, c2, c3)\
  {\
    fprintf (dat->fh, "  9\n$" #name "\n");\
    fprintf (dat->fh, "%3i\n%-16.15g\n", c1, dwg->var.x);\
    fprintf (dat->fh, "%3i\n%-16.15g\n", c2, dwg->var.y);\
    fprintf (dat->fh, "%3i\n%-16.15g\n", c3, dwg->var.z);\
  }
#define POINT_2D(name, var, c1, c2) \
  {\
    fprintf (dat->fh, "  9\n$" #name "\n");\
    fprintf (dat->fh, "%3i\n%-16.15g\n", c1, dwg->var.x);\
    fprintf (dat->fh, "%3i\n%-16.15g\n", c2, dwg->var.y);\
  }
#define HEADER_3D(name)\
  POINT_3D (name, header_vars.name, 10, 20, 30);
#define HEADER_2D(name)\
  POINT_2D (name, header_vars.name, 10, 20);
#define SECTION(section) fprintf(dat->fh, "  0\nSECTION\n  2\n" #section "\n")
#define ENDSEC()         fprintf(dat->fh, "  0\nENDSEC\n")
#define RECORD(record)   fprintf(dat->fh, "  0\n" #record "\n")
#define VALUE(code, value) \
  {\
    snprintf (buf, 4096, "%3i\n%s\n", code, svg_format (code));\
    GCC_DIAG_IGNORE(-Wformat-nonliteral) \
    fprintf(dat->fh, buf, value);\
    GCC_DIAG_RESTORE \
  }
#define HANDLE_NAME(name, code, section) \
  {\
    Dwg_Object_Ref *ref = dwg->header_vars.name;\
    GCC_DIAG_IGNORE(-Wformat-nonliteral) \
    snprintf (buf, 4096, "  9\n$" #name "\n%3i\n%s\n", code, svg_format (code));\
    /*if (ref && !ref->obj) ref->obj = dwg_resolve_handle(dwg, ref->absolute_ref); */ \
    fprintf(dat->fh, buf, ref && ref->obj \
      ? ref->obj->tio.object->tio.section->entry_name : ""); \
    GCC_DIAG_RESTORE \
  }

#define FIELD_DATAHANDLE(name, code, svg) FIELD_HANDLE(name, code, svg)
#define FIELD_HANDLE_N(name, vcount, handle_code, svg) FIELD_HANDLE(name, handle_code, svg)

#define FIELD_B(name,svg)   FIELD(name, B, svg)
#define FIELD_BB(name,svg)  FIELD(name, BB, svg)
#define FIELD_3B(name,svg)  FIELD(name, 3B, svg)
#define FIELD_BS(name,svg)  FIELD(name, BS, svg)
#define FIELD_BL(name,svg)  FIELD(name, BL, svg)
#define FIELD_BLL(name,svg) FIELD(name, BLL, svg)
#define FIELD_BD(name,svg)  FIELD(name, BD, svg)
#define FIELD_RC(name,svg)  FIELD(name, RC, svg)
#define FIELD_RS(name,svg)  FIELD(name, RS, svg)
#define FIELD_RD(name,svg)  FIELD(name, RD, svg)
#define FIELD_RL(name,svg)  FIELD(name, RL, svg)
#define FIELD_RLL(name,svg) FIELD(name, RLL, svg)
#define FIELD_MC(name,svg)  FIELD(name, MC, svg)
#define FIELD_MS(name,svg)  FIELD(name, MS, svg)
#define FIELD_TF(name,len,svg)  FIELD_TEXT(name, _obj->name)
#define FIELD_TFF(name,len,svg) FIELD_TEXT(name, _obj->name)
#define FIELD_TV(name,svg)      FIELD_TEXT(name, _obj->name)
#define FIELD_TU(name,svg)      FIELD_TEXT_TU(name, (BITCODE_TU)_obj->name)
#define FIELD_T(name,svg) \
  { if (dat->version >= R_2007) { FIELD_TU(name, svg); } \
    else                        { FIELD_TV(name, svg); } }
#define FIELD_BT(name,svg) FIELD(name, BT, svg);
#define FIELD_4BITS(name,svg) FIELD(name,4BITS,svg)
#define FIELD_BE(name,svg) FIELD_3RD(name,svg)
#define FIELD_DD(name, _default, svg) \
    fprintf(dat->fh, #name ": " FORMAT_DD ", default: " FORMAT_DD ",\n", _obj->name, _default)
#define FIELD_2DD(name, d1, d2, svg) { FIELD_DD(name.x, d1, svg); FIELD_DD(name.y, d2, svg+10); }
#define FIELD_3DD(name, def, svg) { \
    FIELD_DD(name.x, FIELD_VALUE(def.x), svg); \
    FIELD_DD(name.y, FIELD_VALUE(def.y), svg+10); \
    FIELD_DD(name.z, FIELD_VALUE(def.z), svg+20); }
#define FIELD_2RD(name,svg) {FIELD(name.x, RD, svg); FIELD(name.y, RD, svg+10);}
#define FIELD_2BD(name,svg) {FIELD(name.x, BD, svg); FIELD(name.y, BD, svg+10);}
#define FIELD_2BD_1(name,svg) {FIELD(name.x, BD, svg); FIELD(name.y, BD, svg+1);}
#define FIELD_3RD(name,svg) {FIELD(name.x, RD, svg); FIELD(name.y, RD, svg+10); FIELD(name.z, RD, svg+20);}
#define FIELD_3BD(name,svg) {FIELD(name.x, BD, svg); FIELD(name.y, BD, svg+10); FIELD(name.z, BD, svg+20);}
#define FIELD_3BD_1(name,svg) {FIELD(name.x, BD, svg); FIELD(name.y, BD, svg+1); FIELD(name.z, BD, svg+2);}
#define FIELD_3DPOINT(name,svg) FIELD_3BD(name,svg)
#define FIELD_CMC(name,svg)\
    fprintf(dat->fh, #name ": %d,\n", _obj->name.index)
#define FIELD_TIMEBLL(name,svg) \
    fprintf(dat->fh, #name ": " FORMAT_BL "." FORMAT_BL ",\n", _obj->name.days, _obj->name.ms)

//FIELD_VECTOR_N(name, type, size):
// reads data of the type indicated by 'type' 'size' times and stores
// it all in the vector called 'name'.
#define FIELD_VECTOR_N(name, type, size, svg)\
    fprintf(dat->fh, "["); \
    for (vcount=0; vcount < (int)size; vcount++)\
      {\
        fprintf(dat->fh, #name " " FORMAT_##type ",\n", _obj->name[vcount]); \
      }\
    fprintf(dat->fh, "]\n")
#define FIELD_VECTOR_T(name, size, svg)\
    fprintf(dat->fh, "["); \
    PRE (R_2007) { \
      for (vcount=0; vcount < (int)_obj->size; vcount++)\
        fprintf(dat->fh, #name ": \"%s\",\n", _obj->name[vcount]); \
    } else { \
      for (vcount=0; vcount < (int)_obj->size; vcount++) \
        FIELD_TEXT_TU(name, _obj->name[vcount]);\
    }\
    fprintf(dat->fh, "]\n")

#define FIELD_VECTOR(name, type, size, svg) FIELD_VECTOR_N(name, type, _obj->size, svg)

#define FIELD_2RD_VECTOR(name, size, svg)\
  fprintf(dat->fh, "["); \
  for (vcount=0; vcount < (int)_obj->size; vcount++)\
    {\
      FIELD_2RD(name[vcount], svg);\
    }\
  fprintf(dat->fh, "]\n");

#define FIELD_2DD_VECTOR(name, size, svg)\
  fprintf(dat->fh, "["); \
  FIELD_2RD(name[0], 0);\
  for (vcount = 1; vcount < (int)_obj->size; vcount++)\
    {\
      FIELD_2DD(name[vcount], FIELD_VALUE(name[vcount - 1].x), FIELD_VALUE(name[vcount - 1].y), svg);\
    }\
  fprintf(dat->fh, "]\n");

#define FIELD_3DPOINT_VECTOR(name, size, svg)\
  fprintf(dat->fh, "["); \
  for (vcount=0; vcount < (int)_obj->size; vcount++)\
    {\
      FIELD_3DPOINT(name[vcount], svg);\
    }\
  fprintf(dat->fh, "]\n");

#define HANDLE_VECTOR_N(name, size, code, svg) \
  fprintf(dat->fh, "["); \
  for (vcount=0; vcount < (int)size; vcount++)\
    {\
      FIELD_HANDLE_N(name[vcount], vcount, code, svg);\
    }\
  fprintf(dat->fh, "]\n");

#define HANDLE_VECTOR(name, sizefield, code, svg) \
  HANDLE_VECTOR_N(name, FIELD_VALUE(sizefield), code, svg)

#define FIELD_INSERT_COUNT(insert_count, type, svg) \
  FIELD(insert_count, type, svg)

#define FIELD_XDATA(name, size)

#define REACTORS(code)\
  fprintf(dat->fh, "[");\
  for (vcount=0; vcount < (int)obj->tio.object->num_reactors; vcount++)\
    {\
      FIELD_HANDLE_N(reactors[vcount], vcount, code, svg);\
    }\
  fprintf(dat->fh, "]\n");

#define XDICOBJHANDLE(code)\
  SINCE(R_2004)\
    {\
      if (!obj->tio.object->xdic_missing_flag)\
        FIELD_HANDLE(xdicobjhandle, code, svg);\
    }\
  PRIOR_VERSIONS\
    {\
      FIELD_HANDLE(xdicobjhandle, code, svg);\
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
dwg_svg_##token (Bit_Chain *dat, Dwg_Object * obj) \
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
dwg_svg_ ##token (Bit_Chain *dat, Dwg_Object * obj) \
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
dwg_svg_variable_type(Dwg_Data * dwg, Bit_Chain *dat, Dwg_Object* obj)
{
  int i;
  char *svgname;
  Dwg_Class *klass;
  int is_entity;

  if ((obj->type - 500) > dwg->num_classes)
    return 0;

  i = obj->type - 500;
  klass = &dwg->dwg_class[i];
  svgname = klass->svgname;
  // almost always false
  is_entity = dwg_class_is_entity(klass);

#define UNHANDLED_CLASS \
      LOG_WARN("Unhandled Class %s %d %s (0x%x%s)", is_entity ? "entity" : "object",\
               klass->number, svgname, klass->proxyflag,\
               klass->wasazombie ? " was proxy" : "")
#define UNTESTED_CLASS \
      LOG_WARN("Untested Class %s %d %s (0x%x%s)", is_entity ? "entity" : "object",\
               klass->number, svgname, klass->proxyflag,\
               klass->wasazombie ? " was proxy" : "")
  
  if (!strcmp(svgname, "ACDBDICTIONARYWDFLT"))
    {
      assert(!is_entity);
      dwg_svg_DICTIONARYWDLFT(dat, obj);
      return 1;
    }
  if (!strcmp(svgname, "DICTIONARYVAR"))
    {
      assert(!is_entity);
      dwg_svg_DICTIONARYVAR(dat, obj);
      return 1;
    }
  if (!strcmp(svgname, "HATCH"))
    {
      assert(!is_entity);
      dwg_svg_HATCH(dat, obj);
      return 1;
    }
  if (!strcmp(svgname, "FIELDLIST"))
    {
      UNTESTED_CLASS;
      assert(!is_entity);
      dwg_svg_FIELDLIST(dat, obj);
      return 1;
    }
  if (!strcmp(svgname, "GROUP"))
    {
      UNTESTED_CLASS;
      assert(!is_entity);
      dwg_svg_GROUP(dat, obj);
      return 1;
    }
  if (!strcmp(svgname, "IDBUFFER"))
    {
      dwg_svg_IDBUFFER(dat, obj);
      return 1;
    }
  if (!strcmp(svgname, "IMAGE"))
    {
      dwg_svg_IMAGE(dat, obj);
      return 1;
    }
  if (!strcmp(svgname, "IMAGEDEF"))
    {
      dwg_svg_IMAGEDEF(dat, obj);
      return 1;
    }
  if (!strcmp(svgname, "IMAGEDEF_REACTOR"))
    {
      dwg_svg_IMAGEDEF_REACTOR(dat, obj);
      return 1;
    }
  if (!strcmp(svgname, "LAYER_INDEX"))
    {
      dwg_svg_LAYER_INDEX(dat, obj);
      return 1;
    }
  if (!strcmp(svgname, "LAYOUT"))
    {
      dwg_svg_LAYOUT(dat, obj);
      return 1;
    }
  if (!strcmp(svgname, "LWPLINE"))
    {
      dwg_svg_LWPLINE(dat, obj);
      return 1;
    }
  if (!strcmp(svgname, "MULTILEADER"))
    {
#ifdef DEBUG_MULTILEADER
      UNTESTED_CLASS; //broken Leader_Line's/Points
      dwg_svg_MULTILEADER(dat, obj);
      return 1;
#else
      UNHANDLED_CLASS;
      return 0;
#endif
    }
  if (!strcmp(svgname, "MLEADERSTYLE"))
    {
      dwg_svg_MLEADERSTYLE(dat, obj);
      return 1;
    }
  if (!strcmp(svgname, "OLE2FRAME"))
    {
      dwg_svg_OLE2FRAME(dat, obj);
      return 1;
    }
  if (!strcmp(svgname, "OBJECTCONTEXTDATA")
      || strcmp(klass->cppname, "AcDbObjectContextData"))
    {
      dwg_svg_OBJECTCONTEXTDATA(dat, obj);
      return 1;
    }
  if (!strcmp(svgname, "ACDBPLACEHOLDER"))
    {
      dwg_svg_PLACEHOLDER(dat, obj);
      return 1;
    }
  if (!strcmp(svgname, "PROXY"))
    {
      dwg_svg_PROXY(dat, obj);
      return 1;
    }
  if (!strcmp(svgname, "RASTERVARIABLES"))
    {
      dwg_svg_RASTERVARIABLES(dat, obj);
      return 1;
    }
  if (!strcmp(svgname, "SCALE"))
    {
      dwg_svg_SCALE(dat, obj);
      return 1;
    }
  if (!strcmp(svgname, "SORTENTSTABLE"))
    {
      dwg_svg_SORTENTSTABLE(dat, obj);
      return 1;
    }
  if (!strcmp(svgname, "SPATIAL_FILTER"))
    {
      dwg_svg_SPATIAL_FILTER(dat, obj);
      return 1;
    }
  if (!strcmp(svgname, "SPATIAL_INDEX"))
    {
      dwg_svg_SPATIAL_INDEX(dat, obj);
      return 1;
    }
  if (!strcmp(svgname, "TABLE"))
    {
      UNTESTED_CLASS;
      dwg_svg_TABLE(dat, obj);
      return 1;
    }
  if (!strcmp(svgname, "WIPEOUTVARIABLE"))
    {
      UNTESTED_CLASS;
      dwg_svg_WIPEOUTVARIABLE(dat, obj);
      return 0;
    }
  if (!strcmp(svgname, "WIPEOUT"))
    {
      dwg_svg_WIPEOUT(dat, obj);
      return 1;
    }
  if (!strcmp(svgname, "FIELDLIST"))
    {
      UNTESTED_CLASS;
      dwg_svg_FIELDLIST(dat, obj);
      return 1;
    }
  if (!strcmp(svgname, "VBA_PROJECT"))
    {
#ifdef DEBUG_VBA_PROJECT
      UNTESTED_CLASS;
      dwg_svg_VBA_PROJECT(dat, obj);
      return 1;
#else
      UNHANDLED_CLASS;
      return 0;
#endif
    }
  if (!strcmp(svgname, "CELLSTYLEMAP"))
    {
#ifdef DEBUG_CELLSTYLEMAP
      UNTESTED_CLASS;
      dwg_svg_CELLSTYLEMAP(dat, obj);
      return 1;
#else
      UNHANDLED_CLASS;
      return 0;
#endif
    }
  if (!strcmp(svgname, "VISUALSTYLE"))
    {
      dwg_svg_VISUALSTYLE(dat, obj);
      return 1;
    }
  if (!strcmp(svgname, "AcDbField")) //?
    {
      UNTESTED_CLASS;
      dwg_svg_FIELD(dat, obj);
      return 1;
    }
  if (!strcmp(svgname, "TABLECONTENT"))
    {
      UNTESTED_CLASS;
      dwg_svg_TABLECONTENT(dat, obj);
      return 1;
    }
  if (!strcmp(svgname, "TABLEGEOMETRY"))
    {
      UNTESTED_CLASS;
      dwg_svg_TABLEGEOMETRY(dat, obj);
      return 1;
    }
  if (!strcmp(svgname, "GEODATA"))
    {
      UNTESTED_CLASS;
      dwg_svg_GEODATA(dat, obj);
      return 1;
    }
  if (!strcmp(svgname, "XRECORD"))
    {
      dwg_svg_XRECORD(dat, obj);
      return 1;
    }
  if (!strcmp(svgname, "ARCALIGNEDTEXT"))
    {
      UNHANDLED_CLASS;
      //assert(!is_entity);
      //dwg_svg_ARCALIGNEDTEXT(dat, obj);
      return 0;
    }
  if (!strcmp(svgname, "DIMASSOC"))
    {
      UNHANDLED_CLASS;
      assert(!is_entity);
      //dwg_svg_DIMASSOC(dat, obj);
      return 0;
    }
  if (!strcmp(svgname, "MATERIAL"))
    {
      UNHANDLED_CLASS;
      assert(!is_entity);
      //dwg_svg_MATERIAL(dat, obj);
      return 0;
    }
  if (!strcmp(svgname, "TABLESTYLE"))
    {
      UNHANDLED_CLASS;
      assert(!is_entity);
      //dwg_svg_TABLESTYLE(dat, obj);
      return 0;
    }
  if (!strcmp(svgname, "DBCOLOR"))
    {
      UNHANDLED_CLASS;
      assert(!is_entity);
      //dwg_svg_DBCOLOR(dat, obj);
      return 0;
    }
  if (!strcmp(svgname, "ACDBSECTIONVIEWSTYLE"))
    {
      UNHANDLED_CLASS;
      assert(!is_entity);
      //dwg_svg_SECTIONVIEWSTYLE(dat, obj);
      return 0;
    }
  if (!strcmp(svgname, "ACDBDETAILVIEWSTYLE"))
    {
      UNHANDLED_CLASS;
      assert(!is_entity);
      //dwg_svg_DETAILVIEWSTYLE(dat, obj);
      return 0;
    }
  if (!strcmp(svgname, "ACDBASSOCNETWORK"))
    {
      UNHANDLED_CLASS;
      assert(!is_entity);
      //dwg_svg_ASSOCNETWORK(dat, obj);
      return 0;
    }
  if (!strcmp(svgname, "ACDBASSOC2DCONSTRAINTGROUP"))
    {
      UNHANDLED_CLASS;
      assert(!is_entity);
      //dwg_svg_ASSOC2DCONSTRAINTGROUP(dat, obj);
      return 0;
    }
  if (!strcmp(svgname, "ACDBASSOCGEOMDEPENDENCY"))
    {
      UNHANDLED_CLASS;
      assert(!is_entity);
      //dwg_svg_ASSOCGEOMDEPENDENCY(dat, obj);
      return 0;
    }
  if (!strcmp(svgname, "ACDB_LEADEROBJECTCONTEXTDATA_CLASS"))
    {
      //UNHANDLED_CLASS;
      //dwg_svg_LEADEROBJECTCONTEXTDATA(dat, obj);
      return 0;
    }

  return 0;
}

void
dwg_svg_object(Bit_Chain *dat, Dwg_Object *obj)
{

  switch (obj->type)
    {
    case DWG_TYPE_TEXT:
      dwg_svg_TEXT(dat, obj);
      break;
    case DWG_TYPE_ATTRIB:
      dwg_svg_ATTRIB(dat, obj);
      break;
    case DWG_TYPE_ATTDEF:
      dwg_svg_ATTDEF(dat, obj);
      break;
    case DWG_TYPE_BLOCK:
      dwg_svg_BLOCK(dat, obj);
      break;
    case DWG_TYPE_ENDBLK:
      dwg_svg_ENDBLK(dat, obj);
      break;
    case DWG_TYPE_SEQEND:
      dwg_svg_SEQEND(dat, obj);
      break;
    case DWG_TYPE_INSERT:
      dwg_svg_INSERT(dat, obj);
      break;
    case DWG_TYPE_MINSERT:
      dwg_svg_MINSERT(dat, obj);
      break;
    case DWG_TYPE_VERTEX_2D:
      dwg_svg_VERTEX_2D(dat, obj);
      break;
    case DWG_TYPE_VERTEX_3D:
      dwg_svg_VERTEX_3D(dat, obj);
      break;
    case DWG_TYPE_VERTEX_MESH:
      dwg_svg_VERTEX_MESH(dat, obj);
      break;
    case DWG_TYPE_VERTEX_PFACE:
      dwg_svg_VERTEX_PFACE(dat, obj);
      break;
    case DWG_TYPE_VERTEX_PFACE_FACE:
      dwg_svg_VERTEX_PFACE_FACE(dat, obj);
      break;
    case DWG_TYPE_POLYLINE_2D:
      dwg_svg_POLYLINE_2D(dat, obj);
      break;
    case DWG_TYPE_POLYLINE_3D:
      dwg_svg_POLYLINE_3D(dat, obj);
      break;
    case DWG_TYPE_ARC:
      dwg_svg_ARC(dat, obj);
      break;
    case DWG_TYPE_CIRCLE:
      dwg_svg_CIRCLE(dat, obj);
      break;
    case DWG_TYPE_LINE:
      dwg_svg_LINE(dat, obj);
      break;
    case DWG_TYPE_DIMENSION_ORDINATE:
      dwg_svg_DIMENSION_ORDINATE(dat, obj);
      break;
    case DWG_TYPE_DIMENSION_LINEAR:
      dwg_svg_DIMENSION_LINEAR(dat, obj);
      break;
    case DWG_TYPE_DIMENSION_ALIGNED:
      dwg_svg_DIMENSION_ALIGNED(dat, obj);
      break;
    case DWG_TYPE_DIMENSION_ANG3PT:
      dwg_svg_DIMENSION_ANG3PT(dat, obj);
      break;
    case DWG_TYPE_DIMENSION_ANG2LN:
      dwg_svg_DIMENSION_ANG2LN(dat, obj);
      break;
    case DWG_TYPE_DIMENSION_RADIUS:
      dwg_svg_DIMENSION_RADIUS(dat, obj);
      break;
    case DWG_TYPE_DIMENSION_DIAMETER:
      dwg_svg_DIMENSION_DIAMETER(dat, obj);
      break;
    case DWG_TYPE_POINT:
      dwg_svg_POINT(dat, obj);
      break;
    case DWG_TYPE__3DFACE:
      dwg_svg__3DFACE(dat, obj);
      break;
    case DWG_TYPE_POLYLINE_PFACE:
      dwg_svg_POLYLINE_PFACE(dat, obj);
      break;
    case DWG_TYPE_POLYLINE_MESH:
      dwg_svg_POLYLINE_MESH(dat, obj);
      break;
    case DWG_TYPE_SOLID:
      dwg_svg_SOLID(dat, obj);
      break;
    case DWG_TYPE_TRACE:
      dwg_svg_TRACE(dat, obj);
      break;
    case DWG_TYPE_SHAPE:
      dwg_svg_SHAPE(dat, obj);
      break;
    case DWG_TYPE_VIEWPORT:
      dwg_svg_VIEWPORT(dat, obj);
      break;
    case DWG_TYPE_ELLIPSE:
      dwg_svg_ELLIPSE(dat, obj);
      break;
    case DWG_TYPE_SPLINE:
      dwg_svg_SPLINE(dat, obj);
      break;
    case DWG_TYPE_REGION:
      dwg_svg_REGION(dat, obj);
      break;
    case DWG_TYPE_3DSOLID:
      dwg_svg__3DSOLID(dat, obj);
      break; /* Check the type of the object
              */
    case DWG_TYPE_BODY:
      dwg_svg_BODY(dat, obj);
      break;
    case DWG_TYPE_RAY:
      dwg_svg_RAY(dat, obj);
      break;
    case DWG_TYPE_XLINE:
      dwg_svg_XLINE(dat, obj);
      break;
    case DWG_TYPE_DICTIONARY:
      dwg_svg_DICTIONARY(dat, obj);
      break;
    case DWG_TYPE_MTEXT:
      dwg_svg_MTEXT(dat, obj);
      break;
    case DWG_TYPE_LEADER:
      dwg_svg_LEADER(dat, obj);
      break;
    case DWG_TYPE_TOLERANCE:
      dwg_svg_TOLERANCE(dat, obj);
      break;
    case DWG_TYPE_MLINE:
      dwg_svg_MLINE(dat, obj);
      break;
    case DWG_TYPE_BLOCK_CONTROL:
      dwg_svg_BLOCK_CONTROL(dat, obj);
      break;
    case DWG_TYPE_BLOCK_HEADER:
      dwg_svg_BLOCK_HEADER(dat, obj);
      break;
    case DWG_TYPE_LAYER_CONTROL:
      dwg_svg_LAYER_CONTROL(dat, obj);
      break;
    case DWG_TYPE_LAYER:
      dwg_svg_LAYER(dat, obj);
      break;
    case DWG_TYPE_SHAPEFILE_CONTROL:
      dwg_svg_SHAPEFILE_CONTROL(dat, obj);
      break;
    case DWG_TYPE_SHAPEFILE:
      dwg_svg_SHAPEFILE(dat, obj);
      break;
    case DWG_TYPE_LTYPE_CONTROL:
      dwg_svg_LTYPE_CONTROL(dat, obj);
      break;
    case DWG_TYPE_LTYPE:
      dwg_svg_LTYPE(dat, obj);
      break;
    case DWG_TYPE_VIEW_CONTROL:
      dwg_svg_VIEW_CONTROL(dat, obj);
      break;
    case DWG_TYPE_VIEW:
      dwg_svg_VIEW(dat, obj);
      break;
    case DWG_TYPE_UCS_CONTROL:
      dwg_svg_UCS_CONTROL(dat, obj);
      break;
    case DWG_TYPE_UCS:
      dwg_svg_UCS(dat, obj);
      break;
    case DWG_TYPE_VPORT_CONTROL:
      dwg_svg_VPORT_CONTROL(dat, obj);
      break;
    case DWG_TYPE_VPORT:
      dwg_svg_VPORT(dat, obj);
      break;
    case DWG_TYPE_APPID_CONTROL:
      dwg_svg_APPID_CONTROL(dat, obj);
      break;
    case DWG_TYPE_APPID:
      dwg_svg_APPID(dat, obj);
      break;
    case DWG_TYPE_DIMSTYLE_CONTROL:
      dwg_svg_DIMSTYLE_CONTROL(dat, obj);
      break;
    case DWG_TYPE_DIMSTYLE:
      dwg_svg_DIMSTYLE(dat, obj);
      break;
    case DWG_TYPE_VP_ENT_HDR_CONTROL:
      dwg_svg_VP_ENT_HDR_CONTROL(dat, obj);
      break;
    case DWG_TYPE_VP_ENT_HDR:
      dwg_svg_VP_ENT_HDR(dat, obj);
      break;
    case DWG_TYPE_GROUP:
      dwg_svg_GROUP(dat, obj);
      break;
    case DWG_TYPE_MLINESTYLE:
      dwg_svg_MLINESTYLE(dat, obj);
      break;
    case DWG_TYPE_OLE2FRAME:
      dwg_svg_OLE2FRAME(dat, obj);
      break;
    case DWG_TYPE_DUMMY:
      dwg_svg_DUMMY(dat, obj);
      break;
    case DWG_TYPE_LONG_TRANSACTION:
      dwg_svg_LONG_TRANSACTION(dat, obj);
      break;
    case DWG_TYPE_LWPLINE:
      dwg_svg_LWPLINE(dat, obj);
      break;
    case DWG_TYPE_HATCH:
      dwg_svg_HATCH(dat, obj);
      break;
    case DWG_TYPE_XRECORD:
      dwg_svg_XRECORD(dat, obj);
      break;
    case DWG_TYPE_PLACEHOLDER:
      dwg_svg_PLACEHOLDER(dat, obj);
      break;
    case DWG_TYPE_PROXY_ENTITY:
      dwg_svg_PROXY_ENTITY(dat, obj);
      break;
    case DWG_TYPE_OLEFRAME:
      dwg_svg_OLEFRAME(dat, obj);
      break;
    case DWG_TYPE_VBA_PROJECT:
      LOG_ERROR("Unhandled Object VBA_PROJECT. Has its own section\n");
      //dwg_svg_VBA_PROJECT(dat, obj);
      break;
    case DWG_TYPE_LAYOUT:
      dwg_svg_LAYOUT(dat, obj);
      break;
    default:
      if (obj->type == obj->parent->layout_number)
        {
          dwg_svg_LAYOUT(dat, obj);
        }
      /* > 500:
         TABLE, DICTIONARYWDLFT, IDBUFFER, IMAGE, IMAGEDEF, IMAGEDEF_REACTOR,
         LAYER_INDEX, OLE2FRAME, PROXY, RASTERVARIABLES, SORTENTSTABLE, SPATIAL_FILTER,
         SPATIAL_INDEX
      */
      else if (!dwg_svg_variable_type(obj->parent, dat, obj))
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
              dwg_svg_UNKNOWN_OBJ(dat, obj);
            }
          else if (klass)
            {
              dwg_svg_UNKNOWN_ENT(dat, obj);
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
svg_common_entity_handle_data(Bit_Chain *dat, Dwg_Object* obj)
{
  (void)dat; (void)obj;
}

const char *
svg_format (int code)
{
  if (0 <= code && code < 5)
    return "%s";
  if (code == 5)
    return "%X";
  if (5 < code && code < 10)
    return "%s";
  if (code < 60)
    return "%-16.15g";
  if (code < 80)
    return "%6i";
  if (89 < code && code < 100)
    return "%9li";
  if (code == 100)
    return "%s";
  if (code == 102)
    return "%s";
  if (code == 105)
    return "%X";
  if (105 < code && code < 148)
    return "%-16.15g";
  if (169 < code && code < 180)
    return "%6i";
  if (269 < code && code < 300)
    return "%6i";
  if (299 < code && code < 320)
    return "%s";
  if (319 < code && code < 370)
    return "%X";
  if (369 < code && code < 390)
    return "%6i";
  if (389 < code && code < 400)
    return "%X";
  if (399 < code && code < 410)
    return "%6i";
  if (409 < code && code < 420)
    return "%s";
  if (code == 999)
    return "%s";
  if (999 < code && code < 1010)
    return "%s";
  if (1009 < code && code < 1060)
    return "%-16.15g";
  if (1059 < code && code < 1071)
    return "%6i";
  if (code == 1071)
    return "%9li";

  return "(unknown code)";
}

// see https://www.autodesk.com/techpubs/autocad/acad2000/svg/header_section_group_codes_svg_02.htm
void
svg_header_write(Bit_Chain *dat, Dwg_Data* dwg)
{
  Dwg_Header_Variables* _obj = &dwg->header_vars;
  Dwg_Object* obj = NULL;
  const int minimal = dwg->opts & 0x10;
  double ms;
  const char* codepage =
    (dwg->header.codepage == 30 || dwg->header.codepage == 0)
    ? "ANSI_1252"
    : (dwg->header.version >= R_2007)
      ? "UTF-8"
      : "ANSI_1252";

  SECTION(HEADER);

  VAR (ACADVER, 1, version_codes[dwg->header.version]);
  if (minimal) {
    HEADER_VALUE (HANDSEED, 5);
    ENDSEC();
    return;
  }
  SINCE(R_13) {
    VAR (ACADMAINTVER, 70, dwg->header.maint_version);
  }
  if (dwg->header.codepage != 30 &&
      dwg->header.codepage != 0 &&
      dwg->header.version < R_2007) {
    // some asian or eastern-european codepage
    // see https://pythonhosted.org/ezsvg/svginternals/fileencoding.html
    LOG_WARN("Unknown codepage %d, assuming ANSI_1252", dwg->header.codepage);
  }
  SINCE(R_10) {
    VAR (DWGCODEPAGE, 3, codepage);
  }
  SINCE(R_2010) {
    VAR (LASTSAVEDBY, 1, ""); //TODO
  }
  SINCE(R_2013) {
    HEADER_VALUE (REQUIREDVERSIONS, 160);
  }
  HEADER_3D (INSBASE);
  HEADER_3D (EXTMIN);
  HEADER_3D (EXTMAX);
  HEADER_2D (LIMMIN);
  HEADER_2D (LIMMAX);

  HEADER_VALUE (ORTHOMODE, 70);
  HEADER_VALUE (REGENMODE, 70);
  HEADER_VALUE (FILLMODE, 70);
  HEADER_VALUE (QTEXTMODE, 70);
  HEADER_VALUE (MIRRTEXT, 70);
  UNTIL(R_14) {
    HEADER_VALUE (DRAGMODE, 70);
  }
  HEADER_VALUE (LTSCALE, 40);
  UNTIL(R_14) {
    HEADER_VALUE (OSMODE, 70);
  }
  HEADER_VALUE (ATTMODE, 70);
  HEADER_VALUE (TEXTSIZE, 40);
  HEADER_VALUE (TRACEWID, 40);

  HANDLE_NAME (TEXTSTYLE, 7, SHAPEFILE);
  HANDLE_NAME (CLAYER, 8, LAYER);
  HANDLE_NAME (CELTYPE, 6, LTYPE);
  HEADER_VALUE (CECOLOR, 62);
  SINCE(R_13) {
    HEADER_VALUE (CELTSCALE, 40);
    UNTIL(R_14) {
      HEADER_VALUE (DELOBJ, 70);
    }
    HEADER_VALUE (DISPSILH, 70); // this is WIREFRAME
    HEADER_VALUE (DIMSCALE, 40);
  }
  HEADER_VALUE (DIMASZ, 40);
  HEADER_VALUE (DIMEXO, 40);
  HEADER_VALUE (DIMDLI, 40);
  HEADER_VALUE (DIMRND, 40);
  HEADER_VALUE (DIMDLE, 40);
  HEADER_VALUE (DIMEXE, 40);
  //HEADER_VALUE (DIMTP, 40);
  //HEADER_VALUE (DIMTM, 40);
  HEADER_VALUE (DIMTXT, 40);
  HEADER_VALUE (DIMCEN, 40);
  HEADER_VALUE (DIMTSZ, 40);
  HEADER_VALUE (DIMTOL, 70);
  HEADER_VALUE (DIMLIM, 70);
  HEADER_VALUE (DIMTIH, 70);
  HEADER_VALUE (DIMTOH, 70);
  HEADER_VALUE (DIMSE1, 70);
  HEADER_VALUE (DIMSE2, 70);
  HEADER_VALUE (DIMTAD, 70);
  HEADER_VALUE (DIMZIN, 70);
  HANDLE_NAME (DIMBLK, 1, BLOCK_HEADER);
  HEADER_VALUE (DIMASO, 70);
  HEADER_VALUE (DIMSHO, 70);
  VERSIONS(R_13, R_14) {
    HEADER_VALUE (DIMSAV, 70); //?
  }
  HEADER_VALUE (DIMPOST, 1);
  HEADER_VALUE (DIMAPOST, 1);
  HEADER_VALUE (DIMALT, 70);
  HEADER_VALUE (DIMALTD, 70);
  HEADER_VALUE (DIMALTF, 40);
  HEADER_VALUE (DIMLFAC, 40);
  HEADER_VALUE (DIMTOFL, 70);
  HEADER_VALUE (DIMTVP, 40);
  HEADER_VALUE (DIMTIX, 70);
  HEADER_VALUE (DIMSOXD, 70);
  HEADER_VALUE (DIMSAH, 70);
  HANDLE_NAME (DIMBLK1, 1,  BLOCK_HEADER);
  HANDLE_NAME (DIMBLK2, 1,  BLOCK_HEADER);
  HANDLE_NAME (DIMSTYLE, 2, DIMSTYLE);
  HEADER_VALUE (DIMCLRD, 70);
  HEADER_VALUE (DIMCLRE, 70);
  HEADER_VALUE (DIMCLRT, 70);
  HEADER_VALUE (DIMTFAC, 40);
  HEADER_VALUE (DIMGAP, 40);
  SINCE(R_13) {
    HEADER_VALUE (DIMJUST, 70);
    HEADER_VALUE (DIMSD1, 70);
    HEADER_VALUE (DIMSD2, 70);
    HEADER_VALUE (DIMTOLJ, 70);
    HEADER_VALUE (DIMTZIN, 70);
    HEADER_VALUE (DIMALTZ, 70);
    HEADER_VALUE (DIMALTTZ, 70);
    HEADER_VALUE (DIMUPT, 70);
    HEADER_VALUE (DIMDEC, 70);
    HEADER_VALUE (DIMTDEC, 70);
    HEADER_VALUE (DIMALTU, 70);
    HEADER_VALUE (DIMALTTD, 70);
    HANDLE_NAME (DIMTXSTY, 7, SHAPEFILE);
    HEADER_VALUE (DIMAUNIT, 70);
  }
  SINCE(R_2000) {
    HEADER_VALUE (DIMADEC, 70);
    HEADER_VALUE (DIMALTRND, 40);
    HEADER_VALUE (DIMAZIN, 70);
    HEADER_VALUE (DIMDSEP, 70);
    HEADER_VALUE (DIMATFIT, 70);
    HEADER_VALUE (DIMFRAC, 70);
    HANDLE_NAME (DIMLDRBLK, 1, BLOCK_HEADER);
    HEADER_VALUE (DIMLUNIT, 70);
    HEADER_VALUE (DIMLWD, 70);
    HEADER_VALUE (DIMLWE, 70);
    HEADER_VALUE (DIMTMOVE, 70);
  }
  HEADER_VALUE (LUNITS, 70);
  HEADER_VALUE (LUPREC, 70);
  HEADER_VALUE (SKETCHINC, 40);
  HEADER_VALUE (FILLETRAD, 40);
  HEADER_VALUE (AUNITS, 70);
  HEADER_VALUE (AUPREC, 70);
  HEADER_VALUE (MENU, 1);
  HEADER_VALUE (ELEVATION, 40);
  HEADER_VALUE (PELEVATION, 40);
  HEADER_VALUE (THICKNESS, 40);
  HEADER_VALUE (LIMCHECK, 70);
  UNTIL(R_14) {
      HEADER_VALUE (BLIPMODE, 70);
    }
  HEADER_VALUE (CHAMFERA, 40);
  HEADER_VALUE (CHAMFERB, 40);
  SINCE(R_13) {
    HEADER_VALUE (CHAMFERC, 40);
    HEADER_VALUE (CHAMFERD, 40);
  }
  HEADER_VALUE (SKPOLY, 70);

  ms = (double)dwg->header_vars.TDCREATE.ms;
  VAR (TDCREATE, 40, dwg->header_vars.TDCREATE.days + ms);
  SINCE(R_13) {
    VAR (TDUCREATE, 40, dwg->header_vars.TDCREATE.days + ms);
  }
  ms = (double)dwg->header_vars.TDUPDATE.ms;
  VAR (TDUPDATE, 40, dwg->header_vars.TDUPDATE.days + ms);
  SINCE(R_13) {
    VAR (TDUUPDATE, 40, dwg->header_vars.TDUPDATE.days + ms);
  }
  ms = (double)dwg->header_vars.TDINDWG.ms;
  VAR (TDINDWG, 40, dwg->header_vars.TDINDWG.days + ms);
  ms = (double)dwg->header_vars.TDUSRTIMER.ms;
  VAR (TDUSRTIMER, 40, dwg->header_vars.TDUSRTIMER.days + ms);

  //HEADER_VALUE (USRTIMER, 70); // 1
  HEADER_VALUE (ANGBASE, 50);
  HEADER_VALUE (ANGDIR, 70);
  HEADER_VALUE (PDMODE, 70);
  HEADER_VALUE (PDSIZE, 40);
  HEADER_VALUE (PLINEWID, 40);
  UNTIL(R_14) {
    HEADER_VALUE (COORDS, 70); // 2
  }
  HEADER_VALUE (SPLFRAME, 70);
  HEADER_VALUE (SPLINETYPE, 70);
  HEADER_VALUE (SPLINESEGS, 70);
  UNTIL(R_14) {
    HEADER_VALUE (ATTDIA, 70); //default 1
    HEADER_VALUE (ATTREQ, 70); //default 1
    HEADER_VALUE (HANDLING, 70); //default 1
  }

  HEADER_VALUE (HANDSEED, 5); //default: 20000, before r13: 0xB8BC

  HEADER_VALUE (SURFTAB1, 70); // 6
  HEADER_VALUE (SURFTAB2, 70); // 6
  HEADER_VALUE (SURFTYPE, 70); // 6
  HEADER_VALUE (SURFU, 70); // 6
  HEADER_VALUE (SURFV, 70); // 6
  SINCE(R_13) {
    HANDLE_NAME (UCSBASE, 2, UCS);
  }
  HANDLE_NAME (UCSNAME, 2, UCS);
  HEADER_3D (UCSORG);
  HEADER_3D (UCSXDIR);
  HEADER_3D (UCSYDIR);
  HANDLE_NAME (UCSORTHOREF, 2, UCS);
  HEADER_VALUE (UCSORTHOVIEW, 70);
  HEADER_3D (UCSORGTOP);
  HEADER_3D (UCSORGBOTTOM);
  HEADER_3D (UCSORGLEFT);
  HEADER_3D (UCSORGRIGHT);
  HEADER_3D (UCSORGFRONT);
  HEADER_3D (UCSORGBACK);

  HANDLE_NAME (PUCSBASE, 2, UCS);
  HANDLE_NAME (PUCSNAME, 2, UCS);
  HEADER_3D (PUCSORG);
  HEADER_3D (PUCSXDIR);
  HEADER_3D (PUCSYDIR);
  //HANDLE_NAME (PUCSORTHOREF, 2, UCS);
  HEADER_VALUE (PUCSORTHOVIEW, 70);
  HEADER_3D (PUCSORGTOP);
  HEADER_3D (PUCSORGBOTTOM);
  HEADER_3D (PUCSORGLEFT);
  HEADER_3D (PUCSORGRIGHT);
  HEADER_3D (PUCSORGFRONT);
  HEADER_3D (PUCSORGBACK);

  HEADER_VALUE (USERI1, 70);
  HEADER_VALUE (USERI2, 70);
  HEADER_VALUE (USERI3, 70);
  HEADER_VALUE (USERI4, 70);
  HEADER_VALUE (USERI5, 70);
  HEADER_VALUE (USERR1, 40);
  HEADER_VALUE (USERR2, 40);
  HEADER_VALUE (USERR3, 40);
  HEADER_VALUE (USERR4, 40);
  HEADER_VALUE (USERR5, 40);

  HEADER_VALUE (WORLDVIEW, 70);
  //VERSION(R_13) {
  //  HEADER_VALUE (WIREFRAME, 70); //Undocumented
  //}
  HEADER_VALUE (SHADEDGE, 70);
  HEADER_VALUE (SHADEDIF, 70);
  HEADER_VALUE (TILEMODE, 70);
  HEADER_VALUE (MAXACTVP, 70);

  HEADER_3D (PINSBASE);
  HEADER_VALUE (PLIMCHECK, 70);
  HEADER_3D (PEXTMIN);
  HEADER_3D (PEXTMAX);
  HEADER_2D (PLIMMIN);
  HEADER_2D (PLIMMAX);

  HEADER_VALUE (UNITMODE, 70);
  HEADER_VALUE (VISRETAIN, 70);
  VERSIONS(R_13, R_14) {
    HEADER_VALUE (DELOBJ, 70);
  }
  HEADER_VALUE (PLINEGEN, 70);
  HEADER_VALUE (PSLTSCALE, 70);
  HEADER_VALUE (TREEDEPTH, 70);
  UNTIL(R_11) {
    VAR (DWGCODEPAGE, 3, codepage);
  }
  VERSIONS(R_14, R_2000) {
    HEADER_VALUE (PROXYGRAPHICS, 70);
  }
  //HANDLE_NAME (CMLSTYLE, 2, MLINESTYLE); //default: Standard
  HEADER_VALUE (CMLJUST, 70);
  HEADER_VALUE (CMLSCALE, 40); //default: 20
  VERSIONS(R_13, R_14) {
    HEADER_VALUE (SAVEIMAGES, 70);
  }
  SINCE(R_2000) {
    HEADER_VALUE (PROXYGRAPHICS, 70);
    VAR (MEASUREMENT, 70, dwg->measurement ? 1 : 0);
    HEADER_VALUE (CELWEIGHT, 370);
    HEADER_VALUE (ENDCAPS, 280);
    HEADER_VALUE (JOINSTYLE, 280);
    HEADER_VALUE (LWDISPLAY, 290);
    HEADER_VALUE (INSUNITS, 70);
    HEADER_VALUE (HYPERLINKBASE, 1);
    HEADER_VALUE (STYLESHEET, 1);
    HEADER_VALUE (XEDIT, 290);
    HEADER_VALUE (CEPSNTYPE, 380);

    if (dwg->header_vars.CEPSNTYPE == 3)
    {
      HANDLE_NAME (CPSNID, 390, LTYPE);
    }
    HEADER_VALUE (PSTYLEMODE, 290);
    HEADER_VALUE (FINGERPRINTGUID, 2);
    HEADER_VALUE (VERSIONGUID, 2);
    HEADER_VALUE (EXTNAMES, 290);
    HEADER_VALUE (PSVPSCALE, 40);
    HEADER_VALUE (OLESTARTUP, 290);
  }
  SINCE(R_2004) {
    HEADER_VALUE (SORTENTS, 280);
    HEADER_VALUE (INDEXCTL, 280);
    HEADER_VALUE (HIDETEXT, 280);
    PRE(R_2010) {
      HEADER_VALUE (XCLIPFRAME, 290);
    } LATER_VERSIONS {
      HEADER_VALUE (XCLIPFRAME, 280);
    }
    HEADER_VALUE (HALOGAP, 280);
    HEADER_VALUE (OBSCOLOR, 280);
    HEADER_VALUE (INTERSECTIONCOLOR, 70);
    HEADER_VALUE (OBSLTYPE, 280);
    HEADER_VALUE (INTERSECTIONDISPLAY, 290);
    HEADER_VALUE (DIMASSOC, 280);
    HEADER_VALUE (PROJECTNAME, 1);

    SINCE(R_2007) {
      HEADER_VALUE (CAMERADISPLAY, 290);
      //HEADER_VALUE (unknown_21, 0);
      //HEADER_VALUE (unknown_22, 0);
      //HEADER_VALUE (unknown_23, 0);
      HEADER_VALUE (LENSLENGTH, 40);
      HEADER_VALUE (CAMERAHEIGHT, 40);
      HEADER_VALUE (STEPSPERSEC, 40);
      HEADER_VALUE (STEPSIZE, 40);
      VAR (3DDWFPREC, 40, dwg->header_vars._3DDWFPREC);
      HEADER_VALUE (PSOLWIDTH, 40);
      HEADER_VALUE (PSOLHEIGHT, 40);
      HEADER_VALUE (LOFTANG1, 40);
      HEADER_VALUE (LOFTANG2, 40);
      HEADER_VALUE (LOFTMAG1, 40);
      HEADER_VALUE (LOFTMAG2, 40);
      HEADER_VALUE (LOFTPARAM, 70);
      HEADER_VALUE (LOFTNORMALS, 280);
      HEADER_VALUE (LATITUDE, 40);
      HEADER_VALUE (LONGITUDE, 40);
      HEADER_VALUE (NORTHDIRECTION, 40);
      HEADER_VALUE (TIMEZONE, 70);
      HEADER_VALUE (LIGHTGLYPHDISPLAY, 280);
      HEADER_VALUE (TILEMODELIGHTSYNCH, 280);
      HEADER_VALUE (SOLIDHIST, 280);
      HEADER_VALUE (SHOWHIST, 280);
      HEADER_VALUE (DWFFRAME, 280);
      HEADER_VALUE (DGNFRAME, 280);
      HEADER_VALUE (REALWORLDSCALE, 290);
      FIELD_CMC (INTERFERECOLOR, 62); //TODO default: 1
      //FIELD_HANDLE (INTERFEREOBJVS, 5, 345);
      //FIELD_HANDLE (INTERFEREVPVS, 5, 346);
      //FIELD_HANDLE (DRAGVS, 5, 349);
      HEADER_VALUE (CSHADOW, 280);
      HEADER_VALUE (SHADOWPLANELOCATION, 40);
    }
  }

  ENDSEC();
  return;
}

// only called since r2000
static int
svg_classes_write (Bit_Chain *dat, Dwg_Data * dwg)
{
  unsigned int i;

  SECTION (CLASSES);
  for (i=0; i < dwg->num_classes; i++)
    {
      RECORD (CLASS);
      VALUE (1, dwg->dwg_class[i].svgname);
      PRE (R_2007) {
        VALUE (2, dwg->dwg_class[i].cppname);
        VALUE (3, dwg->dwg_class[i].appname);
      } LATER_VERSIONS {
        char *cppname = bit_convert_TU((BITCODE_TU)dwg->dwg_class[i].cppname);
        char *appname = bit_convert_TU((BITCODE_TU)dwg->dwg_class[i].appname);
        VALUE (2, cppname);
        VALUE (3, appname);
      }
      VALUE (90, dwg->dwg_class[i].proxyflag);
      VALUE (280, dwg->dwg_class[i].wasazombie);
      // Is-an-entity. 1f2 for entities, 1f3 for objects
      VALUE (281, dwg->dwg_class[i].item_class_id == 0x1F2 ? 1 : 0);
    }
  ENDSEC();
  return 0;
}

static int
svg_tables_write (Bit_Chain *dat, Dwg_Data * dwg)
{
  (void)dwg;

  SECTION(TABLES);
  //...
  ENDSEC();
  return 0;
}

static int
svg_blocks_write (Bit_Chain *dat, Dwg_Data * dwg)
{
  (void)dwg;

  SECTION(BLOCKS);
  //...
  ENDSEC();
  return 0;
}

static int
svg_entities_write (Bit_Chain *dat, Dwg_Data * dwg)
{
  (void)dwg;

  SECTION(ENTITIES);
  //...
  ENDSEC();
  return 0;
}

static int
svg_objects_write (Bit_Chain *dat, Dwg_Data * dwg)
{
  (void)dwg;

  SECTION(OBJECTS);
  //...
  ENDSEC();
  return 0;
}

static int
svg_preview_write (Bit_Chain *dat, Dwg_Data * dwg)
{
  (void)dat; (void)dwg;
  //...
  return 0;
}

int
dwg_write_svg(Bit_Chain *dat, Dwg_Data * dwg)
{
  const int minimal = dwg->opts & 0x10;
  struct Dwg_Header *obj = &dwg->header;

  VALUE(999, PACKAGE_STRING);

  // A minimal header requires only $ACADVER, $HANDSEED, and then ENTITIES
  // see https://pythonhosted.org/ezsvg/svginternals/filestructure.html
  SINCE(R_13)
  {
    svg_header_write (dat, dwg);

    SINCE(R_2000) {
      if (svg_classes_write (dat, dwg))
        goto fail;
    }

    if (svg_tables_write (dat, dwg))
      goto fail;

    if (svg_blocks_write (dat, dwg))
      goto fail;
  }

  if (svg_entities_write (dat, dwg))
    goto fail;

  SINCE(R_13) {
    if (svg_objects_write (dat, dwg))
      goto fail;
  }

  if (dwg->header.version >= R_2000 && !minimal) {
    if (svg_preview_write (dat, dwg))
      goto fail;
  }

  return 0;
 fail:
  return 1;
}

#undef IS_PRINT
