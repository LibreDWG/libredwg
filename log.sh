#!/bin/sh
# create -v4 logs for DWGs
make -s -j4
v=-v4

for d in "$@"; do
    case $d in
        -v*) v=$d
             ;;
        *)
        if [ ! -f $d ]; then
            if [ -f test/test-data/$d ]; then d=test/test-data/$d;
            else
                if [ -f test/test-data/$d.dwg ]; then d=test/test-data/$d.dwg; fi
            fi
        fi
        dir=_$(basename `dirname "$d"`)
        if [ x$dir = x_test-data ]; then dir=""; fi
        log=`basename "$d" .dwg`$dir.log
        echo programs/dwgread $v "$d" 2\>$log
        # (sleep 12s; killall dwgread 2>/dev/null) &
        timeout 12 programs/dwgread $v "$d" 2>$log
        ;;
    esac
done

