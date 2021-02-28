// requires -fsanitize=fuzzer
#include <stdio.h>
#include <stdlib.h>
//#include <unistd.h>

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

// libfuzzer limitation:
// Enforce NULL-termination of the input buffer, to avoid bugus reports. copy it.
// Problematic is mostly strtol(3) which also works with \n termination.
static int enforce_null_termination(Bit_Chain *dat)
{
    unsigned char *copy;
    unsigned char c;
    if (!dat->size)
      return 0;
    c = dat->chain[dat->size - 1];
    // Allow \n termination
    if (c == '\0' || c == '\n')
      return 0;
    //fprintf (stderr, "llvmfuzz: Enforce libfuzzer buffer NULL termination\n");
    copy = malloc (dat->size + 1);
    memcpy (copy, dat->chain, dat->size);
    copy[dat->size] = '\0';
    dat->chain = copy;
    return 1;
}

int LLVMFuzzerTestOneInput(const unsigned char *data, size_t size) {
    Dwg_Data dwg;
    Bit_Chain dat = { NULL, 0, 0, 0, 0 };
    Bit_Chain out_dat = { NULL, 0, 0, 0, 0 };
    int copied = 0;
    struct ly_ctx *ctx = NULL;

    static char tmp_file[256];
    dat.chain = (unsigned char *)data;
    dat.size = size;

    memset (&out_dat, 0, sizeof (out_dat));
    memset (&dwg, 0, sizeof (dwg));

    const unsigned int possible_outputformats =
#ifdef DISABLE_DXF
# ifdef DISABLE_JSON
        1;
# else
        3;
# endif
#else
        5;
#endif

    // Detect the input format: DWG, DXF or JSON
    if (dat.size > 2 && dat.chain[0] == 'A' && dat.chain[1] == 'C')
      {
        if (dwg_decode (&dat, &dwg) >= DWG_ERR_CRITICAL)
          return 0;
      }
#ifndef DISABLE_JSON
    else if (dat.size > 1 && dat.chain[0] == '{')
      {
        copied = enforce_null_termination(&dat);
        if (dwg_read_json (&dat, &dwg) >= DWG_ERR_CRITICAL)
          {
            if (copied) bit_chain_free (&dat);
            return 0;
          }
      }
#endif
#ifndef DISABLE_DXF
    else
      {
        copied = enforce_null_termination(&dat);
        if (dwg_read_dxf (&dat, &dwg) >= DWG_ERR_CRITICAL)
          {
            if (copied) bit_chain_free (&dat);
            return 0;
          }
      }
#else
    else
      return 0;
#endif
    bit_chain_set_version (&out_dat, &dat);
    if (copied)
      bit_chain_free (&dat);

#if 0
    snprintf (tmp_file, 255, "/tmp/llvmfuzzer%d.out", getpid());
    tmp_file[255] = '\0';
#elif _WIN32
    strcpy (tmp_file, "NUL");
#else
    strcpy (tmp_file, "/dev/null");
#endif
    out_dat.fh = fopen(tmp_file, "w");

    switch (rand () % possible_outputformats)
      {
      case 0:
          switch (rand() % 6)
            {
            case 0:
              dwg.header.version = R_13;
              break;
            case 1:
              dwg.header.version = R_13c3;
              break;
            case 2:
              dwg.header.version = R_14;
              break;
            case 3: // favor this one
            case 4:
            case 5:
              dwg.header.version = R_2000;
              break;
            }
          dwg_encode (&dwg, &out_dat);
          free (out_dat.chain);
          break;
#ifndef DISABLE_DXF
      case 1:
          dwg_write_dxf (&out_dat, &dwg);
          free (out_dat.chain);
          break;
      case 20: // disabled for now
          dwg_write_dxfb (&out_dat, &dwg);
          free (out_dat.chain);
          break;
# ifndef DISABLE_JSON
      case 3:
          dwg_write_json (&out_dat, &dwg);
          free (out_dat.chain);
          break;
      case 4:
          dwg_write_geojson (&out_dat, &dwg);
          free (out_dat.chain);
          break;
# endif
#endif
      default: break;
      }
    dwg_free (&dwg);
    fclose (out_dat.fh);
    //unlink (tmp_file);
    return 0;
}

