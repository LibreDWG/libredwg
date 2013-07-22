#include "api.h"
#include <dwg.h>
#include <malloc.h>

//---------------------------------------------------------------------------

dwg_ent_circle *
dwg_ent_circle_new()
{
    dwg_ent_circle *circle = (dwg_ent_circle*) malloc(sizeof(Dwg_Entity_CIRCLE));
    return dwg_ent_circle_init(circle);
}

void 
dwg_ent_circle_delete(dwg_ent_circle *circle)
{
    if (circle != 0)
        free(circle);
}

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

// Get/Set the center point of a circle
void
dwg_ent_circle_get_center(dwg_ent_circle *circle, dwg_point_3d *point)
{
    point->x = circle->center.x;
    point->y = circle->center.y;
    point->z = circle->center.z;
}

void
dwg_ent_circle_set_center(dwg_ent_circle *circle, dwg_point_3d *point)
{
    circle->center.x = point->x;
    circle->center.y = point->y;
    circle->center.z = point->z;
}

// Get/Set the radius of a circle
double
dwg_ent_circle_get_radius(dwg_ent_circle *circle)
{
    return circle->radius;
}

void
dwg_ent_circle_set_radius(dwg_ent_circle *circle, double radius)
{
    circle->radius = radius;
}

// Get/Set the thickness of a circle
double
dwg_ent_circle_get_thickness(dwg_ent_circle *circle)
{
    return circle->thickness;
}

void
dwg_ent_circle_set_thickness(dwg_ent_circle *circle, double thickness)
{
    circle->thickness = thickness;
}

// Get/Set the extrusion of a circle
void
dwg_ent_circle_set_extrusion(dwg_ent_circle *circle, dwg_vector_3d *vector)
{
    circle->extrusion.x = vector->x;
    circle->extrusion.y = vector->y;
    circle->extrusion.z = vector->z;
}

void
dwg_ent_circle_get_extrusion(dwg_ent_circle *circle, dwg_vector_3d *vector)
{
    vector->x = circle->extrusion.x;
    vector->y = circle->extrusion.y;
    vector->z = circle->extrusion.z;
}

//---------------------------------------------------------------------------

dwg_ent_line *
dwg_ent_line_new()
{
    dwg_ent_line *line = (dwg_ent_line*) malloc(sizeof(Dwg_Entity_LINE));
    return dwg_ent_line_init(line);
}

void 
dwg_ent_line_delete(dwg_ent_line *line)
{
    if (line != 0)
        free(line);
}

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

void
dwg_ent_line_get_start_point(dwg_ent_line *line, dwg_point_2d *point)
{
    point->x = line->start.x;
    point->y = line->start.y;
}

void
dwg_ent_line_set_start_point(dwg_ent_line *line, dwg_point_2d *point)
{
    line->start.x = point->x;
    line->start.y = point->y;
}

void
dwg_ent_line_get_end_point(dwg_ent_line *line, dwg_point_2d *point)
{
    point->x = line->end.x;
    point->y = line->end.y;
}

void
dwg_ent_line_set_end_point(dwg_ent_line *line, dwg_point_2d *point)
{
    line->end.x = point->x;
    line->end.y = point->y;
}

double
dwg_ent_line_get_thickness(dwg_ent_line *line)
{
    return line->thickness;
}

void
dwg_ent_line_set_thickness(dwg_ent_line *line, double thickness)
{
    line->thickness = thickness;
}

void
dwg_ent_line_get_extrusion(dwg_ent_line *line, dwg_vector_3d *vector)
{
    vector->x = line->extrusion.x;
    vector->y = line->extrusion.y;
    vector->z = line->extrusion.z;
}

void
dwg_ent_line_set_extrusion(dwg_ent_line *line, dwg_vector_3d *vector)
{
    line->extrusion.x = vector->x;
    line->extrusion.y = vector->y;
    line->extrusion.z = vector->z;
}

//---------------------------------------------------------------------------

dwg_ent_arc *
dwg_ent_arc_new()
{
    dwg_ent_arc *arc = (dwg_ent_arc*) malloc(sizeof(Dwg_Entity_ARC));
    return dwg_ent_arc_init(arc);
}

void 
dwg_ent_arc_delete(dwg_ent_arc *arc)
{
    if (arc != 0)
        free(arc);
}

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

// Get/Set the center point of a arc
void
dwg_ent_arc_get_center(dwg_ent_arc *arc, dwg_point_3d *point)
{
    point->x = arc->center.x;
    point->y = arc->center.y;
    point->z = arc->center.z;
}

void
dwg_ent_arc_set_center(dwg_ent_arc *arc, dwg_point_3d *point)
{
    arc->center.x = point->x;
    arc->center.y = point->y;
    arc->center.z = point->z;
}

