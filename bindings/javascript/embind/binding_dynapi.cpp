#include <emscripten/bind.h>
#include <string>
#include <sstream>

#include "dwg.h"
#include "dwg_api.h"
#include "common.h"
#include "bits.h"
#include "binding_common.h"

using namespace emscripten;

/**
 * Returns 1 if the string is not empty and the last character is the
 * specified character.
 */
int is_last_char(const char *str, const char c) {
  if (str != NULL && str[strlen(str) - 1] == c) {
      return 1; // Last character is '*'
  }
  return 0; // Last character is not '*'
}

/* converts UCS-2LE to UTF-8.
   first pass to get the dest len. single malloc.
 */
char *bit_convert_TU (const BITCODE_TU restrict wstr) {
  BITCODE_TU tmp = wstr;
  char *str; 
  int i, len = 0;
  uint16_t c = 0;

  if (!wstr) return NULL;

  while ((c = *tmp++)) {
    len++;
    if (c >= 0x80) {
      len++;
      if (c >= 0x800) len++;
    }
  }
  str = (char *)malloc (len + 1);
  if (!str) {
    return NULL;
  }
  i = 0;
  tmp = wstr;

  while ((c = *tmp++) && i < len) {
    if (c < 0x80) {
      str[i++] = c & 0xFF;
    } else if (c < 0x800) {
      str[i++] = (c >> 6) | 0xC0;
      str[i++] = (c & 0x3F) | 0x80;
    } else { /* if (c < 0x10000) */
      /* windows ucs-2 has no D800-DC00 surrogate pairs. go straight up */
      str[i++] = (c >> 12) | 0xE0;
      str[i++] = ((c >> 6) & 0x3F) | 0x80;
      str[i++] = (c & 0x3F) | 0x80;
    }
  }
  if (i <= len + 1)
    str[i] = '\0';
  return str;
}

/** 
 * Check if the name is a valid ENTITY name, not an OBJECT.
 */
bool is_dwg_entity_wrapper(const std::string& name) {
  return is_dwg_entity(name.c_str());
}

/** 
 * Check if the name is a valid OBJECT name, not an ENTITY.
 */
bool is_dwg_object_wrapper(const std::string& name) {
  return is_dwg_object(name.c_str());
}

