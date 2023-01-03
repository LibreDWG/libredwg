/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2009-2019 Free Software Foundation, Inc.                   */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * print.h: print helper function prototypes
 * written by Felipe Corrêa da Silva Sanches
 * modified by Rodrigo Rodrigues da Silva
 * modified by Reini Urban
 */

#ifndef PRINT_H
#define PRINT_H

#include "dwg.h"
#include "bits.h"

#ifdef IN_DXF_H
#  error in_dxf.h must be included after print.h because of FORMAT_BD
#endif

int dwg_print_object (Bit_Chain *dat, Dwg_Object *obj);

#endif
