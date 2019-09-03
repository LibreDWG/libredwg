/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2014-2019 Free Software Foundation, Inc.                   */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * testsuite.c: generate XML data per entity
 * similar to the out_xml backend, but this uses libxml2 to add bloat for
 * the "professional" company-type folks.
 * written by Achyuta Piyush
 * modified by Reini Urban
 */

#include <string.h>
#include <stdlib.h>
#include "dwg.h"
#include "dwg_api.h"
#include "../../programs/suffix.inc"
#include <libxml/tree.h>
#include <libxml/parser.h>
#include "common.c"

// entities to check against:
// perl -lne'/type="(IAcad.*?)" / and print $1' test/test-data/*/*.xml|sort -u
/*
    IAcad3DPolyline
    IAcadArc
    IAcadCircle
    IAcadEllipse
    IAcadHelix
    IAcadLWPolyline
    IAcadLine
    IAcadMLine
    IAcadMText
    IAcadPoint
    IAcadRay
    IAcadSpline
    IAcadXline
 */
int load_dwg (char *dwgfilename, xmlNodePtr rootnode);
void common_entity_attrs (xmlNodePtr node, const Dwg_Object *obj);
void add_2dpolyline (xmlNodePtr rootnode, const Dwg_Object *obj);
void add_3dpolyline (xmlNodePtr rootnode, const Dwg_Object *obj);
void add_arc (xmlNodePtr rootnode, const Dwg_Object *obj);
void add_block (xmlNodePtr rootnode, const Dwg_Object *obj);
void add_circle (xmlNodePtr rootnode, const Dwg_Object *obj);
void add_ellipse (xmlNodePtr rootnode, const Dwg_Object *obj);
void add_helix (xmlNodePtr rootnode, const Dwg_Object *obj);
void add_insert (xmlNodePtr rootnode, const Dwg_Object *obj);
void add_line (xmlNodePtr rootnode, const Dwg_Object *obj);
void add_lwpolyline (xmlNodePtr rootnode, const Dwg_Object *obj);
void add_mline (xmlNodePtr rootnode, const Dwg_Object *obj);
void add_point (xmlNodePtr rootnode, const Dwg_Object *obj);
void add_ray (xmlNodePtr rootnode, const Dwg_Object *obj);
void add_spline (xmlNodePtr rootnode, const Dwg_Object *obj);
void add_table (xmlNodePtr rootnode, const Dwg_Object *obj);
void add_text (xmlNodePtr rootnode, const Dwg_Object *obj);
void add_xline (xmlNodePtr rootnode, const Dwg_Object *obj);

#define newXMLProp(name, buf)                                                 \
  xmlNewProp (node, (const xmlChar *)name, buf);                              \
  free (buf)
#define newXMLcProp(name, buf)                                                \
  xmlNewProp (node, (const xmlChar *)name, (xmlChar *)(buf))
#define newXMLEntity(rootnode)                                                \
  xmlNewChild (rootnode, NULL, (const xmlChar *)"DwgEntity", NULL)

/*
 * Creates some of the common attributes, luckily we don't have to care about
 * the order.
 * @params xmlNodePtr node       The XML DwgEntity node
 * @params const Dwg_Object *obj The DWG object
 * @return none
 */
void
common_entity_attrs (xmlNodePtr node, const Dwg_Object *obj)
{
  int error;
  xmlChar *buf;
  char *name;
  Dwg_Object_Entity *ent = obj->tio.entity;

  // EntityTransparency, ObjectID, ObjectID32, Visible

  buf = doubletohex (obj->handle.value);
  newXMLProp ("Handle", buf);

  buf = ent->xdicobjhandle ? (xmlChar *)"1" : (xmlChar *)"0";
  newXMLcProp ("HasExtensionDictionary", buf);

  // Always return the default "0"
  name = dwg_ent_get_layer_name (ent, &error);
  if (!error)
    newXMLcProp ("Layer", name); // leaks r2007+

  // Always return the default: ByLayer
  name = dwg_ref_get_table_name (ent->ltype, &error);
  if (!error)
    newXMLcProp ("Linetype", name); // leaks r2007+

  buf = doubletochar (ent->ltype_scale);
  newXMLProp ("LinetypeScale", buf);

  buf = inttochar (dxf_cvt_lweight (ent->linewt));
  newXMLProp ("Lineweight", buf);

  name = dwg_ref_get_table_name (ent->material, &error);
  if (!error)
    newXMLcProp ("Material", name); // leaks r2007+

  name = dwg_ref_get_table_name (ent->plotstyle, &error);
  if (!error)
    newXMLcProp ("PlotStyleName", name); // leaks r2007+
}

