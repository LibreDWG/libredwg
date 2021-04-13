/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2018-2019,2023 Free Software Foundation, Inc.              */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * hash.c: int hashmap for the object_ref map.
 *         uses linear probing for best cache usage.
 *         values are inlined into the array. The 0 key is disallowed.
 * written by Reini Urban
 */

#include "hash.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "logging.h"
#include "common.h"

dwg_inthash *
hash_new (uint64_t size)
{
  dwg_inthash *hash = (dwg_inthash *)MALLOC (sizeof (dwg_inthash));
  uint64_t cap;
  if (!hash)
    return NULL;
  // multiply with load factor,
  // and round size to next power of 2 (fast) or prime (secure),
  if (size < 15)
    size = 15;
  cap = (uint64_t)(size * 100.0 / HASH_LOAD);
  // this is slow, but only done once. clz would be much faster
  while (size <= cap)
    size <<= 1U;
  hash->array = (struct _hashbucket *)CALLOC (
      size, sizeof (struct _hashbucket)); // key+value pairs
  hash->elems = 0;
  hash->size = size;
  return hash;
}

// if exceeds load factor
static inline int
hash_need_resize (dwg_inthash *hash)
{
  return (uint64_t)(hash->elems * 100.0 / HASH_LOAD) > hash->size;
}

static void
hash_resize (dwg_inthash *hash)
{
  dwg_inthash oldhash = *hash;
  uint64_t size = hash->size * 2;
  uint64_t i;

  // allocate key+value pairs afresh
  hash->array
      = (struct _hashbucket *)CALLOC (size, sizeof (struct _hashbucket));
  if (!hash->array)
    {
      *hash = oldhash;
      return;
    }
  hash->elems = 0;
  hash->size = size;
  memset (hash->array, 0, size * sizeof (struct _hashbucket));
  // spread out the old elements in double space, less collisions
  for (i = 0; i < oldhash.size; i++)
    {
      if (oldhash.array[i].key)
        hash_set (hash, oldhash.array[i].key, oldhash.array[i].value);
    }
  FREE (oldhash.array);
  return;
}

// found this gem by Thomas Mueller at stackoverflow. triviality threshold.
// it's like a normal murmur or jenkins finalizer,
// just statistically tested to be optimal.
// 2023: changed to 64bit, checked at https://nullprogram.com/blog/2018/07/31/
// Note that this is entirely "insecure", the inverse func is trivial.
// We don't care as we deal with DWG and had linear search before.
static inline uint64_t
hash_func (uint64_t key)
{
  key = ((key >> 32) ^ key) * UINT64_C (0xd6e8feb86659fd93);
  key = ((key >> 32) ^ key) * UINT64_C (0xd6e8feb86659fd93);
  key = (key >> 32) ^ key;
  return key;
}

// 0 is disallowed as key, even if there's no deletion.
uint64_t
hash_get (dwg_inthash *hash, uint64_t key)
{
  uint64_t i = hash_func (key) % hash->size;
  uint64_t j = i;
  while (hash->array[i].key && hash->array[i].key != key)
    {
      // HANDLER (OUTPUT, "get collision at %d\n", i);
      i++; // linear probing with wrap around
      if (i == hash->size)
        i = 0;
      if (i == j) // not found
        return HASH_NOT_FOUND;
    }
  if (hash->array[i].key)
    return hash->array[i].value;
  else
    return HASH_NOT_FOUND;
}

// search or insert. key 0 is forbidden.
void
hash_set (dwg_inthash *hash, uint64_t key, uint64_t value)
{
  uint64_t i = hash_func (key) % hash->size;
  uint64_t j = i;
  if (key == 0)
    {
      HANDLER (OUTPUT, "forbidden 0 key\n");
      return;
    }
  // empty slot
  if (!hash->array[i].key)
    {
      hash->array[i].key = key;
      hash->array[i].value = value;
      hash->elems++;
      return;
    }
  while (hash->array[i].key)
    {
      if (hash->array[i].key == key)
        { // found
          hash->array[i].value = value;
          return;
        }
      // HANDLER (OUTPUT, "set collision at %d\n", i);
      i++; // linear probing with wrap around
      if (i == hash->size)
        i = 0;
      if (i == j) // not found
        {
          // HANDLER (OUTPUT, "set not found at %d\n", i);
          // if does not exist, add at i+1
          if (hash_need_resize (hash))
            {
              // HANDLER (OUTPUT, "resize at %d\n", hash->size);
              hash_resize (hash);
              hash_set (hash, key, value);
              return;
            }
          while (hash->array[i].key) // find next empty slot
            {
              // up to here we have no coverage!
              // HANDLER (OUTPUT, "set 2nd collision at %d\n", i);
              i++; // again linear probing with wrap around
              if (i == hash->size)
                i = 0;
              if (i == j) // not found
                {
                  // HANDLER (OUTPUT, "not found resize at %d\n", hash->size);
                  hash_resize (hash); // guarantees new empty slots
                  hash_set (hash, key, value);
                  return;
                }
              else
                { // insert at empty slot
                  hash->array[i].key = key;
                  hash->array[i].value = value;
                  hash->elems++;
                  return;
                }
            }
        }
    }
  // empty slot
  hash->array[i].key = key;
  hash->array[i].value = value;
  hash->elems++;
  return;
}

void
hash_free (dwg_inthash *hash)
{
  FREE (hash->array);
  hash->array = NULL;
  hash->size = 0;
  hash->elems = 0;
  FREE (hash);
}
