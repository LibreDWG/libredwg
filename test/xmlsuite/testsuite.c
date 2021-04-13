/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2014-2020 Free Software Foundation, Inc.                   */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * testsuite.c: Generate XML data per entity.
 *   Similar to the out_xml backend, but this uses libxml2 to add bloat for
 *   the "professional" company-type folks.
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

#define MIN(a, b) ((a > b) ? b : a)

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
int xml_dwg (char *dwgfilename, xmlNodePtr rootnode);
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
void add_mtext (xmlNodePtr rootnode, const Dwg_Object *obj);
void add_xline (xmlNodePtr rootnode, const Dwg_Object *obj);

#define newXMLProp(name, buf)                                                 \
  {                                                                           \
    xmlChar *tmp = buf;                                                       \
    xmlNewProp (node, (const xmlChar *)name, tmp);                            \
    FREE (tmp);                                                               \
  }
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
  Dwg_Version_Type dwg_version = obj->parent->header.from_version;
  Dwg_Object_Entity *ent = obj->tio.entity;
  dwg_point_3d extrusion;
  double thickness;

  // TODO: EntityTransparency (ByLayer), ObjectID=int, ObjectID32=int, Visible,
  // TrueColor OwnerID=int OwnerID32=int TrueColor Document=dwg ptr

  buf = doubletohex (obj->handle.value);
  newXMLProp ("Handle", buf);

  newXMLcProp ("HasExtensionDictionary", ent->xdicobjhandle ? "1" : "0");

  name = dwg_ent_get_layer_name (ent, &error);
  if (!error)
    {
      newXMLcProp ("Layer", name);
      if (dwg_version >= R_2007)
        FREE (name);
    }

  name = dwg_ref_get_table_name (ent->ltype, &error);
  if (!error)
    {
      newXMLcProp ("Linetype", name);
      if (dwg_version >= R_2007)
        FREE (name);
    }

  buf = doubletochar (ent->ltype_scale);
  newXMLProp ("LinetypeScale", buf);

  buf = inttochar (dxf_cvt_lweight (ent->linewt));
  newXMLProp ("Lineweight", buf);

  name = dwg_ref_get_table_name (ent->material, &error);
  if (!error)
    {
      newXMLcProp ("Material", name);
      if (dwg_version >= R_2007)
        FREE (name);
    }

  name = dwg_ref_get_table_name (ent->plotstyle, &error);
  if (!error)
    {
      newXMLcProp ("PlotStyleName", name);
      if (dwg_version >= R_2007)
        FREE (name);
    }

  /* Arc, Attribute, AttributeReference, BlockRef, Circle,
     Dim3PointAngular, DimAligned, DimAngular, DimArcLength,
     DimDiametric, DimOrdinate, DimRadial, DimRadialLarge, DimRotated,
     Ellipse, ExternalReference, Hatch, Leader, LightweightPolyline,
     Line, MInsertBlock, MText, Point, Polyline, Region, Section,
     Shape, Solid, Text, Tolerance, Trace */
  if (dwg_dynapi_entity_value (ent->tio.LINE, obj->name, "extrusion",
                               &extrusion, NULL))
    {
      buf = spointprepare (extrusion.x, extrusion.y, extrusion.z);
      newXMLProp ("Normal", buf);
    }

  /* Arc, Attribute, AttributeReference, Circle, LightweightPolyline,
     Line, Point, Polyline, Shape, Solid, Text, Trace */
  if (dwg_dynapi_entity_value (ent->tio.LINE, obj->name, "thickness",
                               &thickness, NULL))
    {
      buf = doubletochar (thickness);
      newXMLProp ("Thickness", buf);
    }
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
  newXMLcProp ("ObjectName", "AcDbLine");

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
  newXMLcProp ("ObjectName", "AcDbCircle");

  buf = spointprepare (circle->center.x, circle->center.y, circle->center.z);
  newXMLProp ("Center", buf);

  dtostring = doubletochar (circle->radius);
  newXMLProp ("Radius", dtostring);

  /* See common_entity_attrs
  dtostring = doubletochar (circle->thickness);
  newXMLProp ("Thickness", dtostring);
  */

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
  Dwg_Entity_HELIX *helix = obj->tio.entity->tio.HELIX;
  xmlChar *buf, *dtostring;
  xmlNodePtr node = newXMLEntity (rootnode);

  newXMLcProp ("type", "IAcadHelix");
  newXMLcProp ("desc", "IAcadHelix: IAcadSpring Interface");
  newXMLcProp ("ObjectName", "AcDbHelix");

  buf = spointprepare (helix->start_pt.x, helix->start_pt.y,
                       helix->start_pt.z);
  newXMLProp ("Position", buf);

  buf = spointprepare (helix->axis_base_pt.x, helix->axis_base_pt.y,
                       helix->axis_base_pt.z);
  newXMLProp ("AxisPoint", buf);

  buf = spointprepare (helix->axis_vector.x, helix->axis_vector.y,
                       helix->axis_vector.z);
  newXMLProp ("AxisVector", buf);

  dtostring = doubletochar (helix->turn_height);
  newXMLProp ("Height", dtostring);

  dtostring = doubletochar (helix->radius);
  newXMLProp ("BaseRadius", dtostring);

  dtostring = doubletochar (0);
  newXMLProp ("TopRadius", dtostring);

  dtostring = doubletochar (helix->turn_height);
  newXMLProp ("TurnHeight", dtostring);

  dtostring = doubletochar (helix->turns);
  newXMLProp ("Turns", dtostring);

  dtostring = doubletochar (0);
  newXMLProp ("Twist", dtostring);

  dtostring = doubletochar (helix->constraint_type);
  newXMLProp ("Constrain", dtostring); // typo?

  dtostring = doubletochar (0);
  newXMLProp ("TurnSlope", dtostring);

  dtostring = doubletochar (0);
  newXMLProp ("TotalLength", dtostring);

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
  newXMLcProp ("ObjectName", "AcDbPolyline");

  newXMLcProp ("Closed", lwpline->flag & 1 ? "1" : "0");
  // ConstantWidth="0.0"

  if (numpts >= 3)
    {
      buf = MALLOC (80);
      sprintf ((char *)buf, "(%.4f %.4f %.4f %.4f %.4f %.4f ... )", pts[0].x,
               pts[0].y, pts[1].x, pts[1].y, pts[2].x, pts[2].y);
      newXMLProp ("Coordinates", buf);
      FREE (buf);
    }
  FREE (pts);

  buf = doubletochar (lwpline->elevation);
  newXMLProp ("Elevation", buf);

  /* See common_entity_attrs
  buf = spointprepare (lwpline->extrusion.x, lwpline->extrusion.y,
                       lwpline->extrusion.z);
  newXMLProp ("Normal", buf);
  */

  /* See common_entity_attrs
  buf = doubletochar (lwpline->thickness);
  newXMLProp ("Thickness", buf);
  */

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
  int error;
  Dwg_Entity_POLYLINE_2D *pline = obj->tio.entity->tio.POLYLINE_2D;
  BITCODE_RL j, numpts = dwg_object_polyline_2d_get_numpoints (obj, &error);
  dwg_point_2d *pts = dwg_object_polyline_2d_get_points (obj, &error);
  xmlChar *buf = NULL;

  // the start of the entity
  xmlNodePtr node = newXMLEntity (rootnode);

  newXMLcProp ("type", "IAcad2DPolyline");
  newXMLcProp ("desc", "IAcad2DPolyline: AutoCAD 2dPolyline Interface");
  newXMLcProp ("ObjectName", "AcDb2dPolyline");

  newXMLcProp ("Closed", pline->flag & 512 ? "1" : "0");

  buf = doubletochar (pline->elevation);
  newXMLProp ("Elevation", buf);

  if (numpts >= 3)
    {
      buf = MALLOC (80);
      sprintf ((char *)buf, "(%.4f %.4f %.4f %.4f %.4f %.4f ... )", pts[0].x,
               pts[0].y, pts[1].x, pts[1].y, pts[2].x, pts[2].y);
      newXMLProp ("Coordinates", buf);
    }
  FREE (pts);

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
  int error;
  Dwg_Entity_POLYLINE_3D *pline = obj->tio.entity->tio.POLYLINE_3D;
  BITCODE_RL j, numpts = dwg_object_polyline_3d_get_numpoints (obj, &error);
  dwg_point_3d *pts = dwg_object_polyline_3d_get_points (obj, &error);
  xmlChar *buf = NULL;
  xmlNodePtr node = newXMLEntity (rootnode);

  newXMLcProp ("type", "IAcad3DPolyline");
  newXMLcProp ("desc", "IAcad3DPolyline: AutoCAD 3dPolyline Interface");
  newXMLcProp ("ObjectName", "AcDb3dPolyline");

  newXMLcProp ("Closed", pline->flag & 512 ? "1" : "0");

  if (numpts >= 2)
    {
      buf = MALLOC (80);
      sprintf ((char *)buf, "(%.4f %.4f %.4f %.4f %.4f %.4f ... )", pts[0].x,
               pts[0].y, pts[0].z, pts[1].x, pts[1].y, pts[1].z);
      newXMLProp ("Coordinates", buf);
    }
  FREE (pts);

  common_entity_attrs (node, obj);
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
  newXMLcProp ("ObjectName", "AcDbArc");

  buf = spointprepare (arc->center.x, arc->center.y, arc->center.z);
  newXMLProp ("Center", buf);

  dtostring = doubletochar (arc->end_angle);
  newXMLProp ("EndAngle", dtostring);

  dtostring = doubletochar (arc->radius);
  newXMLProp ("Radius", dtostring);

  dtostring = doubletochar (arc->start_angle);
  newXMLProp ("StartAngle", dtostring);

  /* See common_entity_attrs
  dtostring = doubletochar (arc->thickness);
  newXMLProp ("Thickness", dtostring);
  */

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
  xmlNodePtr node;

  if (block->name[0] != '*')
    {
      node = newXMLEntity (rootnode);

      newXMLcProp ("type", "IAcadBlock");
      newXMLcProp ("desc", "IAcadBlock: AutoCAD Block Interface");
      newXMLcProp ("ObjectName", "AcDbBlock");

      newXMLcProp ("EffectiveName", block->name);

      common_entity_attrs (node, obj);
      xmlAddChild (rootnode, node);
    }
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
  newXMLcProp ("ObjectName", "AcDbEllipse");

  buf = spointprepare (ellipse->center.x, ellipse->center.y,
                       ellipse->center.z);
  newXMLProp ("Center", buf);

  dtostring = doubletochar (ellipse->start_angle);
  newXMLProp ("StartAngle", dtostring);

  dtostring = doubletochar (ellipse->end_angle);
  newXMLProp ("EndAngle", dtostring);

  // RadiusRatio and Axis
  buf = spointprepare (ellipse->sm_axis.x, ellipse->sm_axis.y,
                       ellipse->sm_axis.z);
  newXMLProp ("MajorAxis", buf);

  dtostring = doubletochar (ellipse->axis_ratio);
  newXMLProp ("RadiusRatio", dtostring);

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
  newXMLcProp ("ObjectName", "AcDbMline");

  if (mline->num_verts >= 3)
    {
      buf = MALLOC (80);
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
  newXMLcProp ("ObjectName", "AcDbXline");

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
  newXMLcProp ("ObjectName", "AcDbPoint");

  buf = spointprepare (point->x, point->y, point->z);
  newXMLProp ("Coordinates", buf);

  /* See common_entity_attrs
  dtostring = doubletochar (point->thickness);
  newXMLProp ("Thickness", dtostring);
  */

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
  newXMLcProp ("ObjectName", "AcDbRay");

  buf = spointprepare (ray->point.x, ray->point.y, ray->point.z);
  newXMLProp ("BasePoint", buf);

  buf = spointprepare (ray->vector.x, ray->vector.y, ray->vector.z);
  newXMLProp ("DirectionVector", buf);

  common_entity_attrs (node, obj);
  xmlAddChild (rootnode, node);
}

