#include <string.h>
#include <stdlib.h>
#include "dwg.h"
#include "dwg_api.h"
#include "suffix.c"
#include <libxml/tree.h>
#include <libxml/parser.h>
#include "common.c"

int load_dwg (char *dwgfilename, xmlNodePtr rootnode);
void common_attr (xmlNodePtr node, Dwg_Object dwgobject);
void add_line (xmlNodePtr rootnode, Dwg_Object dwgobject);
void add_circle (xmlNodePtr rootnode, Dwg_Object dwgobject);
void add_3dpolyline (xmlNodePtr rootnode, Dwg_Object dwgobject);
void add_arc (xmlNodePtr rootnode, Dwg_Object dwgobject);
void add_block (xmlNodePtr rootnode, Dwg_Object dwgobject);
void add_ellipse (xmlNodePtr rootnode, Dwg_Object dwgobject);
void add_mline (xmlNodePtr rootnode, Dwg_Object dwgobject);
void add_point (xmlNodePtr rootnode, Dwg_Object dwgobject);
void add_ray (xmlNodePtr rootnode, Dwg_Object dwgobject);
void add_spline (xmlNodePtr rootnode, Dwg_Object dwgobject);
void add_text (xmlNodePtr rootnode, Dwg_Object dwgobject);
void add_table (xmlNodePtr rootnode, Dwg_Object dwgobject);


/*
 * This functions creates all the common attributes that are common to every object
 * @params xmlNodePtr node The pointer to the node to whose attribute is to be added
 * @params Dwg_Object dwgobject The DWG object
 * @return none
 */
void
common_attr (xmlNodePtr node, Dwg_Object dwgobject)
{
  const xmlChar *pointprepare;

  //Start adding the common attributes

  pointprepare = doubletohex (dwgobject.handle.value);
  xmlNewProp (node, (const xmlChar *)"Handle", pointprepare);

  pointprepare = doubletochar (dwgobject.tio.entity->linetype_scale);
  xmlNewProp (node, (const xmlChar *)"LinetypeScale", pointprepare);

  pointprepare = doubletochar (dwgobject.tio.entity->lineweight);
  xmlNewProp (node, (const xmlChar *)"Lineweight", pointprepare);


}

/* 
 * This function is used to emit all line attributes
 * @param xmlNodePtr rootnode The root node of XML document
 * @param Dwg_Object dwgobject The DWG Object
 */
void
add_line (xmlNodePtr rootnode, Dwg_Object dwgobject)
{
  Dwg_Entity_LINE *line = dwgobject.tio.entity->tio.LINE;

  //Some Fixed Variables
  const xmlChar *type = (const xmlChar *)"IAcadLine";
  const xmlChar *desc = (const xmlChar *)"IAcadLine: AutoCAD Line Interface";
  xmlChar *pointprepare;

  //The start of the entity
  xmlNodePtr dwgentity = xmlNewChild (rootnode, NULL, (const xmlChar *)"DwgEntity", NULL);

  //Now the attributes
  xmlNewProp (dwgentity, (const xmlChar *)"type", type);
  xmlNewProp (dwgentity, (const xmlChar *)"desc", desc);

  pointprepare = spointprepare (line->end.x, line->end.y, line->end.z);
  xmlNewProp (dwgentity, (const xmlChar *)"EndPoint", pointprepare);

  pointprepare = spointprepare (line->start.x, line->start.y, line->start.z);
  xmlNewProp (dwgentity, (const xmlChar *)"StartPoint", pointprepare);

  //Add the default attributes common to all

  //Now link the created node to root
  xmlAddChild (rootnode, dwgentity);

  //Free Dynamically allocated memory
  free (pointprepare);

}

/*
 * This function is emits all circle attributes in the XML file
 * @params xmlNodePtr rootnode The root node of the XML Document
 * @param Dwg_Object dwgobject The DWG Object
 */
