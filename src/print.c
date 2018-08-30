/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2010, 2018 Free Software Foundation, Inc.                  */
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
 * modified by Reini Urban
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
#include "print.h"

#define DWG_LOGLEVEL DWG_LOGLEVEL_TRACE
#include "logging.h"

/* the current version per spec block */
static unsigned int cur_ver = 0;

/*--------------------------------------------------------------------------------
 * MACROS
 */

#define ACTION print
#define IS_PRINT

#define FIELD(name,type,dxf) \
  FIELD_G_TRACE(name,type,dxf)
#define FIELD_TRACE(name,type) \
  LOG_TRACE(#name ": " FORMAT_##type " [" #type "]\n", _obj->name)
#define FIELD_G_TRACE(name,type,dxf) \
  LOG_TRACE(#name ": " FORMAT_##type " [" #type " " #dxf "]\n", _obj->name)
#define FIELD_CAST(name,type,cast,dxf)             \
  FIELD_G_TRACE(name,cast,dxf)

#define LOG_INSANE_TF(var,len)
#define FIELD_VALUE(name) _obj->name
#define FIELD_2PT_TRACE(name, type, dxf) \
  { LOG_TRACE(#name ": (" FORMAT_BD ", " FORMAT_BD ") [" #type " %d]\n", \
            _obj->name.x, _obj->name.y, dxf); }
#define FIELD_3PT_TRACE(name, type, dxf) \
  { LOG_TRACE(#name ": (" FORMAT_BD ", " FORMAT_BD ", " FORMAT_BD ") [" #type " %d]\n", \
              _obj->name.x, _obj->name.y, _obj->name.z, dxf); }

#define ANYCODE -1
#define VALUE_HANDLE(handleptr, name, handle_code, dxf) \
  if (handleptr) { \
    LOG_TRACE(#name ": HANDLE(%x.%d.%lX) absolute:%lX/%lu [%d]\n",\
              handleptr->handleref.code, \
              handleptr->handleref.size, \
              handleptr->handleref.value,\
              handleptr->absolute_ref, handleptr->absolute_ref, dxf);  \
  }
#define FIELD_HANDLE(name, handle_code, dxf) VALUE_HANDLE(_obj->name, name, handle_code, dxf)
#define FIELD_DATAHANDLE(name, code, dxf) FIELD_HANDLE(name, code, dxf)
#define VALUE_HANDLE_N(handleptr, name, vcount, handle_code, dxf)\
  if (handleptr) { \
    LOG_TRACE(#name "[%d]: HANDLE(%d.%d.%lX) absolute:%lX/%lu [%d]\n",\
              (int)vcount, \
              handleptr->handleref.code, \
              handleptr->handleref.size, \
              handleptr->handleref.value, \
              handleptr->absolute_ref, handleptr->absolute_ref, dxf); \
  }
#define FIELD_HANDLE_N(name, vcount, handle_code, dxf) \
  VALUE_HANDLE_N(_obj->name, name, vcount, handle_code, dxf)

#define FIELD_B(name,dxf) FIELD(name, B, dxf);
#define FIELD_BB(name,dxf) FIELD(name, BB, dxf);
#define FIELD_3B(name,dxf) FIELD(name, 3B, dxf);
#define FIELD_BS(name,dxf) FIELD(name, BS, dxf);
#define FIELD_BL(name,dxf) FIELD(name, BL, dxf);
#define FIELD_BLL(name,dxf) FIELD(name, BLL, dxf);
#define FIELD_BD(name,dxf) { \
  if (bit_isnan(_obj->name)) { \
    LOG_ERROR("Invalid BD " #name); \
    return DWG_ERR_VALUEOUTOFBOUNDS; \
  } \
  FIELD(name, BD, dxf); \
}
#define FIELD_RC(name,dxf) FIELD(name, RC, dxf);
#define FIELD_RS(name,dxf) FIELD(name, RS, dxf);
#define FIELD_RD(name,dxf) { \
  if (bit_isnan(_obj->name)) { \
    LOG_ERROR("Invalid BD " #name); \
    return DWG_ERR_VALUEOUTOFBOUNDS; \
  } \
  FIELD(name, RD, dxf); \
}
#define FIELD_RL(name,dxf) FIELD(name, RL, dxf);
#define FIELD_RLL(name,dxf) FIELD(name, RLL, dxf);
#define FIELD_RLx(name,dxf) \
  LOG_TRACE(#name ": %x [RL " #dxf "]\n", _obj->name)
#define FIELD_MC(name,dxf) FIELD(name, MC, dxf);
#define FIELD_MS(name,dxf) FIELD(name, MS, dxf);
#define FIELD_TF(name,len,dxf) { \
    LOG_TRACE( #name ": [%d TF " #dxf "]\n", len); \
    LOG_INSANE_TF(FIELD_VALUE(name), (int)len); }
#define FIELD_TFF(name,len,dxf) { \
    LOG_TRACE( #name ": [%d TFF " #dxf "]\n", len); \
    LOG_INSANE_TF(FIELD_VALUE(name), (int)len); }

#define FIELD_TV(name,dxf) FIELD(name, TV, dxf);
#define FIELD_TU(name,dxf) LOG_TRACE_TU(#name, (BITCODE_TU)_obj->name, dxf)
#define FIELD_T FIELD_TV /*TODO: implement version dependent string fields */
#define FIELD_BT(name,dxf) FIELD(name, BT, dxf);
#define FIELD_4BITS(name,dxf) FIELD_G_TRACE(name,4BITS,dxf)
#define FIELD_BE(name,dxf) FIELD_3RD(name,dxf)
#define FIELD_DD(name, _default, dxf) 
#define FIELD_2DD(name, d1, d2, dxf) FIELD_2PT_TRACE(name, DD, dxf)
#define FIELD_3DD(name, def, dxf) FIELD_3PT_TRACE(name, DD, dxf)
#define FIELD_2RD(name,dxf) FIELD_2PT_TRACE(name,RD,dxf)
#define FIELD_2BD(name,dxf) FIELD_2PT_TRACE(name,BD,dxf)
#define FIELD_2BD_1(name,dxf) FIELD_2PT_TRACE(name,BD,dxf)
#define FIELD_3RD(name,dxf) FIELD_3PT_TRACE(name,RD,dxf)
#define FIELD_3BD(name,dxf) FIELD_3PT_TRACE(name,BD,dxf)
#define FIELD_3BD_1(name,dxf) FIELD_3PT_TRACE(name,BD,dxf)
#define FIELD_3DPOINT(name,dxf) FIELD_3BD(name,dxf)
#define FIELD_CMC(color,dxf1,dxf2) { \
  LOG_TRACE(#color ".index [CMC.BS %d]\n", _obj->color.index) \
  if (dat->version >= R_2004) { \
    LOG_TRACE(#color ".rgb: 0x%06x [CMC.BL %d]\n", (unsigned)_obj->color.rgb, dxf2); \
    LOG_TRACE(#color ".flag: 0x%x [CMC.RC]\n", (unsigned)_obj->color.flag); \
    if (_obj->color.flag & 1) \
      LOG_TRACE(#color ".name: %s [CMC.TV]\n", _obj->color.name); \
    if (_obj->color.flag & 2) \
      LOG_TRACE(#color ".bookname: %s [CMC.TV]\n", _obj->color.book_name); \
  }\
}
#define FIELD_EMC(color,dxf1,dxf2) { \
  LOG_TRACE(#color ".index: %d [EMC.BS %d]\n", _obj->color.index, dxf1); \
  if (dat->version >= R_2004) { \
    if (_obj->color.flag) \
      LOG_TRACE(#color ".flag: 0x%x\n", (unsigned)_obj->color.flag); \
    if (_obj->color.flag & 0x20) \
      LOG_TRACE(#color ".alpha: 0x%06x [EMC.BL %d]\n", (unsigned)_obj->color.alpha, dxf2+20); \
    if (_obj->color.flag & 0x80) \
      LOG_TRACE(#color ".rgb: 0x%06x [EMC.BL %d]\n", (unsigned)_obj->color.rgb, dxf2); \
    if ((_obj->color.flag & 0x41) == 0x41) \
      LOG_TRACE(#color ".name: %s [EMC.TV %d]\n", _obj->color.name, dxf2+10);    \
    if ((_obj->color.flag & 0x42) == 0x42) \
      LOG_TRACE(#color ".bookname: %s [EMC.TV]\n", _obj->color.book_name); \
  } \
}

#define FIELD_TIMEBLL(name,dxf) \
  LOG_TRACE(#name " " #dxf ": " FORMAT_BL "." FORMAT_BL "\n", _obj->name.days, _obj->name.ms)

#define VALUE(value,type,dxf) \
  LOG_TRACE(FORMAT_##type " [" #type " " #dxf "]\n", value)
#define VALUE_RC(value,dxf) VALUE(value, RC, dxf)
#define VALUE_RS(value,dxf) VALUE(value, RS, dxf)
#define VALUE_RL(value,dxf) VALUE(value, RL, dxf)
#define VALUE_RD(value,dxf) VALUE(value, RD, dxf)

//FIELD_VECTOR_N(name, type, size):
// reads data of the type indicated by 'type' 'size' times and stores
// it all in the vector called 'name'.
#define FIELD_VECTOR_N(name, type, size, dxf)\
  if (size > 0 && _obj->name != NULL)\
    {\
      for (vcount=0; vcount < (BITCODE_BL)size; vcount++)\
        {\
          LOG_TRACE(#name "[%ld]: " FORMAT_##type "\n", (long)vcount, _obj->name[vcount]) \
        }\
    }
#define FIELD_VECTOR_T(name, size, dxf)\
  if (_obj->size > 0 && _obj->name != NULL)\
    {\
      for (vcount=0; vcount < (BITCODE_BL)_obj->size; vcount++)\
        {\
          PRE (R_2007) { \
            LOG_TRACE(#name "[%ld]: %s\n", (long)vcount, _obj->name[vcount]) \
          } else { \
            LOG_TRACE_TU(#name, _obj->name[vcount], dxf) \
          } \
        }\
    }

#define FIELD_VECTOR(name, type, size, dxf) FIELD_VECTOR_N(name, type, _obj->size, dxf)

#define FIELD_2RD_VECTOR(name, size, dxf)\
  if (_obj->name) { \
    for (vcount=0; vcount < (BITCODE_BL)_obj->size; vcount++)\
      {\
        FIELD_2RD(name[vcount], dxf);\
      }\
  }

#define FIELD_2DD_VECTOR(name, size, dxf)\
  if (_obj->name) { \
    FIELD_2RD(name[0], 0);\
    for (vcount = 1; vcount < (BITCODE_BL)_obj->size; vcount++)\
      {\
        FIELD_2DD(name[vcount], FIELD_VALUE(name[vcount - 1].x), FIELD_VALUE(name[vcount - 1].y), dxf);\
      }\
    }

#define FIELD_3DPOINT_VECTOR(name, size, dxf)\
  if (_obj->name) { \
    for (vcount=0; vcount < (BITCODE_BL)_obj->size; vcount++)\
      {\
        FIELD_3DPOINT(name[vcount], dxf);\
      }\
    }

#define HANDLE_VECTOR_N(name, size, code, dxf) \
  if (_obj->name) { \
    for (vcount=0; vcount < (BITCODE_BL)size; vcount++)\
      {\
        FIELD_HANDLE_N(name[vcount], vcount, code, dxf);\
      }\
    }

#define HANDLE_VECTOR(name, sizefield, code, dxf) \
  HANDLE_VECTOR_N(name, FIELD_VALUE(sizefield), code, dxf)

#define FIELD_NUM_INSERTS(num_inserts, type, dxf) \
  FIELD_G_TRACE(num_inserts, type, dxf)

#define FIELD_XDATA(name, size)

#define REACTORS(code)\
  if (dat->version >= R_2000 && obj->tio.object->num_reactors > 0x1000) { \
    fprintf(stderr, "Invalid num_reactors: %ld\n", (long)obj->tio.object->num_reactors); return DWG_ERR_VALUEOUTOFBOUNDS; } \
  if (obj->tio.object->reactors) {\
    for (vcount=0; vcount < obj->tio.object->num_reactors; vcount++)\
      {\
        VALUE_HANDLE_N(obj->tio.object->reactors[vcount], reactors, vcount, code, -5); \
      }\
  }

#define XDICOBJHANDLE(code)\
  SINCE(R_2004)\
    {\
      if (!obj->tio.object->xdic_missing_flag)\
        VALUE_HANDLE(obj->tio.object->xdicobjhandle, xdicobjhandle, code, 0); \
    }\
  PRIOR_VERSIONS\
    {\
      VALUE_HANDLE(obj->tio.object->xdicobjhandle, xdicobjhandle, code, 0); \
    }

#define COMMON_ENTITY_HANDLE_DATA /*  Empty */
#define SECTION_STRING_STREAM \
  { \
    Bit_Chain sav_dat = *dat; \
    dat = str_dat;
#define START_STRING_STREAM \
  obj->has_strings = bit_read_B(dat); \
  if (obj->has_strings) { \
    Bit_Chain sav_dat = *dat; \
    obj_string_stream(dat, obj, dat);
#define END_STRING_STREAM \
    *dat = sav_dat; \
  }
#define START_HANDLE_STREAM \
  *hdl_dat = *dat; \
  if (dat->version >= R_2007) bit_set_position(hdl_dat, obj->hdlpos)

#define DWG_ENTITY(token) \
static int \
dwg_print_##token (Bit_Chain *restrict dat, Dwg_Object *restrict obj)\
{\
  BITCODE_BL vcount, rcount1, rcount2, rcount3, rcount4; \
  Dwg_Entity_##token *ent, *_obj;\
  Dwg_Object_Entity *_ent;\
  Bit_Chain *hdl_dat = dat;\
  Bit_Chain* str_dat = dat;\
  Dwg_Data* dwg = obj->parent;\
  int error = 0; \
  LOG_INFO("Entity " #token ":\n")\
  _ent = obj->tio.entity;\
  _obj = ent = _ent->tio.token;\
  LOG_TRACE("Entity handle: %d.%d.%lX\n",\
    obj->handle.code,\
    obj->handle.size,\
    obj->handle.value)

#define DWG_ENTITY_END return 0; }

#define DWG_OBJECT(token) \
static int \
dwg_print_ ##token (Bit_Chain *restrict dat, Dwg_Object *restrict obj) \
{ \
  BITCODE_BL vcount, rcount1, rcount2, rcount3, rcount4;\
  Dwg_Object_##token *_obj;\
  Bit_Chain *hdl_dat = dat;\
  Bit_Chain* str_dat = dat;\
  Dwg_Data* dwg = obj->parent;\
  int error = 0; \
  LOG_INFO("Object " #token ":\n")\
  _obj = obj->tio.object->tio.token;\
  LOG_TRACE("Object handle: %d.%d.%lX\n",\
    obj->handle.code,\
    obj->handle.size,\
    obj->handle.value)

#define DWG_OBJECT_END return 0; }

#include "dwg.spec"

/* Returns 0 on success
   Dispatches on the variable types.
 */
static int
dwg_print_variable_type(Dwg_Data * dwg, Bit_Chain * dat, Dwg_Object* obj)
{
  int i;
  int is_entity;
  Dwg_Class *klass;

  i = obj->type - 500;
  if (i < 0 || i > (int)dwg->num_classes)
    return DWG_ERR_INVALIDTYPE;

  klass = &dwg->dwg_class[i];
  if (!klass || !klass->dxfname)
    return DWG_ERR_INTERNALERROR;
  // almost always false
  is_entity = dwg_class_is_entity(klass);

  #include "classes.inc"

  return DWG_ERR_UNHANDLEDCLASS;
}

/* prints to logging.h OUTPUT (ie stderr). Returns 0 on success
   Dispatches on the fixed types.
*/
int
dwg_print_object(Bit_Chain* dat, Dwg_Object *obj)
{
  int error = 0;
  //Bit_Chain * dat = (Bit_Chain *)obj->parent->bit_chain;
  //Bit_Chain *hdl_dat = dat;
  switch (obj->type)
    {
    case DWG_TYPE_TEXT:
      return dwg_print_TEXT(dat, obj);
    case DWG_TYPE_ATTRIB:
      return dwg_print_ATTRIB(dat, obj);
   case DWG_TYPE_ATTDEF:
      return dwg_print_ATTDEF(dat, obj);
   case DWG_TYPE_BLOCK:
      return dwg_print_BLOCK(dat, obj);
   case DWG_TYPE_ENDBLK:
      return dwg_print_ENDBLK(dat, obj);
   case DWG_TYPE_SEQEND:
      return dwg_print_SEQEND(dat, obj);
   case DWG_TYPE_INSERT:
      return dwg_print_INSERT(dat, obj);
   case DWG_TYPE_MINSERT:
      return dwg_print_MINSERT(dat, obj);
   case DWG_TYPE_VERTEX_2D:
      return dwg_print_VERTEX_2D(dat, obj);
   case DWG_TYPE_VERTEX_3D:
      return dwg_print_VERTEX_3D(dat, obj);
   case DWG_TYPE_VERTEX_MESH:
      return dwg_print_VERTEX_MESH(dat, obj);
   case DWG_TYPE_VERTEX_PFACE:
      return dwg_print_VERTEX_PFACE(dat, obj);
   case DWG_TYPE_VERTEX_PFACE_FACE:
      return dwg_print_VERTEX_PFACE_FACE(dat, obj);
   case DWG_TYPE_POLYLINE_2D:
      return dwg_print_POLYLINE_2D(dat, obj);
   case DWG_TYPE_POLYLINE_3D:
      return dwg_print_POLYLINE_3D(dat, obj);
   case DWG_TYPE_ARC:
      return dwg_print_ARC(dat, obj);
   case DWG_TYPE_CIRCLE:
      return dwg_print_CIRCLE(dat, obj);
   case DWG_TYPE_LINE:
      return dwg_print_LINE(dat, obj);
   case DWG_TYPE_DIMENSION_ORDINATE:
      return dwg_print_DIMENSION_ORDINATE(dat, obj);
   case DWG_TYPE_DIMENSION_LINEAR:
      return dwg_print_DIMENSION_LINEAR(dat, obj);
   case DWG_TYPE_DIMENSION_ALIGNED:
      return dwg_print_DIMENSION_ALIGNED(dat, obj);
   case DWG_TYPE_DIMENSION_ANG3PT:
      return dwg_print_DIMENSION_ANG3PT(dat, obj);
   case DWG_TYPE_DIMENSION_ANG2LN:
      return dwg_print_DIMENSION_ANG2LN(dat, obj);
   case DWG_TYPE_DIMENSION_RADIUS:
      return dwg_print_DIMENSION_RADIUS(dat, obj);
   case DWG_TYPE_DIMENSION_DIAMETER:
      return dwg_print_DIMENSION_DIAMETER(dat, obj);
   case DWG_TYPE_POINT:
      return dwg_print_POINT(dat, obj);
   case DWG_TYPE__3DFACE:
      return dwg_print__3DFACE(dat, obj);
   case DWG_TYPE_POLYLINE_PFACE:
      return dwg_print_POLYLINE_PFACE(dat, obj);
   case DWG_TYPE_POLYLINE_MESH:
      return dwg_print_POLYLINE_MESH(dat, obj);
   case DWG_TYPE_SOLID:
      return dwg_print_SOLID(dat, obj);
   case DWG_TYPE_TRACE:
      return dwg_print_TRACE(dat, obj);
   case DWG_TYPE_SHAPE:
      return dwg_print_SHAPE(dat, obj);
   case DWG_TYPE_VIEWPORT:
      return dwg_print_VIEWPORT(dat, obj);
   case DWG_TYPE_ELLIPSE:
      return dwg_print_ELLIPSE(dat, obj);
   case DWG_TYPE_SPLINE:
      return dwg_print_SPLINE(dat, obj);
   case DWG_TYPE_REGION:
      return dwg_print_REGION(dat, obj);
   case DWG_TYPE__3DSOLID:
      return dwg_print__3DSOLID(dat, obj);
      /* Check the type of the object? */
    case DWG_TYPE_BODY:
      return dwg_print_BODY(dat, obj);
   case DWG_TYPE_RAY:
      return dwg_print_RAY(dat, obj);
   case DWG_TYPE_XLINE:
      return dwg_print_XLINE(dat, obj);
   case DWG_TYPE_DICTIONARY:
      return dwg_print_DICTIONARY(dat, obj);
   case DWG_TYPE_MTEXT:
      return dwg_print_MTEXT(dat, obj);
   case DWG_TYPE_LEADER:
      return dwg_print_LEADER(dat, obj);
   case DWG_TYPE_TOLERANCE:
      return dwg_print_TOLERANCE(dat, obj);
   case DWG_TYPE_MLINE:
      return dwg_print_MLINE(dat, obj);
   case DWG_TYPE_BLOCK_CONTROL:
      return dwg_print_BLOCK_CONTROL(dat, obj);
   case DWG_TYPE_BLOCK_HEADER:
      return dwg_print_BLOCK_HEADER(dat, obj);
   case DWG_TYPE_LAYER_CONTROL:
      return dwg_print_LAYER_CONTROL(dat, obj);
   case DWG_TYPE_LAYER:
      return dwg_print_LAYER(dat, obj);
   case DWG_TYPE_STYLE_CONTROL:
      return dwg_print_STYLE_CONTROL(dat, obj);
   case DWG_TYPE_STYLE:
      return dwg_print_STYLE(dat, obj);
   case DWG_TYPE_LTYPE_CONTROL:
      return dwg_print_LTYPE_CONTROL(dat, obj);
   case DWG_TYPE_LTYPE:
      return dwg_print_LTYPE(dat, obj);
   case DWG_TYPE_VIEW_CONTROL:
      return dwg_print_VIEW_CONTROL(dat, obj);
   case DWG_TYPE_VIEW:
      return dwg_print_VIEW(dat, obj);
   case DWG_TYPE_UCS_CONTROL:
      return dwg_print_UCS_CONTROL(dat, obj);
   case DWG_TYPE_UCS:
      return dwg_print_UCS(dat, obj);
   case DWG_TYPE_VPORT_CONTROL:
      return dwg_print_VPORT_CONTROL(dat, obj);
   case DWG_TYPE_VPORT:
      return dwg_print_VPORT(dat, obj);
   case DWG_TYPE_APPID_CONTROL:
      return dwg_print_APPID_CONTROL(dat, obj);
   case DWG_TYPE_APPID:
      return dwg_print_APPID(dat, obj);
   case DWG_TYPE_DIMSTYLE_CONTROL:
      return dwg_print_DIMSTYLE_CONTROL(dat, obj);
   case DWG_TYPE_DIMSTYLE:
      return dwg_print_DIMSTYLE(dat, obj);
   case DWG_TYPE_VPORT_ENTITY_CONTROL:
      return dwg_print_VPORT_ENTITY_CONTROL(dat, obj);
   case DWG_TYPE_VPORT_ENTITY_HEADER:
      return dwg_print_VPORT_ENTITY_HEADER(dat, obj);
   case DWG_TYPE_GROUP:
      return dwg_print_GROUP(dat, obj);
   case DWG_TYPE_MLINESTYLE:
      return dwg_print_MLINESTYLE(dat, obj);
   case DWG_TYPE_OLE2FRAME:
      return dwg_print_OLE2FRAME(dat, obj);
   case DWG_TYPE_DUMMY:
      return dwg_print_DUMMY(dat, obj);
   case DWG_TYPE_LONG_TRANSACTION:
      return dwg_print_LONG_TRANSACTION(dat, obj);
   case DWG_TYPE_LWPOLYLINE:
      return dwg_print_LWPOLYLINE(dat, obj);
   case DWG_TYPE_HATCH:
      return dwg_print_HATCH(dat, obj);
   case DWG_TYPE_XRECORD:
      return dwg_print_XRECORD(dat, obj);
   case DWG_TYPE_PLACEHOLDER:
      return dwg_print_PLACEHOLDER(dat, obj);
   case DWG_TYPE_OLEFRAME:
      return dwg_print_OLEFRAME(dat, obj);
   case DWG_TYPE_VBA_PROJECT:
      LOG_ERROR("Unhandled Object VBA_PROJECT. Has its own section\n");
      //dwg_print_VBA_PROJECT(dat, obj);
      break;
   case DWG_TYPE_LAYOUT:
      return dwg_print_LAYOUT(dat, obj);
   case DWG_TYPE_PROXY_ENTITY:
      return dwg_print_PROXY_ENTITY(dat, obj);
    case DWG_TYPE_PROXY_OBJECT: //DXF name: PROXY
      return dwg_print_PROXY_OBJECT(dat, obj);
   default:
      if (obj->type == obj->parent->layout_number)
        {
          return dwg_print_LAYOUT(dat, obj);
        }
      /* > 500 */
      else if ((error = dwg_print_variable_type(obj->parent, dat, obj))
               & DWG_ERR_UNHANDLEDCLASS)
        {
          Dwg_Data *dwg = obj->parent;
          int is_entity = 0;
          int i = obj->type - 500;
          Dwg_Class *klass = NULL;

          if (i > 0 && i < (int)dwg->num_classes)
            {
              klass = &dwg->dwg_class[i];
              is_entity = klass ? dwg_class_is_entity(klass) : 0;
            }
          // properly dwg_decode_object/_entity for eed, reactors, xdic
          if (klass && !is_entity)
            {
              return dwg_print_UNKNOWN_OBJ(dat, obj);
            }
          else if (klass)
            {
              return dwg_print_UNKNOWN_ENT(dat, obj);
            }
          else // not a class
            {
              LOG_WARN("Unknown object, skipping eed/reactors/xdic");
              SINCE(R_2000)
                {
                  LOG_INFO("Object bitsize: %u\n", obj->bitsize)
                }
              LOG_INFO("Object handle: %x.%d.%lX\n",
                       obj->handle.code, obj->handle.size, obj->handle.value);
              return error | DWG_ERR_INVALIDTYPE;
            }
        }
    }
  return DWG_ERR_UNHANDLEDCLASS;
}

#undef IS_PRINT
