/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2009 Free Software Foundation, Inc.                        */
/*  Copyright (C) 2010 Thien-Thi Nguyen                                      */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * load_dwg.c: load a DWG, get lines, text and circles
 * written by Felipe Castro
 * modified by Felipe Corrêa da Silva Sances
 * modified by Thien-Thi Nguyen
 */

#include <dwg.h>
#include <dwg_api.h>
#include "suffix.c"
#include "common.c"

void
add_line(FILE *file, Dwg_Object dwgobject)
{
	Dwg_Entity_LINE *line = dwgobject.tio.entity->tio.LINE;
	//Some Fixed Variables
	const char *type = "IAcadLine";
	const char *desc = "IAcadLine: AutoCAD Line Interface";

	//The start of the entity
  entity_header(file);
  fprintf(file, "type='%s' desc='%s' EndPoint='(%f %f %f)'",
   type, desc, line->end.x, line->end.y, line->end.z);

  fprintf(file, "StartPoint='(%f %f %f)' Handle='%lu'",
   line->start.x, line->start.y, line->start.z, dwgobject.handle.value);
  //The end of the entity

  entity_footer(file);
}

void
add_circle(FILE *file, Dwg_Object dwgobject)
{
	Dwg_Entity_CIRCLE *circle = dwgobject.tio.entity->tio.CIRCLE;

	//Some fixed Variables
	const char *type = "IAcadCircle";
	const char *desc = "IAcadCircle: AutoCAD Circle Interface";

	//the start of the entity
	entity_header(file);
	fprintf(file, "type='%s' desc='%s' ", type, desc);
	fprintf(file, "Center='(%f %f %f)' ", circle->center.x, circle->center.y, circle->center.z);
	fprintf(file, "Handle='%lu' ", dwgobject.handle.value);
	fprintf(file, "Radius='%f' ", circle->radius);
	fprintf(file, "Thickness='%f' ", circle->thickness);

	entity_footer(file);

}


//@TODO This is yet to be completed
void
add_3dpolyline(FILE *file, Dwg_Object dwgobject)
{
	Dwg_Entity_POLYLINE_3D *polyline3d = dwgobject.tio.entity->tio.POLYLINE_3D;

	//Some fixed Variables
	const char *type = "IAcad3DPolyline";
	const char *desc = "IAcad3DPolyline: AutoCAD 3dPolyline Interface";

	//the start of the entity
	entity_header(file);
	fprintf(file, "type='%s' desc='%s' ", type, desc);
	fprintf(file, "\n");
}

void
add_arc(FILE *file, Dwg_Object dwgobject)
{
	Dwg_Entity_ARC *arc = dwgobject.tio.entity->tio.ARC;
	//Some fixed Variables
	const char *type = "IAcadArc";
	const char *desc = "IAcadArc: AutoCAD Arc Interface";

	//the start of the entity
	entity_header(file);
	fprintf(file, "type='%s' desc='%s' ", type, desc);
	fprintf(file, "Center='(%f %f %f)' ", arc->center.x, arc->center.y, arc->center.z);
	fprintf(file, "EndAngle='%f' ", arc->end_angle);
	fprintf(file, "Handle='%lu' ", dwgobject.handle.value);
	fprintf(file, "Radius='%f' ", arc->radius);
	fprintf(file, "StartAngle='%f'", arc->start_angle);
	fprintf(file, "Thickness='%f' ", arc->thickness);
	entity_footer(file);
}

void
add_block(FILE *file, Dwg_Object dwgobject)
{
	Dwg_Entity_BLOCK *block = dwgobject.tio.entity->tio.BLOCK;
	//Some fixed Variables
	const char *type = "IAcadBlockReference";
	const char *desc = "IAcadBlockReference: AutoCAD Block Reference Interface";

	//the start of the entity
	entity_header(file);
	fprintf(file, "type='%s' desc='%s' ", type, desc);
	fprintf(file, "EffectiveName='%s' ", block->name);
	entity_footer(file);
}

void
add_ellipse(FILE *file, Dwg_Object dwgobject)
{
	Dwg_Entity_ELLIPSE *ellipse = dwgobject.tio.entity->tio.ELLIPSE;

	//Some fixed Variables
	const char *type = "IAcadEllipse";
	const char *desc = "IAcadEllipse: AutoCAD Ellipse Interface";

	//the start of the entity
	entity_header(file);
	fprintf(file, "type='%s' desc='%s' ", type, desc);
	fprintf(file, "Center='(%f %f %f)' ", ellipse->center.x, ellipse->center.y, ellipse->center.z);
	fprintf(file, "EndAngle='%f' ", ellipse->end_angle);
	fprintf(file, "Handle='%lu' ", dwgobject.handle.value);
	fprintf(file, "StartAngle='%f' ", ellipse->start_angle);
	//Axis Ratio and Sm Axis
	entity_footer(file);
}

