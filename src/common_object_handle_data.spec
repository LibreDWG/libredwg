/* -*- c -*- */

  #include "spec.h"

  FIELD_BL (num_reactors, 0);
#ifdef IS_DECODER
  if (FIELD_VALUE (num_reactors) * dwg_bits_size[BITS_HANDLE] > AVAIL_BITS (hdl_dat))
    {
      LOG_ERROR ("num_reactors: " FORMAT_BL " > AVAIL_BITS(hdl_dat): %lld\n",
                 FIELD_VALUE (num_reactors), AVAIL_BITS (hdl_dat))
      FIELD_VALUE (num_reactors) = 0;
      return DWG_ERR_VALUEOUTOFBOUNDS;
    }
#endif
  SINCE (R_2004) { FIELD_B (xdic_missing_flag, 0); }
  SINCE (R_2013) { FIELD_B (has_ds_binary_data, 0); }

#ifdef IS_DXF
  // unused. see out_dxf.c
  SINCE (R_13) {
    XDICOBJHANDLE (3);
    REACTORS (4);
  }
#endif

  SINCE (R_14) {
    FIELD_HANDLE (ownerhandle, 4, 330);
  }

#ifndef IS_DXF
  SINCE (R_13) {
    REACTORS (4)
    XDICOBJHANDLE (3)
  }
#endif
