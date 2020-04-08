# autogen.sh
#
# Usage: sh autogen.sh [-f]
# Run this in the top directory to regenerate all the files.
# Option "-f" means forcefully create symlinks for missing files
# (by default: copies are made only if necessary).
#
# Tested with:
# - autoconf (GNU Autoconf) 2.65, 2.69
# - automake (GNU automake) 1.14 - 1.16.1
# - ltmain.sh (GNU libtool) 2.2.6b - 2.4.6

set -x
rm -rf autom4te.cache config.status
if test -d .git -a -f build-aux/git-version-gen
then
    git fetch --tags
    rm .tarball-version
    v=`build-aux/git-version-gen .tarball-version`
    echo $v >.tarball-version
    echo $v >.version
fi
set -e
autoreconf --install --symlink "$@" -I m4

if [ -x "$(which git)" ]; then
    git submodule update --init --recursive
else
    curl https://raw.githubusercontent.com/zserge/jsmn/master/jsmn.h -o jsmn/jsmn.h
fi

# autogen.sh ends here
