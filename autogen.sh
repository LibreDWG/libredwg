# autogen.sh
#
# Usage: sh autogen.sh [-f]
# Run this in the top directory to regenerate all the files.
# Option "-f" means forcefully create symlinks for missing files
# (by default: copies are made only if necessary).
#
# Tested with:
# - autoconf (GNU Autoconf) 2.65
# - automake (GNU automake) 1.11.1
# - ltmain.sh (GNU libtool) 2.2.6b

set -x
rm -rf autom4te.cache config.status
if test -d .git; then
    git fetch --tags
    rm .tarball-version
    v=`build-aux/git-version-gen .tarball-version`
    echo $v >.tarball-version
    echo $v >.version
fi
set -e
autoreconf --install --symlink "$@" -I m4

# autogen.sh ends here
