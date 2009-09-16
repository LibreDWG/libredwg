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
/*  Copyright (C) 2009 Rodrigo Rodrigues da Silva <rodrigopitanga@gmail.com> */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

#include "print.h"
#include <stdio.h>

/*------------------------------------------------------------------------------
 * Private functions
 */

void
dwg_print_CMC(Dwg_Color color)
{
  printf("Color:\n\tindex: %u\n", color.index);
  //TODO: detect other versions
}

void
dwg_print_entity(Dwg_Object_Entity * ent)
{
  printf("Bitsize: %lu\n", ent->bitsize);
  printf("Handle: %i.%i.%lu\n", ent->object->handle.code,
      ent->object->handle.size, ent->object->handle.value);
  printf("Extended size: %lu B\n", (long unsigned int) ent->extended_size);
  printf("Picture exists?: %s", ent->picture_exists ? "Yes" : "No");
  if (ent->picture_exists)
    printf("\tSize: %lu B\n", ent->picture_size);
  else
    puts("");
  printf("Regime: %i\n", ent->entity_mode);
  printf("Numreactors: %lu\n", ent->num_reactors);
  printf("No links?: %s\n", ent->nolinks ? "Yes" : "No");
  dwg_print_CMC(ent->color);
  printf("Linetype scale: %1.13g\n", ent->linetype_scale);
  printf("Linetype: 0x%02X\n", ent->linetype_flags);
  printf("Plot Style: 0x%02X\n", ent->plotstyle_flags);
  printf("Invisible: 0x%04X\n", ent->invisible);
  printf("LineThickness: %u\n", ent->lineweight);
}

void
dwg_print_object(Dwg_Object_Object *obj)
{
  printf("Bitsize: %lu\n", obj->bitsize);
  printf("Handle: %i.%i.%lu\n", obj->object->handle.code,
      obj->object->handle.size, obj->object->handle.value);
  printf("Extended size: %lu B\n", (long unsigned int) obj->extended_size);
  printf("Numreactors: %lu\n", obj->num_reactors);
}

void
dwg_print_handleref(Dwg_Object * obj)
{
  unsigned int i;
  unsigned int num_handles = obj->parent->num_object_refs;
  Dwg_Object_Ref* ref = obj->parent->object_ref;

  printf("\tHandle references (%u): ", num_handles);
  if (num_handles == 0)
    {
      puts("");
      return;
    }
  for (i = 0; i <= num_handles; i++)
    printf("%i.%i.%li / ", ref[i].handleref.code, ref[i].handleref.size,
        ref[i].handleref.value);
}

/* OBJECTS *******************************************************************/

void
dwg_print_UNUSED(Dwg_Entity_UNUSED * ent)
{
}

void
dwg_print_TEXT(Dwg_Entity_TEXT * ent)
{
/*
  printf("\tData flags: 0x%02x\n", ent->dataflags);

  printf("\tElevation: %1.13g\n", ent->elevation);
  printf("\tInsertion Point: (%1.13g, %1.13g)\n", ent->x0, ent->y0);
  printf("\tAlignment Point: (%1.13g, %1.13g)\n", ent->alignment.x,
      ent->alignment.y);
  printf("\tExtrusion: (%1.13g, %1.13g, %1.13g)\n", ent->extrusion.x,
      ent->extrusion.y, ent->extrusion.z);
  printf("\tThickness: %1.13g\n", ent->thickness);
  printf("\tOblique ang: %1.13g\n", ent->oblique_ang);
  printf("\tRotation ang: %1.13g\n", ent->rotation_ang);
  printf("\tHeight: %1.13g\n", ent->height);
  printf("\tWidth factor: %1.13g\n", ent->width_factor);
  printf("\tText: \"%s\"\n", ent->text);
  printf("\tGeneration: %u\n", ent->generation);
  printf("\tAlignment (horiz.): %u\n", ent->alignment.h);
  printf("\tAlignment (vert.): %u\n", ent->alignment.v);
*/
}

void
dwg_print_ATTRIB(Dwg_Entity_ATTRIB * ent)
{
/*
  printf("\tData flags: 0x%02x\n", ent->dataflags);

  printf("\tElevation: %1.13g\n", ent->elevation);
  printf("\tInsertion Point: (%1.13g, %1.13g)\n", ent->x0, ent->y0);
  printf("\tAlignment Point: (%1.13g, %1.13g)\n", ent->alignment.x,
      ent->alignment.y);
  printf("\tExtrusion: (%1.13g, %1.13g, %1.13g)\n", ent->extrusion.x,
      ent->extrusion.y, ent->extrusion.z);
  printf("\tThickness: %1.13g\n", ent->thickness);
  printf("\tOblique ang: %1.13g\n", ent->oblique_ang);
  printf("\tRotation ang: %1.13g\n", ent->rotation_ang);
  printf("\tHeight: %1.13g\n", ent->height);
  printf("\tWidth factor: %1.13g\n", ent->width_factor);
  printf("\tText: \"%s\"\n", ent->text);
  printf("\tGeneration: %u\n", ent->generation);
  printf("\tAlignment (horiz.): %u\n", ent->alignment.h);
  printf("\tAlignment (vert.): %u\n", ent->alignment.v);
  printf("\tTag: %s\n", ent->tag);
  printf("\tField length: %i (ne uzata)\n", ent->field_length);
  printf("\tFlags: 0x%02x\n", ent->flags);
*/
}

void
dwg_print_ATTDEF(Dwg_Entity_ATTDEF * ent)
{
/*
  printf("\tData flags: 0x%02x\n", ent->dataflags);

  printf("\tElevation: %1.13g\n", ent->elevation);
  printf("\tInsertion Point: (%1.13g, %1.13g)\n", ent->x0, ent->y0);
  printf("\tAlignment Point: (%1.13g, %1.13g)\n", ent->alignment.x,
      ent->alignment.y);
  printf("\tExtrusion: (%1.13g, %1.13g, %1.13g)\n", ent->extrusion.x,
      ent->extrusion.y, ent->extrusion.z);
  printf("\tThickness: %1.13g\n", ent->thickness);
  printf("\tOblique ang: %1.13g\n", ent->oblique_ang);
  printf("\tRotation ang: %1.13g\n", ent->rotation_ang);
  printf("\tHeight: %1.13g\n", ent->height);
  printf("\tWidth factor: %1.13g\n", ent->width_factor);
  printf("\tText: \"%s\"\n", ent->text);
  printf("\tGeneration: %u\n", ent->generation);
  printf("\tAlignment (horiz.): %u\n", ent->alignment.h);
  printf("\tAlignment (vert.): %u\n", ent->alignment.v);
  printf("\tTag: %s\n", ent->tag);
  printf("\tField length: %i\n", ent->field_length);
  printf("\tFlags: 0x%02x\n", ent->flags);
  printf("\tPrompt (prompt): %s\n", ent->prompt);
*/
}

void
dwg_print_BLOCK(Dwg_Entity_BLOCK * ent)
{
  printf("\tName: %s\n", ent->name);
}

void
dwg_print_ENDBLK(Dwg_Entity_ENDBLK * ent)
{
}

void
dwg_print_SEQEND(Dwg_Entity_SEQEND * ent)
{
}

