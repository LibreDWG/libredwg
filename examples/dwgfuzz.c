/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2020 Free Software Foundation, Inc.                        */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * dwgfuzz.c: afl++ fuzzing for all in- and exporters. Just not the seperate ones.
 * written by Reini Urban
 */

#include "../src/config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <getopt.h>

#include <dwg.h>
#include "common.h"

#include "decode.h"
#include "encode.h"
#include "bits.h"
#include "in_json.h"
#include "out_dxf.h"
#include "out_json.h"
#include "in_dxf.h"

#define FUZZ_INMEM 0
#define FUZZ_STDIN 1
#define FUZZ_FILE 2

#define FUZZ_MODE FUZZ_INMEM
//#define FUZZ_MODE FUZZ_STDIN
//#define FUZZ_MODE FUZZ_FILE

#ifndef __AFL_COMPILER
#  define __AFL_FUZZ_INIT()
#  if FUZZ_MODE == FUZZ_INMEM
     unsigned char *__AFL_FUZZ_TESTCASE_BUF;
     unsigned long __AFL_FUZZ_TESTCASE_LEN;
#    define __AFL_INIT()                                                      \
      stat (argv[2], &attrib);                                                \
      fp = fopen (argv[2], "rb");                                             \
      if (!fp)                                                                \
        return 0;                                                             \
      dat.size = attrib.st_size;                                              \
      __AFL_FUZZ_TESTCASE_LEN = dat.size;                                     \
      dat_read_file (&dat, fp, argv[2]);                                      \
      __AFL_FUZZ_TESTCASE_BUF = dat.chain;
#  else
#    define __AFL_INIT()
#  endif
#  define __AFL_FUZZ_TESTCASE_LEN dat.size
#  define __AFL_LOOP(i) 1
#endif

__AFL_FUZZ_INIT ();

