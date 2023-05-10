/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2020 Free Software Foundation, Inc.                        */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * classes.c: map type to name. classify into unstable, debugging and
 * unhandled. written by Reini Urban
 */

#include "config.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "common.h"
#include "dwg.h"
#include "classes.h"
#include "dynapi.h"

// Now with direct O[1] access by type.
static const char *const _dwg_type_names_fixed[] = {
  "UNUSED",             /* DWG_TYPE_UNUSED 0x00 */
  "TEXT",               /* DWG_TYPE_TEXT 0x01 */
  "ATTRIB",             /* DWG_TYPE_ATTRIB 0x02 */
  "ATTDEF",             /* DWG_TYPE_ATTDEF 0x03 */
  "BLOCK",              /* DWG_TYPE_BLOCK 0x04 */
  "ENDBLK",             /* DWG_TYPE_ENDBLK 0x05 */
  "SEQEND",             /* DWG_TYPE_SEQEND 0x06 */
  "INSERT",             /* DWG_TYPE_INSERT 0x07 */
  "MINSERT",            /* DWG_TYPE_MINSERT 0x08 */
  NULL,                 /* DWG_TYPE_<UNKNOWN> = 0x09, */
  "VERTEX_2D",          /* DWG_TYPE_VERTEX_2D 0x0a */
  "VERTEX_3D",          /* DWG_TYPE_VERTEX_3D 0x0b */
  "VERTEX_MESH",        /* DWG_TYPE_VERTEX_MESH 0x0c */
  "VERTEX_PFACE",       /* DWG_TYPE_VERTEX_PFACE 0x0d */
  "VERTEX_PFACE_FACE",  /* DWG_TYPE_VERTEX_PFACE_FACE 0x0e */
  "POLYLINE_2D",        /* DWG_TYPE_POLYLINE_2D 0x0f */
  "POLYLINE_3D",        /* DWG_TYPE_POLYLINE_3D 0x10 */
  "ARC",                /* DWG_TYPE_ARC 0x11 */
  "CIRCLE",             /* DWG_TYPE_CIRCLE 0x12 */
  "LINE",               /* DWG_TYPE_LINE 0x13 */
  "DIMENSION_ORDINATE", /* DWG_TYPE_DIMENSION_ORDINATE 0x14 */
  "DIMENSION_LINEAR",   /* DWG_TYPE_DIMENSION_LINEAR 0x15 */
  "DIMENSION_ALIGNED",  /* DWG_TYPE_DIMENSION_ALIGNED 0x16 */
  "DIMENSION_ANG3PT",   /* DWG_TYPE_DIMENSION_ANG3PT 0x17 */
  "DIMENSION_ANG2LN",   /* DWG_TYPE_DIMENSION_ANG2LN 0x18 */
  "DIMENSION_RADIUS",   /* DWG_TYPE_DIMENSION_RADIUS 0x19 */
  "DIMENSION_DIAMETER", /* DWG_TYPE_DIMENSION_DIAMETER 0x1A */
  "POINT",              /* DWG_TYPE_POINT 0x1b */
  "3DFACE",             /* DWG_TYPE__3DFACE 0x1c */
  "POLYLINE_PFACE",     /* DWG_TYPE_POLYLINE_PFACE 0x1d */
  "POLYLINE_MESH",      /* DWG_TYPE_POLYLINE_MESH 0x1e */
  "SOLID",              /* DWG_TYPE_SOLID 0x1f */
  "TRACE",              /* DWG_TYPE_TRACE 0x20 */
  "SHAPE",              /* DWG_TYPE_SHAPE 0x21 */
  "VIEWPORT",           /* DWG_TYPE_VIEWPORT 0x22 */
  "ELLIPSE",            /* DWG_TYPE_ELLIPSE 0x23 */
  "SPLINE",             /* DWG_TYPE_SPLINE 0x24 */
  "REGION",             /* DWG_TYPE_REGION 0x25 */
  "3DSOLID",            /* DWG_TYPE__3DSOLID 0x26 */
  "BODY",               /* DWG_TYPE_BODY 0x27 */
  "RAY",                /* DWG_TYPE_RAY 0x28 */
  "XLINE",              /* DWG_TYPE_XLINE 0x29 */
  "DICTIONARY",         /* DWG_TYPE_DICTIONARY 0x2a */
  "OLEFRAME",           /* DWG_TYPE_OLEFRAME = 0x2b */
  "MTEXT",              /* DWG_TYPE_MTEXT = 0x2c */
  "LEADER",             /* DWG_TYPE_LEADER = 0x2d */
  "TOLERANCE",          /* DWG_TYPE_TOLERANCE = 0x2e */
  "MLINE",              /* DWG_TYPE_MLINE 0x2f */
  "BLOCK_CONTROL",      /* DWG_TYPE_BLOCK_CONTROL 0x30 */
  "BLOCK_HEADER",       /* DWG_TYPE_BLOCK_HEADER 0x31 */
  "LAYER_CONTROL",      /* DWG_TYPE_LAYER_CONTROL 0x32 */
  "LAYER",              /* DWG_TYPE_LAYER 0x33 */
  "STYLE_CONTROL",      /* DWG_TYPE_STYLE_CONTROL 0x34 */
  "STYLE",              /* DWG_TYPE_STYLE 0x35 */
  NULL,                 /* DWG_TYPE_<UNKNOWN> = 0x36, */
  NULL,                 /* DWG_TYPE_<UNKNOWN> = 0x37, */
  "LTYPE_CONTROL",      /* DWG_TYPE_LTYPE_CONTROL 0x38 */
  "LTYPE",              /* DWG_TYPE_LTYPE 0x39 */
  NULL,                 /* DWG_TYPE_<UNKNOWN> = 0x3a, */
  NULL,                 /* DWG_TYPE_<UNKNOWN> = 0x3b, */
  "VIEW_CONTROL",       /* DWG_TYPE_VIEW_CONTROL 0x3c */
  "VIEW",               /* DWG_TYPE_VIEW 0x3d */
  "UCS_CONTROL",        /* DWG_TYPE_UCS_CONTROL 0x3e */
  "UCS",                /* DWG_TYPE_UCS 0x3f */
  "VPORT_CONTROL",      /* DWG_TYPE_VPORT_CONTROL 0x40 */
  "VPORT",              /* DWG_TYPE_VPORT 0x41 */
  "APPID_CONTROL",      /* DWG_TYPE_APPID_CONTROL 0x42 */
  "APPID",              /* DWG_TYPE_APPID 0x43 */
  "DIMSTYLE_CONTROL",   /* DWG_TYPE_DIMSTYLE_CONTROL 0x44 */
  "DIMSTYLE",           /* DWG_TYPE_DIMSTYLE 0x45 */
  "VX_CONTROL",         /* DWG_TYPE_VX_CONTROL 0x46 */
  "VX_TABLE_RECORD",    /* DWG_TYPE_VX_TABLE_RECORD 0x47 */
  "GROUP",              /* DWG_TYPE_GROUP 0x48 */
  "MLINESTYLE",         /* DWG_TYPE_MLINESTYLE 0x49 */
  "OLE2FRAME",          /* DWG_TYPE_OLE2FRAME 0x4a */
  "DUMMY",              /* DWG_TYPE_DUMMY 0x4b */
  "LONG_TRANSACTION",   /* DWG_TYPE_LONG_TRANSACTION 0x4c */
  "LWPOLYLINE",         /* DWG_TYPE_LWPOLYLINE 0x4d */
  "HATCH",              /* DWG_TYPE_HATCH 0x4e */
  "XRECORD",            /* DWG_TYPE_XRECORD 0x4f */
  "PLACEHOLDER",        /* DWG_TYPE_PLACEHOLDER 0x50 */
  "VBA_PROJECT",        /* DWG_TYPE_VBA_PROJECT 0x51 */
  "LAYOUT",             /* DWG_TYPE_LAYOUT 0x52 */
};

//  "PROXY_ENTITY" 0x1f2 498
//  "PROXY_OBJECT" 0x1f3 499