void
dwg_print_INSERT(Dwg_Entity_INSERT * ent)
{
  printf("\tInsertion Point: (%1.13g, %1.13g, %1.13g)\n", ent->ins_pt.x, ent->ins_pt.y,
      ent->ins_pt.z);
  printf("\tScale flag: 0x%02x\n", ent->scale_flag);
  printf("\tScale: (%1.13g, %1.13g, %1.13g)\n", ent->scale.x, ent->scale.y,
      ent->scale.z);
  printf("\tRotation ang: %1.13g\n", ent->rotation_ang);
  printf("\tExtrusion: (%1.13g, %1.13g, %1.13g)\n", ent->extrusion.x,
      ent->extrusion.y, ent->extrusion.z);
  printf("\tHas attribs?: %s\n", ent->has_attribs ? "Yes" : "No");
}

void
dwg_print_MINSERT(Dwg_Entity_MINSERT * ent)
{
  printf("\tInsertion Point: (%1.13g, %1.13g, %1.13g)\n", ent->ins_pt.x, ent->ins_pt.y,
      ent->ins_pt.z);
  printf("\tScale flag: 0x%02x\n", ent->scale_flag);
  printf("\tScale: (%1.13g, %1.13g, %1.13g)\n", ent->scale.x, ent->scale.y,
      ent->scale.z);
  printf("\tRotation ang: %1.13g\n", ent->rotation_ang);
  printf("\tExtrusion: (%1.13g, %1.13g, %1.13g)\n", ent->extrusion.x,
      ent->extrusion.y, ent->extrusion.z);
  printf("\tHas attribs?: %s\n", ent->has_attribs ? "Yes" : "No");
  printf("\tColumns: %02i\tInterspace: %1.13g\n", ent->numcols,
      ent->col_spacing);
  printf("\tLines: %02i\tInterspace: %1.13g\n", ent->numrows, ent->row_spacing);
}

void
dwg_print_VERTEX_2D(Dwg_Entity_VERTEX_2D * ent)
{
  printf("\tFlags: 0x%02x\n", ent->flags);
  printf("\tPoint: (%1.13g, %1.13g, %1.13g)\n", ent->x0, ent->y0, ent->z0);
  printf("\tStart width: %1.13g\n", ent->start_width);
  printf("\tEnd width: %1.13g\n", ent->end_width);
  printf("\tBulge: %1.13g\n", ent->bulge);
  printf("\tTangent dir.: %1.13g\n", ent->tangent_dir);
}

void
dwg_print_VERTEX_3D(Dwg_Entity_VERTEX_3D * ent)
{
  printf("\tFlags: 0x%02x\n", ent->flags);
  printf("\tPoint: (%1.13g, %1.13g, %1.13g)\n", ent->x0, ent->y0, ent->z0);
}

void
dwg_print_VERTEX_MESH(Dwg_Entity_VERTEX_MESH * ent)
{
  printf("\tFlags: 0x%02x\n", ent->flags);
  printf("\tPoint: (%1.13g, %1.13g, %1.13g)\n", ent->x0, ent->y0, ent->z0);
}

void
dwg_print_VERTEX_PFACE(Dwg_Entity_VERTEX_PFACE * ent)
{
  printf("\tFlags: 0x%02x\n", ent->flags);
  printf("\tPoint: (%1.13g, %1.13g, %1.13g)\n", ent->x0, ent->y0, ent->z0);
}

void
dwg_print_VERTEX_PFACE_FACE(Dwg_Entity_VERTEX_PFACE_FACE * ent)
{
  printf("\tVertic-index 1: %i\n", ent->vertind[0]);
  printf("\tVertic-index 2: %i\n", ent->vertind[1]);
  printf("\tVertic-index 3: %i\n", ent->vertind[2]);
  printf("\tVertic-index 4: %i\n", ent->vertind[3]);
}

void
dwg_print_POLYLINE_2D(Dwg_Entity_POLYLINE_2D * ent)
{
  printf("\tFlags: 0x%02x\n", ent->flags);
  printf("\tCurve type: 0x%02x\n", ent->curve_type);
  printf("\tStart width: %1.13g\n", ent->start_width);
  printf("\tEnd width: %1.13g\n", ent->end_width);
  printf("\tThickness: %1.13g\n", ent->thickness);
  printf("\tElevation: %1.13g\n", ent->elevation);
  printf("\tExtrusion: (%1.13g, %1.13g, %1.13g)\n", ent->extrusion.x,
      ent->extrusion.y, ent->extrusion.z);
}

void
dwg_print_POLYLINE_3D(Dwg_Entity_POLYLINE_3D * ent)
{
  printf("\tFlags: 0x%02x / 0x%02x\n", ent->flags_1, ent->flags_2);
}

void
dwg_print_ARC(Dwg_Entity_ARC * ent)
{
  printf("\tCentral point: (%1.13g, %1.13g, %1.13g)\n", ent->x0, ent->y0,
      ent->z0);
  printf("\tRadius: %1.13g\n", ent->radius);
  printf("\tThickness: %1.13g\n", ent->thickness);
  printf("\tExtrusion: (%1.13g, %1.13g, %1.13g)\n", ent->extrusion.x,
      ent->extrusion.y, ent->extrusion.z);
  printf("\tStart angle: %1.13g\n", ent->start_angle);
  printf("\tEnd angle: %1.13g\n", ent->end_angle);
}

void
dwg_print_CIRCLE(Dwg_Entity_CIRCLE * ent)
{
  printf("\tCentral point: (%1.13g, %1.13g, %1.13g)\n", ent->x0, ent->y0,
      ent->z0);
  printf("\tRadius: %1.13g\n", ent->radius);
  printf("\tThickness: %1.13g\n", ent->thickness);
  printf("\tExtrusion: (%1.13g, %1.13g, %1.13g)\n", ent->extrusion.x,
      ent->extrusion.y, ent->extrusion.z);
}

void
dwg_print_LINE(Dwg_Entity_LINE * ent)
{
  printf("\tz's are zero?: %s\n", ent->Zs_are_zero ? "Yes" : "No");
  printf("\t1st point: (%1.13g, %1.13g, %1.13g)\n", ent->x0, ent->y0,
      ent->Zs_are_zero ? 0 : ent->z0);
  printf("\t2nd point: (%1.13g, %1.13g, %1.13g)\n", ent->x1, ent->y1,
      ent->Zs_are_zero ? 0 : ent->z1);
  printf("\tThickness: %1.13g\n", ent->thickness);
  printf("\tExtrusion: (%1.13g, %1.13g, %1.13g)\n", ent->extrusion.x,
      ent->extrusion.y, ent->extrusion.z);
}

