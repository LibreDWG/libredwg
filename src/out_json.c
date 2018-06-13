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
#include "decode.h"
#include "out_json.h"

#define DWG_LOGLEVEL DWG_LOGLEVEL_NONE
#include "logging.h"

/* the current version per spec block */
static unsigned int cur_ver = 0;

/*--------------------------------------------------------------------------------
 * MACROS
 */

#define IS_PRINT

#define PREFIX   for (int _i=0; _i<dat->bit; _i++) { fprintf (dat->fh, "  "); }
#define ARRAY    PREFIX fprintf (dat->fh, "[\n"); dat->bit++
#define ENDARRAY dat->bit--; PREFIX fprintf (dat->fh, "],\n")
#define HASH     PREFIX fprintf (dat->fh, "{\n"); dat->bit++
#define ENDHASH  dat->bit--; PREFIX fprintf (dat->fh, "},\n")
#define SECTION(name) PREFIX fprintf (dat->fh, "\"%s\": [\n", #name); dat->bit++;
#define ENDSEC()  ENDARRAY
#define NOCOMMA   fseek(dat->fh, -2, SEEK_CUR)

#define VALUE(value,type,dxf) \
    fprintf(dat->fh, FORMAT_##type, value)
#define VALUE_RC(value,dxf) VALUE(value, RC, dxf)
#define VALUE_RS(value,dxf) VALUE(value, RS, dxf)
#define VALUE_RL(value,dxf) VALUE(value, RL, dxf)
#define VALUE_RD(value,dxf) VALUE(value, RD, dxf)

#define FIELD(name,type,dxf) \
    PREFIX fprintf(dat->fh, "\"" #name "\": " FORMAT_##type ",\n", _obj->name)
#define _FIELD(name,type,value) \
    PREFIX fprintf(dat->fh, "\"" #name "\": " FORMAT_##type ",\n", obj->name)
#define ENT_FIELD(name,type,value) \
    PREFIX fprintf(dat->fh, "\"" #name "\": " FORMAT_##type ",\n", _ent->name)
#define FIELD_CAST(name,type,cast,dxf) FIELD(name,cast,dxf)
#define FIELD_TRACE(name,type)
#define FIELD_TEXT(name,str) \
    PREFIX fprintf(dat->fh, "\"" #name "\": \"%s\",\n", str ? str : "")
#ifdef HAVE_NATIVE_WCHAR2
# define FIELD_TEXT_TU(name,wstr) \
    PREFIX fprintf(dat->fh, "\"" #name "\": \"%ls\",\n", wstr ? (wchar_t*)wstr : L"")
#else
# define FIELD_TEXT_TU(name,wstr) \
  { \
    BITCODE_TU ws = (BITCODE_TU)wstr;\
    uint16_t _c; PREFIX \
    fprintf(dat->fh, "\"" #name "\": \""); \
    if (ws) { \
      while ((_c = *ws++)) { \
        fprintf(dat->fh, "%c", (char)(_c & 0xff)); \
      } \
    }\
    fprintf(dat->fh, "\",\n"); \
  }
#endif

#define FIELD_VALUE(name) _obj->name
#define ANYCODE -1
// todo: only the name, not the ref
#define VALUE_HANDLE(hdlptr, name, handle_code, dxf)     \
  PREFIX if (hdlptr) { \
    fprintf(dat->fh, "\"%s\": \"HANDLE(%d.%d.%lu) absolute:%lu\",\n", #name, \
           hdlptr->handleref.code,                     \
           hdlptr->handleref.size,                     \
           hdlptr->handleref.value,                    \
           hdlptr->absolute_ref);                      \
  }
#define FIELD_HANDLE(name, handle_code, dxf) VALUE_HANDLE(_obj->name, name, handle_code, dxf)
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
    PREFIX fprintf(dat->fh, "\"" #name "\": " FORMAT_DD ",\n", _obj->name)
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
    if (size) NOCOMMA;\
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
    if (_obj->size) NOCOMMA;\
    ENDARRAY;

#define FIELD_VECTOR(name, type, size, dxf) FIELD_VECTOR_N(name, type, _obj->size, dxf)

#define FIELD_2RD_VECTOR(name, size, dxf)\
  ARRAY;\
  for (vcount=0; vcount < (int)_obj->size; vcount++)\
    {\
      FIELD_2RD(name[vcount], dxf);\
    }\
  if (_obj->size) NOCOMMA;\
  ENDARRAY;

#define FIELD_2DD_VECTOR(name, size, dxf)\
  ARRAY;\
  FIELD_2RD(name[0], 0);\
  for (vcount = 1; vcount < (int)_obj->size; vcount++)\
    {\
      FIELD_2DD(name[vcount], FIELD_VALUE(name[vcount - 1].x), FIELD_VALUE(name[vcount - 1].y), dxf);\
    }\
  if (_obj->size) NOCOMMA;\
  ENDARRAY;

#define FIELD_3DPOINT_VECTOR(name, size, dxf)\
  ARRAY;\
  for (vcount=0; vcount < (int)_obj->size; vcount++)\
    {\
      FIELD_3DPOINT(name[vcount], dxf);\
    }\
  if (_obj->size) NOCOMMA;\
  ENDARRAY;

#define HANDLE_VECTOR_N(name, size, code, dxf) \
  ARRAY;\
  for (vcount=0; vcount < (int)size; vcount++)\
    {\
      FIELD_HANDLE_N(name[vcount], vcount, code, dxf);\
    }\
  if (size) NOCOMMA;                            \
  ENDARRAY;

#define HANDLE_VECTOR(name, sizefield, code, dxf) \
  HANDLE_VECTOR_N(name, FIELD_VALUE(sizefield), code, dxf)

#define FIELD_NUM_INSERTS(num_inserts, type, dxf) \
  FIELD(num_inserts, type, dxf)

#define FIELD_XDATA(name, size)

#define REACTORS(code)\
  PREFIX; \
  fprintf(dat->fh, "\"reactors\":"); ARRAY; \
  for (vcount=0; vcount < (int)obj->tio.object->num_reactors; vcount++)\
    {\
      VALUE_HANDLE(obj->tio.object->reactors[vcount], reactors, code, 330); \
    }\
  if (obj->tio.object->num_reactors) NOCOMMA;\
  ENDARRAY;

#define XDICOBJHANDLE(code)\
  SINCE(R_2004)\
    {\
      if (!obj->tio.object->xdic_missing_flag)\
        VALUE_HANDLE(obj->tio.object->xdicobjhandle, xdicobjhandle, code, -3); \
    }\
  PRIOR_VERSIONS\
    {\
      VALUE_HANDLE(obj->tio.object->xdicobjhandle, xdicobjhandle, code, -3); \
    }

#define COMMON_ENTITY_HANDLE_DATA
#define SECTION_STRING_STREAM
#define START_STRING_STREAM
#define END_STRING_STREAM
#define START_HANDLE_STREAM

#define DWG_ENTITY(token) \
static int \
dwg_json_##token (Bit_Chain *restrict dat, Dwg_Object *restrict obj) \
{\
  long vcount, rcount1, rcount2, rcount3, rcount4; \
  Dwg_Entity_##token *ent, *_obj;\
  Dwg_Object_Entity *_ent;\
  LOG_INFO("Entity " #token ":\n")\
  _ent = obj->tio.entity;\
  _obj = ent = _ent->tio.token;\
  FIELD_TEXT(entity, #token);\
  _FIELD(type,RL,0);\
  _FIELD(size,RL,0);\
  _FIELD(bitsize,BL,0);\
  ENT_FIELD(picture_exists,B,0);

#define DWG_ENTITY_END return 0; }

#define DWG_OBJECT(token) \
static int \
dwg_json_ ##token (Bit_Chain *restrict dat, Dwg_Object *restrict obj) \
{ \
  long vcount, rcount1, rcount2, rcount3, rcount4;\
  Bit_Chain *hdl_dat = dat;\
  Dwg_Object_##token *_obj;\
  LOG_INFO("Object " #token ":\n")\
  _obj = obj->tio.object->tio.token;\
  FIELD_TEXT(object, #token);\
  _FIELD(type,RL,0);\
  _FIELD(size,RL,0);\
  _FIELD(bitsize,BL,0);

#define DWG_OBJECT_END return 0; }

#include "dwg.spec"

/* returns 0 on success
 */
static int
dwg_json_variable_type(Dwg_Data *restrict dwg, Bit_Chain *restrict dat,
                       Dwg_Object *restrict  obj)
{
  int i;
  char *dxfname;
  Dwg_Class *klass;
  int is_entity;

  i = obj->type - 500;
  if (i < 0 || i >= (int)dwg->num_classes)
    return DWG_ERR_INVALIDTYPE;

  klass = &dwg->dwg_class[i];
  if (!klass || ! klass->dxfname)
    return DWG_ERR_INTERNALERROR;
  dxfname = klass->dxfname;
  // almost always false
  is_entity = dwg_class_is_entity(klass);

  #define action json
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

/*
static void
json_common_entity_handle_data(Bit_Chain *restrict dat, Dwg_Object *restrict obj)
{
  (void)dat; (void)obj;
}
*/

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

  SECTION(HEADER);
  #include "header_variables.spec"
  NOCOMMA;
  ENDSEC();
  return 0;
}

static int
json_classes_write (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  unsigned int i;

  SECTION(CLASSES);
  LOG_TRACE("num_classes: %u\n", dwg->num_classes);
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
      // Is-an-entity. 1f2 for entities, 1f3 for objects
      //VALUE (281, dwg->dwg_class[i].item_class_id == 0x1F2 ? 1 : 0);
      NOCOMMA;
      ENDHASH;
    }
  NOCOMMA;
  ENDSEC();
  return 0;
}

static int
json_tables_write (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  (void)dwg;

  SECTION(TABLES);
  //...
  ENDSEC();
  return 0;
}

static int
json_blocks_write (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  (void)dwg;

  SECTION(BLOCKS);
  //...
  ENDSEC();
  return 0;
}

static int
json_entities_write (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  unsigned int i;

  SECTION(ENTITIES);
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

/* The object map: we skip this
static int
json_objects_write (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
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
json_preview_write (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  (void)dat; (void)dwg;
  //...
  return 0;
}

EXPORT int
dwg_write_json(Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
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
