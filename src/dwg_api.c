/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2013,2018-2025 Free Software Foundation, Inc.              */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * dwg_api.c: external C API
 *
 * written by Gaganjyot Singh
 * modified by Reini Urban
 */

#include "config.h"
#ifdef HAVE_SINCOS
#  define _GNU_SOURCE 1
#endif
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <math.h>
#include <time.h>
#include <ctype.h>
#include <assert.h>
#ifdef HAVE_WCTYPE_H
#  include <wctype.h>
#endif
#ifdef HAVE_MALLOC_H
#  include <malloc.h>
#endif

#include "dwg.h"
#define _DWG_API_C
#include "common.h"
#define DWG_LOGLEVEL loglevel
#include "logging.h"
#include "dwg_api.h"

#include "bits.h"
#include "classes.h"
#include "decode.h"
#include "encode.h"
#include "hash.h"

#undef NEED_VPORT_FOR_MODEL_LAYOUT

/** We don't pass in Dwg_Object*'s, so we don't know if the object
 *  is >= r2007 or <r13 or what. Default is r2000.
 *  So we need some dwg_api_init_version(&dwg) to store the version.
 *  This is not thread-safe, don't use different DWG versions in parallel
 * threads!
 */
static Dwg_Version_Type dwg_version = R_INVALID;
static unsigned int loglevel = DWG_LOGLEVEL_ERROR;
static unsigned nodeid = 0;

/* Non-public imports */
/* I don't want to export these. */
BITCODE_H dwg_find_tablehandle_silent (Dwg_Data *restrict dwg,
                                       const char *restrict name,
                                       const char *restrict table);
/* Initialization hack only */
void dwg_set_next_hdl (Dwg_Data *dwg, BITCODE_RLL value);
void dwg_set_next_objhandle (Dwg_Object *obj);
/* from dxfclasses */
int dwg_dxfclass_require (Dwg_Data *restrict dwg,
                          const char *restrict dxfname);

// #ifdef USE_WRITE
/* internally used only by dwg_add_POLYLINE* only */
// fixme: Dwg_Entity_POLYLINE_2D* as 1st owner arg
Dwg_Entity_VERTEX_2D *
dwg_add_VERTEX_2D (Dwg_Entity_POLYLINE_2D *restrict pline,
                   const dwg_point_2d *restrict point) __nonnull_all;
Dwg_Entity_VERTEX_3D *
dwg_add_VERTEX_3D (Dwg_Entity_POLYLINE_3D *restrict pline,
                   const dwg_point_3d *restrict point) __nonnull_all;
Dwg_Entity_VERTEX_MESH *
dwg_add_VERTEX_MESH (Dwg_Entity_POLYLINE_MESH *restrict pline,
                     const dwg_point_3d *restrict point) __nonnull_all;
Dwg_Entity_VERTEX_PFACE *
dwg_add_VERTEX_PFACE (Dwg_Entity_POLYLINE_PFACE *restrict pline,
                      const dwg_point_3d *restrict point) __nonnull_all;
Dwg_Entity_VERTEX_PFACE_FACE *
dwg_add_VERTEX_PFACE_FACE (Dwg_Entity_POLYLINE_PFACE *restrict pline,
                           const dwg_face vertind) __nonnull_all;

// #endif

/* Generic version api */
EXPORT const char *
dwg_api_version_string (void)
{
  return PACKAGE_VERSION;
}
EXPORT int
dwg_api_version (void)
{
  return LIBREDWG_VERSION;
}
EXPORT int
dwg_api_version_major (void)
{
  return LIBREDWG_VERSION_MAJOR;
}
EXPORT int
dwg_api_version_minor (void)
{
  return LIBREDWG_VERSION_MINOR;
}
EXPORT bool
dwg_api_version_is_release (void)
{
#ifdef IS_RELEASE
  const char *s = PACKAGE_VERSION;
  int count_dots = 0; // 0.13.3 vs 0.13.3.7539.2_0c711
  if (!s)
    return false;
  while (*s)
    {
      if (*s == '.')
        count_dots++;
      s++;
    }
  return count_dots < 3; // not patched, has a tag.
#else
  return false;
#endif
}
EXPORT const char *
dwg_api_so_version (void)
{
  return LIBREDWG_SO_VERSION;
}

/**
 * Return an object fieldvalue
 */
#ifndef HAVE_NONNULL
#  define dwg_get_OBJECT(name, OBJECT)                                        \
    EXPORT bool dwg_get_##OBJECT (const dwg_##name *restrict name,            \
                                  const char *restrict fieldname,             \
                                  void *restrict out)                         \
    {                                                                         \
      if (name && fieldname && out)                                           \
        return dwg_dynapi_entity_value ((void *)name, #OBJECT, fieldname,     \
                                        out, NULL);                           \
      else                                                                    \
        return false;                                                         \
    }                                                                         \
    EXPORT bool dwg_set_##OBJECT (const dwg_##name *restrict name,            \
                                  const char *restrict fieldname,             \
                                  void *restrict value)                       \
    {                                                                         \
      if (name && fieldname && value)                                         \
        return dwg_dynapi_entity_set_value ((void *)name, #OBJECT, fieldname, \
                                            value, 1);                        \
      else                                                                    \
        return false;                                                         \
    }
#else
#  define dwg_get_OBJECT(name, OBJECT)                                        \
    EXPORT bool dwg_get_##OBJECT (const dwg_##name *restrict name,            \
                                  const char *restrict fieldname,             \
                                  void *restrict out)                         \
    {                                                                         \
      return dwg_dynapi_entity_value ((void *)name, #OBJECT, fieldname, out,  \
                                      NULL);                                  \
    }                                                                         \
    EXPORT bool dwg_set_##OBJECT (const dwg_##name *restrict name,            \
                                  const char *restrict fieldname,             \
                                  void *restrict value)                       \
    {                                                                         \
      return dwg_dynapi_entity_set_value ((void *)name, #OBJECT, fieldname,   \
                                          value, 0);                          \
    }
#endif

#ifdef __AFL_COMPILER
// clang-format: off

/* Most API functions are now auto-generated, but excluded
   from afl-fuzz, because there would be too many.
   These are the exceptions, needed in dwgwrite, ... */

EXPORT
dwg_get_OBJECT (ent_circle, CIRCLE)
dwg_get_OBJECT (ent_line, LINE)

CAST_DWG_OBJECT_TO_ENTITY (INSERT)
CAST_DWG_OBJECT_TO_ENTITY (LINE)
CAST_DWG_OBJECT_TO_ENTITY (CIRCLE)
CAST_DWG_OBJECT_TO_ENTITY (TEXT)
CAST_DWG_OBJECT_TO_ENTITY (ARC)
CAST_DWG_OBJECT_TO_ENTITY (VERTEX_2D)
CAST_DWG_OBJECT_TO_ENTITY (VERTEX_3D)
CAST_DWG_OBJECT_TO_OBJECT (BLOCK_HEADER)

// clang-format: on
#endif

#ifndef __AFL_COMPILER

// clang-format: off
dwg_get_OBJECT (ent__3dface, _3DFACE)
dwg_get_OBJECT (ent__3dsolid, _3DSOLID)
dwg_get_OBJECT (ent_arc, ARC)
dwg_get_OBJECT (ent_attdef, ATTDEF)
dwg_get_OBJECT (ent_attrib, ATTRIB)
dwg_get_OBJECT (ent_block, BLOCK)
dwg_get_OBJECT (ent_circle, CIRCLE)
dwg_get_OBJECT (ent_dim_aligned, DIMENSION_ALIGNED)
dwg_get_OBJECT (ent_dim_ang2ln, DIMENSION_ANG2LN)
dwg_get_OBJECT (ent_dim_ang3pt, DIMENSION_ANG3PT)
dwg_get_OBJECT (ent_dim_diameter, DIMENSION_DIAMETER)
dwg_get_OBJECT (ent_dim_linear, DIMENSION_LINEAR)
dwg_get_OBJECT (ent_dim_ordinate, DIMENSION_ORDINATE)
dwg_get_OBJECT (ent_dim_radius, DIMENSION_RADIUS)
dwg_get_OBJECT (ent_ellipse, ELLIPSE)
dwg_get_OBJECT (ent_endblk, ENDBLK)
dwg_get_OBJECT (ent_insert, INSERT)
dwg_get_OBJECT (ent_leader, LEADER)
dwg_get_OBJECT (ent_line, LINE)
dwg_get_OBJECT (ent_load, LOAD)
dwg_get_OBJECT (ent_minsert, MINSERT)
dwg_get_OBJECT (ent_mline, MLINE)
dwg_get_OBJECT (ent_mtext, MTEXT)
dwg_get_OBJECT (ent_oleframe, OLEFRAME)
dwg_get_OBJECT (ent_point, POINT)
dwg_get_OBJECT (ent_polyline_2d, POLYLINE_2D)
dwg_get_OBJECT (ent_polyline_3d, POLYLINE_3D)
dwg_get_OBJECT (ent_polyline_mesh, POLYLINE_MESH)
dwg_get_OBJECT (ent_polyline_pface, POLYLINE_PFACE)
dwg_get_OBJECT (ent_proxy_entity, PROXY_ENTITY)
dwg_get_OBJECT (ent_ray, RAY)
dwg_get_OBJECT (ent_seqend, SEQEND)
dwg_get_OBJECT (ent_shape, SHAPE)
dwg_get_OBJECT (ent_solid, SOLID)
dwg_get_OBJECT (ent_spline, SPLINE)
dwg_get_OBJECT (ent_text, TEXT)
dwg_get_OBJECT (ent_tolerance, TOLERANCE)
dwg_get_OBJECT (ent_trace, TRACE)
dwg_get_OBJECT (ent_unknown_ent, UNKNOWN_ENT)
dwg_get_OBJECT (ent_vertex_2d, VERTEX_2D)
dwg_get_OBJECT (ent_vertex_3d, VERTEX_3D)
dwg_get_OBJECT (ent_vert_pface_face, VERTEX_PFACE_FACE)
dwg_get_OBJECT (ent_viewport, VIEWPORT)

/* Start auto-generated content. Do not touch. */
// clang-format: off
/* untyped > 500 */
dwg_get_OBJECT (ent__3dline, _3DLINE)
dwg_get_OBJECT (ent_camera, CAMERA)
dwg_get_OBJECT (ent_dgnunderlay, DGNUNDERLAY)
dwg_get_OBJECT (ent_dwfunderlay, DWFUNDERLAY)
dwg_get_OBJECT (ent_endrep, ENDREP)
dwg_get_OBJECT (ent_hatch, HATCH)
dwg_get_OBJECT (ent_image, IMAGE)
dwg_get_OBJECT (ent_jump, JUMP)
dwg_get_OBJECT (ent_light, LIGHT)
dwg_get_OBJECT (ent_lwpline, LWPOLYLINE)
dwg_get_OBJECT (ent_mesh, MESH)
dwg_get_OBJECT (ent_mleader, MULTILEADER)
dwg_get_OBJECT (ent_ole2frame, OLE2FRAME)
dwg_get_OBJECT (ent_pdfunderlay, PDFUNDERLAY)
dwg_get_OBJECT (ent_repeat, REPEAT)
dwg_get_OBJECT (ent_sectionobject, SECTIONOBJECT)
dwg_get_OBJECT (ent_wipeout, WIPEOUT)
/* unstable */
dwg_get_OBJECT (ent_arc_dimension, ARC_DIMENSION)
dwg_get_OBJECT (ent_helix, HELIX)
dwg_get_OBJECT (ent_large_radial_dimension, LARGE_RADIAL_DIMENSION)
dwg_get_OBJECT (ent_layoutprintconfig, LAYOUTPRINTCONFIG)
dwg_get_OBJECT (ent_planesurface, PLANESURFACE)
dwg_get_OBJECT (ent_pointcloud, POINTCLOUD)
dwg_get_OBJECT (ent_pointcloudex, POINTCLOUDEX)
#  ifdef DEBUG_CLASSES
dwg_get_OBJECT (ent_alignmentparameterentity, ALIGNMENTPARAMETERENTITY)
dwg_get_OBJECT (ent_arcalignedtext, ARCALIGNEDTEXT)
dwg_get_OBJECT (ent_basepointparameterentity, BASEPOINTPARAMETERENTITY)
dwg_get_OBJECT (ent_extrudedsurface, EXTRUDEDSURFACE)
dwg_get_OBJECT (ent_flipgripentity, FLIPGRIPENTITY)
dwg_get_OBJECT (ent_flipparameterentity, FLIPPARAMETERENTITY)
dwg_get_OBJECT (ent_geopositionmarker, GEOPOSITIONMARKER)
dwg_get_OBJECT (ent_lineargripentity, LINEARGRIPENTITY)
dwg_get_OBJECT (ent_linearparameterentity, LINEARPARAMETERENTITY)
dwg_get_OBJECT (ent_loftedsurface, LOFTEDSURFACE)
dwg_get_OBJECT (ent_mpolygon, MPOLYGON)
dwg_get_OBJECT (ent_navisworksmodel, NAVISWORKSMODEL)
dwg_get_OBJECT (ent_nurbsurface, NURBSURFACE)
dwg_get_OBJECT (ent_pointparameterentity, POINTPARAMETERENTITY)
dwg_get_OBJECT (ent_polargripentity, POLARGRIPENTITY)
dwg_get_OBJECT (ent_revolvedsurface, REVOLVEDSURFACE)
dwg_get_OBJECT (ent_rotationgripentity, ROTATIONGRIPENTITY)
dwg_get_OBJECT (ent_rotationparameterentity, ROTATIONPARAMETERENTITY)
dwg_get_OBJECT (ent_rtext, RTEXT)
dwg_get_OBJECT (ent_sweptsurface, SWEPTSURFACE)
dwg_get_OBJECT (ent_table, TABLE)
dwg_get_OBJECT (ent_visibilitygripentity, VISIBILITYGRIPENTITY)
dwg_get_OBJECT (ent_visibilityparameterentity, VISIBILITYPARAMETERENTITY)
dwg_get_OBJECT (ent_xygripentity, XYGRIPENTITY)
dwg_get_OBJECT (ent_xyparameterentity, XYPARAMETERENTITY)
#  endif

dwg_get_OBJECT (obj_appid, APPID)
dwg_get_OBJECT (obj_appid_control, APPID_CONTROL)
dwg_get_OBJECT (obj_block_control, BLOCK_CONTROL)
dwg_get_OBJECT (obj_block_header, BLOCK_HEADER)
dwg_get_OBJECT (obj_dictionary, DICTIONARY)
dwg_get_OBJECT (obj_dimstyle, DIMSTYLE)
dwg_get_OBJECT (obj_dimstyle_control, DIMSTYLE_CONTROL)
dwg_get_OBJECT (obj_dummy, DUMMY)
dwg_get_OBJECT (obj_layer, LAYER)
dwg_get_OBJECT (obj_layer_control, LAYER_CONTROL)
dwg_get_OBJECT (obj_long_transaction, LONG_TRANSACTION)
dwg_get_OBJECT (obj_ltype, LTYPE)
dwg_get_OBJECT (obj_ltype_control, LTYPE_CONTROL)
dwg_get_OBJECT (obj_mlinestyle, MLINESTYLE)
dwg_get_OBJECT (obj_style, STYLE)
dwg_get_OBJECT (obj_style_control, STYLE_CONTROL)
dwg_get_OBJECT (obj_ucs, UCS)
dwg_get_OBJECT (obj_ucs_control, UCS_CONTROL)
dwg_get_OBJECT (obj_unknown_obj, UNKNOWN_OBJ)
dwg_get_OBJECT (obj_view, VIEW)
dwg_get_OBJECT (obj_view_control, VIEW_CONTROL)
dwg_get_OBJECT (obj_vport, VPORT)
dwg_get_OBJECT (obj_vport_control, VPORT_CONTROL)
dwg_get_OBJECT (obj_vx_control, VX_CONTROL)
dwg_get_OBJECT (obj_vx_table_record, VX_TABLE_RECORD)
/* untyped > 500 */
dwg_get_OBJECT (obj_acsh_boolean_class, ACSH_BOOLEAN_CLASS)
dwg_get_OBJECT (obj_acsh_box_class, ACSH_BOX_CLASS)
dwg_get_OBJECT (obj_acsh_cone_class, ACSH_CONE_CLASS)
dwg_get_OBJECT (obj_acsh_cylinder_class, ACSH_CYLINDER_CLASS)
dwg_get_OBJECT (obj_acsh_fillet_class, ACSH_FILLET_CLASS)
dwg_get_OBJECT (obj_acsh_history_class, ACSH_HISTORY_CLASS)
dwg_get_OBJECT (obj_acsh_sphere_class, ACSH_SPHERE_CLASS)
dwg_get_OBJECT (obj_acsh_torus_class, ACSH_TORUS_CLASS)
dwg_get_OBJECT (obj_acsh_wedge_class, ACSH_WEDGE_CLASS)
dwg_get_OBJECT (obj_assocgeomdependency, ASSOCGEOMDEPENDENCY)
dwg_get_OBJECT (obj_assocnetwork, ASSOCNETWORK)
dwg_get_OBJECT (obj_blockalignmentgrip, BLOCKALIGNMENTGRIP)
dwg_get_OBJECT (obj_blockalignmentparameter, BLOCKALIGNMENTPARAMETER)
dwg_get_OBJECT (obj_blockbasepointparameter, BLOCKBASEPOINTPARAMETER)
dwg_get_OBJECT (obj_blockflipaction, BLOCKFLIPACTION)
dwg_get_OBJECT (obj_blockflipgrip, BLOCKFLIPGRIP)
dwg_get_OBJECT (obj_blockflipparameter, BLOCKFLIPPARAMETER)
dwg_get_OBJECT (obj_blockgriplocationcomponent, BLOCKGRIPLOCATIONCOMPONENT)
dwg_get_OBJECT (obj_blocklineargrip, BLOCKLINEARGRIP)
dwg_get_OBJECT (obj_blocklookupgrip, BLOCKLOOKUPGRIP)
dwg_get_OBJECT (obj_blockmoveaction, BLOCKMOVEACTION)
dwg_get_OBJECT (obj_blockrotateaction, BLOCKROTATEACTION)
dwg_get_OBJECT (obj_blockrotationgrip, BLOCKROTATIONGRIP)
dwg_get_OBJECT (obj_blockscaleaction, BLOCKSCALEACTION)
dwg_get_OBJECT (obj_blockvisibilitygrip, BLOCKVISIBILITYGRIP)
dwg_get_OBJECT (obj_cellstylemap, CELLSTYLEMAP)
dwg_get_OBJECT (obj_detailviewstyle, DETAILVIEWSTYLE)
dwg_get_OBJECT (obj_dictionaryvar, DICTIONARYVAR)
dwg_get_OBJECT (obj_dictionarywdflt, DICTIONARYWDFLT)
dwg_get_OBJECT (obj_dynamicblockpurgepreventer, DYNAMICBLOCKPURGEPREVENTER)
dwg_get_OBJECT (obj_field, FIELD)
dwg_get_OBJECT (obj_fieldlist, FIELDLIST)
dwg_get_OBJECT (obj_geodata, GEODATA)
dwg_get_OBJECT (obj_group, GROUP)
dwg_get_OBJECT (obj_idbuffer, IDBUFFER)
dwg_get_OBJECT (obj_imagedef, IMAGEDEF)
dwg_get_OBJECT (obj_imagedef_reactor, IMAGEDEF_REACTOR)
dwg_get_OBJECT (obj_index, INDEX)
dwg_get_OBJECT (obj_layerfilter, LAYERFILTER)
dwg_get_OBJECT (obj_layer_index, LAYER_INDEX)
dwg_get_OBJECT (obj_layout, LAYOUT)
dwg_get_OBJECT (obj_placeholder, PLACEHOLDER)
dwg_get_OBJECT (obj_plotsettings, PLOTSETTINGS)
dwg_get_OBJECT (obj_rastervariables, RASTERVARIABLES)
dwg_get_OBJECT (obj_renderenvironment, RENDERENVIRONMENT)
dwg_get_OBJECT (obj_scale, SCALE)
dwg_get_OBJECT (obj_sectionviewstyle, SECTIONVIEWSTYLE)
dwg_get_OBJECT (obj_section_manager, SECTION_MANAGER)
dwg_get_OBJECT (obj_sortentstable, SORTENTSTABLE)
dwg_get_OBJECT (obj_spatial_filter, SPATIAL_FILTER)
dwg_get_OBJECT (obj_sun, SUN)
dwg_get_OBJECT (obj_tablegeometry, TABLEGEOMETRY)
dwg_get_OBJECT (obj_vba_project, VBA_PROJECT)
dwg_get_OBJECT (obj_visualstyle, VISUALSTYLE)
dwg_get_OBJECT (obj_wipeoutvariables, WIPEOUTVARIABLES)
dwg_get_OBJECT (obj_xrecord, XRECORD)
dwg_get_OBJECT (obj_pdfdefinition, PDFDEFINITION)
dwg_get_OBJECT (obj_dgndefinition, DGNDEFINITION)
dwg_get_OBJECT (obj_dwfdefinition, DWFDEFINITION)
/* unstable */
dwg_get_OBJECT (obj_acsh_brep_class, ACSH_BREP_CLASS)
dwg_get_OBJECT (obj_acsh_chamfer_class, ACSH_CHAMFER_CLASS)
dwg_get_OBJECT (obj_acsh_pyramid_class, ACSH_PYRAMID_CLASS)
dwg_get_OBJECT (obj_aldimobjectcontextdata, ALDIMOBJECTCONTEXTDATA)
dwg_get_OBJECT (obj_assoc2dconstraintgroup, ASSOC2DCONSTRAINTGROUP)
dwg_get_OBJECT (obj_assocaction, ASSOCACTION)
dwg_get_OBJECT (obj_assocactionparam, ASSOCACTIONPARAM)
dwg_get_OBJECT (obj_assocarrayactionbody, ASSOCARRAYACTIONBODY)
dwg_get_OBJECT (obj_assocasmbodyactionparam, ASSOCASMBODYACTIONPARAM)
dwg_get_OBJECT (obj_assocblendsurfaceactionbody, ASSOCBLENDSURFACEACTIONBODY)
dwg_get_OBJECT (obj_assoccompoundactionparam, ASSOCCOMPOUNDACTIONPARAM)
dwg_get_OBJECT (obj_assocdependency, ASSOCDEPENDENCY)
dwg_get_OBJECT (obj_assocdimdependencybody, ASSOCDIMDEPENDENCYBODY)
dwg_get_OBJECT (obj_assocextendsurfaceactionbody, ASSOCEXTENDSURFACEACTIONBODY)
dwg_get_OBJECT (obj_assocextrudedsurfaceactionbody, ASSOCEXTRUDEDSURFACEACTIONBODY)
dwg_get_OBJECT (obj_assocfaceactionparam, ASSOCFACEACTIONPARAM)
dwg_get_OBJECT (obj_assocfilletsurfaceactionbody, ASSOCFILLETSURFACEACTIONBODY)
dwg_get_OBJECT (obj_assocloftedsurfaceactionbody, ASSOCLOFTEDSURFACEACTIONBODY)
dwg_get_OBJECT (obj_assocnetworksurfaceactionbody, ASSOCNETWORKSURFACEACTIONBODY)
dwg_get_OBJECT (obj_assocobjectactionparam, ASSOCOBJECTACTIONPARAM)
dwg_get_OBJECT (obj_assocoffsetsurfaceactionbody, ASSOCOFFSETSURFACEACTIONBODY)
dwg_get_OBJECT (obj_assocosnappointrefactionparam, ASSOCOSNAPPOINTREFACTIONPARAM)
dwg_get_OBJECT (obj_assocpatchsurfaceactionbody, ASSOCPATCHSURFACEACTIONBODY)
dwg_get_OBJECT (obj_assocpathactionparam, ASSOCPATHACTIONPARAM)
dwg_get_OBJECT (obj_assocplanesurfaceactionbody, ASSOCPLANESURFACEACTIONBODY)
dwg_get_OBJECT (obj_assocpointrefactionparam, ASSOCPOINTREFACTIONPARAM)
dwg_get_OBJECT (obj_assocrevolvedsurfaceactionbody, ASSOCREVOLVEDSURFACEACTIONBODY)
dwg_get_OBJECT (obj_assoctrimsurfaceactionbody, ASSOCTRIMSURFACEACTIONBODY)
dwg_get_OBJECT (obj_assocvaluedependency, ASSOCVALUEDEPENDENCY)
dwg_get_OBJECT (obj_assocvariable, ASSOCVARIABLE)
dwg_get_OBJECT (obj_assocvertexactionparam, ASSOCVERTEXACTIONPARAM)
dwg_get_OBJECT (obj_blkrefobjectcontextdata, BLKREFOBJECTCONTEXTDATA)
dwg_get_OBJECT (obj_blockalignedconstraintparameter, BLOCKALIGNEDCONSTRAINTPARAMETER)
dwg_get_OBJECT (obj_blockangularconstraintparameter, BLOCKANGULARCONSTRAINTPARAMETER)
dwg_get_OBJECT (obj_blockarrayaction, BLOCKARRAYACTION)
dwg_get_OBJECT (obj_blockdiametricconstraintparameter, BLOCKDIAMETRICCONSTRAINTPARAMETER)
dwg_get_OBJECT (obj_blockhorizontalconstraintparameter, BLOCKHORIZONTALCONSTRAINTPARAMETER)
dwg_get_OBJECT (obj_blocklinearconstraintparameter, BLOCKLINEARCONSTRAINTPARAMETER)
dwg_get_OBJECT (obj_blocklinearparameter, BLOCKLINEARPARAMETER)
dwg_get_OBJECT (obj_blocklookupaction, BLOCKLOOKUPACTION)
dwg_get_OBJECT (obj_blocklookupparameter, BLOCKLOOKUPPARAMETER)
dwg_get_OBJECT (obj_blockparamdependencybody, BLOCKPARAMDEPENDENCYBODY)
dwg_get_OBJECT (obj_blockpointparameter, BLOCKPOINTPARAMETER)
dwg_get_OBJECT (obj_blockpolargrip, BLOCKPOLARGRIP)
dwg_get_OBJECT (obj_blockpolarparameter, BLOCKPOLARPARAMETER)
dwg_get_OBJECT (obj_blockpolarstretchaction, BLOCKPOLARSTRETCHACTION)
dwg_get_OBJECT (obj_blockradialconstraintparameter, BLOCKRADIALCONSTRAINTPARAMETER)
dwg_get_OBJECT (obj_blockrepresentation, BLOCKREPRESENTATION)
dwg_get_OBJECT (obj_blockrotationparameter, BLOCKROTATIONPARAMETER)
dwg_get_OBJECT (obj_blockstretchaction, BLOCKSTRETCHACTION)
dwg_get_OBJECT (obj_blockuserparameter, BLOCKUSERPARAMETER)
dwg_get_OBJECT (obj_blockverticalconstraintparameter, BLOCKVERTICALCONSTRAINTPARAMETER)
dwg_get_OBJECT (obj_blockvisibilityparameter, BLOCKVISIBILITYPARAMETER)
dwg_get_OBJECT (obj_blockxygrip, BLOCKXYGRIP)
dwg_get_OBJECT (obj_blockxyparameter, BLOCKXYPARAMETER)
dwg_get_OBJECT (obj_datalink, DATALINK)
dwg_get_OBJECT (obj_dbcolor, DBCOLOR)
dwg_get_OBJECT (obj_evaluation_graph, EVALUATION_GRAPH)
dwg_get_OBJECT (obj_fcfobjectcontextdata, FCFOBJECTCONTEXTDATA)
dwg_get_OBJECT (obj_gradient_background, GRADIENT_BACKGROUND)
dwg_get_OBJECT (obj_ground_plane_background, GROUND_PLANE_BACKGROUND)
dwg_get_OBJECT (obj_ibl_background, IBL_BACKGROUND)
dwg_get_OBJECT (obj_image_background, IMAGE_BACKGROUND)
dwg_get_OBJECT (obj_leaderobjectcontextdata, LEADEROBJECTCONTEXTDATA)
dwg_get_OBJECT (obj_lightlist, LIGHTLIST)
dwg_get_OBJECT (obj_material, MATERIAL)
dwg_get_OBJECT (obj_mentalrayrendersettings, MENTALRAYRENDERSETTINGS)
dwg_get_OBJECT (obj_mleaderstyle, MLEADERSTYLE)
dwg_get_OBJECT (obj_mtextobjectcontextdata, MTEXTOBJECTCONTEXTDATA)
dwg_get_OBJECT (obj_object_ptr, OBJECT_PTR)
dwg_get_OBJECT (obj_partial_viewing_index, PARTIAL_VIEWING_INDEX)
dwg_get_OBJECT (obj_pointcloudcolormap, POINTCLOUDCOLORMAP)
dwg_get_OBJECT (obj_pointclouddef, POINTCLOUDDEF)
dwg_get_OBJECT (obj_pointclouddefex, POINTCLOUDDEFEX)
dwg_get_OBJECT (obj_pointclouddef_reactor, POINTCLOUDDEF_REACTOR)
dwg_get_OBJECT (obj_pointclouddef_reactor_ex, POINTCLOUDDEF_REACTOR_EX)
dwg_get_OBJECT (obj_proxy_object, PROXY_OBJECT)
dwg_get_OBJECT (obj_rapidrtrendersettings, RAPIDRTRENDERSETTINGS)
dwg_get_OBJECT (obj_renderentry, RENDERENTRY)
dwg_get_OBJECT (obj_renderglobal, RENDERGLOBAL)
dwg_get_OBJECT (obj_rendersettings, RENDERSETTINGS)
dwg_get_OBJECT (obj_section_settings, SECTION_SETTINGS)
dwg_get_OBJECT (obj_skylight_background, SKYLIGHT_BACKGROUND)
dwg_get_OBJECT (obj_solid_background, SOLID_BACKGROUND)
dwg_get_OBJECT (obj_spatial_index, SPATIAL_INDEX)
dwg_get_OBJECT (obj_tablestyle, TABLESTYLE)
dwg_get_OBJECT (obj_textobjectcontextdata, TEXTOBJECTCONTEXTDATA)
dwg_get_OBJECT (obj_assocarraymodifyparameters, ASSOCARRAYMODIFYPARAMETERS)
dwg_get_OBJECT (obj_assocarraypathparameters, ASSOCARRAYPATHPARAMETERS)
dwg_get_OBJECT (obj_assocarraypolarparameters, ASSOCARRAYPOLARPARAMETERS)
dwg_get_OBJECT (obj_assocarrayrectangularparameters, ASSOCARRAYRECTANGULARPARAMETERS)
#  ifdef DEBUG_CLASSES
  dwg_get_OBJECT (obj_acmecommandhistory, ACMECOMMANDHISTORY)
  dwg_get_OBJECT (obj_acmescope, ACMESCOPE)
  dwg_get_OBJECT (obj_acmestatemgr, ACMESTATEMGR)
  dwg_get_OBJECT (obj_acsh_extrusion_class, ACSH_EXTRUSION_CLASS)
  dwg_get_OBJECT (obj_acsh_loft_class, ACSH_LOFT_CLASS)
  dwg_get_OBJECT (obj_acsh_revolve_class, ACSH_REVOLVE_CLASS)
  dwg_get_OBJECT (obj_acsh_sweep_class, ACSH_SWEEP_CLASS)
  dwg_get_OBJECT (obj_angdimobjectcontextdata, ANGDIMOBJECTCONTEXTDATA)
  dwg_get_OBJECT (obj_annotscaleobjectcontextdata, ANNOTSCALEOBJECTCONTEXTDATA)
  dwg_get_OBJECT (obj_assoc3pointangulardimactionbody, ASSOC3POINTANGULARDIMACTIONBODY)
  dwg_get_OBJECT (obj_assocaligneddimactionbody, ASSOCALIGNEDDIMACTIONBODY)
  dwg_get_OBJECT (obj_assocarraymodifyactionbody, ASSOCARRAYMODIFYACTIONBODY)
  dwg_get_OBJECT (obj_assocedgeactionparam, ASSOCEDGEACTIONPARAM)
  dwg_get_OBJECT (obj_assocedgechamferactionbody, ASSOCEDGECHAMFERACTIONBODY)
  dwg_get_OBJECT (obj_assocedgefilletactionbody, ASSOCEDGEFILLETACTIONBODY)
  dwg_get_OBJECT (obj_assocmleaderactionbody, ASSOCMLEADERACTIONBODY)
  dwg_get_OBJECT (obj_assocordinatedimactionbody, ASSOCORDINATEDIMACTIONBODY)
  dwg_get_OBJECT (obj_assocperssubentmanager, ASSOCPERSSUBENTMANAGER)
  dwg_get_OBJECT (obj_assocrestoreentitystateactionbody, ASSOCRESTOREENTITYSTATEACTIONBODY)
  dwg_get_OBJECT (obj_assocrotateddimactionbody, ASSOCROTATEDDIMACTIONBODY)
  dwg_get_OBJECT (obj_assocsweptsurfaceactionbody, ASSOCSWEPTSURFACEACTIONBODY)
  dwg_get_OBJECT (obj_blockpropertiestable, BLOCKPROPERTIESTABLE)
  dwg_get_OBJECT (obj_blockpropertiestablegrip, BLOCKPROPERTIESTABLEGRIP)
  dwg_get_OBJECT (obj_breakdata, BREAKDATA)
  dwg_get_OBJECT (obj_breakpointref, BREAKPOINTREF)
  dwg_get_OBJECT (obj_contextdatamanager, CONTEXTDATAMANAGER)
  dwg_get_OBJECT (obj_csacdocumentoptions, CSACDOCUMENTOPTIONS)
  dwg_get_OBJECT (obj_curvepath, CURVEPATH)
  dwg_get_OBJECT (obj_datatable, DATATABLE)
  dwg_get_OBJECT (obj_dimassoc, DIMASSOC)
  dwg_get_OBJECT (obj_dmdimobjectcontextdata, DMDIMOBJECTCONTEXTDATA)
  dwg_get_OBJECT (obj_dynamicblockproxynode, DYNAMICBLOCKPROXYNODE)
  dwg_get_OBJECT (obj_geomapimage, GEOMAPIMAGE)
  dwg_get_OBJECT (obj_mleaderobjectcontextdata, MLEADEROBJECTCONTEXTDATA)
  dwg_get_OBJECT (obj_motionpath, MOTIONPATH)
  dwg_get_OBJECT (obj_mtextattributeobjectcontextdata, MTEXTATTRIBUTEOBJECTCONTEXTDATA)
  dwg_get_OBJECT (obj_navisworksmodeldef, NAVISWORKSMODELDEF)
  dwg_get_OBJECT (obj_orddimobjectcontextdata, ORDDIMOBJECTCONTEXTDATA)
  dwg_get_OBJECT (obj_persubentmgr, PERSUBENTMGR)
  dwg_get_OBJECT (obj_pointpath, POINTPATH)
  dwg_get_OBJECT (obj_radimlgobjectcontextdata, RADIMLGOBJECTCONTEXTDATA)
  dwg_get_OBJECT (obj_radimobjectcontextdata, RADIMOBJECTCONTEXTDATA)
  dwg_get_OBJECT (obj_sunstudy, SUNSTUDY)
  dwg_get_OBJECT (obj_tablecontent, TABLECONTENT)
  dwg_get_OBJECT (obj_tvdeviceproperties, TVDEVICEPROPERTIES)
  //dwg_get_OBJECT (obj_abshdrawingsettings, ABSHDRAWINGSETTINGS)
  //dwg_get_OBJECT (obj_acaecustobj, ACAECUSTOBJ)
  //dwg_get_OBJECT (obj_acaeeemgrobj, ACAEEEMGROBJ)
  //dwg_get_OBJECT (obj_acamcomp, ACAMCOMP)
  //dwg_get_OBJECT (obj_acamcompdef, ACAMCOMPDEF)
  //dwg_get_OBJECT (obj_acamcompdefmgr, ACAMCOMPDEFMGR)
  //dwg_get_OBJECT (obj_acamcontextmodeler, ACAMCONTEXTMODELER)
  //dwg_get_OBJECT (obj_acamgdimstd, ACAMGDIMSTD)
  //dwg_get_OBJECT (obj_acamgfilterdat, ACAMGFILTERDAT)
  //dwg_get_OBJECT (obj_acamgholechartstdcsn, ACAMGHOLECHARTSTDCSN)
  //dwg_get_OBJECT (obj_acamgholechartstddin, ACAMGHOLECHARTSTDDIN)
  //dwg_get_OBJECT (obj_acamgholechartstdiso, ACAMGHOLECHARTSTDISO)
  //dwg_get_OBJECT (obj_acamglaystd, ACAMGLAYSTD)
  //dwg_get_OBJECT (obj_acamgrcompdef, ACAMGRCOMPDEF)
  //dwg_get_OBJECT (obj_acamgrcompdefset, ACAMGRCOMPDEFSET)
  //dwg_get_OBJECT (obj_acamgtitlestd, ACAMGTITLESTD)
  //dwg_get_OBJECT (obj_acammvdbackupobject, ACAMMVDBACKUPOBJECT)
  //dwg_get_OBJECT (obj_acamproject, ACAMPROJECT)
  //dwg_get_OBJECT (obj_acamshaftcompdef, ACAMSHAFTCOMPDEF)
  //dwg_get_OBJECT (obj_acamstdpcompdef, ACAMSTDPCOMPDEF)
  //dwg_get_OBJECT (obj_acamwblocktempents, ACAMWBLOCKTEMPENTS)
  //dwg_get_OBJECT (obj_acarrayjigentity, ACARRAYJIGENTITY)
  //dwg_get_OBJECT (obj_accmcontext, ACCMCONTEXT)
  //dwg_get_OBJECT (obj_acdbcircarcres, ACDBCIRCARCRES)
  //dwg_get_OBJECT (obj_acdbdimensionres, ACDBDIMENSIONRES)
  //dwg_get_OBJECT (obj_acdbentitycache, ACDBENTITYCACHE)
  //dwg_get_OBJECT (obj_acdblineres, ACDBLINERES)
  //dwg_get_OBJECT (obj_acdbstdpartres_arc, ACDBSTDPARTRES_ARC)
  //dwg_get_OBJECT (obj_acdbstdpartres_line, ACDBSTDPARTRES_LINE)
  //dwg_get_OBJECT (obj_acdb_hatchscalecontextdata_class, ACDB_HATCHSCALECONTEXTDATA_CLASS)
  //dwg_get_OBJECT (obj_acdb_hatchviewcontextdata_class, ACDB_HATCHVIEWCONTEXTDATA_CLASS)
  //dwg_get_OBJECT (obj_acdb_proxy_entity_data, ACDB_PROXY_ENTITY_DATA)
  //dwg_get_OBJECT (obj_acdsrecord, ACDSRECORD)
  //dwg_get_OBJECT (obj_acdsschema, ACDSSCHEMA)
  //dwg_get_OBJECT (obj_acgrefacadmaster, ACGREFACADMASTER)
  //dwg_get_OBJECT (obj_acgrefmaster, ACGREFMASTER)
  //dwg_get_OBJECT (obj_acimintsysvar, ACIMINTSYSVAR)
  //dwg_get_OBJECT (obj_acimrealsysvar, ACIMREALSYSVAR)
  //dwg_get_OBJECT (obj_acimstrsysvar, ACIMSTRSYSVAR)
  //dwg_get_OBJECT (obj_acimsysvarman, ACIMSYSVARMAN)
  //dwg_get_OBJECT (obj_acmanootationviewstandardansi, ACMANOOTATIONVIEWSTANDARDANSI)
  //dwg_get_OBJECT (obj_acmanootationviewstandardcsn, ACMANOOTATIONVIEWSTANDARDCSN)
  //dwg_get_OBJECT (obj_acmanootationviewstandarddin, ACMANOOTATIONVIEWSTANDARDDIN)
  //dwg_get_OBJECT (obj_acmanootationviewstandardiso, ACMANOOTATIONVIEWSTANDARDISO)
  //dwg_get_OBJECT (obj_acmaplegenddbobject, ACMAPLEGENDDBOBJECT)
  //dwg_get_OBJECT (obj_acmaplegenditemdbobject, ACMAPLEGENDITEMDBOBJECT)
  //dwg_get_OBJECT (obj_acmapmapviewportdbobject, ACMAPMAPVIEWPORTDBOBJECT)
  //dwg_get_OBJECT (obj_acmapprintlayoutelementdbobjectcontainer, ACMAPPRINTLAYOUTELEMENTDBOBJECTCONTAINER)
  //dwg_get_OBJECT (obj_acmballoon, ACMBALLOON)
  //dwg_get_OBJECT (obj_acmbom, ACMBOM)
  //dwg_get_OBJECT (obj_acmbomrow, ACMBOMROW)
  //dwg_get_OBJECT (obj_acmbomrowstruct, ACMBOMROWSTRUCT)
  //dwg_get_OBJECT (obj_acmbomstandardansi, ACMBOMSTANDARDANSI)
  //dwg_get_OBJECT (obj_acmbomstandardcsn, ACMBOMSTANDARDCSN)
  //dwg_get_OBJECT (obj_acmbomstandarddin, ACMBOMSTANDARDDIN)
  //dwg_get_OBJECT (obj_acmbomstandardiso, ACMBOMSTANDARDISO)
  //dwg_get_OBJECT (obj_acmcenterlinestandardansi, ACMCENTERLINESTANDARDANSI)
  //dwg_get_OBJECT (obj_acmcenterlinestandardcsn, ACMCENTERLINESTANDARDCSN)
  //dwg_get_OBJECT (obj_acmcenterlinestandarddin, ACMCENTERLINESTANDARDDIN)
  //dwg_get_OBJECT (obj_acmcenterlinestandardiso, ACMCENTERLINESTANDARDISO)
  //dwg_get_OBJECT (obj_acmdatadictionary, ACMDATADICTIONARY)
  //dwg_get_OBJECT (obj_acmdataentry, ACMDATAENTRY)
  //dwg_get_OBJECT (obj_acmdataentryblock, ACMDATAENTRYBLOCK)
  //dwg_get_OBJECT (obj_acmdatumid, ACMDATUMID)
  //dwg_get_OBJECT (obj_acmdatumstandardansi, ACMDATUMSTANDARDANSI)
  //dwg_get_OBJECT (obj_acmdatumstandardcsn, ACMDATUMSTANDARDCSN)
  //dwg_get_OBJECT (obj_acmdatumstandarddin, ACMDATUMSTANDARDDIN)
  //dwg_get_OBJECT (obj_acmdatumstandardiso, ACMDATUMSTANDARDISO)
  //dwg_get_OBJECT (obj_acmdatumstandardiso2012, ACMDATUMSTANDARDISO2012)
  //dwg_get_OBJECT (obj_acmdetailstandardansi, ACMDETAILSTANDARDANSI)
  //dwg_get_OBJECT (obj_acmdetailstandardcsn, ACMDETAILSTANDARDCSN)
  //dwg_get_OBJECT (obj_acmdetailstandarddin, ACMDETAILSTANDARDDIN)
  //dwg_get_OBJECT (obj_acmdetailstandardiso, ACMDETAILSTANDARDISO)
  //dwg_get_OBJECT (obj_acmdetailtandardcustom, ACMDETAILTANDARDCUSTOM)
  //dwg_get_OBJECT (obj_acmdimbreakpersreactor, ACMDIMBREAKPERSREACTOR)
  //dwg_get_OBJECT (obj_acmedrawingman, ACMEDRAWINGMAN)
  //dwg_get_OBJECT (obj_acmeview, ACMEVIEW)
  //dwg_get_OBJECT (obj_acme_database, ACME_DATABASE)
  //dwg_get_OBJECT (obj_acme_document, ACME_DOCUMENT)
  //dwg_get_OBJECT (obj_acmfcframe, ACMFCFRAME)
  //dwg_get_OBJECT (obj_acmfcfstandardansi, ACMFCFSTANDARDANSI)
  //dwg_get_OBJECT (obj_acmfcfstandardcsn, ACMFCFSTANDARDCSN)
  //dwg_get_OBJECT (obj_acmfcfstandarddin, ACMFCFSTANDARDDIN)
  //dwg_get_OBJECT (obj_acmfcfstandardiso, ACMFCFSTANDARDISO)
  //dwg_get_OBJECT (obj_acmfcfstandardiso2004, ACMFCFSTANDARDISO2004)
  //dwg_get_OBJECT (obj_acmfcfstandardiso2012, ACMFCFSTANDARDISO2012)
  //dwg_get_OBJECT (obj_acmidstandardansi, ACMIDSTANDARDANSI)
  //dwg_get_OBJECT (obj_acmidstandardcsn, ACMIDSTANDARDCSN)
  //dwg_get_OBJECT (obj_acmidstandarddin, ACMIDSTANDARDDIN)
  //dwg_get_OBJECT (obj_acmidstandardiso, ACMIDSTANDARDISO)
  //dwg_get_OBJECT (obj_acmidstandardiso2004, ACMIDSTANDARDISO2004)
  //dwg_get_OBJECT (obj_acmidstandardiso2012, ACMIDSTANDARDISO2012)
  //dwg_get_OBJECT (obj_acmnotestandardansi, ACMNOTESTANDARDANSI)
  //dwg_get_OBJECT (obj_acmnotestandardcsn, ACMNOTESTANDARDCSN)
  //dwg_get_OBJECT (obj_acmnotestandarddin, ACMNOTESTANDARDDIN)
  //dwg_get_OBJECT (obj_acmnotestandardiso, ACMNOTESTANDARDISO)
  //dwg_get_OBJECT (obj_acmpartlist, ACMPARTLIST)
  //dwg_get_OBJECT (obj_acmpickobj, ACMPICKOBJ)
  //dwg_get_OBJECT (obj_acmsectionstandardansi, ACMSECTIONSTANDARDANSI)
  //dwg_get_OBJECT (obj_acmsectionstandardcsn2002, ACMSECTIONSTANDARDCSN2002)
  //dwg_get_OBJECT (obj_acmsectionstandardcustom, ACMSECTIONSTANDARDCUSTOM)
  //dwg_get_OBJECT (obj_acmsectionstandarddin, ACMSECTIONSTANDARDDIN)
  //dwg_get_OBJECT (obj_acmsectionstandardiso, ACMSECTIONSTANDARDISO)
  //dwg_get_OBJECT (obj_acmsectionstandardiso2001, ACMSECTIONSTANDARDISO2001)
  //dwg_get_OBJECT (obj_acmstandardansi, ACMSTANDARDANSI)
  //dwg_get_OBJECT (obj_acmstandardcsn, ACMSTANDARDCSN)
  //dwg_get_OBJECT (obj_acmstandarddin, ACMSTANDARDDIN)
  //dwg_get_OBJECT (obj_acmstandardiso, ACMSTANDARDISO)
  //dwg_get_OBJECT (obj_acmsurfstandardansi, ACMSURFSTANDARDANSI)
  //dwg_get_OBJECT (obj_acmsurfstandardcsn, ACMSURFSTANDARDCSN)
  //dwg_get_OBJECT (obj_acmsurfstandarddin, ACMSURFSTANDARDDIN)
  //dwg_get_OBJECT (obj_acmsurfstandardiso, ACMSURFSTANDARDISO)
  //dwg_get_OBJECT (obj_acmsurfstandardiso2002, ACMSURFSTANDARDISO2002)
  //dwg_get_OBJECT (obj_acmsurfsym, ACMSURFSYM)
  //dwg_get_OBJECT (obj_acmtaperstandardansi, ACMTAPERSTANDARDANSI)
  //dwg_get_OBJECT (obj_acmtaperstandardcsn, ACMTAPERSTANDARDCSN)
  //dwg_get_OBJECT (obj_acmtaperstandarddin, ACMTAPERSTANDARDDIN)
  //dwg_get_OBJECT (obj_acmtaperstandardiso, ACMTAPERSTANDARDISO)
  //dwg_get_OBJECT (obj_acmthreadlinestandardansi, ACMTHREADLINESTANDARDANSI)
  //dwg_get_OBJECT (obj_acmthreadlinestandardcsn, ACMTHREADLINESTANDARDCSN)
  //dwg_get_OBJECT (obj_acmthreadlinestandarddin, ACMTHREADLINESTANDARDDIN)
  //dwg_get_OBJECT (obj_acmthreadlinestandardiso, ACMTHREADLINESTANDARDISO)
  //dwg_get_OBJECT (obj_acmweldstandardansi, ACMWELDSTANDARDANSI)
  //dwg_get_OBJECT (obj_acmweldstandardcsn, ACMWELDSTANDARDCSN)
  //dwg_get_OBJECT (obj_acmweldstandarddin, ACMWELDSTANDARDDIN)
  //dwg_get_OBJECT (obj_acmweldstandardiso, ACMWELDSTANDARDISO)
  //dwg_get_OBJECT (obj_acmweldsym, ACMWELDSYM)
  //dwg_get_OBJECT (obj_acrfattgenmgr, ACRFATTGENMGR)
  //dwg_get_OBJECT (obj_acrfinsadj, ACRFINSADJ)
  //dwg_get_OBJECT (obj_acrfinsadjustermgr, ACRFINSADJUSTERMGR)
  //dwg_get_OBJECT (obj_acrfmcadapiattholder, ACRFMCADAPIATTHOLDER)
  //dwg_get_OBJECT (obj_acrfobjattmgr, ACRFOBJATTMGR)
  //dwg_get_OBJECT (obj_acsh_subent_material_class, ACSH_SUBENT_MATERIAL_CLASS)
  //dwg_get_OBJECT (obj_ac_am_2d_xref_mgr, AC_AM_2D_XREF_MGR)
  //dwg_get_OBJECT (obj_ac_am_basic_view, AC_AM_BASIC_VIEW)
  //dwg_get_OBJECT (obj_ac_am_basic_view_def, AC_AM_BASIC_VIEW_DEF)
  //dwg_get_OBJECT (obj_ac_am_complex_hide_situation, AC_AM_COMPLEX_HIDE_SITUATION)
  //dwg_get_OBJECT (obj_ac_am_comp_view_def, AC_AM_COMP_VIEW_DEF)
  //dwg_get_OBJECT (obj_ac_am_comp_view_inst, AC_AM_COMP_VIEW_INST)
  //dwg_get_OBJECT (obj_ac_am_dirty_nodes, AC_AM_DIRTY_NODES)
  //dwg_get_OBJECT (obj_ac_am_hide_situation, AC_AM_HIDE_SITUATION)
  //dwg_get_OBJECT (obj_ac_am_mapper_cache, AC_AM_MAPPER_CACHE)
  //dwg_get_OBJECT (obj_ac_am_master_view_def, AC_AM_MASTER_VIEW_DEF)
  //dwg_get_OBJECT (obj_ac_am_mvd_dep_mgr, AC_AM_MVD_DEP_MGR)
  //dwg_get_OBJECT (obj_ac_am_override_filter, AC_AM_OVERRIDE_FILTER)
  //dwg_get_OBJECT (obj_ac_am_props_override, AC_AM_PROPS_OVERRIDE)
  //dwg_get_OBJECT (obj_ac_am_shaft_hide_situation, AC_AM_SHAFT_HIDE_SITUATION)
  //dwg_get_OBJECT (obj_ac_am_stdp_view_def, AC_AM_STDP_VIEW_DEF)
  //dwg_get_OBJECT (obj_ac_am_transform_ghost, AC_AM_TRANSFORM_GHOST)
  //dwg_get_OBJECT (obj_adappl, ADAPPL)
  //dwg_get_OBJECT (obj_aecc_alignment_design_check_set, AECC_ALIGNMENT_DESIGN_CHECK_SET)
  //dwg_get_OBJECT (obj_aecc_alignment_label_set, AECC_ALIGNMENT_LABEL_SET)
  //dwg_get_OBJECT (obj_aecc_alignment_label_set_ext, AECC_ALIGNMENT_LABEL_SET_EXT)
  //dwg_get_OBJECT (obj_aecc_alignment_parcel_node, AECC_ALIGNMENT_PARCEL_NODE)
  //dwg_get_OBJECT (obj_aecc_alignment_style, AECC_ALIGNMENT_STYLE)
  //dwg_get_OBJECT (obj_aecc_appurtenance_style, AECC_APPURTENANCE_STYLE)
  //dwg_get_OBJECT (obj_aecc_assembly_style, AECC_ASSEMBLY_STYLE)
  //dwg_get_OBJECT (obj_aecc_building_site_style, AECC_BUILDING_SITE_STYLE)
  //dwg_get_OBJECT (obj_aecc_cant_diagram_view_style, AECC_CANT_DIAGRAM_VIEW_STYLE)
  //dwg_get_OBJECT (obj_aecc_catchment_style, AECC_CATCHMENT_STYLE)
  //dwg_get_OBJECT (obj_aecc_class_node, AECC_CLASS_NODE)
  //dwg_get_OBJECT (obj_aecc_contourview, AECC_CONTOURVIEW)
  //dwg_get_OBJECT (obj_aecc_corridor_style, AECC_CORRIDOR_STYLE)
  //dwg_get_OBJECT (obj_aecc_disp_rep_alignment, AECC_DISP_REP_ALIGNMENT)
  //dwg_get_OBJECT (obj_aecc_disp_rep_alignment_cant_label_group, AECC_DISP_REP_ALIGNMENT_CANT_LABEL_GROUP)
  //dwg_get_OBJECT (obj_aecc_disp_rep_alignment_csv, AECC_DISP_REP_ALIGNMENT_CSV)
  //dwg_get_OBJECT (obj_aecc_disp_rep_alignment_curve_label, AECC_DISP_REP_ALIGNMENT_CURVE_LABEL)
  //dwg_get_OBJECT (obj_aecc_disp_rep_alignment_designspeed_label_group, AECC_DISP_REP_ALIGNMENT_DESIGNSPEED_LABEL_GROUP)
  //dwg_get_OBJECT (obj_aecc_disp_rep_alignment_geompt_label_group, AECC_DISP_REP_ALIGNMENT_GEOMPT_LABEL_GROUP)
  //dwg_get_OBJECT (obj_aecc_disp_rep_alignment_indexed_pi_label, AECC_DISP_REP_ALIGNMENT_INDEXED_PI_LABEL)
  //dwg_get_OBJECT (obj_aecc_disp_rep_alignment_minor_station_label_group, AECC_DISP_REP_ALIGNMENT_MINOR_STATION_LABEL_GROUP)
  //dwg_get_OBJECT (obj_aecc_disp_rep_alignment_pi_label, AECC_DISP_REP_ALIGNMENT_PI_LABEL)
  //dwg_get_OBJECT (obj_aecc_disp_rep_alignment_spiral_label, AECC_DISP_REP_ALIGNMENT_SPIRAL_LABEL)
  //dwg_get_OBJECT (obj_aecc_disp_rep_alignment_staequ_label_group, AECC_DISP_REP_ALIGNMENT_STAEQU_LABEL_GROUP)
  //dwg_get_OBJECT (obj_aecc_disp_rep_alignment_station_label_group, AECC_DISP_REP_ALIGNMENT_STATION_LABEL_GROUP)
  //dwg_get_OBJECT (obj_aecc_disp_rep_alignment_station_offset_label, AECC_DISP_REP_ALIGNMENT_STATION_OFFSET_LABEL)
  //dwg_get_OBJECT (obj_aecc_disp_rep_alignment_superelevation_label_group, AECC_DISP_REP_ALIGNMENT_SUPERELEVATION_LABEL_GROUP)
  //dwg_get_OBJECT (obj_aecc_disp_rep_alignment_table, AECC_DISP_REP_ALIGNMENT_TABLE)
  //dwg_get_OBJECT (obj_aecc_disp_rep_alignment_tangent_label, AECC_DISP_REP_ALIGNMENT_TANGENT_LABEL)
  //dwg_get_OBJECT (obj_aecc_disp_rep_alignment_vertical_geompt_labeling, AECC_DISP_REP_ALIGNMENT_VERTICAL_GEOMPT_LABELING)
  //dwg_get_OBJECT (obj_aecc_disp_rep_appurtenance, AECC_DISP_REP_APPURTENANCE)
  //dwg_get_OBJECT (obj_aecc_disp_rep_appurtenance_csv, AECC_DISP_REP_APPURTENANCE_CSV)
  //dwg_get_OBJECT (obj_aecc_disp_rep_appurtenance_labeling, AECC_DISP_REP_APPURTENANCE_LABELING)
  //dwg_get_OBJECT (obj_aecc_disp_rep_appurtenance_profile_labeling, AECC_DISP_REP_APPURTENANCE_PROFILE_LABELING)
  //dwg_get_OBJECT (obj_aecc_disp_rep_assembly, AECC_DISP_REP_ASSEMBLY)
  //dwg_get_OBJECT (obj_aecc_disp_rep_auto_corridor_feature_line, AECC_DISP_REP_AUTO_CORRIDOR_FEATURE_LINE)
  //dwg_get_OBJECT (obj_aecc_disp_rep_auto_corridor_feature_line_profile, AECC_DISP_REP_AUTO_CORRIDOR_FEATURE_LINE_PROFILE)
  //dwg_get_OBJECT (obj_aecc_disp_rep_auto_corridor_feature_line_section, AECC_DISP_REP_AUTO_CORRIDOR_FEATURE_LINE_SECTION)
  //dwg_get_OBJECT (obj_aecc_disp_rep_auto_feature_line, AECC_DISP_REP_AUTO_FEATURE_LINE)
  //dwg_get_OBJECT (obj_aecc_disp_rep_auto_feature_line_csv, AECC_DISP_REP_AUTO_FEATURE_LINE_CSV)
  //dwg_get_OBJECT (obj_aecc_disp_rep_auto_feature_line_profile, AECC_DISP_REP_AUTO_FEATURE_LINE_PROFILE)
  //dwg_get_OBJECT (obj_aecc_disp_rep_auto_feature_line_section, AECC_DISP_REP_AUTO_FEATURE_LINE_SECTION)
  //dwg_get_OBJECT (obj_aecc_disp_rep_buildingsite, AECC_DISP_REP_BUILDINGSITE)
  //dwg_get_OBJECT (obj_aecc_disp_rep_buildingutil_connector, AECC_DISP_REP_BUILDINGUTIL_CONNECTOR)
  //dwg_get_OBJECT (obj_aecc_disp_rep_cant_diagram_view, AECC_DISP_REP_CANT_DIAGRAM_VIEW)
  //dwg_get_OBJECT (obj_aecc_disp_rep_catchment_area, AECC_DISP_REP_CATCHMENT_AREA)
  //dwg_get_OBJECT (obj_aecc_disp_rep_catchment_area_label, AECC_DISP_REP_CATCHMENT_AREA_LABEL)
  //dwg_get_OBJECT (obj_aecc_disp_rep_corridor, AECC_DISP_REP_CORRIDOR)
  //dwg_get_OBJECT (obj_aecc_disp_rep_crossing_pipe_profile_labeling, AECC_DISP_REP_CROSSING_PIPE_PROFILE_LABELING)
  //dwg_get_OBJECT (obj_aecc_disp_rep_crossing_pressure_pipe_profile_labeling, AECC_DISP_REP_CROSSING_PRESSURE_PIPE_PROFILE_LABELING)
  //dwg_get_OBJECT (obj_aecc_disp_rep_csvstationslider, AECC_DISP_REP_CSVSTATIONSLIDER)
  //dwg_get_OBJECT (obj_aecc_disp_rep_face, AECC_DISP_REP_FACE)
  //dwg_get_OBJECT (obj_aecc_disp_rep_feature, AECC_DISP_REP_FEATURE)
  //dwg_get_OBJECT (obj_aecc_disp_rep_feature_label, AECC_DISP_REP_FEATURE_LABEL)
  //dwg_get_OBJECT (obj_aecc_disp_rep_feature_line, AECC_DISP_REP_FEATURE_LINE)
  //dwg_get_OBJECT (obj_aecc_disp_rep_feature_line_csv, AECC_DISP_REP_FEATURE_LINE_CSV)
  //dwg_get_OBJECT (obj_aecc_disp_rep_feature_line_profile, AECC_DISP_REP_FEATURE_LINE_PROFILE)
  //dwg_get_OBJECT (obj_aecc_disp_rep_feature_line_section, AECC_DISP_REP_FEATURE_LINE_SECTION)
  //dwg_get_OBJECT (obj_aecc_disp_rep_fitting, AECC_DISP_REP_FITTING)
  //dwg_get_OBJECT (obj_aecc_disp_rep_fitting_csv, AECC_DISP_REP_FITTING_CSV)
  //dwg_get_OBJECT (obj_aecc_disp_rep_fitting_labeling, AECC_DISP_REP_FITTING_LABELING)
  //dwg_get_OBJECT (obj_aecc_disp_rep_fitting_profile_labeling, AECC_DISP_REP_FITTING_PROFILE_LABELING)
  //dwg_get_OBJECT (obj_aecc_disp_rep_flow_segment_label, AECC_DISP_REP_FLOW_SEGMENT_LABEL)
  //dwg_get_OBJECT (obj_aecc_disp_rep_general_segment_label, AECC_DISP_REP_GENERAL_SEGMENT_LABEL)
  //dwg_get_OBJECT (obj_aecc_disp_rep_grading, AECC_DISP_REP_GRADING)
  //dwg_get_OBJECT (obj_aecc_disp_rep_graph, AECC_DISP_REP_GRAPH)
  //dwg_get_OBJECT (obj_aecc_disp_rep_graphprofile_networkpart, AECC_DISP_REP_GRAPHPROFILE_NETWORKPART)
  //dwg_get_OBJECT (obj_aecc_disp_rep_graphprofile_pressurepart, AECC_DISP_REP_GRAPHPROFILE_PRESSUREPART)
  //dwg_get_OBJECT (obj_aecc_disp_rep_grid_surface, AECC_DISP_REP_GRID_SURFACE)
  //dwg_get_OBJECT (obj_aecc_disp_rep_grid_surface_csv, AECC_DISP_REP_GRID_SURFACE_CSV)
  //dwg_get_OBJECT (obj_aecc_disp_rep_horgeometry_band_label_group, AECC_DISP_REP_HORGEOMETRY_BAND_LABEL_GROUP)
  //dwg_get_OBJECT (obj_aecc_disp_rep_hydro_region, AECC_DISP_REP_HYDRO_REGION)
  //dwg_get_OBJECT (obj_aecc_disp_rep_interference_check, AECC_DISP_REP_INTERFERENCE_CHECK)
  //dwg_get_OBJECT (obj_aecc_disp_rep_interference_part, AECC_DISP_REP_INTERFERENCE_PART)
  //dwg_get_OBJECT (obj_aecc_disp_rep_interference_part_section, AECC_DISP_REP_INTERFERENCE_PART_SECTION)
  //dwg_get_OBJECT (obj_aecc_disp_rep_intersection, AECC_DISP_REP_INTERSECTION)
  //dwg_get_OBJECT (obj_aecc_disp_rep_intersection_location_labeling, AECC_DISP_REP_INTERSECTION_LOCATION_LABELING)
  //dwg_get_OBJECT (obj_aecc_disp_rep_labeling, AECC_DISP_REP_LABELING)
  //dwg_get_OBJECT (obj_aecc_disp_rep_legend_table, AECC_DISP_REP_LEGEND_TABLE)
  //dwg_get_OBJECT (obj_aecc_disp_rep_line_between_points_label, AECC_DISP_REP_LINE_BETWEEN_POINTS_LABEL)
  //dwg_get_OBJECT (obj_aecc_disp_rep_lotline_csv, AECC_DISP_REP_LOTLINE_CSV)
  //dwg_get_OBJECT (obj_aecc_disp_rep_masshaulline, AECC_DISP_REP_MASSHAULLINE)
  //dwg_get_OBJECT (obj_aecc_disp_rep_mass_haul_view, AECC_DISP_REP_MASS_HAUL_VIEW)
  //dwg_get_OBJECT (obj_aecc_disp_rep_matchline_labeling, AECC_DISP_REP_MATCHLINE_LABELING)
  //dwg_get_OBJECT (obj_aecc_disp_rep_match_line, AECC_DISP_REP_MATCH_LINE)
  //dwg_get_OBJECT (obj_aecc_disp_rep_material_section, AECC_DISP_REP_MATERIAL_SECTION)
  //dwg_get_OBJECT (obj_aecc_disp_rep_network, AECC_DISP_REP_NETWORK)
  //dwg_get_OBJECT (obj_aecc_disp_rep_note_label, AECC_DISP_REP_NOTE_LABEL)
  //dwg_get_OBJECT (obj_aecc_disp_rep_offset_elev_label, AECC_DISP_REP_OFFSET_ELEV_LABEL)
  //dwg_get_OBJECT (obj_aecc_disp_rep_parcel_boundary, AECC_DISP_REP_PARCEL_BOUNDARY)
  //dwg_get_OBJECT (obj_aecc_disp_rep_parcel_face_label, AECC_DISP_REP_PARCEL_FACE_LABEL)
  //dwg_get_OBJECT (obj_aecc_disp_rep_parcel_segment, AECC_DISP_REP_PARCEL_SEGMENT)
  //dwg_get_OBJECT (obj_aecc_disp_rep_parcel_segment_label, AECC_DISP_REP_PARCEL_SEGMENT_LABEL)
  //dwg_get_OBJECT (obj_aecc_disp_rep_parcel_segment_table, AECC_DISP_REP_PARCEL_SEGMENT_TABLE)
  //dwg_get_OBJECT (obj_aecc_disp_rep_parcel_table, AECC_DISP_REP_PARCEL_TABLE)
  //dwg_get_OBJECT (obj_aecc_disp_rep_pipe, AECC_DISP_REP_PIPE)
  //dwg_get_OBJECT (obj_aecc_disp_rep_pipenetwork_band_label_group, AECC_DISP_REP_PIPENETWORK_BAND_LABEL_GROUP)
  //dwg_get_OBJECT (obj_aecc_disp_rep_pipe_csv, AECC_DISP_REP_PIPE_CSV)
  //dwg_get_OBJECT (obj_aecc_disp_rep_pipe_labeling, AECC_DISP_REP_PIPE_LABELING)
  //dwg_get_OBJECT (obj_aecc_disp_rep_pipe_profile_labeling, AECC_DISP_REP_PIPE_PROFILE_LABELING)
  //dwg_get_OBJECT (obj_aecc_disp_rep_pipe_section_labeling, AECC_DISP_REP_PIPE_SECTION_LABELING)
  //dwg_get_OBJECT (obj_aecc_disp_rep_pipe_table, AECC_DISP_REP_PIPE_TABLE)
  //dwg_get_OBJECT (obj_aecc_disp_rep_point_ent, AECC_DISP_REP_POINT_ENT)
  //dwg_get_OBJECT (obj_aecc_disp_rep_point_group, AECC_DISP_REP_POINT_GROUP)
  //dwg_get_OBJECT (obj_aecc_disp_rep_point_table, AECC_DISP_REP_POINT_TABLE)
  //dwg_get_OBJECT (obj_aecc_disp_rep_pressurepipenetwork, AECC_DISP_REP_PRESSUREPIPENETWORK)
  //dwg_get_OBJECT (obj_aecc_disp_rep_pressure_part_table, AECC_DISP_REP_PRESSURE_PART_TABLE)
  //dwg_get_OBJECT (obj_aecc_disp_rep_pressure_pipe, AECC_DISP_REP_PRESSURE_PIPE)
  //dwg_get_OBJECT (obj_aecc_disp_rep_pressure_pipe_csv, AECC_DISP_REP_PRESSURE_PIPE_CSV)
  //dwg_get_OBJECT (obj_aecc_disp_rep_pressure_pipe_labeling, AECC_DISP_REP_PRESSURE_PIPE_LABELING)
  //dwg_get_OBJECT (obj_aecc_disp_rep_pressure_pipe_profile_labeling, AECC_DISP_REP_PRESSURE_PIPE_PROFILE_LABELING)
  //dwg_get_OBJECT (obj_aecc_disp_rep_pressure_pipe_section_labeling, AECC_DISP_REP_PRESSURE_PIPE_SECTION_LABELING)
  //dwg_get_OBJECT (obj_aecc_disp_rep_profile, AECC_DISP_REP_PROFILE)
  //dwg_get_OBJECT (obj_aecc_disp_rep_profiledata_band_label_group, AECC_DISP_REP_PROFILEDATA_BAND_LABEL_GROUP)
  //dwg_get_OBJECT (obj_aecc_disp_rep_profile_projection, AECC_DISP_REP_PROFILE_PROJECTION)
  //dwg_get_OBJECT (obj_aecc_disp_rep_profile_projection_label, AECC_DISP_REP_PROFILE_PROJECTION_LABEL)
  //dwg_get_OBJECT (obj_aecc_disp_rep_profile_view, AECC_DISP_REP_PROFILE_VIEW)
  //dwg_get_OBJECT (obj_aecc_disp_rep_profile_view_depth_label, AECC_DISP_REP_PROFILE_VIEW_DEPTH_LABEL)
  //dwg_get_OBJECT (obj_aecc_disp_rep_quantity_takeoff_aggregate_earthwork_table, AECC_DISP_REP_QUANTITY_TAKEOFF_AGGREGATE_EARTHWORK_TABLE)
  //dwg_get_OBJECT (obj_aecc_disp_rep_right_of_way, AECC_DISP_REP_RIGHT_OF_WAY)
  //dwg_get_OBJECT (obj_aecc_disp_rep_sampleline_labeling, AECC_DISP_REP_SAMPLELINE_LABELING)
  //dwg_get_OBJECT (obj_aecc_disp_rep_sample_line, AECC_DISP_REP_SAMPLE_LINE)
  //dwg_get_OBJECT (obj_aecc_disp_rep_sample_line_group, AECC_DISP_REP_SAMPLE_LINE_GROUP)
  //dwg_get_OBJECT (obj_aecc_disp_rep_section, AECC_DISP_REP_SECTION)
  //dwg_get_OBJECT (obj_aecc_disp_rep_sectionaldata_band_label_group, AECC_DISP_REP_SECTIONALDATA_BAND_LABEL_GROUP)
  //dwg_get_OBJECT (obj_aecc_disp_rep_sectiondata_band_label_group, AECC_DISP_REP_SECTIONDATA_BAND_LABEL_GROUP)
  //dwg_get_OBJECT (obj_aecc_disp_rep_sectionsegment_band_label_group, AECC_DISP_REP_SECTIONSEGMENT_BAND_LABEL_GROUP)
  //dwg_get_OBJECT (obj_aecc_disp_rep_section_corridor, AECC_DISP_REP_SECTION_CORRIDOR)
  //dwg_get_OBJECT (obj_aecc_disp_rep_section_corridor_point_label_group, AECC_DISP_REP_SECTION_CORRIDOR_POINT_LABEL_GROUP)
  //dwg_get_OBJECT (obj_aecc_disp_rep_section_gradebreak_label_group, AECC_DISP_REP_SECTION_GRADEBREAK_LABEL_GROUP)
  //dwg_get_OBJECT (obj_aecc_disp_rep_section_minor_offset_label_group, AECC_DISP_REP_SECTION_MINOR_OFFSET_LABEL_GROUP)
  //dwg_get_OBJECT (obj_aecc_disp_rep_section_offset_label_group, AECC_DISP_REP_SECTION_OFFSET_LABEL_GROUP)
  //dwg_get_OBJECT (obj_aecc_disp_rep_section_pipenetwork, AECC_DISP_REP_SECTION_PIPENETWORK)
  //dwg_get_OBJECT (obj_aecc_disp_rep_section_pressurepipenetwork, AECC_DISP_REP_SECTION_PRESSUREPIPENETWORK)
  //dwg_get_OBJECT (obj_aecc_disp_rep_section_projection, AECC_DISP_REP_SECTION_PROJECTION)
  //dwg_get_OBJECT (obj_aecc_disp_rep_section_projection_label, AECC_DISP_REP_SECTION_PROJECTION_LABEL)
  //dwg_get_OBJECT (obj_aecc_disp_rep_section_segment_label_group, AECC_DISP_REP_SECTION_SEGMENT_LABEL_GROUP)
  //dwg_get_OBJECT (obj_aecc_disp_rep_section_view, AECC_DISP_REP_SECTION_VIEW)
  //dwg_get_OBJECT (obj_aecc_disp_rep_section_view_depth_label, AECC_DISP_REP_SECTION_VIEW_DEPTH_LABEL)
  //dwg_get_OBJECT (obj_aecc_disp_rep_section_view_quantity_takeoff_table, AECC_DISP_REP_SECTION_VIEW_QUANTITY_TAKEOFF_TABLE)
  //dwg_get_OBJECT (obj_aecc_disp_rep_sheet, AECC_DISP_REP_SHEET)
  //dwg_get_OBJECT (obj_aecc_disp_rep_spanning_pipe_labeling, AECC_DISP_REP_SPANNING_PIPE_LABELING)
  //dwg_get_OBJECT (obj_aecc_disp_rep_spanning_pipe_profile_labeling, AECC_DISP_REP_SPANNING_PIPE_PROFILE_LABELING)
  //dwg_get_OBJECT (obj_aecc_disp_rep_station_elev_label, AECC_DISP_REP_STATION_ELEV_LABEL)
  //dwg_get_OBJECT (obj_aecc_disp_rep_structure, AECC_DISP_REP_STRUCTURE)
  //dwg_get_OBJECT (obj_aecc_disp_rep_structure_csv, AECC_DISP_REP_STRUCTURE_CSV)
  //dwg_get_OBJECT (obj_aecc_disp_rep_structure_labeling, AECC_DISP_REP_STRUCTURE_LABELING)
  //dwg_get_OBJECT (obj_aecc_disp_rep_structure_profile_labeling, AECC_DISP_REP_STRUCTURE_PROFILE_LABELING)
  //dwg_get_OBJECT (obj_aecc_disp_rep_structure_section_labeling, AECC_DISP_REP_STRUCTURE_SECTION_LABELING)
  //dwg_get_OBJECT (obj_aecc_disp_rep_subassembly, AECC_DISP_REP_SUBASSEMBLY)
  //dwg_get_OBJECT (obj_aecc_disp_rep_superelevation_band_label_group, AECC_DISP_REP_SUPERELEVATION_BAND_LABEL_GROUP)
  //dwg_get_OBJECT (obj_aecc_disp_rep_superelevation_diagram_view, AECC_DISP_REP_SUPERELEVATION_DIAGRAM_VIEW)
  //dwg_get_OBJECT (obj_aecc_disp_rep_surface_contour_label_group, AECC_DISP_REP_SURFACE_CONTOUR_LABEL_GROUP)
  //dwg_get_OBJECT (obj_aecc_disp_rep_surface_elevation_label, AECC_DISP_REP_SURFACE_ELEVATION_LABEL)
  //dwg_get_OBJECT (obj_aecc_disp_rep_surface_slope_label, AECC_DISP_REP_SURFACE_SLOPE_LABEL)
  //dwg_get_OBJECT (obj_aecc_disp_rep_survey_figure_label_group, AECC_DISP_REP_SURVEY_FIGURE_LABEL_GROUP)
  //dwg_get_OBJECT (obj_aecc_disp_rep_svfigure, AECC_DISP_REP_SVFIGURE)
  //dwg_get_OBJECT (obj_aecc_disp_rep_svfigure_csv, AECC_DISP_REP_SVFIGURE_CSV)
  //dwg_get_OBJECT (obj_aecc_disp_rep_svfigure_profile, AECC_DISP_REP_SVFIGURE_PROFILE)
  //dwg_get_OBJECT (obj_aecc_disp_rep_svfigure_section, AECC_DISP_REP_SVFIGURE_SECTION)
  //dwg_get_OBJECT (obj_aecc_disp_rep_svfigure_segment_label, AECC_DISP_REP_SVFIGURE_SEGMENT_LABEL)
  //dwg_get_OBJECT (obj_aecc_disp_rep_svnetwork, AECC_DISP_REP_SVNETWORK)
  //dwg_get_OBJECT (obj_aecc_disp_rep_tangent_intersection_table, AECC_DISP_REP_TANGENT_INTERSECTION_TABLE)
  //dwg_get_OBJECT (obj_aecc_disp_rep_tin_surface, AECC_DISP_REP_TIN_SURFACE)
  //dwg_get_OBJECT (obj_aecc_disp_rep_tin_surface_csv, AECC_DISP_REP_TIN_SURFACE_CSV)
  //dwg_get_OBJECT (obj_aecc_disp_rep_valignment_crestcurve_label_group, AECC_DISP_REP_VALIGNMENT_CRESTCURVE_LABEL_GROUP)
  //dwg_get_OBJECT (obj_aecc_disp_rep_valignment_csv, AECC_DISP_REP_VALIGNMENT_CSV)
  //dwg_get_OBJECT (obj_aecc_disp_rep_valignment_hageompt_label_group, AECC_DISP_REP_VALIGNMENT_HAGEOMPT_LABEL_GROUP)
  //dwg_get_OBJECT (obj_aecc_disp_rep_valignment_line_label_group, AECC_DISP_REP_VALIGNMENT_LINE_LABEL_GROUP)
  //dwg_get_OBJECT (obj_aecc_disp_rep_valignment_minor_station_label_group, AECC_DISP_REP_VALIGNMENT_MINOR_STATION_LABEL_GROUP)
  //dwg_get_OBJECT (obj_aecc_disp_rep_valignment_pvi_label_group, AECC_DISP_REP_VALIGNMENT_PVI_LABEL_GROUP)
  //dwg_get_OBJECT (obj_aecc_disp_rep_valignment_sagcurve_label_group, AECC_DISP_REP_VALIGNMENT_SAGCURVE_LABEL_GROUP)
  //dwg_get_OBJECT (obj_aecc_disp_rep_valignment_station_label_group, AECC_DISP_REP_VALIGNMENT_STATION_LABEL_GROUP)
  //dwg_get_OBJECT (obj_aecc_disp_rep_verticalgeometry_band_label_group, AECC_DISP_REP_VERTICALGEOMETRY_BAND_LABEL_GROUP)
  //dwg_get_OBJECT (obj_aecc_disp_rep_viewframe_labeling, AECC_DISP_REP_VIEWFRAME_LABELING)
  //dwg_get_OBJECT (obj_aecc_disp_rep_view_frame, AECC_DISP_REP_VIEW_FRAME)
  //dwg_get_OBJECT (obj_aecc_featureline_style, AECC_FEATURELINE_STYLE)
  //dwg_get_OBJECT (obj_aecc_feature_style, AECC_FEATURE_STYLE)
  //dwg_get_OBJECT (obj_aecc_fitting_style, AECC_FITTING_STYLE)
  //dwg_get_OBJECT (obj_aecc_format_manager_object, AECC_FORMAT_MANAGER_OBJECT)
  //dwg_get_OBJECT (obj_aecc_gradeview, AECC_GRADEVIEW)
  //dwg_get_OBJECT (obj_aecc_grading_criteria, AECC_GRADING_CRITERIA)
  //dwg_get_OBJECT (obj_aecc_grading_criteria_set, AECC_GRADING_CRITERIA_SET)
  //dwg_get_OBJECT (obj_aecc_grading_group, AECC_GRADING_GROUP)
  //dwg_get_OBJECT (obj_aecc_grading_style, AECC_GRADING_STYLE)
  //dwg_get_OBJECT (obj_aecc_import_storm_sewer_defaults, AECC_IMPORT_STORM_SEWER_DEFAULTS)
  //dwg_get_OBJECT (obj_aecc_interference_style, AECC_INTERFERENCE_STYLE)
  //dwg_get_OBJECT (obj_aecc_intersection_style, AECC_INTERSECTION_STYLE)
  //dwg_get_OBJECT (obj_aecc_label_collector_style, AECC_LABEL_COLLECTOR_STYLE)
  //dwg_get_OBJECT (obj_aecc_label_node, AECC_LABEL_NODE)
  //dwg_get_OBJECT (obj_aecc_label_radial_line_style, AECC_LABEL_RADIAL_LINE_STYLE)
  //dwg_get_OBJECT (obj_aecc_label_text_iterator_curve_or_spiral_style, AECC_LABEL_TEXT_ITERATOR_CURVE_OR_SPIRAL_STYLE)
  //dwg_get_OBJECT (obj_aecc_label_text_iterator_style, AECC_LABEL_TEXT_ITERATOR_STYLE)
  //dwg_get_OBJECT (obj_aecc_label_text_style, AECC_LABEL_TEXT_STYLE)
  //dwg_get_OBJECT (obj_aecc_label_vector_arrow_style, AECC_LABEL_VECTOR_ARROW_STYLE)
  //dwg_get_OBJECT (obj_aecc_legend_table_style, AECC_LEGEND_TABLE_STYLE)
  //dwg_get_OBJECT (obj_aecc_mass_haul_line_style, AECC_MASS_HAUL_LINE_STYLE)
  //dwg_get_OBJECT (obj_aecc_mass_haul_view_style, AECC_MASS_HAUL_VIEW_STYLE)
  //dwg_get_OBJECT (obj_aecc_matchline_style, AECC_MATCHLINE_STYLE)
  //dwg_get_OBJECT (obj_aecc_material_style, AECC_MATERIAL_STYLE)
  //dwg_get_OBJECT (obj_aecc_network_part_catalog_def_node, AECC_NETWORK_PART_CATALOG_DEF_NODE)
  //dwg_get_OBJECT (obj_aecc_network_part_family_item, AECC_NETWORK_PART_FAMILY_ITEM)
  //dwg_get_OBJECT (obj_aecc_network_part_list, AECC_NETWORK_PART_LIST)
  //dwg_get_OBJECT (obj_aecc_network_rule, AECC_NETWORK_RULE)
  //dwg_get_OBJECT (obj_aecc_parcel_node, AECC_PARCEL_NODE)
  //dwg_get_OBJECT (obj_aecc_parcel_style, AECC_PARCEL_STYLE)
  //dwg_get_OBJECT (obj_aecc_part_size_filter, AECC_PART_SIZE_FILTER)
  //dwg_get_OBJECT (obj_aecc_pipe_rules, AECC_PIPE_RULES)
  //dwg_get_OBJECT (obj_aecc_pipe_style, AECC_PIPE_STYLE)
  //dwg_get_OBJECT (obj_aecc_pipe_style_extension, AECC_PIPE_STYLE_EXTENSION)
  //dwg_get_OBJECT (obj_aecc_pointcloud_style, AECC_POINTCLOUD_STYLE)
  //dwg_get_OBJECT (obj_aecc_pointview, AECC_POINTVIEW)
  //dwg_get_OBJECT (obj_aecc_point_style, AECC_POINT_STYLE)
  //dwg_get_OBJECT (obj_aecc_pressure_part_list, AECC_PRESSURE_PART_LIST)
  //dwg_get_OBJECT (obj_aecc_pressure_pipe_style, AECC_PRESSURE_PIPE_STYLE)
  //dwg_get_OBJECT (obj_aecc_profilesectionentity_style, AECC_PROFILESECTIONENTITY_STYLE)
  //dwg_get_OBJECT (obj_aecc_profile_design_check_set, AECC_PROFILE_DESIGN_CHECK_SET)
  //dwg_get_OBJECT (obj_aecc_profile_label_set, AECC_PROFILE_LABEL_SET)
  //dwg_get_OBJECT (obj_aecc_profile_style, AECC_PROFILE_STYLE)
  //dwg_get_OBJECT (obj_aecc_profile_view_band_style_set, AECC_PROFILE_VIEW_BAND_STYLE_SET)
  //dwg_get_OBJECT (obj_aecc_profile_view_data_band_style, AECC_PROFILE_VIEW_DATA_BAND_STYLE)
  //dwg_get_OBJECT (obj_aecc_profile_view_horizontal_geometry_band_style, AECC_PROFILE_VIEW_HORIZONTAL_GEOMETRY_BAND_STYLE)
  //dwg_get_OBJECT (obj_aecc_profile_view_pipe_network_band_style, AECC_PROFILE_VIEW_PIPE_NETWORK_BAND_STYLE)
  //dwg_get_OBJECT (obj_aecc_profile_view_sectional_data_band_style, AECC_PROFILE_VIEW_SECTIONAL_DATA_BAND_STYLE)
  //dwg_get_OBJECT (obj_aecc_profile_view_style, AECC_PROFILE_VIEW_STYLE)
  //dwg_get_OBJECT (obj_aecc_profile_view_superelevation_diagram_band_style, AECC_PROFILE_VIEW_SUPERELEVATION_DIAGRAM_BAND_STYLE)
  //dwg_get_OBJECT (obj_aecc_profile_view_vertical_geometry_band_style, AECC_PROFILE_VIEW_VERTICAL_GEOMETRY_BAND_STYLE)
  //dwg_get_OBJECT (obj_aecc_quantity_takeoff_criteria, AECC_QUANTITY_TAKEOFF_CRITERIA)
  //dwg_get_OBJECT (obj_aecc_roadwaylink_style, AECC_ROADWAYLINK_STYLE)
  //dwg_get_OBJECT (obj_aecc_roadwaymarker_style, AECC_ROADWAYMARKER_STYLE)
  //dwg_get_OBJECT (obj_aecc_roadwayshape_style, AECC_ROADWAYSHAPE_STYLE)
  //dwg_get_OBJECT (obj_aecc_roadway_style_set, AECC_ROADWAY_STYLE_SET)
  //dwg_get_OBJECT (obj_aecc_root_settings_node, AECC_ROOT_SETTINGS_NODE)
  //dwg_get_OBJECT (obj_aecc_sample_line_group_style, AECC_SAMPLE_LINE_GROUP_STYLE)
  //dwg_get_OBJECT (obj_aecc_sample_line_style, AECC_SAMPLE_LINE_STYLE)
  //dwg_get_OBJECT (obj_aecc_section_label_set, AECC_SECTION_LABEL_SET)
  //dwg_get_OBJECT (obj_aecc_section_style, AECC_SECTION_STYLE)
  //dwg_get_OBJECT (obj_aecc_section_view_band_style_set, AECC_SECTION_VIEW_BAND_STYLE_SET)
  //dwg_get_OBJECT (obj_aecc_section_view_data_band_style, AECC_SECTION_VIEW_DATA_BAND_STYLE)
  //dwg_get_OBJECT (obj_aecc_section_view_road_surface_band_style, AECC_SECTION_VIEW_ROAD_SURFACE_BAND_STYLE)
  //dwg_get_OBJECT (obj_aecc_section_view_style, AECC_SECTION_VIEW_STYLE)
  //dwg_get_OBJECT (obj_aecc_settings_node, AECC_SETTINGS_NODE)
  //dwg_get_OBJECT (obj_aecc_sheet_style, AECC_SHEET_STYLE)
  //dwg_get_OBJECT (obj_aecc_slope_pattern_style, AECC_SLOPE_PATTERN_STYLE)
  //dwg_get_OBJECT (obj_aecc_station_format_style, AECC_STATION_FORMAT_STYLE)
  //dwg_get_OBJECT (obj_aecc_structure_rules, AECC_STRUCTURE_RULES)
  //dwg_get_OBJECT (obj_aecc_stucture_style, AECC_STUCTURE_STYLE)
  //dwg_get_OBJECT (obj_aecc_superelevation_diagram_view_style, AECC_SUPERELEVATION_DIAGRAM_VIEW_STYLE)
  //dwg_get_OBJECT (obj_aecc_surface_style, AECC_SURFACE_STYLE)
  //dwg_get_OBJECT (obj_aecc_svfigure_style, AECC_SVFIGURE_STYLE)
  //dwg_get_OBJECT (obj_aecc_svnetwork_style, AECC_SVNETWORK_STYLE)
  //dwg_get_OBJECT (obj_aecc_table_style, AECC_TABLE_STYLE)
  //dwg_get_OBJECT (obj_aecc_tag_manager, AECC_TAG_MANAGER)
  //dwg_get_OBJECT (obj_aecc_tree_node, AECC_TREE_NODE)
  //dwg_get_OBJECT (obj_aecc_user_defined_attribute_classification, AECC_USER_DEFINED_ATTRIBUTE_CLASSIFICATION)
  //dwg_get_OBJECT (obj_aecc_valignment_style_extension, AECC_VALIGNMENT_STYLE_EXTENSION)
  //dwg_get_OBJECT (obj_aecc_view_frame_style, AECC_VIEW_FRAME_STYLE)
  //dwg_get_OBJECT (obj_aecs_disp_props_member, AECS_DISP_PROPS_MEMBER)
  //dwg_get_OBJECT (obj_aecs_disp_props_member_logical, AECS_DISP_PROPS_MEMBER_LOGICAL)
  //dwg_get_OBJECT (obj_aecs_disp_props_member_plan, AECS_DISP_PROPS_MEMBER_PLAN)
  //dwg_get_OBJECT (obj_aecs_disp_props_member_plan_sketch, AECS_DISP_PROPS_MEMBER_PLAN_SKETCH)
  //dwg_get_OBJECT (obj_aecs_disp_props_member_projected, AECS_DISP_PROPS_MEMBER_PROJECTED)
  //dwg_get_OBJECT (obj_aecs_disp_rep_member_elevation_design, AECS_DISP_REP_MEMBER_ELEVATION_DESIGN)
  //dwg_get_OBJECT (obj_aecs_disp_rep_member_elevation_detail, AECS_DISP_REP_MEMBER_ELEVATION_DETAIL)
  //dwg_get_OBJECT (obj_aecs_disp_rep_member_logical, AECS_DISP_REP_MEMBER_LOGICAL)
  //dwg_get_OBJECT (obj_aecs_disp_rep_member_model_design, AECS_DISP_REP_MEMBER_MODEL_DESIGN)
  //dwg_get_OBJECT (obj_aecs_disp_rep_member_model_detail, AECS_DISP_REP_MEMBER_MODEL_DETAIL)
  //dwg_get_OBJECT (obj_aecs_disp_rep_member_plan_design, AECS_DISP_REP_MEMBER_PLAN_DESIGN)
  //dwg_get_OBJECT (obj_aecs_disp_rep_member_plan_detail, AECS_DISP_REP_MEMBER_PLAN_DETAIL)
  //dwg_get_OBJECT (obj_aecs_disp_rep_member_plan_sketch, AECS_DISP_REP_MEMBER_PLAN_SKETCH)
  //dwg_get_OBJECT (obj_aecs_member_node_shape, AECS_MEMBER_NODE_SHAPE)
  //dwg_get_OBJECT (obj_aecs_member_style, AECS_MEMBER_STYLE)
  //dwg_get_OBJECT (obj_aec_2dsection_style, AEC_2DSECTION_STYLE)
  //dwg_get_OBJECT (obj_aec_aecdbdisprepbdgelevlineplan100, AEC_AECDBDISPREPBDGELEVLINEPLAN100)
  //dwg_get_OBJECT (obj_aec_aecdbdisprepbdgelevlineplan50, AEC_AECDBDISPREPBDGELEVLINEPLAN50)
  //dwg_get_OBJECT (obj_aec_aecdbdisprepbdgsectionlineplan100, AEC_AECDBDISPREPBDGSECTIONLINEPLAN100)
  //dwg_get_OBJECT (obj_aec_aecdbdisprepbdgsectionlineplan50, AEC_AECDBDISPREPBDGSECTIONLINEPLAN50)
  //dwg_get_OBJECT (obj_aec_aecdbdisprepceilinggridplan100, AEC_AECDBDISPREPCEILINGGRIDPLAN100)
  //dwg_get_OBJECT (obj_aec_aecdbdisprepceilinggridplan50, AEC_AECDBDISPREPCEILINGGRIDPLAN50)
  //dwg_get_OBJECT (obj_aec_aecdbdisprepcolumngridplan100, AEC_AECDBDISPREPCOLUMNGRIDPLAN100)
  //dwg_get_OBJECT (obj_aec_aecdbdisprepcolumngridplan50, AEC_AECDBDISPREPCOLUMNGRIDPLAN50)
  //dwg_get_OBJECT (obj_aec_aecdbdisprepcurtainwalllayoutplan100, AEC_AECDBDISPREPCURTAINWALLLAYOUTPLAN100)
  //dwg_get_OBJECT (obj_aec_aecdbdisprepcurtainwalllayoutplan50, AEC_AECDBDISPREPCURTAINWALLLAYOUTPLAN50)
  //dwg_get_OBJECT (obj_aec_aecdbdisprepcurtainwallunitplan100, AEC_AECDBDISPREPCURTAINWALLUNITPLAN100)
  //dwg_get_OBJECT (obj_aec_aecdbdisprepcurtainwallunitplan50, AEC_AECDBDISPREPCURTAINWALLUNITPLAN50)
  //dwg_get_OBJECT (obj_aec_aecdbdisprepmvblockrefplan100, AEC_AECDBDISPREPMVBLOCKREFPLAN100)
  //dwg_get_OBJECT (obj_aec_aecdbdisprepmvblockrefplan50, AEC_AECDBDISPREPMVBLOCKREFPLAN50)
  //dwg_get_OBJECT (obj_aec_aecdbdispreproofplan100, AEC_AECDBDISPREPROOFPLAN100)
  //dwg_get_OBJECT (obj_aec_aecdbdispreproofplan50, AEC_AECDBDISPREPROOFPLAN50)
  //dwg_get_OBJECT (obj_aec_aecdbdispreproofslabplan100, AEC_AECDBDISPREPROOFSLABPLAN100)
  //dwg_get_OBJECT (obj_aec_aecdbdispreproofslabplan50, AEC_AECDBDISPREPROOFSLABPLAN50)
  //dwg_get_OBJECT (obj_aec_aecdbdisprepslabplan100, AEC_AECDBDISPREPSLABPLAN100)
  //dwg_get_OBJECT (obj_aec_aecdbdisprepslabplan50, AEC_AECDBDISPREPSLABPLAN50)
  //dwg_get_OBJECT (obj_aec_aecdbdisprepspaceplan100, AEC_AECDBDISPREPSPACEPLAN100)
  //dwg_get_OBJECT (obj_aec_aecdbdisprepspaceplan50, AEC_AECDBDISPREPSPACEPLAN50)
  //dwg_get_OBJECT (obj_aec_aecdbdisprepwallplan100, AEC_AECDBDISPREPWALLPLAN100)
  //dwg_get_OBJECT (obj_aec_aecdbdisprepwallplan50, AEC_AECDBDISPREPWALLPLAN50)
  //dwg_get_OBJECT (obj_aec_aecdbdisprepwindowassemblyplan100, AEC_AECDBDISPREPWINDOWASSEMBLYPLAN100)
  //dwg_get_OBJECT (obj_aec_aecdbdisprepwindowassemblyplan50, AEC_AECDBDISPREPWINDOWASSEMBLYPLAN50)
  //dwg_get_OBJECT (obj_aec_aecdbdisprepzone100, AEC_AECDBDISPREPZONE100)
  //dwg_get_OBJECT (obj_aec_aecdbdisprepzone50, AEC_AECDBDISPREPZONE50)
  //dwg_get_OBJECT (obj_aec_aecdbzonedef, AEC_AECDBZONEDEF)
  //dwg_get_OBJECT (obj_aec_aecdbzonestyle, AEC_AECDBZONESTYLE)
  //dwg_get_OBJECT (obj_aec_anchor_openingbase_to_wall, AEC_ANCHOR_OPENINGBASE_TO_WALL)
  //dwg_get_OBJECT (obj_aec_classification_def, AEC_CLASSIFICATION_DEF)
  //dwg_get_OBJECT (obj_aec_classification_system_def, AEC_CLASSIFICATION_SYSTEM_DEF)
  //dwg_get_OBJECT (obj_aec_cleanup_group_def, AEC_CLEANUP_GROUP_DEF)
  //dwg_get_OBJECT (obj_aec_curtain_wall_layout_style, AEC_CURTAIN_WALL_LAYOUT_STYLE)
  //dwg_get_OBJECT (obj_aec_curtain_wall_unit_style, AEC_CURTAIN_WALL_UNIT_STYLE)
  //dwg_get_OBJECT (obj_aec_cvsectionview, AEC_CVSECTIONVIEW)
  //dwg_get_OBJECT (obj_aec_db_disp_rep_dim_group_plan, AEC_DB_DISP_REP_DIM_GROUP_PLAN)
  //dwg_get_OBJECT (obj_aec_db_disp_rep_dim_group_plan100, AEC_DB_DISP_REP_DIM_GROUP_PLAN100)
  //dwg_get_OBJECT (obj_aec_db_disp_rep_dim_group_plan50, AEC_DB_DISP_REP_DIM_GROUP_PLAN50)
  //dwg_get_OBJECT (obj_aec_dim_style, AEC_DIM_STYLE)
  //dwg_get_OBJECT (obj_aec_displaytheme_style, AEC_DISPLAYTHEME_STYLE)
  //dwg_get_OBJECT (obj_aec_disprepaecdbdisprepmasselemplan100, AEC_DISPREPAECDBDISPREPMASSELEMPLAN100)
  //dwg_get_OBJECT (obj_aec_disprepaecdbdisprepmasselemplan50, AEC_DISPREPAECDBDISPREPMASSELEMPLAN50)
  //dwg_get_OBJECT (obj_aec_disprepaecdbdisprepmassgroupplan100, AEC_DISPREPAECDBDISPREPMASSGROUPPLAN100)
  //dwg_get_OBJECT (obj_aec_disprepaecdbdisprepmassgroupplan50, AEC_DISPREPAECDBDISPREPMASSGROUPPLAN50)
  //dwg_get_OBJECT (obj_aec_disprepaecdbdisprepopeningplan100, AEC_DISPREPAECDBDISPREPOPENINGPLAN100)
  //dwg_get_OBJECT (obj_aec_disprepaecdbdisprepopeningplan50, AEC_DISPREPAECDBDISPREPOPENINGPLAN50)
  //dwg_get_OBJECT (obj_aec_disprepaecdbdisprepopeningplanreflected, AEC_DISPREPAECDBDISPREPOPENINGPLANREFLECTED)
  //dwg_get_OBJECT (obj_aec_disprepaecdbdisprepopeningsillplan, AEC_DISPREPAECDBDISPREPOPENINGSILLPLAN)
  //dwg_get_OBJECT (obj_aec_dispropsmasselemplancommon, AEC_DISPROPSMASSELEMPLANCOMMON)
  //dwg_get_OBJECT (obj_aec_dispropsmassgroupplancommon, AEC_DISPROPSMASSGROUPPLANCOMMON)
  //dwg_get_OBJECT (obj_aec_dispropsopeningplancommon, AEC_DISPROPSOPENINGPLANCOMMON)
  //dwg_get_OBJECT (obj_aec_dispropsopeningplancommonhatched, AEC_DISPROPSOPENINGPLANCOMMONHATCHED)
  //dwg_get_OBJECT (obj_aec_dispropsopeningsillplan, AEC_DISPROPSOPENINGSILLPLAN)
  //dwg_get_OBJECT (obj_aec_disp_props_2d_section, AEC_DISP_PROPS_2D_SECTION)
  //dwg_get_OBJECT (obj_aec_disp_props_clip_volume, AEC_DISP_PROPS_CLIP_VOLUME)
  //dwg_get_OBJECT (obj_aec_disp_props_clip_volume_result, AEC_DISP_PROPS_CLIP_VOLUME_RESULT)
  //dwg_get_OBJECT (obj_aec_disp_props_dim, AEC_DISP_PROPS_DIM)
  //dwg_get_OBJECT (obj_aec_disp_props_displaytheme, AEC_DISP_PROPS_DISPLAYTHEME)
  //dwg_get_OBJECT (obj_aec_disp_props_door, AEC_DISP_PROPS_DOOR)
  //dwg_get_OBJECT (obj_aec_disp_props_door_nominal, AEC_DISP_PROPS_DOOR_NOMINAL)
  //dwg_get_OBJECT (obj_aec_disp_props_door_plan_100, AEC_DISP_PROPS_DOOR_PLAN_100)
  //dwg_get_OBJECT (obj_aec_disp_props_door_plan_50, AEC_DISP_PROPS_DOOR_PLAN_50)
  //dwg_get_OBJECT (obj_aec_disp_props_door_threshold_plan, AEC_DISP_PROPS_DOOR_THRESHOLD_PLAN)
  //dwg_get_OBJECT (obj_aec_disp_props_door_threshold_symbol_plan, AEC_DISP_PROPS_DOOR_THRESHOLD_SYMBOL_PLAN)
  //dwg_get_OBJECT (obj_aec_disp_props_editinplaceprofile_model, AEC_DISP_PROPS_EDITINPLACEPROFILE_MODEL)
  //dwg_get_OBJECT (obj_aec_disp_props_ent, AEC_DISP_PROPS_ENT)
  //dwg_get_OBJECT (obj_aec_disp_props_ent_ref, AEC_DISP_PROPS_ENT_REF)
  //dwg_get_OBJECT (obj_aec_disp_props_grid_assembly_model, AEC_DISP_PROPS_GRID_ASSEMBLY_MODEL)
  //dwg_get_OBJECT (obj_aec_disp_props_grid_assembly_plan, AEC_DISP_PROPS_GRID_ASSEMBLY_PLAN)
  //dwg_get_OBJECT (obj_aec_disp_props_layout_curve, AEC_DISP_PROPS_LAYOUT_CURVE)
  //dwg_get_OBJECT (obj_aec_disp_props_layout_grid2d, AEC_DISP_PROPS_LAYOUT_GRID2D)
  //dwg_get_OBJECT (obj_aec_disp_props_layout_grid3d, AEC_DISP_PROPS_LAYOUT_GRID3D)
  //dwg_get_OBJECT (obj_aec_disp_props_maskblock, AEC_DISP_PROPS_MASKBLOCK)
  //dwg_get_OBJECT (obj_aec_disp_props_mass_elem_model, AEC_DISP_PROPS_MASS_ELEM_MODEL)
  //dwg_get_OBJECT (obj_aec_disp_props_mass_group, AEC_DISP_PROPS_MASS_GROUP)
  //dwg_get_OBJECT (obj_aec_disp_props_material, AEC_DISP_PROPS_MATERIAL)
  //dwg_get_OBJECT (obj_aec_disp_props_opening, AEC_DISP_PROPS_OPENING)
  //dwg_get_OBJECT (obj_aec_disp_props_polygon_model, AEC_DISP_PROPS_POLYGON_MODEL)
  //dwg_get_OBJECT (obj_aec_disp_props_polygon_truecolour, AEC_DISP_PROPS_POLYGON_TRUECOLOUR)
  //dwg_get_OBJECT (obj_aec_disp_props_railing_model, AEC_DISP_PROPS_RAILING_MODEL)
  //dwg_get_OBJECT (obj_aec_disp_props_railing_plan, AEC_DISP_PROPS_RAILING_PLAN)
  //dwg_get_OBJECT (obj_aec_disp_props_roof, AEC_DISP_PROPS_ROOF)
  //dwg_get_OBJECT (obj_aec_disp_props_roofslab, AEC_DISP_PROPS_ROOFSLAB)
  //dwg_get_OBJECT (obj_aec_disp_props_roofslab_plan, AEC_DISP_PROPS_ROOFSLAB_PLAN)
  //dwg_get_OBJECT (obj_aec_disp_props_schedule_table, AEC_DISP_PROPS_SCHEDULE_TABLE)
  //dwg_get_OBJECT (obj_aec_disp_props_slab, AEC_DISP_PROPS_SLAB)
  //dwg_get_OBJECT (obj_aec_disp_props_slab_plan, AEC_DISP_PROPS_SLAB_PLAN)
  //dwg_get_OBJECT (obj_aec_disp_props_slice, AEC_DISP_PROPS_SLICE)
  //dwg_get_OBJECT (obj_aec_disp_props_space_decomposed, AEC_DISP_PROPS_SPACE_DECOMPOSED)
  //dwg_get_OBJECT (obj_aec_disp_props_space_model, AEC_DISP_PROPS_SPACE_MODEL)
  //dwg_get_OBJECT (obj_aec_disp_props_space_plan, AEC_DISP_PROPS_SPACE_PLAN)
  //dwg_get_OBJECT (obj_aec_disp_props_stair_model, AEC_DISP_PROPS_STAIR_MODEL)
  //dwg_get_OBJECT (obj_aec_disp_props_stair_plan, AEC_DISP_PROPS_STAIR_PLAN)
  //dwg_get_OBJECT (obj_aec_disp_props_stair_plan_overlapping, AEC_DISP_PROPS_STAIR_PLAN_OVERLAPPING)
  //dwg_get_OBJECT (obj_aec_disp_props_wall_graph, AEC_DISP_PROPS_WALL_GRAPH)
  //dwg_get_OBJECT (obj_aec_disp_props_wall_model, AEC_DISP_PROPS_WALL_MODEL)
  //dwg_get_OBJECT (obj_aec_disp_props_wall_plan, AEC_DISP_PROPS_WALL_PLAN)
  //dwg_get_OBJECT (obj_aec_disp_props_wall_schem, AEC_DISP_PROPS_WALL_SCHEM)
  //dwg_get_OBJECT (obj_aec_disp_props_window, AEC_DISP_PROPS_WINDOW)
  //dwg_get_OBJECT (obj_aec_disp_props_window_assembly_sill_plan, AEC_DISP_PROPS_WINDOW_ASSEMBLY_SILL_PLAN)
  //dwg_get_OBJECT (obj_aec_disp_props_window_nominal, AEC_DISP_PROPS_WINDOW_NOMINAL)
  //dwg_get_OBJECT (obj_aec_disp_props_window_plan_100, AEC_DISP_PROPS_WINDOW_PLAN_100)
  //dwg_get_OBJECT (obj_aec_disp_props_window_plan_50, AEC_DISP_PROPS_WINDOW_PLAN_50)
  //dwg_get_OBJECT (obj_aec_disp_props_window_sill_plan, AEC_DISP_PROPS_WINDOW_SILL_PLAN)
  //dwg_get_OBJECT (obj_aec_disp_props_zone, AEC_DISP_PROPS_ZONE)
  //dwg_get_OBJECT (obj_aec_disp_rep_2d_section, AEC_DISP_REP_2D_SECTION)
  //dwg_get_OBJECT (obj_aec_disp_rep_anchor, AEC_DISP_REP_ANCHOR)
  //dwg_get_OBJECT (obj_aec_disp_rep_anchor_bub_to_grid, AEC_DISP_REP_ANCHOR_BUB_TO_GRID)
  //dwg_get_OBJECT (obj_aec_disp_rep_anchor_bub_to_grid_model, AEC_DISP_REP_ANCHOR_BUB_TO_GRID_MODEL)
  //dwg_get_OBJECT (obj_aec_disp_rep_anchor_bub_to_grid_rcp, AEC_DISP_REP_ANCHOR_BUB_TO_GRID_RCP)
  //dwg_get_OBJECT (obj_aec_disp_rep_anchor_ent_to_node, AEC_DISP_REP_ANCHOR_ENT_TO_NODE)
  //dwg_get_OBJECT (obj_aec_disp_rep_anchor_ext_tag_to_ent, AEC_DISP_REP_ANCHOR_EXT_TAG_TO_ENT)
  //dwg_get_OBJECT (obj_aec_disp_rep_anchor_tag_to_ent, AEC_DISP_REP_ANCHOR_TAG_TO_ENT)
  //dwg_get_OBJECT (obj_aec_disp_rep_bdg_elevline_model, AEC_DISP_REP_BDG_ELEVLINE_MODEL)
  //dwg_get_OBJECT (obj_aec_disp_rep_bdg_elevline_plan, AEC_DISP_REP_BDG_ELEVLINE_PLAN)
  //dwg_get_OBJECT (obj_aec_disp_rep_bdg_elevline_rcp, AEC_DISP_REP_BDG_ELEVLINE_RCP)
  //dwg_get_OBJECT (obj_aec_disp_rep_bdg_sectionline_model, AEC_DISP_REP_BDG_SECTIONLINE_MODEL)
  //dwg_get_OBJECT (obj_aec_disp_rep_bdg_sectionline_plan, AEC_DISP_REP_BDG_SECTIONLINE_PLAN)
  //dwg_get_OBJECT (obj_aec_disp_rep_bdg_sectionline_rcp, AEC_DISP_REP_BDG_SECTIONLINE_RCP)
  //dwg_get_OBJECT (obj_aec_disp_rep_bdg_section_model, AEC_DISP_REP_BDG_SECTION_MODEL)
  //dwg_get_OBJECT (obj_aec_disp_rep_bdg_section_subdiv, AEC_DISP_REP_BDG_SECTION_SUBDIV)
  //dwg_get_OBJECT (obj_aec_disp_rep_ceiling_grid, AEC_DISP_REP_CEILING_GRID)
  //dwg_get_OBJECT (obj_aec_disp_rep_ceiling_grid_model, AEC_DISP_REP_CEILING_GRID_MODEL)
  //dwg_get_OBJECT (obj_aec_disp_rep_ceiling_grid_rcp, AEC_DISP_REP_CEILING_GRID_RCP)
  //dwg_get_OBJECT (obj_aec_disp_rep_clip_volume_model, AEC_DISP_REP_CLIP_VOLUME_MODEL)
  //dwg_get_OBJECT (obj_aec_disp_rep_clip_volume_plan, AEC_DISP_REP_CLIP_VOLUME_PLAN)
  //dwg_get_OBJECT (obj_aec_disp_rep_clip_volume_result, AEC_DISP_REP_CLIP_VOLUME_RESULT)
  //dwg_get_OBJECT (obj_aec_disp_rep_clip_volume_result_subdiv, AEC_DISP_REP_CLIP_VOLUME_RESULT_SUBDIV)
  //dwg_get_OBJECT (obj_aec_disp_rep_column_grid, AEC_DISP_REP_COLUMN_GRID)
  //dwg_get_OBJECT (obj_aec_disp_rep_column_grid_model, AEC_DISP_REP_COLUMN_GRID_MODEL)
  //dwg_get_OBJECT (obj_aec_disp_rep_column_grid_rcp, AEC_DISP_REP_COLUMN_GRID_RCP)
  //dwg_get_OBJECT (obj_aec_disp_rep_col_block, AEC_DISP_REP_COL_BLOCK)
  //dwg_get_OBJECT (obj_aec_disp_rep_col_circarc2d, AEC_DISP_REP_COL_CIRCARC2D)
  //dwg_get_OBJECT (obj_aec_disp_rep_col_concoincident, AEC_DISP_REP_COL_CONCOINCIDENT)
  //dwg_get_OBJECT (obj_aec_disp_rep_col_conconcentric, AEC_DISP_REP_COL_CONCONCENTRIC)
  //dwg_get_OBJECT (obj_aec_disp_rep_col_conequaldistance, AEC_DISP_REP_COL_CONEQUALDISTANCE)
  //dwg_get_OBJECT (obj_aec_disp_rep_col_conmidpoint, AEC_DISP_REP_COL_CONMIDPOINT)
  //dwg_get_OBJECT (obj_aec_disp_rep_col_connector, AEC_DISP_REP_COL_CONNECTOR)
  //dwg_get_OBJECT (obj_aec_disp_rep_col_connormal, AEC_DISP_REP_COL_CONNORMAL)
  //dwg_get_OBJECT (obj_aec_disp_rep_col_conparallel, AEC_DISP_REP_COL_CONPARALLEL)
  //dwg_get_OBJECT (obj_aec_disp_rep_col_conperpendicular, AEC_DISP_REP_COL_CONPERPENDICULAR)
  //dwg_get_OBJECT (obj_aec_disp_rep_col_consymmetric, AEC_DISP_REP_COL_CONSYMMETRIC)
  //dwg_get_OBJECT (obj_aec_disp_rep_col_contangent, AEC_DISP_REP_COL_CONTANGENT)
  //dwg_get_OBJECT (obj_aec_disp_rep_col_dimangle, AEC_DISP_REP_COL_DIMANGLE)
  //dwg_get_OBJECT (obj_aec_disp_rep_col_dimdiameter, AEC_DISP_REP_COL_DIMDIAMETER)
  //dwg_get_OBJECT (obj_aec_disp_rep_col_dimdistance, AEC_DISP_REP_COL_DIMDISTANCE)
  //dwg_get_OBJECT (obj_aec_disp_rep_col_dimlength, AEC_DISP_REP_COL_DIMLENGTH)
  //dwg_get_OBJECT (obj_aec_disp_rep_col_dimmajorradius, AEC_DISP_REP_COL_DIMMAJORRADIUS)
  //dwg_get_OBJECT (obj_aec_disp_rep_col_dimminorradius, AEC_DISP_REP_COL_DIMMINORRADIUS)
  //dwg_get_OBJECT (obj_aec_disp_rep_col_elliparc2d, AEC_DISP_REP_COL_ELLIPARC2D)
  //dwg_get_OBJECT (obj_aec_disp_rep_col_layoutdata, AEC_DISP_REP_COL_LAYOUTDATA)
  //dwg_get_OBJECT (obj_aec_disp_rep_col_line2d, AEC_DISP_REP_COL_LINE2D)
  //dwg_get_OBJECT (obj_aec_disp_rep_col_modifier_add, AEC_DISP_REP_COL_MODIFIER_ADD)
  //dwg_get_OBJECT (obj_aec_disp_rep_col_modifier_cutplane, AEC_DISP_REP_COL_MODIFIER_CUTPLANE)
  //dwg_get_OBJECT (obj_aec_disp_rep_col_modifier_extrusion, AEC_DISP_REP_COL_MODIFIER_EXTRUSION)
  //dwg_get_OBJECT (obj_aec_disp_rep_col_modifier_group, AEC_DISP_REP_COL_MODIFIER_GROUP)
  //dwg_get_OBJECT (obj_aec_disp_rep_col_modifier_loft, AEC_DISP_REP_COL_MODIFIER_LOFT)
  //dwg_get_OBJECT (obj_aec_disp_rep_col_modifier_path, AEC_DISP_REP_COL_MODIFIER_PATH)
  //dwg_get_OBJECT (obj_aec_disp_rep_col_modifier_revolve, AEC_DISP_REP_COL_MODIFIER_REVOLVE)
  //dwg_get_OBJECT (obj_aec_disp_rep_col_modifier_subtract, AEC_DISP_REP_COL_MODIFIER_SUBTRACT)
  //dwg_get_OBJECT (obj_aec_disp_rep_col_modifier_transition, AEC_DISP_REP_COL_MODIFIER_TRANSITION)
  //dwg_get_OBJECT (obj_aec_disp_rep_col_point2d, AEC_DISP_REP_COL_POINT2D)
  //dwg_get_OBJECT (obj_aec_disp_rep_col_profile, AEC_DISP_REP_COL_PROFILE)
  //dwg_get_OBJECT (obj_aec_disp_rep_col_workplane, AEC_DISP_REP_COL_WORKPLANE)
  //dwg_get_OBJECT (obj_aec_disp_rep_col_workplane_ref, AEC_DISP_REP_COL_WORKPLANE_REF)
  //dwg_get_OBJECT (obj_aec_disp_rep_config, AEC_DISP_REP_CONFIG)
  //dwg_get_OBJECT (obj_aec_disp_rep_curtain_wall_layout_model, AEC_DISP_REP_CURTAIN_WALL_LAYOUT_MODEL)
  //dwg_get_OBJECT (obj_aec_disp_rep_curtain_wall_layout_plan, AEC_DISP_REP_CURTAIN_WALL_LAYOUT_PLAN)
  //dwg_get_OBJECT (obj_aec_disp_rep_curtain_wall_unit_model, AEC_DISP_REP_CURTAIN_WALL_UNIT_MODEL)
  //dwg_get_OBJECT (obj_aec_disp_rep_curtain_wall_unit_plan, AEC_DISP_REP_CURTAIN_WALL_UNIT_PLAN)
  //dwg_get_OBJECT (obj_aec_disp_rep_dcm_dimradius, AEC_DISP_REP_DCM_DIMRADIUS)
  //dwg_get_OBJECT (obj_aec_disp_rep_displaytheme, AEC_DISP_REP_DISPLAYTHEME)
  //dwg_get_OBJECT (obj_aec_disp_rep_door_elev, AEC_DISP_REP_DOOR_ELEV)
  //dwg_get_OBJECT (obj_aec_disp_rep_door_model, AEC_DISP_REP_DOOR_MODEL)
  //dwg_get_OBJECT (obj_aec_disp_rep_door_nominal, AEC_DISP_REP_DOOR_NOMINAL)
  //dwg_get_OBJECT (obj_aec_disp_rep_door_plan, AEC_DISP_REP_DOOR_PLAN)
  //dwg_get_OBJECT (obj_aec_disp_rep_door_plan_50, AEC_DISP_REP_DOOR_PLAN_50)
  //dwg_get_OBJECT (obj_aec_disp_rep_door_plan_hekto, AEC_DISP_REP_DOOR_PLAN_HEKTO)
  //dwg_get_OBJECT (obj_aec_disp_rep_door_rcp, AEC_DISP_REP_DOOR_RCP)
  //dwg_get_OBJECT (obj_aec_disp_rep_door_threshold_plan, AEC_DISP_REP_DOOR_THRESHOLD_PLAN)
  //dwg_get_OBJECT (obj_aec_disp_rep_door_threshold_symbol_plan, AEC_DISP_REP_DOOR_THRESHOLD_SYMBOL_PLAN)
  //dwg_get_OBJECT (obj_aec_disp_rep_editinplaceprofile, AEC_DISP_REP_EDITINPLACEPROFILE)
  //dwg_get_OBJECT (obj_aec_disp_rep_ent_ref, AEC_DISP_REP_ENT_REF)
  //dwg_get_OBJECT (obj_aec_disp_rep_layout_curve, AEC_DISP_REP_LAYOUT_CURVE)
  //dwg_get_OBJECT (obj_aec_disp_rep_layout_grid2d, AEC_DISP_REP_LAYOUT_GRID2D)
  //dwg_get_OBJECT (obj_aec_disp_rep_layout_grid3d, AEC_DISP_REP_LAYOUT_GRID3D)
  //dwg_get_OBJECT (obj_aec_disp_rep_maskblock_ref, AEC_DISP_REP_MASKBLOCK_REF)
  //dwg_get_OBJECT (obj_aec_disp_rep_maskblock_ref_rcp, AEC_DISP_REP_MASKBLOCK_REF_RCP)
  //dwg_get_OBJECT (obj_aec_disp_rep_mass_elem_model, AEC_DISP_REP_MASS_ELEM_MODEL)
  //dwg_get_OBJECT (obj_aec_disp_rep_mass_elem_rcp, AEC_DISP_REP_MASS_ELEM_RCP)
  //dwg_get_OBJECT (obj_aec_disp_rep_mass_elem_schem, AEC_DISP_REP_MASS_ELEM_SCHEM)
  //dwg_get_OBJECT (obj_aec_disp_rep_mass_group_model, AEC_DISP_REP_MASS_GROUP_MODEL)
  //dwg_get_OBJECT (obj_aec_disp_rep_mass_group_plan, AEC_DISP_REP_MASS_GROUP_PLAN)
  //dwg_get_OBJECT (obj_aec_disp_rep_mass_group_rcp, AEC_DISP_REP_MASS_GROUP_RCP)
  //dwg_get_OBJECT (obj_aec_disp_rep_material, AEC_DISP_REP_MATERIAL)
  //dwg_get_OBJECT (obj_aec_disp_rep_mvblock_ref, AEC_DISP_REP_MVBLOCK_REF)
  //dwg_get_OBJECT (obj_aec_disp_rep_mvblock_ref_model, AEC_DISP_REP_MVBLOCK_REF_MODEL)
  //dwg_get_OBJECT (obj_aec_disp_rep_mvblock_ref_rcp, AEC_DISP_REP_MVBLOCK_REF_RCP)
  //dwg_get_OBJECT (obj_aec_disp_rep_opening, AEC_DISP_REP_OPENING)
  //dwg_get_OBJECT (obj_aec_disp_rep_opening_model, AEC_DISP_REP_OPENING_MODEL)
  //dwg_get_OBJECT (obj_aec_disp_rep_polygon_model, AEC_DISP_REP_POLYGON_MODEL)
  //dwg_get_OBJECT (obj_aec_disp_rep_polygon_truecolour, AEC_DISP_REP_POLYGON_TRUECOLOUR)
  //dwg_get_OBJECT (obj_aec_disp_rep_railing_model, AEC_DISP_REP_RAILING_MODEL)
  //dwg_get_OBJECT (obj_aec_disp_rep_railing_plan, AEC_DISP_REP_RAILING_PLAN)
  //dwg_get_OBJECT (obj_aec_disp_rep_railing_plan_100, AEC_DISP_REP_RAILING_PLAN_100)
  //dwg_get_OBJECT (obj_aec_disp_rep_railing_plan_50, AEC_DISP_REP_RAILING_PLAN_50)
  //dwg_get_OBJECT (obj_aec_disp_rep_roofslab_model, AEC_DISP_REP_ROOFSLAB_MODEL)
  //dwg_get_OBJECT (obj_aec_disp_rep_roofslab_plan, AEC_DISP_REP_ROOFSLAB_PLAN)
  //dwg_get_OBJECT (obj_aec_disp_rep_roof_model, AEC_DISP_REP_ROOF_MODEL)
  //dwg_get_OBJECT (obj_aec_disp_rep_roof_plan, AEC_DISP_REP_ROOF_PLAN)
  //dwg_get_OBJECT (obj_aec_disp_rep_roof_rcp, AEC_DISP_REP_ROOF_RCP)
  //dwg_get_OBJECT (obj_aec_disp_rep_schedule_table, AEC_DISP_REP_SCHEDULE_TABLE)
  //dwg_get_OBJECT (obj_aec_disp_rep_set, AEC_DISP_REP_SET)
  //dwg_get_OBJECT (obj_aec_disp_rep_slab_model, AEC_DISP_REP_SLAB_MODEL)
  //dwg_get_OBJECT (obj_aec_disp_rep_slab_plan, AEC_DISP_REP_SLAB_PLAN)
  //dwg_get_OBJECT (obj_aec_disp_rep_slice, AEC_DISP_REP_SLICE)
  //dwg_get_OBJECT (obj_aec_disp_rep_space_decomposed, AEC_DISP_REP_SPACE_DECOMPOSED)
  //dwg_get_OBJECT (obj_aec_disp_rep_space_model, AEC_DISP_REP_SPACE_MODEL)
  //dwg_get_OBJECT (obj_aec_disp_rep_space_plan, AEC_DISP_REP_SPACE_PLAN)
  //dwg_get_OBJECT (obj_aec_disp_rep_space_rcp, AEC_DISP_REP_SPACE_RCP)
  //dwg_get_OBJECT (obj_aec_disp_rep_space_volume, AEC_DISP_REP_SPACE_VOLUME)
  //dwg_get_OBJECT (obj_aec_disp_rep_stair_model, AEC_DISP_REP_STAIR_MODEL)
  //dwg_get_OBJECT (obj_aec_disp_rep_stair_plan, AEC_DISP_REP_STAIR_PLAN)
  //dwg_get_OBJECT (obj_aec_disp_rep_stair_plan_100, AEC_DISP_REP_STAIR_PLAN_100)
  //dwg_get_OBJECT (obj_aec_disp_rep_stair_plan_50, AEC_DISP_REP_STAIR_PLAN_50)
  //dwg_get_OBJECT (obj_aec_disp_rep_stair_plan_overlapping, AEC_DISP_REP_STAIR_PLAN_OVERLAPPING)
  //dwg_get_OBJECT (obj_aec_disp_rep_stair_rcp, AEC_DISP_REP_STAIR_RCP)
  //dwg_get_OBJECT (obj_aec_disp_rep_wall_graph, AEC_DISP_REP_WALL_GRAPH)
  //dwg_get_OBJECT (obj_aec_disp_rep_wall_model, AEC_DISP_REP_WALL_MODEL)
  //dwg_get_OBJECT (obj_aec_disp_rep_wall_plan, AEC_DISP_REP_WALL_PLAN)
  //dwg_get_OBJECT (obj_aec_disp_rep_wall_rcp, AEC_DISP_REP_WALL_RCP)
  //dwg_get_OBJECT (obj_aec_disp_rep_wall_schem, AEC_DISP_REP_WALL_SCHEM)
  //dwg_get_OBJECT (obj_aec_disp_rep_windowassembly_sill_plan, AEC_DISP_REP_WINDOWASSEMBLY_SILL_PLAN)
  //dwg_get_OBJECT (obj_aec_disp_rep_window_assembly_model, AEC_DISP_REP_WINDOW_ASSEMBLY_MODEL)
  //dwg_get_OBJECT (obj_aec_disp_rep_window_assembly_plan, AEC_DISP_REP_WINDOW_ASSEMBLY_PLAN)
  //dwg_get_OBJECT (obj_aec_disp_rep_window_elev, AEC_DISP_REP_WINDOW_ELEV)
  //dwg_get_OBJECT (obj_aec_disp_rep_window_model, AEC_DISP_REP_WINDOW_MODEL)
  //dwg_get_OBJECT (obj_aec_disp_rep_window_nominal, AEC_DISP_REP_WINDOW_NOMINAL)
  //dwg_get_OBJECT (obj_aec_disp_rep_window_plan, AEC_DISP_REP_WINDOW_PLAN)
  //dwg_get_OBJECT (obj_aec_disp_rep_window_plan_100, AEC_DISP_REP_WINDOW_PLAN_100)
  //dwg_get_OBJECT (obj_aec_disp_rep_window_plan_50, AEC_DISP_REP_WINDOW_PLAN_50)
  //dwg_get_OBJECT (obj_aec_disp_rep_window_rcp, AEC_DISP_REP_WINDOW_RCP)
  //dwg_get_OBJECT (obj_aec_disp_rep_window_sill_plan, AEC_DISP_REP_WINDOW_SILL_PLAN)
  //dwg_get_OBJECT (obj_aec_disp_rep_zone, AEC_DISP_REP_ZONE)
  //dwg_get_OBJECT (obj_aec_disp_rops_railing_plan_100, AEC_DISP_ROPS_RAILING_PLAN_100)
  //dwg_get_OBJECT (obj_aec_disp_rops_railing_plan_50, AEC_DISP_ROPS_RAILING_PLAN_50)
  //dwg_get_OBJECT (obj_aec_disp_rops_stair_plan_100, AEC_DISP_ROPS_STAIR_PLAN_100)
  //dwg_get_OBJECT (obj_aec_disp_rops_stair_plan_50, AEC_DISP_ROPS_STAIR_PLAN_50)
  //dwg_get_OBJECT (obj_aec_door_style, AEC_DOOR_STYLE)
  //dwg_get_OBJECT (obj_aec_endcap_style, AEC_ENDCAP_STYLE)
  //dwg_get_OBJECT (obj_aec_frame_def, AEC_FRAME_DEF)
  //dwg_get_OBJECT (obj_aec_layerkey_style, AEC_LAYERKEY_STYLE)
  //dwg_get_OBJECT (obj_aec_list_def, AEC_LIST_DEF)
  //dwg_get_OBJECT (obj_aec_maskblock_def, AEC_MASKBLOCK_DEF)
  //dwg_get_OBJECT (obj_aec_mass_elem_style, AEC_MASS_ELEM_STYLE)
  //dwg_get_OBJECT (obj_aec_material_def, AEC_MATERIAL_DEF)
  //dwg_get_OBJECT (obj_aec_mvblock_def, AEC_MVBLOCK_DEF)
  //dwg_get_OBJECT (obj_aec_mvblock_ref, AEC_MVBLOCK_REF)
  //dwg_get_OBJECT (obj_aec_notification_tracker, AEC_NOTIFICATION_TRACKER)
  //dwg_get_OBJECT (obj_aec_polygon, AEC_POLYGON)
  //dwg_get_OBJECT (obj_aec_polygon_style, AEC_POLYGON_STYLE)
  //dwg_get_OBJECT (obj_aec_property_set_def, AEC_PROPERTY_SET_DEF)
  //dwg_get_OBJECT (obj_aec_railing_style, AEC_RAILING_STYLE)
  //dwg_get_OBJECT (obj_aec_refedit_status_tracker, AEC_REFEDIT_STATUS_TRACKER)
  //dwg_get_OBJECT (obj_aec_roofslabedge_style, AEC_ROOFSLABEDGE_STYLE)
  //dwg_get_OBJECT (obj_aec_roofslab_style, AEC_ROOFSLAB_STYLE)
  //dwg_get_OBJECT (obj_aec_schedule_data_format, AEC_SCHEDULE_DATA_FORMAT)
  //dwg_get_OBJECT (obj_aec_slabedge_style, AEC_SLABEDGE_STYLE)
  //dwg_get_OBJECT (obj_aec_slab_style, AEC_SLAB_STYLE)
  //dwg_get_OBJECT (obj_aec_space_styles, AEC_SPACE_STYLES)
  //dwg_get_OBJECT (obj_aec_stair_style, AEC_STAIR_STYLE)
  //dwg_get_OBJECT (obj_aec_stair_winder_style, AEC_STAIR_WINDER_STYLE)
  //dwg_get_OBJECT (obj_aec_stair_winder_type_balanced, AEC_STAIR_WINDER_TYPE_BALANCED)
  //dwg_get_OBJECT (obj_aec_stair_winder_type_manual, AEC_STAIR_WINDER_TYPE_MANUAL)
  //dwg_get_OBJECT (obj_aec_stair_winder_type_single_point, AEC_STAIR_WINDER_TYPE_SINGLE_POINT)
  //dwg_get_OBJECT (obj_aec_vars_aecbbldsrv, AEC_VARS_AECBBLDSRV)
  //dwg_get_OBJECT (obj_aec_vars_archbase, AEC_VARS_ARCHBASE)
  //dwg_get_OBJECT (obj_aec_vars_dwg_setup, AEC_VARS_DWG_SETUP)
  //dwg_get_OBJECT (obj_aec_vars_munich, AEC_VARS_MUNICH)
  //dwg_get_OBJECT (obj_aec_vars_structurebase, AEC_VARS_STRUCTUREBASE)
  //dwg_get_OBJECT (obj_aec_wallmod_style, AEC_WALLMOD_STYLE)
  //dwg_get_OBJECT (obj_aec_wall_style, AEC_WALL_STYLE)
  //dwg_get_OBJECT (obj_aec_window_assembly_style, AEC_WINDOW_ASSEMBLY_STYLE)
  //dwg_get_OBJECT (obj_aec_window_style, AEC_WINDOW_STYLE)
  //dwg_get_OBJECT (obj_alignmentgripentity, ALIGNMENTGRIPENTITY)
  //dwg_get_OBJECT (obj_amcontextmgr, AMCONTEXTMGR)
  //dwg_get_OBJECT (obj_amdtadmenustate, AMDTADMENUSTATE)
  //dwg_get_OBJECT (obj_amdtammenustate, AMDTAMMENUSTATE)
  //dwg_get_OBJECT (obj_amdtbrowserdbtab, AMDTBROWSERDBTAB)
  //dwg_get_OBJECT (obj_amdtdmmenustate, AMDTDMMENUSTATE)
  //dwg_get_OBJECT (obj_amdtedgestandarddin, AMDTEDGESTANDARDDIN)
  //dwg_get_OBJECT (obj_amdtedgestandarddin13715, AMDTEDGESTANDARDDIN13715)
  //dwg_get_OBJECT (obj_amdtedgestandardiso, AMDTEDGESTANDARDISO)
  //dwg_get_OBJECT (obj_amdtedgestandardiso13715, AMDTEDGESTANDARDISO13715)
  //dwg_get_OBJECT (obj_amdtformulaupdatedispatcher, AMDTFORMULAUPDATEDISPATCHER)
  //dwg_get_OBJECT (obj_amdtinternalreactor, AMDTINTERNALREACTOR)
  //dwg_get_OBJECT (obj_amdtmcommenustate, AMDTMCOMMENUSTATE)
  //dwg_get_OBJECT (obj_amdtmenustatemgr, AMDTMENUSTATEMGR)
  //dwg_get_OBJECT (obj_amdtnote, AMDTNOTE)
  //dwg_get_OBJECT (obj_amdtnotetemplatedb, AMDTNOTETEMPLATEDB)
  //dwg_get_OBJECT (obj_amdtsectionsym, AMDTSECTIONSYM)
  //dwg_get_OBJECT (obj_amdtsectionsymlabel, AMDTSECTIONSYMLABEL)
  //dwg_get_OBJECT (obj_amdtsysattr, AMDTSYSATTR)
  //dwg_get_OBJECT (obj_amgobjpropcfg, AMGOBJPROPCFG)
  //dwg_get_OBJECT (obj_amgsettingsobj, AMGSETTINGSOBJ)
  //dwg_get_OBJECT (obj_amimaster, AMIMASTER)
  //dwg_get_OBJECT (obj_am_drawing_mgr, AM_DRAWING_MGR)
  //dwg_get_OBJECT (obj_am_dwgmgr_name, AM_DWGMGR_NAME)
  //dwg_get_OBJECT (obj_am_dwg_document, AM_DWG_DOCUMENT)
  //dwg_get_OBJECT (obj_am_dwg_sheet, AM_DWG_SHEET)
  //dwg_get_OBJECT (obj_am_viewdimparmap, AM_VIEWDIMPARMAP)
  //dwg_get_OBJECT (obj_binrecord, BINRECORD)
  //dwg_get_OBJECT (obj_camscatalogappobject, CAMSCATALOGAPPOBJECT)
  //dwg_get_OBJECT (obj_camsstructbtnstate, CAMSSTRUCTBTNSTATE)
  //dwg_get_OBJECT (obj_catalogstate, CATALOGSTATE)
  //dwg_get_OBJECT (obj_cbrowserappobject, CBROWSERAPPOBJECT)
  //dwg_get_OBJECT (obj_depmgr, DEPMGR)
  //dwg_get_OBJECT (obj_dmbaseelement, DMBASEELEMENT)
  //dwg_get_OBJECT (obj_dmdefaultstyle, DMDEFAULTSTYLE)
  //dwg_get_OBJECT (obj_dmlegend, DMLEGEND)
  //dwg_get_OBJECT (obj_dmmap, DMMAP)
  //dwg_get_OBJECT (obj_dmmapmanager, DMMAPMANAGER)
  //dwg_get_OBJECT (obj_dmstylecategory, DMSTYLECATEGORY)
  //dwg_get_OBJECT (obj_dmstylelibrary, DMSTYLELIBRARY)
  //dwg_get_OBJECT (obj_dmstylereference, DMSTYLEREFERENCE)
  //dwg_get_OBJECT (obj_dmstylizedentitiestable, DMSTYLIZEDENTITIESTABLE)
  //dwg_get_OBJECT (obj_dmsurrogatestylesets, DMSURROGATESTYLESETS)
  //dwg_get_OBJECT (obj_dm_placeholder, DM_PLACEHOLDER)
  //dwg_get_OBJECT (obj_exactermxrefmap, EXACTERMXREFMAP)
  //dwg_get_OBJECT (obj_exacxrefpanelobject, EXACXREFPANELOBJECT)
  //dwg_get_OBJECT (obj_expo_notifyblock, EXPO_NOTIFYBLOCK)
  //dwg_get_OBJECT (obj_expo_notifyhall, EXPO_NOTIFYHALL)
  //dwg_get_OBJECT (obj_expo_notifypillar, EXPO_NOTIFYPILLAR)
  //dwg_get_OBJECT (obj_expo_notifystand, EXPO_NOTIFYSTAND)
  //dwg_get_OBJECT (obj_expo_notifystandnopoly, EXPO_NOTIFYSTANDNOPOLY)
  //dwg_get_OBJECT (obj_flipactionentity, FLIPACTIONENTITY)
  //dwg_get_OBJECT (obj_gsmanager, GSMANAGER)
  //dwg_get_OBJECT (obj_ird_dsc_dict, IRD_DSC_DICT)
  //dwg_get_OBJECT (obj_ird_dsc_record, IRD_DSC_RECORD)
  //dwg_get_OBJECT (obj_ird_obj_record, IRD_OBJ_RECORD)
  //dwg_get_OBJECT (obj_mapfsmrvobject, MAPFSMRVOBJECT)
  //dwg_get_OBJECT (obj_mapgwsundoobject, MAPGWSUNDOOBJECT)
  //dwg_get_OBJECT (obj_mapiammoudle, MAPIAMMOUDLE)
  //dwg_get_OBJECT (obj_mapmetadataobject, MAPMETADATAOBJECT)
  //dwg_get_OBJECT (obj_mapresourcemanagerobject, MAPRESOURCEMANAGEROBJECT)
  //dwg_get_OBJECT (obj_moveactionentity, MOVEACTIONENTITY)
  //dwg_get_OBJECT (obj_mcdbcontainer2, McDbContainer2)
  //dwg_get_OBJECT (obj_mcdbmarker, McDbMarker)
  //dwg_get_OBJECT (obj_namedappl, NAMEDAPPL)
  //dwg_get_OBJECT (obj_newstdpartparlist, NEWSTDPARTPARLIST)
  //dwg_get_OBJECT (obj_npocollection, NPOCOLLECTION)
  //dwg_get_OBJECT (obj_objcloner, OBJCLONER)
  //dwg_get_OBJECT (obj_parammgr, PARAMMGR)
  //dwg_get_OBJECT (obj_paramscope, PARAMSCOPE)
  //dwg_get_OBJECT (obj_pillar, PILLAR)
  //dwg_get_OBJECT (obj_rapidrtrenderenvironment, RAPIDRTRENDERENVIRONMENT)
  //dwg_get_OBJECT (obj_rotateactionentity, ROTATEACTIONENTITY)
  //dwg_get_OBJECT (obj_scaleactionentity, SCALEACTIONENTITY)
  //dwg_get_OBJECT (obj_stdpart2d, STDPART2D)
  //dwg_get_OBJECT (obj_stretchactionentity, STRETCHACTIONENTITY)
  //dwg_get_OBJECT (obj_tch_arrow, TCH_ARROW)
  //dwg_get_OBJECT (obj_tch_axis_label, TCH_AXIS_LABEL)
  //dwg_get_OBJECT (obj_tch_block_insert, TCH_BLOCK_INSERT)
  //dwg_get_OBJECT (obj_tch_column, TCH_COLUMN)
  //dwg_get_OBJECT (obj_tch_dbconfig, TCH_DBCONFIG)
  //dwg_get_OBJECT (obj_tch_dimension2, TCH_DIMENSION2)
  //dwg_get_OBJECT (obj_tch_drawingindex, TCH_DRAWINGINDEX)
  //dwg_get_OBJECT (obj_tch_handrail, TCH_HANDRAIL)
  //dwg_get_OBJECT (obj_tch_linestair, TCH_LINESTAIR)
  //dwg_get_OBJECT (obj_tch_opening, TCH_OPENING)
  //dwg_get_OBJECT (obj_tch_rectstair, TCH_RECTSTAIR)
  //dwg_get_OBJECT (obj_tch_slab, TCH_SLAB)
  //dwg_get_OBJECT (obj_tch_space, TCH_SPACE)
  //dwg_get_OBJECT (obj_tch_text, TCH_TEXT)
  //dwg_get_OBJECT (obj_tch_wall, TCH_WALL)
  //dwg_get_OBJECT (obj_tgrupopuntos, TGrupoPuntos)
  //dwg_get_OBJECT (obj_vaacimageinventory, VAACIMAGEINVENTORY)
  //dwg_get_OBJECT (obj_vaacxrefpanelobject, VAACXREFPANELOBJECT)
  //dwg_get_OBJECT (obj_xrefpanelobject, XREFPANELOBJECT)
#  endif
// clang-format: on

/********************************************************************
 * Functions to return NULL-terminated array of all owned entities  *
 ********************************************************************/

/**
 * \fn Dwg_Entity_ENTITY* dwg_getall_ENTITY(Dwg_Object_Ref *hdr)
 * \code
 * Usage:
 * Dwg_Entity_TEXT* texts = dwg_getall_TEXT (text,
 *                                           dwg->header_vars.mspace_block);
 * \endcode
 * \param[in]    hdr Dwg_Object_Ref     to a BLOCK_CONTROL obj
 * \return       malloced NULL-terminated array
 *
 * Extracts all entities of this type from a block header (mspace or pspace),
 * and returns a MALLOCed NULL-terminated array.
 */
// clang-format: off
//< \fn Dwg_Entity_TEXT* dwg_getall_TEXT (Dwg_Object_Ref *hdr)
DWG_GETALL_ENTITY (_3DFACE)
DWG_GETALL_ENTITY (_3DLINE)
DWG_GETALL_ENTITY (_3DSOLID)
DWG_GETALL_ENTITY (ARC)
DWG_GETALL_ENTITY (ATTDEF)
DWG_GETALL_ENTITY (ATTRIB)
DWG_GETALL_ENTITY (BLOCK)
DWG_GETALL_ENTITY (BODY)
DWG_GETALL_ENTITY (CAMERA)
DWG_GETALL_ENTITY (CIRCLE)
DWG_GETALL_ENTITY (DGNUNDERLAY)
DWG_GETALL_ENTITY (DIMENSION_ALIGNED)
DWG_GETALL_ENTITY (DIMENSION_ANG2LN)
DWG_GETALL_ENTITY (DIMENSION_ANG3PT)
DWG_GETALL_ENTITY (DIMENSION_DIAMETER)
DWG_GETALL_ENTITY (DIMENSION_LINEAR)
DWG_GETALL_ENTITY (DIMENSION_ORDINATE)
DWG_GETALL_ENTITY (DIMENSION_RADIUS)
DWG_GETALL_ENTITY (DWFUNDERLAY)
DWG_GETALL_ENTITY (ELLIPSE)
DWG_GETALL_ENTITY (ENDBLK)
DWG_GETALL_ENTITY (ENDREP)
DWG_GETALL_ENTITY (HATCH)
DWG_GETALL_ENTITY (IMAGE)
DWG_GETALL_ENTITY (INSERT)
DWG_GETALL_ENTITY (JUMP)
DWG_GETALL_ENTITY (LEADER)
DWG_GETALL_ENTITY (LIGHT)
DWG_GETALL_ENTITY (LINE)
DWG_GETALL_ENTITY (LOAD)
DWG_GETALL_ENTITY (LWPOLYLINE)
DWG_GETALL_ENTITY (MESH)
DWG_GETALL_ENTITY (MINSERT)
DWG_GETALL_ENTITY (MLINE)
DWG_GETALL_ENTITY (MTEXT)
DWG_GETALL_ENTITY (MULTILEADER)
DWG_GETALL_ENTITY (OLE2FRAME)
DWG_GETALL_ENTITY (OLEFRAME)
DWG_GETALL_ENTITY (PDFUNDERLAY)
DWG_GETALL_ENTITY (POINT)
DWG_GETALL_ENTITY (POLYLINE_2D)
DWG_GETALL_ENTITY (POLYLINE_3D)
DWG_GETALL_ENTITY (POLYLINE_MESH)
DWG_GETALL_ENTITY (POLYLINE_PFACE)
DWG_GETALL_ENTITY (PROXY_ENTITY)
DWG_GETALL_ENTITY (RAY)
DWG_GETALL_ENTITY (REGION)
DWG_GETALL_ENTITY (REPEAT)
DWG_GETALL_ENTITY (SECTIONOBJECT)
DWG_GETALL_ENTITY (SEQEND)
DWG_GETALL_ENTITY (SHAPE)
DWG_GETALL_ENTITY (SOLID)
DWG_GETALL_ENTITY (SPLINE)
DWG_GETALL_ENTITY (TEXT)
DWG_GETALL_ENTITY (TOLERANCE)
DWG_GETALL_ENTITY (TRACE)
DWG_GETALL_ENTITY (UNKNOWN_ENT)
DWG_GETALL_ENTITY (VERTEX_2D)
DWG_GETALL_ENTITY (VERTEX_3D)
DWG_GETALL_ENTITY (VERTEX_MESH)
DWG_GETALL_ENTITY (VERTEX_PFACE)
DWG_GETALL_ENTITY (VERTEX_PFACE_FACE)
DWG_GETALL_ENTITY (VIEWPORT)
DWG_GETALL_ENTITY (WIPEOUT)
DWG_GETALL_ENTITY (XLINE)
/* unstable */
DWG_GETALL_ENTITY (ARC_DIMENSION)
DWG_GETALL_ENTITY (HELIX)
DWG_GETALL_ENTITY (LARGE_RADIAL_DIMENSION)
DWG_GETALL_ENTITY (LAYOUTPRINTCONFIG)
DWG_GETALL_ENTITY (PLANESURFACE)
DWG_GETALL_ENTITY (POINTCLOUD)
DWG_GETALL_ENTITY (POINTCLOUDEX)
/* debugging */
DWG_GETALL_ENTITY (ALIGNMENTPARAMETERENTITY)
DWG_GETALL_ENTITY (ARCALIGNEDTEXT)
DWG_GETALL_ENTITY (BASEPOINTPARAMETERENTITY)
DWG_GETALL_ENTITY (EXTRUDEDSURFACE)
DWG_GETALL_ENTITY (FLIPGRIPENTITY)
DWG_GETALL_ENTITY (FLIPPARAMETERENTITY)
DWG_GETALL_ENTITY (GEOPOSITIONMARKER)
DWG_GETALL_ENTITY (LINEARGRIPENTITY)
DWG_GETALL_ENTITY (LINEARPARAMETERENTITY)
DWG_GETALL_ENTITY (LOFTEDSURFACE)
DWG_GETALL_ENTITY (MPOLYGON)
DWG_GETALL_ENTITY (NAVISWORKSMODEL)
DWG_GETALL_ENTITY (NURBSURFACE)
DWG_GETALL_ENTITY (POINTPARAMETERENTITY)
DWG_GETALL_ENTITY (POLARGRIPENTITY)
DWG_GETALL_ENTITY (REVOLVEDSURFACE)
DWG_GETALL_ENTITY (ROTATIONGRIPENTITY)
DWG_GETALL_ENTITY (ROTATIONPARAMETERENTITY)
DWG_GETALL_ENTITY (RTEXT)
DWG_GETALL_ENTITY (SWEPTSURFACE)
DWG_GETALL_ENTITY (TABLE)
DWG_GETALL_ENTITY (VISIBILITYGRIPENTITY)
DWG_GETALL_ENTITY (VISIBILITYPARAMETERENTITY)
DWG_GETALL_ENTITY (XYGRIPENTITY)
DWG_GETALL_ENTITY (XYPARAMETERENTITY)

// clang-format: on
/********************************************************************
 *     Functions to return NULL-terminated array of all objects     *
 ********************************************************************/

/**
 * \fn Dwg_Object_OBJECT dwg_getall_OBJECT(Dwg_Data *dwg)
 * Extracts all objects of this type from a dwg, and returns a MALLOCed
 * NULL-terminated array.
 */

// clang-format: off
DWG_GETALL_OBJECT (ACSH_BOOLEAN_CLASS)
DWG_GETALL_OBJECT (ACSH_BOX_CLASS)
DWG_GETALL_OBJECT (ACSH_CONE_CLASS)
DWG_GETALL_OBJECT (ACSH_CYLINDER_CLASS)
DWG_GETALL_OBJECT (ACSH_FILLET_CLASS)
DWG_GETALL_OBJECT (ACSH_HISTORY_CLASS)
DWG_GETALL_OBJECT (ACSH_SPHERE_CLASS)
DWG_GETALL_OBJECT (ACSH_TORUS_CLASS)
DWG_GETALL_OBJECT (ACSH_WEDGE_CLASS)
DWG_GETALL_OBJECT (APPID)
DWG_GETALL_OBJECT (APPID_CONTROL)
DWG_GETALL_OBJECT (ASSOCGEOMDEPENDENCY)
DWG_GETALL_OBJECT (ASSOCNETWORK)
DWG_GETALL_OBJECT (BLOCKALIGNMENTGRIP)
DWG_GETALL_OBJECT (BLOCKALIGNMENTPARAMETER)
DWG_GETALL_OBJECT (BLOCKBASEPOINTPARAMETER)
DWG_GETALL_OBJECT (BLOCKFLIPACTION)
DWG_GETALL_OBJECT (BLOCKFLIPGRIP)
DWG_GETALL_OBJECT (BLOCKFLIPPARAMETER)
DWG_GETALL_OBJECT (BLOCKGRIPLOCATIONCOMPONENT)
DWG_GETALL_OBJECT (BLOCKLINEARGRIP)
DWG_GETALL_OBJECT (BLOCKLOOKUPGRIP)
DWG_GETALL_OBJECT (BLOCKMOVEACTION)
DWG_GETALL_OBJECT (BLOCKROTATEACTION)
DWG_GETALL_OBJECT (BLOCKROTATIONGRIP)
DWG_GETALL_OBJECT (BLOCKSCALEACTION)
DWG_GETALL_OBJECT (BLOCKVISIBILITYGRIP)
DWG_GETALL_OBJECT (BLOCK_CONTROL)
DWG_GETALL_OBJECT (BLOCK_HEADER)
DWG_GETALL_OBJECT (CELLSTYLEMAP)
DWG_GETALL_OBJECT (DETAILVIEWSTYLE)
DWG_GETALL_OBJECT (DICTIONARY)
DWG_GETALL_OBJECT (DICTIONARYVAR)
DWG_GETALL_OBJECT (DICTIONARYWDFLT)
DWG_GETALL_OBJECT (DIMSTYLE)
DWG_GETALL_OBJECT (DIMSTYLE_CONTROL)
DWG_GETALL_OBJECT (DUMMY)
DWG_GETALL_OBJECT (DYNAMICBLOCKPURGEPREVENTER)
DWG_GETALL_OBJECT (FIELD)
DWG_GETALL_OBJECT (FIELDLIST)
DWG_GETALL_OBJECT (GEODATA)
DWG_GETALL_OBJECT (GROUP)
DWG_GETALL_OBJECT (IDBUFFER)
DWG_GETALL_OBJECT (IMAGEDEF)
DWG_GETALL_OBJECT (IMAGEDEF_REACTOR)
DWG_GETALL_OBJECT (INDEX)
DWG_GETALL_OBJECT (LAYER)
DWG_GETALL_OBJECT (LAYERFILTER)
DWG_GETALL_OBJECT (LAYER_CONTROL)
DWG_GETALL_OBJECT (LAYER_INDEX)
DWG_GETALL_OBJECT (LAYOUT)
DWG_GETALL_OBJECT (LONG_TRANSACTION)
DWG_GETALL_OBJECT (LTYPE)
DWG_GETALL_OBJECT (LTYPE_CONTROL)
DWG_GETALL_OBJECT (MLINESTYLE)
DWG_GETALL_OBJECT (PLACEHOLDER)
DWG_GETALL_OBJECT (PLOTSETTINGS)
DWG_GETALL_OBJECT (RASTERVARIABLES)
DWG_GETALL_OBJECT (RENDERENVIRONMENT)
DWG_GETALL_OBJECT (SCALE)
DWG_GETALL_OBJECT (SECTIONVIEWSTYLE)
DWG_GETALL_OBJECT (SECTION_MANAGER)
DWG_GETALL_OBJECT (SORTENTSTABLE)
DWG_GETALL_OBJECT (SPATIAL_FILTER)
DWG_GETALL_OBJECT (STYLE)
DWG_GETALL_OBJECT (STYLE_CONTROL)
DWG_GETALL_OBJECT (SUN)
DWG_GETALL_OBJECT (TABLEGEOMETRY)
DWG_GETALL_OBJECT (UCS)
DWG_GETALL_OBJECT (UCS_CONTROL)
DWG_GETALL_OBJECT (UNKNOWN_OBJ)
DWG_GETALL_OBJECT (VBA_PROJECT)
DWG_GETALL_OBJECT (VIEW)
DWG_GETALL_OBJECT (VIEW_CONTROL)
DWG_GETALL_OBJECT (VISUALSTYLE)
DWG_GETALL_OBJECT (VPORT)
DWG_GETALL_OBJECT (VPORT_CONTROL)
DWG_GETALL_OBJECT (VX_CONTROL)
DWG_GETALL_OBJECT (VX_TABLE_RECORD)
DWG_GETALL_OBJECT (WIPEOUTVARIABLES)
DWG_GETALL_OBJECT (XRECORD)
DWG_GETALL_OBJECT (PDFDEFINITION)
DWG_GETALL_OBJECT (DGNDEFINITION)
DWG_GETALL_OBJECT (DWFDEFINITION)
/* unstable */
DWG_GETALL_OBJECT (ACSH_BREP_CLASS)
DWG_GETALL_OBJECT (ACSH_CHAMFER_CLASS)
DWG_GETALL_OBJECT (ACSH_PYRAMID_CLASS)
DWG_GETALL_OBJECT (ALDIMOBJECTCONTEXTDATA)
DWG_GETALL_OBJECT (ASSOC2DCONSTRAINTGROUP)
DWG_GETALL_OBJECT (ASSOCACTION)
DWG_GETALL_OBJECT (ASSOCACTIONPARAM)
DWG_GETALL_OBJECT (ASSOCARRAYACTIONBODY)
DWG_GETALL_OBJECT (ASSOCASMBODYACTIONPARAM)
DWG_GETALL_OBJECT (ASSOCBLENDSURFACEACTIONBODY)
DWG_GETALL_OBJECT (ASSOCCOMPOUNDACTIONPARAM)
DWG_GETALL_OBJECT (ASSOCDEPENDENCY)
DWG_GETALL_OBJECT (ASSOCDIMDEPENDENCYBODY)
DWG_GETALL_OBJECT (ASSOCEXTENDSURFACEACTIONBODY)
DWG_GETALL_OBJECT (ASSOCEXTRUDEDSURFACEACTIONBODY)
DWG_GETALL_OBJECT (ASSOCFACEACTIONPARAM)
DWG_GETALL_OBJECT (ASSOCFILLETSURFACEACTIONBODY)
DWG_GETALL_OBJECT (ASSOCLOFTEDSURFACEACTIONBODY)
DWG_GETALL_OBJECT (ASSOCNETWORKSURFACEACTIONBODY)
DWG_GETALL_OBJECT (ASSOCOBJECTACTIONPARAM)
DWG_GETALL_OBJECT (ASSOCOFFSETSURFACEACTIONBODY)
DWG_GETALL_OBJECT (ASSOCOSNAPPOINTREFACTIONPARAM)
DWG_GETALL_OBJECT (ASSOCPATCHSURFACEACTIONBODY)
DWG_GETALL_OBJECT (ASSOCPATHACTIONPARAM)
DWG_GETALL_OBJECT (ASSOCPLANESURFACEACTIONBODY)
DWG_GETALL_OBJECT (ASSOCPOINTREFACTIONPARAM)
DWG_GETALL_OBJECT (ASSOCREVOLVEDSURFACEACTIONBODY)
DWG_GETALL_OBJECT (ASSOCTRIMSURFACEACTIONBODY)
DWG_GETALL_OBJECT (ASSOCVALUEDEPENDENCY)
DWG_GETALL_OBJECT (ASSOCVARIABLE)
DWG_GETALL_OBJECT (ASSOCVERTEXACTIONPARAM)
DWG_GETALL_OBJECT (BLKREFOBJECTCONTEXTDATA)
DWG_GETALL_OBJECT (BLOCKALIGNEDCONSTRAINTPARAMETER)
DWG_GETALL_OBJECT (BLOCKANGULARCONSTRAINTPARAMETER)
DWG_GETALL_OBJECT (BLOCKARRAYACTION)
DWG_GETALL_OBJECT (BLOCKDIAMETRICCONSTRAINTPARAMETER)
DWG_GETALL_OBJECT (BLOCKHORIZONTALCONSTRAINTPARAMETER)
DWG_GETALL_OBJECT (BLOCKLINEARCONSTRAINTPARAMETER)
DWG_GETALL_OBJECT (BLOCKLINEARPARAMETER)
DWG_GETALL_OBJECT (BLOCKLOOKUPACTION)
DWG_GETALL_OBJECT (BLOCKLOOKUPPARAMETER)
DWG_GETALL_OBJECT (BLOCKPARAMDEPENDENCYBODY)
DWG_GETALL_OBJECT (BLOCKPOINTPARAMETER)
DWG_GETALL_OBJECT (BLOCKPOLARGRIP)
DWG_GETALL_OBJECT (BLOCKPOLARPARAMETER)
DWG_GETALL_OBJECT (BLOCKPOLARSTRETCHACTION)
DWG_GETALL_OBJECT (BLOCKRADIALCONSTRAINTPARAMETER)
DWG_GETALL_OBJECT (BLOCKREPRESENTATION)
DWG_GETALL_OBJECT (BLOCKROTATIONPARAMETER)
DWG_GETALL_OBJECT (BLOCKSTRETCHACTION)
DWG_GETALL_OBJECT (BLOCKUSERPARAMETER)
DWG_GETALL_OBJECT (BLOCKVERTICALCONSTRAINTPARAMETER)
DWG_GETALL_OBJECT (BLOCKVISIBILITYPARAMETER)
DWG_GETALL_OBJECT (BLOCKXYGRIP)
DWG_GETALL_OBJECT (BLOCKXYPARAMETER)
DWG_GETALL_OBJECT (DATALINK)
DWG_GETALL_OBJECT (DBCOLOR)
DWG_GETALL_OBJECT (EVALUATION_GRAPH)
DWG_GETALL_OBJECT (FCFOBJECTCONTEXTDATA)
DWG_GETALL_OBJECT (GRADIENT_BACKGROUND)
DWG_GETALL_OBJECT (GROUND_PLANE_BACKGROUND)
DWG_GETALL_OBJECT (IBL_BACKGROUND)
DWG_GETALL_OBJECT (IMAGE_BACKGROUND)
DWG_GETALL_OBJECT (LEADEROBJECTCONTEXTDATA)
DWG_GETALL_OBJECT (LIGHTLIST)
DWG_GETALL_OBJECT (MATERIAL)
DWG_GETALL_OBJECT (MENTALRAYRENDERSETTINGS)
DWG_GETALL_OBJECT (MLEADERSTYLE)
DWG_GETALL_OBJECT (MTEXTOBJECTCONTEXTDATA)
DWG_GETALL_OBJECT (OBJECT_PTR)
DWG_GETALL_OBJECT (PARTIAL_VIEWING_INDEX)
DWG_GETALL_OBJECT (POINTCLOUDCOLORMAP)
DWG_GETALL_OBJECT (POINTCLOUDDEF)
DWG_GETALL_OBJECT (POINTCLOUDDEFEX)
DWG_GETALL_OBJECT (POINTCLOUDDEF_REACTOR)
DWG_GETALL_OBJECT (POINTCLOUDDEF_REACTOR_EX)
DWG_GETALL_OBJECT (PROXY_OBJECT)
DWG_GETALL_OBJECT (RAPIDRTRENDERSETTINGS)
DWG_GETALL_OBJECT (RENDERENTRY)
DWG_GETALL_OBJECT (RENDERGLOBAL)
DWG_GETALL_OBJECT (RENDERSETTINGS)
DWG_GETALL_OBJECT (SECTION_SETTINGS)
DWG_GETALL_OBJECT (SKYLIGHT_BACKGROUND)
DWG_GETALL_OBJECT (SOLID_BACKGROUND)
DWG_GETALL_OBJECT (SPATIAL_INDEX)
DWG_GETALL_OBJECT (TABLESTYLE)
DWG_GETALL_OBJECT (TEXTOBJECTCONTEXTDATA)
DWG_GETALL_OBJECT (ASSOCARRAYMODIFYPARAMETERS)
DWG_GETALL_OBJECT (ASSOCARRAYPATHPARAMETERS)
DWG_GETALL_OBJECT (ASSOCARRAYPOLARPARAMETERS)
DWG_GETALL_OBJECT (ASSOCARRAYRECTANGULARPARAMETERS)
#ifdef DEBUG_CLASSES
  DWG_GETALL_OBJECT (ACMECOMMANDHISTORY)
  DWG_GETALL_OBJECT (ACMESCOPE)
  DWG_GETALL_OBJECT (ACMESTATEMGR)
  DWG_GETALL_OBJECT (ACSH_EXTRUSION_CLASS)
  DWG_GETALL_OBJECT (ACSH_LOFT_CLASS)
  DWG_GETALL_OBJECT (ACSH_REVOLVE_CLASS)
  DWG_GETALL_OBJECT (ACSH_SWEEP_CLASS)
  DWG_GETALL_OBJECT (ANGDIMOBJECTCONTEXTDATA)
  DWG_GETALL_OBJECT (ANNOTSCALEOBJECTCONTEXTDATA)
  DWG_GETALL_OBJECT (ASSOC3POINTANGULARDIMACTIONBODY)
  DWG_GETALL_OBJECT (ASSOCALIGNEDDIMACTIONBODY)
  DWG_GETALL_OBJECT (ASSOCARRAYMODIFYACTIONBODY)
  DWG_GETALL_OBJECT (ASSOCEDGEACTIONPARAM)
  DWG_GETALL_OBJECT (ASSOCEDGECHAMFERACTIONBODY)
  DWG_GETALL_OBJECT (ASSOCEDGEFILLETACTIONBODY)
  DWG_GETALL_OBJECT (ASSOCMLEADERACTIONBODY)
  DWG_GETALL_OBJECT (ASSOCORDINATEDIMACTIONBODY)
  DWG_GETALL_OBJECT (ASSOCPERSSUBENTMANAGER)
  DWG_GETALL_OBJECT (ASSOCRESTOREENTITYSTATEACTIONBODY)
  DWG_GETALL_OBJECT (ASSOCROTATEDDIMACTIONBODY)
  DWG_GETALL_OBJECT (ASSOCSWEPTSURFACEACTIONBODY)
  DWG_GETALL_OBJECT (BLOCKPROPERTIESTABLE)
  DWG_GETALL_OBJECT (BLOCKPROPERTIESTABLEGRIP)
  DWG_GETALL_OBJECT (BREAKDATA)
  DWG_GETALL_OBJECT (BREAKPOINTREF)
  DWG_GETALL_OBJECT (CONTEXTDATAMANAGER)
  DWG_GETALL_OBJECT (CSACDOCUMENTOPTIONS)
  DWG_GETALL_OBJECT (CURVEPATH)
  DWG_GETALL_OBJECT (DATATABLE)
  DWG_GETALL_OBJECT (DIMASSOC)
  DWG_GETALL_OBJECT (DMDIMOBJECTCONTEXTDATA)
  DWG_GETALL_OBJECT (DYNAMICBLOCKPROXYNODE)
  DWG_GETALL_OBJECT (GEOMAPIMAGE)
  DWG_GETALL_OBJECT (MLEADEROBJECTCONTEXTDATA)
  DWG_GETALL_OBJECT (MOTIONPATH)
  DWG_GETALL_OBJECT (MTEXTATTRIBUTEOBJECTCONTEXTDATA)
  DWG_GETALL_OBJECT (NAVISWORKSMODELDEF)
  DWG_GETALL_OBJECT (ORDDIMOBJECTCONTEXTDATA)
  DWG_GETALL_OBJECT (PERSUBENTMGR)
  DWG_GETALL_OBJECT (POINTPATH)
  DWG_GETALL_OBJECT (RADIMLGOBJECTCONTEXTDATA)
  DWG_GETALL_OBJECT (RADIMOBJECTCONTEXTDATA)
  DWG_GETALL_OBJECT (SUNSTUDY)
  DWG_GETALL_OBJECT (TABLECONTENT)
  DWG_GETALL_OBJECT (TVDEVICEPROPERTIES)
  //DWG_GETALL_OBJECT (ABSHDRAWINGSETTINGS)
  //DWG_GETALL_OBJECT (ACAECUSTOBJ)
  //DWG_GETALL_OBJECT (ACAEEEMGROBJ)
  //DWG_GETALL_OBJECT (ACAMCOMP)
  //DWG_GETALL_OBJECT (ACAMCOMPDEF)
  //DWG_GETALL_OBJECT (ACAMCOMPDEFMGR)
  //DWG_GETALL_OBJECT (ACAMCONTEXTMODELER)
  //DWG_GETALL_OBJECT (ACAMGDIMSTD)
  //DWG_GETALL_OBJECT (ACAMGFILTERDAT)
  //DWG_GETALL_OBJECT (ACAMGHOLECHARTSTDCSN)
  //DWG_GETALL_OBJECT (ACAMGHOLECHARTSTDDIN)
  //DWG_GETALL_OBJECT (ACAMGHOLECHARTSTDISO)
  //DWG_GETALL_OBJECT (ACAMGLAYSTD)
  //DWG_GETALL_OBJECT (ACAMGRCOMPDEF)
  //DWG_GETALL_OBJECT (ACAMGRCOMPDEFSET)
  //DWG_GETALL_OBJECT (ACAMGTITLESTD)
  //DWG_GETALL_OBJECT (ACAMMVDBACKUPOBJECT)
  //DWG_GETALL_OBJECT (ACAMPROJECT)
  //DWG_GETALL_OBJECT (ACAMSHAFTCOMPDEF)
  //DWG_GETALL_OBJECT (ACAMSTDPCOMPDEF)
  //DWG_GETALL_OBJECT (ACAMWBLOCKTEMPENTS)
  //DWG_GETALL_OBJECT (ACARRAYJIGENTITY)
  //DWG_GETALL_OBJECT (ACCMCONTEXT)
  //DWG_GETALL_OBJECT (ACDBCIRCARCRES)
  //DWG_GETALL_OBJECT (ACDBDIMENSIONRES)
  //DWG_GETALL_OBJECT (ACDBENTITYCACHE)
  //DWG_GETALL_OBJECT (ACDBLINERES)
  //DWG_GETALL_OBJECT (ACDBSTDPARTRES_ARC)
  //DWG_GETALL_OBJECT (ACDBSTDPARTRES_LINE)
  //DWG_GETALL_OBJECT (ACDB_HATCHSCALECONTEXTDATA_CLASS)
  //DWG_GETALL_OBJECT (ACDB_HATCHVIEWCONTEXTDATA_CLASS)
  //DWG_GETALL_OBJECT (ACDB_PROXY_ENTITY_DATA)
  //DWG_GETALL_OBJECT (ACDSRECORD)
  //DWG_GETALL_OBJECT (ACDSSCHEMA)
  //DWG_GETALL_OBJECT (ACGREFACADMASTER)
  //DWG_GETALL_OBJECT (ACGREFMASTER)
  //DWG_GETALL_OBJECT (ACIMINTSYSVAR)
  //DWG_GETALL_OBJECT (ACIMREALSYSVAR)
  //DWG_GETALL_OBJECT (ACIMSTRSYSVAR)
  //DWG_GETALL_OBJECT (ACIMSYSVARMAN)
  //DWG_GETALL_OBJECT (ACMANOOTATIONVIEWSTANDARDANSI)
  //DWG_GETALL_OBJECT (ACMANOOTATIONVIEWSTANDARDCSN)
  //DWG_GETALL_OBJECT (ACMANOOTATIONVIEWSTANDARDDIN)
  //DWG_GETALL_OBJECT (ACMANOOTATIONVIEWSTANDARDISO)
  //DWG_GETALL_OBJECT (ACMAPLEGENDDBOBJECT)
  //DWG_GETALL_OBJECT (ACMAPLEGENDITEMDBOBJECT)
  //DWG_GETALL_OBJECT (ACMAPMAPVIEWPORTDBOBJECT)
  //DWG_GETALL_OBJECT (ACMAPPRINTLAYOUTELEMENTDBOBJECTCONTAINER)
  //DWG_GETALL_OBJECT (ACMBALLOON)
  //DWG_GETALL_OBJECT (ACMBOM)
  //DWG_GETALL_OBJECT (ACMBOMROW)
  //DWG_GETALL_OBJECT (ACMBOMROWSTRUCT)
  //DWG_GETALL_OBJECT (ACMBOMSTANDARDANSI)
  //DWG_GETALL_OBJECT (ACMBOMSTANDARDCSN)
  //DWG_GETALL_OBJECT (ACMBOMSTANDARDDIN)
  //DWG_GETALL_OBJECT (ACMBOMSTANDARDISO)
  //DWG_GETALL_OBJECT (ACMCENTERLINESTANDARDANSI)
  //DWG_GETALL_OBJECT (ACMCENTERLINESTANDARDCSN)
  //DWG_GETALL_OBJECT (ACMCENTERLINESTANDARDDIN)
  //DWG_GETALL_OBJECT (ACMCENTERLINESTANDARDISO)
  //DWG_GETALL_OBJECT (ACMDATADICTIONARY)
  //DWG_GETALL_OBJECT (ACMDATAENTRY)
  //DWG_GETALL_OBJECT (ACMDATAENTRYBLOCK)
  //DWG_GETALL_OBJECT (ACMDATUMID)
  //DWG_GETALL_OBJECT (ACMDATUMSTANDARDANSI)
  //DWG_GETALL_OBJECT (ACMDATUMSTANDARDCSN)
  //DWG_GETALL_OBJECT (ACMDATUMSTANDARDDIN)
  //DWG_GETALL_OBJECT (ACMDATUMSTANDARDISO)
  //DWG_GETALL_OBJECT (ACMDATUMSTANDARDISO2012)
  //DWG_GETALL_OBJECT (ACMDETAILSTANDARDANSI)
  //DWG_GETALL_OBJECT (ACMDETAILSTANDARDCSN)
  //DWG_GETALL_OBJECT (ACMDETAILSTANDARDDIN)
  //DWG_GETALL_OBJECT (ACMDETAILSTANDARDISO)
  //DWG_GETALL_OBJECT (ACMDETAILTANDARDCUSTOM)
  //DWG_GETALL_OBJECT (ACMDIMBREAKPERSREACTOR)
  //DWG_GETALL_OBJECT (ACMEDRAWINGMAN)
  //DWG_GETALL_OBJECT (ACMEVIEW)
  //DWG_GETALL_OBJECT (ACME_DATABASE)
  //DWG_GETALL_OBJECT (ACME_DOCUMENT)
  //DWG_GETALL_OBJECT (ACMFCFRAME)
  //DWG_GETALL_OBJECT (ACMFCFSTANDARDANSI)
  //DWG_GETALL_OBJECT (ACMFCFSTANDARDCSN)
  //DWG_GETALL_OBJECT (ACMFCFSTANDARDDIN)
  //DWG_GETALL_OBJECT (ACMFCFSTANDARDISO)
  //DWG_GETALL_OBJECT (ACMFCFSTANDARDISO2004)
  //DWG_GETALL_OBJECT (ACMFCFSTANDARDISO2012)
  //DWG_GETALL_OBJECT (ACMIDSTANDARDANSI)
  //DWG_GETALL_OBJECT (ACMIDSTANDARDCSN)
  //DWG_GETALL_OBJECT (ACMIDSTANDARDDIN)
  //DWG_GETALL_OBJECT (ACMIDSTANDARDISO)
  //DWG_GETALL_OBJECT (ACMIDSTANDARDISO2004)
  //DWG_GETALL_OBJECT (ACMIDSTANDARDISO2012)
  //DWG_GETALL_OBJECT (ACMNOTESTANDARDANSI)
  //DWG_GETALL_OBJECT (ACMNOTESTANDARDCSN)
  //DWG_GETALL_OBJECT (ACMNOTESTANDARDDIN)
  //DWG_GETALL_OBJECT (ACMNOTESTANDARDISO)
  //DWG_GETALL_OBJECT (ACMPARTLIST)
  //DWG_GETALL_OBJECT (ACMPICKOBJ)
  //DWG_GETALL_OBJECT (ACMSECTIONSTANDARDANSI)
  //DWG_GETALL_OBJECT (ACMSECTIONSTANDARDCSN2002)
  //DWG_GETALL_OBJECT (ACMSECTIONSTANDARDCUSTOM)
  //DWG_GETALL_OBJECT (ACMSECTIONSTANDARDDIN)
  //DWG_GETALL_OBJECT (ACMSECTIONSTANDARDISO)
  //DWG_GETALL_OBJECT (ACMSECTIONSTANDARDISO2001)
  //DWG_GETALL_OBJECT (ACMSTANDARDANSI)
  //DWG_GETALL_OBJECT (ACMSTANDARDCSN)
  //DWG_GETALL_OBJECT (ACMSTANDARDDIN)
  //DWG_GETALL_OBJECT (ACMSTANDARDISO)
  //DWG_GETALL_OBJECT (ACMSURFSTANDARDANSI)
  //DWG_GETALL_OBJECT (ACMSURFSTANDARDCSN)
  //DWG_GETALL_OBJECT (ACMSURFSTANDARDDIN)
  //DWG_GETALL_OBJECT (ACMSURFSTANDARDISO)
  //DWG_GETALL_OBJECT (ACMSURFSTANDARDISO2002)
  //DWG_GETALL_OBJECT (ACMSURFSYM)
  //DWG_GETALL_OBJECT (ACMTAPERSTANDARDANSI)
  //DWG_GETALL_OBJECT (ACMTAPERSTANDARDCSN)
  //DWG_GETALL_OBJECT (ACMTAPERSTANDARDDIN)
  //DWG_GETALL_OBJECT (ACMTAPERSTANDARDISO)
  //DWG_GETALL_OBJECT (ACMTHREADLINESTANDARDANSI)
  //DWG_GETALL_OBJECT (ACMTHREADLINESTANDARDCSN)
  //DWG_GETALL_OBJECT (ACMTHREADLINESTANDARDDIN)
  //DWG_GETALL_OBJECT (ACMTHREADLINESTANDARDISO)
  //DWG_GETALL_OBJECT (ACMWELDSTANDARDANSI)
  //DWG_GETALL_OBJECT (ACMWELDSTANDARDCSN)
  //DWG_GETALL_OBJECT (ACMWELDSTANDARDDIN)
  //DWG_GETALL_OBJECT (ACMWELDSTANDARDISO)
  //DWG_GETALL_OBJECT (ACMWELDSYM)
  //DWG_GETALL_OBJECT (ACRFATTGENMGR)
  //DWG_GETALL_OBJECT (ACRFINSADJ)
  //DWG_GETALL_OBJECT (ACRFINSADJUSTERMGR)
  //DWG_GETALL_OBJECT (ACRFMCADAPIATTHOLDER)
  //DWG_GETALL_OBJECT (ACRFOBJATTMGR)
  //DWG_GETALL_OBJECT (ACSH_SUBENT_MATERIAL_CLASS)
  //DWG_GETALL_OBJECT (AC_AM_2D_XREF_MGR)
  //DWG_GETALL_OBJECT (AC_AM_BASIC_VIEW)
  //DWG_GETALL_OBJECT (AC_AM_BASIC_VIEW_DEF)
  //DWG_GETALL_OBJECT (AC_AM_COMPLEX_HIDE_SITUATION)
  //DWG_GETALL_OBJECT (AC_AM_COMP_VIEW_DEF)
  //DWG_GETALL_OBJECT (AC_AM_COMP_VIEW_INST)
  //DWG_GETALL_OBJECT (AC_AM_DIRTY_NODES)
  //DWG_GETALL_OBJECT (AC_AM_HIDE_SITUATION)
  //DWG_GETALL_OBJECT (AC_AM_MAPPER_CACHE)
  //DWG_GETALL_OBJECT (AC_AM_MASTER_VIEW_DEF)
  //DWG_GETALL_OBJECT (AC_AM_MVD_DEP_MGR)
  //DWG_GETALL_OBJECT (AC_AM_OVERRIDE_FILTER)
  //DWG_GETALL_OBJECT (AC_AM_PROPS_OVERRIDE)
  //DWG_GETALL_OBJECT (AC_AM_SHAFT_HIDE_SITUATION)
  //DWG_GETALL_OBJECT (AC_AM_STDP_VIEW_DEF)
  //DWG_GETALL_OBJECT (AC_AM_TRANSFORM_GHOST)
  //DWG_GETALL_OBJECT (ADAPPL)
  //DWG_GETALL_OBJECT (AECC_ALIGNMENT_DESIGN_CHECK_SET)
  //DWG_GETALL_OBJECT (AECC_ALIGNMENT_LABEL_SET)
  //DWG_GETALL_OBJECT (AECC_ALIGNMENT_LABEL_SET_EXT)
  //DWG_GETALL_OBJECT (AECC_ALIGNMENT_PARCEL_NODE)
  //DWG_GETALL_OBJECT (AECC_ALIGNMENT_STYLE)
  //DWG_GETALL_OBJECT (AECC_APPURTENANCE_STYLE)
  //DWG_GETALL_OBJECT (AECC_ASSEMBLY_STYLE)
  //DWG_GETALL_OBJECT (AECC_BUILDING_SITE_STYLE)
  //DWG_GETALL_OBJECT (AECC_CANT_DIAGRAM_VIEW_STYLE)
  //DWG_GETALL_OBJECT (AECC_CATCHMENT_STYLE)
  //DWG_GETALL_OBJECT (AECC_CLASS_NODE)
  //DWG_GETALL_OBJECT (AECC_CONTOURVIEW)
  //DWG_GETALL_OBJECT (AECC_CORRIDOR_STYLE)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_ALIGNMENT)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_ALIGNMENT_CANT_LABEL_GROUP)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_ALIGNMENT_CSV)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_ALIGNMENT_CURVE_LABEL)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_ALIGNMENT_DESIGNSPEED_LABEL_GROUP)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_ALIGNMENT_GEOMPT_LABEL_GROUP)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_ALIGNMENT_INDEXED_PI_LABEL)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_ALIGNMENT_MINOR_STATION_LABEL_GROUP)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_ALIGNMENT_PI_LABEL)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_ALIGNMENT_SPIRAL_LABEL)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_ALIGNMENT_STAEQU_LABEL_GROUP)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_ALIGNMENT_STATION_LABEL_GROUP)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_ALIGNMENT_STATION_OFFSET_LABEL)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_ALIGNMENT_SUPERELEVATION_LABEL_GROUP)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_ALIGNMENT_TABLE)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_ALIGNMENT_TANGENT_LABEL)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_ALIGNMENT_VERTICAL_GEOMPT_LABELING)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_APPURTENANCE)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_APPURTENANCE_CSV)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_APPURTENANCE_LABELING)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_APPURTENANCE_PROFILE_LABELING)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_ASSEMBLY)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_AUTO_CORRIDOR_FEATURE_LINE)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_AUTO_CORRIDOR_FEATURE_LINE_PROFILE)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_AUTO_CORRIDOR_FEATURE_LINE_SECTION)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_AUTO_FEATURE_LINE)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_AUTO_FEATURE_LINE_CSV)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_AUTO_FEATURE_LINE_PROFILE)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_AUTO_FEATURE_LINE_SECTION)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_BUILDINGSITE)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_BUILDINGUTIL_CONNECTOR)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_CANT_DIAGRAM_VIEW)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_CATCHMENT_AREA)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_CATCHMENT_AREA_LABEL)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_CORRIDOR)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_CROSSING_PIPE_PROFILE_LABELING)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_CROSSING_PRESSURE_PIPE_PROFILE_LABELING)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_CSVSTATIONSLIDER)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_FACE)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_FEATURE)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_FEATURE_LABEL)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_FEATURE_LINE)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_FEATURE_LINE_CSV)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_FEATURE_LINE_PROFILE)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_FEATURE_LINE_SECTION)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_FITTING)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_FITTING_CSV)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_FITTING_LABELING)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_FITTING_PROFILE_LABELING)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_FLOW_SEGMENT_LABEL)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_GENERAL_SEGMENT_LABEL)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_GRADING)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_GRAPH)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_GRAPHPROFILE_NETWORKPART)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_GRAPHPROFILE_PRESSUREPART)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_GRID_SURFACE)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_GRID_SURFACE_CSV)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_HORGEOMETRY_BAND_LABEL_GROUP)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_HYDRO_REGION)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_INTERFERENCE_CHECK)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_INTERFERENCE_PART)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_INTERFERENCE_PART_SECTION)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_INTERSECTION)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_INTERSECTION_LOCATION_LABELING)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_LABELING)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_LEGEND_TABLE)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_LINE_BETWEEN_POINTS_LABEL)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_LOTLINE_CSV)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_MASSHAULLINE)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_MASS_HAUL_VIEW)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_MATCHLINE_LABELING)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_MATCH_LINE)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_MATERIAL_SECTION)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_NETWORK)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_NOTE_LABEL)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_OFFSET_ELEV_LABEL)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_PARCEL_BOUNDARY)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_PARCEL_FACE_LABEL)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_PARCEL_SEGMENT)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_PARCEL_SEGMENT_LABEL)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_PARCEL_SEGMENT_TABLE)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_PARCEL_TABLE)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_PIPE)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_PIPENETWORK_BAND_LABEL_GROUP)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_PIPE_CSV)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_PIPE_LABELING)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_PIPE_PROFILE_LABELING)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_PIPE_SECTION_LABELING)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_PIPE_TABLE)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_POINT_ENT)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_POINT_GROUP)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_POINT_TABLE)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_PRESSUREPIPENETWORK)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_PRESSURE_PART_TABLE)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_PRESSURE_PIPE)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_PRESSURE_PIPE_CSV)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_PRESSURE_PIPE_LABELING)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_PRESSURE_PIPE_PROFILE_LABELING)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_PRESSURE_PIPE_SECTION_LABELING)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_PROFILE)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_PROFILEDATA_BAND_LABEL_GROUP)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_PROFILE_PROJECTION)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_PROFILE_PROJECTION_LABEL)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_PROFILE_VIEW)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_PROFILE_VIEW_DEPTH_LABEL)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_QUANTITY_TAKEOFF_AGGREGATE_EARTHWORK_TABLE)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_RIGHT_OF_WAY)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_SAMPLELINE_LABELING)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_SAMPLE_LINE)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_SAMPLE_LINE_GROUP)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_SECTION)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_SECTIONALDATA_BAND_LABEL_GROUP)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_SECTIONDATA_BAND_LABEL_GROUP)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_SECTIONSEGMENT_BAND_LABEL_GROUP)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_SECTION_CORRIDOR)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_SECTION_CORRIDOR_POINT_LABEL_GROUP)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_SECTION_GRADEBREAK_LABEL_GROUP)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_SECTION_MINOR_OFFSET_LABEL_GROUP)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_SECTION_OFFSET_LABEL_GROUP)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_SECTION_PIPENETWORK)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_SECTION_PRESSUREPIPENETWORK)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_SECTION_PROJECTION)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_SECTION_PROJECTION_LABEL)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_SECTION_SEGMENT_LABEL_GROUP)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_SECTION_VIEW)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_SECTION_VIEW_DEPTH_LABEL)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_SECTION_VIEW_QUANTITY_TAKEOFF_TABLE)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_SHEET)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_SPANNING_PIPE_LABELING)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_SPANNING_PIPE_PROFILE_LABELING)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_STATION_ELEV_LABEL)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_STRUCTURE)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_STRUCTURE_CSV)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_STRUCTURE_LABELING)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_STRUCTURE_PROFILE_LABELING)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_STRUCTURE_SECTION_LABELING)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_SUBASSEMBLY)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_SUPERELEVATION_BAND_LABEL_GROUP)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_SUPERELEVATION_DIAGRAM_VIEW)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_SURFACE_CONTOUR_LABEL_GROUP)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_SURFACE_ELEVATION_LABEL)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_SURFACE_SLOPE_LABEL)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_SURVEY_FIGURE_LABEL_GROUP)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_SVFIGURE)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_SVFIGURE_CSV)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_SVFIGURE_PROFILE)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_SVFIGURE_SECTION)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_SVFIGURE_SEGMENT_LABEL)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_SVNETWORK)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_TANGENT_INTERSECTION_TABLE)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_TIN_SURFACE)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_TIN_SURFACE_CSV)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_VALIGNMENT_CRESTCURVE_LABEL_GROUP)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_VALIGNMENT_CSV)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_VALIGNMENT_HAGEOMPT_LABEL_GROUP)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_VALIGNMENT_LINE_LABEL_GROUP)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_VALIGNMENT_MINOR_STATION_LABEL_GROUP)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_VALIGNMENT_PVI_LABEL_GROUP)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_VALIGNMENT_SAGCURVE_LABEL_GROUP)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_VALIGNMENT_STATION_LABEL_GROUP)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_VERTICALGEOMETRY_BAND_LABEL_GROUP)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_VIEWFRAME_LABELING)
  //DWG_GETALL_OBJECT (AECC_DISP_REP_VIEW_FRAME)
  //DWG_GETALL_OBJECT (AECC_FEATURELINE_STYLE)
  //DWG_GETALL_OBJECT (AECC_FEATURE_STYLE)
  //DWG_GETALL_OBJECT (AECC_FITTING_STYLE)
  //DWG_GETALL_OBJECT (AECC_FORMAT_MANAGER_OBJECT)
  //DWG_GETALL_OBJECT (AECC_GRADEVIEW)
  //DWG_GETALL_OBJECT (AECC_GRADING_CRITERIA)
  //DWG_GETALL_OBJECT (AECC_GRADING_CRITERIA_SET)
  //DWG_GETALL_OBJECT (AECC_GRADING_GROUP)
  //DWG_GETALL_OBJECT (AECC_GRADING_STYLE)
  //DWG_GETALL_OBJECT (AECC_IMPORT_STORM_SEWER_DEFAULTS)
  //DWG_GETALL_OBJECT (AECC_INTERFERENCE_STYLE)
  //DWG_GETALL_OBJECT (AECC_INTERSECTION_STYLE)
  //DWG_GETALL_OBJECT (AECC_LABEL_COLLECTOR_STYLE)
  //DWG_GETALL_OBJECT (AECC_LABEL_NODE)
  //DWG_GETALL_OBJECT (AECC_LABEL_RADIAL_LINE_STYLE)
  //DWG_GETALL_OBJECT (AECC_LABEL_TEXT_ITERATOR_CURVE_OR_SPIRAL_STYLE)
  //DWG_GETALL_OBJECT (AECC_LABEL_TEXT_ITERATOR_STYLE)
  //DWG_GETALL_OBJECT (AECC_LABEL_TEXT_STYLE)
  //DWG_GETALL_OBJECT (AECC_LABEL_VECTOR_ARROW_STYLE)
  //DWG_GETALL_OBJECT (AECC_LEGEND_TABLE_STYLE)
  //DWG_GETALL_OBJECT (AECC_MASS_HAUL_LINE_STYLE)
  //DWG_GETALL_OBJECT (AECC_MASS_HAUL_VIEW_STYLE)
  //DWG_GETALL_OBJECT (AECC_MATCHLINE_STYLE)
  //DWG_GETALL_OBJECT (AECC_MATERIAL_STYLE)
  //DWG_GETALL_OBJECT (AECC_NETWORK_PART_CATALOG_DEF_NODE)
  //DWG_GETALL_OBJECT (AECC_NETWORK_PART_FAMILY_ITEM)
  //DWG_GETALL_OBJECT (AECC_NETWORK_PART_LIST)
  //DWG_GETALL_OBJECT (AECC_NETWORK_RULE)
  //DWG_GETALL_OBJECT (AECC_PARCEL_NODE)
  //DWG_GETALL_OBJECT (AECC_PARCEL_STYLE)
  //DWG_GETALL_OBJECT (AECC_PART_SIZE_FILTER)
  //DWG_GETALL_OBJECT (AECC_PIPE_RULES)
  //DWG_GETALL_OBJECT (AECC_PIPE_STYLE)
  //DWG_GETALL_OBJECT (AECC_PIPE_STYLE_EXTENSION)
  //DWG_GETALL_OBJECT (AECC_POINTCLOUD_STYLE)
  //DWG_GETALL_OBJECT (AECC_POINTVIEW)
  //DWG_GETALL_OBJECT (AECC_POINT_STYLE)
  //DWG_GETALL_OBJECT (AECC_PRESSURE_PART_LIST)
  //DWG_GETALL_OBJECT (AECC_PRESSURE_PIPE_STYLE)
  //DWG_GETALL_OBJECT (AECC_PROFILESECTIONENTITY_STYLE)
  //DWG_GETALL_OBJECT (AECC_PROFILE_DESIGN_CHECK_SET)
  //DWG_GETALL_OBJECT (AECC_PROFILE_LABEL_SET)
  //DWG_GETALL_OBJECT (AECC_PROFILE_STYLE)
  //DWG_GETALL_OBJECT (AECC_PROFILE_VIEW_BAND_STYLE_SET)
  //DWG_GETALL_OBJECT (AECC_PROFILE_VIEW_DATA_BAND_STYLE)
  //DWG_GETALL_OBJECT (AECC_PROFILE_VIEW_HORIZONTAL_GEOMETRY_BAND_STYLE)
  //DWG_GETALL_OBJECT (AECC_PROFILE_VIEW_PIPE_NETWORK_BAND_STYLE)
  //DWG_GETALL_OBJECT (AECC_PROFILE_VIEW_SECTIONAL_DATA_BAND_STYLE)
  //DWG_GETALL_OBJECT (AECC_PROFILE_VIEW_STYLE)
  //DWG_GETALL_OBJECT (AECC_PROFILE_VIEW_SUPERELEVATION_DIAGRAM_BAND_STYLE)
  //DWG_GETALL_OBJECT (AECC_PROFILE_VIEW_VERTICAL_GEOMETRY_BAND_STYLE)
  //DWG_GETALL_OBJECT (AECC_QUANTITY_TAKEOFF_CRITERIA)
  //DWG_GETALL_OBJECT (AECC_ROADWAYLINK_STYLE)
  //DWG_GETALL_OBJECT (AECC_ROADWAYMARKER_STYLE)
  //DWG_GETALL_OBJECT (AECC_ROADWAYSHAPE_STYLE)
  //DWG_GETALL_OBJECT (AECC_ROADWAY_STYLE_SET)
  //DWG_GETALL_OBJECT (AECC_ROOT_SETTINGS_NODE)
  //DWG_GETALL_OBJECT (AECC_SAMPLE_LINE_GROUP_STYLE)
  //DWG_GETALL_OBJECT (AECC_SAMPLE_LINE_STYLE)
  //DWG_GETALL_OBJECT (AECC_SECTION_LABEL_SET)
  //DWG_GETALL_OBJECT (AECC_SECTION_STYLE)
  //DWG_GETALL_OBJECT (AECC_SECTION_VIEW_BAND_STYLE_SET)
  //DWG_GETALL_OBJECT (AECC_SECTION_VIEW_DATA_BAND_STYLE)
  //DWG_GETALL_OBJECT (AECC_SECTION_VIEW_ROAD_SURFACE_BAND_STYLE)
  //DWG_GETALL_OBJECT (AECC_SECTION_VIEW_STYLE)
  //DWG_GETALL_OBJECT (AECC_SETTINGS_NODE)
  //DWG_GETALL_OBJECT (AECC_SHEET_STYLE)
  //DWG_GETALL_OBJECT (AECC_SLOPE_PATTERN_STYLE)
  //DWG_GETALL_OBJECT (AECC_STATION_FORMAT_STYLE)
  //DWG_GETALL_OBJECT (AECC_STRUCTURE_RULES)
  //DWG_GETALL_OBJECT (AECC_STUCTURE_STYLE)
  //DWG_GETALL_OBJECT (AECC_SUPERELEVATION_DIAGRAM_VIEW_STYLE)
  //DWG_GETALL_OBJECT (AECC_SURFACE_STYLE)
  //DWG_GETALL_OBJECT (AECC_SVFIGURE_STYLE)
  //DWG_GETALL_OBJECT (AECC_SVNETWORK_STYLE)
  //DWG_GETALL_OBJECT (AECC_TABLE_STYLE)
  //DWG_GETALL_OBJECT (AECC_TAG_MANAGER)
  //DWG_GETALL_OBJECT (AECC_TREE_NODE)
  //DWG_GETALL_OBJECT (AECC_USER_DEFINED_ATTRIBUTE_CLASSIFICATION)
  //DWG_GETALL_OBJECT (AECC_VALIGNMENT_STYLE_EXTENSION)
  //DWG_GETALL_OBJECT (AECC_VIEW_FRAME_STYLE)
  //DWG_GETALL_OBJECT (AECS_DISP_PROPS_MEMBER)
  //DWG_GETALL_OBJECT (AECS_DISP_PROPS_MEMBER_LOGICAL)
  //DWG_GETALL_OBJECT (AECS_DISP_PROPS_MEMBER_PLAN)
  //DWG_GETALL_OBJECT (AECS_DISP_PROPS_MEMBER_PLAN_SKETCH)
  //DWG_GETALL_OBJECT (AECS_DISP_PROPS_MEMBER_PROJECTED)
  //DWG_GETALL_OBJECT (AECS_DISP_REP_MEMBER_ELEVATION_DESIGN)
  //DWG_GETALL_OBJECT (AECS_DISP_REP_MEMBER_ELEVATION_DETAIL)
  //DWG_GETALL_OBJECT (AECS_DISP_REP_MEMBER_LOGICAL)
  //DWG_GETALL_OBJECT (AECS_DISP_REP_MEMBER_MODEL_DESIGN)
  //DWG_GETALL_OBJECT (AECS_DISP_REP_MEMBER_MODEL_DETAIL)
  //DWG_GETALL_OBJECT (AECS_DISP_REP_MEMBER_PLAN_DESIGN)
  //DWG_GETALL_OBJECT (AECS_DISP_REP_MEMBER_PLAN_DETAIL)
  //DWG_GETALL_OBJECT (AECS_DISP_REP_MEMBER_PLAN_SKETCH)
  //DWG_GETALL_OBJECT (AECS_MEMBER_NODE_SHAPE)
  //DWG_GETALL_OBJECT (AECS_MEMBER_STYLE)
  //DWG_GETALL_OBJECT (AEC_2DSECTION_STYLE)
  //DWG_GETALL_OBJECT (AEC_AECDBDISPREPBDGELEVLINEPLAN100)
  //DWG_GETALL_OBJECT (AEC_AECDBDISPREPBDGELEVLINEPLAN50)
  //DWG_GETALL_OBJECT (AEC_AECDBDISPREPBDGSECTIONLINEPLAN100)
  //DWG_GETALL_OBJECT (AEC_AECDBDISPREPBDGSECTIONLINEPLAN50)
  //DWG_GETALL_OBJECT (AEC_AECDBDISPREPCEILINGGRIDPLAN100)
  //DWG_GETALL_OBJECT (AEC_AECDBDISPREPCEILINGGRIDPLAN50)
  //DWG_GETALL_OBJECT (AEC_AECDBDISPREPCOLUMNGRIDPLAN100)
  //DWG_GETALL_OBJECT (AEC_AECDBDISPREPCOLUMNGRIDPLAN50)
  //DWG_GETALL_OBJECT (AEC_AECDBDISPREPCURTAINWALLLAYOUTPLAN100)
  //DWG_GETALL_OBJECT (AEC_AECDBDISPREPCURTAINWALLLAYOUTPLAN50)
  //DWG_GETALL_OBJECT (AEC_AECDBDISPREPCURTAINWALLUNITPLAN100)
  //DWG_GETALL_OBJECT (AEC_AECDBDISPREPCURTAINWALLUNITPLAN50)
  //DWG_GETALL_OBJECT (AEC_AECDBDISPREPMVBLOCKREFPLAN100)
  //DWG_GETALL_OBJECT (AEC_AECDBDISPREPMVBLOCKREFPLAN50)
  //DWG_GETALL_OBJECT (AEC_AECDBDISPREPROOFPLAN100)
  //DWG_GETALL_OBJECT (AEC_AECDBDISPREPROOFPLAN50)
  //DWG_GETALL_OBJECT (AEC_AECDBDISPREPROOFSLABPLAN100)
  //DWG_GETALL_OBJECT (AEC_AECDBDISPREPROOFSLABPLAN50)
  //DWG_GETALL_OBJECT (AEC_AECDBDISPREPSLABPLAN100)
  //DWG_GETALL_OBJECT (AEC_AECDBDISPREPSLABPLAN50)
  //DWG_GETALL_OBJECT (AEC_AECDBDISPREPSPACEPLAN100)
  //DWG_GETALL_OBJECT (AEC_AECDBDISPREPSPACEPLAN50)
  //DWG_GETALL_OBJECT (AEC_AECDBDISPREPWALLPLAN100)
  //DWG_GETALL_OBJECT (AEC_AECDBDISPREPWALLPLAN50)
  //DWG_GETALL_OBJECT (AEC_AECDBDISPREPWINDOWASSEMBLYPLAN100)
  //DWG_GETALL_OBJECT (AEC_AECDBDISPREPWINDOWASSEMBLYPLAN50)
  //DWG_GETALL_OBJECT (AEC_AECDBDISPREPZONE100)
  //DWG_GETALL_OBJECT (AEC_AECDBDISPREPZONE50)
  //DWG_GETALL_OBJECT (AEC_AECDBZONEDEF)
  //DWG_GETALL_OBJECT (AEC_AECDBZONESTYLE)
  //DWG_GETALL_OBJECT (AEC_ANCHOR_OPENINGBASE_TO_WALL)
  //DWG_GETALL_OBJECT (AEC_CLASSIFICATION_DEF)
  //DWG_GETALL_OBJECT (AEC_CLASSIFICATION_SYSTEM_DEF)
  //DWG_GETALL_OBJECT (AEC_CLEANUP_GROUP_DEF)
  //DWG_GETALL_OBJECT (AEC_CURTAIN_WALL_LAYOUT_STYLE)
  //DWG_GETALL_OBJECT (AEC_CURTAIN_WALL_UNIT_STYLE)
  //DWG_GETALL_OBJECT (AEC_CVSECTIONVIEW)
  //DWG_GETALL_OBJECT (AEC_DB_DISP_REP_DIM_GROUP_PLAN)
  //DWG_GETALL_OBJECT (AEC_DB_DISP_REP_DIM_GROUP_PLAN100)
  //DWG_GETALL_OBJECT (AEC_DB_DISP_REP_DIM_GROUP_PLAN50)
  //DWG_GETALL_OBJECT (AEC_DIM_STYLE)
  //DWG_GETALL_OBJECT (AEC_DISPLAYTHEME_STYLE)
  //DWG_GETALL_OBJECT (AEC_DISPREPAECDBDISPREPMASSELEMPLAN100)
  //DWG_GETALL_OBJECT (AEC_DISPREPAECDBDISPREPMASSELEMPLAN50)
  //DWG_GETALL_OBJECT (AEC_DISPREPAECDBDISPREPMASSGROUPPLAN100)
  //DWG_GETALL_OBJECT (AEC_DISPREPAECDBDISPREPMASSGROUPPLAN50)
  //DWG_GETALL_OBJECT (AEC_DISPREPAECDBDISPREPOPENINGPLAN100)
  //DWG_GETALL_OBJECT (AEC_DISPREPAECDBDISPREPOPENINGPLAN50)
  //DWG_GETALL_OBJECT (AEC_DISPREPAECDBDISPREPOPENINGPLANREFLECTED)
  //DWG_GETALL_OBJECT (AEC_DISPREPAECDBDISPREPOPENINGSILLPLAN)
  //DWG_GETALL_OBJECT (AEC_DISPROPSMASSELEMPLANCOMMON)
  //DWG_GETALL_OBJECT (AEC_DISPROPSMASSGROUPPLANCOMMON)
  //DWG_GETALL_OBJECT (AEC_DISPROPSOPENINGPLANCOMMON)
  //DWG_GETALL_OBJECT (AEC_DISPROPSOPENINGPLANCOMMONHATCHED)
  //DWG_GETALL_OBJECT (AEC_DISPROPSOPENINGSILLPLAN)
  //DWG_GETALL_OBJECT (AEC_DISP_PROPS_2D_SECTION)
  //DWG_GETALL_OBJECT (AEC_DISP_PROPS_CLIP_VOLUME)
  //DWG_GETALL_OBJECT (AEC_DISP_PROPS_CLIP_VOLUME_RESULT)
  //DWG_GETALL_OBJECT (AEC_DISP_PROPS_DIM)
  //DWG_GETALL_OBJECT (AEC_DISP_PROPS_DISPLAYTHEME)
  //DWG_GETALL_OBJECT (AEC_DISP_PROPS_DOOR)
  //DWG_GETALL_OBJECT (AEC_DISP_PROPS_DOOR_NOMINAL)
  //DWG_GETALL_OBJECT (AEC_DISP_PROPS_DOOR_PLAN_100)
  //DWG_GETALL_OBJECT (AEC_DISP_PROPS_DOOR_PLAN_50)
  //DWG_GETALL_OBJECT (AEC_DISP_PROPS_DOOR_THRESHOLD_PLAN)
  //DWG_GETALL_OBJECT (AEC_DISP_PROPS_DOOR_THRESHOLD_SYMBOL_PLAN)
  //DWG_GETALL_OBJECT (AEC_DISP_PROPS_EDITINPLACEPROFILE_MODEL)
  //DWG_GETALL_OBJECT (AEC_DISP_PROPS_ENT)
  //DWG_GETALL_OBJECT (AEC_DISP_PROPS_ENT_REF)
  //DWG_GETALL_OBJECT (AEC_DISP_PROPS_GRID_ASSEMBLY_MODEL)
  //DWG_GETALL_OBJECT (AEC_DISP_PROPS_GRID_ASSEMBLY_PLAN)
  //DWG_GETALL_OBJECT (AEC_DISP_PROPS_LAYOUT_CURVE)
  //DWG_GETALL_OBJECT (AEC_DISP_PROPS_LAYOUT_GRID2D)
  //DWG_GETALL_OBJECT (AEC_DISP_PROPS_LAYOUT_GRID3D)
  //DWG_GETALL_OBJECT (AEC_DISP_PROPS_MASKBLOCK)
  //DWG_GETALL_OBJECT (AEC_DISP_PROPS_MASS_ELEM_MODEL)
  //DWG_GETALL_OBJECT (AEC_DISP_PROPS_MASS_GROUP)
  //DWG_GETALL_OBJECT (AEC_DISP_PROPS_MATERIAL)
  //DWG_GETALL_OBJECT (AEC_DISP_PROPS_OPENING)
  //DWG_GETALL_OBJECT (AEC_DISP_PROPS_POLYGON_MODEL)
  //DWG_GETALL_OBJECT (AEC_DISP_PROPS_POLYGON_TRUECOLOUR)
  //DWG_GETALL_OBJECT (AEC_DISP_PROPS_RAILING_MODEL)
  //DWG_GETALL_OBJECT (AEC_DISP_PROPS_RAILING_PLAN)
  //DWG_GETALL_OBJECT (AEC_DISP_PROPS_ROOF)
  //DWG_GETALL_OBJECT (AEC_DISP_PROPS_ROOFSLAB)
  //DWG_GETALL_OBJECT (AEC_DISP_PROPS_ROOFSLAB_PLAN)
  //DWG_GETALL_OBJECT (AEC_DISP_PROPS_SCHEDULE_TABLE)
  //DWG_GETALL_OBJECT (AEC_DISP_PROPS_SLAB)
  //DWG_GETALL_OBJECT (AEC_DISP_PROPS_SLAB_PLAN)
  //DWG_GETALL_OBJECT (AEC_DISP_PROPS_SLICE)
  //DWG_GETALL_OBJECT (AEC_DISP_PROPS_SPACE_DECOMPOSED)
  //DWG_GETALL_OBJECT (AEC_DISP_PROPS_SPACE_MODEL)
  //DWG_GETALL_OBJECT (AEC_DISP_PROPS_SPACE_PLAN)
  //DWG_GETALL_OBJECT (AEC_DISP_PROPS_STAIR_MODEL)
  //DWG_GETALL_OBJECT (AEC_DISP_PROPS_STAIR_PLAN)
  //DWG_GETALL_OBJECT (AEC_DISP_PROPS_STAIR_PLAN_OVERLAPPING)
  //DWG_GETALL_OBJECT (AEC_DISP_PROPS_WALL_GRAPH)
  //DWG_GETALL_OBJECT (AEC_DISP_PROPS_WALL_MODEL)
  //DWG_GETALL_OBJECT (AEC_DISP_PROPS_WALL_PLAN)
  //DWG_GETALL_OBJECT (AEC_DISP_PROPS_WALL_SCHEM)
  //DWG_GETALL_OBJECT (AEC_DISP_PROPS_WINDOW)
  //DWG_GETALL_OBJECT (AEC_DISP_PROPS_WINDOW_ASSEMBLY_SILL_PLAN)
  //DWG_GETALL_OBJECT (AEC_DISP_PROPS_WINDOW_NOMINAL)
  //DWG_GETALL_OBJECT (AEC_DISP_PROPS_WINDOW_PLAN_100)
  //DWG_GETALL_OBJECT (AEC_DISP_PROPS_WINDOW_PLAN_50)
  //DWG_GETALL_OBJECT (AEC_DISP_PROPS_WINDOW_SILL_PLAN)
  //DWG_GETALL_OBJECT (AEC_DISP_PROPS_ZONE)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_2D_SECTION)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_ANCHOR)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_ANCHOR_BUB_TO_GRID)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_ANCHOR_BUB_TO_GRID_MODEL)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_ANCHOR_BUB_TO_GRID_RCP)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_ANCHOR_ENT_TO_NODE)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_ANCHOR_EXT_TAG_TO_ENT)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_ANCHOR_TAG_TO_ENT)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_BDG_ELEVLINE_MODEL)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_BDG_ELEVLINE_PLAN)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_BDG_ELEVLINE_RCP)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_BDG_SECTIONLINE_MODEL)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_BDG_SECTIONLINE_PLAN)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_BDG_SECTIONLINE_RCP)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_BDG_SECTION_MODEL)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_BDG_SECTION_SUBDIV)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_CEILING_GRID)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_CEILING_GRID_MODEL)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_CEILING_GRID_RCP)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_CLIP_VOLUME_MODEL)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_CLIP_VOLUME_PLAN)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_CLIP_VOLUME_RESULT)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_CLIP_VOLUME_RESULT_SUBDIV)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_COLUMN_GRID)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_COLUMN_GRID_MODEL)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_COLUMN_GRID_RCP)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_COL_BLOCK)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_COL_CIRCARC2D)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_COL_CONCOINCIDENT)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_COL_CONCONCENTRIC)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_COL_CONEQUALDISTANCE)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_COL_CONMIDPOINT)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_COL_CONNECTOR)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_COL_CONNORMAL)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_COL_CONPARALLEL)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_COL_CONPERPENDICULAR)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_COL_CONSYMMETRIC)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_COL_CONTANGENT)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_COL_DIMANGLE)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_COL_DIMDIAMETER)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_COL_DIMDISTANCE)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_COL_DIMLENGTH)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_COL_DIMMAJORRADIUS)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_COL_DIMMINORRADIUS)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_COL_ELLIPARC2D)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_COL_LAYOUTDATA)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_COL_LINE2D)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_COL_MODIFIER_ADD)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_COL_MODIFIER_CUTPLANE)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_COL_MODIFIER_EXTRUSION)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_COL_MODIFIER_GROUP)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_COL_MODIFIER_LOFT)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_COL_MODIFIER_PATH)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_COL_MODIFIER_REVOLVE)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_COL_MODIFIER_SUBTRACT)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_COL_MODIFIER_TRANSITION)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_COL_POINT2D)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_COL_PROFILE)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_COL_WORKPLANE)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_COL_WORKPLANE_REF)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_CONFIG)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_CURTAIN_WALL_LAYOUT_MODEL)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_CURTAIN_WALL_LAYOUT_PLAN)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_CURTAIN_WALL_UNIT_MODEL)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_CURTAIN_WALL_UNIT_PLAN)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_DCM_DIMRADIUS)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_DISPLAYTHEME)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_DOOR_ELEV)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_DOOR_MODEL)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_DOOR_NOMINAL)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_DOOR_PLAN)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_DOOR_PLAN_50)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_DOOR_PLAN_HEKTO)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_DOOR_RCP)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_DOOR_THRESHOLD_PLAN)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_DOOR_THRESHOLD_SYMBOL_PLAN)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_EDITINPLACEPROFILE)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_ENT_REF)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_LAYOUT_CURVE)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_LAYOUT_GRID2D)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_LAYOUT_GRID3D)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_MASKBLOCK_REF)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_MASKBLOCK_REF_RCP)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_MASS_ELEM_MODEL)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_MASS_ELEM_RCP)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_MASS_ELEM_SCHEM)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_MASS_GROUP_MODEL)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_MASS_GROUP_PLAN)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_MASS_GROUP_RCP)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_MATERIAL)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_MVBLOCK_REF)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_MVBLOCK_REF_MODEL)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_MVBLOCK_REF_RCP)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_OPENING)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_OPENING_MODEL)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_POLYGON_MODEL)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_POLYGON_TRUECOLOUR)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_RAILING_MODEL)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_RAILING_PLAN)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_RAILING_PLAN_100)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_RAILING_PLAN_50)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_ROOFSLAB_MODEL)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_ROOFSLAB_PLAN)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_ROOF_MODEL)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_ROOF_PLAN)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_ROOF_RCP)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_SCHEDULE_TABLE)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_SET)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_SLAB_MODEL)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_SLAB_PLAN)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_SLICE)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_SPACE_DECOMPOSED)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_SPACE_MODEL)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_SPACE_PLAN)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_SPACE_RCP)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_SPACE_VOLUME)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_STAIR_MODEL)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_STAIR_PLAN)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_STAIR_PLAN_100)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_STAIR_PLAN_50)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_STAIR_PLAN_OVERLAPPING)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_STAIR_RCP)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_WALL_GRAPH)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_WALL_MODEL)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_WALL_PLAN)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_WALL_RCP)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_WALL_SCHEM)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_WINDOWASSEMBLY_SILL_PLAN)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_WINDOW_ASSEMBLY_MODEL)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_WINDOW_ASSEMBLY_PLAN)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_WINDOW_ELEV)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_WINDOW_MODEL)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_WINDOW_NOMINAL)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_WINDOW_PLAN)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_WINDOW_PLAN_100)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_WINDOW_PLAN_50)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_WINDOW_RCP)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_WINDOW_SILL_PLAN)
  //DWG_GETALL_OBJECT (AEC_DISP_REP_ZONE)
  //DWG_GETALL_OBJECT (AEC_DISP_ROPS_RAILING_PLAN_100)
  //DWG_GETALL_OBJECT (AEC_DISP_ROPS_RAILING_PLAN_50)
  //DWG_GETALL_OBJECT (AEC_DISP_ROPS_STAIR_PLAN_100)
  //DWG_GETALL_OBJECT (AEC_DISP_ROPS_STAIR_PLAN_50)
  //DWG_GETALL_OBJECT (AEC_DOOR_STYLE)
  //DWG_GETALL_OBJECT (AEC_ENDCAP_STYLE)
  //DWG_GETALL_OBJECT (AEC_FRAME_DEF)
  //DWG_GETALL_OBJECT (AEC_LAYERKEY_STYLE)
  //DWG_GETALL_OBJECT (AEC_LIST_DEF)
  //DWG_GETALL_OBJECT (AEC_MASKBLOCK_DEF)
  //DWG_GETALL_OBJECT (AEC_MASS_ELEM_STYLE)
  //DWG_GETALL_OBJECT (AEC_MATERIAL_DEF)
  //DWG_GETALL_OBJECT (AEC_MVBLOCK_DEF)
  //DWG_GETALL_OBJECT (AEC_MVBLOCK_REF)
  //DWG_GETALL_OBJECT (AEC_NOTIFICATION_TRACKER)
  //DWG_GETALL_OBJECT (AEC_POLYGON)
  //DWG_GETALL_OBJECT (AEC_POLYGON_STYLE)
  //DWG_GETALL_OBJECT (AEC_PROPERTY_SET_DEF)
  //DWG_GETALL_OBJECT (AEC_RAILING_STYLE)
  //DWG_GETALL_OBJECT (AEC_REFEDIT_STATUS_TRACKER)
  //DWG_GETALL_OBJECT (AEC_ROOFSLABEDGE_STYLE)
  //DWG_GETALL_OBJECT (AEC_ROOFSLAB_STYLE)
  //DWG_GETALL_OBJECT (AEC_SCHEDULE_DATA_FORMAT)
  //DWG_GETALL_OBJECT (AEC_SLABEDGE_STYLE)
  //DWG_GETALL_OBJECT (AEC_SLAB_STYLE)
  //DWG_GETALL_OBJECT (AEC_SPACE_STYLES)
  //DWG_GETALL_OBJECT (AEC_STAIR_STYLE)
  //DWG_GETALL_OBJECT (AEC_STAIR_WINDER_STYLE)
  //DWG_GETALL_OBJECT (AEC_STAIR_WINDER_TYPE_BALANCED)
  //DWG_GETALL_OBJECT (AEC_STAIR_WINDER_TYPE_MANUAL)
  //DWG_GETALL_OBJECT (AEC_STAIR_WINDER_TYPE_SINGLE_POINT)
  //DWG_GETALL_OBJECT (AEC_VARS_AECBBLDSRV)
  //DWG_GETALL_OBJECT (AEC_VARS_ARCHBASE)
  //DWG_GETALL_OBJECT (AEC_VARS_DWG_SETUP)
  //DWG_GETALL_OBJECT (AEC_VARS_MUNICH)
  //DWG_GETALL_OBJECT (AEC_VARS_STRUCTUREBASE)
  //DWG_GETALL_OBJECT (AEC_WALLMOD_STYLE)
  //DWG_GETALL_OBJECT (AEC_WALL_STYLE)
  //DWG_GETALL_OBJECT (AEC_WINDOW_ASSEMBLY_STYLE)
  //DWG_GETALL_OBJECT (AEC_WINDOW_STYLE)
  //DWG_GETALL_OBJECT (ALIGNMENTGRIPENTITY)
  //DWG_GETALL_OBJECT (AMCONTEXTMGR)
  //DWG_GETALL_OBJECT (AMDTADMENUSTATE)
  //DWG_GETALL_OBJECT (AMDTAMMENUSTATE)
  //DWG_GETALL_OBJECT (AMDTBROWSERDBTAB)
  //DWG_GETALL_OBJECT (AMDTDMMENUSTATE)
  //DWG_GETALL_OBJECT (AMDTEDGESTANDARDDIN)
  //DWG_GETALL_OBJECT (AMDTEDGESTANDARDDIN13715)
  //DWG_GETALL_OBJECT (AMDTEDGESTANDARDISO)
  //DWG_GETALL_OBJECT (AMDTEDGESTANDARDISO13715)
  //DWG_GETALL_OBJECT (AMDTFORMULAUPDATEDISPATCHER)
  //DWG_GETALL_OBJECT (AMDTINTERNALREACTOR)
  //DWG_GETALL_OBJECT (AMDTMCOMMENUSTATE)
  //DWG_GETALL_OBJECT (AMDTMENUSTATEMGR)
  //DWG_GETALL_OBJECT (AMDTNOTE)
  //DWG_GETALL_OBJECT (AMDTNOTETEMPLATEDB)
  //DWG_GETALL_OBJECT (AMDTSECTIONSYM)
  //DWG_GETALL_OBJECT (AMDTSECTIONSYMLABEL)
  //DWG_GETALL_OBJECT (AMDTSYSATTR)
  //DWG_GETALL_OBJECT (AMGOBJPROPCFG)
  //DWG_GETALL_OBJECT (AMGSETTINGSOBJ)
  //DWG_GETALL_OBJECT (AMIMASTER)
  //DWG_GETALL_OBJECT (AM_DRAWING_MGR)
  //DWG_GETALL_OBJECT (AM_DWGMGR_NAME)
  //DWG_GETALL_OBJECT (AM_DWG_DOCUMENT)
  //DWG_GETALL_OBJECT (AM_DWG_SHEET)
  //DWG_GETALL_OBJECT (AM_VIEWDIMPARMAP)
  //DWG_GETALL_OBJECT (BINRECORD)
  //DWG_GETALL_OBJECT (CAMSCATALOGAPPOBJECT)
  //DWG_GETALL_OBJECT (CAMSSTRUCTBTNSTATE)
  //DWG_GETALL_OBJECT (CATALOGSTATE)
  //DWG_GETALL_OBJECT (CBROWSERAPPOBJECT)
  //DWG_GETALL_OBJECT (DEPMGR)
  //DWG_GETALL_OBJECT (DMBASEELEMENT)
  //DWG_GETALL_OBJECT (DMDEFAULTSTYLE)
  //DWG_GETALL_OBJECT (DMLEGEND)
  //DWG_GETALL_OBJECT (DMMAP)
  //DWG_GETALL_OBJECT (DMMAPMANAGER)
  //DWG_GETALL_OBJECT (DMSTYLECATEGORY)
  //DWG_GETALL_OBJECT (DMSTYLELIBRARY)
  //DWG_GETALL_OBJECT (DMSTYLEREFERENCE)
  //DWG_GETALL_OBJECT (DMSTYLIZEDENTITIESTABLE)
  //DWG_GETALL_OBJECT (DMSURROGATESTYLESETS)
  //DWG_GETALL_OBJECT (DM_PLACEHOLDER)
  //DWG_GETALL_OBJECT (EXACTERMXREFMAP)
  //DWG_GETALL_OBJECT (EXACXREFPANELOBJECT)
  //DWG_GETALL_OBJECT (EXPO_NOTIFYBLOCK)
  //DWG_GETALL_OBJECT (EXPO_NOTIFYHALL)
  //DWG_GETALL_OBJECT (EXPO_NOTIFYPILLAR)
  //DWG_GETALL_OBJECT (EXPO_NOTIFYSTAND)
  //DWG_GETALL_OBJECT (EXPO_NOTIFYSTANDNOPOLY)
  //DWG_GETALL_OBJECT (FLIPACTIONENTITY)
  //DWG_GETALL_OBJECT (GSMANAGER)
  //DWG_GETALL_OBJECT (IRD_DSC_DICT)
  //DWG_GETALL_OBJECT (IRD_DSC_RECORD)
  //DWG_GETALL_OBJECT (IRD_OBJ_RECORD)
  //DWG_GETALL_OBJECT (MAPFSMRVOBJECT)
  //DWG_GETALL_OBJECT (MAPGWSUNDOOBJECT)
  //DWG_GETALL_OBJECT (MAPIAMMOUDLE)
  //DWG_GETALL_OBJECT (MAPMETADATAOBJECT)
  //DWG_GETALL_OBJECT (MAPRESOURCEMANAGEROBJECT)
  //DWG_GETALL_OBJECT (MOVEACTIONENTITY)
  //DWG_GETALL_OBJECT (McDbContainer2)
  //DWG_GETALL_OBJECT (McDbMarker)
  //DWG_GETALL_OBJECT (NAMEDAPPL)
  //DWG_GETALL_OBJECT (NEWSTDPARTPARLIST)
  //DWG_GETALL_OBJECT (NPOCOLLECTION)
  //DWG_GETALL_OBJECT (OBJCLONER)
  //DWG_GETALL_OBJECT (PARAMMGR)
  //DWG_GETALL_OBJECT (PARAMSCOPE)
  //DWG_GETALL_OBJECT (PILLAR)
  //DWG_GETALL_OBJECT (RAPIDRTRENDERENVIRONMENT)
  //DWG_GETALL_OBJECT (ROTATEACTIONENTITY)
  //DWG_GETALL_OBJECT (SCALEACTIONENTITY)
  //DWG_GETALL_OBJECT (STDPART2D)
  //DWG_GETALL_OBJECT (STRETCHACTIONENTITY)
  //DWG_GETALL_OBJECT (TCH_ARROW)
  //DWG_GETALL_OBJECT (TCH_AXIS_LABEL)
  //DWG_GETALL_OBJECT (TCH_BLOCK_INSERT)
  //DWG_GETALL_OBJECT (TCH_COLUMN)
  //DWG_GETALL_OBJECT (TCH_DBCONFIG)
  //DWG_GETALL_OBJECT (TCH_DIMENSION2)
  //DWG_GETALL_OBJECT (TCH_DRAWINGINDEX)
  //DWG_GETALL_OBJECT (TCH_HANDRAIL)
  //DWG_GETALL_OBJECT (TCH_LINESTAIR)
  //DWG_GETALL_OBJECT (TCH_OPENING)
  //DWG_GETALL_OBJECT (TCH_RECTSTAIR)
  //DWG_GETALL_OBJECT (TCH_SLAB)
  //DWG_GETALL_OBJECT (TCH_SPACE)
  //DWG_GETALL_OBJECT (TCH_TEXT)
  //DWG_GETALL_OBJECT (TCH_WALL)
  //DWG_GETALL_OBJECT (TGrupoPuntos)
  //DWG_GETALL_OBJECT (VAACIMAGEINVENTORY)
  //DWG_GETALL_OBJECT (VAACXREFPANELOBJECT)
  //DWG_GETALL_OBJECT (XREFPANELOBJECT)
#endif

// clang-format: on
/*******************************************************************
 *     Functions created from macro to cast dwg_object to entity     *
 *                 Usage :- dwg_object_to_ENTITY(),                  *
 *                where ENTITY can be LINE or CIRCLE                 *
 ********************************************************************/

/**
 * \fn Dwg_Entity_ENTITY *dwg_object_to_ENTITY(Dwg_Object *obj)
 * cast a Dwg_Object to Entity
 */
// clang-format: off
/* fixed <500 */
CAST_DWG_OBJECT_TO_ENTITY (_3DFACE)
CAST_DWG_OBJECT_TO_ENTITY (_3DSOLID)
CAST_DWG_OBJECT_TO_ENTITY (ARC)
CAST_DWG_OBJECT_TO_ENTITY (ATTDEF)
CAST_DWG_OBJECT_TO_ENTITY (ATTRIB)
CAST_DWG_OBJECT_TO_ENTITY (BLOCK)
CAST_DWG_OBJECT_TO_ENTITY (BODY)
CAST_DWG_OBJECT_TO_ENTITY (CIRCLE)
CAST_DWG_OBJECT_TO_ENTITY (DIMENSION_ALIGNED)
CAST_DWG_OBJECT_TO_ENTITY (DIMENSION_ANG2LN)
CAST_DWG_OBJECT_TO_ENTITY (DIMENSION_ANG3PT)
CAST_DWG_OBJECT_TO_ENTITY (DIMENSION_DIAMETER)
CAST_DWG_OBJECT_TO_ENTITY (DIMENSION_LINEAR)
CAST_DWG_OBJECT_TO_ENTITY (DIMENSION_ORDINATE)
CAST_DWG_OBJECT_TO_ENTITY (DIMENSION_RADIUS)
CAST_DWG_OBJECT_TO_ENTITY (ELLIPSE)
CAST_DWG_OBJECT_TO_ENTITY (ENDBLK)
CAST_DWG_OBJECT_TO_ENTITY (INSERT)
CAST_DWG_OBJECT_TO_ENTITY (LEADER)
CAST_DWG_OBJECT_TO_ENTITY (LINE)
CAST_DWG_OBJECT_TO_ENTITY (LOAD)
CAST_DWG_OBJECT_TO_ENTITY (MINSERT)
CAST_DWG_OBJECT_TO_ENTITY (MLINE)
CAST_DWG_OBJECT_TO_ENTITY (MTEXT)
CAST_DWG_OBJECT_TO_ENTITY (OLEFRAME)
CAST_DWG_OBJECT_TO_ENTITY (POINT)
CAST_DWG_OBJECT_TO_ENTITY (POLYLINE_2D)
CAST_DWG_OBJECT_TO_ENTITY (POLYLINE_3D)
CAST_DWG_OBJECT_TO_ENTITY (POLYLINE_MESH)
CAST_DWG_OBJECT_TO_ENTITY (POLYLINE_PFACE)
CAST_DWG_OBJECT_TO_ENTITY (PROXY_ENTITY)
CAST_DWG_OBJECT_TO_ENTITY (RAY)
CAST_DWG_OBJECT_TO_ENTITY (REGION)
CAST_DWG_OBJECT_TO_ENTITY (SEQEND)
CAST_DWG_OBJECT_TO_ENTITY (SHAPE)
CAST_DWG_OBJECT_TO_ENTITY (SOLID)
CAST_DWG_OBJECT_TO_ENTITY (SPLINE)
CAST_DWG_OBJECT_TO_ENTITY (TEXT)
CAST_DWG_OBJECT_TO_ENTITY (TOLERANCE)
CAST_DWG_OBJECT_TO_ENTITY (TRACE)
CAST_DWG_OBJECT_TO_ENTITY (UNKNOWN_ENT)
CAST_DWG_OBJECT_TO_ENTITY (VERTEX_2D)
CAST_DWG_OBJECT_TO_ENTITY (VERTEX_3D)
CAST_DWG_OBJECT_TO_ENTITY (VERTEX_MESH)
CAST_DWG_OBJECT_TO_ENTITY (VERTEX_PFACE)
CAST_DWG_OBJECT_TO_ENTITY (VERTEX_PFACE_FACE)
CAST_DWG_OBJECT_TO_ENTITY (VIEWPORT)
CAST_DWG_OBJECT_TO_ENTITY (XLINE)
/* untyped > 500 */
CAST_DWG_OBJECT_TO_ENTITY_BYNAME (_3DLINE)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME (CAMERA)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME (DGNUNDERLAY)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME (DWFUNDERLAY)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME (ENDREP)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME (HATCH)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME (IMAGE)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME (JUMP)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME (LIGHT)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME (LWPOLYLINE)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME (MESH)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME (MULTILEADER)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME (OLE2FRAME)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME (PDFUNDERLAY)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME (REPEAT)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME (SECTIONOBJECT)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME (WIPEOUT)
/* unstable */
CAST_DWG_OBJECT_TO_ENTITY_BYNAME (ARC_DIMENSION)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME (HELIX)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME (LARGE_RADIAL_DIMENSION)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME (LAYOUTPRINTCONFIG)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME (PLANESURFACE)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME (POINTCLOUD)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME (POINTCLOUDEX)
#  ifdef DEBUG_CLASSES
CAST_DWG_OBJECT_TO_ENTITY_BYNAME (ALIGNMENTPARAMETERENTITY)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME (ARCALIGNEDTEXT)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME (BASEPOINTPARAMETERENTITY)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME (EXTRUDEDSURFACE)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME (FLIPGRIPENTITY)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME (FLIPPARAMETERENTITY)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME (GEOPOSITIONMARKER)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME (LINEARGRIPENTITY)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME (LINEARPARAMETERENTITY)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME (LOFTEDSURFACE)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME (MPOLYGON)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME (NAVISWORKSMODEL)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME (NURBSURFACE)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME (POINTPARAMETERENTITY)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME (POLARGRIPENTITY)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME (REVOLVEDSURFACE)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME (ROTATIONGRIPENTITY)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME (ROTATIONPARAMETERENTITY)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME (RTEXT)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME (SWEPTSURFACE)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME (TABLE)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME (VISIBILITYGRIPENTITY)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME (VISIBILITYPARAMETERENTITY)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME (XYGRIPENTITY)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME (XYPARAMETERENTITY)
#  endif

// clang-format: on
/*******************************************************************
 *     Functions created from macro to cast dwg object to object     *
 *                 Usage :- dwg_object_to_OBJECT(),                  *
 *            where OBJECT can be LAYER or BLOCK_HEADER              *
 ********************************************************************/
/**
 * \fn Dwg_Object_OBJECT *dwg_object_to_OBJECT(Dwg_Object *obj)
 * cast a Dwg_Object to Object
 */
// clang-format: off
CAST_DWG_OBJECT_TO_OBJECT (ACSH_BOOLEAN_CLASS)
CAST_DWG_OBJECT_TO_OBJECT (ACSH_BOX_CLASS)
CAST_DWG_OBJECT_TO_OBJECT (ACSH_CONE_CLASS)
CAST_DWG_OBJECT_TO_OBJECT (ACSH_CYLINDER_CLASS)
CAST_DWG_OBJECT_TO_OBJECT (ACSH_FILLET_CLASS)
CAST_DWG_OBJECT_TO_OBJECT (ACSH_HISTORY_CLASS)
CAST_DWG_OBJECT_TO_OBJECT (ACSH_SPHERE_CLASS)
CAST_DWG_OBJECT_TO_OBJECT (ACSH_TORUS_CLASS)
CAST_DWG_OBJECT_TO_OBJECT (ACSH_WEDGE_CLASS)
CAST_DWG_OBJECT_TO_OBJECT (APPID)
CAST_DWG_OBJECT_TO_OBJECT (APPID_CONTROL)
CAST_DWG_OBJECT_TO_OBJECT (ASSOCGEOMDEPENDENCY)
CAST_DWG_OBJECT_TO_OBJECT (ASSOCNETWORK)
CAST_DWG_OBJECT_TO_OBJECT (BLOCKALIGNMENTGRIP)
CAST_DWG_OBJECT_TO_OBJECT (BLOCKALIGNMENTPARAMETER)
CAST_DWG_OBJECT_TO_OBJECT (BLOCKBASEPOINTPARAMETER)
CAST_DWG_OBJECT_TO_OBJECT (BLOCKFLIPACTION)
CAST_DWG_OBJECT_TO_OBJECT (BLOCKFLIPGRIP)
CAST_DWG_OBJECT_TO_OBJECT (BLOCKFLIPPARAMETER)
CAST_DWG_OBJECT_TO_OBJECT (BLOCKGRIPLOCATIONCOMPONENT)
CAST_DWG_OBJECT_TO_OBJECT (BLOCKLINEARGRIP)
CAST_DWG_OBJECT_TO_OBJECT (BLOCKLOOKUPGRIP)
CAST_DWG_OBJECT_TO_OBJECT (BLOCKMOVEACTION)
CAST_DWG_OBJECT_TO_OBJECT (BLOCKROTATEACTION)
CAST_DWG_OBJECT_TO_OBJECT (BLOCKROTATIONGRIP)
CAST_DWG_OBJECT_TO_OBJECT (BLOCKSCALEACTION)
CAST_DWG_OBJECT_TO_OBJECT (BLOCKVISIBILITYGRIP)
CAST_DWG_OBJECT_TO_OBJECT (BLOCK_CONTROL)
CAST_DWG_OBJECT_TO_OBJECT (BLOCK_HEADER)
CAST_DWG_OBJECT_TO_OBJECT (CELLSTYLEMAP)
CAST_DWG_OBJECT_TO_OBJECT (DETAILVIEWSTYLE)
CAST_DWG_OBJECT_TO_OBJECT (DICTIONARY)
CAST_DWG_OBJECT_TO_OBJECT (DICTIONARYVAR)
CAST_DWG_OBJECT_TO_OBJECT (DICTIONARYWDFLT)
CAST_DWG_OBJECT_TO_OBJECT (DIMSTYLE)
CAST_DWG_OBJECT_TO_OBJECT (DIMSTYLE_CONTROL)
CAST_DWG_OBJECT_TO_OBJECT (DUMMY)
CAST_DWG_OBJECT_TO_OBJECT (DYNAMICBLOCKPURGEPREVENTER)
CAST_DWG_OBJECT_TO_OBJECT (FIELD)
CAST_DWG_OBJECT_TO_OBJECT (FIELDLIST)
CAST_DWG_OBJECT_TO_OBJECT (GEODATA)
CAST_DWG_OBJECT_TO_OBJECT (GROUP)
CAST_DWG_OBJECT_TO_OBJECT (IDBUFFER)
CAST_DWG_OBJECT_TO_OBJECT (IMAGEDEF)
CAST_DWG_OBJECT_TO_OBJECT (IMAGEDEF_REACTOR)
CAST_DWG_OBJECT_TO_OBJECT (INDEX)
CAST_DWG_OBJECT_TO_OBJECT (LAYER)
CAST_DWG_OBJECT_TO_OBJECT (LAYERFILTER)
CAST_DWG_OBJECT_TO_OBJECT (LAYER_CONTROL)
CAST_DWG_OBJECT_TO_OBJECT (LAYER_INDEX)
CAST_DWG_OBJECT_TO_OBJECT (LAYOUT)
CAST_DWG_OBJECT_TO_OBJECT (LONG_TRANSACTION)
CAST_DWG_OBJECT_TO_OBJECT (LTYPE)
CAST_DWG_OBJECT_TO_OBJECT (LTYPE_CONTROL)
CAST_DWG_OBJECT_TO_OBJECT (MLINESTYLE)
CAST_DWG_OBJECT_TO_OBJECT (PLACEHOLDER)
CAST_DWG_OBJECT_TO_OBJECT (PLOTSETTINGS)
CAST_DWG_OBJECT_TO_OBJECT (RASTERVARIABLES)
CAST_DWG_OBJECT_TO_OBJECT (RENDERENVIRONMENT)
CAST_DWG_OBJECT_TO_OBJECT (SCALE)
CAST_DWG_OBJECT_TO_OBJECT (SECTIONVIEWSTYLE)
CAST_DWG_OBJECT_TO_OBJECT (SECTION_MANAGER)
CAST_DWG_OBJECT_TO_OBJECT (SORTENTSTABLE)
CAST_DWG_OBJECT_TO_OBJECT (SPATIAL_FILTER)
CAST_DWG_OBJECT_TO_OBJECT (STYLE)
CAST_DWG_OBJECT_TO_OBJECT (STYLE_CONTROL)
CAST_DWG_OBJECT_TO_OBJECT (SUN)
CAST_DWG_OBJECT_TO_OBJECT (TABLEGEOMETRY)
CAST_DWG_OBJECT_TO_OBJECT (UCS)
CAST_DWG_OBJECT_TO_OBJECT (UCS_CONTROL)
CAST_DWG_OBJECT_TO_OBJECT (UNKNOWN_OBJ)
CAST_DWG_OBJECT_TO_OBJECT (VBA_PROJECT)
CAST_DWG_OBJECT_TO_OBJECT (VIEW)
CAST_DWG_OBJECT_TO_OBJECT (VIEW_CONTROL)
CAST_DWG_OBJECT_TO_OBJECT (VISUALSTYLE)
CAST_DWG_OBJECT_TO_OBJECT (VPORT)
CAST_DWG_OBJECT_TO_OBJECT (VPORT_CONTROL)
CAST_DWG_OBJECT_TO_OBJECT (VX_CONTROL)
CAST_DWG_OBJECT_TO_OBJECT (VX_TABLE_RECORD)
CAST_DWG_OBJECT_TO_OBJECT (WIPEOUTVARIABLES)
CAST_DWG_OBJECT_TO_OBJECT (XRECORD)
CAST_DWG_OBJECT_TO_OBJECT (PDFDEFINITION)
CAST_DWG_OBJECT_TO_OBJECT (DGNDEFINITION)
CAST_DWG_OBJECT_TO_OBJECT (DWFDEFINITION)
/* unstable */
CAST_DWG_OBJECT_TO_OBJECT (ACSH_BREP_CLASS)
CAST_DWG_OBJECT_TO_OBJECT (ACSH_CHAMFER_CLASS)
CAST_DWG_OBJECT_TO_OBJECT (ACSH_PYRAMID_CLASS)
CAST_DWG_OBJECT_TO_OBJECT (ALDIMOBJECTCONTEXTDATA)
CAST_DWG_OBJECT_TO_OBJECT (ASSOC2DCONSTRAINTGROUP)
CAST_DWG_OBJECT_TO_OBJECT (ASSOCACTION)
CAST_DWG_OBJECT_TO_OBJECT (ASSOCACTIONPARAM)
CAST_DWG_OBJECT_TO_OBJECT (ASSOCARRAYACTIONBODY)
CAST_DWG_OBJECT_TO_OBJECT (ASSOCASMBODYACTIONPARAM)
CAST_DWG_OBJECT_TO_OBJECT (ASSOCBLENDSURFACEACTIONBODY)
CAST_DWG_OBJECT_TO_OBJECT (ASSOCCOMPOUNDACTIONPARAM)
CAST_DWG_OBJECT_TO_OBJECT (ASSOCDEPENDENCY)
CAST_DWG_OBJECT_TO_OBJECT (ASSOCDIMDEPENDENCYBODY)
CAST_DWG_OBJECT_TO_OBJECT (ASSOCEXTENDSURFACEACTIONBODY)
CAST_DWG_OBJECT_TO_OBJECT (ASSOCEXTRUDEDSURFACEACTIONBODY)
CAST_DWG_OBJECT_TO_OBJECT (ASSOCFACEACTIONPARAM)
CAST_DWG_OBJECT_TO_OBJECT (ASSOCFILLETSURFACEACTIONBODY)
CAST_DWG_OBJECT_TO_OBJECT (ASSOCLOFTEDSURFACEACTIONBODY)
CAST_DWG_OBJECT_TO_OBJECT (ASSOCNETWORKSURFACEACTIONBODY)
CAST_DWG_OBJECT_TO_OBJECT (ASSOCOBJECTACTIONPARAM)
CAST_DWG_OBJECT_TO_OBJECT (ASSOCOFFSETSURFACEACTIONBODY)
CAST_DWG_OBJECT_TO_OBJECT (ASSOCOSNAPPOINTREFACTIONPARAM)
CAST_DWG_OBJECT_TO_OBJECT (ASSOCPATCHSURFACEACTIONBODY)
CAST_DWG_OBJECT_TO_OBJECT (ASSOCPATHACTIONPARAM)
CAST_DWG_OBJECT_TO_OBJECT (ASSOCPLANESURFACEACTIONBODY)
CAST_DWG_OBJECT_TO_OBJECT (ASSOCPOINTREFACTIONPARAM)
CAST_DWG_OBJECT_TO_OBJECT (ASSOCREVOLVEDSURFACEACTIONBODY)
CAST_DWG_OBJECT_TO_OBJECT (ASSOCTRIMSURFACEACTIONBODY)
CAST_DWG_OBJECT_TO_OBJECT (ASSOCVALUEDEPENDENCY)
CAST_DWG_OBJECT_TO_OBJECT (ASSOCVARIABLE)
CAST_DWG_OBJECT_TO_OBJECT (ASSOCVERTEXACTIONPARAM)
CAST_DWG_OBJECT_TO_OBJECT (BLKREFOBJECTCONTEXTDATA)
CAST_DWG_OBJECT_TO_OBJECT (BLOCKALIGNEDCONSTRAINTPARAMETER)
CAST_DWG_OBJECT_TO_OBJECT (BLOCKANGULARCONSTRAINTPARAMETER)
CAST_DWG_OBJECT_TO_OBJECT (BLOCKARRAYACTION)
CAST_DWG_OBJECT_TO_OBJECT (BLOCKDIAMETRICCONSTRAINTPARAMETER)
CAST_DWG_OBJECT_TO_OBJECT (BLOCKHORIZONTALCONSTRAINTPARAMETER)
CAST_DWG_OBJECT_TO_OBJECT (BLOCKLINEARCONSTRAINTPARAMETER)
CAST_DWG_OBJECT_TO_OBJECT (BLOCKLINEARPARAMETER)
CAST_DWG_OBJECT_TO_OBJECT (BLOCKLOOKUPACTION)
CAST_DWG_OBJECT_TO_OBJECT (BLOCKLOOKUPPARAMETER)
CAST_DWG_OBJECT_TO_OBJECT (BLOCKPARAMDEPENDENCYBODY)
CAST_DWG_OBJECT_TO_OBJECT (BLOCKPOINTPARAMETER)
CAST_DWG_OBJECT_TO_OBJECT (BLOCKPOLARGRIP)
CAST_DWG_OBJECT_TO_OBJECT (BLOCKPOLARPARAMETER)
CAST_DWG_OBJECT_TO_OBJECT (BLOCKPOLARSTRETCHACTION)
CAST_DWG_OBJECT_TO_OBJECT (BLOCKRADIALCONSTRAINTPARAMETER)
CAST_DWG_OBJECT_TO_OBJECT (BLOCKREPRESENTATION)
CAST_DWG_OBJECT_TO_OBJECT (BLOCKROTATIONPARAMETER)
CAST_DWG_OBJECT_TO_OBJECT (BLOCKSTRETCHACTION)
CAST_DWG_OBJECT_TO_OBJECT (BLOCKUSERPARAMETER)
CAST_DWG_OBJECT_TO_OBJECT (BLOCKVERTICALCONSTRAINTPARAMETER)
CAST_DWG_OBJECT_TO_OBJECT (BLOCKVISIBILITYPARAMETER)
CAST_DWG_OBJECT_TO_OBJECT (BLOCKXYGRIP)
CAST_DWG_OBJECT_TO_OBJECT (BLOCKXYPARAMETER)
CAST_DWG_OBJECT_TO_OBJECT (DATALINK)
CAST_DWG_OBJECT_TO_OBJECT (DBCOLOR)
CAST_DWG_OBJECT_TO_OBJECT (EVALUATION_GRAPH)
CAST_DWG_OBJECT_TO_OBJECT (FCFOBJECTCONTEXTDATA)
CAST_DWG_OBJECT_TO_OBJECT (GRADIENT_BACKGROUND)
CAST_DWG_OBJECT_TO_OBJECT (GROUND_PLANE_BACKGROUND)
CAST_DWG_OBJECT_TO_OBJECT (IBL_BACKGROUND)
CAST_DWG_OBJECT_TO_OBJECT (IMAGE_BACKGROUND)
CAST_DWG_OBJECT_TO_OBJECT (LEADEROBJECTCONTEXTDATA)
CAST_DWG_OBJECT_TO_OBJECT (LIGHTLIST)
CAST_DWG_OBJECT_TO_OBJECT (MATERIAL)
CAST_DWG_OBJECT_TO_OBJECT (MENTALRAYRENDERSETTINGS)
CAST_DWG_OBJECT_TO_OBJECT (MLEADERSTYLE)
CAST_DWG_OBJECT_TO_OBJECT (MTEXTOBJECTCONTEXTDATA)
CAST_DWG_OBJECT_TO_OBJECT (OBJECT_PTR)
CAST_DWG_OBJECT_TO_OBJECT (PARTIAL_VIEWING_INDEX)
CAST_DWG_OBJECT_TO_OBJECT (POINTCLOUDCOLORMAP)
CAST_DWG_OBJECT_TO_OBJECT (POINTCLOUDDEF)
CAST_DWG_OBJECT_TO_OBJECT (POINTCLOUDDEFEX)
CAST_DWG_OBJECT_TO_OBJECT (POINTCLOUDDEF_REACTOR)
CAST_DWG_OBJECT_TO_OBJECT (POINTCLOUDDEF_REACTOR_EX)
CAST_DWG_OBJECT_TO_OBJECT (PROXY_OBJECT)
CAST_DWG_OBJECT_TO_OBJECT (RAPIDRTRENDERSETTINGS)
CAST_DWG_OBJECT_TO_OBJECT (RENDERENTRY)
CAST_DWG_OBJECT_TO_OBJECT (RENDERGLOBAL)
CAST_DWG_OBJECT_TO_OBJECT (RENDERSETTINGS)
CAST_DWG_OBJECT_TO_OBJECT (SECTION_SETTINGS)
CAST_DWG_OBJECT_TO_OBJECT (SKYLIGHT_BACKGROUND)
CAST_DWG_OBJECT_TO_OBJECT (SOLID_BACKGROUND)
CAST_DWG_OBJECT_TO_OBJECT (SPATIAL_INDEX)
CAST_DWG_OBJECT_TO_OBJECT (TABLESTYLE)
CAST_DWG_OBJECT_TO_OBJECT (TEXTOBJECTCONTEXTDATA)
CAST_DWG_OBJECT_TO_OBJECT (ASSOCARRAYMODIFYPARAMETERS)
CAST_DWG_OBJECT_TO_OBJECT (ASSOCARRAYPATHPARAMETERS)
CAST_DWG_OBJECT_TO_OBJECT (ASSOCARRAYPOLARPARAMETERS)
CAST_DWG_OBJECT_TO_OBJECT (ASSOCARRAYRECTANGULARPARAMETERS)
#  ifdef DEBUG_CLASSES
CAST_DWG_OBJECT_TO_OBJECT (ACMECOMMANDHISTORY)
CAST_DWG_OBJECT_TO_OBJECT (ACMESCOPE)
CAST_DWG_OBJECT_TO_OBJECT (ACMESTATEMGR)
CAST_DWG_OBJECT_TO_OBJECT (ACSH_EXTRUSION_CLASS)
CAST_DWG_OBJECT_TO_OBJECT (ACSH_LOFT_CLASS)
CAST_DWG_OBJECT_TO_OBJECT (ACSH_REVOLVE_CLASS)
CAST_DWG_OBJECT_TO_OBJECT (ACSH_SWEEP_CLASS)
CAST_DWG_OBJECT_TO_OBJECT (ANGDIMOBJECTCONTEXTDATA)
CAST_DWG_OBJECT_TO_OBJECT (ANNOTSCALEOBJECTCONTEXTDATA)
CAST_DWG_OBJECT_TO_OBJECT (ASSOC3POINTANGULARDIMACTIONBODY)
CAST_DWG_OBJECT_TO_OBJECT (ASSOCALIGNEDDIMACTIONBODY)
CAST_DWG_OBJECT_TO_OBJECT (ASSOCARRAYMODIFYACTIONBODY)
CAST_DWG_OBJECT_TO_OBJECT (ASSOCEDGEACTIONPARAM)
CAST_DWG_OBJECT_TO_OBJECT (ASSOCEDGECHAMFERACTIONBODY)
CAST_DWG_OBJECT_TO_OBJECT (ASSOCEDGEFILLETACTIONBODY)
CAST_DWG_OBJECT_TO_OBJECT (ASSOCMLEADERACTIONBODY)
CAST_DWG_OBJECT_TO_OBJECT (ASSOCORDINATEDIMACTIONBODY)
CAST_DWG_OBJECT_TO_OBJECT (ASSOCPERSSUBENTMANAGER)
CAST_DWG_OBJECT_TO_OBJECT (ASSOCRESTOREENTITYSTATEACTIONBODY)
CAST_DWG_OBJECT_TO_OBJECT (ASSOCROTATEDDIMACTIONBODY)
CAST_DWG_OBJECT_TO_OBJECT (ASSOCSWEPTSURFACEACTIONBODY)
CAST_DWG_OBJECT_TO_OBJECT (BLOCKPROPERTIESTABLE)
CAST_DWG_OBJECT_TO_OBJECT (BLOCKPROPERTIESTABLEGRIP)
CAST_DWG_OBJECT_TO_OBJECT (BREAKDATA)
CAST_DWG_OBJECT_TO_OBJECT (BREAKPOINTREF)
CAST_DWG_OBJECT_TO_OBJECT (CONTEXTDATAMANAGER)
CAST_DWG_OBJECT_TO_OBJECT (CSACDOCUMENTOPTIONS)
CAST_DWG_OBJECT_TO_OBJECT (CURVEPATH)
CAST_DWG_OBJECT_TO_OBJECT (DATATABLE)
CAST_DWG_OBJECT_TO_OBJECT (DIMASSOC)
CAST_DWG_OBJECT_TO_OBJECT (DMDIMOBJECTCONTEXTDATA)
CAST_DWG_OBJECT_TO_OBJECT (DYNAMICBLOCKPROXYNODE)
CAST_DWG_OBJECT_TO_OBJECT (GEOMAPIMAGE)
CAST_DWG_OBJECT_TO_OBJECT (MLEADEROBJECTCONTEXTDATA)
CAST_DWG_OBJECT_TO_OBJECT (MOTIONPATH)
CAST_DWG_OBJECT_TO_OBJECT (MTEXTATTRIBUTEOBJECTCONTEXTDATA)
CAST_DWG_OBJECT_TO_OBJECT (NAVISWORKSMODELDEF)
CAST_DWG_OBJECT_TO_OBJECT (ORDDIMOBJECTCONTEXTDATA)
CAST_DWG_OBJECT_TO_OBJECT (PERSUBENTMGR)
CAST_DWG_OBJECT_TO_OBJECT (POINTPATH)
CAST_DWG_OBJECT_TO_OBJECT (RADIMLGOBJECTCONTEXTDATA)
CAST_DWG_OBJECT_TO_OBJECT (RADIMOBJECTCONTEXTDATA)
CAST_DWG_OBJECT_TO_OBJECT (SUNSTUDY)
CAST_DWG_OBJECT_TO_OBJECT (TABLECONTENT)
CAST_DWG_OBJECT_TO_OBJECT (TVDEVICEPROPERTIES)
// CAST_DWG_OBJECT_TO_OBJECT (ABSHDRAWINGSETTINGS)
// CAST_DWG_OBJECT_TO_OBJECT (ACAECUSTOBJ)
// CAST_DWG_OBJECT_TO_OBJECT (ACAEEEMGROBJ)
// CAST_DWG_OBJECT_TO_OBJECT (ACAMCOMP)
// CAST_DWG_OBJECT_TO_OBJECT (ACAMCOMPDEF)
// CAST_DWG_OBJECT_TO_OBJECT (ACAMCOMPDEFMGR)
// CAST_DWG_OBJECT_TO_OBJECT (ACAMCONTEXTMODELER)
// CAST_DWG_OBJECT_TO_OBJECT (ACAMGDIMSTD)
// CAST_DWG_OBJECT_TO_OBJECT (ACAMGFILTERDAT)
// CAST_DWG_OBJECT_TO_OBJECT (ACAMGHOLECHARTSTDCSN)
// CAST_DWG_OBJECT_TO_OBJECT (ACAMGHOLECHARTSTDDIN)
// CAST_DWG_OBJECT_TO_OBJECT (ACAMGHOLECHARTSTDISO)
// CAST_DWG_OBJECT_TO_OBJECT (ACAMGLAYSTD)
// CAST_DWG_OBJECT_TO_OBJECT (ACAMGRCOMPDEF)
// CAST_DWG_OBJECT_TO_OBJECT (ACAMGRCOMPDEFSET)
// CAST_DWG_OBJECT_TO_OBJECT (ACAMGTITLESTD)
// CAST_DWG_OBJECT_TO_OBJECT (ACAMMVDBACKUPOBJECT)
// CAST_DWG_OBJECT_TO_OBJECT (ACAMPROJECT)
// CAST_DWG_OBJECT_TO_OBJECT (ACAMSHAFTCOMPDEF)
// CAST_DWG_OBJECT_TO_OBJECT (ACAMSTDPCOMPDEF)
// CAST_DWG_OBJECT_TO_OBJECT (ACAMWBLOCKTEMPENTS)
// CAST_DWG_OBJECT_TO_OBJECT (ACARRAYJIGENTITY)
// CAST_DWG_OBJECT_TO_OBJECT (ACCMCONTEXT)
// CAST_DWG_OBJECT_TO_OBJECT (ACDBCIRCARCRES)
// CAST_DWG_OBJECT_TO_OBJECT (ACDBDIMENSIONRES)
// CAST_DWG_OBJECT_TO_OBJECT (ACDBENTITYCACHE)
// CAST_DWG_OBJECT_TO_OBJECT (ACDBLINERES)
// CAST_DWG_OBJECT_TO_OBJECT (ACDBSTDPARTRES_ARC)
// CAST_DWG_OBJECT_TO_OBJECT (ACDBSTDPARTRES_LINE)
// CAST_DWG_OBJECT_TO_OBJECT (ACDB_HATCHSCALECONTEXTDATA_CLASS)
// CAST_DWG_OBJECT_TO_OBJECT (ACDB_HATCHVIEWCONTEXTDATA_CLASS)
// CAST_DWG_OBJECT_TO_OBJECT (ACDB_PROXY_ENTITY_DATA)
// CAST_DWG_OBJECT_TO_OBJECT (ACDSRECORD)
// CAST_DWG_OBJECT_TO_OBJECT (ACDSSCHEMA)
// CAST_DWG_OBJECT_TO_OBJECT (ACGREFACADMASTER)
// CAST_DWG_OBJECT_TO_OBJECT (ACGREFMASTER)
// CAST_DWG_OBJECT_TO_OBJECT (ACIMINTSYSVAR)
// CAST_DWG_OBJECT_TO_OBJECT (ACIMREALSYSVAR)
// CAST_DWG_OBJECT_TO_OBJECT (ACIMSTRSYSVAR)
// CAST_DWG_OBJECT_TO_OBJECT (ACIMSYSVARMAN)
// CAST_DWG_OBJECT_TO_OBJECT (ACMANOOTATIONVIEWSTANDARDANSI)
// CAST_DWG_OBJECT_TO_OBJECT (ACMANOOTATIONVIEWSTANDARDCSN)
// CAST_DWG_OBJECT_TO_OBJECT (ACMANOOTATIONVIEWSTANDARDDIN)
// CAST_DWG_OBJECT_TO_OBJECT (ACMANOOTATIONVIEWSTANDARDISO)
// CAST_DWG_OBJECT_TO_OBJECT (ACMAPLEGENDDBOBJECT)
// CAST_DWG_OBJECT_TO_OBJECT (ACMAPLEGENDITEMDBOBJECT)
// CAST_DWG_OBJECT_TO_OBJECT (ACMAPMAPVIEWPORTDBOBJECT)
// CAST_DWG_OBJECT_TO_OBJECT (ACMAPPRINTLAYOUTELEMENTDBOBJECTCONTAINER)
// CAST_DWG_OBJECT_TO_OBJECT (ACMBALLOON)
// CAST_DWG_OBJECT_TO_OBJECT (ACMBOM)
// CAST_DWG_OBJECT_TO_OBJECT (ACMBOMROW)
// CAST_DWG_OBJECT_TO_OBJECT (ACMBOMROWSTRUCT)
// CAST_DWG_OBJECT_TO_OBJECT (ACMBOMSTANDARDANSI)
// CAST_DWG_OBJECT_TO_OBJECT (ACMBOMSTANDARDCSN)
// CAST_DWG_OBJECT_TO_OBJECT (ACMBOMSTANDARDDIN)
// CAST_DWG_OBJECT_TO_OBJECT (ACMBOMSTANDARDISO)
// CAST_DWG_OBJECT_TO_OBJECT (ACMCENTERLINESTANDARDANSI)
// CAST_DWG_OBJECT_TO_OBJECT (ACMCENTERLINESTANDARDCSN)
// CAST_DWG_OBJECT_TO_OBJECT (ACMCENTERLINESTANDARDDIN)
// CAST_DWG_OBJECT_TO_OBJECT (ACMCENTERLINESTANDARDISO)
// CAST_DWG_OBJECT_TO_OBJECT (ACMDATADICTIONARY)
// CAST_DWG_OBJECT_TO_OBJECT (ACMDATAENTRY)
// CAST_DWG_OBJECT_TO_OBJECT (ACMDATAENTRYBLOCK)
// CAST_DWG_OBJECT_TO_OBJECT (ACMDATUMID)
// CAST_DWG_OBJECT_TO_OBJECT (ACMDATUMSTANDARDANSI)
// CAST_DWG_OBJECT_TO_OBJECT (ACMDATUMSTANDARDCSN)
// CAST_DWG_OBJECT_TO_OBJECT (ACMDATUMSTANDARDDIN)
// CAST_DWG_OBJECT_TO_OBJECT (ACMDATUMSTANDARDISO)
// CAST_DWG_OBJECT_TO_OBJECT (ACMDATUMSTANDARDISO2012)
// CAST_DWG_OBJECT_TO_OBJECT (ACMDETAILSTANDARDANSI)
// CAST_DWG_OBJECT_TO_OBJECT (ACMDETAILSTANDARDCSN)
// CAST_DWG_OBJECT_TO_OBJECT (ACMDETAILSTANDARDDIN)
// CAST_DWG_OBJECT_TO_OBJECT (ACMDETAILSTANDARDISO)
// CAST_DWG_OBJECT_TO_OBJECT (ACMDETAILTANDARDCUSTOM)
// CAST_DWG_OBJECT_TO_OBJECT (ACMDIMBREAKPERSREACTOR)
// CAST_DWG_OBJECT_TO_OBJECT (ACMEDRAWINGMAN)
// CAST_DWG_OBJECT_TO_OBJECT (ACMEVIEW)
// CAST_DWG_OBJECT_TO_OBJECT (ACME_DATABASE)
// CAST_DWG_OBJECT_TO_OBJECT (ACME_DOCUMENT)
// CAST_DWG_OBJECT_TO_OBJECT (ACMFCFRAME)
// CAST_DWG_OBJECT_TO_OBJECT (ACMFCFSTANDARDANSI)
// CAST_DWG_OBJECT_TO_OBJECT (ACMFCFSTANDARDCSN)
// CAST_DWG_OBJECT_TO_OBJECT (ACMFCFSTANDARDDIN)
// CAST_DWG_OBJECT_TO_OBJECT (ACMFCFSTANDARDISO)
// CAST_DWG_OBJECT_TO_OBJECT (ACMFCFSTANDARDISO2004)
// CAST_DWG_OBJECT_TO_OBJECT (ACMFCFSTANDARDISO2012)
// CAST_DWG_OBJECT_TO_OBJECT (ACMIDSTANDARDANSI)
// CAST_DWG_OBJECT_TO_OBJECT (ACMIDSTANDARDCSN)
// CAST_DWG_OBJECT_TO_OBJECT (ACMIDSTANDARDDIN)
// CAST_DWG_OBJECT_TO_OBJECT (ACMIDSTANDARDISO)
// CAST_DWG_OBJECT_TO_OBJECT (ACMIDSTANDARDISO2004)
// CAST_DWG_OBJECT_TO_OBJECT (ACMIDSTANDARDISO2012)
// CAST_DWG_OBJECT_TO_OBJECT (ACMNOTESTANDARDANSI)
// CAST_DWG_OBJECT_TO_OBJECT (ACMNOTESTANDARDCSN)
// CAST_DWG_OBJECT_TO_OBJECT (ACMNOTESTANDARDDIN)
// CAST_DWG_OBJECT_TO_OBJECT (ACMNOTESTANDARDISO)
// CAST_DWG_OBJECT_TO_OBJECT (ACMPARTLIST)
// CAST_DWG_OBJECT_TO_OBJECT (ACMPICKOBJ)
// CAST_DWG_OBJECT_TO_OBJECT (ACMSECTIONSTANDARDANSI)
// CAST_DWG_OBJECT_TO_OBJECT (ACMSECTIONSTANDARDCSN2002)
// CAST_DWG_OBJECT_TO_OBJECT (ACMSECTIONSTANDARDCUSTOM)
// CAST_DWG_OBJECT_TO_OBJECT (ACMSECTIONSTANDARDDIN)
// CAST_DWG_OBJECT_TO_OBJECT (ACMSECTIONSTANDARDISO)
// CAST_DWG_OBJECT_TO_OBJECT (ACMSECTIONSTANDARDISO2001)
// CAST_DWG_OBJECT_TO_OBJECT (ACMSTANDARDANSI)
// CAST_DWG_OBJECT_TO_OBJECT (ACMSTANDARDCSN)
// CAST_DWG_OBJECT_TO_OBJECT (ACMSTANDARDDIN)
// CAST_DWG_OBJECT_TO_OBJECT (ACMSTANDARDISO)
// CAST_DWG_OBJECT_TO_OBJECT (ACMSURFSTANDARDANSI)
// CAST_DWG_OBJECT_TO_OBJECT (ACMSURFSTANDARDCSN)
// CAST_DWG_OBJECT_TO_OBJECT (ACMSURFSTANDARDDIN)
// CAST_DWG_OBJECT_TO_OBJECT (ACMSURFSTANDARDISO)
// CAST_DWG_OBJECT_TO_OBJECT (ACMSURFSTANDARDISO2002)
// CAST_DWG_OBJECT_TO_OBJECT (ACMSURFSYM)
// CAST_DWG_OBJECT_TO_OBJECT (ACMTAPERSTANDARDANSI)
// CAST_DWG_OBJECT_TO_OBJECT (ACMTAPERSTANDARDCSN)
// CAST_DWG_OBJECT_TO_OBJECT (ACMTAPERSTANDARDDIN)
// CAST_DWG_OBJECT_TO_OBJECT (ACMTAPERSTANDARDISO)
// CAST_DWG_OBJECT_TO_OBJECT (ACMTHREADLINESTANDARDANSI)
// CAST_DWG_OBJECT_TO_OBJECT (ACMTHREADLINESTANDARDCSN)
// CAST_DWG_OBJECT_TO_OBJECT (ACMTHREADLINESTANDARDDIN)
// CAST_DWG_OBJECT_TO_OBJECT (ACMTHREADLINESTANDARDISO)
// CAST_DWG_OBJECT_TO_OBJECT (ACMWELDSTANDARDANSI)
// CAST_DWG_OBJECT_TO_OBJECT (ACMWELDSTANDARDCSN)
// CAST_DWG_OBJECT_TO_OBJECT (ACMWELDSTANDARDDIN)
// CAST_DWG_OBJECT_TO_OBJECT (ACMWELDSTANDARDISO)
// CAST_DWG_OBJECT_TO_OBJECT (ACMWELDSYM)
// CAST_DWG_OBJECT_TO_OBJECT (ACRFATTGENMGR)
// CAST_DWG_OBJECT_TO_OBJECT (ACRFINSADJ)
// CAST_DWG_OBJECT_TO_OBJECT (ACRFINSADJUSTERMGR)
// CAST_DWG_OBJECT_TO_OBJECT (ACRFMCADAPIATTHOLDER)
// CAST_DWG_OBJECT_TO_OBJECT (ACRFOBJATTMGR)
// CAST_DWG_OBJECT_TO_OBJECT (ACSH_SUBENT_MATERIAL_CLASS)
// CAST_DWG_OBJECT_TO_OBJECT (AC_AM_2D_XREF_MGR)
// CAST_DWG_OBJECT_TO_OBJECT (AC_AM_BASIC_VIEW)
// CAST_DWG_OBJECT_TO_OBJECT (AC_AM_BASIC_VIEW_DEF)
// CAST_DWG_OBJECT_TO_OBJECT (AC_AM_COMPLEX_HIDE_SITUATION)
// CAST_DWG_OBJECT_TO_OBJECT (AC_AM_COMP_VIEW_DEF)
// CAST_DWG_OBJECT_TO_OBJECT (AC_AM_COMP_VIEW_INST)
// CAST_DWG_OBJECT_TO_OBJECT (AC_AM_DIRTY_NODES)
// CAST_DWG_OBJECT_TO_OBJECT (AC_AM_HIDE_SITUATION)
// CAST_DWG_OBJECT_TO_OBJECT (AC_AM_MAPPER_CACHE)
// CAST_DWG_OBJECT_TO_OBJECT (AC_AM_MASTER_VIEW_DEF)
// CAST_DWG_OBJECT_TO_OBJECT (AC_AM_MVD_DEP_MGR)
// CAST_DWG_OBJECT_TO_OBJECT (AC_AM_OVERRIDE_FILTER)
// CAST_DWG_OBJECT_TO_OBJECT (AC_AM_PROPS_OVERRIDE)
// CAST_DWG_OBJECT_TO_OBJECT (AC_AM_SHAFT_HIDE_SITUATION)
// CAST_DWG_OBJECT_TO_OBJECT (AC_AM_STDP_VIEW_DEF)
// CAST_DWG_OBJECT_TO_OBJECT (AC_AM_TRANSFORM_GHOST)
// CAST_DWG_OBJECT_TO_OBJECT (ADAPPL)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_ALIGNMENT_DESIGN_CHECK_SET)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_ALIGNMENT_LABEL_SET)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_ALIGNMENT_LABEL_SET_EXT)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_ALIGNMENT_PARCEL_NODE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_ALIGNMENT_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_APPURTENANCE_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_ASSEMBLY_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_BUILDING_SITE_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_CANT_DIAGRAM_VIEW_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_CATCHMENT_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_CLASS_NODE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_CONTOURVIEW)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_CORRIDOR_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_ALIGNMENT)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_ALIGNMENT_CANT_LABEL_GROUP)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_ALIGNMENT_CSV)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_ALIGNMENT_CURVE_LABEL)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_ALIGNMENT_DESIGNSPEED_LABEL_GROUP)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_ALIGNMENT_GEOMPT_LABEL_GROUP)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_ALIGNMENT_INDEXED_PI_LABEL)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_ALIGNMENT_MINOR_STATION_LABEL_GROUP)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_ALIGNMENT_PI_LABEL)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_ALIGNMENT_SPIRAL_LABEL)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_ALIGNMENT_STAEQU_LABEL_GROUP)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_ALIGNMENT_STATION_LABEL_GROUP)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_ALIGNMENT_STATION_OFFSET_LABEL)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_ALIGNMENT_SUPERELEVATION_LABEL_GROUP)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_ALIGNMENT_TABLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_ALIGNMENT_TANGENT_LABEL)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_ALIGNMENT_VERTICAL_GEOMPT_LABELING)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_APPURTENANCE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_APPURTENANCE_CSV)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_APPURTENANCE_LABELING)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_APPURTENANCE_PROFILE_LABELING)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_ASSEMBLY)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_AUTO_CORRIDOR_FEATURE_LINE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_AUTO_CORRIDOR_FEATURE_LINE_PROFILE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_AUTO_CORRIDOR_FEATURE_LINE_SECTION)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_AUTO_FEATURE_LINE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_AUTO_FEATURE_LINE_CSV)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_AUTO_FEATURE_LINE_PROFILE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_AUTO_FEATURE_LINE_SECTION)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_BUILDINGSITE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_BUILDINGUTIL_CONNECTOR)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_CANT_DIAGRAM_VIEW)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_CATCHMENT_AREA)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_CATCHMENT_AREA_LABEL)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_CORRIDOR)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_CROSSING_PIPE_PROFILE_LABELING)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_CROSSING_PRESSURE_PIPE_PROFILE_LABELING)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_CSVSTATIONSLIDER)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_FACE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_FEATURE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_FEATURE_LABEL)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_FEATURE_LINE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_FEATURE_LINE_CSV)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_FEATURE_LINE_PROFILE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_FEATURE_LINE_SECTION)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_FITTING)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_FITTING_CSV)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_FITTING_LABELING)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_FITTING_PROFILE_LABELING)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_FLOW_SEGMENT_LABEL)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_GENERAL_SEGMENT_LABEL)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_GRADING)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_GRAPH)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_GRAPHPROFILE_NETWORKPART)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_GRAPHPROFILE_PRESSUREPART)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_GRID_SURFACE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_GRID_SURFACE_CSV)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_HORGEOMETRY_BAND_LABEL_GROUP)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_HYDRO_REGION)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_INTERFERENCE_CHECK)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_INTERFERENCE_PART)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_INTERFERENCE_PART_SECTION)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_INTERSECTION)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_INTERSECTION_LOCATION_LABELING)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_LABELING)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_LEGEND_TABLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_LINE_BETWEEN_POINTS_LABEL)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_LOTLINE_CSV)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_MASSHAULLINE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_MASS_HAUL_VIEW)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_MATCHLINE_LABELING)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_MATCH_LINE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_MATERIAL_SECTION)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_NETWORK)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_NOTE_LABEL)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_OFFSET_ELEV_LABEL)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_PARCEL_BOUNDARY)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_PARCEL_FACE_LABEL)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_PARCEL_SEGMENT)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_PARCEL_SEGMENT_LABEL)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_PARCEL_SEGMENT_TABLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_PARCEL_TABLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_PIPE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_PIPENETWORK_BAND_LABEL_GROUP)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_PIPE_CSV)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_PIPE_LABELING)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_PIPE_PROFILE_LABELING)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_PIPE_SECTION_LABELING)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_PIPE_TABLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_POINT_ENT)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_POINT_GROUP)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_POINT_TABLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_PRESSUREPIPENETWORK)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_PRESSURE_PART_TABLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_PRESSURE_PIPE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_PRESSURE_PIPE_CSV)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_PRESSURE_PIPE_LABELING)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_PRESSURE_PIPE_PROFILE_LABELING)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_PRESSURE_PIPE_SECTION_LABELING)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_PROFILE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_PROFILEDATA_BAND_LABEL_GROUP)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_PROFILE_PROJECTION)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_PROFILE_PROJECTION_LABEL)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_PROFILE_VIEW)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_PROFILE_VIEW_DEPTH_LABEL)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_QUANTITY_TAKEOFF_AGGREGATE_EARTHWORK_TABLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_RIGHT_OF_WAY)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_SAMPLELINE_LABELING)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_SAMPLE_LINE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_SAMPLE_LINE_GROUP)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_SECTION)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_SECTIONALDATA_BAND_LABEL_GROUP)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_SECTIONDATA_BAND_LABEL_GROUP)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_SECTIONSEGMENT_BAND_LABEL_GROUP)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_SECTION_CORRIDOR)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_SECTION_CORRIDOR_POINT_LABEL_GROUP)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_SECTION_GRADEBREAK_LABEL_GROUP)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_SECTION_MINOR_OFFSET_LABEL_GROUP)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_SECTION_OFFSET_LABEL_GROUP)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_SECTION_PIPENETWORK)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_SECTION_PRESSUREPIPENETWORK)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_SECTION_PROJECTION)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_SECTION_PROJECTION_LABEL)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_SECTION_SEGMENT_LABEL_GROUP)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_SECTION_VIEW)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_SECTION_VIEW_DEPTH_LABEL)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_SECTION_VIEW_QUANTITY_TAKEOFF_TABLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_SHEET)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_SPANNING_PIPE_LABELING)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_SPANNING_PIPE_PROFILE_LABELING)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_STATION_ELEV_LABEL)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_STRUCTURE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_STRUCTURE_CSV)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_STRUCTURE_LABELING)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_STRUCTURE_PROFILE_LABELING)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_STRUCTURE_SECTION_LABELING)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_SUBASSEMBLY)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_SUPERELEVATION_BAND_LABEL_GROUP)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_SUPERELEVATION_DIAGRAM_VIEW)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_SURFACE_CONTOUR_LABEL_GROUP)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_SURFACE_ELEVATION_LABEL)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_SURFACE_SLOPE_LABEL)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_SURVEY_FIGURE_LABEL_GROUP)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_SVFIGURE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_SVFIGURE_CSV)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_SVFIGURE_PROFILE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_SVFIGURE_SECTION)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_SVFIGURE_SEGMENT_LABEL)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_SVNETWORK)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_TANGENT_INTERSECTION_TABLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_TIN_SURFACE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_TIN_SURFACE_CSV)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_VALIGNMENT_CRESTCURVE_LABEL_GROUP)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_VALIGNMENT_CSV)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_VALIGNMENT_HAGEOMPT_LABEL_GROUP)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_VALIGNMENT_LINE_LABEL_GROUP)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_VALIGNMENT_MINOR_STATION_LABEL_GROUP)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_VALIGNMENT_PVI_LABEL_GROUP)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_VALIGNMENT_SAGCURVE_LABEL_GROUP)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_VALIGNMENT_STATION_LABEL_GROUP)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_VERTICALGEOMETRY_BAND_LABEL_GROUP)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_VIEWFRAME_LABELING)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_DISP_REP_VIEW_FRAME)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_FEATURELINE_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_FEATURE_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_FITTING_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_FORMAT_MANAGER_OBJECT)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_GRADEVIEW)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_GRADING_CRITERIA)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_GRADING_CRITERIA_SET)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_GRADING_GROUP)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_GRADING_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_IMPORT_STORM_SEWER_DEFAULTS)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_INTERFERENCE_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_INTERSECTION_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_LABEL_COLLECTOR_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_LABEL_NODE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_LABEL_RADIAL_LINE_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_LABEL_TEXT_ITERATOR_CURVE_OR_SPIRAL_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_LABEL_TEXT_ITERATOR_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_LABEL_TEXT_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_LABEL_VECTOR_ARROW_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_LEGEND_TABLE_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_MASS_HAUL_LINE_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_MASS_HAUL_VIEW_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_MATCHLINE_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_MATERIAL_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_NETWORK_PART_CATALOG_DEF_NODE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_NETWORK_PART_FAMILY_ITEM)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_NETWORK_PART_LIST)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_NETWORK_RULE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_PARCEL_NODE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_PARCEL_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_PART_SIZE_FILTER)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_PIPE_RULES)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_PIPE_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_PIPE_STYLE_EXTENSION)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_POINTCLOUD_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_POINTVIEW)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_POINT_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_PRESSURE_PART_LIST)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_PRESSURE_PIPE_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_PROFILESECTIONENTITY_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_PROFILE_DESIGN_CHECK_SET)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_PROFILE_LABEL_SET)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_PROFILE_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_PROFILE_VIEW_BAND_STYLE_SET)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_PROFILE_VIEW_DATA_BAND_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_PROFILE_VIEW_HORIZONTAL_GEOMETRY_BAND_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_PROFILE_VIEW_PIPE_NETWORK_BAND_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_PROFILE_VIEW_SECTIONAL_DATA_BAND_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_PROFILE_VIEW_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_PROFILE_VIEW_SUPERELEVATION_DIAGRAM_BAND_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_PROFILE_VIEW_VERTICAL_GEOMETRY_BAND_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_QUANTITY_TAKEOFF_CRITERIA)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_ROADWAYLINK_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_ROADWAYMARKER_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_ROADWAYSHAPE_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_ROADWAY_STYLE_SET)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_ROOT_SETTINGS_NODE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_SAMPLE_LINE_GROUP_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_SAMPLE_LINE_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_SECTION_LABEL_SET)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_SECTION_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_SECTION_VIEW_BAND_STYLE_SET)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_SECTION_VIEW_DATA_BAND_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_SECTION_VIEW_ROAD_SURFACE_BAND_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_SECTION_VIEW_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_SETTINGS_NODE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_SHEET_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_SLOPE_PATTERN_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_STATION_FORMAT_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_STRUCTURE_RULES)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_STUCTURE_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_SUPERELEVATION_DIAGRAM_VIEW_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_SURFACE_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_SVFIGURE_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_SVNETWORK_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_TABLE_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_TAG_MANAGER)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_TREE_NODE)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_USER_DEFINED_ATTRIBUTE_CLASSIFICATION)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_VALIGNMENT_STYLE_EXTENSION)
// CAST_DWG_OBJECT_TO_OBJECT (AECC_VIEW_FRAME_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AECS_DISP_PROPS_MEMBER)
// CAST_DWG_OBJECT_TO_OBJECT (AECS_DISP_PROPS_MEMBER_LOGICAL)
// CAST_DWG_OBJECT_TO_OBJECT (AECS_DISP_PROPS_MEMBER_PLAN)
// CAST_DWG_OBJECT_TO_OBJECT (AECS_DISP_PROPS_MEMBER_PLAN_SKETCH)
// CAST_DWG_OBJECT_TO_OBJECT (AECS_DISP_PROPS_MEMBER_PROJECTED)
// CAST_DWG_OBJECT_TO_OBJECT (AECS_DISP_REP_MEMBER_ELEVATION_DESIGN)
// CAST_DWG_OBJECT_TO_OBJECT (AECS_DISP_REP_MEMBER_ELEVATION_DETAIL)
// CAST_DWG_OBJECT_TO_OBJECT (AECS_DISP_REP_MEMBER_LOGICAL)
// CAST_DWG_OBJECT_TO_OBJECT (AECS_DISP_REP_MEMBER_MODEL_DESIGN)
// CAST_DWG_OBJECT_TO_OBJECT (AECS_DISP_REP_MEMBER_MODEL_DETAIL)
// CAST_DWG_OBJECT_TO_OBJECT (AECS_DISP_REP_MEMBER_PLAN_DESIGN)
// CAST_DWG_OBJECT_TO_OBJECT (AECS_DISP_REP_MEMBER_PLAN_DETAIL)
// CAST_DWG_OBJECT_TO_OBJECT (AECS_DISP_REP_MEMBER_PLAN_SKETCH)
// CAST_DWG_OBJECT_TO_OBJECT (AECS_MEMBER_NODE_SHAPE)
// CAST_DWG_OBJECT_TO_OBJECT (AECS_MEMBER_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_2DSECTION_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_AECDBDISPREPBDGELEVLINEPLAN100)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_AECDBDISPREPBDGELEVLINEPLAN50)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_AECDBDISPREPBDGSECTIONLINEPLAN100)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_AECDBDISPREPBDGSECTIONLINEPLAN50)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_AECDBDISPREPCEILINGGRIDPLAN100)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_AECDBDISPREPCEILINGGRIDPLAN50)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_AECDBDISPREPCOLUMNGRIDPLAN100)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_AECDBDISPREPCOLUMNGRIDPLAN50)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_AECDBDISPREPCURTAINWALLLAYOUTPLAN100)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_AECDBDISPREPCURTAINWALLLAYOUTPLAN50)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_AECDBDISPREPCURTAINWALLUNITPLAN100)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_AECDBDISPREPCURTAINWALLUNITPLAN50)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_AECDBDISPREPMVBLOCKREFPLAN100)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_AECDBDISPREPMVBLOCKREFPLAN50)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_AECDBDISPREPROOFPLAN100)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_AECDBDISPREPROOFPLAN50)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_AECDBDISPREPROOFSLABPLAN100)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_AECDBDISPREPROOFSLABPLAN50)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_AECDBDISPREPSLABPLAN100)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_AECDBDISPREPSLABPLAN50)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_AECDBDISPREPSPACEPLAN100)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_AECDBDISPREPSPACEPLAN50)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_AECDBDISPREPWALLPLAN100)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_AECDBDISPREPWALLPLAN50)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_AECDBDISPREPWINDOWASSEMBLYPLAN100)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_AECDBDISPREPWINDOWASSEMBLYPLAN50)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_AECDBDISPREPZONE100)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_AECDBDISPREPZONE50)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_AECDBZONEDEF)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_AECDBZONESTYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_ANCHOR_OPENINGBASE_TO_WALL)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_CLASSIFICATION_DEF)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_CLASSIFICATION_SYSTEM_DEF)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_CLEANUP_GROUP_DEF)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_CURTAIN_WALL_LAYOUT_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_CURTAIN_WALL_UNIT_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_CVSECTIONVIEW)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DB_DISP_REP_DIM_GROUP_PLAN)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DB_DISP_REP_DIM_GROUP_PLAN100)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DB_DISP_REP_DIM_GROUP_PLAN50)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DIM_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISPLAYTHEME_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISPREPAECDBDISPREPMASSELEMPLAN100)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISPREPAECDBDISPREPMASSELEMPLAN50)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISPREPAECDBDISPREPMASSGROUPPLAN100)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISPREPAECDBDISPREPMASSGROUPPLAN50)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISPREPAECDBDISPREPOPENINGPLAN100)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISPREPAECDBDISPREPOPENINGPLAN50)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISPREPAECDBDISPREPOPENINGPLANREFLECTED)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISPREPAECDBDISPREPOPENINGSILLPLAN)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISPROPSMASSELEMPLANCOMMON)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISPROPSMASSGROUPPLANCOMMON)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISPROPSOPENINGPLANCOMMON)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISPROPSOPENINGPLANCOMMONHATCHED)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISPROPSOPENINGSILLPLAN)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_PROPS_2D_SECTION)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_PROPS_CLIP_VOLUME)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_PROPS_CLIP_VOLUME_RESULT)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_PROPS_DIM)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_PROPS_DISPLAYTHEME)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_PROPS_DOOR)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_PROPS_DOOR_NOMINAL)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_PROPS_DOOR_PLAN_100)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_PROPS_DOOR_PLAN_50)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_PROPS_DOOR_THRESHOLD_PLAN)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_PROPS_DOOR_THRESHOLD_SYMBOL_PLAN)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_PROPS_EDITINPLACEPROFILE_MODEL)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_PROPS_ENT)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_PROPS_ENT_REF)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_PROPS_GRID_ASSEMBLY_MODEL)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_PROPS_GRID_ASSEMBLY_PLAN)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_PROPS_LAYOUT_CURVE)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_PROPS_LAYOUT_GRID2D)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_PROPS_LAYOUT_GRID3D)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_PROPS_MASKBLOCK)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_PROPS_MASS_ELEM_MODEL)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_PROPS_MASS_GROUP)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_PROPS_MATERIAL)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_PROPS_OPENING)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_PROPS_POLYGON_MODEL)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_PROPS_POLYGON_TRUECOLOUR)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_PROPS_RAILING_MODEL)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_PROPS_RAILING_PLAN)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_PROPS_ROOF)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_PROPS_ROOFSLAB)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_PROPS_ROOFSLAB_PLAN)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_PROPS_SCHEDULE_TABLE)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_PROPS_SLAB)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_PROPS_SLAB_PLAN)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_PROPS_SLICE)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_PROPS_SPACE_DECOMPOSED)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_PROPS_SPACE_MODEL)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_PROPS_SPACE_PLAN)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_PROPS_STAIR_MODEL)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_PROPS_STAIR_PLAN)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_PROPS_STAIR_PLAN_OVERLAPPING)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_PROPS_WALL_GRAPH)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_PROPS_WALL_MODEL)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_PROPS_WALL_PLAN)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_PROPS_WALL_SCHEM)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_PROPS_WINDOW)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_PROPS_WINDOW_ASSEMBLY_SILL_PLAN)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_PROPS_WINDOW_NOMINAL)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_PROPS_WINDOW_PLAN_100)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_PROPS_WINDOW_PLAN_50)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_PROPS_WINDOW_SILL_PLAN)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_PROPS_ZONE)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_2D_SECTION)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_ANCHOR)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_ANCHOR_BUB_TO_GRID)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_ANCHOR_BUB_TO_GRID_MODEL)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_ANCHOR_BUB_TO_GRID_RCP)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_ANCHOR_ENT_TO_NODE)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_ANCHOR_EXT_TAG_TO_ENT)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_ANCHOR_TAG_TO_ENT)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_BDG_ELEVLINE_MODEL)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_BDG_ELEVLINE_PLAN)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_BDG_ELEVLINE_RCP)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_BDG_SECTIONLINE_MODEL)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_BDG_SECTIONLINE_PLAN)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_BDG_SECTIONLINE_RCP)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_BDG_SECTION_MODEL)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_BDG_SECTION_SUBDIV)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_CEILING_GRID)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_CEILING_GRID_MODEL)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_CEILING_GRID_RCP)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_CLIP_VOLUME_MODEL)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_CLIP_VOLUME_PLAN)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_CLIP_VOLUME_RESULT)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_CLIP_VOLUME_RESULT_SUBDIV)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_COLUMN_GRID)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_COLUMN_GRID_MODEL)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_COLUMN_GRID_RCP)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_COL_BLOCK)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_COL_CIRCARC2D)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_COL_CONCOINCIDENT)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_COL_CONCONCENTRIC)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_COL_CONEQUALDISTANCE)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_COL_CONMIDPOINT)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_COL_CONNECTOR)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_COL_CONNORMAL)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_COL_CONPARALLEL)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_COL_CONPERPENDICULAR)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_COL_CONSYMMETRIC)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_COL_CONTANGENT)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_COL_DIMANGLE)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_COL_DIMDIAMETER)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_COL_DIMDISTANCE)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_COL_DIMLENGTH)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_COL_DIMMAJORRADIUS)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_COL_DIMMINORRADIUS)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_COL_ELLIPARC2D)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_COL_LAYOUTDATA)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_COL_LINE2D)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_COL_MODIFIER_ADD)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_COL_MODIFIER_CUTPLANE)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_COL_MODIFIER_EXTRUSION)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_COL_MODIFIER_GROUP)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_COL_MODIFIER_LOFT)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_COL_MODIFIER_PATH)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_COL_MODIFIER_REVOLVE)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_COL_MODIFIER_SUBTRACT)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_COL_MODIFIER_TRANSITION)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_COL_POINT2D)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_COL_PROFILE)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_COL_WORKPLANE)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_COL_WORKPLANE_REF)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_CONFIG)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_CURTAIN_WALL_LAYOUT_MODEL)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_CURTAIN_WALL_LAYOUT_PLAN)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_CURTAIN_WALL_UNIT_MODEL)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_CURTAIN_WALL_UNIT_PLAN)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_DCM_DIMRADIUS)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_DISPLAYTHEME)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_DOOR_ELEV)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_DOOR_MODEL)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_DOOR_NOMINAL)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_DOOR_PLAN)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_DOOR_PLAN_50)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_DOOR_PLAN_HEKTO)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_DOOR_RCP)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_DOOR_THRESHOLD_PLAN)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_DOOR_THRESHOLD_SYMBOL_PLAN)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_EDITINPLACEPROFILE)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_ENT_REF)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_LAYOUT_CURVE)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_LAYOUT_GRID2D)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_LAYOUT_GRID3D)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_MASKBLOCK_REF)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_MASKBLOCK_REF_RCP)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_MASS_ELEM_MODEL)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_MASS_ELEM_RCP)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_MASS_ELEM_SCHEM)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_MASS_GROUP_MODEL)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_MASS_GROUP_PLAN)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_MASS_GROUP_RCP)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_MATERIAL)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_MVBLOCK_REF)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_MVBLOCK_REF_MODEL)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_MVBLOCK_REF_RCP)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_OPENING)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_OPENING_MODEL)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_POLYGON_MODEL)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_POLYGON_TRUECOLOUR)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_RAILING_MODEL)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_RAILING_PLAN)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_RAILING_PLAN_100)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_RAILING_PLAN_50)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_ROOFSLAB_MODEL)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_ROOFSLAB_PLAN)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_ROOF_MODEL)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_ROOF_PLAN)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_ROOF_RCP)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_SCHEDULE_TABLE)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_SET)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_SLAB_MODEL)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_SLAB_PLAN)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_SLICE)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_SPACE_DECOMPOSED)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_SPACE_MODEL)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_SPACE_PLAN)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_SPACE_RCP)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_SPACE_VOLUME)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_STAIR_MODEL)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_STAIR_PLAN)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_STAIR_PLAN_100)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_STAIR_PLAN_50)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_STAIR_PLAN_OVERLAPPING)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_STAIR_RCP)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_WALL_GRAPH)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_WALL_MODEL)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_WALL_PLAN)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_WALL_RCP)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_WALL_SCHEM)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_WINDOWASSEMBLY_SILL_PLAN)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_WINDOW_ASSEMBLY_MODEL)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_WINDOW_ASSEMBLY_PLAN)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_WINDOW_ELEV)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_WINDOW_MODEL)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_WINDOW_NOMINAL)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_WINDOW_PLAN)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_WINDOW_PLAN_100)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_WINDOW_PLAN_50)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_WINDOW_RCP)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_WINDOW_SILL_PLAN)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_REP_ZONE)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_ROPS_RAILING_PLAN_100)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_ROPS_RAILING_PLAN_50)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_ROPS_STAIR_PLAN_100)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DISP_ROPS_STAIR_PLAN_50)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_DOOR_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_ENDCAP_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_FRAME_DEF)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_LAYERKEY_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_LIST_DEF)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_MASKBLOCK_DEF)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_MASS_ELEM_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_MATERIAL_DEF)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_MVBLOCK_DEF)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_MVBLOCK_REF)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_NOTIFICATION_TRACKER)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_POLYGON)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_POLYGON_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_PROPERTY_SET_DEF)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_RAILING_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_REFEDIT_STATUS_TRACKER)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_ROOFSLABEDGE_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_ROOFSLAB_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_SCHEDULE_DATA_FORMAT)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_SLABEDGE_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_SLAB_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_SPACE_STYLES)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_STAIR_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_STAIR_WINDER_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_STAIR_WINDER_TYPE_BALANCED)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_STAIR_WINDER_TYPE_MANUAL)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_STAIR_WINDER_TYPE_SINGLE_POINT)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_VARS_AECBBLDSRV)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_VARS_ARCHBASE)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_VARS_DWG_SETUP)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_VARS_MUNICH)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_VARS_STRUCTUREBASE)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_WALLMOD_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_WALL_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_WINDOW_ASSEMBLY_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (AEC_WINDOW_STYLE)
// CAST_DWG_OBJECT_TO_OBJECT (ALIGNMENTGRIPENTITY)
// CAST_DWG_OBJECT_TO_OBJECT (AMCONTEXTMGR)
// CAST_DWG_OBJECT_TO_OBJECT (AMDTADMENUSTATE)
// CAST_DWG_OBJECT_TO_OBJECT (AMDTAMMENUSTATE)
// CAST_DWG_OBJECT_TO_OBJECT (AMDTBROWSERDBTAB)
// CAST_DWG_OBJECT_TO_OBJECT (AMDTDMMENUSTATE)
// CAST_DWG_OBJECT_TO_OBJECT (AMDTEDGESTANDARDDIN)
// CAST_DWG_OBJECT_TO_OBJECT (AMDTEDGESTANDARDDIN13715)
// CAST_DWG_OBJECT_TO_OBJECT (AMDTEDGESTANDARDISO)
// CAST_DWG_OBJECT_TO_OBJECT (AMDTEDGESTANDARDISO13715)
// CAST_DWG_OBJECT_TO_OBJECT (AMDTFORMULAUPDATEDISPATCHER)
// CAST_DWG_OBJECT_TO_OBJECT (AMDTINTERNALREACTOR)
// CAST_DWG_OBJECT_TO_OBJECT (AMDTMCOMMENUSTATE)
// CAST_DWG_OBJECT_TO_OBJECT (AMDTMENUSTATEMGR)
// CAST_DWG_OBJECT_TO_OBJECT (AMDTNOTE)
// CAST_DWG_OBJECT_TO_OBJECT (AMDTNOTETEMPLATEDB)
// CAST_DWG_OBJECT_TO_OBJECT (AMDTSECTIONSYM)
// CAST_DWG_OBJECT_TO_OBJECT (AMDTSECTIONSYMLABEL)
// CAST_DWG_OBJECT_TO_OBJECT (AMDTSYSATTR)
// CAST_DWG_OBJECT_TO_OBJECT (AMGOBJPROPCFG)
// CAST_DWG_OBJECT_TO_OBJECT (AMGSETTINGSOBJ)
// CAST_DWG_OBJECT_TO_OBJECT (AMIMASTER)
// CAST_DWG_OBJECT_TO_OBJECT (AM_DRAWING_MGR)
// CAST_DWG_OBJECT_TO_OBJECT (AM_DWGMGR_NAME)
// CAST_DWG_OBJECT_TO_OBJECT (AM_DWG_DOCUMENT)
// CAST_DWG_OBJECT_TO_OBJECT (AM_DWG_SHEET)
// CAST_DWG_OBJECT_TO_OBJECT (AM_VIEWDIMPARMAP)
// CAST_DWG_OBJECT_TO_OBJECT (BINRECORD)
// CAST_DWG_OBJECT_TO_OBJECT (CAMSCATALOGAPPOBJECT)
// CAST_DWG_OBJECT_TO_OBJECT (CAMSSTRUCTBTNSTATE)
// CAST_DWG_OBJECT_TO_OBJECT (CATALOGSTATE)
// CAST_DWG_OBJECT_TO_OBJECT (CBROWSERAPPOBJECT)
// CAST_DWG_OBJECT_TO_OBJECT (DEPMGR)
// CAST_DWG_OBJECT_TO_OBJECT (DMBASEELEMENT)
// CAST_DWG_OBJECT_TO_OBJECT (DMDEFAULTSTYLE)
// CAST_DWG_OBJECT_TO_OBJECT (DMLEGEND)
// CAST_DWG_OBJECT_TO_OBJECT (DMMAP)
// CAST_DWG_OBJECT_TO_OBJECT (DMMAPMANAGER)
// CAST_DWG_OBJECT_TO_OBJECT (DMSTYLECATEGORY)
// CAST_DWG_OBJECT_TO_OBJECT (DMSTYLELIBRARY)
// CAST_DWG_OBJECT_TO_OBJECT (DMSTYLEREFERENCE)
// CAST_DWG_OBJECT_TO_OBJECT (DMSTYLIZEDENTITIESTABLE)
// CAST_DWG_OBJECT_TO_OBJECT (DMSURROGATESTYLESETS)
// CAST_DWG_OBJECT_TO_OBJECT (DM_PLACEHOLDER)
// CAST_DWG_OBJECT_TO_OBJECT (EXACTERMXREFMAP)
// CAST_DWG_OBJECT_TO_OBJECT (EXACXREFPANELOBJECT)
// CAST_DWG_OBJECT_TO_OBJECT (EXPO_NOTIFYBLOCK)
// CAST_DWG_OBJECT_TO_OBJECT (EXPO_NOTIFYHALL)
// CAST_DWG_OBJECT_TO_OBJECT (EXPO_NOTIFYPILLAR)
// CAST_DWG_OBJECT_TO_OBJECT (EXPO_NOTIFYSTAND)
// CAST_DWG_OBJECT_TO_OBJECT (EXPO_NOTIFYSTANDNOPOLY)
// CAST_DWG_OBJECT_TO_OBJECT (FLIPACTIONENTITY)
// CAST_DWG_OBJECT_TO_OBJECT (GSMANAGER)
// CAST_DWG_OBJECT_TO_OBJECT (IRD_DSC_DICT)
// CAST_DWG_OBJECT_TO_OBJECT (IRD_DSC_RECORD)
// CAST_DWG_OBJECT_TO_OBJECT (IRD_OBJ_RECORD)
// CAST_DWG_OBJECT_TO_OBJECT (MAPFSMRVOBJECT)
// CAST_DWG_OBJECT_TO_OBJECT (MAPGWSUNDOOBJECT)
// CAST_DWG_OBJECT_TO_OBJECT (MAPIAMMOUDLE)
// CAST_DWG_OBJECT_TO_OBJECT (MAPMETADATAOBJECT)
// CAST_DWG_OBJECT_TO_OBJECT (MAPRESOURCEMANAGEROBJECT)
// CAST_DWG_OBJECT_TO_OBJECT (MOVEACTIONENTITY)
// CAST_DWG_OBJECT_TO_OBJECT (McDbContainer2)
// CAST_DWG_OBJECT_TO_OBJECT (McDbMarker)
// CAST_DWG_OBJECT_TO_OBJECT (NAMEDAPPL)
// CAST_DWG_OBJECT_TO_OBJECT (NEWSTDPARTPARLIST)
// CAST_DWG_OBJECT_TO_OBJECT (NPOCOLLECTION)
// CAST_DWG_OBJECT_TO_OBJECT (OBJCLONER)
// CAST_DWG_OBJECT_TO_OBJECT (PARAMMGR)
// CAST_DWG_OBJECT_TO_OBJECT (PARAMSCOPE)
// CAST_DWG_OBJECT_TO_OBJECT (PILLAR)
// CAST_DWG_OBJECT_TO_OBJECT (RAPIDRTRENDERENVIRONMENT)
// CAST_DWG_OBJECT_TO_OBJECT (ROTATEACTIONENTITY)
// CAST_DWG_OBJECT_TO_OBJECT (SCALEACTIONENTITY)
// CAST_DWG_OBJECT_TO_OBJECT (STDPART2D)
// CAST_DWG_OBJECT_TO_OBJECT (STRETCHACTIONENTITY)
// CAST_DWG_OBJECT_TO_OBJECT (TCH_ARROW)
// CAST_DWG_OBJECT_TO_OBJECT (TCH_AXIS_LABEL)
// CAST_DWG_OBJECT_TO_OBJECT (TCH_BLOCK_INSERT)
// CAST_DWG_OBJECT_TO_OBJECT (TCH_COLUMN)
// CAST_DWG_OBJECT_TO_OBJECT (TCH_DBCONFIG)
// CAST_DWG_OBJECT_TO_OBJECT (TCH_DIMENSION2)
// CAST_DWG_OBJECT_TO_OBJECT (TCH_DRAWINGINDEX)
// CAST_DWG_OBJECT_TO_OBJECT (TCH_HANDRAIL)
// CAST_DWG_OBJECT_TO_OBJECT (TCH_LINESTAIR)
// CAST_DWG_OBJECT_TO_OBJECT (TCH_OPENING)
// CAST_DWG_OBJECT_TO_OBJECT (TCH_RECTSTAIR)
// CAST_DWG_OBJECT_TO_OBJECT (TCH_SLAB)
// CAST_DWG_OBJECT_TO_OBJECT (TCH_SPACE)
// CAST_DWG_OBJECT_TO_OBJECT (TCH_TEXT)
// CAST_DWG_OBJECT_TO_OBJECT (TCH_WALL)
// CAST_DWG_OBJECT_TO_OBJECT (TGrupoPuntos)
// CAST_DWG_OBJECT_TO_OBJECT (VAACIMAGEINVENTORY)
// CAST_DWG_OBJECT_TO_OBJECT (VAACXREFPANELOBJECT)
// CAST_DWG_OBJECT_TO_OBJECT (XREFPANELOBJECT)
#  endif
// clang-format: on
/* End auto-generated content */

/*******************************************************************
 *                FUNCTIONS START HERE ENTITY SPECIFIC               *
 ********************************************************************/

#  ifdef USE_DEPRECATED_API

/** not thread safe. Deprecated.
 *  TODO: replace with walking parents up to the dwg.
 * \deprecated
 */
void
dwg_api_init_version (Dwg_Data *dwg)
{
  dwg_version = (Dwg_Version_Type)dwg->header.version;
  loglevel = dwg->opts & DWG_OPTS_LOGLEVEL;
}

#  endif

/** To access the common DIMENSION fields (only).
 *  There is no generic call dwg_get_DIMENSION, for this you have to
 *  specify the exact DIMENSION_* type.
 */
dwg_ent_dim *
dwg_object_to_DIMENSION (dwg_object *obj)
{
  dwg_ent_dim *ret_obj = NULL;
  if (obj != NULL
      && (obj->fixedtype == DWG_TYPE_DIMENSION_ORDINATE
          || obj->fixedtype == DWG_TYPE_DIMENSION_LINEAR
          || obj->fixedtype == DWG_TYPE_DIMENSION_ALIGNED
          || obj->fixedtype == DWG_TYPE_DIMENSION_ANG3PT
          || obj->fixedtype == DWG_TYPE_DIMENSION_ANG2LN
          || obj->fixedtype == DWG_TYPE_DIMENSION_RADIUS
          || obj->fixedtype == DWG_TYPE_DIMENSION_DIAMETER
          || obj->fixedtype == DWG_TYPE_ARC_DIMENSION))
    {
      ret_obj = obj->tio.entity->tio.DIMENSION_common;
    }
  else
    {
      unsigned int type = obj ? obj->type : 0;
      LOG_ERROR ("invalid %s type: got %u/0x%x", "DIMENSION", type, type);
    }
  return (dwg_ent_dim *)ret_obj;
}

/********************************************************************
 *                    DYNAPI FUNCTIONS                              *
 ********************************************************************/

EXPORT bool
dwg_get_HEADER (const Dwg_Data *restrict dwg, const char *restrict fieldname,
                void *restrict out)
{
#  ifndef HAVE_NONNULL
  if (dwg && fieldname && out)
#  endif
    return dwg_dynapi_header_value (dwg, fieldname, out, NULL);
#  ifndef HAVE_NONNULL
  else
    return false;
#  endif
}

EXPORT bool
dwg_get_HEADER_utf8text (const Dwg_Data *restrict dwg,
                         const char *restrict fieldname, char **restrict textp,
                         int *isnewp)
{
#  ifndef HAVE_NONNULL
  if (dwg && fieldname && textp)
#  endif
    return dwg_dynapi_header_utf8text (dwg, fieldname, textp, isnewp, NULL);
#  ifndef HAVE_NONNULL
  else
    return false;
#  endif
}

EXPORT bool
dwg_set_HEADER (Dwg_Data *restrict dwg, const char *restrict fieldname,
                const void *restrict value)
{
#  ifndef HAVE_NONNULL
  if (dwg && fieldname && value)
#  endif
    return dwg_dynapi_header_set_value (dwg, fieldname, value, 0);
#  ifndef HAVE_NONNULL
  else
    return false;
#  endif
}

EXPORT bool
dwg_set_HEADER_utf8text (Dwg_Data *restrict dwg,
                         const char *restrict fieldname,
                         const char *restrict utf8)
{
#  ifndef HAVE_NONNULL
  if (dwg && fieldname && utf8)
#  endif
    return dwg_dynapi_header_set_value (dwg, fieldname, utf8, 0);
#  ifndef HAVE_NONNULL
  else
    return false;
#  endif
}

EXPORT bool
dwg_get_ENTITY_common (Dwg_Object_Entity *restrict obj,
                       const char *restrict fieldname, void *restrict out)
{
#  ifndef HAVE_NONNULL
  if (obj && fieldname && out)
#  endif
    return dwg_dynapi_common_value (obj, fieldname, out, NULL);
#  ifndef HAVE_NONNULL
  else
    return false;
#  endif
}

EXPORT bool
dwg_set_ENTITY_common (Dwg_Object_Entity *restrict obj,
                       const char *restrict fieldname,
                       const void *restrict value)
{
#  ifndef HAVE_NONNULL
  if (obj && fieldname && value)
#  endif
    return dwg_dynapi_common_set_value (obj, fieldname, value, 0);
#  ifndef HAVE_NONNULL
  else
    return false;
#  endif
}

EXPORT bool
dwg_get_OBJECT_common (Dwg_Object_Object *restrict obj,
                       const char *restrict fieldname, void *restrict out)
{
#  ifndef HAVE_NONNULL
  if (obj && fieldname && out)
#  endif
    return dwg_dynapi_common_value (obj, fieldname, out, NULL);
#  ifndef HAVE_NONNULL
  else
    return false;
#  endif
}

EXPORT bool
dwg_set_OBJECT_common (Dwg_Object_Object *restrict obj,
                       const char *restrict fieldname,
                       const void *restrict value)
{
#  ifndef HAVE_NONNULL
  if (obj && fieldname && value)
#  endif
    return dwg_dynapi_common_set_value (obj, fieldname, value, 0);
#  ifndef HAVE_NONNULL
  else
    return false;
#  endif
}

EXPORT bool
dwg_get_ENTITY_common_utf8text (Dwg_Object_Entity *restrict obj,
                                const char *restrict fieldname,
                                char **restrict textp, int *isnewp)
{
#  ifndef HAVE_NONNULL
  if (obj && fieldname && textp)
#  endif
    return dwg_dynapi_common_utf8text (obj, fieldname, textp, isnewp, NULL);
#  ifndef HAVE_NONNULL
  else
    return false;
#  endif
}

EXPORT bool
dwg_set_ENTITY_common_utf8text (Dwg_Object_Entity *restrict obj,
                                const char *restrict fieldname,
                                const char *restrict utf8)
{
#  ifndef HAVE_NONNULL
  if (obj && fieldname && utf8)
#  endif
    return dwg_dynapi_common_set_value (obj, fieldname, utf8, 0);
#  ifndef HAVE_NONNULL
  else
    return false;
#  endif
}

EXPORT bool
dwg_get_OBJECT_common_utf8text (Dwg_Object_Object *restrict obj,
                                const char *restrict fieldname,
                                char **restrict textp, int *isnewp)
{
#  ifndef HAVE_NONNULL
  if (obj && fieldname && textp)
#  endif
    return dwg_dynapi_common_utf8text (obj, fieldname, textp, isnewp, NULL);
#  ifndef HAVE_NONNULL
  else
    return false;
#  endif
}

EXPORT bool
dwg_set_OBJECT_common_utf8text (Dwg_Object_Object *restrict obj,
                                const char *restrict fieldname,
                                const char *restrict utf8)
{
#  ifndef HAVE_NONNULL
  if (obj && fieldname && utf8)
#  endif
    return dwg_dynapi_common_set_value (obj, fieldname, utf8, 1);
#  ifndef HAVE_NONNULL
  else
    return false;
#  endif
}

/********************************************************************
 *                FUNCTIONS TYPE SPECIFIC                            *
 *********************************************************************/

/* Should we accept dwg and entities? or add dwg_header_get_TYPE */
EXPORT dwg_point_2d *
dwg_ent_get_POINT2D (const void *restrict _obj, const char *restrict fieldname)
{
#  ifndef HAVE_NONNULL
  if (!_obj || !fieldname)
    return NULL;
#  endif
  {
    dwg_point_2d *point;
    Dwg_DYNAPI_field field = { 0 };
    int error;
    const Dwg_Object *obj
        = (const Dwg_Object *)dwg_obj_generic_to_object (_obj, &error);
    if (!obj || !obj->name)
      return NULL;

    point = (dwg_point_2d *)CALLOC (1, sizeof (dwg_point_2d));
    if (!dwg_dynapi_entity_value ((void *)_obj, obj->name, fieldname, &point,
                                  &field))
      {
        FREE (point);
        return NULL;
      }
    if (strEQc (field.type, "2RD") || strEQc (field.type, "2BD")
        || strEQc (field.type, "2DPOINT"))
      {
        return point;
      }
    else
      {
        FREE (point);
        LOG_ERROR (
            "%s.%s has type %s, which is not a POINT2D (2RD,2BD,2DPOINT)",
            obj->name, fieldname, field.type)
        return NULL;
      }
  }
}

EXPORT bool
dwg_ent_set_POINT2D (void *restrict _obj, const char *restrict fieldname,
                     const dwg_point_2d *point)
{
#  ifndef HAVE_NONNULL
  if (!_obj || !fieldname || !point)
    return false;
#  endif
  {
    dwg_point_2d dummy;
    Dwg_DYNAPI_field field = { 0 };
    int error;
    const Dwg_Object *obj
        = (const Dwg_Object *)dwg_obj_generic_to_object (_obj, &error);
    if (!obj || !obj->name)
      return false;

    if (!dwg_dynapi_entity_value ((void *)_obj, obj->name, fieldname, &dummy,
                                  &field))
      {
        return false;
      }
    if (strEQc (field.type, "2RD") || strEQc (field.type, "2BD")
        || strEQc (field.type, "2DPOINT"))
      {
        return dwg_dynapi_entity_set_value ((void *)_obj, obj->name, fieldname,
                                            point, 0);
      }
    else
      {
        LOG_ERROR (
            "%s.%s has type %s, which is not a POINT2D (2RD,2BD,2DPOINT)",
            obj->name, fieldname, field.type)
        return false;
      }
  }
}

EXPORT dwg_point_3d *
dwg_ent_get_POINT3D (const void *restrict _obj, const char *restrict fieldname)
{
#  ifndef HAVE_NONNULL
  if (!_obj || !fieldname)
    return NULL;
#  endif
  {
    dwg_point_3d *point;
    Dwg_DYNAPI_field field = { 0 };
    int error;
    const Dwg_Object *obj
        = (const Dwg_Object *)dwg_obj_generic_to_object (_obj, &error);
    if (!obj || !obj->name)
      return NULL;

    point = (dwg_point_3d *)CALLOC (1, sizeof (dwg_point_3d));
    if (!dwg_dynapi_entity_value ((void *)_obj, obj->name, fieldname, &point,
                                  &field))
      {
        FREE (point);
        return NULL;
      }
    if (strEQc (field.type, "3RD") || strEQc (field.type, "3BD")
        || strEQc (field.type, "BE") || strEQc (field.type, "3DPOINT"))
      {
        return point;
      }
    else
      {
        FREE (point);
        LOG_ERROR (
            "%s.%s has type %s, which is not a POINT3D (3RD,3BD,BE,3DPOINT)",
            obj->name, fieldname, field.type)
        return NULL;
      }
  }
}

EXPORT bool
dwg_ent_set_POINT3D (void *restrict _obj, const char *restrict fieldname,
                     const dwg_point_3d *point)
{
#  ifndef HAVE_NONNULL
  if (!_obj || !fieldname || !point)
    return false;
#  endif
  {
    dwg_point_3d dummy;
    Dwg_DYNAPI_field field = { 0 };
    int error;
    const Dwg_Object *obj
        = (const Dwg_Object *)dwg_obj_generic_to_object (_obj, &error);
    if (!obj || !obj->name)
      return false;

    if (!dwg_dynapi_entity_value ((void *)_obj, obj->name, fieldname, &dummy,
                                  &field))
      return false;
    if (strEQc (field.type, "3RD") || strEQc (field.type, "3BD")
        || strEQc (field.type, "BE") || strEQc (field.type, "3DPOINT"))
      {
        return dwg_dynapi_entity_set_value ((void *)_obj, obj->name, fieldname,
                                            point, 0);
      }
    else
      {
        LOG_ERROR (
            "%s.%s has type %s, which is not a POINT3D (3RD,3BD,BE,3DPOINT)",
            obj->name, fieldname, field.type)
        return false;
      }
  }
}

EXPORT char *
dwg_ent_get_STRING (const void *restrict _obj, const char *restrict fieldname)
{
#  ifndef HAVE_NONNULL
  if (!_obj || !fieldname)
    return NULL;
#  endif
  {
    char *str;
    Dwg_DYNAPI_field field = { 0 };
    int error;
    const Dwg_Object *obj
        = (const Dwg_Object *)dwg_obj_generic_to_object (_obj, &error);
    if (!obj || !obj->name)
      return NULL;

    if (!dwg_dynapi_entity_value ((void *)_obj, obj->name, fieldname, &str,
                                  &field))
      return NULL;
    if (field.is_string || strEQc (field.type, "TF"))
      {
        return str;
      }
    else
      {
        LOG_ERROR ("%s.%s has type %s, which is not a STRING (T,TV,TU,TF)",
                   obj->name, fieldname, field.type)
        return NULL;
      }
  }
}

// convert string to UTF-8
EXPORT char *
dwg_ent_get_UTF8 (const void *restrict _obj, const char *restrict fieldname,
                  int *isnewp)
{
  if (isnewp)
    *isnewp = 0;
#  ifndef HAVE_NONNULL
  if (!_obj || !fieldname)
    return NULL;
#  endif
  {
    char *str;
    Dwg_DYNAPI_field field = { 0 };
    int error;
    const Dwg_Object *obj
        = (const Dwg_Object *)dwg_obj_generic_to_object (_obj, &error);
    if (!obj || !obj->name)
      return NULL;

    if (!dwg_dynapi_entity_utf8text ((void *)_obj, obj->name, fieldname, &str,
                                     isnewp, &field))
      return NULL;
    if (field.is_string || strEQc (field.type, "TF"))
      {
        return str;
      }
    else
      {
        LOG_ERROR ("%s.%s has type %s, which is not a STRING (T,TV,TU,TF)",
                   obj->name, fieldname, field.type)
        return NULL;
      }
  }
}

EXPORT bool
dwg_ent_set_STRING (void *restrict _obj, const char *restrict fieldname,
                    const char *restrict str)
{
#  ifndef HAVE_NONNULL
  if (!_obj || !fieldname || !str)
    return false;
#  endif
  {
    char *dummy;
    Dwg_DYNAPI_field field = { 0 };
    int error;
    const Dwg_Object *obj
        = (const Dwg_Object *)dwg_obj_generic_to_object (_obj, &error);
    if (!obj || !obj->name)
      return false;

    if (!dwg_dynapi_entity_value ((void *)_obj, obj->name, fieldname, &dummy,
                                  &field))
      return false;
    if (field.is_string || strEQc (field.type, "TF"))
      {
        return dwg_dynapi_entity_set_value ((void *)_obj, obj->name, fieldname,
                                            str, 0);
      }
    else
      {
        LOG_ERROR ("%s.%s has type %s, which is not a STRING (T,TV,TU,TF)",
                   obj->name, fieldname, field.type)
        return false;
      }
  }
}

EXPORT bool
dwg_ent_set_UTF8 (void *restrict _obj, const char *restrict fieldname,
                  const char *restrict utf8text)
{
#  ifndef HAVE_NONNULL
  if (!_obj || !fieldname || !utf8text)
    return false;
#  endif
  {
    char *dummy;
    Dwg_DYNAPI_field field = { 0 };
    int error;
    const Dwg_Object *obj
        = (const Dwg_Object *)dwg_obj_generic_to_object (_obj, &error);
    if (!obj || !obj->name)
      return false;

    if (!dwg_dynapi_entity_value ((void *)_obj, obj->name, fieldname, &dummy,
                                  &field))
      return false;
    if (field.is_string || strEQc (field.type, "TF"))
      {
        return dwg_dynapi_entity_set_value ((void *)_obj, obj->name, fieldname,
                                            utf8text, 1);
      }
    else
      {
        LOG_ERROR ("%s.%s has type %s, which is not a STRING (T,TV,TU,TF)",
                   obj->name, fieldname, field.type)
        return false;
      }
  }
}

EXPORT BITCODE_BD
dwg_ent_get_REAL (const void *restrict _obj, const char *restrict fieldname)
{
#  ifndef HAVE_NONNULL
  if (!_obj || !fieldname)
    return 0.0;
#  endif
  {
    BITCODE_BD num;
    Dwg_DYNAPI_field field = { 0 };
    int error;
    const Dwg_Object *obj
        = (const Dwg_Object *)dwg_obj_generic_to_object (_obj, &error);
    if (!obj || !obj->name)
      return 0.0;

    if (!dwg_dynapi_entity_value ((void *)_obj, obj->name, fieldname, &num,
                                  &field))
      return 0.0;
    if (strEQc (field.type, "RD") || strEQc (field.type, "BD"))
      {
        return num;
      }
    else
      {
        LOG_ERROR ("%s.%s has type %s, which is not a REAL (RD,BD)", fieldname,
                   obj->name, field.type)
        return 0.0;
      }
  }
}

EXPORT bool
dwg_ent_set_REAL (void *restrict _obj, const char *restrict fieldname,
                  const BITCODE_BD num)
{
#  ifndef HAVE_NONNULL
  if (!_obj || !fieldname)
    return false;
#  endif
  {
    BITCODE_BD dummy;
    Dwg_DYNAPI_field field = { 0 };
    int error;
    const Dwg_Object *obj
        = (const Dwg_Object *)dwg_obj_generic_to_object (_obj, &error);
    if (!obj || !obj->name)
      return false;
    if (!dwg_dynapi_entity_value ((void *)_obj, obj->name, fieldname, &dummy,
                                  &field))
      return false;
    if (strEQc (field.type, "RD") || strEQc (field.type, "BD"))
      {
        return dwg_dynapi_entity_set_value ((void *)_obj, obj->name, fieldname,
                                            &num, 0);
      }
    else
      {
        LOG_ERROR ("%s.%s has type %s, which is not a REAL (RD,BD)", fieldname,
                   obj->name, field.type)
        return false;
      }
  }
}

EXPORT BITCODE_BS
dwg_ent_get_INT16 (const void *restrict _obj, const char *restrict fieldname)
{
#  ifndef HAVE_NONNULL
  if (!_obj || !fieldname)
    return 0;
#  endif
  {
    BITCODE_BS num;
    Dwg_DYNAPI_field field = { 0 };
    int error;
    const Dwg_Object *obj
        = (const Dwg_Object *)dwg_obj_generic_to_object (_obj, &error);
    if (!obj || !obj->name)
      return 0;

    if (!dwg_dynapi_entity_value ((void *)_obj, obj->name, fieldname, &num,
                                  &field))
      return 0;
    if (strEQc (field.type, "RS") || strEQc (field.type, "BS"))
      {
        return num;
      }
    else
      {
        LOG_ERROR ("%s.%s has type %s, which is not a INT16 (RS,BS)",
                   obj->name, fieldname, field.type)
        return 0;
      }
  }
}

EXPORT bool
dwg_ent_set_INT16 (void *restrict _obj, const char *restrict fieldname,
                   const BITCODE_BS num)
{
#  ifndef HAVE_NONNULL
  if (!_obj || !fieldname)
    return false;
#  endif
  {
    BITCODE_BS dummy;
    Dwg_DYNAPI_field field = { 0 };
    int error;
    const Dwg_Object *obj
        = (const Dwg_Object *)dwg_obj_generic_to_object (_obj, &error);
    if (!obj || !obj->name)
      return false;

    if (!dwg_dynapi_entity_value ((void *)_obj, obj->name, fieldname, &dummy,
                                  &field))
      return false;
    if (strEQc (field.type, "RS") || strEQc (field.type, "BS"))
      {
        return dwg_dynapi_entity_set_value ((void *)_obj, obj->name, fieldname,
                                            &num, 0);
      }
    else
      {
        LOG_ERROR ("%s.%s has type %s, which is not a INT16 (RS,BS)",
                   obj->name, fieldname, field.type)
        return false;
      }
  }
}

EXPORT BITCODE_BL
dwg_ent_get_INT32 (const void *restrict _obj, const char *restrict fieldname)
{
#  ifndef HAVE_NONNULL
  if (!_obj || !fieldname)
    return 0;
#  endif
  {
    BITCODE_BL num;
    Dwg_DYNAPI_field field = { 0 };
    int error;
    const Dwg_Object *obj
        = (const Dwg_Object *)dwg_obj_generic_to_object (_obj, &error);
    if (!obj || !obj->name)
      return 0;

    if (!dwg_dynapi_entity_value ((void *)_obj, obj->name, fieldname, &num,
                                  &field))
      return 0;
    if (strEQc (field.type, "RL") || strEQc (field.type, "BL")
        || strEQc (field.type, "MS"))
      {
        return num;
      }
    else
      {
        LOG_ERROR ("%s.%s has type %s, which is not a INT32 (RL,BL,MS)",
                   obj->name, fieldname, field.type)
        return 0;
      }
  }
}

EXPORT bool
dwg_ent_set_INT32 (void *restrict _obj, const char *restrict fieldname,
                   const BITCODE_BL num)
{
#  ifndef HAVE_NONNULL
  if (!_obj || !fieldname)
    return false;
#  endif
  {
    BITCODE_BL dummy;
    Dwg_DYNAPI_field field = { 0 };
    int error;
    const Dwg_Object *obj
        = (const Dwg_Object *)dwg_obj_generic_to_object (_obj, &error);
    if (!obj || !obj->name)
      return false;

    if (!dwg_dynapi_entity_value ((void *)_obj, obj->name, fieldname, &dummy,
                                  &field))
      return false;
    if (strEQc (field.type, "RL") || strEQc (field.type, "BL")
        || strEQc (field.type, "MS"))
      {
        return dwg_dynapi_entity_set_value ((void *)_obj, obj->name, fieldname,
                                            &num, 0);
      }
    else
      {
        LOG_ERROR ("%s.%s has type %s, which is not a INT32 (RL,BL,MS)",
                   obj->name, fieldname, field.type)
        return false;
      }
  }
}

#  ifdef USE_DEPRECATED_API

/*******************************************************************
 *                    FUNCTIONS FOR CIRCLE ENTITY                    *
 ********************************************************************/

/** Returns the _dwg_entity_CIRCLE::center, DXF 10.
  \code Usage: dwg_ent_circle_get_center(circle, &point, &error);
  \encode
  \param[in]  circle  dwg_ent_circle*
  \param[out] point   dwg_point_3d*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_circle_get_center (const dwg_ent_circle *restrict circle,
                           dwg_point_3d *restrict point, int *restrict error)
{
  if (circle
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = circle->center.x;
      point->y = circle->center.y;
      point->z = circle->center.z;
    }
  else
    {
      LOG_ERROR ("%s: empty point or circle", __FUNCTION__)
      *error = 1;
    }
}

/** Sets the _dwg_entity_CIRCLE::center, DXF 10.
  \code Usage: dwg_ent_circle_set_center(circle, &point, &error);
  \endcode
  \param[in,out] circle dwg_ent_circle*
  \param[in]     point  dwg_point_3d*
  \param[out]    error  set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_circle_set_center (dwg_ent_circle *restrict circle,
                           const dwg_point_3d *restrict point,
                           int *restrict error)
{
  if (circle
#    ifndef HAVE_NONNULL
      && point
#    endif
      && dwg_dynapi_entity_set_value (circle, "CIRCLE", "center", point, 0))
    *error = 0;
  else
    {
      LOG_ERROR ("%s: empty point or circle", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the _dwg_entity_CIRCLE::radius, DXF 40.
  \code Usage: double radius = dwg_ent_circle_get_radius(circle, &error);
  \endcode
  \param[in]  circle  dwg_ent_circle*
  \param[out] error   int*, on gcc<3.3 is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT double
dwg_ent_circle_get_radius (const dwg_ent_circle *restrict circle,
                           int *restrict error)
{
  if (circle)
    {
      *error = 0;
      return circle->radius;
    }
  else
    {
      LOG_ERROR ("%s: empty circle", __FUNCTION__)
      *error = 1;
      return bit_nan ();
    }
}

/** Sets the _dwg_entity_CIRCLE::radius, DXF 40.
  \code Usage: dwg_ent_circle_set_radius(circle, radius, &error);
  \endcode
  \param[in,out] circle  dwg_ent_circle*
  \param[in]     radius  double
  \param[out]    error   int*, on gcc<3.3 is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_circle_set_radius (dwg_ent_circle *restrict circle,
                           const double radius, int *restrict error)
{
  if (circle
      && dwg_dynapi_entity_set_value ((void *)circle, "CIRCLE", "radius",
                                      &radius, 0))
    {
      *error = 0;
    }
  else
    {
      LOG_ERROR ("%s: empty circle", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the_dwg_entity_CIRCLE::thickness, DXF 39 (the cylinder height)
  \code Usage: double thickness = dwg_ent_circle_get_thickness(circle, &error);
  \endcode
  \param[in]  circle  dwg_ent_circle*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT double
dwg_ent_circle_get_thickness (const dwg_ent_circle *restrict circle,
                              int *restrict error)
{
  if (circle)
    {
      *error = 0;
      return circle->thickness;
    }
  else
    {
      LOG_ERROR ("%s: empty circle", __FUNCTION__)
      *error = 1;
      return bit_nan ();
    }
}

/** Sets the_dwg_entity_CIRCLE::thickness, DXF 39 (the cylinder height)
  \code Usage: dwg_ent_circle_set_thickness(circle, thickness, &error);
  \endcode
  \param[in,out] circle dwg_ent_circle*
  \param[in]  thickness double
  \param[out] error     int*, set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_circle_set_thickness (dwg_ent_circle *restrict circle,
                              const double thickness, int *restrict error)
{
  if (circle)
    {
      *error = 0;
      circle->thickness = thickness;
    }
  else
    {
      LOG_ERROR ("%s: empty circle", __FUNCTION__)
      *error = 1;
    }
}

/** Sets the_dwg_entity_CIRCLE::extrusion vector, DXF 210
  \code Usage: dwg_ent_circle_set_extrusion(circle, &vector, &error);
  \endcode
  \param[in,out] circle dwg_ent_circle*
  \param[out] vector  dwg_point_3d*
  \param[out] error   int*, set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_circle_set_extrusion (dwg_ent_circle *restrict circle,
                              const dwg_point_3d *restrict vector,
                              int *restrict error)
{
  if (circle
#    ifndef HAVE_NONNULL
      && vector
#    endif
      && dwg_dynapi_entity_set_value ((void *)circle, "CIRCLE", "extrusion",
                                      vector, 0))
    {
      *error = 0;
    }
  else
    {
      LOG_ERROR ("%s: empty vector or circle", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the _dwg_entity_CIRCLE::extrusion vector, DXF 210
  \code Usage: dwg_ent_circle_get_extrusion(circle, &point, &error);
  \endcode
  \param[in]  circle dwg_ent_circle*
  \param[out] vector dwg_point_3d*
  \param[out] error  int*, is set to 0 for ok, 1 on error
*/
EXPORT void
dwg_ent_circle_get_extrusion (const dwg_ent_circle *restrict circle,
                              dwg_point_3d *restrict vector,
                              int *restrict error)
{
  if (circle
#    ifndef HAVE_NONNULL
      && vector
#    endif
      && dwg_dynapi_entity_value ((void *)circle, "CIRCLE", "extrusion",
                                  vector, NULL))
    {
      *error = 0;
    }
  else
    {
      LOG_ERROR ("%s: empty vector or circle", __FUNCTION__)
      *error = 1;
    }
}

/*******************************************************************
 *                    FUNCTIONS FOR LINE ENTITY                      *
 ********************************************************************/

/** Returns the _dwg_entity_LINE::start point, DXF 10.
  \code Usage: dwg_ent_line_get_start_point(line, &point, &error);
  \endcode
  \param[in]  line    dwg_ent_line*
  \param[out] point   dwg_point_2d*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_line_get_start_point (const dwg_ent_line *restrict line,
                              dwg_point_3d *restrict point,
                              int *restrict error)
{
  if (line
#    ifndef HAVE_NONNULL
      && point
#    endif
      && dwg_dynapi_entity_value ((void *)line, "LINE", "start", point, NULL))
    {
      *error = 0;
    }
  else
    {
      LOG_ERROR ("%s: empty line", __FUNCTION__)
      *error = 1;
    }
}

/** Sets the _dwg_entity_LINE::start point, DXF 10.
  \code Usage: dwg_ent_line_set_start_point(line, point, &error);
  \endcode
  \param[in,out] line dwg_ent_line*
  \param[out] point   dwg_point_2d*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_line_set_start_point (dwg_ent_line *restrict line,
                              const dwg_point_3d *restrict point,
                              int *restrict error)
{
  if (line
#    ifndef HAVE_NONNULL
      && point
#    endif
      && dwg_dynapi_entity_set_value ((void *)line, "LINE", "start", point, 0))
    {
      *error = 0;
    }
  else
    {
      LOG_ERROR ("%s: empty line", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the _dwg_entity_LINE::end point, DXF 11.
  \code Usage: dwg_ent_line_get_end_point(line, &point, &error);
  \endcode
  \param[in]  line    dwg_ent_line*
  \param[out] point   dwg_point_2d*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_line_get_end_point (const dwg_ent_line *restrict line,
                            dwg_point_3d *restrict point, int *restrict error)
{
  if (line
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = line->end.x;
      point->y = line->end.y;
      point->z = line->end.z;
    }
  else
    {
      LOG_ERROR ("%s: empty line", __FUNCTION__)
      *error = 1;
    }
}

/** Sets the _dwg_entity_LINE::end point, DXF 11.
  \code Usage: dwg_ent_line_set_start_end(line, &end_points, &error);
  \endcode
  \param[in,out] line dwg_ent_line*
  \param[out] point   dwg_point_2d*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_line_set_end_point (dwg_ent_line *restrict line,
                            const dwg_point_3d *restrict point,
                            int *restrict error)
{
  if (line
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      line->end.x = point->x;
      line->end.y = point->y;
      line->end.z = point->z;
    }
  else
    {
      LOG_ERROR ("%s: empty line", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the _dwg_entity_LINE::thickness, DXF 39.
  \code Usage: double thickness = dwg_ent_line_get_thickness(line, &error);
  \endcode
  \param[in]  line    dwg_ent_line*
  \param[out] error   int*, is set to 0 for ok, 1 on error
*/
EXPORT double
dwg_ent_line_get_thickness (const dwg_ent_line *restrict line,
                            int *restrict error)
{
  if (line)
    {
      *error = 0;
      return line->thickness;
    }
  else
    {
      LOG_ERROR ("%s: empty line", __FUNCTION__)
      *error = 1;
      return bit_nan ();
    }
}

/** Sets the _dwg_entity_LINE::thickness, DXF 39.
  \code Usage: dwg_ent_line_get_thickness(line, thickness, &error);
  \endcode
  \param[in,out] line   dwg_ent_line*
  \param[out] thickness double
  \param[out] error     int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_line_set_thickness (dwg_ent_line *restrict line,
                            const double thickness, int *restrict error)
{
  if (line)
    {
      *error = 0;
      line->thickness = thickness;
    }
  else
    {
      LOG_ERROR ("%s: empty line", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the _dwg_entity_LINE::extrusion vector, DXF 210.
  \code Usage: dwg_ent_line_get_extrusion(line, &vector, &error);
  \endcode
  \param[in]  line    dwg_ent_line*
  \param[out] vector  dwg_point_3d*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_line_get_extrusion (const dwg_ent_line *restrict line,
                            dwg_point_3d *restrict vector, int *restrict error)
{
  if (line
#    ifndef HAVE_NONNULL
      && vector
#    endif
  )
    {
      *error = 0;
      vector->x = line->extrusion.x;
      vector->y = line->extrusion.y;
      vector->z = line->extrusion.z;
    }
  else
    {
      LOG_ERROR ("%s: empty vector or line", __FUNCTION__)
      *error = 1;
    }
}

/** Sets the _dwg_entity_LINE::extrusion vector, DXF 210.
  \code Usage: dwg_ent_line_set_extrusion(line, vector, &error);
  \endcode
  \param[in,out] line  dwg_ent_line*
  \param[out] vector   dwg_point_3d*
  \param[out] error    int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_line_set_extrusion (dwg_ent_line *restrict line,
                            const dwg_point_3d *restrict vector,
                            int *restrict error)
{
  if (line
#    ifndef HAVE_NONNULL
      && vector
#    endif
  )
    {
      *error = 0;
      line->extrusion.x = vector->x;
      line->extrusion.y = vector->y;
      line->extrusion.z = vector->z;
    }
  else
    {
      LOG_ERROR ("%s: empty vector or line", __FUNCTION__)
      *error = 1;
    }
}

/*******************************************************************
 *                    FUNCTIONS FOR ARC ENTITY                       *
 ********************************************************************/

/** Returns the _dwg_entity_ARC::center, DXF 10.
  \code Usage: dwg_ent_arc_get_center(arc, &point, &error);
  \endcode
  \param[in]   arc    dwg_ent_arc*
  \param[out] dwg_point_3d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_arc_get_center (const dwg_ent_arc *restrict arc,
                        dwg_point_3d *restrict point, int *restrict error)
{
  if (arc
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = arc->center.x;
      point->y = arc->center.y;
      point->z = arc->center.z;
    }
  else
    {
      LOG_ERROR ("%s: empty arc or point", __FUNCTION__)
      *error = 1;
    }
}

/** Sets the _dwg_entity_ARC::center, DXF 10.
  \code Usage: dwg_ent_arc_set_center(arc, &point, &error);
  \endcode
  \param[in,out] arc     dwg_ent_arc*
  \param[out]    center  dwg_point_3d*
  \param[out]    error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_arc_set_center (dwg_ent_arc *restrict arc,
                        const dwg_point_3d *restrict point,
                        int *restrict error)
{
  if (arc
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      arc->center.x = point->x;
      arc->center.y = point->y;
      arc->center.z = point->z;
    }
  else
    {
      LOG_ERROR ("%s: empty arc or point", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the _dwg_entity_ARC::radius, DXF 40.
  \code Usage: double radius = dwg_ent_arc_get_radius(arc, &error);
  \endcode
  \param[in]   arc    dwg_ent_arc*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
double
dwg_ent_arc_get_radius (const dwg_ent_arc *restrict arc, int *restrict error)
{
  if (arc)
    {
      *error = 0;
      return arc->radius;
    }
  else
    {
      LOG_ERROR ("%s: empty arc", __FUNCTION__)
      *error = 1;
      return bit_nan ();
    }
}

/** Sets the _dwg_entity_ARC::radius, DXF 40.
  \code Usage: dwg_ent_arc_set_radius(arc, radius, &error);
  \endcode
  \param[in,out] arc     dwg_ent_arc*
  \param[out]    radius  double
  \param[out]    error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_arc_set_radius (dwg_ent_arc *restrict arc, const double radius,
                        int *restrict error)
{
  if (arc)
    {
      *error = 0;
      arc->radius = radius;
    }
  else
    {
      LOG_ERROR ("%s: empty arc", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the _dwg_entity_ARC::thickness, DXF 39.
  \code Usage: double thickness = dwg_ent_arc_get_thickness(arc, &error);
  \endcode
  \param[in]   arc    dwg_ent_arc*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
double
dwg_ent_arc_get_thickness (const dwg_ent_arc *restrict arc,
                           int *restrict error)
{
  if (arc)
    {
      *error = 0;
      return arc->thickness;
    }
  else
    {
      LOG_ERROR ("%s: empty arc", __FUNCTION__)
      *error = 1;
      return bit_nan ();
    }
}

/** Sets the _dwg_entity_ARC::thickness, DXF 39.
  \code Usage: dwg_ent_arc_get_thickness(arc, thickness, &error);
  \endcode
  \param[in,out] arc       dwg_ent_arc*
  \param[out]    thickness double
  \param[out]    error     int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_arc_set_thickness (dwg_ent_arc *restrict arc, const double thickness,
                           int *restrict error)
{
  if (arc)
    {
      *error = 0;
      arc->thickness = thickness;
    }
  else
    {
      LOG_ERROR ("%s: empty arc", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the _dwg_entity_ARC::extrusion vector, DXF 210.
  \code Usage: dwg_ent_arc_get_extrusion(arc, &vector, &error);
  \endcode
  \param[in]     arc      dwg_ent_arc*
  \param[out]    vector   dwg_point_3d*
  \param[out]    error    int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_arc_get_extrusion (const dwg_ent_arc *restrict arc,
                           dwg_point_3d *restrict vector, int *restrict error)
{
  if (arc
#    ifndef HAVE_NONNULL
      && vector
#    endif
  )
    {
      *error = 0;
      vector->x = arc->extrusion.x;
      vector->y = arc->extrusion.y;
      vector->z = arc->extrusion.z;
    }
  else
    {
      LOG_ERROR ("%s: empty arc or vector", __FUNCTION__)
      *error = 1;
    }
}

/** Sets the _dwg_entity_ARC::extrusion vector, DXF 210.
  \code Usage: dwg_ent_arc_set_extrusion(arc, vector, &error);
  \endcode
  \param[in,out] arc      dwg_ent_arc*
  \param[in]     vector   dwg_point_3d*
  \param[out]    error    int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_arc_set_extrusion (dwg_ent_arc *restrict arc,
                           const dwg_point_3d *restrict vector,
                           int *restrict error)
{
  if (arc
#    ifndef HAVE_NONNULL
      && vector
#    endif
  )
    {
      *error = 0;
      arc->extrusion.x = vector->x;
      arc->extrusion.y = vector->y;
      arc->extrusion.z = vector->z;
    }
  else
    {
      LOG_ERROR ("%s: empty arc or vector", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the _dwg_entity_ARC::start_angle, DXF 50, in radians.
  \code Usage: double start_angle = dwg_ent_arc_get_start_angle(line, &error);
  \endcode
  \param[in]  arc     dwg_ent_arc*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
double
dwg_ent_arc_get_start_angle (const dwg_ent_arc *restrict arc,
                             int *restrict error)
{
  if (arc)
    {
      *error = 0;
      return arc->start_angle;
    }
  else
    {
      LOG_ERROR ("%s: empty arc", __FUNCTION__)
      *error = 1;
      return bit_nan ();
    }
}

/** Sets the _dwg_entity_ARC::start_angle, DXF 50.
  \code Usage: dwg_ent_arc_set_start_angle(arc, angle, &error);
  \endcode
  \param[in,out] arc      dwg_ent_arc*
  \param[in]     angle    double (in radians)
  \param[out]    error    int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_arc_set_start_angle (dwg_ent_arc *restrict arc, const double angle,
                             int *restrict error)
{
  if (arc)
    {
      *error = 0;
      // TODO: normalize to PI? There are some DWG's with 2*PI or 4.623770
      arc->start_angle = angle;
    }
  else
    {
      LOG_ERROR ("%s: empty arc", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the _dwg_entity_ARC::end_angle, DXF 51, in radians.
  \code Usage: double end_angle = dwg_ent_arc_get_end_angle(arc, &error);
  \endcode
  \param[in]  arc     dwg_ent_arc*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
double
dwg_ent_arc_get_end_angle (const dwg_ent_arc *restrict arc,
                           int *restrict error)
{
  if (arc)
    {
      *error = 0;
      return arc->end_angle;
    }
  else
    {
      LOG_ERROR ("%s: empty arc", __FUNCTION__)
      *error = 1;
      return bit_nan ();
    }
}

/** Sets the _dwg_entity_ARC::end_angle, DXF 51.
  \code Usage: dwg_ent_arc_set_end_angle(arc, angle, &error);
  \endcode
  \param[in,out] arc      dwg_ent_arc*
  \param[in]     angle    double (in radians)
  \param[out]    error    int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_arc_set_end_angle (dwg_ent_arc *restrict arc, const double angle,
                           int *restrict error)
{
  if (arc)
    {
      *error = 0;
      // TODO: normalize to PI?
      arc->end_angle = angle;
    }
  else
    {
      LOG_ERROR ("%s: empty arc", __FUNCTION__)
      *error = 1;
    }
}

/*******************************************************************
 *                   FUNCTIONS FOR ELLIPSE ENTITY                    *
 ********************************************************************/

/**  Returns the _dwg_entity_ELLIPSE::center, DXF 10
  \code Usage: dwg_ent_ellipse_get_center(ellipse, &center, &error);
  \endcode
  \param[in]  ellipse dwg_ent_ellipse*
  \param[out] point   dwg_point_3d*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_ellipse_get_center (const dwg_ent_ellipse *restrict ellipse,
                            dwg_point_3d *restrict point, int *restrict error)
{
  if (ellipse
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = ellipse->center.x;
      point->y = ellipse->center.y;
      point->z = ellipse->center.z;
    }
  else
    {
      LOG_ERROR ("%s: empty ellipse or point", __FUNCTION__)
      *error = 1;
    }
}

/**  Sets the _dwg_entity_ELLIPSE::center, DXF 10
  \code Usage: dwg_ent_ellipse_set_center(ellipse, &center &error);
  \endcode
  \param[in,out] ellipse dwg_ent_ellipse*
  \param[in]     point   dwg_point_3d*
  \param[out]    error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_ellipse_set_center (dwg_ent_ellipse *restrict ellipse,
                            const dwg_point_3d *restrict point,
                            int *restrict error)
{
  if (ellipse
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      ellipse->center.x = point->x;
      ellipse->center.y = point->y;
      ellipse->center.z = point->z;
    }
  else
    {
      LOG_ERROR ("%s: empty ellipse or point", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the _dwg_entity_ELLIPSE::sm_axis, DXF 11.
  \code Usage: dwg_ent_ellipse_get_sm_axis(ellipse, &point, &error);
  \endcode
  \param[in]  ellipse dwg_ent_ellipse*
  \param[out] point   dwg_point_3d*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_ellipse_get_sm_axis (const dwg_ent_ellipse *restrict ellipse,
                             dwg_point_3d *restrict point, int *restrict error)
{
  if (ellipse
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = ellipse->sm_axis.x;
      point->y = ellipse->sm_axis.y;
      point->z = ellipse->sm_axis.z;
    }
  else
    {
      LOG_ERROR ("%s: empty ellipse or point", __FUNCTION__)
      *error = 1;
    }
}

/** Sets the _dwg_entity_ELLIPSE::sm_axis, DXF 11.
  \code Usage: dwg_ent_ellipse_set_sm_axis(ellipse, &point, &error);
  \endcode
  \param[in,out] ellipse dwg_ent_ellipse*
  \param[in]  point   dwg_point_3d*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_ellipse_set_sm_axis (dwg_ent_ellipse *restrict ellipse,
                             const dwg_point_3d *restrict point,
                             int *restrict error)
{
  if (ellipse
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      ellipse->sm_axis.x = point->x;
      ellipse->sm_axis.y = point->y;
      ellipse->sm_axis.z = point->z;
    }
  else
    {
      LOG_ERROR ("%s: empty ellipse or point", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the _dwg_entity_ELLIPSE::extrusion vector, DXF 210.
  \code Usage: dwg_ent_ellipse_get_extrusion(ellipse, &ext_points, &error);
  \endcode
  \param[in] ellipse  dwg_ent_ellipse*
  \param[out] vector  dwg_point_3d*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_ellipse_get_extrusion (const dwg_ent_ellipse *restrict ellipse,
                               dwg_point_3d *restrict vector,
                               int *restrict error)
{
  if (ellipse
#    ifndef HAVE_NONNULL
      && vector
#    endif
  )
    {
      *error = 0;
      vector->x = ellipse->extrusion.x;
      vector->y = ellipse->extrusion.y;
      vector->z = ellipse->extrusion.z;
    }
  else
    {
      LOG_ERROR ("%s: empty ellipse or vector", __FUNCTION__)
      *error = 1;
    }
}

/** Sets the _dwg_entity_ELLIPSE::extrusion vector, DXF 210.
  \code Usage: dwg_ent_ellipse_set_extrusion(ellipse, &vector, &error);
  \endcode
  \param[in,out] ellipse dwg_ent_ellipse*
  \param[in]  vector  dwg_point_3d*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_ellipse_set_extrusion (dwg_ent_ellipse *restrict ellipse,
                               const dwg_point_3d *restrict vector,
                               int *restrict error)
{
  if (ellipse
#    ifndef HAVE_NONNULL
      && vector
#    endif
  )
    {
      *error = 0;
      ellipse->extrusion.x = vector->x;
      ellipse->extrusion.y = vector->y;
      ellipse->extrusion.z = vector->z;
    }
  else
    {
      LOG_ERROR ("%s: empty ellipse or vector", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the _dwg_entity_ELLIPSE::axis_ratio, DXF 40.
  \code Usage: double axis_ratio = dwg_ent_ellipse_get_axis_ratio(ellipse,
&error); \endcode \param[in] ellipse dwg_ent_ellipse* \param[out] error   int*,
is set to 0 for ok, 1 on error \deprecated
*/
double
dwg_ent_ellipse_get_axis_ratio (const dwg_ent_ellipse *restrict ellipse,
                                int *restrict error)
{
  if (ellipse)
    {
      *error = 0;
      return ellipse->axis_ratio;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty ellipse", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets the _dwg_entity_ELLIPSE::axis ratio, DXF 40.
  \code Usage: dwg_ent_ellipse_set_axis_ratio(ellipse, axis_ratio, &error);
  \endcode
  \param[in] ellipse dwg_ent_ellipse*
  \param[in] double
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_ellipse_set_axis_ratio (dwg_ent_ellipse *restrict ellipse,
                                const double ratio, int *restrict error)
{
  if (ellipse)
    {
      *error = 0;
      ellipse->axis_ratio = ratio;
    }
  else
    {
      LOG_ERROR ("%s: empty ellipse", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the _dwg_entity_ELLIPSE::start_angle, DXF 41.
  \code Usage: double start_angle = dwg_ent_ellipse_get_start_angle(ellipse,
&error); \endcode \param[in] ellipse dwg_ent_ellipse* \param[out] error   int*,
is set to 0 for ok, 1 on error \deprecated
*/
double
dwg_ent_ellipse_get_start_angle (const dwg_ent_ellipse *restrict ellipse,
                                 int *restrict error)
{
  if (ellipse)
    {
      *error = 0;
      return ellipse->start_angle;
    }
  else
    {
      LOG_ERROR ("%s: empty ellipse", __FUNCTION__)
      *error = 1;
      return bit_nan ();
    }
}

/** Sets the _dwg_entity_ELLIPSE::start angle, DXF 41.
  \code Usage: dwg_ent_ellipse_set_start_angle(ellipse, start_angle, &error);
  \endcode
  \param[in] ellipse dwg_ent_ellipse*
  \param[in] double
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_ellipse_set_start_angle (dwg_ent_ellipse *restrict ellipse,
                                 const double start_angle, int *restrict error)
{
  if (ellipse)
    {
      *error = 0;
      ellipse->start_angle = start_angle;
    }
  else
    {
      LOG_ERROR ("%s: empty ellipse", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the _dwg_entity_ELLIPSE::end_angle, DXF 42.
  \code Usage: double end_angle = dwg_ent_ellipse_get_end_angle(ellipse,
  &error); \endcode \param[in]  ellipse dwg_ent_ellipse* \param[out] error
  int*, is set to 0 for ok, 1 on error \deprecated
*/
double
dwg_ent_ellipse_get_end_angle (const dwg_ent_ellipse *restrict ellipse,
                               int *restrict error)
{
  if (ellipse)
    {
      *error = 0;
      return ellipse->end_angle;
    }
  else
    {
      LOG_ERROR ("%s: empty ellipse", __FUNCTION__)
      *error = 1;
      return bit_nan ();
    }
}

/** Sets the _dwg_entity_ELLIPSE::end_angle, DXF 42.
  \code Usage: dwg_ent_ellipse_set_end_angle(ellipse, end_angle, &error);
  \endcode
  \param[in,out] ellipse    dwg_ent_ellipse*
  \param[in]     end_angle  double
  \param[out]    error      int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_ellipse_set_end_angle (dwg_ent_ellipse *restrict ellipse,
                               const double end_angle, int *restrict error)
{
  if (ellipse)
    {
      *error = 0;
      ellipse->end_angle = end_angle;
    }
  else
    {
      LOG_ERROR ("%s: empty ellipse", __FUNCTION__)
      *error = 1;
    }
}

/*******************************************************************
 *                    FUNCTIONS FOR TEXT ENTITY                      *
 ********************************************************************/

/** Sets the _dwg_entity_TEXT::text string, utf-8 encoded, DXF 1.
  \code Usage: dwg_ent_text_set_text(text, "Hello world", &error);
  \endcode
  \param[in] text dwg_ent_text*
  \param[in] utf-8 string ( char * )
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_text_set_text (dwg_ent_text *restrict ent,
                       const char *restrict text_value, int *restrict error)
{
  Dwg_Data *dwg = dwg_obj_generic_dwg (ent, error);
  if (ent && !error)
    {
      ent->text_value = dwg_add_u8_input (dwg, text_value);
    }
  else
    {
      LOG_ERROR ("%s: empty ent", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the _dwg_entity_TEXT::text string, utf-8 encoded, DXF 1.
  \code Usage: dwg_ent_text_get_text(text, &error);
  \endcode
  \param[in] text dwg_ent_text*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
char *
dwg_ent_text_get_text (const dwg_ent_text *restrict ent, int *restrict error)
{
  if (ent)
    {
      *error = 0;
      if (dwg_version >= R_2007)
        return bit_convert_TU ((BITCODE_TU)ent->text_value);
      else
        return ent->text_value;
    }
  else
    {
      LOG_ERROR ("%s: empty ent", __FUNCTION__)
      *error = 1;
      return NULL;
    }
}

/** Returns the _dwg_entity_TEXT::ins_pt, DXF 10.
  \code Usage: dwg_ent_text_get_insertion_pt(text, &point, &error);
  \endcode
  \param[in] text     dwg_ent_text*
  \param[out] point   dwg_2d_point*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_text_get_insertion_pt (const dwg_ent_text *restrict text,
                               dwg_point_2d *restrict point,
                               int *restrict error)
{
  if (text
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = text->ins_pt.x;
      point->y = text->ins_pt.y;
    }
  else
    {
      LOG_ERROR ("%s: empty text or point", __FUNCTION__)
      *error = 1;
    }
}

/** Sets the _dwg_entity_TEXT::insertion_pt, DXF 10.
  \code Usage: dwg_ent_text_set_insertion_pt(text, &point, &error)
  \endcode
  \param[in,out] text    dwg_ent_text*
  \param[in]     point   dwg_2d_point*
  \param[out]    error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_text_set_insertion_pt (dwg_ent_text *restrict text,
                               const dwg_point_2d *restrict point,
                               int *restrict error)
{
  if (text
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      text->ins_pt.x = point->x;
      text->ins_pt.y = point->y;
    }
  else
    {
      LOG_ERROR ("%s: empty text or point", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the _dwg_entity_TEXT::height, DXF 40. i.e. the fontsize
  \code Usage: double height = dwg_ent_text_get_height(text);
  \endcode
  \param[in]  text    dwg_ent_text*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
double
dwg_ent_text_get_height (const dwg_ent_text *restrict text,
                         int *restrict error)
{
  if (text)
    {
      *error = 0;
      return text->height;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty text", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets the _dwg_entity_TEXT::height, DXF 40.
  \code Usage: dwg_ent_text_set_height(text, 100.0, &error);
  \endcode
  \param[in,out] text    dwg_ent_text*
  \param[in]    height  double
  \param[out]    error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_text_set_height (dwg_ent_text *restrict text, const double height,
                         int *restrict error)
{
  if (text)
    {
      *error = 0;
      text->height = height;
    }
  else
    {
      LOG_ERROR ("%s: empty text", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the _dwg_entity_TEXT::extrusion vector, DXF 210.
  \code dwg_ent_text_get_extrusion(text, &point, &error);
  \endcode
  \param[in]  text   dwg_ent_text*
  \param[out] vector dwg_point_3d*
  \param[out] error  int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_text_get_extrusion (const dwg_ent_text *restrict text,
                            dwg_point_3d *restrict vector, int *restrict error)
{
  if (text
#    ifndef HAVE_NONNULL
      && vector
#    endif
  )
    {
      *error = 0;
      vector->x = text->extrusion.x;
      vector->y = text->extrusion.y;
      vector->z = text->extrusion.z;
    }
  else
    {
      LOG_ERROR ("%s: empty text or vector", __FUNCTION__)
      *error = 1;
    }
}

/** Sets the _dwg_entity_TEXT::extrusion vector, DXF 210.
  \code Usage: dwg_ent_text_set_extrusion(text, &vector, &error);
  \encode
  \param[in,out] text dwg_ent_text*
  \param[in]     vector dwg_point_3d*
  \param[out]    error  int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_text_set_extrusion (dwg_ent_text *restrict text,
                            const dwg_point_3d *restrict vector,
                            int *restrict error)
{
  if (text
#    ifndef HAVE_NONNULL
      && vector
#    endif
  )
    {
      *error = 0;
      text->extrusion.x = vector->x;
      text->extrusion.y = vector->y;
      text->extrusion.z = vector->z;
    }
  else
    {
      LOG_ERROR ("%s: empty text or vector", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the _dwg_entity_TEXT::thickness, DXF 39.
  \code Usage: thickness = dwg_ent_text_get_thickness(text, &error);
  \endcode
  \param[in]  text  dwg_ent_text*
  \param[out] error int*, is set to 0 for ok, 1 on error
  \deprecated
*/
double
dwg_ent_text_get_thickness (const dwg_ent_text *restrict text,
                            int *restrict error)
{
  if (text)
    {
      *error = 0;
      return text->thickness;
    }
  else
    {
      LOG_ERROR ("%s: empty text", __FUNCTION__)
      *error = 1;
      return bit_nan ();
    }
}

/** Sets the _dwg_entity_TEXT::thickness, DXF 39.
  \code Usage: dwg_ent_text_set_thickness(text, 0.0 &error);
  \endcode
  \param[in,out] text      dwg_ent_text*
  \param[in]     thickness double
  \param[out]    error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_text_set_thickness (dwg_ent_text *restrict text,
                            const double thickness, int *restrict error)
{
  if (text)
    {
      *error = 0;
      text->thickness = thickness;
    }
  else
    {
      LOG_ERROR ("%s: empty text", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the _dwg_entity_TEXT::rotation. DXF 50 (radians)
  \code Usage: double rot_ang = dwg_ent_text_get_rotation(text, &error);
  \endcode
  \param[in] text dwg_ent_text*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
double
dwg_ent_text_get_rotation (const dwg_ent_text *restrict text,
                           int *restrict error)
{
  if (text)
    {
      *error = 0;
      return text->rotation;
    }
  else
    {
      LOG_ERROR ("%s: empty text", __FUNCTION__)
      *error = 1;
      return bit_nan ();
    }
}

/** Sets the _dwg_entity_TEXT::rotation angle, in radians, DXF 50.
  \code Usage: dwg_ent_text_set_rotation(text, angle, &error);
  \endcode
  \param[in,out] text   dwg_ent_text*
  \param[in] angle  double
  \param[out] error int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_text_set_rotation (dwg_ent_text *restrict text, const double angle,
                           int *restrict error)
{
  if (text)
    {
      *error = 0;
      text->rotation = angle;
    }
  else
    {
      LOG_ERROR ("%s: empty text", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the _dwg_entity_TEXT::vert_alignment, DXF 73.
  \code Usage: short align = dwg_ent_text_get_vert_alignment(text, &error);
  \endcode
  \param[in] text dwg_ent_text*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \return short(1 = bottom, 2 = middle, 3 = top)
*/
EXPORT BITCODE_BS
dwg_ent_text_get_vert_alignment (const dwg_ent_text *restrict text,
                                 int *restrict error)
{
  if (text)
    {
      *error = 0;
      return text->vert_alignment;
    }
  else
    {
      LOG_ERROR ("%s: empty text", __FUNCTION__)
      *error = 1;
      return -1;
    }
}

/** Sets the _dwg_entity_TEXT::vert_alignment, DXF 73.
  \code Usage: dwg_ent_text_set_vert_alignment(text, angle, &error);
  \endcode
  \param[in] text dwg_ent_text*
  \param[in] alignment  short (1 = bottom, 2 = middle, 3 = top)
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_text_set_vert_alignment (dwg_ent_text *restrict text,
                                 const BITCODE_BS alignment,
                                 int *restrict error)
{
  if (text)
    {
      *error = 0;
      // TODO: validation 1-3
      text->vert_alignment = alignment;
    }
  else
    {
      LOG_ERROR ("%s: empty text", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the _dwg_entity_TEXT::horiz_alignment, DXF 72.
  \code Usage: short align = dwg_ent_text_get_horiz_alignment(text, &error);
  \endcode
  \param[in] text dwg_ent_text*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \return (1 = left, 2 = center, 3 = right)
*/
EXPORT BITCODE_BS
dwg_ent_text_get_horiz_alignment (const dwg_ent_text *restrict text,
                                  int *restrict error)
{
  if (text)
    {
      *error = 0;
      return text->horiz_alignment;
    }
  else
    {
      LOG_ERROR ("%s: empty text", __FUNCTION__)
      *error = 1;
      return 0;
    }
}

/** Sets the _dwg_entity_TEXT::horiz_alignment, DXF 72.
  \code Usage: dwg_ent_text_set_horiz_alignment(text, angle, &error);
  \endcode
  \param[in] text      dwg_ent_text*
  \param[in] alignment short (1 = left, 2 = center, 3 = right)
  \param[out] error    int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_text_set_horiz_alignment (dwg_ent_text *restrict text,
                                  const BITCODE_BS alignment,
                                  int *restrict error)
{
  if (text)
    {
      *error = 0;
      // TODO: validation 1-3
      text->horiz_alignment = alignment;
    }
  else
    {
      LOG_ERROR ("%s: empty text", __FUNCTION__)
      *error = 1;
    }
}

/*******************************************************************
 *                   FUNCTIONS FOR ATTRIB ENTITY                     *
 ********************************************************************/

/** Sets the _dwg_entity_ATTRIB::text_value (utf-8 encoded), DXF 1
  \code Usage: dwg_ent_attrib_set_text(attrib, "Hello world", &error);
  \endcode
  \param[out] attrib     dwg_ent_attrib*
  \param[in]  text_value utf-8 string (char *)
  \param[out] error      int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_attrib_set_text (dwg_ent_attrib *restrict ent,
                         const char *restrict text_value, int *restrict error)
{
  Dwg_Data *dwg = dwg_obj_generic_dwg (ent, error);
  if (ent && !error)
    {
      ent->text_value = dwg_add_u8_input (dwg, text_value);
    }
  else
    {
      LOG_ERROR ("%s: empty attrib", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the _dwg_entity_ATTRIB::text_value (utf-8 encoded), DXF 1.
  \code Usage: char * text_val = dwg_ent_attrib_get_text(attrib, &error);
  \endcode
  \param[in] attrib dwg_ent_attrib*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
char *
dwg_ent_attrib_get_text (const dwg_ent_attrib *restrict ent,
                         int *restrict error)
{
  if (ent)
    {
      *error = 0;
      if (dwg_version >= R_2007)
        return bit_convert_TU ((BITCODE_TU)ent->text_value);
      else
        return ent->text_value;
    }
  else
    {
      LOG_ERROR ("%s: empty attrib", __FUNCTION__)
      *error = 1;
      return NULL;
    }
}

/** Returns the _dwg_entity_ATTRIB::ins_pt, DXF 10.
  \code Usage: dwg_ent_attrib_get_insertion_pt(attrib, &point, &error);
  \endcode
  \param[in] attrib   dwg_ent_attrib*
  \param[out] point   dwg_point_3d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_attrib_get_insertion_pt (const dwg_ent_attrib *restrict attrib,
                                 dwg_point_2d *restrict point,
                                 int *restrict error)
{
  if (attrib
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = attrib->ins_pt.x;
      point->y = attrib->ins_pt.y;
    }
  else
    {
      LOG_ERROR ("%s: empty attrib", __FUNCTION__)
      *error = 1;
    }
}

/** Sets the _dwg_entity_ATTRIB::ins_pt, DXF 10
  \code Usage: dwg_ent_attrib_set_insertion_pt(attrib, &point, &error)
  \endcode
  \param[in] attrib   dwg_ent_attrib*
  \param[in] point    dwg_point_3d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_attrib_set_insertion_pt (dwg_ent_attrib *restrict attrib,
                                 const dwg_point_2d *restrict point,
                                 int *restrict error)
{
  if (attrib
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      attrib->ins_pt.x = point->x;
      attrib->ins_pt.y = point->y;
    }
  else
    {
      LOG_ERROR ("%s: empty attrib", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the _dwg_entity_ATTRIB::height, DXF 40
  \code Usage: double height = dwg_ent_attrib_get_height(attrib, &error);
  \endcode
  \param[in] attrib dwg_ent_attrib*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
double
dwg_ent_attrib_get_height (const dwg_ent_attrib *restrict attrib,
                           int *restrict error)
{
  if (attrib)
    {
      *error = 0;
      return attrib->height;
    }
  else
    {
      LOG_ERROR ("%s: empty attrib", __FUNCTION__)
      *error = 1;
      return bit_nan ();
    }
}

/** Sets the _dwg_entity_ATTRIB::height, DXF 40
  \code Usage: dwg_ent_attrib_set_height(attrib, 100, &error);
  \endcode
  \param[in] attrib   dwg_ent_attrib*
  \param[in] height   double
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_attrib_set_height (dwg_ent_attrib *restrict attrib,
                           const double height, int *restrict error)
{
  if (attrib)
    {
      *error = 0;
      attrib->height = height;
    }
  else
    {
      LOG_ERROR ("%s: empty attrib", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the _dwg_entity_ATTRIB::extrusion vector, DXF 210
  \code Usage: dwg_ent_attrib_get_extrusion(attrib, &point, &error);
  \endcode
  \param[in]  attrib  dwg_ent_attrib*
  \param[out] vector  dwg_point_3d *
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_attrib_get_extrusion (const dwg_ent_attrib *restrict attrib,
                              dwg_point_3d *restrict vector,
                              int *restrict error)
{
  if (attrib
#    ifndef HAVE_NONNULL
      && vector
#    endif
  )
    {
      *error = 0;
      vector->x = attrib->extrusion.x;
      vector->y = attrib->extrusion.y;
      vector->z = attrib->extrusion.z;
    }
  else
    {
      LOG_ERROR ("%s: empty attrib or vector", __FUNCTION__)
      *error = 1;
    }
}

/** Sets the _dwg_entity_ATTRIB::extrusion vector, DXF 210
  \code Usage: dwg_ent_attrib_set_extrusion(attrib, &point, &error);
  \endcode
  \param[out]  attrib  dwg_ent_attrib*
  \param[in] vector  dwg_point_3d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_attrib_set_extrusion (dwg_ent_attrib *restrict attrib,
                              const dwg_point_3d *restrict vector,
                              int *restrict error)
{
  if (attrib
#    ifndef HAVE_NONNULL
      && vector
#    endif
  )
    {
      *error = 0;
      attrib->extrusion.x = vector->x;
      attrib->extrusion.y = vector->y;
      attrib->extrusion.z = vector->z;
    }
  else
    {
      LOG_ERROR ("%s: empty attrib or vector", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the _dwg_entity_ATTRIB::thickness, DXF 39
  \code Usage: double thick = dwg_ent_attrib_get_thickness(attrib, &error);
  \endcode
  \param[in] attrib dwg_ent_attrib*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
double
dwg_ent_attrib_get_thickness (const dwg_ent_attrib *restrict attrib,
                              int *restrict error)
{
  if (attrib)
    {
      *error = 0;
      return attrib->thickness;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty attrib", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets the _dwg_entity_ATTRIB::thickness, DXF 39
  \code Usage: dwg_ent_attrib_set_thickness(attrib, thick, &error);
  \endcode
  \param[out] attrib dwg_ent_attrib*
  \param[in] thickness double
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_attrib_set_thickness (dwg_ent_attrib *restrict attrib,
                              const double thickness, int *restrict error)
{
  if (attrib)
    {
      *error = 0;
      attrib->thickness = thickness;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty attrib", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_ATTRIB::rotation, DXF 50 (in radians)
  \code Usage: double angle = dwg_ent_attrib_get_rot_angle(attrib, &error);
  \endcode
  \param[in] attrib dwg_ent_attrib*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
double
dwg_ent_attrib_get_rotation (const dwg_ent_attrib *restrict attrib,
                             int *restrict error)
{
  if (attrib)
    {
      *error = 0;
      return attrib->rotation;
    }
  else
    {
      LOG_ERROR ("%s: empty attrib", __FUNCTION__)
      *error = 1;
      return bit_nan ();
    }
}

/** Sets the _dwg_entity_ATTRIB::rotation, DXF 50 (in radians)
  \code Usage: dwg_ent_attrib_set_rotation(attrib, angle, &error);
  \endcode
  \param[out] attrib  dwg_ent_attrib*
  \param[in] angle   double
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_attrib_set_rotation (dwg_ent_attrib *restrict attrib,
                             const double angle, int *restrict error)
{
  if (attrib)
    {
      *error = 0;
      attrib->rotation = angle;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_ATTRIB::vert_alignment, DXF 73
  \code Usage: short vert_align = dwg_ent_attrib_get_vert_alignment(attrib,
&error); \endcode \param[in] attrib dwg_ent_attrib* \param[out] error   int*,
is set to 0 for ok, 1 on error \return short(1 = bottom, 2 = middle, 3 = top)
*/
EXPORT BITCODE_BS
dwg_ent_attrib_get_vert_alignment (const dwg_ent_attrib *restrict attrib,
                                   int *restrict error)
{
  if (attrib)
    {
      *error = 0;
      return attrib->vert_alignment;
    }
  else
    {
      LOG_ERROR ("%s: empty attrib", __FUNCTION__)
      *error = 1;
      return -1;
    }
}

/** Sets the  _dwg_entity_ATTRIB::vert_alignment, DXF 73
  \code Usage: dwg_ent_attrib_set_vert_alignment(attrib, angle, &error);
  \endcode
  \param[in] attrib dwg_ent_attrib*
  \param[in] short (1 = bottom, 2 = middle, 3 = top)
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_attrib_set_vert_alignment (dwg_ent_attrib *restrict attrib,
                                   BITCODE_BS alignment, int *restrict error)
{
  if (attrib)
    {
      *error = 0;
      attrib->vert_alignment = alignment;
    }
  else
    {
      LOG_ERROR ("%s: empty attrib", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the _dwg_entity_ATTRIB::horiz_alignment, DXF 72
  \code Usage: short horiz_align =  dwg_ent_attrib_get_horiz_alignment(attrib,
&error); \endcode \param[in] attrib dwg_ent_attrib* \param[out] error int*, is
set to 0 for ok, 1 on error \return (1 = left, 2 = center, 3 = right)
*/
EXPORT BITCODE_BS
dwg_ent_attrib_get_horiz_alignment (const dwg_ent_attrib *restrict attrib,
                                    int *restrict error)
{
  if (attrib)
    {
      *error = 0;
      return attrib->horiz_alignment;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return -1;
    }
}

/** Sets the  _dwg_entity_ATTRIB::horiz_alignment, DXF 72
  \code Usage: dwg_ent_attrib_set_horiz_alignment(attrib, angle, &error);
  \endcode
  \param[out] attrib dwg_ent_attrib*
  \param[in]  alignment short (1 = left, 2 = center, 3 = right)
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_attrib_set_horiz_alignment (dwg_ent_attrib *restrict attrib,
                                    const BITCODE_BS alignment,
                                    int *restrict error)
{
  if (attrib)
    {
      *error = 0;
      attrib->horiz_alignment = alignment;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/*******************************************************************
 *                   FUNCTIONS FOR ATTDEF ENTITY                     *
 ********************************************************************/

/** Sets the _dwg_entity_ATTDEF::default_value, DXF 1 (utf-8 encoded).
  \code Usage: dwg_ent_attdef_set_text(attdef, "Hello world", &error);
  \endcode
  \param[in]  attdef dwg_ent_attdef*
  \param[in]  default_value utf-8 string (char *)
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_attdef_set_default_value (dwg_ent_attdef *restrict ent,
                                  const char *restrict default_value,
                                  int *restrict error)
{
  Dwg_Data *dwg = dwg_obj_generic_dwg (ent, error);
  if (ent && !error)
    {
      ent->default_value = dwg_add_u8_input (dwg, default_value);
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_ATTDEF::default_value, DXF 1 (utf-8 encoded).
  \code Usage: char * text = dwg_ent_attdef_get_text(attdef, &error);
  \endcode
  \param[in] attdef dwg_ent_attdef*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
char *
dwg_ent_attdef_get_default_value (const dwg_ent_attdef *restrict ent,
                                  int *restrict error)
{
  if (ent)
    {
      *error = 0;
      if (dwg_version >= R_2007)
        return bit_convert_TU ((BITCODE_TU)ent->default_value);
      else
        return ent->default_value;
    }
  else
    {
      LOG_ERROR ("%s: empty attdef", __FUNCTION__)
      *error = 1;
      return NULL;
    }
}

/** Returns the _dwg_entity_ATTDEF::ins_pt, DXF 10
  \code Usage: dwg_ent_attdef_get_insertion_pt(attdef, &point, &error);
  \endcode
  \param[in]  attdef  dwg_ent_attdef*
  \param[out] point   dwg_point_2d*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_attdef_get_insertion_pt (const dwg_ent_attdef *restrict attdef,
                                 dwg_point_2d *restrict point,
                                 int *restrict error)
{
  if (attdef
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = attdef->ins_pt.x;
      point->y = attdef->ins_pt.y;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the _dwg_entity_ATTDEF::ins_pt, DXF 10
  \code Usage: dwg_ent_attdef_set_insertion_pt(attdef, &point, &error)
  \endcode
  \param[in] attdef   dwg_ent_attdef*
  \param[in] point    dwg_point_2d*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_attdef_set_insertion_pt (dwg_ent_attdef *restrict attdef,
                                 const dwg_point_2d *restrict point,
                                 int *restrict error)
{
  if (attdef
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      attdef->ins_pt.x = point->x;
      attdef->ins_pt.y = point->y;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_ATTDEF::height, DXF 40.
  \code Usage: dwg_ent_attdef_get_height(attdef, &error);
  \endcode
  \param[in] attdef dwg_ent_attdef*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
double
dwg_ent_attdef_get_height (const dwg_ent_attdef *restrict attdef,
                           int *restrict error)
{
  if (attdef)
    {
      *error = 0;
      return attdef->height;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets the _dwg_entity_ATTDEF::height, DXF 40.
  \code Usage: dwg_ent_attdef_set_height(attdef, 10.0, &error);
  \endcode
  \param[in] attdef dwg_ent_attdef*
  \param[in] height double
  \param[out] error int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_attdef_set_height (dwg_ent_attdef *restrict attdef,
                           const double height, int *restrict error)
{
  if (attdef)
    {
      *error = 0;
      attdef->height = height;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_ATTDEF::extrusion vector, DXF 210
  \code Usage: dwg_ent_attdef_get_extrusion(attdef, &point, &error);
  \endcode
  \param[in]  attdef  dwg_ent_attdef*
  \param[out] vector  dwg_point_3d*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_attdef_get_extrusion (const dwg_ent_attdef *restrict attdef,
                              dwg_point_3d *restrict vector,
                              int *restrict error)
{
  if (attdef
#    ifndef HAVE_NONNULL
      && vector
#    endif
  )
    {
      *error = 0;
      vector->x = attdef->extrusion.x;
      vector->y = attdef->extrusion.y;
      vector->z = attdef->extrusion.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the _dwg_entity_ATTDEF::extrusion vector, DXF 210
  \code Usage: dwg_ent_attdef_set_extrusion(attdef, &point, &error);
  \endcode
  \param[in] attdef dwg_ent_attdef*
  \param[in] vector  dwg_point_3d*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_attdef_set_extrusion (dwg_ent_attdef *restrict attdef,
                              const dwg_point_3d *restrict vector,
                              int *restrict error)
{
  if (attdef
#    ifndef HAVE_NONNULL
      && vector
#    endif
  )
    {
      *error = 0;
      attdef->extrusion.x = vector->x;
      attdef->extrusion.y = vector->y;
      attdef->extrusion.z = vector->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_ATTDEF::thickness vector, DXF 39.
  \code Usage: double thickness = dwg_ent_attdef_get_thickness(attdef, &error);
  \endcode
  \param[in] attdef dwg_ent_attdef*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
double
dwg_ent_attdef_get_thickness (const dwg_ent_attdef *restrict attdef,
                              int *restrict error)
{
  if (attdef)
    {
      *error = 0;
      return attdef->thickness;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets the _dwg_entity_ATTDEF::thickness vector, DXF 39.
  \code Usage: dwg_ent_attdef_set_thickness(attdef, thickness, &error);
  \endcode
  \param[in] attdef dwg_ent_attdef*
  \param[in] thickness double
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_attdef_set_thickness (dwg_ent_attdef *restrict attdef,
                              const double thickness, int *restrict error)
{
  if (attdef)
    {
      *error = 0;
      attdef->thickness = thickness;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_ATTDEF::rotation agle, DXF 50 (radian).
  \code Usage: double angle = dwg_ent_attdef_get_rotation(attdef, &error);
  \endcode
  \param[in] attdef dwg_ent_attdef*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
double
dwg_ent_attdef_get_rotation (const dwg_ent_attdef *restrict attdef,
                             int *restrict error)
{
  if (attdef)
    {
      *error = 0;
      return attdef->rotation;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets the _dwg_entity_ATTDEF::rotation angle, DXF 50 (radian).
  \code Usage: dwg_ent_attdef_set_rotation(attdef, angle, &error);
  \endcode
  \param[in] attdef dwg_ent_attdef*
  \param[in] angle double
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_attdef_set_rotation (dwg_ent_attdef *restrict attdef,
                             const double angle, int *restrict error)
{
  if (attdef)
    {
      *error = 0;
      attdef->rotation = angle;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_ATTDEF::vert_alignment, DXF 74.
  \code Usage: short vert_align = dwg_ent_attdef_get_vert_alignment(attdef,
&error); \endcode \param[in] attdef dwg_ent_attdef* \param[out] error   int*,
is set to 0 for ok, 1 on error \return (1 = bottom, 2 = middle, 3 = top)
*/
EXPORT BITCODE_BS
dwg_ent_attdef_get_vert_alignment (const dwg_ent_attdef *restrict attdef,
                                   int *restrict error)
{
  if (attdef)
    {
      *error = 0;
      return attdef->vert_alignment;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return -1;
    }
}

/** Sets the _dwg_entity_ATTDEF::vert_alignment, DXF 74.
  \code Usage: dwg_ent_attdef_set_vert_alignment(attdef, angle, &error);
  \endcode
  \param[in] attdef dwg_ent_attdef*
  \param[in] alignment (1 = bottom, 2 = middle, 3 = top)
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_attdef_set_vert_alignment (dwg_ent_attdef *restrict attdef,
                                   const BITCODE_BS alignment,
                                   int *restrict error)
{
  if (attdef)
    {
      *error = 0;
      attdef->vert_alignment = alignment;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_ATTDEF::horiz_alignment, DXF 72.
  \code Usage: short horiz_align = dwg_ent_attdef_get_horiz_alignment(attdef,
&error); \endcode \param[in] attdef dwg_ent_attdef* \param[out] error   int*,
is set to 0 for ok, 1 on error \return (1 = left, 2 = center, 3 = right)
*/
EXPORT BITCODE_BS
dwg_ent_attdef_get_horiz_alignment (const dwg_ent_attdef *restrict attdef,
                                    int *restrict error)
{
  if (attdef)
    {
      *error = 0;
      return attdef->horiz_alignment;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return -1;
    }
}

/** Sets the _dwg_entity_ATTDEF::horiz_alignment, DXF 72.
  \code Usage: dwg_ent_attdef_set_horiz_alignment(attdef, alignment, &error);
  \endcode
  \param[in] attdef dwg_ent_attdef*
  \param[in] alignment short (1 = left, 2 = center, 3 = right)
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_attdef_set_horiz_alignment (dwg_ent_attdef *restrict attdef,
                                    const BITCODE_BS alignment,
                                    int *restrict error)
{
  if (attdef)
    {
      *error = 0;
      attdef->horiz_alignment = alignment;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/*******************************************************************
 *                   FUNCTIONS FOR POINT ENTITY                      *
 ********************************************************************/

/** Sets the _dwg_entity_POINT::point, DXF 10.
  \code Usage: dwg_ent_point_set_point(point, &retpoint, &error);
  \endcode
  \param[out] point dwg_ent_point*
  \param[in]  retpoint dwg_point_3d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_point_set_point (dwg_ent_point *restrict point,
                         const dwg_point_3d *restrict retpoint,
                         int *restrict error)
{
  if (point
#    ifndef HAVE_NONNULL
      && retpoint
#    endif
  )
    {
      *error = 0;
      point->x = retpoint->x;
      point->y = retpoint->y;
      point->z = retpoint->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the  _dwg_entity_POINT::point, DXF 10.
  \code Usage: dwg_ent_point_get_point(point, &retpoint, &error);
  \endcode
  \param[in]  point dwg_ent_point*
  \param[out] retpoint dwg_point_3d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_point_get_point (const dwg_ent_point *restrict point,
                         dwg_point_3d *restrict retpoint, int *restrict error)
{
  if (point
#    ifndef HAVE_NONNULL
      && retpoint
#    endif
  )
    {
      *error = 0;
      retpoint->x = point->x;
      retpoint->y = point->y;
      retpoint->z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_POINT::thickness, DXF 39.
  \code Usage: double thickness = dwg_ent_point_get_thickness(point, &error);
  \endcode
  \param[in] point dwg_ent_point*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
double
dwg_ent_point_get_thickness (const dwg_ent_point *restrict point,
                             int *restrict error)
{
  if (point)
    {
      *error = 0;
      return point->thickness;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets the  _dwg_entity_POINT::thickness, DXF 39.
  \code Usage: dwg_ent_point_set_thickness(point, thickness, &error);
  \endcode
  \param[out] point dwg_ent_point*
  \param[in] double
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_point_set_thickness (dwg_ent_point *restrict point,
                             const double thickness, int *restrict error)
{
  if (point)
    {
      *error = 0;
      point->thickness = thickness;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the _dwg_entity_POINT::extrusion vector, DXF 210.
  \code Usage: dwg_ent_point_set_extrusion(point, &retpoint, &error);
  \endcode
  \param[out]  point dwg_ent_point*
  \param[in] vector dwg_point_3d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_point_set_extrusion (dwg_ent_point *restrict point,
                             const dwg_point_3d *restrict vector,
                             int *restrict error)
{
  if (point
#    ifndef HAVE_NONNULL
      && vector
#    endif
  )
    {
      *error = 0;
      point->extrusion.x = vector->x;
      point->extrusion.y = vector->y;
      point->extrusion.z = vector->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_POINT::extrusion vector, DXF 210.
  \code Usage: dwg_ent_point_get_extrusion(point, &retpoint, &error);
  \endcode
  \param[in]  point dwg_ent_point*
  \param[out] vector dwg_point_3d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_point_get_extrusion (const dwg_ent_point *restrict point,
                             dwg_point_3d *restrict vector,
                             int *restrict error)
{
  if (point
#    ifndef HAVE_NONNULL
      && vector
#    endif
  )
    {
      *error = 0;
      vector->x = point->extrusion.x;
      vector->y = point->extrusion.y;
      vector->z = point->extrusion.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/*******************************************************************
 *                   FUNCTIONS FOR SOLID ENTITY                      *
 ********************************************************************/

/** Returns the _dwg_entity_SOLID::thickness, DXF 39.
  \code Usage: double thickness = dwg_ent_solid_get_thickness(solid, &error);
  \endcode
  \param[in] solid dwg_ent_solid*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
double
dwg_ent_solid_get_thickness (const dwg_ent_solid *restrict solid,
                             int *restrict error)
{
  if (solid)
    {
      *error = 0;
      return solid->thickness;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets the _dwg_entity_SOLID::thickness, DXF 39.
  \code Usage: dwg_ent_solid_set_thickness(solid, 2.0, &error);
  \endcode
  \param[out] solid dwg_ent_solid*
  \param[in] thickness double
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_solid_set_thickness (dwg_ent_solid *restrict solid,
                             const double thickness, int *restrict error)
{
  if (solid)
    {
      *error = 0;
      solid->thickness = thickness;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_SOLID::elevation (z-coord), DXF 38.
  \code Usage: double elev = dwg_ent_solid_get_elevation(solid, &error);
  \endcode
  \param[in] solid dwg_ent_solid*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
double
dwg_ent_solid_get_elevation (const dwg_ent_solid *restrict solid,
                             int *restrict error)
{
  if (solid)
    {
      *error = 0;
      return solid->elevation;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets the _dwg_entity_SOLID::elevation (z-coord), DXF 38.
  \code Usage: dwg_ent_solid_set_elevation(solid, 20.0, &error);
  \endcode
  \param[in] solid dwg_ent_solid*
  \param[in] elevation double
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_solid_set_elevation (dwg_ent_solid *restrict solid,
                             const double elevation, int *restrict error)
{
  if (solid)
    {
      *error = 0;
      solid->elevation = elevation;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_SOLID::corner1 2dpoint, DXF 10.
  \code Usage: dwg_ent_solid_get_corner1(solid, &point, &error);
  \endcode
  \param[in] solid dwg_ent_solid*
  \param[out] dwg_point_2d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_solid_get_corner1 (const dwg_ent_solid *restrict solid,
                           dwg_point_2d *restrict point, int *restrict error)
{
  if (solid
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = solid->corner1.x;
      point->y = solid->corner1.y;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the _dwg_entity_SOLID::corner1 2dpoint, DXF 10.
  \code Usage: dwg_ent_solid_set_corner1(solid, &point, &error);
  \endcode
  \param[out] solid dwg_ent_solid*
  \param[in]  point dwg_point_2d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_solid_set_corner1 (dwg_ent_solid *restrict solid,
                           const dwg_point_2d *restrict point,
                           int *restrict error)
{
  if (solid
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      solid->corner1.x = point->x;
      solid->corner1.y = point->y;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_SOLID::corner2 2dpoint, DXF 11.
  \code Usage: dwg_ent_solid_get_corner2(solid, &point, &error);
  \endcode
  \param[in] solid dwg_ent_solid*
  \param[out] point dwg_point_2d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_solid_get_corner2 (const dwg_ent_solid *restrict solid,
                           dwg_point_2d *restrict point, int *restrict error)
{
  if (solid
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = solid->corner2.x;
      point->y = solid->corner2.y;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the _dwg_entity_SOLID::corner2 2dpoint, DXF 11.
  \code Usage: dwg_ent_solid_set_corner2(solid, &point, &error);
  \endcode
  \param[in] solid dwg_ent_solid*
  \param[out] dwg_point_2d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_solid_set_corner2 (dwg_ent_solid *restrict solid,
                           const dwg_point_2d *restrict point,
                           int *restrict error)
{
  if (solid
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      solid->corner2.x = point->x;
      solid->corner2.y = point->y;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_SOLID::corner3 2dpoint, DXF 12.
  \code Usage: dwg_ent_solid_get_corner3(solid, &point, &error);
  \endcode
  \param[in] solid dwg_ent_solid*
  \param[out] point dwg_point_2d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_solid_get_corner3 (const dwg_ent_solid *restrict solid,
                           dwg_point_2d *restrict point, int *restrict error)
{
  if (solid
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = solid->corner3.x;
      point->y = solid->corner3.y;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the _dwg_entity_SOLID::corner3 2dpoint, DXF 12.
  \code Usage: dwg_ent_solid_set_corner3(solid, &point, &error);
  \endcode
  \param[in] solid dwg_ent_solid*
  \param[out] point dwg_point_2d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_solid_set_corner3 (dwg_ent_solid *restrict solid,
                           const dwg_point_2d *restrict point,
                           int *restrict error)
{
  if (solid
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      solid->corner3.x = point->x;
      solid->corner3.y = point->y;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_SOLID::corner4 2dpoint, DXF 13.
  \code Usage: dwg_ent_solid_get_corner4(solid, &point, &error);
  \endcode
  \param[in] solid dwg_ent_solid*
  \param[out] point dwg_point_2d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_solid_get_corner4 (const dwg_ent_solid *restrict solid,
                           dwg_point_2d *restrict point, int *restrict error)
{
  if (solid
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = solid->corner4.x;
      point->y = solid->corner4.y;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the _dwg_entity_SOLID::corner4 2dpoint, DXF 13.
  \code Usage: dwg_ent_solid_set_corner4(solid, &point, &error);
  \endcode
  \param[in] solid dwg_ent_solid*
  \param[out] point dwg_point_2d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_solid_set_corner4 (dwg_ent_solid *restrict solid,
                           const dwg_point_2d *restrict point,
                           int *restrict error)
{
  if (solid
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      solid->corner4.x = point->x;
      solid->corner4.y = point->y;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_SOLID::extrusion vector, DXF 210.
  \code Usage: dwg_ent_solid_get_extrusion(solid, &point, &error);
  \endcode
  \param[in] solid dwg_ent_solid*
  \param[out] vector dwg_point_3d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_solid_get_extrusion (const dwg_ent_solid *restrict solid,
                             dwg_point_3d *restrict vector,
                             int *restrict error)
{
  if (solid
#    ifndef HAVE_NONNULL
      && vector
#    endif
  )
    {
      *error = 0;
      vector->x = solid->extrusion.x;
      vector->y = solid->extrusion.y;
      vector->z = solid->extrusion.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the _dwg_entity_SOLID::extrusion vector, DXF 210.
  \code Usage: dwg_ent_solid_set_extrusion(solid, &point, &error);
  \endcode
  \param[in] solid dwg_ent_solid*
  \param[out] vector dwg_point_3d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_solid_set_extrusion (dwg_ent_solid *restrict solid,
                             const dwg_point_3d *restrict vector,
                             int *restrict error)
{
  if (solid
#    ifndef HAVE_NONNULL
      && vector
#    endif
  )
    {
      *error = 0;
      solid->extrusion.x = vector->x;
      solid->extrusion.y = vector->y;
      solid->extrusion.z = vector->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/*******************************************************************
 *                   FUNCTIONS FOR BLOCK ENTITY                      *
 ********************************************************************/

/** Sets the _dwg_entity_BLOCK::name, DXF 1 (utf-8 encoded)
  \code Usage: dwg_ent_block_set_name(block, "block_name", &error);
  \endcode
  \param[out] block dwg_ent_block*
  \param[in]  name utf-8 char *
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_block_set_name (dwg_ent_block *restrict ent, const char *name,
                        int *restrict error)
{
  Dwg_Data *dwg = dwg_obj_generic_dwg (ent, error);
  if (ent && !error)
    {
      ent->name = dwg_add_u8_input (dwg, default_value);
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_BLOCK::name, DXF 1 (utf-8 encoded).
If dwg_version >= R_2007, the return value is malloc'ed
  \code Usage: char * name = dwg_ent_block_get_name(block, &error);
  \endcode
  \param[in] block dwg_ent_block*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
||||||| parent of 28c1ef9cb (Enable and use Boehm-Weiser libgc)
If dwg_version >= R_2007, the return value is malloc'ed
\code Usage: char * name = dwg_ent_block_get_name(block, &error);
\endcode
\param[in] block dwg_ent_block*
\param[out] error   int*, is set to 0 for ok, 1 on error
\deprecated
*/
char *
dwg_ent_block_get_name (const dwg_ent_block *restrict block,
                        int *restrict error)
{
  if (block)
    {
      *error = 0;
      if (dwg_version >= R_2007)
        return bit_convert_TU ((BITCODE_TU)block->name);
      else
        return block->name;
    }
  else
    {
      LOG_ERROR ("%s: empty block", __FUNCTION__)
      *error = 1;
      return NULL;
    }
}

/*******************************************************************
 *                    FUNCTIONS FOR RAY ENTITY                       *
 ********************************************************************/

/** Returns the _dwg_entity_RAY::point, DXF 10
  \code Usage: dwg_ent_ray_get_point(ray, &point, &error);
  \endcode
  \param[in] ray dwg_ent_ray*
  \param[out] point dwg_point_3d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_ray_get_point (const dwg_ent_ray *restrict ray,
                       dwg_point_3d *restrict point, int *restrict error)
{
  if (ray
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = ray->point.x;
      point->y = ray->point.y;
      point->z = ray->point.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the _dwg_entity_RAY::point, DXF 10
  \code Usage: dwg_ent_ray_set_point(ray, &point, &error);
  \endcode
  \param[in] ray dwg_ent_ray*
  \param[out] point dwg_point_3d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_ray_set_point (dwg_ent_ray *restrict ray,
                       const dwg_point_3d *restrict point, int *restrict error)
{
  if (ray
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      ray->point.x = point->x;
      ray->point.y = point->y;
      ray->point.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_RAY::vector, DXF 11
  \code Usage: dwg_ent_ray_get_vector(ray, &point, &error);
  \endcode
  \param[in] ray dwg_ent_ray*
  \param[out] vector dwg_point_3d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_ray_get_vector (const dwg_ent_ray *restrict ray,
                        dwg_point_3d *restrict vector, int *restrict error)
{
  if (ray
#    ifndef HAVE_NONNULL
      && vector
#    endif
  )
    {
      *error = 0;
      vector->x = ray->vector.x;
      vector->y = ray->vector.y;
      vector->z = ray->vector.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the _dwg_entity_RAY::vector, DXF 11
  \code Usage: dwg_ent_ray_set_vector(ray, &point, &error);
  \endcode
  \param[in] ray dwg_ent_ray*
  \param[out] vector dwg_point_3d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_ray_set_vector (dwg_ent_ray *restrict ray,
                        const dwg_point_3d *restrict vector,
                        int *restrict error)
{
  if (ray
#    ifndef HAVE_NONNULL
      && vector
#    endif
  )
    {
      *error = 0;
      ray->vector.x = vector->x;
      ray->vector.y = vector->y;
      ray->vector.z = vector->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/*******************************************************************
 *                   FUNCTIONS FOR XLINE ENTITY                      *
 ********************************************************************/

/** Returns the _dwg_entity_XLINE::point, DXF 10
  \code Usage: dwg_ent_xline_get_point(xline, &point, &error);
  \endcode
  \param[in] xline dwg_ent_xline*
  \param[out] point dwg_point_3d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_xline_get_point (const dwg_ent_xline *restrict xline,
                         dwg_point_3d *restrict point, int *restrict error)
{
  if (xline
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = xline->point.x;
      point->y = xline->point.y;
      point->z = xline->point.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the _dwg_entity_XLINE::point, DXF 10
  \code Usage: dwg_ent_xline_set_point(xline, &point, &error);
  \endcode
  \param[out]  xline dwg_ent_xline*
  \param[in] point dwg_point_3d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_xline_set_point (dwg_ent_xline *restrict xline,
                         const dwg_point_3d *restrict point,
                         int *restrict error)
{
  if (xline
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      xline->point.x = point->x;
      xline->point.y = point->y;
      xline->point.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_RAY::vector, DXF 11
  \code Usage: dwg_ent_xline_get_vector(xline, &point, &error);
  \endcode
  \param[in] xline dwg_ent_xline*
  \param[out] vector dwg_point_3d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_xline_get_vector (const dwg_ent_xline *restrict xline,
                          dwg_point_3d *restrict vector, int *restrict error)
{
  if (xline
#    ifndef HAVE_NONNULL
      && vector
#    endif
  )
    {
      *error = 0;
      vector->x = xline->vector.x;
      vector->y = xline->vector.y;
      vector->z = xline->vector.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the _dwg_entity_RAY::vector, DXF 11
Usage: dwg_ent_xline_set_vector(xline, &point, &error);
  \param[out]  xline dwg_ent_xline*
  \param[in] vector dwg_point_3d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_xline_set_vector (dwg_ent_xline *restrict xline,
                          const dwg_point_3d *restrict vector,
                          int *restrict error)
{
  if (xline
#    ifndef HAVE_NONNULL
      && vector
#    endif
  )
    {
      *error = 0;
      xline->vector.x = vector->x;
      xline->vector.y = vector->y;
      xline->vector.z = vector->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/*******************************************************************
 *                   FUNCTIONS FOR TRACE ENTITY                      *
 ********************************************************************/

/** Returns the _dwg_entity_TRACE::thickness, DXF 39
  \code Usage: double thickness = dwg_ent_trace_get_thickness(trace, &error);
  \endcode
  \param[in] trace dwg_ent_trace*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
double
dwg_ent_trace_get_thickness (const dwg_ent_trace *restrict trace,
                             int *restrict error)
{
  if (trace)
    {
      *error = 0;
      return trace->thickness;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets the _dwg_entity_TRACE::thickness, DXF 39
  \code Usage: dwg_ent_trace_set_thickness(trace, 2.0, &error);
  \endcode
  \param[out] trace dwg_ent_trace*
  \param[in] thickness double
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_trace_set_thickness (dwg_ent_trace *restrict trace,
                             const double thickness, int *restrict error)
{
  if (trace)
    {
      *error = 0;
      trace->thickness = thickness;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_TRACE::elevation (z-coord), DXF 38
  \code Usage: double elev = dwg_ent_trace_get_elevation(trace, &error);
  \endcode
  \param[in] trace dwg_ent_trace*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
double
dwg_ent_trace_get_elevation (const dwg_ent_trace *restrict trace,
                             int *restrict error)
{
  if (trace)
    {
      *error = 0;
      return trace->elevation;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets the _dwg_entity_TRACE::elevation (z-coord), DXF 38
  \code Usage: dwg_ent_trace_set_elevation(trace, 20, &error);
  \endcode
  \param[out] trace dwg_ent_trace*
  \param[in] elevation double
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_trace_set_elevation (dwg_ent_trace *restrict trace,
                             const double elevation, int *restrict error)
{
  if (trace)
    {
      *error = 0;
      trace->elevation = elevation;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_TRACE::corner1 2dpoint, DXF 10
  \code Usage: dwg_ent_trace_get_corner1(trace, &point, &error);
  \endcode
  \param[in] trace dwg_ent_trace*
  \param[out] point dwg_point_2d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_trace_get_corner1 (const dwg_ent_trace *restrict trace,
                           dwg_point_2d *restrict point, int *restrict error)
{
  if (trace
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = trace->corner1.x;
      point->y = trace->corner1.y;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the _dwg_entity_TRACE::corner1 2dpoint, DXF 10
  \code Usage: dwg_ent_trace_set_corner1(trace, &point, &error);
  \endcode
  \param[in] trace dwg_ent_trace*
  \param[out] point dwg_point_2d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_trace_set_corner1 (dwg_ent_trace *restrict trace,
                           const dwg_point_2d *restrict point,
                           int *restrict error)
{
  if (trace
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      trace->corner1.x = point->x;
      trace->corner1.y = point->y;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_TRACE::corner2 2dpoint, DXF 11
  \code Usage: dwg_ent_trace_get_corner2(trace, &point, &error);
  \endcode
  \param[in] trace dwg_ent_trace*
  \param[out] point dwg_point_2d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_trace_get_corner2 (const dwg_ent_trace *restrict trace,
                           dwg_point_2d *restrict point, int *restrict error)
{
  if (trace
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = trace->corner2.x;
      point->y = trace->corner2.y;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the _dwg_entity_TRACE::corner2 2dpoint, DXF 11
  \code Usage: dwg_ent_trace_set_corner2(trace, &point, &error);
  \endcode
  \param[out] trace dwg_ent_trace*
  \param[in] point dwg_point_2d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_trace_set_corner2 (dwg_ent_trace *restrict trace,
                           const dwg_point_2d *restrict point,
                           int *restrict error)
{
  if (trace
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      trace->corner2.x = point->x;
      trace->corner2.y = point->y;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_TRACE::corner3 2dpoint, DXF 12
  \code Usage: dwg_ent_trace_get_corner3(trace, &point, &error);
  \endcode
  \param[in] trace dwg_ent_trace*
  \param[out] point dwg_point_2d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_trace_get_corner3 (const dwg_ent_trace *restrict trace,
                           dwg_point_2d *restrict point, int *restrict error)
{
  if (trace
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = trace->corner3.x;
      point->y = trace->corner3.y;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the _dwg_entity_TRACE::corner3 2dpoint, DXF 12
  \code Usage: dwg_ent_trace_set_corner3(trace, &point, &error);
  \endcode
  \param[out] trace dwg_ent_trace*
  \param[in] point dwg_point_2d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_trace_set_corner3 (dwg_ent_trace *restrict trace,
                           const dwg_point_2d *restrict point,
                           int *restrict error)
{
  if (trace
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      trace->corner3.x = point->x;
      trace->corner3.y = point->y;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_TRACE::corner4 2dpoint, DXF 13
  \code Usage: dwg_ent_trace_get_corner4(trace, &point, &error);
  \endcode
  \param[in] trace dwg_ent_trace*
  \param[out] point dwg_point_2d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_trace_get_corner4 (const dwg_ent_trace *restrict trace,
                           dwg_point_2d *restrict point, int *restrict error)
{
  if (trace
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = trace->corner4.x;
      point->y = trace->corner4.y;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the _dwg_entity_TRACE::corner4 2dpoint, DXF 13.
  \code Usage: dwg_ent_trace_set_corner4(trace, &point, &error);
  \endcode
  \param[out] trace dwg_ent_trace*
  \param[in] point dwg_point_2d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_trace_set_corner4 (dwg_ent_trace *restrict trace,
                           const dwg_point_2d *restrict point,
                           int *restrict error)
{
  if (trace
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      trace->corner4.x = point->x;
      trace->corner4.y = point->y;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_TRACE::extrusion vector, DXF 210.
  \code Usage: dwg_ent_trace_get_extrusion(trace, &ext, &error);
  \endcode
  \param[in] trace dwg_ent_trace*
  \param[out] vector dwg_point_3d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_trace_get_extrusion (const dwg_ent_trace *restrict trace,
                             dwg_point_3d *restrict vector,
                             int *restrict error)
{
  if (trace
#    ifndef HAVE_NONNULL
      && vector
#    endif
  )
    {
      *error = 0;
      vector->x = trace->extrusion.x;
      vector->y = trace->extrusion.y;
      vector->z = trace->extrusion.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the _dwg_entity_TRACE::extrusion vector, DXF 210.
  \code Usage: dwg_ent_trace_set_extrusion(trace, &ext, &error);
  \endcode
  \param[in] trace dwg_ent_trace*
  \param[out] vector dwg_point_3d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_trace_set_extrusion (dwg_ent_trace *restrict trace,
                             const dwg_point_3d *restrict vector,
                             int *restrict error)
{
  if (trace
#    ifndef HAVE_NONNULL
      && vector
#    endif
  )
    {
      *error = 0;
      trace->extrusion.x = vector->x;
      trace->extrusion.y = vector->y;
      trace->extrusion.z = vector->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/*******************************************************************
 *                 FUNCTIONS FOR VERTEX_3D ENTITY                    *
 ********************************************************************/

/** Returns the _dwg_entity_VERTEX_3D::flag, DXF 70.
  \code Usage: char flag = dwg_ent_vertex_3d_get_flag(vert, &error);
  \endcode
  \param[in]  vert    dwg_ent_vertex_3d*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
char
dwg_ent_vertex_3d_get_flag (const dwg_ent_vertex_3d *restrict vert,
                            int *restrict error)
{
  if (vert)
    {
      *error = 0;
      return vert->flag;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/** Sets the _dwg_entity_VERTEX_3D::flag, DXF 70.
  \code Usage: dwg_ent_vertex_3d_set_flag(vert, flag, &error);
  \endcode
  \param[out] vert   dwg_ent_vertex_3d*
  \param[in]  flag   char
  \param[out] error  int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_vertex_3d_set_flag (dwg_ent_vertex_3d *restrict vert, const char flag,
                            int *restrict error)
{
  if (vert)
    {
      *error = 0;
      vert->flag = flag;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_VERTEX_3D::point, DXF 10.
  \code Usage: dwg_ent_vertex_3d_get_point(vert, &point, &error);
  \endcode
  \param[in]  vert   dwg_ent_vertex_3d*
  \param[out] point  dwg_point_3d
  \param[out] error  int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_vertex_3d_get_point (const dwg_ent_vertex_3d *restrict vert,
                             dwg_point_3d *restrict point, int *restrict error)
{
  if (vert
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = vert->point.x;
      point->y = vert->point.y;
      point->z = vert->point.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the_dwg_entity_VERTEX_3D::point, DXF 10.
  \code Usage: dwg_ent_vertex_3d_set_point(vert, &point, &error);
  \endcode
  \param[in]  vert  dwg_ent_vertex_3d*
  \param[out] point dwg_point_3d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_vertex_3d_set_point (dwg_ent_vertex_3d *restrict vert,
                             const dwg_point_3d *restrict point,
                             int *restrict error)
{
  if (vert
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      vert->point.x = point->x;
      vert->point.y = point->y;
      vert->point.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/*******************************************************************
 *               FUNCTIONS FOR VERTEX_MESH ENTITY                    *
 ********************************************************************/

/** Returns the _dwg_entity_VERTEX_MESH::flag, DXF 70.
  \code Usage: char flag = dwg_ent_vertex_mesh_get_flag(vert, &error);
  \endcode
  \param[in]  vert   dwg_ent_vertex_mesh*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
char
dwg_ent_vertex_mesh_get_flag (const dwg_ent_vertex_mesh *restrict vert,
                              int *restrict error)
{
  if (vert)
    {
      *error = 0;
      return vert->flag;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/** Sets the _dwg_entity_VERTEX_MESH::flag, DXF 70.
  \code Usage: dwg_ent_vertex_mesh_set_flag(vert, flag, &error);
  \endcode
  \param[out] vert   dwg_ent_vertex_mesh*
  \param[in] flag    char
  \param[out] error  int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_vertex_mesh_set_flag (dwg_ent_vertex_mesh *restrict vert,
                              const char flag, int *restrict error)
{
  if (vert)
    {
      *error = 0;
      vert->flag = flag;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_VERTEX_MESH::point, DXF 10.
  \code Usage: dwg_ent_vertex_mesh_get_point(vert, &point, &error);
  \endcode
  \param[in]  vert    dwg_ent_vertex_mesh*
  \param[out] point   dwg_point_3d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_vertex_mesh_get_point (const dwg_ent_vertex_mesh *restrict vert,
                               dwg_point_3d *restrict point,
                               int *restrict error)
{
  if (vert
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = vert->point.x;
      point->y = vert->point.y;
      point->z = vert->point.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the _dwg_entity_VERTEX_MESH::point, DXF 10.
  \code Usage: dwg_ent_vertex_mesh_set_point(vert, &point, &error);
  \endcode
  \param[in]  vert    dwg_ent_vertex_mesh*
  \param[out] point   dwg_point_3d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_vertex_mesh_set_point (dwg_ent_vertex_mesh *restrict vert,
                               const dwg_point_3d *restrict point,
                               int *restrict error)
{
  if (vert
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      vert->point.x = point->x;
      vert->point.y = point->y;
      vert->point.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/*******************************************************************
 *               FUNCTIONS FOR VERTEX_PFACE ENTITY                   *
 ********************************************************************/

/** Returns the _dwg_entity_VERTEX_PFACE::flag, DXF 70.
  \code Usage: char flag = dwg_ent_vertex_pface_get_flag(vert, &error);
  \endcode
  \param[in] vertex_pface dwg_ent_vertex_pface*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
char
dwg_ent_vertex_pface_get_flag (const dwg_ent_vertex_pface *restrict vert,
                               int *restrict error)
{
  if (vert)
    {
      *error = 0;
      return vert->flag;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/** Sets the _dwg_entity_VERTEX_PFACE::flag, DXF 70.
  \code Usage: dwg_ent_vertex_pface_set_flag(vert, flag, &error);
  \endcode
  \param[out] vertex_pface dwg_ent_vertex_pface*
  \param[in] flag
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_vertex_pface_set_flag (dwg_ent_vertex_pface *restrict vert,
                               const char flag, int *restrict error)
{
  if (vert)
    {
      *error = 0;
      vert->flag = flag;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_VERTEX_PFACE::point, DXF 10.
  \code Usage: dwg_ent_vertex_pface_get_point(vert, &point, &error);
  \endcode
  \param[in]  vert    dwg_ent_vertex_pface*
  \param[out] point   dwg_point_3d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_vertex_pface_get_point (const dwg_ent_vertex_pface *restrict vert,
                                dwg_point_3d *restrict point,
                                int *restrict error)
{
  if (vert
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = vert->point.x;
      point->y = vert->point.y;
      point->z = vert->point.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the _dwg_entity_VERTEX_PFACE::point, DXF 10.
  \code Usage: dwg_ent_vertex_pface_set_point(vert, &point, &error);
  \endcode
  \param[out] vert    dwg_ent_vertex_pface*
  \param[in]  point   dwg_point_3d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_vertex_pface_set_point (dwg_ent_vertex_pface *restrict vert,
                                const dwg_point_3d *restrict point,
                                int *restrict error)
{
  if (vert
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      vert->point.x = point->x;
      vert->point.y = point->y;
      vert->point.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/*******************************************************************
 *                 FUNCTIONS FOR VERTEX_2D ENTITY                    *
 ********************************************************************/

/** Returns the _dwg_entity_VERTEX_2D::flag, DXF 70.
  \code Usage: dwg_ent_vertex_2d_get_flag(vert, &error);
  \endcode
  \param[in]  vert    dwg_ent_vertex_2d*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
char
dwg_ent_vertex_2d_get_flag (const dwg_ent_vertex_2d *restrict vert,
                            int *restrict error)
{
  if (vert)
    {
      *error = 0;
      return vert->flag;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/** Sets the _dwg_entity_VERTEX_2D::flag, DXF 70.
  \code Usage: dwg_ent_vertex_2d_set_flag(vert, flag, &error);
  \endcode
  \param[out] vert dwg_ent_vertex_mesh*
  \param[in]  flag char
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_vertex_2d_set_flag (dwg_ent_vertex_2d *restrict vert, const char flag,
                            int *restrict error)
{
  if (vert)
    {
      *error = 0;
      vert->flag = flag;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_VERTEX_2D::point, DXF 10.
  \code Usage: dwg_ent_vertex_2d_get_point(vert, &point, &error);
  \endcode
  \param[in]  vert    dwg_ent_vertex_2d*
  \param[out] point   dwg_point_3d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_vertex_2d_get_point (const dwg_ent_vertex_2d *restrict vert,
                             dwg_point_3d *restrict point, int *restrict error)
{
  if (vert
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = vert->point.x;
      point->y = vert->point.y;
      point->z = vert->point.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the _dwg_entity_VERTEX_2D::point, DXF 10.
  \code Usage: dwg_ent_vertex_2d_set_point(vert, &point, &error);
  \endcode
  \param[out] vert    dwg_ent_vertex_2d*
  \param[in]  point   dwg_point_3d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_vertex_2d_set_point (dwg_ent_vertex_2d *restrict vert,
                             const dwg_point_3d *restrict point,
                             int *restrict error)
{
  if (vert
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      vert->point.x = point->x;
      vert->point.y = point->y;
      vert->point.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_VERTEX_2D::start_width, DXF 40.
  \code Usage: double width = dwg_ent_vertex_2d_get_start_width(vert, &error);
  \endcode
  \param[in]  vert    dwg_ent_vertex_2d*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
double
dwg_ent_vertex_2d_get_start_width (const dwg_ent_vertex_2d *restrict vert,
                                   int *restrict error)
{
  if (vert)
    {
      *error = 0;
      return vert->start_width;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets the _dwg_entity_VERTEX_2D::start_width, DXF 40.
  \code Usage: dwg_ent_vertex_2d_set_start_width(vert, 20, &error);
  \endcode
  \param[out] vert        dwg_ent_vertex_2d*
  \param[in] start_width double
  \param[out] error      int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_vertex_2d_set_start_width (dwg_ent_vertex_2d *restrict vert,
                                   const double start_width,
                                   int *restrict error)
{
  if (vert)
    {
      *error = 0;
      vert->start_width = start_width;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_VERTEX_2D::end_width, DXF 41.
  \code Usage: double width = dwg_ent_vertex_2d_get_end_width(vert, &error);
  \endcode
  \param[in] vert     dwg_ent_vertex_2d*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
double
dwg_ent_vertex_2d_get_end_width (const dwg_ent_vertex_2d *restrict vert,
                                 int *restrict error)
{
  if (vert)
    {
      *error = 0;
      return vert->end_width;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets the _dwg_entity_VERTEX_2D::end_width, DXF 41.
  \code Usage: dwg_ent_vertex_2d_set_end_width(vert, 20, &error);
  \endcode
  \param[out] vert dwg_ent_vertex_2d*
  \param[in] end_width double
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_vertex_2d_set_end_width (dwg_ent_vertex_2d *restrict vert,
                                 const double end_width, int *restrict error)
{
  if (vert)
    {
      *error = 0;
      vert->end_width = end_width;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_VERTEX_2D::bulge, DXF 42. (radians)
  \code Usage: double bulge = dwg_ent_vertex_2d_get_bulge(vert, &error);
  \endcode
  \param[in] vert dwg_ent_vertex_2d*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
double
dwg_ent_vertex_2d_get_bulge (const dwg_ent_vertex_2d *restrict vert,
                             int *restrict error)
{
  if (vert)
    {
      *error = 0;
      return vert->bulge;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets the _dwg_entity_VERTEX_2D::bulge, DXF 42. (radians)
  \code Usage: dwg_ent_vertex_2d_set_bulge(vert, 20, &error);
  \endcode
  \param[in] vert dwg_ent_vertex_2d*
  \param[in] bulge double
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_vertex_2d_set_bulge (dwg_ent_vertex_2d *restrict vert,
                             const double bulge, int *restrict error)
{
  if (vert)
    {
      *error = 0;
      vert->bulge = bulge;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_VERTEX_2D::tangent_dir, DXF 50. (radians)
  \code Usage: double tangent_dir = dwg_ent_vertex_2d_get_tangent_dir(vert,
&error); \endcode \param[in] vert dwg_ent_vertex_2d* \param[out] error   int*,
is set to 0 for ok, 1 on error \deprecated
*/
double
dwg_ent_vertex_2d_get_tangent_dir (const dwg_ent_vertex_2d *restrict vert,
                                   int *restrict error)
{
  if (vert)
    {
      *error = 0;
      return vert->tangent_dir;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets the _dwg_entity_VERTEX_2D::tangent_dir, DXF 50. (radians)
  \code Usage: dwg_ent_vertex_2d_set_tangent_dir(vert, 20, &error);
  \endcode
  \param[in] vert dwg_ent_vertex_2d*
  \param[in] tangent_dir double
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_vertex_2d_set_tangent_dir (dwg_ent_vertex_2d *restrict vert,
                                   const double tangent_dir,
                                   int *restrict error)
{
  if (vert)
    {
      *error = 0;
      vert->tangent_dir = tangent_dir;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/*******************************************************************
 *                   FUNCTIONS FOR INSERT ENTITY                     *
 ********************************************************************/

/** Returns the _dwg_entity_VERTEX_2D::ins_pt, DXF 10.
  \code Usage: dwg_ent_insert_get_ins_pt(insert, &point, &error);
  \endcode
  \param[in] insert dwg_ent_insert*
  \param[out] point dwg_point_3d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_insert_get_ins_pt (const dwg_ent_insert *restrict insert,
                           dwg_point_3d *restrict point, int *restrict error)
{
  if (insert
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = insert->ins_pt.x;
      point->y = insert->ins_pt.y;
      point->z = insert->ins_pt.z;
    }
  else
    {
      LOG_ERROR ("%s: empty insert", __FUNCTION__)
      *error = 1;
    }
}

/** Sets the  _dwg_entity_VERTEX_2D::ins_pt, DXF 10.
  \code Usage: dwg_ent_insert_set_ins_pt(insert, &point, &error);
  \endcode
  \param[out] insert dwg_ent_insert*
  \param[in] point dwg_point_3d
  \param error[out]   int*
*/
EXPORT void
dwg_ent_insert_set_ins_pt (dwg_ent_insert *restrict insert,
                           const dwg_point_3d *restrict point,
                           int *restrict error)
{
  if (insert
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      insert->ins_pt.x = point->x;
      insert->ins_pt.y = point->y;
      insert->ins_pt.z = point->z;
    }
  else
    {
      LOG_ERROR ("%s: empty insert", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the  _dwg_entity_INSERT::scale vector, DXF 41-43.
  \code Usage: dwg_ent_insert_get_scale(insert, &point, &error);
  \endcode
  \param[in]  insert   dwg_ent_insert*
  \param[out] scale3d  dwg_point_3d*
  \param[in]  error    int*
*/
EXPORT void
dwg_ent_insert_get_scale (const dwg_ent_insert *restrict insert,
                          dwg_point_3d *restrict scale3d, int *restrict error)
{
  if (insert
#    ifndef HAVE_NONNULL
      && scale3d
#    endif
  )
    {
      *error = 0;
      scale3d->x = insert->scale.x;
      scale3d->y = insert->scale.y;
      scale3d->z = insert->scale.z;
    }
  else
    {
      LOG_ERROR ("%s: empty insert", __FUNCTION__)
      *error = 1;
    }
}

/** Sets the _dwg_entity_INSERT::scale vector, DXF 41-43.
  \code Usage: dwg_ent_insert_set_scale(insert, &point, &error);
  \endcode
  \param[out] insert   dwg_ent_insert*
  \param[in]  scale3d  dwg_point_3d*
  \param[out] error    int*
*/
EXPORT void
dwg_ent_insert_set_scale (dwg_ent_insert *restrict insert,
                          const dwg_point_3d *restrict scale3d,
                          int *restrict error)
{
  if (insert
#    ifndef HAVE_NONNULL
      && scale3d
#    endif
  )
    {
      *error = 0;
      insert->scale.x = scale3d->x;
      insert->scale.y = scale3d->y;
      insert->scale.z = scale3d->z;
    }
  else
    {
      LOG_ERROR ("%s: empty insert", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the _dwg_entity_INSERT::rotation angle, DXF 50 (radians).
  \code Usage: double angle = dwg_ent_insert_get_rotation(insert, &error);
  \endcode
  \param[in] insert dwg_ent_insert*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
double
dwg_ent_insert_get_rotation (const dwg_ent_insert *restrict insert,
                             int *restrict error)
{
  if (insert)
    {
      *error = 0;
      return insert->rotation;
    }
  else
    {
      LOG_ERROR ("%s: empty insert", __FUNCTION__)
      *error = 1;
      return bit_nan ();
    }
}

/** Sets the _dwg_entity_INSERT::rotation angle, DXF 50 (radians).
  \code Usage: dwg_ent_insert_set_rotation(insert, angle, &error);
  \endcode
  \param[in] insert dwg_ent_insert*
  \param[in] rotation double
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_insert_set_rotation (dwg_ent_insert *restrict insert,
                             const double rotation, int *restrict error)
{
  if (insert)
    {
      *error = 0;
      insert->rotation = rotation;
    }
  else
    {
      LOG_ERROR ("%s: empty insert", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the _dwg_entity_INSERT::extrusion vector, DXF 210.
  \code Usage: dwg_ent_insert_get_extrusion(insert, &point, &error);
  \endcode
  \param[in]  insert  dwg_ent_insert*
  \param[out] vector  dwg_point_3d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_insert_get_extrusion (const dwg_ent_insert *restrict insert,
                              dwg_point_3d *restrict vector,
                              int *restrict error)
{
  if (insert
#    ifndef HAVE_NONNULL
      && vector
#    endif
  )
    {
      *error = 0;
      vector->x = insert->extrusion.x;
      vector->y = insert->extrusion.y;
      vector->z = insert->extrusion.z;
    }
  else
    {
      LOG_ERROR ("%s: empty insert", __FUNCTION__)
      *error = 1;
    }
}

/** Sets the _dwg_entity_INSERT::extrusion vector, DXF 210.
  \code Usage: dwg_ent_insert_set_extrusion(insert, &point, &error);
  \endcode
  \param[out] insert  dwg_ent_insert*
  \param[in]  vector  dwg_point_3d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_insert_set_extrusion (dwg_ent_insert *restrict insert,
                              const dwg_point_3d *restrict vector,
                              int *restrict error)
{
  if (insert
#    ifndef HAVE_NONNULL
      && vector
#    endif
  )
    {
      *error = 0;
      insert->extrusion.x = vector->x;
      insert->extrusion.y = vector->y;
      insert->extrusion.z = vector->z;
    }
  else
    {
      LOG_ERROR ("%s: empty insert", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the _dwg_entity_INSERT::has_attribs value, DXF 66.
  \code Usage: double attribs = dwg_ent_insert_has_attribs(insert, &error);
  \endcode
  \param[in] insert dwg_ent_insert*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
char
dwg_ent_insert_has_attribs (dwg_ent_insert *restrict insert,
                            int *restrict error)
{
  if (insert)
    {
      *error = 0;
      return insert->has_attribs;
    }
  else
    {
      LOG_ERROR ("%s: empty insert", __FUNCTION__)
      *error = 1;
      return '\0';
    }
}

/** Returns the  _dwg_entity_INSERT::num_owned count.
  \code Usage: BITCODE_BL count = dwg_ent_insert_get_num_owned(insert, &error);
  \endcode
  \param[in] insert dwg_ent_insert*
  \param[out] error int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT BITCODE_BL
dwg_ent_insert_get_num_owned (const dwg_ent_insert *restrict insert,
                              int *restrict error)
{
  if (insert)
    {
      *error = 0;
      return insert->num_owned;
    }
  else
    {
      LOG_ERROR ("%s: empty insert", __FUNCTION__)
      *error = 1;
      return 0L;
    }
}

/// FIXME needs to adjust handle array instead: add/delete
// TODO dwg_ent_insert_add_owned, dwg_ent_insert_delete_owned
// TODO get_name

/** Returns the _dwg_entity_INSERT::block_header.
  \code Usage: dwg_object* block_header =
dwg_ent_insert_get_block_header(insert, &error); \endcode \param[in]  insert
dwg_ent_insert* \param[out] error   int*, is set to 0 for ok, 1 on error
\deprecated
*/
EXPORT dwg_object *
dwg_ent_insert_get_block_header (const dwg_ent_insert *restrict insert,
                                 int *restrict error)
{
  if (insert)
    {
      *error = 0;
      return dwg_ref_get_object (insert->block_header, error);
    }
  else
    {
      LOG_ERROR ("%s: empty insert", __FUNCTION__)
      *error = 1;
      return NULL;
    }
}

/*******************************************************************
 *                  FUNCTIONS FOR MINSERT ENTITY                     *
 ********************************************************************/

/** Returns the _dwg_entity_MINSERT::ins_pt, DXF 10.
  \code Usage: dwg_ent_minsert_get_ins_pt(minsert, &point, &error);
  \endcode
  \param[in]  minsert dwg_ent_minsert*
  \param[out] point dwg_point_3d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_minsert_get_ins_pt (const dwg_ent_minsert *restrict minsert,
                            dwg_point_3d *restrict point, int *restrict error)
{
  if (minsert
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = minsert->ins_pt.x;
      point->y = minsert->ins_pt.y;
      point->z = minsert->ins_pt.z;
    }
  else
    {
      LOG_ERROR ("%s: empty insert", __FUNCTION__)
      *error = 1;
    }
}

/** Sets the _dwg_entity_MINSERT::ins_pt, DXF 10.
  \code Usage: dwg_ent_minsert_set_ins_pt(minsert, &point, &error);
  \endcode
  \param[out] minsert dwg_ent_minsert*
  \param[in]  point dwg_point_3d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_minsert_set_ins_pt (dwg_ent_minsert *restrict minsert,
                            const dwg_point_3d *restrict point,
                            int *restrict error)
{
  if (minsert
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      minsert->ins_pt.x = point->x;
      minsert->ins_pt.y = point->y;
      minsert->ins_pt.z = point->z;
    }
  else
    {
      LOG_ERROR ("%s: empty minsert", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the _dwg_entity_MINSERT::scale vector, DXF 41-43.
  \code Usage: dwg_ent_minsert_get_scale(minsert, &scale3d, &error);
  \endcode
  \param minsert[in]   dwg_ent_insert*
  \param scale3d[out]  dwg_point_3d*
  \param error[out]    int*
*/
EXPORT void
dwg_ent_minsert_get_scale (const dwg_ent_minsert *restrict minsert,
                           dwg_point_3d *restrict scale3d, int *restrict error)
{
  if (minsert
#    ifndef HAVE_NONNULL
      && scale3d
#    endif
  )
    {
      *error = 0;
      scale3d->x = minsert->scale.x;
      scale3d->y = minsert->scale.y;
      scale3d->z = minsert->scale.z;
    }
  else
    {
      LOG_ERROR ("%s: empty minsert", __FUNCTION__)
      *error = 1;
    }
}

/** Sets the _dwg_entity_MINSERT::scale vector, DXF 41-43. (TODO scale_flag)
  \code Usage: dwg_ent_minsert_set_scale(minsert, &scale3d, &error);
  \endcode
  \param minsert[out]  minsert  dwg_ent_insert*
  \param scale3d[in]   scale3d  dwg_point_3d*
  \param error[out]    int*
*/
EXPORT void
dwg_ent_minsert_set_scale (dwg_ent_minsert *restrict minsert,
                           const dwg_point_3d *restrict scale3d,
                           int *restrict error)
{
  if (minsert
#    ifndef HAVE_NONNULL
      && scale3d
#    endif
  )
    {
      *error = 0;
      // TODO: set scale_flag
      minsert->scale.x = scale3d->x;
      minsert->scale.y = scale3d->y;
      minsert->scale.z = scale3d->z;
    }
  else
    {
      LOG_ERROR ("%s: empty minsert", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the _dwg_entity_MINSERT::rotation angle, DXF 50. (radians)
  \code Usage: double angle = dwg_ent_minsert_get_rotation(minsert, &error);
  \endcode
  \param[in] minsert dwg_ent_minsert*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT double
dwg_ent_minsert_get_rotation (const dwg_ent_minsert *restrict minsert,
                              int *restrict error)
{
  if (minsert)
    {
      *error = 0;
      return minsert->rotation;
    }
  else
    {
      LOG_ERROR ("%s: empty minsert", __FUNCTION__)
      *error = 1;
      return bit_nan ();
    }
}

/** Sets the _dwg_entity_MINSERT::rotation angle, DXF 50. (radians)
  \code Usage: dwg_ent_minsert_set_rotation(minsert, angle, &error);
  \endcode
  \param[in] minsert dwg_ent_minsert*
  \param[in] double
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_minsert_set_rotation (dwg_ent_minsert *restrict minsert,
                              const double rot_ang, int *restrict error)
{
  if (minsert)
    {
      *error = 0;
      minsert->rotation = rot_ang;
    }
  else
    {
      LOG_ERROR ("%s: empty minsert", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the _dwg_entity_MINSERT::extrusion vector, DXF 210.
  \code Usage: dwg_ent_minsert_get_extrusion(minsert, &point, &error);
  \endcode
  \param[in]  minsert dwg_ent_minsert*
  \param[out] vector dwg_point_3d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_minsert_get_extrusion (const dwg_ent_minsert *restrict minsert,
                               dwg_point_3d *restrict vector,
                               int *restrict error)
{
  if (minsert
#    ifndef HAVE_NONNULL
      && vector
#    endif
  )
    {
      *error = 0;
      vector->x = minsert->extrusion.x;
      vector->y = minsert->extrusion.y;
      vector->z = minsert->extrusion.z;
    }
  else
    {
      LOG_ERROR ("%s: empty minsert", __FUNCTION__)
      *error = 1;
    }
}

/** Sets the _dwg_entity_MINSERT::extrusion vector, DXF 210.
  \code Usage: dwg_ent_minsert_set_extrusion(minsert, &point, &error);
  \endcode
  \param[out] minsert dwg_ent_minsert*
  \param[in]  vector dwg_point_3d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_minsert_set_extrusion (dwg_ent_minsert *restrict minsert,
                               const dwg_point_3d *restrict vector,
                               int *restrict error)
{
  if (minsert
#    ifndef HAVE_NONNULL
      && vector
#    endif
  )
    {
      *error = 0;
      minsert->extrusion.x = vector->x;
      minsert->extrusion.y = vector->y;
      minsert->extrusion.z = vector->z;
    }
  else
    {
      LOG_ERROR ("%s: empty minsert", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the _dwg_entity_MINSERT::has_attrib value, DXF 66
  \code Usage: double attribs = dwg_ent_minsert_has_attribs(mintrest, &error);
  \endcode
  \param[in] minsert dwg_ent_minsert*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT char
dwg_ent_minsert_has_attribs (dwg_ent_minsert *restrict minsert,
                             int *restrict error)
{
  if (minsert)
    {
      *error = 0;
      return minsert->has_attribs;
    }
  else
    {
      LOG_ERROR ("%s: empty minsert", __FUNCTION__)
      *error = 1;
      return '\0';
    }
}

/** Returns the  _dwg_entity_MINSERT::num_owned count, no DXF.
  \code Usage: BITCODE_BL count = dwg_ent_minsert_get_num_owned(minsert,
  &error); \endcode \param[in] minsert dwg_ent_minsert* \param[out] error int*,
  is set to 0 for ok, 1 on error \deprecated
*/
EXPORT BITCODE_BL
dwg_ent_minsert_get_num_owned (const dwg_ent_minsert *restrict minsert,
                               int *restrict error)
{
  if (minsert)
    {
      *error = 0;
      return minsert->num_owned;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}

/// FIXME needs to adjust attribs array: add/delete

// TODO dwg_ent_minsert_add_owned, dwg_ent_minsert_delete_owned

/** Sets the  _dwg_entity_MINSERT::num_cols count, DXF 70.
  \code Usage: dwg_ent_minsert_set_num_cols(minsert, 2, &error);
  \endcode
  \param[out] minsert dwg_ent_minsert*
  \param[in] num_cols
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_minsert_set_num_cols (dwg_ent_minsert *restrict minsert,
                              const BITCODE_BL num_cols, int *restrict error)
{
  if (minsert)
    {
      *error = 0;
      minsert->num_cols = num_cols;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the  _dwg_entity_MINSERT::num_cols count, DXF 70.
  \code Usage: BITCODE_BL num_cols = dwg_ent_minsert_get_num_cols(minsert,
&error); \endcode \param[in] minsert dwg_ent_minsert* \param[out] error   int*,
is set to 0 for ok, 1 on error \deprecated
*/
EXPORT BITCODE_BL
dwg_ent_minsert_get_num_cols (const dwg_ent_minsert *restrict minsert,
                              int *restrict error)
{
  if (minsert)
    {
      *error = 0;
      return minsert->num_cols;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}

/** Sets the  _dwg_entity_MINSERT::num_rows count, DXF 71.
  \code Usage: dwg_ent_minsert_set_num_rows(minsert, 2, &error);
  \endcode
  \param[out] minsert dwg_ent_minsert*
  \param[in]  num_rows
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_minsert_set_num_rows (dwg_ent_minsert *restrict minsert,
                              const BITCODE_BL num_rows, int *restrict error)
{
  if (minsert)
    {
      *error = 0;
      minsert->num_rows = num_rows;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the  _dwg_entity_MINSERT::num_rows count, DXF 71.
  \code Usage: BITCODE_BL num_rows = dwg_ent_minsert_get_num_rows(minsert,
&error); \endcode \param[in] minsert dwg_ent_minsert* \param[out] error   int*,
is set to 0 for ok, 1 on error \deprecated
*/
EXPORT BITCODE_BL
dwg_ent_minsert_get_num_rows (const dwg_ent_minsert *restrict minsert,
                              int *restrict error)
{
  if (minsert)
    {
      *error = 0;
      return minsert->num_rows;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}

/** Returns the _dwg_entity_MINSERT::col_spacing, DXF 44.
  \code Usage: double spacing = dwg_ent_minsert_get_col_spacing(minsert,
  &error); \endcode \param[in] minsert dwg_ent_minsert* \param[out] error int*,
  is set to 0 for ok, 1 on error \deprecated
*/
EXPORT double
dwg_ent_minsert_get_col_spacing (const dwg_ent_minsert *restrict minsert,
                                 int *restrict error)
{
  if (minsert)
    {
      *error = 0;
      return minsert->col_spacing;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets the _dwg_entity_MINSERT::col_spacing, DXF 44.
  \code Usage: dwg_ent_minsert_set_col_spacing(minsert, 20, &error);
  \endcode
  \param[out] minsert dwg_ent_insert*
  \param[in]  spacing double
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_minsert_set_col_spacing (dwg_ent_minsert *restrict minsert,
                                 const double spacing, int *restrict error)
{
  if (minsert)
    {
      *error = 0;
      minsert->col_spacing = spacing;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_MINSERT::row_spacing, DXF 45.
  \code Usage: double spacing = dwg_ent_minsert_get_row_spacing(minsert,
  &error); \endcode \param[in] minsert dwg_ent_minsert* \param[out] error int*,
  is set to 0 for ok, 1 on error \deprecated
*/
EXPORT double
dwg_ent_minsert_get_row_spacing (const dwg_ent_minsert *restrict minsert,
                                 int *restrict error)
{
  if (minsert)
    {
      *error = 0;
      return minsert->row_spacing;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets the  _dwg_entity_MINSERT::row_spacing, DXF 45.
  \code Usage: dwg_ent_minsert_set_row_spacing(minsert, 20, &error);
  \endcode
  \param[out] insert dwg_ent_insert*
  \param[in]  spacing double
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_minsert_set_row_spacing (dwg_ent_minsert *restrict minsert,
                                 const double spacing, int *restrict error)
{
  if (minsert)
    {
      *error = 0;
      minsert->row_spacing = spacing;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the  _dwg_entity_MINSERT::block_header object, DXF 2.
  \code Usage: dwg_object* block_header =
dwg_ent_minsert_get_block_header(minsert, &error); \endcode \param[in]  minsert
dwg_ent_minsert* \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT dwg_object *
dwg_ent_minsert_get_block_header (const dwg_ent_minsert *restrict minsert,
                                  int *restrict error)
{
  if (minsert)
    {
      *error = 0;
      return dwg_ref_get_object (minsert->block_header, error);
    }
  else
    {
      LOG_ERROR ("%s: empty insert", __FUNCTION__)
      *error = 1;
      return NULL;
    }
}

/*******************************************************************
 *                FUNCTIONS FOR MLINESTYLE OBJECT                    *
 ********************************************************************/

/** Returns the  _dwg_object_MLINESTYLE::name, DXF 2 (utf-8 encoded)
  \code Usage: char * name = dwg_obj_mlinestyle_get_name(mlinestyle, &error);
  \endcode
  \param[in]  mlinestyle dwg_obj_mlinestyle
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT char *
dwg_obj_mlinestyle_get_name (const dwg_obj_mlinestyle *restrict mlinestyle,
                             int *restrict error)
{
  if (mlinestyle)
    {
      *error = 0;
      if (dwg_version >= R_2007)
        return bit_convert_TU ((BITCODE_TU)mlinestyle->name);
      else
        return mlinestyle->name;
    }
  else
    {
      LOG_ERROR ("%s: empty mlinestyle", __FUNCTION__)
      *error = 1;
      return NULL;
    }
}

/** Sets the _dwg_object_MLINESTYLE::name
  \code Usage: dwg_obj_mlinestyle_set_name(minsert, "mstylename", &error);
  \endcode
  \param[out] mlinestyle dwg_obj_mlinestyle
  \param[in]  name char *
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_obj_mlinestyle_set_name (dwg_obj_mlinestyle *restrict mlinestyle,
                             const char *restrict name, int *restrict error)
{
  Dwg_Data *dwg = dwg_obj_generic_dwg (mlinestyle, error);
  if (mlinestyle && !error)
    {
      mlinestyle->name = dwg_add_u8_input (dwg, name);
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_object_MLINESTYLE::desc, DXF 3 (utf-8 encoded)
  \code Usage: char * desc = dwg_obj_mlinestyle_get_desc(mlinestyle, &error);
  \endcode
  \param[in]  mlinestyle dwg_obj_mlinestyle
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT char *
dwg_obj_mlinestyle_get_desc (const dwg_obj_mlinestyle *restrict mlinestyle,
                             int *restrict error)
{
  if (mlinestyle)
    {
      *error = 0;
      if (dwg_version >= R_2007)
        return bit_convert_TU ((BITCODE_TU)mlinestyle->description);
      else
        return mlinestyle->description;
    }
  else
    {
      LOG_ERROR ("%s: empty mlinestyle", __FUNCTION__)
      *error = 1;
      return NULL;
    }
}

/** Sets the _dwg_object_MLINESTYLE::desc, DXF 3. (utf-8 encoded)
  \code Usage: dwg_obj_mlinestyle_set_desc(minsert, desc, &error);
  \endcode
  \param[out] mlinestyle dwg_obj_mlinestyle
  \param[in] desc utf-8 char *
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_obj_mlinestyle_set_desc (dwg_obj_mlinestyle *restrict mlinestyle,
                             const char *restrict desc, int *restrict error)
{
  Dwg_Data *dwg = dwg_obj_generic_dwg (mlinestyle, error);
  if (mlinestyle && !error)
    {
      mlinestyle->description = dwg_add_u8_input (dwg, desc);
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_object_MLINESTYLE::flag, DXF 70
  \code Usage: int flag = dwg_obj_mlinestyle_get_flag(minsert, &error);
  \endcode
  \param[in]  mlinestyle dwg_obj_mlinestyle
  \param[out] error      int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT int
dwg_obj_mlinestyle_get_flag (const dwg_obj_mlinestyle *restrict mlinestyle,
                             int *restrict error)
{
  if (mlinestyle)
    {
      *error = 0;
      return mlinestyle->flag;
    }
  else
    {
      LOG_ERROR ("%s: empty mlinestyle", __FUNCTION__)
      *error = 1;
      return '\0';
    }
}

/** Sets the _dwg_object_MLINESTYLE::flag, DXF 70
  \code Usage: dwg_obj_mlinestyle_set_flag(mlinestyle, 1+2, &error);
  \endcode
  \param[out] mlinestyle  dwg_ent_mlinestyle*
  \param[in]  flags int
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_obj_mlinestyle_set_flag (dwg_obj_mlinestyle *restrict mlinestyle,
                             const int flags, int *restrict error)
{
  if (mlinestyle)
    {
      *error = 0;
      mlinestyle->flag = flags;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_object_MLINESTYLE::start_angle, DXF 51 (radians)
  \code Usage: double start_angle =
  dwg_obj_mlinestyle_get_start_angle(mlinestyle, &error); \endcode \param[in]
  mlinestyle  dwg_obj_mlinestyle \param[out] error   int*, is set to 0 for ok,
  1 on error \deprecated
*/
EXPORT double
dwg_obj_mlinestyle_get_start_angle (
    const dwg_obj_mlinestyle *restrict mlinestyle, int *restrict error)
{
  if (mlinestyle)
    {
      *error = 0;
      return mlinestyle->start_angle;
    }
  else
    {
      LOG_ERROR ("%s: empty mlinestyle", __FUNCTION__)
      *error = 1;
      return '\0';
    }
}

/** Sets the _dwg_object_MLINESTYLE::start_angle, DXF 51 (radians)
  \code Usage: dwg_obj_mlinestyle_set_start_angle(mlinestyle, M_PI_2, &error);
  \endcode
  \param[out] mlinestyle  dwg_obj_mlinestyle
  \param[in]  start_angle double
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_obj_mlinestyle_set_start_angle (dwg_obj_mlinestyle *restrict mlinestyle,
                                    const double start_angle,
                                    int *restrict error)
{
  if (mlinestyle)
    {
      *error = 0;
      mlinestyle->start_angle = start_angle;
    }
  else
    {
      LOG_ERROR ("%s: empty mlinestyle", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the _dwg_object_MLINESTYLE::end_angle, DXF 52 (radians)
  \code Usage: double angle = dwg_obj_mlinestyle_get_end_angle(mlinestyle,
&error); \endcode \param[in]  mlinestyle dwg_obj_mlinestyle \param[out] error
int*, is set to 0 for ok, 1 on error \deprecated
*/
EXPORT double
dwg_obj_mlinestyle_get_end_angle (
    const dwg_obj_mlinestyle *restrict mlinestyle, int *restrict error)
{
  if (mlinestyle)
    {
      *error = 0;
      return mlinestyle->end_angle;
    }
  else
    {
      LOG_ERROR ("%s: empty mlinestyle", __FUNCTION__)
      *error = 1;
      return '\0';
    }
}

/** Sets the _dwg_object_MLINESTYLE::end_angle, DXF 51 (radians)
  \code Usage: dwg_obj_mlinestyle_set_end_angle(mlinestyle, M_PI_2, &error);
  \endcode
  \param[out] mlinestyle dwg_obj_mlinestyle
  \param[in]  end_angle double
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_obj_mlinestyle_set_end_angle (dwg_obj_mlinestyle *restrict mlinestyle,
                                  const double end_angle, int *restrict error)
{
  if (mlinestyle)
    {
      *error = 0;
      mlinestyle->end_angle = end_angle;
    }
  else
    {
      LOG_ERROR ("%s: empty mlinestyle", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the _dwg_object_MLINESTYLE::num_lines, DXF 71.
  \code Usage: char lines = dwg_obj_mlinestyle_get_num_lines(mlinestyle,
  &error); \endcode \param[in]  mlinestyle dwg_obj_mlinestyle \param[out] error
  int*, is set to 0 for ok, 1 on error \deprecated
*/
EXPORT char
dwg_obj_mlinestyle_get_num_lines (
    const dwg_obj_mlinestyle *restrict mlinestyle, int *restrict error)
{
  if (mlinestyle)
    {
      *error = 0;
      return mlinestyle->num_lines;
    }
  else
    {
      LOG_ERROR ("%s: empty mlinestyle", __FUNCTION__)
      *error = 1;
      return '\0';
    }
}

/*******************************************************************
 *               FUNCTIONS FOR APPID_CONTROL OBJECT                  *
 ********************************************************************/

/** Returns the _dwg_entity_APPID_CONTROL::num_entries, DXF 70.
  \code Usage: int num = dwg_obj_appid_control_get_num_entries(appcontrol,
&error); \endcode \param[in]  appid dwg_obj_appid_control \param[out] error
int*, is set to 0 for ok, 1 on error \deprecated
*/
EXPORT BITCODE_BS
dwg_obj_appid_control_get_num_entries (
    const dwg_obj_appid_control *restrict appid, int *restrict error)
{
  if (appid)
    {
      *error = 0;
      return appid->num_entries;
    }
  else
    {
      LOG_ERROR ("%s: empty appid", __FUNCTION__)
      *error = 1;
      return 0;
    }
}

/** Returns the idx'th _dwg_entity_APPID:: object.
  \code Usage: int num = dwg_obj_appid_control_get_appid(appcontrol, 0,
  &error); \endcode \param[in]  appid dwg_obj_appid_control* \param[in]  idx
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT dwg_object_ref *
dwg_obj_appid_control_get_appid (const dwg_obj_appid_control *restrict appid,
                                 const BITCODE_BS idx, int *restrict error)
{
  if (appid != NULL && idx < appid->num_entries)
    {
      *error = 0;
      return appid->entries[idx];
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return NULL;
    }
}

/*******************************************************************
 *                    FUNCTIONS FOR APPID OBJECT                     *
 ********************************************************************/

/** Returns the _dwg_entity_APPID::name, DXF 2. (utf-8 encoded)
  \code Usage: char * name = dwg_obj_appid_get_name(mlinestyle, &error);
  \endcode
  \param[in]  appid  dwg_obj_appid
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT char *
dwg_obj_appid_get_name (const dwg_obj_appid *restrict appid,
                        int *restrict error)
{
  if (appid)
    {
      *error = 0;
      if (dwg_version >= R_2007)
        return bit_convert_TU ((BITCODE_TU)appid->name);
      else
        return appid->name;
    }
  else
    {
      LOG_ERROR ("%s: empty appid", __FUNCTION__)
      *error = 1;
      return NULL;
    }
}

/** Returns the _dwg_entity_APPID::flag, DXF 70.
  \code Usage: char flag = dwg_obj_appid_get_flag(appid, &error);
  \endcode
  \param[in]  appid   dwg_obj_appid
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT BITCODE_RC
dwg_obj_appid_get_flag (const dwg_obj_appid *restrict appid,
                        int *restrict error)
{
  if (appid)
    {
      *error = 0;
      return appid->flag & 1 || appid->is_xref_ref >> 4
             || appid->is_xref_dep >> 6;
    }
  else
    {
      LOG_ERROR ("%s: empty appid", __FUNCTION__)
      *error = 1;
      return 0;
    }
}

/** Sets the _dwg_entity_APPID::flag, DXF 70.
    and the other related xref flags.
  \code Usage: dwg_obj_appid_set_flag(appid, flag, &error);
  \endcode
  \param[out] appid  dwg_obj_appid*
  \param[in]  flag
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_obj_appid_set_flag (dwg_obj_appid *restrict appid, const BITCODE_RC flag,
                        int *restrict error)
{
  if (appid)
    {
      *error = 0;
      appid->flag = flag;
      appid->is_xref_ref = flag & 16;
      appid->is_xref_dep = flag & 64;
    }
  else
    {
      LOG_ERROR ("%s: empty appid", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the  _dwg_entity_APPID_CONTROL:: object for the appid object.
  \code Usage: dwg_obj_appid_control* appid =
dwg_obj_appid_get_appid_control(appid, &error); \endcode \param[in]  appid
dwg_obj_appid* \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
EXPORT */
dwg_obj_appid_control *
dwg_obj_appid_get_appid_control (const dwg_obj_appid *restrict appid,
                                 int *restrict error)
{
  if (appid)
    {
      *error = 0;
      return appid->parent->ownerhandle->obj->tio.object->tio.APPID_CONTROL;
    }
  else
    {
      LOG_ERROR ("%s: empty appid", __FUNCTION__)
      *error = 1;
      return NULL;
    }
}

/*******************************************************************
 *            FUNCTIONS FOR ALL DIMENSION ENTITIES                *
 ********************************************************************/

/** Returns the _dwg_entity_DIMENSION_common:::: block name, DXF 2. (utf-8
encoded) \code Usage: char * name = dwg_ent_dim_get_block_name(dim, &error);
  \endcode
  \param[in]  dim     dwg_ent_dim *
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT char *
dwg_ent_dim_get_block_name (const dwg_ent_dim *restrict dim,
                            int *restrict error)
{
  if (dim)
    {
      char *name = ((dwg_ent_dim_linear *)dim)
                       ->block->obj->tio.object->tio.BLOCK_HEADER->name;
      *error = 0;
      if (dwg_version >= R_2007)
        return bit_convert_TU ((BITCODE_TU)name);
      else
        return name;
    }
  else
    {
      LOG_ERROR ("%s: empty dim", __FUNCTION__)
      *error = 1;
      return NULL;
    }
}

/** Returns the _dwg_entity_DIMENSION_common::elevation, the z-coord for all
EXPORT 11,12, 16 ECS points. \code Usage: double elevation =
dwg_ent_dim_get_elevation(dim, &error); \endcode \param[in]  dim dwg_ent_dim*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT double
dwg_ent_dim_get_elevation (const dwg_ent_dim *restrict dim,
                           int *restrict error)
{
  if (dim)
    {
      *error = 0;
      return dim->elevation;
    }
  else
    {
      LOG_ERROR ("%s: empty dim", __FUNCTION__)
      *error = 1;
      return bit_nan ();
    }
}

/** Sets the _dwg_entity_DIMENSION_common::elevation for the 11, 12, 16 ECS
points \code Usage: dwg_ent_dim_set_elevation(dim, z, &error); \endcode
  \param[out] dim       dwg_ent_dim*
  \param[in]  elevation double
  \param[out] error     int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_dim_set_elevation (dwg_ent_dim *restrict dim, const double elevation,
                           int *restrict error)
{
  if (dim)
    {
      *error = 0;
      dim->elevation = elevation;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_DIMENSION_common::flag1, DXF 70.
  \code Usage: char flag1 = dwg_ent_dim_get_flag1(dim, &error);
  \endcode
  \param[in]  dim     dwg_ent_dim*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT char
dwg_ent_dim_get_flag1 (const dwg_ent_dim *restrict dim, int *restrict error)
{
  if (dim)
    {
      *error = 0;
      return dim->flag1;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/** Sets the _dwg_entity_DIMENSION_common::flag1, DXF 70.
  \code Usage: dwg_ent_dim_set_flag1(dim, flag1, &error);
  \endcode
  \param[in] dim dwg_ent_dim*
  \param 2 char
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_dim_set_flag1 (dwg_ent_dim *restrict dim, const char flag,
                       int *restrict error)
{
  if (dim)
    {
      *error = 0;
      dim->flag1 = flag;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_DIMENSION_common::act_measurement, DXF 42.
  \code Usage: double measure = dwg_ent_dim_get_act_measurement(dim, &error);
  \endcode
  \param[in] dim dwg_ent_dim*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT double
dwg_ent_dim_get_act_measurement (const dwg_ent_dim *restrict dim,
                                 int *restrict error)
{
  if (dim)
    {
      *error = 0;
      return dim->act_measurement;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets the _dwg_entity_DIMENSION_common::act_measurement, DXF 42.
  \code Usage: dwg_ent_dim_set_act_measurement(dim, measure, &error);
  \endcode
  \param[out]  dim     dwg_ent_dim*
  \param[in]   act_measurement double
  \param[out]  error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_dim_set_act_measurement (dwg_ent_dim *restrict dim,
                                 double act_measurement, int *restrict error)
{
  if (dim)
    {
      *error = 0;
      dim->act_measurement = act_measurement;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_DIMENSION_common::horiz_dir, DXF 51.
  \code Usage: double horiz_dir = dwg_ent_dim_get_horiz_dir(dim, &error);
  \endcode
  \param[in]  dim     dwg_ent_dim*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT double
dwg_ent_dim_get_horiz_dir (const dwg_ent_dim *restrict dim,
                           int *restrict error)
{
  if (dim)
    {
      *error = 0;
      return dim->horiz_dir;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets the _dwg_entity_DIMENSION_common::horiz_dir, DXF 51.
  \code Usage: dwg_ent_dim_set_horiz_dir(dim, horiz_dir, &error);
  \endcode
  \param[out] dim      dwg_ent_dim*
  \param[in]  horiz_dir  double
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_dim_set_horiz_dir (dwg_ent_dim *restrict dim, const double horiz_dir,
                           int *restrict error)
{
  if (dim)
    {
      *error = 0;
      dim->horiz_dir = horiz_dir;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_DIMENSION_common::lspace_factor, DXF 41.
  \code Usage: double lspace_factor = dwg_ent_dim_get_lspace_factor(dim,
  &error); \endcode \param[in]  dim dwg_ent_dim* \param[out] error   int*, is
  set to 0 for ok, 1 on error \deprecated
*/
EXPORT double
dwg_ent_dim_get_lspace_factor (const dwg_ent_dim *restrict dim,
                               int *restrict error)
{
  if (dim)
    {
      *error = 0;
      return dim->lspace_factor;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets the _dwg_entity_DIMENSION_common::lspace_factor, DXF 41.
  \code Usage: dwg_ent_dim_set_lspace_factor(dim, factor, &error);
  \endcode
  \param[out] dim     dwg_ent_dim*
  \param[in] factor   double
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_dim_set_lspace_factor (dwg_ent_dim *restrict dim, const double factor,
                               int *restrict error)
{
  if (dim)
    {
      *error = 0;
      dim->lspace_factor = factor;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_DIMENSION_common::lspace_style idx, DXF 72.
  \code Usage: BITCODE_BS lspace_style = dwg_ent_dim_get_lspace_style(dim,
&error); \endcode \param[in] dim dwg_ent_dim* \param[out] error   int*, is set
to 0 for ok, 1 on error \deprecated
*/
EXPORT BITCODE_BS
dwg_ent_dim_get_lspace_style (const dwg_ent_dim *restrict dim,
                              int *restrict error)
{
  if (dim)
    {
      *error = 0;
      return dim->lspace_style;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/** Sets the _dwg_entity_DIMENSION_common::lspace_style idx, DXF 72.
  \code Usage: dwg_ent_dim_set_lspace_style(dim, style, &error);
  \endcode
  \param[out] dim     dwg_ent_dim*
  \param[in]  style   BITCODE_BS
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_dim_set_lspace_style (dwg_ent_dim *restrict dim,
                              const BITCODE_BS style, int *restrict error)
{
  if (dim)
    {
      *error = 0;
      dim->lspace_style = style;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_DIMENSION_common::attachment idx, DXF 71.
  \code Usage: BITCODE_BS attachment = dwg_ent_dim_get_attachment(dim, &error);
  \endcode
  \param[in] dim dwg_ent_dim*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT BITCODE_BS
dwg_ent_dim_get_attachment (const dwg_ent_dim *restrict dim,
                            int *restrict error)
{
  if (dim)
    {
      *error = 0;
      return dim->attachment;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets the _dwg_entity_DIMENSION_common::attachment idx, DXF 71.
  \code Usage: dwg_ent_dim_set_attachment(dim, attachment, &error);
  \endcode
  \param[out] dim        dwg_ent_dim*
  \param[in]  attachment BITCODE_BS point idx
  \param[out] error      int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_dim_set_attachment (dwg_ent_dim *restrict dim,
                            const BITCODE_BS attachment, int *restrict error)
{
  if (dim)
    {
      *error = 0;
      dim->attachment = attachment;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the _dwg_entity_DIMENSION_common::extrusion, DXF 210.
  \code Usage: dwg_ent_dim_get_extrusion(dim, &point, &error);
  \endcode
  \param[out] dim     dwg_ent_dim*
  \param[in]  vector  dwg_point_3d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_dim_set_extrusion (dwg_ent_dim *restrict dim,
                           const dwg_point_3d *restrict vector,
                           int *restrict error)
{
  if (dim
#    ifndef HAVE_NONNULL
      && vector
#    endif
  )
    {
      *error = 0;
      dim->extrusion.x = vector->x;
      dim->extrusion.y = vector->y;
      dim->extrusion.z = vector->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the _dwg_entity_DIMENSION_common::extrusion, DXF 210.
  \code Usage: dwg_ent_dim_set_extrusion(dim, &point, &error);
  \endcode
  \param[in]  dim     dwg_ent_dim*
  \param[out] vector  dwg_point_3d*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_dim_get_extrusion (const dwg_ent_dim *restrict dim,
                           dwg_point_3d *restrict vector, int *restrict error)
{
  if (dim
#    ifndef HAVE_NONNULL
      && vector
#    endif
  )
    {
      *error = 0;
      vector->x = dim->extrusion.x;
      vector->y = dim->extrusion.y;
      vector->z = dim->extrusion.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_DIMENSION_common::user_text, DXF 1. (utf-8 encoded)
  \code Usage: char * text  = dwg_ent_dim_get_user_text(dim, &error);
  \endcode
  \param[in]  dim dwg_ent_dim*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT char *
dwg_ent_dim_get_user_text (const dwg_ent_dim *restrict dim,
                           int *restrict error)
{
  if (dim)
    {
      *error = 0;
      if (dwg_version >= R_2007)
        return bit_convert_TU ((BITCODE_TU)dim->user_text);
      else
        return dim->user_text;
    }
  else
    {
      LOG_ERROR ("%s: empty dim", __FUNCTION__)
      *error = 1;
      return NULL;
    }
}

/** Sets the _dwg_entity_DIMENSION_common::user_text, DXF 1. (utf-8 encoded)
  \code Usage: dwg_ent_dim_set_user_text(dim, "dimension text", &error);
  \endcode
  \param[out] dim    dwg_ent_dim*
  \param[in]  text   char*
  \param[out] error  int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_dim_set_user_text (dwg_ent_dim *restrict dim,
                           const char *restrict text, int *restrict error)
{
  Dwg_Data *dwg = dwg_obj_generic_dwg (ent, error);
  if (ent && !error)
    {
      ent->user_text = dwg_add_u8_input (dwg, text);
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_DIMENSION_common::text_rotation, DXF 53 (radian).
  \code Usage: double text_rot  = dwg_ent_dim_get_text_rotation(dim, &error);
  \endcode
  \param[in]  dim     dwg_ent_dim*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT double
dwg_ent_dim_get_text_rotation (const dwg_ent_dim *restrict dim,
                               int *restrict error)
{
  if (dim)
    {
      *error = 0;
      return dim->text_rotation;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets the _dwg_entity_DIMENSION_common::text_rotation, DXF 53 (radian).
  \code Usage: dwg_ent_dim_set_text_rotation(dim, 0.0, &error);
  \endcode
  \param[out] dim      dwg_ent_dim*
  \param[in]  rotation double
  \param[out] error    int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_dim_set_text_rotation (dwg_ent_dim *restrict dim,
                               const double rotation, int *restrict error)
{
  if (dim)
    {
      *error = 0;
      dim->text_rotation = rotation;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_DIMENSION_common::ins_rotation, DXF 54 (radian).
  \code Usage: double ins_rot  = dwg_ent_dim_get_ins_rotation(dim, &error);
  \endcode
  \param[in] dim dwg_ent_dim*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT double
dwg_ent_dim_get_ins_rotation (const dwg_ent_dim *restrict dim,
                              int *restrict error)
{
  if (dim)
    {
      *error = 0;
      return dim->ins_rotation;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets the _dwg_entity_DIMENSION_common::ins_rotation, DXF 54 (radian).
  \code Usage: dwg_ent_dim_set_ins_rotation(dim, 0.0, &error);
  \endcode
  \param[out] dim      dwg_ent_dim*
  \param[in]  rotation double
  \param[out] error    int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_dim_set_ins_rotation (dwg_ent_dim *restrict dim, const double rotation,
                              int *restrict error)
{
  if (dim)
    {
      *error = 0;
      dim->ins_rotation = rotation;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_DIMENSION_common::flip_arrow1, DXF 74
  \code Usage: char arrow1 = dwg_ent_dim_get_flip_arrow1(dim, &error);
  \endcode
  \param[in]  dim     dwg_ent_dim*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT char
dwg_ent_dim_get_flip_arrow1 (const dwg_ent_dim *restrict dim,
                             int *restrict error)
{
  if (dim)
    {
      *error = 0;
      return dim->flip_arrow1;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/** Sets the _dwg_entity_DIMENSION_common::flip_arrow1 to 1 or 0, DXF 74.
  \code Usage: dwg_ent_dim_set_flip_arrow1(dim, arrow1, &error);
  \endcode
  \param[out] dim        dwg_ent_dim*
  \param[in]  flip_arrow char
  \param[out] error      int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_dim_set_flip_arrow1 (dwg_ent_dim *restrict dim, const char flip_arrow,
                             int *restrict error)
{
  if (dim && (flip_arrow == 0 || flip_arrow == 1))
    {
      *error = 0;
      dim->flip_arrow1 = flip_arrow;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_DIMENSION_common::flip_arrow2 to 1 or 0, DXF 75.
  \code Usage: has_arrow2 = dwg_ent_dim_get_flip_arrow2(dim, &error);
  \endcode
  \param[in] dim dwg_ent_dim*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT char
dwg_ent_dim_get_flip_arrow2 (const dwg_ent_dim *restrict dim,
                             int *restrict error)
{
  if (dim)
    {
      *error = 0;
      return dim->flip_arrow2;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/** Sets the _dwg_entity_DIMENSION_common::flip_arrow1 to 1 or 0, DXF 75.
  \code Usage: dwg_ent_dim_set_flip_arrow2(dim, arrow2, &error);
  \endcode
  \param[out] dim        dwg_ent_dim*
  \param[in]  flip_arrow char
  \param[out] error      int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_dim_set_flip_arrow2 (dwg_ent_dim *restrict dim, const char flip_arrow,
                             int *restrict error)
{
  if (dim && (flip_arrow == 0 || flip_arrow == 1))
    {
      *error = 0;
      dim->flip_arrow2 = flip_arrow;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_DIMENSION_common::text_midpt, DXF 11.
  \code Usage: dwg_ent_dim_get_text_midpt(dim, &point, &error);
  \endcode
  \param[out] dim     dwg_ent_dim*
  \param[in]  point   dwg_point_2d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_dim_set_text_midpt (dwg_ent_dim *restrict dim,
                            const dwg_point_2d *restrict point,
                            int *restrict error)
{
  if (dim
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      dim->text_midpt.x = point->x;
      dim->text_midpt.y = point->y;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the  _dwg_entity_DIMENSION_common::text_midpt, DXF 11.
  \code Usage: dwg_ent_dim_set_text_mid_pt(dim, &point, &error);
  \endcode
  \param[in]  dim     dwg_ent_dim*
  \param[out] point   dwg_point_2d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_dim_get_text_midpt (const dwg_ent_dim *restrict dim,
                            dwg_point_2d *restrict point, int *restrict error)
{
  if (dim
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = dim->text_midpt.x;
      point->y = dim->text_midpt.y;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the  _dwg_entity_DIMENSION_common::ins_scale vector, DXF 41.
  \code Usage: dwg_ent_dim_set_ins_scale(dim, &point, &error);
  \endcode
  \param[out] dim     dwg_ent_dim*
  \param[in]  scale3d dwg_point_3d*
  \param[out] error   int*
*/
EXPORT void
dwg_ent_dim_set_ins_scale (dwg_ent_dim *restrict dim,
                           const dwg_point_3d *restrict scale3d,
                           int *restrict error)
{
  if (dim
#    ifndef HAVE_NONNULL
      && scale3d
#    endif
  )
    {
      *error = 0;
      dim->ins_scale.x = scale3d->x;
      dim->ins_scale.y = scale3d->y;
      dim->ins_scale.z = scale3d->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_DIMENSION_common::ins_scale vector, DXF 41.
  \code Usage: dwg_ent_dim_get_ins_scale(dim, &point, &error);
  \endcode
  \param dim[in]      dwg_ent_dim*
  \param scale3d[out] dwg_point_3d*
  \param error[out]   int*
*/
EXPORT void
dwg_ent_dim_get_ins_scale (const dwg_ent_dim *restrict dim,
                           dwg_point_3d *restrict scale3d, int *restrict error)
{
  if (dim
#    ifndef HAVE_NONNULL
      && scale3d
#    endif
  )
    {
      *error = 0;
      scale3d->x = dim->ins_scale.x;
      scale3d->y = dim->ins_scale.y;
      scale3d->z = dim->ins_scale.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the _dwg_entity_DIMENSION_common::clone_ins_pt, DXF 12.
  \code Usage: dwg_ent_dim_set_clone_ins_pt(dim, &point, &error);
  \endcode
  \param[out] dim    dwg_ent_dim*
  \param[in]  point  dwg_point_2d
  \param[out] error  int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_dim_set_clone_ins_pt (dwg_ent_dim *restrict dim,
                              const dwg_point_2d *restrict point,
                              int *restrict error)
{
  if (dim
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      dim->clone_ins_pt.x = point->x;
      dim->clone_ins_pt.y = point->y;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_DIMENSION_common::clone_ins_pt, DXF 12.
  \code Usage: dwg_ent_dim_get_clone_ins_pt(dim, &point, &error);
  \endcode
  \param[in]  dim    dwg_ent_dim*
  \param[out] point  dwg_point_2d
  \param[out] error  int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_dim_get_clone_ins_pt (const dwg_ent_dim *restrict dim,
                              dwg_point_2d *restrict point,
                              int *restrict error)
{
  if (dim
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = dim->clone_ins_pt.x;
      point->y = dim->clone_ins_pt.y;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/*******************************************************************
 *              FUNCTIONS FOR ORDINATE DIMENSION ENTITY             *
 ********************************************************************/

/** Returns the _dwg_entity_DIMENSION_ORDINATE::flag2, DXF 70.
  \code Usage: char flag2 = dwg_ent_dim_ordinate_get_flag2(dim, &error);
  \endcode
  \param[in]  dim     dwg_ent_dim_ordinate*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT char
dwg_ent_dim_ordinate_get_flag2 (const dwg_ent_dim_ordinate *restrict dim,
                                int *restrict error)
{
  if (dim)
    {
      *error = 0;
      return dim->flag2;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/** Sets _dwg_entity_DIMENSION_ORDINATE::flag2, DXF 70.
  \code Usage: dwg_ent_dim_ordinate_set_flag2(dim, flag2, &error);
  \endcode
  \param[out] dim    dwg_ent_dim_ordinate*
  \param[in]  flag   char
  \param[out] error  int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_dim_ordinate_set_flag2 (dwg_ent_dim_ordinate *restrict dim,
                                const char flag, int *restrict error)
{
  if (dim)
    {
      *error = 0;
      dim->flag2 = flag;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the 10 ucsorigin point
  \code Usage: dwg_ent_dim_ordinate_set_def_pt(dim, &point, &error);
  \endcode
  \param[in] dim_ordinate dwg_ent_dim_ordinate*
  \param[out] dwg_point_3d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_dim_ordinate_set_def_pt (dwg_ent_dim_ordinate *restrict dim,
                                 const dwg_point_3d *restrict point,
                                 int *restrict error)
{
  if (dim
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      dim->def_pt.x = point->x;
      dim->def_pt.y = point->y;
      dim->def_pt.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the 10 def point (ucsorigin)
  \code Usage: dwg_ent_dim_ordinate_get_def_pt(dim, &point, &error);
  \endcode
  \param[in] dim_ordinate dwg_ent_dim_ordinate*
  \param[out] dwg_point_3d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_dim_ordinate_get_def_pt (const dwg_ent_dim_ordinate *restrict dim,
                                 dwg_point_3d *restrict point,
                                 int *restrict error)
{
  if (dim
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = dim->def_pt.x;
      point->y = dim->def_pt.y;
      point->z = dim->def_pt.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the 13 feature_location point
  \code Usage: dwg_ent_dim_ordinate_set_feature_location_pt(dim, &point,
  &error); \endcode \param[in] dim_ordinate dwg_ent_dim_ordinate* \param[out]
  dwg_point_3d \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_dim_ordinate_set_feature_location_pt (
    dwg_ent_dim_ordinate *restrict dim, const dwg_point_3d *restrict point,
    int *restrict error)
{
  if (dim
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      dim->feature_location_pt.x = point->x;
      dim->feature_location_pt.y = point->y;
      dim->feature_location_pt.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the 13 feature_location point
  \code Usage: dwg_ent_dim_ordinate_get_feature_location_pt(dim, &point,
  &error); \endcode \param[in] dim_ordinate dwg_ent_dim_ordinate* \param[out]
  dwg_point_3d \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_dim_ordinate_get_feature_location_pt (
    const dwg_ent_dim_ordinate *restrict dim, dwg_point_3d *restrict point,
    int *restrict error)
{
  if (dim
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = dim->feature_location_pt.x;
      point->y = dim->feature_location_pt.y;
      point->z = dim->feature_location_pt.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the 14 leader_endpt
  \code Usage: dwg_ent_dim_ordinate_set_leader_endpt(dim, &point, &error);
  \endcode
  \param[in] dim_ordinate dwg_ent_dim_ordinate*
  \param[out] dwg_point_3d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_dim_ordinate_set_leader_endpt (dwg_ent_dim_ordinate *restrict dim,
                                       const dwg_point_3d *restrict point,
                                       int *restrict error)
{
  if (dim
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      dim->leader_endpt.x = point->x;
      dim->leader_endpt.y = point->y;
      dim->leader_endpt.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the 14 leader_endpoint point
  \code Usage: dwg_ent_dim_ordinate_get_leader_endpt(dim, &point, &error);
  \endcode
  \param[in] dim_ordinate dwg_ent_dim_ordinate*
  \param[out] dwg_point_3d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_dim_ordinate_get_leader_endpt (
    const dwg_ent_dim_ordinate *restrict dim, dwg_point_3d *restrict point,
    int *restrict error)
{
  if (dim
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = dim->leader_endpt.x;
      point->y = dim->leader_endpt.y;
      point->z = dim->leader_endpt.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/*******************************************************************
 *              FUNCTIONS FOR LINEAR DIMENSION ENTITY                *
 ********************************************************************/

/** Sets the 10 def point
  \code Usage: dwg_ent_dim_linear_set_def_pt(dim, &point, &error);
  \endcode
  \param[in] dim_linear dwg_ent_dim_linear*
  \param[out] dwg_point_3d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_dim_linear_set_def_pt (dwg_ent_dim_linear *restrict dim,
                               const dwg_point_3d *restrict point,
                               int *restrict error)
{
  if (dim
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      dim->def_pt.x = point->x;
      dim->def_pt.y = point->y;
      dim->def_pt.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the 10 def point
  \code Usage: dwg_ent_dim_linear_set_def_pt(dim, &point, &error);
  \endcode
  \param[in] dim_linear dwg_ent_dim_linear*
  \param[out] dwg_point_3d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_dim_linear_get_def_pt (const dwg_ent_dim_linear *restrict dim,
                               dwg_point_3d *restrict point,
                               int *restrict error)
{
  if (dim
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = dim->def_pt.x;
      point->y = dim->def_pt.y;
      point->z = dim->def_pt.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the xline1 point
  \code Usage: dwg_ent_dim_linear_set_13_pt(dim, &point, &error);
  \endcode
  \param[in] dim_linear dwg_ent_dim_linear*
  \param[out] dwg_point_3d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_dim_linear_set_13_pt (dwg_ent_dim_linear *restrict dim,
                              const dwg_point_3d *restrict point,
                              int *restrict error)
{
  if (dim
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      dim->xline1_pt.x = point->x;
      dim->xline1_pt.y = point->y;
      dim->xline1_pt.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the xline1 point
  \code Usage: dwg_ent_dim_linear_set_13_pt(dim, &point, &error);
  \endcode
  \param[in] dim_linear dwg_ent_dim_linear*
  \param[out] dwg_point_3d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_dim_linear_get_13_pt (const dwg_ent_dim_linear *restrict dim,
                              dwg_point_3d *restrict point,
                              int *restrict error)
{
  if (dim
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = dim->xline1_pt.x;
      point->y = dim->xline1_pt.y;
      point->z = dim->xline1_pt.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the xline2 point
  \code Usage: dwg_ent_dim_linear_set_14_pt(dim, &point, &error);
  \endcode
  \param[in] dim   dwg_ent_dim_linear*
  \param[out] dwg_point_3d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_dim_linear_set_14_pt (dwg_ent_dim_linear *restrict dim,
                              const dwg_point_3d *restrict point,
                              int *restrict error)
{
  if (dim
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      dim->xline2_pt.x = point->x;
      dim->xline2_pt.y = point->y;
      dim->xline2_pt.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the 14 point
  \code Usage: dwg_ent_dim_linear_get_14_pt(dim, &point, &error);
  \endcode
  \param[in] dim   dwg_ent_dim_linear*
  \param[out] dwg_point_3d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_dim_linear_get_14_pt (const dwg_ent_dim_linear *restrict dim,
                              dwg_point_3d *restrict point,
                              int *restrict error)
{
  if (dim
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = dim->xline2_pt.x;
      point->y = dim->xline2_pt.y;
      point->z = dim->xline2_pt.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the oblique angle
  \code Usage: double rot = dwg_ent_dim_linear_get_ext_line_rotation(dim,
&error); \endcode \param[in] dim   dwg_ent_dim_linear* \param[out] error int*,
is set to 0 for ok, 1 on error \deprecated
*/
EXPORT double
dwg_ent_dim_linear_get_ext_line_rotation (
    const dwg_ent_dim_linear *restrict dim, int *restrict error)
{
  if (dim)
    {
      *error = 0;
      return dim->oblique_angle;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets the oblique_angle.
  \code Usage: dwg_ent_dim_linear_set_ext_line_rotation(dim, rot, &error);
  \endcode
  \param[out] dim   dwg_ent_dim_linear*
  \param[in]  rotation double
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_dim_linear_set_ext_line_rotation (dwg_ent_dim_linear *restrict dim,
                                          const double angle,
                                          int *restrict error)
{
  if (dim)
    {
      *error = 0;
      dim->oblique_angle = angle;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the dim rotation
  \code Usage: double rot = dwg_ent_dim_linear_get_dim_rotation(dim, &error);
  \endcode
  \param[in]  dim   dwg_ent_dim_linear*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT double
dwg_ent_dim_linear_get_dim_rotation (const dwg_ent_dim_linear *restrict dim,
                                     int *restrict error)
{
  if (dim)
    {
      *error = 0;
      return dim->dim_rotation;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets the dim rotation
  \code Usage: dwg_ent_dim_linear_set_dim_rotation(dim, rot, &error);
  \endcode
  \param[out] dim   dwg_ent_dim_linear*
  \param[in]  rotation double
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_dim_linear_set_dim_rotation (dwg_ent_dim_linear *restrict dim,
                                     const double rotation,
                                     int *restrict error)
{
  if (dim)
    {
      *error = 0;
      dim->dim_rotation = rotation;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/*******************************************************************
 *             FUNCTIONS FOR ALIGNED DIMENSION ENTITY                *
 ********************************************************************/

/** Sets the 10 def point
  \code Usage: dwg_ent_dim_aligned_set_def_pt(dim, &point, &error);
  \endcode
  \param[out] dim   dwg_ent_dim_aligned*
  \param[in] dwg_point_3d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_dim_aligned_set_def_pt (dwg_ent_dim_aligned *restrict dim,
                                const dwg_point_3d *restrict point,
                                int *restrict error)
{
  if (dim
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      dim->def_pt.x = point->x;
      dim->def_pt.y = point->y;
      dim->def_pt.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the 10 def point
  \code Usage: dwg_ent_dim_aligned_get_def_pt(dim, &point, &error);
  \endcode
  \param[in] dim   dwg_ent_dim_aligned*
  \param[out] dwg_point_3d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_dim_aligned_get_def_pt (const dwg_ent_dim_aligned *restrict dim,
                                dwg_point_3d *restrict point,
                                int *restrict error)
{
  if (dim
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = dim->def_pt.x;
      point->y = dim->def_pt.y;
      point->z = dim->def_pt.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the xline1 point
  \code Usage: dwg_ent_dim_aligned_set_13_pt(dim, &point, &error);
  \endcode
  \param[out] dim   dwg_ent_dim_aligned*
  \param[in] dwg_point_3d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_dim_aligned_set_13_pt (dwg_ent_dim_aligned *restrict dim,
                               const dwg_point_3d *restrict point,
                               int *restrict error)
{
  if (dim
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      dim->xline1_pt.x = point->x;
      dim->xline1_pt.y = point->y;
      dim->xline1_pt.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the xline1 point
  \code Usage: dwg_ent_dim_aligned_get_13_pt(dim, &point, &error);
  \endcode
  \param[in]  dim   dwg_ent_dim_aligned*
  \param[out] dwg_point_3d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_dim_aligned_get_13_pt (const dwg_ent_dim_aligned *restrict dim,
                               dwg_point_3d *restrict point,
                               int *restrict error)
{
  if (dim
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = dim->xline1_pt.x;
      point->y = dim->xline1_pt.y;
      point->z = dim->xline1_pt.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the 14 point
  \code Usage: dwg_ent_dim_aligned_set_14_pt(dim, &point, &error);
  \endcode
  \param[out] dim   dwg_ent_dim_aligned*
  \param[in]  dwg_point_3d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_dim_aligned_set_14_pt (dwg_ent_dim_aligned *restrict dim,
                               const dwg_point_3d *restrict point,
                               int *restrict error)
{
  if (dim
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      dim->xline2_pt.x = point->x;
      dim->xline2_pt.y = point->y;
      dim->xline2_pt.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the 14 point
  \code Usage: dwg_ent_dim_aligned_get_14_pt(dim, &point, &error);
  \endcode
  \param[in] dim   dwg_ent_dim_aligned*
  \param[out] dwg_point_3d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_dim_aligned_get_14_pt (const dwg_ent_dim_aligned *restrict dim,
                               dwg_point_3d *restrict point,
                               int *restrict error)
{
  if (dim)
    {
      *error = 0;
      point->x = dim->xline2_pt.x;
      point->y = dim->xline2_pt.y;
      point->z = dim->xline2_pt.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the oblique_angle
  \code Usage: double rot = dwg_ent_dim_aligned_get_ext_line_rotation(dim,
&error); \endcode \param[in] dim   dwg_ent_dim_aligned* \param[out] error int*,
is set to 0 for ok, 1 on error \deprecated
*/
EXPORT double
dwg_ent_dim_aligned_get_ext_line_rotation (
    const dwg_ent_dim_aligned *restrict dim, int *restrict error)
{
  if (dim)
    {
      *error = 0;
      return dim->oblique_angle;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets the oblique_angle
  \code Usage: dwg_ent_dim_aligned_set_ext_line_rotation(dim, rot, &error);
  \endcode
  \param[out] dim   dwg_ent_dim_aligned*
  \param[in]  rotation double
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_dim_aligned_set_ext_line_rotation (dwg_ent_dim_aligned *restrict dim,
                                           const double rotation,
                                           int *restrict error)
{
  if (dim)
    {
      *error = 0;
      dim->oblique_angle = rotation;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/*******************************************************************
 *              FUNCTIONS FOR ANG3PT DIMENSION ENTITY                *
 ********************************************************************/

/** Sets the 10 point
  \code Usage: dwg_ent_dim_ang3pt_set_10_pt(dim, &point, &error);
  \endcode
  \param[out] ang    dwg_ent_dim_ang3pt*
  \param[in]  point  dwg_point_3d
  \param[out] error  int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_dim_ang3pt_set_def_pt (dwg_ent_dim_ang3pt *restrict ang,
                               const dwg_point_3d *restrict point,
                               int *restrict error)
{
  if (ang
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      ang->def_pt.x = point->x;
      ang->def_pt.y = point->y;
      ang->def_pt.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the 10 def pt
  \code Usage: dwg_ent_dim_ang3pt_get_def_pt(dim, &point, &error);
  \endcode
  \param[in]  ang     dwg_ent_dim_ang3pt*
  \param[out] point   dwg_point_3d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_dim_ang3pt_get_def_pt (const dwg_ent_dim_ang3pt *restrict ang,
                               dwg_point_3d *restrict point,
                               int *restrict error)
{
  if (ang)
    {
      *error = 0;
      point->x = ang->def_pt.x;
      point->y = ang->def_pt.y;
      point->z = ang->def_pt.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the xline1 point
  \code Usage: dwg_ent_dim_ang3pt_set_13_pt(dim, &point, &error);
  \endcode
  \param[out] ang     dwg_ent_dim_ang3pt*
  \param[in]  point   dwg_point_3d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_dim_ang3pt_set_13_pt (dwg_ent_dim_ang3pt *restrict ang,
                              const dwg_point_3d *restrict point,
                              int *restrict error)
{
  if (ang
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      ang->xline1_pt.x = point->x;
      ang->xline1_pt.y = point->y;
      ang->xline1_pt.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the xline1 pt
  \code Usage: dwg_ent_dim_ang3pt_get_13_pt(dim, &point, &error);
  \endcode
  \param[in]  ang     dwg_ent_dim_ang3pt*
  \param[out] point   dwg_point_3d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_dim_ang3pt_get_13_pt (const dwg_ent_dim_ang3pt *restrict ang,
                              dwg_point_3d *restrict point,
                              int *restrict error)
{
  if (ang
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = ang->xline1_pt.x;
      point->y = ang->xline1_pt.y;
      point->z = ang->xline1_pt.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the xline2 point
  \code Usage: dwg_ent_dim_ang3pt_set_14_pt(dim, &point, &error);
  \endcode
  \param[out] ang     dwg_ent_dim_ang3pt*
  \param[in]  point   dwg_point_3d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_dim_ang3pt_set_14_pt (dwg_ent_dim_ang3pt *restrict ang,
                              const dwg_point_3d *restrict point,
                              int *restrict error)
{
  if (ang
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      ang->xline2_pt.x = point->x;
      ang->xline2_pt.y = point->y;
      ang->xline2_pt.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the xline2 pt
  \code Usage: dwg_ent_dim_ang3pt_get_14_pt(dim, &point, &error);
  \endcode
  \param[in]  ang     dwg_ent_dim_ang3pt*
  \param[out] point   dwg_point_3d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_dim_ang3pt_get_14_pt (const dwg_ent_dim_ang3pt *restrict ang,
                              dwg_point_3d *restrict point,
                              int *restrict error)
{
  if (ang
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = ang->xline2_pt.x;
      point->y = ang->xline2_pt.y;
      point->z = ang->xline2_pt.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the center_pt 15
  \code Usage: dwg_ent_dim_ang3pt_set_first_arc_pt(dim, &point, &error);
  \endcode
  \param[out] ang dwg_ent_dim_ang3pt*
  \param[in]  dwg_point_3d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_dim_ang3pt_set_first_arc_pt (dwg_ent_dim_ang3pt *restrict ang,
                                     const dwg_point_3d *restrict point,
                                     int *restrict error)
{
  if (ang
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      ang->center_pt.x = point->x;
      ang->center_pt.y = point->y;
      ang->center_pt.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the center_pt 15
  \code Usage: dwg_ent_dim_ang3pt_get_first_arc_pt(dim, &point, &error);
  \endcode
  \param[in] ang dwg_ent_dim_ang3pt*
  \param[out] dwg_point_3d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_dim_ang3pt_get_first_arc_pt (const dwg_ent_dim_ang3pt *restrict ang,
                                     dwg_point_3d *restrict point,
                                     int *restrict error)
{
  if (ang
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = ang->center_pt.x;
      point->y = ang->center_pt.y;
      point->z = ang->center_pt.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/*******************************************************************
 *              FUNCTIONS FOR ANG2LN DIMENSION ENTITY                *
 ********************************************************************/

/** Sets dim ang2ln 10 def point. The z-coord is ignored, it is the eleavtion.
 */
EXPORT void
dwg_ent_dim_ang2ln_set_def_pt (dwg_ent_dim_ang2ln *restrict ang,
                               const dwg_point_3d *restrict point,
                               int *restrict error)
{
  if (ang
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      ang->def_pt.x = point->x;
      ang->def_pt.y = point->y;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns dim ang2ln 10 def point, The z-coord is the elevation.
 */
EXPORT void
dwg_ent_dim_ang2ln_get_def_pt (const dwg_ent_dim_ang2ln *restrict ang,
                               dwg_point_3d *restrict point,
                               int *restrict error)
{
  if (ang
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = ang->def_pt.x;
      point->y = ang->def_pt.y;
      point->z = ang->elevation;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets dim ang2ln 13 xline1start point
 */
EXPORT void
dwg_ent_dim_ang2ln_set_13_pt (dwg_ent_dim_ang2ln *restrict ang,
                              const dwg_point_3d *restrict point,
                              int *restrict error)
{
  if (ang
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      ang->xline1start_pt.x = point->x;
      ang->xline1start_pt.y = point->y;
      ang->xline1start_pt.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns dim ang2ln 13 xline1start point
 */
EXPORT void
dwg_ent_dim_ang2ln_get_13_pt (const dwg_ent_dim_ang2ln *restrict ang,
                              dwg_point_3d *restrict point,
                              int *restrict error)
{
  if (ang
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = ang->xline1start_pt.x;
      point->y = ang->xline1start_pt.y;
      point->z = ang->xline1start_pt.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets dim ang2ln 14 xline1end point
 */
EXPORT void
dwg_ent_dim_ang2ln_set_14_pt (dwg_ent_dim_ang2ln *restrict ang,
                              const dwg_point_3d *restrict point,
                              int *restrict error)
{
  if (ang
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      ang->xline1end_pt.x = point->x;
      ang->xline1end_pt.y = point->y;
      ang->xline1end_pt.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns dim ang2ln 14 xline1end point
 */
EXPORT void
dwg_ent_dim_ang2ln_get_14_pt (const dwg_ent_dim_ang2ln *restrict ang,
                              dwg_point_3d *restrict point,
                              int *restrict error)
{
  if (ang
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = ang->xline1end_pt.x;
      point->y = ang->xline1end_pt.y;
      point->z = ang->xline1end_pt.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets dim ang2ln xline2start 15 point
 */
EXPORT void
dwg_ent_dim_ang2ln_set_first_arc_pt (dwg_ent_dim_ang2ln *restrict ang,
                                     const dwg_point_3d *restrict point,
                                     int *restrict error)
{
  if (ang
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      ang->xline2start_pt.x = point->x;
      ang->xline2start_pt.y = point->y;
      ang->xline2start_pt.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns dim ang2ln xline2start 15 point
 */
EXPORT void
dwg_ent_dim_ang2ln_get_first_arc_pt (const dwg_ent_dim_ang2ln *restrict ang,
                                     dwg_point_3d *restrict point,
                                     int *restrict error)
{
  if (ang
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = ang->xline2start_pt.x;
      point->y = ang->xline2start_pt.y;
      point->z = ang->xline2start_pt.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets dim ang2ln xline2end 16 point
 */
EXPORT void
dwg_ent_dim_ang2ln_set_16_pt (dwg_ent_dim_ang2ln *restrict ang,
                              const dwg_point_3d *restrict point,
                              int *restrict error)
{
  if (ang
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      ang->xline2end_pt.x = point->x;
      ang->xline2end_pt.y = point->y;
      ang->xline2end_pt.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns dim ang2ln xline2end 16 point
 */
EXPORT void
dwg_ent_dim_ang2ln_get_16_pt (const dwg_ent_dim_ang2ln *restrict ang,
                              dwg_point_3d *restrict point,
                              int *restrict error)
{
  if (ang
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = ang->xline2end_pt.x;
      point->y = ang->xline2end_pt.y;
      point->z = ang->xline2end_pt.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/*******************************************************************
 *              FUNCTIONS FOR RADIUS DIMENSION ENTITY                *
 ********************************************************************/

/** Sets dim radius def 10 point
 */
EXPORT void
dwg_ent_dim_radius_set_def_pt (dwg_ent_dim_radius *restrict radius,
                               const dwg_point_3d *restrict point,
                               int *restrict error)
{
  if (radius
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      radius->def_pt.x = point->x;
      radius->def_pt.y = point->y;
      radius->def_pt.z = point->z;
    }
  else
    {
      LOG_ERROR ("%s: empty radius", __FUNCTION__)
      *error = 1;
    }
}

/** Returns dim radius def 10 point
 */
EXPORT void
dwg_ent_dim_radius_get_def_pt (const dwg_ent_dim_radius *restrict radius,
                               dwg_point_3d *restrict point,
                               int *restrict error)
{
  if (radius
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = radius->def_pt.x;
      point->y = radius->def_pt.y;
      point->z = radius->def_pt.z;
    }
  else
    {
      LOG_ERROR ("%s: empty radius", __FUNCTION__)
      *error = 1;
    }
}

/** Sets dim radius first_arc 15 point
 */
EXPORT void
dwg_ent_dim_radius_set_first_arc_pt (dwg_ent_dim_radius *restrict radius,
                                     const dwg_point_3d *restrict point,
                                     int *restrict error)
{
  if (radius
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      radius->first_arc_pt.x = point->x;
      radius->first_arc_pt.y = point->y;
      radius->first_arc_pt.z = point->z;
    }
  else
    {
      LOG_ERROR ("%s: empty radius", __FUNCTION__)
      *error = 1;
    }
}

/** Returns dim radius first_arc 15 point
 */
EXPORT void
dwg_ent_dim_radius_get_first_arc_pt (const dwg_ent_dim_radius *restrict radius,
                                     dwg_point_3d *restrict point,
                                     int *restrict error)
{
  if (radius
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = radius->first_arc_pt.x;
      point->y = radius->first_arc_pt.y;
      point->z = radius->first_arc_pt.z;
    }
  else
    {
      LOG_ERROR ("%s: empty radius", __FUNCTION__)
      *error = 1;
    }
}

/** Returns dim radius leader length
 */
EXPORT double
dwg_ent_dim_radius_get_leader_length (
    const dwg_ent_dim_radius *restrict radius, int *restrict error)
{
  if (radius)
    {
      *error = 0;
      return radius->leader_len;
    }
  else
    {
      LOG_ERROR ("%s: empty radius", __FUNCTION__)
      *error = 1;
      return bit_nan ();
    }
}

/** Sets dim radius leader length
 */
EXPORT void
dwg_ent_dim_radius_set_leader_length (dwg_ent_dim_radius *restrict radius,
                                      const double length, int *restrict error)
{
  if (radius)
    {
      *error = 0;
      radius->leader_len = length;
    }
  else
    {
      LOG_ERROR ("%s: empty radius", __FUNCTION__)
      *error = 1;
    }
}

/*******************************************************************
 *             FUNCTIONS FOR DIAMETER DIMENSION ENTITY               *
 ********************************************************************/

/** Sets dim diameter def 10 point
 */
EXPORT void
dwg_ent_dim_diameter_set_def_pt (dwg_ent_dim_diameter *restrict dia,
                                 const dwg_point_3d *restrict point,
                                 int *restrict error)
{
  if (dia
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      dia->def_pt.x = point->x;
      dia->def_pt.y = point->y;
      dia->def_pt.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns dim diameter def 10 point
 */
EXPORT void
dwg_ent_dim_diameter_get_def_pt (const dwg_ent_dim_diameter *restrict dia,
                                 dwg_point_3d *restrict point,
                                 int *restrict error)
{
  if (dia
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = dia->def_pt.x;
      point->y = dia->def_pt.y;
      point->z = dia->def_pt.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets dim diameter first_arc 15 point
 */
EXPORT void
dwg_ent_dim_diameter_set_first_arc_pt (dwg_ent_dim_diameter *restrict dia,
                                       const dwg_point_3d *restrict point,
                                       int *restrict error)
{
  if (dia
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      dia->first_arc_pt.x = point->x;
      dia->first_arc_pt.y = point->y;
      dia->first_arc_pt.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns dim diameter first_arc 15 point
 */
EXPORT void
dwg_ent_dim_diameter_get_first_arc_pt (
    const dwg_ent_dim_diameter *restrict dia, dwg_point_3d *restrict point,
    int *restrict error)
{
  if (dia
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = dia->first_arc_pt.x;
      point->y = dia->first_arc_pt.y;
      point->z = dia->first_arc_pt.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns dim diameter leader length
 */
EXPORT double
dwg_ent_dim_diameter_get_leader_length (
    const dwg_ent_dim_diameter *restrict dia, int *restrict error)
{
  if (dia)
    {
      *error = 0;
      return dia->leader_len;
    }
  else
    {
      LOG_ERROR ("%s: empty dia", __FUNCTION__)
      *error = 1;
      return bit_nan ();
    }
}

/** Sets dim diameter leader length
 */
EXPORT void
dwg_ent_dim_diameter_set_leader_length (dwg_ent_dim_diameter *restrict dia,
                                        const double length,
                                        int *restrict error)
{
  if (dia)
    {
      *error = 0;
      dia->leader_len = length;
    }
  else
    {
      LOG_ERROR ("%s: empty dia", __FUNCTION__)
      *error = 1;
    }
}

/*******************************************************************
 *                    FUNCTIONS FOR SHAPE ENTITY                     *
 ********************************************************************/

/** Returns shape ins point
 */
EXPORT void
dwg_ent_shape_get_ins_pt (const dwg_ent_shape *restrict shape,
                          dwg_point_3d *restrict point, int *restrict error)
{
  if (shape
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = shape->ins_pt.x;
      point->y = shape->ins_pt.y;
      point->z = shape->ins_pt.z;
    }
  else
    {
      LOG_ERROR ("%s: empty shape or point", __FUNCTION__)
      *error = 1;
    }
}

/** Sets shape ins point
 */
EXPORT void
dwg_ent_shape_set_ins_pt (dwg_ent_shape *restrict shape,
                          const dwg_point_3d *restrict point,
                          int *restrict error)
{
  if (shape
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      shape->ins_pt.x = point->x;
      shape->ins_pt.y = point->y;
      shape->ins_pt.z = point->z;
    }
  else
    {
      LOG_ERROR ("%s: empty shape or point", __FUNCTION__)
      *error = 1;
    }
}

/** Returns shape scale
 */
EXPORT double
dwg_ent_shape_get_scale (const dwg_ent_shape *restrict shape,
                         int *restrict error)
{
  if (shape)
    {
      *error = 0;
      return shape->scale;
    }
  else
    {
      LOG_ERROR ("%s: empty shape", __FUNCTION__)
      *error = 1;
      return bit_nan ();
    }
}

/** Sets shape scale
 */
EXPORT void
dwg_ent_shape_set_scale (dwg_ent_shape *restrict shape, const double scale,
                         int *restrict error)
{
  if (shape)
    {
      *error = 0;
      shape->scale = scale;
    }
  else
    {
      LOG_ERROR ("%s: empty shape", __FUNCTION__)
      *error = 1;
    }
}

/** Returns shape rotation
 */
EXPORT double
dwg_ent_shape_get_rotation (const dwg_ent_shape *restrict shape,
                            int *restrict error)
{
  if (shape)
    {
      *error = 0;
      return shape->rotation;
    }
  else
    {
      LOG_ERROR ("%s: empty shape", __FUNCTION__)
      *error = 1;
      return bit_nan ();
    }
}

/** Sets shape rotation
 */
EXPORT void
dwg_ent_shape_set_rotation (dwg_ent_shape *restrict shape,
                            const double rotation, int *restrict error)
{
  if (shape)
    {
      *error = 0;
      shape->rotation = rotation;
    }
  else
    {
      LOG_ERROR ("%s: empty shape", __FUNCTION__)
      *error = 1;
    }
}

/** Returns shape width factor
 */
EXPORT double
dwg_ent_shape_get_width_factor (const dwg_ent_shape *restrict shape,
                                int *restrict error)
{
  if (shape)
    {
      *error = 0;
      return shape->width_factor;
    }
  else
    {
      LOG_ERROR ("%s: empty shape", __FUNCTION__)
      *error = 1;
      return bit_nan ();
    }
}

/** Sets shape width factor
 */
EXPORT void
dwg_ent_shape_set_width_factor (dwg_ent_shape *restrict shape,
                                const double width_factor, int *restrict error)
{
  if (shape)
    {
      *error = 0;
      shape->width_factor = width_factor;
    }
  else
    {
      LOG_ERROR ("%s: empty shape", __FUNCTION__)
      *error = 1;
    }
}

/** Returns shape oblique
 */
EXPORT double
dwg_ent_shape_get_oblique (const dwg_ent_shape *restrict shape,
                           int *restrict error)
{
  if (shape)
    {
      *error = 0;
      return shape->oblique_angle;
    }
  else
    {
      LOG_ERROR ("%s: empty shape", __FUNCTION__)
      *error = 1;
      return bit_nan ();
    }
}

/** Sets shape oblique
 */
EXPORT void
dwg_ent_shape_set_oblique (dwg_ent_shape *restrict shape, const double oblique,
                           int *restrict error)
{
  if (shape)
    {
      *error = 0;
      shape->oblique_angle = oblique;
    }
  else
    {
      LOG_ERROR ("%s: empty shape", __FUNCTION__)
      *error = 1;
    }
}

/** Returns shape thickness
 */
EXPORT double
dwg_ent_shape_get_thickness (const dwg_ent_shape *restrict shape,
                             int *restrict error)
{
  if (shape)
    {
      *error = 0;
      return shape->thickness;
    }
  else
    {
      LOG_ERROR ("%s: empty shape", __FUNCTION__)
      *error = 1;
      return bit_nan ();
    }
}

/** Sets shape thickness
 */
EXPORT void
dwg_ent_shape_set_thickness (dwg_ent_shape *restrict shape,
                             const double thickness, int *restrict error)
{
  if (shape)
    {
      *error = 0;
      shape->thickness = thickness;
    }
  else
    {
      LOG_ERROR ("%s: empty shape", __FUNCTION__)
      *error = 1;
    }
}

/** Returns SHAPE.style_id
 */
EXPORT BITCODE_BS
dwg_ent_shape_get_shape_no (const dwg_ent_shape *restrict shape,
                            int *restrict error)
{
  if (shape)
    {
      *error = 0;
      return shape->style_id;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets SHAPE.style_id by index, not name.
 */
EXPORT void
dwg_ent_shape_set_shape_no (dwg_ent_shape *restrict shape, const BITCODE_BS id,
                            int *restrict error)
{
  if (shape)
    {
      *error = 0;
      // FIXME: check valid index: STYLE_CONTROL.num_entries
      shape->style_id = id;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns SHAPE.style_id
 */
EXPORT BITCODE_BS
dwg_ent_shape_get_style_id (const dwg_ent_shape *restrict shape,
                            int *restrict error)
{
  if (shape)
    {
      *error = 0;
      return shape->style_id;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets SHAPE.style_id by index, not name.
 */
EXPORT void
dwg_ent_shape_set_style_id (dwg_ent_shape *restrict shape, const BITCODE_BS id,
                            int *restrict error)
{
  if (shape)
    {
      *error = 0;
      // FIXME: check valid index: STYLE_CONTROL.num_entries
      shape->style_id = id;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns shape extrusion
 */
EXPORT void
dwg_ent_shape_get_extrusion (const dwg_ent_shape *restrict shape,
                             dwg_point_3d *restrict point, int *restrict error)
{
  if (shape
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = shape->extrusion.x;
      point->y = shape->extrusion.y;
      point->z = shape->extrusion.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets shape extrusion
 */
EXPORT void
dwg_ent_shape_set_extrusion (dwg_ent_shape *restrict shape,
                             const dwg_point_3d *restrict point,
                             int *restrict error)
{
  if (shape
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      shape->extrusion.x = point->x;
      shape->extrusion.y = point->y;
      shape->extrusion.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/*******************************************************************
 *                    FUNCTIONS FOR MTEXT ENTITY                     *
 ********************************************************************/

/** Sets the _dwg_entity_MTEXT::ins_pt, DXF 10
 */
EXPORT void
dwg_ent_mtext_set_insertion_pt (dwg_ent_mtext *restrict mtext,
                                const dwg_point_3d *restrict point,
                                int *restrict error)
{
  if (mtext
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      mtext->ins_pt.x = point->x;
      mtext->ins_pt.y = point->y;
      mtext->ins_pt.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_MTEXT::ins_pt, DXF 10
 */
EXPORT void
dwg_ent_mtext_get_insertion_pt (const dwg_ent_mtext *restrict mtext,
                                dwg_point_3d *restrict point,
                                int *restrict error)
{
  if (mtext
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = mtext->ins_pt.x;
      point->y = mtext->ins_pt.y;
      point->z = mtext->ins_pt.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the _dwg_entity_MTEXT::extrusion vector, DXF 210
 */
EXPORT void
dwg_ent_mtext_set_extrusion (dwg_ent_mtext *restrict mtext,
                             const dwg_point_3d *restrict vector,
                             int *restrict error)
{
  if (mtext
#    ifndef HAVE_NONNULL
      && vector
#    endif
  )
    {
      *error = 0;
      mtext->extrusion.x = vector->x;
      mtext->extrusion.y = vector->y;
      mtext->extrusion.z = vector->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_MTEXT::extrusion vector, DXF 210
 */
EXPORT void
dwg_ent_mtext_get_extrusion (const dwg_ent_mtext *restrict mtext,
                             dwg_point_3d *restrict point, int *restrict error)
{
  if (mtext
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = mtext->extrusion.x;
      point->y = mtext->extrusion.y;
      point->z = mtext->extrusion.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the _dwg_entity_MTEXT::x_axis_dir vector, DXF 11 (in WCS)
 */
EXPORT void
dwg_ent_mtext_set_x_axis_dir (dwg_ent_mtext *restrict mtext,
                              const dwg_point_3d *restrict vector,
                              int *restrict error)
{
  if (mtext
#    ifndef HAVE_NONNULL
      && vector
#    endif
  )
    {
      *error = 0;
      mtext->x_axis_dir.x = vector->x;
      mtext->x_axis_dir.y = vector->y;
      mtext->x_axis_dir.z = vector->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_MTEXT::x_axis_dir vector, DXF 11 (in WCS)
 */
EXPORT void
dwg_ent_mtext_get_x_axis_dir (const dwg_ent_mtext *restrict mtext,
                              dwg_point_3d *restrict vector,
                              int *restrict error)
{
  if (mtext
#    ifndef HAVE_NONNULL
      && vector
#    endif
  )
    {
      *error = 0;
      vector->x = mtext->x_axis_dir.x;
      vector->y = mtext->x_axis_dir.y;
      vector->z = mtext->x_axis_dir.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the _dwg_entity_MTEXT::rect_height, no DXF
 */
EXPORT void
dwg_ent_mtext_set_rect_height (dwg_ent_mtext *restrict mtext,
                               const double rect_height, int *restrict error)
{
  if (mtext)
    {
      *error = 0;
      mtext->rect_height = rect_height;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_MTEXT::rect_height, no DXF
 */
EXPORT double
dwg_ent_mtext_get_rect_height (const dwg_ent_mtext *restrict mtext,
                               int *restrict error)
{
  if (mtext)
    {
      *error = 0;
      return mtext->rect_height;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets the _dwg_entity_MTEXT::rect_width, DXF 41.
 */
EXPORT void
dwg_ent_mtext_set_rect_width (dwg_ent_mtext *restrict mtext,
                              const double rect_width, int *restrict error)
{
  if (mtext)
    {
      *error = 0;
      mtext->rect_width = rect_width;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_MTEXT::rect_width, DXF 41.
 */
EXPORT double
dwg_ent_mtext_get_rect_width (const dwg_ent_mtext *restrict mtext,
                              int *restrict error)
{
  if (mtext)
    {
      *error = 0;
      return mtext->rect_width;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets the _dwg_entity_MTEXT::text_height, DXF 40.
 */
EXPORT void
dwg_ent_mtext_set_text_height (dwg_ent_mtext *restrict mtext,
                               const double text_height, int *restrict error)
{
  if (mtext)
    {
      *error = 0;
      mtext->text_height = text_height;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_MTEXT::text_height, DXF 40.
 */
EXPORT double
dwg_ent_mtext_get_text_height (const dwg_ent_mtext *restrict mtext,
                               int *restrict error)
{
  if (mtext)
    {
      *error = 0;
      return mtext->text_height;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Returns the _dwg_entity_MTEXT::attachment flag, DXF 71.
 */
EXPORT BITCODE_BS
dwg_ent_mtext_get_attachment (const dwg_ent_mtext *restrict mtext,
                              int *restrict error)
{
  if (mtext)
    {
      *error = 0;
      return mtext->attachment;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets the _dwg_entity_MTEXT::attachment flag, DXF 71.
 */
EXPORT void
dwg_ent_mtext_set_attachment (dwg_ent_mtext *restrict mtext,
                              const BITCODE_BS attachment, int *restrict error)
{
  if (mtext && attachment < 10)
    {
      *error = 0;
      mtext->attachment = attachment;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty or wrong arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_MTEXT::flow_dir flag, DXF 72.
 */
EXPORT BITCODE_BS
dwg_ent_mtext_get_drawing_dir (const dwg_ent_mtext *restrict mtext,
                               int *restrict error)
{
  if (mtext)
    {
      *error = 0;
      return mtext->flow_dir;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets the _dwg_entity_MTEXT::flow_dir flag, DXF 72.
 */
EXPORT void
dwg_ent_mtext_set_drawing_dir (dwg_ent_mtext *restrict mtext,
                               const BITCODE_BS dir, int *restrict error)
{
  if (mtext && dir < 6)
    {
      *error = 0;
      mtext->flow_dir = dir;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty or wrong arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_MTEXT::extents_height, DXF 42.
 */
EXPORT double
dwg_ent_mtext_get_extents_height (const dwg_ent_mtext *restrict mtext,
                                  int *restrict error)
{
  if (mtext)
    {
      *error = 0;
      return mtext->extents_height;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets the _dwg_entity_MTEXT::extents_height, DXF 42.
 */
EXPORT void
dwg_ent_mtext_set_extents_height (dwg_ent_mtext *restrict mtext,
                                  const double height, int *restrict error)
{
  if (mtext)
    {
      *error = 0;
      mtext->extents_height = height;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_MTEXT::extents_width, DXF 43.
 */
EXPORT double
dwg_ent_mtext_get_extents_width (const dwg_ent_mtext *restrict mtext,
                                 int *restrict error)
{
  if (mtext)
    {
      *error = 0;
      return mtext->extents_width;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets the _dwg_entity_MTEXT::extents_width, DXF 43.
 */
EXPORT void
dwg_ent_mtext_set_extents_width (dwg_ent_mtext *restrict mtext,
                                 const double wid, int *restrict error)
{
  if (mtext)
    {
      *error = 0;
      mtext->extents_width = wid;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns mtext text value (utf-8 encoded)
 */
EXPORT char *
dwg_ent_mtext_get_text (const dwg_ent_mtext *restrict ent, int *restrict error)
{
  if (ent)
    {
      *error = 0;
      if (dwg_version >= R_2007)
        return bit_convert_TU ((BITCODE_TU)ent->text); // a copy
      else
        return ent->text; // the ref
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return NULL;
    }
}

/** Sets mtext text value (utf-8 encoded)
 */
EXPORT void
dwg_ent_mtext_set_text (dwg_ent_mtext *restrict ent, char *text,
                        int *restrict error)
{
  Dwg_Data *dwg = dwg_obj_generic_dwg (ent, error);
  if (ent && !error)
    {
      ent->text = dwg_add_u8_input (dwg, text);
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns mtext linespace style
 */
EXPORT BITCODE_BS
dwg_ent_mtext_get_linespace_style (const dwg_ent_mtext *restrict mtext,
                                   int *restrict error)
{
  if (mtext)
    {
      *error = 0;
      return mtext->linespace_style;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets mtext linespace style
 */
EXPORT void
dwg_ent_mtext_set_linespace_style (dwg_ent_mtext *restrict mtext,
                                   BITCODE_BS style, int *restrict error)
{
  if (mtext)
    {
      *error = 0;
      mtext->linespace_style = style;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns mtext linespace factor
 */
EXPORT double
dwg_ent_mtext_get_linespace_factor (const dwg_ent_mtext *restrict mtext,
                                    int *restrict error)
{
  if (mtext)
    {
      *error = 0;
      return mtext->linespace_factor;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets mtext linespace factor
 */
EXPORT void
dwg_ent_mtext_set_linespace_factor (dwg_ent_mtext *restrict mtext,
                                    const double factor, int *restrict error)
{
  if (mtext)
    {
      *error = 0;
      mtext->linespace_factor = factor;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/*******************************************************************
 *                   FUNCTIONS FOR LEADER ENTITY                     *
 ********************************************************************/

/** Sets leader annot type
 */
EXPORT void
dwg_ent_leader_set_annot_type (dwg_ent_leader *restrict leader,
                               const BITCODE_BS type, int *restrict error)
{
  if (leader)
    {
      *error = 0;
      leader->annot_type = type;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns leader annot type
 */
EXPORT BITCODE_BS
dwg_ent_leader_get_annot_type (const dwg_ent_leader *restrict leader,
                               int *restrict error)
{
  if (leader)
    {
      *error = 0;
      return leader->annot_type;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets leader path type
 */
EXPORT void
dwg_ent_leader_set_path_type (dwg_ent_leader *restrict leader,
                              const BITCODE_BS type, int *restrict error)
{
  if (leader)
    {
      *error = 0;
      leader->path_type = type;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns leader path type
 */
EXPORT BITCODE_BS
dwg_ent_leader_get_path_type (const dwg_ent_leader *restrict leader,
                              int *restrict error)
{
  if (leader)
    {
      *error = 0;
      return leader->path_type;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Returns leader num_points
 */
EXPORT BITCODE_BL
dwg_ent_leader_get_num_points (const dwg_ent_leader *restrict leader,
                               int *restrict error)
{
  if (leader)
    {
      *error = 0;
      return leader->num_points;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}

// TODO: dwg_ent_leader_add_point, dwg_ent_leader_delete_pts

/** Sets leader end point proj
 */
EXPORT void
dwg_ent_leader_set_origin (dwg_ent_leader *restrict leader,
                           const dwg_point_3d *restrict point,
                           int *restrict error)
{
  if (leader
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      leader->origin.x = point->x;
      leader->origin.y = point->y;
      leader->origin.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns leader origin
 */
EXPORT void
dwg_ent_leader_get_origin (const dwg_ent_leader *restrict leader,
                           dwg_point_3d *restrict point, int *restrict error)
{
  if (leader
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = leader->origin.x;
      point->y = leader->origin.y;
      point->z = leader->origin.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets leader extrusion value
 */
EXPORT void
dwg_ent_leader_set_extrusion (dwg_ent_leader *restrict leader,
                              const dwg_point_3d *restrict point,
                              int *restrict error)
{
  if (leader
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      leader->extrusion.x = point->x;
      leader->extrusion.y = point->y;
      leader->extrusion.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns leader extrusion value
 */
EXPORT void
dwg_ent_leader_get_extrusion (const dwg_ent_leader *restrict leader,
                              dwg_point_3d *restrict point,
                              int *restrict error)
{
  if (leader
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = leader->extrusion.x;
      point->y = leader->extrusion.y;
      point->z = leader->extrusion.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets leader x direction value
 */
EXPORT void
dwg_ent_leader_set_x_direction (dwg_ent_leader *restrict leader,
                                const dwg_point_3d *restrict point,
                                int *restrict error)
{
  if (leader
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      leader->x_direction.x = point->x;
      leader->x_direction.y = point->y;
      leader->x_direction.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns leader x direction value
 */
EXPORT void
dwg_ent_leader_get_x_direction (const dwg_ent_leader *restrict leader,
                                dwg_point_3d *restrict point,
                                int *restrict error)
{
  if (leader
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = leader->x_direction.x;
      point->y = leader->x_direction.y;
      point->z = leader->x_direction.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets leader offset to block insert point
 */
EXPORT void
dwg_ent_leader_set_inspt_offset (dwg_ent_leader *restrict leader,
                                 const dwg_point_3d *restrict point,
                                 int *restrict error)
{
  if (leader
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      leader->inspt_offset.x = point->x;
      leader->inspt_offset.y = point->y;
      leader->inspt_offset.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns leader offset to block ins point
 */
EXPORT void
dwg_ent_leader_get_inspt_offset (const dwg_ent_leader *restrict leader,
                                 dwg_point_3d *restrict point,
                                 int *restrict error)
{
  if (leader
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = leader->inspt_offset.x;
      point->y = leader->inspt_offset.y;
      point->z = leader->inspt_offset.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets leader dimgap
 */
EXPORT void
dwg_ent_leader_set_dimgap (dwg_ent_leader *restrict leader,
                           const double dimgap, int *restrict error)
{
  if (leader)
    {
      *error = 0;
      leader->dimgap = dimgap;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns leader dimgap
 */
EXPORT double
dwg_ent_leader_get_dimgap (const dwg_ent_leader *restrict leader,
                           int *restrict error)
{
  if (leader)
    {
      *error = 0;
      return leader->dimgap;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets leader box height
 */
EXPORT void
dwg_ent_leader_set_box_height (dwg_ent_leader *restrict leader,
                               const double height, int *restrict error)
{
  if (leader)
    {
      *error = 0;
      leader->box_height = height;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns leader box height
 */
EXPORT double
dwg_ent_leader_get_box_height (const dwg_ent_leader *restrict leader,
                               int *restrict error)
{
  if (leader)
    {
      *error = 0;
      return leader->box_height;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets leader box width
 */
EXPORT void
dwg_ent_leader_set_box_width (dwg_ent_leader *restrict leader,
                              const double width, int *restrict error)
{
  if (leader)
    {
      *error = 0;
      leader->box_width = width;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns leader box width
 */
EXPORT double
dwg_ent_leader_get_box_width (const dwg_ent_leader *restrict leader,
                              int *restrict error)
{
  if (leader)
    {
      *error = 0;
      return leader->box_width;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets leader hookline_dir value
 */
EXPORT void
dwg_ent_leader_set_hookline_dir (dwg_ent_leader *restrict leader, char dir,
                                 int *restrict error)
{
  if (leader)
    {
      *error = 0;
      leader->hookline_dir = dir;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns leader hook line on x dir value
 */
EXPORT char
dwg_ent_leader_get_hookline_dir (const dwg_ent_leader *restrict leader,
                                 int *restrict error)
{
  if (leader)
    {
      *error = 0;
      return leader->hookline_dir;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/** Sets leader arrowhead on
 */
EXPORT void
dwg_ent_leader_set_arrowhead_on (dwg_ent_leader *restrict leader,
                                 const char arrow, int *restrict error)
{
  if (leader)
    {
      *error = 0;
      leader->arrowhead_on = arrow;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns leader arrow head on
 */
EXPORT char
dwg_ent_leader_get_arrowhead_on (const dwg_ent_leader *restrict leader,
                                 int *restrict error)
{
  if (leader)
    {
      *error = 0;
      return leader->arrowhead_on;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/** Sets leader arrow head type
 */
EXPORT void
dwg_ent_leader_set_arrowhead_type (dwg_ent_leader *restrict leader,
                                   const BITCODE_BS type, int *restrict error)
{
  if (leader)
    {
      leader->arrowhead_type = type;
      *error = 0;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns leader arrowhead type
 */
EXPORT BITCODE_BS
dwg_ent_leader_get_arrowhead_type (const dwg_ent_leader *restrict leader,
                                   int *restrict error)
{
  if (leader)
    {
      *error = 0;
      return leader->arrowhead_type;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets leader dimasz
 */
EXPORT void
dwg_ent_leader_set_dimasz (dwg_ent_leader *restrict leader,
                           const double dimasz, int *restrict error)
{
  if (leader)
    {
      *error = 0;
      leader->dimasz = dimasz;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns leader dimasz
 */
EXPORT double
dwg_ent_leader_get_dimasz (const dwg_ent_leader *restrict leader,
                           int *restrict error)
{
  if (leader)
    {
      *error = 0;
      return leader->dimasz;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets leader byblock color
 */
EXPORT void
dwg_ent_leader_set_byblock_color (dwg_ent_leader *restrict leader,
                                  const BITCODE_BS color, int *restrict error)
{
  if (leader)
    {
      *error = 0;
      leader->byblock_color = color;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns leader byblock color
 */
EXPORT BITCODE_BS
dwg_ent_leader_get_byblock_color (const dwg_ent_leader *restrict leader,
                                  int *restrict error)
{
  if (leader)
    {
      *error = 0;
      return leader->byblock_color;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/*******************************************************************
 *                  FUNCTIONS FOR TOLERANCE ENTITY                   *
 ********************************************************************/

/** Sets tolerance height
 */
EXPORT void
dwg_ent_tolerance_set_height (dwg_ent_tolerance *restrict tol,
                              const double height, int *restrict error)
{
  if (tol)
    {
      *error = 0;
      tol->height = height;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns tolerance height
 */
EXPORT double
dwg_ent_tolerance_get_height (const dwg_ent_tolerance *restrict tol,
                              int *restrict error)
{
  if (tol)
    {
      *error = 0;
      return tol->height;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets tolerance dimgap
 */
EXPORT void
dwg_ent_tolerance_set_dimgap (dwg_ent_tolerance *restrict tol,
                              const double dimgap, int *restrict error)
{
  if (tol)
    {
      *error = 0;
      tol->dimgap = dimgap;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns tolerance dimgap
 */
EXPORT double
dwg_ent_tolerance_get_dimgap (const dwg_ent_tolerance *restrict tol,
                              int *restrict error)
{
  if (tol)
    {
      *error = 0;
      return tol->dimgap;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets tolerance insertion point
 */
EXPORT void
dwg_ent_tolerance_set_ins_pt (dwg_ent_tolerance *restrict tol,
                              const dwg_point_3d *restrict point,
                              int *restrict error)
{
  if (tol
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      tol->ins_pt.x = point->x;
      tol->ins_pt.y = point->y;
      tol->ins_pt.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns tolerance insertion point
 */
EXPORT void
dwg_ent_tolerance_get_ins_pt (const dwg_ent_tolerance *restrict tol,
                              dwg_point_3d *restrict point,
                              int *restrict error)
{
  if (tol
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = tol->ins_pt.x;
      point->y = tol->ins_pt.y;
      point->z = tol->ins_pt.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets tolerance x direction
 */
EXPORT void
dwg_ent_tolerance_set_x_direction (dwg_ent_tolerance *restrict tol,
                                   const dwg_point_3d *restrict point,
                                   int *restrict error)
{
  if (tol
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      tol->x_direction.x = point->x;
      tol->x_direction.y = point->y;
      tol->x_direction.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns tolerance x direction
 */
EXPORT void
dwg_ent_tolerance_get_x_direction (const dwg_ent_tolerance *restrict tol,
                                   dwg_point_3d *restrict point,
                                   int *restrict error)
{
  if (tol
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = tol->x_direction.x;
      point->y = tol->x_direction.y;
      point->z = tol->x_direction.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets tolerance extrusion
 */
EXPORT void
dwg_ent_tolerance_set_extrusion (dwg_ent_tolerance *restrict tol,
                                 const dwg_point_3d *restrict point,
                                 int *restrict error)
{
  if (tol
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      tol->extrusion.x = point->x;
      tol->extrusion.y = point->y;
      tol->extrusion.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns tolerance extrusion
 */
EXPORT void
dwg_ent_tolerance_get_extrusion (const dwg_ent_tolerance *restrict tol,
                                 dwg_point_3d *restrict point,
                                 int *restrict error)
{
  if (tol
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = tol->extrusion.x;
      point->y = tol->extrusion.y;
      point->z = tol->extrusion.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets tolerance text string (utf-8 encoded)
 */
EXPORT void
dwg_ent_tolerance_set_text_string (dwg_ent_tolerance *restrict tol,
                                   const char *restrict string,
                                   int *restrict error)
{
  Dwg_Data *dwg = dwg_obj_generic_dwg (ent, error);
  if (ent && !error)
    {
      ent->text_value = dwg_add_u8_input (dwg, string);
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns tolerance text string (utf-8 encoded)
 */
EXPORT char *
dwg_ent_tolerance_get_text_string (const dwg_ent_tolerance *restrict tol,
                                   int *restrict error)
{
  if (tol)
    {
      *error = 0;
      if (dwg_version >= R_2007)
        return bit_convert_TU ((BITCODE_TU)tol->text_value);
      else
        return tol->text_value;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return NULL;
    }
}

/*******************************************************************
 *                   FUNCTIONS FOR LWPOLYLINE ENTITY                    *
 ********************************************************************/
/** Returns lwpline flags
 */
EXPORT BITCODE_BS
dwg_ent_lwpline_get_flag (const dwg_ent_lwpline *restrict lwpline,
                          int *restrict error)
{
  if (lwpline)
    {
      *error = 0;
      return lwpline->flag;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/** Sets lwpline flags
 */
EXPORT void
dwg_ent_lwpline_set_flag (dwg_ent_lwpline *restrict lwpline, char flags,
                          int *restrict error)
{
  if (lwpline)
    {
      *error = 0;
      lwpline->flag = flags;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns lwpline const width
 */
EXPORT double
dwg_ent_lwpline_get_const_width (const dwg_ent_lwpline *restrict lwpline,
                                 int *restrict error)
{
  if (lwpline)
    {
      *error = 0;
      return lwpline->const_width;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets lwpline const width
 */
EXPORT void
dwg_ent_lwpline_set_const_width (dwg_ent_lwpline *restrict lwpline,
                                 const double const_width, int *restrict error)
{
  if (lwpline)
    {
      *error = 0;
      lwpline->const_width = const_width;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}
/** Returns lwpline elevation
 */
EXPORT double
dwg_ent_lwpline_get_elevation (const dwg_ent_lwpline *restrict lwpline,
                               int *restrict error)
{
  if (lwpline)
    {
      *error = 0;
      return lwpline->elevation;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets lwpline elevation
 */
EXPORT void
dwg_ent_lwpline_set_elevation (dwg_ent_lwpline *restrict lwpline,
                               const double elevation, int *restrict error)
{
  if (lwpline)
    {
      *error = 0;
      lwpline->elevation = elevation;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}
/** Returns lwpline thickness
 */
EXPORT double
dwg_ent_lwpline_get_thickness (const dwg_ent_lwpline *restrict lwpline,
                               int *restrict error)
{
  if (lwpline)
    {
      *error = 0;
      return lwpline->thickness;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets lwpline thickness
 */
EXPORT void
dwg_ent_lwpline_set_thickness (dwg_ent_lwpline *restrict lwpline,
                               const double thickness, int *restrict error)
{
  if (lwpline)
    {
      *error = 0;
      lwpline->thickness = thickness;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns lwpline bulges count
 */
EXPORT BITCODE_BL
dwg_ent_lwpline_get_numbulges (const dwg_ent_lwpline *restrict lwpline,
                               int *restrict error)
{
  if (lwpline)
    {
      *error = 0;
      return lwpline->num_bulges;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}

/** Returns lwpline width count
 */
EXPORT BITCODE_BL
dwg_ent_lwpline_get_numwidths (const dwg_ent_lwpline *restrict lwpline,
                               int *restrict error)
{
  if (lwpline)
    {
      *error = 0;
      return lwpline->num_widths;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}

/** Returns lwpline normal
 */
EXPORT void
dwg_ent_lwpline_get_extrusion (const dwg_ent_lwpline *restrict lwpline,
                               dwg_point_3d *restrict point,
                               int *restrict error)
{
  if (lwpline
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = lwpline->extrusion.x;
      point->y = lwpline->extrusion.y;
      point->z = lwpline->extrusion.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets lwpline normal
 */
EXPORT void
dwg_ent_lwpline_set_extrusion (dwg_ent_lwpline *restrict lwpline,
                               const dwg_point_3d *restrict point,
                               int *restrict error)
{
  if (lwpline
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      lwpline->extrusion.x = point->x;
      lwpline->extrusion.y = point->y;
      lwpline->extrusion.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/*******************************************************************
 *                  FUNCTIONS FOR OLE2FRAME ENTITY                   *
 ********************************************************************/

/** Returns ole2frame type: 1: Link, 2: Embedded, 3: Static
 */
EXPORT BITCODE_BS
dwg_ent_ole2frame_get_type (const dwg_ent_ole2frame *restrict frame,
                            int *restrict error)
{
  if (frame)
    {
      *error = 0;
      return frame->type;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return (BITCODE_BS)-1;
    }
}

/** Sets ole2frame type. 1, 2 or 3
 */
EXPORT void
dwg_ent_ole2frame_set_type (dwg_ent_ole2frame *restrict frame,
                            const BITCODE_BS type, int *restrict error)
{
  if (frame && type >= 1 && type <= 3)
    {
      *error = 0;
      frame->type = type;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty or wrong arg", __FUNCTION__)
    }
}

/** Returns ole2frame mode, 0 or 1
 */
EXPORT BITCODE_BS
dwg_ent_ole2frame_get_mode (const dwg_ent_ole2frame *restrict frame,
                            int *restrict error)
{
  if (frame)
    {
      *error = 0;
      return frame->mode;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return (BITCODE_BS)-1;
    }
}

/// set ole2frame mode
EXPORT void
dwg_ent_ole2frame_set_mode (dwg_ent_ole2frame *restrict frame,
                            const BITCODE_BS mode, int *restrict error)
{
  // assert: mode is unsigned
  if (frame && mode <= 1)
    {
      *error = 0;
      frame->mode = mode;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty or wrong arg", __FUNCTION__)
    }
}

/** Returns ole2frame data length
 */
EXPORT BITCODE_BL
dwg_ent_ole2frame_get_data_size (const dwg_ent_ole2frame *restrict frame,
                                 int *restrict error)
{
  if (frame)
    {
      *error = 0;
      return frame->data_size;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}

/** Returns ole2frame data (binary)
 */
EXPORT BITCODE_TF
dwg_ent_ole2frame_get_data (const dwg_ent_ole2frame *restrict frame,
                            int *restrict error)
{
  if (frame)
    {
      *error = 0;
      return frame->data;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return NULL;
    }
}

/** Sets ole2frame data
 */
EXPORT void
dwg_ent_ole2frame_set_data (dwg_ent_ole2frame *restrict frame,
                            const BITCODE_TF restrict data,
                            const BITCODE_BL data_size, int *restrict error)
{
  if (frame)
    {
      *error = 0;
      frame->data = data;
      frame->data_size = data_size;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/********************************************************************
 *                  FUNCTIONS FOR PROXY OBJECT                       *
 ********************************************************************/

EXPORT BITCODE_BL
dwg_obj_proxy_get_class_id (const dwg_obj_proxy *restrict proxy,
                            int *restrict error)
{
  if (proxy)
    {
      *error = 0;
      return proxy->class_id;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}

EXPORT void
dwg_obj_proxy_set_class_id (dwg_obj_proxy *restrict proxy,
                            const BITCODE_BL class_id, int *restrict error)
{
  if (proxy)
    {
      *error = 0;
      proxy->class_id = class_id;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

EXPORT BITCODE_BL
dwg_obj_proxy_get_version (const dwg_obj_proxy *restrict proxy,
                           int *restrict error)
{
  if (proxy)
    {
      *error = 0;
      return proxy->version;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}

EXPORT void
dwg_obj_proxy_set_version (dwg_obj_proxy *restrict proxy,
                           const BITCODE_BL version, int *restrict error)
{
  if (proxy)
    {
      *error = 0;
      proxy->version = version;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

EXPORT BITCODE_B
dwg_obj_proxy_get_from_dxf (const dwg_obj_proxy *restrict proxy,
                            int *restrict error)
{
  if (proxy)
    {
      *error = 0;
      return proxy->from_dxf;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}

EXPORT void
dwg_obj_proxy_set_from_dxf (dwg_obj_proxy *restrict proxy,
                            const BITCODE_B from_dxf, int *restrict error)
{
  if (proxy)
    {
      *error = 0;
      proxy->from_dxf = from_dxf;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

EXPORT BITCODE_RC *
dwg_obj_proxy_get_data (const dwg_obj_proxy *restrict proxy,
                        int *restrict error)
{
  if (proxy)
    {
      *error = 0;
      return proxy->data;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return NULL;
    }
}

EXPORT void
dwg_obj_proxy_set_data (dwg_obj_proxy *restrict proxy,
                        const BITCODE_RC *restrict data, int *restrict error)
{
  if (proxy)
    {
      *error = 0;
      proxy->data = (BITCODE_RC *)data;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

EXPORT dwg_object_ref **
dwg_obj_proxy_get_objids (const dwg_obj_proxy *restrict proxy,
                          int *restrict error)
{
  if (proxy)
    {
      *error = 0;
      return proxy->objids;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return NULL;
    }
}

/********************************************************************
 *                  FUNCTIONS FOR XRECORD OBJECT                       *
 ********************************************************************/

EXPORT BITCODE_BL
dwg_obj_xrecord_get_xdata_size (const dwg_obj_xrecord *restrict xrecord,
                                int *restrict error)
{
  if (xrecord)
    {
      *error = 0;
      return xrecord->xdata_size;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}

EXPORT BITCODE_BS
dwg_obj_xrecord_get_cloning_flags (const dwg_obj_xrecord *restrict xrecord,
                                   int *restrict error)
{
  if (xrecord)
    {
      *error = 0;
      return xrecord->cloning;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}

EXPORT void
dwg_obj_xrecord_set_cloning_flags (dwg_obj_xrecord *restrict xrecord,
                                   const BITCODE_BS cloning_flags,
                                   int *restrict error)
{
  if (xrecord)
    {
      *error = 0;
      xrecord->cloning = cloning_flags;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

EXPORT BITCODE_BL
dwg_obj_xrecord_get_num_xdata (const dwg_obj_xrecord *restrict xrecord,
                               int *restrict error)
{
  if (xrecord)
    {
      *error = 0;
      return xrecord->num_xdata;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0U;
    }
}

EXPORT Dwg_Resbuf *
dwg_obj_xrecord_get_xdata (const dwg_obj_xrecord *restrict xrecord,
                           int *restrict error)
{
  if (xrecord)
    {
      *error = 0;
      return xrecord->xdata;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return NULL;
    }
}

EXPORT void
dwg_obj_xrecord_set_xdata (dwg_obj_xrecord *restrict xrecord,
                           const Dwg_Resbuf *xdata, int *restrict error)
{
  if (xrecord)
    {
      *error = 0;
      xrecord->xdata = (Dwg_Resbuf *)xdata;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

EXPORT BITCODE_BL
dwg_obj_xrecord_get_num_objid_handles (const dwg_obj_xrecord *restrict xrecord,
                                       int *restrict error)
{
  if (xrecord)
    {
      *error = 0;
      return xrecord->num_objid_handles;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

EXPORT dwg_object_ref **
dwg_obj_xrecord_get_objid_handles (const dwg_obj_xrecord *restrict xrecord,
                                   int *restrict error)
{
  if (xrecord)
    {
      *error = 0;
      return xrecord->objid_handles;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return NULL;
    }
}

/*******************************************************************
 *                   FUNCTIONS FOR SPLINE ENTITY                     *
 ********************************************************************/

/** Returns spline scenario
 */
EXPORT BITCODE_BS
dwg_ent_spline_get_scenario (const dwg_ent_spline *restrict spline,
                             int *restrict error)
{
  if (spline)
    {
      *error = 0;
      return spline->scenario;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets spline scenario
 */
EXPORT void
dwg_ent_spline_set_scenario (dwg_ent_spline *restrict spline,
                             const BITCODE_BS scenario, int *restrict error)
{
  if (spline)
    {
      *error = 0;
      spline->scenario = scenario;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns spline degree
 */
EXPORT BITCODE_BS
dwg_ent_spline_get_degree (const dwg_ent_spline *restrict spline,
                           int *restrict error)
{
  if (spline)
    {
      *error = 0;
      return spline->degree;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets spline degree
 */
EXPORT void
dwg_ent_spline_set_degree (dwg_ent_spline *restrict spline,
                           const BITCODE_BS degree, int *restrict error)
{
  if (spline)
    {
      *error = 0;
      spline->degree = degree;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns spline fit tol
 */
EXPORT double
dwg_ent_spline_get_fit_tol (const dwg_ent_spline *restrict spline,
                            int *restrict error)
{
  if (spline)
    {
      *error = 0;
      return spline->fit_tol;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets spline fit tol
 */
EXPORT void
dwg_ent_spline_set_fit_tol (dwg_ent_spline *restrict spline, int fit_tol,
                            int *restrict error)
{
  if (spline)
    {
      *error = 0;
      spline->fit_tol = fit_tol;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns spline begin tan vector
 */
EXPORT void
dwg_ent_spline_get_begin_tan_vector (const dwg_ent_spline *restrict spline,
                                     dwg_point_3d *restrict point,
                                     int *restrict error)
{
  if (spline
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = spline->beg_tan_vec.x;
      point->y = spline->beg_tan_vec.y;
      point->z = spline->beg_tan_vec.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets spline begin tan vector
 */
EXPORT void
dwg_ent_spline_set_begin_tan_vector (dwg_ent_spline *restrict spline,
                                     const dwg_point_3d *restrict point,
                                     int *restrict error)
{
  if (spline
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      spline->beg_tan_vec.x = point->x;
      spline->beg_tan_vec.y = point->y;
      spline->beg_tan_vec.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns spline end tan vector points
 */
EXPORT void
dwg_ent_spline_get_end_tan_vector (const dwg_ent_spline *restrict spline,
                                   dwg_point_3d *restrict point,
                                   int *restrict error)
{
  if (spline
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = spline->end_tan_vec.x;
      point->y = spline->end_tan_vec.y;
      point->z = spline->end_tan_vec.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets spline end tan vector
 */
EXPORT void
dwg_ent_spline_set_end_tan_vector (dwg_ent_spline *restrict spline,
                                   const dwg_point_3d *restrict point,
                                   int *restrict error)
{
  if (spline
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      spline->end_tan_vec.x = point->x;
      spline->end_tan_vec.y = point->y;
      spline->end_tan_vec.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns spline knot tol value
 */
EXPORT double
dwg_ent_spline_get_knot_tol (const dwg_ent_spline *restrict spline,
                             int *restrict error)
{
  if (spline)
    {
      *error = 0;
      return spline->knot_tol;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets spline knot tol value
 */
EXPORT void
dwg_ent_spline_set_knot_tol (dwg_ent_spline *restrict spline,
                             const double knot_tol, int *restrict error)
{
  if (spline)
    {
      *error = 0;
      spline->knot_tol = knot_tol;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns spline control tol value
 */
EXPORT double
dwg_ent_spline_get_ctrl_tol (const dwg_ent_spline *restrict spline,
                             int *restrict error)
{
  if (spline)
    {
      *error = 0;
      return spline->ctrl_tol;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets spline control tol
 */
EXPORT void
dwg_ent_spline_set_ctrl_tol (dwg_ent_spline *restrict spline,
                             const double ctrl_tol, int *restrict error)
{
  if (spline)
    {
      *error = 0;
      spline->ctrl_tol = ctrl_tol;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns spline  number of fit points
 */
EXPORT BITCODE_BS
dwg_ent_spline_get_num_fit_pts (const dwg_ent_spline *restrict spline,
                                int *restrict error)
{
  if (spline)
    {
      *error = 0;
      return spline->num_fit_pts;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Returns spline rational
 */
EXPORT char
dwg_ent_spline_get_rational (const dwg_ent_spline *restrict spline,
                             int *restrict error)
{
  if (spline)
    {
      *error = 0;
      return spline->rational;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/** Sets rational value
 */
EXPORT void
dwg_ent_spline_set_rational (dwg_ent_spline *restrict spline, char rational,
                             int *restrict error)
{
  if (spline)
    {
      *error = 0;
      spline->rational = rational;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns spline closed_b
 */
EXPORT char
dwg_ent_spline_get_closed_b (const dwg_ent_spline *restrict spline,
                             int *restrict error)
{
  if (spline)
    {
      *error = 0;
      return spline->closed_b;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/** Sets spline closed_b
 */
EXPORT void
dwg_ent_spline_set_closed_b (dwg_ent_spline *restrict spline, char closed_b,
                             int *restrict error)
{
  if (spline)
    {
      *error = 0;
      spline->closed_b = closed_b;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns spline weighted value
 */
EXPORT char
dwg_ent_spline_get_weighted (const dwg_ent_spline *restrict spline,
                             int *restrict error)
{
  if (spline)
    {
      *error = 0;
      return spline->weighted;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/** Sets spline weighted
 */
EXPORT void
dwg_ent_spline_set_weighted (dwg_ent_spline *restrict spline, char weighted,
                             int *restrict error)
{
  if (spline)
    {
      *error = 0;
      spline->weighted = weighted;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns spline periodic
 */
EXPORT char
dwg_ent_spline_get_periodic (const dwg_ent_spline *restrict spline,
                             int *restrict error)
{
  if (spline)
    {
      *error = 0;
      return spline->periodic;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/** Sets spline periodic
 */
EXPORT void
dwg_ent_spline_set_periodic (dwg_ent_spline *restrict spline, char periodic,
                             int *restrict error)
{
  if (spline)
    {
      *error = 0;
      spline->periodic = periodic;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns spline knots number
 */
EXPORT BITCODE_BL
dwg_ent_spline_get_num_knots (const dwg_ent_spline *restrict spline,
                              int *restrict error)
{
  if (spline)
    {
      *error = 0;
      return spline->num_knots;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}

// TODO: dwg_ent_spline_add_fit_pts, dwg_ent_spline_delete_fit_pts
// TODO: dwg_ent_spline_add_knots, dwg_ent_spline_delete_knots
// TODO: dwg_ent_spline_add_ctrl_pts, dwg_ent_spline_delete_ctrl_pts

/** Returns spline control points number
 */
EXPORT BITCODE_BL
dwg_ent_spline_get_num_ctrl_pts (const dwg_ent_spline *restrict spline,
                                 int *restrict error)
{
  if (spline)
    {
      *error = 0;
      return spline->num_ctrl_pts;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}

/// Return all spline fit points
EXPORT dwg_point_3d *
dwg_ent_spline_get_fit_pts (const dwg_ent_spline *restrict spline,
                            int *restrict error)
{
  dwg_point_3d *ptx
      = (dwg_point_3d *)CALLOC (spline->num_fit_pts, sizeof (dwg_point_3d));
  if (ptx)
    {
      BITCODE_BS i;
      *error = 0;
      for (i = 0; i < spline->num_fit_pts; i++)
        {
          memcpy (&ptx[i], &spline->fit_pts[i], sizeof (dwg_point_3d));
        }
      return ptx;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: Out of memory", __FUNCTION__);
      return NULL;
    }
}

/** Returns spline control points
 */
EXPORT dwg_spline_control_point *
dwg_ent_spline_get_ctrl_pts (const dwg_ent_spline *restrict spline,
                             int *restrict error)
{
  dwg_spline_control_point *ptx = (dwg_spline_control_point *)CALLOC (
      spline->num_ctrl_pts, sizeof (dwg_spline_control_point));
  if (ptx)
    {
      BITCODE_BL i;
      *error = 0;
      for (i = 0; i < spline->num_ctrl_pts; i++)
        {
          ptx[i] = spline->ctrl_pts[i];
        }
      return ptx;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: Out of memory", __FUNCTION__)
      return NULL;
    }
}

/** Returns spline knots
 */
double *
dwg_ent_spline_get_knots (const dwg_ent_spline *restrict spline,
                          int *restrict error)
{
  double *ptx = (double *)MALLOC (sizeof (double) * spline->num_knots);
  if (ptx)
    {
      BITCODE_BL i;
      *error = 0;
      for (i = 0; i < spline->num_knots; i++)
        {
          ptx[i] = spline->knots[i];
        }
      return ptx;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: Out of memory", __FUNCTION__)
      return NULL;
    }
}

/*******************************************************************
 *                   FUNCTIONS FOR VIEWPORT ENTITY                   *
 ********************************************************************/

/** Returns viewport center points
 */
EXPORT void
dwg_ent_viewport_get_center (const dwg_ent_viewport *restrict vp,
                             dwg_point_3d *restrict point, int *restrict error)
{
  if (vp
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = vp->center.x;
      point->y = vp->center.y;
      point->z = vp->center.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets viewport center
 */
EXPORT void
dwg_ent_viewport_set_center (dwg_ent_viewport *restrict vp,
                             const dwg_point_3d *restrict point,
                             int *restrict error)
{
  if (vp
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      vp->center.x = point->x;
      vp->center.y = point->y;
      vp->center.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns viewport width
 */
EXPORT double
dwg_ent_viewport_get_width (const dwg_ent_viewport *restrict vp,
                            int *restrict error)
{
  if (vp)
    {
      *error = 0;
      return vp->width;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets viewport width
 */
EXPORT void
dwg_ent_viewport_set_width (dwg_ent_viewport *restrict vp, const double width,
                            int *restrict error)
{
  if (vp)
    {
      *error = 0;
      vp->width = width;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns viewport height
 */
EXPORT double
dwg_ent_viewport_get_height (const dwg_ent_viewport *restrict vp,
                             int *restrict error)
{
  if (vp)
    {
      *error = 0;
      return vp->height;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets viewport height
 */
EXPORT void
dwg_ent_viewport_set_height (dwg_ent_viewport *restrict vp,
                             const double height, int *restrict error)
{
  if (vp)
    {
      *error = 0;
      vp->height = height;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns viewport grid major
 */
EXPORT BITCODE_BS
dwg_ent_viewport_get_grid_major (const dwg_ent_viewport *restrict vp,
                                 int *restrict error)
{
  if (vp)
    {
      *error = 0;
      return vp->grid_major;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets viewport grid major
 */
EXPORT void
dwg_ent_viewport_set_grid_major (dwg_ent_viewport *restrict vp,
                                 const BITCODE_BS major, int *restrict error)
{
  if (vp)
    {
      *error = 0;
      vp->grid_major = major;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns viewport frozen layer count
 */
EXPORT BITCODE_BL
dwg_ent_viewport_get_num_frozen_layers (const dwg_ent_viewport *restrict vp,
                                        int *restrict error)
{
  if (vp)
    {
      *error = 0;
      return vp->num_frozen_layers;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}

/** Sets viewport frozen layer count (apparently safe to set)
 */
EXPORT void
dwg_ent_viewport_set_num_frozen_layers (dwg_ent_viewport *restrict vp,
                                        const BITCODE_BL count,
                                        int *restrict error)
{
  if (vp)
    {
      *error = 0;
      vp->num_frozen_layers = count;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns viewport style sheet name (utf-8 encoded)
 */
char *
dwg_ent_viewport_get_style_sheet (const dwg_ent_viewport *restrict vp,
                                  int *restrict error)
{
  if (vp)
    {
      *error = 0;
      if (dwg_version >= R_2007)
        return bit_convert_TU ((BITCODE_TU)vp->style_sheet);
      else
        return vp->style_sheet;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return NULL;
    }
}

/** Sets viewport style sheet name (utf-8 encoded)
 */
EXPORT void
dwg_ent_viewport_set_style_sheet (dwg_ent_viewport *restrict ent, char *sheet,
                                  int *restrict error)
{
  Dwg_Data *dwg = dwg_obj_generic_dwg (ent, error);
  if (ent && !error)
    {
      ent->style_sheet = dwg_add_u8_input (dwg, sheet);
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets circle zoom value
 */
EXPORT void
dwg_ent_viewport_set_circle_zoom (dwg_ent_viewport *restrict vp,
                                  const BITCODE_BS zoom, int *restrict error)
{
  if (vp)
    {
      *error = 0;
      vp->circle_zoom = zoom;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns circle zoom value
 */
EXPORT BITCODE_BS
dwg_ent_viewport_get_circle_zoom (const dwg_ent_viewport *restrict vp,
                                  int *restrict error)
{
  if (vp)
    {
      *error = 0;
      return vp->circle_zoom;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets viewport status flags
 */
EXPORT void
dwg_ent_viewport_set_status_flag (dwg_ent_viewport *restrict vp,
                                  const BITCODE_BL flags, int *restrict error)
{
  if (vp)
    {
      *error = 0;
      vp->status_flag = flags;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns viewport status flag
 */
EXPORT BITCODE_BL
dwg_ent_viewport_get_status_flag (const dwg_ent_viewport *restrict vp,
                                  int *restrict error)
{
  if (vp)
    {
      *error = 0;
      return vp->status_flag;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}

/** Returns VIEWPORT.render_mode
 */
EXPORT char
dwg_ent_viewport_get_render_mode (const dwg_ent_viewport *restrict vp,
                                  int *restrict error)
{
  if (vp)
    {
      *error = 0;
      return vp->render_mode;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/** Sets VIEWPORT.render_mode
 */
EXPORT void
dwg_ent_viewport_set_render_mode (dwg_ent_viewport *restrict vp, char mode,
                                  int *restrict error)
{
  if (vp)
    {
      *error = 0;
      vp->render_mode = mode;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets VIEWPORT.ucs_at_origin
 */
EXPORT void
dwg_ent_viewport_set_ucs_at_origin (dwg_ent_viewport *restrict vp,
                                    unsigned char origin, int *restrict error)
{
  if (vp)
    {
      *error = 0;
      vp->ucs_at_origin = origin;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns VIEWPORT.ucs_at_origin
 */
unsigned char
dwg_ent_viewport_get_ucs_at_origin (const dwg_ent_viewport *restrict vp,
                                    int *restrict error)
{
  if (vp)
    {
      *error = 0;
      return vp->ucs_at_origin;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/** Sets VIEWPORT.UCSVP
 */
EXPORT void
dwg_ent_viewport_set_UCSVP (dwg_ent_viewport *restrict vp, unsigned char ucsvp,
                            int *restrict error)
{
  if (vp)
    {
      *error = 0;
      vp->UCSVP = ucsvp;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns VIEWPORT.UCSVP
 */
unsigned char
dwg_ent_viewport_get_UCSVP (const dwg_ent_viewport *restrict vp,
                            int *restrict error)
{
  if (vp)
    {
      *error = 0;
      return vp->UCSVP;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/** Sets viewport view target
 */
EXPORT void
dwg_ent_viewport_set_view_target (dwg_ent_viewport *restrict vp,
                                  const dwg_point_3d *restrict point,
                                  int *restrict error)
{
  if (vp
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      vp->view_target.x = point->x;
      vp->view_target.y = point->y;
      vp->view_target.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns viewport view target
 */
EXPORT void
dwg_ent_viewport_get_view_target (const dwg_ent_viewport *restrict vp,
                                  dwg_point_3d *restrict point,
                                  int *restrict error)
{
  if (vp
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = vp->view_target.x;
      point->y = vp->view_target.y;
      point->z = vp->view_target.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets VIEWPORT.VIEWDIR
 */
EXPORT void
dwg_ent_viewport_set_VIEWDIR (dwg_ent_viewport *restrict vp,
                              const dwg_point_3d *restrict point,
                              int *restrict error)
{
  if (vp
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      vp->VIEWDIR.x = point->x;
      vp->VIEWDIR.y = point->y;
      vp->VIEWDIR.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns viewport view direction
 */
EXPORT void
dwg_ent_viewport_get_VIEWDIR (const dwg_ent_viewport *restrict vp,
                              dwg_point_3d *restrict point,
                              int *restrict error)
{
  if (vp
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = vp->VIEWDIR.x;
      point->y = vp->VIEWDIR.y;
      point->z = vp->VIEWDIR.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets VIEWPORT.twist_angle
 */
EXPORT void
dwg_ent_viewport_set_twist_angle (dwg_ent_viewport *restrict vp,
                                  const double angle, int *restrict error)
{
  if (vp)
    {
      *error = 0;
      vp->twist_angle = angle;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns VIEWPORT.twist_angle
 */
EXPORT double
dwg_ent_viewport_get_twist_angle (const dwg_ent_viewport *restrict vp,
                                  int *restrict error)
{
  if (vp)
    {
      *error = 0;
      return vp->twist_angle;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets VIEWPORT.VIEWSIZE
 */
EXPORT void
dwg_ent_viewport_set_VIEWSIZE (dwg_ent_viewport *restrict vp,
                               const double height, int *restrict error)
{
  if (vp)
    {
      *error = 0;
      vp->VIEWSIZE = height;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns VIEWPORT.VIEWSIZE
 */
EXPORT double
dwg_ent_viewport_get_VIEWSIZE (const dwg_ent_viewport *restrict vp,
                               int *restrict error)
{
  if (vp)
    {
      *error = 0;
      return vp->VIEWSIZE;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets viewport lens length
 */
EXPORT void
dwg_ent_viewport_set_lens_length (dwg_ent_viewport *restrict vp,
                                  const double length, int *restrict error)
{
  if (vp)
    {
      *error = 0;
      vp->lens_length = length;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns lens length
 */
EXPORT double
dwg_ent_viewport_get_lens_length (const dwg_ent_viewport *restrict vp,
                                  int *restrict error)
{
  if (vp)
    {
      *error = 0;
      return vp->lens_length;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets viewport front clip z value
 */
EXPORT void
dwg_ent_viewport_set_front_clip_z (dwg_ent_viewport *restrict vp,
                                   const double front_z, int *restrict error)
{
  if (vp)
    {
      *error = 0;
      vp->front_clip_z = front_z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns viewport front clip z value
 */
EXPORT double
dwg_ent_viewport_get_front_clip_z (const dwg_ent_viewport *restrict vp,
                                   int *restrict error)
{
  if (vp)
    {
      *error = 0;
      return vp->front_clip_z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets viewport back clip z value
 */
EXPORT void
dwg_ent_viewport_set_back_clip_z (dwg_ent_viewport *restrict vp,
                                  const double back_z, int *restrict error)
{
  if (vp)
    {
      *error = 0;
      vp->back_clip_z = back_z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns viewport back clip z value
 */
EXPORT double
dwg_ent_viewport_get_back_clip_z (const dwg_ent_viewport *restrict vp,
                                  int *restrict error)
{
  if (vp)
    {
      *error = 0;
      return vp->back_clip_z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets viewport snap angle
 */
EXPORT void
dwg_ent_viewport_set_SNAPANG (dwg_ent_viewport *restrict vp,
                              const double angle, int *restrict error)
{
  if (vp)
    {
      *error = 0;
      vp->SNAPANG = angle;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns viewport snap angle
 */
EXPORT double
dwg_ent_viewport_get_SNAPANG (const dwg_ent_viewport *restrict vp,
                              int *restrict error)
{
  if (vp)
    {
      *error = 0;
      return vp->SNAPANG;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Returns viewport view center
 */
EXPORT void
dwg_ent_viewport_get_VIEWCTR (const dwg_ent_viewport *restrict vp,
                              dwg_point_2d *restrict point,
                              int *restrict error)
{
  if (vp
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = vp->VIEWCTR.x;
      point->y = vp->VIEWCTR.y;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets viewport view center
 */
EXPORT void
dwg_ent_viewport_set_VIEWCTR (dwg_ent_viewport *restrict vp,
                              const dwg_point_2d *restrict point,
                              int *restrict error)
{
  if (vp)
    {
      *error = 0;
      vp->VIEWCTR.x = point->x;
      vp->VIEWCTR.y = point->y;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns grid spacing
 */
EXPORT void
dwg_ent_viewport_get_GRIDUNIT (const dwg_ent_viewport *restrict vp,
                               dwg_point_2d *restrict point,
                               int *restrict error)
{
  if (vp
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = vp->GRIDUNIT.x;
      point->y = vp->GRIDUNIT.y;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets grid spacing
 */
EXPORT void
dwg_ent_viewport_set_GRIDUNIT (dwg_ent_viewport *restrict vp,
                               const dwg_point_2d *restrict point,
                               int *restrict error)
{
  if (vp
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      vp->GRIDUNIT.x = point->x;
      vp->GRIDUNIT.y = point->y;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns viewport snap base
 */
EXPORT void
dwg_ent_viewport_get_SNAPBASE (const dwg_ent_viewport *restrict vp,
                               dwg_point_2d *restrict point,
                               int *restrict error)
{
  if (vp
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = vp->SNAPBASE.x;
      point->y = vp->SNAPBASE.y;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets viewport snap base
 */
EXPORT void
dwg_ent_viewport_set_SNAPBASE (dwg_ent_viewport *restrict vp,
                               const dwg_point_2d *restrict point,
                               int *restrict error)
{
  if (vp
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      vp->SNAPBASE.x = point->x;
      vp->SNAPBASE.y = point->y;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns viewport snap spacing
 */
EXPORT void
dwg_ent_viewport_get_SNAPUNIT (const dwg_ent_viewport *restrict vp,
                               dwg_point_2d *restrict point,
                               int *restrict error)
{
  if (vp
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = vp->SNAPUNIT.x;
      point->y = vp->SNAPUNIT.y;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets viewport snap spacing
 */
EXPORT void
dwg_ent_viewport_set_SNAPUNIT (dwg_ent_viewport *restrict vp,
                               const dwg_point_2d *restrict point,
                               int *restrict error)
{
  if (vp
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      vp->SNAPUNIT.x = point->x;
      vp->SNAPUNIT.y = point->y;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets viewport ucs origin
 */
EXPORT void
dwg_ent_viewport_set_ucsorg (dwg_ent_viewport *restrict vp,
                             const dwg_point_3d *restrict point,
                             int *restrict error)
{
  if (vp
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      vp->ucsorg.x = point->x;
      vp->ucsorg.y = point->y;
      vp->ucsorg.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns viewport ucs origin
 */
EXPORT void
dwg_ent_viewport_get_ucsorg (const dwg_ent_viewport *restrict vp,
                             dwg_point_3d *restrict point, int *restrict error)
{
  if (vp
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = vp->ucsorg.x;
      point->y = vp->ucsorg.y;
      point->z = vp->ucsorg.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets viewport ucs X axis
 */
EXPORT void
dwg_ent_viewport_set_ucsxdir (dwg_ent_viewport *restrict vp,
                              const dwg_point_3d *restrict point,
                              int *restrict error)
{
  if (vp
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      vp->ucsxdir.x = point->x;
      vp->ucsxdir.y = point->y;
      vp->ucsxdir.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns viewport ucs X axis
 */
EXPORT void
dwg_ent_viewport_get_ucsxdir (const dwg_ent_viewport *restrict vp,
                              dwg_point_3d *restrict point,
                              int *restrict error)
{
  if (vp
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = vp->ucsxdir.x;
      point->y = vp->ucsxdir.y;
      point->z = vp->ucsxdir.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets viewport ucs y axis
 */
EXPORT void
dwg_ent_viewport_set_ucsydir (dwg_ent_viewport *restrict vp,
                              const dwg_point_3d *restrict point,
                              int *restrict error)
{
  if (vp
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      vp->ucsydir.x = point->x;
      vp->ucsydir.y = point->y;
      vp->ucsydir.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns viewport ucs y axis
 */
EXPORT void
dwg_ent_viewport_get_ucsydir (const dwg_ent_viewport *restrict vp,
                              dwg_point_3d *restrict point,
                              int *restrict error)
{
  if (vp
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = vp->ucsydir.x;
      point->y = vp->ucsydir.y;
      point->z = vp->ucsydir.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets viewport ucs elevation
 */
EXPORT void
dwg_ent_viewport_set_ucs_elevation (dwg_ent_viewport *restrict vp,
                                    const double elevation,
                                    int *restrict error)
{
  if (vp)
    {
      *error = 0;
      vp->ucs_elevation = elevation;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns ucs elevation
 */
EXPORT double
dwg_ent_viewport_get_ucs_elevation (const dwg_ent_viewport *restrict vp,
                                    int *restrict error)
{
  if (vp)
    {
      *error = 0;
      return vp->ucs_elevation;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets UCS ortho view type
 */
EXPORT void
dwg_ent_viewport_set_UCSORTHOVIEW (dwg_ent_viewport *restrict vp,
                                   const BITCODE_BS type, int *restrict error)
{
  if (vp)
    {
      *error = 0;
      vp->UCSORTHOVIEW = type;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns UCS ortho view type
 */
EXPORT BITCODE_BS
dwg_ent_viewport_get_UCSORTHOVIEW (const dwg_ent_viewport *restrict vp,
                                   int *restrict error)
{
  if (vp)
    {
      *error = 0;
      return vp->UCSORTHOVIEW;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets shade plot mode value
 */
EXPORT void
dwg_ent_viewport_set_shadeplot_mode (dwg_ent_viewport *restrict vp,
                                     const BITCODE_BS shadeplot,
                                     int *restrict error)
{
  if (vp)
    {
      *error = 0;
      vp->shadeplot_mode = shadeplot;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns shade plot mode value
 */
EXPORT BITCODE_BS
dwg_ent_viewport_get_shadeplot_mode (const dwg_ent_viewport *restrict vp,
                                     int *restrict error)
{
  if (vp)
    {
      *error = 0;
      return vp->shadeplot_mode;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets viewport default lightning usage
 */
EXPORT void
dwg_ent_viewport_set_use_default_lights (dwg_ent_viewport *restrict vp,
                                         const unsigned char lights,
                                         int *restrict error)
{
  if (vp)
    {
      *error = 0;
      vp->use_default_lights = lights;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns viewport default lightning usage
 */
unsigned char
dwg_ent_viewport_get_use_default_lights (const dwg_ent_viewport *restrict vp,
                                         int *restrict error)
{
  if (vp)
    {
      *error = 0;
      return vp->use_default_lights;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/** Sets viewport default lightning type
 */
EXPORT void
dwg_ent_viewport_set_default_lighting_type (dwg_ent_viewport *restrict vp,
                                            const char type,
                                            int *restrict error)
{
  if (vp)
    {
      *error = 0;
      vp->default_lighting_type = type;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns viewport default lightning type
 */
EXPORT char
dwg_ent_viewport_get_default_lighting_type (
    const dwg_ent_viewport *restrict vp, int *restrict error)
{
  if (vp)
    {
      *error = 0;
      return vp->default_lighting_type;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/** Sets viewport brightness
 */
EXPORT void
dwg_ent_viewport_set_brightness (dwg_ent_viewport *restrict vp,
                                 const double brightness, int *restrict error)
{
  if (vp)
    {
      *error = 0;
      vp->brightness = brightness;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns viewport brightness
 */
EXPORT double
dwg_ent_viewport_get_brightness (const dwg_ent_viewport *restrict vp,
                                 int *restrict error)
{
  if (vp)
    {
      *error = 0;
      return vp->brightness;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets viewport contrast
 */
EXPORT void
dwg_ent_viewport_set_contrast (dwg_ent_viewport *restrict vp,
                               const double contrast, int *restrict error)
{
  if (vp)
    {
      *error = 0;
      vp->contrast = contrast;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns viewport contrast
 */
EXPORT double
dwg_ent_viewport_get_contrast (const dwg_ent_viewport *restrict vp,
                               int *restrict error)
{
  if (vp)
    {
      *error = 0;
      return vp->contrast;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/*******************************************************************
 *                FUNCTIONS FOR POLYLINE PFACE ENTITY                *
 ********************************************************************/

/** Returns polyline pface num verts
 */
EXPORT BITCODE_BS
dwg_ent_polyline_pface_get_numpoints (
    const dwg_ent_polyline_pface *restrict pface, int *restrict error)
{
  if (pface)
    {
      *error = 0;
      return pface->numverts;
    }
  else
    {
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      *error = 1;
      return 0;
    }
}

/** Returns polyline pface numfaces
 */
EXPORT BITCODE_BS
dwg_ent_polyline_pface_get_numfaces (
    const dwg_ent_polyline_pface *restrict pface, int *restrict error)
{
  if (pface)
    {
      *error = 0;
      return pface->numfaces;
    }
  else
    {
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      *error = 1;
      return 0;
    }
}

dwg_point_3d *
dwg_ent_polyline_pface_get_points (const dwg_object *restrict obj,
                                   int *restrict error)
{
  if (obj && obj->fixedtype == DWG_TYPE_POLYLINE_PFACE)
    {
      *error = 0;
      LOG_ERROR ("%s: nyi", __FUNCTION__);
      return NULL; // TODO
    }
  else
    {
      LOG_ERROR ("%s: empty or wrong arg", __FUNCTION__)
      *error = 1;
      return NULL;
    }
}

/*******************************************************************
 *                FUNCTIONS FOR POLYLINE_MESH ENTITY                 *
 ********************************************************************/

/** Returns polyline mesh flags
 */
EXPORT BITCODE_BS
dwg_ent_polyline_mesh_get_flag (const dwg_ent_polyline_mesh *restrict mesh,
                                int *restrict error)
{
  if (mesh)
    {
      *error = 0;
      return mesh->flag;
    }
  else
    {
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      *error = 1;
      return 0;
    }
}

/** Sets polyline mesh flags
 */
EXPORT void
dwg_ent_polyline_mesh_set_flag (dwg_ent_polyline_mesh *restrict mesh,
                                const BITCODE_BS flags, int *restrict error)
{
  if (mesh)
    {
      *error = 0;
      mesh->flag = flags;
    }
  else
    {
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      *error = 1;
    }
}

/** Returns polyline mesh curve type
 */
EXPORT BITCODE_BS
dwg_ent_polyline_mesh_get_curve_type (
    const dwg_ent_polyline_mesh *restrict mesh, int *restrict error)
{
  if (mesh)
    {
      *error = 0;
      return mesh->curve_type;
    }
  else
    {
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      *error = 1;
      return 0;
    }
}

/** Sets polyline mesh curve type
 */
EXPORT void
dwg_ent_polyline_mesh_set_curve_type (dwg_ent_polyline_mesh *restrict mesh,
                                      const BITCODE_BS curve_type,
                                      int *restrict error)
{
  if (mesh)
    {
      *error = 0;
      mesh->curve_type = curve_type;
    }
  else
    {
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      *error = 1;
    }
}

/** Returns polyline mesh n vert count
 */
EXPORT BITCODE_BS
dwg_ent_polyline_mesh_get_num_m_verts (
    const dwg_ent_polyline_mesh *restrict mesh, int *restrict error)
{
  if (mesh)
    {
      *error = 0;
      return mesh->num_m_verts;
    }
  else
    {
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      *error = 1;
      return 0;
    }
}

/// TODO: dwg_ent_polyline_mesh_add_{m,n}_vert,
/// dwg_ent_polyline_mesh_delete_{m,n}_vert

/** Returns polyline mesh n vert count
 */
EXPORT BITCODE_BS
dwg_ent_polyline_mesh_get_num_n_verts (
    const dwg_ent_polyline_mesh *restrict mesh, int *restrict error)
{
  if (mesh)
    {
      *error = 0;
      return mesh->num_n_verts;
    }
  else
    {
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      *error = 1;
      return 0;
    }
}

/** Returns polyline mesh n density
 */
EXPORT BITCODE_BS
dwg_ent_polyline_mesh_get_m_density (
    const dwg_ent_polyline_mesh *restrict mesh, int *restrict error)
{
  if (mesh)
    {
      *error = 0;
      return mesh->m_density;
    }
  else
    {
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      *error = 1;
      return 0;
    }
}

/** Sets polyline mesh M density
 */
EXPORT void
dwg_ent_polyline_mesh_set_m_density (dwg_ent_polyline_mesh *restrict mesh,
                                     const BITCODE_BS m_density,
                                     int *restrict error)
{
  if (mesh)
    {
      *error = 0;
      mesh->m_density = m_density;
    }
  else
    {
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      *error = 1;
    }
}

/** Returns polyline mesh n density
 */
EXPORT BITCODE_BS
dwg_ent_polyline_mesh_get_n_density (
    const dwg_ent_polyline_mesh *restrict mesh, int *restrict error)
{
  if (mesh)
    {
      *error = 0;
      return mesh->n_density;
    }
  else
    {
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      *error = 1;
      return 0;
    }
}

/** Sets polyline mesh n density
 */
EXPORT void
dwg_ent_polyline_mesh_set_n_density (dwg_ent_polyline_mesh *restrict mesh,
                                     const BITCODE_BS n_density,
                                     int *restrict error)
{
  if (mesh)
    {
      *error = 0;
      mesh->n_density = n_density;
    }
  else
    {
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      *error = 1;
    }
}

/** Returns polyline mesh owned object count
 */
EXPORT BITCODE_BL
dwg_ent_polyline_mesh_get_num_owned (
    const dwg_ent_polyline_mesh *restrict mesh, int *restrict error)
{
  if (mesh)
    {
      *error = 0;
      return mesh->num_owned;
    }
  else
    {
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      *error = 1;
      return 0L;
    }
}

/*******************************************************************
 *                 FUNCTIONS FOR POLYLINE_2D ENTITY                  *
 ********************************************************************/

/** Returns the _dwg_entity_POLYLINE_2D::extrusion vector, DXF 210.
  \param[in]  pline2d dwg_ent_polyline_2d*
  \param[out] vector  dwg_point_3d*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_polyline_2d_get_extrusion (const dwg_ent_polyline_2d *restrict pline2d,
                                   dwg_point_3d *restrict vector,
                                   int *restrict error)
{
  if (pline2d
#    ifndef HAVE_NONNULL
      && vector
#    endif
  )
    {
      *error = 0;
      vector->x = pline2d->extrusion.x;
      vector->y = pline2d->extrusion.y;
      vector->z = pline2d->extrusion.z;
    }
  else
    {
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      *error = 1;
    }
}

/** Sets the _dwg_entity_POLYLINE_2D::extrusion vector, DXF 210.
  \param[out] pline2d dwg_ent_polyline_2d*
  \param[in]  vector  dwg_point_3d*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_polyline_2d_set_extrusion (dwg_ent_polyline_2d *restrict pline2d,
                                   const dwg_point_3d *restrict vector,
                                   int *restrict error)
{
  if (pline2d
#    ifndef HAVE_NONNULL
      && vector
#    endif
  )
    {
      *error = 0;
      pline2d->extrusion.x = vector->x;
      pline2d->extrusion.y = vector->y;
      pline2d->extrusion.z = vector->z;
    }
  else
    {
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the _dwg_entity_POLYLINE_2D::start_width, DXF 40.
  \param[in]  pline2d dwg_ent_polyline_2d*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT double
dwg_ent_polyline_2d_get_start_width (
    const dwg_ent_polyline_2d *restrict pline2d, int *restrict error)
{
  if (pline2d)
    {
      *error = 0;
      return pline2d->start_width;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets the _dwg_entity_POLYLINE_2D::start_width, DXF 40.
  \param[out] pline2d     dwg_ent_polyline_2d*
  \param[in]  start_width double
  \param[out] error       int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_polyline_2d_set_start_width (dwg_ent_polyline_2d *restrict pline2d,
                                     const double start_width,
                                     int *restrict error)
{
  if (pline2d)
    {
      *error = 0;
      pline2d->start_width = start_width;
    }
  else
    {
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      *error = 1;
    }
}

/** Returns polyline 2d end width
 */
EXPORT double
dwg_ent_polyline_2d_get_end_width (const dwg_ent_polyline_2d *restrict pline2d,
                                   int *restrict error)
{
  if (pline2d)
    {
      *error = 0;
      return pline2d->end_width;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets polyline 2d end width
 */
EXPORT void
dwg_ent_polyline_2d_set_end_width (dwg_ent_polyline_2d *restrict pline2d,
                                   const double end_width, int *restrict error)
{
  if (pline2d)
    {
      *error = 0;
      pline2d->end_width = end_width;
    }
  else
    {
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      *error = 1;
    }
}

/** Returns polyline 2d thickness
 */
EXPORT double
dwg_ent_polyline_2d_get_thickness (const dwg_ent_polyline_2d *restrict pline2d,
                                   int *restrict error)
{
  if (pline2d)
    {
      *error = 0;
      return pline2d->thickness;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets polyline 2d thickness
 */
EXPORT void
dwg_ent_polyline_2d_set_thickness (dwg_ent_polyline_2d *restrict pline2d,
                                   const double thickness, int *restrict error)
{
  if (pline2d)
    {
      *error = 0;
      pline2d->thickness = thickness;
    }
  else
    {
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      *error = 1;
    }
}

/** Returns polyline 2d elevation
 */
EXPORT double
dwg_ent_polyline_2d_get_elevation (const dwg_ent_polyline_2d *restrict pline2d,
                                   int *restrict error)
{
  if (pline2d)
    {
      *error = 0;
      return pline2d->elevation;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets polyline 2d elevation
 */
EXPORT void
dwg_ent_polyline_2d_set_elevation (dwg_ent_polyline_2d *restrict pline2d,
                                   const double elevation, int *restrict error)
{
  if (pline2d)
    {
      *error = 0;
      pline2d->elevation = elevation;
    }
  else
    {
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the _dwg_entity_POLYLINE_2D::flag, DXF 70.
  \param[in]  pline2d dwg_ent_polyline_2d *
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT BITCODE_BS
dwg_ent_polyline_2d_get_flag (const dwg_ent_polyline_2d *restrict pline2d,
                              int *restrict error)
{
  if (pline2d)
    {
      *error = 0;
      return pline2d->flag;
    }
  else
    {
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      *error = 1;
      return 0;
    }
}

/** Sets the _dwg_entity_POLYLINE_2D:: flag, DXF 70.
  \param[out] pline2d dwg_ent_polyline_2d*
  \param[in]  flag    BITCODE_RC
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_polyline_2d_set_flag (dwg_ent_polyline_2d *restrict pline2d,
                              const BITCODE_BS flags, int *restrict error)
{
  if (pline2d)
    {
      *error = 0;
      pline2d->flag = flags;
    }
  else
    {
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      *error = 1;
    }
}

/** Returns curve type value
 */
EXPORT BITCODE_BS
dwg_ent_polyline_2d_get_curve_type (
    const dwg_ent_polyline_2d *restrict pline2d, int *restrict error)
{
  if (pline2d)
    {
      *error = 0;
      return pline2d->curve_type;
    }
  else
    {
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      *error = 1;
      return 0;
    }
}

/** Sets curve type value
 */
EXPORT void
dwg_ent_polyline_2d_set_curve_type (dwg_ent_polyline_2d *restrict pline2d,
                                    const BITCODE_BS curve_type,
                                    int *restrict error)
{
  if (pline2d)
    {
      *error = 0;
      pline2d->curve_type = curve_type;
    }
  else
    {
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      *error = 1;
    }
}

/*******************************************************************
 *                 FUNCTIONS FOR POLYLINE_3D ENTITY                  *
 ********************************************************************/

/** Returns the _dwg_entity_POLYLINE_3D:: flag, DXF 70.
  \param[in]  pline3d dwg_ent_polyline_3d *
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT BITCODE_RC
dwg_ent_polyline_3d_get_flag (const dwg_ent_polyline_3d *restrict pline3d,
                              int *restrict error)
{
  if (pline3d)
    {
      *error = 0;
      return pline3d->flag;
    }
  else
    {
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      *error = 1;
      return '\0';
    }
}

/** Sets the _dwg_entity_POLYLINE_3D:: flag, DXF 70.
  \param[out] pline3d dwg_ent_polyline_3d *
  \param[in]  flag    BITCODE_RC
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_polyline_3d_set_flag (dwg_ent_polyline_3d *restrict pline3d,
                              const BITCODE_RC flag, int *restrict error)
{
  if (pline3d)
    {
      *error = 0;
      pline3d->flag = flag;
    }
  else
    {
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      *error = 1;
    }
}

/** Returns polyline 3d curve_type
 */
EXPORT BITCODE_RC
dwg_ent_polyline_3d_get_curve_type (
    const dwg_ent_polyline_3d *restrict pline3d, int *restrict error)
{
  if (pline3d)
    {
      *error = 0;
      return pline3d->curve_type;
    }
  else
    {
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      *error = 1;
      return '\0';
    }
}

/** Sets polyline 3d curve_type
 */
EXPORT void
dwg_ent_polyline_3d_set_curve_type (dwg_ent_polyline_3d *restrict pline3d,
                                    const BITCODE_RC curve_type,
                                    int *restrict error)
{
  if (pline3d)
    {
      *error = 0;
      pline3d->curve_type = curve_type;
    }
  else
    {
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      *error = 1;
    }
}

/*******************************************************************
 *                   FUNCTIONS FOR 3DFACE ENTITY                     *
 ********************************************************************/

/** Returns the _dwg_entity_3DFACE::invis_flags, DXF 70.
  \code Usage: BITCODE_BS flag = dwg_ent_3dface_get_invis_flags(_3dface,
  &error); \endcode \param[in] _3dface  dwg_ent_3dface* \param[out] error   set
  to 0 for ok, 1 on error \deprecated
*/
EXPORT BITCODE_BS
dwg_ent_3dface_get_invis_flags (const dwg_ent_3dface *restrict _3dface,
                                int *restrict error)
{
  if (_3dface)
    {
      *error = 0;
      return _3dface->invis_flags;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets the _dwg_entity_3DFACE::invis_flags, DXF 70.
  \code Usage: dwg_ent_3dface_set_invis_flagsx(_3dface, flags, &error);
  \endcode
  \param[in,out] 3dface      dwg_ent_3dface*
  \param[in]     invis_flags BITCODE_BS
  \param[out]    error       int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_3dface_set_invis_flags (dwg_ent_3dface *restrict _3dface,
                                const BITCODE_BS invis_flags,
                                int *restrict error)
{
  if (_3dface)
    {
      *error = 0;
      _3dface->invis_flags = invis_flags;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_3DFACE::corner1 point, DXF 10.
  \code Usage: dwg_ent_3dface_get_corner1(face, &point, &error);
  \endcode
  \param[in]  3dface  dwg_ent_3dface*
  \param[out] point   dwg_point_3d*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_3dface_get_corner1 (const dwg_ent_3dface *restrict _3dface,
                            dwg_point_3d *restrict point, int *restrict error)
{
  if (_3dface
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = _3dface->corner1.x;
      point->y = _3dface->corner1.y;
      point->z = _3dface->corner1.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the _dwg_entity_3DFACE::corner1 point, DXF 10.
  \code Usage: dwg_ent_3dface_set_corner1(face, &point, &error);
  \endcode
  \param[in,out] 3dface   dwg_ent_3dface*
  \param[in]     point    dwg_point_3d*
  \param[out]    error    int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_3dface_set_corner1 (dwg_ent_3dface *restrict _3dface,
                            const dwg_point_3d *restrict point,
                            int *restrict error)
{
  if (_3dface
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      _3dface->corner1.x = point->x;
      _3dface->corner1.y = point->y;
      _3dface->corner1.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_3DFACE::corner2 point, DXF 11.
  \code Usage: dwg_ent_3dface_get_corner2(face, &point, &error);
  \endcode
  \param[in]  3dface  dwg_ent_3dface*
  \param[out] point   dwg_point_3d*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_3dface_get_corner2 (const dwg_ent_3dface *restrict _3dface,
                            dwg_point_3d *restrict point, int *restrict error)
{
  if (_3dface
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = _3dface->corner2.x;
      point->y = _3dface->corner2.y;
      point->z = _3dface->corner2.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the _dwg_entity_3DFACE::corner2 point, DXF 11.
  \code Usage: dwg_ent_3dface_set_corner2(face, &point, &error);
  \endcode
  \param[in,out] 3dface   dwg_ent_3dface*
  \param[in]     point    dwg_point_3d*
  \param[out]    error    int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_3dface_set_corner2 (dwg_ent_3dface *restrict _3dface,
                            const dwg_point_3d *restrict point,
                            int *restrict error)
{
  if (_3dface
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      _3dface->corner2.x = point->x;
      _3dface->corner2.y = point->y;
      _3dface->corner2.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_3DFACE::corner1 point, DXF 12.
  \code Usage: dwg_ent_3dface_get_corner3(face, &point, &error);
  \endcode
  \param[in]  3dface  dwg_ent_3dface*
  \param[out] point   dwg_point_3d*
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_3dface_get_corner3 (const dwg_ent_3dface *restrict _3dface,
                            dwg_point_3d *restrict point, int *restrict error)
{
  if (_3dface
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = _3dface->corner3.x;
      point->y = _3dface->corner3.y;
      point->z = _3dface->corner3.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the _dwg_entity_3DFACE::corner3 point, DXF 12.
  \code Usage: dwg_ent_3dface_set_corner3(face, &point, &error);
  \endcode
  \param[in,out] 3dface   dwg_ent_3dface*
  \param[in]     point    dwg_point_3d*
  \param[out]    error    int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_3dface_set_corner3 (dwg_ent_3dface *restrict _3dface,
                            const dwg_point_3d *restrict point,
                            int *restrict error)
{
  if (_3dface
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      _3dface->corner3.x = point->x;
      _3dface->corner3.y = point->y;
      _3dface->corner3.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_3DFACE::corner1 point, DXF 13.
  \code Usage: dwg_ent_3dface_get_corner4(face, &point, &error);
  \endcode
  \param[in]  3dface  dwg_ent_3dface*
  \param[out] point   dwg_point_3d
  \param[out] error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_3dface_get_corner4 (const dwg_ent_3dface *restrict _3dface,
                            dwg_point_3d *restrict point, int *restrict error)
{
  if (_3dface
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = _3dface->corner4.x;
      point->y = _3dface->corner4.y;
      point->z = _3dface->corner4.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the _dwg_entity_3DFACE::corner4 point, DXF 13.
  \code Usage: dwg_ent_3dface_set_corner4(face, &point, &error);
  \endcode
  \param[out]  3dface  dwg_ent_3dface*
  \param[in]   point   dwg_point_3d*
  \param[out]  error   int*, is set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_3dface_set_corner4 (dwg_ent_3dface *restrict _3dface,
                            const dwg_point_3d *restrict point,
                            int *restrict error)
{
  if (_3dface
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      _3dface->corner4.x = point->x;
      _3dface->corner4.y = point->y;
      _3dface->corner4.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/*******************************************************************
 *                    FUNCTIONS FOR IMAGE ENTITY                     *
 ********************************************************************/

/** Returns image class version
 */
EXPORT BITCODE_BL
dwg_ent_image_get_class_version (const dwg_ent_image *restrict image,
                                 int *restrict error)
{
  if (image)
    {
      *error = 0;
      return image->class_version;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}

/** Sets image class version
 */
EXPORT void
dwg_ent_image_set_class_version (dwg_ent_image *restrict image,
                                 const BITCODE_BL class_version,
                                 int *restrict error)
{
  if (image)
    {
      *error = 0;
      image->class_version = class_version;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns image point 0 points
 */
EXPORT void
dwg_ent_image_get_pt0 (const dwg_ent_image *restrict image,
                       dwg_point_3d *restrict point, int *restrict error)
{
  if (image
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = image->pt0.x;
      point->y = image->pt0.y;
      point->z = image->pt0.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets image point 0 points
 */
EXPORT void
dwg_ent_image_set_pt0 (dwg_ent_image *restrict image,
                       const dwg_point_3d *restrict point, int *restrict error)
{
  if (image
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      image->pt0.x = point->x;
      image->pt0.y = point->y;
      image->pt0.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns image U_vector points
 */
EXPORT void
dwg_ent_image_get_u_vector (const dwg_ent_image *restrict image,
                            dwg_point_3d *restrict point, int *restrict error)
{
  if (image
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = image->uvec.x;
      point->y = image->uvec.y;
      point->z = image->uvec.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets image U_vector points
 */
EXPORT void
dwg_ent_image_set_u_vector (dwg_ent_image *restrict image,
                            const dwg_point_3d *restrict point,
                            int *restrict error)
{
  if (image
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      image->uvec.x = point->x;
      image->uvec.y = point->y;
      image->uvec.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns image v_vector points
 */
EXPORT void
dwg_ent_image_get_v_vector (const dwg_ent_image *restrict image,
                            dwg_point_3d *restrict point, int *restrict error)
{
  if (image
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = image->vvec.x;
      point->y = image->vvec.y;
      point->z = image->vvec.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets v vector points
 */
EXPORT void
dwg_ent_image_set_v_vector (dwg_ent_image *restrict image,
                            const dwg_point_3d *restrict point,
                            int *restrict error)
{
  if (image
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      image->vvec.x = point->x;
      image->vvec.y = point->y;
      image->vvec.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns image size height
 */
EXPORT double
dwg_ent_image_get_size_height (const dwg_ent_image *restrict image,
                               int *restrict error)
{
  if (image)
    {
      *error = 0;
      return image->size.y;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets image size height
 */
EXPORT void
dwg_ent_image_set_size_height (dwg_ent_image *restrict image,
                               const double size_height, int *restrict error)
{
  if (image)
    {
      *error = 0;
      image->size.y = size_height;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns image size width
 */
EXPORT double
dwg_ent_image_get_size_width (const dwg_ent_image *restrict image,
                              int *restrict error)
{
  if (image)
    {
      *error = 0;
      return image->size.x;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets image size width
 */
EXPORT void
dwg_ent_image_set_size_width (dwg_ent_image *restrict image,
                              const double size_width, int *restrict error)
{
  if (image)
    {
      *error = 0;
      image->size.x = size_width;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns image display props
 */
EXPORT BITCODE_BS
dwg_ent_image_get_display_props (const dwg_ent_image *restrict image,
                                 int *restrict error)
{
  if (image)
    {
      *error = 0;
      return image->display_props;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets image display props
 */
EXPORT void
dwg_ent_image_set_display_props (dwg_ent_image *restrict image,
                                 const BITCODE_BS display_props,
                                 int *restrict error)
{
  if (image)
    {
      *error = 0;
      image->display_props = display_props;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns image clipping
 */
EXPORT BITCODE_B
dwg_ent_image_get_clipping (const dwg_ent_image *restrict image,
                            int *restrict error)
{
  if (image)
    {
      *error = 0;
      return image->clipping;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/** Sets image clipping
 */
EXPORT void
dwg_ent_image_set_clipping (dwg_ent_image *restrict image, BITCODE_B clipping,
                            int *restrict error)
{
  if (image)
    {
      *error = 0;
      image->clipping = clipping;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns image brightness
 */
EXPORT char
dwg_ent_image_get_brightness (const dwg_ent_image *restrict image,
                              int *restrict error)
{
  if (image)
    {
      *error = 0;
      return image->brightness;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/** Sets image brightness
 */
EXPORT void
dwg_ent_image_set_brightness (dwg_ent_image *restrict image,
                              const char brightness, int *restrict error)
{
  if (image)
    {
      *error = 0;
      image->brightness = brightness;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/// Return image contrast
EXPORT char
dwg_ent_image_get_contrast (const dwg_ent_image *restrict image,
                            int *restrict error)
{
  if (image)
    {
      *error = 0;
      return image->contrast;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/** Sets image contrast
 */
EXPORT void
dwg_ent_image_set_contrast (dwg_ent_image *restrict image, const char contrast,
                            int *restrict error)
{
  if (image)
    {
      *error = 0;
      image->contrast = contrast;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns image fade
 */
EXPORT char
dwg_ent_image_get_fade (const dwg_ent_image *restrict image,
                        int *restrict error)
{
  if (image)
    {
      *error = 0;
      return image->fade;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/** Sets image fade
 */
EXPORT void
dwg_ent_image_set_fade (dwg_ent_image *restrict image, const char fade,
                        int *restrict error)
{
  if (image)
    {
      *error = 0;
      image->fade = fade;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns image clip boundary type
 */
EXPORT BITCODE_BS
dwg_ent_image_get_clip_boundary_type (const dwg_ent_image *restrict image,
                                      int *restrict error)
{
  if (image)
    {
      *error = 0;
      return image->clip_boundary_type;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets image clip boundary type
 */
EXPORT void
dwg_ent_image_set_clip_boundary_type (dwg_ent_image *restrict image,
                                      const BITCODE_BS type,
                                      int *restrict error)
{
  if (image)
    {
      *error = 0;
      image->clip_boundary_type = type;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns image boundary point 0
 */
EXPORT void
dwg_ent_image_get_boundary_pt0 (const dwg_ent_image *restrict image,
                                dwg_point_2d *restrict point,
                                int *restrict error)
{
  if (image
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = image->clip_verts[0].x;
      point->y = image->clip_verts[0].y;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets image boundary point 0
 */
EXPORT void
dwg_ent_image_set_boundary_pt0 (dwg_ent_image *restrict image,
                                const dwg_point_2d *restrict point,
                                int *restrict error)
{
  if (image
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      image->clip_verts[0].x = point->x;
      image->clip_verts[0].y = point->y;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns boundary point1
 */
EXPORT void
dwg_ent_image_get_boundary_pt1 (const dwg_ent_image *restrict image,
                                dwg_point_2d *restrict point,
                                int *restrict error)
{
  if (image
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = image->clip_verts[1].x;
      point->y = image->clip_verts[1].y;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets boundary point1
 */
EXPORT void
dwg_ent_image_set_boundary_pt1 (dwg_ent_image *restrict image,
                                const dwg_point_2d *restrict point,
                                int *restrict error)
{
  if (image
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      image->clip_verts[1].x = point->x;
      image->clip_verts[1].y = point->y;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/// Returns number of clip verts
EXPORT double
dwg_ent_image_get_num_clip_verts (const dwg_ent_image *restrict image,
                                  int *restrict error)
{
  if (image)
    {
      *error = 0;
      return image->num_clip_verts;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Returns image clip verts
 */
EXPORT BITCODE_2RD *
dwg_ent_image_get_clip_verts (const dwg_ent_image *restrict image,
                              int *restrict error)
{
  BITCODE_2RD *ptx
      = (BITCODE_2RD *)CALLOC (image->num_clip_verts, sizeof (BITCODE_2RD));
  if (ptx)
    {
      BITCODE_BL i;
      *error = 0;
      for (i = 0; i < image->num_clip_verts; i++)
        {
          ptx[i] = image->clip_verts[i];
        }
      return ptx;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: Out of memory", __FUNCTION__)
      return NULL;
    }
}

/*******************************************************************
 *                    FUNCTIONS FOR MLINE ENTITY                     *
 ********************************************************************/

/** Sets mline scale value
 */
EXPORT void
dwg_ent_mline_set_scale (dwg_ent_mline *restrict mline, const double scale,
                         int *restrict error)
{
  if (mline)
    {
      *error = 0;
      mline->scale = scale;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns scale value
 */
EXPORT double
dwg_ent_mline_get_scale (const dwg_ent_mline *restrict mline,
                         int *restrict error)
{
  if (mline)
    {
      *error = 0;
      return mline->scale;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets justification value
 */
EXPORT void
dwg_ent_mline_set_justification (dwg_ent_mline *restrict mline,
                                 const BITCODE_RC justification,
                                 int *restrict error)
{
  if (mline)
    {
      *error = 0;
      mline->justification = justification;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns justification value
 */
EXPORT BITCODE_RC
dwg_ent_mline_get_justification (const dwg_ent_mline *restrict mline,
                                 int *restrict error)
{
  if (mline)
    {
      *error = 0;
      return mline->justification;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/** Sets base point value
 */
EXPORT void
dwg_ent_mline_set_base_point (dwg_ent_mline *restrict mline,
                              const dwg_point_3d *restrict point,
                              int *restrict error)
{
  if (mline
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      mline->base_point.x = point->x;
      mline->base_point.y = point->y;
      mline->base_point.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns base point value
 */
EXPORT void
dwg_ent_mline_get_base_point (const dwg_ent_mline *restrict mline,
                              dwg_point_3d *restrict point,
                              int *restrict error)
{
  if (mline
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = mline->base_point.x;
      point->y = mline->base_point.y;
      point->z = mline->base_point.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets extrusion points
 */
EXPORT void
dwg_ent_mline_set_extrusion (dwg_ent_mline *restrict mline,
                             const dwg_point_3d *restrict point,
                             int *restrict error)
{
  if (mline
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      mline->extrusion.x = point->x;
      mline->extrusion.y = point->y;
      mline->extrusion.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns extrusion points
 */
EXPORT void
dwg_ent_mline_get_extrusion (const dwg_ent_mline *restrict mline,
                             dwg_point_3d *restrict point, int *restrict error)
{
  if (mline
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = mline->extrusion.x;
      point->y = mline->extrusion.y;
      point->z = mline->extrusion.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets open closed value
 */
EXPORT void
dwg_ent_mline_set_flags (dwg_ent_mline *restrict mline, const BITCODE_BS oc,
                         int *restrict error)
{
  if (mline)
    {
      *error = 0;
      mline->flags = oc;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns flags value
 */
EXPORT BITCODE_BS
dwg_ent_mline_get_flags (const dwg_ent_mline *restrict mline,
                         int *restrict error)
{
  if (mline)
    {
      *error = 0;
      return mline->flags;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Returns number of lines
 */
EXPORT BITCODE_RC
dwg_ent_mline_get_num_lines (const dwg_ent_mline *restrict mline,
                             int *restrict error)
{
  if (mline)
    {
      *error = 0;
      return mline->num_lines;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/** Returns all mline vertex lines
 */
dwg_mline_line *
dwg_mline_vertex_get_lines (const dwg_mline_vertex *restrict vertex,
                            int *restrict error)
{
  dwg_mline_line *ptx
      = (dwg_mline_line *)CALLOC (vertex->num_lines, sizeof (dwg_mline_line));
  if (ptx)
    {
      BITCODE_BS i;
      *error = 0;
      for (i = 0; i < vertex->num_lines; i++)
        {
          ptx[i] = vertex->lines[i];
        }
      return ptx;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: Out of memory", __FUNCTION__)
      return NULL;
    }
}

/** Returns number of vertices
 */
EXPORT BITCODE_BS
dwg_ent_mline_get_num_verts (const dwg_ent_mline *restrict mline,
                             int *restrict error)
{
  if (mline)
    {
      *error = 0;
      return mline->num_verts;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Returns mline vertices
 */
dwg_mline_vertex *
dwg_ent_mline_get_verts (const dwg_ent_mline *restrict mline,
                         int *restrict error)
{
  dwg_mline_vertex *ptx = (dwg_mline_vertex *)CALLOC (
      mline->num_verts, sizeof (dwg_mline_vertex));
  if (ptx)
    {
      BITCODE_BS i;
      *error = 0;
      for (i = 0; i < mline->num_verts; i++)
        {
          ptx[i] = mline->verts[i];
        }
      return ptx;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: Out of memory", __FUNCTION__)
      return NULL;
    }
}

/*******************************************************************
 *              FUNCTIONS FOR VERTEX_PFACE_FACE ENTITY               *
 ********************************************************************/

/** Returns vertex_pface vertind
 */
EXPORT BITCODE_BSd
dwg_ent_vertex_pface_face_get_vertind (const dwg_ent_vert_pface_face *face)
{
  if (face)
    {
      return face->vertind[3];
    }
  else
    {
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return (BITCODE_BS)-1;
    }
}

/** Sets vertex_pface vertind
 */
EXPORT void
dwg_ent_vertex_pface_face_set_vertind (dwg_ent_vert_pface_face *restrict face,
                                       const BITCODE_BSd vertind[4])
{
  if (face
#    ifndef HAVE_NONNULL
      && vertind
#    endif
  )
    {
      face->vertind[0] = vertind[0];
      face->vertind[1] = vertind[1];
      face->vertind[2] = vertind[2];
      face->vertind[3] = vertind[3];
    }
  else
    {
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/*******************************************************************
 *                  FUNCTIONS FOR 3DSOLID ENTITY                     *
 ********************************************************************/

/** Returns the _dwg_entity_3DSOLID::acis_empty bit
 */
unsigned char
dwg_ent_3dsolid_get_acis_empty (const dwg_ent_3dsolid *restrict _3dsolid,
                                int *restrict error)
{
  if (_3dsolid)
    {
      *error = 0;
      return _3dsolid->acis_empty;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/** Sets the _dwg_entity_3DSOLID::acis_empty bit
 */
EXPORT void
dwg_ent_3dsolid_set_acis_empty (dwg_ent_3dsolid *restrict _3dsolid,
                                const unsigned char empty, int *restrict error)
{
  if (_3dsolid)
    {
      *error = 0;
      _3dsolid->acis_empty = empty;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_3DSOLID::version
/// the modeler format version number, DXF 70.
/// Should be 1, we cannot handle 2 yet
 */
EXPORT BITCODE_BS
dwg_ent_3dsolid_get_version (const dwg_ent_3dsolid *restrict _3dsolid,
                             int *restrict error)
{
  if (_3dsolid)
    {
      *error = 0;
      return _3dsolid->version;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Returns the decrypted _dwg_entity_3DSOLID::acis_data string.
 */
unsigned char *
dwg_ent_3dsolid_get_acis_data (const dwg_ent_3dsolid *restrict _3dsolid,
                               int *restrict error)
{
  if (_3dsolid)
    {
      *error = 0;
      return _3dsolid->acis_data;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return NULL;
    }
}

/** Sets the _dwg_entity_3DSOLID::acis_data string.
 */
EXPORT void
dwg_ent_3dsolid_set_acis_data (dwg_ent_3dsolid *restrict _3dsolid,
                               const unsigned char *restrict acis_data,
                               int *restrict error)
{
  if (_3dsolid)
    {
      *error = 0;
      _3dsolid->acis_data = (unsigned char *)acis_data;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_3DSOLID::wireframe_data_present bit
 */
EXPORT char
dwg_ent_3dsolid_get_wireframe_data_present (
    const dwg_ent_3dsolid *restrict _3dsolid, int *restrict error)
{
  if (_3dsolid)
    {
      *error = 0;
      return _3dsolid->wireframe_data_present;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/** Sets the _dwg_entity_3DSOLID::wireframe_data_present bit
 */
EXPORT void
dwg_ent_3dsolid_set_wireframe_data_present (dwg_ent_3dsolid *restrict _3dsolid,
                                            const char present,
                                            int *restrict error)
{
  if (_3dsolid)
    {
      *error = 0;
      _3dsolid->wireframe_data_present = present;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_3DSOLID::point_present bit
 */
EXPORT char
dwg_ent_3dsolid_get_point_present (const dwg_ent_3dsolid *restrict _3dsolid,
                                   int *restrict error)
{
  if (_3dsolid)
    {
      *error = 0;
      return _3dsolid->point_present;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/** Sets the _dwg_entity_3DSOLID::point_present bit
 */
EXPORT void
dwg_ent_3dsolid_set_point_present (dwg_ent_3dsolid *restrict _3dsolid,
                                   const char present, int *restrict error)
{
  if (_3dsolid)
    {
      *error = 0;
      _3dsolid->point_present = present;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_3DSOLID::point
 */
EXPORT void
dwg_ent_3dsolid_get_point (const dwg_ent_3dsolid *restrict _3dsolid,
                           dwg_point_3d *restrict point, int *restrict error)
{
  if (_3dsolid
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = _3dsolid->point.x;
      point->y = _3dsolid->point.y;
      point->z = _3dsolid->point.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the _dwg_entity_3DSOLID::point
 */
EXPORT void
dwg_ent_3dsolid_set_point (dwg_ent_3dsolid *restrict _3dsolid,
                           const dwg_point_3d *restrict point,
                           int *restrict error)
{
  if (_3dsolid
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      _3dsolid->point.x = point->x;
      _3dsolid->point.y = point->y;
      _3dsolid->point.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the number of _dwg_entity_3DSOLID::isolines
 */
EXPORT BITCODE_BL
dwg_ent_3dsolid_get_isolines (const dwg_ent_3dsolid *restrict _3dsolid,
                              int *restrict error)
{
  if (_3dsolid)
    {
      *error = 0;
      return _3dsolid->isolines;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}

/** Sets the number of _dwg_entity_3DSOLID::isolines
    (apparently safe to set)
 */
EXPORT void
dwg_ent_3dsolid_set_isolines (dwg_ent_3dsolid *restrict _3dsolid,
                              const BITCODE_BL num, int *restrict error)
{
  if (_3dsolid)
    {
      *error = 0;
      _3dsolid->isolines = num;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the _dwg_entity_3DSOLID::isoline_present bit
    If set, wires and silhouettes are present.
 */
EXPORT char
dwg_ent_3dsolid_get_isoline_present (const dwg_ent_3dsolid *restrict _3dsolid,
                                     int *restrict error)
{
  if (_3dsolid)
    {
      *error = 0;
      return _3dsolid->isoline_present;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/** Sets the _dwg_entity_3DSOLID::isoline_present bit
    If set, wires and silhouettes are present.
 */
EXPORT void
dwg_ent_3dsolid_set_isoline_present (dwg_ent_3dsolid *restrict _3dsolid,
                                     const char present, int *restrict error)
{
  if (_3dsolid)
    {
      *error = 0;
      _3dsolid->isoline_present = present;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the number of _dwg_entity_3DSOLID::num_wires wires
 */
EXPORT BITCODE_BL
dwg_ent_3dsolid_get_num_wires (const dwg_ent_3dsolid *restrict _3dsolid,
                               int *restrict error)
{
  if (_3dsolid)
    {
      *error = 0;
      return _3dsolid->num_wires;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}

/** Returns all _dwg_entity_3DSOLID::wire structs, as array of
 * _dwg_3DSOLID_wire::
 */
dwg_3dsolid_wire *
dwg_ent_3dsolid_get_wires (const dwg_ent_3dsolid *restrict _3dsolid,
                           int *restrict error)
{
  dwg_3dsolid_wire *wire = (dwg_3dsolid_wire *)CALLOC (
      _3dsolid->num_wires, sizeof (dwg_3dsolid_wire));
  if (wire)
    {
      BITCODE_BL i;
      *error = 0;
      for (i = 0; i < _3dsolid->num_wires; i++)
        {
          wire[i] = _3dsolid->wires[i];
        }
      return wire;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: Out of memory", __FUNCTION__)
      return NULL;
    }
}

/** Returns the number of _dwg_entity_3DSOLID::num_silhouettes entries.
 */
EXPORT BITCODE_BL
dwg_ent_3dsolid_get_num_silhouettes (const dwg_ent_3dsolid *restrict _3dsolid,
                                     int *restrict error)
{
  if (_3dsolid)
    {
      *error = 0;
      return _3dsolid->num_silhouettes;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}

/** Returns all _dwg_entity_3DSOLID::silhouettes structs,
    as array of _dwg_3DSOLID_silhouette:: structs.
 */
dwg_3dsolid_silhouette *
dwg_ent_3dsolid_get_silhouettes (const dwg_ent_3dsolid *restrict _3dsolid,
                                 int *restrict error)
{
  dwg_3dsolid_silhouette *sh = (dwg_3dsolid_silhouette *)CALLOC (
      _3dsolid->num_silhouettes, sizeof (dwg_3dsolid_silhouette));
  if (sh)
    {
      BITCODE_BL i;
      *error = 0;
      for (i = 0; i < _3dsolid->num_silhouettes; i++)
        {
          sh[i] = _3dsolid->silhouettes[i];
        }
      return sh;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: Out of memory", __FUNCTION__)
      return NULL;
    }
}

/** Returns the _dwg_entity_3DSOLID::acis_empty2 bit
 */
unsigned char
dwg_ent_3dsolid_get_acis_empty2 (const dwg_ent_3dsolid *restrict _3dsolid,
                                 int *restrict error)
{
  if (_3dsolid)
    {
      *error = 0;
      return _3dsolid->acis_empty2;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/** Sets the _dwg_entity_3DSOLID::acis_empty2 bit
 */
EXPORT void
dwg_ent_3dsolid_set_acis_empty2 (dwg_ent_3dsolid *restrict _3dsolid,
                                 unsigned char acis, int *restrict error)
{
  if (_3dsolid)
    {
      *error = 0;
      _3dsolid->acis_empty2 = acis;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/*******************************************************************
 *                   FUNCTIONS FOR REGION ENTITY                     *
 ********************************************************************/

/// Returns the Dwg_Entity_REGION::acis_empty bit
unsigned char
dwg_ent_region_get_acis_empty (const dwg_ent_region *restrict region,
                               int *restrict error)
{
  return dwg_ent_3dsolid_get_acis_empty ((const dwg_ent_3dsolid *)region,
                                         error);
}

/** Sets the Dwg_Entity_REGION::acis_empty bit
 */
EXPORT void
dwg_ent_region_set_acis_empty (dwg_ent_region *restrict region,
                               unsigned char empty, int *restrict error)
{
  dwg_ent_3dsolid_set_acis_empty ((dwg_ent_3dsolid *)region, empty, error);
}

/** Returns the Dwg_Entity_REGION::version
    the modeler format version number, DXF 70.
    Should be 1, we cannot handle 2 yet
 */
EXPORT BITCODE_BS
dwg_ent_region_get_version (const dwg_ent_region *restrict region,
                            int *restrict error)
{
  return dwg_ent_3dsolid_get_version ((const dwg_ent_3dsolid *)region, error);
}

/** Returns the decrypted Dwg_Entity_REGION::acis_data string.
 */
unsigned char *
dwg_ent_region_get_acis_data (const dwg_ent_region *restrict region,
                              int *restrict error)
{
  return dwg_ent_3dsolid_get_acis_data ((const dwg_ent_3dsolid *)region,
                                        error);
}

/** Sets the Dwg_Entity_REGION::acis_data string.
 */
EXPORT void
dwg_ent_region_set_acis_data (dwg_ent_region *restrict region,
                              const unsigned char *restrict sat_data,
                              int *restrict error)
{
  dwg_ent_3dsolid_set_acis_data ((dwg_ent_3dsolid *)region, sat_data, error);
}

/** Returns the Dwg_Entity_REGION::wireframe_data_present bit
 */
EXPORT char
dwg_ent_region_get_wireframe_data_present (
    const dwg_ent_region *restrict region, int *restrict error)
{
  return dwg_ent_3dsolid_get_wireframe_data_present (
      (const dwg_ent_3dsolid *)region, error);
}

/** Sets the Dwg_Entity_REGION::wireframe_data_present bit
 */
EXPORT void
dwg_ent_region_set_wireframe_data_present (dwg_ent_region *restrict region,
                                           const char present,
                                           int *restrict error)
{
  dwg_ent_3dsolid_set_wireframe_data_present ((dwg_ent_3dsolid *)region,
                                              present, error);
}

/** Returns the Dwg_Entity_REGION::point_present bit
 */
EXPORT char
dwg_ent_region_get_point_present (const dwg_ent_region *restrict region,
                                  int *restrict error)
{
  return dwg_ent_3dsolid_get_point_present ((const dwg_ent_3dsolid *)region,
                                            error);
}

/** Sets the Dwg_Entity_REGION::point_present bit
 */
EXPORT void
dwg_ent_region_set_point_present (dwg_ent_region *restrict region,
                                  const char present, int *restrict error)
{
  dwg_ent_3dsolid_set_point_present ((dwg_ent_3dsolid *)region, present,
                                     error);
}

/** Returns the Dwg_Entity_REGION::point
 */
EXPORT void
dwg_ent_region_get_point (const dwg_ent_region *restrict region,
                          dwg_point_3d *restrict point, int *restrict error)
{
  return dwg_ent_3dsolid_get_point ((const dwg_ent_3dsolid *)region, point,
                                    error);
}

/** Sets the Dwg_Entity_REGION::point
 */
EXPORT void
dwg_ent_region_set_point (dwg_ent_region *restrict region,
                          const dwg_point_3d *restrict point,
                          int *restrict error)
{
  dwg_ent_3dsolid_set_point ((dwg_ent_3dsolid *)region, point, error);
}

/** Returns the Dwg_Entity_REGION::isolines
 */
EXPORT BITCODE_BL
dwg_ent_region_get_isolines (const dwg_ent_region *restrict region,
                             int *restrict error)
{
  return dwg_ent_3dsolid_get_isolines ((const dwg_ent_3dsolid *)region, error);
}

/** Sets the Dwg_Entity_REGION::isolines (??)
 */
EXPORT void
dwg_ent_region_set_isolines (dwg_ent_region *restrict region,
                             const BITCODE_BL num, int *restrict error)
{
  dwg_ent_3dsolid_set_isolines ((dwg_ent_3dsolid *)region, num, error);
}

/** Returns the Dwg_Entity_REGION::isoline_present bit
/// If set, wires and silhouettes are present.
 */
EXPORT char
dwg_ent_region_get_isoline_present (const dwg_ent_region *restrict region,
                                    int *restrict error)
{
  return dwg_ent_3dsolid_get_isoline_present ((const dwg_ent_3dsolid *)region,
                                              error);
}

/** Sets the Dwg_Entity_REGION::isoline_present bit
/// If set, wires and silhouettes are present.
 */
EXPORT void
dwg_ent_region_set_isoline_present (dwg_ent_region *restrict region,
                                    char present, int *restrict error)
{
  dwg_ent_3dsolid_set_isoline_present ((dwg_ent_3dsolid *)region, present,
                                       error);
}

/** Returns the number of Dwg_Entity_REGION::num_wires wires
 */
EXPORT BITCODE_BL
dwg_ent_region_get_num_wires (const dwg_ent_region *restrict region,
                              int *restrict error)
{
  return dwg_ent_3dsolid_get_num_wires ((const dwg_ent_3dsolid *)region,
                                        error);
}

// TODO dwg_ent_region_add_wire, dwg_ent_region_delete_wire

/** Returns all Dwg_Entity_REGION::wire structs, as array of
 * _dwg_3DSOLID_wire::
 */
dwg_3dsolid_wire *
dwg_ent_region_get_wires (const dwg_ent_region *restrict region,
                          int *restrict error)
{
  return dwg_ent_3dsolid_get_wires ((const dwg_ent_3dsolid *)region, error);
}

/** Returns the number of Dwg_Entity_REGION::num_silhouettes entries.
 */
EXPORT BITCODE_BL
dwg_ent_region_get_num_silhouettes (const dwg_ent_region *restrict region,
                                    int *restrict error)
{
  return dwg_ent_3dsolid_get_num_silhouettes ((const dwg_ent_3dsolid *)region,
                                              error);
}

// TODO dwg_ent_region_add_silhouette, dwg_ent_region_delete_silhouette

/** Returns all Dwg_Entity_REGION::silhouettes structs, as array of
 * _dwg_3DSOLID_silhouette::
 */
dwg_3dsolid_silhouette *
dwg_ent_region_get_silhouettes (const dwg_ent_region *restrict region,
                                int *restrict error)
{
  return dwg_ent_3dsolid_get_silhouettes ((const dwg_ent_3dsolid *)region,
                                          error);
}

/** Returns the Dwg_Entity_REGION::acis_empty2 bit
 */
unsigned char
dwg_ent_region_get_acis_empty2 (const dwg_ent_region *restrict region,
                                int *restrict error)
{
  return dwg_ent_3dsolid_get_acis_empty2 ((const dwg_ent_3dsolid *)region,
                                          error);
}

/** Sets the Dwg_Entity_REGION::acis_empty2 bit
 */
EXPORT void
dwg_ent_region_set_acis_empty2 (dwg_ent_region *restrict region,
                                unsigned char empty2, int *restrict error)
{
  dwg_ent_3dsolid_set_acis_empty2 ((dwg_ent_3dsolid *)region, empty2, error);
}

/*******************************************************************
 *                    FUNCTIONS FOR BODY ENTITY                      *
 ********************************************************************/

// Returns the Dwg_Entity_BODY::acis_empty bit
unsigned char
dwg_ent_body_get_acis_empty (const dwg_ent_body *restrict body,
                             int *restrict error)
{
  return dwg_ent_3dsolid_get_acis_empty ((const dwg_ent_3dsolid *)body, error);
}

/** Sets the Dwg_Entity_BODY::acis_empty bit
 */
EXPORT void
dwg_ent_body_set_acis_empty (dwg_ent_body *restrict body, unsigned char empty,
                             int *restrict error)
{
  dwg_ent_3dsolid_set_acis_empty ((dwg_ent_3dsolid *)body, empty, error);
}

/** Returns the Dwg_Entity_BODY::version
/// the modeler format version number, DXF 70.
/// Should be 1, we cannot handle 2 yet
 */
EXPORT BITCODE_BS
dwg_ent_body_get_version (const dwg_ent_body *restrict body,
                          int *restrict error)
{
  return dwg_ent_3dsolid_get_version ((const dwg_ent_3dsolid *)body, error);
}

/** Returns the decrypted Dwg_Entity_BODY::acis_data string.
 */
unsigned char *
dwg_ent_body_get_acis_data (const dwg_ent_body *restrict body,
                            int *restrict error)
{
  return dwg_ent_3dsolid_get_acis_data ((const dwg_ent_3dsolid *)body, error);
}

/** Sets the Dwg_Entity_BODY::acis_data string.
 */
EXPORT void
dwg_ent_body_set_acis_data (dwg_ent_body *restrict body,
                            const unsigned char *restrict sat_data,
                            int *restrict error)
{
  dwg_ent_3dsolid_set_acis_data ((dwg_ent_3dsolid *)body, sat_data, error);
}

/** Returns the Dwg_Entity_BODY::wireframe_data_present bit
 */
EXPORT char
dwg_ent_body_get_wireframe_data_present (const dwg_ent_body *restrict body,
                                         int *restrict error)
{
  return dwg_ent_3dsolid_get_wireframe_data_present (
      (const dwg_ent_3dsolid *)body, error);
}

/** Sets the Dwg_Entity_BODY::wireframe_data_present bit
 */
EXPORT void
dwg_ent_body_set_wireframe_data_present (dwg_ent_body *restrict body,
                                         const char present,
                                         int *restrict error)
{
  dwg_ent_3dsolid_set_wireframe_data_present ((dwg_ent_3dsolid *)body, present,
                                              error);
}

/** Returns the Dwg_Entity_BODY::point_present bit
 */
EXPORT char
dwg_ent_body_get_point_present (const dwg_ent_body *restrict body,
                                int *restrict error)
{
  return dwg_ent_3dsolid_get_point_present ((const dwg_ent_3dsolid *)body,
                                            error);
}

/** Sets the Dwg_Entity_BODY::point_present bit
 */
EXPORT void
dwg_ent_body_set_point_present (dwg_ent_body *restrict body,
                                const char present, int *restrict error)
{
  dwg_ent_3dsolid_set_point_present ((dwg_ent_3dsolid *)body, present, error);
}

/** Returns the Dwg_Entity_BODY::point
 */
EXPORT void
dwg_ent_body_get_point (const dwg_ent_body *restrict body,
                        dwg_point_3d *restrict point, int *restrict error)
{
  return dwg_ent_3dsolid_get_point ((const dwg_ent_3dsolid *)body, point,
                                    error);
}

/** Sets the Dwg_Entity_BODY::point
 */
EXPORT void
dwg_ent_body_set_point (dwg_ent_body *restrict body,
                        const dwg_point_3d *restrict point,
                        int *restrict error)
{
  dwg_ent_3dsolid_set_point ((dwg_ent_3dsolid *)body, point, error);
}

/** Returns the Dwg_Entity_BODY::isolines
 */
EXPORT BITCODE_BL
dwg_ent_body_get_isolines (const dwg_ent_body *restrict body,
                           int *restrict error)
{
  return dwg_ent_3dsolid_get_isolines ((const dwg_ent_3dsolid *)body, error);
}

/** Sets the Dwg_Entity_BODY::isolines (??)
 */
EXPORT void
dwg_ent_body_set_isolines (dwg_ent_body *restrict body, const BITCODE_BL num,
                           int *restrict error)
{
  dwg_ent_3dsolid_set_isolines ((dwg_ent_3dsolid *)body, num, error);
}

/** Returns the Dwg_Entity_BODY::isoline_present bit
    If set, wires and silhouettes are present.
 */
EXPORT char
dwg_ent_body_get_isoline_present (const dwg_ent_body *restrict body,
                                  int *restrict error)
{
  return dwg_ent_3dsolid_get_isoline_present ((const dwg_ent_3dsolid *)body,
                                              error);
}

/** Sets the Dwg_Entity_BODY::isoline_present bit
    If set, wires and silhouettes are present.
 */
EXPORT void
dwg_ent_body_set_isoline_present (dwg_ent_body *restrict body, char present,
                                  int *restrict error)
{
  dwg_ent_3dsolid_set_isoline_present ((dwg_ent_3dsolid *)body, present,
                                       error);
}

/** Returns the number of Dwg_Entity_BODY::num_wires wires
 */
EXPORT BITCODE_BL
dwg_ent_body_get_num_wires (const dwg_ent_body *restrict body,
                            int *restrict error)
{
  return dwg_ent_3dsolid_get_num_wires ((const dwg_ent_3dsolid *)body, error);
}

// TODO dwg_ent_body_add_wire, dwg_ent_body_delete_wire

/** Returns all Dwg_Entity_BODY::wire structs, as array of _dwg_3DSOLID_wire::
 */
dwg_3dsolid_wire *
dwg_ent_body_get_wires (const dwg_ent_body *restrict body, int *restrict error)
{
  return dwg_ent_3dsolid_get_wires ((const dwg_ent_3dsolid *)body, error);
}

/** Returns the number of Dwg_Entity_BODY::num_silhouettes entries.
 */
EXPORT BITCODE_BL
dwg_ent_body_get_num_silhouettes (const dwg_ent_body *restrict body,
                                  int *restrict error)
{
  return dwg_ent_3dsolid_get_num_silhouettes ((const dwg_ent_3dsolid *)body,
                                              error);
}

// TODO dwg_ent_body_add_silhouette, dwg_ent_body_delete_silhouette

/** Returns all Dwg_Entity_BODY::silhouettes structs, as array of
 * _dwg_3DSOLID_silhouette::
 */
dwg_3dsolid_silhouette *
dwg_ent_body_get_silhouettes (const dwg_ent_body *restrict body,
                              int *restrict error)
{
  return dwg_ent_3dsolid_get_silhouettes ((const dwg_ent_3dsolid *)body,
                                          error);
}

/** Returns the Dwg_Entity_BODY::acis_empty2 bit
 */
unsigned char
dwg_ent_body_get_acis_empty2 (const dwg_ent_body *restrict body,
                              int *restrict error)
{
  return dwg_ent_3dsolid_get_acis_empty2 ((const dwg_ent_3dsolid *)body,
                                          error);
}

/** Sets the Dwg_Entity_BODY::acis_empty2 bit
 */
EXPORT void
dwg_ent_body_set_acis_empty2 (dwg_ent_body *restrict body,
                              unsigned char empty2, int *restrict error)
{
  dwg_ent_3dsolid_set_acis_empty2 ((dwg_ent_3dsolid *)body, empty2, error);
}

/*******************************************************************
 *                    FUNCTIONS FOR TABLE ENTITY                     *
 ********************************************************************/

/** Sets _dwg_entity_TABLE::ins_pt, DXF 10.
  \param[in,out] table      dwg_ent_table*
  \param[in]     point      dwg_point_3d *
  \param[out]    error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_set_insertion_pt (dwg_ent_table *restrict table,
                                const dwg_point_3d *restrict point,
                                int *restrict error)
{
  if (table
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      table->ins_pt.x = point->x;
      table->ins_pt.y = point->y;
      table->ins_pt.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::ins_pt, DXF 10.
  \param[in]  table      dwg_ent_table *
  \param[out] point      dwg_point_3d *
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_get_insertion_pt (const dwg_ent_table *restrict table,
                                dwg_point_3d *restrict point,
                                int *restrict error)
{
  if (table
#    ifndef HAVE_NONNULL
      && point
#    endif
  )
    {
      *error = 0;
      point->x = table->ins_pt.x;
      point->y = table->ins_pt.y;
      point->z = table->ins_pt.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets _dwg_entity_TABLE::scale, DXF 41. and the internal scale_flag.
  \param[in,out] table      dwg_ent_table*
  \param[in]     scale3d    dwg_point_3d*, scale in x, y, z
  \param[out]    error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_set_scale (dwg_ent_table *restrict table,
                         const dwg_point_3d *restrict scale3d,
                         int *restrict error)
{
  if (table
#    ifndef HAVE_NONNULL
      && scale3d
#    endif
  )
    {
      *error = 0;
      // set scale_flag (for r2000+)
      if (scale3d->x == 1.0)
        {
          if (scale3d->y == 1.0 && scale3d->z == 1.0)
            table->scale_flag = 3;
          else
            table->scale_flag = 1;
        }
      else if (scale3d->x == scale3d->y && scale3d->x == scale3d->z)
        table->scale_flag = 2;
      else
        table->scale_flag = 0;

      table->scale.x = scale3d->x;
      table->scale.y = scale3d->y;
      table->scale.z = scale3d->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::scale, DXF 41. if r13+
  \param[in]  table      dwg_ent_table *
  \param[out] scale3d    dwg_point_3d *
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_get_scale (const dwg_ent_table *restrict table,
                         dwg_point_3d *restrict scale3d, int *restrict error)
{
  if (table
#    ifndef HAVE_NONNULL
      && scale3d
#    endif
  )
    {
      *error = 0;
      scale3d->x = table->scale.x;
      scale3d->y = table->scale.y;
      scale3d->z = table->scale.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Sets _dwg_entity_TABLE::rotation, DXF 50.
  \param[in]  table      dwg_ent_table *
  \param[in]  rotation   double
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_set_rotation (dwg_ent_table *restrict table,
                            const double rotation, int *restrict error)
{
  if (table)
    {
      *error = 0;
      table->rotation = rotation;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::rotation, DXF 50.
  \param[in]  table      dwg_ent_table *
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT double
dwg_ent_table_get_rotation (const dwg_ent_table *restrict table,
                            int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->rotation;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets _dwg_entity_TABLE::extrusion, DXF 210.
  \param[in,out] table      dwg_ent_table *
  \param[in]     vector     dwg_point_3d *
  \param[out]    error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_set_extrusion (dwg_ent_table *restrict table,
                             const dwg_point_3d *restrict vector,
                             int *restrict error)
{
  if (table
#    ifndef HAVE_NONNULL
      && vector
#    endif
  )
    {
      *error = 0;
      table->extrusion.x = vector->x;
      table->extrusion.y = vector->y;
      table->extrusion.z = vector->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::extrusion, DXF 210.
  \param[in]  table      dwg_ent_table *
  \param[out] vector     dwg_point_3d *
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_get_extrusion (const dwg_ent_table *restrict table,
                             dwg_point_3d *restrict vector,
                             int *restrict error)
{
  if (table
#    ifndef HAVE_NONNULL
      && vector
#    endif
  )
    {
      *error = 0;
      vector->x = table->extrusion.x;
      vector->y = table->extrusion.y;
      vector->z = table->extrusion.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::has_attribs boolean, DXF 66.
  \param[in]  table      dwg_ent_table *
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
unsigned char
dwg_ent_table_has_attribs (dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->has_attribs;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

// needs to adjust handle array: add/delete
// TODO dwg_ent_table_add_owned, dwg_ent_table_delete_owned

/** Returns _dwg_entity_TABLE::num_owned, no DXF.
  \param[in]  table      dwg_ent_table *
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT BITCODE_BL
dwg_ent_table_get_num_owned (const dwg_ent_table *restrict table,
                             int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->num_owned;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}

/** Sets _dwg_entity_TABLE::flag_for_table_value, DXF 90.
  \param[in]  table      dwg_ent_table *
  \param[in]  value      short

    Bit flags, 0x06 (0x02 + 0x04): has block, 0x10: table direction, 0
    = up, 1 = down, 0x20: title suppressed. Normally 0x06 is always
    set.

  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_set_flag_for_table_value (dwg_ent_table *restrict table,
                                        const BITCODE_BS value,
                                        int *restrict error)
{
  if (table
#    ifndef HAVE_NONNULL
      && value < 0x30
#    endif
  )
    {
      *error = 0;
      table->flag_for_table_value = value;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::flag_for_table_value, DXF 90.
    \sa dwg_ent_table_set_flag_for_table_value
  \param[in]  table      dwg_ent_table *
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT BITCODE_BS
dwg_ent_table_get_flag_for_table_value (const dwg_ent_table *restrict table,
                                        int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->flag_for_table_value;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::horiz_direction, DXF 11.
  \param[out] table      dwg_ent_table *
  \param[in]  vector      dwg_point_3d *
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_set_horiz_direction (dwg_ent_table *restrict table,
                                   const dwg_point_3d *restrict vector,
                                   int *restrict error)
{
  if (table
#    ifndef HAVE_NONNULL
      && vector
#    endif
  )
    {
      *error = 0;
      table->horiz_direction.x = vector->x;
      table->horiz_direction.y = vector->y;
      table->horiz_direction.z = vector->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::horiz_direction, DXF 11.
  \param[in]  table      dwg_ent_table *
  \param[out]  vector      dwg_point_3d *
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_get_horiz_direction (const dwg_ent_table *restrict table,
                                   dwg_point_3d *restrict vector,
                                   int *restrict error)
{
  if (table
#    ifndef HAVE_NONNULL
      && vector
#    endif
  )
    {
      *error = 0;
      vector->x = table->horiz_direction.x;
      vector->y = table->horiz_direction.y;
      vector->z = table->horiz_direction.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::num_cols number of columns, DXF 91.
  \param[in]  table      dwg_ent_table *
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT BITCODE_BL
dwg_ent_table_get_num_cols (const dwg_ent_table *restrict table,
                            int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->num_cols;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}

/** Returns _dwg_entity_TABLE::num_cols number of rows, DXF 92.
  \param[in]  table      dwg_ent_table *
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT BITCODE_BL
dwg_ent_table_get_num_rows (const dwg_ent_table *restrict table,
                            int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->num_rows;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}

//  TODO dwg_ent_table_add_col, dwg_ent_table_delete_col
//  TODO dwg_ent_table_add_row, dwg_ent_table_delete_row

/** Returns array of _dwg_entity_TABLE::col_widths, DXF 142
  \param[in]  table      dwg_ent_table *
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
double *
dwg_ent_table_get_col_widths (const dwg_ent_table *restrict table,
                              int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->col_widths;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return NULL;
    }
}

/** Returns array of _dwg_entity_TABLE::row_heights, DXF 141
  \param[in]  table      dwg_ent_table *
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
double *
dwg_ent_table_get_row_heights (const dwg_ent_table *restrict table,
                               int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->row_heights;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return NULL;
    }
}

/** Returns _dwg_entity_TABLE::has_table_overrides boolean, no DXF.
  \param[in]  table      dwg_ent_table *
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT BITCODE_B
dwg_ent_table_has_table_overrides (dwg_ent_table *restrict table,
                                   int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->has_table_overrides;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/** Sets _dwg_entity_TABLE::table_flag_override, DXF 90.
  \param[in]  table      dwg_ent_table *
  \param[in]  override   0 - 0x7fffff
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_set_table_flag_override (dwg_ent_table *restrict table,
                                       const BITCODE_BL override,
                                       int *restrict error)
{
  if (table != NULL && override < 0x800000)
    {
      *error = 0;
      table->table_flag_override = override;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::table_flag_override, DXF 93.
  \param[in]  table      dwg_ent_table *
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT BITCODE_BL
dwg_ent_table_get_table_flag_override (const dwg_ent_table *restrict table,
                                       int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->table_flag_override;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}

/** Sets _dwg_entity_TABLE::title_suppressed, DXF 280.
  \param[in]  table      dwg_ent_table *
  \param[in]  yesno      0 or 1
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_set_title_suppressed (dwg_ent_table *restrict table,
                                    const unsigned char yesno,
                                    int *restrict error)
{
  if (table != NULL && yesno <= 1)
    {
      *error = 0;
      if (yesno)
        table->table_flag_override |= 0x1;
      table->title_suppressed = yesno;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::title_suppressed, DXF 280.
  \param[in]  table      dwg_ent_table *
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
unsigned char
dwg_ent_table_get_title_suppressed (const dwg_ent_table *restrict table,
                                    int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->title_suppressed;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/** Sets _dwg_entity_TABLE::header_suppressed, DXF 281.
  \param[in]  table      dwg_ent_table *
  \param[in]  header     0 or 1
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_set_header_suppressed (dwg_ent_table *restrict table,
                                     const unsigned char header,
                                     int *restrict error)
{
  if (table != NULL && header <= 1)
    {
      *error = 0;
      table->header_suppressed = header;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::header_suppressed, DXF 281.
  \param[in]  table      dwg_ent_table *
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
unsigned char
dwg_ent_table_get_header_suppressed (const dwg_ent_table *restrict table,
                                     int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->header_suppressed;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/** Sets _dwg_entity_TABLE::flow_direction, DXF 70.
  \param[in]  table      dwg_ent_table *
  \param[in]  dir        short?
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_set_flow_direction (dwg_ent_table *restrict table,
                                  const BITCODE_BS dir, int *restrict error)
{
  if (table)
    {
      *error = 0;
      if (dir)
        table->table_flag_override |= 0x4;
      table->flow_direction = dir;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::flow_direction, DXF 70.
  \param[in]  table      dwg_ent_table *
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT BITCODE_BS
dwg_ent_table_get_flow_direction (const dwg_ent_table *restrict table,
                                  int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->flow_direction;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::horiz_cell_margin, DXF 41.
  \param[in]  table      dwg_ent_table *
  \param[in]  margin     double
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_set_horiz_cell_margin (dwg_ent_table *restrict table,
                                     const double margin, int *restrict error)
{
  if (table)
    {
      *error = 0;
      if (margin > 0.0)
        table->table_flag_override |= 0x8;
      else
        table->table_flag_override &= ~0x8;
      table->horiz_cell_margin = margin;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::horiz_cell_margin, DXF 41.
  \param[in]  table      dwg_ent_table *
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT double
dwg_ent_table_get_horiz_cell_margin (const dwg_ent_table *restrict table,
                                     int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->horiz_cell_margin;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets _dwg_entity_TABLE::vert_cell_margin, DXF 41.
  \param[in]  table      dwg_ent_table *
  \param[in]  margin     double
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_set_vert_cell_margin (dwg_ent_table *restrict table,
                                    const double margin, int *restrict error)
{
  if (table)
    {
      *error = 0;
      if (margin > 0.0)
        table->table_flag_override |= 0x10;
      else
        table->table_flag_override &= ~0x10;
      table->vert_cell_margin = margin;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::vert_cell_margin, DXF 41.
  \param[in]  table      dwg_ent_table *
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT double
dwg_ent_table_get_vert_cell_margin (const dwg_ent_table *restrict table,
                                    int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->vert_cell_margin;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets _dwg_entity_TABLE::title_row_fill_none, DXF 283.
  \param[in]  table      dwg_ent_table *
  \param[in]  fill       ?
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_set_title_row_fill_none (dwg_ent_table *restrict table,
                                       const unsigned char fill,
                                       int *restrict error)
{
  if (table)
    {
      *error = 0;
      if (fill)
        table->table_flag_override |= 0x100;
      table->title_row_fill_none = fill;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::title_row_fill_none, DXF 283.
  \param[in]  table      dwg_ent_table *
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
unsigned char
dwg_ent_table_get_title_row_fill_none (const dwg_ent_table *restrict table,
                                       int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->title_row_fill_none;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/** Sets _dwg_entity_TABLE::header_row_fill_none, DXF 283.
  \param[in]  table      dwg_ent_table *
  \param[in]  fill       ?
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_set_header_row_fill_none (dwg_ent_table *restrict table,
                                        const unsigned char fill,
                                        int *restrict error)
{
  if (table)
    {
      *error = 0;
      if (fill)
        table->table_flag_override |= 0x200;
      table->header_row_fill_none = fill;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::header_row_fill_none, DXF 283.
  \param[in]  table      dwg_ent_table *
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
unsigned char
dwg_ent_table_get_header_row_fill_none (const dwg_ent_table *restrict table,
                                        int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->header_row_fill_none;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/** Sets _dwg_entity_TABLE::data_row_fill_none, DXF 283.
  \param[in]  table      dwg_ent_table *
  \param[in]  fill       ?
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_set_data_row_fill_none (dwg_ent_table *restrict table,
                                      const unsigned char fill,
                                      int *restrict error)
{
  if (table)
    {
      *error = 0;
      if (fill)
        table->table_flag_override |= 0x400;
      table->data_row_fill_none = fill;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::data_row_fill_none, DXF 283.
  \param[in]  table      dwg_ent_table *
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
unsigned char
dwg_ent_table_get_data_row_fill_none (const dwg_ent_table *restrict table,
                                      int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->data_row_fill_none;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/** Sets _dwg_entity_TABLE::title_row_align, DXF 170.
    and possibly enables bitmask 0x4000 of
    _dwg_entity_TABLE::table_flag_override, DXF 93 .
  \param[in]  table      dwg_ent_table *
  \param[in]  align      short?
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_set_title_row_alignment (dwg_ent_table *restrict table,
                                       const unsigned char align,
                                       int *restrict error)
{
  if (table)
    {
      *error = 0;
      if (align)
        table->table_flag_override |= 0x4000;
      table->title_row_alignment = align;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::title_row_align, DXF 170.
  \param[in]  table      dwg_ent_table *
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT BITCODE_BS
dwg_ent_table_get_title_row_alignment (const dwg_ent_table *restrict table,
                                       int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->title_row_alignment;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::header_row_align, DXF 170.
    and possibly enables bitmask 0x8000 of
_dwg_entity_TABLE::table_flag_override, DXF 93 . \param[in]  table
dwg_ent_table * \param[in]  alignment      short? \param[out] error      set to
0 for ok, 1 on error \deprecated
*/
EXPORT void
dwg_ent_table_set_header_row_alignment (dwg_ent_table *restrict table,
                                        const BITCODE_BS align,
                                        int *restrict error)
{
  if (table)
    {
      *error = 0;
      if (align)
        table->table_flag_override |= 0x8000;
      table->header_row_alignment = align;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::header_row_align, DXF 170.
    Might be ignored if bit of 0x8000 of table_flag_override DXF 93 is not set.
  \param[in]  table      dwg_ent_table *
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT BITCODE_BS
dwg_ent_table_get_header_row_alignment (const dwg_ent_table *restrict table,
                                        int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->header_row_alignment;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::data_row_align, DXF 170,
    and possibly table_flag_override 93.

    TODO: possible values?
  \param[in]  table      dwg_ent_table *
  \param[in]  alignment      short?
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_set_data_row_alignment (dwg_ent_table *restrict table,
                                      const BITCODE_BS align,
                                      int *restrict error)
{
  if (table)
    {
      *error = 0;
      if (align)
        table->table_flag_override |= 0x10000;
      table->data_row_alignment = align;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::data_row_align, DXF 170.
    Might be ignored if bit of 0x10000 of table_flag_override, DXF 93 is not
set. \param[in]  table      dwg_ent_table * \param[out] error      set to 0 for
ok, 1 on error \deprecated
*/
EXPORT BITCODE_BS
dwg_ent_table_get_data_row_alignment (const dwg_ent_table *restrict table,
                                      int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->data_row_alignment;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::title_row_height, DXF 140.
    and en/disables the _dwg_entity_TABLE::table_flag_override
  \param[in]  table      dwg_ent_table *
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_set_title_row_height (dwg_ent_table *restrict table,
                                    const double height, int *restrict error)
{
  if (table)
    {
      *error = 0;
      if (height > 0.0)
        table->table_flag_override |= 0x100000;
      else
        table->table_flag_override &= ~0x100000;
      table->title_row_height = height;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::title_row_height, DXF 140.
  \param[in]  table      dwg_ent_table *
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT double
dwg_ent_table_get_title_row_height (const dwg_ent_table *restrict table,
                                    int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->title_row_height;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets _dwg_entity_TABLE::header_row_height, DXF 140.
    and en/disables the _dwg_entity_TABLE::table_flag_override.
  \param[in]  table      dwg_ent_table *
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_set_header_row_height (dwg_ent_table *restrict table,
                                     const double height, int *restrict error)
{
  if (table)
    {
      *error = 0;
      if (height > 0.0)
        table->table_flag_override |= 0x200000;
      else
        table->table_flag_override &= ~0x200000;
      table->header_row_height = height;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::header_row_height, DXF 140.
  \param[in]  table      dwg_ent_table *
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT double
dwg_ent_table_get_header_row_height (const dwg_ent_table *restrict table,
                                     int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->header_row_height;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Sets _dwg_entity_TABLE::data_row_height, DXF 140.
    and en/disables the _dwg_entity_TABLE::table_flag_override.
  \param[in]  table      dwg_ent_table *
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_set_data_row_height (dwg_ent_table *restrict table,
                                   const double height, int *restrict error)
{
  if (table)
    {
      *error = 0;
      if (height > 0.0)
        table->table_flag_override |= 0x400000;
      else
        table->table_flag_override &= ~0x400000;
      table->data_row_height = height;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::data_row_height, DXF 140.
  \param[in]  table      dwg_ent_table *
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT double
dwg_ent_table_get_data_row_height (const dwg_ent_table *restrict table,
                                   int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->data_row_height;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return bit_nan ();
    }
}

/** Returns _dwg_entity_TABLE::has_border_color_overrides, if DXF 94 > 0.
  \param[in]  table      dwg_ent_table *
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
unsigned char
dwg_ent_table_has_border_color_overrides (dwg_ent_table *restrict table,
                                          int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->has_border_color_overrides;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/** Sets _dwg_entity_TABLE::border_color_overrides_flag, DXF 94.
    \sa dwg_ent_table_get_border_color_overrides_flag
  \param[in]  table      dwg_ent_table *
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_set_border_color_overrides_flag (dwg_ent_table *restrict table,
                                               const BITCODE_BL overrides,
                                               int *restrict error)
{
  if (table != NULL && overrides <= 1)
    {
      *error = 0;
      table->border_color_overrides_flag = overrides;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty or invalid arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::border_color_overrides_flag, DXF 94.

    Bitmask 1: has title_horiz_top_color
            2: has title_horiz_ins_color
            4: has title_horiz_bottom_color
            8: has title_vert_left_color
           10: has title_vert_ins_color
           20: has title_vert_right_color
           ...
  \param[in]  table      dwg_ent_table *
  \param[out] error      set to 0 for ok, 1 on error
 */
EXPORT BITCODE_BL
dwg_ent_table_get_border_color_overrides_flag (
    const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->border_color_overrides_flag;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}

/** Returns _dwg_entity_TABLE::has_border_lineweight_overrides, if DXF 95 > 0
  \param[in]  table      dwg_ent_table *
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
unsigned char
dwg_ent_table_has_border_lineweight_overrides (dwg_ent_table *restrict table,
                                               int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->has_border_lineweight_overrides;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/** Sets _dwg_entity_TABLE::border_lineweight_overrides_flag, DXF 95.
  \param[in]  table      dwg_ent_table *
  \param[in]  overrides  0 or 1
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_set_border_lineweight_overrides_flag (
    dwg_ent_table *restrict table, const BITCODE_BL overrides,
    int *restrict error)
{
  if (table != NULL && overrides <= 1)
    {
      *error = 0;
      table->border_lineweight_overrides_flag = overrides;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty or invalid arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::border_lineweight_overrides_flag, DXF 95.
  \param[in]  table      dwg_ent_table *
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT BITCODE_BL
dwg_ent_table_get_border_lineweight_overrides_flag (
    const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->border_lineweight_overrides_flag;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}

/** Sets _dwg_entity_TABLE::title_horiz_top_linewt
  \param[in]  table      dwg_ent_table *
  \param[in]  linewt     short
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_set_title_horiz_top_linewt (dwg_ent_table *restrict table,
                                          const BITCODE_BS linewt,
                                          int *restrict error)
{
  if (table)
    {
      *error = 0;
      table->title_horiz_top_linewt = linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::title_horiz_top_linewt
  \param[in]  table      dwg_ent_table *
  \param[in]  linewt     short
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT BITCODE_BS
dwg_ent_table_get_title_horiz_top_linewt (const dwg_ent_table *restrict table,
                                          int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->title_horiz_top_linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::title_horiz_ins_linewt
  \param[in]  table      dwg_ent_table *
  \param[in]  linewt     short
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_set_title_horiz_ins_linewt (dwg_ent_table *restrict table,
                                          const BITCODE_BS linewt,
                                          int *restrict error)
{
  if (table)
    {
      *error = 0;
      table->title_horiz_ins_linewt = linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::title_horiz_ins_linewt
  \param[in]  table      dwg_ent_table *
  \param[in]  linewt     short
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT BITCODE_BS
dwg_ent_table_get_title_horiz_ins_linewt (const dwg_ent_table *restrict table,
                                          int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->title_horiz_ins_linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::title_horiz_bottom_linewt
  \param[in]  table      dwg_ent_table *
  \param[in]  linewt     short
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_set_title_horiz_bottom_linewt (dwg_ent_table *restrict table,
                                             const BITCODE_BS linewt,
                                             int *restrict error)
{
  if (table)
    {
      *error = 0;
      table->title_horiz_bottom_linewt = linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::title_horiz_bottom_linewt
  \param[in]  table      dwg_ent_table *
  \param[in]  linewt     short
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT BITCODE_BS
dwg_ent_table_get_title_horiz_bottom_linewt (
    const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->title_horiz_bottom_linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::title_vert_left_linewt
  \param[in]  table      dwg_ent_table *
  \param[in]  linewt     short
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_set_title_vert_left_linewt (dwg_ent_table *restrict table,
                                          const BITCODE_BS linewt,
                                          int *restrict error)
{
  if (table)
    {
      *error = 0;
      table->title_vert_left_linewt = linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::title_vert_left_linewt
  \param[in]  table      dwg_ent_table *
  \param[in]  linewt     short
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT BITCODE_BS
dwg_ent_table_get_title_vert_left_linewt (const dwg_ent_table *restrict table,
                                          int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->title_vert_left_linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::title_vert_ins_linewt
  \param[in]  table      dwg_ent_table *
  \param[in]  linewt     short
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_set_title_vert_ins_linewt (dwg_ent_table *restrict table,
                                         const BITCODE_BS linewt,
                                         int *restrict error)
{
  if (table)
    {
      *error = 0;
      table->title_vert_ins_linewt = linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::title_vert_ins_linewt
  \param[in]  table      dwg_ent_table *
  \param[in]  linewt     short
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT BITCODE_BS
dwg_ent_table_get_title_vert_ins_linewt (const dwg_ent_table *restrict table,
                                         int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->title_vert_ins_linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::title_vert_right_linewt
  \param[in]  table      dwg_ent_table *
  \param[in]  linewt     short
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_set_title_vert_right_linewt (dwg_ent_table *restrict table,
                                           const BITCODE_BS linewt,
                                           int *restrict error)
{
  if (table)
    {
      *error = 0;
      table->title_vert_right_linewt = linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::title_vert_right_linewt
  \param[in]  table      dwg_ent_table *
  \param[in]  linewt     short
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT BITCODE_BS
dwg_ent_table_get_title_vert_right_linewt (const dwg_ent_table *restrict table,
                                           int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->title_vert_right_linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::header_horiz_top_linewt
  \param[in]  table      dwg_ent_table *
  \param[in]  linewt     short
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_set_header_horiz_top_linewt (dwg_ent_table *restrict table,
                                           const BITCODE_BS linewt,
                                           int *restrict error)
{
  if (table)
    {
      *error = 0;
      table->header_horiz_top_linewt = linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::header_horiz_top_linewt
  \param[in]  table      dwg_ent_table *
  \param[in]  linewt     short
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT BITCODE_BS
dwg_ent_table_get_header_horiz_top_linewt (const dwg_ent_table *restrict table,
                                           int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->header_horiz_top_linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::header_horiz_ins_linewt
  \param[in]  table      dwg_ent_table *
  \param[in]  linewt     short
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_set_header_horiz_ins_linewt (dwg_ent_table *restrict table,
                                           const BITCODE_BS linewt,
                                           int *restrict error)
{
  if (table)
    {
      *error = 0;
      table->header_horiz_ins_linewt = linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::header_horiz_ins_linewt
  \param[in]  table      dwg_ent_table *
  \param[in]  linewt     short
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT BITCODE_BS
dwg_ent_table_get_header_horiz_ins_linewt (const dwg_ent_table *restrict table,
                                           int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->header_horiz_ins_linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::header_horiz_bottom_linewt
  \param[in]  table      dwg_ent_table *
  \param[in]  linewt     short
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_set_header_horiz_bottom_linewt (dwg_ent_table *restrict table,
                                              const BITCODE_BS linewt,
                                              int *restrict error)
{
  if (table)
    {
      *error = 0;
      table->header_horiz_bottom_linewt = linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::header_horiz_bottom_linewt
  \param[in]  table      dwg_ent_table *
  \param[in]  linewt     short
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT BITCODE_BS
dwg_ent_table_get_header_horiz_bottom_linewt (
    const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->header_horiz_bottom_linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::header_vert_left_linewt
  \param[in]  table      dwg_ent_table *
  \param[in]  linewt     short
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_set_header_vert_left_linewt (dwg_ent_table *restrict table,
                                           const BITCODE_BS linewt,
                                           int *restrict error)
{
  if (table)
    {
      *error = 0;
      table->header_vert_left_linewt = linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::header_vert_left_linewt
  \param[in]  table      dwg_ent_table *
  \param[in]  linewt     short
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT BITCODE_BS
dwg_ent_table_get_header_vert_left_linewt (const dwg_ent_table *restrict table,
                                           int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->header_vert_left_linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::header_vert_ins_linewt
  \param[in]  table      dwg_ent_table *
  \param[in]  linewt     short
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_set_header_vert_ins_linewt (dwg_ent_table *restrict table,
                                          const BITCODE_BS linewt,
                                          int *restrict error)
{
  if (table)
    {
      *error = 0;
      table->header_vert_ins_linewt = linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::header_vert_ins_linewt
  \param[in]  table      dwg_ent_table *
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT BITCODE_BS
dwg_ent_table_get_header_vert_ins_linewt (const dwg_ent_table *restrict table,
                                          int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->header_vert_ins_linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::header_vert_right_linewt
  \param[in]  table      dwg_ent_table *
  \param[in]  linewt     short
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_set_header_vert_right_linewt (dwg_ent_table *restrict table,
                                            const BITCODE_BS linewt,
                                            int *restrict error)
{
  if (table)
    {
      *error = 0;
      table->header_vert_right_linewt = linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::header_vert_right_linewt
  \param[in]  table      dwg_ent_table *
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT BITCODE_BS
dwg_ent_table_get_header_vert_right_linewt (
    const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->header_vert_right_linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::data_horiz_top_linewt, DXF ?.
  \param[in]  table      dwg_ent_table *
  \param[in]  linewt     short
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_set_data_horiz_top_linewt (dwg_ent_table *restrict table,
                                         const BITCODE_BS linewt,
                                         int *restrict error)
{
  if (table)
    {
      *error = 0;
      table->data_horiz_top_linewt = linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::data_horiz_top_linewt, DXF ?.
  \param[in]  table      dwg_ent_table *
  \param[in]  linewt     short
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT BITCODE_BS
dwg_ent_table_get_data_horiz_top_linewt (const dwg_ent_table *restrict table,
                                         int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->data_horiz_top_linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::data_horiz_ins_linewt, DXF ?.
  \param[in]  table      dwg_ent_table *
  \param[in]  linewt     short
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_set_data_horiz_ins_linewt (dwg_ent_table *restrict table,
                                         const BITCODE_BS linewt,
                                         int *restrict error)
{
  if (table)
    {
      *error = 0;
      table->data_horiz_ins_linewt = linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::data_horiz_ins_linewt, DXF ?.
  \param[in]  table      dwg_ent_table *
  \param[in]  linewt     short
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT BITCODE_BS
dwg_ent_table_get_data_horiz_ins_linewt (const dwg_ent_table *restrict table,
                                         int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->data_horiz_ins_linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::data_horiz_bottom_linewt
  \param[in]  table      dwg_ent_table *
  \param[in]  linewt     short
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_set_data_horiz_bottom_linewt (dwg_ent_table *restrict table,
                                            const BITCODE_BS linewt,
                                            int *restrict error)
{
  if (table)
    {
      *error = 0;
      table->data_horiz_bottom_linewt = linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::data_horiz_bottom_linewt
  \param[in]  table      dwg_ent_table *
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT BITCODE_BS
dwg_ent_table_get_data_horiz_bottom_linewt (
    const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->data_horiz_bottom_linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::data_vert_ins_linewt
  \param[in]  table      dwg_ent_table *
  \param[in]  linewt     short
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_set_data_vert_ins_linewt (dwg_ent_table *restrict table,
                                        const BITCODE_BS linewt,
                                        int *restrict error)
{
  if (table)
    {
      *error = 0;
      table->data_vert_ins_linewt = linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::data_vert_ins_linewt
  \param[in]  table      dwg_ent_table *
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT BITCODE_BS
dwg_ent_table_get_data_vert_ins_linewt (const dwg_ent_table *restrict table,
                                        int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->data_vert_ins_linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::data_vert_right_linewt
  \param[in]  table      dwg_ent_table *
  \param[in]  linewt     short
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_set_data_vert_right_linewt (dwg_ent_table *restrict table,
                                          const BITCODE_BS linewt,
                                          int *restrict error)
{
  if (table)
    {
      *error = 0;
      table->data_vert_right_linewt = linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::data_vert_right_linewt
  \param[in]  table      dwg_ent_table *
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT BITCODE_BS
dwg_ent_table_get_data_vert_right_linewt (const dwg_ent_table *restrict table,
                                          int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->data_vert_right_linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::data_vert_left_linewt
  \param[in]  table      dwg_ent_table *
  \param[in]  linewt     short
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_set_data_vert_left_linewt (dwg_ent_table *restrict table,
                                         const BITCODE_BS linewt,
                                         int *restrict error)
{
  if (table)
    {
      *error = 0;
      table->data_vert_left_linewt = linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::data_vert_left_linewt
  \param[in]  table      dwg_ent_table *
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT BITCODE_BS
dwg_ent_table_get_data_vert_left_linewt (const dwg_ent_table *restrict table,
                                         int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->data_vert_left_linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Returns _dwg_entity_TABLE::has_border_visibility_overrides
  \param[in]  table      dwg_ent_table *
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
unsigned char
dwg_ent_table_has_border_visibility_overrides (dwg_ent_table *restrict table,
                                               int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->has_border_visibility_overrides;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/** Sets _dwg_entity_TABLE::border_visibility_overrides_flag, DXF 96.
  \param[in]  table      dwg_ent_table *
  \param[in]  overrides  0 or 1
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_set_border_visibility_overrides_flag (
    dwg_ent_table *restrict table, const BITCODE_BL overrides,
    int *restrict error)
{
  if (table != NULL && overrides <= 1)
    {
      *error = 0;
      table->border_visibility_overrides_flag = overrides;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty or invalid arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::border_visibility_overrides_flag, DXF 96.
  \param[in]  table      dwg_ent_table *
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT BITCODE_BL
dwg_ent_table_get_border_visibility_overrides_flag (
    const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->border_visibility_overrides_flag;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}

/** Sets _dwg_entity_TABLE::title_horiz_top_visibility, DXF ??.
  \param[in]  table      dwg_ent_table *
  \param[in]  visibility 0 or 1
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_set_title_horiz_top_visibility (dwg_ent_table *restrict table,
                                              const BITCODE_BS visibility,
                                              int *restrict error)
{
  if (table != NULL && visibility <= 1)
    {
      *error = 0;
      if (visibility)
        table->border_visibility_overrides_flag |= 0x1;
      table->title_horiz_top_visibility = visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty or invalid arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::title_horiz_top_visibility, DXF ??.
  \param[in]  table      dwg_ent_table *
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT BITCODE_BS
dwg_ent_table_get_title_horiz_top_visibility (
    const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->title_horiz_top_visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::title_horiz_ins_visibility, DXF ??.
  \param[in]  table      dwg_ent_table *
  \param[in]  visibility 0 or 1
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_set_title_horiz_ins_visibility (dwg_ent_table *restrict table,
                                              const BITCODE_BS visibility,
                                              int *restrict error)
{
  if (table != NULL && visibility <= 1)
    {
      *error = 0;
      if (visibility)
        table->border_visibility_overrides_flag |= 0x2;
      table->title_horiz_ins_visibility = visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty or invalid arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::title_horiz_ins_visibility, DXF ??.
  \param[in]  table      dwg_ent_table *
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT BITCODE_BS
dwg_ent_table_get_title_horiz_ins_visibility (
    const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->title_horiz_ins_visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::title_horiz_bottom_visibility, DXF ??.
  \param[in]  table      dwg_ent_table *
  \param[in]  visibility 0 or 1
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_set_title_horiz_bottom_visibility (dwg_ent_table *restrict table,
                                                 const BITCODE_BS visibility,
                                                 int *restrict error)
{
  if (table != NULL && visibility <= 1)
    {
      *error = 0;
      if (visibility)
        table->border_visibility_overrides_flag |= 0x4;
      table->title_horiz_bottom_visibility = visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty or invalid arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::title_horiz_bottom_visibility, DXF ??.
  \param[in]  table      dwg_ent_table *
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT BITCODE_BS
dwg_ent_table_get_title_horiz_bottom_visibility (
    const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->title_horiz_bottom_visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::title_vert_left_visibility, DXF ??.
  \param[in]  table      dwg_ent_table *
  \param[in]  visibility 0 or 1
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_set_title_vert_left_visibility (dwg_ent_table *restrict table,
                                              const BITCODE_BS visibility,
                                              int *restrict error)
{
  if (table != NULL && visibility <= 1)
    {
      *error = 0;
      if (visibility)
        table->border_visibility_overrides_flag |= 0x8;
      table->title_vert_left_visibility = visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty or invalid arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::title_vert_left_visibility, DXF ??.
  \param[in]  table      dwg_ent_table *
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT BITCODE_BS
dwg_ent_table_get_title_vert_left_visibility (
    const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->title_vert_left_visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::title_vert_ins_visibility, DXF ??.
  \param[in]  table      dwg_ent_table *
  \param[in]  visibility 0 or 1
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_set_title_vert_ins_visibility (dwg_ent_table *restrict table,
                                             const BITCODE_BS visibility,
                                             int *restrict error)
{
  if (table != NULL && visibility <= 1)
    {
      *error = 0;
      if (visibility)
        table->border_visibility_overrides_flag |= 0x10;
      table->title_vert_ins_visibility = visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty or invalid arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::title_vert_ins_visibility, DXF ??.
  \param[in]  table      dwg_ent_table *
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT BITCODE_BS
dwg_ent_table_get_title_vert_ins_visibility (
    const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->title_vert_ins_visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::title_vert_right_visibility, DXF ??.
  \param[in]  table      dwg_ent_table *
  \param[in]  visibility 0 or 1
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_set_title_vert_right_visibility (dwg_ent_table *restrict table,
                                               const BITCODE_BS visibility,
                                               int *restrict error)
{
  if (table != NULL && visibility <= 1)
    {
      *error = 0;
      if (visibility)
        table->border_visibility_overrides_flag |= 0x20;
      table->title_vert_right_visibility = visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty or invalid arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::title_vert_right_visibility, DXF ??.
  \param[in]  table      dwg_ent_table *
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT BITCODE_BS
dwg_ent_table_get_title_vert_right_visibility (
    const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->title_vert_right_visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::header_horiz_top_visibility, DXF ??.
  \param[in]  table      dwg_ent_table *
  \param[in]  visibility 0 or 1
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_set_header_horiz_top_visibility (dwg_ent_table *restrict table,
                                               const BITCODE_BS visibility,
                                               int *restrict error)
{
  if (table != NULL && visibility <= 1)
    {
      *error = 0;
      if (visibility)
        table->border_visibility_overrides_flag |= 0x40;
      table->header_horiz_top_visibility = visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty or invalid arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::header_horiz_top_visibility, DXF ??.
  \param[in]  table      dwg_ent_table *
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT BITCODE_BS
dwg_ent_table_get_header_horiz_top_visibility (
    const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->header_horiz_top_visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::header_horiz_ins_visibility, DXF ??.
  \param[in]  table      dwg_ent_table *
  \param[in]  visibility 0 or 1
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_set_header_horiz_ins_visibility (dwg_ent_table *restrict table,
                                               const BITCODE_BS visibility,
                                               int *restrict error)
{
  if (table != NULL && visibility <= 1)
    {
      *error = 0;
      if (visibility)
        table->border_visibility_overrides_flag |= 0x80;
      table->header_horiz_ins_visibility = visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty or invalid arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::header_horiz_ins_visibility, DXF ??.
  \param[in]  table      dwg_ent_table *
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT BITCODE_BS
dwg_ent_table_get_header_horiz_ins_visibility (
    const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->header_horiz_ins_visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::header_horiz_bottom_visibility, DXF ??.
  \param[in]  table      dwg_ent_table *
  \param[in]  visibility 0 or 1
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_set_header_horiz_bottom_visibility (
    dwg_ent_table *restrict table, const BITCODE_BS visibility,
    int *restrict error)
{
  if (table != NULL && visibility <= 1)
    {
      *error = 0;
      if (visibility)
        table->border_visibility_overrides_flag |= 0x100;
      table->header_horiz_bottom_visibility = visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty or invalid arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::header_horiz_bottom_visibility, DXF ??.
  \param[in]  table      dwg_ent_table *
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT BITCODE_BS
dwg_ent_table_get_header_horiz_bottom_visibility (
    const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->header_horiz_bottom_visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::header_vert_left_visibility, DXF ??.
  \param[in]  table      dwg_ent_table *
  \param[in]  visibility 0 or 1
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_set_header_vert_left_visibility (dwg_ent_table *restrict table,
                                               const BITCODE_BS visibility,
                                               int *restrict error)
{
  if (table != NULL && visibility <= 1)
    {
      *error = 0;
      if (visibility)
        table->border_visibility_overrides_flag |= 0x200;
      table->header_vert_left_visibility = visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty or invalid arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::header_vert_left_visibility, DXF ??.
  \param[in]  table      dwg_ent_table *
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT BITCODE_BS
dwg_ent_table_get_header_vert_left_visibility (
    const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->header_vert_left_visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::header_vert_ins_visibility, DXF ??.
  \param[in]  table      dwg_ent_table *
  \param[in]  visibility 0 or 1
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_set_header_vert_ins_visibility (dwg_ent_table *restrict table,
                                              const BITCODE_BS visibility,
                                              int *restrict error)
{
  if (table != NULL && visibility <= 1)
    {
      *error = 0;
      if (visibility)
        table->border_visibility_overrides_flag |= 0x400;
      table->header_vert_ins_visibility = visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty or invalid arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::header_vert_ins_visibility, DXF ??.
  \param[in]  table      dwg_ent_table *
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT BITCODE_BS
dwg_ent_table_get_header_vert_ins_visibility (
    const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->header_vert_ins_visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets data header vert right visibility
  \param[in]  table      dwg_ent_table *
  \param[in]  visibility 0 or 1
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_set_header_vert_right_visibility (dwg_ent_table *restrict table,
                                                const BITCODE_BS visibility,
                                                int *restrict error)
{
  if (table != NULL && visibility <= 1)
    {
      *error = 0;
      if (visibility)
        table->border_visibility_overrides_flag |= 0x800;
      table->header_vert_right_visibility = visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty or invalid arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::header_vert_right_visibility, DXF ??.
  \param[in]  table      dwg_ent_table *
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT BITCODE_BS
dwg_ent_table_get_header_vert_right_visibility (
    const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->header_vert_right_visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::data_horiz_top_visibility, DXF ??.
  \param[in]  table      dwg_ent_table *
  \param[in]  visibility 0 or 1
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_set_data_horiz_top_visibility (dwg_ent_table *restrict table,
                                             const BITCODE_BS visibility,
                                             int *restrict error)
{
  if (table != NULL && visibility <= 1)
    {
      *error = 0;
      if (visibility)
        table->border_visibility_overrides_flag |= 0x1000;
      table->data_horiz_top_visibility = visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty or invalid arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::data_horiz_top_visibility, DXF ??.
  \param[in]  table      dwg_ent_table *
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT BITCODE_BS
dwg_ent_table_get_data_horiz_top_visibility (
    const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->data_horiz_top_visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::data_horiz_ins_visibility, DXF ?
  \param[in]  table      dwg_ent_table *
  \param[in]  visibility 0 or 1
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_set_data_horiz_ins_visibility (dwg_ent_table *restrict table,
                                             const BITCODE_BS visibility,
                                             int *restrict error)
{
  if (table != NULL && visibility <= 1)
    {
      *error = 0;
      if (visibility)
        table->border_visibility_overrides_flag |= 0x2000;
      table->data_horiz_ins_visibility = visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty or invalid arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::data_horiz_ins_visibility, DXF ?
  \param[in]  table      dwg_ent_table *
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT BITCODE_BS
dwg_ent_table_get_data_horiz_ins_visibility (
    const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->data_horiz_ins_visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty or invalid arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::data_horiz_bottom_visibility, DXF ?.
  \param[in]  table      dwg_ent_table *
  \param[in]  visibility 0 or 1
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_set_data_horiz_bottom_visibility (dwg_ent_table *restrict table,
                                                const BITCODE_BS visibility,
                                                int *restrict error)
{
  if (table != NULL && visibility <= 1)
    {
      *error = 0;
      if (visibility)
        table->border_visibility_overrides_flag |= 0x4000;
      table->data_horiz_bottom_visibility = visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty or invalid arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::data_horiz_bottom_visibility, DXF ?.
  \param[in]  table      dwg_ent_table *
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT BITCODE_BS
dwg_ent_table_get_data_horiz_bottom_visibility (
    const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->data_horiz_bottom_visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::data_vert_left_visibility, DXF ?.
  \param[in]  table      dwg_ent_table *
  \param[in]  visibility 0 or 1
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_set_data_vert_left_visibility (dwg_ent_table *restrict table,
                                             const BITCODE_BS visibility,
                                             int *restrict error)
{
  if (table != NULL && visibility <= 1)
    {
      *error = 0;
      if (visibility)
        table->border_visibility_overrides_flag |= 0x8000;
      table->data_vert_left_visibility = visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty or invalid arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::data_vert_left_visibility, DXF ?.
  \param[in]  table  dwg_ent_table *
  \param[out] error  set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT BITCODE_BS
dwg_ent_table_get_data_vert_left_visibility (
    const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->data_vert_left_visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::data_vert_ins_visibility, DXF ?.
  \param[in]  table      dwg_ent_table *
  \param[in]  visibility 0 or 1
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_set_data_vert_ins_visibility (dwg_ent_table *restrict table,
                                            const BITCODE_BS visibility,
                                            int *restrict error)
{
  if (table != NULL && visibility <= 1)
    {
      *error = 0;
      if (visibility)
        table->border_visibility_overrides_flag |= 0x10000;
      table->data_vert_ins_visibility = visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty or invalid arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::data_vert_ins_visibility, DXF ?.
  \param[in]  table  dwg_ent_table *
  \param[out] error  set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT BITCODE_BS
dwg_ent_table_get_data_vert_ins_visibility (
    const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->data_vert_ins_visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets the table data vert right visibility.
    Bit 0x20000 of border_visibility override flag, DXF 96
  \param[in]  table      dwg_ent_table *
  \param[in]  visibility short: 0 = visible, 1 = invisible
  \param[out] error      set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT void
dwg_ent_table_set_data_vert_right_visibility (dwg_ent_table *restrict table,
                                              const BITCODE_BS visibility,
                                              int *restrict error)
{
  if (table != NULL && visibility <= 1)
    {
      *error = 0;
      if (visibility)
        table->border_visibility_overrides_flag |= 0x20000;
      table->data_vert_right_visibility = visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty or invalid arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::data vert right visibility, DXF ?.
    Bit 0x20000 of border_visibility override flag, DXF 96
  \param[in]  table  dwg_ent_table *
  \param[out] error  set to 0 for ok, 1 on error
  \deprecated
*/
EXPORT BITCODE_BS
dwg_ent_table_get_data_vert_right_visibility (
    const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->data_vert_right_visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

#  endif /* USE_DEPRECATED_API */

#endif /* __AFL_COMPILER ************************************************ */

/** Returns number of vertices
 */
EXPORT BITCODE_BL
dwg_object_polyline_2d_get_numpoints (const dwg_object *restrict obj,
                                      int *restrict error)
{
  if (obj && obj->fixedtype == DWG_TYPE_POLYLINE_2D)
    {
      BITCODE_BL num_points = 0;
      Dwg_Data *dwg = obj->parent;
      Dwg_Entity_POLYLINE_2D *_obj = obj->tio.entity->tio.POLYLINE_2D;
      Dwg_Entity_VERTEX_2D *vertex;
      *error = 0;

      if (dwg->header.version >= R_2004)
        return obj->tio.entity->tio.POLYLINE_2D->num_owned;
      // iterate over first_vertex - last_vertex
      else if (dwg->header.version >= R_13b1)
        {
          Dwg_Object *vobj = dwg_ref_object (dwg, _obj->first_vertex);
          Dwg_Object *vlast = dwg_ref_object (dwg, _obj->last_vertex);
          if (!vobj)
            *error = 1;
          else
            {
              do
                {
                  if (vobj->fixedtype == DWG_TYPE_VERTEX_2D)
                    num_points++;
                  else
                    *error = 1; // return not all vertices, but some
                }
              while ((vobj = dwg_next_object (vobj)) && vobj != vlast);
            }
        }
      else // <r13: iterate over vertices until seqend
        {
          Dwg_Object *vobj = (Dwg_Object *)obj;
          while ((vobj = dwg_next_object (vobj))
                 && vobj->fixedtype != DWG_TYPE_SEQEND)
            {
              if (vobj->fixedtype == DWG_TYPE_VERTEX_2D)
                num_points++;
              else
                *error = 1; // return not all vertices, but some
            }
        }
      return num_points;
    }
  else
    {
      LOG_ERROR ("%s: empty or wrong arg", __FUNCTION__)
      *error = 1;
      return 0L;
    }
}

/** Returns a copy of the points
 */
dwg_point_2d *
dwg_object_polyline_2d_get_points (const dwg_object *restrict obj,
                                   int *restrict error)
{
  *error = 0;
  if (obj && obj->fixedtype == DWG_TYPE_POLYLINE_2D)
    {
      BITCODE_BL i;
      Dwg_Data *dwg = obj->parent;
      Dwg_Entity_POLYLINE_2D *_obj = obj->tio.entity->tio.POLYLINE_2D;
      BITCODE_BL num_points
          = dwg_object_polyline_2d_get_numpoints (obj, error);
      Dwg_Entity_VERTEX_2D *vertex = NULL;
      dwg_point_2d *ptx;

      if (!num_points || *error)
        return NULL;
      ptx = (dwg_point_2d *)CALLOC (num_points, sizeof (dwg_point_2d));
      if (!ptx)
        {
          LOG_ERROR ("%s: Out of memory", __FUNCTION__);
          *error = 1;
          return NULL;
        }
      if (dwg->header.version >= R_2004)
        for (i = 0; i < num_points; i++)
          {
            Dwg_Object *vobj = dwg_ref_object (dwg, _obj->vertex[i]);
            if (vobj && vobj->fixedtype == DWG_TYPE_VERTEX_2D
                && (vertex = dwg_object_to_VERTEX_2D (vobj)))
              {
                ptx[i].x = vertex->point.x;
                ptx[i].y = vertex->point.y;
              }
            else
              {
                *error = 1; // return not all vertices, but some
              }
          }
      // iterate over first_vertex - last_vertex
      else if (dwg->header.version >= R_13b1)
        {
          Dwg_Object *vobj = dwg_ref_object (dwg, _obj->first_vertex);
          Dwg_Object *vlast = dwg_ref_object (dwg, _obj->last_vertex);
          if (!vobj)
            *error = 1;
          else
            {
              i = 0;
              do
                {
                  if (vobj->fixedtype == DWG_TYPE_VERTEX_2D
                      && (vertex = dwg_object_to_VERTEX_2D (vobj)))
                    {
                      ptx[i].x = vertex->point.x;
                      ptx[i].y = vertex->point.y;
                      i++;
                      if (i > num_points)
                        {
                          *error = 1;
                          break;
                        }
                    }
                  else
                    {
                      *error = 1; // return not all vertices, but some
                    }
                }
              while ((vobj = dwg_next_object (vobj)) && vobj != vlast);
            }
        }
      else // <r13: iterate over vertices until seqend
        {
          Dwg_Object *vobj = (Dwg_Object *)obj;
          i = 0;
          while ((vobj = dwg_next_object (vobj))
                 && vobj->fixedtype != DWG_TYPE_SEQEND)
            {
              if (vobj->fixedtype == DWG_TYPE_VERTEX_2D
                  && (vertex = dwg_object_to_VERTEX_2D (vobj)))
                {
                  ptx[i].x = vertex->point.x;
                  ptx[i].y = vertex->point.y;
                  i++;
                  if (i > num_points)
                    {
                      *error = 1;
                      break;
                    }
                }
              else
                {
                  *error = 1; // return not all vertices, but some
                }
            }
        }
      return ptx;
    }
  else
    {
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      *error = 1;
      return NULL;
    }
}

/** Returns the number of _dwg_object:: POLYLINE_3D vertices,
    the list of associated _dwg_object_VERTEX_3D:: points.
*/
EXPORT BITCODE_BL
dwg_object_polyline_3d_get_numpoints (const dwg_object *restrict obj,
                                      int *restrict error)
{
  if (obj && obj->fixedtype == DWG_TYPE_POLYLINE_3D)
    {
      BITCODE_BL num_points = 0;
      Dwg_Data *dwg = obj->parent;
      Dwg_Entity_POLYLINE_3D *_obj = obj->tio.entity->tio.POLYLINE_3D;
      Dwg_Entity_VERTEX_3D *vertex;
      *error = 0;

      if (dwg->header.version >= R_2004)
        return obj->tio.entity->tio.POLYLINE_3D->num_owned;
      // iterate over first_vertex - last_vertex
      else if (dwg->header.version >= R_13b1)
        {
          Dwg_Object *vobj = dwg_ref_object (dwg, _obj->first_vertex);
          Dwg_Object *vlast = dwg_ref_object (dwg, _obj->last_vertex);
          if (!vobj)
            *error = 1;
          else
            {
              do
                {
                  if (vobj->fixedtype == DWG_TYPE_VERTEX_3D
                      && (vertex = dwg_object_to_VERTEX_3D (vobj)))
                    {
                      num_points++;
                    }
                  else
                    {
                      *error = 1; // return not all vertices, but some
                    }
                }
              while ((vobj = dwg_next_object (vobj)) && vobj != vlast);
            }
        }
      else // <r13: iterate over vertices until seqend
        {
          Dwg_Object *vobj = (Dwg_Object *)obj;
          while ((vobj = dwg_next_object (vobj))
                 && vobj->fixedtype != DWG_TYPE_SEQEND)
            {
              if (vobj->fixedtype == DWG_TYPE_VERTEX_3D
                  && (vertex = dwg_object_to_VERTEX_3D (vobj)))
                num_points++;
              else
                *error = 1; // return not all vertices, but some
            }
        }
      return num_points;
    }
  else
    {
      LOG_ERROR ("%s: empty or wrong arg", __FUNCTION__)
      *error = 1;
      return 0L;
    }
}

/** Returns the _dwg_object:: POLYLINE_3D vertices,
    the list of associated _dwg_object_VERTEX_3D:: points.
*/
EXPORT dwg_point_3d *
dwg_object_polyline_3d_get_points (const dwg_object *restrict obj,
                                   int *restrict error)
{
  *error = 0;
  if (obj && obj->fixedtype == DWG_TYPE_POLYLINE_3D)
    {
      BITCODE_BL i;
      Dwg_Data *dwg = obj->parent;
      Dwg_Entity_POLYLINE_3D *_obj = obj->tio.entity->tio.POLYLINE_3D;
      const BITCODE_BL num_points
          = dwg_object_polyline_3d_get_numpoints (obj, error);
      Dwg_Entity_VERTEX_3D *vertex = NULL;
      dwg_point_3d *ptx;

      if (!num_points || *error)
        return NULL;
      ptx = (dwg_point_3d *)CALLOC (num_points, sizeof (dwg_point_3d));
      if (!ptx)
        {
          LOG_ERROR ("%s: Out of memory", __FUNCTION__);
          *error = 1;
          return NULL;
        }
      vertex = NULL;
      if (dwg->header.version >= R_2004)
        for (i = 0; i < num_points; i++)
          {
            Dwg_Object *vobj = dwg_ref_object (dwg, _obj->vertex[i]);
            if (vobj && vobj->fixedtype == DWG_TYPE_VERTEX_3D
                && (vertex = dwg_object_to_VERTEX_3D (vobj)))
              {
                ptx[i].x = vertex->point.x;
                ptx[i].y = vertex->point.y;
                ptx[i].z = vertex->point.z;
              }
            else
              {
                *error = 1; // return not all vertices, but some
              }
          }
      // iterate over first_vertex - last_vertex
      else if (dwg->header.version >= R_13b1)
        {
          Dwg_Object *vobj = dwg_ref_object (dwg, _obj->first_vertex);
          Dwg_Object *vlast = dwg_ref_object (dwg, _obj->last_vertex);
          if (!vobj)
            *error = 1;
          else
            {
              i = 0;
              do
                {
                  if (vobj->fixedtype == DWG_TYPE_VERTEX_3D
                      && (vertex = dwg_object_to_VERTEX_3D (vobj)))
                    {
                      ptx[i].x = vertex->point.x;
                      ptx[i].y = vertex->point.y;
                      ptx[i].z = vertex->point.z;
                      i++;
                      if (i > num_points)
                        {
                          *error = 1;
                          break;
                        }
                    }
                  else
                    {
                      *error = 1; // return not all vertices, but some
                    }
                }
              while ((vobj = dwg_next_object (vobj)) && vobj != vlast);
            }
        }
      else // <r13: iterate over vertices until seqend
        {
          Dwg_Object *vobj = (Dwg_Object *)obj;
          i = 0;
          while ((vobj = dwg_next_object (vobj))
                 && vobj->fixedtype != DWG_TYPE_SEQEND)
            {
              if (vobj->fixedtype == DWG_TYPE_VERTEX_3D
                  && (vertex = dwg_object_to_VERTEX_3D (vobj)))
                {
                  ptx[i].x = vertex->point.x;
                  ptx[i].y = vertex->point.y;
                  ptx[i].z = vertex->point.z;
                  i++;
                  if (i > num_points)
                    {
                      *error = 1;
                      break;
                    }
                }
              else
                {
                  *error = 1; // return not all vertices, but some
                }
            }
        }
      return ptx;
    }
  else
    {
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      *error = 1;
      return NULL;
    }
}

/** Returns lwpline bulges
 */
EXPORT double *
dwg_ent_lwpline_get_bulges (const dwg_ent_lwpline *restrict lwpline,
                            int *restrict error)
{
  BITCODE_BD *ptx
      = (BITCODE_BD *)MALLOC (sizeof (BITCODE_BD) * lwpline->num_bulges);
  if (ptx)
    {
      BITCODE_BL i;
      *error = 0;
      for (i = 0; i < lwpline->num_bulges; i++)
        {
          ptx[i] = lwpline->bulges[i];
        }
      return ptx;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: Out of memory", __FUNCTION__)
      return NULL;
    }
}

/** Returns lwpline point count
 */
EXPORT BITCODE_BL
dwg_ent_lwpline_get_numpoints (const dwg_ent_lwpline *restrict lwpline,
                               int *restrict error)
{
  if (lwpline)
    {
      *error = 0;
      return lwpline->num_points;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}

/** Returns lwpline points
 */
EXPORT dwg_point_2d *
dwg_ent_lwpline_get_points (const dwg_ent_lwpline *restrict lwpline,
                            int *restrict error)
{
  dwg_point_2d *ptx
      = (dwg_point_2d *)MALLOC (sizeof (dwg_point_2d) * lwpline->num_points);
  if (ptx)
    {
      BITCODE_BL i;
      *error = 0;
      for (i = 0; i < lwpline->num_points; i++)
        {
          ptx[i].x = lwpline->points[i].x;
          ptx[i].y = lwpline->points[i].y;
        }
      return ptx;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: Out of memory", __FUNCTION__)
      return NULL;
    }
}

EXPORT int
dwg_ent_lwpline_set_points (dwg_ent_lwpline *restrict lwpline,
                            const BITCODE_BL num_pts2d,
                            const dwg_point_2d *restrict pts2d)
{
  lwpline->points = (BITCODE_2RD *)MALLOC (sizeof (dwg_point_2d) * num_pts2d);
  if (lwpline->points)
    {
      lwpline->num_points = num_pts2d;
      for (BITCODE_BL i = 0; i < num_pts2d; i++)
        {
          const dwg_point_2d pt = pts2d[i];
          if (bit_isnan (pt.x) || bit_isnan (pt.y))
            goto isnan;
          lwpline->points[i].x = pt.x;
          lwpline->points[i].y = pt.y;
        }
      return 0;
    }
  else
    {
      LOG_ERROR ("%s: Out of memory", __FUNCTION__)
      return 1;
    }
isnan:
  LOG_ERROR ("%s: Invalid vertex nan", __FUNCTION__);
  return 2;
}

/** Returns lwpline widths
 */
EXPORT dwg_lwpline_widths *
dwg_ent_lwpline_get_widths (const dwg_ent_lwpline *restrict lwpline,
                            int *restrict error)
{
  dwg_lwpline_widths *ptx = (dwg_lwpline_widths *)MALLOC (
      sizeof (dwg_lwpline_widths) * lwpline->num_widths);
  if (ptx)
    {
      BITCODE_BL i;
      *error = 0;
      for (i = 0; i < lwpline->num_widths; i++)
        {
          ptx[i].start = lwpline->widths[i].start;
          ptx[i].end = lwpline->widths[i].end;
        }
      return ptx;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: Out of memory", __FUNCTION__)
      return NULL;
    }
}

/*******************************************************************
 *                    FUNCTIONS FOR TABLES                          *
 *        First the special tables: BLOCKS and LAYER                *
 ********************************************************************/

/*******************************************************************
 *               FUNCTIONS FOR BLOCK_CONTROL OBJECT                  *
 ********************************************************************/

/** Returns the block control object from the block header
  \code
    Usage: dwg_obj_block_control *blk = dwg_block_header_get_block_control(hdr,
  &error); \endcode \param[in] block_header \param[out] error  set to 0 for ok,
  >0 if not found.
*/
dwg_obj_block_control *
dwg_block_header_get_block_control (const dwg_obj_block_header *block_header,
                                    int *restrict error)
{
  if (block_header->parent && block_header->parent->ownerhandle
      && block_header->parent->ownerhandle->obj
      && block_header->parent->ownerhandle->obj->fixedtype
             == DWG_TYPE_BLOCK_CONTROL
      && block_header->parent->ownerhandle->obj->tio.object)
    {
      *error = 0;
      return block_header->parent->ownerhandle->obj->tio.object->tio
          .BLOCK_CONTROL;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty or invalid arg", __FUNCTION__)
      return NULL;
    }
}

/** Extracts and returns all block headers as references
  \param[in]  ctrl
  \param[out] error  set to 0 for ok, >0 if not found.
*/
dwg_object_ref **
dwg_obj_block_control_get_block_headers (
    const dwg_obj_block_control *restrict ctrl, int *restrict error)
{
  dwg_object_ref **ptx;

  if (ctrl->num_entries && !ctrl->entries)
    {
      *error = 1;
      LOG_ERROR ("%s: null block_headers", __FUNCTION__);
      return NULL;
    }

  ptx = (dwg_object_ref **)MALLOC (ctrl->num_entries
                                   * sizeof (Dwg_Object_Ref *));
  if (ptx)
    {
      BITCODE_BS i;
      *error = 0;
      for (i = 0; i < ctrl->num_entries; i++)
        {
          ptx[i] = ctrl->entries[i];
        }
      return ptx;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: Out of memory", __FUNCTION__)
      return NULL;
    }
}

/** Returns number of blocks
  \param[in]  ctrl
  \param[out] error  set to 0 for ok, >0 if not found.
*/
EXPORT BITCODE_BL
dwg_obj_block_control_get_num_entries (
    const dwg_obj_block_control *restrict ctrl, int *restrict error)
{
  if (ctrl)
    {
      *error = 0;
      return ctrl->num_entries;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}

/** Returns reference to model space block
  \param[in]  ctrl
  \param[out] error  set to 0 for ok, >0 if not found.
*/
dwg_object_ref *
dwg_obj_block_control_get_model_space (
    const dwg_obj_block_control *restrict ctrl, int *restrict error)
{
  if (ctrl)
    {
      *error = 0;
      return ctrl->model_space;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return NULL;
    }
}

/** Returns reference to paper space block
  \param[in]  ctrl
  \param[out] error  set to 0 for ok, >0 if not found.
*/
dwg_object_ref *
dwg_obj_block_control_get_paper_space (
    const dwg_obj_block_control *restrict ctrl, int *restrict error)
{
  if (ctrl)
    {
      *error = 0;
      return ctrl->paper_space;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty arg", __FUNCTION__)
      return NULL;
    }
}

/*******************************************************************
 *                FUNCTIONS FOR BLOCK_HEADER OBJECT                  *
 ********************************************************************/

/** Get name of the block header (utf-8 encoded)
  \code Usage: char* block_name = dwg_obj_block_header_get_name(hdr, &error);
  \endcode
  \param[in]  hdr
  \param[out] error  set to 0 for ok, >0 if not found.
*/
char *
dwg_obj_block_header_get_name (const dwg_obj_block_header *restrict hdr,
                               int *restrict error)
{
  *error = 0;
  if (dwg_version >= R_2007)
    return bit_convert_TU ((BITCODE_TU)hdr->name);
  else
    return hdr->name;
}

/** Returns 1st block header present in the dwg file.
    Usually the model space block.
  \code Usage: dwg_obj_block_header = dwg_get_block_header(dwg, &error);
  \endcode
  \param[in]  dwg
  \param[out] error  set to 0 for ok, >0 if not found.
*/
dwg_obj_block_header *
dwg_get_block_header (dwg_data *restrict dwg, int *restrict error)
{
  Dwg_Object *obj;
  Dwg_Object_BLOCK_HEADER *blk;

  *error = 0;
  if (dwg->num_classes > 1000 || dwg->num_objects > 0xfffffff)
    {
      *error = 1;
      return NULL;
    }
  if (dwg_version == R_INVALID)
    dwg_version = (Dwg_Version_Type)dwg->header.version;

  obj = &dwg->object[0];
  while (obj && obj->fixedtype != DWG_TYPE_BLOCK_HEADER)
    {
      if (obj->size > 0xffff)
        {
          *error = 2;
          return NULL;
        }
      obj = dwg_next_object (obj);
    }
  if (obj && DWG_TYPE_BLOCK_HEADER == obj->fixedtype)
    {
      if (obj->size > 0xffff)
        {
          *error = 2;
          return NULL;
        }
      blk = obj->tio.object->tio.BLOCK_HEADER;
      if (blk->name && strEQc (blk->name, "*Paper_Space"))
        dwg->pspace_block = obj;
      else if (blk->name && strEQc (blk->name, "*Model_Space"))
        dwg->mspace_block = obj;
      return blk;
    }
  else
    {
      *error = 3;
      LOG_ERROR ("%s: BLOCK_HEADER not found", __FUNCTION__)
      return NULL;
    }
}

/*******************************************************************
 *                    FUNCTIONS FOR LAYER OBJECT                     *
 ********************************************************************/

/** Get name of the layer (utf-8 encoded)
  \code Usage: char* layer_name = dwg_obj_layer_get_name(layer, &error);
  \endcode
  \param[in]  layer
  \param[out] error  set to 0 for ok, >0 if not found.
*/
char *
dwg_obj_layer_get_name (const dwg_obj_layer *restrict layer,
                        int *restrict error)
{
  const Dwg_Object *obj = dwg_obj_generic_to_object (layer, error);
  if (*error || obj->fixedtype != DWG_TYPE_LAYER)
    {
      *error = 1;
      LOG_ERROR ("%s: arg not a LAYER", __FUNCTION__)
      return NULL;
    }
  *error = 0;
  if (dwg_version >= R_2007)
    return bit_convert_TU ((BITCODE_TU)layer->name);
  else
    return layer->name;
}

/** Change name of the layer (utf-8 encoded).
    The result is freshly allocated from the input, so it can be safely
    free'd/deleted.
  \code Usage: error = dwg_obj_layer_set_name(layer, name);
  \endcode
  \param[in]  layer
  \param[in]  name   (utf-8 encoded)
  \param[out] error  set to 0 for ok, >0 if not found.
*/
EXPORT void
dwg_obj_layer_set_name (dwg_obj_layer *restrict layer,
                        const char *restrict name, int *restrict error)
{
  const Dwg_Object *obj = dwg_obj_generic_to_object (layer, error);
  if (*error || obj->fixedtype != DWG_TYPE_LAYER)
    {
      LOG_ERROR ("%s: arg not a LAYER", __FUNCTION__)
      *error = 1;
      return;
    }
  *error = 0;
  if (dwg_version >= R_2007)
    layer->name = bit_convert_TU ((BITCODE_TU)layer->name);
  else
    layer->name = STRDUP (name);
  return;
}

/*******************************************************************
 *           GENERIC FUNCTIONS FOR OTHER TABLE OBJECTS              *
 ********************************************************************/

/** Get number of table entries from the generic table control object.
  \code Usage: char* name = dwg_object_tablectrl_get_num_entries(obj, &error);
  \endcode
  \param[in]  obj    a TABLE_CONTROL dwg_object*
  \param[out] error  set to 0 for ok, >0 if not found.
*/
EXPORT BITCODE_BL
dwg_object_tablectrl_get_num_entries (const dwg_object *restrict obj,
                                      int *restrict error)
{
  if (obj->supertype == DWG_SUPERTYPE_OBJECT && dwg_obj_is_control (obj))
    {
      // HACK: we can guarantee that num_entries is always the first field.
      Dwg_Object_STYLE_CONTROL *ctrl = obj->tio.object->tio.STYLE_CONTROL;
      *error = 0;
      return ctrl->num_entries;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty or invalid table control arg %p, type: 0x%x",
                 __FUNCTION__, obj, obj->type)
      return 0;
    }
}

/** Get all table entries from the generic table control object.
  \code Usage: dwg_object_ref **refs = dwg_object_tablectrl_get_entries(obj,
&error); \endcode \param[in]  obj    a TABLE_CONTROL dwg_object* \param[out]
error  set to 0 for ok, >0 if not found.
*/
EXPORT dwg_object_ref **
dwg_object_tablectrl_get_entries (const dwg_object *restrict obj,
                                  int *restrict error)
{
  if (obj->supertype == DWG_SUPERTYPE_OBJECT && dwg_obj_is_control (obj))
    {
      // HACK: we can guarantee a common layout of the common fields
      Dwg_Object_STYLE_CONTROL *ctrl = obj->tio.object->tio.STYLE_CONTROL;
      return ctrl->entries;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty or invalid table control arg %p, type: 0x%x",
                 __FUNCTION__, obj, obj->type)
      return NULL;
    }
}

/** Get the nth table entry from the generic table control object.
  \code Usage: dwg_object_ref *ref = dwg_object_tablectrl_get_entry(obj, 0,
&error); \endcode \param[in]  obj    a TABLE_CONTROL dwg_object* \param[in] idx
BITCODE_BS \param[out] error  set to 0 for ok, >0 if not found.
*/
EXPORT dwg_object_ref *
dwg_object_tablectrl_get_entry (const dwg_object *restrict obj,
                                const BITCODE_BS idx, int *restrict error)
{
  if (obj->supertype == DWG_SUPERTYPE_OBJECT && dwg_obj_is_control (obj))
    {
      // HACK: we can guarantee a common layout of the common fields
      Dwg_Object_STYLE_CONTROL *ctrl = obj->tio.object->tio.STYLE_CONTROL;
      const BITCODE_BS count = ctrl->num_entries;
      if (idx < count)
        {
          *error = 0;
          return ctrl->entries[idx];
        }
      else
        {
          *error = 2;
          LOG_ERROR ("%s: idx %d out of bounds %d", __FUNCTION__, idx, count);
          return NULL;
        }
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty or invalid table control arg %p, type: 0x%x",
                 __FUNCTION__, obj, obj->type)
      return NULL;
    }
}

/** Get the null_handle from the generic table control object.
  \code Usage: dwg_object_ref *ref = dwg_object_tablectrl_get_null_handle(obj,
&error); \endcode \param[in]  obj    a TABLE_CONTROL dwg_object* \param[out]
error  set to 0 for ok, >0 if not found.
*/
EXPORT dwg_object_ref *
dwg_object_tablectrl_get_ownerhandle (const dwg_object *restrict obj,
                                      int *restrict error)
{
  if (obj->supertype == DWG_SUPERTYPE_OBJECT && dwg_obj_is_control (obj))
    {
      return obj->tio.object->ownerhandle;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty or invalid table control arg %p, type: 0x%x",
                 __FUNCTION__, obj, obj->type)
      return NULL;
    }
}

/** Get the xdicobjhandle from the generic table control object.
  \code Usage: dwg_object_ref *ref =
dwg_object_tablectrl_get_xdicobjhandle(obj, &error); \endcode \param[in]  obj
a TABLE_CONTROL dwg_object* \param[out] error  set to 0 for ok, >0 if not
found.
*/
EXPORT dwg_object_ref *
dwg_object_tablectrl_get_xdicobjhandle (const dwg_object *restrict obj,
                                        int *restrict error)
{
  if (obj->supertype == DWG_SUPERTYPE_OBJECT && dwg_obj_is_control (obj))
    {
      return obj->tio.object->xdicobjhandle;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty or invalid table control arg %p, type: 0x%x",
                 __FUNCTION__, obj, obj->type)
      return NULL;
    }
}

/** Get the objid from the generic table control object.
  \code Usage: objid = dwg_object_tablectrl_get_objid(obj, &error);
  \endcode
  \param[in]  obj    a TABLE_CONTROL dwg_object*
  \param[out] error  set to 0 for ok, >0 if not found.
*/
EXPORT BITCODE_BL
dwg_object_tablectrl_get_objid (const dwg_object *restrict obj,
                                int *restrict error)
{
  if (obj->supertype == DWG_SUPERTYPE_OBJECT && dwg_obj_is_control (obj))
    {
      return obj->tio.object->objid;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty or invalid table control arg %p, type: 0x%x",
                 __FUNCTION__, obj, obj->type)
      return 0;
    }
}

/** Returns name of the referenced table entry (as UTF-8). Defaults to ByLayer
    Since r2007 it returns a MALLOC'd copy, before the direct reference to the
    dwg field or the constant "ByLayer".
  \code Usage: char* name = dwg_ref_get_table_name(ref, &error);
  \endcode
  \param[in]  ref     dwg_obj_ref*   A handle
  \param[out] error   int*, is set to 0 for ok, 1 on error
*/
EXPORT char *
dwg_ref_get_table_name (const dwg_object_ref *restrict ref,
                        int *restrict error)
{
  char *name = NULL;
  if (ref->obj)
    name = dwg_obj_table_get_name (ref->obj, error);
  if (!name)
    name = (char *)"ByLayer";
  return name;
}

// TODO: the same for the dwg_tbl_generic obj

/** Get name of the table object entry (utf-8 encoded)
    Since r2007 it returns a MALLOC'd copy, before the direct reference to the
    dwg field.
    Should not be used for a BLOCK, rather use dwg_handle_name() then.
  \code Usage: char* name = dwg_obj_table_get_name(obj, &error);
  \endcode
  \param[in]  obj    a TABLE dwg_object*
  \param[out] error  set to 0 for ok, >0 if not found.
*/
EXPORT char *
dwg_obj_table_get_name (const dwg_object *restrict obj, int *restrict error)
{
  if (obj->supertype == DWG_SUPERTYPE_OBJECT
      && (dwg_obj_is_table (
          obj) /* || obj->fixedtype == DWG_TYPE_DICTIONARY */))
    {
      // HACK: we can guarantee that the table name is always the first field,
      // by using COMMON_TABLE_FLAGS.
      // TODO: Dictionary also?
      const Dwg_Data *dwg = obj->parent;
      Dwg_Object_STYLE *table = obj->tio.object->tio.STYLE;
      *error = 0;
      // importers are still a hack and don't store TU
      if (IS_FROM_TU_DWG (dwg))
        return bit_convert_TU ((BITCODE_TU)table->name); // creates a copy
      else
        return table->name;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty or invalid table arg %p, type: 0x%x", __FUNCTION__,
                 obj, obj->type)
      return NULL;
    }
}

/*******************************************************************
 *                    FUNCTIONS FOR GENERIC ENTITY                  *
 ********************************************************************/

/** Returns the entity layer name (as UTF-8), or "0"
    Since r2007 it returns a MALLOC'd copy, before the direct reference
    to the dwg field or the constant "0".
  \code Usage: char* layer = dwg_ent_get_layer_name(ent, &error);
  \endcode
  \param[in]  ent     dwg_obj_ent*
  \param[out] error   int*, is set to 0 for ok, 1 on error
*/
EXPORT char *
dwg_ent_get_layer_name (const dwg_obj_ent *restrict ent, int *restrict error)
{
  char *name = NULL;
  Dwg_Object *layer = ent->layer ? ent->layer->obj : NULL;

  if (layer)
    name = dwg_obj_table_get_name (layer, error);
  if (!name)
    name = (char *)"0";
  return name;
}

/** Returns the entity linetype name (as UTF-8), or "ByLayer"
    Since r2007 it returns a MALLOC'd copy, before the direct reference
    to the dwg field or the constant "ByLayer".
  \code Usage: char* ltype = dwg_ent_get_ltype_name(ent, &error);
  \endcode
  \param[in]  ent     dwg_obj_ent*
  \param[out] error   int*, is set to 0 for ok, 1 on error
*/
EXPORT char *
dwg_ent_get_ltype_name (const dwg_obj_ent *restrict ent, int *restrict error)
{
  char *name = NULL;
  Dwg_Object *ltype = ent->ltype ? ent->ltype->obj : NULL;

  if (ltype)
    name = dwg_obj_table_get_name (ltype, error);
  if (!name)
    name = (char *)"ByLayer";
  return name;
}

#ifndef __AFL_COMPILER

/** Returns the entity bitsize
  \code Usage: bitsize = dwg_ent_get_bitsize(ent, &error);
  \endcode
  \param[in]  ent     dwg_obj_ent*
  \param[out] error   int*, is set to 0 for ok, 1 on error
*/
EXPORT BITCODE_RL
dwg_ent_get_bitsize (const dwg_obj_ent *restrict ent, int *restrict error)
{
  Dwg_Object *obj = dwg_ent_to_object (ent, error);
  if (!*error)
    {
      return obj->bitsize;
    }
  else
    {
      return 0;
    }
}

/** Returns the number of entity EED structures
See dwg_object_to_entity how to get the ent.
  \code Usage: int num_eed = dwg_ent_get_num_eed(ent,&error);
  \endcode
  \param[in]  ent     dwg_obj_ent*
  \param[out] error   int*, is set to 0 for ok, 1 on error
*/
EXPORT BITCODE_BL
dwg_ent_get_num_eed (const dwg_obj_ent *restrict ent, int *restrict error)
{
  if (!ent)
    {
      *error = 1;
      return 0;
    }
  *error = 0;
  return ent->num_eed;
}
/** Returns the nth EED structure.
  \code Usage: dwg_entity_eed *eed = dwg_ent_get_eed(ent,0,&error);
  \endcode
  \param[in]  ent    dwg_obj_ent*
  \param[in]  idx  [0 - num_eed-1]
  \param[out] error  set to 0 for ok, 1 if ent == NULL or 2 if idx out of
  bounds.
*/
EXPORT dwg_entity_eed *
dwg_ent_get_eed (const dwg_obj_ent *restrict ent, const BITCODE_BL idx,
                 int *restrict error)
{
  if (!ent)
    {
      *error = 1;
      LOG_ERROR ("%s: empty or invalid ent", __FUNCTION__)
      return NULL;
    }
  else if (idx >= ent->num_eed)
    {
      *error = 2;
      return NULL;
    }
  else
    {
      *error = 0;
      return &ent->eed[idx];
    }
}

/** Returns the data union of the nth EED structure.
  \code Usage: dwg_entity_eed_data *eed = dwg_ent_get_eed_data(ent,0,&error);
  \endcode
  \param[in]  ent    dwg_obj_ent*
  \param[in]  idx  [0 - num_eed-1]
  \param[out] error  set to 0 for ok, 1 if ent == NULL or 2 if idx out of
  bounds.
*/
dwg_entity_eed_data *
dwg_ent_get_eed_data (const dwg_obj_ent *restrict ent, const BITCODE_BL idx,
                      int *restrict error)
{
  if (!ent)
    {
      *error = 1;
      LOG_ERROR ("%s: empty or invalid ent", __FUNCTION__)
      return NULL;
    }
  else if (idx >= ent->num_eed)
    {
      *error = 2;
      return NULL;
    }
  else
    {
      *error = 0;
      return ent->eed[idx].data;
    }
}

#endif /* __AFL_COMPILER */

#define _BODY_FIELD(ent, field)                                               \
  *error = 0;                                                                 \
  return ent->field

#ifndef __AFL_COMPILER

EXPORT const Dwg_Color *
dwg_ent_get_color (const dwg_obj_ent *restrict ent, int *restrict error)
{
  if (!ent)
    {
      *error = 1;
      return NULL;
    }
  *error = 0;
  return &(ent->color);
}

EXPORT BITCODE_B
dwg_ent_get_picture_exists (const dwg_obj_ent *restrict ent,
                            int *restrict error)
{
  _BODY_FIELD (ent, preview_exists);
}

EXPORT BITCODE_BLL
dwg_ent_get_picture_size (const dwg_obj_ent *restrict ent,
                          int *restrict error) // before r2007 only RL
{
  _BODY_FIELD (ent, preview_size);
}

EXPORT BITCODE_TF
dwg_ent_get_picture (const dwg_obj_ent *restrict ent, int *restrict error)
{
  _BODY_FIELD (ent, preview);
}

EXPORT BITCODE_BB
dwg_ent_get_entmode (const dwg_obj_ent *restrict ent, int *restrict error)
{
  _BODY_FIELD (ent, entmode);
}

EXPORT BITCODE_BL
dwg_ent_get_num_reactors (const dwg_obj_ent *restrict ent, int *restrict error)
{
  _BODY_FIELD (ent, num_reactors);
}

EXPORT BITCODE_B
dwg_ent_get_is_xdic_missing (const dwg_obj_ent *restrict ent,
                             int *restrict error) // r2004+
{
  _BODY_FIELD (ent, is_xdic_missing);
}

EXPORT BITCODE_B
dwg_ent_get_isbylayerlt (const dwg_obj_ent *restrict ent,
                         int *restrict error) // r13-r14 only
{
  _BODY_FIELD (ent, isbylayerlt);
}

EXPORT BITCODE_B
dwg_ent_get_nolinks (const dwg_obj_ent *restrict ent, int *restrict error)
{
  _BODY_FIELD (ent, nolinks);
}

EXPORT double
dwg_ent_get_linetype_scale (const dwg_obj_ent *restrict ent,
                            int *restrict error)
{
  _BODY_FIELD (ent, ltype_scale);
}

EXPORT BITCODE_BB
dwg_ent_get_linetype_flags (const dwg_obj_ent *restrict ent,
                            int *restrict error) // r2000+
{
  _BODY_FIELD (ent, ltype_flags);
}

EXPORT BITCODE_BB
dwg_ent_get_plotstyle_flags (const dwg_obj_ent *restrict ent,
                             int *restrict error) // r2000+
{
  _BODY_FIELD (ent, plotstyle_flags);
}

EXPORT BITCODE_BB
dwg_ent_get_material_flags (const dwg_obj_ent *restrict ent,
                            int *restrict error) // r2007+
{
  _BODY_FIELD (ent, material_flags);
}

EXPORT BITCODE_RC
dwg_ent_get_shadow_flags (const dwg_obj_ent *restrict ent,
                          int *restrict error) // r2007+
{
  _BODY_FIELD (ent, shadow_flags);
}

EXPORT BITCODE_B
dwg_ent_has_full_visualstyle (dwg_obj_ent *restrict ent,
                              int *restrict error) // r2010+
{
  _BODY_FIELD (ent, has_full_visualstyle);
}

EXPORT BITCODE_B
dwg_ent_has_face_visualstyle (dwg_obj_ent *restrict ent,
                              int *restrict error) // r2010+
{
  _BODY_FIELD (ent, has_face_visualstyle);
}

EXPORT BITCODE_B
dwg_ent_has_edge_visualstyle (dwg_obj_ent *restrict ent,
                              int *restrict error) // r2010+
{
  _BODY_FIELD (ent, has_edge_visualstyle);
}

EXPORT BITCODE_BS
dwg_ent_get_invisible (const dwg_obj_ent *restrict ent, int *restrict error)
{
  _BODY_FIELD (ent, invisible);
}

EXPORT BITCODE_RC
dwg_ent_get_linewt (const dwg_obj_ent *restrict ent,
                    int *restrict error) // r2000+
{
  _BODY_FIELD (ent, linewt);
}

// TODO: dwg_object_ref* or dwg_handle*, not handle
EXPORT dwg_object_ref *
dwg_ent_get_ownerhandle (const dwg_obj_ent *restrict ent, int *restrict error)
{
  _BODY_FIELD (ent, ownerhandle);
}

EXPORT dwg_object_ref **
dwg_ent_get_reactors (const dwg_obj_ent *restrict ent, int *restrict error)
{
  _BODY_FIELD (ent, reactors);
}

EXPORT dwg_object_ref *
dwg_ent_get_xdicobjhandle (const dwg_obj_ent *restrict ent,
                           int *restrict error)
{
  _BODY_FIELD (ent, xdicobjhandle);
}

EXPORT dwg_object_ref *
dwg_ent_get_prev_entity (const dwg_obj_ent *restrict ent,
                         int *restrict error) // r13-r2000
{
  _BODY_FIELD (ent, prev_entity);
}

EXPORT dwg_object_ref *
dwg_ent_get_next_entity (const dwg_obj_ent *restrict ent,
                         int *restrict error) // r13-r2000
{
  _BODY_FIELD (ent, next_entity);
}

EXPORT dwg_object_ref *
dwg_ent_get_layer (const dwg_obj_ent *restrict ent, int *restrict error)
{
  _BODY_FIELD (ent, layer);
}

EXPORT dwg_object_ref *
dwg_ent_get_ltype (const dwg_obj_ent *restrict ent, int *restrict error)
{
  _BODY_FIELD (ent, ltype);
}

EXPORT dwg_object_ref *
dwg_ent_get_material (const dwg_obj_ent *restrict ent,
                      int *restrict error) // r2007+
{
  _BODY_FIELD (ent, material);
}

EXPORT dwg_object_ref *
dwg_ent_get_plotstyle (const dwg_obj_ent *restrict ent,
                       int *restrict error) // r2000+
{
  _BODY_FIELD (ent, plotstyle);
}

EXPORT dwg_object_ref *
dwg_ent_get_full_visualstyle (const dwg_obj_ent *restrict ent,
                              int *restrict error) // r2010+
{
  _BODY_FIELD (ent, full_visualstyle);
}

EXPORT dwg_object_ref *
dwg_ent_get_face_visualstyle (const dwg_obj_ent *restrict ent,
                              int *restrict error) // r2010+
{
  _BODY_FIELD (ent, face_visualstyle);
}

EXPORT dwg_object_ref *
dwg_ent_get_edge_visualstyle (const dwg_obj_ent *restrict ent,
                              int *restrict error) // r2010+
{
  _BODY_FIELD (ent, edge_visualstyle);
}

// accepts utf-8 string
EXPORT int
dwg_ent_set_ltype (dwg_obj_ent *restrict ent, const char *restrict name)
{
  int error = 0;
  // check given handle (to the ltype tablerecord)
  Dwg_Data *dwg = ent->dwg;
  BITCODE_H lt_ref = dwg_find_tablehandle (dwg, name, "LTYPE");
  if (!lt_ref)
    {
      if (!dwg_is_valid_name_u8 (dwg, name))
        {
          LOG_WARN ("Invalid symbol table record name \"%s\"\n", name);
          return 2; // invalid name
        }
      return 1; // not found
    }
  if (!ent->ltype || lt_ref->absolute_ref != ent->ltype->absolute_ref)
    {
      // TODO preR13
      if (!strcasecmp (name, "BYLAYER"))
        {
          ent->isbylayerlt = 1;
          ent->ltype_flags = 0;
        }
      else if (!strcasecmp (name, "BYBLOCK"))
        {
          ent->isbylayerlt = 0;
          ent->ltype_flags = 1;
        }
      else if (!strcasecmp (name, "CONTINUOUS"))
        {
          ent->isbylayerlt = 0;
          ent->ltype_flags = 2;
        }
      else
        {
          ent->isbylayerlt = 0;
          ent->ltype_flags = 3;
          // set ltype tablerecord if not one the 3 builtins
          ent->ltype = dwg_add_handleref (dwg, 5, lt_ref->absolute_ref, NULL);
        }
    }
  return error;
}

#endif /* __AFL_COMPILER */

/** Returns dwg_object* from dwg_obj_ent*
  \code Usage: dwg_object* obj = dwg_obj_ent_to_object(ent, &error);
  \endcode
  \param[in]  obj     dwg_obj_ent*
  \param[out] error   int*, is set to 0 for ok, 1 on error
*/
dwg_object *
dwg_ent_to_object (const dwg_obj_ent *restrict obj, int *restrict error)
{
  dwg_data *dwg;
  dwg_object *retval;
  dwg = obj->dwg;
  if (dwg_version == R_INVALID)
    dwg_version = (Dwg_Version_Type)dwg->header.version;
  if (obj->objid >= dwg->num_objects)
    {
      *error = 1;
      return NULL;
    }
  retval = &dwg->object[obj->objid];
  if (retval->supertype == DWG_SUPERTYPE_ENTITY)
    {
      *error = 0;
      return retval;
    }
  else
    {
      *error = 1;
      return NULL;
    }
}

/** Returns dwg_object* from any dwg_ent_*, the parent of the parent.
  \code Usage: dwg_object* obj = dwg_ent_generic_to_object(_obj, &error);
  \endcode
  \param[in]  _obj     dwg_ent_generic* (line, circle, ...)
  \param[out] error   int*, is set to 0 for ok, 1 on error
*/
dwg_object *
dwg_ent_generic_to_object (const void *restrict _obj, int *restrict error)
{
  return dwg_obj_generic_to_object (_obj, error);
}

Dwg_Data *
dwg_obj_generic_dwg (const void *restrict _obj, int *restrict error)
{
  dwg_obj_generic *o = (dwg_obj_generic *)_obj;
  return (o && o->parent && o->parent->dwg) ? o->parent->dwg : NULL;
}

/** Returns dwg_obj_ent* from any dwg_ent_* entity
  \code Usage: dwg_obj_ent* ent = dwg_ent_generic_parent(_ent, &error);
  \endcode
  \param[in]  ent     dwg_ent_generic* (line, circle, ...)
  \param[out] error   int*, is set to 0 for ok, 1 on error
*/
dwg_obj_ent *
dwg_ent_generic_parent (const void *restrict ent, int *restrict error)
{
  const dwg_ent_generic *xent = (const dwg_ent_generic *)ent;
  if (xent && xent->parent)
    {
      dwg_obj_ent *retval = xent->parent;
      *error = 0;
      return retval;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: Empty or invalid obj", __FUNCTION__)
      return NULL;
    }
}

/** Returns dwg_obj_ent* from dwg object
  \code Usage : dwg_obj_ent* ent = dwg_object_to_entity(obj, &error);
  \endcode
  \param obj   dwg_object*
  \param error
*/
dwg_obj_ent *
dwg_object_to_entity (dwg_object *restrict obj, int *restrict error)
{
  if (obj->supertype == DWG_SUPERTYPE_ENTITY)
    {
      *error = 0;
      if (dwg_version == R_INVALID)
        dwg_version = (Dwg_Version_Type)obj->parent->header.version;
      return obj->tio.entity;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: Empty or invalid obj", __FUNCTION__)
      return NULL;
    }
}

/*******************************************************************
 *                    FUNCTIONS FOR DWG                             *
 ********************************************************************/

/** Returns the number of classes or 0
  \code Usage: unsigned num_classes = dwg_get_num_classes(dwg);
  \endcode
  \param[in]  dwg   dwg_data*
*/
unsigned int
dwg_get_num_classes (const dwg_data *dwg)
{
  if (!dwg)
    return 0;
  if (dwg_version == R_INVALID)
    dwg_version = (Dwg_Version_Type)dwg->header.version;
  return dwg->num_classes;
}

/** Returns the nth class or NULL
  \code Usage: dwg_object* obj = dwg_get_object(dwg, 0);
  \endcode
  \param[in]  dwg   dwg_data*
  \param[in]  idx
*/
dwg_class *
dwg_get_class (const dwg_data *dwg, unsigned int idx)
{
  if (!dwg)
    return NULL;
  if (dwg_version == R_INVALID)
    dwg_version = (Dwg_Version_Type)dwg->header.version;
  return (idx < dwg->num_classes) ? &dwg->dwg_class[idx] : NULL;
}

/** Returns the nth object or NULL
  \code Usage: dwg_object* obj = dwg_get_object(dwg, 0);
  \endcode
  \param[in]  dwg   dwg_data*
  \param[in]  idx
*/
dwg_object *
dwg_get_object (dwg_data *dwg, const BITCODE_BL idx)
{
  if (!dwg)
    return NULL;
  if (dwg_version == R_INVALID)
    dwg_version = (Dwg_Version_Type)dwg->header.version;
  return (idx < dwg->num_objects) ? &dwg->object[idx] : NULL;
}

/** Returns object from absolute reference or NULL
  \code Usage: dwg_object* obj = dwg_absref_get_object(dwg, absref);
  \endcode
  \param[in]  dwg
  \param[in]  absref
*/
dwg_object *
dwg_absref_get_object (const dwg_data *dwg, const BITCODE_BL absref)
{
  if (absref)
    return dwg_resolve_handle (dwg, absref);
  else
    return NULL;
}

/*******************************************************************
 *                    FUNCTIONS FOR DWG OBJECT                       *
 ********************************************************************/

/** Returns the object bitsize or 0
  \code Usage: bitsize = dwg_object_get_bitsize(obj);
  \endcode
  \param[in]  obj   dwg_object*
*/
EXPORT BITCODE_RL
dwg_object_get_bitsize (const dwg_object *obj)
{
  return obj ? obj->bitsize : 0;
}

/** Returns the global idx/objid in the list of all objects.
    This is the same as a dwg_handle absolute_ref value.
    \sa dwg_obj_get_objid

  \code Usage: int idx = dwg_object_get_index(obj, &error);
  \endcode
  \param[in]  obj     dwg_object*
  \param[out] error   int*, is set to 0 for ok, 1 on error
*/
EXPORT BITCODE_BL
dwg_object_get_index (const dwg_object *restrict obj, int *restrict error)
{
  *error = 0;
  if (dwg_version == R_INVALID)
    dwg_version = (Dwg_Version_Type)obj->parent->header.version;
  return obj->index;
}

/** Returns dwg_handle* from dwg_object*
  \code Usage: dwg_handle* handle = dwg_object_get_handle(obj, &error);
  \endcode
  \param[in]  obj     dwg_object*
  \param[out] error   int*, is set to 0 for ok, 1 on error
*/
dwg_handle *
dwg_object_get_handle (dwg_object *restrict obj, int *restrict error)
{
  *error = 0;
  return &(obj->handle);
}

/** Returns the dwg object type, see \ref DWG_OBJECT_TYPE "enum
DWG_OBJECT_TYPE". With types > 500 you need to check the dxfname instead, or
check fixedtype. \sa dwg_object_get_dxfname \sa dwg_get_fixedtype \code Usage:
  int type = dwg_object_get_type(obj);
  if (type > 500) dxfname = dwg_object_get_dxfname(obj);
  \endcode
  \param[in]  obj   dwg_object*
*/
EXPORT int
dwg_object_get_type (const dwg_object *obj)
{
  if (obj)
    {
      return obj->type;
    }
  else
    {
      LOG_ERROR ("%s: empty ref", __FUNCTION__)
      return -1;
    }
}

/** Returns the fixed dwg object type, see \ref DWG_OBJECT_TYPE "enum
DWG_OBJECT_TYPE". \sa dwg_object_get_type \code Usage: int type =
dwg_object_get_type(obj); if (type > 500) dxfname =
dwg_object_get_fixedtype(obj); \endcode \param[in]  obj   dwg_object*
*/
EXPORT int
dwg_object_get_fixedtype (const dwg_object *obj)
{
  if (obj)
    {
      return obj->fixedtype;
    }
  else
    {
      LOG_ERROR ("%s: empty ref", __FUNCTION__)
      return -1;
    }
}

/** Returns the object dxfname as ASCII string. Since r2007 utf8 encoded, but
    we haven't seen unicode names for the dxfname yet.
  \code Usage: const char* name = dwg_object_get_dxfname(obj);
  \endcode
  \param obj dwg_object*
*/
char *
dwg_object_get_dxfname (const dwg_object *obj)
{
  if (obj)
    {
      if (dwg_version == R_INVALID)
        dwg_version = (Dwg_Version_Type)obj->parent->header.version;
      return obj->dxfname;
    }
  else
    {
      LOG_ERROR ("%s: empty ref", __FUNCTION__)
      return NULL;
    }
}

/*******************************************************************
 *                    FUNCTIONS FOR DWG OBJECT SUBCLASSES           *
 ********************************************************************/

/** This is the same as \sa dwg_object_get_index */
EXPORT BITCODE_BL
dwg_obj_get_objid (const dwg_obj_obj *restrict obj, int *restrict error)
{
  _BODY_FIELD (obj, objid);
}

/** Returns the number of object EED structures.
  \code Usage: int num_eed = dwg_obj_get_num_eed(ent, &error);
  \endcode
  \param[in]  obj     dwg_obj_obj*
  \param[out] error   int*, is set to 0 for ok, 1 on error
*/
EXPORT BITCODE_BL
dwg_obj_get_num_eed (const dwg_obj_obj *restrict obj, int *restrict error)
{
  return obj->num_eed;
}
/** Returns the nth EED structure.
  \code Usage: dwg_entity_eed *eed = dwg_obj_get_eed(obj,0,&error);
  \endcode
  \param[in]  obj    dwg_obj_obj*
  \param[in]  idx  [0 - num_eed-1]
  \param[out] error  set to 0 for ok, 1 if obj == NULL or 2 if index out of
bounds.
*/
dwg_entity_eed *
dwg_obj_get_eed (const dwg_obj_obj *restrict obj, const BITCODE_BL idx,
                 int *restrict error)
{
  if (idx >= obj->num_eed)
    {
      *error = 2;
      return NULL;
    }
  else
    {
      *error = 0;
      return &obj->eed[idx];
    }
}

/** Returns the data union of the nth EED structure.
  \code Usage: dwg_entity_eed_data *eed = dwg_obj_get_eed_data(obj,0,&error);
  \endcode
  \param[in]  obj    dwg_obj_obj*
  \param[in]  idx  [0 - num_eed-1]
  \param[out] error  set to 0 for ok, 1 if obj == NULL or 2 if index out of
bounds.
*/
dwg_entity_eed_data *
dwg_obj_get_eed_data (const dwg_obj_obj *restrict obj, const BITCODE_BL idx,
                      int *restrict error)
{
  if (!obj)
    {
      *error = 1;
      LOG_ERROR ("%s: empty or invalid obj", __FUNCTION__)
      return NULL;
    }
  else if (idx >= obj->num_eed)
    {
      *error = 2;
      return NULL;
    }
  else
    {
      *error = 0;
      return obj->eed[idx].data;
    }
}

EXPORT BITCODE_H
dwg_obj_get_ownerhandle (const dwg_obj_obj *restrict obj, int *restrict error)
{
  _BODY_FIELD (obj, ownerhandle);
}
EXPORT BITCODE_BL
dwg_obj_get_num_reactors (const dwg_obj_obj *restrict obj, int *restrict error)
{
  _BODY_FIELD (obj, num_reactors);
}
EXPORT BITCODE_H *
dwg_obj_get_reactors (const dwg_obj_obj *restrict obj, int *restrict error)
{
  _BODY_FIELD (obj, reactors);
}
EXPORT BITCODE_H
dwg_obj_get_xdicobjhandle (const dwg_obj_obj *restrict obj,
                           int *restrict error)
{
  _BODY_FIELD (obj, xdicobjhandle);
}
/* r2004+ */
EXPORT BITCODE_B
dwg_obj_get_is_xdic_missing (const dwg_obj_obj *restrict obj,
                             int *restrict error)
{
  _BODY_FIELD (obj, is_xdic_missing);
}
/* r2013+ */
EXPORT BITCODE_B
dwg_obj_get_has_ds_binary_data (const dwg_obj_obj *restrict obj,
                                int *restrict error)
{
  _BODY_FIELD (obj, has_ds_data);
}
EXPORT Dwg_Handle *
dwg_obj_get_handleref (const dwg_obj_obj *restrict obj, int *restrict error)
{
  _BODY_FIELD (obj, handleref);
}

/** Returns dwg_obj_obj* from dwg_object*
  \code Usage: dwg_obj_obj ent = dwg_object_to_object(obj, &error);
  \endcode
  \param[in]  obj   dwg_object*
  \param[out] error   int*, is set to 0 for ok, 1 on error
*/
EXPORT dwg_obj_obj *
dwg_object_to_object (dwg_object *restrict obj, int *restrict error)
{
  if (obj->supertype == DWG_SUPERTYPE_OBJECT)
    {
      *error = 0;
      if ((dwg_version == R_INVALID) && (obj->parent != NULL))
        dwg_version = (Dwg_Version_Type)obj->parent->header.version;
      return obj->tio.object;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: Empty or invalid obj", __FUNCTION__)
      return NULL;
    }
}

/** Returns dwg_object* from dwg_obj_obj*
  \code Usage: dwg_object* obj = dwg_obj_obj_to_object(_obj, &error);
  \endcode
  \param[in]  obj     dwg_obj_obj*
  \param[out] error   int*, is set to 0 for ok, 1 on error
*/
EXPORT dwg_object *
dwg_obj_obj_to_object (const dwg_obj_obj *restrict obj, int *restrict error)
{
  dwg_data *dwg;
  dwg_object *retval;

  dwg = obj->dwg;
  if (!dwg)
    {
      *error = 1;
      return NULL;
    }
  if (dwg_version == R_INVALID)
    dwg_version = (Dwg_Version_Type)dwg->header.version;
  if (obj->objid >= dwg->num_objects)
    {
      *error = 1;
      return NULL;
    }
  retval = &dwg->object[obj->objid];
  if (retval->supertype == DWG_SUPERTYPE_OBJECT)
    {
      *error = 0;
      return retval;
    }
  else
    {
      *error = 1;
      return NULL;
    }
}

/** Returns dwg_object* from any dwg_obj_*
  \code Usage: dwg_object* obj = dwg_obj_generic_to_object(_obj, &error);
  \endcode
  \param[in]  _vobj   dwg_obj_generic* (layer, block_header, xrecord, ...)
  \param[out] error   int*, is set to 0 for ok, 1 on error
*/
EXPORT dwg_object *
dwg_obj_generic_to_object (const void *restrict _vobj, int *restrict error)
{
  const dwg_obj_generic *_obj = (const dwg_obj_generic *)_vobj;
  if (_obj && _obj->parent)
    {
      dwg_data *dwg = _obj->parent->dwg;
      dwg_object *retval = dwg ? &dwg->object[_obj->parent->objid] : NULL;
      if (!dwg || _obj->parent->objid > dwg->num_objects
          || dwg->header.version > R_AFTER)
        {
          *error = 1;
          LOG_ERROR ("%s: Invalid obj", __FUNCTION__)
          return NULL;
        }
      *error = 0;
      loglevel = dwg->opts & DWG_OPTS_LOGLEVEL;
      if (dwg && dwg_version == R_INVALID)
        dwg_version = (Dwg_Version_Type)dwg->header.version;
      return retval;
    }
  else
    {
      *error = 1;
      // LOG_ERROR("%s: Empty or invalid obj", __FUNCTION__)
      return NULL;
    }
}

/** Returns the handle value for any dwg_obj_*
  \code Usage: handle = dwg_obj_generic_handlevalue(_obj);
  \endcode
  \return The handle value or 0;
  \param[in]  _obj   dwg_obj_generic* (layer, block_header, xrecord, ...) as
void
* to avoid casts.
*/
EXPORT BITCODE_RLL
dwg_obj_generic_handlevalue (void *_obj)
{
  int error;
  Dwg_Object *obj = dwg_obj_generic_to_object (_obj, &error);
  if (!error)
    return obj->handle.value;
  else
    return 0UL;
}

/** Returns dwg_obj_obj* from any dwg_obj_*
  \code Usage: dwg_obj_obj* obj = dwg_obj_generic_parent(_obj, &error);
  \endcode
  \param[in]  _vobj   dwg_obj_generic* (layer, block_header, xrecord, ...)
  \param[out] error   int*, is set to 0 for ok, 1 on error
*/
EXPORT dwg_obj_obj *
dwg_obj_generic_parent (const void *restrict _vobj, int *restrict error)
{
  const dwg_obj_generic *_obj = (const dwg_obj_generic *)_vobj;
  if (_obj && _obj->parent)
    {
      dwg_obj_obj *retval = _obj->parent;
      *error = 0;
      return retval;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: Empty or invalid obj", __FUNCTION__)
      return NULL;
    }
}

/*******************************************************************
 *                    FUNCTIONS FOR DWG OBJECT REF                  *
 ********************************************************************/

/** Returns object from reference or NULL
  \code Usage: dwg_object obj = dwg_ref_get_object(ref, &error);
  \endcode
  \param[in]  ref     dwg_object_ref*
  \param[out] error   int*, is set to 0 for ok, 1 or 2 on error
*/
EXPORT dwg_object *
dwg_ref_get_object (const dwg_object_ref *restrict ref, int *restrict error)
{
  if (ref)
    {
      if (!ref->obj)
        {
          /* It is not possible to get the dwg from the ref only, only from an
             obj. The caller has to call the code below: */
          /* Dwg_Data *dwg = ;
          ref->obj = dwg_resolve_handle (dwg, ref->absolute_ref);
          if (!ref->obj) {
          */
          *error = 2;
          // LOG_ERROR ("%s: empty ref", __FUNCTION__)
          // }
        }
      *error = 0;
      return ref->obj;
    }
  else
    {
      *error = 1;
      LOG_ERROR ("%s: empty ref", __FUNCTION__)
      return NULL;
    }
}

/* Returns the absolute handle reference, to be looked up
   in dwg->object_refs[]
  \code Usage: BITCODE_BL ref = dwg_ref_get_absref(obj, &error);
  \endcode
  \param[in]  ref   dwg_object_ref*
  \param[out] error   int*, is set to 0 for ok, 1 on error
*/
EXPORT BITCODE_BL
dwg_ref_get_absref (const dwg_object_ref *restrict ref, int *restrict error)
{
  return ref->absolute_ref;
}

/* This was previously in encode and out_dxf, but since out_dxf needs it for
   r2013+ 3DSOLIDs and --disable-write is still an option, we need to move it
   here. A global acis_data_idx is needed, since encr_acis_data is split into
   blocks, but acis_data is a single stream, so we need to keep track of the
   current position.
 */
EXPORT char *
dwg_encrypt_SAT1 (BITCODE_BL blocksize, BITCODE_RC *restrict acis_data,
                  int *restrict acis_data_idx)
{
  BITCODE_RC *encr_sat_data = (BITCODE_RC *)CALLOC (blocksize + 1, 1);
  int i;
  for (i = 0; i < (int)blocksize; i++)
    {
      if (acis_data[i] <= 32)
        encr_sat_data[i] = acis_data[i];
      else
        encr_sat_data[i] = 159 - acis_data[i];
    }
  *acis_data_idx = i;
  return (char *)encr_sat_data;
}

/* check for valid symbol table record name.
   names can be up to 255 characters long and can contain letters,
   digits, and the following special characters:
   dollar sign ($), hyphen (-), and underscore (_).
   utf-8 string.
   check if in codepage
*/
EXPORT bool
dwg_is_valid_name_u8 (Dwg_Data *restrict dwg, const char *restrict name)
{
  Dwg_Version_Type version = dwg->header.version;
  const Dwg_Codepage cp = (const Dwg_Codepage)dwg->header.codepage;
  BITCODE_TU wstr;
  size_t wlen;
#ifndef HAVE_NONNULL
  if (!name)
    return false;
#endif
  if (!*name || strlen (name) > 255)
    return false;
  // decode utf-8, check wide-chars
  wstr = bit_utf8_to_TU ((char *)name, 0);
  wlen = bit_wcs2nlen (wstr, 255);
  if (wlen > 255 || !wlen)
    goto fail;
  for (size_t i = 0; i < wlen; i++)
    {
      uint16_t c = wstr[i];
      if (i == 0 && c == '*')
        continue;
      if (version < R_13 &&
#ifdef HAVE_WCTYPE_H
          iswlower (c)
#else
          c < 128 && islower (c)
#endif
      )
        goto fail;
      if (c < 128)
        {
          if (!(c == '$' || c == '_' || c == '-' || isalnum (c)))
            goto fail;
          else
            continue;
        }
      else if (dwg_codepage_isasian (cp))
        {
          // reverse lookup in the cp for the cp index, and then check this
          // for alnum
          uint16_t idx = dwg_codepage_wc (cp, c);
          if (!dwg_codepage_isalnum (cp, idx))
            goto fail;
        }
      else
        {
          uint8_t idx = dwg_codepage_c (cp, c);
          if (!dwg_codepage_isalnum (cp, idx))
            goto fail;
        }
    }
  FREE (wstr);
  return true;
fail:
  FREE (wstr);
  return false;
}

/* check for valid symbol table record name.
   names can be up to 255 characters long and can contain letters,
   digits, and the following special characters:
   dollar sign ($), hyphen (-), and underscore (_).
   native TV or TU string.
   check if each char in codepage.
*/
EXPORT bool
dwg_is_valid_name (Dwg_Data *restrict dwg, const char *restrict name)
{
  Dwg_Version_Type version = dwg->header.version;
  const Dwg_Codepage cp = (const Dwg_Codepage)dwg->header.codepage;
#ifndef HAVE_NONNULL
  if (!name)
    return false;
#endif
  if (dwg->header.from_version < R_2007)
    {
      bool isasian = dwg_codepage_isasian (cp);
      if (!*name || strlen (name) > 255)
        return false;
      if (*name == '*') // valid at the beginning
        name++;

      while (*name)
        {
          uint16_t c = (unsigned char)*name;
          name++;
          if (version < R_13 && c < 128 && islower (c))
            return false;
          if (c > 127 && isasian)
            {
              c = (c << 8) + (unsigned char)*name++;
            }
          if (!(c == '$' || c == '_' || c == '-'
                || dwg_codepage_isalnum (cp, c)))
            return false;
        }
      return true;
    }
  else // TU string
    {
      BITCODE_TU wstr = (BITCODE_TU)name;
      size_t wlen = bit_wcs2nlen (wstr, 255);
      if (wlen > 255 || !wlen)
        return false;
      for (size_t i = 0; i < wlen; i++)
        {
          uint16_t c = wstr[i];
          if (i == 0 && c == '*')
            continue;
          if (version < R_13 &&
#ifdef HAVE_WCTYPE_H
              iswlower (c)
#else
              c < 128 && islower (c)
#endif
          )
            return false;
          // check if char in target codepage
          if (!(c == '$' || c == '_' || c == '-'
                || dwg_codepage_isalnum (cp, c)))
            return false;
        }
    }
  return true;
}

/* utf-8? string without lowercase letters, space or ! */
EXPORT bool
dwg_is_valid_tag (const char *tag)
{
#ifndef HAVE_NONNULL
  if (!tag)
    return false;
#endif
  if (strchr (tag, ' ') || strchr (tag, '!') || strlen (tag) > 256)
    return false;
#ifdef HAVE_WCTYPE_H
  {
    // decode utf-8, check wide-chars (but only in current locale!)
    BITCODE_TU wstr = bit_utf8_to_TU ((char *)tag, 0);
    size_t len = bit_wcs2nlen (wstr, 256);
    if (len > 256 || !len)
      goto fail1;
    for (size_t i = 0; i < len; i++)
      {
        uint16_t c = wstr[i];
        if (iswlower (c)
            || !(c == '$' || c == '_' || c == '-' || iswalnum (c)))
          goto fail1;
      }
    FREE (wstr);
    return true;
  fail1:
    FREE (wstr);
    return false;
  }
#else
  // only ascii support, no wctype nor maxlen checks
  {
    uint8_t c;
    while (c = *tag++)
      {
        if (islower (c) || !(c == '$' || c == '_' || c == '-' || isalnum (c)))
          return false;
      }
  }
  return true;
#endif
}

/********************************************************************
 *                    FUNCTIONS FOR ADDING OBJECTS                  *
 ********************************************************************/

// #ifdef USE_WRITE
#define NEW_OBJECT(dwg, obj)                                                  \
  {                                                                           \
    BITCODE_BL idx = dwg->num_objects;                                        \
    if (dwg_add_object (dwg) < 0)                                             \
      dwg_resolve_objectrefs_silent (dwg);                                    \
    obj = &dwg->object[idx];                                                  \
    dwg->cur_index++;                                                         \
    obj->supertype = DWG_SUPERTYPE_OBJECT;                                    \
    obj->tio.object                                                           \
        = (Dwg_Object_Object *)CALLOC (1, sizeof (Dwg_Object_Object));        \
    obj->tio.object->objid = obj->index;                                      \
    obj->tio.object->dwg = dwg;                                               \
  }

// returns BLOCK_HEADER owner for generic entity from ent->ownerhandle.
EXPORT Dwg_Object_BLOCK_HEADER *
dwg_entity_owner (const void *_ent)
{
  int error;
  Dwg_Object *ent = dwg_ent_generic_to_object (_ent, &error);
  Dwg_Object_Ref *owner;
  Dwg_Object *hdr;
  Dwg_Data *dwg = ent->parent;
  int iter = 0;

  if (error || !ent || ent->supertype != DWG_SUPERTYPE_ENTITY)
    return NULL;
  dwg = ent->parent;
  owner = ent->tio.entity->ownerhandle;
  hdr = dwg_ref_object (dwg, owner);
hdr_again:
  if (!hdr || hdr->fixedtype != DWG_TYPE_BLOCK_HEADER)
    {
      if (iter)
        return NULL;
      if (ent->tio.entity->entmode == 2)
        {
          iter++;
          hdr = dwg_ref_object (dwg, dwg->header_vars.BLOCK_RECORD_MSPACE);
          goto hdr_again;
        }
      else if (ent->tio.entity->entmode == 1)
        {
          iter++;
          hdr = dwg_ref_object (dwg, dwg->header_vars.BLOCK_RECORD_PSPACE);
          goto hdr_again;
        }
      return NULL;
    }
  else
    return hdr->tio.object->tio.BLOCK_HEADER;
}

static void
add_ent_reactor (Dwg_Object_Entity *obj, BITCODE_RLL absolute_ref)
{
  if (obj->num_reactors)
    {
      obj->num_reactors++;
      obj->reactors = (BITCODE_H *)REALLOC (
          obj->reactors, obj->num_reactors * sizeof (BITCODE_H));
    }
  else
    {
      obj->num_reactors = 1;
      obj->reactors = (BITCODE_H *)CALLOC (1, sizeof (BITCODE_H));
    }
  obj->reactors[obj->num_reactors - 1]
      = dwg_add_handleref (obj->dwg, 4, absolute_ref, NULL);
}

static void
add_obj_reactor (Dwg_Object_Object *obj, BITCODE_RLL absolute_ref)
{
  if (obj->num_reactors)
    {
      obj->num_reactors++;
      obj->reactors = (BITCODE_H *)REALLOC (
          obj->reactors, obj->num_reactors * sizeof (BITCODE_H));
    }
  else
    {
      obj->num_reactors = 1;
      obj->reactors = (BITCODE_H *)CALLOC (1, sizeof (BITCODE_H));
    }
  obj->reactors[obj->num_reactors - 1]
      = dwg_add_handleref (obj->dwg, 4, absolute_ref, NULL);
}

// check if radian or degree, need to normalize.
// max observed angle: 10.307697 in some ELLIPSE.end_angle
#define ADD_CHECK_ANGLE(angle)                                                \
  if (isnan (angle))                                                          \
    {                                                                         \
      LOG_ERROR ("Invalid %s: NaN", #angle)                                   \
      return NULL;                                                            \
    }                                                                         \
  if (fabs (angle) > 12.0)                                                    \
    {                                                                         \
      LOG_ERROR ("Invalid %s: %f needs to "                                   \
                 "be radian (%f)",                                            \
                 #angle, angle, deg2rad (angle))                              \
      return NULL;                                                            \
    }                                                                         \
  if (fabs (angle) > M_PI)                                                    \
    {                                                                         \
      const double old = angle;                                               \
      while (angle > M_PI)                                                    \
        angle -= (M_PI * 2.0);                                                \
      while (angle < -M_PI)                                                   \
        angle += (M_PI * 2.0);                                                \
      LOG_WARN ("Bad angle %s: %f normalized "                                \
                "to %f",                                                      \
                #angle, old, angle)                                           \
    }

#define ADD_CHECK_3DPOINT(pt)                                                 \
  if (isnan (pt->x) || isnan (pt->y) || isnan (pt->z))                        \
    {                                                                         \
      LOG_ERROR ("Invalid %s: NaN", #pt)                                      \
      return NULL;                                                            \
    }
#define ADD_CHECK_2DPOINT(pt)                                                 \
  if (isnan (pt->x) || isnan (pt->y))                                         \
    {                                                                         \
      LOG_ERROR ("Invalid %s: NaN", #pt)                                      \
      return NULL;                                                            \
    }
#define ADD_CHECK_DOUBLE(dbl)                                                 \
  if (isnan (dbl))                                                            \
    {                                                                         \
      LOG_ERROR ("Invalid %s: NaN", #dbl)                                     \
      return NULL;                                                            \
    }

/* Convert UTF-8 strings to BITCODE_T fields */
EXPORT __nonnull_all BITCODE_T
dwg_add_u8_input (Dwg_Data *restrict dwg, const char *restrict u8str)
{
  if (IS_FROM_TU_DWG (dwg))
    {
      return (BITCODE_T)bit_utf8_to_TU ((char *restrict)u8str, 0);
    }
  else
    {
      // TODO Encode unicode to \U+... bit_utf8_to_TV. codepage conversions
#if 0
      int size = 1024;
      char *dest = (char *)MALLOC (size);
      char *tgt = bit_utf8_to_TV (dest, u8str, size, strlen(u8str), 0,
                                  dwg->header.codepage);
      if (!dest)
        {
          LOG_ERROR ("%s: Out of memory", __FUNCTION__);
          return NULL;
        }
      while (!tgt)
        {
          size *= 2;
          if (size >= 1>>32)
            {
              LOG_ERROR ("%s: Out of memory", __FUNCTION__);
              return NULL;
            }
          dest = (char*)REALLOC (dest, size);
          tgt = bit_utf8_to_TV (dest, u8str, size, strlen(u8str), 0,
                                dwg->header.codepage);
        }
      return tgt;
#endif
      if (dwg->header.version <= R_12 && strlen (u8str) < 32)
        {
          // those old names are usually 32byte, and bit_write_TF
          // might heap-overflow then.
          char *buf = (char *)MALLOC (33);
          strncpy (buf, u8str, 32);
          buf[32] = '\0';
          return buf;
        }
      else
        return STRDUP (u8str);
    }
}

static void
default_numheader_vars (Dwg_Data *dwg, const Dwg_Version_Type version)
{
  if (version < R_2_0b)
    ;
  else if (version <= R_2_0b)
    dwg->header.numheader_vars = 74;
  else if (version <= R_2_22)
    dwg->header.numheader_vars = 83;
  else if (version <= R_2_4)
    dwg->header.numheader_vars = 101;
  else if (version <= R_2_5)
    dwg->header.numheader_vars = 104;
  // dead code
  else if (version <= R_2_5 && dwg->header.maint_version > 9)
    dwg->header.numheader_vars = 114;
  else if (version <= R_2_6)
    dwg->header.numheader_vars = 120;
  // dead code
  else if (version <= R_2_6 && dwg->header.maint_version > 10)
    dwg->header.numheader_vars = 122;
  else if (version <= R_9)
    dwg->header.numheader_vars = 129;
  else if (version <= R_9c1)
    dwg->header.numheader_vars = 158;
  else if (version <= R_10)
    dwg->header.numheader_vars = 160;
  else if (version < R_11)
    dwg->header.numheader_vars = 204;
  else if (version <= R_13b1)
    dwg->header.numheader_vars = 205;
}

// preR13 --disable-write support
#ifdef USE_WRITE
#  define IN_POSTPROCESS_HANDLES(obj) in_postprocess_handles (obj)
#  define IN_POSTPROCESS_SEQEND(obj, b, c) in_postprocess_SEQEND (obj, b, c)
#  define ENCODE_GET_CLASS(dwg, obj) (void)dwg_encode_get_class (dwg, obj)
/* Now implemented in dxfclasses.c as gperf lookup */
#  define REQUIRE_CLASS(cname)                                                \
    dwg_require_class (dwg, cname, sizeof (cname) - 1)
#else
#  define IN_POSTPROCESS_HANDLES(obj)
#  define IN_POSTPROCESS_SEQEND(obj, b, c)
#  define ENCODE_GET_CLASS(dwg, obj)                                          \
    while (0)                                                                 \
      {                                                                       \
      }
#  define REQUIRE_CLASS(cname)                                                \
    while (0)                                                                 \
      {                                                                       \
      }
#endif

/* The Document API should be similar to the public VBA interface */

#define CMC_DEFAULTS 0, 0, 0, 0, NULL, NULL, NULL, 0, 0, 0

/* The internal driver, which takes an existing dwg */
EXPORT int
dwg_add_Document (Dwg_Data *restrict dwg, const int imperial)
{
  int error;
  Dwg_Object_BLOCK_CONTROL *block_control;
  Dwg_Object_BLOCK_HEADER *mspace, *pspace = NULL;
  Dwg_Object_STYLE *style;
  Dwg_Object_LAYER *layer;
  Dwg_Object_LTYPE *ltype;
  Dwg_Object_LTYPE_CONTROL *ltype_ctrl;
  Dwg_Object_DICTIONARY *nod, *dict, *layoutdict;
  Dwg_Object_MLINESTYLE *mlstyle;
  Dwg_Object_LAYOUT *layout;
  Dwg_Object_VPORT *vport_active;
  Dwg_Entity_VIEWPORT *pviewport;
  dwg_point_3d pt0 = { 0.0, 1.0, 0.0 };
  Dwg_Object *obj, *ctrl, *mspaceobj;
  const char *canonical_media_name;
  struct dwg_versions *dwg_ver_struct;
  Dwg_Version_Type version = dwg->header.version;

  loglevel = dwg->opts & DWG_OPTS_LOGLEVEL;
  /* Set the import flag, so we don't encode to TU, just TV */
  if (version >= R_13b1)
    dwg->opts |= DWG_OPTS_IN;
  dwg->dirty_refs = 0;

  if (!dwg->object_map)
    dwg->object_map = hash_new (200);

  if (version)
    dwg->header.from_version = version;
  else
    version = dwg->header.version = dwg->header.from_version;

  // dwg->header.is_maint = 0xf;
  if (version > R_2_5)
    dwg->header.zero_one_or_three = 1;
  // dwg->header.dwg_version = 0x17; // prefer encode if dwg_version is 0
  dwg_ver_struct = (struct dwg_versions *)dwg_version_struct (version);
  if (!dwg->header.codepage)
    dwg->header.codepage = 30; // FIXME: local OS codepage

  // with decode_r11 we already have proper numheader_vars
  if (!dwg->header.numheader_vars
      || dwg->header.version != dwg->header.from_version)
    {
      dwg->header.dwg_version = dwg_ver_struct->dwg_version;
      // dwg->header.maint_version = 29;
      default_numheader_vars (dwg, version);
    }
  error = dwg_sections_init (dwg);
  if (error) // DWG_ERR_OUTOFMEM or DWG_ERR_INVALIDDWG
    return error;

  dwg->header_vars.unit1_ratio = 412148564080.0; // m to ??
  if (version >= R_13 && version < R_2000)
    {
      dwg->header_vars.unit2_ratio = 6.162483e-14;
      dwg->header_vars.unit3_ratio = 1.62263e+13;
      dwg->header_vars.unit4_ratio = 2.63294e+26;
      if (dwg->header_vars.unit1_name)
        FREE (dwg->header_vars.unit1_name);
      dwg->header_vars.unit1_name = dwg_add_u8_input (dwg, "meter");
      dwg->header_vars.unit2_name = dwg_add_u8_input (dwg, "inch");
      dwg->header_vars.unit3_name = dwg_add_u8_input (dwg, "inch");
      dwg->header_vars.unit4_name = dwg_add_u8_input (dwg, "sq inch");
    }
  else
    {
      dwg->header_vars.unit2_ratio = 1.0;
      dwg->header_vars.unit3_ratio = 1.0;
      dwg->header_vars.unit4_ratio = 1.0;
      if (version >= R_10) // also meter sometimes. unit1_text
        {
          if (dwg->header_vars.unit1_name)
            FREE (dwg->header_vars.unit1_name);
          dwg->header_vars.unit1_name = dwg_add_u8_input (dwg, "m");
        }
    }
  dwg->header_vars.SNAPUNIT = (BITCODE_2RD){ 1.0, 1.0 };
  dwg->header_vars.DIMASO = 1;
  dwg->header_vars.DIMSHO = 0; // Obsolete
  dwg->header_vars.REGENMODE = 1;
  dwg->header_vars.FILLMODE = 1;
  dwg->header_vars.PSLTSCALE = 1;
  dwg->header_vars.BLIPMODE = 1;
  dwg->header_vars.USRTIMER = 1;
  // dwg->header_vars.SKPOLY = 0;
  dwg->header_vars.TILEMODE = 1;
  dwg->header_vars.VISRETAIN = 1;
  dwg->header_vars.ATTREQ = 1;
  dwg->header_vars.MIRRTEXT = 1;
  dwg->header_vars.WORLDVIEW = 1; // since r11
  dwg->header_vars.TILEMODE = 1;
  dwg->header_vars.DELOBJ = 1;
  dwg->header_vars.PROXYGRAPHICS = 1;
  dwg->header_vars.DRAGMODE = 2;
  dwg->header_vars.TREEDEPTH = 3020;
  dwg->header_vars.LUNITS = 2;
  dwg->header_vars.LUPREC = 4;
  dwg->header_vars.ATTMODE = 1;
  dwg->header_vars.COORDS = version >= R_14 ? 1 : 0;
  dwg->header_vars.HANDLING = version > R_10 ? 1 : 0;
  dwg->header_vars.PICKSTYLE = 1;
  dwg->header_vars.SPLINESEGS = 8;
  dwg->header_vars.SURFU = 6;
  dwg->header_vars.SURFV = 6;
  dwg->header_vars.SURFTYPE = 6;
  dwg->header_vars.SURFTAB1 = 6;
  dwg->header_vars.SURFTAB2 = 6;
  dwg->header_vars.SPLINETYPE = 6;
  dwg->header_vars.SHADEDGE = 3;
  dwg->header_vars.SHADEDIF = 70;
  if (version <= R_11)
    dwg->header_vars.MAXACTVP = 16;
  else
    dwg->header_vars.MAXACTVP = 48;
  dwg->header_vars.ISOLINES = 4;
  dwg->header_vars.TEXTQLTY = 50;
  dwg->header_vars.LTSCALE = 1.0;
  dwg->header_vars.TEXTSIZE = 0.2;
  dwg->header_vars.TRACEWID = 0.05;
  dwg->header_vars.SKETCHINC = 0.1;
  dwg->header_vars.FACETRES = 0.5;
  dwg->header_vars.CMLSCALE = imperial ? 1.0 : 20.0;
  dwg->header_vars.CELTSCALE = 1.0;
  dwg->header_vars.INSUNITS = imperial ? 1 : 4;
  if (!dwg->header_vars.MENU)
    dwg->header_vars.MENU = dwg_add_u8_input (dwg, "acad");

  dwg->header_vars.FLAGS = 0x2a1d; // or 0x281d
  dwg->header_vars.CELWEIGHT = -1; // => FLAGS & 0x1f + lweight lookup
  {
    time_t now = time (NULL);
    BITCODE_RLL days = now / 86400L;
    BITCODE_RLL ms = 1000 * (now % 86400L);
    long dzoff = 1000 * tm_offset ();
    ms += dzoff;
    // julian days until the 1970 epoch: 2440587.5
    // (https://planetcalc.com/503/?date=1970-01-01%2000%3A00%3A00)
    days += 2440588L;
    dwg->header_vars.TDUCREATE
        = (BITCODE_TIMEBLL){ (BITCODE_BL)days, (BITCODE_BL)ms,
                             days + (ms * 1e-8) };
  }
  dwg->header_vars.TDUUPDATE = dwg->header_vars.TDUCREATE;
  // CECOLOR.index: 256 [CMC.BS 62]
  dwg->header_vars.CECOLOR = (BITCODE_CMC){ 256, CMC_DEFAULTS }; // ByLayer
  if (version > R_9)
    {
      // HANDSEED: 0.1.49 [H 0] // FIXME needs to be updated on encode
      dwg->header_vars.HANDSEED
          = dwg_add_handleref (dwg, 0, UINT64_C (0x2c), NULL);
    }
  if (version > R_10)
    {
      dwg->header_vars.PEXTMIN
          = (BITCODE_3BD){ 100000000000000000000.0, 100000000000000000000.0,
                           100000000000000000000.0 };
      dwg->header_vars.PEXTMAX
          = (BITCODE_3BD){ -100000000000000000000.0, -100000000000000000000.0,
                           -100000000000000000000.0 };
      // dwg->header_vars.PUCSORG = { 0.0, 0.0, 0.0 };
      dwg->header_vars.PUCSXDIR = (BITCODE_3BD){ 1.0, 0.0, 0.0 };
      dwg->header_vars.PUCSYDIR = (BITCODE_3BD){ 0.0, 1.0, 0.0 };
      // PUCSNAME: (5.0.0) abs:0 [H 2]
    }
  if (version > R_1_4 && version < R_13b1)
    dwg->header_vars.oldCECOLOR_lo = 15;
  if (version < R_10)
    {
      dwg->header_vars.FASTZOOM = 1;
      dwg->header_vars.VPOINTX = (BITCODE_3RD){ 1.0, 0.0, 0.0 };
      dwg->header_vars.VPOINTY = (BITCODE_3RD){ 0.0, 1.0, 0.0 };
      dwg->header_vars.VPOINTZ = (BITCODE_3RD){ 0.0, 0.0, 1.0 };
      dwg->header_vars.VPOINTXALT = (BITCODE_3RD){ 1.0, 0.0, 0.0 };
      dwg->header_vars.VPOINTYALT = (BITCODE_3RD){ 0.0, 1.0, 0.0 };
      dwg->header_vars.VPOINTZALT = (BITCODE_3RD){ 0.0, 0.0, 1.0 };
    }
  dwg->header_vars.VIEWDIR = (BITCODE_3BD){ 0.0, 0.0, 1.0 };
  dwg->header_vars.EXTMIN
      = (BITCODE_3BD){ 100000000000000000000.0, 100000000000000000000.0,
                       100000000000000000000.0 };
  dwg->header_vars.EXTMAX
      = (BITCODE_3BD){ -100000000000000000000.0, -100000000000000000000.0,
                       -100000000000000000000.0 };
  if (imperial)
    {
      dwg->header_vars.PLIMMAX = (BITCODE_2DPOINT){ 12.0, 9.0 };
      dwg->header_vars.LIMMAX = (BITCODE_2DPOINT){ 12.0, 9.0 };
      dwg->header_vars.VIEWCTR = (BITCODE_3RD){ 7.21903, 5.22844, 0 };
      dwg->header_vars.VIEWSIZE = 10.4569;
    }
  else
    {
      dwg->header_vars.PLIMMAX = (BITCODE_2DPOINT){ 420.0, 297.0 };
      dwg->header_vars.LIMMAX = (BITCODE_2DPOINT){ 420.0, 297.0 };
      dwg->header_vars.VIEWCTR = (BITCODE_3RD){ 210.0, 148.5, 0 };
      dwg->header_vars.VIEWSIZE = 228.422194;
    }
  // UCSORG: (0.0, 0.0, 0.0) [3BD 10]
  dwg->header_vars.UCSXDIR = (BITCODE_3BD){ 1.0, 0.0, 0.0 };
  dwg->header_vars.UCSYDIR = (BITCODE_3BD){ 0.0, 1.0, 0.0 };
  // UCSNAME: (5.0.0) abs:0 [H 2]
  dwg->header_vars.DIMTIH = 1;
  dwg->header_vars.DIMTOH = 1;
  dwg->header_vars.DIMALTD = 2;
  dwg->header_vars.DIMTOLJ = 1;
  dwg->header_vars.DIMFIT = 3;
  dwg->header_vars.DIMUNIT = 2;
  dwg->header_vars.DIMDEC = 4;
  dwg->header_vars.DIMTDEC = 4;
  dwg->header_vars.DIMALTU = 2;
  dwg->header_vars.DIMALTTD = 2;
  dwg->header_vars.DIMSCALE = 1.0;
  dwg->header_vars.DIMASZ = 0.18;
  dwg->header_vars.DIMEXO = 0.0625;
  dwg->header_vars.DIMDLI = 0.38;
  dwg->header_vars.DIMEXE = 0.18;
  dwg->header_vars.DIMTXT = 0.18;
  dwg->header_vars.DIMCEN = 0.09;
  dwg->header_vars.DIMALTF = 25.4;
  dwg->header_vars.DIMLFAC = 1.0;
  dwg->header_vars.DIMTFAC = 1.0;
  dwg->header_vars.DIMGAP = 0.09;
  // dwg->header_vars.DIMPOST = dwg_add_u8_input (dwg, "");
  // dwg->header_vars.DIMAPOST = dwg_add_u8_input (dwg, "");
  // dwg->header_vars.DIMBLK_T = dwg_add_u8_input (dwg, "");
  // dwg->header_vars.DIMBLK1_T = dwg_add_u8_input (dwg, "");
  // dwg->header_vars.DIMBLK2_T = dwg_add_u8_input (dwg, "");
  if (version > R_2_21 && version < R_13b1)
    dwg->header_vars.circle_zoom_percent = 100;

  dwg->header_vars.DIMCLRD = (BITCODE_CMC){ 0, CMC_DEFAULTS };
  dwg->header_vars.DIMCLRE = (BITCODE_CMC){ 0, CMC_DEFAULTS };
  dwg->header_vars.DIMCLRT = (BITCODE_CMC){ 0, CMC_DEFAULTS };

  dwg->header_vars.MEASUREMENT = imperial ? 0 : 1;
  canonical_media_name = imperial ? "ANSI_A_(8.50_x_11.00_Inches)"
                                  : "ISO_A1_(841.00_x_594.00_MM)";

  // BLOCK_CONTROL_OBJECT: (3.1.1) abs:1 [H 0]
  dwg_set_next_hdl (dwg, UINT64_C (0x1));
  block_control
      = dwg_add_BLOCK_CONTROL (dwg,
                               0x1F,                          // model space
                               version >= R_13b1 ? 0x20 : 0); // paper space
  // LAYER_CONTROL_OBJECT: (3.1.2) abs:2 [H 0]
  dwg_set_next_hdl (dwg, UINT64_C (0x2));
  dwg_add_LAYER (dwg, NULL);
  // STYLE_CONTROL_OBJECT: (3.1.3) abs:3 [H 0]
  dwg_set_next_hdl (dwg, UINT64_C (0x3));
  dwg_add_STYLE (dwg, NULL);
  // hole at 4
  dwg_set_next_hdl (dwg, UINT64_C (0x5));
  // LTYPE_CONTROL_OBJECT: (3.1.5) abs:5 [H 0]
  dwg_add_LTYPE (dwg, NULL);
  // VIEW_CONTROL_OBJECT: (3.1.6) abs:6 [H 0]
  dwg_set_next_hdl (dwg, UINT64_C (0x6));
  dwg_add_VIEW (dwg, NULL);
  // dwg->view_control = *dwg->object[4].tio.object->tio.VIEW_CONTROL;
  if (version >= R_10)
    {
      // UCS_CONTROL_OBJECT: (3.1.7) abs:7 [H 0]
      dwg_set_next_hdl (dwg, UINT64_C (0x7));
      dwg_add_UCS (dwg, &pt0, NULL, NULL, NULL);
      // dwg->ucs_control = *dwg->object[5].tio.object->tio.UCS_CONTROL;
      // VPORT_CONTROL_OBJECT: (3.1.8) abs:8 [H 0]
      dwg_set_next_hdl (dwg, UINT64_C (0x8));
      dwg_add_VPORT (dwg, NULL);
      // dwg->vport_control = *dwg->object[6].tio.object->tio.VPORT_CONTROL;
      // APPID_CONTROL_OBJECT: (3.1.9) abs:9 [H 0]
      dwg_set_next_hdl (dwg, UINT64_C (0x9));
      dwg_add_APPID (dwg, NULL);
    }
  if (version >= R_11)
    {
      // DIMSTYLE_CONTROL_OBJECT: (3.1.A) abs:A [H 0]
      // We don't create the DIMSTYLE Standard 3.1.1D upfront, only on demand.
      dwg_set_next_hdl (dwg, UINT64_C (0xA));
      dwg_add_DIMSTYLE (dwg, NULL);
    }
  if (version >= R_11 && version <= R_2000)
    {
      // VX_CONTROL_OBJECT: (3.1.B) abs:B [H 0]
      dwg_set_next_hdl (dwg, UINT64_C (0xB));
      dwg_add_VX (dwg, NULL);
    }
  if (version > R_11)
    {
      // DICTIONARY_NAMED_OBJECT: (3.1.C) abs:C [H 0]
      dwg_set_next_hdl (dwg, UINT64_C (0xC));
      nod = dwg_add_DICTIONARY (dwg, NULL, (const BITCODE_T) "NAMED_OBJECT",
                                0UL);
      dwg->header_vars.DICTIONARY_NAMED_OBJECT
          = dwg_add_handleref (dwg, 3, UINT64_C (0xC), NULL);
      // DICTIONARY_ACAD_GROUP: (5.1.D) abs:D [H 0]
      dwg_set_next_hdl (dwg, UINT64_C (0xD));
      dwg_add_DICTIONARY (dwg, (const BITCODE_T) "ACAD_GROUP", NULL,
                          UINT64_C (0));
      dwg->header_vars.DICTIONARY_ACAD_GROUP
          = dwg_add_handleref (dwg, 5, UINT64_C (0xD), NULL);
      dwg_add_DICTIONARY_item (nod, (const BITCODE_T) "ACAD_GROUP",
                               UINT64_C (0xD));
    }
  if (version >= R_2000)
    {
      Dwg_Object_PLACEHOLDER *plh;
      // DICTIONARY (5.1.E) //FIXME
      dwg_set_next_hdl (dwg, UINT64_C (0xE));
      dwg_add_DICTIONARYWDFLT (dwg, (const BITCODE_T) "ACAD_PLOTSTYLENAME",
                               (const BITCODE_T) "Normal", UINT64_C (0xF));
      dwg->header_vars.DICTIONARY_PLOTSTYLENAME
          = dwg_add_handleref (dwg, 5, UINT64_C (0xE), NULL);
      // PLOTSTYLE (2.1.F)
      dwg_set_next_hdl (dwg, UINT64_C (0xF));
      plh = dwg_add_PLACEHOLDER (dwg); // PLOTSTYLE
      obj = dwg_obj_generic_to_object (plh, &error);
      obj->tio.object->ownerhandle
          = dwg_add_handleref (dwg, 4, UINT64_C (0xE), obj);
      add_obj_reactor (obj->tio.object, UINT64_C (0xE));
    }
  //else
  //  {
  //    dwg_set_next_hdl (dwg, UINT64_C (0x10));
  //  }
  if (version >= R_9)
    {
      const char *standard
          = dwg->header.version < R_13 ? "STANDARD" : "Standard";
      // LAYER: (0.1.10)
      dwg_set_next_hdl (dwg, UINT64_C (0x10));
      layer = dwg_add_LAYER (dwg, (const BITCODE_T) "0");
      if (layer)
        {
          layer->color = (BITCODE_CMC){ 7, CMC_DEFAULTS };
          layer->ltype = dwg_add_handleref (dwg, 5, UINT64_C (0x16),
                                            NULL); // Continuous
          layer->plotstyle = dwg_add_handleref (dwg, 5, UINT64_C (0xF), NULL);
          // CLAYER: (5.1.F) abs:F [H 8]
          dwg->header_vars.CLAYER
              = dwg_add_handleref (dwg, 5, UINT64_C (0x10), NULL);
        }
      // ctrl = dwg_get_first_object (dwg, DWG_TYPE_LAYER_CONTROL);
      // if (ctrl)
      //   dwg->layer_control = ctrl->tio.object->tio.LAYER_CONTROL;
      // STYLE: (0.1.11)
      dwg_set_next_hdl (dwg, UINT64_C (0x11));
      style = dwg_add_STYLE (dwg, standard);
      if (style)
        {
          style->font_file = dwg_add_u8_input (dwg, "txt");
          style->last_height = 0.2;
          style->width_factor = 1.0;
        }
      // TEXTSTYLE: (5.1.11) [H 7]
      dwg->header_vars.TEXTSTYLE
          = dwg_add_handleref (dwg, 5, UINT64_C (0x11), NULL);
      dwg->header_vars.DIMTXSTY
          = dwg_add_handleref (dwg, 5, UINT64_C (0x11), NULL);
      // ctrl = dwg_get_first_object (dwg, DWG_TYPE_STYLE_CONTROL);
      // if (ctrl)
      //   dwg->style_control = ctrl->tio.object->tio.STYLE_CONTROL;
      // APPID "ACAD": (0.1.12)
      dwg_set_next_hdl (dwg, UINT64_C (0x12));
      dwg_add_APPID (dwg, "ACAD");
      // hole at 13. already in r13
      dwg_set_next_hdl (dwg, UINT64_C (0x14));
      ctrl = dwg_get_first_object (dwg, DWG_TYPE_LTYPE_CONTROL);
      ltype_ctrl = ctrl->tio.object->tio.LTYPE_CONTROL;
      // LTYPE->byblock: (3.1.14)
      ltype = dwg_add_LTYPE (dwg, "BYBLOCK");
      if (ltype)
        ltype->flag &= ~64; // not loaded
      ltype_ctrl->num_entries--;
      ltype_ctrl->byblock = dwg_add_handleref (dwg, 3, UINT64_C (0x14), NULL);
      dwg->header_vars.LTYPE_BYBLOCK
          = dwg_add_handleref (dwg, 5, UINT64_C (0x14), NULL);
      // LTYPE->bylayer: (3.1.15)
      dwg_set_next_hdl (dwg, UINT64_C (0x15));
      ltype = dwg_add_LTYPE (dwg, "BYLAYER");
      if (ltype)
        ltype->flag &= ~64; // not loaded
      ltype_ctrl->num_entries--;
      ltype_ctrl->bylayer = dwg_add_handleref (dwg, 3, UINT64_C (0x15), NULL);
      dwg->header_vars.LTYPE_BYLAYER
          = dwg_add_handleref (dwg, 5, UINT64_C (0x15), NULL);
      // CELTYPE: (5.1.14) abs:14 [H 6]
      dwg->header_vars.CELTYPE
          = dwg_add_handleref (dwg, 5, UINT64_C (0x15), NULL);
      // LTYPE_CONTINUOUS: (5.1.16)
      dwg_set_next_hdl (dwg, UINT64_C (0x16));
      ltype = dwg_add_LTYPE (dwg, "CONTINUOUS");
      if (ltype)
        ltype->description = dwg_add_u8_input (dwg, "Solid line");
      dwg->header_vars.LTYPE_CONTINUOUS
          = dwg_add_handleref (dwg, 5, UINT64_C (0x16), NULL);
    }

  if (version >= R_13b1)
    {
      // DICTIONARY ACAD_MLINESTYLE: (5.1.17) abs:E [H 0]
      dwg_set_next_hdl (dwg, UINT64_C (0x17));
      dwg_add_DICTIONARY (dwg, "ACAD_MLINESTYLE", "Standard", UINT64_C (0x18));
      dwg->header_vars.DICTIONARY_ACAD_MLINESTYLE
          = dwg_add_handleref (dwg, 5, UINT64_C (0x17), NULL);
      // MLINESTYLE: (0.1.18)
      dwg_set_next_hdl (dwg, UINT64_C (0x18));
      mlstyle = dwg_add_MLINESTYLE (dwg, "Standard");
      if (mlstyle)
        {
          obj = dwg_obj_generic_to_object (mlstyle, &error);
          if (!error)
            dwg->header_vars.CMLSTYLE
                = dwg_add_handleref (dwg, 5, obj->handle.value, NULL);
        }

      // DICTIONARY ACAD_PLOTSETTINGS: (5.1.19)
      dwg_set_next_hdl (dwg, UINT64_C (0x19));
      dwg_add_DICTIONARY (dwg, "ACAD_PLOTSETTINGS", NULL, 0);
      dwg->header_vars.DICTIONARY_PLOTSETTINGS
          = dwg_add_handleref (dwg, 5, UINT64_C (0x19), NULL);
    }
  if (version >= R_2000)
    {
      // DICTIONARY_LAYOUT: (5.1.1A)
      dwg_set_next_hdl (dwg, UINT64_C (0x1A));
      layoutdict = dwg_add_DICTIONARY (dwg, "ACAD_LAYOUT", NULL, 0);
      if (layoutdict)
        {
          obj = dwg_obj_generic_to_object (layoutdict, &error);
          dwg->header_vars.DICTIONARY_LAYOUT
              = dwg_add_handleref (dwg, 5, obj->handle.value, NULL);
        }
    }
  // DIMSTYLE: STANDARD (5.1.1D) abs:1D [H 2] (only if used)

  // hole until 1F
  // BLOCK_RECORD_MSPACE: (5.1.1F)
  dwg_set_next_hdl (dwg, UINT64_C (0x1F));
  mspace = dwg_add_BLOCK_HEADER (dwg, "*MODEL_SPACE");
  mspaceobj = dwg_obj_generic_to_object (mspace, &error);
  if (!mspaceobj)
    return 1;
  block_control->num_entries--;
  dwg->header_vars.BLOCK_RECORD_MSPACE
      = dwg_add_handleref (dwg, 5, mspaceobj->handle.value, NULL);
  dwg->header_vars.BLOCK_RECORD_MSPACE->obj = mspaceobj;
  block_control->model_space
      = dwg_add_handleref (dwg, 3, mspaceobj->handle.value, NULL);
  if (version >= R_13b1)
    {
      // BLOCK_RECORD_PSPACE: (5.1.20)
      dwg_set_next_hdl (dwg, UINT64_C (0x20));
      pspace = dwg_add_BLOCK_HEADER (dwg, "*PAPER_SPACE");
      obj = dwg_obj_generic_to_object (pspace, &error);
      block_control->num_entries--;
      if (!block_control->num_entries)
        {
          FREE (block_control->entries);
          block_control->entries = NULL;
        }
      dwg->header_vars.BLOCK_RECORD_PSPACE
          = dwg_add_handleref (dwg, 5, obj->handle.value, NULL);
      dwg->header_vars.BLOCK_RECORD_PSPACE->obj = obj;
      block_control->paper_space
          = dwg_add_handleref (dwg, 3, obj->handle.value, NULL);
      dwg->block_control = *block_control;
      // BLOCK: (5.1.21)
      dwg_set_next_hdl (dwg, UINT64_C (0x21));
      dwg_add_BLOCK (pspace, "*PAPER_SPACE");
      // ENDBLK: (5.1.22)
      dwg_set_next_hdl (dwg, UINT64_C (0x22));
      dwg_add_ENDBLK (pspace);
    }
  // LAYOUT (0.1.23)
  // dwg_set_next_hdl (dwg, UINT64_C (0x23));
  // layout = dwg_add_LAYOUT (layoutdict);
  // pspace->layout = dwg_add_handleref (dwg, 5, UINT64_C (0x23), NULL);

  dwg_set_next_hdl (dwg, UINT64_C (0x24));
  {
    // BLOCK: (5.1.24)
    Dwg_Entity_BLOCK *block = dwg_add_BLOCK (mspace, "*MODEL_SPACE");
    if (dwg->header.version <= R_12) // fixup the type
      {
        obj = dwg_obj_generic_to_object (block, &error);
        obj->type = DWG_TYPE_UNUSED_r11; // don't encode it
      }
  }
  dwg_set_next_hdl (dwg, UINT64_C (0x25));
  {
    // ENDBLK: (5.1.25)
    Dwg_Entity_ENDBLK *endblk = dwg_add_ENDBLK (mspace);
    if (dwg->header.version <= R_12) // fixup the type
      {
        obj = dwg_obj_generic_to_object (endblk, &error);
        obj->type = DWG_TYPE_UNUSED_r11; // don't encode it
      }
  }
  if (dwg->header.version >= R_10)
    {
      // VPORT (0.1.26)
      dwg_set_next_hdl (dwg, UINT64_C (0x26));
      vport_active = dwg_add_VPORT (
          dwg, dwg->header.version >= R_13b1 ? "*Active" : "*ACTIVE");
    }
  if (dwg->header.version >= R_2000)
    {
      dwg_set_next_hdl (dwg, UINT64_C (0x27));
#ifdef NEED_VPORT_FOR_MODEL_LAYOUT
      // LAYOUT (0.1.27)
      obj = dwg_obj_generic_to_object (vport_active, &error);
      layout = dwg_add_LAYOUT (obj, "Model", canonical_media_name);
#else
      layout = dwg_add_LAYOUT (mspaceobj, "Model", canonical_media_name);
#endif
      obj = dwg_obj_generic_to_object (layout, &error);
      mspace->layout = dwg_add_handleref (dwg, 5, obj->handle.value, NULL);

#ifdef NEED_VPORT_FOR_MODEL_LAYOUT
      // VIEWPORT (0.1.28)
      dwg_set_next_hdl (dwg, UINT64_C (0x28));
      pviewport = dwg_add_VIEWPORT (pspace, "");
      obj = dwg_obj_generic_to_object (pviewport, &error);
#else
      obj = dwg_obj_generic_to_object (pspace, &error);
#endif
      // LAYOUT (0.1.29)
      dwg_set_next_hdl (dwg, UINT64_C (0x29));
      layout = dwg_add_LAYOUT (obj, "Layout1", canonical_media_name);
      obj = dwg_obj_generic_to_object (layout, &error);
      pspace->layout = dwg_add_handleref (dwg, 5, obj->handle.value, NULL);
    }

  // a non-invasive variant of resolve_objectref_vector()
  for (unsigned i = 0; i < dwg->num_object_refs; i++)
    {
      Dwg_Object_Ref *ref = dwg->object_ref[i];
      // possibly update the obj if realloced
      if ((obj = dwg_resolve_handle (dwg, ref->absolute_ref)))
        ref->obj = obj;
    }
  // dwg->dirty_refs = 0;
  return 0;
}

/* Initialize a new dwg. Which template, imperial or metric */
EXPORT Dwg_Data *
dwg_new_Document (const Dwg_Version_Type version, const int imperial,
                  const int log_level)
{
  Dwg_Data *dwg = (Dwg_Data *)CALLOC (1, sizeof (Dwg_Data));
  dwg->header.version = version;
  dwg->opts = log_level;

  (void)dwg_add_Document (dwg, imperial); // ignores errors
  return dwg;
}

/* Returns <0 on error, the new 50x klass id on success..
   -1 out of memory.
   Takes UTF-8 names only */
EXPORT int
dwg_add_class (Dwg_Data *restrict dwg, const char *const restrict dxfname,
               const char *const restrict cppname,
               const char *const restrict appname, const bool is_entity)
{
  /* calc. new number, no proxy, no is_zombie */
  BITCODE_BS i = dwg->num_classes;
  Dwg_Class *klass;
  if (i == 0)
    dwg->dwg_class = (Dwg_Class *)MALLOC (sizeof (Dwg_Class));
  else
    dwg->dwg_class
        = (Dwg_Class *)REALLOC (dwg->dwg_class, (i + 1) * sizeof (Dwg_Class));
  if (!dwg->dwg_class)
    {
      LOG_ERROR ("%s: Out of memory", __FUNCTION__);
      return -1;
    }
  klass = &dwg->dwg_class[i];
  memset (klass, 0, sizeof (Dwg_Class));
  klass->number = i + 500;
  klass->dxfname = STRDUP (dxfname);
  if (dwg->header.version >= R_2007)
    klass->dxfname_u = bit_utf8_to_TU ((char *restrict)dxfname, 0);
  klass->appname = dwg_add_u8_input (dwg, appname);
  klass->cppname = dwg_add_u8_input (dwg, cppname);
  klass->item_class_id = is_entity ? 0x1f2 : 0x1f3;
  dwg->num_classes++;
  return klass->number;
}

#define NEW_ENTITY(dwg, obj)                                                  \
  {                                                                           \
    BITCODE_BL idx = dwg->num_objects;                                        \
    if (dwg_add_object (dwg) < 0)                                             \
      {                                                                       \
        dwg_resolve_objectrefs_silent (dwg);                                  \
        blkobj = dwg_obj_generic_to_object (blkhdr, &error);                  \
      }                                                                       \
    obj = &dwg->object[idx];                                                  \
    dwg->cur_index++;                                                         \
    obj->supertype = DWG_SUPERTYPE_ENTITY;                                    \
    obj->tio.entity                                                           \
        = (Dwg_Object_Entity *)CALLOC (1, sizeof (Dwg_Object_Entity));        \
    obj->tio.entity->objid = obj->index;                                      \
    obj->tio.entity->dwg = dwg;                                               \
  }

/* globals: dwg, obj, _obj, dxfname */
#define ADD_ENTITY(token)                                                     \
  obj->fixedtype = DWG_TYPE_##token;                                          \
  if (dwg->header.version > R_11)                                             \
    obj->type = DWG_TYPE_##token;                                             \
  obj->dxfname = (char *)dwg_type_dxfname (DWG_TYPE_##token);                 \
  if (memBEGINc (#token, "_3D"))                                              \
    obj->name = (char *)&#token[1];                                           \
  else                                                                        \
    obj->name = (char *)#token;                                               \
  if (!obj->dxfname)                                                          \
    {                                                                         \
      LOG_TRACE ("Unknown dxfname for %s\n", obj->name)                       \
      obj->dxfname = obj->name;                                               \
    }                                                                         \
  if (dwg->opts & DWG_OPTS_IN)                                                \
    obj->dxfname = STRDUP (obj->dxfname);                                     \
  if (dwg->opts & DWG_OPTS_INJSON)                                            \
    obj->name = STRDUP (obj->name);                                           \
  if (obj->type >= DWG_TYPE_GROUP)                                            \
    ENCODE_GET_CLASS (obj->parent, obj);                                      \
  LOG_TRACE ("  ADD_ENTITY %s [%d]\n", obj->name, obj->index)                 \
  _obj = (Dwg_Entity_##token *)CALLOC (1, sizeof (Dwg_Entity_##token));       \
  obj->tio.entity->tio.token = (Dwg_Entity_##token *)_obj;                    \
  obj->tio.entity->tio.token->parent = obj->tio.entity;                       \
  obj->tio.entity->objid = obj->index;                                        \
  dwg_add_entity_defaults (dwg, obj->tio.entity);                             \
  if (strEQc (#token, "SEQEND") || memBEGINc (#token, "VERTEX"))              \
  obj->tio.entity->linewt = 0x1c

/* globals: dxfname, blkhdr=owner */
#define API_ADD_ENTITY(token)                                                 \
  int error;                                                                  \
  Dwg_Object *obj;                                                            \
  Dwg_Entity_##token *_obj;                                                   \
  Dwg_Object *blkobj = dwg_obj_generic_to_object (blkhdr, &error);            \
  Dwg_Data *dwg = blkobj && !error ? blkobj->parent : NULL;                   \
  if (!dwg || !blkobj                                                         \
      || !(blkobj->fixedtype == DWG_TYPE_BLOCK_HEADER                         \
           || dwg_obj_has_subentity (blkobj)))                                \
    {                                                                         \
      LOG_ERROR ("Entity %s can not be added to %s", #token,                  \
                 blkobj ? dwg_type_name (blkobj->fixedtype) : "NULL");        \
      return NULL;                                                            \
    }                                                                         \
  NEW_ENTITY (dwg, obj);                                                      \
  ADD_ENTITY (token);                                                         \
  obj->tio.entity->ownerhandle                                                \
      = dwg_add_handleref (dwg, 5, blkobj->handle.value, obj);                \
  dwg_set_next_objhandle (obj);                                               \
  LOG_TRACE ("  handle " FORMAT_H "\n", ARGS_H (obj->handle));                \
  IN_POSTPROCESS_HANDLES (obj);                                               \
  if (dwg->header.version < R_10)                                             \
    dwg->header_vars.numentities++;                                           \
  dwg_insert_entity ((Dwg_Object_BLOCK_HEADER *)blkhdr, obj)

/* split into 2 to add controls before */
#define API_ADD_PREP(token)                                                   \
  int error;                                                                  \
  Dwg_Object *obj;                                                            \
  Dwg_Entity_##token *_obj;                                                   \
  Dwg_Object *blkobj = dwg_obj_generic_to_object (blkhdr, &error);            \
  Dwg_Data *dwg = blkobj && !error ? blkobj->parent : NULL;                   \
  if (!dwg || !blkobj                                                         \
      || !(blkobj->fixedtype == DWG_TYPE_BLOCK_HEADER                         \
           || dwg_obj_has_subentity (blkobj)))                                \
    {                                                                         \
      LOG_ERROR ("Entity %s can not be added to %s", #token,                  \
                 blkobj ? dwg_type_name (blkobj->fixedtype) : "NULL");        \
      return NULL;                                                            \
    }

#define API_ADD_ENTITY2(token)                                                \
  NEW_ENTITY (dwg, obj);                                                      \
  ADD_ENTITY (token);                                                         \
  obj->tio.entity->ownerhandle                                                \
      = dwg_add_handleref (dwg, 5, blkobj->handle.value, obj);                \
  dwg_set_next_objhandle (obj);                                               \
  LOG_TRACE ("  handle " FORMAT_H "\n", ARGS_H (obj->handle));                \
  IN_POSTPROCESS_HANDLES (obj);                                               \
  if (dwg->header.version < R_10)                                             \
    dwg->header_vars.numentities++;                                           \
  dwg_insert_entity ((Dwg_Object_BLOCK_HEADER *)blkhdr, obj)

#define API_UNADD_ENTITY                                                      \
  if (dwg->header.version < R_13b1)                                           \
    obj->type = -(long)(obj->type);                                           \
  else                                                                        \
    obj->type = DWG_TYPE_UNUSED

#define ADD_OBJECT(token)                                                     \
  obj->type = obj->fixedtype = DWG_TYPE_##token;                              \
  obj->name = (char *)#token;                                                 \
  obj->dxfname = (char *)dwg_type_dxfname (DWG_TYPE_##token);                 \
  if (!obj->dxfname)                                                          \
    {                                                                         \
      LOG_TRACE ("Unknown dxfname for %s\n", obj->name)                       \
      obj->dxfname = obj->name;                                               \
    }                                                                         \
  if (dwg->opts & DWG_OPTS_IN)                                                \
    obj->dxfname = STRDUP (obj->dxfname);                                     \
  if (dwg->opts & DWG_OPTS_INJSON)                                            \
    obj->name = STRDUP (obj->name);                                           \
  if (obj->type >= DWG_TYPE_GROUP)                                            \
    ENCODE_GET_CLASS (obj->parent, obj);                                      \
  LOG_TRACE ("  ADD_OBJECT %s [%d]\n", obj->name, obj->index)                 \
  _obj = (Dwg_Object_##token *)CALLOC (1, sizeof (Dwg_Object_##token));       \
  obj->tio.object->tio.token = (Dwg_Object_##token *)_obj;                    \
  obj->tio.object->tio.token->parent = obj->tio.object;                       \
  obj->tio.object->objid = obj->index

/* globals: dwg */
#define API_ADD_OBJECT(token)                                                 \
  int error;                                                                  \
  Dwg_Object *obj;                                                            \
  Dwg_Object_##token *_obj;                                                   \
  NEW_OBJECT (dwg, obj);                                                      \
  ADD_OBJECT (token);                                                         \
  dwg_set_next_objhandle (obj);                                               \
  LOG_TRACE ("  handle " FORMAT_H "\n", ARGS_H (obj->handle));                \
  IN_POSTPROCESS_HANDLES (obj)

EXPORT int
dwg_add_entity_defaults (Dwg_Data *restrict dwg,
                         Dwg_Object_Entity *restrict ent)
{
  int error;
  Dwg_Object *obj = dwg_ent_to_object (ent, &error);
  // const Dwg_Version_Type version = dwg->header.version;
  ent->is_xdic_missing = 1;
  ent->color.index = 256; /* ByLayer */
  // ltype either Bylayer or Continuous. if it has an ltype field, assume Cont
  if (!error && !dwg_dynapi_entity_field (obj->name, "ltype"))
    ent->isbylayerlt = 1;
  ent->ltype_scale = 1.0;
  // ltype_flags = 0; ByLayer
  // plotstyle_flags  = 0; ByLayer
  if (!error
      & (strEQc (obj->name, "SEQEND") || memBEGINc (obj->name, "VERTEX")))
    ent->linewt = 0x1c;
  else
    ent->linewt = 0x1d;
  if (dwg->header_vars.CLAYER)
    ent->layer = dwg_add_handleref (
        dwg, 5, dwg->header_vars.CLAYER->absolute_ref, NULL);
  // we cannot yet write >r2000, so no material, visualstyle, yet ...

  if (dwg->header_vars.THICKNESS != 0.0 && !error
      && dwg_dynapi_entity_field (obj->name, "thickness"))
    {
      BITCODE_BD thickness = dwg->header_vars.THICKNESS;
      dwg_dynapi_entity_set_value (obj, obj->name, "thickness", &thickness, 0);
    }
  if (!error && dwg_dynapi_entity_field (obj->name, "extrusion"))
    {
      BITCODE_BE extrusion = (BITCODE_BE){ 0.0, 0.0, 1.0 };
      dwg_dynapi_entity_set_value (ent->tio.POINT, obj->name, "extrusion",
                                   &extrusion, 0);
    }
  return 0;
}

/* Insert new entity into the block.
   We have two entity chains, one in the BLOCK_HEADER (not relative, obj always
   NULL) and the 2nd in the entities itself, these are always relative, so
   different ref. _owner may be BLOCK_HEADER or POLYLINE_*
 */
EXPORT int
dwg_insert_entity (Dwg_Object_BLOCK_HEADER *restrict _owner,
                   Dwg_Object *restrict obj)
{
  int error;
  Dwg_Data *dwg = obj->parent;
  const Dwg_Version_Type version = dwg->header.version;
  Dwg_Object *owner = dwg_obj_generic_to_object (_owner, &error);
  const Dwg_Object_Ref *mspace = dwg_model_space_ref (dwg);
  const Dwg_Object_Ref *pspace = dwg_paper_space_ref (dwg);
  Dwg_Object_Entity *ent = obj->tio.entity;

  assert (mspace);
  // set entmode and ownerhandle
  if (owner->fixedtype == DWG_TYPE_BLOCK_HEADER
      && owner->handle.value == mspace->absolute_ref)
    ent->entmode = 2;
  else if (owner->fixedtype == DWG_TYPE_BLOCK_HEADER && pspace
           && owner->handle.value == pspace->absolute_ref)
    ent->entmode = 1;
  else
    obj->tio.entity->ownerhandle
        = dwg_add_handleref (dwg, 4, owner->handle.value, obj);
  if (!obj->tio.entity->ownerhandle
      && (obj->fixedtype == DWG_TYPE_BLOCK || obj->fixedtype == DWG_TYPE_ENDBLK
          || obj->fixedtype == DWG_TYPE_SEQEND))
    // BLOCK,ENDBLK,SEQEND always have the ownerhandle set.
    ent->ownerhandle = dwg_add_handleref (dwg, 4, owner->handle.value, obj);

  // TODO 2004+
  if (1)
    {
      if (owner->fixedtype == DWG_TYPE_BLOCK_HEADER
          && obj->fixedtype == DWG_TYPE_BLOCK)
        {
          _owner->block_entity
              = dwg_add_handleref (dwg, 3, obj->handle.value, NULL);
          LOG_TRACE ("%s.block_entity = " FORMAT_REF "\n", owner->name,
                     ARGS_REF (_owner->block_entity));
        }
      else if (owner->fixedtype == DWG_TYPE_BLOCK_HEADER
               && obj->fixedtype == DWG_TYPE_ENDBLK)
        {
          _owner->endblk_entity
              = dwg_add_handleref (dwg, 3, obj->handle.value, NULL);
          LOG_TRACE ("%s.endblk_entity = " FORMAT_REF "\n", owner->name,
                     ARGS_REF (_owner->endblk_entity));
        }
      else if (obj->fixedtype == DWG_TYPE_SEQEND)
        {
          ent->prev_entity = ent->next_entity
              = dwg_add_handleref (dwg, 4, 0, NULL);
        }
      else if (owner->fixedtype == DWG_TYPE_BLOCK_HEADER
               && !_owner->first_entity && !_owner->num_owned
               && !dwg_obj_is_subentity (obj))
        {
          BITCODE_H ref;
          _owner->first_entity = _owner->last_entity
              = dwg_add_handleref (dwg, 4, obj->handle.value, NULL);
          LOG_TRACE ("%s.{first,last}_entity = " FORMAT_REF "\n", owner->name,
                     ARGS_REF (_owner->first_entity));
          ref = dwg_add_handleref (dwg, 3, obj->handle.value, NULL);
          LOG_TRACE ("%s.entities[%d] = " FORMAT_REF "\n", owner->name,
                     _owner->num_owned, ARGS_REF (ref));
          PUSH_HV (_owner, num_owned, entities, ref)
          // ent->nolinks = 1;
          LOG_TRACE ("%s.num_owned = %u\n", owner->name, _owner->num_owned);
        }
      else
        {
          Dwg_Object *prev = NULL;
          if (dwg_obj_is_subentity (obj) && obj->index)
            {
              prev = &dwg->object[obj->index - 1];
              if (!dwg_obj_is_subentity (prev))
                prev = NULL;
            }
          else if (owner->fixedtype == DWG_TYPE_BLOCK_HEADER)
            {
              BITCODE_H ref = _owner->last_entity;
              prev = ref ? dwg_ref_object (dwg, ref) : NULL; // may fail!
              _owner->last_entity
                  = dwg_add_handleref (dwg, 4, obj->handle.value, NULL);
              LOG_TRACE ("%s.last_entity = " FORMAT_REF "\n", owner->name,
                         ARGS_REF (_owner->last_entity));
              ref = dwg_add_handleref (dwg, 3, obj->handle.value, NULL);
              LOG_TRACE ("%s.entities[%d] = " FORMAT_REF "\n", owner->name,
                         _owner->num_owned, ARGS_REF (ref));
              PUSH_HV (_owner, num_owned, entities, ref)
              LOG_TRACE ("%s.num_owned = %u\n", owner->name,
                         _owner->num_owned);
            }
          // link prev. last to curr last
          if (prev && prev->supertype == DWG_SUPERTYPE_ENTITY)
            {
              if (prev->index + 1 == obj->index && // immediate next
                  prev->tio.entity->prev_entity && // and pref exist
                  prev->tio.entity->prev_entity->absolute_ref)
                {
                  // is next and is not first
                  prev->tio.entity->nolinks = 1;
                }
              else
                {
                  prev->tio.entity->next_entity
                      = dwg_add_handleref (dwg, 4, obj->handle.value, prev);
                  LOG_TRACE ("prev.next_entity = " FORMAT_REF "\n",
                             ARGS_REF (prev->tio.entity->next_entity));
                  ent->prev_entity
                      = dwg_add_handleref (dwg, 4, prev->handle.value, obj);
                  LOG_TRACE ("%s.prev_entity = " FORMAT_REF "\n", obj->name,
                             ARGS_REF (ent->prev_entity));
                  prev->tio.entity->nolinks = 0;
                }
            }
          else
            ent->prev_entity = dwg_add_handleref (dwg, 4, 0, NULL);
        }
    }
  IN_POSTPROCESS_HANDLES (obj);
  dwg->prev_entity_index = obj->index;
  return 0;
}

/* -- For now only the entities needed for SolveSpace -- */

EXPORT Dwg_Entity_TEXT *
dwg_add_TEXT (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
              const char *restrict text_value,
              const dwg_point_3d *restrict ins_pt, const double height)
{
  API_ADD_ENTITY (TEXT);
  ADD_CHECK_3DPOINT (ins_pt);
  ADD_CHECK_DOUBLE (height);
  _obj->text_value = dwg_add_u8_input (dwg, text_value);
  _obj->ins_pt.x = ins_pt->x;
  _obj->ins_pt.y = ins_pt->y;
  _obj->elevation = ins_pt->z;
  _obj->height = height;
  if (dwg->header_vars.TEXTSTYLE)
    _obj->style = dwg_add_handleref (
        dwg, 5, dwg->header_vars.TEXTSTYLE->absolute_ref, NULL);
  if (dwg->header.version <= R_12)
    {
      obj->type = DWG_TYPE_TEXT_r11;
      if (_obj->elevation == 0.0)
        obj->tio.entity->flag_r11 |= FLAG_R11_HAS_ELEVATION;
    }
  return _obj;
}

static Dwg_Entity_INSERT *
add_attrib_links (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                  Dwg_Entity_INSERT *restrict insert,
                  Dwg_Object *restrict insobj, Dwg_Object *restrict attobj)
{
  Dwg_Object *seqend = dwg_ref_object (insobj->parent, insert->seqend);
  // prev attrib
  Dwg_Object *lastobj = dwg_ref_object (insobj->parent, insert->last_attrib);
  if (!insert->has_attribs || !seqend || !lastobj) // no ATTRIB and SEQEND yet
    {
      API_ADD_ENTITY (SEQEND);
      if (dwg->header.version <= R_12)
        obj->type = DWG_TYPE_SEQEND_r11;
      insert->has_attribs = 1;
      insert->num_owned = 1;
      obj->tio.entity->entmode = 0;
      obj->tio.entity->nolinks = 0;
      obj->tio.entity->ownerhandle
          = dwg_add_handleref (dwg, 4, insobj->handle.value, insobj);
      insert->first_attrib
          = dwg_add_handleref (dwg, 4, attobj->handle.value, insobj);
      insert->last_attrib = insert->first_attrib;
      insert->seqend = dwg_add_handleref (dwg, 3, obj->handle.value, insobj);
      insert->attribs = (BITCODE_H *)MALLOC (sizeof (BITCODE_H));
      insert->attribs[0] = insert->last_attrib;
      IN_POSTPROCESS_SEQEND (obj, insert->num_owned, insert->attribs);
    }
  else
    {
      Dwg_Data *dwg = insobj->parent;
      insert->last_attrib
          = dwg_add_handleref (dwg, 4, attobj->handle.value, insobj);
      attobj->tio.entity->prev_entity = insert->last_attrib;
      insert->num_owned++;
      insert->attribs = (BITCODE_H *)REALLOC (
          insert->attribs, insert->num_owned * sizeof (BITCODE_H));
      lastobj->tio.entity->next_entity
          = dwg_add_handleref (dwg, 3, attobj->handle.value, insobj);
      insert->attribs[insert->num_owned - 1] = insert->last_attrib;
      seqend = dwg_ref_object (dwg, insert->seqend);
      IN_POSTPROCESS_SEQEND (seqend, insert->num_owned, insert->attribs);
    }
  return insert;
}

/* This adds the ATTRIB and SEQEND to the insert,
   and the ATTDEF and ENDBLK to the block. */
EXPORT Dwg_Entity_ATTRIB *
dwg_add_Attribute (Dwg_Entity_INSERT *restrict insert, const double height,
                   const int flags, const char *restrict prompt,
                   const dwg_point_3d *restrict ins_pt,
                   const char *restrict tag, const char *restrict text_value)
{
  Dwg_Object *hdr, *attobj, *insobj;
  Dwg_Object_BLOCK_HEADER *restrict blkhdr;
  Dwg_Entity_ENDBLK *endblk;
  Dwg_Entity_ATTDEF *attdef;
  Dwg_Entity_ATTRIB *attrib;
  int err;

#ifndef HAVE_NONNULL
  if (!insert)
    {
      LOG_ERROR ("add_Attribute: Missing insert");
      return NULL;
    }
#endif
  if (!dwg_is_valid_tag (tag))
    {
      LOG_ERROR ("add_Attribute: Invalid tag %s", tag);
      return NULL;
    }
  ADD_CHECK_3DPOINT (ins_pt);
  ADD_CHECK_DOUBLE (height);
  insobj = dwg_obj_generic_to_object (insert, &err);
  if (!insobj || err)
    {
      LOG_ERROR ("add_Attribute: No INSERT found");
      return NULL;
    }
  hdr = dwg_ref_object (insobj->parent, insert->block_header);
  if (!hdr)
    {
      LOG_ERROR ("add_Attribute: No INSERT.block_header found");
      return NULL;
    }
  blkhdr = hdr->tio.object->tio.BLOCK_HEADER;

  // TODO check if this ATTDEF already exists.
  attdef = dwg_add_ATTDEF (blkhdr, height, flags, prompt, ins_pt, tag,
                           text_value);
  if (!attdef)
    LOG_WARN ("No ATTDEF %s added", tag)
  // ENDBLK must exist already though
  attrib = dwg_add_ATTRIB (insert, height, flags, ins_pt, tag, text_value);
  attobj = dwg_obj_generic_to_object (attrib, &err);
  if (!attobj || err)
    {
      LOG_ERROR ("No ATTRIB %s added", tag);
      return NULL;
    }
  // dwg->header_vars.AFLAGS = flags; FIXME
  add_attrib_links (blkhdr, insert, insobj, attobj);
  return attrib;
}

EXPORT Dwg_Entity_ATTRIB *
dwg_add_ATTRIB (Dwg_Entity_INSERT *restrict insert, const double height,
                const int flags, const dwg_point_3d *restrict ins_pt,
                const char *restrict tag, const char *restrict text_value)
{
  int err;
  Dwg_Object_BLOCK_HEADER *restrict blkhdr = dwg_entity_owner (insert);
  Dwg_Object *restrict insobj = dwg_obj_generic_to_object (insert, &err);
  API_ADD_ENTITY (ATTRIB);
  ADD_CHECK_3DPOINT (ins_pt);
  ADD_CHECK_DOUBLE (height);
  if (!insobj || err)
    {
      LOG_ERROR ("add_ATTRIB: No INSERT found");
      API_UNADD_ENTITY;
      return NULL;
    }
  if (!dwg_is_valid_tag (tag))
    {
      LOG_ERROR ("add_ATTRIB: Invalid tag %s", tag);
      API_UNADD_ENTITY;
      return NULL;
    }
  _obj->tag = dwg_add_u8_input (dwg, tag);
  _obj->text_value = dwg_add_u8_input (dwg, text_value);
  _obj->ins_pt.x = ins_pt->x;
  _obj->ins_pt.y = ins_pt->y;
  _obj->elevation = ins_pt->z;
  _obj->height = height;
  // block handles
  if (dwg->header_vars.TEXTSTYLE)
    _obj->style = dwg_add_handleref (
        dwg, 5, dwg->header_vars.TEXTSTYLE->absolute_ref, NULL);
  insert->has_attribs = 1;
  // TODO: if !blkhdr->hasattrs: error no ATTDEF
  insert->block_header
      = dwg_add_handleref (dwg, 3, blkobj->handle.value, insobj);
  if (dwg->header.version <= R_12)
    {
      obj->type = DWG_TYPE_ATTRIB_r11;
      if (_obj->elevation == 0.0)
        obj->tio.entity->flag_r11 |= FLAG_R11_HAS_ELEVATION;
    }
  // integrate into INSERT.attribs[]
  add_attrib_links (blkhdr, insert, insobj, obj);
  return _obj;
}

/* internally used only by dwg_add_Attribute only */
EXPORT Dwg_Entity_ATTDEF *
dwg_add_ATTDEF (Dwg_Object_BLOCK_HEADER *restrict blkhdr, const double height,
                const int flags, const char *restrict prompt,
                const dwg_point_3d *restrict ins_pt, const char *restrict tag,
                const char *restrict default_value)
{
  API_ADD_ENTITY (ATTDEF);
  if (dwg->header.version < R_2_0b)
    {
      LOG_ERROR ("Invalid entity %s <r2.0b", "ATTDEF")
      API_UNADD_ENTITY;
      return NULL;
    }
  if (!dwg_is_valid_tag (tag))
    {
      LOG_ERROR ("add_ATTRIB: Invalid tag %s", tag);
      API_UNADD_ENTITY;
      return NULL;
    }
  ADD_CHECK_3DPOINT (ins_pt);
  ADD_CHECK_DOUBLE (height);
  _obj->prompt = dwg_add_u8_input (dwg, prompt);
  _obj->tag = dwg_add_u8_input (dwg, tag);
  _obj->default_value = dwg_add_u8_input (dwg, default_value);
  _obj->ins_pt.x = ins_pt->x;
  _obj->ins_pt.y = ins_pt->y;
  _obj->elevation = ins_pt->z;
  _obj->height = height;
  // block handles
  if (dwg->header_vars.TEXTSTYLE)
    _obj->style = dwg_add_handleref (
        dwg, 5, dwg->header_vars.TEXTSTYLE->absolute_ref, NULL);
  blkhdr->hasattrs = 1;
  if (dwg->header.version <= R_12)
    {
      obj->type = DWG_TYPE_ATTDEF_r11;
      if (_obj->elevation == 0.0)
        obj->tio.entity->flag_r11 |= FLAG_R11_HAS_ELEVATION;
    }
  return _obj;
}

EXPORT Dwg_Entity_BLOCK *
dwg_add_BLOCK (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
               const char *restrict name)
{
  API_ADD_ENTITY (BLOCK);
  if (dwg->header.version <= R_12)
    obj->type = DWG_TYPE_BLOCK_r11;
  _obj->name = dwg_add_u8_input (dwg, name);
  return _obj;
}

EXPORT Dwg_Entity_ENDBLK *
dwg_add_ENDBLK (Dwg_Object_BLOCK_HEADER *restrict blkhdr)
{
  API_ADD_ENTITY (ENDBLK);
  if (dwg->header.version <= R_12)
    obj->type = DWG_TYPE_ENDBLK_r11;
  dwg_fixup_BLOCKS_entities (dwg);
  return _obj;
}

// owned by POLYLINE or INSERT
EXPORT Dwg_Entity_SEQEND *
dwg_add_SEQEND (dwg_ent_generic *restrict owner)
{
  dwg_ent_generic *restrict blkhdr = owner;
  API_ADD_ENTITY (SEQEND);
  if (dwg->header.version <= R_12)
    obj->type = DWG_TYPE_SEQEND_r11;
  return _obj;
}

EXPORT Dwg_Entity_INSERT *
dwg_add_INSERT (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                const dwg_point_3d *restrict ins_pt, const char *restrict name,
                const double xscale, const double yscale, const double zscale,
                const double rotation)
{
  BITCODE_H hdrref;
  API_ADD_ENTITY (INSERT);
  ADD_CHECK_3DPOINT (ins_pt);
  ADD_CHECK_DOUBLE (xscale);
  ADD_CHECK_DOUBLE (yscale);
  ADD_CHECK_DOUBLE (zscale);
  _obj->ins_pt.x = ins_pt->x;
  _obj->ins_pt.y = ins_pt->y;
  _obj->ins_pt.z = ins_pt->z;
  _obj->scale.x = xscale;
  _obj->scale.y = yscale;
  _obj->scale.z = zscale;
  // TODO scale_flag
  _obj->rotation = rotation;
  ADD_CHECK_ANGLE (_obj->rotation);
  if (!dwg_is_valid_name_u8 (dwg, name))
    {
      LOG_WARN ("Invalid blockname %s", name);
      // API_UNADD_ENTITY;
      // return NULL;
    }
  hdrref = dwg_find_tablehandle (dwg, name, "BLOCK");
  if (hdrref)
    {
      Dwg_Object *hdr = dwg_ref_object (dwg, hdrref);
      if (!hdr)
        return _obj;
      _obj->block_header = dwg_add_handleref (dwg, 5, hdr->handle.value, NULL);
      blkhdr = hdr->tio.object->tio.BLOCK_HEADER;
      blkhdr->used = 1;
      blkhdr->is_xref_ref = 1;
      blkhdr->num_inserts++;
      if (!blkhdr->inserts)
        blkhdr->inserts = (BITCODE_H *)CALLOC (1, sizeof (BITCODE_H));
      else
        blkhdr->inserts = (BITCODE_H *)REALLOC (
            blkhdr->inserts, blkhdr->num_inserts * sizeof (BITCODE_H));
      blkhdr->inserts[blkhdr->num_inserts - 1]
          = dwg_add_handleref (dwg, 4, obj->handle.value, NULL);
    }
  else
    {
      API_UNADD_ENTITY;
      LOG_ERROR ("block %s not found", name);
      return NULL;
    }
  if (dwg->header.version < R_2_0b)
    _obj->block_name = STRDUP (name);
  if (dwg->header.version <= R_12)
    {
      obj->type = DWG_TYPE_INSERT_r11;
      if (_obj->ins_pt.z == 0.0)
        {
          obj->tio.entity->flag_r11 |= FLAG_R11_HAS_ELEVATION;
          obj->tio.entity->opts_r11 = 1;
        }
    }
  return _obj;
}

EXPORT Dwg_Entity_MINSERT *
dwg_add_MINSERT (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                 const dwg_point_3d *restrict ins_pt,
                 const char *restrict name, const double xscale,
                 const double yscale, const double zscale,
                 const double rotation, const int num_rows, const int num_cols,
                 const double row_spacing, const double col_spacing)
{
  BITCODE_H hdrref;
  API_ADD_ENTITY (MINSERT);
  ADD_CHECK_3DPOINT (ins_pt);
  ADD_CHECK_DOUBLE (xscale);
  ADD_CHECK_DOUBLE (yscale);
  ADD_CHECK_DOUBLE (zscale);
  _obj->ins_pt.x = ins_pt->x;
  _obj->ins_pt.y = ins_pt->y;
  _obj->ins_pt.z = ins_pt->z;
  _obj->scale.x = xscale;
  _obj->scale.y = yscale;
  _obj->scale.z = zscale;
  // TODO scale_flag
  _obj->rotation = rotation;
  ADD_CHECK_ANGLE (_obj->rotation);
  _obj->num_rows = (BITCODE_BS)num_rows;
  _obj->num_cols = (BITCODE_BS)num_cols;
  _obj->row_spacing = row_spacing;
  _obj->col_spacing = col_spacing;
  hdrref = dwg_find_tablehandle (dwg, name, "BLOCK");
  if (hdrref)
    {
      Dwg_Object *hdr = dwg_ref_object (dwg, hdrref);
      if (!hdr)
        return _obj;
      _obj->block_header = dwg_add_handleref (dwg, 5, hdr->handle.value, NULL);
      blkhdr = hdr->tio.object->tio.BLOCK_HEADER;
      blkhdr->used = 1;
      blkhdr->is_xref_ref = 1;
      blkhdr->num_inserts++;
      if (!blkhdr->inserts)
        blkhdr->inserts = (BITCODE_H *)CALLOC (1, sizeof (BITCODE_H));
      else
        blkhdr->inserts = (BITCODE_H *)REALLOC (
            blkhdr->inserts, blkhdr->num_inserts * sizeof (BITCODE_H));
      blkhdr->inserts[blkhdr->num_inserts - 1]
          = dwg_add_handleref (dwg, 4, obj->handle.value, NULL);
    }
  if (dwg->header.version <= R_12)
    {
      obj->type = DWG_TYPE_INSERT_r11;
      if (_obj->ins_pt.z == 0.0)
        {
          obj->tio.entity->flag_r11 |= FLAG_R11_HAS_ELEVATION;
          obj->tio.entity->opts_r11 = 1;
        }
    }
  return _obj;
}

// TODO blkhdr => pline owner
Dwg_Entity_VERTEX_2D *
dwg_add_VERTEX_2D (Dwg_Entity_POLYLINE_2D *restrict pline,
                   const dwg_point_2d *restrict point)
{
  Dwg_Object_BLOCK_HEADER *restrict blkhdr = dwg_entity_owner (pline);
  API_ADD_ENTITY (VERTEX_2D);
  obj->tio.entity->entmode = 0;
  obj->tio.entity->ownerhandle
      = dwg_add_handleref (dwg, 4, dwg_obj_generic_handlevalue (pline), obj);
  ADD_CHECK_2DPOINT (point);
  _obj->point.x = point->x;
  _obj->point.y = point->y;
  if (dwg->header.version <= R_12)
    {
      obj->type = DWG_TYPE_VERTEX_r11;
    }
  return _obj;
}

Dwg_Entity_VERTEX_3D *
dwg_add_VERTEX_3D (Dwg_Entity_POLYLINE_3D *restrict pline,
                   const dwg_point_3d *restrict point)
{
  Dwg_Object_BLOCK_HEADER *restrict blkhdr = dwg_entity_owner (pline);
  API_ADD_ENTITY (VERTEX_3D);
  obj->tio.entity->entmode = 0;
  obj->tio.entity->ownerhandle
      = dwg_add_handleref (dwg, 4, dwg_obj_generic_handlevalue (pline), obj);
  ADD_CHECK_3DPOINT (point);
  _obj->point.x = point->x;
  _obj->point.y = point->y;
  _obj->point.z = point->z;
  _obj->flag = FLAG_VERTEX_3D;
  if (dwg->header.version <= R_12)
    {
      obj->type = DWG_TYPE_VERTEX_r11;
      obj->tio.entity->opts_r11 = OPTS_R11_VERTEX_HAS_FLAG;
      if (_obj->point.z != 0.0)
        {
          obj->tio.entity->flag_r11 |= FLAG_R11_HAS_ELEVATION;
          obj->tio.entity->elevation_r11 = point->z;
        }
    }
  return _obj;
}

EXPORT Dwg_Entity_POLYLINE_2D *
dwg_add_POLYLINE_2D (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                     const int num_pts, const dwg_point_2d *restrict pts)
{
  Dwg_Object *pl, *seq, *vtx = NULL;
  Dwg_Entity_POLYLINE_2D *_pl;
  Dwg_Entity_VERTEX_2D *_vtx;
  Dwg_Entity_SEQEND *_seq;

  API_ADD_ENTITY (POLYLINE_2D);
  pl = obj;
  _pl = _obj;
  _pl->vertex = (BITCODE_H *)MALLOC (num_pts * sizeof (BITCODE_H));
  if (!_pl->vertex)
    return NULL;
  if (num_pts)
    {
      _pl->has_vertex = 1;
      obj->tio.entity->flag_r11 = FLAG_R11_HAS_ATTRIBS;
    }
  for (int i = 0; i < num_pts; i++)
    {
      _vtx = dwg_add_VERTEX_2D (_pl, &pts[i]);
      if (!_vtx)
        {
        vtx_2d_err:
          LOG_ERROR ("No VERTEX_2D[%d] added", i);
          return NULL;
        }
      vtx = dwg_obj_generic_to_object (_vtx, &error);
      if (!vtx || !vtx->tio.entity || vtx->supertype != DWG_SUPERTYPE_ENTITY)
        goto vtx_2d_err;
      vtx->tio.entity->next_entity = NULL;
      _pl->vertex[i] = dwg_add_handleref (dwg, 3, vtx->handle.value, pl);
      if (i == 0)
        _pl->first_vertex
            = dwg_add_handleref (dwg, 4, vtx->handle.value, NULL);
      if (i == num_pts - 1)
        {
          vtx->tio.entity->prev_entity
              = dwg_add_handleref (dwg, 4, vtx->handle.value - 1, vtx);
          _pl->last_vertex
              = dwg_add_handleref (dwg, 4, vtx->handle.value, NULL);
          // set flag 1 if closed
          if (pts[i].x == pts[0].x && pts[i].y == pts[0].y)
            {
              obj->tio.entity->opts_r11 = OPTS_R11_POLYLINE_HAS_FLAG;
              _obj->flag |= FLAG_POLYLINE_CLOSED;
            }
        }
    }
  _seq = dwg_add_SEQEND ((dwg_ent_generic *)_pl);
  if (!_seq)
    {
      LOG_ERROR ("No SEQEND added");
      return NULL;
    }
  seq = dwg_obj_generic_to_object (_seq, &error);
  _pl->seqend
      = dwg_add_handleref (dwg, 3, dwg_obj_generic_handlevalue (_seq), pl);
  pl->tio.entity->next_entity = NULL;
  if (dwg->header.version <= R_12)
    obj->type = DWG_TYPE_POLYLINE_r11;

  _pl->num_owned = num_pts;
  IN_POSTPROCESS_SEQEND (seq, _pl->num_owned, _pl->vertex);
  return _pl;
}

EXPORT Dwg_Entity_POLYLINE_3D *
dwg_add_POLYLINE_3D (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                     const int num_pts, const dwg_point_3d *restrict pts)
{
  Dwg_Object *pl, *seq, *vtx = NULL;
  Dwg_Entity_POLYLINE_3D *_pl;
  Dwg_Entity_VERTEX_3D *_vtx;
  Dwg_Entity_SEQEND *_seq;

  API_ADD_ENTITY (POLYLINE_3D);
  pl = obj;
  _pl = _obj;
  _pl->vertex = (BITCODE_H *)MALLOC (num_pts * sizeof (BITCODE_H));
  if (!_pl->vertex)
    {
      LOG_ERROR ("%s: Out of memory", __FUNCTION__)
      return NULL;
    }
  obj->tio.entity->opts_r11 = OPTS_R11_POLYLINE_HAS_FLAG;
  _obj->flag = FLAG_POLYLINE_3D;
  if (num_pts)
    {
      _pl->has_vertex = 1;
      obj->tio.entity->flag_r11 = FLAG_R11_HAS_ATTRIBS;
    }
  for (int i = 0; i < num_pts; i++)
    {
      _vtx = dwg_add_VERTEX_3D (_pl, &pts[i]);
      if (!_vtx)
        {
        vtx_3d_err:
          LOG_ERROR ("No VERTEX_3D[%d] added", i);
          return NULL;
        }
      vtx = dwg_obj_generic_to_object (_vtx, &error);
      if (!vtx || !vtx->tio.entity || vtx->supertype != DWG_SUPERTYPE_ENTITY)
        goto vtx_3d_err;
      vtx->tio.entity->next_entity = NULL;
      _pl->vertex[i] = dwg_add_handleref (dwg, 3, vtx->handle.value, pl);
      if (i == 0)
        _pl->first_vertex
            = dwg_add_handleref (dwg, 4, vtx->handle.value, NULL);
      if (i == num_pts - 1)
        {
          vtx->tio.entity->prev_entity
              = dwg_add_handleref (dwg, 4, vtx->handle.value - 1, vtx);
          _pl->last_vertex
              = dwg_add_handleref (dwg, 4, vtx->handle.value, NULL);
          // set flag 1 if closed
          if (pts[i].x == pts[0].x && pts[i].y == pts[0].y
              && pts[i].z == pts[0].z)
            _obj->flag |= FLAG_POLYLINE_CLOSED;
        }
    }
  _seq = dwg_add_SEQEND ((dwg_ent_generic *)_pl);
  if (!_seq)
    {
      LOG_ERROR ("No SEQEND added");
      return NULL;
    }
  seq = dwg_obj_generic_to_object (_seq, &error);
  _pl->seqend
      = dwg_add_handleref (dwg, 3, dwg_obj_generic_handlevalue (_seq), pl);
  pl->tio.entity->next_entity = NULL;
  if (dwg->header.version <= R_12)
    obj->type = DWG_TYPE_POLYLINE_r11;

  _pl->num_owned = num_pts;
  IN_POSTPROCESS_SEQEND (seq, _pl->num_owned, _pl->vertex);
  return _pl;
}

Dwg_Entity_VERTEX_PFACE *
dwg_add_VERTEX_PFACE (Dwg_Entity_POLYLINE_PFACE *restrict pline,
                      const dwg_point_3d *restrict point)
{
  Dwg_Object_BLOCK_HEADER *restrict blkhdr = dwg_entity_owner (pline);
  API_ADD_ENTITY (VERTEX_PFACE);
  obj->tio.entity->entmode = 0;
  obj->tio.entity->ownerhandle
      = dwg_add_handleref (dwg, 4, dwg_obj_generic_handlevalue (pline), obj);
  ADD_CHECK_3DPOINT (point);
  _obj->point.x = point->x;
  _obj->point.y = point->y;
  _obj->point.z = point->z;
  obj->tio.entity->opts_r11 = OPTS_R11_VERTEX_HAS_FLAG;
  _obj->flag = FLAG_VERTEX_PFACE_MESH + FLAG_VERTEX_MESH;
  if (dwg->header.version <= R_12)
    {
      obj->type = DWG_TYPE_VERTEX_r11;
      if (_obj->point.z != 0.0)
        {
          obj->tio.entity->flag_r11 |= FLAG_R11_HAS_ELEVATION;
          obj->tio.entity->elevation_r11 = point->z;
        }
    }
  //_obj->flag = 0xc0;
  return _obj;
}

// invisible face edge if negative
// no 4th edge (ie a triangle) if the last face has index 0 (starts with 1)
Dwg_Entity_VERTEX_PFACE_FACE *
dwg_add_VERTEX_PFACE_FACE (Dwg_Entity_POLYLINE_PFACE *restrict pline,
                           const dwg_face vertind)
{
  Dwg_Object_BLOCK_HEADER *restrict blkhdr = dwg_entity_owner (pline);
  API_ADD_ENTITY (VERTEX_PFACE_FACE);
  obj->tio.entity->entmode = 0;
  obj->tio.entity->ownerhandle
      = dwg_add_handleref (dwg, 4, dwg_obj_generic_handlevalue (pline), obj);
  obj->tio.entity->opts_r11
      = OPTS_R11_VERTEX_HAS_FLAG | OPTS_R11_VERTEX_HAS_INDEX1
        | OPTS_R11_VERTEX_HAS_INDEX2 | OPTS_R11_VERTEX_HAS_INDEX3 | 0x100
        | OPTS_R11_VERTEX_HAS_NOT_X_Y;
  _obj->flag = FLAG_VERTEX_PFACE_MESH;
  _obj->vertind[0] = vertind[0];
  _obj->vertind[1] = vertind[1];
  _obj->vertind[2] = vertind[2];
  _obj->vertind[3] = vertind[3];
  if (vertind[3] != 0)
    obj->tio.entity->opts_r11 |= OPTS_R11_VERTEX_HAS_INDEX4;
  if (dwg->header.version <= R_12)
    {
      obj->type = DWG_TYPE_VERTEX_r11;
    }
  return _obj;
}

// Polyface individual Mesh
EXPORT Dwg_Entity_POLYLINE_PFACE *
dwg_add_POLYLINE_PFACE (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                        const unsigned numverts, const unsigned numfaces,
                        const dwg_point_3d *restrict verts,
                        const dwg_face *restrict faces)
{
  Dwg_Object *pl, *seq, *vtx;
  Dwg_Entity_POLYLINE_PFACE *_pl;
  Dwg_Entity_VERTEX_PFACE *_vtx;
  Dwg_Entity_VERTEX_PFACE_FACE *_vtxf;
  Dwg_Entity_SEQEND *_seq;

  API_ADD_ENTITY (POLYLINE_PFACE);
  pl = obj;
  _pl = _obj;
  _pl->vertex = (BITCODE_H *)MALLOC (((unsigned long)numverts + numfaces)
                                     * sizeof (BITCODE_H));
  if (!_pl->vertex)
    {
      LOG_ERROR ("%s: Out of memory", __FUNCTION__)
      return NULL;
    }
  _pl->has_vertex = 1;
  obj->tio.entity->opts_r11 = OPTS_R11_POLYLINE_HAS_FLAG
                              | OPTS_R11_POLYLINE_HAS_M_VERTS
                              | OPTS_R11_POLYLINE_HAS_N_VERTS;
  // obj->tio.entity->flag_r11 = FLAG_R11_HAS_ATTRIBS; // not
  _obj->flag = FLAG_POLYLINE_PFACE_MESH;
  _pl->numverts = numverts;
  _pl->numfaces = numfaces;
  _pl->num_owned = numverts + numfaces;
  for (unsigned i = 0; i < numverts; i++)
    {
      _vtx = dwg_add_VERTEX_PFACE (_pl, &verts[i]);
      if (!_vtx)
        {
        vtx_pface_err:
          LOG_ERROR ("No VERTEX_PFACE[%d] added", i);
          return NULL;
        }
      vtx = dwg_obj_generic_to_object (_vtx, &error);
      if (!vtx || !vtx->tio.entity || vtx->supertype != DWG_SUPERTYPE_ENTITY)
        goto vtx_pface_err;
      _pl->vertex[i] = dwg_add_handleref (dwg, 3, vtx->handle.value, pl);
      if (i == 0)
        _pl->first_vertex
            = dwg_add_handleref (dwg, 4, vtx->handle.value, NULL);
    }
  for (unsigned j = 0; j < numfaces; j++)
    {
      _vtxf = dwg_add_VERTEX_PFACE_FACE (_pl, faces[j]);
      if (!_vtxf)
        {
        vtx_pface_face_err:
          LOG_ERROR ("No VERTEX_PFACE_FACE[%d] added", j);
          return NULL;
        }
      vtx = dwg_obj_generic_to_object (_vtxf, &error);
      if (!vtx || !vtx->tio.entity || vtx->supertype != DWG_SUPERTYPE_ENTITY)
        goto vtx_pface_face_err;
      vtx->tio.entity->next_entity = NULL;
      _pl->vertex[numverts + j]
          = dwg_add_handleref (dwg, 3, vtx->handle.value, pl);
      if (j == numfaces - 1)
        {
          // FIXME
          vtx->tio.entity->prev_entity
              = dwg_add_handleref (dwg, 4, vtx->handle.value - 1, vtx);
          _pl->last_vertex
              = dwg_add_handleref (dwg, 4, vtx->handle.value, NULL);
        }
    }
  _seq = dwg_add_SEQEND ((dwg_ent_generic *)_pl);
  if (!_seq)
    {
      LOG_ERROR ("No SEQEND added");
      return NULL;
    }
  seq = dwg_obj_generic_to_object (_seq, &error);
  _pl->seqend
      = dwg_add_handleref (dwg, 3, dwg_obj_generic_handlevalue (_seq), pl);
  IN_POSTPROCESS_SEQEND (seq, _pl->num_owned, _pl->vertex);
  pl->tio.entity->next_entity = NULL; // fixup
  if (dwg->header.version <= R_12)
    obj->type = DWG_TYPE_POLYLINE_r11;
  return _pl;
}

Dwg_Entity_VERTEX_MESH *
dwg_add_VERTEX_MESH (Dwg_Entity_POLYLINE_MESH *restrict pline,
                     const dwg_point_3d *restrict point)
{
  Dwg_Object_BLOCK_HEADER *restrict blkhdr = dwg_entity_owner (pline);
  API_ADD_ENTITY (VERTEX_MESH);
  obj->tio.entity->entmode = 0;
  obj->tio.entity->ownerhandle
      = dwg_add_handleref (dwg, 4, dwg_obj_generic_handlevalue (pline), obj);
  ADD_CHECK_3DPOINT (point);
  _obj->point.x = point->x;
  _obj->point.y = point->y;
  _obj->point.z = point->z;
  _obj->flag = FLAG_VERTEX_MESH;
  if (dwg->header.version <= R_12)
    {
      obj->type = DWG_TYPE_VERTEX_r11;
      // obj->tio.entity->flag_r11 = 0;
      obj->tio.entity->opts_r11 = OPTS_R11_VERTEX_HAS_FLAG;
      if (_obj->point.z != 0.0)
        {
          obj->tio.entity->flag_r11 |= FLAG_R11_HAS_ELEVATION;
          obj->tio.entity->elevation_r11 = point->z;
        }
    }
  return _obj;
}

EXPORT Dwg_Entity_POLYLINE_MESH *
dwg_add_POLYLINE_MESH (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                       const unsigned num_m_verts, const unsigned num_n_verts,
                       const dwg_point_3d *restrict verts)
{
  Dwg_Object *pl, *seq, *vtx;
  Dwg_Entity_POLYLINE_MESH *_pl;
  Dwg_Entity_VERTEX_MESH *_vtx;
  Dwg_Entity_SEQEND *_seq;

  API_ADD_ENTITY (POLYLINE_MESH);
  pl = obj;
  _pl = _obj;
  _pl->vertex = (BITCODE_H *)MALLOC ((unsigned long)num_m_verts * num_n_verts
                                     * sizeof (BITCODE_H));
  if (!_pl->vertex)
    {
      LOG_ERROR ("%s: Out of memory", __FUNCTION__)
      return NULL;
    }
  _pl->flag = FLAG_POLYLINE_MESH;
  _pl->num_m_verts = num_m_verts;
  _pl->num_n_verts = num_n_verts;
  _pl->num_owned = num_m_verts * num_n_verts;
  if (_pl->num_owned)
    _pl->has_vertex = 1;
  for (unsigned i = 0; i < _pl->num_owned; i++)
    {
      _vtx = dwg_add_VERTEX_MESH (_pl, &verts[i]);
      if (!_vtx)
        {
        vtx_mesh_err:
          LOG_ERROR ("No VERTEX_MESH[%d] added", i);
          return NULL;
        }
      vtx = dwg_obj_generic_to_object (_vtx, &error);
      if (!vtx || !vtx->tio.entity || vtx->supertype != DWG_SUPERTYPE_ENTITY)
        goto vtx_mesh_err;
      vtx->tio.entity->next_entity = NULL;
      _pl->vertex[i] = dwg_add_handleref (dwg, 3, vtx->handle.value, pl);
      if (i == 0)
        _pl->first_vertex
            = dwg_add_handleref (dwg, 4, vtx->handle.value, NULL);
      if (i == _pl->num_owned - 1)
        {
          vtx->tio.entity->prev_entity
              = dwg_add_handleref (dwg, 4, vtx->handle.value - 1, vtx);
          _pl->last_vertex
              = dwg_add_handleref (dwg, 4, vtx->handle.value, NULL);
        }
    }
  _seq = dwg_add_SEQEND ((dwg_ent_generic *)_pl);
  if (!_seq)
    {
      LOG_ERROR ("No SEQEND added");
      return NULL;
    }
  seq = dwg_obj_generic_to_object (_seq, &error);
  _pl->seqend
      = dwg_add_handleref (dwg, 3, dwg_obj_generic_handlevalue (_seq), pl);
  pl->tio.entity->next_entity = NULL;
  if (dwg->header.version <= R_12)
    {
      obj->type = DWG_TYPE_POLYLINE_r11;
      obj->tio.entity->opts_r11 = OPTS_R11_POLYLINE_HAS_FLAG;
      if (_pl->num_owned)
        obj->tio.entity->flag_r11 |= FLAG_R11_HAS_ATTRIBS;
      if (num_m_verts)
        obj->tio.entity->opts_r11 |= OPTS_R11_POLYLINE_HAS_M_VERTS;
      if (num_n_verts)
        obj->tio.entity->opts_r11 |= OPTS_R11_POLYLINE_HAS_N_VERTS;
    }
  IN_POSTPROCESS_SEQEND (seq, _pl->num_owned, _pl->vertex);
  return _pl;
}

EXPORT Dwg_Entity_ARC *
dwg_add_ARC (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
             const dwg_point_3d *restrict center, const double radius,
             const double start_angle, const double end_angle)
{
  API_ADD_ENTITY (ARC);
  ADD_CHECK_3DPOINT (center);
  _obj->center.x = center->x;
  _obj->center.y = center->y;
  _obj->center.z = center->z;
  _obj->radius = radius;
  _obj->start_angle = start_angle;
  _obj->end_angle = end_angle;
  ADD_CHECK_ANGLE (_obj->start_angle);
  ADD_CHECK_ANGLE (_obj->end_angle);
  if (dwg->header.version <= R_12)
    {
      obj->type = DWG_TYPE_ARC_r11;
      if (_obj->center.z == 0.0)
        obj->tio.entity->flag_r11 |= FLAG_R11_HAS_ELEVATION;
    }
  return _obj;
}

EXPORT Dwg_Entity_CIRCLE *
dwg_add_CIRCLE (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                const dwg_point_3d *restrict center, const double radius)
{
  API_ADD_ENTITY (CIRCLE);
  ADD_CHECK_3DPOINT (center);
  _obj->center.x = center->x;
  _obj->center.y = center->y;
  _obj->center.z = center->z;
  _obj->radius = radius;
  if (dwg->header.version <= R_12)
    {
      obj->type = DWG_TYPE_CIRCLE_r11;
      // UNTESTED
      if (_obj->center.z == 0.0)
        obj->tio.entity->flag_r11 |= FLAG_R11_HAS_ELEVATION;
    }
  return _obj;
}

EXPORT Dwg_Entity_LINE *
dwg_add_LINE (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
              const dwg_point_3d *restrict start_pt,
              const dwg_point_3d *restrict end_pt)
{
  API_ADD_ENTITY (LINE);
  ADD_CHECK_3DPOINT (start_pt);
  ADD_CHECK_3DPOINT (end_pt);
  _obj->start.x = start_pt->x;
  _obj->start.y = start_pt->y;
  _obj->start.z = start_pt->z;
  _obj->end.x = end_pt->x;
  _obj->end.y = end_pt->y;
  _obj->end.z = end_pt->z;

  if (dwg->header.version <= R_11)
    obj->type = DWG_TYPE_LINE_r11;
  // In case of 3d line we are changing type to 3DLINE entity
  if (dwg->header.version >= R_2_4 && dwg->header.version < R_10)
    {
      if (_obj->start.z != 0.0)
        {
          obj->type = DWG_TYPE_3DLINE_r11;
          obj->fixedtype = DWG_TYPE__3DLINE;
          obj->tio.entity->opts_r11 |= 1;
        }
      if (_obj->end.z != 0.0)
        {
          obj->type = DWG_TYPE_3DLINE_r11;
          obj->fixedtype = DWG_TYPE__3DLINE;
          obj->tio.entity->opts_r11 |= 2;
        }
    }
  // There is 3DLINE entity in R_10, but duplicit with LINE (without
  // HAS_ELEVATION)
  if (dwg->header.version == R_10)
    {
      if (_obj->start.z == 0.0 && _obj->end.z == 0.0)
        obj->tio.entity->flag_r11 |= FLAG_R11_HAS_ELEVATION;
    }
  return _obj;
}

static void
dwg_require_DIMSTYLE_Standard (Dwg_Data *restrict dwg)
{
  const char *standard = dwg->header.version < R_13 ? "STANDARD" : "Standard";
  if (!(dwg_find_tablehandle_silent (dwg, standard, "DIMSTYLE")))
    {
      Dwg_Object_DIMSTYLE *std = dwg_add_DIMSTYLE (dwg, standard);
      if (std)
        dwg->header_vars.DIMSTYLE = dwg_add_handleref (
            dwg, 5, dwg_obj_generic_handlevalue (std), NULL);
    }
}

#define DIMENSION_DEFAULTS                                                    \
  _obj->extrusion.z = 1.0;                                                    \
  if (dwg->header_vars.DIMSTYLE)                                              \
  _obj->dimstyle = dwg_add_handleref (                                        \
      dwg, 5, dwg->header_vars.DIMSTYLE->absolute_ref, NULL)

EXPORT Dwg_Entity_DIMENSION_ALIGNED *
dwg_add_DIMENSION_ALIGNED (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                           const dwg_point_3d *restrict xline1_pt,
                           const dwg_point_3d *restrict xline2_pt,
                           const dwg_point_3d *restrict text_midpt)
{
  API_ADD_PREP (DIMENSION_ALIGNED);
  if (dwg->header.version <= R_2_22)
    {
      LOG_ERROR ("Invalid entity %s <r2.22", "DIMENSION_ALIGNED")
      return NULL;
    }
  dwg_require_DIMSTYLE_Standard (dwg);
  API_ADD_ENTITY2 (DIMENSION_ALIGNED);
  DIMENSION_DEFAULTS;
  ADD_CHECK_3DPOINT (xline1_pt);
  ADD_CHECK_3DPOINT (xline2_pt);
  ADD_CHECK_3DPOINT (text_midpt);
  _obj->text_midpt.x = text_midpt->x;
  _obj->text_midpt.y = text_midpt->y;
  //_obj->text_midpt.z= text_midpt->z;
  _obj->xline1_pt.x = xline1_pt->x;
  _obj->xline1_pt.y = xline1_pt->y;
  _obj->xline1_pt.z = xline1_pt->z;
  _obj->xline2_pt.x = xline2_pt->x;
  _obj->xline2_pt.y = xline2_pt->y;
  _obj->xline2_pt.z = xline2_pt->z;
  // TODO calc oblique_angle
  if (dwg->header.version <= R_12)
    {
      obj->type = DWG_TYPE_DIMENSION_r11;
      obj->tio.entity->flag_r11 = 64 + FLAG_R11_DIMENSION_ALIGNED;
    }
  return _obj;
}

EXPORT Dwg_Entity_DIMENSION_ANG2LN * /* DimAngular */
dwg_add_DIMENSION_ANG2LN (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                          const dwg_point_3d *restrict center_pt,
                          const dwg_point_3d *restrict xline1end_pt,
                          const dwg_point_3d *restrict xline2end_pt,
                          const dwg_point_3d *restrict text_midpt)
{
  API_ADD_PREP (DIMENSION_ANG2LN);
  if (dwg->header.version <= R_2_22)
    {
      LOG_ERROR ("Invalid entity %s <r2.22", "DIMENSION_ANG2LN")
      return NULL;
    }
  dwg_require_DIMSTYLE_Standard (dwg);
  API_ADD_ENTITY2 (DIMENSION_ANG2LN);
  DIMENSION_DEFAULTS;
  ADD_CHECK_3DPOINT (center_pt);
  ADD_CHECK_3DPOINT (xline1end_pt);
  ADD_CHECK_3DPOINT (xline2end_pt);
  ADD_CHECK_3DPOINT (text_midpt);
  _obj->def_pt.x = center_pt->x;
  _obj->def_pt.y = center_pt->y;
  _obj->def_pt.z = center_pt->z;
  _obj->text_midpt.x = text_midpt->x;
  _obj->text_midpt.y = text_midpt->y;
  //_obj->text_midpt.z= text_midpt->z;
  // TODO calc xline1start_pt, xline2start_pt
  _obj->xline1end_pt.x = xline1end_pt->x;
  _obj->xline1end_pt.y = xline1end_pt->y;
  _obj->xline1end_pt.z = xline1end_pt->z;
  _obj->xline2end_pt.x = xline2end_pt->x;
  _obj->xline2end_pt.y = xline2end_pt->y;
  _obj->xline2end_pt.z = xline2end_pt->z;
  if (dwg->header.version <= R_12)
    {
      obj->type = DWG_TYPE_DIMENSION_r11;
      obj->tio.entity->flag_r11 = 64 + FLAG_R11_DIMENSION_ANG2LN;
    }
  return _obj;
}

EXPORT Dwg_Entity_DIMENSION_ANG3PT *
dwg_add_DIMENSION_ANG3PT (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                          const dwg_point_3d *restrict center_pt,
                          const dwg_point_3d *restrict xline1_pt,
                          const dwg_point_3d *restrict xline2_pt,
                          const dwg_point_3d *restrict text_midpt)
{
  API_ADD_PREP (DIMENSION_ANG3PT);
  if (dwg->header.version <= R_2_22)
    {
      LOG_ERROR ("Invalid entity %s <r2.22", "DIMENSION_ANG3PT")
      return NULL;
    }
  dwg_require_DIMSTYLE_Standard (dwg);
  API_ADD_ENTITY2 (DIMENSION_ANG3PT);
  DIMENSION_DEFAULTS;
  ADD_CHECK_3DPOINT (center_pt);
  ADD_CHECK_3DPOINT (xline1_pt);
  ADD_CHECK_3DPOINT (xline2_pt);
  ADD_CHECK_3DPOINT (text_midpt);
  _obj->center_pt.x = center_pt->x;
  _obj->center_pt.y = center_pt->y;
  _obj->center_pt.z = center_pt->z;
  _obj->text_midpt.x = text_midpt->x;
  _obj->text_midpt.y = text_midpt->y;
  //_obj->text_midpt.z = text_midpt->z;
  _obj->xline1_pt.x = xline1_pt->x;
  _obj->xline1_pt.y = xline1_pt->y;
  _obj->xline1_pt.z = xline1_pt->z;
  _obj->xline2_pt.x = xline2_pt->x;
  _obj->xline2_pt.y = xline2_pt->y;
  _obj->xline2_pt.z = xline2_pt->z;
  if (dwg->header.version <= R_12)
    {
      obj->type = DWG_TYPE_DIMENSION_r11;
      obj->tio.entity->flag_r11 = 64 + FLAG_R11_DIMENSION_ANG3PT;
    }
  return _obj;
}

EXPORT Dwg_Entity_DIMENSION_DIAMETER *
dwg_add_DIMENSION_DIAMETER (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                            const dwg_point_3d *restrict chord_pt,
                            const dwg_point_3d *restrict far_chord_pt,
                            const double leader_len)
{
  API_ADD_PREP (DIMENSION_DIAMETER);
  if (dwg->header.version <= R_2_22)
    {
      LOG_ERROR ("Invalid entity %s <r2.22", "DIMENSION_DIAMETER")
      return NULL;
    }
  dwg_require_DIMSTYLE_Standard (dwg);
  API_ADD_ENTITY2 (DIMENSION_DIAMETER);
  DIMENSION_DEFAULTS;
  ADD_CHECK_3DPOINT (chord_pt);
  ADD_CHECK_3DPOINT (far_chord_pt);
  ADD_CHECK_DOUBLE (leader_len);
  _obj->def_pt.x = far_chord_pt->x;
  _obj->def_pt.y = far_chord_pt->y;
  _obj->def_pt.z = far_chord_pt->z;
  _obj->first_arc_pt.x = chord_pt->x;
  _obj->first_arc_pt.y = chord_pt->y;
  _obj->first_arc_pt.z = chord_pt->z;
  _obj->leader_len = leader_len;
  if (dwg->header.version <= R_12)
    {
      obj->type = DWG_TYPE_DIMENSION_r11;
      obj->tio.entity->flag_r11 = 64 + FLAG_R11_DIMENSION_DIAMETER;
    }
  return _obj;
}

EXPORT Dwg_Entity_DIMENSION_ORDINATE *
dwg_add_DIMENSION_ORDINATE (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                            const dwg_point_3d *restrict feature_location_pt,
                            const dwg_point_3d *restrict leader_endpt,
                            const bool use_x_axis)
{
  API_ADD_PREP (DIMENSION_ORDINATE);
  if (dwg->header.version <= R_2_22)
    {
      LOG_ERROR ("Invalid entity %s <r2.22", "DIMENSION_ORDINATE")
      return NULL;
    }
  dwg_require_DIMSTYLE_Standard (dwg);
  API_ADD_ENTITY2 (DIMENSION_ORDINATE);
  DIMENSION_DEFAULTS;
  ADD_CHECK_3DPOINT (feature_location_pt);
  ADD_CHECK_3DPOINT (leader_endpt);
  _obj->feature_location_pt.x = feature_location_pt->x;
  _obj->feature_location_pt.y = feature_location_pt->y;
  _obj->feature_location_pt.z = feature_location_pt->z;
  _obj->leader_endpt.x = leader_endpt->x;
  _obj->leader_endpt.y = leader_endpt->y;
  _obj->leader_endpt.z = leader_endpt->z;
  _obj->flag2 = (BITCODE_RC)use_x_axis;
  if (dwg->header.version <= R_12)
    {
      obj->type = DWG_TYPE_DIMENSION_r11;
      obj->tio.entity->flag_r11 = 64 + FLAG_R11_DIMENSION_ORDINATE;
    }
  return _obj;
}

EXPORT Dwg_Entity_DIMENSION_RADIUS *
dwg_add_DIMENSION_RADIUS (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                          const dwg_point_3d *restrict center_pt,
                          const dwg_point_3d *restrict chord_pt,
                          const double leader_len)
{
  API_ADD_PREP (DIMENSION_RADIUS);
  if (dwg->header.version <= R_2_22)
    {
      LOG_ERROR ("Invalid entity %s <r2.22", "DIMENSION_RADIUS")
      return NULL;
    }
  dwg_require_DIMSTYLE_Standard (dwg);
  API_ADD_ENTITY2 (DIMENSION_RADIUS);
  DIMENSION_DEFAULTS;
  ADD_CHECK_3DPOINT (center_pt);
  ADD_CHECK_3DPOINT (chord_pt);
  ADD_CHECK_DOUBLE (leader_len);
  _obj->def_pt.x = center_pt->x;
  _obj->def_pt.y = center_pt->y;
  _obj->def_pt.z = center_pt->z;
  _obj->first_arc_pt.x = chord_pt->x;
  _obj->first_arc_pt.y = chord_pt->y;
  _obj->first_arc_pt.z = chord_pt->z;
  _obj->leader_len = leader_len;
  if (dwg->header.version <= R_12)
    {
      obj->type = DWG_TYPE_DIMENSION_r11;
      obj->tio.entity->flag_r11 = 64 + FLAG_R11_DIMENSION_RADIUS;
    }
  return _obj;
}

EXPORT Dwg_Entity_DIMENSION_LINEAR * /* Rotated */
dwg_add_DIMENSION_LINEAR (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                          const dwg_point_3d *restrict xline1_pt,
                          const dwg_point_3d *restrict xline2_pt,
                          const dwg_point_3d *restrict def_pt,
                          const double rotation_angle)
{
  API_ADD_PREP (DIMENSION_LINEAR);
  if (dwg->header.version <= R_2_22)
    {
      LOG_ERROR ("Invalid entity %s <r2.22", "DIMENSION_LINEAR")
      return NULL;
    }
  dwg_require_DIMSTYLE_Standard (dwg);
  API_ADD_ENTITY2 (DIMENSION_LINEAR);
  DIMENSION_DEFAULTS;
  ADD_CHECK_3DPOINT (xline1_pt);
  ADD_CHECK_3DPOINT (xline2_pt);
  ADD_CHECK_3DPOINT (def_pt);
  _obj->def_pt.x = def_pt->x; // dimline_pt
  _obj->def_pt.y = def_pt->y;
  _obj->def_pt.z = def_pt->z;
  _obj->xline1_pt.x = xline1_pt->x;
  _obj->xline1_pt.y = xline1_pt->y;
  _obj->xline1_pt.z = xline1_pt->z;
  _obj->xline2_pt.x = xline2_pt->x;
  _obj->xline2_pt.y = xline2_pt->y;
  _obj->xline2_pt.z = xline2_pt->z;
  _obj->dim_rotation = rotation_angle;
  ADD_CHECK_ANGLE (_obj->dim_rotation);
  // TODO calc oblique_angle
  if (dwg->header.version <= R_12)
    {
      obj->type = DWG_TYPE_DIMENSION_r11;
      obj->tio.entity->flag_r11 = 64 + FLAG_R11_DIMENSION_LINEAR;
    }
  return _obj;
}

EXPORT Dwg_Entity_POINT *
dwg_add_POINT (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
               const dwg_point_3d *restrict pt)
{
  API_ADD_ENTITY (POINT);
  ADD_CHECK_3DPOINT (pt);
  _obj->x = pt->x;
  _obj->y = pt->y;
  _obj->z = pt->z;
  if (dwg->header.version <= R_12)
    {
      obj->type = DWG_TYPE_POINT_r11;
      if (_obj->z == 0.0)
        obj->tio.entity->flag_r11 |= FLAG_R11_HAS_ELEVATION;
    }
  return _obj;
}

EXPORT Dwg_Entity__3DFACE *
dwg_add_3DFACE (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                const dwg_point_3d *restrict pt1,
                const dwg_point_3d *restrict pt2,
                const dwg_point_3d *restrict pt3,
                const dwg_point_3d *restrict pt4 /* may be NULL */)
{
  API_ADD_ENTITY (_3DFACE);
  if (dwg->header.version <= R_2_22)
    {
      LOG_ERROR ("Invalid entity %s <r2.22", "3DFACE")
      API_UNADD_ENTITY;
      return NULL;
    }
  ADD_CHECK_3DPOINT (pt1);
  ADD_CHECK_3DPOINT (pt2);
  ADD_CHECK_3DPOINT (pt3);
  _obj->corner1.x = pt1->x;
  _obj->corner1.y = pt1->y;
  _obj->corner1.z = pt1->z;
  if (dwg->header.version < R_10 && pt1->z != 0.0)
    obj->tio.entity->opts_r11 |= 1;
  _obj->corner2.x = pt2->x;
  _obj->corner2.y = pt2->y;
  _obj->corner2.z = pt2->z;
  if (dwg->header.version < R_10 && pt2->z != 0.0)
    obj->tio.entity->opts_r11 |= 2;
  _obj->corner3.x = pt3->x;
  _obj->corner3.y = pt3->y;
  _obj->corner3.z = pt3->z;
  if (dwg->header.version < R_10 && pt3->z != 0.0)
    obj->tio.entity->opts_r11 |= 3;
  if (pt4)
    {
      ADD_CHECK_3DPOINT (pt4);
      _obj->corner4.x = pt4->x;
      _obj->corner4.y = pt4->y;
      _obj->corner4.z = pt4->z;
      if (dwg->header.version < R_10 && pt4->z != 0.0)
        obj->tio.entity->opts_r11 |= 4;
    }
  else
    {
      _obj->corner4.x = pt3->x;
      _obj->corner4.y = pt3->y;
      _obj->corner4.z = pt3->z;
      if (dwg->header.version < R_10 && pt3->z != 0.0)
        obj->tio.entity->opts_r11 |= 4;
    }
  if (pt1->z == 0.0 && pt2->z == 0.0 && pt3->z == 0.0
      && (!pt4 || pt4->z == 0.0))
    {
      _obj->z_is_zero = 1;
      obj->tio.entity->flag_r11 |= FLAG_R11_HAS_ELEVATION;
    }
  if (dwg->header.version <= R_12)
    obj->type = DWG_TYPE_3DFACE_r11;

  _obj->has_no_flags = 1; // set invis_flags extra
  return _obj;
}

EXPORT Dwg_Entity_SOLID *
dwg_add_SOLID (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
               const dwg_point_3d *restrict pt1,
               const dwg_point_2d *restrict pt2,
               const dwg_point_2d *restrict pt3,
               const dwg_point_2d *restrict pt4)
{
  API_ADD_ENTITY (SOLID);
  ADD_CHECK_3DPOINT (pt1);
  ADD_CHECK_2DPOINT (pt2);
  ADD_CHECK_2DPOINT (pt3);
  ADD_CHECK_2DPOINT (pt4);
  _obj->corner1.x = pt1->x;
  _obj->corner1.y = pt1->y;
  _obj->elevation = pt1->z;
  _obj->corner2.x = pt2->x;
  _obj->corner2.y = pt2->y;
  _obj->corner3.x = pt3->x;
  _obj->corner3.y = pt3->y;
  _obj->corner4.x = pt4->x;
  _obj->corner4.y = pt4->y;
  if (dwg->header.version <= R_12)
    {
      obj->type = DWG_TYPE_SOLID_r11;
      if (_obj->elevation != 0.0)
        obj->tio.entity->opts_r11 |= 2;
    }
  return _obj;
}

EXPORT Dwg_Entity_TRACE *
dwg_add_TRACE (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
               const dwg_point_3d *restrict pt1,
               const dwg_point_2d *restrict pt2,
               const dwg_point_2d *restrict pt3,
               const dwg_point_2d *restrict pt4)
{
  API_ADD_ENTITY (TRACE);
  ADD_CHECK_3DPOINT (pt1);
  ADD_CHECK_2DPOINT (pt2);
  ADD_CHECK_2DPOINT (pt3);
  ADD_CHECK_2DPOINT (pt4);
  _obj->corner1.x = pt1->x;
  _obj->corner1.y = pt1->y;
  _obj->elevation = pt1->z;
  _obj->corner2.x = pt2->x;
  _obj->corner2.y = pt2->y;
  _obj->corner3.x = pt3->x;
  _obj->corner3.y = pt3->y;
  _obj->corner4.x = pt4->x;
  _obj->corner4.y = pt4->y;
  if (dwg->header.version <= R_12)
    {
      obj->type = DWG_TYPE_TRACE_r11;
      if (_obj->elevation == 0.0)
        obj->tio.entity->flag_r11 |= FLAG_R11_HAS_ELEVATION;
    }
  return _obj;
}

EXPORT Dwg_Entity_SHAPE *
dwg_add_SHAPE (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
               const char *restrict name, const dwg_point_3d *restrict ins_pt,
               const double scale, const double oblique_angle)
{
  API_ADD_ENTITY (SHAPE);
  ADD_CHECK_3DPOINT (ins_pt);
  ADD_CHECK_DOUBLE (scale);
  _obj->ins_pt.x = ins_pt->x;
  _obj->ins_pt.y = ins_pt->y;
  _obj->ins_pt.z = ins_pt->z;
  _obj->scale = scale;
  _obj->oblique_angle = oblique_angle;
  ADD_CHECK_ANGLE (_obj->oblique_angle);
  _obj->width_factor = 1.0;
  if (dwg->header_vars.TEXTSTYLE && dwg->header_vars.TEXTSTYLE->absolute_ref)
    {
      _obj->style = dwg_add_handleref (
          dwg, 5, dwg->header_vars.TEXTSTYLE->absolute_ref, NULL);
      _obj->style_id = 1; // TODO STYLE_CONTROL index + 1
    }
  else
    _obj->style_id = 1; // Standard
  if (dwg->header.version <= R_12)
    {
      obj->type = DWG_TYPE_SHAPE_r11;
      if (_obj->ins_pt.z == 0.0)
        obj->tio.entity->flag_r11 |= FLAG_R11_HAS_ELEVATION;
    }
  return _obj;
}

EXPORT Dwg_Entity_VIEWPORT *
dwg_add_VIEWPORT (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                  const char *restrict name)
{
  Dwg_Object_VX_TABLE_RECORD *vx = NULL;
  API_ADD_PREP (VIEWPORT);
  if (dwg->header.version < R_11)
    {
      LOG_ERROR ("Invalid entity %s <r11", "VIEWPORT")
      return NULL;
    }
  if (dwg->header.version < R_2004)
    {
      vx = dwg_add_VX (dwg, name);
    }
  API_ADD_ENTITY2 (VIEWPORT);
  if (dwg->header.version < R_2004 && vx)
    {
      Dwg_Object *vxobj = dwg_obj_generic_to_object (vx, &error);
      vx->is_on = 1;
      // FIXME vxobj->tio.object->ownerhandle
      vx->viewport = dwg_add_handleref (dwg, 4, obj->handle.value, NULL);
      _obj->vport_entity_header = dwg_add_handleref (dwg, 5, vxobj ? vxobj->handle.value : 0, NULL);
    }
  // TODO get defaults from name
  _obj->lens_length = 50.0;
  _obj->VIEWDIR.z = 1.0;
  _obj->center.x = _obj->VIEWCTR.x = 133.349991;
  _obj->center.y = _obj->VIEWCTR.y = 101.599997;
  _obj->width = 428.291613;
  _obj->height = _obj->VIEWSIZE = 228.422194;
  _obj->circle_zoom = 1000;
  _obj->status_flag = 32800;
  _obj->GRIDUNIT.x = 10.0;
  _obj->GRIDUNIT.y = 10.0;
  _obj->SNAPUNIT.x = 10.0;
  _obj->SNAPUNIT.y = 10.0;
  _obj->UCSVP = 1;
  _obj->ucsxdir.x = 1.0;
  _obj->ucsydir.y = 1.0;
  if (dwg->header.version <= R_12)
    obj->type = DWG_TYPE_VIEWPORT_r11;
  return _obj;
}

EXPORT Dwg_Entity_ELLIPSE *
dwg_add_ELLIPSE (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                 const dwg_point_3d *restrict center, const double major_axis,
                 const double axis_ratio)
{
  API_ADD_ENTITY (ELLIPSE);
  if (dwg->header.version <= R_12)
    {
      LOG_ERROR ("Invalid entity %s <r13", "3DSOLID")
      API_UNADD_ENTITY;
      return NULL;
    }
  ADD_CHECK_3DPOINT (center);
  ADD_CHECK_DOUBLE (major_axis);
  ADD_CHECK_DOUBLE (axis_ratio); // Only (0 - 1], ie. RadiusRatio
  _obj->center.x = center->x;
  _obj->center.y = center->y;
  _obj->center.z = center->z;
  _obj->sm_axis.x = major_axis;
  _obj->sm_axis.y = major_axis;
  _obj->sm_axis.z
      = center->z; // Error 150 (eGeneralModelingFailure) when not co-planar
  _obj->axis_ratio = axis_ratio;
  if (axis_ratio > 1.0 || axis_ratio <= 0.0)
    {
      LOG_ERROR ("Illegal ELLIPSE.axis_ratio %f. Set to 1.0", axis_ratio);
      _obj->axis_ratio = 1.0;
    }
  if (major_axis == 0.0)
    {
      LOG_ERROR ("Illegal ELLIPSE.major_axis 0.0, needs to be != 0");
      return NULL;
    }
  _obj->end_angle = M_PI * 2.0;
  return _obj;
}

EXPORT Dwg_Entity_SPLINE *
dwg_add_SPLINE (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                const int num_fit_pts, const dwg_point_3d *restrict fit_pts,
                const dwg_point_3d *restrict beg_tan_vec,
                const dwg_point_3d *restrict end_tan_vec)
{
  API_ADD_ENTITY (SPLINE);
  if (dwg->header.version <= R_12)
    {
      LOG_ERROR ("Invalid entity %s <r13", "SPLINE")
      API_UNADD_ENTITY;
      return NULL;
    }
  ADD_CHECK_3DPOINT (beg_tan_vec);
  ADD_CHECK_3DPOINT (end_tan_vec);
  _obj->beg_tan_vec.x = beg_tan_vec->x;
  _obj->beg_tan_vec.y = beg_tan_vec->y;
  _obj->beg_tan_vec.z = beg_tan_vec->z;
  _obj->end_tan_vec.x = end_tan_vec->x;
  _obj->end_tan_vec.y = end_tan_vec->y;
  _obj->end_tan_vec.z = end_tan_vec->z;
  _obj->num_fit_pts = (BITCODE_BL)num_fit_pts;
  assert (sizeof (BITCODE_3BD) == sizeof (dwg_point_3d));
  _obj->fit_pts = (BITCODE_3BD *)MALLOC (num_fit_pts * sizeof (BITCODE_3BD));
  if (!_obj->fit_pts)
    {
      LOG_ERROR ("%s: Out of memory", __FUNCTION__)
    }
  else
    {
      memcpy (_obj->fit_pts, fit_pts, num_fit_pts * sizeof (BITCODE_3BD));
    }
  return _obj;
}

EXPORT Dwg_Entity_REGION *
dwg_add_REGION (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                const char *acis_data)
{
  const size_t len = strlen (acis_data);
  unsigned j;
  int acis_data_idx = 0;
  API_ADD_ENTITY (REGION);
  if (dwg->header.version <= R_12)
    {
      LOG_ERROR ("Invalid entity %s <r13", "REGION")
      API_UNADD_ENTITY;
      return NULL;
    }
  _obj->num_blocks = (BITCODE_BL)((len / 4096) & 0xFFFFFFFF);
  if (len % 4096)
    _obj->num_blocks++;
  j = _obj->num_blocks;
  _obj->acis_data = (BITCODE_RC *)STRDUP (acis_data);
  _obj->block_size = (BITCODE_BL *)CALLOC (j + 1, sizeof (BITCODE_BL));
  _obj->encr_sat_data = (char **)CALLOC (j + 1, sizeof (char *));
  _obj->version = 1;
  _obj->unknown = 1;
  for (unsigned i = 0; i < _obj->num_blocks; i++)
    {
      if (i == j - 1)
        _obj->block_size[i] = len % 4096;
      else
        _obj->block_size[i] = 4096;
      _obj->encr_sat_data[i]
          = dwg_encrypt_SAT1 (_obj->block_size[i],
                              &_obj->acis_data[acis_data_idx], &acis_data_idx);
    }
  return _obj;
}

EXPORT Dwg_Entity__3DSOLID *
dwg_add_3DSOLID (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                 const char *acis_data)
{
  const size_t len = strlen (acis_data);
  unsigned j;
  int acis_data_idx = 0;
  API_ADD_ENTITY (_3DSOLID);
  if (dwg->header.version <= R_12)
    {
      LOG_ERROR ("Invalid entity %s <r13", "_3DSOLID")
      API_UNADD_ENTITY;
      return NULL;
    }
  _obj->num_blocks = (BITCODE_BL)((len / 4096) & 0xFFFFFFFF);
  if (len % 4096)
    _obj->num_blocks++;
  j = _obj->num_blocks;
  _obj->acis_data = (BITCODE_RC *)STRDUP (acis_data);
  _obj->block_size = (BITCODE_BL *)CALLOC (j + 1, sizeof (BITCODE_BL));
  _obj->encr_sat_data = (char **)CALLOC (j + 1, sizeof (char *));
  _obj->version = 1;
  _obj->unknown = 1;
  for (unsigned i = 0; i < j; i++)
    {
      if (i == j - 1)
        _obj->block_size[i] = len % 4096;
      else
        _obj->block_size[i] = 4096;
      _obj->encr_sat_data[i]
          = dwg_encrypt_SAT1 (_obj->block_size[i],
                              &_obj->acis_data[acis_data_idx], &acis_data_idx);
    }
  //_obj->encr_sat_data[j] = NULL;
  //_obj->block_size[j] = 0;
  return _obj;
}

EXPORT Dwg_Entity_BODY *
dwg_add_BODY (Dwg_Object_BLOCK_HEADER *restrict blkhdr, const char *acis_data)
{
  const size_t len = strlen (acis_data);
  unsigned j;
  int acis_data_idx = 0;
  API_ADD_ENTITY (BODY);
  if (dwg->header.version <= R_12)
    {
      LOG_ERROR ("Invalid entity %s <r13", "BODY")
      API_UNADD_ENTITY;
      return NULL;
    }
  _obj->num_blocks = (BITCODE_BL)((len / 4096) & 0xFFFFFFFF);
  if (len % 4096)
    _obj->num_blocks++;
  j = _obj->num_blocks;
  _obj->acis_data = (BITCODE_RC *)STRDUP (acis_data);
  _obj->block_size = (BITCODE_BL *)CALLOC (j + 1, sizeof (BITCODE_BL));
  _obj->encr_sat_data = (char **)CALLOC (j + 1, sizeof (char *));
  _obj->version = 1;
  _obj->unknown = 1;
  for (unsigned i = 0; i < _obj->num_blocks; i++)
    {
      if (i == j - 1)
        _obj->block_size[i] = len % 4096;
      else
        _obj->block_size[i] = 4096;
      _obj->encr_sat_data[i]
          = dwg_encrypt_SAT1 (_obj->block_size[i],
                              &_obj->acis_data[acis_data_idx], &acis_data_idx);
    }
  return _obj;
}

EXPORT Dwg_Entity_RAY *
dwg_add_RAY (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
             const dwg_point_3d *restrict point,
             const dwg_point_3d *restrict vector) /* different to VBA */
{
  API_ADD_ENTITY (RAY);
  if (dwg->header.version <= R_12)
    {
      LOG_ERROR ("Invalid entity %s <r13", "RAY")
      API_UNADD_ENTITY;
      return NULL;
    }
  ADD_CHECK_3DPOINT (point);
  ADD_CHECK_3DPOINT (vector);
  _obj->point.x = point->x;
  _obj->point.y = point->y;
  _obj->point.z = point->z;
  dwg_geom_normalize ((dwg_point_3d *)&_obj->vector, *vector);
  return _obj;
}

EXPORT Dwg_Entity_XLINE *
dwg_add_XLINE (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
               const dwg_point_3d *restrict point,
               const dwg_point_3d *restrict vector) /* different to VBA */
{
  API_ADD_ENTITY (XLINE);
  if (dwg->header.version <= R_12)
    {
      LOG_ERROR ("Invalid entity %s <r13", "XLINE")
      API_UNADD_ENTITY;
      return NULL;
    }
  ADD_CHECK_3DPOINT (point);
  ADD_CHECK_3DPOINT (vector);
  _obj->point.x = point->x;
  _obj->point.y = point->y;
  _obj->point.z = point->z;
  dwg_geom_normalize ((dwg_point_3d *)&_obj->vector, *vector);
  return _obj;
}

/* The name is the NOD entry. On NULL this is the NOD 0.1.C ("Named Object
 * Dictionary") */
EXPORT Dwg_Object_DICTIONARY *
dwg_add_DICTIONARY (Dwg_Data *restrict dwg,
                    const char *restrict name, /* the NOD entry */
                    const char *restrict key,  /* maybe NULL */
                    const BITCODE_RLL absolute_ref)
{
  Dwg_Object *nod;
  API_ADD_OBJECT (DICTIONARY);
  if (key && strNEc (key, "NAMED_OBJECT")) // the initial NOD is empty
    {
      _obj->numitems = 1;
      _obj->texts = (BITCODE_T *)CALLOC (1, sizeof (BITCODE_T));
      _obj->itemhandles = (BITCODE_H *)CALLOC (1, sizeof (BITCODE_H));
      _obj->texts[0] = dwg_add_u8_input (dwg, key);
      _obj->itemhandles[0] = dwg_add_handleref (dwg, 2, absolute_ref, NULL);
    }
  if (name)
    {
      nod = dwg_get_first_object (dwg, DWG_TYPE_DICTIONARY);
      if (nod)
        {
          dwg_add_DICTIONARY_item (nod->tio.object->tio.DICTIONARY, name,
                                   obj->handle.value);
          /* owner is relative, reactor absolute */
          obj->tio.object->ownerhandle
              = dwg_add_handleref (dwg, 4, nod->handle.value, obj);
          if (!obj->tio.object->num_reactors)
            add_obj_reactor (obj->tio.object, nod->handle.value);
          _obj->cloning = 1;
        }
    }
  else /* not a direct NOD item */
    {
      obj->tio.object->ownerhandle = dwg_add_handleref (dwg, 4, 0, NULL);
      _obj->cloning = 1;
    }
  return _obj;
}

EXPORT Dwg_Object_DICTIONARY *
dwg_add_DICTIONARY_item (Dwg_Object_DICTIONARY *_obj, const char *restrict key,
                         const BITCODE_RLL absolute_ref)
{
  int error;
  Dwg_Object *obj = dwg_obj_generic_to_object (_obj, &error);
  Dwg_Data *dwg = (obj && !error) ? obj->parent : NULL;
  if (!dwg)
    {
      LOG_ERROR ("dwg_add_DICTIONARY_item: no obj from DICTIONARY");
      return NULL;
    }
  if (!_obj->numitems)
    {
      _obj->texts = (char **)CALLOC (1, sizeof (BITCODE_T));
      _obj->itemhandles = (BITCODE_H *)CALLOC (1, sizeof (BITCODE_H));
    }
  else
    {
      // check if text already exists, and if so just replace handle.
      for (unsigned i = 0; i < _obj->numitems; i++)
        {
          char **texts = _obj->texts;
          BITCODE_H *hdlv = _obj->itemhandles;
          if (!hdlv || !texts || !texts[i])
            continue;
          if (IS_FROM_TU_DWG (dwg))
            {
              if (bit_eq_TU (key, (BITCODE_TU)texts[i]))
                {
                  _obj->itemhandles[i]
                      = dwg_add_handleref (dwg, 2, absolute_ref, NULL);
                  LOG_TRACE ("replace DICTIONARY_item %s=> " FORMAT_REF "\n",
                             key, ARGS_REF (_obj->itemhandles[i]));
                  return _obj;
                }
            }
          else
            {
              if (strEQ (key, texts[i]))
                {
                  _obj->itemhandles[i]
                      = dwg_add_handleref (dwg, 2, absolute_ref, NULL);
                  LOG_TRACE ("replace DICTIONARY_item %s => " FORMAT_REF "\n",
                             key, ARGS_REF (_obj->itemhandles[i]));
                  return _obj;
                }
            }
        }
      // not found:
      _obj->texts = (char **)REALLOC (_obj->texts, (_obj->numitems + 1)
                                                       * sizeof (BITCODE_T));
      _obj->itemhandles = (BITCODE_H *)REALLOC (
          _obj->itemhandles, (_obj->numitems + 1) * sizeof (BITCODE_H));
    }

  _obj->texts[_obj->numitems] = dwg_add_u8_input (dwg, key);
  _obj->itemhandles[_obj->numitems]
      = dwg_add_handleref (dwg, 2, absolute_ref, NULL);
  LOG_TRACE ("add DICTIONARY_item %s => " FORMAT_REF "\n", key,
             ARGS_REF (_obj->itemhandles[_obj->numitems]));
  _obj->numitems++;
  return _obj;
}

// dwg_add_DICTIONARYWDFLT (dwg, "Normal", handle (0xF));
EXPORT Dwg_Object_DICTIONARYWDFLT *
dwg_add_DICTIONARYWDFLT (Dwg_Data *restrict dwg,
                         const char *restrict name, /* the NOD entry */
                         const char *restrict key,  /* maybe NULL */
                         const BITCODE_RLL absolute_ref)
{
  Dwg_Object *nod;
  {
    REQUIRE_CLASS ("ACDBDICTIONARYWDFLT");
  }
  {
    API_ADD_OBJECT (DICTIONARYWDFLT);
    if (key)
      {
        _obj->numitems = 1;
        _obj->texts = (BITCODE_T *)CALLOC (1, sizeof (BITCODE_T));
        _obj->itemhandles = (BITCODE_H *)CALLOC (1, sizeof (BITCODE_H));
        _obj->texts[0] = dwg_add_u8_input (dwg, key);
        _obj->itemhandles[0] = dwg_add_handleref (dwg, 2, absolute_ref, NULL);
      }
    if (absolute_ref)
      {
        _obj->cloning = 1;
        _obj->defaultid = dwg_add_handleref (dwg, 5, absolute_ref, obj);
      }

    if (name)
      {
        nod = dwg_get_first_object (dwg, DWG_TYPE_DICTIONARY);
        if (nod)
          {
            dwg_add_DICTIONARY_item (nod->tio.object->tio.DICTIONARY, name,
                                     obj->handle.value);
            /* owner is relative, reactor absolute */
            obj->tio.object->ownerhandle
                = dwg_add_handleref (dwg, 4, nod->handle.value, obj);
            if (!obj->tio.object->num_reactors)
              add_obj_reactor (obj->tio.object, nod->handle.value);
          }
      }
    else /* not a direct NOD item */
      {
        obj->tio.object->ownerhandle = dwg_add_handleref (dwg, 4, 0, NULL);
        _obj->cloning = 1;
      }

    return _obj;
  }
}

EXPORT Dwg_Entity_OLE2FRAME *
dwg_add_OLE2FRAME (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                   const dwg_point_3d *restrict pt1,
                   const dwg_point_3d *restrict pt2)
{
  // const Dwg_Object_Ref *pspace =  dwg_paper_space_ref (dwg);
  API_ADD_ENTITY (OLE2FRAME);
  if (dwg->header.version <= R_12)
    {
      LOG_ERROR ("Invalid entity %s <r13", "OLE2FRAME")
      API_UNADD_ENTITY;
      return NULL;
    }
  _obj->pt1.x = pt1->x;
  _obj->pt1.y = pt1->y;
  _obj->pt1.z = pt1->z;
  _obj->pt2.x = pt2->x;
  _obj->pt2.y = pt2->y;
  _obj->pt2.z = pt2->z;
  _obj->oleversion = 2;
  if (strEQ (blkhdr->name, "*PAPER_SPACE"))
    _obj->mode = 1;
  return _obj;
}

EXPORT Dwg_Entity_MTEXT *
dwg_add_MTEXT (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
               const dwg_point_3d *restrict ins_pt, const double rect_width,
               const char *restrict text)
{
  API_ADD_ENTITY (MTEXT);
  _obj->text = dwg_add_u8_input (dwg, text);
  ADD_CHECK_3DPOINT (ins_pt);
  ADD_CHECK_DOUBLE (rect_width);
  _obj->ins_pt.x = ins_pt->x;
  _obj->ins_pt.y = ins_pt->y;
  _obj->ins_pt.z = ins_pt->z;
  _obj->rect_width = rect_width;
  // defaults:
  _obj->x_axis_dir.x = 1.0;
  _obj->linespace_style = 1;
  _obj->linespace_factor = 1.0;
  _obj->text_height = dwg->header_vars.TEXTSIZE;
  _obj->extents_height = dwg->header_vars.TEXTSIZE;
  _obj->extents_width = _obj->rect_width;
  _obj->attachment = 1;
  _obj->flow_dir = 1;
  if (dwg->header_vars.TEXTSTYLE)
    _obj->style = dwg_add_handleref (
        dwg, 5, dwg->header_vars.TEXTSTYLE->absolute_ref, NULL);
  return _obj;
}

EXPORT Dwg_Entity_LEADER *
dwg_add_LEADER (
    Dwg_Object_BLOCK_HEADER *restrict blkhdr, const unsigned num_points,
    const dwg_point_3d *restrict points,
    const Dwg_Entity_MTEXT *restrict associated_annotation, /* maybe NULL */
    const unsigned type)
{
  BITCODE_H annotative = NULL;
  Dwg_Object_DIMSTYLE *annot_style = NULL;
  Dwg_Object *annot_o = NULL;
  API_ADD_PREP (LEADER);
  if (!num_points)
    {
      LOG_ERROR ("no num_points")
      return NULL;
    }
  if (dwg->header.version <= R_12)
    {
      LOG_ERROR ("Invalid entity %s <r13", "LEADER")
      return NULL;
    }
  // dimstyles need to be created before the entity
  if (associated_annotation)
    {
      annot_o = dwg_obj_generic_to_object (associated_annotation, &error);
      if (error || !annot_o || annot_o->fixedtype != DWG_TYPE_MTEXT)
        {
          LOG_ERROR ("Invalid associated_annotation object");
          return NULL;
        }
      // use DIMSTYLE "Annotative"
      annotative = dwg_find_tablehandle (dwg, "Annotative", "DIMSTYLE");
      if (!annotative)
        { // create it
          annot_style = dwg_add_DIMSTYLE (dwg, (const BITCODE_T) "Annotative");
        }
    }
  
  API_ADD_ENTITY2 (LEADER);
  _obj->points = (BITCODE_3BD *)CALLOC (num_points, sizeof (BITCODE_3BD));
  _obj->num_points = num_points;
  for (unsigned i = 0; i < num_points; i++)
    {
      ADD_CHECK_DOUBLE (points[i].x);
      ADD_CHECK_DOUBLE (points[i].y);
      ADD_CHECK_DOUBLE (points[i].z);
      _obj->points[i].x = points[i].x;
      _obj->points[i].y = points[i].y;
      _obj->points[i].z = points[i].z;
    }
  _obj->origin.x = points[0].x;
  _obj->origin.y = points[0].y;
  _obj->origin.z = points[0].z;
  // TODO type => path_type + annot_type + arrowhead_on
  // TODO check more valid types
  // defaults:
  _obj->x_direction.x = 1.0;
  if (!_obj->dimstyle && dwg->header_vars.DIMSTYLE)
    _obj->dimstyle = dwg_add_handleref (
        dwg, 5, dwg->header_vars.DIMSTYLE->absolute_ref, NULL);
  _obj->dimgap = dwg->header_vars.DIMGAP;
  _obj->box_height = dwg->header_vars.DIMTXT;
  _obj->endptproj.y = -(_obj->box_height / 2.0);
  // TODO more calcs ...
  _obj->box_width = 0.82;
  _obj->arrowhead_type = 8;
  if (associated_annotation)
    {
      _obj->annot_type = 1;
      _obj->associated_annotation = dwg_add_handleref (
          dwg, 5, dwg_obj_generic_handlevalue ((void *)associated_annotation),
          obj);
      add_obj_reactor (annot_o->tio.object, obj->handle.value);
      // use DIMSTYLE "Annotative"
      if (annotative)
        _obj->dimstyle
            = dwg_add_handleref (dwg, 5, annotative->absolute_ref, NULL);
      else if (annot_style) // created above
        _obj->dimstyle = dwg_add_handleref (
            dwg, 5, dwg_obj_generic_handlevalue (annot_style), NULL);
    }
  return _obj;
}

EXPORT Dwg_Entity_TOLERANCE *
dwg_add_TOLERANCE (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                   const char *restrict text_value,
                   const dwg_point_3d *restrict ins_pt,
                   const dwg_point_3d *restrict x_direction /* maybe NULL */)
{
  API_ADD_ENTITY (TOLERANCE);
  _obj->text_value = dwg_add_u8_input (dwg, text_value);
  if (dwg->header.version <= R_12)
    {
      LOG_ERROR ("Invalid entity %s <r13", "TOLERANCE")
      API_UNADD_ENTITY;
      return NULL;
    }
  ADD_CHECK_3DPOINT (ins_pt);
  _obj->ins_pt.x = ins_pt->x;
  _obj->ins_pt.y = ins_pt->y;
  _obj->ins_pt.z = ins_pt->z;
  if (x_direction)
    {
      ADD_CHECK_3DPOINT (x_direction);
      dwg_geom_normalize ((dwg_point_3d *)&_obj->x_direction, *x_direction);
    }
  else
    _obj->x_direction.x = 1.0;
  // defaults:
  if (dwg->header_vars.DIMSTYLE)
    _obj->dimstyle = dwg_add_handleref (
        dwg, 5, dwg->header_vars.DIMSTYLE->absolute_ref, NULL);
  _obj->dimgap = dwg->header_vars.DIMGAP;
  return _obj;
}

EXPORT Dwg_Entity_MLINE *
dwg_add_MLINE (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
               const unsigned num_verts, const dwg_point_3d *restrict verts)
{
  BITCODE_H mlstyref;
  Dwg_Object_MLINESTYLE *mlstyle = NULL;
  API_ADD_ENTITY (MLINE);
  if (dwg->header.version <= R_12)
    {
      LOG_ERROR ("Invalid entity %s <r13", "MLINE")
      API_UNADD_ENTITY;
      return NULL;
    }
  if (!num_verts)
    return NULL;
  _obj->verts
      = (Dwg_MLINE_vertex *)CALLOC (num_verts, sizeof (Dwg_MLINE_vertex));
  _obj->num_verts = num_verts;
  _obj->base_point.x = verts[0].x;
  _obj->base_point.y = verts[0].y;
  _obj->base_point.z = verts[0].z;
  // defaults:
  _obj->scale = dwg->header_vars.CMLSCALE;
  _obj->extrusion.z = 1.0;
  // flags?
  // set current mlinestyle
  mlstyref = dwg->header_vars.CMLSTYLE;
  _obj->mlinestyle = dwg_add_handleref (dwg, 5, mlstyref->absolute_ref, NULL);
  obj = dwg_ref_object (dwg, mlstyref);
  if (obj)
    {
      mlstyle = obj->tio.object->tio.MLINESTYLE;
      _obj->num_lines = mlstyle->num_lines;
    }
  for (unsigned i = 0; i < num_verts; i++)
    {
      // dwg_point_3d pt, ext;
      BITCODE_3BD dir;
      unsigned next = i + 1;
      unsigned prev = i ? i - 1 : num_verts - 1;
      if (next == num_verts)
        next = 0;
      _obj->verts[i].parent = _obj;
      _obj->verts[i].vertex.x = verts[i].x;
      _obj->verts[i].vertex.y = verts[i].y;
      _obj->verts[i].vertex.z = verts[i].z;
      // calc vertex_direction
      dwg_geom_normalize ((dwg_point_3d *)&dir,
                          (dwg_point_3d){ verts[next].x - verts[i].x,
                                          verts[next].y - verts[i].y,
                                          verts[next].z - verts[i].z });
      _obj->verts[i].vertex_direction = dir;
      // FIXME miter_direction = seems to be the extrusion (=normal) of the
      // vertex_direction to calculate the rotation matrix, starting with the
      // start_angle.
      if (i == 0)
        {
#if defined(HAVE_SINCOS) && !defined(__clang__)
          double cosa, sina;
          sincos (mlstyle->start_angle, &sina, &cosa);
#else
          const double cosa = cos (mlstyle->start_angle);
          const double sina = sin (mlstyle->start_angle);
#endif
          dir = _obj->verts[i].vertex_direction;
          // rotate by the mlstyle->start_angle
          dir.x = (dir.x * cosa) - (dir.y * sina);
          dir.y = (dir.x * sina) + (dir.y * cosa);
          // dwg_geom_normalize ((dwg_point_3d
          // *)&_obj->verts[i].miter_direction, dir);
          _obj->verts[i].miter_direction = dir; // already normalized
        }
      else
        {
          // TODO v[prev]->vert_dir, v[i]->vert_dir, extrusion, v[0]->miter_dir
          dwg_point_3d ax, ay, az;
          memcpy (&az, &dir, sizeof (dwg_point_3d));
          if ((fabs (az.x) < 1 / 64.0) && (fabs (az.y) < 1 / 64.0))
            {
              dwg_point_3d tmp = { 0.0, 1.0, 0.0 };
              dwg_geom_cross (&tmp, tmp, az);
              dwg_geom_normalize (&ax, tmp);
            }
          else
            {
              dwg_point_3d tmp = { 0.0, 0.0, 1.0 };
              dwg_geom_cross (&tmp, tmp, az);
              dwg_geom_normalize (&ax, tmp);
            }
          dwg_geom_cross (&ay, az, ax);
          dwg_geom_normalize (&ay, ay);
          memcpy (&dir, &ay, sizeof (dwg_point_3d));
          _obj->verts[i].miter_direction = dir;
        }
      _obj->verts[i].lines = (Dwg_MLINE_line *)CALLOC (
          _obj->num_lines, sizeof (Dwg_MLINE_line));
      for (unsigned j = 0; j < _obj->num_lines; j++)
        {
          _obj->verts[i].lines[j].parent = _obj->verts;
          _obj->verts[i].lines[j].num_segparms = 2;
          _obj->verts[i].lines[j].segparms
              = (BITCODE_BD *)CALLOC (_obj->verts[i].lines[j].num_segparms, 8);
          for (unsigned k = 0; k < _obj->verts[i].lines[j].num_segparms; k++)
            { // TODO
              _obj->verts[i].lines[j].segparms[k] = 0.0;
            }
        }
    }
  return _obj;
}

// Tables:

/* This is a singleton and must be at 1. Must only be called from
 * dwg_add_Document() */
EXPORT Dwg_Object_BLOCK_CONTROL *
dwg_add_BLOCK_CONTROL (Dwg_Data *restrict dwg, const unsigned ms,
                       const unsigned ps)
{
  API_ADD_OBJECT (BLOCK_CONTROL);
  // obj->handle.value = 1;
  // obj->handle.size = 1;
  // dwg->block_control = _obj; // but will be realloc'ed soon
  dwg->header_vars.BLOCK_CONTROL_OBJECT
      = dwg_add_handleref (dwg, 3, UINT64_C (1), obj);
  dwg->header_vars.BLOCK_CONTROL_OBJECT->obj = obj;
  if (ms)
    {
      // Usually this does not exist yet.
      _obj->model_space = dwg_add_handleref (dwg, 3, (BITCODE_RLL)ms, obj);
      dwg->header_vars.BLOCK_RECORD_MSPACE = _obj->model_space;
      LOG_TRACE ("blkctrl.model_space = " FORMAT_REF "\n",
                 ARGS_REF (_obj->model_space));
    }
  if (ps)
    {
      // This neither
      _obj->paper_space = dwg_add_handleref (dwg, 3, (BITCODE_RLL)ps, obj);
      dwg->header_vars.BLOCK_RECORD_PSPACE = _obj->paper_space;
      LOG_TRACE ("blkctrl.paper_space = " FORMAT_REF "\n",
                 ARGS_REF (_obj->paper_space));
    }
  dwg->block_control = *_obj;
  return _obj;
}

#define API_ADD_TABLE(record, control, ...)                                   \
  Dwg_Object_##record *_record = NULL;                                        \
  /* first check TABLE_CONTROL */                                             \
  Dwg_Object *ctrl = dwg_get_first_object (dwg, DWG_TYPE_##control);          \
  Dwg_Object_##control *_ctrl;                                                \
  BITCODE_RLL ctrlhdl, ctrlidx;                                               \
  if (name && !dwg_is_valid_name_u8 (dwg, name))                              \
    {                                                                         \
      LOG_WARN ("Invalid symbol table record name \"%s\"\n", name);           \
      /*return NULL;*/                                                        \
    }                                                                         \
  if (!ctrl || !ctrl->tio.object || !ctrl->tio.object->tio.control)           \
    {                                                                         \
      API_ADD_OBJECT (control);                                               \
      dwg->header_vars.control##_OBJECT                                       \
          = dwg_add_handleref (dwg, 3, obj->handle.value, obj);               \
      LOG_TRACE (#control "_OBJECT = " FORMAT_REF "\n",                       \
                 ARGS_REF (dwg->header_vars.control##_OBJECT));               \
      dwg->header_vars.control##_OBJECT->obj = obj;                           \
      ctrl = obj;                                                             \
      _ctrl = _obj;                                                           \
    }                                                                         \
  else                                                                        \
    {                                                                         \
      _ctrl = ctrl->tio.object->tio.control;                                  \
    }                                                                         \
  ctrlhdl = ctrl->handle.value;                                               \
  ctrlidx = ctrl->index;                                                      \
  if (name)                                                                   \
    {                                                                         \
      API_ADD_OBJECT (record);                                                \
      _record = _obj;                                                         \
      _obj->used = -1;                                                        \
      _obj->name = dwg_add_u8_input (dwg, name);                              \
      LOG_TRACE (#record ".name = %s\n", _obj->name);                         \
      ctrl = &dwg->object[ctrlidx];                                           \
      ctrl->size = 0;                                                         \
      ctrl->bitsize = 0;                                                      \
      __VA_ARGS__                                                             \
      if (_ctrl->entries)                                                     \
        _ctrl->entries = (BITCODE_H *)REALLOC (                               \
            _ctrl->entries, (_ctrl->num_entries + 1) * sizeof (BITCODE_H));   \
      else                                                                    \
        _ctrl->entries = (BITCODE_H *)CALLOC (_ctrl->num_entries + 1,         \
                                              sizeof (BITCODE_H));            \
      _ctrl->entries[_ctrl->num_entries]                                      \
          = dwg_add_handleref (dwg, 2, obj->handle.value, NULL);              \
      LOG_TRACE (#control ".entries[%d] = " FORMAT_REF "\n",                  \
                 _ctrl->num_entries,                                          \
                 ARGS_REF (_ctrl->entries[_ctrl->num_entries]));              \
      _ctrl->num_entries++;                                                   \
      obj->tio.object->ownerhandle                                            \
          = dwg_add_handleref (dwg, 4, ctrlhdl, obj);                         \
      LOG_TRACE (#record ".ownerhandle = " FORMAT_REF "\n",                  \
                 ARGS_REF (obj->tio.object->ownerhandle));                    \
      _obj->is_xref_ref = 1;                                                  \
      return _obj;                                                            \
    }                                                                         \
  else                                                                        \
    return NULL

EXPORT Dwg_Object_BLOCK_HEADER *
dwg_add_BLOCK_HEADER (Dwg_Data *restrict dwg, const char *restrict name)
{
  API_ADD_TABLE (BLOCK_HEADER, BLOCK_CONTROL);
  dwg->block_control = *_ctrl;
  return _record;
}

EXPORT Dwg_Object_LAYER *
dwg_add_LAYER (Dwg_Data *restrict dwg, const char *restrict name)
{
  if (dwg->header.version >= R_2000)
    {
      // PLOTSTYLE placeholder
      API_ADD_TABLE (LAYER, LAYER_CONTROL, {
        _obj->plotstyle = dwg_add_handleref (dwg, 5, UINT64_C (0xF), NULL);
        _obj->plotflag = 1;
        _obj->linewt = 0x1f;
      });
    }
  else
    {
      API_ADD_TABLE (LAYER, LAYER_CONTROL);
    }
}

EXPORT Dwg_Object_STYLE *
dwg_add_STYLE (Dwg_Data *restrict dwg, const char *restrict name)
{
  API_ADD_TABLE (STYLE, STYLE_CONTROL, { _obj->width_factor = 1.0; });
}

EXPORT Dwg_Object_LTYPE *
dwg_add_LTYPE (Dwg_Data *restrict dwg, const char *restrict name)
{
  API_ADD_TABLE (LTYPE, LTYPE_CONTROL, { _obj->flag = 64; _obj->alignment = 0x41; });
}

EXPORT Dwg_Object_VIEW *
dwg_add_VIEW (Dwg_Data *restrict dwg, const char *restrict name)
{
  API_ADD_TABLE (VIEW, VIEW_CONTROL, {
    _obj->lens_length = 50.0;
    _obj->VIEWDIR.z = 1.0;
    _obj->VIEWMODE = 1;
    _obj->VIEWSIZE = 13314.951254;
    _obj->view_width = 19003.498504;
    _obj->VIEWCTR.x = 5771.997570;
    _obj->VIEWCTR.y = 789.325613;
    _obj->ucsxdir.x = 1.0;
    _obj->ucsydir.y = 1.0;
  });
}

EXPORT Dwg_Object_VPORT *
dwg_add_VPORT (Dwg_Data *restrict dwg, const char *restrict name)
{
  API_ADD_TABLE (VPORT, VPORT_CONTROL, {
    _obj->lens_length = 50.0;
    _obj->VIEWDIR.z = 1.0;
    _obj->VIEWMODE = 1;
    _obj->VIEWSIZE = 13314.951254;
    _obj->view_width = 19003.498504;
    _obj->VIEWCTR.x = 5771.997570;
    _obj->VIEWCTR.y = 789.325613;
    _obj->upper_right.x = 1.0;
    _obj->upper_right.y = 1.0;
    _obj->circle_zoom = 1000;
    _obj->FASTZOOM = 1;
    _obj->UCSICON = 3;
    _obj->GRIDUNIT.x = 10.0;
    _obj->GRIDUNIT.y = 10.0;
    _obj->SNAPUNIT.x = 10.0;
    _obj->SNAPUNIT.y = 10.0;
    _obj->UCSVP = 1;
    _obj->ucsxdir.x = 1.0;
    _obj->ucsydir.y = 1.0;
  });
}

// This is normally called automatically, not needed to set by the user.
EXPORT Dwg_Object_APPID *
dwg_add_APPID (Dwg_Data *restrict dwg, const char *restrict name)
{
  API_ADD_TABLE (APPID, APPID_CONTROL);
}

EXPORT Dwg_Object_DIMSTYLE *
dwg_add_DIMSTYLE (Dwg_Data *restrict dwg, const char *restrict name)
{
  if (name
      && strNE (name, dwg->header.version < R_13 ? "STANDARD" : "Standard"))
    dwg_require_DIMSTYLE_Standard (dwg);
  {
    API_ADD_TABLE (DIMSTYLE, DIMSTYLE_CONTROL, {
      _obj->DIMTIH = 1;
      _obj->DIMTOH = 1;
      _obj->DIMALTD = 2;
      _obj->DIMTOLJ = 1;
      _obj->DIMFIT = 3;
      _obj->DIMUNIT = 2;
      _obj->DIMDEC = 4;
      _obj->DIMTDEC = 4;
      _obj->DIMALTU = 2;
      _obj->DIMALTTD = 2;
      _obj->DIMLUNIT = 2;
      _obj->DIMATFIT = 3;
      _obj->DIMLWD = -2;
      _obj->DIMLWE = -2;
      _obj->DIMSCALE = strEQc (name, "Annotative") ? 0.0 : 1.0;
      _obj->DIMASZ = 0.18;
      _obj->DIMEXO = 0.0625;
      _obj->DIMDLI = 0.38;
      _obj->DIMEXE = 0.18;
      _obj->DIMTXT = 0.18;
      _obj->DIMCEN = 0.09;
      _obj->DIMALTF = 25.4;
      _obj->DIMLFAC = 1.0;
      _obj->DIMTFAC = 1.0;
      _obj->DIMGAP = 0.09;
      _obj->DIMCLRD = (BITCODE_CMC){ 0, CMC_DEFAULTS };
      _obj->DIMCLRE = (BITCODE_CMC){ 0, CMC_DEFAULTS };
      _obj->DIMCLRT = (BITCODE_CMC){ 0, CMC_DEFAULTS };
      if (dwg->header_vars.TEXTSTYLE)
        _obj->DIMTXSTY = dwg_add_handleref (
            dwg, 5, dwg->header_vars.TEXTSTYLE->absolute_ref, NULL);
    });
  }
}

EXPORT Dwg_Object_UCS *
dwg_add_UCS (Dwg_Data *restrict dwg, const dwg_point_3d *restrict origin,
             const dwg_point_3d *restrict x_axis,
             const dwg_point_3d *restrict y_axis, const char *restrict name)
{
  API_ADD_TABLE (UCS, UCS_CONTROL, {
    if (origin)
      {
        _obj->ucsorg.x = origin->x;
        _obj->ucsorg.y = origin->y;
        _obj->ucsorg.z = origin->z;
      }
    if (x_axis)
      {
        _obj->ucsxdir.x = x_axis->x;
        _obj->ucsxdir.y = x_axis->y;
        _obj->ucsxdir.z = x_axis->z;
      }
    if (y_axis)
      {
        _obj->ucsydir.x = y_axis->x;
        _obj->ucsydir.y = y_axis->y;
        _obj->ucsydir.z = y_axis->z;
      }
  });
}

// VX_CONTROL
// VX_TABLE_RECORD
// only r11-r2000
EXPORT Dwg_Object_VX_TABLE_RECORD *
dwg_add_VX (Dwg_Data *restrict dwg, const char *restrict name /* maybe NULL */)
{
  API_ADD_TABLE (VX_TABLE_RECORD, VX_CONTROL);
}

EXPORT Dwg_Object_GROUP *
dwg_add_GROUP (Dwg_Data *restrict dwg,
               const char *restrict name /* maybe NULL */)
{
  Dwg_Object_DICTIONARY *dict = NULL;
  Dwg_Object *dictobj;
  Dwg_Object_Ref *groupdict;
  Dwg_Object *nod = dwg_get_first_object (dwg, DWG_TYPE_DICTIONARY);
  if (name && !dwg_is_valid_name_u8 (dwg, name))
    LOG_WARN ("Invalid name \"%s\"\n", name);
  // return NULL;
  {
    API_ADD_OBJECT (GROUP);
    if (dwg->header.version <= R_12)
      {
        LOG_ERROR ("Invalid entity %s <r13", "GROUP");
        API_UNADD_ENTITY;
        return NULL;
      }
    // find nod dict
    groupdict = dwg_ctrl_table (dwg, "GROUP");
    if (!groupdict)
      {
        dict = dwg_add_DICTIONARY (dwg, (const BITCODE_T) "ACAD_GROUP", name,
                                   obj->handle.value);
      }
    else
      {
        Dwg_Object *group = dwg_ref_object (dwg, groupdict);
        if (group)
          dict = dwg_add_DICTIONARY_item (obj->tio.object->tio.DICTIONARY,
                                          (const BITCODE_T) "ACAD_GROUP",
                                          group->handle.value);
      }
    if (dict)
      {
        dictobj = dwg_obj_generic_to_object (dict, &error);
        obj->tio.object->ownerhandle
            = dwg_add_handleref (dwg, 4, dictobj->handle.value, NULL);
        obj->tio.object->ownerhandle->obj = NULL;
        add_obj_reactor (obj->tio.object, dictobj->handle.value);
      }

    _obj->selectable = 1;
    if (name)
      _obj->name = dwg_add_u8_input (dwg, name);
    else
      _obj->unnamed = 1;
    return _obj;
  }
}

EXPORT Dwg_Object_MLINESTYLE *
dwg_add_MLINESTYLE (Dwg_Data *restrict dwg, const char *restrict name)
{
  Dwg_Object_DICTIONARY *dict;
  Dwg_Object_Ref *dictref;
  if (!dwg_is_valid_name_u8 (dwg, name))
    LOG_WARN ("Invalid name \"%s\"\n", name);
  // return NULL;
  {
    API_ADD_OBJECT (MLINESTYLE);
    // find nod dict
    dictref = dwg_find_dictionary (dwg, "ACAD_MLINESTYLE");
    if (!dictref)
      {
        dict = dwg_add_DICTIONARY (dwg, (const BITCODE_T) "ACAD_MLINESTYLE",
                                   name, obj->handle.value);
        if (dict)
          {
            obj->tio.object->ownerhandle = dwg_add_handleref (
                dwg, 4, dwg_obj_generic_handlevalue (dict), obj);
            if (!obj->tio.object->num_reactors)
              add_obj_reactor (obj->tio.object,
                               dwg_obj_generic_handlevalue (dict));
          }
      }
    else
      {
        Dwg_Object *dictobj = dwg_ref_object (dwg, dictref);
        if (dictobj)
          {
            dwg_add_DICTIONARY_item (dictobj->tio.object->tio.DICTIONARY, name,
                                     obj->handle.value);
            obj->tio.object->ownerhandle
                = dwg_add_handleref (dwg, 4, dictobj->handle.value, obj);
            if (!obj->tio.object->num_reactors)
              add_obj_reactor (obj->tio.object, dictobj->handle.value);
          }
      }

    _obj->name = strEQc (name, "Standard") ? dwg_add_u8_input (dwg, "Standard")
                                           : dwg_add_u8_input (dwg, name);
    _obj->fill_color = (BITCODE_CMC){ 256, CMC_DEFAULTS };
    if (strEQc (name, "Standard") || strEQc (name, "STANDARD"))
      {
        _obj->start_angle = _obj->end_angle = deg2rad (90.0);
        _obj->num_lines = 2;
        _obj->lines
            = (Dwg_MLINESTYLE_line *)CALLOC (2, sizeof (Dwg_MLINESTYLE_line));
        _obj->lines[0].parent = _obj;
        _obj->lines[0].offset = 0.5;
        _obj->lines[0].color = (BITCODE_CMC){ 256, CMC_DEFAULTS };
        _obj->lines[0].parent = _obj;
        _obj->lines[1].offset = -0.5;
        _obj->lines[1].color = (BITCODE_CMC){ 256, CMC_DEFAULTS };
        if (dwg->header.version >= R_2018)
          {
            _obj->lines[0].lt.ltype = NULL; // FIXME
            _obj->lines[1].lt.ltype = NULL;
          }
        else
          {
            _obj->lines[0].lt.index = 32767;
            _obj->lines[1].lt.index = 32767;
          }
      }
    return _obj;
  }
}

// OLE2FRAME
// DUMMY
// LONG_TRANSACTION

EXPORT Dwg_Entity_LWPOLYLINE *
dwg_add_LWPOLYLINE (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                    const int num_pts2d, const dwg_point_2d *restrict pts2d)
{
  {
    int err;
    Dwg_Object *hdr = dwg_obj_generic_to_object (blkhdr, &err);
    Dwg_Data *dwg = hdr ? hdr->parent : NULL;
    if (dwg && dwg->header.version < R_2000)
      REQUIRE_CLASS ("LWPOLYLINE");
    if (dwg && dwg->header.version <= R_12)
      {
        LOG_ERROR ("Invalid entity %s <r13", "LWPOLYLINE")
        return NULL;
      }
  }
  {
    API_ADD_ENTITY (LWPOLYLINE);
    error = dwg_ent_lwpline_set_points (_obj, num_pts2d, pts2d);
    return _obj;
  }
}

// GCC33_DIAG_IGNORE (-Wswitch-enum)
EXPORT Dwg_Entity_HATCH *
dwg_add_HATCH (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
               const int pattern_type, const char *restrict name,
               const bool is_associative, const unsigned num_paths,
               // Line, Polyline, Circle, Arc, Ellipse, Spline or Region
               const Dwg_Object **pathobjs)
{
  {
    int err;
    Dwg_Object *hdr = dwg_obj_generic_to_object (blkhdr, &err);
    Dwg_Data *dwg = hdr ? hdr->parent : NULL;
    if (dwg && dwg->header.version < R_2000)
      REQUIRE_CLASS ("HATCH");
    if (dwg && dwg->header.version <= R_12)
      {
        LOG_ERROR ("Invalid entity %s <r13", "HATCH")
        return NULL;
      }
  }
  {
    API_ADD_ENTITY (HATCH);
    if (strEQc (name, "SPHERICAL") || strEQc (name, "HEMISPHERICAL")
        || strEQc (name, "CURVED") || strEQc (name, "LINEAR")
        || strEQc (name, "CYLINDER"))
      {
        _obj->is_gradient_fill = 1;
        _obj->gradient_name = dwg_add_u8_input (dwg, name);
      }
    else
      {
        _obj->name = dwg_add_u8_input (dwg, name);
        // predefined (acad.pat), user-defined (ltype), custom (some.pat file)
        _obj->pattern_type = pattern_type;
      }
    if (strEQc (_obj->name, "SOLID"))
      _obj->is_solid_fill = 1;
    _obj->is_associative = is_associative;
    _obj->scale_spacing = 1.0;
    //_obj->x_dir.z = 1.0;
    _obj->num_paths = num_paths;
    //_obj->num_boundary_handles = num_paths;
    //_obj->boundary_handles = CALLOC (1, sizeof (BITCODE_H));
    _obj->paths
        = (Dwg_HATCH_Path *)CALLOC (num_paths, sizeof (Dwg_HATCH_Path));
    for (unsigned i = 0; i < num_paths; i++)
      {
        Dwg_Object_Type type = pathobjs[i]->fixedtype;
        if (type != DWG_TYPE_LINE && type != DWG_TYPE_ARC
            && type != DWG_TYPE_CIRCLE && type != DWG_TYPE_ELLIPSE
            && type != DWG_TYPE_POLYLINE_2D && type != DWG_TYPE_LWPOLYLINE
            && type != DWG_TYPE_SPLINE && type != DWG_TYPE_REGION)
          {
            LOG_ERROR (
                "Invalid HATCH.path[%d] object type %s. Only accept "
                "Line, Polyline, Circle, Arc, Ellipse, Spline or Region",
                i, dwg_type_name (type));
            return NULL;
          }
        _obj->paths[i].parent = _obj;
        // one path per object only here
        _obj->paths[i].num_boundary_handles = 1;
        _obj->paths[i].boundary_handles
            = (BITCODE_H *)CALLOC (1, sizeof (BITCODE_H));
        _obj->paths[i].boundary_handles[0]
            = dwg_add_handleref (dwg, 4, pathobjs[i]->handle.value, NULL);
        if (is_associative)
          add_ent_reactor (pathobjs[i]->tio.entity, obj->handle.value);
        // Split geometry into paths per pathobject
        switch (type)
          {
          case DWG_TYPE_LINE:
            {
              Dwg_Entity_LINE *line = pathobjs[i]->tio.entity->tio.LINE;
              _obj->paths[i].flag = 1 + (is_associative ? 0x200 : 0);
              _obj->paths[i].num_segs_or_paths = 1;
              _obj->paths[i].segs = (Dwg_HATCH_PathSeg *)CALLOC (
                  1, sizeof (Dwg_HATCH_PathSeg));
              _obj->paths[i].segs[0].parent = &_obj->paths[i];
              _obj->paths[i].segs[0].curve_type = 1;
              _obj->paths[i].segs[0].first_endpoint.x = line->start.x;
              _obj->paths[i].segs[0].first_endpoint.y = line->start.y;
              _obj->paths[i].segs[0].second_endpoint.x = line->end.x;
              _obj->paths[i].segs[0].second_endpoint.y = line->end.y;
              break;
            }
          case DWG_TYPE_ARC:
            {
              Dwg_Entity_ARC *arc = pathobjs[i]->tio.entity->tio.ARC;
              _obj->paths[i].flag
                  = 0x21 + (is_associative ? 0x200 : 0); // is_open
              _obj->paths[i].num_segs_or_paths = 1;
              _obj->paths[i].segs = (Dwg_HATCH_PathSeg *)CALLOC (
                  1, sizeof (Dwg_HATCH_PathSeg));
              _obj->paths[i].segs[0].parent = &_obj->paths[i];
              _obj->paths[i].segs[0].curve_type = 2;
              _obj->paths[i].segs[0].center.x = arc->center.x;
              _obj->paths[i].segs[0].center.y = arc->center.y;
              _obj->paths[i].segs[0].radius = arc->radius;
              _obj->paths[i].segs[0].start_angle = arc->start_angle;
              _obj->paths[i].segs[0].end_angle = arc->end_angle;
              _obj->paths[i].segs[0].is_ccw = 1;
              break;
            }
          case DWG_TYPE_CIRCLE:
            {
              Dwg_Entity_CIRCLE *arc = pathobjs[i]->tio.entity->tio.CIRCLE;
              _obj->paths[i].flag = 1 + (is_associative ? 0x200 : 0);
              _obj->paths[i].num_segs_or_paths = 1;
              _obj->paths[i].segs = (Dwg_HATCH_PathSeg *)CALLOC (
                  1, sizeof (Dwg_HATCH_PathSeg));
              _obj->paths[i].segs[0].parent = &_obj->paths[i];
              _obj->paths[i].segs[0].curve_type = 2;
              _obj->paths[i].segs[0].center.x = arc->center.x;
              _obj->paths[i].segs[0].center.y = arc->center.y;
              _obj->paths[i].segs[0].radius = arc->radius;
              _obj->paths[i].segs[0].start_angle = 0;
              _obj->paths[i].segs[0].end_angle = M_PI * 2;
              _obj->paths[i].segs[0].is_ccw = 1; //?
              break;
            }
          case DWG_TYPE_ELLIPSE:
            {
              Dwg_Entity_ELLIPSE *ell = pathobjs[i]->tio.entity->tio.ELLIPSE;
              _obj->paths[i].flag = 1 + (is_associative ? 0x200 : 0);
              _obj->paths[i].num_segs_or_paths = 1;
              _obj->paths[i].segs = (Dwg_HATCH_PathSeg *)CALLOC (
                  1, sizeof (Dwg_HATCH_PathSeg));
              _obj->paths[i].segs[0].parent = &_obj->paths[i];
              _obj->paths[i].segs[0].curve_type = 3;
              _obj->paths[i].segs[0].center.x = ell->center.x;
              _obj->paths[i].segs[0].center.y = ell->center.y;
              _obj->paths[i].segs[0].endpoint.x = ell->sm_axis.x;
              _obj->paths[i].segs[0].endpoint.y = ell->sm_axis.y;
              _obj->paths[i].segs[0].minor_major_ratio = ell->axis_ratio;
              _obj->paths[i].segs[0].start_angle = ell->start_angle;
              _obj->paths[i].segs[0].end_angle = ell->end_angle;
              _obj->paths[i].segs[0].is_ccw = 1; //?
              break;
            }
          case DWG_TYPE_SPLINE:
          case DWG_TYPE_REGION:
            LOG_WARN ("Path segment extraction for HATCH from %s not yet "
                      "implemented",
                      dwg_type_name (type));
            break;
          case DWG_TYPE_LWPOLYLINE:
            {
              bool has_bulges = false;
              Dwg_Entity_LWPOLYLINE *pline
                  = pathobjs[i]->tio.entity->tio.LWPOLYLINE;
              // If no bulges just use flag 1 as with LINE?? (but flag 2 would
              // be much more compact) I have seen LWPOLYLINE's without bulges
              // being HATCHed to segments.
              if (pline->num_bulges && pline->flag & 16)
                {
                  for (unsigned j = 0; j < pline->num_bulges; j++)
                    {
                      if (pline->bulges[j] != 0.0)
                        {
                          has_bulges = true;
                          break;
                        }
                    }
                }
              if (!has_bulges) // use the segment path
                {
                  unsigned num;
                  _obj->paths[i].flag = 1 + (is_associative ? 0x200 : 0);
                  if (pline->flag & 512) // closed
                    num = pline->num_points;
                  else
                    {
                      _obj->paths[i].flag += 0x20;
                      num = pline->num_points - 1;
                    }
                  _obj->paths[i].num_segs_or_paths = num;
                  _obj->paths[i].segs = (Dwg_HATCH_PathSeg *)CALLOC (
                      num, sizeof (Dwg_HATCH_PathSeg));
                  for (unsigned j = 0; j < num; j++)
                    {
                      unsigned k = j + 1;
                      if (k == pline->num_points)
                        k = 0;
                      _obj->paths[i].segs[j].parent = &_obj->paths[i];
                      _obj->paths[i].segs[j].curve_type = 1;
                      _obj->paths[i].segs[j].first_endpoint.x
                          = pline->points[j].x;
                      _obj->paths[i].segs[j].first_endpoint.y
                          = pline->points[j].y;
                      _obj->paths[i].segs[j].second_endpoint.x
                          = pline->points[k].x;
                      _obj->paths[i].segs[j].second_endpoint.y
                          = pline->points[k].y;
                    }
                }
              else
                {
                  _obj->paths[i].flag = 2 + (is_associative ? 0x200 : 0);
                  _obj->paths[i].bulges_present = 1;
                  _obj->paths[i].closed = pline->flag & 512 ? 1 : 0;
                  _obj->paths[i].num_segs_or_paths = pline->num_points;
                  _obj->paths[i].polyline_paths
                      = (Dwg_HATCH_PolylinePath *)CALLOC (
                          pline->num_points, sizeof (Dwg_HATCH_PolylinePath));
                  for (unsigned j = 0; j < pline->num_points; j++)
                    {
                      _obj->paths[i].polyline_paths[j].parent
                          = &_obj->paths[i];
                      _obj->paths[i].polyline_paths[j].point.x
                          = pline->points[j].x;
                      _obj->paths[i].polyline_paths[j].point.y
                          = pline->points[j].y;
                      if (_obj->paths[i].bulges_present)
                        _obj->paths[i].polyline_paths[j].bulge
                            = pline->bulges[j];
                    }
                }
            }
            break;
          case DWG_TYPE_POLYLINE_2D:
            {
              Dwg_Entity_POLYLINE_2D *pline
                  = pathobjs[i]->tio.entity->tio.POLYLINE_2D;
              dwg_point_2d *pts;
              _obj->paths[i].flag = 2 + (is_associative ? 0x200 : 0);
              _obj->paths[i].closed = pline->flag & 1 ? 1 : 0;
              _obj->paths[i].num_segs_or_paths = pline->num_owned;
              _obj->paths[i].polyline_paths
                  = (Dwg_HATCH_PolylinePath *)CALLOC (
                      pline->num_owned, sizeof (Dwg_HATCH_PolylinePath));
              pts = dwg_object_polyline_2d_get_points (pathobjs[i], &error);
              if (error)
                return NULL;
              for (unsigned j = 0; j < pline->num_owned; j++)
                {
                  _obj->paths[i].polyline_paths[j].parent = &_obj->paths[i];
                  _obj->paths[i].polyline_paths[j].point.x = pts[j].x;
                  _obj->paths[i].polyline_paths[j].point.y = pts[j].y;
                }
              // TODO bulges, curve_type
              FREE (pts);
            }
            break;
          default:
            LOG_ERROR (
                "Invalid HATCH.path[%d] object type %s. Only accept "
                "Line, Polyline, Circle, Arc, Ellipse, Spline or Region",
                i, dwg_type_name (type));
            return NULL;
          }
      }
    return _obj;
  }
}

EXPORT Dwg_Object_XRECORD *
dwg_add_XRECORD (Dwg_Object_DICTIONARY *restrict dict,
                 const char *restrict key)
{
  int err;
  Dwg_Object *dictobj = dwg_obj_generic_to_object (dict, &err);
  Dwg_Data *dwg = dictobj->parent;
  if (dictobj->fixedtype
      != DWG_TYPE_DICTIONARY) // allow WDFLT? not seen in the wild
    {
      LOG_ERROR ("Object XRECORD must be added to a DICTIONARY, not %s",
                 dwg_type_name (dictobj->fixedtype));
      return NULL;
    }
  {
    if (dwg->header.version < R_2000)
      REQUIRE_CLASS ("XRECORD");
  }
  {
    API_ADD_OBJECT (XRECORD);
    _obj->cloning = dict->cloning;
    // find the key in the dict, and set the handle. or add it
    dwg_add_DICTIONARY_item (dict, key, obj->handle.value);
    return _obj;
  }
}

static Dwg_Resbuf *
rbuf_last (Dwg_Resbuf *rbuf)
{
  Dwg_Resbuf *prev = rbuf;
  while (rbuf)
    {
      prev = rbuf;
      rbuf = rbuf->nextrb;
    }
  return prev;
}

static Dwg_Resbuf *
rbuf_add (Dwg_Resbuf *rbuf)
{
  rbuf = rbuf_last (rbuf);
  if (!rbuf)
    {
      return (Dwg_Resbuf *)CALLOC (1, sizeof (Dwg_Resbuf));
    }
  else
    {
      rbuf->nextrb = (Dwg_Resbuf *)CALLOC (1, sizeof (Dwg_Resbuf));
      return rbuf->nextrb;
    }
}

#define CHECK_XRECORD                                                         \
  int error;                                                                  \
  Dwg_Object *obj = dwg_obj_generic_to_object (_obj, &error);                 \
  if (!obj || obj->fixedtype != DWG_TYPE_XRECORD)                             \
    {                                                                         \
      LOG_ERROR ("Not a XRECORD, but %s",                                     \
                 obj ? dwg_type_name (obj->fixedtype) : "NULL");              \
      return NULL;                                                            \
    }

EXPORT Dwg_Object_XRECORD *
dwg_add_XRECORD_bool (Dwg_Object_XRECORD *restrict _obj, const short dxf,
                      const BITCODE_B value)
{
  Dwg_Resbuf *rbuf;
  CHECK_XRECORD;
  rbuf = rbuf_add (_obj->xdata);
  if (!_obj->xdata)
    _obj->xdata = rbuf;
  _obj->num_xdata++;
  rbuf->type = dxf;
  rbuf->value.i8 = value;
  _obj->xdata_size += 3;
  return _obj;
}

EXPORT Dwg_Object_XRECORD *
dwg_add_XRECORD_int8 (Dwg_Object_XRECORD *restrict _obj, const short dxf,
                      const BITCODE_RC value)
{
  int error;
  Dwg_Resbuf *rbuf;
  Dwg_Object *obj = dwg_obj_generic_to_object (_obj, &error);
  if (obj->fixedtype != DWG_TYPE_XRECORD)
    {
      LOG_ERROR ("Not a XRECORD, but %s", dwg_type_name (obj->fixedtype));
      return NULL;
    }
  rbuf = rbuf_add (_obj->xdata);
  if (!_obj->xdata)
    _obj->xdata = rbuf;
  _obj->num_xdata++;
  rbuf->type = dxf;
  rbuf->value.i8 = value;
  _obj->xdata_size += 3; // 2 + 1
  return _obj;
}

EXPORT Dwg_Object_XRECORD *
dwg_add_XRECORD_int16 (Dwg_Object_XRECORD *restrict _obj, const short dxf,
                       const BITCODE_BS value)
{
  int error;
  Dwg_Resbuf *rbuf;
  Dwg_Object *obj = dwg_obj_generic_to_object (_obj, &error);
  if (obj->fixedtype != DWG_TYPE_XRECORD)
    {
      LOG_ERROR ("Not a XRECORD, but %s", dwg_type_name (obj->fixedtype));
      return NULL;
    }
  rbuf = rbuf_add (_obj->xdata);
  if (!_obj->xdata)
    _obj->xdata = rbuf;
  _obj->num_xdata++;
  rbuf->type = dxf;
  rbuf->value.i16 = value;
  _obj->xdata_size += 4; // 2 + 2
  return _obj;
}

EXPORT Dwg_Object_XRECORD *
dwg_add_XRECORD_int32 (Dwg_Object_XRECORD *restrict _obj, const short dxf,
                       const BITCODE_BL value)
{
  int error;
  Dwg_Resbuf *rbuf;
  Dwg_Object *obj = dwg_obj_generic_to_object (_obj, &error);
  if (!obj || obj->fixedtype != DWG_TYPE_XRECORD)
    {
      LOG_ERROR ("Not a XRECORD, but %s",
                 obj ? dwg_type_name (obj->fixedtype) : "NULL");
      return NULL;
    }
  rbuf = rbuf_add (_obj->xdata);
  if (!_obj->xdata)
    _obj->xdata = rbuf;
  _obj->num_xdata++;
  rbuf->type = dxf;
  rbuf->value.i32 = value;
  _obj->xdata_size += 6; // 2 + 4
  return _obj;
}

EXPORT Dwg_Object_XRECORD *
dwg_add_XRECORD_int64 (Dwg_Object_XRECORD *restrict _obj, const short dxf,
                       const BITCODE_BLL value)
{
  int err;
  Dwg_Resbuf *rbuf;
  Dwg_Object *obj = dwg_obj_generic_to_object (_obj, &err);
  if (obj->fixedtype != DWG_TYPE_XRECORD)
    {
      LOG_ERROR ("Not a XRECORD, but %s", dwg_type_name (obj->fixedtype));
      return NULL;
    }
  rbuf = rbuf_add (_obj->xdata);
  if (!_obj->xdata)
    _obj->xdata = rbuf;
  _obj->num_xdata++;
  rbuf->type = dxf;
  rbuf->value.i64 = value;
  _obj->xdata_size += 10; // 2 + 8
  return _obj;
}

EXPORT Dwg_Object_XRECORD *
dwg_add_XRECORD_real (Dwg_Object_XRECORD *restrict _obj, const short dxf,
                      const BITCODE_BD value)
{
  int err;
  Dwg_Resbuf *rbuf;
  Dwg_Object *obj = dwg_obj_generic_to_object (_obj, &err);
  if (obj->fixedtype != DWG_TYPE_XRECORD)
    {
      LOG_ERROR ("Not a XRECORD, but %s", dwg_type_name (obj->fixedtype));
      return NULL;
    }
  rbuf = rbuf_add (_obj->xdata);
  if (!_obj->xdata)
    _obj->xdata = rbuf;
  _obj->num_xdata++;
  rbuf->type = dxf;
  rbuf->value.dbl = value;
  _obj->xdata_size += 10; // 2 + 8
  return _obj;
}

EXPORT Dwg_Object_XRECORD *
dwg_add_XRECORD_pointd3d (Dwg_Object_XRECORD *restrict _obj, const short dxf,
                          const BITCODE_3DPOINT *pt)
{
  int err;
  Dwg_Resbuf *rbuf;
  Dwg_Object *obj = dwg_obj_generic_to_object (_obj, &err);
  if (obj->fixedtype != DWG_TYPE_XRECORD)
    {
      LOG_ERROR ("Not a XRECORD, but %s", dwg_type_name (obj->fixedtype));
      return NULL;
    }
  rbuf = rbuf_add (_obj->xdata);
  if (!_obj->xdata)
    _obj->xdata = rbuf;
  _obj->num_xdata++;
  rbuf->type = dxf;
  rbuf->value.pt[0] = pt->x;
  rbuf->value.pt[1] = pt->y;
  rbuf->value.pt[2] = pt->z;
  _obj->xdata_size += 26; // 2 + 3*8
  return _obj;
}

EXPORT Dwg_Object_XRECORD *
dwg_add_XRECORD_binary (Dwg_Object_XRECORD *restrict _obj, const short dxf,
                        const int size, const BITCODE_RC *data)
{
  int error;
  Dwg_Resbuf *rbuf;
  Dwg_Object *obj = dwg_obj_generic_to_object (_obj, &error);
  Dwg_Data *dwg = obj ? obj->parent : NULL;
  if (obj->fixedtype != DWG_TYPE_XRECORD)
    {
      LOG_ERROR ("Not a XRECORD, but %s", dwg_type_name (obj->fixedtype));
      return NULL;
    }
  rbuf = rbuf_add (_obj->xdata);
  if (!_obj->xdata)
    _obj->xdata = rbuf;
  _obj->num_xdata++;
  rbuf->type = dxf;
  rbuf->value.str.is_tu = 0;
  rbuf->value.str.u.data = (char *)MALLOC (size);
  if (!rbuf->value.str.u.data)
    {
      LOG_ERROR ("%s: Out of memory", __FUNCTION__)
      rbuf->value.str.size = 0;
      _obj->xdata_size += 3; // 2 + 1
    }
  else
    {
      rbuf->value.str.size = size;
      memcpy (rbuf->value.str.u.data, data, size);
      _obj->xdata_size += 3 + size; // 2 + 1 + len
    }
  return _obj;
}

EXPORT Dwg_Object_XRECORD *
dwg_add_XRECORD_string (Dwg_Object_XRECORD *restrict _obj, const short dxf,
                        const BITCODE_BS len,
                        const char *str) // utf8
{
  int error;
  Dwg_Resbuf *rbuf;
  Dwg_Object *obj = dwg_obj_generic_to_object (_obj, &error);
  Dwg_Data *dwg = obj ? obj->parent : NULL;
  if (obj->fixedtype != DWG_TYPE_XRECORD)
    {
      LOG_ERROR ("Not a XRECORD, but %s", dwg_type_name (obj->fixedtype));
      return NULL;
    }
  rbuf = rbuf_add (_obj->xdata);
  if (!_obj->xdata)
    _obj->xdata = rbuf;
  _obj->num_xdata++;
  rbuf->type = dxf;
  rbuf->value.str.codepage
      = (dwg && dwg->header.version < R_2007) ? dwg->header.codepage : 30;
  rbuf->value.str.is_tu = 0;
  rbuf->value.str.size = len;
  rbuf->value.str.u.data = (char *)MALLOC (len);
  memcpy (rbuf->value.str.u.data, str, len); // utf-8 or single-byte
  _obj->xdata_size += 4 + len;
  return _obj;
}

EXPORT Dwg_Object_XRECORD *
dwg_add_XRECORD_handle (Dwg_Object_XRECORD *restrict _obj, const short dxf,
                        const Dwg_Handle hdl)
{
  int error;
  Dwg_Resbuf *rbuf;
  Dwg_Object *obj = dwg_obj_generic_to_object (_obj, &error);
  Dwg_Data *dwg = obj ? obj->parent : NULL;
  if (obj->fixedtype != DWG_TYPE_XRECORD)
    {
      LOG_ERROR ("Not a XRECORD, but %s", dwg_type_name (obj->fixedtype));
      return NULL;
    }
  rbuf = rbuf_add (_obj->xdata);
  if (!_obj->xdata)
    _obj->xdata = rbuf;
  _obj->num_xdata++;
  rbuf->type = dxf;
  rbuf->value.absref = hdl.value;
  _obj->xdata_size += 10; // 2 + 8
  return _obj;
}

EXPORT Dwg_Object_PLACEHOLDER *
dwg_add_PLACEHOLDER (Dwg_Data *restrict dwg)
{
  {
    REQUIRE_CLASS ("ACDBPLACEHOLDER");
  }
  {
    API_ADD_OBJECT (PLACEHOLDER);
    return _obj;
  }
}

EXPORT Dwg_Object_VBA_PROJECT *
dwg_add_VBA_PROJECT (Dwg_Data *restrict dwg, const BITCODE_BL size,
                     const BITCODE_RC *data)
{
  if (dwg->header.version < R_2000)
    return NULL;
  {
    REQUIRE_CLASS ("VBA_PROJECT");
  }
  {
    API_ADD_OBJECT (VBA_PROJECT);
    _obj->data_size = size;
    // add the data to dwg->vbaproject, the SECTION_VBAPROJECT
    dwg->vbaproject.size = size;
    dwg->vbaproject.unknown_bits = (BITCODE_TF)MALLOC (size);
    // header.vbaproj_address is set in encode
    return _obj;
  }
}

/* either added to a p/mspace BLOCK_HEADER, to a VIEWPORT entity in pspace,
   or a VPORT object in mspace. */
EXPORT Dwg_Object_LAYOUT *
dwg_add_LAYOUT (Dwg_Object *restrict vp, const char *restrict name,
                const char *restrict canonical_media_name)
{
  int err;
  Dwg_Data *dwg = vp->parent;
#ifdef NEED_VPORT_FOR_MODEL_LAYOUT
  if (vp->fixedtype != DWG_TYPE_VPORT && vp->fixedtype != DWG_TYPE_VIEWPORT
      && vp->fixedtype != DWG_TYPE_BLOCK_HEADER)
    {
      LOG_ERROR ("LAYOUT can only be added to VPORT (in mspace) or "
                 "the *Paper_Space BLOCK_HEADER or VIEWPORT (in pspace)");
      return NULL;
    }
#endif
  {
    // TODL skip if <r2000
    REQUIRE_CLASS ("LAYOUT");
  }
  {
    Dwg_Object_DICTIONARY *dict = NULL;
    Dwg_Object *dictobj;
    Dwg_Object_Ref *dictref;
    Dwg_Object *nod;
    unsigned long ownerhandle = 0UL;

    API_ADD_OBJECT (LAYOUT);

    _obj->layout_name = dwg_add_u8_input (dwg, name);
    _obj->layout_flags = 1;
    _obj->plotsettings.canonical_media_name
        = dwg_add_u8_input (dwg, canonical_media_name);
    _obj->UCSXDIR.x = 1.0;
    _obj->UCSYDIR.y = 1.0;
    if (vp->fixedtype == DWG_TYPE_VIEWPORT)
      {
        _obj->LIMMAX = dwg->header_vars.PLIMMAX;
        _obj->EXTMIN = dwg->header_vars.PEXTMIN;
        _obj->EXTMAX = dwg->header_vars.PEXTMAX;
        _obj->UCSXDIR = dwg->header_vars.PUCSXDIR;
        _obj->UCSYDIR = dwg->header_vars.PUCSYDIR;
      }
    else
      {
        _obj->LIMMAX = dwg->header_vars.LIMMAX;
        _obj->EXTMIN = dwg->header_vars.EXTMIN;
        _obj->EXTMAX = dwg->header_vars.EXTMAX;
        _obj->UCSXDIR = dwg->header_vars.UCSXDIR;
        _obj->UCSYDIR = dwg->header_vars.UCSYDIR;
      }

    // either VIEWPORT or VPORT or to a pspace or mspace BLOCK_HEADER
    if (vp->fixedtype == DWG_TYPE_BLOCK_HEADER)
      {
        ownerhandle = vp->handle.value;
        _obj->active_viewport = dwg_add_handleref (dwg, 4, 0, NULL);
      }
    else if (vp->fixedtype == DWG_TYPE_VPORT)
      {
        ownerhandle = dwg->header_vars.BLOCK_RECORD_MSPACE->absolute_ref;
        _obj->active_viewport
            = dwg_add_handleref (dwg, 4, vp->handle.value, NULL);
      }
    else if (vp->fixedtype == DWG_TYPE_VIEWPORT)
      {
        // in pspace
        ownerhandle = vp->tio.entity->ownerhandle->absolute_ref;
        _obj->active_viewport
            = dwg_add_handleref (dwg, 4, vp->handle.value, NULL);
      }

    if (ownerhandle)
      _obj->block_header = dwg_add_handleref (dwg, 4, ownerhandle, NULL);
    // TODO copy the plotsettings and viewport settings as default

    dictref = dwg_find_dictionary (dwg, "ACAD_LAYOUT");
    if (dictref)
      {
        dictobj = dwg_ref_object (dwg, dictref);
        if (dictobj)
          dict = dwg_add_DICTIONARY_item (dictobj->tio.object->tio.DICTIONARY,
                                          name, obj->handle.value);
      }
    if (dict)
      {
        dictobj = dwg_obj_generic_to_object (dict, &error);
        obj->tio.object->ownerhandle
            = dwg_add_handleref (dwg, 4, dictobj->handle.value, obj);
        add_obj_reactor (obj->tio.object, dictobj->handle.value);
        if (!dwg->header_vars.DICTIONARY_LAYOUT
            || !dwg->header_vars.DICTIONARY_LAYOUT->absolute_ref)
          dwg->header_vars.DICTIONARY_LAYOUT
              = dwg_add_handleref (dwg, 5, dictobj->handle.value, NULL);
      }
    {
      // Attach to block_header also if empty. Only to *Paper_Space or
      // *Model_Space.
      Dwg_Object_Ref *blkref = _obj->block_header;
      Dwg_Object *blk = dwg_ref_object (dwg, blkref);
      if (!blk)
        return _obj;
      if (!blk->tio.object->tio.BLOCK_HEADER->layout
          || !blk->tio.object->tio.BLOCK_HEADER->layout->absolute_ref)
        blk->tio.object->tio.BLOCK_HEADER->layout
            = dwg_add_handleref (dwg, 5, obj->handle.value, NULL);
    }
    return _obj;
  }
}

EXPORT Dwg_Entity_PROXY_ENTITY *
dwg_add_PROXY_ENTITY (Dwg_Object_BLOCK_HEADER *restrict blkhdr /* ... */)
{
  {
    int err;
    Dwg_Object *hdr = dwg_obj_generic_to_object (blkhdr, &err);
    Dwg_Data *dwg = hdr ? hdr->parent : NULL;
    if (dwg && dwg->header.version < R_2000)
      REQUIRE_CLASS ("ACAD_PROXY_ENTITY_WRAPPER");
  }
  {
    API_ADD_ENTITY (PROXY_ENTITY);
    return _obj;
  }
}

// owned by a DICT: name: nod key, e.g. HOST_DOC_SETTINGS, key: e.g.
// NCDOCPARAMETERS
EXPORT Dwg_Object_PROXY_OBJECT *
dwg_add_PROXY_OBJECT (Dwg_Data *restrict dwg, char *name, char *key
                      /*, size, data */)
{
  Dwg_Object_DICTIONARY *dict;
  Dwg_Object *nod, *dictobj;
  {
    int error;
    REQUIRE_CLASS ("ACAD_PROXY_OBJECT_WRAPPER");

    // add name to NOD
    dict = dwg_add_DICTIONARY (dwg, name, key, 0);
    nod = dwg_get_first_object (dwg, DWG_TYPE_DICTIONARY);
    dictobj = dwg_obj_generic_to_object (dict, &error);
  }

  {
    API_ADD_OBJECT (PROXY_OBJECT);
    dwg_add_DICTIONARY_item (nod->tio.object->tio.DICTIONARY, key,
                             obj->handle.value);
    obj->tio.object->ownerhandle
        = dwg_add_handleref (dwg, 4, dictobj->handle.value, NULL);
    add_obj_reactor (obj->tio.object, dictobj->handle.value);
    _obj->class_id = 499;
    return _obj;
  }
}

// ACDSRECORD
// ACDSSCHEMA
// ACMECOMMANDHISTORY
// ACMESCOPE
// ACMESTATEMGR

// ACSH_BOOLEAN_CLASS
// Performs a Boolean operation (0 union, 1 intersect, or 2 subtract) between
// the object and another 3DSolid or Region object

EXPORT Dwg_Object_EVALUATION_GRAPH *
dwg_add_EVALUATION_GRAPH (Dwg_Data *restrict dwg, const int has_graph,
                          const int e_nodeid, const unsigned num_nodes,
                          const BITCODE_H *restrict evalexpr)
{
  API_ADD_OBJECT (EVALUATION_GRAPH);
  obj->tio.object->ownerhandle
      = dwg_add_handleref (dwg, 4, obj->handle.value + 1, obj);
  _obj->major = 27;
  _obj->minor = 52;
  _obj->has_graph = has_graph;
  _obj->first_nodeid = e_nodeid;
  _obj->first_nodeid_copy = e_nodeid;
  _obj->num_nodes = num_nodes;
  _obj->nodes
      = (Dwg_EVAL_Node *)CALLOC (_obj->num_nodes, sizeof (Dwg_EVAL_Node));
  for (unsigned i = 0; i < num_nodes; i++)
    {
      _obj->nodes[i].parent = _obj;
      _obj->nodes[i].id = i;
      _obj->nodes[i].edge_flags = 32;
      _obj->nodes[i].nextid = i + 1;
      _obj->nodes[i].evalexpr = evalexpr[i];
      _obj->nodes[i].node[0] = -1;
      _obj->nodes[i].node[1] = -1;
      _obj->nodes[i].node[2] = -1;
      _obj->nodes[i].node[3] = -1;
    }
  _obj->edges
      = (Dwg_EVAL_Edge *)CALLOC (_obj->num_edges, sizeof (Dwg_EVAL_Edge));
  for (unsigned i = 0; i < _obj->num_edges; i++)
    {
      _obj->edges[i].parent = _obj;
      _obj->edges[i].id = i;
      _obj->edges[i].nextid = -1;
      _obj->edges[i].e1 = -1; // incoming edges
      _obj->edges[i].e2 = -1;
      _obj->edges[i].e3 = -1;
      _obj->edges[i].out_edge[0] = -1;
      _obj->edges[i].out_edge[1] = -1;
      _obj->edges[i].out_edge[2] = -1;
      _obj->edges[i].out_edge[3] = -1;
      _obj->edges[i].out_edge[4] = -1;
    }
  return _obj;
}

EXPORT Dwg_Object_ACSH_HISTORY_CLASS *
dwg_add_ACSH_HISTORY_CLASS (Dwg_Entity_3DSOLID *restrict region,
                            const int h_nodeid)
{
  int err;
  Dwg_Object *hdr = dwg_obj_generic_to_object (region, &err);
  Dwg_Data *dwg = hdr ? hdr->parent : NULL;
  if (!dwg)
    return NULL;
  {
    API_ADD_OBJECT (ACSH_HISTORY_CLASS);
    obj->tio.object->ownerhandle = dwg_add_handleref (
        dwg, 4, dwg_obj_generic_handlevalue (region), obj);
    _obj->major = 27;
    _obj->minor = 52;
    _obj->h_nodeid = h_nodeid;
    _obj->record_history = 1;
    return _obj;
  }
}

/* Some geometric helpers */
EXPORT double
dwg_geom_angle_normalize (double angle)
{
  if (fabs (angle) > M_PI)
    {
      while (angle > M_PI)
        angle -= (M_PI * 2.0);
      while (angle < -M_PI)
        angle += (M_PI * 2.0);
    }
  return angle;
}

EXPORT dwg_point_3d *
dwg_geom_normalize (dwg_point_3d *out, const dwg_point_3d pt)
{
  double l = sqrt ((pt.x * pt.x) + (pt.y * pt.y) + (pt.z * pt.z));
  *out = pt;
  if (l != 1.0 && l != 0.0)
    {
      out->x = pt.x / l;
      out->y = pt.y / l;
      out->z = pt.z / l;
    }
  return out;
}

EXPORT dwg_point_3d *
dwg_geom_cross (dwg_point_3d *out, const dwg_point_3d pt1,
                const dwg_point_3d pt2)
{
  out->x = pt1.y * pt2.z - pt1.z * pt2.y;
  out->y = pt1.z * pt2.x - pt1.x * pt2.z;
  out->z = pt1.x * pt2.y - pt1.y * pt2.x;
  return out;
}

// Transform a 3D point via its OCS (extrusion or normal)
EXPORT dwg_point_3d *
dwg_geom_transform_OCS (dwg_point_3d *out, const dwg_point_3d pt,
                        const dwg_point_3d ext)
{
  if (ext.x == 0.0 && ext.y == 0.0 && ext.z == 1.0)
    {
      *out = pt;
    }
  else if (ext.x == 0.0 && ext.y == 0.0 && ext.z == -1.0)
    {
      *out = pt;
      out->x = -out->x;
    }
  else
    {
      /* This is called the "Arbitrary Axis Algorithm" to calculate
         the OCS x-axis from the extrusion z-vector */
      dwg_point_3d ax, ay, az, be;
      be = ext;
      dwg_geom_normalize (&az, be);
      if ((fabs (az.x) < 1 / 64.0) && (fabs (az.y) < 1 / 64.0))
        {
          dwg_point_3d tmp = { 0.0, 1.0, 0.0 };
          dwg_geom_cross (&tmp, tmp, az);
          dwg_geom_normalize (&ax, tmp);
        }
      else
        {
          dwg_point_3d tmp = { 0.0, 0.0, 1.0 };
          dwg_geom_cross (&tmp, tmp, az);
          dwg_geom_normalize (&ax, tmp);
        }
      dwg_geom_cross (&ay, az, ax);
      dwg_geom_normalize (&ay, ay);
      out->x = pt.x * ax.x + pt.y * ax.y + pt.z * ax.z;
      out->y = pt.x * ay.x + pt.y * ay.y + pt.z * ay.z;
      out->z = pt.x * az.x + pt.y * az.y + pt.z * az.z;
    }
  return out;
}

// A limited rotation matrix, defining the 3 3d-axis rotations.
// Geometry uses a double[16] transformation matrix with the
// origin/offset on the right side columns.
// and the scale vector as last row.
typedef double dwg_matrix9[9];

// Via the arbitrary axis algorithm we can define the 3 rotations (dwg_matrix9)
// as a single normal. This helper function creates the rotation matrix from
// the normal vector.
static void
dwg_geom_normal_to_matrix9 (const dwg_point_3d *restrict normal,
                            dwg_matrix9 *matrix)
{
  // TODO for now we keep the unrotated defaults
#if 0
  if (normal.x == 0.0 && normal.y == 0.0 && normal.z == 1.0)
    {
      dwg_matrix9 def_matrix = {
        1.0, 0.0, 0.0,
        0.0, 1.0, 0.0,
        0.0, 0.0, 1.0 };
      memcpy (&matrix, &def_matrix, sizeof (dwg_point_3d));
      return;
    }
  else
    {
      dwg_point_3d ax, ay, az;
      memcpy (&az, &normal, sizeof (dwg_point_3d));
      if ((fabs (az.x) < 1 / 64.0) && (fabs (az.y) < 1 / 64.0))
        {
          dwg_point_3d tmp = { 0.0, 1.0, 0.0 };
          dwg_geom_cross (&tmp, tmp, az);
          dwg_geom_normalize (&ax, tmp);
        }
      else
        {
          dwg_point_3d tmp = { 0.0, 0.0, 1.0 };
          dwg_geom_cross (&tmp, tmp, az);
          dwg_geom_normalize (&ax, tmp);
        }
      dwg_geom_cross (&ay, az, ax);
      dwg_geom_normalize (&ay, ay);
      //??
      memcpy (&matrix, &ay, sizeof (dwg_point_3d));
    }
#endif
  ;
}

static void
dwg_init_ACSH_CLASS (Dwg_Data *restrict dwg, Dwg_Object *restrict obj,
                     void *restrict acsh,
                     Dwg_Object_EVALUATION_GRAPH *restrict evalgraph,
                     const dwg_point_3d *restrict origin_pt,
                     const dwg_point_3d *restrict normal)
{
  Dwg_Object_ACSH_BOX_CLASS *_obj = (Dwg_Object_ACSH_BOX_CLASS *)acsh;
  dwg_matrix9 matrix = { 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0 };

  obj->tio.object->ownerhandle = dwg_add_handleref (
      dwg, 5, dwg_obj_generic_handlevalue (evalgraph), obj);
  _obj->evalexpr.parentid = -1;
  _obj->evalexpr.major = 27;
  _obj->evalexpr.minor = 52;
  _obj->evalexpr.value_code = -9999;
  _obj->evalexpr.nodeid = nodeid;
  _obj->history_node.major = 27;
  _obj->history_node.minor = 52;
  _obj->history_node.color.index = 256;
  _obj->history_node.color.rgb = 0xc3000001;
  _obj->history_node.color.method = 0xc3;
  _obj->history_node.color.flag = 0x0;
  _obj->history_node.step_id = 97;    //?
  _obj->history_node.material = NULL; // => MATERIAL of LAYER "0"
  dwg_geom_normal_to_matrix9 (normal, &matrix);
  _obj->history_node.trans = (BITCODE_BD *)CALLOC (16, 8);
  _obj->history_node.trans[0] = matrix[0];
  _obj->history_node.trans[1] = matrix[1];
  _obj->history_node.trans[2] = matrix[2];
  _obj->history_node.trans[3] = origin_pt->x;
  _obj->history_node.trans[4] = matrix[3];
  _obj->history_node.trans[5] = matrix[4];
  _obj->history_node.trans[6] = matrix[5];
  _obj->history_node.trans[7] = origin_pt->y;
  _obj->history_node.trans[8] = matrix[6];
  _obj->history_node.trans[9] = matrix[7];
  _obj->history_node.trans[10] = matrix[8];
  _obj->history_node.trans[11] = origin_pt->z;
  // no scale, keep it at 0.0 from CALLOC
  _obj->history_node.trans[15] = 1.0;
  _obj->major = 27;
  _obj->minor = 52;
}

static size_t
dwg_acis_date (char *date, size_t size)
{
  time_t rawtime;
  struct tm *tm;

  time (&rawtime);
  tm = localtime (&rawtime);
  // "Thu Mar 26 22:02:42 2009"
  return strftime (date, size, "%a %b %d %H:%M:%S %Y", tm);
}

EXPORT Dwg_Object_ACSH_BOX_CLASS *
dwg_add_ACSH_BOX_CLASS (Dwg_Object_EVALUATION_GRAPH *restrict evalgraph,
                        const dwg_point_3d *restrict origin_pt,
                        const dwg_point_3d *restrict normal,
                        const double length, const double width,
                        const double height)
{
  int err;
  Dwg_Object *hdr = dwg_obj_generic_to_object (evalgraph, &err);
  Dwg_Data *dwg = hdr ? hdr->parent : NULL;
  if (!dwg)
    return NULL;
  {
    API_ADD_OBJECT (ACSH_BOX_CLASS);
    dwg_init_ACSH_CLASS (dwg, obj, _obj, evalgraph, origin_pt, normal);
    _obj->length = length;
    _obj->width = width;
    _obj->height = height;
    return _obj;
  }
}

static void
ACSH_init_evalgraph (Dwg_Data *restrict dwg, void *restrict _acsh,
                     Dwg_Entity_3DSOLID *restrict solid)
{
  int error;
  Dwg_Object_ACSH_HISTORY_CLASS *hist;
  Dwg_Object_EVALUATION_GRAPH *eval;
  Dwg_Object *solidobj, *histobj, *evalobj, *acsh;
  BITCODE_H *evalexpr = (BITCODE_H *)CALLOC (1, sizeof (BITCODE_H));

  acsh = dwg_obj_generic_to_object (_acsh, &error);
  solidobj = dwg_obj_generic_to_object (solid, &error);

  evalexpr[0] = dwg_add_handleref (dwg, 3, acsh->handle.value, NULL);
  eval = dwg_add_EVALUATION_GRAPH (dwg, 0, nodeid++, 1, evalexpr);
  FREE (evalexpr);
  evalobj = dwg_obj_generic_to_object (eval, &error);
  acsh->tio.object->ownerhandle
      = dwg_add_handleref (dwg, 4, evalobj->handle.value, acsh);

  hist = dwg_add_ACSH_HISTORY_CLASS (solid, 1);
  hist->owner = dwg_add_handleref (dwg, 3, evalobj->handle.value, NULL);
  histobj = dwg_obj_generic_to_object (hist, &error);
  solid->history_id
      = dwg_add_handleref (dwg, 5, histobj->handle.value, solidobj);
  evalobj->tio.object->ownerhandle
      = dwg_add_handleref (dwg, 4, histobj->handle.value, evalobj);
}

EXPORT Dwg_Entity_3DSOLID *
dwg_add_BOX (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
             const dwg_point_3d *restrict origin_pt,
             const dwg_point_3d *restrict normal, const double length,
             const double width, const double height)
{
  int err;
  Dwg_Data *dwg;
  {
    Dwg_Object *hdr = dwg_obj_generic_to_object (blkhdr, &err);
    dwg = hdr ? hdr->parent : NULL;
    if (dwg)
      {
        REQUIRE_CLASS ("ACAD_EVALUATION_GRAPH");
        REQUIRE_CLASS ("ACSH_HISTORY_CLASS");
        REQUIRE_CLASS ("ACSH_BOX_CLASS");
      }
    else
      return NULL;
  }
  {
    Dwg_Entity_3DSOLID *solid;
    Dwg_Object_ACSH_BOX_CLASS *_obj;
    dwg_point_3d defnormal = { 0.0, 0.0, 1.0 };
    dwg_matrix9 matrix = { 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0 };
#define ACIS_BOX_SIZE 7500
    char acis_data[ACIS_BOX_SIZE];
    char date[48];
    size_t date_size = dwg_acis_date (date, 48);
    // origin: 7.791946762401224191 11.02220663951163004 1.271660108551718515
    // length: 4.416106 [BD 40]
    // width: 2.044413 [BD 41]
    // height: 2.543320 [BD 42]
    const double l2 = length / 2.0; // 2.208053237598775809
    const double w2 = width / 2.0;  // 1.022206639511630044
    const double h2 = height / 2.0; // 1.271660108551718515
    const char box_acis_format[] =  /* len = 890 => 957 */
        // version num_records num_entities has_history
        "700 104 1 0 \n"
        // product acis_version date
        "8 LibreDWG 19 ASM 223.0.1.1930 NT %" PRIuSIZE " %s \n"
        // num_mm_units resabs resnor
        "25.39999999999999858 9.999999999999999547e-07 "
        "1.000000000000000036e-10\n"
        "body $-1 -1 $-1 $1 $-1 $2 #\n"
        "lump $-1 -1 $-1 $-1 $3 $0 #\n"
        "transform $-1 -1 "
        "%g %g %g "
        "%g %g %g "
        "%g %g %g "
        "%g %g %g "
        "1 no_rotate no_reflect no_shear #\n"
        "shell $-1 -1 $-1 $-1 $-1 $4 $-1 $1 #\n"
        "face $5 -1 $-1 $-1 $-1 $3 $-1 $6 forward single #\n"
        "color-adesk-attrib $-1 -1 $-1 $-1 $4 256 #\n"
        "face $9 -1 $-1 $10 $11 $3 $-1 $12 reversed single #\n"
        "loop $-1 -1 $-1 $-1 $13 $4 #\n"
        // h2
        "plane-surface $-1 -1 $-1 0 0 %f 0 0 1 -1 0 0 reverse_v I I I I #\n"
        "color-adesk-attrib $-1 -1 $-1 $-1 $6 256 #\n"
        "face $14 -1 $-1 $15 $16 $3 $-1 $17 reversed single #\n"
        "loop $-1 -1 $-1 $-1 $18 $6 #\n"
        // -h2
        "plane-surface $-1 -1 $-1 0 0 %f 0 0 1 -1 0 0 reverse_v I I I I #\n"
        "coedge $-1 -1 $-1 $19 $20 $21 $22 reversed $7 $-1 #\n"
        "color-adesk-attrib $-1 -1 $-1 $-1 $10 256 #\n"
        "face $23 -1 $-1 $24 $25 $3 $-1 $26 reversed single #\n"
        "loop $-1 -1 $-1 $-1 $27 $10 #\n"
        // -w2
        "plane-surface $-1 -1 $-1 0 %f 0 0 1 0 0 0 1 reverse_v I I I I #\n"
        "coedge $-1 -1 $-1 $28 $29 $30 $31 reversed $11 $-1 #\n"
        "coedge $-1 -1 $-1 $32 $13 $33 $34 reversed $7 $-1 #\n"
        "coedge $-1 -1 $-1 $13 $32 $35 $36 reversed $7 $-1 #\n"
        "coedge $-1 -1 $-1 $37 $38 $13 $22 forward $39 $-1 #\n"
        // -w2, w2
        "edge $40 -1 $-1 $41 %f $42 %f $21 $43 forward @7 unknown #\n"
        "color-adesk-attrib $-1 -1 $-1 $-1 $15 256 #\n"
        "face $44 -1 $-1 $45 $46 $3 $-1 $47 reversed single #\n"
        "loop $-1 -1 $-1 $-1 $48 $15 #\n"
        // l2
        "plane-surface $-1 -1 $-1 %f 0 0 -1 0 0 0 0 -1 reverse_v I I I I #\n"
        "coedge $-1 -1 $-1 $33 $49 $50 $51 reversed $16 $-1 #\n"
        "coedge $-1 -1 $-1 $52 $18 $53 $54 reversed $11 $-1 #\n"
        "coedge $-1 -1 $-1 $18 $52 $49 $55 reversed $11 $-1 #\n"
        "coedge $-1 -1 $-1 $38 $37 $18 $31 forward $39 $-1 #\n"
        // -w2, w2
        "edge $56 -1 $-1 $57 %f $58 %f $30 $59 forward @7 unknown #\n"
        "coedge $-1 -1 $-1 $20 $19 $60 $61 reversed $7 $-1 #\n"
        "coedge $-1 -1 $-1 $62 $27 $19 $34 forward $16 $-1 #\n"
        // -l2, l2
        "edge $63 -1 $-1 $64 %f $41 %f $33 $65 forward @7 unknown #\n"
        "coedge $-1 -1 $-1 $66 $67 $20 $36 forward $46 $-1 #\n"
        // -l2, l2
        "edge $68 -1 $-1 $42 %f $69 %f $35 $70 forward @7 unknown #\n"
        "coedge $-1 -1 $-1 $30 $21 $67 $71 forward $39 $-1 #\n"
        "coedge $-1 -1 $-1 $21 $30 $62 $72 reversed $39 $-1 #\n"
        "loop $-1 -1 $-1 $-1 $38 $45 #\n"
        "color-adesk-attrib $-1 -1 $-1 $-1 $22 256 #\n"
        "vertex $-1 -1 $-1 $22 $73 #\n"
        "vertex $-1 -1 $-1 $22 $74 #\n"
        // -l2, h2
        "straight-curve $-1 -1 $-1 %f 0 %f 0 1 0 I I #\n"
        "color-adesk-attrib $-1 -1 $-1 $-1 $24 256 #\n"
        "face $75 -1 $-1 $-1 $39 $3 $-1 $76 reversed single #\n"
        "loop $-1 -1 $-1 $-1 $67 $24 #\n"
        // w2
        "plane-surface $-1 -1 $-1 0 %f 0 0 -1 0 0 0 -1 reverse_v I I I I #\n"
        "coedge $-1 -1 $-1 $60 $77 $66 $78 reversed $25 $-1 #\n"
        "coedge $-1 -1 $-1 $27 $62 $29 $55 forward $16 $-1 #\n"
        "coedge $-1 -1 $-1 $77 $60 $27 $51 forward $25 $-1 #\n"
        // -h2, h2
        "edge $79 -1 $-1 $64 %f $80 %f $50 $81 forward @7 unknown #\n"
        "coedge $-1 -1 $-1 $29 $28 $77 $82 reversed $11 $-1 #\n"
        "coedge $-1 -1 $-1 $67 $66 $28 $54 forward $46 $-1 #\n"
        // -l2, l2
        "edge $83 -1 $-1 $84 %f $57 %f $53 $85 forward @7 unknown #\n"
        // -l2, l2
        "edge $86 -1 $-1 $58 %f $80 %f $49 $87 forward @7 unknown #\n"
        "color-adesk-attrib $-1 -1 $-1 $-1 $31 256 #\n"
        "vertex $-1 -1 $-1 $31 $88 #\n"
        "vertex $-1 -1 $-1 $72 $89 #\n"
        // -l2, -h2
        "straight-curve $-1 -1 $-1 %f 0 %f 0 -1 0 I I #\n"
        "coedge $-1 -1 $-1 $50 $48 $32 $61 forward $25 $-1 #\n"
        // -w2, w2
        "edge $90 -1 $-1 $69 %f $64 %f $60 $91 forward @7 unknown #\n"
        "coedge $-1 -1 $-1 $49 $33 $38 $72 forward $16 $-1 #\n"
        "color-adesk-attrib $-1 -1 $-1 $-1 $34 256 #\n"
        "vertex $-1 -1 $-1 $61 $92 #\n"
        // -w2, h2
        "straight-curve $-1 -1 $-1 0 %f %f -1 0 0 I I #\n"
        "coedge $-1 -1 $-1 $53 $35 $48 $78 forward $46 $-1 #\n"
        "coedge $-1 -1 $-1 $35 $53 $37 $71 reversed $46 $-1 #\n"
        "color-adesk-attrib $-1 -1 $-1 $-1 $36 256 #\n"
        "vertex $-1 -1 $-1 $36 $93 #\n"
        // w2, h2
        "straight-curve $-1 -1 $-1 0 %f %f 1 0 0 I I #\n"
        // -h2, h2
        "edge $94 -1 $-1 $42 %f $57 %f $37 $95 forward @7 unknown #\n"
        // -h2, h2
        "edge $96 -1 $-1 $41 %f $58 %f $38 $97 forward @7 unknown #\n"
        // -l2 -w2 h2
        "point $-1 -1 $-1 %f %f %f #\n"
        // -l2 w2 h2
        "point $-1 -1 $-1 %f %f %f #\n"
        "color-adesk-attrib $-1 -1 $-1 $-1 $45 256 #\n"
        // -l2
        "plane-surface $-1 -1 $-1 %f 0 0 1 0 0 0 0 1 reverse_v I I I I #\n"
        "coedge $-1 -1 $-1 $48 $50 $52 $82 forward $25 $-1 #\n"
        // -h2, h2
        "edge $98 -1 $-1 $69 %f $84 %f $66 $99 forward @7 unknown #\n"
        "color-adesk-attrib $-1 -1 $-1 $-1 $51 256 #\n"
        "vertex $-1 -1 $-1 $82 $100 #\n"
        // l2 -w2
        "straight-curve $-1 -1 $-1 %f %f 0 0 0 -1 I I #\n"
        // -w2 w2
        "edge $101 -1 $-1 $80 %f $84 %f $77 $102 forward @7 unknown #\n"
        "color-adesk-attrib $-1 -1 $-1 $-1 $54 256 #\n"
        "vertex $-1 -1 $-1 $54 $103 #\n"
        // w2 -h2
        "straight-curve $-1 -1 $-1 0 %f %f -1 0 0 I I #\n"
        "color-adesk-attrib $-1 -1 $-1 $-1 $55 256 #\n"
        // -w2 -h2
        "straight-curve $-1 -1 $-1 0 %f %f 1 0 0 I I #\n"
        // -l2 w2 -h2
        "point $-1 -1 $-1 %f %f %f #\n"
        // -l2 -w2 -h2
        "point $-1 -1 $-1 %f %f %f #\n"
        "color-adesk-attrib $-1 -1 $-1 $-1 $61 256 #\n"
        // l2, h2
        "straight-curve $-1 -1 $-1 %f 0 %f 0 -1 0 I I #\n"
        // l2 -w2 h2
        "point $-1 -1 $-1 %f %f %f #\n"
        // l2 w2 h2
        "point $-1 -1 $-1 %f %f %f #\n"
        "color-adesk-attrib $-1 -1 $-1 $-1 $71 256 #\n"
        // -l2 w2
        "straight-curve $-1 -1 $-1 %f %f 0 0 0 -1 I I #\n"
        "color-adesk-attrib $-1 -1 $-1 $-1 $72 256 #\n"
        // -l2 -w2
        "straight-curve $-1 -1 $-1 %f %f 0 0 0 -1 I I #\n"
        "color-adesk-attrib $-1 -1 $-1 $-1 $78 256 #\n"
        // l2, w2
        "straight-curve $-1 -1 $-1 %f %f 0 0 0 -1 I I #\n"
        // l2 -w2 -h2
        "point $-1 -1 $-1 %f %f %f #\n"
        "color-adesk-attrib $-1 -1 $-1 $-1 $82 256 #\n"
        // l2, -h2
        "straight-curve $-1 -1 $-1 %f 0 %f 0 1 0 I I #\n"
        // l2 w2 -h2
        "point $-1 -1 $-1 %f %f %f #\n"
        "End-of-ACIS-data\n";
    dwg_geom_normal_to_matrix9 (normal, &matrix);
    snprintf (acis_data, ACIS_BOX_SIZE, box_acis_format, date_size, date,
              matrix[0], matrix[1], matrix[2], matrix[3], matrix[4], matrix[5],
              matrix[6], matrix[7], matrix[8],
              // clang-format off
              origin_pt->x, origin_pt->y, origin_pt->z,
              h2,
              -h2,
              -w2,
              -w2, w2,
              l2,
              -w2, w2,
              -l2, l2,
              -l2, l2,
              -l2, h2,
              w2,
              -h2, h2,
              -l2, l2,
              -l2, l2,
              -l2, -h2,
              -w2, w2,
              -w2, h2,
              w2, h2,
              -h2, h2,
              -h2, h2,
              -l2, -w2, h2,
              -l2, w2, h2,
              -l2,
              -h2, h2,
              l2, -w2,
              -w2, w2,
              w2, -h2,
              -w2, -h2,
              -l2, w2, -h2,
              -l2, -w2, -h2,
              l2, h2,
              l2, -w2, h2,
              l2, w2, h2,
              -l2, w2,
              -l2, -w2,
              l2, w2,
              l2, -w2, -h2,
              l2, -h2,
              l2, w2, -h2
              );
    // clang-format on
    solid = dwg_add_3DSOLID (blkhdr, acis_data);
    solid->wireframe_data_present = 1;
    solid->point_present = 1;
    solid->point.x = origin_pt->x;
    solid->point.y = origin_pt->y;
    solid->point.z = origin_pt->z;
    solid->acis_empty_bit = 1;

    // solid only needed for the dwg
    _obj = dwg_add_ACSH_BOX_CLASS (
        (Dwg_Object_EVALUATION_GRAPH *)(void *)solid, origin_pt,
        normal ? normal : &defnormal, length, width, height);
    ACSH_init_evalgraph (dwg, _obj, solid);
    return solid;
  }
}

// ACSH_BREP_CLASS

EXPORT Dwg_Object_ACSH_CHAMFER_CLASS *
dwg_add_ACSH_CHAMFER_CLASS (Dwg_Object_EVALUATION_GRAPH *restrict evalgraph,
                            const dwg_point_3d *restrict origin_pt,
                            const dwg_point_3d *restrict normal,
                            const int bl92, const double base_dist,
                            const double other_dist, const int num_edges,
                            const int32_t *edges, const int bl95)
{
  int err;
  Dwg_Object *hdr = dwg_obj_generic_to_object (evalgraph, &err);
  Dwg_Data *dwg = hdr ? hdr->parent : NULL;
  if (!dwg)
    return NULL;
  {
    API_ADD_OBJECT (ACSH_CHAMFER_CLASS);
    dwg_init_ACSH_CLASS (dwg, obj, _obj, evalgraph, origin_pt, normal);
    _obj->bl92 = bl92;
    _obj->base_dist = base_dist;
    _obj->other_dist = other_dist;
    _obj->num_edges = num_edges;
    if (num_edges)
      {
        _obj->edges = (BITCODE_BL *)CALLOC (num_edges, 4);
        memcpy (_obj->edges, edges, num_edges * 4);
      }
    _obj->bl95 = bl95;
    return _obj;
  }
}

#if 0
EXPORT Dwg_Entity_3DSOLID*
dwg_add_CHAMFER (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                 const dwg_point_3d *restrict origin_pt, const dwg_point_3d *restrict normal,
                 const int bl92, const double base_dist,
                 const double other_dist, const int num_edges,
                 const int32_t* edges, const int bl95)
{
  int err;
  Dwg_Data *dwg;
  {
    Dwg_Object *hdr = dwg_obj_generic_to_object (blkhdr, &err);
    dwg = hdr ? hdr->parent : NULL;
    if (dwg)
      {
        REQUIRE_CLASS ("ACAD_EVALUATION_GRAPH");
        REQUIRE_CLASS ("ACSH_HISTORY_CLASS");
        REQUIRE_CLASS ("ACSH_CHAMFER_CLASS");
      }
    else
      return NULL;
  }
  {
    Dwg_Entity_3DSOLID *solid;
    Dwg_Object_ACSH_CHAMFER_CLASS *_obj;
    Dwg_Object_ACSH_HISTORY_CLASS *hist;
    Dwg_Object_EVALUATION_GRAPH *eval;
    Dwg_Object *solidobj, *histobj, *evalobj, *acsh;
    dwg_point_3d defnormal = { 0.0, 0.0, 1.0 };
    dwg_matrix9 matrix = {
      1.0, 0.0, 0.0,
      0.0, 1.0, 0.0,
      0.0, 0.0, 1.0 };
    char acis_data[1048];
    char date[48];
    size_t date_size = dwg_acis_date (date, 48);
    // acis version 106 (r14) would be nicer
    const char base_acis_format[] = /* len = 890 => 957 */
      // version num_records num_entities has_history
      "400 6 1 0 \n"
      // product acis_version date
      "8 LibreDWG 19 ASM 223.0.1.1930 NT %" PRIuSIZE " %s \n"
      // num_mm_units resabs resnor
      "25.39999999999999858 9.999999999999999547e-07 1.000000000000000036e-10\n"
      "body $-1 -1 $-1 $1 $-1 $2 #\n"
      "lump $-1 -1 $-1 $-1 $3 $0 #\n"
      "transform $-1 -1 " "%g %g %g " "%g %g %g " "%g %g %g " "%g %g %g " "1 no_rotate no_reflect no_shear #\n"
      "shell $-1 -1 $-1 $-1 $-1 $4 $-1 $1 #\n"
      "face $5 -1 $-1 $-1 $-1 $3 $-1 $6 forward single #\n"
      "color-adesk-attrib $-1 -1 $-1 $-1 $4 256 #\n"
      "End-of-ACIS-data\n";
    //FIXME
    dwg_geom_normal_to_matrix9 (normal, &matrix);
    snprintf (acis_data, 1048, base_acis_format,
              date_size, date,
              matrix[0], matrix[1], matrix[2],
              matrix[3], matrix[4], matrix[5],
              matrix[6], matrix[7], matrix[8],
              origin_pt->x, origin_pt->y, origin_pt->z);
    solid = dwg_add_3DSOLID (blkhdr, acis_data);
    solidobj = dwg_obj_generic_to_object (solid, &err);
    solid->wireframe_data_present = 1;
    solid->point_present = 1;
    solid->point.x = origin_pt->x;
    solid->point.y = origin_pt->y;
    solid->point.z = origin_pt->z;
    solid->acis_empty_bit = 1;

    _obj = dwg_add_ACSH_CHAMFER_CLASS (
        (Dwg_Object_EVALUATION_GRAPH *)(void *)solid, origin_pt,
        normal ? normal : &defnormal, bl92,
        base_dist, other_dist, num_edges, edges, bl95);
    ACSH_init_evalgraph (dwg, _obj, solid);
    return solid;
  }
}
#endif

EXPORT Dwg_Object_ACSH_CONE_CLASS *
dwg_add_ACSH_CONE_CLASS (Dwg_Object_EVALUATION_GRAPH *restrict evalgraph,
                         const dwg_point_3d *restrict origin_pt,
                         const dwg_point_3d *restrict normal,
                         const double height, const double major_radius,
                         const double minor_radius, const double x_radius)
{
  int err;
  Dwg_Object *hdr = dwg_obj_generic_to_object (evalgraph, &err);
  Dwg_Data *dwg = hdr ? hdr->parent : NULL;
  if (!dwg)
    return NULL;
  {
    API_ADD_OBJECT (ACSH_CONE_CLASS);
    dwg_init_ACSH_CLASS (dwg, obj, _obj, evalgraph, origin_pt, normal);
    _obj->height = height;
    _obj->major_radius = major_radius;
    _obj->minor_radius = minor_radius;
    _obj->x_radius = x_radius;
    return _obj;
  }
}

// same as CYLINDER
EXPORT Dwg_Entity_3DSOLID *
dwg_add_CONE (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
              const dwg_point_3d *restrict origin_pt,
              const dwg_point_3d *restrict normal, const double height,
              const double major_radius, const double minor_radius,
              const double x_radius)
{
  int err;
  Dwg_Data *dwg;
  {
    Dwg_Object *hdr = dwg_obj_generic_to_object (blkhdr, &err);
    dwg = hdr ? hdr->parent : NULL;
    if (dwg)
      {
        REQUIRE_CLASS ("ACAD_EVALUATION_GRAPH");
        REQUIRE_CLASS ("ACSH_HISTORY_CLASS");
        REQUIRE_CLASS ("ACSH_CONE_CLASS");
      }
    else
      return NULL;
  }
  {
    Dwg_Entity_3DSOLID *solid;
    Dwg_Object_ACSH_CONE_CLASS *_obj;
    dwg_point_3d defnormal = { 0.0, 0.0, 1.0 };
    dwg_matrix9 matrix = { 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0 };
    double h2 = height / 2.0;
    double majr2 = major_radius / 2.0;
    double minr2 = minor_radius / 2.0;
    double x2 = x_radius / 2.0;
    double dbl_pi = 2.0 * M_PI; // i.e. 360
    char acis_data[1600];
    char date[48];
    size_t date_size = dwg_acis_date (date, 48);
    const char cone_acis_format[] = /* len = 1200? => 1338 */
        // version num_records num_entities has_history
        "400 27 1 0 \n"
        // product acis_version date
        "8 LibreDWG 19 ASM 223.0.1.1930 NT %" PRIuSIZE " %s \n"
        // num_mm_units resabs resnor
        "25.39999999999999858 9.999999999999999547e-07 "
        "1.000000000000000036e-10\n"
        "body $-1 -1 $-1 $1 $-1 $2 #\n"
        "lump $-1 -1 $-1 $-1 $3 $0 #\n"
        "transform $-1 -1 "
        "%g %g %g "
        "%g %g %g "
        "%g %g %g "
        "%g %g %g "
        "1 no_rotate no_reflect no_shear #\n"
        "shell $-1 -1 $-1 $-1 $-1 $4 $-1 $1 #\n"
        "face $5 -1 $-1 $6 $7 $3 $-1 $8 forward single #\n"
        "color-adesk-attrib $-1 -1 $-1 $-1 $4 256 #\n"
        "face $9 -1 $-1 $-1 $10 $3 $-1 $11 forward single #\n"
        "loop $-1 -1 $-1 $12 $13 $4 #\n"
        "cone-surface $-1 -1 $-1 0 0 0 0 0 1 %f 0 0 1 I I %f %f %f forward I "
        "I I I #\n" // 2.5, -0.3162277660168379412 0.948683298050513768 2.5
        "color-adesk-attrib $-1 -1 $-1 $-1 $6 256 #\n"
        "loop $-1 -1 $-1 $-1 $14 $6 #\n"
        "plane-surface $-1 -1 $-1 0 0 %f 0 0 -1 -1 0 0 forward_v I I I I #\n" // -7.5
        "loop $-1 -1 $-1 $-1 $15 $4 #\n"
        "coedge $-1 -1 $-1 $13 $13 $14 $16 reversed $7 $-1 #\n"
        "coedge $-1 -1 $-1 $14 $14 $13 $16 forward $10 $-1 #\n"
        "coedge $-1 -1 $-1 $15 $15 $-1 $17 reversed $12 $-1 #\n"
        "edge $18 -1 $-1 $19 0 $19 %f $14 $20 forward @7 unknown #\n" // 2*pi
        "edge $21 -1 $-1 $22 1 $22 0 $15 $-1 forward @7 unknown #\n"
        "color-adesk-attrib $-1 -1 $-1 $-1 $16 256 #\n"
        "vertex $-1 -1 $-1 $16 $23 #\n"
        "ellipse-curve $-1 -1 $-1 0 0 %f 0 0 -1 %f 0 0 1 I I #\n" // -7.5, 5
        "color-adesk-attrib $-1 -1 $-1 $-1 $17 256 #\n"
        "vertex $-1 -1 $-1 $17 $24 #\n"
        "point $-1 -1 $-1 %f 0 %f #\n" // 5, -7.5
        "point $-1 -1 $-1 %f 0 %f #\n" // 0, 7.5
        "End-of-ACIS-data\n";
    // pt: 10, 10, 7.5
    // height: 15.000000 [BD 40]
    // major_radius: 5.000000 [BD 41]
    // minor_radius: 5.000000 [BD 42]
    // x_radius: 0.000000 [BD 43]
    dwg_geom_normal_to_matrix9 (normal, &matrix);
    snprintf (acis_data, 1600, cone_acis_format, date_size, date, matrix[0],
              matrix[1], matrix[2], matrix[3], matrix[4], matrix[5], matrix[6],
              matrix[7], matrix[8], origin_pt->x, origin_pt->y, origin_pt->z,
              majr2, -0.3162277660168379412, 0.948683298050513768, minr2, //?
              -majr2, dbl_pi, -majr2, major_radius, minor_radius, -h2,
              x_radius, h2);
    solid = dwg_add_3DSOLID (blkhdr, acis_data);
    solid->wireframe_data_present = 1;
    solid->point_present = 1;
    solid->point.x = origin_pt->x;
    solid->point.y = origin_pt->y;
    solid->point.z = origin_pt->z;
    solid->acis_empty_bit = 1;

    _obj = dwg_add_ACSH_CONE_CLASS (
        (Dwg_Object_EVALUATION_GRAPH *)(void *)solid, origin_pt,
        normal ? normal : &defnormal, height, major_radius, minor_radius,
        x_radius);
    ACSH_init_evalgraph (dwg, _obj, solid);
    return solid;
  }
}

EXPORT Dwg_Object_ACSH_CYLINDER_CLASS *
dwg_add_ACSH_CYLINDER_CLASS (Dwg_Object_EVALUATION_GRAPH *restrict evalgraph,
                             const dwg_point_3d *restrict origin_pt,
                             const dwg_point_3d *restrict normal,
                             const double height, const double major_radius,
                             const double minor_radius, const double x_radius)
{
  int err;
  Dwg_Object *hdr = dwg_obj_generic_to_object (evalgraph, &err);
  Dwg_Data *dwg = hdr ? hdr->parent : NULL;
  if (!dwg)
    return NULL;
  {
    API_ADD_OBJECT (ACSH_CYLINDER_CLASS);
    dwg_init_ACSH_CLASS (dwg, obj, _obj, evalgraph, origin_pt, normal);
    _obj->height = height;
    _obj->major_radius = major_radius;
    _obj->minor_radius = minor_radius;
    _obj->x_radius = x_radius;
    return _obj;
  }
}

// TODO compare to a cone, it's the same
EXPORT Dwg_Entity_3DSOLID *
dwg_add_CYLINDER (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                  const dwg_point_3d *restrict origin_pt,
                  const dwg_point_3d *restrict normal, const double height,
                  const double major_radius, const double minor_radius,
                  const double x_radius)
{
  int err;
  Dwg_Data *dwg;
  {
    Dwg_Object *hdr = dwg_obj_generic_to_object (blkhdr, &err);
    dwg = hdr ? hdr->parent : NULL;
    if (dwg)
      {
        REQUIRE_CLASS ("ACAD_EVALUATION_GRAPH");
        REQUIRE_CLASS ("ACSH_HISTORY_CLASS");
        REQUIRE_CLASS ("ACSH_CYLINDER_CLASS");
      }
    else
      return NULL;
  }
  {
    Dwg_Entity_3DSOLID *solid;
    Dwg_Object_ACSH_CYLINDER_CLASS *_obj;
    dwg_point_3d defnormal = { 0.0, 0.0, 1.0 };
    dwg_matrix9 matrix = { 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0 };
    double h2 = height / 2.0;
    double dbl_pi = 2.0 * M_PI; // i.e. 360
    char acis_data[2000];
    char date[48];
    size_t date_size = dwg_acis_date (date, 48);
    // acis version 106 (r14) would be nicer
    const char cylinder_acis_format[] = /* len = 890 => 1609 */
        // version num_records num_entities has_history
        "400 30 1 0 \n"
        // product acis_version date
        "8 LibreDWG 19 ASM 223.0.1.1930 NT %" PRIuSIZE " %s \n"
        // num_mm_units resabs resnor
        "25.39999999999999858 9.999999999999999547e-07 "
        "1.000000000000000036e-10\n"
        "body $-1 -1 $-1 $1 $-1 $2 #\n"
        "lump $-1 -1 $-1 $-1 $3 $0 #\n"
        "transform $-1 -1 "
        "%g %g %g "
        "%g %g %g "
        "%g %g %g "
        "%g %g %g "
        "1 no_rotate no_reflect no_shear #\n"
        "shell $-1 -1 $-1 $-1 $-1 $4 $-1 $1 #\n"
        "face $5 -1 $-1 $6 $7 $3 $-1 $8 forward single #\n"
        "color-adesk-attrib $-1 -1 $-1 $-1 $4 256 #\n"
        "face $9 -1 $-1 $10 $11 $3 $-1 $12 forward single #\n"
        "loop $-1 -1 $-1 $13 $14 $4 #\n"
        "cone-surface $-1 -1 $-1 0 0 0 0 0 1 %g 0 0 1 I I 0 1 %g forward I I "
        "I I #\n"
        "color-adesk-attrib $-1 -1 $-1 $-1 $6 256 #\n"
        "face $15 -1 $-1 $-1 $16 $3 $-1 $17 forward single #\n"
        "loop $-1 -1 $-1 $-1 $18 $6 #\n"
        "plane-surface $-1 -1 $-1 0 0 %g 0 0 -1 -1 0 0 forward_v I I I I #\n" // -height/2
        "loop $-1 -1 $-1 $-1 $19 $4 #\n"
        "coedge $-1 -1 $-1 $14 $14 $18 $20 reversed $7 $-1 #\n"
        "color-adesk-attrib $-1 -1 $-1 $-1 $10 256 #\n"
        "loop $-1 -1 $-1 $-1 $21 $10 #\n"
        "plane-surface $-1 -1 $-1 0 0 %g 0 0 1 1 0 0 forward_v I I I I #\n" // height/2
        "coedge $-1 -1 $-1 $18 $18 $14 $20 forward $11 $-1 #\n"
        "coedge $-1 -1 $-1 $19 $19 $21 $22 reversed $13 $-1 #\n"
        "edge $23 -1 $-1 $24 0 $24 %g $18 $25 forward @7 unknown #\n" // 2*pi
        "coedge $-1 -1 $-1 $21 $21 $19 $22 forward $16 $-1 #\n"
        "edge $26 -1 $-1 $27 0 $27 %g $21 $28 forward @7 unknown #\n" // 2*pi
        "color-adesk-attrib $-1 -1 $-1 $-1 $20 256 #\n"
        "vertex $-1 -1 $-1 $20 $29 #\n"
        "ellipse-curve $-1 -1 $-1 0 0 %g 0 0 -1 %g 0 0 1 I I #\n" // -height/2,
                                                                  // major_radius
        "color-adesk-attrib $-1 -1 $-1 $-1 $22 256 #\n"
        "vertex $-1 -1 $-1 $22 $30 #\n"
        "ellipse-curve $-1 -1 $-1 0 0 %g 0 0 1 %g 0 0 1 I I #\n" // -height/2,
                                                                 // minor_radius
        "point $-1 -1 $-1 %g 0 %g #\n" // major_radius, -height/2,
        "point $-1 -1 $-1 %g 0 %g #\n" // major_radius, height/2,
        "End-of-ACIS-data\n";
    dwg_geom_normal_to_matrix9 (normal, &matrix);
    snprintf (acis_data, 2000, cylinder_acis_format, date_size, date,
              matrix[0], matrix[1], matrix[2], matrix[3], matrix[4], matrix[5],
              matrix[6], matrix[7], matrix[8], origin_pt->x, origin_pt->y,
              origin_pt->z, major_radius, minor_radius, -h2, h2, dbl_pi,
              dbl_pi, -h2, major_radius, -h2, major_radius, major_radius, -h2,
              major_radius, h2);
    solid = dwg_add_3DSOLID (blkhdr, acis_data);
    solid->wireframe_data_present = 1;
    solid->point_present = 1;
    solid->point.x = origin_pt->x;
    solid->point.y = origin_pt->y;
    solid->point.z = origin_pt->z;
    solid->acis_empty_bit = 1;

    _obj = dwg_add_ACSH_CYLINDER_CLASS (
        (Dwg_Object_EVALUATION_GRAPH *)(void *)solid, origin_pt,
        normal ? normal : &defnormal, height, major_radius, minor_radius,
        x_radius);
    ACSH_init_evalgraph (dwg, _obj, solid);
    return solid;
  }
}

EXPORT Dwg_Object_ACSH_PYRAMID_CLASS *
dwg_add_ACSH_PYRAMID_CLASS (Dwg_Object_EVALUATION_GRAPH *restrict evalgraph,
                            const dwg_point_3d *restrict origin_pt,
                            const dwg_point_3d *restrict normal,
                            const double height, const int sides,
                            const double radius, const double topradius)
{
  int err;
  Dwg_Object *hdr = dwg_obj_generic_to_object (evalgraph, &err);
  Dwg_Data *dwg = hdr ? hdr->parent : NULL;
  if (!dwg)
    return NULL;
  {
    API_ADD_OBJECT (ACSH_PYRAMID_CLASS);
    dwg_init_ACSH_CLASS (dwg, obj, _obj, evalgraph, origin_pt, normal);
    _obj->height = height;
    _obj->sides = sides;
    _obj->radius = radius;
    _obj->topradius = topradius;
    return _obj;
  }
}

// not yet implemented
EXPORT Dwg_Entity_3DSOLID *
dwg_add_PYRAMID (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                 const dwg_point_3d *restrict origin_pt,
                 const dwg_point_3d *restrict normal, const double height,
                 const int sides, const double radius, const double topradius)
{
  int err;
  Dwg_Data *dwg;
  {
    Dwg_Object *hdr = dwg_obj_generic_to_object (blkhdr, &err);
    dwg = hdr ? hdr->parent : NULL;
    if (dwg)
      {
        REQUIRE_CLASS ("ACAD_EVALUATION_GRAPH");
        REQUIRE_CLASS ("ACSH_HISTORY_CLASS");
        REQUIRE_CLASS ("ACSH_PYRAMID_CLASS");
      }
    else
      return NULL;
  }
  {
    Dwg_Entity_3DSOLID *solid;
    Dwg_Object_ACSH_PYRAMID_CLASS *_obj;
    dwg_point_3d defnormal = { 0.0, 0.0, 1.0 };
    dwg_matrix9 matrix = { 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0 };
    char acis_data[1048];
    char date[48];
    size_t date_size = dwg_acis_date (date, 48);
    // acis version 106 (r14) would be nicer
    const char base_acis_format[] = /* len = 890 => 957 */
        // version num_records num_entities has_history
        "400 6 1 0 \n"
        // product acis_version date
        "8 LibreDWG 19 ASM 223.0.1.1930 NT %" PRIuSIZE " %s \n"
        // num_mm_units resabs resnor
        "25.39999999999999858 9.999999999999999547e-07 "
        "1.000000000000000036e-10\n"
        "body $-1 -1 $-1 $1 $-1 $2 #\n"
        "lump $-1 -1 $-1 $-1 $3 $0 #\n"
        "transform $-1 -1 "
        "%g %g %g "
        "%g %g %g "
        "%g %g %g "
        "%g %g %g "
        "1 no_rotate no_reflect no_shear #\n"
        "shell $-1 -1 $-1 $-1 $-1 $4 $-1 $1 #\n"
        "face $5 -1 $-1 $-1 $-1 $3 $-1 $6 forward single #\n"
        "color-adesk-attrib $-1 -1 $-1 $-1 $4 256 #\n"
        "End-of-ACIS-data\n";
    // TODO
    dwg_geom_normal_to_matrix9 (normal, &matrix);
    snprintf (acis_data, 1048, base_acis_format, date_size, date, matrix[0],
              matrix[1], matrix[2], matrix[3], matrix[4], matrix[5], matrix[6],
              matrix[7], matrix[8], origin_pt->x, origin_pt->y, origin_pt->z);
    solid = dwg_add_3DSOLID (blkhdr, acis_data);
    solid->wireframe_data_present = 1;
    solid->point_present = 1;
    solid->point.x = origin_pt->x;
    solid->point.y = origin_pt->y;
    solid->point.z = origin_pt->z;
    solid->acis_empty_bit = 1;

    _obj = dwg_add_ACSH_PYRAMID_CLASS (
        (Dwg_Object_EVALUATION_GRAPH *)(void *)solid, origin_pt,
        normal ? normal : &defnormal, height, sides, radius, topradius);
    ACSH_init_evalgraph (dwg, _obj, solid);
    return solid;
  }
}

// EXPORT Dwg_Entity_3DSOLID*
// dwg_add_ELLIPTICAL_CONE (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
//                          const dwg_point_3d *restrict origin_pt,
//                          const dwg_point_3d *restrict normal, /* maybe NULL
//                          */ const double major_radius, const double
//                          minor_radius, const double height)
//{
//   LOG_ERROR ("%s not yet implemented", __FUNCTION__)
//   return NULL;
// }
//
// EXPORT Dwg_Entity_3DSOLID*
// dwg_add_ELLIPTICAL_CYLINDER (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
//                              const dwg_point_3d *restrict origin_pt,
//                              const dwg_point_3d *restrict normal, /* maybe
//                              NULL */ const double major_radius, const double
//                              minor_radius, const double height)
//{
//   LOG_ERROR ("%s not yet implemented", __FUNCTION__)
//   return NULL;
// }

EXPORT Dwg_Entity_3DSOLID *
dwg_add_EXTRUDED_SOLID (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                        const Dwg_Object *restrict profile,
                        const double height, const double taper_angle)
{
  LOG_ERROR ("%s not yet implemented", __FUNCTION__)
  return NULL;
}
EXPORT Dwg_Entity_3DSOLID *
dwg_add_EXTRUDED_PATH (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                       const Dwg_Object *restrict profile, const double height,
                       const double taper_angle)
{
  LOG_ERROR ("%s not yet implemented", __FUNCTION__)
  return NULL;
}

EXPORT Dwg_Entity_3DSOLID *
dwg_add_REVOLVED_SOLID (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                        const Dwg_Object *restrict profile,
                        const dwg_point_3d *restrict axis_pt,
                        const dwg_point_3d *restrict axis_dir,
                        const double angle)
{
  LOG_ERROR ("%s not yet implemented", __FUNCTION__)
  return NULL;
}

// ACSH_EXTRUSION_CLASS (needed)
// ACSH_FILLET_CLASS
// ACSH_LOFT_CLASS
// ACSH_REVOLVE_CLASS (needed)

EXPORT Dwg_Object_ACSH_SPHERE_CLASS *
dwg_add_ACSH_SPHERE_CLASS (Dwg_Object_EVALUATION_GRAPH *restrict evalgraph,
                           const dwg_point_3d *restrict origin_pt,
                           const dwg_point_3d *restrict normal,
                           const double radius)
{
  int err;
  Dwg_Object *hdr = dwg_obj_generic_to_object (evalgraph, &err);
  Dwg_Data *dwg = hdr ? hdr->parent : NULL;
  if (!dwg)
    return NULL;
  {
    API_ADD_OBJECT (ACSH_SPHERE_CLASS);
    dwg_init_ACSH_CLASS (dwg, obj, _obj, evalgraph, origin_pt, normal);
    _obj->radius = radius;
    return _obj;
  }
}

EXPORT Dwg_Entity_3DSOLID *
dwg_add_SPHERE (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                const dwg_point_3d *restrict origin_pt,
                const dwg_point_3d *restrict normal, const double radius)
{
  int err;
  Dwg_Data *dwg;
  {
    Dwg_Object *hdr = dwg_obj_generic_to_object (blkhdr, &err);
    dwg = hdr ? hdr->parent : NULL;
    if (dwg)
      {
        REQUIRE_CLASS ("ACAD_EVALUATION_GRAPH");
        REQUIRE_CLASS ("ACSH_HISTORY_CLASS");
        REQUIRE_CLASS ("ACSH_SPHERE_CLASS");
      }
    else
      return NULL;
  }
  {
    Dwg_Entity_3DSOLID *solid;
    Dwg_Object_ACSH_SPHERE_CLASS *_obj;
    dwg_point_3d defnormal = { 0.0, 0.0, 1.0 };
    dwg_matrix9 matrix = { 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0 };
    char acis_data[650];
    char date[48];
    size_t date_size = dwg_acis_date (date, 48);
    const char sphere_acis_format[] = /* len = 524 => 552 */
        // version num_records num_entities has_history
        "400 7 1 0 \n"
        // product acis_version date
        "8 LibreDWG 19 ASM 223.0.1.1930 NT %" PRIuSIZE " %s \n"
        // num_mm_units resabs resnor
        "25.39999999999999858 9.999999999999999547e-07 "
        "1.000000000000000036e-10\n"
        "body $-1 -1 $-1 $1 $-1 $2 #\n"
        "lump $-1 -1 $-1 $-1 $3 $0 #\n"
        "transform $-1 -1 "
        "%g %g %g "
        "%g %g %g "
        "%g %g %g "
        "%g %g %g "
        "1 no_rotate no_reflect no_shear #\n"
        "shell $-1 -1 $-1 $-1 $-1 $4 $-1 $1 #\n"
        "face $5 -1 $-1 $-1 $-1 $3 $-1 $6 forward single #\n"
        "color-adesk-attrib $-1 -1 $-1 $-1 $4 256 #\n"
        "sphere-surface $-1 -1 $-1 0 0 0 %g 1 0 0 0 0 1 forward_v I I I I #\n"
        "End-of-ACIS-data\n";
    dwg_geom_normal_to_matrix9 (normal, &matrix);
    snprintf (acis_data, 650, sphere_acis_format, date_size, date, matrix[0],
              matrix[1], matrix[2], matrix[3], matrix[4], matrix[5], matrix[6],
              matrix[7], matrix[8], origin_pt->x, origin_pt->y, origin_pt->z,
              radius);
    solid = dwg_add_3DSOLID (blkhdr, acis_data);
    solid->wireframe_data_present = 1;
    solid->point_present = 1;
    solid->point.x = origin_pt->x;
    solid->point.y = origin_pt->y;
    solid->point.z = origin_pt->z;
    solid->acis_empty_bit = 1;

    _obj = dwg_add_ACSH_SPHERE_CLASS (
        (Dwg_Object_EVALUATION_GRAPH *)(void *)solid, origin_pt,
        normal ? normal : &defnormal, radius);
    ACSH_init_evalgraph (dwg, _obj, solid);
    return solid;
  }
}

// ACSH_SWEEP_CLASS

EXPORT Dwg_Object_ACSH_TORUS_CLASS *
dwg_add_ACSH_TORUS_CLASS (Dwg_Object_EVALUATION_GRAPH *restrict evalgraph,
                          const dwg_point_3d *restrict origin_pt,
                          const dwg_point_3d *restrict normal,
                          const double major_radius, const double minor_radius)
{
  int err;
  Dwg_Object *hdr = dwg_obj_generic_to_object (evalgraph, &err);
  Dwg_Data *dwg = hdr ? hdr->parent : NULL;
  if (!dwg)
    return NULL;
  {
    API_ADD_OBJECT (ACSH_TORUS_CLASS);
    dwg_init_ACSH_CLASS (dwg, obj, _obj, evalgraph, origin_pt, normal);
    _obj->major_radius = major_radius;
    _obj->minor_radius = minor_radius;
    return _obj;
  }
}

EXPORT Dwg_Entity_3DSOLID *
dwg_add_TORUS (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
               const dwg_point_3d *restrict origin_pt,
               const dwg_point_3d *restrict normal, const double major_radius,
               const double minor_radius)
{
  int err;
  Dwg_Data *dwg;
  {
    Dwg_Object *hdr = dwg_obj_generic_to_object (blkhdr, &err);
    dwg = hdr ? hdr->parent : NULL;
    if (dwg)
      {
        REQUIRE_CLASS ("ACAD_EVALUATION_GRAPH");
        REQUIRE_CLASS ("ACSH_HISTORY_CLASS");
        REQUIRE_CLASS ("ACSH_TORUS_CLASS");
      }
    else
      return NULL;
  }
  {
    Dwg_Entity_3DSOLID *solid;
    Dwg_Object_ACSH_TORUS_CLASS *_obj;
    dwg_point_3d defnormal = { 0.0, 0.0, 1.0 };
    dwg_matrix9 matrix = { 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0 };
    char acis_data[1048];
    char date[48];
    size_t date_size = dwg_acis_date (date, 48);
    // acis version 106 (r14) would be nicer
    const char torus_acis_format[] = /* len = 890 => 957 */
        // version num_records num_entities has_history
        "400 19 1 0 \n"
        // product acis_version date
        "8 LibreDWG 19 ASM 223.0.1.1930 NT %" PRIuSIZE " %s \n"
        // num_mm_units resabs resnor
        "25.39999999999999858 9.999999999999999547e-07 "
        "1.000000000000000036e-10\n"
        "body $-1 -1 $-1 $1 $-1 $2 #\n"
        "lump $-1 -1 $-1 $-1 $3 $0 #\n"
        "transform $-1 -1 "
        "%g %g %g "
        "%g %g %g "
        "%g %g %g "
        "%g %g %g "
        "1 no_rotate no_reflect no_shear #\n"
        "shell $-1 -1 $-1 $-1 $-1 $4 $-1 $1 #\n"
        "face $5 -1 $-1 $-1 $6 $3 $-1 $7 forward single #\n"
        "color-adesk-attrib $-1 -1 $-1 $-1 $4 256 #\n"
        "loop $-1 -1 $-1 $8 $9 $4 #\n"
        "torus-surface $-1 -1 $-1 0 0 0 0 0 1 %g %g 1 0 0 forward_v I I I I "
        "#\n"
        "loop $-1 -1 $-1 $-1 $10 $4 #\n"
        "coedge $-1 -1 $-1 $9 $9 $-1 $11 reversed $6 $-1 #\n"
        "coedge $-1 -1 $-1 $10 $10 $-1 $12 reversed $8 $-1 #\n"
        "edge $13 -1 $-1 $14 1 $14 0 $9 $-1 forward @7 unknown #\n"
        "edge $15 -1 $-1 $16 1 $16 0 $10 $-1 forward @7 unknown #\n"
        "color-adesk-attrib $-1 -1 $-1 $-1 $11 256 #\n"
        "vertex $-1 -1 $-1 $11 $17 #\n"
        "color-adesk-attrib $-1 -1 $-1 $-1 $12 256 #\n"
        "vertex $-1 -1 $-1 $12 $18 #\n"
        "point $-1 -1 $-1 0 0 %g #\n"
        "point $-1 -1 $-1 0 0 %g #\n"
        "End-of-ACIS-data\n";
    dwg_geom_normal_to_matrix9 (normal, &matrix);
    snprintf (acis_data, 1048, torus_acis_format, date_size, date, matrix[0],
              matrix[1], matrix[2], matrix[3], matrix[4], matrix[5], matrix[6],
              matrix[7], matrix[8], origin_pt->x, origin_pt->y, origin_pt->z,
              major_radius, minor_radius, major_radius, -major_radius);
    solid = dwg_add_3DSOLID (blkhdr, acis_data);
    solid->wireframe_data_present = 1;
    solid->point_present = 1;
    solid->point.x = origin_pt->x;
    solid->point.y = origin_pt->y;
    solid->point.z = origin_pt->z;
    solid->acis_empty_bit = 1;

    _obj = dwg_add_ACSH_TORUS_CLASS (
        (Dwg_Object_EVALUATION_GRAPH *)(void *)solid, origin_pt,
        normal ? normal : &defnormal, major_radius, minor_radius);
    ACSH_init_evalgraph (dwg, _obj, solid);
    return solid;
  }
}

EXPORT Dwg_Object_ACSH_WEDGE_CLASS *
dwg_add_ACSH_WEDGE_CLASS (Dwg_Object_EVALUATION_GRAPH *restrict evalgraph,
                          const dwg_point_3d *restrict origin_pt,
                          const dwg_point_3d *restrict normal,
                          const double length, const double width,
                          const double height)
{
  int err;
  Dwg_Object *hdr = dwg_obj_generic_to_object (evalgraph, &err);
  Dwg_Data *dwg = hdr ? hdr->parent : NULL;
  if (!dwg)
    return NULL;
  {
    API_ADD_OBJECT (ACSH_WEDGE_CLASS);
    dwg_init_ACSH_CLASS (dwg, obj, _obj, evalgraph, origin_pt, normal);
    _obj->length = length;
    _obj->width = width;
    _obj->height = height;
    return _obj;
  }
}

EXPORT Dwg_Entity_3DSOLID *
dwg_add_WEDGE (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
               const dwg_point_3d *restrict origin_pt,
               const dwg_point_3d *restrict normal, const double length,
               const double width, const double height)
{
  int err;
  Dwg_Data *dwg;
  {
    Dwg_Object *hdr = dwg_obj_generic_to_object (blkhdr, &err);
    dwg = hdr ? hdr->parent : NULL;
    if (dwg)
      {
        REQUIRE_CLASS ("ACAD_EVALUATION_GRAPH");
        REQUIRE_CLASS ("ACSH_HISTORY_CLASS");
        REQUIRE_CLASS ("ACSH_WEDGE_CLASS");
      }
    else
      return NULL;
  }
  {
    Dwg_Entity_3DSOLID *solid;
    Dwg_Object_ACSH_WEDGE_CLASS *_obj;
    dwg_point_3d defnormal = { 0.0, 0.0, 1.0 };
    dwg_matrix9 matrix = { 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0 };
#define WEDGE_ACIS_SIZE 6500
    char acis_data[WEDGE_ACIS_SIZE];
    char date[48];
    size_t date_size = dwg_acis_date (date, 48);
    // the center is in the middle
    const double l2 = length / 2;            // 1.674079
    const double w2 = width / 2;             // 1.244932
    const double h2 = height / 2;            // 2.4430318
    const double c1 = 0.8249088118009861859; // ??
    const double c2 = 0.5652658243809589589; // ??
    // acis version 106 (r14) would be nicer
    const char wedge_acis_format[] = /* len => 4332 - 5300 */
        // version num_records num_entities has_history
        "400 87 1 0 \n"
        // product acis_version date
        "8 LibreDWG 19 ASM 223.0.1.1930 NT %" PRIuSIZE " %s \n"
        // num_mm_units resabs resnor
        "25.39999999999999858 9.999999999999999547e-07 "
        "1.000000000000000036e-10\n"
        "body $-1 -1 $-1 $1 $-1 $2 #\n"
        "lump $-1 -1 $-1 $-1 $3 $0 #\n"
        "transform $-1 -1 "
        "%g %g %g "
        "%g %g %g "
        "%g %g %g "
        "%g %g %g "
        "1 no_rotate no_reflect no_shear #\n"
        "shell $-1 -1 $-1 $-1 $-1 $4 $-1 $1 #\n"
        "face $5 -1 $-1 $-1 $-1 $3 $-1 $6 forward single #\n"
        "persubent-acadSolidHistory-attrib $-1 $-1 $-1 $4 1 1 1 0 #\n"
        "face $9 $10 $11 $3 $-1 $12 forward single #\n"
        "loop $-1 $-1 $13 $4 #\n"
        // -l2 w2 -h2, -c1 0 c2, -c2 -0 -c1
        "plane-surface $-1 "
        "%f %f %f "
        "%f %d %f "
        "%f %d %f "
        "forward_v I I I I #\n"
        "persubent-acadSolidHistory-attrib $-1 $-1 $-1 $6 1 1 5 0 #\n"
        "face $14 $15 $16 $3 $-1 $17 forward single #\n"
        "loop $-1 $-1 $18 $6 #\n"
        // l2 w2 h2
        "plane-surface $-1 "
        "%f %f %f "
        "1 0 0 "
        "0 0 1 "
        "forward_v I I I I #\n"
        "coedge $-1 $19 $20 $21 $22 forward $7 $-1 #\n"
        "persubent-acadSolidHistory-attrib $-1 $-1 $-1 $10 1 1 9 0 #\n"
        "face $23 $24 $25 $3 $-1 $26 reversed single #\n"
        "loop $-1 $-1 $27 $10 #\n"
        // l2 w2 -h2
        "plane-surface $-1 "
        "%f %f %f "
        "0 0 -1 "
        "1 0 0 "
        "forward_v I I I I #\n"
        "coedge $-1 $28 $29 $30 $31 forward $11 $-1 #\n"
        "coedge $-1 $32 $13 $29 $33 reversed $7 $-1 #\n"
        "coedge $-1 $13 $32 $34 $35 forward $7 $-1 #\n"
        "coedge $-1 $36 $30 $13 $22 reversed $25 $-1 #\n"
        "edge $37 $38 $39 $13 $40 forward #\n"
        "persubent-acadSolidHistory-attrib $-1 $-1 $-1 $15 1 1 13 0 #\n"
        "face $41 $-1 $42 $3 $-1 $43 forward single #\n"
        "loop $-1 $-1 $21 $15 #\n"
        // 0 -w2 0
        "plane-surface $-1 "
        "%d %f %d "
        "0 1 0 "
        "0 0 -1 "
        "forward_v I I I I #\n"
        "coedge $-1 $34 $44 $36 $45 forward $16 $-1 #\n"
        "coedge $-1 $46 $18 $44 $47 reversed $11 $-1 #\n"
        "coedge $-1 $18 $46 $19 $33 forward $11 $-1 #\n"
        "coedge $-1 $21 $36 $18 $31 reversed $25 $-1 #\n"
        "edge $48 $39 $49 $18 $50 forward #\n"
        "coedge $-1 $20 $19 $51 $52 reversed $7 $-1 #\n"
        "edge $53 $54 $39 $29 $55 forward #\n"
        "coedge $-1 $56 $27 $20 $35 reversed $16 $-1 #\n"
        "edge $57 $58 $38 $20 $59 forward #\n"
        "coedge $-1 $30 $21 $27 $45 reversed $25 $-1 #\n"
        "persubent-acadSolidHistory-attrib $-1 $-1 $-1 $22 1 1 2 0 #\n"
        "vertex $60 $35 $61 #\n"
        "vertex $62 $33 $63 #\n"
        // -l2 -w2 -h2 c2 0 c1
        "straight-curve $-1 "
        "%f %f %f "
        "%f %d %f "
        " I I #\n"
        "persubent-acadSolidHistory-attrib $-1 $-1 $-1 $24 1 1 14 0 #\n"
        "loop $-1 $-1 $64 $24 #\n"
        // 0 w2 0
        "plane-surface $-1 "
        "%d %f %d "
        "0 1 0 "
        "0 0 -1 "
        "forward_v I I I I #\n"
        "coedge $-1 $27 $56 $28 $47 forward $16 $-1 #\n"
        "edge $65 $49 $38 $27 $66 forward #\n"
        "coedge $-1 $29 $28 $67 $68 reversed $11 $-1 #\n"
        "edge $69 $70 $49 $44 $71 forward #\n"
        "persubent-acadSolidHistory-attrib $-1 $-1 $-1 $31 1 1 6 0 #\n"
        "vertex $72 $47 $73 #\n"
        // l2 -w2 h2
        "straight-curve $-1 "
        "%f %f %f "
        "0 0 -1 "
        "I I #\n"
        "coedge $-1 $67 $64 $32 $52 forward $42 $-1 #\n"
        "edge $74 $58 $54 $51 $75 forward #\n"
        "persubent-acadSolidHistory-attrib $-1 $-1 $-1 $33 1 1 7 0 #\n"
        "vertex $76 $52 $77 #\n"
        // l2 w2 h2
        "straight-curve $-1 "
        "%f %f %f "
        "0 -1 0 "
        "I I #\n"
        "coedge $-1 $44 $34 $64 $78 reversed $16 $-1 #\n"
        "persubent-acadSolidHistory-attrib $-1 $-1 $-1 $35 1 1 3 0 #\n"
        "vertex $79 $78 $80 #\n"
        // -l2 w2 -h2
        "straight-curve $-1 "
        "%f %f %f "
        "0 -1 0 "
        "I I #\n"
        "persubent-acadSolidHistory-attrib $-1 $-1 $-1 $38 1 1 4 0 #\n"
        // -l2 -w2 -h2
        "point $-1 %f %f %f #\n"
        "persubent-acadSolidHistory-attrib $-1 $-1 $-1 $39 1 1 8 0 #\n"
        // l2 -w2 h2
        "point $-1 %f %f %f #\n"
        "coedge $-1 $51 $67 $56 $78 forward $42 $-1 #\n"
        "persubent-acadSolidHistory-attrib $-1 $-1 $-1 $45 1 1 10 0 #\n"
        // l2 -w2 -h2
        "straight-curve $-1 %f %f %f -1 0 0 I I #\n"
        "coedge $-1 $64 $51 $46 $68 forward $42 $-1 #\n"
        "edge $81 $54 $70 $67 $82 forward #\n"
        "persubent-acadSolidHistory-attrib $-1 $-1 $-1 $47 1 1 11 0 #\n"
        "vertex $83 $78 $84 #\n"
        // l2 w2 -h2
        "straight-curve $-1 %f %f %f 0 -1 0 I I #\n"
        "persubent-acadSolidHistory-attrib $-1 $-1 $-1 $49 1 1 12 0 #\n"
        // l2 -w2 -h2
        "point $-1 %f %f %f #\n"
        "persubent-acadSolidHistory-attrib $-1 $-1 $-1 $52 1 1 18 0 #\n"
        // -l2 w2 -h2 c2 0 c1
        "straight-curve $-1 %f %f %f %f %d %f I I #\n"
        "persubent-acadSolidHistory-attrib $-1 $-1 $-1 $54 1 1 19 0 #\n"
        // l2 w2 h2
        "point $-1 %f %f %f #\n"
        "edge $85 $70 $58 $64 $86 forward #\n"
        "persubent-acadSolidHistory-attrib $-1 $-1 $-1 $58 1 1 17 0 #\n"
        // -l2 w2 -h2
        "point $-1 %f %f %f #\n"
        "persubent-acadSolidHistory-attrib $-1 $-1 $-1 $68 1 1 20 0 #\n"
        // l2 w2 h2
        "straight-curve $-1 %f %f %f 0 0 -1 I I #\n"
        "persubent-acadSolidHistory-attrib $-1 $-1 $-1 $70 1 1 16 0 #\n"
        // l2 w2 -h2
        "point $-1 %f %f %f #\n"
        "persubent-acadSolidHistory-attrib $-1 $-1 $-1 $78 1 1 15 0 #\n"
        // l2 w2 -h2
        "straight-curve $-1 %f %f %f -1 0 0 I I #\n"
        "End-of-ACIS-data\n";
    // origin: 8.325921, 8.755068, 2.443032
    // matrix -1 0 0, 0 -1 0, 0 0 1
    // length: 3.348158
    // width: 2.489864
    // height: 4.886064
    dwg_geom_normal_to_matrix9 (normal, &matrix);
    snprintf (acis_data, WEDGE_ACIS_SIZE, wedge_acis_format, date_size, date,
              matrix[0], matrix[1], matrix[2], matrix[3], matrix[4], matrix[5],
              matrix[6], matrix[7], matrix[8], origin_pt->x, origin_pt->y,
              origin_pt->z,
              // clang-format off
              -l2, w2, -h2, -c1, 0, c2, -c2, -0, -c1,
              l2, w2, h2,
              l2, w2, -h2,
              0, -w2, 0,
              -l2, -w2, -h2, c2, 0, c1,
              0, w2, 0,
              l2, -w2, h2,
              l2, w2, h2,
              -l2, w2, -h2,
              -l2, -w2, -h2,
              l2, -w2, h2,
              l2, -w2, -h2,
              l2, w2, -h2,
              l2, -w2, -h2,
              -l2, w2, -h2, c2, 0, c1,
              l2, w2, h2,
              -l2, w2, -h2,
              l2, w2, h2,
              l2, w2, -h2,
              l2, w2, -h2
              // clang-format on
    );
    solid = dwg_add_3DSOLID (blkhdr, acis_data);
    solid->wireframe_data_present = 1;
    solid->point_present = 1;
    ADD_CHECK_3DPOINT (origin_pt);
    solid->point.x = origin_pt->x;
    solid->point.y = origin_pt->y;
    solid->point.z = origin_pt->z;
    solid->acis_empty_bit = 1;

    _obj = dwg_add_ACSH_WEDGE_CLASS (
        (Dwg_Object_EVALUATION_GRAPH *)(void *)solid, origin_pt,
        normal ? normal : &defnormal, length, width, height);
    ACSH_init_evalgraph (dwg, _obj, solid);
    return solid;
  }
}

// ALDIMOBJECTCONTEXTDATA
// ALIGNMENTPARAMETERENTITY
// ANGDIMOBJECTCONTEXTDATA
// ANNOTSCALEOBJECTCONTEXTDATA
// ARC_DIMENSION  (needed)
// ARCALIGNEDTEXT
// ASSOC2DCONSTRAINTGROUP
// ASSOC3POINTANGULARDIMACTIONBODY
// ASSOCACTION
// ASSOCACTIONPARAM
// ASSOCALIGNEDDIMACTIONBODY
// ASSOCARRAYACTIONBODY
// ASSOCARRAYMODIFYACTIONBODY
// ASSOCARRAYMODIFYPARAMETERS
// ASSOCARRAYPATHPARAMETERS
// ASSOCARRAYPOLARPARAMETERS
// ASSOCARRAYRECTANGULARPARAMETERS
// ASSOCASMBODYACTIONPARAM
// ASSOCBLENDSURFACEACTIONBODY
// ASSOCCOMPOUNDACTIONPARAM
// ASSOCDEPENDENCY
// ASSOCDIMDEPENDENCYBODY
// ASSOCEDGEACTIONPARAM
// ASSOCEDGECHAMFERACTIONBODY
// ASSOCEDGEFILLETACTIONBODY
// ASSOCEXTENDSURFACEACTIONBODY
// ASSOCEXTRUDEDSURFACEACTIONBODY
// ASSOCFACEACTIONPARAM
// ASSOCFILLETSURFACEACTIONBODY
// ASSOCGEOMDEPENDENCY
// ASSOCLOFTEDSURFACEACTIONBODY
// ASSOCMLEADERACTIONBODY
// ASSOCNETWORK
// ASSOCNETWORKSURFACEACTIONBODY
// ASSOCOBJECTACTIONPARAM
// ASSOCOFFSETSURFACEACTIONBODY
// ASSOCORDINATEDIMACTIONBODY
// ASSOCOSNAPPOINTREFACTIONPARAM
// ASSOCPATCHSURFACEACTIONBODY
// ASSOCPATHACTIONPARAM
// ASSOCPERSSUBENTMANAGER
// ASSOCPLANESURFACEACTIONBODY
// ASSOCPOINTREFACTIONPARAM
// ASSOCRESTOREENTITYSTATEACTIONBODY
// ASSOCREVOLVEDSURFACEACTIONBODY
// ASSOCROTATEDDIMACTIONBODY
// ASSOCSWEPTSURFACEACTIONBODY
// ASSOCTRIMSURFACEACTIONBODY
// ASSOCVALUEDEPENDENCY
// ASSOCVARIABLE
// ASSOCVERTEXACTIONPARAM
// BACKGROUND
// BASEPOINTPARAMETERENTITY
// BLKREFOBJECTCONTEXTDATA
// BLOCKALIGNEDCONSTRAINTPARAMETER
// BLOCKALIGNMENTGRIP
// BLOCKALIGNMENTPARAMETER
// BLOCKANGULARCONSTRAINTPARAMETER
// BLOCKARRAYACTION
// BLOCKBASEPOINTPARAMETER
// BLOCKDIAMETRICCONSTRAINTPARAMETER
// BLOCKFLIPACTION
// BLOCKFLIPGRIP
// BLOCKFLIPPARAMETER
// BLOCKGRIPLOCATIONCOMPONENT
// BLOCKHORIZONTALCONSTRAINTPARAMETER
// BLOCKLINEARCONSTRAINTPARAMETER
// BLOCKLINEARGRIP
// BLOCKLINEARPARAMETER
// BLOCKLOOKUPACTION
// BLOCKLOOKUPGRIP
// BLOCKLOOKUPPARAMETER
// BLOCKMOVEACTION
// BLOCKPARAMDEPENDENCYBODY
// BLOCKPOINTPARAMETER
// BLOCKPOLARGRIP
// BLOCKPOLARPARAMETER
// BLOCKPOLARSTRETCHACTION
// BLOCKPROPERTIESTABLE
// BLOCKPROPERTIESTABLEGRIP
// BLOCKRADIALCONSTRAINTPARAMETER
// BLOCKREPRESENTATION
// BLOCKROTATEACTION
// BLOCKROTATIONGRIP
// BLOCKROTATIONPARAMETER
// BLOCKSCALEACTION
// BLOCKSTRETCHACTION
// BLOCKUSERPARAMETER
// BLOCKVERTICALCONSTRAINTPARAMETER
// BLOCKVISIBILITYGRIP
// BLOCKVISIBILITYPARAMETER
// BLOCKXYGRIP
// BLOCKXYPARAMETER
// CAMERA
// CELLSTYLEMAP
// CONTEXTDATAMANAGER
// CSACDOCUMENTOPTIONS
// CURVEPATH
// DATALINK
// DATATABLE
// DBCOLOR
// DETAILVIEWSTYLE
// DICTIONARYVAR
// DIMASSOC
// DMDIMOBJECTCONTEXTDATA
// DYNAMICBLOCKPROXYNODE
// DYNAMICBLOCKPURGEPREVENTER
// EXTRUDEDSURFACE
// FCFOBJECTCONTEXTDATA
// FIELD
// FIELDLIST
// FLIPPARAMETERENTITY
// GEODATA
// GEOMAPIMAGE
// GEOPOSITIONMARKER
// HELIX
// IDBUFFER

// Called Raster in VBA
EXPORT Dwg_Entity_IMAGE *
dwg_add_IMAGE (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
               const char *restrict file_path,
               const dwg_point_3d *restrict ins_pt, const double scale_factor,
               const double rotation_angle)
{
  Dwg_Object *img;
  Dwg_Entity_IMAGE *_img;
  // Dwg_Object *imgdef;
  {
    int err;
    Dwg_Object *hdr = dwg_obj_generic_to_object (blkhdr, &err);
    Dwg_Data *dwg = hdr ? hdr->parent : NULL;
    if (dwg)
      {
        REQUIRE_CLASS ("IMAGEDEF");
        REQUIRE_CLASS ("IMAGEDEF_REACTOR");
        REQUIRE_CLASS ("IMAGE");
      }
  }
  {
    API_ADD_ENTITY (IMAGE);
    img = obj;
    if (!img)
      return NULL;
    _img = _obj;
    ADD_CHECK_3DPOINT (ins_pt);
    _obj->pt0.x = ins_pt->x;
    _obj->pt0.y = ins_pt->y;
    _obj->pt0.z = ins_pt->z;
    ADD_CHECK_DOUBLE (scale_factor);
    // TODO rotation cos()
    // ADD_CHECK_ANGLE (_obj->rotation);
    _obj->uvec.x = scale_factor;
    _obj->uvec.y = scale_factor;
    _obj->uvec.z = 1.0;
    _obj->vvec.x = scale_factor;
    _obj->vvec.y = scale_factor;
    _obj->vvec.z = 1.0;
    _obj->brightness = 0x32;
    _obj->contrast = 0x32;
  }
  // TODO normally a DICTIONARY owns an IMAGEDEF
  {
    Dwg_Data *dwg = img->parent;
    API_ADD_OBJECT (IMAGEDEF);
    // imgdef = obj;
    //_obj->class_version = 0;
    _obj->file_path = dwg_add_u8_input (dwg, file_path);
    // TODO: get pixel props from the image. is_loaded, pixel_size, ...
    // needs -lpng -ljpeg ... load dynamically?
    _img->imagedef = dwg_add_handleref (dwg, 4, obj->handle.value, img);
  }
  {
    Dwg_Data *dwg = img->parent;
    API_ADD_OBJECT (IMAGEDEF_REACTOR);
    obj->tio.object->ownerhandle
        = dwg_add_handleref (dwg, 5, img->handle.value, obj);
    _obj->class_version = 2;
    _img->imagedefreactor = dwg_add_handleref (dwg, 3, obj->handle.value, img);
  }
  return _img;
}

// Not in VBA
// Searches all PDFDEFINITION's for filename, if not found create a new one.
EXPORT Dwg_Entity_PDFUNDERLAY *
dwg_add_PDFUNDERLAY (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                     const char *restrict filename,
                     const dwg_point_3d *restrict ins_pt,
                     const double scale_factor, const double rotation_angle)
{
  Dwg_Object *und, *dict = NULL, *def = NULL;
  Dwg_Object_DICTIONARY *_dict;
  Dwg_Data *_dwg;
  char name[80], *base, *ext;
  unsigned int i;
  {
    int err;
    Dwg_Object *hdr = dwg_obj_generic_to_object (blkhdr, &err);
    Dwg_Data *dwg = _dwg = hdr ? hdr->parent : NULL;
    char base_1[80];
    Dwg_Object *defs;
    Dwg_Object_DICTIONARY *_defs;
    BITCODE_H defsref;

    if (!dwg || err)
      return NULL;
    i = 1;
    REQUIRE_CLASS ("PDFDEFINITION");
    REQUIRE_CLASS ("PDFUNDERLAY");
    // name = "dxf - 1";
    // search/register in NOD
    base = split_filepath (filename, &ext);
    snprintf (name, 80, "%d", i);
    snprintf (base_1, 80, "%s - %d", base, i);
    defsref = dwg_find_dictionary (dwg, "ACAD_PDFDEFINITIONS");
    if (!defsref) // no nod entry yet, create
      _dict = dwg_add_DICTIONARY (dwg, "ACAD_PDFDEFINITIONS", base_1, 0);
    else
      {
        // PDFDEFINITIONS dict exists
        dict = dwg_ref_object (dwg, defsref);
        _dict = dict->tio.object->tio.DICTIONARY;
        // check if a PDFDEFINITION for this filename already exists.
        // if same path: re-use it. if same base: inc i and name.
        for (unsigned j = 0; j < _dict->numitems; j++)
          {
            BITCODE_T text = _dict->texts[j];
            Dwg_Object_Ref *ref = _dict->itemhandles[j];
            Dwg_Object *o = dwg_ref_object (dwg, ref);
            if (o && o->fixedtype == DWG_TYPE_PDFDEFINITION)
              {
                Bit_Chain dat = { 0 };
                Dwg_Object_PDFDEFINITION *_def
                    = o->tio.object->tio.PDFDEFINITION;
                char text1[80];
                int i1;
                // for IS_FROM_TU in bit_eq_T: don't convert with DXF
                dat.from_version = dwg->header.from_version;
                dat.opts = dwg->opts;
                if (bit_eq_T (&dat, _def->filename, filename))
                  {
                    // same filename: re-use
                    def = o;
                    break;
                  }
                // same base: i++ and inc name
                sscanf (text, "%s - %d", text1, &i1);
                if (strEQ (text1, base))
                  {
                    i++;
                    snprintf (name, 80, "%d", i);
                    snprintf (base_1, 80, "%s - %d", base, i);
                    break;
                  }
              }
          }
        if (!def) // matching def + dict not found, add entry
          dwg_add_DICTIONARY_item (_dict, base_1, 0);
      }
    if (!def)
      {
        API_ADD_OBJECT (PDFDEFINITION);
        if (!_obj)
          return NULL;
        def = obj;
        _dict->itemhandles[_dict->numitems - 1]
            = dwg_add_handleref (dwg, 2, obj->handle.value, NULL);
        _obj->filename = dwg_add_u8_input (dwg, filename);
        _obj->name = STRDUP (name);
        if (!dict)
          dict = dwg_obj_generic_to_object (_dict, &error);
        if (dict)
          {
            obj->tio.object->ownerhandle
                = dwg_add_handleref (dwg, 4, dict->handle.value, obj);
            add_obj_reactor (obj->tio.object, dict->handle.value);
          }
      }
  }
  {
    API_ADD_ENTITY (PDFUNDERLAY);
    if (!_obj)
      return NULL;
    _obj->definition_id = dwg_add_handleref (dwg, 5, def->handle.value, obj);
    ADD_CHECK_3DPOINT (ins_pt);
    _obj->ins_pt.x = ins_pt->x;
    _obj->ins_pt.y = ins_pt->y;
    _obj->ins_pt.z = ins_pt->z;
    _obj->angle = rotation_angle;
    ADD_CHECK_ANGLE (_obj->angle);
    ADD_CHECK_DOUBLE (scale_factor);
    _obj->scale.x = scale_factor;
    _obj->scale.y = scale_factor;
    _obj->scale.z = 1.0;
    _obj->contrast = 100;
    return _obj;
  }
}

// INDEX

EXPORT Dwg_Entity_LARGE_RADIAL_DIMENSION *
dwg_add_LARGE_RADIAL_DIMENSION (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                                const dwg_point_3d *restrict center_pt,
                                const dwg_point_3d *restrict chord_pt,
                                const dwg_point_3d *restrict ovr_center,
                                const dwg_point_3d *restrict jog_pt,
                                const double jog_angle)
{
  API_ADD_ENTITY (LARGE_RADIAL_DIMENSION);
  _obj->def_pt.x = center_pt->x;
  _obj->def_pt.y = center_pt->y;
  _obj->def_pt.z = center_pt->z;
  _obj->chord_pt.x = chord_pt->x;
  _obj->chord_pt.y = chord_pt->y;
  _obj->chord_pt.z = chord_pt->z;
  _obj->ovr_center.x = ovr_center->x;
  _obj->ovr_center.y = ovr_center->y;
  _obj->ovr_center.z = ovr_center->z;
  _obj->jog_pt.x = jog_pt->x;
  _obj->jog_pt.y = jog_pt->y;
  _obj->jog_pt.z = jog_pt->z;
  _obj->jog_angle = jog_angle;
  return _obj;
}

// TODO, no coverage
// LAYER_CONTROL->owner DICTIONARY with ACAD_LAYERFILTERS => DICTIONARY,
// which is xdicobjhandle of LAYER_CONTROL
EXPORT Dwg_Object_LAYERFILTER *
dwg_add_LAYERFILTER (Dwg_Data *restrict dwg /* ... */)
{
  API_ADD_OBJECT (LAYERFILTER);
  return _obj;
}

// no coverage
EXPORT Dwg_Object_LAYER_INDEX *
dwg_add_LAYER_INDEX (Dwg_Data *restrict dwg /* ... */)
{
  {
    REQUIRE_CLASS ("LAYER_INDEX");
  }
  {
    API_ADD_OBJECT (LAYER_INDEX);
    return _obj;
  }
}

// Untested, experimental.
// INSERT.xdicobjhandle ->
//   DICT_item "ACAD_FILTER" ->
//   DICT_item "SPATIAL" -> obj
EXPORT Dwg_Object_SPATIAL_FILTER *
dwg_add_SPATIAL_FILTER (
    Dwg_Entity_INSERT *restrict insert /*, clip_verts... */)
{
  int err;
  Dwg_Object *ins;
  Dwg_Object_DICTIONARY *_filter, *_spatial;
  Dwg_Object *filter, *spatial;
  Dwg_Data *dwg;

  ins = dwg_ent_generic_to_object (insert, &err);
  dwg = ins ? ins->parent : NULL;
  if (!dwg || err)
    return NULL;
  {
    REQUIRE_CLASS ("SPATIAL_FILTER");

    _filter = dwg_add_DICTIONARY (dwg, NULL, "ACAD_FILTER", 0);
    filter = dwg_obj_generic_to_object (_filter, &err);
    filter->tio.object->ownerhandle
        = dwg_add_handleref (dwg, 5, ins->handle.value, filter);
    _filter->is_hardowner = 1;

    ins->tio.entity->xdicobjhandle
        = dwg_add_handleref (dwg, 3, filter->handle.value, ins);

    _spatial = dwg_add_DICTIONARY (dwg, NULL, "SPATIAL", 0);
    _spatial->is_hardowner = 1;
    spatial = dwg_obj_generic_to_object (_spatial, &err);
    _filter->itemhandles[0]
        = dwg_add_handleref (dwg, 2, spatial->handle.value, filter);
    spatial->tio.object->ownerhandle
        = dwg_add_handleref (dwg, 5, filter->handle.value, spatial);
    add_obj_reactor (spatial->tio.object, filter->handle.value);
  }
  {
    API_ADD_OBJECT (SPATIAL_FILTER);
    _spatial->itemhandles[0]
        = dwg_add_handleref (dwg, 2, obj->handle.value, filter);
    obj->tio.object->ownerhandle
        = dwg_add_handleref (dwg, 5, spatial->handle.value, obj);
    add_obj_reactor (obj->tio.object, spatial->handle.value);
    // TODO normal -> matrix
    _obj->transform[0] = 1.0;
    _obj->transform[5] = 1.0;
    _obj->transform[10] = 1.0;
    return _obj;
  }
}

// no coverage
EXPORT Dwg_Object_SPATIAL_INDEX *
dwg_add_SPATIAL_INDEX (Dwg_Data *restrict dwg /* ... */)
{
  {
    REQUIRE_CLASS ("SPATIAL_INDEX");
  }
  {
    API_ADD_OBJECT (SPATIAL_INDEX);
    return _obj;
  }
}

// LEADEROBJECTCONTEXTDATA
// LIGHT
// LIGHTLIST
// LINEARPARAMETERENTITY
// LOFTEDSURFACE
// MATERIAL
// MENTALRAYRENDERSETTINGS
// MESH (needed)
// MLEADEROBJECTCONTEXTDATA
// MLEADERSTYLE
// MOTIONPATH
// MPOLYGON
// MTEXTATTRIBUTEOBJECTCONTEXTDATA
// MTEXTOBJECTCONTEXTDATA
// MULTILEADER (needed)
// NAVISWORKSMODEL
// NAVISWORKSMODELDEF
// NPOCOLLECTION
// NURBSURFACE
// OBJECT_PTR
// ORDDIMOBJECTCONTEXTDATA
// PERSUBENTMGR
// PLANESURFACE
// PLOTSETTINGS
// POINTCLOUD
// POINTCLOUDEX
// POINTCLOUDDEF
// POINTCLOUDDEFEX
// POINTCLOUDDEF_REACTOR
// POINTCLOUDDEF_REACTOR_EX
// POINTCLOUDCOLORMAP
// POINTPARAMETERENTITY
// POINTPATH
// RADIMLGOBJECTCONTEXTDATA
// RADIMOBJECTCONTEXTDATA
// RAPIDRTRENDERSETTINGS
// RASTERVARIABLES
// RENDERENTRY
// RENDERENVIRONMENT
// RENDERGLOBAL
// RENDERSETTINGS
// REVOLVEDSURFACE
// ROTATIONPARAMETERENTITY
// RTEXT
// SCALE (needed)
// SECTIONOBJECT (needed)
// SECTIONVIEWSTYLE
// SECTION_MANAGER
// SECTION_SETTINGS
// SORTENTSTABLE
// SUN
// SUNSTUDY
// SWEPTSURFACE
// TABLE (needed)
// TABLECONTENT
// TABLEGEOMETRY
// TABLESTYLE (needed)
// TEXTOBJECTCONTEXTDATA
// TVDEVICEPROPERTIES
// VISIBILITYGRIPENTITY
// VISIBILITYPARAMETERENTITY
// VISUALSTYLE
// WIPEOUT

// just for testing dwg_type_dxfname()
EXPORT Dwg_Object_WIPEOUTVARIABLES *
dwg_add_WIPEOUTVARIABLES (Dwg_Data *dwg /* ... */)
{
  {
    REQUIRE_CLASS ("WIPEOUTVARIABLES");
  }
  {
    API_ADD_OBJECT (WIPEOUTVARIABLES);
    return _obj;
  }
}

// XREFPANELOBJECT
// XYPARAMETERENTITY

// #endif /* USE_WRITE */