/*
 * This function is used to emit all line attributes
 * @param xmlNodePtr rootnode The root node of XML document
 * @param const Dwg_Object *obj The DWG Object
 */
void
add_line (xmlNodePtr rootnode, const Dwg_Object *obj)
{
  Dwg_Entity_LINE *line = obj->tio.entity->tio.LINE;
  xmlChar *buf;
  xmlNodePtr node = newXMLEntity (rootnode);

  newXMLcProp ("type", "IAcadLine");
  newXMLcProp ("desc", "IAcadLine: AutoCAD Line Interface");

  buf = spointprepare (line->end.x, line->end.y, line->end.z);
  newXMLProp ("EndPoint", buf);

  buf = spointprepare (line->start.x, line->start.y, line->start.z);
  newXMLProp ("StartPoint", buf);

  common_entity_attrs (node, obj);
  xmlAddChild (rootnode, node);
}

/*
 * This function is emits all circle attributes in the XML file
 * @params xmlNodePtr rootnode The root node of the XML Document
 * @param const Dwg_Object *obj The DWG Object
 */
void
add_circle (xmlNodePtr rootnode, const Dwg_Object *obj)
{
  Dwg_Entity_CIRCLE *circle = obj->tio.entity->tio.CIRCLE;
  xmlChar *buf, *dtostring;
  xmlNodePtr node = newXMLEntity (rootnode);

  newXMLcProp ("type", "IAcadCircle");
  newXMLcProp ("desc", "IAcadCircle: AutoCAD Circle Interface");

  buf = spointprepare (circle->center.x, circle->center.y, circle->center.z);
  newXMLProp ("Center", buf);

  dtostring = doubletochar (circle->radius);
  newXMLProp ("Radius", dtostring);

  dtostring = doubletochar (circle->thickness);
  newXMLProp ("Thickness", dtostring);

  common_entity_attrs (node, obj);
  xmlAddChild (rootnode, node);
}

/*
 * This function is emits all hypothetical helix/spring attributes in the XML
 * file
 * @params xmlNodePtr rootnode The root node of the XML Document
 * @param const Dwg_Object *obj The DWG Object
 */
void
add_helix (xmlNodePtr rootnode, const Dwg_Object *obj)
{
  // TODO: decode some AcDbHelix class
  // but it is not even parsed by dwgread
  Dwg_Entity_CIRCLE *circle = obj->tio.entity->tio.CIRCLE;
  xmlChar *buf, *dtostring;
  xmlNodePtr node = newXMLEntity (rootnode);

  newXMLcProp ("type", "IAcadHelix");
  newXMLcProp ("desc", "IAcadHelix: IAcadSpring Interface");

  buf = spointprepare (circle->center.x, circle->center.y, circle->center.z);
  newXMLProp ("Position", buf);

  dtostring = doubletochar (circle->radius);
  newXMLProp ("BaseRadius", dtostring);

  dtostring = doubletochar (circle->thickness);
  newXMLProp ("Thickness", dtostring);

  dtostring = doubletochar (1);
  newXMLProp ("Constrain", dtostring); // typo!

  dtostring = doubletochar (circle->thickness);
  newXMLProp ("TopRadius", dtostring);

  dtostring = doubletochar (circle->thickness);
  newXMLProp ("TotalLength", dtostring);

  dtostring = doubletochar (circle->thickness);
  newXMLProp ("TurnHeight", dtostring);

  dtostring = doubletochar (circle->thickness);
  newXMLProp ("Turns", dtostring);

  dtostring = doubletochar (circle->thickness);
  newXMLProp ("TurnSlope", dtostring);

  dtostring = doubletochar (circle->thickness);
  newXMLProp ("Twist", dtostring);

  common_entity_attrs (node, obj);
  xmlAddChild (rootnode, node);
}

