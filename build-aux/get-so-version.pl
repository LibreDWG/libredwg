#!/usr/bin/env perl -an
if (/^LIBREDWG_SO_VERSION=(.+)/) {
  print $1;
  exit;
}
