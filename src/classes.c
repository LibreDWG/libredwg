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
#include "classes.h"
#include "dynapi.h"
#include "dec_macros.h"
#include "encode.h"
#define DWG_LOGLEVEL loglevel
#include "logging.h"

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
  "ACIDBLOCKREFERENCE",                                       /* 502 */
  "ACMECOMMANDHISTORY",                                       /* 503 */
  "ACMESCOPE",                                                /* 504 */
  "ACMESTATEMGR",                                             /* 505 */
  "ACSH_BOOLEAN_CLASS",                                       /* 506 */
  "ACSH_BOX_CLASS",                                           /* 507 */
  "ACSH_BREP_CLASS",                                          /* 508 */
  "ACSH_CHAMFER_CLASS",                                       /* 509 */
  "ACSH_CONE_CLASS",                                          /* 510 */
  "ACSH_CYLINDER_CLASS",                                      /* 511 */
  "ACSH_EXTRUSION_CLASS",                                     /* 512 */
  "ACSH_FILLET_CLASS",                                        /* 513 */
  "ACSH_HISTORY_CLASS",                                       /* 514 */
  "ACSH_LOFT_CLASS",                                          /* 515 */
  "ACSH_PYRAMID_CLASS",                                       /* 516 */
  "ACSH_REVOLVE_CLASS",                                       /* 517 */
  "ACSH_SPHERE_CLASS",                                        /* 518 */
  "ACSH_SWEEP_CLASS",                                         /* 519 */
  "ACSH_TORUS_CLASS",                                         /* 520 */
  "ACSH_WEDGE_CLASS",                                         /* 521 */
  "ALDIMOBJECTCONTEXTDATA",                                   /* 522 */
  "ALIGNMENTPARAMETERENTITY",                                 /* 523 */
  "ANGDIMOBJECTCONTEXTDATA",                                  /* 524 */
  "ANNOTSCALEOBJECTCONTEXTDATA",                              /* 525 */
  "ARCALIGNEDTEXT",                                           /* 526 */
  "ARC_DIMENSION",                                            /* 527 */
  "ASSOC2DCONSTRAINTGROUP",                                   /* 528 */
  "ASSOC3POINTANGULARDIMACTIONBODY",                          /* 529 */
  "ASSOCACTION",                                              /* 530 */
  "ASSOCACTIONPARAM",                                         /* 531 */
  "ASSOCALIGNEDDIMACTIONBODY",                                /* 532 */
  "ASSOCARRAYACTIONBODY",                                     /* 533 */
  "ASSOCARRAYMODIFYACTIONBODY",                               /* 534 */
  "ASSOCARRAYMODIFYPARAMETERS",                               /* 535 */
  "ASSOCARRAYPATHPARAMETERS",                                 /* 536 */
  "ASSOCARRAYPOLARPARAMETERS",                                /* 537 */
  "ASSOCARRAYRECTANGULARPARAMETERS",                          /* 538 */
  "ASSOCASMBODYACTIONPARAM",                                  /* 539 */
  "ASSOCBLENDSURFACEACTIONBODY",                              /* 540 */
  "ASSOCCOMPOUNDACTIONPARAM",                                 /* 541 */
  "ASSOCDEPENDENCY",                                          /* 542 */
  "ASSOCDIMDEPENDENCYBODY",                                   /* 543 */
  "ASSOCEDGEACTIONPARAM",                                     /* 544 */
  "ASSOCEDGECHAMFERACTIONBODY",                               /* 545 */
  "ASSOCEDGEFILLETACTIONBODY",                                /* 546 */
  "ASSOCEXTENDSURFACEACTIONBODY",                             /* 547 */
  "ASSOCEXTRUDEDSURFACEACTIONBODY",                           /* 548 */
  "ASSOCFACEACTIONPARAM",                                     /* 549 */
  "ASSOCFILLETSURFACEACTIONBODY",                             /* 550 */
  "ASSOCGEOMDEPENDENCY",                                      /* 551 */
  "ASSOCLOFTEDSURFACEACTIONBODY",                             /* 552 */
  "ASSOCMLEADERACTIONBODY",                                   /* 553 */
  "ASSOCNETWORK",                                             /* 554 */
  "ASSOCNETWORKSURFACEACTIONBODY",                            /* 555 */
  "ASSOCOBJECTACTIONPARAM",                                   /* 556 */
  "ASSOCOFFSETSURFACEACTIONBODY",                             /* 557 */
  "ASSOCORDINATEDIMACTIONBODY",                               /* 558 */
  "ASSOCOSNAPPOINTREFACTIONPARAM",                            /* 559 */
  "ASSOCPATCHSURFACEACTIONBODY",                              /* 560 */
  "ASSOCPATHACTIONPARAM",                                     /* 561 */
  "ASSOCPERSSUBENTMANAGER",                                   /* 562 */
  "ASSOCPLANESURFACEACTIONBODY",                              /* 563 */
  "ASSOCPOINTREFACTIONPARAM",                                 /* 564 */
  "ASSOCRESTOREENTITYSTATEACTIONBODY",                        /* 565 */
  "ASSOCREVOLVEDSURFACEACTIONBODY",                           /* 566 */
  "ASSOCROTATEDDIMACTIONBODY",                                /* 567 */
  "ASSOCSWEPTSURFACEACTIONBODY",                              /* 568 */
  "ASSOCTRIMSURFACEACTIONBODY",                               /* 569 */
  "ASSOCVALUEDEPENDENCY",                                     /* 570 */
  "ASSOCVARIABLE",                                            /* 571 */
  "ASSOCVERTEXACTIONPARAM",                                   /* 572 */
  "BASEPOINTPARAMETERENTITY",                                 /* 573 */
  "BLKREFOBJECTCONTEXTDATA",                                  /* 574 */
  "BLOCKALIGNEDCONSTRAINTPARAMETER",                          /* 575 */
  "BLOCKALIGNMENTGRIP",                                       /* 576 */
  "BLOCKALIGNMENTPARAMETER",                                  /* 577 */
  "BLOCKANGULARCONSTRAINTPARAMETER",                          /* 578 */
  "BLOCKARRAYACTION",                                         /* 579 */
  "BLOCKBASEPOINTPARAMETER",                                  /* 580 */
  "BLOCKDIAMETRICCONSTRAINTPARAMETER",                        /* 581 */
  "BLOCKFLIPACTION",                                          /* 582 */
  "BLOCKFLIPGRIP",                                            /* 583 */
  "BLOCKFLIPPARAMETER",                                       /* 584 */
  "BLOCKGRIPLOCATIONCOMPONENT",                               /* 585 */
  "BLOCKHORIZONTALCONSTRAINTPARAMETER",                       /* 586 */
  "BLOCKLINEARCONSTRAINTPARAMETER",                           /* 587 */
  "BLOCKLINEARGRIP",                                          /* 588 */
  "BLOCKLINEARPARAMETER",                                     /* 589 */
  "BLOCKLOOKUPACTION",                                        /* 590 */
  "BLOCKLOOKUPGRIP",                                          /* 591 */
  "BLOCKLOOKUPPARAMETER",                                     /* 592 */
  "BLOCKMOVEACTION",                                          /* 593 */
  "BLOCKPARAMDEPENDENCYBODY",                                 /* 594 */
  "BLOCKPOINTPARAMETER",                                      /* 595 */
  "BLOCKPOLARGRIP",                                           /* 596 */
  "BLOCKPOLARPARAMETER",                                      /* 597 */
  "BLOCKPOLARSTRETCHACTION",                                  /* 598 */
  "BLOCKPROPERTIESTABLE",                                     /* 599 */
  "BLOCKPROPERTIESTABLEGRIP",                                 /* 600 */
  "BLOCKRADIALCONSTRAINTPARAMETER",                           /* 601 */
  "BLOCKREPRESENTATION",                                      /* 602 */
  "BLOCKROTATEACTION",                                        /* 603 */
  "BLOCKROTATIONGRIP",                                        /* 604 */
  "BLOCKROTATIONPARAMETER",                                   /* 605 */
  "BLOCKSCALEACTION",                                         /* 606 */
  "BLOCKSTRETCHACTION",                                       /* 607 */
  "BLOCKUSERPARAMETER",                                       /* 608 */
  "BLOCKVERTICALCONSTRAINTPARAMETER",                         /* 609 */
  "BLOCKVISIBILITYGRIP",                                      /* 610 */
  "BLOCKVISIBILITYPARAMETER",                                 /* 611 */
  "BLOCKXYGRIP",                                              /* 612 */
  "BLOCKXYPARAMETER",                                         /* 613 */
  "CAMERA",                                                   /* 614 */
  "CELLSTYLEMAP",                                             /* 615 */
  "CONTEXTDATAMANAGER",                                       /* 616 */
  "CSACDOCUMENTOPTIONS",                                      /* 617 */
  "CURVEPATH",                                                /* 618 */
  "DATALINK",                                                 /* 619 */
  "DATATABLE",                                                /* 620 */
  "DBCOLOR",                                                  /* 621 */
  "DETAILVIEWSTYLE",                                          /* 622 */
  "DGNDEFINITION",                                            /* 623 */
  "DGNUNDERLAY",                                              /* 624 */
  "DICTIONARYVAR",                                            /* 625 */
  "DICTIONARYWDFLT",                                          /* 626 */
  "DIMASSOC",                                                 /* 627 */
  "DMDIMOBJECTCONTEXTDATA",                                   /* 628 */
  "DWFDEFINITION",                                            /* 629 */
  "DWFUNDERLAY",                                              /* 630 */
  "DYNAMICBLOCKPROXYNODE",                                    /* 631 */
  "DYNAMICBLOCKPURGEPREVENTER",                               /* 632 */
  "EVALUATION_GRAPH",                                         /* 633 */
  "EXTRUDEDSURFACE",                                          /* 634 */
  "FCFOBJECTCONTEXTDATA",                                     /* 635 */
  "FIELD",                                                    /* 636 */
  "FIELDLIST",                                                /* 637 */
  "FLIPPARAMETERENTITY",                                      /* 638 */
  "FLIPACTIONENTITY",                                         /* 639 */
  "GEODATA",                                                  /* 640 */
  "GEOMAPIMAGE",                                              /* 641 */
  "GEOPOSITIONMARKER",                                        /* 642 */
  "GRADIENT_BACKGROUND",                                      /* 643 */
  "GROUND_PLANE_BACKGROUND",                                  /* 644 */
  "HELIX",                                                    /* 645 */
  "IBL_BACKGROUND",                                           /* 646 */
  "IDBUFFER",                                                 /* 647 */
  "IMAGE",                                                    /* 648 */
  "IMAGEDEF",                                                 /* 649 */
  "IMAGEDEF_REACTOR",                                         /* 650 */
  "IMAGE_BACKGROUND",                                         /* 651 */
  "INDEX",                                                    /* 652 */
  "LARGE_RADIAL_DIMENSION",                                   /* 653 */
  "LAYER_FILTER",                                             /* 654 */
  "LAYER_INDEX",                                              /* 655 */
  "LAYOUTPRINTCONFIG",                                        /* 656 */
  "LEADEROBJECTCONTEXTDATA",                                  /* 657 */
  "LIGHT",                                                    /* 658 */
  "LIGHTLIST",                                                /* 659 */
  "LINEARPARAMETERENTITY",                                    /* 660 */
  "LOFTEDSURFACE",                                            /* 661 */
  "MATERIAL",                                                 /* 662 */
  "MENTALRAYRENDERSETTINGS",                                  /* 663 */
  "MESH",                                                     /* 664 */
  "MLEADEROBJECTCONTEXTDATA",                                 /* 665 */
  "MLEADERSTYLE",                                             /* 666 */
  "MOVEACTIONENTITY",                                         /* 667 */
  "MOTIONPATH",                                               /* 668 */
  "MPOLYGON",                                                 /* 669 */
  "MTEXTATTRIBUTEOBJECTCONTEXTDATA",                          /* 670 */
  "MTEXTOBJECTCONTEXTDATA",                                   /* 671 */
  "MULTILEADER",                                              /* 672 */
  "NAVISWORKSMODEL",                                          /* 673 */
  "NAVISWORKSMODELDEF",                                       /* 674 */
  "NPOCOLLECTION",                                            /* 675 */
  "NURBSURFACE",                                              /* 676 */
  "OBJECT_PTR",                                               /* 677 */
  "ORDDIMOBJECTCONTEXTDATA",                                  /* 678 */
  "PARTIAL_VIEWING_INDEX",                                    /* 679 */
  "PDFDEFINITION",                                            /* 680 */
  "PDFUNDERLAY",                                              /* 681 */
  "PERSUBENTMGR",                                             /* 682 */
  "PLANESURFACE",                                             /* 683 */
  "PLOTSETTINGS",                                             /* 684 */
  "POINTCLOUD",                                               /* 685 */
  "POINTCLOUDCOLORMAP",                                       /* 686 */
  "POINTCLOUDDEF",                                            /* 687 */
  "POINTCLOUDDEFEX",                                          /* 688 */
  "POINTCLOUDDEF_REACTOR",                                    /* 689 */
  "POINTCLOUDDEF_REACTOR_EX",                                 /* 690 */
  "POINTCLOUDEX",                                             /* 691 */
  "POINTPARAMETERENTITY",                                     /* 692 */
  "POINTPATH",                                                /* 693 */
  "POLARGRIPENTITY",                                          /* 694 */
  "RADIMLGOBJECTCONTEXTDATA",                                 /* 695 */
  "RADIMOBJECTCONTEXTDATA",                                   /* 696 */
  "RAPIDRTRENDERSETTINGS",                                    /* 697 */
  "RASTERVARIABLES",                                          /* 698 */
  "RENDERENTRY",                                              /* 699 */
  "RENDERENVIRONMENT",                                        /* 700 */
  "RENDERGLOBAL",                                             /* 701 */
  "RENDERSETTINGS",                                           /* 702 */
  "REVOLVEDSURFACE",                                          /* 703 */
  "ROTATIONPARAMETERENTITY",                                  /* 704 */
  "ROTATEACTIONENTITY",                                       /* 705 */
  "RTEXT",                                                    /* 706 */
  "SCALE",                                                    /* 707 */
  "SCALEACTIONENTITY",                                        /* 708 */
  "SECTIONOBJECT",                                            /* 709 */
  "SECTIONVIEWSTYLE",                                         /* 710 */
  "SECTION_MANAGER",                                          /* 711 */
  "SECTION_SETTINGS",                                         /* 712 */
  "SKYLIGHT_BACKGROUND",                                      /* 713 */
  "SOLID_BACKGROUND",                                         /* 714 */
  "SORTENTSTABLE",                                            /* 715 */
  "SPATIAL_FILTER",                                           /* 716 */
  "SPATIAL_INDEX",                                            /* 717 */
  "STRETCHACTIONENTITY",                                      /* 718 */
  "SUN",                                                      /* 719 */
  "SUNSTUDY",                                                 /* 720 */
  "SWEPTSURFACE",                                             /* 721 */
  "TABLE",                                                    /* 722 */
  "TABLECONTENT",                                             /* 723 */
  "TABLEGEOMETRY",                                            /* 724 */
  "TABLESTYLE",                                               /* 725 */
  "TEXTOBJECTCONTEXTDATA",                                    /* 726 */
  "TVDEVICEPROPERTIES",                                       /* 727 */
  "VISIBILITYGRIPENTITY",                                     /* 728 */
  "VISIBILITYPARAMETERENTITY",                                /* 729 */
  "VISUALSTYLE",                                              /* 730 */
  "WIPEOUT",                                                  /* 731 */
  "WIPEOUTVARIABLES",                                         /* 732 */
  "XREFPANELOBJECT",                                          /* 733 */
  "XYPARAMETERENTITY",                                        /* 734 */
  "BREAKDATA",                                                /* 735 */
  "BREAKPOINTREF",                                            /* 736 */
  "FLIPGRIPENTITY",                                           /* 737 */
  "LINEARGRIPENTITY",                                         /* 738 */
  "ROTATIONGRIPENTITY",                                       /* 739 */
  "XYGRIPENTITY",                                             /* 740 */
  "3DLINE",                                                   /* 741 */
  "REPEAT",                                                   /* 742 */
  "ENDREP",                                                   /* 743 */
  "JUMP",                                                     /* 744 */
  "LOAD",                                                     /* 745 */
  "ABSHDRAWINGSETTINGS",                                      /* 746 */
  "ACAECUSTOBJ",                                              /* 747 */
  "ACAEEEMGROBJ",                                             /* 748 */
  "ACAMCOMP",                                                 /* 749 */
  "ACAMCOMPDEF",                                              /* 750 */
  "ACAMCOMPDEFMGR",                                           /* 751 */
  "ACAMCONTEXTMODELER",                                       /* 752 */
  "ACAMGDIMSTD",                                              /* 753 */
  "ACAMGFILTERDAT",                                           /* 754 */
  "ACAMGHOLECHARTSTDCSN",                                     /* 755 */
  "ACAMGHOLECHARTSTDDIN",                                     /* 756 */
  "ACAMGHOLECHARTSTDISO",                                     /* 757 */
  "ACAMGLAYSTD",                                              /* 758 */
  "ACAMGRCOMPDEF",                                            /* 759 */
  "ACAMGRCOMPDEFSET",                                         /* 760 */
  "ACAMGTITLESTD",                                            /* 761 */
  "ACAMMVDBACKUPOBJECT",                                      /* 762 */
  "ACAMPROJECT",                                              /* 763 */
  "ACAMSHAFTCOMPDEF",                                         /* 764 */
  "ACAMSTDPCOMPDEF",                                          /* 765 */
  "ACAMWBLOCKTEMPENTS",                                       /* 766 */
  "ACARRAYJIGENTITY",                                         /* 767 */
  "ACCMCONTEXT",                                              /* 768 */
  "ACDBCIRCARCRES",                                           /* 769 */
  "ACDBDIMENSIONRES",                                         /* 770 */
  "ACDBENTITYCACHE",                                          /* 771 */
  "ACDBLINERES",                                              /* 772 */
  "ACDBSTDPARTRES_ARC",                                       /* 773 */
  "ACDBSTDPARTRES_LINE",                                      /* 774 */
  "ACDB_HATCHSCALECONTEXTDATA_CLASS",                         /* 775 */
  "ACDB_HATCHVIEWCONTEXTDATA_CLASS",                          /* 776 */
  "ACDB_PROXY_ENTITY_DATA",                                   /* 777 */
  "ACGREFACADMASTER",                                         /* 778 */
  "ACGREFMASTER",                                             /* 779 */
  "ACIMINTSYSVAR",                                            /* 780 */
  "ACIMREALSYSVAR",                                           /* 781 */
  "ACIMSTRSYSVAR",                                            /* 782 */
  "ACIMSYSVARMAN",                                            /* 783 */
  "ACMANOOTATIONVIEWSTANDARDANSI",                            /* 784 */
  "ACMANOOTATIONVIEWSTANDARDCSN",                             /* 785 */
  "ACMANOOTATIONVIEWSTANDARDDIN",                             /* 786 */
  "ACMANOOTATIONVIEWSTANDARDISO",                             /* 787 */
  "ACMAPLEGENDDBOBJECT",                                      /* 788 */
  "ACMAPLEGENDITEMDBOBJECT",                                  /* 789 */
  "ACMAPMAPVIEWPORTDBOBJECT",                                 /* 790 */
  "ACMAPPRINTLAYOUTELEMENTDBOBJECTCONTAINER",                 /* 791 */
  "ACMBALLOON",                                               /* 792 */
  "ACMBOM",                                                   /* 793 */
  "ACMBOMROW",                                                /* 794 */
  "ACMBOMROWSTRUCT",                                          /* 795 */
  "ACMBOMSTANDARDANSI",                                       /* 796 */
  "ACMBOMSTANDARDCSN",                                        /* 797 */
  "ACMBOMSTANDARDDIN",                                        /* 798 */
  "ACMBOMSTANDARDISO",                                        /* 799 */
  "ACMCENTERLINESTANDARDANSI",                                /* 800 */
  "ACMCENTERLINESTANDARDCSN",                                 /* 801 */
  "ACMCENTERLINESTANDARDDIN",                                 /* 802 */
  "ACMCENTERLINESTANDARDISO",                                 /* 803 */
  "ACMDATADICTIONARY",                                        /* 804 */
  "ACMDATAENTRY",                                             /* 805 */
  "ACMDATAENTRYBLOCK",                                        /* 806 */
  "ACMDATUMID",                                               /* 807 */
  "ACMDATUMSTANDARDANSI",                                     /* 808 */
  "ACMDATUMSTANDARDCSN",                                      /* 809 */
  "ACMDATUMSTANDARDDIN",                                      /* 810 */
  "ACMDATUMSTANDARDISO",                                      /* 811 */
  "ACMDATUMSTANDARDISO2012",                                  /* 812 */
  "ACMDETAILSTANDARDANSI",                                    /* 813 */
  "ACMDETAILSTANDARDCSN",                                     /* 814 */
  "ACMDETAILSTANDARDDIN",                                     /* 815 */
  "ACMDETAILSTANDARDISO",                                     /* 816 */
  "ACMDETAILTANDARDCUSTOM",                                   /* 817 */
  "ACMDIMBREAKPERSREACTOR",                                   /* 818 */
  "ACMEDRAWINGMAN",                                           /* 819 */
  "ACMEVIEW",                                                 /* 820 */
  "ACME_DATABASE",                                            /* 821 */
  "ACME_DOCUMENT",                                            /* 822 */
  "ACMFCFRAME",                                               /* 823 */
  "ACMFCFSTANDARDANSI",                                       /* 824 */
  "ACMFCFSTANDARDCSN",                                        /* 825 */
  "ACMFCFSTANDARDDIN",                                        /* 826 */
  "ACMFCFSTANDARDISO",                                        /* 827 */
  "ACMFCFSTANDARDISO2004",                                    /* 828 */
  "ACMFCFSTANDARDISO2012",                                    /* 829 */
  "ACMIDSTANDARDANSI",                                        /* 830 */
  "ACMIDSTANDARDCSN",                                         /* 831 */
  "ACMIDSTANDARDDIN",                                         /* 832 */
  "ACMIDSTANDARDISO",                                         /* 833 */
  "ACMIDSTANDARDISO2004",                                     /* 834 */
  "ACMIDSTANDARDISO2012",                                     /* 835 */
  "ACMNOTESTANDARDANSI",                                      /* 836 */
  "ACMNOTESTANDARDCSN",                                       /* 837 */
  "ACMNOTESTANDARDDIN",                                       /* 838 */
  "ACMNOTESTANDARDISO",                                       /* 839 */
  "ACMPARTLIST",                                              /* 840 */
  "ACMPICKOBJ",                                               /* 841 */
  "ACMSECTIONSTANDARDANSI",                                   /* 842 */
  "ACMSECTIONSTANDARDCSN2002",                                /* 843 */
  "ACMSECTIONSTANDARDCUSTOM",                                 /* 844 */
  "ACMSECTIONSTANDARDDIN",                                    /* 845 */
  "ACMSECTIONSTANDARDISO",                                    /* 846 */
  "ACMSECTIONSTANDARDISO2001",                                /* 847 */
  "ACMSTANDARDANSI",                                          /* 848 */
  "ACMSTANDARDCSN",                                           /* 849 */
  "ACMSTANDARDDIN",                                           /* 850 */
  "ACMSTANDARDISO",                                           /* 851 */
  "ACMSURFSTANDARDANSI",                                      /* 852 */
  "ACMSURFSTANDARDCSN",                                       /* 853 */
  "ACMSURFSTANDARDDIN",                                       /* 854 */
  "ACMSURFSTANDARDISO",                                       /* 855 */
  "ACMSURFSTANDARDISO2002",                                   /* 856 */
  "ACMSURFSYM",                                               /* 857 */
  "ACMTAPERSTANDARDANSI",                                     /* 858 */
  "ACMTAPERSTANDARDCSN",                                      /* 859 */
  "ACMTAPERSTANDARDDIN",                                      /* 860 */
  "ACMTAPERSTANDARDISO",                                      /* 861 */
  "ACMTHREADLINESTANDARDANSI",                                /* 862 */
  "ACMTHREADLINESTANDARDCSN",                                 /* 863 */
  "ACMTHREADLINESTANDARDDIN",                                 /* 864 */
  "ACMTHREADLINESTANDARDISO",                                 /* 865 */
  "ACMWELDSTANDARDANSI",                                      /* 866 */
  "ACMWELDSTANDARDCSN",                                       /* 867 */
  "ACMWELDSTANDARDDIN",                                       /* 868 */
  "ACMWELDSTANDARDISO",                                       /* 869 */
  "ACMWELDSYM",                                               /* 870 */
  "ACRFATTGENMGR",                                            /* 871 */
  "ACRFINSADJ",                                               /* 872 */
  "ACRFINSADJUSTERMGR",                                       /* 873 */
  "ACRFMCADAPIATTHOLDER",                                     /* 874 */
  "ACRFOBJATTMGR",                                            /* 875 */
  "ACSH_SUBENT_MATERIAL_CLASS",                               /* 876 */
  "AC_AM_2D_XREF_MGR",                                        /* 877 */
  "AC_AM_BASIC_VIEW",                                         /* 878 */
  "AC_AM_BASIC_VIEW_DEF",                                     /* 879 */
  "AC_AM_COMPLEX_HIDE_SITUATION",                             /* 880 */
  "AC_AM_COMP_VIEW_DEF",                                      /* 881 */
  "AC_AM_COMP_VIEW_INST",                                     /* 882 */
  "AC_AM_DIRTY_NODES",                                        /* 883 */
  "AC_AM_HIDE_SITUATION",                                     /* 884 */
  "AC_AM_MAPPER_CACHE",                                       /* 885 */
  "AC_AM_MASTER_VIEW_DEF",                                    /* 886 */
  "AC_AM_MVD_DEP_MGR",                                        /* 887 */
  "AC_AM_OVERRIDE_FILTER",                                    /* 888 */
  "AC_AM_PROPS_OVERRIDE",                                     /* 889 */
  "AC_AM_SHAFT_HIDE_SITUATION",                               /* 890 */
  "AC_AM_STDP_VIEW_DEF",                                      /* 891 */
  "AC_AM_TRANSFORM_GHOST",                                    /* 892 */
  "ADAPPL",                                                   /* 893 */
  "AECC_ALIGNMENT_DESIGN_CHECK_SET",                          /* 894 */
  "AECC_ALIGNMENT_LABEL_SET",                                 /* 895 */
  "AECC_ALIGNMENT_LABEL_SET_EXT",                             /* 896 */
  "AECC_ALIGNMENT_PARCEL_NODE",                               /* 897 */
  "AECC_ALIGNMENT_STYLE",                                     /* 898 */
  "AECC_APPURTENANCE_STYLE",                                  /* 899 */
  "AECC_ASSEMBLY_STYLE",                                      /* 900 */
  "AECC_BUILDING_SITE_STYLE",                                 /* 901 */
  "AECC_CANT_DIAGRAM_VIEW_STYLE",                             /* 902 */
  "AECC_CATCHMENT_STYLE",                                     /* 903 */
  "AECC_CLASS_NODE",                                          /* 904 */
  "AECC_CONTOURVIEW",                                         /* 905 */
  "AECC_CORRIDOR_STYLE",                                      /* 906 */
  "AECC_DISP_REP_ALIGNMENT",                                  /* 907 */
  "AECC_DISP_REP_ALIGNMENT_CANT_LABEL_GROUP",                 /* 908 */
  "AECC_DISP_REP_ALIGNMENT_CSV",                              /* 909 */
  "AECC_DISP_REP_ALIGNMENT_CURVE_LABEL",                      /* 910 */
  "AECC_DISP_REP_ALIGNMENT_DESIGNSPEED_LABEL_GROUP",          /* 911 */
  "AECC_DISP_REP_ALIGNMENT_GEOMPT_LABEL_GROUP",               /* 912 */
  "AECC_DISP_REP_ALIGNMENT_INDEXED_PI_LABEL",                 /* 913 */
  "AECC_DISP_REP_ALIGNMENT_MINOR_STATION_LABEL_GROUP",        /* 914 */
  "AECC_DISP_REP_ALIGNMENT_PI_LABEL",                         /* 915 */
  "AECC_DISP_REP_ALIGNMENT_SPIRAL_LABEL",                     /* 916 */
  "AECC_DISP_REP_ALIGNMENT_STAEQU_LABEL_GROUP",               /* 917 */
  "AECC_DISP_REP_ALIGNMENT_STATION_LABEL_GROUP",              /* 918 */
  "AECC_DISP_REP_ALIGNMENT_STATION_OFFSET_LABEL",             /* 919 */
  "AECC_DISP_REP_ALIGNMENT_SUPERELEVATION_LABEL_GROUP",       /* 920 */
  "AECC_DISP_REP_ALIGNMENT_TABLE",                            /* 921 */
  "AECC_DISP_REP_ALIGNMENT_TANGENT_LABEL",                    /* 922 */
  "AECC_DISP_REP_ALIGNMENT_VERTICAL_GEOMPT_LABELING",         /* 923 */
  "AECC_DISP_REP_APPURTENANCE",                               /* 924 */
  "AECC_DISP_REP_APPURTENANCE_CSV",                           /* 925 */
  "AECC_DISP_REP_APPURTENANCE_LABELING",                      /* 926 */
  "AECC_DISP_REP_APPURTENANCE_PROFILE_LABELING",              /* 927 */
  "AECC_DISP_REP_ASSEMBLY",                                   /* 928 */
  "AECC_DISP_REP_AUTO_CORRIDOR_FEATURE_LINE",                 /* 929 */
  "AECC_DISP_REP_AUTO_CORRIDOR_FEATURE_LINE_PROFILE",         /* 930 */
  "AECC_DISP_REP_AUTO_CORRIDOR_FEATURE_LINE_SECTION",         /* 931 */
  "AECC_DISP_REP_AUTO_FEATURE_LINE",                          /* 932 */
  "AECC_DISP_REP_AUTO_FEATURE_LINE_CSV",                      /* 933 */
  "AECC_DISP_REP_AUTO_FEATURE_LINE_PROFILE",                  /* 934 */
  "AECC_DISP_REP_AUTO_FEATURE_LINE_SECTION",                  /* 935 */
  "AECC_DISP_REP_BUILDINGSITE",                               /* 936 */
  "AECC_DISP_REP_BUILDINGUTIL_CONNECTOR",                     /* 937 */
  "AECC_DISP_REP_CANT_DIAGRAM_VIEW",                          /* 938 */
  "AECC_DISP_REP_CATCHMENT_AREA",                             /* 939 */
  "AECC_DISP_REP_CATCHMENT_AREA_LABEL",                       /* 940 */
  "AECC_DISP_REP_CORRIDOR",                                   /* 941 */
  "AECC_DISP_REP_CROSSING_PIPE_PROFILE_LABELING",             /* 942 */
  "AECC_DISP_REP_CROSSING_PRESSURE_PIPE_PROFILE_LABELING",    /* 943 */
  "AECC_DISP_REP_CSVSTATIONSLIDER",                           /* 944 */
  "AECC_DISP_REP_FACE",                                       /* 945 */
  "AECC_DISP_REP_FEATURE",                                    /* 946 */
  "AECC_DISP_REP_FEATURE_LABEL",                              /* 947 */
  "AECC_DISP_REP_FEATURE_LINE",                               /* 948 */
  "AECC_DISP_REP_FEATURE_LINE_CSV",                           /* 949 */
  "AECC_DISP_REP_FEATURE_LINE_PROFILE",                       /* 950 */
  "AECC_DISP_REP_FEATURE_LINE_SECTION",                       /* 951 */
  "AECC_DISP_REP_FITTING",                                    /* 952 */
  "AECC_DISP_REP_FITTING_CSV",                                /* 953 */
  "AECC_DISP_REP_FITTING_LABELING",                           /* 954 */
  "AECC_DISP_REP_FITTING_PROFILE_LABELING",                   /* 955 */
  "AECC_DISP_REP_FLOW_SEGMENT_LABEL",                         /* 956 */
  "AECC_DISP_REP_GENERAL_SEGMENT_LABEL",                      /* 957 */
  "AECC_DISP_REP_GRADING",                                    /* 958 */
  "AECC_DISP_REP_GRAPH",                                      /* 959 */
  "AECC_DISP_REP_GRAPHPROFILE_NETWORKPART",                   /* 960 */
  "AECC_DISP_REP_GRAPHPROFILE_PRESSUREPART",                  /* 961 */
  "AECC_DISP_REP_GRID_SURFACE",                               /* 962 */
  "AECC_DISP_REP_GRID_SURFACE_CSV",                           /* 963 */
  "AECC_DISP_REP_HORGEOMETRY_BAND_LABEL_GROUP",               /* 964 */
  "AECC_DISP_REP_HYDRO_REGION",                               /* 965 */
  "AECC_DISP_REP_INTERFERENCE_CHECK",                         /* 966 */
  "AECC_DISP_REP_INTERFERENCE_PART",                          /* 967 */
  "AECC_DISP_REP_INTERFERENCE_PART_SECTION",                  /* 968 */
  "AECC_DISP_REP_INTERSECTION",                               /* 969 */
  "AECC_DISP_REP_INTERSECTION_LOCATION_LABELING",             /* 970 */
  "AECC_DISP_REP_LABELING",                                   /* 971 */
  "AECC_DISP_REP_LEGEND_TABLE",                               /* 972 */
  "AECC_DISP_REP_LINE_BETWEEN_POINTS_LABEL",                  /* 973 */
  "AECC_DISP_REP_LOTLINE_CSV",                                /* 974 */
  "AECC_DISP_REP_MASSHAULLINE",                               /* 975 */
  "AECC_DISP_REP_MASS_HAUL_VIEW",                             /* 976 */
  "AECC_DISP_REP_MATCHLINE_LABELING",                         /* 977 */
  "AECC_DISP_REP_MATCH_LINE",                                 /* 978 */
  "AECC_DISP_REP_MATERIAL_SECTION",                           /* 979 */
  "AECC_DISP_REP_NETWORK",                                    /* 980 */
  "AECC_DISP_REP_NOTE_LABEL",                                 /* 981 */
  "AECC_DISP_REP_OFFSET_ELEV_LABEL",                          /* 982 */
  "AECC_DISP_REP_PARCEL_BOUNDARY",                            /* 983 */
  "AECC_DISP_REP_PARCEL_FACE_LABEL",                          /* 984 */
  "AECC_DISP_REP_PARCEL_SEGMENT",                             /* 985 */
  "AECC_DISP_REP_PARCEL_SEGMENT_LABEL",                       /* 986 */
  "AECC_DISP_REP_PARCEL_SEGMENT_TABLE",                       /* 987 */
  "AECC_DISP_REP_PARCEL_TABLE",                               /* 988 */
  "AECC_DISP_REP_PIPE",                                       /* 989 */
  "AECC_DISP_REP_PIPENETWORK_BAND_LABEL_GROUP",               /* 990 */
  "AECC_DISP_REP_PIPE_CSV",                                   /* 991 */
  "AECC_DISP_REP_PIPE_LABELING",                              /* 992 */
  "AECC_DISP_REP_PIPE_PROFILE_LABELING",                      /* 993 */
  "AECC_DISP_REP_PIPE_SECTION_LABELING",                      /* 994 */
  "AECC_DISP_REP_PIPE_TABLE",                                 /* 995 */
  "AECC_DISP_REP_POINT_ENT",                                  /* 996 */
  "AECC_DISP_REP_POINT_GROUP",                                /* 997 */
  "AECC_DISP_REP_POINT_TABLE",                                /* 998 */
  "AECC_DISP_REP_PRESSUREPIPENETWORK",                        /* 999 */
  "AECC_DISP_REP_PRESSURE_PART_TABLE",                        /* 1000 */
  "AECC_DISP_REP_PRESSURE_PIPE",                              /* 1001 */
  "AECC_DISP_REP_PRESSURE_PIPE_CSV",                          /* 1002 */
  "AECC_DISP_REP_PRESSURE_PIPE_LABELING",                     /* 1003 */
  "AECC_DISP_REP_PRESSURE_PIPE_PROFILE_LABELING",             /* 1004 */
  "AECC_DISP_REP_PRESSURE_PIPE_SECTION_LABELING",             /* 1005 */
  "AECC_DISP_REP_PROFILE",                                    /* 1006 */
  "AECC_DISP_REP_PROFILEDATA_BAND_LABEL_GROUP",               /* 1007 */
  "AECC_DISP_REP_PROFILE_PROJECTION",                         /* 1008 */
  "AECC_DISP_REP_PROFILE_PROJECTION_LABEL",                   /* 1009 */
  "AECC_DISP_REP_PROFILE_VIEW",                               /* 1010 */
  "AECC_DISP_REP_PROFILE_VIEW_DEPTH_LABEL",                   /* 1011 */
  "AECC_DISP_REP_QUANTITY_TAKEOFF_AGGREGATE_EARTHWORK_TABLE", /* 1012 */
  "AECC_DISP_REP_RIGHT_OF_WAY",                               /* 1013 */
  "AECC_DISP_REP_SAMPLELINE_LABELING",                        /* 1014 */
  "AECC_DISP_REP_SAMPLE_LINE",                                /* 1015 */
  "AECC_DISP_REP_SAMPLE_LINE_GROUP",                          /* 1016 */
  "AECC_DISP_REP_SECTION",                                    /* 1017 */
  "AECC_DISP_REP_SECTIONALDATA_BAND_LABEL_GROUP",             /* 1018 */
  "AECC_DISP_REP_SECTIONDATA_BAND_LABEL_GROUP",               /* 1019 */
  "AECC_DISP_REP_SECTIONSEGMENT_BAND_LABEL_GROUP",            /* 1020 */
  "AECC_DISP_REP_SECTION_CORRIDOR",                           /* 1021 */
  "AECC_DISP_REP_SECTION_CORRIDOR_POINT_LABEL_GROUP",         /* 1022 */
  "AECC_DISP_REP_SECTION_GRADEBREAK_LABEL_GROUP",             /* 1023 */
  "AECC_DISP_REP_SECTION_MINOR_OFFSET_LABEL_GROUP",           /* 1024 */
  "AECC_DISP_REP_SECTION_OFFSET_LABEL_GROUP",                 /* 1025 */
  "AECC_DISP_REP_SECTION_PIPENETWORK",                        /* 1026 */
  "AECC_DISP_REP_SECTION_PRESSUREPIPENETWORK",                /* 1027 */
  "AECC_DISP_REP_SECTION_PROJECTION",                         /* 1028 */
  "AECC_DISP_REP_SECTION_PROJECTION_LABEL",                   /* 1029 */
  "AECC_DISP_REP_SECTION_SEGMENT_LABEL_GROUP",                /* 1030 */
  "AECC_DISP_REP_SECTION_VIEW",                               /* 1031 */
  "AECC_DISP_REP_SECTION_VIEW_DEPTH_LABEL",                   /* 1032 */
  "AECC_DISP_REP_SECTION_VIEW_QUANTITY_TAKEOFF_TABLE",        /* 1033 */
  "AECC_DISP_REP_SHEET",                                      /* 1034 */
  "AECC_DISP_REP_SPANNING_PIPE_LABELING",                     /* 1035 */
  "AECC_DISP_REP_SPANNING_PIPE_PROFILE_LABELING",             /* 1036 */
  "AECC_DISP_REP_STATION_ELEV_LABEL",                         /* 1037 */
  "AECC_DISP_REP_STRUCTURE",                                  /* 1038 */
  "AECC_DISP_REP_STRUCTURE_CSV",                              /* 1039 */
  "AECC_DISP_REP_STRUCTURE_LABELING",                         /* 1040 */
  "AECC_DISP_REP_STRUCTURE_PROFILE_LABELING",                 /* 1041 */
  "AECC_DISP_REP_STRUCTURE_SECTION_LABELING",                 /* 1042 */
  "AECC_DISP_REP_SUBASSEMBLY",                                /* 1043 */
  "AECC_DISP_REP_SUPERELEVATION_BAND_LABEL_GROUP",            /* 1044 */
  "AECC_DISP_REP_SUPERELEVATION_DIAGRAM_VIEW",                /* 1045 */
  "AECC_DISP_REP_SURFACE_CONTOUR_LABEL_GROUP",                /* 1046 */
  "AECC_DISP_REP_SURFACE_ELEVATION_LABEL",                    /* 1047 */
  "AECC_DISP_REP_SURFACE_SLOPE_LABEL",                        /* 1048 */
  "AECC_DISP_REP_SURVEY_FIGURE_LABEL_GROUP",                  /* 1049 */
  "AECC_DISP_REP_SVFIGURE",                                   /* 1050 */
  "AECC_DISP_REP_SVFIGURE_CSV",                               /* 1051 */
  "AECC_DISP_REP_SVFIGURE_PROFILE",                           /* 1052 */
  "AECC_DISP_REP_SVFIGURE_SECTION",                           /* 1053 */
  "AECC_DISP_REP_SVFIGURE_SEGMENT_LABEL",                     /* 1054 */
  "AECC_DISP_REP_SVNETWORK",                                  /* 1055 */
  "AECC_DISP_REP_TANGENT_INTERSECTION_TABLE",                 /* 1056 */
  "AECC_DISP_REP_TIN_SURFACE",                                /* 1057 */
  "AECC_DISP_REP_TIN_SURFACE_CSV",                            /* 1058 */
  "AECC_DISP_REP_VALIGNMENT_CRESTCURVE_LABEL_GROUP",          /* 1059 */
  "AECC_DISP_REP_VALIGNMENT_CSV",                             /* 1060 */
  "AECC_DISP_REP_VALIGNMENT_HAGEOMPT_LABEL_GROUP",            /* 1061 */
  "AECC_DISP_REP_VALIGNMENT_LINE_LABEL_GROUP",                /* 1062 */
  "AECC_DISP_REP_VALIGNMENT_MINOR_STATION_LABEL_GROUP",       /* 1063 */
  "AECC_DISP_REP_VALIGNMENT_PVI_LABEL_GROUP",                 /* 1064 */
  "AECC_DISP_REP_VALIGNMENT_SAGCURVE_LABEL_GROUP",            /* 1065 */
  "AECC_DISP_REP_VALIGNMENT_STATION_LABEL_GROUP",             /* 1066 */
  "AECC_DISP_REP_VERTICALGEOMETRY_BAND_LABEL_GROUP",          /* 1067 */
  "AECC_DISP_REP_VIEWFRAME_LABELING",                         /* 1068 */
  "AECC_DISP_REP_VIEW_FRAME",                                 /* 1069 */
  "AECC_FEATURELINE_STYLE",                                   /* 1070 */
  "AECC_FEATURE_STYLE",                                       /* 1071 */
  "AECC_FITTING_STYLE",                                       /* 1072 */
  "AECC_FORMAT_MANAGER_OBJECT",                               /* 1073 */
  "AECC_GRADEVIEW",                                           /* 1074 */
  "AECC_GRADING_CRITERIA",                                    /* 1075 */
  "AECC_GRADING_CRITERIA_SET",                                /* 1076 */
  "AECC_GRADING_GROUP",                                       /* 1077 */
  "AECC_GRADING_STYLE",                                       /* 1078 */
  "AECC_IMPORT_STORM_SEWER_DEFAULTS",                         /* 1079 */
  "AECC_INTERFERENCE_STYLE",                                  /* 1080 */
  "AECC_INTERSECTION_STYLE",                                  /* 1081 */
  "AECC_LABEL_COLLECTOR_STYLE",                               /* 1082 */
  "AECC_LABEL_NODE",                                          /* 1083 */
  "AECC_LABEL_RADIAL_LINE_STYLE",                             /* 1084 */
  "AECC_LABEL_TEXT_ITERATOR_CURVE_OR_SPIRAL_STYLE",           /* 1085 */
  "AECC_LABEL_TEXT_ITERATOR_STYLE",                           /* 1086 */
  "AECC_LABEL_TEXT_STYLE",                                    /* 1087 */
  "AECC_LABEL_VECTOR_ARROW_STYLE",                            /* 1088 */
  "AECC_LEGEND_TABLE_STYLE",                                  /* 1089 */
  "AECC_MASS_HAUL_LINE_STYLE",                                /* 1090 */
  "AECC_MASS_HAUL_VIEW_STYLE",                                /* 1091 */
  "AECC_MATCHLINE_STYLE",                                     /* 1092 */
  "AECC_MATERIAL_STYLE",                                      /* 1093 */
  "AECC_NETWORK_PART_CATALOG_DEF_NODE",                       /* 1094 */
  "AECC_NETWORK_PART_FAMILY_ITEM",                            /* 1095 */
  "AECC_NETWORK_PART_LIST",                                   /* 1096 */
  "AECC_NETWORK_RULE",                                        /* 1097 */
  "AECC_PARCEL_NODE",                                         /* 1098 */
  "AECC_PARCEL_STYLE",                                        /* 1099 */
  "AECC_PART_SIZE_FILTER",                                    /* 1100 */
  "AECC_PIPE_RULES",                                          /* 1101 */
  "AECC_PIPE_STYLE",                                          /* 1102 */
  "AECC_PIPE_STYLE_EXTENSION",                                /* 1103 */
  "AECC_POINTCLOUD_STYLE",                                    /* 1104 */
  "AECC_POINTVIEW",                                           /* 1105 */
  "AECC_POINT_STYLE",                                         /* 1106 */
  "AECC_PRESSURE_PART_LIST",                                  /* 1107 */
  "AECC_PRESSURE_PIPE_STYLE",                                 /* 1108 */
  "AECC_PROFILESECTIONENTITY_STYLE",                          /* 1109 */
  "AECC_PROFILE_DESIGN_CHECK_SET",                            /* 1110 */
  "AECC_PROFILE_LABEL_SET",                                   /* 1111 */
  "AECC_PROFILE_STYLE",                                       /* 1112 */
  "AECC_PROFILE_VIEW_BAND_STYLE_SET",                         /* 1113 */
  "AECC_PROFILE_VIEW_DATA_BAND_STYLE",                        /* 1114 */
  "AECC_PROFILE_VIEW_HORIZONTAL_GEOMETRY_BAND_STYLE",         /* 1115 */
  "AECC_PROFILE_VIEW_PIPE_NETWORK_BAND_STYLE",                /* 1116 */
  "AECC_PROFILE_VIEW_SECTIONAL_DATA_BAND_STYLE",              /* 1117 */
  "AECC_PROFILE_VIEW_STYLE",                                  /* 1118 */
  "AECC_PROFILE_VIEW_SUPERELEVATION_DIAGRAM_BAND_STYLE",      /* 1119 */
  "AECC_PROFILE_VIEW_VERTICAL_GEOMETRY_BAND_STYLE",           /* 1120 */
  "AECC_QUANTITY_TAKEOFF_CRITERIA",                           /* 1121 */
  "AECC_ROADWAYLINK_STYLE",                                   /* 1122 */
  "AECC_ROADWAYMARKER_STYLE",                                 /* 1123 */
  "AECC_ROADWAYSHAPE_STYLE",                                  /* 1124 */
  "AECC_ROADWAY_STYLE_SET",                                   /* 1125 */
  "AECC_ROOT_SETTINGS_NODE",                                  /* 1126 */
  "AECC_SAMPLE_LINE_GROUP_STYLE",                             /* 1127 */
  "AECC_SAMPLE_LINE_STYLE",                                   /* 1128 */
  "AECC_SECTION_LABEL_SET",                                   /* 1129 */
  "AECC_SECTION_STYLE",                                       /* 1130 */
  "AECC_SECTION_VIEW_BAND_STYLE_SET",                         /* 1131 */
  "AECC_SECTION_VIEW_DATA_BAND_STYLE",                        /* 1132 */
  "AECC_SECTION_VIEW_ROAD_SURFACE_BAND_STYLE",                /* 1133 */
  "AECC_SECTION_VIEW_STYLE",                                  /* 1134 */
  "AECC_SETTINGS_NODE",                                       /* 1135 */
  "AECC_SHEET_STYLE",                                         /* 1136 */
  "AECC_SLOPE_PATTERN_STYLE",                                 /* 1137 */
  "AECC_STATION_FORMAT_STYLE",                                /* 1138 */
  "AECC_STRUCTURE_RULES",                                     /* 1139 */
  "AECC_STUCTURE_STYLE",                                      /* 1140 */
  "AECC_SUPERELEVATION_DIAGRAM_VIEW_STYLE",                   /* 1141 */
  "AECC_SURFACE_STYLE",                                       /* 1142 */
  "AECC_SVFIGURE_STYLE",                                      /* 1143 */
  "AECC_SVNETWORK_STYLE",                                     /* 1144 */
  "AECC_TABLE_STYLE",                                         /* 1145 */
  "AECC_TAG_MANAGER",                                         /* 1146 */
  "AECC_TREE_NODE",                                           /* 1147 */
  "AECC_USER_DEFINED_ATTRIBUTE_CLASSIFICATION",               /* 1148 */
  "AECC_VALIGNMENT_STYLE_EXTENSION",                          /* 1149 */
  "AECC_VIEW_FRAME_STYLE",                                    /* 1150 */
  "AECS_DISP_PROPS_MEMBER",                                   /* 1151 */
  "AECS_DISP_PROPS_MEMBER_LOGICAL",                           /* 1152 */
  "AECS_DISP_PROPS_MEMBER_PLAN",                              /* 1153 */
  "AECS_DISP_PROPS_MEMBER_PLAN_SKETCH",                       /* 1154 */
  "AECS_DISP_PROPS_MEMBER_PROJECTED",                         /* 1155 */
  "AECS_DISP_REP_MEMBER_ELEVATION_DESIGN",                    /* 1156 */
  "AECS_DISP_REP_MEMBER_ELEVATION_DETAIL",                    /* 1157 */
  "AECS_DISP_REP_MEMBER_LOGICAL",                             /* 1158 */
  "AECS_DISP_REP_MEMBER_MODEL_DESIGN",                        /* 1159 */
  "AECS_DISP_REP_MEMBER_MODEL_DETAIL",                        /* 1160 */
  "AECS_DISP_REP_MEMBER_PLAN_DESIGN",                         /* 1161 */
  "AECS_DISP_REP_MEMBER_PLAN_DETAIL",                         /* 1162 */
  "AECS_DISP_REP_MEMBER_PLAN_SKETCH",                         /* 1163 */
  "AECS_MEMBER_NODE_SHAPE",                                   /* 1164 */
  "AECS_MEMBER_STYLE",                                        /* 1165 */
  "AEC_2DSECTION_STYLE",                                      /* 1166 */
  "AEC_AECDBDISPREPBDGELEVLINEPLAN100",                       /* 1167 */
  "AEC_AECDBDISPREPBDGELEVLINEPLAN50",                        /* 1168 */
  "AEC_AECDBDISPREPBDGSECTIONLINEPLAN100",                    /* 1169 */
  "AEC_AECDBDISPREPBDGSECTIONLINEPLAN50",                     /* 1170 */
  "AEC_AECDBDISPREPCEILINGGRIDPLAN100",                       /* 1171 */
  "AEC_AECDBDISPREPCEILINGGRIDPLAN50",                        /* 1172 */
  "AEC_AECDBDISPREPCOLUMNGRIDPLAN100",                        /* 1173 */
  "AEC_AECDBDISPREPCOLUMNGRIDPLAN50",                         /* 1174 */
  "AEC_AECDBDISPREPCURTAINWALLLAYOUTPLAN100",                 /* 1175 */
  "AEC_AECDBDISPREPCURTAINWALLLAYOUTPLAN50",                  /* 1176 */
  "AEC_AECDBDISPREPCURTAINWALLUNITPLAN100",                   /* 1177 */
  "AEC_AECDBDISPREPCURTAINWALLUNITPLAN50",                    /* 1178 */
  "AEC_AECDBDISPREPMVBLOCKREFPLAN100",                        /* 1179 */
  "AEC_AECDBDISPREPMVBLOCKREFPLAN50",                         /* 1180 */
  "AEC_AECDBDISPREPROOFPLAN100",                              /* 1181 */
  "AEC_AECDBDISPREPROOFPLAN50",                               /* 1182 */
  "AEC_AECDBDISPREPROOFSLABPLAN100",                          /* 1183 */
  "AEC_AECDBDISPREPROOFSLABPLAN50",                           /* 1184 */
  "AEC_AECDBDISPREPSLABPLAN100",                              /* 1185 */
  "AEC_AECDBDISPREPSLABPLAN50",                               /* 1186 */
  "AEC_AECDBDISPREPSPACEPLAN100",                             /* 1187 */
  "AEC_AECDBDISPREPSPACEPLAN50",                              /* 1188 */
  "AEC_AECDBDISPREPWALLPLAN100",                              /* 1189 */
  "AEC_AECDBDISPREPWALLPLAN50",                               /* 1190 */
  "AEC_AECDBDISPREPWINDOWASSEMBLYPLAN100",                    /* 1191 */
  "AEC_AECDBDISPREPWINDOWASSEMBLYPLAN50",                     /* 1192 */
  "AEC_AECDBDISPREPZONE100",                                  /* 1193 */
  "AEC_AECDBDISPREPZONE50",                                   /* 1194 */
  "AEC_AECDBZONEDEF",                                         /* 1195 */
  "AEC_AECDBZONESTYLE",                                       /* 1196 */
  "AEC_ANCHOR_OPENINGBASE_TO_WALL",                           /* 1197 */
  "AEC_CLASSIFICATION_DEF",                                   /* 1198 */
  "AEC_CLASSIFICATION_SYSTEM_DEF",                            /* 1199 */
  "AEC_CLEANUP_GROUP_DEF",                                    /* 1200 */
  "AEC_CURTAIN_WALL_LAYOUT_STYLE",                            /* 1201 */
  "AEC_CURTAIN_WALL_UNIT_STYLE",                              /* 1202 */
  "AEC_CVSECTIONVIEW",                                        /* 1203 */
  "AEC_DB_DISP_REP_DIM_GROUP_PLAN",                           /* 1204 */
  "AEC_DB_DISP_REP_DIM_GROUP_PLAN100",                        /* 1205 */
  "AEC_DB_DISP_REP_DIM_GROUP_PLAN50",                         /* 1206 */
  "AEC_DIM_STYLE",                                            /* 1207 */
  "AEC_DISPLAYTHEME_STYLE",                                   /* 1208 */
  "AEC_DISP_PROPS_2D_SECTION",                                /* 1209 */
  "AEC_DISP_PROPS_CLIP_VOLUME",                               /* 1210 */
  "AEC_DISP_PROPS_CLIP_VOLUME_RESULT",                        /* 1211 */
  "AEC_DISP_PROPS_DIM",                                       /* 1212 */
  "AEC_DISP_PROPS_DISPLAYTHEME",                              /* 1213 */
  "AEC_DISP_PROPS_DOOR",                                      /* 1214 */
  "AEC_DISP_PROPS_DOOR_NOMINAL",                              /* 1215 */
  "AEC_DISP_PROPS_DOOR_PLAN_100",                             /* 1216 */
  "AEC_DISP_PROPS_DOOR_PLAN_50",                              /* 1217 */
  "AEC_DISP_PROPS_DOOR_THRESHOLD_PLAN",                       /* 1218 */
  "AEC_DISP_PROPS_DOOR_THRESHOLD_SYMBOL_PLAN",                /* 1219 */
  "AEC_DISP_PROPS_EDITINPLACEPROFILE_MODEL",                  /* 1220 */
  "AEC_DISP_PROPS_ENT",                                       /* 1221 */
  "AEC_DISP_PROPS_ENT_REF",                                   /* 1222 */
  "AEC_DISP_PROPS_GRID_ASSEMBLY_MODEL",                       /* 1223 */
  "AEC_DISP_PROPS_GRID_ASSEMBLY_PLAN",                        /* 1224 */
  "AEC_DISP_PROPS_LAYOUT_CURVE",                              /* 1225 */
  "AEC_DISP_PROPS_LAYOUT_GRID2D",                             /* 1226 */
  "AEC_DISP_PROPS_LAYOUT_GRID3D",                             /* 1227 */
  "AEC_DISP_PROPS_MASKBLOCK",                                 /* 1228 */
  "AEC_DISP_PROPS_MASS_ELEM_MODEL",                           /* 1229 */
  "AEC_DISP_PROPS_MASS_GROUP",                                /* 1230 */
  "AEC_DISP_PROPS_MATERIAL",                                  /* 1231 */
  "AEC_DISP_PROPS_OPENING",                                   /* 1232 */
  "AEC_DISP_PROPS_POLYGON_MODEL",                             /* 1233 */
  "AEC_DISP_PROPS_POLYGON_TRUECOLOUR",                        /* 1234 */
  "AEC_DISP_PROPS_RAILING_MODEL",                             /* 1235 */
  "AEC_DISP_PROPS_RAILING_PLAN",                              /* 1236 */
  "AEC_DISP_PROPS_ROOF",                                      /* 1237 */
  "AEC_DISP_PROPS_ROOFSLAB",                                  /* 1238 */
  "AEC_DISP_PROPS_ROOFSLAB_PLAN",                             /* 1239 */
  "AEC_DISP_PROPS_SCHEDULE_TABLE",                            /* 1240 */
  "AEC_DISP_PROPS_SLAB",                                      /* 1241 */
  "AEC_DISP_PROPS_SLAB_PLAN",                                 /* 1242 */
  "AEC_DISP_PROPS_SLICE",                                     /* 1243 */
  "AEC_DISP_PROPS_SPACE_DECOMPOSED",                          /* 1244 */
  "AEC_DISP_PROPS_SPACE_MODEL",                               /* 1245 */
  "AEC_DISP_PROPS_SPACE_PLAN",                                /* 1246 */
  "AEC_DISP_PROPS_STAIR_MODEL",                               /* 1247 */
  "AEC_DISP_PROPS_STAIR_PLAN",                                /* 1248 */
  "AEC_DISP_PROPS_STAIR_PLAN_OVERLAPPING",                    /* 1249 */
  "AEC_DISP_PROPS_WALL_GRAPH",                                /* 1250 */
  "AEC_DISP_PROPS_WALL_MODEL",                                /* 1251 */
  "AEC_DISP_PROPS_WALL_PLAN",                                 /* 1252 */
  "AEC_DISP_PROPS_WALL_SCHEM",                                /* 1253 */
  "AEC_DISP_PROPS_WINDOW",                                    /* 1254 */
  "AEC_DISP_PROPS_WINDOW_ASSEMBLY_SILL_PLAN",                 /* 1255 */
  "AEC_DISP_PROPS_WINDOW_NOMINAL",                            /* 1256 */
  "AEC_DISP_PROPS_WINDOW_PLAN_100",                           /* 1257 */
  "AEC_DISP_PROPS_WINDOW_PLAN_50",                            /* 1258 */
  "AEC_DISP_PROPS_WINDOW_SILL_PLAN",                          /* 1259 */
  "AEC_DISP_PROPS_ZONE",                                      /* 1260 */
  "AEC_DISP_REP_2D_SECTION",                                  /* 1261 */
  "AEC_DISPREPAECDBDISPREPMASSELEMPLAN100",                   /* 1262 */
  "AEC_DISPREPAECDBDISPREPMASSELEMPLAN50",                    /* 1263 */
  "AEC_DISPREPAECDBDISPREPMASSGROUPPLAN100",                  /* 1264 */
  "AEC_DISPREPAECDBDISPREPMASSGROUPPLAN50",                   /* 1265 */
  "AEC_DISPREPAECDBDISPREPOPENINGPLAN100",                    /* 1266 */
  "AEC_DISPREPAECDBDISPREPOPENINGPLAN50",                     /* 1267 */
  "AEC_DISPREPAECDBDISPREPOPENINGPLANREFLECTED",              /* 1268 */
  "AEC_DISPREPAECDBDISPREPOPENINGSILLPLAN",                   /* 1269 */
  "AEC_DISP_REP_ANCHOR",                                      /* 1270 */
  "AEC_DISP_REP_ANCHOR_BUB_TO_GRID",                          /* 1271 */
  "AEC_DISP_REP_ANCHOR_BUB_TO_GRID_MODEL",                    /* 1272 */
  "AEC_DISP_REP_ANCHOR_BUB_TO_GRID_RCP",                      /* 1273 */
  "AEC_DISP_REP_ANCHOR_ENT_TO_NODE",                          /* 1274 */
  "AEC_DISP_REP_ANCHOR_EXT_TAG_TO_ENT",                       /* 1275 */
  "AEC_DISP_REP_ANCHOR_TAG_TO_ENT",                           /* 1276 */
  "AEC_DISP_REP_BDG_ELEVLINE_MODEL",                          /* 1277 */
  "AEC_DISP_REP_BDG_ELEVLINE_PLAN",                           /* 1278 */
  "AEC_DISP_REP_BDG_ELEVLINE_RCP",                            /* 1279 */
  "AEC_DISP_REP_BDG_SECTIONLINE_MODEL",                       /* 1280 */
  "AEC_DISP_REP_BDG_SECTIONLINE_PLAN",                        /* 1281 */
  "AEC_DISP_REP_BDG_SECTIONLINE_RCP",                         /* 1282 */
  "AEC_DISP_REP_BDG_SECTION_MODEL",                           /* 1283 */
  "AEC_DISP_REP_BDG_SECTION_SUBDIV",                          /* 1284 */
  "AEC_DISP_REP_CEILING_GRID",                                /* 1285 */
  "AEC_DISP_REP_CEILING_GRID_MODEL",                          /* 1286 */
  "AEC_DISP_REP_CEILING_GRID_RCP",                            /* 1287 */
  "AEC_DISP_REP_CLIP_VOLUME_MODEL",                           /* 1288 */
  "AEC_DISP_REP_CLIP_VOLUME_PLAN",                            /* 1289 */
  "AEC_DISP_REP_CLIP_VOLUME_RESULT",                          /* 1290 */
  "AEC_DISP_REP_CLIP_VOLUME_RESULT_SUBDIV",                   /* 1291 */
  "AEC_DISP_REP_COL_BLOCK",                                   /* 1292 */
  "AEC_DISP_REP_COL_CIRCARC2D",                               /* 1293 */
  "AEC_DISP_REP_COL_CONCOINCIDENT",                           /* 1294 */
  "AEC_DISP_REP_COL_CONCONCENTRIC",                           /* 1295 */
  "AEC_DISP_REP_COL_CONEQUALDISTANCE",                        /* 1296 */
  "AEC_DISP_REP_COL_CONMIDPOINT",                             /* 1297 */
  "AEC_DISP_REP_COL_CONNECTOR",                               /* 1298 */
  "AEC_DISP_REP_COL_CONNORMAL",                               /* 1299 */
  "AEC_DISP_REP_COL_CONPARALLEL",                             /* 1300 */
  "AEC_DISP_REP_COL_CONPERPENDICULAR",                        /* 1301 */
  "AEC_DISP_REP_COL_CONSYMMETRIC",                            /* 1302 */
  "AEC_DISP_REP_COL_CONTANGENT",                              /* 1303 */
  "AEC_DISP_REP_COL_DIMANGLE",                                /* 1304 */
  "AEC_DISP_REP_COL_DIMDIAMETER",                             /* 1305 */
  "AEC_DISP_REP_COL_DIMDISTANCE",                             /* 1306 */
  "AEC_DISP_REP_COL_DIMLENGTH",                               /* 1307 */
  "AEC_DISP_REP_COL_DIMMAJORRADIUS",                          /* 1308 */
  "AEC_DISP_REP_COL_DIMMINORRADIUS",                          /* 1309 */
  "AEC_DISP_REP_COL_ELLIPARC2D",                              /* 1310 */
  "AEC_DISP_REP_COL_LAYOUTDATA",                              /* 1311 */
  "AEC_DISP_REP_COL_LINE2D",                                  /* 1312 */
  "AEC_DISP_REP_COL_MODIFIER_ADD",                            /* 1313 */
  "AEC_DISP_REP_COL_MODIFIER_CUTPLANE",                       /* 1314 */
  "AEC_DISP_REP_COL_MODIFIER_EXTRUSION",                      /* 1315 */
  "AEC_DISP_REP_COL_MODIFIER_GROUP",                          /* 1316 */
  "AEC_DISP_REP_COL_MODIFIER_LOFT",                           /* 1317 */
  "AEC_DISP_REP_COL_MODIFIER_PATH",                           /* 1318 */
  "AEC_DISP_REP_COL_MODIFIER_REVOLVE",                        /* 1319 */
  "AEC_DISP_REP_COL_MODIFIER_SUBTRACT",                       /* 1320 */
  "AEC_DISP_REP_COL_MODIFIER_TRANSITION",                     /* 1321 */
  "AEC_DISP_REP_COL_POINT2D",                                 /* 1322 */
  "AEC_DISP_REP_COL_PROFILE",                                 /* 1323 */
  "AEC_DISP_REP_COLUMN_GRID",                                 /* 1324 */
  "AEC_DISP_REP_COLUMN_GRID_MODEL",                           /* 1325 */
  "AEC_DISP_REP_COLUMN_GRID_RCP",                             /* 1326 */
  "AEC_DISP_REP_COL_WORKPLANE",                               /* 1327 */
  "AEC_DISP_REP_COL_WORKPLANE_REF",                           /* 1328 */
  "AEC_DISP_REP_CONFIG",                                      /* 1329 */
  "AEC_DISP_REP_CURTAIN_WALL_LAYOUT_MODEL",                   /* 1330 */
  "AEC_DISP_REP_CURTAIN_WALL_LAYOUT_PLAN",                    /* 1331 */
  "AEC_DISP_REP_CURTAIN_WALL_UNIT_MODEL",                     /* 1332 */
  "AEC_DISP_REP_CURTAIN_WALL_UNIT_PLAN",                      /* 1333 */
  "AEC_DISP_REP_DCM_DIMRADIUS",                               /* 1334 */
  "AEC_DISP_REP_DISPLAYTHEME",                                /* 1335 */
  "AEC_DISP_REP_DOOR_ELEV",                                   /* 1336 */
  "AEC_DISP_REP_DOOR_MODEL",                                  /* 1337 */
  "AEC_DISP_REP_DOOR_NOMINAL",                                /* 1338 */
  "AEC_DISP_REP_DOOR_PLAN",                                   /* 1339 */
  "AEC_DISP_REP_DOOR_PLAN_50",                                /* 1340 */
  "AEC_DISP_REP_DOOR_PLAN_HEKTO",                             /* 1341 */
  "AEC_DISP_REP_DOOR_RCP",                                    /* 1342 */
  "AEC_DISP_REP_DOOR_THRESHOLD_PLAN",                         /* 1343 */
  "AEC_DISP_REP_DOOR_THRESHOLD_SYMBOL_PLAN",                  /* 1344 */
  "AEC_DISP_REP_EDITINPLACEPROFILE",                          /* 1345 */
  "AEC_DISP_REP_ENT_REF",                                     /* 1346 */
  "AEC_DISP_REP_LAYOUT_CURVE",                                /* 1347 */
  "AEC_DISP_REP_LAYOUT_GRID2D",                               /* 1348 */
  "AEC_DISP_REP_LAYOUT_GRID3D",                               /* 1349 */
  "AEC_DISP_REP_MASKBLOCK_REF",                               /* 1350 */
  "AEC_DISP_REP_MASKBLOCK_REF_RCP",                           /* 1351 */
  "AEC_DISP_REP_MASS_ELEM_MODEL",                             /* 1352 */
  "AEC_DISP_REP_MASS_ELEM_RCP",                               /* 1353 */
  "AEC_DISP_REP_MASS_ELEM_SCHEM",                             /* 1354 */
  "AEC_DISP_REP_MASS_GROUP_MODEL",                            /* 1355 */
  "AEC_DISP_REP_MASS_GROUP_PLAN",                             /* 1356 */
  "AEC_DISP_REP_MASS_GROUP_RCP",                              /* 1357 */
  "AEC_DISP_REP_MATERIAL",                                    /* 1358 */
  "AEC_DISP_REP_MVBLOCK_REF",                                 /* 1359 */
  "AEC_DISP_REP_MVBLOCK_REF_MODEL",                           /* 1360 */
  "AEC_DISP_REP_MVBLOCK_REF_RCP",                             /* 1361 */
  "AEC_DISP_REP_OPENING",                                     /* 1362 */
  "AEC_DISP_REP_OPENING_MODEL",                               /* 1363 */
  "AEC_DISP_REP_POLYGON_MODEL",                               /* 1364 */
  "AEC_DISP_REP_POLYGON_TRUECOLOUR",                          /* 1365 */
  "AEC_DISP_REP_RAILING_MODEL",                               /* 1366 */
  "AEC_DISP_REP_RAILING_PLAN",                                /* 1367 */
  "AEC_DISP_REP_RAILING_PLAN_100",                            /* 1368 */
  "AEC_DISP_REP_RAILING_PLAN_50",                             /* 1369 */
  "AEC_DISP_REP_ROOF_MODEL",                                  /* 1370 */
  "AEC_DISP_REP_ROOF_PLAN",                                   /* 1371 */
  "AEC_DISP_REP_ROOF_RCP",                                    /* 1372 */
  "AEC_DISP_REP_ROOFSLAB_MODEL",                              /* 1373 */
  "AEC_DISP_REP_ROOFSLAB_PLAN",                               /* 1374 */
  "AEC_DISP_REP_SCHEDULE_TABLE",                              /* 1375 */
  "AEC_DISP_REP_SET",                                         /* 1376 */
  "AEC_DISP_REP_SLAB_MODEL",                                  /* 1377 */
  "AEC_DISP_REP_SLAB_PLAN",                                   /* 1378 */
  "AEC_DISP_REP_SLICE",                                       /* 1379 */
  "AEC_DISP_REP_SPACE_DECOMPOSED",                            /* 1380 */
  "AEC_DISP_REP_SPACE_MODEL",                                 /* 1381 */
  "AEC_DISP_REP_SPACE_PLAN",                                  /* 1382 */
  "AEC_DISP_REP_SPACE_RCP",                                   /* 1383 */
  "AEC_DISP_REP_SPACE_VOLUME",                                /* 1384 */
  "AEC_DISP_REP_STAIR_MODEL",                                 /* 1385 */
  "AEC_DISP_REP_STAIR_PLAN",                                  /* 1386 */
  "AEC_DISP_REP_STAIR_PLAN_100",                              /* 1387 */
  "AEC_DISP_REP_STAIR_PLAN_50",                               /* 1388 */
  "AEC_DISP_REP_STAIR_PLAN_OVERLAPPING",                      /* 1389 */
  "AEC_DISP_REP_STAIR_RCP",                                   /* 1390 */
  "AEC_DISP_REP_WALL_GRAPH",                                  /* 1391 */
  "AEC_DISP_REP_WALL_MODEL",                                  /* 1392 */
  "AEC_DISP_REP_WALL_PLAN",                                   /* 1393 */
  "AEC_DISP_REP_WALL_RCP",                                    /* 1394 */
  "AEC_DISP_REP_WALL_SCHEM",                                  /* 1395 */
  "AEC_DISP_REP_WINDOW_ASSEMBLY_MODEL",                       /* 1396 */
  "AEC_DISP_REP_WINDOW_ASSEMBLY_PLAN",                        /* 1397 */
  "AEC_DISP_REP_WINDOWASSEMBLY_SILL_PLAN",                    /* 1398 */
  "AEC_DISP_REP_WINDOW_ELEV",                                 /* 1399 */
  "AEC_DISP_REP_WINDOW_MODEL",                                /* 1400 */
  "AEC_DISP_REP_WINDOW_NOMINAL",                              /* 1401 */
  "AEC_DISP_REP_WINDOW_PLAN",                                 /* 1402 */
  "AEC_DISP_REP_WINDOW_PLAN_100",                             /* 1403 */
  "AEC_DISP_REP_WINDOW_PLAN_50",                              /* 1404 */
  "AEC_DISP_REP_WINDOW_RCP",                                  /* 1405 */
  "AEC_DISP_REP_WINDOW_SILL_PLAN",                            /* 1406 */
  "AEC_DISP_REP_ZONE",                                        /* 1407 */
  "AEC_DISPROPSMASSELEMPLANCOMMON",                           /* 1408 */
  "AEC_DISPROPSMASSGROUPPLANCOMMON",                          /* 1409 */
  "AEC_DISPROPSOPENINGPLANCOMMON",                            /* 1410 */
  "AEC_DISPROPSOPENINGPLANCOMMONHATCHED",                     /* 1411 */
  "AEC_DISPROPSOPENINGSILLPLAN",                              /* 1412 */
  "AEC_DISP_ROPS_RAILING_PLAN_100",                           /* 1413 */
  "AEC_DISP_ROPS_RAILING_PLAN_50",                            /* 1414 */
  "AEC_DISP_ROPS_STAIR_PLAN_100",                             /* 1415 */
  "AEC_DISP_ROPS_STAIR_PLAN_50",                              /* 1416 */
  "AEC_DOOR_STYLE",                                           /* 1417 */
  "AEC_ENDCAP_STYLE",                                         /* 1418 */
  "AEC_FRAME_DEF",                                            /* 1419 */
  "AEC_LAYERKEY_STYLE",                                       /* 1420 */
  "AEC_LIST_DEF",                                             /* 1421 */
  "AEC_MASKBLOCK_DEF",                                        /* 1422 */
  "AEC_MASS_ELEM_STYLE",                                      /* 1423 */
  "AEC_MATERIAL_DEF",                                         /* 1424 */
  "AEC_MVBLOCK_DEF",                                          /* 1425 */
  "AEC_MVBLOCK_REF",                                          /* 1426 */
  "AEC_NOTIFICATION_TRACKER",                                 /* 1427 */
  "AEC_POLYGON",                                              /* 1428 */
  "AEC_POLYGON_STYLE",                                        /* 1429 */
  "AEC_PROPERTY_SET_DEF",                                     /* 1430 */
  "AEC_RAILING_STYLE",                                        /* 1431 */
  "AEC_REFEDIT_STATUS_TRACKER",                               /* 1432 */
  "AEC_ROOFSLABEDGE_STYLE",                                   /* 1433 */
  "AEC_ROOFSLAB_STYLE",                                       /* 1434 */
  "AEC_SCHEDULE_DATA_FORMAT",                                 /* 1435 */
  "AEC_SLABEDGE_STYLE",                                       /* 1436 */
  "AEC_SLAB_STYLE",                                           /* 1437 */
  "AEC_SPACE_STYLES",                                         /* 1438 */
  "AEC_STAIR_STYLE",                                          /* 1439 */
  "AEC_STAIR_WINDER_STYLE",                                   /* 1440 */
  "AEC_STAIR_WINDER_TYPE_BALANCED",                           /* 1441 */
  "AEC_STAIR_WINDER_TYPE_MANUAL",                             /* 1442 */
  "AEC_STAIR_WINDER_TYPE_SINGLE_POINT",                       /* 1443 */
  "AEC_VARS_AECBBLDSRV",                                      /* 1444 */
  "AEC_VARS_ARCHBASE",                                        /* 1445 */
  "AEC_VARS_DWG_SETUP",                                       /* 1446 */
  "AEC_VARS_MUNICH",                                          /* 1447 */
  "AEC_VARS_STRUCTUREBASE",                                   /* 1448 */
  "AEC_WALLMOD_STYLE",                                        /* 1449 */
  "AEC_WALL_STYLE",                                           /* 1450 */
  "AEC_WINDOW_ASSEMBLY_STYLE",                                /* 1451 */
  "AEC_WINDOW_STYLE",                                         /* 1452 */
  "ALIGNMENTGRIPENTITY",                                      /* 1453 */
  "AMCONTEXTMGR",                                             /* 1454 */
  "AMDTADMENUSTATE",                                          /* 1455 */
  "AMDTAMMENUSTATE",                                          /* 1456 */
  "AMDTBROWSERDBTAB",                                         /* 1457 */
  "AMDTDMMENUSTATE",                                          /* 1458 */
  "AMDTEDGESTANDARDDIN",                                      /* 1459 */
  "AMDTEDGESTANDARDDIN13715",                                 /* 1460 */
  "AMDTEDGESTANDARDISO",                                      /* 1461 */
  "AMDTEDGESTANDARDISO13715",                                 /* 1462 */
  "AMDTFORMULAUPDATEDISPATCHER",                              /* 1463 */
  "AMDTINTERNALREACTOR",                                      /* 1464 */
  "AMDTMCOMMENUSTATE",                                        /* 1465 */
  "AMDTMENUSTATEMGR",                                         /* 1466 */
  "AMDTNOTE",                                                 /* 1467 */
  "AMDTNOTETEMPLATEDB",                                       /* 1468 */
  "AMDTSECTIONSYM",                                           /* 1469 */
  "AMDTSECTIONSYMLABEL",                                      /* 1470 */
  "AMDTSYSATTR",                                              /* 1471 */
  "AMGOBJPROPCFG",                                            /* 1472 */
  "AMGSETTINGSOBJ",                                           /* 1473 */
  "AMIMASTER",                                                /* 1474 */
  "AM_DRAWING_MGR",                                           /* 1475 */
  "AM_DWGMGR_NAME",                                           /* 1476 */
  "AM_DWG_DOCUMENT",                                          /* 1477 */
  "AM_DWG_SHEET",                                             /* 1478 */
  "AM_VIEWDIMPARMAP",                                         /* 1479 */
  "BINRECORD",                                                /* 1480 */
  "CAMSCATALOGAPPOBJECT",                                     /* 1481 */
  "CAMSSTRUCTBTNSTATE",                                       /* 1482 */
  "CATALOGSTATE",                                             /* 1483 */
  "CBROWSERAPPOBJECT",                                        /* 1484 */
  "DEPMGR",                                                   /* 1485 */
  "DMBASEELEMENT",                                            /* 1486 */
  "DMDEFAULTSTYLE",                                           /* 1487 */
  "DMLEGEND",                                                 /* 1488 */
  "DMMAP",                                                    /* 1489 */
  "DMMAPMANAGER",                                             /* 1490 */
  "DMSTYLECATEGORY",                                          /* 1491 */
  "DMSTYLELIBRARY",                                           /* 1492 */
  "DMSTYLEREFERENCE",                                         /* 1493 */
  "DMSTYLIZEDENTITIESTABLE",                                  /* 1494 */
  "DMSURROGATESTYLESETS",                                     /* 1495 */
  "DM_PLACEHOLDER",                                           /* 1496 */
  "EXACTERMXREFMAP",                                          /* 1497 */
  "EXACXREFPANELOBJECT",                                      /* 1498 */
  "EXPO_NOTIFYBLOCK",                                         /* 1499 */
  "EXPO_NOTIFYHALL",                                          /* 1500 */
  "EXPO_NOTIFYPILLAR",                                        /* 1501 */
  "EXPO_NOTIFYSTAND",                                         /* 1502 */
  "EXPO_NOTIFYSTANDNOPOLY",                                   /* 1503 */
  "GSMANAGER",                                                /* 1504 */
  "IRD_DSC_DICT",                                             /* 1505 */
  "IRD_DSC_RECORD",                                           /* 1506 */
  "IRD_OBJ_RECORD",                                           /* 1507 */
  "MAPFSMRVOBJECT",                                           /* 1508 */
  "MAPGWSUNDOOBJECT",                                         /* 1509 */
  "MAPIAMMOUDLE",                                             /* 1510 */
  "MAPMETADATAOBJECT",                                        /* 1511 */
  "MAPRESOURCEMANAGEROBJECT",                                 /* 1512 */
  "McDbContainer2",                                           /* 1513 */
  "McDbMarker",                                               /* 1514 */
  "NAMEDAPPL",                                                /* 1515 */
  "NEWSTDPARTPARLIST",                                        /* 1516 */
  "NOTEPOSITION",                                             /* 1517 */
  "OBJCLONER",                                                /* 1518 */
  "PARAMMGR",                                                 /* 1519 */
  "PARAMSCOPE",                                               /* 1520 */
  "PILLAR",                                                   /* 1521 */
  "STDPART2D",                                                /* 1522 */
  "TCH_ARROW",                                                /* 1523 */
  "TCH_AXIS_LABEL",                                           /* 1524 */
  "TCH_BLOCK_INSERT",                                         /* 1525 */
  "TCH_COLUMN",                                               /* 1526 */
  "TCH_DBCONFIG",                                             /* 1527 */
  "TCH_DIMENSION2",                                           /* 1528 */
  "TCH_DRAWINGINDEX",                                         /* 1529 */
  "TCH_HANDRAIL",                                             /* 1530 */
  "TCH_LINESTAIR",                                            /* 1531 */
  "TCH_OPENING",                                              /* 1532 */
  "TCH_RECTSTAIR",                                            /* 1533 */
  "TCH_SLAB",                                                 /* 1534 */
  "TCH_SPACE",                                                /* 1535 */
  "TCH_TEXT",                                                 /* 1536 */
  "TCH_WALL",                                                 /* 1537 */
  "TGrupoPuntos",                                             /* 1538 */
  "VAACIMAGEINVENTORY",                                       /* 1539 */
  "VAACXREFPANELOBJECT",                                      /* 1540 */
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
  // linear, TODO better binary search. Or a switch jumptable, as in free.
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
          return !strEQc (name, "MATERIAL") &&     // 72 missing
                 !strEQc (name, "SUN") &&          // 421 missing
                 !strEQc (name, "PROXY_ENTITY") && // 90 missing
                 !strEQc (name, "PROXY_OBJECT");   // 90 missing
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
  "ACIDBLOCKREFERENCE",                                       /* 502 */
  "ACMECOMMANDHISTORY",                                       /* 503 */
  "ACMESCOPE",                                                /* 504 */
  "ACMESTATEMGR",                                             /* 505 */
  "ACSH_BOOLEAN_CLASS",                                       /* 506 */
  "ACSH_BOX_CLASS",                                           /* 507 */
  "ACSH_BREP_CLASS",                                          /* 508 */
  "ACSH_CHAMFER_CLASS",                                       /* 509 */
  "ACSH_CONE_CLASS",                                          /* 510 */
  "ACSH_CYLINDER_CLASS",                                      /* 511 */
  "ACSH_EXTRUSION_CLASS",                                     /* 512 */
  "ACSH_FILLET_CLASS",                                        /* 513 */
  "ACSH_HISTORY_CLASS",                                       /* 514 */
  "ACSH_LOFT_CLASS",                                          /* 515 */
  "ACSH_PYRAMID_CLASS",                                       /* 516 */
  "ACSH_REVOLVE_CLASS",                                       /* 517 */
  "ACSH_SPHERE_CLASS",                                        /* 518 */
  "ACSH_SWEEP_CLASS",                                         /* 519 */
  "ACSH_TORUS_CLASS",                                         /* 520 */
  "ACSH_WEDGE_CLASS",                                         /* 521 */
  "ACDB_ALDIMOBJECTCONTEXTDATA_CLASS",                        /* 522 */
  "ALIGNMENTPARAMETERENTITY",                                 /* 523 */
  "ACDB_ANGDIMOBJECTCONTEXTDATA_CLASS",                       /* 524 */
  "ACDB_ANNOTSCALEOBJECTCONTEXTDATA_CLASS",                   /* 525 */
  "ARCALIGNEDTEXT",                                           /* 526 */
  "ARC_DIMENSION",                                            /* 527 */
  "ACDBASSOC2DCONSTRAINTGROUP",                               /* 528 */
  "ACDBASSOC3POINTANGULARDIMACTIONBODY",                      /* 529 */
  "ACDBASSOCACTION",                                          /* 530 */
  "ACDBASSOCACTIONPARAM",                                     /* 531 */
  "ACDBASSOCALIGNEDDIMACTIONBODY",                            /* 532 */
  "ACDBASSOCARRAYACTIONBODY",                                 /* 533 */
  "ACDBASSOCARRAYMODIFYACTIONBODY",                           /* 534 */
  "ACDBASSOCARRAYMODIFYPARAMETERS",                           /* 535 */
  "ACDBASSOCARRAYPATHPARAMETERS",                             /* 536 */
  "ACDBASSOCARRAYPOLARPARAMETERS",                            /* 537 */
  "ACDBASSOCARRAYRECTANGULARPARAMETERS",                      /* 538 */
  "ACDBASSOCASMBODYACTIONPARAM",                              /* 539 */
  "ACDBASSOCBLENDSURFACEACTIONBODY",                          /* 540 */
  "ACDBASSOCCOMPOUNDACTIONPARAM",                             /* 541 */
  "ACDBASSOCDEPENDENCY",                                      /* 542 */
  "ACDBASSOCDIMDEPENDENCYBODY",                               /* 543 */
  "ACDBASSOCEDGEACTIONPARAM",                                 /* 544 */
  "ACDBASSOCEDGECHAMFERACTIONBODY",                           /* 545 */
  "ACDBASSOCEDGEFILLETACTIONBODY",                            /* 546 */
  "ACDBASSOCEXTENDSURFACEACTIONBODY",                         /* 547 */
  "ACDBASSOCEXTRUDEDSURFACEACTIONBODY",                       /* 548 */
  "ACDBASSOCFACEACTIONPARAM",                                 /* 549 */
  "ACDBASSOCFILLETSURFACEACTIONBODY",                         /* 550 */
  "ACDBASSOCGEOMDEPENDENCY",                                  /* 551 */
  "ACDBASSOCLOFTEDSURFACEACTIONBODY",                         /* 552 */
  "ACDBASSOCMLEADERACTIONBODY",                               /* 553 */
  "ACDBASSOCNETWORK",                                         /* 554 */
  "ACDBASSOCNETWORKSURFACEACTIONBODY",                        /* 555 */
  "ACDBASSOCOBJECTACTIONPARAM",                               /* 556 */
  "ACDBASSOCOFFSETSURFACEACTIONBODY",                         /* 557 */
  "ACDBASSOCORDINATEDIMACTIONBODY",                           /* 558 */
  "ACDBASSOCOSNAPPOINTREFACTIONPARAM",                        /* 559 */
  "ACDBASSOCPATCHSURFACEACTIONBODY",                          /* 560 */
  "ACDBASSOCPATHACTIONPARAM",                                 /* 561 */
  "ACDBASSOCPERSSUBENTMANAGER",                               /* 562 */
  "ACDBASSOCPLANESURFACEACTIONBODY",                          /* 563 */
  "ACDBASSOCPOINTREFACTIONPARAM",                             /* 564 */
  "ACDBASSOCRESTOREENTITYSTATEACTIONBODY",                    /* 565 */
  "ACDBASSOCREVOLVEDSURFACEACTIONBODY",                       /* 566 */
  "ACDBASSOCROTATEDDIMACTIONBODY",                            /* 567 */
  "ACDBASSOCSWEPTSURFACEACTIONBODY",                          /* 568 */
  "ACDBASSOCTRIMSURFACEACTIONBODY",                           /* 569 */
  "ACDBASSOCVALUEDEPENDENCY",                                 /* 570 */
  "ACDBASSOCVARIABLE",                                        /* 571 */
  "ACDBASSOCVERTEXACTIONPARAM",                               /* 572 */
  "BASEPOINTPARAMETERENTITY",                                 /* 573 */
  "ACDB_BLKREFOBJECTCONTEXTDATA_CLASS",                       /* 574 */
  "BLOCKALIGNEDCONSTRAINTPARAMETER",                          /* 575 */
  "BLOCKALIGNMENTGRIP",                                       /* 576 */
  "BLOCKALIGNMENTPARAMETER",                                  /* 577 */
  "BLOCKANGULARCONSTRAINTPARAMETER",                          /* 578 */
  "BLOCKARRAYACTION",                                         /* 579 */
  "BLOCKBASEPOINTPARAMETER",                                  /* 580 */
  "BLOCKDIAMETRICCONSTRAINTPARAMETER",                        /* 581 */
  "BLOCKFLIPACTION",                                          /* 582 */
  "BLOCKFLIPGRIP",                                            /* 583 */
  "BLOCKFLIPPARAMETER",                                       /* 584 */
  "BLOCKGRIPLOCATIONCOMPONENT",                               /* 585 */
  "BLOCKHORIZONTALCONSTRAINTPARAMETER",                       /* 586 */
  "BLOCKLINEARCONSTRAINTPARAMETER",                           /* 587 */
  "BLOCKLINEARGRIP",                                          /* 588 */
  "BLOCKLINEARPARAMETER",                                     /* 589 */
  "BLOCKLOOKUPACTION",                                        /* 590 */
  "BLOCKLOOKUPGRIP",                                          /* 591 */
  "BLOCKLOOKUPPARAMETER",                                     /* 592 */
  "BLOCKMOVEACTION",                                          /* 593 */
  "BLOCKPARAMDEPENDENCYBODY",                                 /* 594 */
  "BLOCKPOINTPARAMETER",                                      /* 595 */
  "BLOCKPOLARGRIP",                                           /* 596 */
  "BLOCKPOLARPARAMETER",                                      /* 597 */
  "BLOCKPOLARSTRETCHACTION",                                  /* 598 */
  "BLOCKPROPERTIESTABLE",                                     /* 599 */
  "BLOCKPROPERTIESTABLEGRIP",                                 /* 600 */
  "BLOCKRADIALCONSTRAINTPARAMETER",                           /* 601 */
  "ACDB_BLOCKREPRESENTATION_DATA",                            /* 602 */
  "BLOCKROTATEACTION",                                        /* 603 */
  "BLOCKROTATIONGRIP",                                        /* 604 */
  "BLOCKROTATIONPARAMETER",                                   /* 605 */
  "BLOCKSCALEACTION",                                         /* 606 */
  "BLOCKSTRETCHACTION",                                       /* 607 */
  "BLOCKUSERPARAMETER",                                       /* 608 */
  "BLOCKVERTICALCONSTRAINTPARAMETER",                         /* 609 */
  "BLOCKVISIBILITYGRIP",                                      /* 610 */
  "BLOCKVISIBILITYPARAMETER",                                 /* 611 */
  "BLOCKXYGRIP",                                              /* 612 */
  "BLOCKXYPARAMETER",                                         /* 613 */
  "CAMERA",                                                   /* 614 */
  "CELLSTYLEMAP",                                             /* 615 */
  "CONTEXTDATAMANAGER",                                       /* 616 */
  "CSACDOCUMENTOPTIONS",                                      /* 617 */
  "ACDBCURVEPATH",                                            /* 618 */
  "DATALINK",                                                 /* 619 */
  "DATATABLE",                                                /* 620 */
  "DBCOLOR",                                                  /* 621 */
  "ACDBDETAILVIEWSTYLE",                                      /* 622 */
  "DGNDEFINITION",                                            /* 623 */
  "DGNUNDERLAY",                                              /* 624 */
  "DICTIONARYVAR",                                            /* 625 */
  "ACDBDICTIONARYWDFLT",                                      /* 626 */
  "DIMASSOC",                                                 /* 627 */
  "ACDB_DMDIMOBJECTCONTEXTDATA_CLASS",                        /* 628 */
  "DWFDEFINITION",                                            /* 629 */
  "DWFUNDERLAY",                                              /* 630 */
  "ACAD_DYNAMICBLOCKPROXYNODE",                               /* 631 */
  "ACDB_DYNAMICBLOCKPURGEPREVENTER_VERSION",                  /* 632 */
  "ACAD_EVALUATION_GRAPH",                                    /* 633 */
  "EXTRUDEDSURFACE",                                          /* 634 */
  "ACDB_FCFOBJECTCONTEXTDATA_CLASS",                          /* 635 */
  "FIELD",                                                    /* 636 */
  "FIELDLIST",                                                /* 637 */
  "FLIPPARAMETERENTITY",                                      /* 638 */
  "FLIPACTIONENTITY",                                         /* 639 */
  "GEODATA",                                                  /* 640 */
  "GEOMAPIMAGE",                                              /* 641 */
  "POSITIONMARKER",                                           /* 642 */
  "GRADIENT_BACKGROUND",                                      /* 643 */
  "GROUND_PLANE_BACKGROUND",                                  /* 644 */
  "HELIX",                                                    /* 645 */
  "RAPIDRTRENDERENVIRONMENT",                                 /* 646 */
  "IDBUFFER",                                                 /* 647 */
  "IMAGE",                                                    /* 648 */
  "IMAGEDEF",                                                 /* 649 */
  "IMAGEDEF_REACTOR",                                         /* 650 */
  "IMAGE_BACKGROUND",                                         /* 651 */
  "INDEX",                                                    /* 652 */
  "LARGE_RADIAL_DIMENSION",                                   /* 653 */
  "LAYER_FILTER",                                             /* 654 */
  "LAYER_INDEX",                                              /* 655 */
  "LAYOUTPRINTCONFIG",                                        /* 656 */
  "ACDB_LEADEROBJECTCONTEXTDATA_CLASS",                       /* 657 */
  "LIGHT",                                                    /* 658 */
  "LIGHTLIST",                                                /* 659 */
  "LINEARPARAMETERENTITY",                                    /* 660 */
  "LOFTEDSURFACE",                                            /* 661 */
  "MATERIAL",                                                 /* 662 */
  "MENTALRAYRENDERSETTINGS",                                  /* 663 */
  "MESH",                                                     /* 664 */
  "ACDB_MLEADEROBJECTCONTEXTDATA_CLASS",                      /* 665 */
  "MLEADERSTYLE",                                             /* 666 */
  "MOVEACTIONENTITY",                                         /* 667 */
  "ACDBMOTIONPATH",                                           /* 668 */
  "MPOLYGON",                                                 /* 669 */
  "ACDB_MTEXTATTRIBUTEOBJECTCONTEXTDATA_CLASS",               /* 670 */
  "ACDB_MTEXTOBJECTCONTEXTDATA_CLASS",                        /* 671 */
  "MULTILEADER",                                              /* 672 */
  "COORDINATION_MODEL",                                       /* 673 */
  "ACDBNAVISWORKSMODELDEF",                                   /* 674 */
  "NPOCOLLECTION",                                            /* 675 */
  "NURBSURFACE",                                              /* 676 */
  "OBJECT_PTR",                                               /* 677 */
  "ACDB_ORDDIMOBJECTCONTEXTDATA_CLASS",                       /* 678 */
  "PARTIAL_VIEWING_INDEX",                                    /* 679 */
  "PDFDEFINITION",                                            /* 680 */
  "PDFUNDERLAY",                                              /* 681 */
  "ACDBPERSSUBENTMANAGER",                                    /* 682 */
  "PLANESURFACE",                                             /* 683 */
  "PLOTSETTINGS",                                             /* 684 */
  "ACDBPOINTCLOUD",                                           /* 685 */
  "ACDBPOINTCLOUDCOLORMAP",                                   /* 686 */
  "ACDBPOINTCLOUDDEF",                                        /* 687 */
  "ACDBPOINTCLOUDDEFEX",                                      /* 688 */
  "ACDBPOINTCLOUDDEF_REACTOR",                                /* 689 */
  "ACDBPOINTCLOUDDEF_REACTOR_EX",                             /* 690 */
  "ACDBPOINTCLOUDEX",                                         /* 691 */
  "POINTPARAMETERENTITY",                                     /* 692 */
  "ACDBPOINTPATH",                                            /* 693 */
  "POLARGRIPENTITY",                                          /* 694 */
  "ACDB_RADIMLGOBJECTCONTEXTDATA_CLASS",                      /* 695 */
  "ACDB_RADIMOBJECTCONTEXTDATA_CLASS",                        /* 696 */
  "RAPIDRTRENDERSETTINGS",                                    /* 697 */
  "RASTERVARIABLES",                                          /* 698 */
  "RENDERENTRY",                                              /* 699 */
  "RENDERENVIRONMENT",                                        /* 700 */
  "RENDERGLOBAL",                                             /* 701 */
  "RENDERSETTINGS",                                           /* 702 */
  "REVOLVEDSURFACE",                                          /* 703 */
  "ROTATIONPARAMETERENTITY",                                  /* 704 */
  "ROTATEACTIONENTITY",                                       /* 705 */
  "RTEXT",                                                    /* 706 */
  "SCALE",                                                    /* 707 */
  "SCALEACTIONENTITY",                                        /* 708 */
  "SECTIONOBJECT",                                            /* 709 */
  "ACDBSECTIONVIEWSTYLE",                                     /* 710 */
  "SECTION_MANAGER",                                          /* 711 */
  "SECTION_SETTINGS",                                         /* 712 */
  "SKYLIGHT_BACKGROUND",                                      /* 713 */
  "SOLID_BACKGROUND",                                         /* 714 */
  "SORTENTSTABLE",                                            /* 715 */
  "SPATIAL_FILTER",                                           /* 716 */
  "SPATIAL_INDEX",                                            /* 717 */
  "STRETCHACTIONENTITY",                                      /* 718 */
  "SUN",                                                      /* 719 */
  "SUNSTUDY",                                                 /* 720 */
  "SWEPTSURFACE",                                             /* 721 */
  "ACAD_TABLE",                                               /* 722 */
  "TABLECONTENT",                                             /* 723 */
  "TABLEGEOMETRY",                                            /* 724 */
  "TABLESTYLE",                                               /* 725 */
  "ACDB_TEXTOBJECTCONTEXTDATA_CLASS",                         /* 726 */
  "TVDEVICEPROPERTIES",                                       /* 727 */
  "VISIBILITYGRIPENTITY",                                     /* 728 */
  "VISIBILITYPARAMETERENTITY",                                /* 729 */
  "VISUALSTYLE",                                              /* 730 */
  "WIPEOUT",                                                  /* 731 */
  "WIPEOUTVARIABLES",                                         /* 732 */
  "EXACXREFPANELOBJECT",                                      /* 733 */
  "XYPARAMETERENTITY",                                        /* 734 */
  "BREAKDATA",                                                /* 735 */
  "BREAKPOINTREF",                                            /* 736 */
  "FLIPGRIPENTITY",                                           /* 737 */
  "LINEARGRIPENTITY",                                         /* 738 */
  "ROTATIONGRIPENTITY",                                       /* 739 */
  "XYGRIPENTITY",                                             /* 740 */
  "3DLINE",                                                   /* 741 */
  "REPEAT",                                                   /* 742 */
  "ENDREP",                                                   /* 743 */
  "JUMP",                                                     /* 744 */
  "LOAD",                                                     /* 745 */
  "ABSHDRAWINGSETTINGS",                                      /* 746 */
  "ACAECUSTOBJ",                                              /* 747 */
  "ACAEEEMGROBJ",                                             /* 748 */
  "ACAMCOMP",                                                 /* 749 */
  "ACAMCOMPDEF",                                              /* 750 */
  "ACAMCOMPDEFMGR",                                           /* 751 */
  "ACAMCONTEXTMODELER",                                       /* 752 */
  "ACAMGDIMSTD",                                              /* 753 */
  "ACAMGFILTERDAT",                                           /* 754 */
  "ACAMGHOLECHARTSTDCSN",                                     /* 755 */
  "ACAMGHOLECHARTSTDDIN",                                     /* 756 */
  "ACAMGHOLECHARTSTDISO",                                     /* 757 */
  "ACAMGLAYSTD",                                              /* 758 */
  "ACAMGRCOMPDEF",                                            /* 759 */
  "ACAMGRCOMPDEFSET",                                         /* 760 */
  "ACAMGTITLESTD",                                            /* 761 */
  "ACAMMVDBACKUPOBJECT",                                      /* 762 */
  "ACAMPROJECT",                                              /* 763 */
  "ACAMSHAFTCOMPDEF",                                         /* 764 */
  "ACAMSTDPCOMPDEF",                                          /* 765 */
  "ACAMWBLOCKTEMPENTS",                                       /* 766 */
  "ACARRAYJIGENTITY",                                         /* 767 */
  "ACCMCONTEXT",                                              /* 768 */
  "ACDBCIRCARCRES",                                           /* 769 */
  "ACDBDIMENSIONRES",                                         /* 770 */
  "ACDBENTITYCACHE",                                          /* 771 */
  "ACDBLINERES",                                              /* 772 */
  "ACDBSTDPARTRES_ARC",                                       /* 773 */
  "ACDBSTDPARTRES_LINE",                                      /* 774 */
  "ACDB_HATCHSCALECONTEXTDATA_CLASS",                         /* 775 */
  "ACDB_HATCHVIEWCONTEXTDATA_CLASS",                          /* 776 */
  "ACDB_PROXY_ENTITY_DATA",                                   /* 777 */
  "ACGREFACADMASTER",                                         /* 778 */
  "ACGREFMASTER",                                             /* 779 */
  "ACIMINTSYSVAR",                                            /* 780 */
  "ACIMREALSYSVAR",                                           /* 781 */
  "ACIMSTRSYSVAR",                                            /* 782 */
  "ACIMSYSVARMAN",                                            /* 783 */
  "ACMANOOTATIONVIEWSTANDARDANSI",                            /* 784 */
  "ACMANOOTATIONVIEWSTANDARDCSN",                             /* 785 */
  "ACMANOOTATIONVIEWSTANDARDDIN",                             /* 786 */
  "ACMANOOTATIONVIEWSTANDARDISO",                             /* 787 */
  "ACMAPLEGENDDBOBJECT",                                      /* 788 */
  "ACMAPLEGENDITEMDBOBJECT",                                  /* 789 */
  "ACMAPMAPVIEWPORTDBOBJECT",                                 /* 790 */
  "ACMAPPRINTLAYOUTELEMENTDBOBJECTCONTAINER",                 /* 791 */
  "ACMBALLOON",                                               /* 792 */
  "ACMBOM",                                                   /* 793 */
  "ACMBOMROW",                                                /* 794 */
  "ACMBOMROWSTRUCT",                                          /* 795 */
  "ACMBOMSTANDARDANSI",                                       /* 796 */
  "ACMBOMSTANDARDCSN",                                        /* 797 */
  "ACMBOMSTANDARDDIN",                                        /* 798 */
  "ACMBOMSTANDARDISO",                                        /* 799 */
  "ACMCENTERLINESTANDARDANSI",                                /* 800 */
  "ACMCENTERLINESTANDARDCSN",                                 /* 801 */
  "ACMCENTERLINESTANDARDDIN",                                 /* 802 */
  "ACMCENTERLINESTANDARDISO",                                 /* 803 */
  "ACMDATADICTIONARY",                                        /* 804 */
  "ACMDATAENTRY",                                             /* 805 */
  "ACMDATAENTRYBLOCK",                                        /* 806 */
  "ACMDATUMID",                                               /* 807 */
  "ACMDATUMSTANDARDANSI",                                     /* 808 */
  "ACMDATUMSTANDARDCSN",                                      /* 809 */
  "ACMDATUMSTANDARDDIN",                                      /* 810 */
  "ACMDATUMSTANDARDISO",                                      /* 811 */
  "ACMDATUMSTANDARDISO2012",                                  /* 812 */
  "ACMDETAILSTANDARDANSI",                                    /* 813 */
  "ACMDETAILSTANDARDCSN",                                     /* 814 */
  "ACMDETAILSTANDARDDIN",                                     /* 815 */
  "ACMDETAILSTANDARDISO",                                     /* 816 */
  "ACMDETAILTANDARDCUSTOM",                                   /* 817 */
  "ACMDIMBREAKPERSREACTOR",                                   /* 818 */
  "ACMEDRAWINGMAN",                                           /* 819 */
  "ACMEVIEW",                                                 /* 820 */
  "ACME_DATABASE",                                            /* 821 */
  "ACME_DOCUMENT",                                            /* 822 */
  "ACMFCFRAME",                                               /* 823 */
  "ACMFCFSTANDARDANSI",                                       /* 824 */
  "ACMFCFSTANDARDCSN",                                        /* 825 */
  "ACMFCFSTANDARDDIN",                                        /* 826 */
  "ACMFCFSTANDARDISO",                                        /* 827 */
  "ACMFCFSTANDARDISO2004",                                    /* 828 */
  "ACMFCFSTANDARDISO2012",                                    /* 829 */
  "ACMIDSTANDARDANSI",                                        /* 830 */
  "ACMIDSTANDARDCSN",                                         /* 831 */
  "ACMIDSTANDARDDIN",                                         /* 832 */
  "ACMIDSTANDARDISO",                                         /* 833 */
  "ACMIDSTANDARDISO2004",                                     /* 834 */
  "ACMIDSTANDARDISO2012",                                     /* 835 */
  "ACMNOTESTANDARDANSI",                                      /* 836 */
  "ACMNOTESTANDARDCSN",                                       /* 837 */
  "ACMNOTESTANDARDDIN",                                       /* 838 */
  "ACMNOTESTANDARDISO",                                       /* 839 */
  "ACMPARTLIST",                                              /* 840 */
  "ACMPICKOBJ",                                               /* 841 */
  "ACMSECTIONSTANDARDANSI",                                   /* 842 */
  "ACMSECTIONSTANDARDCSN2002",                                /* 843 */
  "ACMSECTIONSTANDARDCUSTOM",                                 /* 844 */
  "ACMSECTIONSTANDARDDIN",                                    /* 845 */
  "ACMSECTIONSTANDARDISO",                                    /* 846 */
  "ACMSECTIONSTANDARDISO2001",                                /* 847 */
  "ACMSTANDARDANSI",                                          /* 848 */
  "ACMSTANDARDCSN",                                           /* 849 */
  "ACMSTANDARDDIN",                                           /* 850 */
  "ACMSTANDARDISO",                                           /* 851 */
  "ACMSURFSTANDARDANSI",                                      /* 852 */
  "ACMSURFSTANDARDCSN",                                       /* 853 */
  "ACMSURFSTANDARDDIN",                                       /* 854 */
  "ACMSURFSTANDARDISO",                                       /* 855 */
  "ACMSURFSTANDARDISO2002",                                   /* 856 */
  "ACMSURFSYM",                                               /* 857 */
  "ACMTAPERSTANDARDANSI",                                     /* 858 */
  "ACMTAPERSTANDARDCSN",                                      /* 859 */
  "ACMTAPERSTANDARDDIN",                                      /* 860 */
  "ACMTAPERSTANDARDISO",                                      /* 861 */
  "ACMTHREADLINESTANDARDANSI",                                /* 862 */
  "ACMTHREADLINESTANDARDCSN",                                 /* 863 */
  "ACMTHREADLINESTANDARDDIN",                                 /* 864 */
  "ACMTHREADLINESTANDARDISO",                                 /* 865 */
  "ACMWELDSTANDARDANSI",                                      /* 866 */
  "ACMWELDSTANDARDCSN",                                       /* 867 */
  "ACMWELDSTANDARDDIN",                                       /* 868 */
  "ACMWELDSTANDARDISO",                                       /* 869 */
  "ACMWELDSYM",                                               /* 870 */
  "ACRFATTGENMGR",                                            /* 871 */
  "ACRFINSADJ",                                               /* 872 */
  "ACRFINSADJUSTERMGR",                                       /* 873 */
  "ACRFMCADAPIATTHOLDER",                                     /* 874 */
  "ACRFOBJATTMGR",                                            /* 875 */
  "ACSH_SUBENT_MATERIAL_CLASS",                               /* 876 */
  "AC_AM_2D_XREF_MGR",                                        /* 877 */
  "AC_AM_BASIC_VIEW",                                         /* 878 */
  "AC_AM_BASIC_VIEW_DEF",                                     /* 879 */
  "AC_AM_COMPLEX_HIDE_SITUATION",                             /* 880 */
  "AC_AM_COMP_VIEW_DEF",                                      /* 881 */
  "AC_AM_COMP_VIEW_INST",                                     /* 882 */
  "AC_AM_DIRTY_NODES",                                        /* 883 */
  "AC_AM_HIDE_SITUATION",                                     /* 884 */
  "AC_AM_MAPPER_CACHE",                                       /* 885 */
  "AC_AM_MASTER_VIEW_DEF",                                    /* 886 */
  "AC_AM_MVD_DEP_MGR",                                        /* 887 */
  "AC_AM_OVERRIDE_FILTER",                                    /* 888 */
  "AC_AM_PROPS_OVERRIDE",                                     /* 889 */
  "AC_AM_SHAFT_HIDE_SITUATION",                               /* 890 */
  "AC_AM_STDP_VIEW_DEF",                                      /* 891 */
  "AC_AM_TRANSFORM_GHOST",                                    /* 892 */
  "ADAPPL",                                                   /* 893 */
  "AECC_ALIGNMENT_DESIGN_CHECK_SET",                          /* 894 */
  "AECC_ALIGNMENT_LABEL_SET",                                 /* 895 */
  "AECC_ALIGNMENT_LABEL_SET_EXT",                             /* 896 */
  "AECC_ALIGNMENT_PARCEL_NODE",                               /* 897 */
  "AECC_ALIGNMENT_STYLE",                                     /* 898 */
  "AECC_APPURTENANCE_STYLE",                                  /* 899 */
  "AECC_ASSEMBLY_STYLE",                                      /* 900 */
  "AECC_BUILDING_SITE_STYLE",                                 /* 901 */
  "AECC_CANT_DIAGRAM_VIEW_STYLE",                             /* 902 */
  "AECC_CATCHMENT_STYLE",                                     /* 903 */
  "AECC_CLASS_NODE",                                          /* 904 */
  "AECC_CONTOURVIEW",                                         /* 905 */
  "AECC_CORRIDOR_STYLE",                                      /* 906 */
  "AECC_DISP_REP_ALIGNMENT",                                  /* 907 */
  "AECC_DISP_REP_ALIGNMENT_CANT_LABEL_GROUP",                 /* 908 */
  "AECC_DISP_REP_ALIGNMENT_CSV",                              /* 909 */
  "AECC_DISP_REP_ALIGNMENT_CURVE_LABEL",                      /* 910 */
  "AECC_DISP_REP_ALIGNMENT_DESIGNSPEED_LABEL_GROUP",          /* 911 */
  "AECC_DISP_REP_ALIGNMENT_GEOMPT_LABEL_GROUP",               /* 912 */
  "AECC_DISP_REP_ALIGNMENT_INDEXED_PI_LABEL",                 /* 913 */
  "AECC_DISP_REP_ALIGNMENT_MINOR_STATION_LABEL_GROUP",        /* 914 */
  "AECC_DISP_REP_ALIGNMENT_PI_LABEL",                         /* 915 */
  "AECC_DISP_REP_ALIGNMENT_SPIRAL_LABEL",                     /* 916 */
  "AECC_DISP_REP_ALIGNMENT_STAEQU_LABEL_GROUP",               /* 917 */
  "AECC_DISP_REP_ALIGNMENT_STATION_LABEL_GROUP",              /* 918 */
  "AECC_DISP_REP_ALIGNMENT_STATION_OFFSET_LABEL",             /* 919 */
  "AECC_DISP_REP_ALIGNMENT_SUPERELEVATION_LABEL_GROUP",       /* 920 */
  "AECC_DISP_REP_ALIGNMENT_TABLE",                            /* 921 */
  "AECC_DISP_REP_ALIGNMENT_TANGENT_LABEL",                    /* 922 */
  "AECC_DISP_REP_ALIGNMENT_VERTICAL_GEOMPT_LABELING",         /* 923 */
  "AECC_DISP_REP_APPURTENANCE",                               /* 924 */
  "AECC_DISP_REP_APPURTENANCE_CSV",                           /* 925 */
  "AECC_DISP_REP_APPURTENANCE_LABELING",                      /* 926 */
  "AECC_DISP_REP_APPURTENANCE_PROFILE_LABELING",              /* 927 */
  "AECC_DISP_REP_ASSEMBLY",                                   /* 928 */
  "AECC_DISP_REP_AUTO_CORRIDOR_FEATURE_LINE",                 /* 929 */
  "AECC_DISP_REP_AUTO_CORRIDOR_FEATURE_LINE_PROFILE",         /* 930 */
  "AECC_DISP_REP_AUTO_CORRIDOR_FEATURE_LINE_SECTION",         /* 931 */
  "AECC_DISP_REP_AUTO_FEATURE_LINE",                          /* 932 */
  "AECC_DISP_REP_AUTO_FEATURE_LINE_CSV",                      /* 933 */
  "AECC_DISP_REP_AUTO_FEATURE_LINE_PROFILE",                  /* 934 */
  "AECC_DISP_REP_AUTO_FEATURE_LINE_SECTION",                  /* 935 */
  "AECC_DISP_REP_BUILDINGSITE",                               /* 936 */
  "AECC_DISP_REP_BUILDINGUTIL_CONNECTOR",                     /* 937 */
  "AECC_DISP_REP_CANT_DIAGRAM_VIEW",                          /* 938 */
  "AECC_DISP_REP_CATCHMENT_AREA",                             /* 939 */
  "AECC_DISP_REP_CATCHMENT_AREA_LABEL",                       /* 940 */
  "AECC_DISP_REP_CORRIDOR",                                   /* 941 */
  "AECC_DISP_REP_CROSSING_PIPE_PROFILE_LABELING",             /* 942 */
  "AECC_DISP_REP_CROSSING_PRESSURE_PIPE_PROFILE_LABELING",    /* 943 */
  "AECC_DISP_REP_CSVSTATIONSLIDER",                           /* 944 */
  "AECC_DISP_REP_FACE",                                       /* 945 */
  "AECC_DISP_REP_FEATURE",                                    /* 946 */
  "AECC_DISP_REP_FEATURE_LABEL",                              /* 947 */
  "AECC_DISP_REP_FEATURE_LINE",                               /* 948 */
  "AECC_DISP_REP_FEATURE_LINE_CSV",                           /* 949 */
  "AECC_DISP_REP_FEATURE_LINE_PROFILE",                       /* 950 */
  "AECC_DISP_REP_FEATURE_LINE_SECTION",                       /* 951 */
  "AECC_DISP_REP_FITTING",                                    /* 952 */
  "AECC_DISP_REP_FITTING_CSV",                                /* 953 */
  "AECC_DISP_REP_FITTING_LABELING",                           /* 954 */
  "AECC_DISP_REP_FITTING_PROFILE_LABELING",                   /* 955 */
  "AECC_DISP_REP_FLOW_SEGMENT_LABEL",                         /* 956 */
  "AECC_DISP_REP_GENERAL_SEGMENT_LABEL",                      /* 957 */
  "AECC_DISP_REP_GRADING",                                    /* 958 */
  "AECC_DISP_REP_GRAPH",                                      /* 959 */
  "AECC_DISP_REP_GRAPHPROFILE_NETWORKPART",                   /* 960 */
  "AECC_DISP_REP_GRAPHPROFILE_PRESSUREPART",                  /* 961 */
  "AECC_DISP_REP_GRID_SURFACE",                               /* 962 */
  "AECC_DISP_REP_GRID_SURFACE_CSV",                           /* 963 */
  "AECC_DISP_REP_HORGEOMETRY_BAND_LABEL_GROUP",               /* 964 */
  "AECC_DISP_REP_HYDRO_REGION",                               /* 965 */
  "AECC_DISP_REP_INTERFERENCE_CHECK",                         /* 966 */
  "AECC_DISP_REP_INTERFERENCE_PART",                          /* 967 */
  "AECC_DISP_REP_INTERFERENCE_PART_SECTION",                  /* 968 */
  "AECC_DISP_REP_INTERSECTION",                               /* 969 */
  "AECC_DISP_REP_INTERSECTION_LOCATION_LABELING",             /* 970 */
  "AECC_DISP_REP_LABELING",                                   /* 971 */
  "AECC_DISP_REP_LEGEND_TABLE",                               /* 972 */
  "AECC_DISP_REP_LINE_BETWEEN_POINTS_LABEL",                  /* 973 */
  "AECC_DISP_REP_LOTLINE_CSV",                                /* 974 */
  "AECC_DISP_REP_MASSHAULLINE",                               /* 975 */
  "AECC_DISP_REP_MASS_HAUL_VIEW",                             /* 976 */
  "AECC_DISP_REP_MATCHLINE_LABELING",                         /* 977 */
  "AECC_DISP_REP_MATCH_LINE",                                 /* 978 */
  "AECC_DISP_REP_MATERIAL_SECTION",                           /* 979 */
  "AECC_DISP_REP_NETWORK",                                    /* 980 */
  "AECC_DISP_REP_NOTE_LABEL",                                 /* 981 */
  "AECC_DISP_REP_OFFSET_ELEV_LABEL",                          /* 982 */
  "AECC_DISP_REP_PARCEL_BOUNDARY",                            /* 983 */
  "AECC_DISP_REP_PARCEL_FACE_LABEL",                          /* 984 */
  "AECC_DISP_REP_PARCEL_SEGMENT",                             /* 985 */
  "AECC_DISP_REP_PARCEL_SEGMENT_LABEL",                       /* 986 */
  "AECC_DISP_REP_PARCEL_SEGMENT_TABLE",                       /* 987 */
  "AECC_DISP_REP_PARCEL_TABLE",                               /* 988 */
  "AECC_DISP_REP_PIPE",                                       /* 989 */
  "AECC_DISP_REP_PIPENETWORK_BAND_LABEL_GROUP",               /* 990 */
  "AECC_DISP_REP_PIPE_CSV",                                   /* 991 */
  "AECC_DISP_REP_PIPE_LABELING",                              /* 992 */
  "AECC_DISP_REP_PIPE_PROFILE_LABELING",                      /* 993 */
  "AECC_DISP_REP_PIPE_SECTION_LABELING",                      /* 994 */
  "AECC_DISP_REP_PIPE_TABLE",                                 /* 995 */
  "AECC_DISP_REP_POINT_ENT",                                  /* 996 */
  "AECC_DISP_REP_POINT_GROUP",                                /* 997 */
  "AECC_DISP_REP_POINT_TABLE",                                /* 998 */
  "AECC_DISP_REP_PRESSUREPIPENETWORK",                        /* 999 */
  "AECC_DISP_REP_PRESSURE_PART_TABLE",                        /* 1000 */
  "AECC_DISP_REP_PRESSURE_PIPE",                              /* 1001 */
  "AECC_DISP_REP_PRESSURE_PIPE_CSV",                          /* 1002 */
  "AECC_DISP_REP_PRESSURE_PIPE_LABELING",                     /* 1003 */
  "AECC_DISP_REP_PRESSURE_PIPE_PROFILE_LABELING",             /* 1004 */
  "AECC_DISP_REP_PRESSURE_PIPE_SECTION_LABELING",             /* 1005 */
  "AECC_DISP_REP_PROFILE",                                    /* 1006 */
  "AECC_DISP_REP_PROFILEDATA_BAND_LABEL_GROUP",               /* 1007 */
  "AECC_DISP_REP_PROFILE_PROJECTION",                         /* 1008 */
  "AECC_DISP_REP_PROFILE_PROJECTION_LABEL",                   /* 1009 */
  "AECC_DISP_REP_PROFILE_VIEW",                               /* 1010 */
  "AECC_DISP_REP_PROFILE_VIEW_DEPTH_LABEL",                   /* 1011 */
  "AECC_DISP_REP_QUANTITY_TAKEOFF_AGGREGATE_EARTHWORK_TABLE", /* 1012 */
  "AECC_DISP_REP_RIGHT_OF_WAY",                               /* 1013 */
  "AECC_DISP_REP_SAMPLELINE_LABELING",                        /* 1014 */
  "AECC_DISP_REP_SAMPLE_LINE",                                /* 1015 */
  "AECC_DISP_REP_SAMPLE_LINE_GROUP",                          /* 1016 */
  "AECC_DISP_REP_SECTION",                                    /* 1017 */
  "AECC_DISP_REP_SECTIONALDATA_BAND_LABEL_GROUP",             /* 1018 */
  "AECC_DISP_REP_SECTIONDATA_BAND_LABEL_GROUP",               /* 1019 */
  "AECC_DISP_REP_SECTIONSEGMENT_BAND_LABEL_GROUP",            /* 1020 */
  "AECC_DISP_REP_SECTION_CORRIDOR",                           /* 1021 */
  "AECC_DISP_REP_SECTION_CORRIDOR_POINT_LABEL_GROUP",         /* 1022 */
  "AECC_DISP_REP_SECTION_GRADEBREAK_LABEL_GROUP",             /* 1023 */
  "AECC_DISP_REP_SECTION_MINOR_OFFSET_LABEL_GROUP",           /* 1024 */
  "AECC_DISP_REP_SECTION_OFFSET_LABEL_GROUP",                 /* 1025 */
  "AECC_DISP_REP_SECTION_PIPENETWORK",                        /* 1026 */
  "AECC_DISP_REP_SECTION_PRESSUREPIPENETWORK",                /* 1027 */
  "AECC_DISP_REP_SECTION_PROJECTION",                         /* 1028 */
  "AECC_DISP_REP_SECTION_PROJECTION_LABEL",                   /* 1029 */
  "AECC_DISP_REP_SECTION_SEGMENT_LABEL_GROUP",                /* 1030 */
  "AECC_DISP_REP_SECTION_VIEW",                               /* 1031 */
  "AECC_DISP_REP_SECTION_VIEW_DEPTH_LABEL",                   /* 1032 */
  "AECC_DISP_REP_SECTION_VIEW_QUANTITY_TAKEOFF_TABLE",        /* 1033 */
  "AECC_DISP_REP_SHEET",                                      /* 1034 */
  "AECC_DISP_REP_SPANNING_PIPE_LABELING",                     /* 1035 */
  "AECC_DISP_REP_SPANNING_PIPE_PROFILE_LABELING",             /* 1036 */
  "AECC_DISP_REP_STATION_ELEV_LABEL",                         /* 1037 */
  "AECC_DISP_REP_STRUCTURE",                                  /* 1038 */
  "AECC_DISP_REP_STRUCTURE_CSV",                              /* 1039 */
  "AECC_DISP_REP_STRUCTURE_LABELING",                         /* 1040 */
  "AECC_DISP_REP_STRUCTURE_PROFILE_LABELING",                 /* 1041 */
  "AECC_DISP_REP_STRUCTURE_SECTION_LABELING",                 /* 1042 */
  "AECC_DISP_REP_SUBASSEMBLY",                                /* 1043 */
  "AECC_DISP_REP_SUPERELEVATION_BAND_LABEL_GROUP",            /* 1044 */
  "AECC_DISP_REP_SUPERELEVATION_DIAGRAM_VIEW",                /* 1045 */
  "AECC_DISP_REP_SURFACE_CONTOUR_LABEL_GROUP",                /* 1046 */
  "AECC_DISP_REP_SURFACE_ELEVATION_LABEL",                    /* 1047 */
  "AECC_DISP_REP_SURFACE_SLOPE_LABEL",                        /* 1048 */
  "AECC_DISP_REP_SURVEY_FIGURE_LABEL_GROUP",                  /* 1049 */
  "AECC_DISP_REP_SVFIGURE",                                   /* 1050 */
  "AECC_DISP_REP_SVFIGURE_CSV",                               /* 1051 */
  "AECC_DISP_REP_SVFIGURE_PROFILE",                           /* 1052 */
  "AECC_DISP_REP_SVFIGURE_SECTION",                           /* 1053 */
  "AECC_DISP_REP_SVFIGURE_SEGMENT_LABEL",                     /* 1054 */
  "AECC_DISP_REP_SVNETWORK",                                  /* 1055 */
  "AECC_DISP_REP_TANGENT_INTERSECTION_TABLE",                 /* 1056 */
  "AECC_DISP_REP_TIN_SURFACE",                                /* 1057 */
  "AECC_DISP_REP_TIN_SURFACE_CSV",                            /* 1058 */
  "AECC_DISP_REP_VALIGNMENT_CRESTCURVE_LABEL_GROUP",          /* 1059 */
  "AECC_DISP_REP_VALIGNMENT_CSV",                             /* 1060 */
  "AECC_DISP_REP_VALIGNMENT_HAGEOMPT_LABEL_GROUP",            /* 1061 */
  "AECC_DISP_REP_VALIGNMENT_LINE_LABEL_GROUP",                /* 1062 */
  "AECC_DISP_REP_VALIGNMENT_MINOR_STATION_LABEL_GROUP",       /* 1063 */
  "AECC_DISP_REP_VALIGNMENT_PVI_LABEL_GROUP",                 /* 1064 */
  "AECC_DISP_REP_VALIGNMENT_SAGCURVE_LABEL_GROUP",            /* 1065 */
  "AECC_DISP_REP_VALIGNMENT_STATION_LABEL_GROUP",             /* 1066 */
  "AECC_DISP_REP_VERTICALGEOMETRY_BAND_LABEL_GROUP",          /* 1067 */
  "AECC_DISP_REP_VIEWFRAME_LABELING",                         /* 1068 */
  "AECC_DISP_REP_VIEW_FRAME",                                 /* 1069 */
  "AECC_FEATURELINE_STYLE",                                   /* 1070 */
  "AECC_FEATURE_STYLE",                                       /* 1071 */
  "AECC_FITTING_STYLE",                                       /* 1072 */
  "AECC_FORMAT_MANAGER_OBJECT",                               /* 1073 */
  "AECC_GRADEVIEW",                                           /* 1074 */
  "AECC_GRADING_CRITERIA",                                    /* 1075 */
  "AECC_GRADING_CRITERIA_SET",                                /* 1076 */
  "AECC_GRADING_GROUP",                                       /* 1077 */
  "AECC_GRADING_STYLE",                                       /* 1078 */
  "AECC_IMPORT_STORM_SEWER_DEFAULTS",                         /* 1079 */
  "AECC_INTERFERENCE_STYLE",                                  /* 1080 */
  "AECC_INTERSECTION_STYLE",                                  /* 1081 */
  "AECC_LABEL_COLLECTOR_STYLE",                               /* 1082 */
  "AECC_LABEL_NODE",                                          /* 1083 */
  "AECC_LABEL_RADIAL_LINE_STYLE",                             /* 1084 */
  "AECC_LABEL_TEXT_ITERATOR_CURVE_OR_SPIRAL_STYLE",           /* 1085 */
  "AECC_LABEL_TEXT_ITERATOR_STYLE",                           /* 1086 */
  "AECC_LABEL_TEXT_STYLE",                                    /* 1087 */
  "AECC_LABEL_VECTOR_ARROW_STYLE",                            /* 1088 */
  "AECC_LEGEND_TABLE_STYLE",                                  /* 1089 */
  "AECC_MASS_HAUL_LINE_STYLE",                                /* 1090 */
  "AECC_MASS_HAUL_VIEW_STYLE",                                /* 1091 */
  "AECC_MATCHLINE_STYLE",                                     /* 1092 */
  "AECC_MATERIAL_STYLE",                                      /* 1093 */
  "AECC_NETWORK_PART_CATALOG_DEF_NODE",                       /* 1094 */
  "AECC_NETWORK_PART_FAMILY_ITEM",                            /* 1095 */
  "AECC_NETWORK_PART_LIST",                                   /* 1096 */
  "AECC_NETWORK_RULE",                                        /* 1097 */
  "AECC_PARCEL_NODE",                                         /* 1098 */
  "AECC_PARCEL_STYLE",                                        /* 1099 */
  "AECC_PART_SIZE_FILTER",                                    /* 1100 */
  "AECC_PIPE_RULES",                                          /* 1101 */
  "AECC_PIPE_STYLE",                                          /* 1102 */
  "AECC_PIPE_STYLE_EXTENSION",                                /* 1103 */
  "AECC_POINTCLOUD_STYLE",                                    /* 1104 */
  "AECC_POINTVIEW",                                           /* 1105 */
  "AECC_POINT_STYLE",                                         /* 1106 */
  "AECC_PRESSURE_PART_LIST",                                  /* 1107 */
  "AECC_PRESSURE_PIPE_STYLE",                                 /* 1108 */
  "AECC_PROFILESECTIONENTITY_STYLE",                          /* 1109 */
  "AECC_PROFILE_DESIGN_CHECK_SET",                            /* 1110 */
  "AECC_PROFILE_LABEL_SET",                                   /* 1111 */
  "AECC_PROFILE_STYLE",                                       /* 1112 */
  "AECC_PROFILE_VIEW_BAND_STYLE_SET",                         /* 1113 */
  "AECC_PROFILE_VIEW_DATA_BAND_STYLE",                        /* 1114 */
  "AECC_PROFILE_VIEW_HORIZONTAL_GEOMETRY_BAND_STYLE",         /* 1115 */
  "AECC_PROFILE_VIEW_PIPE_NETWORK_BAND_STYLE",                /* 1116 */
  "AECC_PROFILE_VIEW_SECTIONAL_DATA_BAND_STYLE",              /* 1117 */
  "AECC_PROFILE_VIEW_STYLE",                                  /* 1118 */
  "AECC_PROFILE_VIEW_SUPERELEVATION_DIAGRAM_BAND_STYLE",      /* 1119 */
  "AECC_PROFILE_VIEW_VERTICAL_GEOMETRY_BAND_STYLE",           /* 1120 */
  "AECC_QUANTITY_TAKEOFF_CRITERIA",                           /* 1121 */
  "AECC_ROADWAYLINK_STYLE",                                   /* 1122 */
  "AECC_ROADWAYMARKER_STYLE",                                 /* 1123 */
  "AECC_ROADWAYSHAPE_STYLE",                                  /* 1124 */
  "AECC_ROADWAY_STYLE_SET",                                   /* 1125 */
  "AECC_ROOT_SETTINGS_NODE",                                  /* 1126 */
  "AECC_SAMPLE_LINE_GROUP_STYLE",                             /* 1127 */
  "AECC_SAMPLE_LINE_STYLE",                                   /* 1128 */
  "AECC_SECTION_LABEL_SET",                                   /* 1129 */
  "AECC_SECTION_STYLE",                                       /* 1130 */
  "AECC_SECTION_VIEW_BAND_STYLE_SET",                         /* 1131 */
  "AECC_SECTION_VIEW_DATA_BAND_STYLE",                        /* 1132 */
  "AECC_SECTION_VIEW_ROAD_SURFACE_BAND_STYLE",                /* 1133 */
  "AECC_SECTION_VIEW_STYLE",                                  /* 1134 */
  "AECC_SETTINGS_NODE",                                       /* 1135 */
  "AECC_SHEET_STYLE",                                         /* 1136 */
  "AECC_SLOPE_PATTERN_STYLE",                                 /* 1137 */
  "AECC_STATION_FORMAT_STYLE",                                /* 1138 */
  "AECC_STRUCTURE_RULES",                                     /* 1139 */
  "AECC_STUCTURE_STYLE",                                      /* 1140 */
  "AECC_SUPERELEVATION_DIAGRAM_VIEW_STYLE",                   /* 1141 */
  "AECC_SURFACE_STYLE",                                       /* 1142 */
  "AECC_SVFIGURE_STYLE",                                      /* 1143 */
  "AECC_SVNETWORK_STYLE",                                     /* 1144 */
  "AECC_TABLE_STYLE",                                         /* 1145 */
  "AECC_TAG_MANAGER",                                         /* 1146 */
  "AECC_TREE_NODE",                                           /* 1147 */
  "AECC_USER_DEFINED_ATTRIBUTE_CLASSIFICATION",               /* 1148 */
  "AECC_VALIGNMENT_STYLE_EXTENSION",                          /* 1149 */
  "AECC_VIEW_FRAME_STYLE",                                    /* 1150 */
  "AECS_DISP_PROPS_MEMBER",                                   /* 1151 */
  "AECS_DISP_PROPS_MEMBER_LOGICAL",                           /* 1152 */
  "AECS_DISP_PROPS_MEMBER_PLAN",                              /* 1153 */
  "AECS_DISP_PROPS_MEMBER_PLAN_SKETCH",                       /* 1154 */
  "AECS_DISP_PROPS_MEMBER_PROJECTED",                         /* 1155 */
  "AECS_DISP_REP_MEMBER_ELEVATION_DESIGN",                    /* 1156 */
  "AECS_DISP_REP_MEMBER_ELEVATION_DETAIL",                    /* 1157 */
  "AECS_DISP_REP_MEMBER_LOGICAL",                             /* 1158 */
  "AECS_DISP_REP_MEMBER_MODEL_DESIGN",                        /* 1159 */
  "AECS_DISP_REP_MEMBER_MODEL_DETAIL",                        /* 1160 */
  "AECS_DISP_REP_MEMBER_PLAN_DESIGN",                         /* 1161 */
  "AECS_DISP_REP_MEMBER_PLAN_DETAIL",                         /* 1162 */
  "AECS_DISP_REP_MEMBER_PLAN_SKETCH",                         /* 1163 */
  "AECS_MEMBER_NODE_SHAPE",                                   /* 1164 */
  "AECS_MEMBER_STYLE",                                        /* 1165 */
  "AEC_2DSECTION_STYLE",                                      /* 1166 */
  "AEC_AECDBDISPREPBDGELEVLINEPLAN100",                       /* 1167 */
  "AEC_AECDBDISPREPBDGELEVLINEPLAN50",                        /* 1168 */
  "AEC_AECDBDISPREPBDGSECTIONLINEPLAN100",                    /* 1169 */
  "AEC_AECDBDISPREPBDGSECTIONLINEPLAN50",                     /* 1170 */
  "AEC_AECDBDISPREPCEILINGGRIDPLAN100",                       /* 1171 */
  "AEC_AECDBDISPREPCEILINGGRIDPLAN50",                        /* 1172 */
  "AEC_AECDBDISPREPCOLUMNGRIDPLAN100",                        /* 1173 */
  "AEC_AECDBDISPREPCOLUMNGRIDPLAN50",                         /* 1174 */
  "AEC_AECDBDISPREPCURTAINWALLLAYOUTPLAN100",                 /* 1175 */
  "AEC_AECDBDISPREPCURTAINWALLLAYOUTPLAN50",                  /* 1176 */
  "AEC_AECDBDISPREPCURTAINWALLUNITPLAN100",                   /* 1177 */
  "AEC_AECDBDISPREPCURTAINWALLUNITPLAN50",                    /* 1178 */
  "AEC_AECDBDISPREPMVBLOCKREFPLAN100",                        /* 1179 */
  "AEC_AECDBDISPREPMVBLOCKREFPLAN50",                         /* 1180 */
  "AEC_AECDBDISPREPROOFPLAN100",                              /* 1181 */
  "AEC_AECDBDISPREPROOFPLAN50",                               /* 1182 */
  "AEC_AECDBDISPREPROOFSLABPLAN100",                          /* 1183 */
  "AEC_AECDBDISPREPROOFSLABPLAN50",                           /* 1184 */
  "AEC_AECDBDISPREPSLABPLAN100",                              /* 1185 */
  "AEC_AECDBDISPREPSLABPLAN50",                               /* 1186 */
  "AEC_AECDBDISPREPSPACEPLAN100",                             /* 1187 */
  "AEC_AECDBDISPREPSPACEPLAN50",                              /* 1188 */
  "AEC_AECDBDISPREPWALLPLAN100",                              /* 1189 */
  "AEC_AECDBDISPREPWALLPLAN50",                               /* 1190 */
  "AEC_AECDBDISPREPWINDOWASSEMBLYPLAN100",                    /* 1191 */
  "AEC_AECDBDISPREPWINDOWASSEMBLYPLAN50",                     /* 1192 */
  "AEC_AECDBDISPREPZONE100",                                  /* 1193 */
  "AEC_AECDBDISPREPZONE50",                                   /* 1194 */
  "AEC_AECDBZONEDEF",                                         /* 1195 */
  "AEC_AECDBZONESTYLE",                                       /* 1196 */
  "AEC_ANCHOR_OPENINGBASE_TO_WALL",                           /* 1197 */
  "AEC_CLASSIFICATION_DEF",                                   /* 1198 */
  "AEC_CLASSIFICATION_SYSTEM_DEF",                            /* 1199 */
  "AEC_CLEANUP_GROUP_DEF",                                    /* 1200 */
  "AEC_CURTAIN_WALL_LAYOUT_STYLE",                            /* 1201 */
  "AEC_CURTAIN_WALL_UNIT_STYLE",                              /* 1202 */
  "AEC_CVSECTIONVIEW",                                        /* 1203 */
  "AEC_DB_DISP_REP_DIM_GROUP_PLAN",                           /* 1204 */
  "AEC_DB_DISP_REP_DIM_GROUP_PLAN100",                        /* 1205 */
  "AEC_DB_DISP_REP_DIM_GROUP_PLAN50",                         /* 1206 */
  "AEC_DIM_STYLE",                                            /* 1207 */
  "AEC_DISPLAYTHEME_STYLE",                                   /* 1208 */
  "AEC_DISP_PROPS_2D_SECTION",                                /* 1209 */
  "AEC_DISP_PROPS_CLIP_VOLUME",                               /* 1210 */
  "AEC_DISP_PROPS_CLIP_VOLUME_RESULT",                        /* 1211 */
  "AEC_DISP_PROPS_DIM",                                       /* 1212 */
  "AEC_DISP_PROPS_DISPLAYTHEME",                              /* 1213 */
  "AEC_DISP_PROPS_DOOR",                                      /* 1214 */
  "AEC_DISP_PROPS_DOOR_NOMINAL",                              /* 1215 */
  "AEC_DISP_PROPS_DOOR_PLAN_100",                             /* 1216 */
  "AEC_DISP_PROPS_DOOR_PLAN_50",                              /* 1217 */
  "AEC_DISP_PROPS_DOOR_THRESHOLD_PLAN",                       /* 1218 */
  "AEC_DISP_PROPS_DOOR_THRESHOLD_SYMBOL_PLAN",                /* 1219 */
  "AEC_DISP_PROPS_EDITINPLACEPROFILE_MODEL",                  /* 1220 */
  "AEC_DISP_PROPS_ENT",                                       /* 1221 */
  "AEC_DISP_PROPS_ENT_REF",                                   /* 1222 */
  "AEC_DISP_PROPS_GRID_ASSEMBLY_MODEL",                       /* 1223 */
  "AEC_DISP_PROPS_GRID_ASSEMBLY_PLAN",                        /* 1224 */
  "AEC_DISP_PROPS_LAYOUT_CURVE",                              /* 1225 */
  "AEC_DISP_PROPS_LAYOUT_GRID2D",                             /* 1226 */
  "AEC_DISP_PROPS_LAYOUT_GRID3D",                             /* 1227 */
  "AEC_DISP_PROPS_MASKBLOCK",                                 /* 1228 */
  "AEC_DISP_PROPS_MASS_ELEM_MODEL",                           /* 1229 */
  "AEC_DISP_PROPS_MASS_GROUP",                                /* 1230 */
  "AEC_DISP_PROPS_MATERIAL",                                  /* 1231 */
  "AEC_DISP_PROPS_OPENING",                                   /* 1232 */
  "AEC_DISP_PROPS_POLYGON_MODEL",                             /* 1233 */
  "AEC_DISP_PROPS_POLYGON_TRUECOLOUR",                        /* 1234 */
  "AEC_DISP_PROPS_RAILING_MODEL",                             /* 1235 */
  "AEC_DISP_PROPS_RAILING_PLAN",                              /* 1236 */
  "AEC_DISP_PROPS_ROOF",                                      /* 1237 */
  "AEC_DISP_PROPS_ROOFSLAB",                                  /* 1238 */
  "AEC_DISP_PROPS_ROOFSLAB_PLAN",                             /* 1239 */
  "AEC_DISP_PROPS_SCHEDULE_TABLE",                            /* 1240 */
  "AEC_DISP_PROPS_SLAB",                                      /* 1241 */
  "AEC_DISP_PROPS_SLAB_PLAN",                                 /* 1242 */
  "AEC_DISP_PROPS_SLICE",                                     /* 1243 */
  "AEC_DISP_PROPS_SPACE_DECOMPOSED",                          /* 1244 */
  "AEC_DISP_PROPS_SPACE_MODEL",                               /* 1245 */
  "AEC_DISP_PROPS_SPACE_PLAN",                                /* 1246 */
  "AEC_DISP_PROPS_STAIR_MODEL",                               /* 1247 */
  "AEC_DISP_PROPS_STAIR_PLAN",                                /* 1248 */
  "AEC_DISP_PROPS_STAIR_PLAN_OVERLAPPING",                    /* 1249 */
  "AEC_DISP_PROPS_WALL_GRAPH",                                /* 1250 */
  "AEC_DISP_PROPS_WALL_MODEL",                                /* 1251 */
  "AEC_DISP_PROPS_WALL_PLAN",                                 /* 1252 */
  "AEC_DISP_PROPS_WALL_SCHEM",                                /* 1253 */
  "AEC_DISP_PROPS_WINDOW",                                    /* 1254 */
  "AEC_DISP_PROPS_WINDOW_ASSEMBLY_SILL_PLAN",                 /* 1255 */
  "AEC_DISP_PROPS_WINDOW_NOMINAL",                            /* 1256 */
  "AEC_DISP_PROPS_WINDOW_PLAN_100",                           /* 1257 */
  "AEC_DISP_PROPS_WINDOW_PLAN_50",                            /* 1258 */
  "AEC_DISP_PROPS_WINDOW_SILL_PLAN",                          /* 1259 */
  "AEC_DISP_PROPS_ZONE",                                      /* 1260 */
  "AEC_DISP_REP_2D_SECTION",                                  /* 1261 */
  "AEC_DISPREPAECDBDISPREPMASSELEMPLAN100",                   /* 1262 */
  "AEC_DISPREPAECDBDISPREPMASSELEMPLAN50",                    /* 1263 */
  "AEC_DISPREPAECDBDISPREPMASSGROUPPLAN100",                  /* 1264 */
  "AEC_DISPREPAECDBDISPREPMASSGROUPPLAN50",                   /* 1265 */
  "AEC_DISPREPAECDBDISPREPOPENINGPLAN100",                    /* 1266 */
  "AEC_DISPREPAECDBDISPREPOPENINGPLAN50",                     /* 1267 */
  "AEC_DISPREPAECDBDISPREPOPENINGPLANREFLECTED",              /* 1268 */
  "AEC_DISPREPAECDBDISPREPOPENINGSILLPLAN",                   /* 1269 */
  "AEC_DISP_REP_ANCHOR",                                      /* 1270 */
  "AEC_DISP_REP_ANCHOR_BUB_TO_GRID",                          /* 1271 */
  "AEC_DISP_REP_ANCHOR_BUB_TO_GRID_MODEL",                    /* 1272 */
  "AEC_DISP_REP_ANCHOR_BUB_TO_GRID_RCP",                      /* 1273 */
  "AEC_DISP_REP_ANCHOR_ENT_TO_NODE",                          /* 1274 */
  "AEC_DISP_REP_ANCHOR_EXT_TAG_TO_ENT",                       /* 1275 */
  "AEC_DISP_REP_ANCHOR_TAG_TO_ENT",                           /* 1276 */
  "AEC_DISP_REP_BDG_ELEVLINE_MODEL",                          /* 1277 */
  "AEC_DISP_REP_BDG_ELEVLINE_PLAN",                           /* 1278 */
  "AEC_DISP_REP_BDG_ELEVLINE_RCP",                            /* 1279 */
  "AEC_DISP_REP_BDG_SECTIONLINE_MODEL",                       /* 1280 */
  "AEC_DISP_REP_BDG_SECTIONLINE_PLAN",                        /* 1281 */
  "AEC_DISP_REP_BDG_SECTIONLINE_RCP",                         /* 1282 */
  "AEC_DISP_REP_BDG_SECTION_MODEL",                           /* 1283 */
  "AEC_DISP_REP_BDG_SECTION_SUBDIV",                          /* 1284 */
  "AEC_DISP_REP_CEILING_GRID",                                /* 1285 */
  "AEC_DISP_REP_CEILING_GRID_MODEL",                          /* 1286 */
  "AEC_DISP_REP_CEILING_GRID_RCP",                            /* 1287 */
  "AEC_DISP_REP_CLIP_VOLUME_MODEL",                           /* 1288 */
  "AEC_DISP_REP_CLIP_VOLUME_PLAN",                            /* 1289 */
  "AEC_DISP_REP_CLIP_VOLUME_RESULT",                          /* 1290 */
  "AEC_DISP_REP_CLIP_VOLUME_RESULT_SUBDIV",                   /* 1291 */
  "AEC_DISP_REP_COL_BLOCK",                                   /* 1292 */
  "AEC_DISP_REP_COL_CIRCARC2D",                               /* 1293 */
  "AEC_DISP_REP_COL_CONCOINCIDENT",                           /* 1294 */
  "AEC_DISP_REP_COL_CONCONCENTRIC",                           /* 1295 */
  "AEC_DISP_REP_COL_CONEQUALDISTANCE",                        /* 1296 */
  "AEC_DISP_REP_COL_CONMIDPOINT",                             /* 1297 */
  "AEC_DISP_REP_COL_CONNECTOR",                               /* 1298 */
  "AEC_DISP_REP_COL_CONNORMAL",                               /* 1299 */
  "AEC_DISP_REP_COL_CONPARALLEL",                             /* 1300 */
  "AEC_DISP_REP_COL_CONPERPENDICULAR",                        /* 1301 */
  "AEC_DISP_REP_COL_CONSYMMETRIC",                            /* 1302 */
  "AEC_DISP_REP_COL_CONTANGENT",                              /* 1303 */
  "AEC_DISP_REP_COL_DIMANGLE",                                /* 1304 */
  "AEC_DISP_REP_COL_DIMDIAMETER",                             /* 1305 */
  "AEC_DISP_REP_COL_DIMDISTANCE",                             /* 1306 */
  "AEC_DISP_REP_COL_DIMLENGTH",                               /* 1307 */
  "AEC_DISP_REP_COL_DIMMAJORRADIUS",                          /* 1308 */
  "AEC_DISP_REP_COL_DIMMINORRADIUS",                          /* 1309 */
  "AEC_DISP_REP_COL_ELLIPARC2D",                              /* 1310 */
  "AEC_DISP_REP_COL_LAYOUTDATA",                              /* 1311 */
  "AEC_DISP_REP_COL_LINE2D",                                  /* 1312 */
  "AEC_DISP_REP_COL_MODIFIER_ADD",                            /* 1313 */
  "AEC_DISP_REP_COL_MODIFIER_CUTPLANE",                       /* 1314 */
  "AEC_DISP_REP_COL_MODIFIER_EXTRUSION",                      /* 1315 */
  "AEC_DISP_REP_COL_MODIFIER_GROUP",                          /* 1316 */
  "AEC_DISP_REP_COL_MODIFIER_LOFT",                           /* 1317 */
  "AEC_DISP_REP_COL_MODIFIER_PATH",                           /* 1318 */
  "AEC_DISP_REP_COL_MODIFIER_REVOLVE",                        /* 1319 */
  "AEC_DISP_REP_COL_MODIFIER_SUBTRACT",                       /* 1320 */
  "AEC_DISP_REP_COL_MODIFIER_TRANSITION",                     /* 1321 */
  "AEC_DISP_REP_COL_POINT2D",                                 /* 1322 */
  "AEC_DISP_REP_COL_PROFILE",                                 /* 1323 */
  "AEC_DISP_REP_COLUMN_GRID",                                 /* 1324 */
  "AEC_DISP_REP_COLUMN_GRID_MODEL",                           /* 1325 */
  "AEC_DISP_REP_COLUMN_GRID_RCP",                             /* 1326 */
  "AEC_DISP_REP_COL_WORKPLANE",                               /* 1327 */
  "AEC_DISP_REP_COL_WORKPLANE_REF",                           /* 1328 */
  "AEC_DISP_REP_CONFIG",                                      /* 1329 */
  "AEC_DISP_REP_CURTAIN_WALL_LAYOUT_MODEL",                   /* 1330 */
  "AEC_DISP_REP_CURTAIN_WALL_LAYOUT_PLAN",                    /* 1331 */
  "AEC_DISP_REP_CURTAIN_WALL_UNIT_MODEL",                     /* 1332 */
  "AEC_DISP_REP_CURTAIN_WALL_UNIT_PLAN",                      /* 1333 */
  "AEC_DISP_REP_DCM_DIMRADIUS",                               /* 1334 */
  "AEC_DISP_REP_DISPLAYTHEME",                                /* 1335 */
  "AEC_DISP_REP_DOOR_ELEV",                                   /* 1336 */
  "AEC_DISP_REP_DOOR_MODEL",                                  /* 1337 */
  "AEC_DISP_REP_DOOR_NOMINAL",                                /* 1338 */
  "AEC_DISP_REP_DOOR_PLAN",                                   /* 1339 */
  "AEC_DISP_REP_DOOR_PLAN_50",                                /* 1340 */
  "AEC_DISP_REP_DOOR_PLAN_HEKTO",                             /* 1341 */
  "AEC_DISP_REP_DOOR_RCP",                                    /* 1342 */
  "AEC_DISP_REP_DOOR_THRESHOLD_PLAN",                         /* 1343 */
  "AEC_DISP_REP_DOOR_THRESHOLD_SYMBOL_PLAN",                  /* 1344 */
  "AEC_DISP_REP_EDITINPLACEPROFILE",                          /* 1345 */
  "AEC_DISP_REP_ENT_REF",                                     /* 1346 */
  "AEC_DISP_REP_LAYOUT_CURVE",                                /* 1347 */
  "AEC_DISP_REP_LAYOUT_GRID2D",                               /* 1348 */
  "AEC_DISP_REP_LAYOUT_GRID3D",                               /* 1349 */
  "AEC_DISP_REP_MASKBLOCK_REF",                               /* 1350 */
  "AEC_DISP_REP_MASKBLOCK_REF_RCP",                           /* 1351 */
  "AEC_DISP_REP_MASS_ELEM_MODEL",                             /* 1352 */
  "AEC_DISP_REP_MASS_ELEM_RCP",                               /* 1353 */
  "AEC_DISP_REP_MASS_ELEM_SCHEM",                             /* 1354 */
  "AEC_DISP_REP_MASS_GROUP_MODEL",                            /* 1355 */
  "AEC_DISP_REP_MASS_GROUP_PLAN",                             /* 1356 */
  "AEC_DISP_REP_MASS_GROUP_RCP",                              /* 1357 */
  "AEC_DISP_REP_MATERIAL",                                    /* 1358 */
  "AEC_DISP_REP_MVBLOCK_REF",                                 /* 1359 */
  "AEC_DISP_REP_MVBLOCK_REF_MODEL",                           /* 1360 */
  "AEC_DISP_REP_MVBLOCK_REF_RCP",                             /* 1361 */
  "AEC_DISP_REP_OPENING",                                     /* 1362 */
  "AEC_DISP_REP_OPENING_MODEL",                               /* 1363 */
  "AEC_DISP_REP_POLYGON_MODEL",                               /* 1364 */
  "AEC_DISP_REP_POLYGON_TRUECOLOUR",                          /* 1365 */
  "AEC_DISP_REP_RAILING_MODEL",                               /* 1366 */
  "AEC_DISP_REP_RAILING_PLAN",                                /* 1367 */
  "AEC_DISP_REP_RAILING_PLAN_100",                            /* 1368 */
  "AEC_DISP_REP_RAILING_PLAN_50",                             /* 1369 */
  "AEC_DISP_REP_ROOF_MODEL",                                  /* 1370 */
  "AEC_DISP_REP_ROOF_PLAN",                                   /* 1371 */
  "AEC_DISP_REP_ROOF_RCP",                                    /* 1372 */
  "AEC_DISP_REP_ROOFSLAB_MODEL",                              /* 1373 */
  "AEC_DISP_REP_ROOFSLAB_PLAN",                               /* 1374 */
  "AEC_DISP_REP_SCHEDULE_TABLE",                              /* 1375 */
  "AEC_DISP_REP_SET",                                         /* 1376 */
  "AEC_DISP_REP_SLAB_MODEL",                                  /* 1377 */
  "AEC_DISP_REP_SLAB_PLAN",                                   /* 1378 */
  "AEC_DISP_REP_SLICE",                                       /* 1379 */
  "AEC_DISP_REP_SPACE_DECOMPOSED",                            /* 1380 */
  "AEC_DISP_REP_SPACE_MODEL",                                 /* 1381 */
  "AEC_DISP_REP_SPACE_PLAN",                                  /* 1382 */
  "AEC_DISP_REP_SPACE_RCP",                                   /* 1383 */
  "AEC_DISP_REP_SPACE_VOLUME",                                /* 1384 */
  "AEC_DISP_REP_STAIR_MODEL",                                 /* 1385 */
  "AEC_DISP_REP_STAIR_PLAN",                                  /* 1386 */
  "AEC_DISP_REP_STAIR_PLAN_100",                              /* 1387 */
  "AEC_DISP_REP_STAIR_PLAN_50",                               /* 1388 */
  "AEC_DISP_REP_STAIR_PLAN_OVERLAPPING",                      /* 1389 */
  "AEC_DISP_REP_STAIR_RCP",                                   /* 1390 */
  "AEC_DISP_REP_WALL_GRAPH",                                  /* 1391 */
  "AEC_DISP_REP_WALL_MODEL",                                  /* 1392 */
  "AEC_DISP_REP_WALL_PLAN",                                   /* 1393 */
  "AEC_DISP_REP_WALL_RCP",                                    /* 1394 */
  "AEC_DISP_REP_WALL_SCHEM",                                  /* 1395 */
  "AEC_DISP_REP_WINDOW_ASSEMBLY_MODEL",                       /* 1396 */
  "AEC_DISP_REP_WINDOW_ASSEMBLY_PLAN",                        /* 1397 */
  "AEC_DISP_REP_WINDOWASSEMBLY_SILL_PLAN",                    /* 1398 */
  "AEC_DISP_REP_WINDOW_ELEV",                                 /* 1399 */
  "AEC_DISP_REP_WINDOW_MODEL",                                /* 1400 */
  "AEC_DISP_REP_WINDOW_NOMINAL",                              /* 1401 */
  "AEC_DISP_REP_WINDOW_PLAN",                                 /* 1402 */
  "AEC_DISP_REP_WINDOW_PLAN_100",                             /* 1403 */
  "AEC_DISP_REP_WINDOW_PLAN_50",                              /* 1404 */
  "AEC_DISP_REP_WINDOW_RCP",                                  /* 1405 */
  "AEC_DISP_REP_WINDOW_SILL_PLAN",                            /* 1406 */
  "AEC_DISP_REP_ZONE",                                        /* 1407 */
  "AEC_DISPROPSMASSELEMPLANCOMMON",                           /* 1408 */
  "AEC_DISPROPSMASSGROUPPLANCOMMON",                          /* 1409 */
  "AEC_DISPROPSOPENINGPLANCOMMON",                            /* 1410 */
  "AEC_DISPROPSOPENINGPLANCOMMONHATCHED",                     /* 1411 */
  "AEC_DISPROPSOPENINGSILLPLAN",                              /* 1412 */
  "AEC_DISP_ROPS_RAILING_PLAN_100",                           /* 1413 */
  "AEC_DISP_ROPS_RAILING_PLAN_50",                            /* 1414 */
  "AEC_DISP_ROPS_STAIR_PLAN_100",                             /* 1415 */
  "AEC_DISP_ROPS_STAIR_PLAN_50",                              /* 1416 */
  "AEC_DOOR_STYLE",                                           /* 1417 */
  "AEC_ENDCAP_STYLE",                                         /* 1418 */
  "AEC_FRAME_DEF",                                            /* 1419 */
  "AEC_LAYERKEY_STYLE",                                       /* 1420 */
  "AEC_LIST_DEF",                                             /* 1421 */
  "AEC_MASKBLOCK_DEF",                                        /* 1422 */
  "AEC_MASS_ELEM_STYLE",                                      /* 1423 */
  "AEC_MATERIAL_DEF",                                         /* 1424 */
  "AEC_MVBLOCK_DEF",                                          /* 1425 */
  "AEC_MVBLOCK_REF",                                          /* 1426 */
  "AEC_NOTIFICATION_TRACKER",                                 /* 1427 */
  "AEC_POLYGON",                                              /* 1428 */
  "AEC_POLYGON_STYLE",                                        /* 1429 */
  "AEC_PROPERTY_SET_DEF",                                     /* 1430 */
  "AEC_RAILING_STYLE",                                        /* 1431 */
  "AEC_REFEDIT_STATUS_TRACKER",                               /* 1432 */
  "AEC_ROOFSLABEDGE_STYLE",                                   /* 1433 */
  "AEC_ROOFSLAB_STYLE",                                       /* 1434 */
  "AEC_SCHEDULE_DATA_FORMAT",                                 /* 1435 */
  "AEC_SLABEDGE_STYLE",                                       /* 1436 */
  "AEC_SLAB_STYLE",                                           /* 1437 */
  "AEC_SPACE_STYLES",                                         /* 1438 */
  "AEC_STAIR_STYLE",                                          /* 1439 */
  "AEC_STAIR_WINDER_STYLE",                                   /* 1440 */
  "AEC_STAIR_WINDER_TYPE_BALANCED",                           /* 1441 */
  "AEC_STAIR_WINDER_TYPE_MANUAL",                             /* 1442 */
  "AEC_STAIR_WINDER_TYPE_SINGLE_POINT",                       /* 1443 */
  "AEC_VARS_AECBBLDSRV",                                      /* 1444 */
  "AEC_VARS_ARCHBASE",                                        /* 1445 */
  "AEC_VARS_DWG_SETUP",                                       /* 1446 */
  "AEC_VARS_MUNICH",                                          /* 1447 */
  "AEC_VARS_STRUCTUREBASE",                                   /* 1448 */
  "AEC_WALLMOD_STYLE",                                        /* 1449 */
  "AEC_WALL_STYLE",                                           /* 1450 */
  "AEC_WINDOW_ASSEMBLY_STYLE",                                /* 1451 */
  "AEC_WINDOW_STYLE",                                         /* 1452 */
  "ALIGNMENTGRIPENTITY",                                      /* 1453 */
  "AMCONTEXTMGR",                                             /* 1454 */
  "AMDTADMENUSTATE",                                          /* 1455 */
  "AMDTAMMENUSTATE",                                          /* 1456 */
  "AMDTBROWSERDBTAB",                                         /* 1457 */
  "AMDTDMMENUSTATE",                                          /* 1458 */
  "AMDTEDGESTANDARDDIN",                                      /* 1459 */
  "AMDTEDGESTANDARDDIN13715",                                 /* 1460 */
  "AMDTEDGESTANDARDISO",                                      /* 1461 */
  "AMDTEDGESTANDARDISO13715",                                 /* 1462 */
  "AMDTFORMULAUPDATEDISPATCHER",                              /* 1463 */
  "AMDTINTERNALREACTOR",                                      /* 1464 */
  "AMDTMCOMMENUSTATE",                                        /* 1465 */
  "AMDTMENUSTATEMGR",                                         /* 1466 */
  "AMDTNOTE",                                                 /* 1467 */
  "AMDTNOTETEMPLATEDB",                                       /* 1468 */
  "AMDTSECTIONSYM",                                           /* 1469 */
  "AMDTSECTIONSYMLABEL",                                      /* 1470 */
  "AMDTSYSATTR",                                              /* 1471 */
  "AMGOBJPROPCFG",                                            /* 1472 */
  "AMGSETTINGSOBJ",                                           /* 1473 */
  "AMIMASTER",                                                /* 1474 */
  "AM_DRAWING_MGR",                                           /* 1475 */
  "AM_DWGMGR_NAME",                                           /* 1476 */
  "AM_DWG_DOCUMENT",                                          /* 1477 */
  "AM_DWG_SHEET",                                             /* 1478 */
  "AM_VIEWDIMPARMAP",                                         /* 1479 */
  "BINRECORD",                                                /* 1480 */
  "CAMSCATALOGAPPOBJECT",                                     /* 1481 */
  "CAMSSTRUCTBTNSTATE",                                       /* 1482 */
  "CATALOGSTATE",                                             /* 1483 */
  "CBROWSERAPPOBJECT",                                        /* 1484 */
  "DEPMGR",                                                   /* 1485 */
  "DMBASEELEMENT",                                            /* 1486 */
  "DMDEFAULTSTYLE",                                           /* 1487 */
  "DMLEGEND",                                                 /* 1488 */
  "DMMAP",                                                    /* 1489 */
  "DMMAPMANAGER",                                             /* 1490 */
  "DMSTYLECATEGORY",                                          /* 1491 */
  "DMSTYLELIBRARY",                                           /* 1492 */
  "DMSTYLEREFERENCE",                                         /* 1493 */
  "DMSTYLIZEDENTITIESTABLE",                                  /* 1494 */
  "DMSURROGATESTYLESETS",                                     /* 1495 */
  "DM_PLACEHOLDER",                                           /* 1496 */
  "EXACTERMXREFMAP",                                          /* 1497 */
  "EXACXREFPANELOBJECT",                                      /* 1498 */
  "EXPO_NOTIFYBLOCK",                                         /* 1499 */
  "EXPO_NOTIFYHALL",                                          /* 1500 */
  "EXPO_NOTIFYPILLAR",                                        /* 1501 */
  "EXPO_NOTIFYSTAND",                                         /* 1502 */
  "EXPO_NOTIFYSTANDNOPOLY",                                   /* 1503 */
  "GSMANAGER",                                                /* 1504 */
  "IRD_DSC_DICT",                                             /* 1505 */
  "IRD_DSC_RECORD",                                           /* 1506 */
  "IRD_OBJ_RECORD",                                           /* 1507 */
  "MAPFSMRVOBJECT",                                           /* 1508 */
  "MAPGWSUNDOOBJECT",                                         /* 1509 */
  "MAPIAMMOUDLE",                                             /* 1510 */
  "MAPMETADATAOBJECT",                                        /* 1511 */
  "MAPRESOURCEMANAGEROBJECT",                                 /* 1512 */
  "McDbContainer2",                                           /* 1513 */
  "McDbMarker",                                               /* 1514 */
  "NAMEDAPPL",                                                /* 1515 */
  "NEWSTDPARTPARLIST",                                        /* 1516 */
  "NOTEPOSITION",                                             /* 1517 */
  "OBJCLONER",                                                /* 1518 */
  "PARAMMGR",                                                 /* 1519 */
  "PARAMSCOPE",                                               /* 1520 */
  "PILLAR",                                                   /* 1521 */
  "STDPART2D",                                                /* 1522 */
  "TCH_ARROW",                                                /* 1523 */
  "TCH_AXIS_LABEL",                                           /* 1524 */
  "TCH_BLOCK_INSERT",                                         /* 1525 */
  "TCH_COLUMN",                                               /* 1526 */
  "TCH_DBCONFIG",                                             /* 1527 */
  "TCH_DIMENSION2",                                           /* 1528 */
  "TCH_DRAWINGINDEX",                                         /* 1529 */
  "TCH_HANDRAIL",                                             /* 1530 */
  "TCH_LINESTAIR",                                            /* 1531 */
  "TCH_OPENING",                                              /* 1532 */
  "TCH_RECTSTAIR",                                            /* 1533 */
  "TCH_SLAB",                                                 /* 1534 */
  "TCH_SPACE",                                                /* 1535 */
  "TCH_TEXT",                                                 /* 1536 */
  "TCH_WALL",                                                 /* 1537 */
  "TGrupoPuntos",                                             /* 1538 */
  "VAACIMAGEINVENTORY",                                       /* 1539 */
  "VAACXREFPANELOBJECT",                                      /* 1540 */
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
      // LOG_ERROR ("Unknown object type %d", type);
      return NULL;
    }
}

