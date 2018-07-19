/* -*- c -*- */
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
 * classes.inc: the dynamic class dispatcher
 * for all types > 500
 * written by Reini Urban
 */

/**
 * Variable-typed classes and it's stability
 */

#ifndef ACTION
# error ACTION define missing (decode, encode, dxf, ...)
#endif

#define WARN_UNHANDLED_CLASS \
      LOG_WARN("Unhandled Class %s %d %s (0x%x%s) %u/%lX", is_entity ? "entity" : "object",\
               klass->number, klass->dxfname, klass->proxyflag,\
               klass->wasazombie ? " was proxy" : "",\
               obj->index, obj->handle.value)
#define WARN_UNTESTED_CLASS \
      LOG_WARN("Untested Class %s %d %s (0x%x%s) %u/%lX", is_entity ? "entity" : "object",\
               klass->number, klass->dxfname, klass->proxyflag,\
               klass->wasazombie ? " was proxy" : "",\
               obj->index, obj->handle.value)

//#define _DWG_FUNC_N(ACTION,name) dwg_ ## ACTION ## _ ## name
//#define DWG_FUNC_N(ACTION,name) _DWG_FUNC_N(ACTION,name)

#define STABLE_CLASS(ACTION, name) \
  if (!strcmp(klass->dxfname, #name)) \
    { \
      if (!strcmp(#ACTION, "decode")) \
        obj->fixedtype = DWG_TYPE_##name; \
      return DWG_FUNC_N(ACTION,name)(dat, obj); \
    }
#define STABLE_CLASS_DXF(ACTION, name, _dxfname) \
  if (!strcmp(klass->dxfname, #_dxfname)) \
    { \
      if (!strcmp(#ACTION, "decode")) \
        obj->fixedtype = DWG_TYPE_##name; \
      return DWG_FUNC_N(ACTION,name)(dat, obj); \
    }
#define STABLE_CLASS_CPP(ACTION, name, _cppname) \
  if (!strcmp(klass->cppname, #name)) \
    { \
      if (!strcmp(#ACTION, "decode")) \
        obj->fixedtype = DWG_TYPE_##name; \
      return DWG_FUNC_N(ACTION,name)(dat, obj); \
    }
#define UNTESTED_CLASS(ACTION, name) \
  if (!strcmp(klass->dxfname, #name)) \
    { \
      WARN_UNTESTED_CLASS; \
      if (!strcmp(#ACTION, "decode")) \
        obj->fixedtype = DWG_TYPE_##name; \
      return DWG_FUNC_N(ACTION,name)(dat, obj); \
    }
#define UNTESTED_CLASS_DXF(ACTION, name, _dxfname) \
  if (!strcmp(klass->dxfname, #_dxfname)) \
    { \
      WARN_UNTESTED_CLASS; \
      if (!strcmp(#ACTION, "decode") || !memcmp(#ACTION, "in", 2)) \
        obj->fixedtype = DWG_TYPE_##name; \
      return DWG_FUNC_N(ACTION,name)(dat, obj); \
    }
#define UNTESTED_CLASS_CPP(ACTION, name, _cppname) \
  if (!strcmp(klass->cppname, #_cppname)) \
    { \
      WARN_UNTESTED_CLASS; \
      if (!strcmp(#ACTION, "decode")) \
        obj->fixedtype = DWG_TYPE_##name; \
      return DWG_FUNC_N(ACTION,name)(dat, obj); \
    }
#define UNHANDLED_CLASS(ACTION, name) \
  if (!strcmp(klass->dxfname, #name)) \
    { \
      WARN_UNHANDLED_CLASS; \
      if (!strcmp(#ACTION, "decode")) \
        obj->fixedtype = DWG_TYPE_##name; \
      /* return dwg_##ACTION_##name(dat, obj); */ \
      return DWG_ERR_UNHANDLEDCLASS; \
    }
#define UNHANDLED_CLASS_DXF(ACTION, name, _dxfname) \
  if (!strcmp(klass->dxfname, #_dxfname)) \
    { \
      WARN_UNHANDLED_CLASS; \
      if (!strcmp(#ACTION, "decode") || !memcmp(#ACTION, "in", 2)) \
        obj->fixedtype = DWG_TYPE_##name; \
      /* return dwg_##ACTION_##name(dat, obj); */ \
      return DWG_ERR_UNHANDLEDCLASS; \
    }

#ifdef DEBUG_CLASSES
#define DEBUGGING_CLASS(ACTION, name)               UNTESTED_CLASS(ACTION, name)
#define DEBUGGING_CLASS_DXF(ACTION, name, _dxfname) UNTESTED_CLASS_DXF(ACTION, name, _dxfname)
#else
#define DEBUGGING_CLASS(ACTION, name)               UNHANDLED_CLASS(ACTION, name)
#define DEBUGGING_CLASS_DXF(ACTION, name, _dxfname) UNHANDLED_CLASS_DXF(ACTION, name, _dxfname)
#endif

  /* Entities */
  STABLE_CLASS    (ACTION, IMAGE)     /*ent*/
  STABLE_CLASS    (ACTION, OLE2FRAME) /*ent (also fixed) */
  STABLE_CLASS    (ACTION, WIPEOUT)   /*ent*/

  /* Objects */
  STABLE_CLASS    (ACTION, DICTIONARYVAR)
  STABLE_CLASS_DXF(ACTION, DICTIONARYWDFLT, ACDBDICTIONARYWDFLT)
  STABLE_CLASS    (ACTION, FIELD)
  STABLE_CLASS    (ACTION, FIELDLIST)
  STABLE_CLASS    (ACTION, GROUP)
  STABLE_CLASS    (ACTION, HATCH)
  STABLE_CLASS    (ACTION, IDBUFFER)
  STABLE_CLASS    (ACTION, IMAGEDEF)
  STABLE_CLASS    (ACTION, IMAGEDEF_REACTOR)
  STABLE_CLASS    (ACTION, LAYER_INDEX)
  STABLE_CLASS    (ACTION, LAYOUT)
  STABLE_CLASS    (ACTION, LWPOLYLINE)
  STABLE_CLASS    (ACTION, MLEADERSTYLE)
  STABLE_CLASS    (ACTION, OBJECTCONTEXTDATA)
  STABLE_CLASS_CPP(ACTION, OBJECTCONTEXTDATA, AcDbObjectContextData)
  STABLE_CLASS_DXF(ACTION, PLACEHOLDER, ACDBPLACEHOLDER)
  STABLE_CLASS    (ACTION, RASTERVARIABLES)
  STABLE_CLASS    (ACTION, SCALE)
  STABLE_CLASS    (ACTION, SORTENTSTABLE)
  STABLE_CLASS    (ACTION, SPATIAL_FILTER)
  STABLE_CLASS    (ACTION, SPATIAL_INDEX)
  STABLE_CLASS    (ACTION, VISUALSTYLE)
  STABLE_CLASS    (ACTION, WIPEOUTVARIABLES)
  STABLE_CLASS    (ACTION, XRECORD)

  // not enough coverage, but assumed ok
  UNTESTED_CLASS    (ACTION, CAMERA) /* ent, not persistent in a DWG */
  DEBUGGING_CLASS   (ACTION, DBCOLOR)
  UNTESTED_CLASS    (ACTION, GEODATA)
  UNTESTED_CLASS    (ACTION, OBJECT_PTR)
  UNTESTED_CLASS_CPP(ACTION, OBJECT_PTR, CAseDLPNTableRecord)
  UNTESTED_CLASS_DXF(ACTION, PROXY_OBJECT, PROXY)

  // coverage exists, but broken and being worked on. needs -DDEBUG_CLASSES. see also
  // the unknown branch with all the unknown blobs of these.
  //PROXY_ENTITY has a fixed type
  DEBUGGING_CLASS     (ACTION, GEOPOSITIONMARKER) /*ent, yet unsorted */
  DEBUGGING_CLASS     (ACTION, HELIX)             /*ent, yet unsorted */
  DEBUGGING_CLASS     (ACTION, LIGHT)             /*ent, yet unsorted */
  DEBUGGING_CLASS     (ACTION, MULTILEADER)        /*ent, broken Leader_Line's/Points */
  DEBUGGING_CLASS     (ACTION, SURFACE)           /*ent, yet unsorted */
  DEBUGGING_CLASS     (ACTION, UNDERLAY)          /*ent DGN DWF PDF */
  DEBUGGING_CLASS_DXF (ACTION, TABLE, ACAD_TABLE) /*ent, r2010+ needs subclassing */
  DEBUGGING_CLASS     (ACTION, TABLECONTENT) //wrong values
  DEBUGGING_CLASS     (ACTION, TABLEGEOMETRY) //wrong geom_data
  DEBUGGING_CLASS     (ACTION, CELLSTYLEMAP) //broken
  DEBUGGING_CLASS     (ACTION, MATERIAL)     //working on
  DEBUGGING_CLASS     (ACTION, PLOTSETTINGS) //yet unsorted
  DEBUGGING_CLASS     (ACTION, SUN)           //i.e. 2000/1.dwg DS_Line_R2000
  DEBUGGING_CLASS     (ACTION, SUNSTUDY)     //yet unsorted
  DEBUGGING_CLASS     (ACTION, VBA_PROJECT)  //Has its own section?
  DEBUGGING_CLASS_DXF (ACTION, ASSOCACTION, ACDBASSOCACTION)
  DEBUGGING_CLASS_DXF (ACTION, ASSOCNETWORK, ACDBASSOCNETWORK) /* very close */
  DEBUGGING_CLASS_DXF (ACTION, ASSOCOSNAPPOINTREFACTIONPARAM, ACDBASSOCOSNAPPOINTREFACTIONPARAM)
  DEBUGGING_CLASS_DXF (ACTION, EVALUATION_GRAPH, ACAD_EVALUATION_GRAPH)
  DEBUGGING_CLASS_DXF (ACTION, ASSOCPERSSUBENTMANAGER, ACDBASSOCPERSSUBENTMANAGER)

  // no coverage, unimplemented, passed through
  UNHANDLED_CLASS     (ACTION, ACSH_HISTORY_CLASS)
  UNHANDLED_CLASS     (ACTION, ACSH_SWEEP_CLASS)
  UNHANDLED_CLASS     (ACTION, ARCALIGNEDTEXT)
  UNHANDLED_CLASS     (ACTION, ARC_DIMENSION) //ent
  UNHANDLED_CLASS_DXF (ACTION, ASSOC2DCONSTRAINTGROUP, ACDBASSOC2DCONSTRAINTGROUP)
  UNHANDLED_CLASS_DXF (ACTION, ASSOCALIGNEDDIMACTIONBODY, ACDBASSOCALIGNEDDIMACTIONBODY)
  UNHANDLED_CLASS_DXF (ACTION, ASSOCDEPENDENCY, ACDBASSOCDEPENDENCY)
  UNHANDLED_CLASS_DXF (ACTION, ASSOCGEOMDEPENDENCY, ACDBASSOCGEOMDEPENDENCY)
  UNHANDLED_CLASS_DXF (ACTION, ASSOCOSNAPPOINTREFACTIONPARAM, ACDBASSOCOSNAPPOINTREFACTIONPARAM)
  UNHANDLED_CLASS_DXF (ACTION, ASSOCVERTEXACTIONPARAM, ACDBASSOCVERTEXACTIONPARAM)
  UNHANDLED_CLASS_DXF (ACTION, PERSSUBENTMANAGER, ACDBPERSSUBENTMANAGER)
  UNHANDLED_CLASS     (ACTION, CSACDOCUMENTOPTIONS) //or just DOCUMENTOPTIONS?
  UNHANDLED_CLASS     (ACTION, DATATABLE)
  UNHANDLED_CLASS_DXF (ACTION, DETAILVIEWSTYLE, ACDBDETAILVIEWSTYLE)
  UNHANDLED_CLASS     (ACTION, DIMASSOC)
  UNHANDLED_CLASS     (ACTION, GEODATA)
  UNHANDLED_CLASS     (ACTION, LAYER_FILTER)
  UNHANDLED_CLASS     (ACTION, LAYOUTPRINTCONFIG)
  UNHANDLED_CLASS_DXF (ACTION, LEADEROBJECTCONTEXTDATA, ACDB_LEADEROBJECTCONTEXTDATA_CLASS)
  UNHANDLED_CLASS     (ACTION, LIGHTLIST)
  UNHANDLED_CLASS     (ACTION, NPOCOLLECTION)
  UNHANDLED_CLASS_DXF (ACTION, POINTCLOUD, ACDBPOINTCLOUD)
  UNHANDLED_CLASS     (ACTION, RTEXT)
  UNHANDLED_CLASS_DXF (ACTION, SECTIONVIEWSTYLE, ACDBSECTIONVIEWSTYLE)
  UNHANDLED_CLASS     (ACTION, TABLESTYLE)
  UNHANDLED_CLASS     (ACTION, UNDERLAYDEFINITION)
  UNHANDLED_CLASS_DXF (ACTION, XREFPANELOBJECT, EXACXREFPANELOBJECT)
