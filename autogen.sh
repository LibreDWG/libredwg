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
if test -d .git; then git fetch --tags; test -f .tarball-version && rm .tarball-version; fi
test -f .version || git describe --long --tags --always > .version
set -e
autoreconf --install --symlink "$@" -I m4

# autogen.sh ends here