// expand aliases: name => CLASSES.dxfname
static const char *
dxf_encode_alias (char *restrict name)
{
  if (strEQc (name, "DICTIONARYWDFLT"))
    return "ACDBDICTIONARYWDFLT";
  else if (strEQc (name, "SECTIONVIEWSTYLE"))
    return "ACDBSECTIONVIEWSTYLE";
  else if (strEQc (name, "PLACEHOLDER"))
    return "ACDBPLACEHOLDER";
  else if (strEQc (name, "DETAILVIEWSTYLE"))
    return "ACDBDETAILVIEWSTYLE";
  else if (strEQc (name, "ASSOCPERSSUBENTMANAGER"))
    return "ACDBASSOCPERSSUBENTMANAGER";
  else if (strEQc (name, "EVALUATION_GRAPH"))
    return "ACAD_EVALUATION_GRAPH";
  else if (strEQc (name, "ASSOCACTION"))
    return "ACDBASSOCACTION";
  else if (strEQc (name, "ASSOCALIGNEDDIMACTIONBODY"))
    return "ACDBASSOCALIGNEDDIMACTIONBODY";
  else if (strEQc (name, "ASSOCOSNAPPOINTREFACTIONPARAM"))
    return "ACDBASSOCOSNAPPOINTREFACTIONPARAM";
  else if (strEQc (name, "ASSOCVERTEXACTIONPARAM"))
    return "ACDBASSOCVERTEXACTIONPARAM";
  else if (strEQc (name, "ASSOCGEOMDEPENDENCY"))
    return "ACDBASSOCGEOMDEPENDENCY";
  else if (strEQc (name, "ASSOCDEPENDENCY"))
    return "ACDBASSOCDEPENDENCY";
  else if (strEQc (name, "TABLE"))
    return "ACAD_TABLE";
  else
    return NULL;
}

