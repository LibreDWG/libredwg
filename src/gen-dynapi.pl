#!/usr/bin/perl -w
# Copyright (C) 2019 Free Software Foundation, Inc., GPL3
#
# Generate src/dynapi.c and test/unit-testing/dynapi_test.c
# C structs/arrays for all dwg objects and its fields for a dynamic API.
#   -> name, type, size, offset, dxfgroup, memory-type
# Within each object linear search is good enough.
# This is needed for in_dxf, dwgfilter,
# a maintainable and shorter dwg_api and shorter language bindings.
# Written by: Reini Urban

#dwg.h:
# typedef struct _dwg_header_variables
# typedef struct _dwg_entity_(.*)
# typedef struct _dwg_object_(.*)
#subtypes:
# typedef struct _dwg_TYPE_subtype

use strict;
use warnings;
use vars qw(@entity_names @object_names @subtypes
            $max_entity_names $max_object_names);
use Convert::Binary::C;
#use Data::Dumper;
#BEGIN { chdir 'src' if $0 =~ /src/; }

# add gcc/clang -print-search-dirs paths
my (@ccincdir, $srcdir, $incdir);
if ($0 =~ m{(^\.\./.*src)/gen}) {
  $srcdir = $1;
  $incdir = "$srcdir/../include";
} elsif ($0 =~ m{^src/gen}) {
  $srcdir = "src";
  $incdir = "include";
} else {
  $srcdir = ".";
  $incdir = "../include";
}
my $CC = `grep '^CC =' Makefile`;
if ($CC) {
  $CC =~ s/^CC =//;
  chomp $CC;
  $CC =~ s/^\s+//;
  if ($CC =~ /^afl-(gcc|clang)/) {
    $ENV{AFL_QUIET} = 1;
  }
  my $out = `$CC -print-search-dirs`;
  if ($out) {
    if ($out =~ /^install: (.+?)$/m) {
      my $d = $1;
      $d =~ s/\/$//;
      @ccincdir = ("$d/include") if -d "$d/include";
    } elsif ($out =~ /^libraries: =(.+?)$/m) {
      @ccincdir = grep { -d "$_/include" ? "$_/include" : undef }
                       split ':', $1;
    }
  }
}
#__WORDSIZE=64
# glibc quirks to include stdint.h and stddef.h directly with this old pre-C99
# Convert::Binary::C preprocessor
my @defines = ('__GNUC__=4', '__x86_64__', '__inline=inline',
               '__THROW=', '__attribute__(x)=');
if ($^O =~ /darwin|bsd/) {
  push @defines, ('__signed=signed', '__builtin_va_list=void*', '__extension__=');
}
if (@ccincdir and join(" ",@ccincdir) =~ /clang/) {
  push @defines, ('__has_feature(x)=0', '__has_include_next(x)=0',
                  '__INTPTR_TYPE__=long int', '__UINTPTR_TYPE__=unsigned long int',
                  '__INTMAX_TYPE__=long int', '__UINTMAX_TYPE__=unsigned long int',
                  '__gwchar_t=int', '____gwchar_t_defined',
    );
}

warn "using $CC with @ccincdir\n" if @ccincdir;
my $c = Convert::Binary::C->new
  ->Include('.', @ccincdir, '/usr/include')
  ->Define(@defines);
my $hdr = "$incdir/dwg.h";
$c->parse_file($hdr);

#print Data::Dumper->Dump([$c->struct('_dwg_entity_TEXT')], ['_dwg_entity_TEXT']);
#print Data::Dumper->Dump([$c->struct('struct _dwg_header_variables')], ['Dwg_Header_Variables']);

my (%h, $n, %structs, %ENT, %DXF);
local (@entity_names, @object_names, @subtypes, $max_entity_names, $max_object_names);
# todo: harmonize more subtypes
for (sort $c->struct_names) {
  if (/_dwg_entity_([A-Z0-9_]+)/) {
    $structs{$1}++;
    push @entity_names, $1;
  } elsif (/_dwg_object_([A-Z0-9_]+)/) {
    $structs{$1}++;
    push @object_names, $1;
  } elsif (/_dwg_header_variables/) {
    ;
  } elsif (/_dwg_([A-Z0-9]+)(_|$)/) {
    $structs{$_}++;
    push @subtypes, $_;
  } else {
    #print " (?)";
  }
}
# get BITCODE_ macro types for each struct field
open my $in, "<", $hdr or die "hdr: $!";
my $f;
while (<$in>) {
  if (!$n) {
    if (/^typedef struct (_dwg_.+) \{/) {
      $n = $1;
    } elsif (/^typedef struct (_dwg_\S+)$/) {
      $n = $1;
    } elsif (/^#define (COMMON_\w+)\((\w+)\)/) { # COMMON_TABLE_CONTROL_FIELDS
      # BUG: Convert::Binary::C cannot separate H* from H, both are just Dwg_Object_Ref
      # So we need to parse the defines
      $n = $1;
      $f = $2;
      $h{$n}{reactors} = 'H*'; # has no ;
    } elsif (/^#define (COMMON_\w+)/) { # COMMON_ENTITY_POLYLINE
      $n = $1;
      $h{$n}{seqend}   = 'H'; # has no ;
    }
  } elsif (/^\}/) { # close the struct
    $n = '';
  } elsif ($n and $_ =~ /^ +BITCODE_([\w\*]+)\s+(\w.*);/) {
    my $type = $1;
    my $v = $2;
    $v =~ s/^_3/3/;
    $type =~ s/\s+$//;
    if ($n eq 'COMMON_TABLE_CONTROL_FIELDS' && $v eq 'entries') {
      $h{$n}{$_} = 'H*'
        for qw(block_headers layers styles linetypes views ucs vports apps
               dimstyles vport_entity_headers);
    }
    $h{$n}{$v} = $type;
  }
}
#$h{Dwg_Bitcode_3BD} = '3BD';
#$h{Dwg_Bitcode_2BD} = '2BD';
#$h{Dwg_Bitcode_3RD} = '3RD';
#$h{Dwg_Bitcode_2RD} = '2RD';
close $in;

