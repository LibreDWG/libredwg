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
#include "free.h"

#define DWG_LOGLEVEL DWG_LOGLEVEL_TRACE
#include "logging.h"

/* the current version per spec block */
static unsigned int cur_ver = 0;
static Bit_Chain pdat = {NULL,0,0,0,0,0};
static Bit_Chain *dat = &pdat;

/*--------------------------------------------------------------------------------
 * MACROS
 */

#define IS_FREE

#define FIELD(name,type)
#define FIELD_TRACE(name,type)\
  LOG_TRACE(#name ": " FORMAT_##type "\n", _obj->name)
#define FIELD_CAST(name,type,cast)\
  FIELD_TRACE(name,cast)
#define FIELD_VALUE(name) _obj->name

#define ANYCODE -1
#define FIELD_HANDLE(name, handle_code)
#define FIELD_HANDLE_N(name, vcount, handle_code)

#define FIELD_B(name)
#define FIELD_BB(name) 
#define FIELD_3B(name) 
#define FIELD_BS(name) 
#define FIELD_BL(name) 
#define FIELD_BLL(name)
#define FIELD_BD(name) 
#define FIELD_RC(name) 
#define FIELD_RS(name) 
#define FIELD_RD(name) 
#define FIELD_RL(name) 
#define FIELD_RLL(name)
#define FIELD_MC(name)
#define FIELD_MS(name)
#define FIELD_TV(name) \
  if (FIELD_VALUE(name))\
    {\
      free (FIELD_VALUE(name)); \
      FIELD_VALUE(name) = NULL; \
    }

#define FIELD_T FIELD_TV /*TODO: implement version dependant string fields */
#define FIELD_BT(name)
#define FIELD_4BITS(name)
#define FIELD_BE(name)
#define FIELD_DD(name, _default)
#define FIELD_2DD(name, d1, d2)
#define FIELD_2RD(name)
#define FIELD_2BD(name)
#define FIELD_3RD(name)
#define FIELD_3BD(name)
#define FIELD_3DPOINT(name)
#define FIELD_CMC(token)\
	FIELD_TV(token.name); \
	FIELD_TV(token.book_name);

//FIELD_VECTOR_N(name, type, size):
// reads data of the type indicated by 'type' 'size' times and stores
// it all in the vector called 'name'.
#define FIELD_VECTOR_N(name, type, size) FIELD_TV(name)
#define FIELD_VECTOR(name, type, size) FIELD_TV(name)
#define FIELD_2RD_VECTOR(name, size) FIELD_TV(name)
#define FIELD_2DD_VECTOR(name, size) FIELD_TV(name)
#define FIELD_3DPOINT_VECTOR(name, size) FIELD_TV(name)
#define HANDLE_VECTOR_N(name, size, code) FIELD_TV(name)
#define HANDLE_VECTOR(name, sizefield, code) FIELD_TV(name)
#define FIELD_INSERT_COUNT(insert_count, type)
#define FIELD_XDATA(name, size)

#define REACTORS(code) FIELD_TV(reactors)
#define ENT_REACTORS(code) FIELD_TV(reactors)
#define XDICOBJHANDLE(code)\
  SINCE(R_2004)\
    {\
      if (!obj->tio.object->xdic_missing_flag)\
        {\
          FIELD_HANDLE(xdicobjhandle, code);  \
        }\
    }\
  PRIOR_VERSIONS\
    {\
      FIELD_HANDLE(xdicobjhandle, code);      \
    }
#define ENT_XDICOBJHANDLE(code)\
  SINCE(R_2004)\
    {\
      if (!obj->tio.entity->xdic_missing_flag)\
        {\
          FIELD_HANDLE(xdicobjhandle, code);  \
        }\
    }\
  PRIOR_VERSIONS\
    {\
      FIELD_HANDLE(xdicobjhandle, code);      \
    }

#define REPEAT_N(times, name, type) \
  for (rcount=0; rcount<(int)times; rcount++)

#define REPEAT(times, name, type) \
  for (rcount=0; rcount<(int)_obj->times; rcount++)

#define REPEAT2(times, name, type) \
  for (rcount2=0; rcount2<(int)_obj->times; rcount2++)

#define REPEAT3(times, name, type) \
  for (rcount3=0; rcount3<(int)_obj->times; rcount3++)

#define COMMON_ENTITY_HANDLE_DATA
//TODO num_eed and reactors

#define DWG_ENTITY(token) \
static void \
dwg_free_ ##token (Dwg_Object * obj)\
{\
  int vcount, rcount, rcount2, rcount3;\
  Dwg_Entity_##token *ent, *_obj;\
  LOG_HANDLE("Free entity " #token "\n")\
  ent = obj->tio.entity->tio.token;\
  _obj = ent;

#define DWG_ENTITY_END \
    free(_obj);\
    free(obj->tio.entity);\
}

#define DWG_OBJECT(token) \
static void \
dwg_free_ ##token (Dwg_Object * obj) \
{ \
  int vcount, rcount, rcount2, rcount3;\
  Dwg_Object_##token *_obj;\
  LOG_HANDLE("Free object " #token "\n")\
  _obj = obj->tio.object->tio.token;

#define DWG_OBJECT_END \
    free(_obj);\
    free(obj->tio.object);\
}

#include "dwg.spec"

/* returns 1 if object could be freed and 0 otherwise
 */
static int
dwg_free_variable_type(Dwg_Data * dwg, Dwg_Object* obj)
{
  int i;

  if ((obj->type - 500) > dwg->num_classes)
    return 0;

  i = obj->type - 500;

  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "DICTIONARYVAR"))
    {
      dwg_free_DICTIONARYVAR(obj);
      return 1;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "ACDBDICTIONARYWDFLT"))
    {
      dwg_free_DICTIONARYWDLFT(obj);
      return 1;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "HATCH"))
    {
      dwg_free_HATCH(obj);
      return 1;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "FIELDLIST"))
    {
      dwg_free_FIELDLIST(obj);
      return 1;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "IDBUFFER"))
    {
      dwg_free_IDBUFFER(obj);
      return 1;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "IMAGE"))
    {
      dwg_free_IMAGE(obj);
      return 1;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "IMAGEDEF"))
    {
      dwg_free_IMAGEDEF(obj);
      return 1;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "IMAGEDEF_REACTOR"))
    {
      dwg_free_IMAGEDEF_REACTOR(obj);
      return 1;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "LAYER_INDEX"))
    {
      dwg_free_LAYER_INDEX(obj);
      return 1;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "LAYOUT"))
    {
      dwg_free_LAYOUT(obj);
      return 1;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "LWPLINE"))
    {
      dwg_free_LWPLINE(obj);
      return 1;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "MLEADERSTYLE"))
    {
      //dwg_free_MLEADERSTYLE(obj);
      return 0;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "OLE2FRAME"))
    {
      dwg_free_OLE2FRAME(obj);
      return 1;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "ACDBPLACEHOLDER"))
    {
      dwg_free_PLACEHOLDER(obj);
      return 1;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "PROXY"))
    {
      dwg_free_PROXY(obj);
      return 1;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "RASTERVARIABLES"))
    {
      dwg_free_RASTERVARIABLES(obj);
      return 1;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "SCALE"))
    {
      dwg_free_SCALE(obj);
      return 1;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "SORTENTSTABLE"))
    {
      dwg_free_SORTENTSTABLE(obj);
      return 1;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "SPATIAL_FILTER"))
    {
      dwg_free_SPATIAL_FILTER(obj);
      return 1;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "SPATIAL_INDEX"))
    {
      dwg_free_SPATIAL_INDEX(obj);
      return 1;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "TABLE"))
    {
      dwg_free_TABLE(obj);
      return 1;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "WIPEOUTVARIABLE"))
    {
      //dwg_free_WIPEOUTVARIABLE(obj);
      return 0;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "WIPEOUT"))
    {
      dwg_free_WIPEOUT(obj);
      return 1;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "VBA_PROJECT"))
    {
      dwg_free_VBA_PROJECT(obj);
      return 0;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "CELLSTYLEMAP"))
    {
      dwg_free_CELLSTYLEMAP(obj);
      return 0;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "VISUALSTYLE"))
    {
      //dwg_free_VISUALSTYLE(obj);
      return 0;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "AcDbField")) //?
    {
      dwg_free_FIELD(obj);
      return 1;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "XRECORD"))
    {
      dwg_free_XRECORD(obj);
      return 1;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "DIMASSOC"))
    {
//TODO:      dwg_free_DIMASSOC(obj);
      return 0;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "MATERIAL"))
    {
//TODO:      dwg_free_MATERIAL(obj);
      return 0;
    }

  return 0;
}

