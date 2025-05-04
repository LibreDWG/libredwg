#include <emscripten/bind.h>
#include <string>
#include <sstream>

#include "dwg.h"
#include "dwg_api.h"
#include "binding_common.h"

using namespace emscripten;

emscripten::val dwg_read_file_wrapper(const std::string& filename) {
  Dwg_Data* dwg = new Dwg_Data();
  int error = dwg_read_file(filename.c_str(), dwg);

  emscripten::val result = emscripten::val::object();
  result.set("error", error);
  result.set("data", reinterpret_cast<uintptr_t>(dwg));
  return result;
}

// emscripten::val dxf_read_file_wrapper(const std::string& filename) {
//   Dwg_Data* dwg = new Dwg_Data(); 
//   int error = dxf_read_file(filename.c_str(), dwg);

//   emscripten::val result = emscripten::val::object();
//   result.set("error", error);
//   result.set("data", reinterpret_cast<uintptr_t>(dwg));
//   return result;
// }

// emscripten::val dwg_write_file_wrapper(
//   const std::string& filename,
//   uintptr_t dwg_ptr) {
//   Dwg_Data* dwg = reinterpret_cast<Dwg_Data*>(dwg_ptr);
//   int error = dwg_write_file(filename.c_str(), dwg);

//   emscripten::val result = emscripten::val::object();
//   result.set("error", error);
//   result.set("data", reinterpret_cast<uintptr_t>(dwg));
//   return result;
// }

/* 
 * Supports multiple preview picture types.
 * Currently 3 types: BMP 2, WMF 3 and PNG as type 6.
 * Returns the size of the image.
 */
emscripten::val dwg_bmp_wrapper(uintptr_t dwg_ptr) {
  Dwg_Data* dwg = reinterpret_cast<Dwg_Data*>(dwg_ptr);
  BITCODE_RL size = 0;
  BITCODE_RC typep = 0;
  unsigned char* array = dwg_bmp(dwg, &size, &typep);
  if (array) {
    emscripten::val result = emscripten::val::object();
    result.set("type", typep);
    result.set("data", dwg_ptr_to_unsigned_char_array(array, size));
    return result;
  }
  return emscripten::val::null();
}

/** 
 * Search for the name in the associated table, and return its handle. Search
 * is case-insensitive.
 * Both name and table are ascii.
 */
uintptr_t dwg_find_tablehandle_wrapper(
  uintptr_t dwg_ptr,
  const std::string& name,
  const std::string& table) {
  Dwg_Data* dwg = reinterpret_cast<Dwg_Data*>(dwg_ptr);
  return reinterpret_cast<uintptr_t>(dwg_find_tablehandle(dwg, name.c_str(), table.c_str()));
}

uintptr_t dwg_find_tablehandle_index_wrapper(
  uintptr_t dwg_ptr,
  const int index,
  const std::string& table) {
  Dwg_Data* dwg = reinterpret_cast<Dwg_Data*>(dwg_ptr);
  return reinterpret_cast<uintptr_t>(dwg_find_tablehandle_index(dwg, index, table.c_str()));
}

/**
 * Search for handle in associated table, and return its name (as UTF-8) 
 */
std::string dwg_handle_name_wrapper (
  uintptr_t dwg_ptr,
  const std::string& table,
  uintptr_t handle_ptr) {
  Dwg_Data* dwg = reinterpret_cast<Dwg_Data*>(dwg_ptr);
  BITCODE_H handle = reinterpret_cast<BITCODE_H>(handle_ptr);
  return std::string(dwg_handle_name(dwg, table.c_str(), handle));
}

/** 
 * Not checking the header_vars entry, only searching the objects
 * Returning a hardowner or hardpointer (DICTIONARY) ref (code 3 or 5)
 * to it, as stored in header_vars. table must contain the "_CONTROL" suffix.
 * table is ascii.
 */
uintptr_t dwg_find_table_control_wrapper(
  uintptr_t dwg_ptr,
  const std::string& table) {
  Dwg_Data* dwg = reinterpret_cast<Dwg_Data*>(dwg_ptr);
  return reinterpret_cast<uintptr_t>(dwg_find_table_control(dwg, table.c_str()));
}

/** 
 * Search for a dictionary ref.
 * Returning a hardpointer ref (5) to it, as stored in header_vars.
 * Name is ascii.
 */
uintptr_t dwg_find_dictionary_wrapper(
  uintptr_t dwg_ptr,
  const std::string& name) {
  Dwg_Data* dwg = reinterpret_cast<Dwg_Data*>(dwg_ptr);
  return reinterpret_cast<uintptr_t>(dwg_find_dictionary(dwg, name.c_str()));
}

/** 
 * Search for a named dictionary entry in the given dict.
 * Search is case-sensitive. name is ASCII.
 */
uintptr_t dwg_find_dicthandle_wrapper(
  uintptr_t dwg_ptr,
  uintptr_t dict_ptr,
  const std::string& name) {
  Dwg_Data* dwg = reinterpret_cast<Dwg_Data*>(dwg_ptr);
  BITCODE_H dict = reinterpret_cast<BITCODE_H>(dict_ptr);
  return reinterpret_cast<uintptr_t>(dwg_find_dicthandle(dwg, dict, name.c_str()));
}

