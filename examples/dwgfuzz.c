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
 * dwgfuzz.c: afl++/honggfuzz fuzzing for all in- and exporters. Just not the
 separate ones.
 *
 * Also usable like:
    ../configure --disable-shared --disable-bindings CC=hfuzz-clang CFLAGS='-O2
 -g -fsanitize=address,undefined -fno-omit-frame-pointer -I/usr/local/include'
 && make -C src && make -C examples dwgfuzz honggfuzz -i ../.fuzz-in-dxf --
 examples/dwgfuzz -indxf ___FILE___

 * Also useful for debugging the fuzzers.
    AFL_DONT_OPTIMIZE=1 AFL_LLVM_INSTRIM=1 AFL_USE_ASAN=1 make -C examples
 dwgfuzz V=1 AFL_DEBUG=15 AFL_DEBUG_CHILD_OUTPUT=1 gdb --args afl-fuzz -m none
 -i ../.fuzz-in-dxf/ -o .fuzz-out/ examples/dwgfuzz (gdb) set follow-fork-mode
 child
 * written by Reini Urban
 */

#include "../src/config.h"
#ifdef HAVE_SSCANF_S
#  define __STDC_WANT_LIB_EXT1__ 1
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <getopt.h>

#include <dwg.h>
#include <dwg_api.h>
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

#define FUZZ_INMEM 0
#define FUZZ_STDIN 1
#define FUZZ_FILE 2

// defined by ./make-afl-clang-fast.sh for INMEM. defaults to FILE for
// debugging
#ifndef FUZZ_MODE
#  define FUZZ_MODE FUZZ_INMEM
// #define FUZZ_MODE FUZZ_STDIN
// #define FUZZ_MODE FUZZ_FILE
#endif

int dwg_fuzz_dat (Dwg_Data **restrict dwgp, Bit_Chain *restrict dat);

static int
version (void)
{
  printf ("dwgfuzz %s INMEM\n", PACKAGE_VERSION);
#ifndef __AFL_COMPILER
  printf ("not instrumented\n");
#else
#  ifdef __AFL_FUZZ_TESTCASE_BUF
  printf ("shared-memory instrumented\n");
#  else
  printf ("instrumented\n");
#  endif
#endif
  return 0;
}

#ifndef __AFL_COMPILER
#  define __AFL_FUZZ_INIT()
#  if FUZZ_MODE == FUZZ_INMEM
unsigned char *__AFL_FUZZ_TESTCASE_BUF;
unsigned long __AFL_FUZZ_TESTCASE_LEN;
#    define __AFL_INIT()                                                      \
      fp = fopen (argv[2], "rb");                                             \
      if (!fp)                                                                \
        return 0;                                                             \
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

static int
help (void)
{
#if FUZZ_MODE == FUZZ_FILE
  printf ("\nUsage: dwgfuzz MODE @@\n");
#elif !defined __AFL_COMPILER
  printf ("\nUsage: dwgfuzz MODE ../.fuzz-in/INFILE\n");
#else
  printf ("\nUsage: dwgfuzz MODE\n");
#endif
  printf ("afl++ clang-fast shared-memory backend, using many importers and "
          "exporters.\n"
          "\n");
  printf ("MODE:\n");
#ifdef USE_WRITE
#  ifndef DISABLE_DXF
  printf ("  -indxf:   import from DXF,  export as r2000 DWG\n");
#  endif
#  ifndef DISABLE_JSON
  printf ("  -injson:  import from JSON, export as r2000 DWG\n");
#  endif
  printf ("  -rw:      import from DWG,  export as r2000 DWG, re-import from "
          "this DWG (rewrite)\n");
  printf ("  -add:     import from special add file, export as DWG and DXF, "
          "re-import from this\n");
#endif
  printf ("  -dwg:     import from DWG only\n");
#ifndef DISABLE_DXF
  printf ("  -dxf:     import from DWG,  export as DXF\n");
  printf ("  -dxfb:    import from DWG,  export as binary DXF\n");
#endif
#ifndef DISABLE_JSON
  printf ("  -json:    import from DWG,  export as JSON\n");
  printf ("  -geojson: import from DWG,  export as GeoJSON\n");
#endif
  printf ("\n"
          " --version        display the version and exit\n");
  printf (" --help           display this help and exit\n");
  return 0;
}

// for LLVMFuzzerTestOneInput see llvmfuzz.c

