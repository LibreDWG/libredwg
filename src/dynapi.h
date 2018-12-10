/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2018 Free Software Foundation, Inc.                        */
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
#include <dwg.h>
#include <stdbool.h>

#ifdef HAVE_STDDEF_H
#include <stddef.h>
#define OFF(st,f,def) offsetof(st, f)
#else
#define OFF(st,f,def) def
#endif

typedef struct _name_type_offset {
  const char *const name;
  const char *const type;
  int offset;
} Dwg_DYNAPI_field;

const Dwg_DYNAPI_field*
dwg_dynapi_entity_fields(const char* name);
const Dwg_DYNAPI_field*
dwg_dynapi_object_fields(const char* name);

EXPORT bool
is_dwg_entity(const char* name);

EXPORT bool
is_dwg_object(const char* name);

#endif