void
dwg_free_object(Dwg_Object *obj)
{
  dat->version = obj->parent->header.version;
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
      break; /* Check the type of the object
              */
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
    case DWG_TYPE_SHAPEFILE_CONTROL:
      dwg_free_SHAPEFILE_CONTROL(obj);
      break;
    case DWG_TYPE_SHAPEFILE:
      dwg_free_SHAPEFILE(obj);
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
    case DWG_TYPE_VP_ENT_HDR_CONTROL:
      dwg_free_VP_ENT_HDR_CONTROL(obj);
      break;
    case DWG_TYPE_VP_ENT_HDR:
      dwg_free_VP_ENT_HDR(obj);
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
    case DWG_TYPE_VBA_PROJECT:
      //dwg_free_VBA_PROJECT(obj);
      break;
    case DWG_TYPE_LAYOUT:
      dwg_free_LAYOUT(obj);
      break;
    default:
      if (obj->type == obj->parent->layout_number)
        {
          dwg_free_LAYOUT(obj);
        }
      /* > 500:
         TABLE, DICTIONARYWDLFT, IDBUFFER, IMAGE, IMAGEDEF, IMAGEDEF_REACTOR,
         LAYER_INDEX, OLE2FRAME, PROXY, RASTERVARIABLES, SORTENTSTABLE, SPATIAL_FILTER,
         SPATIAL_INDEX
      */
      else if (!dwg_free_variable_type(obj->parent, obj))
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
}

void
dwg_free(Dwg_Data * dwg)
{
  unsigned int i;
  if (dwg)
    {
      LOG_TRACE("dwg_free %p\n", dwg)
      /*if (dwg->bit_chain && dwg->bit_chain->size)
        free (dwg->bit_chain->chain);*/
#define FREE_IF(ptr) if (ptr) free(ptr)
      for (i=0; i < dwg->num_objects; ++i)
        {
          dwg_free_object(&dwg->object[i]);
        }
      FREE_IF(dwg->header.section);
      if (dwg->picture.size && dwg->picture.chain)
        free(dwg->picture.chain);
      for (i=0; i < dwg->num_classes; ++i)
        {
          FREE_IF(dwg->dwg_class[i].appname);
          FREE_IF(dwg->dwg_class[i].cppname);
          FREE_IF(dwg->dwg_class[i].dxfname);
        }
      if (dwg->num_classes) {
        FREE_IF(dwg->dwg_class);
      }
      for (i=0; i < dwg->header.num_descriptions; ++i)
        {
          FREE_IF(dwg->header.section_info[i].sections);
        }
      if (dwg->header.num_descriptions) {
        FREE_IF(dwg->header.section_info);
      }
      FREE_IF(dwg->object_ref);
      FREE_IF(dwg->object);
#undef FREE_IF
    }
}

#undef IS_FREE
