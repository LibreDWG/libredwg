/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2010,2011,2018 Free Software Foundation, Inc.              */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * dwg.i: SWIG interface file
 * written by Rodrigo Rodrigues da Silva
 * ideas contributed by James Michael DuPont
 * modified by Reini Urban
 */

%module libredwg

%{
#include "../src/config.h"
#include "dwg_api.h"
%}

%include "carrays.i"
%array_functions(Dwg_Object, Dwg_Object_Array);

%include "dwg.h"
//too large
//%include "dwg_api.h"

EXPORT Dwg_Object_Ref **
dwg_object_tablectrl_get_entries(const Dwg_Object *restrict obj,
                                 int *restrict error);
EXPORT char *
dwg_ref_get_table_name(const Dwg_Object_Ref *restrict ref,
                       int *restrict error);
EXPORT char *
dwg_ent_get_layer_name(const dwg_obj_ent *restrict ent,
                       int *restrict error);
EXPORT char *
dwg_obj_layer_get_name(const dwg_obj_layer *restrict layer,
                          int *restrict error);
EXPORT char *
dwg_obj_table_get_name(const Dwg_Object *restrict obj,
                          int *restrict error);
EXPORT Dwg_Object *
dwg_ent_to_object(const dwg_obj_ent *restrict obj,
                          int *restrict error);
EXPORT Dwg_Object_Object *
dwg_object_to_object(Dwg_Object *restrict obj,
                     int *restrict error);
EXPORT Dwg_Object *
dwg_obj_obj_to_object(const Dwg_Object_Object *restrict obj,
                      int *restrict error);
EXPORT Dwg_Object_Entity *
dwg_object_to_entity(Dwg_Object *restrict obj,
                     int *restrict error);
EXPORT int
dwg_object_get_type(const Dwg_Object *obj);
EXPORT int
dwg_object_get_fixedtype(const Dwg_Object *obj);
EXPORT char*
dwg_object_get_dxfname(const Dwg_Object *obj);
EXPORT BITCODE_BL
dwg_ref_get_absref(const Dwg_Object_Ref *restrict ref,
                   int *restrict error);
EXPORT Dwg_Object *
dwg_ref_get_object(const Dwg_Object_Ref *restrict ref,
                   int *restrict error);
EXPORT Dwg_Object *
dwg_absref_get_object(const Dwg_Data* dwg, const BITCODE_BL absref);
EXPORT unsigned int
dwg_get_num_classes(const Dwg_Data *dwg);
EXPORT Dwg_Class *
dwg_get_class(const Dwg_Data *dwg, unsigned int index);
EXPORT Dwg_Object *
dwg_get_object(Dwg_Data *dwg, BITCODE_BL index);

struct dwg_entity_DIMENSION_common *
dwg_object_to_DIMENSION(Dwg_Object *obj);