/*
 * This function emits most the spline related attributes
 * @param xmlNodePtr rootnode The rootnode of the XML Document
 * @param const Dwg_Object *obj The DWG Object
 */
void
add_spline (xmlNodePtr rootnode, const Dwg_Object *obj)
{
  Dwg_Entity_SPLINE *spline = obj->tio.entity->tio.SPLINE;
  xmlChar *buf, *dtostring;
  xmlNodePtr node = newXMLEntity (rootnode);
  int len;

  newXMLcProp ("type", "IAcadSpline");
  newXMLcProp ("desc", "IAcadSpline: AutoCAD Spline Interface");
  newXMLcProp ("ObjectName", "AcDbSpline");

  newXMLcProp ("Closed", spline->closed_b ? "1" : "0");
  newXMLProp ("Degree", doubletochar (spline->degree));
  // Closed2, Degree2

  buf = spointprepare (spline->beg_tan_vec.x, spline->beg_tan_vec.y,
                       spline->beg_tan_vec.z);
  newXMLProp ("StartTangent", buf);
  buf = spointprepare (spline->end_tan_vec.x, spline->end_tan_vec.y,
                       spline->end_tan_vec.z);
  newXMLProp ("EndTangent", buf);

  newXMLProp ("FitTolerance", doubletochar (spline->fit_tol));
  newXMLcProp ("IsPeriodic", spline->periodic ? "1" : "0");
  // IsPlanar="-1"
  newXMLcProp ("IsRational", spline->rational ? "1" : "0");
  if (spline->num_knots)
    {
      int bufsize = spline->num_knots * 16;
      buf = MALLOC (bufsize + 1);
      buf[0] = '(';
      buf[1] = '\0';
      len = 1;
      for (unsigned i = 0; i < MIN (6, spline->num_knots); i++)
        {
          // Knots="(0.0 0.0 0.0 0.0 12.2776 26.0835 ... )"
          char buf1[24];
          if (i == 0)
            len += snprintf (buf1, 24, "%f", spline->knots[i]);
          else
            len += snprintf (buf1, 24, " %f", spline->knots[i]);
          if (len > bufsize)
            {
              bufsize = len + 24;
              buf = REALLOC (buf, bufsize);
            }
          strncat ((char *)buf, buf1, bufsize - len);
        }
      if (spline->num_knots > 6)
        strcat ((char *)buf, " ... ");
      strcat ((char *)buf, ")");
      newXMLProp ("Knots", buf);
    }
  newXMLProp ("KnotParameterization", inttochar (spline->knotparam));
  newXMLProp ("NumberOfControlPoints", inttochar (spline->num_ctrl_pts));
  newXMLProp ("NumberOfFitPoints", inttochar (spline->num_fit_pts));
  newXMLcProp ("SplineFrame", spline->splineflags & 2 ? "1" : "0");
  newXMLcProp ("SplineMethod", spline->splineflags & 1 ? "1" : "0");

  if (spline->num_ctrl_pts)
    {
      int bufsize = spline->num_ctrl_pts * (24 * 4);
      buf = MALLOC (bufsize + 1);
      buf[0] = '(';
      buf[1] = '\0';
      len = 1;
      for (unsigned i = 0; i < MIN (2, spline->num_ctrl_pts); i++)
        {
          // ControlPoints="(0.0 0.0 0.0 0.0  12.2776 26.0835 ... )"
          char buf1[24];
          if (!i)
            len += snprintf (buf1, 24, "%f", spline->ctrl_pts[i].x);
          else
            len += snprintf (buf1, 24, " %f", spline->ctrl_pts[i].x);
          strncat ((char *)buf, buf1, bufsize - len);
          len += snprintf (buf1, 24, " %f", spline->ctrl_pts[i].y);
          strncat ((char *)buf, buf1, bufsize - len);
          len += snprintf (buf1, 24, " %f", spline->ctrl_pts[i].z);
          strncat ((char *)buf, buf1, bufsize - len);
          len += snprintf (buf1, 24, " %f ", spline->ctrl_pts[i].w);
          strncat ((char *)buf, buf1, bufsize - len);

          if (len > bufsize)
            {
              bufsize = len + (24 * 4);
              buf = REALLOC (buf, bufsize);
            }
        }
      if (spline->num_ctrl_pts > 2)
        strcat ((char *)buf, " ... ");
      strcat ((char *)buf, ")");
      newXMLProp ("ControlPoints", buf);
    }

  if (spline->num_fit_pts)
    {
      int bufsize = spline->num_fit_pts * (24 * 3);
      buf = MALLOC (bufsize + 1);
      buf[0] = '(';
      buf[1] = '\0';
      len = 1;
      for (unsigned i = 0; i < MIN (2, spline->num_fit_pts); i++)
        {
          char buf1[24];
          if (!i)
            len += snprintf (buf1, 24, "%f", spline->fit_pts[i].x);
          else
            len += snprintf (buf1, 24, " %f", spline->fit_pts[i].x);
          strncat ((char *)buf, buf1, bufsize - len);
          len += snprintf (buf1, 24, " %f", spline->fit_pts[i].y);
          strncat ((char *)buf, buf1, bufsize - len);
          len += snprintf (buf1, 24, " %f", spline->fit_pts[i].z);
          strncat ((char *)buf, buf1, bufsize - len);

          if (len > bufsize)
            {
              bufsize = len + (24 * 3);
              buf = REALLOC (buf, bufsize);
            }
        }
      if (spline->num_fit_pts > 2)
        strcat ((char *)buf, " ... ");
      strcat ((char *)buf, ")");
      newXMLProp ("FitPoints", buf);
    }

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
  const Dwg_Version_Type dwg_version = obj->parent->header.from_version;
  Dwg_Entity_TEXT *text = obj->tio.entity->tio.TEXT;
  xmlChar *buf, *dtostring;
  xmlNodePtr node = newXMLEntity (rootnode);
  char *name;
  int error;

  newXMLcProp ("type", "IAcadText");
  newXMLcProp ("desc", "IAcadText: AutoCAD Text Interface");
  newXMLcProp ("ObjectName", "AcDbText");

  dtostring = doubletochar (text->height);
  newXMLProp ("Height", dtostring);

  buf = spointprepare2 (text->ins_pt.x, text->ins_pt.y);
  newXMLProp ("InsertionPoint", buf);

  name = dwg_ref_get_table_name (text->style, &error);
  if (!error)
    {
      newXMLcProp ("StyleName", name);
      if (dwg_version >= R_2007)
        FREE (name);
    }

  newXMLcProp ("TextString", (xmlChar *)text->text_value);

  dtostring = doubletochar (text->width_factor);
  newXMLProp ("Width", dtostring);

  //@TODO: Lots of attributes were also left. Check MText also

  common_entity_attrs (node, obj);
  xmlAddChild (rootnode, node);
}

