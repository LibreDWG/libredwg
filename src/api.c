#include "api.h"
#include "dwg.h"
#include <malloc.h>

/*******************************************************************
*        Functions created from macro to extract entities           *
********************************************************************/

/// Macro call to extract all text entities from a block
GET_DWG_ENTITY(TEXT);
/// Macro call to extract all attrib entities from a block
GET_DWG_ENTITY(ATTRIB);
/// Macro call to extract all attdef entities from a block
GET_DWG_ENTITY(ATTDEF);
/// Macro call to extract all block entities from a block
GET_DWG_ENTITY(BLOCK);
/// Macro call to extract endblk entity from a block
GET_DWG_ENTITY(ENDBLK);
/// Macro call to extract all seqend entities from a block
GET_DWG_ENTITY(SEQEND);
/// Macro call to extract all insert entities from a block
GET_DWG_ENTITY(INSERT);
/// Macro call to extract all minsert entities from a block
GET_DWG_ENTITY(MINSERT);
/// Macro call to extract all vertex_2d entities from a block
GET_DWG_ENTITY(VERTEX_2D);
/// Macro call to extract all vertex_3d entities from a block
GET_DWG_ENTITY(VERTEX_3D);
/// Macro call to extract all vertex_mesh entities from a block
GET_DWG_ENTITY(VERTEX_MESH);
/// Macro call to extract all vertex_pface entities from a block
GET_DWG_ENTITY(VERTEX_PFACE);
/// Macro call to extract all vertex_pface_face entities from a block
GET_DWG_ENTITY(VERTEX_PFACE_FACE);
/// Macro call to extract all polyline_2d entities from a block
GET_DWG_ENTITY(POLYLINE_2D);
/// Macro call to extract all polyline_3d entities from a block
GET_DWG_ENTITY(POLYLINE_3D);
/// Macro call to extract all arc entities from a block
GET_DWG_ENTITY(ARC);
/// Macro call to extract all circle entities from a block
GET_DWG_ENTITY(CIRCLE);
/// Macro call to extract all line entities from a block
GET_DWG_ENTITY(LINE);
/// Macro call to extract all dimension ordinate entities from a block
GET_DWG_ENTITY(DIMENSION_ORDINATE);
/// Macro call to extract all dimension linear entities from a block
GET_DWG_ENTITY(DIMENSION_LINEAR);
/// Macro call to extract all dimension aligned entities from a block
GET_DWG_ENTITY(DIMENSION_ALIGNED);
/// Macro call to extract all dimension ang3pt entities from a block
GET_DWG_ENTITY(DIMENSION_ANG3PT);
/// Macro call to extract all dimension ang2ln entities from a block
GET_DWG_ENTITY(DIMENSION_ANG2LN);
/// Macro call to extract all dimension radius entities from a block
GET_DWG_ENTITY(DIMENSION_RADIUS);
/// Macro call to extract all dimension diameter entities from a block
GET_DWG_ENTITY(DIMENSION_DIAMETER);
/// Macro call to extract all points entities from a block
GET_DWG_ENTITY(POINT);
/// Macro call to extract all polyline_pface entities from a block
GET_DWG_ENTITY(POLYLINE_PFACE);
/// Macro call to extract all polyline_mesh entities from a block
GET_DWG_ENTITY(POLYLINE_MESH);
/// Macro call to extract all solid entities from a block
GET_DWG_ENTITY(SOLID);
/// Macro call to extract all trace entities from a block
GET_DWG_ENTITY(TRACE);
/// Macro call to extract all shape entities from a block
GET_DWG_ENTITY(SHAPE);
/// Macro call to extract all viewport entities from a block
GET_DWG_ENTITY(VIEWPORT);
/// Macro call to extract all ellipse entities from a block
GET_DWG_ENTITY(ELLIPSE);
/// Macro call to extract all spline entities from a block
GET_DWG_ENTITY(SPLINE);
/// Macro call to extract all region entities from a block
GET_DWG_ENTITY(REGION);
/// Macro call to extract all body entities from a block
GET_DWG_ENTITY(BODY);
/// Macro call to extract all ray entities from a block
GET_DWG_ENTITY(RAY);
/// Macro call to extract all xline entities from a block
GET_DWG_ENTITY(XLINE);
/// Macro call to extract all mtext entities from a block
GET_DWG_ENTITY(MTEXT);
/// Macro call to extract all leader entities from a block
GET_DWG_ENTITY(LEADER);
/// Macro call to extract all tolerance entities from a block
GET_DWG_ENTITY(TOLERANCE);
/// Macro call to extract all mline entities from a block
GET_DWG_ENTITY(MLINE);
/// Macro call to extract all lwpline entities from a block
GET_DWG_ENTITY(LWPLINE);
/// Macro call to extract all hatch entities from a block
GET_DWG_ENTITY(HATCH);

/*******************************************************************
*     Functions created from macro to cast dwg_object to entity     *
*                 Usage :- dwg_object_to_ENTITY(),                  *
*                where ENTITY can be LINE or CIRCLE                 *
********************************************************************/

/// Macro call to cast dwg object to text
CAST_DWG_OBJECT_TO_ENTITY(TEXT);
/// Macro call to cast dwg object to attrib
CAST_DWG_OBJECT_TO_ENTITY(ATTRIB);
/// Macro call to cast dwg object to attdef
CAST_DWG_OBJECT_TO_ENTITY(ATTDEF);
/// Macro call to cast dwg object to block
CAST_DWG_OBJECT_TO_ENTITY(BLOCK);
/// Macro call to extract endblk entity from a block
CAST_DWG_OBJECT_TO_ENTITY(ENDBLK);
/// Macro call to cast dwg object to seqend
CAST_DWG_OBJECT_TO_ENTITY(SEQEND);
/// Macro call to cast dwg object to insert
CAST_DWG_OBJECT_TO_ENTITY(INSERT);
/// Macro call to cast dwg object to minsert
CAST_DWG_OBJECT_TO_ENTITY(MINSERT);
/// Macro call to cast dwg object to vertex_2d
CAST_DWG_OBJECT_TO_ENTITY(VERTEX_2D);
/// Macro call to cast dwg object to vertex_3d
CAST_DWG_OBJECT_TO_ENTITY(VERTEX_3D);
/// Macro call to cast dwg object to vertex_mesh
CAST_DWG_OBJECT_TO_ENTITY(VERTEX_MESH);
/// Macro call to cast dwg object to vertex_pface
CAST_DWG_OBJECT_TO_ENTITY(VERTEX_PFACE);
/// Macro call to cast dwg object to vertex_pface_face
CAST_DWG_OBJECT_TO_ENTITY(VERTEX_PFACE_FACE);
/// Macro call to cast dwg object to polyline_2d
CAST_DWG_OBJECT_TO_ENTITY(POLYLINE_2D);
/// Macro call to cast dwg object to polyline_3d
CAST_DWG_OBJECT_TO_ENTITY(POLYLINE_3D);
/// Macro call to cast dwg object to arc
CAST_DWG_OBJECT_TO_ENTITY(ARC);
/// Macro call to cast dwg object to circle
CAST_DWG_OBJECT_TO_ENTITY(CIRCLE);
/// Macro call to cast dwg object to line
CAST_DWG_OBJECT_TO_ENTITY(LINE);
/// Macro call to cast dwg object to dimension ordinate
CAST_DWG_OBJECT_TO_ENTITY(DIMENSION_ORDINATE);
/// Macro call to cast dwg object to dimension linear
CAST_DWG_OBJECT_TO_ENTITY(DIMENSION_LINEAR);
/// Macro call to cast dwg object to dimension aligned
CAST_DWG_OBJECT_TO_ENTITY(DIMENSION_ALIGNED);
/// Macro call to cast dwg object to dimension ang3pt
CAST_DWG_OBJECT_TO_ENTITY(DIMENSION_ANG3PT);
/// Macro call to cast dwg object to dimension ang2ln
CAST_DWG_OBJECT_TO_ENTITY(DIMENSION_ANG2LN);
/// Macro call to cast dwg object to dimension radius
CAST_DWG_OBJECT_TO_ENTITY(DIMENSION_RADIUS);
/// Macro call to cast dwg object to dimension diameter
CAST_DWG_OBJECT_TO_ENTITY(DIMENSION_DIAMETER);
/// Macro call to cast dwg object to points
CAST_DWG_OBJECT_TO_ENTITY(POINT);
/// Macro call to cast dwg object to polyline_pface
CAST_DWG_OBJECT_TO_ENTITY(POLYLINE_PFACE);
/// Macro call to cast dwg object to polyline_mesh
CAST_DWG_OBJECT_TO_ENTITY(POLYLINE_MESH);
/// Macro call to cast dwg object to solid
CAST_DWG_OBJECT_TO_ENTITY(SOLID);
/// Macro call to cast dwg object to trace
CAST_DWG_OBJECT_TO_ENTITY(TRACE);
/// Macro call to cast dwg object to shape
CAST_DWG_OBJECT_TO_ENTITY(SHAPE);
/// Macro call to cast dwg object to viewport
CAST_DWG_OBJECT_TO_ENTITY(VIEWPORT);
/// Macro call to cast dwg object to ellipse
CAST_DWG_OBJECT_TO_ENTITY(ELLIPSE);
/// Macro call to cast dwg object to spline
CAST_DWG_OBJECT_TO_ENTITY(SPLINE);
/// Macro call to cast dwg object to region
CAST_DWG_OBJECT_TO_ENTITY(REGION);
/// Macro call to cast dwg object to body
CAST_DWG_OBJECT_TO_ENTITY(BODY);
/// Macro call to cast dwg object to ray
CAST_DWG_OBJECT_TO_ENTITY(RAY);
/// Macro call to cast dwg object to xline
CAST_DWG_OBJECT_TO_ENTITY(XLINE);
/// Macro call to cast dwg object to mtext
CAST_DWG_OBJECT_TO_ENTITY(MTEXT);
/// Macro call to cast dwg object to leader
CAST_DWG_OBJECT_TO_ENTITY(LEADER);
/// Macro call to cast dwg object to tolerance
CAST_DWG_OBJECT_TO_ENTITY(TOLERANCE);
/// Macro call to cast dwg object to mline
CAST_DWG_OBJECT_TO_ENTITY(MLINE);
/// Macro call to cast dwg object to lwpline
CAST_DWG_OBJECT_TO_ENTITY(LWPLINE);
/// Macro call to cast dwg object to hatch
CAST_DWG_OBJECT_TO_ENTITY(HATCH);


/*******************************************************************
*     Functions created from macro to cast dwg object to object     *
*                 Usage :- dwg_object_to_OBJECT(),                  *
*            where OBJECT can be LAYER or BLOCK_HEADER              *
********************************************************************/
/// Macro call to cast dwg object to block header
CAST_DWG_OBJECT_TO_OBJECT(BLOCK_HEADER);
/// Macro call to cast dwg object to block control
CAST_DWG_OBJECT_TO_OBJECT(BLOCK_CONTROL);
/// Macro call to cast dwg object to layer
CAST_DWG_OBJECT_TO_OBJECT(LAYER);

/*******************************************************************
*                FUNCTIONS START HERE ENTITY SPECIFIC               *
********************************************************************/

/*******************************************************************
*                    FUNCTIONS FOR CIRCLE ENTITY                    *
********************************************************************/

/// This function creates and returns a new circle entity with default  values.
/** Usage : dwg_ent_circle_new();
*/
dwg_ent_circle *
dwg_ent_circle_new(int *error)
{
  dwg_ent_circle *circle = (dwg_ent_circle*) 
  malloc(sizeof(Dwg_Entity_CIRCLE));
    if(circle != 0)
      {
        *error = 0;
        return dwg_ent_circle_init(circle);        
      }
    else
      *error = 1;
}

/// This function frees the memory allocated by the dwg_ent_circle_new() function.
/** Usage :- dwg_ent_circle_delete();
*/
void 
dwg_ent_circle_delete(dwg_ent_circle *circle, int *error)
{
  if (circle != 0)
    {
      *error = 0;
      free(circle);
    }
  else
    *error = 1;
}

/// This function initialises a new circle entity with default values.
/** Usage :- dwg_ent_circle_init(circle);
where circle is a pointer of dwg_ent_circle type
*/
dwg_ent_circle *
dwg_ent_circle_init(dwg_ent_circle *circle)
{
  if (circle != 0)
    {
      circle->center.x    = circle->center.y = 0.0;
      circle->center.z    = 0.0;
      circle->radius      = 0.0;
      circle->thickness   = 0.0;
      circle->extrusion.x = circle->extrusion.y = 0.0;
      circle->extrusion.z = 0.0;
    }
  return circle;
}

/// Returns the center values of circle to second argument.
/** Usage : dwg_ent_circle_get_center(circle, &point, &error);
\param 1 dwg_ent_circle
\param 2 dwg_point_3d
\param 3 int
*/
void
dwg_ent_circle_get_center(dwg_ent_circle *circle, dwg_point_3d *point, 
                          int *error)
{
  if(point != 0 && circle != 0)
    {
      *error = 0;
      point->x = circle->center.x;
      point->y = circle->center.y;
      point->z = circle->center.z;
    }
  else
    *error = 1;
}

/// sets the center values of circle.
/** Usage : dwg_ent_circle_set_center(circle, &point, &error);
\param 1 dwg_ent_circle
\param 2 dwg_point_3d
\param 3 int
*/
void
dwg_ent_circle_set_center(dwg_ent_circle *circle, dwg_point_3d *point,
                          int *error)
{
  if(point != 0 && circle != 0)
    {
      *error = 0;
      circle->center.x = point->x;
      circle->center.y = point->y;
      circle->center.z = point->z;
    }
  else
    *error = 1;
}

/// returns the circle radius.
/** Usage : double radius = dwg_ent_circle_get_radius(circle, &error);
\param 1 dwg_ent_circle
\param 2 int
*/
double
dwg_ent_circle_get_radius(dwg_ent_circle *circle, int *error)
{
  if(circle != 0)
    {
      *error = 0;
      return circle->radius;
    }
  else
    *error = 1;
}

/// sets the circle radius.
/** Usage : dwg_ent_circle_set_radius(circle, radius, &error);
\param 1 dwg_ent_circle
\param 2 double
\param 3 int
*/
void
dwg_ent_circle_set_radius(dwg_ent_circle *circle, double radius, int *error)
{
  if(circle != 0)
    {
      *error = 0;
      circle->radius = radius;
    }
  else
    *error = 1;
}

/// returns the circle thickness.
/** Usage : double thickness = dwg_ent_circle_get_thickness(circle, &error);
\param 1 dwg_ent_circle
\param 2 int
*/
double
dwg_ent_circle_get_thickness(dwg_ent_circle *circle, int *error)
{
  if(circle != 0)
    {
      *error = 0;
      return circle->thickness;
    }
  else
    *error = 1;
}

/// sets the circle thickness.
/** Usage : dwg_ent_circle_set_thickness(circle, thickness, &error);
\param 1 dwg_ent_circle
\param 2 int
*/
void
dwg_ent_circle_set_thickness(dwg_ent_circle *circle, double thickness,
                             int *error)
{
  if(circle != 0)
    {
      *error = 0;
      circle->thickness = thickness;
    }
  else
    *error = 1;
}

/// returns extrusion value of circle.
/** Usage : dwg_ent_circle_get_extrusion(circle, &point, &error);
\param 1 dwg_ent_circle
\param 2 dwg_point_3d
\param 3 int
*/
void
dwg_ent_circle_set_extrusion(dwg_ent_circle *circle, dwg_point_3d *vector,
                             int *error)
{
  if(circle != 0)
    {
      *error = 0;
      circle->extrusion.x = vector->x;
      circle->extrusion.y = vector->y;
      circle->extrusion.z = vector->z;
    }
  else
    *error = 1;
}

/// sets extrusion value of circle.
/** Usage : dwg_ent_circle_set_extrusion(circle, &point, &error);
\param 1 dwg_ent_circle
\param 2 dwg_point_3d
\param 3 int
*/
void
dwg_ent_circle_get_extrusion(dwg_ent_circle *circle, dwg_point_3d *vector,
                             int *error)
{
  if(circle != 0)
    {
      *error = 0;
      vector->x = circle->extrusion.x;
      vector->y = circle->extrusion.y;
      vector->z = circle->extrusion.z;
    }
  else
    *error = 1;
}

/*******************************************************************
*                    FUNCTIONS FOR LINE ENTITY                      *
********************************************************************/

/// This function creates a new entity of line.
/** Usage :- dwg_ent_line_new();
*/
dwg_ent_line *
dwg_ent_line_new(int *error)
{
  dwg_ent_line *line = (dwg_ent_line*) 
  malloc(sizeof(Dwg_Entity_LINE));
    if(line != 0)
      {
        *error = 0;
        return dwg_ent_line_init(line);
      }
    else
      *error = 1;
}

/// This function deletes the entity created using dwg_ent_line_new() function and frees the allocated memory.
/** Usage :- dwg_ent_line_delete();
*/
void 
dwg_ent_line_delete(dwg_ent_line *line, int *error)
{
  if (line != 0)
    {
      *error = 0;
      free(line);
    }
  else
    *error = 1;
}

/// This function initialises a line entity with default or zero values.
/** Usage :- dwg_ent_line_init();
*/
dwg_ent_line *
dwg_ent_line_init(dwg_ent_line *line)
{
  if (line != 0)
    {
      line->start.x     = line->start.y = line->start.z = 0.0;
      line->end.x       = line->end.y = line->end.z = 0.0;
      line->thickness   = 0.0;
      line->extrusion.x = line->extrusion.y = 0.0; 
      line->extrusion.z = 0.0;
    }
  return line;
}

/// returns line start points.
/** Usage : dwg_ent_line_get_start_point(line, &start_points, &error);
\param 1 dwg_ent_line
\param 2 dwg_point_2d
\param 3 int
*/
void
dwg_ent_line_get_start_point(dwg_ent_line *line, dwg_point_3d *point,
                             int *error)
{
  if (line != 0)
    {
      *error = 0;
      point->x = line->start.x;
      point->y = line->start.y;
      point->z = line->start.z;
    }
  else
    *error = 1;
}

/// sets line start points.
/** Usage : dwg_ent_line_set_start_point(line, &start_points, &error);
\param 1 dwg_ent_line
\param 2 dwg_point_2d
\param 3 int
*/
void
dwg_ent_line_set_start_point(dwg_ent_line *line, dwg_point_3d *point,
                             int *error)
{
  if (line != 0)
    {
      *error = 0;
      line->start.x = point->x;
      line->start.y = point->y;
      line->start.z = point->z;
    }
  else
    *error = 1;
}

/// returns line end points.
/** Usage : dwg_ent_line_get_start_end(line, &end_points, &error);
\param 1 dwg_ent_line
\param 2 dwg_point_2d
\param 3 int
*/
void
dwg_ent_line_get_end_point(dwg_ent_line *line, dwg_point_3d *point, 
                           int *error)
{
  if (line != 0)
    {
      *error = 0;
      point->x = line->end.x;
      point->y = line->end.y;
      point->z = line->end.z;
    }
  else
    *error = 1;
}

/// sets line end points.
/** Usage : dwg_ent_line_set_start_end(line, &end_points, &error);
\param 1 dwg_ent_line
\param 2 dwg_point_2d
\param 3 int
*/
void
dwg_ent_line_set_end_point(dwg_ent_line *line, dwg_point_3d *point, 
                           int *error)
{
  if (line != 0)
    {
      *error = 0;
      line->end.x = point->x;
      line->end.y = point->y;
      line->end.z = point->z;
    }
  else
    *error = 1;
}

/// Returns line thickness.
/** Usage : double thickness = dwg_ent_line_get_thickness(line, &error);
\param 1 dwg_ent_line
\param 2 int
*/
double
dwg_ent_line_get_thickness(dwg_ent_line *line, int *error)
{
  if (line != 0)
    {
      *error = 0;
      return line->thickness;
    }
  else
    *error = 1;
}

/// Sets line thickness.
/** Usage : dwg_ent_line_get_thickness(line, thickness, &error);
\param 1 dwg_ent_line
\param 2 double
\param 3 int
*/
void
dwg_ent_line_set_thickness(dwg_ent_line *line, double thickness, int *error)
{
  if (line != 0)
    {
      *error = 0;
      line->thickness = thickness;
    }
  else
    *error = 1;
}

/// returns line extrusion.
/** Usage : dwg_ent_line_get_extrusion(line, &ext_points, &error);
\param 1 dwg_ent_line
\param 2 dwg_point_3d
\param 3 int
*/
void
dwg_ent_line_get_extrusion(dwg_ent_line *line, dwg_point_3d *vector,
                           int *error)
{
  if (line != 0)
    {
      *error = 0;
      vector->x = line->extrusion.x;
      vector->y = line->extrusion.y;
      vector->z = line->extrusion.z;
    }
  else
    *error = 1;
}

/// sets line extrusion.
/** Usage : dwg_ent_line_set_extrusion(line, &ext_points, &error);
\param 1 dwg_ent_line
\param 2 dwg_point_3d
\param 3 int
*/
void
dwg_ent_line_set_extrusion(dwg_ent_line *line, dwg_point_3d *vector,
                           int *error)
{
  if (line != 0)
    {
      *error = 0;
      line->extrusion.x = vector->x;
      line->extrusion.y = vector->y;
      line->extrusion.z = vector->z;
    }
  else
    *error = 1;
}

/*******************************************************************
*                    FUNCTIONS FOR ARC ENTITY                       *
********************************************************************/

/// This function creates a new entity of arc type.
/** Usage :- dwg_ent_arc_new();
*/
dwg_ent_arc *
dwg_ent_arc_new(int *error)
{
  dwg_ent_arc *arc = (dwg_ent_arc*) malloc(sizeof(Dwg_Entity_ARC));
    if (arc != 0)
      {
        *error = 0;
        return dwg_ent_arc_init(arc);
      }
    else
      *error = 1;
}

/// This function deletes entity created by dwg_ent_arc_new() function and frees the allocated memory.
/** Usage :- dwg_ent_arc_delete();
*/
void 
dwg_ent_arc_delete(dwg_ent_arc *arc, int *error)
{
  if (arc != 0)
    {
      *error = 0;
      free(arc);
    }
  else
    *error = 1;
}

/// This function initialises a arc entity with default/zero values.
/** Usage :- dwg_ent_arc_init();
*/
dwg_ent_arc *
dwg_ent_arc_init(dwg_ent_arc *arc)
{
  if (arc != 0)
    {
      arc->center.x    = arc->center.y = arc->center.z = 0.0;
      arc->radius      = 0.0;
      arc->thickness   = 0.0;
      arc->extrusion.x = arc->extrusion.y = arc->extrusion.z = 0.0;
      arc->start_angle = 0.0;
      arc->end_angle   = 0.0;
    }
  return arc;
}

/// sets arc center.
/** Usage : dwg_ent_arc_get_center(arc, &point, &error);
\param 1 dwg_ent_arc
\param 2 dwg_point_3d
\param 3 int
*/
void
dwg_ent_arc_get_center(dwg_ent_arc *arc, dwg_point_3d *point,
                       int *error)
{
  if (arc != 0)
    {
      *error = 0;
      point->x = arc->center.x;
      point->y = arc->center.y;
      point->z = arc->center.z;
    }
  else
    *error = 1;
}

/// sets arc center.
/** Usage : dwg_ent_arc_set_center(arc, &point, &error);
\param 1 dwg_ent_arc
\param 2 dwg_point_3d
\param 3 int
*/
void
dwg_ent_arc_set_center(dwg_ent_arc *arc, dwg_point_3d *point,
                       int *error)
{
  if (arc != 0)
    {
      *error = 0;
      arc->center.x = point->x;
      arc->center.y = point->y;
      arc->center.z = point->z;
    }
  else
    *error = 1;
}

/// Returns arc radius.
/** Usage : double radius = dwg_ent_arc_get_radius(arc, &error);
\param 1 dwg_ent_arc
\param 2 int
*/
double
dwg_ent_arc_get_radius(dwg_ent_arc *arc, int *error)
{
  if (arc != 0)
    {
      *error = 0;
      return arc->radius;
    }
  else
    *error = 1;
}

/// Sets arc radius
/** Usage : dwg_ent_arc_set_radius(arc, radius, &error);
\param 1 dwg_ent_arc
\param 2 double
\param 3 int
*/
void
dwg_ent_arc_set_radius(dwg_ent_arc *arc, double radius, int *error)
{
  if (arc != 0)
    {
      *error = 0;
      arc->radius = radius;
    }
  else
    *error = 1;
}
    
/// Returns arc thickness.
/** Usage : double thickness = dwg_ent_arc_get_thickness(arc, &error);
\param 1 dwg_ent_arc
\param 2 int
*/
double
dwg_ent_arc_get_thickness(dwg_ent_arc *arc, int *error)
{
  if (arc != 0)
    {
      *error = 0;
      return arc->thickness;
    }
  else
    *error = 1;
}

/// Sets arc thickness.
/** Usage : dwg_ent_arc_get_thickness(arc, thickness, &error);
\param 1 dwg_ent_arc
\param 2 double
\param 3 int
*/
void
dwg_ent_arc_set_thickness(dwg_ent_arc *arc, double thickness, int *error)
{
  if (arc != 0)
    {
      *error = 0;
      arc->thickness = thickness;
    }
  else
    *error = 1;
}

/// returns arc extrusion.
/** Usage : dwg_ent_arc_get_extrusion(arc, &ext_points, &error);
\param 1 dwg_ent_arc
\param 2 dwg_point_3d
\param 3 int
*/
void
dwg_ent_arc_get_extrusion(dwg_ent_arc *arc, dwg_point_3d *vector, int *error)
{
  if (arc != 0)
    {
      *error = 0;
      vector->x = arc->extrusion.x;
      vector->y = arc->extrusion.y;
      vector->z = arc->extrusion.z;
    }
  else
    *error = 1;
}

/// sets arc extrusion.
/** Usage : dwg_ent_arc_set_extrusion(arc, &ext_points, &error);
\param 1 dwg_ent_arc
\param 2 dwg_point_3d
\param 3 int
*/
void 
dwg_ent_arc_set_extrusion(dwg_ent_arc *arc, dwg_point_3d *vector, int *error)
{
  if (arc != 0)
    {
      *error = 0;
      arc->extrusion.x = vector->x;
      arc->extrusion.y = vector->y;
      arc->extrusion.z = vector->z;    
    }
  else
    *error = 1;
}

/// Returns arc start angle.
/** Usage : double start_angle = dwg_ent_arc_get_start_angle(line, &error);
\param 1 dwg_ent_arc
\param 2 int
*/
double
dwg_ent_arc_get_start_angle(dwg_ent_arc *arc, int *error)
{
  if (arc != 0)
    {
      *error = 0;
      return arc->start_angle;
    }
  else
    *error = 1;
}

/// Sets arc start angle.
/** Usage : dwg_ent_arc_set_start_angle(arc, start_angle, &error);
\param 1 dwg_ent_arc
\param 2 double
\param 3 int
*/
void
dwg_ent_arc_set_start_angle(dwg_ent_arc *arc, double start_angle, int *error)
{
  if (arc != 0)
    {
      *error = 0;
      arc->start_angle = start_angle;
    }
  else
    *error = 1;
}

/// Returns arc end angle.
/** Usage : double end_angle = dwg_ent_arc_get_end_angle(arc, &error);
\param 1 dwg_ent_arc
\param 2 int
*/
double
dwg_ent_arc_get_end_angle(dwg_ent_arc *arc, int *error)
{
  if (arc != 0)
    {
      *error = 0;
      return arc->end_angle;
    }
  else
    *error = 1;
}

/// Sets arc end angle.
/** Usage : dwg_ent_arc_set_end_angle(arc, end_angle, &error);
\param 1 dwg_ent_arc
\param 2 double
\param 3 int
*/
void
dwg_ent_arc_set_end_angle(dwg_ent_arc *arc, double end_angle, int *error)
{
  if (arc != 0)
    {
      *error = 0;
      arc->end_angle = end_angle;
    }
  else
    *error = 1;
}

/*******************************************************************
*                   FUNCTIONS FOR ELLIPSE ENTITY                    *
********************************************************************/

/// This function creates a new entity of ellipse type.
/** Usage :- dwg_ent_ellipse_new();
*/
dwg_ent_ellipse *
dwg_ent_ellipse_new(int *error)
{
  dwg_ent_ellipse *ellipse = (dwg_ent_ellipse*)
  malloc(sizeof(Dwg_Entity_ELLIPSE));
    if (ellipse != 0)
      {
        *error = 0;
        return dwg_ent_ellipse_init(ellipse);
      }
    else
      *error = 1;
}

/// This function deletes entity created by dwg_ent_ellipse_new() function and frees the allocated memory.
/** Usage :- dwg_ent_ellipse_delete();
*/
void
dwg_ent_ellipse_delete(dwg_ent_ellipse *ellipse, int *error)
{
  if (ellipse != 0)
    {
      *error = 0;
      free(ellipse);
    }
  else
    *error = 1;
}

/// This function initialises a ellipse entity with default or zero values.
/** Usage :- dwg_ent_ellipse_init();
*/
dwg_ent_ellipse *
dwg_ent_ellipse_init(dwg_ent_ellipse *ellipse)
{
  if (ellipse != 0)
    {
      ellipse->center.x    = ellipse->center.y = 0.0;
      ellipse->center.z    = 0.0;
      ellipse->sm_axis.x   = ellipse->sm_axis.y = 0.0;
      ellipse->sm_axis.z   = 0.0;
      ellipse->extrusion.x = ellipse->extrusion.y = 0.0;
      ellipse->extrusion.z = 0.0;
      ellipse->axis_ratio  = 0.0;
      ellipse->start_angle = 0.0;
      ellipse->end_angle   = 0.0;
    }
  return ellipse;
}

/// returns ellipse center.
/** Usage : dwg_ent_ellipse_get_center(ellipse, &center, &error);
\param 1 dwg_ent_ellipse
\param 2 dwg_point_3d
\param 3 int
*/
void
dwg_ent_ellipse_get_center(dwg_ent_ellipse *ellipse, dwg_point_3d *point,
                           int *error)
{
  if (ellipse != 0)
    {
      *error = 0;
      point->x = ellipse->center.x;
      point->y = ellipse->center.y;
      point->z = ellipse->center.z;
    }
  else
    *error = 1;
}

/// sets ellipse center
/** Usage : dwg_ent_ellipse_set_center(ellipse, &center &error);
\param 1 dwg_ent_ellipse
\param 2 dwg_point_3d
\param 3 int
*/
void
dwg_ent_ellipse_set_center(dwg_ent_ellipse *ellipse, dwg_point_3d *point,
                           int *error)
{
  if (ellipse != 0)
    {
      *error = 0;
      ellipse->center.x = point->x;
      ellipse->center.y = point->y;
      ellipse->center.z = point->z;
    }
  else
    *error = 1;
}

/// returns ellipse sm axis.
/** Usage : dwg_ent_ellipse_get_sm_axis(ellipse, &point, &error);
\param 1 dwg_ent_ellipse
\param 2 dwg_point_3d
\param 3 int
*/
void
dwg_ent_ellipse_get_sm_axis(dwg_ent_ellipse *ellipse, dwg_point_3d *point,
                            int *error)
{
  if (ellipse != 0)
    {
      *error = 0;
      point->x = ellipse->sm_axis.x;
      point->y = ellipse->sm_axis.y;
      point->z = ellipse->sm_axis.z;
    }
  else
    *error = 1;
}

/// sets ellipse sm axis.
/** Usage : dwg_ent_ellipse_set_sm_axis(ellipse, &point, &error);
\param 1 dwg_ent_ellipse
\param 2 dwg_point_3d
\param 3 int
*/
void
dwg_ent_ellipse_set_sm_axis(dwg_ent_ellipse *ellipse, dwg_point_3d *point,
                            int *error)
{
  if (ellipse != 0)
    {
      *error = 0;
      ellipse->sm_axis.x = point->x;
      ellipse->sm_axis.y = point->y;
      ellipse->sm_axis.z = point->z;
    }
  else
    *error = 1;
}

/// returns ellipse extrusion.
/** Usage : dwg_ent_ellipse_get_extrusion(ellipse, &ext_points, &error);
\param 1 dwg_ent_ellipse
\param 2 dwg_point_3d
\param 3 int
*/
void
dwg_ent_ellipse_get_extrusion(dwg_ent_ellipse *ellipse, dwg_point_3d *vector,
                              int *error)
{
  if (ellipse != 0)
    {
      *error = 0;
      vector->x = ellipse->extrusion.x;
      vector->y = ellipse->extrusion.y;
      vector->z = ellipse->extrusion.z;
    }
  else
    *error = 1;
}

/// sets ellipse extrusion.
/** Usage : dwg_ent_ellipse_set_extrusion(ellipse, &ext_points, &error);
\param 1 dwg_ent_ellipse
\param 2 dwg_point_3d
\param 3 int
*/
void
dwg_ent_ellipse_set_extrusion(dwg_ent_ellipse *ellipse, dwg_point_3d *vector, 
                              int *error)
{
  if (ellipse != 0)
    {
      *error = 0;
      ellipse->extrusion.x = vector->x;
      ellipse->extrusion.y = vector->y;
      ellipse->extrusion.z = vector->z;
    }
  else
    *error = 1;
}

/// Returns ellipse axis ratio
/** Usage : double axis_ratio = dwg_ent_ellipse_get_axis_ratio(ellipse, &error);
\param 1 dwg_ent_ellipse
\param 2 int
*/
double
dwg_ent_ellipse_get_axis_ratio(dwg_ent_ellipse *ellipse, int *error)
{
  if (ellipse != 0)
    {
      *error = 0;
      return ellipse->axis_ratio;
    }
  else
    *error = 1;
}

/// Sets ellipse axis ratio.
/** Usage : dwg_ent_ellipse_set_axis_ratio(ellipse, axis_ratio, &error);
\param 1 dwg_ent_ellipse
\param 2 double
\param 3 int
*/
void
dwg_ent_ellipse_set_axis_ratio(dwg_ent_ellipse *ellipse, double ratio,
                               int *error)
{
  if (ellipse != 0)
    {
      *error = 0;
      ellipse->axis_ratio = ratio;
    }
  else
    *error = 1;
}

/// Returns ellipse start angle.
/** Usage : double start_angle = dwg_ent_ellipse_get_start_angle(ellipse, &error);
\param 1 dwg_ent_ellipse
\param 2 int
*/
double
dwg_ent_ellipse_get_start_angle(dwg_ent_ellipse *ellipse, int *error)
{
  if (ellipse != 0)
    {
      *error = 0;
      return ellipse->start_angle;
    }
  else
    *error = 1;
}

/// Sets ellipse start angle.
/** Usage : dwg_ent_ellipse_set_start_angle(ellipse, start_angle, &error);
\param 1 dwg_ent_ellipse
\param 2 double
\param 3 int
*/
void
dwg_ent_ellipse_set_start_angle(dwg_ent_ellipse *ellipse, double start_angle,
                                int *error)
{
  if (ellipse != 0)
    {
      *error = 0;
      ellipse->start_angle = start_angle;
    }
  else
    *error = 1;
}

/// Returns ellipse end angle.
/** Usage : double end_angle = dwg_ent_ellipse_get_end_angle(ellipse, &error);
\param 1 dwg_ent_ellipse
\param 2 int
*/
double
dwg_ent_ellipse_get_end_angle(dwg_ent_ellipse *ellipse, int *error)
{
  if (ellipse != 0)
    {
      *error = 0;
      return ellipse->end_angle;
    }
  else
    *error = 1;
}

/// Sets ellipse end angle.
/** Usage : dwg_ent_ellipse_set_end_angle(ellipse, end_angle, &error);
\param 1 dwg_ent_ellipse
\param 2 double
\param 3 int
*/
void
dwg_ent_ellipse_set_end_angle(dwg_ent_ellipse *ellipse, double end_angle,
                              int *error)
{
  if (ellipse != 0)
    {
      *error = 0;
      ellipse->end_angle = end_angle;
    }
  else
    *error = 1;
}

/*******************************************************************
*                    FUNCTIONS FOR TEXT ENTITY                      *
********************************************************************/

/// Sets text value of a text entity
/** Usage : dwg_ent_text_set_text(text, "Hello world", &error);
\param 1 dwg_ent_text
\param 2 string ( char * )
\param 2 int 
*/
void
dwg_ent_text_set_text(dwg_ent_text *text, char * text_value, int *error)
{
  if (text != 0)
    {
      *error = 0;
      text->text_value = text_value;
    }
  else
    *error = 1;
}

/// This returns the text value of a text entity.
/** Usage : dwg_ent_text_get_text(text, &error);
\param 1 dwg_ent_text
\param 2 int
*/
char *
dwg_ent_text_get_text(dwg_ent_text *text, int *error)
{
  if (text != 0)
    {
      *error = 0;
      return text->text_value;
    }
  else
    *error = 1;
}

/// This returns the insertion point of a text entity into second argument.
/** Usage : dwg_ent_text_get_insertion_point(text, &point, &error);
\param 1 dwg_ent_text
\param 2 dwg_2d_point
\param 3 int
*/
void
dwg_ent_text_get_insertion_point(dwg_ent_text *text, dwg_point_2d *point,
                                 int *error)
{
  if (text != 0)
    {
      *error = 0;
      point->x = text->insertion_pt.x;
      point->y = text->insertion_pt.y;
    }
  else
    *error = 1;
}

/// Sets insertion point
/** Usage :- dwg_ent_text_set_insertion_point(text, &point, &error)
\param 1 dwg_ent_text
\param 2 dwg_point_2d
\param 3 int
*/
void
dwg_ent_text_set_insertion_point(dwg_ent_text *text, dwg_point_2d *point,
                                 int *error)
{
  if (text != 0)
    {
      *error = 0;
      text->insertion_pt.x = point->x;
      text->insertion_pt.y = point->y;
    }
  else
    *error = 1;
}

