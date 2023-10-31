#!/bin/sh
# assuming ../tz has all the ossfuzz testcases
# shellcheck disable=SC2034
set -e
make -s -j4 -C src
make -s -C examples llvmfuzz_standalone
for f in ../tz/*/*
do
    for OUT in $(seq 0 4)
    do
        if [ "$OUT" = "0" ]; then
            for VER in $(seq 0 16)
            do
                ASAN_OPTIONS=detect_leaks=0 examples/llvmfuzz_standalone "$f" || true
            done
        else
            ASAN_OPTIONS=detect_leaks=0 examples/llvmfuzz_standalone "$f" || true
        fi
    done
done
