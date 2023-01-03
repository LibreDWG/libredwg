/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2020 Free Software Foundation, Inc.                        */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * classes.h: map type to name. classify into unstable, debugging and
 * unhandled. written by Reini Urban
 */

#ifndef CLASSES_H
#define CLASSES_H

#include "config.h"
#include <stdbool.h>
#include "common.h"
#include "dwg.h"

/* helpers: is_type_unstable (type); ... */

const char *dwg_type_name (const Dwg_Object_Type type);
const char *dwg_type_dxfname (const Dwg_Object_Type type);
Dwg_Object_Type dwg_name_type (const char *name);
bool is_type_stable (const Dwg_Object_Type type);
bool is_type_unstable_all (const Dwg_Object_Type type);
bool is_type_unstable (const Dwg_Object_Type type);
bool is_type_debugging (const Dwg_Object_Type type);
bool is_type_unhandled (const Dwg_Object_Type type);
bool is_class_stable (const char *name) __nonnull_all;
bool is_class_unstable (const char *name) __nonnull_all;
bool is_class_debugging (const char *name) __nonnull_all;
bool is_class_unhandled (const char *name) __nonnull_all;
bool is_dxf_class_importable (const char *name) __nonnull_all;
bool dwg_find_class (const Dwg_Data *restrict dwg, const char *dxfname,
                     BITCODE_BS *numberp) __nonnull ((1, 2));
void object_alias (char *restrict name) __nonnull_all;
void entity_alias (char *restrict name) __nonnull_all;
bool has_entity_DD (const Dwg_Object *obj) __nonnull_all;
bool obj_has_strings (const Dwg_Object *obj) __nonnull_all;

#endif
