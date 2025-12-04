#!/bin/bash
#
# clang-format: a tool to run clang-format on files
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
    echo "Usage: $0 FILE..."
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

# Check all of the arguments first to make sure they're all files
for file in "$@"; do
    if [ "$file" = "-style=file" ]; then
        continue
    fi
    if [ ! -e "${file}" ]; then
        echo "${file} not found"
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
for file in "$@"
do
    if [ "$file" = "-style=file" ]; then
        continue
    fi
    dir="$(dirname "$file")"
    if [ "$dir" = "bindings" ]; then
        echo skip bindings
        continue
    fi
    if [ "$file" = "src/dynapi.c" ] || \
       [ "$file" = "test/unit-testing/dynapi_test.c" ] || \
       [ "$file" = "src/dxfclasses.c" ] || \
       [ "$file" = "src/objects.c" ] || \
       [ "$dir" = "src/codepages" ]
    then
        echo "skip generated $file"
        continue
    fi
    # pushd "${dir}" || exit
    if ! find-dominating-file "$dir" .clang-format; then
        echo "Failed to find dominating .clang-format starting at $PWD"
        continue
    fi
    ${FMT} -i -verbose "$file"
    if grep -q 'IGNORE (-W' "$file"; then
        # shellcheck disable=SC2046,2035
        sed -e's, (-Wformat - nonliteral) , (-Wformat-nonliteral)   ,;' \
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
            <"${file}" >"${file}.new"
        if ! cmp -s "${file}" "${file}.new"; then
            echo "post clang-format fixups (clang-format bugs)"
            mv "${file}.new" "${file}"
        else
            rm -f "${file}.new"
        fi
    fi
    if [ "$file" = "examples/dwgadd.c" ]; then
        if grep -q 'define SZ , 119' "$file"; then
            echo "post clang-format fixups (clang-format bugs)"
            sed -i -e's/define SZ , 119/define SZ ,119/' examples/dwgadd.c
        fi
    fi
done
