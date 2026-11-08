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
 * encode_acds.c: write the r2013+ AcDb:AcDsPrototype_1b datastorage section,
 * which keeps the SAB data of the ACIS entities (3DSOLID, REGION, BODY,
 * surfaces), see ODA chapter 24 and acds.spec for the reader.
 *
 * The section is written from the entities (acis_data), not from the
 * Dwg_AcDs structure read: one segidx, datidx, _data_, schidx, schdat and
 * search segment, as AutoCAD writes it in a fresh save, with one blob01
 * segment per page of large data. The schema segments are constant.
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
#include "encode.h"
#include "acds.h"

#define DWG_LOGLEVEL loglevel
#include "logging.h"

#define SEG_HDR_SIZE 48
#define SEG_ALIGN 0x80
// larger data is stored in a page of a blob01 segment, ODA writes one page
// per data record
#define DIRECT_MAX 0x40000
#define BLOB_REF_HDR_SIZE 32

/* The constant schema index (24.2.2.5) and schema data (24.2.2.6) payloads, as
   AutoCAD writes them in a fresh save (example_2018.dwg): the 6 schemas
   AcDb_Thumbnail_Schema, AcDb3DSolid_ASM_Data and the 4 built-in ones, with
   their properties and names. The schidx has the segment index of the
   schdat 5, patched on use. */
// clang-format off
static const unsigned char acds_schidx_payload[354] = {
  0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x05, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
  0x05, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
  0x05, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
  0x05, 0x00, 0x00, 0x00, 0xd2, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
  0x05, 0x00, 0x00, 0x00, 0xe4, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00,
  0x05, 0x00, 0x00, 0x00, 0xf6, 0x00, 0x00, 0x00, 0x0c, 0xf1, 0x0a, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
  0x05, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
  0x05, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
  0x05, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00,
  0x05, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
  0x05, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
  0x05, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
  0x05, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00,
  0x06, 0x00, 0x00, 0x00, 0x41, 0x63, 0x44, 0x62, 0x5f, 0x54, 0x68, 0x75,
  0x6d, 0x62, 0x6e, 0x61, 0x69, 0x6c, 0x5f, 0x53, 0x63, 0x68, 0x65, 0x6d,
  0x61, 0x00, 0x41, 0x63, 0x44, 0x62, 0x33, 0x44, 0x53, 0x6f, 0x6c, 0x69,
  0x64, 0x5f, 0x41, 0x53, 0x4d, 0x5f, 0x44, 0x61, 0x74, 0x61, 0x00, 0x41,
  0x63, 0x44, 0x62, 0x44, 0x73, 0x3a, 0x3a, 0x54, 0x72, 0x65, 0x61, 0x74,
  0x65, 0x64, 0x41, 0x73, 0x4f, 0x62, 0x6a, 0x65, 0x63, 0x74, 0x44, 0x61,
  0x74, 0x61, 0x53, 0x63, 0x68, 0x65, 0x6d, 0x61, 0x00, 0x41, 0x63, 0x44,
  0x62, 0x44, 0x73, 0x3a, 0x3a, 0x4c, 0x65, 0x67, 0x61, 0x63, 0x79, 0x53,
  0x63, 0x68, 0x65, 0x6d, 0x61, 0x00, 0x41, 0x63, 0x44, 0x62, 0x44, 0x73,
  0x3a, 0x3a, 0x49, 0x6e, 0x64, 0x65, 0x78, 0x65, 0x64, 0x50, 0x72, 0x6f,
  0x70, 0x65, 0x72, 0x74, 0x79, 0x53, 0x63, 0x68, 0x65, 0x6d, 0x61, 0x00,
  0x41, 0x63, 0x44, 0x62, 0x44, 0x73, 0x3a, 0x3a, 0x48, 0x61, 0x6e, 0x64,
  0x6c, 0x65, 0x41, 0x74, 0x74, 0x72, 0x69, 0x62, 0x75, 0x74, 0x65, 0x53,
  0x63, 0x68, 0x65, 0x6d, 0x61, 0x00,
};
static const unsigned char acds_schdat_payload[404] = {
  0x08, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00,
  0x01, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
  0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00,
  0x01, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
  0x08, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00,
  0x02, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00,
  0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
  0x0a, 0x00, 0x00, 0x00, 0x02, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00,
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x08, 0x00,
  0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x01, 0x00,
  0x00, 0x00, 0x01, 0x00, 0x00, 0x73, 0x73, 0x73, 0x08, 0x00, 0x00, 0x00,
  0x41, 0x63, 0x44, 0x62, 0x44, 0x73, 0x3a, 0x3a, 0x49, 0x44, 0x00, 0x54,
  0x68, 0x75, 0x6d, 0x62, 0x6e, 0x61, 0x69, 0x6c, 0x5f, 0x44, 0x61, 0x74,
  0x61, 0x00, 0x41, 0x63, 0x44, 0x62, 0x44, 0x73, 0x3a, 0x3a, 0x49, 0x44,
  0x00, 0x41, 0x53, 0x4d, 0x5f, 0x44, 0x61, 0x74, 0x61, 0x00, 0x41, 0x63,
  0x44, 0x62, 0x44, 0x73, 0x3a, 0x3a, 0x54, 0x72, 0x65, 0x61, 0x74, 0x65,
  0x64, 0x41, 0x73, 0x4f, 0x62, 0x6a, 0x65, 0x63, 0x74, 0x44, 0x61, 0x74,
  0x61, 0x00, 0x41, 0x63, 0x44, 0x62, 0x44, 0x73, 0x3a, 0x3a, 0x4c, 0x65,
  0x67, 0x61, 0x63, 0x79, 0x00, 0x41, 0x63, 0x44, 0x73, 0x3a, 0x49, 0x6e,
  0x64, 0x65, 0x78, 0x61, 0x62, 0x6c, 0x65, 0x00, 0x41, 0x63, 0x44, 0x62,
  0x44, 0x73, 0x3a, 0x3a, 0x48, 0x61, 0x6e, 0x64, 0x6c, 0x65, 0x41, 0x74,
  0x74, 0x72, 0x69, 0x62, 0x75, 0x74, 0x65, 0x00,
};
// clang-format on

