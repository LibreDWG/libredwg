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
 * acds.c: the r2013+ AcDb:AcDsPrototype_1b datastorage section, which keeps
 * the SAB data of the ACIS entities (3DSOLID, REGION, BODY, surfaces).
 * See ODA chapter 24. The section is read by acds.spec, written by
 * encode_acds.c. Here the part shared by the readers and writers: the
 * assignment of the data records to the entities, and the list of the records
 * to write.
 * written by Reini Urban
 */

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "common.h"
#include "bits.h"
#include "dwg.h"
#include "acds.h"

#define DWG_LOGLEVEL loglevel
#include "logging.h"

/* The SAB data records of the AcDs _data_ segments, per entity handle
   (24.2.2.2 - 24.2.2.4). The datidx entry points to a 20 byte record header
   at segment + 48 + offset: size, 1, handle, local offset. The data
   starts at segment + (objdata_algn_offset << 4) + local offset with a
   UInt32 size. Large data (> 0x40000) is a blob reference record to
   pages in blob01 segments. */
#define ACDS_SEG_HDR_SIZE 48
// instead of the data size at the start of a blob reference record
#define ACDS_BLOB_REF_MARKER 0xBB106BB1U

static uint32_t
acds_u32 (const unsigned char *p)
{
  return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16)
         | ((uint32_t)p[3] << 24);
}

static uint64_t
acds_u64 (const unsigned char *p)
{
  return (uint64_t)acds_u32 (p) | ((uint64_t)acds_u32 (p + 4) << 32);
}

/* The file offset of segment idx of the given name type, or 0 */
static size_t
acds_segment_offset (const Dwg_AcDs *restrict acds,
                     const Bit_Chain *restrict dat, const BITCODE_RL idx,
                     const int type)
{
  size_t off;
  if (idx >= acds->num_segidx || !acds->segidx || !acds->segments
      || acds->segments[idx].type != type)
    return 0;
  off = (size_t)acds->segidx[idx].offset;
  if (!off || off + ACDS_SEG_HDR_SIZE > dat->size)
    return 0;
  return off;
}

/* Assemble a blob from its pages. Returns a malloc'ed buffer or NULL. */
static unsigned char *
acds_read_blob (const Dwg_AcDs *restrict acds, const Bit_Chain *restrict dat,
                const unsigned char *restrict ref, const size_t len,
                size_t *restrict psize)
{
  const uint64_t total = acds_u64 (ref);
  const uint32_t num_pages = acds_u32 (ref + 8);
  unsigned char *blob;
  uint32_t i;
  if (total > 0x7fffffff || !num_pages || len != 32 + (size_t)num_pages * 8)
    return NULL;
  blob = (unsigned char *)CALLOC (1, (size_t)total + 1);
  if (!blob)
    return NULL;
  for (i = 0; i < num_pages; i++)
    {
      const uint32_t pidx = acds_u32 (ref + 32 + i * 8);
      const uint32_t pgsize = acds_u32 (ref + 32 + i * 8 + 4);
      const size_t off = acds_segment_offset (acds, dat, pidx, 6);
      const unsigned char *pg;
      uint64_t start, size;
      if (!off || off + ACDS_SEG_HDR_SIZE + 32 > dat->size)
        goto fail;
      pg = &dat->chain[off + ACDS_SEG_HDR_SIZE];
      start = acds_u64 (pg + 8);
      size = acds_u64 (pg + 24);
      if (size != pgsize || start > total || size > total - start
          || off + ACDS_SEG_HDR_SIZE + 32 + size > dat->size)
        goto fail;
      memcpy (&blob[start], pg + 32, (size_t)size);
    }
  *psize = (size_t)total;
  return blob;
fail:
  FREE (blob);
  return NULL;
}

/* Returns a malloc'ed copy of the data record of the datidx entry, with its
   handle and size. NULL if not a valid record. */
static unsigned char *
acds_read_record (const Dwg_AcDs *restrict acds, const Bit_Chain *restrict dat,
                  const Dwg_AcDs_DataIndex_Entry *restrict e,
                  BITCODE_RLL *restrict phandle, size_t *restrict psize)
{
  const size_t segoff = acds_segment_offset (acds, dat, e->segidx, 2);
  size_t hpos, dpos, len;
  uint32_t local;
  unsigned char *rec;
  if (!segoff)
    return NULL;
  hpos = segoff + ACDS_SEG_HDR_SIZE + e->offset;
  if (hpos + 20 > dat->size)
    return NULL;
  *phandle = acds_u64 (&dat->chain[hpos + 8]);
  local = acds_u32 (&dat->chain[hpos + 16]);
  dpos = segoff + ((size_t)acds->segments[e->segidx].objdata_algn_offset << 4)
         + local;
  if (dpos + 4 > dat->size)
    return NULL;
  len = acds_u32 (&dat->chain[dpos]);
  if (len == ACDS_BLOB_REF_MARKER)
    {
      // a blob reference record, with its size in it
      if (dpos + 4 + 40 > dat->size)
        return NULL;
      len = acds_u32 (&dat->chain[dpos + 4 + 12]);
      if (dpos + 4 + len > dat->size)
        return NULL;
      return acds_read_blob (acds, dat, &dat->chain[dpos + 4], len, psize);
    }
  if (dpos + 4 + len > dat->size)
    return NULL;
  rec = (unsigned char *)MALLOC (len ? len : 1);
  if (!rec)
    return NULL;
  memcpy (rec, &dat->chain[dpos + 4], len);
  *psize = len;
  return rec;
}

