@echo off

cd %APPVEYOR_BUILD_FOLDER%
echo ARCH: %ARCH%

rem TODO cygwin pkg and src tar.xz
if "%CYGWIN%" == "1" exit

rem simple windows .zip with bins and pdf in root
set DESTDIR=libredwg-%appveyor_build_version%-%ARCH%
if not "x%APPVEYOR_REPO_TAG_NAME%" == "x" set DESTDIR=libredwg-%APPVEYOR_REPO_TAG_NAME%-%ARCH%
echo DESTDIR: %DESTDIR%

rem TODO install-pdf broken
bash -c "make install prefix= bindir= pdfdir= DESTDIR=`pwd`/%DESTDIR%"
cd %DESTDIR%
copy ..\README README.txt
if "%MINGW%"=="1" 7z a ../%DESTDIR%.zip *
rem if "%MINGW%"=="1" appveyor PushArtifact ../%DESTDIR%.zip *
cd ..
