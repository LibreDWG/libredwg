#include "api.h"
#include <dwg.h>
#include <malloc.h>

//---------------------------------------------------------------------------

/* This function creates and returns a new circle entity with default 
values.
Usage : dwg_ent_circle_new();
*/
dwg_ent_circle *
dwg_ent_circle_new(int *error)
{
    dwg_ent_circle *circle = (dwg_ent_circle*) malloc(sizeof(Dwg_Entity_CIRCLE));
    if(circle != 0)
        {
            *error = 0;
            return dwg_ent_circle_init(circle);        
        }
    else
        {
            *error = 1;
        }
    
}

/* This function frees the memory allocated by the dwg_ent_circle_new()
function.
Usage :- dwg_ent_circle_delete();
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
        {
            *error = 1;
        }
}


/* This function initialises a new circle entity with default values.
Usage :- dwg_ent_circle_init(circle);
where circle is a pointer of dwg_ent_circle type
*/
dwg_ent_circle *
dwg_ent_circle_init(dwg_ent_circle *circle)
{
    if (circle != 0)
    {
        circle->center.x    = circle->center.y = circle->center.z = 0.0;
        circle->radius      = 0.0;
        circle->thickness   = 0.0;
        circle->extrusion.x = circle->extrusion.y = circle->extrusion.z = 0.0;
    }

    return circle;
}

/* Returns the center values of circle to second argument.
Usage :- dwg_ent_circle_get_center(circle, point);
where circle is a pointer of dwg_ent_circle type and point is 
of dwg_point_3d
*/
void
dwg_ent_circle_get_center(dwg_ent_circle *circle, dwg_point_3d *point, int *error)
{
    if(point != 0 && circle != 0)
        {
            *error = 0;
            point->x = circle->center.x;
            point->y = circle->center.y;
            point->z = circle->center.z;
        }
    else
        {
            *error = 1;
        }
}

/* Sets the circle center equal to values from second argument.
Usage :- dwg_ent_circle_set_center(circle, point);
where circle is a pointer of dwg_ent_circle type and point is 
of dwg_point_3d
*/
void
dwg_ent_circle_set_center(dwg_ent_circle *circle, dwg_point_3d *point, int *error)
{
    if(point != 0 && circle != 0)
        {
            *error = 0;
            circle->center.x = point->x;
            circle->center.y = point->y;
            circle->center.z = point->z;
        }
    else
        {
            *error = 1;
        }
}

/* Returns radius of circle entity passed in argument.
Usage :- 
double circle_radius;
circle_radius = dwg_ent_circle_get_radius(circle); 
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
        {
            *error = 1;
        }
}

/* Sets radius of a circle entity passed in 
first argument equal to (radius) value in second argument.
dwg_ent_circle_set_radius(circle, 50.0);
so circle radius becomes 50.0 
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
        {
            *error = 1;
        }
}

/* Returns thickness of entity passed in argument.
Usage :- 
double thickness;
thickness = dwg_ent_circle_get_thickness(circle);
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
        {
            *error = 1;
        }
}

/* Sets thickness of a circle entity passed in 
first argument equal to (thickness) value in second argument.
Usage :- dwg_ent_circle_set_thickness(circle, 50.0);
circle thickness becomes 50.0 
*/
void
dwg_ent_circle_set_thickness(dwg_ent_circle *circle, double thickness, int *error)
{
    if(circle != 0)
        {
            *error = 0;
            circle->thickness = thickness;
        }
    else
        {
            *error = 1;
        }
}

/* This function sets the extrusion values of a circle entity equal to 
values in second argument.
extrusion_value.x = 10.0;
extrusion_value.y = 0.0;
extrusion_value.z = 0.0;
Usage :- dwg_ent_circle_set_extrusion(circle, extrusion_value);
where circle is an entity of circle type, and extrusion value contains
the x, y and z values
hence circle->extrusion.x = 10.0
*/
void
dwg_ent_circle_set_extrusion(dwg_ent_circle *circle, dwg_point_3d *vector, int *error)
{
    if(circle != 0)
        {
            *error = 0;
            circle->extrusion.x = vector->x;
            circle->extrusion.y = vector->y;
            circle->extrusion.z = vector->z;
        }
    else
        {
            *error = 1;
        }
}

/* This function returns the extrusion values of a circle entity 
into second argument.
Usage :- dwg_ent_circle_get_extrusion(circle, extrusion_value);
where circle is an entity of circle type, and extrusion value contains
the x, y and z values
if circle->extrusion.x = 50.0 after calling function 
extrusion_value.x = 50.0
*/
void
dwg_ent_circle_get_extrusion(dwg_ent_circle *circle, dwg_point_3d *vector, int *error)
{
    if(circle != 0)
        {
            *error = 0;
            vector->x = circle->extrusion.x;
            vector->y = circle->extrusion.y;
            vector->z = circle->extrusion.z;
        }
    else
        {
            *error = 1;
        }
}

//---------------------------------------------------------------------------

/* This function creates a new entity of line.
Usage :- dwg_ent_line_new();
*/
dwg_ent_line *
dwg_ent_line_new(int *error)
{
    dwg_ent_line *line = (dwg_ent_line*) malloc(sizeof(Dwg_Entity_LINE));
    if(line != 0)
        {
            *error = 0;
            return dwg_ent_line_init(line);
        }
    else
        {
            *error = 1;
        }
}

/* This function deletes the entity created using dwg_ent_line_new() 
function and frees the allocated memory.
Usage :- dwg_ent_line_delete();
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
        {
            *error = 1;
        }
}

/* This function initialises a line entity with default or zero values.
Usage :- dwg_ent_line_init();
*/
dwg_ent_line *
dwg_ent_line_init(dwg_ent_line *line)
{
    if (line != 0)
    {
        line->start.x     = line->start.y = line->start.z = 0.0;
        line->end.x       = line->end.y = line->end.z = 0.0;
        line->thickness   = 0.0;
        line->extrusion.x = line->extrusion.y = line->extrusion.z = 0.0;
    }
    return line;
}

/* This function returns the line start points to the second argument.
Usage :- dwg_ent_line_get_start_point(line, point);
line is entity of line type and point is a dwg_point_3d datatype.
after calling this point.x will have start x value.
*/
void
dwg_ent_line_get_start_point(dwg_ent_line *line, dwg_point_3d *point, int *error)
{
    if (line != 0)
        {
            *error = 0;
            point->x = line->start.x;
            point->y = line->start.y;
            point->z = line->start.z;
        }
    else
        {
            *error = 1;
        }
}

/* This function sets the line start points equal to the 
point values in second argument.
Usage :- dwg_ent_line_set_start_point(line, point);
line is entity of line type and point is a dwg_point_3d datatype.
*/
void
dwg_ent_line_set_start_point(dwg_ent_line *line, dwg_point_3d *point, int *error)
{
    if (line != 0)
        {
            *error = 0;
            line->start.x = point->x;
            line->start.y = point->y;
            line->start.z = point->z;
        }
    else
        {
            *error = 1;
        }
}

/* This function returns the line end points to the second argument.
Usage :- dwg_ent_line_get_end_point(line, point);
line is entity of line type and point is a dwg_point_3d datatype.
after calling this point.x will have end x value and point.y will have
end y value.
*/
void
dwg_ent_line_get_end_point(dwg_ent_line *line, dwg_point_3d *point, int *error)
{
    if (line != 0)
        {
            *error = 0;
            point->x = line->end.x;
            point->y = line->end.y;
            point->z = line->end.z;
        }
    else
        {
            *error = 1;
        }
}

/* This function sets the line end points equal to the 
point values in second argument.
Usage :- dwg_ent_line_set_end_point(line, point);
line is entity of line type and point is a dwg_point_3d datatype.
*/
void
dwg_ent_line_set_end_point(dwg_ent_line *line, dwg_point_3d *point, int *error)
{
    if (line != 0)
        {
            *error = 0;
            line->end.x = point->x;
            line->end.y = point->y;
            line->end.z = point->z;
        }
    else
        {
            *error = 1;
        }
}

/* Returns line thickness of line entity passed in argument. 
Usage :- double thickness = dwg_ent_line_get_thickness(line);
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
        {
            *error = 1;
        }
}

/* Sets line thickness equal to (thickness) value passed in second 
argument.
Usage :- dwg_ent_line_set_thickness(line, thickness_value);
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
        {
            *error = 1;
        }
}

/* This function returns the extrusion of a line entity into second 
argument.
Usage :- dwg_ent_line_get_extrusion(line, extrusion_value);
where line is an entity of line type, and extrusion value contains
the x, y and z values
*/
void
dwg_ent_line_get_extrusion(dwg_ent_line *line, dwg_point_3d *vector, int *error)
{
    if (line != 0)
        {
            *error = 0;
            vector->x = line->extrusion.x;
            vector->y = line->extrusion.y;
            vector->z = line->extrusion.z;
        }
    else
        {
            *error = 1;
        }
}

