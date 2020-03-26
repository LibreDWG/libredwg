#!/usr/bin/perl
=head1 USAGE

    perl examples/log_unknown_dxf.pl examples/alldwg.inc
    make -C examples alldxf_0.inc

    Creates the 3 examples/alldxf_*.inc includes from the examples/alldwg.inc
    parsed from the logs, and mixed with the values from the matching DXF sources.

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
use File::Basename qw(dirname);

# DXF types
use constant UNKNOWN => 0;
use constant B       => 1;
use constant NUM     => 2; #BL, BS, RL, RS or RC
use constant HDL     => 3;
use constant STR     => 4;
use constant HEXSTR  => 5;
use constant DBL     => 6;

#require dirname(__FILE__)."/unstable.pm";
my $dir = dirname(__FILE__);
open my $f0, ">", "$dir/alldxf_0.inc" or die "$!";
open my $f1a, ">", "$dir/alldxf_1.inc" or die "$!";
open my $f2, ">", "$dir/alldxf_2.inc" or die "$!";
my $i = 0;
my (%skip, %dupl);

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

my @ASSOCACTION_fields =
  ( 90 => 'solution_status',
    90 => 'geometry_status',
    330 => 'readdep',
    360 => 'writedep',
    90 => 'constraint_status',
    90 => 'dof',
    90 => 'is_body_a_proxy'
  );

my @AcDbAssocPathBasedSurfaceActionBody =
  (100 => 'AcDbAssocActionBody',
    90 => 'aab_status',
    100 => 'AcDbAssocParamBasedActionBody',
    90 => 'pab_status',
    90 => 'pab_l2',
    90 => 'num_deps',
    360 => 'writedeps',
    90 => 'pab_l4',
    90 => 'pab_l5',
    330 => 'readdeps',
    1 => 'description',
    100 => 'AcDbAssocSurfaceActionBody',
    90 => 'sab_status',
    290 => 'sab_b1',
    90 => 'sab_l2',
    290 => 'sab_b2',
    70 => 'sab_s1',
    100 => 'AcDbAssocPathBasedSurfaceActionBody',
   90 => 'pbsab_status');
my @DIMASSOC_Ref = (
  1 => 'DIMASSOC_Ref.classname',
  72 => 'DIMASSOC_Ref.osnap_type',
  331 => 'DIMASSOC_Ref.mainobjs',
  73 => 'DIMASSOC_Ref.main_subent_type',
  91 => 'DIMASSOC_Ref.main_gsmarker',
  301 => 'DIMASSOC_Ref.xrefobj',
  40 => 'DIMASSOC_Ref.osnap_dist',
  10 => 'DIMASSOC_Ref.osnap_pt.x',
  20 => 'DIMASSOC_Ref.osnap_pt.y',
  30 => 'DIMASSOC_Ref.osnap_pt.z',
  75 => 'DIMASSOC_Ref.has_lastpt_ref',
  # rarely:
  332 => 'DIMASSOC_Ref.intsectobj',
  74 => 'DIMASSOC_Ref.intsect_subent_type');

