
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

typedef struct dwg_point_2d
{
	double x;
	double y;
} dwg_point_2d;

typedef struct error
{
  int evalue;
} error;

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

//------------------------------------------------------------------------------
typedef struct _dwg_entity_CIRCLE            dwg_ent_circle;
typedef struct _dwg_entity_LINE              dwg_ent_line;
typedef struct _dwg_entity_ARC               dwg_ent_arc;
typedef struct _dwg_entity_ELLIPSE           dwg_ent_ellipse;
typedef struct _dwg_entity_TEXT              dwg_ent_text;
typedef struct _dwg_entity_POINT             dwg_ent_point;
typedef struct _dwg_entity_ATTRIB            dwg_ent_attrib;
typedef struct _dwg_entity_ATTDEF            dwg_ent_attdef;
typedef struct _dwg_entity_SOLID             dwg_ent_solid;
typedef struct _dwg_entity_TRACE             dwg_ent_trace;
typedef struct _dwg_entity_INSERT            dwg_ent_insert;
typedef struct _dwg_entity_MINSERT           dwg_ent_minsert;
typedef struct _dwg_entity_BLOCK             dwg_ent_block;
typedef struct _dwg_entity_RAY               dwg_ent_ray, 
                                             dwg_ent_xline;
typedef struct _dwg_entity_VERTEX_3D         dwg_ent_vertex_3d, 
                                             dwg_ent_vertex_mesh,
                                             dwg_ent_vertex_pface;
typedef struct _dwg_entity_VERTEX_2D         dwg_ent_vertex_2d;

//-------------------------------------------------------------------------------

typedef struct _dwg_object_LAYER             dwg_obj_layer;
typedef struct _dwg_object_BLOCK_HEADER      dwg_obj_block_header;
typedef struct _dwg_object_BLOCK_CONTROL     dwg_obj_block_control;
typedef struct _dwg_object_MLINESTYLE        dwg_obj_mlinestyle;
typedef struct _dwg_object_APPID             dwg_obj_appid;
typedef struct _dwg_object_APPID_CONTROL     dwg_obj_appid_control;

//-------------------------------------------------------------------------------

// Creates a new circle entity
dwg_ent_circle* dwg_ent_circle_new(int *error);

// Deletes a circle entity
void   dwg_ent_circle_delete(dwg_ent_circle *circle, int *error);

// Initializes a circle with its default values
dwg_ent_circle* dwg_ent_circle_init(dwg_ent_circle *circle);

// Get/Set the center point of a circle
void   dwg_ent_circle_get_center(dwg_ent_circle *circle, dwg_point_3d *point, int *error);
void   dwg_ent_circle_set_center(dwg_ent_circle *circle, dwg_point_3d *point, int *error);

// Get/Set the radius of a circle
double dwg_ent_circle_get_radius(dwg_ent_circle *circle, int *error);
void   dwg_ent_circle_set_radius(dwg_ent_circle *circle, double radius, int *error);

// Get/Set the thickness of a circle
double dwg_ent_circle_get_thickness(dwg_ent_circle *circle, int *error);
void   dwg_ent_circle_set_thickness(dwg_ent_circle *circle, double thickness, int *error);

// Get/Set the extrusion of a circle
void   dwg_ent_circle_set_extrusion(dwg_ent_circle *circle, dwg_point_3d *vector, int *error);
void   dwg_ent_circle_get_extrusion(dwg_ent_circle *circle, dwg_point_3d *vector, int *error);

//-------------------------------------------------------------------------------

// Creates a new line entity
dwg_ent_line* dwg_ent_line_new(int *error);

// Deletes a line entity
void   dwg_ent_line_delete(dwg_ent_line *line, int *error);

// Initializes a line with its default values
dwg_ent_line* dwg_ent_line_init(dwg_ent_line *line);

// Get/Set the start point of a line
void   dwg_ent_line_get_start_point(dwg_ent_line *line, dwg_point_3d *point, int *error);
void   dwg_ent_line_set_start_point(dwg_ent_line *line, dwg_point_3d *point, int *error);

// Get/Set the end point of a line
void   dwg_ent_line_get_end_point(dwg_ent_line *line, dwg_point_3d *point, int *error);
void   dwg_ent_line_set_end_point(dwg_ent_line *line, dwg_point_3d *point, int *error);