/* This function sets the extrusion of a line entity equal to 
values in second argument.
Usage :- dwg_ent_line_set_extrusion(line, extrusion_value);
where line is an entity of line type, and extrusion value contains
the x, y and z values
*/
void
dwg_ent_line_set_extrusion(dwg_ent_line *line, dwg_point_3d *vector, int *error)
{
    if (line != 0)
        {
            *error = 0;
            line->extrusion.x = vector->x;
            line->extrusion.y = vector->y;
            line->extrusion.z = vector->z;
        }
    else
        {
            *error = 1;
        }
}

//---------------------------------------------------------------------------

/* This function creates a new entity of arc type.
Usage :- dwg_ent_arc_new();
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
        {
            *error = 1;
        }
}

/* This function deletes entity created by dwg_ent_arc_new() function
and frees the allocated memory.
Usage :- dwg_ent_arc_delete();
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
        {
            *error = 1;
        }
}

/* This function initialises a arc entity with default/zero values.
Usage :- dwg_ent_arc_init();
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

/* Returns the center values of arc to second argument.
Usage :- dwg_ent_arc_get_center(arc, point);
where arc is a pointer of dwg_ent_arc type and point is 
of dwg_point_3d. point.x will contain x value of center,
point.y will contain y value of center
*/
void
dwg_ent_arc_get_center(dwg_ent_arc *arc, dwg_point_3d *point, int *error)
{
    if (arc != 0)
        {
            *error = 0;
            point->x = arc->center.x;
            point->y = arc->center.y;
            point->z = arc->center.z;
        }
    else
        {
            *error = 1;
        }
}

/* Sets the second argument values equal to the arc center.
Usage :- dwg_ent_arc_get_center(arc, point);
where arc is a pointer of dwg_ent_arc type and point is 
of dwg_point_3d. point.x will contain x value of center,
point.y will contain y value of center
*/
void
dwg_ent_arc_set_center(dwg_ent_arc *arc, dwg_point_3d *point, int *error)
{
    if (arc != 0)
        {
            *error = 0;
            arc->center.x = point->x;
            arc->center.y = point->y;
            arc->center.z = point->z;
        }
    else
        {
            *error = 1;
        }
}

/* Returns radius of arc entity passed in argument.
Usage :- double arc_radius = dwg_ent_arc_get_radius(arc);
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
        {
            *error = 1;
        }
}

/* Sets radius of arc entity equal to (radius) value 
provided in second argument.
Usage :- dwg_ent_arc_set_radius(arc, 40.0);
hence radius of arc sets to 40.0
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
        {
            *error = 1;
        }
}

/* Returns thickness of entity passed in argument.
Usage :- 
double thickness;
thickness = dwg_ent_arc_get_thickness(circle);
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
        {
            *error = 1;
        }
}

/* Sets thickness of a arc entity passed in 
first argument equal to (thickness) value in second argument.
Usage :- dwg_ent_circle_set_thickness(circle, 50.0);
hence circle thickness becomes 50.0 
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
        {
            *error = 1;
        }
}

/* This function returns the extrusion of a arc entity into second 
argument.
Usage :- dwg_ent_arc_get_extrusion(arc, extrusion_value);
where arc is an entity of arc type, and extrusion value contains
the x, y and z values
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
        {
            *error = 1;
        }
}

/* This function sets the extrusion of a arc entity equal to 
data in second argument.
Usage :- dwg_ent_arc_set_extrusion(arc, extrusion_value);
where arc is an entity of arc type, and extrusion value contains
the x, y and z values
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
        {
            *error = 1;
        }
}

/* Returns start angle of arc passed in argument.
Usage :- double start_angle = dwg_ent_arc_get_start_angle(arc);
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
        {
            *error = 1;
        }
}

/* Sets start angle of arc passed in argument 1 equal to (angle)
value in argument 2.
Usage :- double start_angle = dwg_ent_arc_get_start_angle(arc);
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
        {
            *error = 1;
        }
}

/* Returns end angle of arc passed in argument.
Usage :- double end_angle = dwg_ent_arc_get_end_angle(arc);
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
        {
            *error = 1;
        }
}

/* Sets end angle of arc passed in argument 1 equal to (angle)
value in argument 2.
Usage :- double end_angle = dwg_ent_arc_get_end_angle(arc);
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
        {
            *error = 1;
        }
}

//---------------------------------------------------------------------------

/* This function creates a new entity of ellipse type.
Usage :- dwg_ent_ellipse_new();
*/
dwg_ent_ellipse *
dwg_ent_ellipse_new(int *error)
{
    dwg_ent_ellipse *ellipse = (dwg_ent_ellipse*) malloc(sizeof(Dwg_Entity_ELLIPSE));
    if (ellipse != 0)
        {
            *error = 0;
            return dwg_ent_ellipse_init(ellipse);
        }
    else
        {
            *error = 1;
        }
}

/* This function deletes entity created by dwg_ent_ellipse_new() function
and frees the allocated memory.
Usage :- dwg_ent_ellipse_delete();
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
        {
            *error = 1;
        }
}

/* This function initialises a ellipse entity with default or zero values.
Usage :- dwg_ent_ellipse_init();
*/
dwg_ent_ellipse *
dwg_ent_ellipse_init(dwg_ent_ellipse *ellipse)
{
    if (ellipse != 0)
        {
            ellipse->center.x    = ellipse->center.y = ellipse->center.z = 0.0;
            ellipse->sm_axis.x   = ellipse->sm_axis.y = ellipse->sm_axis.z = 0.0;
            ellipse->extrusion.x = ellipse->extrusion.y = ellipse->extrusion.z = 0.0;
            ellipse->axis_ratio  = 0.0;
            ellipse->start_angle = 0.0;
            ellipse->end_angle   = 0.0;
        }
    return ellipse;
}

/* Returns the center values of ellipse to second argument.
Usage :- dwg_ent_ellipse_get_center(ellipse, point);
where ellipse is a pointer of dwg_ent_ellipse type and point is 
of dwg_point_3d. point.x will contain x value of center,
point.y will contain y value of center
*/
void
dwg_ent_ellipse_get_center(dwg_ent_ellipse *ellipse, dwg_point_3d *point, int *error)
{
    if (ellipse != 0)
        {
            *error = 0;
            point->x = ellipse->center.x;
            point->y = ellipse->center.y;
            point->z = ellipse->center.z;
        }
    else
        {
            *error = 1;
        }
}

/* Sets the ellipse center values equal to values of second argument.
Usage :- dwg_ent_ellipse_get_center(ellipse, point);
where ellipse is a pointer of dwg_ent_ellipse type and point is 
of dwg_point_3d. point.x will contain x value of center,
point.y will contain y value of center
*/
void
dwg_ent_ellipse_set_center(dwg_ent_ellipse *ellipse, dwg_point_3d *point, int *error)
{
    if (ellipse != 0)
        {
            *error = 0;
            ellipse->center.x = point->x;
            ellipse->center.y = point->y;
            ellipse->center.z = point->z;
        }
    else
        {
            *error = 1;
        }
}

/* Returns the sm_axis values of ellipse to second argument.
Usage :- dwg_ent_ellipse_get_sm_axis(ellipse, point);
where ellipse is a pointer of dwg_ent_ellipse type and point is 
of dwg_point_3d. point.x will contain x value of sm_axis,
point.y will contain y value of sm_axis
*/
void
dwg_ent_ellipse_get_sm_axis(dwg_ent_ellipse *ellipse, dwg_point_3d *point, int *error)
{
    if (ellipse != 0)
        {
            *error = 0;
            point->x = ellipse->sm_axis.x;
            point->y = ellipse->sm_axis.y;
            point->z = ellipse->sm_axis.z;
        }
    else
        {
            *error = 1;
        }
}

/* Sets the ellipse sm_axis values equal to values of second argument.
Usage :- dwg_ent_ellipse_get_sm_axis(ellipse, point);
where ellipse is a pointer of dwg_ent_ellipse type and point is 
of dwg_point_3d. point.x will contain x value of sm_axis,
point.y will contain y value of sm_axis
*/
void
dwg_ent_ellipse_set_sm_axis(dwg_ent_ellipse *ellipse, dwg_point_3d *point, int *error)
{
    if (ellipse != 0)
        {
            *error = 0;
            ellipse->sm_axis.x = point->x;
            ellipse->sm_axis.y = point->y;
            ellipse->sm_axis.z = point->z;
        }
    else
        {
            *error = 1;
        }
}

/* Returns the extrusion values of ellipse to second argument.
Usage :- dwg_ent_ellipse_get_extrusion(ellipse, point);
where ellipse is a pointer of dwg_ent_ellipse type and point is 
of dwg_point_3d. point.x will contain x value of extrusion,
point.y will contain y value of extrusion
*/
void
dwg_ent_ellipse_get_extrusion(dwg_ent_ellipse *ellipse, dwg_point_3d *vector, int *error)
{
    if (ellipse != 0)
        {
            *error = 0;
            vector->x = ellipse->extrusion.x;
            vector->y = ellipse->extrusion.y;
            vector->z = ellipse->extrusion.z;
        }
    else
        {
            *error = 1;
        }
}