# dxf names, not obj names.
# The name in the all*.inc files.
# which comes from the (?:Unhandled|Unstable) Class (object|entity) \d+ (\w+) warning
# which uses the klass->dxfname.
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
  DBCOLOR => [
    62  => 'color',
    420 => 'rgb',
    430 => 'name',
    330 => 'ownerhandle',
    ],
  MATERIAL => [
    330 => 'ownerhandle',
    1 => 'name',
    2 => 'description',
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
    43 => 'diffusemap_transmatrix[0]', #x16
    43 => 'diffusemap_transmatrix[1]',
    43 => 'diffusemap_transmatrix[2]',
    43 => 'diffusemap_transmatrix[3]',
    43 => 'diffusemap_transmatrix[4]',
    43 => 'diffusemap_transmatrix[5]',
    43 => 'diffusemap_transmatrix[6]',
    43 => 'diffusemap_transmatrix[7]',
    43 => 'diffusemap_transmatrix[8]',
    43 => 'diffusemap_transmatrix[9]',
    43 => 'diffusemap_transmatrix[10]',
    43 => 'diffusemap_transmatrix[11]',
    43 => 'diffusemap_transmatrix[12]',
    43 => 'diffusemap_transmatrix[13]',
    43 => 'diffusemap_transmatrix[14]',
    43 => 'diffusemap_transmatrix[15]',
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
    47 => 'specularmap_transmatrix[0]',
    47 => 'specularmap_transmatrix[1]',
    47 => 'specularmap_transmatrix[2]',
    47 => 'specularmap_transmatrix[3]',
    47 => 'specularmap_transmatrix[4]',
    47 => 'specularmap_transmatrix[5]',
    47 => 'specularmap_transmatrix[6]',
    47 => 'specularmap_transmatrix[7]',
    47 => 'specularmap_transmatrix[8]',
    47 => 'specularmap_transmatrix[9]',
    47 => 'specularmap_transmatrix[10]',
    47 => 'specularmap_transmatrix[11]',
    47 => 'specularmap_transmatrix[12]',
    47 => 'specularmap_transmatrix[13]',
    47 => 'specularmap_transmatrix[14]',
    47 => 'specularmap_transmatrix[15]',
    171 => 'reflectionmap_source',
    6   => 'reflectionmap_filename',
    48  => 'reflectionmap_blendfactor',
    172 => 'reflectionmap_projection',
    173 => 'reflectionmap_tiling',
    174 => 'reflectionmap_autotransform',
    49  => 'reflectionmap_transmatrix[0]',
    49 => 'reflectionmap_transmatrix[1]',
    49 => 'reflectionmap_transmatrix[2]',
    49 => 'reflectionmap_transmatrix[3]',
    49 => 'reflectionmap_transmatrix[4]',
    49 => 'reflectionmap_transmatrix[5]',
    49 => 'reflectionmap_transmatrix[6]',
    49 => 'reflectionmap_transmatrix[7]',
    49 => 'reflectionmap_transmatrix[8]',
    49 => 'reflectionmap_transmatrix[9]',
    49 => 'reflectionmap_transmatrix[10]',
    49 => 'reflectionmap_transmatrix[11]',
    49 => 'reflectionmap_transmatrix[12]',
    49 => 'reflectionmap_transmatrix[13]',
    49 => 'reflectionmap_transmatrix[14]',
    49 => 'reflectionmap_transmatrix[15]',
    140 => 'opacity_percent',
    175 => 'opacitymap_source',
    7   => 'opacitymap_filename',
    141 => 'opacitymap_blendfactor',
    176 => 'opacitymap_projection',
    177 => 'opacitymap_tiling',
    178 => 'opacitymap_autotransform',
    142 => 'opacitymap_transmatrix[0]',
    142 => 'opacitymap_transmatrix[1]',
    142 => 'opacitymap_transmatrix[2]',
    142 => 'opacitymap_transmatrix[3]',
    142 => 'opacitymap_transmatrix[4]',
    142 => 'opacitymap_transmatrix[5]',
    142 => 'opacitymap_transmatrix[6]',
    142 => 'opacitymap_transmatrix[7]',
    142 => 'opacitymap_transmatrix[8]',
    142 => 'opacitymap_transmatrix[9]',
    142 => 'opacitymap_transmatrix[10]',
    142 => 'opacitymap_transmatrix[11]',
    142 => 'opacitymap_transmatrix[12]',
    142 => 'opacitymap_transmatrix[13]',
    142 => 'opacitymap_transmatrix[14]',
    142 => 'opacitymap_transmatrix[15]',
    179 => 'bumpmap_source',
    8   => 'bumpmap_filename',
    143 => 'bumpmap_blendfactor',
    270 => 'bumpmap_projection',
    271 => 'bumpmap_tiling',
    272 => 'bumpmap_autotransform',
    144 => 'bumpmap_transmatrix[0]',
    144 => 'bumpmap_transmatrix[1]',
    144 => 'bumpmap_transmatrix[2]',
    144 => 'bumpmap_transmatrix[3]',
    144 => 'bumpmap_transmatrix[4]',
    144 => 'bumpmap_transmatrix[5]',
    144 => 'bumpmap_transmatrix[6]',
    144 => 'bumpmap_transmatrix[7]',
    144 => 'bumpmap_transmatrix[8]',
    144 => 'bumpmap_transmatrix[9]',
    144 => 'bumpmap_transmatrix[10]',
    144 => 'bumpmap_transmatrix[11]',
    144 => 'bumpmap_transmatrix[12]',
    144 => 'bumpmap_transmatrix[13]',
    144 => 'bumpmap_transmatrix[14]',
    144 => 'bumpmap_transmatrix[15]',
    145 => 'refraction_index',
    273 => 'refractionmap_source',
    9   => 'refractionmap_filename',
    146 => 'refractionmap_blendfactor',
    274 => 'refractionmap_projection',
    275 => 'refractionmap_tiling',
    276 => 'refractionmap_autotransform',
    147 => 'refractionmap_transmatrix[0]',
    147 => 'refractionmap_transmatrix[1]',
    147 => 'refractionmap_transmatrix[2]',
    147 => 'refractionmap_transmatrix[3]',
    147 => 'refractionmap_transmatrix[4]',
    147 => 'refractionmap_transmatrix[5]',
    147 => 'refractionmap_transmatrix[6]',
    147 => 'refractionmap_transmatrix[7]',
    147 => 'refractionmap_transmatrix[8]',
    147 => 'refractionmap_transmatrix[9]',
    147 => 'refractionmap_transmatrix[10]',
    147 => 'refractionmap_transmatrix[11]',
    147 => 'refractionmap_transmatrix[12]',
    147 => 'refractionmap_transmatrix[13]',
    147 => 'refractionmap_transmatrix[14]',
    147 => 'refractionmap_transmatrix[15]',
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
    43 => 'normalmap_transmatrix[0]',
    43 => 'normalmap_transmatrix[1]',
    43 => 'normalmap_transmatrix[2]',
    43 => 'normalmap_transmatrix[3]',
    43 => 'normalmap_transmatrix[4]',
    43 => 'normalmap_transmatrix[5]',
    43 => 'normalmap_transmatrix[6]',
    43 => 'normalmap_transmatrix[7]',
    43 => 'normalmap_transmatrix[8]',
    43 => 'normalmap_transmatrix[9]',
    43 => 'normalmap_transmatrix[10]',
    43 => 'normalmap_transmatrix[11]',
    43 => 'normalmap_transmatrix[12]',
    43 => 'normalmap_transmatrix[13]',
    43 => 'normalmap_transmatrix[14]',
    43 => 'normalmap_transmatrix[15]',
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
  ACDBASSOCACTION => [
    @ASSOCACTION_fields,
    90 => 'num_deps',
    90 => 'status',
    ],
  ACDBASSOCDEPENDENCY => [
    #100 => 'AcDbAssocDependency',
    90 => 'class_version',
    90 => 'status',
    290 => 'isread_dep',
    290 => 'iswrite_dep',
    290 => 'isobjectstate_dep',
    290 => 'unknown_b4',
    90 => 'order',
    290 => 'unknown_b5',
    #330 => 'ownerhandle',
    330 => 'owner',
    330 => 'readdep',
    330 => 'node',
    360 => 'writedep',
    90 => 'depbodyid',
    ],
  ACDBASSOCALIGNEDDIMACTIONBODY => [
    90 => 'aab_status',
    #100 => 'AcDbAssocParamBasedActionBody',
    90 => 'pab_status',
    90 => 'pab_l2',
    90 => 'num_deps',
    360 => 'writedep',
    90 => 'pab_l4',
    90 => 'pab_l5',
    330 => 'readdep',
    #100 => ACDBASSOCALIGNEDDIMACTIONBODY,
    90 => 'dcm_status',
    330 => 'd_node',
    330 => 'r_node',
    ],
  ACDBASSOCNETWORK => [
    @ASSOCACTION_fields,
    #100 => 'AcDbAssocNetwork',
    90 => 'unknown_n1',
    90 => 'unknown_n2',
    90 => 'num_actions',
    330 => 'actions',
    ],
  ACDBASSOCPLANESURFACEACTIONBODY => [
    @AcDbAssocPathBasedSurfaceActionBody,
    100 => 'AcDbAssocPlaneSurfaceActionBody',
    90 => 'psab_status',
    ],
  ACDBASSOCEXTRUDEDSURFACEACTIONBODY => [
    @AcDbAssocPathBasedSurfaceActionBody,
    100 => 'AcDbAssocExtrudedSurfaceActionBody',
    90 => 'esab_status',
  ],
  ACDBASSOCLOFTEDSURFACEACTIONBODY => [
    @AcDbAssocPathBasedSurfaceActionBody,
    100 => 'AcDbAssocLoftedSurfaceActionBody',
    90 => 'lsab_status',
  ],
  ACDBASSOCREVOLVEDSURFACEACTIONBODY => [
    @AcDbAssocPathBasedSurfaceActionBody,
    100 => 'AcDbAssocRevolvedSurfaceActionBody',
    90 => 'rsab_status',
  ],
  ACDBASSOCSWEPTSURFACEACTIONBODY => [
    @AcDbAssocPathBasedSurfaceActionBody,
    100 => 'AcDbAssocSweptSurfaceActionBody',
    90 => 'ssab_status',
    ],
  ACDBPERSSUBENTMANAGER => [
    100 => 'AcDbPersSubentManager',
    90 => 'class_version',
    90 => 'unknown_bl1',
    90 => 'unknown_bl2',
    90 => 'unknown_bl3',
    90 => 'unknown_bl4',
    90 => 'unknown_bl5',
    90 => 'unknown_bl6',
    ],
  ACDBASSOCPERSSUBENTMANAGER => [
    100 => 'AcDbAssocPersSubentManager',
    90 => 'class_version',
    90 => 'unknown_bl1',
    90 => 'unknown_bl2',
    90 => 'unknown_bl3',
    90 => 'unknown_bl4',
    90 => 'unknown_bl5',
    90 => 'unknown_bl6',
    90 => 'unknown_bl6a',
    90 => 'unknown_bl7a',
    90 => 'unknown_bl7',
    90 => 'unknown_bl7',
    90 => 'unknown_bl8',
    90 => 'unknown_bl9',
    90 => 'unknown_bl10',
    90 => 'unknown_bl11',
    90 => 'unknown_bl12',
    90 => 'unknown_bl13',
    90 => 'unknown_bl14',
    90 => 'unknown_bl15',
    90 => 'unknown_bl16',
    90 => 'unknown_bl17',
    90 => 'unknown_bl18',
    90 => 'unknown_bl19',
    90 => 'unknown_bl20',
    90 => 'unknown_bl21',
    90 => 'unknown_bl22',
    90 => 'unknown_bl23',
    90 => 'unknown_bl24',
    90 => 'unknown_bl25',
    90 => 'unknown_bl26',
    90 => 'unknown_bl27',
    90 => 'unknown_bl28',
    90 => 'unknown_bl29',
    90 => 'unknown_bl30',
    90 => 'unknown_bl31',
    90 => 'unknown_bl32',
    90 => 'unknown_bl33',
    90 => 'unknown_bl34',
    90 => 'unknown_bl35',
    90 => 'unknown_bl36',
    290 => 'unknown_b37',
    ],
  ACDBASSOC2DCONSTRAINTGROUP => [
    @ASSOCACTION_fields,
    10 => 'workplane',
    360 => 'h1',
    90 => 'l6',
    360 => 'h2',
    360 => 'h3',
    90 => 'l7',
    90 => 'l8',
    1  => 't1', # AcConstrainedCircle
    330 => 'h4',
    90 => 'cl1',
    70 => 'cs1',
    90 => 'cl2',
    90 => 'cl3',
    330 => 'h5',
    90 => 'cl4',
    10 => 'c1',
    10 => 'c2',
    10 => 'c3',
    40 => 'w1',
    40 => 'w2',
    40 => 'w3',
    1 => 't2', # AcConstrainedImplicitPoint
    ],
  ACDBASSOCOSNAPPOINTREFACTIONPARAM => [
    90 => 'status',
    90 => 'flags',
    90 => 'num_actions',
    360 => 'writedep',
    330 => 'actions',
    #40 => 'unknown3',
    #...
    ],
  ACDBASSOCGEOMDEPENDENCY => [
    # 100 AcDbAssocDependency
    90 => 'class_version',
    90 => 'dependent_on_object_status',
    290 => 'has_cached_value',
    290 => 'is_actionevaluation_in_progress',
    290 => 'is_attached_to_object',
    290 => 'b290_4',
    90 => 'bs90_2',
    330 => 'h330_1',
    290 => 'b290_5',
    330 => 'h330_2',
    330 => 'h330_3',
    360 => 'h360',
    90 => 'bs90_3',
    # 100 AcDbAssocGeomDependency
    90 => 'bs90_4',
    290 => 'b290_6',
    1 => 't', # AcDbAssocSingleEdgePersSubentId
    290 => 'dependent_on_compound_object',
    ],
  ACSH_BOX_CLASS => [
    # 100 => AcDbEvalExpr
    90 => 'class_version',
    98 => 'ee_bl98',
    99 => 'ee_bl99',
    # 100 => AcDbShHistoryNode
    90 => 'shhn_bl90',
    91 => 'shhn_bl91',
    40 => 'shhn_pts',
    62 => 'color',
    92 => 'shhn_b92',
    347 => 'shhn_bl347',
    # 100 => AcDbShPrimitive
    # 100 => AcDbShBox
    90 => 'shb_bl90',
    91 => 'shb_bl91',
    40 => 'shb_bd40',
    41 => 'shb_bd41',
    42 => 'shb_bd42',
    ],
  ACSH_SWEEP_CLASS => [
    # 100 => AcDbEvalExpr
    90 => 'class_version',
    98 => 'ee_bl98',
    99 => 'ee_bl99',
    # 100 => AcDbShHistoryNode
    90 => 'shhn_bl90',
    91 => 'shhn_bl91',
    40 => 'shhn_pts',
    62 => 'color',
    92 => 'shhn_b92',
    347 => 'shhn_bl347',
    # 100 => AcDbShPrimitive
    # 100 => AcDbShSweepBase
    90 => 'shsw_bl90',
    91 => 'shsw_bl91',
    10 => 'basept',
    92 => 'shsw_bl92',
    90 => 'shsw_text_size',
    310 => 'shsw_text',
    93 => 'shsw_bl93',
    90 => 'shsw_text2_size',
    310 => 'shsw_text2',
    42 => 'draft_angle',
    43 => 'start_draft_dist',
    44 => 'end_draft_dist',
    45 => 'scale_factor',
    48 => 'twist_angle',
    49 => 'align_angle',
    46 => 'sweepentity_transform',
    47 => 'pathentity_transform',
    70 => 'align_option',
    71 => 'miter_option',
    290 => 'has_align_start',
    292 => 'bank',
    293 => 'check_intersections',
    294 => 'shsw_b294',
    295 => 'shsw_b295',
    296 => 'shsw_b296',
    11 => 'pt2',
    ],
  ACSH_HISTORY_CLASS => [
    # 100 => AcDbEvalExpr
    90 => 'class_version',
    98 => 'ee_bl98',
    99 => 'ee_bl99',
    # 100 => AcDbShHistoryNode
    90 => 'shhn_bl90',
    91 => 'shhn_bl91',
    40 => 'shhn_pts',
    62 => 'color',
    92 => 'shhn_b92',
    347 => 'shhn_bl347',
    ],
  ARC_DIMENSION => [
    210 => 'extrusion',
    2 => 'block_name',
    10 => 'def_pt',
    11 => 'text_midpt',
    31 => 'elevation',
    70 => 'flag',
    1 => 'user_text',
    53 => 'text_rotation',
    51 => 'horiz_dir',
    40 => 'leader_len',
    41 => 'ins_scale',
    54 => 'ins_rotation',
    71 => 'attachment',
    72 => 'lspace_style',
    41 => 'lspace_factor',
    42 => 'act_measurement',
    73 => 'unknown',
    74 => 'flip_arrow1',
    75 => 'flip_arrow2',
    12 => 'clone_ins_pt',

    10 => 'def_pt',
    13 => '_13_pt',
    14 => '_14_pt',
    15 => '_15_pt',
    16 => '_16_pt',
    17 => '_17_pt',
    40 => 'leader_len',
    41 => 'ins_scale.x',
    70 => 'flag2',
    ],
  DATALINK => [
    70 => 'class_version',
    1 => 'appname',
    300 => 'description',
    301 => 'link',
    302 => 'cell',
    #90 => '',
    #91 => '',
    #92 => '',
    170 => 'year',
    171 => 'month',
    172 => 'day',
    173 => 'hour',
    174 => 'minute',
    175 => 'seconds',
    #176 => '',
    #177 => '',
    #93 => '',
    #304 => '',
    94 => 'num_readdeps',
    330 => 'readdeps',
    360 => 'writedep',
    #305 => '', # CUSTOMDATA
    ],
  DIMASSOC => [
    330 => 'dimensionobj',
    90 => 'associativity',
    70 => 'trans_space_flag',
    71 => 'rotated_type',
    @DIMASSOC_Ref,
    @DIMASSOC_Ref,
    @DIMASSOC_Ref,
    @DIMASSOC_Ref,
    92 => 'intsect_gsmarker',
    301 => 'xrefobj',
    302 => 'intsectxrefobj',
    302 => 'intsectxrefobj',
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
  NAVISWORKSMODELDEF => [
    #100 => AcDbNavisworksModelDef
    1 => 'path',
    290 => 'status',
    10 => 'min_extent',
    11 => 'max_extent',
    290 => 'host_drawing_visibility',
    ],
  RENDERENVIRONMENT => [
    # 100 => AcDbRenderEnvironment
    90 => 'class_version',
    290 => 'fog_enabled',
    290 => 'fog_background_enabled',
    280 => 'fog_color',
    40 => 'fog_density_near',
    40 => 'fog_density_far',
    40 => 'fog_distance_near',
    40 => 'fog_distance_far',
    290 => 'environ_image_enabled',
    1 => 'environ_image_filename',
    ],
  RAPIDRTRENDERSETTINGS => [
    # 100 => AcDbRenderSettings
    90 => 'class_version',
    1 => 'name',
    290 => 'fog_enabled',
    290 => 'fog_background_enabled',
    290 => 'b290_0',
    290 => 'environ_image_enabled',
    1 => 'environ_image_filename',
    1 => 'description',
    90 => 'bs90',
    # 100 => AcDbRapidRTRenderSettings
    90 => 'bs90_0',
    1 => 'rc70',
    90 => 'bs90_1',
    90 => 'bs90_2',
    70 => 'rc70_2',
    70 => 'rc70_3',
    40 => 'fog_density_near',
    40 => 'fog_density_far',
    290 => 'b290_2',
    ],
  RENDERGLOBAL => [
    # 100 => AcDbRenderGlobal
    90 => 'class_version',
    90 => 'procedure',
    90 => 'destination',
    290 => 'save_enabled',
    1 => 'save_filename',
    90 => 'image_width',
    90 => 'image_height',
    290 => 'predef_presets_first',
    290 => 'highlevel_info',
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
    330 => 'ownerhandle',
    63 => 'color',
    ],
  SUNSTUDY => [
    90 => 'class_version',
    1 => 'setup_name',
    2 => 'description',
    70 => 'output_type',
    290 => 'use_subset',
    3 => 'sheet_set_name',
    4 => 'sheet_subset_name',
    291 => 'select_dates_from_calendar',
    91 => 'num_dates',
    90 => 'dates.julian_day',
    90 => 'dates.time',
    292 => 'select_range_of_dates',
    93 => 'start_time',
    94 => 'end_time',
    95 => 'interval',
    91 => 'num_hours',
    290 => 'hours',
    74 => 'shade_plot_type',
    75 => 'numvports',
    76 => 'numrows',
    77 => 'numcols',
    40 => 'spacing',
    293 => 'lock_viewports',
    294 => 'label_viewports',
    330 => 'ownerhandle',
    340 => 'page_setup_wizard',
    341 => 'view',
    342 => 'visual_style',
    343 => 'text_style',
    ],
  ACAD_TABLE => [
    #SUBCLASS (AcDbBlockReference)
    10 => 'insertion_point',
    41 => 'scale.x',
    #0 => 'data_flags',
    42 => 'scale.y',
    43 => 'scale.z',
    50 => 'rotation',
    210 => 'extrusion',
    66 => 'has_attribs',
    #0 => 'num_owned',
    90 => 'flag_for_table_value',
    11 => 'horiz_direction',
    92 => 'num_cols',
    91 => 'num_rows',
    142 => 'col_widths',
    141 => 'row_heights',
    #SUBCLASS (AcDbDataCell)
    171 => 'cells.type',
    172 => 'cells.flags',
    173 => 'cells.merged_value',
    174 => 'cells.autofit_flag',
    175 => 'cells.merged_width_flag',
    176 => 'cells.merged_height_flag',
    145 => 'cells.rotation_value',
    1 => 'cells.text_string',
    144 => 'cells.block_scale',
    #0 => 'cells.additional_data_flag',
    179 => 'cells.num_attr_defs',
    #0 => 'cells.attr_def_index',
    300 => 'cells.attr_def_text',
    #0 => 'cells.additional_data_flag',
    177 => 'cells.cell_flag_override',
    178 => 'cells.virtual_edge_flag',
    170 => 'cells.cell_alignment',
    283 => 'cells.background_fill_none',
    63 => 'cells.background_color',
    64 => 'cells.content_color',
    7 => 'cells.text_style',
    140 => 'cells.text_height',
    69 => 'cells.top_grid_color',
    279 => 'cells.top_grid_linewt',
    289 => 'cells.top_visibility',
    65 => 'cells.right_grid_color',
    275 => 'cells.right_grid_linewt',
    285 => 'cells.right_visibility',
    66 => 'cells.bottom_grid_color',
    276 => 'cells.bottom_grid_linewt',
    286 => 'cells.bottom_visibility',
    68 => 'cells.left_grid_color',
    278 => 'cells.left_grid_linewt',
    288 => 'cells.left_visibility',
    #0 => 'cells.unknown',
    #0 => 'has_table_overrides',
    93 => 'table_flag_override',
    280 => 'title_suppressed',
    281 => 'header_suppressed',
    70 => 'flow_direction',
    40 => 'horiz_cell_margin',
    41 => 'vert_cell_margin',
    64 => 'title_row_color',
    64 => 'header_row_color',
    64 => 'data_row_color',
    283 => 'title_row_fill_none',
    283 => 'header_row_fill_none',
    283 => 'data_row_fill_none',
    63 => 'title_row_fill_color',
    63 => 'header_row_fill_color',
    63 => 'data_row_fill_color',
    170 => 'title_row_alignment',
    170 => 'header_row_alignment',
    170 => 'data_row_alignment',
    7 => 'title_text_style',
    7 => 'header_text_style',
    7 => 'data_text_style',
    140 => 'title_row_height',
    140 => 'header_row_height',
    140 => 'data_row_height',
    #0 => 'has_border_color_overrides',
    94 => 'border_color_overrides_flag',
    64 => 'title_horiz_top_color',
    65 => 'title_horiz_ins_color',
    66 => 'title_horiz_bottom_color',
    63 => 'title_vert_left_color',
    68 => 'title_vert_ins_color',
    69 => 'title_vert_right_color',
    64 => 'header_horiz_top_color',
    65 => 'header_horiz_ins_color',
    66 => 'header_horiz_bottom_color',
    63 => 'header_vert_left_color',
    68 => 'header_vert_ins_color',
    69 => 'header_vert_right_color',
    64 => 'data_horiz_top_color',
    65 => 'data_horiz_ins_color',
    66 => 'data_horiz_bottom_color',
    63 => 'data_vert_left_color',
    68 => 'data_vert_ins_color',
    69 => 'data_vert_right_color',
    #0 => 'has_border_lineweight_overrides',
    95 => 'border_lineweight_overrides_flag',
    #0 => 'title_horiz_top_linewt',
    #0 => 'title_horiz_ins_linewt',
    #0 => 'title_horiz_bottom_linewt',
    #0 => 'title_vert_left_linewt',
    #0 => 'title_vert_ins_linewt',
    #0 => 'title_vert_right_linewt',
    #0 => 'header_horiz_top_linewt',
    #0 => 'header_horiz_ins_linewt',
    #0 => 'header_horiz_bottom_linewt',
    #0 => 'header_vert_left_linewt',
    #0 => 'header_vert_ins_linewt',
    #0 => 'header_vert_right_linewt',
    #0 => 'data_horiz_top_linewt',
    #0 => 'data_horiz_ins_linewt',
    #0 => 'data_horiz_bottom_linewt',
    #0 => 'data_vert_left_linewt',
    #0 => 'data_vert_ins_linewt',
    #0 => 'data_vert_right_linewt',
    #0 => 'has_border_visibility_overrides',
    96 => 'border_visibility_overrides_flag',
    0 => 'title_horiz_top_visibility',
    0 => 'title_horiz_ins_visibility',
    0 => 'title_horiz_bottom_visibility',
    0 => 'title_vert_left_visibility',
    0 => 'title_vert_ins_visibility',
    0 => 'title_vert_right_visibility',
    0 => 'header_horiz_top_visibility',
    0 => 'header_horiz_ins_visibility',
    0 => 'header_horiz_bottom_visibility',
    0 => 'header_vert_left_visibility',
    0 => 'header_vert_ins_visibility',
    0 => 'header_vert_right_visibility',
    0 => 'data_horiz_top_visibility',
    0 => 'data_horiz_ins_visibility',
    0 => 'data_horiz_bottom_visibility',
    0 => 'data_vert_left_visibility',
    0 => 'data_vert_ins_visibility',
    0 => 'data_vert_right_visibility',
    2 => 'block_header',
    #0 => 'first_attrib',
    #0 => 'last_attrib',
    #0 => 'seqend',
    342 => 'table_style_id',
    344 => 'cells.cell_handle',
    340 => 'cells.cell_handle',
    331 => 'cells.attr_def_id',
    7 => 'cells.text_style_override',
    7 => 'title_row_style_override',
    7 => 'header_row_style_override',
    7 => 'data_row_style_override',
    0 => 'unknown_bs',
    11 => 'hor_dir',
    #0 => 'has_break_data',
    #0 => 'break_flag',
    #0 => 'break_flow_direction',
    #0 => 'break_spacing',
    #0 => 'break_unknown1',
    #0 => 'break_unknown2',
    #0 => 'num_break_heights',
    #0 => 'break_heights.position',
    #0 => 'break_heights.height',
    #0 => 'break_heights.flag',
    #0 => 'num_break_rows',
    #0 => 'break_rows.position',
    #0 => 'break_rows.start',
    ],
  TABLECONTENT => [
    #100 => AcDbDataTableContent
    1 => 'ldata.name',
    300 => 'ldata.desc',
    90 => 'tdata.num_cols',
    300 => 'tdata.cols.name',
    91 => 'tdata.cols.custom_data',
    #Cell_Style_Fields(tdata.cols.cell_style);
    90 => 'tdata.num_rows',
    90 => 'row.num_cells',
    90 => 'cell.flag',
    300 => 'cell.tooltip',
    91 => 'cell.customdata',
    90 => 'cell.num_customdata_items',
    300 => 'cell.customdata_items.name',
    92 => 'cell.has_linked_data',
    340 => 'cell.data_link',
    93 => 'cell.num_rows',
    94 => 'cell.num_cols',
    96 => 'cell.unknown',
    95 => 'cell.num_cell_contents',
    90 => 'content.type',
    # Table_Value(content.value)
    340 => 'content.handle',
    91 => 'content.num_attrs',
    330 => 'attr.attdef',
    301 => 'attr.value',
    92 => 'attr.index',
    # Content_Format(content.content_format);
    90 => 'cell.style_id',
    91 => 'cell.has_geom_data',
    91 => 'cell.geom_data_flag',
    40 => 'cell.unknown_d40',
    41 => 'cell.unknown_d41',
    #0 => 'cell.has_cell_geom',
    #0 => 'cell.cell_geom_handle',
    #0 => 'geom.dist_top_left',
    #0 => 'geom.dist_center',
    #0 => 'geom.content_width',
    #0 => 'geom.width',
    #0 => 'geom.height',
    #0 => 'geom.unknown',
    91 => 'row.custom_data',
    90 => 'row.num_customdata_items',
    300 => 'row.customdata_items.name',
    #Table_Value(row.customdata_items[rcount3].value);
    #Cell_Style_Fields(row.cell_style);
    90 => 'row.style_id',
    40 => 'row.height',
    #0 => 'tdata.num_field_refs',
    #0 => 'tdata.field_refs',
    90 => 'fdata.num_merged_cells',
    91 => 'merged.top_row',
    92 => 'merged.left_col',
    93 => 'merged.bottom_row',
    94 => 'merged.right_col',
    340 => 'table_style, 3',
    ],
  TABLEGEOMETRY => [
    90 => 'num_rows',
    91 => 'num_cols',
    92 => 'num_cells',
    93 => 'cell.flag',
    40 => 'cell.width_w_gap',
    41 => 'cell.height_w_gap',
    330 => 'cell.unknown',
    94 => 'cell.num_geom_data',
    #0 => 'geom.dist_top_left',
    #0 => 'geom.dist_center',
    #0 => 'geom.content_width',
    #0 => 'geom.width',
    #0 => 'geom.height',
    #0 => 'geom.unknown',
    ],
  TABLESTYLE => [
    100 => 'AcDbTableStyle',
    3 => 'name',
    70 => 'flow_direction',
    71 => 'flags',
    40 => 'horiz_cell_margin',
    41 => 'vert_cell_margin',
    280 => 'title_suppressed',
    281 => 'header_suppressed',
    7 => 'rowstyles.text_style',
    140 => 'rowstyles.text_height',
    170 => 'rowstyles.text_alignment',
    62 => 'rowstyles.text_color',
    63 => 'rowstyles.fill_color',
    283 => 'rowstyles.has_bgcolor',
    274 => 'rowstyles.borders.linewt',
    284 => 'rowstyles.borders.visible',
    264 => 'rowstyles.borders.color',
    275 => 'rowstyles.borders.linewt',
    285 => 'rowstyles.borders.visible',
    265 => 'rowstyles.borders.color',
    276 => 'rowstyles.borders.linewt',
    286 => 'rowstyles.borders.visible',
    266 => 'rowstyles.borders.color',
    277 => 'rowstyles.borders.linewt',
    287 => 'rowstyles.borders.visible',
    267 => 'rowstyles.borders.color',
    278 => 'rowstyles.borders.linewt',
    288 => 'rowstyles.borders.visible',
    268 => 'rowstyles.borders.color',
    279 => 'rowstyles.borders.linewt',
    289 => 'rowstyles.borders.visible',
    269 => 'rowstyles.borders.color',
    90 => 'rowstyles.data_type',
    91 => 'rowstyles.unit_type',
    1 => 'rowstyles.format_string',
    ],
  ACDB_ALDIMOBJECTCONTEXTDATA_CLASS => [
    100 => 'AcDbObjectContextData',
    70 => 'class_version',
    290 => 'has_file',
    90 => 'defaultflag',
    100 => 'AcDbAnnotScaleObjectContextData',
    340 => 'scale, 2',
    100 => 'AcDbDimensionObjectContextData',
    2 => 'name',
    10 => 'def_pt',
    293 => 'b293',
    294 => 'b294',
    140 => 'bd140',
    298 => 'b298',
    291 => 'b291',
    70 => 'flag',
    292 => 'b292',
    71 => 'bs71',
    280 => 'b280',
    295 => 'b295',
    296 => 'b296',
    297 => 'b297',
    100 => 'AcDbAlignedDimensionObjectContextData',
    11 => '_11pt',
    ],
  ACDB_BLKREFOBJECTCONTEXTDATA_CLASS => [
    100 => 'AcDbObjectContextData',
    70 => 'class_version',
    290 => 'has_file',
    0 => 'defaultflag',
    100 => 'AcDbAnnotScaleObjectContextData',
    340 => 'scale, 2',
    ],
  ACDB_LEADEROBJECTCONTEXTDATA_CLASS => [
    100 => 'AcDbObjectContextData',
    70 => 'class_version',
    290 => 'has_file',
    90 => 'defaultflag',
    100 => 'AcDbAnnotScaleObjectContextData',
    340 => 'scale',
    70 => 'num_points',
    10 => 'points[0]',
    10 => 'points[1]',
    10 => 'points[2]',
    11 => 'x_direction',
    290 => 'b290', # arrowhead_on?
    12 => 'inspt_offset',
    13 => 'endptproj',
    ],
  ACDB_MLEADEROBJECTCONTEXTDATA_CLASS => [
    100 => 'AcDbObjectContextData',
    70 => 'class_version',
    290 => 'has_file',
    0 => 'defaultflag',
    100 => 'AcDbAnnotScaleObjectContextData',
    340 => 'scale, 2',
    ],
  ACDB_TEXTOBJECTCONTEXTDATA_CLASS => [
    100 => 'AcDbObjectContextData',
    70 => 'class_version',
    290 => 'has_file',
    90 => 'defaultflag',
    100 => 'AcDbAnnotScaleObjectContextData',
    340 => 'scale',
    70 => 'flag',
    50 => 'rotation',
    10 => 'insertion_pt',
    11 => 'alignment_pt',
    ],
  ACDB_MTEXTATTRIBUTEOBJECTCONTEXTDATA_CLASS => [
    100 => 'AcDbObjectContextData',
    70 => 'class_version',
    290 => 'has_file',
    90 => 'defaultflag',
    100 => 'AcDbAnnotScaleObjectContextData',
    340 => 'scale',
  ],
  ACDB_MTEXTOBJECTCONTEXTDATA_CLASS => [
    100 => 'AcDbObjectContextData',
    70 => 'class_version',
    290 => 'has_file',
    90 => 'defaultflag',
    100 => 'AcDbAnnotScaleObjectContextData',
    340 => 'scale',
    70 => 'flag',
    11 => 'insertion_pt',
    10 => 'x_axis_dir',
    40 => 'text_height',
    41 => 'rect_width',
    42 => 'extents_width',
    43 => 'extents_height',
    71 => 'attachment',
    72 => 'drawing_dir',
    73 => 'linespace_style',
    44 => 'linespace_factor',
    45 => 'bd45',
    74 => 'bs74',
    46 => 'rect_height',
    ],
  MULTILEADER => [
    270 => 'class_version',
    70 => 'ctx.class_version',
    #0 => 'ctx.has_xdic_file',
    290 => 'ctx.is_default',
    340 => 'ctx.scale_handle',
    302 => 'LEADER{',
    #0 => 'ctx.num_leaders',
    290 => 'lev1.is_valid',
    291 => 'lev1.unknown',
    10 => 'lev1.connection',
    11 => 'lev1.direction',
    #0 => 'lev1.num_breaks',
    12 => 'lev1.breaks.start',
    13 => 'lev1.breaks.end',
    90 => 'lev1.index',
    40 => 'lev1.landing_distance',
    304 => 'LEADER_LINE{',
    #0 => 'lev2.num_points',
    10 => 'lev2.points',
    #0 => 'lev2.num_breaks',
    90 => 'lev2.segment_index',
    11 => 'lev2.breaks.start',
    12 => 'lev2.breaks.end',
    91 => 'lev2.index',
    170 => 'lev2.type',
    92 => 'lev2.color',
    340 => 'lev2.type_handle',
    171 => 'lev2.weight',
    40 => 'lev2.arrow_size',
    341 => 'lev2.arrow_handle',
    93 => 'lev2.flags',
    305 => '}',
    271 => 'lev1.attach_dir',
    303 => '}',
    40 => 'ctx.scale',
    10 => 'ctx.content_base',
    41 => 'ctx.text_height',
    140 => 'ctx.arrow_size',
    145 => 'ctx.landing_gap',
    174 => 'ctx.text_left',
    175 => 'ctx.text_right',
    176 => 'ctx.text_alignment',
    177 => 'ctx.attach_type',
    290 => 'ctx.has_content_txt',
    304 => 'ctx.txt.label',
    11 => 'ctx.txt.normal',
    340 => 'ctx.txt.style',
    12 => 'ctx.txt.location',
    13 => 'ctx.txt.direction',
    42 => 'ctx.txt.rotation',
    43 => 'ctx.txt.width',
    44 => 'ctx.txt.height',
    45 => 'ctx.txt.line_spacing_factor',
    170 => 'ctx.txt.line_spacing_style',
    90 => 'ctx.txt.color',
    171 => 'ctx.txt.alignment',
    172 => 'ctx.txt.flow',
    91 => 'ctx.txt.bg_color',
    141 => 'ctx.txt.bg_scale',
    92 => 'ctx.txt.bg_transparency',
    291 => 'ctx.txt.is_bg_fill',
    292 => 'ctx.txt.is_bg_mask_fill',
    173 => 'ctx.txt.col_type',
    293 => 'ctx.txt.is_height_auto',
    142 => 'ctx.txt.col_width',
    143 => 'ctx.txt.col_gutter',
    294 => 'ctx.txt.is_col_flow_reversed',
    #0 => 'ctx.txt.num_col_sizes',
    144 => 'ctx.txt.col_sizes',
    295 => 'ctx.txt.word_break',
    #0 => 'ctx.txt.unknown',
    296 => 'ctx.has_content_blk',
    341 => 'ctx.blk.block_table',
    14 => 'ctx.blk.normal',
    15 => 'ctx.blk.location',
    16 => 'ctx.blk.scale',
    46 => 'ctx.blk.rotation',
    93 => 'ctx.blk.color',
    47 => 'ctx.blk.transform',
    110 => 'ctx.base',
    111 => 'ctx.base_dir',
    112 => 'ctx.base_vert',
    297 => 'ctx.is_normal_reversed',
    273 => 'ctx.text_top',
    272 => 'ctx.text_bottom',
    301 => '}',
    340 => 'mleaderstyle',
    90 => 'flags',
    170 => 'type',
    91 => 'color',
    341 => 'ltype',
    171 => 'linewt',
    290 => 'landing',
    291 => 'dog_leg',
    41 => 'landing_dist',
    342 => 'arrow_head',
    42 => 'arrow_head_size',
    172 => 'style_content',
    343 => 'text_style',
    95 => 'text_left',
    95 => 'text_right',
    174 => 'text_angletype',
    175 => 'attach_type',
    92 => 'text_color',
    292 => 'text_frame',
    344 => 'block_style',
    93 => 'block_color',
    10 => 'block_scale',
    43 => 'block_rotation',
    176 => 'style_attachment',
    293 => 'is_annotative',
    #0 => 'num_arrowheads',
    94 => 'arrowheads.is_default',
    345 => 'arrowheads.arrowhead',
    #0 => 'num_blocklabels',
    330 => 'blocklabels.attdef',
    302 => 'blocklabels.label_text',
    177 => 'blocklabels.ui_index',
    44 => 'blocklabels.width',
    294 => 'neg_textdir',
    178 => 'ipe_alignment',
    179 => 'justification',
    45 => 'scale_factor',
    271 => 'attach_dir',
    273 => 'attach_top',
    272 => 'attach_bottom',
    295 => 'text_extended',
    ],
  GEOPOSITIONMARKER => [ # or POSITIONMARKER
    #0 => 'type',
    10 => 'position',
    #0 => 'lat_lon_alt',
    210 => 'extrusion',
    40 => 'radius',
    #0 => 'landing_gap',
    1 => 'text',
    #0 => 'text_alignment',
    #0 => 'mtext_visible',
    #0 => 'mtext_is_vertical',
    #0 => 'mtext_actual_width',
    #0 => 'mtext_actual_height',
    #0 => 'enable_frame_text',
    #0 => 'notes',
    ],
  EXTRUDEDSURFACE => [
    #100 => AcDbModelerGeometry,
    70 => 'modeler_format_version',
    #100 => AcDbSurface,
    71 => 'u_isolines',
    72 => 'v_isolines',
    #100 => AcDbExtrudedSurface
    90 => 'class_version',
    10 => 'sweep_vector',
    40 => 'sweep_transmatrix', #x16
    42 => 'draft_angle',
    43 => 'draft_start_distance',
    44 => 'draft_end_distance',
    45 => 'twist_angle',
    48 => 'scale_factor',
    49 => 'align_angle',
    46 => 'sweep_entity_transmatrix',
    47 => 'path_entity_transmatrix',
    290 => 'solid',
    290 => 'sweep_alignment_flags',
    292 => 'align_start',
    293 => 'bank',
    294 => 'base_point_set',
    295 => 'sweep_entity_transform_computed',
    296 => 'path_entity_transform_computed',
    11 => 'reference_vector_for_controlling_twist',
    ],
  LOFTEDSURFACE => [
    #100 => AcDbModelerGeometry,
    70 => 'modeler_format_version',
    #100 => AcDbSurface,
    71 => 'u_isolines',
    72 => 'v_isolines',
    #100 => AcDbLoftedSurface
    40 => 'loft_entity_transmatrix', #x16
    #90 => 'class_version',
    70 => 'plane_normal_lofting_type',
    41 => 'start_draft_angle',
    42 => 'end_draft_angle',
    43 => 'start_draft_magnitude',
    44 => 'end_draft_magnitude',
    290 => 'arc_length_parameterization',
    291 => 'no_twist',
    292 => 'align_direction',
    293 => 'simple_surfaces',
    294 => 'closed_surfaces',
    295 => 'solid',
    296 => 'ruled_surface',
    297 => 'virtual_guide',
    90  => 'num_cross_sections',
    310 => 'cross_sections',
    90  => 'num_guide_curves',
    310 => 'guide_curves',
    5   => 'path_curve',
    ],
  REVOLVEDSURFACE => [
    #100 => AcDbModelerGeometry,
    70 => 'modeler_format_version',
    #100 => AcDbSurface,
    71 => 'u_isolines',
    72 => 'v_isolines',
    #100 => AcDbRevolvedSurface
    90 => 'class_version',
    90 => 'id',
    90 => 'num_bindata',
    310 => 'bindata',
    10 => 'axis_point',
    11 => 'axis_vector',
    40 => 'revolve_angle',
    41 => 'start_angle',
    42 => 'revolved_entity_transmatrix',
    43 => 'draft_angle',
    44 => 'draft_start_distance',
    45 => 'draft_end_distance',
    46 => 'twist_angle',
    290 => 'solid',
    291 => 'close_to_axis',
    ],
  SWEPTSURFACE => [
    #100 => AcDbModelerGeometry,
    70 => 'modeler_format_version',
    #100 => AcDbSurface,
    71 => 'u_isolines',
    72 => 'v_isolines',
    #100 => AcDbSweptSurface
    90 => 'class_version',
    90 => 'swept_entity_id',
    90 => 'num_sweepdata',
    310 => 'sweepdata',
    90 => 'path_entity_id',
    90 => 'num_pathdata',
    310 => 'pathdata',
    40 => 'sweep_entity_transmatrix',
    41 => 'path_entity_transmatrix',
    42 => 'draft_angle',
    43 => 'draft_start_distance',
    44 => 'draft_end_distance',
    45 => 'twist_angle',
    48 => 'scale_factor',
    290 => 'solid',
    70 => 'sweep_alignment',
    292 => 'align_start',
    293 => 'bank',
    294 => 'base_point_set',
    295 => 'sweep_entity_transform_computed',
    296 => 'path_entity_transform_computed',
    11 => 'reference_vector_for_controlling_twist',
    ],
  SURFACE => [
    #100 => AcDbModelerGeometry,
    70 => 'modeler_format_version',
    #100 => AcDbSurface,
    71 => 'u_isolines',
    72 => 'v_isolines',
    #100 => AcDbPlaneSurface
    90 => 'class_version',
    ],
  MESH => [
    90 => 'class_version',
    # 100 => AcDbSubDMesh
    71 => 'dlevel',
    72 => 'is_watertight',
    91 => 'num_subdiv_vertex',
    10 => 'subdiv_vertex',
    92 => 'num_vertex',
    10 => 'vertex',
    93 => 'num_faces',
    90 => 'faces',
    94 => 'num_edges',
    90 => 'edges.from',
    90 => 'edges.to',
    95 => 'num_crease',
    # 90?  BD crease
    ],
  GEOMAPIMAGE => [
    # 100 => AcDbGeomapImage
    90 => 'class_version',
    10 => 'pt0',
    13 => 'size',
    70 => 'display_props',
    280 => 'clipping',
    281 => 'brightness',
    282 => 'contrast',
    283 => 'fade',
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
  UNDERLAYDEFINITION => [
    1 => 'filename',
    2 => 'name',
    ],
  VISUALSTYLE => [
    2 => 'description',
    70 => 'style_type',
    177 => 'unknown_lighting_model',
    291 => 'has_ext',
    71 => 'face_lighting_model',
    72 => 'face_lighting_quality',
    73 => 'face_color_mode',
    40 => 'face_opacity',
    41 => 'face_specular',
    90 => 'face_modifier',
    62 => 'color',
    420 => 'color.rgb',
    63 => 'face_mono_color',
    421 => 'face_mono_color.rgb',
    74 => 'edge_model',
    91 => 'edge_style',
    64 => 'edge_intersection_color',
    65 => 'edge_obscured_color',
    75  => 'edge_obscured_line_pattern',
    175 => 'edge_intersection_line_pattern',
    42 => 'edge_crease_angle',
    92 => 'edge_modifier',
    66 => 'edge_color',
    43 => 'edge_opacity',
    76 => 'edge_width',
    77 => 'edge_overhang',
    78 => 'edge_jitter',
    67 => 'edge_silhouette_color',
    79 => 'edge_silhouette_width',
    170 => 'edge_halo_gap',
    171 => 'num_edge_isolines',
    290 => 'edge_hide_precision_flag',
    174 => 'edge_style_apply_flag',
    93 => 'display_style',
    44 => 'display_brightness',
    173 => 'display_shadow_type',
    45 => 'unknown_float45',
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
  ACDBDETAILVIEWSTYLE => [
    100 => 'AcDbModelDocViewStyle',
    70 => 'class_version',
    3 => 'name',
    290 => 'is_modified_for_recompute',
    100 => 'AcDbDetailViewStyle',
    70 => 'identifier_placement',
    90 => 'model_edge',
    280 => 'show_arrowheads',
    71 => 'connection_line_weight',
    62 => 'connection_line_color',
    62 => 'identifier_color',
    62 => 'arrow_symbol_color',
    62 => 'boundary_line_color',
    62 => 'viewlabel_text_color',
    40 => 'identifier_height',
    42 => 'identifier_offset',
    340 => 'identifier_style',
    340 => 'arrow_symbol',
    40 => 'arrow_symbol_size',
    71 => 'boundary_line_weight',
    340 => 'boundary_line_type',
    340 => 'viewlabel_text_style',
    340 => 'connection_line_type',
    40 => 'viewlabel_text_height',
    300 => 'viewlabel_field',
    42 => 'viewlabel_offset',
    70 => 'viewlabel_attachment',
    72 => 'viewlabel_alignment',
    90 => 'viewlabel_pattern',
    280 => 'show_viewlabel',
    71 => 'borderline_weight',
    62 => 'borderline_color',
    340 => 'borderline_type',
    ],
  ACDBSECTIONVIEWSTYLE => [
    100 => 'AcDbModelDocViewStyle',
    70 => 'class_version',
    3 => 'name',
    290 => 'is_modified_for_recompute',
    300 => 'description',
    90 => 'identifier_exclude_characters',
    100 => 'AcDbSectionViewStyle',
    62 => 'identifier_color',
    40 => 'identifier_height',
    62 => 'plane_line_color',
    42 => 'identifier_offset',
    300 => 'viewlabel_text',
    40 => 'identifier_position',
    90 => 'viewlabel_pattern',
    62 => 'arrow_symbol_color',
    90 => 'hatch_pattern',
    62 => 'bend_line_color',
    40 => 'arrow_symbol_size',
    40 => 'arrow_position',
    62 => 'viewlabel_text_color',
    40 => 'bend_line_length',
    62 => 'hatch_color',
    62 => 'hatch_bg_color',
    340 => 'identifier_style',
    340 => 'arrow_start_symbol',
    340 => 'arrow_end_symbol',
    40 => 'arrow_symbol_extension_length',
    71 => 'plane_line_weight',
    340 => 'plane_line_type',
    340 => 'bend_line_type',
    340 => 'viewlabel_text_style',
    0 => 'bend_line_weight',
    0 => 'end_line_length',
    40 => 'viewlabel_text_height',
    300 => 'viewlabel_field',
    42 => 'viewlabel_offset',
    70 => 'viewlabel_attachment',
    72 => 'viewlabel_alignment',
    0 => 'hatch_scale',
    0 => 'hatch_angles',
    0 => 'hatch_transparency',
    290 => 'is_continuous_labeling',
    290 => 'show_arrowheads',
    290 => 'show_viewlabel',
    290 => 'show_all_plane_lines',
    290 => 'show_all_bend_indentifiers',
    290 => 'show_end_and_bend_lines',
    ],
  LAYERFILTER => [
    2 => 'name',
    3 => 'description',
    ],
  LAYOUTPRINTCONFIG => [
    93 => 'flag',
    ],
  ACMECOMMANDHISTORY => [
    ],
  ACMESCOPE => [
    ],
  ACMESTATEMGR => [
    ],
};

my $firstline = "/* ex: set ro ft=c: -*- mode: c; buffer-read-only: t -*- */\n"
              . "/* generated by make regen-unknown, do not modify */\n\n";
print $f0 $firstline;
print $f0 "// name, dxf, handle, bytes, is_entity, num_bits, commonsize, hdloff, ".
          "strsize, hdlsize, bitsize, fieldptr\n";
print $f1a $firstline;
print $f2 $firstline;
my ($f, $f1, $prevdxf, $prevobj);

LINE:
while (<>) {
  my @F = split(' ');

  my $dxf = $F[4];
  if ($dxf eq 'NULL,' or $dxf !~ /\.dxf",/) {
    next LINE; # -n
  }
  $dxf = substr($dxf, 1, -2);
  my $fdxf = $dxf;
  if (!-f $fdxf) {
    if (!-f "../$fdxf") {
      $fdxf = "$dir/../$dxf";
      if (!-f "$fdxf") {
        warn "$fdxf not found";
      }
    } else {
      $fdxf = "../$fdxf";
    }
  }
  #next LINE if $F[0] =~ m|//{|; # skip duplicates
  my $obj   = substr($F[1],1,-2); # "MATERIAL",
  my $bytes = substr($F[3],1,-2);
  my $hdl   = substr($F[5],2,-1); # 0xXXX,
  my $is_entity  = substr($F[6],0,-1);
  my $num_bits   = substr($F[7],0,-1);
  my $commonsize = substr($F[8],0,-1);
  my $hdloff     = substr($F[9],0,-1);
  my $strsize    = substr($F[10],0,-1);
  my $hdlsize    = substr($F[11],0,-1);
  my $bitsize    = substr($F[12],0,-1);
  if ($skip{"$obj-$hdl-$num_bits"}) { # skip empty unknowns
    warn "skip empty $obj-$hdl-$num_bits $dxf\n";
    next LINE;
  }
  # picat runs out of memory there
  if ($num_bits > 30000) {
    warn "skip overlarge $obj-$hdl-$num_bits $dxf\n";
    next LINE;
  }

  my $unknown = pack ("H*", $bytes);
  $unknown = join("", map { sprintf("\\%03o", $_) } unpack("C*", $unknown));
  # $unknown = substr($unknown, 0, $num_bits);

  if (exists $dupl{"$obj-$unknown"}) {
    warn "skip duplicate $obj-$hdl-$num_bits $dxf\n";
    next LINE;
  } else {
    $dupl{"$obj-$unknown"}++;
  }
  #warn "$dxf: $obj HANDLE($hdl)\n";
  # 9080187 5160203 9080187 201AA 51E0204 90C0202 35200204 20640A8 2D22020C 90A01D1
  #if ($hdl =~ /^([0-9A-F]){1,4}0([0-9A-F]+)$/) {
  #  $hdl = $2;
  #  #warn "=> try HANDLE($hdl)\n";
  #}

  # linear search in dxf file for this obj-handle

  if (!$f or $fdxf ne $prevdxf) {
    close $f if $f;
    open $f, "$fdxf" or next LINE;
    $prevdxf = $fdxf;
  }
  if ($obj ne $prevobj) {
    # also enforce a new dxffile, as the prev obj might have been be after the current one
    # and we cannot walk back.
    undef $f;
    $prevobj = $obj;
    print $f1a "#include \"alldxf_$obj.inc\"\n";
    close $f1 if $f1;
    # objects ARE sorted
    open $f1, ">", "$dir/alldxf_$obj.inc" or next LINE;
    print $f1 $firstline;
    print $f1 "// code, value, bits, pre_bits, num_bits, type, name, num, pos[]\n";
#  } else {
#    open $f1, ">>", "$dir/alldxf_$obj.inc" or next LINE;
  }

  my ($foundobj, $foundhdl, @FIELD, $in_entity);
  my ($react, $xdict, $seen100, @avail);
  if ($obj =~ /^(?:PDF|DWF|DGN)UNDERLAY/) {
    @avail = @{$known->{UNDERLAY}};
  }
  elsif ($obj =~ /^(?:PDF|DWF|DGN)DEFINITION/) {
    @avail = @{$known->{UNDERLAYDEFINITION}};
  }
  @avail = @{$known->{$obj}} if exists $known->{$obj};
  # cannot walk back
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
          my $count = $sorted{"$t:$_->[1]"};
          emit_field($f1, $obj, $count, @$_);
        }
        @FIELD = ();
        print $f1 "    { 0, NULL, NULL, 0, BITS_UNKNOWN, NULL, 0, {-1,-1,-1,-1,-1}}\n};\n";
      }
    }
    if ($foundobj and $code =~ /^ *5$/) {
      $name = "handle";
      $foundhdl = $v eq $hdl;
      if ($foundhdl) {
        warn "found $obj $hdl in $dxf\n";
        print $f0  "  { \"$obj\", \"$dxf\", 0x$hdl, /* $i */\n";
        printf $f0  "    \"%s\", %d, %d, %d, %d, %d, %d, %d, NULL },\n",
          $unknown, $is_entity, $num_bits, $commonsize, $hdloff, $strsize,
          $hdlsize, $bitsize;

        print $f1 "/* $obj $hdl in $dxf */\n";
        print $f1 "static struct _unknown_field unknown_dxf_${obj}_${i}\[\] = {\n";
        print $f2 "unknown_dxf\[$i\].fields = unknown_dxf_${obj}_${i};\n";
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
          $name = "ownerhandle";
        }
      } elsif ($code == 360 and $xdict) {
        $name = "xdicobjhandle";
      } elsif ($code == 100) {
        if ($v eq 'AcDbEntity') { $in_entity++ }
        else { $in_entity = 0 }
        $seen100++;
        if ($v eq 'AcDbModelerGeometry') {
          push @FIELD, [$code, $v, $name];
          push @FIELD, [1, "ACIS BinaryFile", "bin_sat_data"];
          next;
        }
      } elsif ($code == 102) {
        if ($v eq '{ACAD_REACTORS') {
          $react = 1;
        } elsif ($v eq '{ACAD_XDICTIONARY') {
          $xdict = 1;
        } elsif ($v eq '}') {
          $xdict = 0; $react = 0;
        }
      } elsif ($code == 430 && $obj eq 'DBCOLOR' &&
               $v =~ /DIC COLOR GUIDE\(R\)\$DIC (\d+)/)
      {
        push @FIELD, [430, "DIC $1", "name"];
        $v = "DIC COLOR GUIDE(R)";
        $name = "catalog";
      } elsif ($x = find_name($code, $obj, \@avail, \@FIELD)) {
        $name = $x;
      } elsif ($code == 8) {
        $name = "layer";
      } elsif ($code >= 60 && $code <= 65) {
        $name = "color";
      } elsif ($code == 7) {
        $name = "ltype";
      }
      if ($in_entity && is_common_entity($code, $v, $name)) {
        push @FIELD, [$code, $v, $name, 1];
      } elsif ($code >= 1000) { # no EED
        push @FIELD, [$code, $v, $name, 1];
      } elsif ($code == 5 && !$seen100) {
        push @FIELD, [$code, $v, $name, 1];
      } else {
        push @FIELD, [$code, $v, $name];
      }
    }
  }
  # stay in there. only close on new obj or changed dxf
  #close $f;
}
close $f; close $f0; close $f1; close $f1a; close $f2;

