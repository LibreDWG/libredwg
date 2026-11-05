#!/usr/bin/env perl -an
if (/^LIBREDWG_SO_VERSION=([^\r\n]+)/) {
    print $1;
    exit;
}
