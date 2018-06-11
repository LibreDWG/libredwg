/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2018 Free Software Foundation, Inc.                        */
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
 *         values are inlined into the array.
 * written by Reini Urban
 */

#include "hash.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include "logging.h"

dwg_inthash *hash_new(uint32_t size)
{
  dwg_inthash *hash = malloc(sizeof(dwg_inthash));
  uint32_t cap;
  if (!hash)
    return NULL;
  // multiply with load factor,
  // and round size to next power of 2 (fast) or prime (secure),
  cap = (uint32_t)(size * 100.0/HASH_LOAD);
  while (size <= cap) // this is slow, but only done once. clz would be much faster
    size <<= 1U;
  hash->array = calloc(size, 2*sizeof(uint32_t)); // key+value pairs
  hash->size = size;
  return hash;
}

// if exceeds load factor
static inline int hash_need_resize(dwg_inthash *hash, uint32_t size)
{
  return (uint32_t)(size * 100.0/HASH_LOAD) > hash->size;
}

static void hash_resize(dwg_inthash *hash, uint32_t size)
{
  uint32_t cap;
  dwg_inthash oldhash = *hash;
  // multiply with load factor,
  // and round size to next power of 2 (fast) or prime (secure).
  cap = size * 100/HASH_LOAD;
  while (size <= cap) // this is slow, but only done once
    size <<= 1U;
  if (size <= hash->size)
    {
      //LOG_TRACE("hash_resize ignored. new hash size smaller than old one %d <= %d",
      //          size, hash->size);
      return;
    }

  // key+value pairs
  hash->array = realloc(hash->array, size * sizeof(struct _hashbucket));
  if (!hash->array) {
    *hash = oldhash;
    return;
  }
  hash->size = size;
  // only if different clear the new slack and insert old
  if (oldhash.array != hash->array) // reinsert all the elements
    {
      uint32_t i;
      memset(hash->array, 0, size * sizeof(struct _hashbucket));
      // spread out the old elements in double space, less collisions
      for (i=0; i<oldhash.size; i++)
        {
          if (oldhash.array[i].key)
            hash_set(hash, oldhash.array[i].key,  oldhash.array[i].value);
        }
    }
  else // same ptr: clear the slack at the end
    {
      memset(&hash->array[oldhash.size], 0,
             (size - oldhash.size) * sizeof(struct _hashbucket));
    }
  return;
}

// found this gem by Thomas Mueller at stackoverflow. triviality treshold.
// it's like a normal murmur or jenkins finalizer,
// just statistically tested to be optimal.
// Note that this is entirely "insecure", the inverse func is trivial.
// We don't care as we deal with DWG and had linear search before.
static inline uint32_t hash_func(uint32_t key)
{
  key = ((key >> 16) ^ key) * 0x45d9f3b;
  key = ((key >> 16) ^ key) * 0x45d9f3b;
  key = (key >> 16) ^ key;
  return key;
}

// 0 is disallowed as key, even if there's no deletion.
uint32_t hash_get(dwg_inthash *hash, uint32_t key)
{
  uint32_t i = hash_func(key) % hash->size;
  uint32_t j = i;
  while (hash->array[i].key && hash->array[i].key != key)
    {
      //fprintf(stderr, "get collision at %d\n", i);
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

// search or insert. key and value 0 is forbidden.
void hash_set(dwg_inthash *hash, uint32_t key, uint32_t value)
{
  uint32_t i = hash_func(key) % hash->size;
  uint32_t j = i;
  if (key == 0) {
      fprintf(stderr, "forbidden 0 key");
      return;
  }
  // empty slot
  if (!hash->array[i].key) {
    hash->array[i].key = key;
    hash->array[i].value = value;
    return;
  }
  while (hash->array[i].key)
    {
      if (hash->array[i].key == key) { // found
        hash->array[i].value = value;
        return;
      }
      //fprintf(stderr, "set collision at %d\n", i);
      i++; // linear probing with wrap around
      if (i == hash->size)
        i = 0;
      if (i == j) // not found
        {
          //fprintf(stderr, "set not found at %d\n", i);
          i++;
          // if does not exist, add at i+1
          if (hash_need_resize(hash, hash->size+1)) {
            // up to here we have no coverage!
            //fprintf(stderr, "resize at %d\n", hash->size);
            hash_resize(hash, hash->size*2);
          }
          while (hash->array[i].key) // find next empty slot
            {
              // up to here we have no coverage!
              //fprintf(stderr, "set 2nd collision at %d\n", i);
              i++; // again linear probing with wrap around
              if (i == hash->size)
                i = 0;
              if (i == j) // not found
                {
                  //fprintf(stderr, "not found resize at %d\n", hash->size);
                  hash_resize(hash, hash->size*2); // guarantees new empty slots
                  hash_set(hash, key, value);
                  return;
                }
              else
                { // insert at empty slot
                  hash->array[i].key = key;
                  hash->array[i].value = value;
                  return;
                }
            }
        }
    }
  // empty slot
  hash->array[i].key = key;
  hash->array[i].value = value;
  return;
}

void hash_free(dwg_inthash *hash)
{
  free (hash->array);
  hash->array = NULL;
  hash->size = 0;
  free (hash);
}
