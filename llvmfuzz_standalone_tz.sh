#!/bin/sh
# assuming ../tz has all the ossfuzz testcases
# shellcheck disable=SC2034
set -e
make -s -j4 -C src
make -s -C examples llvmfuzz_standalone
old_ASAN_OPTIONS="$ASAN_OPTIONS"

run() {
    f="$1"
    # indxf still leaks
    if echo "$f" | grep -i "\.dxf$"; then
        export ASAN_OPTIONS=detect_leaks=0
    elif echo "$f" | grep "/indxf"; then
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
}

if [ -z "$1" ]; then
    for f in ../tz/*/*; do
        run "$f"
    done
else
    for f in "$@"; do
        run "$f"
    done
fi
ASAN_OPTIONS="$old_ASAN_OPTIONS"