typedef struct
{
  unsigned char *p;
  size_t len, cap;
  int oom;
} Buf;

static void
buf_put (Buf *restrict b, const void *restrict src, const size_t n)
{
  if (b->oom)
    return;
  if (b->len + n > b->cap)
    {
      size_t cap = b->cap ? b->cap : 4096;
      unsigned char *p;
      while (cap < b->len + n)
        cap *= 2;
      p = (unsigned char *)realloc (b->p, cap);
      if (!p)
        {
          b->oom = 1;
          return;
        }
      b->p = p;
      b->cap = cap;
    }
  memcpy (&b->p[b->len], src, n);
  b->len += n;
}

static void
buf_fill (Buf *restrict b, const unsigned char c, size_t n)
{
  unsigned char tmp[64];
  memset (tmp, c, sizeof tmp);
  while (n)
    {
      const size_t l = n < sizeof tmp ? n : sizeof tmp;
      buf_put (b, tmp, l);
      n -= l;
    }
}

static void
put16 (Buf *restrict b, const uint16_t v)
{
  const unsigned char t[2] = { v & 0xff, (v >> 8) & 0xff };
  buf_put (b, t, 2);
}

static void
put32 (Buf *restrict b, const uint32_t v)
{
  const unsigned char t[4]
      = { v & 0xff, (v >> 8) & 0xff, (v >> 16) & 0xff, (v >> 24) & 0xff };
  buf_put (b, t, 4);
}

static void
put64 (Buf *restrict b, const uint64_t v)
{
  put32 (b, (uint32_t)(v & 0xffffffff));
  put32 (b, (uint32_t)(v >> 32));
}

