#!/usr/bin/perl
=head1 USAGE

    perl examples/log_unknown_dxf.pl examples/alldwg.inc
    make -C examples alldxf_0.inc

    In C we cannot statically initialize variable-sized arrays of structs,
    to split it up into 3 parts.

e.g.

    { "MATERIAL", "004014080405aa6a9aa404014080405aa6a9aa404000000000000e03f4080405aa6a9aa4054080405aa6a9aa4050201016a9aa6a90150201016a9aa6a900aa4fe41d0801e40130018401e4019401c802aaa1803824882e464030dfbfb8", "11", "Drawing_2007.log", "test/test-data/Drawing_2007.dxf", 0x96, 746 },

Search 0 MATERIAL + Handle 96 in test/test-data/Drawing_2007.dxf =>

      0
    MATERIAL
      5
    96
-     1
-   ByLayer
-    94
-          63
=>
    FIELD_T   1  "ByLayer"    => dwg.spec: FIELD_T (name, 1);
    FIELD_BL 94 63  (int32_t) => dwg.spec: FIELD_BL (channel_flags, 94);

=cut

my $i = 0;
open my $f0, ">", "examples/alldxf_0.inc" || die "$!";
open my $f1, ">", "examples/alldxf_1.inc" || die "$!";
open my $f2, ">", "examples/alldxf_2.inc" || die "$!";

LINE:
while (<>) {
  @F = split(' ');
  
  my $dxf = $F[5];
  if ($dxf eq 'NULL,' or $dxf !~ /\.dxf",/) {
    next LINE; # -n
  }
  $dxf = substr($dxf, 1, -2);
  if (!-f $dxf) {
    if (!-f "../$dxf") {
      warn "$dxf and ../$dxf not found";
    } else {
      $dxf = "../$dxf";
    }
  }
  my $obj = substr($F[1],1,-2); # "MATERIAL",
  my $bytes   = substr($F[2],1,-2);
  my $bits = substr($F[3],1,-2);
  my $bitsize = $F[7];
  my $unknown = pack ("H*", $bytes);
  if ($bits) {
    $unknown .= pack ("B8", $bits);
  }
  $unknown = join("", map { sprintf("\\%03o", $_) } unpack("C*", $unknown));
  my $hdl = substr($F[6],2,-1); # 0xXXX,
  #warn "$dxf: $obj HANDLE($hdl)\n";
  # 9080187 5160203 9080187 201AA 51E0204 90C0202 35200204 20640A8 2D22020C 90A01D1 
  if ($hdl =~ /^([0-9A-F]){1,4}0([0-9A-F]+)$/) {
    $hdl = $2;
    #warn "=> try HANDLE($hdl)\n";
  }

  open my $f, "$dxf" or next LINE;
  my ($foundobj, $foundhdl);
  while (my $code = <$f>) {
    $code =~ s/\cM\cJ//;
    my $v = <$f>;
    $v =~ s/\cM\cJ//;
    $v =~ s/^\s*//;
    if ($code =~ /^ *0$/) {
      $foundobj = $v eq $obj;
      if ($foundhdl) {
        $foundhdl = 0;
        print $f0 " NULL },\n";
        print $f1 "    { 0,    NULL, NULL, 0, BITS_UNKNOWN, {-1,-1,-1,-1,-1}}\n};\n";
      }
    }
    if ($foundobj and $code =~ /^ *5$/) {
      $foundhdl = $v eq $hdl;
      if ($foundhdl) {
        warn "found $obj $hdl in $dxf\n";
        print $f0  "  { \"$obj\", \"$dxf\", 0x$hdl, /* $i */\n";
        print $f0  "    \"$unknown\", $bitsize,";

        print $f1 "/* $obj $hdl in $dxf */\n";
        print $f1 "static const struct _unknown_field unknown_dxf_$i\[\] = {\n";
        print $f2 "unknown_dxf\[$i\].fields = unknown_dxf_$i;\n";
        $i++;
      }
    }
    if ($foundhdl) {
      process_dxf($f1, $code, $v);
    }
  }
  close $f;
}
close $fu;

sub process_dxf {
  my ($f, $code, $v) = @_;
  #warn "$code: $v\n";
  #return if $code == 100;
  $v =~ s/\\/\\\\/g;
  $v =~ s/"/\\"/g;
  # code, value, bytes, bitsize, type, pos
  print $f "    { $code, \"$v\", NULL, 0, BITS_UNKNOWN, {-1,-1,-1,-1,-1} },\n";
}

