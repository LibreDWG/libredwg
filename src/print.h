#include "common.h"
#include "bits.h"
#include "dwg.h"

void dwg_print (Dwg_Structure * dwg);
void dwg_print_entity (Dwg_Object_Entity * ent);
void dwg_print_object (Dwg_Object_Object *obj);
void dwg_print_traktref (Dwg_Object * obj);
void dwg_print_TEXT (Dwg_Entity_TEXT * ent);
void dwg_print_ATTRIB (Dwg_Entity_ATTRIB * ent);
void dwg_print_ATTDEF (Dwg_Entity_ATTDEF * ent);
void dwg_print_BLOCK (Dwg_Entity_BLOCK * ent);
void dwg_print_INSERT (Dwg_Entity_INSERT * ent);
void dwg_print_MINSERT (Dwg_Entity_MINSERT * ent);
void dwg_print_VERTEX_2D (Dwg_Entity_VERTEX_2D * ent);
void dwg_print_VERTEX_3D (Dwg_Entity_VERTEX_3D * ent);
void dwg_print_VERTEX_PFACE_FACE (Dwg_Entity_VERTEX_PFACE_FACE * ent);
void dwg_print_POLYLINE_2D (Dwg_Entity_POLYLINE_2D * ent);
void dwg_print_POLYLINE_3D (Dwg_Entity_POLYLINE_3D * ent);
void dwg_print_ARC (Dwg_Entity_ARC * ent);
void dwg_print_CIRCLE (Dwg_Entity_CIRCLE * ent);
void dwg_print_LINE (Dwg_Entity_LINE * ent);
void dwg_print_POINT (Dwg_Entity_POINT *ent);
void dwg_print_ELLIPSE (Dwg_Entity_ELLIPSE *ent);
void dwg_print_RAY (Dwg_Entity_RAY *ent);
void dwg_print_MTEXT (Dwg_Entity_MTEXT *ent);
void dwg_print_LAYER (Dwg_Object_LAYER *obj);
void dwg_print_LAYOUT (Dwg_Object_LAYOUT *obj);