template <typename T>
emscripten::val get_obj_value(const Dwg_Data *dwg, T _obj, const Dwg_DYNAPI_field *f) {
  emscripten::val result = emscripten::val::object();
  result.set("success", true); 
  if (f->is_string || strEQc(f->type, "TF")) {
    // UTF8 String
    const bool is_tu = dwg ? IS_FROM_TU_DWG (dwg) : false;
    if (is_tu && strNE (f->type, "TF")) { /* not TF */
        BITCODE_TU wstr = *(BITCODE_TU*)((char*)_obj + f->offset);
        char *utf8 = bit_convert_TU(wstr);
        if (wstr && !utf8) { // some conversion error, invalid wchar (nyi)
          result.set("success", false);
          result.set("message", std::string("Failed to convert string!"));
          return result;
        }
        result.set("data", std::string(utf8));
        free(utf8);
    } else {
      char *utf8 = *(char **)((char*)_obj + f->offset);
      result.set("data", std::string(utf8));
    }
  } else if (strEQc(f->type, "RL") || strEQc(f->type, "BL") || strEQc (f->type, "MS")) {
    // INT32
    result.set("data", *reinterpret_cast<BITCODE_BL*>(&((char *)_obj)[f->offset]));
  } else if (strEQc (f->type, "RS") || strEQc (f->type, "BS")) {
    // INT16
    result.set("data", *reinterpret_cast<BITCODE_BS*>(&((char *)_obj)[f->offset]));
  } else if (strEQc(f->type, "RD") || strEQc(f->type, "BD")) {
    // DOUBLE
    result.set("data", *reinterpret_cast<BITCODE_BD*>(&((char *)_obj)[f->offset]));
  } else if (strEQc(f->type, "RC") || strEQc(f->type, "BB") || strEQc(f->type, "B")) {
    // UNSIGNED CHAR
    result.set("data", *reinterpret_cast<BITCODE_RC*>(&((char *)_obj)[f->offset]));
  } else if (strEQc(f->type, "RCd")) {
    // SIGNED CHAR
    result.set("data", *reinterpret_cast<BITCODE_RCd*>(&((char *)_obj)[f->offset]));
  } else if (strEQc(f->type, "3RD") || strEQc(f->type, "3BD") || strEQc(f->type, "3BD_1") || 
             strEQc(f->type, "BE") || strEQc(f->type, "3DPOINT")) {
    // POINT3D
    auto point = reinterpret_cast<dwg_point_3d*>(&((char *)_obj)[f->offset]);
    emscripten::val point_obj = point3d_to_js_object(point);
    result.set("data", point_obj);
  } else if (strEQc(f->type, "2RD") || strEQc(f->type, "2BD") || strEQc(f->type, "2BD_1") ||
             strEQc(f->type, "2DPOINT")) {
    // POINT2D
    auto point = reinterpret_cast<dwg_point_2d*>(&((char *)_obj)[f->offset]);
    emscripten::val point_obj = point2d_to_js_object(point);
    result.set("data", point_obj);
  } else if (strEQc(f->type, "CMC")) {
    // Dwg_Color* (BITCODE_CMC)
    auto color = reinterpret_cast<Dwg_Color*>(&((char *)_obj)[f->offset]);
    auto js_object = color_to_js_object(color);
    result.set("data", js_object);
  } else if (strEQc(f->type, "H")) {
    // Dwg_Object_Ref* (BITCODE_H)
    auto object_ref = *reinterpret_cast<BITCODE_H*>(&((char *)_obj)[f->offset]);
    result.set("data", reinterpret_cast<uintptr_t>(object_ref));
  } else if (is_last_char(f->type, '*')) {
    // Array
    auto ptr = *reinterpret_cast<uintptr_t**>(&((char *)_obj)[f->offset]);
    result.set("data", reinterpret_cast<uintptr_t>(ptr));
  }

  // TODO: support "color_r11" (BITCODE_RCd)
  return result;
}

/** 
 * Returns the HEADER.fieldname value in out.
 * The optional Dwg_DYNAPI_field *fp is filled with the field types from dynapi.c
 */
emscripten::val dwg_dynapi_header_value_wrapper(
  uintptr_t dwg_ptr,
  const std::string& fieldname) {
  Dwg_Data* dwg = reinterpret_cast<Dwg_Data*>(dwg_ptr);
  if (!dwg) {
    emscripten::val result = emscripten::val::object();
    result.set("success", false); 
    result.set("message", std::string("Null dwg pointer passed!"));
    result.set("data", emscripten::val::null());
    return result;
  }

  emscripten::val result = emscripten::val::object();
  result.set("success", true); 
  const Dwg_DYNAPI_field *f = dwg_dynapi_header_field(fieldname.c_str());
  if (f) {
    const Dwg_Header_Variables *const _obj = &dwg->header_vars;
    return get_obj_value(dwg, _obj, f);
  } else {
    std::ostringstream ss;
    ss << "Invalid header field '" << fieldname << "'!";
    result.set("success", false); 
    result.set("message", ss.str());
    return result;
  }
}

/** 
 * Returns the ENTITY|OBJECT.fieldname value in out.
 * entity is the Dwg_Entity_ENTITY or Dwg_Object_OBJECT struct with the
 * specific fields. The optional Dwg_DYNAPI_field *fp is filled with the
 * field types from dynapi.c.
 */
emscripten::val dwg_dynapi_entity_value_wrapper(
  uintptr_t _obj_ptr, 
  const std::string& fieldname) {
  void* _obj = reinterpret_cast<void*>(_obj_ptr);
  if (!_obj) {
    emscripten::val result = emscripten::val::object();
    result.set("success", false); 
    result.set("message", std::string("Null object pointer passed!"));
    return result;
  }

  int error;
  const Dwg_Object* obj = dwg_obj_generic_to_object(_obj, &error);
  // Here we need to ignore errors, because we allow subentities via
  // CHK_SUBCLASS_* e.g. layout->plotsetting via PLOTSETTING
  if (!obj) { // objid may be 0
    emscripten::val result = emscripten::val::object();
    result.set("success", false); 
    result.set("message", std::string("Invalid object pointer passed!"));
    return result;
  }

  const Dwg_DYNAPI_field *f = dwg_dynapi_entity_field(obj->name, fieldname.c_str());
  if (!f) {
    std::ostringstream ss;
    ss << "Invalid object name '" << obj->name << "' or field name '" << fieldname << "'!";
    emscripten::val result = emscripten::val::object();
    result.set("success", false); 
    result.set("message", ss.str());
    return result;
  }

  const Dwg_Data *dwg = obj ? obj->parent : NULL;
  return get_obj_value(dwg, _obj, f);
}