// Get/Set the radius of a arc
double
dwg_ent_arc_get_radius(dwg_ent_arc *arc)
{
    return arc->radius;
}

void
dwg_ent_arc_set_radius(dwg_ent_arc *arc, double radius)
{
    arc->radius = radius;
}

// Get/Set the thickness of arc
double
dwg_ent_arc_get_thickness(dwg_ent_arc *arc)
{
    return arc->thickness;
}

void
dwg_ent_arc_set_thickness(dwg_ent_arc *arc, double thickness)
{
    arc->thickness = thickness;
}

//Get/Set the extrusion of arc
void 
dwg_ent_arc_get_extrusion(dwg_ent_arc *arc, dwg_vector_3d *vector)
{
    vector->x = arc->extrusion.x;
    vector->y = arc->extrusion.y;
    vector->z = arc->extrusion.z;
}

void 
dwg_ent_arc_set_extrusion(dwg_ent_arc *arc, dwg_vector_3d *vector)
{
    arc->extrusion.x = vector->x;
    arc->extrusion.y = vector->y;
    arc->extrusion.z = vector->z;    
}

// Get/set start angle arc
double
dwg_ent_arc_get_start_angle(dwg_ent_arc *arc)
{
    return arc->start_angle;
}

void
dwg_ent_arc_set_start_angle(dwg_ent_arc *arc, double start_angle)
{
    arc->start_angle = start_angle;
}

// Get/set end angle of arc
double
dwg_ent_arc_get_end_angle(dwg_ent_arc *arc)
{
    return arc->end_angle;
}

void
dwg_ent_arc_set_end_angle(dwg_ent_arc *arc, double end_angle)
{
    arc->end_angle = end_angle;
}

//---------------------------------------------------------------------------

dwg_ent_ellipse *
dwg_ent_ellipse_new()
{
    dwg_ent_ellipse *ellipse = (dwg_ent_ellipse*) malloc(sizeof(Dwg_Entity_ELLIPSE));
    return dwg_ent_ellipse_init(ellipse);
}

void
dwg_ent_ellipse_delete(dwg_ent_ellipse *ellipse)
{
    if (ellipse != 0)
        free(ellipse);
}

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

// Get/Set center of ellipse
void
dwg_ent_ellipse_get_center(dwg_ent_ellipse *ellipse, dwg_point_3d *point)
{
    point->x = ellipse->center.x;
    point->y = ellipse->center.y;
    point->z = ellipse->center.z;
}

void
dwg_ent_ellipse_set_center(dwg_ent_ellipse *ellipse, dwg_point_3d *point)
{
    ellipse->center.x = point->x;
    ellipse->center.y = point->y;
    ellipse->center.z = point->z;
}

// Get/Set sm axis of ellipse
void
dwg_ent_ellipse_get_sm_axis(dwg_ent_ellipse *ellipse, dwg_point_3d *point)
{
    point->x = ellipse->sm_axis.x;
    point->y = ellipse->sm_axis.y;
    point->z = ellipse->sm_axis.z;
}

void
dwg_ent_ellipse_set_sm_axis(dwg_ent_ellipse *ellipse, dwg_point_3d *point)
{
    ellipse->sm_axis.x = point->x;
    ellipse->sm_axis.y = point->y;
    ellipse->sm_axis.z = point->z;
}

// Get/Set extrusion of ellipse
void
dwg_ent_ellipse_get_extrusion(dwg_ent_ellipse *ellipse, dwg_vector_3d *vector)
{
    vector->x = ellipse->extrusion.x;
    vector->y = ellipse->extrusion.y;
    vector->z = ellipse->extrusion.z;
}

void
dwg_ent_ellipse_set_extrusion(dwg_ent_ellipse *ellipse, dwg_vector_3d *vector)
{
    ellipse->extrusion.x = vector->x;
    ellipse->extrusion.y = vector->y;
    ellipse->extrusion.z = vector->z;
}

// Get/Set axis ratio of ellipse
double
dwg_ent_ellipse_get_axis_ratio(dwg_ent_ellipse *ellipse)
{
    return ellipse->axis_ratio;
}

void
dwg_ent_ellipse_set_axis_ratio(dwg_ent_ellipse *ellipse, double ratio)
{
    ellipse->axis_ratio = ratio;
}

// Get/Set start angle of ellipse
double
dwg_ent_ellipse_get_start_angle(dwg_ent_ellipse *ellipse)
{
    return ellipse->start_angle;
}

void
dwg_ent_ellipse_set_start_angle(dwg_ent_ellipse *ellipse, double start_angle)
{
    ellipse->start_angle = start_angle;
}

// Get/Set end angle of ellipse
double
dwg_ent_ellipse_get_end_angle(dwg_ent_ellipse *ellipse)
{
    return ellipse->end_angle;
}

