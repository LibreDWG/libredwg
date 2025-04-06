#include "binding.h"

emscripten::val handle_to_js_object(Dwg_Handle* handle) {
  emscripten::val handle_obj = emscripten::val::object();
  handle_obj.set("code", handle->code);
  handle_obj.set("size", handle->size);
  handle_obj.set("value", handle->value);
  handle_obj.set("is_global", handle->is_global);
  return handle_obj;
}

emscripten::val object_ref_to_js_object(BITCODE_H object_ref) {
  emscripten::val handle_obj = emscripten::val::object();
  handle_obj.set("obj", reinterpret_cast<uintptr_t>(object_ref->obj));
  handle_obj.set("handleref", handle_to_js_object(&object_ref->handleref));
  handle_obj.set("absolute_ref", object_ref->absolute_ref);
  handle_obj.set("r11_idx", object_ref->r11_idx);
  return handle_obj;
}

emscripten::val color_to_js_object(const Dwg_Color* color) {
  emscripten::val color_obj = emscripten::val::object();
  color_obj.set("index", color->index);
  color_obj.set("flag", color->flag);
  // TODO: combine 'rgb' and 'alpha' together and convert them to 'rgba'
  color_obj.set("rgb", color->rgb);
  color_obj.set("name", std::string(color->name));
  color_obj.set("book_name", std::string(color->book_name));
  return color_obj;
}

/***********************************************************************/

/**
 * Return one JavaScript object for Dwg_Handle instance
 */
emscripten::val dwg_handle_wrapper(Dwg_Handle_Ptr handle_ptr) {
  Dwg_Handle* ptr = reinterpret_cast<Dwg_Handle*>(handle_ptr);
  return handle_to_js_object(ptr);
}

/**
 * Return one JavaScript object for Dwg_Object_Ref instance
 */
emscripten::val dwg_object_ref_wrapper(Dwg_Object_Ref_Ptr ref_ptr) {
  Dwg_Object_Ref* ref = reinterpret_cast<Dwg_Object_Ref*>(ref_ptr);
  return object_ref_to_js_object(ref);
}

EMSCRIPTEN_BINDINGS(libredwg_type) {
  DEFINE_FUNC(dwg_handle);
  DEFINE_FUNC(dwg_object_ref);
}

/***********************************************************************/

#define DEFINE_ARRAY_FUNC(type)                                                        \
emscripten::val dwg_ptr_to_##type##_array_wrapper(uintptr_t array_ptr, size_t size) {  \
  type* array = reinterpret_cast<type*>(array_ptr);                                    \
  emscripten::val jsArray = emscripten::val::array();                                  \
  for (int index = 0; index < size; ++index) {                                         \
    jsArray.call<void>("push", array[index]);                                          \
  }                                                                                    \
  return jsArray;                                                                      \
}

DEFINE_ARRAY_FUNC(double)
DEFINE_ARRAY_FUNC(uint16_t)
DEFINE_ARRAY_FUNC(int16_t)
DEFINE_ARRAY_FUNC(uint32_t)
DEFINE_ARRAY_FUNC(int32_t)
DEFINE_ARRAY_FUNC(uint64_t)
DEFINE_ARRAY_FUNC(int64_t)


emscripten::val dwg_ptr_to_unsigned_char_array_wrapper(uintptr_t array_ptr, size_t size) {
  unsigned char* array = reinterpret_cast<unsigned char*>(array_ptr);
  emscripten::val jsArray = emscripten::val::array();
  for (int index = 0; index < size; ++index) {
    jsArray.call<void>("push", array[index]);
  }
  return jsArray;
}

emscripten::val dwg_ptr_to_signed_char_array_wrapper(uintptr_t array_ptr, size_t size) {
  signed char* array = reinterpret_cast<signed char*>(array_ptr);
  emscripten::val jsArray = emscripten::val::array();
  for (int index = 0; index < size; ++index) {
    jsArray.call<void>("push", array[index]);
  }
  return jsArray;
}