int
main (int argc, char *argv[])
{
  int i = 0;
  Dwg_Data dwg;
  Bit_Chain dat = { 0 };
  Bit_Chain out_dat = { 0 };
  FILE *fp;
  struct stat attrib;
  enum
  {
    INVALID,
    DWG,
#if defined(USE_WRITE) && !defined(DISABLE_DXF)
    INDXF,
#endif
#if defined(USE_WRITE) && !defined(DISABLE_JSON)
    INJSON,
#endif
#if defined(USE_WRITE)
    RW,
    ADD,
#endif
#if !defined(DISABLE_DXF)
    DXF,
    DXFB,
#endif
#if !defined(DISABLE_JSON)
    JSON,
    GEOJSON,
#endif
  } mode
      = INVALID;
  __AFL_INIT ();

  GC_INIT ();
  if (argc <= 1 || !*argv[1])
    return 1;
  if (strEQc (argv[1], "-dwg"))
    mode = DWG;
#ifdef USE_WRITE
#  ifndef DISABLE_DXF
  else if (strEQc (argv[1], "-indxf"))
    mode = INDXF;
#  endif
#  ifndef DISABLE_JSON
  else if (strEQc (argv[1], "-injson"))
    mode = INJSON;
#  endif
  else if (strEQc (argv[1], "-rw"))
    mode = RW;
  else if (strEQc (argv[1], "-add"))
    mode = ADD;
#endif /* USE_WRITE */
#ifndef DISABLE_DXF
  else if (strEQc (argv[1], "-dxf"))
    mode = DXF;
  else if (strEQc (argv[1], "-dxfb"))
    mode = DXFB;
#endif
#ifndef DISABLE_JSON
  else if (strEQc (argv[1], "-json"))
    mode = JSON;
  else if (strEQc (argv[1], "-geojson"))
    mode = GEOJSON;
#endif
  else if (strEQc (argv[1], "--version"))
    return version ();
  else if (strEQc (argv[1], "--help"))
    return help ();
  else
    return 1;
  if (mode == INVALID)
    return 1;
#if FUZZ_MODE == FUZZ_FILE
  if (argc <= 2 || !*argv[2])
    return 1;
#endif
#if FUZZ_MODE == FUZZ_INMEM && !defined(__AFL_COMPILER)
  if (argc <= 2 || !*argv[2])
    return 1;
#endif

  memset (&dwg, 0, sizeof (dwg));
  dat.chain = NULL;
  // dat.opts = 3;;
#if FUZZ_MODE == FUZZ_INMEM
  dat.chain = __AFL_FUZZ_TESTCASE_BUF;
#endif

  while (__AFL_LOOP (10000))
    { // llvm_mode persistent, non-forking mode
      i++;
#ifndef __AFL_COMPILER
#  ifdef USE_WRITE
      if (mode == ADD && i > 1)
        break;
#  endif
      if (i > 3)
        break;
#endif
#if FUZZ_MODE == FUZZ_INMEM
      // fastest mode via shared mem
      if (!dat.chain)
        dat.chain = __AFL_FUZZ_TESTCASE_BUF;
      dat.size = __AFL_FUZZ_TESTCASE_LEN;
      printf ("Fuzzing from shared memory (%" PRIuSIZE ")\n", dat.size);
#elif FUZZ_MODE == FUZZ_STDIN
      // still 10x faster than the old file-forking fuzzer.
      dat.size = 0;
      // dat.chain = NULL;
      dat_read_stream (&dat, stdin);
      printf ("Fuzzing from stdin (%" PRIuSIZE ")\n", dat.size);
#elif FUZZ_MODE == FUZZ_FILE
      dat.size = 0;
      fp = fopen (argv[2], "rb");
      if (!fp)
        return 0;
      dat_read_file (&dat, fp, argv[2]);
      fclose (fp);
      printf ("Fuzzing from file (%" PRIuSIZE ")\n", dat.size);
#else
#  error Missing FUZZ_MODE
#endif
      if (dat.size == 0)
        exit (1);
      if (dat.size < 100)
        continue; // useful minimum input length
      memset (&out_dat, 0, sizeof (out_dat));
      bit_chain_set_version (&out_dat, &dat);
#ifdef _WIN32
      out_dat.fh = fopen ("NUL", "w");
#else
      out_dat.fh = fopen ("/dev/null", "w");
#endif

      switch (mode)
        {
        case DWG:
          if (dwg_decode (&dat, &dwg) >= DWG_ERR_CRITICAL)
            exit (0);
          break;
#if defined(USE_WRITE) && !defined(DISABLE_DXF)
        case INDXF:
          if (dwg_read_dxf (&dat, &dwg) < DWG_ERR_CRITICAL)
            {
              out_dat.version = R_2000;
              if (dwg_encode (&dwg, &out_dat) >= DWG_ERR_CRITICAL)
                exit (0);
              FREE (out_dat.chain);
            }
          break;
#endif
#if defined(USE_WRITE)
        case RW:
          if (dwg_decode (&dat, &dwg) < DWG_ERR_CRITICAL)
            {
              out_dat.version = R_2000;
              if (dwg_encode (&dwg, &out_dat) >= DWG_ERR_CRITICAL)
                exit (0);
              if (dwg_decode (&out_dat, &dwg) >= DWG_ERR_CRITICAL)
                exit (0);
              FREE (out_dat.chain);
            }
          break;
        case ADD:
          {
            Dwg_Data *dwgp = &dwg;
            if (dwg_fuzz_dat (&dwgp, &dat) == 0)
              {
                int error;
                out_dat.byte = 0;
                out_dat.bit = 0;
                out_dat.from_version = dwg.header.from_version;
                out_dat.version = dwg.header.version;
                out_dat.opts = dwg.opts;
                error = dwg_encode (&dwg, &out_dat); // dwg -> out_dat
                if (error >= DWG_ERR_CRITICAL)
                  exit (0);

                out_dat.byte = 0;
                out_dat.bit = 0;
                memset (&dwg, 0, sizeof (Dwg_Data));
                error = dwg_decode (&out_dat, &dwg); // out_dat -> dwg
                if (error >= DWG_ERR_CRITICAL)
                  exit (0);
              }
          }
          break;
#endif
#if defined(USE_WRITE) && !defined(DISABLE_JSON)
        case INJSON:
          if (dwg_read_json (&dat, &dwg) < DWG_ERR_CRITICAL)
            {
              out_dat.version = R_2000;
              if (dwg_encode (&dwg, &out_dat) >= DWG_ERR_CRITICAL)
                exit (0);
            }
          break;
#endif
#ifndef DISABLE_DXF
        case DXF:
          if (dwg_decode (&dat, &dwg) < DWG_ERR_CRITICAL)
            {
              if (dwg_write_dxf (&out_dat, &dwg) >= DWG_ERR_CRITICAL)
                exit (0);
            }
          break;
        case DXFB:
          if (dwg_decode (&dat, &dwg) < DWG_ERR_CRITICAL)
            {
              if (dwg_write_dxfb (&out_dat, &dwg) >= DWG_ERR_CRITICAL)
                exit (0);
            }
          break;
#endif
#ifndef DISABLE_JSON
        case JSON:
          if (dwg_decode (&dat, &dwg) < DWG_ERR_CRITICAL)
            {
              if (dwg_write_json (&out_dat, &dwg) >= DWG_ERR_CRITICAL)
                exit (0);
            }
          break;
        case GEOJSON:
          if (dwg_decode (&dat, &dwg) < DWG_ERR_CRITICAL)
            {
              if (dwg_write_geojson (&out_dat, &dwg) >= DWG_ERR_CRITICAL)
                exit (0);
            }
          break;
#endif
        case INVALID:
        default:
          exit (1);
        }
      FREE (out_dat.chain);
      fclose (out_dat.fh);
    }
  dwg_free (&dwg);
  return 0;
}