sub is_common_entity {
  my ($code, $v, $name) = @_;
  return 1 if $code == 8 or $code == 6 or $code == 7 or $code == 62 or
              $code == 92 or $code == 160 or
              $code == 310 or $code == 420 or $code == 440;
}

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
  for my $j (0..(2*$len)-1) {
    my $c = $f[$j];
    if ($c == $code) {
      # delete ($c,$n) from $avail
      splice @$avail, $j, 2;
      my $n = $f[$j+1];
      return $n;
    }
  }
  return undef;
}

sub emit_field {
  my ($f, $obj, $count, $code, $v, $name, $hidden) = @_;
  #warn "$code: $v\n";
  #return if $code == 100;
  $v =~ s/\\/\\\\/g;
  $v =~ s/"/\\"/g;
  $count = 0 unless $count;
  # code, value, bytes, bitsize, type, name, num, pos[]
  if ($hidden) {
    print $f "  //";
  } else {
    print $f "    ";
  }
  my $bits = 0;
  my $type = "BITS_UNKNOWN";
  # need stronger hints for some fields
  if ($obj eq 'DIMASSOC') {
    if ($name =~ /(osnap_type|trans_space_flag)$/) {
      $type = "BITS_RC";
      $bits = 8;
    }
    elsif ($name =~ /has_lastpt_ref/) { # weird dxf code
      $type = "BITS_B";
      $bits = 1;
    }
    elsif ($name =~ /\.osnap_/) {
      $type = "BITS_BD";
    }
  }
  print $f "{ $code, \"$v\", NULL, $bits, $type, \"$name\", $count, {-1,-1,-1,-1,-1} },\n";
}
