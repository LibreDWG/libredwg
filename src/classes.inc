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
#define WARN_UNSTABLE_CLASS \
      LOG_WARN("Unstable Class %s %d %s (0x%x%s) %u/%lX", is_entity ? "entity" : "object",\
               klass->number, klass->dxfname, klass->proxyflag,\
               klass->wasazombie ? " was proxy" : "",\
               obj->index, obj->handle.value)
#if defined(IS_FREE) || defined(IS_PRINT)
# undef WARN_UNSTABLE_CLASS
# undef WARN_UNHANDLED_CLASS
# define WARN_UNSTABLE_CLASS
# define WARN_UNHANDLED_CLASS
#endif

//#define _DWG_FUNC_N(ACTION,name) dwg_ ## ACTION ## _ ## name
//#define DWG_FUNC_N(ACTION,name) _DWG_FUNC_N(ACTION,name)

#define STABLE_CLASS(ACTION, _name) \
  if (!strcmp(klass->dxfname, #_name)) \
    { \
      if (!strcmp(#ACTION, "decode") || !memcmp(#ACTION, "in", 2)) { \
        obj->name = (char*) #_name; \
        obj->dxfname = (char*) #_name; \
        obj->fixedtype = DWG_TYPE_##_name; \
      } \
      return DWG_FUNC_N(ACTION,_name)(dat, obj); \
    }
#define STABLE_CLASS_DXF(ACTION, _name, _dxfname) \
  if (!strcmp(klass->dxfname, #_dxfname)) \
    { \
      if (!strcmp(#ACTION, "decode") || !memcmp(#ACTION, "in", 2)) { \
        obj->name = (char*) #_name; \
        obj->dxfname = (char*) #_name; \
        obj->fixedtype = DWG_TYPE_##_name; \
      } \
      return DWG_FUNC_N(ACTION,_name)(dat, obj); \
    }
#define STABLE_CLASS_CPP(ACTION, _name, _cppname) \
  if (!strcmp(klass->cppname, #_name)) \
    { \
      if (!strcmp(#ACTION, "decode") || !memcmp(#ACTION, "in", 2)) { \
        obj->name = (char*) #_name; \
        obj->dxfname = (char*) #_name; \
        obj->fixedtype = DWG_TYPE_##_name; \
      } \
      return DWG_FUNC_N(ACTION,_name)(dat, obj); \
    }
#define UNSTABLE_CLASS(ACTION, _name) \
  if (!strcmp(klass->dxfname, #_name)) \
    { \
      WARN_UNSTABLE_CLASS; \
      if (!strcmp(#ACTION, "decode") || !memcmp(#ACTION, "in", 2)) { \
        obj->name = (char*) #_name; \
        obj->dxfname = (char*) #_name; \
        obj->fixedtype = DWG_TYPE_##_name; \
      } \
      return DWG_FUNC_N(ACTION,_name)(dat, obj); \
    }
#define UNSTABLE_CLASS_DXF(ACTION, _name, _dxfname) \
  if (!strcmp(klass->dxfname, #_dxfname)) \
    { \
      WARN_UNSTABLE_CLASS; \
      if (!strcmp(#ACTION, "decode") || !memcmp(#ACTION, "in", 2)) { \
        obj->name = (char*) #_name; \
        obj->dxfname = (char*) #_dxfname; \
        obj->fixedtype = DWG_TYPE_##_name; \
      } \
      return DWG_FUNC_N(ACTION,_name)(dat, obj); \
    }
#define UNSTABLE_CLASS_CPP(ACTION, _name, _cppname) \
  if (!strcmp(klass->cppname, #_cppname)) \
    { \
      WARN_UNSTABLE_CLASS; \
      if (!strcmp(#ACTION, "decode") || !memcmp(#ACTION, "in", 2)) { \
        obj->name = (char*) #_name; \
        obj->dxfname = (char*) #_name; \
        obj->fixedtype = DWG_TYPE_##_name; \
      } \
      return DWG_FUNC_N(ACTION,_name)(dat, obj); \
    }
#define UNHANDLED_CLASS(ACTION, _name) \
  if (!strcmp(klass->dxfname, #_name)) \
    { \
      WARN_UNHANDLED_CLASS; \
      if (!strcmp(#ACTION, "decode") || !memcmp(#ACTION, "in", 2)) { \
        obj->name = (char*) #_name; \
        obj->dxfname = (char*) #_name; \
      } \
      /* return dwg_##ACTION_##_name(dat, obj); */ \
      return DWG_ERR_UNHANDLEDCLASS; \
    }
#define UNHANDLED_CLASS_DXF(ACTION, _name, _dxfname) \
  if (!strcmp(klass->dxfname, #_dxfname)) \
    { \
      WARN_UNHANDLED_CLASS; \
      if (!strcmp(#ACTION, "decode") || !memcmp(#ACTION, "in", 2)) { \
        obj->name = (char*) #_name; \
        obj->dxfname = (char*) #_dxfname; \
      } \
      /* return dwg_##ACTION_##name(dat, obj); */ \
      return DWG_ERR_UNHANDLEDCLASS; \
    }

#ifdef DEBUG_CLASSES
#define DEBUGGING_CLASS(ACTION, name)               UNSTABLE_CLASS(ACTION, name)
#define DEBUGGING_CLASS_DXF(ACTION, name, _dxfname) UNSTABLE_CLASS_DXF(ACTION, name, _dxfname)
#else
#define DEBUGGING_CLASS(ACTION, name)               UNHANDLED_CLASS(ACTION, name)
#define DEBUGGING_CLASS_DXF(ACTION, name, _dxfname) UNHANDLED_CLASS_DXF(ACTION, name, _dxfname)
#endif

  /* Entities */
  STABLE_CLASS    (ACTION, CAMERA)    /*ent, not persistent in a DWG */
  STABLE_CLASS    (ACTION, IMAGE)     /*ent*/
  STABLE_CLASS    (ACTION, OLE2FRAME) /*ent (also fixed) */
  STABLE_CLASS_DXF(ACTION, UNDERLAY, PDFUNDERLAY)
  STABLE_CLASS_DXF(ACTION, UNDERLAY, DGNUNDERLAY)
  STABLE_CLASS_DXF(ACTION, UNDERLAY, DWFUNDERLAY)
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
  STABLE_CLASS    (ACTION, UNDERLAY)              /* ent DGN DWF PDF */
  STABLE_CLASS    (ACTION, UNDERLAYDEFINITION) //only tested pdf, but very simple
  STABLE_CLASS_DXF(ACTION, UNDERLAYDEFINITION, PDFDEFINITION)
  STABLE_CLASS_DXF(ACTION, UNDERLAYDEFINITION, DGNDEFINITION)
  STABLE_CLASS_DXF(ACTION, UNDERLAYDEFINITION, DWFDEFINITION)
  STABLE_CLASS    (ACTION, WIPEOUTVARIABLES)
  STABLE_CLASS    (ACTION, XRECORD)

  // unstable/undertested.
  // not enough coverage, but assumed mostly ok. field names may change.
  UNSTABLE_CLASS     (ACTION, MULTILEADER)  /* ent */
  UNSTABLE_CLASS_DXF (ACTION, ASSOCDEPENDENCY, ACDBASSOCDEPENDENCY)
  UNSTABLE_CLASS_DXF (ACTION, ASSOCPLANESURFACEACTIONBODY, ACDBASSOCPLANESURFACEACTIONBODY)
  UNSTABLE_CLASS     (ACTION, DIMASSOC)
  UNSTABLE_CLASS     (ACTION, DBCOLOR)
  UNSTABLE_CLASS_DXF (ACTION, DYNAMICBLOCKPURGEPREVENTER, ACDB_DYNAMICBLOCKPURGEPREVENTER_VERSION)
  UNSTABLE_CLASS     (ACTION, GEODATA)
  UNSTABLE_CLASS     (ACTION, HELIX)             /*ent */
  UNSTABLE_CLASS     (ACTION, LIGHT)             /*ent */
  UNSTABLE_CLASS     (ACTION, OBJECT_PTR)
  UNSTABLE_CLASS_CPP (ACTION, OBJECT_PTR, CAseDLPNTableRecord)
  UNSTABLE_CLASS_DXF (ACTION, PROXY_OBJECT, PROXY)
  UNSTABLE_CLASS_DXF (ACTION, PERSSUBENTMANAGER, ACDBPERSSUBENTMANAGER)
  UNSTABLE_CLASS     (ACTION, VISUALSTYLE)

  // coverage exists, but broken and being worked on. needs -DDEBUG_CLASSES. See also
  // examples/unknown with all the unknown blobs of these.
  //PROXY_ENTITY has a fixed type
  DEBUGGING_CLASS     (ACTION, GEOPOSITIONMARKER) /*ent, yet unsorted. no coverage */
  DEBUGGING_CLASS     (ACTION, PLANESURFACE)      /*ent */
  DEBUGGING_CLASS     (ACTION, EXTRUDEDSURFACE)   /*ent */
  DEBUGGING_CLASS     (ACTION, LOFTEDSURFACE)     /*ent */
  DEBUGGING_CLASS     (ACTION, REVOLVEDSURFACE)   /*ent */
  DEBUGGING_CLASS     (ACTION, SWEPTSURFACE)      /*ent */
  DEBUGGING_CLASS_DXF (ACTION, TABLE, ACAD_TABLE) /*ent, r2010+ needs subclassing */
  DEBUGGING_CLASS     (ACTION, TABLECONTENT) //wrong values
  DEBUGGING_CLASS     (ACTION, TABLEGEOMETRY) //wrong geom_data
  DEBUGGING_CLASS     (ACTION, CELLSTYLEMAP) //broken
  DEBUGGING_CLASS     (ACTION, MATERIAL)     //working on
  DEBUGGING_CLASS     (ACTION, PLOTSETTINGS) //yet unsorted
  DEBUGGING_CLASS     (ACTION, SUN)          //90%
  DEBUGGING_CLASS     (ACTION, SUNSTUDY)     //almost
  DEBUGGING_CLASS     (ACTION, VBA_PROJECT)  //Has its own section?
  DEBUGGING_CLASS     (ACTION, ACSH_SWEEP_CLASS) //SWEEPOPTIONS?
  DEBUGGING_CLASS     (ACTION, ACDBNAVISWORKSMODELDEF)
  DEBUGGING_CLASS_DXF (ACTION, ASSOCACTION, ACDBASSOCACTION)
  DEBUGGING_CLASS_DXF (ACTION, ASSOCNETWORK, ACDBASSOCNETWORK) /* very close */
  DEBUGGING_CLASS_DXF (ACTION, ASSOCALIGNEDDIMACTIONBODY, ACDBASSOCALIGNEDDIMACTIONBODY)
  DEBUGGING_CLASS_DXF (ACTION, ASSOCOSNAPPOINTREFACTIONPARAM, ACDBASSOCOSNAPPOINTREFACTIONPARAM)
  DEBUGGING_CLASS_DXF (ACTION, ASSOCPERSSUBENTMANAGER, ACDBASSOCPERSSUBENTMANAGER)
  DEBUGGING_CLASS_DXF (ACTION, ASSOC2DCONSTRAINTGROUP, ACDBASSOC2DCONSTRAINTGROUP)
  DEBUGGING_CLASS_DXF (ACTION, EVALUATION_GRAPH, ACAD_EVALUATION_GRAPH) /* almost */

  // unimplemented, passed through. coverage exists mostly, but documentation not.
  UNHANDLED_CLASS     (ACTION, ACSH_HISTORY_CLASS)
  UNHANDLED_CLASS     (ACTION, ARCALIGNEDTEXT)
  UNHANDLED_CLASS     (ACTION, ARC_DIMENSION) //ent
  UNHANDLED_CLASS_DXF (ACTION, ASSOCGEOMDEPENDENCY, ACDBASSOCGEOMDEPENDENCY)
  UNHANDLED_CLASS_DXF (ACTION, ASSOCOSNAPPOINTREFACTIONPARAM, ACDBASSOCOSNAPPOINTREFACTIONPARAM)
  UNHANDLED_CLASS_DXF (ACTION, ASSOCVERTEXACTIONPARAM, ACDBASSOCVERTEXACTIONPARAM)
  UNHANDLED_CLASS     (ACTION, DATATABLE)
  UNHANDLED_CLASS_DXF (ACTION, DETAILVIEWSTYLE, ACDBDETAILVIEWSTYLE)
  UNHANDLED_CLASS_DXF (ACTION, DOCUMENTOPTIONS, CSACDOCUMENTOPTIONS)
  UNHANDLED_CLASS     (ACTION, LAYER_FILTER)
  UNHANDLED_CLASS     (ACTION, LAYOUTPRINTCONFIG)
  UNHANDLED_CLASS_DXF (ACTION, LEADEROBJECTCONTEXTDATA, ACDB_LEADEROBJECTCONTEXTDATA_CLASS)
  UNHANDLED_CLASS     (ACTION, LIGHTLIST)
  UNHANDLED_CLASS     (ACTION, NPOCOLLECTION)
  UNHANDLED_CLASS_DXF (ACTION, POINTCLOUD, ACDBPOINTCLOUD)
  UNHANDLED_CLASS     (ACTION, RTEXT)
  UNHANDLED_CLASS_DXF (ACTION, SECTIONVIEWSTYLE, ACDBSECTIONVIEWSTYLE)
  UNHANDLED_CLASS     (ACTION, TABLESTYLE)
  UNHANDLED_CLASS_DXF (ACTION, XREFPANELOBJECT, EXACXREFPANELOBJECT)

/* Missing DXF names:
  ACSH_BOX_CLASS
  ACAD_PROXY_ENTITY  ACDBPOINTCLOUDEX  ARRAY
  ATTDYNBLOCKREF  GEOMAPIMAGE
  ATTBLOCKREF  ATTDYNBLOCKREF  BLOCKREF  CENTERMARK CENTERLINE
  DYNBLOCKREF XREF
  SECTIONOBJECT
*/