/*
 * This function emits some LWPolyline related attributes in XML
 * @params xmlNodePtr rootnode The root node of the XML document
 * @params Dwg_Object The DWG Object
 */
void
add_lwpolyline (xmlNodePtr rootnode, const Dwg_Object *obj)
{
  Dwg_Entity_LWPOLYLINE *lwpline = obj->tio.entity->tio.LWPOLYLINE;
  int error;
  BITCODE_RL j, numpts = dwg_ent_lwpline_get_numpoints (lwpline, &error);
  dwg_point_2d *pts = dwg_ent_lwpline_get_points (lwpline, &error);
  xmlChar *buf = NULL;
  xmlNodePtr node = newXMLEntity (rootnode);

  newXMLcProp ("type", "IAcadLWPolyline");
  newXMLcProp ("desc",
               "IAcadLWPolyline: AutoCAD Lightweight Polyline Interface");

  // flag?
  newXMLcProp ("Closed", "0");
  // ConstantWidth="0.0"

  if (numpts >= 3)
    {
      buf = malloc (80);
      sprintf ((char *)buf, "(%.4f %.4f %.4f %.4f %.4f %.4f ... )", pts[0].x,
               pts[0].y, pts[1].x, pts[1].y, pts[2].x, pts[2].y);
      newXMLProp ("Coordinates", buf);
      free (buf);
    }
  free (pts);

  buf = doubletochar (lwpline->elevation);
  newXMLProp ("Elevation", buf);

  buf = spointprepare (lwpline->extrusion.x, lwpline->extrusion.y,
                       lwpline->extrusion.z);
  newXMLProp ("Extrusion", buf);

  newXMLcProp ("ObjectName", "AcDbPolyline");

  buf = doubletochar (lwpline->thickness);
  newXMLProp ("Thickness", buf);

  common_entity_attrs (node, obj);
  xmlAddChild (rootnode, node);
}

/*
 * This function emits some 2D Polyline related attributes in XML
 * @params xmlNodePtr rootnode The root node of the XML document
 * @params Dwg_Object The DWG Object
 */
void
add_2dpolyline (xmlNodePtr rootnode, const Dwg_Object *obj)
{
  // Dwg_Entity_POLYLINE_2D *polyline2d = obj->tio.entity->tio.POLYLINE_2D;
  int error;
  BITCODE_RL j, numpts = dwg_object_polyline_2d_get_numpoints (obj, &error);
  dwg_point_2d *pts = dwg_object_polyline_2d_get_points (obj, &error);
  xmlChar *buf = NULL;

  // the start of the entity
  xmlNodePtr node = newXMLEntity (rootnode);

  newXMLcProp ("type", "IAcad2DPolyline");
  newXMLcProp ("desc", "IAcad2DPolyline: AutoCAD 2dPolyline Interface");

  // flag?
  newXMLcProp ("Closed", "0");

  if (numpts >= 3)
    {
      buf = malloc (80);
      sprintf ((char *)buf, "(%.4f %.4f %.4f %.4f %.4f %.4f ... )", pts[0].x,
               pts[0].y, pts[1].x, pts[1].y, pts[2].x, pts[2].y);
      newXMLProp ("Coordinates", buf);
    }
  free (pts);

  common_entity_attrs (node, obj);
  xmlAddChild (rootnode, node);
}

//@TODO This is yet to be completed

/*
 * This function emits some 3D Polyline related attributes in XML
 * @params xmlNodePtr rootnode The root node of the XML document
 * @params Dwg_Object The DWG Object
 */
