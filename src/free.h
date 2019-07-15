/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2018-2019 Free Software Foundation, Inc.                   */
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

#include "dwg.h"
#include "bits.h"

EXPORT void dwg_free_eed (Dwg_Object *obj);

#endif