void
dwg_print_DIMENSION_ORDINATE(Dwg_Entity_DIMENSION_ORDINATE * ent)
{
  printf("\tExtrusion: (%1.13g, %1.13g, %1.13g)\n", ent->extrusion.x,
      ent->extrusion.y, ent->extrusion.z);
  printf("\tText midpt: (%1.13g, %1.13g)\n", ent->x0, ent->y0);

  if (ent->elevation.ecs_11 != ent->elevation.ecs_12)
    {
      fprintf(
          stderr,
          "encode_DIMENSION_ORDINARY: Maybe there is something wrong here. Elevation values should be all the same.\n");
    }
  printf("\tElevation: %1.13g\n", ent->elevation.ecs_11);

  //spec: flag bit 6 indicates ORDINATE dimension
  printf("\tFlags1: 0x%02x\n", ent->flags_1);
  printf("\tUser text: %s\n", ent->user_text);
  printf("\tText rot: %1.13g\n", ent->text_rot);
  printf("\tHoriz dir: %1.13g\n", ent->horiz_dir);
  printf("\tIns X-scale: %1.13g\n", ent->ins_scale.x);
  printf("\tIns Y-scale: %1.13g\n", ent->ins_scale.y);
  printf("\tIns Z-scale: %1.13g\n", ent->ins_scale.z);
  printf("\tIns rotation: %1.13g\n", ent->ins_rotation);

  //if(dat->version >= R_2000){
  printf("\tAttachment point: %u", ent->attachment_point);
  printf("\tLspace style: %u", ent->lspace_style);
  printf("\tLspace factor: %1.13g\n", ent->lspace_factor);
  printf("\tAct measurement: %1.13g\n", ent->act_measurement);
  //}

  /*
   if(dat->version >= R_2007){
   printf("\tUnknown: %u", (int)ent->unknown);
   printf("\tFlip arrow1: %u", (int)ent->flip_arrow1);
   printf("\tFlip arrow2: %u", (int)ent->flip_arrow2);
   }
   */

  printf("\t12 x: %1.13g\n", ent->_12_pt.x);
  printf("\t12 y: %1.13g\n", ent->_12_pt.y);
  printf("\t10 x: %1.13g\n", ent->_10_pt.x);
  printf("\t10 y: %1.13g\n", ent->_10_pt.y);
  printf("\t10 z: %1.13g\n", ent->_10_pt.z);
  printf("\t13 x: %1.13g\n", ent->_13_pt.x);
  printf("\t13 y: %1.13g\n", ent->_13_pt.y);
  printf("\t13 z: %1.13g\n", ent->_13_pt.z);
  printf("\t14 x: %1.13g\n", ent->_14_pt.x);
  printf("\t14 y: %1.13g\n", ent->_14_pt.y);
  printf("\t14 z: %1.13g\n", ent->_14_pt.z);

  printf("\tFlags2: 0x%02x\n", ent->flags_2);
}

void
dwg_print_DIMENSION_LINEAR(Dwg_Entity_DIMENSION_LINEAR * ent)
{
  printf("\tExtrusion: (%1.13g, %1.13g, %1.13g)\n", ent->extrusion.x,
      ent->extrusion.y, ent->extrusion.z);
  printf("\tText midpt: (%1.13g, %1.13g)\n", ent->x0, ent->y0);

  if (ent->elevation.ecs_11 != ent->elevation.ecs_12)
    {
      fprintf(
          stderr,
          "encode_DIMENSION_ORDINARY: Maybe there is something wrong here. Elevation values should be all the same.\n");
    }
  printf("\tElevation: %1.13g\n", ent->elevation.ecs_11);

  //spec: flag bit 6 indicates ORDINATE dimension
  printf("\tFlags: 0x%02x\n", ent->flags);
  printf("\tUser text: %s\n", ent->user_text);
  printf("\tText rot: %1.13g\n", ent->text_rot);
  printf("\tHoriz dir: %1.13g\n", ent->horiz_dir);
  printf("\tIns X-scale: %1.13g\n", ent->ins_scale.x);
  printf("\tIns Y-scale: %1.13g\n", ent->ins_scale.y);
  printf("\tIns Z-scale: %1.13g\n", ent->ins_scale.z);
  printf("\tIns rotation: %1.13g\n", ent->ins_rotation);

  //if(dat->version >= R_2000){
  printf("\tAttachment point: %u", ent->attachment_point);
  printf("\tLspace style: %u", ent->lspace_style);
  printf("\tLspace factor: %1.13g\n", ent->lspace_factor);
  printf("\tAct measurement: %1.13g\n", ent->act_measurement);
  //}

  /*
   if(dat->version >= R_2007){
   printf("\tUnknown: %u", (int)ent->unknown);
   printf("\tFlip arrow1: %u", (int)ent->flip_arrow1);
   printf("\tFlip arrow2: %u", (int)ent->flip_arrow2);
   }
   */

  printf("\t12 x: %1.13g\n", ent->_12_pt.x);
  printf("\t12 y: %1.13g\n", ent->_12_pt.y);
  printf("\t13 x: %1.13g\n", ent->_13_pt.x);
  printf("\t13 y: %1.13g\n", ent->_13_pt.y);
  printf("\t13 z: %1.13g\n", ent->_13_pt.z);
  printf("\t14 x: %1.13g\n", ent->_14_pt.x);
  printf("\t14 y: %1.13g\n", ent->_14_pt.y);
  printf("\t14 z: %1.13g\n", ent->_14_pt.z);
  printf("\t10 x: %1.13g\n", ent->_10_pt.x);
  printf("\t10 y: %1.13g\n", ent->_10_pt.y);
  printf("\t10 z: %1.13g\n", ent->_10_pt.z);

  printf("\tExtension line rotation ln rot: %1.13g\n", ent->ext_line_rot);
  printf("\tLinear dimension rotation: %1.13g\n", ent->dim_rot);
}

void
dwg_print_DIMENSION_ALIGNED(Dwg_Entity_DIMENSION_ALIGNED * ent)
{
  printf("\tExtrusion: (%1.13g, %1.13g, %1.13g)\n", ent->extrusion.x,
      ent->extrusion.y, ent->extrusion.z);
  printf("\tText midpt: (%1.13g, %1.13g)\n", ent->x0, ent->y0);

  if (ent->elevation.ecs_11 != ent->elevation.ecs_12)
    {
      fprintf(
          stderr,
          "encode_DIMENSION_ORDINARY: Maybe there is something wrong here. Elevation values should be all the same.\n");
    }
  printf("\tElevation: %1.13g\n", ent->elevation.ecs_11);

  //spec: flag bit 6 indicates ORDINATE dimension
  printf("\tFlags: 0x%02x\n", ent->flags);
  printf("\tUser text: %s\n", ent->user_text);
  printf("\tText rot: %1.13g\n", ent->text_rot);
  printf("\tHoriz dir: %1.13g\n", ent->horiz_dir);
  printf("\tIns X-scale: %1.13g\n", ent->ins_scale.x);
  printf("\tIns Y-scale: %1.13g\n", ent->ins_scale.y);
  printf("\tIns Z-scale: %1.13g\n", ent->ins_scale.z);
  printf("\tIns rotation: %1.13g\n", ent->ins_rotation);

  //if(dat->version >= R_2000){
  printf("\tAttachment point: %u", ent->attachment_point);
  printf("\tLspace style: %u", ent->lspace_style);
  printf("\tLspace factor: %1.13g\n", ent->lspace_factor);
  printf("\tAct measurement: %1.13g\n", ent->act_measurement);
  //}

  /*
   if(dat->version >= R_2007){
   printf("\tUnknown: %u", (int)ent->unknown);
   printf("\tFlip arrow1: %u", (int)ent->flip_arrow1);
   printf("\tFlip arrow2: %u", (int)ent->flip_arrow2);
   }
   */

  printf("\t12 x: %1.13g\n", ent->_12_pt.x);
  printf("\t12 y: %1.13g\n", ent->_12_pt.y);
  printf("\t13 x: %1.13g\n", ent->_13_pt.x);
  printf("\t13 y: %1.13g\n", ent->_13_pt.y);
  printf("\t13 z: %1.13g\n", ent->_13_pt.z);
  printf("\t14 x: %1.13g\n", ent->_14_pt.x);
  printf("\t14 y: %1.13g\n", ent->_14_pt.y);
  printf("\t14 z: %1.13g\n", ent->_14_pt.z);
  printf("\t10 x: %1.13g\n", ent->_10_pt.x);
  printf("\t10 y: %1.13g\n", ent->_10_pt.y);
  printf("\t10 z: %1.13g\n", ent->_10_pt.z);

  printf("\tExtension line rotation ln rot: %1.13g\n", ent->ext_line_rot);
}