static void
put32_at (Buf *restrict b, const size_t pos, const uint32_t v)
{
  if (!b->oom && pos + 4 <= b->len)
    {
      b->p[pos] = v & 0xff;
      b->p[pos + 1] = (v >> 8) & 0xff;
      b->p[pos + 2] = (v >> 16) & 0xff;
      b->p[pos + 3] = (v >> 24) & 0xff;
    }
}

static size_t
seg_round (const size_t n)
{
  return (n + SEG_ALIGN - 1) & ~((size_t)SEG_ALIGN - 1);
}

/* 24.2.1 the segment header, 48 bytes */
static void
put_seg_hdr (Buf *restrict b, const char *restrict name, const unsigned idx,
             const unsigned is_blob01, const size_t segsize,
             const unsigned sys_algn, const unsigned obj_algn)
{
  put16 (b, 0xd5ac);
  buf_put (b, name, 6);
  put32 (b, idx);
  put32 (b, is_blob01);
  put32 (b, (uint32_t)segsize);
  put32 (b, 0);
  put32 (b, 1); // data storage revision
  put32 (b, 0);
  put32 (b, sys_algn);
  put32 (b, obj_algn);
  buf_fill (b, 0x55, 8);
}

/* 24.2.2.7 the search segment: for each of the 2 schemas the sorted index
   and the handle to index lookup of the records. */
static void
put_search (Buf *restrict b, const AcDs_Rec *restrict rec, const unsigned num)
{
  unsigned s, i, idx = 0;
  put32 (b, 2); // number of schemas
  for (s = 0; s < 2; s++)
    {
      unsigned k = 0, first = idx;
      for (i = 0; i < num; i++)
        if (rec[i].schidx == s)
          k++;
      put32 (b, s); // schema name index
      put64 (b, k);
      for (i = 0; i < k; i++)
        put64 (b, first + i);
      // the one set of ID indexes, also if empty, as ODA writes it
      put32 (b, 1);
      put32 (b, 0);
      put32 (b, k);
      for (i = 0; i < num; i++)
        if (rec[i].schidx == s)
          {
            put64 (b, rec[i].handle);
            put64 (b, 1);
            put64 (b, idx++);
          }
    }
}

/* A segment: its bytes, padded to the aligned size, with the header */
typedef struct
{
  Buf b;
  unsigned idx;
  size_t off;
} Seg;

/* Build a segment from the payload (24.2.1). hdr_size is the size the header
   claims, the aligned size for all but the blob01 segments, where ODA writes
   only the size of the header. */
static void
seg_build (Seg *restrict seg, const char *restrict name, const unsigned idx,
           const unsigned is_blob01, const unsigned sys_algn,
           const unsigned obj_algn, const Buf *restrict payload,
           const size_t hdr_size)
{
  const size_t size = seg_round (SEG_HDR_SIZE + payload->len);
  seg->idx = idx;
  put_seg_hdr (&seg->b, name, idx, is_blob01, hdr_size ? hdr_size : size,
               sys_algn, obj_algn);
  buf_put (&seg->b, payload->p, payload->len);
  buf_fill (&seg->b, 0x70, size - seg->b.len);
}

/* Write the AcDs section from the ACIS entities into dat, which then owns the
   malloc'ed bytes. No section (dat->byte == 0) if there is no data.

   Without large data the segments are in the order AutoCAD writes them, with
   large data (blob01 pages) as ODA, with the blob01 segments first and the
   segidx last. */