#if defined(USE_WRITE)

static dwg_point_2d *
scan_pts2d (unsigned num_pts, char **pp)
{
  char *p = *pp;
  dwg_point_2d *pts;

  p = strchr (p, '(');
  if (!p)
    return NULL;
  p++;
  if (num_pts > 5000)
    exit (0);
  pts = CALLOC (num_pts, 16);
  if (!pts)
    exit (0);
  for (unsigned i = 0; i < num_pts; i++)
    {
      if (sscanf (p, "(%lf %lf)", &pts[i].x, &pts[i].y))
        {
          p = strchr (p, ')');
          if (!p)
            break;
          p++;
        }
      else
        {
          *pp = p;
          p = NULL;
          break;
        }
    }

  if (p)
    {
      *pp = p;
      return pts;
    }
  else
    {
      FREE (pts);
      return NULL;
    }
}

static dwg_point_3d *
scan_pts3d (unsigned num_pts, char **pp)
{
  char *p = *pp;
  dwg_point_3d *pts;

  p = strchr (p, '(');
  if (!p)
    return NULL;
  p++;
  if (num_pts > 5000)
    exit (0);
  pts = CALLOC (num_pts, 24);
  if (!pts)
    exit (0);
  for (unsigned i = 0; i < num_pts; i++)
    {
      if (sscanf (p, "(%lf %lf %lf)", &pts[i].x, &pts[i].y, &pts[i].z))
        {
          p = strchr (p, ')');
          if (!p)
            break;
          p++;
        }
      else
        {
          *pp = p;
          p = NULL;
          break;
        }
    }

  if (p && num_pts)
    {
      *pp = p;
      return pts;
    }
  else
    {
      FREE (pts);
      return NULL;
    }
}