void
dwg_print_DIMENSION_ANG3PT(Dwg_Entity_DIMENSION_ANG3PT * ent)
{
  printf("\tExtrusion: (%1.13g, %1.13g, %1.13g)\n", ent->extrusion.x,
      ent->extrusion.y, ent->extrusion.z);
  printf("\tText midpt: (%1.13g, %1.13g)\n", ent->x0, ent->y0);

  if (ent->elevation.ecs_11 != ent->elevation.ecs_12)
    {
      fprintf(
          stderr,
          "encode_DIMENSION_ORDINARY: Maybe there is something wrong here. Elevation values should be all the same.\n");
    }
  printf("\tElevation: %1.13g\n", ent->elevation.ecs_11);

  //spec: flag bit 6 indicates ORDINATE dimension
  printf("\tFlags: 0x%02x\n", ent->flags);
  printf("\tUser text: %s\n", ent->user_text);
  printf("\tText rot: %1.13g\n", ent->text_rot);
  printf("\tHoriz dir: %1.13g\n", ent->horiz_dir);
  printf("\tIns X-scale: %1.13g\n", ent->ins_scale.x);
  printf("\tIns Y-scale: %1.13g\n", ent->ins_scale.y);
  printf("\tIns Z-scale: %1.13g\n", ent->ins_scale.z);
  printf("\tIns rotation: %1.13g\n", ent->ins_rotation);

  //if(dat->version >= R_2000){
  printf("\tAttachment point: %u", ent->attachment_point);
  printf("\tLspace style: %u", ent->lspace_style);
  printf("\tLspace factor: %1.13g\n", ent->lspace_factor);
  printf("\tAct measurement: %1.13g\n", ent->act_measurement);
  //}

  /*
   if(dat->version >= R_2007){
   printf("\tUnknown: %u", (int)ent->unknown);
   printf("\tFlip arrow1: %u", (int)ent->flip_arrow1);
   printf("\tFlip arrow2: %u", (int)ent->flip_arrow2);
   }
   */

  printf("\t12 x: %1.13g\n", ent->_12_pt.x);
  printf("\t12 y: %1.13g\n", ent->_12_pt.y);
  printf("\t10 x: %1.13g\n", ent->_10_pt.x);
  printf("\t10 y: %1.13g\n", ent->_10_pt.y);
  printf("\t10 z: %1.13g\n", ent->_10_pt.z);
  printf("\t13 x: %1.13g\n", ent->_13_pt.x);
  printf("\t13 y: %1.13g\n", ent->_13_pt.y);
  printf("\t13 z: %1.13g\n", ent->_13_pt.z);
  printf("\t14 x: %1.13g\n", ent->_14_pt.x);
  printf("\t14 y: %1.13g\n", ent->_14_pt.y);
  printf("\t14 z: %1.13g\n", ent->_14_pt.z);
  printf("\t15 x: %1.13g\n", ent->_15_pt.x);
  printf("\t15 y: %1.13g\n", ent->_15_pt.y);
  printf("\t15 z: %1.13g\n", ent->_15_pt.z);
}

void
dwg_print_DIMENSION_ANG2LN(Dwg_Entity_DIMENSION_ANG2LN * ent)
{
  printf("\tExtrusion: (%1.13g, %1.13g, %1.13g)\n", ent->extrusion.x,
      ent->extrusion.y, ent->extrusion.z);
  printf("\tText midpt: (%1.13g, %1.13g)\n", ent->x0, ent->y0);

  if (ent->elevation.ecs_11 != ent->elevation.ecs_12)
    {
      fprintf(
          stderr,
          "encode_DIMENSION_ORDINARY: Maybe there is something wrong here. Elevation values should be all the same.\n");
    }
  printf("\tElevation: %1.13g\n", ent->elevation.ecs_11);

  //spec: flag bit 6 indicates ORDINATE dimension
  printf("\tFlags: 0x%02x\n", ent->flags);
  printf("\tUser text: %s\n", ent->user_text);
  printf("\tText rot: %1.13g\n", ent->text_rot);
  printf("\tHoriz dir: %1.13g\n", ent->horiz_dir);
  printf("\tIns X-scale: %1.13g\n", ent->ins_scale.x);
  printf("\tIns Y-scale: %1.13g\n", ent->ins_scale.y);
  printf("\tIns Z-scale: %1.13g\n", ent->ins_scale.z);
  printf("\tIns rotation: %1.13g\n", ent->ins_rotation);

  //if(dat->version >= R_2000){
  printf("\tAttachment point: %u", ent->attachment_point);
  printf("\tLspace style: %u", ent->lspace_style);
  printf("\tLspace factor: %1.13g\n", ent->lspace_factor);
  printf("\tAct measurement: %1.13g\n", ent->act_measurement);
  //}

  /*
   if(dat->version >= R_2007){
   printf("\tUnknown: %u", (int)ent->unknown);
   printf("\tFlip arrow1: %u", (int)ent->flip_arrow1);
   printf("\tFlip arrow2: %u", (int)ent->flip_arrow2);
   }
   */

  printf("\t12 x: %1.13g\n", ent->_12_pt.x);
  printf("\t12 y: %1.13g\n", ent->_12_pt.y);
  printf("\t16 x: %1.13g\n", ent->_16_pt.x);
  printf("\t16 y: %1.13g\n", ent->_16_pt.y);
  printf("\t13 x: %1.13g\n", ent->_13_pt.x);
  printf("\t13 y: %1.13g\n", ent->_13_pt.y);
  printf("\t13 z: %1.13g\n", ent->_13_pt.z);
  printf("\t14 x: %1.13g\n", ent->_14_pt.x);
  printf("\t14 y: %1.13g\n", ent->_14_pt.y);
  printf("\t14 z: %1.13g\n", ent->_14_pt.z);
  printf("\t15 x: %1.13g\n", ent->_15_pt.x);
  printf("\t15 y: %1.13g\n", ent->_15_pt.y);
  printf("\t15 z: %1.13g\n", ent->_15_pt.z);
  printf("\t10 x: %1.13g\n", ent->_10_pt.x);
  printf("\t10 y: %1.13g\n", ent->_10_pt.y);
  printf("\t10 z: %1.13g\n", ent->_10_pt.z);
}

