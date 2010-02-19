# snap.GNUmakefile
#
# Copyright (C) 2010 Free Software Foundation, Inc.
#
# This program is free software, licensed under the terms of the GNU
# General Public License as published by the Free Software Foundation,
# either version 3 of the License, or (at your option) any later version.
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# Author: Thien-Thi Nguyen
##
# Usage: make -f snap.GNUmakefile [COMMAND]
# Default command is "help".
#
# Typically you will iterate on "full" and/or "pretty" until you are
# satisfied with the resulting tarball and index.html, then do one
# "stage pubdir=/PATH/TO/STAGING/DIRECTORY/", then finally "clean".
#
#  NOTE: This program does not handle copying the files from the
#        pubdir to the Internet.  Use rsync, scp, etc, for that.
#
# These variables affect various commands:

updpol		:= snap.update-policy
ml-addr		:= libredwg@gnu.org
source-repo	:= git://git.sv.gnu.org/libredwg.git
cgit		:= http://git.savannah.gnu.org/cgit/libredwg.git/commit/?id
project		:= http://savannah.gnu.org/projects/libredwg/
dir		:= libredwg-$(shell date +'%Y-%m-%d')
cruft		:= autom4te.cache/ .git/ .gitignore
pubdir		:=

# The contents of the optional file $(updpol) should be something like:
# "This is updated every few days or whenever ttn feels like it."
# (without the quotes).  This file is inlined into index.html as is,
# so actually any valid HTML is ok.
#
# Here are the supported commands:
##

help:
	@( me=$(lastword $(MAKEFILE_LIST)) ;			\
	   sed '/^##/,/^##/!d;/^##/d;s/^# //g;s/^#$$//g' $$me ;	\
	   sed '/^[a-z]*:/!d;s/^/ /;s/:.*//' $$me )

full: snapshot regen decruft tarball zonk pretty

snapshot:
	git clone $(source-repo) $(dir)

regen:
	cd $(dir) && git show-ref -s origin/master > autogen.log
	cd $(dir) && sh autogen.sh >> autogen.log 2>&1

decruft:
	cd $(dir) ; rm -rf $(cruft)

bz2 := $(dir).tar.bz2

tarball:
	rm -f $(bz2)
	tar cjhf $(bz2) $(dir)

zonk:
	find . -type d -name 'libredwg-*' | xargs rm -rf

# This is a normal variable assignment (using "=", not ":=")
# because we want it expanded at runtime, not at read-time.
commit = $(shell tar xjf $(bz2) -O $(dir)/autogen.log | sed 1q)

pretty:
	@echo Creating index.html
	@( echo '<html><head><title>LibreDWG Snapshot</title></head>' ;	\
	   echo '<body>' ;						\
	   echo '<h1>LibreDWG Snapshot</h1>' ;				\
	   echo '<p><a href="$(bz2)">$(bz2)</a>' ;			\
	   echo ' is an automatically prepared snapshot of the' ;	\
	   echo ' <a href="$(project)">LibreDWG</a> repository.' ;	\
	   if [ -r $(updpol) ] ; then cat $(updpol) ; fi ;		\
	   echo ' Preparation means we did:</p>' ;			\
	   echo '<pre>' ;						\
	   echo 'git show-ref origin/master > autogen.log' ;		\
	   echo 'sh autogen.sh >> autogen.log' ;			\
	   echo '# (with tool versions)' ;				\
	   for tool in autoconf automake libtool ; do			\
	     $$tool --version | sed 's/^/#   /;1q' ;  done ;		\
	   echo 'rm -rf $(cruft)' ;					\
	   echo '</pre>' ;						\
	   echo '<p>This means you do not need Autoconf, Automake' ;	\
	   echo ' or Libtool to proceed.  On the other hand, if you' ;	\
	   echo ' <b>do</b> have these tools (later versions better)' ;	\
	   echo ' installed, why not hack on the actual repository' ;	\
	   echo ' <a href="$(cgit)=$(commit)">directly</a>' ;		\
	   echo ' (with "git clone $(source-repo)")?</p>' ;		\
	   echo '<p>To play, make sure you have a C compiler and' ;	\
	   echo ' a "make" program installed, then do:</p>' ; 		\
	   echo '<pre>' ;						\
	   echo 'tar xjf $(bz2)' ;					\
	   echo 'cd $(dir)' ;						\
	   echo './configure [OPTIONS]' ;				\
	   echo 'make' ;						\
	   echo '</pre>' ;						\
	   echo '<p><b>Note:</b> We expect the "configure" to work' ;	\
	   echo ' (please <a href="mailto:$(ml-addr)">report</a>' ;	\
	   echo ' any problems you encounter), but the "make" might' ;	\
	   echo ' not succeed. </p>' ;					\
	   echo '</body></html>' ) > index.html

interesting := index.html libredwg-*.tar.bz2

stage:
	@test "$(pubdir)" || { echo 'ERROR: pubdir not set' ; false ; }
	cp -p $(interesting) "$(pubdir)"

clean: zonk
	rm -f $(interesting)

# snap.GNUmakefile ends here