/** 
 * Search all dictionary entries in the given dict.
 * Check for the matching name of the handle object. (Control lists).
 * Search is case-insensitive 
 */
uintptr_t dwg_find_dicthandle_objname_wrapper(
  uintptr_t dwg_ptr,
  uintptr_t dict_ptr,
  const std::string& name) {
  Dwg_Data* dwg = reinterpret_cast<Dwg_Data*>(dwg_ptr);
  BITCODE_H dict = reinterpret_cast<BITCODE_H>(dict_ptr);
  return reinterpret_cast<uintptr_t>(dwg_find_dicthandle_objname(dwg, dict, name.c_str()));
}

/** 
 * Search for a table EXTNAME 
 */
std::string dwg_find_table_extname_wrapper(
  uintptr_t dwg_ptr,
  uintptr_t obj_ptr) {
  Dwg_Data* dwg = reinterpret_cast<Dwg_Data*>(dwg_ptr);
  Dwg_Object* obj = reinterpret_cast<Dwg_Object*>(obj_ptr);
  return std::string(dwg_find_table_extname(dwg, obj));
}

/**
 * Returns the string value of the member of the AcDbVariableDictionary.
 * The name is ascii. E.g. LIGHTINGUNITS => "0"
 */
std::string dwg_variable_dict_wrapper(
  uintptr_t dwg_ptr,
  const std::string& name) {
  Dwg_Data* dwg = reinterpret_cast<Dwg_Data*>(dwg_ptr);
  return std::string(dwg_variable_dict(dwg, name.c_str()));
}

double dwg_model_x_min_wrapper(uintptr_t dwg_ptr) {
  Dwg_Data* dwg = reinterpret_cast<Dwg_Data*>(dwg_ptr);
  return dwg_model_x_min(dwg);
}

double dwg_model_x_max_wrapper(uintptr_t dwg_ptr) {
  Dwg_Data* dwg = reinterpret_cast<Dwg_Data*>(dwg_ptr);
  return dwg_model_x_max(dwg);
}

double dwg_model_y_min_wrapper(uintptr_t dwg_ptr) {
  Dwg_Data* dwg = reinterpret_cast<Dwg_Data*>(dwg_ptr);
  return dwg_model_y_min(dwg);
}

double dwg_model_y_max_wrapper(uintptr_t dwg_ptr) {
  Dwg_Data* dwg = reinterpret_cast<Dwg_Data*>(dwg_ptr);
  return dwg_model_y_max(dwg);
}

double dwg_model_z_min_wrapper(uintptr_t dwg_ptr) {
  Dwg_Data* dwg = reinterpret_cast<Dwg_Data*>(dwg_ptr);
  return dwg_model_z_min(dwg);
}

double dwg_model_z_max_wrapper(uintptr_t dwg_ptr) {
  Dwg_Data* dwg = reinterpret_cast<Dwg_Data*>(dwg_ptr);
  return dwg_model_z_max(dwg);
}

double dwg_page_x_min_wrapper(uintptr_t dwg_ptr) {
  Dwg_Data* dwg = reinterpret_cast<Dwg_Data*>(dwg_ptr);
  return dwg_page_x_min(dwg);
}

double dwg_page_x_max_wrapper(uintptr_t dwg_ptr) {
  Dwg_Data* dwg = reinterpret_cast<Dwg_Data*>(dwg_ptr);
  return dwg_page_x_max(dwg);
}

double dwg_page_y_min_wrapper(uintptr_t dwg_ptr) {
  Dwg_Data* dwg = reinterpret_cast<Dwg_Data*>(dwg_ptr);
  return dwg_page_y_min(dwg);
}

double dwg_page_y_max_wrapper(uintptr_t dwg_ptr) {
  Dwg_Data* dwg = reinterpret_cast<Dwg_Data*>(dwg_ptr);
  return dwg_page_y_max(dwg);
}

uintptr_t dwg_block_control_wrapper(uintptr_t dwg_ptr) {
  Dwg_Data* dwg = reinterpret_cast<Dwg_Data*>(dwg_ptr);
  return reinterpret_cast<uintptr_t>(dwg_block_control(dwg)); 
}

uintptr_t dwg_model_space_ref_wrapper(uintptr_t dwg_ptr) {
  Dwg_Data* dwg = reinterpret_cast<Dwg_Data*>(dwg_ptr);
  return reinterpret_cast<uintptr_t>(dwg_model_space_ref(dwg));  
}

uintptr_t dwg_paper_space_ref_wrapper(uintptr_t dwg_ptr) {
  Dwg_Data* dwg = reinterpret_cast<Dwg_Data*>(dwg_ptr);
  return reinterpret_cast<uintptr_t>(dwg_paper_space_ref(dwg));  
}

uintptr_t dwg_model_space_object_wrapper(uintptr_t dwg_ptr) {
  Dwg_Data* dwg = reinterpret_cast<Dwg_Data*>(dwg_ptr);
  return reinterpret_cast<uintptr_t>(dwg_model_space_object(dwg));  
}

uintptr_t dwg_paper_space_object_wrapper(uintptr_t dwg_ptr) {
  Dwg_Data* dwg = reinterpret_cast<Dwg_Data*>(dwg_ptr);
  return reinterpret_cast<uintptr_t>(dwg_paper_space_object(dwg));  
}

