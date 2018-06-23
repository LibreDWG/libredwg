#!/usr/bin/perl -sn
=head1 USAGE

    log_unknown_dxf alldwg.inc >alldxf.inc
    make -C examples alldxf.inc

e.g.

    { "MATERIAL", "004014080405aa6a9aa404014080405aa6a9aa404000000000000e03f4080405aa6a9aa4054080405aa6a9aa4050201016a9aa6a90150201016a9aa6a900aa4fe41d0801e40130018401e4019401c802aaa1803824882e464030dfbfb8", "11", "Drawing_2007.log", "test/test-data/Drawing_2007.dxf", 0x96, 746 },

Search 0 MATERIAL + Handle 96 in test/test-data/Drawing_2007.dxf =>

      0
    MATERIAL
      5
    96
    102
    {ACAD_XDICTIONARY
    360
    186
    102
    }
    102
    {ACAD_REACTORS
    330
    72
    102
    }
    330
    72
    100
    AcDbMaterial
      1
    ByLayer
     94
           63

=>
    FIELD_T   1  "ByLayer"    => dwg.spec: FIELD_T (name, 1);
    FIELD_BL 94 63  (int32_t) => dwg.spec: FIELD_BL (channel_flags, 94);

    { "ACDBASSOCNETWORK", "406aa405014804203e140304203e0fa1", "100", "Drawing_2007.log", "test/test-data/Drawing_2007.dxf", 0x3E2, 131 },


      0
    ACDBASSOCNETWORK
      5
    3E2
    102
    {ACAD_REACTORS
    330
    3E1
    102
    }
    330
    3E1
    100
    AcDbAssocAction
     90
            1
     90
            0
    330
    0
    360
    0
     90
            0
     90
            0
     90
            0
    100
    AcDbAssocNetwork
     90
            0
     90
            1
     90
            1
    330
    3E0
     90
            0


=cut

