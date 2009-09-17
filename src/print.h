/*****************************************************************************/
/*  LibreDWG - Free DWG library                                              */
/*  http://code.google.com/p/libredwg/                                       */
/*                                                                           */
/*    based on LibDWG - Free DWG read-only library                           */
/*    http://sourceforge.net/projects/libdwg                                 */
/*    originally written by Felipe Castro <felipo at users.sourceforge.net>  */
/*                                                                           */
/*  Copyright (C) 2008, 2009 Free Software Foundation, Inc.                  */
/*  Copyright (C) 2009 Felipe Sanches <jucablues@users.sourceforge.net>      */
/*  Copyright (C) 2009 Rodrigo Rodrigues da Silva <pitanga@members.fsf.org>  */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

//I am disabling the print module because our MACROS on decode.c provide us enough debugging messages
#if 0

#include "common.h"
#include "bits.h"
#include "dwg.h"

void
dwg_print(Dwg_Data * dwg);

void
dwg_print_entity(Dwg_Object_Entity * ent);

void
dwg_print_object(Dwg_Object_Object *obj);

void
dwg_print_traktref(Dwg_Object * obj);

void
dwg_print_TEXT(Dwg_Entity_TEXT * ent);

void
dwg_print_ATTRIB(Dwg_Entity_ATTRIB * ent);

void
dwg_print_ATTDEF(Dwg_Entity_ATTDEF * ent);

void
dwg_print_BLOCK(Dwg_Entity_BLOCK * ent);

void
dwg_print_INSERT(Dwg_Entity_INSERT * ent);

void
dwg_print_MINSERT(Dwg_Entity_MINSERT * ent);

void
dwg_print_VERTEX_2D(Dwg_Entity_VERTEX_2D * ent);

void
dwg_print_VERTEX_3D(Dwg_Entity_VERTEX_3D * ent);

void
dwg_print_VERTEX_PFACE_FACE(Dwg_Entity_VERTEX_PFACE_FACE * ent);

void
dwg_print_POLYLINE_2D(Dwg_Entity_POLYLINE_2D * ent);

void
dwg_print_POLYLINE_3D(Dwg_Entity_POLYLINE_3D * ent);

void
dwg_print_ARC(Dwg_Entity_ARC * ent);

void
dwg_print_CIRCLE(Dwg_Entity_CIRCLE * ent);

void
dwg_print_LINE(Dwg_Entity_LINE * ent);

void
dwg_print_POINT(Dwg_Entity_POINT *ent);

void
dwg_print_ELLIPSE(Dwg_Entity_ELLIPSE *ent);

void
dwg_print_RAY(Dwg_Entity_RAY *ent);

void
dwg_print_MTEXT(Dwg_Entity_MTEXT *ent);

void
dwg_print_DICTIONARY(Dwg_Object_DICTIONARY *obj);

void
dwg_print_BLOCK_CONTROL(Dwg_Object_BLOCK_CONTROL *obj);

void
dwg_print_LAYER(Dwg_Object_LAYER *obj);

void
dwg_print_LAYOUT(Dwg_Object_LAYOUT *obj);

#endif
