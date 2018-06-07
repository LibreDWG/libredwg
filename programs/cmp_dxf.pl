#!/usr/bin/perl
# usage: diff -bu dxf1 dxf2 | cmp_dxf
# compares two ascii dxf files, and compares it line by line,
# adjusting for different %g/%f formatting, and some
# volatile elements.

use strict;
$\ = " ";
die "Usage: diff -bu dxf1 dxf2 | cmp_dxf\n" if @ARGV;

sub isfloat {
  my $g = shift;
  return ($g >= 10  && $g <= 59) ||
         ($g >= 110 && $g <= 149) ||
         ($g >= 210 && $g <= 239) ||
         ($g >= 460 && $g <= 469) ||
         ($g >= 1060 && $g < 1070);
}
sub isstr {
  my $g = shift;
  return ($g >= 0  && $g < 5) ||
         ($g >= 6 && $g <= 9) ||
         ($g >= 100 && $g <= 104) ||
         ($g >= 300 && $g <= 319) ||
         ($g >= 410 && $g <= 419) ||
         ($g >= 430 && $g <= 439) ||
         ($g >= 470 && $g <= 479) ||
         ($g >= 999 && $g <= 1009);
}

# open my $f, "<:crlf", $ARGV[0] or die "Failed to read diff: $!\n";
my $var;
while (<>) {
  chomp;
  $var = $_  if /^ \$(\w+)/;
  next if /^ /;
  next if /^\+999/;
  next if /^\+LibreDWG /;
  #next if /^@@/;
  next if /^[\+\-]\d+\.\d+/;
  print "$_\n";
  # ignore comments
  #if ($a =~ /^999/) { <$d1>; $a = <$d1>; chomp $a }
  #if ($b =~ /^999/) { <$d2>; $b = <$d2>; chomp $b }
  #if ($a != $b) { print "$.: DXF $a <> $b\n"; next }

  #my $v1 = <$d1>; my $v2 = <$d2>;
  #chomp $v1; chomp $v2;
  #if (isfloat($a)) {
  #  if (abs($v1 - $v2) > 1e-6) { print "$.: $a $v1 <> $v2\n"; next }
  #}
  #elsif (isstr($a)) {
  #  if ($v1 ne $v2) { print "$.: $a \"$v1\" <> \"$v2\"\n"; next }
  #} else {
  #  if ($v1 != $v2) { print "$.: $a $v1 <> $v2\n"; next }
  #}
}
