#!/bin/sh
make -s -j4 -C examples unknown
perl -anle'm{/\* } && print $F[1]' examples/alldxf_1.inc | sort -u > unknown.lst
for s in `cat unknown.lst`
do
    examples/unknown --class $s > unknown-$s.log
    /bin/echo -n "$s "
    tail -n1 unknown-$s.log
done
