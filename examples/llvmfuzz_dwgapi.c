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
 * llvmfuzz_dwgapi.c: libfuzzer target for the DWG API (add/get functions).
 * Reads a DWG/DXF/JSON file, then exercises the DWG API (add entities,
 * write back) to find bugs in the API and encoder layers.
 * written by Reini Urban
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include <dwg.h>
#include <dwg_api.h>
#include "decode.h"
#include "encode.h"

#ifndef DISABLE_JSON
#include "in_json.h"
#endif
#ifndef DISABLE_DXF
#include "in_dxf.h"
#endif
#ifndef DISABLE_DXF
extern int enforce_null_termination (Bit_Chain *dat, bool utf8);
#endif
extern int out;
extern int ver;
#define LLVMFUZZ_NUM_OUTPUTS 5

int
LLVMFuzzerTestOneInput (const unsigned char *data, size_t size)
{
  Dwg_Data dwg;
  Bit_Chain dat = { NULL, 0, 0, 0, 0, 0, 0, NULL, 0 };
  int copied = 0;

#ifndef STANDALONE
  out = 0;
  ver = 0;
  if (size)
    {
      unsigned int h = 2166136261u;
      const size_t n = size > 4096 ? 4096 : size;
      for (size_t i = 0; i < n; i++)
        h = (h ^ data[i]) * 16777619u;
      out = (int)(h % LLVMFUZZ_NUM_OUTPUTS);
      ver = (int)((h >> 8) % 20);
    }
#endif

  dat.chain = (unsigned char *)data;
  dat.size = size;
  memset (&dwg, 0, sizeof (dwg));

  if (size > 10 * 1024 * 1024)
    return 0;

  if (dat.size > 2 && dat.chain[0] == 'A' && dat.chain[1] == 'C')
    {
      if (dwg_decode (&dat, &dwg) >= DWG_ERR_CRITICAL)
        {
          dwg_free (&dwg);
          return 0;
        }
    }
#ifndef DISABLE_JSON
  else if (dat.size > 1 && dat.chain[0] == '{')
    {
      copied = enforce_null_termination (&dat, true);
      if (dwg_read_json (&dat, &dwg) >= DWG_ERR_CRITICAL)
        {
          if (copied)
            bit_chain_free (&dat);
          dwg_free (&dwg);
          return 0;
        }
      dat.opts |= DWG_OPTS_INJSON;
      dwg.opts |= DWG_OPTS_INJSON;
    }
#endif
#ifndef DISABLE_DXF
  else
    {
      copied = enforce_null_termination (&dat, false);
      if (dwg_read_dxf (&dat, &dwg) >= DWG_ERR_CRITICAL)
        {
          if (copied)
            bit_chain_free (&dat);
          dwg_free (&dwg);
          return 0;
        }
    }
#else
  else
    return 0;
#endif

  // Exercise the DWG API: add entities to the model space
  {
    Dwg_Object_Ref *mspace_ref = dwg_model_space_ref (&dwg);
    Dwg_Object *mspace = mspace_ref ? mspace_ref->obj : NULL;
    Dwg_Object_BLOCK_HEADER *mspace_hdr
        = mspace ? mspace->tio.object->tio.BLOCK_HEADER : NULL;
    dwg_point_3d pt = { 0.0, 0.0, 0.0 };
    dwg_point_3d pt2 = { 1.0, 1.0, 0.0 };
    if (mspace_hdr)
      {
        Dwg_Entity_LINE *line = dwg_add_LINE (mspace_hdr, &pt, &pt2);
        if (line)
          line->extrusion = (BITCODE_3DPOINT){ 0, 0, 1 };
        Dwg_Entity_CIRCLE *circle
            = dwg_add_CIRCLE (mspace_hdr, &pt, 1.0);
        if (circle)
          circle->extrusion = (BITCODE_3DPOINT){ 0, 0, 1 };
      }
  }

  // Write the DWG back to exercise the encoder
  {
    Bit_Chain out_dat = { NULL, 0, 0, 0, 0, 0, 0, NULL, 0 };
    bit_chain_set_version (&out_dat, &dat);
    if (dwg_encode (&dwg, &out_dat) >= 0)
      bit_chain_free (&out_dat);
  }

  if (copied)
    bit_chain_free (&dat);
  dwg_free (&dwg);
  return 0;
}

#ifdef STANDALONE
int
main (int argc, char **argv)
{
  if (argc > 1)
    {
      FILE *fp = fopen (argv[1], "rb");
      if (!fp)
        return 1;
      fseek (fp, 0, SEEK_END);
      size_t len = (size_t)ftell (fp);
      fseek (fp, 0, SEEK_SET);
      unsigned char *buf = (unsigned char *)malloc (len);
      if (!buf)
        {
          fclose (fp);
          return 1;
        }
      if (fread (buf, 1, len, fp) != len)
        {
          free (buf);
          fclose (fp);
          return 1;
        }
      fclose (fp);
      LLVMFuzzerTestOneInput (buf, len);
      free (buf);
    }
  return 0;
}
#endif
