#!/usr/bin/perl -n
=head1 USAGE

    make -C examples alldwg.inc

    static struct _unknown {
      const char *name;
      const char *bytes;
      const char *bits;
      const char *log; const char *dxf;
      const unsigned int handle; const int bitsize;
    } unknowns[];

=cut

if (/handle: .+\..+\.(.+) \[5\]\n/) {
  $handle = $1; next;
}
if (/Warning: Unhandled Class (?:object|entity) \d+ (\w+) /) {
  $object = $1; next;
}
if (/object UNKNOWN_OBJ bitsize: (.\d+) /) {
  $bitsize = $1; next;
}
if (/entity UNKNOWN_ENT Entity bitsize: (.\d+) /) {
  $bitsize = $1; next;
}
if (/ \(bitsize: (.\d+), hdlpos/) {
  $bitsize = $1; next;
}
if (/^([0-9a-f]+\n)/) {
  $b = $1;
  chomp $b; next;
}
if (/^bits\[\d\]: (\d)/) {
  $bits .= $1; next;
}
if (/Next object: /) {
  if ($object and $b and $handle && $bitsize) {
    my $dxf = $ARGV;
    my ($n, $d) = $dxf =~ /^(.*)_(r\d+|R?20\d\d)\.log$/;
    if ($n =~ /^(example|sample|Drawing|DS_li)/) {
      $dxf = "test/test-data/${n}_${d}.dxf";
    } else {
      $dxf = "test/test-data/$d/$n.dxf";
    }
    $dxf = undef unless -f $dxf;
    printf "    { \"$object\", \"$b\", \"$bits\", \"$ARGV\", %s, 0x$handle, $bitsize },\n",
      $dxf ? "\"$dxf\"" : "NULL";
    $object = $bitsize = $b = $bits = $handle = undef;
  }
}