void
add_circle (xmlNodePtr rootnode, Dwg_Object dwgobject)
{
  Dwg_Entity_CIRCLE *circle = dwgobject.tio.entity->tio.CIRCLE;

  //Some fixed Variables
  const xmlChar *type = (const xmlChar *)"IAcadCircle";
  const xmlChar *desc = (const xmlChar *)"IAcadCircle: AutoCAD Circle Interface";
  xmlChar *pointprepare, *dtostring;
  //the start of the entity
  xmlNodePtr dwgentity = xmlNewChild (rootnode, NULL, (const xmlChar *)"DwgEntity", NULL);

  //Now the attributes
  xmlNewProp (dwgentity, (const xmlChar *)"type", type);
  xmlNewProp (dwgentity, (const xmlChar *)"desc", desc);

  pointprepare =
    spointprepare (circle->center.x, circle->center.y, circle->center.z);
  xmlNewProp (dwgentity, (const xmlChar *)"Center", pointprepare);

  dtostring = doubletochar (circle->radius);
  xmlNewProp (dwgentity, (const xmlChar *)"Radius", dtostring);

  dtostring = doubletochar (circle->thickness);
  xmlNewProp (dwgentity, (const xmlChar *)"Thickness", dtostring);

  //Add the default attributes common to all

  //Now link the created node to root
  xmlAddChild (rootnode, dwgentity);
  free (pointprepare);

}


//@TODO This is yet to be completed
/*
 * This function emits all 3D Polyline related attributes in XML
 * @params xmlNodePtr rootnode The root node of the XML document
 * @params Dwg_Object The DWG Object
 */
void
add_3dpolyline (xmlNodePtr rootnode, Dwg_Object dwgobject)
{
  Dwg_Entity_POLYLINE_3D *polyline3d = dwgobject.tio.entity->tio.POLYLINE_3D;

  //Some fixed Variables
  const xmlChar *type = (const xmlChar *)"IAcad3DPolyline";
  const xmlChar *desc = (const xmlChar *)"IAcad3DPolyline: AutoCAD 3dPolyline Interface";

  //the start of the entity
  xmlNodePtr dwgentity = xmlNewChild (rootnode, NULL, (const xmlChar *)"DwgEntity", NULL);

  //Now the attributes
  xmlNewProp (dwgentity, (const xmlChar *)"type", type);
  xmlNewProp (dwgentity, (const xmlChar *)"desc", desc);

  //Add the default attributes common to all

  //Now link the created node to root
  xmlAddChild (rootnode, dwgentity);
}

/*
 * This function emits all arc related attributes
 * @params xmlNodePtr rootnode The root node of the XML Document
 * @params Dwg_Object dwgobject The DWG Object
 */
void
add_arc (xmlNodePtr rootnode, Dwg_Object dwgobject)
{
  Dwg_Entity_ARC *arc = dwgobject.tio.entity->tio.ARC;
  //Some fixed Variables
  const xmlChar *type = (const xmlChar *)"IAcadArc";
  const xmlChar *desc = (const xmlChar *)"IAcadArc: AutoCAD Arc Interface";
  xmlChar *pointprepare, *dtostring;

  //the start of the entity
  xmlNodePtr dwgentity = xmlNewChild (rootnode, NULL, (const xmlChar *)"DwgEntity", NULL);

  //Now the attributes
  xmlNewProp (dwgentity, (const xmlChar *)"type", type);
  xmlNewProp (dwgentity, (const xmlChar *)"desc", desc);

  pointprepare = spointprepare (arc->center.x, arc->center.y, arc->center.z);
  xmlNewProp (dwgentity, (const xmlChar *)"Center", pointprepare);

  dtostring = doubletochar (arc->end_angle);
  xmlNewProp (dwgentity, (const xmlChar *)"EndAngle", dtostring);

  dtostring = doubletochar (arc->radius);
  xmlNewProp (dwgentity, (const xmlChar *)"Radius", dtostring);

  dtostring = doubletochar (arc->start_angle);
  xmlNewProp (dwgentity, (const xmlChar *)"StartAngle", dtostring);

  dtostring = doubletochar (arc->thickness);
  xmlNewProp (dwgentity, (const xmlChar *)"Thickness", dtostring);
  //Add the default attributes common to all

  //Now link the created node to root
  xmlAddChild (rootnode, dwgentity);
  free (pointprepare);
  free (dtostring);
}

