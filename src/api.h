
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

typedef struct dwg_lwpline_widths
{
  double start;
  double end;
} dwg_lwpline_widths;

// Extract All Entities of a specific type from a BLOCK
#define GET_DWG_ENTITY(token) \
Dwg_Entity_##token ** \
 dwg_get_##token (Dwg_Object_Ref * ref) \
{ \
  int x=0,counts=0; \
  Dwg_Object * obj; \
  Dwg_Object_BLOCK_HEADER *hdr; \
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
#define CAST_DWG_OBJECT_TO_ENTITY(token) \
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

#define CAST_DWG_OBJECT_TO_OBJECT(token) \
Dwg_Object_##token * \
dwg_object_to_##token(Dwg_Object *obj) \
{ \
  Dwg_Object_##token *ret_obj; \
    if(obj != 0 && obj->type == DWG_TYPE_##token) \
      { \
        ret_obj = obj->tio.object->tio.token; \
      } \
  return ret_obj; \
}

///////////////////////////////////////////////////////////////////////////

typedef struct _dwg_entity_CIRCLE                 dwg_ent_circle;
typedef struct _dwg_entity_LINE                   dwg_ent_line;
typedef struct _dwg_entity_POLYLINE_3D            dwg_ent_polyline_3d;
typedef struct _dwg_entity_POLYLINE_2D            dwg_ent_polyline_2d;
typedef struct _dwg_entity_POLYLINE_MESH          dwg_ent_polyline_mesh;
typedef struct _dwg_entity_POLYLINE_PFACE         dwg_ent_polyline_pface;
typedef struct _dwg_entity_LWPLINE                dwg_ent_lwpline;
typedef struct _dwg_entity_ARC                    dwg_ent_arc;
typedef struct _dwg_entity_ELLIPSE                dwg_ent_ellipse;
typedef struct _dwg_entity_TEXT                   dwg_ent_text;
typedef struct _dwg_entity_POINT                  dwg_ent_point;
typedef struct _dwg_entity_ATTRIB                 dwg_ent_attrib;
typedef struct _dwg_entity_ATTDEF                 dwg_ent_attdef;
typedef struct _dwg_entity_SOLID                  dwg_ent_solid;
typedef struct _dwg_entity_TRACE                  dwg_ent_trace;
typedef struct _dwg_entity_3D_FACE                dwg_ent_3dface;
typedef struct _dwg_entity_INSERT                 dwg_ent_insert;
typedef struct _dwg_entity_MINSERT                dwg_ent_minsert;
typedef struct _dwg_entity_BLOCK                  dwg_ent_block;
typedef struct _dwg_entity_IMAGE                  dwg_ent_image;
typedef struct _dwg_entity_IMAGE_clip_vert        dwg_ent_image_clip_vert;
typedef struct _dwg_entity_MLINE_vert             dwg_ent_mline_vert;
typedef struct _dwg_entity_MLINE                  dwg_ent_mline;
typedef struct _dwg_entity_RAY                    dwg_ent_ray, 
                                                  dwg_ent_xline;
typedef struct _dwg_entity_VERTEX_3D              dwg_ent_vertex_3d, 
                                                  dwg_ent_vertex_mesh,
                                                  dwg_ent_vertex_pface;
typedef struct _dwg_entity_VERTEX_2D              dwg_ent_vertex_2d;
typedef struct _dwg_entity_DIMENSION_ORDINATE     dwg_ent_dim_ordinate;
typedef struct _dwg_entity_DIMENSION_LINEAR       dwg_ent_dim_linear;
typedef struct _dwg_entity_DIMENSION_ALIGNED      dwg_ent_dim_aligned;
typedef struct _dwg_entity_DIMENSION_ANG3PT       dwg_ent_dim_ang3pt;
typedef struct _dwg_entity_DIMENSION_ANG2LN       dwg_ent_dim_ang2ln;
typedef struct _dwg_entity_DIMENSION_RADIUS       dwg_ent_dim_radius;
typedef struct _dwg_entity_DIMENSION_DIAMETER     dwg_ent_dim_diameter;
typedef struct _dwg_entity_LEADER                 dwg_ent_leader;
typedef struct _dwg_entity_SHAPE                  dwg_ent_shape;
typedef struct _dwg_entity_MTEXT                  dwg_ent_mtext;
typedef struct _dwg_entity_TOLERANCE              dwg_ent_tolerance;
typedef struct _dwg_entity_ENDBLK                 dwg_ent_endblk;
typedef struct _dwg_entity_SEQEND                 dwg_ent_seqend;
typedef struct _dwg_entity_SPLINE_point           dwg_ent_spline_point;
typedef struct _dwg_entity_SPLINE                 dwg_ent_spline;
typedef struct _dwg_entity_SPLINE_control_point   dwg_ent_spline_control_point;
typedef struct _dwg_entity_OLE2FRAME              dwg_ent_ole2frame;
typedef struct _dwg_entity_VIEWPORT               dwg_ent_viewport;
typedef struct _dwg_entity_3DSOLID                dwg_ent_3dsolid,
                                                  dwg_ent_region,
                                                  dwg_ent_body;
typedef struct _dwg_entity_3DSOLID_wire           dwg_ent_solid_wire;
typedef struct _dwg_entity_3DSOLID_silhouette     dwg_ent_solid_silhouette;
typedef struct _dwg_entity_TABLE                  dwg_ent_table;
typedef struct _dwg_entity_HATCH                  dwg_ent_hatch;
typedef struct _dwg_entity_VERTEX_PFACE_FACE      dwg_ent_vert_pface_face;

///////////////////////////////////////////////////////////////////////////

typedef struct _dwg_object_LAYER                  dwg_obj_layer;
typedef struct _dwg_object_BLOCK_HEADER           dwg_obj_block_header;
typedef struct _dwg_object_BLOCK_CONTROL          dwg_obj_block_control;
typedef struct _dwg_object_MLINESTYLE             dwg_obj_mlinestyle;
typedef struct _dwg_object_APPID                  dwg_obj_appid;
typedef struct _dwg_object_APPID_CONTROL          dwg_obj_appid_control;
typedef struct _dwg_object                        dwg_object;
typedef struct _dwg_object_ref                    dwg_object_ref;
typedef struct _dwg_handle                        dwg_handle;
typedef struct _dwg_object_entity                 dwg_obj_ent;
typedef struct _dwg_object_object                 dwg_obj_obj;
typedef struct _dwg_struct                        dwg_data;
typedef struct _dwg_object_ref		          dwg_object_ref;
///////////////////////////////////////////////////////////////////////////


/********************************************************************
*                FUNCTIONS START HERE ENTITY SPECIFIC               *
********************************************************************/


/********************************************************************
*                    FUNCTIONS FOR CIRCLE ENTITY                    *
********************************************************************/


// Creates a new circle entity
dwg_ent_circle*
dwg_ent_circle_new(int *error);

// Deletes a circle entity
void
dwg_ent_circle_delete(dwg_ent_circle *circle, int *error);

// Initializes a circle with its default values
dwg_ent_circle *
dwg_ent_circle_init(dwg_ent_circle *circle);

// Get/Set the center point of a circle
void
dwg_ent_circle_get_center(dwg_ent_circle *circle, dwg_point_3d *point,
                          int *error);

void
dwg_ent_circle_set_center(dwg_ent_circle *circle, dwg_point_3d *point,
                          int *error);

// Get/Set the radius of a circle
double
dwg_ent_circle_get_radius(dwg_ent_circle *circle, int *error);

void
dwg_ent_circle_set_radius(dwg_ent_circle *circle, double radius,
                          int *error);

// Get/Set the thickness of a circle
double
dwg_ent_circle_get_thickness(dwg_ent_circle *circle, int *error);

void
dwg_ent_circle_set_thickness(dwg_ent_circle *circle, double thickness,
                             int *error);

// Get/Set the extrusion of a circle
void
dwg_ent_circle_set_extrusion(dwg_ent_circle *circle,
                             dwg_point_3d *vector, int *error);

void
dwg_ent_circle_get_extrusion(dwg_ent_circle *circle,
                             dwg_point_3d *vector, int *error);


/********************************************************************
*                    FUNCTIONS FOR LINE ENTITY                      *
********************************************************************/


// Creates a new line entity
dwg_ent_line *
dwg_ent_line_new(int *error);

// Deletes a line entity
void
dwg_ent_line_delete(dwg_ent_line *line, int *error);

// Initializes a line with its default values
dwg_ent_line *
dwg_ent_line_init(dwg_ent_line *line);

// Get/Set the start point of a line
void
dwg_ent_line_get_start_point(dwg_ent_line *line, dwg_point_3d *point,
                             int *error);

void
dwg_ent_line_set_start_point(dwg_ent_line *line, dwg_point_3d *point,
                             int *error);

// Get/Set the end point of a line
void
dwg_ent_line_get_end_point(dwg_ent_line *line, dwg_point_3d *point,
                           int *error);

void
dwg_ent_line_set_end_point(dwg_ent_line *line, dwg_point_3d *point,
                           int *error);

// Get/Set the thickness of a line
double
dwg_ent_line_get_thickness(dwg_ent_line *line, int *error);

void
dwg_ent_line_set_thickness(dwg_ent_line *line, double thickness, int *error);

// Get/Set the extrusion of a line
void
dwg_ent_line_set_extrusion(dwg_ent_line *line, dwg_point_3d *vector,
                           int *error);

void
dwg_ent_line_get_extrusion(dwg_ent_line *line, dwg_point_3d *vector,
                           int *error);


/********************************************************************
*                    FUNCTIONS FOR ARC ENTITY                       *
********************************************************************/


// Creates a new arc entity
dwg_ent_arc* dwg_ent_arc_new(int *error);

// Deletes a arc entity
void
dwg_ent_arc_delete(dwg_ent_arc *arc, int *error);

// Initializes a arc with its default values
dwg_ent_arc* dwg_ent_arc_init(dwg_ent_arc *arc);

// Get/Set the center point of a arc
void
dwg_ent_arc_get_center(dwg_ent_arc *arc, dwg_point_3d *point, int *error);

void
dwg_ent_arc_set_center(dwg_ent_arc *arc, dwg_point_3d *point, int *error);

// Get/Set the radius of a arc
double
dwg_ent_arc_get_radius(dwg_ent_arc *arc, int *error);

void
dwg_ent_arc_set_radius(dwg_ent_arc *arc, double radius, int *error);

// Get/Set the thickness of arc
double
dwg_ent_arc_get_thickness(dwg_ent_arc *arc, int *error);

void
dwg_ent_arc_set_thickness(dwg_ent_arc *arc, double thickness, int *error);

//Get/Set the extrusion of arc
void
dwg_ent_arc_get_extrusion(dwg_ent_arc *arc, dwg_point_3d *vector, int *error);

void
dwg_ent_arc_set_extrusion(dwg_ent_arc *arc, dwg_point_3d *vector, int *error);

// Get/set start angle arc
double
dwg_ent_arc_get_start_angle(dwg_ent_arc *arc, int *error);

void
dwg_ent_arc_set_start_angle(dwg_ent_arc *arc, double start_angle, int *error);

// Get/set end angle of arc
double
dwg_ent_arc_get_end_angle(dwg_ent_arc *arc, int *error);

void
dwg_ent_arc_set_end_angle(dwg_ent_arc *arc, double end_angle, int *error);


/********************************************************************
*                   FUNCTIONS FOR ELLIPSE ENTITY                    *
********************************************************************/


// Creates a new ellipse entity
dwg_ent_ellipse *
dwg_ent_ellipse_new(int *error);

// Deletes a ellipse entity
void
dwg_ent_ellipse_delete(dwg_ent_ellipse *ellipse, int *error);

// Initializes a ellipse with its default values
dwg_ent_ellipse *
dwg_ent_ellipse_init(dwg_ent_ellipse *ellipse);

// Get/Set center of ellipse
void
dwg_ent_ellipse_get_center(dwg_ent_ellipse *ellipse, dwg_point_3d *point,
                           int *error);

void
dwg_ent_ellipse_set_center(dwg_ent_ellipse *ellipse, dwg_point_3d *point,
                           int *error);

// Get/Set sm axis of ellipse
void
dwg_ent_ellipse_get_sm_axis(dwg_ent_ellipse *ellipse, dwg_point_3d *point,
                            int *error);

void
dwg_ent_ellipse_set_sm_axis(dwg_ent_ellipse *ellipse, dwg_point_3d *point,
                            int *error);

// Get/Set extrusion of ellipse
void
dwg_ent_ellipse_get_extrusion(dwg_ent_ellipse *ellipse, dwg_point_3d *vector,
                              int *error);

void
dwg_ent_ellipse_set_extrusion(dwg_ent_ellipse *ellipse, dwg_point_3d *vector,
                              int *error);

// Get/Set axis ratio of ellipse
double
dwg_ent_ellipse_get_axis_ratio(dwg_ent_ellipse *ellipse, int *error);

void
dwg_ent_ellipse_set_axis_ratio(dwg_ent_ellipse *ellipse, double ratio,
                               int *error);

// Get/Set start angle of ellipse
double
dwg_ent_ellipse_get_start_angle(dwg_ent_ellipse *ellipse, int *error);

void
dwg_ent_ellipse_set_start_angle(dwg_ent_ellipse *ellipse, double start_angle,
                                int *error);

// Get/Set end angle of ellipse
double
dwg_ent_ellipse_get_end_angle(dwg_ent_ellipse *ellipse, int *error);

void
dwg_ent_ellipse_set_end_angle(dwg_ent_ellipse *ellipse, double end_angle,
                              int *error);


/********************************************************************
*                    FUNCTIONS FOR TEXT ENTITY                      *
********************************************************************/


// Get/Set text of text
void
dwg_ent_text_set_text(dwg_ent_text *text, char *text_value, int *error);

char *
dwg_ent_text_get_text(dwg_ent_text *text, int *error);

// Get/Set insertion points of text
void
dwg_ent_text_get_insertion_point(dwg_ent_text *text, dwg_point_2d *point,
                                 int *error);

void
dwg_ent_text_set_insertion_point(dwg_ent_text *text, dwg_point_2d *point,
                                 int *error);

//Get/Set height of text
double
dwg_ent_text_get_height(dwg_ent_text *text, int *error);

void
dwg_ent_text_set_height(dwg_ent_text *text, double height, int *error);

//Get/Set extrusion of text
void
dwg_ent_text_get_extrusion(dwg_ent_text *text, dwg_point_3d *vector,
                           int *error);

void
dwg_ent_text_set_extrusion(dwg_ent_text *text, dwg_point_3d *vector,
                           int *error);

//Get/Set thickness of text
double
dwg_ent_text_get_thickness(dwg_ent_text *text, int *error);

void
dwg_ent_text_set_thickness(dwg_ent_text *text, double thickness, int *error);

//Get/Set rotation angle of text
double
dwg_ent_text_get_rot_angle(dwg_ent_text *text, int *error);

void
dwg_ent_text_set_rot_angle(dwg_ent_text *text, double angle, int *error);

//Get/Set horizontal alignment angle of text
double
dwg_ent_text_get_vert_align(dwg_ent_text *text, int *error);

void
dwg_ent_text_set_vert_align(dwg_ent_text *text, double alignment, int *error);

//Get/Set vertical alignment of text
double
dwg_ent_text_get_horiz_align(dwg_ent_text *text, int *error);

void
dwg_ent_text_set_horiz_align(dwg_ent_text *text, double alignment, int *error);


/********************************************************************
*                   FUNCTIONS FOR ATTRIB ENTITY                     *
********************************************************************/


// Get/Set text of attrib
void
dwg_ent_attrib_set_text(dwg_ent_attrib *attrib, char *text_value, int *error);

char *
dwg_ent_attrib_get_text(dwg_ent_attrib *attrib, int *error);

// Get/Set insertion points of attrib
void
dwg_ent_attrib_get_insertion_point(dwg_ent_attrib *attrib, dwg_point_2d *point,
                                   int *error);

void
dwg_ent_attrib_set_insertion_point(dwg_ent_attrib *attrib, dwg_point_2d *point,
                                   int *error);

//Get/Set height of attrib
double
dwg_ent_attrib_get_height(dwg_ent_attrib *attrib, int *error);

void
dwg_ent_attrib_set_height(dwg_ent_attrib *attrib, double height, int *error);

//Get/Set extrusion of attrib
void
dwg_ent_attrib_get_extrusion(dwg_ent_attrib *attrib, dwg_point_3d *vector,
                             int *error);

void
dwg_ent_attrib_set_extrusion(dwg_ent_attrib *attrib, dwg_point_3d *vector,
                             int *error);

//Get/Set thickness of attrib
double
dwg_ent_attrib_get_thickness(dwg_ent_attrib *attrib, int *error);

void
dwg_ent_attrib_set_thickness(dwg_ent_attrib *attrib, double thickness,
                             int *error);

//Get/Set rotation angle of attrib
double
dwg_ent_attrib_get_rot_angle(dwg_ent_attrib *attrib, int *error);

void
dwg_ent_attrib_set_rot_angle(dwg_ent_attrib *attrib, double angle,
                             int *error);

//Get/Set horizontal alignment angle of attrib
double
dwg_ent_attrib_get_vert_align(dwg_ent_attrib *attrib, int *error);

void
dwg_ent_attrib_set_vert_align(dwg_ent_attrib *attrib, double alignment,
                              int *error);

//Get/Set vertical alignment of attrib
double
dwg_ent_attrib_get_horiz_align(dwg_ent_attrib *attrib, int *error);

void
dwg_ent_attrib_set_horiz_align(dwg_ent_attrib *attrib, double alignment,
                               int *error);


/********************************************************************
*                   FUNCTIONS FOR ATTDEF ENTITY                     *
********************************************************************/


// Get/Set text of attdef
void
dwg_ent_attdef_set_text(dwg_ent_attdef *attdef, char *default_value,
                        int *error);

char *
dwg_ent_attdef_get_text(dwg_ent_attdef *attdef, int *error);

// Get/Set insertion points of attdef
void
dwg_ent_attdef_get_insertion_point(dwg_ent_attdef *attdef, dwg_point_2d *point,
                                   int *error);

void
dwg_ent_attdef_set_insertion_point(dwg_ent_attdef *attdef, dwg_point_2d *point,
                                   int *error);

//Get/Set height of attdef
double
dwg_ent_attdef_get_height(dwg_ent_attdef *attdef, int *error);

void
dwg_ent_attdef_set_height(dwg_ent_attdef *attdef, double height, int *error);

//Get/Set extrusion of attdef
void
dwg_ent_attdef_get_extrusion(dwg_ent_attdef *attdef, dwg_point_3d *vector,
                             int *error);

void
dwg_ent_attdef_set_extrusion(dwg_ent_attdef *attdef, dwg_point_3d *vector,
                             int *error);

//Get/Set thickness of attdef
double
dwg_ent_attdef_get_thickness(dwg_ent_attdef *attdef, int *error);

void
dwg_ent_attdef_set_thickness(dwg_ent_attdef *attdef, double thickness,
                             int *error);

//Get/Set rotation angle of attdef
double
dwg_ent_attdef_get_rot_angle(dwg_ent_attdef *attdef, int *error);

void
dwg_ent_attdef_set_rot_angle(dwg_ent_attdef *attdef, double angle,
                             int *error);

//Get/Set horizontal alignment angle of attdef
double
dwg_ent_attdef_get_vert_align(dwg_ent_attdef *attdef, int *error);

void
dwg_ent_attdef_set_vert_align(dwg_ent_attdef *attdef, double alignment,
                              int *error);

//Get/Set vertical alignment of attdef
double
dwg_ent_attdef_get_horiz_align(dwg_ent_attdef *attdef, int *error);

void
dwg_ent_attdef_set_horiz_align(dwg_ent_attdef *attdef, double alignment,
                               int *error);


/********************************************************************
*                   FUNCTIONS FOR POINT ENTITY                      *
********************************************************************/

// Get/Set point of point entity
void
dwg_ent_point_set_point(dwg_ent_point *point, dwg_point_3d *retpoint,
                        int *error);

void
dwg_ent_point_get_point(dwg_ent_point *point, dwg_point_3d *retpoint,
                        int *error);

// Get/Set thickness of point entity
double
dwg_ent_point_get_thickness(dwg_ent_point *point, int *error);

void
dwg_ent_point_set_thickness(dwg_ent_point *point, double thickness,
                            int *error);

// Get/Set extrusion of point entity
void
dwg_ent_point_set_extrusion(dwg_ent_point *point, dwg_point_3d *retpoint,
                            int *error);

void
dwg_ent_point_get_extrusion(dwg_ent_point *point, dwg_point_3d *retpoint,
                            int *error);


/********************************************************************
*                   FUNCTIONS FOR SOLID ENTITY                      *
********************************************************************/


// Get/Set thickness of solid entity
double
dwg_ent_solid_get_thickness(dwg_ent_solid *solid, int *error);

void
dwg_ent_solid_set_thickness(dwg_ent_solid *solid, double thickness,
                            int *error);

// Get/Set elevation of solid entity
double
dwg_ent_solid_get_elevation(dwg_ent_solid *solid, int *error);

void
dwg_ent_solid_set_elevation(dwg_ent_solid *solid, double elevation,
                            int *error);

// Get/Set corner1 of solid entity
void
dwg_ent_solid_get_corner1(dwg_ent_solid *solid, dwg_point_2d *point,
                          int *error);

void
dwg_ent_solid_set_corner1(dwg_ent_solid *solid, dwg_point_2d *point,
                          int *error);

// Get/Set corner2 of solid entity
void
dwg_ent_solid_get_corner2(dwg_ent_solid *solid, dwg_point_2d *point,
                          int *error);

void
dwg_ent_solid_set_corner2(dwg_ent_solid *solid, dwg_point_2d *point,
                          int *error);

// Get/Set corner3 of solid entity
void
dwg_ent_solid_get_corner3(dwg_ent_solid *solid, dwg_point_2d *point,
                          int *error);

void
dwg_ent_solid_set_corner3(dwg_ent_solid *solid, dwg_point_2d *point,
                          int *error);

// Get/Set corner4 of solid entity
void
dwg_ent_solid_get_corner4(dwg_ent_solid *solid, dwg_point_2d *point,
                          int *error);

void
dwg_ent_solid_set_corner4(dwg_ent_solid *solid, dwg_point_2d *point,
                          int *error);

// Get/Set extrusion of solid entity
void
dwg_ent_solid_get_extrusion(dwg_ent_solid *solid, dwg_point_3d *vector,
                            int *error);

void
dwg_ent_solid_set_extrusion(dwg_ent_solid *solid, dwg_point_3d *vector,
                            int *error);


/********************************************************************
*                   FUNCTIONS FOR BLOCk ENTITY                      *
********************************************************************/


// Get/Set text of block entity
void
dwg_ent_block_set_name(dwg_ent_block *block, char *name, int *error);

char *
dwg_ent_block_get_name(dwg_ent_block *block, int *error);


/********************************************************************
*                    FUNCTIONS FOR RAY ENTITY                       *
********************************************************************/


// Get/Set point of ray entity
void
dwg_ent_ray_get_point(dwg_ent_ray *ray, dwg_point_3d *point, int *error);

void
dwg_ent_ray_set_point(dwg_ent_ray *ray, dwg_point_3d *point, int *error);

// Get/Set vector of ray entity
void
dwg_ent_ray_get_vector(dwg_ent_ray *ray, dwg_point_3d *vector, int *error);

void
dwg_ent_ray_set_vector(dwg_ent_ray *ray, dwg_point_3d *vector, int *error);


/********************************************************************
*                   FUNCTIONS FOR XLINE ENTITY                      *
********************************************************************/


// Get/Set point of xline entity
void
dwg_ent_xline_get_point(dwg_ent_xline *xline, dwg_point_3d *point, int *error);

void
dwg_ent_xline_set_point(dwg_ent_xline *xline, dwg_point_3d *point, int *error);

// Get/Set vector of xline entity
void
dwg_ent_xline_get_vector(dwg_ent_xline *xline, dwg_point_3d *vector,
                         int *error);

void
dwg_ent_xline_set_vector(dwg_ent_xline *xline, dwg_point_3d *vector,
                         int *error);


/********************************************************************
*                   FUNCTIONS FOR TRACE ENTITY                      *
********************************************************************/


// Get/Set thickness of trace entity
double
dwg_ent_trace_get_thickness(dwg_ent_trace *trace, int *error);

void
dwg_ent_trace_set_thickness(dwg_ent_trace *trace, double thickness,
                            int *error);

// Get/Set elevation of trace entity
double
dwg_ent_trace_get_elevation(dwg_ent_trace *trace, int *error);

void
dwg_ent_trace_set_elevation(dwg_ent_trace *trace, double elevation,
                            int *error);

// Get/Set corner1 of trace entity
void
dwg_ent_trace_get_corner1(dwg_ent_trace *trace, dwg_point_2d *point,
                          int *error);

void
dwg_ent_trace_set_corner1(dwg_ent_trace *trace, dwg_point_2d *point,
                          int *error);

// Get/Set corner2 of trace entity
void
dwg_ent_trace_get_corner2(dwg_ent_trace *trace, dwg_point_2d *point,
                          int *error);

void
dwg_ent_trace_set_corner2(dwg_ent_trace *trace, dwg_point_2d *point,
                          int *error);

// Get/Set corner3 of trace entity
void
dwg_ent_trace_get_corner3(dwg_ent_trace *trace, dwg_point_2d *point,
                          int *error);

void
dwg_ent_trace_set_corner3(dwg_ent_trace *trace, dwg_point_2d *point,
                          int *error);

// Get/Set corner4 of trace entity
void
dwg_ent_trace_get_corner4(dwg_ent_trace *trace, dwg_point_2d *point,
                          int *error);

void
dwg_ent_trace_set_corner4(dwg_ent_trace *trace, dwg_point_2d *point,
                          int *error);

// Get/Set extrusion of trace entity
void
dwg_ent_trace_get_extrusion(dwg_ent_trace *trace, dwg_point_3d *vector,
                            int *error);

void
dwg_ent_trace_set_extrusion(dwg_ent_trace *trace, dwg_point_3d *vector,
                            int *error);


/********************************************************************
*                 FUNCTIONS FOR VERTEX_3D ENTITY                    *
********************************************************************/


// Get/Set flags of vertex_3d entity
char
dwg_ent_vertex_3d_get_flags(dwg_ent_vertex_3d *vert, int *error);

void
dwg_ent_vertex_3d_set_flags(dwg_ent_vertex_3d *vert, char flags, int *error);

// Get/Set point of vertex_3d entity
void
dwg_ent_vertex_3d_get_point(dwg_ent_vertex_3d *vert, dwg_point_3d *point,
                            int *error);

void
dwg_ent_vertex_3d_set_point(dwg_ent_vertex_3d *vert, dwg_point_3d *point,
                            int *error);


/********************************************************************
*               FUNCTIONS FOR VERTEX_MESH ENTITY                    *
********************************************************************/


// Get/Set flags of vertex_mesh entity
char
dwg_ent_vertex_mesh_get_flags(dwg_ent_vertex_mesh *vert, int *error);

void
dwg_ent_vertex_mesh_set_flags(dwg_ent_vertex_mesh *vert, char flags,
                              int *error);

// Get/Set point of vertex_mesh entity
void
dwg_ent_vertex_mesh_get_point(dwg_ent_vertex_mesh *vert, dwg_point_3d *point,
                              int *error);

void
dwg_ent_vertex_mesh_set_point(dwg_ent_vertex_mesh *vert, dwg_point_3d *point,
                              int *error);


/********************************************************************
*               FUNCTIONS FOR VERTEX_PFACE ENTITY                   *
********************************************************************/


// Get/Set flags of vertex_pface entity
char
dwg_ent_vertex_pface_get_flags(dwg_ent_vertex_pface *vert, int *error);

void
dwg_ent_vertex_pface_set_flags(dwg_ent_vertex_pface *vert, char flags,
                               int *error);

// Get/Set point of vertex_pface entity
void
dwg_ent_vertex_pface_get_point(dwg_ent_vertex_pface *vert, dwg_point_3d *point,
                               int *error);

void
dwg_ent_vertex_pface_set_point(dwg_ent_vertex_pface *vert, dwg_point_3d *point,
                               int *error);


/********************************************************************
*                 FUNCTIONS FOR VERTEX_2D ENTITY                    *
********************************************************************/


// Get/Set flags of vertex_2d entity
char
dwg_ent_vertex_2d_get_flags(dwg_ent_vertex_2d *vert, int *error);

void
dwg_ent_vertex_2d_set_flags(dwg_ent_vertex_2d *vert, char flags, int *error);

// Get/Set point of vertex_2d entity
void
dwg_ent_vertex_2d_get_point(dwg_ent_vertex_2d *vert, dwg_point_3d *point,
                            int *error);

void
dwg_ent_vertex_2d_set_point(dwg_ent_vertex_2d *vert, dwg_point_3d *point,
                            int *error);

// Get/Set start_width of vertex_2d entity
double
dwg_ent_vertex_2d_get_start_width(dwg_ent_vertex_2d *vert, int *error);

void
dwg_ent_vertex_2d_set_start_width(dwg_ent_vertex_2d *vert, double start_width,
                                  int *error);

// Get/Set end width of a vertex_2d entity
double
dwg_ent_vertex_2d_get_end_width(dwg_ent_vertex_2d *vert, int *error);

void
dwg_ent_vertex_2d_set_end_width(dwg_ent_vertex_2d *vert, double end_width,
                                int *error);

// Get/Set bulge of a vertex_2d entity
double
dwg_ent_vertex_2d_get_bulge(dwg_ent_vertex_2d *vert, int *error);

void
dwg_ent_vertex_2d_set_bulge(dwg_ent_vertex_2d *vert, double bulge, int *error);

// Get/Set tanget_direction of a vertex_2d entity
double
dwg_ent_vertex_2d_get_tangent_dir(dwg_ent_vertex_2d *vert, int *error);

void
dwg_ent_vertex_2d_set_tangent_dir(dwg_ent_vertex_2d *vert, double tangent_dir,
                                  int *error);


/********************************************************************
*                   FUNCTIONS FOR INSERT ENTITY                     *
********************************************************************/


// Get/Set insertion point of insert entity
void
dwg_ent_insert_get_ins_pt(dwg_ent_insert *insert, dwg_point_3d *point,
                          int *error);

void
dwg_ent_insert_set_ins_pt(dwg_ent_insert *insert, dwg_point_3d *point,
                          int *error);

// Get/Set scale flag of an insert entity
char
dwg_ent_insert_get_scale_flag(dwg_ent_insert *insert, int *error);

void
dwg_ent_insert_set_scale_flag(dwg_ent_insert *insert, char flags, int *error);

void
dwg_ent_insert_get_scale(dwg_ent_insert *insert, dwg_point_3d *point,
                         int *error);

void
dwg_ent_insert_set_scale(dwg_ent_insert *insert, dwg_point_3d *point,
                         int *error);

double
dwg_ent_insert_get_rotation_angle(dwg_ent_insert *insert, int *error);

void
dwg_ent_insert_set_rotation_angle(dwg_ent_insert *insert, double rot_ang,
                                  int *error);

void
dwg_ent_insert_get_extrusion(dwg_ent_insert *insert, dwg_point_3d *point,
                             int *error);

void
dwg_ent_insert_set_extrusion(dwg_ent_insert *insert, dwg_point_3d *point,
                             int *error);

char
dwg_ent_insert_get_has_attribs(dwg_ent_insert *insert, int *error);

void
dwg_ent_insert_set_has_attribs(dwg_ent_insert *insert, char attribs,
                               int *error);

long
dwg_ent_insert_get_owned_obj_count(dwg_ent_insert *insert, int *error);

void
dwg_ent_insert_set_owned_obj_count(dwg_ent_insert *insert, long count,
                                   int *error);


/********************************************************************
*                  FUNCTIONS FOR MINSERT ENTITY                     *
********************************************************************/


void
dwg_ent_minsert_get_ins_pt(dwg_ent_minsert *minsert, dwg_point_3d *point,
                           int *error);

void
dwg_ent_minsert_set_ins_pt(dwg_ent_minsert *minsert, dwg_point_3d *point,
                           int *error);

char
dwg_ent_minsert_get_scale_flag(dwg_ent_minsert *minsert, int *error);

void
dwg_ent_minsert_set_scale_flag(dwg_ent_minsert *minsert, char flags,
                               int *error);

void
dwg_ent_minsert_get_scale(dwg_ent_minsert *minsert, dwg_point_3d *point,
                          int *error);

void
dwg_ent_minsert_set_scale(dwg_ent_minsert *minsert, dwg_point_3d *point,
                          int *error);

double
dwg_ent_minsert_get_rotation_angle(dwg_ent_minsert *minsert, int *error);

void
dwg_ent_minsert_set_rotation_angle(dwg_ent_minsert *minsert, double rot_ang,
                                   int *error);

void
dwg_ent_minsert_get_extrusion(dwg_ent_minsert *minsert, dwg_point_3d *point,
                              int *error);

void
dwg_ent_minsert_set_extrusion(dwg_ent_minsert *minsert, dwg_point_3d *point,
                              int *error);

char
dwg_ent_minsert_get_has_attribs(dwg_ent_minsert *minsert, int *error);

void
dwg_ent_minsert_set_has_attribs(dwg_ent_minsert *minsert, char attribs,
                                int *error);

long
dwg_ent_minsert_get_owned_obj_count(dwg_ent_minsert *minsert, int *error);

void
dwg_ent_minsert_set_owned_obj_count(dwg_ent_minsert *minsert, long count,
                                    int *error);

long
dwg_ent_minsert_get_numcols(dwg_ent_minsert *minsert, int *error);

void
dwg_ent_minsert_set_numcols(dwg_ent_minsert *minsert, long cols,
                            int *error);

long
dwg_ent_minsert_get_numrows(dwg_ent_minsert *minsert, int *error);

void
dwg_ent_minsert_set_numrows(dwg_ent_minsert *minsert, long cols,
                            int *error);

double
dwg_ent_minsert_get_col_spacing(dwg_ent_minsert *minsert, int *error);

void
dwg_ent_minsert_set_col_spacing(dwg_ent_minsert *minsert, double spacing,
                                int *error);

double
dwg_ent_minsert_get_row_spacing(dwg_ent_minsert *minsert, int *error);

void
dwg_ent_minsert_set_row_spacing(dwg_ent_minsert *minsert, double spacing,
                                int *error);


/********************************************************************
*                FUNCTIONS FOR MLINESTYLE OBJECT                    *
********************************************************************/


char *
dwg_obj_mlinstyle_get_name(dwg_obj_mlinestyle *mlinestyle, int *error);

void
dwg_obj_mlinestyle_set_name(dwg_obj_mlinestyle *mlinestyle, char *name,
                            int *error);

char *
dwg_obj_mlinestyle_get_desc(dwg_obj_mlinestyle *mlinestyle, int *error);

void
dwg_obj_mlinestyle_set_desc(dwg_obj_mlinestyle *mlinestyle, char *desc,
                            int *error);

int
dwg_obj_mlinestyle_get_flags(dwg_obj_mlinestyle *mlinestyle, int *error);

void
dwg_obj_mlinestyle_set_flags(dwg_obj_mlinestyle *mlinestyle, int flags,
                             int *error);

double
dwg_obj_mlinestyle_get_start_angle(dwg_obj_mlinestyle *mlinestyle, int *error);

void
dwg_obj_mlinestyle_set_start_angle(dwg_obj_mlinestyle *mlinestyle,
                                   double startang, int *error);

double
dwg_obj_mlinestyle_get_end_angle(dwg_obj_mlinestyle *mlinestyle, int *error);

void
dwg_obj_mlinestyle_set_end_angle(dwg_obj_mlinestyle *mlinestyle,
                                 double endang, int *error);

char
dwg_obj_mlinestyle_get_linesinstyle(dwg_obj_mlinestyle *mlinestyle,
                                    int *error);

void
dwg_obj_mlinestyle_set_linesinstyle(dwg_obj_mlinestyle *mlinestyle,
                                    char linesinstyle, int *error);


/********************************************************************
*               FUNCTIONS FOR APPID_CONTROL OBJECT                  *
********************************************************************/


int
dwg_obj_appid_control_get_num_entries(dwg_obj_appid_control *appid,
                                      int *error);

void
dwg_obj_appid_control_set_num_entries(dwg_obj_appid_control *appid,
                                      int entries, int *error);


/********************************************************************
*                    FUNCTIONS FOR APPID OBJECT                     *
********************************************************************/


char *
dwg_obj_appid_get_entry_name(dwg_obj_appid *appid, int *error);

void
dwg_obj_appid_set_entry_name(dwg_obj_appid *appid, char *entry_name,
                             int *error);

char
dwg_obj_appid_get_flag(dwg_obj_appid *appid, int *error);

void
dwg_obj_appid_set_flag(dwg_obj_appid *appid, char flag, int *error);

dwg_obj_appid_control 
dwg_obj_appid_get_appid_control(dwg_obj_appid *appid,
                                int *error);


/********************************************************************
*            FUNCTIONS FOR ORDINATE DIMENSION ENTITY                *
********************************************************************/


double
dwg_ent_dim_ordinate_get_elevation_ecs11(dwg_ent_dim_ordinate *dim,
                                         int *error);

void
dwg_ent_dim_ordinate_set_elevation_ecs11(dwg_ent_dim_ordinate *dim,
                                         double elevation_ecs11, int *error);

double
dwg_ent_dim_ordinate_get_elevation_ecs12(dwg_ent_dim_ordinate *dim,
                                         int *error);

void
dwg_ent_dim_ordinate_set_elevation_ecs12(dwg_ent_dim_ordinate *dim,
                                         double elevation_ecs12, int *error);

char
dwg_ent_dim_ordinate_get_flags1(dwg_ent_dim_ordinate *dim, int *error);

void
dwg_ent_dim_ordinate_set_flags1(dwg_ent_dim_ordinate *dim, char flag,
                                int *error);

double
dwg_ent_dim_ordinate_get_act_measurement(dwg_ent_dim_ordinate *dim,
                                         int *error);

void
dwg_ent_dim_ordinate_set_act_measurement(dwg_ent_dim_ordinate *dim,
                                         double act_measurement, int *error);

double
dwg_ent_dim_ordinate_get_horiz_dir(dwg_ent_dim_ordinate *dim, int *error);

void
dwg_ent_dim_ordinate_set_horiz_dir(dwg_ent_dim_ordinate *dim, double horiz_dir,
                                   int *error);

double
dwg_ent_dim_ordinate_get_lspace_factor(dwg_ent_dim_ordinate *dim, int *error);

void
dwg_ent_dim_ordinate_set_lspace_factor(dwg_ent_dim_ordinate *dim,
                                       double factor, int *error);

unsigned int
dwg_ent_dim_ordinate_get_lspace_style(dwg_ent_dim_ordinate *dim, int *error);

void
dwg_ent_dim_ordinate_set_lspace_style(dwg_ent_dim_ordinate *dim,
                                      unsigned int style, int *error);

unsigned int
dwg_ent_dim_ordinate_get_attachment_point(dwg_ent_dim_ordinate *dim,
                                          int *error);

void
dwg_ent_dim_ordinate_set_attachment_point(dwg_ent_dim_ordinate *dim,
                                          unsigned int point, int *error);

void
dwg_ent_dim_ordinate_set_extrusion(dwg_ent_dim_ordinate *dim,
                                   dwg_point_3d *point, int *error);

void
dwg_ent_dim_ordinate_get_extrusion(dwg_ent_dim_ordinate *dim,
                                   dwg_point_3d *point, int *error);

char *
dwg_ent_dim_ordinate_get_user_text(dwg_ent_dim_ordinate *dim, int *error);

void
dwg_ent_dim_ordinate_set_user_text(dwg_ent_dim_ordinate *dim, char *text,
                                   int *error);

double
dwg_ent_dim_ordinate_get_text_rot(dwg_ent_dim_ordinate *dim, int *error);

void
dwg_ent_dim_ordinate_set_text_rot(dwg_ent_dim_ordinate *dim, double rot,
                                  int *error);

double
dwg_ent_dim_ordinate_get_ins_rotation(dwg_ent_dim_ordinate *dim, int *error);

void
dwg_ent_dim_ordinate_set_ins_rotation(dwg_ent_dim_ordinate *dim, double rot,
                                      int *error);

char
dwg_ent_dim_ordinate_get_flip_arrow1(dwg_ent_dim_ordinate *dim, int *error);

void
dwg_ent_dim_ordinate_set_flip_arrow1(dwg_ent_dim_ordinate *dim,
                                     char flip_arrow, int *error);

char
dwg_ent_dim_ordinate_get_flip_arrow2(dwg_ent_dim_ordinate *dim, int *error);

void
dwg_ent_dim_ordinate_set_flip_arrow2(dwg_ent_dim_ordinate *dim,
                                     char flip_arrow, int *error);

void
dwg_ent_dim_ordinate_set_text_mid_pt(dwg_ent_dim_ordinate *dim,
                                     dwg_point_2d *point, int *error);

void
dwg_ent_dim_ordinate_get_text_mid_pt(dwg_ent_dim_ordinate *dim,
                                     dwg_point_2d *point, int *error);

void
dwg_ent_dim_ordinate_set_ins_scale(dwg_ent_dim_ordinate *dim,
                                   dwg_point_3d *point, int *error);

void
dwg_ent_dim_ordinate_get_ins_scale(dwg_ent_dim_ordinate *dim,
                                   dwg_point_3d *point, int *error);

void
dwg_ent_dim_ordinate_set_12_pt(dwg_ent_dim_ordinate *dim, dwg_point_2d *point,
                               int *error);

void
dwg_ent_dim_ordinate_get_12_pt(dwg_ent_dim_ordinate *dim, dwg_point_2d *point,
                               int *error);

void
dwg_ent_dim_ordinate_set_10_pt(dwg_ent_dim_ordinate *ord, dwg_point_3d *point,
                               int *error);

void
dwg_ent_dim_ordinate_get_10_pt(dwg_ent_dim_ordinate *ord, dwg_point_3d *point,
                               int *error);

void
dwg_ent_dim_ordinate_set_13_pt(dwg_ent_dim_ordinate *ord, dwg_point_3d *point,
                               int *error);

void
dwg_ent_dim_ordinate_get_13_pt(dwg_ent_dim_ordinate *ord, dwg_point_3d *point,
                               int *error);

void
dwg_ent_dim_ordinate_set_14_pt(dwg_ent_dim_ordinate *ord, dwg_point_3d *point,
                               int *error);

void
dwg_ent_dim_ordinate_get_14_pt(dwg_ent_dim_ordinate *ord, dwg_point_3d *point,
                               int *error);

void
dwg_ent_dim_ordinate_set_flags2(dwg_ent_dim_ordinate *ord, char flag,
                                int *error);

char
dwg_ent_dim_ordinate_get_flags2(dwg_ent_dim_ordinate *ord, int *error);


/********************************************************************
*              FUNCTIONS FOR LINEAR DIMENSION ENTITY                *
********************************************************************/


char *
dwg_ent_dim_linear_get_block_name(dwg_ent_dim_linear *dim, int *error);

double
dwg_ent_dim_linear_get_elevation_ecs11(dwg_ent_dim_linear *dim, int *error);

void
dwg_ent_dim_linear_set_elevation_ecs11(dwg_ent_dim_linear *dim,
                                       double elevation_ecs11, int *error);

double
dwg_ent_dim_linear_get_elevation_ecs12(dwg_ent_dim_linear *dim, int *error);

void
dwg_ent_dim_linear_set_elevation_ecs12(dwg_ent_dim_linear *dim,
                                       double elevation_ecs12, int *error);

char
dwg_ent_dim_linear_get_flags1(dwg_ent_dim_linear *dim, int *error);

void
dwg_ent_dim_linear_set_flags1(dwg_ent_dim_linear *dim, char flag, int *error);

double
dwg_ent_dim_linear_get_act_measurement(dwg_ent_dim_linear *dim, int *error);

void
dwg_ent_dim_linear_set_act_measurement(dwg_ent_dim_linear *dim,
                                       double act_measurement, int *error);

double
dwg_ent_dim_linear_get_horiz_dir(dwg_ent_dim_linear *dim, int *error);

void
dwg_ent_dim_linear_set_horiz_dir(dwg_ent_dim_linear *dim, double horiz_dir,
                                 int *error);

double
dwg_ent_dim_linear_get_lspace_factor(dwg_ent_dim_linear *dim, int *error);

void
dwg_ent_dim_linear_set_lspace_factor(dwg_ent_dim_linear *dim, double factor,
                                     int *error);

unsigned int
dwg_ent_dim_linear_get_lspace_style(dwg_ent_dim_linear *dim, int *error);

void
dwg_ent_dim_linear_set_lspace_style(dwg_ent_dim_linear *dim,
                                    unsigned int style, int *error);

unsigned int
dwg_ent_dim_linear_get_attachment_point(dwg_ent_dim_linear *dim, int *error);

void
dwg_ent_dim_linear_set_attachment_point(dwg_ent_dim_linear *dim,
                                        unsigned int point, int *error);

void
dwg_ent_dim_linear_set_extrusion(dwg_ent_dim_linear *dim, dwg_point_3d *point,
                                 int *error);

void
dwg_ent_dim_linear_get_extrusion(dwg_ent_dim_linear *dim, dwg_point_3d *point,
                                 int *error);

char *
dwg_ent_dim_linear_get_user_text(dwg_ent_dim_linear *dim, int *error);

void
dwg_ent_dim_linear_set_user_text(dwg_ent_dim_linear *dim, char *text,
                                 int *error);

double
dwg_ent_dim_linear_get_text_rot(dwg_ent_dim_linear *dim, int *error);

void
dwg_ent_dim_linear_set_text_rot(dwg_ent_dim_linear *dim, double rot,
                                int *error);

double
dwg_ent_dim_linear_get_ins_rotation(dwg_ent_dim_linear *dim, int *error);

void
dwg_ent_dim_linear_set_ins_rotation(dwg_ent_dim_linear *dim, double rot,
                                    int *error);

char
dwg_ent_dim_linear_get_flip_arrow1(dwg_ent_dim_linear *dim, int *error);

void
dwg_ent_dim_linear_set_flip_arrow1(dwg_ent_dim_linear *dim, char flip_arrow,
                                   int *error);

char
dwg_ent_dim_linear_get_flip_arrow2(dwg_ent_dim_linear *dim, int *error);

void
dwg_ent_dim_linear_set_flip_arrow2(dwg_ent_dim_linear *dim, char flip_arrow,
                                   int *error);

void
dwg_ent_dim_linear_set_text_mid_pt(dwg_ent_dim_linear *dim,
                                   dwg_point_2d *point, int *error);

void
dwg_ent_dim_linear_get_text_mid_pt(dwg_ent_dim_linear *dim,
                                   dwg_point_2d *point, int *error);

void
dwg_ent_dim_linear_set_ins_scale(dwg_ent_dim_linear *dim, dwg_point_3d *point,
                                 int *error);

void
dwg_ent_dim_linear_get_ins_scale(dwg_ent_dim_linear *dim, dwg_point_3d *point,
                                 int *error);

void
dwg_ent_dim_linear_set_12_pt(dwg_ent_dim_linear *dim, dwg_point_2d *point,
                             int *error);

void
dwg_ent_dim_linear_get_12_pt(dwg_ent_dim_linear *dim, dwg_point_2d *point,
                             int *error);

void
dwg_ent_dim_linear_set_10_pt(dwg_ent_dim_linear *dim, dwg_point_3d *point,
                             int *error);

void
dwg_ent_dim_linear_get_10_pt(dwg_ent_dim_linear *dim, dwg_point_3d *point,
                             int *error);

void
dwg_ent_dim_linear_set_13_pt(dwg_ent_dim_linear *dim, dwg_point_3d *point,
                             int *error);

void
dwg_ent_dim_linear_get_13_pt(dwg_ent_dim_linear *dim, dwg_point_3d *point,
                             int *error);

void
dwg_ent_dim_linear_set_14_pt(dwg_ent_dim_linear *dim, dwg_point_3d *point,
                             int *error);

void
dwg_ent_dim_linear_get_14_pt(dwg_ent_dim_linear *dim, dwg_point_3d *point,
                             int *error);

double
dwg_ent_dim_linear_get_dim_rot(dwg_ent_dim_linear *dim, int *error);

void
dwg_ent_dim_linear_set_dim_rot(dwg_ent_dim_linear *dim, double rot,
                               int *error);

double
dwg_ent_dim_linear_get_ext_line_rotation(dwg_ent_dim_linear *dim, int *error);

void
dwg_ent_dim_linear_set_ext_line_rotation(dwg_ent_dim_linear *dim, double rot,
                                         int *error);


/********************************************************************
*             FUNCTIONS FOR ALIGNED DIMENSION ENTITY                *
********************************************************************/


double
dwg_ent_dim_aligned_get_elevation_ecs11(dwg_ent_dim_aligned *dim, int *error);

void
dwg_ent_dim_aligned_set_elevation_ecs11(dwg_ent_dim_aligned *dim,
                                        double elevation_ecs11, int *error);

double
dwg_ent_dim_aligned_get_elevation_ecs12(dwg_ent_dim_aligned *dim, int *error);

void
dwg_ent_dim_aligned_set_elevation_ecs12(dwg_ent_dim_aligned *dim,
                                        double elevation_ecs12, int *error);

char
dwg_ent_dim_aligned_get_flags1(dwg_ent_dim_aligned *dim, int *error);

void
dwg_ent_dim_aligned_set_flags1(dwg_ent_dim_aligned *dim, char flag,
                               int *error);

double
dwg_ent_dim_aligned_get_act_measurement(dwg_ent_dim_aligned *dim, int *error);

void
dwg_ent_dim_aligned_set_act_measurement(dwg_ent_dim_aligned *dim,
                                        double act_measurement, int *error);

double
dwg_ent_dim_aligned_get_horiz_dir(dwg_ent_dim_aligned *dim, int *error);

void
dwg_ent_dim_aligned_set_horiz_dir(dwg_ent_dim_aligned *dim, double horiz_dir,
                                  int *error);

double
dwg_ent_dim_aligned_get_lspace_factor(dwg_ent_dim_aligned *dim, int *error);

void
dwg_ent_dim_aligned_set_lspace_factor(dwg_ent_dim_aligned *dim, double factor,
                                      int *error);

unsigned int
dwg_ent_dim_aligned_get_lspace_style(dwg_ent_dim_aligned *dim, int *error);

void
dwg_ent_dim_aligned_set_lspace_style(dwg_ent_dim_aligned *dim,
                                     unsigned int style, int *error);

unsigned int
dwg_ent_dim_aligned_get_attachment_point(dwg_ent_dim_aligned *dim, int *error);

void
dwg_ent_dim_aligned_set_attachment_point(dwg_ent_dim_aligned *dim,
                                         unsigned int point, int *error);

void
dwg_ent_dim_aligned_set_extrusion(dwg_ent_dim_aligned *dim,
                                  dwg_point_3d *point, int *error);

void
dwg_ent_dim_aligned_get_extrusion(dwg_ent_dim_aligned *dim,
                                  dwg_point_3d *point, int *error);

char *
dwg_ent_dim_aligned_get_user_text(dwg_ent_dim_aligned *dim, int *error);

void
dwg_ent_dim_aligned_set_user_text(dwg_ent_dim_aligned *dim, char *text,
                                  int *error);

double
dwg_ent_dim_aligned_get_text_rot(dwg_ent_dim_aligned *dim, int *error);

void
dwg_ent_dim_aligned_set_text_rot(dwg_ent_dim_aligned *dim, double rot,
                                 int *error);

double
dwg_ent_dim_aligned_get_ins_rotation(dwg_ent_dim_aligned *dim, int *error);

void
dwg_ent_dim_aligned_set_ins_rotation(dwg_ent_dim_aligned *dim, double rot,
                                     int *error);

char
dwg_ent_dim_aligned_get_flip_arrow1(dwg_ent_dim_aligned *dim, int *error);

void
dwg_ent_dim_aligned_set_flip_arrow1(dwg_ent_dim_aligned *dim, char flip_arrow,
                                    int *error);

char
dwg_ent_dim_aligned_get_flip_arrow2(dwg_ent_dim_aligned *dim, int *error);

void
dwg_ent_dim_aligned_set_flip_arrow2(dwg_ent_dim_aligned *dim, char flip_arrow,
                                    int *error);

void
dwg_ent_dim_aligned_set_text_mid_pt(dwg_ent_dim_aligned *dim,
                                    dwg_point_2d *point, int *error);

void
dwg_ent_dim_aligned_get_text_mid_pt(dwg_ent_dim_aligned *dim,
                                    dwg_point_2d *point, int *error);

void
dwg_ent_dim_aligned_set_ins_scale(dwg_ent_dim_aligned *dim,
                                  dwg_point_3d *point, int *error);

void
dwg_ent_dim_aligned_get_ins_scale(dwg_ent_dim_aligned *dim,
                                  dwg_point_3d *point, int *error);

void
dwg_ent_dim_aligned_set_12_pt(dwg_ent_dim_aligned *dim, dwg_point_2d *point,
                              int *error);

void
dwg_ent_dim_aligned_get_12_pt(dwg_ent_dim_aligned *dim, dwg_point_2d *point,
                              int *error);

void
dwg_ent_dim_aligned_set_10_pt(dwg_ent_dim_aligned *dim, dwg_point_3d *point,
                              int *error);

void
dwg_ent_dim_aligned_get_10_pt(dwg_ent_dim_aligned *dim, dwg_point_3d *point,
                              int *error);

void
dwg_ent_dim_aligned_set_13_pt(dwg_ent_dim_aligned *dim, dwg_point_3d *point,
                              int *error);

void
dwg_ent_dim_aligned_get_13_pt(dwg_ent_dim_aligned *dim, dwg_point_3d *point,
                              int *error);

void
dwg_ent_dim_aligned_set_14_pt(dwg_ent_dim_aligned *dim, dwg_point_3d *point,
                              int *error);

void
dwg_ent_dim_aligned_get_14_pt(dwg_ent_dim_aligned *dim, dwg_point_3d *point,
                              int *error);

double
dwg_ent_dim_aligned_get_ext_line_rotation(dwg_ent_dim_aligned *dim,
                                          int *error);

void
dwg_ent_dim_aligned_set_ext_line_rotation(dwg_ent_dim_aligned *dim, double rot,
                                          int *error);


/********************************************************************
*              FUNCTIONS FOR ANG3PT DIMENSION ENTITY                *
********************************************************************/


double
dwg_ent_dim_ang3pt_get_elevation_ecs11(dwg_ent_dim_ang3pt *ang, int *error);

void
dwg_ent_dim_ang3pt_set_elevation_ecs11(dwg_ent_dim_ang3pt *ang,
                                       double elevation_ecs11, int *error);

double
dwg_ent_dim_ang3pt_get_elevation_ecs12(dwg_ent_dim_ang3pt *ang, int *error);

void
dwg_ent_dim_ang3pt_set_elevation_ecs12(dwg_ent_dim_ang3pt *ang,
                                       double elevation_ecs12, int *error);

char
dwg_ent_dim_ang3pt_get_flags1(dwg_ent_dim_ang3pt *ang, int *error);

void
dwg_ent_dim_ang3pt_set_flags1(dwg_ent_dim_ang3pt *ang, char flag, int *error);

double
dwg_ent_dim_ang3pt_get_act_measurement(dwg_ent_dim_ang3pt *ang, int *error);

void
dwg_ent_dim_ang3pt_set_act_measurement(dwg_ent_dim_ang3pt *ang,
                                       double act_measurement, int *error);

double
dwg_ent_dim_ang3pt_get_horiz_dir(dwg_ent_dim_ang3pt *ang, int *error);

void
dwg_ent_dim_ang3pt_set_horiz_dir(dwg_ent_dim_ang3pt *ang, double horiz_dir,
                                 int *error);

double
dwg_ent_dim_ang3pt_get_lspace_factor(dwg_ent_dim_ang3pt *ang, int *error);

void
dwg_ent_dim_ang3pt_set_lspace_factor(dwg_ent_dim_ang3pt *ang, double factor,
                                     int *error);

unsigned int
dwg_ent_dim_ang3pt_get_lspace_style(dwg_ent_dim_ang3pt *ang, int *error);

void
dwg_ent_dim_ang3pt_set_lspace_style(dwg_ent_dim_ang3pt *ang,
                                    unsigned int style, int *error);

unsigned int
dwg_ent_dim_ang3pt_get_attachment_point(dwg_ent_dim_ang3pt *ang, int *error);

void
dwg_ent_dim_ang3pt_set_attachment_point(dwg_ent_dim_ang3pt *ang,
                                        unsigned int point, int *error);

void
dwg_ent_dim_ang3pt_set_extrusion(dwg_ent_dim_ang3pt *ang, dwg_point_3d *point,
                                 int *error);

void
dwg_ent_dim_ang3pt_get_extrusion(dwg_ent_dim_ang3pt *ang, dwg_point_3d *point,
                                 int *error);

char *
dwg_ent_dim_ang3pt_get_user_text(dwg_ent_dim_ang3pt *ang, int *error);

void
dwg_ent_dim_ang3pt_set_user_text(dwg_ent_dim_ang3pt *ang, char *text,
                                 int *error);

double
dwg_ent_dim_ang3pt_get_text_rot(dwg_ent_dim_ang3pt *ang, int *error);

void
dwg_ent_dim_ang3pt_set_text_rot(dwg_ent_dim_ang3pt *ang, double rot,
                                int *error);

double
dwg_ent_dim_ang3pt_get_ins_rotation(dwg_ent_dim_ang3pt *ang, int *error);

void
dwg_ent_dim_ang3pt_set_ins_rotation(dwg_ent_dim_ang3pt *ang, double rot,
                                    int *error);

char
dwg_ent_dim_ang3pt_get_flip_arrow1(dwg_ent_dim_ang3pt *ang, int *error);

void
dwg_ent_dim_ang3pt_set_flip_arrow1(dwg_ent_dim_ang3pt *ang, char flip_arrow,
                                   int *error);

char
dwg_ent_dim_ang3pt_get_flip_arrow2(dwg_ent_dim_ang3pt *ang, int *error);

void
dwg_ent_dim_ang3pt_set_flip_arrow2(dwg_ent_dim_ang3pt *ang, char flip_arrow,
                                   int *error);

void
dwg_ent_dim_ang3pt_set_text_mid_pt(dwg_ent_dim_ang3pt *ang,
                                   dwg_point_2d *point, int *error);

void
dwg_ent_dim_ang3pt_get_text_mid_pt(dwg_ent_dim_ang3pt *ang,
                                   dwg_point_2d *point, int *error);

void
dwg_ent_dim_ang3pt_set_ins_scale(dwg_ent_dim_ang3pt *ang, dwg_point_3d *point,
                                 int *error);

void
dwg_ent_dim_ang3pt_get_ins_scale(dwg_ent_dim_ang3pt *ang, dwg_point_3d *point,
                                 int *error);

void
dwg_ent_dim_ang3pt_set_12_pt(dwg_ent_dim_ang3pt *ang, dwg_point_2d *point,
                             int *error);

void
dwg_ent_dim_ang3pt_get_12_pt(dwg_ent_dim_ang3pt *ang, dwg_point_2d *point,
                             int *error);

void
dwg_ent_dim_ang3pt_set_10_pt(dwg_ent_dim_ang3pt *ang, dwg_point_3d *point,
                             int *error);

void
dwg_ent_dim_ang3pt_get_10_pt(dwg_ent_dim_ang3pt *ang, dwg_point_3d *point,
                             int *error);

void
dwg_ent_dim_ang3pt_set_13_pt(dwg_ent_dim_ang3pt *ang, dwg_point_3d *point,
                             int *error);

void
dwg_ent_dim_ang3pt_get_13_pt(dwg_ent_dim_ang3pt *ang, dwg_point_3d *point,
                             int *error);

void
dwg_ent_dim_ang3pt_set_14_pt(dwg_ent_dim_ang3pt *ang, dwg_point_3d *point,
                             int *error);

void
dwg_ent_dim_ang3pt_get_14_pt(dwg_ent_dim_ang3pt *ang, dwg_point_3d *point,
                             int *error);

void
dwg_ent_dim_ang3pt_set_15_pt(dwg_ent_dim_ang3pt *ang, dwg_point_3d *point,
                             int *error);

void
dwg_ent_dim_ang3pt_get_15_pt(dwg_ent_dim_ang3pt *ang, dwg_point_3d *point,
                             int *error);


/********************************************************************
*              FUNCTIONS FOR ANG2LN DIMENSION ENTITY                *
********************************************************************/


double
dwg_ent_dim_ang2ln_get_elevation_ecs11(dwg_ent_dim_ang2ln *ang, int *error);

void
dwg_ent_dim_ang2ln_set_elevation_ecs11(dwg_ent_dim_ang2ln *ang,
                                       double elevation_ecs11, int *error);

double
dwg_ent_dim_ang2ln_get_elevation_ecs12(dwg_ent_dim_ang2ln *ang, int *error);

void
dwg_ent_dim_ang2ln_set_elevation_ecs12(dwg_ent_dim_ang2ln *ang,
                                       double elevation_ecs12, int *error);

char
dwg_ent_dim_ang2ln_get_flags1(dwg_ent_dim_ang2ln *ang, int *error);

void
dwg_ent_dim_ang2ln_set_flags1(dwg_ent_dim_ang2ln *ang, char flag, int *error);

double
dwg_ent_dim_ang2ln_get_act_measurement(dwg_ent_dim_ang2ln *ang, int *error);

void
dwg_ent_dim_ang2ln_set_act_measurement(dwg_ent_dim_ang2ln *ang,
                                       double act_measurement, int *error);

double
dwg_ent_dim_ang2ln_get_horiz_dir(dwg_ent_dim_ang2ln *ang, int *error);

void
dwg_ent_dim_ang2ln_set_horiz_dir(dwg_ent_dim_ang2ln *ang, double horiz_dir,
                                 int *error);

double
dwg_ent_dim_ang2ln_get_lspace_factor(dwg_ent_dim_ang2ln *ang, int *error);

void
dwg_ent_dim_ang2ln_set_lspace_factor(dwg_ent_dim_ang2ln *ang, double factor,
                                     int *error);

unsigned int
dwg_ent_dim_ang2ln_get_lspace_style(dwg_ent_dim_ang2ln *ang, int *error);

void
dwg_ent_dim_ang2ln_set_lspace_style(dwg_ent_dim_ang2ln *ang,
                                    unsigned int style, int *error);

unsigned int
dwg_ent_dim_ang2ln_get_attachment_point(dwg_ent_dim_ang2ln *ang, int *error);

void
dwg_ent_dim_ang2ln_set_attachment_point(dwg_ent_dim_ang2ln *ang,
                                        unsigned int point, int *error);

void
dwg_ent_dim_ang2ln_set_extrusion(dwg_ent_dim_ang2ln *ang, dwg_point_3d *point,
                                 int *error);

void
dwg_ent_dim_ang2ln_get_extrusion(dwg_ent_dim_ang2ln *ang, dwg_point_3d *point,
                                 int *error);

char *
dwg_ent_dim_ang2ln_get_user_text(dwg_ent_dim_ang2ln *ang, int *error);

void
dwg_ent_dim_ang2ln_set_user_text(dwg_ent_dim_ang2ln *ang, char *text,
                                 int *error);

double
dwg_ent_dim_ang2ln_get_text_rot(dwg_ent_dim_ang2ln *ang, int *error);

void
dwg_ent_dim_ang2ln_set_text_rot(dwg_ent_dim_ang2ln *ang, double rot,
                                int *error);

double
dwg_ent_dim_ang2ln_get_ins_rotation(dwg_ent_dim_ang2ln *ang, int *error);

void
dwg_ent_dim_ang2ln_set_ins_rotation(dwg_ent_dim_ang2ln *ang, double rot,
                                    int *error);

char
dwg_ent_dim_ang2ln_get_flip_arrow1(dwg_ent_dim_ang2ln *ang, int *error);

void
dwg_ent_dim_ang2ln_set_flip_arrow1(dwg_ent_dim_ang2ln *ang, char flip_arrow,
                                   int *error);

char
dwg_ent_dim_ang2ln_get_flip_arrow2(dwg_ent_dim_ang2ln *ang, int *error);

void
dwg_ent_dim_ang2ln_set_flip_arrow2(dwg_ent_dim_ang2ln *ang, char flip_arrow,
                                   int *error);

void
dwg_ent_dim_ang2ln_set_text_mid_pt(dwg_ent_dim_ang2ln *ang,
                                   dwg_point_2d *point, int *error);

void
dwg_ent_dim_ang2ln_get_text_mid_pt(dwg_ent_dim_ang2ln *ang,
                                   dwg_point_2d *point, int *error);

void
dwg_ent_dim_ang2ln_set_ins_scale(dwg_ent_dim_ang2ln *ang, dwg_point_3d *point,
                                 int *error);

void
dwg_ent_dim_ang2ln_get_ins_scale(dwg_ent_dim_ang2ln *ang, dwg_point_3d *point,
                                 int *error);

void
dwg_ent_dim_ang2ln_set_12_pt(dwg_ent_dim_ang2ln *ang, dwg_point_2d *point,
                             int *error);

void
dwg_ent_dim_ang2ln_get_12_pt(dwg_ent_dim_ang2ln *ang, dwg_point_2d *point,
                             int *error);

void
dwg_ent_dim_ang2ln_set_10_pt(dwg_ent_dim_ang2ln *ang, dwg_point_3d *point,
                             int *error);

void
dwg_ent_dim_ang2ln_get_10_pt(dwg_ent_dim_ang2ln *ang, dwg_point_3d *point,
                             int *error);

void
dwg_ent_dim_ang2ln_set_13_pt(dwg_ent_dim_ang2ln *ang, dwg_point_3d *point,
                             int *error);

void
dwg_ent_dim_ang2ln_get_13_pt(dwg_ent_dim_ang2ln *ang, dwg_point_3d *point,
                             int *error);

void
dwg_ent_dim_ang2ln_set_14_pt(dwg_ent_dim_ang2ln *ang, dwg_point_3d *point,
                             int *error);

void
dwg_ent_dim_ang2ln_get_14_pt(dwg_ent_dim_ang2ln *ang, dwg_point_3d *point,
                             int *error);

void
dwg_ent_dim_ang2ln_set_15_pt(dwg_ent_dim_ang2ln *ang, dwg_point_3d *point,
                             int *error);

void
dwg_ent_dim_ang2ln_get_15_pt(dwg_ent_dim_ang2ln *ang, dwg_point_3d *point,
                             int *error);

void
dwg_ent_dim_ang2ln_set_16_pt(dwg_ent_dim_ang2ln *ang, dwg_point_2d *point,
                             int *error);

void
dwg_ent_dim_ang2ln_get_16_pt(dwg_ent_dim_ang2ln *ang, dwg_point_2d *point,
                             int *error);


/********************************************************************
*              FUNCTIONS FOR RADIUS DIMENSION ENTITY                *
********************************************************************/


double
dwg_ent_dim_radius_get_elevation_ecs11(dwg_ent_dim_radius *radius, int *error);

void
dwg_ent_dim_radius_set_elevation_ecs11(dwg_ent_dim_radius *radius,
                                       double elevation_ecs11, int *error);

double
dwg_ent_dim_radius_get_elevation_ecs12(dwg_ent_dim_radius *radius, int *error);

void
dwg_ent_dim_radius_set_elevation_ecs12(dwg_ent_dim_radius *radius,
                                       double elevation_ecs12, int *error);

char
dwg_ent_dim_radius_get_flags1(dwg_ent_dim_radius *radius, int *error);

void
dwg_ent_dim_radius_set_flags1(dwg_ent_dim_radius *radius, char flag,
                              int *error);

double
dwg_ent_dim_radius_get_act_measurement(dwg_ent_dim_radius *radius, int *error);

void
dwg_ent_dim_radius_set_act_measurement(dwg_ent_dim_radius *radius,
                                       double act_measurement, int *error);

double
dwg_ent_dim_radius_get_horiz_dir(dwg_ent_dim_radius *radius, int *error);

void
dwg_ent_dim_radius_set_horiz_dir(dwg_ent_dim_radius *radius, double horiz_dir,
                                 int *error);

double
dwg_ent_dim_radius_get_lspace_factor(dwg_ent_dim_radius *radius, int *error);

void
dwg_ent_dim_radius_set_lspace_factor(dwg_ent_dim_radius *radius, double factor,
                                     int *error);

unsigned int
dwg_ent_dim_radius_get_lspace_style(dwg_ent_dim_radius *radius, int *error);

void
dwg_ent_dim_radius_set_lspace_style(dwg_ent_dim_radius *radius,
                                    unsigned int style, int *error);

unsigned int
dwg_ent_dim_radius_get_attachment_point(dwg_ent_dim_radius *radius,
                                        int *error);

void
dwg_ent_dim_radius_set_attachment_point(dwg_ent_dim_radius *radius,
                                        unsigned int point, int *error);

void
dwg_ent_dim_radius_set_extrusion(dwg_ent_dim_radius *radius,
                                 dwg_point_3d *point, int *error);

void
dwg_ent_dim_radius_get_extrusion(dwg_ent_dim_radius *radius,
                                 dwg_point_3d *point, int *error);

char *
dwg_ent_dim_radius_get_user_text(dwg_ent_dim_radius *radius, int *error);

void
dwg_ent_dim_radius_set_user_text(dwg_ent_dim_radius *radius, char *text,
                                 int *error);

double
dwg_ent_dim_radius_get_text_rot(dwg_ent_dim_radius *radius, int *error);

void
dwg_ent_dim_radius_set_text_rot(dwg_ent_dim_radius *radius, double rot,
                                int *error);

double
dwg_ent_dim_radius_get_ins_rotation(dwg_ent_dim_radius *radius, int *error);

void
dwg_ent_dim_radius_set_ins_rotation(dwg_ent_dim_radius *radius, double rot,
                                    int *error);

char
dwg_ent_dim_radius_get_flip_arrow1(dwg_ent_dim_radius *radius, int *error);

void
dwg_ent_dim_radius_set_flip_arrow1(dwg_ent_dim_radius *radius, char flip_arrow,
                                   int *error);

char
dwg_ent_dim_radius_get_flip_arrow2(dwg_ent_dim_radius *radius, int *error);

void
dwg_ent_dim_radius_set_flip_arrow2(dwg_ent_dim_radius *radius, char flip_arrow,
                                   int *error);

void
dwg_ent_dim_radius_set_text_mid_pt(dwg_ent_dim_radius *radius,
                                   dwg_point_2d *point, int *error);

void
dwg_ent_dim_radius_get_text_mid_pt(dwg_ent_dim_radius *radius,
                                   dwg_point_2d *point, int *error);

void
dwg_ent_dim_radius_set_ins_scale(dwg_ent_dim_radius *radius,
                                 dwg_point_3d *point, int *error);

void
dwg_ent_dim_radius_get_ins_scale(dwg_ent_dim_radius *radius,
                                 dwg_point_3d *point, int *error);

void
dwg_ent_dim_radius_set_12_pt(dwg_ent_dim_radius *radius, dwg_point_2d *point,
                             int *error);

void
dwg_ent_dim_radius_get_12_pt(dwg_ent_dim_radius *radius, dwg_point_2d *point,
                             int *error);

void
dwg_ent_dim_radius_set_10_pt(dwg_ent_dim_radius *radius, dwg_point_3d *point,
                             int *error);

void
dwg_ent_dim_radius_get_10_pt(dwg_ent_dim_radius *radius, dwg_point_3d *point,
                             int *error);

void
dwg_ent_dim_radius_set_15_pt(dwg_ent_dim_radius *radius, dwg_point_3d *point,
                             int *error);

void
dwg_ent_dim_radius_get_15_pt(dwg_ent_dim_radius *radius, dwg_point_3d *point,
                             int *error);

double
dwg_ent_dim_radius_get_leader_length(dwg_ent_dim_radius *radius, int *error);

void
dwg_ent_dim_radius_set_leader_length(dwg_ent_dim_radius *radius, double length,
                                  int *error);


/********************************************************************
*             FUNCTIONS FOR DIAMETER DIMENSION ENTITY               *
********************************************************************/


double
dwg_ent_dim_diameter_get_elevation_ecs11(dwg_ent_dim_diameter *dia,
                                         int *error);

void
dwg_ent_dim_diameter_set_elevation_ecs11(dwg_ent_dim_diameter *dia,
                                         double elevation_ecs11, int *error);

double
dwg_ent_dim_diameter_get_elevation_ecs12(dwg_ent_dim_diameter *dia,
                                         int *error);

void
dwg_ent_dim_diameter_set_elevation_ecs12(dwg_ent_dim_diameter *dia,
                                         double elevation_ecs12, int *error);

char
dwg_ent_dim_diameter_get_flags1(dwg_ent_dim_diameter *dia, int *error);

void
dwg_ent_dim_diameter_set_flags1(dwg_ent_dim_diameter *dia, char flag,
                                int *error);

double
dwg_ent_dim_diameter_get_act_measurement(dwg_ent_dim_diameter *dia,
                                         int *error);

void
dwg_ent_dim_diameter_set_act_measurement(dwg_ent_dim_diameter *dia,
                                         double act_measurement, int *error);

double
dwg_ent_dim_diameter_get_horiz_dir(dwg_ent_dim_diameter *dia, int *error);

void
dwg_ent_dim_diameter_set_horiz_dir(dwg_ent_dim_diameter *dia, double horiz_dir,
                                   int *error);

double
dwg_ent_dim_diameter_get_lspace_factor(dwg_ent_dim_diameter *dia, int *error);

void
dwg_ent_dim_diameter_set_lspace_factor(dwg_ent_dim_diameter *dia,
                                       double factor, int *error);

unsigned int
dwg_ent_dim_diameter_get_lspace_style(dwg_ent_dim_diameter *dia, int *error);

void
dwg_ent_dim_diameter_set_lspace_style(dwg_ent_dim_diameter *dia,
                                      unsigned int style, int *error);

unsigned int
dwg_ent_dim_diameter_get_attachment_point(dwg_ent_dim_diameter *dia,
                                          int *error);

void
dwg_ent_dim_diameter_set_attachment_point(dwg_ent_dim_diameter *dia,
                                          unsigned int point, int *error);

void
dwg_ent_dim_diameter_set_extrusion(dwg_ent_dim_diameter *dia,
                                   dwg_point_3d *point, int *error);

void
dwg_ent_dim_diameter_get_extrusion(dwg_ent_dim_diameter *dia,
                                   dwg_point_3d *point, int *error);

char *
dwg_ent_dim_diameter_get_user_text(dwg_ent_dim_diameter *dia, int *error);

void
dwg_ent_dim_diameter_set_user_text(dwg_ent_dim_diameter *dia, char *text,
                                   int *error);

double
dwg_ent_dim_diameter_get_text_rot(dwg_ent_dim_diameter *dia, int *error);

void
dwg_ent_dim_diameter_set_text_rot(dwg_ent_dim_diameter *dia, double rot,
                                  int *error);

double
dwg_ent_dim_diameter_get_ins_rotation(dwg_ent_dim_diameter *dia, int *error);

void
dwg_ent_dim_diameter_set_ins_rotation(dwg_ent_dim_diameter *dia, double rot,
                                      int *error);

char
dwg_ent_dim_diameter_get_flip_arrow1(dwg_ent_dim_diameter *dia, int *error);

void
dwg_ent_dim_diameter_set_flip_arrow1(dwg_ent_dim_diameter *dia,
                                     char flip_arrow, int *error);

char
dwg_ent_dim_diameter_get_flip_arrow2(dwg_ent_dim_diameter *dia, int *error);

void
dwg_ent_dim_diameter_set_flip_arrow2(dwg_ent_dim_diameter *dia,
                                     char flip_arrow, int *error);

void
dwg_ent_dim_diameter_set_text_mid_pt(dwg_ent_dim_diameter *dia,
                                     dwg_point_2d *point, int *error);

void
dwg_ent_dim_diameter_get_text_mid_pt(dwg_ent_dim_diameter *dia,
                                     dwg_point_2d *point, int *error);

void
dwg_ent_dim_diameter_set_ins_scale(dwg_ent_dim_diameter *dia,
                                   dwg_point_3d *point, int *error);

void
dwg_ent_dim_diameter_get_ins_scale(dwg_ent_dim_diameter *dia,
                                   dwg_point_3d *point, int *error);

void
dwg_ent_dim_diameter_set_12_pt(dwg_ent_dim_diameter *dia, dwg_point_2d *point,
                               int *error);

void
dwg_ent_dim_diameter_get_12_pt(dwg_ent_dim_diameter *dia, dwg_point_2d *point,
                               int *error);

void
dwg_ent_dim_diameter_set_10_pt(dwg_ent_dim_diameter *dia, dwg_point_3d *point,
                               int *error);

void
dwg_ent_dim_diameter_get_10_pt(dwg_ent_dim_diameter *dia, dwg_point_3d *point,
                               int *error);

void
dwg_ent_dim_diameter_set_15_pt(dwg_ent_dim_diameter *dia, dwg_point_3d *point,
                               int *error);

void
dwg_ent_dim_diameter_get_15_pt(dwg_ent_dim_diameter *dia, dwg_point_3d *point,
                               int *error);

double
dwg_ent_dim_diameter_get_leader_length(dwg_ent_dim_diameter *dia, int *error);

void
dwg_ent_dim_diameter_set_leader_length(dwg_ent_dim_diameter *dia,
                                    double leader_len, int *error);


/********************************************************************
*                   FUNCTIONS FOR ENDBLK ENTITY                     *
********************************************************************/


char
dwg_ent_endblk_get_dummy(dwg_ent_endblk *endblk, int *error);

void
dwg_ent_endblk_set_dummy(dwg_ent_endblk *endblk, char dummy, int *error);


/********************************************************************
*                   FUNCTIONS FOR SEQEND ENTITY                     *
********************************************************************/


char
dwg_ent_seqend_get_dummy(dwg_ent_seqend *seqend, int *error);

void
dwg_ent_seqend_set_dummy(dwg_ent_seqend *seqend, char dummy, int *error);


/********************************************************************
*                    FUNCTIONS FOR SHAPE ENTITY                     *
********************************************************************/


void
dwg_ent_shape_get_ins_pt(dwg_ent_shape *shape, dwg_point_3d *point,
                         int *error);

void
dwg_ent_shape_set_ins_pt(dwg_ent_shape *shape, dwg_point_3d *point,
                         int *error);

double
dwg_ent_shape_get_scale(dwg_ent_shape *shape, int *error);

void
dwg_ent_shape_set_scale(dwg_ent_shape *shape, double scale, int *error);

double
dwg_ent_shape_get_rotation(dwg_ent_shape *shape, int *error);

void
dwg_ent_shape_set_rotation(dwg_ent_shape *shape, double rotation, int *error);

double
dwg_ent_shape_get_width_factor(dwg_ent_shape *shape, int *error);

void
dwg_ent_shape_set_width_factor(dwg_ent_shape *shape, double width_factor,
                               int *error);

double
dwg_ent_shape_get_oblique(dwg_ent_shape *shape, int *error);

void
dwg_ent_shape_set_oblique(dwg_ent_shape *shape, double oblique, int *error);

double
dwg_ent_shape_get_thickness(dwg_ent_shape *shape, int *error);

void
dwg_ent_shape_set_thickness(dwg_ent_shape *shape, double thickness,
                            int *error);

double
dwg_ent_shape_get_shape_no(dwg_ent_shape *shape, int *error);

void
dwg_ent_shape_set_shape_no(dwg_ent_shape *shape, double no, int *error);

void
dwg_ent_shape_get_extrusion(dwg_ent_shape *shape, dwg_point_3d *point,
                            int *error);

void
dwg_ent_shape_set_extrusion(dwg_ent_shape *shape, dwg_point_3d *point,
                            int *error);


/********************************************************************
*                    FUNCTIONS FOR MTEXT ENTITY                     *
********************************************************************/


void
dwg_ent_mtext_set_insertion_pt(dwg_ent_mtext *mtext, dwg_point_3d *point,
                               int *error);

void
dwg_ent_mtext_get_insertion_pt(dwg_ent_mtext *mtext, dwg_point_3d *point,
                               int *error);

void
dwg_ent_mtext_set_extrusion(dwg_ent_mtext *mtext, dwg_point_3d *point,
                            int *error);

void
dwg_ent_mtext_get_extrusion(dwg_ent_mtext *mtext, dwg_point_3d *point,
                            int *error);

void
dwg_ent_mtext_set_x_axis_dir(dwg_ent_mtext *mtext, dwg_point_3d *point,
                             int *error);

void
dwg_ent_mtext_get_x_axis_dir(dwg_ent_mtext *mtext, dwg_point_3d *point,
                             int *error);

void
dwg_ent_mtext_set_rect_height(dwg_ent_mtext *mtext, double rect_height,
                              int *error);

double
dwg_ent_mtext_get_rect_height(dwg_ent_mtext *mtext, int *error);

void
dwg_ent_mtext_set_rect_width(dwg_ent_mtext *mtext, double rect_width,
                             int *error);

double
dwg_ent_mtext_get_rect_width(dwg_ent_mtext *mtext, int *error);

void
dwg_ent_mtext_set_text_height(dwg_ent_mtext *mtext, double text_height,
                              int *error);

double
dwg_ent_mtext_get_text_height(dwg_ent_mtext *mtext, int *error);

unsigned int
dwg_ent_mtext_get_attachment(dwg_ent_mtext *mtext, int *error);

void
dwg_ent_mtext_set_attachment(dwg_ent_mtext *mtext, unsigned int attachment,
                             int *error);

unsigned int
dwg_ent_mtext_get_drawing_dir(dwg_ent_mtext *mtext, int *error);

void
dwg_ent_mtext_set_drawing_dir(dwg_ent_mtext *mtext, unsigned int dir,
                              int *error);

double
dwg_ent_mtext_get_extends_ht(dwg_ent_mtext *mtext, int *error);

void
dwg_ent_mtext_set_extends_ht(dwg_ent_mtext *mtext, double ht, int *error);

double
dwg_ent_mtext_get_extends_wid(dwg_ent_mtext *mtext, int *error);

void
dwg_ent_mtext_set_extends_wid(dwg_ent_mtext *mtext, double wid, int *error);

char* dwg_ent_mtext_get_text(dwg_ent_mtext *mtext, int *error);

void
dwg_ent_mtext_set_text(dwg_ent_mtext *mtext, char *text, int *error);

unsigned int
dwg_ent_mtext_get_linespace_style(dwg_ent_mtext *mtext, int *error);

void
dwg_ent_mtext_set_linespace_style(dwg_ent_mtext *mtext, unsigned int style,
                                  int *error);

double
dwg_ent_mtext_get_linespace_factor(dwg_ent_mtext *mtext, int *error);

void
dwg_ent_mtext_set_linespace_factor(dwg_ent_mtext *mtext, double factor,
                                   int *error);


/********************************************************************
*                   FUNCTIONS FOR LEADER ENTITY                     *
********************************************************************/


void
dwg_ent_leader_set_annot_type(dwg_ent_leader *leader, unsigned int type,
                              int *error);

unsigned int
dwg_ent_leader_get_annot_type(dwg_ent_leader *leader, int *error);

void
dwg_ent_leader_set_path_type(dwg_ent_leader *leader, unsigned int type,
                             int *error);

unsigned int
dwg_ent_leader_get_path_type(dwg_ent_leader *leader, int *error);

long
dwg_ent_leader_get_numpts(dwg_ent_leader *leader, int *error);

void
dwg_ent_leader_set_numpts(dwg_ent_leader *leader, long numpts, int *error);

void
dwg_ent_leader_set_end_pt_proj(dwg_ent_leader *leader, dwg_point_3d *point,
                               int *error);

void
dwg_ent_leader_get_end_pt_proj(dwg_ent_leader *leader, dwg_point_3d *point,
                               int *error);

void
dwg_ent_leader_set_extrusion(dwg_ent_leader *leader, dwg_point_3d *point,
                             int *error);

void
dwg_ent_leader_get_extrusion(dwg_ent_leader *leader, dwg_point_3d *point,
                             int *error);

void
dwg_ent_leader_set_x_direction(dwg_ent_leader *leader, dwg_point_3d *point,
                               int *error);

void
dwg_ent_leader_get_x_direction(dwg_ent_leader *leader, dwg_point_3d *point,
                               int *error);

void
dwg_ent_leader_set_offset_to_block_ins_pt(dwg_ent_leader *leader,
                                          dwg_point_3d *point, int *error);

void
dwg_ent_leader_get_offset_to_block_ins_pt(dwg_ent_leader *leader,
                                          dwg_point_3d *point, int *error);

void
dwg_ent_leader_set_dimgap(dwg_ent_leader *leader, double dimgap, int *error);

double
dwg_ent_leader_get_dimgap(dwg_ent_leader *leader, int *error);

void
dwg_ent_leader_set_box_height(dwg_ent_leader *leader, double height,
                              int *error);

double
dwg_ent_leader_get_box_height(dwg_ent_leader *leader, int *error);

void
dwg_ent_leader_set_box_width(dwg_ent_leader *leader, double width,
                             int *error);

double
dwg_ent_leader_get_box_width(dwg_ent_leader *leader, int *error);

void
dwg_ent_leader_set_hook_line_on_x_dir(dwg_ent_leader *leader, char hook,
                                      int *error);

char
dwg_ent_leader_get_hook_line_on_x_dir(dwg_ent_leader *leader, int *error);

void
dwg_ent_leader_set_arrowhead_on(dwg_ent_leader *leader, char arrow,
                                int *error);

char
dwg_ent_leader_get_arrowhead_on(dwg_ent_leader *leader, int *error);

void
dwg_ent_leader_set_arrowhead_type(dwg_ent_leader *leader, unsigned int type,
                                  int *error);

unsigned int
dwg_ent_leader_get_arrowhead_type(dwg_ent_leader *leader, int *error);

void
dwg_ent_leader_set_dimasz(dwg_ent_leader *leader, double dimasz, int *error);

double
dwg_ent_leader_get_dimasz(dwg_ent_leader *leader, int *error);

void
dwg_ent_leader_set_byblock_color(dwg_ent_leader *leader, unsigned int color,
                                 int *error);

unsigned int
dwg_ent_leader_get_byblock_color(dwg_ent_leader *leader, int *error);


/********************************************************************
*                  FUNCTIONS FOR TOLERANCE ENTITY                   *
********************************************************************/


void
dwg_ent_tolerance_set_height(dwg_ent_tolerance *tol, double height,
                             int *error);

double
dwg_ent_tolerance_get_height(dwg_ent_tolerance *tol, int *error);

void
dwg_ent_tolerance_set_dimgap(dwg_ent_tolerance *tol, double dimgap,
                             int *error);

double
dwg_ent_tolerance_get_dimgap(dwg_ent_tolerance *tol, int *error);

void
dwg_ent_tolerance_set_ins_pt(dwg_ent_tolerance *tol, dwg_point_3d *point,
                             int *error);

void
dwg_ent_tolerance_get_ins_pt(dwg_ent_tolerance *tol, dwg_point_3d *point,
                             int *error);

void
dwg_ent_tolerance_set_x_direction(dwg_ent_tolerance *tol, dwg_point_3d *point,
                                  int *error);

void
dwg_ent_tolerance_get_x_direction(dwg_ent_tolerance *tol, dwg_point_3d *point,
                                  int *error);

void
dwg_ent_tolerance_set_extrusion(dwg_ent_tolerance *tol, dwg_point_3d *point,
                                int *error);

void
dwg_ent_tolerance_get_extrusion(dwg_ent_tolerance *tol, dwg_point_3d *point,
                                int *error);

void
dwg_ent_tolerance_set_text_string(dwg_ent_tolerance *tol, char *string,
                                  int *error);

char *
dwg_ent_tolerance_get_text_string(dwg_ent_tolerance *tol, int *error);


/********************************************************************
*                   FUNCTIONS FOR LWPLINE ENTITY                    *
********************************************************************/


char
dwg_ent_lwpline_get_flags(dwg_ent_lwpline *lwpline, int *error);

void
dwg_ent_lwpline_set_flags(dwg_ent_lwpline *lwpline, char flags, int *error);

double
dwg_ent_lwpline_get_const_width(dwg_ent_lwpline *lwpline, int *error);

void
dwg_ent_lwpline_set_const_width(dwg_ent_lwpline *lwpline, double const_width,
                                int *error);

double
dwg_ent_lwpline_get_elevation(dwg_ent_lwpline *lwpline, int *error);

void
dwg_ent_lwpline_set_elevation(dwg_ent_lwpline *lwpline, double elevation,
                              int *error);

double
dwg_ent_lwpline_get_thickness(dwg_ent_lwpline *lwpline, int *error);

void
dwg_ent_lwpline_set_thickness(dwg_ent_lwpline *lwpline, double thickness,
                              int *error);

long
dwg_ent_lwpline_get_num_points(dwg_ent_lwpline *lwpline, int *error);

void
dwg_ent_lwpline_set_num_points(dwg_ent_lwpline *lwpline, long num_points,
                               int *error);

long
dwg_ent_lwpline_get_num_bulges(dwg_ent_lwpline *lwpline, int *error);

void
dwg_ent_lwpline_set_num_bulges(dwg_ent_lwpline *lwpline, long num_bulges,
                               int *error);

long
dwg_ent_lwpline_get_num_widths(dwg_ent_lwpline *lwpline, int *error);

void
dwg_ent_lwpline_set_num_widths(dwg_ent_lwpline *lwpline, long num_widths,
                               int *error);

void
dwg_ent_lwpline_get_normal(dwg_ent_lwpline *lwpline, dwg_point_3d *points,
                           int *error);

void
dwg_ent_lwpline_set_normal(dwg_ent_lwpline *lwpline, dwg_point_3d *points,
                           int *error);

double *
dwg_ent_lwpline_get_bulges(dwg_ent_lwpline *lwpline, int *error);

dwg_point_2d *
dwg_ent_lwpline_get_points(dwg_ent_lwpline *lwpline, int *error);

dwg_lwpline_widths *
dwg_ent_lwpline_get_widths(dwg_ent_lwpline *lwpline, int *error);


/********************************************************************
*                  FUNCTIONS FOR OLE2FRAME ENTITY                   *
********************************************************************/


unsigned int
dwg_ent_ole2frame_get_flags(dwg_ent_ole2frame *frame, int *error);

void
dwg_ent_ole2frame_set_flags(dwg_ent_ole2frame *frame, unsigned int flags,
                            int *error);

unsigned int
dwg_ent_ole2frame_get_mode(dwg_ent_ole2frame *frame, int *error);

void
dwg_ent_ole2frame_set_mode(dwg_ent_ole2frame *frame, unsigned int mode,
                           int *error);

long
dwg_ent_ole2frame_get_data_length(dwg_ent_ole2frame *frame, int *error);

void
dwg_ent_ole2frame_set_data_length(dwg_ent_ole2frame *frame, long data_length,
                                  int *error);

char *
dwg_ent_ole2frame_get_data(dwg_ent_ole2frame *frame, int *error);

void
dwg_ent_ole2frame_set_data(dwg_ent_ole2frame *frame, char *data, int *error);


/********************************************************************
*                   FUNCTIONS FOR SPLINE ENTITY                     *
********************************************************************/


unsigned int
dwg_entity_spline_get_scenario(dwg_ent_spline *spline, int *error);

void
dwg_entity_spline_set_scenario(dwg_ent_spline *spline, unsigned int scenario,
                               int *error);

unsigned int
dwg_entity_spline_get_degree(dwg_ent_spline *spline, int *error);

void
dwg_entity_spline_set_degree(dwg_ent_spline *spline, unsigned int degree,
                             int *error);

double
dwg_entity_spline_get_fit_tol(dwg_ent_spline *spline, int *error);

void
dwg_entity_spline_set_fit_tol(dwg_ent_spline *spline, int fit_tol,
                              int *error);

void
dwg_entity_spline_get_begin_tan_vector(dwg_ent_spline *spline,
                                       dwg_point_3d *point, int *error);

void
dwg_entity_spline_set_begin_tan_vector(dwg_ent_spline *spline,
                                       dwg_point_3d *point, int *error);

void
dwg_entity_spline_get_end_tan_vector(dwg_ent_spline *spline,
                                     dwg_point_3d *point, int *error);

void
dwg_entity_spline_set_end_tan_vector(dwg_ent_spline *spline,
                                     dwg_point_3d *point, int *error);

double
dwg_entity_spline_get_knot_tol(dwg_ent_spline *spline, int *error);

void
dwg_entity_spline_set_knot_tol(dwg_ent_spline *spline, double knot_tol,
                               int *error);

double
dwg_entity_spline_get_ctrl_tol(dwg_ent_spline *spline, int *error);

void
dwg_entity_spline_set_ctrl_tol(dwg_ent_spline *spline, double ctrl_tol,
                               int *error);

unsigned int
dwg_entity_spline_get_num_fit_pts(dwg_ent_spline *spline, int *error);

void
dwg_entity_spline_set_num_fit_pts(dwg_ent_spline *spline, int num_fit_pts,
                                  int *error);

char
dwg_entity_spline_get_rational(dwg_ent_spline *spline, int *error);

void
dwg_entity_spline_set_rational(dwg_ent_spline *spline, char rational,
                               int *error);

char
dwg_entity_spline_get_closed_b(dwg_ent_spline *spline, int *error);

void
dwg_entity_spline_set_closed_b(dwg_ent_spline *spline, char closed_b,
                               int *error);

char
dwg_entity_spline_get_weighted(dwg_ent_spline *spline, int *error);

void
dwg_entity_spline_set_weighted(dwg_ent_spline *spline, char weighted,
                               int *error);

char
dwg_entity_spline_get_periodic(dwg_ent_spline *spline, int *error);

void
dwg_entity_spline_set_periodic(dwg_ent_spline *spline, char periodic,
                               int *error);

long
dwg_entity_spline_get_num_knots(dwg_ent_spline *spline, int *error);

void
dwg_entity_spline_set_num_knots(dwg_ent_spline *spline, long nums, int *error);

long
dwg_entity_spline_get_num_ctrl_pts(dwg_ent_spline *spline, int *error);

void
dwg_entity_spline_set_num_ctrl_pts(dwg_ent_spline *spline, long nums,
                                   int *error);

dwg_ent_spline_point *
dwg_ent_spline_get_fit_points(dwg_ent_spline *spline, int *error);

dwg_ent_spline_control_point *
dwg_ent_spline_get_ctrl_pts(dwg_ent_spline *spline, int *error);

double *
dwg_ent_spline_get_knots(dwg_ent_spline *spline, int *error);


/********************************************************************
*                   FUNCTIONS FOR VIEWPORT ENTITY                   *
********************************************************************/


void
dwg_ent_viewport_get_center(dwg_ent_viewport *vp, dwg_point_3d *point,
                            int *error);

void
dwg_ent_viewport_set_center(dwg_ent_viewport *vp, dwg_point_3d *point,
                            int *error);

double
dwg_ent_viewport_get_width(dwg_ent_viewport *vp, int *error);

void
dwg_ent_viewport_set_width(dwg_ent_viewport *vp, double width, int *error);

double
dwg_ent_viewport_get_height(dwg_ent_viewport *vp, int *error);

void
dwg_ent_viewport_set_height(dwg_ent_viewport *vp, double height, int *error);

unsigned int
dwg_ent_viewport_get_grid_major(dwg_ent_viewport *vp, int *error);

void
dwg_ent_viewport_set_grid_major(dwg_ent_viewport *vp, unsigned int major,
                                int *error);

long
dwg_ent_viewport_get_frozen_layer_count(dwg_ent_viewport *vp, int *error);

void
dwg_ent_viewport_set_frozen_layer_count(dwg_ent_viewport *vp, long count,
                                        int *error);

char *
dwg_ent_viewport_get_style_sheet(dwg_ent_viewport *vp, int *error);

void
dwg_ent_viewport_set_style_sheet(dwg_ent_viewport *vp, char *sheet,
                                 int *error);

void
dwg_ent_viewport_set_circle_zoom(dwg_ent_viewport *vp, unsigned int zoom,
                                 int *error);

unsigned int
dwg_ent_viewport_get_circle_zoom(dwg_ent_viewport *vp, int *error);

void
dwg_ent_viewport_set_status_flags(dwg_ent_viewport *vp, long flags,
                                  int *error);

long
dwg_ent_viewport_get_status_flags(dwg_ent_viewport *vp, int *error);

char
dwg_ent_viewport_get_render_mode(dwg_ent_viewport *vp, int *error);

void
dwg_ent_viewport_set_render_mode(dwg_ent_viewport *vp, char mode, int *error);

void
dwg_ent_viewport_set_ucs_at_origin(dwg_ent_viewport *vp, unsigned char origin,
                                   int *error);

unsigned char
dwg_ent_viewport_get_ucs_at_origin(dwg_ent_viewport *vp, int *error);

void
dwg_ent_viewport_set_ucs_per_viewport(dwg_ent_viewport *vp,
                                      unsigned char viewport, int *error);

unsigned char
dwg_ent_viewport_get_ucs_per_viewport(dwg_ent_viewport *vp, int *error);

void
dwg_ent_viewport_set_view_target(dwg_ent_viewport *vp, dwg_point_3d *point,
                                 int *error);

void
dwg_ent_viewport_get_view_target(dwg_ent_viewport *vp, dwg_point_3d *point,
                                 int *error);

void
dwg_ent_viewport_set_view_direction(dwg_ent_viewport *vp, dwg_point_3d *point,
                                    int *error);

void
dwg_ent_viewport_get_view_direction(dwg_ent_viewport *vp, dwg_point_3d *point,
                                    int *error);

void
dwg_ent_viewport_set_view_twist_angle(dwg_ent_viewport *vp, double angle,
                                      int *error);

double
dwg_ent_viewport_get_view_twist_angle(dwg_ent_viewport *vp, int *error);

void
dwg_ent_viewport_set_view_height(dwg_ent_viewport *vp, double height,
                                 int *error);

double
dwg_ent_viewport_get_view_height(dwg_ent_viewport *vp, int *error);

void
dwg_ent_viewport_set_lens_length(dwg_ent_viewport *vp, double length,
                                 int *error);

double
dwg_ent_viewport_get_lens_length(dwg_ent_viewport *vp, int *error);

void
dwg_ent_viewport_set_front_clip_z(dwg_ent_viewport *vp, double front_z,
                                  int *error);

double
dwg_ent_viewport_get_front_clip_z(dwg_ent_viewport *vp, int *error);

void
dwg_ent_viewport_set_back_clip_z(dwg_ent_viewport *vp, double back_z,
                                 int *error);

double
dwg_ent_viewport_get_back_clip_z(dwg_ent_viewport *vp, int *error);

void
dwg_ent_viewport_set_snap_angle(dwg_ent_viewport *vp, double angle,
                                int *error);

double
dwg_ent_viewport_get_snap_angle(dwg_ent_viewport *vp, int *error);

void
dwg_ent_viewport_get_view_center(dwg_ent_viewport *vp, dwg_point_2d *point,
                                 int *error);

void
dwg_ent_viewport_set_view_center(dwg_ent_viewport *vp, dwg_point_2d *point,
                                 int *error);

void
dwg_ent_viewport_get_grid_spacing(dwg_ent_viewport *vp, dwg_point_2d *point,
                                  int *error);

void
dwg_ent_viewport_set_grid_spacing(dwg_ent_viewport *vp, dwg_point_2d *point,
                                  int *error);

void
dwg_ent_viewport_get_snap_base(dwg_ent_viewport *vp, dwg_point_2d *point,
                               int *error);

void
dwg_ent_viewport_set_snap_base(dwg_ent_viewport *vp, dwg_point_2d *point,
                               int *error);

void
dwg_ent_viewport_get_snap_spacing(dwg_ent_viewport *vp, dwg_point_2d *point,
                                  int *error);

void
dwg_ent_viewport_set_snap_spacing(dwg_ent_viewport *vp, dwg_point_2d *point,
                                  int *error);

void
dwg_ent_viewport_set_ucs_origin(dwg_ent_viewport *vp, dwg_point_3d *point,
                                int *error);

void
dwg_ent_viewport_get_ucs_origin(dwg_ent_viewport *vp, dwg_point_3d *point,
                                int *error);

void
dwg_ent_viewport_set_ucs_x_axis(dwg_ent_viewport *vp, dwg_point_3d *point,
                                int *error);

void
dwg_ent_viewport_get_ucs_x_axis(dwg_ent_viewport *vp, dwg_point_3d *point,
                                int *error);

void
dwg_ent_viewport_set_ucs_y_axis(dwg_ent_viewport *vp, dwg_point_3d *point,
                                int *error);

void
dwg_ent_viewport_get_ucs_y_axis(dwg_ent_viewport *vp, dwg_point_3d *point,
                                int *error);

void
dwg_ent_viewport_set_ucs_elevation(dwg_ent_viewport *vp, double elevation,
                                   int *error);

double
dwg_ent_viewport_get_ucs_elevation(dwg_ent_viewport *vp, int *error);

void
dwg_ent_viewport_set_ucs_ortho_view_type(dwg_ent_viewport *vp, 
                                         unsigned int type, int *error);

unsigned int
dwg_ent_viewport_get_ucs_ortho_view_type(dwg_ent_viewport *vp, int *error);

void
dwg_ent_viewport_set_shadeplot_mode(dwg_ent_viewport *vp,
                                    unsigned int shadeplot, int *error);

unsigned int
dwg_ent_viewport_get_shadeplot_mode(dwg_ent_viewport *vp, int *error);

void
dwg_ent_viewport_set_use_def_lights(dwg_ent_viewport *vp,
                                    unsigned char lights, int *error);

unsigned char
dwg_ent_viewport_get_use_def_lights(dwg_ent_viewport *vp, int *error);

void
dwg_ent_viewport_set_def_lighting_type(dwg_ent_viewport *vp, char type,
                                       int *error);

char
dwg_ent_viewport_get_def_lighting_type(dwg_ent_viewport *vp, int *error);

void
dwg_ent_viewport_set_brightness(dwg_ent_viewport *vp, double brightness,
                                int *error);

double
dwg_ent_viewport_get_brightness(dwg_ent_viewport *vp, int *error);

void
dwg_ent_viewport_set_contrast(dwg_ent_viewport *vp, double contrast,
                              int *error);

double
dwg_ent_viewport_get_contrast(dwg_ent_viewport *vp, int *error);


/********************************************************************
*                FUNCTIONS FOR POLYLINE PFACE ENTITY                *
********************************************************************/


unsigned int
dwg_ent_polyline_pface_get_numverts(dwg_ent_polyline_pface *pface, int *error);

void
dwg_ent_polyline_pface_set_numverts(dwg_ent_polyline_pface *pface,
                                    unsigned int numverts, int *error);

long
dwg_ent_polyline_pface_get_owned_obj_count(dwg_ent_polyline_pface *pface,
                                           int *error);

void
dwg_ent_polyline_pface_set_owned_obj_count(dwg_ent_polyline_pface *pface,
                                           long owned_obj_count, int *error);

unsigned int
dwg_ent_polyline_pface_get_numfaces(dwg_ent_polyline_pface *pface, int *error);

void
dwg_ent_polyline_pface_set_numfaces(dwg_ent_polyline_pface *pface,
                                    unsigned int numfaces, int *error);


/********************************************************************
*                FUNCTIONS FOR POLYLINE_MESH ENTITY                 *
********************************************************************/


unsigned int
dwg_ent_polyline_mesh_get_flags(dwg_ent_polyline_mesh *mesh, int *error);

void
dwg_ent_polyline_mesh_set_flags(dwg_ent_polyline_mesh *mesh,
                                unsigned int flags, int *error);

unsigned int
dwg_ent_polyline_mesh_get_curve_type(dwg_ent_polyline_mesh *mesh, int *error);

void
dwg_ent_polyline_mesh_set_curve_type(dwg_ent_polyline_mesh *mesh,
                                     unsigned int curve_type, int *error);

unsigned int
dwg_ent_polyline_mesh_get_m_vert_count(dwg_ent_polyline_mesh *mesh,
                                       int *error);

void
dwg_ent_polyline_mesh_set_m_vert_count(dwg_ent_polyline_mesh *mesh,
                                       unsigned int m_vert_count, int *error);

unsigned int
dwg_ent_polyline_mesh_get_n_vert_count(dwg_ent_polyline_mesh *mesh,
                                       int *error);

void
dwg_ent_polyline_mesh_set_n_vert_count(dwg_ent_polyline_mesh *mesh,
                                       unsigned int n_vert_count, int *error);

unsigned int
dwg_ent_polyline_mesh_get_m_density(dwg_ent_polyline_mesh *mesh, int *error);

void
dwg_ent_polyline_mesh_set_m_density(dwg_ent_polyline_mesh *mesh,
                                    unsigned int m_density, int *error);

unsigned int
dwg_ent_polyline_mesh_get_n_density(dwg_ent_polyline_mesh *mesh, int *error);

void
dwg_ent_polyline_mesh_set_n_density(dwg_ent_polyline_mesh *mesh,
                                    unsigned int n_density, int *error);

long
dwg_ent_polyline_mesh_get_owned_obj_count(dwg_ent_polyline_mesh *mesh,
                                          int *error);

void
dwg_ent_polyline_mesh_set_owned_obj_count(dwg_ent_polyline_mesh *mesh,
                                          long owned_obj_count, int *error);


/********************************************************************
*                 FUNCTIONS FOR POLYLINE_2D ENTITY                  *
********************************************************************/


void
dwg_ent_polyline_2d_get_extrusion(dwg_ent_polyline_2d *line2d,
                                  dwg_point_3d *point, int *error);

void
dwg_ent_polyline_2d_set_extrusion(dwg_ent_polyline_2d *line2d,
                                  dwg_point_3d *point, int *error);

double
dwg_ent_polyline_2d_get_start_width(dwg_ent_polyline_2d *line2d, int *error);

void
dwg_ent_polyline_2d_set_start_width(dwg_ent_polyline_2d *line2d,
                                    double start_width, int *error);

double
dwg_ent_polyline_2d_get_end_width(dwg_ent_polyline_2d *line2d, int *error);

void
dwg_ent_polyline_2d_set_end_width(dwg_ent_polyline_2d *line2d,
                                  double end_width, int *error);

double
dwg_ent_polyline_2d_get_thickness(dwg_ent_polyline_2d *line2d, int *error);

void
dwg_ent_polyline_2d_set_thickness(dwg_ent_polyline_2d *line2d,
                                  double thickness, int *error);

double
dwg_ent_polyline_2d_get_elevation(dwg_ent_polyline_2d *line2d, int *error);

void
dwg_ent_polyline_2d_set_elevation(dwg_ent_polyline_2d *line2d,
                                  double elevation, int *error);

unsigned int
dwg_ent_polyline_2d_get_flags(dwg_ent_polyline_2d *line2d, int *error);

void
dwg_ent_polyline_2d_set_flags(dwg_ent_polyline_2d *line2d, unsigned int flags,
                              int *error);

unsigned int
dwg_ent_polyline_2d_get_curve_type(dwg_ent_polyline_2d *line2d, int *error);

void
dwg_ent_polyline_2d_set_curve_type(dwg_ent_polyline_2d *line2d,
                                   unsigned int curve_type, int *error);

long
dwg_ent_polyline_2d_get_owned_obj_count(dwg_ent_polyline_2d *line2d,
                                        int *error);

void
dwg_ent_polyline_2d_set_owned_obj_count(dwg_ent_polyline_2d *line2d,
                                        long owned_obj_count, int *error);


/********************************************************************
*                 FUNCTIONS FOR POLYLINE_3D ENTITY                  *
********************************************************************/


char
dwg_ent_polyline_3d_get_flags_1(dwg_ent_polyline_3d *line3d, int *error);

void
dwg_ent_polyline_3d_set_flags_1(dwg_ent_polyline_3d *line3d, char flags_1,
                                int *error);

char
dwg_ent_polyline_3d_get_flags_2(dwg_ent_polyline_3d *line3d, int *error);

void
dwg_ent_polyline_3d_set_flags_2(dwg_ent_polyline_3d *line3d, char flags_2,
                                int *error);

long
dwg_ent_polyline_3d_get_owned_obj_count(dwg_ent_polyline_3d *line3d,
                                        int *error);

void
dwg_ent_polyline_3d_set_owned_obj_count(dwg_ent_polyline_3d *line3d,
                                        long owned_obj_count, int *error);


/********************************************************************
*                   FUNCTIONS FOR 3DFACE ENTITY                     *
********************************************************************/


unsigned int
dwg_ent_3dface_get_invis_flags(dwg_ent_3dface *_3dface, int *error);

void
dwg_ent_3dface_set_invis_flags(dwg_ent_3dface *_3dface,
                               unsigned int invis_flags, int *error);

void
dwg_ent_3dface_get_corner1(dwg_ent_3dface *_3dface, dwg_point_2d *point,
                           int *error);

void
dwg_ent_3dface_set_corner1(dwg_ent_3dface *_3dface, dwg_point_2d *point,
                           int *error);

void
dwg_ent_3dface_get_corner2(dwg_ent_3dface *_3dface, dwg_point_2d *point,
                           int *error);

void
dwg_ent_3dface_set_corner2(dwg_ent_3dface *_3dface, dwg_point_2d *point,
                           int *error);

void
dwg_ent_3dface_get_corner3(dwg_ent_3dface *_3dface, dwg_point_2d *point,
                           int *error);

void
dwg_ent_3dface_set_corner3(dwg_ent_3dface *_3dface, dwg_point_2d *point,
                           int *error);

void
dwg_ent_3dface_get_corner4(dwg_ent_3dface *_3dface, dwg_point_2d *point,
                           int *error);

void
dwg_ent_3dface_set_corner4(dwg_ent_3dface *_3dface, dwg_point_2d *point,
                           int *error);


/********************************************************************
*                    FUNCTIONS FOR IMAGE ENTITY                     *
********************************************************************/


long
dwg_ent_image_get_class_version(dwg_ent_image *image, int *error);

void
dwg_ent_image_set_class_version(dwg_ent_image *image, long class_version,
                                int *error);

void
dwg_ent_image_get_pt0(dwg_ent_image *image, dwg_point_3d *point, int *error);

void
dwg_ent_image_set_pt0(dwg_ent_image *image, dwg_point_3d *point, int *error);

void
dwg_ent_image_get_u_vector(dwg_ent_image *image, dwg_point_3d *point,
                           int *error);

void
dwg_ent_image_set_u_vector(dwg_ent_image *image, dwg_point_3d *point,
                           int *error);

void
dwg_ent_image_get_v_vector(dwg_ent_image *image, dwg_point_3d *point,
                           int *error);

void
dwg_ent_image_set_v_vector(dwg_ent_image *image, dwg_point_3d *point,
                           int *error);

double
dwg_ent_image_get_size_height(dwg_ent_image *image, int *error);

void
dwg_ent_image_set_size_height(dwg_ent_image *image, double size_height,
                              int *error);

double
dwg_ent_image_get_size_width(dwg_ent_image *image, int *error);

void
dwg_ent_image_set_size_width(dwg_ent_image *image, double size_width,
                             int *error);

unsigned int
dwg_ent_image_get_display_props(dwg_ent_image *image, int *error);

void
dwg_ent_image_set_display_props(dwg_ent_image *image,
                                unsigned int display_props, int *error);

unsigned char
dwg_ent_image_get_clipping(dwg_ent_image *image, int *error);

void
dwg_ent_image_set_clipping(dwg_ent_image *image, unsigned char clipping,
                           int *error);

char
dwg_ent_image_get_brightness(dwg_ent_image *image, int *error);

void
dwg_ent_image_set_brightness(dwg_ent_image *image, char brightness,
                             int *error);

char
dwg_ent_image_get_contrast(dwg_ent_image *image, int *error);

void
dwg_ent_image_set_contrast(dwg_ent_image *image, char contrast, int *error);

char
dwg_ent_image_get_fade(dwg_ent_image *image, int *error);

void
dwg_ent_image_set_fade(dwg_ent_image *image, char fade, int *error);

unsigned int
dwg_ent_image_get_clip_boundary_type(dwg_ent_image *image, int *error);

void
dwg_ent_image_set_clip_boundary_type(dwg_ent_image *image, unsigned int type,
                                    int *error);

void
dwg_ent_image_get_boundary_pt0(dwg_ent_image *image, dwg_point_2d *point,
                               int *error);

void
dwg_ent_image_set_boundary_pt0(dwg_ent_image *image, dwg_point_2d *point,
                               int *error);

void
dwg_ent_image_get_boundary_pt1(dwg_ent_image *image, dwg_point_2d *point,
                               int *error);

void
dwg_ent_image_set_boundary_pt1(dwg_ent_image *image, dwg_point_2d *point,
                               int *error);

double
dwg_ent_image_get_num_clip_verts(dwg_ent_image *image, int *error);

void
dwg_ent_image_set_num_clip_verts(dwg_ent_image *image, double num, int *error);

dwg_ent_image_clip_vert *
dwg_ent_image_get_clip_verts(dwg_ent_image *image, int *error);


/********************************************************************
*                    FUNCTIONS FOR MLINE ENTITY                     *
********************************************************************/


void
dwg_ent_mline_set_scale(dwg_ent_mline *mline, double scale, int *error);

double
dwg_ent_mline_get_scale(dwg_ent_mline *mline, int *error);

void
dwg_ent_mline_set_just(dwg_ent_mline *mline, char just, int *error);

char
dwg_ent_mline_get_just(dwg_ent_mline *mline, int *error);

void
dwg_ent_mline_set_base_point(dwg_ent_mline *mline, dwg_point_3d *point,
                             int *error);

void
dwg_ent_mline_get_base_point(dwg_ent_mline *mline, dwg_point_3d *point,
                             int *error);

void
dwg_ent_mline_set_extrusion(dwg_ent_mline *mline, dwg_point_3d *point,
                            int *error);

void
dwg_ent_mline_get_extrusion(dwg_ent_mline *mline, dwg_point_3d *point,
                            int *error);

void
dwg_ent_mline_set_open_closed(dwg_ent_mline *mline, unsigned int oc,
                              int *error);

unsigned int
dwg_ent_mline_get_open_closed(dwg_ent_mline *mline, int *error);

void
dwg_ent_mline_set_num_lines(dwg_ent_mline *mline, char num, int *error);

char
dwg_ent_mline_get_num_lines(dwg_ent_mline *mline, int *error);

void
dwg_ent_mline_set_num_verts(dwg_ent_mline *mline, unsigned int num,
                            int *error);

unsigned int
dwg_ent_mline_get_num_verts(dwg_ent_mline *mline, int *error);

dwg_ent_mline_vert *
dwg_ent_mline_get_verts(dwg_ent_mline *mline, int *error);


/********************************************************************
*                  FUNCTIONS FOR 3DSOLID ENTITY                     *
********************************************************************/


unsigned char
dwg_ent_3dsolid_get_acis_empty(dwg_ent_3dsolid *_3dsolid, int *error);

void
dwg_ent_3dsolid_set_acis_empty(dwg_ent_3dsolid *_3dsolid, unsigned char acis,
                               int *error);

unsigned int
dwg_ent_3dsolid_get_version(dwg_ent_3dsolid *_3dsolid, int *error);

void
dwg_ent_3dsolid_set_version(dwg_ent_3dsolid *_3dsolid, unsigned int version,
                            int *error);

long *
dwg_ent_3dsolid_get_block_size(dwg_ent_3dsolid *_3dsolid, int *error);

char *
dwg_ent_3dsolid_get_acis_data(dwg_ent_3dsolid *_3dsolid, int *error);

char *
dwg_ent_3dsolid_set_acis_data(dwg_ent_3dsolid *_3dsolid, char *data,
                              int *error);

char
dwg_ent_3dsolid_get_wireframe_data_present(dwg_ent_3dsolid *_3dsolid,
                                           int *error);

void
dwg_ent_3dsolid_set_wireframe_data_present(dwg_ent_3dsolid *_3dsolid,
                                           char data, int *error);

char
dwg_ent_3dsolid_get_point_present(dwg_ent_3dsolid *_3dsolid, int *error);

void
dwg_ent_3dsolid_set_point_present(dwg_ent_3dsolid *_3dsolid, char point,
                                  int *error);

void
dwg_ent_3dsolid_get_point(dwg_ent_3dsolid *_3dsolid, dwg_point_3d *point, 
                          int *error);

void
dwg_ent_3dsolid_set_point(dwg_ent_3dsolid *_3dsolid, dwg_point_3d *point, 
                          int *error);

long
dwg_ent_3dsolid_get_num_isolines(dwg_ent_3dsolid *_3dsolid, int *error);

void
dwg_ent_3dsolid_set_num_isolines(dwg_ent_3dsolid *_3dsolid, long num,
                                 int *error);

char
dwg_ent_3dsolid_get_isoline_present(dwg_ent_3dsolid *_3dsolid, int *error);

void
dwg_ent_3dsolid_set_isoline_present(dwg_ent_3dsolid *_3dsolid, char iso,
                                    int *error);

long
dwg_ent_3dsolid_get_num_wires(dwg_ent_3dsolid *_3dsolid, int *error);

void
dwg_ent_3dsolid_set_num_wires(dwg_ent_3dsolid *_3dsolid, long num, int *error);

dwg_ent_solid_wire *
dwg_ent_3dsolid_get_wire(dwg_ent_3dsolid *_3dsolid, int *error);

long
dwg_ent_3dsolid_get_num_silhouettes(dwg_ent_3dsolid *_3dsolid, int *error);

void
dwg_ent_3dsolid_set_num_silhouettes(dwg_ent_3dsolid *_3dsolid,
                                    long silhouettes, int *error);

dwg_ent_solid_silhouette *
dwg_ent_3dsolid_get_silhouette(dwg_ent_3dsolid *_3dsolid, int *error);

unsigned char
dwg_ent_3dsolid_get_acis_empty2(dwg_ent_3dsolid *_3dsolid, int *error);

void
dwg_ent_3dsolid_set_acis_empty2(dwg_ent_3dsolid *_3dsolid, unsigned char acis,
                                int *error); 


/********************************************************************
*                   FUNCTIONS FOR REGION ENTITY                     *
********************************************************************/


unsigned char
dwg_ent_region_get_acis_empty(dwg_ent_region *region, int *error);

void
dwg_ent_region_set_acis_empty(dwg_ent_region *region, unsigned char acis,
                              int *error);

unsigned int
dwg_ent_region_get_version(dwg_ent_region *region, int *error);

void
dwg_ent_region_set_version(dwg_ent_region *region, unsigned int version,
                           int *error);

long *
dwg_ent_region_get_block_size(dwg_ent_region *region, int *error);

char *
dwg_ent_region_get_acis_data(dwg_ent_region *region, int *error);

char *
dwg_ent_region_set_acis_data(dwg_ent_region *region, char *data, int *error);

char
dwg_ent_region_get_wireframe_data_present(dwg_ent_region *region, int *error);

void
dwg_ent_region_set_wireframe_data_present(dwg_ent_region *region, char data,
                                          int *error);

char
dwg_ent_region_get_point_present(dwg_ent_region *region, int *error);

void
dwg_ent_region_set_point_present(dwg_ent_region *region, char point,
                                 int *error);

void
dwg_ent_region_get_point(dwg_ent_region *region, dwg_point_3d *point,
                         int *error);

void
dwg_ent_region_set_point(dwg_ent_region *region, dwg_point_3d *point,
                         int *error);

long
dwg_ent_region_get_num_isolines(dwg_ent_region *region, int *error);

void
dwg_ent_region_set_num_isolines(dwg_ent_region *region, long num, int *error);

char
dwg_ent_region_get_isoline_present(dwg_ent_region *region, int *error);

void
dwg_ent_region_set_isoline_present(dwg_ent_region *region, char iso,
                                   int *error);

long
dwg_ent_region_get_num_wires(dwg_ent_region *region, int *error);

void
dwg_ent_region_set_num_wires(dwg_ent_region *region, long num, int *error);

dwg_ent_solid_wire *
dwg_ent_region_get_wire(dwg_ent_region *region, int *error);

long
dwg_ent_region_get_num_silhouettes(dwg_ent_region *region, int *error);

void
dwg_ent_region_set_num_silhouettes(dwg_ent_region *region, long silhouettes,
                                   int *error);

dwg_ent_solid_silhouette *
dwg_ent_region_get_silhouette(dwg_ent_region *region, int *error);

unsigned char
dwg_ent_region_get_acis_empty2(dwg_ent_region *region, int *error);

void
dwg_ent_region_set_acis_empty2(dwg_ent_region *region, unsigned char acis,
                               int *error); 


/********************************************************************
*                    FUNCTIONS FOR BODY ENTITY                      *
********************************************************************/


unsigned char
dwg_ent_body_get_acis_empty(dwg_ent_body *body, int *error);

void
dwg_ent_body_set_acis_empty(dwg_ent_body *body, unsigned char acis,
                            int *error);

unsigned int
dwg_ent_body_get_version(dwg_ent_body *body, int *error);

void
dwg_ent_body_set_version(dwg_ent_body *body, unsigned int version, int *error);

long * 
dwg_ent_body_get_block_size(dwg_ent_body *body, int *error);

char *
dwg_ent_body_get_acis_data(dwg_ent_body *body, int *error);

char *
dwg_ent_body_set_acis_data(dwg_ent_body *body, char *data, int *error);

char
dwg_ent_body_get_wireframe_data_present(dwg_ent_body *body, int *error);

void
dwg_ent_body_set_wireframe_data_present(dwg_ent_body *body, char data,
                                        int *error);

char
dwg_ent_body_get_point_present(dwg_ent_body *body, int *error);

void
dwg_ent_body_set_point_present(dwg_ent_body *body, char point, int *error);

void
dwg_ent_body_get_point(dwg_ent_body *body, dwg_point_3d *point, int *error);

void
dwg_ent_body_set_point(dwg_ent_body *body, dwg_point_3d *point, int *error);

long
dwg_ent_body_get_num_isolines(dwg_ent_body *body, int *error);

void
dwg_ent_body_set_num_isolines(dwg_ent_body *body, long num, int *error);

char
dwg_ent_body_get_isoline_present(dwg_ent_body *body, int *error);

void
dwg_ent_body_set_isoline_present(dwg_ent_body *body, char iso, int *error);

long
dwg_ent_body_get_num_wires(dwg_ent_body *body, int *error);

void
dwg_ent_body_set_num_wires(dwg_ent_body *body, long num, int *error);

dwg_ent_solid_wire *
dwg_ent_body_get_wire(dwg_ent_body *body, int *error);

long
dwg_ent_body_get_num_silhouettes(dwg_ent_body *body, int *error);

void
dwg_ent_body_set_num_silhouettes(dwg_ent_body *body, long silhouettes,
                                 int *error);

dwg_ent_solid_silhouette *
dwg_ent_body_get_silhouette(dwg_ent_body *body, int *error);

unsigned char
dwg_ent_body_get_acis_empty2(dwg_ent_body *body, int *error);

void
dwg_ent_body_set_acis_empty2(dwg_ent_body *body, unsigned char acis,
                             int *error); 


/********************************************************************
*                    FUNCTIONS FOR TABLE ENTITY                     *
********************************************************************/


void
dwg_ent_table_set_insertion_point(dwg_ent_table *table, dwg_point_3d *point,
                                  int *error);

void
dwg_ent_table_get_insertion_point(dwg_ent_table *table, dwg_point_3d *point,
                                  int *error);

void
dwg_ent_table_set_scale(dwg_ent_table *table, dwg_point_3d *point,
                        int *error);

void
dwg_ent_table_get_scale(dwg_ent_table *table, dwg_point_3d *point,
                        int *error);

void
dwg_ent_table_set_data_flags(dwg_ent_table *table, unsigned char flags,
                             int *error);

unsigned char
dwg_ent_table_get_data_flags(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_rotation(dwg_ent_table *table, double rotation, int *error);

double
dwg_ent_table_get_rotation(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_extrusion(dwg_ent_table *table, dwg_point_3d *point,
                            int *error);

void
dwg_ent_table_get_extrusion(dwg_ent_table *table, dwg_point_3d *point,
                            int *error);

void
dwg_ent_table_set_has_attribs(dwg_ent_table *table, unsigned char attribs,
                              int *error);

unsigned char
dwg_ent_table_get_has_attribs(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_owned_object_count(dwg_ent_table *table, long count,
                                     int *error);

long
dwg_ent_table_get_owned_object_count(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_flag_for_table_value(dwg_ent_table *table,
                                       unsigned int value, int *error);

unsigned int
dwg_ent_table_get_flag_for_table_value(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_horiz_direction(dwg_ent_table *table, dwg_point_3d *point,
                                  int *error);

void
dwg_ent_table_get_horiz_direction(dwg_ent_table *table, dwg_point_3d *point,
                                  int *error);

void
dwg_ent_table_set_num_cols(dwg_ent_table *table, long num, int *error);

long
dwg_ent_table_get_num_cols(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_num_rows(dwg_ent_table *table, long num, int *error);

long
dwg_ent_table_get_num_rows(dwg_ent_table *table, int *error);

double *
dwg_ent_table_get_col_widths(dwg_ent_table *table, int *error);

double *
dwg_ent_table_get_row_heights(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_table_overrides_present(dwg_ent_table *table,
                                          unsigned char present, int *error);

unsigned char
dwg_ent_table_get_table_overrides_present(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_table_flag_override(dwg_ent_table *table, long override,
                                      int *error);

long
dwg_ent_table_get_table_flag_override(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_title_suppressed(dwg_ent_table *table, unsigned char title,
                                   int *error);

unsigned char
dwg_ent_table_get_title_suppressed(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_header_suppressed(dwg_ent_table *table, unsigned char header,
                                    int *error);

unsigned char
dwg_ent_table_get_header_suppressed(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_flow_direction(dwg_ent_table *table, unsigned int dir,
                                 int *error);

unsigned int
dwg_ent_table_get_flow_direction(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_horiz_cell_margin(dwg_ent_table *table, double margin,
                                    int *error);

double
dwg_ent_table_get_horiz_cell_margin(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_vert_cell_margin(dwg_ent_table *table, double margin,
                                   int *error);

double
dwg_ent_table_get_vert_cell_margin(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_title_row_fill_none(dwg_ent_table *table, unsigned char fill,
                                      int *error);

unsigned char
dwg_ent_table_get_title_row_fill_none(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_header_row_fill_none(dwg_ent_table *table, 
                                       unsigned char fill, int *error);

unsigned char
dwg_ent_table_get_header_row_fill_none(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_data_row_fill_none(dwg_ent_table *table, unsigned char fill,
                                     int *error);

unsigned char
dwg_ent_table_get_data_row_fill_none(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_title_row_align(dwg_ent_table *table, unsigned char fill,
                                  int *error);

unsigned int
dwg_ent_table_get_title_row_align(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_header_row_align(dwg_ent_table *table, unsigned int align,
                                   int *error);

unsigned int
dwg_ent_table_get_header_row_align(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_data_row_align(dwg_ent_table *table, unsigned int align,
                                 int *error);

unsigned int
dwg_ent_table_get_data_row_align(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_title_row_height(dwg_ent_table *table, double height,
                                   int *error);

double
dwg_ent_table_get_title_row_height(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_header_row_height(dwg_ent_table *table, double height,
                                    int *error);

double
dwg_ent_table_get_header_row_height(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_data_row_height(dwg_ent_table *table, double height,
                                  int *error);

double
dwg_ent_table_get_data_row_height(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_border_color_overrides_present(dwg_ent_table *table,
                                                 unsigned char present,
                                                 int *error);

unsigned char
dwg_ent_table_get_border_color_overrides_present(dwg_ent_table *table,
                                                 int *error);

void
dwg_ent_table_set_border_color_overrides_flag(dwg_ent_table *table,
                                              long overrides, int *error);

long
dwg_ent_table_get_border_color_overrides_flag(dwg_ent_table *table,
                                              int *error);

void
dwg_ent_table_set_border_lineweight_overrides_present(dwg_ent_table *table,
                                                      unsigned char present,
                                                      int *error);

unsigned char
dwg_ent_table_get_border_lineweight_overrides_present(dwg_ent_table *table,
                                                      int *error);

void
dwg_ent_table_set_border_lineweight_overrides_flag(dwg_ent_table *table,
                                                   long overrides, int *error);

long
dwg_ent_table_get_border_lineweight_overrides_flag(dwg_ent_table *table,
                                                   int *error);

void
dwg_ent_table_set_title_horiz_top_lineweight(dwg_ent_table *table,
                                             unsigned int lw, int *error);

unsigned int
dwg_ent_table_get_title_horiz_top_lineweight(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_title_horiz_ins_lineweight(dwg_ent_table *table,
                                             unsigned int lw, int *error);

unsigned int
dwg_ent_table_get_title_horiz_ins_lineweight(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_title_horiz_bottom_lineweight(dwg_ent_table *table,
                                                unsigned int lw, int *error);

unsigned int
dwg_ent_table_get_title_horiz_bottom_lineweight(dwg_ent_table *table,
                                                int *error);

void
dwg_ent_table_set_title_vert_left_lineweight(dwg_ent_table *table,
                                             unsigned int lw, int *error);

unsigned int
dwg_ent_table_get_title_vert_left_lineweight(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_title_vert_ins_lineweight(dwg_ent_table *table,
                                            unsigned int lw, int *error);

unsigned int
dwg_ent_table_get_title_vert_ins_lineweight(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_title_vert_right_lineweight(dwg_ent_table *table,
                                              unsigned int lw, int *error);

unsigned int
dwg_ent_table_get_title_vert_right_lineweight(dwg_ent_table *table,
                                              int *error);

void
dwg_ent_table_set_header_horiz_top_lineweight(dwg_ent_table *table,
                                              unsigned int lw, int *error);

unsigned int
dwg_ent_table_get_header_horiz_top_lineweight(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_header_horiz_ins_lineweight(dwg_ent_table *table,
                                              unsigned int lw, int *error);

unsigned int
dwg_ent_table_get_header_horiz_ins_lineweight(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_header_horiz_bottom_lineweight(dwg_ent_table *table,
                                                 unsigned int lw, int *error);

unsigned int
dwg_ent_table_get_header_horiz_bottom_lineweight(dwg_ent_table *table,
                                                 int *error);

void
dwg_ent_table_set_header_vert_left_lineweight(dwg_ent_table *table,
                                              unsigned int lw, int *error);

unsigned int
dwg_ent_table_get_header_vert_left_lineweight(dwg_ent_table *table,
                                              int *error);

void
dwg_ent_table_set_header_vert_ins_lineweight(dwg_ent_table *table,
                                             unsigned int lw, int *error);

unsigned int
dwg_ent_table_get_header_vert_ins_lineweight(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_header_vert_right_lineweight(dwg_ent_table *table,
                                               unsigned int lw, int *error);

unsigned int
dwg_ent_table_get_header_vert_right_lineweight(dwg_ent_table *table,
                                               int *error);

void
dwg_ent_table_set_data_horiz_top_lineweight(dwg_ent_table *table,
                                            unsigned int lw, int *error);

unsigned int
dwg_ent_table_get_data_horiz_top_lineweight(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_data_horiz_ins_lineweight(dwg_ent_table *table,
                                            unsigned int lw, int *error);

unsigned int
dwg_ent_table_get_data_horiz_ins_lineweight(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_data_horiz_bottom_lineweight(dwg_ent_table *table,
                                               unsigned int lw, int *error);

unsigned int
dwg_ent_table_get_data_horiz_bottom_lineweight(dwg_ent_table *table,
                                               int *error);

void
dwg_ent_table_set_data_vert_left_lineweight(dwg_ent_table *table,
                                            unsigned int lw, int *error);

unsigned int
dwg_ent_table_get_data_vert_left_lineweight(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_data_vert_ins_lineweight(dwg_ent_table *table,
                                           unsigned int lw, int *error);

unsigned int
dwg_ent_table_get_data_vert_ins_lineweight(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_data_vert_right_lineweight(dwg_ent_table *table,
                                             unsigned int lw, int *error);

unsigned int
dwg_ent_table_get_data_vert_right_lineweight(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_border_visibility_overrides_present(dwg_ent_table *table,
                                                      unsigned char overrides,
                                                      int *error);

unsigned char
dwg_ent_table_get_border_visibility_overrides_present(dwg_ent_table *table,
                                                      int *error);

void
dwg_ent_table_set_border_visibility_overrides_flag(dwg_ent_table *table,
                                                   long overrides, int *error);

long
dwg_ent_table_get_border_visibility_overrides_flag(dwg_ent_table *table,
                                                   int *error);

void
dwg_ent_table_set_title_horiz_top_visibility(dwg_ent_table *table,
                                             unsigned int visibility,
                                             int *error);

unsigned int
dwg_ent_table_get_title_horiz_top_visibility(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_title_horiz_ins_visibility(dwg_ent_table *table,
                                             unsigned int visibility,
                                             int *error);

unsigned int
dwg_ent_table_get_title_horiz_ins_visibility(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_title_horiz_bottom_visibility(dwg_ent_table *table,
                                                unsigned int visibility,
                                                int *error);

unsigned int
dwg_ent_table_get_title_horiz_bottom_visibility(dwg_ent_table *table,
                                                int *error);

void
dwg_ent_table_set_title_vert_left_visibility(dwg_ent_table *table,
                                             unsigned int visibility,
                                             int *error);

unsigned int
dwg_ent_table_get_title_vert_left_visibility(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_title_vert_ins_visibility(dwg_ent_table *table,
                                            unsigned int visibility,
                                            int *error);

unsigned int
dwg_ent_table_get_title_vert_ins_visibility(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_title_vert_right_visibility(dwg_ent_table *table,
                                              unsigned int visibility,
                                              int *error);

unsigned int
dwg_ent_table_get_title_vert_right_visibility(dwg_ent_table *table,
                                              int *error);

void
dwg_ent_table_set_header_horiz_top_visibility(dwg_ent_table *table,
                                              unsigned int visibility,
                                              int *error);

unsigned int
dwg_ent_table_get_header_horiz_top_visibility(dwg_ent_table *table,
                                              int *error);

void
dwg_ent_table_set_header_horiz_ins_visibility(dwg_ent_table *table,
                                              unsigned int visibility,
                                              int *error);

unsigned int
dwg_ent_table_get_header_horiz_ins_visibility(dwg_ent_table *table,
                                              int *error);

void
dwg_ent_table_set_header_horiz_bottom_visibility(dwg_ent_table *table,
                                                 unsigned int visibility,
                                                 int *error);

unsigned int
dwg_ent_table_get_header_horiz_bottom_visibility(dwg_ent_table *table,
                                                 int *error);

void
dwg_ent_table_set_header_vert_ins_visibility(dwg_ent_table *table,
                                             unsigned int visibility,
                                             int *error);

unsigned int
dwg_ent_table_get_header_vert_ins_visibility(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_header_vert_right_visibility(dwg_ent_table *table,
                                               unsigned int visibility,
                                               int *error);

unsigned int
dwg_ent_table_get_header_vert_right_visibility(dwg_ent_table *table,
                                               int *error);


void
dwg_ent_table_set_data_horiz_top_visibility(dwg_ent_table *table,
                                            unsigned int visibility,
                                            int *error);

unsigned int
dwg_ent_table_get_data_horiz_top_visibility(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_data_horiz_ins_visibility(dwg_ent_table *table,
                                            unsigned int visibility,
                                            int *error);

unsigned int
dwg_ent_table_get_data_horiz_ins_visibility(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_data_horiz_bottom_visibility(dwg_ent_table *table,
                                               unsigned int visibility,
                                               int *error);

unsigned int
dwg_ent_table_get_data_horiz_bottom_visibility(dwg_ent_table *table,
                                               int *error);

void
dwg_ent_table_set_data_vert_ins_visibility(dwg_ent_table *table,
                                           unsigned int visibility,
                                           int *error);

unsigned int
dwg_ent_table_get_data_vert_ins_visibility(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_data_vert_right_visibility(dwg_ent_table *table,
                                             unsigned int visibility,
                                             int *error);

unsigned int
dwg_ent_table_get_data_vert_right_visibility(dwg_ent_table *table, int *error);


/********************************************************************
*                    FUNCTIONS FOR LAYER OBJECT                     *
********************************************************************/


// Get Layer Name
char *
dwg_obj_layer_get_name(dwg_obj_layer *layer, int *error);


/********************************************************************
*              FUNCTIONS FOR VERTEX_PFACE_FACE ENTITY               *
********************************************************************/


// Get/Set vertind of a vertex_pface_face entity
unsigned int
dwg_ent_vertex_pface_face_set_vertind(dwg_ent_vert_pface_face *face);

void
dwg_ent_vertex_pface_face_get_vertind(dwg_ent_vert_pface_face *face,
                                      unsigned int vertind[4]);


/********************************************************************
*                FUNCTIONS FOR BLOCK_HEADER OBJECT                  *
********************************************************************/


/* Get Block Name of the block header type argument passed in function
Usage :- char *block_name = dwg_obj_block_header_get_name(hdr);
*/
char *
dwg_obj_block_header_get_name(dwg_obj_block_header *hdr, int *error);

dwg_obj_block_header *
dwg_get_block_header(dwg_data *dwg, int *error);


/********************************************************************
*               FUNCTIONS FOR BLOCK_CONTROL OBJECT                  *
********************************************************************/


long
dwg_obj_block_control_get_num_entries(dwg_obj_block_control *ctrl, int *error);

dwg_object_ref **
dwg_obj_block_control_get_block_headers(dwg_obj_block_control *ctrl,
                                        int *error);

dwg_obj_block_control *
dwg_block_header_get_block_control(dwg_obj_block_header* block_header,
                                   int *error);

dwg_object_ref *
dwg_obj_block_control_get_model_space(dwg_obj_block_control *ctrl, int *error);

dwg_object_ref *
dwg_obj_block_control_get_paper_space(dwg_obj_block_control *ctrl, int *error);


/********************************************************************
*                    FUNCTIONS FOR DWG OBJECT                       *
********************************************************************/


int 
dwg_obj_object_get_index(dwg_object *obj, int *error);

dwg_handle
dwg_obj_get_handle(dwg_object *obj, int *error);

unsigned long
dwg_obj_ref_get_abs_ref(dwg_object_ref *ref, int *error);

dwg_handle
dwg_ent_insert_get_ref_handle(dwg_ent_insert *insert, int *error);

dwg_object *
dwg_obj_reference_get_object(dwg_object_ref *ref, int *error);

unsigned long
dwg_ent_insert_get_abs_ref(dwg_ent_insert *insert, int *error);

dwg_obj_obj *
dwg_object_to_object(dwg_object *obj, int *error);

dwg_obj_ent *
dwg_object_to_entity(dwg_object *obj, int *error);

int 
dwg_get_type(dwg_object *obj);

#endif
