#include <emscripten/bind.h>
#include "dwg.h"
#include "dwg_api.h"

using namespace emscripten;

#define DEFINE_FUNC(funcName)                                                 \
  function(#funcName, &funcName##_wrapper)

#define DEFINE_FUNC_WITH_REF_POLICY(funcName)                                 \
  function(#funcName, &funcName##_wrapper, return_value_policy::reference())


typedef uintptr_t Dwg_Handle_Ptr;
typedef uintptr_t Dwg_Object_Ptr;
typedef uintptr_t Dwg_Object_Ref_Ptr;
typedef uintptr_t Dwg_Object_Object_Ptr;
typedef uintptr_t Dwg_Object_Entity_Ptr;

emscripten::val handle_to_js_object(Dwg_Handle* handle);

emscripten::val object_ref_to_js_object(BITCODE_H object_ref);

emscripten::val color_to_js_object(const Dwg_Color* color);

emscripten::val point2d_to_js_object(const dwg_point_2d* point);

emscripten::val bitcode_2bd_to_js_object(const BITCODE_2BD* point);

emscripten::val bitcode_2rd_to_js_object(const BITCODE_2RD* point);

emscripten::val point3d_to_js_object(const dwg_point_3d* point);

emscripten::val bitcode_3bd_to_js_object(const BITCODE_3BD* point);

emscripten::val bitcode_3rd_to_js_object(const BITCODE_3RD* point);

emscripten::val point4d_to_js_object(const Dwg_SPLINE_control_point* point);

emscripten::val dwg_ptr_to_unsigned_char_array(unsigned char* array, size_t size);

emscripten::val dwg_ptr_to_signed_char_array(signed char* array, size_t size);