Dwg_Class *
dwg_encode_get_class (Dwg_Data *dwg, Dwg_Object *obj)
{
  int i;
  Dwg_Class *klass = NULL;
  if (!dwg || !dwg->dwg_class)
    return NULL;
  // indxf has a different class order
  if (obj->dxfname) // search class by name, not offset
    {
      int invalid_klass = 0;
      for (i = 0; i < dwg->num_classes; i++)
        {
          klass = &dwg->dwg_class[i];
          if (!klass->dxfname)
            {
              invalid_klass++;
              continue;
            }
          if (strEQ (obj->dxfname, klass->dxfname))
            {
              obj->type = 500 + i;
              break;
            }
          else
            {
              // alias DICTIONARYWDFLT => ACDBDICTIONARYWDFLT
              const char *alias = dxf_encode_alias (obj->dxfname);
              if (alias && klass->dxfname && strEQ (alias, klass->dxfname))
                {
                  // a static string, which cannot be free'd. important for
                  // indxf
                  if (dwg->opts & DWG_OPTS_IN)
                    {
                      free (obj->dxfname);
                      obj->dxfname = strdup ((char *)alias);
                    }
                  else
                    obj->dxfname = (char *)alias;
                  obj->type = 500 + i;
                  break;
                }
              klass = NULL; // inefficient

              if (invalid_klass > 2 && !(dwg->opts & DWG_OPTS_IN))
                goto search_by_index;
            }
        }
    }
  else // search by index
    {
    search_by_index:
      i = obj->type - 500;
      if (i < 0 || i >= (int)dwg->num_classes)
        {
          LOG_WARN ("Invalid object type %d, only %u classes", obj->type,
                    dwg->num_classes);
          return NULL;
        }

      klass = &dwg->dwg_class[i];
      if (!klass->dxfname)
        return NULL;
      if (dwg->opts & DWG_OPTS_IN)
        {
          free (obj->dxfname);
          obj->dxfname = strdup (klass->dxfname);
        }
      else
        obj->dxfname = klass->dxfname;
    }
  return klass;
}

