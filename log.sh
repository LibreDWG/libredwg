#!/bin/sh
# create -v5 logs for a DWGs
make -s -j4
d=$1
if [ ! -f $d ]; then
    if [ -f test/test-data/$d ]; then d=test/test-data/$d;
    else
        if [ -f test/test-data/$d.dwg ]; then d=test/test-data/$d.dwg; fi
    fi
fi
dir=_$(basename `dirname "$d"`)
if [ x$dir = x_test-data ]; then dir=""; fi
log=`basename "$d" .dwg`$dir.log
echo programs/dwgread -v5 "$d" 2\>$log
(sleep 10s; killall dwgread 2>/dev/null) &

programs/dwgread -v5 "$d" 2>$log