void
add_3dpolyline (xmlNodePtr rootnode, const Dwg_Object *obj)
{
  // Dwg_Entity_POLYLINE_3D *polyline3d = obj->tio.entity->tio.POLYLINE_3D;
  int error;
  BITCODE_RL j, numpts = dwg_object_polyline_3d_get_numpoints (obj, &error);
  dwg_point_3d *pts = dwg_object_polyline_3d_get_points (obj, &error);
  xmlChar *buf = NULL;
  xmlNodePtr node = newXMLEntity (rootnode);

  newXMLcProp ("type", "IAcad3DPolyline");
  newXMLcProp ("desc", "IAcad3DPolyline: AutoCAD 3dPolyline Interface");

  // flag?
  newXMLcProp ("Closed", "0");

  if (numpts >= 2)
    {
      buf = malloc (80);
      sprintf ((char *)buf, "(%.4f %.4f %.4f %.4f %.4f %.4f ... )", pts[0].x,
               pts[0].y, pts[0].z, pts[1].x, pts[1].y, pts[1].z);
      newXMLProp ("Coordinates", buf);
    }
  free (pts);

  common_entity_attrs (node, obj);
  newXMLcProp ("ObjectName", "Acad3DPolyline");
  xmlAddChild (rootnode, node);
}

/*
 * This function emits all arc related attributes
 * @params xmlNodePtr rootnode The root node of the XML Document
 * @params const Dwg_Object *obj The DWG Object
 */
void
add_arc (xmlNodePtr rootnode, const Dwg_Object *obj)
{
  Dwg_Entity_ARC *arc = obj->tio.entity->tio.ARC;
  xmlChar *buf, *dtostring;

  // the start of the entity
  xmlNodePtr node = newXMLEntity (rootnode);

  newXMLcProp ("type", "IAcadArc");
  newXMLcProp ("desc", "IAcadArc: AutoCAD Arc Interface");

  buf = spointprepare (arc->center.x, arc->center.y, arc->center.z);
  newXMLProp ("Center", buf);

  dtostring = doubletochar (arc->end_angle);
  newXMLProp ("EndAngle", dtostring);

  dtostring = doubletochar (arc->radius);
  newXMLProp ("Radius", dtostring);

  dtostring = doubletochar (arc->start_angle);
  newXMLProp ("StartAngle", dtostring);

  dtostring = doubletochar (arc->thickness);
  newXMLProp ("Thickness", dtostring);

  common_entity_attrs (node, obj);
  xmlAddChild (rootnode, node);
}

/*
 * This functions emits all block related attributes in the XML
 * @param xmlNodePtr rootnode The root node of the XML document
 * @param const Dwg_Object *obj The DWG Object
 */
void
add_block (xmlNodePtr rootnode, const Dwg_Object *obj)
{
  Dwg_Entity_BLOCK *block = obj->tio.entity->tio.BLOCK;
  xmlNodePtr node = newXMLEntity (rootnode);

  newXMLcProp ("type", "IAcadBlock");
  newXMLcProp ("desc", "IAcadBlock: AutoCAD Block Interface");

  newXMLcProp ("EffectiveName", block->name);

  common_entity_attrs (node, obj);
  xmlAddChild (rootnode, node);
}

/*
 * This functions emits all insert related attributes in the XML
 * @param xmlNodePtr rootnode The root node of the XML document
 * @param const Dwg_Object *obj The DWG Object
 */
void
add_insert (xmlNodePtr rootnode, const Dwg_Object *obj)
{
  Dwg_Entity_INSERT *block = obj->tio.entity->tio.INSERT;
  xmlNodePtr node = newXMLEntity (rootnode);
  xmlChar *buf;
  int error;

  newXMLcProp ("type", "IAcadBlockReference");
  newXMLcProp ("desc",
               "IAcadBlockReference: AutoCAD Block Reference Interface");

  newXMLcProp ("EffectiveName",
               dwg_ref_get_table_name (block->block_header, &error));

  buf = spointprepare (block->ins_pt.x, block->ins_pt.y, block->ins_pt.z);
  newXMLProp ("BasePoint", buf);

  buf = spointprepare (block->scale.x, block->scale.y, block->scale.z);
  newXMLProp ("Scale", buf);

  buf = doubletochar (block->rotation);
  newXMLProp ("Rotation", buf);

  common_entity_attrs (node, obj);
  xmlAddChild (rootnode, node);
}

/*
 * This function emits all the ellipse related attributes
 * @param xmlNodePtr rootnode The root node of the XML document
 * @param Dwg_Object The DWG Object
 */
