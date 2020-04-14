#!/usr/bin/env perl
=head1 SYNOPSIS

  cmp-objs.pl example_2000.log.orig example_2000.log
  cmp-objs.pl example_2000.log
  cmp-objs.pl

=head1 DESCRIPTION

Needs to two logfiles, one from the original dwg, and one from the created dwg.

Searches for Object number: 17/11, Size: 280 [MS], Type: 57 [BS], Address: 132451
and compares via xxd the obj at offset 132451, length 282 (incl CRC)
with the same object on the other logfile.

Prints all binary differences.

=cut

use strict;
use warnings;
my ($l1, $l2, $dwg1, $dwg2, $o1, $o2, $s2, $name1, $line1, $done);
if (@ARGV == 1) {
  $l2 = shift;
  $l1 = "$l2.orig";
} elsif (@ARGV == 2) {
  $l1 = shift;
  $l2 = shift;
} elsif (@ARGV == 0) {
  $l1 = "example_2000.log.orig";
  $l2 = "example_2000.log";
}
open my $f1, "<", $l1 or die "$l1 $!";
open my $f2, "<", $l2 or die "$l2 $!";
# get the dwg from the logfile
while (<$f1>) {
  if (!$dwg1 && /^Reading DWG file (.+\.dwg)$/) {
    $dwg1 = $1;
    next;
  }
  $done = 0;
  if (/^Object number: (\d+)\/(\w+), Size: (\d+) \[MS\], Type: (\d+) \[BS\], Address: (\d+)$/) {
    my ($num, undef, $size, $type, $o1) = ($1, $2, $3, $4, $5);
    my $line1 = $_;
    if (!$dwg1) {
      die "Reading DWG file not found";
    }
    $_ = <$f1>;
    if (/^Add (object|entity) (\w+ \[\d+\]) /) {
      $name1 = $2;
    }
    # search the same in f2
    while (my $line2 = <$f2>) {
      if (!$dwg2 && $line2 =~ /^Reading DWG file (.+\.dwg)$/) {
        $dwg2 = $1;
        next;
      }
      if ($line2 =~ /^Object number: (\d+)\/(\w+), Size: (\d+) \[MS\], Type: (\d+) \[BS\], Address: (\d+)$/) {
        my ($n2, undef, $size2, $type2, $o2) = ($1, $2, $3, $4, $5);
        if (!$dwg2) {
          die "Reading DWG file not found";
        }
        <$f2>;
        # TODO we really should be comparing same handles, not just nearby same types
        if ($type == $type2 && abs($size - $size2) <= 4 && abs($num - $n2) < 10) {
          my $size1 = $size + 2; # including the crc
          $size2 += 2;           # including the crc
          # needs bash, not dash
          # with od it would be: `od -A x -t x1 -j $offset -N $size $file`
          my $cmd1 = "diff <(xxd -s $o1 -l $size1 \"$dwg1\" | cut -c11-)"
                       . " <(xxd -s $o2 -l $size2 \"$dwg2\" | cut -c11-)";
          my $cmd2 = "diff -bu <(xxd -s $o1 -l $size1 \"$dwg1\" | cut -c11-50)"
                           . " <(xxd -s $o2 -l $size2 \"$dwg2\" | cut -c11-50)";
          my $cmd3 = "wdiff <(xxd -s $o1 -l $size1 \"$dwg1\" | cut -c11-50)"
                        . " <(xxd -s $o2 -l $size2 \"$dwg2\" | cut -c11-50)";
          if (system ("bash -c \"$cmd1 >/dev/null\"")) {
            print "$name1\n";
            print "<".$line1;
            print ">".$line2;
            print "$cmd2\n";
            system ("bash -c \"$cmd2\"");
            print "$cmd3\n";
            system ("bash -c \"$cmd3\"");
            print "\n";
          }
          $done = 1;
          last;
        }
        elsif ($type == $type2 && $n2 > $num + 5) {
          warn "Object number: $num / $n2, Size $size / $size2, Type $type bypassed in $dwg2\n";
          seek $f2, 0, 0; # restart
          last;
        }
      }
    }
    if (!$done) {
      warn "Object number: $num, Size $size, Type $type not found in $dwg2\n";
      seek $f2, 0, 0; # restart
    }
  }
}