Dwg_Object *
find_prev_entity (Dwg_Object *obj)
{
  Dwg_Data *dwg = obj->parent;
  if (obj->supertype != DWG_SUPERTYPE_ENTITY)
    return NULL;
  if (dwg->prev_entity_index < obj->index
      && dwg->prev_entity_index < dwg->num_objects)
    {
      Dwg_Object *prev = &dwg->object[dwg->prev_entity_index];
      if (prev->supertype == DWG_SUPERTYPE_ENTITY
          && prev->tio.entity->entmode == obj->tio.entity->entmode)
        {
          if (prev->fixedtype == DWG_TYPE_SEQEND
              || prev->fixedtype == DWG_TYPE_ENDBLK)
            return NULL;
          else
            return prev;
        }
    }
  /*
  for (BITCODE_BL i = obj->index - 1; i > 0; i--)
    {
      Dwg_Object *prev = &dwg->object[i];
      if (prev->supertype == DWG_SUPERTYPE_ENTITY
          && prev->tio.entity->entmode == obj->tio.entity->entmode)
        {
          if (prev->fixedtype == DWG_TYPE_SEQEND
              || prev->fixedtype == DWG_TYPE_ENDBLK)
            return NULL;
          else
            return prev;
        }
    }
  */
  return NULL;
}

