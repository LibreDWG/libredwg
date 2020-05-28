/* -*- c -*- */
/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2018-2020 Free Software Foundation, Inc.                   */
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
 * Variable-typed classes and its stability
 */

#ifndef ACTION
# error ACTION define missing: decode, encode, dxf, indxf, print, free, ...
#endif

#define _XSTR(s) _STR(s)
#define _STR(s)  #s

#define WARN_UNHANDLED_CLASS \
      LOG_WARN ("Unhandled Class %s %d %s (0x%x%s) %u/%lX", is_entity ? "entity" : "object",\
               klass->number, klass->dxfname, klass->proxyflag,\
               klass->is_zombie ? "is_zombie" : "",\
               obj->index, obj->handle.value)
#define WARN_UNSTABLE_CLASS \
      LOG_WARN ("Unstable Class %s %d %s (0x%x%s) %u/%lX", is_entity ? "entity" : "object",\
               klass->number, klass->dxfname, klass->proxyflag,\
               klass->is_zombie ? "is_zombie" : "",\
               obj->index, obj->handle.value)
#if defined(IS_FREE) || defined(IS_PRINT)
# undef WARN_UNSTABLE_CLASS
# undef WARN_UNHANDLED_CLASS
# define WARN_UNSTABLE_CLASS
# define WARN_UNHANDLED_CLASS
#endif

#define DISALLOW_DXF(action, _name) \
  (strEQc (_STR(action), "dxf")) ? DWG_ERR_NOTYETSUPPORTED :
#define ALLOW_DXF(action, _name)

#ifdef DEBUG_CLASSES
# define DEBUGGING_DXF(action, name)                 ALLOW_DXF(action, name)
# define DEBUGGING_CLASS(action, name)               UNSTABLE_CLASS(action, name)
# define DEBUGGING_CLASS_DXF(action, name, _dxfname) UNSTABLE_CLASS_DXF(action, name, _dxfname)
# define DEBUGGING_CLASS_CPP(action, name, _cppname) UNSTABLE_CLASS_CPP(action, name, _cppname)
#else
# define DEBUGGING_DXF(action, name)                 DISALLOW_DXF(action, name)
# define DEBUGGING_CLASS(action, name)               UNHANDLED_CLASS(action, name)
# define DEBUGGING_CLASS_DXF(action, name, _dxfname) UNHANDLED_CLASS_DXF(action, name, _dxfname)
# define DEBUGGING_CLASS_CPP(action, name, _cppname) UNHANDLED_CLASS_CPP(action, name, _cppname)
#endif

