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

//-------------------------------------------------------------------------------------------

char * dwg_ent_dim_linear_get_block_name(dwg_ent_dim_linear *dim, int *error)
{
    return dim->block->obj->tio.object->tio.BLOCK_HEADER->entry_name;
}

double
dwg_ent_dim_ordinate_get_elevation_ecs11(dwg_ent_dim_ordinate *dim, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            return dim->elevation.ecs_11;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ordinate_set_elevation_ecs11(dwg_ent_dim_ordinate *dim, double elevation_ecs11, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->elevation.ecs_11 = elevation_ecs11;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ordinate_set_elevation_ecs12(dwg_ent_dim_ordinate *dim, double elevation_ecs12, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->elevation.ecs_12 = elevation_ecs12;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ordinate_set_flags1(dwg_ent_dim_ordinate *dim, char flag, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->flags_1 = flag;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ordinate_set_act_measurement(dwg_ent_dim_ordinate *dim, double act_measurement, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->act_measurement = act_measurement;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ordinate_set_horiz_dir(dwg_ent_dim_ordinate *dim, double horiz_dir, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->horiz_dir = horiz_dir;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ordinate_set_lspace_factor(dwg_ent_dim_ordinate *dim, double factor, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->lspace_factor = factor;
        }
    else
        {
            *error = 1;
        }
}

unsigned int
dwg_ent_dim_ordinate_get_lspace_style(dwg_ent_dim_ordinate *dim, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            return dim->lspace_style;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ordinate_set_lspace_style(dwg_ent_dim_ordinate *dim, unsigned int style, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->lspace_style = style;
        }
    else
        {
            *error = 1;
        }
}

unsigned int
dwg_ent_dim_ordinate_get_attachment_point(dwg_ent_dim_ordinate *dim, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            return dim->attachment_point;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ordinate_set_attachment_point(dwg_ent_dim_ordinate *dim, unsigned int point, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->attachment_point = point;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ordinate_set_extrusion(dwg_ent_dim_ordinate *dim, dwg_point_3d *point, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->extrusion.x = point->x;
            dim->extrusion.y = point->y;
            dim->extrusion.z = point->z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ordinate_get_extrusion(dwg_ent_dim_ordinate *dim, dwg_point_3d *point, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            point->x = dim->extrusion.x;
            point->y = dim->extrusion.y;
            point->z = dim->extrusion.z;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ordinate_set_user_text(dwg_ent_dim_ordinate *dim, char * text, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->user_text = text;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ordinate_set_text_rot(dwg_ent_dim_ordinate *dim, double rot, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->text_rot = rot;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ordinate_set_ins_rotation(dwg_ent_dim_ordinate *dim, double rot, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->ins_rotation = rot;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ordinate_set_flip_arrow1(dwg_ent_dim_ordinate *dim, char flip_arrow, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->flip_arrow1 = flip_arrow;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ordinate_set_flip_arrow2(dwg_ent_dim_ordinate *dim, char flip_arrow, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->flip_arrow2 = flip_arrow;
        }
    else
        {
            *error = 1;
        }
}

void dwg_ent_dim_ordinate_set_text_mid_pt(dwg_ent_dim_ordinate *dim, dwg_point_2d *point, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->text_midpt.x = point->x;
            dim->text_midpt.y = point->y;
        }
    else
        {
            *error = 1;
        }
}

void dwg_ent_dim_ordinate_get_text_mid_pt(dwg_ent_dim_ordinate *dim, dwg_point_2d *point, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            point->x = dim->text_midpt.x;
            point->y = dim->text_midpt.y;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ordinate_set_ins_scale(dwg_ent_dim_ordinate *dim, dwg_point_3d *point, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->ins_scale.x = point->x;
            dim->ins_scale.y = point->y;
            dim->ins_scale.z = point->z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ordinate_get_ins_scale(dwg_ent_dim_ordinate *dim, dwg_point_3d *point, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            point->x = dim->ins_scale.x;
            point->y = dim->ins_scale.y;
            point->z = dim->ins_scale.z;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ordinate_set_flags2(dwg_ent_dim_ordinate *dim, char flag, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->flags_2 = flag;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ordinate_set_12_pt(dwg_ent_dim_ordinate *dim, dwg_point_2d *point, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->_12_pt.x = point->x;
            dim->_12_pt.y = point->y;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ordinate_get_12_pt(dwg_ent_dim_ordinate *dim, dwg_point_2d *point, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            point->x = dim->_12_pt.x;
            point->y = dim->_12_pt.y;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ordinate_set_10_pt(dwg_ent_dim_ordinate *dim, dwg_point_3d *point, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->_10_pt.x = point->x;
            dim->_10_pt.y = point->y;
            dim->_10_pt.z = point->z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ordinate_get_10_pt(dwg_ent_dim_ordinate *dim, dwg_point_3d *point, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            point->x = dim->_10_pt.x;
            point->y = dim->_10_pt.y;
            point->z = dim->_10_pt.z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ordinate_set_13_pt(dwg_ent_dim_ordinate *dim, dwg_point_3d *point, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->_13_pt.x = point->x;
            dim->_13_pt.y = point->y;
            dim->_13_pt.z = point->z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ordinate_get_13_pt(dwg_ent_dim_ordinate *dim, dwg_point_3d *point, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            point->x = dim->_13_pt.x;
            point->y = dim->_13_pt.y;
            point->z = dim->_13_pt.z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ordinate_set_14_pt(dwg_ent_dim_ordinate *dim, dwg_point_3d *point, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->_14_pt.x = point->x;
            dim->_14_pt.y = point->y;
            dim->_14_pt.z = point->z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ordinate_get_14_pt(dwg_ent_dim_ordinate *dim, dwg_point_3d *point, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            point->x = dim->_14_pt.x;
            point->y = dim->_14_pt.y;
            point->z = dim->_14_pt.z;
        }
    else
        {
            *error = 1;
        }
}

//-------------------------------------------------------------------------------------------

double
dwg_ent_dim_linear_get_elevation_ecs11(dwg_ent_dim_linear *dim, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            return dim->elevation.ecs_11;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_linear_set_elevation_ecs11(dwg_ent_dim_linear *dim, double elevation_ecs11, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->elevation.ecs_11 = elevation_ecs11;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_linear_set_elevation_ecs12(dwg_ent_dim_linear *dim, double elevation_ecs12, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->elevation.ecs_12 = elevation_ecs12;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_linear_set_act_measurement(dwg_ent_dim_linear *dim, double act_measurement, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->act_measurement = act_measurement;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_linear_set_horiz_dir(dwg_ent_dim_linear *dim, double horiz_dir, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->horiz_dir = horiz_dir;
        }
    else
        {
            *error = 1;
        }
}

double
dwg_ent_dim_linear_get_lspace_factor(dwg_ent_dim_linear *dim, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            return dim->lspace_factor;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_linear_set_lspace_factor(dwg_ent_dim_linear *dim, double factor, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->lspace_factor = factor;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_linear_set_lspace_style(dwg_ent_dim_linear *dim, unsigned int style, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->lspace_style = style;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_linear_set_attachment_point(dwg_ent_dim_linear *dim, unsigned int point, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->attachment_point = point;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_linear_set_extrusion(dwg_ent_dim_linear *dim, dwg_point_3d *point, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->extrusion.x = point->x;
            dim->extrusion.y = point->y;
            dim->extrusion.z = point->z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_linear_get_extrusion(dwg_ent_dim_linear *dim, dwg_point_3d *point, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            point->x = dim->extrusion.x;
            point->y = dim->extrusion.y;
            point->z = dim->extrusion.z;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_linear_set_user_text(dwg_ent_dim_linear *dim, char * text, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->user_text = text;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_linear_set_text_rot(dwg_ent_dim_linear *dim, double rot, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->text_rot = rot;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_linear_set_ins_rotation(dwg_ent_dim_linear *dim, double rot, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->ins_rotation = rot;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_linear_set_flip_arrow1(dwg_ent_dim_linear *dim, char flip_arrow, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->flip_arrow1 = flip_arrow;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_linear_set_flip_arrow2(dwg_ent_dim_linear *dim, char flip_arrow, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->flip_arrow2 = flip_arrow;
        }
    else
        {
            *error = 1;
        }
}

void dwg_ent_dim_linear_set_text_mid_pt(dwg_ent_dim_linear *dim, dwg_point_2d *point, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->text_midpt.x = point->x;
            dim->text_midpt.y = point->y;
        }
    else
        {
            *error = 1;
        }
}

void dwg_ent_dim_linear_get_text_mid_pt(dwg_ent_dim_linear *dim, dwg_point_2d *point, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            point->x = dim->text_midpt.x;
            point->y = dim->text_midpt.y;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_linear_set_ins_scale(dwg_ent_dim_linear *dim, dwg_point_3d *point, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->ins_scale.x = point->x;
            dim->ins_scale.y = point->y;
            dim->ins_scale.z = point->z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_linear_get_ins_scale(dwg_ent_dim_linear *dim, dwg_point_3d *point, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            point->x = dim->ins_scale.x;
            point->y = dim->ins_scale.y;
            point->z = dim->ins_scale.z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_linear_set_12_pt(dwg_ent_dim_linear *dim, dwg_point_2d *point, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->_12_pt.x = point->x;
            dim->_12_pt.y = point->y;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_linear_get_12_pt(dwg_ent_dim_linear *dim, dwg_point_2d *point, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            point->x = dim->_12_pt.x;
            point->y = dim->_12_pt.y;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_linear_set_10_pt(dwg_ent_dim_linear *dim, dwg_point_3d *point, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->_10_pt.x = point->x;
            dim->_10_pt.y = point->y;
            dim->_10_pt.z = point->z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_linear_get_10_pt(dwg_ent_dim_linear *dim, dwg_point_3d *point, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            point->x = dim->_10_pt.x;
            point->y = dim->_10_pt.y;
            point->z = dim->_10_pt.z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_linear_set_13_pt(dwg_ent_dim_linear *dim, dwg_point_3d *point, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->_13_pt.x = point->x;
            dim->_13_pt.y = point->y;
            dim->_13_pt.z = point->z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_linear_get_13_pt(dwg_ent_dim_linear *dim, dwg_point_3d *point, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            point->x = dim->_13_pt.x;
            point->y = dim->_13_pt.y;
            point->z = dim->_13_pt.z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_linear_set_14_pt(dwg_ent_dim_linear *dim, dwg_point_3d *point, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->_14_pt.x = point->x;
            dim->_14_pt.y = point->y;
            dim->_14_pt.z = point->z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_linear_get_14_pt(dwg_ent_dim_linear *dim, dwg_point_3d *point, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            point->x = dim->_14_pt.x;
            point->y = dim->_14_pt.y;
            point->z = dim->_14_pt.z;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_linear_set_ext_line_rotation(dwg_ent_dim_linear *dim, double rot, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->ext_line_rot = rot;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

//-------------------------------------------------------------------------------------------


double
dwg_ent_dim_aligned_get_elevation_ecs11(dwg_ent_dim_aligned *dim, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            return dim->elevation.ecs_11;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_aligned_set_elevation_ecs11(dwg_ent_dim_aligned *dim, double elevation_ecs11, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->elevation.ecs_11 = elevation_ecs11;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_aligned_set_elevation_ecs12(dwg_ent_dim_aligned *dim, double elevation_ecs12, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->elevation.ecs_12 = elevation_ecs12;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_aligned_set_act_measurement(dwg_ent_dim_aligned *dim, double act_measurement, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->act_measurement = act_measurement;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_aligned_set_horiz_dir(dwg_ent_dim_aligned *dim, double horiz_dir, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->horiz_dir = horiz_dir;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_aligned_set_lspace_factor(dwg_ent_dim_aligned *dim, double factor, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->lspace_factor = factor;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_aligned_set_lspace_style(dwg_ent_dim_aligned *dim, unsigned int style, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->lspace_style = style;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_aligned_set_attachment_point(dwg_ent_dim_aligned *dim, unsigned int point, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->attachment_point = point;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_aligned_set_extrusion(dwg_ent_dim_aligned *dim, dwg_point_3d *point, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->extrusion.x = point->x;
            dim->extrusion.y = point->y;
            dim->extrusion.z = point->z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_aligned_get_extrusion(dwg_ent_dim_aligned *dim, dwg_point_3d *point, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            point->x = dim->extrusion.x;
            point->y = dim->extrusion.y;
            point->z = dim->extrusion.z;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_aligned_set_user_text(dwg_ent_dim_aligned *dim, char * text, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->user_text = text;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_aligned_set_text_rot(dwg_ent_dim_aligned *dim, double rot, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->text_rot = rot;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_aligned_set_ins_rotation(dwg_ent_dim_aligned *dim, double rot, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->ins_rotation = rot;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_aligned_set_flip_arrow1(dwg_ent_dim_aligned *dim, char flip_arrow, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->flip_arrow1 = flip_arrow;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_aligned_set_flip_arrow2(dwg_ent_dim_aligned *dim, char flip_arrow, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->flip_arrow2 = flip_arrow;
        }
    else
        {
            *error = 1;
        }
}

void dwg_ent_dim_aligned_set_text_mid_pt(dwg_ent_dim_aligned *dim, dwg_point_2d *point, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->text_midpt.x = point->x;
            dim->text_midpt.y = point->y;
        }
    else
        {
            *error = 1;
        }
}

void dwg_ent_dim_aligned_get_text_mid_pt(dwg_ent_dim_aligned *dim, dwg_point_2d *point, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            point->x = dim->text_midpt.x;
            point->y = dim->text_midpt.y;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_aligned_set_ins_scale(dwg_ent_dim_aligned *dim, dwg_point_3d *point, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->ins_scale.x = point->x;
            dim->ins_scale.y = point->y;
            dim->ins_scale.z = point->z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_aligned_get_ins_scale(dwg_ent_dim_aligned *dim, dwg_point_3d *point, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            point->x = dim->ins_scale.x;
            point->y = dim->ins_scale.y;
            point->z = dim->ins_scale.z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_aligned_set_12_pt(dwg_ent_dim_aligned *dim, dwg_point_2d *point, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->_12_pt.x = point->x;
            dim->_12_pt.y = point->y;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_aligned_get_12_pt(dwg_ent_dim_aligned *dim, dwg_point_2d *point, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            point->x = dim->_12_pt.x;
            point->y = dim->_12_pt.y;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_aligned_set_10_pt(dwg_ent_dim_aligned *dim, dwg_point_3d *point, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->_10_pt.x = point->x;
            dim->_10_pt.y = point->y;
            dim->_10_pt.z = point->z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_aligned_get_10_pt(dwg_ent_dim_aligned *dim, dwg_point_3d *point, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            point->x = dim->_10_pt.x;
            point->y = dim->_10_pt.y;
            point->z = dim->_10_pt.z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_aligned_set_13_pt(dwg_ent_dim_aligned *dim, dwg_point_3d *point, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->_13_pt.x = point->x;
            dim->_13_pt.y = point->y;
            dim->_13_pt.z = point->z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_aligned_get_13_pt(dwg_ent_dim_aligned *dim, dwg_point_3d *point, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            point->x = dim->_13_pt.x;
            point->y = dim->_13_pt.y;
            point->z = dim->_13_pt.z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_aligned_set_14_pt(dwg_ent_dim_aligned *dim, dwg_point_3d *point, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->_14_pt.x = point->x;
            dim->_14_pt.y = point->y;
            dim->_14_pt.z = point->z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_aligned_get_14_pt(dwg_ent_dim_aligned *dim, dwg_point_3d *point, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            point->x = dim->_14_pt.x;
            point->y = dim->_14_pt.y;
            point->z = dim->_14_pt.z;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_aligned_set_ext_line_rotation(dwg_ent_dim_aligned *dim, double rot, int *error)
{
    if (dim != 0)
        {
            *error = 0;
            dim->ext_line_rot = rot;
        }
    else
        {
            *error = 1;
        }
}

//-------------------------------------------------------------------------------------------

double
dwg_ent_dim_ang3pt_get_elevation_ecs11(dwg_ent_dim_ang3pt *ang, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            return ang->elevation.ecs_11;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ang3pt_set_elevation_ecs11(dwg_ent_dim_ang3pt *ang, double elevation_ecs11, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            ang->elevation.ecs_11 = elevation_ecs11;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ang3pt_set_elevation_ecs12(dwg_ent_dim_ang3pt *ang, double elevation_ecs12, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            ang->elevation.ecs_12 = elevation_ecs12;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ang3pt_set_act_measurement(dwg_ent_dim_ang3pt *ang, double act_measurement, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            ang->act_measurement = act_measurement;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ang3pt_set_horiz_dir(dwg_ent_dim_ang3pt *ang, double horiz_dir, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            ang->horiz_dir = horiz_dir;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ang3pt_set_lspace_factor(dwg_ent_dim_ang3pt *ang, double factor, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            ang->lspace_factor = factor;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ang3pt_set_lspace_style(dwg_ent_dim_ang3pt *ang, unsigned int style, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            ang->lspace_style = style;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ang3pt_set_attachment_point(dwg_ent_dim_ang3pt *ang, unsigned int point, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            ang->attachment_point = point;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ang3pt_set_extrusion(dwg_ent_dim_ang3pt *ang, dwg_point_3d *point, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            ang->extrusion.x = point->x;
            ang->extrusion.y = point->y;
            ang->extrusion.z = point->z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ang3pt_get_extrusion(dwg_ent_dim_ang3pt *ang, dwg_point_3d *point, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            point->x = ang->extrusion.x;
            point->y = ang->extrusion.y;
            point->z = ang->extrusion.z;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ang3pt_set_user_text(dwg_ent_dim_ang3pt *ang, char * text, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            ang->user_text = text;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ang3pt_set_text_rot(dwg_ent_dim_ang3pt *ang, double rot, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            ang->text_rot = rot;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ang3pt_set_ins_rotation(dwg_ent_dim_ang3pt *ang, double rot, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            ang->ins_rotation = rot;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ang3pt_set_flip_arrow1(dwg_ent_dim_ang3pt *ang, char flip_arrow, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            ang->flip_arrow1 = flip_arrow;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ang3pt_set_flip_arrow2(dwg_ent_dim_ang3pt *ang, char flip_arrow, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            ang->flip_arrow2 = flip_arrow;
        }
    else
        {
            *error = 1;
        }
}

void dwg_ent_dim_ang3pt_set_text_mid_pt(dwg_ent_dim_ang3pt *ang, dwg_point_2d *point, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            ang->text_midpt.x = point->x;
            ang->text_midpt.y = point->y;
        }
    else
        {
            *error = 1;
        }
}

void dwg_ent_dim_ang3pt_get_text_mid_pt(dwg_ent_dim_ang3pt *ang, dwg_point_2d *point, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            point->x = ang->text_midpt.x;
            point->y = ang->text_midpt.y;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ang3pt_set_ins_scale(dwg_ent_dim_ang3pt *ang, dwg_point_3d *point, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            ang->ins_scale.x = point->x;
            ang->ins_scale.y = point->y;
            ang->ins_scale.z = point->z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ang3pt_get_ins_scale(dwg_ent_dim_ang3pt *ang, dwg_point_3d *point, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            point->x = ang->ins_scale.x;
            point->y = ang->ins_scale.y;
            point->z = ang->ins_scale.z;
        }
    else
        {
            *error = 1;
        }
}


void
dwg_ent_dim_ang3pt_set_12_pt(dwg_ent_dim_ang3pt *ang, dwg_point_2d *point, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            ang->_12_pt.x = point->x;
            ang->_12_pt.y = point->y;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ang3pt_get_12_pt(dwg_ent_dim_ang3pt *ang, dwg_point_2d *point, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            point->x = ang->_12_pt.x;
            point->y = ang->_12_pt.y;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ang3pt_set_10_pt(dwg_ent_dim_ang3pt *ang, dwg_point_3d *point, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            ang->_10_pt.x = point->x;
            ang->_10_pt.y = point->y;
            ang->_10_pt.z = point->z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ang3pt_get_10_pt(dwg_ent_dim_ang3pt *ang, dwg_point_3d *point, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            point->x = ang->_10_pt.x;
            point->y = ang->_10_pt.y;
            point->z = ang->_10_pt.z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ang3pt_set_13_pt(dwg_ent_dim_ang3pt *ang, dwg_point_3d *point, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            ang->_13_pt.x = point->x;
            ang->_13_pt.y = point->y;
            ang->_13_pt.z = point->z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ang3pt_get_13_pt(dwg_ent_dim_ang3pt *ang, dwg_point_3d *point, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            point->x = ang->_13_pt.x;
            point->y = ang->_13_pt.y;
            point->z = ang->_13_pt.z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ang3pt_set_14_pt(dwg_ent_dim_ang3pt *ang, dwg_point_3d *point, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            ang->_14_pt.x = point->x;
            ang->_14_pt.y = point->y;
            ang->_14_pt.z = point->z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ang3pt_get_14_pt(dwg_ent_dim_ang3pt *ang, dwg_point_3d *point, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            point->x = ang->_14_pt.x;
            point->y = ang->_14_pt.y;
            point->z = ang->_14_pt.z;
        }
    else
        {
            *error = 1;
        }
}


void
dwg_ent_dim_ang3pt_set_15_pt(dwg_ent_dim_ang3pt *ang, dwg_point_3d *point, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            ang->_15_pt.x = point->x;
            ang->_15_pt.y = point->y;
            ang->_15_pt.z = point->z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ang3pt_get_15_pt(dwg_ent_dim_ang3pt *ang, dwg_point_3d *point, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            point->x = ang->_15_pt.x;
            point->y = ang->_15_pt.y;
            point->z = ang->_15_pt.z;
        }
    else
        {
            *error = 1;
        }
}

//-------------------------------------------------------------------------------------------

double
dwg_ent_dim_ang2ln_get_elevation_ecs11(dwg_ent_dim_ang2ln *ang, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            return ang->elevation.ecs_11;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ang2ln_set_elevation_ecs11(dwg_ent_dim_ang2ln *ang, double elevation_ecs11, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            ang->elevation.ecs_11 = elevation_ecs11;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ang2ln_set_elevation_ecs12(dwg_ent_dim_ang2ln *ang, double elevation_ecs12, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            ang->elevation.ecs_12 = elevation_ecs12;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

double
dwg_ent_dim_ang2ln_get_act_measurement(dwg_ent_dim_ang2ln *ang, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            return ang->act_measurement;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ang2ln_set_act_measurement(dwg_ent_dim_ang2ln *ang, double act_measurement, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            ang->act_measurement = act_measurement;
        }
    else
        {
            *error = 1;
        }
}

double
dwg_ent_dim_ang2ln_get_horiz_dir(dwg_ent_dim_ang2ln *ang, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            return ang->horiz_dir;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ang2ln_set_horiz_dir(dwg_ent_dim_ang2ln *ang, double horiz_dir, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            ang->horiz_dir = horiz_dir;
        }
    else
        {
            *error = 1;
        }
}

double
dwg_ent_dim_ang2ln_get_lspace_factor(dwg_ent_dim_ang2ln *ang, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            return ang->lspace_factor;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ang2ln_set_lspace_factor(dwg_ent_dim_ang2ln *ang, double factor, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            ang->lspace_factor = factor;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ang2ln_set_lspace_style(dwg_ent_dim_ang2ln *ang, unsigned int style, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            ang->lspace_style = style;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ang2ln_set_attachment_point(dwg_ent_dim_ang2ln *ang, unsigned int point, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            ang->attachment_point = point;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ang2ln_set_extrusion(dwg_ent_dim_ang2ln *ang, dwg_point_3d *point, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            ang->extrusion.x = point->x;
            ang->extrusion.y = point->y;
            ang->extrusion.z = point->z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ang2ln_get_extrusion(dwg_ent_dim_ang2ln *ang, dwg_point_3d *point, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            point->x = ang->extrusion.x;
            point->y = ang->extrusion.y;
            point->z = ang->extrusion.z;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ang2ln_set_user_text(dwg_ent_dim_ang2ln *ang, char * text, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            ang->user_text = text;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ang2ln_set_text_rot(dwg_ent_dim_ang2ln *ang, double rot, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            ang->text_rot = rot;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ang2ln_set_ins_rotation(dwg_ent_dim_ang2ln *ang, double rot, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            ang->ins_rotation = rot;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ang2ln_set_flip_arrow1(dwg_ent_dim_ang2ln *ang, char flip_arrow, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            ang->flip_arrow1 = flip_arrow;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ang2ln_set_flip_arrow2(dwg_ent_dim_ang2ln *ang, char flip_arrow, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            ang->flip_arrow2 = flip_arrow;
        }
    else
        {
            *error = 1;
        }
}

void dwg_ent_dim_ang2ln_set_text_mid_pt(dwg_ent_dim_ang2ln *ang, dwg_point_2d *point, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            ang->text_midpt.x = point->x;
            ang->text_midpt.y = point->y;
        }
    else
        {
            *error = 1;
        }
}

void dwg_ent_dim_ang2ln_get_text_mid_pt(dwg_ent_dim_ang2ln *ang, dwg_point_2d *point, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            point->x = ang->text_midpt.x;
            point->y = ang->text_midpt.y;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ang2ln_set_ins_scale(dwg_ent_dim_ang2ln *ang, dwg_point_3d *point, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            ang->ins_scale.x = point->x;
            ang->ins_scale.y = point->y;
            ang->ins_scale.z = point->z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ang2ln_get_ins_scale(dwg_ent_dim_ang2ln *ang, dwg_point_3d *point, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            point->x = ang->ins_scale.x;
            point->y = ang->ins_scale.y;
            point->z = ang->ins_scale.z;
        }
    else
        {
            *error = 1;
        }
}


void
dwg_ent_dim_ang2ln_set_12_pt(dwg_ent_dim_ang2ln *ang, dwg_point_2d *point, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            ang->_12_pt.x = point->x;
            ang->_12_pt.y = point->y;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ang2ln_get_12_pt(dwg_ent_dim_ang2ln *ang, dwg_point_2d *point, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            point->x = ang->_12_pt.x;
            point->y = ang->_12_pt.y;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ang2ln_set_10_pt(dwg_ent_dim_ang2ln *ang, dwg_point_3d *point, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            ang->_10_pt.x = point->x;
            ang->_10_pt.y = point->y;
            ang->_10_pt.z = point->z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ang2ln_get_10_pt(dwg_ent_dim_ang2ln *ang, dwg_point_3d *point, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            point->x = ang->_10_pt.x;
            point->y = ang->_10_pt.y;
            point->z = ang->_10_pt.z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ang2ln_set_13_pt(dwg_ent_dim_ang2ln *ang, dwg_point_3d *point, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            ang->_13_pt.x = point->x;
            ang->_13_pt.y = point->y;
            ang->_13_pt.z = point->z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ang2ln_get_13_pt(dwg_ent_dim_ang2ln *ang, dwg_point_3d *point, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            point->x = ang->_13_pt.x;
            point->y = ang->_13_pt.y;
            point->z = ang->_13_pt.z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ang2ln_set_14_pt(dwg_ent_dim_ang2ln *ang, dwg_point_3d *point, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            ang->_14_pt.x = point->x;
            ang->_14_pt.y = point->y;
            ang->_14_pt.z = point->z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ang2ln_get_14_pt(dwg_ent_dim_ang2ln *ang, dwg_point_3d *point, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            point->x = ang->_14_pt.x;
            point->y = ang->_14_pt.y;
            point->z = ang->_14_pt.z;
        }
    else
        {
            *error = 1;
        }
}


void
dwg_ent_dim_ang2ln_set_15_pt(dwg_ent_dim_ang2ln *ang, dwg_point_3d *point, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            ang->_15_pt.x = point->x;
            ang->_15_pt.y = point->y;
            ang->_15_pt.z = point->z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ang2ln_get_15_pt(dwg_ent_dim_ang2ln *ang, dwg_point_3d *point, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            point->x = ang->_15_pt.x;
            point->y = ang->_15_pt.y;
            point->z = ang->_15_pt.z;
        }
    else
        {
            *error = 1;
        }
}


void
dwg_ent_dim_ang2ln_set_16_pt(dwg_ent_dim_ang2ln *ang, dwg_point_2d *point, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            ang->_16_pt.x = point->x;
            ang->_16_pt.y = point->y;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_ang2ln_get_16_pt(dwg_ent_dim_ang2ln *ang, dwg_point_2d *point, int *error)
{
    if (ang != 0)
        {
            *error = 0;
            point->x = ang->_16_pt.x;
            point->y = ang->_16_pt.y;
        }
    else
        {
            *error = 1;
        }
}

//-------------------------------------------------------------------------------------------

double
dwg_ent_dim_radius_get_elevation_ecs11(dwg_ent_dim_radius *radius, int *error)
{
    if (radius != 0)
        {
            *error = 0;
            return radius->elevation.ecs_11;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_radius_set_elevation_ecs11(dwg_ent_dim_radius *radius, double elevation_ecs11, int *error)
{
    if (radius != 0)
        {
            *error = 0;
            radius->elevation.ecs_11 = elevation_ecs11;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_radius_set_elevation_ecs12(dwg_ent_dim_radius *radius, double elevation_ecs12, int *error)
{
    if (radius != 0)
        {
            *error = 0;
            radius->elevation.ecs_12 = elevation_ecs12;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_radius_set_flags1(dwg_ent_dim_radius *radius, char flag, int *error)
{
    if (radius != 0)
        {
            *error = 0;
            radius->flags_1 = flag;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_radius_set_act_measurement(dwg_ent_dim_radius *radius, double act_measurement, int *error)
{
    if (radius != 0)
        {
            *error = 0;
            radius->act_measurement = act_measurement;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_radius_set_horiz_dir(dwg_ent_dim_radius *radius, double horiz_dir, int *error)
{
    if (radius != 0)
        {
            *error = 0;
            radius->horiz_dir = horiz_dir;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_radius_set_lspace_factor(dwg_ent_dim_radius *radius, double factor, int *error)
{
    if (radius != 0)
        {
            *error = 0;
            radius->lspace_factor = factor;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_radius_set_lspace_style(dwg_ent_dim_radius *radius, unsigned int style, int *error)
{
    if (radius != 0)
        {
            *error = 0;
            radius->lspace_style = style;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_radius_set_attachment_point(dwg_ent_dim_radius *radius, unsigned int point, int *error)
{
    if (radius != 0)
        {
            *error = 0;
            radius->attachment_point = point;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_radius_set_extrusion(dwg_ent_dim_radius *radius, dwg_point_3d *point, int *error)
{
    if (radius != 0)
        {
            *error = 0;
            radius->extrusion.x = point->x;
            radius->extrusion.y = point->y;
            radius->extrusion.z = point->z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_radius_get_extrusion(dwg_ent_dim_radius *radius, dwg_point_3d *point, int *error)
{
    if (radius != 0)
        {
            *error = 0;
            point->x = radius->extrusion.x;
            point->y = radius->extrusion.y;
            point->z = radius->extrusion.z;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_radius_set_user_text(dwg_ent_dim_radius *radius, char * text, int *error)
{
    if (radius != 0)
        {
            *error = 0;
            radius->user_text = text;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_radius_set_text_rot(dwg_ent_dim_radius *radius, double rot, int *error)
{
    if (radius != 0)
        {
            *error = 0;
            radius->text_rot = rot;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_radius_set_ins_rotation(dwg_ent_dim_radius *radius, double rot, int *error)
{
    if (radius != 0)
        {
            *error = 0;
            radius->ins_rotation = rot;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_radius_set_flip_arrow1(dwg_ent_dim_radius *radius, char flip_arrow, int *error)
{
    if (radius != 0)
        {
            *error = 0;
            radius->flip_arrow1 = flip_arrow;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_radius_set_flip_arrow2(dwg_ent_dim_radius *radius, char flip_arrow, int *error)
{
    if (radius != 0)
        {
            *error = 0;
            radius->flip_arrow2 = flip_arrow;
        }
    else
        {
            *error = 1;
        }
}

void dwg_ent_dim_radius_set_text_mid_pt(dwg_ent_dim_radius *radius, dwg_point_2d *point, int *error)
{
    if (radius != 0)
        {
            *error = 0;
            radius->text_midpt.x = point->x;
            radius->text_midpt.y = point->y;
        }
    else
        {
            *error = 1;
        }
}

void dwg_ent_dim_radius_get_text_mid_pt(dwg_ent_dim_radius *radius, dwg_point_2d *point, int *error)
{
    if (radius != 0)
        {
            *error = 0;
            point->x = radius->text_midpt.x;
            point->y = radius->text_midpt.y;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_radius_set_ins_scale(dwg_ent_dim_radius *radius, dwg_point_3d *point, int *error)
{
    if (radius != 0)
        {
            *error = 0;
            radius->ins_scale.x = point->x;
            radius->ins_scale.y = point->y;
            radius->ins_scale.z = point->z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_radius_get_ins_scale(dwg_ent_dim_radius *radius, dwg_point_3d *point, int *error)
{
    if (radius != 0)
        {
            *error = 0;
            point->x = radius->ins_scale.x;
            point->y = radius->ins_scale.y;
            point->z = radius->ins_scale.z;
        }
    else
        {
            *error = 1;
        }
}


void
dwg_ent_dim_radius_set_12_pt(dwg_ent_dim_radius *radius, dwg_point_2d *point, int *error)
{
    if (radius != 0)
        {
            *error = 0;
            radius->_12_pt.x = point->x;
            radius->_12_pt.y = point->y;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_radius_get_12_pt(dwg_ent_dim_radius *radius, dwg_point_2d *point, int *error)
{
    if (radius != 0)
        {
            *error = 0;
            point->x = radius->_12_pt.x;
            point->y = radius->_12_pt.y;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_radius_set_10_pt(dwg_ent_dim_radius *radius, dwg_point_3d *point, int *error)
{
    if (radius != 0)
        {
            *error = 0;
            radius->_10_pt.x = point->x;
            radius->_10_pt.y = point->y;
            radius->_10_pt.z = point->z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_radius_get_10_pt(dwg_ent_dim_radius *radius, dwg_point_3d *point, int *error)
{
    if (radius != 0)
        {
            *error = 0;
            point->x = radius->_10_pt.x;
            point->y = radius->_10_pt.y;
            point->z = radius->_10_pt.z;
        }
    else
        {
            *error = 1;
        }
}


void
dwg_ent_dim_radius_set_15_pt(dwg_ent_dim_radius *radius, dwg_point_3d *point, int *error)
{
    if (radius != 0)
        {
            *error = 0;
            radius->_15_pt.x = point->x;
            radius->_15_pt.y = point->y;
            radius->_15_pt.z = point->z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_radius_get_15_pt(dwg_ent_dim_radius *radius, dwg_point_3d *point, int *error)
{
    if (radius != 0)
        {
            *error = 0;
            point->x = radius->_15_pt.x;
            point->y = radius->_15_pt.y;
            point->z = radius->_15_pt.z;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_radius_set_leader_length(dwg_ent_dim_radius *radius, double length, int *error)
{
    if (radius != 0)
        {
            *error = 0;
            radius->leader_len = length;
        }
    else
        {
            *error = 1;
        }
}

//-------------------------------------------------------------------------------------------

double
dwg_ent_dim_diameter_get_elevation_ecs11(dwg_ent_dim_diameter *dia, int *error)
{
    if (dia != 0)
        {
            *error = 0;
            return dia->elevation.ecs_11;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_diameter_set_elevation_ecs11(dwg_ent_dim_diameter *dia, double elevation_ecs11, int *error)
{
    if (dia != 0)
        {
            *error = 0;
            dia->elevation.ecs_11 = elevation_ecs11;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_diameter_set_elevation_ecs12(dwg_ent_dim_diameter *dia, double elevation_ecs12, int *error)
{
    if (dia != 0)
        {
            *error = 0;
            dia->elevation.ecs_12 = elevation_ecs12;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_diameter_set_flags1(dwg_ent_dim_diameter *dia, char flag, int *error)
{
    if (dia != 0)
        {
            *error = 0;
            dia->flags_1 = flag;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_diameter_set_act_measurement(dwg_ent_dim_diameter *dia, double act_measurement, int *error)
{
    if (dia != 0)
        {
            *error = 0;
            dia->act_measurement = act_measurement;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_diameter_set_horiz_dir(dwg_ent_dim_diameter *dia, double horiz_dir, int *error)
{
    if (dia != 0)
        {
            *error = 0;
            dia->horiz_dir = horiz_dir;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_diameter_set_lspace_factor(dwg_ent_dim_diameter *dia, double factor, int *error)
{
    if (dia != 0)
        {
            *error = 0;
            dia->lspace_factor = factor;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_diameter_set_lspace_style(dwg_ent_dim_diameter *dia, unsigned int style, int *error)
{
    if (dia != 0)
        {
            *error = 0;
            dia->lspace_style = style;
        }
    else
        {
            *error = 1;
        }
}

unsigned int
dwg_ent_dim_diameter_get_attachment_point(dwg_ent_dim_diameter *dia, int *error)
{
    if (dia != 0)
        {
            *error = 0;
            return dia->attachment_point;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_diameter_set_attachment_point(dwg_ent_dim_diameter *dia, unsigned int point, int *error)
{
    if (dia != 0)
        {
            *error = 0;
            dia->attachment_point = point;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_diameter_set_extrusion(dwg_ent_dim_diameter *dia, dwg_point_3d *point, int *error)
{
    if (dia != 0)
        {
            *error = 0;
            dia->extrusion.x = point->x;
            dia->extrusion.y = point->y;
            dia->extrusion.z = point->z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_diameter_get_extrusion(dwg_ent_dim_diameter *dia, dwg_point_3d *point, int *error)
{
    if (dia != 0)
        {
            *error = 0;
            point->x = dia->extrusion.x;
            point->y = dia->extrusion.y;
            point->z = dia->extrusion.z;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_diameter_set_user_text(dwg_ent_dim_diameter *dia, char * text, int *error)
{
    if (dia != 0)
        {
            *error = 0;
            dia->user_text = text;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_diameter_set_text_rot(dwg_ent_dim_diameter *dia, double rot, int *error)
{
    if (dia != 0)
        {
            *error = 0;
            dia->text_rot = rot;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_diameter_set_ins_rotation(dwg_ent_dim_diameter *dia, double rot, int *error)
{
    if (dia != 0)
        {
            *error = 0;
            dia->ins_rotation = rot;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_diameter_set_flip_arrow1(dwg_ent_dim_diameter *dia, char flip_arrow, int *error)
{
    if (dia != 0)
        {
            *error = 0;
            dia->flip_arrow1 = flip_arrow;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_diameter_set_flip_arrow2(dwg_ent_dim_diameter *dia, char flip_arrow, int *error)
{
    if (dia != 0)
        {
            *error = 0;
            dia->flip_arrow2 = flip_arrow;
        }
    else
        {
            *error = 1;
        }
}

void dwg_ent_dim_diameter_set_text_mid_pt(dwg_ent_dim_diameter *dia, dwg_point_2d *point, int *error)
{
    if (dia != 0)
        {
            *error = 0;
            dia->text_midpt.x = point->x;
            dia->text_midpt.y = point->y;
        }
    else
        {
            *error = 1;
        }
}

void dwg_ent_dim_diameter_get_text_mid_pt(dwg_ent_dim_diameter *dia, dwg_point_2d *point, int *error)
{
    if (dia != 0)
        {
            *error = 0;
            point->x = dia->text_midpt.x;
            point->y = dia->text_midpt.y;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_diameter_set_ins_scale(dwg_ent_dim_diameter *dia, dwg_point_3d *point, int *error)
{
    if (dia != 0)
        {
            *error = 0;
            dia->ins_scale.x = point->x;
            dia->ins_scale.y = point->y;
            dia->ins_scale.z = point->z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_diameter_get_ins_scale(dwg_ent_dim_diameter *dia, dwg_point_3d *point, int *error)
{
    if (dia != 0)
        {
            *error = 0;
            point->x = dia->ins_scale.x;
            point->y = dia->ins_scale.y;
            point->z = dia->ins_scale.z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_diameter_set_12_pt(dwg_ent_dim_diameter *dia, dwg_point_2d *point, int *error)
{
    if (dia != 0)
        {
            *error = 0;
            dia->_12_pt.x = point->x;
            dia->_12_pt.y = point->y;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_diameter_get_12_pt(dwg_ent_dim_diameter *dia, dwg_point_2d *point, int *error)
{
    if (dia != 0)
        {
            *error = 0;
            point->x = dia->_12_pt.x;
            point->y = dia->_12_pt.y;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_diameter_set_10_pt(dwg_ent_dim_diameter *dia, dwg_point_3d *point, int *error)
{
    if (dia != 0)
        {
            *error = 0;
            dia->_10_pt.x = point->x;
            dia->_10_pt.y = point->y;
            dia->_10_pt.z = point->z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_diameter_get_10_pt(dwg_ent_dim_diameter *dia, dwg_point_3d *point, int *error)
{
    if (dia != 0)
        {
            *error = 0;
            point->x = dia->_10_pt.x;
            point->y = dia->_10_pt.y;
            point->z = dia->_10_pt.z;
        }
    else
        {
            *error = 1;
        }
}


void
dwg_ent_dim_diameter_set_15_pt(dwg_ent_dim_diameter *dia, dwg_point_3d *point, int *error)
{
    if (dia != 0)
        {
            *error = 0;
            dia->_15_pt.x = point->x;
            dia->_15_pt.y = point->y;
            dia->_15_pt.z = point->z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_dim_diameter_get_15_pt(dwg_ent_dim_diameter *dia, dwg_point_3d *point, int *error)
{
    if (dia != 0)
        {
            *error = 0;
            point->x = dia->_15_pt.x;
            point->y = dia->_15_pt.y;
            point->z = dia->_15_pt.z;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_dim_diameter_set_leader_length(dwg_ent_dim_diameter *dia, double length, int *error)
{
    if (dia != 0)
        {
            *error = 0;
            dia->leader_len = length;
        }
    else
        {
            *error = 1;
        }
}
//-------------------------------------------------------------------------------

char
dwg_ent_endblk_get_dummy(dwg_ent_endblk *endblk, int *error)
{
    if(endblk != 0)
        {
            *error = 0;
            return endblk->dummy;
        }
    else
        {
            *error = 1;
        }   
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
        {
            *error = 1;
        }   
}

//--------------------------------------------------------------------------

char
dwg_ent_seqend_get_dummy(dwg_ent_seqend *seqend, int *error)
{
    if(seqend != 0)
        {
            *error = 0;
            return seqend->dummy;
        }
    else
        {
            *error = 1;
        }   
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
        {
            *error = 1;
        }   
}

//--------------------------------------------------------------------------

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
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_shape_set_width_factor(dwg_ent_shape *shape, double width_factor, int *error)
{
    if(shape != 0)
        {
            *error = 0;
            shape->width_factor = width_factor;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_shape_set_thickness(dwg_ent_shape *shape, double thickness, int *error)
{
    if(shape != 0)
        {
            *error = 0;
            shape->thickness = thickness;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_shape_get_extrusion(dwg_ent_shape *shape, dwg_point_3d *point, int *error)
{
    if(shape != 0)
        {
            *error = 0;
            point->x = shape->extrusion.x;
            point->y = shape->extrusion.y;
            point->z = shape->extrusion.z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_shape_set_extrusion(dwg_ent_shape *shape, dwg_point_3d *point, int *error)
{
    if(shape != 0)
        {
            *error = 0;
            shape->extrusion.x = point->x;
            shape->extrusion.y = point->y;
            shape->extrusion.z = point->z;
        }
    else
        {
            *error = 1;
        }
}


//-------------------------------------------------------------------------------

void
dwg_ent_mtext_set_insertion_pt(dwg_ent_mtext *mtext, dwg_point_3d *point, int *error)
{
    if(mtext != 0)
        {
            *error = 0;
            mtext->insertion_pt.x = point->x;
            mtext->insertion_pt.y = point->y;
            mtext->insertion_pt.z = point->z;
        }
    else
        {
            *error = 1;
        }
}
void
dwg_ent_mtext_get_insertion_pt(dwg_ent_mtext *mtext, dwg_point_3d *point, int *error)
{
    if(mtext != 0)
        {
            *error = 0;
            point->x = mtext->insertion_pt.x;
            point->y = mtext->insertion_pt.y;
            point->z = mtext->insertion_pt.z;
        }
    else
        {
            *error = 1;
        }

}

void
dwg_ent_mtext_set_extrusion(dwg_ent_mtext *mtext, dwg_point_3d *point, int *error)
{
    if(mtext != 0)
        {           
            *error = 0;
            mtext->extrusion.x = point->x;
            mtext->extrusion.y = point->y;
            mtext->extrusion.z = point->z;
        }
    else
        {
            *error = 1;
        }

}
void
dwg_ent_mtext_get_extrusion(dwg_ent_mtext *mtext, dwg_point_3d *point, int *error)
{
    if(mtext != 0)
        {
            *error = 0;
            point->x = mtext->extrusion.x;
            point->y = mtext->extrusion.y;
            point->z = mtext->extrusion.z;
        }
    else
        {
            *error = 1;
        }

}

void
dwg_ent_mtext_set_x_axis_dir(dwg_ent_mtext *mtext, dwg_point_3d *point, int *error)
{
    if(mtext != 0)
        {
            *error = 0;
            mtext->x_axis_dir.x = point->x;
            mtext->x_axis_dir.y = point->y;
            mtext->x_axis_dir.z = point->z;
        }
    else
        {
            *error = 1;
        }

}
void
dwg_ent_mtext_get_x_axis_dir(dwg_ent_mtext *mtext, dwg_point_3d *point, int *error)
{
    if(mtext != 0)
        {
            *error = 0;
            point->x = mtext->x_axis_dir.x;
            point->y = mtext->x_axis_dir.y;
            point->z = mtext->x_axis_dir.z;
        }
    else
        {
            *error = 1;
        }

}

void
dwg_ent_mtext_set_rect_height(dwg_ent_mtext *mtext, double rect_height, int *error)
{
    if(mtext != 0)
        {
            *error = 0;
            mtext->rect_height = rect_height;
        }
    else
        {
            *error = 1;
        }

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
        {
            *error = 1;
        }

}

void
dwg_ent_mtext_set_rect_width(dwg_ent_mtext *mtext, double rect_width, int *error)
{
    if(mtext != 0)
        {
            *error = 0;
            mtext->rect_width = rect_width;
        }
    else
        {
            *error = 1;
        }

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
        {
            *error = 1;
        }

}

void
dwg_ent_mtext_set_text_height(dwg_ent_mtext *mtext, double text_height, int *error)
{
    if(mtext != 0)
        {
            *error = 0;
            mtext->text_height = text_height;
        }
    else
        {
            *error = 1;
        }

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
        {
            *error = 1;
        }

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
        {
            *error = 1;
        }

}
void
dwg_ent_mtext_set_attachment(dwg_ent_mtext *mtext, unsigned int attachment, int *error)
{
    if(mtext != 0)
        {
            *error = 0;
            mtext->attachment = attachment;
        }
    else
        {
            *error = 1;
        }

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
        {
            *error = 1;
        }

}
void
dwg_ent_mtext_set_drawing_dir(dwg_ent_mtext *mtext, unsigned int dir, int *error)
{
    if(mtext != 0)
        {
            *error = 0;
            mtext->drawing_dir = dir;
        }
    else
        {
            *error = 1;
        }

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
        {
            *error = 1;
        }

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
        {
            *error = 1;
        }

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
        {
            *error = 1;
        }

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
        {
            *error = 1;
        }

}

char* dwg_ent_mtext_get_text(dwg_ent_mtext *mtext, int *error)
{
    if(mtext != 0)
        {
            *error = 0;
            return mtext->text;
        }
    else
        {
            *error = 1;
        }

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
        {
            *error = 1;
        }

}

unsigned int
dwg_ent_mtext_linespace_style(dwg_ent_mtext *mtext, int *error)
{
    if(mtext != 0)
        {
            *error = 0;
            return mtext->linespace_style;
        }
    else
        {
            *error = 1;
        }

}
void
dwg_ent_mtext_set_linespace_style(dwg_ent_mtext *mtext, unsigned int style, int *error)
{
    if(mtext != 0)
        {
            *error = 0;
            mtext->linespace_style = style;
        }
    else
        {
            *error = 1;
        }

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
        {
            *error = 1;
        }

}
void
dwg_ent_mtext_set_linespace_factor(dwg_ent_mtext *mtext, double factor, int *error)
{
    if(mtext != 0)
        {
            *error = 0;
            mtext->linespace_factor = factor;
        }
    else
        {
            *error = 1;
        }

}

//-------------------------------------------------------------------------------

void
dwg_ent_leader_set_annot_type(dwg_ent_leader *leader, unsigned int type, int *error)
{
    if(leader != 0)
        {
            *error = 0;
            leader->annot_type = type;
        }
    else
        {
            *error = 1;
        }

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
        {
            *error = 1;
        }

}

void
dwg_ent_leader_set_path_type(dwg_ent_leader *leader, unsigned int type, int *error)
{
    if(leader != 0)
        {
            *error = 0;
            leader->path_type = type;
        }
    else
        {
            *error = 1;
        }

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
        {
            *error = 1;
        }

}

long dwg_ent_leader_get_numpts(dwg_ent_leader *leader, int *error)
{
    if(leader != 0)
        {
            *error = 0;
            return leader->numpts;
        }
    else
        {
            *error = 1;
        }

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
        {
            *error = 1;
        }

}

void
dwg_ent_leader_set_end_pt_proj(dwg_ent_leader *leader, dwg_point_3d *point, int *error)
{
    if(leader != 0)
        {
            *error = 0;
            leader->end_pt_proj.x = point->x;
            leader->end_pt_proj.y = point->y;
            leader->end_pt_proj.z = point->z;
        }
    else
        {
            *error = 1;
        }

}
void
dwg_ent_leader_get_end_pt_proj(dwg_ent_leader *leader, dwg_point_3d *point, int *error)
{
    if(leader != 0)
        {
            *error = 0;
            point->x = leader->end_pt_proj.x;
            point->y = leader->end_pt_proj.y;
            point->z = leader->end_pt_proj.z;
        }
    else
        {
            *error = 1;
        }

}

void
dwg_ent_leader_set_extrusion(dwg_ent_leader *leader, dwg_point_3d *point, int *error)
{
    if(leader != 0)
        {
            *error = 0;
            leader->extrusion.x = point->x;
            leader->extrusion.y = point->y;
            leader->extrusion.z = point->z;
        }
    else
        {
            *error = 1;
        }

}
void
dwg_ent_leader_get_extrusion(dwg_ent_leader *leader, dwg_point_3d *point, int *error)
{
    if(leader != 0)
        {
            *error = 0;
            point->x = leader->extrusion.x;
            point->y = leader->extrusion.y;
            point->z = leader->extrusion.z;
        }
    else
        {
            *error = 1;
        }

}

void
dwg_ent_leader_set_x_direction(dwg_ent_leader *leader, dwg_point_3d *point, int *error)
{
    if(leader != 0)
        {
            *error = 0;
            leader->x_direction.x = point->x;
            leader->x_direction.y = point->y;
            leader->x_direction.z = point->z;
        }
    else
        {
            *error = 1;
        }

}
void
dwg_ent_leader_get_x_direction(dwg_ent_leader *leader, dwg_point_3d *point, int *error)
{
    if(leader != 0)
        {
            *error = 0;
            point->x = leader->x_direction.x;
            point->y = leader->x_direction.y;
            point->z = leader->x_direction.z;
        }
    else
        {
            *error = 1;
        }

}

void
dwg_ent_leader_set_offset_to_block_ins_pt(dwg_ent_leader *leader, dwg_point_3d *point, int *error)
{
    if(leader != 0)
        {
            *error = 0;
            leader->offset_to_block_ins_pt.x = point->x;
            leader->offset_to_block_ins_pt.y = point->y;
            leader->offset_to_block_ins_pt.z = point->z;
        }
    else
        {
            *error = 1;
        }

}
void
dwg_ent_leader_get_offset_to_block_ins_pt(dwg_ent_leader *leader, dwg_point_3d *point, int *error)
{
    if(leader != 0)
        {
            *error = 0;
            point->x = leader->offset_to_block_ins_pt.x;
            point->y = leader->offset_to_block_ins_pt.y;
            point->z = leader->offset_to_block_ins_pt.z;
        }
    else
        {
            *error = 1;
        }

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
        {
            *error = 1;
        }

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
        {
            *error = 1;
        }

}

void
dwg_ent_leader_set_box_height(dwg_ent_leader *leader, double height, int *error)
{
    if(leader != 0)
        {
            *error = 0;
            leader->box_height = height;
        }
    else
        {
            *error = 1;
        }

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
        {
            *error = 1;
        }

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
        {
            *error = 1;
        }

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
        {
            *error = 1;
        }

}

void
dwg_ent_leader_set_hook_line_on_x_dir(dwg_ent_leader *leader, char hook, int *error)
{
    if(leader != 0)
        {
            *error = 0;
            leader->hooklineonxdir = hook;
        }
    else
        {
            *error = 1;
        }

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
        {
            *error = 1;
        }

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
        {
            *error = 1;
        }

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
        {
            *error = 1;
        }

}

void
dwg_ent_leader_set_arrowhead_type(dwg_ent_leader *leader, unsigned int type, int *error)
{
    if(leader != 0)
        {
            leader->arrowhead_type = type;
            *error = 0;
        }
    else
        {
            *error = 1;
        }

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
        {
            *error = 1;
        }

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
        {
            *error = 1;
        }

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
        {
            *error = 1;
        }

}

void
dwg_ent_leader_set_byblock_color(dwg_ent_leader *leader, unsigned int color, int *error)
{
    if(leader != 0)
        {
            *error = 0;
            leader->byblock_color = color;
        }
    else
        {
            *error = 1;
        }

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
        {
            *error = 1;
        }

}

//-------------------------------------------------------------------------------

void
dwg_ent_tolerance_set_height(dwg_ent_tolerance *tol, double height, int *error)
{
    if(tol != 0)
        {
            *error = 0;
            tol->height = height;
        }
    else
        {
            *error = 1;
        }

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
        {
            *error = 1;
        }

}

void
dwg_ent_tolerance_set_dimgap(dwg_ent_tolerance *tol, double dimgap, int *error)
{
    if(tol != 0)
        {
            *error = 0;
            tol->dimgap = dimgap;
        }
    else
        {
            *error = 1;
        }

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
        {
            *error = 1;
        }

}

void
dwg_ent_tolerance_set_ins_pt(dwg_ent_tolerance *tol, dwg_point_3d *point, int *error)
{
    if(tol != 0)
        {
            *error = 0;
            tol->ins_pt.x = point->x;
            tol->ins_pt.y = point->y;
            tol->ins_pt.z = point->z;
        }
    else
        {
            *error = 1;
        }

}
void
dwg_ent_tolerance_get_ins_pt(dwg_ent_tolerance *tol, dwg_point_3d *point, int *error)
{
    if(tol != 0)
        {
            *error = 0;
            point->x = tol->ins_pt.x;
            point->y = tol->ins_pt.y;
            point->z = tol->ins_pt.z;
        }
    else
        {
            *error = 1;
        }

}

void
dwg_ent_tolerance_set_x_direction(dwg_ent_tolerance *tol, dwg_point_3d *point, int *error)
{
    if(tol != 0)
        {
            *error = 0;
            tol->x_direction.x = point->x;
            tol->x_direction.y = point->y;
            tol->x_direction.z = point->z;
        }
    else
        {
            *error = 1;
        }

}
void
dwg_ent_tolerance_get_x_direction(dwg_ent_tolerance *tol, dwg_point_3d *point, int *error)
{
    if(tol != 0)
        {
            *error = 0;
            point->x = tol->x_direction.x;
            point->y = tol->x_direction.y;
            point->z = tol->x_direction.z;
        }
    else
        {
            *error = 1;
        }

}

void
dwg_ent_tolerance_set_extrusion(dwg_ent_tolerance *tol, dwg_point_3d *point, int *error)
{
    if(tol != 0)
        {
            *error = 0;
            tol->extrusion.x = point->x;
            tol->extrusion.y = point->y;
            tol->extrusion.z = point->z;
        }
    else
        {
            *error = 1;
        }

}
void
dwg_ent_tolerance_get_extrusion(dwg_ent_tolerance *tol, dwg_point_3d *point, int *error)
{
    if(tol != 0)
        {
            *error = 0;
            point->x = tol->extrusion.x;
            point->y = tol->extrusion.y;
            point->z = tol->extrusion.z;
        }
    else
        {
            *error = 1;
        }

}

void
dwg_ent_tolerance_set_text_string(dwg_ent_tolerance *tol, char * string, int *error)
{
    if(tol != 0)
        {
            *error = 0;
            tol->text_string = string;
        }
    else
        {
            *error = 1;
        }

}
char * dwg_ent_tolerance_get_text_string(dwg_ent_tolerance *tol, int *error)
{
    if(tol != 0)
        {
            *error = 0;
            return tol->text_string;
        }
    else
        {
            *error = 1;
        }

}

//-------------------------------------------------------------------------------

char dwg_ent_lwpline_get_flags(dwg_ent_lwpline *lwpline, int *error)
{
  if(lwpline != 0)
    {
      *error = 0;  
      return lwpline->flags;
    }
  else
    {
      *error = 1;
    }
}
void dwg_ent_lwpline_set_flags(dwg_ent_lwpline *lwpline, char flags, int *error)
{
  if(lwpline != 0)
    {
      *error = 0;
      lwpline->flags = flags;
    }
  else
    {
      *error = 1;
    }
}

double dwg_ent_lwpline_get_const_width(dwg_ent_lwpline *lwpline, int *error)
{
  if(lwpline != 0)
    {
      *error = 0;
      return lwpline->const_width;
    }
  else
    {
      *error = 1;
    }
}
void dwg_ent_lwpline_set_const_width(dwg_ent_lwpline *lwpline, double const_width, int *error)
{
  if(lwpline != 0)
    {
      *error = 0;
      lwpline->const_width = const_width;    
    }
  else
    {
      *error = 1;
    }
}

double dwg_ent_lwpline_get_elevation(dwg_ent_lwpline *lwpline, int *error)
{
  if(lwpline != 0)
    {
      *error = 0;
      return lwpline->elevation;
    }
  else
    {
      *error = 1;
    }
}
void dwg_ent_lwpline_set_elevation(dwg_ent_lwpline *lwpline, double elevation, int *error)
{
  if(lwpline != 0)
    {
      *error = 0;
      lwpline->elevation = elevation;
    }
  else
    {
      *error = 1;
    }
}

double dwg_ent_lwpline_get_thickness(dwg_ent_lwpline *lwpline, int *error)
{
  if(lwpline != 0)
    {
      *error = 0;
      return lwpline->thickness;
    }
  else
    {
      *error = 1;
    }
}
void dwg_ent_lwpline_set_thickness(dwg_ent_lwpline *lwpline, double thickness, int *error)
{
  if(lwpline != 0)
    {
      *error = 0;
      lwpline->thickness = thickness;
    }
  else
    {
      *error = 1;
    }
}

long dwg_ent_lwpline_get_num_points(dwg_ent_lwpline *lwpline, int *error)
{
  if(lwpline != 0)
    {
      *error = 0;
      return lwpline->num_points;
    }
  else
    {
      *error = 1;
    }
}
void dwg_ent_lwpline_set_num_points(dwg_ent_lwpline *lwpline, long num_points, int *error)
{
  if(lwpline != 0)
    {
      *error = 0;
      lwpline->num_points = num_points;
    }
  else
    {
      *error = 1;
    }
}

long dwg_ent_lwpline_get_num_bulges(dwg_ent_lwpline *lwpline, int *error)
{
  if(lwpline != 0)
    {
      *error = 0;
      return lwpline->num_bulges;
    }
  else
    {
      *error = 1;
    }
}
void dwg_ent_lwpline_set_num_bulges(dwg_ent_lwpline *lwpline, long num_bulges, int *error)
{
  if(lwpline != 0)
    {
      *error = 0;
      lwpline->num_bulges = num_bulges;
    }
  else
    {
      *error = 1;
    }
}

long dwg_ent_lwpline_get_num_widths(dwg_ent_lwpline *lwpline, int *error)
{
  if(lwpline != 0)
    {
      *error = 0;
      return lwpline->num_widths;
    }
  else
    {
      *error = 1;
    }
}
void dwg_ent_lwpline_set_num_widths(dwg_ent_lwpline *lwpline, long num_widths, int *error)
{
  if(lwpline != 0)
    {
      *error = 0;
      lwpline->num_widths = num_widths;
    }
  else
    {
      *error = 1;
    }
}

void dwg_ent_lwpline_get_normal(dwg_ent_lwpline *lwpline, dwg_point_3d *points, int *error)
{
  if(lwpline != 0)
    {
      *error = 0;
      points->x = lwpline->normal.x;
      points->y = lwpline->normal.y;
      points->z = lwpline->normal.z;
    }
  else
    {
      *error = 1;
    }
}
void dwg_ent_lwpline_set_normal(dwg_ent_lwpline *lwpline, dwg_point_3d *points, int *error)
{
  if(lwpline != 0)
    {
      *error = 0;
      lwpline->normal.x = points->x;
      lwpline->normal.y = points->y;
      lwpline->normal.z = points->z;
    }
  else
    {
      *error = 1;
    }
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
    {
      *error = 1;
    }
}

dwg_point_2d *
dwg_ent_lwpline_get_points(dwg_ent_lwpline *lwpline, int *error)
{
  dwg_point_2d *ptx = (dwg_point_2d*) malloc(sizeof(dwg_point_2d)* lwpline->num_points);
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
    {
      *error = 1;
    }
}

dwg_lwpline_widths *
dwg_ent_lwpline_get_widths(dwg_ent_lwpline *lwpline, int *error)
{
  dwg_lwpline_widths *ptx = (dwg_lwpline_widths*) malloc(sizeof(dwg_lwpline_widths)* lwpline->num_widths);
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
    {
      *error = 1;
    }
}

//-------------------------------------------------------------------------------

unsigned int
dwg_ent_ole2frame_get_flags(dwg_ent_ole2frame *frame, int *error)
  {
    return frame->flags;
  }
void
dwg_ent_ole2frame_set_flags(dwg_ent_ole2frame *frame, unsigned int flags, int *error)
  {
    frame->flags = flags;
  }
unsigned int
dwg_ent_ole2frame_get_mode(dwg_ent_ole2frame *frame, int *error)
  {
    return frame->mode;
  }
void
dwg_ent_ole2frame_set_mode(dwg_ent_ole2frame *frame, unsigned int mode, int *error)
  {
    frame->mode = mode;
  }
long
dwg_ent_ole2frame_get_data_length(dwg_ent_ole2frame *frame, int *error)
  {
    return frame->data_length;
  }
void
dwg_ent_ole2frame_set_data_length(dwg_ent_ole2frame *frame, long data_length, int *error)
  {
    frame->data_length = data_length;
  }
char *
dwg_ent_ole2frame_get_data(dwg_ent_ole2frame *frame, int *error)
  {
    return frame->data;
  }
void
dwg_ent_ole2frame_set_data(dwg_ent_ole2frame *frame, char * data, int *error)
  {
    frame->data = data;
  }

//-------------------------------------------------------------------------------

unsigned int
dwg_entity_spline_get_scenario(dwg_ent_spline *spline, int *error)
{
    return spline->scenario;
}

void
dwg_entity_spline_set_scenario(dwg_ent_spline *spline, unsigned int scenario, int *error)
{
    spline->scenario = scenario;
}

unsigned int
dwg_entity_spline_get_degree(dwg_ent_spline *spline, int *error)
{
    return spline->degree;
}

void
dwg_entity_spline_set_degree(dwg_ent_spline *spline, unsigned int degree, int *error)
{
    spline->degree = degree;
}

double
dwg_entity_spline_get_fit_tol(dwg_ent_spline *spline, int *error)
{
    return spline->fit_tol;
}

void
dwg_entity_spline_set_fit_tol(dwg_ent_spline *spline, int fit_tol, int *error)
{
    spline->fit_tol = fit_tol;
}

void
dwg_entity_spline_get_begin_tan_vector(dwg_ent_spline *spline, dwg_point_3d *point, int *error)
{
    if(spline != 0)
    {
      *error = 0;
      point->x = spline->beg_tan_vec.x;
      point->y = spline->beg_tan_vec.y;
      point->z = spline->beg_tan_vec.z;
    }
  else
    {
      *error = 1;
    }
}


void
dwg_entity_spline_set_begin_tan_vector(dwg_ent_spline *spline, dwg_point_3d *point, int *error)
{
    if(spline != 0)
    {
      *error = 0;
      spline->beg_tan_vec.x = point->x;
      spline->beg_tan_vec.y = point->y;
      spline->beg_tan_vec.z = point->z;
    }
  else
    {
      *error = 1;
    }
}

void
dwg_entity_spline_get_end_tan_vector(dwg_ent_spline *spline, dwg_point_3d *point, int *error)
{
    if(spline != 0)
    {
      *error = 0;
      point->x = spline->end_tan_vec.x;
      point->y = spline->end_tan_vec.y;
      point->z = spline->end_tan_vec.z;
    }
  else
    {
      *error = 1;
    }
}

void
dwg_entity_spline_set_end_tan_vector(dwg_ent_spline *spline, dwg_point_3d *point, int *error)
{
  if(spline != 0)
    {
      *error = 0;
      spline->end_tan_vec.x = point->x;
      spline->end_tan_vec.y = point->y;
      spline->end_tan_vec.z = point->z;
    }
  else
    {
      *error = 1;
    }
}

double
dwg_entity_spline_get_knot_tol(dwg_ent_spline *spline, int *error)
{
    return spline->knot_tol;
}

void
dwg_entity_spline_set_knot_tol(dwg_ent_spline *spline, double knot_tol, int *error)
{
    spline->knot_tol = knot_tol;
}

double
dwg_entity_spline_get_ctrl_tol(dwg_ent_spline *spline, int *error)
{
    return spline->ctrl_tol;
}

void
dwg_entity_spline_set_ctrl_tol(dwg_ent_spline *spline, double ctrl_tol, int *error)
{
    spline->ctrl_tol = ctrl_tol;
}

unsigned int
dwg_entity_spline_get_num_fit_pts(dwg_ent_spline *spline, int *error)
{
    return spline->num_fit_pts;
}

void
dwg_entity_spline_set_num_fit_pts(dwg_ent_spline *spline, int num_fit_pts, int *error)
{
    spline->num_fit_pts = num_fit_pts;
}

char
dwg_entity_spline_get_rational(dwg_ent_spline *spline, int *error)
{
    return spline->rational;
}

void
dwg_entity_spline_set_rational(dwg_ent_spline *spline, char rational, int *error)
{
    spline->rational = rational;
}

char
dwg_entity_spline_get_closed_b(dwg_ent_spline *spline, int *error)
{
    return spline->closed_b;
}
void
dwg_entity_spline_set_closed_b(dwg_ent_spline *spline, char closed_b, int *error)
{
    spline->closed_b = closed_b;
}

char
dwg_entity_spline_get_weighted(dwg_ent_spline *spline, int *error)
{
    return spline->weighted;
}

void
dwg_entity_spline_set_weighted(dwg_ent_spline *spline, char weighted, int *error)
{
    spline->weighted = weighted;
}

char
dwg_entity_spline_get_periodic(dwg_ent_spline *spline, int *error)
{
    return spline->periodic;
}
void
dwg_entity_spline_set_periodic(dwg_ent_spline *spline, char periodic, int *error)
{
    spline->periodic = periodic;
}

long
dwg_entity_spline_get_num_knots(dwg_ent_spline *spline, int *error)
{
    return spline->num_knots;
}
void
dwg_entity_spline_set_num_knots(dwg_ent_spline *spline, long nums, int *error)
{
    spline->num_knots = nums;
}

long
dwg_entity_spline_get_num_ctrl_pts(dwg_ent_spline *spline, int *error)
{
    return spline->num_ctrl_pts;
}
void
dwg_entity_spline_set_num_ctrl_pts(dwg_ent_spline *spline, long nums, int *error)
{
    spline->num_ctrl_pts = nums;
}


dwg_ent_spline_point *
dwg_ent_spline_get_fit_points(dwg_ent_spline *spline, int *error)
{
  dwg_ent_spline_point *ptx = (dwg_ent_spline_point*) malloc(sizeof(dwg_ent_spline_point)* spline->num_fit_pts);
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
    {
      *error = 1;
    }
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
    {
      *error = 1;
    }
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
    {
      *error = 1;
    }
}

//-------------------------------------------------------------------------------

void
dwg_ent_viewport_get_center(dwg_ent_viewport *vp, dwg_point_3d *point, int *error)
{
    if (vp != 0)
        {
            *error = 0;
            point->x = vp->center.x;
            point->y = vp->center.y;
            point->z = vp->center.z;
        }
    else
        {
            *error = 1;
        }
}
void
dwg_ent_viewport_set_center(dwg_ent_viewport *vp, dwg_point_3d *point, int *error)
{
    if (vp != 0)
        {
            *error = 0;
            vp->center.x = point->x;
            vp->center.y = point->y;
            vp->center.z = point->z;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

unsigned int dwg_ent_viewport_get_grid_major(dwg_ent_viewport *vp, int *error)
{
    if (vp != 0)
        {
            *error = 0;
            return vp->grid_major;
        }
    else
        {
            *error = 1;
        }
}
void
dwg_ent_viewport_set_grid_major(dwg_ent_viewport *vp, unsigned int major, int *error)
{
    if (vp != 0)
        {
            *error = 0;
            vp->grid_major = major;
        }
    else
        {
            *error = 1;
        }
}

long   dwg_ent_viewport_get_frozen_layer_count(dwg_ent_viewport *vp, int *error)
{
    if (vp != 0)
        {
            *error = 0;
            return vp->frozen_layer_count;
        }
    else
        {
            *error = 1;
        }
}
void
dwg_ent_viewport_set_frozen_layer_count(dwg_ent_viewport *vp, long count, int *error)
{
    if (vp != 0)
        {
            *error = 0;
            vp->frozen_layer_count = count;
        }
    else
        {
            *error = 1;
        }
}

char * dwg_ent_viewport_get_style_sheet(dwg_ent_viewport *vp, int *error)
{
    if (vp != 0)
        {
            *error = 0;
            return vp->style_sheet;
        }
    else
        {
            *error = 1;
        }
}
void
dwg_ent_viewport_set_style_sheet(dwg_ent_viewport *vp, char * sheet, int *error)
{
    if (vp != 0)
        {
            *error = 0;
            vp->style_sheet = sheet;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_viewport_set_circle_zoom(dwg_ent_viewport *vp, unsigned int zoom, int *error)
{
    if (vp != 0)
        {
            *error = 0;
            vp->circle_zoom = zoom;
        }
    else
        {
            *error = 1;
        }
}
unsigned int dwg_ent_viewport_get_circle_zoom(dwg_ent_viewport *vp, int *error)
{
    if (vp != 0)
        {
            *error = 0;
            return vp->circle_zoom;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}
long   dwg_ent_viewport_get_status_flags(dwg_ent_viewport *vp, int *error)
{
    if (vp != 0)
        {
            *error = 0;
            return vp->status_flags;
        }
    else
        {
            *error = 1;
        }
}

char   dwg_ent_viewport_get_render_mode(dwg_ent_viewport *vp, int *error)
{
    if (vp != 0)
        {
            *error = 0;
            return vp->render_mode;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_viewport_set_ucs_at_origin(dwg_ent_viewport *vp, unsigned char origin, int *error)
{
    if (vp != 0)
        {
            *error = 0;
            vp->ucs_at_origin = origin;
        }
    else
        {
            *error = 1;
        }
}
unsigned char dwg_ent_viewport_get_ucs_at_origin(dwg_ent_viewport *vp, int *error)
{
    if (vp != 0)
        {
            *error = 0;
            return vp->ucs_at_origin;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_viewport_set_ucs_per_viewport(dwg_ent_viewport *vp, unsigned char viewport, int *error)
{
    if (vp != 0)
        {
            *error = 0;
            vp->ucs_per_viewport = viewport;
        }
    else
        {
            *error = 1;
        }
}
unsigned char dwg_ent_viewport_get_ucs_per_viewport(dwg_ent_viewport *vp, int *error)
{
    if (vp != 0)
        {
            *error = 0;
            return vp->ucs_per_viewport;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_viewport_set_view_target(dwg_ent_viewport *vp, dwg_point_3d *point, int *error)
{
    if (vp != 0)
        {
            *error = 0;
            vp->view_target.x = point->x;
            vp->view_target.y = point->y;
            vp->view_target.z = point->z;
        }
    else
        {
            *error = 1;
        }
}
void
dwg_ent_viewport_get_view_target(dwg_ent_viewport *vp, dwg_point_3d *point, int *error)
{
    if (vp != 0)
        {
            *error = 0;
            point->x = vp->view_target.x;
            point->y = vp->view_target.y;
            point->z = vp->view_target.z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_viewport_set_view_direction(dwg_ent_viewport *vp, dwg_point_3d *point, int *error)
{
    if (vp != 0)
        {
            *error = 0;
            vp->view_direction.x = point->x;
            vp->view_direction.y = point->y;
            vp->view_direction.z = point->z;
        }
    else
        {
            *error = 1;
        }
}
void
dwg_ent_viewport_get_view_direction(dwg_ent_viewport *vp, dwg_point_3d *point, int *error)
{
    if (vp != 0)
        {
            *error = 0;
            point->x = vp->view_direction.x;
            point->y = vp->view_direction.y;
            point->z = vp->view_direction.z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_viewport_set_view_twist_angle(dwg_ent_viewport *vp, double angle, int *error)
{
    if (vp != 0)
        {
            *error = 0;
            vp->view_twist_angle = angle;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_viewport_set_view_height(dwg_ent_viewport *vp, double height, int *error)
{
    if (vp != 0)
        {
            *error = 0;
            vp->view_height = height;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_viewport_set_lens_length(dwg_ent_viewport *vp, double length, int *error)
{
    if (vp != 0)
        {
            *error = 0;
            vp->lens_length = length;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_viewport_set_front_clip_z(dwg_ent_viewport *vp, double front_z, int *error)
{
    if (vp != 0)
        {
            *error = 0;
            vp->front_clip_z = front_z;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_viewport_set_back_clip_z(dwg_ent_viewport *vp, double back_z, int *error)
{
    if (vp != 0)
        {
            *error = 0;
            vp->back_clip_z = back_z;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_viewport_get_view_center(dwg_ent_viewport *vp,  dwg_point_2d *point,int *error)
{
    if (vp != 0)
        {
            *error = 0;
            point->x = vp->view_center.x;
            point->y = vp->view_center.y;
        }
    else
        {
            *error = 1;
        }
}
void
dwg_ent_viewport_set_view_center(dwg_ent_viewport *vp, dwg_point_2d *point, int *error)
{
    if (vp != 0)
        {
            *error = 0;
            vp->view_center.x = point->x;
            vp->view_center.y = point->y;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_viewport_get_grid_spacing(dwg_ent_viewport *vp, dwg_point_2d *point, int *error)
{
    if (vp != 0)
        {
            *error = 0;
            point->x = vp->grid_spacing.x;
            point->y = vp->grid_spacing.y;
        }
    else
        {
            *error = 1;
        }
}
void
dwg_ent_viewport_set_grid_spacing(dwg_ent_viewport *vp, dwg_point_2d *point, int *error)
{
    if (vp != 0)
        {
            *error = 0;
            vp->grid_spacing.x = point->x;
            vp->grid_spacing.y = point->y;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_viewport_get_snap_base(dwg_ent_viewport *vp, dwg_point_2d *point, int *error)
{
    if (vp != 0)
        {
            *error = 0;
            point->x = vp->snap_base.x;
            point->y = vp->snap_base.y;
        }
    else
        {
            *error = 1;
        }
}
void
dwg_ent_viewport_set_snap_base(dwg_ent_viewport *vp, dwg_point_2d *point, int *error)
{
    if (vp != 0)
        {
            *error = 0;
            vp->snap_base.x = point->x;
            vp->snap_base.y = point->y;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_viewport_get_snap_spacing(dwg_ent_viewport *vp, dwg_point_2d *point, int *error)
{
    if (vp != 0)
        {
            *error = 0;
            point->x = vp->snap_spacing.x;
            point->y = vp->snap_spacing.y;
        }
    else
        {
            *error = 1;
        }
}
void
dwg_ent_viewport_set_snap_spacing(dwg_ent_viewport *vp, dwg_point_2d *point, int *error)
{
    if (vp != 0)
        {
            *error = 0;
            vp->snap_spacing.x = point->x;
            vp->snap_spacing.y = point->y;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_viewport_set_ucs_origin(dwg_ent_viewport *vp, dwg_point_3d *point, int *error)
{
    if (vp != 0)
        {
            *error = 0;
            vp->ucs_origin.x = point->x;
            vp->ucs_origin.y = point->y;
            vp->ucs_origin.z = point->z;
        }
    else
        {
            *error = 1;
        }
}
void
dwg_ent_viewport_get_ucs_origin(dwg_ent_viewport *vp, dwg_point_3d *point, int *error)
{
    if (vp != 0)
        {
            *error = 0;
            point->x = vp->ucs_origin.x;
            point->y = vp->ucs_origin.y;
            point->z = vp->ucs_origin.z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_viewport_set_ucs_x_axis(dwg_ent_viewport *vp, dwg_point_3d *point, int *error)
{
    if (vp != 0)
        {
            *error = 0;
            vp->ucs_x_axis.x = point->x;
            vp->ucs_x_axis.y = point->y;
            vp->ucs_x_axis.z = point->z;
        }
    else
        {
            *error = 1;
        }
}
void
dwg_ent_viewport_get_ucs_x_axis(dwg_ent_viewport *vp, dwg_point_3d *point, int *error)
{
    if (vp != 0)
        {
            *error = 0;
            point->x = vp->ucs_x_axis.x;
            point->y = vp->ucs_x_axis.y;
            point->z = vp->ucs_x_axis.z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_viewport_set_ucs_y_axis(dwg_ent_viewport *vp, dwg_point_3d *point, int *error)
{
    if (vp != 0)
        {
            *error = 0;
            vp->ucs_y_axis.x = point->x;
            vp->ucs_y_axis.y = point->y;
            vp->ucs_y_axis.z = point->z;
        }
    else
        {
            *error = 1;
        }
}
void
dwg_ent_viewport_get_ucs_y_axis(dwg_ent_viewport *vp, dwg_point_3d *point, int *error)
{
    if (vp != 0)
        {
            *error = 0;
            point->x = vp->ucs_y_axis.x;
            point->y = vp->ucs_y_axis.y;
            point->z = vp->ucs_y_axis.z;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_viewport_set_ucs_elevation(dwg_ent_viewport *vp, double elevation, int *error)
{
    if (vp != 0)
        {
            *error = 0;
            vp->ucs_elevation = elevation;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_viewport_set_ucs_ortho_view_type(dwg_ent_viewport *vp, unsigned int type, int *error)
{
    if (vp != 0)
        {
            *error = 0;
            vp->ucs_ortho_view_type = type;
        }
    else
        {
            *error = 1;
        }
}
unsigned int dwg_ent_viewport_get_ucs_ortho_view_type(dwg_ent_viewport *vp, int *error)
{
    if (vp != 0)
        {
            *error = 0;
            return vp->ucs_ortho_view_type;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_viewport_set_shadeplot_mode(dwg_ent_viewport *vp, unsigned int shadeplot, int *error)
{
    if (vp != 0)
        {
            *error = 0;
            vp->shadeplot_mode = shadeplot;
        }
    else
        {
            *error = 1;
        }
}
unsigned int dwg_ent_viewport_get_shadeplot_mode(dwg_ent_viewport *vp, int *error)
{
    if (vp != 0)
        {
            *error = 0;
            return vp->shadeplot_mode;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_viewport_set_use_def_lights(dwg_ent_viewport *vp, unsigned char lights, int *error)
{
    if (vp != 0)
        {
            *error = 0;
            vp->use_def_lights = lights;
        }
    else
        {
            *error = 1;
        }
}
unsigned char dwg_ent_viewport_get_use_def_lights(dwg_ent_viewport *vp, int *error)
{
    if (vp != 0)
        {
            *error = 0;
            return vp->use_def_lights;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_viewport_set_def_lighting_type(dwg_ent_viewport *vp, char type, int *error)
{
    if (vp != 0)
        {
            *error = 0;
            vp->def_lighting_type = type;
        }
    else
        {
            *error = 1;
        }
}
char dwg_ent_viewport_get_def_lighting_type(dwg_ent_viewport *vp, int *error)
{
    if (vp != 0)
        {
            *error = 0;
            return vp->def_lighting_type;
        }
    else
        {
            *error = 1;
        }
}

void
dwg_ent_viewport_set_brightness(dwg_ent_viewport *vp, double brightness, int *error)
{
    if (vp != 0)
        {
            *error = 0;
            vp->brightness = brightness;
        }
    else
        {
            *error = 1;
        }
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
        {
            *error = 1;
        }
}

void
dwg_ent_viewport_set_contrast(dwg_ent_viewport *vp, double contrast, int *error)
{
    if (vp != 0)
        {
            *error = 0;
            vp->contrast = contrast;
        }
    else
        {
            *error = 1;
        }
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