/// Returns height of a text.
/** Usage :- double height = dwg_ent_text_get_height(text);
*/
double
dwg_ent_text_get_height(dwg_ent_text *text, int *error)
{
  if (text != 0)
    {
      *error = 0;
      return text->height;
    }
  else
    *error = 1;
}

/// Sets height of Text
/** Usage :- dwg_ent_text_set_height(text, 100, &error);
\param dwg_ent_text
\param double
\param int
*/
void
dwg_ent_text_set_height(dwg_ent_text *text, double height, int *error)
{
  if (text != 0)
    {
      *error = 0;
      text->height = height;
    }
  else
    *error = 1;
}

/// Returns extrusion value
/** Usage : dwg_ent_text_get_extrusion(text, &point, &error);
\param dwg_ent_text
\param dwg_point_2d
\param int
*/
void
dwg_ent_text_get_extrusion(dwg_ent_text *text, dwg_point_3d *vector,
                           int *error)
{
  if (text != 0)
    {
      *error = 0;
      vector->x = text->extrusion.x;
      vector->y = text->extrusion.y;
      vector->z = text->extrusion.z;
    }
  else
    *error = 1;
}

/// Sets the text extrusion value.
/** Usage :- dwg_ent_text_set_extrusion(text, &point, &error);
\param dwg_ent_text
\param dwg_point_2d
\param int
*/
void
dwg_ent_text_set_extrusion(dwg_ent_text *text, dwg_point_3d *vector,
                           int *error)
{
  if (text != 0)
    {
      *error = 0;
      text->extrusion.x = vector->x;
      text->extrusion.y = vector->y;
      text->extrusion.z = vector->z;
    }
  else
    *error = 1;
}

/// Returns the thickness of a text entity.
/** Usage : dwg_ent_text_get_thickness(text, &error);
\param dwg_ent_text
\param int
*/
double
dwg_ent_text_get_thickness(dwg_ent_text *text, int *error)
{
  if (text != 0)
    {
      *error = 0;
      return text->thickness;
    }
  else
    *error = 1;
}

/// Sets the thickness of text.
/** Usage :- dwg_ent_text_set_thickness(text, 50, &error);
\param dwg_ent_text
\param double
\param int
*/
void
dwg_ent_text_set_thickness(dwg_ent_text *text, double thickness, int *error)
{
  if (text != 0)
    {
      *error = 0;
      text->thickness = thickness;
    }
  else
    *error = 1;
}

/// Returns the rotation angle of a text entity.
/** Usage :- double rot_ang = dwg_ent_text_get_rot_angle(text, &error);
\param dwg_ent_text
\param int
*/
double
dwg_ent_text_get_rot_angle(dwg_ent_text *text, int *error)
{
  if (text != 0)
    {
      *error = 0;
      return text->rotation_ang;
    }
  else
    *error = 1;
}

/// Sets the rotation angle of a text entity.
/** Usage : dwg_ent_text_set_rot_angle(text, angle, &error);
\param dwg_ent_text
\param double
\param int
*/
void
dwg_ent_text_set_rot_angle(dwg_ent_text *text, double angle, int *error)
{
  if (text != 0)
    {
      *error = 0;
      text->rotation_ang = angle;
    }
  else
    *error = 1;
}

/// Returns the vertical alignment of a text entity.
/** Usage : double align = dwg_ent_text_get_vert_align(text, &error);
\param dwg_ent_text
\param int
*/
double
dwg_ent_text_get_vert_align(dwg_ent_text *text, int *error)
{
  if (text != 0)
    {
      *error = 0;
      return text->vert_alignment;
    }
  else
    *error = 1;
}

/// Sets the vertical alignment of a text entity 
/** Usage : dwg_ent_text_set_vert_align(text, angle, &error);
\param dwg_ent_text
\param double
\param int
*/
void
dwg_ent_text_set_vert_align(dwg_ent_text *text, double alignment, int *error)
{
  if (text != 0)
    {
      *error = 0;
      text->vert_alignment = alignment;
    }
  else
    *error = 1;
}

/// Returns the horizontal alignment of a text entity.
/** Usage : double align = dwg_ent_text_get_horiz_align(text, &error);
\param dwg_ent_text
\param int
*/
double
dwg_ent_text_get_horiz_align(dwg_ent_text *text, int *error)
{
  if (text != 0)
    {
      *error = 0;
      return text->horiz_alignment;
    }
  else
    *error = 1;
}

/// Sets the horizontal alignment of a text entity.
/** Usage : dwg_ent_text_set_horiz_align(text, angle, &error);
\param dwg_ent_text
\param double
\param int
*/
void
dwg_ent_text_set_horiz_align(dwg_ent_text *text, double alignment, int *error)
{
  if (text != 0)
    {
      *error = 0;
      text->horiz_alignment = alignment;
    }
  else
    *error = 1;
}

/*******************************************************************
*                   FUNCTIONS FOR ATTRIB ENTITY                     *
********************************************************************/

/// Sets text value of a attrib entity.
/** Usage : dwg_ent_attrib_set_text(attrib, "Hello world", &error);
\param dwg_ent_attrib
\param string (char *)
\param int
*/
void
dwg_ent_attrib_set_text(dwg_ent_attrib *attrib, char * text_value, int *error)
{
  if (attrib != 0)
    {
      *error = 0;
      attrib->text_value = text_value;
    }
  else
    *error = 1;
}

/// Returns the text value of a attrib entity.
/** Usage : char * text_val = dwg_ent_attrib_get_text(attrib, &error);
\param dwg_ent_attrib
\param int
*/
char *
dwg_ent_attrib_get_text(dwg_ent_attrib *attrib, int *error)
{
  if (attrib != 0)
    {
      *error = 0;
      return attrib->text_value;
    }
  else
    *error = 1;
}

/// Returns the insertion point of a attrib entity.
/** Usage : dwg_ent_attrib_get_insertion_point(attrib, &point, &error);
\param dwg_ent_attrib
\param dwg_point_3d
\param int
*/
void
dwg_ent_attrib_get_insertion_point(dwg_ent_attrib *attrib,
                                   dwg_point_2d *point, int *error)
{
  if (attrib != 0)
    {
      *error = 0;
      point->x = attrib->insertion_pt.x;
      point->y = attrib->insertion_pt.y;
    }
  else
    *error = 1;
}

/// Sets the insertion point of a attrib entity.
/** Usage : dwg_ent_attrib_set_insertion_point(attrib, &point, &error)
\param dwg_ent_attrib
\param dwg_point_3d
\param int
*/
void
dwg_ent_attrib_set_insertion_point(dwg_ent_attrib *attrib,
                                   dwg_point_2d *point, int *error)
{
  if (attrib != 0)
    {
      *error = 0;
      attrib->insertion_pt.x = point->x;
      attrib->insertion_pt.y = point->y;
    }
  else
    *error = 1;
}

/// This returns the height of a attrib entity.
/** Usage : double height = dwg_ent_attrib_get_height(attrib, &error);
\param dwg_ent_attrib
\param int
*/
double
dwg_ent_attrib_get_height(dwg_ent_attrib *attrib, int *error)
{
  if (attrib != 0)
    {
      *error = 0;
      return attrib->height;
    }
  else
    *error = 1;
}

/// This sets height of a attrib entity.
/** Usage : dwg_ent_attrib_set_height(attrib, 100, &error);
\param dwg_ent_attrib
\param double
\param int
*/
void
dwg_ent_attrib_set_height(dwg_ent_attrib *attrib, double height, int *error)
{
  if (attrib != 0)
    {
      *error = 0;
      attrib->height = height;
    }
  else
    *error = 1;
}

/// Returns the extrusion values of attrib.
/** Usage : dwg_ent_attrib_get_extrusion(attrib, &point, &error);
\param dwg_ent_attrib
\param dwg_point_3d
\param int
*/
void
dwg_ent_attrib_get_extrusion(dwg_ent_attrib *attrib, dwg_point_3d *vector,
                             int *error)
{
  if (attrib != 0)
    {
      *error = 0;
      vector->x = attrib->extrusion.x;
      vector->y = attrib->extrusion.y;
      vector->z = attrib->extrusion.z;
    }
  else
    *error = 1;
}

/// Sets the attrib extrusion values equal to values of second argument.
/** Usage : dwg_ent_attrib_set_extrusion(attrib, &point, &error);
\param dwg_ent_attrib
\param dwg_point_3d
\param int
*/
void
dwg_ent_attrib_set_extrusion(dwg_ent_attrib *attrib, dwg_point_3d *vector,
                             int *error)
{
  if (attrib != 0)
    {
      *error = 0;
      attrib->extrusion.x = vector->x;
      attrib->extrusion.y = vector->y;
      attrib->extrusion.z = vector->z;
    }
  else
    *error = 1;
}

/// Returns the thickness of a attrib entity.
/** Usage : double thick = dwg_ent_attrib_get_thickness(attrib, &error);
\param dwg_ent_attrib
\param int
*/
double
dwg_ent_attrib_get_thickness(dwg_ent_attrib *attrib, int *error)
{
  if (attrib != 0)
    {
      *error = 0;
      return attrib->thickness;
    }
  else
    *error = 1;
}

/// This sets the thickness of a attrib entity.
/** Usage : dwg_ent_attrib_set_thickness(attrib, thick, &error);
\param dwg_ent_attrib
\param double
\param int
*/
void
dwg_ent_attrib_set_thickness(dwg_ent_attrib *attrib, double thickness,
                             int *error)
{
  if (attrib != 0)
    {
      *error = 0;
      attrib->thickness = thickness;
    }
  else
    *error = 1;
}

/// Returns the rotation angle of a attrib entity.
/** Usage : double angle = dwg_ent_attrib_get_rot_angle(attrib, &error);
\param dwg_ent_attrib
\param int
*/
double
dwg_ent_attrib_get_rot_angle(dwg_ent_attrib *attrib, int *error)
{
  if (attrib != 0)
    {
      *error = 0;
      return attrib->rotation_ang;
    }
  else
    *error = 1;
}

/// This sets the rotation angle of a attrib entity equal to value in second argument.
/** Usage :- dwg_ent_attrib_set_rot_angle(attrib, angle);
*/
void
dwg_ent_attrib_set_rot_angle(dwg_ent_attrib *attrib, double angle, int *error)
{
  if (attrib != 0)
    {
      *error = 0;
      attrib->rotation_ang = angle;
    }
  else
    *error = 1;
}

/// This returns the vertical alignment of a attrib entity.
/** Usage :- dwg_ent_attrib_get_vert_align(attrib);
*/
double
dwg_ent_attrib_get_vert_align(dwg_ent_attrib *attrib, int *error)
{
  if (attrib != 0)
    {
      *error = 0;
      return attrib->vert_alignment;
    }
  else
    *error = 1;
}

/// This sets the vertical alignment of a attrib entity equal to value in second argument.
/** Usage :- dwg_ent_attrib_set_vert_align(attrib, angle);
*/
void
dwg_ent_attrib_set_vert_align(dwg_ent_attrib *attrib, double alignment,
                              int *error)
{
  if (attrib != 0)
    {
      *error = 0;
      attrib->vert_alignment = alignment;
    }
  else
    *error = 1;
}

/// This returns the horizontal alignment of a attrib entity.
/** Usage :- dwg_ent_attrib_get_horiz_align(attrib);
*/
double
dwg_ent_attrib_get_horiz_align(dwg_ent_attrib *attrib, int *error)
{
  if (attrib != 0)
    {
      *error = 0;
      return attrib->horiz_alignment;
    }
  else
    *error = 1;
}

/// This sets the horizontal alignment of a attrib entity equal to value in second argument.
/** Usage :- dwg_ent_attrib_set_horiz_align(attrib, angle);
*/
void
dwg_ent_attrib_set_horiz_align(dwg_ent_attrib *attrib, double alignment,
                               int *error)
{
  if (attrib != 0)
    {
      *error = 0;
      attrib->horiz_alignment = alignment;
    }
  else
    *error = 1;
}

/*******************************************************************
*                   FUNCTIONS FOR ATTDEF ENTITY                     *
********************************************************************/

/// This sets text value of a attdef entity equal to value provided in second argument.
/** Usage :- dwg_ent_attdef_set_text(attdef, "Hello world");
This will set text value to Hello world.
*/
void
dwg_ent_attdef_set_text(dwg_ent_attdef *attdef, char * default_value,
                        int *error)
{
  if (attdef != 0)
    {
      *error = 0;
      attdef->default_value = default_value;
    }
  else
    *error = 1;
}

/// This returns the text value of a attdef entity.
/** Usage :- dwg_ent_attdef_get_text(attdef);
*/
char *
dwg_ent_attdef_get_text(dwg_ent_attdef *attdef, int *error)
{
  if (attdef != 0)
    {
      *error = 0;
      return attdef->default_value;
    }
  else
    *error = 1;
}

/// This returns the insertion point of a attdef entity into second argument.
/** Usage :- dwg_ent_attdef_get_insertion_point(attdef, point);
point.x and point.y contains x and y cordinate data respectively.
*/
void
dwg_ent_attdef_get_insertion_point(dwg_ent_attdef *attdef,
                                   dwg_point_2d *point, int *error)
{
  if (attdef != 0)
    {
      *error = 0;
      point->x = attdef->insertion_pt.x;
      point->y = attdef->insertion_pt.y;
    }
  else
    *error = 1;
}

/// This sets the insertion point of a attdef entity equal to co-ordinate values in second argument.
/** Usage :- dwg_ent_attdef_set_insertion_point(attdef, point)
*/
void
dwg_ent_attdef_set_insertion_point(dwg_ent_attdef *attdef,
                                   dwg_point_2d *point, int *error)
{
  if (attdef != 0)
    {
      *error = 0;
      attdef->insertion_pt.x = point->x;
      attdef->insertion_pt.y = point->y;
    }
  else
    *error = 1;
}

/// This returns the height of a attdef entity.
/** Usage :- dwg_ent_attdef_get_height(attdef);
*/
double
dwg_ent_attdef_get_height(dwg_ent_attdef *attdef, int *error)
{
  if (attdef != 0)
    {
      *error = 0;
      return attdef->height;
    }
  else
    *error = 1;
}

/// This sets height of a attdef entity equal to value provided in second argument.
/** Usage :- dwg_ent_attdef_set_height(attdef, 100);
This will set height to 100.
*/
void
dwg_ent_attdef_set_height(dwg_ent_attdef *attdef, double height, int *error)
{
  if (attdef != 0)
    {
      *error = 0;
      attdef->height = height;
    }
  else
    *error = 1;
}

/// Returns the extrusion values of attdef to second argument.
/** Usage :- dwg_ent_attdef_get_extrusion(attdef, point);
where attdef is a pointer of dwg_ent_attdef type and point is 
of dwg_point_3d. point.x will contain x value of extrusion,
point.y will contain y value of extrusion
*/
void
dwg_ent_attdef_get_extrusion(dwg_ent_attdef *attdef, dwg_point_3d *vector,
                             int *error)
{
  if (attdef != 0)
    {
      *error = 0;
      vector->x = attdef->extrusion.x;
      vector->y = attdef->extrusion.y;
      vector->z = attdef->extrusion.z;
    }
  else
    *error = 1;
}

/// Sets the attdef extrusion values equal to values of second argument.
/** Usage :- dwg_ent_attdef_set_extrusion(attdef, point);
where attdef is a pointer of dwg_ent_attdef type and point is 
of dwg_point_3d. point.x will contain x value of extrusion,
point.y will contain y value of extrusion
*/
void
dwg_ent_attdef_set_extrusion(dwg_ent_attdef *attdef, dwg_point_3d *vector,
                             int *error)
{
  if (attdef != 0)
    {
      *error = 0;
      attdef->extrusion.x = vector->x;
      attdef->extrusion.y = vector->y;
      attdef->extrusion.z = vector->z;
    }
  else
    *error = 1;
}

/// This returns the thickness of a attdef entity.
/** Usage :- dwg_ent_attdef_get_thickness(attdef);
*/
double
dwg_ent_attdef_get_thickness(dwg_ent_attdef *attdef, int *error)
{
  if (attdef != 0)
    {
      *error = 0;
      return attdef->thickness;
    }
  else
    *error = 1;
}

/// This sets the thickness of a attdef entity equal to value in second argument.
/** Usage :- dwg_ent_attdef_set_thickness(attdef, thickness);
*/
void
dwg_ent_attdef_set_thickness(dwg_ent_attdef *attdef, double thickness,
                             int *error)
{
  if (attdef != 0)
    {
      *error = 0;
      attdef->thickness = thickness;
    }
  else
    *error = 1;
}

/// This returns the rotation angle of a attdef entity.
/** Usage :- dwg_ent_attdef_get_rot_angle(attdef);
*/
double
dwg_ent_attdef_get_rot_angle(dwg_ent_attdef *attdef, int *error)
{
  if (attdef != 0)
    {
      *error = 0;
      return attdef->rotation_ang;
    }
  else
    *error = 1;
}

/// This sets the rotation angle of a attdef entity equal to value in second argument.
/** Usage :- dwg_ent_attdef_set_rot_angle(attdef, angle);
*/
void
dwg_ent_attdef_set_rot_angle(dwg_ent_attdef *attdef, double angle, int *error)
{
  if (attdef != 0)
    {
      *error = 0;
      attdef->rotation_ang = angle;
    }
  else
    *error = 1;
}

/// This returns the vertical alignment of a attdef entity.
/** Usage :- dwg_ent_attdef_get_vert_align(attdef);
*/
double
dwg_ent_attdef_get_vert_align(dwg_ent_attdef *attdef, int *error)
{
  if (attdef != 0)
    {
      *error = 0;
      return attdef->vert_alignment;
    }
  else
    *error = 1;
}

/// This sets the vertical alignment of a attdef entity equal to value in second argument.
/** Usage :- dwg_ent_attdef_set_vert_align(attdef, angle);
*/
void
dwg_ent_attdef_set_vert_align(dwg_ent_attdef *attdef, double alignment,
                              int *error)
{
  if (attdef != 0)
    {
      *error = 0;
      attdef->vert_alignment = alignment;
    }
  else
    *error = 1;
}

/// This returns the horizontal alignment of a attdef entity.
/** Usage :- dwg_ent_attdef_get_horiz_align(attdef);
*/
double
dwg_ent_attdef_get_horiz_align(dwg_ent_attdef *attdef, int *error)
{
  if (attdef != 0)
    {
      *error = 0;
      return attdef->horiz_alignment;
    }
  else
    *error = 1;
}

/// This sets the horizontal alignment of a attdef entity equal to value in second argument.
/** Usage :- dwg_ent_attdef_set_horiz_align(attdef, angle);
*/
void
dwg_ent_attdef_set_horiz_align(dwg_ent_attdef *attdef, double alignment,
                               int *error)
{
  if (attdef != 0)
    {
      *error = 0;
      attdef->horiz_alignment = alignment;
    }
  else
    *error = 1;
}

/*******************************************************************
*                   FUNCTIONS FOR POINT ENTITY                      *
********************************************************************/

/// Sets the point point values equal to values of second argument.
/** Usage :- dwg_ent_point_set_point(point, retpoint);
where point is a pointer of dwg_ent_point type and retpoint is 
of dwg_point_3d. retpoint.x will contain x value of point,
retpoint.y will contain y value of point
*/
void
dwg_ent_point_set_point(dwg_ent_point *point, dwg_point_3d *retpoint,
                        int *error)
{    
  if (point != 0)
    {
      *error = 0;
      point->x = retpoint->x;
      point->y = retpoint->y;
      point->z = retpoint->z;
    }
  else
    *error = 1;
}

/// Returns the point values of point to second argument.
/** Usage :- dwg_ent_point_get_point(point, retpoint);
where point is a pointer of dwg_ent_point type and retpoint is 
of dwg_point_3d. retpoint.x will contain x value of point,
retpoint.y will contain y value of point
*/
void
dwg_ent_point_get_point(dwg_ent_point *point, dwg_point_3d *retpoint,
                        int *error)
{    
  if (point != 0)
    {
      *error = 0;
      retpoint->x = point->x;
      retpoint->y = point->y;
      retpoint->z = point->z;
    }
  else
    *error = 1;
}

/// This returns the thickness of a point entity.
/** Usage :- dwg_ent_point_get_thickness(point);
*/
double
dwg_ent_point_get_thickness(dwg_ent_point *point, int *error)
{    
  if (point != 0)
    {
      *error = 0;
      return point->thickness;
    }
  else
    *error = 1;
}

/// This sets the thickness of a point entity equal to value in second argument.
/** Usage :- dwg_ent_point_set_thickness(point, thickness);
*/
void
dwg_ent_point_set_thickness(dwg_ent_point *point, double thickness,
                            int *error)
{    
  if (point != 0)
    {
      *error = 0;
      point->thickness = thickness;
    }
  else
    *error = 1;
}

/// Sets the point extrusion values equal to values of second argument.
/** Usage :- dwg_ent_point_set_extrusion(point, retpoint);
where point is a pointer of dwg_ent_point type and retpoint is 
of dwg_point_3d. retpoint.x will contain x value of extrusion,
retpoint.y will contain y value of extrusion
*/
void
dwg_ent_point_set_extrusion(dwg_ent_point *point, dwg_point_3d *retpoint,
                            int *error)
{    
  if (point != 0)
    {
      *error = 0;
      point->extrusion.x = retpoint->x;
      point->extrusion.y = retpoint->y;
      point->extrusion.z = retpoint->z;
    }
  else
    *error = 1;
}

/// Returns the extrusion values of point to second argument.
/** Usage :- dwg_ent_point_get_extrusion(point, retpoint);
where point is a pointer of dwg_ent_point type and retpoint is 
of dwg_point_3d. retpoint.x will contain x value of extrusion,
retpoint.y will contain y value of extrusion
*/
void
dwg_ent_point_get_extrusion(dwg_ent_point *point, dwg_point_3d *retpoint, 
                            int *error)
{    
  if (point != 0)
    {
      *error = 0;
      retpoint->x = point->extrusion.x;
      retpoint->y = point->extrusion.y;
      retpoint->z = point->extrusion.z;
    }
  else
    *error = 1;
}

/*******************************************************************
*                   FUNCTIONS FOR SOLID ENTITY                      *
********************************************************************/

/// This returns the thickness of a solid entity.
/** Usage :- dwg_ent_solid_get_thickness(solid);
*/
double
dwg_ent_solid_get_thickness(dwg_ent_solid *solid, int *error)
{    
  if (solid != 0)
    {
      *error = 0;
      return solid->thickness;
    }
  else
    *error = 1;
}

/// This sets the thickness of a solid entity equal to value in second argument.
/** Usage :- dwg_ent_solid_set_thickness(solid, 2.0);
Hence thickness sets to 2.0
*/
void
dwg_ent_solid_set_thickness(dwg_ent_solid *solid, double thickness,
                            int *error)
{    
  if (solid != 0)
    {
      *error = 0;
      solid->thickness = thickness;
    }
  else
    *error = 1;
}

/// This returns the elevation of a solid entity.
/** Usage :- dwg_ent_solid_get_elevation(solid);
*/
double
dwg_ent_solid_get_elevation(dwg_ent_solid *solid, int *error)
{    
  if (solid != 0)
    {
      *error = 0;
      return solid->elevation;
    }
  else
    *error = 1;
}

/// This sets the elevation of a solid entity equal to value in second argument.
/** Usage :- dwg_ent_solid_set_elevation(solid, 20);
Hence elevation sets to 20
*/
void
dwg_ent_solid_set_elevation(dwg_ent_solid *solid, double elevation,
                            int *error)
{    
  if (solid != 0)
    {
      *error = 0;
      solid->elevation = elevation;
    }
  else
    *error = 1;
}

/// Returns the corner1 values of solid to second argument.
/** Usage :- dwg_ent_solid_get_corner1(solid, point);
where solid is a pointer of dwg_ent_solid type and point is 
of dwg_point_3d. point.x will contain x value of corner1,
point.y will contain y value of corner1
*/
void
dwg_ent_solid_get_corner1(dwg_ent_solid *solid, dwg_point_2d *point,
                          int *error)
{    
  if (solid != 0)
    {
      *error = 0;
      point->x = solid->corner1.x;
      point->y = solid->corner1.y;
    }
  else
    *error = 1;
}

/// Sets the solid corner1 values equal to values of second argument.
/** Usage :- dwg_ent_solid_set_corner1(solid, point);
where solid is a pointer of dwg_ent_solid type and point is 
of dwg_point_3d. point.x will contain x value of corner1,
point.y will contain y value of corner1
*/
void
dwg_ent_solid_set_corner1(dwg_ent_solid *solid, dwg_point_2d *point,
                          int *error)
{    
  if (solid != 0)
    {
      *error = 0;
      solid->corner1.x = point->x;
      solid->corner1.y = point->y;
    }
  else
    *error = 1;
}

/// Returns the corner2 values of solid to second argument.
/** Usage :- dwg_ent_solid_get_corner2(solid, point);
where solid is a pointer of dwg_ent_solid type and point is 
of dwg_point_3d. point.x will contain x value of corner2,
point.y will contain y value of corner2
*/
void
dwg_ent_solid_get_corner2(dwg_ent_solid *solid, dwg_point_2d *point,
                          int *error)
{    
  if (solid != 0)
    {
      *error = 0;
      point->x = solid->corner2.x;
      point->y = solid->corner2.y;
    }
  else
    *error = 1;
}

/// Sets the solid corner2 values equal to values of second argument.
/** Usage :- dwg_ent_solid_set_corner2(solid, point);
where solid is a pointer of dwg_ent_solid type and point is 
of dwg_point_3d. point.x will contain x value of corner2,
point.y will contain y value of corner2
*/
void
dwg_ent_solid_set_corner2(dwg_ent_solid *solid, dwg_point_2d *point,
                          int *error)
{    
  if (solid != 0)
    {
      *error = 0;
      solid->corner2.x = point->x;
      solid->corner2.y = point->y;
    }
  else
    *error = 1;
}

/// Returns the corner3 values of solid to second argument.
/** Usage :- dwg_ent_solid_get_corner3(solid, point);
where solid is a pointer of dwg_ent_solid type and point is 
of dwg_point_3d. point.x will contain x value of corner3,
point.y will contain y value of corner3
*/
void
dwg_ent_solid_get_corner3(dwg_ent_solid *solid, dwg_point_2d *point,
                          int *error)
{    
  if (solid != 0)
    {
      *error = 0;
      point->x = solid->corner3.x;
      point->y = solid->corner3.y;
    }
  else
    *error = 1;
}

/// Sets the solid corner3 values equal to values of second argument.
/** Usage :- dwg_ent_solid_set_corner3(solid, point);
where solid is a pointer of dwg_ent_solid type and point is 
of dwg_point_3d. point.x will contain x value of corner3,
point.y will contain y value of corner3
*/
void
dwg_ent_solid_set_corner3(dwg_ent_solid *solid, dwg_point_2d *point,
                          int *error)
{    
  if (solid != 0)
    {
      *error = 0;
      solid->corner3.x = point->x;
      solid->corner3.y = point->y;
    }
  else
    *error = 1;
}

/// Returns the corner4 values of solid to second argument.
/** Usage :- dwg_ent_solid_get_corner4(solid, point);
where solid is a pointer of dwg_ent_solid type and point is 
of dwg_point_3d. point.x will contain x value of corner4,
point.y will contain y value of corner4
*/
void
dwg_ent_solid_get_corner4(dwg_ent_solid *solid, dwg_point_2d *point,
                          int *error)
{    
  if (solid != 0)
    {
      *error = 0;
      point->x = solid->corner4.x;
      point->y = solid->corner4.y;
    }
  else
    *error = 1;
}

/// Sets the solid corner4 values equal to values of second argument.
/** Usage :- dwg_ent_solid_set_corner4(solid, point);
where solid is a pointer of dwg_ent_solid type and point is 
of dwg_point_3d. point.x will contain x value of corner4,
point.y will contain y value of corner4
*/
void
dwg_ent_solid_set_corner4(dwg_ent_solid *solid, dwg_point_2d *point,
                          int *error)
{    
  if (solid != 0)
    {
      *error = 0;
      solid->corner4.x = point->x;
      solid->corner4.y = point->y;
    }
  else
    *error = 1;
}

/// Returns the extrusion values of solid to second argument.
/** Usage :- dwg_ent_solid_get_extrusion(solid, point);
where solid is a pointer of dwg_ent_solid type and point is 
of dwg_point_3d. point.x will contain x value of extrusion,
point.y will contain y value of extrusion
*/
void
dwg_ent_solid_get_extrusion(dwg_ent_solid *solid, dwg_point_3d *vector,
                            int *error)
{    
  if (solid != 0)
    {
      *error = 0;
      vector->x = solid->extrusion.x;
      vector->y = solid->extrusion.y;
      vector->z = solid->extrusion.z;
    }
  else
    *error = 1;
}

/// Sets the solid extrusion values equal to values of second argument.
/** Usage :- dwg_ent_solid_set_extrusion(solid, point);
where solid is a pointer of dwg_ent_solid type and point is 
of dwg_point_3d. point.x will contain x value of extrusion,
point.y will contain y value of extrusion
*/
void
dwg_ent_solid_set_extrusion(dwg_ent_solid *solid, dwg_point_3d *vector,
                            int *error)
{    
  if (solid != 0)
    {
      *error = 0;
      solid->extrusion.x = vector->x;
      solid->extrusion.y = vector->y;
      solid->extrusion.z = vector->z;
    }
  else
    *error = 1;
}

/*******************************************************************
*                   FUNCTIONS FOR BLOCk ENTITY                      *
********************************************************************/

/// sets name of the block entity equal to value passed in 2nd argument.
/** Usage :- dwg_ent_block_get_name(block);
*/
void
dwg_ent_block_set_name(dwg_ent_block *block, char * name, int *error)
{    
  if (block != 0)
    {
      *error = 0;
      block->name = name;
    }
  else
    *error = 1;
}

/// Returns name of the block entity passed in argument.
/** Usage :- dwg_ent_block_get_name(block);
*/
char *
dwg_ent_block_get_name(dwg_ent_block *block, int *error)
{    
  if (block != 0)
    {
      *error = 0;
      return block->name;
    }
  else
    *error = 1;
}

/*******************************************************************
*                    FUNCTIONS FOR RAY ENTITY                       *
********************************************************************/

void
dwg_ent_ray_get_point(dwg_ent_ray *ray, dwg_point_3d *point, int *error)
{    
  if (ray != 0)
    {
      *error = 0;
      point->x = ray->point.x;
      point->y = ray->point.y;
      point->z = ray->point.z;
    }
  else
    *error = 1;
}

void
dwg_ent_ray_set_point(dwg_ent_ray *ray, dwg_point_3d *point, int *error)
{    
  if (ray != 0)
    {
      *error = 0;
      ray->point.x = point->x;
      ray->point.y = point->y;
      ray->point.z = point->z;
    }
  else
    *error = 1;
}

void
dwg_ent_ray_get_vector(dwg_ent_ray *ray, dwg_point_3d *vector, int *error)
{    
  if (ray != 0)
    {
      *error = 0;
      vector->x = ray->vector.x;
      vector->y = ray->vector.y;
      vector->z = ray->vector.z;
    }
  else
    *error = 1;
}

void
dwg_ent_ray_set_vector(dwg_ent_ray *ray, dwg_point_3d *vector, int *error)
{    
  if (ray != 0)
    {
      *error = 0;
      ray->vector.x = vector->x;
      ray->vector.y = vector->y;
      ray->vector.z = vector->z;
    }
  else
    *error = 1;
}

/*******************************************************************
*                   FUNCTIONS FOR XLINE ENTITY                      *
********************************************************************/

void
dwg_ent_xline_get_point(dwg_ent_xline *xline, dwg_point_3d *point, int *error)
{    
  if (xline != 0)
    {
      *error = 0;
      point->x = xline->point.x;
      point->y = xline->point.y;
      point->z = xline->point.z;
    }
  else
    *error = 1;
}

void
dwg_ent_xline_set_point(dwg_ent_xline *xline, dwg_point_3d *point, int *error)
{    
  if (xline != 0)
    {
      *error = 0;
      xline->point.x = point->x;
      xline->point.y = point->y;
      xline->point.z = point->z;
    }
  else
    *error = 1;
}

void
dwg_ent_xline_get_vector(dwg_ent_xline *xline, dwg_point_3d *vector,
                         int *error)
{    
  if (xline != 0)
    {
      *error = 0;
      vector->x = xline->vector.x;
      vector->y = xline->vector.y;
      vector->z = xline->vector.z;
    }
  else
    *error = 1;
}

void
dwg_ent_xline_set_vector(dwg_ent_xline *xline, dwg_point_3d *vector,
                         int *error)
{    
  if (xline != 0)
    {
      *error = 0;
      xline->vector.x = vector->x;
      xline->vector.y = vector->y;
      xline->vector.z = vector->z;
    }
  else
    *error = 1;
}

/*******************************************************************
*                   FUNCTIONS FOR TRACE ENTITY                      *
********************************************************************/

/// This returns the thickness of a trace entity.
/** Usage :- dwg_ent_trace_get_thickness(trace);
*/
double
dwg_ent_trace_get_thickness(dwg_ent_trace *trace, int *error)
{    
  if (trace != 0)
    {
      *error = 0;
      return trace->thickness;
    }
  else
    *error = 1;
}

/// This sets the thickness of a trace entity equal to value in second argument.
/** Usage :- dwg_ent_trace_set_thickness(trace, 2.0);
Hence thickness sets to 2.0
*/
void
dwg_ent_trace_set_thickness(dwg_ent_trace *trace, double thickness,
                            int *error)
{    
  if (trace != 0)
    {
      *error = 0;
      trace->thickness = thickness;
    }
  else
    *error = 1;
}

/// This returns the elevation of a trace entity.
/** Usage :- dwg_ent_trace_get_elevation(trace);
*/
double
dwg_ent_trace_get_elevation(dwg_ent_trace *trace, int *error)
{    
  if (trace != 0)
    {
      *error = 0;
      return trace->elevation;
    }
  else
    *error = 1;
}

/// This sets the elevation of a trace entity equal to value in second argument.
/** Usage :- dwg_ent_trace_set_elevation(trace, 20);
Hence elevation sets to 20
*/
void
dwg_ent_trace_set_elevation(dwg_ent_trace *trace, double elevation,
                            int *error)
{    
  if (trace != 0)
    {
      *error = 0;
      trace->elevation = elevation;
    }
  else
    *error = 1;
}

/// Returns the corner1 values of trace to second argument.
/** Usage :- dwg_ent_trace_get_corner1(trace, point);
where trace is a pointer of dwg_ent_trace type and point is 
of dwg_point_3d. point.x will contain x value of corner1,
point.y will contain y value of corner1
*/
void
dwg_ent_trace_get_corner1(dwg_ent_trace *trace, dwg_point_2d *point,
                          int *error)
{    
  if (trace != 0)
    {
      *error = 0;
      point->x = trace->corner1.x;
      point->y = trace->corner1.y;
    }
  else
    *error = 1;
}

/// Sets the trace corner1 values equal to values of second argument.
/** Usage :- dwg_ent_trace_set_corner1(trace, point);
where trace is a pointer of dwg_ent_trace type and point is 
of dwg_point_3d. point.x will contain x value of corner1,
point.y will contain y value of corner1
*/
void
dwg_ent_trace_set_corner1(dwg_ent_trace *trace, dwg_point_2d *point,
                          int *error)
{    
  if (trace != 0)
    {
      *error = 0;
      trace->corner1.x = point->x;
      trace->corner1.y = point->y;
    }
  else
    *error = 1;
}

/// Returns the corner2 values of trace to second argument.
/** Usage :- dwg_ent_trace_get_corner2(trace, point);
where trace is a pointer of dwg_ent_trace type and point is 
of dwg_point_3d. point.x will contain x value of corner2,
point.y will contain y value of corner2
*/
void
dwg_ent_trace_get_corner2(dwg_ent_trace *trace, dwg_point_2d *point,
                          int *error)
{    
  if (trace != 0)
    {
      *error = 0;
      point->x = trace->corner2.x;
      point->y = trace->corner2.y;
    }
  else
    *error = 1;
}

/// Sets the trace corner2 values equal to values of second argument.
/** Usage :- dwg_ent_trace_set_corner2(trace, point);
where trace is a pointer of dwg_ent_trace type and point is 
of dwg_point_3d. point.x will contain x value of corner2,
point.y will contain y value of corner2
*/
void
dwg_ent_trace_set_corner2(dwg_ent_trace *trace, dwg_point_2d *point,
                          int *error)
{    
  if (trace != 0)
    {
      *error = 0;
      trace->corner2.x = point->x;
      trace->corner2.y = point->y;
    }
  else
    *error = 1;
}

/// Returns the corner3 values of trace to second argument.
/** Usage :- dwg_ent_trace_get_corner3(trace, point);
where trace is a pointer of dwg_ent_trace type and point is 
of dwg_point_3d. point.x will contain x value of corner3,
point.y will contain y value of corner3
*/
void
dwg_ent_trace_get_corner3(dwg_ent_trace *trace, dwg_point_2d *point,
                          int *error)
{    
  if (trace != 0)
    {
      *error = 0;
      point->x = trace->corner3.x;
      point->y = trace->corner3.y;
    }
  else
    *error = 1;
}

/// Sets the trace corner3 values equal to values of second argument.
/** Usage :- dwg_ent_trace_set_corner3(trace, point);
where trace is a pointer of dwg_ent_trace type and point is 
of dwg_point_3d. point.x will contain x value of corner3,
point.y will contain y value of corner3
*/
void
dwg_ent_trace_set_corner3(dwg_ent_trace *trace, dwg_point_2d *point,
                          int *error)
{    
  if (trace != 0)
    {
      *error = 0;
      trace->corner3.x = point->x;
      trace->corner3.y = point->y;
    }
  else
    *error = 1;
}

