/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2018-2020 Free Software Foundation, Inc.                   */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * dynapi.h: dynamic access to all object and field names and types
 * written by Reini Urban
 */

#ifndef DYNAPI_H
#define DYNAPI_H

#include "config.h"
#include <stdbool.h>
#include "common.h"
#include "dwg.h"

#ifdef HAVE_STDDEF_H
#  include <stddef.h>
#else /* cygwin */
#  ifndef offsetof
#    define offsetof(type, member) ((size_t) & (((type *)0)->member))
#  endif
#endif
#define OFF(st, f) offsetof (st, f)

// avoid double linkage on windows with unit-testing
#if defined(DYNAPI_TEST_C)
#  undef EXPORT
#  define EXPORT
#endif

#ifndef _DWG_API_H_
/* Public API, duplicate of dwg_api.h */
typedef struct dwg_field_name_type_offset
{
  const char *const name;    /* field name */
  const char *const type;    /* e.g "RS" for BITCODE_RS */
  const unsigned short size; /* e.g. 2 for RS, 4 for BL */
  const unsigned short offset;
  const unsigned short
      is_indirect : 1; // for pointers, references, like 3BD, CMC, H, TV
  const unsigned short is_malloc : 1; // for strings and dynamic arrays only,
                                      // H*, TV, 3BD*, unknown size
  const unsigned short is_string : 1; // for null-terminated strings, use
                                      // strcpy/wcscpy. not memcpy
  const short dxf;
} Dwg_DYNAPI_field;

EXPORT bool is_dwg_entity (const char *name) __nonnull ((1));
EXPORT bool is_dwg_object (const char *name) __nonnull ((1));
EXPORT bool dwg_dynapi_header_value (const Dwg_Data *restrict dwg,
                                     const char *restrict fieldname,
                                     void *restrict out,
                                     Dwg_DYNAPI_field *restrict fp)
    __nonnull ((1, 2, 3));
EXPORT bool
dwg_dynapi_entity_value (void *restrict entity, const char *restrict dxfname,
                         const char *restrict fieldname, void *restrict out,
                         Dwg_DYNAPI_field *restrict fp)
    __nonnull ((1, 2, 3, 4));
EXPORT bool
dwg_dynapi_common_value (void *restrict _obj, const char *restrict fieldname,
                         void *restrict out, Dwg_DYNAPI_field *restrict fp)
    __nonnull ((1, 2, 3));
// r_2007+ creates a fresh UTF-8 copy, <r2007 returns the field value
EXPORT bool dwg_dynapi_header_utf8text (const Dwg_Data *restrict dwg,
                                        const char *restrict fieldname,
                                        char **restrict out,  int *isnew,
                                        Dwg_DYNAPI_field *restrict fp)
    __nonnull ((1, 2, 3));
EXPORT bool
dwg_dynapi_entity_utf8text (void *restrict _obj, const char *restrict name,
                            const char *restrict fieldname,
                            char **restrict out, int *isnew,
                            Dwg_DYNAPI_field *restrict fp)
    __nonnull ((1, 2, 3, 4));
EXPORT bool dwg_dynapi_common_utf8text (void *restrict _obj,
                                        const char *restrict fieldname,
                                        char **restrict out, int *isnew,
                                        Dwg_DYNAPI_field *restrict fp)
    __nonnull ((1, 2, 3));

EXPORT bool dwg_dynapi_header_set_value (Dwg_Data *restrict dwg,
                                         const char *restrict fieldname,
                                         const void *restrict value,
                                         const bool is_utf8)
    __nonnull ((1, 2, 3));
EXPORT bool dwg_dynapi_entity_set_value (void *restrict entity,
                                         const char *restrict dxfname,
                                         const char *restrict fieldname,
                                         const void *restrict value,
                                         const bool is_utf8)
    __nonnull ((1, 2, 3, 4));
EXPORT bool dwg_dynapi_common_set_value (void *restrict entity,
                                         const char *restrict fieldname,
                                         const void *restrict value,
                                         const bool is_utf8)
    __nonnull ((1, 2, 3));

EXPORT bool
dwg_dynapi_subclass_value (const void *restrict ptr, const char *restrict subclass,
                           const char *restrict fieldname, void *restrict out,
                           Dwg_DYNAPI_field *restrict fp)
    __nonnull ((1, 2, 3, 4));
EXPORT bool
dwg_dynapi_field_get_value (const void *restrict ptr,
                            const Dwg_DYNAPI_field *restrict field,
                            void *restrict out)
  __nonnull ((1, 2, 3));

EXPORT bool
dwg_dynapi_field_set_value (const Dwg_Data *restrict dwg, /* only needed if unicode strings */
                            void *restrict ptr,
                            const Dwg_DYNAPI_field *restrict field,
                            const void *restrict value,
                            const bool is_utf8)
  __nonnull ((2, 3, 4));

EXPORT char *dwg_dynapi_handle_name (const Dwg_Data *restrict dwg,
                                     Dwg_Object_Ref *restrict hdl)
    __nonnull ((1, 2));

EXPORT const Dwg_DYNAPI_field *
dwg_dynapi_header_field (const char *restrict fieldname) __nonnull ((1));

EXPORT const Dwg_DYNAPI_field *
dwg_dynapi_entity_field (const char *restrict name,
                         const char *restrict fieldname) __nonnull ((1, 2));

EXPORT const Dwg_DYNAPI_field *
dwg_dynapi_subclass_field (const char *restrict name,
                           const char *restrict fieldname) __nonnull ((1, 2));

EXPORT const Dwg_DYNAPI_field *
dwg_dynapi_common_entity_field (const char *restrict fieldname)
    __nonnull ((1));

EXPORT const Dwg_DYNAPI_field *
dwg_dynapi_common_object_field (const char *restrict fieldname)
    __nonnull ((1));

/* The array of all fields, NULL terminated. Also for all objects. */
EXPORT const Dwg_DYNAPI_field *
dwg_dynapi_entity_fields (const char *restrict name) __nonnull ((1));

EXPORT const Dwg_DYNAPI_field *dwg_dynapi_common_entity_fields (void);

EXPORT const Dwg_DYNAPI_field *dwg_dynapi_common_object_fields (void);

/* Find the fields for this subclass. See dwg.h */
EXPORT const Dwg_DYNAPI_field *
dwg_dynapi_subclass_fields (const char *restrict name) __nonnull ((1));

/* Search fields by dxf. Returns the first found field,
   Sets unique to 1 if it's the only result */
EXPORT const Dwg_DYNAPI_field *
dwg_dynapi_field_dxf (const Dwg_DYNAPI_field *restrict fields,
                      const int dxf, int *restrict unique);

// The struct size of the object or entity
EXPORT int
dwg_dynapi_entity_size (const char *restrict name) __nonnull ((1));

// The struct size of the subclass
EXPORT int
dwg_dynapi_subclass_size (const char *restrict name) __nonnull ((1));

int _fields_size_sum (const Dwg_DYNAPI_field *restrict fields);

// The size of the entity or subclass name.
EXPORT int dwg_dynapi_fields_size (const char *restrict name) __nonnull ((1));

// Converts from the fields type, like "Dwg_MLINESTYLE_line*" to the
// subclass name, like "MLINESTYLE_line".
ATTRIBUTE_MALLOC char*
dwg_dynapi_subclass_name (const char *restrict type);

#endif

/* Searches in dwg_name_subclasses[].
   FIXME: Not yet ready. Need to expand all defines in the spec.
   Not yet public in 0.11.1 */
EXPORT bool
dwg_has_subclass (const char *restrict classname, const char *restrict subclass) __nonnull ((1, 2));

#endif
