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

use strict;

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
my %skip;

if (0) {
  open my $skip_fh, "<", "examples/alldwg.skip"
    or warn "examples/alldwg.skip missing";
  while (<$skip_fh>) {
    my @F = split(' ');
    shift @F;
    # NAME-HANDLE-BITSIZE
    $F[2] =~ s/^0x//;
    $skip{"$F[0]-$F[2]-$F[3]"} = \@F;
  }
  close $skip_fh;
}

my $known = {
  ACAD_EVALUATION_GRAPH => [
    96 => 'has_graph',
    97 => 'unknown1',
    91 => 'nodeid',
    93 => 'edge_flags',
    95 => 'num_evalexpr',
    92 => 'node_edge1',
    92 => 'node_edge2',
    92 => 'node_edge3',
    92 => 'node_edge4',
    360 => 'evalexpr'
    ],
  MATERIAL => [
    1 => 'name',
    2 => 'desc',
    70 => 'ambient_color_flag',
    40 => 'ambient_color_factor',
    90 => 'ambient_color',
    71 => 'diffuse_color_flag',
    41 => 'diffuse_color_factor',
    92 => 'diffuse_color',
    72 => 'diffusemap_source',
    3  => 'diffusemap_filename',
    42 => 'diffusemap_blendfactor',
    73 => 'diffusemap_projection',
    74 => 'diffusemap_tiling',
    75 => 'diffusemap_autotransform',
    43 => 'diffusemap_transmatrix', #x16
    44 => 'specular_gloss_factor',
    76 => 'specular_color_flag',
    45 => 'specular_color_factor',
    92 => 'specular_color',
    77 => 'specularmap_source',
    4  => 'specularmap_filename',
    46 => 'specularmap_blendfactor',
    78 => 'specularmap_projection',
    74 => 'specularmap_tiling',
    170 => 'specularmap_autotransform',
    47 => 'specularmap_transmatrix',
    171 => 'reflectionmap_source',
    6   => 'reflectionmap_filename',
    48  => 'reflectionmap_blendfactor',
    172 => 'reflectionmap_projection',
    173 => 'reflectionmap_tiling',
    174 => 'reflectionmap_autotransform',
    49  => 'reflectionmap_transmatrix',
    140 => 'opacity_percent',
    175 => 'opacitymap_source',
    7   => 'opacitymap_filename',
    141 => 'opacitymap_blendfactor',
    176 => 'opacitymap_projection',
    177 => 'opacitymap_tiling',
    178 => 'opacitymap_autotransform',
    142 => 'opacitymap_transmatrix',
    179 => 'bumpmap_source',
    8   => 'bumpmap_filename',
    143 => 'bumpmap_blendfactor',
    270 => 'bumpmap_projection',
    271 => 'bumpmap_tiling',
    272 => 'bumpmap_autotransform',
    144 => 'bumpmap_transmatrix',
    145 => 'refraction_index',
    273 => 'refractionmap_source',
    9   => 'refractionmap_filename',
    146 => 'refractionmap_blendfactor',
    274 => 'refractionmap_projection',
    275 => 'refractionmap_tiling',
    276 => 'refractionmap_autotransform',
    147 => 'refractionmap_transmatrix',
    460 => 'color_bleed_scale',
    461 => 'indirect_dump_scale',
    462 => 'reflectance_scale',
    463 => 'transmittance_scale',
    290 => 'two_sided_material',
    464 => 'luminance',
    270 => 'luminance_mode',
    271 => 'normalmap_method',
    465 => 'normalmap_strength',
    72 => 'normalmap_source',
    3   => 'normalmap_filename',
    42 => 'normalmap_blendfactor',
    73 => 'normalmap_projection',
    74 => 'normalmap_tiling',
    75 => 'normalmap_autotransform',
    43 => 'normalmap_transmatrix',
    293 => 'materials_anonymous',
    272 => 'global_illumination_mode',
    273 => 'final_gather_mode',
    300 => 'genprocname',
    291 => 'genprocvalbool',
    271 => 'genprocvalint',
    469 => 'genprocvalreal',
    301 => 'genprocvaltext',
    292 => 'genproctableend',
    62 => 'genprocvalcolorindex',
    420 => 'genprocvalcolorrgb',
    430 => 'genprocvalcolorname',
    270 => 'map_utile',
    148 => 'translucence',
    90 => 'self_illumination',
    468 => 'reflectivity',
    93 => 'illumination_model',
    94 => 'channel_flags',
    ],
  PLOTSETTINGS => [
    1 => 'page_setup_name',
    2 => 'printer_cfg_file',
    4 => 'paper_size',
    6 => 'plotview',
    40 => 'left_margin',
    41 => 'bottom_margin',
    42 => 'right_margin',
    43 => 'top_margin',
    44 => 'paper_width',
    45 => 'paper_height',
    46 => 'plot_origin',
    48 => 'plot_window_ll',
    140 => 'plot_window_ur',
    142 => 'num_custom_print_scale',
    143 => 'den_custom_print_scale',
    70 => 'plot_layout',
    72 => 'plot_paper_units',
    73 => 'plot_rotation',
    74 => 'plot_type',
    7 => 'stylesheet',
    #0 => 'use_std_scale',
    75 => 'std_scale_type',
    147 => 'std_scale_factor',
    148 => 'paper_image_origin',
    76 => 'shade_plot_mode',
    77 => 'shade_plot_res_level',
    78 => 'shade_plot_custom_dpi',
    333 => 'shade_plot_id',
    ],
  ASSOCNETWORK => [
    #100 => 'AcDbAssocAction',
    90 => 'status',
    90 => 'num_deps',
    330 => 'readdep',
    360 => 'writedep',
    90 => 'unknown_assoc',
    #100 => 'AcDbAssocNetwork',
    90 => 'unknown_n1',
    90 => 'unknown_n2',
    90 => 'num_actions',
    330 => 'actions',
    ],
  LIGHT => [
    90 => 'class_version',
    1 => 'name',
    70 => 'type',
    290 => 'status',
    63 => 'color',
    291 => 'plot_glyph',
    40 => 'intensity',
    10 => 'position',
    11 => 'target',
    72 => 'attenuation_type',
    292 => 'use_attenuation_limits',
    41 => 'attenuation_start_limit',
    42 => 'attenuation_end_limit',
    50 => 'hotspot_angle',
    51 => 'falloff_angle',
    293 => 'cast_shadows',
    73 => 'shadow_type',
    91 => 'shadow_map_size',
    280 => 'shadow_map_softness',
    #0 => 'lamp_color_preset',
    #0 => 'lamp_color_type',
    #0 => 'physical_intensity_method',
    #0 => 'drawable_type',
    #0 => 'glyph_display_type',
    #0 => 'glyph_display',
    ],
  SUN => [
    90 => 'class_version',
    290 => 'is_on',
    291 => 'has_shadow',
    292 => 'is_dst',
    421 => 'unknown',
    40 => 'intensity',
    291 => 'has_shadow',
    91 => 'julian_day',
    92 => 'time',
    292 => 'is_dst',
    #0 => 'altitude',
    #0 => 'azimuth',
    #0 => 'direction',
    70 => 'shadow_type',
    280 => 'shadow_softness',
    71 => 'shadow_mapsize',
    330 => 'parenthandle',
    63 => 'color',
    ],
  UNDERLAY => [
    10 => 'insertion_pt',
    41 => 'scale',
    50 => 'angle',
    210 => 'extrusion',
    11 => 'clip_boundary->pt',
    280 => 'flag',
    281 => 'contrast',
    282 => 'fade',
    340 => 'definition_id',
    ],
  HELIX => [
    90 => 'major_version',
    91 => 'maint_version',
    10 => 'axis_base_pt',
    12 => 'start_pt',
    40 => 'radius',
    41 => 'num_turns',
    42 => 'height',
    290 => 'handedness',
    280 => 'constraint_type',
    ],
};

