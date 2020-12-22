#!/usr/bin/env perl
=head1 DESCRIPTION

Check if test/unit-testing/Makefile.am contains all known objects (as in src/objects.inc)
Use -v for verbose ok messages

=cut

use strict;
use File::Basename;
my $v;
$v++ if shift eq '-v';
my $objfile = dirname(__FILE__)."/../../src/objects.inc";
my $makefile = dirname(__FILE__)."/Makefile.am";
my $fail;
open my $fo, "<", $objfile or die "$objfile not found: $!";
my %o;
while (<$fo>) {
  if (/^DWG_(?:ENTITY|OBJECT)\s+\((.+)\)/) {
    my $s = $1;
    if ($s =~ /^_/) { # _3DFACE, ...
      $s = substr($s, 1);
    }
    warn "ok $s in objects.inc\n" if $v;
    $o{ $s }++;
  }
}
close $fo;
# there must be at least 100 objects and makefile keys
my $len_o = scalar keys %o;
if ($len_o) {
  warn "ok - $len_o objects in objects.inc\n";
}
if ($len_o < 100) {
  warn "not ok only $len_o objects in objects.inc\n"; # fatal
  $fail++;
  exit 1 unless $len_o;
}
  
open my $fm, "<", $makefile or die "$makefile not found: $!";
my ($in, %m, $m);
while ( <$fm> ) {
  if ($in and /^check_PROGRAMS/) {
    $in = 0;
    last;
  }
  if (/^testobjects (\+)?=/) {
    $in = 1;
  }
  if ($in and /^\t+(\S+)/) {
    $m = $1;
    my $u = uc ($m);
    $m{ $u }++;
    warn "ok - $u in Makefile.am\n" if $v;
    if ( !exists $o{$u} && $m !~ /^(bits|common|decode|hash|dynapi|dxf|add)_test$/) {
      warn "ok - TODO $u not in objects.inc\n" ; # harmless
    }
  }
}
my $len_m = scalar keys %m;
if ($len_m) {
  warn "ok - $len_m objects in Makefile.am\n";
}
if ($len_m < 100) {
  warn "not ok - only $len_m keys in Makefile.am\n"; # fatal
  $fail++;
  exit 1 unless $len_m;
}
# all known objects must be in the Makefile
for (sort keys %o) {
  if ( !exists $m{$_} ) {
    warn "not ok - $_ not in Makefile.am\n"; # fatal
    $fail++;
  }
}
exit $fail;
