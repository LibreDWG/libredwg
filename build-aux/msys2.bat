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

rem echo Installing git...
rem bash -lc "pacman -S --needed --noconfirm git"

echo Installing dependencies...
rem see https://github.com/Alexpux/MSYS2-packages
bash -lc "pacman -S --needed --noconfirm mingw-w64-%MSYS2_ARCH%-toolchain mingw-w64-%MSYS2_ARCH%-libtool mingw-w64-%MSYS2_ARCH%-libiconv mingw-w64-%MSYS2_ARCH%-libxml2 mingw-w64-%MSYS2_ARCH%-swig mingw-w64-%MSYS2_ARCH%-python2 texinfo dejagnu"
rem autoconf-2.69 automake-wrapper