print $f1 "/* code, value, bytes, bitsize, type, name, num, pos[] */\n";

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
  my $obj   = substr($F[1],1,-2); # "MATERIAL",
  my $bytes = substr($F[2],1,-2);
  my $bits  = substr($F[3],1,-2);
  my $hdl   = substr($F[6],2,-1); # 0xXXX,
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
  my ($react, $xdict, $seen100, @avail);
  @avail = @{$known->{$obj}} if exists $known->{$obj};
  while (my $code = <$f>) {
    $code =~ s/\cM\cJ//;
    my $v = <$f>;
    my $name = "";
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
          emit_field($f1, $_->[0], $_->[1], $_->[2],, $num);
        }
        @FIELD = ();
        print $f0 " NULL },\n";
        print $f1 "    { 0, NULL, NULL, 0, BITS_UNKNOWN, NULL, 0, {-1,-1,-1,-1,-1}}\n};\n";
      }
    }
    if ($foundobj and $code =~ /^ *5$/) {
      $name = "handle";
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
      my $x;
      $code =~ s/\s//g;
      # fill-in the field name, from heuristics and some already known classes
      if ($code == 5) {
        $name = "handle";
      } elsif ($code == 330) {
        if ($react) {
          $react--;
          $name = "reactors[$react]";
          $react++; $react++;
        } elsif (!$seen100) {
          $name = "parenthandle";
        }
      } elsif ($code == 360 and $xdict) {
        $name = "xdicobjhandle";
      } elsif ($code == 100) {
        $seen100++;
      } elsif ($code == 102) {
        if ($v eq '{ACAD_REACTORS') {
          $react = 1;
        } elsif ($v eq '{ACAD_XDICTIONARY') {
          $xdict = 1;
        } elsif ($v eq '}') {
          $xdict = 0; $react = 0;
        }
      } elsif ($x = find_name($code, $obj, \@avail, \@FIELD)) {
        $name = $x;
      } elsif ($code == 8) {
        $name = "layer";
      } elsif ($code >= 60 && $code <= 65) {
        $name = "color";
      } elsif ($code == 7) {
        $name = "ltype";
      }
      push @FIELD, [$code, $v, $name];
    }
  }
  close $f;
}
close $f0; close $f1; close $f2;

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

sub find_name {
  my ($code, $obj, $avail, $fieldsref) = @_;
  return undef if !$avail;
  my @f = @$avail;
  my $len = scalar @f;
  for my $i (0..(2*$len)-1) {
    my $c = $f[$i];
    if ($c == $code) {
      # delete ($c,$n) from $avail
      splice @$avail, $i, 2;
      my $n = $f[$i+1];
      return $n;
    }
  }
  return undef;
}

sub emit_field {
  my ($f, $code, $v, $name, $num) = @_;
  #warn "$code: $v\n";
  #return if $code == 100;
  $v =~ s/\\/\\\\/g;
  $v =~ s/"/\\"/g;
  $num = 0 unless $num;
  # code, value, bytes, bitsize, type, name, num, pos[]
  print $f "    { $code, \"$v\", NULL, 0, BITS_UNKNOWN, \"$name\", $num, {-1,-1,-1,-1,-1} },\n";
}