emscripten::val dwg_ptr_to_point2d_array_wrapper(uintptr_t array_ptr, size_t size) {
  dwg_point_2d* array = reinterpret_cast<dwg_point_2d*>(array_ptr);

  emscripten::val points_obj = emscripten::val::array();
  for (int index = 0; index < size; ++index) {
    emscripten::val point_obj = emscripten::val::object();
    auto point = array[index];
    point_obj.set("x", point.x);
    point_obj.set("y", point.y);
    points_obj.call<void>("push", point_obj);
  }
  return points_obj;
}

emscripten::val dwg_ptr_to_point3d_array_wrapper(uintptr_t array_ptr, size_t size) {
  dwg_point_3d* array = reinterpret_cast<dwg_point_3d*>(array_ptr);

  emscripten::val points_obj = emscripten::val::array();
  for (int index = 0; index < size; ++index) {
    emscripten::val point_obj = emscripten::val::object();
    auto point = array[index];
    point_obj.set("x", point.x);
    point_obj.set("y", point.y);
    point_obj.set("z", point.z);
    points_obj.call<void>("push", point_obj);
  }
  return points_obj;
}

emscripten::val dwg_ptr_to_ltype_dash_array_wrapper(uintptr_t array_ptr, size_t size) {
  Dwg_LTYPE_dash* array = reinterpret_cast<Dwg_LTYPE_dash*>(array_ptr);

  emscripten::val dashes = emscripten::val::array();
  for (int index = 0; index < size; ++index) {
    emscripten::val dash_obj = emscripten::val::object();
    auto dash = array[index];
    dash_obj.set("length", dash.length);
    dash_obj.set("complex_shapecode", dash.complex_shapecode);
    dash_obj.set("style", reinterpret_cast<uintptr_t>(dash.style));
    dash_obj.set("x_offset", dash.x_offset);
    dash_obj.set("y_offset", dash.y_offset);
    dash_obj.set("scale", dash.scale);
    dash_obj.set("rotation", dash.rotation);
    dash_obj.set("shape_flag", dash.shape_flag);
    dash_obj.set("text", std::string(dash.text));
    dash_obj.call<void>("push", dash);
  }
  return dashes;
}

EMSCRIPTEN_BINDINGS(libredwg_array) {
  DEFINE_FUNC(dwg_ptr_to_unsigned_char_array);
  DEFINE_FUNC(dwg_ptr_to_signed_char_array);
  DEFINE_FUNC(dwg_ptr_to_uint16_t_array);
  DEFINE_FUNC(dwg_ptr_to_int16_t_array);
  DEFINE_FUNC(dwg_ptr_to_uint32_t_array);
  DEFINE_FUNC(dwg_ptr_to_int32_t_array);
  DEFINE_FUNC(dwg_ptr_to_uint64_t_array);
  DEFINE_FUNC(dwg_ptr_to_int64_t_array);
  DEFINE_FUNC(dwg_ptr_to_double_array);
  DEFINE_FUNC(dwg_ptr_to_point2d_array);
  DEFINE_FUNC(dwg_ptr_to_point3d_array);
  DEFINE_FUNC(dwg_ptr_to_ltype_dash_array);
}

/***********************************************************************/

Dwg_Object_Object_Ptr dwg_object_get_tio_wrapper(Dwg_Object_Ptr obj_ptr) {
  Dwg_Object* obj = reinterpret_cast<Dwg_Object*>(obj_ptr);
  // The address of 'tio.entity' is same as the address of 'tio.object'.
  return (obj == NULL) ? 0 : reinterpret_cast<uintptr_t>(obj->tio.object);
}

int dwg_object_get_type_wrapper(Dwg_Object_Ptr obj_ptr) {
  Dwg_Object* obj = reinterpret_cast<Dwg_Object*>(obj_ptr);
  return (obj == NULL) ? 0 : obj->type;
}

int dwg_object_get_fixedtype_wrapper(Dwg_Object_Ptr obj_ptr) {
  Dwg_Object* obj = reinterpret_cast<Dwg_Object*>(obj_ptr);
  return (obj == NULL) ? 0 : static_cast<int>(obj->fixedtype);
}

int dwg_object_get_supertype_wrapper(Dwg_Object_Ptr obj_ptr) {
  Dwg_Object* obj = reinterpret_cast<Dwg_Object*>(obj_ptr);
  return (obj == NULL) ? 0 : static_cast<int>(obj->supertype);
}