/*
 * This functions emits all block related attributes in the XML
 * @param xmlNodePtr rootnode The root node of the XML document
 * @param Dwg_Object dwgobject The DWG Object
 */
void
add_block (xmlNodePtr rootnode, Dwg_Object dwgobject)
{
  Dwg_Entity_BLOCK *block = dwgobject.tio.entity->tio.BLOCK;
  //Some fixed Variables
  const xmlChar *type = (const xmlChar *)"IAcadBlockReference";
  const xmlChar *desc = (const xmlChar *)"IAcadBlockReference: AutoCAD Block Reference Interface";

  //the start of the entity
  xmlNodePtr dwgentity = xmlNewChild (rootnode, NULL, (const xmlChar *)"DwgEntity", NULL);

  //Now the attributes
  xmlNewProp (dwgentity, (const xmlChar *)"type", type);
  xmlNewProp (dwgentity, (const xmlChar *)"desc", desc);

  xmlNewProp (dwgentity, (const xmlChar *)"EffectiveName", (xmlChar*)block->name);

  //Add the default attributes common to all

  //Now link the created node to root
  xmlAddChild (rootnode, dwgentity);
}

/*
 * This function emits all the ellipse related attributes
 * @param xmlNodePtr rootnode The root node of the XML document
 * @param Dwg_Object The DWG Object
 */
void
add_ellipse (xmlNodePtr rootnode, Dwg_Object dwgobject)
{
  Dwg_Entity_ELLIPSE *ellipse = dwgobject.tio.entity->tio.ELLIPSE;

  //Some fixed Variables
  const xmlChar *type = (const xmlChar *)"IAcadEllipse";
  const xmlChar *desc = (const xmlChar *)"IAcadEllipse: AutoCAD Ellipse Interface";
  xmlChar *pointprepare, *dtostring;
  //the start of the entity
  xmlNodePtr dwgentity = xmlNewChild (rootnode, NULL, (const xmlChar *)"DwgEntity", NULL);

  //Now the attributes
  xmlNewProp (dwgentity, (const xmlChar *)"type", type);
  xmlNewProp (dwgentity, (const xmlChar *)"desc", desc);

  pointprepare =
    spointprepare (ellipse->center.x, ellipse->center.y, ellipse->center.z);
  xmlNewProp (dwgentity, (const xmlChar *)"Center", pointprepare);

  dtostring = doubletochar (ellipse->end_angle);
  xmlNewProp (dwgentity, (const xmlChar *)"EndAngle", dtostring);

  dtostring = doubletochar (ellipse->start_angle);
  xmlNewProp (dwgentity, (const xmlChar *)"StartAngle", dtostring);
  //Axis Ratio and Sm Axis


  //Add the default attributes common to all

  //Now link the created node to root
  xmlAddChild (rootnode, dwgentity);
  free (pointprepare);
  free (dtostring);
}

// @TODO: This is not proper. Still some attributes are missing
/*
 * This function emits all the MLINE related attributes
 * @param xmlNodePtr rootnode The root node of the XML document
 * @param Dwg_Object dwgobject The DWG Object
 */
