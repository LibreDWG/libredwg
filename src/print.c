/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2010 Free Software Foundation, Inc.                        */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * print.c: print helper functions
 * written by Rodrigo Rodrigues da Silva
 */

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "common.h"
#include "bits.h"
#include "dwg.h"
#include "print.h"

#define DWG_LOGLEVEL DWG_LOGLEVEL_TRACE
#include "logging.h"

/* the current version per spec block */
static unsigned int cur_ver = 0;

/*--------------------------------------------------------------------------------
 * MACROS
 */

#define IS_PRINT

#define FIELD(name,type)\
  FIELD_TRACE(name,type)
#define FIELD_TRACE(name,type)\
  LOG_TRACE(#name ": " FORMAT_##type "\n", _obj->name)
#define FIELD_CAST(name,type,cast)\
  FIELD_TRACE(name,cast)

#define FIELD_VALUE(name) _obj->name

#define ANYCODE -1
#define FIELD_HANDLE(name, handle_code)\
  LOG_TRACE(#name ": HANDLE(%d.%d.%lu) absolute:%lu\n",\
        _obj->name->handleref.code,\
        _obj->name->handleref.size,\
        _obj->name->handleref.value,\
        _obj->name->absolute_ref)
#define FIELD_HANDLE_N(name, vcount, handle_code)\
  LOG_TRACE(#name "[%d]: HANDLE(%d.%d.%lu) absolute:%lu\n",\
        (int)vcount,\
        _obj->name->handleref.code,\
        _obj->name->handleref.size,\
        _obj->name->handleref.value,\
        _obj->name->absolute_ref)

#define FIELD_B(name) FIELD(name, B);
#define FIELD_BB(name) FIELD(name, BB);
#define FIELD_3B(name) FIELD(name, 3B);
#define FIELD_BS(name) FIELD(name, BS);
#define FIELD_BL(name) FIELD(name, BL);
#define FIELD_BLL(name) FIELD(name, BLL);
#define FIELD_BD(name) FIELD(name, BD);
#define FIELD_RC(name) FIELD(name, RC);
#define FIELD_RS(name) FIELD(name, RS);
#define FIELD_RD(name) FIELD(name, RD);
#define FIELD_RL(name) FIELD(name, RL);
#define FIELD_RLL(name) FIELD(name, RLL);
#define FIELD_MC(name) FIELD(name, MC);
#define FIELD_MS(name) FIELD(name, MS);
#define FIELD_TV(name) FIELD(name, TV);
#define FIELD_T FIELD_TV /*TODO: implement version dependant string fields */
#define FIELD_BT(name) FIELD(name, BT);
#define FIELD_4BITS(name) FIELD_TRACE(name,4BITS)
#define FIELD_BE(name) FIELD_3RD(name)
#define FIELD_DD(name, _default) \
  LOG_TRACE(#name ": " FORMAT_DD ", default: " FORMAT_DD "\n", _obj->name, _default)
#define FIELD_2DD(name, d1, d2) FIELD_DD(name.x, d1); FIELD_DD(name.y, d2);
#define FIELD_2RD(name) FIELD(name.x, RD); FIELD(name.y, RD);
#define FIELD_2BD(name) FIELD(name.x, BD); FIELD(name.y, BD);
#define FIELD_3RD(name) FIELD(name.x, RD); FIELD(name.y, RD); FIELD(name.z, RD);
#define FIELD_3BD(name) FIELD(name.x, BD); FIELD(name.y, BD); FIELD(name.z, BD);
#define FIELD_3DPOINT(name) FIELD_3BD(name)
#define FIELD_CMC(name)\
    LOG_TRACE(#name ": index %d\n", _obj->name.index)

//FIELD_VECTOR_N(name, type, size):
// reads data of the type indicated by 'type' 'size' times and stores
// it all in the vector called 'name'.
#define FIELD_VECTOR_N(name, type, size)\
  if (size>0)\
    {\
      for (vcount=0; vcount < (int)size; vcount++)\
        {\
          LOG_TRACE(#name "[%d]: " FORMAT_##type "\n", vcount, _obj->name[vcount])\
        }\
    }

#define FIELD_VECTOR(name, type, size) FIELD_VECTOR_N(name, type, _obj->size)

#define FIELD_2RD_VECTOR(name, size)\
  for (vcount=0; vcount < (int)_obj->size; vcount++)\
    {\
      FIELD_2RD(name[vcount]);\
    }

#define FIELD_2DD_VECTOR(name, size)\
  FIELD_2RD(name[0]);\
  for (vcount = 1; vcount < (int)_obj->size; vcount++)\
    {\
      FIELD_2DD(name[vcount], FIELD_VALUE(name[vcount - 1].x), FIELD_VALUE(name[vcount - 1].y));\
    }

#define FIELD_3DPOINT_VECTOR(name, size)\
  for (vcount=0; vcount < (int)_obj->size; vcount++)\
    {\
      FIELD_3DPOINT(name[vcount]);\
    }

#define HANDLE_VECTOR_N(name, size, code)\
  for (vcount=0; vcount < (int)size; vcount++)\
    {\
      FIELD_HANDLE_N(name[vcount], vcount, code);\
    }

#define HANDLE_VECTOR(name, sizefield, code) \
  HANDLE_VECTOR_N(name, FIELD_VALUE(sizefield), code)

#define FIELD_INSERT_COUNT(insert_count, type)   \
      FIELD_TRACE(insert_count, type)

#define FIELD_XDATA(name, size)

#define REACTORS(code)\
  for (vcount=0; vcount < (int)obj->tio.object->num_reactors; vcount++)\
    {\
      FIELD_HANDLE_N(reactors[vcount], vcount, code);\
    }

#define XDICOBJHANDLE(code)\
  SINCE(R_2004)\
    {\
      if (!obj->tio.object->xdic_missing_flag)\
        FIELD_HANDLE(xdicobjhandle, code);\
    }\
  PRIOR_VERSIONS\
    {\
      FIELD_HANDLE(xdicobjhandle, code);\
    }

#define REPEAT_N(times, name, type) \
  for (rcount=0; rcount<(int)times; rcount++)

#define REPEAT(times, name, type) \
  for (rcount=0; rcount<(int)_obj->times; rcount++)

#define REPEAT2(times, name, type) \
  for (rcount2=0; rcount2<(int)_obj->times; rcount2++)

#define REPEAT3(times, name, type) \
  for (rcount3=0; rcount3<(int)_obj->times; rcount3++)

#define COMMON_ENTITY_HANDLE_DATA /*  Empty */

#define DWG_ENTITY(token) \
static void \
dwg_print_##token (Bit_Chain * dat, Dwg_Object * obj)\
{\
  int vcount, rcount, rcount2, rcount3;\
  Dwg_Entity_##token *ent, *_obj;\
  LOG_INFO("Entity " #token ":\n")\
  ent = obj->tio.entity->tio.token;\
  _obj=ent;\
  LOG_INFO("Entity handle: %d.%d.%lu\n",\
    obj->handle.code,\
    obj->handle.size,\
    obj->handle.value)

#define DWG_ENTITY_END }

#define DWG_OBJECT(token) \
static void \
dwg_print_ ##token (Bit_Chain * dat, Dwg_Object * obj) \
{ \
  int vcount, rcount, rcount2, rcount3;\
  Dwg_Object_##token *_obj;\
  LOG_INFO("Object " #token ":\n")\
  _obj = obj->tio.object->tio.token;\
  LOG_INFO("Object handle: %d.%d.%lu\n",\
    obj->handle.code,\
    obj->handle.size,\
    obj->handle.value)

#define DWG_OBJECT_END }

#include "dwg.spec"

/* returns 1 if object could be printd and 0 otherwise
 */
static int
dwg_print_variable_type(Dwg_Data * dwg, Bit_Chain * dat, Dwg_Object* obj)
{
  int i;
  char *dxfname;
  Dwg_Class *klass;

  if ((obj->type - 500) > dwg->num_classes)
    return 0;

  i = obj->type - 500;
  klass = &dwg->dwg_class[i];
  dxfname = klass->dxfname;

  if (!strcmp(dxfname, "DICTIONARYVAR"))
    {
      dwg_print_DICTIONARYVAR(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "ACDBDICTIONARYWDFLT"))
    {
      dwg_print_DICTIONARYWDLFT(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "HATCH"))
    {
      dwg_print_HATCH(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "FIELDLIST"))
    {
      dwg_print_FIELDLIST(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "IDBUFFER"))
    {
      dwg_print_IDBUFFER(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "IMAGE"))
    {
      dwg_print_IMAGE(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "IMAGEDEF"))
    {
      dwg_print_IMAGEDEF(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "IMAGEDEF_REACTOR"))
    {
      dwg_print_IMAGEDEF_REACTOR(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "LAYER_INDEX"))
    {
      dwg_print_LAYER_INDEX(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "LAYOUT"))
    {
      dwg_print_LAYOUT(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "LWPLINE"))
    {
      dwg_print_LWPLINE(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "MLEADERSTYLE"))
    {
      //dwg_print_MLEADERSTYLE(dat, obj);
      return 0;
    }
  if (!strcmp(dxfname, "OLE2FRAME"))
    {
      dwg_print_OLE2FRAME(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "OBJECTCONTEXTDATA")
      || strcmp(klass->cppname, "AcDbObjectContextData"))
    {
      dwg_print_OBJECTCONTEXTDATA(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "ACDBPLACEHOLDER"))
    {
      dwg_print_PLACEHOLDER(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "PROXY"))
    {
      dwg_print_PROXY(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "RASTERVARIABLES"))
    {
      dwg_print_RASTERVARIABLES(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "SCALE"))
    {
      dwg_print_SCALE(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "SORTENTSTABLE"))
    {
      dwg_print_SORTENTSTABLE(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "SPATIAL_FILTER"))
    {
      dwg_print_SPATIAL_FILTER(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "SPATIAL_INDEX"))
    {
      dwg_print_SPATIAL_INDEX(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "TABLE"))
    {
      dwg_print_TABLE(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "WIPEOUTVARIABLE"))
    {
      // TODO
      LOG_WARN("Unhandled Object/Class %s\n", dwg->dwg_class[i].dxfname);
      //dwg_print_WIPEOUTVARIABLE(dat, obj);
      return 0;
    }
  if (!strcmp(dxfname, "WIPEOUT"))
    {
      // TODO
      //LOG_WARN("Unhandled Object/Class %s\n", dwg->dwg_class[i].dxfname);
      dwg_print_WIPEOUT(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "VBA_PROJECT"))
    {
      LOG_ERROR("Unhandled Object VBA_PROJECT. Has its own section?\n");
      dwg_print_VBA_PROJECT(dat, obj);
      return 0;
    }
  if (!strcmp(dxfname, "CELLSTYLEMAP"))
    {
      // TODO
      LOG_WARN("Unhandled Object/Class %s\n", dwg->dwg_class[i].dxfname);
      dwg_print_CELLSTYLEMAP(dat, obj);
      return 0;
    }
  if (!strcmp(dxfname, "VISUALSTYLE"))
    {
      // TODO
      LOG_WARN("Unhandled Object/Class %s\n", dwg->dwg_class[i].dxfname);
      //dwg_print_VISUALSTYLE(dat, obj);
      return 0;
    }
  if (!strcmp(dxfname, "AcDbField")) //?
    {
      // TODO
      LOG_WARN("Untested Object/Class %s\n", dwg->dwg_class[i].dxfname);
      dwg_print_FIELD(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "XRECORD"))
    {
      dwg_print_XRECORD(dat, obj);
      return 1;
    }
  if (!strcmp(dxfname, "DIMASSOC"))
    {
//TODO:      dwg_print_DIMASSOC(dat, obj);
      return 0;
    }
  if (!strcmp(dxfname, "MATERIAL"))
    {
//TODO:      dwg_print_MATERIAL(dat, obj);
      return 0;
    }

  return 0;
}

void
dwg_print_object(Bit_Chain* dat, Dwg_Object *obj)
{
  //Bit_Chain * dat = (Bit_Chain *)obj->parent->bit_chain;
  switch (obj->type)
    {
    case DWG_TYPE_TEXT:
      dwg_print_TEXT(dat, obj);
      break;
    case DWG_TYPE_ATTRIB:
      dwg_print_ATTRIB(dat, obj);
      break;
    case DWG_TYPE_ATTDEF:
      dwg_print_ATTDEF(dat, obj);
      break;
    case DWG_TYPE_BLOCK:
      dwg_print_BLOCK(dat, obj);
      break;
    case DWG_TYPE_ENDBLK:
      dwg_print_ENDBLK(dat, obj);
      break;
    case DWG_TYPE_SEQEND:
      dwg_print_SEQEND(dat, obj);
      break;
    case DWG_TYPE_INSERT:
      dwg_print_INSERT(dat, obj);
      break;
    case DWG_TYPE_MINSERT:
      dwg_print_MINSERT(dat, obj);
      break;
    case DWG_TYPE_VERTEX_2D:
      dwg_print_VERTEX_2D(dat, obj);
      break;
    case DWG_TYPE_VERTEX_3D:
      dwg_print_VERTEX_3D(dat, obj);
      break;
    case DWG_TYPE_VERTEX_MESH:
      dwg_print_VERTEX_MESH(dat, obj);
      break;
    case DWG_TYPE_VERTEX_PFACE:
      dwg_print_VERTEX_PFACE(dat, obj);
      break;
    case DWG_TYPE_VERTEX_PFACE_FACE:
      dwg_print_VERTEX_PFACE_FACE(dat, obj);
      break;
    case DWG_TYPE_POLYLINE_2D:
      dwg_print_POLYLINE_2D(dat, obj);
      break;
    case DWG_TYPE_POLYLINE_3D:
      dwg_print_POLYLINE_3D(dat, obj);
      break;
    case DWG_TYPE_ARC:
      dwg_print_ARC(dat, obj);
      break;
    case DWG_TYPE_CIRCLE:
      dwg_print_CIRCLE(dat, obj);
      break;
    case DWG_TYPE_LINE:
      dwg_print_LINE(dat, obj);
      break;
    case DWG_TYPE_DIMENSION_ORDINATE:
      dwg_print_DIMENSION_ORDINATE(dat, obj);
      break;
    case DWG_TYPE_DIMENSION_LINEAR:
      dwg_print_DIMENSION_LINEAR(dat, obj);
      break;
    case DWG_TYPE_DIMENSION_ALIGNED:
      dwg_print_DIMENSION_ALIGNED(dat, obj);
      break;
    case DWG_TYPE_DIMENSION_ANG3PT:
      dwg_print_DIMENSION_ANG3PT(dat, obj);
      break;
    case DWG_TYPE_DIMENSION_ANG2LN:
      dwg_print_DIMENSION_ANG2LN(dat, obj);
      break;
    case DWG_TYPE_DIMENSION_RADIUS:
      dwg_print_DIMENSION_RADIUS(dat, obj);
      break;
    case DWG_TYPE_DIMENSION_DIAMETER:
      dwg_print_DIMENSION_DIAMETER(dat, obj);
      break;
    case DWG_TYPE_POINT:
      dwg_print_POINT(dat, obj);
      break;
    case DWG_TYPE__3DFACE:
      dwg_print__3DFACE(dat, obj);
      break;
    case DWG_TYPE_POLYLINE_PFACE:
      dwg_print_POLYLINE_PFACE(dat, obj);
      break;
    case DWG_TYPE_POLYLINE_MESH:
      dwg_print_POLYLINE_MESH(dat, obj);
      break;
    case DWG_TYPE_SOLID:
      dwg_print_SOLID(dat, obj);
      break;
    case DWG_TYPE_TRACE:
      dwg_print_TRACE(dat, obj);
      break;
    case DWG_TYPE_SHAPE:
      dwg_print_SHAPE(dat, obj);
      break;
    case DWG_TYPE_VIEWPORT:
      dwg_print_VIEWPORT(dat, obj);
      break;
    case DWG_TYPE_ELLIPSE:
      dwg_print_ELLIPSE(dat, obj);
      break;
    case DWG_TYPE_SPLINE:
      dwg_print_SPLINE(dat, obj);
      break;
    case DWG_TYPE_REGION:
      dwg_print_REGION(dat, obj);
      break;
    case DWG_TYPE_3DSOLID:
      dwg_print__3DSOLID(dat, obj);
      break; /* Check the type of the object
              */
    case DWG_TYPE_BODY:
      dwg_print_BODY(dat, obj);
      break;
    case DWG_TYPE_RAY:
      dwg_print_RAY(dat, obj);
      break;
    case DWG_TYPE_XLINE:
      dwg_print_XLINE(dat, obj);
      break;
    case DWG_TYPE_DICTIONARY:
      dwg_print_DICTIONARY(dat, obj);
      break;
    case DWG_TYPE_MTEXT:
      dwg_print_MTEXT(dat, obj);
      break;
    case DWG_TYPE_LEADER:
      dwg_print_LEADER(dat, obj);
      break;
    case DWG_TYPE_TOLERANCE:
      dwg_print_TOLERANCE(dat, obj);
      break;
    case DWG_TYPE_MLINE:
      dwg_print_MLINE(dat, obj);
      break;
    case DWG_TYPE_BLOCK_CONTROL:
      dwg_print_BLOCK_CONTROL(dat, obj);
      break;
    case DWG_TYPE_BLOCK_HEADER:
      dwg_print_BLOCK_HEADER(dat, obj);
      break;
    case DWG_TYPE_LAYER_CONTROL:
      dwg_print_LAYER_CONTROL(dat, obj);
      break;
    case DWG_TYPE_LAYER:
      dwg_print_LAYER(dat, obj);
      break;
    case DWG_TYPE_SHAPEFILE_CONTROL:
      dwg_print_SHAPEFILE_CONTROL(dat, obj);
      break;
    case DWG_TYPE_SHAPEFILE:
      dwg_print_SHAPEFILE(dat, obj);
      break;
    case DWG_TYPE_LTYPE_CONTROL:
      dwg_print_LTYPE_CONTROL(dat, obj);
      break;
    case DWG_TYPE_LTYPE:
      dwg_print_LTYPE(dat, obj);
      break;
    case DWG_TYPE_VIEW_CONTROL:
      dwg_print_VIEW_CONTROL(dat, obj);
      break;
    case DWG_TYPE_VIEW:
      dwg_print_VIEW(dat, obj);
      break;
    case DWG_TYPE_UCS_CONTROL:
      dwg_print_UCS_CONTROL(dat, obj);
      break;
    case DWG_TYPE_UCS:
      dwg_print_UCS(dat, obj);
      break;
    case DWG_TYPE_VPORT_CONTROL:
      dwg_print_VPORT_CONTROL(dat, obj);
      break;
    case DWG_TYPE_VPORT:
      dwg_print_VPORT(dat, obj);
      break;
    case DWG_TYPE_APPID_CONTROL:
      dwg_print_APPID_CONTROL(dat, obj);
      break;
    case DWG_TYPE_APPID:
      dwg_print_APPID(dat, obj);
      break;
    case DWG_TYPE_DIMSTYLE_CONTROL:
      dwg_print_DIMSTYLE_CONTROL(dat, obj);
      break;
    case DWG_TYPE_DIMSTYLE:
      dwg_print_DIMSTYLE(dat, obj);
      break;
    case DWG_TYPE_VP_ENT_HDR_CONTROL:
      dwg_print_VP_ENT_HDR_CONTROL(dat, obj);
      break;
    case DWG_TYPE_VP_ENT_HDR:
      dwg_print_VP_ENT_HDR(dat, obj);
      break;
    case DWG_TYPE_GROUP:
      dwg_print_GROUP(dat, obj);
      break;
    case DWG_TYPE_MLINESTYLE:
      dwg_print_MLINESTYLE(dat, obj);
      break;
    case DWG_TYPE_OLE2FRAME:
      dwg_print_OLE2FRAME(dat, obj);
      break;
    case DWG_TYPE_DUMMY:
      dwg_print_DUMMY(dat, obj);
      break;
    case DWG_TYPE_LONG_TRANSACTION:
      dwg_print_LONG_TRANSACTION(dat, obj);
      break;
    case DWG_TYPE_LWPLINE:
      dwg_print_LWPLINE(dat, obj);
      break;
    case DWG_TYPE_HATCH:
      dwg_print_HATCH(dat, obj);
      break;
    case DWG_TYPE_XRECORD:
      dwg_print_XRECORD(dat, obj);
      break;
    case DWG_TYPE_PLACEHOLDER:
      dwg_print_PLACEHOLDER(dat, obj);
      break;
    case DWG_TYPE_PROXY_ENTITY:
      dwg_print_PROXY_ENTITY(dat, obj);
      break;
    case DWG_TYPE_OLEFRAME:
      dwg_print_OLEFRAME(dat, obj);
      break;
    case DWG_TYPE_VBA_PROJECT:
      LOG_ERROR("Unhandled Object VBA_PROJECT. Has its own section\n");
      //dwg_print_VBA_PROJECT(dat, obj);
      break;
    case DWG_TYPE_LAYOUT:
      dwg_print_LAYOUT(dat, obj);
      break;
    default:
      if (obj->type == obj->parent->layout_number)
        {
          dwg_print_LAYOUT(dat, obj);
        }
      /* > 500:
         TABLE, DICTIONARYWDLFT, IDBUFFER, IMAGE, IMAGEDEF, IMAGEDEF_REACTOR,
         LAYER_INDEX, OLE2FRAME, PROXY, RASTERVARIABLES, SORTENTSTABLE, SPATIAL_FILTER,
         SPATIAL_INDEX
      */
      else if (!dwg_print_variable_type(obj->parent, dat, obj))
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
              dwg_print_UNKNOWN_OBJ(dat, obj);
            }
          else if (klass)
            {
              dwg_print_UNKNOWN_ENT(dat, obj);
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

#undef IS_PRINT