void
dwg_ent_ellipse_set_end_angle(dwg_ent_ellipse *ellipse, double end_angle)
{
    ellipse->end_angle = end_angle;
}

//---------------------------------------------------------------------------

// Get/Set text of text
void
dwg_ent_text_set_text(dwg_ent_text *text, char * text_value)
{
    text->text_value = text_value;
}

char *
dwg_ent_text_get_text(dwg_ent_text *text)
{
    return text->text_value;
}

//Get/Set insertion point of text
void
dwg_ent_text_get_insertion_point(dwg_ent_text *text, dwg_point_2d *point)
{
    point->x = text->insertion_pt.x;
    point->y = text->insertion_pt.y;
}

void
dwg_ent_text_set_insertion_point(dwg_ent_text *text, dwg_point_2d *point)
{
    text->insertion_pt.x = point->x;
    text->insertion_pt.y = point->y;
}

//Get/Set height of text
double
dwg_ent_text_get_height(dwg_ent_text *text)
{
    return text->height;
}

void
dwg_ent_text_set_height(dwg_ent_text *text, double height)
{
    text->height = height;
}

//Get/Set extrusion of text
void
dwg_ent_text_get_extrusion(dwg_ent_text *text, dwg_vector_3d *vector)
{
    vector->x = text->extrusion.x;
    vector->y = text->extrusion.y;
    vector->z = text->extrusion.z;
}

void
dwg_ent_text_set_extrusion(dwg_ent_text *text, dwg_vector_3d *vector)
{
    text->extrusion.x = vector->x;
    text->extrusion.y = vector->y;
    text->extrusion.z = vector->z;
}

//Get/Set thickness of text
double
dwg_ent_text_get_thickness(dwg_ent_text *text)
{
    return text->thickness;
}

void
dwg_ent_text_set_thickness(dwg_ent_text *text, double thickness)
{
    text->thickness = thickness;
}

//Get/Set rotation angle of text
double
dwg_ent_text_get_rot_angle(dwg_ent_text *text)
{
    return text->rotation_ang;
}

void
dwg_ent_text_set_rot_angle(dwg_ent_text *text, double angle)
{
    text->rotation_ang = angle;
}

//Get/Set horizontal alignment angle of text
double
dwg_ent_text_get_vert_align(dwg_ent_text *text)
{
    return text->vert_alignment;
}
void
dwg_ent_text_set_vert_align(dwg_ent_text *text, double alignment)
{
    text->vert_alignment = alignment;
}

//Get/Set vertical alignment of text
double
dwg_ent_text_get_horiz_align(dwg_ent_text *text)
{
    return text->horiz_alignment;
}

void
dwg_ent_text_set_horiz_align(dwg_ent_text *text, double alignment)
{
    text->horiz_alignment = alignment;
}

//---------------------------------------------------------------------------

// Get/Set text of attrib
void
dwg_ent_attrib_set_text(dwg_ent_attrib *attrib, char * text_value)
{
    attrib->text_value = text_value;
}

char *
dwg_ent_attrib_get_text(dwg_ent_attrib *attrib)
{
    return attrib->text_value;
}

//Get/Set insertion point of attrib
void
dwg_ent_attrib_get_insertion_point(dwg_ent_attrib *attrib, dwg_point_2d *point)
{
    point->x = attrib->insertion_pt.x;
    point->y = attrib->insertion_pt.y;
}

void
dwg_ent_attrib_set_insertion_point(dwg_ent_attrib *attrib, dwg_point_2d *point)
{
    attrib->insertion_pt.x = point->x;
    attrib->insertion_pt.y = point->y;
}

//Get/Set height of attrib
double
dwg_ent_attrib_get_height(dwg_ent_attrib *attrib)
{
    return attrib->height;
}

void
dwg_ent_attrib_set_height(dwg_ent_attrib *attrib, double height)
{
    attrib->height = height;
}

//Get/Set extrusion of attrib
void
dwg_ent_attrib_get_extrusion(dwg_ent_attrib *attrib, dwg_vector_3d *vector)
{
    vector->x = attrib->extrusion.x;
    vector->y = attrib->extrusion.y;
    vector->z = attrib->extrusion.z;
}

void
dwg_ent_attrib_set_extrusion(dwg_ent_attrib *attrib, dwg_vector_3d *vector)
{
    attrib->extrusion.x = vector->x;
    attrib->extrusion.y = vector->y;
    attrib->extrusion.z = vector->z;
}

//Get/Set thickness of attrib
double
dwg_ent_attrib_get_thickness(dwg_ent_attrib *attrib)
{
    return attrib->thickness;
}

void
dwg_ent_attrib_set_thickness(dwg_ent_attrib *attrib, double thickness)
{
    attrib->thickness = thickness;
}