/* Sets the ellipse extrusion values equal to values of second argument.
Usage :- dwg_ent_ellipse_set_extrusion(ellipse, point);
where ellipse is a pointer of dwg_ent_ellipse type and point is 
of dwg_point_3d. point.x will contain x value of extrusion,
point.y will contain y value of extrusion
*/
void
dwg_ent_ellipse_set_extrusion(dwg_ent_ellipse *ellipse, dwg_point_3d *vector, int *error)
{
    if (ellipse != 0)
        {
            *error = 0;
            ellipse->extrusion.x = vector->x;
            ellipse->extrusion.y = vector->y;
            ellipse->extrusion.z = vector->z;
        }
    else
        {
            *error = 1;
        }
}

/* Returns the axis ratio of an ellipse entity passed in argument.
Usage :- double axis_ratio = dwg_ent_ellipse_get_axis_ratio(ellipse);
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
        {
            *error = 1;
        }
}

/* Sets the axis ratio of an ellipse entity equal to value passed in 2nd argument.
Usage :- dwg_ent_ellipse_get_axis_ratio(ellipse, 3.0);
hence axis ratio of ellipse becomes 3.0
*/
void
dwg_ent_ellipse_set_axis_ratio(dwg_ent_ellipse *ellipse, double ratio, int *error)
{
    if (ellipse != 0)
        {
            *error = 0;
            ellipse->axis_ratio = ratio;
        }
    else
        {
            *error = 1;
        }
}

/* Returns start angle of ellipse passed in argument.
Usage :- double start_angle = dwg_ent_ellipse_get_start_angle(ellipse);
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
        {
            *error = 1;
        }
}

/* Sets start angle of ellipse passed in argument 1 equal to (angle)
value in argument 2.
Usage :- double start_angle = dwg_ent_ellipse_get_start_angle(ellipse);
*/
void
dwg_ent_ellipse_set_start_angle(dwg_ent_ellipse *ellipse, double start_angle, int *error)
{
    if (ellipse != 0)
        {
            *error = 0;
            ellipse->start_angle = start_angle;
        }
    else
        {
            *error = 1;
        }
}

/* Returns end angle of ellipse passed in argument.
Usage :- double end_angle = dwg_ent_ellipse_get_end_angle(ellipse);
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
        {
            *error = 1;
        }
}

/* Sets end angle of ellipse passed in argument 1 equal to (angle)
value in argument 2.
Usage :- double end_angle = dwg_ent_ellipse_get_end_angle(ellipse);
*/
void
dwg_ent_ellipse_set_end_angle(dwg_ent_ellipse *ellipse, double end_angle, int *error)
{
    if (ellipse != 0)
        {
            *error = 0;
            ellipse->end_angle = end_angle;
        }
    else
        {
            *error = 1;
        }
}

//---------------------------------------------------------------------------

/* This sets text value of a text entity equal to value provided 
in second argument.
Usage :- dwg_ent_text_set_text(text, "Hello world");
This will set text value to Hello world.
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
        {
            *error = 1;
        }
}

/* This returns the text value of a text entity.
Usage :- dwg_ent_text_get_text(text);
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
        {
            *error = 1;
        }
}

/* This returns the insertion point of a text entity 
into second argument.
Usage :- dwg_ent_text_get_insertion_point(text, point);
point.x and point.y contains x and y cordinate data respectively.
*/
void
dwg_ent_text_get_insertion_point(dwg_ent_text *text, dwg_point_2d *point, int *error)
{
    if (text != 0)
        {
            *error = 0;
            point->x = text->insertion_pt.x;
            point->y = text->insertion_pt.y;
        }
    else
        {
            *error = 1;
        }
}

/* This sets the insertion point of a text entity equal to 
co-ordinate values in second argument.
Usage :- dwg_ent_text_set_insertion_point(text, point)
*/
void
dwg_ent_text_set_insertion_point(dwg_ent_text *text, dwg_point_2d *point, int *error)
{
    if (text != 0)
        {
            *error = 0;
            text->insertion_pt.x = point->x;
            text->insertion_pt.y = point->y;
        }
    else
        {
            *error = 1;
        }
}

/* This returns the height of a text entity.
Usage :- dwg_ent_text_get_height(text);
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
        {
            *error = 1;
        }
}

/* This sets height of a text entity equal to value provided 
in second argument.
Usage :- dwg_ent_text_set_height(text, 100);
This will set height to 100.
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
        {
            *error = 1;
        }
}

/* Returns the extrusion values of text to second argument.
Usage :- dwg_ent_text_get_extrusion(text, point);
where text is a pointer of dwg_ent_text type and point is 
of dwg_point_3d. point.x will contain x value of extrusion,
point.y will contain y value of extrusion
*/
void
dwg_ent_text_get_extrusion(dwg_ent_text *text, dwg_point_3d *vector, int *error)
{
    if (text != 0)
        {
            *error = 0;
            vector->x = text->extrusion.x;
            vector->y = text->extrusion.y;
            vector->z = text->extrusion.z;
        }
    else
        {
            *error = 1;
        }
}

/* Sets the text extrusion values equal to values of second argument.
Usage :- dwg_ent_text_set_extrusion(text, point);
where text is a pointer of dwg_ent_text type and point is 
of dwg_point_3d. point.x will contain x value of extrusion,
point.y will contain y value of extrusion
*/
void
dwg_ent_text_set_extrusion(dwg_ent_text *text, dwg_point_3d *vector, int *error)
{
    if (text != 0)
        {
            *error = 0;
            text->extrusion.x = vector->x;
            text->extrusion.y = vector->y;
            text->extrusion.z = vector->z;
        }
    else
        {
            *error = 1;
        }
}

/* This returns the thickness of a text entity.
Usage :- dwg_ent_text_get_thickness(text);
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
        {
            *error = 1;
        }
}

/* This sets the thickness of a text entity equal to 
value in second argument.
Usage :- dwg_ent_text_set_thickness(text, thickness);
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
        {
            *error = 1;
        }
}

/* This returns the rotation angle of a text entity.
Usage :- dwg_ent_text_get_rot_angle(text);
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
        {
            *error = 1;
        }
}

/* This sets the rotation angle of a text entity equal to 
value in second argument.
Usage :- dwg_ent_text_set_rot_angle(text, angle);
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
        {
            *error = 1;
        }
}

/* This returns the vertical alignment of a text entity.
Usage :- dwg_ent_text_get_vert_align(text);
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
        {
            *error = 1;
        }
}

/* This sets the vertical alignment of a text entity 
equal to value in second argument.
Usage :- dwg_ent_text_set_vert_align(text, angle);
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
        {
            *error = 1;
        }
}

/* This returns the horizontal alignment of a text entity.
Usage :- dwg_ent_text_get_horiz_align(text);
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
        {
            *error = 1;
        }
}

/* This sets the horizontal alignment of a text entity 
equal to value in second argument.
Usage :- dwg_ent_text_set_horiz_align(text, angle);
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
        {
            *error = 1;
        }
}

//---------------------------------------------------------------------------

/* This sets text value of a attrib entity equal to value provided 
in second argument.
Usage :- dwg_ent_attrib_set_text(attrib, "Hello world");
This will set text value to Hello world.
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
        {
            *error = 1;
        }
}

/* This returns the text value of a attrib entity.
Usage :- dwg_ent_attrib_get_text(attrib);
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
        {
            *error = 1;
        }
}

/* This returns the insertion point of a attrib entity 
into second argument.
Usage :- dwg_ent_attrib_get_insertion_point(attrib, point);
point.x and point.y contains x and y cordinate data respectively.
*/
void
dwg_ent_attrib_get_insertion_point(dwg_ent_attrib *attrib, dwg_point_2d *point, int *error)
{
    if (attrib != 0)
        {
            *error = 0;
            point->x = attrib->insertion_pt.x;
            point->y = attrib->insertion_pt.y;
        }
    else
        {
            *error = 1;
        }
}

/* This sets the insertion point of a attrib entity equal to 
co-ordinate values in second argument.
Usage :- dwg_ent_attrib_set_insertion_point(attrib, point)
*/
void
dwg_ent_attrib_set_insertion_point(dwg_ent_attrib *attrib, dwg_point_2d *point, int *error)
{
    if (attrib != 0)
        {
            *error = 0;
            attrib->insertion_pt.x = point->x;
            attrib->insertion_pt.y = point->y;
        }
    else
        {
            *error = 1;
        }
}

/* This returns the height of a attrib entity.
Usage :- dwg_ent_attrib_get_height(attrib);
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
        {
            *error = 1;
        }
}

/* This sets height of a attrib entity equal to value provided 
in second argument.
Usage :- dwg_ent_attrib_set_height(attrib, 100);
This will set height to 100.
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
        {
            *error = 1;
        }
}

/* Returns the extrusion values of attrib to second argument.
Usage :- dwg_ent_attrib_get_extrusion(attrib, point);
where attrib is a pointer of dwg_ent_attrib type and point is 
of dwg_point_3d. point.x will contain x value of extrusion,
point.y will contain y value of extrusion
*/
void
dwg_ent_attrib_get_extrusion(dwg_ent_attrib *attrib, dwg_point_3d *vector, int *error)
{
    if (attrib != 0)
        {
            *error = 0;
            vector->x = attrib->extrusion.x;
            vector->y = attrib->extrusion.y;
            vector->z = attrib->extrusion.z;
        }
    else
        {
            *error = 1;
        }
}