/// Returns the corner4 values of trace to second argument.
/** Usage :- dwg_ent_trace_get_corner4(trace, point);
where trace is a pointer of dwg_ent_trace type and point is 
of dwg_point_3d. point.x will contain x value of corner4,
point.y will contain y value of corner4
*/
void
dwg_ent_trace_get_corner4(dwg_ent_trace *trace, dwg_point_2d *point,
                          int *error)
{    
  if (trace != 0)
    {
      *error = 0;
      point->x = trace->corner4.x;
      point->y = trace->corner4.y;
    }
  else
    *error = 1;
}

/// Sets the trace corner4 values equal to values of second argument.
/** Usage :- dwg_ent_trace_set_corner4(trace, point);
where trace is a pointer of dwg_ent_trace type and point is 
of dwg_point_3d. point.x will contain x value of corner4,
point.y will contain y value of corner4
*/
void
dwg_ent_trace_set_corner4(dwg_ent_trace *trace, dwg_point_2d *point,
                          int *error)
{    
  if (trace != 0)
    {
      *error = 0;
      trace->corner4.x = point->x;
      trace->corner4.y = point->y;
    }
  else
    *error = 1;
}

/// Returns the extrusion values of trace to second argument.
/** Usage :- dwg_ent_trace_get_extrusion(trace, point);
where trace is a pointer of dwg_ent_trace type and point is 
of dwg_point_3d. point.x will contain x value of extrusion,
point.y will contain y value of extrusion
*/
void
dwg_ent_trace_get_extrusion(dwg_ent_trace *trace, dwg_point_3d *vector,
                            int *error)
{    
  if (trace != 0)
    {
      *error = 0;
      vector->x = trace->extrusion.x;
      vector->y = trace->extrusion.y;
      vector->z = trace->extrusion.z;
    }
  else
    *error = 1;
}

/// Sets the trace extrusion values equal to values of second argument.
/** Usage :- dwg_ent_trace_set_extrusion(trace, point);
where trace is a pointer of dwg_ent_trace type and point is 
of dwg_point_3d. point.x will contain x value of extrusion,
point.y will contain y value of extrusion
*/
void
dwg_ent_trace_set_extrusion(dwg_ent_trace *trace, dwg_point_3d *vector,
                            int *error)
{    
  if (trace != 0)
    {
      *error = 0;
      trace->extrusion.x = vector->x;
      trace->extrusion.y = vector->y;
      trace->extrusion.z = vector->z;
    }
  else
    *error = 1;
}

/*******************************************************************
*                 FUNCTIONS FOR VERTEX_3D ENTITY                    *
********************************************************************/

char
dwg_ent_vertex_3d_get_flags(dwg_ent_vertex_3d *vert, int *error)
{    
  if (vert != 0)
    {
      *error = 0;
      return vert->flags;
    }
  else
    *error = 1;
}

void
dwg_ent_vertex_3d_set_flags(dwg_ent_vertex_3d *vert, char flags, int *error)
{    
  if (vert != 0)
    {
      *error = 0;
      vert->flags = flags;
    }
  else
    *error = 1;
}

void
dwg_ent_vertex_3d_get_point(dwg_ent_vertex_3d *vert, dwg_point_3d *point,
                            int *error)
{    
  if (vert != 0)
    {
      *error = 0;
      point->x = vert->point.x;
      point->y = vert->point.y;
      point->z = vert->point.z;
    }
  else
    *error = 1;
}

void
dwg_ent_vertex_3d_set_point(dwg_ent_vertex_3d *vert,
                            dwg_point_3d *point, int *error)
{    
  if (vert != 0)
    {
      *error = 0;
      vert->point.x = point->x;
      vert->point.y = point->y;
      vert->point.z = point->z;
    }
  else
    *error = 1;
}

/*******************************************************************
*               FUNCTIONS FOR VERTEX_MESH ENTITY                    *
********************************************************************/

char
dwg_ent_vertex_mesh_get_flags(dwg_ent_vertex_mesh *vert, int *error)
{    
  if (vert != 0)
    {
      *error = 0;
      return vert->flags;
    }
  else
    *error = 1;
}

void
dwg_ent_vertex_mesh_set_flags(dwg_ent_vertex_mesh *vert, char flags,
                              int *error)
{    
  if (vert != 0)
    {
      *error = 0;
      vert->flags = flags;
    }
  else
    *error = 1;
}

void
dwg_ent_vertex_mesh_get_point(dwg_ent_vertex_mesh *vert, dwg_point_3d *point,
                              int *error)
{    
  if (vert != 0)
    {
      *error = 0;
      point->x = vert->point.x;
      point->y = vert->point.y;
      point->z = vert->point.z;
    }
  else
    *error = 1;
}

void
dwg_ent_vertex_mesh_set_point(dwg_ent_vertex_mesh *vert, dwg_point_3d *point,
                              int *error)
{    
  if (vert != 0)
    {
      *error = 0;
      vert->point.x = point->x;
      vert->point.y = point->y;
      vert->point.z = point->z;
    }
  else
    *error = 1;
}

/*******************************************************************
*               FUNCTIONS FOR VERTEX_PFACE ENTITY                   *
********************************************************************/

char
dwg_ent_vertex_pface_get_flags(dwg_ent_vertex_pface *vert, int *error)
{    
  if (vert != 0)
    {
      *error = 0;
      return vert->flags;
    }
  else
    *error = 1;
}

void
dwg_ent_vertex_pface_set_flags(dwg_ent_vertex_pface *vert, char flags,
                               int *error)
{    
  if (vert != 0)
    {
      *error = 0;
      vert->flags = flags;
    }
  else
    *error = 1;
}

void
dwg_ent_vertex_pface_get_point(dwg_ent_vertex_pface *vert,
                               dwg_point_3d *point, int *error)
{    
  if (vert != 0)
    {
      *error = 0;
      point->x = vert->point.x;
      point->y = vert->point.y;
      point->z = vert->point.z;
    }
  else
    *error = 1;
}

void
dwg_ent_vertex_pface_set_point(dwg_ent_vertex_pface *vert,
                               dwg_point_3d *point, int *error)
{    
  if (vert != 0)
    {
      *error = 0;
      vert->point.x = point->x;
      vert->point.y = point->y;
      vert->point.z = point->z;
    }
  else
    *error = 1;
}

/*******************************************************************
*                 FUNCTIONS FOR VERTEX_2D ENTITY                    *
********************************************************************/

char
dwg_ent_vertex_2d_get_flags(dwg_ent_vertex_2d *vert, int *error)
{    
  if (vert != 0)
    {
      *error = 0;
      return vert->flags;
    }
  else
    *error = 1;
}

void
dwg_ent_vertex_2d_set_flags(dwg_ent_vertex_2d *vert, char flags, int *error)
{    
  if (vert != 0)
    {
      *error = 0;
      vert->flags = flags;
    }
  else
    *error = 1;
}

void
dwg_ent_vertex_2d_get_point(dwg_ent_vertex_2d *vert, dwg_point_3d *point,
                            int *error)
{    
  if (vert != 0)
    {
      *error = 0;
      point->x = vert->point.x;
      point->y = vert->point.y;
      point->z = vert->point.z;
    }
  else
    *error = 1;
}

void
dwg_ent_vertex_2d_set_point(dwg_ent_vertex_2d *vert, dwg_point_3d *point,
                            int *error)
{    
  if (vert != 0)
    {
      *error = 0;
      vert->point.x = point->x;
      vert->point.y = point->y;
      vert->point.z = point->z;
    }
  else
    *error = 1;
}

double
dwg_ent_vertex_2d_get_start_width(dwg_ent_vertex_2d *vert, int *error)
{    
  if (vert != 0)
    {
      *error = 0;
      return vert->start_width;
    }
  else
    *error = 1;
}

void
dwg_ent_vertex_2d_set_start_width(dwg_ent_vertex_2d *vert, double start_width,
                                  int *error)
{    
  if (vert != 0)
    {
      *error = 0;
      vert->start_width = start_width;
    }
  else
    *error = 1;
}

double
dwg_ent_vertex_2d_get_end_width(dwg_ent_vertex_2d *vert, int *error)
{    
  if (vert != 0)
    {
      *error = 0;
      return vert->end_width;
    }
  else
    *error = 1;
}

void
dwg_ent_vertex_2d_set_end_width(dwg_ent_vertex_2d *vert, double end_width,
                                int *error)
{    
  if (vert != 0)
    {
      *error = 0;
      vert->end_width = end_width;
    }
  else
    *error = 1;
}

double
dwg_ent_vertex_2d_get_bulge(dwg_ent_vertex_2d *vert, int *error)
{    
  if (vert != 0)
    {
      *error = 0;
      return vert->bulge;
    }
  else
    *error = 1;
}

void
dwg_ent_vertex_2d_set_bulge(dwg_ent_vertex_2d *vert, double bulge, int *error)
{    
  if (vert != 0)
    {
      *error = 0;
      vert->bulge = bulge;
    }
  else
    *error = 1;
}

double
dwg_ent_vertex_2d_get_tangent_dir(dwg_ent_vertex_2d *vert, int *error)
{    
  if (vert != 0)
    {
      *error = 0;
      return vert->tangent_dir;
    }
  else
    *error = 1;
}

void
dwg_ent_vertex_2d_set_tangent_dir(dwg_ent_vertex_2d *vert, double tangent_dir,
                                  int *error)
{    
  if (vert != 0)
    {
      *error = 0;
      vert->tangent_dir = tangent_dir;
    }
  else
    *error = 1;
}

/*******************************************************************
*                   FUNCTIONS FOR INSERT ENTITY                     *
********************************************************************/

void
dwg_ent_insert_get_ins_pt(dwg_ent_insert *insert, dwg_point_3d *point,
                          int *error)
{    
  if (insert != 0)
    {
      *error = 0;
      point->x = insert->ins_pt.x;
      point->y = insert->ins_pt.y;
      point->z = insert->ins_pt.z;
    }
  else
    *error = 1;
}

void
dwg_ent_insert_set_ins_pt(dwg_ent_insert *insert, dwg_point_3d *point,
                          int *error)
{    
  if (insert != 0)
    {
      *error = 0;
      insert->ins_pt.x = point->x;
      insert->ins_pt.y = point->y;
      insert->ins_pt.z = point->z;
    }
  else
    *error = 1;
}

char
dwg_ent_insert_get_scale_flag(dwg_ent_insert *insert, int *error)
{    
  if (insert != 0)
    {
      *error = 0;
      return insert->scale_flag;
    }
  else
    *error = 1;
}

void
dwg_ent_insert_set_scale_flag(dwg_ent_insert *insert, char flags, int *error)
{    
  if (insert != 0)
    {
      *error = 0;
      insert->scale_flag = flags;
    }
  else
    *error = 1;
}

void
dwg_ent_insert_get_scale(dwg_ent_insert *insert, dwg_point_3d *point,
                         int *error)
{    
  if (insert != 0)
    {
      *error = 0;
      point->x = insert->scale.x;
      point->y = insert->scale.y;
      point->z = insert->scale.z;
    }
  else
    *error = 1;
}

void
dwg_ent_insert_set_scale(dwg_ent_insert *insert, dwg_point_3d *point,
                         int *error)
{    
  if (insert != 0)
    {
      *error = 0;
      insert->scale.x = point->x;
      insert->scale.y = point->y;
      insert->scale.z = point->z;
    }
  else
    *error = 1;
}

double
dwg_ent_insert_get_rotation_angle(dwg_ent_insert *insert, int *error)
{    
  if (insert != 0)
    {
      *error = 0;
      return insert->rotation_ang;
    }
  else
    *error = 1;
}

void
dwg_ent_insert_set_rotation_angle(dwg_ent_insert *insert, double rot_ang,
                                  int *error)
{    
  if (insert != 0)
    {
      *error = 0;
      insert->rotation_ang = rot_ang;
    }
  else
    *error = 1;
}

void
dwg_ent_insert_get_extrusion(dwg_ent_insert *insert, dwg_point_3d *point,
                             int *error)
{    
  if (insert != 0)
    {
      *error = 0;
      point->x = insert->extrusion.x;
      point->y = insert->extrusion.y;
      point->z = insert->extrusion.z;
    }
  else
    *error = 1;
}

void
dwg_ent_insert_set_extrusion(dwg_ent_insert *insert, dwg_point_3d *point,
                             int *error)
{    
  if (insert != 0)
    {
      *error = 0;
      insert->extrusion.x = point->x;
      insert->extrusion.y = point->y;
      insert->extrusion.z = point->z;
    }
  else
    *error = 1;
}

char
dwg_ent_insert_get_has_attribs(dwg_ent_insert *insert, int *error)
{    
  if (insert != 0)
    {
      *error = 0;
      return insert->has_attribs;
    }
  else
    *error = 1;
}

void
dwg_ent_insert_set_has_attribs(dwg_ent_insert *insert, char attribs,
                               int *error)
{    
  if (insert != 0)
    {
      *error = 0;
      insert->has_attribs = attribs;
    }
  else
    *error = 1;
}

long
dwg_ent_insert_get_owned_obj_count(dwg_ent_insert *insert, int *error)
{    
  if (insert != 0)
    {
      *error = 0;
      return insert->owned_obj_count;
    }
  else
    *error = 1;
}

void
dwg_ent_insert_set_owned_obj_count(dwg_ent_insert *insert, long count,
                                   int *error)
{    
  if (insert != 0)
    {
      *error = 0;
      insert->owned_obj_count = count;
    }
  else
    *error = 1;
}

dwg_handle
dwg_ent_insert_get_ref_handle(dwg_ent_insert *insert, int *error)
{
  if(insert != 0)
    {
      *error = 0;
      return insert->block_header->handleref;
    }
  else
    *error = 1;
}

unsigned long
dwg_ent_insert_get_abs_ref(dwg_ent_insert *insert, int *error)
{
  if(insert != 0)
    {
      *error = 0;
      return insert->block_header->absolute_ref;
    }
  else
    *error = 1;
}

/*******************************************************************
*                  FUNCTIONS FOR MINSERT ENTITY                     *
********************************************************************/

void
dwg_ent_minsert_get_ins_pt(dwg_ent_minsert *minsert, dwg_point_3d *point,
                           int *error)
{    
  if (minsert != 0)
    {
      *error = 0;
      point->x = minsert->ins_pt.x;
      point->y = minsert->ins_pt.y;
      point->z = minsert->ins_pt.z;
    }
  else
    *error = 1;
}

void
dwg_ent_minsert_set_ins_pt(dwg_ent_minsert *minsert, dwg_point_3d *point,
                           int *error)
{    
  if (minsert != 0)
    {
      *error = 0;
      minsert->ins_pt.x = point->x;
      minsert->ins_pt.y = point->y;
      minsert->ins_pt.z = point->z;
    }
  else
    *error = 1;
}

char 
dwg_ent_minsert_get_scale_flag(dwg_ent_minsert *minsert, int *error)
{    
  if (minsert != 0)
    {
      *error = 0;
      return minsert->scale_flag;
    }
  else
    *error = 1;
}

void
dwg_ent_minsert_set_scale_flag(dwg_ent_minsert *minsert, char flags,
                               int *error)
{    
  if (minsert != 0)
    {
      *error = 0;
      minsert->scale_flag = flags;
    }
  else
    *error = 1;
}

void
dwg_ent_minsert_get_scale(dwg_ent_minsert *minsert, dwg_point_3d *point,
                          int *error)
{    
  if (minsert != 0)
    {
      *error = 0;
      point->x = minsert->scale.x;
      point->y = minsert->scale.y;
      point->z = minsert->scale.z;
    }
  else
    *error = 1;
}

void
dwg_ent_minsert_set_scale(dwg_ent_minsert *minsert, dwg_point_3d *point, 
                          int *error)
{    
  if (minsert != 0)
    {
      *error = 0;
      minsert->scale.x = point->x;
      minsert->scale.y = point->y;
      minsert->scale.z = point->z;
    }
  else
    *error = 1;
}

double
dwg_ent_minsert_get_rotation_angle(dwg_ent_minsert *minsert, int *error)
{    
  if (minsert != 0)
    {
      *error = 0;
      return minsert->rotation_ang;
    }
  else
    *error = 1;
}

void
dwg_ent_minsert_set_rotation_angle(dwg_ent_minsert *minsert, double rot_ang,
                                   int *error)
{    
  if (minsert != 0)
    {
      *error = 0;
      minsert->rotation_ang = rot_ang;
    }
  else
    *error = 1;
}

void
dwg_ent_minsert_get_extrusion(dwg_ent_minsert *minsert, dwg_point_3d *point,
                              int *error)
{    
  if (minsert != 0)
    {
      *error = 0;
      point->x = minsert->extrusion.x;
      point->y = minsert->extrusion.y;
      point->z = minsert->extrusion.z;
    }
  else
    *error = 1;
}

void
dwg_ent_minsert_set_extrusion(dwg_ent_minsert *minsert, dwg_point_3d *point,
                              int *error)
{    
  if (minsert != 0)
    {
      *error = 0;
      minsert->extrusion.x = point->x;
      minsert->extrusion.y = point->y;
      minsert->extrusion.z = point->z;
    }
  else
    *error = 1;
}

char 
dwg_ent_minsert_get_has_attribs(dwg_ent_minsert *minsert, int *error)
{    
  if (minsert != 0)
    {
      *error = 0;
      return minsert->has_attribs;
    }
  else
    *error = 1;
}

void
dwg_ent_minsert_set_has_attribs(dwg_ent_minsert *minsert, char attribs,
                                int *error)
{    
  if (minsert != 0)
    {
      *error = 0;
      minsert->has_attribs = attribs;
    }
  else
    *error = 1;
}

long
dwg_ent_minsert_get_owned_obj_count(dwg_ent_minsert *minsert, int *error)
{    
  if (minsert != 0)
    {
      *error = 0;
      return minsert->owned_obj_count;
    }
  else
    *error = 1;
}

void
dwg_ent_minsert_set_owned_obj_count(dwg_ent_minsert *minsert, long count,
                                    int *error)
{    
  if (minsert != 0)
    {
      *error = 0;
      minsert->owned_obj_count = count;
    }
  else
    *error = 1;
}

long
dwg_ent_minsert_get_numcols(dwg_ent_minsert *minsert, int *error)
{    
  if (minsert != 0)
    {
      *error = 0;
      return minsert->numcols;
    }
  else
    *error = 1;
}

void
dwg_ent_minsert_set_numcols(dwg_ent_minsert *minsert, long cols, int *error)
{    
  if (minsert != 0)
    {
      *error = 0;
      minsert->numcols = cols;
    }
  else
    *error = 1;
}

long
dwg_ent_minsert_get_numrows(dwg_ent_minsert *minsert, int *error)
{    
  if (minsert != 0)
    {
      *error = 0;
      return minsert->numrows;
    }
  else
    *error = 1;
}

void
dwg_ent_minsert_set_numrows(dwg_ent_minsert *minsert, long cols, int *error)
{    
  if (minsert != 0)
    {
      *error = 0;
      minsert->numrows = cols;
    }
  else
    *error = 1;
}

double
dwg_ent_minsert_get_col_spacing(dwg_ent_minsert *minsert, int *error)
{    
  if (minsert != 0)
    {
      *error = 0;
      return minsert->col_spacing;
    }
  else
    *error = 1;
}

void
dwg_ent_minsert_set_col_spacing(dwg_ent_minsert *minsert, double spacing,
                                int *error)
{    
  if (minsert != 0)
    {
      *error = 0;
      minsert->col_spacing = spacing;
    }
  else
    *error = 1;
}

double
dwg_ent_minsert_get_row_spacing(dwg_ent_minsert *minsert, int *error)
{    
  if (minsert != 0)
    {
      *error = 0;
      return minsert->row_spacing;
    }
  else
    *error = 1;
}

void
dwg_ent_minsert_set_row_spacing(dwg_ent_minsert *minsert, double spacing,
                                int *error)
{    
  if (minsert != 0)
    {
      *error = 0;
      minsert->row_spacing = spacing;
    }
  else
    *error = 1;
}

/*******************************************************************
*                FUNCTIONS FOR MLINESTYLE OBJECT                    *
********************************************************************/

char *
dwg_obj_mlinstyle_get_name(dwg_obj_mlinestyle *mlinestyle, int *error)
{    
  if (mlinestyle != 0)
    {
      *error = 0;
      return mlinestyle->name;
    }
  else
    *error = 1;
}

void
dwg_obj_mlinestyle_set_name(dwg_obj_mlinestyle *mlinestyle, char * name,
                            int *error)
{    
  if (mlinestyle != 0)
    {
      *error = 0;
      mlinestyle->name = name;
    }
  else
    *error = 1;
}

char *
dwg_obj_mlinestyle_get_desc(dwg_obj_mlinestyle *mlinestyle, int *error)
{    
  if (mlinestyle != 0)
    {
      *error = 0;
      return mlinestyle->desc;
    }
  else
    *error = 1;
}

void
dwg_obj_mlinestyle_set_desc(dwg_obj_mlinestyle *mlinestyle, char * desc,
                            int *error)
{    
  if (mlinestyle != 0)
    {
      *error = 0;
      mlinestyle->desc = desc;
    }
  else
    *error = 1;
}

int
dwg_obj_mlinestyle_get_flags(dwg_obj_mlinestyle *mlinestyle, int *error)
{    
  if (mlinestyle != 0)
    {
      *error = 0;
      return mlinestyle->flags;
    }
  else
    *error = 1;
}

void
dwg_obj_mlinestyle_set_flags(dwg_obj_mlinestyle *mlinestyle, int flags,
                             int *error)
{    
  if (mlinestyle != 0)
    {
      *error = 0;
      mlinestyle->flags = flags;
    }
  else
    *error = 1;
}

double
dwg_obj_mlinestyle_get_start_angle(dwg_obj_mlinestyle *mlinestyle, int *error)
{    
  if (mlinestyle != 0)
    {
      *error = 0;
      return mlinestyle->startang;
    }
  else
    *error = 1;
}

void
dwg_obj_mlinestyle_set_start_angle(dwg_obj_mlinestyle *mlinestyle,
                                   double startang, int *error)
{    
  if (mlinestyle != 0)
    {
      *error = 0;
      mlinestyle->startang = startang;
    }
  else
    *error = 1;
}

double
dwg_obj_mlinestyle_get_end_angle(dwg_obj_mlinestyle *mlinestyle, int *error)
{    
  if (mlinestyle != 0)
    {
      *error = 0;
      return mlinestyle->endang;
    }
  else
    *error = 1;
}

void
dwg_obj_mlinestyle_set_end_angle(dwg_obj_mlinestyle *mlinestyle, double endang,
                                 int *error)
{    
  if (mlinestyle != 0)
    {
      *error = 0;
      mlinestyle->endang = endang;
    }
  else
    *error = 1;
}

char
dwg_obj_mlinestyle_get_linesinstyle(dwg_obj_mlinestyle *mlinestyle, 
                                    int *error)
{    
  if (mlinestyle != 0)
    {
      *error = 0;
      return mlinestyle->linesinstyle;
    }
  else
    *error = 1;
}

void
dwg_obj_mlinestyle_set_linesinstyle(dwg_obj_mlinestyle *mlinestyle,
                                    char linesinstyle, int *error)
{    
  if (mlinestyle != 0)
    {
      *error = 0;
      mlinestyle->linesinstyle = linesinstyle;
    }
  else
    *error = 1;
}

/*******************************************************************
*               FUNCTIONS FOR APPID_CONTROL OBJECT                  *
********************************************************************/

int
dwg_obj_appid_control_get_num_entries(dwg_obj_appid_control *appid, int *error)
{
  if (appid != 0)
    {
      *error = 0;
      return appid->num_entries;
    }
  else
    *error = 1;

}

void
dwg_obj_appid_control_set_num_entries(dwg_obj_appid_control *appid,
                                      int entries, int *error)
{
  if (appid != 0)
    {
      *error = 0;
      appid->num_entries = entries;
    }
  else
    *error = 1;

}

/*******************************************************************
*                    FUNCTIONS FOR APPID OBJECT                     *
********************************************************************/

char *
dwg_obj_appid_get_entry_name(dwg_obj_appid *appid, int *error)
{
  if (appid != 0)
    {
      *error = 0;
      return appid->entry_name;
    }
  else
    *error = 1;

}

void
dwg_obj_appid_set_entry_name(dwg_obj_appid *appid, char * entry_name,
                             int *error)
{
  if (appid != 0)
    {
      *error = 0;
      appid->entry_name = entry_name;
    }
  else
    *error = 1;

}

char
dwg_obj_appid_get_flag(dwg_obj_appid *appid, int *error)
{
  if (appid != 0)
    {
      *error = 0;
      return appid->_64_flag;
    }
  else
    *error = 1;

}

void
dwg_obj_appid_set_flag(dwg_obj_appid *appid, char flag, int *error)
{
  if (appid != 0)
    {
      *error = 0;
      appid->_64_flag = flag;
    }
  else
    *error = 1;

}

dwg_obj_appid_control
dwg_obj_appid_get_appid_control(dwg_obj_appid *appid, int *error)
{
  if (appid != 0)
    {
      *error = 0;
      appid->app_control->obj->tio.object->tio.APPID_CONTROL;
    }
  else
    *error = 1;

}

/*******************************************************************
*            FUNCTIONS FOR ORDINATE DIMENSION ENTITY                *
********************************************************************/

double
dwg_ent_dim_ordinate_get_elevation_ecs11(dwg_ent_dim_ordinate *dim, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      return dim->elevation.ecs_11;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ordinate_set_elevation_ecs11(dwg_ent_dim_ordinate *dim,
                                         double elevation_ecs11, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->elevation.ecs_11 = elevation_ecs11;
    }
  else
    *error = 1;
}

double
dwg_ent_dim_ordinate_get_elevation_ecs12(dwg_ent_dim_ordinate *dim, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      return dim->elevation.ecs_12;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ordinate_set_elevation_ecs12(dwg_ent_dim_ordinate *dim,
                                         double elevation_ecs12, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->elevation.ecs_12 = elevation_ecs12;
    }
  else
    *error = 1;
}

char
dwg_ent_dim_ordinate_get_flags1(dwg_ent_dim_ordinate *dim, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      return dim->flags_1;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ordinate_set_flags1(dwg_ent_dim_ordinate *dim, char flag,
                                int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->flags_1 = flag;
    }
  else
    *error = 1;
}

double
dwg_ent_dim_ordinate_get_act_measurement(dwg_ent_dim_ordinate *dim, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      return dim->act_measurement;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ordinate_set_act_measurement(dwg_ent_dim_ordinate *dim,
                                         double act_measurement, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->act_measurement = act_measurement;
    }
  else
    *error = 1;
}

double
dwg_ent_dim_ordinate_get_horiz_dir(dwg_ent_dim_ordinate *dim, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      return dim->horiz_dir;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ordinate_set_horiz_dir(dwg_ent_dim_ordinate *dim, double horiz_dir,
                                   int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->horiz_dir = horiz_dir;
    }
  else
    *error = 1;
}

double
dwg_ent_dim_ordinate_get_lspace_factor(dwg_ent_dim_ordinate *dim, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      return dim->lspace_factor;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ordinate_set_lspace_factor(dwg_ent_dim_ordinate *dim,
                                       double factor, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->lspace_factor = factor;
    }
  else
    *error = 1;
}