unsigned int dwg_get_layer_count_wrapper(uintptr_t dwg_ptr) {
  Dwg_Data* dwg = reinterpret_cast<Dwg_Data*>(dwg_ptr);
  return dwg_get_layer_count(dwg);  
}

uintptr_t dwg_get_layer_index_wrapper(
  uintptr_t dwg_ptr,
  size_t index) {
  Dwg_Data* dwg = reinterpret_cast<Dwg_Data*>(dwg_ptr);
  Dwg_Object_LAYER ** layers = dwg_get_layers(dwg);
  return reinterpret_cast<uintptr_t>(layers[index]);
}

BITCODE_BL dwg_get_num_objects_wrapper(uintptr_t dwg_ptr) {
  Dwg_Data* dwg = reinterpret_cast<Dwg_Data*>(dwg_ptr);
  return dwg_get_num_objects(dwg);
}

uintptr_t dwg_get_object_index_wrapper(
  uintptr_t dwg_ptr,
  size_t index) {
  Dwg_Data* dwg = reinterpret_cast<Dwg_Data*>(dwg_ptr);
  return reinterpret_cast<uintptr_t>(&dwg->object[index]);
}

BITCODE_BL dwg_get_object_num_objects_wrapper(uintptr_t dwg_ptr) {
  Dwg_Data* dwg = reinterpret_cast<Dwg_Data*>(dwg_ptr);
  return dwg_get_object_num_objects(dwg);  
}

int dwg_class_is_entity_wrapper(uintptr_t class_ptr) {
  Dwg_Class* klass= reinterpret_cast<Dwg_Class*>(class_ptr);
  return dwg_class_is_entity(klass);  
}

int dwg_obj_is_control_wrapper(uintptr_t obj_ptr) {
  Dwg_Object* obj= reinterpret_cast<Dwg_Object*>(obj_ptr);
  return dwg_obj_is_control(obj);  
}

int dwg_obj_is_table_wrapper(uintptr_t obj_ptr) {
  Dwg_Object* obj= reinterpret_cast<Dwg_Object*>(obj_ptr);
  return dwg_obj_is_table(obj);  
}

int dwg_obj_is_subentity_wrapper(uintptr_t obj_ptr) {
  Dwg_Object* obj= reinterpret_cast<Dwg_Object*>(obj_ptr);
  return dwg_obj_is_subentity(obj);  
}

int dwg_obj_has_subentity_wrapper(uintptr_t obj_ptr) {
  Dwg_Object* obj= reinterpret_cast<Dwg_Object*>(obj_ptr);
  return dwg_obj_has_subentity(obj);  
}

int dwg_obj_is_3dsolid_wrapper(uintptr_t obj_ptr) {
  Dwg_Object* obj= reinterpret_cast<Dwg_Object*>(obj_ptr);
  return dwg_obj_is_3dsolid(obj);  
}

int dwg_obj_is_acsh_wrapper(uintptr_t obj_ptr) {
  Dwg_Object* obj= reinterpret_cast<Dwg_Object*>(obj_ptr);
  return dwg_obj_is_acsh(obj);  
}

BITCODE_BL dwg_get_num_entities_wrapper(uintptr_t dwg_ptr) {
  Dwg_Data* dwg = reinterpret_cast<Dwg_Data*>(dwg_ptr);
  return dwg_get_num_entities(dwg);
}

uintptr_t dwg_get_entity_index_wrapper(uintptr_t dwg_ptr, size_t index) {
  Dwg_Data* dwg = reinterpret_cast<Dwg_Data*>(dwg_ptr);
  Dwg_Object_Entity ** entities = dwg_get_entities(dwg);
  return reinterpret_cast<uintptr_t>(entities[index]);
}

uintptr_t dwg_get_entity_layer_wrapper(uintptr_t entity_ptr) {
  Dwg_Object_Entity* entity = reinterpret_cast<Dwg_Object_Entity*>(entity_ptr);
  return reinterpret_cast<uintptr_t>(entity);
}

uintptr_t dwg_next_object_wrapper(uintptr_t obj_ptr) {
  Dwg_Object* obj= reinterpret_cast<Dwg_Object*>(obj_ptr);
  return reinterpret_cast<uintptr_t>(dwg_next_object(obj));  
}

uintptr_t dwg_next_entity_wrapper(uintptr_t obj_ptr) {
  Dwg_Object* obj= reinterpret_cast<Dwg_Object*>(obj_ptr);
  return reinterpret_cast<uintptr_t>(dwg_next_entity(obj));  
}

BITCODE_RLL dwg_next_handle_wrapper(uintptr_t dwg_ptr) {
  Dwg_Data* dwg = reinterpret_cast<Dwg_Data*>(dwg_ptr);
  return dwg_next_handle(dwg);
}

BITCODE_RLL dwg_next_handseed_wrapper(uintptr_t dwg_ptr) {
  Dwg_Data* dwg = reinterpret_cast<Dwg_Data*>(dwg_ptr);
  return dwg_next_handseed(dwg);
}

