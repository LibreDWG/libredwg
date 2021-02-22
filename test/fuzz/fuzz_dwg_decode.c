#include <stdio.h>
#include <stdlib.h>

#include <dwg.h>
#include <dwg_api.h>
#include "common.h"
#include "decode.h"

int LLVMFuzzerTestOneInput(const char *data, size_t size) {
    Dwg_Data dwg;
    Bit_Chain dat = { NULL, 0, 0, 0, 0 };
    struct ly_ctx *ctx = NULL;

    char filename[256];
    sprintf(filename, "/tmp/libfuzzer.%d", getpid());

    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        return 0;
    }
    fwrite(data, size, 1, fp);
    fclose(fp);

    fp = fopen(filename, "r");
    dat_read_file (&dat, fp, filename);
    fclose(fp);

    dwg_decode (&dat, &dwg);

    unlink(filename);
    return 0;
}

