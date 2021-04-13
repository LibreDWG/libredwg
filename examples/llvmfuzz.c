/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2021, 2023 Free Software Foundation, Inc.                  */
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
// #include <unistd.h>
#include <sys/stat.h>

#include "common.h"
#include <dwg.h>
#ifdef HAVE_SYS_TIME_H
#  include <sys/time.h>
#endif
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

int out;
int ver;

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
  copy = MALLOC (dat->size + 1);
  memcpy (copy, dat->chain, dat->size);
  copy[dat->size] = '\0';
  dat->chain = copy;
  return 1;
}

int
LLVMFuzzerTestOneInput (const unsigned char *data, size_t size)
{
  Dwg_Data dwg;
  Bit_Chain dat = { NULL, 0, 0, 0, 0, 0, 0, NULL, 0 };
  Bit_Chain out_dat = { NULL, 0, 0, 0, 0, 0, 0, NULL, 0 };
  int copied = 0;
  struct ly_ctx *ctx = NULL;

  static char tmp_file[256];
#ifndef STANDALONE
  GC_INIT ();
#endif
  dat.chain = (unsigned char *)data;
  dat.size = size;
  memset (&dwg, 0, sizeof (dwg));

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

  switch (out)
    {
    case 0:
      {
        switch (ver)
          {
          // TODO support preR13, many downconverters still missing
          case 0:
            out_dat.version = dwg.header.version = R_1_4;
            break;
          case 1:
            out_dat.version = dwg.header.version = R_2_0;
            break;
          case 2:
            out_dat.version = dwg.header.version = R_2_10;
            break;
          case 3:
            out_dat.version = dwg.header.version = R_2_21;
            break;
          case 4:
            out_dat.version = dwg.header.version = R_2_4;
            break;
          case 5:
            out_dat.version = dwg.header.version = R_2_6;
            break;
          case 6:
            out_dat.version = dwg.header.version = R_9;
            break;
          case 7:
            out_dat.version = dwg.header.version = R_10;
            break;
          case 8:
            out_dat.version = dwg.header.version = R_11;
            break;
          case 9:
            out_dat.version = dwg.header.version = R_12;
            break;
          case 10:
            out_dat.version = dwg.header.version = R_13;
            break;
          case 11:
            out_dat.version = dwg.header.version = R_13c3;
            break;
          case 12:
            out_dat.version = dwg.header.version = R_14;
            break;
          case 13:
            out_dat.version = dwg.header.version = R_2004;
            break;
          default: // favor this one
            out_dat.version = dwg.header.version = R_2000;
            break;
          }
        dwg_encode (&dwg, &out_dat);
        break;
      }
#ifndef DISABLE_DXF
    case 1:
      dwg_write_dxf (&out_dat, &dwg);
      break;
    case 2: // experimental
      dwg_write_dxfb (&out_dat, &dwg);
      break;
#  ifndef DISABLE_JSON
    case 3:
      dwg_write_json (&out_dat, &dwg);
      break;
    case 4:
      dwg_write_geojson (&out_dat, &dwg);
      break;
#  endif
#endif
    default:
      break;
    }
  dwg_free (&dwg);
  FREE (out_dat.chain);
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
  unsigned seed;
  const unsigned int possible_outputformats =
#  ifdef DISABLE_DXF
#    ifdef DISABLE_JSON
      1;
#    else
      3;
#    endif
#  else
      5;
#  endif

  GC_INIT ();
  if (argc <= 1 || !*argv[1])
    return usage ();
  if (getenv ("SEED"))
    seed = (unsigned)strtol (getenv ("SEED"), NULL, 10) % 9999;
  else
    {
#  ifdef HAVE_GETTIMEOFDAY
      struct timeval tval;
      gettimeofday (&tval, NULL);
      seed = (unsigned)(tval.tv_sec * 1000 + tval.tv_usec) % 9999;
#  else
      seed = (unsigned)time (NULL) % 9999;
#  endif
    }
  srand (seed);
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
      buf = (unsigned char *)MALLOC (len);
      n_read = fread (buf, 1, len, f);
      fclose (f);
      assert ((long)n_read == len);

      out = rand () % possible_outputformats;
#  ifdef STANDALONE
      if (getenv ("OUT"))
        out = strtol (getenv ("OUT"), NULL, 10);
      // print SEED onlyu when needed (no env vars given)
      if (!(out || getenv ("VER")))
        fprintf (stderr, "SEED=%04u ", seed);
      fprintf (stderr, "OUT=%d ", out);
#  endif
      if (out == 0)
        {
          ver = rand () % 20;
#  ifdef STANDALONE
          if (getenv ("VER"))
            ver = strtol (getenv ("VER"), NULL, 10);
          fprintf (stderr, "VER=%d ", ver);
#  endif
        }
      fprintf (stderr, "examples/llvmfuzz_standalone %s [%" PRIuSIZE "]\n",
               argv[i], len);
      LLVMFuzzerTestOneInput (buf, len);
      FREE (buf);
      // Bit_Chain dat = { 0 };
      // dat_read_file (&dat, fp, argv[i]);
      // LLVMFuzzerTestOneInput (dat.chain, dat.size);
      // bit_free_chain (&dat);
    }
}
#endif