// Get/Set the thickness of a line
double dwg_ent_line_get_thickness(dwg_ent_line *line, int *error);
void   dwg_ent_line_set_thickness(dwg_ent_line *line, double thickness, int *error);

// Get/Set the extrusion of a line
void   dwg_ent_line_set_extrusion(dwg_ent_line *line, dwg_point_3d *vector, int *error);
void   dwg_ent_line_get_extrusion(dwg_ent_line *line, dwg_point_3d *vector, int *error);

//-------------------------------------------------------------------------------

// Creates a new arc entity
dwg_ent_arc* dwg_ent_arc_new(int *error);

// Deletes a arc entity
void dwg_ent_arc_delete(dwg_ent_arc *arc, int *error);

// Initializes a arc with its default values
dwg_ent_arc* dwg_ent_arc_init(dwg_ent_arc *arc);

// Get/Set the center point of a arc
void   dwg_ent_arc_get_center(dwg_ent_arc *arc, dwg_point_3d *point, int *error);
void   dwg_ent_arc_set_center(dwg_ent_arc *arc, dwg_point_3d *point, int *error);

// Get/Set the radius of a arc
double dwg_ent_arc_get_radius(dwg_ent_arc *arc, int *error);
void   dwg_ent_arc_set_radius(dwg_ent_arc *arc, double radius, int *error);

// Get/Set the thickness of arc
double dwg_ent_arc_get_thickness(dwg_ent_arc *arc, int *error);
void   dwg_ent_arc_set_thickness(dwg_ent_arc *arc, double thickness, int *error);

//Get/Set the extrusion of arc
void   dwg_ent_arc_get_extrusion(dwg_ent_arc *arc, dwg_point_3d *vector, int *error);
void   dwg_ent_arc_set_extrusion(dwg_ent_arc *arc, dwg_point_3d *vector, int *error);

// Get/set start angle arc
double dwg_ent_arc_get_start_angle(dwg_ent_arc *arc, int *error);
void   dwg_ent_arc_set_start_angle(dwg_ent_arc *arc, double start_angle, int *error);

// Get/set end angle of arc
double dwg_ent_arc_get_end_angle(dwg_ent_arc *arc, int *error);
void   dwg_ent_arc_set_end_angle(dwg_ent_arc *arc, double end_angle, int *error);

//-------------------------------------------------------------------------------

// Creates a new ellipse entity
dwg_ent_ellipse* dwg_ent_ellipse_new(int *error);

// Deletes a ellipse entity
void   dwg_ent_ellipse_delete(dwg_ent_ellipse *ellipse, int *error);

// Initializes a ellipse with its default values
dwg_ent_ellipse* dwg_ent_ellipse_init(dwg_ent_ellipse *ellipse);

// Get/Set center of ellipse
void   dwg_ent_ellipse_get_center(dwg_ent_ellipse *ellipse, dwg_point_3d *point, int *error);
void   dwg_ent_ellipse_set_center(dwg_ent_ellipse *ellipse, dwg_point_3d *point, int *error);

// Get/Set sm axis of ellipse
void   dwg_ent_ellipse_get_sm_axis(dwg_ent_ellipse *ellipse, dwg_point_3d *point, int *error);
void   dwg_ent_ellipse_set_sm_axis(dwg_ent_ellipse *ellipse, dwg_point_3d *point, int *error);

// Get/Set extrusion of ellipse
void   dwg_ent_ellipse_get_extrusion(dwg_ent_ellipse *ellipse, dwg_point_3d *vector, int *error);
void   dwg_ent_ellipse_set_extrusion(dwg_ent_ellipse *ellipse, dwg_point_3d *vector, int *error);

// Get/Set axis ratio of ellipse
double dwg_ent_ellipse_get_axis_ratio(dwg_ent_ellipse *ellipse, int *error);
void   dwg_ent_ellipse_set_axis_ratio(dwg_ent_ellipse *ellipse, double ratio, int *error);

// Get/Set start angle of ellipse
double dwg_ent_ellipse_get_start_angle(dwg_ent_ellipse *ellipse, int *error);
void   dwg_ent_ellipse_set_start_angle(dwg_ent_ellipse *ellipse, double start_angle, int *error);

