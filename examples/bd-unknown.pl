#!/usr/bin/perl
# get all double values which are not found (and even found), and check for hints
# for a different binary repr.
# update bd-unknown.inc then
# usage: bd-unknown.pl|sort|uniq >bd-unknown.inc

for my $fn (<*.pi>) {
  my $o = $fn;
  $o =~ s{examples/}{};
  $o =~ s{\.pi}{};
  open my $f, $fn;
  while (<$f>) {
    if (/new_struct\('_?[RB]D(\d+)', \[("[01]+"), '(.+?)', /) {
      my ($name, $bits, $val) = ($1, $2, $3);
      my $len = length($bits) - 2;
      if ($len == 66 || $len == 64) {
        printf "{ %-22s, %s }, // %s %s\n", "\"$val\"", $bits, $name, $o;
      #} else {
      #  warn "skip imprecise $name length ", $len if $len > 2;
      }
    }
  }
}