int
main (int argc, char *argv[])
{
  int i = 0;
  Dwg_Data dwg;
  Bit_Chain dat = { NULL, 0, 0, 0, 0 };
  Bit_Chain out_dat = { NULL, 0, 0, 0, 0 };
  FILE *fp;
  struct stat attrib;
  enum
  {
    INDXF,
    INJSON,
    RW,
    DWG,
    DXF,
    DXFB,
    JSON,
    GEOJSON,
  } mode;

  if (argc <= 1 || !*argv[1])
    return 1;
  if (strEQc (argv[1], "-indxf"))
    mode = INDXF;
  else if (strEQc (argv[1], "-injson"))
    mode = INJSON;
  else if (strEQc (argv[1], "-rw"))
    mode = RW;
  else if (strEQc (argv[1], "-dwg"))
    mode = DWG;
  else if (strEQc (argv[1], "-dxf"))
    mode = DXF;
  else if (strEQc (argv[1], "-dxfb"))
    mode = DXFB;
  else if (strEQc (argv[1], "-json"))
    mode = JSON;
  else if (strEQc (argv[1], "-geojson"))
    mode = GEOJSON;
  else
    return 1;
#if FUZZ_MODE == FUZZ_FILE
  if (argc <= 2 || !*argv[2])
    return 1;
#endif
#if FUZZ_MODE == FUZZ_INMEM && !defined(__AFL_COMPILER)
  if (argc <= 2 || !*argv[2])
    return 1;
#endif

  __AFL_INIT ();
  memset (&dwg, 0, sizeof (dwg));
  dat.chain = NULL;
  //dat.opts = 3;;
#if FUZZ_MODE == FUZZ_INMEM
  dat.chain = __AFL_FUZZ_TESTCASE_BUF;
#endif

  while (__AFL_LOOP (10000))
    { // llvm_mode persistent, non-forking mode
      i++;
#ifndef __AFL_COMPILER
      if (i > 3)
        break;
#endif
#if FUZZ_MODE == FUZZ_INMEM
      // fastest mode via shared mem
      dat.chain = __AFL_FUZZ_TESTCASE_BUF;
      dat.size = __AFL_FUZZ_TESTCASE_LEN;
      printf ("Fuzzing from shared memory (%lu)\n", dat.size);
#elif FUZZ_MODE == FUZZ_STDIN
      // still 10x faster than the old file-forking fuzzer.
      dat.size = 0;
      //dat.chain = NULL;
      dat_read_stream (&dat, stdin);
      printf ("Fuzzing from stdin (%lu)\n", dat.size);
#elif FUZZ_MODE == FUZZ_FILE
      stat (argv[2], &attrib);
      fp = fopen (argv[2], "rb");
      if (!fp)
        return 0;
      dat.size = attrib.st_size;
      dat_read_file (&dat, fp, argv[2]);
      fclose (fp);
      printf ("Fuzzing from file (%lu)\n", dat.size);
#endif
      if (dat.size == 0)
        exit (1);

      if (dat.size < 100)
        continue; // useful minimum input length
      switch (mode)
        {
        case INDXF:
          if (dwg_read_dxf (&dat, &dwg) < DWG_ERR_CRITICAL)
            {
              memset (&out_dat, 0, sizeof (out_dat));
              bit_chain_set_version (&out_dat, &dat);
              out_dat.version = R_2000;
              if (dwg_encode (&dwg, &out_dat) >= DWG_ERR_CRITICAL)
                exit (0);
              free (out_dat.chain);
            }
          break;
        case RW:
          if (dwg_decode (&dat, &dwg) < DWG_ERR_CRITICAL)
            {
              memset (&out_dat, 0, sizeof (out_dat));
              bit_chain_set_version (&out_dat, &dat);
              out_dat.version = R_2000;
              if (dwg_encode (&dwg, &out_dat) >= DWG_ERR_CRITICAL)
                exit (0);
              if (dwg_decode (&out_dat, &dwg) >= DWG_ERR_CRITICAL)
                exit (0);
              free (out_dat.chain);
            }
          break;
        case INJSON:
          if (dwg_read_json (&dat, &dwg) < DWG_ERR_CRITICAL)
            {
              memset (&out_dat, 0, sizeof (out_dat));
              bit_chain_set_version (&out_dat, &dat);
              out_dat.version = R_2000;
              if (dwg_encode (&dwg, &out_dat) >= DWG_ERR_CRITICAL)
                exit (0);
              free (out_dat.chain);
            }
          break;
        case DWG:
          if (dwg_decode (&dat, &dwg) >= DWG_ERR_CRITICAL)
            exit (0);
          break;
        case DXF:
          if (dwg_decode (&dat, &dwg) < DWG_ERR_CRITICAL)
            {
              memset (&out_dat, 0, sizeof (out_dat));
              bit_chain_set_version (&out_dat, &dat);
              if (dwg_write_dxf (&out_dat, &dwg) >= DWG_ERR_CRITICAL)
                exit (0);
              free (out_dat.chain);
            }
          break;
        case DXFB:
          if (dwg_decode (&dat, &dwg) < DWG_ERR_CRITICAL)
            {
              memset (&out_dat, 0, sizeof (out_dat));
              bit_chain_set_version (&out_dat, &dat);
              if (dwg_write_dxfb (&out_dat, &dwg) >= DWG_ERR_CRITICAL)
                exit (0);
              free (out_dat.chain);
            }
          break;
        case JSON:
          if (dwg_decode (&dat, &dwg) < DWG_ERR_CRITICAL)
            {
              memset (&out_dat, 0, sizeof (out_dat));
              bit_chain_set_version (&out_dat, &dat);
              if (dwg_write_json (&out_dat, &dwg) >= DWG_ERR_CRITICAL)
                exit (0);
              free (out_dat.chain);
            }
          break;
        case GEOJSON:
          if (dwg_decode (&dat, &dwg) < DWG_ERR_CRITICAL)
            {
              memset (&out_dat, 0, sizeof (out_dat));
              bit_chain_set_version (&out_dat, &dat);
              if (dwg_write_geojson (&out_dat, &dwg) >= DWG_ERR_CRITICAL)
                exit (0);
              free (out_dat.chain);
            }
          break;
        default:
          exit (1);
        }
    }
  dwg_free (&dwg);
  return 0;
}