void
add_ellipse (xmlNodePtr rootnode, const Dwg_Object *obj)
{
  Dwg_Entity_ELLIPSE *ellipse = obj->tio.entity->tio.ELLIPSE;
  xmlNodePtr node = newXMLEntity (rootnode);
  xmlChar *buf, *dtostring;

  // Now the attributes
  newXMLcProp ("type", "IAcadEllipse");
  newXMLcProp ("desc", "IAcadEllipse: AutoCAD Ellipse Interface");

  buf = spointprepare (ellipse->center.x, ellipse->center.y,
                       ellipse->center.z);
  newXMLProp ("Center", buf);

  dtostring = doubletochar (ellipse->end_angle);
  newXMLProp ("EndAngle", dtostring);

  dtostring = doubletochar (ellipse->start_angle);
  newXMLProp ("StartAngle", dtostring);

  // Axis Ratio and Sm Axis

  common_entity_attrs (node, obj);
  xmlAddChild (rootnode, node);
}

// @TODO: This is not proper. Still some attributes are missing
/*
 * This function emits all the MLINE related attributes
 * @param xmlNodePtr rootnode The root node of the XML document
 * @param const Dwg_Object *obj The DWG Object
 */
void
add_mline (xmlNodePtr rootnode, const Dwg_Object *obj)
{
  Dwg_Entity_MLINE *mline = obj->tio.entity->tio.MLINE;
  xmlNodePtr node = newXMLEntity (rootnode);
  xmlChar *buf, *dtostring;

  newXMLcProp ("type", "IAcadMLine");
  newXMLcProp ("desc", "IAcadMLine: IAcadMLine Interface");

  if (mline->num_verts >= 3)
    {
      buf = malloc (80);
      sprintf ((char *)buf,
               "(%.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f ... )",
               mline->verts[0].vertex.x, mline->verts[0].vertex.y,
               mline->verts[0].vertex.z, mline->verts[1].vertex.x,
               mline->verts[1].vertex.y, mline->verts[1].vertex.z,
               mline->verts[2].vertex.x, mline->verts[2].vertex.y,
               mline->verts[2].vertex.z);
      newXMLProp ("Coordinates", buf);
    }

  dtostring = doubletochar (mline->justification);
  newXMLProp ("Justification", dtostring);

  dtostring = doubletochar (mline->scale);
  newXMLProp ("MLineScale", dtostring);

  common_entity_attrs (node, obj);
  xmlAddChild (rootnode, node);
}

/*
 * This function is used to emit all line attributes
 * @param xmlNodePtr rootnode The root node of XML document
 * @param const Dwg_Object *obj The DWG Object
 */
void
add_xline (xmlNodePtr rootnode, const Dwg_Object *obj)
{
  Dwg_Entity_XLINE *line = obj->tio.entity->tio.XLINE;
  xmlChar *buf;
  xmlNodePtr node = newXMLEntity (rootnode);

  newXMLcProp ("type", "IAcadXline");
  newXMLcProp ("desc", "IAcadXline: AutoCAD Xline Interface");

  buf = spointprepare (line->point.x, line->point.y, line->point.z);
  newXMLProp ("BasePoint", buf);

  buf = spointprepare (line->vector.x, line->vector.y, line->vector.z);
  newXMLProp ("DirectionVector", buf);

  /*
  buf = spointprepare (line->start.x, line->start.y, line->start.z);
  newXMLProp ("SecondPoint", buf);
  */

  common_entity_attrs (node, obj);
  xmlAddChild (rootnode, node);
}

/*
 * This function emits all the point related attributes
 * @param xmlNodePtr rootnode The root node of the XML document
 * @param const Dwg_Object *obj The DWG Object
 */
void
add_point (xmlNodePtr rootnode, const Dwg_Object *obj)
{
  Dwg_Entity_POINT *point = obj->tio.entity->tio.POINT;
  xmlChar *buf, *dtostring;
  xmlNodePtr node = newXMLEntity (rootnode);

  newXMLcProp ("type", "IAcadPoint");
  newXMLcProp ("desc", "IAcadPoint: AutoCAD Point Interface");

  buf = spointprepare (point->x, point->y, point->z);
  newXMLProp ("Coordinates", buf);

  dtostring = doubletochar (point->thickness);
  newXMLProp ("Thickness", dtostring);

  common_entity_attrs (node, obj);
  xmlAddChild (rootnode, node);
}

