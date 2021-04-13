/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2018-2021 Free Software Foundation, Inc.                   */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * free.h: helper functions to free all spec fields
 * written by Reini Urban
 *
 * now empty, all methods are public
 */

#ifndef FREE_H
#define FREE_H

#include "config.h"
#include "dwg.h"
#include "common.h"
#include "bits.h"

#ifndef HAVE_LIBGC
EXPORT void dwg_free_eed (Dwg_Object *obj);
/* Needed when we cast types.
   By fixedtype, not dxfname. */
EXPORT void dwg_free_object_private (Dwg_Object *obj);

int dwg_free_variable_type_private (Dwg_Object *restrict obj);
int dwg_free_variable_type (Dwg_Data *restrict dwg, Dwg_Object *restrict obj);
int dwg_free_variable_no_class (Dwg_Data *restrict dwg,
                                Dwg_Object *restrict obj);

#else
#  define dwg_free_eed(x)
#  define dwg_free_object(x)
#  define dwg_free_object_private(x)
#endif

#endif
