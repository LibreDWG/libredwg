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

emscripten::val color_to_js_object(Dwg_Color* color);