void
add_mline (xmlNodePtr rootnode, Dwg_Object dwgobject)
{
  Dwg_Entity_MLINE *mline = dwgobject.tio.entity->tio.MLINE;

  //Some fixed Variables
  const xmlChar *type = (const xmlChar *)"IAcadMLine";
  const xmlChar *desc = (const xmlChar *)"IAcadMLine: IAcadMLine Interface";

  //the start of the entity
  xmlNodePtr dwgentity = xmlNewChild (rootnode, NULL, (const xmlChar *)"DwgEntity", NULL);

  //Now the attributes
  xmlNewProp (dwgentity, (const xmlChar *)"type", type);
  xmlNewProp (dwgentity, (const xmlChar *)"desc", desc);


  //Add the default attributes common to all

  //Now link the created node to root
  xmlAddChild (rootnode, dwgentity);
}

/*
 * This function emits all the point related attributes
 * @param xmlNodePtr rootnode The root node of the XML document
 * @param Dwg_Object dwgobject The DWG Object
 */
void
add_point (xmlNodePtr rootnode, Dwg_Object dwgobject)
{
  Dwg_Entity_POINT *point = dwgobject.tio.entity->tio.POINT;

  //Some fixed Variables
  const xmlChar *type = (const xmlChar *)"IAcadPoint";
  const xmlChar *desc = (const xmlChar *)"IAcadPoint: AutoCAD Point Interface";
  xmlChar *pointprepare, *dtostring;
  //the start of the entity
  xmlNodePtr dwgentity = xmlNewChild (rootnode, NULL, (const xmlChar *)"DwgEntity", NULL);

  //Now the attributes
  xmlNewProp (dwgentity, (const xmlChar *)"type", type);
  xmlNewProp (dwgentity, (const xmlChar *)"desc", desc);

  pointprepare = spointprepare (point->x, point->y, point->z);
  xmlNewProp (dwgentity, (const xmlChar *)"Coordinates", pointprepare);

  dtostring = doubletochar (point->thickness);
  xmlNewProp (dwgentity, (const xmlChar *)"Thickness", dtostring);
  //Add the default attributes common to all

  //Now link the created node to root
  xmlAddChild (rootnode, dwgentity);
  free (pointprepare);
  free (dtostring);
}

/*
 * This function emits all the ray related attributes in the XML
 * @param xmlNodePtr rootnode The root node of the XML document
 * @param Dwg_Object The DWG Object
 */
void
add_ray (xmlNodePtr rootnode, Dwg_Object dwgobject)
{
  Dwg_Entity_RAY *ray = dwgobject.tio.entity->tio.RAY;

  //Some fixed Variables
  const xmlChar *type = (const xmlChar *)"IAcadRay";
  const xmlChar *desc = (const xmlChar *)"IAcadRay: AutoCAD Ray Interface";
  xmlChar *pointprepare;

  //the start of the entity
  xmlNodePtr dwgentity = xmlNewChild (rootnode, NULL, (const xmlChar *)"DwgEntity", NULL);

  //Now the attributes
  xmlNewProp (dwgentity, (const xmlChar *)"type", type);
  xmlNewProp (dwgentity, (const xmlChar *)"desc", desc);

  pointprepare = spointprepare (ray->point.x, ray->point.y, ray->point.z);
  xmlNewProp (dwgentity, (const xmlChar *)"BasePoint", pointprepare);

  pointprepare = spointprepare (ray->vector.x, ray->vector.y, ray->vector.z);
  xmlNewProp (dwgentity, (const xmlChar *)"DirectionVector", pointprepare);

//  Add the default attributes common to all

  //Now link the created node to root
  xmlAddChild (rootnode, dwgentity);
  free (pointprepare);
}

/*
 * This function emits all the spline related attributes
 * @param xmlNodePtr rootnode The rootnode of the XML Document
 * @param Dwg_Object dwgobject The DWG Object
 */