/*
 * This function emits all the ray related attributes in the XML
 * @param xmlNodePtr rootnode The root node of the XML document
 * @param Dwg_Object The DWG Object
 */
void
add_ray (xmlNodePtr rootnode, const Dwg_Object *obj)
{
  Dwg_Entity_RAY *ray = obj->tio.entity->tio.RAY;
  xmlChar *buf;
  xmlNodePtr node = newXMLEntity (rootnode);

  newXMLcProp ("type", "IAcadRay");
  newXMLcProp ("desc", "IAcadRay: AutoCAD Ray Interface");

  buf = spointprepare (ray->point.x, ray->point.y, ray->point.z);
  newXMLProp ("BasePoint", buf);

  buf = spointprepare (ray->vector.x, ray->vector.y, ray->vector.z);
  newXMLProp ("DirectionVector", buf);

  common_entity_attrs (node, obj);
  xmlAddChild (rootnode, node);
}

/*
 * This function emits all the spline related attributes
 * @param xmlNodePtr rootnode The rootnode of the XML Document
 * @param const Dwg_Object *obj The DWG Object
 */
void
add_spline (xmlNodePtr rootnode, const Dwg_Object *obj)
{
  Dwg_Entity_SPLINE *spline = obj->tio.entity->tio.SPLINE;
  xmlChar *buf, *dtostring;
  xmlNodePtr node = newXMLEntity (rootnode);

  newXMLcProp ("type", "IAcadSpline");
  newXMLcProp ("desc", "IAcadSpline: AutoCAD Spline Interface");

  dtostring = doubletochar (spline->closed_b);
  newXMLProp ("Closed", dtostring);

  dtostring = doubletochar (spline->degree);
  newXMLProp ("Degree", dtostring);

  buf = spointprepare (spline->end_tan_vec.x, spline->end_tan_vec.y,
                       spline->end_tan_vec.z);
  newXMLProp ("EndTangent", buf);

  dtostring = doubletochar (spline->fit_tol);
  newXMLProp ("FitTolerance", dtostring);

  // This is for fit points. DO this too

  // @TODO this is an array. Use it properly
  // fprintf(file, (const xmlChar *)"ControlPoints='(%f %f %f %f)' ", );

  common_entity_attrs (node, obj);
  xmlAddChild (rootnode, node);
}

/*
 * This function emits all the text related attributes
 * @param xmlNodePtr rootnode The rootnode of the XML Document
 * @param Dwg_Object The DWG Object
 */
void
add_text (xmlNodePtr rootnode, const Dwg_Object *obj)
{
  Dwg_Entity_TEXT *text = obj->tio.entity->tio.TEXT;
  xmlChar *buf, *dtostring;
  xmlNodePtr node = newXMLEntity (rootnode);

  newXMLcProp ("type", "IAcadText");
  newXMLcProp ("desc", "IAcadText: AutoCAD Text Interface");

  dtostring = doubletochar (text->height);
  newXMLProp ("Height", dtostring);

  buf = spointprepare2 (text->insertion_pt.x, text->insertion_pt.y);
  newXMLProp ("InsertionPoint", buf);

  newXMLcProp ("TextString", (xmlChar *)text->text_value);

  dtostring = doubletochar (text->width_factor);
  newXMLProp ("Width", dtostring);

  //@TODO: Lots of attributes were also left. Check this.

  common_entity_attrs (node, obj);
  xmlAddChild (rootnode, node);
}

/*
 * This functions emits all the table related attributes in the XML
 * @param xmlNodePtr rootnode The root node of the XML document
 * @param const Dwg_Object *obj The DWG Object
 */
