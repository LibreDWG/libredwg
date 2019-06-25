@echo off

cd %APPVEYOR_BUILD_FOLDER%
set MSYS2_DIR=msys64

echo ARCH: %ARCH%
echo Compiler: %COMPILER%
echo MSYS2_ARCH: %MSYS2_ARCH%
echo Platform: %PLATFORM%
echo MSYS2 directory: %MSYS2_DIR%
echo MSYS2 system: %MSYSTEM%

echo Start updating build dependencies...

echo Installation MSYS2 build preconditions...

echo Extending path to MSYS...
SET "PATH=C:\%MSYS2_DIR%\%MSYSTEM%\bin;C:\%MSYS2_DIR%\usr\bin;%PATH%"

echo Updating pacman...
bash -lc "pacman -S --needed --noconfirm pacman-mirrors"
bash -lc "pacman -Syyu --noconfirm"
rem need it twice, really
bash -lc "pacman -Syyu --noconfirm"

rem echo Installing git...
rem bash -lc "pacman -S --needed --noconfirm git"

rem echo Removing outdated conflicts...
rem mingw-w64-x86_64-pdcurses and mingw-w64-x86_64-ncurses are in conflict...
rem bash -lc "pacman -R --noconfirm --nodeps mingw-w64-%MSYS2_ARCH%-python3 mingw-w64-%MSYS2_ARCH%-readline mingw-w64-%MSYS2_ARCH%-sqlite3 mingw-w64-%MSYS2_ARCH%-ncurses mingw-w64-%MSYS2_ARCH%-termcap"

echo Installing dependencies...
rem see https://github.com/Alexpux/MSYS2-packages
bash -lc "pacman -S --needed --noconfirm mingw-w64-%MSYS2_ARCH%-toolchain mingw-w64-%MSYS2_ARCH%-libtool mingw-w64-%MSYS2_ARCH%-libiconv mingw-w64-%MSYS2_ARCH%-libxml2 mingw-w64-%MSYS2_ARCH%-swig mingw-w64-%MSYS2_ARCH%-python2 texinfo dejagnu mingw-w64-%MSYS2_ARCH%-perl mingw-w64-%MSYS2_ARCH%-pcre2"
rem autoconf-2.69 automake-wrapper
