#!/usr/bin/perl -p
# Filter an ASCII DXF to appear similar to a native DXF.
#
# For now normalizes floating point numbers:
# The msvcrt sprintf runtime prints different ending zero's, not
# do-able with a POSIX sprintf, even not with the various alternate %G %F variants.

die "Usage: diff -bu <(./filt_dxf.pl dxf1) <(./filt_dxf.pl dxf2)\n" if @ARGV;

if (/(-?[0-9]+\.[0-9]+(?:E\+[0-9]+)?)/) {
  s/^(\Q$1\E)/sprintf("%.06f",$1)/e;
}

# 1.1000000 => 1.1 no ending zero if it doesn't end with zero
#s/^(-?[0-9]+\.[1-9]+)0+\r?$/$1/;

# 1.010000 => 1.01
#s/^(-?[0-9]+\.[0-9]*[1-9]+)0+\r?$/$1/;

# 0.0000000 => 0.0 keep an ending zero if there's nothing else
#s/^(-?[0-9]+)\.0+\r?$/$1.0/;

# max 6 after-comma places
#s/^(-?[0-9]+)\.([0-9]{6}).+\r?$/$1.$2/;