uintptr_t get_first_owned_entity_wrapper(uintptr_t obj_ptr) {
  Dwg_Object* obj= reinterpret_cast<Dwg_Object*>(obj_ptr);
  return reinterpret_cast<uintptr_t>(get_first_owned_entity(obj));  
}

uintptr_t get_next_owned_entity_wrapper(
  uintptr_t obj_ptr,
  uintptr_t current_ptr) {
  Dwg_Object* obj= reinterpret_cast<Dwg_Object*>(obj_ptr);
  Dwg_Object* current= reinterpret_cast<Dwg_Object*>(current_ptr);
  return reinterpret_cast<uintptr_t>(get_next_owned_entity(obj, current));
}

uintptr_t get_first_owned_subentity_wrapper(uintptr_t owner_ptr) {
  Dwg_Object* owner = reinterpret_cast<Dwg_Object*>(owner_ptr);
  return reinterpret_cast<uintptr_t>(get_first_owned_subentity(owner));
}

uintptr_t get_next_owned_subentity_wrapper(
  uintptr_t owner_ptr,
  uintptr_t current_ptr) {
  Dwg_Object* owner = reinterpret_cast<Dwg_Object*>(owner_ptr);
  Dwg_Object* current = reinterpret_cast<Dwg_Object*>(current_ptr);
  return reinterpret_cast<uintptr_t>(get_next_owned_subentity(owner, current));
}

/** 
 * Returns the BLOCK entity owned by the block hdr.
 * Only NULL on illegal hdr argument or dwg version.
 */
uintptr_t get_first_owned_block_wrapper(uintptr_t hdr_ptr) {
  Dwg_Object* hdr = reinterpret_cast<Dwg_Object*>(hdr_ptr);
  return reinterpret_cast<uintptr_t>(get_first_owned_block(hdr));
}

/** 
 * Returns the last ENDBLK entity owned by the block hdr.
 * Only NULL on illegal hdr argument or dwg version.
 */
uintptr_t get_last_owned_block_wrapper(uintptr_t hdr_ptr) {
  Dwg_Object* hdr = reinterpret_cast<Dwg_Object*>(hdr_ptr);
  return reinterpret_cast<uintptr_t>(get_last_owned_block(hdr));
}

/** 
 * Returns the next block object after current owned by the block hdr, or NULL.
 */
uintptr_t get_next_owned_block_wrapper(
  uintptr_t hdr_ptr,
  uintptr_t current_ptr) {
  Dwg_Object* hdr = reinterpret_cast<Dwg_Object*>(hdr_ptr);
  Dwg_Object* current = reinterpret_cast<Dwg_Object*>(current_ptr);
  return reinterpret_cast<uintptr_t>(get_next_owned_block(hdr, current));
}

/** 
 * Returns the next block object until last_entity
 * after current owned by the block hdr, or NULL.
 */
uintptr_t get_next_owned_block_entity_wrapper(
  uintptr_t hdr_ptr,
  uintptr_t current_ptr) {
  Dwg_Object* hdr = reinterpret_cast<Dwg_Object*>(hdr_ptr);
  Dwg_Object* current = reinterpret_cast<Dwg_Object*>(current_ptr);
  return reinterpret_cast<uintptr_t>(get_next_owned_block_entity(hdr, current));
}

uintptr_t dwg_get_first_object_wrapper(
  uintptr_t dwg_ptr,
  const Dwg_Object_Type type) {
  Dwg_Data* dwg = reinterpret_cast<Dwg_Data*>(dwg_ptr);
  return reinterpret_cast<uintptr_t>(dwg_get_first_object(dwg, type));
}

uintptr_t dwg_get_next_object_wrapper(
  uintptr_t dwg_ptr,
  const Dwg_Object_Type type,
  const BITCODE_RL index) {
  Dwg_Data* dwg = reinterpret_cast<Dwg_Data*>(dwg_ptr);
  return reinterpret_cast<uintptr_t>(dwg_get_next_object(dwg, type, index));
}


uintptr_t dwg_resolve_jump_wrapper(uintptr_t obj_ptr) {
  Dwg_Object* obj = reinterpret_cast<Dwg_Object*>(obj_ptr);
  return reinterpret_cast<uintptr_t>(dwg_resolve_jump(obj));
}

Dwg_Section_Type dwg_section_type_wrapper(const std::string& name) {
  return dwg_section_type(name.c_str());
}

// Dwg_Section_Type dwg_section_wtype_wrapper(const std::wstring& wname) {
//   return dwg_section_wtype(wname.c_str());
// }

std::string dwg_section_name_wrapper(
  uintptr_t dwg_ptr,
  const unsigned int sec_id) {
  Dwg_Data* dwg = reinterpret_cast<Dwg_Data*>(dwg_ptr);
  return std::string(dwg_section_name(dwg, sec_id));
}

/** 
 * Free the whole DWG. all tables, sections, objects, ...
 */
void dwg_free_wrapper(uintptr_t dwg_ptr) {
  Dwg_Data* dwg = reinterpret_cast<Dwg_Data*>(dwg_ptr);
  dwg_free(dwg);
}

/** 
 * Free the object (all three structs and its fields)
 */
void dwg_free_object_wrapper(uintptr_t obj_ptr) {
  Dwg_Object* obj = reinterpret_cast<Dwg_Object*>(obj_ptr);
  dwg_free_object(obj);  
}

