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
#ifdef __STDC_ALLOC_LIB__
#define __STDC_WANT_LIB_EXT2__ 1 /* for strdup */
#endif
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

/* from dwg.c */
int dwg_obj_is_control(const Dwg_Object *obj);

/*--------------------------------------------------------------------------------
 * MACROS
 */

#define IS_FREE

#define FREE_IF(ptr) { if (ptr) free(ptr); ptr = NULL; }

#define VALUE(value,type,dxf)
#define VALUE_RC(value,dxf) VALUE(value, RC, dxf)
#define VALUE_RS(value,dxf) VALUE(value, RS, dxf)
#define VALUE_RL(value,dxf) VALUE(value, RL, dxf)
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

#define FIELD_NUM_INSERTS(num_inserts, type, dxf)
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
static int \
dwg_free_ ##token (Dwg_Object * obj)\
{\
  int error = 0; \
  long vcount, rcount1, rcount2, rcount3, rcount4;\
  Dwg_Entity_##token *ent, *_obj;\
  Dwg_Object_Entity *_ent;\
  Bit_Chain *hdl_dat = dat;\
  Bit_Chain* str_dat = dat;\
  Dwg_Data* dwg = obj->parent;\
  LOG_HANDLE("Free entity " #token "\n")\
  _ent = obj->tio.entity;\
  _obj = ent = _ent->tio.token;

#define DWG_ENTITY_END      \
  FREE_IF(_obj);            \
  FREE_IF(obj->tio.entity); \
  return error; \
}

#define DWG_OBJECT(token) \
static int \
dwg_free_ ##token (Dwg_Object * obj) \
{ \
  int error = 0; \
  long vcount, rcount1, rcount2, rcount3, rcount4; \
  Dwg_Data* dwg = obj->parent;                   \
  Dwg_Object_##token *_obj;                      \
  Bit_Chain *hdl_dat = dat;                      \
  Bit_Chain* str_dat = dat;                      \
  LOG_HANDLE("Free object " #token " %p\n", obj) \
  _obj = obj->tio.object->tio.token;

#define DWG_OBJECT_END                                  \
  dwg_free_eed(obj);                                    \
  FREE_IF(_obj);                                        \
  FREE_IF(obj->tio.object);                             \
  obj->parent = NULL;                                   \
  /* free(obj); obj = NULL; */                          \
  return error; \
}

static void
dwg_free_handleref(Dwg_Object_Ref *restrict ref, Dwg_Data *restrict dwg)
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

  i = obj->type - 500;
  if (i < 0 || i > (int)dwg->num_classes)
    return DWG_ERR_INVALIDTYPE;

  klass = &dwg->dwg_class[i];
  if (!klass || !klass->dxfname)
    return DWG_ERR_INTERNALERROR;
  dxfname = strdup(klass->dxfname);
  if (!dxfname)
    return DWG_ERR_OUTOFMEM;

  if (!strcmp(dxfname, "DICTIONARYVAR"))
    {
      free(dxfname);
      return dwg_free_DICTIONARYVAR(obj);
    }
  if (!strcmp(dxfname, "ACDBDICTIONARYWDFLT"))
    {
      free(dxfname);
      return dwg_free_DICTIONARYWDLFT(obj);
    }
  if (!strcmp(dxfname, "HATCH"))
    {
      free(dxfname);
      return dwg_free_HATCH(obj);
    }
  if (!strcmp(dxfname, "FIELDLIST"))
    {
      free(dxfname);
      return dwg_free_FIELDLIST(obj);
    }
  if (!strcmp(dxfname, "IDBUFFER"))
    {
      free(dxfname);
      return dwg_free_IDBUFFER(obj);
    }
  if (!strcmp(dxfname, "IMAGE"))
    {
      free(dxfname);
      return dwg_free_IMAGE(obj);
    }
  if (!strcmp(dxfname, "IMAGEDEF"))
    {
      free(dxfname);
      return dwg_free_IMAGEDEF(obj);
    }
  if (!strcmp(dxfname, "IMAGEDEF_REACTOR"))
    {
      free(dxfname);
      return dwg_free_IMAGEDEF_REACTOR(obj);
    }
  if (!strcmp(dxfname, "LAYER_INDEX"))
    {
      free(dxfname);
      return dwg_free_LAYER_INDEX(obj);
    }
  if (!strcmp(dxfname, "LAYOUT"))
    {
      free(dxfname);
      return dwg_free_LAYOUT(obj);
    }
  if (!strcmp(dxfname, "LWPOLYLINE"))
    {
      free(dxfname);
      return dwg_free_LWPOLYLINE(obj);
    }
  if (!strcmp(dxfname, "MULTILEADER"))
    {
#ifdef DEBUG_MULTILEADER
      //broken Leader_Line's/Points
      free(dxfname);
      return dwg_free_MULTILEADER(obj);
#else
      return DWG_ERR_UNHANDLEDCLASS;
#endif
    }
  if (!strcmp(dxfname, "MLEADERSTYLE"))
    {
      free(dxfname);
      return dwg_free_MLEADERSTYLE(obj);
    }
  if (!strcmp(dxfname, "OLE2FRAME"))
    {
      free(dxfname);
      return dwg_free_OLE2FRAME(obj);
    }
  if (!strcmp(dxfname, "OBJECTCONTEXTDATA") ||
      !strcmp(klass->cppname, "AcDbObjectContextData"))
    {
      free(dxfname);
      return dwg_free_OBJECTCONTEXTDATA(obj);
    }
  if (!strcmp(dxfname, "OBJECT_PTR") ||
      !strcmp(klass->cppname, "CAseDLPNTableRecord"))
    {
      free(dxfname);
      return dwg_free_OBJECT_PTR(obj);
    }
  if (!strcmp(dxfname, "ACDBPLACEHOLDER"))
    {
      free(dxfname);
      return dwg_free_PLACEHOLDER(obj);
    }
  if (!strcmp(dxfname, "PROXY"))
    {
      free(dxfname);
      return dwg_free_PROXY_OBJECT(obj);
    }
  if (!strcmp(dxfname, "RASTERVARIABLES"))
    {
      free(dxfname);
      return dwg_free_RASTERVARIABLES(obj);
    }
  if (!strcmp(dxfname, "SCALE"))
    {
      free(dxfname);
      return dwg_free_SCALE(obj);
    }
  if (!strcmp(dxfname, "SORTENTSTABLE"))
    {
      free(dxfname);
      return dwg_free_SORTENTSTABLE(obj);
    }
  if (!strcmp(dxfname, "SPATIAL_FILTER"))
    {
      free(dxfname);
      return dwg_free_SPATIAL_FILTER(obj);
    }
  if (!strcmp(dxfname, "SPATIAL_INDEX"))
    {
      free(dxfname);
      return dwg_free_SPATIAL_INDEX(obj);
    }
  if (!strcmp(dxfname, "TABLE") || 
      !strcmp(dxfname, "ACAD_TABLE"))
    {
      free(dxfname);
      return dwg_free_TABLE(obj);
    }
  if (!strcmp(dxfname, "WIPEOUTVARIABLES"))
    {
      free(dxfname);
      return dwg_free_WIPEOUTVARIABLES(obj);
    }
  if (!strcmp(dxfname, "WIPEOUT"))
    {
      free(dxfname);
      return dwg_free_WIPEOUT(obj);
    }
  if (!strcmp(dxfname, "VBA_PROJECT"))
    {
#ifdef DEBUG_VBA_PROJECT
      free(dxfname);
      return dwg_free_VBA_PROJECT(obj);
#else
      return DWG_ERR_UNHANDLEDCLASS;
#endif
    }
  if (!strcmp(dxfname, "CELLSTYLEMAP"))
    {
#ifdef DEBUG_CELLSTYLEMAP
      free(dxfname);
      return dwg_free_CELLSTYLEMAP(obj);
#else
      return DWG_ERR_UNHANDLEDCLASS;
#endif
    }
  if (!strcmp(dxfname, "VISUALSTYLE"))
    {
      free(dxfname);
      return dwg_free_VISUALSTYLE(obj);
    }
  if (!strcmp(dxfname, "ACDBSECTIONVIEWSTYLE"))
    {
      free(dxfname);
      //return dwg_free_SECTIONVIEWSTYLE(obj);
      return DWG_ERR_UNHANDLEDCLASS;
    }
  if (!strcmp(dxfname, "ACDBDETAILVIEWSTYLE"))
    {
      free(dxfname);
      //return dwg_free_DETAILVIEWSTYLE(obj);
      return DWG_ERR_UNHANDLEDCLASS;
    }
  if (!strcmp(dxfname, "AcDbField")) //?
    {
      free(dxfname);
      return dwg_free_FIELD(obj);
    }
  if (!strcmp(dxfname, "TABLECONTENT"))
    {
      free(dxfname);
      return dwg_free_TABLECONTENT(obj);
    }
  if (!strcmp(dxfname, "TABLEGEOMETRY"))
    {
      free(dxfname);
      return dwg_free_TABLEGEOMETRY(obj);
    }
  if (!strcmp(dxfname, "GEODATA"))
    {
      free(dxfname);
      return dwg_free_GEODATA(obj);
    }
  if (!strcmp(dxfname, "XRECORD"))
    {
      free(dxfname);
      return dwg_free_XRECORD(obj);
    }
  if (!strcmp(dxfname, "DIMASSOC"))
    {
      free(dxfname);
      //return dwg_free_DIMASSOC(obj);
      return DWG_ERR_UNHANDLEDCLASS;
    }
  if (!strcmp(dxfname, "MATERIAL"))
    {
#ifdef DEBUG_MATERIAL
      free(dxfname);
      return dwg_free_MATERIAL(obj);
#else
      return DWG_ERR_UNHANDLEDCLASS;
#endif
    }
  if (!strcmp(dxfname, "PLOTSETTINGS"))
    {
#ifdef DEBUG_PLOTSETTINGS
      free(dxfname);
      return dwg_free_PLOTSETTINGS(obj);
#else
      return DWG_ERR_UNHANDLEDCLASS;
#endif
    }
  if (!strcmp(dxfname, "LIGHT"))
    {
#ifdef DEBUG_LIGHT
      free(dxfname);
      return dwg_free_LIGHT(obj);
#else
      return DWG_ERR_UNHANDLEDCLASS;
#endif
    }
  if (!strcmp(dxfname, "SUN"))
    {
#ifdef DEBUG_SUN
      free(dxfname);
      return dwg_free_SUN(obj);
#else
      return DWG_ERR_UNHANDLEDCLASS;
#endif
    }
  if (!strcmp(dxfname, "GEOPOSITIONMARKER"))
    {
#ifdef DEBUG_GEOPOSITIONMARKER
      free(dxfname);
      return dwg_free_GEOPOSITIONMARKER(obj);
#else
      return DWG_ERR_UNHANDLEDCLASS;
#endif
    }
  if (!strcmp(dxfname, "EXTRUDEDSURFACE"))
    {
      free(dxfname);
#ifdef DEBUG_EXTRUDEDSURFACE
      return dwg_free_EXTRUDEDSURFACE(obj);
#else
      return DWG_ERR_UNHANDLEDCLASS;
#endif
    }
  if (!strcmp(dxfname, "DATATABLE"))
    {
      free(dxfname);
#ifdef DEBUG_DATATABLE
      return dwg_free_DATATABLE(obj);
#else
      return DWG_ERR_UNHANDLEDCLASS;
#endif
    }

  return DWG_ERR_UNHANDLEDCLASS;
}

void
dwg_free_object(Dwg_Object *obj)
{
  int error = 0;
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
    case DWG_TYPE__3DSOLID:
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
    case DWG_TYPE_LWPOLYLINE:
      dwg_free_LWPOLYLINE(obj);
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

      else if ((error = dwg_free_variable_type(obj->parent, obj))
               & DWG_ERR_UNHANDLEDCLASS)
        {
          int is_entity;
          int i = obj->type - 500;
          Dwg_Class *klass = NULL;

          dwg = obj->parent;
          if (dwg->dwg_class && i >= 0 && i < (int)dwg->num_classes)
            {
              klass = &dwg->dwg_class[i];
              is_entity = klass ? dwg_class_is_entity(klass) : 0;
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

static int dwg_free_header_vars(Dwg_Data * dwg)
{
  Dwg_Header_Variables* _obj = &dwg->header_vars;
  Dwg_Object* obj = NULL;
  #include "header_variables.spec"
  return 0;
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
      // copied table fields have duplicate pointers, but are freed only once
      for (i=0; i < dwg->num_objects; ++i)
        {
          if (!dwg_obj_is_control(&dwg->object[i]))
            dwg_free_object(&dwg->object[i]);
        }
      FREE_IF(dwg->header.section);
      dwg_free_header_vars(dwg);
      if (dwg->picture.size && dwg->picture.chain)
        free(dwg->picture.chain);
      if (dwg->num_classes)
        {
          for (i=0; i < (int)dwg->num_classes; ++i)
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