/** 
 * Returns the common ENTITY|OBJECT.fieldname value in out.
 * _obj is the Dwg_Entity_ENTITY or Dwg_Object_OBJECT struct with the
 * specific fields. The optional Dwg_DYNAPI_field *fp is filled with the
 * field types from dynapi.c
 */
emscripten::val dwg_dynapi_common_value_wrapper(
  uintptr_t _obj_ptr, 
  const std::string& fieldname) {
  void* _obj = reinterpret_cast<void*>(_obj_ptr);
  if (!_obj) {
    emscripten::val result = emscripten::val::object();
    result.set("success", false); 
    result.set("message", std::string("Null object pointer passed!"));
    return result;
  }

  int error;
  const Dwg_Object* obj = dwg_obj_generic_to_object(_obj, &error);
  if (!obj || error) {
    emscripten::val result = emscripten::val::object();
    result.set("success", false); 
    result.set("message", std::string("Invalid object pointer passed!"));
    return result;
  }

  const Dwg_DYNAPI_field *f;
  if (obj->supertype == DWG_SUPERTYPE_ENTITY) {
    f = dwg_dynapi_common_entity_field(fieldname.c_str());
    _obj = obj->tio.entity;
  } else if (obj->supertype == DWG_SUPERTYPE_OBJECT) {
    f = dwg_dynapi_common_object_field(fieldname.c_str());
    _obj = obj->tio.object;
  } else {
    std::ostringstream ss;
    ss << "Unhandled " << obj->name << ".supertype!";
    emscripten::val result = emscripten::val::object();
    result.set("success", false); 
    result.set("message", ss.str());
    return result;
  }

  if (!f) {
    std::ostringstream ss;
    ss << "Invalid object name '" << obj->name << "' or field name '" << fieldname << "'!";
    emscripten::val result = emscripten::val::object();
    result.set("success", false); 
    result.set("message", ss.str());
    return result;
  }

  if (f->dxf == 160 && strEQc(fieldname.c_str(), "preview_size") && obj->parent->header.version < R_2010) {
    // INT32
    emscripten::val result = emscripten::val::object();
    result.set("success", false); 
    result.set("data", *reinterpret_cast<BITCODE_BL*>(&((char *)_obj)[f->offset]));
  } else {
    const Dwg_Data *dwg = obj ? obj->parent : NULL;
    return get_obj_value(dwg, _obj, f);
  }
}

/** 
 * Returns the common OBJECT.subclass.fieldname value in out.
 * ptr points to the subclass field. The optional Dwg_DYNAPI_field *fp is
 * filled with the field types from dynapi.c
 */
emscripten::val dwg_dynapi_subclass_value_wrapper(
  uintptr_t raw_ptr,
  const std::string& subclass,
  const std::string& fieldname) {
  void* ptr = reinterpret_cast<void*>(raw_ptr);
  if (!ptr) {
    emscripten::val result = emscripten::val::object();
    result.set("success", false); 
    result.set("message", std::string("Null object pointer passed!"));
    return result;
  }

  const char* subclass_ptr = subclass.c_str();
  const char* fieldname_ptr = fieldname.c_str();
  Dwg_DYNAPI_field *f = (Dwg_DYNAPI_field *)dwg_dynapi_subclass_field(subclass_ptr, fieldname_ptr);
  if (!f) {
    if (memBEGINc(subclass_ptr, "Dwg_Object_"))
    f = (Dwg_DYNAPI_field *)dwg_dynapi_entity_field(&subclass_ptr[strlen("Dwg_Object_")], fieldname_ptr);
    if (!f) {
      std::ostringstream ss;
      ss << "Invalid subclass name '" << subclass << "' or field name '" << fieldname << "'!";
      emscripten::val result = emscripten::val::object();
      result.set("success", false); 
      result.set("message", ss.str());
      return result;
    }
  }

  return get_obj_value(NULL, ptr, f);
}