// Auto-generated array of names starting at index 500, with direct access by
// fixedtype.
static const char *const _dwg_type_names_variable[] = {
  /* Start auto-generated variable. Do not modify */
  // clang-format: off
  "ACDSRECORD",                                               /* 500 */
  "ACDSSCHEMA",                                               /* 501 */
  "ACMECOMMANDHISTORY",                                       /* 502 */
  "ACMESCOPE",                                                /* 503 */
  "ACMESTATEMGR",                                             /* 504 */
  "ACSH_BOOLEAN_CLASS",                                       /* 505 */
  "ACSH_BOX_CLASS",                                           /* 506 */
  "ACSH_BREP_CLASS",                                          /* 507 */
  "ACSH_CHAMFER_CLASS",                                       /* 508 */
  "ACSH_CONE_CLASS",                                          /* 509 */
  "ACSH_CYLINDER_CLASS",                                      /* 510 */
  "ACSH_EXTRUSION_CLASS",                                     /* 511 */
  "ACSH_FILLET_CLASS",                                        /* 512 */
  "ACSH_HISTORY_CLASS",                                       /* 513 */
  "ACSH_LOFT_CLASS",                                          /* 514 */
  "ACSH_PYRAMID_CLASS",                                       /* 515 */
  "ACSH_REVOLVE_CLASS",                                       /* 516 */
  "ACSH_SPHERE_CLASS",                                        /* 517 */
  "ACSH_SWEEP_CLASS",                                         /* 518 */
  "ACSH_TORUS_CLASS",                                         /* 519 */
  "ACSH_WEDGE_CLASS",                                         /* 520 */
  "ALDIMOBJECTCONTEXTDATA",                                   /* 521 */
  "ALIGNMENTPARAMETERENTITY",                                 /* 522 */
  "ANGDIMOBJECTCONTEXTDATA",                                  /* 523 */
  "ANNOTSCALEOBJECTCONTEXTDATA",                              /* 524 */
  "ARCALIGNEDTEXT",                                           /* 525 */
  "ARC_DIMENSION",                                            /* 526 */
  "ASSOC2DCONSTRAINTGROUP",                                   /* 527 */
  "ASSOC3POINTANGULARDIMACTIONBODY",                          /* 528 */
  "ASSOCACTION",                                              /* 529 */
  "ASSOCACTIONPARAM",                                         /* 530 */
  "ASSOCALIGNEDDIMACTIONBODY",                                /* 531 */
  "ASSOCARRAYACTIONBODY",                                     /* 532 */
  "ASSOCARRAYMODIFYACTIONBODY",                               /* 533 */
  "ASSOCARRAYMODIFYPARAMETERS",                               /* 534 */
  "ASSOCARRAYPATHPARAMETERS",                                 /* 535 */
  "ASSOCARRAYPOLARPARAMETERS",                                /* 536 */
  "ASSOCARRAYRECTANGULARPARAMETERS",                          /* 537 */
  "ASSOCASMBODYACTIONPARAM",                                  /* 538 */
  "ASSOCBLENDSURFACEACTIONBODY",                              /* 539 */
  "ASSOCCOMPOUNDACTIONPARAM",                                 /* 540 */
  "ASSOCDEPENDENCY",                                          /* 541 */
  "ASSOCDIMDEPENDENCYBODY",                                   /* 542 */
  "ASSOCEDGEACTIONPARAM",                                     /* 543 */
  "ASSOCEDGECHAMFERACTIONBODY",                               /* 544 */
  "ASSOCEDGEFILLETACTIONBODY",                                /* 545 */
  "ASSOCEXTENDSURFACEACTIONBODY",                             /* 546 */
  "ASSOCEXTRUDEDSURFACEACTIONBODY",                           /* 547 */
  "ASSOCFACEACTIONPARAM",                                     /* 548 */
  "ASSOCFILLETSURFACEACTIONBODY",                             /* 549 */
  "ASSOCGEOMDEPENDENCY",                                      /* 550 */
  "ASSOCLOFTEDSURFACEACTIONBODY",                             /* 551 */
  "ASSOCMLEADERACTIONBODY",                                   /* 552 */
  "ASSOCNETWORK",                                             /* 553 */
  "ASSOCNETWORKSURFACEACTIONBODY",                            /* 554 */
  "ASSOCOBJECTACTIONPARAM",                                   /* 555 */
  "ASSOCOFFSETSURFACEACTIONBODY",                             /* 556 */
  "ASSOCORDINATEDIMACTIONBODY",                               /* 557 */
  "ASSOCOSNAPPOINTREFACTIONPARAM",                            /* 558 */
  "ASSOCPATCHSURFACEACTIONBODY",                              /* 559 */
  "ASSOCPATHACTIONPARAM",                                     /* 560 */
  "ASSOCPERSSUBENTMANAGER",                                   /* 561 */
  "ASSOCPLANESURFACEACTIONBODY",                              /* 562 */
  "ASSOCPOINTREFACTIONPARAM",                                 /* 563 */
  "ASSOCRESTOREENTITYSTATEACTIONBODY",                        /* 564 */
  "ASSOCREVOLVEDSURFACEACTIONBODY",                           /* 565 */
  "ASSOCROTATEDDIMACTIONBODY",                                /* 566 */
  "ASSOCSWEPTSURFACEACTIONBODY",                              /* 567 */
  "ASSOCTRIMSURFACEACTIONBODY",                               /* 568 */
  "ASSOCVALUEDEPENDENCY",                                     /* 569 */
  "ASSOCVARIABLE",                                            /* 570 */
  "ASSOCVERTEXACTIONPARAM",                                   /* 571 */
  "BASEPOINTPARAMETERENTITY",                                 /* 572 */
  "BLKREFOBJECTCONTEXTDATA",                                  /* 573 */
  "BLOCKALIGNEDCONSTRAINTPARAMETER",                          /* 574 */
  "BLOCKALIGNMENTGRIP",                                       /* 575 */
  "BLOCKALIGNMENTPARAMETER",                                  /* 576 */
  "BLOCKANGULARCONSTRAINTPARAMETER",                          /* 577 */
  "BLOCKARRAYACTION",                                         /* 578 */
  "BLOCKBASEPOINTPARAMETER",                                  /* 579 */
  "BLOCKDIAMETRICCONSTRAINTPARAMETER",                        /* 580 */
  "BLOCKFLIPACTION",                                          /* 581 */
  "BLOCKFLIPGRIP",                                            /* 582 */
  "BLOCKFLIPPARAMETER",                                       /* 583 */
  "BLOCKGRIPLOCATIONCOMPONENT",                               /* 584 */
  "BLOCKHORIZONTALCONSTRAINTPARAMETER",                       /* 585 */
  "BLOCKLINEARCONSTRAINTPARAMETER",                           /* 586 */
  "BLOCKLINEARGRIP",                                          /* 587 */
  "BLOCKLINEARPARAMETER",                                     /* 588 */
  "BLOCKLOOKUPACTION",                                        /* 589 */
  "BLOCKLOOKUPGRIP",                                          /* 590 */
  "BLOCKLOOKUPPARAMETER",                                     /* 591 */
  "BLOCKMOVEACTION",                                          /* 592 */
  "BLOCKPARAMDEPENDENCYBODY",                                 /* 593 */
  "BLOCKPOINTPARAMETER",                                      /* 594 */
  "BLOCKPOLARGRIP",                                           /* 595 */
  "BLOCKPOLARPARAMETER",                                      /* 596 */
  "BLOCKPOLARSTRETCHACTION",                                  /* 597 */
  "BLOCKPROPERTIESTABLE",                                     /* 598 */
  "BLOCKPROPERTIESTABLEGRIP",                                 /* 599 */
  "BLOCKRADIALCONSTRAINTPARAMETER",                           /* 600 */
  "BLOCKREPRESENTATION",                                      /* 601 */
  "BLOCKROTATEACTION",                                        /* 602 */
  "BLOCKROTATIONGRIP",                                        /* 603 */
  "BLOCKROTATIONPARAMETER",                                   /* 604 */
  "BLOCKSCALEACTION",                                         /* 605 */
  "BLOCKSTRETCHACTION",                                       /* 606 */
  "BLOCKUSERPARAMETER",                                       /* 607 */
  "BLOCKVERTICALCONSTRAINTPARAMETER",                         /* 608 */
  "BLOCKVISIBILITYGRIP",                                      /* 609 */
  "BLOCKVISIBILITYPARAMETER",                                 /* 610 */
  "BLOCKXYGRIP",                                              /* 611 */
  "BLOCKXYPARAMETER",                                         /* 612 */
  "CAMERA",                                                   /* 613 */
  "CELLSTYLEMAP",                                             /* 614 */
  "CONTEXTDATAMANAGER",                                       /* 615 */
  "CSACDOCUMENTOPTIONS",                                      /* 616 */
  "CURVEPATH",                                                /* 617 */
  "DATALINK",                                                 /* 618 */
  "DATATABLE",                                                /* 619 */
  "DBCOLOR",                                                  /* 620 */
  "DETAILVIEWSTYLE",                                          /* 621 */
  "DGNDEFINITION",                                            /* 622 */
  "DGNUNDERLAY",                                              /* 623 */
  "DICTIONARYVAR",                                            /* 624 */
  "DICTIONARYWDFLT",                                          /* 625 */
  "DIMASSOC",                                                 /* 626 */
  "DMDIMOBJECTCONTEXTDATA",                                   /* 627 */
  "DWFDEFINITION",                                            /* 628 */
  "DWFUNDERLAY",                                              /* 629 */
  "DYNAMICBLOCKPROXYNODE",                                    /* 630 */
  "DYNAMICBLOCKPURGEPREVENTER",                               /* 631 */
  "EVALUATION_GRAPH",                                         /* 632 */
  "EXTRUDEDSURFACE",                                          /* 633 */
  "FCFOBJECTCONTEXTDATA",                                     /* 634 */
  "FIELD",                                                    /* 635 */
  "FIELDLIST",                                                /* 636 */
  "FLIPPARAMETERENTITY",                                      /* 637 */
  "FLIPACTIONENTITY",                                         /* 638 */
  "GEODATA",                                                  /* 639 */
  "GEOMAPIMAGE",                                              /* 640 */
  "GEOPOSITIONMARKER",                                        /* 641 */
  "GRADIENT_BACKGROUND",                                      /* 642 */
  "GROUND_PLANE_BACKGROUND",                                  /* 643 */
  "HELIX",                                                    /* 644 */
  "IBL_BACKGROUND",                                           /* 645 */
  "IDBUFFER",                                                 /* 646 */
  "IMAGE",                                                    /* 647 */
  "IMAGEDEF",                                                 /* 648 */
  "IMAGEDEF_REACTOR",                                         /* 649 */
  "IMAGE_BACKGROUND",                                         /* 650 */
  "INDEX",                                                    /* 651 */
  "LARGE_RADIAL_DIMENSION",                                   /* 652 */
  "LAYERFILTER",                                              /* 653 */
  "LAYER_INDEX",                                              /* 654 */
  "LAYOUTPRINTCONFIG",                                        /* 655 */
  "LEADEROBJECTCONTEXTDATA",                                  /* 656 */
  "LIGHT",                                                    /* 657 */
  "LIGHTLIST",                                                /* 658 */
  "LINEARPARAMETERENTITY",                                    /* 659 */
  "LOFTEDSURFACE",                                            /* 660 */
  "MATERIAL",                                                 /* 661 */
  "MENTALRAYRENDERSETTINGS",                                  /* 662 */
  "MESH",                                                     /* 663 */
  "MLEADEROBJECTCONTEXTDATA",                                 /* 664 */
  "MLEADERSTYLE",                                             /* 665 */
  "MOVEACTIONENTITY",                                         /* 666 */
  "MOTIONPATH",                                               /* 667 */
  "MPOLYGON",                                                 /* 668 */
  "MTEXTATTRIBUTEOBJECTCONTEXTDATA",                          /* 669 */
  "MTEXTOBJECTCONTEXTDATA",                                   /* 670 */
  "MULTILEADER",                                              /* 671 */
  "NAVISWORKSMODEL",                                          /* 672 */
  "NAVISWORKSMODELDEF",                                       /* 673 */
  "NPOCOLLECTION",                                            /* 674 */
  "NURBSURFACE",                                              /* 675 */
  "OBJECT_PTR",                                               /* 676 */
  "ORDDIMOBJECTCONTEXTDATA",                                  /* 677 */
  "PARTIAL_VIEWING_INDEX",                                    /* 678 */
  "PDFDEFINITION",                                            /* 679 */
  "PDFUNDERLAY",                                              /* 680 */
  "PERSUBENTMGR",                                             /* 681 */
  "PLANESURFACE",                                             /* 682 */
  "PLOTSETTINGS",                                             /* 683 */
  "POINTCLOUD",                                               /* 684 */
  "POINTCLOUDCOLORMAP",                                       /* 685 */
  "POINTCLOUDDEF",                                            /* 686 */
  "POINTCLOUDDEFEX",                                          /* 687 */
  "POINTCLOUDDEF_REACTOR",                                    /* 688 */
  "POINTCLOUDDEF_REACTOR_EX",                                 /* 689 */
  "POINTCLOUDEX",                                             /* 690 */
  "POINTPARAMETERENTITY",                                     /* 691 */
  "POINTPATH",                                                /* 692 */
  "POLARGRIPENTITY",                                          /* 693 */
  "RADIMLGOBJECTCONTEXTDATA",                                 /* 694 */
  "RADIMOBJECTCONTEXTDATA",                                   /* 695 */
  "RAPIDRTRENDERSETTINGS",                                    /* 696 */
  "RASTERVARIABLES",                                          /* 697 */
  "RENDERENTRY",                                              /* 698 */
  "RENDERENVIRONMENT",                                        /* 699 */
  "RENDERGLOBAL",                                             /* 700 */
  "RENDERSETTINGS",                                           /* 701 */
  "REVOLVEDSURFACE",                                          /* 702 */
  "ROTATIONPARAMETERENTITY",                                  /* 703 */
  "ROTATEACTIONENTITY",                                       /* 704 */
  "RTEXT",                                                    /* 705 */
  "SCALE",                                                    /* 706 */
  "SCALEACTIONENTITY",                                        /* 707 */
  "SECTIONOBJECT",                                            /* 708 */
  "SECTIONVIEWSTYLE",                                         /* 709 */
  "SECTION_MANAGER",                                          /* 710 */
  "SECTION_SETTINGS",                                         /* 711 */
  "SKYLIGHT_BACKGROUND",                                      /* 712 */
  "SOLID_BACKGROUND",                                         /* 713 */
  "SORTENTSTABLE",                                            /* 714 */
  "SPATIAL_FILTER",                                           /* 715 */
  "SPATIAL_INDEX",                                            /* 716 */
  "STRETCHACTIONENTITY",                                      /* 717 */
  "SUN",                                                      /* 718 */
  "SUNSTUDY",                                                 /* 719 */
  "SWEPTSURFACE",                                             /* 720 */
  "TABLE",                                                    /* 721 */
  "TABLECONTENT",                                             /* 722 */
  "TABLEGEOMETRY",                                            /* 723 */
  "TABLESTYLE",                                               /* 724 */
  "TEXTOBJECTCONTEXTDATA",                                    /* 725 */
  "TVDEVICEPROPERTIES",                                       /* 726 */
  "VISIBILITYGRIPENTITY",                                     /* 727 */
  "VISIBILITYPARAMETERENTITY",                                /* 728 */
  "VISUALSTYLE",                                              /* 729 */
  "WIPEOUT",                                                  /* 730 */
  "WIPEOUTVARIABLES",                                         /* 731 */
  "XREFPANELOBJECT",                                          /* 732 */
  "XYPARAMETERENTITY",                                        /* 733 */
  "BREAKDATA",                                                /* 734 */
  "BREAKPOINTREF",                                            /* 735 */
  "FLIPGRIPENTITY",                                           /* 736 */
  "LINEARGRIPENTITY",                                         /* 737 */
  "ROTATIONGRIPENTITY",                                       /* 738 */
  "XYGRIPENTITY",                                             /* 739 */
  "_3DLINE",                                                  /* 740 */
  "REPEAT",                                                   /* 741 */
  "ENDREP",                                                   /* 742 */
  "JUMP",                                                     /* 743 */
  "POLYLINE",                                                 /* 744 */
  "VERTEX",                                                   /* 745 */
  "LOAD",                                                     /* 746 */
  "ABSHDRAWINGSETTINGS",                                      /* 747 */
  "ACAECUSTOBJ",                                              /* 748 */
  "ACAEEEMGROBJ",                                             /* 749 */
  "ACAMCOMP",                                                 /* 750 */
  "ACAMCOMPDEF",                                              /* 751 */
  "ACAMCOMPDEFMGR",                                           /* 752 */
  "ACAMCONTEXTMODELER",                                       /* 753 */
  "ACAMGDIMSTD",                                              /* 754 */
  "ACAMGFILTERDAT",                                           /* 755 */
  "ACAMGHOLECHARTSTDCSN",                                     /* 756 */
  "ACAMGHOLECHARTSTDDIN",                                     /* 757 */
  "ACAMGHOLECHARTSTDISO",                                     /* 758 */
  "ACAMGLAYSTD",                                              /* 759 */
  "ACAMGRCOMPDEF",                                            /* 760 */
  "ACAMGRCOMPDEFSET",                                         /* 761 */
  "ACAMGTITLESTD",                                            /* 762 */
  "ACAMMVDBACKUPOBJECT",                                      /* 763 */
  "ACAMPROJECT",                                              /* 764 */
  "ACAMSHAFTCOMPDEF",                                         /* 765 */
  "ACAMSTDPCOMPDEF",                                          /* 766 */
  "ACAMWBLOCKTEMPENTS",                                       /* 767 */
  "ACARRAYJIGENTITY",                                         /* 768 */
  "ACCMCONTEXT",                                              /* 769 */
  "ACDBCIRCARCRES",                                           /* 770 */
  "ACDBDIMENSIONRES",                                         /* 771 */
  "ACDBENTITYCACHE",                                          /* 772 */
  "ACDBLINERES",                                              /* 773 */
  "ACDBSTDPARTRES_ARC",                                       /* 774 */
  "ACDBSTDPARTRES_LINE",                                      /* 775 */
  "ACDB_HATCHSCALECONTEXTDATA_CLASS",                         /* 776 */
  "ACDB_HATCHVIEWCONTEXTDATA_CLASS",                          /* 777 */
  "ACDB_PROXY_ENTITY_DATA",                                   /* 778 */
  "ACGREFACADMASTER",                                         /* 779 */
  "ACGREFMASTER",                                             /* 780 */
  "ACIMINTSYSVAR",                                            /* 781 */
  "ACIMREALSYSVAR",                                           /* 782 */
  "ACIMSTRSYSVAR",                                            /* 783 */
  "ACIMSYSVARMAN",                                            /* 784 */
  "ACMANOOTATIONVIEWSTANDARDANSI",                            /* 785 */
  "ACMANOOTATIONVIEWSTANDARDCSN",                             /* 786 */
  "ACMANOOTATIONVIEWSTANDARDDIN",                             /* 787 */
  "ACMANOOTATIONVIEWSTANDARDISO",                             /* 788 */
  "ACMAPLEGENDDBOBJECT",                                      /* 789 */
  "ACMAPLEGENDITEMDBOBJECT",                                  /* 790 */
  "ACMAPMAPVIEWPORTDBOBJECT",                                 /* 791 */
  "ACMAPPRINTLAYOUTELEMENTDBOBJECTCONTAINER",                 /* 792 */
  "ACMBALLOON",                                               /* 793 */
  "ACMBOM",                                                   /* 794 */
  "ACMBOMROW",                                                /* 795 */
  "ACMBOMROWSTRUCT",                                          /* 796 */
  "ACMBOMSTANDARDANSI",                                       /* 797 */
  "ACMBOMSTANDARDCSN",                                        /* 798 */
  "ACMBOMSTANDARDDIN",                                        /* 799 */
  "ACMBOMSTANDARDISO",                                        /* 800 */
  "ACMCENTERLINESTANDARDANSI",                                /* 801 */
  "ACMCENTERLINESTANDARDCSN",                                 /* 802 */
  "ACMCENTERLINESTANDARDDIN",                                 /* 803 */
  "ACMCENTERLINESTANDARDISO",                                 /* 804 */
  "ACMDATADICTIONARY",                                        /* 805 */
  "ACMDATAENTRY",                                             /* 806 */
  "ACMDATAENTRYBLOCK",                                        /* 807 */
  "ACMDATUMID",                                               /* 808 */
  "ACMDATUMSTANDARDANSI",                                     /* 809 */
  "ACMDATUMSTANDARDCSN",                                      /* 810 */
  "ACMDATUMSTANDARDDIN",                                      /* 811 */
  "ACMDATUMSTANDARDISO",                                      /* 812 */
  "ACMDATUMSTANDARDISO2012",                                  /* 813 */
  "ACMDETAILSTANDARDANSI",                                    /* 814 */
  "ACMDETAILSTANDARDCSN",                                     /* 815 */
  "ACMDETAILSTANDARDDIN",                                     /* 816 */
  "ACMDETAILSTANDARDISO",                                     /* 817 */
  "ACMDETAILTANDARDCUSTOM",                                   /* 818 */
  "ACMDIMBREAKPERSREACTOR",                                   /* 819 */
  "ACMEDRAWINGMAN",                                           /* 820 */
  "ACMEVIEW",                                                 /* 821 */
  "ACME_DATABASE",                                            /* 822 */
  "ACME_DOCUMENT",                                            /* 823 */
  "ACMFCFRAME",                                               /* 824 */
  "ACMFCFSTANDARDANSI",                                       /* 825 */
  "ACMFCFSTANDARDCSN",                                        /* 826 */
  "ACMFCFSTANDARDDIN",                                        /* 827 */
  "ACMFCFSTANDARDISO",                                        /* 828 */
  "ACMFCFSTANDARDISO2004",                                    /* 829 */
  "ACMFCFSTANDARDISO2012",                                    /* 830 */
  "ACMIDSTANDARDANSI",                                        /* 831 */
  "ACMIDSTANDARDCSN",                                         /* 832 */
  "ACMIDSTANDARDDIN",                                         /* 833 */
  "ACMIDSTANDARDISO",                                         /* 834 */
  "ACMIDSTANDARDISO2004",                                     /* 835 */
  "ACMIDSTANDARDISO2012",                                     /* 836 */
  "ACMNOTESTANDARDANSI",                                      /* 837 */
  "ACMNOTESTANDARDCSN",                                       /* 838 */
  "ACMNOTESTANDARDDIN",                                       /* 839 */
  "ACMNOTESTANDARDISO",                                       /* 840 */
  "ACMPARTLIST",                                              /* 841 */
  "ACMPICKOBJ",                                               /* 842 */
  "ACMSECTIONSTANDARDANSI",                                   /* 843 */
  "ACMSECTIONSTANDARDCSN2002",                                /* 844 */
  "ACMSECTIONSTANDARDCUSTOM",                                 /* 845 */
  "ACMSECTIONSTANDARDDIN",                                    /* 846 */
  "ACMSECTIONSTANDARDISO",                                    /* 847 */
  "ACMSECTIONSTANDARDISO2001",                                /* 848 */
  "ACMSTANDARDANSI",                                          /* 849 */
  "ACMSTANDARDCSN",                                           /* 850 */
  "ACMSTANDARDDIN",                                           /* 851 */
  "ACMSTANDARDISO",                                           /* 852 */
  "ACMSURFSTANDARDANSI",                                      /* 853 */
  "ACMSURFSTANDARDCSN",                                       /* 854 */
  "ACMSURFSTANDARDDIN",                                       /* 855 */
  "ACMSURFSTANDARDISO",                                       /* 856 */
  "ACMSURFSTANDARDISO2002",                                   /* 857 */
  "ACMSURFSYM",                                               /* 858 */
  "ACMTAPERSTANDARDANSI",                                     /* 859 */
  "ACMTAPERSTANDARDCSN",                                      /* 860 */
  "ACMTAPERSTANDARDDIN",                                      /* 861 */
  "ACMTAPERSTANDARDISO",                                      /* 862 */
  "ACMTHREADLINESTANDARDANSI",                                /* 863 */
  "ACMTHREADLINESTANDARDCSN",                                 /* 864 */
  "ACMTHREADLINESTANDARDDIN",                                 /* 865 */
  "ACMTHREADLINESTANDARDISO",                                 /* 866 */
  "ACMWELDSTANDARDANSI",                                      /* 867 */
  "ACMWELDSTANDARDCSN",                                       /* 868 */
  "ACMWELDSTANDARDDIN",                                       /* 869 */
  "ACMWELDSTANDARDISO",                                       /* 870 */
  "ACMWELDSYM",                                               /* 871 */
  "ACRFATTGENMGR",                                            /* 872 */
  "ACRFINSADJ",                                               /* 873 */
  "ACRFINSADJUSTERMGR",                                       /* 874 */
  "ACRFMCADAPIATTHOLDER",                                     /* 875 */
  "ACRFOBJATTMGR",                                            /* 876 */
  "ACSH_SUBENT_MATERIAL_CLASS",                               /* 877 */
  "AC_AM_2D_XREF_MGR",                                        /* 878 */
  "AC_AM_BASIC_VIEW",                                         /* 879 */
  "AC_AM_BASIC_VIEW_DEF",                                     /* 880 */
  "AC_AM_COMPLEX_HIDE_SITUATION",                             /* 881 */
  "AC_AM_COMP_VIEW_DEF",                                      /* 882 */
  "AC_AM_COMP_VIEW_INST",                                     /* 883 */
  "AC_AM_DIRTY_NODES",                                        /* 884 */
  "AC_AM_HIDE_SITUATION",                                     /* 885 */
  "AC_AM_MAPPER_CACHE",                                       /* 886 */
  "AC_AM_MASTER_VIEW_DEF",                                    /* 887 */
  "AC_AM_MVD_DEP_MGR",                                        /* 888 */
  "AC_AM_OVERRIDE_FILTER",                                    /* 889 */
  "AC_AM_PROPS_OVERRIDE",                                     /* 890 */
  "AC_AM_SHAFT_HIDE_SITUATION",                               /* 891 */
  "AC_AM_STDP_VIEW_DEF",                                      /* 892 */
  "AC_AM_TRANSFORM_GHOST",                                    /* 893 */
  "ADAPPL",                                                   /* 894 */
  "AECC_ALIGNMENT_DESIGN_CHECK_SET",                          /* 895 */
  "AECC_ALIGNMENT_LABEL_SET",                                 /* 896 */
  "AECC_ALIGNMENT_LABEL_SET_EXT",                             /* 897 */
  "AECC_ALIGNMENT_PARCEL_NODE",                               /* 898 */
  "AECC_ALIGNMENT_STYLE",                                     /* 899 */
  "AECC_APPURTENANCE_STYLE",                                  /* 900 */
  "AECC_ASSEMBLY_STYLE",                                      /* 901 */
  "AECC_BUILDING_SITE_STYLE",                                 /* 902 */
  "AECC_CANT_DIAGRAM_VIEW_STYLE",                             /* 903 */
  "AECC_CATCHMENT_STYLE",                                     /* 904 */
  "AECC_CLASS_NODE",                                          /* 905 */
  "AECC_CONTOURVIEW",                                         /* 906 */
  "AECC_CORRIDOR_STYLE",                                      /* 907 */
  "AECC_DISP_REP_ALIGNMENT",                                  /* 908 */
  "AECC_DISP_REP_ALIGNMENT_CANT_LABEL_GROUP",                 /* 909 */
  "AECC_DISP_REP_ALIGNMENT_CSV",                              /* 910 */
  "AECC_DISP_REP_ALIGNMENT_CURVE_LABEL",                      /* 911 */
  "AECC_DISP_REP_ALIGNMENT_DESIGNSPEED_LABEL_GROUP",          /* 912 */
  "AECC_DISP_REP_ALIGNMENT_GEOMPT_LABEL_GROUP",               /* 913 */
  "AECC_DISP_REP_ALIGNMENT_INDEXED_PI_LABEL",                 /* 914 */
  "AECC_DISP_REP_ALIGNMENT_MINOR_STATION_LABEL_GROUP",        /* 915 */
  "AECC_DISP_REP_ALIGNMENT_PI_LABEL",                         /* 916 */
  "AECC_DISP_REP_ALIGNMENT_SPIRAL_LABEL",                     /* 917 */
  "AECC_DISP_REP_ALIGNMENT_STAEQU_LABEL_GROUP",               /* 918 */
  "AECC_DISP_REP_ALIGNMENT_STATION_LABEL_GROUP",              /* 919 */
  "AECC_DISP_REP_ALIGNMENT_STATION_OFFSET_LABEL",             /* 920 */
  "AECC_DISP_REP_ALIGNMENT_SUPERELEVATION_LABEL_GROUP",       /* 921 */
  "AECC_DISP_REP_ALIGNMENT_TABLE",                            /* 922 */
  "AECC_DISP_REP_ALIGNMENT_TANGENT_LABEL",                    /* 923 */
  "AECC_DISP_REP_ALIGNMENT_VERTICAL_GEOMPT_LABELING",         /* 924 */
  "AECC_DISP_REP_APPURTENANCE",                               /* 925 */
  "AECC_DISP_REP_APPURTENANCE_CSV",                           /* 926 */
  "AECC_DISP_REP_APPURTENANCE_LABELING",                      /* 927 */
  "AECC_DISP_REP_APPURTENANCE_PROFILE_LABELING",              /* 928 */
  "AECC_DISP_REP_ASSEMBLY",                                   /* 929 */
  "AECC_DISP_REP_AUTO_CORRIDOR_FEATURE_LINE",                 /* 930 */
  "AECC_DISP_REP_AUTO_CORRIDOR_FEATURE_LINE_PROFILE",         /* 931 */
  "AECC_DISP_REP_AUTO_CORRIDOR_FEATURE_LINE_SECTION",         /* 932 */
  "AECC_DISP_REP_AUTO_FEATURE_LINE",                          /* 933 */
  "AECC_DISP_REP_AUTO_FEATURE_LINE_CSV",                      /* 934 */
  "AECC_DISP_REP_AUTO_FEATURE_LINE_PROFILE",                  /* 935 */
  "AECC_DISP_REP_AUTO_FEATURE_LINE_SECTION",                  /* 936 */
  "AECC_DISP_REP_BUILDINGSITE",                               /* 937 */
  "AECC_DISP_REP_BUILDINGUTIL_CONNECTOR",                     /* 938 */
  "AECC_DISP_REP_CANT_DIAGRAM_VIEW",                          /* 939 */
  "AECC_DISP_REP_CATCHMENT_AREA",                             /* 940 */
  "AECC_DISP_REP_CATCHMENT_AREA_LABEL",                       /* 941 */
  "AECC_DISP_REP_CORRIDOR",                                   /* 942 */
  "AECC_DISP_REP_CROSSING_PIPE_PROFILE_LABELING",             /* 943 */
  "AECC_DISP_REP_CROSSING_PRESSURE_PIPE_PROFILE_LABELING",    /* 944 */
  "AECC_DISP_REP_CSVSTATIONSLIDER",                           /* 945 */
  "AECC_DISP_REP_FACE",                                       /* 946 */
  "AECC_DISP_REP_FEATURE",                                    /* 947 */
  "AECC_DISP_REP_FEATURE_LABEL",                              /* 948 */
  "AECC_DISP_REP_FEATURE_LINE",                               /* 949 */
  "AECC_DISP_REP_FEATURE_LINE_CSV",                           /* 950 */
  "AECC_DISP_REP_FEATURE_LINE_PROFILE",                       /* 951 */
  "AECC_DISP_REP_FEATURE_LINE_SECTION",                       /* 952 */
  "AECC_DISP_REP_FITTING",                                    /* 953 */
  "AECC_DISP_REP_FITTING_CSV",                                /* 954 */
  "AECC_DISP_REP_FITTING_LABELING",                           /* 955 */
  "AECC_DISP_REP_FITTING_PROFILE_LABELING",                   /* 956 */
  "AECC_DISP_REP_FLOW_SEGMENT_LABEL",                         /* 957 */
  "AECC_DISP_REP_GENERAL_SEGMENT_LABEL",                      /* 958 */
  "AECC_DISP_REP_GRADING",                                    /* 959 */
  "AECC_DISP_REP_GRAPH",                                      /* 960 */
  "AECC_DISP_REP_GRAPHPROFILE_NETWORKPART",                   /* 961 */
  "AECC_DISP_REP_GRAPHPROFILE_PRESSUREPART",                  /* 962 */
  "AECC_DISP_REP_GRID_SURFACE",                               /* 963 */
  "AECC_DISP_REP_GRID_SURFACE_CSV",                           /* 964 */
  "AECC_DISP_REP_HORGEOMETRY_BAND_LABEL_GROUP",               /* 965 */
  "AECC_DISP_REP_HYDRO_REGION",                               /* 966 */
  "AECC_DISP_REP_INTERFERENCE_CHECK",                         /* 967 */
  "AECC_DISP_REP_INTERFERENCE_PART",                          /* 968 */
  "AECC_DISP_REP_INTERFERENCE_PART_SECTION",                  /* 969 */
  "AECC_DISP_REP_INTERSECTION",                               /* 970 */
  "AECC_DISP_REP_INTERSECTION_LOCATION_LABELING",             /* 971 */
  "AECC_DISP_REP_LABELING",                                   /* 972 */
  "AECC_DISP_REP_LEGEND_TABLE",                               /* 973 */
  "AECC_DISP_REP_LINE_BETWEEN_POINTS_LABEL",                  /* 974 */
  "AECC_DISP_REP_LOTLINE_CSV",                                /* 975 */
  "AECC_DISP_REP_MASSHAULLINE",                               /* 976 */
  "AECC_DISP_REP_MASS_HAUL_VIEW",                             /* 977 */
  "AECC_DISP_REP_MATCHLINE_LABELING",                         /* 978 */
  "AECC_DISP_REP_MATCH_LINE",                                 /* 979 */
  "AECC_DISP_REP_MATERIAL_SECTION",                           /* 980 */
  "AECC_DISP_REP_NETWORK",                                    /* 981 */
  "AECC_DISP_REP_NOTE_LABEL",                                 /* 982 */
  "AECC_DISP_REP_OFFSET_ELEV_LABEL",                          /* 983 */
  "AECC_DISP_REP_PARCEL_BOUNDARY",                            /* 984 */
  "AECC_DISP_REP_PARCEL_FACE_LABEL",                          /* 985 */
  "AECC_DISP_REP_PARCEL_SEGMENT",                             /* 986 */
  "AECC_DISP_REP_PARCEL_SEGMENT_LABEL",                       /* 987 */
  "AECC_DISP_REP_PARCEL_SEGMENT_TABLE",                       /* 988 */
  "AECC_DISP_REP_PARCEL_TABLE",                               /* 989 */
  "AECC_DISP_REP_PIPE",                                       /* 990 */
  "AECC_DISP_REP_PIPENETWORK_BAND_LABEL_GROUP",               /* 991 */
  "AECC_DISP_REP_PIPE_CSV",                                   /* 992 */
  "AECC_DISP_REP_PIPE_LABELING",                              /* 993 */
  "AECC_DISP_REP_PIPE_PROFILE_LABELING",                      /* 994 */
  "AECC_DISP_REP_PIPE_SECTION_LABELING",                      /* 995 */
  "AECC_DISP_REP_PIPE_TABLE",                                 /* 996 */
  "AECC_DISP_REP_POINT_ENT",                                  /* 997 */
  "AECC_DISP_REP_POINT_GROUP",                                /* 998 */
  "AECC_DISP_REP_POINT_TABLE",                                /* 999 */
  "AECC_DISP_REP_PRESSUREPIPENETWORK",                        /* 1000 */
  "AECC_DISP_REP_PRESSURE_PART_TABLE",                        /* 1001 */
  "AECC_DISP_REP_PRESSURE_PIPE",                              /* 1002 */
  "AECC_DISP_REP_PRESSURE_PIPE_CSV",                          /* 1003 */
  "AECC_DISP_REP_PRESSURE_PIPE_LABELING",                     /* 1004 */
  "AECC_DISP_REP_PRESSURE_PIPE_PROFILE_LABELING",             /* 1005 */
  "AECC_DISP_REP_PRESSURE_PIPE_SECTION_LABELING",             /* 1006 */
  "AECC_DISP_REP_PROFILE",                                    /* 1007 */
  "AECC_DISP_REP_PROFILEDATA_BAND_LABEL_GROUP",               /* 1008 */
  "AECC_DISP_REP_PROFILE_PROJECTION",                         /* 1009 */
  "AECC_DISP_REP_PROFILE_PROJECTION_LABEL",                   /* 1010 */
  "AECC_DISP_REP_PROFILE_VIEW",                               /* 1011 */
  "AECC_DISP_REP_PROFILE_VIEW_DEPTH_LABEL",                   /* 1012 */
  "AECC_DISP_REP_QUANTITY_TAKEOFF_AGGREGATE_EARTHWORK_TABLE", /* 1013 */
  "AECC_DISP_REP_RIGHT_OF_WAY",                               /* 1014 */
  "AECC_DISP_REP_SAMPLELINE_LABELING",                        /* 1015 */
  "AECC_DISP_REP_SAMPLE_LINE",                                /* 1016 */
  "AECC_DISP_REP_SAMPLE_LINE_GROUP",                          /* 1017 */
  "AECC_DISP_REP_SECTION",                                    /* 1018 */
  "AECC_DISP_REP_SECTIONALDATA_BAND_LABEL_GROUP",             /* 1019 */
  "AECC_DISP_REP_SECTIONDATA_BAND_LABEL_GROUP",               /* 1020 */
  "AECC_DISP_REP_SECTIONSEGMENT_BAND_LABEL_GROUP",            /* 1021 */
  "AECC_DISP_REP_SECTION_CORRIDOR",                           /* 1022 */
  "AECC_DISP_REP_SECTION_CORRIDOR_POINT_LABEL_GROUP",         /* 1023 */
  "AECC_DISP_REP_SECTION_GRADEBREAK_LABEL_GROUP",             /* 1024 */
  "AECC_DISP_REP_SECTION_MINOR_OFFSET_LABEL_GROUP",           /* 1025 */
  "AECC_DISP_REP_SECTION_OFFSET_LABEL_GROUP",                 /* 1026 */
  "AECC_DISP_REP_SECTION_PIPENETWORK",                        /* 1027 */
  "AECC_DISP_REP_SECTION_PRESSUREPIPENETWORK",                /* 1028 */
  "AECC_DISP_REP_SECTION_PROJECTION",                         /* 1029 */
  "AECC_DISP_REP_SECTION_PROJECTION_LABEL",                   /* 1030 */
  "AECC_DISP_REP_SECTION_SEGMENT_LABEL_GROUP",                /* 1031 */
  "AECC_DISP_REP_SECTION_VIEW",                               /* 1032 */
  "AECC_DISP_REP_SECTION_VIEW_DEPTH_LABEL",                   /* 1033 */
  "AECC_DISP_REP_SECTION_VIEW_QUANTITY_TAKEOFF_TABLE",        /* 1034 */
  "AECC_DISP_REP_SHEET",                                      /* 1035 */
  "AECC_DISP_REP_SPANNING_PIPE_LABELING",                     /* 1036 */
  "AECC_DISP_REP_SPANNING_PIPE_PROFILE_LABELING",             /* 1037 */
  "AECC_DISP_REP_STATION_ELEV_LABEL",                         /* 1038 */
  "AECC_DISP_REP_STRUCTURE",                                  /* 1039 */
  "AECC_DISP_REP_STRUCTURE_CSV",                              /* 1040 */
  "AECC_DISP_REP_STRUCTURE_LABELING",                         /* 1041 */
  "AECC_DISP_REP_STRUCTURE_PROFILE_LABELING",                 /* 1042 */
  "AECC_DISP_REP_STRUCTURE_SECTION_LABELING",                 /* 1043 */
  "AECC_DISP_REP_SUBASSEMBLY",                                /* 1044 */
  "AECC_DISP_REP_SUPERELEVATION_BAND_LABEL_GROUP",            /* 1045 */
  "AECC_DISP_REP_SUPERELEVATION_DIAGRAM_VIEW",                /* 1046 */
  "AECC_DISP_REP_SURFACE_CONTOUR_LABEL_GROUP",                /* 1047 */
  "AECC_DISP_REP_SURFACE_ELEVATION_LABEL",                    /* 1048 */
  "AECC_DISP_REP_SURFACE_SLOPE_LABEL",                        /* 1049 */
  "AECC_DISP_REP_SURVEY_FIGURE_LABEL_GROUP",                  /* 1050 */
  "AECC_DISP_REP_SVFIGURE",                                   /* 1051 */
  "AECC_DISP_REP_SVFIGURE_CSV",                               /* 1052 */
  "AECC_DISP_REP_SVFIGURE_PROFILE",                           /* 1053 */
  "AECC_DISP_REP_SVFIGURE_SECTION",                           /* 1054 */
  "AECC_DISP_REP_SVFIGURE_SEGMENT_LABEL",                     /* 1055 */
  "AECC_DISP_REP_SVNETWORK",                                  /* 1056 */
  "AECC_DISP_REP_TANGENT_INTERSECTION_TABLE",                 /* 1057 */
  "AECC_DISP_REP_TIN_SURFACE",                                /* 1058 */
  "AECC_DISP_REP_TIN_SURFACE_CSV",                            /* 1059 */
  "AECC_DISP_REP_VALIGNMENT_CRESTCURVE_LABEL_GROUP",          /* 1060 */
  "AECC_DISP_REP_VALIGNMENT_CSV",                             /* 1061 */
  "AECC_DISP_REP_VALIGNMENT_HAGEOMPT_LABEL_GROUP",            /* 1062 */
  "AECC_DISP_REP_VALIGNMENT_LINE_LABEL_GROUP",                /* 1063 */
  "AECC_DISP_REP_VALIGNMENT_MINOR_STATION_LABEL_GROUP",       /* 1064 */
  "AECC_DISP_REP_VALIGNMENT_PVI_LABEL_GROUP",                 /* 1065 */
  "AECC_DISP_REP_VALIGNMENT_SAGCURVE_LABEL_GROUP",            /* 1066 */
  "AECC_DISP_REP_VALIGNMENT_STATION_LABEL_GROUP",             /* 1067 */
  "AECC_DISP_REP_VERTICALGEOMETRY_BAND_LABEL_GROUP",          /* 1068 */
  "AECC_DISP_REP_VIEWFRAME_LABELING",                         /* 1069 */
  "AECC_DISP_REP_VIEW_FRAME",                                 /* 1070 */
  "AECC_FEATURELINE_STYLE",                                   /* 1071 */
  "AECC_FEATURE_STYLE",                                       /* 1072 */
  "AECC_FITTING_STYLE",                                       /* 1073 */
  "AECC_FORMAT_MANAGER_OBJECT",                               /* 1074 */
  "AECC_GRADEVIEW",                                           /* 1075 */
  "AECC_GRADING_CRITERIA",                                    /* 1076 */
  "AECC_GRADING_CRITERIA_SET",                                /* 1077 */
  "AECC_GRADING_GROUP",                                       /* 1078 */
  "AECC_GRADING_STYLE",                                       /* 1079 */
  "AECC_IMPORT_STORM_SEWER_DEFAULTS",                         /* 1080 */
  "AECC_INTERFERENCE_STYLE",                                  /* 1081 */
  "AECC_INTERSECTION_STYLE",                                  /* 1082 */
  "AECC_LABEL_COLLECTOR_STYLE",                               /* 1083 */
  "AECC_LABEL_NODE",                                          /* 1084 */
  "AECC_LABEL_RADIAL_LINE_STYLE",                             /* 1085 */
  "AECC_LABEL_TEXT_ITERATOR_CURVE_OR_SPIRAL_STYLE",           /* 1086 */
  "AECC_LABEL_TEXT_ITERATOR_STYLE",                           /* 1087 */
  "AECC_LABEL_TEXT_STYLE",                                    /* 1088 */
  "AECC_LABEL_VECTOR_ARROW_STYLE",                            /* 1089 */
  "AECC_LEGEND_TABLE_STYLE",                                  /* 1090 */
  "AECC_MASS_HAUL_LINE_STYLE",                                /* 1091 */
  "AECC_MASS_HAUL_VIEW_STYLE",                                /* 1092 */
  "AECC_MATCHLINE_STYLE",                                     /* 1093 */
  "AECC_MATERIAL_STYLE",                                      /* 1094 */
  "AECC_NETWORK_PART_CATALOG_DEF_NODE",                       /* 1095 */
  "AECC_NETWORK_PART_FAMILY_ITEM",                            /* 1096 */
  "AECC_NETWORK_PART_LIST",                                   /* 1097 */
  "AECC_NETWORK_RULE",                                        /* 1098 */
  "AECC_PARCEL_NODE",                                         /* 1099 */
  "AECC_PARCEL_STYLE",                                        /* 1100 */
  "AECC_PART_SIZE_FILTER",                                    /* 1101 */
  "AECC_PIPE_RULES",                                          /* 1102 */
  "AECC_PIPE_STYLE",                                          /* 1103 */
  "AECC_PIPE_STYLE_EXTENSION",                                /* 1104 */
  "AECC_POINTCLOUD_STYLE",                                    /* 1105 */
  "AECC_POINTVIEW",                                           /* 1106 */
  "AECC_POINT_STYLE",                                         /* 1107 */
  "AECC_PRESSURE_PART_LIST",                                  /* 1108 */
  "AECC_PRESSURE_PIPE_STYLE",                                 /* 1109 */
  "AECC_PROFILESECTIONENTITY_STYLE",                          /* 1110 */
  "AECC_PROFILE_DESIGN_CHECK_SET",                            /* 1111 */
  "AECC_PROFILE_LABEL_SET",                                   /* 1112 */
  "AECC_PROFILE_STYLE",                                       /* 1113 */
  "AECC_PROFILE_VIEW_BAND_STYLE_SET",                         /* 1114 */
  "AECC_PROFILE_VIEW_DATA_BAND_STYLE",                        /* 1115 */
  "AECC_PROFILE_VIEW_HORIZONTAL_GEOMETRY_BAND_STYLE",         /* 1116 */
  "AECC_PROFILE_VIEW_PIPE_NETWORK_BAND_STYLE",                /* 1117 */
  "AECC_PROFILE_VIEW_SECTIONAL_DATA_BAND_STYLE",              /* 1118 */
  "AECC_PROFILE_VIEW_STYLE",                                  /* 1119 */
  "AECC_PROFILE_VIEW_SUPERELEVATION_DIAGRAM_BAND_STYLE",      /* 1120 */
  "AECC_PROFILE_VIEW_VERTICAL_GEOMETRY_BAND_STYLE",           /* 1121 */
  "AECC_QUANTITY_TAKEOFF_CRITERIA",                           /* 1122 */
  "AECC_ROADWAYLINK_STYLE",                                   /* 1123 */
  "AECC_ROADWAYMARKER_STYLE",                                 /* 1124 */
  "AECC_ROADWAYSHAPE_STYLE",                                  /* 1125 */
  "AECC_ROADWAY_STYLE_SET",                                   /* 1126 */
  "AECC_ROOT_SETTINGS_NODE",                                  /* 1127 */
  "AECC_SAMPLE_LINE_GROUP_STYLE",                             /* 1128 */
  "AECC_SAMPLE_LINE_STYLE",                                   /* 1129 */
  "AECC_SECTION_LABEL_SET",                                   /* 1130 */
  "AECC_SECTION_STYLE",                                       /* 1131 */
  "AECC_SECTION_VIEW_BAND_STYLE_SET",                         /* 1132 */
  "AECC_SECTION_VIEW_DATA_BAND_STYLE",                        /* 1133 */
  "AECC_SECTION_VIEW_ROAD_SURFACE_BAND_STYLE",                /* 1134 */
  "AECC_SECTION_VIEW_STYLE",                                  /* 1135 */
  "AECC_SETTINGS_NODE",                                       /* 1136 */
  "AECC_SHEET_STYLE",                                         /* 1137 */
  "AECC_SLOPE_PATTERN_STYLE",                                 /* 1138 */
  "AECC_STATION_FORMAT_STYLE",                                /* 1139 */
  "AECC_STRUCTURE_RULES",                                     /* 1140 */
  "AECC_STUCTURE_STYLE",                                      /* 1141 */
  "AECC_SUPERELEVATION_DIAGRAM_VIEW_STYLE",                   /* 1142 */
  "AECC_SURFACE_STYLE",                                       /* 1143 */
  "AECC_SVFIGURE_STYLE",                                      /* 1144 */
  "AECC_SVNETWORK_STYLE",                                     /* 1145 */
  "AECC_TABLE_STYLE",                                         /* 1146 */
  "AECC_TAG_MANAGER",                                         /* 1147 */
  "AECC_TREE_NODE",                                           /* 1148 */
  "AECC_USER_DEFINED_ATTRIBUTE_CLASSIFICATION",               /* 1149 */
  "AECC_VALIGNMENT_STYLE_EXTENSION",                          /* 1150 */
  "AECC_VIEW_FRAME_STYLE",                                    /* 1151 */
  "AECS_DISP_PROPS_MEMBER",                                   /* 1152 */
  "AECS_DISP_PROPS_MEMBER_LOGICAL",                           /* 1153 */
  "AECS_DISP_PROPS_MEMBER_PLAN",                              /* 1154 */
  "AECS_DISP_PROPS_MEMBER_PLAN_SKETCH",                       /* 1155 */
  "AECS_DISP_PROPS_MEMBER_PROJECTED",                         /* 1156 */
  "AECS_DISP_REP_MEMBER_ELEVATION_DESIGN",                    /* 1157 */
  "AECS_DISP_REP_MEMBER_ELEVATION_DETAIL",                    /* 1158 */
  "AECS_DISP_REP_MEMBER_LOGICAL",                             /* 1159 */
  "AECS_DISP_REP_MEMBER_MODEL_DESIGN",                        /* 1160 */
  "AECS_DISP_REP_MEMBER_MODEL_DETAIL",                        /* 1161 */
  "AECS_DISP_REP_MEMBER_PLAN_DESIGN",                         /* 1162 */
  "AECS_DISP_REP_MEMBER_PLAN_DETAIL",                         /* 1163 */
  "AECS_DISP_REP_MEMBER_PLAN_SKETCH",                         /* 1164 */
  "AECS_MEMBER_NODE_SHAPE",                                   /* 1165 */
  "AECS_MEMBER_STYLE",                                        /* 1166 */
  "AEC_2DSECTION_STYLE",                                      /* 1167 */
  "AEC_AECDBDISPREPBDGELEVLINEPLAN100",                       /* 1168 */
  "AEC_AECDBDISPREPBDGELEVLINEPLAN50",                        /* 1169 */
  "AEC_AECDBDISPREPBDGSECTIONLINEPLAN100",                    /* 1170 */
  "AEC_AECDBDISPREPBDGSECTIONLINEPLAN50",                     /* 1171 */
  "AEC_AECDBDISPREPCEILINGGRIDPLAN100",                       /* 1172 */
  "AEC_AECDBDISPREPCEILINGGRIDPLAN50",                        /* 1173 */
  "AEC_AECDBDISPREPCOLUMNGRIDPLAN100",                        /* 1174 */
  "AEC_AECDBDISPREPCOLUMNGRIDPLAN50",                         /* 1175 */
  "AEC_AECDBDISPREPCURTAINWALLLAYOUTPLAN100",                 /* 1176 */
  "AEC_AECDBDISPREPCURTAINWALLLAYOUTPLAN50",                  /* 1177 */
  "AEC_AECDBDISPREPCURTAINWALLUNITPLAN100",                   /* 1178 */
  "AEC_AECDBDISPREPCURTAINWALLUNITPLAN50",                    /* 1179 */
  "AEC_AECDBDISPREPMVBLOCKREFPLAN100",                        /* 1180 */
  "AEC_AECDBDISPREPMVBLOCKREFPLAN50",                         /* 1181 */
  "AEC_AECDBDISPREPROOFPLAN100",                              /* 1182 */
  "AEC_AECDBDISPREPROOFPLAN50",                               /* 1183 */
  "AEC_AECDBDISPREPROOFSLABPLAN100",                          /* 1184 */
  "AEC_AECDBDISPREPROOFSLABPLAN50",                           /* 1185 */
  "AEC_AECDBDISPREPSLABPLAN100",                              /* 1186 */
  "AEC_AECDBDISPREPSLABPLAN50",                               /* 1187 */
  "AEC_AECDBDISPREPSPACEPLAN100",                             /* 1188 */
  "AEC_AECDBDISPREPSPACEPLAN50",                              /* 1189 */
  "AEC_AECDBDISPREPWALLPLAN100",                              /* 1190 */
  "AEC_AECDBDISPREPWALLPLAN50",                               /* 1191 */
  "AEC_AECDBDISPREPWINDOWASSEMBLYPLAN100",                    /* 1192 */
  "AEC_AECDBDISPREPWINDOWASSEMBLYPLAN50",                     /* 1193 */
  "AEC_AECDBDISPREPZONE100",                                  /* 1194 */
  "AEC_AECDBDISPREPZONE50",                                   /* 1195 */
  "AEC_AECDBZONEDEF",                                         /* 1196 */
  "AEC_AECDBZONESTYLE",                                       /* 1197 */
  "AEC_ANCHOR_OPENINGBASE_TO_WALL",                           /* 1198 */
  "AEC_CLASSIFICATION_DEF",                                   /* 1199 */
  "AEC_CLASSIFICATION_SYSTEM_DEF",                            /* 1200 */
  "AEC_CLEANUP_GROUP_DEF",                                    /* 1201 */
  "AEC_CURTAIN_WALL_LAYOUT_STYLE",                            /* 1202 */
  "AEC_CURTAIN_WALL_UNIT_STYLE",                              /* 1203 */
  "AEC_CVSECTIONVIEW",                                        /* 1204 */
  "AEC_DB_DISP_REP_DIM_GROUP_PLAN",                           /* 1205 */
  "AEC_DB_DISP_REP_DIM_GROUP_PLAN100",                        /* 1206 */
  "AEC_DB_DISP_REP_DIM_GROUP_PLAN50",                         /* 1207 */
  "AEC_DIM_STYLE",                                            /* 1208 */
  "AEC_DISPLAYTHEME_STYLE",                                   /* 1209 */
  "AEC_DISP_PROPS_2D_SECTION",                                /* 1210 */
  "AEC_DISP_PROPS_CLIP_VOLUME",                               /* 1211 */
  "AEC_DISP_PROPS_CLIP_VOLUME_RESULT",                        /* 1212 */
  "AEC_DISP_PROPS_DIM",                                       /* 1213 */
  "AEC_DISP_PROPS_DISPLAYTHEME",                              /* 1214 */
  "AEC_DISP_PROPS_DOOR",                                      /* 1215 */
  "AEC_DISP_PROPS_DOOR_NOMINAL",                              /* 1216 */
  "AEC_DISP_PROPS_DOOR_PLAN_100",                             /* 1217 */
  "AEC_DISP_PROPS_DOOR_PLAN_50",                              /* 1218 */
  "AEC_DISP_PROPS_DOOR_THRESHOLD_PLAN",                       /* 1219 */
  "AEC_DISP_PROPS_DOOR_THRESHOLD_SYMBOL_PLAN",                /* 1220 */
  "AEC_DISP_PROPS_EDITINPLACEPROFILE_MODEL",                  /* 1221 */
  "AEC_DISP_PROPS_ENT",                                       /* 1222 */
  "AEC_DISP_PROPS_ENT_REF",                                   /* 1223 */
  "AEC_DISP_PROPS_GRID_ASSEMBLY_MODEL",                       /* 1224 */
  "AEC_DISP_PROPS_GRID_ASSEMBLY_PLAN",                        /* 1225 */
  "AEC_DISP_PROPS_LAYOUT_CURVE",                              /* 1226 */
  "AEC_DISP_PROPS_LAYOUT_GRID2D",                             /* 1227 */
  "AEC_DISP_PROPS_LAYOUT_GRID3D",                             /* 1228 */
  "AEC_DISP_PROPS_MASKBLOCK",                                 /* 1229 */
  "AEC_DISP_PROPS_MASS_ELEM_MODEL",                           /* 1230 */
  "AEC_DISP_PROPS_MASS_GROUP",                                /* 1231 */
  "AEC_DISP_PROPS_MATERIAL",                                  /* 1232 */
  "AEC_DISP_PROPS_OPENING",                                   /* 1233 */
  "AEC_DISP_PROPS_POLYGON_MODEL",                             /* 1234 */
  "AEC_DISP_PROPS_POLYGON_TRUECOLOUR",                        /* 1235 */
  "AEC_DISP_PROPS_RAILING_MODEL",                             /* 1236 */
  "AEC_DISP_PROPS_RAILING_PLAN",                              /* 1237 */
  "AEC_DISP_PROPS_ROOF",                                      /* 1238 */
  "AEC_DISP_PROPS_ROOFSLAB",                                  /* 1239 */
  "AEC_DISP_PROPS_ROOFSLAB_PLAN",                             /* 1240 */
  "AEC_DISP_PROPS_SCHEDULE_TABLE",                            /* 1241 */
  "AEC_DISP_PROPS_SLAB",                                      /* 1242 */
  "AEC_DISP_PROPS_SLAB_PLAN",                                 /* 1243 */
  "AEC_DISP_PROPS_SLICE",                                     /* 1244 */
  "AEC_DISP_PROPS_SPACE_DECOMPOSED",                          /* 1245 */
  "AEC_DISP_PROPS_SPACE_MODEL",                               /* 1246 */
  "AEC_DISP_PROPS_SPACE_PLAN",                                /* 1247 */
  "AEC_DISP_PROPS_STAIR_MODEL",                               /* 1248 */
  "AEC_DISP_PROPS_STAIR_PLAN",                                /* 1249 */
  "AEC_DISP_PROPS_STAIR_PLAN_OVERLAPPING",                    /* 1250 */
  "AEC_DISP_PROPS_WALL_GRAPH",                                /* 1251 */
  "AEC_DISP_PROPS_WALL_MODEL",                                /* 1252 */
  "AEC_DISP_PROPS_WALL_PLAN",                                 /* 1253 */
  "AEC_DISP_PROPS_WALL_SCHEM",                                /* 1254 */
  "AEC_DISP_PROPS_WINDOW",                                    /* 1255 */
  "AEC_DISP_PROPS_WINDOW_ASSEMBLY_SILL_PLAN",                 /* 1256 */
  "AEC_DISP_PROPS_WINDOW_NOMINAL",                            /* 1257 */
  "AEC_DISP_PROPS_WINDOW_PLAN_100",                           /* 1258 */
  "AEC_DISP_PROPS_WINDOW_PLAN_50",                            /* 1259 */
  "AEC_DISP_PROPS_WINDOW_SILL_PLAN",                          /* 1260 */
  "AEC_DISP_PROPS_ZONE",                                      /* 1261 */
  "AEC_DISP_REP_2D_SECTION",                                  /* 1262 */
  "AEC_DISPREPAECDBDISPREPMASSELEMPLAN100",                   /* 1263 */
  "AEC_DISPREPAECDBDISPREPMASSELEMPLAN50",                    /* 1264 */
  "AEC_DISPREPAECDBDISPREPMASSGROUPPLAN100",                  /* 1265 */
  "AEC_DISPREPAECDBDISPREPMASSGROUPPLAN50",                   /* 1266 */
  "AEC_DISPREPAECDBDISPREPOPENINGPLAN100",                    /* 1267 */
  "AEC_DISPREPAECDBDISPREPOPENINGPLAN50",                     /* 1268 */
  "AEC_DISPREPAECDBDISPREPOPENINGPLANREFLECTED",              /* 1269 */
  "AEC_DISPREPAECDBDISPREPOPENINGSILLPLAN",                   /* 1270 */
  "AEC_DISP_REP_ANCHOR",                                      /* 1271 */
  "AEC_DISP_REP_ANCHOR_BUB_TO_GRID",                          /* 1272 */
  "AEC_DISP_REP_ANCHOR_BUB_TO_GRID_MODEL",                    /* 1273 */
  "AEC_DISP_REP_ANCHOR_BUB_TO_GRID_RCP",                      /* 1274 */
  "AEC_DISP_REP_ANCHOR_ENT_TO_NODE",                          /* 1275 */
  "AEC_DISP_REP_ANCHOR_EXT_TAG_TO_ENT",                       /* 1276 */
  "AEC_DISP_REP_ANCHOR_TAG_TO_ENT",                           /* 1277 */
  "AEC_DISP_REP_BDG_ELEVLINE_MODEL",                          /* 1278 */
  "AEC_DISP_REP_BDG_ELEVLINE_PLAN",                           /* 1279 */
  "AEC_DISP_REP_BDG_ELEVLINE_RCP",                            /* 1280 */
  "AEC_DISP_REP_BDG_SECTIONLINE_MODEL",                       /* 1281 */
  "AEC_DISP_REP_BDG_SECTIONLINE_PLAN",                        /* 1282 */
  "AEC_DISP_REP_BDG_SECTIONLINE_RCP",                         /* 1283 */
  "AEC_DISP_REP_BDG_SECTION_MODEL",                           /* 1284 */
  "AEC_DISP_REP_BDG_SECTION_SUBDIV",                          /* 1285 */
  "AEC_DISP_REP_CEILING_GRID",                                /* 1286 */
  "AEC_DISP_REP_CEILING_GRID_MODEL",                          /* 1287 */
  "AEC_DISP_REP_CEILING_GRID_RCP",                            /* 1288 */
  "AEC_DISP_REP_CLIP_VOLUME_MODEL",                           /* 1289 */
  "AEC_DISP_REP_CLIP_VOLUME_PLAN",                            /* 1290 */
  "AEC_DISP_REP_CLIP_VOLUME_RESULT",                          /* 1291 */
  "AEC_DISP_REP_CLIP_VOLUME_RESULT_SUBDIV",                   /* 1292 */
  "AEC_DISP_REP_COL_BLOCK",                                   /* 1293 */
  "AEC_DISP_REP_COL_CIRCARC2D",                               /* 1294 */
  "AEC_DISP_REP_COL_CONCOINCIDENT",                           /* 1295 */
  "AEC_DISP_REP_COL_CONCONCENTRIC",                           /* 1296 */
  "AEC_DISP_REP_COL_CONEQUALDISTANCE",                        /* 1297 */
  "AEC_DISP_REP_COL_CONMIDPOINT",                             /* 1298 */
  "AEC_DISP_REP_COL_CONNECTOR",                               /* 1299 */
  "AEC_DISP_REP_COL_CONNORMAL",                               /* 1300 */
  "AEC_DISP_REP_COL_CONPARALLEL",                             /* 1301 */
  "AEC_DISP_REP_COL_CONPERPENDICULAR",                        /* 1302 */
  "AEC_DISP_REP_COL_CONSYMMETRIC",                            /* 1303 */
  "AEC_DISP_REP_COL_CONTANGENT",                              /* 1304 */
  "AEC_DISP_REP_COL_DIMANGLE",                                /* 1305 */
  "AEC_DISP_REP_COL_DIMDIAMETER",                             /* 1306 */
  "AEC_DISP_REP_COL_DIMDISTANCE",                             /* 1307 */
  "AEC_DISP_REP_COL_DIMLENGTH",                               /* 1308 */
  "AEC_DISP_REP_COL_DIMMAJORRADIUS",                          /* 1309 */
  "AEC_DISP_REP_COL_DIMMINORRADIUS",                          /* 1310 */
  "AEC_DISP_REP_COL_ELLIPARC2D",                              /* 1311 */
  "AEC_DISP_REP_COL_LAYOUTDATA",                              /* 1312 */
  "AEC_DISP_REP_COL_LINE2D",                                  /* 1313 */
  "AEC_DISP_REP_COL_MODIFIER_ADD",                            /* 1314 */
  "AEC_DISP_REP_COL_MODIFIER_CUTPLANE",                       /* 1315 */
  "AEC_DISP_REP_COL_MODIFIER_EXTRUSION",                      /* 1316 */
  "AEC_DISP_REP_COL_MODIFIER_GROUP",                          /* 1317 */
  "AEC_DISP_REP_COL_MODIFIER_LOFT",                           /* 1318 */
  "AEC_DISP_REP_COL_MODIFIER_PATH",                           /* 1319 */
  "AEC_DISP_REP_COL_MODIFIER_REVOLVE",                        /* 1320 */
  "AEC_DISP_REP_COL_MODIFIER_SUBTRACT",                       /* 1321 */
  "AEC_DISP_REP_COL_MODIFIER_TRANSITION",                     /* 1322 */
  "AEC_DISP_REP_COL_POINT2D",                                 /* 1323 */
  "AEC_DISP_REP_COL_PROFILE",                                 /* 1324 */
  "AEC_DISP_REP_COLUMN_GRID",                                 /* 1325 */
  "AEC_DISP_REP_COLUMN_GRID_MODEL",                           /* 1326 */
  "AEC_DISP_REP_COLUMN_GRID_RCP",                             /* 1327 */
  "AEC_DISP_REP_COL_WORKPLANE",                               /* 1328 */
  "AEC_DISP_REP_COL_WORKPLANE_REF",                           /* 1329 */
  "AEC_DISP_REP_CONFIG",                                      /* 1330 */
  "AEC_DISP_REP_CURTAIN_WALL_LAYOUT_MODEL",                   /* 1331 */
  "AEC_DISP_REP_CURTAIN_WALL_LAYOUT_PLAN",                    /* 1332 */
  "AEC_DISP_REP_CURTAIN_WALL_UNIT_MODEL",                     /* 1333 */
  "AEC_DISP_REP_CURTAIN_WALL_UNIT_PLAN",                      /* 1334 */
  "AEC_DISP_REP_DCM_DIMRADIUS",                               /* 1335 */
  "AEC_DISP_REP_DISPLAYTHEME",                                /* 1336 */
  "AEC_DISP_REP_DOOR_ELEV",                                   /* 1337 */
  "AEC_DISP_REP_DOOR_MODEL",                                  /* 1338 */
  "AEC_DISP_REP_DOOR_NOMINAL",                                /* 1339 */
  "AEC_DISP_REP_DOOR_PLAN",                                   /* 1340 */
  "AEC_DISP_REP_DOOR_PLAN_50",                                /* 1341 */
  "AEC_DISP_REP_DOOR_PLAN_HEKTO",                             /* 1342 */
  "AEC_DISP_REP_DOOR_RCP",                                    /* 1343 */
  "AEC_DISP_REP_DOOR_THRESHOLD_PLAN",                         /* 1344 */
  "AEC_DISP_REP_DOOR_THRESHOLD_SYMBOL_PLAN",                  /* 1345 */
  "AEC_DISP_REP_EDITINPLACEPROFILE",                          /* 1346 */
  "AEC_DISP_REP_ENT_REF",                                     /* 1347 */
  "AEC_DISP_REP_LAYOUT_CURVE",                                /* 1348 */
  "AEC_DISP_REP_LAYOUT_GRID2D",                               /* 1349 */
  "AEC_DISP_REP_LAYOUT_GRID3D",                               /* 1350 */
  "AEC_DISP_REP_MASKBLOCK_REF",                               /* 1351 */
  "AEC_DISP_REP_MASKBLOCK_REF_RCP",                           /* 1352 */
  "AEC_DISP_REP_MASS_ELEM_MODEL",                             /* 1353 */
  "AEC_DISP_REP_MASS_ELEM_RCP",                               /* 1354 */
  "AEC_DISP_REP_MASS_ELEM_SCHEM",                             /* 1355 */
  "AEC_DISP_REP_MASS_GROUP_MODEL",                            /* 1356 */
  "AEC_DISP_REP_MASS_GROUP_PLAN",                             /* 1357 */
  "AEC_DISP_REP_MASS_GROUP_RCP",                              /* 1358 */
  "AEC_DISP_REP_MATERIAL",                                    /* 1359 */
  "AEC_DISP_REP_MVBLOCK_REF",                                 /* 1360 */
  "AEC_DISP_REP_MVBLOCK_REF_MODEL",                           /* 1361 */
  "AEC_DISP_REP_MVBLOCK_REF_RCP",                             /* 1362 */
  "AEC_DISP_REP_OPENING",                                     /* 1363 */
  "AEC_DISP_REP_OPENING_MODEL",                               /* 1364 */
  "AEC_DISP_REP_POLYGON_MODEL",                               /* 1365 */
  "AEC_DISP_REP_POLYGON_TRUECOLOUR",                          /* 1366 */
  "AEC_DISP_REP_RAILING_MODEL",                               /* 1367 */
  "AEC_DISP_REP_RAILING_PLAN",                                /* 1368 */
  "AEC_DISP_REP_RAILING_PLAN_100",                            /* 1369 */
  "AEC_DISP_REP_RAILING_PLAN_50",                             /* 1370 */
  "AEC_DISP_REP_ROOF_MODEL",                                  /* 1371 */
  "AEC_DISP_REP_ROOF_PLAN",                                   /* 1372 */
  "AEC_DISP_REP_ROOF_RCP",                                    /* 1373 */
  "AEC_DISP_REP_ROOFSLAB_MODEL",                              /* 1374 */
  "AEC_DISP_REP_ROOFSLAB_PLAN",                               /* 1375 */
  "AEC_DISP_REP_SCHEDULE_TABLE",                              /* 1376 */
  "AEC_DISP_REP_SET",                                         /* 1377 */
  "AEC_DISP_REP_SLAB_MODEL",                                  /* 1378 */
  "AEC_DISP_REP_SLAB_PLAN",                                   /* 1379 */
  "AEC_DISP_REP_SLICE",                                       /* 1380 */
  "AEC_DISP_REP_SPACE_DECOMPOSED",                            /* 1381 */
  "AEC_DISP_REP_SPACE_MODEL",                                 /* 1382 */
  "AEC_DISP_REP_SPACE_PLAN",                                  /* 1383 */
  "AEC_DISP_REP_SPACE_RCP",                                   /* 1384 */
  "AEC_DISP_REP_SPACE_VOLUME",                                /* 1385 */
  "AEC_DISP_REP_STAIR_MODEL",                                 /* 1386 */
  "AEC_DISP_REP_STAIR_PLAN",                                  /* 1387 */
  "AEC_DISP_REP_STAIR_PLAN_100",                              /* 1388 */
  "AEC_DISP_REP_STAIR_PLAN_50",                               /* 1389 */
  "AEC_DISP_REP_STAIR_PLAN_OVERLAPPING",                      /* 1390 */
  "AEC_DISP_REP_STAIR_RCP",                                   /* 1391 */
  "AEC_DISP_REP_WALL_GRAPH",                                  /* 1392 */
  "AEC_DISP_REP_WALL_MODEL",                                  /* 1393 */
  "AEC_DISP_REP_WALL_PLAN",                                   /* 1394 */
  "AEC_DISP_REP_WALL_RCP",                                    /* 1395 */
  "AEC_DISP_REP_WALL_SCHEM",                                  /* 1396 */
  "AEC_DISP_REP_WINDOW_ASSEMBLY_MODEL",                       /* 1397 */
  "AEC_DISP_REP_WINDOW_ASSEMBLY_PLAN",                        /* 1398 */
  "AEC_DISP_REP_WINDOWASSEMBLY_SILL_PLAN",                    /* 1399 */
  "AEC_DISP_REP_WINDOW_ELEV",                                 /* 1400 */
  "AEC_DISP_REP_WINDOW_MODEL",                                /* 1401 */
  "AEC_DISP_REP_WINDOW_NOMINAL",                              /* 1402 */
  "AEC_DISP_REP_WINDOW_PLAN",                                 /* 1403 */
  "AEC_DISP_REP_WINDOW_PLAN_100",                             /* 1404 */
  "AEC_DISP_REP_WINDOW_PLAN_50",                              /* 1405 */
  "AEC_DISP_REP_WINDOW_RCP",                                  /* 1406 */
  "AEC_DISP_REP_WINDOW_SILL_PLAN",                            /* 1407 */
  "AEC_DISP_REP_ZONE",                                        /* 1408 */
  "AEC_DISPROPSMASSELEMPLANCOMMON",                           /* 1409 */
  "AEC_DISPROPSMASSGROUPPLANCOMMON",                          /* 1410 */
  "AEC_DISPROPSOPENINGPLANCOMMON",                            /* 1411 */
  "AEC_DISPROPSOPENINGPLANCOMMONHATCHED",                     /* 1412 */
  "AEC_DISPROPSOPENINGSILLPLAN",                              /* 1413 */
  "AEC_DISP_ROPS_RAILING_PLAN_100",                           /* 1414 */
  "AEC_DISP_ROPS_RAILING_PLAN_50",                            /* 1415 */
  "AEC_DISP_ROPS_STAIR_PLAN_100",                             /* 1416 */
  "AEC_DISP_ROPS_STAIR_PLAN_50",                              /* 1417 */
  "AEC_DOOR_STYLE",                                           /* 1418 */
  "AEC_ENDCAP_STYLE",                                         /* 1419 */
  "AEC_FRAME_DEF",                                            /* 1420 */
  "AEC_LAYERKEY_STYLE",                                       /* 1421 */
  "AEC_LIST_DEF",                                             /* 1422 */
  "AEC_MASKBLOCK_DEF",                                        /* 1423 */
  "AEC_MASS_ELEM_STYLE",                                      /* 1424 */
  "AEC_MATERIAL_DEF",                                         /* 1425 */
  "AEC_MVBLOCK_DEF",                                          /* 1426 */
  "AEC_MVBLOCK_REF",                                          /* 1427 */
  "AEC_NOTIFICATION_TRACKER",                                 /* 1428 */
  "AEC_POLYGON",                                              /* 1429 */
  "AEC_POLYGON_STYLE",                                        /* 1430 */
  "AEC_PROPERTY_SET_DEF",                                     /* 1431 */
  "AEC_RAILING_STYLE",                                        /* 1432 */
  "AEC_REFEDIT_STATUS_TRACKER",                               /* 1433 */
  "AEC_ROOFSLABEDGE_STYLE",                                   /* 1434 */
  "AEC_ROOFSLAB_STYLE",                                       /* 1435 */
  "AEC_SCHEDULE_DATA_FORMAT",                                 /* 1436 */
  "AEC_SLABEDGE_STYLE",                                       /* 1437 */
  "AEC_SLAB_STYLE",                                           /* 1438 */
  "AEC_SPACE_STYLES",                                         /* 1439 */
  "AEC_STAIR_STYLE",                                          /* 1440 */
  "AEC_STAIR_WINDER_STYLE",                                   /* 1441 */
  "AEC_STAIR_WINDER_TYPE_BALANCED",                           /* 1442 */
  "AEC_STAIR_WINDER_TYPE_MANUAL",                             /* 1443 */
  "AEC_STAIR_WINDER_TYPE_SINGLE_POINT",                       /* 1444 */
  "AEC_VARS_AECBBLDSRV",                                      /* 1445 */
  "AEC_VARS_ARCHBASE",                                        /* 1446 */
  "AEC_VARS_DWG_SETUP",                                       /* 1447 */
  "AEC_VARS_MUNICH",                                          /* 1448 */
  "AEC_VARS_STRUCTUREBASE",                                   /* 1449 */
  "AEC_WALLMOD_STYLE",                                        /* 1450 */
  "AEC_WALL_STYLE",                                           /* 1451 */
  "AEC_WINDOW_ASSEMBLY_STYLE",                                /* 1452 */
  "AEC_WINDOW_STYLE",                                         /* 1453 */
  "ALIGNMENTGRIPENTITY",                                      /* 1454 */
  "AMCONTEXTMGR",                                             /* 1455 */
  "AMDTADMENUSTATE",                                          /* 1456 */
  "AMDTAMMENUSTATE",                                          /* 1457 */
  "AMDTBROWSERDBTAB",                                         /* 1458 */
  "AMDTDMMENUSTATE",                                          /* 1459 */
  "AMDTEDGESTANDARDDIN",                                      /* 1460 */
  "AMDTEDGESTANDARDDIN13715",                                 /* 1461 */
  "AMDTEDGESTANDARDISO",                                      /* 1462 */
  "AMDTEDGESTANDARDISO13715",                                 /* 1463 */
  "AMDTFORMULAUPDATEDISPATCHER",                              /* 1464 */
  "AMDTINTERNALREACTOR",                                      /* 1465 */
  "AMDTMCOMMENUSTATE",                                        /* 1466 */
  "AMDTMENUSTATEMGR",                                         /* 1467 */
  "AMDTNOTE",                                                 /* 1468 */
  "AMDTNOTETEMPLATEDB",                                       /* 1469 */
  "AMDTSECTIONSYM",                                           /* 1470 */
  "AMDTSECTIONSYMLABEL",                                      /* 1471 */
  "AMDTSYSATTR",                                              /* 1472 */
  "AMGOBJPROPCFG",                                            /* 1473 */
  "AMGSETTINGSOBJ",                                           /* 1474 */
  "AMIMASTER",                                                /* 1475 */
  "AM_DRAWING_MGR",                                           /* 1476 */
  "AM_DWGMGR_NAME",                                           /* 1477 */
  "AM_DWG_DOCUMENT",                                          /* 1478 */
  "AM_DWG_SHEET",                                             /* 1479 */
  "AM_VIEWDIMPARMAP",                                         /* 1480 */
  "BINRECORD",                                                /* 1481 */
  "CAMSCATALOGAPPOBJECT",                                     /* 1482 */
  "CAMSSTRUCTBTNSTATE",                                       /* 1483 */
  "CATALOGSTATE",                                             /* 1484 */
  "CBROWSERAPPOBJECT",                                        /* 1485 */
  "DEPMGR",                                                   /* 1486 */
  "DMBASEELEMENT",                                            /* 1487 */
  "DMDEFAULTSTYLE",                                           /* 1488 */
  "DMLEGEND",                                                 /* 1489 */
  "DMMAP",                                                    /* 1490 */
  "DMMAPMANAGER",                                             /* 1491 */
  "DMSTYLECATEGORY",                                          /* 1492 */
  "DMSTYLELIBRARY",                                           /* 1493 */
  "DMSTYLEREFERENCE",                                         /* 1494 */
  "DMSTYLIZEDENTITIESTABLE",                                  /* 1495 */
  "DMSURROGATESTYLESETS",                                     /* 1496 */
  "DM_PLACEHOLDER",                                           /* 1497 */
  "EXACTERMXREFMAP",                                          /* 1498 */
  "EXACXREFPANELOBJECT",                                      /* 1499 */
  "EXPO_NOTIFYBLOCK",                                         /* 1500 */
  "EXPO_NOTIFYHALL",                                          /* 1501 */
  "EXPO_NOTIFYPILLAR",                                        /* 1502 */
  "EXPO_NOTIFYSTAND",                                         /* 1503 */
  "EXPO_NOTIFYSTANDNOPOLY",                                   /* 1504 */
  "GSMANAGER",                                                /* 1505 */
  "IRD_DSC_DICT",                                             /* 1506 */
  "IRD_DSC_RECORD",                                           /* 1507 */
  "IRD_OBJ_RECORD",                                           /* 1508 */
  "MAPFSMRVOBJECT",                                           /* 1509 */
  "MAPGWSUNDOOBJECT",                                         /* 1510 */
  "MAPIAMMOUDLE",                                             /* 1511 */
  "MAPMETADATAOBJECT",                                        /* 1512 */
  "MAPRESOURCEMANAGEROBJECT",                                 /* 1513 */
  "McDbContainer2",                                           /* 1514 */
  "McDbMarker",                                               /* 1515 */
  "NAMEDAPPL",                                                /* 1516 */
  "NEWSTDPARTPARLIST",                                        /* 1517 */
  "NOTEPOSITION",                                             /* 1518 */
  "OBJCLONER",                                                /* 1519 */
  "PARAMMGR",                                                 /* 1520 */
  "PARAMSCOPE",                                               /* 1521 */
  "PILLAR",                                                   /* 1522 */
  "STDPART2D",                                                /* 1523 */
  "TCH_ARROW",                                                /* 1524 */
  "TCH_AXIS_LABEL",                                           /* 1525 */
  "TCH_BLOCK_INSERT",                                         /* 1526 */
  "TCH_COLUMN",                                               /* 1527 */
  "TCH_DBCONFIG",                                             /* 1528 */
  "TCH_DIMENSION2",                                           /* 1529 */
  "TCH_DRAWINGINDEX",                                         /* 1530 */
  "TCH_HANDRAIL",                                             /* 1531 */
  "TCH_LINESTAIR",                                            /* 1532 */
  "TCH_OPENING",                                              /* 1533 */
  "TCH_RECTSTAIR",                                            /* 1534 */
  "TCH_SLAB",                                                 /* 1535 */
  "TCH_SPACE",                                                /* 1536 */
  "TCH_TEXT",                                                 /* 1537 */
  "TCH_WALL",                                                 /* 1538 */
  "TGrupoPuntos",                                             /* 1539 */
  "VAACIMAGEINVENTORY",                                       /* 1540 */
  "VAACXREFPANELOBJECT",                                      /* 1541 */
  // clang-format: on
  /* End auto-generated variable */
};