// Get/Set end angle of ellipse
double dwg_ent_ellipse_get_end_angle(dwg_ent_ellipse *ellipse, int *error);
void   dwg_ent_ellipse_set_end_angle(dwg_ent_ellipse *ellipse, double end_angle, int *error);

//-------------------------------------------------------------------------------

// Get/Set text of text
void   dwg_ent_text_set_text(dwg_ent_text *text, char * text_value, int *error);
char * dwg_ent_text_get_text(dwg_ent_text *text, int *error);

// Get/Set insertion points of text
void   dwg_ent_text_get_insertion_point(dwg_ent_text *text, dwg_point_2d *point, int *error);
void   dwg_ent_text_set_insertion_point(dwg_ent_text *text, dwg_point_2d *point, int *error);

//Get/Set height of text
double dwg_ent_text_get_height(dwg_ent_text *text, int *error);
void   dwg_ent_text_set_height(dwg_ent_text *text, double height, int *error);

//Get/Set extrusion of text
void   dwg_ent_text_get_extrusion(dwg_ent_text *text, dwg_point_3d *vector, int *error);
void   dwg_ent_text_set_extrusion(dwg_ent_text *text, dwg_point_3d *vector, int *error);

//Get/Set thickness of text
double dwg_ent_text_get_thickness(dwg_ent_text *text, int *error);
void   dwg_ent_text_set_thickness(dwg_ent_text *text, double thickness, int *error);

//Get/Set rotation angle of text
double dwg_ent_text_get_rot_angle(dwg_ent_text *text, int *error);
void   dwg_ent_text_set_rot_angle(dwg_ent_text *text, double angle, int *error);

//Get/Set horizontal alignment angle of text
double dwg_ent_text_get_vert_align(dwg_ent_text *text, int *error);
void   dwg_ent_text_set_vert_align(dwg_ent_text *text, double alignment, int *error);

//Get/Set vertical alignment of text
double dwg_ent_text_get_horiz_align(dwg_ent_text *text, int *error);
void   dwg_ent_text_set_horiz_align(dwg_ent_text *text, double alignment, int *error);

//-------------------------------------------------------------------------------

// Get/Set text of attrib
void   dwg_ent_attrib_set_text(dwg_ent_attrib *attrib, char * text_value, int *error);
char * dwg_ent_attrib_get_text(dwg_ent_attrib *attrib, int *error);

// Get/Set insertion points of attrib
void   dwg_ent_attrib_get_insertion_point(dwg_ent_attrib *attrib, dwg_point_2d *point, int *error);
void   dwg_ent_attrib_set_insertion_point(dwg_ent_attrib *attrib, dwg_point_2d *point, int *error);

//Get/Set height of attrib
double dwg_ent_attrib_get_height(dwg_ent_attrib *attrib, int *error);
void   dwg_ent_attrib_set_height(dwg_ent_attrib *attrib, double height, int *error);

//Get/Set extrusion of attrib
void   dwg_ent_attrib_get_extrusion(dwg_ent_attrib *attrib, dwg_point_3d *vector, int *error);
void   dwg_ent_attrib_set_extrusion(dwg_ent_attrib *attrib, dwg_point_3d *vector, int *error);

//Get/Set thickness of attrib
double dwg_ent_attrib_get_thickness(dwg_ent_attrib *attrib, int *error);
void   dwg_ent_attrib_set_thickness(dwg_ent_attrib *attrib, double thickness, int *error);

//Get/Set rotation angle of attrib
double dwg_ent_attrib_get_rot_angle(dwg_ent_attrib *attrib, int *error);
void   dwg_ent_attrib_set_rot_angle(dwg_ent_attrib *attrib, double angle, int *error);

//Get/Set horizontal alignment angle of attrib
double dwg_ent_attrib_get_vert_align(dwg_ent_attrib *attrib, int *error);
void   dwg_ent_attrib_set_vert_align(dwg_ent_attrib *attrib, double alignment, int *error);

//Get/Set vertical alignment of attrib
double dwg_ent_attrib_get_horiz_align(dwg_ent_attrib *attrib, int *error);
void   dwg_ent_attrib_set_horiz_align(dwg_ent_attrib *attrib, double alignment, int *error);

//-------------------------------------------------------------------------------

// Get/Set text of attdef
void   dwg_ent_attdef_set_text(dwg_ent_attdef *attdef, char * default_value, int *error);
char * dwg_ent_attdef_get_text(dwg_ent_attdef *attdef, int *error);

