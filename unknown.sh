#!/bin/sh
make -s -j4 -C src
make -s -j4 -C examples unknown
perl -anle'm{/\* } && print $F[1]' examples/alldxf_1.inc | sort -u >unknown.lst
while IFS= read -r s; do
    examples/unknown --class "$s" >"unknown-$s.log"
    # shellcheck disable=SC2039,SC3037
    /bin/echo -n "$s "
    tail -n1 "unknown-$s.log"
done <unknown.lst
