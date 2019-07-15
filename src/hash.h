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

#ifndef HASH_H
#define HASH_H

/*
 * hash.h: simple fast int hashmap for the object_ref map,
 *         mapping uint32_t to uint32_t.
 *         0 kys and values are disallowed even if there's no deletion.
 * written by Reini Urban
 */

#include "config.h"
#include <stdint.h>
#include <inttypes.h>

#define HASH_LOAD 75 // in percent. recommended is 50
#define HASH_NOT_FOUND (uint32_t) - 1

struct _hashbucket
{
  uint32_t key;
  uint32_t value;
};
typedef struct _inthash
{
  struct _hashbucket *array; /* of key, value pairs */
  uint32_t size;
  uint32_t elems; // to get the fill rate
} dwg_inthash;

dwg_inthash *hash_new (uint32_t size);
uint32_t hash_get (dwg_inthash *hash, uint32_t key);
void hash_set (dwg_inthash *hash, uint32_t key, uint32_t value);
void hash_free (dwg_inthash *hash);

#endif
