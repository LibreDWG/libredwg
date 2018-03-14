## Makefile.am
##
## Copyright (C) 2015 Free Software Foundation, Inc.
##
## This file is part of LibreDWG.
##
## LibreDWG is free software: you can redistribute it and/or modify it
## under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## LibreDWG is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty
## of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
## See the GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program.  If not, see <http://www.gnu.org/licenses/>.

LDADD = ../src/libredwg.la

# NB: After you modify ‘check_PROGRAMS’, do "make update-ignorance"
#     and include this directory's .gitignore in the commit.
check_PROGRAMS = \
 3dsolid \
 arc \
 attdef \
 attrib \
 block \
 body \
 circle \
 dim_aligned \
 dim_ang2ln \
 dim_ang3pt \
 dim_diameter \
 dim_linear \
 dim_ordinate \
 dim_radius \
 ellipse \
 endblk \
 insert \
 line \
 lwpline \
 minsert \
 mline \
 mtext \
 point \
 polyline_2d \
 polyline_3d \
 polyline_mesh \
 polyline_pface \
 ray \
 region \
 seqend \
 shape \
 solid \
 text \
 tolerance \
 trace \
 vertex_2d \
 vertex_3d \
 vertex_mesh \
 vertex_pface \
 xline

TESTS = $(check_PROGRAMS)
TESTS_ENVIRONMENT = \
  INPUT=$(srcdir)/example.dwg

update-ignorance:
	printf '/%s\n' $(check_PROGRAMS) > .gitignore

EXTRA_DIST = example.dwg

## Makefile.am ends here
