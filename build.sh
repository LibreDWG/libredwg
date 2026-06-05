#!/usr/bin/env bash
# Vercel buildCommand: compile dwg2dxf from THIS pinned source commit and stage
# it for the Python function bundle. No binaries are committed to the repo.
#
# If the Vercel build image lacks the C toolchain (cmake / compiler), the build
# fails here with a clear BLOCKED line — that is the feasibility answer.
set -uo pipefail

echo "== build.sh: toolchain probe =="
for t in cc gcc make cmake perl; do
  printf '  %s: ' "$t"; command -v "$t" || echo MISSING
done
cmake --version 2>/dev/null | head -1 || true

# Vercel ("Other" framework) wants a static output directory to serve.
mkdir -p public
echo "libredwg vercel python feasibility spike" > public/index.html

if ! command -v cmake >/dev/null 2>&1; then
  echo "BLOCKED: cmake not present in the Vercel build image; cannot build LibreDWG from source." >&2
  exit 1
fi
if ! command -v cc >/dev/null 2>&1 && ! command -v gcc >/dev/null 2>&1; then
  echo "BLOCKED: no C compiler (cc/gcc) in the Vercel build image." >&2
  exit 1
fi

set -e
echo "== build.sh: configure (static) =="
cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_SHARED_LIBS=OFF \
  -DLIBREDWG_LIBONLY=OFF

echo "== build.sh: build dwg2dxf =="
cmake --build build --target dwg2dxf -j 2

mkdir -p api/bin
BIN="$(find build -type f -name dwg2dxf | head -1)"
if [ -z "${BIN}" ]; then
  echo "BLOCKED: dwg2dxf binary was not produced by the build." >&2
  exit 1
fi
cp "${BIN}" api/bin/dwg2dxf
chmod 0755 api/bin/dwg2dxf
# Co-bundle the shared lib in case the binary is not fully static.
find build -name 'libredwg.so*' -exec cp {} api/bin/ \; 2>/dev/null || true

echo "== build.sh: staged binary =="
ls -la api/bin || true
file api/bin/dwg2dxf 2>/dev/null || true
ldd  api/bin/dwg2dxf 2>/dev/null || true
echo "== build.sh: done =="