EXPORT Dwg_Entity_TEXT** dwg_get_TEXT (Dwg_Object_Ref* hdr);
EXPORT Dwg_Entity_ATTRIB** dwg_get_ATTRIB (Dwg_Object_Ref* hdr);
EXPORT Dwg_Entity_ATTDEF** dwg_get_ATTDEF (Dwg_Object_Ref* hdr);
EXPORT Dwg_Entity_BLOCK** dwg_get_BLOCK (Dwg_Object_Ref* hdr);
EXPORT Dwg_Entity_ENDBLK** dwg_get_ENDBLK (Dwg_Object_Ref* hdr);
EXPORT Dwg_Entity_SEQEND** dwg_get_SEQEND (Dwg_Object_Ref* hdr);
EXPORT Dwg_Entity_INSERT** dwg_get_INSERT (Dwg_Object_Ref* hdr);
EXPORT Dwg_Entity_MINSERT** dwg_get_MINSERT (Dwg_Object_Ref* hdr);
EXPORT Dwg_Entity_VERTEX_2D** dwg_get_VERTEX_2D (Dwg_Object_Ref* hdr);
EXPORT Dwg_Entity_VERTEX_3D** dwg_get_VERTEX_3D (Dwg_Object_Ref* hdr);
EXPORT Dwg_Entity_VERTEX_MESH** dwg_get_VERTEX_MESH (Dwg_Object_Ref* hdr);
EXPORT Dwg_Entity_VERTEX_PFACE** dwg_get_VERTEX_PFACE (Dwg_Object_Ref* hdr);
EXPORT Dwg_Entity_VERTEX_PFACE_FACE** dwg_get_VERTEX_PFACE_FACE (Dwg_Object_Ref* hdr);
EXPORT Dwg_Entity_POLYLINE_2D** dwg_get_POLYLINE_2D (Dwg_Object_Ref* hdr);
EXPORT Dwg_Entity_POLYLINE_3D** dwg_get_POLYLINE_3D (Dwg_Object_Ref* hdr);
EXPORT Dwg_Entity_ARC** dwg_get_ARC (Dwg_Object_Ref* hdr);
EXPORT Dwg_Entity_CIRCLE** dwg_get_CIRCLE (Dwg_Object_Ref* hdr);
EXPORT Dwg_Entity_LINE** dwg_get_LINE (Dwg_Object_Ref* hdr);
EXPORT Dwg_Entity_DIMENSION_ORDINATE** dwg_get_DIMENSION_ORDINATE (Dwg_Object_Ref* hdr);
EXPORT Dwg_Entity_DIMENSION_LINEAR** dwg_get_DIMENSION_LINEAR (Dwg_Object_Ref* hdr);
EXPORT Dwg_Entity_DIMENSION_ALIGNED** dwg_get_DIMENSION_ALIGNED (Dwg_Object_Ref* hdr);
EXPORT Dwg_Entity_DIMENSION_ANG3PT** dwg_get_DIMENSION_ANG3PT (Dwg_Object_Ref* hdr);
EXPORT Dwg_Entity_DIMENSION_ANG2LN** dwg_get_DIMENSION_ANG2LN (Dwg_Object_Ref* hdr);
EXPORT Dwg_Entity_DIMENSION_RADIUS** dwg_get_DIMENSION_RADIUS (Dwg_Object_Ref* hdr);
EXPORT Dwg_Entity_DIMENSION_DIAMETER** dwg_get_DIMENSION_DIAMETER (Dwg_Object_Ref* hdr);
EXPORT Dwg_Entity_POINT** dwg_get_POINT (Dwg_Object_Ref* hdr);
EXPORT Dwg_Entity_POLYLINE_PFACE** dwg_get_POLYLINE_PFACE (Dwg_Object_Ref* hdr);
EXPORT Dwg_Entity_POLYLINE_MESH** dwg_get_POLYLINE_MESH (Dwg_Object_Ref* hdr);
EXPORT Dwg_Entity_SOLID** dwg_get_SOLID (Dwg_Object_Ref* hdr);
EXPORT Dwg_Entity_TRACE** dwg_get_TRACE (Dwg_Object_Ref* hdr);
EXPORT Dwg_Entity_SHAPE** dwg_get_SHAPE (Dwg_Object_Ref* hdr);
EXPORT Dwg_Entity_VIEWPORT** dwg_get_VIEWPORT (Dwg_Object_Ref* hdr);
EXPORT Dwg_Entity_ELLIPSE** dwg_get_ELLIPSE (Dwg_Object_Ref* hdr);
EXPORT Dwg_Entity_SPLINE** dwg_get_SPLINE (Dwg_Object_Ref* hdr);
EXPORT Dwg_Entity_REGION** dwg_get_REGION (Dwg_Object_Ref* hdr);
EXPORT Dwg_Entity_BODY** dwg_get_BODY (Dwg_Object_Ref* hdr);
EXPORT Dwg_Entity_RAY** dwg_get_RAY (Dwg_Object_Ref* hdr);
EXPORT Dwg_Entity_XLINE** dwg_get_XLINE (Dwg_Object_Ref* hdr);
EXPORT Dwg_Entity_OLEFRAME** dwg_get_OLEFRAME (Dwg_Object_Ref* hdr);
EXPORT Dwg_Entity_MTEXT** dwg_get_MTEXT (Dwg_Object_Ref* hdr);
EXPORT Dwg_Entity_LEADER** dwg_get_LEADER (Dwg_Object_Ref* hdr);
EXPORT Dwg_Entity_TOLERANCE** dwg_get_TOLERANCE (Dwg_Object_Ref* hdr);
EXPORT Dwg_Entity_MLINE** dwg_get_MLINE (Dwg_Object_Ref* hdr);
EXPORT Dwg_Entity_OLE2FRAME** dwg_get_OLE2FRAME (Dwg_Object_Ref* hdr);
EXPORT Dwg_Entity_LWPOLYLINE** dwg_get_LWPOLYLINE (Dwg_Object_Ref* hdr);
EXPORT Dwg_Entity_HATCH** dwg_get_HATCH (Dwg_Object_Ref* hdr);
EXPORT Dwg_Entity_IMAGE** dwg_get_IMAGE (Dwg_Object_Ref* hdr);
EXPORT Dwg_Entity_CAMERA** dwg_get_CAMERA (Dwg_Object_Ref* hdr);
EXPORT Dwg_Entity_SURFACE** dwg_get_SURFACE (Dwg_Object_Ref* hdr);
EXPORT Dwg_Entity_GEOPOSITIONMARKER** dwg_get_GEOPOSITIONMARKER (Dwg_Object_Ref* hdr);
EXPORT Dwg_Entity_LIGHT** dwg_get_LIGHT (Dwg_Object_Ref* hdr);
EXPORT Dwg_Entity_MULTILEADER** dwg_get_MULTILEADER (Dwg_Object_Ref* hdr);
EXPORT Dwg_Entity_TABLE** dwg_get_TABLE (Dwg_Object_Ref* hdr);
EXPORT Dwg_Entity_UNDERLAY** dwg_get_UNDERLAY (Dwg_Object_Ref* hdr);
EXPORT Dwg_Entity_WIPEOUT** dwg_get_WIPEOUT (Dwg_Object_Ref* hdr);
EXPORT Dwg_Entity_TEXT* dwg_object_to_TEXT (Dwg_Object* obj);
EXPORT Dwg_Entity_ATTRIB* dwg_object_to_ATTRIB (Dwg_Object* obj);
EXPORT Dwg_Entity_ATTDEF* dwg_object_to_ATTDEF (Dwg_Object* obj);
EXPORT Dwg_Entity_BLOCK* dwg_object_to_BLOCK (Dwg_Object* obj);
EXPORT Dwg_Entity_ENDBLK* dwg_object_to_ENDBLK (Dwg_Object* obj);
EXPORT Dwg_Entity_SEQEND* dwg_object_to_SEQEND (Dwg_Object* obj);
EXPORT Dwg_Entity_INSERT* dwg_object_to_INSERT (Dwg_Object* obj);
EXPORT Dwg_Entity_MINSERT* dwg_object_to_MINSERT (Dwg_Object* obj);
EXPORT Dwg_Entity_VERTEX_2D* dwg_object_to_VERTEX_2D (Dwg_Object* obj);
EXPORT Dwg_Entity_VERTEX_3D* dwg_object_to_VERTEX_3D (Dwg_Object* obj);
EXPORT Dwg_Entity_VERTEX_MESH* dwg_object_to_VERTEX_MESH (Dwg_Object* obj);
EXPORT Dwg_Entity_VERTEX_PFACE* dwg_object_to_VERTEX_PFACE (Dwg_Object* obj);
EXPORT Dwg_Entity_VERTEX_PFACE_FACE* dwg_object_to_VERTEX_PFACE_FACE (Dwg_Object* obj);
EXPORT Dwg_Entity_POLYLINE_2D* dwg_object_to_POLYLINE_2D (Dwg_Object* obj);
EXPORT Dwg_Entity_POLYLINE_3D* dwg_object_to_POLYLINE_3D (Dwg_Object* obj);
EXPORT Dwg_Entity_ARC* dwg_object_to_ARC (Dwg_Object* obj);
EXPORT Dwg_Entity_CIRCLE* dwg_object_to_CIRCLE (Dwg_Object* obj);
EXPORT Dwg_Entity_LINE* dwg_object_to_LINE (Dwg_Object* obj);
EXPORT Dwg_Entity_DIMENSION_ORDINATE* dwg_object_to_DIMENSION_ORDINATE (Dwg_Object* obj);
EXPORT Dwg_Entity_DIMENSION_LINEAR* dwg_object_to_DIMENSION_LINEAR (Dwg_Object* obj);
EXPORT Dwg_Entity_DIMENSION_ALIGNED* dwg_object_to_DIMENSION_ALIGNED (Dwg_Object* obj);
EXPORT Dwg_Entity_DIMENSION_ANG3PT* dwg_object_to_DIMENSION_ANG3PT (Dwg_Object* obj);
EXPORT Dwg_Entity_DIMENSION_ANG2LN* dwg_object_to_DIMENSION_ANG2LN (Dwg_Object* obj);
EXPORT Dwg_Entity_DIMENSION_RADIUS* dwg_object_to_DIMENSION_RADIUS (Dwg_Object* obj);
EXPORT Dwg_Entity_DIMENSION_DIAMETER* dwg_object_to_DIMENSION_DIAMETER (Dwg_Object* obj);
EXPORT Dwg_Entity_POINT* dwg_object_to_POINT (Dwg_Object* obj);
EXPORT Dwg_Entity_POLYLINE_PFACE* dwg_object_to_POLYLINE_PFACE (Dwg_Object* obj);
EXPORT Dwg_Entity_POLYLINE_MESH* dwg_object_to_POLYLINE_MESH (Dwg_Object* obj);
EXPORT Dwg_Entity_SOLID* dwg_object_to_SOLID (Dwg_Object* obj);
EXPORT Dwg_Entity_TRACE* dwg_object_to_TRACE (Dwg_Object* obj);
EXPORT Dwg_Entity_SHAPE* dwg_object_to_SHAPE (Dwg_Object* obj);
EXPORT Dwg_Entity_VIEWPORT* dwg_object_to_VIEWPORT (Dwg_Object* obj);
EXPORT Dwg_Entity_ELLIPSE* dwg_object_to_ELLIPSE (Dwg_Object* obj);
EXPORT Dwg_Entity_SPLINE* dwg_object_to_SPLINE (Dwg_Object* obj);
EXPORT Dwg_Entity_REGION* dwg_object_to_REGION (Dwg_Object* obj);
EXPORT Dwg_Entity_BODY* dwg_object_to_BODY (Dwg_Object* obj);
EXPORT Dwg_Entity_RAY* dwg_object_to_RAY (Dwg_Object* obj);
EXPORT Dwg_Entity_XLINE* dwg_object_to_XLINE (Dwg_Object* obj);
EXPORT Dwg_Entity_OLEFRAME* dwg_object_to_OLEFRAME (Dwg_Object* obj);
EXPORT Dwg_Entity_MTEXT* dwg_object_to_MTEXT (Dwg_Object* obj);
EXPORT Dwg_Entity_LEADER* dwg_object_to_LEADER (Dwg_Object* obj);
EXPORT Dwg_Entity_TOLERANCE* dwg_object_to_TOLERANCE (Dwg_Object* obj);
EXPORT Dwg_Entity_MLINE* dwg_object_to_MLINE (Dwg_Object* obj);
EXPORT Dwg_Entity_OLE2FRAME* dwg_object_to_OLE2FRAME (Dwg_Object* obj);
EXPORT Dwg_Entity_LWPOLYLINE* dwg_object_to_LWPOLYLINE (Dwg_Object* obj);
EXPORT Dwg_Entity_HATCH* dwg_object_to_HATCH (Dwg_Object* obj);
EXPORT Dwg_Entity_IMAGE* dwg_object_to_IMAGE (Dwg_Object* obj);
EXPORT Dwg_Entity_CAMERA* dwg_object_to_CAMERA (Dwg_Object* obj);
EXPORT Dwg_Entity_SURFACE* dwg_object_to_SURFACE (Dwg_Object* obj);
EXPORT Dwg_Entity_GEOPOSITIONMARKER* dwg_object_to_GEOPOSITIONMARKER (Dwg_Object* obj);
EXPORT Dwg_Entity_LIGHT* dwg_object_to_LIGHT (Dwg_Object* obj);
EXPORT Dwg_Entity_MULTILEADER* dwg_object_to_MULTILEADER (Dwg_Object* obj);
EXPORT Dwg_Entity_TABLE* dwg_object_to_TABLE (Dwg_Object* obj);
EXPORT Dwg_Entity_UNDERLAY* dwg_object_to_UNDERLAY (Dwg_Object* obj);
EXPORT Dwg_Entity_WIPEOUT* dwg_object_to_WIPEOUT (Dwg_Object* obj);
EXPORT Dwg_Object_BLOCK_HEADER* dwg_object_to_BLOCK_HEADER (Dwg_Object* obj);
EXPORT Dwg_Object_BLOCK_CONTROL* dwg_object_to_BLOCK_CONTROL (Dwg_Object* obj);
EXPORT Dwg_Object_LAYER* dwg_object_to_LAYER (Dwg_Object* obj);
EXPORT Dwg_Object_XRECORD* dwg_object_to_XRECORD (Dwg_Object* obj);