/** 
 * Sets the HEADER.fieldname to a value.
 * A malloc'ed struct or string is passed by ptr, not by the content.
 * A non-malloc'ed struct is set by content.
 * If is_utf8 is set, the given value is a UTF-8 string, and will be
 * converted to TV or TU.
*/
bool dwg_dynapi_header_set_value_wrapper(
  uintptr_t dwg_ptr,
  const std::string& fieldname,
  uintptr_t value_ptr,
  const bool is_utf8) {
  Dwg_Data* dwg = reinterpret_cast<Dwg_Data*>(dwg_ptr);
  void* value = reinterpret_cast<void*>(value_ptr);
  return dwg_dynapi_header_set_value(dwg, fieldname.c_str(), value, is_utf8);
}

/** 
 * Sets the ENTITY.fieldname to a value.
 * A malloc'ed struct is passed by ptr, not by the content.
 * A non-malloc'ed struct is set by content.
 * Arrays or strings must be malloced before. We just set the new pointer,
 * the old value will be freed.
 * If is_utf8 is set, the given value is a UTF-8 string, and will be
 * converted to TV or TU.
 */
bool dwg_dynapi_entity_set_value_wrapper(
  uintptr_t entity_ptr,
  const std::string& dxfname,
  const std::string& fieldname,
  uintptr_t value_ptr,
  const bool is_utf8) {
  void* entity = reinterpret_cast<void*>(entity_ptr);
  void* value = reinterpret_cast<void*>(value_ptr);
  return dwg_dynapi_entity_set_value(entity, dxfname.c_str(), fieldname.c_str(), value, is_utf8);
}

/**
 * Sets the common ENTITY or OBJECT.fieldname to a value.
 * A malloc'ed struct is passed by ptr, not by the content.
 * A non-malloc'ed struct is set by content.
 * Arrays or strings must be malloced before. We just set the new pointer,
 * the old value will be freed.
 * If is_utf8 is set, the given value is a UTF-8 string, and will be
 * converted to TV or TU.
 */
bool dwg_dynapi_common_set_value_wrapper(
  uintptr_t obj_ptr,
  const std::string& fieldname,
  uintptr_t value_ptr,
  const bool is_utf8) {
  void* obj = reinterpret_cast<void*>(obj_ptr);
  void* value = reinterpret_cast<void*>(value_ptr);
  return dwg_dynapi_common_set_value(obj, fieldname.c_str(), value, is_utf8);
}

std::string dwg_dynapi_handle_name_wrapper(
  uintptr_t dwg_ptr,
  uintptr_t hdl_ptr,
  uintptr_t alloced_ptr) {
  Dwg_Data* dwg = reinterpret_cast<Dwg_Data*>(dwg_ptr);
  Dwg_Object_Ref* hdl = reinterpret_cast<Dwg_Object_Ref*>(hdl_ptr);
  int* alloced = reinterpret_cast<int*>(alloced_ptr);
  return std::string(dwg_dynapi_handle_name(dwg, hdl, alloced));
}

EMSCRIPTEN_BINDINGS(libredwg_dynapi) {
  DEFINE_FUNC(is_dwg_entity);
  DEFINE_FUNC(is_dwg_object);
  DEFINE_FUNC(dwg_dynapi_header_value);
  DEFINE_FUNC(dwg_dynapi_entity_value);
  DEFINE_FUNC(dwg_dynapi_common_value);
  DEFINE_FUNC(dwg_dynapi_subclass_value);
  DEFINE_FUNC(dwg_dynapi_header_set_value);
  DEFINE_FUNC(dwg_dynapi_entity_set_value);
  DEFINE_FUNC(dwg_dynapi_common_set_value);
  DEFINE_FUNC(dwg_dynapi_handle_name);
}