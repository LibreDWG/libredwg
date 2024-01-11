#!/bin/sh
# shellcheck disable=SC2034,SC2044
set -e
make -s -j4 -C src
make -s -C examples llvmfuzz_standalone
old_ASAN_OPTIONS="$ASAN_OPTIONS"

for f in $(find ../test/test-data/ -type f -iname \*.dxf -o -iname \*.dwg \
    -o -iname \*.json); do
    #indxf still leaks
    if echo "$f" | grep -i "\.dxf$"; then
        export ASAN_OPTIONS=detect_leaks=0
    else
        export ASAN_OPTIONS=
    fi
    for OUT in $(seq 0 4); do
        if [ "$OUT" = "0" ]; then
            for VER in $(seq 0 16); do
                OUT="$OUT" VER="$VER" timeout 30 examples/llvmfuzz_standalone "$f" || true
            done
        else
            OUT="$OUT" timeout 30 examples/llvmfuzz_standalone "$f" || true
        fi
    done
done
ASAN_OPTIONS="$old_ASAN_OPTIONS"