// Get/Set insertion points of attdef
void   dwg_ent_attdef_get_insertion_point(dwg_ent_attdef *attdef, dwg_point_2d *point, int *error);
void   dwg_ent_attdef_set_insertion_point(dwg_ent_attdef *attdef, dwg_point_2d *point, int *error);

//Get/Set height of attdef
double dwg_ent_attdef_get_height(dwg_ent_attdef *attdef, int *error);
void   dwg_ent_attdef_set_height(dwg_ent_attdef *attdef, double height, int *error);

//Get/Set extrusion of attdef
void   dwg_ent_attdef_get_extrusion(dwg_ent_attdef *attdef, dwg_point_3d *vector, int *error);
void   dwg_ent_attdef_set_extrusion(dwg_ent_attdef *attdef, dwg_point_3d *vector, int *error);

//Get/Set thickness of attdef
double dwg_ent_attdef_get_thickness(dwg_ent_attdef *attdef, int *error);
void   dwg_ent_attdef_set_thickness(dwg_ent_attdef *attdef, double thickness, int *error);

//Get/Set rotation angle of attdef
double dwg_ent_attdef_get_rot_angle(dwg_ent_attdef *attdef, int *error);
void   dwg_ent_attdef_set_rot_angle(dwg_ent_attdef *attdef, double angle, int *error);

//Get/Set horizontal alignment angle of attdef
double dwg_ent_attdef_get_vert_align(dwg_ent_attdef *attdef, int *error);
void   dwg_ent_attdef_set_vert_align(dwg_ent_attdef *attdef, double alignment, int *error);

//Get/Set vertical alignment of attdef
double dwg_ent_attdef_get_horiz_align(dwg_ent_attdef *attdef, int *error);
void   dwg_ent_attdef_set_horiz_align(dwg_ent_attdef *attdef, double alignment, int *error);

//--------------------------------------------------------------------------------

void   dwg_ent_point_set_point(dwg_ent_point *point, dwg_point_3d *retpoint, int *error);
void   dwg_ent_point_get_point(dwg_ent_point *point, dwg_point_3d *retpoint, int *error);

double dwg_ent_point_get_thickness(dwg_ent_point *point, int *error);
void   dwg_ent_point_set_thickness(dwg_ent_point *point, double thickness, int *error);

void   dwg_ent_point_set_extrusion(dwg_ent_point *point, dwg_point_3d *retpoint, int *error);
void   dwg_ent_point_get_extrusion(dwg_ent_point *point, dwg_point_3d *retpoint, int *error);

//---------------------------------------------------------------------------------

double dwg_ent_solid_get_thickness(dwg_ent_solid *solid, int *error);
void   dwg_ent_solid_set_thickness(dwg_ent_solid *solid, double thickness, int *error);

double dwg_ent_solid_get_elevation(dwg_ent_solid *solid, int *error);
void   dwg_ent_solid_set_elevation(dwg_ent_solid *solid, double elevation, int *error);

void   dwg_ent_solid_get_corner1(dwg_ent_solid *solid, dwg_point_2d *point, int *error);
void   dwg_ent_solid_set_corner1(dwg_ent_solid *solid, dwg_point_2d *point, int *error);

void   dwg_ent_solid_get_corner2(dwg_ent_solid *solid, dwg_point_2d *point, int *error);
void   dwg_ent_solid_set_corner2(dwg_ent_solid *solid, dwg_point_2d *point, int *error);

void   dwg_ent_solid_get_corner3(dwg_ent_solid *solid, dwg_point_2d *point, int *error);
void   dwg_ent_solid_set_corner3(dwg_ent_solid *solid, dwg_point_2d *point, int *error);

void   dwg_ent_solid_get_corner4(dwg_ent_solid *solid, dwg_point_2d *point, int *error);
void   dwg_ent_solid_set_corner4(dwg_ent_solid *solid, dwg_point_2d *point, int *error);

void   dwg_ent_solid_get_extrusion(dwg_ent_solid *solid, dwg_point_3d *vector, int *error);
void   dwg_ent_solid_set_extrusion(dwg_ent_solid *solid, dwg_point_3d *vector, int *error);

//--------------------------------------------------------------------------------

// Get/Set text of block
void   dwg_ent_block_set_name(dwg_ent_block *block, char * name, int *error);
char * dwg_ent_block_get_name(dwg_ent_block *block, int *error);

