#****************************************************************************
#  LibreDWG - free implementation of the DWG file format                    *
#                                                                           *
#  Copyright (C) 2018 Free Software Foundation, Inc.                        *
#                                                                           *
#  This library is free software, licensed under the terms of the GNU       *
#  General Public License as published by the Free Software Foundation,     *
#  either version 3 of the License, or (at your option) any later version.  *
#  You should have received a copy of the GNU General Public License        *
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.    *
#****************************************************************************

# dwgiview.pl: DWG internal inspector UI (via perl5 and Tk),
#              requires perl-Tk and perl-Tk-MK
# written by Reini Urban

use strict;
use warnings;
use LibreDWG;
use Tk;
use TK::DTree;

my $WIN32 = 1 if $^O =~ /Win32/;
my @FONT = ($WIN32 ? (-font => 'systemfixed') : () );

my $win = Tk::MainWindow->new;
$win->title('dwgview');
# right text area
my $st = $win->Scrolled
  ('Text',
   -scrollbars => 'osoe',
   -wrap => 'none',
   -width => 80, -height => 25, @FONT);
my $t = $st->Subwidget('scrolled');
$st->pack(-fill => 'both', -expand => 'true');

# left tree
my $tree = $win->DTree;
#my $scroll = $tree->Scrollbar();
#$scroll->pack(-side => 'right', -fill => 'y');
$tree->pack;
$tree->update;
my $style = $win->ItemStyle
  ('text',
   -anchor => 'e',
   #-justify => 'right',
   #-wraplength => '6',
   #-background => 'yellow'
  );
$tree->configure( -datastyle => $style );
$tree->add
  ('i01', -data => 'i01',
   -itemtype => 'text',
   -text => 'DWG',
   #-image => $xpms{project_icon},
   -datastyle => $style);
$tree->update;


Tk::MainLoop();