// return a relative softptr (4 handle) to the prev_ref handle, relative to
// obj.
static Dwg_Object_Ref *
dwg_link_prev (Dwg_Object_Ref *restrict prev_ref, Dwg_Object *restrict obj)
{
  Dwg_Object *prev;
  Dwg_Data *dwg = obj ? obj->parent : NULL;
  if (!prev_ref)
    return dwg_add_handleref (dwg, 4, 0, NULL);
  if (!obj)
    return NULL;
  prev = dwg_ref_object (dwg, prev_ref);
  if (!prev || prev->supertype != DWG_SUPERTYPE_ENTITY)
    return NULL;
  if (obj && obj->supertype == DWG_SUPERTYPE_ENTITY)
    obj->tio.entity->nolinks = 1;
  return dwg_add_handleref (dwg, 4, prev->handle.value, obj);
}

// return a relative softptr (4 handle) to the next_ref handle, relative to
// obj. sets obj nolinks to 1 or 0.
static Dwg_Object_Ref *
dwg_link_next (Dwg_Object_Ref *restrict next_ref, Dwg_Object *restrict obj)
{
  Dwg_Object *next;
  Dwg_Object_Ref *prev_ref;
  Dwg_Data *dwg = obj ? obj->parent : NULL;
  if (!next_ref)
    return dwg_add_handleref (dwg, 4, 0, NULL);
  if (!obj || obj->supertype != DWG_SUPERTYPE_ENTITY)
    return NULL;
  next = dwg_ref_object (dwg, next_ref);
  if (!next || next->supertype != DWG_SUPERTYPE_ENTITY)
    return NULL;
  if (next->index == obj->index + 1)
    {
      prev_ref = obj->tio.entity->prev_entity;
      // check if nolinks can be set
      if (prev_ref && prev_ref->handleref.code == 8
          && prev_ref->handleref.value == 0)
        {
          obj->tio.entity->nolinks = 1;
          LOG_TRACE ("%s.nolinks = 1\n", obj->name);
          return NULL;
        }
    }
  obj->tio.entity->nolinks = 0;
  return dwg_add_handleref (dwg, 4, next->handle.value, obj);
}