//--------------------------------------------------------------------------------

void   dwg_ent_ray_get_point(dwg_ent_ray *ray, dwg_point_3d *point, int *error);
void   dwg_ent_ray_set_point(dwg_ent_ray *ray, dwg_point_3d *point, int *error);

void   dwg_ent_ray_get_vector(dwg_ent_ray *ray, dwg_point_3d *vector, int *error);
void   dwg_ent_ray_set_vector(dwg_ent_ray *ray, dwg_point_3d *vector, int *error);

//--------------------------------------------------------------------------------

void   dwg_ent_xline_get_point(dwg_ent_xline *xline, dwg_point_3d *point, int *error);
void   dwg_ent_xline_set_point(dwg_ent_xline *xline, dwg_point_3d *point, int *error);

void   dwg_ent_xline_get_vector(dwg_ent_xline *xline, dwg_point_3d *vector, int *error);
void   dwg_ent_xline_set_vector(dwg_ent_xline *xline, dwg_point_3d *vector, int *error);

//--------------------------------------------------------------------------------

double dwg_ent_trace_get_thickness(dwg_ent_trace *trace, int *error);
void   dwg_ent_trace_set_thickness(dwg_ent_trace *trace, double thickness, int *error);

double dwg_ent_trace_get_elevation(dwg_ent_trace *trace, int *error);
void   dwg_ent_trace_set_elevation(dwg_ent_trace *trace, double elevation, int *error);

void   dwg_ent_trace_get_corner1(dwg_ent_trace *trace, dwg_point_2d *point, int *error);
void   dwg_ent_trace_set_corner1(dwg_ent_trace *trace, dwg_point_2d *point, int *error);

void   dwg_ent_trace_get_corner2(dwg_ent_trace *trace, dwg_point_2d *point, int *error);
void   dwg_ent_trace_set_corner2(dwg_ent_trace *trace, dwg_point_2d *point, int *error);

void   dwg_ent_trace_get_corner3(dwg_ent_trace *trace, dwg_point_2d *point, int *error);
void   dwg_ent_trace_set_corner3(dwg_ent_trace *trace, dwg_point_2d *point, int *error);

void   dwg_ent_trace_get_corner4(dwg_ent_trace *trace, dwg_point_2d *point, int *error);
void   dwg_ent_trace_set_corner4(dwg_ent_trace *trace, dwg_point_2d *point, int *error);

void   dwg_ent_trace_get_extrusion(dwg_ent_trace *trace, dwg_point_3d *vector, int *error);
void   dwg_ent_trace_set_extrusion(dwg_ent_trace *trace, dwg_point_3d *vector, int *error);

//--------------------------------------------------------------------------------

char   dwg_ent_vertex_3d_get_flags(dwg_ent_vertex_3d *vert, int *error);
void   dwg_ent_vertex_3d_set_flags(dwg_ent_vertex_3d *vert, char flags, int *error);

void   dwg_ent_vertex_3d_get_point(dwg_ent_vertex_3d *vert, dwg_point_3d *point, int *error);
void   dwg_ent_vertex_3d_set_point(dwg_ent_vertex_3d *vert, dwg_point_3d *point, int *error);

//--------------------------------------------------------------------------------

char   dwg_ent_vertex_mesh_get_flags(dwg_ent_vertex_mesh *vert, int *error);
void   dwg_ent_vertex_mesh_set_flags(dwg_ent_vertex_mesh *vert, char flags, int *error);

void   dwg_ent_vertex_mesh_get_point(dwg_ent_vertex_mesh *vert, dwg_point_3d *point, int *error);
void   dwg_ent_vertex_mesh_set_point(dwg_ent_vertex_mesh *vert, dwg_point_3d *point, int *error);

//--------------------------------------------------------------------------------

char   dwg_ent_vertex_pface_get_flags(dwg_ent_vertex_pface *vert, int *error);
void   dwg_ent_vertex_pface_set_flags(dwg_ent_vertex_pface *vert, char flags, int *error);

void   dwg_ent_vertex_pface_get_point(dwg_ent_vertex_pface *vert, dwg_point_3d *point, int *error);
void   dwg_ent_vertex_pface_set_point(dwg_ent_vertex_pface *vert, dwg_point_3d *point, int *error);