void
dwg_print_DIMENSION_RADIUS(Dwg_Entity_DIMENSION_RADIUS * ent)
{
  printf("\tExtrusion: (%1.13g, %1.13g, %1.13g)\n", ent->extrusion.x,
      ent->extrusion.y, ent->extrusion.z);
  printf("\tText midpt: (%1.13g, %1.13g)\n", ent->x0, ent->y0);

  if (ent->elevation.ecs_11 != ent->elevation.ecs_12)
    {
      fprintf(
          stderr,
          "encode_DIMENSION_ORDINARY: Maybe there is something wrong here. Elevation values should be all the same.\n");
    }
  printf("\tElevation: %1.13g\n", ent->elevation.ecs_11);

  printf("\tFlags: 0x%02x\n", ent->flags);
  printf("\tUser text: %s\n", ent->user_text);
  printf("\tText rot: %1.13g\n", ent->text_rot);
  printf("\tHoriz dir: %1.13g\n", ent->horiz_dir);
  printf("\tIns X-scale: %1.13g\n", ent->ins_scale.x);
  printf("\tIns Y-scale: %1.13g\n", ent->ins_scale.y);
  printf("\tIns Z-scale: %1.13g\n", ent->ins_scale.z);
  printf("\tIns rotation: %1.13g\n", ent->ins_rotation);

  //if(dat->version >= R_2000){
  printf("\tAttachment point: %u", ent->attachment_point);
  printf("\tLspace style: %u", ent->lspace_style);
  printf("\tLspace factor: %1.13g\n", ent->lspace_factor);
  printf("\tAct measurement: %1.13g\n", ent->act_measurement);
  //}

  /*
   if(dat->version >= R_2007){
   printf("\tUnknown: %u", (int)ent->unknown);
   printf("\tFlip arrow1: %u", (int)ent->flip_arrow1);
   printf("\tFlip arrow2: %u", (int)ent->flip_arrow2);
   }
   */

  printf("\t12 x: %1.13g\n", ent->_12_pt.x);
  printf("\t12 y: %1.13g\n", ent->_12_pt.y);
  printf("\t10 x: %1.13g\n", ent->_10_pt.x);
  printf("\t10 y: %1.13g\n", ent->_10_pt.y);
  printf("\t10 z: %1.13g\n", ent->_10_pt.z);
  printf("\t15 x: %1.13g\n", ent->_15_pt.x);
  printf("\t15 y: %1.13g\n", ent->_15_pt.y);
  printf("\t15 z: %1.13g\n", ent->_15_pt.z);

  printf("\tLeader length: %1.13g\n", ent->leader_len);
}

void
dwg_print_DIMENSION_DIAMETER(Dwg_Entity_DIMENSION_DIAMETER * ent)
{
  printf("\tExtrusion: (%1.13g, %1.13g, %1.13g)\n", ent->extrusion.x,
      ent->extrusion.y, ent->extrusion.z);
  printf("\tText midpt: (%1.13g, %1.13g)\n", ent->x0, ent->y0);

  if (ent->elevation.ecs_11 != ent->elevation.ecs_12)
    {
      fprintf(
          stderr,
          "encode_DIMENSION_ORDINARY: Maybe there is something wrong here. Elevation values should be all the same.\n");
    }
  printf("\tElevation: %1.13g\n", ent->elevation.ecs_11);

  printf("\tFlags: 0x%02x\n", ent->flags);
  printf("\tUser text: %s\n", ent->user_text);
  printf("\tText rot: %1.13g\n", ent->text_rot);
  printf("\tHoriz dir: %1.13g\n", ent->horiz_dir);
  printf("\tIns X-scale: %1.13g\n", ent->ins_scale.x);
  printf("\tIns Y-scale: %1.13g\n", ent->ins_scale.y);
  printf("\tIns Z-scale: %1.13g\n", ent->ins_scale.z);
  printf("\tIns rotation: %1.13g\n", ent->ins_rotation);

  //if(dat->version >= R_2000){
  printf("\tAttachment point: %u", ent->attachment_point);
  printf("\tLspace style: %u", ent->lspace_style);
  printf("\tLspace factor: %1.13g\n", ent->lspace_factor);
  printf("\tAct measurement: %1.13g\n", ent->act_measurement);
  //}

  /*
   if(dat->version >= R_2007){
   printf("\tUnknown: %u", (int)ent->unknown);
   printf("\tFlip arrow1: %u", (int)ent->flip_arrow1);
   printf("\tFlip arrow2: %u", (int)ent->flip_arrow2);
   }
   */

  printf("\t12 x: %1.13g\n", ent->_12_pt.x);
  printf("\t12 y: %1.13g\n", ent->_12_pt.y);
  printf("\t15 x: %1.13g\n", ent->_15_pt.x);
  printf("\t15 y: %1.13g\n", ent->_15_pt.y);
  printf("\t15 z: %1.13g\n", ent->_15_pt.z);
  printf("\t10 x: %1.13g\n", ent->_10_pt.x);
  printf("\t10 y: %1.13g\n", ent->_10_pt.y);
  printf("\t10 z: %1.13g\n", ent->_10_pt.z);

  printf("\tLeader length: %1.13g\n", ent->leader_len);
}

void
dwg_print_POINT(Dwg_Entity_POINT *ent)
{
  printf("\tPoint: (%1.13g, %1.13g, %1.13g)\n", ent->x, ent->y, ent->z);
  printf("\tThickness: %1.13g\n", ent->thickness);
  printf("\tExtrusion: (%1.13g, %1.13g, %1.13g)\n", ent->extrusion.x,
      ent->extrusion.y, ent->extrusion.z);
  printf("\tX-angle: %1.13g\n", ent->x_ang);
}

void
dwg_print__3DFACE(Dwg_Entity__3DFACE * ent)
{
  //TODO: implement me!
  puts("print not implemented!");
}

void
dwg_print_POLYLINE_PFACE(Dwg_Entity_POLYLINE_PFACE * ent)
{
  //TODO: implement me!
  puts("print not implemented!");
}

void
dwg_print_POLYLINE_MESH(Dwg_Entity_POLYLINE_MESH * ent)
{
  //TODO: implement me!
  puts("print not implemented!");
}

void
dwg_print_SOLID(Dwg_Entity_SOLID * ent)
{
  //TODO: implement me!
  puts("print not implemented!");
}

void
dwg_print_TRACE(Dwg_Entity_TRACE * ent)
{
  //TODO: implement me!
  puts("print not implemented!");
}

void
dwg_print_SHAPE(Dwg_Entity_SHAPE * ent)
{
  //TODO: implement me!
  puts("print not implemented!");
}

void
dwg_print_VIEWPORT(Dwg_Entity_VIEWPORT * ent)
{
  //TODO: implement me!
  puts("print not implemented!");
}

void
dwg_print_ELLIPSE(Dwg_Entity_ELLIPSE *ent)
{
  printf("\tCentral point: (%1.13g, %1.13g, %1.13g)\n", ent->x0, ent->y0,
      ent->z0);
  printf("\tSemi-major axis: (%1.13g, %1.13g, %1.13g)\n", ent->x1, ent->y1,
      ent->z1);
  printf("\tExtrusion: (%1.13g, %1.13g, %1.13g)\n", ent->extrusion.x,
      ent->extrusion.y, ent->extrusion.z);
  printf("\tRadius-ratio: %1.13g\n", ent->axis_ratio);
  printf("\tStart angle: %1.13g\n", ent->start_angle);
  printf("\tEnd angle: %1.13g\n", ent->end_angle);
}

void
dwg_print_SPLINE(Dwg_Entity_SPLINE * ent)
{
  //TODO: implement me!
  puts("print not implemented!");
}

void
dwg_print_REGION(Dwg_Entity_REGION * ent)
{
  //TODO: implement me!
  puts("print not implemented!");
}

void
dwg_print_3DSOLID(Dwg_Entity_3DSOLID * ent)
{
  //TODO: implement me!
  puts("print not implemented!");
}

