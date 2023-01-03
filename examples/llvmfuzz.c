/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2021 Free Software Foundation, Inc.                        */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * llvmfuzz.c: libfuzzer testing, esp. for oss-fuzz. with libfuzzer or
 * standalone written by Reini Urban
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
//#include <unistd.h>
#include <sys/stat.h>

#include <dwg.h>
#include "common.h"
#include "decode.h"
#include "encode.h"
#include "bits.h"
#ifndef DISABLE_DXF
#  include "out_dxf.h"
#  ifndef DISABLE_JSON
#    include "in_json.h"
#    include "out_json.h"
#  endif
#  include "in_dxf.h"
#endif

extern int LLVMFuzzerTestOneInput (const unsigned char *data, size_t size);

// libfuzzer limitation:
// Enforce NULL-termination of the input buffer, to avoid bogus reports. copy
// it. Problematic is mostly strtol(3) which also works with \n termination.
static int
enforce_null_termination (Bit_Chain *dat, bool enforce)
{
  unsigned char *copy;
  unsigned char c;
  if (!dat->size)
    return 0;
  c = dat->chain[dat->size - 1];
  // Allow \n termination without \0 in DXF? No, still crashes
  if (!enforce && ((c == '\n' && c + 1 == '\0') || c == '\0'))
    return 0;
#ifdef STANDALONE
  fprintf (stderr,
           "llvmfuzz_standalone: enforce libfuzzer buffer NULL termination\n");
#endif
  copy = malloc (dat->size + 1);
  memcpy (copy, dat->chain, dat->size);
  copy[dat->size] = '\0';
  dat->chain = copy;
  return 1;
}

int
LLVMFuzzerTestOneInput (const unsigned char *data, size_t size)
{
  Dwg_Data dwg;
  Bit_Chain dat = { NULL, 0, 0, 0, 0 };
  Bit_Chain out_dat = { NULL, 0, 0, 0, 0 };
  int copied = 0;
  struct ly_ctx *ctx = NULL;
  unsigned int possible_outputformats;
  int out;

  static char tmp_file[256];
  dat.chain = (unsigned char *)data;
  dat.size = size;
  memset (&dwg, 0, sizeof (dwg));

  possible_outputformats =
#ifdef DISABLE_DXF
#  ifdef DISABLE_JSON
      1;
#  else
      3;
#  endif
#else
      5;
#endif

  // Detect the input format: DWG, DXF or JSON
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

  memset (&out_dat, 0, sizeof (out_dat));
  bit_chain_set_version (&out_dat, &dat);
  if (copied)
    bit_chain_free (&dat);

#if 0
    snprintf (tmp_file, 255, "/tmp/llvmfuzzer%d.out", getpid());
    tmp_file[255] = '\0';
#elif defined _WIN32
  strcpy (tmp_file, "NUL");
#else
  strcpy (tmp_file, "/dev/null");
#endif
  out_dat.fh = fopen (tmp_file, "w");

  out = rand () % possible_outputformats;
#ifdef STANDALONE
  if (getenv ("OUT"))
    out = strtol (getenv ("OUT"), NULL, 10);
#endif
  switch (out)
    {
    case 0:
      {
        int ver = rand () % 6;
#ifdef STANDALONE
        if (getenv ("VER"))
          ver = strtol (getenv ("VER"), NULL, 10);
#endif
        switch (ver)
          {
          case 0:
            out_dat.version = dwg.header.version = R_13;
            break;
          case 1:
            out_dat.version = dwg.header.version = R_13c3;
            break;
          case 2:
            out_dat.version = dwg.header.version = R_14;
            break;
          case 3: // favor this one
          case 4:
          case 5:
          default:
            out_dat.version = dwg.header.version = R_2000;
            break;
          }
        dwg_encode (&dwg, &out_dat);
        free (out_dat.chain);
        break;
      }
#ifndef DISABLE_DXF
    case 1:
      dwg_write_dxf (&out_dat, &dwg);
      free (out_dat.chain);
      break;
    case 2: // experimental
      dwg_write_dxfb (&out_dat, &dwg);
      free (out_dat.chain);
      break;
#  ifndef DISABLE_JSON
    case 3:
      dwg_write_json (&out_dat, &dwg);
      free (out_dat.chain);
      break;
    case 4:
      dwg_write_geojson (&out_dat, &dwg);
      free (out_dat.chain);
      break;
#  endif
#endif
    default:
      break;
    }
  dwg_free (&dwg);
  fclose (out_dat.fh);
  // unlink (tmp_file);
  return 0;
}

#ifdef STANDALONE
/*
# ifdef __GNUC__
__attribute__((weak))
# endif
extern int LLVMFuzzerInitialize(int *argc, char ***argv);
*/

static int
usage (void)
{
  printf ("\nUsage: OUT=0 VER=3 llvmfuzz_standalone INPUT...");
  return 1;
}
// llvmfuzz_standalone reproducer, see OUT and VER env vars
int
main (int argc, char *argv[])
{
  if (argc <= 1 || !*argv[1])
    return usage ();
  /* works only on linux
  if (LLVMFuzzerInitialize)
    LLVMFuzzerInitialize (&argc, &argv);
  */
  for (int i = 1; i < argc; i++)
    {
      unsigned char *buf;
      FILE *f = fopen (argv[i], "rb");
      struct stat attrib;
      long len;
      size_t n_read;
      int fd;
      if (!f)
        {
          fprintf (stderr, "Illegal file argument %s\n", argv[i]);
          continue;
        }
      fd = fileno (f);
      if (fd < 0 || fstat (fd, &attrib)
          || !(S_ISREG (attrib.st_mode)
#  ifndef _WIN32
               || S_ISLNK (attrib.st_mode)
#  endif
                   ))
        {
          fprintf (stderr, "Illegal input file \"%s\"\n", argv[i]);
          continue;
        }
      // libFuzzer design bug, not zero-terminating its text buffer
      fseek (f, 0, SEEK_END);
      len = ftell (f);
      fseek (f, 0, SEEK_SET);
      if (len <= 0)
        continue;
      buf = (unsigned char *)malloc (len);
      n_read = fread (buf, 1, len, f);
      fclose (f);
      assert ((long)n_read == len);
      fprintf (stderr, "llvmfuzz_standalone %s [%zu]\n", argv[i], len);
      LLVMFuzzerTestOneInput (buf, len);
      free (buf);
      // Bit_Chain dat = { 0 };
      // dat_read_file (&dat, fp, argv[i]);
      // LLVMFuzzerTestOneInput (dat.chain, dat.size);
      // bit_free_chain (&dat);
    }
}
#endif