//--------------------------------------------------------------------------------

char   dwg_ent_vertex_2d_get_flags(dwg_ent_vertex_2d *vert, int *error);
void   dwg_ent_vertex_2d_set_flags(dwg_ent_vertex_2d *vert, char flags, int *error);

void   dwg_ent_vertex_2d_get_point(dwg_ent_vertex_2d *vert, dwg_point_3d *point, int *error);
void   dwg_ent_vertex_2d_set_point(dwg_ent_vertex_2d *vert, dwg_point_3d *point, int *error);

double dwg_ent_vertex_2d_get_start_width(dwg_ent_vertex_2d *vert, int *error);
void   dwg_ent_vertex_2d_set_start_width(dwg_ent_vertex_2d *vert, double start_width, int *error);

double dwg_ent_vertex_2d_get_end_width(dwg_ent_vertex_2d *vert, int *error);
void   dwg_ent_vertex_2d_set_end_width(dwg_ent_vertex_2d *vert, double end_width, int *error);

double dwg_ent_vertex_2d_get_bulge(dwg_ent_vertex_2d *vert, int *error);
void   dwg_ent_vertex_2d_set_bulge(dwg_ent_vertex_2d *vert, double bulge, int *error);

double dwg_ent_vertex_2d_get_tangent_dir(dwg_ent_vertex_2d *vert, int *error);
void   dwg_ent_vertex_2d_set_tangent_dir(dwg_ent_vertex_2d *vert, double tangent_dir, int *error);

//--------------------------------------------------------------------------------

void   dwg_ent_insert_get_ins_pt(dwg_ent_insert *insert, dwg_point_3d *point, int *error);
void   dwg_ent_insert_set_ins_pt(dwg_ent_insert *insert, dwg_point_3d *point, int *error);

char   dwg_ent_insert_get_scale_flag(dwg_ent_insert *insert, int *error);
void   dwg_ent_insert_set_scale_flag(dwg_ent_insert *insert, char flags, int *error);

void   dwg_ent_insert_get_scale(dwg_ent_insert *insert, dwg_point_3d *point, int *error);
void   dwg_ent_insert_set_scale(dwg_ent_insert *insert, dwg_point_3d *point, int *error);

double dwg_ent_insert_get_rotation_angle(dwg_ent_insert *insert, int *error);
void   dwg_ent_insert_set_rotation_angle(dwg_ent_insert *insert, double rot_ang, int *error);

void   dwg_ent_insert_get_extrusion(dwg_ent_insert *insert, dwg_point_3d *point, int *error);
void   dwg_ent_insert_set_extrusion(dwg_ent_insert *insert, dwg_point_3d *point, int *error);

char   dwg_ent_insert_get_has_attribs(dwg_ent_insert *insert, int *error);
void   dwg_ent_insert_set_has_attribs(dwg_ent_insert *insert, char attribs, int *error);

long   dwg_ent_insert_get_owned_obj_count(dwg_ent_insert *insert, int *error);
void   dwg_ent_insert_set_owned_obj_count(dwg_ent_insert *insert, long count, int *error);

//--------------------------------------------------------------------------------

void   dwg_ent_minsert_get_ins_pt(dwg_ent_minsert *minsert, dwg_point_3d *point, int *error);
void   dwg_ent_minsert_set_ins_pt(dwg_ent_minsert *minsert, dwg_point_3d *point, int *error);

char   dwg_ent_minsert_get_scale_flag(dwg_ent_minsert *minsert, int *error);
void   dwg_ent_minsert_set_scale_flag(dwg_ent_minsert *minsert, char flags, int *error);

void   dwg_ent_minsert_get_scale(dwg_ent_minsert *minsert, dwg_point_3d *point, int *error);
void   dwg_ent_minsert_set_scale(dwg_ent_minsert *minsert, dwg_point_3d *point, int *error);

double dwg_ent_minsert_get_rotation_angle(dwg_ent_minsert *minsert, int *error);
void   dwg_ent_minsert_set_rotation_angle(dwg_ent_minsert *minsert, double rot_ang, int *error);

void   dwg_ent_minsert_get_extrusion(dwg_ent_minsert *minsert, dwg_point_3d *point, int *error);
void   dwg_ent_minsert_set_extrusion(dwg_ent_minsert *minsert, dwg_point_3d *point, int *error);