int
encode_acds (Dwg_Data *restrict dwg, Bit_Chain *restrict dat)
{
  unsigned num, i, num_blobs = 0, nseg, blob_idx;
  unsigned seg_data, seg_datidx, seg_schidx, seg_schdat, seg_search;
  AcDs_Rec *rec;
  Seg *blob = NULL;
  Seg s_data = { { 0 }, 0, 0 }, s_datidx = { { 0 }, 0, 0 },
      s_schidx = { { 0 }, 0, 0 }, s_schdat = { { 0 }, 0, 0 },
      s_search = { { 0 }, 0, 0 }, s_segidx = { { 0 }, 1, 0 };
  Seg **order = NULL;
  Buf out = { 0 }, pay = { 0 }, data = { 0 }, hdrs = { 0 };
  size_t off, data_start;
  unsigned obj_algn, n = 0;
  int error = 0;

  rec = acds_collect_records (dwg, &num);
  if (!rec)
    return 0;
  for (i = 0; i < num; i++)
    if (rec[i].size > DIRECT_MAX)
      num_blobs++;
  blob = (Seg *)calloc (num_blobs + 1, sizeof (Seg));
  order = (Seg **)calloc (num_blobs + 8, sizeof (Seg *));
  if (!blob || !order)
    {
      error = DWG_ERR_OUTOFMEM;
      goto done;
    }

  /* the segment indices and the order in the file */
  if (!num_blobs)
    {
      seg_datidx = 2;
      seg_data = 3;
      seg_schidx = 4;
      seg_schdat = 5;
      seg_search = 6;
      order[n++] = &s_segidx;
      order[n++] = &s_datidx;
      order[n++] = &s_data;
      order[n++] = &s_schidx;
      order[n++] = &s_schdat;
      order[n++] = &s_search;
    }
  else
    {
      seg_data = 2 + num_blobs;
      seg_datidx = seg_data + 1;
      seg_schdat = seg_datidx + 1;
      seg_schidx = seg_schdat + 1;
      seg_search = seg_schidx + 1;
      for (i = 0; i < num_blobs; i++)
        order[n++] = &blob[i];
      order[n++] = &s_data;
      order[n++] = &s_datidx;
      order[n++] = &s_schdat;
      order[n++] = &s_schidx;
      order[n++] = &s_search;
      order[n++] = &s_segidx;
    }
  nseg = n + 1; // with the empty segment 0

  /* The data records, in the _data_ segment: UInt32 size and the bytes.
     Large data is a blob reference record to its page in a blob01 segment. */
  {
    const size_t hdr_end = SEG_HDR_SIZE + (size_t)num * 20;
    obj_algn = (unsigned)((hdr_end + 15) / 16);
    data_start = (size_t)obj_algn * 16;
  }
  blob_idx = 0;
  for (i = 0; i < num; i++)
    {
      const size_t local = data.len;
      const AcDs_Rec *r = &rec[i];
      put32 (&hdrs, 20);
      put32 (&hdrs, 1);
      put64 (&hdrs, r->handle);
      put32 (&hdrs, (uint32_t)local);
      if (r->size > DIRECT_MAX)
        {
          const unsigned idx = 2 + blob_idx;
          // 24.2.2.3.1 Data blob reference record
          put32 (&data, 0xBB106BB1U); // instead of the size
          put64 (&data, r->size);
          put32 (&data, 1); // one page
          put32 (&data, BLOB_REF_HDR_SIZE + 8);
          put32 (&data, (uint32_t)r->size);
          put32 (&data, (uint32_t)r->size);
          put32 (&data, 0);
          put32 (&data, 0);
          put32 (&data, idx);
          put32 (&data, (uint32_t)r->size);
          // 24.2.2.4 Blob01 file segment, with one page
          put64 (&pay, r->size);
          put64 (&pay, 0);
          put32 (&pay, 0);
          put32 (&pay, 1);
          put64 (&pay, r->size);
          buf_put (&pay, r->data, r->size);
          seg_build (&blob[blob_idx], "blob01", idx, 1, 0, 0, &pay,
                     SEG_HDR_SIZE);
          pay.len = 0;
          blob_idx++;
        }
      else
        {
          put32 (&data, (uint32_t)r->size);
          buf_put (&data, r->data, r->size);
        }
    }

  /* _data_ */
  buf_put (&pay, hdrs.p, hdrs.len);
  buf_fill (&pay, 0x62, data_start - SEG_HDR_SIZE - pay.len);
  buf_put (&pay, data.p, data.len);
  seg_build (&s_data, "_data_", seg_data, 0, 0, obj_algn, &pay, 0);
  pay.len = 0;
  /* datidx */
  put32 (&pay, num);
  put32 (&pay, 0);
  for (i = 0; i < num; i++)
    {
      put32 (&pay, seg_data);
      put32 (&pay, i * 20);
      put32 (&pay, rec[i].schidx);
    }
  seg_build (&s_datidx, "datidx", seg_datidx, 0, 0, 0, &pay, 0);
  pay.len = 0;
  /* schidx and schdat, constant. The schidx has the segment index of the
     schdat: 6 properties of 12 bytes from 8, with the segment index at +4, and
     8 property entries of 12 bytes from 96, with the segment index first. */
  buf_put (&pay, acds_schidx_payload, sizeof (acds_schidx_payload));
  for (i = 0; i < 6; i++)
    put32_at (&pay, 12 + 12 * i, seg_schdat);
  for (i = 0; i < 8; i++)
    put32_at (&pay, 96 + 12 * i, seg_schdat);
  seg_build (&s_schidx, "schidx", seg_schidx, 0, 15, 0, &pay, 0);
  pay.len = 0;
  buf_put (&pay, acds_schdat_payload, sizeof (acds_schdat_payload));
  seg_build (&s_schdat, "schdat", seg_schdat, 0, 20, 0, &pay, 0);
  pay.len = 0;
  /* search */
  put_search (&pay, rec, num);
  seg_build (&s_search, "search", seg_search, 0, 0, 0, &pay, 0);
  pay.len = 0;
  /* segidx, with the offsets of all segments. AutoCAD writes one more, empty
     entry than the segment index count */
  off = SEG_ALIGN;
  for (i = 0; i < n; i++)
    {
      order[i]->off = off;
      if (order[i] == &s_segidx)
        {
          // the size of the segidx itself
          off += seg_round (SEG_HDR_SIZE + ((size_t)nseg + 1) * 12);
        }
      else
        off += order[i]->b.len;
    }
  for (i = 0; i <= nseg; i++)
    {
      size_t o = 0, sz = 0;
      unsigned j;
      for (j = 0; j < n; j++)
        if (order[j]->idx == i && i)
          {
            o = order[j]->off;
            sz = order[j] == &s_segidx
                     ? seg_round (SEG_HDR_SIZE + ((size_t)nseg + 1) * 12)
                     : order[j]->b.len;
            break;
          }
      put64 (&pay, o);
      put32 (&pay, (uint32_t)sz);
    }
  seg_build (&s_segidx, "segidx", 1, 0, 0, 0, &pay, 0);
  pay.len = 0;

  /* the file header */
  put32 (&out, 0x6472616a); // "jard"
  put32 (&out, SEG_ALIGN);
  put32 (&out, 8);
  put32 (&out, 2);
  put32 (&out, 0);
  put32 (&out, 1);
  put32 (&out, (uint32_t)s_segidx.off);
  put32 (&out, 0);
  put32 (&out, nseg);
  put32 (&out, seg_schidx);
  put32 (&out, seg_datidx);
  put32 (&out, seg_search);
  put32 (&out, 0);
  put32 (&out, (uint32_t)off); // the file size
  buf_fill (&out, 0, SEG_ALIGN - out.len);
  for (i = 0; i < n; i++)
    buf_put (&out, order[i]->b.p, order[i]->b.len);

  if (out.oom || data.oom || hdrs.oom || pay.oom)
    {
      error = DWG_ERR_OUTOFMEM;
      goto done;
    }
  LOG_TRACE ("AcDs: %u data records, %u blob pages, %" PRIuSIZE " bytes\n",
             num, num_blobs, out.len);
  free (dat->chain);
  dat->chain = out.p;
  dat->size = dat->byte = out.len;
  dat->bit = 0;
  out.p = NULL;
done:
  free (out.p);
  free (pay.p);
  free (data.p);
  free (hdrs.p);
  free (s_data.b.p);
  free (s_datidx.b.p);
  free (s_schidx.b.p);
  free (s_schdat.b.p);
  free (s_search.b.p);
  free (s_segidx.b.p);
  for (i = 0; blob && i < num_blobs; i++)
    free (blob[i].b.p);
  free (blob);
  free (order);
  free (rec);
  return error;
}
