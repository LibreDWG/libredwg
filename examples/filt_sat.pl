#!/usr/bin/perl -p
# normalizes ASCII floating point numbers

die "Usage: diff -bu <(./filt_sat.pl sat1) <(./filt_sat.pl sat2)\n" if @ARGV;

s/(-?[0-9]+\.[0-9]+(?:E\+[0-9]+)?)/sprintf("%.08f",$1)/ge;
