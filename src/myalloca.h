/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2019 Free Software Foundation, Inc.                        */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * myalloca.h: alloca checks and replacement, use with FREEa()
 * written by Reini Urban
 */

#ifndef MYALLOCA_H
#define MYALLOCA_H

#include "config.h"
#if defined HAVE_ALLOCA_H
#  include <alloca.h>
#elif defined __GNUC__
#  undef alloca
#  define alloca __builtin_alloca
#elif defined _AIX
#  undef alloca
#  define alloca __alloca
#elif defined _MSC_VER
#  undef alloca
#  include <malloc.h>
#  define alloca _alloca
#else
#  include <stddef.h>
#  ifdef __cplusplus
extern "C"
#  endif
    void *
    alloca (size_t);
#endif

#ifndef HAVE_ALLOCA
static inline char *alloca (size_t size);
static inline char *
alloca (size_t size)
{
  return MALLOC (size);
}
#  define FREEa(ptr) FREE (ptr)
#else
#  define FREEa(ptr)
#endif

#endif