// Also exported to in_json and dwg_api, where we have no target version still.
// To set to linked list of children in POLYLINE_*/*INSERT
// similar to dwg_fixup_BLOCKS_entities()
void
in_postprocess_SEQEND (Dwg_Object *restrict obj, BITCODE_BL num_owned,
                       BITCODE_H *owned)
{
  Dwg_Data *dwg;
  // Dwg_Entity_SEQEND *o = obj->tio.entity->tio.SEQEND;
  Dwg_Object *owner;
  Dwg_Entity_POLYLINE_2D *ow;
  const char *owhdls; // the name of the H*
  const char *firstfield;
  const char *lastfield;

  if (!obj || !obj->parent || obj->fixedtype != DWG_TYPE_SEQEND
      || !obj->tio.entity)
    {
      LOG_ERROR ("wrong in_postprocess_SEQEND obj");
      return;
    }
  dwg = obj->parent;
  loglevel = dwg->opts & DWG_OPTS_LOGLEVEL;
  LOG_TRACE ("in_postprocess_SEQEND (%u):\n", (unsigned)num_owned);
  owner = dwg_ref_object (dwg, obj->tio.entity->ownerhandle);
  // r12 and earlier: search for owner backwards
  if (dwg->header.from_version < R_13b1 && !owner
      && !obj->tio.entity->ownerhandle)
    {
      for (BITCODE_BL i = obj->index - 1; i > 0; i--)
        {
          Dwg_Object *_o = &dwg->object[i];
          if (is_insert_polyline (_o->fixedtype))
            {
              owner = _o;
              obj->tio.entity->ownerhandle
                  = dwg_add_handleref (dwg, 4, _o->handle.value, obj);
              LOG_TRACE ("SEQEND.owner = " FORMAT_H " (%s) [H* 0]\n",
                         ARGS_H (_o->handle), _o->name);
              break;
            }
        }
    }
  if (!owner || !owner->tio.entity || !owner->name)
    {
      if (obj->tio.entity->ownerhandle)
        LOG_WARN ("Missing owner (" FORMAT_HV ") from " FORMAT_REF " [H 330]",
                  obj->handle.value, ARGS_REF (obj->tio.entity->ownerhandle));
      else
        LOG_WARN ("Missing owner (" FORMAT_HV ")", obj->handle.value);
      return;
    }

  obj->tio.entity->ownerhandle->obj = NULL;
  owhdls = memBEGINc (owner->name, "POLYLINE_") ? "vertex" : "attribs";
  // not the same layout for all possible owners
  ow = owner->tio.entity->tio.POLYLINE_2D;
  if (!num_owned || !owned)
    {
      dwg_dynapi_entity_value (ow, owner->name, "num_owned", &num_owned, 0);
      dwg_dynapi_entity_value (ow, owner->name, owhdls, &owned, 0);
    }
  if (!num_owned)
    return;

  if (memBEGINc (owner->name, "POLYLINE_"))
    {
      firstfield = "first_vertex";
      lastfield = "last_vertex";
    }
  else
    {
      firstfield = "first_attrib";
      lastfield = "last_attrib";
    }
  // store all these fields, or just the ones for the requested version?
  // this is also called from IN_JSON and DWG_API which has not target version
  // yet
  if ((dwg->header.from_version > R_2000 || dwg->opts & DWG_OPTS_INDXF)
      && owned)
    {
      Dwg_Object *owned_obj;
      Dwg_Object_Entity *ent;
      Dwg_Object_Ref *hdl;
      // need to turn code 3 into absolute 4.
      if (owned[0])
        {
          hdl = dwg_add_handleref (dwg, 4, owned[0]->absolute_ref, NULL);
          dwg_dynapi_entity_set_value (ow, owner->name, firstfield, &hdl, 0);
          LOG_TRACE ("%s[0].%s = " FORMAT_REF "[H 0]\n", owner->name,
                     firstfield, ARGS_REF (hdl));
        }
      if (owned[num_owned - 1])
        {
          hdl = dwg_add_handleref (dwg, 4, owned[num_owned - 1]->absolute_ref,
                                   NULL);
          dwg_dynapi_entity_set_value (ow, owner->name, lastfield, &hdl, 0);
          LOG_TRACE ("%s[%u].%s = " FORMAT_REF "[H 0]\n", owner->name,
                     num_owned - 1, lastfield, ARGS_REF (hdl));
        }
      // link the list, because the children have entmode, different to the
      // owner.
      owned_obj = dwg_ref_object (dwg, owned[0]);
      if (!owned_obj || owned_obj->supertype != DWG_SUPERTYPE_ENTITY)
        return;
      ent = owned_obj->tio.entity;
      ent->prev_entity = dwg_link_prev (NULL, owned_obj);
      if (ent->prev_entity)
        {
          LOG_TRACE ("%s[0].prev_entity = " FORMAT_REF "[H 0]\n",
                     owned_obj->name, ARGS_REF (ent->prev_entity));
        }
      else
        ent->nolinks = 0;
      ent->next_entity
          = dwg_link_next (num_owned > 1 ? owned[1] : NULL, owned_obj);
      if (ent->next_entity)
        LOG_TRACE ("%s[0].next_entity = " FORMAT_REF "[H 0]\n",
                   owned_obj->name, ARGS_REF (ent->next_entity));
      else
        ent->nolinks = 0;
      if (ent->nolinks == 1 && num_owned == 1)
        {
          ent->nolinks = 0;
          LOG_TRACE ("%s[0].nolinks = 0\n", owned_obj->name);
        }
      for (unsigned i = 1; i < num_owned; i++)
        {
          owned_obj = dwg_ref_object (dwg, owned[i]);
          if (!owned_obj || owned_obj->supertype != DWG_SUPERTYPE_ENTITY
              || !owned_obj->tio.entity)
            continue;
          ent = owned_obj->tio.entity;
          ent->prev_entity = dwg_link_prev (owned[i - 1], owned_obj);
          if (ent->prev_entity)
            {
              LOG_TRACE ("%s[%u].prev_entity = " FORMAT_REF "[H 0]\n",
                         owned_obj->name, i, ARGS_REF (ent->prev_entity));
            }
          if (i == num_owned - 1) // the last
            {
              if (ent->nolinks == 1)
                {
                  ent->nolinks = 0;
                  LOG_TRACE ("%s[%u].nolinks = 0\n", owned_obj->name, i);
                }
            }
          else
            {
              ent->next_entity = dwg_link_next (owned[i + 1], owned_obj);
              if (ent->next_entity)
                LOG_TRACE ("%s[%u].next_entity = " FORMAT_REF "[H 0]\n",
                           owned_obj->name, i, ARGS_REF (ent->next_entity));
            }
        }
    }
  else if ((dwg->header.from_version <= R_2000 || dwg->opts & DWG_OPTS_INDXF)
           && !owned)
    {
      BITCODE_H first, last, ref;
      unsigned i = 0;
      owned = (BITCODE_H *)calloc (1, sizeof (BITCODE_H));
      dwg_dynapi_entity_value (ow, owner->name, firstfield, &first, 0);
      dwg_dynapi_entity_value (ow, owner->name, lastfield, &last, 0);
      ref = first;
      if (!first || !last || !last->absolute_ref)
        {
          num_owned = 0;
          owned[0] = first;
        }
      else if (first->absolute_ref == last->absolute_ref)
        {
          num_owned = 1;
          owned[0] = first;
        }
      else
        {
          const unsigned max_steps = dwg->num_objects;
          unsigned steps = 0;
          while (ref && ref->absolute_ref
                 && ref->absolute_ref != last->absolute_ref)
            {
              Dwg_Object *ref_obj = dwg_ref_object (dwg, ref);
              if (steps++ > max_steps)
                {
                  LOG_WARN ("Cycle in subentity chain for %s\n", owner->name);
                  break;
                }
              if (!ref_obj || ref_obj->supertype != DWG_SUPERTYPE_ENTITY
                  || !ref_obj->tio.entity)
                break;
              if (i > 0)
                owned = (BITCODE_H *)realloc (owned,
                                              (i + 1) * sizeof (BITCODE_H));
              owned[i] = ref;
              if (ref)
                LOG_TRACE ("%s.%s[%u] = " FORMAT_REF "[H 0]\n", owner->name,
                           owhdls, i, ARGS_REF (ref));
              ref = ref_obj->tio.entity->next_entity;
              i++;
              num_owned = i;
            }
        }
      dwg_dynapi_entity_set_value (ow, owner->name, "num_owned", &num_owned,
                                   0);
      dwg_dynapi_entity_set_value (ow, owner->name, owhdls, &owned, 0);
    }
}