/* Sets the attrib extrusion values equal to values of second argument.
Usage :- dwg_ent_attrib_set_extrusion(attrib, point);
where attrib is a pointer of dwg_ent_attrib type and point is 
of dwg_point_3d. point.x will contain x value of extrusion,
point.y will contain y value of extrusion
*/
void
dwg_ent_attrib_set_extrusion(dwg_ent_attrib *attrib, dwg_point_3d *vector, int *error)
{
    if (attrib != 0)
        {
            *error = 0;
            attrib->extrusion.x = vector->x;
            attrib->extrusion.y = vector->y;
            attrib->extrusion.z = vector->z;
        }
    else
        {
            *error = 1;
        }
}

/* This returns the thickness of a attrib entity.
Usage :- dwg_ent_attrib_get_thickness(attrib);
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
        {
            *error = 1;
        }
}

/* This sets the thickness of a attrib entity equal to 
value in second argument.
Usage :- dwg_ent_attrib_set_thickness(attrib, thickness);
*/
void
dwg_ent_attrib_set_thickness(dwg_ent_attrib *attrib, double thickness, int *error)
{
    if (attrib != 0)
        {
            *error = 0;
            attrib->thickness = thickness;
        }
    else
        {
            *error = 1;
        }
}

/* This returns the rotation angle of a attrib entity.
Usage :- dwg_ent_attrib_get_rot_angle(attrib);
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
        {
            *error = 1;
        }
}

/* This sets the rotation angle of a attrib entity equal to 
value in second argument.
Usage :- dwg_ent_attrib_set_rot_angle(attrib, angle);
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
        {
            *error = 1;
        }
}

/* This returns the vertical alignment of a attrib entity.
Usage :- dwg_ent_attrib_get_vert_align(attrib);
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
        {
            *error = 1;
        }
}

/* This sets the vertical alignment of a attrib entity 
equal to value in second argument.
Usage :- dwg_ent_attrib_set_vert_align(attrib, angle);
*/
void
dwg_ent_attrib_set_vert_align(dwg_ent_attrib *attrib, double alignment, int *error)
{
    if (attrib != 0)
        {
            *error = 0;
            attrib->vert_alignment = alignment;
        }
    else
        {
            *error = 1;
        }
}

/* This returns the horizontal alignment of a attrib entity.
Usage :- dwg_ent_attrib_get_horiz_align(attrib);
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
        {
            *error = 1;
        }
}

/* This sets the horizontal alignment of a attrib entity 
equal to value in second argument.
Usage :- dwg_ent_attrib_set_horiz_align(attrib, angle);
*/
void
dwg_ent_attrib_set_horiz_align(dwg_ent_attrib *attrib, double alignment, int *error)
{
    if (attrib != 0)
        {
            *error = 0;
            attrib->horiz_alignment = alignment;
        }
    else
        {
            *error = 1;
        }
}

//---------------------------------------------------------------------------

/* This sets text value of a attdef entity equal to value provided 
in second argument.
Usage :- dwg_ent_attdef_set_text(attdef, "Hello world");
This will set text value to Hello world.
*/
void
dwg_ent_attdef_set_text(dwg_ent_attdef *attdef, char * default_value, int *error)
{
    if (attdef != 0)
        {
            *error = 0;
            attdef->default_value = default_value;
        }
    else
        {
            *error = 1;
        }
}

/* This returns the text value of a attdef entity.
Usage :- dwg_ent_attdef_get_text(attdef);
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
        {
            *error = 1;
        }
}

/* This returns the insertion point of a attdef entity 
into second argument.
Usage :- dwg_ent_attdef_get_insertion_point(attdef, point);
point.x and point.y contains x and y cordinate data respectively.
*/
void
dwg_ent_attdef_get_insertion_point(dwg_ent_attdef *attdef, dwg_point_2d *point, int *error)
{
    if (attdef != 0)
        {
            *error = 0;
            point->x = attdef->insertion_pt.x;
            point->y = attdef->insertion_pt.y;
        }
    else
        {
            *error = 1;
        }
}

/* This sets the insertion point of a attdef entity equal to 
co-ordinate values in second argument.
Usage :- dwg_ent_attdef_set_insertion_point(attdef, point)
*/
void
dwg_ent_attdef_set_insertion_point(dwg_ent_attdef *attdef, dwg_point_2d *point, int *error)
{
    if (attdef != 0)
        {
            *error = 0;
            attdef->insertion_pt.x = point->x;
            attdef->insertion_pt.y = point->y;
        }
    else
        {
            *error = 1;
        }
}

/* This returns the height of a attdef entity.
Usage :- dwg_ent_attdef_get_height(attdef);
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
        {
            *error = 1;
        }
}

/* This sets height of a attdef entity equal to value provided 
in second argument.
Usage :- dwg_ent_attdef_set_height(attdef, 100);
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
        {
            *error = 1;
        }
}

/* Returns the extrusion values of attdef to second argument.
Usage :- dwg_ent_attdef_get_extrusion(attdef, point);
where attdef is a pointer of dwg_ent_attdef type and point is 
of dwg_point_3d. point.x will contain x value of extrusion,
point.y will contain y value of extrusion
*/
void
dwg_ent_attdef_get_extrusion(dwg_ent_attdef *attdef, dwg_point_3d *vector, int *error)
{
    if (attdef != 0)
        {
            *error = 0;
            vector->x = attdef->extrusion.x;
            vector->y = attdef->extrusion.y;
            vector->z = attdef->extrusion.z;
        }
    else
        {
            *error = 1;
        }
}

/* Sets the attdef extrusion values equal to values of second argument.
Usage :- dwg_ent_attdef_set_extrusion(attdef, point);
where attdef is a pointer of dwg_ent_attdef type and point is 
of dwg_point_3d. point.x will contain x value of extrusion,
point.y will contain y value of extrusion
*/
void
dwg_ent_attdef_set_extrusion(dwg_ent_attdef *attdef, dwg_point_3d *vector, int *error)
{
    if (attdef != 0)
        {
            *error = 0;
            attdef->extrusion.x = vector->x;
            attdef->extrusion.y = vector->y;
            attdef->extrusion.z = vector->z;
        }
    else
        {
            *error = 1;
        }
}

/* This returns the thickness of a attdef entity.
Usage :- dwg_ent_attdef_get_thickness(attdef);
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
        {
            *error = 1;
        }
}

/* This sets the thickness of a attdef entity equal to 
value in second argument.
Usage :- dwg_ent_attdef_set_thickness(attdef, thickness);
*/
void
dwg_ent_attdef_set_thickness(dwg_ent_attdef *attdef, double thickness, int *error)
{
    if (attdef != 0)
        {
            *error = 0;
            attdef->thickness = thickness;
        }
    else
        {
            *error = 1;
        }
}

/* This returns the rotation angle of a attdef entity.
Usage :- dwg_ent_attdef_get_rot_angle(attdef);
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
        {
            *error = 1;
        }
}

/* This sets the rotation angle of a attdef entity equal to 
value in second argument.
Usage :- dwg_ent_attdef_set_rot_angle(attdef, angle);
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
        {
            *error = 1;
        }
}

/* This returns the vertical alignment of a attdef entity.
Usage :- dwg_ent_attdef_get_vert_align(attdef);
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
        {
            *error = 1;
        }
}

/* This sets the vertical alignment of a attdef entity 
equal to value in second argument.
Usage :- dwg_ent_attdef_set_vert_align(attdef, angle);
*/
void
dwg_ent_attdef_set_vert_align(dwg_ent_attdef *attdef, double alignment, int *error)
{
    if (attdef != 0)
        {
            *error = 0;
            attdef->vert_alignment = alignment;
        }
    else
        {
            *error = 1;
        }
}

/* This returns the horizontal alignment of a attdef entity.
Usage :- dwg_ent_attdef_get_horiz_align(attdef);
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
        {
            *error = 1;
        }
}

/* This sets the horizontal alignment of a attdef entity 
equal to value in second argument.
Usage :- dwg_ent_attdef_set_horiz_align(attdef, angle);
*/
void
dwg_ent_attdef_set_horiz_align(dwg_ent_attdef *attdef, double alignment, int *error)
{
    if (attdef != 0)
        {
            *error = 0;
            attdef->horiz_alignment = alignment;
        }
    else
        {
            *error = 1;
        }
}

//-------------------------------------------------------------------------------

/* Sets the point point values equal to values of second argument.
Usage :- dwg_ent_point_set_point(point, retpoint);
where point is a pointer of dwg_ent_point type and retpoint is 
of dwg_point_3d. retpoint.x will contain x value of point,
retpoint.y will contain y value of point
*/
void
dwg_ent_point_set_point(dwg_ent_point *point, dwg_point_3d *retpoint, int *error)
{    
    if (point != 0)
        {
            *error = 0;
            point->x = retpoint->x;
            point->y = retpoint->y;
            point->z = retpoint->z;
        }
    else
        {
            *error = 1;
        }
}

