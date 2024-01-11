#!/bin/sh
make -s
make -s -j4 -C src &&
    make -s -j4 -C programs dwg2dxf &&
    programs/dwg2dxf -m -y -o Drawing_2000_min.dxf test/test-data/Drawing_2000.dwg

for f in test/test-data/Drawing_2*.dwg \
    test/test-data/sample_2*.dwg \
    test/test-data/example_*.dwg; do
    echo
    echo programs/dwg2dxf -v0 -y "$f"
    programs/dwg2dxf -v0 -y "$f"
done

for d in r9 r10 r11 r12 r13 r14 2000 2004 2007 2010 2013 2018; do
    for f in test/test-data/"$d"/*.dwg; do
        if [ "$f" != "test/test-data/$d/*.dwg" ]; then
            b="$(basename "$f" .dwg)"
            echo
            echo programs/dwg2dxf -v0 -y -o "${b}_${d}.dxf" "$f"
            programs/dwg2dxf -v0 -y -o "${b}_${d}.dxf" "$f"
        fi
    done
    for f in test/test-big/"$d"/*.dwg; do
        if [ "$f" != "test/test-big/$d/*.dwg" ]; then
            b="$(basename "$f" .dwg)"
            echo
            echo programs/dwg2dxf -v0 -y -o "${b}_${d}.dxf" "$f"
            programs/dwg2dxf -v0 -y -o "${b}_${d}.dxf" "$f"
        fi
    done
done

for f in test/test-old/r*/*/*.DWG test/test-old/r*/*/*.dwg test/test-old/2*/*/*.dwg; do
    ./dxf -v0 "$f"
done