// @TODO: This is not proper. Still some attributes are missing
void
add_mline(FILE *file, Dwg_Object dwgobject)
{
	Dwg_Entity_MLINE *mline = dwgobject.tio.entity->tio.MLINE;

	//Some fixed Variables
	const char *type = "IAcadMLine";
	const char *desc = "IAcadMLine: IAcadMLine Interface";

	//the start of the entity
	entity_header(file);
	fprintf(file, "type='%s' desc='%s' ", type, desc);
	fprintf(file, "Handle=''%lu", dwgobject.handle.value);
	entity_footer(file)
}

void
add_point(FILE* file, Dwg_Object dwgobject)
{
	Dwg_Entity_POINT *point = dwgobject.tio.entity->tio.POINT;

	//Some fixed Variables
	const char *type = "IAcadPoint";
	const char *desc = "IAcadPoint: AutoCAD Point Interface";

	//the start of the entity
	entity_header(file);
	fprintf(file, "type='%s' desc='%s' ", type, desc);
	fprintf(file, "Handle='%lu' ", dwgobject.handle.value);
	fprintf(file, "Coordinates='(%f %f %f)'", point->x, point->y, point->z);
	fprintf(file, "Thickness='%f'", point->thickness);
	entity_footer(file)

}

void
add_ray(FILE *file, Dwg_Object dwgobject)
{
	Dwg_Entity_RAY *ray = dwgobject.tio.entity->tio.RAY;

	//Some fixed Variables
	const char *type = "IAcadRay";
	const char *desc = "IAcadRay: AutoCAD Ray Interface";

	//the start of the entity
	entity_header(file);
	fprintf(file, "type='%s' desc='%s' ", type, desc);
	fprintf(file, "Handle='%lu' ", dwgobject.handle.value);
	fprintf(file, "BasePoint='(%f %f %f)' ", ray->point.x, ray->point.y, ray->point.z);
	fprintf(file, "DirectionVector='(%f %f %f)' ", ray->vector.x, ray->vector.y, ray->vector.z);
	entity_footer(file);

}


void
add_spline(FILE *file, Dwg_Object dwgobject)
{
	Dwg_Entity_SPLINE *spline = dwgobject.tio.entity->tio.SPLINE;

	//Some fixed Variables
	const char *type = "IAcadSpline";
	const char *desc = "IAcadSpline: AutoCAD Spline Interface";

	//the start of the entity
	entity_header(file);
	fprintf(file, "type='%s' desc='%s' ", type, desc);
	fprintf(file, "Handle='%lu' ", dwgobject.handle.value);
	fprintf(file, "Closed='%f' ", spline->closed_b);
	fprintf(file, "Degree='%u' ", spline->degree);
	fprintf(file, "EndTangent='(%f %f %f)' ", spline->end_tan_vec.x, spline->end_tan_vec.y, spline->end_tan_vec.z);
	fprintf(file, "FitTolerance='%f'", spline->fit_tol);
	
	//This is for fit points. DO this too
	fprintf(file, "");

	// @TODO this is an array. Use it properly
	//fprintf(file, "ControlPoints='(%f %f %f %f)' ", );
	entity_footer(file);

}
int
load_dwg(char *filename)
{
  unsigned int i;
  int success;
  Dwg_Data dwg;

  dwg.num_objects = 0;
  success = dwg_read_file(filename, &dwg);
  const char *xmlfilename = "Line.xml";
  FILE *xmlfile;
  xmlfile = fopen(xmlfilename, "a+");
  xml_header(xmlfile);

  for (i = 0; i < dwg.num_objects; i++)
    {
      switch (dwg.object[i].type)
        {
      		case DWG_TYPE_LINE:
        		//What to do when we have a line
	        	add_line(xmlfile, dwg.object[i]);
        	break;

        	case DWG_TYPE_CIRCLE:
        	//What to do when we get a cicle
        	add_circle(xmlfile, dwg.object[i]);
        	break;

        	case DWG_TYPE_POLYLINE_3D:
        	//What to do when we get a 3D Polyline
        	add_3dpolyline(xmlfile, dwg.object[i]);
        	break;

        	case DWG_TYPE_BLOCK:
        	//What to do when we get a block
        	add_block(xmlfile, dwg.object[i]);
        	break;

        	case DWG_TYPE_MLINE:
        	//What to do when we get a mline
        	add_mline(xmlfile, dwg.object[i]);
        	break;

        	case DWG_TYPE_ELLIPSE:
        	//What to do when we get a ellipse
        	add_ellipse(xmlfile, dwg.object[i]);
        	break;

        	case DWG_TYPE_POINT:
        	//What to do when get a point
        	add_line(xmlfile, dwg.object[i]);
        	break;

        	case DWG_TYPE_RAY:
        	//What to do when we get a ray
        	add_ray(xmlfile, dwg.object[i]);
        	break;

        	case DWG_TYPE_SPLINE:
        	//What to do when we get a spline
        	add_spline(xmlfile, dwg.object[i]);
      }
    }

  //Close the file and free the dwg
  fclose(xmlfile);
  dwg_free(&dwg);
  return success;
}

int
main (int argc, char *argv[])
{
	//Check if we have the filename
  REQUIRE_INPUT_FILE_ARG (argc);

  //Load the DWG file
  load_dwg (argv[1]);
  return 0;
}
