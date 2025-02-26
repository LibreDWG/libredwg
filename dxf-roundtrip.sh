#!/usr/bin/env bash
# test dwg2dxf via teigha roundtrips
# for f in test/test-data/20*/*.dwg; do ./dxf-roundtrip.sh $f; done
case $(uname) in
    Darwin)  ODAFileConverter=/Applications/ODAFileConverter.app/Contents/MacOS/ODAFileConverter ;;
    Linux)   ODAFileConverter=/usr/bin/ODAFileConverter ;;
    Windows) ODAFileConverter=ODAFileConverter ;;
esac
#full=test/test-data/2000/$f.dwg
full=$1
if [ ! -f $full ]; then
    echo $full not found
    exit
fi
f=$(basename $full .dwg)
r=$(basename $(dirname $full))
case $r in
    20*)      ;;
    r9)       r=9 ;;
    r11)      r=12 ;;
    r1[0234]) r=${r:1} ;;
    *)  case $f in
            *_20*)
                r=${f:(-4)}
                f=$(basename $f _$r)
                ;;
            *_r9)
                r=9
                f=$(basename $f _r$r)
                ;;
            *_r11)
                r=12
                ;;
            *_r1[0234])
                r=${f:(-2)}
                f=$(basename $f _r$r)
                ;;
            *)
               echo wrong version $r
               exit
               ;;
    esac     ;;
esac
case $r in
    9|10|11|12|13|14|2000|2004|2007|2010|2013|2018) ;;
    *) make -s -C examples odaversion
       r="$(examples/odaversion "$full")"
       ;;
esac

echo programs/dwg2dxf -y -v4 -o ${f}_${r}.dxf $full
programs/dwg2dxf -y -v4 -o ${f}_${r}.dxf $full 2>${f}_${r}.log ||
    grep Error ${f}_${r}.log

mv ${f}_${r}.dxf test/
echo ODAFileConverter "test" . ACAD$r DWG 0 1 ${f}_${r}.dxf
$ODAFileConverter "test" "." ACAD$r DWG 0 1 ${f}_${r}.dxf
mv test/${f}_${r}.dxf ./

if [ -f ${f}_${r}.dwg ]; then
    echo test ${f}_${r}.dwg created from ${f}_${r}.dxf
    programs/dwgread -v4 ${f}_${r}.dwg 2>${f}_${r}_dxf.log ||
        grep Error ${f}_${r}_dxf.log
else
    cat ${f}_${r}.dwg.err
fi