/*
 * This function emits all the multi-line text related attributes
 * @param xmlNodePtr rootnode The rootnode of the XML Document
 * @param Dwg_Object The DWG Object
 */
void
add_mtext (xmlNodePtr rootnode, const Dwg_Object *obj)
{
  const Dwg_Version_Type dwg_version = obj->parent->header.from_version;
  Dwg_Entity_MTEXT *text = obj->tio.entity->tio.MTEXT;
  xmlChar *buf, *dtostring;
  xmlNodePtr node = newXMLEntity (rootnode);
  // double rotation;
  char *name;
  int error;

  newXMLcProp ("type", "IAcadMText");
  newXMLcProp ("desc", "IAcadMText: AutoCAD MText Interface");
  newXMLcProp ("ObjectName", "AcDbMText");

  dtostring = doubletochar (text->rect_height);
  newXMLProp ("Height", dtostring);

  buf = spointprepare2 (text->ins_pt.x, text->ins_pt.y);
  newXMLProp ("InsertionPoint", buf);

  // rotation = geom_some(text->x_axis_dir);
  // dtostring = doubletochar (rotation);
  // newXMLProp ("Rotation", dtostring);

  buf = inttochar (text->attachment);
  newXMLProp ("AttachmentPoint", buf);
  buf = inttochar (text->bg_fill_flag);
  newXMLProp ("BackgroundFill", buf);
  if (text->bg_fill_flag)
    {
      dtostring = doubletochar (text->bg_fill_scale);
      newXMLProp ("BackgroundFill", dtostring);
    }
  // if (text->bg_fill_flag == 1)
  //   {
  //     buf = colortochar (text->bg_fill_color);
  //     newXMLProp ("BackgroundColor", buf);
  //   }
  buf = inttochar (text->flow_dir);
  newXMLProp ("DrawingDirection", buf);
  dtostring = doubletochar (text->text_height);
  newXMLProp ("LineSpacingDistance", dtostring);
  dtostring = doubletochar (text->linespace_factor);
  newXMLProp ("LineSpacingFactor", dtostring);
  buf = inttochar (text->linespace_style);
  newXMLProp ("LineSpacingStyle", buf);

  newXMLcProp ("TextString", (xmlChar *)text->text);

  name = dwg_ref_get_table_name (text->style, &error);
  if (!error)
    {
      newXMLcProp ("StyleName", name);
      if (dwg_version >= R_2007)
        FREE (name);
    }

  dtostring = doubletochar (text->rect_width);
  newXMLProp ("Width", dtostring);

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
  newXMLcProp ("ObjectName", "AcDbTable");

  buf = spointprepare (table->ins_pt.x, table->ins_pt.y, table->ins_pt.z);
  newXMLProp ("InsertionPoint", buf);

  dtostring = doubletochar (table->num_rows);
  newXMLProp ("Rows", dtostring);

  //@TODO: Lots of attributes are here. Need to figure out them and match them

  common_entity_attrs (node, obj);
  xmlAddChild (rootnode, node);
}

