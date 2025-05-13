#!/bin/bash
#
# clang-format-all: a tool to run clang-format on an entire project
# Copyright (C) 2016 Evan Klitzke <evan@eklitzke.org>
# Copyright (C) 2019 Reini Urban <rurban@cpan.org>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

function usage {
    echo "Usage: $0 DIR..."
    exit 1
}

if [ $# -eq 0 ]; then
    usage
fi

# Variable that will hold the name of the clang-format command
FMT=""

# Some distros just call it clang-format. Others (e.g. Ubuntu) are insistent
# that the version number be part of the command. We prefer clang-format if
# that's present, otherwise we work backwards from highest version to lowest
# version. macports adds an -mp suffix and has newer versions, so prefer that.
for clangfmt in clang-format{-mp,}{-devel,-{20,18,15,8,7,6,5,4,3}{,.9,.8,.7,.6,.5,.4,.3,.2,.1,.0},}
do
    if which "$clangfmt" &>/dev/null; then
        FMT="$clangfmt"
        break
    fi
done

# Check if we found a working clang-format
if [ -z "$FMT" ]; then
    echo "failed to find clang-format"
    exit 1
fi

# Check all of the arguments first to make sure they're all directories
for dir in "$@"; do
    if [ ! -d "${dir}" ]; then
        echo "${dir} is not a directory"
        usage
    fi
done

# Find a dominating file, starting from a given directory and going up.
find-dominating-file() {
    if [ -r "$1"/"$2" ]; then
        return 0
    fi
    if [ "$1" = "/" ]; then
        return 1
    fi
    find-dominating-file "$(realpath "$1"/..)" "$2"
    return $?
}

# Run clang-format -i on all of the things
for dir in "$@"; do
    if [ X$dir = Xbindings ]; then
        echo skip bindings
        continue
    fi
    pushd "${dir}"
    if ! find-dominating-file . .clang-format; then
        echo "Failed to find dominating .clang-format starting at $PWD"
        continue
    fi
    find . \
         \( -name '*.c' \
         -o -name '*.h' \) \
         -a \! \( -path './bindings/*' \
               -o -path './codepages/*' \
               -o -name dwg.h \
               -o -name dwg_api.h \
               -o -name dwg_api.c \
               -o -name config.h \
               -o -name objects.c \
               -o -name dxfclasses.c \
               -o -name dynapi_test.c \
               -o -name dynapi.c \) \
         -exec "${FMT}" -i -verbose '{}' \;
    echo "post clang-format fixups (clang-format bugs)"
    sed -i -e's, (-Wformat - nonliteral) , (-Wformat-nonliteral)   ,;' \
           -e's, (-Wformat - nonliteral), (-Wformat-nonliteral),;' \
           -e's, (-Wmissing - prototypes), (-Wmissing-prototypes),;' \
           -e's, (-Wdeprecated - declarations), (-Wdeprecated-declarations),;' \
           -e's, (-Wpragma - pack), (-Wpragma-pack),;' \
           -e's, (-Wswitch - enum), (-Wswitch-enum),;' \
           -e's, (-Wformat - y2k), (-Wformat-y2k),;' \
           -e's, (-Wmaybe - uninitialized), (-Wmaybe-uninitialized),;' \
           -e's, (-Wstringop - truncation), (-Wstringop-truncation),;' \
           -e's, (-Wstringop - overflow), (-Wstringop-overflow),;' \
           -e's, (-Wanalyzer - allocation - size) , (-Wanalyzer-allocation-size)   ,;' \
           -e's, (-Wanalyzer - possible - null - dereference), (-Wanalyzer-possible-null-dereference),;' \
           -e's, (-Wanalyzer - null - dereference), (-Wanalyzer-null-dereference),;' \
           -e's, (-Wanalyzer - malloc - leak), (-Wanalyzer-malloc-leak),;' \
        `grep -l 'DIAG_IGNORE' */*.{c,h} *.{c,h}`
    if [ X$dir = Xexamples ]; then
        sed -i -e's/define SZ , 119/define SZ ,119/' dwgadd.c
    fi
    popd &>/dev/null
done
