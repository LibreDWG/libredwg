#!/bin/sh
# create -v5 logs for all DWGs
make -s -j4
for d in test/test-data/2*/*.dwg test/test-data/r*/*.dwg; do
    dir=$(basename `dirname "$d"`)
    log=`basename "$d" .dwg`_$dir.log
    echo $d
    timeout -k 1 10 programs/dwgread -v5 "$d" 2>$log
    #(sleep 10s; kill %1 2>/dev/null) &
done
for d in test/test-data/*.dwg; do
    log=`basename "$d" .dwg`.log
    echo $d
    timeout -k 1 10 programs/dwgread -v5 "$d" 2>$log
    #(sleep 10s; kill %1 2>/dev/null) &
done
#with background killing we would need to wait for all processes to end
# while pgrep dwgread; do sleep 1; done

#GNU parallel would be better:
# parallel timeout 10 programs/dwgread -v5 {} \2\>{/.}.log ::: test/test-data/*.dwg
# cd test/test-data
# parallel timeout 10 ../../programs/dwgread -v5 {} \2\>../../{/.}_{//}.log ::: */*.dwg
# cd ../..