void
add_table (xmlNodePtr rootnode, const Dwg_Object *obj)
{
  Dwg_Entity_TABLE *table = obj->tio.entity->tio.TABLE;
  xmlChar *buf, *dtostring;
  xmlNodePtr node = newXMLEntity (rootnode);

  newXMLcProp ("type", "IAcadTable");
  newXMLcProp ("desc", "IAcadTable: IAcadTable Interface");

  buf = spointprepare (table->insertion_point.x, table->insertion_point.y,
                       table->insertion_point.z);
  newXMLProp ("InsertionPoint", buf);

  dtostring = doubletochar (table->num_rows);
  newXMLProp ("Rows", dtostring);

  //@TODO: Lots of attributes are here. Need to figure out them and match them

  common_entity_attrs (node, obj);
  xmlAddChild (rootnode, node);
}

// Main function
int
load_dwg (char *dwgfilename, xmlNodePtr rootnode)
{
  BITCODE_BL i;
  int error;
  Dwg_Data dwg;

  dwg.num_objects = 0;

  // Read the DWG file
  dwg.opts = 0; // silently
  error = dwg_read_file (dwgfilename, &dwg);
  if (error)
    return error;

  // Emit some entities/objects to the XML file
  for (i = 0; i < dwg.num_objects; i++)
    {
      const Dwg_Object *obj = &dwg.object[i];
      switch (dwg.object[i].type)
        {
        case DWG_TYPE_ARC:
          add_arc (rootnode, obj);
          break;

        case DWG_TYPE_LINE:
          add_line (rootnode, obj);
          break;

        case DWG_TYPE_CIRCLE:
          add_circle (rootnode, obj);
          break;

        case DWG_TYPE_POLYLINE_2D:
          add_2dpolyline (rootnode, obj);
          break;

        case DWG_TYPE_POLYLINE_3D:
          add_3dpolyline (rootnode, obj);
          break;

        case DWG_TYPE_BLOCK:
          add_block (rootnode, obj);
          break;

        case DWG_TYPE_INSERT:
          add_insert (rootnode, obj);
          break;

        case DWG_TYPE_MLINE:
          add_mline (rootnode, obj);
          break;

        case DWG_TYPE_ELLIPSE:
          add_ellipse (rootnode, obj);
          break;

        case DWG_TYPE_POINT:
          add_point (rootnode, obj);
          break;

        case DWG_TYPE_RAY:
          add_ray (rootnode, obj);
          break;

          /*case DWG_TYPE_HELIX:
            add_helix (rootnode, obj);
            break;*/

        case DWG_TYPE_TEXT:
          add_text (rootnode, obj);
          break;

        case DWG_TYPE_SPLINE:
          add_spline (rootnode, obj);
          break;

        case DWG_TYPE_XLINE:
          add_xline (rootnode, obj);
          break;

          /*      case DWG_TYPE_TABLE:
                    add_table(rootnode, obj);
                    break;*/

        default:
          if (obj->type < 500 || (obj->type - 500) >= dwg.num_classes)
            break;
          if (!obj->dxfname || obj->supertype == DWG_SUPERTYPE_UNKNOWN)
            break;
          if (!strcmp (obj->dxfname, "HELIX"))
            add_helix (rootnode, obj);

          break;
        }
    }

  // free the dwg
  dwg_free (&dwg);
  return error;
}

int
main (int argc, char *argv[])
{
  // Basic Root Entity
  char *xml = (char *)"<DwgData></DwgData>";
  xmlDocPtr doc;
  xmlNodePtr root;
  int error;

  // Check if we have the filename
  REQUIRE_INPUT_FILE_ARG (argc);

  // Load the XML interface
  LIBXML_TEST_VERSION

  doc = xmlParseMemory (xml, xmlStrlen ((xmlChar *)xml));
  // Check if it was able to read it
  if (doc == NULL)
    {
      printf ("xmlDoc Creation Failed. Please Check;\n");
      return 1;
    }

  // We have xml loaded. Now get handle to root node
  root = xmlDocGetRootElement (doc);

  if (root == NULL)
    {
      printf ("Cannot get handle to root node. Please Check\n");
      return 1;
    }

  // Load the DWG file
  error = load_dwg (argv[1], root);
  if (error)
    {
      xmlFreeDoc (doc);
      xmlCleanupParser ();
      return error;
    }

  // Save the XML and Cleanup
  xmlSaveFormatFileEnc (argv[2], doc, "UTF-8", 1);
  xmlFreeDoc (doc);
  xmlCleanupParser ();

  return 0;
}