/**
 * public entity/object name
 */
std::string dwg_object_get_name_wrapper(Dwg_Object_Ptr obj_ptr) {
  Dwg_Object* obj = reinterpret_cast<Dwg_Object*>(obj_ptr);
  return (obj == NULL) ? 0 : std::string(obj->name);
}

/**
 * The internal dxf classname, often with a ACDB prefix
 */
std::string dwg_object_get_dxfname_wrapper(Dwg_Object_Ptr obj_ptr) {
  Dwg_Object* obj = reinterpret_cast<Dwg_Object*>(obj_ptr);
  return (obj == NULL) ? 0 : std::string(obj->dxfname);
}

int dwg_object_get_handle_wrapper(Dwg_Object_Ptr obj_ptr) {
  Dwg_Object* obj = reinterpret_cast<Dwg_Object*>(obj_ptr);
  return (obj == NULL) ? 0 : reinterpret_cast<uintptr_t>(&(obj->handle));
}

emscripten::val dwg_object_get_handle_object_wrapper(Dwg_Object_Ptr obj_ptr) {
  Dwg_Object* obj = reinterpret_cast<Dwg_Object*>(obj_ptr);
  return handle_to_js_object(&(obj->handle));
}

/**
 * Methods to access fields of Dwg_Object
 */
EMSCRIPTEN_BINDINGS(libredwg_dwg_object) {
  DEFINE_FUNC(dwg_object_get_tio);
  DEFINE_FUNC(dwg_object_get_type);
  DEFINE_FUNC(dwg_object_get_supertype);
  DEFINE_FUNC(dwg_object_get_fixedtype);
  DEFINE_FUNC(dwg_object_get_name);
  DEFINE_FUNC(dwg_object_get_dxfname);
  DEFINE_FUNC(dwg_object_get_handle);
  DEFINE_FUNC(dwg_object_get_handle_object);
}


/***********************************************************************/

Dwg_Object_Object_Ptr dwg_object_object_get_tio_wrapper(Dwg_Object_Object_Ptr obj_obj_ptr) {
  Dwg_Object_Object* obj_obj = reinterpret_cast<Dwg_Object_Object*>(obj_obj_ptr);
  return (obj_obj == NULL) ? 0 : reinterpret_cast<uintptr_t>(obj_obj->tio.APPID);
}

int dwg_object_object_get_objid_wrapper(Dwg_Object_Object_Ptr obj_obj_ptr) {
  Dwg_Object_Object* obj_obj = reinterpret_cast<Dwg_Object_Object*>(obj_obj_ptr);
  return (obj_obj == NULL) ? 0 : obj_obj->objid;
}

uintptr_t dwg_object_object_get_ownerhandle_wrapper(Dwg_Object_Object_Ptr obj_obj_ptr) {
  Dwg_Object_Object* obj_obj = reinterpret_cast<Dwg_Object_Object*>(obj_obj_ptr);
  return (obj_obj == NULL) ? 0 : reinterpret_cast<uintptr_t>(obj_obj->ownerhandle);
}

emscripten::val dwg_object_object_get_ownerhandle_object_wrapper(Dwg_Object_Object_Ptr obj_obj_ptr) {
  Dwg_Object_Object* obj_obj = reinterpret_cast<Dwg_Object_Object*>(obj_obj_ptr);
  return object_ref_to_js_object(obj_obj->ownerhandle);
}

uintptr_t dwg_object_object_get_handle_wrapper(Dwg_Object_Object_Ptr obj_obj_ptr) {
  Dwg_Object_Object* obj_obj = reinterpret_cast<Dwg_Object_Object*>(obj_obj_ptr);
  return (obj_obj == NULL) ? 0 : reinterpret_cast<uintptr_t>(obj_obj->handleref);
}

emscripten::val dwg_object_object_get_handle_object_wrapper(Dwg_Object_Object_Ptr obj_obj_ptr) {
  Dwg_Object_Object* obj_obj = reinterpret_cast<Dwg_Object_Object*>(obj_obj_ptr);
  return handle_to_js_object(obj_obj->handleref);
}