/* Returns the point values of point to second argument.
Usage :- dwg_ent_point_get_point(point, retpoint);
where point is a pointer of dwg_ent_point type and retpoint is 
of dwg_point_3d. retpoint.x will contain x value of point,
retpoint.y will contain y value of point
*/
void
dwg_ent_point_get_point(dwg_ent_point *point, dwg_point_3d *retpoint, int *error)
{    
    if (point != 0)
        {
            *error = 0;
            retpoint->x = point->x;
            retpoint->y = point->y;
            retpoint->z = point->z;
        }
    else
        {
            *error = 1;
        }
}

/* This returns the thickness of a point entity.
Usage :- dwg_ent_point_get_thickness(point);
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
        {
            *error = 1;
        }
}

/* This sets the thickness of a point entity equal to 
value in second argument.
Usage :- dwg_ent_point_set_thickness(point, thickness);
*/
void
dwg_ent_point_set_thickness(dwg_ent_point *point, double thickness, int *error)
{    
    if (point != 0)
        {
            *error = 0;
            point->thickness = thickness;
        }
    else
        {
            *error = 1;
        }
}

/* Sets the point extrusion values equal to values of second argument.
Usage :- dwg_ent_point_set_extrusion(point, retpoint);
where point is a pointer of dwg_ent_point type and retpoint is 
of dwg_point_3d. retpoint.x will contain x value of extrusion,
retpoint.y will contain y value of extrusion
*/
void
dwg_ent_point_set_extrusion(dwg_ent_point *point, dwg_point_3d *retpoint, int *error)
{    
    if (point != 0)
        {
            *error = 0;
            point->extrusion.x = retpoint->x;
            point->extrusion.y = retpoint->y;
            point->extrusion.z = retpoint->z;
        }
    else
        {
            *error = 1;
        }
}

/* Returns the extrusion values of point to second argument.
Usage :- dwg_ent_point_get_extrusion(point, retpoint);
where point is a pointer of dwg_ent_point type and retpoint is 
of dwg_point_3d. retpoint.x will contain x value of extrusion,
retpoint.y will contain y value of extrusion
*/
void
dwg_ent_point_get_extrusion(dwg_ent_point *point, dwg_point_3d *retpoint, int *error)
{    
    if (point != 0)
        {
            *error = 0;
            retpoint->x = point->extrusion.x;
            retpoint->y = point->extrusion.y;
            retpoint->z = point->extrusion.z;
        }
    else
        {
            *error = 1;
        }
}

//-------------------------------------------------------------------------------

/* This returns the thickness of a solid entity.
Usage :- dwg_ent_solid_get_thickness(solid);
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
        {
            *error = 1;
        }
}

/* This sets the thickness of a solid entity equal to 
value in second argument.
Usage :- dwg_ent_solid_set_thickness(solid, 2.0);
Hence thickness sets to 2.0
*/
void
dwg_ent_solid_set_thickness(dwg_ent_solid *solid, double thickness, int *error)
{    
    if (solid != 0)
        {
            *error = 0;
            solid->thickness = thickness;
        }
    else
        {
            *error = 1;
        }
}

/* This returns the elevation of a solid entity.
Usage :- dwg_ent_solid_get_elevation(solid);
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
        {
            *error = 1;
        }
}

/* This sets the elevation of a solid entity equal to 
value in second argument.
Usage :- dwg_ent_solid_set_elevation(solid, 20);
Hence elevation sets to 20
*/
void
dwg_ent_solid_set_elevation(dwg_ent_solid *solid, double elevation, int *error)
{    
    if (solid != 0)
        {
            *error = 0;
            solid->elevation = elevation;
        }
    else
        {
            *error = 1;
        }
}

/* Returns the corner1 values of solid to second argument.
Usage :- dwg_ent_solid_get_corner1(solid, point);
where solid is a pointer of dwg_ent_solid type and point is 
of dwg_point_3d. point.x will contain x value of corner1,
point.y will contain y value of corner1
*/
void
dwg_ent_solid_get_corner1(dwg_ent_solid *solid, dwg_point_2d *point, int *error)
{    
    if (solid != 0)
        {
            *error = 0;
            point->x = solid->corner1.x;
            point->y = solid->corner1.y;
        }
    else
        {
            *error = 1;
        }
}

/* Sets the solid corner1 values equal to values of second argument.
Usage :- dwg_ent_solid_set_corner1(solid, point);
where solid is a pointer of dwg_ent_solid type and point is 
of dwg_point_3d. point.x will contain x value of corner1,
point.y will contain y value of corner1
*/
void
dwg_ent_solid_set_corner1(dwg_ent_solid *solid, dwg_point_2d *point, int *error)
{    
    if (solid != 0)
        {
            *error = 0;
            solid->corner1.x = point->x;
            solid->corner1.y = point->y;
        }
    else
        {
            *error = 1;
        }
}

/* Returns the corner2 values of solid to second argument.
Usage :- dwg_ent_solid_get_corner2(solid, point);
where solid is a pointer of dwg_ent_solid type and point is 
of dwg_point_3d. point.x will contain x value of corner2,
point.y will contain y value of corner2
*/
void
dwg_ent_solid_get_corner2(dwg_ent_solid *solid, dwg_point_2d *point, int *error)
{    
    if (solid != 0)
        {
            *error = 0;
            point->x = solid->corner2.x;
            point->y = solid->corner2.y;
        }
    else
        {
            *error = 1;
        }
}

/* Sets the solid corner2 values equal to values of second argument.
Usage :- dwg_ent_solid_set_corner2(solid, point);
where solid is a pointer of dwg_ent_solid type and point is 
of dwg_point_3d. point.x will contain x value of corner2,
point.y will contain y value of corner2
*/
void
dwg_ent_solid_set_corner2(dwg_ent_solid *solid, dwg_point_2d *point, int *error)
{    
    if (solid != 0)
        {
            *error = 0;
            solid->corner2.x = point->x;
            solid->corner2.y = point->y;
        }
    else
        {
            *error = 1;
        }
}

/* Returns the corner3 values of solid to second argument.
Usage :- dwg_ent_solid_get_corner3(solid, point);
where solid is a pointer of dwg_ent_solid type and point is 
of dwg_point_3d. point.x will contain x value of corner3,
point.y will contain y value of corner3
*/
void
dwg_ent_solid_get_corner3(dwg_ent_solid *solid, dwg_point_2d *point, int *error)
{    
    if (solid != 0)
        {
            *error = 0;
            point->x = solid->corner3.x;
            point->y = solid->corner3.y;
        }
    else
        {
            *error = 1;
        }
}

/* Sets the solid corner3 values equal to values of second argument.
Usage :- dwg_ent_solid_set_corner3(solid, point);
where solid is a pointer of dwg_ent_solid type and point is 
of dwg_point_3d. point.x will contain x value of corner3,
point.y will contain y value of corner3
*/
void
dwg_ent_solid_set_corner3(dwg_ent_solid *solid, dwg_point_2d *point, int *error)
{    
    if (solid != 0)
        {
            *error = 0;
            solid->corner3.x = point->x;
            solid->corner3.y = point->y;
        }
    else
        {
            *error = 1;
        }
}

/* Returns the corner4 values of solid to second argument.
Usage :- dwg_ent_solid_get_corner4(solid, point);
where solid is a pointer of dwg_ent_solid type and point is 
of dwg_point_3d. point.x will contain x value of corner4,
point.y will contain y value of corner4
*/
void
dwg_ent_solid_get_corner4(dwg_ent_solid *solid, dwg_point_2d *point, int *error)
{    
    if (solid != 0)
        {
            *error = 0;
            point->x = solid->corner4.x;
            point->y = solid->corner4.y;
        }
    else
        {
            *error = 1;
        }
}

/* Sets the solid corner4 values equal to values of second argument.
Usage :- dwg_ent_solid_set_corner4(solid, point);
where solid is a pointer of dwg_ent_solid type and point is 
of dwg_point_3d. point.x will contain x value of corner4,
point.y will contain y value of corner4
*/
void
dwg_ent_solid_set_corner4(dwg_ent_solid *solid, dwg_point_2d *point, int *error)
{    
    if (solid != 0)
        {
            *error = 0;
            solid->corner4.x = point->x;
            solid->corner4.y = point->y;
        }
    else
        {
            *error = 1;
        }
}

/* Returns the extrusion values of solid to second argument.
Usage :- dwg_ent_solid_get_extrusion(solid, point);
where solid is a pointer of dwg_ent_solid type and point is 
of dwg_point_3d. point.x will contain x value of extrusion,
point.y will contain y value of extrusion
*/
void
dwg_ent_solid_get_extrusion(dwg_ent_solid *solid, dwg_point_3d *vector, int *error)
{    
    if (solid != 0)
        {
            *error = 0;
            vector->x = solid->extrusion.x;
            vector->y = solid->extrusion.y;
            vector->z = solid->extrusion.z;
        }
    else
        {
            *error = 1;
        }
}

/* Sets the solid extrusion values equal to values of second argument.
Usage :- dwg_ent_solid_set_extrusion(solid, point);
where solid is a pointer of dwg_ent_solid type and point is 
of dwg_point_3d. point.x will contain x value of extrusion,
point.y will contain y value of extrusion
*/
void
dwg_ent_solid_set_extrusion(dwg_ent_solid *solid, dwg_point_3d *vector, int *error)
{    
    if (solid != 0)
        {
            *error = 0;
            solid->extrusion.x = vector->x;
            solid->extrusion.y = vector->y;
            solid->extrusion.z = vector->z;
        }
    else
        {
            *error = 1;
        }
}

