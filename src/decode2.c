/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2026 Free Software Foundation, Inc.                        */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * decode2.c: second part of decoding functions (dwg2.spec entities/objects)
 * split from decode.c to reduce compile times.
 * written by Reini Urban
 */

#ifndef _DEFAULT_SOURCE
#  define _DEFAULT_SOURCE 1
#endif
#ifndef _GNU_SOURCE
#  define _GNU_SOURCE 1 /* for memmem on linux */
#endif
#ifdef __STDC_ALLOC_LIB__
#  define __STDC_WANT_LIB_EXT2__ 1 /* for strdup */
#else
#  define _USE_BSD 1
#endif
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <limits.h>

#define IS_DECODER
#define DWG2_SPEC
#include "common.h"
#include "bits.h"
#include "dwg.h"
#include "hash.h"
#include "decode.h"
#include "print.h"
#include "free.h"
#include "dynapi.h"

static unsigned int loglevel;
/* the current version per spec block */
static int cur_ver = 0;
static BITCODE_BL rcount1 = 0, rcount2 = 0;
static bool is_teigha = false;

#ifdef DWG_ABORT
static unsigned int errors = 0;
#  ifndef DWG_ABORT_LIMIT
#    define DWG_ABORT_LIMIT 20
#  endif
#endif

#ifdef USE_TRACING
static bool env_var_checked_p;
#endif /* USE_TRACING */
#define DWG_LOGLEVEL loglevel

#include "logging.h"

/* Forward declarations for non-static functions generated below */
GCC46_DIAG_IGNORE (-Wredundant-decls)
#define DWG_ENTITY(token)                                                     \
  int dwg_decode_##token (Bit_Chain *restrict dat,                            \
                          Dwg_Object *restrict obj);
#define DWG_ENTITY_END
#define DWG_OBJECT(token)                                                     \
  int dwg_decode_##token (Bit_Chain *restrict dat,                            \
                          Dwg_Object *restrict obj);
#define DWG_OBJECT_END
#include "objects.inc"
GCC46_DIAG_RESTORE
#undef DWG_ENTITY
#undef DWG_ENTITY_END
#undef DWG_OBJECT
#undef DWG_OBJECT_END

#include "dec_macros.h"

#include "dwg2.spec"