sub dxf_in {
  $in = shift;
  while (<$in>) {
    $f = '';
    s/DXF \{ //;
    if (!$n) {
      if (/^DWG_(ENTITY|OBJECT)\((\w+)\)/) {
        $n = $2;
        $n =~ s/^_3/3/;
        warn $n;
      }
      elsif (/^\s*SECTION\(HEADER\)/) {
        $n = 'header_variables';
        warn $n;
      }
      elsif (/^int DWG_FUNC_N\(ACTION,(\w+)\)/) {
        $n = $1;
        warn $n;
      }
    } elsif (/^DWG_(ENTITY|OBJECT)_END/) { # close
      $n = '';
    } elsif (!$n) {
      ;
    } elsif (/^\s+FIELD_HANDLE\s*\((\w+),\s*\d+,\s*(\d+)\)/) {
      $f = $1;
      $DXF{$n}->{$f} = $2 if $2;
    } elsif (/^\s+VALUE_HANDLE\s*\(.+,\s*(\w+),\s*\d,\s*(\d+)\)/) {
      $f = $1;
      $DXF{$n}->{$f} = $2 if $2;
    } elsif (/^\s+FIELD_.+\s*\((\w+),\s*(\d+)\)/) {
      $f = $1;
      $DXF{$n}->{$f} = $2 if $2;
    } elsif (/^\s+FIELD_.+\s*\((\w+),.*,\s*(\d+)\)/) {
      $f = $1;
      $DXF{$n}->{$f} = $2 if $2;
    } elsif (/^\s+HEADER_.+\s*\((\w+),\s*(\d+)\)/) { # HEADER_RC
      $f = $1;
      $DXF{$n}->{$f} = $2 if $2;
    } elsif (/^\s+HEADER_.+\s*\((\w+),\s*(\d+),\s*\w+\)/) {
      $f = $1;
      $DXF{$n}->{$f} = $2 if $2;
    } elsif (/^\s+HEADER_.+\s*\((\w+),\s*\w+,\s*(\d+),\s*\D.+\)/) {
      $f = $1;
      #$f =~ s/^_3/3/;
      $DXF{$n}->{$f} = $2 if $2;
    } elsif (/^\s+HEADER_([23])D\s*\((\w+)\)/) {
      $f = $2;
      $DXF{$n}->{$f} = $1 eq '2' ? 20 : 30;
    } elsif (/^\s+VALUE_.+\s*\((\w.+),.*,\s*(\d+)\)/) {
      $f = $1;
      $DXF{$n}->{$f} = $2 if $2;
    }
    if ($f and $n and exists $DXF{$n}->{$f}) {
      warn "  $f $DXF{$n}->{$f}\n";
    }
  }
}

# get dxf group for each struct field
sub dxfin_spec {
  my $fn = shift;
  open my $in, "<", $fn  or die "$fn: $!";
  dxf_in($in);
  close $in;
}
dxfin_spec "$srcdir/dwg.spec";
$DXF{'3DSOLID'}->{'version'} = 70;
$DXF{'REGION'}->{'version'} = 70;
$DXF{'BODY'}->{'version'} = 70;
$DXF{'3DSOLID'}->{'encr_sat_data'} = 1;
$DXF{'REGION'}->{'encr_sat_data'} = 1;
$DXF{'BODY'}->{'encr_sat_data'} = 1;
$DXF{'BLOCK'}->{'name'} = 2; # and 3
$DXF{'VISUALSTYLE'}->{'edge_hide_precision_flag'} = 290;
$DXF{'VISUALSTYLE'}->{'is_internal_use_only'} = 291;

dxfin_spec "$srcdir/header_variables_dxf.spec";
$DXF{header_variables}->{'_3DDWFPREC'} = 40;

$n = 'object_entity';
dxfin_spec "$srcdir/common_entity_data.spec";
dxfin_spec "$srcdir/common_entity_handle_data.spec";
$DXF{$n}->{'color'} = $DXF{$n}->{'color_r11'} = 62;
$DXF{$n}->{'color.handle'} = 420;
$DXF{$n}->{'paper_r11'} = 67;
$DXF{$n}->{'plotstyle'} = 390;
$DXF{$n}->{'ownerhandle'} = 330;
$DXF{$n}->{'xdicobjhandle'} = 360;
$DXF{$n}->{'reactors'} = 330;

$n = 'object_object';
$DXF{$n}->{'ownerhandle'} = 330;
$DXF{$n}->{'xdicobjhandle'} = 360;
$DXF{$n}->{'reactors'} = 330;

my $cfile = "dynapi.c";
chmod 0644, $cfile if -e $cfile;
open my $fh, ">", $cfile or die "$cfile: $!";

sub out_struct {
  my ($tmpl, $n) = @_;
  #print $fh " /* ", Data::Dumper->Dump([$s], [$n]), "*/\n";
  my $key = $n;
  $n = "_dwg_$n" unless $n =~ /^_dwg_/;
  my $ns = $tmpl;
  $ns =~ s/^struct //;
  my $sortedby = 'offset';
  my $s = $c->struct($tmpl);
  return unless $s->{declarations};
  my @declarations = @{$s->{declarations}};
  if ($n =~ /^_dwg_(header_variables|object_object|object_entity)$/) {
    @declarations = sort {
      my $aname = $a->{declarators}->[0]->{declarator};
      my $bname = $b->{declarators}->[0]->{declarator};
      $aname =~ s/^\*//g;
      $bname =~ s/^\*//g;
      return $aname cmp $bname
    } @declarations;
    $sortedby = 'name';
  }
  print $fh "/* from typedef $tmpl: (sorted by $sortedby) */\n",
    "static const Dwg_DYNAPI_field $n","_fields[] = {\n";
  for my $d (@declarations) {
    my $type = $d->{type};
    my $decl = $d->{declarators}->[0];
    my $name = $decl->{declarator};
    while ($name =~ /^\*/) {
      $name =~ s/^\*//;
      $type .= '*';
    }
    # unexpand BITCODE_ macros: e.g. unsigned int -> BITCODE_BL
    my $bc = exists $h{$ns} ? $h{$ns}{$name} : undef;
    if (!$bc && $ns =~ /_CONTROL$/) {
      $bc = $h{COMMON_TABLE_CONTROL_FIELDS}{$name};
    } elsif (!$bc && $ns =~ /_entity_POLYLINE_/) {
      $bc = $h{COMMON_ENTITY_POLYLINE}{$name};
    }
    $type = $bc if $bc;
    if ($name eq 'encr_sat_data') {
      $type = 'char **'; $bc = '';
    }
    $type =~ s/\s+$//;
    my $size = $bc ? "sizeof (BITCODE_$type)" : "sizeof ($type)";
    $type =~ s/BITCODE_//;
    # TODO: DIMENSION_COMMON, _3DSOLID_FIELDS macros
    if ($type eq 'unsigned char') {
      $type = 'RC';
    } elsif ($type eq 'unsigned char*') {
      $type = 'RC*';
    } elsif ($type eq 'double') {
      $type = 'BD';
    } elsif ($type eq 'double*') {
      $type = 'BD*';
    } elsif ($type =~ /^Dwg_Bitcode_(\w+)/) {
      $type = $1;
    } elsif ($type eq 'char*') {
      $type = 'TV';
    } elsif ($type eq 'unsigned short int') {
      $type = 'BS';
    } elsif ($type eq 'uint16_t') {
      $type = 'BS';
    } elsif ($type eq 'unsigned int') {
      $type = 'BL';
    } elsif ($type eq 'unsigned int*') {
      $type = 'BL*';
    } elsif ($type eq 'uint32_t') {
      $type = 'BL';
    } elsif ($type eq 'uint32_t*') {
      $type = 'BL*';
    } elsif ($type eq 'Dwg_Object_Ref*') {
      $type = 'H';
    } elsif ($type eq 'Dwg_Object_Ref**') {
      $type = 'H*';
    } elsif ($type =~ /\b(unsigned|char|int|long|double)\b/) {
      warn "unexpanded $type $n.$name\n";
    } elsif ($type =~ /^struct/) {
      $size = "sizeof (void *)";
    } elsif ($type =~ /^HASH\(/) { # inlined struct or union
      warn "ignore inlined field $n.$name\n";
      next;
    }
    my $is_malloc = ($type =~ /\*$/ or $type =~ /^(T$|T[UVF])/) ? 1 : 0;
    my $is_indirect = ($is_malloc or $type =~ /^(struct|[23T]|CMC|H$)/) ? 1 : 0;
    my $is_string = ($is_malloc and $type =~ /^T[UV]?$/) ? 1 : 0; # not TF or TFF
    my $sname = $name;
    if ($name =~ /\[(\d+)\]$/) {
      $is_malloc = 0;
      $size = "$1 * $size";
      $sname =~ s/\[(\d+)\]$//;
    }
    $ENT{$key}->{$name} = $type;
    my $dxf = $DXF{$key}->{$name};
    $dxf = 0 unless $dxf;
    warn "no dxf for $key: $name 0\n" unless $dxf or
      ($name eq 'parent') or
      ($key eq 'header_variables' and $name eq lc($name));

    printf $fh "  { \"%s\", \"%s\", %s, OFF (%s,%s), %d,%d,%d, %d },\n",
      $name, $type, $size, $tmpl, $sname, $is_indirect, $is_malloc, $is_string, $dxf;
  }
  print $fh "  {NULL, NULL, 0, 0, 0,0,0, 0},\n";
  print $fh "};\n";
}

sub maxlen {
  my $maxlen = 0;
  for (@_) {
    $maxlen = length($_) if $maxlen < length($_);
  }
  $maxlen
}
$max_entity_names = 1+maxlen(@entity_names);
$max_object_names = 1+maxlen(@object_names);

for (<DATA>) {
  # expand enum or struct
  if (/^(.*)\@\@(\w+ \w+)\@\@(.*)/) {
    my ($pre, $post) = ($1, $3);
    my $tmpl = $2;
    print $fh $pre;
    if ($tmpl =~ /^enum (\w+)/) {
      my $s = $c->enum($tmpl);
      #print $fh "\n/* ";
      #print $fh Data::Dumper->Dump([$s], [$1]);
      #print $fh "\n*/";
      my $i = 0;
      my @keys = map { s/^DWG_TYPE__3D/DWG_TYPE_3D/; $_ } keys %{$s->{enumerators}};
      for (sort @keys) {
        my ($k,$v) = ($_, $s->{enumerators}->{$_});
        if ($tmpl eq 'enum DWG_OBJECT_TYPE') {
          $k =~ s/^DWG_TYPE_//;
          if (!$v && $k =~ /^3D/) {
            $v = $s->{enumerators}->{'DWG_TYPE__'.$k};
          }
          # see if the fields do exist:
          my $fields = exists $structs{$k} ? "_dwg_".$k."_fields" : "NULL";
          if ($k =~ /^(BODY|REGION)$/) {
            $fields = "_dwg_3DSOLID_fields";
          } elsif ($k eq 'XLINE') {
            $fields = "_dwg_RAY_fields";
          } elsif ($k =~ /^VERTEX_(MESH|PFACE)$/) {
            $fields = "_dwg_VERTEX_3D_fields";
          }
          printf $fh "  { \"%s\", %d, %s },\t/* %d */\n",
              $k, $v, $fields, $i++;
        } else {
          printf $fh "  { \"%s\", %d },\t/* %d */\n",
            $k, $v, $i++;
        }
      }
    } elsif ($tmpl =~ /^list (\w+)/) {
      no strict 'refs';
      my $n = $1;
      my $i = 0;
      my $maxlen = 0;
      for (@{$n}) {
        $maxlen = length($_) if $maxlen < length($_);
      }
      for (@{$n}) {
        my $len = length($_);
        printf $fh "  \"%s\" \"%s\",\t/* %d */\n", $_, "\\0" x ($maxlen-$len), $i++;
      }
    } elsif ($tmpl =~ /^scalar (\w+)/) {
      no strict 'refs';
      my $n = $1;
      printf $fh ${$n};
    } elsif ($tmpl =~ /^for dwg_entity_ENTITY/) {
      for (@entity_names) {
        out_struct("struct _dwg_entity_$_", $_);
      }
    } elsif ($tmpl =~ /^for dwg_object_OBJECT/) {
      for (@object_names) {
        out_struct("struct _dwg_object_$_", $_);
      }
    } elsif ($tmpl =~ /^for dwg_subtypes/) {
      for (@subtypes) {
        out_struct("struct $_", $_);
      }
    } elsif ($tmpl =~ /^struct _dwg_(\w+)/) {
      out_struct($tmpl, $1);
    }
    print $fh $post,"\n";
  } else {
    print $fh $_;
  }
}
chmod 0444, $fh;
close $fh;

# TODO: use dwg.h formats
my %FMT = (
    'double' => '%g',
    'unsigned char' => '%c',
    'unsigned int' => '%u',
    'unsigned long' => '%lu',
    'unsigned short int' => '%hu',
    'short' => '%hd',
    'long' => '%l',
    'char**' => '%p',
    'TV' => '%s',
    'T'  => '%s',
    'TU' => '%ls',
    'TFF' => '%s',
    'BD' => '%g',
    'BL' => '%u',
    'BS' => '%hu',
    'RD' => '%g',
    'RL' => '%u',
    'RS' => '%hu',
    'RC' => '%u',
    'RC*' => '%s',
    );

my $infile = "$srcdir/../test/unit-testing/dynapi_test.c.in";
open $in, $infile or die "$infile: $!";
$cfile  = "$srcdir/../test/unit-testing/dynapi_test.c";
chmod 0644, $cfile if -e $cfile;
open $fh, ">", $cfile or die "$cfile: $!";
print $fh "/* ex: set ro ft=c: -*- mode: c; buffer-read-only: t -*- */\n";
for (<$in>) {
  print $fh $_;
  if (m{/\* \@\@for test_HEADER\@@ \*/}) {
    my $s = $c->struct('_dwg_header_variables');
    for my $d (@{$s->{declarations}}) {
      my $type = $d->{type};
      my $decl = $d->{declarators}->[0];
      my $name = $decl->{declarator};
      while ($name =~ /^\*/) {
        $name =~ s/^\*//;
        $type .= '*';
      }
      $type =~ s/ $//g;
      my $xname = $name =~ /^3/ ? "_$name" : $name;
      my $lname = lc $xname;
      my $var = $lname;
      my $sname = $name;
      if (exists $ENT{header_variables}->{$name}) {
        $type = $ENT{header_variables}->{$name};
      }
      my $fmt = exists $FMT{$type} ? $FMT{$type} : undef;
      if (!$fmt) {
        if ($type =~ /[ \*]/ or $type eq 'H') {
          $fmt = '%p';
        } else {
          $fmt = "\" FORMAT_$type \"";
        }
      }
      my $is_ptr = ($type =~ /^(struct|Dwg_)/ or
                    $type =~ /^[23HT]/ or
                    $type =~ /\*$/ or
                    $var  =~ /\[\d+\]$/ or
                    $type =~ /^(BE|CMC)/)
        ? 1 : 0;
      if ($var  =~ /\[\d+\]$/) {
        $lname =~ s/\[\d+\]$//g;
        $sname =~ s/\[\d+\]$//g;
      }
      my $stype = $type;
      $type = 'BITCODE_'.$type unless ($type =~ /^(struct|Dwg_)/ or $type =~ /^[a-z]/);
      if (!$is_ptr) {
        print $fh <<"EOF";
  {
    $type $var;
    if (dwg_dynapi_header_value (dwg, "$name", &$var, NULL)
        && $var == dwg->header_vars.$name)
      pass ();
    else
      fail ("HEADER.$name [$stype] $fmt != $fmt", dwg->header_vars.$sname, $var);
EOF
        if ($type =~ /(int|long|short|char ||double|_B\b|_B[BSLD]\b|_R[CSLD])/) {
          print $fh "    $var++;\n";
        }
        print $fh <<"EOF";
    if (dwg_dynapi_header_set_value (dwg, "$name", &$var, 0)
        && $var == dwg->header_vars.$name)
      pass ();
    else
      fail ("HEADER.$name [$stype] set+1 $fmt != $fmt",
            dwg->header_vars.$sname, $var);
EOF
        if ($type =~ /(int|long|short|char ||double|_B\b|_B[BSLD]\b|_R[CSLD])/) {
          print $fh "    $var--;\n";
          print $fh "    dwg_dynapi_header_set_value (dwg, \"$name\", &$var, 0);\n";
        }
        print $fh "\n  }\n";
      } else {
        print $fh <<"EOF";
  {
    $type $var;
    if (dwg_dynapi_header_value (dwg, "$name", &$lname, NULL)
EOF
        if ($type !~ /\*\*/) {
          print $fh <<"EOF";
        && !memcmp (&$lname, &dwg->header_vars.$sname, sizeof (dwg->header_vars.$sname))
EOF
        }
        print $fh <<"EOF";
       )
      pass ();
    else
      fail ("HEADER.$name [$stype]");
  }
EOF
      }
    }
  }
  if (m{/\* \@\@for if_test_OBJECT\@\@ \*/}) {
    for my $name (@entity_names, @object_names) {
      my $xname = $name =~ /^3/ ? "_$name" : $name; # 3DFACE, 3DSOLID
      next if $name eq 'DIMENSION_';
      next if $name eq 'PROXY_LWPOLYLINE';
      print $fh "  else" if $name ne '3DFACE'; # the first
      print $fh <<"EOF";
  if (obj->fixedtype == DWG_TYPE_$xname)
    error += test_$xname(obj);
EOF
    }
  }
  if (m{/\* \@\@for test_OBJECT\@\@ \*/}) {
    for my $name (@entity_names, @object_names) {
      next if $name eq 'DIMENSION_';
      next if $name eq 'PROXY_LWPOLYLINE';
      my $is_ent = grep { $name eq $_ } @entity_names;
      my ($Entity, $lentity) = $is_ent ? ('Entity', 'entity') : ('Object', 'object');
      my $xname = $name =~ /^3/ ? "_$name" : $name;
      my $lname = lc $xname;
      my $struct = "Dwg_$Entity" . "_$xname";
      print $fh <<"EOF";
static int test_$xname (const Dwg_Object *obj)
{
  int error = 0;
  const Dwg_Object_$Entity *restrict obj_obj = obj->tio.$lentity;
  $struct *restrict $lname = obj->tio.$lentity->tio.$xname;
EOF

  for my $var (sort keys %{$ENT{$name}}) {
    my $type = $ENT{$name}->{$var};
    my $fmt = exists $FMT{$type} ? $FMT{$type} : undef;
    if (!$fmt) {
      if ($type =~ /[ \*]/ or $type eq 'H') {
        $fmt = '%p';
      } else {
        $fmt = "\" FORMAT_$type \"";
      }
    }
    my $svar = $var;
    my $is_ptr = ($type =~ /^(struct|Dwg_)/ or
                  $type =~ /^[TH23]/ or
                  $type =~ /\*$/ or
                  $var =~ /\[\d+\]$/ or
                  $type =~ /^(BE|CMC|BLL)$/)
      ? 1 : 0;
    if ($var  =~ /\[\d+\]$/) {
      $svar =~ s/\[\d+\]$//g;
    }
    my $stype = $type;
    $type = 'BITCODE_'.$type unless ($type =~ /^(struct|Dwg_)/ or $type =~ /^[a-z]/);
    if (!$is_ptr) {
      print $fh <<"EOF";
  {
    $type $var;
    if (dwg_dynapi_entity_value($lname, "$name", "$var", &$svar, NULL)
        && $var == $lname->$svar)
      pass ();
    else
      fail ("$name.$var [$stype] $fmt != $fmt", $lname->$svar, $svar);
EOF
      if ($type =~ /(int|long|short|char|double|_B\b|_B[BSLD]\b|_R[CSLD])/) {
        print $fh "    $svar++;\n";
      }
      print $fh <<"EOF";
    if (dwg_dynapi_entity_set_value ($lname, "$name", "$var", &$svar, 0)
        && $var == $lname->$svar)
      pass ();
    else
      fail ("$name.$var [$stype] set+1 $fmt != $fmt", $lname->$svar, $svar);
EOF
      if ($type =~ /(int|long|short|char ||double|_B\b|_B[BSLD]\b|_R[CSLD])/) {
        print $fh "    $lname->$svar--;\n";
      }
      print $fh "\n  }\n";
    } elsif ($type =~ /\*$/ and $type !~ /(RC\*|struct _dwg_object_)/
             # no countfield
             and $var !~ /^(ref|block_size|extra_acis_data|objid_object_handles)$/
             # VECTOR_N
             and $var !~ /(_transform|_transmatrix1?|shhn_pts)$/) {
      my %countfield = (
        attrib_handles => 'num_owned',
        attribs => 'num_owned', # XXX TABLE
        vertex => 'num_owned',
        itemhandles => 'numitems',
        entities => 'num_owned',
        insert_handles => 'num_inserts',
        group_entries => 'num_handles',
        field_handles => 'num_fields',
        sort_handles => 'num_ents',
        attr_def_id => 'num_attr_defs',
        readdeps  => 'num_deps',
        writedeps => 'num_deps',
        dashes_r11 => 'num_dashes',

        text => 'numitems',

        block_headers => 'num_entries',
        layers => 'num_entries',
        styles => 'num_entries',
        linetypes => 'num_entries',
        views => 'num_entries',
        ucs => 'num_entries',
        vports => 'num_entries',
        apps => 'num_entries',
        dimstyles => 'num_entries',
        vport_entity_headers => 'num_entries',
        entry_handles => 'num_entries',
        encr_sat_data => 'num_blocks',
        );
      my $countfield = exists $countfield{$var} ? $countfield{$var} : "num_$var";
      $countfield = 'num_dashes' if $name eq 'LTYPE' and $var eq 'styles';
      my $count = 1;
      if ($var eq 'encr_sat_data') {
        print $fh <<"EOF";
  {
    $type $var;
    if (dwg_dynapi_entity_value ($lname, "$name", "$var", &$svar, NULL)
        && !memcmp (&$svar, &$lname->$svar, sizeof ($lname->$svar)))
      pass ();
    else
      fail ("$name.$var [$stype]");
  }
EOF
      }
      elsif ($var eq 'reactors' and $type eq 'BITCODE_H*') {
        print $fh <<"EOF";
  {
    $type $var;
    BITCODE_BL count = obj_obj->num_reactors;
    if (dwg_dynapi_entity_value ($lname, "$name", "$var", &$svar, NULL)
        && $svar == $lname->$svar)
      pass ();
    else
      fail ("$name.$var [$stype] * %u $countfield", count);
  }
EOF
      } else {
        print $fh <<"EOF";
  {
    $type $var;
    BITCODE_BL count = 0;
    if (dwg_dynapi_entity_value ($lname, "$name", "$countfield", &count, NULL)
        && dwg_dynapi_entity_value ($lname, "$name", "$var", &$svar, NULL)
EOF
        if ($type !~ /\*\*/) {
          print $fh "        && $svar == $lname->$svar";
        }
        print $fh ")\n";
        print $fh <<"EOF";
      pass ();
    else
      fail ("$name.$var [$stype] * %u $countfield", count);
  }
EOF
      }
    } else { # is_ptr
      my $is_str;
      print $fh <<"EOF";
  {
    $type $var;
    if (dwg_dynapi_entity_value($lname, "$name", "$var", &$svar, NULL)
EOF
        if ($stype =~ /^(TV|RC\*|unsigned char\*|char\*)$/) {
          $is_str = 1;
          print $fh "        && $svar\n";
          print $fh "           ? !strcmp ((char *)$svar, (char *)$lname->$svar)\n";
          print $fh "           : !$lname->$svar)\n";
        } elsif ($type !~ /\*\*/) {
          print $fh "        && !memcmp (&$svar, &$lname->$svar, sizeof ($lname->$svar)))\n";
        } else {
          print $fh ")\n";
        }
        if ($is_str) {
          print $fh <<"EOF";
      pass ();
    else
      fail ("$name.$var [$stype] '$fmt' <> '$fmt'", $svar, $lname->$svar);
  }
EOF
        } else {
          print $fh <<"EOF";
        pass ();
    else
        fail ("$name.$var [$stype]");
  }
EOF
        }
      }
    }
    print $fh <<"EOF";
  return failed;
}
EOF
    }
  }
  if (m{/\* \@\@for if_test_OBJECT\@\@ \*/}) {
    for my $name (@entity_names, @object_names) {
      my $xname = $name =~ /^3/ ? "_$name" : $name; # 3DFACE, 3DSOLID
      next if $name eq 'DIMENSION_';
      next if $name eq 'PROXY_LWPOLYLINE';
      print $fh "  else" if $name ne '3DFACE'; # the first
      print $fh <<"EOF";
  if (obj->fixedtype == DWG_TYPE_$xname)
    error += test_$xname (obj);
EOF
    }
  }
}
close $in;
chmod 0444, $fh;
close $fh;

__DATA__
/* ex: set ro ft=c: -*- mode: c; buffer-read-only: t -*- */
#line 750 "gen-dynapi.pl"
/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2018-2019 Free Software Foundation, Inc.                   */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * dynapi.c: dynamic access to all object and field names and types
 * written by Reini Urban
 * generated by src/gen-dynapi.pl from include/dwg.h, do not modify.
 */

#include "config.h"
#include <string.h>
#include <stdlib.h>
#include "common.h"
#include "dynapi.h"
#define DWG_LOGLEVEL loglevel
#include "logging.h"
#include "dwg.h"
#include "bits.h"
  
#ifndef _DWG_API_H_
Dwg_Object *dwg_obj_generic_to_object (const void *restrict obj,
                                       int *restrict error);
#endif

#define MAXLEN_ENTITIES @@scalar max_entity_names@@
#define MAXLEN_OBJECTS @@scalar max_object_names@@

/* sorted for bsearch. from typedef struct _dwg_entity_*: */
static const char dwg_entity_names[][MAXLEN_ENTITIES] = {
@@list entity_names@@
};
/* sorted for bsearch. from typedef struct _dwg_object_*: */
static const char dwg_object_names[][MAXLEN_OBJECTS] = {
@@list object_names@@
};

@@struct _dwg_header_variables@@
@@for dwg_entity_ENTITY@@
@@for dwg_object_OBJECT@@
@@for dwg_subtypes@@

/* common fields: */
@@struct _dwg_object_entity@@
@@struct _dwg_object_object@@

struct _name_type_fields {
  const char *const name;
  const int type;
  const Dwg_DYNAPI_field *const fields;
};

/* sorted for bsearch. from enum DWG_OBJECT_TYPE: */
static const struct _name_type_fields dwg_name_types[] = {
  @@enum DWG_OBJECT_TYPE@@
};

#line 817 "gen-dynapi.pl"
static int
_name_inl_cmp (const void *restrict key, const void *restrict elem)
{
  //https://en.cppreference.com/w/c/algorithm/bsearch
  return strcmp ((const char *)key, (const char *)elem); //inlined
}

struct _name
{
  const char *const name;
};

static int
_name_struct_cmp (const void *restrict key, const void *restrict elem)
{
  //https://en.cppreference.com/w/c/algorithm/bsearch
  const struct _name *f = (struct _name *)elem;
  return strcmp ((const char *)key, f->name); //deref
}

#define ARRAY_SIZE(arr) (sizeof (arr) / sizeof (arr[0]))
#define NUM_ENTITIES    ARRAY_SIZE(dwg_entity_names)
#define NUM_OBJECTS     ARRAY_SIZE(dwg_object_names)
#define NUM_NAME_TYPES  ARRAY_SIZE(dwg_name_types)

EXPORT bool
is_dwg_entity (const char *name)
{
  return bsearch (name, dwg_entity_names, NUM_ENTITIES, MAXLEN_ENTITIES,
                  _name_inl_cmp)
             ? true
             : false;
}

EXPORT bool
is_dwg_object (const char *name)
{
  return bsearch (name, dwg_object_names, NUM_OBJECTS, MAXLEN_OBJECTS,
                  _name_inl_cmp)
             ? true
             : false;
}

const Dwg_DYNAPI_field *
dwg_dynapi_entity_fields (const char *name)
{
  const char *p = bsearch (name, dwg_name_types, NUM_NAME_TYPES - 1,
                           sizeof (dwg_name_types[0]), /* NULL terminated */
                           _name_struct_cmp);
  if (p)
    {
      const int i = (p - (char *)dwg_name_types) / sizeof (dwg_name_types[0]);
      const struct _name_type_fields *f = &dwg_name_types[i];
      return f->fields;
    }
  else
    return NULL;
}

const Dwg_DYNAPI_field*
dwg_dynapi_entity_field (const char *restrict name, const char *restrict field)
{
  const Dwg_DYNAPI_field *fields = dwg_dynapi_entity_fields (name);
  if (fields)
    { /* linear search (unsorted) */
      Dwg_DYNAPI_field *f = (Dwg_DYNAPI_field *)fields;
      for (; f->name; f++)
        {
          if (strcmp (f->name, field) == 0)
            return f;
        }
    }
  return NULL;
}

/* generic field getters */
EXPORT bool
dwg_dynapi_entity_value (void *restrict _obj, const char *restrict name,
                         const char *restrict fieldname,
                         void *restrict out, Dwg_DYNAPI_field *restrict fp)
{
  if (!_obj || !name || !fieldname || !out)
    return false;
  {
    int error;
    const Dwg_Object* obj = dwg_obj_generic_to_object (_obj, &error);
    if (obj && strcmp (obj->name, name)) // objid may be 0
      {
        const int loglevel = obj->parent->opts & 0xf;
        LOG_ERROR ("%s: Invalid entity type %s, wanted %s", __FUNCTION__,
                   obj->name, name);
        return false;
      }
    {
      const Dwg_DYNAPI_field *f = dwg_dynapi_entity_field (name, fieldname);
      if (!f)
        {
          int loglevel;
          if (obj)
            loglevel = obj->parent->opts & 0xf;
          else
            loglevel = DWG_LOGLEVEL_ERROR;
          LOG_ERROR ("%s: Invalid %s field %s", __FUNCTION__, name, fieldname);
          return false;
        }
      if (fp)
        memcpy (fp, f, sizeof (Dwg_DYNAPI_field));
      memcpy (out, &((char *)_obj)[f->offset], f->size);
      return true;
    }
  }
}

EXPORT bool
dwg_dynapi_entity_utf8text (void *restrict _obj, const char *restrict name,
                            const char *restrict fieldname,
                            char **restrict out, Dwg_DYNAPI_field *restrict fp)
{
  if (!_obj || !name || !fieldname || !out)
    return false;
  {
    int error;
    const Dwg_Object* obj = dwg_obj_generic_to_object (_obj, &error);
    if (obj && strcmp (obj->name, name)) // objid may be 0
      {
        const int loglevel = obj->parent->opts & 0xf;
        LOG_ERROR ("%s: Invalid entity type %s, wanted %s", __FUNCTION__,
                   obj->name, name);
        return false;
      }
    {
      const Dwg_DYNAPI_field *f = dwg_dynapi_entity_field (name, fieldname);
      const Dwg_Version_Type dwg_version
          = obj ? obj->parent->header.version : R_INVALID;
      if (!f || !f->is_string)
        {
          int loglevel;
          if (obj)
            loglevel = obj->parent->opts & 0xf;
          else
            loglevel = DWG_LOGLEVEL_ERROR;
          LOG_ERROR ("%s: Invalid %s text field %s", __FUNCTION__, name, fieldname);
          return false;
        }
      if (fp)
        memcpy (fp, f, sizeof (Dwg_DYNAPI_field));

      if (dwg_version >= R_2007 && strcmp (f->type, "TF")) /* not TF */
        {
          BITCODE_TU wstr = *(BITCODE_TU*)((char*)_obj + f->offset);
          char *utf8 = bit_convert_TU (wstr);
          if (!utf8) // some conversion error, invalid wchar (nyi)
            return false;
          *out = utf8;
        }
      else
        {
          char *utf8 = *(char **)((char*)_obj + f->offset);
          *out = utf8;
        }

      return true;
    }
  }
}

EXPORT bool
dwg_dynapi_header_value (const Dwg_Data *restrict dwg,
                         const char *restrict fieldname, void *restrict out,
                         Dwg_DYNAPI_field *restrict fp)
{
  if (!dwg || !fieldname || !out)
    return false;
  {
    Dwg_DYNAPI_field *f = (Dwg_DYNAPI_field *)bsearch (
        fieldname, _dwg_header_variables_fields,
        ARRAY_SIZE (_dwg_header_variables_fields) - 1, /* NULL terminated */
        sizeof (_dwg_header_variables_fields[0]), _name_struct_cmp);
    if (f)
      {
        const Dwg_Header_Variables *const _obj = &dwg->header_vars;
        if (fp)
          memcpy (fp, f, sizeof (Dwg_DYNAPI_field));
        memcpy (out, &((char*)_obj)[f->offset], f->size);
        return true;
      }
    else
      {
        const int loglevel = dwg->opts & 0xf;
        LOG_ERROR ("%s: Invalid header field %s", __FUNCTION__, fieldname);
        return false;
      }
  }
}

EXPORT bool
dwg_dynapi_header_utf8text (const Dwg_Data *restrict dwg,
                            const char *restrict fieldname,
                            char **restrict out,
                            Dwg_DYNAPI_field *restrict fp)
{
  if (!dwg || !fieldname || !out)
    return false;
  {
    Dwg_DYNAPI_field *f = (Dwg_DYNAPI_field *)bsearch (
        fieldname, _dwg_header_variables_fields,
        ARRAY_SIZE (_dwg_header_variables_fields) - 1, /* NULL terminated */
        sizeof (_dwg_header_variables_fields[0]), _name_struct_cmp);
    if (f && f->is_string)
      {
        const Dwg_Header_Variables *const _obj = &dwg->header_vars;
        const Dwg_Version_Type dwg_version = dwg->header.version;
        
        if (fp)
          memcpy (fp, f, sizeof (Dwg_DYNAPI_field));

        if (dwg_version >= R_2007 && strcmp (f->type, "TF")) /* not TF */
          {
            BITCODE_TU wstr = *(BITCODE_TU*)((char*)_obj + f->offset);
            char *utf8 = bit_convert_TU (wstr);
            if (!utf8) // some conversion error, invalid wchar (nyi)
              return false;
            *out = utf8;
          }
        else
          {
            char *utf8 = *(char **)((char*)_obj + f->offset);
            *out = utf8;
          }

        return true;
      }
    else
      {
        const int loglevel = dwg->opts & 0xf;
        LOG_ERROR ("%s: Invalid header text field %s", __FUNCTION__, fieldname);
        return false;
      }
  }
}

EXPORT bool
dwg_dynapi_common_value(void *restrict _obj, const char *restrict fieldname,
                        void *restrict out, Dwg_DYNAPI_field *restrict fp)
{
  if (!_obj || !fieldname || !out)
    return false;
  {
    Dwg_DYNAPI_field *f;
    int error;
    const Dwg_Object *obj = dwg_obj_generic_to_object (_obj, &error);
    if (!obj)
      {
        const int loglevel = DWG_LOGLEVEL_ERROR;
        LOG_ERROR ("%s: dwg_obj_generic_to_object failed", __FUNCTION__);
        return false;
      }

    if (obj->supertype == DWG_SUPERTYPE_ENTITY)
      {
        _obj = obj->tio.entity;
        f = (Dwg_DYNAPI_field *)bsearch (
            fieldname, _dwg_object_entity_fields,
            ARRAY_SIZE (_dwg_object_entity_fields) - 1, /* NULL terminated */
            sizeof (_dwg_object_entity_fields[0]), _name_struct_cmp);
      }
    else if (obj->supertype == DWG_SUPERTYPE_OBJECT)
      {
        _obj = obj->tio.object;
        f = (Dwg_DYNAPI_field *)bsearch (
            fieldname, _dwg_object_object_fields,
            ARRAY_SIZE (_dwg_object_object_fields) - 1, /* NULL terminated */
            sizeof (_dwg_object_object_fields[0]), _name_struct_cmp);
      }
    else
      {
        const int loglevel = obj->parent->opts & 0xf; // DWG_LOGLEVEL_ERROR;
        LOG_ERROR ("%s: Unhandled %s.supertype ", __FUNCTION__, obj->name);
        return false;
      }

    if (f)
      {
        if (fp)
          memcpy (fp, f, sizeof(Dwg_DYNAPI_field));
        memcpy (out, &((char *)_obj)[f->offset], f->size);
        return true;
      }
    else
      {
        const int loglevel = obj->parent->opts & 0xf;
        LOG_ERROR ("%s: Invalid common field %s", __FUNCTION__, fieldname);
        return false;
      }
  }
}

EXPORT bool
dwg_dynapi_common_utf8text(void *restrict _obj, const char *restrict fieldname,
                           char **restrict out, Dwg_DYNAPI_field *restrict fp)
{
  if (!_obj || !fieldname || !out)
    return false;
  {
    Dwg_DYNAPI_field *f;
    int error;
    const Dwg_Object *obj = dwg_obj_generic_to_object (_obj, &error);
    Dwg_Data *dwg;

    if (!obj)
      {
        const int loglevel = DWG_LOGLEVEL_ERROR;
        LOG_ERROR ("%s: dwg_obj_generic_to_object failed", __FUNCTION__);
        return false;
      }
    if (obj->supertype == DWG_SUPERTYPE_ENTITY)
      {
        dwg = obj ? obj->parent : ((Dwg_Entity_UNKNOWN_ENT *)_obj)->parent->dwg;
        _obj = obj->tio.entity;
        f = (Dwg_DYNAPI_field *)bsearch (
            fieldname, _dwg_object_entity_fields,
            ARRAY_SIZE (_dwg_object_entity_fields) - 1, /* NULL terminated */
            sizeof (_dwg_object_entity_fields[0]), _name_struct_cmp);
      }
    else if (obj->supertype == DWG_SUPERTYPE_OBJECT)
      {
        dwg = obj ? obj->parent : ((Dwg_Object_UNKNOWN_OBJ *)_obj)->parent->dwg;
        _obj = obj->tio.object;
        f = (Dwg_DYNAPI_field *)bsearch (
            fieldname, _dwg_object_object_fields,
            ARRAY_SIZE (_dwg_object_object_fields) - 1, /* NULL terminated */
            sizeof (_dwg_object_object_fields[0]), _name_struct_cmp);
      }
    else
      {
        const int loglevel = DWG_LOGLEVEL_ERROR;
        LOG_ERROR ("%s: Unhandled %s.supertype ", __FUNCTION__, obj->name);
        return false;
      }

    if (f && f->is_string)
      {
        const Dwg_Version_Type dwg_version = dwg->header.version;

        if (fp)
          memcpy (fp, f, sizeof(Dwg_DYNAPI_field));

        if (dwg_version >= R_2007 && strcmp (f->type, "TF")) /* not TF */
          {
            BITCODE_TU wstr = *(BITCODE_TU*)((char*)_obj + f->offset);
            char *utf8 = bit_convert_TU (wstr);
            if (!utf8) // some conversion error, invalid wchar (nyi)
              return false;
            *out = utf8;
          }
        else
          {
            char *utf8 = *(char **)((char*)_obj + f->offset);
            *out = utf8;
          }

        return true;
      }
    else
      {
        const int loglevel = dwg->opts & 0xf;
        LOG_ERROR ("%s: Invalid common text field %s", __FUNCTION__, fieldname);
        return false;
      }
  }
}

static void
dynapi_set_helper (void *restrict old, const Dwg_DYNAPI_field *restrict f,
                   const Dwg_Version_Type dwg_version,
                   const void *restrict value, const bool is_utf8)
{
  // TODO: sanity checks
  // if text strcpy or wcscpy, or do utf8 conversion
  if (f->is_string)
    {
      char *str;
      //ascii or wide?
      if (!strcmp (f->type, "TF")
          || ((!strcmp (f->type, "T") || !strcmp (f->type, "TV"))
              && dwg_version < R_2007))
        {
          str = malloc (strlen ((char*)value)+1);
          strcpy (str, value);
          memcpy (old, str, f->size); // size of ptr
        }
      else if (!strcmp(f->type, "TU")
               || ((!strcmp(f->type, "T") || !strcmp(f->type, "TV"))
                   && dwg_version >= R_2007))
        {
#if defined(HAVE_WCHAR_H) && defined(SIZEOF_WCHAR_T) && SIZEOF_WCHAR_T == 2
          str = malloc (2 * (wcslen ((wchar_t *)value) + 1));
          wcscpy ((wchar_t *)str, value);
#else
          int length = 0;
          for (; ((BITCODE_TU)value)[length]; length++)
            ;
          length++;
          str = malloc (2 * length);
          memcpy (str, value, length * 2);
#endif
          memcpy (old, str, f->size); // size of ptr
        }
    }
  else
    memcpy (old, value, f->size);
  if (f->is_malloc)
    free (old);
}

/* generic field setters */
EXPORT bool
dwg_dynapi_entity_set_value (void *restrict _obj, const char *restrict name,
                             const char *restrict fieldname,
                             const void *restrict value, const bool is_utf8)
{
  if (!_obj || !fieldname || !value) // cannot set NULL value
    return false;
  {
    int error;
    const Dwg_Object *obj = dwg_obj_generic_to_object (_obj, &error);
    if (obj && strcmp (obj->name, name))
      {
        const int loglevel = obj->parent->opts & 0xf;
        LOG_ERROR ("%s: Invalid entity type %s, wanted %s", __FUNCTION__,
                   obj->name, name);
        return false;
      }
    {
      void *old;
      const Dwg_DYNAPI_field *f = dwg_dynapi_entity_field (name, fieldname);
      const Dwg_Data *dwg
        = obj ? obj->parent
              : ((Dwg_Object_UNKNOWN_OBJ *)_obj)->parent->dwg;
      const Dwg_Version_Type dwg_version = dwg->header.version;

      if (!f)
        {
          const int loglevel = dwg->opts & 0xf;
          LOG_ERROR ("%s: Invalid %s field %s", __FUNCTION__, name, fieldname);
          return false;
        }

      old = &((char*)_obj)[f->offset];
      dynapi_set_helper (old, f, dwg_version, value, is_utf8);
      return true;
    }
  }
}

EXPORT bool
dwg_dynapi_header_set_value (const Dwg_Data *restrict dwg,
                             const char *restrict fieldname,
                             const void *restrict value, const bool is_utf8)
{
  if (!dwg || !fieldname || !value) // cannot set NULL value
    return false;
  {
    Dwg_DYNAPI_field *f = (Dwg_DYNAPI_field *)bsearch (
        fieldname, _dwg_header_variables_fields,
        ARRAY_SIZE (_dwg_header_variables_fields) - 1, /* NULL terminated */
        sizeof (_dwg_header_variables_fields[0]), _name_struct_cmp);
    if (f && is_utf8)
      {
        if (f->is_string)
          {
            const Dwg_Header_Variables *const _obj = &dwg->header_vars;
          }
        else
          {
            const int loglevel = dwg->opts & 0xf;
            LOG_ERROR ("%s: Invalid header text field %s", __FUNCTION__, fieldname);
            return false;
          }
      }
    if (f)
      {
        // there are no malloc'd fields in the HEADER, so no need to free().
        const Dwg_Header_Variables *const _obj = &dwg->header_vars;
        memcpy (&((char *)_obj)[f->offset], value, f->size);
        return true;
      }
    else
      {
        const int loglevel = dwg->opts & 0xf;
        LOG_ERROR ("%s: Invalid header field %s", __FUNCTION__, fieldname);
        return false;
      }
  }
}

EXPORT bool
dwg_dynapi_common_set_value (void *restrict _obj,
                             const char *restrict fieldname,
                             const void *restrict value, const bool is_utf8)
{
  if (!_obj || !fieldname || !value)
    return false;
  {
    Dwg_DYNAPI_field *f;
    int error;
    void *old;
    const Dwg_Object *obj = dwg_obj_generic_to_object (_obj, &error);
    if (!obj)
      {
        const int loglevel = DWG_LOGLEVEL_ERROR;
        LOG_ERROR ("%s: dwg_obj_generic_to_object failed", __FUNCTION__);
        return false;
      }
    if (obj->supertype == DWG_SUPERTYPE_ENTITY)
      {
        _obj = obj->tio.entity;
        f = (Dwg_DYNAPI_field *)bsearch (
            fieldname, _dwg_object_entity_fields,
            ARRAY_SIZE (_dwg_object_entity_fields) - 1, /* NULL terminated */
            sizeof (_dwg_object_entity_fields[0]), _name_struct_cmp);
      }
    else if (obj->supertype == DWG_SUPERTYPE_OBJECT)
      {
        _obj = obj->tio.object;
        f = (Dwg_DYNAPI_field *)bsearch (
            fieldname, _dwg_object_object_fields,
            ARRAY_SIZE (_dwg_object_object_fields) - 1, /* NULL terminated */
            sizeof (_dwg_object_object_fields[0]), _name_struct_cmp);
      }
    else
      {
        const int loglevel = DWG_LOGLEVEL_ERROR;
        LOG_ERROR ("%s: Unhandled %s.supertype ", __FUNCTION__, obj->name);
        return false;
      }

    if (!f)
      {
        const int loglevel = obj->parent->opts & 0xf;
        LOG_ERROR ("%s: Invalid %s common field %s", __FUNCTION__, obj->name, fieldname);
        return false;
      }

    old = &((char*)_obj)[f->offset];
    dynapi_set_helper (old, f, obj->parent->header.version, value, is_utf8);
    return true;
  }
}

// check if the handle points to an object with a name.
// see also dwg_obj_table_get_name, which only supports tables.
EXPORT char*
dwg_dynapi_handle_name (const Dwg_Data *restrict dwg, Dwg_Object_Ref *restrict hdl)
{
  char *name;
  const Dwg_Version_Type dwg_version = dwg->header.version;
  Dwg_Object *obj = dwg_ref_object (dwg, hdl);

  if (!obj)
    return NULL;
  {
    const Dwg_DYNAPI_field *f = dwg_dynapi_entity_field (obj->name, "name");
    // just some random type is enough.
    Dwg_Object_STYLE *_obj = obj->tio.object->tio.STYLE;

    if (!f || !f->is_string)
      return NULL;
    if (dwg_version >= R_2007 && strcmp (f->type, "TF")) /* not TF */
      {
        BITCODE_TU wstr = *(BITCODE_TU *)((char *)_obj + f->offset);
        return bit_convert_TU (wstr);
      }
    else
      {
        return *(char **)((char *)_obj + f->offset);
      }
  }
}

/* Local Variables: */
/* mode: c */
/* End: */