void
in_postprocess_handles (Dwg_Object *restrict obj)
{
  Dwg_Data *dwg = obj->parent;
#ifndef __AFL_COMPILER
  const char *name = obj->name;
#endif
  int is_entity = obj->supertype == DWG_SUPERTYPE_ENTITY;

  // common_entity_handle_data:
  // set is_xdic_missing and xdicobjhandle if <2004
  if (is_entity ? !obj->tio.entity->xdicobjhandle
                : !obj->tio.object->xdicobjhandle)
    {
      if (dwg->header.version >= R_2004)
        {
          if (is_entity)
            obj->tio.entity->is_xdic_missing = 1;
          else
            obj->tio.object->is_xdic_missing = 1;
        }
      else if (dwg->header.version >= R_13b1 && !is_entity)
        obj->tio.object->xdicobjhandle = dwg_add_handleref (dwg, 3, 0, obj);
      else if (dwg->header.version >= R_13b1 && is_entity)
        obj->tio.entity->xdicobjhandle = dwg_add_handleref (dwg, 3, 0, obj);
    }

  if (is_entity)
    {
      Dwg_Object_Entity *ent = obj->tio.entity;
      if (dwg->header.version >= R_13b1 && dwg->header.version <= R_14)
        {
          if (ent->ltype_flags < 3)
            ent->isbylayerlt = 1;
        }
      if (dwg->header.version >= R_13b1 && dwg->header.version <= R_2000
          && obj->type != DWG_TYPE_SEQEND && obj->type != DWG_TYPE_ENDBLK)
        {
          Dwg_Object *prev = find_prev_entity (obj);
          ent->next_entity = NULL; // temp.
          if (prev)
            {
              // if (prev->tio.entity->prev_entity)
              //  prev->tio.entity->nolinks = 0;
              if (prev->index + 1 != obj->index)
                {
                  prev->tio.entity->nolinks = 0;
                  prev->tio.entity->next_entity
                      = dwg_add_handleref (dwg, 4, obj->handle.value, prev);
                  LOG_TRACE ("prev %s(" FORMAT_HV ").next_entity = " FORMAT_REF
                             "\n",
                             prev->name, prev->handle.value,
                             ARGS_REF (prev->tio.entity->next_entity));
                  ent->nolinks = 0;
                  ent->prev_entity
                      = dwg_add_handleref (dwg, 4, prev->handle.value, obj);
                  LOG_TRACE ("%s.prev_entity = " FORMAT_REF "\n", name,
                             ARGS_REF (ent->prev_entity));
                }
              else
                {
                  LOG_TRACE ("%s.prev_entity = NULL HANDLE 4\n", name);
                  ent->prev_entity = NULL;
                  ent->nolinks = 1;
                }
            }
          else if (obj->type == DWG_TYPE_BLOCK)
            {
              ent->nolinks = 0;
              ent->prev_entity = dwg_add_handleref (dwg, 4, 0, NULL);
              ent->next_entity = dwg_add_handleref (dwg, 4, 0, NULL);
              LOG_TRACE ("%s.prev_entity = next_entity = " FORMAT_REF "\n",
                         name, ARGS_REF (ent->prev_entity));
            }
          else
            {
              LOG_TRACE ("%s.prev_entity = NULL HANDLE 4\n", name);
              ent->prev_entity = NULL;
              ent->nolinks = 1;
            }
        }
      else if (obj->type != DWG_TYPE_SEQEND && obj->type != DWG_TYPE_ENDBLK)
        ent->nolinks = 1;
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
  else if (memBEGINc (name, "ACDB_") && len > 28
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
      || type == DWG_TYPE_ACIDBLOCKREFERENCE
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
      || type == DWG_TYPE_MATERIAL || type == DWG_TYPE_LAYER_FILTER
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
