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

emscripten::val color_to_js_object(Dwg_Color* color) {
  emscripten::val color_obj = emscripten::val::object();
  color_obj.set("index", color->index);
  color_obj.set("flag", color->flag);
  // TODO: combine 'rgb' and 'alpha' together and convert them to 'rgba'
  color_obj.set("rgb", color->rgb);
  color_obj.set("name", std::string(color->name));
  color_obj.set("book_name", std::string(color->book_name));
  return color_obj;
}

/**
 * Return one JavaScript object for Dwg_Handle instance
 */
emscripten::val dwg_handle_wrapper(uintptr_t handle_ptr) {
  Dwg_Handle* ptr = reinterpret_cast<Dwg_Handle*>(handle_ptr);
  return handle_to_js_object(ptr);
}

/**
 * Return one JavaScript object for Dwg_Object_Ref instance
 */
emscripten::val dwg_object_ref_wrapper(uintptr_t ref_ptr) {
  Dwg_Object_Ref* ref = reinterpret_cast<Dwg_Object_Ref*>(ref_ptr);
  return object_ref_to_js_object(ref);
}

EMSCRIPTEN_BINDINGS(libredwg_type) {
  DEFINE_FUNC(dwg_handle);
  DEFINE_FUNC(dwg_object_ref);
}

uintptr_t dwg_object_get_tio_wrapper(uintptr_t obj_ptr) {
  Dwg_Object* obj = reinterpret_cast<Dwg_Object*>(obj_ptr);
  // The address of 'tio.entity' is same as the address of 'tio.object'.
  return (obj == NULL) ? 0 : reinterpret_cast<uintptr_t>(obj->tio.object);
}

int dwg_object_get_type_wrapper(uintptr_t obj_ptr) {
  Dwg_Object* obj = reinterpret_cast<Dwg_Object*>(obj_ptr);
  return (obj == NULL) ? 0 : obj->type;
}

int dwg_object_get_fixedtype_wrapper(uintptr_t obj_ptr) {
  Dwg_Object* obj = reinterpret_cast<Dwg_Object*>(obj_ptr);
  return (obj == NULL) ? 0 : obj->fixedtype;
}

int dwg_object_get_supertype_wrapper(uintptr_t obj_ptr) {
  Dwg_Object* obj = reinterpret_cast<Dwg_Object*>(obj_ptr);
  return (obj == NULL) ? 0 : obj->supertype;
}

/**
 * public entity/object name
 */
std::string dwg_object_get_name_wrapper(uintptr_t obj_ptr) {
  Dwg_Object* obj = reinterpret_cast<Dwg_Object*>(obj_ptr);
  return (obj == NULL) ? 0 : std::string(obj->name);
}

/**
 * The internal dxf classname, often with a ACDB prefix
 */
std::string dwg_object_get_dxfname_wrapper(uintptr_t obj_ptr) {
  Dwg_Object* obj = reinterpret_cast<Dwg_Object*>(obj_ptr);
  return (obj == NULL) ? 0 : std::string(obj->dxfname);
}

int dwg_object_get_handle_wrapper(uintptr_t obj_ptr) {
  Dwg_Object* obj = reinterpret_cast<Dwg_Object*>(obj_ptr);
  return (obj == NULL) ? 0 : reinterpret_cast<uintptr_t>(&(obj->handle));
}

emscripten::val dwg_object_get_handle_object_wrapper(uintptr_t obj_ptr) {
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