int dwg_object_object_get_num_reactors_wrapper(Dwg_Object_Object_Ptr obj_obj_ptr) {
  Dwg_Object_Object* obj_obj = reinterpret_cast<Dwg_Object_Object*>(obj_obj_ptr);
  return (obj_obj == NULL) ? 0 : obj_obj->num_reactors;
}

uintptr_t dwg_object_object_get_reactors_wrapper(Dwg_Object_Object_Ptr obj_obj_ptr) {
  Dwg_Object_Object* obj_obj = reinterpret_cast<Dwg_Object_Object*>(obj_obj_ptr);
  return (obj_obj == NULL) ? 0 : reinterpret_cast<uintptr_t>(obj_obj->reactors);
}

/**
 * Methods to access fields of Dwg_Object_Object
 */
EMSCRIPTEN_BINDINGS(libredwg_dwg_object_object) {
  DEFINE_FUNC(dwg_object_object_get_tio);
  DEFINE_FUNC(dwg_object_object_get_objid);
  DEFINE_FUNC(dwg_object_object_get_ownerhandle);
  DEFINE_FUNC(dwg_object_object_get_ownerhandle_object);
  DEFINE_FUNC(dwg_object_object_get_handle);
  DEFINE_FUNC(dwg_object_object_get_handle_object);
  DEFINE_FUNC(dwg_object_object_get_num_reactors);
  DEFINE_FUNC(dwg_object_object_get_reactors);
}

/***********************************************************************/

uintptr_t dwg_object_entity_get_ownerhandle_wrapper(Dwg_Object_Entity_Ptr ent_ptr) {
  Dwg_Object_Entity* ent = reinterpret_cast<Dwg_Object_Entity*>(ent_ptr);
  int error = 0;
  return reinterpret_cast<uintptr_t>(dwg_ent_get_ownerhandle(ent, &error));
}

emscripten::val dwg_object_entity_get_ownerhandle_object_wrapper(Dwg_Object_Entity_Ptr ent_ptr) {
  Dwg_Object_Entity* ent = reinterpret_cast<Dwg_Object_Entity*>(ent_ptr);
  int error = 0;
  Dwg_Object_Ref* ownerhandle = dwg_ent_get_ownerhandle(ent, &error);
  return object_ref_to_js_object(ownerhandle);
}

uintptr_t dwg_object_entity_get_handle_wrapper(Dwg_Object_Entity_Ptr ent_ptr) {
  Dwg_Object_Entity* ent = reinterpret_cast<Dwg_Object_Entity*>(ent_ptr);
  int error = 0;
  Dwg_Object* obj = dwg_ent_to_object(ent, &error);
  return reinterpret_cast<uintptr_t>(dwg_object_get_handle(obj, &error));
}

emscripten::val dwg_object_entity_get_handle_object_wrapper(Dwg_Object_Entity_Ptr ent_ptr) {
  Dwg_Object_Entity* ent = reinterpret_cast<Dwg_Object_Entity*>(ent_ptr);
  int error = 0;
  Dwg_Object* obj = dwg_ent_to_object(ent, &error);
  Dwg_Handle* handle = dwg_object_get_handle(obj, &error);
  return handle_to_js_object(handle);
}

emscripten::val dwg_object_entity_get_color_object_wrapper(Dwg_Object_Entity_Ptr ent_ptr) {
  Dwg_Object_Entity* ent = reinterpret_cast<Dwg_Object_Entity*>(ent_ptr);
  int error = 0;
  const Dwg_Color* color = dwg_ent_get_color(ent, &error);
  return color_to_js_object(color);
}

/**
 * Get the name of the layer referenced by this entity
 */
std::string dwg_object_entity_get_layer_name_wrapper(Dwg_Object_Entity_Ptr ent_ptr) {
  Dwg_Object_Entity* ent = reinterpret_cast<Dwg_Object_Entity*>(ent_ptr);
  int error = 0;
  return dwg_ent_get_layer_name(ent, &error);
}

/**
 * Get the name of the line type referenced by this entity
 */
std::string dwg_object_entity_get_ltype_name_wrapper(Dwg_Object_Entity_Ptr ent_ptr) {
  Dwg_Object_Entity* ent = reinterpret_cast<Dwg_Object_Entity*>(ent_ptr);
  int error = 0;
  return dwg_ent_get_ltype_name(ent, &error);
}