// after 1.0 add new types here for binary compat
/*
  { DWG_TYPE_FREED      , "FREED      " }, // 0xfffd
  { DWG_TYPE_UNKNOWN_ENT, "UNKNOWN_ENT" }, // 0xfffe
  { DWG_TYPE_UNKNOWN_OBJ, "UNKNOWN_OBJ" }, // 0xffff
*/

/* Fast type -> name */
const char *
dwg_type_name (const Dwg_Object_Type type)
{
#if 1
  if (type <= DWG_TYPE_LAYOUT)
    return _dwg_type_names_fixed[type];
  else if ((unsigned)type >= 500
           && type < ARRAY_SIZE (_dwg_type_names_variable) + 500)
    return _dwg_type_names_variable[type - 500];
  else if (type == DWG_TYPE_UNKNOWN_ENT)
    return "UNKNOWN_ENT";
  else if (type == DWG_TYPE_UNKNOWN_OBJ)
    return "UNKNOWN_OBJ";
  else if (type == DWG_TYPE_PROXY_ENTITY)
    return "ACAD_PROXY_ENTITY";
  else if (type == DWG_TYPE_PROXY_OBJECT)
    return "ACAD_PROXY_OBJECT";
  else if (type == DWG_TYPE_FREED)
    return "FREED";
  else
    return "INVALID";
#else
  const struct _obj_type_name *s = (struct _obj_type_name *)_dwg_type_name;
  // just 12 unhandled are missing in objects.inc
  for (; s->type != DWG_TYPE_UNKNOWN_OBJ; s++)
    {
      if (type == s->type)
        return s->name;
    }
  return "INVALID";
#endif
}

