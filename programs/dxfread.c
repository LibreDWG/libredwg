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
 * dxfread.c: read a DXF or JSON file, write it in the other format.
 * written by Reini Urban
 *
 * Usage: dxfread [-j|-d] [-o OUTFILE] [INFILE|-]
 *   -j  write JSON (default)
 *   -d  write DXF
 *   -o  output file (default: stdout)
 *   INFILE: DXF or JSON file (auto-detected); '-' for stdin
 */

#define _POSIX_C_SOURCE 200809L
#include "../src/config.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "dwg.h"
#include "common.h"
#include "bits.h"
#ifndef DISABLE_DXF
#  include "decode.h"
#  include "encode.h"
#  include "out_dxf.h"
#  ifndef DISABLE_JSON
#    include "out_json.h"
#    include "in_json.h"
#  endif
#  include "in_dxf.h"
#endif

static int
enforce_null_termination (Bit_Chain *dat, bool utf8)
{
  unsigned char *copy;
  unsigned char c;
  if (!dat->size)
    return 0;
  c = dat->chain[dat->size - 1];
  if (utf8)
    {
      if (c == '\0')
        return 0;
    }
  else
    {
      if (c == '\n' || c == '\0')
        return 0;
    }
  copy = malloc (dat->size + 1);
  memcpy (copy, dat->chain, dat->size);
  copy[dat->size] = '\0';
  dat->chain = copy;
  return 1;
}

static int
usage (void)
{
#ifndef DISABLE_DXF
  printf ("\nUsage: dxfread [-j|-d] [-o OUTFILE] [INFILE|-]\n");
#else
  printf ("\nUsage: dxfread [-j] [-o OUTFILE] [INFILE|-]\n");
#endif
  return 1;
}

int
main (int argc, char *argv[])
{
  int c, err, out_json = 1;
  Dwg_Data dwg;
  Bit_Chain dat = { NULL, 0, 0, 0, 0, 0, 0, NULL, 0 };
  int copied = 0;
  unsigned char *orig_buf = NULL;
  FILE *outf = stdout;
  char *outfile = NULL;

  memset (&dwg, 0, sizeof (dwg));

  while ((c = getopt (argc, argv, "jdo:")) > 0)
    {
      switch (c)
        {
        case 'j':
          out_json = 1;
          break;
        case 'd':
          out_json = 0;
          break;
        case 'o':
          outfile = optarg;
          break;
        default:
          return usage ();
        }
    }

  if (optind >= argc)
    return usage ();

  if (outfile)
    {
      outf = fopen (outfile, "wb");
      if (!outf)
        {
          fprintf (stderr, "Cannot open output file %s\n", outfile);
          return 1;
        }
    }

  // Read the input file
  if (strcmp (argv[optind], "-") == 0)
    {
      // Read from stdin
      size_t len = 0, cap = 65536, n;
      unsigned char *buf = (unsigned char *)malloc (cap);
      if (!buf)
        return 1;
      while ((n = fread (buf + len, 1, cap - len, stdin)) > 0)
        {
          len += n;
          if (len == cap)
            {
              cap *= 2;
              buf = (unsigned char *)realloc (buf, cap);
              if (!buf)
                return 1;
            }
        }
      dat.chain = buf;
      dat.size = len;
      orig_buf = buf;
      copied = 1;
    }
  else
    {
      size_t len;
      unsigned char *buf;
      FILE *in = fopen (argv[optind], "rb");
      if (!in)
        {
          fprintf (stderr, "Cannot open input file %s\n", argv[optind]);
          return 1;
        }
      fseek (in, 0, SEEK_END);
      len = (size_t)ftell (in);
      fseek (in, 0, SEEK_SET);
      buf = (unsigned char *)malloc (len);
      if (!buf)
        {
          fclose (in);
          return 1;
        }
      if (fread (buf, 1, len, in) != len)
        {
          free (buf);
          fclose (in);
          return 1;
        }
      fclose (in);
      dat.chain = buf;
      dat.size = len;
      orig_buf = buf;
      copied = 1;
    }

  // Detect input format: JSON starts with '{', DXF starts with "0\n"
  if (dat.size > 1 && dat.chain[0] == '{')
    {
#ifndef DISABLE_JSON
      copied = enforce_null_termination (&dat, true);
      err = dwg_read_json (&dat, &dwg);
      if (err >= DWG_ERR_CRITICAL)
        {
          fprintf (stderr, "Failed to read JSON input\n");
          if (dat.chain != orig_buf)
            free (dat.chain);
          free (orig_buf);
          dwg_free (&dwg);
          return 1;
        }
#else
      fprintf (stderr, "JSON not supported\n");
      if (dat.chain != orig_buf)
        free (dat.chain);
      free (orig_buf);
      return 1;
#endif
    }
  else
    {
#ifndef DISABLE_DXF
      copied = enforce_null_termination (&dat, false);
      err = dwg_read_dxf (&dat, &dwg);
      if (err >= DWG_ERR_CRITICAL)
        {
          fprintf (stderr, "Failed to read DXF input\n");
          if (dat.chain != orig_buf)
            free (dat.chain);
          free (orig_buf);
          dwg_free (&dwg);
          return 1;
        }
#else
      fprintf (stderr, "DXF not supported\n");
      if (dat.chain != orig_buf)
        free (dat.chain);
      free (orig_buf);
      return 1;
#endif
    }

  // Write the output directly to the file handle
  // Readers only guarantee from_version; JSON input leaves header.version
  // unset, fix it up like dxf2dwg.c does, else the writer emits $ACADVER
  // INVALI and skips the TABLES/BLOCKS/OBJECTS sections.
  if (dwg.header.version == R_INVALID)
    dwg.header.version = dwg.header.from_version;
  if (dwg.header.from_version == R_INVALID)
    dwg.header.from_version = dwg.header.version;
  dat.fh = outf;
  dat.version = dat.from_version = dwg.header.version;
  dat.codepage = dwg.header.codepage;
  dat.opts = 0;
  if (out_json)
    {
#ifndef DISABLE_JSON
      err = dwg_write_json (&dat, &dwg);
      if (err >= DWG_ERR_CRITICAL)
        {
          fprintf (stderr, "Failed to write JSON output\n");
          if (dat.chain != orig_buf)
            free (dat.chain);
          free (orig_buf);
          dwg_free (&dwg);
          if (outf != stdout)
            fclose (outf);
          return 1;
        }
#else
      fprintf (stderr, "JSON not supported\n");
      if (dat.chain != orig_buf)
        free (dat.chain);
      free (orig_buf);
      dwg_free (&dwg);
      if (outf != stdout)
        fclose (outf);
      return 1;
#endif
    }
  else
    {
#ifndef DISABLE_DXF
      err = dwg_write_dxf (&dat, &dwg);
      if (err >= DWG_ERR_CRITICAL)
        {
          fprintf (stderr, "Failed to write DXF output\n");
          if (dat.chain != orig_buf)
            free (dat.chain);
          free (orig_buf);
          dwg_free (&dwg);
          if (outf != stdout)
            fclose (outf);
          return 1;
        }
#else
      fprintf (stderr, "DXF not supported\n");
      if (dat.chain != orig_buf)
        free (dat.chain);
      free (orig_buf);
      dwg_free (&dwg);
      if (outf != stdout)
        fclose (outf);
      return 1;
#endif
    }

  if (dat.chain != orig_buf)
    free (dat.chain);
  free (orig_buf);
  dwg_free (&dwg);
  if (outf != stdout)
    fclose (outf);
  return 0;
}