//Get/Set rotation angle of attrib
double
dwg_ent_attrib_get_rot_angle(dwg_ent_attrib *attrib)
{
    return attrib->rotation_ang;
}

void
dwg_ent_attrib_set_rot_angle(dwg_ent_attrib *attrib, double angle)
{
    attrib->rotation_ang = angle;
}

//Get/Set horizontal alignment angle of attrib
double
dwg_ent_attrib_get_vert_align(dwg_ent_attrib *attrib)
{
    return attrib->vert_alignment;
}
void
dwg_ent_attrib_set_vert_align(dwg_ent_attrib *attrib, double alignment)
{
    attrib->vert_alignment = alignment;
}

//Get/Set vertical alignment of attrib
double
dwg_ent_attrib_get_horiz_align(dwg_ent_attrib *attrib)
{
    return attrib->horiz_alignment;
}

void
dwg_ent_attrib_set_horiz_align(dwg_ent_attrib *attrib, double alignment)
{
    attrib->horiz_alignment = alignment;
}

//---------------------------------------------------------------------------

// Get/Set text of attdef
void
dwg_ent_attdef_set_text(dwg_ent_attdef *attdef, char * default_value)
{
    attdef->default_value = default_value;
}

char *
dwg_ent_attdef_get_text(dwg_ent_attdef *attdef)
{
    return attdef->default_value;
}

//Get/Set insertion point of attdef
void
dwg_ent_attdef_get_insertion_point(dwg_ent_attdef *attdef, dwg_point_2d *point)
{
    point->x = attdef->insertion_pt.x;
    point->y = attdef->insertion_pt.y;
}

void
dwg_ent_attdef_set_insertion_point(dwg_ent_attdef *attdef, dwg_point_2d *point)
{
    attdef->insertion_pt.x = point->x;
    attdef->insertion_pt.y = point->y;
}

//Get/Set height of attdef
double
dwg_ent_attdef_get_height(dwg_ent_attdef *attdef)
{
    return attdef->height;
}

void
dwg_ent_attdef_set_height(dwg_ent_attdef *attdef, double height)
{
    attdef->height = height;
}

//Get/Set extrusion of attdef
void
dwg_ent_attdef_get_extrusion(dwg_ent_attdef *attdef, dwg_vector_3d *vector)
{
    vector->x = attdef->extrusion.x;
    vector->y = attdef->extrusion.y;
    vector->z = attdef->extrusion.z;
}

void
dwg_ent_attdef_set_extrusion(dwg_ent_attdef *attdef, dwg_vector_3d *vector)
{
    attdef->extrusion.x = vector->x;
    attdef->extrusion.y = vector->y;
    attdef->extrusion.z = vector->z;
}

//Get/Set thickness of attdef
double
dwg_ent_attdef_get_thickness(dwg_ent_attdef *attdef)
{
    return attdef->thickness;
}

void
dwg_ent_attdef_set_thickness(dwg_ent_attdef *attdef, double thickness)
{
    attdef->thickness = thickness;
}

//Get/Set rotation angle of attdef
double
dwg_ent_attdef_get_rot_angle(dwg_ent_attdef *attdef)
{
    return attdef->rotation_ang;
}

void
dwg_ent_attdef_set_rot_angle(dwg_ent_attdef *attdef, double angle)
{
    attdef->rotation_ang = angle;
}

//Get/Set horizontal alignment angle of attdef
double
dwg_ent_attdef_get_vert_align(dwg_ent_attdef *attdef)
{
    return attdef->vert_alignment;
}

void
dwg_ent_attdef_set_vert_align(dwg_ent_attdef *attdef, double alignment)
{
    attdef->vert_alignment = alignment;
}

//Get/Set vertical alignment of attdef
double
dwg_ent_attdef_get_horiz_align(dwg_ent_attdef *attdef)
{
    return attdef->horiz_alignment;
}

void
dwg_ent_attdef_set_horiz_align(dwg_ent_attdef *attdef, double alignment)
{
    attdef->horiz_alignment = alignment;
}

//-------------------------------------------------------------------------------

dwg_obj_block_header *
dwg_get_block_header(Dwg_Data *dwg)
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

dwg_obj_block_control *
dwg_block_header_get_block_control(dwg_obj_block_header* block_header)
{
    return block_header->block_control_handle->obj->tio.object->tio.BLOCK_CONTROL;
}

//-------------------------------------------------------------------------------

// Get Layer Name
char *
dwg_obj_layer_get_name(dwg_obj_layer *layer)
{
    return layer->entry_name;
}

//-------------------------------------------------------------------------------

// Get Block Name
char *
dwg_obj_block_header_get_name(dwg_obj_block_header *hdr)
{
    return hdr->entry_name;
}