// gperf lookup: name -> type, dxfname
Dwg_Object_Type
dwg_name_type (const char *name)
{
#if 1
  Dwg_Object_Type type;
  if (dwg_object_name (name, NULL, &type, NULL, NULL))
    return type;
  else
    return DWG_TYPE_UNUSED;
#else
  struct _obj_type_name *s = (struct _obj_type_name *)_dwg_type_name;
  for (; s->type != DWG_TYPE_UNKNOWN_OBJ; s++)
    {
      if (strEQ (name, s->name))
        return s->type;
    }
  return DWG_TYPE_UNUSED;
#endif
}

bool
is_type_stable (const Dwg_Object_Type type)
{
  // clang-format off
  return !is_type_unstable (type)
      && !is_type_debugging (type)
      && !is_type_unstable (type)
      && type != DWG_TYPE_UNKNOWN_OBJ
      && type != DWG_TYPE_UNKNOWN_ENT
      && type != DWG_TYPE_FREED;
  // clang-format on
}

bool
is_type_unstable_all (const Dwg_Object_Type type)
{
  return is_type_unstable (type) || is_type_debugging (type);
}

bool
is_type_unstable (const Dwg_Object_Type type)
{
#if 1
  return is_class_unstable (dwg_type_name (type));
#else
  // unsorted, only linear
  for (int i = 0; i < ARRAY_SIZE (_classes_unstable); i++)
    {
      if (type == _classes_unstable[i])
        return true;
    }
#endif
  return false;
}
bool
is_type_debugging (const Dwg_Object_Type type)
{
#if 1
  return is_class_debugging (dwg_type_name (type));
#else
  // unsorted, only linear
  for (int i = 0; i < ARRAY_SIZE (_classes_debugging); i++)
    {
      if (type == _classes_debugging[i])
        return true;
    }
#endif
  return false;
}
bool
is_type_unhandled (const Dwg_Object_Type type)
{
#if 1
  return is_class_unhandled (dwg_type_name (type));
#else
  // unsorted, only linear
  for (int i = 0; i < ARRAY_SIZE (_classes_unhandled); i++)
    {
      if (type == _classes_unhandled[i])
        return true;
    }
#endif
  return false;
}

bool
is_class_stable (const char *name)
{
#if 1
  // hash lookup
  Dwg_Class_Stability stable;
  if (dwg_object_name (name, NULL, NULL, NULL, &stable))
    return stable == DWG_CLASS_STABLE;
  else
    return false;
#else
  return is_type_stable (dwg_name_type (name));
#endif
}
bool
is_class_unstable (const char *name)
{
#if 1
  // hash lookup
  Dwg_Class_Stability stable;
  if (dwg_object_name (name, NULL, NULL, NULL, &stable))
    return stable == DWG_CLASS_UNSTABLE;
  else
    return false;
#else
  return is_type_unstable (dwg_name_type (name));
#endif
}
bool
is_class_debugging (const char *name)
{
#if 1
  // hash lookup
  Dwg_Class_Stability stable;
  if (dwg_object_name (name, NULL, NULL, NULL, &stable))
    return stable == DWG_CLASS_DEBUGGING;
  else
    return false;
#else
  return is_type_debugging (dwg_name_type (name));
#endif
}
bool
is_class_unhandled (const char *name)
{
#if 1
  // hash lookup
  Dwg_Class_Stability stable;
  if (dwg_object_name (name, NULL, NULL, NULL, &stable))
    return stable == DWG_CLASS_UNHANDLED;
  else
    return true;
#else
  return is_type_unhandled (dwg_name_type (name));
#endif
}

// if all DXF groups are known
bool
is_dxf_class_importable (const char *name)
{
  // hash lookup
  Dwg_Class_Stability stability;
  if (dwg_object_name (name, NULL, NULL, NULL, &stability))
    {
      if (stability == DWG_CLASS_UNSTABLE)
        {
          return !strEQc (name, "MATERIAL") &&      // 72 missing
                 !strEQc (name, "ARC_DIMENSION") && // 2 missing
                 !strEQc (name, "SUN") &&           // 421 missing
                 !strEQc (name, "PROXY_ENTITY") &&  // 90 missing
                 !strEQc (name, "PROXY_OBJECT");    // 90 missing
        }
      else
        return stability == DWG_CLASS_STABLE;
    }
  else
    return false;
}

bool
dwg_find_class (const Dwg_Data *restrict dwg, const char *dxfname,
                BITCODE_BS *numberp)
{
  assert (dwg);
  assert (dxfname);
  // linear search is good enough, with ~20 classes
  for (BITCODE_BS i = 0; i < dwg->num_classes; i++)
    {
      // ->dxfname is always ASCII/UTF-8, dxfname_u is the TU counterpart
      if (dwg->dwg_class[i].dxfname
          && strEQ (dwg->dwg_class[i].dxfname, dxfname))
        {
          if (numberp)
            *numberp = dwg->dwg_class[i].number;
          return true;
        }
    }
  return false;
}