#define STABLE_CLASS(action, _name) \
  if (klass->dxfname && strEQc (klass->dxfname, #_name)) \
    { \
      if (strEQc (_STR(action), "decode") || !memcmp (_STR(action), "in", 2)) { \
        obj->name = (char*) #_name; \
        obj->dxfname = (char*) #_name; \
        obj->fixedtype = DWG_TYPE_##_name; \
        obj->klass = klass; \
      } \
      return DWG_FUNC_N(action,_name) (dat, obj); \
    }
#define STABLE_CLASS_DXF(action, _name, _dxfname) \
  if (klass->dxfname && strEQc (klass->dxfname, #_dxfname)) \
    { \
      if (strEQc (_STR(action), "decode") || !memcmp (_STR(action), "in", 2)) { \
        obj->name = (char*) #_name; \
        obj->dxfname = (char*) #_dxfname; \
        obj->fixedtype = DWG_TYPE_##_name; \
        obj->klass = klass; \
      } \
      return DWG_FUNC_N(action,_name) (dat, obj); \
    }
#define STABLE_CLASS_CPP(action, _name, _cppname) \
  if (klass->cppname && strEQc (klass->cppname, #_name)) \
    { \
      if (strEQc (_STR(action), "decode") || !memcmp (_STR(action), "in", 2)) { \
        obj->name = (char*) #_name; \
        obj->dxfname = (char*) #_name; \
        obj->fixedtype = DWG_TYPE_##_name; \
        obj->klass = klass; \
      } \
      return DWG_FUNC_N(action,_name) (dat, obj); \
    }
#define UNSTABLE_CLASS(action, _name) \
  if (klass->dxfname && strEQc (klass->dxfname, #_name)) \
    { \
      WARN_UNSTABLE_CLASS; \
      if (strEQc (_STR(action), "decode") || !memcmp (_STR(action), "in", 2)) { \
        obj->name = (char*) #_name; \
        obj->dxfname = (char*) #_name; \
        obj->fixedtype = DWG_TYPE_##_name; \
        obj->klass = klass; \
      } \
      return DEBUGGING_DXF(action, name) DWG_FUNC_N(action,_name) (dat, obj); \
    }
#define UNSTABLE_CLASS_DXF(action, _name, _dxfname) \
  if (klass->dxfname && strEQc (klass->dxfname, #_dxfname)) \
    { \
      WARN_UNSTABLE_CLASS; \
      if (strEQc (_STR(action), "decode") || !memcmp (_STR(action), "in", 2)) { \
        obj->name = (char*) #_name; \
        obj->dxfname = (char*) #_dxfname; \
        obj->fixedtype = DWG_TYPE_##_name; \
        obj->klass = klass; \
      } \
      return DEBUGGING_DXF(action, name) DWG_FUNC_N(action,_name) (dat, obj); \
    }
#define UNSTABLE_CLASS_CPP(action, _name, _cppname) \
  if (klass->cppname && strEQc (klass->cppname, #_cppname)) \
    { \
      WARN_UNSTABLE_CLASS; \
      if (strEQc (_STR(action), "decode") || !memcmp (_STR(action), "in", 2)) { \
        obj->name = (char*) #_name; \
        obj->dxfname = (char*) #_name; \
        obj->fixedtype = DWG_TYPE_##_name; \
        obj->klass = klass; \
      } \
      return DEBUGGING_DXF(action, name) DWG_FUNC_N(action,_name) (dat, obj); \
    }
#define UNHANDLED_CLASS(action, _name) \
  if (klass->dxfname && strEQc (klass->dxfname, #_name)) \
    { \
      WARN_UNHANDLED_CLASS; \
      if (strEQc (_STR(action), "decode") || !memcmp (_STR(action), "in", 2)) { \
        obj->name = (char*) #_name; \
        obj->dxfname = (char*) #_name; \
        obj->klass = klass; \
      } \
      return DWG_ERR_UNHANDLEDCLASS; \
    }
#define UNHANDLED_CLASS_DXF(action, _name, _dxfname) \
  if (klass->dxfname && strEQc (klass->dxfname, #_dxfname)) \
    { \
      WARN_UNHANDLED_CLASS; \
      if (strEQc (_STR(action), "decode") || !memcmp (_STR(action), "in", 2)) { \
        obj->name = (char*) #_name; \
        obj->dxfname = (char*) #_dxfname; \
        obj->klass = klass; \
      } \
      return DWG_ERR_UNHANDLEDCLASS; \
    }
#define UNHANDLED_CLASS_CPP(action, _name, _cppname) \
  if (klass->cppname && strEQc (klass->cppname, #_cppname)) \
    { \
      WARN_UNHANDLED_CLASS; \
      if (strEQc (_STR(action), "decode") || !memcmp (_STR(action), "in", 2)) { \
        obj->name = (char*) #_name; \
        obj->dxfname = (char*) #_name; \
        obj->klass = klass; \
      } \
      return DWG_ERR_UNHANDLEDCLASS; \
    }

  /* Entities */
  STABLE_CLASS    (ACTION, CAMERA)    /* ent, not persistent in a DWG */
  STABLE_CLASS    (ACTION, IMAGE)     /* ent */
  STABLE_CLASS    (ACTION, LIGHT)     /* ent */
  STABLE_CLASS    (ACTION, LWPOLYLINE)
  STABLE_CLASS    (ACTION, MESH)      /* ent */
  STABLE_CLASS    (ACTION, OLE2FRAME) /* ent (also fixed) */
  STABLE_CLASS_DXF(ACTION, UNDERLAY, PDFUNDERLAY)
  STABLE_CLASS_DXF(ACTION, UNDERLAY, DGNUNDERLAY)
  STABLE_CLASS_DXF(ACTION, UNDERLAY, DWFUNDERLAY)
  STABLE_CLASS    (ACTION, WIPEOUT)   /* ent */

  /* Objects */
  STABLE_CLASS    (ACTION, CELLSTYLEMAP)
  STABLE_CLASS    (ACTION, DICTIONARYVAR)
  STABLE_CLASS_DXF(ACTION, DICTIONARYWDFLT, ACDBDICTIONARYWDFLT)
  STABLE_CLASS_DXF(ACTION, DYNAMICBLOCKPURGEPREVENTER, ACDB_DYNAMICBLOCKPURGEPREVENTER_VERSION)
  STABLE_CLASS    (ACTION, FIELD)
  STABLE_CLASS    (ACTION, FIELDLIST)
  STABLE_CLASS    (ACTION, GEODATA)
  STABLE_CLASS    (ACTION, GROUP)
  STABLE_CLASS    (ACTION, HATCH)
  STABLE_CLASS    (ACTION, IDBUFFER)
  STABLE_CLASS    (ACTION, IMAGEDEF)
  STABLE_CLASS    (ACTION, IMAGEDEF_REACTOR)
  STABLE_CLASS    (ACTION, LAYER_INDEX)
  STABLE_CLASS    (ACTION, LAYERFILTER)
  STABLE_CLASS    (ACTION, LAYOUT)       // includes PLOTSETTINGS
  STABLE_CLASS    (ACTION, MLEADERSTYLE) // TODO block_rotation nan
  STABLE_CLASS    (ACTION, MULTILEADER)       /* ent */
  STABLE_CLASS_DXF(ACTION, PLACEHOLDER, ACDBPLACEHOLDER)
  STABLE_CLASS    (ACTION, PLOTSETTINGS)
  STABLE_CLASS    (ACTION, RASTERVARIABLES)
  STABLE_CLASS    (ACTION, SCALE)
  STABLE_CLASS    (ACTION, SORTENTSTABLE)
  STABLE_CLASS    (ACTION, SPATIAL_FILTER)
  STABLE_CLASS    (ACTION, TABLEGEOMETRY)
  STABLE_CLASS    (ACTION, UNDERLAY)              /* ent DGN DWF PDF */
  STABLE_CLASS    (ACTION, UNDERLAYDEFINITION)    // only tested pdf, but very simple
  STABLE_CLASS_DXF(ACTION, UNDERLAYDEFINITION, PDFDEFINITION)
  STABLE_CLASS_DXF(ACTION, UNDERLAYDEFINITION, DGNDEFINITION)
  STABLE_CLASS_DXF(ACTION, UNDERLAYDEFINITION, DWFDEFINITION)
  STABLE_CLASS    (ACTION, VBA_PROJECT)
  STABLE_CLASS    (ACTION, VISUALSTYLE)
  STABLE_CLASS    (ACTION, WIPEOUTVARIABLES)
  STABLE_CLASS    (ACTION, XRECORD)

  // unstable/undertested.
  // not enough coverage, but assumed mostly ok. field names may change.
  // DXF output skipped
  UNSTABLE_CLASS     (ACTION, ARC_DIMENSION)     /* ent */
  UNSTABLE_CLASS_DXF (ACTION, ASSOCDEPENDENCY, ACDBASSOCDEPENDENCY)
  UNSTABLE_CLASS_DXF (ACTION, ASSOCALIGNEDDIMACTIONBODY, ACDBASSOCALIGNEDDIMACTIONBODY)
  UNSTABLE_CLASS_DXF (ACTION, ASSOCPLANESURFACEACTIONBODY, ACDBASSOCPLANESURFACEACTIONBODY)
  UNSTABLE_CLASS     (ACTION, DIMASSOC)
  UNSTABLE_CLASS     (ACTION, DBCOLOR)
  UNSTABLE_CLASS     (ACTION, HELIX)             /* ent */
  UNSTABLE_CLASS     (ACTION, LIGHTLIST)
  UNSTABLE_CLASS     (ACTION, MATERIAL)
  UNSTABLE_CLASS     (ACTION, MENTALRAYRENDERSETTINGS)
  UNSTABLE_CLASS     (ACTION, OBJECT_PTR)
  UNSTABLE_CLASS_CPP (ACTION, OBJECT_PTR, CAseDLPNTableRecord)
  UNSTABLE_CLASS_DXF (ACTION, PROXY_OBJECT, PROXY)
  UNSTABLE_CLASS_DXF (ACTION, PERSUBENTMGR, ACDBPERSSUBENTMANAGER)
  UNSTABLE_CLASS     (ACTION, RAPIDRTRENDERSETTINGS)
  UNSTABLE_CLASS     (ACTION, RENDERSETTINGS)
  UNSTABLE_CLASS     (ACTION, SPATIAL_INDEX)
  UNSTABLE_CLASS     (ACTION, TABLESTYLE)        // stable, but for r2010+ some bits missing at the end
  UNSTABLE_CLASS     (ACTION, SECTIONOBJECT)     /* ent */
  UNSTABLE_CLASS     (ACTION, SECTION_MANAGER)
  UNSTABLE_CLASS     (ACTION, SUN)

  // coverage exists, but broken and being worked on. needs -DDEBUG_CLASSES. See also
  // examples/unknown with all the unknown blobs of these.
  // PROXY_ENTITY has a fixed type
  // DXF output skipped, but some INDXF are done.
  DEBUGGING_CLASS     (ACTION, GEOPOSITIONMARKER) /* ent, yet unsorted. no coverage */
  DEBUGGING_CLASS     (ACTION, NURBSURFACE)       /* ent */
  DEBUGGING_CLASS     (ACTION, PLANESURFACE)      /* ent */
  DEBUGGING_CLASS     (ACTION, EXTRUDEDSURFACE)   /* ent */
  DEBUGGING_CLASS     (ACTION, LOFTEDSURFACE)     /* ent */
  DEBUGGING_CLASS     (ACTION, REVOLVEDSURFACE)   /* ent */
  DEBUGGING_CLASS     (ACTION, SWEPTSURFACE)      /* ent */
  DEBUGGING_CLASS_DXF (ACTION, TABLE, ACAD_TABLE) /* ent, r2010+ needs subclassing */
  DEBUGGING_CLASS     (ACTION, ATEXT)             // ent expresstools
  DEBUGGING_CLASS     (ACTION, RTEXT)             // ent expresstools
  DEBUGGING_CLASS_CPP (ACTION, NAVISWORKSMODEL, AcDbNavisworksModel)   /* ent,
                                                     dxfname COORDINATION_MODEL? */
  DEBUGGING_CLASS_CPP (ACTION, TABLECONTENT, AcDbTableContent) // dxfname: "TABLE"
  DEBUGGING_CLASS     (ACTION, SUNSTUDY)     // almost
  DEBUGGING_CLASS     (ACTION, ACSH_SWEEP_CLASS)
  DEBUGGING_CLASS     (ACTION, ACSH_BOX_CLASS)
  DEBUGGING_CLASS     (ACTION, ACSH_EXTRUSION_CLASS)
  DEBUGGING_CLASS     (ACTION, GEOMAPIMAGE)  // LiveMap image overlay
  DEBUGGING_CLASS_DXF (ACTION, NAVISWORKSMODELDEF, ACDBNAVISWORKSMODELDEF)
  DEBUGGING_CLASS_DXF (ACTION, ASSOCACTION, ACDBASSOCACTION)
  DEBUGGING_CLASS_DXF (ACTION, ASSOCNETWORK, ACDBASSOCNETWORK) /* very close */
  DEBUGGING_CLASS_DXF (ACTION, ASSOCEXTRUDEDSURFACEACTIONBODY, ACDBASSOCEXTRUDEDSURFACEACTIONBODY)
  DEBUGGING_CLASS_DXF (ACTION, ASSOCLOFTEDSURFACEACTIONBODY, ACDBASSOCLOFTEDSURFACEACTIONBODY)
  DEBUGGING_CLASS_DXF (ACTION, ASSOCREVOLVEDSURFACEACTIONBODY, ACDBASSOCREVOLVEDSURFACEACTIONBODY)
  DEBUGGING_CLASS_DXF (ACTION, ASSOCSWEPTSURFACEACTIONBODY, ACDBASSOCSWEPTSURFACEACTIONBODY)
  DEBUGGING_CLASS_DXF (ACTION, ASSOCOSNAPPOINTREFACTIONPARAM, ACDBASSOCOSNAPPOINTREFACTIONPARAM)
  DEBUGGING_CLASS_DXF (ACTION, ASSOCPERSSUBENTMANAGER, ACDBASSOCPERSSUBENTMANAGER)
  DEBUGGING_CLASS_DXF (ACTION, ASSOC2DCONSTRAINTGROUP, ACDBASSOC2DCONSTRAINTGROUP)
  DEBUGGING_CLASS_DXF (ACTION, EVALUATION_GRAPH, ACAD_EVALUATION_GRAPH) /* almost */
  DEBUGGING_CLASS     (ACTION, RENDERENVIRONMENT) // no coverage
  DEBUGGING_CLASS     (ACTION, RENDERENTRY)       // no coverage
  DEBUGGING_CLASS     (ACTION, RENDERGLOBAL)      // no coverage
  DEBUGGING_CLASS     (ACTION, DATALINK)
  DEBUGGING_CLASS     (ACTION, DATATABLE)
  DEBUGGING_CLASS_DXF (ACTION, DETAILVIEWSTYLE, ACDBDETAILVIEWSTYLE) // excellent coverage
  DEBUGGING_CLASS_DXF (ACTION, SECTIONVIEWSTYLE, ACDBSECTIONVIEWSTYLE)
  DEBUGGING_CLASS     (ACTION, LAYOUTPRINTCONFIG)
  DEBUGGING_CLASS_DXF (ACTION, ALDIMOBJECTCONTEXTDATA, ACDB_ALDIMOBJECTCONTEXTDATA_CLASS)
  DEBUGGING_CLASS_DXF (ACTION, BLKREFOBJECTCONTEXTDATA, ACDB_BLKREFOBJECTCONTEXTDATA_CLASS)
  DEBUGGING_CLASS_DXF (ACTION, FCFOBJECTCONTEXTDATA, ACDB_FCFOBJECTCONTEXTDATA_CLASS)
  DEBUGGING_CLASS_DXF (ACTION, LEADEROBJECTCONTEXTDATA, ACDB_LEADEROBJECTCONTEXTDATA_CLASS)
  DEBUGGING_CLASS_DXF (ACTION, MLEADEROBJECTCONTEXTDATA, ACDB_MLEADEROBJECTCONTEXTDATA_CLASS)
  DEBUGGING_CLASS_DXF (ACTION, MTEXTATTRIBUTEOBJECTCONTEXTDATA, ACDB_MTEXTATTRIBUTEOBJECTCONTEXTDATA_CLASS)
  DEBUGGING_CLASS_DXF (ACTION, MTEXTOBJECTCONTEXTDATA, ACDB_MTEXTOBJECTCONTEXTDATA_CLASS)
  DEBUGGING_CLASS_DXF (ACTION, TEXTOBJECTCONTEXTDATA,  ACDB_TEXTOBJECTCONTEXTDATA_CLASS)
  DEBUGGING_CLASS     (ACTION, ACMECOMMANDHISTORY)
  DEBUGGING_CLASS     (ACTION, ACMESCOPE)
  DEBUGGING_CLASS     (ACTION, ACMESTATEMGR)
  DEBUGGING_CLASS     (ACTION, ACSH_HISTORY_CLASS)
  DEBUGGING_CLASS_DXF (ACTION, ASSOCGEOMDEPENDENCY, ACDBASSOCGEOMDEPENDENCY)
  DEBUGGING_CLASS_DXF (ACTION, ASSOCOSNAPPOINTREFACTIONPARAM, ACDBASSOCOSNAPPOINTREFACTIONPARAM)
  DEBUGGING_CLASS_DXF (ACTION, ASSOCVERTEXACTIONPARAM, ACDBASSOCVERTEXACTIONPARAM)
  DEBUGGING_CLASS     (ACTION, CSACDOCUMENTOPTIONS)
  DEBUGGING_CLASS     (ACTION, SECTION_SETTINGS)
  DEBUGGING_CLASS_DXF (ACTION, CURVEPATH, ACDBCURVEPATH)
  DEBUGGING_CLASS     (ACTION, MOTIONPATH)
  DEBUGGING_CLASS_DXF (ACTION, POINTPATH, ACDBPOINTPATH)
  DEBUGGING_CLASS     (ACTION, TVDEVICEPROPERTIES)
  DEBUGGING_CLASS     (ACTION, BLOCKVISIBILITYGRIP)
  DEBUGGING_CLASS     (ACTION, BLOCKVISIBILITYPARAMETER)
  DEBUGGING_CLASS     (ACTION, BLOCKGRIPLOCATIONCOMPONENT)

  // unimplemented, passed through. coverage exists mostly, but documentation not.
  // DXF output skipped
  UNHANDLED_CLASS     (ACTION, ACDSRECORD)
  UNHANDLED_CLASS     (ACTION, ACDSSCHEMA)
  UNHANDLED_CLASS     (ACTION, ACSH_REVOLVE_CLASS)
  UNHANDLED_CLASS     (ACTION, ACSH_SPHERE_CLASS)
  UNHANDLED_CLASS_DXF (ACTION, BACKGROUND, ACAD_BACKGROUND)
  UNHANDLED_CLASS     (ACTION, NPOCOLLECTION)
  UNHANDLED_CLASS_DXF (ACTION, POINTCLOUD, ACDBPOINTCLOUD)
  UNHANDLED_CLASS     (ACTION, RAPIDRTRENDERENVIRONMENT)  //no coverage
  UNHANDLED_CLASS_DXF (ACTION, XREFPANELOBJECT, EXACXREFPANELOBJECT)
  UNHANDLED_CLASS     (ACTION, VISIBILITYGRIPENTITY)
  UNHANDLED_CLASS     (ACTION, VISIBILITYPARAMETERENTITY)

/* Missing DXF names:
   ACAD_PROXY_ENTITY
   ACDBPOINTCLOUDEX
   ARRAY
   ATTBLOCKREF
   ATTDYNBLOCKREF
   BLOCKREF
   DYNBLOCKREF
   XREF
   CENTERMARK
   CENTERLINE
*/

/* Teigha does not export:
   ACAD_EVALUATION_GRAPH
   ACSH_BOX_CLASS
   ACSH_EXTRUSION_CLASS
   ACSH_HISTORY_CLASS
   ACSH_SWEEP_CLASS
   MENTALRAYRENDERSETTINGS
 */