void
dwg_print_BODY(Dwg_Entity_BODY * ent)
{
  //TODO: implement me!
  puts("print not implemented!");
}

void
dwg_print_RAY(Dwg_Entity_RAY *ent)
{
  printf("\tPoint: (%1.13g, %1.13g, %1.13g)\n", ent->x0, ent->y0, ent->z0);
  printf("\tVector: (%1.13g, %1.13g, %1.13g)\n", ent->x1, ent->y1, ent->z1);
}

void
dwg_print_XLINE(Dwg_Entity_XLINE * ent)
{
  printf("\tPoint: (%1.13g, %1.13g, %1.13g)\n", ent->x0, ent->y0, ent->z0);
  printf("\tVector: (%1.13g, %1.13g, %1.13g)\n", ent->x1, ent->y1, ent->z1);
}

void
dwg_print_DICTIONARY(Dwg_Object_DICTIONARY *obj)
{
  int i;

  printf("\tNumitems: %lu\n", (long unsigned int) obj->numitems);
  printf("\tCloning flag: 0x%X\n", obj->cloning);
  printf("\tHard Owner flag: 0x%X\n", obj->hard_owner);
  printf("\tDictionary items:");
  for (i = 0; i < obj->numitems; i++)
    printf(" %s,", obj->text[i]);
  printf("\n");
}

void
dwg_print_MTEXT(Dwg_Entity_MTEXT *ent)
{
  printf("\tInsertion pt3: (%1.13g, %1.13g, %1.13g)\n", ent->x0, ent->y0,
      ent->z0);
  printf("\tExtrusion: (%1.13g, %1.13g, %1.13g)\n", ent->extrusion.x,
      ent->extrusion.y, ent->extrusion.z);
  printf("\tX-axis dir: (%1.13g, %1.13g, %1.13g)\n", ent->x1, ent->y1, ent->z1);

  //TODO: a way to figure out DWG version
  /*	if (version >= R_2007){
   printf ("\tRect height: %1.13g\n", ent->rect_height);
   }
   */
  printf("\tRect width: %1.13g\n", ent->rect_width);
  printf("\tText height: %1.13g\n", ent->text_height);
  printf("\tAttachment: 0x%0x\n", ent->attachment);
  printf("\tDrawing dir: 0x%0x\n", ent->drawing_dir);
  printf("\tText: %s\n", ent->text);
  //	if (version >= R_2000){
  printf("\tLinespacing Style: 0x%0x\n", ent->linespace_style);
  printf("\tLinespacing Factor: %1.13g\n", (double) ent->linespace_factor);
  printf("\tUnknown bit: %s\n", ent->unknown_bit ? "ON" : "OFF");
  //  }
  //	if (version >= R_2000){
  //      printf ("\tUnknown long: %f\n", ent->unknown_long);
  //  }
}

void
dwg_print_LEADER(Dwg_Entity_LEADER * ent)
{
  //TODO: implement me!
  puts("print not implemented!");
}

void
dwg_print_TOLERANCE(Dwg_Entity_TOLERANCE * ent)
{
  //TODO: implement me!
  puts("print not implemented!");
}

void
dwg_print_MLINE(Dwg_Entity_MLINE * ent)
{
  //TODO: implement me!
  puts("print not implemented!");
}

void
dwg_print_BLOCK_CONTROL(Dwg_Object_BLOCK_CONTROL *obj)
{
  printf("\tSize: %u\n", obj->size);
}

void
dwg_print_LAYER(Dwg_Object_LAYER *obj)
{
  printf("\tName: %s\n", obj->name);
  printf("\tIs 64?: %s\n", obj->bit64 ? "Yes" : "No");
  printf("\tXref index: %u\n", obj->xrefi);
  printf("\tXref dependant?: %s\n", obj->xrefdep ? "Yes" : "No");
  printf("\tValues: 0x%0x\n", obj->values);
  printf("\tColour: %u\n", obj->colour);
}

void
dwg_print_LAYOUT(Dwg_Object_LAYOUT *obj)
{
/*
  printf("\tAgord-name: %s\n", obj->page.agordo);
  printf("\tPrintilo: %s\n", obj->page.printilo);
  printf("\tPrint-flags: 0x%0x\n", obj->page.flags);
  printf(
      "\tMarĝenoj. maldekstre: %1.13g; malsupre: %1.13g; dekstre: %1.13g; supre: %1.13g\n",
      obj->page.maldekstre, obj->page.malsupre, obj->page.dekstre,
      obj->page.supre);
  printf("\tLargxeco: %1.13g\n", obj->page.width);
  printf("\tHeight: %1.13g\n", obj->page.height);
  printf("\tSize: %s\n", obj->page.size);
  printf("\tDeŝovo: (%1.13g, %1.13g)\n", obj->page.dx, obj->page.dy);
  printf("\tUnuoj: %u\n", obj->page.unuoj);
  printf("\tRotacio: %u\n", obj->page.rotacio);
  printf("\tType: %u\n", obj->page.type);
  printf("\tPrint-limoj: (%1.13g, %1.13g) / (%1.13g, %1.13g)\n",
      obj->page.x_min, obj->page.y_min, obj->page.x_max, obj->page.y_max);
  printf("\tPaĝ-name: %s\n", obj->page.name);
  printf("\tSkal-proporcio: %u:%u\n", (unsigned int) obj->page.scale.A,
      (unsigned int) obj->page.scale.B);
  printf("\tStilfolio: %s\n", obj->page.stilfolio);
  printf("\tSkal-type: %u\n", obj->page.scale.type);
  printf("\tSkal-factor: %u\n", (unsigned int) obj->page.scale.factor);
  printf("\tPaĝ-origino: (%1.13g, %1.13g)\n", obj->page.x0, obj->page.y0);
  puts("");
  printf("\tAranĝ-name: %s\n", obj->name);
  printf("\tOrdo: %u\n", obj->ordo);
  printf("\tFlags: 0x%0x\n", obj->flags);
  printf("\tUCS-origino: (%1.13g, %1.13g, %1.13g)\n", obj->x0, obj->y0, obj->z0);
  printf("\tAranĝ-limoj: (%1.13g, %1.13g) / (%1.13g, %1.13g)\n", obj->x_min,
      obj->y_min, obj->x_max, obj->y_max);
  printf("\tEnmeto: (%1.13g, %1.13g, %1.13g)\n", obj->enmeto.x0,
      obj->enmeto.y0, obj->enmeto.z0);
  printf("\tUCS-axis_X: (%1.13g, %1.13g, %1.13g)\n", obj->axis_X.x0,
      obj->axis_X.y0, obj->axis_X.z0);
  printf("\tUCS-axis_Y: (%1.13g, %1.13g, %1.13g)\n", obj->axis_Y.x0,
      obj->axis_Y.y0, obj->axis_Y.z0);
  printf("\tLevigxo: %1.13g\n", obj->elevation);
  printf("\tRigard-type: %u\n", obj->rigardtype);
  printf("\tMinimumo: (%1.13g, %1.13g, %1.13g)\n", obj->limo.x_min,
      obj->limo.y_min, obj->limo.z_min);
  printf("\tMaksimumo: (%1.13g, %1.13g, %1.13g)\n", obj->limo.x_max,
      obj->limo.y_max, obj->limo.z_max);
*/
}

/*------------------------------------------------------------------------------
 * Special public function for printing values of an object
 */