unsigned int
dwg_ent_dim_ordinate_get_lspace_style(dwg_ent_dim_ordinate *dim,
                                      int *error)
{
  if (dim != 0)
    {
      *error = 0;
      return dim->lspace_style;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ordinate_set_lspace_style(dwg_ent_dim_ordinate *dim,
                                      unsigned int style, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->lspace_style = style;
    }
  else
    *error = 1;
}

unsigned int
dwg_ent_dim_ordinate_get_attachment_point(dwg_ent_dim_ordinate *dim,
                                          int *error)
{
  if (dim != 0)
    {
      *error = 0;
      return dim->attachment_point;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ordinate_set_attachment_point(dwg_ent_dim_ordinate *dim,
                                          unsigned int point, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->attachment_point = point;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ordinate_set_extrusion(dwg_ent_dim_ordinate *dim,
                                   dwg_point_3d *point, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->extrusion.x = point->x;
      dim->extrusion.y = point->y;
      dim->extrusion.z = point->z;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ordinate_get_extrusion(dwg_ent_dim_ordinate *dim,
                                   dwg_point_3d *point, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      point->x = dim->extrusion.x;
      point->y = dim->extrusion.y;
      point->z = dim->extrusion.z;
    }
  else
    *error = 1;
}

char *
dwg_ent_dim_ordinate_get_user_text(dwg_ent_dim_ordinate *dim, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      return dim->user_text;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ordinate_set_user_text(dwg_ent_dim_ordinate *dim, char * text,
                                   int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->user_text = text;
    }
  else
    *error = 1;
}

double
dwg_ent_dim_ordinate_get_text_rot(dwg_ent_dim_ordinate *dim, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      return dim->text_rot;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ordinate_set_text_rot(dwg_ent_dim_ordinate *dim, double rot,
                                  int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->text_rot = rot;
    }
  else
    *error = 1;
}

double
dwg_ent_dim_ordinate_get_ins_rotation(dwg_ent_dim_ordinate *dim, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      return dim->ins_rotation;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ordinate_set_ins_rotation(dwg_ent_dim_ordinate *dim, double rot,
                                      int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->ins_rotation = rot;
    }
  else
    *error = 1;
}

char
dwg_ent_dim_ordinate_get_flip_arrow1(dwg_ent_dim_ordinate *dim, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      return dim->flip_arrow1;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ordinate_set_flip_arrow1(dwg_ent_dim_ordinate *dim,
                                     char flip_arrow, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->flip_arrow1 = flip_arrow;
    }
  else
    *error = 1;
}

char
dwg_ent_dim_ordinate_get_flip_arrow2(dwg_ent_dim_ordinate *dim, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      return dim->flip_arrow2;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ordinate_set_flip_arrow2(dwg_ent_dim_ordinate *dim,
                                     char flip_arrow, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->flip_arrow2 = flip_arrow;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ordinate_set_text_mid_pt(dwg_ent_dim_ordinate *dim,
                                          dwg_point_2d *point, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->text_midpt.x = point->x;
      dim->text_midpt.y = point->y;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ordinate_get_text_mid_pt(dwg_ent_dim_ordinate *dim,
                                          dwg_point_2d *point, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      point->x = dim->text_midpt.x;
      point->y = dim->text_midpt.y;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ordinate_set_ins_scale(dwg_ent_dim_ordinate *dim,
                                   dwg_point_3d *point, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->ins_scale.x = point->x;
      dim->ins_scale.y = point->y;
      dim->ins_scale.z = point->z;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ordinate_get_ins_scale(dwg_ent_dim_ordinate *dim,
                                   dwg_point_3d *point, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      point->x = dim->ins_scale.x;
      point->y = dim->ins_scale.y;
      point->z = dim->ins_scale.z;
    }
  else
    *error = 1;
}

char
dwg_ent_dim_ordinate_get_flags2(dwg_ent_dim_ordinate *dim, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      return dim->flags_2;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ordinate_set_flags2(dwg_ent_dim_ordinate *dim, char flag,
                                int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->flags_2 = flag;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ordinate_set_12_pt(dwg_ent_dim_ordinate *dim, dwg_point_2d *point,
                               int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->_12_pt.x = point->x;
      dim->_12_pt.y = point->y;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ordinate_get_12_pt(dwg_ent_dim_ordinate *dim, dwg_point_2d *point,
                               int *error)
{
  if (dim != 0)
    {
      *error = 0;
      point->x = dim->_12_pt.x;
      point->y = dim->_12_pt.y;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ordinate_set_10_pt(dwg_ent_dim_ordinate *dim, dwg_point_3d *point,
                               int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->_10_pt.x = point->x;
      dim->_10_pt.y = point->y;
      dim->_10_pt.z = point->z;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ordinate_get_10_pt(dwg_ent_dim_ordinate *dim, dwg_point_3d *point,
                               int *error)
{
  if (dim != 0)
    {
      *error = 0;
      point->x = dim->_10_pt.x;
      point->y = dim->_10_pt.y;
      point->z = dim->_10_pt.z;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ordinate_set_13_pt(dwg_ent_dim_ordinate *dim, dwg_point_3d *point,
                               int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->_13_pt.x = point->x;
      dim->_13_pt.y = point->y;
      dim->_13_pt.z = point->z;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ordinate_get_13_pt(dwg_ent_dim_ordinate *dim, dwg_point_3d *point,
                               int *error)
{
  if (dim != 0)
    {
      *error = 0;
      point->x = dim->_13_pt.x;
      point->y = dim->_13_pt.y;
      point->z = dim->_13_pt.z;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ordinate_set_14_pt(dwg_ent_dim_ordinate *dim, dwg_point_3d *point,
                               int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->_14_pt.x = point->x;
      dim->_14_pt.y = point->y;
      dim->_14_pt.z = point->z;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ordinate_get_14_pt(dwg_ent_dim_ordinate *dim, dwg_point_3d *point,
                               int *error)
{
  if (dim != 0)
    {
      *error = 0;
      point->x = dim->_14_pt.x;
      point->y = dim->_14_pt.y;
      point->z = dim->_14_pt.z;
    }
  else
    *error = 1;
}

/*******************************************************************
*              FUNCTIONS FOR LINEAR DIMENSION ENTITY                *
********************************************************************/

char *
dwg_ent_dim_linear_get_block_name(dwg_ent_dim_linear *dim, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      return dim->block->obj->tio.object->tio.BLOCK_HEADER->entry_name;
    }
  else
    *error = 1;
}

double
dwg_ent_dim_linear_get_elevation_ecs11(dwg_ent_dim_linear *dim, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      return dim->elevation.ecs_11;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_linear_set_elevation_ecs11(dwg_ent_dim_linear *dim,
                                       double elevation_ecs11, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->elevation.ecs_11 = elevation_ecs11;
    }
  else
    *error = 1;
}

double
dwg_ent_dim_linear_get_elevation_ecs12(dwg_ent_dim_linear *dim, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      return dim->elevation.ecs_12;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_linear_set_elevation_ecs12(dwg_ent_dim_linear *dim,
                                       double elevation_ecs12, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->elevation.ecs_12 = elevation_ecs12;
    }
  else
    *error = 1;
}

char
dwg_ent_dim_linear_get_flags1(dwg_ent_dim_linear *dim, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      return dim->flags_1;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_linear_set_flags1(dwg_ent_dim_linear *dim, char flag, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->flags_1 = flag;
    }
  else
    *error = 1;
}

double
dwg_ent_dim_linear_get_act_measurement(dwg_ent_dim_linear *dim, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      return dim->act_measurement;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_linear_set_act_measurement(dwg_ent_dim_linear *dim,
                                       double act_measurement, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->act_measurement = act_measurement;
    }
  else
    *error = 1;
}

double
dwg_ent_dim_linear_get_horiz_dir(dwg_ent_dim_linear *dim, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      return dim->horiz_dir;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_linear_set_horiz_dir(dwg_ent_dim_linear *dim,
                                 double horiz_dir, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->horiz_dir = horiz_dir;
    }
  else
    *error = 1;
}

double
dwg_ent_dim_linear_get_lspace_factor(dwg_ent_dim_linear *dim,
                                     int *error)
{
  if (dim != 0)
    {
      *error = 0;
      return dim->lspace_factor;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_linear_set_lspace_factor(dwg_ent_dim_linear *dim, double factor,
                                     int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->lspace_factor = factor;
    }
  else
    *error = 1;
}

unsigned int
dwg_ent_dim_linear_get_lspace_style(dwg_ent_dim_linear *dim, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      return dim->lspace_style;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_linear_set_lspace_style(dwg_ent_dim_linear *dim,
                                    unsigned int style, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->lspace_style = style;
    }
  else
    *error = 1;
}

unsigned int
dwg_ent_dim_linear_get_attachment_point(dwg_ent_dim_linear *dim, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      return dim->attachment_point;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_linear_set_attachment_point(dwg_ent_dim_linear *dim,
                                        unsigned int point, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->attachment_point = point;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_linear_set_extrusion(dwg_ent_dim_linear *dim, dwg_point_3d *point,
                                 int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->extrusion.x = point->x;
      dim->extrusion.y = point->y;
      dim->extrusion.z = point->z;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_linear_get_extrusion(dwg_ent_dim_linear *dim, dwg_point_3d *point,
                                 int *error)
{
  if (dim != 0)
    {
      *error = 0;
      point->x = dim->extrusion.x;
      point->y = dim->extrusion.y;
      point->z = dim->extrusion.z;
    }
  else
    *error = 1;
}

char *
dwg_ent_dim_linear_get_user_text(dwg_ent_dim_linear *dim, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      return dim->user_text;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_linear_set_user_text(dwg_ent_dim_linear *dim, char * text,
                                 int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->user_text = text;
    }
  else
    *error = 1;
}

double
dwg_ent_dim_linear_get_text_rot(dwg_ent_dim_linear *dim, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      return dim->text_rot;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_linear_set_text_rot(dwg_ent_dim_linear *dim, double rot,
                                int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->text_rot = rot;
    }
  else
    *error = 1;
}

double
dwg_ent_dim_linear_get_ins_rotation(dwg_ent_dim_linear *dim, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      return dim->ins_rotation;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_linear_set_ins_rotation(dwg_ent_dim_linear *dim, double rot,
                                    int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->ins_rotation = rot;
    }
  else
    *error = 1;
}

char
dwg_ent_dim_linear_get_flip_arrow1(dwg_ent_dim_linear *dim, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      return dim->flip_arrow1;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_linear_set_flip_arrow1(dwg_ent_dim_linear *dim, char flip_arrow,
                                   int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->flip_arrow1 = flip_arrow;
    }
  else
    *error = 1;
}

char
dwg_ent_dim_linear_get_flip_arrow2(dwg_ent_dim_linear *dim, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      return dim->flip_arrow2;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_linear_set_flip_arrow2(dwg_ent_dim_linear *dim, char flip_arrow,
                                   int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->flip_arrow2 = flip_arrow;
    }
  else
    *error = 1;
}

void 
dwg_ent_dim_linear_set_text_mid_pt(dwg_ent_dim_linear *dim,
                                   dwg_point_2d *point, 
                                   int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->text_midpt.x = point->x;
      dim->text_midpt.y = point->y;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_linear_get_text_mid_pt(dwg_ent_dim_linear *dim,
                                   dwg_point_2d *point, 
                                   int *error)
{
  if (dim != 0)
    {
      *error = 0;
      point->x = dim->text_midpt.x;
      point->y = dim->text_midpt.y;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_linear_set_ins_scale(dwg_ent_dim_linear *dim, dwg_point_3d *point, 
                                 int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->ins_scale.x = point->x;
      dim->ins_scale.y = point->y;
      dim->ins_scale.z = point->z;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_linear_get_ins_scale(dwg_ent_dim_linear *dim, dwg_point_3d *point, 
                                 int *error)
{
  if (dim != 0)
    {
      *error = 0;
      point->x = dim->ins_scale.x;
      point->y = dim->ins_scale.y;
      point->z = dim->ins_scale.z;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_linear_set_12_pt(dwg_ent_dim_linear *dim, dwg_point_2d *point, 
                             int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->_12_pt.x = point->x;
      dim->_12_pt.y = point->y;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_linear_get_12_pt(dwg_ent_dim_linear *dim, dwg_point_2d *point, 
                             int *error)
{
  if (dim != 0)
    {
      *error = 0;
      point->x = dim->_12_pt.x;
      point->y = dim->_12_pt.y;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_linear_set_10_pt(dwg_ent_dim_linear *dim, dwg_point_3d *point, 
                             int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->_10_pt.x = point->x;
      dim->_10_pt.y = point->y;
      dim->_10_pt.z = point->z;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_linear_get_10_pt(dwg_ent_dim_linear *dim, dwg_point_3d *point, 
                             int *error)
{
  if (dim != 0)
    {
      *error = 0;
      point->x = dim->_10_pt.x;
      point->y = dim->_10_pt.y;
      point->z = dim->_10_pt.z;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_linear_set_13_pt(dwg_ent_dim_linear *dim, dwg_point_3d *point, 
                             int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->_13_pt.x = point->x;
      dim->_13_pt.y = point->y;
      dim->_13_pt.z = point->z;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_linear_get_13_pt(dwg_ent_dim_linear *dim, dwg_point_3d *point,
                             int *error)
{
  if (dim != 0)
    {
      *error = 0;
      point->x = dim->_13_pt.x;
      point->y = dim->_13_pt.y;
      point->z = dim->_13_pt.z;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_linear_set_14_pt(dwg_ent_dim_linear *dim, dwg_point_3d *point,
                             int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->_14_pt.x = point->x;
      dim->_14_pt.y = point->y;
      dim->_14_pt.z = point->z;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_linear_get_14_pt(dwg_ent_dim_linear *dim, dwg_point_3d *point,
                             int *error)
{
  if (dim != 0)
    {
      *error = 0;
      point->x = dim->_14_pt.x;
      point->y = dim->_14_pt.y;
      point->z = dim->_14_pt.z;
    }
  else
    *error = 1;
}

double
dwg_ent_dim_linear_get_ext_line_rotation(dwg_ent_dim_linear *dim, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      return dim->ext_line_rot;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_linear_set_ext_line_rotation(dwg_ent_dim_linear *dim, double rot,
                                         int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->ext_line_rot = rot;
    }
  else
    *error = 1;
}

double
dwg_ent_dim_linear_get_dim_rot(dwg_ent_dim_linear *dim, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      return dim->dim_rot;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_linear_set_dim_rot(dwg_ent_dim_linear *dim, double rot, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->dim_rot = rot;
    }
  else
    *error = 1;
}

/*******************************************************************
*             FUNCTIONS FOR ALIGNED DIMENSION ENTITY                *
********************************************************************/

double
dwg_ent_dim_aligned_get_elevation_ecs11(dwg_ent_dim_aligned *dim, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      return dim->elevation.ecs_11;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_aligned_set_elevation_ecs11(dwg_ent_dim_aligned *dim,
                                        double elevation_ecs11,
                                        int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->elevation.ecs_11 = elevation_ecs11;
    }
  else
    *error = 1;
}

double
dwg_ent_dim_aligned_get_elevation_ecs12(dwg_ent_dim_aligned *dim, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      return dim->elevation.ecs_12;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_aligned_set_elevation_ecs12(dwg_ent_dim_aligned *dim,
                                        double elevation_ecs12,
                                        int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->elevation.ecs_12 = elevation_ecs12;
    }
  else
    *error = 1;
}

char
dwg_ent_dim_aligned_get_flags1(dwg_ent_dim_aligned *dim, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      return dim->flags_1;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_aligned_set_flags1(dwg_ent_dim_aligned *dim, char flag, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->flags_1 = flag;
    }
  else
    *error = 1;
}

double
dwg_ent_dim_aligned_get_act_measurement(dwg_ent_dim_aligned *dim, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      return dim->act_measurement;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_aligned_set_act_measurement(dwg_ent_dim_aligned *dim,
                                        double act_measurement, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->act_measurement = act_measurement;
    }
  else
    *error = 1;
}

double
dwg_ent_dim_aligned_get_horiz_dir(dwg_ent_dim_aligned *dim, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      return dim->horiz_dir;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_aligned_set_horiz_dir(dwg_ent_dim_aligned *dim, double horiz_dir,
                                  int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->horiz_dir = horiz_dir;
    }
  else
    *error = 1;
}

double
dwg_ent_dim_aligned_get_lspace_factor(dwg_ent_dim_aligned *dim, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      return dim->lspace_factor;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_aligned_set_lspace_factor(dwg_ent_dim_aligned *dim, double factor,
                                      int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->lspace_factor = factor;
    }
  else
    *error = 1;
}

unsigned int
dwg_ent_dim_aligned_get_lspace_style(dwg_ent_dim_aligned *dim, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      return dim->lspace_style;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_aligned_set_lspace_style(dwg_ent_dim_aligned *dim,
                                     unsigned int style, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->lspace_style = style;
    }
  else
    *error = 1;
}

unsigned int
dwg_ent_dim_aligned_get_attachment_point(dwg_ent_dim_aligned *dim, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      return dim->attachment_point;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_aligned_set_attachment_point(dwg_ent_dim_aligned *dim,
                                         unsigned int point, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->attachment_point = point;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_aligned_set_extrusion(dwg_ent_dim_aligned *dim,
                                  dwg_point_3d *point, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->extrusion.x = point->x;
      dim->extrusion.y = point->y;
      dim->extrusion.z = point->z;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_aligned_get_extrusion(dwg_ent_dim_aligned *dim,
                                  dwg_point_3d *point, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      point->x = dim->extrusion.x;
      point->y = dim->extrusion.y;
      point->z = dim->extrusion.z;
    }
  else
    *error = 1;
}

char *
dwg_ent_dim_aligned_get_user_text(dwg_ent_dim_aligned *dim, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      return dim->user_text;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_aligned_set_user_text(dwg_ent_dim_aligned *dim, char * text, 
                                  int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->user_text = text;
    }
  else
    *error = 1;
}

double
dwg_ent_dim_aligned_get_text_rot(dwg_ent_dim_aligned *dim, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      return dim->text_rot;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_aligned_set_text_rot(dwg_ent_dim_aligned *dim, double rot,
                                 int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->text_rot = rot;
    }
  else
    *error = 1;
}

double
dwg_ent_dim_aligned_get_ins_rotation(dwg_ent_dim_aligned *dim, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      return dim->ins_rotation;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_aligned_set_ins_rotation(dwg_ent_dim_aligned *dim, double rot,
                                     int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->ins_rotation = rot;
    }
  else
    *error = 1;
}

char
dwg_ent_dim_aligned_get_flip_arrow1(dwg_ent_dim_aligned *dim, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      return dim->flip_arrow1;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_aligned_set_flip_arrow1(dwg_ent_dim_aligned *dim, char flip_arrow,
                                    int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->flip_arrow1 = flip_arrow;
    }
  else
    *error = 1;
}

char
dwg_ent_dim_aligned_get_flip_arrow2(dwg_ent_dim_aligned *dim, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      return dim->flip_arrow2;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_aligned_set_flip_arrow2(dwg_ent_dim_aligned *dim, char flip_arrow,
                                    int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->flip_arrow2 = flip_arrow;
    }
  else
    *error = 1;
}

void 
dwg_ent_dim_aligned_set_text_mid_pt(dwg_ent_dim_aligned *dim, 
                                    dwg_point_2d *point, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->text_midpt.x = point->x;
      dim->text_midpt.y = point->y;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_aligned_get_text_mid_pt(dwg_ent_dim_aligned *dim,
                                    dwg_point_2d *point, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      point->x = dim->text_midpt.x;
      point->y = dim->text_midpt.y;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_aligned_set_ins_scale(dwg_ent_dim_aligned *dim,
                                  dwg_point_3d *point, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->ins_scale.x = point->x;
      dim->ins_scale.y = point->y;
      dim->ins_scale.z = point->z;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_aligned_get_ins_scale(dwg_ent_dim_aligned *dim,
                                  dwg_point_3d *point, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      point->x = dim->ins_scale.x;
      point->y = dim->ins_scale.y;
      point->z = dim->ins_scale.z;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_aligned_set_12_pt(dwg_ent_dim_aligned *dim, dwg_point_2d *point,
                              int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->_12_pt.x = point->x;
      dim->_12_pt.y = point->y;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_aligned_get_12_pt(dwg_ent_dim_aligned *dim, dwg_point_2d *point,
                              int *error)
{
  if (dim != 0)
    {
      *error = 0;
      point->x = dim->_12_pt.x;
      point->y = dim->_12_pt.y;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_aligned_set_10_pt(dwg_ent_dim_aligned *dim, dwg_point_3d *point,
                              int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->_10_pt.x = point->x;
      dim->_10_pt.y = point->y;
      dim->_10_pt.z = point->z;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_aligned_get_10_pt(dwg_ent_dim_aligned *dim, dwg_point_3d *point,
                              int *error)
{
  if (dim != 0)
    {
      *error = 0;
      point->x = dim->_10_pt.x;
      point->y = dim->_10_pt.y;
      point->z = dim->_10_pt.z;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_aligned_set_13_pt(dwg_ent_dim_aligned *dim, dwg_point_3d *point,
                              int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->_13_pt.x = point->x;
      dim->_13_pt.y = point->y;
      dim->_13_pt.z = point->z;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_aligned_get_13_pt(dwg_ent_dim_aligned *dim, dwg_point_3d *point,
                              int *error)
{
  if (dim != 0)
    {
      *error = 0;
      point->x = dim->_13_pt.x;
      point->y = dim->_13_pt.y;
      point->z = dim->_13_pt.z;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_aligned_set_14_pt(dwg_ent_dim_aligned *dim, dwg_point_3d *point,
                              int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->_14_pt.x = point->x;
      dim->_14_pt.y = point->y;
      dim->_14_pt.z = point->z;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_aligned_get_14_pt(dwg_ent_dim_aligned *dim, dwg_point_3d *point,
                              int *error)
{
  if (dim != 0)
    {
      *error = 0;
      point->x = dim->_14_pt.x;
      point->y = dim->_14_pt.y;
      point->z = dim->_14_pt.z;
    }
  else
    *error = 1;
}

double
dwg_ent_dim_aligned_get_ext_line_rotation(dwg_ent_dim_aligned *dim, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      return dim->ext_line_rot;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_aligned_set_ext_line_rotation(dwg_ent_dim_aligned *dim,
                                          double rot, int *error)
{
  if (dim != 0)
    {
      *error = 0;
      dim->ext_line_rot = rot;
    }
  else
    *error = 1;
}

/*******************************************************************
*              FUNCTIONS FOR ANG3PT DIMENSION ENTITY                *
********************************************************************/

double
dwg_ent_dim_ang3pt_get_elevation_ecs11(dwg_ent_dim_ang3pt *ang, int *error)
{
  if (ang != 0)
    {
      *error = 0;
      return ang->elevation.ecs_11;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ang3pt_set_elevation_ecs11(dwg_ent_dim_ang3pt *ang,
                                       double elevation_ecs11, int *error)
{
  if (ang != 0)
    {
      *error = 0;
      ang->elevation.ecs_11 = elevation_ecs11;
    }
  else
    *error = 1;
}

double
dwg_ent_dim_ang3pt_get_elevation_ecs12(dwg_ent_dim_ang3pt *ang, int *error)
{
  if (ang != 0)
    {
      *error = 0;
      return ang->elevation.ecs_12;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ang3pt_set_elevation_ecs12(dwg_ent_dim_ang3pt *ang,
                                       double elevation_ecs12, int *error)
{
  if (ang != 0)
    {
      *error = 0;
      ang->elevation.ecs_12 = elevation_ecs12;
    }
  else
    *error = 1;
}

char
dwg_ent_dim_ang3pt_get_flags1(dwg_ent_dim_ang3pt *ang, int *error)
{
  if (ang != 0)
    {
      *error = 0;
      return ang->flags_1;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ang3pt_set_flags1(dwg_ent_dim_ang3pt *ang, char flag, int *error)
{
  if (ang != 0)
    {
      *error = 0;
      ang->flags_1 = flag;
    }
  else
    *error = 1;
}

double
dwg_ent_dim_ang3pt_get_act_measurement(dwg_ent_dim_ang3pt *ang, int *error)
{
  if (ang != 0)
    {
      *error = 0;
      return ang->act_measurement;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ang3pt_set_act_measurement(dwg_ent_dim_ang3pt *ang,
                                       double act_measurement, int *error)
{
  if (ang != 0)
    {
      *error = 0;
      ang->act_measurement = act_measurement;
    }
  else
    *error = 1;
}

double
dwg_ent_dim_ang3pt_get_horiz_dir(dwg_ent_dim_ang3pt *ang, int *error)
{
  if (ang != 0)
    {
      *error = 0;
      return ang->horiz_dir;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ang3pt_set_horiz_dir(dwg_ent_dim_ang3pt *ang, double horiz_dir,
                                 int *error)
{
  if (ang != 0)
    {
      *error = 0;
      ang->horiz_dir = horiz_dir;
    }
  else
    *error = 1;
}

double
dwg_ent_dim_ang3pt_get_lspace_factor(dwg_ent_dim_ang3pt *ang, int *error)
{
  if (ang != 0)
    {
      *error = 0;
      return ang->lspace_factor;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ang3pt_set_lspace_factor(dwg_ent_dim_ang3pt *ang, double factor,
                                     int *error)
{
  if (ang != 0)
    {
      *error = 0;
      ang->lspace_factor = factor;
    }
  else
    *error = 1;
}

unsigned int
dwg_ent_dim_ang3pt_get_lspace_style(dwg_ent_dim_ang3pt *ang, int *error)
{
  if (ang != 0)
    {
      *error = 0;
      return ang->lspace_style;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ang3pt_set_lspace_style(dwg_ent_dim_ang3pt *ang,
                                    unsigned int style, int *error)
{
  if (ang != 0)
    {
      *error = 0;
      ang->lspace_style = style;
    }
  else
    *error = 1;
}

unsigned int
dwg_ent_dim_ang3pt_get_attachment_point(dwg_ent_dim_ang3pt *ang, int *error)
{
  if (ang != 0)
    {
      *error = 0;
      return ang->attachment_point;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ang3pt_set_attachment_point(dwg_ent_dim_ang3pt *ang,
                                        unsigned int point, int *error)
{
  if (ang != 0)
    {
      *error = 0;
      ang->attachment_point = point;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ang3pt_set_extrusion(dwg_ent_dim_ang3pt *ang, dwg_point_3d *point,
                                 int *error)
{
  if (ang != 0)
    {
      *error = 0;
      ang->extrusion.x = point->x;
      ang->extrusion.y = point->y;
      ang->extrusion.z = point->z;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ang3pt_get_extrusion(dwg_ent_dim_ang3pt *ang, dwg_point_3d *point,
                                 int *error)
{
  if (ang != 0)
    {
      *error = 0;
      point->x = ang->extrusion.x;
      point->y = ang->extrusion.y;
      point->z = ang->extrusion.z;
    }
  else
    *error = 1;
}

char *
dwg_ent_dim_ang3pt_get_user_text(dwg_ent_dim_ang3pt *ang, int *error)
{
  if (ang != 0)
    {
      *error = 0;
      return ang->user_text;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ang3pt_set_user_text(dwg_ent_dim_ang3pt *ang, char * text,
                                 int *error)
{
  if (ang != 0)
    {
      *error = 0;
      ang->user_text = text;
    }
  else
    *error = 1;
}

double
dwg_ent_dim_ang3pt_get_text_rot(dwg_ent_dim_ang3pt *ang, int *error)
{
  if (ang != 0)
    {
      *error = 0;
      return ang->text_rot;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ang3pt_set_text_rot(dwg_ent_dim_ang3pt *ang, double rot,
                                int *error)
{
  if (ang != 0)
    {
      *error = 0;
      ang->text_rot = rot;
    }
  else
    *error = 1;
}

double
dwg_ent_dim_ang3pt_get_ins_rotation(dwg_ent_dim_ang3pt *ang, int *error)
{
  if (ang != 0)
    {
      *error = 0;
      return ang->ins_rotation;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ang3pt_set_ins_rotation(dwg_ent_dim_ang3pt *ang, double rot,
                                    int *error)
{
  if (ang != 0)
    {
      *error = 0;
      ang->ins_rotation = rot;
    }
  else
    *error = 1;
}

char
dwg_ent_dim_ang3pt_get_flip_arrow1(dwg_ent_dim_ang3pt *ang, int *error)
{
  if (ang != 0)
    {
      *error = 0;
      return ang->flip_arrow1;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ang3pt_set_flip_arrow1(dwg_ent_dim_ang3pt *ang, char flip_arrow,
                                   int *error)
{
  if (ang != 0)
    {
      *error = 0;
      ang->flip_arrow1 = flip_arrow;
    }
  else
    *error = 1;
}

char
dwg_ent_dim_ang3pt_get_flip_arrow2(dwg_ent_dim_ang3pt *ang, int *error)
{
  if (ang != 0)
    {
      *error = 0;
      return ang->flip_arrow2;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ang3pt_set_flip_arrow2(dwg_ent_dim_ang3pt *ang, char flip_arrow,
                                   int *error)
{
  if (ang != 0)
    {
      *error = 0;
      ang->flip_arrow2 = flip_arrow;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ang3pt_set_text_mid_pt(dwg_ent_dim_ang3pt *ang,
                                   dwg_point_2d *point, int *error)
{
  if (ang != 0)
    {
      *error = 0;
      ang->text_midpt.x = point->x;
      ang->text_midpt.y = point->y;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ang3pt_get_text_mid_pt(dwg_ent_dim_ang3pt *ang,
                                   dwg_point_2d *point, int *error)
{
  if (ang != 0)
    {
      *error = 0;
      point->x = ang->text_midpt.x;
      point->y = ang->text_midpt.y;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ang3pt_set_ins_scale(dwg_ent_dim_ang3pt *ang, dwg_point_3d *point,
                                 int *error)
{
  if (ang != 0)
    {
      *error = 0;
      ang->ins_scale.x = point->x;
      ang->ins_scale.y = point->y;
      ang->ins_scale.z = point->z;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ang3pt_get_ins_scale(dwg_ent_dim_ang3pt *ang, dwg_point_3d *point,
                                 int *error)
{
  if (ang != 0)
    {
      *error = 0;
      point->x = ang->ins_scale.x;
      point->y = ang->ins_scale.y;
      point->z = ang->ins_scale.z;
    }
  else
    *error = 1;
}


void
dwg_ent_dim_ang3pt_set_12_pt(dwg_ent_dim_ang3pt *ang, dwg_point_2d *point,
                             int *error)
{
  if (ang != 0)
    {
      *error = 0;
      ang->_12_pt.x = point->x;
      ang->_12_pt.y = point->y;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ang3pt_get_12_pt(dwg_ent_dim_ang3pt *ang, dwg_point_2d *point,
                             int *error)
{
  if (ang != 0)
    {
      *error = 0;
      point->x = ang->_12_pt.x;
      point->y = ang->_12_pt.y;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ang3pt_set_10_pt(dwg_ent_dim_ang3pt *ang, dwg_point_3d *point,
                             int *error)
{
  if (ang != 0)
    {
      *error = 0;
      ang->_10_pt.x = point->x;
      ang->_10_pt.y = point->y;
      ang->_10_pt.z = point->z;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ang3pt_get_10_pt(dwg_ent_dim_ang3pt *ang, dwg_point_3d *point,
                             int *error)
{
  if (ang != 0)
    {
      *error = 0;
      point->x = ang->_10_pt.x;
      point->y = ang->_10_pt.y;
      point->z = ang->_10_pt.z;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ang3pt_set_13_pt(dwg_ent_dim_ang3pt *ang, dwg_point_3d *point,
                            int *error)
{
  if (ang != 0)
    {
      *error = 0;
      ang->_13_pt.x = point->x;
      ang->_13_pt.y = point->y;
      ang->_13_pt.z = point->z;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ang3pt_get_13_pt(dwg_ent_dim_ang3pt *ang, dwg_point_3d *point, 
                             int *error)
{
  if (ang != 0)
    {
      *error = 0;
      point->x = ang->_13_pt.x;
      point->y = ang->_13_pt.y;
      point->z = ang->_13_pt.z;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ang3pt_set_14_pt(dwg_ent_dim_ang3pt *ang, dwg_point_3d *point,
                             int *error)
{
  if (ang != 0)
    {
      *error = 0;
      ang->_14_pt.x = point->x;
      ang->_14_pt.y = point->y;
      ang->_14_pt.z = point->z;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ang3pt_get_14_pt(dwg_ent_dim_ang3pt *ang, dwg_point_3d *point,
                             int *error)
{
  if (ang != 0)
    {
      *error = 0;
      point->x = ang->_14_pt.x;
      point->y = ang->_14_pt.y;
      point->z = ang->_14_pt.z;
    }
  else
    *error = 1;
}


void
dwg_ent_dim_ang3pt_set_15_pt(dwg_ent_dim_ang3pt *ang, dwg_point_3d *point,
                             int *error)
{
  if (ang != 0)
    {
      *error = 0;
      ang->_15_pt.x = point->x;
      ang->_15_pt.y = point->y;
      ang->_15_pt.z = point->z;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ang3pt_get_15_pt(dwg_ent_dim_ang3pt *ang, dwg_point_3d *point,
                             int *error)
{
  if (ang != 0)
    {
      *error = 0;
      point->x = ang->_15_pt.x;
      point->y = ang->_15_pt.y;
      point->z = ang->_15_pt.z;
    }
  else
    *error = 1;
}

/*******************************************************************
*              FUNCTIONS FOR ANG2LN DIMENSION ENTITY                *
********************************************************************/

double
dwg_ent_dim_ang2ln_get_elevation_ecs11(dwg_ent_dim_ang2ln *ang, int *error)
{
  if (ang != 0)
    {
      *error = 0;
      return ang->elevation.ecs_11;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ang2ln_set_elevation_ecs11(dwg_ent_dim_ang2ln *ang,
                                       double elevation_ecs11, int *error)
{
  if (ang != 0)
    {
      *error = 0;
      ang->elevation.ecs_11 = elevation_ecs11;
    }
  else
    *error = 1;
}

double
dwg_ent_dim_ang2ln_get_elevation_ecs12(dwg_ent_dim_ang2ln *ang, int *error)
{
  if (ang != 0)
    {
      *error = 0;
      return ang->elevation.ecs_12;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ang2ln_set_elevation_ecs12(dwg_ent_dim_ang2ln *ang,
                                       double elevation_ecs12, int *error)
{
  if (ang != 0)
    {
      *error = 0;
      ang->elevation.ecs_12 = elevation_ecs12;
    }
  else
    *error = 1;
}

char
dwg_ent_dim_ang2ln_get_flags1(dwg_ent_dim_ang2ln *ang, int *error)
{
  if (ang != 0)
    {
      *error = 0;
      return ang->flags_1;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ang2ln_set_flags1(dwg_ent_dim_ang2ln *ang, char flag, int *error)
{
  if (ang != 0)
    {
      *error = 0;
      ang->flags_1 = flag;
    }
  else
    *error = 1;
}

double
dwg_ent_dim_ang2ln_get_act_measurement(dwg_ent_dim_ang2ln *ang,
                                       int *error)
{
  if (ang != 0)
    {
      *error = 0;
      return ang->act_measurement;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ang2ln_set_act_measurement(dwg_ent_dim_ang2ln *ang,
                                       double act_measurement, int *error)
{
  if (ang != 0)
    {
      *error = 0;
      ang->act_measurement = act_measurement;
    }
  else
    *error = 1;
}

double
dwg_ent_dim_ang2ln_get_horiz_dir(dwg_ent_dim_ang2ln *ang,
                                 int *error)
{
  if (ang != 0)
    {
      *error = 0;
      return ang->horiz_dir;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ang2ln_set_horiz_dir(dwg_ent_dim_ang2ln *ang,
                                 double horiz_dir, int *error)
{
  if (ang != 0)
    {
      *error = 0;
      ang->horiz_dir = horiz_dir;
    }
  else
    *error = 1;
}

double
dwg_ent_dim_ang2ln_get_lspace_factor(dwg_ent_dim_ang2ln *ang,
                                     int *error)
{
  if (ang != 0)
    {
      *error = 0;
      return ang->lspace_factor;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ang2ln_set_lspace_factor(dwg_ent_dim_ang2ln *ang,
                                     double factor, int *error)
{
  if (ang != 0)
    {
      *error = 0;
      ang->lspace_factor = factor;
    }
  else
    *error = 1;
}

unsigned int
dwg_ent_dim_ang2ln_get_lspace_style(dwg_ent_dim_ang2ln *ang, int *error)
{
  if (ang != 0)
    {
      *error = 0;
      return ang->lspace_style;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ang2ln_set_lspace_style(dwg_ent_dim_ang2ln *ang,
                                    unsigned int style, int *error)
{
  if (ang != 0)
    {
      *error = 0;
      ang->lspace_style = style;
    }
  else
    *error = 1;
}

unsigned int
dwg_ent_dim_ang2ln_get_attachment_point(dwg_ent_dim_ang2ln *ang, int *error)
{
  if (ang != 0)
    {
      *error = 0;
      return ang->attachment_point;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ang2ln_set_attachment_point(dwg_ent_dim_ang2ln *ang,
                                        unsigned int point, int *error)
{
  if (ang != 0)
    {
      *error = 0;
      ang->attachment_point = point;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ang2ln_set_extrusion(dwg_ent_dim_ang2ln *ang, dwg_point_3d *point,
                                 int *error)
{
  if (ang != 0)
    {
      *error = 0;
      ang->extrusion.x = point->x;
      ang->extrusion.y = point->y;
      ang->extrusion.z = point->z;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ang2ln_get_extrusion(dwg_ent_dim_ang2ln *ang, dwg_point_3d *point,
                                 int *error)
{
  if (ang != 0)
    {
      *error = 0;
      point->x = ang->extrusion.x;
      point->y = ang->extrusion.y;
      point->z = ang->extrusion.z;
    }
  else
    *error = 1;
}

char *
dwg_ent_dim_ang2ln_get_user_text(dwg_ent_dim_ang2ln *ang, int *error)
{
  if (ang != 0)
    {
      *error = 0;
      return ang->user_text;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ang2ln_set_user_text(dwg_ent_dim_ang2ln *ang, char * text,
                                 int *error)
{
  if (ang != 0)
    {
      *error = 0;
      ang->user_text = text;
    }
  else
    *error = 1;
}

double
dwg_ent_dim_ang2ln_get_text_rot(dwg_ent_dim_ang2ln *ang, int *error)
{
  if (ang != 0)
    {
      *error = 0;
      return ang->text_rot;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ang2ln_set_text_rot(dwg_ent_dim_ang2ln *ang, double rot,
                                int *error)
{
  if (ang != 0)
    {
      *error = 0;
      ang->text_rot = rot;
    }
  else
    *error = 1;
}

double
dwg_ent_dim_ang2ln_get_ins_rotation(dwg_ent_dim_ang2ln *ang, int *error)
{
  if (ang != 0)
    {
      *error = 0;
      return ang->ins_rotation;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ang2ln_set_ins_rotation(dwg_ent_dim_ang2ln *ang, double rot,
                                int *error)
{
  if (ang != 0)
    {
      *error = 0;
      ang->ins_rotation = rot;
    }
  else
    *error = 1;
}

char
dwg_ent_dim_ang2ln_get_flip_arrow1(dwg_ent_dim_ang2ln *ang, int *error)
{
  if (ang != 0)
    {
      *error = 0;
      return ang->flip_arrow1;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ang2ln_set_flip_arrow1(dwg_ent_dim_ang2ln *ang, char flip_arrow,
                                   int *error)
{
  if (ang != 0)
    {
      *error = 0;
      ang->flip_arrow1 = flip_arrow;
    }
  else
    *error = 1;
}

char
dwg_ent_dim_ang2ln_get_flip_arrow2(dwg_ent_dim_ang2ln *ang, int *error)
{
  if (ang != 0)
    {
      *error = 0;
      return ang->flip_arrow2;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ang2ln_set_flip_arrow2(dwg_ent_dim_ang2ln *ang, char flip_arrow,
                                   int *error)
{
  if (ang != 0)
    {
      *error = 0;
      ang->flip_arrow2 = flip_arrow;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ang2ln_set_text_mid_pt(dwg_ent_dim_ang2ln *ang,
                                   dwg_point_2d *point, int *error)
{
  if (ang != 0)
    {
      *error = 0;
      ang->text_midpt.x = point->x;
      ang->text_midpt.y = point->y;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ang2ln_get_text_mid_pt(dwg_ent_dim_ang2ln *ang,
                                   dwg_point_2d *point, int *error)
{
  if (ang != 0)
    {
      *error = 0;
      point->x = ang->text_midpt.x;
      point->y = ang->text_midpt.y;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ang2ln_set_ins_scale(dwg_ent_dim_ang2ln *ang, dwg_point_3d *point,
                                 int *error)
{
  if (ang != 0)
    {
      *error = 0;
      ang->ins_scale.x = point->x;
      ang->ins_scale.y = point->y;
      ang->ins_scale.z = point->z;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ang2ln_get_ins_scale(dwg_ent_dim_ang2ln *ang, dwg_point_3d *point,
                                 int *error)
{
  if (ang != 0)
    {
      *error = 0;
      point->x = ang->ins_scale.x;
      point->y = ang->ins_scale.y;
      point->z = ang->ins_scale.z;
    }
  else
    *error = 1;
}


void
dwg_ent_dim_ang2ln_set_12_pt(dwg_ent_dim_ang2ln *ang, dwg_point_2d *point,
                             int *error)
{
  if (ang != 0)
    {
      *error = 0;
      ang->_12_pt.x = point->x;
      ang->_12_pt.y = point->y;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ang2ln_get_12_pt(dwg_ent_dim_ang2ln *ang, dwg_point_2d *point,
                             int *error)
{
  if (ang != 0)
    {
      *error = 0;
      point->x = ang->_12_pt.x;
      point->y = ang->_12_pt.y;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ang2ln_set_10_pt(dwg_ent_dim_ang2ln *ang, dwg_point_3d *point,
                             int *error)
{
  if (ang != 0)
    {
      *error = 0;
      ang->_10_pt.x = point->x;
      ang->_10_pt.y = point->y;
      ang->_10_pt.z = point->z;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ang2ln_get_10_pt(dwg_ent_dim_ang2ln *ang, dwg_point_3d *point,
                             int *error)
{
  if (ang != 0)
    {
      *error = 0;
      point->x = ang->_10_pt.x;
      point->y = ang->_10_pt.y;
      point->z = ang->_10_pt.z;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ang2ln_set_13_pt(dwg_ent_dim_ang2ln *ang, dwg_point_3d *point,
                             int *error)
{
  if (ang != 0)
    {
      *error = 0;
      ang->_13_pt.x = point->x;
      ang->_13_pt.y = point->y;
      ang->_13_pt.z = point->z;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ang2ln_get_13_pt(dwg_ent_dim_ang2ln *ang, dwg_point_3d *point,
                             int *error)
{
  if (ang != 0)
    {
      *error = 0;
      point->x = ang->_13_pt.x;
      point->y = ang->_13_pt.y;
      point->z = ang->_13_pt.z;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ang2ln_set_14_pt(dwg_ent_dim_ang2ln *ang, dwg_point_3d *point,
                             int *error)
{
  if (ang != 0)
    {
      *error = 0;
      ang->_14_pt.x = point->x;
      ang->_14_pt.y = point->y;
      ang->_14_pt.z = point->z;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ang2ln_get_14_pt(dwg_ent_dim_ang2ln *ang, dwg_point_3d *point,
                             int *error)
{
  if (ang != 0)
    {
      *error = 0;
      point->x = ang->_14_pt.x;
      point->y = ang->_14_pt.y;
      point->z = ang->_14_pt.z;
    }
  else
    *error = 1;
}


void
dwg_ent_dim_ang2ln_set_15_pt(dwg_ent_dim_ang2ln *ang, dwg_point_3d *point,
                             int *error)
{
  if (ang != 0)
    {
      *error = 0;
      ang->_15_pt.x = point->x;
      ang->_15_pt.y = point->y;
      ang->_15_pt.z = point->z;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ang2ln_get_15_pt(dwg_ent_dim_ang2ln *ang, dwg_point_3d *point,
                             int *error)
{
  if (ang != 0)
    {
      *error = 0;
      point->x = ang->_15_pt.x;
      point->y = ang->_15_pt.y;
      point->z = ang->_15_pt.z;
    }
  else
    *error = 1;
}


void
dwg_ent_dim_ang2ln_set_16_pt(dwg_ent_dim_ang2ln *ang, dwg_point_2d *point,
                             int *error)
{
  if (ang != 0)
    {
      *error = 0;
      ang->_16_pt.x = point->x;
      ang->_16_pt.y = point->y;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_ang2ln_get_16_pt(dwg_ent_dim_ang2ln *ang, dwg_point_2d *point,
                             int *error)
{
  if (ang != 0)
    {
      *error = 0;
      point->x = ang->_16_pt.x;
      point->y = ang->_16_pt.y;
    }
  else
    *error = 1;
}

/*******************************************************************
*              FUNCTIONS FOR RADIUS DIMENSION ENTITY                *
********************************************************************/

double
dwg_ent_dim_radius_get_elevation_ecs11(dwg_ent_dim_radius *radius, int *error)
{
  if (radius != 0)
    {
      *error = 0;
      return radius->elevation.ecs_11;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_radius_set_elevation_ecs11(dwg_ent_dim_radius *radius,
                                       double elevation_ecs11, int *error)
{
  if (radius != 0)
    {
      *error = 0;
      radius->elevation.ecs_11 = elevation_ecs11;
    }
  else
    *error = 1;
}

double
dwg_ent_dim_radius_get_elevation_ecs12(dwg_ent_dim_radius *radius, int *error)
{
  if (radius != 0)
    {
      *error = 0;
      return radius->elevation.ecs_12;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_radius_set_elevation_ecs12(dwg_ent_dim_radius *radius,
                                       double elevation_ecs12, int *error)
{
  if (radius != 0)
    {
      *error = 0;
      radius->elevation.ecs_12 = elevation_ecs12;
    }
  else
    *error = 1;
}

char
dwg_ent_dim_radius_get_flags1(dwg_ent_dim_radius *radius, int *error)
{
  if (radius != 0)
    {
      *error = 0;
      return radius->flags_1;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_radius_set_flags1(dwg_ent_dim_radius *radius, char flag,
                              int *error)
{
  if (radius != 0)
    {
      *error = 0;
      radius->flags_1 = flag;
    }
  else
    *error = 1;
}

double
dwg_ent_dim_radius_get_act_measurement(dwg_ent_dim_radius *radius, int *error)
{
  if (radius != 0)
    {
      *error = 0;
      return radius->act_measurement;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_radius_set_act_measurement(dwg_ent_dim_radius *radius,
                                       double act_measurement, int *error)
{
  if (radius != 0)
    {
      *error = 0;
      radius->act_measurement = act_measurement;
    }
  else
    *error = 1;
}

double
dwg_ent_dim_radius_get_horiz_dir(dwg_ent_dim_radius *radius, int *error)
{
  if (radius != 0)
    {
      *error = 0;
      return radius->horiz_dir;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_radius_set_horiz_dir(dwg_ent_dim_radius *radius, double horiz_dir,
                                 int *error)
{
  if (radius != 0)
    {
      *error = 0;
      radius->horiz_dir = horiz_dir;
    }
  else
    *error = 1;
}

double
dwg_ent_dim_radius_get_lspace_factor(dwg_ent_dim_radius *radius, int *error)
{
  if (radius != 0)
    {
      *error = 0;
      return radius->lspace_factor;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_radius_set_lspace_factor(dwg_ent_dim_radius *radius, double factor,
                                     int *error)
{
  if (radius != 0)
    {
      *error = 0;
      radius->lspace_factor = factor;
    }
  else
    *error = 1;
}

unsigned int
dwg_ent_dim_radius_get_lspace_style(dwg_ent_dim_radius *radius, int *error)
{
  if (radius != 0)
    {
      *error = 0;
      return radius->lspace_style;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_radius_set_lspace_style(dwg_ent_dim_radius *radius,
                                    unsigned int style, int *error)
{
  if (radius != 0)
    {
      *error = 0;
      radius->lspace_style = style;
    }
  else
    *error = 1;
}

unsigned int
dwg_ent_dim_radius_get_attachment_point(dwg_ent_dim_radius *radius, int *error)
{
  if (radius != 0)
    {
      *error = 0;
      return radius->attachment_point;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_radius_set_attachment_point(dwg_ent_dim_radius *radius,
                                        unsigned int point, int *error)
{
  if (radius != 0)
    {
      *error = 0;
      radius->attachment_point = point;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_radius_set_extrusion(dwg_ent_dim_radius *radius,
                                 dwg_point_3d *point, int *error)
{
  if (radius != 0)
    {
      *error = 0;
      radius->extrusion.x = point->x;
      radius->extrusion.y = point->y;
      radius->extrusion.z = point->z;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_radius_get_extrusion(dwg_ent_dim_radius *radius,
                                 dwg_point_3d *point, int *error)
{
  if (radius != 0)
    {
      *error = 0;
      point->x = radius->extrusion.x;
      point->y = radius->extrusion.y;
      point->z = radius->extrusion.z;
    }
  else
    *error = 1;
}

char *
dwg_ent_dim_radius_get_user_text(dwg_ent_dim_radius *radius, int *error)
{
  if (radius != 0)
    {
      *error = 0;
      return radius->user_text;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_radius_set_user_text(dwg_ent_dim_radius *radius, char * text,
                                 int *error)
{
  if (radius != 0)
    {
      *error = 0;
      radius->user_text = text;
    }
  else
    *error = 1;
}

double
dwg_ent_dim_radius_get_text_rot(dwg_ent_dim_radius *radius, int *error)
{
  if (radius != 0)
    {
      *error = 0;
      return radius->text_rot;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_radius_set_text_rot(dwg_ent_dim_radius *radius, double rot,
                                int *error)
{
  if (radius != 0)
    {
      *error = 0;
      radius->text_rot = rot;
    }
  else
    *error = 1;
}

double
dwg_ent_dim_radius_get_ins_rotation(dwg_ent_dim_radius *radius, int *error)
{
  if (radius != 0)
    {
      *error = 0;
      return radius->ins_rotation;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_radius_set_ins_rotation(dwg_ent_dim_radius *radius, double rot,
                                    int *error)
{
  if (radius != 0)
    {
      *error = 0;
      radius->ins_rotation = rot;
    }
  else
    *error = 1;
}

char
dwg_ent_dim_radius_get_flip_arrow1(dwg_ent_dim_radius *radius, int *error)
{
  if (radius != 0)
    {
      *error = 0;
      return radius->flip_arrow1;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_radius_set_flip_arrow1(dwg_ent_dim_radius *radius, char flip_arrow,
                                   int *error)
{
  if (radius != 0)
    {
      *error = 0;
      radius->flip_arrow1 = flip_arrow;
    }
  else
    *error = 1;
}

char
dwg_ent_dim_radius_get_flip_arrow2(dwg_ent_dim_radius *radius, int *error)
{
  if (radius != 0)
    {
      *error = 0;
      return radius->flip_arrow2;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_radius_set_flip_arrow2(dwg_ent_dim_radius *radius, char flip_arrow,
                                   int *error)
{
  if (radius != 0)
    {
      *error = 0;
      radius->flip_arrow2 = flip_arrow;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_radius_set_text_mid_pt(dwg_ent_dim_radius *radius,
                                   dwg_point_2d *point, int *error)
{
  if (radius != 0)
    {
      *error = 0;
      radius->text_midpt.x = point->x;
      radius->text_midpt.y = point->y;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_radius_get_text_mid_pt(dwg_ent_dim_radius *radius,
                                   dwg_point_2d *point, int *error)
{
  if (radius != 0)
    {
      *error = 0;
      point->x = radius->text_midpt.x;
      point->y = radius->text_midpt.y;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_radius_set_ins_scale(dwg_ent_dim_radius *radius,
                                 dwg_point_3d *point, int *error)
{
  if (radius != 0)
    {
      *error = 0;
      radius->ins_scale.x = point->x;
      radius->ins_scale.y = point->y;
      radius->ins_scale.z = point->z;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_radius_get_ins_scale(dwg_ent_dim_radius *radius,
                                 dwg_point_3d *point, int *error)
{
  if (radius != 0)
    {
      *error = 0;
      point->x = radius->ins_scale.x;
      point->y = radius->ins_scale.y;
      point->z = radius->ins_scale.z;
    }
  else
    *error = 1;
}


void
dwg_ent_dim_radius_set_12_pt(dwg_ent_dim_radius *radius, dwg_point_2d *point,
                             int *error)
{
  if (radius != 0)
    {
      *error = 0;
      radius->_12_pt.x = point->x;
      radius->_12_pt.y = point->y;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_radius_get_12_pt(dwg_ent_dim_radius *radius, dwg_point_2d *point,
                             int *error)
{
  if (radius != 0)
    {
      *error = 0;
      point->x = radius->_12_pt.x;
      point->y = radius->_12_pt.y;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_radius_set_10_pt(dwg_ent_dim_radius *radius, dwg_point_3d *point,
                             int *error)
{
  if (radius != 0)
    {
      *error = 0;
      radius->_10_pt.x = point->x;
      radius->_10_pt.y = point->y;
      radius->_10_pt.z = point->z;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_radius_get_10_pt(dwg_ent_dim_radius *radius, dwg_point_3d *point,
                             int *error)
{
  if (radius != 0)
    {
      *error = 0;
      point->x = radius->_10_pt.x;
      point->y = radius->_10_pt.y;
      point->z = radius->_10_pt.z;
    }
  else
    *error = 1;
}


void
dwg_ent_dim_radius_set_15_pt(dwg_ent_dim_radius *radius, dwg_point_3d *point,
                             int *error)
{
  if (radius != 0)
    {
      *error = 0;
      radius->_15_pt.x = point->x;
      radius->_15_pt.y = point->y;
      radius->_15_pt.z = point->z;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_radius_get_15_pt(dwg_ent_dim_radius *radius, dwg_point_3d *point,
                             int *error)
{
  if (radius != 0)
    {
      *error = 0;
      point->x = radius->_15_pt.x;
      point->y = radius->_15_pt.y;
      point->z = radius->_15_pt.z;
    }
  else
    *error = 1;
}

double
dwg_ent_dim_radius_get_leader_length(dwg_ent_dim_radius *radius, int *error)
{
  if (radius != 0)
    {
      *error = 0;
      return radius->leader_len;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_radius_set_leader_length(dwg_ent_dim_radius *radius, double length,
                                     int *error)
{
  if (radius != 0)
    {
      *error = 0;
      radius->leader_len = length;
    }
  else
    *error = 1;
}

/*******************************************************************
*             FUNCTIONS FOR DIAMETER DIMENSION ENTITY               *
********************************************************************/

double
dwg_ent_dim_diameter_get_elevation_ecs11(dwg_ent_dim_diameter *dia, int *error)
{
  if (dia != 0)
    {
      *error = 0;
      return dia->elevation.ecs_11;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_diameter_set_elevation_ecs11(dwg_ent_dim_diameter *dia,
                                         double elevation_ecs11, int *error)
{
  if (dia != 0)
    {
      *error = 0;
      dia->elevation.ecs_11 = elevation_ecs11;
    }
  else
    *error = 1;
}

double
dwg_ent_dim_diameter_get_elevation_ecs12(dwg_ent_dim_diameter *dia, int *error)
{
  if (dia != 0)
    {
      *error = 0;
      return dia->elevation.ecs_12;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_diameter_set_elevation_ecs12(dwg_ent_dim_diameter *dia,
                                         double elevation_ecs12, int *error)
{
  if (dia != 0)
    {
      *error = 0;
      dia->elevation.ecs_12 = elevation_ecs12;
    }
  else
    *error = 1;
}

char
dwg_ent_dim_diameter_get_flags1(dwg_ent_dim_diameter *dia, int *error)
{
  if (dia != 0)
    {
      *error = 0;
      return dia->flags_1;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_diameter_set_flags1(dwg_ent_dim_diameter *dia, char flag,
                                int *error)
{
  if (dia != 0)
    {
      *error = 0;
      dia->flags_1 = flag;
    }
  else
    *error = 1;
}

double
dwg_ent_dim_diameter_get_act_measurement(dwg_ent_dim_diameter *dia, int *error)
{
  if (dia != 0)
    {
      *error = 0;
      return dia->act_measurement;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_diameter_set_act_measurement(dwg_ent_dim_diameter *dia,
                                         double act_measurement, int *error)
{
  if (dia != 0)
    {
      *error = 0;
      dia->act_measurement = act_measurement;
    }
  else
    *error = 1;
}

double
dwg_ent_dim_diameter_get_horiz_dir(dwg_ent_dim_diameter *dia, int *error)
{
  if (dia != 0)
    {
      *error = 0;
      return dia->horiz_dir;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_diameter_set_horiz_dir(dwg_ent_dim_diameter *dia, double horiz_dir,
                                   int *error)
{
  if (dia != 0)
    {
      *error = 0;
      dia->horiz_dir = horiz_dir;
    }
  else
    *error = 1;
}

double
dwg_ent_dim_diameter_get_lspace_factor(dwg_ent_dim_diameter *dia, int *error)
{
  if (dia != 0)
    {
      *error = 0;
      return dia->lspace_factor;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_diameter_set_lspace_factor(dwg_ent_dim_diameter *dia,
                                       double factor, int *error)
{
  if (dia != 0)
    {
      *error = 0;
      dia->lspace_factor = factor;
    }
  else
    *error = 1;
}

unsigned int
dwg_ent_dim_diameter_get_lspace_style(dwg_ent_dim_diameter *dia, int *error)
{
  if (dia != 0)
    {
      *error = 0;
      return dia->lspace_style;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_diameter_set_lspace_style(dwg_ent_dim_diameter *dia,
                                      unsigned int style, int *error)
{
  if (dia != 0)
    {
      *error = 0;
      dia->lspace_style = style;
    }
  else
    *error = 1;
}

unsigned int
dwg_ent_dim_diameter_get_attachment_point(dwg_ent_dim_diameter *dia,
                                          int *error)
{
  if (dia != 0)
    {
      *error = 0;
      return dia->attachment_point;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_diameter_set_attachment_point(dwg_ent_dim_diameter *dia,
                                          unsigned int point, int *error)
{
  if (dia != 0)
    {
      *error = 0;
      dia->attachment_point = point;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_diameter_set_extrusion(dwg_ent_dim_diameter *dia,
                                   dwg_point_3d *point, int *error)
{
  if (dia != 0)
    {
      *error = 0;
      dia->extrusion.x = point->x;
      dia->extrusion.y = point->y;
      dia->extrusion.z = point->z;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_diameter_get_extrusion(dwg_ent_dim_diameter *dia,
                                   dwg_point_3d *point, int *error)
{
  if (dia != 0)
    {
      *error = 0;
      point->x = dia->extrusion.x;
      point->y = dia->extrusion.y;
      point->z = dia->extrusion.z;
    }
  else
    *error = 1;
}

char *
dwg_ent_dim_diameter_get_user_text(dwg_ent_dim_diameter *dia, int *error)
{
  if (dia != 0)
    {
      *error = 0;
      return dia->user_text;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_diameter_set_user_text(dwg_ent_dim_diameter *dia, char * text,
                                   int *error)
{
  if (dia != 0)
    {
      *error = 0;
      dia->user_text = text;
    }
  else
    *error = 1;
}

double
dwg_ent_dim_diameter_get_text_rot(dwg_ent_dim_diameter *dia, int *error)
{
  if (dia != 0)
    {
      *error = 0;
      return dia->text_rot;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_diameter_set_text_rot(dwg_ent_dim_diameter *dia, double rot,
                                  int *error)
{
  if (dia != 0)
    {
      *error = 0;
      dia->text_rot = rot;
    }
  else
    *error = 1;
}

double
dwg_ent_dim_diameter_get_ins_rotation(dwg_ent_dim_diameter *dia, int *error)
{
  if (dia != 0)
    {
      *error = 0;
      return dia->ins_rotation;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_diameter_set_ins_rotation(dwg_ent_dim_diameter *dia, double rot,
                                      int *error)
{
  if (dia != 0)
    {
      *error = 0;
      dia->ins_rotation = rot;
    }
  else
    *error = 1;
}

char
dwg_ent_dim_diameter_get_flip_arrow1(dwg_ent_dim_diameter *dia, int *error)
{
  if (dia != 0)
    {
      *error = 0;
      return dia->flip_arrow1;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_diameter_set_flip_arrow1(dwg_ent_dim_diameter *dia,
                                     char flip_arrow, int *error)
{
  if (dia != 0)
    {
      *error = 0;
      dia->flip_arrow1 = flip_arrow;
    }
  else
    *error = 1;
}

char
dwg_ent_dim_diameter_get_flip_arrow2(dwg_ent_dim_diameter *dia, int *error)
{
  if (dia != 0)
    {
      *error = 0;
      return dia->flip_arrow2;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_diameter_set_flip_arrow2(dwg_ent_dim_diameter *dia, 
                                     char flip_arrow, int *error)
{
  if (dia != 0)
    {
      *error = 0;
      dia->flip_arrow2 = flip_arrow;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_diameter_set_text_mid_pt(dwg_ent_dim_diameter *dia,
                                     dwg_point_2d *point, int *error)
{
  if (dia != 0)
    {
      *error = 0;
      dia->text_midpt.x = point->x;
      dia->text_midpt.y = point->y;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_diameter_get_text_mid_pt(dwg_ent_dim_diameter *dia,
                                     dwg_point_2d *point, int *error)
{
  if (dia != 0)
    {
      *error = 0;
      point->x = dia->text_midpt.x;
      point->y = dia->text_midpt.y;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_diameter_set_ins_scale(dwg_ent_dim_diameter *dia, 
                                   dwg_point_3d *point, int *error)
{
  if (dia != 0)
    {
      *error = 0;
      dia->ins_scale.x = point->x;
      dia->ins_scale.y = point->y;
      dia->ins_scale.z = point->z;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_diameter_get_ins_scale(dwg_ent_dim_diameter *dia, 
                                   dwg_point_3d *point, int *error)
{
  if (dia != 0)
    {
      *error = 0;
      point->x = dia->ins_scale.x;
      point->y = dia->ins_scale.y;
      point->z = dia->ins_scale.z;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_diameter_set_12_pt(dwg_ent_dim_diameter *dia, dwg_point_2d *point,
                               int *error)
{
  if (dia != 0)
    {
      *error = 0;
      dia->_12_pt.x = point->x;
      dia->_12_pt.y = point->y;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_diameter_get_12_pt(dwg_ent_dim_diameter *dia, dwg_point_2d *point,
                               int *error)
{
  if (dia != 0)
    {
      *error = 0;
      point->x = dia->_12_pt.x;
      point->y = dia->_12_pt.y;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_diameter_set_10_pt(dwg_ent_dim_diameter *dia, dwg_point_3d *point,
                               int *error)
{
  if (dia != 0)
    {
      *error = 0;
      dia->_10_pt.x = point->x;
      dia->_10_pt.y = point->y;
      dia->_10_pt.z = point->z;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_diameter_get_10_pt(dwg_ent_dim_diameter *dia, dwg_point_3d *point,
                               int *error)
{
  if (dia != 0)
    {
      *error = 0;
      point->x = dia->_10_pt.x;
      point->y = dia->_10_pt.y;
      point->z = dia->_10_pt.z;
    }
  else
    *error = 1;
}


void
dwg_ent_dim_diameter_set_15_pt(dwg_ent_dim_diameter *dia, dwg_point_3d *point,
                               int *error)
{
  if (dia != 0)
    {
      *error = 0;
      dia->_15_pt.x = point->x;
      dia->_15_pt.y = point->y;
      dia->_15_pt.z = point->z;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_diameter_get_15_pt(dwg_ent_dim_diameter *dia, dwg_point_3d *point,
                               int *error)
{
  if (dia != 0)
    {
      *error = 0;
      point->x = dia->_15_pt.x;
      point->y = dia->_15_pt.y;
      point->z = dia->_15_pt.z;
    }
  else
    *error = 1;
}

double
dwg_ent_dim_diameter_get_leader_length(dwg_ent_dim_diameter *dia, int *error)
{
  if (dia != 0)
    {
      *error = 0;
      return dia->leader_len;
    }
  else
    *error = 1;
}

void
dwg_ent_dim_diameter_set_leader_length(dwg_ent_dim_diameter *dia,
                                       double length, int *error)
{
  if (dia != 0)
    {
      *error = 0;
      dia->leader_len = length;
    }
  else
    *error = 1;
}

/*******************************************************************
*                   FUNCTIONS FOR ENDBLK ENTITY                     *
********************************************************************/

char
dwg_ent_endblk_get_dummy(dwg_ent_endblk *endblk, int *error)
{
  if(endblk != 0)
    {
      *error = 0;
      return endblk->dummy;
    }
  else
    *error = 1;   
}

void
dwg_ent_endblk_set_dummy(dwg_ent_endblk *endblk, char dummy, int *error)
{
  if(endblk != 0)
    {
      *error = 0;
      endblk->dummy = dummy;
    }
  else
    *error = 1;   
}

/*******************************************************************
*                   FUNCTIONS FOR SEQEND ENTITY                     *
********************************************************************/

char
dwg_ent_seqend_get_dummy(dwg_ent_seqend *seqend, int *error)
{
  if(seqend != 0)
    {
      *error = 0;
      return seqend->dummy;
    }
  else
    *error = 1;   
}

void
dwg_ent_seqend_set_dummy(dwg_ent_seqend *seqend, char dummy, int *error)
{
  if(seqend != 0)
    {
      *error = 0;
      seqend->dummy = dummy;
    }
  else
    *error = 1;   
}

/*******************************************************************
*                    FUNCTIONS FOR SHAPE ENTITY                     *
********************************************************************/

void
dwg_ent_shape_get_ins_pt(dwg_ent_shape *shape, dwg_point_3d *point, int *error)
{
  if(shape != 0)
    {
      *error = 0;
      point->x = shape->ins_pt.x;
      point->y = shape->ins_pt.y;
      point->z = shape->ins_pt.z;
    }
  else
    *error = 1;
}

void
dwg_ent_shape_set_ins_pt(dwg_ent_shape *shape, dwg_point_3d *point, int *error)
{
  if(shape != 0)
    {
      *error = 0;
      shape->ins_pt.x = point->x;
      shape->ins_pt.y = point->y;
      shape->ins_pt.z = point->z;
    }
  else
    *error = 1;
}

double
dwg_ent_shape_get_scale(dwg_ent_shape *shape, int *error)
{
  if(shape != 0)
    {
      *error = 0;
      return shape->scale;
    }
  else
    *error = 1;
}

void
dwg_ent_shape_set_scale(dwg_ent_shape *shape, double scale, int *error)
{
  if(shape != 0)
    {
      *error = 0;
      shape->scale = scale;
    }
  else
    *error = 1;
}

double
dwg_ent_shape_get_rotation(dwg_ent_shape *shape, int *error)
{
  if(shape != 0)
    {
      *error = 0;
      return shape->rotation;
    }
  else
    *error = 1;
}

void
dwg_ent_shape_set_rotation(dwg_ent_shape *shape, double rotation, int *error)
{
  if(shape != 0)
    {
      *error = 0;
      shape->rotation = rotation;
    }
  else
    *error = 1;
}

double
dwg_ent_shape_get_width_factor(dwg_ent_shape *shape, int *error)
{
  if(shape != 0)
    {
      *error = 0;
      return shape->width_factor;
    }
  else
    *error = 1;
}

void
dwg_ent_shape_set_width_factor(dwg_ent_shape *shape, double width_factor,
                               int *error)
{
  if(shape != 0)
    {
      *error = 0;
      shape->width_factor = width_factor;
    }
  else
    *error = 1;
}

double
dwg_ent_shape_get_oblique(dwg_ent_shape *shape, int *error)
{
  if(shape != 0)
    {
      *error = 0;
      return shape->oblique;
    }
  else
    *error = 1;
}

void
dwg_ent_shape_set_oblique(dwg_ent_shape *shape, double oblique, int *error)
{
  if(shape != 0)
    {
      *error = 0;
      shape->oblique = oblique;
    }
  else
    *error = 1;
}

double
dwg_ent_shape_get_thickness(dwg_ent_shape *shape, int *error)
{
  if(shape != 0)
    {
      *error = 0;
      return shape->thickness;
    }
  else
    *error = 1;
}

void
dwg_ent_shape_set_thickness(dwg_ent_shape *shape, double thickness,
                            int *error)
{
  if(shape != 0)
    {
      *error = 0;
      shape->thickness = thickness;
    }
  else
    *error = 1;
}

double
dwg_ent_shape_get_shape_no(dwg_ent_shape *shape, int *error)
{
  if(shape != 0)
    {
      *error = 0;
      return shape->shape_no;
    }
  else
    *error = 1;
}

void
dwg_ent_shape_set_shape_no(dwg_ent_shape *shape, double no, int *error)
{
  if(shape != 0)
    {
      *error = 0;
      shape->shape_no = no;
    }
  else
    *error = 1;
}

void
dwg_ent_shape_get_extrusion(dwg_ent_shape *shape, dwg_point_3d *point,
                            int *error)
{
  if(shape != 0)
    {
      *error = 0;
      point->x = shape->extrusion.x;
      point->y = shape->extrusion.y;
      point->z = shape->extrusion.z;
    }
  else
    *error = 1;
}

void
dwg_ent_shape_set_extrusion(dwg_ent_shape *shape, dwg_point_3d *point,
                            int *error)
{
  if(shape != 0)
    {
      *error = 0;
      shape->extrusion.x = point->x;
      shape->extrusion.y = point->y;
      shape->extrusion.z = point->z;
    }
  else
    *error = 1;
}


/*******************************************************************
*                    FUNCTIONS FOR MTEXT ENTITY                     *
********************************************************************/

void
dwg_ent_mtext_set_insertion_pt(dwg_ent_mtext *mtext, dwg_point_3d *point,
                               int *error)
{
  if(mtext != 0)
    {
      *error = 0;
      mtext->insertion_pt.x = point->x;
      mtext->insertion_pt.y = point->y;
      mtext->insertion_pt.z = point->z;
    }
  else
    *error = 1;
}

void
dwg_ent_mtext_get_insertion_pt(dwg_ent_mtext *mtext, dwg_point_3d *point,
                               int *error)
{
  if(mtext != 0)
    {
      *error = 0;
      point->x = mtext->insertion_pt.x;
      point->y = mtext->insertion_pt.y;
      point->z = mtext->insertion_pt.z;
    }
  else
    *error = 1;

}

void
dwg_ent_mtext_set_extrusion(dwg_ent_mtext *mtext, dwg_point_3d *point,
                            int *error)
{
  if(mtext != 0)
    {           
      *error = 0;
      mtext->extrusion.x = point->x;
      mtext->extrusion.y = point->y;
      mtext->extrusion.z = point->z;
    }
  else
    *error = 1;

}

void
dwg_ent_mtext_get_extrusion(dwg_ent_mtext *mtext, dwg_point_3d *point,
                            int *error)
{
  if(mtext != 0)
    {
      *error = 0;
      point->x = mtext->extrusion.x;
      point->y = mtext->extrusion.y;
      point->z = mtext->extrusion.z;
    }
  else
    *error = 1;

}

void
dwg_ent_mtext_set_x_axis_dir(dwg_ent_mtext *mtext, dwg_point_3d *point,
                             int *error)
{
  if(mtext != 0)
    {
      *error = 0;
      mtext->x_axis_dir.x = point->x;
      mtext->x_axis_dir.y = point->y;
      mtext->x_axis_dir.z = point->z;
    }
  else
    *error = 1;

}

void
dwg_ent_mtext_get_x_axis_dir(dwg_ent_mtext *mtext, dwg_point_3d *point,
                             int *error)
{
  if(mtext != 0)
    {
      *error = 0;
      point->x = mtext->x_axis_dir.x;
      point->y = mtext->x_axis_dir.y;
      point->z = mtext->x_axis_dir.z;
    }
  else
    *error = 1;

}

void
dwg_ent_mtext_set_rect_height(dwg_ent_mtext *mtext, double rect_height,
                              int *error)
{
  if(mtext != 0)
    {
      *error = 0;
      mtext->rect_height = rect_height;
    }
  else
    *error = 1;

}

double
dwg_ent_mtext_get_rect_height(dwg_ent_mtext *mtext, int *error)
{
  if(mtext != 0)
    {
      *error = 0;
      return mtext->rect_height;
    }
  else
    *error = 1;

}

void
dwg_ent_mtext_set_rect_width(dwg_ent_mtext *mtext, double rect_width,
                             int *error)
{
  if(mtext != 0)
    {
      *error = 0;
      mtext->rect_width = rect_width;
    }
  else
    *error = 1;

}

double
dwg_ent_mtext_get_rect_width(dwg_ent_mtext *mtext, int *error)
{
  if(mtext != 0)
    {
      *error = 0;
      return mtext->rect_width;
    }
  else
    *error = 1;

}

void
dwg_ent_mtext_set_text_height(dwg_ent_mtext *mtext, double text_height,
                              int *error)
{
  if(mtext != 0)
    {
      *error = 0;
      mtext->text_height = text_height;
    }
  else
    *error = 1;

}

double
dwg_ent_mtext_get_text_height(dwg_ent_mtext *mtext, int *error)
{
  if(mtext != 0)
    {
      *error = 0;
      return mtext->text_height;
    }
  else
    *error = 1;

}

unsigned int
dwg_ent_mtext_get_attachment(dwg_ent_mtext *mtext, int *error)
{
  if(mtext != 0)
    {
      *error = 0;
      return mtext->attachment;
    }
  else
    *error = 1;

}

void
dwg_ent_mtext_set_attachment(dwg_ent_mtext *mtext, unsigned int attachment,
                             int *error)
{
  if(mtext != 0)
    {
      *error = 0;
      mtext->attachment = attachment;
    }
  else
    *error = 1;

}

unsigned int
dwg_ent_mtext_get_drawing_dir(dwg_ent_mtext *mtext, int *error)
{
  if(mtext != 0)
    {
      *error = 0;
      return mtext->drawing_dir;
    }
  else
    *error = 1;

}

void
dwg_ent_mtext_set_drawing_dir(dwg_ent_mtext *mtext, unsigned int dir,
                              int *error)
{
  if(mtext != 0)
    {
      *error = 0;
      mtext->drawing_dir = dir;
    }
  else
    *error = 1;

}

double
dwg_ent_mtext_get_extends_ht(dwg_ent_mtext *mtext, int *error)
{
  if(mtext != 0)
    {
      *error = 0;
      return mtext->extends_ht;
    }
  else
    *error = 1;

}

void
dwg_ent_mtext_set_extends_ht(dwg_ent_mtext *mtext, double ht, int *error)
{
  if(mtext != 0)
    {
      *error = 0;
      mtext->extends_ht = ht;
    }
  else
    *error = 1;

}

double
dwg_ent_mtext_get_extends_wid(dwg_ent_mtext *mtext, int *error)
{
  if(mtext != 0)
    {
      *error = 0;
      return mtext->extends_wid;
    }
  else
    *error = 1;

}

void
dwg_ent_mtext_set_extends_wid(dwg_ent_mtext *mtext, double wid, int *error)
{
  if(mtext != 0)
    {
      *error = 0;
      mtext->extends_wid = wid;
    }
  else
    *error = 1;

}

char *
dwg_ent_mtext_get_text(dwg_ent_mtext *mtext, int *error)
{
  if(mtext != 0)
    {
      *error = 0;
      return mtext->text;
    }
  else
    *error = 1;

}

void
dwg_ent_mtext_set_text(dwg_ent_mtext *mtext, char * text, int *error)
{
  if(mtext != 0)
    {
      *error = 0;
      mtext->text = text;
    }
  else
    *error = 1;

}

unsigned int
dwg_ent_mtext_get_linespace_style(dwg_ent_mtext *mtext, int *error)
{
  if(mtext != 0)
    {
      *error = 0;
      return mtext->linespace_style;
    }
  else
    *error = 1;

}

void
dwg_ent_mtext_set_linespace_style(dwg_ent_mtext *mtext, unsigned int style,
                                  int *error)
{
  if(mtext != 0)
    {
      *error = 0;
      mtext->linespace_style = style;
    }
  else
    *error = 1;

}

double
dwg_ent_mtext_get_linespace_factor(dwg_ent_mtext *mtext, int *error)
{
  if(mtext != 0)
    {
      *error = 0;
      return mtext->linespace_factor;
    }
  else
    *error = 1;

}

void
dwg_ent_mtext_set_linespace_factor(dwg_ent_mtext *mtext, double factor,
                                   int *error)
{
  if(mtext != 0)
    {
      *error = 0;
      mtext->linespace_factor = factor;
    }
  else
    *error = 1;

}

/*******************************************************************
*                   FUNCTIONS FOR LEADER ENTITY                     *
********************************************************************/

void
dwg_ent_leader_set_annot_type(dwg_ent_leader *leader, unsigned int type,
                              int *error)
{
  if(leader != 0)
    {
      *error = 0;
      leader->annot_type = type;
    }
  else
    *error = 1;

}

unsigned int
dwg_ent_leader_get_annot_type(dwg_ent_leader *leader, int *error)
{
  if(leader != 0)
    {
      *error = 0;
      return leader->annot_type;
    }
  else
    *error = 1;

}

void
dwg_ent_leader_set_path_type(dwg_ent_leader *leader, unsigned int type,
                             int *error)
{
  if(leader != 0)
    {
      *error = 0;
      leader->path_type = type;
    }
  else
    *error = 1;

}

unsigned int
dwg_ent_leader_get_path_type(dwg_ent_leader *leader, int *error)
{
  if(leader != 0)
    {
      *error = 0;
      return leader->path_type;
    }
  else
    *error = 1;

}

long
dwg_ent_leader_get_numpts(dwg_ent_leader *leader, int *error)
{
  if(leader != 0)
    {
      *error = 0;
      return leader->numpts;
    }
  else
    *error = 1;

}

void
dwg_ent_leader_set_numpts(dwg_ent_leader *leader, long numpts, int *error)
{
  if(leader != 0)
    {
      *error = 0;
      leader->numpts = numpts;
    }
  else
    *error = 1;

}

void
dwg_ent_leader_set_end_pt_proj(dwg_ent_leader *leader, dwg_point_3d *point,
                               int *error)
{
  if(leader != 0)
    {
      *error = 0;
      leader->end_pt_proj.x = point->x;
      leader->end_pt_proj.y = point->y;
      leader->end_pt_proj.z = point->z;
    }
  else
    *error = 1;

}

void
dwg_ent_leader_get_end_pt_proj(dwg_ent_leader *leader, dwg_point_3d *point,
                               int *error)
{
  if(leader != 0)
    {
      *error = 0;
      point->x = leader->end_pt_proj.x;
      point->y = leader->end_pt_proj.y;
      point->z = leader->end_pt_proj.z;
    }
  else
    *error = 1;

}

void
dwg_ent_leader_set_extrusion(dwg_ent_leader *leader, dwg_point_3d *point,
                             int *error)
{
  if(leader != 0)
    {
      *error = 0;
      leader->extrusion.x = point->x;
      leader->extrusion.y = point->y;
      leader->extrusion.z = point->z;
    }
  else
    *error = 1;

}

void
dwg_ent_leader_get_extrusion(dwg_ent_leader *leader, dwg_point_3d *point,
                             int *error)
{
  if(leader != 0)
    {
      *error = 0;
      point->x = leader->extrusion.x;
      point->y = leader->extrusion.y;
      point->z = leader->extrusion.z;
    }
  else
    *error = 1;

}

void
dwg_ent_leader_set_x_direction(dwg_ent_leader *leader, dwg_point_3d *point,
                               int *error)
{
  if(leader != 0)
    {
      *error = 0;
      leader->x_direction.x = point->x;
      leader->x_direction.y = point->y;
      leader->x_direction.z = point->z;
    }
  else
    *error = 1;

}

void
dwg_ent_leader_get_x_direction(dwg_ent_leader *leader, dwg_point_3d *point,
                               int *error)
{
  if(leader != 0)
    {
      *error = 0;
      point->x = leader->x_direction.x;
      point->y = leader->x_direction.y;
      point->z = leader->x_direction.z;
    }
  else
    *error = 1;

}

void
dwg_ent_leader_set_offset_to_block_ins_pt(dwg_ent_leader *leader,
                                          dwg_point_3d *point, int *error)
{
  if(leader != 0)
    {
      *error = 0;
      leader->offset_to_block_ins_pt.x = point->x;
      leader->offset_to_block_ins_pt.y = point->y;
      leader->offset_to_block_ins_pt.z = point->z;
    }
  else
    *error = 1;

}

void
dwg_ent_leader_get_offset_to_block_ins_pt(dwg_ent_leader *leader,
                                          dwg_point_3d *point, int *error)
{
  if(leader != 0)
    {
      *error = 0;
      point->x = leader->offset_to_block_ins_pt.x;
      point->y = leader->offset_to_block_ins_pt.y;
      point->z = leader->offset_to_block_ins_pt.z;
    }
  else
    *error = 1;

}

void
dwg_ent_leader_set_dimgap(dwg_ent_leader *leader, double dimgap, int *error)
{
  if(leader != 0)
    {
      *error = 0;
      leader->dimgap = dimgap;
    }
  else
    *error = 1;

}

double
dwg_ent_leader_get_dimgap(dwg_ent_leader *leader, int *error)
{
  if(leader != 0)
    {
      *error = 0;
      return leader->dimgap;
    }
  else
    *error = 1;

}

void
dwg_ent_leader_set_box_height(dwg_ent_leader *leader, double height,
                              int *error)
{
  if(leader != 0)
    {
      *error = 0;
      leader->box_height = height;
    }
  else
    *error = 1;

}

double
dwg_ent_leader_get_box_height(dwg_ent_leader *leader, int *error)
{
  if(leader != 0)
    {
      *error = 0;
      return leader->box_height;
    }
  else
    *error = 1;

}

void
dwg_ent_leader_set_box_width(dwg_ent_leader *leader, double width, int *error)
{
  if(leader != 0)
    {
      *error = 0;
      leader->box_width = width;
    }
  else
    *error = 1;

}

double
dwg_ent_leader_get_box_width(dwg_ent_leader *leader, int *error)
{
  if(leader != 0)
    {
      *error = 0;
      return leader->box_width;
    }
  else
    *error = 1;

}

void
dwg_ent_leader_set_hook_line_on_x_dir(dwg_ent_leader *leader, char hook,
                                      int *error)
{
  if(leader != 0)
    {
      *error = 0;
      leader->hooklineonxdir = hook;
    }
  else
    *error = 1;

}

char
dwg_ent_leader_get_hook_line_on_x_dir(dwg_ent_leader *leader, int *error)
{
  if(leader != 0)
    {
      *error = 0;
      return leader->hooklineonxdir;
    }
  else
    *error = 1;

}

void
dwg_ent_leader_set_arrowhead_on(dwg_ent_leader *leader, char arrow, int *error)
{
  if(leader != 0)
    {
      *error = 0;
      leader->arrowhead_on = arrow;
    }
  else
    *error = 1;

}

char
dwg_ent_leader_get_arrowhead_on(dwg_ent_leader *leader, int *error)
{
  if(leader != 0)
    {
      *error = 0;
      return leader->arrowhead_on;
    }
  else
    *error = 1;

}

void
dwg_ent_leader_set_arrowhead_type(dwg_ent_leader *leader, unsigned int type,
                                  int *error)
{
  if(leader != 0)
    {
      leader->arrowhead_type = type;
      *error = 0;
    }
  else
    *error = 1;

}

unsigned int
dwg_ent_leader_get_arrowhead_type(dwg_ent_leader *leader, int *error)
{
  if(leader != 0)
    {
      *error = 0;
      return leader->arrowhead_type;
    }
  else
    *error = 1;

}

void
dwg_ent_leader_set_dimasz(dwg_ent_leader *leader, double dimasz, int *error)
{
  if(leader != 0)
    {
      *error = 0;
      leader->dimasz = dimasz;
    }
  else
    *error = 1;

}

double
dwg_ent_leader_get_dimasz(dwg_ent_leader *leader, int *error)
{
  if(leader != 0)
    {
      *error = 0;
      return leader->dimasz;
    }
  else
    *error = 1;

}

void
dwg_ent_leader_set_byblock_color(dwg_ent_leader *leader, unsigned int color,
                                 int *error)
{
  if(leader != 0)
    {
      *error = 0;
      leader->byblock_color = color;
    }
  else
    *error = 1;

}

unsigned int
dwg_ent_leader_get_byblock_color(dwg_ent_leader *leader, int *error)
{
  if(leader != 0)
    {
      *error = 0;
      return leader->byblock_color;
    }
  else
    *error = 1;

}

/*******************************************************************
*                  FUNCTIONS FOR TOLERANCE ENTITY                   *
********************************************************************/

void
dwg_ent_tolerance_set_height(dwg_ent_tolerance *tol, double height,
                             int *error)
{
  if(tol != 0)
    {
      *error = 0;
      tol->height = height;
    }
  else
    *error = 1;

}

double
dwg_ent_tolerance_get_height(dwg_ent_tolerance *tol, int *error)
{
  if(tol != 0)
    {
      *error = 0;
      return tol->height;
    }
  else
    *error = 1;

}

void
dwg_ent_tolerance_set_dimgap(dwg_ent_tolerance *tol, double dimgap,
                             int *error)
{
  if(tol != 0)
    {
      *error = 0;
      tol->dimgap = dimgap;
    }
  else
    *error = 1;

}

double
dwg_ent_tolerance_get_dimgap(dwg_ent_tolerance *tol, int *error)
{
  if(tol != 0)
    {
      *error = 0;
      return tol->dimgap;
    }
  else
    *error = 1;

}

void
dwg_ent_tolerance_set_ins_pt(dwg_ent_tolerance *tol, dwg_point_3d *point,
                             int *error)
{
  if(tol != 0)
    {
      *error = 0;
      tol->ins_pt.x = point->x;
      tol->ins_pt.y = point->y;
      tol->ins_pt.z = point->z;
    }
  else
    *error = 1;

}

void
dwg_ent_tolerance_get_ins_pt(dwg_ent_tolerance *tol, dwg_point_3d *point,
                             int *error)
{
  if(tol != 0)
    {
      *error = 0;
      point->x = tol->ins_pt.x;
      point->y = tol->ins_pt.y;
      point->z = tol->ins_pt.z;
    }
  else
    *error = 1;

}

void
dwg_ent_tolerance_set_x_direction(dwg_ent_tolerance *tol, dwg_point_3d *point,
                                  int *error)
{
  if(tol != 0)
    {
      *error = 0;
      tol->x_direction.x = point->x;
      tol->x_direction.y = point->y;
      tol->x_direction.z = point->z;
    }
  else
    *error = 1;

}

void
dwg_ent_tolerance_get_x_direction(dwg_ent_tolerance *tol, dwg_point_3d *point,
                                  int *error)
{
  if(tol != 0)
    {
      *error = 0;
      point->x = tol->x_direction.x;
      point->y = tol->x_direction.y;
      point->z = tol->x_direction.z;
    }
  else
    *error = 1;

}

void
dwg_ent_tolerance_set_extrusion(dwg_ent_tolerance *tol, dwg_point_3d *point,
                                int *error)
{
  if(tol != 0)
    {
      *error = 0;
      tol->extrusion.x = point->x;
      tol->extrusion.y = point->y;
      tol->extrusion.z = point->z;
    }
  else
    *error = 1;

}

void
dwg_ent_tolerance_get_extrusion(dwg_ent_tolerance *tol, dwg_point_3d *point,
                                int *error)
{
  if(tol != 0)
    {
      *error = 0;
      point->x = tol->extrusion.x;
      point->y = tol->extrusion.y;
      point->z = tol->extrusion.z;
    }
  else
    *error = 1;

}

void
dwg_ent_tolerance_set_text_string(dwg_ent_tolerance *tol, char * string,
                                  int *error)
{
  if(tol != 0)
    {
      *error = 0;
      tol->text_string = string;
    }
  else
    *error = 1;

}

char *
dwg_ent_tolerance_get_text_string(dwg_ent_tolerance *tol, int *error)
{
  if(tol != 0)
    {
      *error = 0;
      return tol->text_string;
    }
  else
    *error = 1;

}

/*******************************************************************
*                   FUNCTIONS FOR LWPLINE ENTITY                    *
********************************************************************/

char 
dwg_ent_lwpline_get_flags(dwg_ent_lwpline *lwpline, int *error)
{
  if(lwpline != 0)
    {
      *error = 0;  
      return lwpline->flags;
    }
  else
    *error = 1;
}

void
dwg_ent_lwpline_set_flags(dwg_ent_lwpline *lwpline, char flags, int *error)
{
  if(lwpline != 0)
    {
      *error = 0;
      lwpline->flags = flags;
    }
  else
    *error = 1;
}

double
dwg_ent_lwpline_get_const_width(dwg_ent_lwpline *lwpline, int *error)
{
  if(lwpline != 0)
    {
      *error = 0;
      return lwpline->const_width;
    }
  else
    *error = 1;
}

void
dwg_ent_lwpline_set_const_width(dwg_ent_lwpline *lwpline, double const_width,
                                int *error)
{
  if(lwpline != 0)
    {
      *error = 0;
      lwpline->const_width = const_width;    
    }
  else
    *error = 1;
}

double
dwg_ent_lwpline_get_elevation(dwg_ent_lwpline *lwpline, int *error)
{
  if(lwpline != 0)
    {
      *error = 0;
      return lwpline->elevation;
    }
  else
    *error = 1;
}

void
dwg_ent_lwpline_set_elevation(dwg_ent_lwpline *lwpline, double elevation,
                              int *error)
{
  if(lwpline != 0)
    {
      *error = 0;
      lwpline->elevation = elevation;
    }
  else
    *error = 1;
}

double
dwg_ent_lwpline_get_thickness(dwg_ent_lwpline *lwpline, int *error)
{
  if(lwpline != 0)
    {
      *error = 0;
      return lwpline->thickness;
    }
  else
    *error = 1;
}

void
dwg_ent_lwpline_set_thickness(dwg_ent_lwpline *lwpline, double thickness,
                              int *error)
{
  if(lwpline != 0)
    {
      *error = 0;
      lwpline->thickness = thickness;
    }
  else
    *error = 1;
}

long
dwg_ent_lwpline_get_num_points(dwg_ent_lwpline *lwpline, int *error)
{
  if(lwpline != 0)
    {
      *error = 0;
      return lwpline->num_points;
    }
  else
    *error = 1;
}

void
dwg_ent_lwpline_set_num_points(dwg_ent_lwpline *lwpline, long num_points,
                               int *error)
{
  if(lwpline != 0)
    {
      *error = 0;
      lwpline->num_points = num_points;
    }
  else
    *error = 1;
}

long
dwg_ent_lwpline_get_num_bulges(dwg_ent_lwpline *lwpline, int *error)
{
  if(lwpline != 0)
    {
      *error = 0;
      return lwpline->num_bulges;
    }
  else
    *error = 1;
}

void
dwg_ent_lwpline_set_num_bulges(dwg_ent_lwpline *lwpline, long num_bulges,
                               int *error)
{
  if(lwpline != 0)
    {
      *error = 0;
      lwpline->num_bulges = num_bulges;
    }
  else
    *error = 1;
}

long
dwg_ent_lwpline_get_num_widths(dwg_ent_lwpline *lwpline, int *error)
{
  if(lwpline != 0)
    {
      *error = 0;
      return lwpline->num_widths;
    }
  else
    *error = 1;
}

void
dwg_ent_lwpline_set_num_widths(dwg_ent_lwpline *lwpline, long num_widths,
                               int *error)
{
  if(lwpline != 0)
    {
      *error = 0;
      lwpline->num_widths = num_widths;
    }
  else
    *error = 1;
}

void
dwg_ent_lwpline_get_normal(dwg_ent_lwpline *lwpline, dwg_point_3d *points,
                           int *error)
{
  if(lwpline != 0)
    {
      *error = 0;
      points->x = lwpline->normal.x;
      points->y = lwpline->normal.y;
      points->z = lwpline->normal.z;
    }
  else
    *error = 1;
}

void
dwg_ent_lwpline_set_normal(dwg_ent_lwpline *lwpline, dwg_point_3d *points,
                           int *error)
{
  if(lwpline != 0)
    {
      *error = 0;
      lwpline->normal.x = points->x;
      lwpline->normal.y = points->y;
      lwpline->normal.z = points->z;
  }
  else
    *error = 1;
}

double *
dwg_ent_lwpline_get_bulges(dwg_ent_lwpline *lwpline, int *error)
{
  double *ptx = (double*) malloc(sizeof(double)* lwpline->num_bulges);
  if(ptx != 0)
    {
      *error = 0;
      int i = 0;
        for (i = 0; i < lwpline->num_bulges ; i++)
          {
            ptx[i] = lwpline->bulges[i];
          }
      return ptx;
    }
  else
    *error = 1;
}

dwg_point_2d *
dwg_ent_lwpline_get_points(dwg_ent_lwpline *lwpline, int *error)
{
  dwg_point_2d *ptx = (dwg_point_2d*) 
  malloc(sizeof(dwg_point_2d)* lwpline->num_points);
  if(ptx != 0)
    {
      *error = 0;
      int i = 0;
        for (i = 0; i < lwpline->num_points ; i++)
        {
          ptx[i].x = lwpline->points[i].x;
          ptx[i].y = lwpline->points[i].y;
        }
      return ptx;
    }
  else
    *error = 1;
}

dwg_lwpline_widths *
dwg_ent_lwpline_get_widths(dwg_ent_lwpline *lwpline, int *error)
{
  dwg_lwpline_widths *ptx = (dwg_lwpline_widths*)
    malloc(sizeof(dwg_lwpline_widths)* lwpline->num_widths);
  if(ptx != 0)
    {
      *error = 0;
      int i = 0;
        for (i = 0; i < lwpline->num_widths ; i++)
          {
            ptx[i].start = lwpline->widths[i].start;
            ptx[i].end = lwpline->widths[i].end;
          }
      return ptx;
    }
  else
    *error = 1;
}

/*******************************************************************
*                  FUNCTIONS FOR OLE2FRAME ENTITY                   *
********************************************************************/

unsigned int
dwg_ent_ole2frame_get_flags(dwg_ent_ole2frame *frame, int *error)
{
  if(frame != 0)
    {
      *error = 0;
      return frame->flags;
    }
  else
    *error = 1;
}

void
dwg_ent_ole2frame_set_flags(dwg_ent_ole2frame *frame, unsigned int flags,
                            int *error)
{
  if(frame != 0)
    {
      *error = 0;
      frame->flags = flags;
    }
  else
    *error = 1;
}

unsigned int
dwg_ent_ole2frame_get_mode(dwg_ent_ole2frame *frame, int *error)
{
  if(frame != 0)
    {
      *error = 0;
      return frame->mode;
    }
  else
    *error = 1;
}

void
dwg_ent_ole2frame_set_mode(dwg_ent_ole2frame *frame, unsigned int mode,
                           int *error)
{
  if(frame != 0)
    {
      *error = 0;
      frame->mode = mode;
    }
  else
    *error = 1;
}
long
dwg_ent_ole2frame_get_data_length(dwg_ent_ole2frame *frame, int *error)
{
  if(frame != 0)
    {
      *error = 0;
      return frame->data_length;
    }
  else
    *error = 1;
}

void
dwg_ent_ole2frame_set_data_length(dwg_ent_ole2frame *frame, long data_length,
                                  int *error)
{
  if(frame != 0)
    {
      *error = 0;
      frame->data_length = data_length;
    }
  else
    *error = 1;
}

char *
dwg_ent_ole2frame_get_data(dwg_ent_ole2frame *frame, int *error)
{
  if(frame != 0)
    {
      *error = 0;
      return frame->data;
    }
  else
    *error = 1;
}

void
dwg_ent_ole2frame_set_data(dwg_ent_ole2frame *frame, char * data, int *error)
{
  if(frame != 0)
    {
      *error = 0;
      frame->data = data;
    }
  else
    *error = 1;
}

/*******************************************************************
*                   FUNCTIONS FOR SPLINE ENTITY                     *
********************************************************************/

unsigned int
dwg_ent_spline_get_scenario(dwg_ent_spline *spline, int *error)
{
  if(spline != 0)
    {
      *error = 0;
      return spline->scenario;
    }
  else
    *error = 1;
}

void
dwg_ent_spline_set_scenario(dwg_ent_spline *spline, unsigned int scenario,
                               int *error)
{
  if(spline != 0)
    {
      *error = 0;
      spline->scenario = scenario;
    }
  else
    *error = 1;
}

unsigned int
dwg_ent_spline_get_degree(dwg_ent_spline *spline, int *error)
{
  if(spline != 0)
    {
      *error = 0;
      return spline->degree;
    }
  else
    *error = 1;
}

void
dwg_ent_spline_set_degree(dwg_ent_spline *spline, unsigned int degree,
                             int *error)
{
  if(spline != 0)
    {
      *error = 0;
      spline->degree = degree;
    }
  else
    *error = 1;
}

double
dwg_ent_spline_get_fit_tol(dwg_ent_spline *spline, int *error)
{
  if(spline != 0)
    {
      *error = 0;
      return spline->fit_tol;
    }
  else
    *error = 1;
}

void
dwg_ent_spline_set_fit_tol(dwg_ent_spline *spline, int fit_tol, int *error)
{
  if(spline != 0)
    {
      *error = 0;
      spline->fit_tol = fit_tol;
    }
  else
    *error = 1;
}

void
dwg_ent_spline_get_begin_tan_vector(dwg_ent_spline *spline,
                                       dwg_point_3d *point, int *error)
{
  if(spline != 0)
    {
      *error = 0;
      point->x = spline->beg_tan_vec.x;
      point->y = spline->beg_tan_vec.y;
    point->z = spline->beg_tan_vec.z;
  }
  else
    *error = 1;
}

void
dwg_ent_spline_set_begin_tan_vector(dwg_ent_spline *spline,
                                       dwg_point_3d *point, int *error)
{
  if(spline != 0)
    {
      *error = 0;
      spline->beg_tan_vec.x = point->x;
      spline->beg_tan_vec.y = point->y;
    spline->beg_tan_vec.z = point->z;
  }
  else
    *error = 1;
}

void
dwg_ent_spline_get_end_tan_vector(dwg_ent_spline *spline,
                                     dwg_point_3d *point, int *error)
{
  if(spline != 0)
    {
      *error = 0;
      point->x = spline->end_tan_vec.x;
      point->y = spline->end_tan_vec.y;
    point->z = spline->end_tan_vec.z;
  }
  else
    *error = 1;
}

void
dwg_ent_spline_set_end_tan_vector(dwg_ent_spline *spline,
                                     dwg_point_3d *point, int *error)
{
  if(spline != 0)
    {
      *error = 0;
      spline->end_tan_vec.x = point->x;
      spline->end_tan_vec.y = point->y;
    spline->end_tan_vec.z = point->z;
  }
  else
    *error = 1;
}

double
dwg_ent_spline_get_knot_tol(dwg_ent_spline *spline, int *error)
{
  if(spline != 0)
    {
      *error = 0;
      return spline->knot_tol;
    }
  else
    *error = 1;
}

void
dwg_ent_spline_set_knot_tol(dwg_ent_spline *spline, double knot_tol,
                               int *error)
{
  if(spline != 0)
    {
      *error = 0;
      spline->knot_tol = knot_tol;
    }
  else
    *error = 1;
}

double
dwg_ent_spline_get_ctrl_tol(dwg_ent_spline *spline, int *error)
{
  if(spline != 0)
    {
      *error = 0;
      return spline->ctrl_tol;
    }
  else
    *error = 1;
}

void
dwg_ent_spline_set_ctrl_tol(dwg_ent_spline *spline, double ctrl_tol,
                               int *error)
{
  if(spline != 0)
    {
      *error = 0;
      spline->ctrl_tol = ctrl_tol;
    }
  else
    *error = 1;
}

unsigned int
dwg_ent_spline_get_num_fit_pts(dwg_ent_spline *spline, int *error)
{
  if(spline != 0)
    {
      *error = 0;
      return spline->num_fit_pts;
    }
  else
    *error = 1;
}

void
dwg_ent_spline_set_num_fit_pts(dwg_ent_spline *spline, int num_fit_pts,
                                  int *error)
{
  if(spline != 0)
    {
      *error = 0;
      spline->num_fit_pts = num_fit_pts;
    }
  else
    *error = 1;
}

char
dwg_ent_spline_get_rational(dwg_ent_spline *spline, int *error)
{
  if(spline != 0)
    {
      *error = 0;
      return spline->rational;
    }
  else
    *error = 1;
}

void
dwg_ent_spline_set_rational(dwg_ent_spline *spline, char rational,
                               int *error)
{
  if(spline != 0)
    {
      *error = 0;
      spline->rational = rational;
    }
  else
    *error = 1;
}

char
dwg_ent_spline_get_closed_b(dwg_ent_spline *spline, int *error)
{
  if(spline != 0)
    {
      *error = 0;
      return spline->closed_b;
    }
  else
    *error = 1;
}

void
dwg_ent_spline_set_closed_b(dwg_ent_spline *spline, char closed_b,
                               int *error)
{
  if(spline != 0)
    {
      *error = 0;
      spline->closed_b = closed_b;
    }
  else
    *error = 1;
}

char
dwg_ent_spline_get_weighted(dwg_ent_spline *spline, int *error)
{
  if(spline != 0)
    {
      *error = 0;
      return spline->weighted;
    }
  else
    *error = 1;
}

void
dwg_ent_spline_set_weighted(dwg_ent_spline *spline, char weighted,
                               int *error)
{
  if(spline != 0)
    {
      *error = 0;
      spline->weighted = weighted;
    }
  else
    *error = 1;
}

char
dwg_ent_spline_get_periodic(dwg_ent_spline *spline, int *error)
{
  if(spline != 0)
    {
      *error = 0;
      return spline->periodic;
    }
  else
    *error = 1;
}

void
dwg_ent_spline_set_periodic(dwg_ent_spline *spline, char periodic,
                               int *error)
{
  if(spline != 0)
    {
      *error = 0;
      spline->periodic = periodic;
    }
  else
    *error = 1;
}

long
dwg_ent_spline_get_num_knots(dwg_ent_spline *spline, int *error)
{
  if(spline != 0)
    {
      *error = 0;
      return spline->num_knots;
    }
  else
    *error = 1;
}

void
dwg_ent_spline_set_num_knots(dwg_ent_spline *spline, long nums, int *error)
{
  if(spline != 0)
    {
      *error = 0;
      spline->num_knots = nums;
    }
  else
    *error = 1;
}

long
dwg_ent_spline_get_num_ctrl_pts(dwg_ent_spline *spline, int *error)
{
  if(spline != 0)
    {
      *error = 0;
      return spline->num_ctrl_pts;
    }
  else
    *error = 1;
}

void
dwg_ent_spline_set_num_ctrl_pts(dwg_ent_spline *spline, long nums,
                                   int *error)
{
  if(spline != 0)
    {
      *error = 0;
      spline->num_ctrl_pts = nums;
    }
  else
    *error = 1;
}


dwg_ent_spline_point *
dwg_ent_spline_get_fit_points(dwg_ent_spline *spline, int *error)
{
  dwg_ent_spline_point *ptx = (dwg_ent_spline_point*)
  malloc(sizeof(dwg_ent_spline_point)* spline->num_fit_pts);
    if(ptx != 0)
      {
        *error = 0;
        int i;
        for (i = 0; i < spline->num_fit_pts ; i++)
          {
            ptx[i] = spline->fit_pts[i];
          }
        return ptx;
      }
  else
    *error = 1;
}

dwg_ent_spline_control_point *
dwg_ent_spline_get_ctrl_pts(dwg_ent_spline *spline, int *error)
{
  dwg_ent_spline_control_point *ptx = (dwg_ent_spline_control_point*) 
  malloc(sizeof(dwg_ent_spline_control_point)* spline->num_ctrl_pts);
    if(ptx != 0)
      {
        *error = 0;
        int i;
          for (i = 0; i < spline->num_ctrl_pts ; i++)
            {
              ptx[i] = spline->ctrl_pts[i];
            }
        return ptx;
      }
    else
      *error = 1;
}

double *
dwg_ent_spline_get_knots(dwg_ent_spline *spline, int *error)
{
  double *ptx = (double*) malloc(sizeof(double)* spline->num_knots);
    if(ptx != 0)
      {
        *error = 0;
        int i = 0;
          for (i = 0; i < spline->num_knots ; i++)
            {
              ptx[i] = spline->knots[i];
            }
        return ptx;
      }
    else
      *error = 1;
}

/*******************************************************************
*                   FUNCTIONS FOR VIEWPORT ENTITY                   *
********************************************************************/

void
dwg_ent_viewport_get_center(dwg_ent_viewport *vp, dwg_point_3d *point,
                            int *error)
{
  if (vp != 0)
    {
      *error = 0;
      point->x = vp->center.x;
      point->y = vp->center.y;
      point->z = vp->center.z;
    }
  else
    *error = 1;
}

void
dwg_ent_viewport_set_center(dwg_ent_viewport *vp, dwg_point_3d *point,
                            int *error)
{
  if (vp != 0)
    {
      *error = 0;
      vp->center.x = point->x;
      vp->center.y = point->y;
      vp->center.z = point->z;
    }
  else
    *error = 1;
}

double
dwg_ent_viewport_get_width(dwg_ent_viewport *vp, int *error)
{
  if (vp != 0)
    {
      *error = 0;
      return vp->width;
    }
  else
    *error = 1;
}

void
dwg_ent_viewport_set_width(dwg_ent_viewport *vp, double width, int *error)
{
  if (vp != 0)
    {
      *error = 0;
      vp->width = width;
    }
  else
    *error = 1;
}

double
dwg_ent_viewport_get_height(dwg_ent_viewport *vp, int *error)
{
  if (vp != 0)
    {
      *error = 0;
      return vp->height;
    }
  else
    *error = 1;
}

void
dwg_ent_viewport_set_height(dwg_ent_viewport *vp, double height, int *error)
{
  if (vp != 0)
    {
      *error = 0;
      vp->height = height;
    }
  else
    *error = 1;
}

unsigned int
dwg_ent_viewport_get_grid_major(dwg_ent_viewport *vp, int *error)
{
  if (vp != 0)
    {
      *error = 0;
      return vp->grid_major;
    }
  else
    *error = 1;
}

void
dwg_ent_viewport_set_grid_major(dwg_ent_viewport *vp, unsigned int major,
                                int *error)
{
  if (vp != 0)
    {
      *error = 0;
      vp->grid_major = major;
    }
  else
    *error = 1;
}

long
dwg_ent_viewport_get_frozen_layer_count(dwg_ent_viewport *vp, int *error)
{
  if (vp != 0)
    {
      *error = 0;
      return vp->frozen_layer_count;
    }
  else
    *error = 1;
}

void
dwg_ent_viewport_set_frozen_layer_count(dwg_ent_viewport *vp, long count,
                                        int *error)
{
  if (vp != 0)
    {
      *error = 0;
      vp->frozen_layer_count = count;
    }
  else
    *error = 1;
}

char *
dwg_ent_viewport_get_style_sheet(dwg_ent_viewport *vp, int *error)
{
  if (vp != 0)
    {
      *error = 0;
      return vp->style_sheet;
    }
  else
    *error = 1;
}

void
dwg_ent_viewport_set_style_sheet(dwg_ent_viewport *vp, char * sheet,
                                 int *error)
{
  if (vp != 0)
    {
      *error = 0;
      vp->style_sheet = sheet;
    }
  else
    *error = 1;
}

void
dwg_ent_viewport_set_circle_zoom(dwg_ent_viewport *vp, unsigned int zoom,
                                 int *error)
{
  if (vp != 0)
    {
      *error = 0;
      vp->circle_zoom = zoom;
    }
  else
    *error = 1;
}

unsigned int
dwg_ent_viewport_get_circle_zoom(dwg_ent_viewport *vp, int *error)
{
  if (vp != 0)
    {
      *error = 0;
      return vp->circle_zoom;
    }
  else
    *error = 1;
}

void
dwg_ent_viewport_set_status_flags(dwg_ent_viewport *vp, long flags, int *error)
{
  if (vp != 0)
    {
      *error = 0;
      vp->status_flags = flags;
    }
  else
    *error = 1;
}
long
dwg_ent_viewport_get_status_flags(dwg_ent_viewport *vp, int *error)
{
  if (vp != 0)
    {
      *error = 0;
      return vp->status_flags;
    }
  else
    *error = 1;
}

char
dwg_ent_viewport_get_render_mode(dwg_ent_viewport *vp, int *error)
{
  if (vp != 0)
    {
      *error = 0;
      return vp->render_mode;
    }
  else
    *error = 1;
}

void
dwg_ent_viewport_set_render_mode(dwg_ent_viewport *vp, char mode, int *error)
{
  if (vp != 0)
    {
      *error = 0;
      vp->render_mode = mode;
    }
  else
    *error = 1;
}

void
dwg_ent_viewport_set_ucs_at_origin(dwg_ent_viewport *vp, unsigned char origin,
                                   int *error)
{
  if (vp != 0)
    {
      *error = 0;
      vp->ucs_at_origin = origin;
    }
  else
    *error = 1;
}

unsigned char 
dwg_ent_viewport_get_ucs_at_origin(dwg_ent_viewport *vp, int *error)
{
  if (vp != 0)
    {
      *error = 0;
      return vp->ucs_at_origin;
    }
  else
    *error = 1;
}

void
dwg_ent_viewport_set_ucs_per_viewport(dwg_ent_viewport *vp,
                                      unsigned char viewport, int *error)
{
  if (vp != 0)
    {
      *error = 0;
      vp->ucs_per_viewport = viewport;
    }
  else
    *error = 1;
}

unsigned char 
dwg_ent_viewport_get_ucs_per_viewport(dwg_ent_viewport *vp, int *error)
{
  if (vp != 0)
    {
      *error = 0;
      return vp->ucs_per_viewport;
    }
  else
    *error = 1;
}

void
dwg_ent_viewport_set_view_target(dwg_ent_viewport *vp, dwg_point_3d *point,
                                 int *error)
{
  if (vp != 0)
    {
      *error = 0;
      vp->view_target.x = point->x;
      vp->view_target.y = point->y;
      vp->view_target.z = point->z;
    }
  else
    *error = 1;
}

void
dwg_ent_viewport_get_view_target(dwg_ent_viewport *vp, dwg_point_3d *point,
                                 int *error)
{
  if (vp != 0)
    {
      *error = 0;
      point->x = vp->view_target.x;
      point->y = vp->view_target.y;
      point->z = vp->view_target.z;
    }
  else
    *error = 1;
}

void
dwg_ent_viewport_set_view_direction(dwg_ent_viewport *vp, dwg_point_3d *point,
                                    int *error)
{
  if (vp != 0)
    {
      *error = 0;
      vp->view_direction.x = point->x;
      vp->view_direction.y = point->y;
      vp->view_direction.z = point->z;
    }
  else
    *error = 1;
}

void
dwg_ent_viewport_get_view_direction(dwg_ent_viewport *vp, dwg_point_3d *point,
                                    int *error)
{
  if (vp != 0)
    {
      *error = 0;
      point->x = vp->view_direction.x;
      point->y = vp->view_direction.y;
      point->z = vp->view_direction.z;
    }
  else
    *error = 1;
}

void
dwg_ent_viewport_set_view_twist_angle(dwg_ent_viewport *vp, double angle,
                                      int *error)
{
  if (vp != 0)
    {
      *error = 0;
      vp->view_twist_angle = angle;
    }
  else
    *error = 1;
}

double
dwg_ent_viewport_get_view_twist_angle(dwg_ent_viewport *vp, int *error)
{
  if (vp != 0)
    {
      *error = 0;
      return vp->view_twist_angle;
    }
  else
    *error = 1;
}

void
dwg_ent_viewport_set_view_height(dwg_ent_viewport *vp, double height,
                                 int *error)
{
  if (vp != 0)
    {
      *error = 0;
      vp->view_height = height;
    }
  else
    *error = 1;
}

double
dwg_ent_viewport_get_view_height(dwg_ent_viewport *vp, int *error)
{
  if (vp != 0)
    {
      *error = 0;
      return vp->view_height;
    }
  else
    *error = 1;
}

void
dwg_ent_viewport_set_lens_length(dwg_ent_viewport *vp, double length,
                                 int *error)
{
  if (vp != 0)
    {
      *error = 0;
      vp->lens_length = length;
    }
  else
    *error = 1;
}

double
dwg_ent_viewport_get_lens_length(dwg_ent_viewport *vp, int *error)
{
  if (vp != 0)
    {
      *error = 0;
      return vp->lens_length;
    }
  else
    *error = 1;
}

void
dwg_ent_viewport_set_front_clip_z(dwg_ent_viewport *vp, double front_z,
                                  int *error)
{
  if (vp != 0)
    {
      *error = 0;
      vp->front_clip_z = front_z;
    }
  else
    *error = 1;
}

double
dwg_ent_viewport_get_front_clip_z(dwg_ent_viewport *vp, int *error)
{
  if (vp != 0)
    {
      *error = 0;
      return vp->front_clip_z;
    }
  else
    *error = 1;
}

void
dwg_ent_viewport_set_back_clip_z(dwg_ent_viewport *vp, double back_z,
                                 int *error)
{
  if (vp != 0)
    {
      *error = 0;
      vp->back_clip_z = back_z;
    }
  else
    *error = 1;
}

double
dwg_ent_viewport_get_back_clip_z(dwg_ent_viewport *vp, int *error)
{
  if (vp != 0)
    {
      *error = 0;
      return vp->back_clip_z;
    }
  else
    *error = 1;
}

void
dwg_ent_viewport_set_snap_angle(dwg_ent_viewport *vp, double angle, int *error)
{
  if (vp != 0)
    {
      *error = 0;
      vp->snap_angle = angle;
    }
  else
    *error = 1;
}

double
dwg_ent_viewport_get_snap_angle(dwg_ent_viewport *vp, int *error)
{
  if (vp != 0)
    {
      *error = 0;
      return vp->snap_angle;
    }
  else
    *error = 1;
}

void
dwg_ent_viewport_get_view_center(dwg_ent_viewport *vp, dwg_point_2d *point,
                                 int *error)
{
  if (vp != 0)
    {
      *error = 0;
      point->x = vp->view_center.x;
      point->y = vp->view_center.y;
    }
  else
    *error = 1;
}

void
dwg_ent_viewport_set_view_center(dwg_ent_viewport *vp, dwg_point_2d *point,
                                 int *error)
{
  if (vp != 0)
    {
      *error = 0;
      vp->view_center.x = point->x;
      vp->view_center.y = point->y;
    }
  else
    *error = 1;
}

void
dwg_ent_viewport_get_grid_spacing(dwg_ent_viewport *vp, dwg_point_2d *point,
                                  int *error)
{
  if (vp != 0)
    {
      *error = 0;
      point->x = vp->grid_spacing.x;
      point->y = vp->grid_spacing.y;
    }
  else
    *error = 1;
}

void
dwg_ent_viewport_set_grid_spacing(dwg_ent_viewport *vp, dwg_point_2d *point,
                                  int *error)
{
  if (vp != 0)
    {
      *error = 0;
      vp->grid_spacing.x = point->x;
      vp->grid_spacing.y = point->y;
    }
  else
    *error = 1;
}

void
dwg_ent_viewport_get_snap_base(dwg_ent_viewport *vp, dwg_point_2d *point,
                               int *error)
{
  if (vp != 0)
    {
      *error = 0;
      point->x = vp->snap_base.x;
      point->y = vp->snap_base.y;
    }
  else
    *error = 1;
}

void
dwg_ent_viewport_set_snap_base(dwg_ent_viewport *vp, dwg_point_2d *point,
                               int *error)
{
  if (vp != 0)
    {
      *error = 0;
      vp->snap_base.x = point->x;
      vp->snap_base.y = point->y;
    }
  else
    *error = 1;
}

void
dwg_ent_viewport_get_snap_spacing(dwg_ent_viewport *vp, dwg_point_2d *point,
                                  int *error)
{
  if (vp != 0)
    {
      *error = 0;
      point->x = vp->snap_spacing.x;
      point->y = vp->snap_spacing.y;
    }
  else
    *error = 1;
}

void
dwg_ent_viewport_set_snap_spacing(dwg_ent_viewport *vp, dwg_point_2d *point,
                                  int *error)
{
  if (vp != 0)
    {
      *error = 0;
      vp->snap_spacing.x = point->x;
      vp->snap_spacing.y = point->y;
    }
  else
    *error = 1;
}

void
dwg_ent_viewport_set_ucs_origin(dwg_ent_viewport *vp, dwg_point_3d *point,
                                int *error)
{
  if (vp != 0)
    {
      *error = 0;
      vp->ucs_origin.x = point->x;
      vp->ucs_origin.y = point->y;
      vp->ucs_origin.z = point->z;
    }
  else
    *error = 1;
}

void
dwg_ent_viewport_get_ucs_origin(dwg_ent_viewport *vp, dwg_point_3d *point,
                                int *error)
{
  if (vp != 0)
    {
      *error = 0;
      point->x = vp->ucs_origin.x;
      point->y = vp->ucs_origin.y;
      point->z = vp->ucs_origin.z;
    }
  else
    *error = 1;
}

void
dwg_ent_viewport_set_ucs_x_axis(dwg_ent_viewport *vp, dwg_point_3d *point,
                                int *error)
{
  if (vp != 0)
    {
      *error = 0;
      vp->ucs_x_axis.x = point->x;
      vp->ucs_x_axis.y = point->y;
      vp->ucs_x_axis.z = point->z;
    }
  else
    *error = 1;
}

void
dwg_ent_viewport_get_ucs_x_axis(dwg_ent_viewport *vp, dwg_point_3d *point,
                                int *error)
{
  if (vp != 0)
    {
      *error = 0;
      point->x = vp->ucs_x_axis.x;
      point->y = vp->ucs_x_axis.y;
      point->z = vp->ucs_x_axis.z;
    }
  else
    *error = 1;
}

void
dwg_ent_viewport_set_ucs_y_axis(dwg_ent_viewport *vp, dwg_point_3d *point,
                                int *error)
{
  if (vp != 0)
    {
      *error = 0;
      vp->ucs_y_axis.x = point->x;
      vp->ucs_y_axis.y = point->y;
      vp->ucs_y_axis.z = point->z;
    }
  else
    *error = 1;
}

void
dwg_ent_viewport_get_ucs_y_axis(dwg_ent_viewport *vp, dwg_point_3d *point,
                                int *error)
{
  if (vp != 0)
    {
      *error = 0;
      point->x = vp->ucs_y_axis.x;
      point->y = vp->ucs_y_axis.y;
      point->z = vp->ucs_y_axis.z;
    }
  else
    *error = 1;
}

void
dwg_ent_viewport_set_ucs_elevation(dwg_ent_viewport *vp, double elevation,
                                   int *error)
{
  if (vp != 0)
    {
      *error = 0;
      vp->ucs_elevation = elevation;
    }
  else
    *error = 1;
}

double
dwg_ent_viewport_get_ucs_elevation(dwg_ent_viewport *vp, int *error)
{
  if (vp != 0)
    {
      *error = 0;
      return vp->ucs_elevation;
    }
  else
    *error = 1;
}

void
dwg_ent_viewport_set_ucs_ortho_view_type(dwg_ent_viewport *vp,
                                         unsigned int type, int *error)
{
  if (vp != 0)
    {
      *error = 0;
      vp->ucs_ortho_view_type = type;
    }
  else
    *error = 1;
}

unsigned int
dwg_ent_viewport_get_ucs_ortho_view_type(dwg_ent_viewport *vp, int *error)
{
  if (vp != 0)
    {
      *error = 0;
      return vp->ucs_ortho_view_type;
    }
  else
    *error = 1;
}

void
dwg_ent_viewport_set_shadeplot_mode(dwg_ent_viewport *vp,
                                    unsigned int shadeplot, int *error)
{
  if (vp != 0)
    {
      *error = 0;
      vp->shadeplot_mode = shadeplot;
    }
  else
    *error = 1;
}

unsigned int
dwg_ent_viewport_get_shadeplot_mode(dwg_ent_viewport *vp, int *error)
{
  if (vp != 0)
    {
      *error = 0;
      return vp->shadeplot_mode;
    }
  else
    *error = 1;
}

void
dwg_ent_viewport_set_use_def_lights(dwg_ent_viewport *vp,
                                    unsigned char lights, int *error)
{
  if (vp != 0)
    {
      *error = 0;
      vp->use_def_lights = lights;
    }
  else
    *error = 1;
}

unsigned char 
dwg_ent_viewport_get_use_def_lights(dwg_ent_viewport *vp, int *error)
{
  if (vp != 0)
    {
      *error = 0;
      return vp->use_def_lights;
    }
  else
    *error = 1;
}

void
dwg_ent_viewport_set_def_lighting_type(dwg_ent_viewport *vp, char type,
                                       int *error)
{
  if (vp != 0)
    {
      *error = 0;
      vp->def_lighting_type = type;
    }
  else
    *error = 1;
}

char 
dwg_ent_viewport_get_def_lighting_type(dwg_ent_viewport *vp, int *error)
{
  if (vp != 0)
    {
      *error = 0;
      return vp->def_lighting_type;
    }
  else
    *error = 1;
}

void
dwg_ent_viewport_set_brightness(dwg_ent_viewport *vp, double brightness,
                                int *error)
{
  if (vp != 0)
    {
      *error = 0;
      vp->brightness = brightness;
    }
  else
    *error = 1;
}

double
dwg_ent_viewport_get_brightness(dwg_ent_viewport *vp, int *error)
{
  if (vp != 0)
    {
      *error = 0;
      return vp->brightness;
    }
  else
    *error = 1;
}

void
dwg_ent_viewport_set_contrast(dwg_ent_viewport *vp, double contrast,
                              int *error)
{
  if (vp != 0)
    {
      *error = 0;
      vp->contrast = contrast;
    }
  else
    *error = 1;
}

double
dwg_ent_viewport_get_contrast(dwg_ent_viewport *vp, int *error)
{
  if (vp != 0)
    {
      *error = 0;
      return vp->contrast;
    }
  else
    *error = 1;
}

/*******************************************************************
*                FUNCTIONS FOR POLYLINE PFACE ENTITY                *
********************************************************************/

unsigned int
dwg_ent_polyline_pface_get_numverts(dwg_ent_polyline_pface *pface, int *error)
{
  if (pface != 0)
    {
      *error = 0;
      return pface->numverts;
    }
  else
    {
      *error = 1;
    }
}

void
dwg_ent_polyline_pface_set_numverts(dwg_ent_polyline_pface *pface,
                                    unsigned int numverts, int *error)
{
  if (pface != 0)
    {
      *error = 0;
      pface->numverts = numverts;
    }
  else
    {
      *error = 1;
    }
}

long
dwg_ent_polyline_pface_get_owned_obj_count(dwg_ent_polyline_pface *pface,
                                           int *error)
{
  if (pface != 0)
    {
      *error = 0;
      return pface->owned_obj_count;
    }
  else
    {
      *error = 1;
    }
}

void
dwg_ent_polyline_pface_set_owned_obj_count(dwg_ent_polyline_pface *pface,
                                           long owned_obj_count, int *error)
{
  if (pface != 0)
    {
      *error = 0;
      pface->owned_obj_count = owned_obj_count;
    }
  else
    {
      *error = 1;
    }
}

unsigned int
dwg_ent_polyline_pface_get_numfaces(dwg_ent_polyline_pface *pface, int *error)
{
  if (pface != 0)
    {
      *error = 0;
      return pface->numfaces;
    }
  else
    {
      *error = 1;
    }
}

void
dwg_ent_polyline_pface_set_numfaces(dwg_ent_polyline_pface *pface,
                                    unsigned int numfaces, int *error)
{
  if (pface != 0)
    {
      *error = 0;
      pface->numfaces = numfaces;
    }
  else
    {
      *error = 1;
    }
}

/*******************************************************************
*                FUNCTIONS FOR POLYLINE_MESH ENTITY                 *
********************************************************************/

unsigned int
dwg_ent_polyline_mesh_get_flags(dwg_ent_polyline_mesh *mesh, int *error)
{
  if (mesh != 0)
    {
      *error = 0;
      return mesh->flags;
    }
  else
    {
      *error = 1;
    }
}

void
dwg_ent_polyline_mesh_set_flags(dwg_ent_polyline_mesh *mesh,
                                unsigned int flags, int *error)
{
  if (mesh != 0)
    {
      *error = 0;
      mesh->flags = flags;
    }
  else
    {
      *error = 1;
    }
}

unsigned int
dwg_ent_polyline_mesh_get_curve_type(dwg_ent_polyline_mesh *mesh, int *error)
{
  if (mesh != 0)
    {
      *error = 0;
      return mesh->curve_type;
    }
  else
    {
      *error = 1;
    }
}

void
dwg_ent_polyline_mesh_set_curve_type(dwg_ent_polyline_mesh *mesh,
                                     unsigned int curve_type, int *error)
{
  if (mesh != 0)
    {
      *error = 0;
      mesh->curve_type = curve_type;
    }
  else
    {
      *error = 1;
    }
}

unsigned int
dwg_ent_polyline_mesh_get_m_vert_count(dwg_ent_polyline_mesh *mesh,
                                       int *error)
{
  if (mesh != 0)
    {
      *error = 0;
      return mesh->m_vert_count;
    }
  else
    {
      *error = 1;
    }
}

void
dwg_ent_polyline_mesh_set_m_vert_count(dwg_ent_polyline_mesh *mesh,
                                       unsigned int m_vert_count, int *error)
{
  if (mesh != 0)
    {
      *error = 0;
      mesh->m_vert_count = m_vert_count;
    }
  else
    {
      *error = 1;
    }
}

unsigned int
dwg_ent_polyline_mesh_get_n_vert_count(dwg_ent_polyline_mesh *mesh,
                                       int *error)
{
  if (mesh != 0)
    {
      *error = 0;
      return mesh->n_vert_count;
    }
  else
    {
      *error = 1;
    }
}

void
dwg_ent_polyline_mesh_set_n_vert_count(dwg_ent_polyline_mesh *mesh,
                                       unsigned int n_vert_count, int *error)
{
  if (mesh != 0)
    {
      *error = 0;
      mesh->n_vert_count = n_vert_count;
    }
  else
    {
      *error = 1;
    }
}

unsigned int
dwg_ent_polyline_mesh_get_m_density(dwg_ent_polyline_mesh *mesh, int *error)
{
  if (mesh != 0)
    {
      *error = 0;
      return mesh->m_density;
    }
  else
    {
      *error = 1;
    }
}

void
dwg_ent_polyline_mesh_set_m_density(dwg_ent_polyline_mesh *mesh,
                                    unsigned int m_density, int *error)
{
  if (mesh != 0)
    {
      *error = 0;
      mesh->m_density = m_density;
    }
  else
    {
      *error = 1;
    }
}

unsigned int
dwg_ent_polyline_mesh_get_n_density(dwg_ent_polyline_mesh *mesh, int *error)
{
  if (mesh != 0)
    {
      *error = 0;
      return mesh->n_density;
    }
  else
    {
      *error = 1;
    }
}

void
dwg_ent_polyline_mesh_set_n_density(dwg_ent_polyline_mesh *mesh,
                                    unsigned int n_density, int *error)
{
  if (mesh != 0)
    {
      *error = 0;
      mesh->n_density = n_density;
    }
  else
    {
      *error = 1;
    }
}

long
dwg_ent_polyline_mesh_get_owned_obj_count(dwg_ent_polyline_mesh *mesh,
                                          int *error)
{
  if (mesh != 0)
    {
      *error = 0;
      return mesh->owned_obj_count;
    }
  else
    {
      *error = 1;
    }
}

void
dwg_ent_polyline_mesh_set_owned_obj_count(dwg_ent_polyline_mesh *mesh,
                                          long owned_obj_count, int *error)
{
  if (mesh != 0)
    {
      *error = 0;
      mesh->owned_obj_count = owned_obj_count;
    }
  else
    {
      *error = 1;
    }
}

/*******************************************************************
*                 FUNCTIONS FOR POLYLINE_2D ENTITY                  *
********************************************************************/

void
dwg_ent_polyline_2d_get_extrusion(dwg_ent_polyline_2d *line2d,
                                  dwg_point_3d *point, int *error)
{
  if (line2d != 0)
    {
      *error = 0;
      point->x = line2d->extrusion.x;
      point->y = line2d->extrusion.y;
      point->z = line2d->extrusion.z;
  }
  else
    {
      *error = 1;
    }
}

void
dwg_ent_polyline_2d_set_extrusion(dwg_ent_polyline_2d *line2d,
                                  dwg_point_3d *point, int *error)
{
  if (line2d != 0)
    {
      *error = 0;
      line2d->extrusion.x = point->x;
      line2d->extrusion.y = point->y;
      line2d->extrusion.z = point->z;
  }
  else
    {
      *error = 1;
    }
}

double
dwg_ent_polyline_2d_get_start_width(dwg_ent_polyline_2d *line2d, int *error)
{
  if (line2d != 0)
    {
      *error = 0;
      return line2d->start_width;
    }
  else
    {
      *error = 1;
    }
}

void
dwg_ent_polyline_2d_set_start_width(dwg_ent_polyline_2d *line2d,
                                    double start_width, int *error)
{
  if (line2d != 0)
    {
      *error = 0;
      line2d->start_width = start_width;
    }
  else
    {
      *error = 1;
    }
}

double
dwg_ent_polyline_2d_get_end_width(dwg_ent_polyline_2d *line2d, int *error)
{
  if (line2d != 0)
    {
      *error = 0;
      return line2d->end_width;
    }
  else
    {
      *error = 1;
    }
}

void
dwg_ent_polyline_2d_set_end_width(dwg_ent_polyline_2d *line2d,
                                  double end_width, int *error)
{
  if (line2d != 0)
    {
      *error = 0;
      line2d->end_width = end_width;
    }
  else
    {
      *error = 1;
    }
}

double
dwg_ent_polyline_2d_get_thickness(dwg_ent_polyline_2d *line2d, int *error)
{
  if (line2d != 0)
    {
      *error = 0;
      return line2d->thickness;
    }
  else
    {
      *error = 1;
    }
}

void
dwg_ent_polyline_2d_set_thickness(dwg_ent_polyline_2d *line2d,
                                  double thickness, int *error)
{
  if (line2d != 0)
    {
      *error = 0;
      line2d->thickness = thickness;
    }
  else
    {
      *error = 1;
    }
}

double
dwg_ent_polyline_2d_get_elevation(dwg_ent_polyline_2d *line2d, int *error)
{
  if (line2d != 0)
    {
      *error = 0;
      return line2d->elevation;
    }
  else
    {
      *error = 1;
    }
}

void
dwg_ent_polyline_2d_set_elevation(dwg_ent_polyline_2d *line2d,
                                  double elevation, int *error)
{
  if (line2d != 0)
    {
      *error = 0;
      line2d->elevation = elevation;
    }
  else
    {
      *error = 1;
    }
}

unsigned int
dwg_ent_polyline_2d_get_flags(dwg_ent_polyline_2d *line2d, int *error)
{
  if (line2d != 0)
    {
      *error = 0;
      return line2d->flags;
    }
  else
    {
      *error = 1;
    }
}

void
dwg_ent_polyline_2d_set_flags(dwg_ent_polyline_2d *line2d, unsigned int flags,
                              int *error)
{
  if (line2d != 0)
    {
      *error = 0;
      line2d->flags = flags;
    }
  else
    {
      *error = 1;
    }
}

unsigned int
dwg_ent_polyline_2d_get_curve_type(dwg_ent_polyline_2d *line2d, int *error)
{
  if (line2d != 0)
    {
      *error = 0;
      return line2d->curve_type;
    }
  else
    {
      *error = 1;
    }
}

void
dwg_ent_polyline_2d_set_curve_type(dwg_ent_polyline_2d *line2d,
                                   unsigned int curve_type, int *error)
{
  if (line2d != 0)
    {
      *error = 0;
      line2d->curve_type = curve_type;
    }
  else
    {
      *error = 1;
    }
}

long
dwg_ent_polyline_2d_get_owned_obj_count(dwg_ent_polyline_2d *line2d,
                                        int *error)
{
  if (line2d != 0)
    {
      *error = 0;
      return line2d->owned_obj_count;
    }
  else
    {
      *error = 1;
    }
}

void
dwg_ent_polyline_2d_set_owned_obj_count(dwg_ent_polyline_2d *line2d,
                                        long owned_obj_count, int *error)
{
  if (line2d != 0)
    {
      *error = 0;
      line2d->owned_obj_count = owned_obj_count;
    }
  else
    {
      *error = 1;
    }
}

/*******************************************************************
*                 FUNCTIONS FOR POLYLINE_3D ENTITY                  *
********************************************************************/

char
dwg_ent_polyline_3d_get_flags_1(dwg_ent_polyline_3d *line3d, int *error)
{
  if (line3d != 0)
    {
      *error = 0;
      return line3d->flags_1;
    }
  else
    {
      *error = 1;
    }
}

void
dwg_ent_polyline_3d_set_flags_1(dwg_ent_polyline_3d *line3d, char flags_1,
                                int *error)
{
  if (line3d != 0)
    {
      *error = 0;
      line3d->flags_1 = flags_1;
    }
  else
    {
      *error = 1;
    }
}

char
dwg_ent_polyline_3d_get_flags_2(dwg_ent_polyline_3d *line3d, int *error)
{
  if (line3d != 0)
    {
      *error = 0;
      return line3d->flags_2;
    }
  else
    {
      *error = 1;
    }
}

void
dwg_ent_polyline_3d_set_flags_2(dwg_ent_polyline_3d *line3d, char flags_2,
                                int *error)
{
  if (line3d != 0)
    {
      *error = 0;
      line3d->flags_2 = flags_2;
    }
  else
    {
      *error = 1;
    }
}

long
dwg_ent_polyline_3d_get_owned_obj_count(dwg_ent_polyline_3d *line3d,
                                        int *error)
{
  if (line3d != 0)
    {
      *error = 0;
      return line3d->owned_obj_count;
    }
  else
    {
      *error = 1;
    }
}

void
dwg_ent_polyline_3d_set_owned_obj_count(dwg_ent_polyline_3d *line3d,
                                        long owned_obj_count, int *error)
{
  if (line3d != 0)
    {
      *error = 0;
      line3d->owned_obj_count = owned_obj_count;
    }
  else
    {
      *error = 1;
    }
}

/*******************************************************************
*                   FUNCTIONS FOR 3DFACE ENTITY                     *
********************************************************************/

/// This returns the elevation of a _3dface entity.
/** Usage :- dwg_ent_3dface_get_invis_flags(_3dface);
*/
unsigned int
dwg_ent_3dface_get_invis_flags(dwg_ent_3dface *_3dface, int *error)
{    
  if (_3dface != 0)
    {
      *error = 0;
      return _3dface->invis_flags;
    }
  else
    *error = 1;
}

/// This sets the invis_flags of a _3dface entity equal to value in second argument.
/** Usage :- dwg_ent_3dface_set_invis_flags(_3dface, 20);
Hence invis_flags sets to 20
*/
void
dwg_ent_3dface_set_invis_flags(dwg_ent_3dface *_3dface,
                               unsigned int invis_flags, int *error)
{    
  if (_3dface != 0)
    {
      *error = 0;
      _3dface->invis_flags = invis_flags;
    }
  else
    *error = 1;
}

/// Returns the corner1 values of _3dface to second argument.
/** Usage :- dwg_ent_3dface_get_corner1(_3dface, point);
where _3dface is a pointer of dwg_ent_3dface type and point is 
of dwg_point_3d. point.x will contain x value of corner1,
point.y will contain y value of corner1
*/
void
dwg_ent_3dface_get_corner1(dwg_ent_3dface *_3dface, dwg_point_2d *point,
                           int *error)
{    
  if (_3dface != 0)
    {
      *error = 0;
      point->x = _3dface->corner1.x;
      point->y = _3dface->corner1.y;
    }
  else
    *error = 1;
}

/// Sets the _3dface corner1 values equal to values of second argument.
/** Usage :- dwg_ent_3dface_set_corner1(_3dface, point);
where _3dface is a pointer of dwg_ent_3dface type and point is 
of dwg_point_3d. point.x will contain x value of corner1,
point.y will contain y value of corner1
*/
void
dwg_ent_3dface_set_corner1(dwg_ent_3dface *_3dface, dwg_point_2d *point,
                           int *error)
{    
  if (_3dface != 0)
    {
      *error = 0;
      _3dface->corner1.x = point->x;
      _3dface->corner1.y = point->y;
    }
  else
    *error = 1;
}

/// Returns the corner2 values of _3dface to second argument.
/** Usage :- dwg_ent_3dface_get_corner2(_3dface, point);
where _3dface is a pointer of dwg_ent_3dface type and point is 
of dwg_point_3d. point.x will contain x value of corner2,
point.y will contain y value of corner2
*/
void
dwg_ent_3dface_get_corner2(dwg_ent_3dface *_3dface, dwg_point_2d *point,
                           int *error)
{    
  if (_3dface != 0)
    {
      *error = 0;
      point->x = _3dface->corner2.x;
      point->y = _3dface->corner2.y;
    }
  else
    *error = 1;
}

/// Sets the _3dface corner2 values equal to values of second argument.
/** Usage :- dwg_ent_3dface_set_corner2(_3dface, point);
where _3dface is a pointer of dwg_ent_3dface type and point is 
of dwg_point_3d. point.x will contain x value of corner2,
point.y will contain y value of corner2
*/
void
dwg_ent_3dface_set_corner2(dwg_ent_3dface *_3dface, dwg_point_2d *point,
                           int *error)
{    
  if (_3dface != 0)
    {
      *error = 0;
      _3dface->corner2.x = point->x;
      _3dface->corner2.y = point->y;
    }
  else
    *error = 1;
}

/// Returns the corner3 values of _3dface to second argument.
/** Usage :- dwg_ent_3dface_get_corner3(_3dface, point);
where _3dface is a pointer of dwg_ent_3dface type and point is 
of dwg_point_3d. point.x will contain x value of corner3,
point.y will contain y value of corner3
*/
void
dwg_ent_3dface_get_corner3(dwg_ent_3dface *_3dface, dwg_point_2d *point,
                           int *error)
{    
  if (_3dface != 0)
    {
      *error = 0;
      point->x = _3dface->corner3.x;
      point->y = _3dface->corner3.y;
    }
  else
    *error = 1;
}

/// Sets the _3dface corner3 values equal to values of second argument.
/** Usage :- dwg_ent_3dface_set_corner3(_3dface, point);
where _3dface is a pointer of dwg_ent_3dface type and point is 
of dwg_point_3d. point.x will contain x value of corner3,
point.y will contain y value of corner3
*/
void
dwg_ent_3dface_set_corner3(dwg_ent_3dface *_3dface, dwg_point_2d *point,
                           int *error)
{    
  if (_3dface != 0)
    {
      *error = 0;
      _3dface->corner3.x = point->x;
      _3dface->corner3.y = point->y;
    }
  else
    *error = 1;
}

/// Returns the corner4 values of _3dface to second argument.
/** Usage :- dwg_ent_3dface_get_corner4(_3dface, point);
where _3dface is a pointer of dwg_ent_3dface type and point is 
of dwg_point_3d. point.x will contain x value of corner4,
point.y will contain y value of corner4
*/
void
dwg_ent_3dface_get_corner4(dwg_ent_3dface *_3dface, dwg_point_2d *point,
                           int *error)
{    
  if (_3dface != 0)
    {
      *error = 0;
      point->x = _3dface->corner4.x;
      point->y = _3dface->corner4.y;
    }
  else
    *error = 1;
}

/// Sets the _3dface corner4 values equal to values of second argument.
/** Usage :- dwg_ent_3dface_set_corner4(_3dface, point);
where _3dface is a pointer of dwg_ent_3dface type and point is 
of dwg_point_3d. point.x will contain x value of corner4,
point.y will contain y value of corner4
*/
void
dwg_ent_3dface_set_corner4(dwg_ent_3dface *_3dface, dwg_point_2d *point,
                           int *error)
{    
  if (_3dface != 0)
    {
      *error = 0;
      _3dface->corner4.x = point->x;
      _3dface->corner4.y = point->y;
    }
  else
    *error = 1;
}

/*******************************************************************
*                    FUNCTIONS FOR IMAGE ENTITY                     *
********************************************************************/

long
dwg_ent_image_get_class_version(dwg_ent_image *image, int *error)
{
  if (image != 0)
    {
      *error = 0;
      return image->class_version;
    }
  else
    *error = 1;
}

void
dwg_ent_image_set_class_version(dwg_ent_image *image, long class_version,
                                int *error)
{
  if (image != 0)
    {
      *error = 0;
      image->class_version = class_version;
    }
  else
    *error = 1;
}

void
dwg_ent_image_get_pt0(dwg_ent_image *image, dwg_point_3d *point, int *error)
{
  if (image != 0)
    {
      *error = 0;
      point->x = image->pt0.x;
      point->y = image->pt0.y;
      point->z = image->pt0.z;
    }
  else
    *error = 1;
}

void
dwg_ent_image_set_pt0(dwg_ent_image *image, dwg_point_3d *point, int *error)
{
  if (image != 0)
    {
      *error = 0;
      image->pt0.x = point->x;
      image->pt0.y = point->y;
      image->pt0.z = point->z;
    }
  else
    *error = 1;
}

void
dwg_ent_image_get_u_vector(dwg_ent_image *image, dwg_point_3d *point,
 int *error)
{
  if (image != 0)
    {
      *error = 0;
      point->x = image->uvec.x;
      point->y = image->uvec.y;
      point->z = image->uvec.z;
    }
  else
    *error = 1;
}

void
dwg_ent_image_set_u_vector(dwg_ent_image *image, dwg_point_3d *point,
                           int *error)
{
  if (image != 0)
    {
      *error = 0;
      image->uvec.x = point->x ;
      image->uvec.y = point->y ;
      image->uvec.z = point->z ;
    }
  else
    *error = 1;
}

void
dwg_ent_image_get_v_vector(dwg_ent_image *image, dwg_point_3d *point,
                           int *error)
{
  if (image != 0)
    {
      *error = 0;
      point->x = image->vvec.x;
      point->y = image->vvec.y;
      point->z = image->vvec.z;
    }
  else
    *error = 1;
}

void
dwg_ent_image_set_v_vector(dwg_ent_image *image, dwg_point_3d *point,
                           int *error)
{
  if (image != 0)
    {
      *error = 0;
      image->vvec.x = point->x ;
      image->vvec.y = point->y ;
      image->vvec.z = point->z ;
    }
  else
    *error = 1;
}

double
dwg_ent_image_get_size_height(dwg_ent_image *image, int *error)
{
  if (image != 0)
    {
      *error = 0;
      return image->size.height;
    }
  else
    *error = 1;
}

void
dwg_ent_image_set_size_height(dwg_ent_image *image, double size_height,
                              int *error)
{
  if (image != 0)
    {
      *error = 0;
      image->size.height = size_height;
    }
  else
    *error = 1;
}

double
dwg_ent_image_get_size_width(dwg_ent_image *image, int *error)
{
  if (image != 0)
    {
      *error = 0;
      return image->size.width;
    }
  else
    *error = 1;
}

void
dwg_ent_image_set_size_width(dwg_ent_image *image, double size_width,
                             int *error)
{
  if (image != 0)
    {
      *error = 0;
      image->size.width = size_width;
    }
  else
    *error = 1;
}

unsigned int
dwg_ent_image_get_display_props(dwg_ent_image *image, int *error)
{
  if (image != 0)
    {
      *error = 0;
      return image->display_props;
    }
  else
    *error = 1;
}

void      
dwg_ent_image_set_display_props(dwg_ent_image *image,
                                unsigned int display_props, int *error)
{
  if (image != 0)
    {
      *error = 0;
      image->display_props = display_props;
    }
  else
    *error = 1;
}

unsigned char
dwg_ent_image_get_clipping(dwg_ent_image *image, int *error)
{
  if (image != 0)
    {
      *error = 0;
      return image->clipping;
    }
  else
    *error = 1;
}

void       
dwg_ent_image_set_clipping(dwg_ent_image *image, unsigned char clipping,
                           int *error)
{
  if (image != 0)
    {
      *error = 0;
      image->clipping = clipping;
    }
  else
    *error = 1;
}

char
dwg_ent_image_get_brightness(dwg_ent_image *image, int *error)
{
  if (image != 0)
    {
      *error = 0;
      return image->brightness;
    }
  else
    *error = 1;
}

void
dwg_ent_image_set_brightness(dwg_ent_image *image, char brightness, int *error)
{
  if (image != 0)
    {
      *error = 0;
      image->brightness = brightness;
    }
  else
    *error = 1;
}

char
dwg_ent_image_get_contrast(dwg_ent_image *image, int *error)
{
  if (image != 0)
    {
      *error = 0;
      return image->contrast;
    }
  else
    *error = 1;
}

void
dwg_ent_image_set_contrast(dwg_ent_image *image, char contrast, int *error)
{
  if (image != 0)
    {
      *error = 0;
      image->contrast = contrast;
    }
  else
    *error = 1;
}

char
dwg_ent_image_get_fade(dwg_ent_image *image, int *error)
{
  if (image != 0)
    {
      *error = 0;
      return image->fade;
    }
  else
    *error = 1;
}

void
dwg_ent_image_set_fade(dwg_ent_image *image, char fade, int *error)
{
  if (image != 0)
    {
      *error = 0;
      image->fade = fade;
    }
  else
    *error = 1;
}

unsigned int
dwg_ent_image_get_clip_boundary_type(dwg_ent_image *image, int *error)
{
  if (image != 0)
    {
      *error = 0;
      return image->clip_boundary_type;
    }
  else
    *error = 1;
}

void      
dwg_ent_image_set_clip_boundary_type(dwg_ent_image *image, unsigned int type,
                                     int *error)
{
  if (image != 0)
    {
      *error = 0;
      image->clip_boundary_type = type;
    }
  else
    *error = 1;
}

void
dwg_ent_image_get_boundary_pt0(dwg_ent_image *image, dwg_point_2d *point,
                               int *error)
{
  if (image != 0)
    {
      *error = 0;
      point->x = image->boundary_pt0.x;
      point->y = image->boundary_pt0.y;
    }
  else
    *error = 1;
}

void
dwg_ent_image_set_boundary_pt0(dwg_ent_image *image, dwg_point_2d *point,
                               int *error)
{
  if (image != 0)
    {
      *error = 0;
      image->boundary_pt0.x = point->x;
      image->boundary_pt0.y = point->y;
    }
  else
    *error = 1;
}

void
dwg_ent_image_get_boundary_pt1(dwg_ent_image *image, dwg_point_2d *point,
                               int *error)
{
  if (image != 0)
    {
      *error = 0;
      point->x = image->boundary_pt1.x;
      point->y = image->boundary_pt1.y;
    }
  else
    *error = 1;
}

void
dwg_ent_image_set_boundary_pt1(dwg_ent_image *image, dwg_point_2d *point,
                               int *error)
{
  if (image != 0)
    {
      *error = 0;
      point->x = image->boundary_pt1.x;
      point->y = image->boundary_pt1.y;
    }
  else
    *error = 1;
}

double
dwg_ent_image_get_num_clip_verts(dwg_ent_image *image, int *error)
{
  if (image != 0)
    {
      *error = 0;
      return image->num_clip_verts;
    }
  else
    *error = 1;
}

void
dwg_ent_image_set_num_clip_verts(dwg_ent_image *image, double num, int *error)
{
  if (image != 0)
    {
      *error = 0;
      image->num_clip_verts = num;
    }
  else
    *error = 1;
}

dwg_ent_image_clip_vert *
dwg_ent_image_get_clip_verts(dwg_ent_image *image, int *error)
{
  dwg_ent_image_clip_vert *ptx = (dwg_ent_image_clip_vert*) 
  malloc(sizeof(dwg_ent_image_clip_vert)* image->num_clip_verts);
    if(ptx != 0)
      {
        *error = 0;
        int i;
          for (i = 0; i < image->num_clip_verts ; i++)
            {
              ptx[i] = image->clip_verts[i];
            }
        return ptx;
      }
    else
      *error = 1;
}

/*******************************************************************
*                    FUNCTIONS FOR MLINE ENTITY                     *
********************************************************************/

void
dwg_ent_mline_set_scale(dwg_ent_mline *mline, double scale, int *error)
{
  if (mline != 0)
    {
      *error = 0;
      mline->scale = scale;
    }
  else
    *error = 1;
}

double
dwg_ent_mline_get_scale(dwg_ent_mline *mline, int *error)
{
  if (mline != 0)
    {
      *error = 0;
      return mline->scale;
    }
  else
    *error = 1;
}

void
dwg_ent_mline_set_just(dwg_ent_mline *mline, char just, int *error)
{
  if (mline != 0)
    {
      *error = 0;
      mline->just = just;
    }
  else
    *error = 1;
}

char
dwg_ent_mline_get_just(dwg_ent_mline *mline, int *error)
{
  if (mline != 0)
    {
      *error = 0;
      return mline->just;
    }
  else
    *error = 1;
}

void
dwg_ent_mline_set_base_point(dwg_ent_mline *mline, dwg_point_3d *point,
                             int *error)
{
  if (mline != 0)
    {
      *error = 0;
      mline->base_point.x = point->x;
      mline->base_point.y = point->y;
      mline->base_point.z = point->z;
    }
  else
    *error = 1;
}

void
dwg_ent_mline_get_base_point(dwg_ent_mline *mline, dwg_point_3d *point,
                             int *error)
{
  if (mline != 0)
    {
      *error = 0;
      point->x = mline->base_point.x;
      point->y = mline->base_point.y;
      point->z = mline->base_point.z;
    }
  else
    *error = 1;
}

void
dwg_ent_mline_set_extrusion(dwg_ent_mline *mline, dwg_point_3d *point,
                            int *error)
{
  if (mline != 0)
    {
      *error = 0;
      mline->extrusion.x = point->x;
      mline->extrusion.y = point->y;
      mline->extrusion.z = point->z;
    }
  else
    *error = 1;
}

void
dwg_ent_mline_get_extrusion(dwg_ent_mline *mline, dwg_point_3d *point,
                            int *error)
{
  if (mline != 0)
    {
      *error = 0;
      point->x = mline->extrusion.x;
      point->y = mline->extrusion.y;
      point->z = mline->extrusion.z;
    }
  else
    *error = 1;
}

void
dwg_ent_mline_set_open_closed(dwg_ent_mline *mline, unsigned int oc,
                              int *error)
{
  if (mline != 0)
    {
      *error = 0;
      mline->open_closed = oc;
    }
  else
    *error = 1;
}

unsigned int
dwg_ent_mline_get_open_closed(dwg_ent_mline *mline, int *error)
{
  if (mline != 0)
    {
      *error = 0;
      return mline->open_closed;
    }
  else
    *error = 1;
}

void
dwg_ent_mline_set_num_lines(dwg_ent_mline *mline, char num, int *error)
{
  if (mline != 0)
    {
      *error = 0;
      mline->num_lines = num;
    }
  else
    *error = 1;
}

char
dwg_ent_mline_get_num_lines(dwg_ent_mline *mline, int *error)
{
  if (mline != 0)
    {
      *error = 0;
      return mline->num_lines;
    }
  else
    *error = 1;
}

void
dwg_ent_mline_set_num_verts(dwg_ent_mline *mline, unsigned int num,
                            int *error)
{
  if (mline != 0)
    {
      *error = 0;
      mline->num_verts = num;
    }
  else
    *error = 1;
}

unsigned int
dwg_ent_mline_get_num_verts(dwg_ent_mline *mline, int *error)
{
  if (mline != 0)
    {
      *error = 0;
      return mline->num_verts;
    }
  else
    *error = 1;
}
dwg_ent_mline_vert *
dwg_ent_mline_get_verts(dwg_ent_mline *mline, int *error)
{
  dwg_ent_mline_vert *ptx = (dwg_ent_mline_vert*) 
  malloc(sizeof(dwg_ent_mline_vert)* mline->num_verts);
    if(ptx != 0)
      {
        *error = 0;
        int i = 0;
        for (i = 0; i < mline->num_verts ; i++)
          {
          ptx[i] = mline->verts[i];
          }
        return ptx;
      }
    else
      *error = 1;
}

/*******************************************************************
*              FUNCTIONS FOR VERTEX_PFACE_FACE ENTITY               *
********************************************************************/

unsigned int
dwg_ent_vertex_pface_face_set_vertind(dwg_ent_vert_pface_face *face)
{
  return face->vertind[4];
}

void
dwg_ent_vertex_pface_face_get_vertind(dwg_ent_vert_pface_face *face,
                                           unsigned int vertind[4])
{
  face->vertind[1] = vertind[1];
  face->vertind[2] = vertind[2];
  face->vertind[3] = vertind[3];
  face->vertind[4] = vertind[4];
}

/*******************************************************************
*                  FUNCTIONS FOR 3DSOLID ENTITY                     *
********************************************************************/

unsigned char
dwg_ent_3dsolid_get_acis_empty(dwg_ent_3dsolid *_3dsolid, int *error)
{
  if (_3dsolid != 0)
    {
      *error = 0;
      return _3dsolid->acis_empty;
    }
  else
    *error = 1;
}

void
dwg_ent_3dsolid_set_acis_empty(dwg_ent_3dsolid *_3dsolid, unsigned char acis,
                               int *error)
{
  if (_3dsolid != 0)
    {
      *error = 0;
      _3dsolid->acis_empty = acis;
    }
  else
    *error = 1;
}

unsigned int
dwg_ent_3dsolid_get_version(dwg_ent_3dsolid *_3dsolid, int *error)
{
  if (_3dsolid != 0)
    {
      *error = 0;
      return _3dsolid->version;
    }
  else
    *error = 1;
}

void
dwg_ent_3dsolid_set_version(dwg_ent_3dsolid *_3dsolid, unsigned int version,
                            int *error)
{
  if (_3dsolid != 0)
    {
      *error = 0;
      _3dsolid->version = version;
    }
  else
    *error = 1;
}

long *
dwg_ent_3dsolid_get_block_size(dwg_ent_3dsolid *_3dsolid, int *error)
{
  if (_3dsolid != 0)
    {
      *error = 0;
      return _3dsolid->block_size;
    }
  else
    *error = 1;
}

char *dwg_ent_3dsolid_get_acis_data(dwg_ent_3dsolid *_3dsolid, int *error)
{
  if (_3dsolid != 0)
    {
      *error = 0;
      return _3dsolid->acis_data;
    }
  else
    *error = 1;
}

char *
dwg_ent_3dsolid_set_acis_data(dwg_ent_3dsolid *_3dsolid,
                                     char * data, int *error)
{
  if (_3dsolid != 0)
    {
      *error = 0;
      _3dsolid->acis_data = data;
    }
  else
    *error = 1;
}

char
dwg_ent_3dsolid_get_wireframe_data_present(dwg_ent_3dsolid *_3dsolid,
                                           int *error)
{
  if (_3dsolid != 0)
    {
      *error = 0;
      return _3dsolid->wireframe_data_present;
    }
  else
    *error = 1;
}

void
dwg_ent_3dsolid_set_wireframe_data_present(dwg_ent_3dsolid *_3dsolid,
                                           char data, int *error)
{
  if (_3dsolid != 0)
    {
      *error = 0;
      _3dsolid->wireframe_data_present = data;
    }
  else
    *error = 1;
}

char
dwg_ent_3dsolid_get_point_present(dwg_ent_3dsolid *_3dsolid, int *error)
{
  if (_3dsolid != 0)
    {
      *error = 0;
      return _3dsolid->point_present;
    }
  else
    *error = 1;
}

void
dwg_ent_3dsolid_set_point_present(dwg_ent_3dsolid *_3dsolid, char point,
                                  int *error)
{
  if (_3dsolid != 0)
    {
      *error = 0;
      _3dsolid->point_present = point;
    }
  else
    *error = 1;
}

void
dwg_ent_3dsolid_get_point(dwg_ent_3dsolid *_3dsolid, dwg_point_3d *point, 
                          int *error)
{
  if (_3dsolid != 0)
    {
      *error = 0;
      point->x = _3dsolid->point.x;
      point->y = _3dsolid->point.y;
      point->z = _3dsolid->point.z;
    }
  else
    *error = 1;
}

void
dwg_ent_3dsolid_set_point(dwg_ent_3dsolid *_3dsolid, dwg_point_3d *point,
                          int *error)
{
  if (_3dsolid != 0)
    {
      *error = 0;
      _3dsolid->point.x = point->x;
      _3dsolid->point.y = point->y;
      _3dsolid->point.z = point->z;
    }
  else
    *error = 1;
}

long
dwg_ent_3dsolid_get_num_isolines(dwg_ent_3dsolid *_3dsolid, int *error)
{
  if (_3dsolid != 0)
    {
      *error = 0;
      return _3dsolid->num_isolines;
    }
  else
    *error = 1;
}

void
dwg_ent_3dsolid_set_num_isolines(dwg_ent_3dsolid *_3dsolid, long num,
                                 int *error)
{
  if (_3dsolid != 0)
    {
      *error = 0;
      _3dsolid->num_isolines = num;
    }
  else
    *error = 1;
}

char
dwg_ent_3dsolid_get_isoline_present(dwg_ent_3dsolid *_3dsolid, int *error)
{
  if (_3dsolid != 0)
    {
      *error = 0;
      return _3dsolid->isoline_present;
    }
  else
    *error = 1;
}

void
dwg_ent_3dsolid_set_isoline_present(dwg_ent_3dsolid *_3dsolid, char iso,
                                    int *error)
{
  if (_3dsolid != 0)
    {
      *error = 0;
      _3dsolid->isoline_present = iso;
    }
  else
    *error = 1;
}

long
dwg_ent_3dsolid_get_num_wires(dwg_ent_3dsolid *_3dsolid, int *error)
{
  if (_3dsolid != 0)
    {
      *error = 0;
      return _3dsolid->num_wires;
    }
  else
    *error = 1;
}

void
dwg_ent_3dsolid_set_num_wires(dwg_ent_3dsolid *_3dsolid, long num, int *error)
{
  if (_3dsolid != 0)
    {
      *error = 0;
      _3dsolid->num_wires = num;
    }
  else
    *error = 1;
}

dwg_ent_solid_wire *
dwg_ent_3dsolid_get_wire(dwg_ent_3dsolid *_3dsolid, int *error)
{
  dwg_ent_solid_wire *ptx = (dwg_ent_solid_wire*)
  malloc(sizeof(dwg_ent_solid_wire)* _3dsolid->num_wires);
    if(ptx != 0)
      {
        *error = 0;
        int i = 0;
        for (i = 0; i < _3dsolid->num_wires ; i++)
          {
            ptx[i] = _3dsolid->wires[i];
          }
        return ptx;
      }
    else
      *error = 1;
}

long
dwg_ent_3dsolid_get_num_silhouettes(dwg_ent_3dsolid *_3dsolid, int *error)
{
  if (_3dsolid != 0)
    {
      *error = 0;
      return _3dsolid->num_silhouettes;
    }
  else
    *error = 1;
}

void
dwg_ent_3dsolid_set_num_silhouettes(dwg_ent_3dsolid *_3dsolid,
                                    long silhouettes, int *error)
{
  if (_3dsolid != 0)
    {
      *error = 0;
      _3dsolid->num_silhouettes = silhouettes;
    }
  else
    *error = 1;
}

dwg_ent_solid_silhouette *
dwg_ent_3dsolid_get_silhouette(dwg_ent_3dsolid *_3dsolid, int *error)
{
  dwg_ent_solid_silhouette *ptx = (dwg_ent_solid_silhouette*)
  malloc(sizeof(dwg_ent_solid_silhouette)* _3dsolid->num_silhouettes);
    if(ptx != 0)
      {
        *error = 0;
        int i = 0;
          for (i = 0; i < _3dsolid->num_silhouettes ; i++)
            {
              ptx[i] = _3dsolid->silhouettes[i];
            }
        return ptx;
      }
    else
      *error = 1;
}

unsigned char
dwg_ent_3dsolid_get_acis_empty2(dwg_ent_3dsolid *_3dsolid, int *error)
{
  if (_3dsolid != 0)
    {
      *error = 0;
      return _3dsolid->acis_empty2;
    }
  else
    *error = 1;
}

void
dwg_ent_3dsolid_set_acis_empty2(dwg_ent_3dsolid *_3dsolid, unsigned char acis,
                                int *error)
{
  if (_3dsolid != 0)
    {
      *error = 0;
      _3dsolid->acis_empty2 = acis;
    }
  else
    *error = 1;
} 

/*******************************************************************
*                   FUNCTIONS FOR REGION ENTITY                     *
********************************************************************/

unsigned char
dwg_ent_region_get_acis_empty(dwg_ent_region *region, int *error)
{
  if (region != 0)
    {
      *error = 0;
      return region->acis_empty;
    }
  else
    *error = 1;
}

void
dwg_ent_region_set_acis_empty(dwg_ent_region *region, unsigned char acis,
                              int *error)
{
  if (region != 0)
    {
      *error = 0;
      region->acis_empty = acis;
    }
  else
    *error = 1;
}

unsigned int
dwg_ent_region_get_version(dwg_ent_region *region, int *error)
{
  if (region != 0)
    {
      *error = 0;
      return region->version;
    }
  else
    *error = 1;
}

void
dwg_ent_region_set_version(dwg_ent_region *region, unsigned int version,
                           int *error)
{
  if (region != 0)
    {
      *error = 0;
      region->version = version;
    }
  else
    *error = 1;
}

long *
dwg_ent_region_get_block_size(dwg_ent_region *region, int *error)
{
  if (region != 0)
    {
      *error = 0;
      return region->block_size;
    }
  else
    *error = 1;
}

char *dwg_ent_region_get_acis_data(dwg_ent_region *region, int *error)
{
  if (region != 0)
    {
      *error = 0;
      return region->acis_data;
    }
  else
    *error = 1;
}

char *
dwg_ent_region_set_acis_data(dwg_ent_region *region, char * data,
                                    int *error)
{
  if (region != 0)
    {
      *error = 0;
      region->acis_data = data;
    }
  else
    *error = 1;
}

char
dwg_ent_region_get_wireframe_data_present(dwg_ent_region *region, int *error)
{
  if (region != 0)
    {
      *error = 0;
      return region->wireframe_data_present;
    }
  else
    *error = 1;
}

void
dwg_ent_region_set_wireframe_data_present(dwg_ent_region *region, char data,
                                          int *error)
{
  if (region != 0)
    {
      *error = 0;
      region->wireframe_data_present = data;
    }
  else
    *error = 1;
}

char
dwg_ent_region_get_point_present(dwg_ent_region *region, int *error)
{
  if (region != 0)
    {
      *error = 0;
      return region->point_present;
    }
  else
    *error = 1;
}

void
dwg_ent_region_set_point_present(dwg_ent_region *region, char point,
                                 int *error)
{
  if (region != 0)
    {
      *error = 0;
      region->point_present = point;
    }
  else
    *error = 1;
}

void
dwg_ent_region_get_point(dwg_ent_region *region, dwg_point_3d *point,
                         int *error)
{
  if (region != 0)
    {
      *error = 0;
      point->x = region->point.x;
      point->y = region->point.y;
      point->z = region->point.z;
    }
  else
    *error = 1;
}

void
dwg_ent_region_set_point(dwg_ent_region *region, dwg_point_3d *point,
                         int *error)
{
  if (region != 0)
    {
      *error = 0;
      region->point.x = point->x;
      region->point.y = point->y;
      region->point.z = point->z;
    }
  else
    *error = 1;
}

long
dwg_ent_region_get_num_isolines(dwg_ent_region *region, int *error)
{
  if (region != 0)
    {
      *error = 0;
      return region->num_isolines;
    }
  else
    *error = 1;
}

void
dwg_ent_region_set_num_isolines(dwg_ent_region *region, long num, int *error)
{
  if (region != 0)
    {
      *error = 0;
      region->num_isolines = num;
    }
  else
    *error = 1;
}

char
dwg_ent_region_get_isoline_present(dwg_ent_region *region, int *error)
{
  if (region != 0)
    {
      *error = 0;
      return region->isoline_present;
    }
  else
    *error = 1;
}

void
dwg_ent_region_set_isoline_present(dwg_ent_region *region, char iso,
                                   int *error)
{
  if (region != 0)
    {
      *error = 0;
      region->isoline_present = iso;
    }
  else
    *error = 1;
}

long
dwg_ent_region_get_num_wires(dwg_ent_region *region, int *error)
{
  if (region != 0)
    {
      *error = 0;
      return region->num_wires;
    }
  else
    *error = 1;
}

void
dwg_ent_region_set_num_wires(dwg_ent_region *region, long num, int *error)
{
  if (region != 0)
    {
      *error = 0;
      region->num_wires = num;
    }
  else
    *error = 1;
}

dwg_ent_solid_wire *
dwg_ent_region_get_wire(dwg_ent_region *region, int *error)
{
  dwg_ent_solid_wire *ptx = (dwg_ent_solid_wire*)
  malloc(sizeof(dwg_ent_solid_wire)* region->num_wires);
    if(ptx != 0)
      {
        *error = 0;
        int i = 0;
          for (i = 0; i < region->num_wires ; i++)
            {
              ptx[i] = region->wires[i];
            }
        return ptx;
      }
    else
      *error = 1;
}

long
dwg_ent_region_get_num_silhouettes(dwg_ent_region *region, int *error)
{
  if (region != 0)
    {
      *error = 0;
      return region->num_silhouettes;
    }
  else
    *error = 1;
}

void
dwg_ent_region_set_num_silhouettes(dwg_ent_region *region, long silhouettes,
                                   int *error)
{
  if (region != 0)
    {
      *error = 0;
      region->num_silhouettes = silhouettes;
    }
  else
    *error = 1;
}

dwg_ent_solid_silhouette *
dwg_ent_region_get_silhouette(dwg_ent_region *region, int *error)
{
  dwg_ent_solid_silhouette *ptx = (dwg_ent_solid_silhouette*)
  malloc(sizeof(dwg_ent_solid_silhouette)* region->num_silhouettes);
    if(ptx != 0)
      {
        *error = 0;
        int i = 0;
          for (i = 0; i < region->num_silhouettes ; i++)
            {
              ptx[i] = region->silhouettes[i];
            }
        return ptx;
      }
    else
      *error = 1;
}

unsigned char
dwg_ent_region_get_acis_empty2(dwg_ent_region *region, int *error)
{
  if (region != 0)
    {
      *error = 0;
      return region->acis_empty2;
    }
  else
    *error = 1;
}

void
dwg_ent_region_set_acis_empty2(dwg_ent_region *region, unsigned char acis,
                               int *error)
{
  if (region != 0)
    {
      *error = 0;
      region->acis_empty2 = acis;
    }
  else
    *error = 1;
} 

/*******************************************************************
*                    FUNCTIONS FOR BODY ENTITY                      *
********************************************************************/

unsigned char
dwg_ent_body_get_acis_empty(dwg_ent_body *body, int *error)
{
  if (body != 0)
    {
      *error = 0;
      return body->acis_empty;
    }
  else
    *error = 1;
}

void
dwg_ent_body_set_acis_empty(dwg_ent_body *body, unsigned char acis,
                            int *error)
{
  if (body != 0)
    {
      *error = 0;
      body->acis_empty = acis;
    }
  else
    *error = 1;
}

unsigned int
dwg_ent_body_get_version(dwg_ent_body *body, int *error)
{
  if (body != 0)
    {
      *error = 0;
      return body->version;
    }
  else
    *error = 1;
}

void
dwg_ent_body_set_version(dwg_ent_body *body, unsigned int version, int *error)
{
  if (body != 0)
    {
      *error = 0;
      body->version = version;
    }
  else
    *error = 1;
}

long * 
dwg_ent_body_get_block_size(dwg_ent_body *body, int *error)
{
  if (body != 0)
    {
      *error = 0;
      return body->block_size;
    }
  else
    *error = 1;
}

char *dwg_ent_body_get_acis_data(dwg_ent_body *body, int *error)
{
  if (body != 0)
    {
      *error = 0;
      return body->acis_data;
    }
  else
    *error = 1;
}

char *
dwg_ent_body_set_acis_data(dwg_ent_body *body, char * data, int *error)
{
  if (body != 0)
    {
      *error = 0;
      body->acis_data = data;
    }
  else
    *error = 1;
}

char
dwg_ent_body_get_wireframe_data_present(dwg_ent_body *body, int *error)
{
  if (body != 0)
    {
      *error = 0;
      return body->wireframe_data_present;
    }
  else
    *error = 1;
}

void
dwg_ent_body_set_wireframe_data_present(dwg_ent_body *body, char data,
                                        int *error)
{
  if (body != 0)
    {
      *error = 0;
      body->wireframe_data_present = data;
    }
  else
    *error = 1;
}

char
dwg_ent_body_get_point_present(dwg_ent_body *body, int *error)
{
  if (body != 0)
    {
      *error = 0;
      return body->point_present;
    }
  else
    *error = 1;
}

void
dwg_ent_body_set_point_present(dwg_ent_body *body, char point, int *error)
{
  if (body != 0)
    {
      *error = 0;
      body->point_present = point;
    }
  else
    *error = 1;
}

void
dwg_ent_body_get_point(dwg_ent_body *body, dwg_point_3d *point, int *error)
{
  if (body != 0)
    {
      *error = 0;
      point->x = body->point.x;
      point->y = body->point.y;
      point->z = body->point.z;
    }
  else
    *error = 1;
}

void
dwg_ent_body_set_point(dwg_ent_body *body, dwg_point_3d *point, int *error)
{
  if (body != 0)
    {
      *error = 0;
      body->point.x = point->x;
      body->point.y = point->y;
      body->point.z = point->z;
    }
  else
    *error = 1;
}

long
dwg_ent_body_get_num_isolines(dwg_ent_body *body, int *error)
{
  if (body != 0)
    {
      *error = 0;
      return body->num_isolines;
    }
  else
    *error = 1;
}

void
dwg_ent_body_set_num_isolines(dwg_ent_body *body, long num, int *error)
{
  if (body != 0)
    {
      *error = 0;
      body->num_isolines = num;
    }
  else
    *error = 1;
}

char
dwg_ent_body_get_isoline_present(dwg_ent_body *body, int *error)
{
  if (body != 0)
    {
      *error = 0;
      return body->isoline_present;
    }
  else
    *error = 1;
}

void
dwg_ent_body_set_isoline_present(dwg_ent_body *body, char iso, int *error)
{
  if (body != 0)
    {
      *error = 0;
      body->isoline_present = iso;
    }
  else
    *error = 1;
}

long
dwg_ent_body_get_num_wires(dwg_ent_body *body, int *error)
{
  if (body != 0)
    {
      *error = 0;
      return body->num_wires;
    }
  else
    *error = 1;
}

void
dwg_ent_body_set_num_wires(dwg_ent_body *body, long num, int *error)
{
  if (body != 0)
    {
      *error = 0;
      body->num_wires = num;
    }
  else
    *error = 1;
}

dwg_ent_solid_wire *
dwg_ent_body_get_wire(dwg_ent_body *body, int *error)
{
  dwg_ent_solid_wire *ptx = (dwg_ent_solid_wire*)
  malloc(sizeof(dwg_ent_solid_wire)* body->num_wires);
    if(ptx != 0)
      {
        *error = 0;
        int i = 0;
          for (i = 0; i < body->num_wires ; i++)
            {
              ptx[i] = body->wires[i];
            }
        return ptx;
      }
    else
      *error = 1;
}

long
dwg_ent_body_get_num_silhouettes(dwg_ent_body *body, int *error)
{
  if (body != 0)
    {
      *error = 0;
      return body->num_silhouettes;
    }
  else
    *error = 1;
}

void
dwg_ent_body_set_num_silhouettes(dwg_ent_body *body, long silhouettes,
                                 int *error)
{
  if (body != 0)
    {
      *error = 0;
      body->num_silhouettes = silhouettes;
    }
  else
    *error = 1;
}

dwg_ent_solid_silhouette *
dwg_ent_body_get_silhouette(dwg_ent_body *body,
                            int *error)
{
  dwg_ent_solid_silhouette *ptx = (dwg_ent_solid_silhouette*) 
  malloc(sizeof(dwg_ent_solid_silhouette)* body->num_silhouettes);
    if(ptx != 0)
      {
        *error = 0;
        int i = 0;
          for (i = 0; i < body->num_silhouettes ; i++)
            {
              ptx[i] = body->silhouettes[i];
            }
        return ptx;
      }
    else
      *error = 1;
}

unsigned char
dwg_ent_body_get_acis_empty2(dwg_ent_body *body, int *error)
{
  if (body != 0)
    {
      *error = 0;
      return body->acis_empty2;
    }
  else
    *error = 1;
}

void
dwg_ent_body_set_acis_empty2(dwg_ent_body *body, unsigned char acis,
                             int *error)
{
  if (body != 0)
    {
      *error = 0;
      body->acis_empty2 = acis;
    }
  else
    *error = 1;
} 

/*******************************************************************
*                    FUNCTIONS FOR TABLE ENTITY                     *
********************************************************************/

void
dwg_ent_table_set_insertion_point(dwg_ent_table *table, dwg_point_3d *point,
                                  int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->insertion_point.x = point->x;
      table->insertion_point.y = point->y;
      table->insertion_point.z = point->z;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_get_insertion_point(dwg_ent_table *table, dwg_point_3d *point,
                                  int *error)
{
  if (table != 0)
    {
      *error = 0;
      point->x = table->insertion_point.x;
      point->y = table->insertion_point.y;
      point->z = table->insertion_point.z;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_scale(dwg_ent_table *table, dwg_point_3d *point, int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->scale.x = point->x;
      table->scale.y = point->y;
      table->scale.z = point->z;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_get_scale(dwg_ent_table *table, dwg_point_3d *point, int *error)
{
  if (table != 0)
    {
      *error = 0;
      point->x = table->scale.x;
      point->y = table->scale.y;
      point->z = table->scale.z;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_data_flags(dwg_ent_table *table, unsigned char flags,
                             int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->data_flags = flags;
    }
  else
    *error = 1;   
}

unsigned char
dwg_ent_table_get_data_flags(dwg_ent_table *table, int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->data_flags;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_rotation(dwg_ent_table *table, double rotation, int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->rotation = rotation;
    }
  else
    *error = 1;   
}

double
dwg_ent_table_get_rotation(dwg_ent_table *table, int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->rotation;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_extrusion(dwg_ent_table *table, dwg_point_3d *point,
                            int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->extrusion.x = point->x;
      table->extrusion.y = point->y;
      table->extrusion.z = point->z;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_get_extrusion(dwg_ent_table *table, dwg_point_3d *point,
                            int *error)
{
  if (table != 0)
    {
      *error = 0;
      point->x = table->extrusion.x;
      point->y = table->extrusion.y;
      point->z = table->extrusion.z;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_has_attribs(dwg_ent_table *table, unsigned char attribs,
                              int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->has_attribs = attribs;
    }
  else
    *error = 1;   
}

unsigned char
dwg_ent_table_get_has_attribs(dwg_ent_table *table, int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->has_attribs;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_owned_object_count(dwg_ent_table *table, long count,
                                     int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->owned_object_count = count;
    }
  else
    *error = 1;   
}

long
dwg_ent_table_get_owned_object_count(dwg_ent_table *table, int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->owned_object_count;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_flag_for_table_value(dwg_ent_table *table,
                                       unsigned int value, int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->flag_for_table_value = value;
    }
  else
    *error = 1;   
}

unsigned int 
dwg_ent_table_get_flag_for_table_value(dwg_ent_table *table, int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->flag_for_table_value;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_horiz_direction(dwg_ent_table *table, dwg_point_3d *point,
                                  int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->horiz_direction.x = point->x;
      table->horiz_direction.y = point->y;
      table->horiz_direction.z = point->z;

    }
  else
    *error = 1;   
}

void
dwg_ent_table_get_horiz_direction(dwg_ent_table *table, dwg_point_3d *point,
                                  int *error)
{
  if (table != 0)
    {
      *error = 0;
      point->x = table->horiz_direction.x;
      point->y = table->horiz_direction.y;
      point->z = table->horiz_direction.z;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_num_cols(dwg_ent_table *table, long num, int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->num_cols = num;
    }
  else
    *error = 1;   
}

long
dwg_ent_table_get_num_cols(dwg_ent_table *table, int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->num_cols;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_num_rows(dwg_ent_table *table, long num, int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->num_rows = num;
    }
  else
    *error = 1;   
}

long
dwg_ent_table_get_num_rows(dwg_ent_table *table, int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->num_rows;
    }
  else
    *error = 1;   
}

double *
dwg_ent_table_get_col_widths(dwg_ent_table *table, int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->col_widths;
    }
  else
    *error = 1;   
}

double *
dwg_ent_table_get_row_heights(dwg_ent_table *table, int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->row_heights;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_table_overrides_present(dwg_ent_table *table,
                                          unsigned char present, int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->table_overrides_present = present;
    }
  else
    *error = 1;   
}

unsigned char
dwg_ent_table_get_table_overrides_present(dwg_ent_table *table, int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->table_overrides_present;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_table_flag_override(dwg_ent_table *table, long override,
                                      int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->table_flag_override = override;
    }
  else
    *error = 1;   
}

long
dwg_ent_table_get_table_flag_override(dwg_ent_table *table, int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->table_flag_override;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_title_suppressed(dwg_ent_table *table, unsigned char title,
                                   int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->title_suppressed = title;
    }
  else
    *error = 1;   
}

unsigned char
dwg_ent_table_get_title_suppressed(dwg_ent_table *table, int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->title_suppressed;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_header_suppressed(dwg_ent_table *table, unsigned char header,
                                    int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->header_suppressed = header;
    }
  else
    *error = 1;   
}

unsigned char
dwg_ent_table_get_header_suppressed(dwg_ent_table *table, int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->header_suppressed;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_flow_direction(dwg_ent_table *table, unsigned int dir,
                                 int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->flow_direction = dir;
    }
  else
    *error = 1;   
}

unsigned int 
dwg_ent_table_get_flow_direction(dwg_ent_table *table, int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->flow_direction;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_horiz_cell_margin(dwg_ent_table *table, double margin,
                                    int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->horiz_cell_margin = margin;
    }
  else
    *error = 1;   
}

double
dwg_ent_table_get_horiz_cell_margin(dwg_ent_table *table, int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->horiz_cell_margin;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_vert_cell_margin(dwg_ent_table *table, double margin,
                                   int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->vert_cell_margin = margin;
    }
  else
    *error = 1;   
}

double
dwg_ent_table_get_vert_cell_margin(dwg_ent_table *table, int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->vert_cell_margin;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_title_row_fill_none(dwg_ent_table *table, unsigned char fill,
                                      int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->title_row_fill_none = fill;
    }
  else
    *error = 1;   
}

unsigned char
dwg_ent_table_get_title_row_fill_none(dwg_ent_table *table, int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->title_row_fill_none;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_header_row_fill_none(dwg_ent_table *table,
                                       unsigned char fill, int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->header_row_fill_none = fill;
    }
  else
    *error = 1;   
}

unsigned char
dwg_ent_table_get_header_row_fill_none(dwg_ent_table *table, int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->header_row_fill_none;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_data_row_fill_none(dwg_ent_table *table, unsigned char fill,
                                     int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->data_row_fill_none = fill;
    }
  else
    *error = 1;   
}

unsigned char
dwg_ent_table_get_data_row_fill_none(dwg_ent_table *table, int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->data_row_fill_none;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_title_row_align(dwg_ent_table *table, unsigned char fill,
                                  int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->title_row_align = fill;
    }
  else
    *error = 1;   
}

unsigned int 
dwg_ent_table_get_title_row_align(dwg_ent_table *table, int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->title_row_align;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_header_row_align(dwg_ent_table *table, unsigned int align,
                                   int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->header_row_align = align;
    }
  else
    *error = 1;   
}

unsigned int 
dwg_ent_table_get_header_row_align(dwg_ent_table *table, int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->header_row_align;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_data_row_align(dwg_ent_table *table, unsigned int align,
                                 int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->data_row_align = align;
    }
  else
    *error = 1;   
}

unsigned int 
dwg_ent_table_get_data_row_align(dwg_ent_table *table, int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->data_row_align;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_title_row_height(dwg_ent_table *table, double height,
                                   int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->title_row_height = height;
    }
  else
    *error = 1;   
}

double
dwg_ent_table_get_title_row_height(dwg_ent_table *table, int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->title_row_height;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_header_row_height(dwg_ent_table *table, double height,
                                    int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->header_row_height = height;
    }
  else
    *error = 1;   
}

double
dwg_ent_table_get_header_row_height(dwg_ent_table *table, int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->header_row_height;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_data_row_height(dwg_ent_table *table, double height,
                                  int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->data_row_height = height;
    }
  else
    *error = 1;   
}

double
dwg_ent_table_get_data_row_height(dwg_ent_table *table, int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->data_row_height;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_border_color_overrides_present(dwg_ent_table *table,
                                                 unsigned char present,
                                                 int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->border_color_overrides_present = present;
    }
  else
    *error = 1;   
}

unsigned char
dwg_ent_table_get_border_color_overrides_present(dwg_ent_table *table,
                                                 int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->border_color_overrides_present;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_border_color_overrides_flag(dwg_ent_table *table,
                                              long overrides, int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->border_color_overrides_flag = overrides;
    }
  else
    *error = 1;   
}

long
dwg_ent_table_get_border_color_overrides_flag(dwg_ent_table *table, int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->border_color_overrides_flag;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_border_lineweight_overrides_present(dwg_ent_table *table,
                                                      unsigned char present,
                                                      int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->border_lineweight_overrides_present = present;
    }
  else
    *error = 1;   
}

unsigned char
dwg_ent_table_get_border_lineweight_overrides_present(dwg_ent_table *table,
                                                      int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->border_lineweight_overrides_present;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_border_lineweight_overrides_flag(dwg_ent_table *table,
                                                   long overrides, int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->border_lineweight_overrides_flag = overrides;
    }
  else
    *error = 1;   
}

long
dwg_ent_table_get_border_lineweight_overrides_flag(dwg_ent_table *table,
                                                   int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->border_lineweight_overrides_flag;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_title_horiz_top_lineweight(dwg_ent_table *table,
                                             unsigned int lw, int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->title_horiz_top_lineweigh = lw;
    }
  else
    *error = 1;   
}

unsigned int 
dwg_ent_table_get_title_horiz_top_lineweight(dwg_ent_table *table, int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->title_horiz_top_lineweigh;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_title_horiz_ins_lineweight(dwg_ent_table *table,
                                             unsigned int lw, int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->title_horiz_ins_lineweigh = lw;
    }
  else
    *error = 1;   
}

unsigned int 
dwg_ent_table_get_title_horiz_ins_lineweight(dwg_ent_table *table, int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->title_horiz_ins_lineweigh;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_title_horiz_bottom_lineweight(dwg_ent_table *table,
                                                unsigned int lw, int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->title_horiz_bottom_lineweigh = lw;
    }
  else
    *error = 1;   
}

unsigned int 
dwg_ent_table_get_title_horiz_bottom_lineweight(dwg_ent_table *table,
                                                int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->title_horiz_bottom_lineweigh;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_title_vert_left_lineweight(dwg_ent_table *table,
                                             unsigned int lw, int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->title_vert_left_lineweigh = lw;
    }
  else
    *error = 1;   
}

unsigned int 
dwg_ent_table_get_title_vert_left_lineweight(dwg_ent_table *table, int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->title_vert_left_lineweigh;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_title_vert_ins_lineweight(dwg_ent_table *table,
                                            unsigned int lw, int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->title_vert_ins_lineweigh = lw;
    }
  else
    *error = 1;   
}

unsigned int 
dwg_ent_table_get_title_vert_ins_lineweight(dwg_ent_table *table, int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->title_vert_ins_lineweigh;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_title_vert_right_lineweight(dwg_ent_table *table,
                                              unsigned int lw, int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->title_vert_right_lineweigh = lw;
    }
  else
    *error = 1;
}

unsigned int 
dwg_ent_table_get_title_vert_right_lineweight(dwg_ent_table *table, int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->title_vert_right_lineweigh;
    }
  else
    *error = 1;
}

void
dwg_ent_table_set_header_horiz_top_lineweight(dwg_ent_table *table,
                                              unsigned int lw, int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->header_horiz_top_lineweigh = lw;
    }
  else
    *error = 1;   
}

unsigned int 
dwg_ent_table_get_header_horiz_top_lineweight(dwg_ent_table *table, int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->header_horiz_top_lineweigh;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_header_horiz_ins_lineweight(dwg_ent_table *table,
                                              unsigned int lw, int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->header_horiz_ins_lineweigh = lw;
    }
  else
    *error = 1;   
}

unsigned int 
dwg_ent_table_get_header_horiz_ins_lineweight(dwg_ent_table *table,
                                              int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->header_horiz_ins_lineweigh;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_header_horiz_bottom_lineweight(dwg_ent_table *table,
                                                 unsigned int lw, int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->header_horiz_bottom_lineweigh = lw;
    }
  else
    *error = 1;   
}

unsigned int 
dwg_ent_table_get_header_horiz_bottom_lineweight(dwg_ent_table *table,
                                                 int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->header_horiz_bottom_lineweigh;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_header_vert_left_lineweight(dwg_ent_table *table,
                                              unsigned int lw, int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->header_vert_left_lineweigh = lw;
    }
  else
    *error = 1;   
}

unsigned int 
dwg_ent_table_get_header_vert_left_lineweight(dwg_ent_table *table, int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->header_vert_left_lineweigh;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_header_vert_ins_lineweight(dwg_ent_table *table,
                                             unsigned int lw, int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->header_vert_ins_lineweigh = lw;
    }
  else
    *error = 1;   
}

unsigned int 
dwg_ent_table_get_header_vert_ins_lineweight(dwg_ent_table *table, int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->header_vert_ins_lineweigh;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_header_vert_right_lineweight(dwg_ent_table *table,
                                               unsigned int lw, int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->header_vert_right_lineweigh = lw;
    }
  else
    *error = 1;   
}

unsigned int 
dwg_ent_table_get_header_vert_right_lineweight(dwg_ent_table *table,
                                               int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->header_vert_right_lineweigh;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_data_horiz_top_lineweight(dwg_ent_table *table,
                                            unsigned int lw, int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->data_horiz_top_lineweigh = lw;
    }
  else
    *error = 1;   
}

unsigned int 
dwg_ent_table_get_data_horiz_top_lineweight(dwg_ent_table *table, int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->data_horiz_top_lineweigh;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_data_horiz_ins_lineweight(dwg_ent_table *table,
                                            unsigned int lw, int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->data_horiz_ins_lineweigh = lw;
    }
  else
    *error = 1;   
}

unsigned int 
dwg_ent_table_get_data_horiz_ins_lineweight(dwg_ent_table *table, int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->data_horiz_ins_lineweigh;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_data_horiz_bottom_lineweight(dwg_ent_table *table,
                                               unsigned int lw, int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->data_horiz_bottom_lineweigh = lw;
    }
  else
    *error = 1;   
}

unsigned int 
dwg_ent_table_get_data_horiz_bottom_lineweight(dwg_ent_table *table,
                                               int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->data_horiz_bottom_lineweigh;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_data_vert_ins_lineweight(dwg_ent_table *table,
                                           unsigned int lw, int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->data_horiz_ins_lineweigh = lw;
    }
  else
    *error = 1;   
}

unsigned int 
dwg_ent_table_get_data_vert_ins_lineweight(dwg_ent_table *table, int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->data_horiz_ins_lineweigh;
    }
  else
    *error = 1;   
}



void
dwg_ent_table_set_border_visibility_overrides_present(dwg_ent_table *table,
                                                      unsigned char overrides,
                                                      int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->border_visibility_overrides_present = overrides;
    }
  else
    *error = 1;   
}

unsigned char
dwg_ent_table_get_border_visibility_overrides_present(dwg_ent_table *table,
                                                      int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->border_visibility_overrides_present;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_border_visibility_overrides_flag(dwg_ent_table *table,
                                                   long overrides, int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->border_visibility_overrides_flag = overrides;
    }
  else
    *error = 1;   
}

long
dwg_ent_table_get_border_visibility_overrides_flag(dwg_ent_table *table,
                                                   int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->border_visibility_overrides_flag;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_title_horiz_top_visibility(dwg_ent_table *table,
                                             unsigned int visibility,
                                             int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->title_horiz_top_visibility = visibility;
    }
  else
    *error = 1;   
}

unsigned int 
dwg_ent_table_get_title_horiz_top_visibility(dwg_ent_table *table, int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->title_horiz_top_visibility;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_title_horiz_ins_visibility(dwg_ent_table *table,
                                             unsigned int visibility,
                                             int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->title_horiz_ins_visibility = visibility;
    }
  else
    *error = 1;   
}

unsigned int 
dwg_ent_table_get_title_horiz_ins_visibility(dwg_ent_table *table, int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->title_horiz_ins_visibility;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_title_horiz_bottom_visibility(dwg_ent_table *table,
                                                unsigned int visibility,
                                                int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->title_horiz_bottom_visibility = visibility;
    }
  else
    *error = 1;   
}

unsigned int 
dwg_ent_table_get_title_horiz_bottom_visibility(dwg_ent_table *table,
                                                int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->title_horiz_bottom_visibility;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_title_vert_left_visibility(dwg_ent_table *table,
                                             unsigned int visibility,
                                             int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->title_vert_left_visibility = visibility;
    }
  else
    *error = 1;   
}

unsigned int 
dwg_ent_table_get_title_vert_left_visibility(dwg_ent_table *table, int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->title_vert_left_visibility;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_title_vert_ins_visibility(dwg_ent_table *table,
                                            unsigned int visibility,
                                            int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->title_vert_ins_visibility = visibility;
    }
  else
    *error = 1;   
}

unsigned int 
dwg_ent_table_get_title_vert_ins_visibility(dwg_ent_table *table, int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->title_vert_ins_visibility;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_title_vert_right_visibility(dwg_ent_table *table,
                                              unsigned int visibility,
                                              int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->title_vert_right_visibility = visibility;
    }
  else
    *error = 1;   
}

unsigned int 
dwg_ent_table_get_title_vert_right_visibility(dwg_ent_table *table,
                                              int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->title_vert_right_visibility;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_header_horiz_top_visibility(dwg_ent_table *table,
                                              unsigned int visibility,
                                              int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->header_horiz_top_visibility = visibility;
    }
  else
    *error = 1;   
}

unsigned int 
dwg_ent_table_get_header_horiz_top_visibility(dwg_ent_table *table,
                                              int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->header_horiz_top_visibility;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_header_horiz_ins_visibility(dwg_ent_table *table,
                                              unsigned int visibility,
                                              int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->header_horiz_ins_visibility = visibility;
    }
  else
    *error = 1;   
}

unsigned int 
dwg_ent_table_get_header_horiz_ins_visibility(dwg_ent_table *table,
                                              int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->header_horiz_ins_visibility;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_header_horiz_bottom_visibility(dwg_ent_table *table,
                                                 unsigned int visibility,
                                                 int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->header_horiz_bottom_visibility = visibility;
    }
  else
    *error = 1;   
}

unsigned int 
dwg_ent_table_get_header_vert_left_visibility(dwg_ent_table *table,
                                              int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->header_vert_left_visibility;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_header_vert_ins_visibility(dwg_ent_table *table,
                                             unsigned int visibility,
                                             int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->header_vert_ins_visibility = visibility;
    }
  else
    *error = 1;   
}

unsigned int 
dwg_ent_table_get_header_vert_ins_visibility(dwg_ent_table *table, int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->header_vert_ins_visibility;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_header_vert_right_visibility(dwg_ent_table *table,
                                               unsigned int visibility,
                                               int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->header_vert_right_visibility = visibility;
    }
  else
    *error = 1;   
}

unsigned int 
dwg_ent_table_get_header_vert_right_visibility(dwg_ent_table *table,
                                               int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->header_vert_right_visibility;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_data_horiz_top_visibility(dwg_ent_table *table,
                                            unsigned int visibility,
                                            int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->data_horiz_top_visibility = visibility;
    }
  else
    *error = 1;   
}

unsigned int 
dwg_ent_table_get_data_horiz_top_visibility(dwg_ent_table *table, int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->data_horiz_top_visibility;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_data_horiz_ins_visibility(dwg_ent_table *table,
                                            unsigned int visibility,
                                            int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->data_horiz_ins_visibility = visibility;
    }
  else
    *error = 1;   
}

unsigned int 
dwg_ent_table_get_data_horiz_ins_visibility(dwg_ent_table *table, int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->data_horiz_ins_visibility;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_data_horiz_bottom_visibility(dwg_ent_table *table,
                                               unsigned int visibility,
                                               int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->data_horiz_bottom_visibility = visibility;
    }
  else
    *error = 1;   
}

unsigned int 
dwg_ent_table_get_data_horiz_bottom_visibility(dwg_ent_table *table,
                                               int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->data_horiz_bottom_visibility;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_data_vert_ins_visibility(dwg_ent_table *table,
                                           unsigned int visibility,
                                           int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->data_horiz_ins_visibility = visibility;
    }
  else
    *error = 1;   
}

unsigned int 
dwg_ent_table_get_data_vert_ins_visibility(dwg_ent_table *table, int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->data_horiz_ins_visibility;
    }
  else
    *error = 1;   
}

void
dwg_ent_table_set_data_vert_right_visibility(dwg_ent_table *table,
                                             unsigned int visibility,
                                             int *error)
{
  if (table != 0)
    {
      *error = 0;
      table->data_vert_right_visibility = visibility;
    }
  else
    *error = 1;   
}

unsigned int 
dwg_ent_table_get_data_vert_right_visibility(dwg_ent_table *table, int *error)
{
  if (table != 0)
    {
      *error = 0;
      return table->data_vert_right_visibility;
    }
  else
    *error = 1;   
}

/*******************************************************************
*               FUNCTIONS FOR BLOCK_CONTROL OBJECT                  *
********************************************************************/

/// Returns block control object from a block header type argument passed to function
/** Usage :- 
dwg_obj_block_control *blc;
blc = dwg_block_header_get_block_control(hdr);
*/
dwg_obj_block_control *
dwg_block_header_get_block_control(dwg_obj_block_header* block_header,
                                   int *error)
{
  if (block_header != 0)
    {
      *error = 0;
      return block_header->block_control_handle->obj->tio.object->tio.BLOCK_CONTROL;
    }
  else
    *error = 1;

}

dwg_object_ref **
dwg_obj_block_control_get_block_headers(dwg_obj_block_control *ctrl,
                                        int *error)
{
  dwg_object_ref **ptx = (dwg_object_ref**) 
  malloc(ctrl->num_entries * sizeof(Dwg_Object_Ref *));
    if(ptx != 0)
      {
        *error = 0;
        int i = 0;
          for (i=0; i<ctrl->num_entries; i++)
            {
              ptx[i] = ctrl->block_headers[i];
            }
        return ptx;
      }
    else
      *error = 1;
}

long
dwg_obj_block_control_get_num_entries(dwg_obj_block_control *ctrl, int *error)
{
  if(ctrl != 0)
    {
      *error = 0;
      return ctrl->num_entries;
    }
  else
    *error = 1;
}

dwg_object_ref *
dwg_obj_block_control_get_model_space(dwg_obj_block_control *ctrl, int *error)
{
  if(ctrl != 0)
    {
      *error = 0;
      return ctrl->model_space;
    }
  else
    *error = 1;
}

dwg_object_ref *
dwg_obj_block_control_get_paper_space(dwg_obj_block_control *ctrl, int *error)
{
  if(ctrl != 0)
    {
      *error = 0;
      return ctrl->paper_space;
    }
  else
    *error = 1;
}

/*******************************************************************
*                    FUNCTIONS FOR LAYER OBJECT                     *
********************************************************************/

/// Get layer Name of the layer type argument passed in function
/** Usage :- char * layer_name = dwg_obj_layer_get_name(layer);
*/
char *
dwg_obj_layer_get_name(dwg_obj_layer *layer, int *error)
{
  if (layer != 0)
    {
      *error = 0;
      return layer->entry_name;
    }
  else
    *error = 1;

}

/*******************************************************************
*                FUNCTIONS FOR BLOCK_HEADER OBJECT                  *
********************************************************************/

/// Get Block Name of the block header type argument passed in function
/** Usage :- char * block_name = dwg_obj_block_header_get_name(hdr);
*/
char *
dwg_obj_block_header_get_name(dwg_obj_block_header *hdr, int *error)
{
  if (hdr != 0)
    {
      *error = 0;
      return hdr->entry_name;
    }
  else
    *error = 1;
}

/// Returns 1st block header present in the dwg file from the dwg_data type argument passed to function.
/** Usage :- dwg_get_block_header(dwg);
*/
dwg_obj_block_header *
dwg_get_block_header(dwg_data *dwg, int *error)
{    
  Dwg_Object *obj;
  Dwg_Object_BLOCK_HEADER *blk;
  obj = &dwg->object[0];
    while(obj->type != DWG_TYPE_BLOCK_HEADER)
      {
        obj = dwg_next_object(obj);
      }
    if (DWG_TYPE_BLOCK_HEADER == obj->type )
      {
        blk = obj->tio.object->tio.BLOCK_HEADER;
      }
  return blk;
}

/*******************************************************************
*                    FUNCTIONS FOR DWG OBJECT                       *
********************************************************************/

int
dwg_obj_object_get_index(dwg_object *obj, int *error)
{
  if(obj != 0)
    {
      *error = 0;
      return obj->index;
    }
  else
    *error = 1;
}

dwg_handle
dwg_obj_get_handle(dwg_object *obj, int *error)
{
  if(obj != 0)
    {
      *error = 0;
      return obj->handle;
    }
  else
    *error = 1;
}


dwg_obj_obj *
dwg_object_to_object(dwg_object *obj, int *error)
{

  if(obj != 0)
    {
      *error = 0;
      return obj->tio.object;
    }
  else
    *error = 1;    
}

dwg_obj_ent *
dwg_object_to_entity(dwg_object *obj, int *error)
{
  return obj->tio.entity;
  if(obj != 0)
    {
      *error = 0;
      return obj->tio.entity;
    }
  else
    *error = 1;    
}

dwg_object *
dwg_obj_reference_get_object(dwg_object_ref *ref, int *error)
{
  if(ref != 0)
    {
      *error = 0;
      return ref->obj;
    }
  else
    *error = 1;
}

unsigned long
dwg_obj_ref_get_abs_ref(dwg_object_ref *ref, int *error)
{
  if(ref != 0)
    {
      *error = 0;
      return ref->absolute_ref;
    }
  else
    *error = 1;    
}

int 
dwg_get_type(dwg_object *obj)
{
  return obj->type;
}
