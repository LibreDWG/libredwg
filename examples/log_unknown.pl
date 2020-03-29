#!/usr/bin/perl -n
=head1 USAGE

    make -C examples alldwg.inc

=head1 DESCRIPTION

Creates an C include file with all UNKNOWN_BITS of all unstable and
debugging objects in all found DWG's.

=cut
no strict;
my $td = "test/test-data";
my $tb = "test/test-big";

# also triggered by objectmap (print) and free
if (/ Hdlsize: (\d+)[, ]/) {
  $hdlsize = $1; next;
}
if (/^Warning: (?:Unhandled|Unstable) Class (object|entity) \d+ (\w+) /) {
  print "//log=$ARGV object=$2\n";
  $is_entity = $1 eq "entity" ? 1 : 0;
  $object = $2; next;
}
next unless $object;

if (/ bitsize: (\d+), hdl_dat/) {
  print "//bitsize computed=$1\n";
  $bitsize = $1; next;
}
elsif (/^bitsize: (\d+) /) {
  print "//bitsize decoded=$1\n";
  $bitsize = $1; next;
}
elsif (/handle: 0\.\d+\.([0-9A-F]+) \[H 5\]$/) {
  print "//handle=$1\n";
  $handle = $1; next;
}
next unless $bitsize and $handle;

if (/^unknown_bits \[(\d+) \((\d+),(-?\d+),(\d+)\) (\d+) TF\]: ([0-9A-F]+$)/) {
  ($num_bits, $commonsize, $hdloff, $strsize, $len, $b) = ($1, $2, $3, $4, $5, $6);
  print "//offsets=$num_bits, $commonsize, $hdloff, $strsize, $hdlsize. len=$len\n";
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
    $dxf = "$td/${n}_${d}.dxf";
    unless (-f $dxf || -f "../$dxf") {
      $dxf = "$td/$d/$n.dxf";
    }
  } else {
    $dxf = "$td/$d/$n.dxf";
    if (!-f "../$dxf" && -f "../$tb/$d/$n.dxf") {
      $dxf = "$tb/$d/$n.dxf";
    }
    unless (-f $dxf || -f "../$dxf") {
      $dxf = "$td/${n}_${d}.dxf";
    }
  }
  next if $dxf =~ /work\.orig/; # skip temp. duplicates
  unless (-f $dxf || -f "../$dxf") {
    warn "//$dxf not found\n";
    $dxf = undef;
  }
  printf "    { \"$object\", \"$ARGV\", 0x$handle, \"$b\", %s, ".
    "$is_entity, $num_bits, $commonsize, $hdloff, $strsize, $hdlsize, $bitsize },\n",
    $dxf ? "\"$dxf\"" : 'NULL';
  $object = $b = $handle = undef;
  $num_bits = $commonsize = $hdloff = $strsize = $hdlsize = $bitsize = $is_entity = 0;
}
