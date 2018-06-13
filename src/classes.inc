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

#ifndef action
# error action define missing (decode, encode, encode, ...)
#endif

#define WARN_UNHANDLED_CLASS \
      LOG_WARN("Unhandled Class %s %d %s (0x%x%s) %u/%lX", is_entity ? "entity" : "object",\
               klass->number, dxfname, klass->proxyflag,\
               klass->wasazombie ? " was proxy" : "",\
               obj->index, obj->handle.value)
#define WARN_UNTESTED_CLASS \
      LOG_WARN("Untested Class %s %d %s (0x%x%s) %u/%lX", is_entity ? "entity" : "object",\
               klass->number, dxfname, klass->proxyflag,\
               klass->wasazombie ? " was proxy" : "",\
               obj->index, obj->handle.value)

#define _DWG_FUNC_N(action,name) dwg_ ## action ## _ ## name
#define DWG_FUNC_N(action,name) _DWG_FUNC_N(action,name)

#define STABLE_CLASS(action, name) \
  if (!strcmp(dxfname, #name)) \
    { \
      if (!strcmp(#action, "decode")) \
        obj->fixedtype = DWG_TYPE_##name; \
      if (!strcmp(#action, "free")) \
        free(dxfname);; \
      return DWG_FUNC_N(action,name)(dat, obj); \
    }
#define STABLE_CLASS_DXF(action, name, _dxfname) \
  if (!strcmp(dxfname, #_dxfname)) \
    { \
      if (!strcmp(#action, "decode")) \
        obj->fixedtype = DWG_TYPE_##name; \
      if (!strcmp(#action, "free")) \
        free(dxfname);; \
      return DWG_FUNC_N(action,name)(dat, obj); \
    }
#define STABLE_CLASS_CPP(action, name, _cppname) \
  if (!strcmp(klass->cppname, #name)) \
    { \
      if (!strcmp(#action, "decode")) \
        obj->fixedtype = DWG_TYPE_##name; \
      if (!strcmp(#action, "free")) \
        free(dxfname);; \
      return DWG_FUNC_N(action,name)(dat, obj); \
    }
#define UNTESTED_CLASS(action, name) \
  if (!strcmp(dxfname, #name)) \
    { \
      WARN_UNTESTED_CLASS; \
      if (!strcmp(#action, "decode")) \
        obj->fixedtype = DWG_TYPE_##name; \
      if (!strcmp(#action, "free")) \
        free(dxfname);; \
      return DWG_FUNC_N(action,name)(dat, obj); \
    }
#define UNTESTED_CLASS_CPP(action, name, _cppname) \
  if (!strcmp(klass->cppname, #_cppname)) \
    { \
      WARN_UNTESTED_CLASS; \
      if (!strcmp(#action, "decode")) \
        obj->fixedtype = DWG_TYPE_##name; \
      if (!strcmp(#action, "free")) \
        free(dxfname);; \
      return DWG_FUNC_N(action,name)(dat, obj); \
    }
#define UNHANDLED_CLASS(action, name) \
  if (!strcmp(dxfname, #name)) \
    { \
      WARN_UNHANDLED_CLASS; \
      if (!strcmp(#action, "decode")) \
        obj->fixedtype = DWG_TYPE_##name; \
      if (!strcmp(#action, "free")) \
        free(dxfname);; \
      /* return dwg_##action_##name(dat, obj); */ \
      return DWG_ERR_UNHANDLEDCLASS; \
    }
#define UNHANDLED_CLASS_DXF(action, name, _dxfname) \
  if (!strcmp(dxfname, #_dxfname)) \
    { \
      WARN_UNHANDLED_CLASS; \
      if (!strcmp(#action, "decode") || !memcmp(#action, "in", 2)) \
        obj->fixedtype = DWG_TYPE_##name; \
      if (!strcmp(#action, "free")) \
        free(dxfname);; \
      /* return dwg_##action_##name(dat, obj); */ \
      return DWG_ERR_UNHANDLEDCLASS; \
    }

#ifdef DEBUG_CLASSES
#define DEBUGGING_CLASS(action, name) UNTESTED_CLASS(action, name)
#else
#define DEBUGGING_CLASS(action, name) UNHANDLED_CLASS(action, name)
#endif

  /* Entities */
  STABLE_CLASS    (action, IMAGE)     /*ent*/
  STABLE_CLASS    (action, OLE2FRAME) /*ent (also fixed) */
  STABLE_CLASS    (action, TABLE)     /*ent*/
  STABLE_CLASS    (action, WIPEOUT)   /*ent*/
  STABLE_CLASS_DXF(action, TABLE, ACAD_TABLE) /*ent*/
  /* Objects */
  STABLE_CLASS    (action, DICTIONARYVAR)
  STABLE_CLASS_DXF(action, DICTIONARYWDFLT, ACDBDICTIONARYWDFLT)
  STABLE_CLASS    (action, FIELD)
  STABLE_CLASS    (action, FIELDLIST)
  STABLE_CLASS    (action, GROUP)
  STABLE_CLASS    (action, HATCH)
  STABLE_CLASS    (action, IDBUFFER)
  STABLE_CLASS    (action, IMAGEDEF)
  STABLE_CLASS    (action, IMAGEDEF_REACTOR)
  STABLE_CLASS    (action, LAYER_INDEX)
  STABLE_CLASS    (action, LAYOUT)
  STABLE_CLASS    (action, LWPOLYLINE)
  STABLE_CLASS    (action, MLEADERSTYLE)
  STABLE_CLASS    (action, OBJECTCONTEXTDATA)
  STABLE_CLASS_CPP(action, OBJECTCONTEXTDATA, AcDbObjectContextData)
  STABLE_CLASS_DXF(action, PLACEHOLDER, ACDBPLACEHOLDER)
  //STABLE_CLASS_DXF(action, PROXY_OBJECT, PROXY)
  STABLE_CLASS    (action, RASTERVARIABLES)
  STABLE_CLASS    (action, SCALE)
  STABLE_CLASS    (action, SORTENTSTABLE)
  STABLE_CLASS    (action, SPATIAL_FILTER)
  STABLE_CLASS    (action, SPATIAL_INDEX)
  STABLE_CLASS    (action, VISUALSTYLE)
  STABLE_CLASS    (action, XRECORD)

  // not enough coverage, but assumed ok
  UNTESTED_CLASS    (action, CAMERA) /* ent, not persistent in a DWG */
  UNTESTED_CLASS    (action, GEODATA)
  UNTESTED_CLASS    (action, OBJECT_PTR)
  UNTESTED_CLASS_CPP(action, OBJECT_PTR, CAseDLPNTableRecord)
  UNTESTED_CLASS    (action, TABLECONTENT)
  UNTESTED_CLASS    (action, TABLEGEOMETRY)
  UNTESTED_CLASS    (action, WIPEOUTVARIABLES)

  // coverage exists, but broken and being worked on. needs -DDEBUG_CLASSES. see also
  // the unknown branch with all the unknown blobs of these.
  //PROXY_ENTITY has a fixed type
  DEBUGGING_CLASS  (action, GEOPOSITIONMARKER) /*ent, yet unsorted */
  DEBUGGING_CLASS  (action, HELIX)             /*ent, yet unsorted */
  DEBUGGING_CLASS  (action, LIGHT)             /*ent, yet unsorted */
  DEBUGGING_CLASS  (action, MULTILEADER)       /*ent, broken Leader_Line's/Points */
  DEBUGGING_CLASS  (action, SURFACE)           /*ent, yet unsorted */
  DEBUGGING_CLASS  (action, UNDERLAY)          /*ent DGN DWF PDF */

  DEBUGGING_CLASS  (action, CELLSTYLEMAP) //broken
  DEBUGGING_CLASS  (action, MATERIAL)     //working on
  DEBUGGING_CLASS  (action, PLOTSETTINGS) //yet unsorted
  DEBUGGING_CLASS  (action, SUN)          //i.e. 2000/1.dwg
  DEBUGGING_CLASS  (action, VBA_PROJECT)  //Has its own section?

  // no coverage, unimplemented, passed through
  UNHANDLED_CLASS    (action, ARCALIGNEDTEXT)
  UNHANDLED_CLASS_DXF(action, ASSOC2DCONSTRAINTGROUP, ACDBASSOC2DCONSTRAINTGROUP)
  UNHANDLED_CLASS_DXF(action, ASSOCGEOMDEPENDENCY, ACDBASSOCGEOMDEPENDENCY)
  UNHANDLED_CLASS_DXF(action, ASSOCNETWORK, ACDBASSOCNETWORK)
  UNHANDLED_CLASS    (action, CSACDOCUMENTOPTIONS) //or just DOCUMENTOPTIONS?
  UNHANDLED_CLASS    (action, DATATABLE)
  UNHANDLED_CLASS    (action, DBCOLOR)
  UNHANDLED_CLASS_DXF(action, DETAILVIEWSTYLE, ACDBDETAILVIEWSTYLE)
  UNHANDLED_CLASS    (action, DIMASSOC)
  UNHANDLED_CLASS    (action, GEODATA)
  UNHANDLED_CLASS_DXF(action, LEADEROBJECTCONTEXTDATA, ACDB_LEADEROBJECTCONTEXTDATA_CLASS)
  UNHANDLED_CLASS    (action, LIGHTLIST)
  UNHANDLED_CLASS    (action, NPOCOLLECTION)
  UNHANDLED_CLASS_DXF(action, POINTCLOUD, ACDBPOINTCLOUD)
  UNHANDLED_CLASS    (action, RTEXT)
  UNHANDLED_CLASS_DXF(action, SECTIONVIEWSTYLE, ACDBSECTIONVIEWSTYLE)
  UNHANDLED_CLASS    (action, TABLESTYLE)
  UNHANDLED_CLASS    (action, UNDERLAYDEFINITION)
  UNHANDLED_CLASS_DXF(action, XREFPANELOBJECT, EXACXREFPANELOBJECT)
