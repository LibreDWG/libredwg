#!/usr/bin/perl
# get all double values which are not found, and check for hints
# for a different binary repr.
# update bd-unknown.inc then
# usage: bd-unknown.pl|sort|uniq >bd-unknown.inc

for my $fn (<*.pi>) {
  my $o = $fn;
  $o =~ s{examples/}{};
  $o =~ s{\.pi}{};
  open my $f, $fn;
  while (<$f>) {
    if (/%new_struct\('_BD(\d+)', \[("[01]+"), '(.+?)', /) {
      printf "{ %-22s, %s }, // %s %s\n", "\"$3\"", $2, $1, $o;
    }
  }
}