void
acds_assign_sab (Dwg_Data *restrict dwg, const Bit_Chain *restrict dat,
                 Dwg_AcDs *restrict acds)
{
  BITCODE_RL i;
  unsigned num = 0, num_solids = 0;

  for (i = 0; acds->datidx.entries && i < acds->datidx.num_entries; i++)
    {
      const Dwg_AcDs_DataIndex_Entry *e = &acds->datidx.entries[i];
      BITCODE_RLL handle = 0;
      size_t size = 0;
      unsigned char *data;
      Dwg_Object *o;
      BITCODE_BL sab_size;

      if (!e->segidx) // stub entry
        continue;
      data = acds_read_record (acds, dat, e, &handle, &size);
      if (!data)
        {
          LOG_WARN ("Invalid AcDs data record %u, segidx %u offset %u", i,
                    e->segidx, e->offset);
          continue;
        }
      o = dwg_resolve_handle (dwg, handle);
      if (!o || o->supertype != DWG_SUPERTYPE_ENTITY
          || !dwg_obj_is_3dsolid (o))
        {
          // e.g. the thumbnail PNG, or an entity we cannot decode. Keep the
          // record, to write it back.
          Dwg_AcDs_Record *rec = (Dwg_AcDs_Record *)REALLOC (
              acds->records,
              (acds->num_records + 1) * sizeof (Dwg_AcDs_Record));
          LOG_TRACE ("AcDs data record " FORMAT_RLL ": %" PRIuSIZE
                     " bytes, no ACIS entity\n",
                     handle, size);
          if (!rec)
            {
              FREE (data);
              continue;
            }
          acds->records = rec;
          rec = &acds->records[acds->num_records++];
          rec->handle = handle;
          rec->schidx = e->schidx;
          rec->size = size & 0xFFFFFFFF;
          rec->data = data;
          continue;
        }
      num_solids++;
      sab_size = size & 0xFFFFFFFF;
      // a later record of the same handle replaces an earlier
      if (dwg_obj_set_sab (o, data, sab_size))
        {
          FREE (data);
          continue;
        }
      LOG_TRACE ("%s.acis_data = %" PRIuSIZE " [" FORMAT_RLL "]\n", o->name,
                 size, handle);
      num++;
    }
  LOG_TRACE ("AcDs: %u SAB data of %u ACIS entities\n", num, num_solids);
}

static int
rec_cmp (const void *a, const void *b)
{
  const AcDs_Rec *ra = (const AcDs_Rec *)a, *rb = (const AcDs_Rec *)b;
  if (ra->schidx != rb->schidx)
    return ra->schidx < rb->schidx ? -1 : 1;
  if (ra->handle != rb->handle)
    return ra->handle < rb->handle ? -1 : 1;
  return 0;
}

/* The records to write: the SAB data of the ACIS entities, and the records
   read which do not belong to an ACIS entity we have, as the thumbnail. Sorted
   by schema and handle. Free the result. */
AcDs_Rec *
acds_collect_records (const Dwg_Data *restrict dwg, unsigned *restrict pnum)
{
  AcDs_Rec *rec;
  unsigned n = 0, i;
  const unsigned max = dwg->num_objects + dwg->acds.num_records;
  *pnum = 0;
  if (!max)
    return NULL;
  rec = (AcDs_Rec *)CALLOC (max, sizeof (AcDs_Rec));
  if (!rec)
    return NULL;
  for (i = 0; i < dwg->num_objects; i++)
    {
      const Dwg_Object *obj = &dwg->object[i];
      const unsigned char *data;
      BITCODE_BL size;
      if (obj->supertype == DWG_SUPERTYPE_ENTITY
          && dwg_obj_get_sab (obj, &data, &size))
        {
          rec[n].handle = obj->handle.value;
          rec[n].schidx = ACDS_SCHEMA_ASM_DATA;
          rec[n].data = data;
          rec[n].size = size;
          n++;
        }
    }
  for (i = 0; i < dwg->acds.num_records; i++)
    {
      const Dwg_AcDs_Record *r = &dwg->acds.records[i];
      unsigned j;
      if (!r->data || !r->size)
        continue;
      // the thumbnail has no object. Others only if still there, and not
      // already replaced by the SAB data of the entity
      if (r->schidx != ACDS_SCHEMA_THUMBNAIL
          && !dwg_resolve_handle_silent (dwg, r->handle))
        continue;
      for (j = 0; j < n; j++)
        if (rec[j].handle == r->handle)
          break;
      if (j < n)
        continue;
      rec[n].handle = r->handle;
      rec[n].schidx = r->schidx;
      rec[n].data = r->data;
      rec[n].size = r->size;
      n++;
    }
  if (!n)
    {
      FREE (rec);
      return NULL;
    }
  qsort (rec, n, sizeof (AcDs_Rec), rec_cmp);
  *pnum = n;
  return rec;
}
