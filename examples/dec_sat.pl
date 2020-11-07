#!/usr/bin/perl
my $s = join (" ", @ARGV);
$s =~ s/\^ /\^/g;
for (split //, $s) {
  printf ("%s", ord($_) <= 32 ? $_ : chr(159 - ord($_)));
}