void
dwg_print(Dwg_Structure *dwg_struct)
{
  unsigned char sig;
  unsigned int i, j;
  const char *dwg_obtype[81] =
    { "UNUSED", "TEXT", "ATTRIB", "ATTDEF", "BLOCK", "ENDBLK", "SEQEND",
        "INSERT", "MINSERT", "NULL_09", "VERTEX_2D", "VERTEX_3D",
        "VERTEX_MESH", "VERTEX_PFACE", "VERTEX_PFACE_FACE", "POLYLINE_2D",
        "POLYLINE_3D", "ARC", "CIRCLE", "LINE", "DIMENSION_ORDINATE",
        "DIMENSION_LINEAR", "DIMENSION_ALIGNED", "DIMENSION_ANG3PT",
        "DIMENSION_ANG2LN", "DIMENSION_RADIUS", "DIMENSION_DIAMETER", "POINT",
        "3DFACE", "POLYLINE_PFACE", "POLYLINE_MESH", "SOLID", "TRACE", "SHAPE",
        "VIEWPORT", "ELLIPSE", "SPLINE", "REGION", "3DSOLID", "BODY", "RAY",
        "XLINE", "DICTIONARY", "NULL_43", "MTEXT", "LEADER", "TOLERANCE",
        "MLINE", "BLOCK_CONTROL", "BLOCK_HEADER", "LAYER_CONTROL", "LAYER",
        "STYLE_CONTROL", "STYLE", "NULL_54", "NULL_55", "LTYPE_CONTROL",
        "LTYPE", "NULL_58", "NULL_59", "VIEW_CONTROL", "VIEW", "UCS_CONTROL",
        "UCS", "VPORT_CONTROL", "VPORT", "APPID_CONTROL", "APPID",
        "DIMSTYLE_CONTROL", "DIMSTYLE", "VP_ENT_HDR_CTRL", "VP_ENT_HDR",
        "GROUP", "MLINESTYLE", "DICTIONARYVAR", "DICTIONARYWDLFT", "HATCH",
        "IDBUFFER", "IMAGE", "IMAGEDEF", "IMAGEDEFREACTOR" };

  puts("**************************************************");
  puts("Section HEADER");
  puts("**************************************************");
  printf("Version: %s\n", version_codes[dwg_struct->header.version]);
  printf("Codepage: %u\n", dwg_struct->header.codepage);
  for (i = 0; i < dwg_struct->header.num_sections; i++)
    printf("Section %i\t Address: %7lu\t Size: %7lu B\n",
        dwg_struct->header.section[i].number,
        dwg_struct->header.section[i].address,
        dwg_struct->header.section[i].size);
  puts("");

  if (dwg_struct->header.num_sections == 6)
    {
      puts("**************************************************");
      puts("Section UNKNOWN 1");
      puts("**************************************************");
      printf("Size: %lu B\n", dwg_struct->unknown1.size);
      bit_print((Bit_Chain *) &dwg_struct->unknown1, dwg_struct->unknown1.size);
      puts("");
    }

  puts("**************************************************");
  puts("Section PICTURE");
  puts("**************************************************");
  printf("Size: %lu B\n", dwg_struct->picture.size);
  //bit_print ((Bit_Chain *) &dwg_struct->picture, dwg_struct->picture.size);
  puts("");

  puts("**************************************************");
  puts("Section VARIABLES");
  puts("**************************************************");
  for (i = 0; i < DWG_NUM_VARIABLES; i++)
    {
      printf("[%03i] - ", i + 1);
      if (i == 221 && dwg_struct->var[220].dubitoko != 3)
        {
          puts("(Non-Existant)");
          continue;
        }
      switch (dwg_var_map(dwg_struct->header.version, i))
        {
      case DWG_DT_B:
        printf("B: %u", dwg_struct->var[i].bitoko);
        break;
      case DWG_DT_BS:
        printf("BS: %u", dwg_struct->var[i].dubitoko);
        break;
      case DWG_DT_BL:
        printf("BL: %lu", dwg_struct->var[i].kvarbitoko);
        break;
      case DWG_DT_BD:
        printf("BD: %lg", dwg_struct->var[i].duglitajxo);
        break;
      case DWG_DT_H:
        printf("H: %i.%i.%li", dwg_struct->var[i].handle.code,
            dwg_struct->var[i].handle.size, dwg_struct->var[i].handle.value);
        break;
      case DWG_DT_T:
        printf("T: \"%s\"", dwg_struct->var[i].text);
        break;
      case DWG_DT_CMC:
        printf("CMC: %u", dwg_struct->var[i].dubitoko);
        break;
      case DWG_DT_2RD:
        printf("X: %lg\t", dwg_struct->var[i].xy[0]);
        printf("Y: %lg", dwg_struct->var[i].xy[1]);
        break;
      case DWG_DT_3BD:
        printf("X: %lg\t", dwg_struct->var[i].xyz[0]);
        printf("Y: %lg\t", dwg_struct->var[i].xyz[1]);
        printf("Z: %lg", dwg_struct->var[i].xyz[2]);
        break;
      default:
        printf("No handleebla type: %i (var: %i)\n", dwg_var_map(
            dwg_struct->header.version, i), i);
        }
      puts("");
    }
  puts("");

  puts("**************************************************");
  puts("Section CLASSES");
  puts("**************************************************");
  for (i = 0; i < dwg_struct->num_classes; i++)
    {
      printf("Class: [%02u]\n", i);
      printf("\tNumber: %u\n", dwg_struct->class[i].number);
      printf("\tVersion: %u\n", dwg_struct->class[i].version);
      printf("\tApplication Name: \"%s\"\n", dwg_struct->class[i].appname);
      printf("\tC++ Name: \"%s\"\n", dwg_struct->class[i].cppname);
      printf("\tDXF Name: \"%s\"\n", dwg_struct->class[i].dxfname);
      printf("\tWas ghost: \"%s\"\n", dwg_struct->class[i].wasazombie ? "Yes"
          : "No");
      printf("\tEroId: %u\n", dwg_struct->class[i].item_class_id);
    }
  puts("");

  puts("**************************************************");
  puts("Section OBJECTS");
  puts("**************************************************");
  for (i = 0; i < dwg_struct->num_objects; i++)
    {
      Dwg_Object *obj;

      printf("(%u) ", i);
      obj = &dwg_struct->object[i];

      printf("Type: %s (%03i)\t", obj->type > 80 ? (obj->type
          == dwg_struct->dwg_ot_layout ? "LAYOUT" : "??")
          : dwg_obtype[obj->type], obj->type);
      printf("Size: %u\t", obj->size);
      printf("Handle: (%lu)\t", obj->handle);
      printf("Super-type: ");
      switch (obj->supertype)
        {
      case DWG_SUPERTYPE_ENTITY:
        puts("entity");
        dwg_print_entity(obj->tio.entity);
        break;
      case DWG_SUPERTYPE_OBJECT:
        puts("object");
        dwg_print_object(obj->tio.object);
        break;
      default:
        puts("unknown");
        continue;
        }

      switch (obj->type)
        {
      case DWG_TYPE_UNUSED:
        break;
      case DWG_TYPE_TEXT:
        dwg_print_TEXT(obj->tio.entity->tio.TEXT);
        break;
      case DWG_TYPE_ATTRIB:
        dwg_print_ATTRIB(obj->tio.entity->tio.ATTRIB);
        break;
      case DWG_TYPE_ATTDEF:
        dwg_print_ATTDEF(obj->tio.entity->tio.ATTDEF);
        break;
      case DWG_TYPE_BLOCK:
        dwg_print_BLOCK(obj->tio.entity->tio.BLOCK);
        break;
      case DWG_TYPE_ENDBLK:
      case DWG_TYPE_SEQEND:
        break;
      case DWG_TYPE_INSERT:
        dwg_print_INSERT(obj->tio.entity->tio.INSERT);
        break;
      case DWG_TYPE_MINSERT:
        dwg_print_MINSERT(obj->tio.entity->tio.MINSERT);
        break;
      case DWG_TYPE_VERTEX_2D:
        dwg_print_VERTEX_2D(obj->tio.entity->tio.VERTEX_2D);
        break;
      case DWG_TYPE_VERTEX_3D:
        dwg_print_VERTEX_3D(obj->tio.entity->tio.VERTEX_3D);
        break;
      case DWG_TYPE_VERTEX_MESH:
        dwg_print_VERTEX_MESH(obj->tio.entity->tio.VERTEX_MESH);
        break;
      case DWG_TYPE_VERTEX_PFACE:
        dwg_print_VERTEX_PFACE(obj->tio.entity->tio.VERTEX_PFACE);
        break;
      case DWG_TYPE_VERTEX_PFACE_FACE:
        dwg_print_VERTEX_PFACE_FACE(obj->tio.entity->tio.VERTEX_PFACE_FACE);
        break;
      case DWG_TYPE_POLYLINE_2D:
        dwg_print_POLYLINE_2D(obj->tio.entity->tio.POLYLINE_2D);
        break;
      case DWG_TYPE_POLYLINE_3D:
        dwg_print_POLYLINE_3D(obj->tio.entity->tio.POLYLINE_3D);
        break;
      case DWG_TYPE_ARC:
        dwg_print_ARC(obj->tio.entity->tio.ARC);
        break;
      case DWG_TYPE_CIRCLE:
        dwg_print_CIRCLE(obj->tio.entity->tio.CIRCLE);
        break;
      case DWG_TYPE_LINE:
        dwg_print_LINE(obj->tio.entity->tio.LINE);
        break;
      case DWG_TYPE_DIMENSION_ORDINATE:
        dwg_print_DIMENSION_ORDINATE(obj->tio.entity->tio.DIMENSION_ORDINATE);
        break;
      case DWG_TYPE_DIMENSION_LINEAR:
        dwg_print_DIMENSION_LINEAR(obj->tio.entity->tio.DIMENSION_LINEAR);
        break;
      case DWG_TYPE_DIMENSION_ALIGNED:
        dwg_print_DIMENSION_ALIGNED(obj->tio.entity->tio.DIMENSION_ALIGNED);
        break;
      case DWG_TYPE_DIMENSION_ANG3PT:
        dwg_print_DIMENSION_ANG3PT(obj->tio.entity->tio.DIMENSION_ANG3PT);
        break;
      case DWG_TYPE_DIMENSION_ANG2LN:
        dwg_print_DIMENSION_ANG2LN(obj->tio.entity->tio.DIMENSION_ANG2LN);
        break;
      case DWG_TYPE_DIMENSION_RADIUS:
        dwg_print_DIMENSION_RADIUS(obj->tio.entity->tio.DIMENSION_RADIUS);
        break;
      case DWG_TYPE_DIMENSION_DIAMETER:
        dwg_print_DIMENSION_DIAMETER(obj->tio.entity->tio.DIMENSION_DIAMETER);
        break;
      case DWG_TYPE_POINT:
        dwg_print_POINT(obj->tio.entity->tio.POINT);
        break;
      case DWG_TYPE__3DFACE:
        dwg_print__3DFACE(obj->tio.entity->tio._3DFACE);
        break;
      case DWG_TYPE_POLYLINE_PFACE:
        dwg_print_POLYLINE_PFACE(obj->tio.entity->tio.POLYLINE_PFACE);
        break;
      case DWG_TYPE_POLYLINE_MESH:
        dwg_print_POLYLINE_MESH(obj->tio.entity->tio.POLYLINE_MESH);
        break;
      case DWG_TYPE_SOLID:
        dwg_print_SOLID(obj->tio.entity->tio.SOLID);
        break;
      case DWG_TYPE_TRACE:
        dwg_print_TRACE(obj->tio.entity->tio.TRACE);
        break;
      case DWG_TYPE_SHAPE:
        dwg_print_SHAPE(obj->tio.entity->tio.SHAPE);
        break;
      case DWG_TYPE_VIEWPORT:
        dwg_print_VIEWPORT(obj->tio.entity->tio.VIEWPORT);
        break;
      case DWG_TYPE_ELLIPSE:
        dwg_print_ELLIPSE(obj->tio.entity->tio.ELLIPSE);
        break;
      case DWG_TYPE_SPLINE:
        dwg_print_SPLINE(obj->tio.entity->tio.SPLINE);
        break;
      case DWG_TYPE_REGION:
        dwg_print_REGION(obj->tio.entity->tio.REGION);
        break;
      case DWG_TYPE_3DSOLID:
        dwg_print_3DSOLID(obj->tio.entity->tio._3DSOLID);
        break;
      case DWG_TYPE_BODY:
        dwg_print_BODY(obj->tio.entity->tio.BODY);
        break;
      case DWG_TYPE_RAY:
        dwg_print_RAY(obj->tio.entity->tio.RAY);
        break;
      case DWG_TYPE_XLINE:
        dwg_print_XLINE(obj->tio.entity->tio.XLINE);
        break;
      case DWG_TYPE_DICTIONARY:
        dwg_print_DICTIONARY(obj->tio.object->tio.DICTIONARY);
        break;
      case DWG_TYPE_MTEXT:
        dwg_print_MTEXT(obj->tio.entity->tio.MTEXT);
        break;
      case DWG_TYPE_LEADER:
        dwg_print_LEADER(obj->tio.entity->tio.LEADER);
        break;
      case DWG_TYPE_TOLERANCE:
        dwg_print_TOLERANCE(obj->tio.entity->tio.TOLERANCE);
        break;
      case DWG_TYPE_MLINE:
        dwg_print_MLINE(obj->tio.entity->tio.MLINE);
        break;
      case DWG_TYPE_LAYER:
        dwg_print_LAYER(obj->tio.object->tio.LAYER);
        break;
      default:
        if (obj->type == dwg_struct->dwg_ot_layout)
          dwg_print_LAYOUT(obj->tio.object->tio.LAYOUT);
        else
          continue;
        }

      dwg_print_handleref(obj);

      puts("");
    }

  puts("**************************************************");
  puts("Section SECOND HEADER ");
  puts("**************************************************");
  for (i = 0; i < 14; i++)
    {
      printf("Rikordo[%02i] Longo: %u\tChain:", i,
          dwg_struct->second_header.handlerik[i].size);
      for (j = 0; j < dwg_struct->second_header.handlerik[i].size; j++)
        printf(" %02X", dwg_struct->second_header.handlerik[i].chain[j]);
      puts("");
    }
  puts("");

  puts("**************************************************");
  puts("Section MEASUREMENT");
  puts("**************************************************");
  printf("MEASUREMENT: 0x%08X\n", (unsigned int) dwg_struct->measurement);
  puts("");
}