/**
 * Add the empty ref to the DWG (freshly malloc'ed), or NULL.
 */
uintptr_t dwg_new_ref_wrapper(uintptr_t dwg_ptr) {
  Dwg_Data* dwg = reinterpret_cast<Dwg_Data*>(dwg_ptr);
  return reinterpret_cast<uintptr_t>(dwg_new_ref(dwg));
}

/** 
 * For encode:
 * May need obj to shorten the code to a relative offset, but not in header_vars.
 * There obj is NULL.
 */
int dwg_add_handle_wrapper(
  uintptr_t hdl_ptr,
  const BITCODE_RC code,
  const BITCODE_RLL value,
  uintptr_t obj_ptr) {
  Dwg_Handle* hdl = reinterpret_cast<Dwg_Handle*>(hdl_ptr);
  Dwg_Object* obj = reinterpret_cast<Dwg_Object*>(obj_ptr);
  return dwg_add_handle(hdl, code, value, obj);
}

/** 
 * Returns an existing ref with the same ownership (hard/soft, owner/pointer)
 * or creates it. With obj non-NULL it may return a relative offset, otherwise
 * always absolute.
 */
uintptr_t dwg_add_handleref_wrapper(
  uintptr_t dwg_ptr,
  const BITCODE_RC code,
  const BITCODE_RLL value,
  uintptr_t obj_ptr) {
  Dwg_Data* dwg = reinterpret_cast<Dwg_Data*>(dwg_ptr);
  Dwg_Object* obj = reinterpret_cast<Dwg_Object*>(obj_ptr);
  return reinterpret_cast<uintptr_t>(dwg_add_handleref(dwg, code, value, obj));
}

/** 
 * Return a link to the global ref or a new one. Or a NULLHDL. 
 */
uintptr_t dwg_dup_handleref_wrapper(
  uintptr_t dwg_ptr,
  uintptr_t ref_ptr) {
  Dwg_Data* dwg = reinterpret_cast<Dwg_Data*>(dwg_ptr);
  Dwg_Object_Ref* ref = reinterpret_cast<Dwg_Object_Ref*>(ref_ptr);
  return reinterpret_cast<uintptr_t>(dwg_dup_handleref(dwg, ref));
}

/** 
 * Creates a non-global, free'able handle ref. Never relative 
 */
uintptr_t dwg_add_handleref_free_wrapper(
  const BITCODE_RC code,
  const BITCODE_RLL absref) {
  return reinterpret_cast<uintptr_t>(dwg_add_handleref_free(code, absref));
}

std::string dwg_version_type_wrapper(const Dwg_Version_Type version) {
  return std::string(dwg_version_type(version));
}

Dwg_Version_Type dwg_version_as_wrapper(const std::string& version) {
  return dwg_version_as(version.c_str());
}

Dwg_Version_Type dwg_version_hdr_type_wrapper(const std::string& hdr) {
  return dwg_version_hdr_type(hdr.c_str());
}

int dwg_supports_eed_wrapper(uintptr_t dwg_ptr) {
  Dwg_Data* dwg = reinterpret_cast<Dwg_Data*>(dwg_ptr);
  return dwg_supports_eed(dwg);
}

/* to read and write */
int dwg_supports_obj_wrapper(
  uintptr_t dwg_ptr,
  uintptr_t obj_ptr) {
  Dwg_Data* dwg = reinterpret_cast<Dwg_Data*>(dwg_ptr);
  Dwg_Object* obj = reinterpret_cast<Dwg_Object*>(obj_ptr);
  return dwg_supports_obj(dwg, obj);
}

std::string dwg_encrypt_SAT1_wrapper(
  BITCODE_BL blocksize,
  uintptr_t acis_data_ptr,
  uintptr_t acis_data_offset_ptr) {
  BITCODE_RC* acis_data = reinterpret_cast<BITCODE_RC*>(acis_data_ptr);
  int* acis_data_offset = reinterpret_cast<int*>(acis_data_offset_ptr);
  return dwg_encrypt_SAT1(blocksize, acis_data, acis_data_offset);
}

/* 
 * Converts v2 SAB acis_data in-place to SAT v1 encr_sat_data[].
 * Sets _obj->_dxf_sab_converted to 1, denoting that encr_sat_data is NOT the
 * encrypted acis_data anymore, rather the converted from SAB for DXF 
 */
// int dwg_convert_SAB_to_SAT1_wrapper(uintptr_t obj_ptr) {
//   Dwg_Entity_3DSOLID* obj = reinterpret_cast<Dwg_Entity_3DSOLID*>(obj_ptr);
//   return dwg_convert_SAB_to_SAT1(obj);
// }

/** 
 * Add the empty object to the DWG.
 * Returns DWG_ERR_OUTOFMEM, -1 for realloced or 0 if not.
 * objects are allocated in bulk, and all old obj pointers may become invalid.
 * The new object is at &dwg->object[dwg->num_objects - 1].
 */
int dwg_add_object_wrapper(uintptr_t dwg_ptr) {
  Dwg_Data* dwg = reinterpret_cast<Dwg_Data*>(dwg_ptr);
  return dwg_add_object(dwg);
}

