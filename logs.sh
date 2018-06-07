#!/bin/sh
# create -v5 logs for all DWGs
gmake -s -j4
for d in test/test-data/2*/*.dwg test/test-data/r*/*.dwg; do
    dir=$(basename `dirname "$d"`)
    log=`basename "$d" .dwg`_$dir.log
    echo $d
    programs/dwgread -v5 "$d" 2>$log
done
for d in test/test-data/*.dwg; do
    log=`basename "$d" .dwg`.log
    echo $d
    programs/dwgread -v5 "$d" 2>$log
done
