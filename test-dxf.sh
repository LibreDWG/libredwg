#!/bin/sh
# see also dxf-roundtrip.sh
arg="${1:-sample_2000}"
dir="$(dirname "$arg")"
base="$(basename "$arg")"
if [ "$dir" = "." ]; then
    out="${base}"
else
    out="${base}_${dir}"
fi
cmpdxf="programs/cmp_dxf.pl"
filtdxf="programs/filt_dxf.pl"
datadir="test/test-data"
if [ ! -f "$cmpdxf" ]; then
    cmpdxf="../programs/cmp_dxf.pl"
    filtdxf="../programs/filt_dxf.pl"
    datadir="../test/test-data"
fi
echo "$arg => $out.dxf"

make -s -j4 &&
    echo programs/dwg2dxf -v4 -o "$out.dxf" "$datadir/$arg.dwg" &&
    programs/dwg2dxf -v4 -o "$out.dxf" "$datadir/$arg.dwg" 2>"${out}.log" &&
    $filtdxf "$out.dxf" >"_${out}.dxf"
mv "_${out}.dxf" "${out}.dxf"

grep -E -v -B1 '\r' "${out}.dxf" | head && unix2dos "${out}.dxf"

if [ -f "$datadir/$arg.dxf" ]; then
    diff -bu "$datadir/$arg.dxf" "$out.dxf" | $cmpdxf | less
elif [ -f "$datadir-$dir/$arg.dxf" ]; then
    diff -bu "$datadir-$dir/$arg.dxf" "$out.dxf" | $cmpdxf | less
fi