//-------------------------------------------------------------------------------

/* sets name of the block entity equal to value passed in 2nd argument.
Usage :- dwg_ent_block_get_name(block);
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
        {
            *error = 1;
        }
}

/* Returns name of the block entity passed in argument.
Usage :- dwg_ent_block_get_name(block);
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
        {
            *error = 1;
        }
}

//--------------------------------------------------------------------------------

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
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

//--------------------------------------------------------------------------------

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
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_xline_get_vector(dwg_ent_xline *xline, dwg_point_3d *vector, int *error)
{    
    if (xline != 0)
        {
            *error = 0;
            vector->x = xline->vector.x;
            vector->y = xline->vector.y;
            vector->z = xline->vector.z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_xline_set_vector(dwg_ent_xline *xline, dwg_point_3d *vector, int *error)
{    
    if (xline != 0)
        {
            *error = 0;
            xline->vector.x = vector->x;
            xline->vector.y = vector->y;
            xline->vector.z = vector->z;
        }
    else
        {
            *error = 1;
        }
}

//-------------------------------------------------------------------------------

/* This returns the thickness of a trace entity.
Usage :- dwg_ent_trace_get_thickness(trace);
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
        {
            *error = 1;
        }
}

/* This sets the thickness of a trace entity equal to 
value in second argument.
Usage :- dwg_ent_trace_set_thickness(trace, 2.0);
Hence thickness sets to 2.0
*/
void
dwg_ent_trace_set_thickness(dwg_ent_trace *trace, double thickness, int *error)
{    
    if (trace != 0)
        {
            *error = 0;
            trace->thickness = thickness;
        }
    else
        {
            *error = 1;
        }
}

/* This returns the elevation of a trace entity.
Usage :- dwg_ent_trace_get_elevation(trace);
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
        {
            *error = 1;
        }
}

/* This sets the elevation of a trace entity equal to 
value in second argument.
Usage :- dwg_ent_trace_set_elevation(trace, 20);
Hence elevation sets to 20
*/
void
dwg_ent_trace_set_elevation(dwg_ent_trace *trace, double elevation, int *error)
{    
    if (trace != 0)
        {
            *error = 0;
            trace->elevation = elevation;
        }
    else
        {
            *error = 1;
        }
}

/* Returns the corner1 values of trace to second argument.
Usage :- dwg_ent_trace_get_corner1(trace, point);
where trace is a pointer of dwg_ent_trace type and point is 
of dwg_point_3d. point.x will contain x value of corner1,
point.y will contain y value of corner1
*/
void
dwg_ent_trace_get_corner1(dwg_ent_trace *trace, dwg_point_2d *point, int *error)
{    
    if (trace != 0)
        {
            *error = 0;
            point->x = trace->corner1.x;
            point->y = trace->corner1.y;
        }
    else
        {
            *error = 1;
        }
}

/* Sets the trace corner1 values equal to values of second argument.
Usage :- dwg_ent_trace_set_corner1(trace, point);
where trace is a pointer of dwg_ent_trace type and point is 
of dwg_point_3d. point.x will contain x value of corner1,
point.y will contain y value of corner1
*/
void
dwg_ent_trace_set_corner1(dwg_ent_trace *trace, dwg_point_2d *point, int *error)
{    
    if (trace != 0)
        {
            *error = 0;
            trace->corner1.x = point->x;
            trace->corner1.y = point->y;
        }
    else
        {
            *error = 1;
        }
}

/* Returns the corner2 values of trace to second argument.
Usage :- dwg_ent_trace_get_corner2(trace, point);
where trace is a pointer of dwg_ent_trace type and point is 
of dwg_point_3d. point.x will contain x value of corner2,
point.y will contain y value of corner2
*/
void
dwg_ent_trace_get_corner2(dwg_ent_trace *trace, dwg_point_2d *point, int *error)
{    
    if (trace != 0)
        {
            *error = 0;
            point->x = trace->corner2.x;
            point->y = trace->corner2.y;
        }
    else
        {
            *error = 1;
        }
}

/* Sets the trace corner2 values equal to values of second argument.
Usage :- dwg_ent_trace_set_corner2(trace, point);
where trace is a pointer of dwg_ent_trace type and point is 
of dwg_point_3d. point.x will contain x value of corner2,
point.y will contain y value of corner2
*/
void
dwg_ent_trace_set_corner2(dwg_ent_trace *trace, dwg_point_2d *point, int *error)
{    
    if (trace != 0)
        {
            *error = 0;
            trace->corner2.x = point->x;
            trace->corner2.y = point->y;
        }
    else
        {
            *error = 1;
        }
}

/* Returns the corner3 values of trace to second argument.
Usage :- dwg_ent_trace_get_corner3(trace, point);
where trace is a pointer of dwg_ent_trace type and point is 
of dwg_point_3d. point.x will contain x value of corner3,
point.y will contain y value of corner3
*/
void
dwg_ent_trace_get_corner3(dwg_ent_trace *trace, dwg_point_2d *point, int *error)
{    
    if (trace != 0)
        {
            *error = 0;
            point->x = trace->corner3.x;
            point->y = trace->corner3.y;
        }
    else
        {
            *error = 1;
        }
}

/* Sets the trace corner3 values equal to values of second argument.
Usage :- dwg_ent_trace_set_corner3(trace, point);
where trace is a pointer of dwg_ent_trace type and point is 
of dwg_point_3d. point.x will contain x value of corner3,
point.y will contain y value of corner3
*/
void
dwg_ent_trace_set_corner3(dwg_ent_trace *trace, dwg_point_2d *point, int *error)
{    
    if (trace != 0)
        {
            *error = 0;
            trace->corner3.x = point->x;
            trace->corner3.y = point->y;
        }
    else
        {
            *error = 1;
        }
}

/* Returns the corner4 values of trace to second argument.
Usage :- dwg_ent_trace_get_corner4(trace, point);
where trace is a pointer of dwg_ent_trace type and point is 
of dwg_point_3d. point.x will contain x value of corner4,
point.y will contain y value of corner4
*/
void
dwg_ent_trace_get_corner4(dwg_ent_trace *trace, dwg_point_2d *point, int *error)
{    
    if (trace != 0)
        {
            *error = 0;
            point->x = trace->corner4.x;
            point->y = trace->corner4.y;
        }
    else
        {
            *error = 1;
        }
}

/* Sets the trace corner4 values equal to values of second argument.
Usage :- dwg_ent_trace_set_corner4(trace, point);
where trace is a pointer of dwg_ent_trace type and point is 
of dwg_point_3d. point.x will contain x value of corner4,
point.y will contain y value of corner4
*/
void
dwg_ent_trace_set_corner4(dwg_ent_trace *trace, dwg_point_2d *point, int *error)
{    
    if (trace != 0)
        {
            *error = 0;
            trace->corner4.x = point->x;
            trace->corner4.y = point->y;
        }
    else
        {
            *error = 1;
        }
}

/* Returns the extrusion values of trace to second argument.
Usage :- dwg_ent_trace_get_extrusion(trace, point);
where trace is a pointer of dwg_ent_trace type and point is 
of dwg_point_3d. point.x will contain x value of extrusion,
point.y will contain y value of extrusion
*/
void
dwg_ent_trace_get_extrusion(dwg_ent_trace *trace, dwg_point_3d *vector, int *error)
{    
    if (trace != 0)
        {
            *error = 0;
            vector->x = trace->extrusion.x;
            vector->y = trace->extrusion.y;
            vector->z = trace->extrusion.z;
        }
    else
        {
            *error = 1;
        }
}

/* Sets the trace extrusion values equal to values of second argument.
Usage :- dwg_ent_trace_set_extrusion(trace, point);
where trace is a pointer of dwg_ent_trace type and point is 
of dwg_point_3d. point.x will contain x value of extrusion,
point.y will contain y value of extrusion
*/
void
dwg_ent_trace_set_extrusion(dwg_ent_trace *trace, dwg_point_3d *vector, int *error)
{    
    if (trace != 0)
        {
            *error = 0;
            trace->extrusion.x = vector->x;
            trace->extrusion.y = vector->y;
            trace->extrusion.z = vector->z;
        }
    else
        {
            *error = 1;
        }
}

//-------------------------------------------------------------------------------

