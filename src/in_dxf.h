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
 * in_dxf.h: read Ascii DXF to dwg
 * written by Reini Urban
 */

#ifndef IN_DXF_H
#define IN_DXF_H

#include "dwg.h"
#include "bits.h"
#include "decode.h"
#include "dynapi.h"

// global array of [obj -> [fields], ...]
typedef struct _dxf_field
{
  char *name;
  char *type;
  int dxf;
} Dxf_Field;

// to search obj ptr in array
typedef struct _dxf_objs
{
  Dwg_Object *obj;
  int num_fields;
  int size_fields;
  Dxf_Field *fields;
} Dxf_Objs;

typedef struct _dxf_pair
{
  short code;
  enum RES_BUF_VALUE_TYPE type;
  union
  {
    int i;
    char *s;
    long l;
    double d;
  } value;
} Dxf_Pair;

/* We need to postpone the HEADER handles from names,
   when we didn't read the TABLES yet, which sets the handle values.
   Store all handle fieldnames and string values into this array,
   which is prefixed with the number of stored items.
 */
struct array_hdl {
  char *field;
  char *name;
};
typedef struct _array_hdls {
  uint32_t nitems;
  uint32_t size;            // in chunks of 16
  struct array_hdl items[]; // Flexible array grows
} array_hdls;

array_hdls *array_push (array_hdls *restrict hdls, char *restrict field,
                        char *restrict name);
int matches_type (Dxf_Pair *restrict pair, const Dwg_DYNAPI_field *restrict f);

void dxf_add_field (Dwg_Object *restrict obj, const char *restrict name,
                    const char *restrict type, int dxf);
Dxf_Field *dxf_search_field (Dwg_Object *restrict obj,
                             const char *restrict name,
                             const char *restrict type, int dxf);

EXPORT int dwg_read_dxf (Bit_Chain *restrict dat, Dwg_Data *restrict dwg);
EXPORT int dwg_read_dxfb (Bit_Chain *restrict dat, Dwg_Data *restrict dwg);

#endif
