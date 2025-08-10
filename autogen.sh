#!/bin/sh
# autogen.sh
#
# Usage: sh autogen.sh [-f]
# Run this in the top directory to regenerate all the files.
# Option "-f" means forcefully create symlinks for missing files
# (by default: copies are made only if necessary).
#
# Tested with:
# - autoconf (GNU Autoconf) 2.65, 2.69, 2.72
# - automake (GNU automake) 1.12 - 1.17
# - ltmain.sh (GNU libtool) 2.2.6b - 2.5.4

cd "$(dirname "$0")" || exit 1
set -x
rm -rf autom4te.cache config.status
if test -d .git -a -f build-aux/git-version-gen; then
    git fetch --tags
    if [ -f .tarball-version ]; then
        rm .tarball-version
    fi
    v=$(build-aux/git-version-gen .tarball-version)
    echo $v >.version
fi

amver=$(automake --version | head -n1 | perl -lne'/ \d\.(\d+)/ && print $1')
if [ $amver -gt 0 -a $amver -lt 14 ]; then
    if [ $amver -lt 13 ]; then
        perl -p -i.bak -e 's/1.14 gnu no-define serial-tests dist-xz info-in-builddir/1.11 gnu no-define/' configure.ac
    else
        perl -p -i.bak -e 's/1.14 gnu no-define serial-tests dist-xz info-in-builddir/1.13 gnu no-define serial-tests dist-xz/' configure.ac
    fi
fi

set -e
autoreconf --install --symlink "$@" -I m4

if
    command -v git &
    >/dev/null
then
    git submodule update --init --recursive
else
    curl https://raw.githubusercontent.com/zserge/jsmn/master/jsmn.h -o jsmn/jsmn.h
fi

# autogen.sh ends here