double dwg_object_entity_get_ltype_flags_wrapper(Dwg_Object_Entity_Ptr ent_ptr) {
  Dwg_Object_Entity* ent = reinterpret_cast<Dwg_Object_Entity*>(ent_ptr);
  int error = 0;
  return dwg_ent_get_linetype_flags(ent, &error);
}

/**
 * Get The line type scale factor of this entity.
 */
double dwg_object_entity_get_ltype_scale_wrapper(Dwg_Object_Entity_Ptr ent_ptr) {
  Dwg_Object_Entity* ent = reinterpret_cast<Dwg_Object_Entity*>(ent_ptr);
  int error = 0;
  return dwg_ent_get_linetype_scale(ent, &error);
}

/**
 * Get the line weight used by this entity.
 */
BITCODE_RC dwg_object_entity_get_line_weight_wrapper(Dwg_Object_Entity_Ptr ent_ptr) {
  Dwg_Object_Entity* ent = reinterpret_cast<Dwg_Object_Entity*>(ent_ptr);
  int error = 0;
  return dwg_ent_get_linewt(ent, &error);
}

/**
 * Get the entity mode of this entity
 * - 0: has no ownerhandle
 * - 1: paper space
 * - 2: model space
 * - 3: has ownerhandle
 */
BITCODE_BB dwg_object_entity_get_entmode_wrapper(Dwg_Object_Entity_Ptr ent_ptr) {
  Dwg_Object_Entity* ent = reinterpret_cast<Dwg_Object_Entity*>(ent_ptr);
  int error = 0;
  return dwg_ent_get_entmode(ent, &error);
}

/**
 * Get the invisibility state of this entity.
 */
BITCODE_BS dwg_object_entity_get_invisible_wrapper(Dwg_Object_Entity_Ptr ent_ptr) {
  Dwg_Object_Entity* ent = reinterpret_cast<Dwg_Object_Entity*>(ent_ptr);
  int error = 0;
  return dwg_ent_get_invisible(ent, &error);
}

BITCODE_BB dwg_object_entity_get_plotstyle_flags_wrapper(Dwg_Object_Entity_Ptr ent_ptr) {
  Dwg_Object_Entity* ent = reinterpret_cast<Dwg_Object_Entity*>(ent_ptr);
  int error = 0;
  return dwg_ent_get_plotstyle_flags(ent, &error);
}

BITCODE_BB dwg_object_entity_get_material_flags_wrapper(Dwg_Object_Entity_Ptr ent_ptr) {
  Dwg_Object_Entity* ent = reinterpret_cast<Dwg_Object_Entity*>(ent_ptr);
  int error = 0;
  return dwg_ent_get_material_flags(ent, &error);
}

BITCODE_BB dwg_object_entity_get_shadow_flags_wrapper(Dwg_Object_Entity_Ptr ent_ptr) {
  Dwg_Object_Entity* ent = reinterpret_cast<Dwg_Object_Entity*>(ent_ptr);
  int error = 0;
  return dwg_ent_get_shadow_flags(ent, &error);
}

BITCODE_BB dwg_object_entity_has_full_visualstyle_wrapper(Dwg_Object_Entity_Ptr ent_ptr) {
  Dwg_Object_Entity* ent = reinterpret_cast<Dwg_Object_Entity*>(ent_ptr);
  int error = 0;
  return dwg_ent_has_full_visualstyle(ent, &error);
}

BITCODE_BB dwg_object_entity_has_face_visualstyle_wrapper(Dwg_Object_Entity_Ptr ent_ptr) {
  Dwg_Object_Entity* ent = reinterpret_cast<Dwg_Object_Entity*>(ent_ptr);
  int error = 0;
  return dwg_ent_has_face_visualstyle(ent, &error);
}

BITCODE_BB dwg_object_entity_has_edge_visualstyle_wrapper(Dwg_Object_Entity_Ptr ent_ptr) {
  Dwg_Object_Entity* ent = reinterpret_cast<Dwg_Object_Entity*>(ent_ptr);
  int error = 0;
  return dwg_ent_has_edge_visualstyle(ent, &error);
}