/* 
 * Find if an object name (our internal name, not anything used elsewhere)
 * is defined, and return our fixed type, the public dxfname and if it's an entity. 
 */
int dwg_object_name_wrapper(
  const std::string& name, // in
  uintptr_t dxfname_ptr,  // out, maybe NULL
  uintptr_t type_ptr, // out, maybe NULL
  uintptr_t is_ent_ptr, // out, maybe NULL
  uintptr_t stability_ptr) { // out, maybe NULL
  const char ** dxfnamep = reinterpret_cast<const char **>(dxfname_ptr);
  Dwg_Object_Type* typep = reinterpret_cast<Dwg_Object_Type*>(type_ptr);
  int* is_entp = reinterpret_cast<int*>(is_ent_ptr);
  Dwg_Class_Stability* stabilityp = reinterpret_cast<Dwg_Class_Stability*>(stability_ptr);
  return dwg_object_name(name.c_str(), dxfnamep, typep, is_entp, stabilityp);
}

/********************************************************************
 *                    FUNCTIONS FOR LAYER OBJECT                     *
 ********************************************************************/

/**
 * Get/Set name (utf-8) of the layer object
 */
std::string dwg_obj_layer_get_name_wrapper(uintptr_t layer_ptr) {
  dwg_obj_layer* layer = reinterpret_cast<dwg_obj_layer*>(layer_ptr);
  int error = 0;
  return std::string(dwg_obj_layer_get_name(layer, &error));
}

int dwg_obj_layer_set_name_wrapper(
  uintptr_t layer_ptr,
  const std::string& name) {
  dwg_obj_layer* layer = reinterpret_cast<dwg_obj_layer*>(layer_ptr);
  int error = 0;
  dwg_obj_layer_set_name(layer, name.c_str(), &error);
  return error;
}

/*******************************************************************
 *                    FUNCTIONS FOR TABLES                          *
 *             All other tables and table entries                   *
 ********************************************************************/

/**
 * Get name of any table entry. Defaults to ByLayer.
 */
std::string dwg_obj_table_get_name_wrapper(uintptr_t obj_ptr) {
  dwg_object* obj = reinterpret_cast<dwg_object*>(obj_ptr);
  int error = 0;
  return std::string(dwg_obj_table_get_name(obj, &error));
}

/** 
 * Get name of the referenced table entry. Defaults to ByLayer
 */
std::string dwg_ref_get_table_name_wrapper(uintptr_t ref_ptr) {
  dwg_object_ref* ref = reinterpret_cast<dwg_object_ref*>(ref_ptr);
  int error = 0;
  return std::string(dwg_ref_get_table_name(ref, &error));
}

/********************************************************************
 *                FUNCTIONS TYPE SPECIFIC                            *
 *********************************************************************/
dwg_point_2d *dwg_ent_get_POINT2D_wrapper(
  uintptr_t obj_ptr,
  const std::string& fieldname) {
  void* obj = reinterpret_cast<void*>(obj_ptr);
  return dwg_ent_get_POINT2D(obj, fieldname.c_str());
}

bool dwg_ent_set_POINT2D_wrapper(
  uintptr_t obj_ptr,
  const std::string& fieldname,
  const dwg_point_2d *point) {
  void* obj = reinterpret_cast<void*>(obj_ptr);
  return dwg_ent_set_POINT2D(obj, fieldname.c_str(), point);
}

dwg_point_3d *dwg_ent_get_POINT3D_wrapper(
  uintptr_t obj_ptr,
  const std::string& fieldname) {
  void* obj = reinterpret_cast<void*>(obj_ptr);
  return dwg_ent_get_POINT3D(obj, fieldname.c_str());
}

bool dwg_ent_set_POINT3D_wrapper(
  uintptr_t obj_ptr,
  const std::string& fieldname,
  const dwg_point_3d *point) {
  void* obj = reinterpret_cast<void*>(obj_ptr);
  return dwg_ent_set_POINT3D(obj, fieldname.c_str(), point);
}

std::string dwg_ent_get_STRING_wrapper(
  uintptr_t obj_ptr,
  const std::string& fieldname) {
  void* obj = reinterpret_cast<void*>(obj_ptr);
  return std::string(dwg_ent_get_STRING(obj, fieldname.c_str()));
}

bool dwg_ent_set_STRING_wrapper(
  uintptr_t obj_ptr,
  const std::string& fieldname,
  const std::string& string) {
  void* obj = reinterpret_cast<void*>(obj_ptr);
  // TODO: double check whether to alloc memory for string before calling this function.
  return dwg_ent_set_STRING(obj, fieldname.c_str(), string.c_str());
}

std::string dwg_ent_get_UTF8_wrapper(
  uintptr_t obj_ptr,
  const std::string& fieldname) {
  void* obj = reinterpret_cast<void*>(obj_ptr);
  int isnew = 0;
  char* str_ptr = dwg_ent_get_UTF8(obj, fieldname.c_str(), &isnew);
  std::string result(str_ptr);
  if (isnew) free(str_ptr);
  return result;
}

bool dwg_ent_set_UTF8_wrapper(
  uintptr_t obj_ptr,
  const std::string& fieldname,
  const std::string& string) {
  void* obj = reinterpret_cast<void*>(obj_ptr);
  // TODO: double check whether to alloc memory for string before calling this function.
  return dwg_ent_set_UTF8(obj, fieldname.c_str(), string.c_str());
}