char
dwg_ent_vertex_3d_get_flags(dwg_ent_vertex_3d *vert, int *error)
{    
    if (vert != 0)
        {
            *error = 0;
            return vert->flags;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_vertex_3d_get_point(dwg_ent_vertex_3d *vert, dwg_point_3d *point, int *error)
{    
    if (vert != 0)
        {
            *error = 0;
            point->x = vert->point.x;
            point->y = vert->point.y;
            point->z = vert->point.z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_vertex_3d_set_point(dwg_ent_vertex_3d *vert, dwg_point_3d *point, int *error)
{    
    if (vert != 0)
        {
            *error = 0;
            vert->point.x = point->x;
            vert->point.y = point->y;
            vert->point.z = point->z;
        }
    else
        {
            *error = 1;
        }
}

//-------------------------------------------------------------------------------

char
dwg_ent_vertex_mesh_get_flags(dwg_ent_vertex_mesh *vert, int *error)
{    
    if (vert != 0)
        {
            *error = 0;
            return vert->flags;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_vertex_mesh_set_flags(dwg_ent_vertex_mesh *vert, char flags, int *error)
{    
    if (vert != 0)
        {
            *error = 0;
            vert->flags = flags;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_vertex_mesh_get_point(dwg_ent_vertex_mesh *vert, dwg_point_3d *point, int *error)
{    
    if (vert != 0)
        {
            *error = 0;
            point->x = vert->point.x;
            point->y = vert->point.y;
            point->z = vert->point.z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_vertex_mesh_set_point(dwg_ent_vertex_mesh *vert, dwg_point_3d *point, int *error)
{    
    if (vert != 0)
        {
            *error = 0;
            vert->point.x = point->x;
            vert->point.y = point->y;
            vert->point.z = point->z;
        }
    else
        {
            *error = 1;
        }
}

//-------------------------------------------------------------------------------

char
dwg_ent_vertex_pface_get_flags(dwg_ent_vertex_pface *vert, int *error)
{    
    if (vert != 0)
        {
            *error = 0;
            return vert->flags;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_vertex_pface_set_flags(dwg_ent_vertex_pface *vert, char flags, int *error)
{    
    if (vert != 0)
        {
            *error = 0;
            vert->flags = flags;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_vertex_pface_get_point(dwg_ent_vertex_pface *vert, dwg_point_3d *point, int *error)
{    
    if (vert != 0)
        {
            *error = 0;
            point->x = vert->point.x;
            point->y = vert->point.y;
            point->z = vert->point.z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_vertex_pface_set_point(dwg_ent_vertex_pface *vert, dwg_point_3d *point, int *error)
{    
    if (vert != 0)
        {
            *error = 0;
            vert->point.x = point->x;
            vert->point.y = point->y;
            vert->point.z = point->z;
        }
    else
        {
            *error = 1;
        }
}

//-------------------------------------------------------------------------------

char
dwg_ent_vertex_2d_get_flags(dwg_ent_vertex_2d *vert, int *error)
{    
    if (vert != 0)
        {
            *error = 0;
            return vert->flags;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_vertex_2d_get_point(dwg_ent_vertex_2d *vert, dwg_point_3d *point, int *error)
{    
    if (vert != 0)
        {
            *error = 0;
            point->x = vert->point.x;
            point->y = vert->point.y;
            point->z = vert->point.z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_vertex_2d_set_point(dwg_ent_vertex_2d *vert, dwg_point_3d *point, int *error)
{    
    if (vert != 0)
        {
            *error = 0;
            vert->point.x = point->x;
            vert->point.y = point->y;
            vert->point.z = point->z;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_vertex_2d_set_start_width(dwg_ent_vertex_2d *vert, double start_width, int *error)
{    
    if (vert != 0)
        {
            *error = 0;
            vert->start_width = start_width;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_vertex_2d_set_end_width(dwg_ent_vertex_2d *vert, double end_width, int *error)
{    
    if (vert != 0)
        {
            *error = 0;
            vert->end_width = end_width;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_vertex_2d_set_tangent_dir(dwg_ent_vertex_2d *vert, double tangent_dir, int *error)
{    
    if (vert != 0)
        {
            *error = 0;
            vert->tangent_dir = tangent_dir;
        }
    else
        {
            *error = 1;
        }
}

//--------------------------------------------------------------------------------

void
dwg_ent_insert_get_ins_pt(dwg_ent_insert *insert, dwg_point_3d *point, int *error)
{    
    if (insert != 0)
        {
            *error = 0;
            point->x = insert->ins_pt.x;
            point->y = insert->ins_pt.y;
            point->z = insert->ins_pt.z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_insert_set_ins_pt(dwg_ent_insert *insert, dwg_point_3d *point, int *error)
{    
    if (insert != 0)
        {
            *error = 0;
            insert->ins_pt.x = point->x;
            insert->ins_pt.y = point->y;
            insert->ins_pt.z = point->z;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_insert_get_scale(dwg_ent_insert *insert, dwg_point_3d *point, int *error)
{    
    if (insert != 0)
        {
            *error = 0;
            point->x = insert->scale.x;
            point->y = insert->scale.y;
            point->z = insert->scale.z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_insert_set_scale(dwg_ent_insert *insert, dwg_point_3d *point, int *error)
{    
    if (insert != 0)
        {
            *error = 0;
            insert->scale.x = point->x;
            insert->scale.y = point->y;
            insert->scale.z = point->z;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_insert_set_rotation_angle(dwg_ent_insert *insert, double rot_ang, int *error)
{    
    if (insert != 0)
        {
            *error = 0;
            insert->rotation_ang = rot_ang;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_insert_get_extrusion(dwg_ent_insert *insert, dwg_point_3d *point, int *error)
{    
    if (insert != 0)
        {
            *error = 0;
            point->x = insert->extrusion.x;
            point->y = insert->extrusion.y;
            point->z = insert->extrusion.z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_insert_set_extrusion(dwg_ent_insert *insert, dwg_point_3d *point, int *error)
{    
    if (insert != 0)
        {
            *error = 0;
            insert->extrusion.x = point->x;
            insert->extrusion.y = point->y;
            insert->extrusion.z = point->z;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_insert_set_has_attribs(dwg_ent_insert *insert, char attribs, int *error)
{    
    if (insert != 0)
        {
            *error = 0;
            insert->has_attribs = attribs;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_insert_set_owned_obj_count(dwg_ent_insert *insert, long count, int *error)
{    
    if (insert != 0)
        {
            *error = 0;
            insert->owned_obj_count = count;
        }
    else
        {
            *error = 1;
        }
}

//--------------------------------------------------------------------------------

void
dwg_ent_minsert_get_ins_pt(dwg_ent_minsert *minsert, dwg_point_3d *point, int *error)
{    
    if (minsert != 0)
        {
            *error = 0;
            point->x = minsert->ins_pt.x;
            point->y = minsert->ins_pt.y;
            point->z = minsert->ins_pt.z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_minsert_set_ins_pt(dwg_ent_minsert *minsert, dwg_point_3d *point, int *error)
{    
    if (minsert != 0)
        {
            *error = 0;
            minsert->ins_pt.x = point->x;
            minsert->ins_pt.y = point->y;
            minsert->ins_pt.z = point->z;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_minsert_set_scale_flag(dwg_ent_minsert *minsert, char  flags, int *error)
{    
    if (minsert != 0)
        {
            *error = 0;
            minsert->scale_flag = flags;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_minsert_get_scale(dwg_ent_minsert *minsert, dwg_point_3d *point, int *error)
{    
    if (minsert != 0)
        {
            *error = 0;
            point->x = minsert->scale.x;
            point->y = minsert->scale.y;
            point->z = minsert->scale.z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_minsert_set_scale(dwg_ent_minsert *minsert, dwg_point_3d *point, int *error)
{    
    if (minsert != 0)
        {
            *error = 0;
            minsert->scale.x = point->x;
            minsert->scale.y = point->y;
            minsert->scale.z = point->z;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_minsert_set_rotation_angle(dwg_ent_minsert *minsert, double rot_ang, int *error)
{    
    if (minsert != 0)
        {
            *error = 0;
            minsert->rotation_ang = rot_ang;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_minsert_get_extrusion(dwg_ent_minsert *minsert, dwg_point_3d *point, int *error)
{    
    if (minsert != 0)
        {
            *error = 0;
            point->x = minsert->extrusion.x;
            point->y = minsert->extrusion.y;
            point->z = minsert->extrusion.z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_minsert_set_extrusion(dwg_ent_minsert *minsert, dwg_point_3d *point, int *error)
{    
    if (minsert != 0)
        {
            *error = 0;
            minsert->extrusion.x = point->x;
            minsert->extrusion.y = point->y;
            minsert->extrusion.z = point->z;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_minsert_set_has_attribs(dwg_ent_minsert *minsert, char  attribs, int *error)
{    
    if (minsert != 0)
        {
            *error = 0;
            minsert->has_attribs = attribs;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_minsert_set_owned_obj_count(dwg_ent_minsert *minsert, long count, int *error)
{    
    if (minsert != 0)
        {
            *error = 0;
            minsert->owned_obj_count = count;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_minsert_set_col_spacing(dwg_ent_minsert *minsert, double spacing, int *error)
{    
    if (minsert != 0)
        {
            *error = 0;
            minsert->col_spacing = spacing;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_minsert_set_row_spacing(dwg_ent_minsert *minsert, double spacing, int *error)
{    
    if (minsert != 0)
        {
            *error = 0;
            minsert->row_spacing = spacing;
        }
    else
        {
            *error = 1;
        }
}

//--------------------------------------------------------------------------------

char *
dwg_obj_mlinstyle_get_name(dwg_obj_mlinestyle *mlinestyle, int *error)
{    
    if (mlinestyle != 0)
        {
            *error = 0;
            return mlinestyle->name;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_obj_mlinestyle_set_name(dwg_obj_mlinestyle *mlinestyle, char * name, int *error)
{    
    if (mlinestyle != 0)
        {
            *error = 0;
            mlinestyle->name = name;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_obj_mlinestyle_set_desc(dwg_obj_mlinestyle *mlinestyle, char * desc, int *error)
{    
    if (mlinestyle != 0)
        {
            *error = 0;
            mlinestyle->desc = desc;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_obj_mlinestyle_set_flags(dwg_obj_mlinestyle *mlinestyle, int flags, int *error)
{    
    if (mlinestyle != 0)
        {
            *error = 0;
            mlinestyle->flags = flags;
        }
    else
        {
            *error = 1;
        }
}

double dwg_obj_mlinestyle_get_start_angle(dwg_obj_mlinestyle *mlinestyle, int *error)
{    
    if (mlinestyle != 0)
        {
            *error = 0;
            return mlinestyle->startang;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_obj_mlinestyle_set_start_angle(dwg_obj_mlinestyle *mlinestyle, double startang, int *error)
{    
    if (mlinestyle != 0)
        {
            *error = 0;
            mlinestyle->startang = startang;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_obj_mlinestyle_set_end_angle(dwg_obj_mlinestyle *mlinestyle, double endang, int *error)
{    
    if (mlinestyle != 0)
        {
            *error = 0;
            mlinestyle->endang = endang;
        }
    else
        {
            *error = 1;
        }
}

char
dwg_obj_mlinestyle_get_linesinstyle(dwg_obj_mlinestyle *mlinestyle, int *error)
{    
    if (mlinestyle != 0)
        {
            *error = 0;
            return mlinestyle->linesinstyle;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_obj_mlinestyle_set_linesinstyle(dwg_obj_mlinestyle *mlinestyle, char linesinstyle, int *error)
{    
    if (mlinestyle != 0)
        {
            *error = 0;
            mlinestyle->linesinstyle = linesinstyle;
        }
    else
        {
            *error = 1;
        }
}

//--------------------------------------------------------------------------------

/* Returns 1st block header present in the dwg file from the Dwg_Data type 
argument passed to function.
Usage :- dwg_get_block_header(dwg);
*/
dwg_obj_block_header *
dwg_get_block_header(Dwg_Data *dwg, int *error)
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

//-------------------------------------------------------------------------------

int
dwg_obj_appid_control_get_num_entries(dwg_obj_appid_control *appid, int *error)
{
    if (appid != 0)
        {
            *error = 0;
            return appid->num_entries;
        }
    else
        {
            *error = 1;
        }

}

void
dwg_obj_appid_control_set_num_entries(dwg_obj_appid_control *appid, int entries, int *error)
{
    if (appid != 0)
        {
            *error = 0;
            appid->num_entries = entries;
        }
    else
        {
            *error = 1;
        }

}

//-------------------------------------------------------------------------------

char *
dwg_obj_appid_get_entry_name(dwg_obj_appid *appid, int *error)
{
    if (appid != 0)
        {
            *error = 0;
            return appid->entry_name;
        }
    else
        {
            *error = 1;
        }

}

void
dwg_obj_appid_set_entry_name(dwg_obj_appid *appid, char * entry_name, int *error)
{
    if (appid != 0)
        {
            *error = 0;
            appid->entry_name = entry_name;
        }
    else
        {
            *error = 1;
        }

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
        {
            *error = 1;
        }

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
        {
            *error = 1;
        }

}

dwg_obj_appid_control
dwg_obj_get_appid_control(dwg_obj_appid *appid, int *error)
{
    if (appid != 0)
        {
            *error = 0;
            appid->app_control->obj->tio.object->tio.APPID_CONTROL;
        }
    else
        {
            *error = 1;
        }

}

//-------------------------------------------------------------------------------
/*
dwg_ent_dimension_ordinate

#define DIMENSION_COMMON \
    BITCODE_3BD extrusion; \
    BITCODE_2RD text_midpt; \
    struct \
      { \
        BITCODE_BD ecs_11; \
        BITCODE_BD ecs_12; \
      } elevation; \
    BITCODE_RC flags_1; \
    BITCODE_TV user_text; \
    BITCODE_BD text_rot; \
    BITCODE_BD horiz_dir; \
    BITCODE_3BD ins_scale; \
    BITCODE_BD ins_rotation; \
    BITCODE_BS attachment_point; \
    BITCODE_BS lspace_style; \
    BITCODE_BD lspace_factor; \
    BITCODE_BD act_measurement; \
    BITCODE_B unknown; \
    BITCODE_B flip_arrow1; \
    BITCODE_B flip_arrow2;

/**
 Struct for ordinate dimension - DIMENSION_ORDINATE (20)

typedef struct _dwg_entity_DIMENSION_ORDINATE
{
  DIMENSION_COMMON
  BITCODE_2RD _12_pt;
  BITCODE_3BD _10_pt;
  BITCODE_3BD _13_pt;
  BITCODE_3BD _14_pt;
  BITCODE_RC flags_2;
  BITCODE_H dimstyle;
  BITCODE_H block;
} Dwg_Entity_DIMENSION_ORDINATE;
*/
//-------------------------------------------------------------------------------

void
dwg_ent_dim_ordinate_set_extrusion(dwg_ent_dim_ordinate *ord, dwg_point_3d *point, int *error)
{
    if (ord != 0)
        {
            *error = 0;
            ord->extrusion.x = point->x;
            ord->extrusion.y = point->y;
            ord->extrusion.z = point->z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ordinate_get_extrusion(dwg_ent_dim_ordinate *ord, dwg_point_3d *point, int *error)
{
    if (ord != 0)
        {
            *error = 0;
            point->x = ord->extrusion.x;
            point->y = ord->extrusion.y;
            point->z = ord->extrusion.z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ordinate_set_ins_scale(dwg_ent_dim_ordinate *ord, dwg_point_3d *point, int *error)
{
    if (ord != 0)
        {
            *error = 0;
            ord->ins_scale.x = point->x;
            ord->ins_scale.y = point->y;
            ord->ins_scale.z = point->z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ordinate_get_ins_scale(dwg_ent_dim_ordinate *ord, dwg_point_3d *point, int *error)
{
    if (ord != 0)
        {
            *error = 0;
            point->x = ord->ins_scale.x;
            point->y = ord->ins_scale.y;
            point->z = ord->ins_scale.z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ordinate_set_10_pt(dwg_ent_dim_ordinate *ord, dwg_point_3d *point, int *error)
{
    if (ord != 0)
        {
            *error = 0;
            ord->_10_pt.x = point->x;
            ord->_10_pt.y = point->y;
            ord->_10_pt.z = point->z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ordinate_get_10_pt(dwg_ent_dim_ordinate *ord, dwg_point_3d *point, int *error)
{
    if (ord != 0)
        {
            *error = 0;
            point->x = ord->_10_pt.x;
            point->y = ord->_10_pt.y;
            point->z = ord->_10_pt.z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ordinate_set_13_pt(dwg_ent_dim_ordinate *ord, dwg_point_3d *point, int *error)
{
    if (ord != 0)
        {
            *error = 0;
            ord->_13_pt.x = point->x;
            ord->_13_pt.y = point->y;
            ord->_13_pt.z = point->z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ordinate_get_13_pt(dwg_ent_dim_ordinate *ord, dwg_point_3d *point, int *error)
{
    if (ord != 0)
        {
            *error = 0;
            point->x = ord->_13_pt.x;
            point->y = ord->_13_pt.y;
            point->z = ord->_13_pt.z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ordinate_set_14_pt(dwg_ent_dim_ordinate *ord, dwg_point_3d *point, int *error)
{
    if (ord != 0)
        {
            *error = 0;
            ord->_14_pt.x = point->x;
            ord->_14_pt.y = point->y;
            ord->_14_pt.z = point->z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ordinate_get_14_pt(dwg_ent_dim_ordinate *ord, dwg_point_3d *point, int *error)
{
    if (ord != 0)
        {
            *error = 0;
            point->x = ord->_14_pt.x;
            point->y = ord->_14_pt.y;
            point->z = ord->_14_pt.z;
        }
    else
        {
            *error = 1;
        }
}

//-------------------------------------------------------------------------------

/* Returns block control object from a block header type argument passed 
to function
Usage :- 
dwg_obj_block_control *blc;
blc = dwg_block_header_get_block_control(hdr);
*/
dwg_obj_block_control *
dwg_block_header_get_block_control(dwg_obj_block_header* block_header, int *error)
{
    if (block_header != 0)
        {
            *error = 0;
            return block_header->block_control_handle->obj->tio.object->tio.BLOCK_CONTROL;
        }
    else
        {
            *error = 1;
        }

}

//-------------------------------------------------------------------------------

/* Get layer Name of the layer type argument passed in function
Usage :- char * layer_name = dwg_obj_layer_get_name(layer);
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
        {
            *error = 1;
        }

}

//-------------------------------------------------------------------------------

/* Get Block Name of the block header type argument passed in function
Usage :- char * block_name = dwg_obj_block_header_get_name(hdr);
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
        {
            *error = 1;
        }

}