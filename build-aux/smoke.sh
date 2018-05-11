#!/bin/sh

find -name .deps -type d -exec rm -rf '{}' 2>/dev/null \;
sh autogen.sh
# autoreconf
make=make

case `uname` in
Darwin) # macports compilers
    make=gmake

export PKG_CONFIG_PATH=/opt/local/lib/pkgconfig
gmake -s clean
echo clang-mp-5.0 -fsanitize=address,undefined -fno-omit-frame-pointer
CC="clang-mp-5.0 -fsanitize=address,undefined -fno-omit-frame-pointer" \
    ./configure --enable-trace --enable-write && \
    gmake -s -j4 check || exit
gmake -s clean
echo clang-mp-6.0 -O3 -march=native
CC="clang-mp-6.0 -O3 -march=native" \
    ./configure --enable-write && \
    gmake -s -j4 check || exit
gmake -s clean
echo clang-mp-devel
CC="clang-mp-devel" \
    ./configure && \
    gmake -s -j4 check || exit
gmake -s clean
for CC in gcc-mp-4.3 gcc-mp-4.6 gcc-mp-4.8 gcc-mp-4.9 gcc-mp-5 gcc-mp-6 gcc-mp-7; do
    echo $CC
    ./configure && \
    gmake -s -j4 check || exit
    gmake -s clean
done

# port install arm-elf-gcc (with newlib, not glibc)
if [ -e /opt/local/bin/arm-elf-gcc-4.7 ]; then
    echo arm-elf-gcc-4.7 --host=arm-elf --disable-shared
    CC=arm-elf-gcc-4.7 ./configure --host=arm-elf --disable-shared && \
        gmake -s -j4 || \
        (gmake -s -C src out_dxf.i && mv src/out_dxf.i src/out_dxf.c && gmake -s);
    $make -s -j4 -C examples check_PROGRAMS
    #qemu-arm -L /opt/local/arm-elf programs/dwgread test/test-data/sample_2000.dwg || exit
fi

;;

Linux)
make -s -j4 clean

echo clang-3.9 -fsanitize=address -fno-omit-frame-pointer
CC="clang-3.9 -fsanitize=address -fno-omit-frame-pointer" \
    ./configure --enable-write && \
    make -s -j4 check || exit
make -s -j4 clean
echo clang-5.0 -march=native
CC="clang-5.0 -march=native" \
    ./configure && \
    make -s -j4 check
make -s -j4 clean
echo clang-4.0 -std=c99
CC="clang-4.0 -std=c99" \
    ./configure  && \
    make -s -j4 check || exit
make -s -j4 clean
echo gcc-4.4 -ansi
CC="gcc-4.4 -ansi" ./configure && \
    make -s -j4 check || exit
make -s -j4 clean
CC="gcc-6" ./configure && \
    make -s -j4 check || exit
make -s -j4 clean
CC="gcc-7" ./configure && \
    make -s -j4 check || exit
make -s -j4 clean
CC="clang-7 -fsanitize=address,undefined -fno-omit-frame-pointer" \
    ./configure --enable-write --enable-trace && \
    make -s -j4 check || exit
make -s -j4 clean

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

CC="cc -m32" ./configure && \
    $make -s -j4 check || exit
$make -s -j4 clean
./configure && \
    $make -s -j4 check || exit
    $make distcheck
$make -s -j4 clean
./configure --enable-trace && \
    $make -s -j4 check || exit
$make -s -j4 clean
./configure --enable-write && \
    $make -s -j4 check || exit
$make -s -j4 clean
./configure --disable-shared && \
    $make -s -j4 check || exit
$make -s -j4 clean

WINEARCH=win32 CFLAGS="-g -gdwarf-2" \
    ./configure --host=i686-w64-mingw32 && \
    $make -s -j4 check
    $make -s -j4 clean
WINEARCH=win64 ./configure --host=x86_64-w64-mingw32 && \
    $make -s -j4 check
    $make -s -j4 clean

./configure --enable-write --disable-shared && \
    $make -s -j4 check || exit