BITCODE_BD dwg_ent_get_REAL_wrapper(
  uintptr_t obj_ptr,
  const std::string& fieldname) {
  void* obj = reinterpret_cast<void*>(obj_ptr);
  return dwg_ent_get_REAL(obj, fieldname.c_str());
}

bool dwg_ent_set_REAL_wrapper(
  uintptr_t obj_ptr,
  const std::string& fieldname,
  const BITCODE_BD num) {
  void* obj = reinterpret_cast<void*>(obj_ptr);
  return dwg_ent_set_REAL(obj, fieldname.c_str(), num);
}

BITCODE_BS dwg_ent_get_INT16_wrapper(
  uintptr_t obj_ptr,
  const std::string& fieldname) {
  void* obj = reinterpret_cast<void*>(obj_ptr);
  return dwg_ent_get_INT16(obj, fieldname.c_str());
}

bool dwg_ent_set_INT16_wrapper(
  uintptr_t obj_ptr,
  const std::string& fieldname,
  const BITCODE_BS num) {
  void* obj = reinterpret_cast<void*>(obj_ptr);
  return dwg_ent_set_INT16(obj, fieldname.c_str(), num);
}

BITCODE_BL dwg_ent_get_INT32_wrapper(
  uintptr_t obj_ptr,
  const std::string& fieldname) {
  void* obj = reinterpret_cast<void*>(obj_ptr);
  return dwg_ent_get_INT32(obj, fieldname.c_str());
}

bool dwg_ent_set_INT32_wrapper(
  uintptr_t obj_ptr,
  const std::string& fieldname,
  const BITCODE_BL num) {
  void* obj = reinterpret_cast<void*>(obj_ptr);
  return dwg_ent_set_INT32(obj, fieldname.c_str(), num);
}

/**
 * Get handle value from Dwg_Object* 
 */
BITCODE_RLL dwg_obj_get_handle_value_wrapper(uintptr_t obj_ptr) {
  Dwg_Object* obj = reinterpret_cast<Dwg_Object*>(obj_ptr);
  return obj->handle.value;
}

/**
 * Get handle value from Dwg_Object_Ref* 
 */
BITCODE_RLL dwg_ref_get_handle_value_wrapper(uintptr_t obj_ref_ptr) {
  Dwg_Object_Ref* obj_ref = reinterpret_cast<Dwg_Object_Ref*>(obj_ref_ptr);
  return obj_ref->handleref.value;
}

/**
 * Get absolute_ref value from Dwg_Object_Ref* 
 */
BITCODE_RLL dwg_ref_get_handle_absolute_ref_wrapper(uintptr_t obj_ref_ptr) {
  Dwg_Object_Ref* obj_ref = reinterpret_cast<Dwg_Object_Ref*>(obj_ref_ptr);
  return obj_ref->absolute_ref;
}

uintptr_t dwg_get_object_wrapper(uintptr_t dwg_ptr, BITCODE_BL index) {
  Dwg_Data* dwg = reinterpret_cast<Dwg_Data*>(dwg_ptr);
  return reinterpret_cast<uintptr_t>(dwg_get_object(dwg, index));  
}

/**
 * Returns Dwg_Object* from absolute reference
 */
uintptr_t dwg_absref_get_object_wrapper(
  uintptr_t dwg_ptr,
  const BITCODE_BL absref) {
  Dwg_Data* dwg = reinterpret_cast<Dwg_Data*>(dwg_ptr);
  return reinterpret_cast<uintptr_t>(dwg_absref_get_object(dwg, absref));
}