BITCODE_BL dwg_object_entity_get_num_reactors_wrapper(Dwg_Object_Entity_Ptr ent_ptr) {
  Dwg_Object_Entity* ent = reinterpret_cast<Dwg_Object_Entity*>(ent_ptr);
  int error = 0;
  return dwg_ent_get_num_reactors(ent, &error);
}

uintptr_t dwg_object_entity_get_reactors_wrapper(Dwg_Object_Entity_Ptr ent_ptr) {
  Dwg_Object_Entity* ent = reinterpret_cast<Dwg_Object_Entity*>(ent_ptr);
  int error = 0;
  return reinterpret_cast<uintptr_t>(dwg_ent_get_reactors(ent, &error));
}

/**
 * Methods to access fields of Dwg_Object_Entity
 */
EMSCRIPTEN_BINDINGS(libredwg_dwg_object_entity) {
  DEFINE_FUNC(dwg_object_entity_get_ownerhandle);
  DEFINE_FUNC(dwg_object_entity_get_ownerhandle_object);
  DEFINE_FUNC(dwg_object_entity_get_handle);
  DEFINE_FUNC(dwg_object_entity_get_handle_object);
  DEFINE_FUNC(dwg_object_entity_get_color_object);
  DEFINE_FUNC(dwg_object_entity_get_layer_name);
  DEFINE_FUNC(dwg_object_entity_get_ltype_name);
  DEFINE_FUNC(dwg_object_entity_get_ltype_flags);
  DEFINE_FUNC(dwg_object_entity_get_ltype_scale);
  DEFINE_FUNC(dwg_object_entity_get_line_weight);
  DEFINE_FUNC(dwg_object_entity_get_entmode);
  DEFINE_FUNC(dwg_object_entity_get_invisible);
  DEFINE_FUNC(dwg_object_entity_get_plotstyle_flags);
  DEFINE_FUNC(dwg_object_entity_get_material_flags);
  DEFINE_FUNC(dwg_object_entity_get_shadow_flags);
  DEFINE_FUNC(dwg_object_entity_has_full_visualstyle);
  DEFINE_FUNC(dwg_object_entity_has_face_visualstyle);
  DEFINE_FUNC(dwg_object_entity_has_edge_visualstyle);
  DEFINE_FUNC(dwg_object_entity_get_num_reactors);
  DEFINE_FUNC(dwg_object_entity_get_reactors);
}

/***********************************************************************/

/**
 * Returns the absolute handle reference (field 'absolute_ref') of Dwg_Object_Ref*
 */
BITCODE_BL dwg_ref_get_absref_wrapper(Dwg_Object_Ref_Ptr ref_ptr) {
  Dwg_Object_Ref* ref = reinterpret_cast<Dwg_Object_Ref*>(ref_ptr);
  return (ref == 0) ? 0 : ref->absolute_ref;
}

/**
 * Returns Dwg_Object* from Dwg_Object_Ref*
 */
Dwg_Object_Ptr dwg_ref_get_object_wrapper(Dwg_Object_Ref_Ptr ref_ptr) {
  Dwg_Object_Ref* ref = reinterpret_cast<Dwg_Object_Ref*>(ref_ptr);
  int error = 0;
  return reinterpret_cast<Dwg_Object_Ptr>(dwg_ref_get_object(ref, &error));
}

int dwg_ref_get_handle_wrapper(Dwg_Object_Ref_Ptr ref_ptr) {
  Dwg_Object_Ref* ref = reinterpret_cast<Dwg_Object_Ref*>(ref_ptr);
  return (ref == NULL) ? 0 : reinterpret_cast<uintptr_t>(&(ref->handleref));
}

emscripten::val dwg_ref_get_handle_object_wrapper(Dwg_Object_Ref_Ptr ref_ptr) {
  Dwg_Object_Ref* ref = reinterpret_cast<Dwg_Object_Ref*>(ref_ptr);
  return handle_to_js_object(&(ref->handleref));
}

EMSCRIPTEN_BINDINGS(libredwg_dwg_object_ref) {
  DEFINE_FUNC(dwg_ref_get_absref);
  DEFINE_FUNC(dwg_ref_get_object);
  DEFINE_FUNC(dwg_ref_get_handle);
  DEFINE_FUNC(dwg_ref_get_handle_object);
}