void
add_spline (xmlNodePtr rootnode, Dwg_Object dwgobject)
{
  Dwg_Entity_SPLINE *spline = dwgobject.tio.entity->tio.SPLINE;

  //Some fixed Variables
  const xmlChar *type = (const xmlChar *)"IAcadSpline";
  const xmlChar *desc = (const xmlChar *)"IAcadSpline: AutoCAD Spline Interface";
  xmlChar *pointprepare, *dtostring;
  //the start of the entity
  xmlNodePtr dwgentity = xmlNewChild (rootnode, NULL, (const xmlChar *)"DwgEntity", NULL);

  //Now the attributes
  xmlNewProp (dwgentity, (const xmlChar *)"type", type);
  xmlNewProp (dwgentity, (const xmlChar *)"desc", desc);

  dtostring = doubletochar (spline->closed_b);
  xmlNewProp (dwgentity, (const xmlChar *)"Closed", dtostring);

  dtostring = doubletochar (spline->degree);
  xmlNewProp (dwgentity, (const xmlChar *)"Degree", dtostring);

  pointprepare =
    spointprepare (spline->end_tan_vec.x, spline->end_tan_vec.y,
		   spline->end_tan_vec.z);
  xmlNewProp (dwgentity, (const xmlChar *)"EndTangent", pointprepare);

  dtostring = doubletochar (spline->fit_tol);
  xmlNewProp (dwgentity, (const xmlChar *)"FitTolerance", dtostring);

  //This is for fit points. DO this too

  // @TODO this is an array. Use it properly
  //fprintf(file, (const xmlChar *)"ControlPoints='(%f %f %f %f)' ", );
//  Add the default attributes common to all

  //Now link the created node to root
  xmlAddChild (rootnode, dwgentity);
  free (dtostring);
  free (pointprepare);

}

/*
 * This function emits all the text related attributes
 * @param xmlNodePtr rootnode The rootnode of the XML Document
 * @param Dwg_Object The DWG Object
 */
void
add_text (xmlNodePtr rootnode, Dwg_Object dwgobject)
{
  Dwg_Entity_TEXT *text = dwgobject.tio.entity->tio.TEXT;

  //Some fixed Variables
  const xmlChar *type = (const xmlChar *)"IAcadMText";
  const xmlChar *desc = (const xmlChar *)"IAcadMText: AutoCAD MText Interface";
  xmlChar *pointprepare, *dtostring;
  //the start of the entity
  xmlNodePtr dwgentity = xmlNewChild (rootnode, NULL, (const xmlChar *)"DwgEntity", NULL);

  //Now the attributes
  xmlNewProp (dwgentity, (const xmlChar *)"type", type);
  xmlNewProp (dwgentity, (const xmlChar *)"desc", desc);

  dtostring = doubletochar (text->height);
  xmlNewProp (dwgentity, (const xmlChar *)"Height", dtostring);

  pointprepare = spointprepare2 (text->insertion_pt.x, text->insertion_pt.y);
  xmlNewProp (dwgentity, (const xmlChar *)"InsertionPoint", pointprepare);
  xmlNewProp (dwgentity, (const xmlChar *)"TextString", (xmlChar*)text->text_value);

  dtostring = doubletochar (text->width_factor);
  xmlNewProp (dwgentity, (const xmlChar *)"Width", dtostring);

  //@TODO: Lots of attributes were also left. Check this.
  //Add the default attributes common to all

  //Now link the created node to root
  xmlAddChild (rootnode, dwgentity);
  free (pointprepare);
  free (dtostring);
}

/*
 * This functions emits all the table related attribues in the XML
 * @param xmlNodePtr rootnode The root node of the XML document
 * @param Dwg_Object dwgobject The DWG Object
 */