// Similar to _dwg_type_name, just with immediate access, no linear/binary
// search.
static const char *_dwg_dxfnames_fixed[] = {
  NULL,               /* UNUSED = 0x00, */
  "TEXT",             /* 0x01 */
  "ATTRIB",           /* 0x02 */
  "ATTDEF",           /* 0x03, */
  "BLOCK",            /* 0x04, */
  "ENDBLK",           /* 0x05, */
  "SEQEND",           /* 0x06, */
  "INSERT",           /* 0x07, */
  "INSERT",           /* MINSERT 0x08, */
  "TRACE",            /* TRACE_old = 0x09, old TRACE r10-r11 only */
  "VERTEX",           /* _2D 0x0a, */
  "VERTEX",           /* _3D 0x0b, */
  "VERTEX",           /* _MESH 0x0c, */
  "VERTEX",           /* _PFACE 0x0d, */
  "VERTEX",           /* _PFACE_FACE 0x0e, */
  "POLYLINE",         /* _2D 0x0f, */
  "POLYLINE",         /* _3D 0x10, */
  "ARC",              /* 0x11, */
  "CIRCLE",           /* 0x12, */
  "LINE",             /* 0x13, */
  "DIMENSION",        /* _ORDINATE 0x14, */
  "DIMENSION",        /* _LINEAR 0x15, */
  "DIMENSION",        /* _ALIGNED 0x16, */
  "DIMENSION",        /* _ANG3PT 0x17, */
  "DIMENSION",        /* _ANG2LN 0x18, */
  "DIMENSION",        /* _RADIUS 0x19, */
  "DIMENSION",        /* _DIAMETER 0x1A, */
  "POINT",            /* 0x1b, */
  "3DFACE",           /* 0x1c, */
  "POLYLINE",         /* POLYLINE_PFACE 0x1d, */
  "POLYLINE",         /* POLYLINE_MESH 0x1e, */
  "SOLID",            /* 0x1f, */
  "TRACE",            /* 0x20, */
  "SHAPE",            /* 0x21, */
  "VIEWPORT",         /* 0x22, */
  "ELLIPSE",          /* 0x23, */
  "SPLINE",           /* 0x24, */
  "REGION",           /* 0x25, */
  "3DSOLID",          /* 0x26, */
  "BODY",             /* 0x27, */
  "RAY",              /* 0x28, */
  "XLINE",            /* 0x29, */
  "DICTIONARY",       /* 0x2a, */
  "OLEFRAME",         /* 0x2b, */
  "MTEXT",            /* 0x2c, */
  "LEADER",           /* 0x2d, */
  "TOLERANCE",        /* 0x2e, */
  "MLINE",            /* 0x2f, */
  "BLOCK_CONTROL",    /* 0x30, */
  "BLOCK_HEADER",     /* 0x31, */
  "LAYER_CONTROL",    /* 0x32, */
  "LAYER",            /* 0x33, */
  "STYLE_CONTROL",    /* 0x34, 52 SHAPEFILE_CONTROL */
  "STYLE",            /* 0x35, */
  NULL,               /* DWG_TYPE_<UNKNOWN> = 0x36, */
  NULL,               /* DWG_TYPE_<UNKNOWN> = 0x37, */
  "LTYPE_CONTROL",    /* 0x38, */
  "LTYPE",            /* 0x39, */
  NULL,               /* DWG_TYPE_<UNKNOWN> = 0x3a, */
  NULL,               /* DWG_TYPE_<UNKNOWN> = 0x3b, */
  "VIEW_CONTROL",     /* 0x3c, */
  "VIEW",             /* 0x3d, */
  "UCS_CONTROL",      /* 0x3e, */
  "UCS",              /* 0x3f, */
  "VPORT_CONTROL",    /* 0x40, */
  "VPORT",            /* 0x41, */
  "APPID_CONTROL",    /* 0x42, */
  "APPID",            /* 0x43, */
  "DIMSTYLE_CONTROL", /* 0x44, */
  "DIMSTYLE",         /* 0x45, */
  "VX_CONTROL",       /* 0x46, */
  "VX_TABLE_RECORD",  /* 0x47, */
  "GROUP",            /* 0x48, */
  "MLINESTYLE",       /* 0x49, */
  "OLE2FRAME",        /* 0x4a, */
  "DUMMY",            /* 0x4b, */
  "LONG_TRANSACTION", /* 0x4c, */
  "LWPOLYLINE",       /* 0x4d */
  "HATCH",            /* 0x4e, */
  "XRECORD",          /* 0x4f, */
  "ACDBPLACEHOLDER",  /* 0x50, */
  "VBA_PROJECT",      /* 0x51, */
  "LAYOUT"            /* 0x52 */
};

