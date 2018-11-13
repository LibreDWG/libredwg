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
 * in_dxfb.c: read Binary DXF to dwg WIP
 * written by Reini Urban
 *
 * See in_dxf.c for the plan. Only the input methods are customized here.
 */

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "common.h"
#include "bits.h"
#include "dwg.h"
#include "out_dxf.h"
#include "in_dxf.h"

#define DWG_LOGLEVEL DWG_LOGLEVEL_TRACE
#include "logging.h"

/* the current version per spec block */
static unsigned int cur_ver = 0;
static char buf[4096];

/*--------------------------------------------------------------------------------
 * MACROS
 */

#define ACTION indxfb
#define IS_PRINT

// add the name/type/dxf combo to some structure for this element.
#define FIELD(name,type,dxf) \
  dxf_add_field(obj, #name, #type, dxf);

#define FIELD_CAST(name,type,cast,dxf) FIELD(name,cast,dxf)
#define FIELD_TRACE(name,type)
#define FIELD_TEXT(name,len,str) \
    fprintf(dat->fh, #name ": \"%s\",\n", str)
#ifdef HAVE_NATIVE_WCHAR2
# define FIELD_TEXT_TU(name,wlen,wstr) \
    fprintf(dat->fh, #name ": \"%ls\",\n", wstr)
#else
# define FIELD_TEXT_TU(name,wlen,wstr) \
  { \
    BITCODE_TU _ws = (BITCODE_TU)wstr; \
    uint16_t _c; \
    fprintf(dat->fh, #name ": \""); \
    while ((_c = *_ws++)) { \
      fprintf(dat->fh, "%c", (char)(_c & 0xff)); \
    } \
    fprintf(dat->fh, "\",\n"); \
  }
#endif

#define FIELD_VALUE(name) _obj->name
#define ANYCODE -1
//TODO read
#define VALUE_HANDLE(hdlptr, handle_code, dxf) \
  if (hdlptr) { \
     uint32_t _hdl = (uint32_t)hdlptr->absolute_ref; \
     GROUP(dxf); \
     fwrite(&_hdl, sizeof(uint32_t), 1, dat->fh); \
  }
#define FIELD_HANDLE(name, handle_code, dxf) VALUE_HANDLE(_obj->name, handle_code, dxf)

#define GROUP(code)                  \
    {                                \
    if (dat->version < R_14) {       \
      unsigned char icode = (unsigned char)code; \
      fwrite(&icode, sizeof(unsigned char), 1, dat->fh); \
    } else {                         \
      short icode = code;            \
      fwrite(&icode, sizeof(short), 1, dat->fh); \
    }                                \
  }
#define VALUE_TV(str, dxf)\
    GROUP(dxf);\
    fprintf(dat->fh, "%s%c", str, 0)  
#define FIELD_TV(name,dxf) VALUE_TV(_obj->name, dxf)
#define HEADER_TV(name,dxf) \
    HEADER_9(name);\
    VALUE_TV(dwg->header_vars.name, dxf)
#define VAR(name) \
    GROUP(9);\
    fprintf (dat->fh, "$%s%c", #name, 0)
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
#define SECTION(section) \
    VALUE_TV("SECTION", 0);\
    VALUE_TV(#section, 2)
#define ENDSEC()        VALUE_TV("ENDSEC", 0)
#define RECORD(record)  VALUE_TV(#record, 0)

#define VALUE(value, type, dxf) \
  if (dxf) { \
    GROUP(dxf);\
    snprintf (buf, 4096, "%s\n", dxf_format (dxf));\
    GCC_DIAG_IGNORE(-Wformat-nonliteral) \
    fprintf(dat->fh, buf, value);\
    GCC_DIAG_RESTORE \
  }

#define HANDLE_NAME(name, code, section) \
  HEADER_HANDLE_NAME(name, code, section)

#define FIELD_DATAHANDLE(name, code, dxf) FIELD_HANDLE(name, code, dxf)
#define FIELD_HANDLE_N(name, vcount, handle_code, dxf) FIELD_HANDLE(name, handle_code, dxf)

#define FIELD_B(name,dxf)   FIELD_RC(name, dxf)
#define FIELD_BB(name,dxf)  FIELD_RC(name, dxf)
#define FIELD_3B(name,dxf)  FIELD_RC(name, dxf)
#define FIELD_BS(name,dxf)  FIELD_RS(name, dxf)
#define FIELD_BL(name,dxf)  FIELD_RL(name, dxf)
#define FIELD_BLL(name,dxf) FIELD_RLL(name, dxf)
#define FIELD_BD(name,dxf)  FIELD_RD(name, dxf)

#define HEADER_9(name) \
    GROUP(9);\
    fprintf (dat->fh, "$%s%c", #name, 0)
#define VALUE_RC(value,dxf) \
  {\
    BITCODE_RC _c = value;\
    GROUP(dxf);\
    fread(&_c, 1, 1, dat->fh); \
  }
#define FIELD_RC(name,dxf)  FIELD(name,RC,dxf)
#define HEADER_RC(name,dxf)  HEADER_9(name); FIELD(name,RC,dxf)
#define HEADER_RLL(name,dxf) HEADER_9(name); FIELD(name, RLL, dxf)
#define HEADER_T(name,dxf)   HEADER_9(name); VALUE_T(_obj->name, dxf)
#define HEADER_TU(name,dxf)  HEADER_9(name); VALUE_TU(_obj->name, dxf)
#define HEADER_B(name,dxf)   HEADER_9(name); FIELD(name, B, dxf)
#define HEADER_BS(name,dxf)  HEADER_9(name); FIELD(name, BS, dxf)
#define HEADER_BL(name,dxf)  HEADER_9(name); FIELD(name, BL, dxf)
#define HEADER_BD(name,dxf)  HEADER_9(name); FIELD(name, BD, dxf)

#define VALUE_RS(value,dxf) \
  {\
    BITCODE_RS _s = value;\
    GROUP(dxf);\
    fread(&_s, 2, 1, dat->fh);\
  }
#define FIELD_RS(name,dxf) FIELD(name,RS,dxf)
#define HEADER_RS(name,dxf) \
    HEADER_9(name);\
    VALUE_RS(dwg->header_vars.name, dxf)

#define VALUE_RD(value,dxf)\
  {\
    double _d = value;\
    GROUP(dxf);\
    fwrite(&_d, sizeof(double), 1, dat->fh); \
  }
#define FIELD_RD(name,dxf) VALUE_RD(_obj->name,dxf)
#define HEADER_RD(name,dxf) \
    HEADER_9(name);\
    VALUE_RD(dwg->header_vars.name, dxf)

#define VALUE_RL(value,dxf)\
  {\
    BITCODE_RL _s = value;\
    GROUP(dxf);\
    fwrite(&_s, sizeof(BITCODE_RL), 1, dat->fh);\
  }
#define FIELD_RL(name,dxf) VALUE_RL(_obj->name,dxf)
#define HEADER_RL(name,dxf) \
    HEADER_9(name);\
    VALUE_RL(dwg->header_vars.name, dxf)

#define VALUE_H(value,dxf) \
  {\
    Dwg_Object_Ref *_ref = value;\
    if (_ref && _ref->obj) { VALUE_RS(_ref->absolute_ref, dxf); }\
    else {VALUE_RS(0, dxf);}                                \
  }
#define HEADER_H(name,dxf) \
    HEADER_9(name);\
    VALUE_H(dwg->header_vars.name, dxf)
#define VALUE_HANDLE_NAME(value,dxf,section)\
  {\
    Dwg_Object_Ref *_ref = value;\
    if (_ref && _ref->obj) { VALUE_TV(_ref->obj->tio.object->tio.section->entry_name, dxf);} \
    else VALUE_TV("", dxf);\
  }
#define FIELD_HANDLE_NAME(name,dxf,section) VALUE_HANDLE_NAME(_obj->name,dxf,section)
#define HEADER_HANDLE_NAME(name,dxf,section)\
    HEADER_9(name);\
    VALUE_HANDLE_NAME(dwg->header_vars.name,dxf,section)

#define FIELD_RLL(name,dxf) \
  {\
    BITCODE_RLL _s = _obj->name;\
    GROUP(9);\
    fprintf (dat->fh, "$%s%c", #name, 0);\
    GROUP(dxf);\
    fwrite(&_s, sizeof(BITCODE_RLL), 1, dat->fh);\
  }
#define FIELD_MC(name,dxf) FIELD_RC(name,dxf)
#define FIELD_MS(name,dxf)  FIELD_RS(name,dxf)
#define FIELD_TF(name,len,dxf)  FIELD_TEXT(name, len, _obj->name)
#define FIELD_TFF(name,len,dxf) FIELD_TEXT(name, len, _obj->name)
//#define FIELD_TV(name,dxf)      FIELD_TEXT(name, strlen(_obj->name), _obj->name)
#define FIELD_TU(name,dxf)      FIELD_TEXT_TU(name, wcslen((wchar_t*)_obj->name), (wchar_t*)_obj->name)
#define FIELD_T(name,dxf) \
  { if (dat->version >= R_2007) { FIELD_TU(name, dxf); } \
    else                        { FIELD_TV(name, dxf); } }
#define FIELD_BT(name,dxf)    FIELD(name, BT, dxf);
#define FIELD_4BITS(name,dxf) FIELD(name,4BITS,dxf)
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
#define FIELD_CMC(color,dxf1,dxf2) VALUE_RS(_obj->color.index, dxf1)
// TODO rgb
#define HEADER_CMC(name,dxf) \
    HEADER_9(name);\
    VALUE_RS(dwg->header_vars.name.index, dxf)
#define FIELD_TIMEBLL(name,dxf) \
  VALUE_RL(_obj->name.days, dxf); VALUE_RL(_obj->name.ms, dxf)

//FIELD_VECTOR_N(name, type, size):
// reads data of the type indicated by 'type' 'size' times and stores
// it all in the vector called 'name'.
#define FIELD_VECTOR_N(name, type, size, dxf)\
    for (vcount=0; vcount < (BITCODE_BL)size; vcount++)\
      fprintf(dat->fh, #name " " FORMAT_##type ",\n", _obj->name[vcount]);
#define FIELD_VECTOR_T(name, size, dxf)\
    PRE (R_2007) { \
      for (vcount=0; vcount < (BITCODE_BL)_obj->size; vcount++) \
        fprintf(dat->fh, #name ": \"%s\",\n", _obj->name[vcount]); \
    } else { \
      for (vcount=0; vcount < (BITCODE_BL)_obj->size; vcount++) \
        FIELD_TEXT_TU(name, wcslen((wchar_t*)_obj->name[vcount]), _obj->name[vcount]); \
    }

#define FIELD_VECTOR(name, type, size, dxf) FIELD_VECTOR_N(name, type, _obj->size, dxf)

#define FIELD_2RD_VECTOR(name, size, dxf)\
  fprintf(dat->fh, "["); \
  for (vcount=0; vcount < (BITCODE_BL)_obj->size; vcount++)\
    {\
      FIELD_2RD(name[vcount], dxf);\
    }\
  fprintf(dat->fh, "]\n");

#define FIELD_2DD_VECTOR(name, size, dxf)\
  fprintf(dat->fh, "["); \
  FIELD_2RD(name[0], 0);\
  for (vcount = 1; vcount < (BITCODE_BL)_obj->size; vcount++)\
    {\
      FIELD_2DD(name[vcount], FIELD_VALUE(name[vcount - 1].x), FIELD_VALUE(name[vcount - 1].y), dxf);\
    }\
  fprintf(dat->fh, "]\n");

#define FIELD_3DPOINT_VECTOR(name, size, dxf)\
  fprintf(dat->fh, "["); \
  for (vcount=0; vcount < (BITCODE_BL)_obj->size; vcount++)\
    {\
      FIELD_3DPOINT(name[vcount], dxf);\
    }\
  fprintf(dat->fh, "]\n");

#define HANDLE_VECTOR_N(name, size, code, dxf) \
  fprintf(dat->fh, "["); \
  for (vcount=0; vcount < (BITCODE_BL)size; vcount++)\
    {\
      FIELD_HANDLE_N(name[vcount], vcount, code, dxf);\
    }\
  fprintf(dat->fh, "]\n");

#define HANDLE_VECTOR(name, sizefield, code, dxf) \
  HANDLE_VECTOR_N(name, FIELD_VALUE(sizefield), code, dxf)

#define FIELD_NUM_INSERTS(num_inserts, type, dxf) \
  FIELD(num_inserts, type, dxf)

#define FIELD_XDATA(name, size)

#define REACTORS(code)\
  fprintf(dat->fh, "[");\
  for (vcount=0; vcount < obj->tio.object->num_reactors; vcount++)\
    {\
      VALUE_HANDLE(obj->tio.object->reactors[vcount], code, 330);\
    }\
  fprintf(dat->fh, "]\n");

#define XDICOBJHANDLE(code)\
  SINCE(R_2004)\
    {\
      if (!obj->tio.object->xdic_missing_flag)\
        VALUE_HANDLE(obj->tio.object->xdicobjhandle, code, -3);\
    }\
  PRIOR_VERSIONS\
    {\
      VALUE_HANDLE(obj->tio.object->xdicobjhandle, code, -3);\
    }

#define COMMON_ENTITY_HANDLE_DATA \
  SINCE(R_13) { \
    error |= dxfb_common_entity_handle_data(dat, obj); \
  }  
#define SECTION_STRING_STREAM
#define START_STRING_STREAM
#define END_STRING_STREAM
#define START_HANDLE_STREAM

#define DWG_ENTITY(token) \
static int \
dwg_indxfb_##token (Bit_Chain *dat, Dwg_Object * obj) \
{\
  BITCODE_BL vcount, rcount1, rcount2, rcount3, rcount4; \
  Dwg_Entity_##token *ent, *_obj;\
  Dwg_Object_Entity *_ent;\
  int error = 0;\
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
dwg_indxfb_ ##token (Bit_Chain *dat, Dwg_Object * obj) \
{ \
  BITCODE_BL vcount, rcount1, rcount2, rcount3, rcount4; \
  Bit_Chain *hdl_dat = dat;\
  Dwg_Data* dwg = obj->parent;\
  Dwg_Object_##token *_obj;\
  int error = 0;\
  LOG_INFO("Object " #token ":\n")\
  _obj = obj->tio.object->tio.token;\
  LOG_TRACE("Object handle: %d.%d.%lX\n",\
    obj->handle.code,\
    obj->handle.size,\
    obj->handle.value)

#define DWG_OBJECT_END return 0; }

static int
dxfb_common_entity_handle_data(Bit_Chain *dat, Dwg_Object* obj)
{
  (void)dat; (void)obj;
  return 0;
}

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

  i = obj->type - 500;
  if (i < 0 || i >= dwg->num_classes)
    return DWG_ERR_INVALIDTYPE;

  klass = &dwg->dwg_class[i];
  dxfname = klass->dxfname;
  // almost always false
  is_entity = dwg_class_is_entity(klass);
  
  #include "classes.inc"

  return DWG_ERR_UNHANDLEDCLASS;
}

static int
dwg_indxfb_object(Bit_Chain *dat, Dwg_Object *obj)
{

  switch (obj->type)
    {
    case DWG_TYPE_TEXT:
      return dwg_indxfb_TEXT(dat, obj);
    case DWG_TYPE_ATTRIB:
      return dwg_indxfb_ATTRIB(dat, obj);
    case DWG_TYPE_ATTDEF:
      return dwg_indxfb_ATTDEF(dat, obj);
    case DWG_TYPE_BLOCK:
      return dwg_indxfb_BLOCK(dat, obj);
    case DWG_TYPE_ENDBLK:
      return dwg_indxfb_ENDBLK(dat, obj);
    case DWG_TYPE_SEQEND:
      return dwg_indxfb_SEQEND(dat, obj);
    case DWG_TYPE_INSERT:
      return dwg_indxfb_INSERT(dat, obj);
    case DWG_TYPE_MINSERT:
      return dwg_indxfb_MINSERT(dat, obj);
    case DWG_TYPE_VERTEX_2D:
      return dwg_indxfb_VERTEX_2D(dat, obj);
    case DWG_TYPE_VERTEX_3D:
      return dwg_indxfb_VERTEX_3D(dat, obj);
    case DWG_TYPE_VERTEX_MESH:
      return dwg_indxfb_VERTEX_MESH(dat, obj);
    case DWG_TYPE_VERTEX_PFACE:
      return dwg_indxfb_VERTEX_PFACE(dat, obj);
    case DWG_TYPE_VERTEX_PFACE_FACE:
      return dwg_indxfb_VERTEX_PFACE_FACE(dat, obj);
    case DWG_TYPE_POLYLINE_2D:
      return dwg_indxfb_POLYLINE_2D(dat, obj);
    case DWG_TYPE_POLYLINE_3D:
      return dwg_indxfb_POLYLINE_3D(dat, obj);
    case DWG_TYPE_ARC:
      return dwg_indxfb_ARC(dat, obj);
    case DWG_TYPE_CIRCLE:
      return dwg_indxfb_CIRCLE(dat, obj);
    case DWG_TYPE_LINE:
      return dwg_indxfb_LINE(dat, obj);
    case DWG_TYPE_DIMENSION_ORDINATE:
      return dwg_indxfb_DIMENSION_ORDINATE(dat, obj);
    case DWG_TYPE_DIMENSION_LINEAR:
      return dwg_indxfb_DIMENSION_LINEAR(dat, obj);
    case DWG_TYPE_DIMENSION_ALIGNED:
      return dwg_indxfb_DIMENSION_ALIGNED(dat, obj);
    case DWG_TYPE_DIMENSION_ANG3PT:
      return dwg_indxfb_DIMENSION_ANG3PT(dat, obj);
    case DWG_TYPE_DIMENSION_ANG2LN:
      return dwg_indxfb_DIMENSION_ANG2LN(dat, obj);
    case DWG_TYPE_DIMENSION_RADIUS:
      return dwg_indxfb_DIMENSION_RADIUS(dat, obj);
    case DWG_TYPE_DIMENSION_DIAMETER:
      return dwg_indxfb_DIMENSION_DIAMETER(dat, obj);
    case DWG_TYPE_POINT:
      return dwg_indxfb_POINT(dat, obj);
    case DWG_TYPE__3DFACE:
      return dwg_indxfb__3DFACE(dat, obj);
    case DWG_TYPE_POLYLINE_PFACE:
      return dwg_indxfb_POLYLINE_PFACE(dat, obj);
    case DWG_TYPE_POLYLINE_MESH:
      return dwg_indxfb_POLYLINE_MESH(dat, obj);
    case DWG_TYPE_SOLID:
      return dwg_indxfb_SOLID(dat, obj);
    case DWG_TYPE_TRACE:
      return dwg_indxfb_TRACE(dat, obj);
    case DWG_TYPE_SHAPE:
      return dwg_indxfb_SHAPE(dat, obj);
    case DWG_TYPE_VIEWPORT:
      return dwg_indxfb_VIEWPORT(dat, obj);
    case DWG_TYPE_ELLIPSE:
      return dwg_indxfb_ELLIPSE(dat, obj);
    case DWG_TYPE_SPLINE:
      return dwg_indxfb_SPLINE(dat, obj);
    case DWG_TYPE_REGION:
      return dwg_indxfb_REGION(dat, obj);
    case DWG_TYPE__3DSOLID:
      return dwg_indxfb__3DSOLID(dat, obj);
    case DWG_TYPE_BODY:
      return dwg_indxfb_BODY(dat, obj);
    case DWG_TYPE_RAY:
      return dwg_indxfb_RAY(dat, obj);
    case DWG_TYPE_XLINE:
      return dwg_indxfb_XLINE(dat, obj);
    case DWG_TYPE_DICTIONARY:
      return dwg_indxfb_DICTIONARY(dat, obj);
    case DWG_TYPE_MTEXT:
      return dwg_indxfb_MTEXT(dat, obj);
    case DWG_TYPE_LEADER:
      return dwg_indxfb_LEADER(dat, obj);
    case DWG_TYPE_TOLERANCE:
      return dwg_indxfb_TOLERANCE(dat, obj);
    case DWG_TYPE_MLINE:
      return dwg_indxfb_MLINE(dat, obj);
    case DWG_TYPE_BLOCK_CONTROL:
      return dwg_indxfb_BLOCK_CONTROL(dat, obj);
    case DWG_TYPE_BLOCK_HEADER:
      return dwg_indxfb_BLOCK_HEADER(dat, obj);
    case DWG_TYPE_LAYER_CONTROL:
      return dwg_indxfb_LAYER_CONTROL(dat, obj);
    case DWG_TYPE_LAYER:
      return dwg_indxfb_LAYER(dat, obj);
    case DWG_TYPE_STYLE_CONTROL:
      return dwg_indxfb_STYLE_CONTROL(dat, obj);
    case DWG_TYPE_STYLE:
      return dwg_indxfb_STYLE(dat, obj);
    case DWG_TYPE_LTYPE_CONTROL:
      return dwg_indxfb_LTYPE_CONTROL(dat, obj);
    case DWG_TYPE_LTYPE:
      return dwg_indxfb_LTYPE(dat, obj);
    case DWG_TYPE_VIEW_CONTROL:
      return dwg_indxfb_VIEW_CONTROL(dat, obj);
    case DWG_TYPE_VIEW:
      return dwg_indxfb_VIEW(dat, obj);
    case DWG_TYPE_UCS_CONTROL:
      return dwg_indxfb_UCS_CONTROL(dat, obj);
    case DWG_TYPE_UCS:
      return dwg_indxfb_UCS(dat, obj);
    case DWG_TYPE_VPORT_CONTROL:
      return dwg_indxfb_VPORT_CONTROL(dat, obj);
    case DWG_TYPE_VPORT:
      return dwg_indxfb_VPORT(dat, obj);
    case DWG_TYPE_APPID_CONTROL:
      return dwg_indxfb_APPID_CONTROL(dat, obj);
    case DWG_TYPE_APPID:
      return dwg_indxfb_APPID(dat, obj);
    case DWG_TYPE_DIMSTYLE_CONTROL:
      return dwg_indxfb_DIMSTYLE_CONTROL(dat, obj);
    case DWG_TYPE_DIMSTYLE:
      return dwg_indxfb_DIMSTYLE(dat, obj);
    case DWG_TYPE_VPORT_ENTITY_CONTROL:
      return dwg_indxfb_VPORT_ENTITY_CONTROL(dat, obj);
    case DWG_TYPE_VPORT_ENTITY_HEADER:
      return dwg_indxfb_VPORT_ENTITY_HEADER(dat, obj);
    case DWG_TYPE_GROUP:
      return dwg_indxfb_GROUP(dat, obj);
    case DWG_TYPE_MLINESTYLE:
      return dwg_indxfb_MLINESTYLE(dat, obj);
    case DWG_TYPE_OLE2FRAME:
      return dwg_indxfb_OLE2FRAME(dat, obj);
    case DWG_TYPE_DUMMY:
      return dwg_indxfb_DUMMY(dat, obj);
    case DWG_TYPE_LONG_TRANSACTION:
      return dwg_indxfb_LONG_TRANSACTION(dat, obj);
    case DWG_TYPE_LWPOLYLINE:
      return dwg_indxfb_LWPOLYLINE(dat, obj);
    case DWG_TYPE_HATCH:
      return dwg_indxfb_HATCH(dat, obj);
    case DWG_TYPE_XRECORD:
      return dwg_indxfb_XRECORD(dat, obj);
    case DWG_TYPE_PLACEHOLDER:
      return dwg_indxfb_PLACEHOLDER(dat, obj);
    case DWG_TYPE_OLEFRAME:
      return dwg_indxfb_OLEFRAME(dat, obj);
    case DWG_TYPE_VBA_PROJECT:
      LOG_ERROR("Unhandled Object VBA_PROJECT. Has its own section\n");
      //dwg_indxfb_VBA_PROJECT(dat, obj);
      return DWG_ERR_UNHANDLEDCLASS;
    case DWG_TYPE_LAYOUT:
      return dwg_indxfb_LAYOUT(dat, obj);
    case DWG_TYPE_PROXY_ENTITY:
      return dwg_indxfb_PROXY_ENTITY(dat, obj);
    case DWG_TYPE_PROXY_OBJECT:
      return dwg_indxfb_PROXY_OBJECT(dat, obj);
    default:
      if (obj->type == obj->parent->layout_number)
        {
          return dwg_indxfb_LAYOUT(dat, obj);
        }
      /* > 500 */
      if (DWG_ERR_UNHANDLEDCLASS &
          dwg_dxfb_variable_type(obj->parent, dat, obj))
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
              return 0; //dwg_indxfb_UNKNOWN_OBJ(dat, obj);
            }
          else if (klass)
            {
              return 0; //dwg_indxfb_UNKNOWN_ENT(dat, obj);
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
  return DWG_ERR_UNHANDLEDCLASS;
}

// see https://www.autodesk.com/techpubs/autocad/acad2000/dxf/header_section_group_codes_dxf_02.htm
static int
dxfb_header_read(Bit_Chain *dat, Dwg_Data* dwg)
{
  Dwg_Header_Variables* _obj = &dwg->header_vars;
  Dwg_Object* obj = NULL;
  double ms;
  const int minimal = dwg->opts & 0x10;
  const char* codepage =
    (dwg->header.codepage == 30 || dwg->header.codepage == 0)
    ? "ANSI_1252"
    : (dwg->header.version >= R_2007)
      ? "UTF-8"
      : "ANSI_1252";

  return 0;
}

static int
dxfb_classes_read (Bit_Chain *dat, Dwg_Data * dwg)
{
  unsigned int i;

  SECTION(HEADER);
  for (i=0; i < dwg->num_classes; i++)
    {
      RECORD(CLASS);
      VALUE_TV (dwg->dwg_class[i].dxfname, 1);
      VALUE_TV (dwg->dwg_class[i].cppname, 2);
      VALUE_TV (dwg->dwg_class[i].appname, 3);
      VALUE_RS (dwg->dwg_class[i].proxyflag, 90);
      VALUE_RC (dwg->dwg_class[i].wasazombie, 280);
      // Is-an-entity. 1f2 for entities, 1f3 for objects
      VALUE_RC (dwg->dwg_class[i].item_class_id == 0x1F2 ? 1 : 0, 281);
    }
  ENDSEC();
  return 0;
}

static int
dxfb_tables_read (Bit_Chain *dat, Dwg_Data * dwg)
{
  (void)dwg;

  SECTION(TABLES);
  //...
  ENDSEC();
  return 0;
}

static int
dxfb_blocks_read (Bit_Chain *dat, Dwg_Data * dwg)
{
  (void)dwg;

  SECTION(BLOCKS);
  //...
  ENDSEC();
  return 0;
}

static int
dxfb_entities_read (Bit_Chain *dat, Dwg_Data * dwg)
{
  (void)dwg;

  SECTION(ENTITIES);
  //...
  ENDSEC();
  return 0;
}

static int
dxfb_objects_read (Bit_Chain *dat, Dwg_Data * dwg)
{
  (void)dwg;

  SECTION(OBJECTS);
  //...
  ENDSEC();
  return 0;
}

static int
dxfb_preview_read (Bit_Chain *dat, Dwg_Data * dwg)
{
  (void)dat; (void)dwg;
  //...
  return 0;
}

int
dwg_read_dxfb(Bit_Chain *dat, Dwg_Data * dwg)
{
  const int minimal = dwg->opts & 0x10;
  struct Dwg_Header *obj = &dwg->header;

  //TODO read header and check
  fprintf(dat->fh, "AutoCAD Binary DXF%s", "\r\n\x1a\0");
  //VALUE(999, PACKAGE_STRING);

  dxfb_header_read (dat, dwg);

  SINCE(R_2000) {
    if (dxfb_classes_read (dat, dwg))
      goto fail;
  }

  if (dxfb_tables_read (dat, dwg))
    goto fail;

  if (dxfb_blocks_read (dat, dwg))
    goto fail;

  if (dxfb_entities_read (dat, dwg))
    goto fail;

  SINCE(R_13) {
    if (dxfb_objects_read (dat, dwg))
      goto fail;
  }

  SINCE(R_2000) {
    if (dxfb_preview_read (dat, dwg))
      goto fail;
  }

  return 0;
 fail:
  return 1;
}

#undef IS_PRINT
