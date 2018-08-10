#!/usr/bin/perl
# get all double values which are not found, and check for hints
# for a different binary repr.
# update bd-unknown.inc then
# usage: examples/bd-unknown.pl|sort|uniq > examples/bd-unknown.inc

for my $fn (<examples/*.pi>) {
  my $log = "$fn.log";
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