/* Non-fixed types > 500. Not stored as type, but as fixedtype. */
static const char *_dwg_dxfnames_variable[] = {
  /* Start auto-generated dxfnames. Do not modify */
  // clang-format: off
  "ACDSRECORD",                                               /* 500 */
  "ACDSSCHEMA",                                               /* 501 */
  "ACMECOMMANDHISTORY",                                       /* 502 */
  "ACMESCOPE",                                                /* 503 */
  "ACMESTATEMGR",                                             /* 504 */
  "ACSH_BOOLEAN_CLASS",                                       /* 505 */
  "ACSH_BOX_CLASS",                                           /* 506 */
  "ACSH_BREP_CLASS",                                          /* 507 */
  "ACSH_CHAMFER_CLASS",                                       /* 508 */
  "ACSH_CONE_CLASS",                                          /* 509 */
  "ACSH_CYLINDER_CLASS",                                      /* 510 */
  "ACSH_EXTRUSION_CLASS",                                     /* 511 */
  "ACSH_FILLET_CLASS",                                        /* 512 */
  "ACSH_HISTORY_CLASS",                                       /* 513 */
  "ACSH_LOFT_CLASS",                                          /* 514 */
  "ACSH_PYRAMID_CLASS",                                       /* 515 */
  "ACSH_REVOLVE_CLASS",                                       /* 516 */
  "ACSH_SPHERE_CLASS",                                        /* 517 */
  "ACSH_SWEEP_CLASS",                                         /* 518 */
  "ACSH_TORUS_CLASS",                                         /* 519 */
  "ACSH_WEDGE_CLASS",                                         /* 520 */
  "ACDB_ALDIMOBJECTCONTEXTDATA_CLASS",                        /* 521 */
  "ALIGNMENTPARAMETERENTITY",                                 /* 522 */
  "ACDB_ANGDIMOBJECTCONTEXTDATA_CLASS",                       /* 523 */
  "ACDB_ANNOTSCALEOBJECTCONTEXTDATA_CLASS",                   /* 524 */
  "ARCALIGNEDTEXT",                                           /* 525 */
  "ARC_DIMENSION",                                            /* 526 */
  "ACDBASSOC2DCONSTRAINTGROUP",                               /* 527 */
  "ACDBASSOC3POINTANGULARDIMACTIONBODY",                      /* 528 */
  "ACDBASSOCACTION",                                          /* 529 */
  "ACDBASSOCACTIONPARAM",                                     /* 530 */
  "ACDBASSOCALIGNEDDIMACTIONBODY",                            /* 531 */
  "ACDBASSOCARRAYACTIONBODY",                                 /* 532 */
  "ACDBASSOCARRAYMODIFYACTIONBODY",                           /* 533 */
  "ACDBASSOCARRAYMODIFYPARAMETERS",                           /* 534 */
  "ACDBASSOCARRAYPATHPARAMETERS",                             /* 535 */
  "ACDBASSOCARRAYPOLARPARAMETERS",                            /* 536 */
  "ACDBASSOCARRAYRECTANGULARPARAMETERS",                      /* 537 */
  "ACDBASSOCASMBODYACTIONPARAM",                              /* 538 */
  "ACDBASSOCBLENDSURFACEACTIONBODY",                          /* 539 */
  "ACDBASSOCCOMPOUNDACTIONPARAM",                             /* 540 */
  "ACDBASSOCDEPENDENCY",                                      /* 541 */
  "ACDBASSOCDIMDEPENDENCYBODY",                               /* 542 */
  "ACDBASSOCEDGEACTIONPARAM",                                 /* 543 */
  "ACDBASSOCEDGECHAMFERACTIONBODY",                           /* 544 */
  "ACDBASSOCEDGEFILLETACTIONBODY",                            /* 545 */
  "ACDBASSOCEXTENDSURFACEACTIONBODY",                         /* 546 */
  "ACDBASSOCEXTRUDEDSURFACEACTIONBODY",                       /* 547 */
  "ACDBASSOCFACEACTIONPARAM",                                 /* 548 */
  "ACDBASSOCFILLETSURFACEACTIONBODY",                         /* 549 */
  "ACDBASSOCGEOMDEPENDENCY",                                  /* 550 */
  "ACDBASSOCLOFTEDSURFACEACTIONBODY",                         /* 551 */
  "ACDBASSOCMLEADERACTIONBODY",                               /* 552 */
  "ACDBASSOCNETWORK",                                         /* 553 */
  "ACDBASSOCNETWORKSURFACEACTIONBODY",                        /* 554 */
  "ACDBASSOCOBJECTACTIONPARAM",                               /* 555 */
  "ACDBASSOCOFFSETSURFACEACTIONBODY",                         /* 556 */
  "ACDBASSOCORDINATEDIMACTIONBODY",                           /* 557 */
  "ACDBASSOCOSNAPPOINTREFACTIONPARAM",                        /* 558 */
  "ACDBASSOCPATCHSURFACEACTIONBODY",                          /* 559 */
  "ACDBASSOCPATHACTIONPARAM",                                 /* 560 */
  "ACDBASSOCPERSSUBENTMANAGER",                               /* 561 */
  "ACDBASSOCPLANESURFACEACTIONBODY",                          /* 562 */
  "ACDBASSOCPOINTREFACTIONPARAM",                             /* 563 */
  "ACDBASSOCRESTOREENTITYSTATEACTIONBODY",                    /* 564 */
  "ACDBASSOCREVOLVEDSURFACEACTIONBODY",                       /* 565 */
  "ACDBASSOCROTATEDDIMACTIONBODY",                            /* 566 */
  "ACDBASSOCSWEPTSURFACEACTIONBODY",                          /* 567 */
  "ACDBASSOCTRIMSURFACEACTIONBODY",                           /* 568 */
  "ACDBASSOCVALUEDEPENDENCY",                                 /* 569 */
  "ACDBASSOCVARIABLE",                                        /* 570 */
  "ACDBASSOCVERTEXACTIONPARAM",                               /* 571 */
  "BASEPOINTPARAMETERENTITY",                                 /* 572 */
  "ACDB_BLKREFOBJECTCONTEXTDATA_CLASS",                       /* 573 */
  "BLOCKALIGNEDCONSTRAINTPARAMETER",                          /* 574 */
  "BLOCKALIGNMENTGRIP",                                       /* 575 */
  "BLOCKALIGNMENTPARAMETER",                                  /* 576 */
  "BLOCKANGULARCONSTRAINTPARAMETER",                          /* 577 */
  "BLOCKARRAYACTION",                                         /* 578 */
  "BLOCKBASEPOINTPARAMETER",                                  /* 579 */
  "BLOCKDIAMETRICCONSTRAINTPARAMETER",                        /* 580 */
  "BLOCKFLIPACTION",                                          /* 581 */
  "BLOCKFLIPGRIP",                                            /* 582 */
  "BLOCKFLIPPARAMETER",                                       /* 583 */
  "BLOCKGRIPLOCATIONCOMPONENT",                               /* 584 */
  "BLOCKHORIZONTALCONSTRAINTPARAMETER",                       /* 585 */
  "BLOCKLINEARCONSTRAINTPARAMETER",                           /* 586 */
  "BLOCKLINEARGRIP",                                          /* 587 */
  "BLOCKLINEARPARAMETER",                                     /* 588 */
  "BLOCKLOOKUPACTION",                                        /* 589 */
  "BLOCKLOOKUPGRIP",                                          /* 590 */
  "BLOCKLOOKUPPARAMETER",                                     /* 591 */
  "BLOCKMOVEACTION",                                          /* 592 */
  "BLOCKPARAMDEPENDENCYBODY",                                 /* 593 */
  "BLOCKPOINTPARAMETER",                                      /* 594 */
  "BLOCKPOLARGRIP",                                           /* 595 */
  "BLOCKPOLARPARAMETER",                                      /* 596 */
  "BLOCKPOLARSTRETCHACTION",                                  /* 597 */
  "BLOCKPROPERTIESTABLE",                                     /* 598 */
  "BLOCKPROPERTIESTABLEGRIP",                                 /* 599 */
  "BLOCKRADIALCONSTRAINTPARAMETER",                           /* 600 */
  "ACDB_BLOCKREPRESENTATION_DATA",                            /* 601 */
  "BLOCKROTATEACTION",                                        /* 602 */
  "BLOCKROTATIONGRIP",                                        /* 603 */
  "BLOCKROTATIONPARAMETER",                                   /* 604 */
  "BLOCKSCALEACTION",                                         /* 605 */
  "BLOCKSTRETCHACTION",                                       /* 606 */
  "BLOCKUSERPARAMETER",                                       /* 607 */
  "BLOCKVERTICALCONSTRAINTPARAMETER",                         /* 608 */
  "BLOCKVISIBILITYGRIP",                                      /* 609 */
  "BLOCKVISIBILITYPARAMETER",                                 /* 610 */
  "BLOCKXYGRIP",                                              /* 611 */
  "BLOCKXYPARAMETER",                                         /* 612 */
  "CAMERA",                                                   /* 613 */
  "CELLSTYLEMAP",                                             /* 614 */
  "CONTEXTDATAMANAGER",                                       /* 615 */
  "CSACDOCUMENTOPTIONS",                                      /* 616 */
  "ACDBCURVEPATH",                                            /* 617 */
  "DATALINK",                                                 /* 618 */
  "DATATABLE",                                                /* 619 */
  "DBCOLOR",                                                  /* 620 */
  "ACDBDETAILVIEWSTYLE",                                      /* 621 */
  "DGNDEFINITION",                                            /* 622 */
  "DGNUNDERLAY",                                              /* 623 */
  "DICTIONARYVAR",                                            /* 624 */
  "ACDBDICTIONARYWDFLT",                                      /* 625 */
  "DIMASSOC",                                                 /* 626 */
  "ACDB_DMDIMOBJECTCONTEXTDATA_CLASS",                        /* 627 */
  "DWFDEFINITION",                                            /* 628 */
  "DWFUNDERLAY",                                              /* 629 */
  "ACAD_DYNAMICBLOCKPROXYNODE",                               /* 630 */
  "ACDB_DYNAMICBLOCKPURGEPREVENTER_VERSION",                  /* 631 */
  "ACAD_EVALUATION_GRAPH",                                    /* 632 */
  "EXTRUDEDSURFACE",                                          /* 633 */
  "ACDB_FCFOBJECTCONTEXTDATA_CLASS",                          /* 634 */
  "FIELD",                                                    /* 635 */
  "FIELDLIST",                                                /* 636 */
  "FLIPPARAMETERENTITY",                                      /* 637 */
  "FLIPACTIONENTITY",                                         /* 638 */
  "GEODATA",                                                  /* 639 */
  "GEOMAPIMAGE",                                              /* 640 */
  "POSITIONMARKER",                                           /* 641 */
  "GRADIENT_BACKGROUND",                                      /* 642 */
  "GROUND_PLANE_BACKGROUND",                                  /* 643 */
  "HELIX",                                                    /* 644 */
  "RAPIDRTRENDERENVIRONMENT",                                 /* 645 */
  "IDBUFFER",                                                 /* 646 */
  "IMAGE",                                                    /* 647 */
  "IMAGEDEF",                                                 /* 648 */
  "IMAGEDEF_REACTOR",                                         /* 649 */
  "IMAGE_BACKGROUND",                                         /* 650 */
  "INDEX",                                                    /* 651 */
  "LARGE_RADIAL_DIMENSION",                                   /* 652 */
  "LAYERFILTER",                                              /* 653 */
  "LAYER_INDEX",                                              /* 654 */
  "LAYOUTPRINTCONFIG",                                        /* 655 */
  "ACDB_LEADEROBJECTCONTEXTDATA_CLASS",                       /* 656 */
  "LIGHT",                                                    /* 657 */
  "LIGHTLIST",                                                /* 658 */
  "LINEARPARAMETERENTITY",                                    /* 659 */
  "LOFTEDSURFACE",                                            /* 660 */
  "MATERIAL",                                                 /* 661 */
  "MENTALRAYRENDERSETTINGS",                                  /* 662 */
  "MESH",                                                     /* 663 */
  "ACDB_MLEADEROBJECTCONTEXTDATA_CLASS",                      /* 664 */
  "MLEADERSTYLE",                                             /* 665 */
  "MOVEACTIONENTITY",                                         /* 666 */
  "ACDBMOTIONPATH",                                           /* 667 */
  "MPOLYGON",                                                 /* 668 */
  "ACDB_MTEXTATTRIBUTEOBJECTCONTEXTDATA_CLASS",               /* 669 */
  "ACDB_MTEXTOBJECTCONTEXTDATA_CLASS",                        /* 670 */
  "MULTILEADER",                                              /* 671 */
  "COORDINATION_MODEL",                                       /* 672 */
  "ACDBNAVISWORKSMODELDEF",                                   /* 673 */
  "NPOCOLLECTION",                                            /* 674 */
  "NURBSURFACE",                                              /* 675 */
  "OBJECT_PTR",                                               /* 676 */
  "ACDB_ORDDIMOBJECTCONTEXTDATA_CLASS",                       /* 677 */
  "PARTIAL_VIEWING_INDEX",                                    /* 678 */
  "PDFDEFINITION",                                            /* 679 */
  "PDFUNDERLAY",                                              /* 680 */
  "ACDBPERSSUBENTMANAGER",                                    /* 681 */
  "PLANESURFACE",                                             /* 682 */
  "PLOTSETTINGS",                                             /* 683 */
  "ACDBPOINTCLOUD",                                           /* 684 */
  "ACDBPOINTCLOUDCOLORMAP",                                   /* 685 */
  "ACDBPOINTCLOUDDEF",                                        /* 686 */
  "ACDBPOINTCLOUDDEFEX",                                      /* 687 */
  "ACDBPOINTCLOUDDEF_REACTOR",                                /* 688 */
  "ACDBPOINTCLOUDDEF_REACTOR_EX",                             /* 689 */
  "ACDBPOINTCLOUDEX",                                         /* 690 */
  "POINTPARAMETERENTITY",                                     /* 691 */
  "ACDBPOINTPATH",                                            /* 692 */
  "POLARGRIPENTITY",                                          /* 693 */
  "ACDB_RADIMLGOBJECTCONTEXTDATA_CLASS",                      /* 694 */
  "ACDB_RADIMOBJECTCONTEXTDATA_CLASS",                        /* 695 */
  "RAPIDRTRENDERSETTINGS",                                    /* 696 */
  "RASTERVARIABLES",                                          /* 697 */
  "RENDERENTRY",                                              /* 698 */
  "RENDERENVIRONMENT",                                        /* 699 */
  "RENDERGLOBAL",                                             /* 700 */
  "RENDERSETTINGS",                                           /* 701 */
  "REVOLVEDSURFACE",                                          /* 702 */
  "ROTATIONPARAMETERENTITY",                                  /* 703 */
  "ROTATEACTIONENTITY",                                       /* 704 */
  "RTEXT",                                                    /* 705 */
  "SCALE",                                                    /* 706 */
  "SCALEACTIONENTITY",                                        /* 707 */
  "SECTIONOBJECT",                                            /* 708 */
  "ACDBSECTIONVIEWSTYLE",                                     /* 709 */
  "SECTION_MANAGER",                                          /* 710 */
  "SECTION_SETTINGS",                                         /* 711 */
  "SKYLIGHT_BACKGROUND",                                      /* 712 */
  "SOLID_BACKGROUND",                                         /* 713 */
  "SORTENTSTABLE",                                            /* 714 */
  "SPATIAL_FILTER",                                           /* 715 */
  "SPATIAL_INDEX",                                            /* 716 */
  "STRETCHACTIONENTITY",                                      /* 717 */
  "SUN",                                                      /* 718 */
  "SUNSTUDY",                                                 /* 719 */
  "SWEPTSURFACE",                                             /* 720 */
  "ACAD_TABLE",                                               /* 721 */
  "TABLECONTENT",                                             /* 722 */
  "TABLEGEOMETRY",                                            /* 723 */
  "TABLESTYLE",                                               /* 724 */
  "ACDB_TEXTOBJECTCONTEXTDATA_CLASS",                         /* 725 */
  "TVDEVICEPROPERTIES",                                       /* 726 */
  "VISIBILITYGRIPENTITY",                                     /* 727 */
  "VISIBILITYPARAMETERENTITY",                                /* 728 */
  "VISUALSTYLE",                                              /* 729 */
  "WIPEOUT",                                                  /* 730 */
  "WIPEOUTVARIABLES",                                         /* 731 */
  "EXACXREFPANELOBJECT",                                      /* 732 */
  "XYPARAMETERENTITY",                                        /* 733 */
  "BREAKDATA",                                                /* 734 */
  "BREAKPOINTREF",                                            /* 735 */
  "FLIPGRIPENTITY",                                           /* 736 */
  "LINEARGRIPENTITY",                                         /* 737 */
  "ROTATIONGRIPENTITY",                                       /* 738 */
  "XYGRIPENTITY",                                             /* 739 */
  "3DLINE",                                                   /* 740 */
  "REPEAT",                                                   /* 741 */
  "ENDREP",                                                   /* 742 */
  "JUMP",                                                     /* 743 */
  "POLYLINE",                                                 /* 744 */
  "VERTEX",                                                   /* 745 */
  "LOAD",                                                     /* 746 */
  "ABSHDRAWINGSETTINGS",                                      /* 747 */
  "ACAECUSTOBJ",                                              /* 748 */
  "ACAEEEMGROBJ",                                             /* 749 */
  "ACAMCOMP",                                                 /* 750 */
  "ACAMCOMPDEF",                                              /* 751 */
  "ACAMCOMPDEFMGR",                                           /* 752 */
  "ACAMCONTEXTMODELER",                                       /* 753 */
  "ACAMGDIMSTD",                                              /* 754 */
  "ACAMGFILTERDAT",                                           /* 755 */
  "ACAMGHOLECHARTSTDCSN",                                     /* 756 */
  "ACAMGHOLECHARTSTDDIN",                                     /* 757 */
  "ACAMGHOLECHARTSTDISO",                                     /* 758 */
  "ACAMGLAYSTD",                                              /* 759 */
  "ACAMGRCOMPDEF",                                            /* 760 */
  "ACAMGRCOMPDEFSET",                                         /* 761 */
  "ACAMGTITLESTD",                                            /* 762 */
  "ACAMMVDBACKUPOBJECT",                                      /* 763 */
  "ACAMPROJECT",                                              /* 764 */
  "ACAMSHAFTCOMPDEF",                                         /* 765 */
  "ACAMSTDPCOMPDEF",                                          /* 766 */
  "ACAMWBLOCKTEMPENTS",                                       /* 767 */
  "ACARRAYJIGENTITY",                                         /* 768 */
  "ACCMCONTEXT",                                              /* 769 */
  "ACDBCIRCARCRES",                                           /* 770 */
  "ACDBDIMENSIONRES",                                         /* 771 */
  "ACDBENTITYCACHE",                                          /* 772 */
  "ACDBLINERES",                                              /* 773 */
  "ACDBSTDPARTRES_ARC",                                       /* 774 */
  "ACDBSTDPARTRES_LINE",                                      /* 775 */
  "ACDB_HATCHSCALECONTEXTDATA_CLASS",                         /* 776 */
  "ACDB_HATCHVIEWCONTEXTDATA_CLASS",                          /* 777 */
  "ACDB_PROXY_ENTITY_DATA",                                   /* 778 */
  "ACGREFACADMASTER",                                         /* 779 */
  "ACGREFMASTER",                                             /* 780 */
  "ACIMINTSYSVAR",                                            /* 781 */
  "ACIMREALSYSVAR",                                           /* 782 */
  "ACIMSTRSYSVAR",                                            /* 783 */
  "ACIMSYSVARMAN",                                            /* 784 */
  "ACMANOOTATIONVIEWSTANDARDANSI",                            /* 785 */
  "ACMANOOTATIONVIEWSTANDARDCSN",                             /* 786 */
  "ACMANOOTATIONVIEWSTANDARDDIN",                             /* 787 */
  "ACMANOOTATIONVIEWSTANDARDISO",                             /* 788 */
  "ACMAPLEGENDDBOBJECT",                                      /* 789 */
  "ACMAPLEGENDITEMDBOBJECT",                                  /* 790 */
  "ACMAPMAPVIEWPORTDBOBJECT",                                 /* 791 */
  "ACMAPPRINTLAYOUTELEMENTDBOBJECTCONTAINER",                 /* 792 */
  "ACMBALLOON",                                               /* 793 */
  "ACMBOM",                                                   /* 794 */
  "ACMBOMROW",                                                /* 795 */
  "ACMBOMROWSTRUCT",                                          /* 796 */
  "ACMBOMSTANDARDANSI",                                       /* 797 */
  "ACMBOMSTANDARDCSN",                                        /* 798 */
  "ACMBOMSTANDARDDIN",                                        /* 799 */
  "ACMBOMSTANDARDISO",                                        /* 800 */
  "ACMCENTERLINESTANDARDANSI",                                /* 801 */
  "ACMCENTERLINESTANDARDCSN",                                 /* 802 */
  "ACMCENTERLINESTANDARDDIN",                                 /* 803 */
  "ACMCENTERLINESTANDARDISO",                                 /* 804 */
  "ACMDATADICTIONARY",                                        /* 805 */
  "ACMDATAENTRY",                                             /* 806 */
  "ACMDATAENTRYBLOCK",                                        /* 807 */
  "ACMDATUMID",                                               /* 808 */
  "ACMDATUMSTANDARDANSI",                                     /* 809 */
  "ACMDATUMSTANDARDCSN",                                      /* 810 */
  "ACMDATUMSTANDARDDIN",                                      /* 811 */
  "ACMDATUMSTANDARDISO",                                      /* 812 */
  "ACMDATUMSTANDARDISO2012",                                  /* 813 */
  "ACMDETAILSTANDARDANSI",                                    /* 814 */
  "ACMDETAILSTANDARDCSN",                                     /* 815 */
  "ACMDETAILSTANDARDDIN",                                     /* 816 */
  "ACMDETAILSTANDARDISO",                                     /* 817 */
  "ACMDETAILTANDARDCUSTOM",                                   /* 818 */
  "ACMDIMBREAKPERSREACTOR",                                   /* 819 */
  "ACMEDRAWINGMAN",                                           /* 820 */
  "ACMEVIEW",                                                 /* 821 */
  "ACME_DATABASE",                                            /* 822 */
  "ACME_DOCUMENT",                                            /* 823 */
  "ACMFCFRAME",                                               /* 824 */
  "ACMFCFSTANDARDANSI",                                       /* 825 */
  "ACMFCFSTANDARDCSN",                                        /* 826 */
  "ACMFCFSTANDARDDIN",                                        /* 827 */
  "ACMFCFSTANDARDISO",                                        /* 828 */
  "ACMFCFSTANDARDISO2004",                                    /* 829 */
  "ACMFCFSTANDARDISO2012",                                    /* 830 */
  "ACMIDSTANDARDANSI",                                        /* 831 */
  "ACMIDSTANDARDCSN",                                         /* 832 */
  "ACMIDSTANDARDDIN",                                         /* 833 */
  "ACMIDSTANDARDISO",                                         /* 834 */
  "ACMIDSTANDARDISO2004",                                     /* 835 */
  "ACMIDSTANDARDISO2012",                                     /* 836 */
  "ACMNOTESTANDARDANSI",                                      /* 837 */
  "ACMNOTESTANDARDCSN",                                       /* 838 */
  "ACMNOTESTANDARDDIN",                                       /* 839 */
  "ACMNOTESTANDARDISO",                                       /* 840 */
  "ACMPARTLIST",                                              /* 841 */
  "ACMPICKOBJ",                                               /* 842 */
  "ACMSECTIONSTANDARDANSI",                                   /* 843 */
  "ACMSECTIONSTANDARDCSN2002",                                /* 844 */
  "ACMSECTIONSTANDARDCUSTOM",                                 /* 845 */
  "ACMSECTIONSTANDARDDIN",                                    /* 846 */
  "ACMSECTIONSTANDARDISO",                                    /* 847 */
  "ACMSECTIONSTANDARDISO2001",                                /* 848 */
  "ACMSTANDARDANSI",                                          /* 849 */
  "ACMSTANDARDCSN",                                           /* 850 */
  "ACMSTANDARDDIN",                                           /* 851 */
  "ACMSTANDARDISO",                                           /* 852 */
  "ACMSURFSTANDARDANSI",                                      /* 853 */
  "ACMSURFSTANDARDCSN",                                       /* 854 */
  "ACMSURFSTANDARDDIN",                                       /* 855 */
  "ACMSURFSTANDARDISO",                                       /* 856 */
  "ACMSURFSTANDARDISO2002",                                   /* 857 */
  "ACMSURFSYM",                                               /* 858 */
  "ACMTAPERSTANDARDANSI",                                     /* 859 */
  "ACMTAPERSTANDARDCSN",                                      /* 860 */
  "ACMTAPERSTANDARDDIN",                                      /* 861 */
  "ACMTAPERSTANDARDISO",                                      /* 862 */
  "ACMTHREADLINESTANDARDANSI",                                /* 863 */
  "ACMTHREADLINESTANDARDCSN",                                 /* 864 */
  "ACMTHREADLINESTANDARDDIN",                                 /* 865 */
  "ACMTHREADLINESTANDARDISO",                                 /* 866 */
  "ACMWELDSTANDARDANSI",                                      /* 867 */
  "ACMWELDSTANDARDCSN",                                       /* 868 */
  "ACMWELDSTANDARDDIN",                                       /* 869 */
  "ACMWELDSTANDARDISO",                                       /* 870 */
  "ACMWELDSYM",                                               /* 871 */
  "ACRFATTGENMGR",                                            /* 872 */
  "ACRFINSADJ",                                               /* 873 */
  "ACRFINSADJUSTERMGR",                                       /* 874 */
  "ACRFMCADAPIATTHOLDER",                                     /* 875 */
  "ACRFOBJATTMGR",                                            /* 876 */
  "ACSH_SUBENT_MATERIAL_CLASS",                               /* 877 */
  "AC_AM_2D_XREF_MGR",                                        /* 878 */
  "AC_AM_BASIC_VIEW",                                         /* 879 */
  "AC_AM_BASIC_VIEW_DEF",                                     /* 880 */
  "AC_AM_COMPLEX_HIDE_SITUATION",                             /* 881 */
  "AC_AM_COMP_VIEW_DEF",                                      /* 882 */
  "AC_AM_COMP_VIEW_INST",                                     /* 883 */
  "AC_AM_DIRTY_NODES",                                        /* 884 */
  "AC_AM_HIDE_SITUATION",                                     /* 885 */
  "AC_AM_MAPPER_CACHE",                                       /* 886 */
  "AC_AM_MASTER_VIEW_DEF",                                    /* 887 */
  "AC_AM_MVD_DEP_MGR",                                        /* 888 */
  "AC_AM_OVERRIDE_FILTER",                                    /* 889 */
  "AC_AM_PROPS_OVERRIDE",                                     /* 890 */
  "AC_AM_SHAFT_HIDE_SITUATION",                               /* 891 */
  "AC_AM_STDP_VIEW_DEF",                                      /* 892 */
  "AC_AM_TRANSFORM_GHOST",                                    /* 893 */
  "ADAPPL",                                                   /* 894 */
  "AECC_ALIGNMENT_DESIGN_CHECK_SET",                          /* 895 */
  "AECC_ALIGNMENT_LABEL_SET",                                 /* 896 */
  "AECC_ALIGNMENT_LABEL_SET_EXT",                             /* 897 */
  "AECC_ALIGNMENT_PARCEL_NODE",                               /* 898 */
  "AECC_ALIGNMENT_STYLE",                                     /* 899 */
  "AECC_APPURTENANCE_STYLE",                                  /* 900 */
  "AECC_ASSEMBLY_STYLE",                                      /* 901 */
  "AECC_BUILDING_SITE_STYLE",                                 /* 902 */
  "AECC_CANT_DIAGRAM_VIEW_STYLE",                             /* 903 */
  "AECC_CATCHMENT_STYLE",                                     /* 904 */
  "AECC_CLASS_NODE",                                          /* 905 */
  "AECC_CONTOURVIEW",                                         /* 906 */
  "AECC_CORRIDOR_STYLE",                                      /* 907 */
  "AECC_DISP_REP_ALIGNMENT",                                  /* 908 */
  "AECC_DISP_REP_ALIGNMENT_CANT_LABEL_GROUP",                 /* 909 */
  "AECC_DISP_REP_ALIGNMENT_CSV",                              /* 910 */
  "AECC_DISP_REP_ALIGNMENT_CURVE_LABEL",                      /* 911 */
  "AECC_DISP_REP_ALIGNMENT_DESIGNSPEED_LABEL_GROUP",          /* 912 */
  "AECC_DISP_REP_ALIGNMENT_GEOMPT_LABEL_GROUP",               /* 913 */
  "AECC_DISP_REP_ALIGNMENT_INDEXED_PI_LABEL",                 /* 914 */
  "AECC_DISP_REP_ALIGNMENT_MINOR_STATION_LABEL_GROUP",        /* 915 */
  "AECC_DISP_REP_ALIGNMENT_PI_LABEL",                         /* 916 */
  "AECC_DISP_REP_ALIGNMENT_SPIRAL_LABEL",                     /* 917 */
  "AECC_DISP_REP_ALIGNMENT_STAEQU_LABEL_GROUP",               /* 918 */
  "AECC_DISP_REP_ALIGNMENT_STATION_LABEL_GROUP",              /* 919 */
  "AECC_DISP_REP_ALIGNMENT_STATION_OFFSET_LABEL",             /* 920 */
  "AECC_DISP_REP_ALIGNMENT_SUPERELEVATION_LABEL_GROUP",       /* 921 */
  "AECC_DISP_REP_ALIGNMENT_TABLE",                            /* 922 */
  "AECC_DISP_REP_ALIGNMENT_TANGENT_LABEL",                    /* 923 */
  "AECC_DISP_REP_ALIGNMENT_VERTICAL_GEOMPT_LABELING",         /* 924 */
  "AECC_DISP_REP_APPURTENANCE",                               /* 925 */
  "AECC_DISP_REP_APPURTENANCE_CSV",                           /* 926 */
  "AECC_DISP_REP_APPURTENANCE_LABELING",                      /* 927 */
  "AECC_DISP_REP_APPURTENANCE_PROFILE_LABELING",              /* 928 */
  "AECC_DISP_REP_ASSEMBLY",                                   /* 929 */
  "AECC_DISP_REP_AUTO_CORRIDOR_FEATURE_LINE",                 /* 930 */
  "AECC_DISP_REP_AUTO_CORRIDOR_FEATURE_LINE_PROFILE",         /* 931 */
  "AECC_DISP_REP_AUTO_CORRIDOR_FEATURE_LINE_SECTION",         /* 932 */
  "AECC_DISP_REP_AUTO_FEATURE_LINE",                          /* 933 */
  "AECC_DISP_REP_AUTO_FEATURE_LINE_CSV",                      /* 934 */
  "AECC_DISP_REP_AUTO_FEATURE_LINE_PROFILE",                  /* 935 */
  "AECC_DISP_REP_AUTO_FEATURE_LINE_SECTION",                  /* 936 */
  "AECC_DISP_REP_BUILDINGSITE",                               /* 937 */
  "AECC_DISP_REP_BUILDINGUTIL_CONNECTOR",                     /* 938 */
  "AECC_DISP_REP_CANT_DIAGRAM_VIEW",                          /* 939 */
  "AECC_DISP_REP_CATCHMENT_AREA",                             /* 940 */
  "AECC_DISP_REP_CATCHMENT_AREA_LABEL",                       /* 941 */
  "AECC_DISP_REP_CORRIDOR",                                   /* 942 */
  "AECC_DISP_REP_CROSSING_PIPE_PROFILE_LABELING",             /* 943 */
  "AECC_DISP_REP_CROSSING_PRESSURE_PIPE_PROFILE_LABELING",    /* 944 */
  "AECC_DISP_REP_CSVSTATIONSLIDER",                           /* 945 */
  "AECC_DISP_REP_FACE",                                       /* 946 */
  "AECC_DISP_REP_FEATURE",                                    /* 947 */
  "AECC_DISP_REP_FEATURE_LABEL",                              /* 948 */
  "AECC_DISP_REP_FEATURE_LINE",                               /* 949 */
  "AECC_DISP_REP_FEATURE_LINE_CSV",                           /* 950 */
  "AECC_DISP_REP_FEATURE_LINE_PROFILE",                       /* 951 */
  "AECC_DISP_REP_FEATURE_LINE_SECTION",                       /* 952 */
  "AECC_DISP_REP_FITTING",                                    /* 953 */
  "AECC_DISP_REP_FITTING_CSV",                                /* 954 */
  "AECC_DISP_REP_FITTING_LABELING",                           /* 955 */
  "AECC_DISP_REP_FITTING_PROFILE_LABELING",                   /* 956 */
  "AECC_DISP_REP_FLOW_SEGMENT_LABEL",                         /* 957 */
  "AECC_DISP_REP_GENERAL_SEGMENT_LABEL",                      /* 958 */
  "AECC_DISP_REP_GRADING",                                    /* 959 */
  "AECC_DISP_REP_GRAPH",                                      /* 960 */
  "AECC_DISP_REP_GRAPHPROFILE_NETWORKPART",                   /* 961 */
  "AECC_DISP_REP_GRAPHPROFILE_PRESSUREPART",                  /* 962 */
  "AECC_DISP_REP_GRID_SURFACE",                               /* 963 */
  "AECC_DISP_REP_GRID_SURFACE_CSV",                           /* 964 */
  "AECC_DISP_REP_HORGEOMETRY_BAND_LABEL_GROUP",               /* 965 */
  "AECC_DISP_REP_HYDRO_REGION",                               /* 966 */
  "AECC_DISP_REP_INTERFERENCE_CHECK",                         /* 967 */
  "AECC_DISP_REP_INTERFERENCE_PART",                          /* 968 */
  "AECC_DISP_REP_INTERFERENCE_PART_SECTION",                  /* 969 */
  "AECC_DISP_REP_INTERSECTION",                               /* 970 */
  "AECC_DISP_REP_INTERSECTION_LOCATION_LABELING",             /* 971 */
  "AECC_DISP_REP_LABELING",                                   /* 972 */
  "AECC_DISP_REP_LEGEND_TABLE",                               /* 973 */
  "AECC_DISP_REP_LINE_BETWEEN_POINTS_LABEL",                  /* 974 */
  "AECC_DISP_REP_LOTLINE_CSV",                                /* 975 */
  "AECC_DISP_REP_MASSHAULLINE",                               /* 976 */
  "AECC_DISP_REP_MASS_HAUL_VIEW",                             /* 977 */
  "AECC_DISP_REP_MATCHLINE_LABELING",                         /* 978 */
  "AECC_DISP_REP_MATCH_LINE",                                 /* 979 */
  "AECC_DISP_REP_MATERIAL_SECTION",                           /* 980 */
  "AECC_DISP_REP_NETWORK",                                    /* 981 */
  "AECC_DISP_REP_NOTE_LABEL",                                 /* 982 */
  "AECC_DISP_REP_OFFSET_ELEV_LABEL",                          /* 983 */
  "AECC_DISP_REP_PARCEL_BOUNDARY",                            /* 984 */
  "AECC_DISP_REP_PARCEL_FACE_LABEL",                          /* 985 */
  "AECC_DISP_REP_PARCEL_SEGMENT",                             /* 986 */
  "AECC_DISP_REP_PARCEL_SEGMENT_LABEL",                       /* 987 */
  "AECC_DISP_REP_PARCEL_SEGMENT_TABLE",                       /* 988 */
  "AECC_DISP_REP_PARCEL_TABLE",                               /* 989 */
  "AECC_DISP_REP_PIPE",                                       /* 990 */
  "AECC_DISP_REP_PIPENETWORK_BAND_LABEL_GROUP",               /* 991 */
  "AECC_DISP_REP_PIPE_CSV",                                   /* 992 */
  "AECC_DISP_REP_PIPE_LABELING",                              /* 993 */
  "AECC_DISP_REP_PIPE_PROFILE_LABELING",                      /* 994 */
  "AECC_DISP_REP_PIPE_SECTION_LABELING",                      /* 995 */
  "AECC_DISP_REP_PIPE_TABLE",                                 /* 996 */
  "AECC_DISP_REP_POINT_ENT",                                  /* 997 */
  "AECC_DISP_REP_POINT_GROUP",                                /* 998 */
  "AECC_DISP_REP_POINT_TABLE",                                /* 999 */
  "AECC_DISP_REP_PRESSUREPIPENETWORK",                        /* 1000 */
  "AECC_DISP_REP_PRESSURE_PART_TABLE",                        /* 1001 */
  "AECC_DISP_REP_PRESSURE_PIPE",                              /* 1002 */
  "AECC_DISP_REP_PRESSURE_PIPE_CSV",                          /* 1003 */
  "AECC_DISP_REP_PRESSURE_PIPE_LABELING",                     /* 1004 */
  "AECC_DISP_REP_PRESSURE_PIPE_PROFILE_LABELING",             /* 1005 */
  "AECC_DISP_REP_PRESSURE_PIPE_SECTION_LABELING",             /* 1006 */
  "AECC_DISP_REP_PROFILE",                                    /* 1007 */
  "AECC_DISP_REP_PROFILEDATA_BAND_LABEL_GROUP",               /* 1008 */
  "AECC_DISP_REP_PROFILE_PROJECTION",                         /* 1009 */
  "AECC_DISP_REP_PROFILE_PROJECTION_LABEL",                   /* 1010 */
  "AECC_DISP_REP_PROFILE_VIEW",                               /* 1011 */
  "AECC_DISP_REP_PROFILE_VIEW_DEPTH_LABEL",                   /* 1012 */
  "AECC_DISP_REP_QUANTITY_TAKEOFF_AGGREGATE_EARTHWORK_TABLE", /* 1013 */
  "AECC_DISP_REP_RIGHT_OF_WAY",                               /* 1014 */
  "AECC_DISP_REP_SAMPLELINE_LABELING",                        /* 1015 */
  "AECC_DISP_REP_SAMPLE_LINE",                                /* 1016 */
  "AECC_DISP_REP_SAMPLE_LINE_GROUP",                          /* 1017 */
  "AECC_DISP_REP_SECTION",                                    /* 1018 */
  "AECC_DISP_REP_SECTIONALDATA_BAND_LABEL_GROUP",             /* 1019 */
  "AECC_DISP_REP_SECTIONDATA_BAND_LABEL_GROUP",               /* 1020 */
  "AECC_DISP_REP_SECTIONSEGMENT_BAND_LABEL_GROUP",            /* 1021 */
  "AECC_DISP_REP_SECTION_CORRIDOR",                           /* 1022 */
  "AECC_DISP_REP_SECTION_CORRIDOR_POINT_LABEL_GROUP",         /* 1023 */
  "AECC_DISP_REP_SECTION_GRADEBREAK_LABEL_GROUP",             /* 1024 */
  "AECC_DISP_REP_SECTION_MINOR_OFFSET_LABEL_GROUP",           /* 1025 */
  "AECC_DISP_REP_SECTION_OFFSET_LABEL_GROUP",                 /* 1026 */
  "AECC_DISP_REP_SECTION_PIPENETWORK",                        /* 1027 */
  "AECC_DISP_REP_SECTION_PRESSUREPIPENETWORK",                /* 1028 */
  "AECC_DISP_REP_SECTION_PROJECTION",                         /* 1029 */
  "AECC_DISP_REP_SECTION_PROJECTION_LABEL",                   /* 1030 */
  "AECC_DISP_REP_SECTION_SEGMENT_LABEL_GROUP",                /* 1031 */
  "AECC_DISP_REP_SECTION_VIEW",                               /* 1032 */
  "AECC_DISP_REP_SECTION_VIEW_DEPTH_LABEL",                   /* 1033 */
  "AECC_DISP_REP_SECTION_VIEW_QUANTITY_TAKEOFF_TABLE",        /* 1034 */
  "AECC_DISP_REP_SHEET",                                      /* 1035 */
  "AECC_DISP_REP_SPANNING_PIPE_LABELING",                     /* 1036 */
  "AECC_DISP_REP_SPANNING_PIPE_PROFILE_LABELING",             /* 1037 */
  "AECC_DISP_REP_STATION_ELEV_LABEL",                         /* 1038 */
  "AECC_DISP_REP_STRUCTURE",                                  /* 1039 */
  "AECC_DISP_REP_STRUCTURE_CSV",                              /* 1040 */
  "AECC_DISP_REP_STRUCTURE_LABELING",                         /* 1041 */
  "AECC_DISP_REP_STRUCTURE_PROFILE_LABELING",                 /* 1042 */
  "AECC_DISP_REP_STRUCTURE_SECTION_LABELING",                 /* 1043 */
  "AECC_DISP_REP_SUBASSEMBLY",                                /* 1044 */
  "AECC_DISP_REP_SUPERELEVATION_BAND_LABEL_GROUP",            /* 1045 */
  "AECC_DISP_REP_SUPERELEVATION_DIAGRAM_VIEW",                /* 1046 */
  "AECC_DISP_REP_SURFACE_CONTOUR_LABEL_GROUP",                /* 1047 */
  "AECC_DISP_REP_SURFACE_ELEVATION_LABEL",                    /* 1048 */
  "AECC_DISP_REP_SURFACE_SLOPE_LABEL",                        /* 1049 */
  "AECC_DISP_REP_SURVEY_FIGURE_LABEL_GROUP",                  /* 1050 */
  "AECC_DISP_REP_SVFIGURE",                                   /* 1051 */
  "AECC_DISP_REP_SVFIGURE_CSV",                               /* 1052 */
  "AECC_DISP_REP_SVFIGURE_PROFILE",                           /* 1053 */
  "AECC_DISP_REP_SVFIGURE_SECTION",                           /* 1054 */
  "AECC_DISP_REP_SVFIGURE_SEGMENT_LABEL",                     /* 1055 */
  "AECC_DISP_REP_SVNETWORK",                                  /* 1056 */
  "AECC_DISP_REP_TANGENT_INTERSECTION_TABLE",                 /* 1057 */
  "AECC_DISP_REP_TIN_SURFACE",                                /* 1058 */
  "AECC_DISP_REP_TIN_SURFACE_CSV",                            /* 1059 */
  "AECC_DISP_REP_VALIGNMENT_CRESTCURVE_LABEL_GROUP",          /* 1060 */
  "AECC_DISP_REP_VALIGNMENT_CSV",                             /* 1061 */
  "AECC_DISP_REP_VALIGNMENT_HAGEOMPT_LABEL_GROUP",            /* 1062 */
  "AECC_DISP_REP_VALIGNMENT_LINE_LABEL_GROUP",                /* 1063 */
  "AECC_DISP_REP_VALIGNMENT_MINOR_STATION_LABEL_GROUP",       /* 1064 */
  "AECC_DISP_REP_VALIGNMENT_PVI_LABEL_GROUP",                 /* 1065 */
  "AECC_DISP_REP_VALIGNMENT_SAGCURVE_LABEL_GROUP",            /* 1066 */
  "AECC_DISP_REP_VALIGNMENT_STATION_LABEL_GROUP",             /* 1067 */
  "AECC_DISP_REP_VERTICALGEOMETRY_BAND_LABEL_GROUP",          /* 1068 */
  "AECC_DISP_REP_VIEWFRAME_LABELING",                         /* 1069 */
  "AECC_DISP_REP_VIEW_FRAME",                                 /* 1070 */
  "AECC_FEATURELINE_STYLE",                                   /* 1071 */
  "AECC_FEATURE_STYLE",                                       /* 1072 */
  "AECC_FITTING_STYLE",                                       /* 1073 */
  "AECC_FORMAT_MANAGER_OBJECT",                               /* 1074 */
  "AECC_GRADEVIEW",                                           /* 1075 */
  "AECC_GRADING_CRITERIA",                                    /* 1076 */
  "AECC_GRADING_CRITERIA_SET",                                /* 1077 */
  "AECC_GRADING_GROUP",                                       /* 1078 */
  "AECC_GRADING_STYLE",                                       /* 1079 */
  "AECC_IMPORT_STORM_SEWER_DEFAULTS",                         /* 1080 */
  "AECC_INTERFERENCE_STYLE",                                  /* 1081 */
  "AECC_INTERSECTION_STYLE",                                  /* 1082 */
  "AECC_LABEL_COLLECTOR_STYLE",                               /* 1083 */
  "AECC_LABEL_NODE",                                          /* 1084 */
  "AECC_LABEL_RADIAL_LINE_STYLE",                             /* 1085 */
  "AECC_LABEL_TEXT_ITERATOR_CURVE_OR_SPIRAL_STYLE",           /* 1086 */
  "AECC_LABEL_TEXT_ITERATOR_STYLE",                           /* 1087 */
  "AECC_LABEL_TEXT_STYLE",                                    /* 1088 */
  "AECC_LABEL_VECTOR_ARROW_STYLE",                            /* 1089 */
  "AECC_LEGEND_TABLE_STYLE",                                  /* 1090 */
  "AECC_MASS_HAUL_LINE_STYLE",                                /* 1091 */
  "AECC_MASS_HAUL_VIEW_STYLE",                                /* 1092 */
  "AECC_MATCHLINE_STYLE",                                     /* 1093 */
  "AECC_MATERIAL_STYLE",                                      /* 1094 */
  "AECC_NETWORK_PART_CATALOG_DEF_NODE",                       /* 1095 */
  "AECC_NETWORK_PART_FAMILY_ITEM",                            /* 1096 */
  "AECC_NETWORK_PART_LIST",                                   /* 1097 */
  "AECC_NETWORK_RULE",                                        /* 1098 */
  "AECC_PARCEL_NODE",                                         /* 1099 */
  "AECC_PARCEL_STYLE",                                        /* 1100 */
  "AECC_PART_SIZE_FILTER",                                    /* 1101 */
  "AECC_PIPE_RULES",                                          /* 1102 */
  "AECC_PIPE_STYLE",                                          /* 1103 */
  "AECC_PIPE_STYLE_EXTENSION",                                /* 1104 */
  "AECC_POINTCLOUD_STYLE",                                    /* 1105 */
  "AECC_POINTVIEW",                                           /* 1106 */
  "AECC_POINT_STYLE",                                         /* 1107 */
  "AECC_PRESSURE_PART_LIST",                                  /* 1108 */
  "AECC_PRESSURE_PIPE_STYLE",                                 /* 1109 */
  "AECC_PROFILESECTIONENTITY_STYLE",                          /* 1110 */
  "AECC_PROFILE_DESIGN_CHECK_SET",                            /* 1111 */
  "AECC_PROFILE_LABEL_SET",                                   /* 1112 */
  "AECC_PROFILE_STYLE",                                       /* 1113 */
  "AECC_PROFILE_VIEW_BAND_STYLE_SET",                         /* 1114 */
  "AECC_PROFILE_VIEW_DATA_BAND_STYLE",                        /* 1115 */
  "AECC_PROFILE_VIEW_HORIZONTAL_GEOMETRY_BAND_STYLE",         /* 1116 */
  "AECC_PROFILE_VIEW_PIPE_NETWORK_BAND_STYLE",                /* 1117 */
  "AECC_PROFILE_VIEW_SECTIONAL_DATA_BAND_STYLE",              /* 1118 */
  "AECC_PROFILE_VIEW_STYLE",                                  /* 1119 */
  "AECC_PROFILE_VIEW_SUPERELEVATION_DIAGRAM_BAND_STYLE",      /* 1120 */
  "AECC_PROFILE_VIEW_VERTICAL_GEOMETRY_BAND_STYLE",           /* 1121 */
  "AECC_QUANTITY_TAKEOFF_CRITERIA",                           /* 1122 */
  "AECC_ROADWAYLINK_STYLE",                                   /* 1123 */
  "AECC_ROADWAYMARKER_STYLE",                                 /* 1124 */
  "AECC_ROADWAYSHAPE_STYLE",                                  /* 1125 */
  "AECC_ROADWAY_STYLE_SET",                                   /* 1126 */
  "AECC_ROOT_SETTINGS_NODE",                                  /* 1127 */
  "AECC_SAMPLE_LINE_GROUP_STYLE",                             /* 1128 */
  "AECC_SAMPLE_LINE_STYLE",                                   /* 1129 */
  "AECC_SECTION_LABEL_SET",                                   /* 1130 */
  "AECC_SECTION_STYLE",                                       /* 1131 */
  "AECC_SECTION_VIEW_BAND_STYLE_SET",                         /* 1132 */
  "AECC_SECTION_VIEW_DATA_BAND_STYLE",                        /* 1133 */
  "AECC_SECTION_VIEW_ROAD_SURFACE_BAND_STYLE",                /* 1134 */
  "AECC_SECTION_VIEW_STYLE",                                  /* 1135 */
  "AECC_SETTINGS_NODE",                                       /* 1136 */
  "AECC_SHEET_STYLE",                                         /* 1137 */
  "AECC_SLOPE_PATTERN_STYLE",                                 /* 1138 */
  "AECC_STATION_FORMAT_STYLE",                                /* 1139 */
  "AECC_STRUCTURE_RULES",                                     /* 1140 */
  "AECC_STUCTURE_STYLE",                                      /* 1141 */
  "AECC_SUPERELEVATION_DIAGRAM_VIEW_STYLE",                   /* 1142 */
  "AECC_SURFACE_STYLE",                                       /* 1143 */
  "AECC_SVFIGURE_STYLE",                                      /* 1144 */
  "AECC_SVNETWORK_STYLE",                                     /* 1145 */
  "AECC_TABLE_STYLE",                                         /* 1146 */
  "AECC_TAG_MANAGER",                                         /* 1147 */
  "AECC_TREE_NODE",                                           /* 1148 */
  "AECC_USER_DEFINED_ATTRIBUTE_CLASSIFICATION",               /* 1149 */
  "AECC_VALIGNMENT_STYLE_EXTENSION",                          /* 1150 */
  "AECC_VIEW_FRAME_STYLE",                                    /* 1151 */
  "AECS_DISP_PROPS_MEMBER",                                   /* 1152 */
  "AECS_DISP_PROPS_MEMBER_LOGICAL",                           /* 1153 */
  "AECS_DISP_PROPS_MEMBER_PLAN",                              /* 1154 */
  "AECS_DISP_PROPS_MEMBER_PLAN_SKETCH",                       /* 1155 */
  "AECS_DISP_PROPS_MEMBER_PROJECTED",                         /* 1156 */
  "AECS_DISP_REP_MEMBER_ELEVATION_DESIGN",                    /* 1157 */
  "AECS_DISP_REP_MEMBER_ELEVATION_DETAIL",                    /* 1158 */
  "AECS_DISP_REP_MEMBER_LOGICAL",                             /* 1159 */
  "AECS_DISP_REP_MEMBER_MODEL_DESIGN",                        /* 1160 */
  "AECS_DISP_REP_MEMBER_MODEL_DETAIL",                        /* 1161 */
  "AECS_DISP_REP_MEMBER_PLAN_DESIGN",                         /* 1162 */
  "AECS_DISP_REP_MEMBER_PLAN_DETAIL",                         /* 1163 */
  "AECS_DISP_REP_MEMBER_PLAN_SKETCH",                         /* 1164 */
  "AECS_MEMBER_NODE_SHAPE",                                   /* 1165 */
  "AECS_MEMBER_STYLE",                                        /* 1166 */
  "AEC_2DSECTION_STYLE",                                      /* 1167 */
  "AEC_AECDBDISPREPBDGELEVLINEPLAN100",                       /* 1168 */
  "AEC_AECDBDISPREPBDGELEVLINEPLAN50",                        /* 1169 */
  "AEC_AECDBDISPREPBDGSECTIONLINEPLAN100",                    /* 1170 */
  "AEC_AECDBDISPREPBDGSECTIONLINEPLAN50",                     /* 1171 */
  "AEC_AECDBDISPREPCEILINGGRIDPLAN100",                       /* 1172 */
  "AEC_AECDBDISPREPCEILINGGRIDPLAN50",                        /* 1173 */
  "AEC_AECDBDISPREPCOLUMNGRIDPLAN100",                        /* 1174 */
  "AEC_AECDBDISPREPCOLUMNGRIDPLAN50",                         /* 1175 */
  "AEC_AECDBDISPREPCURTAINWALLLAYOUTPLAN100",                 /* 1176 */
  "AEC_AECDBDISPREPCURTAINWALLLAYOUTPLAN50",                  /* 1177 */
  "AEC_AECDBDISPREPCURTAINWALLUNITPLAN100",                   /* 1178 */
  "AEC_AECDBDISPREPCURTAINWALLUNITPLAN50",                    /* 1179 */
  "AEC_AECDBDISPREPMVBLOCKREFPLAN100",                        /* 1180 */
  "AEC_AECDBDISPREPMVBLOCKREFPLAN50",                         /* 1181 */
  "AEC_AECDBDISPREPROOFPLAN100",                              /* 1182 */
  "AEC_AECDBDISPREPROOFPLAN50",                               /* 1183 */
  "AEC_AECDBDISPREPROOFSLABPLAN100",                          /* 1184 */
  "AEC_AECDBDISPREPROOFSLABPLAN50",                           /* 1185 */
  "AEC_AECDBDISPREPSLABPLAN100",                              /* 1186 */
  "AEC_AECDBDISPREPSLABPLAN50",                               /* 1187 */
  "AEC_AECDBDISPREPSPACEPLAN100",                             /* 1188 */
  "AEC_AECDBDISPREPSPACEPLAN50",                              /* 1189 */
  "AEC_AECDBDISPREPWALLPLAN100",                              /* 1190 */
  "AEC_AECDBDISPREPWALLPLAN50",                               /* 1191 */
  "AEC_AECDBDISPREPWINDOWASSEMBLYPLAN100",                    /* 1192 */
  "AEC_AECDBDISPREPWINDOWASSEMBLYPLAN50",                     /* 1193 */
  "AEC_AECDBDISPREPZONE100",                                  /* 1194 */
  "AEC_AECDBDISPREPZONE50",                                   /* 1195 */
  "AEC_AECDBZONEDEF",                                         /* 1196 */
  "AEC_AECDBZONESTYLE",                                       /* 1197 */
  "AEC_ANCHOR_OPENINGBASE_TO_WALL",                           /* 1198 */
  "AEC_CLASSIFICATION_DEF",                                   /* 1199 */
  "AEC_CLASSIFICATION_SYSTEM_DEF",                            /* 1200 */
  "AEC_CLEANUP_GROUP_DEF",                                    /* 1201 */
  "AEC_CURTAIN_WALL_LAYOUT_STYLE",                            /* 1202 */
  "AEC_CURTAIN_WALL_UNIT_STYLE",                              /* 1203 */
  "AEC_CVSECTIONVIEW",                                        /* 1204 */
  "AEC_DB_DISP_REP_DIM_GROUP_PLAN",                           /* 1205 */
  "AEC_DB_DISP_REP_DIM_GROUP_PLAN100",                        /* 1206 */
  "AEC_DB_DISP_REP_DIM_GROUP_PLAN50",                         /* 1207 */
  "AEC_DIM_STYLE",                                            /* 1208 */
  "AEC_DISPLAYTHEME_STYLE",                                   /* 1209 */
  "AEC_DISP_PROPS_2D_SECTION",                                /* 1210 */
  "AEC_DISP_PROPS_CLIP_VOLUME",                               /* 1211 */
  "AEC_DISP_PROPS_CLIP_VOLUME_RESULT",                        /* 1212 */
  "AEC_DISP_PROPS_DIM",                                       /* 1213 */
  "AEC_DISP_PROPS_DISPLAYTHEME",                              /* 1214 */
  "AEC_DISP_PROPS_DOOR",                                      /* 1215 */
  "AEC_DISP_PROPS_DOOR_NOMINAL",                              /* 1216 */
  "AEC_DISP_PROPS_DOOR_PLAN_100",                             /* 1217 */
  "AEC_DISP_PROPS_DOOR_PLAN_50",                              /* 1218 */
  "AEC_DISP_PROPS_DOOR_THRESHOLD_PLAN",                       /* 1219 */
  "AEC_DISP_PROPS_DOOR_THRESHOLD_SYMBOL_PLAN",                /* 1220 */
  "AEC_DISP_PROPS_EDITINPLACEPROFILE_MODEL",                  /* 1221 */
  "AEC_DISP_PROPS_ENT",                                       /* 1222 */
  "AEC_DISP_PROPS_ENT_REF",                                   /* 1223 */
  "AEC_DISP_PROPS_GRID_ASSEMBLY_MODEL",                       /* 1224 */
  "AEC_DISP_PROPS_GRID_ASSEMBLY_PLAN",                        /* 1225 */
  "AEC_DISP_PROPS_LAYOUT_CURVE",                              /* 1226 */
  "AEC_DISP_PROPS_LAYOUT_GRID2D",                             /* 1227 */
  "AEC_DISP_PROPS_LAYOUT_GRID3D",                             /* 1228 */
  "AEC_DISP_PROPS_MASKBLOCK",                                 /* 1229 */
  "AEC_DISP_PROPS_MASS_ELEM_MODEL",                           /* 1230 */
  "AEC_DISP_PROPS_MASS_GROUP",                                /* 1231 */
  "AEC_DISP_PROPS_MATERIAL",                                  /* 1232 */
  "AEC_DISP_PROPS_OPENING",                                   /* 1233 */
  "AEC_DISP_PROPS_POLYGON_MODEL",                             /* 1234 */
  "AEC_DISP_PROPS_POLYGON_TRUECOLOUR",                        /* 1235 */
  "AEC_DISP_PROPS_RAILING_MODEL",                             /* 1236 */
  "AEC_DISP_PROPS_RAILING_PLAN",                              /* 1237 */
  "AEC_DISP_PROPS_ROOF",                                      /* 1238 */
  "AEC_DISP_PROPS_ROOFSLAB",                                  /* 1239 */
  "AEC_DISP_PROPS_ROOFSLAB_PLAN",                             /* 1240 */
  "AEC_DISP_PROPS_SCHEDULE_TABLE",                            /* 1241 */
  "AEC_DISP_PROPS_SLAB",                                      /* 1242 */
  "AEC_DISP_PROPS_SLAB_PLAN",                                 /* 1243 */
  "AEC_DISP_PROPS_SLICE",                                     /* 1244 */
  "AEC_DISP_PROPS_SPACE_DECOMPOSED",                          /* 1245 */
  "AEC_DISP_PROPS_SPACE_MODEL",                               /* 1246 */
  "AEC_DISP_PROPS_SPACE_PLAN",                                /* 1247 */
  "AEC_DISP_PROPS_STAIR_MODEL",                               /* 1248 */
  "AEC_DISP_PROPS_STAIR_PLAN",                                /* 1249 */
  "AEC_DISP_PROPS_STAIR_PLAN_OVERLAPPING",                    /* 1250 */
  "AEC_DISP_PROPS_WALL_GRAPH",                                /* 1251 */
  "AEC_DISP_PROPS_WALL_MODEL",                                /* 1252 */
  "AEC_DISP_PROPS_WALL_PLAN",                                 /* 1253 */
  "AEC_DISP_PROPS_WALL_SCHEM",                                /* 1254 */
  "AEC_DISP_PROPS_WINDOW",                                    /* 1255 */
  "AEC_DISP_PROPS_WINDOW_ASSEMBLY_SILL_PLAN",                 /* 1256 */
  "AEC_DISP_PROPS_WINDOW_NOMINAL",                            /* 1257 */
  "AEC_DISP_PROPS_WINDOW_PLAN_100",                           /* 1258 */
  "AEC_DISP_PROPS_WINDOW_PLAN_50",                            /* 1259 */
  "AEC_DISP_PROPS_WINDOW_SILL_PLAN",                          /* 1260 */
  "AEC_DISP_PROPS_ZONE",                                      /* 1261 */
  "AEC_DISP_REP_2D_SECTION",                                  /* 1262 */
  "AEC_DISPREPAECDBDISPREPMASSELEMPLAN100",                   /* 1263 */
  "AEC_DISPREPAECDBDISPREPMASSELEMPLAN50",                    /* 1264 */
  "AEC_DISPREPAECDBDISPREPMASSGROUPPLAN100",                  /* 1265 */
  "AEC_DISPREPAECDBDISPREPMASSGROUPPLAN50",                   /* 1266 */
  "AEC_DISPREPAECDBDISPREPOPENINGPLAN100",                    /* 1267 */
  "AEC_DISPREPAECDBDISPREPOPENINGPLAN50",                     /* 1268 */
  "AEC_DISPREPAECDBDISPREPOPENINGPLANREFLECTED",              /* 1269 */
  "AEC_DISPREPAECDBDISPREPOPENINGSILLPLAN",                   /* 1270 */
  "AEC_DISP_REP_ANCHOR",                                      /* 1271 */
  "AEC_DISP_REP_ANCHOR_BUB_TO_GRID",                          /* 1272 */
  "AEC_DISP_REP_ANCHOR_BUB_TO_GRID_MODEL",                    /* 1273 */
  "AEC_DISP_REP_ANCHOR_BUB_TO_GRID_RCP",                      /* 1274 */
  "AEC_DISP_REP_ANCHOR_ENT_TO_NODE",                          /* 1275 */
  "AEC_DISP_REP_ANCHOR_EXT_TAG_TO_ENT",                       /* 1276 */
  "AEC_DISP_REP_ANCHOR_TAG_TO_ENT",                           /* 1277 */
  "AEC_DISP_REP_BDG_ELEVLINE_MODEL",                          /* 1278 */
  "AEC_DISP_REP_BDG_ELEVLINE_PLAN",                           /* 1279 */
  "AEC_DISP_REP_BDG_ELEVLINE_RCP",                            /* 1280 */
  "AEC_DISP_REP_BDG_SECTIONLINE_MODEL",                       /* 1281 */
  "AEC_DISP_REP_BDG_SECTIONLINE_PLAN",                        /* 1282 */
  "AEC_DISP_REP_BDG_SECTIONLINE_RCP",                         /* 1283 */
  "AEC_DISP_REP_BDG_SECTION_MODEL",                           /* 1284 */
  "AEC_DISP_REP_BDG_SECTION_SUBDIV",                          /* 1285 */
  "AEC_DISP_REP_CEILING_GRID",                                /* 1286 */
  "AEC_DISP_REP_CEILING_GRID_MODEL",                          /* 1287 */
  "AEC_DISP_REP_CEILING_GRID_RCP",                            /* 1288 */
  "AEC_DISP_REP_CLIP_VOLUME_MODEL",                           /* 1289 */
  "AEC_DISP_REP_CLIP_VOLUME_PLAN",                            /* 1290 */
  "AEC_DISP_REP_CLIP_VOLUME_RESULT",                          /* 1291 */
  "AEC_DISP_REP_CLIP_VOLUME_RESULT_SUBDIV",                   /* 1292 */
  "AEC_DISP_REP_COL_BLOCK",                                   /* 1293 */
  "AEC_DISP_REP_COL_CIRCARC2D",                               /* 1294 */
  "AEC_DISP_REP_COL_CONCOINCIDENT",                           /* 1295 */
  "AEC_DISP_REP_COL_CONCONCENTRIC",                           /* 1296 */
  "AEC_DISP_REP_COL_CONEQUALDISTANCE",                        /* 1297 */
  "AEC_DISP_REP_COL_CONMIDPOINT",                             /* 1298 */
  "AEC_DISP_REP_COL_CONNECTOR",                               /* 1299 */
  "AEC_DISP_REP_COL_CONNORMAL",                               /* 1300 */
  "AEC_DISP_REP_COL_CONPARALLEL",                             /* 1301 */
  "AEC_DISP_REP_COL_CONPERPENDICULAR",                        /* 1302 */
  "AEC_DISP_REP_COL_CONSYMMETRIC",                            /* 1303 */
  "AEC_DISP_REP_COL_CONTANGENT",                              /* 1304 */
  "AEC_DISP_REP_COL_DIMANGLE",                                /* 1305 */
  "AEC_DISP_REP_COL_DIMDIAMETER",                             /* 1306 */
  "AEC_DISP_REP_COL_DIMDISTANCE",                             /* 1307 */
  "AEC_DISP_REP_COL_DIMLENGTH",                               /* 1308 */
  "AEC_DISP_REP_COL_DIMMAJORRADIUS",                          /* 1309 */
  "AEC_DISP_REP_COL_DIMMINORRADIUS",                          /* 1310 */
  "AEC_DISP_REP_COL_ELLIPARC2D",                              /* 1311 */
  "AEC_DISP_REP_COL_LAYOUTDATA",                              /* 1312 */
  "AEC_DISP_REP_COL_LINE2D",                                  /* 1313 */
  "AEC_DISP_REP_COL_MODIFIER_ADD",                            /* 1314 */
  "AEC_DISP_REP_COL_MODIFIER_CUTPLANE",                       /* 1315 */
  "AEC_DISP_REP_COL_MODIFIER_EXTRUSION",                      /* 1316 */
  "AEC_DISP_REP_COL_MODIFIER_GROUP",                          /* 1317 */
  "AEC_DISP_REP_COL_MODIFIER_LOFT",                           /* 1318 */
  "AEC_DISP_REP_COL_MODIFIER_PATH",                           /* 1319 */
  "AEC_DISP_REP_COL_MODIFIER_REVOLVE",                        /* 1320 */
  "AEC_DISP_REP_COL_MODIFIER_SUBTRACT",                       /* 1321 */
  "AEC_DISP_REP_COL_MODIFIER_TRANSITION",                     /* 1322 */
  "AEC_DISP_REP_COL_POINT2D",                                 /* 1323 */
  "AEC_DISP_REP_COL_PROFILE",                                 /* 1324 */
  "AEC_DISP_REP_COLUMN_GRID",                                 /* 1325 */
  "AEC_DISP_REP_COLUMN_GRID_MODEL",                           /* 1326 */
  "AEC_DISP_REP_COLUMN_GRID_RCP",                             /* 1327 */
  "AEC_DISP_REP_COL_WORKPLANE",                               /* 1328 */
  "AEC_DISP_REP_COL_WORKPLANE_REF",                           /* 1329 */
  "AEC_DISP_REP_CONFIG",                                      /* 1330 */
  "AEC_DISP_REP_CURTAIN_WALL_LAYOUT_MODEL",                   /* 1331 */
  "AEC_DISP_REP_CURTAIN_WALL_LAYOUT_PLAN",                    /* 1332 */
  "AEC_DISP_REP_CURTAIN_WALL_UNIT_MODEL",                     /* 1333 */
  "AEC_DISP_REP_CURTAIN_WALL_UNIT_PLAN",                      /* 1334 */
  "AEC_DISP_REP_DCM_DIMRADIUS",                               /* 1335 */
  "AEC_DISP_REP_DISPLAYTHEME",                                /* 1336 */
  "AEC_DISP_REP_DOOR_ELEV",                                   /* 1337 */
  "AEC_DISP_REP_DOOR_MODEL",                                  /* 1338 */
  "AEC_DISP_REP_DOOR_NOMINAL",                                /* 1339 */
  "AEC_DISP_REP_DOOR_PLAN",                                   /* 1340 */
  "AEC_DISP_REP_DOOR_PLAN_50",                                /* 1341 */
  "AEC_DISP_REP_DOOR_PLAN_HEKTO",                             /* 1342 */
  "AEC_DISP_REP_DOOR_RCP",                                    /* 1343 */
  "AEC_DISP_REP_DOOR_THRESHOLD_PLAN",                         /* 1344 */
  "AEC_DISP_REP_DOOR_THRESHOLD_SYMBOL_PLAN",                  /* 1345 */
  "AEC_DISP_REP_EDITINPLACEPROFILE",                          /* 1346 */
  "AEC_DISP_REP_ENT_REF",                                     /* 1347 */
  "AEC_DISP_REP_LAYOUT_CURVE",                                /* 1348 */
  "AEC_DISP_REP_LAYOUT_GRID2D",                               /* 1349 */
  "AEC_DISP_REP_LAYOUT_GRID3D",                               /* 1350 */
  "AEC_DISP_REP_MASKBLOCK_REF",                               /* 1351 */
  "AEC_DISP_REP_MASKBLOCK_REF_RCP",                           /* 1352 */
  "AEC_DISP_REP_MASS_ELEM_MODEL",                             /* 1353 */
  "AEC_DISP_REP_MASS_ELEM_RCP",                               /* 1354 */
  "AEC_DISP_REP_MASS_ELEM_SCHEM",                             /* 1355 */
  "AEC_DISP_REP_MASS_GROUP_MODEL",                            /* 1356 */
  "AEC_DISP_REP_MASS_GROUP_PLAN",                             /* 1357 */
  "AEC_DISP_REP_MASS_GROUP_RCP",                              /* 1358 */
  "AEC_DISP_REP_MATERIAL",                                    /* 1359 */
  "AEC_DISP_REP_MVBLOCK_REF",                                 /* 1360 */
  "AEC_DISP_REP_MVBLOCK_REF_MODEL",                           /* 1361 */
  "AEC_DISP_REP_MVBLOCK_REF_RCP",                             /* 1362 */
  "AEC_DISP_REP_OPENING",                                     /* 1363 */
  "AEC_DISP_REP_OPENING_MODEL",                               /* 1364 */
  "AEC_DISP_REP_POLYGON_MODEL",                               /* 1365 */
  "AEC_DISP_REP_POLYGON_TRUECOLOUR",                          /* 1366 */
  "AEC_DISP_REP_RAILING_MODEL",                               /* 1367 */
  "AEC_DISP_REP_RAILING_PLAN",                                /* 1368 */
  "AEC_DISP_REP_RAILING_PLAN_100",                            /* 1369 */
  "AEC_DISP_REP_RAILING_PLAN_50",                             /* 1370 */
  "AEC_DISP_REP_ROOF_MODEL",                                  /* 1371 */
  "AEC_DISP_REP_ROOF_PLAN",                                   /* 1372 */
  "AEC_DISP_REP_ROOF_RCP",                                    /* 1373 */
  "AEC_DISP_REP_ROOFSLAB_MODEL",                              /* 1374 */
  "AEC_DISP_REP_ROOFSLAB_PLAN",                               /* 1375 */
  "AEC_DISP_REP_SCHEDULE_TABLE",                              /* 1376 */
  "AEC_DISP_REP_SET",                                         /* 1377 */
  "AEC_DISP_REP_SLAB_MODEL",                                  /* 1378 */
  "AEC_DISP_REP_SLAB_PLAN",                                   /* 1379 */
  "AEC_DISP_REP_SLICE",                                       /* 1380 */
  "AEC_DISP_REP_SPACE_DECOMPOSED",                            /* 1381 */
  "AEC_DISP_REP_SPACE_MODEL",                                 /* 1382 */
  "AEC_DISP_REP_SPACE_PLAN",                                  /* 1383 */
  "AEC_DISP_REP_SPACE_RCP",                                   /* 1384 */
  "AEC_DISP_REP_SPACE_VOLUME",                                /* 1385 */
  "AEC_DISP_REP_STAIR_MODEL",                                 /* 1386 */
  "AEC_DISP_REP_STAIR_PLAN",                                  /* 1387 */
  "AEC_DISP_REP_STAIR_PLAN_100",                              /* 1388 */
  "AEC_DISP_REP_STAIR_PLAN_50",                               /* 1389 */
  "AEC_DISP_REP_STAIR_PLAN_OVERLAPPING",                      /* 1390 */
  "AEC_DISP_REP_STAIR_RCP",                                   /* 1391 */
  "AEC_DISP_REP_WALL_GRAPH",                                  /* 1392 */
  "AEC_DISP_REP_WALL_MODEL",                                  /* 1393 */
  "AEC_DISP_REP_WALL_PLAN",                                   /* 1394 */
  "AEC_DISP_REP_WALL_RCP",                                    /* 1395 */
  "AEC_DISP_REP_WALL_SCHEM",                                  /* 1396 */
  "AEC_DISP_REP_WINDOW_ASSEMBLY_MODEL",                       /* 1397 */
  "AEC_DISP_REP_WINDOW_ASSEMBLY_PLAN",                        /* 1398 */
  "AEC_DISP_REP_WINDOWASSEMBLY_SILL_PLAN",                    /* 1399 */
  "AEC_DISP_REP_WINDOW_ELEV",                                 /* 1400 */
  "AEC_DISP_REP_WINDOW_MODEL",                                /* 1401 */
  "AEC_DISP_REP_WINDOW_NOMINAL",                              /* 1402 */
  "AEC_DISP_REP_WINDOW_PLAN",                                 /* 1403 */
  "AEC_DISP_REP_WINDOW_PLAN_100",                             /* 1404 */
  "AEC_DISP_REP_WINDOW_PLAN_50",                              /* 1405 */
  "AEC_DISP_REP_WINDOW_RCP",                                  /* 1406 */
  "AEC_DISP_REP_WINDOW_SILL_PLAN",                            /* 1407 */
  "AEC_DISP_REP_ZONE",                                        /* 1408 */
  "AEC_DISPROPSMASSELEMPLANCOMMON",                           /* 1409 */
  "AEC_DISPROPSMASSGROUPPLANCOMMON",                          /* 1410 */
  "AEC_DISPROPSOPENINGPLANCOMMON",                            /* 1411 */
  "AEC_DISPROPSOPENINGPLANCOMMONHATCHED",                     /* 1412 */
  "AEC_DISPROPSOPENINGSILLPLAN",                              /* 1413 */
  "AEC_DISP_ROPS_RAILING_PLAN_100",                           /* 1414 */
  "AEC_DISP_ROPS_RAILING_PLAN_50",                            /* 1415 */
  "AEC_DISP_ROPS_STAIR_PLAN_100",                             /* 1416 */
  "AEC_DISP_ROPS_STAIR_PLAN_50",                              /* 1417 */
  "AEC_DOOR_STYLE",                                           /* 1418 */
  "AEC_ENDCAP_STYLE",                                         /* 1419 */
  "AEC_FRAME_DEF",                                            /* 1420 */
  "AEC_LAYERKEY_STYLE",                                       /* 1421 */
  "AEC_LIST_DEF",                                             /* 1422 */
  "AEC_MASKBLOCK_DEF",                                        /* 1423 */
  "AEC_MASS_ELEM_STYLE",                                      /* 1424 */
  "AEC_MATERIAL_DEF",                                         /* 1425 */
  "AEC_MVBLOCK_DEF",                                          /* 1426 */
  "AEC_MVBLOCK_REF",                                          /* 1427 */
  "AEC_NOTIFICATION_TRACKER",                                 /* 1428 */
  "AEC_POLYGON",                                              /* 1429 */
  "AEC_POLYGON_STYLE",                                        /* 1430 */
  "AEC_PROPERTY_SET_DEF",                                     /* 1431 */
  "AEC_RAILING_STYLE",                                        /* 1432 */
  "AEC_REFEDIT_STATUS_TRACKER",                               /* 1433 */
  "AEC_ROOFSLABEDGE_STYLE",                                   /* 1434 */
  "AEC_ROOFSLAB_STYLE",                                       /* 1435 */
  "AEC_SCHEDULE_DATA_FORMAT",                                 /* 1436 */
  "AEC_SLABEDGE_STYLE",                                       /* 1437 */
  "AEC_SLAB_STYLE",                                           /* 1438 */
  "AEC_SPACE_STYLES",                                         /* 1439 */
  "AEC_STAIR_STYLE",                                          /* 1440 */
  "AEC_STAIR_WINDER_STYLE",                                   /* 1441 */
  "AEC_STAIR_WINDER_TYPE_BALANCED",                           /* 1442 */
  "AEC_STAIR_WINDER_TYPE_MANUAL",                             /* 1443 */
  "AEC_STAIR_WINDER_TYPE_SINGLE_POINT",                       /* 1444 */
  "AEC_VARS_AECBBLDSRV",                                      /* 1445 */
  "AEC_VARS_ARCHBASE",                                        /* 1446 */
  "AEC_VARS_DWG_SETUP",                                       /* 1447 */
  "AEC_VARS_MUNICH",                                          /* 1448 */
  "AEC_VARS_STRUCTUREBASE",                                   /* 1449 */
  "AEC_WALLMOD_STYLE",                                        /* 1450 */
  "AEC_WALL_STYLE",                                           /* 1451 */
  "AEC_WINDOW_ASSEMBLY_STYLE",                                /* 1452 */
  "AEC_WINDOW_STYLE",                                         /* 1453 */
  "ALIGNMENTGRIPENTITY",                                      /* 1454 */
  "AMCONTEXTMGR",                                             /* 1455 */
  "AMDTADMENUSTATE",                                          /* 1456 */
  "AMDTAMMENUSTATE",                                          /* 1457 */
  "AMDTBROWSERDBTAB",                                         /* 1458 */
  "AMDTDMMENUSTATE",                                          /* 1459 */
  "AMDTEDGESTANDARDDIN",                                      /* 1460 */
  "AMDTEDGESTANDARDDIN13715",                                 /* 1461 */
  "AMDTEDGESTANDARDISO",                                      /* 1462 */
  "AMDTEDGESTANDARDISO13715",                                 /* 1463 */
  "AMDTFORMULAUPDATEDISPATCHER",                              /* 1464 */
  "AMDTINTERNALREACTOR",                                      /* 1465 */
  "AMDTMCOMMENUSTATE",                                        /* 1466 */
  "AMDTMENUSTATEMGR",                                         /* 1467 */
  "AMDTNOTE",                                                 /* 1468 */
  "AMDTNOTETEMPLATEDB",                                       /* 1469 */
  "AMDTSECTIONSYM",                                           /* 1470 */
  "AMDTSECTIONSYMLABEL",                                      /* 1471 */
  "AMDTSYSATTR",                                              /* 1472 */
  "AMGOBJPROPCFG",                                            /* 1473 */
  "AMGSETTINGSOBJ",                                           /* 1474 */
  "AMIMASTER",                                                /* 1475 */
  "AM_DRAWING_MGR",                                           /* 1476 */
  "AM_DWGMGR_NAME",                                           /* 1477 */
  "AM_DWG_DOCUMENT",                                          /* 1478 */
  "AM_DWG_SHEET",                                             /* 1479 */
  "AM_VIEWDIMPARMAP",                                         /* 1480 */
  "BINRECORD",                                                /* 1481 */
  "CAMSCATALOGAPPOBJECT",                                     /* 1482 */
  "CAMSSTRUCTBTNSTATE",                                       /* 1483 */
  "CATALOGSTATE",                                             /* 1484 */
  "CBROWSERAPPOBJECT",                                        /* 1485 */
  "DEPMGR",                                                   /* 1486 */
  "DMBASEELEMENT",                                            /* 1487 */
  "DMDEFAULTSTYLE",                                           /* 1488 */
  "DMLEGEND",                                                 /* 1489 */
  "DMMAP",                                                    /* 1490 */
  "DMMAPMANAGER",                                             /* 1491 */
  "DMSTYLECATEGORY",                                          /* 1492 */
  "DMSTYLELIBRARY",                                           /* 1493 */
  "DMSTYLEREFERENCE",                                         /* 1494 */
  "DMSTYLIZEDENTITIESTABLE",                                  /* 1495 */
  "DMSURROGATESTYLESETS",                                     /* 1496 */
  "DM_PLACEHOLDER",                                           /* 1497 */
  "EXACTERMXREFMAP",                                          /* 1498 */
  "EXACXREFPANELOBJECT",                                      /* 1499 */
  "EXPO_NOTIFYBLOCK",                                         /* 1500 */
  "EXPO_NOTIFYHALL",                                          /* 1501 */
  "EXPO_NOTIFYPILLAR",                                        /* 1502 */
  "EXPO_NOTIFYSTAND",                                         /* 1503 */
  "EXPO_NOTIFYSTANDNOPOLY",                                   /* 1504 */
  "GSMANAGER",                                                /* 1505 */
  "IRD_DSC_DICT",                                             /* 1506 */
  "IRD_DSC_RECORD",                                           /* 1507 */
  "IRD_OBJ_RECORD",                                           /* 1508 */
  "MAPFSMRVOBJECT",                                           /* 1509 */
  "MAPGWSUNDOOBJECT",                                         /* 1510 */
  "MAPIAMMOUDLE",                                             /* 1511 */
  "MAPMETADATAOBJECT",                                        /* 1512 */
  "MAPRESOURCEMANAGEROBJECT",                                 /* 1513 */
  "McDbContainer2",                                           /* 1514 */
  "McDbMarker",                                               /* 1515 */
  "NAMEDAPPL",                                                /* 1516 */
  "NEWSTDPARTPARLIST",                                        /* 1517 */
  "NOTEPOSITION",                                             /* 1518 */
  "OBJCLONER",                                                /* 1519 */
  "PARAMMGR",                                                 /* 1520 */
  "PARAMSCOPE",                                               /* 1521 */
  "PILLAR",                                                   /* 1522 */
  "STDPART2D",                                                /* 1523 */
  "TCH_ARROW",                                                /* 1524 */
  "TCH_AXIS_LABEL",                                           /* 1525 */
  "TCH_BLOCK_INSERT",                                         /* 1526 */
  "TCH_COLUMN",                                               /* 1527 */
  "TCH_DBCONFIG",                                             /* 1528 */
  "TCH_DIMENSION2",                                           /* 1529 */
  "TCH_DRAWINGINDEX",                                         /* 1530 */
  "TCH_HANDRAIL",                                             /* 1531 */
  "TCH_LINESTAIR",                                            /* 1532 */
  "TCH_OPENING",                                              /* 1533 */
  "TCH_RECTSTAIR",                                            /* 1534 */
  "TCH_SLAB",                                                 /* 1535 */
  "TCH_SPACE",                                                /* 1536 */
  "TCH_TEXT",                                                 /* 1537 */
  "TCH_WALL",                                                 /* 1538 */
  "TGrupoPuntos",                                             /* 1539 */
  "VAACIMAGEINVENTORY",                                       /* 1540 */
  "VAACXREFPANELOBJECT",                                      /* 1541 */
  // clang-format: on
  /* End auto-generated dxfnames */
};