char   dwg_ent_minsert_get_has_attribs(dwg_ent_minsert *minsert, int *error);
void   dwg_ent_minsert_set_has_attribs(dwg_ent_minsert *minsert, char attribs, int *error);

long   dwg_ent_minsert_get_owned_obj_count(dwg_ent_minsert *minsert, int *error);
void   dwg_ent_minsert_set_owned_obj_count(dwg_ent_minsert *minsert, long count, int *error);

long   dwg_ent_minsert_get_numcols(dwg_ent_minsert *minsert, int *error);
void   dwg_ent_minsert_set_numcols(dwg_ent_minsert *minsert, long cols, int *error);

long   dwg_ent_minsert_get_numrows(dwg_ent_minsert *minsert, int *error);
void   dwg_ent_minsert_set_numrows(dwg_ent_minsert *minsert, long cols, int *error);

double dwg_ent_minsert_get_col_spacing(dwg_ent_minsert *minsert, int *error);
void   dwg_ent_minsert_set_col_spacing(dwg_ent_minsert *minsert, double spacing, int *error);

double dwg_ent_minsert_get_row_spacing(dwg_ent_minsert *minsert, int *error);
void   dwg_ent_minsert_set_row_spacing(dwg_ent_minsert *minsert, double spacing, int *error);

//--------------------------------------------------------------------------------

char * dwg_obj_mlinstyle_get_name(dwg_obj_mlinestyle *mlinestyle, int *error);
void   dwg_obj_mlinestyle_set_name(dwg_obj_mlinestyle *mlinestyle, char * name, int *error);

char * dwg_obj_mlinestyle_get_desc(dwg_obj_mlinestyle *mlinestyle, int *error);
void   dwg_obj_mlinestyle_set_desc(dwg_obj_mlinestyle *mlinestyle, char * desc, int *error);

int    dwg_obj_mlinestyle_get_flags(dwg_obj_mlinestyle *mlinestyle, int *error);
void   dwg_obj_mlinestyle_set_flags(dwg_obj_mlinestyle *mlinestyle, int flags, int *error);

double dwg_obj_mlinestyle_get_start_angle(dwg_obj_mlinestyle *mlinestyle, int *error);
void   dwg_obj_mlinestyle_set_start_angle(dwg_obj_mlinestyle *mlinestyle, double startang, int *error);

double dwg_obj_mlinestyle_get_end_angle(dwg_obj_mlinestyle *mlinestyle, int *error);
void   dwg_obj_mlinestyle_set_end_angle(dwg_obj_mlinestyle *mlinestyle, double endang, int *error);

char   dwg_obj_mlinestyle_get_linesinstyle(dwg_obj_mlinestyle *mlinestyle, int *error);
void   dwg_obj_mlinestyle_set_linesinstyle(dwg_obj_mlinestyle *mlinestyle, char linesinstyle, int *error);

//--------------------------------------------------------------------------------

int    dwg_obj_appid_control_get_num_entries(dwg_obj_appid_control *appid, int *error);
void   dwg_obj_appid_control_set_num_entries(dwg_obj_appid_control *appid, int entries, int *error);

//--------------------------------------------------------------------------------

char * dwg_obj_appid_get_entry_name(dwg_obj_appid *appid, int *error);
void   dwg_obj_appid_set_entry_name(dwg_obj_appid *appid, char * entry_name, int *error);

char   dwg_obj_appid_get_flag(dwg_obj_appid *appid, int *error);
void   dwg_obj_appid_set_flag(dwg_obj_appid *appid, char flag, int *error);

dwg_obj_appid_control dwg_obj_appid_get_appid_control(dwg_obj_appid *appid, int *error);

//--------------------------------------------------------------------------------



//--------------------------------------------------------------------------------

dwg_obj_block_header * dwg_get_block_header(Dwg_Data *dwg, int *error);

//--------------------------------------------------------------------------------

dwg_obj_block_control* dwg_block_header_get_block_control(dwg_obj_block_header* block_header, int *error);

//--------------------------------------------------------------------------------

// Get Layer Name
char * dwg_obj_layer_get_name(dwg_obj_layer *layer, int *error);

//--------------------------------------------------------------------------------

// Get Block Name
char * dwg_obj_block_header_get_name(dwg_obj_block_header *hdr, int *error);

#endif