void
add_table (xmlNodePtr rootnode, Dwg_Object dwgobject)
{
  Dwg_Entity_TABLE *table = dwgobject.tio.entity->tio.TABLE;

  const xmlChar *type = (const xmlChar *)"IAcadTable";
  const xmlChar *desc = (const xmlChar *)"IAcadTable: IAcadTable Interface";
  xmlChar *pointprepare, *dtostring;

  //the start of the entity
  xmlNodePtr dwgentity = xmlNewChild (rootnode, NULL, (const xmlChar *)"DwgEntity", NULL);

  //Now the attributes
  xmlNewProp (dwgentity, (const xmlChar *)"type", type);
  xmlNewProp (dwgentity, (const xmlChar *)"desc", desc);

  pointprepare =
    spointprepare (table->insertion_point.x, table->insertion_point.y,
		   table->insertion_point.z);
  xmlNewProp (dwgentity, (const xmlChar *)"InsertionPoint", pointprepare);

  dtostring = doubletochar (table->num_rows);
  xmlNewProp (dwgentity, (const xmlChar *)"Rows", dtostring);

  //@TODO: Lots of attributes are here. Need to figure out them and match them
  //Add the default attributes common to all

  //Now link the created node to root
  xmlAddChild (rootnode, dwgentity);
  free (pointprepare);
  free (dtostring);
}

// Main function
int
load_dwg (char *dwgfilename, xmlNodePtr rootnode)
{
  unsigned int i;
  int error;
  Dwg_Data dwg;

  dwg.num_objects = 0;

  //Read the DWG file
  error = dwg_read_file (dwgfilename, &dwg);
  if (error)
    return error;

  //Emit all the objects to the XML file
  for (i = 0; i < dwg.num_objects; i++)
    {
      switch (dwg.object[i].type)
	{
	case DWG_TYPE_ARC:
	  add_arc (rootnode, dwg.object[i]);
	  break;

	case DWG_TYPE_LINE:
	  add_line (rootnode, dwg.object[i]);
	  break;

	case DWG_TYPE_CIRCLE:
	  add_circle (rootnode, dwg.object[i]);
	  break;

	case DWG_TYPE_POLYLINE_3D:
	  add_3dpolyline (rootnode, dwg.object[i]);
	  break;

     /* case DWG_TYPE_BLOCK:
          add_block(rootnode, dwg.object[i]);
          break; */

	case DWG_TYPE_MLINE:
	  add_mline (rootnode, dwg.object[i]);
	  break;

	case DWG_TYPE_ELLIPSE:
	  add_ellipse (rootnode, dwg.object[i]);
	  break;

	case DWG_TYPE_POINT:
	  add_point (rootnode, dwg.object[i]);
	  break;

	case DWG_TYPE_RAY:
	  add_ray (rootnode, dwg.object[i]);
	  break;

	case DWG_TYPE_TEXT:
	  add_text (rootnode, dwg.object[i]);
	  break;

	case DWG_TYPE_SPLINE:
	  add_spline (rootnode, dwg.object[i]);
	  break;

/*      case DWG_TYPE_TABLE:
          add_table(rootnode, dwg.object[i]);
          break;*/

        default:
          break;
	}
    }

  //free the dwg
  dwg_free (&dwg);
  return error;
}

int
main (int argc, char *argv[])
{
  //Basic Root Entity
  char * xml = (char *)"<DwgData></DwgData>";
  xmlDocPtr doc;
  xmlNodePtr root;
  int error;

  //Check if we have the filename
  REQUIRE_INPUT_FILE_ARG (argc);

  //Load the XML interface
  LIBXML_TEST_VERSION

  doc = xmlParseMemory (xml, xmlStrlen ((xmlChar*)xml));
  //Check if it was able to read it
  if (doc == NULL)
    {
      printf ("xmlDoc Creation Failed. Please Check;\n");
      return 1;
    }

  //We have xml loaded. Now get handle to root node
  root = xmlDocGetRootElement (doc);

  if (root == NULL)
    {
      printf ("Cannot get handle to root node. Please Check\n");
      return 1;
    }

  //Load the DWG file
  error = load_dwg (argv[1], root);
  if (error) {
    xmlFreeDoc (doc);
    xmlCleanupParser ();
    return error;
  }

  //Save the XML and Cleanup
  xmlSaveFormatFileEnc (argv[2], doc, "UTF-8", 1);
  xmlFreeDoc (doc);
  xmlCleanupParser ();

  //This would depend if the program is able to
  //read the dwg file
  return 0;
}