EMSCRIPTEN_BINDINGS(libredwg_api) {
  DEFINE_FUNC(dwg_read_file);
  // DEFINE_FUNC(dxf_read_file);
  // DEFINE_FUNC(dwg_write_file);
  DEFINE_FUNC(dwg_bmp);

  DEFINE_FUNC(dwg_find_tablehandle);
  DEFINE_FUNC(dwg_find_tablehandle_index);
  DEFINE_FUNC(dwg_handle_name);
  DEFINE_FUNC(dwg_find_table_control);
  DEFINE_FUNC(dwg_find_dictionary);
  DEFINE_FUNC(dwg_find_dicthandle);
  DEFINE_FUNC(dwg_find_dicthandle_objname);
  DEFINE_FUNC(dwg_find_table_extname);
  DEFINE_FUNC(dwg_variable_dict);
  DEFINE_FUNC(dwg_model_x_min);
  DEFINE_FUNC(dwg_model_x_max);
  DEFINE_FUNC(dwg_model_y_min);
  DEFINE_FUNC(dwg_model_y_max);
  DEFINE_FUNC(dwg_model_z_min);
  DEFINE_FUNC(dwg_model_z_max);
  DEFINE_FUNC(dwg_page_x_min);
  DEFINE_FUNC(dwg_page_x_max);
  DEFINE_FUNC(dwg_page_y_min);
  DEFINE_FUNC(dwg_page_y_max);
  DEFINE_FUNC(dwg_block_control);
  DEFINE_FUNC(dwg_model_space_ref);
  DEFINE_FUNC(dwg_paper_space_ref);
  DEFINE_FUNC(dwg_model_space_object);
  DEFINE_FUNC(dwg_paper_space_object);
  DEFINE_FUNC(dwg_get_layer_count);
  DEFINE_FUNC(dwg_get_layer_index);
  DEFINE_FUNC(dwg_get_num_objects);
  DEFINE_FUNC(dwg_get_object_num_objects);
  DEFINE_FUNC(dwg_class_is_entity);
  DEFINE_FUNC(dwg_obj_is_control);
  DEFINE_FUNC(dwg_obj_is_table);
  DEFINE_FUNC(dwg_obj_is_subentity);
  DEFINE_FUNC(dwg_obj_has_subentity);
  DEFINE_FUNC(dwg_obj_is_3dsolid);  
  DEFINE_FUNC(dwg_obj_is_acsh);
  DEFINE_FUNC(dwg_get_num_entities);
  DEFINE_FUNC(dwg_get_entity_index);
  DEFINE_FUNC(dwg_get_entity_layer);
  DEFINE_FUNC(dwg_next_object);
  DEFINE_FUNC(dwg_next_entity);
  DEFINE_FUNC(dwg_next_handle);
  DEFINE_FUNC(dwg_next_handseed);

  DEFINE_FUNC(get_first_owned_entity);
  DEFINE_FUNC(get_next_owned_entity);
  DEFINE_FUNC(get_first_owned_subentity);
  DEFINE_FUNC(get_next_owned_subentity);
  DEFINE_FUNC(get_first_owned_block);
  DEFINE_FUNC(get_last_owned_block);
  DEFINE_FUNC(get_next_owned_block);
  DEFINE_FUNC(get_next_owned_block_entity);
  DEFINE_FUNC(dwg_get_first_object);
  DEFINE_FUNC(dwg_get_next_object);

  DEFINE_FUNC(dwg_resolve_jump);
  DEFINE_FUNC(dwg_section_type);
  // DEFINE_FUNC(dwg_section_wtype);
  DEFINE_FUNC(dwg_section_name);
  function("dwg_resbuf_value_type", &dwg_resbuf_value_type);
  DEFINE_FUNC(dwg_free);
  DEFINE_FUNC(dwg_free_object);
  DEFINE_FUNC(dwg_new_ref);
  DEFINE_FUNC(dwg_add_handle);
  DEFINE_FUNC(dwg_add_handleref);
  DEFINE_FUNC(dwg_dup_handleref);
  DEFINE_FUNC(dwg_add_handleref_free);
  DEFINE_FUNC(dwg_version_type);
  DEFINE_FUNC(dwg_version_as);
  DEFINE_FUNC(dwg_version_hdr_type);
  DEFINE_FUNC(dwg_supports_eed);
  DEFINE_FUNC(dwg_supports_obj);
  function("dwg_errstrings", &dwg_errstrings);
  DEFINE_FUNC(dwg_encrypt_SAT1);
  // DEFINE_FUNC(dwg_convert_SAB_to_SAT1);
  function("dwg_rgb_palette_index", &dwg_rgb_palette_index);
  function("dwg_find_color_index", &dwg_find_color_index);
  DEFINE_FUNC(dwg_add_object);
  DEFINE_FUNC(dwg_object_name);

  DEFINE_FUNC(dwg_obj_layer_get_name);
  DEFINE_FUNC(dwg_obj_layer_set_name);
  DEFINE_FUNC(dwg_obj_table_get_name);
  DEFINE_FUNC(dwg_ref_get_table_name);

  class_<dwg_point_3d>("dwg_point_3d")
    .property("x", &dwg_point_3d::x)
    .property("y", &dwg_point_3d::y)
    .property("z", &dwg_point_3d::z);

  class_<dwg_point_2d>("dwg_point_2d")
    .property("x", &dwg_point_2d::x)
    .property("y", &dwg_point_2d::y);

  DEFINE_FUNC_WITH_REF_POLICY(dwg_ent_get_POINT2D);
  DEFINE_FUNC_WITH_REF_POLICY(dwg_ent_set_POINT2D);
  DEFINE_FUNC_WITH_REF_POLICY(dwg_ent_get_POINT3D);
  DEFINE_FUNC_WITH_REF_POLICY(dwg_ent_set_POINT3D);
  DEFINE_FUNC(dwg_ent_get_STRING);
  DEFINE_FUNC(dwg_ent_set_STRING);
  DEFINE_FUNC(dwg_ent_get_UTF8);
  DEFINE_FUNC(dwg_ent_set_UTF8);
  DEFINE_FUNC(dwg_ent_get_REAL);
  DEFINE_FUNC(dwg_ent_set_REAL);
  DEFINE_FUNC(dwg_ent_get_INT16);
  DEFINE_FUNC(dwg_ent_set_INT16);
  DEFINE_FUNC(dwg_ent_get_INT32);
  DEFINE_FUNC(dwg_ent_set_INT32);
  DEFINE_FUNC(dwg_obj_get_handle_value);
  DEFINE_FUNC(dwg_ref_get_handle_value);
  DEFINE_FUNC(dwg_ref_get_handle_absolute_ref);
  
  DEFINE_FUNC(dwg_get_object);
  DEFINE_FUNC(dwg_absref_get_object);
}