/* Find the dxfname for our internal object/entity name.
 * The reverse of the _alias() functions, replaces dxf_encode_alias (name).
 * Only needed for the add API.
 * Some names return either NULL or one of many possibilities,
 * like UNDERLAY => PDFUNDERLAY.
 */
const char *
dwg_type_dxfname (const Dwg_Object_Type type)
{
  if (type <= DWG_TYPE_LAYOUT)
    return _dwg_dxfnames_fixed[type];
  else if ((unsigned)type >= 500 && type <= DWG_TYPE_XYGRIPENTITY)
    return _dwg_dxfnames_variable[type - 500];
  // DWG_TYPE_PROXY_ENTITY = 0x1f2, /* 498 */
  // DWG_TYPE_PROXY_OBJECT = 0x1f3, /* 499 */
  else if (type == DWG_TYPE_PROXY_ENTITY)
    return "ACAD_PROXY_ENTITY";
  else if (type == DWG_TYPE_PROXY_OBJECT)
    return "ACAD_PROXY_OBJECT";
  else if (type == DWG_TYPE_FREED || type == DWG_TYPE_UNKNOWN_ENT
           || type == DWG_TYPE_UNKNOWN_OBJ)
    return NULL;
  else
    {
      // LOG_ERROR ("Unknown object type %d", type)
      return NULL;
    }
}

