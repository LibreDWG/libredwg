
// libredwg API example
// This is the only file an user of the API needs to include

#ifndef _API_H_
#define _API_H_

typedef struct dwg_point_3d
{
	double x;
	double y;
	double z;
} dwg_point_3d;

typedef struct dwg_vector_3d
{
	double x;
	double y;
	double z;
} dwg_vector_3d;

typedef struct dwg_point_2d
{
	double x;
	double y;
} dwg_point_2d;

typedef struct dwg_vector_2d
{
	double x;
	double y;
} dwg_vector_2d;

// Extract All Entities of a specific type from a BLOCK
#define GET_DWG_ENTITY(token) \
Dwg_Entity_##token ** \
 dwg_get_##token (Dwg_Object_Ref * ref) \
{ \
  int x=0,counts=0; \
  Dwg_Object * obj; \
  Dwg_Object_BLOCK_HEADER * hdr; \
  hdr = ref->obj->tio.object->tio.BLOCK_HEADER; \
  obj = get_first_owned_object(ref->obj, hdr); \
  while(obj) \
    { \
    if(obj->type== DWG_TYPE_##token) \
      { \
        counts++; \
      } \
    obj = get_next_owned_object(ref->obj, obj, hdr); \
    } \
  Dwg_Entity_##token ** ret_##token = (Dwg_Entity_##token **)malloc ( \
  counts * sizeof ( Dwg_Entity_##token *)); \
  obj = get_first_owned_object(ref->obj, hdr); \
  while(obj) \
   { \
     if(obj->type==DWG_TYPE_##token) \
      { \
        ret_##token[x] = obj->tio.entity->tio.token; \
        x++; \
      } \
    obj = get_next_owned_object(ref->obj, obj, hdr); \
  } \
  return ret_##token; \
}

// Cast a Dwg_Object to Entity
#define CAST_OBJECT(token) \
Dwg_Entity_##token * \
dwg_object_to_##token(Dwg_Object *obj) \
{ \
  Dwg_Entity_##token *ret_obj; \
  if(obj != 0 && obj->type == DWG_TYPE_##token) \
    { \
      ret_obj = obj->tio.entity->tio.token; \
    } \
  return ret_obj; \
}

typedef struct _dwg_entity_CIRCLE         dwg_ent_circle;
typedef struct _dwg_entity_LINE           dwg_ent_line;
typedef struct _dwg_entity_ARC            dwg_ent_arc;
typedef struct _dwg_entity_ELLIPSE        dwg_ent_ellipse;
typedef struct _dwg_entity_TEXT           dwg_ent_text;
typedef struct _dwg_entity_ATTRIB         dwg_ent_attrib;
typedef struct _dwg_entity_ATTDEF         dwg_ent_attdef;
typedef struct _dwg_object_LAYER          dwg_obj_layer;
typedef struct _dwg_object_BLOCK_HEADER   dwg_obj_block_header;
typedef struct _dwg_object_BLOCK_CONTROL  dwg_obj_block_control;

//-------------------------------------------------------------------------------

// Creates a new circle entity
dwg_ent_circle* dwg_ent_circle_new();

// Deletes a circle entity
void   dwg_ent_circle_delete(dwg_ent_circle *circle);

// Initializes a circle with its default values
dwg_ent_circle* dwg_ent_circle_init(dwg_ent_circle *circle);

// Get/Set the center point of a circle
void   dwg_ent_circle_get_center(dwg_ent_circle *circle, dwg_point_3d *point);
void   dwg_ent_circle_set_center(dwg_ent_circle *circle, dwg_point_3d *point);

// Get/Set the radius of a circle
double dwg_ent_circle_get_radius(dwg_ent_circle *circle);
void   dwg_ent_circle_set_radius(dwg_ent_circle *circle, double radius);

// Get/Set the thickness of a circle
double dwg_ent_circle_get_thickness(dwg_ent_circle *circle);
void   dwg_ent_circle_set_thickness(dwg_ent_circle *circle, double thickness);

// Get/Set the extrusion of a circle
void   dwg_ent_circle_set_extrusion(dwg_ent_circle *circle, dwg_vector_3d *vector);
void   dwg_ent_circle_get_extrusion(dwg_ent_circle *circle, dwg_vector_3d *vector);

//-------------------------------------------------------------------------------

// Creates a new line entity
dwg_ent_line* dwg_ent_line_new();

// Deletes a line entity
void   dwg_ent_line_delete(dwg_ent_line *line);

// Initializes a line with its default values
dwg_ent_line* dwg_ent_line_init(dwg_ent_line *line);

// Get/Set the start point of a line
void   dwg_ent_line_get_start_point(dwg_ent_line *line, dwg_point_2d *point);
void   dwg_ent_line_set_start_point(dwg_ent_line *line, dwg_point_2d *point);

// Get/Set the end point of a line
void   dwg_ent_line_get_end_point(dwg_ent_line *line, dwg_point_2d *point);
void   dwg_ent_line_set_end_point(dwg_ent_line *line, dwg_point_2d *point);

// Get/Set the thickness of a line
double dwg_ent_line_get_thickness(dwg_ent_line *line);
void   dwg_ent_line_set_thickness(dwg_ent_line *line, double thickness);

// Get/Set the extrusion of a line
void   dwg_ent_line_set_extrusion(dwg_ent_line *line, dwg_vector_3d *vector);
void   dwg_ent_line_get_extrusion(dwg_ent_line *line, dwg_vector_3d *vector);

//-------------------------------------------------------------------------------

// Creates a new arc entity
dwg_ent_arc* dwg_ent_arc_new();

// Deletes a arc entity
void dwg_ent_arc_delete(dwg_ent_arc *arc);

// Initializes a arc with its default values
dwg_ent_arc* dwg_ent_arc_init(dwg_ent_arc *arc);

// Get/Set the center point of a arc
void   dwg_ent_arc_get_center(dwg_ent_arc *arc, dwg_point_3d *point);
void   dwg_ent_arc_set_center(dwg_ent_arc *arc, dwg_point_3d *point);

// Get/Set the radius of a arc
double dwg_ent_arc_get_radius(dwg_ent_arc *arc);
void   dwg_ent_arc_set_radius(dwg_ent_arc *arc, double radius);

// Get/Set the thickness of arc
double dwg_ent_arc_get_thickness(dwg_ent_arc *arc);
void   dwg_ent_arc_set_thickness(dwg_ent_arc *arc, double thickness);

//Get/Set the extrusion of arc
void   dwg_ent_arc_get_extrusion(dwg_ent_arc *arc, dwg_vector_3d *vector);
void   dwg_ent_arc_set_extrusion(dwg_ent_arc *arc, dwg_vector_3d *vector);

// Get/set start angle arc
double dwg_ent_arc_get_start_angle(dwg_ent_arc *arc);
void   dwg_ent_arc_set_start_angle(dwg_ent_arc *arc, double start_angle);

// Get/set end angle of arc
double dwg_ent_arc_get_end_angle(dwg_ent_arc *arc);
void   dwg_ent_arc_set_end_angle(dwg_ent_arc *arc, double end_angle);

//-------------------------------------------------------------------------------

// Creates a new ellipse entity
dwg_ent_ellipse* dwg_ent_ellipse_new();

// Deletes a ellipse entity
void   dwg_ent_ellipse_delete(dwg_ent_ellipse *ellipse);

// Initializes a ellipse with its default values
dwg_ent_ellipse* dwg_ent_ellipse_init(dwg_ent_ellipse *ellipse);

// Get/Set center of ellipse
void   dwg_ent_ellipse_get_center(dwg_ent_ellipse *ellipse, dwg_point_3d *point);
void   dwg_ent_ellipse_set_center(dwg_ent_ellipse *ellipse, dwg_point_3d *point);

// Get/Set sm axis of ellipse
void   dwg_ent_ellipse_get_sm_axis(dwg_ent_ellipse *ellipse, dwg_point_3d *point);
void   dwg_ent_ellipse_set_sm_axis(dwg_ent_ellipse *ellipse, dwg_point_3d *point);

// Get/Set extrusion of ellipse
void   dwg_ent_ellipse_get_extrusion(dwg_ent_ellipse *ellipse, dwg_vector_3d *vector);
void   dwg_ent_ellipse_set_extrusion(dwg_ent_ellipse *ellipse, dwg_vector_3d *vector);

// Get/Set axis ratio of ellipse
double dwg_ent_ellipse_get_axis_ratio(dwg_ent_ellipse *ellipse);
void   dwg_ent_ellipse_set_axis_ratio(dwg_ent_ellipse *ellipse, double ratio);

// Get/Set start angle of ellipse
double dwg_ent_ellipse_get_start_angle(dwg_ent_ellipse *ellipse);
void   dwg_ent_ellipse_set_start_angle(dwg_ent_ellipse *ellipse, double start_angle);

// Get/Set end angle of ellipse
double dwg_ent_ellipse_get_end_angle(dwg_ent_ellipse *ellipse);
void   dwg_ent_ellipse_set_end_angle(dwg_ent_ellipse *ellipse, double end_angle);

//-------------------------------------------------------------------------------

// Get/Set text of text
void   dwg_ent_text_set_text(dwg_ent_text *text, char * text_value);
char * dwg_ent_text_get_text(dwg_ent_text *text);

// Get/Set insertion points of text
void   dwg_ent_text_get_insertion_point(dwg_ent_text *text, dwg_point_2d *point);
void   dwg_ent_text_set_insertion_point(dwg_ent_text *text, dwg_point_2d *point);

//Get/Set height of text
double dwg_ent_text_get_height(dwg_ent_text *text);
void   dwg_ent_text_set_height(dwg_ent_text *text, double height);

//Get/Set extrusion of text
void   dwg_ent_text_get_extrusion(dwg_ent_text *text, dwg_vector_3d *vector);
void   dwg_ent_text_set_extrusion(dwg_ent_text *text, dwg_vector_3d *vector);

//Get/Set thickness of text
double dwg_ent_text_get_thickness(dwg_ent_text *text);
void   dwg_ent_text_set_thickness(dwg_ent_text *text, double thickness);

//Get/Set rotation angle of text
double dwg_ent_text_get_rot_angle(dwg_ent_text *text);
void   dwg_ent_text_set_rot_angle(dwg_ent_text *text, double angle);

//Get/Set horizontal alignment angle of text
double dwg_ent_text_get_vert_align(dwg_ent_text *text);
void   dwg_ent_text_set_vert_align(dwg_ent_text *text, double alignment);

//Get/Set vertical alignment of text
double dwg_ent_text_get_horiz_align(dwg_ent_text *text);
void   dwg_ent_text_set_horiz_align(dwg_ent_text *text, double alignment);

//-------------------------------------------------------------------------------

// Get/Set text of attrib
void   dwg_ent_attrib_set_text(dwg_ent_attrib *attrib, char * text_value);
char * dwg_ent_attrib_get_text(dwg_ent_attrib *attrib);

// Get/Set insertion points of attrib
void   dwg_ent_attrib_get_insertion_point(dwg_ent_attrib *attrib, dwg_point_2d *point);
void   dwg_ent_attrib_set_insertion_point(dwg_ent_attrib *attrib, dwg_point_2d *point);

//Get/Set height of attrib
double dwg_ent_attrib_get_height(dwg_ent_attrib *attrib);
void   dwg_ent_attrib_set_height(dwg_ent_attrib *attrib, double height);

//Get/Set extrusion of attrib
void   dwg_ent_attrib_get_extrusion(dwg_ent_attrib *attrib, dwg_vector_3d *vector);
void   dwg_ent_attrib_set_extrusion(dwg_ent_attrib *attrib, dwg_vector_3d *vector);

//Get/Set thickness of attrib
double dwg_ent_attrib_get_thickness(dwg_ent_attrib *attrib);
void   dwg_ent_attrib_set_thickness(dwg_ent_attrib *attrib, double thickness);

//Get/Set rotation angle of attrib
double dwg_ent_attrib_get_rot_angle(dwg_ent_attrib *attrib);
void   dwg_ent_attrib_set_rot_angle(dwg_ent_attrib *attrib, double angle);

//Get/Set horizontal alignment angle of attrib
double dwg_ent_attrib_get_vert_align(dwg_ent_attrib *attrib);
void   dwg_ent_attrib_set_vert_align(dwg_ent_attrib *attrib, double alignment);

//Get/Set vertical alignment of attrib
double dwg_ent_attrib_get_horiz_align(dwg_ent_attrib *attrib);
void   dwg_ent_attrib_set_horiz_align(dwg_ent_attrib *attrib, double alignment);

//-------------------------------------------------------------------------------

// Get/Set text of attrib
void   dwg_ent_attdef_set_text(dwg_ent_attdef *attdef, char * default_value);
char * dwg_ent_attdef_get_text(dwg_ent_attdef *attdef);

// Get/Set insertion points of attdef
void   dwg_ent_attdef_get_insertion_point(dwg_ent_attdef *attdef, dwg_point_2d *point);
void   dwg_ent_attdef_set_insertion_point(dwg_ent_attdef *attdef, dwg_point_2d *point);

//Get/Set height of attdef
double dwg_ent_attdef_get_height(dwg_ent_attdef *attdef);
void   dwg_ent_attdef_set_height(dwg_ent_attdef *attdef, double height);

//Get/Set extrusion of attdef
void   dwg_ent_attdef_get_extrusion(dwg_ent_attdef *attdef, dwg_vector_3d *vector);
void   dwg_ent_attdef_set_extrusion(dwg_ent_attdef *attdef, dwg_vector_3d *vector);

//Get/Set thickness of attdef
double dwg_ent_attdef_get_thickness(dwg_ent_attdef *attdef);
void   dwg_ent_attdef_set_thickness(dwg_ent_attdef *attdef, double thickness);

//Get/Set rotation angle of attdef
double dwg_ent_attdef_get_rot_angle(dwg_ent_attdef *attdef);
void   dwg_ent_attdef_set_rot_angle(dwg_ent_attdef *attdef, double angle);

//Get/Set horizontal alignment angle of attdef
double dwg_ent_attdef_get_vert_align(dwg_ent_attdef *attdef);
void   dwg_ent_attdef_set_vert_align(dwg_ent_attdef *attdef, double alignment);

//Get/Set vertical alignment of attdef
double dwg_ent_attdef_get_horiz_align(dwg_ent_attdef *attdef);
void   dwg_ent_attdef_set_horiz_align(dwg_ent_attdef *attdef, double alignment);

//--------------------------------------------------------------------------------

dwg_obj_block_header * dwg_get_block_header(Dwg_Data *dwg);

//--------------------------------------------------------------------------------

dwg_obj_block_control* dwg_block_header_get_block_control(dwg_obj_block_header* block_header);

//--------------------------------------------------------------------------------

// Get Layer Name
char * dwg_obj_layer_get_name(dwg_obj_layer *layer);

//--------------------------------------------------------------------------------

// Get Block Name
char * dwg_obj_block_header_get_name(dwg_obj_block_header *hdr);

#endif
