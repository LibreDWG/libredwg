#include <emscripten/bind.h>
#include <string>
#include "dwg.h"
#include "dwg_api.h"

using namespace emscripten;

#define DEFINE_FUNC(funcName)                                             \
  function(#funcName, &funcName##_wrapper)

uintptr_t dwg_read_file_ex(const std::string& fileName) {
  Dwg_Data* dwg = new Dwg_Data(); 
  dwg_read_file(fileName.c_str(), dwg);
  return reinterpret_cast<uintptr_t>(dwg);
}

BITCODE_BL dwg_get_num_objects_wrapper(uintptr_t ptr) {
  Dwg_Data* dwg = reinterpret_cast<Dwg_Data*>(ptr);
  return dwg_get_num_objects(dwg);
}

BITCODE_BL dwg_get_object_num_objects_wrapper(uintptr_t ptr) {
  Dwg_Data* dwg = reinterpret_cast<Dwg_Data*>(ptr);
  return dwg_get_object_num_objects(dwg);  
}

BITCODE_BL dwg_get_num_entities_wrapper(uintptr_t ptr) {
  Dwg_Data* dwg = reinterpret_cast<Dwg_Data*>(ptr);
  return dwg_get_num_entities(dwg);
}

// Dwg_Object_Entity **dwg_get_entities_wrapper(uintptr_t ptr) {
//   Dwg_Data* dwg = reinterpret_cast<Dwg_Data*>(ptr);
//   return dwg_get_num_entities(dwg);
// }

EMSCRIPTEN_BINDINGS(libredwg_functions) {
  function("dwg_read_file", &dwg_read_file_ex);
  DEFINE_FUNC(dwg_get_num_objects);
  DEFINE_FUNC(dwg_get_object_num_objects);
  DEFINE_FUNC(dwg_get_num_entities);
  // DEFINE_FUNC(dwg_get_entities);
}