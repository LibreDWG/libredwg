#!/bin/sh
arg=${1:-sample_2000}
dir=$(dirname $arg)
base=$(basename $arg)
if [ x$dir = x. ]; then out=${base};
else out=${base}_${dir}
fi
echo "$arg => $out.dxf"

gmake -s -j4 && dsymutil programs/dwg2dxf && \
  echo programs/dwg2dxf -v4 -o $out.dxf test/test-data/$arg.dwg && \
  programs/dwg2dxf -v4 -o $out.dxf test/test-data/$arg.dwg 2>${out}.log && \
  programs/filt_dxf.pl $out.dxf > _${out}.dxf; mv _${out}.dxf ${out}.dxf;

egrep -v -B1 '\r' ${out}.dxf | head && unix2dos ${out}.dxf

if [ -f test/test-data/$arg.dxf ]; then
    diff -bu test/test-data/$arg.dxf $out.dxf | programs/cmp_dxf.pl |less
elif [ -f test/test-data-$dir/$arg.dxf ]; then
  diff -bu test/test-data-$dir/$arg.dxf $out.dxf | programs/cmp_dxf.pl |less
fi