// dxfname => name
void
object_alias (char *restrict name)
{
  const size_t len = strlen (name);
  // check aliases (dxfname => name)
  if (strEQc (name, "ACAD_PROXY_OBJECT"))
    strcpy (name, "PROXY_OBJECT");
  else if (strEQc (name, "ACDBPERSSUBENTMANAGER"))
    strcpy (name, "PERSUBENTMGR");
  else if (strEQc (name, "ACDB_DYNAMICBLOCKPURGEPREVENTER_VERSION"))
    strcpy (name, "DYNAMICBLOCKPURGEPREVENTER");
  else if (strEQc (name, "EXACXREFPANELOBJECT"))
    strcpy (name, "XREFPANELOBJECT");
  else if (strstr (name, "_BACKGROUND"))
    strcpy (name, "BACKGROUND");
  // ACDB_*OBJECTCONTEXTDATA_CLASS => *OBJECTCONTEXTDATA
  else if (memBEGINc (name, "ACAD_") && len > 28
           && strEQc (&name[len - 6], "_CLASS"))
    {
      name[len - 6] = '\0';
      if (is_dwg_object (&name[5]))
        memmove (name, &name[5], len - 4);
      else
        name[len - 6] = '_';
    }
  // strip ACAD_ prefix
  else if (memBEGINc (name, "ACAD_") && is_dwg_object (&name[5]))
    memmove (name, &name[5], len - 4);
  // strip the ACDB prefix
  else if (memBEGINc (name, "ACDB") && is_dwg_object (&name[4]))
    memmove (name, &name[4], len - 3);
}

// dxfname => name
void
entity_alias (char *restrict name)
{
  const size_t len = strlen (name);
  // check aliases (dxfname => name)
  if (strEQc (name, "ACAD_TABLE"))
    strcpy (name, "TABLE");
  else if (strEQc (name, "ACAD_PROXY_ENTITY"))
    strcpy (name, "PROXY_ENTITY");
  /* else if (strEQc (name, "ACAD_PROXY_ENTITY_WRAPPER"))
    strcpy (name, "PROXY_ENTITY_WRAPPER"); */
  else if (strEQc (name, "ACDBPLACEHOLDER"))
    strcpy (name, "PLACEHOLDER");
  else if (strEQc (name, "POLYLINE"))
    strcpy (name, "POLYLINE_2D"); // other POLYLINE_* by flag or subclass?
  else if (strEQc (name, "VERTEX"))
    strcpy (name, "VERTEX_2D"); // other VERTEX_* by flag?
  else if (strEQc (name, "SURFACE"))
    strcpy (name, "PLANESURFACE");
  else if (strEQc (name, "COORDINATION_MODEL")) //??
    strcpy (name, "NAVISWORKSMODEL");
  else if (strEQc (name, "POSITIONMARKER"))
    strcpy (name, "GEOPOSITIONMARKER");
  // if (strEQc (name, "BLOCK"))
  //  strcpy (name, "BLOCK_HEADER");
  // else if (strEQc (name, "VERTEX_MESH") || strEQc (name, "VERTEX_PFACE"))
  //  strcpy (name, "VERTEX_3D");
  // else if (strEQc (name, "DIMENSION"))
  //  strcpy (name, "DIMENSION_ANG2LN");   // allocate room for the largest
  // strip a ACAD_ prefix
  else if (memBEGINc (name, "ACAD_") && is_dwg_entity (&name[5]))
    memmove (name, &name[5], len - 4);
  // strip the ACDB prefix
  else if (memBEGINc (name, "ACDB") && is_dwg_entity (&name[4]))
    memmove (name, &name[4], len - 3);
}

bool
has_entity_DD (const Dwg_Object *obj)
{
  const Dwg_Version_Type version = obj->parent->header.version;
  const Dwg_Object_Type type = obj->fixedtype;
  if (version < R_2000)
    return false;
  if (type == DWG_TYPE_LWPOLYLINE || type == DWG_TYPE_LINE
      || type == DWG_TYPE_INSERT || type == DWG_TYPE_MINSERT
      || type == DWG_TYPE_TABLE)
    return true;
  return false;
}

bool
obj_has_strings (const Dwg_Object *obj)
{
  const Dwg_Version_Type version = obj->parent->header.from_version;
  const Dwg_Object_Type type = obj->fixedtype;
  if (version >= R_2007)
    return obj->has_strings ? true : false;
  if (dwg_obj_is_table (obj))
    return true;
  if (type == DWG_TYPE_TEXT || type == DWG_TYPE_ATTRIB
      || type == DWG_TYPE_ATTDEF || type == DWG_TYPE_BLOCK
      || type == DWG_TYPE_DIMENSION_ORDINATE
      || type == DWG_TYPE_DIMENSION_LINEAR
      || type == DWG_TYPE_DIMENSION_ALIGNED
      || type == DWG_TYPE_DIMENSION_ANG3PT || type == DWG_TYPE_DIMENSION_ANG2LN
      || type == DWG_TYPE_DIMENSION_RADIUS
      || type == DWG_TYPE_DIMENSION_DIAMETER
      || type == DWG_TYPE_LARGE_RADIAL_DIMENSION
      || type == DWG_TYPE_ARC_DIMENSION || type == DWG_TYPE_SHAPE
      || type == DWG_TYPE_VIEWPORT || type == DWG_TYPE__3DSOLID
      || type == DWG_TYPE_REGION || type == DWG_TYPE_BODY
      || type == DWG_TYPE_DICTIONARY || type == DWG_TYPE_DICTIONARYVAR
      || type == DWG_TYPE_DICTIONARYWDFLT || type == DWG_TYPE_MTEXT
      || type == DWG_TYPE_TOLERANCE || type == DWG_TYPE_BLOCK_HEADER
      || type == DWG_TYPE_GROUP || type == DWG_TYPE_MLINESTYLE
      || type == DWG_TYPE_HATCH || type == DWG_TYPE_MPOLYGON
      || type == DWG_TYPE_IMAGEDEF || type == DWG_TYPE_LAYER_INDEX
      || type == DWG_TYPE_PLOTSETTINGS || type == DWG_TYPE_LAYOUT
      || type == DWG_TYPE_OLE2FRAME || type == DWG_TYPE_FIELD
      || type == DWG_TYPE_GEODATA || type == DWG_TYPE_TABLECONTENT
      || type == DWG_TYPE_TABLE || type == DWG_TYPE_TABLESTYLE
      || type == DWG_TYPE_CELLSTYLEMAP || type == DWG_TYPE_SCALE
      || type == DWG_TYPE_MULTILEADER || type == DWG_TYPE_MLEADERSTYLE
      || type == DWG_TYPE_PDFDEFINITION || type == DWG_TYPE_DGNDEFINITION
      || type == DWG_TYPE_DWFDEFINITION || type == DWG_TYPE_SECTIONOBJECT
      || type == DWG_TYPE_SECTION_SETTINGS || type == DWG_TYPE_VISUALSTYLE
      || type == DWG_TYPE_LIGHT || type == DWG_TYPE_LIGHTLIST
      || type == DWG_TYPE_SUN || type == DWG_TYPE_RENDERSETTINGS
      || type == DWG_TYPE_MENTALRAYRENDERSETTINGS
      || type == DWG_TYPE_RAPIDRTRENDERSETTINGS || type == DWG_TYPE_DBCOLOR
      || type == DWG_TYPE_MATERIAL || type == DWG_TYPE_LAYERFILTER
      || type == DWG_TYPE_ASSOCDEPENDENCY
      || type == DWG_TYPE_ASSOCEDGEACTIONPARAM
      || type == DWG_TYPE_CONTEXTDATAMANAGER
      || (type >= DWG_TYPE_ACSH_BOOLEAN_CLASS
          && type <= DWG_TYPE_ACSH_WEDGE_CLASS)
      || (type >= DWG_TYPE_ASSOC2DCONSTRAINTGROUP
          && type <= DWG_TYPE_ASSOCVERTEXACTIONPARAM)
      || dwg_has_subclass (obj->name, "AcDbEvalVariant")
      || dwg_has_subclass (obj->name, "AcDbValueParam")
      || dwg_has_subclass (obj->name, "AcDbAssocActionParam")
      || dwg_has_subclass (obj->name, "AcDbAssocDependency")
      || dwg_has_subclass (obj->name, "AcDbEvalExpr")
      || dwg_has_subclass (obj->name, "AdDbAssocIndexPersSubentId")
      || dwg_has_subclass (obj->name, "AcDbAssocPersSubentId")
      || dwg_has_subclass (obj->name, "AcDbAssocEdgePersSubentId")
      || dwg_has_subclass (obj->name, "AcDbBlockElement")
      || dwg_has_subclass (obj->name, "AcDbBlockGripExpr")
      || type == DWG_TYPE_DIMASSOC || type == DWG_TYPE_SUNSTUDY
      || type == DWG_TYPE_GEOPOSITIONMARKER
      || type == DWG_TYPE_NAVISWORKSMODELDEF || type == DWG_TYPE_DATATABLE
      || type == DWG_TYPE_DATALINK || type == DWG_TYPE_DETAILVIEWSTYLE
      || type == DWG_TYPE_SECTIONVIEWSTYLE || type == DWG_TYPE_IBL_BACKGROUND
      || type == DWG_TYPE_IMAGE_BACKGROUND || type == DWG_TYPE_RENDERENTRY
      || type == DWG_TYPE_RENDERENVIRONMENT || type == DWG_TYPE_RENDERGLOBAL
      || type == DWG_TYPE_ARCALIGNEDTEXT || type == DWG_TYPE_RTEXT
      || type == DWG_TYPE_POINTCLOUD || type == DWG_TYPE_POINTCLOUDEX
      || type == DWG_TYPE_POINTCLOUDDEF || type == DWG_TYPE_POINTCLOUDDEFEX
      || type == DWG_TYPE_POINTCLOUDCOLORMAP || type == DWG_TYPE_ASSOCVARIABLE
      || dwg_has_subclass (obj->name, "AcDbCompoundObjectId")
      || type == DWG_TYPE_BLOCKPOLARPARAMETER
      || type == DWG_TYPE_ASSOCDIMDEPENDENCYBODY
      || type == DWG_TYPE_BLOCKANGULARCONSTRAINTPARAMETER
      || dwg_has_subclass (obj->name, "AcDbAssocArrayActionBody")
      || dwg_has_subclass (obj->name, "AcDbAssocArrayCommonParameters")
      || dwg_has_subclass (obj->name, "AcDbImpAssocDimDependencyBodyBase")
      || dwg_has_subclass (obj->name, "AcDbBlockConstraintParameter")
      || type == DWG_TYPE_BLOCKLOOKUPACTION
      || type == DWG_TYPE_BLOCKLOOKUPPARAMETER
      || type == DWG_TYPE_BLOCKPOINTPARAMETER
      || type == DWG_TYPE_BLOCKUSERPARAMETER || 0)
    return true;
  return false;
}
