#!/bin/sh

find -name .deps -type d -exec rm -rf '{}' 2>/dev/null \;
git clean -dxf src examples programs test/testcases test/unit-testing test/xmlsuite
sh autogen.sh
# autoreconf
make=make

case `uname` in
Darwin) # macports compilers
make=gmake

# for dejagnu
#export PKG_CONFIG_PATH=/opt/local/lib/pkgconfig
#export PYTHONPATH=/opt/local/Library/Frameworks/Python.framework/Versions/2.7/lib/python2.7/site-packages
# for pslib    
CFLAGS=-I/usr/local/include
LDFLAGS=-L/usr/local/lib

gmake -s -j4 clean
echo clang-mp-devel -fsanitize=address,undefined -fno-omit-frame-pointer --enable-trace
CC="clang-mp-devel -fsanitize=address,undefined -fno-omit-frame-pointer" \
    ./configure --enable-trace && \
    gmake -s -j4 check || exit
gmake -s -j4 clean

echo clang-mp-6.0 -O3 -march=native
CC="clang-mp-6.0 -O3 -march=native" \
    ./configure && \
    gmake -s -j4 check || exit
    gmake scan-build
gmake -s -j4 clean

for CC in gcc-mp-4.3 gcc-mp-4.6 gcc-mp-4.8 gcc-mp-4.9 gcc-mp-5 gcc-mp-6 gcc-mp-7 gcc-8
do
    echo $CC
    ./configure && \
    gmake -s -j4 check || exit
    gmake -s -j4 clean
done

for CC in clang-mp-3.3 clang-mp-3.7 clang-mp-3.8 clang-mp-3.9 clang-mp-4.0 \
          clang-mp-5.0 clang-mp-6.0 clang-mp-devel
do
    echo $CC
    CFLAGS="-I/opt/local/include" ./configure && \
    gmake -s -j4 check || exit
    gmake -s -j4 clean
done

unset PYTHONPATH
export PYTHONPATH
echo gcc-mp-6 --enable-python=python3.6m
CC="gcc-mp-6" ./configure --enable-python=python3.6m && \
    make -s -j4 check || exit
    make -s -j4 gcov
gmake -s -j4 clean

export PYTHONPATH=/opt/local/Library/Frameworks/Python.framework/Versions/2.7/lib/python2.7/site-packages

echo gcc-mp-6 --enable-gcov
CC="gcc-mp-6" ./configure --enable-trace --enable-gcov=gcov-mp-6 && \
    make -s -j4 check || exit
    make -s -j4 gcov
gmake -s -j4 clean

# port install arm-elf-gcc (with newlib, not glibc)
if [ -e /opt/local/bin/arm-elf-gcc-4.7 ]; then
    echo arm-elf-gcc-4.7 --host=arm-elf --disable-shared
    CC=arm-elf-gcc-4.7 ./configure --host=arm-elf --disable-shared && \
        gmake -s -j4 && \
        gmake -s -j4 -C examples load_dwg dwg2svg2
    #qemu-arm -L /opt/local/arm-elf programs/dwgread test/test-data/sample_2000.dwg || exit
fi
gmake -s -j4 clean

;;

Linux)
make -s -j4 clean

echo clang-7 -fsanitize=address -fno-omit-frame-pointer
CC="clang-7 -fsanitize=address -fno-omit-frame-pointer" \
    ./configure && \
    make -s -j4 check || exit
make -s -j4 clean
echo clang-5.0 -march=native
CC="clang-5.0 -march=native" \
    ./configure && \
    make -s -j4 check
make -s -j4 clean
echo clang-5.0 -std=c99
CC="clang-5.0 -std=c99" \
    ./configure  && \
    make -s -j4 check || exit
make -s -j4 clean
echo gcc-4.4 -ansi
CC="gcc-4.4 -ansi" ./configure && \
    make -s -j4 check || exit
make -s -j4 clean
echo gcc-6
CC="gcc-6" ./configure && \
    make -s -j4 check || exit
make -s -j4 clean
echo gcc-7
CC="gcc-7" ./configure  && \
    make -s -j4 check || exit
make -s -j4 clean
echo gcc-8
CC="gcc-8" ./configure && \
    make -s -j4 check || exit
make -s -j4 clean
echo gcc-8 --enable-gcov
CC="gcc-8" ./configure --enable-gcov=gcov-8 && \
    make -s -j4 check || exit
    make -s -j4 gcov
make -s -j4 clean

CC="clang-7 -fsanitize=address,undefined -fno-omit-frame-pointer" \
    ./configure --enable-trace && \
    make -s -j4 check || exit
make -s -j4 clean

for CC in gcc-4.3 gcc-4.6 gcc-4.8 gcc-4.9 gcc-5 gcc-6 gcc-7 gcc-8
do
    echo $CC
    ./configure && \
    make -s -j4 check || exit
    make -s -j4 clean
done

for CC in clang-3.0 clang-3.4 clang-3.5 clang-3.6 clang-3.7 \
          clang-5.0 clang-6.0 clang-7
do
    echo $CC
    ./configure && \
    make -s -j4 check || exit
    make -s -j4 clean
done

if [ -e /usr/bin/arm-linux-gnueabihf-gcc ]; then
    echo "--host=arm-linux-gnueabihf --disable-shared"
    ./configure --host=arm-linux-gnueabihf --disable-shared && \
        make -s -j4 || exit;
    #if [ ! -e /usr/arm-linux-gnueabihf/lib/libredwg.so.0 ]; then
    #    cd /usr/arm-linux-gnueabihf/lib/;
    #    sudo ln -s $OLDPWD/src/.libs/libredwg.so.0;
    #    cd -
    #fi
    #$make -s -j4 -C examples check_PROGRAMS
    qemu-arm -L /usr/arm-linux-gnueabihf programs/dwgread test/test-data/sample_2000.dwg \
        || exit
    #for t in test/testcases/*.c; do
    #    b=$(basename $t .c)
    #    qemu-arm -L /usr/arm-linux-gnueabihf test/testcases/$b | tee test/$b.log
    #done
    make -s -j4 clean
fi

;;

esac

# features
CC="cc -m32" ./configure --disable-python && \
    $make -s -j4 check || exit
$make -s -j4 clean

./configure && \
    $make -s -j4 check || exit
    $make distcheck
$make -s -j4 clean

./configure --enable-trace && \
    $make -s -j4 check || exit
$make -s -j4 clean

./configure --disable-write && \
    $make -s -j4 check || exit
    $make check-valgrind
$make -s -j4 clean

./configure --disable-shared && \
    $make -s -j4 check || exit
$make -s -j4 clean

./configure --disable-python && \
    $make -s -j4 check || exit
$make -s -j4 clean

./configure --disable-dxf && \
    $make -s -j4 check || exit
$make -s -j4 clean

./configure --disable-bindings && \
    $make -s -j4 check || exit
$make -s -j4 clean

./configure --enable-debug && \
    $make -s -j4 check || exit
$make -s -j4 clean

WINEARCH=win32 WINEPREFIX=~/.win32 CFLAGS="-g -gdwarf-2" \
    ./configure --host=i686-w64-mingw32 && \
    $make -s -j4 check
$make -s -j4 clean

WINEARCH=win64 ./configure --host=x86_64-w64-mingw32 && \
    $make -s -j4 check
$make -s -j4 clean

# and at last keep the cfg for typical fast debugging
./configure --disable-bindings --disable-python --disable-shared --enable-debug \
            --disable-dxf --disable-write && \
    $make -s -j4 check || exit
