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
if (/Warning: (?:Unhandled|Unstable) Class (?:object|entity) \d+ (\w+) /) {
  $object = $1; next;
}
if (/object UNKNOWN_OBJ bitsize: (.\d+)/) {
  $bitsize = $1; next;
}
if (/entity UNKNOWN_ENT Entity bitsize: (.\d+)/) {
  $bitsize = $1; next;
}
if (/ \(bitsize: (.\d+), hdlpos/) {
  $bitsize = $1; next;
}
if (/ Decode (?:object|entity) $object bitsize: (.\d+)$/) {
  $bitsize = $1; next;
}
if (/^unknown_bits \[(\d+) \((\d),(\d+)\) TF\]: ([0-9a-f]+\n)/) {
  ($num_bytes, $pre_bits, $num_bits, $b) = ($1, $2, $3, $4);
  chomp $b; next;
}
if (/Next object: /) {
  if ($object and $b and $handle && $num_bits) {
    #duplicate bytes, skipping dxf and handle are done in log_unknown_dxf.pl
    my $dxf = $ARGV; # cvt log to dxf
    my ($n, $d) = $dxf =~ /^(.*)_(r\d+|R?20\d\d)\.log$/;
    if ($n =~ /^(example|sample|Drawing|DS_li)/) {
      $dxf = "test/test-data/${n}_${d}.dxf";
      unless (-f $dxf) {
        $dxf = "test/test-data/$d/$n.dxf";
      }
    } else {
      $dxf = "test/test-data/$d/$n.dxf";
      unless (-f $dxf) {
        $dxf = "test/test-data/${n}_${d}.dxf";
      }
    }
    next if $dxf =~ /work\.orig/; # skip temp. duplicates
    $dxf = undef unless -f $dxf;
    printf "    { \"$object\", \"$b\", \"$ARGV\", %s, 0x$handle, $pre_bits, $num_bits },\n",
      $dxf ? "\"$dxf\"" : "NULL";
    $object = $bitsize = $b = $bits = $handle = undef;
    $num_bytes = $pre_bits = $num_bits = undef;
  }
}
