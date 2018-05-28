/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2009 Free Software Foundation, Inc.                        */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * encode.h: encoding function prototypes, write from dwg to dat buffer.
 * written by Felipe Castro
 * modified by Felipe Corrêa da Silva Sances
 * modified by Rodrigo Rodrigues da Silva
 * modified by Reini Urban
 */

#ifndef ENCODE_H
#define ENCODE_H

#include "bits.h"
#include "dwg.h"

int dwg_encode(Dwg_Data *dwg, Bit_Chain *dat);

int dwg_add_TEXT (Dwg_Data * dwg);
int dwg_add_ATTRIB (Dwg_Data * dwg);
int dwg_add_ATTDEF (Dwg_Data * dwg);
int dwg_add_BLOCK (Dwg_Data * dwg);
int dwg_add_ENDBLK (Dwg_Data * dwg);
int dwg_add_SEQEND (Dwg_Data * dwg);
int dwg_add_INSERT (Dwg_Data * dwg);
int dwg_add_MINSERT (Dwg_Data * dwg);
int dwg_add_VERTEX_2D (Dwg_Data * dwg);
int dwg_add_VERTEX_3D (Dwg_Data * dwg);
int dwg_add_VERTEX_MESH (Dwg_Data * dwg);
int dwg_add_VERTEX_PFACE (Dwg_Data * dwg);
int dwg_add_VERTEX_PFACE_FACE (Dwg_Data * dwg);
int dwg_add_POLYLINE_2D (Dwg_Data * dwg);
int dwg_add_POLYLINE_3D (Dwg_Data * dwg);
int dwg_add_ARC (Dwg_Data * dwg);
int dwg_add_CIRCLE (Dwg_Data * dwg);
int dwg_add_LINE (Dwg_Data * dwg);
int dwg_add_DIMENSION_ORDINATE (Dwg_Data * dwg);
int dwg_add_DIMENSION_LINEAR (Dwg_Data * dwg);
int dwg_add_DIMENSION_ALIGNED (Dwg_Data * dwg);
int dwg_add_DIMENSION_ANG3PT (Dwg_Data * dwg);
int dwg_add_DIMENSION_ANG2LN (Dwg_Data * dwg);
int dwg_add_DIMENSION_RADIUS (Dwg_Data * dwg);
int dwg_add_DIMENSION_DIAMETER (Dwg_Data * dwg);
int dwg_add_POINT (Dwg_Data * dwg);
int dwg_add__3DFACE (Dwg_Data * dwg);
int dwg_add__3DSOLID (Dwg_Data * dwg);
int dwg_add_POLYLINE_PFACE (Dwg_Data * dwg);
int dwg_add_POLYLINE_MESH (Dwg_Data * dwg);
int dwg_add_SOLID (Dwg_Data * dwg);
int dwg_add_TRACE (Dwg_Data * dwg);
int dwg_add_SHAPE (Dwg_Data * dwg);
int dwg_add_VIEWPORT (Dwg_Data * dwg);
int dwg_add_ELLIPSE (Dwg_Data * dwg);
int dwg_add_SPLINE (Dwg_Data * dwg);
int dwg_add_REGION (Dwg_Data * dwg);
int dwg_add_3DSOLID (Dwg_Data * dwg);
int dwg_add_BODY (Dwg_Data * dwg);
int dwg_add_RAY (Dwg_Data * dwg);
int dwg_add_XLINE (Dwg_Data * dwg);
int dwg_add_DICTIONARY (Dwg_Data * dwg);
int dwg_add_OLEFRAME (Dwg_Data * dwg);
int dwg_add_MTEXT (Dwg_Data * dwg);
int dwg_add_LEADER (Dwg_Data * dwg);
int dwg_add_TOLERANCE (Dwg_Data * dwg);
int dwg_add_MLINE (Dwg_Data * dwg);
int dwg_add_BLOCK_CONTROL (Dwg_Data * dwg);
int dwg_add_BLOCK_HEADER (Dwg_Data * dwg);
int dwg_add_LAYER_CONTROL (Dwg_Data * dwg);
int dwg_add_LAYER (Dwg_Data * dwg);
int dwg_add_STYLE_CONTROL (Dwg_Data * dwg);
int dwg_add_STYLE (Dwg_Data * dwg);
int dwg_add_LTYPE_CONTROL (Dwg_Data * dwg);
int dwg_add_LTYPE (Dwg_Data * dwg);
int dwg_add_VIEW_CONTROL (Dwg_Data * dwg);
int dwg_add_VIEW (Dwg_Data * dwg);
int dwg_add_UCS_CONTROL (Dwg_Data * dwg);
int dwg_add_UCS (Dwg_Data * dwg);
int dwg_add_VPORT_CONTROL (Dwg_Data * dwg);
int dwg_add_VPORT (Dwg_Data * dwg);
int dwg_add_APPID_CONTROL (Dwg_Data * dwg);
int dwg_add_APPID (Dwg_Data * dwg);
int dwg_add_DIMSTYLE_CONTROL (Dwg_Data * dwg);
int dwg_add_DIMSTYLE (Dwg_Data * dwg);
int dwg_add_VPORT_ENTITY_CONTROL (Dwg_Data * dwg);
int dwg_add_VPORT_ENTITY_HEADER (Dwg_Data * dwg);
int dwg_add_GROUP (Dwg_Data * dwg);
int dwg_add_MLINESTYLE (Dwg_Data * dwg);
int dwg_add_OLE2FRAME (Dwg_Data * dwg);
int dwg_add_DUMMY (Dwg_Data * dwg);
int dwg_add_LONG_TRANSACTION (Dwg_Data * dwg);
int dwg_add_LWPOLYLINE (Dwg_Data * dwg);
int dwg_add_HATCH (Dwg_Data * dwg);
int dwg_add_XRECORD (Dwg_Data * dwg);
int dwg_add_PLACEHOLDER (Dwg_Data * dwg);
int dwg_add_VBA_PROJECT (Dwg_Data * dwg);
int dwg_add_LAYOUT (Dwg_Data * dwg);
int dwg_add_PROXY_ENTITY (Dwg_Data * dwg);
int dwg_add_PROXY_OBJECT (Dwg_Data * dwg);
int dwg_add_ARCALIGNEDTEXT (Dwg_Data * dwg);
int dwg_add_ASSOC2DCONSTRAINTGROUP (Dwg_Data * dwg);
int dwg_add_ASSOCGEOMDEPENDENCY (Dwg_Data * dwg);
int dwg_add_ASSOCNETWORK (Dwg_Data * dwg);
int dwg_add_CELLSTYLEMAP (Dwg_Data * dwg);
int dwg_add_DBCOLOR (Dwg_Data * dwg);
int dwg_add_DETAILVIEWSTYLE (Dwg_Data * dwg);
int dwg_add_DIMASSOC (Dwg_Data * dwg);
int dwg_add_DICTIONARYVAR (Dwg_Data * dwg);
int dwg_add_DICTIONARYWDLFT (Dwg_Data * dwg);
int dwg_add_EXACXREFPANELOBJECT (Dwg_Data * dwg);
int dwg_add_FIELD (Dwg_Data * dwg);
int dwg_add_FIELDLIST (Dwg_Data * dwg);
int dwg_add_GEODATA (Dwg_Data * dwg);
int dwg_add_IDBUFFER (Dwg_Data * dwg);
int dwg_add_IMAGE (Dwg_Data * dwg);
int dwg_add_IMAGEDEF (Dwg_Data * dwg);
int dwg_add_IMAGEDEF_REACTOR (Dwg_Data * dwg);
int dwg_add_LAYER_INDEX (Dwg_Data * dwg);
int dwg_add_LAYER_FILTER (Dwg_Data * dwg);
int dwg_add_LEADEROBJECTCONTEXTDATA (Dwg_Data * dwg);
int dwg_add_LIGHTLIST (Dwg_Data * dwg);
int dwg_add_MATERIAL (Dwg_Data * dwg);
int dwg_add_MULTILEADER (Dwg_Data * dwg);
int dwg_add_MLEADERSTYLE (Dwg_Data * dwg);
int dwg_add_NPOCOLLECTION (Dwg_Data * dwg);
int dwg_add_PLOTSETTINGS (Dwg_Data * dwg);
int dwg_add_OBJECTCONTEXTDATA (Dwg_Data * dwg);
int dwg_add_RASTERVARIABLES (Dwg_Data * dwg);
int dwg_add_SCALE (Dwg_Data * dwg);
int dwg_add_SECTIONVIEWSTYLE (Dwg_Data * dwg);
int dwg_add_SORTENTSTABLE (Dwg_Data * dwg);
int dwg_add_SPATIAL_FILTER (Dwg_Data * dwg);
int dwg_add_SPATIAL_INDEX (Dwg_Data * dwg);
int dwg_add_TABLE (Dwg_Data * dwg);
int dwg_add_TABLECONTENT (Dwg_Data * dwg);
int dwg_add_TABLEGEOMETRY (Dwg_Data * dwg);
int dwg_add_TABLESTYLE (Dwg_Data * dwg);
int dwg_add_VISUALSTYLE (Dwg_Data * dwg);
int dwg_add_WIPEOUT (Dwg_Data * dwg);
int dwg_add_WIPEOUTVARIABLE (Dwg_Data * dwg);
int dwg_add_UNKNOWN_ENT (Dwg_Data * dwg);
int dwg_add_UNKNOWN_OBJ (Dwg_Data * dwg);

#endif