static char *
next_line (char *restrict p, const char *restrict end)
{
  while (p < end && *p != '\n')
    p++;
  if (p < end)
    p++;
  return p;
}

int
dwg_fuzz_dat (Dwg_Data **restrict dwgp, Bit_Chain *restrict dat)
{
  Dwg_Data *dwg;
  Dwg_Object *mspace;
  Dwg_Object_Ref *mspace_ref;
  Dwg_Object_BLOCK_HEADER *hdr;
  const char *end;
  char *p;
  Dwg_Version_Type version = R_INVALID;
  int i = 0;
  int imperial = 0;
  BITCODE_BL orig_num;

  if (!dat->chain)
    abort ();
  end = (char *)&dat->chain[dat->size - 1];
  if ((p = strstr ((char *)dat->chain, "\nimperial\n")))
    {
      imperial = 1;
      p += strlen ("\nimperial\n");
    }
  if ((p = strstr ((char *)dat->chain, "version")))
    {
      int i_ver;
      char s_ver[16];
      i = sscanf (p, "version %d", &i_ver);
      if (i)
        {
          snprintf (s_ver, 16, "r%d", i_ver);
          s_ver[15] = '\0';
          version = dwg_version_as (s_ver);
          p += strlen ("version ");
        }
      else
        p += strlen ("version ");
      p = next_line (p, end);
    }
  if (!i || version >= R_AFTER)
    version = R_2000;

  dwg = dwg_new_Document (version, imperial, 0);
  *dwgp = dwg;
  mspace = dwg_model_space_object (dwg);
  mspace_ref = dwg_model_space_ref (dwg);
  hdr = mspace->tio.object->tio.BLOCK_HEADER;
  orig_num = dwg->num_objects;

  // read dat line by line and call the matching add API
  while (p && p < end)
    {
      char text[120], s1[120];
      dwg_point_2d p2, p3, p4;
      dwg_point_3d pt1, pt2, pt3, pt4, scale;
      double height, rot, len, f1, f2;
      int i1, i2;
      unsigned u;
      Dwg_Entity_VIEWPORT *viewport = NULL;
      Dwg_Entity_MTEXT *mtext = NULL;
      Dwg_Object_DICTIONARY *dictionary = NULL;
      Dwg_Object_XRECORD *xrecord = NULL;
      Dwg_Object_MLINESTYLE *mlinestyle = NULL;
      Dwg_Object_DIMSTYLE *dimstyle = NULL;
      Dwg_Object_UCS *ucs = NULL;
      Dwg_Object_LAYOUT *layout = NULL;

// accepts only ASCII strings, for fuzzing only
#  ifdef HAVE_SSCANF_S
#    define SSCANF_S sscanf_s
#    define SZ , 119
#    define FMT_NAME "%[a-zA-Z0-9_]"
#    define FMT_TBL "\"%[a-zA-Z0-9._ -]\""
#    define FMT_PATH "\"%[a-zA-Z0-9_. \\-]\""
#    define FMT_ANY "\"%s\""
#  else
#    define SSCANF_S sscanf
#    define SZ
#    define FMT_NAME "%119[a-zA-Z0-9_]"
#    define FMT_TBL "\"%119[a-zA-Z0-9._ -]\""
#    define FMT_PATH "\"%119[a-zA-Z0-9_. \\-]\""
#    define FMT_ANY "\"%119s\""
#  endif

#  define SET_ENT(var, name)                                                  \
    if (!var)                                                                 \
      ;                                                                       \
    else if (SSCANF_S (p, #var "." FMT_NAME " = %d", &s1[0] SZ, &i1))         \
      dwg_dynapi_entity_set_value (var, #name, s1, &i1, 0);                   \
    else if (SSCANF_S (p, #var "." FMT_NAME " = %lf", &s1[0] SZ, &f1))        \
      dwg_dynapi_entity_set_value (var, #name, s1, &f1, 0);                   \
    else if (SSCANF_S (p, #var "." FMT_NAME " = " FMT_ANY, &s1[0] SZ,         \
                       &text[0] SZ))                                          \
    dwg_dynapi_entity_set_value (var, #name, s1, text, 1)

      if (SSCANF_S (p, "line (%lf %lf %lf) (%lf %lf %lf)", &pt1.x, &pt1.y,
                    &pt1.z, &pt2.x, &pt2.y, &pt2.z))
        dwg_add_LINE (hdr, &pt1, &pt2);
      else if (SSCANF_S (p, "ray (%lf %lf %lf) (%lf %lf %lf)", &pt1.x, &pt1.y,
                         &pt1.z, &pt2.x, &pt2.y, &pt2.z))
        dwg_add_RAY (hdr, &pt1, &pt2);
      else if (SSCANF_S (p, "xline (%lf %lf %lf) (%lf %lf %lf)", &pt1.x,
                         &pt1.y, &pt1.z, &pt2.x, &pt2.y, &pt2.z))
        dwg_add_XLINE (hdr, &pt1, &pt2);
      else if (SSCANF_S (p, "text " FMT_ANY " (%lf %lf %lf) %lf", &text[0] SZ,
                         &pt1.x, &pt1.y, &pt1.z, &height))
        dwg_add_TEXT (hdr, text, &pt1, height);
      else if (SSCANF_S (p, "mtext (%lf %lf %lf) %lf " FMT_ANY, &pt1.x, &pt1.y,
                         &pt1.z, &height, &text[0] SZ))
        mtext = dwg_add_MTEXT (hdr, &pt1, height, text);
      else
        SET_ENT (mtext, MTEXT);
      else if (SSCANF_S (p, "block " FMT_TBL, &text[0] SZ))
          dwg_add_BLOCK (hdr, text);
      else if (memBEGINc (p, "endblk\n")) dwg_add_ENDBLK (hdr);
      else if (SSCANF_S (p, "insert (%lf %lf %lf) " FMT_TBL " %lf %lf %lf %lf",
                         &pt1.x, &pt1.y, &pt1.z, &text[0] SZ, &scale.x,
                         &scale.y, &scale.z, &rot))
          dwg_add_INSERT (hdr, &pt1, text, scale.x, scale.y, scale.z,
                          deg2rad (rot));
      else if (SSCANF_S (p,
                         "minsert (%lf %lf %lf) " FMT_TBL
                         " %lf %lf %lf %lf %d %d "
                         "%lf %lf",
                         &pt1.x, &pt1.y, &pt1.z, &text[0] SZ, &scale.x,
                         &scale.y, &scale.z, &rot, &i1, &i2, &f1, &f2))
          dwg_add_MINSERT (hdr, &pt1, text, scale.x, scale.y, scale.z,
                           deg2rad (rot), i1, i2, f1, f2);
      else if (SSCANF_S (p, "point (%lf %lf %lf)", &pt1.x, &pt1.y, &pt1.z))
          dwg_add_POINT (hdr, &pt1);
      else if (SSCANF_S (p, "circle (%lf %lf %lf) %lf", &pt1.x, &pt1.y, &pt1.z,
                         &f1)) dwg_add_CIRCLE (hdr, &pt1, f1);
      else if (SSCANF_S (p, "arc (%lf %lf %lf) %lf %lf %lf", &pt1.x, &pt1.y,
                         &pt1.z, &f1, &f2, &height))
          dwg_add_ARC (hdr, &pt1, f1, f2, height);
      else if (SSCANF_S (p,
                         "dimension_aligned (%lf %lf %lf) (%lf %lf %lf) (%lf "
                         "%lf %lf)",
                         &pt1.x, &pt1.y, &pt1.z, &pt2.x, &pt2.y, &pt2.z,
                         &pt3.x, &pt3.y, &pt3.z))
          dwg_add_DIMENSION_ALIGNED (hdr, &pt1, &pt2, &pt3);
      else if (SSCANF_S (
                   p,
                   "dimension_linear (%lf %lf %lf) (%lf %lf %lf) (%lf %lf "
                   "%lf) %lf",
                   &pt1.x, &pt1.y, &pt1.z, &pt2.x, &pt2.y, &pt2.z, &pt3.x,
                   &pt3.y, &pt3.z, &rot))
          dwg_add_DIMENSION_LINEAR (hdr, &pt1, &pt2, &pt3, deg2rad (rot));
      else if (SSCANF_S (
                   p,
                   "dimension_ang2ln (%lf %lf %lf) (%lf %lf %lf) (%lf %lf "
                   "%lf) (%lf %lf %lf)",
                   &pt1.x, &pt1.y, &pt1.z, &pt2.x, &pt2.y, &pt2.z, &pt3.x,
                   &pt3.y, &pt3.z, &pt4.x, &pt4.y, &pt4.z))
          dwg_add_DIMENSION_ANG2LN (hdr, &pt1, &pt2, &pt3, &pt4);
      else if (SSCANF_S (
                   p,
                   "dimension_ang3pt (%lf %lf %lf) (%lf %lf %lf) (%lf %lf "
                   "%lf) (%lf %lf %lf)",
                   &pt1.x, &pt1.y, &pt1.z, &pt2.x, &pt2.y, &pt2.z, &pt3.x,
                   &pt3.y, &pt3.z, &pt4.x, &pt4.y, &pt4.z))
          dwg_add_DIMENSION_ANG3PT (hdr, &pt1, &pt2, &pt3, &pt4);
      else if (SSCANF_S (p,
                         "dimension_diameter (%lf %lf %lf) (%lf %lf %lf) %lf",
                         &pt1.x, &pt1.y, &pt1.z, &pt2.x, &pt2.y, &pt2.z, &len))
          dwg_add_DIMENSION_DIAMETER (hdr, &pt1, &pt2, len);
      else if (SSCANF_S (p,
                         "dimension_ordinate (%lf %lf %lf) (%lf %lf %lf) %d",
                         &pt1.x, &pt1.y, &pt1.z, &pt2.x, &pt2.y, &pt2.z, &i1))
          dwg_add_DIMENSION_ORDINATE (hdr, &pt1, &pt2, i1 ? true : false);
      else if (SSCANF_S (p, "dimension_radius (%lf %lf %lf) (%lf %lf %lf) %lf",
                         &pt1.x, &pt1.y, &pt1.z, &pt2.x, &pt2.y, &pt2.z, &len))
          dwg_add_DIMENSION_RADIUS (hdr, &pt1, &pt2, len);
      else if (SSCANF_S (p,
                         "3dface (%lf %lf %lf) (%lf %lf %lf) (%lf %lf "
                         "%lf) (%lf %lf %lf)",
                         &pt1.x, &pt1.y, &pt1.z, &pt2.x, &pt2.y, &pt2.z,
                         &pt3.x, &pt3.y, &pt3.z, &pt4.x, &pt4.y, &pt4.z))
          dwg_add_3DFACE (hdr, &pt1, &pt2, &pt3, &pt4);
      else if (SSCANF_S (p,
                         "3dface (%lf %lf %lf) (%lf %lf %lf) (%lf %lf "
                         "%lf)",
                         &pt1.x, &pt1.y, &pt1.z, &pt2.x, &pt2.y, &pt2.z,
                         &pt3.x, &pt3.y, &pt3.z))
          dwg_add_3DFACE (hdr, &pt1, &pt2, &pt3, NULL);
      else if (SSCANF_S (
                   p, "solid (%lf %lf %lf) (%lf %lf) (%lf %lf)  (%lf %lf)",
                   &pt1.x, &pt1.y, &pt1.z, &p2.x, &p2.y, &p3.x, &p3.y, &p4.x,
                   &p4.y)) dwg_add_SOLID (hdr, &pt1, &p2, &p3, &p4);
      else if (SSCANF_S (
                   p, "trace (%lf %lf %lf) (%lf %lf) (%lf %lf)  (%lf %lf)",
                   &pt1.x, &pt1.y, &pt1.z, &p2.x, &p2.y, &p3.x, &p3.y, &p4.x,
                   &p4.y)) dwg_add_TRACE (hdr, &pt1, &p2, &p3, &p4);
      else if (SSCANF_S (p, "polyline_2d %d ((%lf %lf)", &i1, &pt1.x, &pt1.y))
      {
        dwg_point_2d *pts = scan_pts2d (i1, &p);
        if (i1 && pts)
          {
            dwg_add_POLYLINE_2D (hdr, i1, pts);
            FREE (pts);
          }
      }
      else if (SSCANF_S (p, "polyline_3d %d ((%lf %lf %lf)", &i1, &pt1.x,
                         &pt1.y, &pt1.z))
      {
        dwg_point_3d *pts = scan_pts3d (i1, &p);
        if (i1 && pts)
          {
            dwg_add_POLYLINE_3D (hdr, i1, pts);
            FREE (pts);
          }
      }
      else if (SSCANF_S (p, "polyline_mesh %d %d ((%lf %lf %lf)", &i1, &i2,
                         &pt1.x, &pt1.y, &pt1.z))
      {
        dwg_point_3d *pts = scan_pts3d (i1 * i2, &p);
        if (i1 && i2 && pts)
          {
            dwg_add_POLYLINE_MESH (hdr, i1, i2, pts);
            FREE (pts);
          }
      }
      else if (SSCANF_S (p, "dictionary " FMT_TBL " " FMT_TBL " %u",
                         &text[0] SZ, &s1[0] SZ, &u)) dictionary
          = dwg_add_DICTIONARY (dwg, text, s1, (unsigned long)u);
      else if (dictionary
               && SSCANF_S (p, "xrecord dictionary " FMT_TBL, &text[0] SZ))
          xrecord
          = dwg_add_XRECORD (dictionary, text);
      else if (SSCANF_S (p, "shape " FMT_PATH " (%lf %lf %lf) %lf %lf",
                         &text[0] SZ, &pt1.x, &pt1.y, &pt1.z, &scale.x, &rot))
          dwg_add_SHAPE (hdr, text, &pt1, scale.x, deg2rad (rot));
      else if (SSCANF_S (p, "viewport " FMT_TBL, &text[0] SZ)) viewport
          = dwg_add_VIEWPORT (hdr, text);
      else SET_ENT (viewport, VIEWPORT);
      else if (SSCANF_S (p, "ellipse (%lf %lf %lf) %lf %lf", &pt1.x, &pt1.y,
                         &pt1.z, &f1, &f2))
          dwg_add_ELLIPSE (hdr, &pt1, f1, f2);
      else if (SSCANF_S (p, "spline %d ((%lf %lf %lf)", &i1, &pt1.x, &pt1.y,
                         &pt1.z))
      {
        dwg_point_3d *fitpts = scan_pts3d (i1, &p);
        if (i1 && fitpts
            && sscanf (p, ") (%lf %lf %lf) (%lf %lf %lf)", &pt2.x, &pt2.y,
                       &pt2.z, &pt3.x, &pt3.y, &pt3.z))
          {
            dwg_add_SPLINE (hdr, i1, fitpts, &pt2, &pt3);
          }
        FREE (fitpts);
      }
      else if (mtext
               && sscanf (p, "leader %d ((%lf %lf %lf)", &i1, &pt1.x, &pt1.y,
                          &pt1.z))
      {
        dwg_point_3d *pts = scan_pts3d (i1, &p);
        if (i1 && pts && sscanf (p, ") mtext %d", &i2))
          {
            dwg_add_LEADER (hdr, i1, pts, mtext, i2);
          }
        FREE (pts);
      }
      else if (SSCANF_S (p,
                         "tolerance " FMT_TBL " (%lf %lf %lf) (%lf %lf %lf)",
                         &text[0] SZ, &pt1.x, &pt1.y, &pt1.z, &pt2.x, &pt2.y,
                         &pt2.z)) dwg_add_TOLERANCE (hdr, text, &pt1, &pt2);
      else if (SSCANF_S (p, "mlinestyle " FMT_TBL, &text[0] SZ)) mlinestyle
          = dwg_add_MLINESTYLE (dwg, text);
      else if (SSCANF_S (p, "dimstyle " FMT_TBL, &text[0] SZ)) dimstyle
          = dwg_add_DIMSTYLE (dwg, text);
      else SET_ENT (mlinestyle, MLINESTYLE);
      else SET_ENT (dimstyle, DIMSTYLE);
      else if (SSCANF_S (
                   p, "ucs (%lf %lf %lf) (%lf %lf %lf) (%lf %lf %lf) " FMT_TBL,
                   &pt1.x, &pt1.y, &pt1.z, &pt2.x, &pt2.y, &pt2.z, &pt3.x,
                   &pt3.y, &pt3.z, &text[0] SZ)) ucs
          = dwg_add_UCS (dwg, &pt1, &pt2, &pt3, text);
      else SET_ENT (ucs, UCS);
      else if (viewport
               && SSCANF_S (p, "layout viewport " FMT_TBL " " FMT_ANY,
                            &text[0] SZ, &s1[0] SZ))
      {
        int error;
        Dwg_Object *obj = dwg_ent_generic_to_object (viewport, &error);
        if (!error)
          layout = dwg_add_LAYOUT (obj, text, s1);
      }
      else if (SSCANF_S (p, "torus (%lf %lf %lf) (%lf %lf %lf) %lf %lf",
                         &pt1.x, &pt1.y, &pt1.z, &pt2.x, &pt2.y, &pt2.z, &f1,
                         &f2)) dwg_add_TORUS (hdr, &pt1, &pt2, f1, f2);
      else if (SSCANF_S (p, "sphere (%lf %lf %lf) (%lf %lf %lf) %lf", &pt1.x,
                         &pt1.y, &pt1.z, &pt2.x, &pt2.y, &pt2.z, &f1))
          dwg_add_SPHERE (hdr, &pt1, &pt2, f1);
      else if (SSCANF_S (
                   p, "cylinder (%lf %lf %lf) (%lf %lf %lf) %lf %lf %lf %lf",
                   &pt1.x, &pt1.y, &pt1.z, &pt2.x, &pt2.y, &pt2.z, &height,
                   &f1, &f2, &len))
          dwg_add_CYLINDER (hdr, &pt1, &pt2, height, f1, f2, len);
      else if (SSCANF_S (p, "cone (%lf %lf %lf) (%lf %lf %lf) %lf %lf %lf %lf",
                         &pt1.x, &pt1.y, &pt1.z, &pt2.x, &pt2.y, &pt2.z,
                         &height, &f1, &f2, &len))
          dwg_add_CONE (hdr, &pt1, &pt2, height, f1, f2, len);
      else if (SSCANF_S (p, "wedge (%lf %lf %lf) (%lf %lf %lf) %lf %lf %lf",
                         &pt1.x, &pt1.y, &pt1.z, &pt2.x, &pt2.y, &pt2.z, &len,
                         &f1, &height))
          dwg_add_WEDGE (hdr, &pt1, &pt2, len, f1, height);
      else if (SSCANF_S (p, "box (%lf %lf %lf) (%lf %lf %lf) %lf %lf %lf",
                         &pt1.x, &pt1.y, &pt1.z, &pt2.x, &pt2.y, &pt2.z, &len,
                         &f1, &height))
          dwg_add_BOX (hdr, &pt1, &pt2, len, f1, height);
      else if (SSCANF_S (p,
                         "pyramid (%lf %lf %lf) (%lf %lf %lf) %lf %d %lf %lf",
                         &pt1.x, &pt1.y, &pt1.z, &pt2.x, &pt2.y, &pt2.z,
                         &height, &i1, &f1, &f2))
          dwg_add_PYRAMID (hdr, &pt1, &pt2, height, i1, f1, f2);
      else if (SSCANF_S (p, "HEADER." FMT_NAME " = %d", &s1[0] SZ, &i1))
          dwg_dynapi_header_set_value (dwg, s1, &i1, 0);
      else if (SSCANF_S (p, "HEADER." FMT_NAME " = %lf", &s1[0] SZ, &f1))
          dwg_dynapi_header_set_value (dwg, s1, &f1, 0);
      else if (SSCANF_S (p, "HEADER." FMT_NAME " = " FMT_ANY, &s1[0] SZ,
                         &text[0] SZ))
          dwg_dynapi_header_set_value (dwg, s1, text, 1);

      p = next_line (p, end);
    }
  // dwg_resolve_objectrefs_silent (orig_dwg);
  //  start fuzzing if at least 2 entities were added.
  return (dwg->num_objects - orig_num > 2 ? 0 : 1);
}

#endif // USE_WRITE
