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
 * free.c: helper functions to free all spec fields
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
#include "free.h"

static unsigned int loglevel;
#ifdef USE_TRACING
static int env_var_checked_p;
#endif
#define DWG_LOGLEVEL loglevel
#include "logging.h"

/* the current version per spec block */
static unsigned int cur_ver = 0;
static Bit_Chain pdat = {NULL,0,0,0,0,0};
static Bit_Chain *dat = &pdat;

extern void dwg_free_xdata_resbuf(Dwg_Resbuf *rbuf);
extern int  dwg_obj_is_control(const Dwg_Object *obj);
/*--------------------------------------------------------------------------------
 * MACROS
 */

#define IS_FREE

#define VALUE(value,type,dxf)
#define VALUE_RC(value,dxf) VALUE(value, RC, dxf)
#define VALUE_RD(value,dxf) VALUE(value, RD, dxf)

#define FIELD(name,type) {}
#define FIELD_TRACE(name,type) \
  LOG_TRACE(#name ": " FORMAT_##type "\n", _obj->name)
#define FIELD_CAST(name,type,cast,dxf) {}
#define FIELD_VALUE(name) _obj->name

#define ANYCODE -1
#define FIELD_HANDLE(name,code,dxf) dwg_free_handleref(_obj->name, dwg)
#define FIELD_DATAHANDLE(name,code,dxf) FIELD_HANDLE(name, code, dxf)
#define FIELD_HANDLE_N(name,vcount,code,dxf) FIELD_HANDLE(name, code, dxf)

#define FIELD_B(name,dxf) FIELD(name, B)
#define FIELD_BB(name,dxf) FIELD(name, BB)
#define FIELD_3B(name,dxf) FIELD(name, 3B)
#define FIELD_BS(name,dxf) FIELD(name, BS)
#define FIELD_BL(name,dxf) FIELD(name, BL)
#define FIELD_BLL(name,dxf) FIELD(name, BLL)
#define FIELD_BD(name,dxf) FIELD(name, BD)
#define FIELD_RC(name,dxf) FIELD(name, RC)
#define FIELD_RS(name,dxf) FIELD(name, RS)
#define FIELD_RD(name,dxf) FIELD(name, RD)
#define FIELD_RL(name,dxf) FIELD(name, RL)
#define FIELD_RLL(name,dxf) FIELD(name, RLL)
#define FIELD_MC(name,dxf) FIELD(name, MC)
#define FIELD_MS(name,dxf) FIELD(name, MS)
#define FIELD_TV(name,dxf) \
  if (FIELD_VALUE(name))\
    {\
      free (FIELD_VALUE(name)); \
      FIELD_VALUE(name) = NULL; \
    }
#define FIELD_TU(name,dxf)  FIELD_TV(name,dxf)
#define FIELD_TF(name,len,dxf) FIELD_TV(name,dxf)
#define FIELD_TFF(name,len,dxf) {}
#define FIELD_T FIELD_TV /*TODO: implement version dependant string fields */
#define FIELD_BT(name,dxf) FIELD(name, BT);
#define FIELD_4BITS(name,dxf) {}
#define FIELD_BE(name,dxf) {}
#define FIELD_DD(name, _default, dxf) {}
#define FIELD_2DD(name, d1, d2, dxf) {}
#define FIELD_3DD(name, def, dxf) {}
#define FIELD_2RD(name,dxf) {}
#define FIELD_2BD(name,dxf) {}
#define FIELD_2BD_1(name,dxf) {}
#define FIELD_3RD(name,dxf) {}
#define FIELD_3BD(name,dxf) {}
#define FIELD_3BD_1(name,dxf) {}
#define FIELD_3DPOINT(name,dxf) {}
#define FIELD_TIMEBLL(name,dxf)
#define FIELD_CMC(token, dxf)\
  { FIELD_TV(token.name, dxf); \
    FIELD_TV(token.book_name, dxf+1); }

//FIELD_VECTOR_N(name, type, size):
// reads data of the type indicated by 'type' 'size' times and stores
// it all in the vector called 'name'.
#define FIELD_VECTOR_N(name, type, size, dxf) \
  if (size) { \
    for (vcount=0; vcount < (int)size; vcount++)  \
      FIELD_##type(name[vcount], dxf); \
  } \
  FIELD_TV(name,dxf);
#define FIELD_VECTOR_T(name, size, dxf) FIELD_VECTOR_N(name, TV, _obj->size, dxf)
#define FIELD_VECTOR(name, type, size, dxf) FIELD_VECTOR_N(name, type, _obj->size, dxf)
#define FIELD_2RD_VECTOR(name, size, dxf) FIELD_TV(name,dxf)
#define FIELD_2DD_VECTOR(name, size, dxf) FIELD_TV(name,dxf)
#define FIELD_3DPOINT_VECTOR(name, size, dxf) FIELD_TV(name,dxf)
#define HANDLE_VECTOR_N(name, size, code, dxf) \
  for (vcount=0; vcount < (long)size; vcount++) \
    {\
      FIELD_HANDLE_N(name[vcount], vcount, code, dxf);  \
    } \
  if (size) { FIELD_TV(name,dxf) }
#define HANDLE_VECTOR(name, sizefield, code, dxf) \
  HANDLE_VECTOR_N(name, FIELD_VALUE(sizefield), code, dxf)

#define FIELD_INSERT_COUNT(insert_count, type, dxf)
#define FIELD_XDATA(name, size) \
  dwg_free_xdata(_obj, _obj->size)

#define REACTORS(code) \
  for (vcount=0; vcount < (long)obj->tio.object->num_reactors; vcount++) \
    {\
      FIELD_HANDLE_N(reactors[vcount], vcount, code, -5); \
    } \
    FIELD_TV(reactors,0)
#define ENT_REACTORS(code)  \
  for (vcount=0; vcount < obj->tio.entity->num_reactors; vcount++)\
    {\
      FIELD_HANDLE_N(reactors[vcount], vcount, code, -5);  \
    }\
  FIELD_TV(reactors,0)
#define XDICOBJHANDLE(code)\
  SINCE(R_2004)\
    {\
      if (!obj->tio.object->xdic_missing_flag)\
        {\
          FIELD_HANDLE(xdicobjhandle, code, 0); \
        }\
    }\
  PRIOR_VERSIONS\
    {\
      FIELD_HANDLE(xdicobjhandle, code, 0); \
    }
#define ENT_XDICOBJHANDLE(code)\
  SINCE(R_2004)\
    {\
      if (!obj->tio.entity->xdic_missing_flag)\
        {\
          FIELD_HANDLE(xdicobjhandle, code, 0); \
        }\
    }\
  PRIOR_VERSIONS\
    {\
      FIELD_HANDLE(xdicobjhandle, code, 0); \
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

#define END_REPEAT(field) FIELD_TV(field,0)

#define COMMON_ENTITY_HANDLE_DATA \
  SINCE(R_13) {\
    dwg_free_common_entity_handle_data(obj); \
  }
#define SECTION_STRING_STREAM
#define START_STRING_STREAM
#define END_STRING_STREAM
#define START_HANDLE_STREAM

#define DWG_ENTITY(token) \
static void \
dwg_free_ ##token (Dwg_Object * obj)\
{\
  int vcount, rcount, rcount2, rcount3, rcount4;\
  Dwg_Entity_##token *ent, *_obj;\
  Dwg_Object_Entity *_ent;\
  Bit_Chain *hdl_dat = dat;\
  Bit_Chain* str_dat = dat;\
  Dwg_Data* dwg = obj->parent;\
  LOG_HANDLE("Free entity " #token "\n")\
  _ent = obj->tio.entity;\
  _obj = ent = _ent->tio.token;

#define DWG_ENTITY_END \
    free(_obj); obj->tio.entity->tio.UNKNOWN_ENT = NULL; \
    free(obj->tio.entity); obj->tio.object = NULL;\
}

#define DWG_OBJECT(token) \
static void \
dwg_free_ ##token (Dwg_Object * obj) \
{ \
  int vcount, rcount, rcount2, rcount3, rcount4; \
  Dwg_Data* dwg = obj->parent;                   \
  Dwg_Object_##token *_obj;                      \
  Bit_Chain *hdl_dat = dat;                      \
  Bit_Chain* str_dat = dat;                      \
  LOG_HANDLE("Free object " #token " %p\n", obj) \
  _obj = obj->tio.object->tio.token;

#define DWG_OBJECT_END                                  \
  dwg_free_eed(obj);                                    \
  free(_obj); obj->tio.object->tio.UNKNOWN_OBJ = NULL;  \
  free(obj->tio.object); obj->tio.object = NULL;        \
  obj->parent = NULL;                                   \
  /* free(obj); obj = NULL; */                          \
}

static void
dwg_free_handleref(Dwg_Object_Ref *ref, Dwg_Data * dwg)
{
  long unsigned int i;
  if (!dwg) {
    free(ref);
    ref = NULL;
    return;
  }
  for (i=0; i < dwg->num_object_refs; i++)
    {
      if (dwg->object_ref[i] == ref)
        {
          dwg->object_ref[i] = NULL;
          if (ref) { free(ref); ref = NULL; }
        }
    }
}

static void
dwg_free_common_entity_handle_data(Dwg_Object* obj)
{

  Dwg_Data *dwg = obj->parent;
  Dwg_Object_Entity *_obj;
  long unsigned int vcount;
  Dwg_Object_Entity *ent;

  ent = obj->tio.entity;
  _obj = ent;

  #include "common_entity_handle_data.spec"

}

static void
dwg_free_xdata(Dwg_Object_XRECORD *obj, int size)
{
  dwg_free_xdata_resbuf(obj->xdata);
}

static void
dwg_free_eed(Dwg_Object* obj)
{
  unsigned int i;
  if (obj->supertype == DWG_SUPERTYPE_OBJECT) {
    Dwg_Object_Object* _obj = obj->tio.object;
    for (i=0; i < _obj->num_eed; i++) {
      if (_obj->eed[i].size && _obj->eed[i].raw)
        free (_obj->eed[i].raw);
      _obj->eed[i].raw = NULL;
      if (_obj->eed[i].data)
        free (_obj->eed[i].data);
      _obj->eed[i].data = NULL;
    }
    free(_obj->eed);
    _obj->eed = NULL;
  }
  else if (obj->supertype == DWG_SUPERTYPE_ENTITY) {
    Dwg_Object_Entity* _obj = obj->tio.entity;
    for (i=0; i < _obj->num_eed; i++) {
      if (_obj->eed[i].size && _obj->eed[i].raw)
        free (_obj->eed[i].raw);
      _obj->eed[i].raw = NULL;
      if (_obj->eed[i].data)
        free (_obj->eed[i].data);
      _obj->eed[i].data = NULL;
    }
    free(_obj->eed);
    _obj->eed = NULL;
  }
}

#include "dwg.spec"

/* returns 1 if object could be freed and 0 otherwise
 */
static int
dwg_free_variable_type(Dwg_Data * dwg, Dwg_Object* obj)
{
  int i;
  char *dxfname;
  Dwg_Class *klass;

  if ((obj->type - 500) > dwg->num_classes)
    return 0;

  i = obj->type - 500;
  klass = &dwg->dwg_class[i];
  dxfname = strdup(klass->dxfname);

  if (!strcmp(dxfname, "DICTIONARYVAR"))
    {
      dwg_free_DICTIONARYVAR(obj);
      goto known;
    }
  if (!strcmp(dxfname, "ACDBDICTIONARYWDFLT"))
    {
      dwg_free_DICTIONARYWDLFT(obj);
      goto known;
    }
  if (!strcmp(dxfname, "HATCH"))
    {
      dwg_free_HATCH(obj);
      goto known;
    }
  if (!strcmp(dxfname, "FIELDLIST"))
    {
      dwg_free_FIELDLIST(obj);
      goto known;
    }
  if (!strcmp(dxfname, "IDBUFFER"))
    {
      dwg_free_IDBUFFER(obj);
      goto known;
    }
  if (!strcmp(dxfname, "IMAGE"))
    {
      dwg_free_IMAGE(obj);
      goto known;
    }
  if (!strcmp(dxfname, "IMAGEDEF"))
    {
      dwg_free_IMAGEDEF(obj);
      goto known;
    }
  if (!strcmp(dxfname, "IMAGEDEF_REACTOR"))
    {
      dwg_free_IMAGEDEF_REACTOR(obj);
      goto known;
    }
  if (!strcmp(dxfname, "LAYER_INDEX"))
    {
      dwg_free_LAYER_INDEX(obj);
      goto known;
    }
  if (!strcmp(dxfname, "LAYOUT"))
    {
      dwg_free_LAYOUT(obj);
      goto known;
    }
  if (!strcmp(dxfname, "LWPLINE"))
    {
      dwg_free_LWPLINE(obj);
      goto known;
    }
  if (!strcmp(dxfname, "MULTILEADER"))
    {
#ifdef DEBUG_MULTILEADER
      //broken Leader_Line's/Points
      dwg_free_MULTILEADER(obj);
      goto known;
#else
      goto unknown;
#endif
    }
  if (!strcmp(dxfname, "MLEADERSTYLE"))
    {
      dwg_free_MLEADERSTYLE(obj);
      goto known;
    }
  if (!strcmp(dxfname, "OLE2FRAME"))
    {
      dwg_free_OLE2FRAME(obj);
      goto known;
    }
  if (!strcmp(dxfname, "OBJECTCONTEXTDATA")
      || strcmp(klass->cppname, "AcDbObjectContextData"))
    {
      dwg_free_OBJECTCONTEXTDATA(obj);
      goto known;
    }
  if (!strcmp(dxfname, "ACDBPLACEHOLDER"))
    {
      dwg_free_PLACEHOLDER(obj);
      goto known;
    }
  if (!strcmp(dxfname, "PROXY"))
    {
      dwg_free_PROXY(obj);
      goto known;
    }
  if (!strcmp(dxfname, "RASTERVARIABLES"))
    {
      dwg_free_RASTERVARIABLES(obj);
      goto known;
    }
  if (!strcmp(dxfname, "SCALE"))
    {
      dwg_free_SCALE(obj);
      goto known;
    }
  if (!strcmp(dxfname, "SORTENTSTABLE"))
    {
      dwg_free_SORTENTSTABLE(obj);
      goto known;
    }
  if (!strcmp(dxfname, "SPATIAL_FILTER"))
    {
      dwg_free_SPATIAL_FILTER(obj);
      goto known;
    }
  if (!strcmp(dxfname, "SPATIAL_INDEX"))
    {
      dwg_free_SPATIAL_INDEX(obj);
      goto known;
    }
  if (!strcmp(dxfname, "TABLE"))
    {
      dwg_free_TABLE(obj);
      goto known;
    }
  if (!strcmp(dxfname, "WIPEOUTVARIABLE"))
    {
      dwg_free_WIPEOUTVARIABLE(obj);
      goto known;
    }
  if (!strcmp(dxfname, "WIPEOUT"))
    {
      dwg_free_WIPEOUT(obj);
      goto known;
    }
  if (!strcmp(dxfname, "VBA_PROJECT"))
    {
#ifdef DEBUG_VBA_PROJECT
      dwg_free_VBA_PROJECT(obj);
      goto known;
#else
      goto unknown;
#endif
    }
  if (!strcmp(dxfname, "CELLSTYLEMAP"))
    {
#ifdef DEBUG_CELLSTYLEMAP
      dwg_free_CELLSTYLEMAP(obj);
      goto known;
#else
      goto unknown;
#endif
    }
  if (!strcmp(dxfname, "VISUALSTYLE"))
    {
      dwg_free_VISUALSTYLE(obj);
      goto known;
    }
  if (!strcmp(dxfname, "AcDbField")) //?
    {
      dwg_free_FIELD(obj);
      goto known;
    }
  if (!strcmp(dxfname, "TABLECONTENT"))
    {
      dwg_free_TABLECONTENT(obj);
      goto known;
    }
  if (!strcmp(dxfname, "TABLEGEOMETRY"))
    {
      dwg_free_TABLEGEOMETRY(obj);
      goto known;
    }
  if (!strcmp(dxfname, "GEODATA"))
    {
      dwg_free_GEODATA(obj);
      goto known;
    }
  if (!strcmp(dxfname, "XRECORD"))
    {
      dwg_free_XRECORD(obj);
      goto known;
    }
  if (!strcmp(dxfname, "DIMASSOC"))
    {
      //dwg_free_DIMASSOC(obj);
      goto unknown;
    }
  if (!strcmp(dxfname, "MATERIAL"))
    {
      //dwg_free_MATERIAL(obj);
      goto unknown;
    }
 unknown:  
  free(dxfname);
  return 0;

 known:
  free(dxfname);
  return 1;
}

void
dwg_free_object(Dwg_Object *obj)
{
  long unsigned int j;
  Dwg_Data *dwg;

  if (obj && obj->parent) {
    dwg = obj->parent;
    dat->version = dwg->header.version;
  } else
    return;
  if (obj->type == DWG_TYPE_FREED)
    return;
  dat->from_version = dat->version;
  switch (obj->type)
    {
    case DWG_TYPE_TEXT:
      dwg_free_TEXT(obj);
      break;
    case DWG_TYPE_ATTRIB:
      dwg_free_ATTRIB(obj);
      break;
    case DWG_TYPE_ATTDEF:
      dwg_free_ATTDEF(obj);
      break;
    case DWG_TYPE_BLOCK:
      dwg_free_BLOCK(obj);
      break;
    case DWG_TYPE_ENDBLK:
      dwg_free_ENDBLK(obj);
      break;
    case DWG_TYPE_SEQEND:
      dwg_free_SEQEND(obj);
      break;
    case DWG_TYPE_INSERT:
      dwg_free_INSERT(obj);
      break;
    case DWG_TYPE_MINSERT:
      dwg_free_MINSERT(obj);
      break;
    case DWG_TYPE_VERTEX_2D:
      dwg_free_VERTEX_2D(obj);
      break;
    case DWG_TYPE_VERTEX_3D:
      dwg_free_VERTEX_3D(obj);
      break;
    case DWG_TYPE_VERTEX_MESH:
      dwg_free_VERTEX_MESH(obj);
      break;
    case DWG_TYPE_VERTEX_PFACE:
      dwg_free_VERTEX_PFACE(obj);
      break;
    case DWG_TYPE_VERTEX_PFACE_FACE:
      dwg_free_VERTEX_PFACE_FACE(obj);
      break;
    case DWG_TYPE_POLYLINE_2D:
      dwg_free_POLYLINE_2D(obj);
      break;
    case DWG_TYPE_POLYLINE_3D:
      dwg_free_POLYLINE_3D(obj);
      break;
    case DWG_TYPE_ARC:
      dwg_free_ARC(obj);
      break;
    case DWG_TYPE_CIRCLE:
      dwg_free_CIRCLE(obj);
      break;
    case DWG_TYPE_LINE:
      dwg_free_LINE(obj);
      break;
    case DWG_TYPE_DIMENSION_ORDINATE:
      dwg_free_DIMENSION_ORDINATE(obj);
      break;
    case DWG_TYPE_DIMENSION_LINEAR:
      dwg_free_DIMENSION_LINEAR(obj);
      break;
    case DWG_TYPE_DIMENSION_ALIGNED:
      dwg_free_DIMENSION_ALIGNED(obj);
      break;
    case DWG_TYPE_DIMENSION_ANG3PT:
      dwg_free_DIMENSION_ANG3PT(obj);
      break;
    case DWG_TYPE_DIMENSION_ANG2LN:
      dwg_free_DIMENSION_ANG2LN(obj);
      break;
    case DWG_TYPE_DIMENSION_RADIUS:
      dwg_free_DIMENSION_RADIUS(obj);
      break;
    case DWG_TYPE_DIMENSION_DIAMETER:
      dwg_free_DIMENSION_DIAMETER(obj);
      break;
    case DWG_TYPE_POINT:
      dwg_free_POINT(obj);
      break;
    case DWG_TYPE__3DFACE:
      dwg_free__3DFACE(obj);
      break;
    case DWG_TYPE_POLYLINE_PFACE:
      dwg_free_POLYLINE_PFACE(obj);
      break;
    case DWG_TYPE_POLYLINE_MESH:
      dwg_free_POLYLINE_MESH(obj);
      break;
    case DWG_TYPE_SOLID:
      dwg_free_SOLID(obj);
      break;
    case DWG_TYPE_TRACE:
      dwg_free_TRACE(obj);
      break;
    case DWG_TYPE_SHAPE:
      dwg_free_SHAPE(obj);
      break;
    case DWG_TYPE_VIEWPORT:
      dwg_free_VIEWPORT(obj);
      break;
    case DWG_TYPE_ELLIPSE:
      dwg_free_ELLIPSE(obj);
      break;
    case DWG_TYPE_SPLINE:
      dwg_free_SPLINE(obj);
      break;
    case DWG_TYPE_REGION:
      dwg_free_REGION(obj);
      break;
    case DWG_TYPE_3DSOLID:
      dwg_free__3DSOLID(obj);
      break; /* Check the type of the object */
    case DWG_TYPE_BODY:
      dwg_free_BODY(obj);
      break;
    case DWG_TYPE_RAY:
      dwg_free_RAY(obj);
      break;
    case DWG_TYPE_XLINE:
      dwg_free_XLINE(obj);
      break;
    case DWG_TYPE_DICTIONARY:
      dwg_free_DICTIONARY(obj);
      break;
    case DWG_TYPE_MTEXT:
      dwg_free_MTEXT(obj);
      break;
    case DWG_TYPE_LEADER:
      dwg_free_LEADER(obj);
      break;
    case DWG_TYPE_TOLERANCE:
      dwg_free_TOLERANCE(obj);
      break;
    case DWG_TYPE_MLINE:
      dwg_free_MLINE(obj);
      break;
    case DWG_TYPE_BLOCK_CONTROL:
      dwg_free_BLOCK_CONTROL(obj);
      break;
    case DWG_TYPE_BLOCK_HEADER:
      dwg_free_BLOCK_HEADER(obj);
      break;
    case DWG_TYPE_LAYER_CONTROL:
      dwg_free_LAYER_CONTROL(obj);
      break;
    case DWG_TYPE_LAYER:
      dwg_free_LAYER(obj);
      break;
    case DWG_TYPE_STYLE_CONTROL:
      dwg_free_STYLE_CONTROL(obj);
      break;
    case DWG_TYPE_STYLE:
      dwg_free_STYLE(obj);
      break;
    case DWG_TYPE_LTYPE_CONTROL:
      dwg_free_LTYPE_CONTROL(obj);
      break;
    case DWG_TYPE_LTYPE:
      dwg_free_LTYPE(obj);
      break;
    case DWG_TYPE_VIEW_CONTROL:
      dwg_free_VIEW_CONTROL(obj);
      break;
    case DWG_TYPE_VIEW:
      dwg_free_VIEW(obj);
      break;
    case DWG_TYPE_UCS_CONTROL:
      dwg_free_UCS_CONTROL(obj);
      break;
    case DWG_TYPE_UCS:
      dwg_free_UCS(obj);
      break;
    case DWG_TYPE_VPORT_CONTROL:
      dwg_free_VPORT_CONTROL(obj);
      break;
    case DWG_TYPE_VPORT:
      dwg_free_VPORT(obj);
      break;
    case DWG_TYPE_APPID_CONTROL:
      dwg_free_APPID_CONTROL(obj);
      break;
    case DWG_TYPE_APPID:
      dwg_free_APPID(obj);
      break;
    case DWG_TYPE_DIMSTYLE_CONTROL:
      dwg_free_DIMSTYLE_CONTROL(obj);
      break;
    case DWG_TYPE_DIMSTYLE:
      dwg_free_DIMSTYLE(obj);
      break;
    case DWG_TYPE_VPORT_ENTITY_CONTROL:
      dwg_free_VPORT_ENTITY_CONTROL(obj);
      break;
    case DWG_TYPE_VPORT_ENTITY_HEADER:
      dwg_free_VPORT_ENTITY_HEADER(obj);
      break;
    case DWG_TYPE_GROUP:
      dwg_free_GROUP(obj);
      break;
    case DWG_TYPE_MLINESTYLE:
      dwg_free_MLINESTYLE(obj);
      break;
    case DWG_TYPE_OLE2FRAME:
      dwg_free_OLE2FRAME(obj);
      break;
    case DWG_TYPE_DUMMY:
      dwg_free_DUMMY(obj);
      break;
    case DWG_TYPE_LONG_TRANSACTION:
      dwg_free_LONG_TRANSACTION(obj);
      break;
    case DWG_TYPE_LWPLINE:
      dwg_free_LWPLINE(obj);
      break;
    case DWG_TYPE_HATCH:
      dwg_free_HATCH(obj);
      break;
    case DWG_TYPE_XRECORD:
      dwg_free_XRECORD(obj);
      break;
    case DWG_TYPE_PLACEHOLDER:
      dwg_free_PLACEHOLDER(obj);
      break;
    case DWG_TYPE_PROXY_ENTITY:
      dwg_free_PROXY_ENTITY(obj);
      break;
    case DWG_TYPE_OLEFRAME:
      dwg_free_OLEFRAME(obj);
      break;
#ifdef DEBUG_VBA_PROJECT
    case DWG_TYPE_VBA_PROJECT:
      dwg_free_VBA_PROJECT(obj);
      break;
#endif
    case DWG_TYPE_LAYOUT:
      dwg_free_LAYOUT(obj);
      break;
    default:
      if (obj->type == obj->parent->layout_number)
        {
          /*SINCE(R_13) {
            dwg_free_LAYOUT(obj); // avoid double-free, esp. in eed
          }*/
        }

      else if (!dwg_free_variable_type(obj->parent, obj))
        {
          int is_entity;
          int i = obj->type - 500;
          Dwg_Class *klass = NULL;

          dwg = obj->parent;
          if (dwg->dwg_class && i >= 0 && i <= (int)dwg->num_classes)
            {
              klass = &dwg->dwg_class[i];
              is_entity = dwg_class_is_entity(klass);
            }
          if (klass && !is_entity)
            {
              dwg_free_UNKNOWN_OBJ(obj);
            }
          else if (klass)
            {
              dwg_free_UNKNOWN_ENT(obj);
            }
          else // not a class
            {
              free(obj->tio.unknown);
            }
        }
    }
  obj->type = DWG_TYPE_FREED;
}

void
dwg_free(Dwg_Data * dwg)
{
  unsigned int i;
  if (dwg)
    {
      if (dwg->opts)
        loglevel = dwg->opts & 0xf;
#ifdef USE_TRACING
      /* Before starting, set the logging level, but only do so once.  */
      if (! env_var_checked_p)
        {
          char *probe = getenv ("LIBREDWG_TRACE");
          if (probe)
            loglevel = atoi (probe);
          env_var_checked_p = 1;
        }
#endif  /* USE_TRACING */
      LOG_INFO("dwg_free\n")
#define FREE_IF(ptr) { if (ptr) free(ptr); ptr = NULL; }
      // copied table fields have duplicate pointers, but are freed only once
      for (i=0; i < dwg->num_objects; ++i)
        {
          if (!dwg_obj_is_control(&dwg->object[i]))
            dwg_free_object(&dwg->object[i]);
        }
      FREE_IF(dwg->header.section);
      {
        Dwg_Header_Variables* _obj = &dwg->header_vars;
        Dwg_Object* obj = NULL;

        #include "header_variables.spec"
      }
      if (dwg->picture.size && dwg->picture.chain)
        free(dwg->picture.chain);
      if (dwg->num_classes)
        {
          for (i=0; i < dwg->num_classes; ++i)
            {
              FREE_IF(dwg->dwg_class[i].appname);
              FREE_IF(dwg->dwg_class[i].cppname);
              FREE_IF(dwg->dwg_class[i].dxfname);
              if (dwg->header.version >= R_2007)
                FREE_IF(dwg->dwg_class[i].dxfname_u);
            }
          FREE_IF(dwg->dwg_class);
        }
      for (i=0; i < dwg->header.num_infos; ++i)
        FREE_IF(dwg->header.section_info[i].sections);
      if (dwg->header.num_infos)
        FREE_IF(dwg->header.section_info);
      for (i=0; i < dwg->second_header.num_handlers; i++)
        FREE_IF(dwg->second_header.handlers[i].data);
      for (i=0; i < dwg->num_objects; ++i)
        {
          if (dwg_obj_is_control(&dwg->object[i]))
            dwg_free_object(&dwg->object[i]);
        }
      for (i=0; i < dwg->num_object_refs; ++i)
        FREE_IF(dwg->object_ref[i]);
      FREE_IF(dwg->object_ref);
      FREE_IF(dwg->object);
#undef FREE_IF
    }
}

#undef IS_FREE
