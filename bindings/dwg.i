/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2010,2011,2018 Free Software Foundation, Inc.              */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * dwg.i: SWIG interface file
 * written by Rodrigo Rodrigues da Silva
 * ideas contributed by James Michael DuPont
 * modified by Reini Urban
 */

%module libredwg

%{
#include "../src/config.h"
#include "dwg_api.h"
%}

%include "carrays.i"
%array_functions(Dwg_Object, Dwg_Object_Array);

%include "dwg.h"
%include "dwg_api.h"
