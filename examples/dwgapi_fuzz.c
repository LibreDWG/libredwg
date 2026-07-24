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
 * dwgapi_fuzz.c: libfuzzer testing of the public dwg_api consumer surface.
 * llvmfuzz.c decodes and re-encodes a model but never reads it back through
 * the public API. This harness decodes a DWG in memory and then walks the
 * model the way a downstream application does -- iterating block headers and
 * their owned entities and calling the dwg_api.c getters -- which exercises the
 * accessor layer (dwg_api.c) that the encoder round-trip does not reach.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include <dwg.h>
#include <dwg_api.h>
#include "decode.h"
#include "bits.h"

static void
walk_entity (dwg_object *obj)
{
  int error;

  if (!obj)
    return;

  /* Common accessors, valid for every entity object. */
  (void)dwg_object_get_index (obj, &error);
  (void)dwg_object_get_type (obj);
  (void)dwg_object_get_dxfname (obj);

  /* A few representative typed getters; mirrors examples/dwg2svg2.c. */
  switch (dwg_object_get_fixedtype (obj))
    {
    case DWG_TYPE_LINE:
      {
        dwg_ent_line *line = dwg_object_to_LINE (obj);
        dwg_point_3d pt;
        if (line)
          {
            dwg_get_LINE (line, "start", &pt);
            dwg_get_LINE (line, "end", &pt);
          }
        break;
      }
    case DWG_TYPE_CIRCLE:
      {
        dwg_ent_circle *circle = dwg_object_to_CIRCLE (obj);
        dwg_point_3d center;
        double radius;
        if (circle)
          {
            dwg_get_CIRCLE (circle, "center", &center);
            dwg_get_CIRCLE (circle, "radius", &radius);
          }
        break;
      }
    case DWG_TYPE_ARC:
      {
        dwg_ent_arc *arc = dwg_object_to_ARC (obj);
        dwg_point_3d center;
        double radius;
        if (arc)
          {
            dwg_get_ARC (arc, "center", &center);
            dwg_get_ARC (arc, "radius", &radius);
          }
        break;
      }
    case DWG_TYPE_TEXT:
      {
        dwg_ent_text *text = dwg_object_to_TEXT (obj);
        dwg_point_2d ins_pt;
        if (text)
          dwg_get_TEXT (text, "ins_pt", &ins_pt);
        break;
      }
    default:
      break;
    }
}

int
LLVMFuzzerTestOneInput (const unsigned char *data, size_t size)
{
  Dwg_Data dwg;
  Bit_Chain dat = { NULL, 0, 0, 0, 0, 0, 0, NULL, 0 };
  dwg_obj_block_control *ctrl;
  dwg_object_ref **hdr_refs;
  unsigned int i, num_hdr_objs;
  int error;

  /* Decode binary DWG only; the DXF/JSON readers and the encoders are already
     covered by llvmfuzz, and the read-back walk below is additive regardless. */
  if (size <= 2 || data[0] != 'A' || data[1] != 'C')
    return 0;

  dat.chain = (unsigned char *)data;
  dat.size = size;
  memset (&dwg, 0, sizeof (dwg));

  if (dwg_decode (&dat, &dwg) >= DWG_ERR_CRITICAL)
    {
      dwg_free (&dwg);
      return 0;
    }

  /* Walk every block header and its owned entities through the public API. */
  ctrl = dwg_block_control (&dwg);
  if (ctrl)
    {
      hdr_refs = dwg_obj_block_control_get_block_headers (ctrl, &error);
      num_hdr_objs = dwg_obj_block_control_get_num_entries (ctrl, &error);
      if (!error && hdr_refs)
        {
          for (i = 0; i < num_hdr_objs; i++)
            {
              dwg_object *hdr = dwg_ref_get_object (hdr_refs[i], &error);
              dwg_object *obj;
              if (!hdr)
                continue;
              for (obj = get_first_owned_entity (hdr); obj;
                   obj = get_next_owned_entity (hdr, obj))
                walk_entity (obj);
            }
          free (hdr_refs);
        }
    }

  dwg_free (&dwg);
  return 0;
}

#ifdef STANDALONE
int
main (int argc, char *argv[])
{
  for (int i = 1; i < argc; i++)
    {
      unsigned char *buf;
      long len;
      size_t n_read;
      FILE *f = fopen (argv[i], "rb");
      if (!f)
        {
          fprintf (stderr, "Illegal file argument %s\n", argv[i]);
          continue;
        }
      fseek (f, 0, SEEK_END);
      len = ftell (f);
      fseek (f, 0, SEEK_SET);
      if (len <= 0)
        {
          fclose (f);
          continue;
        }
      buf = (unsigned char *)malloc (len);
      n_read = fread (buf, 1, len, f);
      fclose (f);
      if ((long)n_read == len)
        LLVMFuzzerTestOneInput (buf, len);
      free (buf);
    }
  return 0;
}
#endif
