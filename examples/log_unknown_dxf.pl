#!/usr/bin/perl
=head1 USAGE

    perl examples/log_unknown_dxf.pl examples/alldwg.inc
    make -C examples alldxf_0.inc

    In C we cannot statically initialize variable-sized arrays of structs,
    so we split it up into 3 parts.

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

calc. number of same-typed: value fields as num.

=cut

# DXF types
use constant UNKNOWN => 0;
use constant B       => 1;
use constant NUM     => 2; #BL, BS, RL, RS or RC
use constant HDL     => 3;
use constant STR     => 4;
use constant HEXSTR  => 5;
use constant DBL     => 6;

my $i = 0;
open my $f0, ">", "examples/alldxf_0.inc" || die "$!";
open my $f1, ">", "examples/alldxf_1.inc" || die "$!";
open my $f2, ">", "examples/alldxf_2.inc" || die "$!";

open my $skip_fh, "<", "examples/alldwg.skip"
  or warn "examples/alldwg.skip missing";
my %skip;
while (<$skip_fh>) {
  my @F = split(' ');
  shift @F;
  # NAME-HANDLE-BITSIZE
  $F[2] =~ s/^0x//;
  $skip{"$F[0]-$F[2]-$F[3]"} = \@F;
}

LINE:
while (<>) {
  my @F = split(' ');
  
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
  #next LINE if $F[0] =~ m|//{|; # skip duplicates
  my $obj = substr($F[1],1,-2); # "MATERIAL",
  my $bytes   = substr($F[2],1,-2);
  my $bits = substr($F[3],1,-2);
  my $hdl = substr($F[6],2,-1); # 0xXXX,
  my $bitsize = $F[7];
  if ($skip{"$obj-$hdl-$bitsize"}) { # skip empty unknowns
    warn "skip empty $obj-$hdl-$bitsize $dxf\n";
    next LINE;
  }
  my $unknown = pack ("H*", $bytes);
  if ($bits) {
    $unknown .= pack ("B8", $bits);
  }
  $unknown = join("", map { sprintf("\\%03o", $_) } unpack("C*", $unknown));
  #warn "$dxf: $obj HANDLE($hdl)\n";
  # 9080187 5160203 9080187 201AA 51E0204 90C0202 35200204 20640A8 2D22020C 90A01D1 
  #if ($hdl =~ /^([0-9A-F]){1,4}0([0-9A-F]+)$/) {
  #  $hdl = $2;
  #  #warn "=> try HANDLE($hdl)\n";
  #}

  open my $f, "$dxf" or next LINE;
  my ($foundobj, $foundhdl, @FIELD);
  while (my $code = <$f>) {
    $code =~ s/\cM\cJ//;
    my $v = <$f>;
    $v =~ s/\cM\cJ//;
    $v =~ s/^\s*//;
    if ($code =~ /^ *0$/) {
      $foundobj = $v eq $obj;
      if ($foundhdl) {
        $foundhdl = 0;
        # search FIELD for duplicates
        my %sorted = ();
        for (@FIELD) {
          my $t = dxf_type($_->[0]);
          $sorted{"$t:$_->[1]"}++;
        }
        for (@FIELD) {
          my $t = dxf_type($_->[0]);
          my $num = $sorted{"$t:$_->[1]"};
          emit_field($f1, $_->[0], $_->[1], $num);
        }
        @FIELD = ();
        print $f0 " NULL },\n";
        print $f1 "    { 0,    NULL, NULL, 0, BITS_UNKNOWN, 0, {-1,-1,-1,-1,-1}}\n};\n";
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
        @FIELD = ();
        $i++;
      }
    }
    if ($foundhdl) {
      $code =~ s/\s//g;
      push @FIELD, [$code, $v];
    }
  }
  close $f;
}
close $fu;

sub dxf_type {
  my $code = shift;
  if ($code < 5) {
    return STR;
  } elsif ($code == 5) {
    return HDL;
  } elsif ($code < 10) {
    return STR;
  } elsif ($code < 60) {
    return DBL;
  } elsif ($code < 100) {
    return NUM;
  } elsif ($code == 102) {
    return STR;
  } elsif ($code == 105) {
    return HDL;
  } elsif ($code < 110) {
    return STR;
  } elsif ($code < 160) {
    return DBL;
  } elsif ($code < 210) {
    return NUM;
  } elsif ($code < 240) {
    return DBL;
  } elsif ($code < 289) {
    return NUM;
  } elsif ($code < 299) {
    return B;
  } elsif ($code < 320) {
    return STR;
  } elsif ($code < 370) {
    return HDL;
  } elsif ($code < 390) {
    return NUM;
  } elsif ($code < 400) {
    return HDL;
  } elsif ($code < 410) {
    return NUM;
  } elsif ($code < 420) {
    return STR;
  } elsif ($code < 430) {
    return NUM;
  } elsif ($code < 440) {
    return STR;
  } elsif ($code < 460) {
    return NUM;
  } elsif ($code < 470) {
    return DBL;
  } elsif ($code < 480) {
    return STR;
  } elsif ($code < 481) {
    return HDL;
  } elsif ($code < 1010) { # eed fields:
    return STR;
  } elsif ($code < 1060) {
    return DBL;
  } elsif ($code <= 1071) {
    return NUM;
  }
  warn "unknown DXF code $code\n";
  return UNKNOWN;
}

sub emit_field {
  my ($f, $code, $v, $num) = @_;
  #warn "$code: $v\n";
  #return if $code == 100;
  $v =~ s/\\/\\\\/g;
  $v =~ s/"/\\"/g;
  $num = 0 unless $num;
  # code, value, bytes, bitsize, type, num, pos[]
  print $f "    { $code, \"$v\", NULL, 0, BITS_UNKNOWN, $num, {-1,-1,-1,-1,-1} },\n";
}