// Main function
int
xml_dwg (char *dwgfilename, xmlNodePtr rootnode)
{
  BITCODE_BL i;
  int error;
  Dwg_Data dwg;

  dwg.num_objects = 0;

  // Read the DWG file
  dwg.opts = 0; // silently
  error = dwg_read_file (dwgfilename, &dwg);
  if (error >= DWG_ERR_CRITICAL)
    {
      dwg_free (&dwg);
      return error;
    }

  // Emit some entities/objects to the XML file
  for (i = 0; i < dwg.num_objects; i++)
    {
      const Dwg_Object *obj = &dwg.object[i];
      int type = (int)obj->fixedtype;
      switch (type)
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
          // add_block (rootnode, obj);
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

        case DWG_TYPE_HELIX:
          add_helix (rootnode, obj);
          break;

        case DWG_TYPE_TEXT:
          add_text (rootnode, obj);
          break;

        case DWG_TYPE_MTEXT:
          add_text (rootnode, obj);
          break;

        case DWG_TYPE_SPLINE:
          add_spline (rootnode, obj);
          break;

        case DWG_TYPE_XLINE:
          add_xline (rootnode, obj);
          break;

          /*case DWG_TYPE_TABLE:
            add_table(rootnode, obj);
            break;*/

        default:
          if (obj->type < 500 || (obj->type - 500) >= dwg.num_classes)
            break;
          if (!obj->dxfname)
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
      printf ("Cannot get handle to root node. Please check\n");
      return 1;
    }

  // Process the DWG file
  error = xml_dwg (argv[1], root);
  if (error >= DWG_ERR_CRITICAL)
    {
      xmlFreeDoc (doc);
      xmlCleanupParser ();
      return error;
    }

  // Save the XML and cleanup
  xmlSaveFormatFileEnc (argv[2], doc, "UTF-8", 1);
  xmlFreeDoc (doc);
  xmlCleanupParser ();

  return 0;
}
