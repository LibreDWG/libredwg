#!/usr/bin/perl -n
=head1 USAGE

    make -C examples alldwg.inc

=cut

# also triggered by objectmap (print) and free
if (/^Warning: (?:Unhandled|Unstable) Class (?:object|entity) \d+ (\w+) /) {
  print "//log=$ARGV object=$1\n";
  $object = $1; next;
}
next unless $object;

if (/ \(bitsize: (.\d+), hdlpos/) {
  print "//bitsize hdlpos=$1\n";
  $bitsize = $1; next;
}
elsif (/ Decode (?:object|entity) \w+ bitsize: (\d+)/) {
  print "//bitsize decode=$1\n";
  $bitsize = $1; next;
}
elsif (/^bitsize: (\d+) \[RL\]/) {
  print "//bitsize r14=$1\n";
  $bitsize = $1; next;
}
elsif (/handle: 0\.\d+\.([0-9A-F]+) \[5\]$/) {
  print "//handle=$1\n";
  $handle = $1; next;
}
next unless $bitsize and $handle;

if (/^unknown_bits \[(\d+) \((\d+),(-?\d+),(\d+)\) TF\]: ([0-9a-f]+$)/) {
  ($num_bits, $commonsize, $hdloff, $strsize, $b) = ($1, $2, $3, $4, $5);
  print "//offsets=$num_bits, $commonsize, $hdloff, $strsize\n";
  chomp $b; next;
}
next unless $b;

if (/Next object: / or /^Num objects:/) {
  unless ($object and $b and $handle and $num_bits) {
    warn "empty object=$object, b=$b, handle=$handle, num_bits=$num_bits"; next;
  }
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
  printf "    { \"$object\", \"$ARGV\", \"$b\", %s, 0x$handle, ".
    "$num_bits, $commonsize, $hdloff, $strsize, $bitsize },\n",
    $dxf ? "\"$dxf\"" : "NULL";
  $object = $b = $handle = undef;
  $num_bits = $commonsize = $hdloff = $strsize = $bitsize = 0;
}
