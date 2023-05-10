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
  "ACDSRECORD",                         /* 500 */
  "ACDSSCHEMA",                         /* 501 */
  "ACMECOMMANDHISTORY",                 /* 502 */
  "ACMESCOPE",                          /* 503 */
  "ACMESTATEMGR",                       /* 504 */
  "ACSH_BOOLEAN_CLASS",                 /* 505 */
  "ACSH_BOX_CLASS",                     /* 506 */
  "ACSH_BREP_CLASS",                    /* 507 */
  "ACSH_CHAMFER_CLASS",                 /* 508 */
  "ACSH_CONE_CLASS",                    /* 509 */
  "ACSH_CYLINDER_CLASS",                /* 510 */
  "ACSH_EXTRUSION_CLASS",               /* 511 */
  "ACSH_FILLET_CLASS",                  /* 512 */
  "ACSH_HISTORY_CLASS",                 /* 513 */
  "ACSH_LOFT_CLASS",                    /* 514 */
  "ACSH_PYRAMID_CLASS",                 /* 515 */
  "ACSH_REVOLVE_CLASS",                 /* 516 */
  "ACSH_SPHERE_CLASS",                  /* 517 */
  "ACSH_SWEEP_CLASS",                   /* 518 */
  "ACSH_TORUS_CLASS",                   /* 519 */
  "ACSH_WEDGE_CLASS",                   /* 520 */
  "ALDIMOBJECTCONTEXTDATA",             /* 521 */
  "ALIGNMENTPARAMETERENTITY",           /* 522 */
  "ANGDIMOBJECTCONTEXTDATA",            /* 523 */
  "ANNOTSCALEOBJECTCONTEXTDATA",        /* 524 */
  "ARCALIGNEDTEXT",                     /* 525 */
  "ARC_DIMENSION",                      /* 526 */
  "ASSOC2DCONSTRAINTGROUP",             /* 527 */
  "ASSOC3POINTANGULARDIMACTIONBODY",    /* 528 */
  "ASSOCACTION",                        /* 529 */
  "ASSOCACTIONPARAM",                   /* 530 */
  "ASSOCALIGNEDDIMACTIONBODY",          /* 531 */
  "ASSOCARRAYACTIONBODY",               /* 532 */
  "ASSOCARRAYMODIFYACTIONBODY",         /* 533 */
  "ASSOCARRAYMODIFYPARAMETERS",         /* 534 */
  "ASSOCARRAYPATHPARAMETERS",           /* 535 */
  "ASSOCARRAYPOLARPARAMETERS",          /* 536 */
  "ASSOCARRAYRECTANGULARPARAMETERS",    /* 537 */
  "ASSOCASMBODYACTIONPARAM",            /* 538 */
  "ASSOCBLENDSURFACEACTIONBODY",        /* 539 */
  "ASSOCCOMPOUNDACTIONPARAM",           /* 540 */
  "ASSOCDEPENDENCY",                    /* 541 */
  "ASSOCDIMDEPENDENCYBODY",             /* 542 */
  "ASSOCEDGEACTIONPARAM",               /* 543 */
  "ASSOCEDGECHAMFERACTIONBODY",         /* 544 */
  "ASSOCEDGEFILLETACTIONBODY",          /* 545 */
  "ASSOCEXTENDSURFACEACTIONBODY",       /* 546 */
  "ASSOCEXTRUDEDSURFACEACTIONBODY",     /* 547 */
  "ASSOCFACEACTIONPARAM",               /* 548 */
  "ASSOCFILLETSURFACEACTIONBODY",       /* 549 */
  "ASSOCGEOMDEPENDENCY",                /* 550 */
  "ASSOCLOFTEDSURFACEACTIONBODY",       /* 551 */
  "ASSOCMLEADERACTIONBODY",             /* 552 */
  "ASSOCNETWORK",                       /* 553 */
  "ASSOCNETWORKSURFACEACTIONBODY",      /* 554 */
  "ASSOCOBJECTACTIONPARAM",             /* 555 */
  "ASSOCOFFSETSURFACEACTIONBODY",       /* 556 */
  "ASSOCORDINATEDIMACTIONBODY",         /* 557 */
  "ASSOCOSNAPPOINTREFACTIONPARAM",      /* 558 */
  "ASSOCPATCHSURFACEACTIONBODY",        /* 559 */
  "ASSOCPATHACTIONPARAM",               /* 560 */
  "ASSOCPERSSUBENTMANAGER",             /* 561 */
  "ASSOCPLANESURFACEACTIONBODY",        /* 562 */
  "ASSOCPOINTREFACTIONPARAM",           /* 563 */
  "ASSOCRESTOREENTITYSTATEACTIONBODY",  /* 564 */
  "ASSOCREVOLVEDSURFACEACTIONBODY",     /* 565 */
  "ASSOCROTATEDDIMACTIONBODY",          /* 566 */
  "ASSOCSWEPTSURFACEACTIONBODY",        /* 567 */
  "ASSOCTRIMSURFACEACTIONBODY",         /* 568 */
  "ASSOCVALUEDEPENDENCY",               /* 569 */
  "ASSOCVARIABLE",                      /* 570 */
  "ASSOCVERTEXACTIONPARAM",             /* 571 */
  "BASEPOINTPARAMETERENTITY",           /* 572 */
  "BLKREFOBJECTCONTEXTDATA",            /* 573 */
  "BLOCKALIGNEDCONSTRAINTPARAMETER",    /* 574 */
  "BLOCKALIGNMENTGRIP",                 /* 575 */
  "BLOCKALIGNMENTPARAMETER",            /* 576 */
  "BLOCKANGULARCONSTRAINTPARAMETER",    /* 577 */
  "BLOCKARRAYACTION",                   /* 578 */
  "BLOCKBASEPOINTPARAMETER",            /* 579 */
  "BLOCKDIAMETRICCONSTRAINTPARAMETER",  /* 580 */
  "BLOCKFLIPACTION",                    /* 581 */
  "BLOCKFLIPGRIP",                      /* 582 */
  "BLOCKFLIPPARAMETER",                 /* 583 */
  "BLOCKGRIPLOCATIONCOMPONENT",         /* 584 */
  "BLOCKHORIZONTALCONSTRAINTPARAMETER", /* 585 */
  "BLOCKLINEARCONSTRAINTPARAMETER",     /* 586 */
  "BLOCKLINEARGRIP",                    /* 587 */
  "BLOCKLINEARPARAMETER",               /* 588 */
  "BLOCKLOOKUPACTION",                  /* 589 */
  "BLOCKLOOKUPGRIP",                    /* 590 */
  "BLOCKLOOKUPPARAMETER",               /* 591 */
  "BLOCKMOVEACTION",                    /* 592 */
  "BLOCKPARAMDEPENDENCYBODY",           /* 593 */
  "BLOCKPOINTPARAMETER",                /* 594 */
  "BLOCKPOLARGRIP",                     /* 595 */
  "BLOCKPOLARPARAMETER",                /* 596 */
  "BLOCKPOLARSTRETCHACTION",            /* 597 */
  "BLOCKPROPERTIESTABLE",               /* 598 */
  "BLOCKPROPERTIESTABLEGRIP",           /* 599 */
  "BLOCKRADIALCONSTRAINTPARAMETER",     /* 600 */
  "BLOCKREPRESENTATION",                /* 601 */
  "BLOCKROTATEACTION",                  /* 602 */
  "BLOCKROTATIONGRIP",                  /* 603 */
  "BLOCKROTATIONPARAMETER",             /* 604 */
  "BLOCKSCALEACTION",                   /* 605 */
  "BLOCKSTRETCHACTION",                 /* 606 */
  "BLOCKUSERPARAMETER",                 /* 607 */
  "BLOCKVERTICALCONSTRAINTPARAMETER",   /* 608 */
  "BLOCKVISIBILITYGRIP",                /* 609 */
  "BLOCKVISIBILITYPARAMETER",           /* 610 */
  "BLOCKXYGRIP",                        /* 611 */
  "BLOCKXYPARAMETER",                   /* 612 */
  "CAMERA",                             /* 613 */
  "CELLSTYLEMAP",                       /* 614 */
  "CONTEXTDATAMANAGER",                 /* 615 */
  "CSACDOCUMENTOPTIONS",                /* 616 */
  "CURVEPATH",                          /* 617 */
  "DATALINK",                           /* 618 */
  "DATATABLE",                          /* 619 */
  "DBCOLOR",                            /* 620 */
  "DETAILVIEWSTYLE",                    /* 621 */
  "DGNDEFINITION",                      /* 622 */
  "DGNUNDERLAY",                        /* 623 */
  "DICTIONARYVAR",                      /* 624 */
  "DICTIONARYWDFLT",                    /* 625 */
  "DIMASSOC",                           /* 626 */
  "DMDIMOBJECTCONTEXTDATA",             /* 627 */
  "DWFDEFINITION",                      /* 628 */
  "DWFUNDERLAY",                        /* 629 */
  "DYNAMICBLOCKPROXYNODE",              /* 630 */
  "DYNAMICBLOCKPURGEPREVENTER",         /* 631 */
  "EVALUATION_GRAPH",                   /* 632 */
  "EXTRUDEDSURFACE",                    /* 633 */
  "FCFOBJECTCONTEXTDATA",               /* 634 */
  "FIELD",                              /* 635 */
  "FIELDLIST",                          /* 636 */
  "FLIPPARAMETERENTITY",                /* 637 */
  "GEODATA",                            /* 638 */
  "GEOMAPIMAGE",                        /* 639 */
  "GEOPOSITIONMARKER",                  /* 640 */
  "GRADIENT_BACKGROUND",                /* 641 */
  "GROUND_PLANE_BACKGROUND",            /* 642 */
  "HELIX",                              /* 643 */
  "IBL_BACKGROUND",                     /* 644 */
  "IDBUFFER",                           /* 645 */
  "IMAGE",                              /* 646 */
  "IMAGEDEF",                           /* 647 */
  "IMAGEDEF_REACTOR",                   /* 648 */
  "IMAGE_BACKGROUND",                   /* 649 */
  "INDEX",                              /* 650 */
  "LARGE_RADIAL_DIMENSION",             /* 651 */
  "LAYERFILTER",                        /* 652 */
  "LAYER_INDEX",                        /* 653 */
  "LAYOUTPRINTCONFIG",                  /* 654 */
  "LEADEROBJECTCONTEXTDATA",            /* 655 */
  "LIGHT",                              /* 656 */
  "LIGHTLIST",                          /* 657 */
  "LINEARPARAMETERENTITY",              /* 658 */
  "LOFTEDSURFACE",                      /* 659 */
  "MATERIAL",                           /* 660 */
  "MENTALRAYRENDERSETTINGS",            /* 661 */
  "MESH",                               /* 662 */
  "MLEADEROBJECTCONTEXTDATA",           /* 663 */
  "MLEADERSTYLE",                       /* 664 */
  "MOTIONPATH",                         /* 665 */
  "MPOLYGON",                           /* 666 */
  "MTEXTATTRIBUTEOBJECTCONTEXTDATA",    /* 667 */
  "MTEXTOBJECTCONTEXTDATA",             /* 668 */
  "MULTILEADER",                        /* 669 */
  "NAVISWORKSMODEL",                    /* 670 */
  "NAVISWORKSMODELDEF",                 /* 671 */
  "NPOCOLLECTION",                      /* 672 */
  "NURBSURFACE",                        /* 673 */
  "OBJECT_PTR",                         /* 674 */
  "ORDDIMOBJECTCONTEXTDATA",            /* 675 */
  "PARTIAL_VIEWING_INDEX",              /* 676 */
  "PDFDEFINITION",                      /* 677 */
  "PDFUNDERLAY",                        /* 678 */
  "PERSUBENTMGR",                       /* 679 */
  "PLANESURFACE",                       /* 680 */
  "PLOTSETTINGS",                       /* 681 */
  "POINTCLOUD",                         /* 682 */
  "POINTCLOUDCOLORMAP",                 /* 683 */
  "POINTCLOUDDEF",                      /* 684 */
  "POINTCLOUDDEFEX",                    /* 685 */
  "POINTCLOUDDEF_REACTOR",              /* 686 */
  "POINTCLOUDDEF_REACTOR_EX",           /* 687 */
  "POINTCLOUDEX",                       /* 688 */
  "POINTPARAMETERENTITY",               /* 689 */
  "POINTPATH",                          /* 690 */
  "POLARGRIPENTITY",                    /* 691 */
  "RADIMLGOBJECTCONTEXTDATA",           /* 692 */
  "RADIMOBJECTCONTEXTDATA",             /* 693 */
  "RAPIDRTRENDERSETTINGS",              /* 694 */
  "RASTERVARIABLES",                    /* 695 */
  "RENDERENTRY",                        /* 696 */
  "RENDERENVIRONMENT",                  /* 697 */
  "RENDERGLOBAL",                       /* 698 */
  "RENDERSETTINGS",                     /* 699 */
  "REVOLVEDSURFACE",                    /* 700 */
  "ROTATIONPARAMETERENTITY",            /* 701 */
  "RTEXT",                              /* 702 */
  "SCALE",                              /* 703 */
  "SECTIONOBJECT",                      /* 704 */
  "SECTIONVIEWSTYLE",                   /* 705 */
  "SECTION_MANAGER",                    /* 706 */
  "SECTION_SETTINGS",                   /* 707 */
  "SKYLIGHT_BACKGROUND",                /* 708 */
  "SOLID_BACKGROUND",                   /* 709 */
  "SORTENTSTABLE",                      /* 710 */
  "SPATIAL_FILTER",                     /* 711 */
  "SPATIAL_INDEX",                      /* 712 */
  "SUN",                                /* 713 */
  "SUNSTUDY",                           /* 714 */
  "SWEPTSURFACE",                       /* 715 */
  "TABLE",                              /* 716 */
  "TABLECONTENT",                       /* 717 */
  "TABLEGEOMETRY",                      /* 718 */
  "TABLESTYLE",                         /* 719 */
  "TEXTOBJECTCONTEXTDATA",              /* 720 */
  "TVDEVICEPROPERTIES",                 /* 721 */
  "VISIBILITYGRIPENTITY",               /* 722 */
  "VISIBILITYPARAMETERENTITY",          /* 723 */
  "VISUALSTYLE",                        /* 724 */
  "WIPEOUT",                            /* 725 */
  "WIPEOUTVARIABLES",                   /* 726 */
  "XREFPANELOBJECT",                    /* 727 */
  "XYPARAMETERENTITY",                  /* 728 */
  "BREAKDATA",                          /* 729 */
  "BREAKPOINTREF",                      /* 730 */
  "FLIPGRIPENTITY",                     /* 731 */
  "LINEARGRIPENTITY",                   /* 732 */
  "ROTATIONGRIPENTITY",                 /* 733 */
  "XYGRIPENTITY",                       /* 734 */
  "_3DLINE",                            /* 735 */
  "REPEAT",                             /* 736 */
  "ENDREP",                             /* 737 */
  "JUMP",                               /* 738 */
  "POLYLINE",                           /* 739 */
  "VERTEX",                             /* 740 */
  "LOAD",                               /* 741 */
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
  "ACDSRECORD",                                 /* 500 */
  "ACDSSCHEMA",                                 /* 501 */
  "ACMECOMMANDHISTORY",                         /* 502 */
  "ACMESCOPE",                                  /* 503 */
  "ACMESTATEMGR",                               /* 504 */
  "ACSH_BOOLEAN_CLASS",                         /* 505 */
  "ACSH_BOX_CLASS",                             /* 506 */
  "ACSH_BREP_CLASS",                            /* 507 */
  "ACSH_CHAMFER_CLASS",                         /* 508 */
  "ACSH_CONE_CLASS",                            /* 509 */
  "ACSH_CYLINDER_CLASS",                        /* 510 */
  "ACSH_EXTRUSION_CLASS",                       /* 511 */
  "ACSH_FILLET_CLASS",                          /* 512 */
  "ACSH_HISTORY_CLASS",                         /* 513 */
  "ACSH_LOFT_CLASS",                            /* 514 */
  "ACSH_PYRAMID_CLASS",                         /* 515 */
  "ACSH_REVOLVE_CLASS",                         /* 516 */
  "ACSH_SPHERE_CLASS",                          /* 517 */
  "ACSH_SWEEP_CLASS",                           /* 518 */
  "ACSH_TORUS_CLASS",                           /* 519 */
  "ACSH_WEDGE_CLASS",                           /* 520 */
  "ACDB_ALDIMOBJECTCONTEXTDATA_CLASS",          /* 521 */
  "ALIGNMENTPARAMETERENTITY",                   /* 522 */
  "ACDB_ANGDIMOBJECTCONTEXTDATA_CLASS",         /* 523 */
  "ACDB_ANNOTSCALEOBJECTCONTEXTDATA_CLASS",     /* 524 */
  "ARCALIGNEDTEXT",                             /* 525 */
  "ARC_DIMENSION",                              /* 526 */
  "ACDBASSOC2DCONSTRAINTGROUP",                 /* 527 */
  "ACDBASSOC3POINTANGULARDIMACTIONBODY",        /* 528 */
  "ACDBASSOCACTION",                            /* 529 */
  "ACDBASSOCACTIONPARAM",                       /* 530 */
  "ACDBASSOCALIGNEDDIMACTIONBODY",              /* 531 */
  "ACDBASSOCARRAYACTIONBODY",                   /* 532 */
  "ACDBASSOCARRAYMODIFYACTIONBODY",             /* 533 */
  "ACDBASSOCARRAYMODIFYPARAMETERS",             /* 534 */
  "ACDBASSOCARRAYPATHPARAMETERS",               /* 535 */
  "ACDBASSOCARRAYPOLARPARAMETERS",              /* 536 */
  "ACDBASSOCARRAYRECTANGULARPARAMETERS",        /* 537 */
  "ACDBASSOCASMBODYACTIONPARAM",                /* 538 */
  "ACDBASSOCBLENDSURFACEACTIONBODY",            /* 539 */
  "ACDBASSOCCOMPOUNDACTIONPARAM",               /* 540 */
  "ACDBASSOCDEPENDENCY",                        /* 541 */
  "ACDBASSOCDIMDEPENDENCYBODY",                 /* 542 */
  "ACDBASSOCEDGEACTIONPARAM",                   /* 543 */
  "ACDBASSOCEDGECHAMFERACTIONBODY",             /* 544 */
  "ACDBASSOCEDGEFILLETACTIONBODY",              /* 545 */
  "ACDBASSOCEXTENDSURFACEACTIONBODY",           /* 546 */
  "ACDBASSOCEXTRUDEDSURFACEACTIONBODY",         /* 547 */
  "ACDBASSOCFACEACTIONPARAM",                   /* 548 */
  "ACDBASSOCFILLETSURFACEACTIONBODY",           /* 549 */
  "ACDBASSOCGEOMDEPENDENCY",                    /* 550 */
  "ACDBASSOCLOFTEDSURFACEACTIONBODY",           /* 551 */
  "ACDBASSOCMLEADERACTIONBODY",                 /* 552 */
  "ACDBASSOCNETWORK",                           /* 553 */
  "ACDBASSOCNETWORKSURFACEACTIONBODY",          /* 554 */
  "ACDBASSOCOBJECTACTIONPARAM",                 /* 555 */
  "ACDBASSOCOFFSETSURFACEACTIONBODY",           /* 556 */
  "ACDBASSOCORDINATEDIMACTIONBODY",             /* 557 */
  "ACDBASSOCOSNAPPOINTREFACTIONPARAM",          /* 558 */
  "ACDBASSOCPATCHSURFACEACTIONBODY",            /* 559 */
  "ACDBASSOCPATHACTIONPARAM",                   /* 560 */
  "ACDBASSOCPERSSUBENTMANAGER",                 /* 561 */
  "ACDBASSOCPLANESURFACEACTIONBODY",            /* 562 */
  "ACDBASSOCPOINTREFACTIONPARAM",               /* 563 */
  "ACDBASSOCRESTOREENTITYSTATEACTIONBODY",      /* 564 */
  "ACDBASSOCREVOLVEDSURFACEACTIONBODY",         /* 565 */
  "ACDBASSOCROTATEDDIMACTIONBODY",              /* 566 */
  "ACDBASSOCSWEPTSURFACEACTIONBODY",            /* 567 */
  "ACDBASSOCTRIMSURFACEACTIONBODY",             /* 568 */
  "ACDBASSOCVALUEDEPENDENCY",                   /* 569 */
  "ACDBASSOCVARIABLE",                          /* 570 */
  "ACDBASSOCVERTEXACTIONPARAM",                 /* 571 */
  "BASEPOINTPARAMETERENTITY",                   /* 572 */
  "ACDB_BLKREFOBJECTCONTEXTDATA_CLASS",         /* 573 */
  "BLOCKALIGNEDCONSTRAINTPARAMETER",            /* 574 */
  "BLOCKALIGNMENTGRIP",                         /* 575 */
  "BLOCKALIGNMENTPARAMETER",                    /* 576 */
  "BLOCKANGULARCONSTRAINTPARAMETER",            /* 577 */
  "BLOCKARRAYACTION",                           /* 578 */
  "BLOCKBASEPOINTPARAMETER",                    /* 579 */
  "BLOCKDIAMETRICCONSTRAINTPARAMETER",          /* 580 */
  "BLOCKFLIPACTION",                            /* 581 */
  "BLOCKFLIPGRIP",                              /* 582 */
  "BLOCKFLIPPARAMETER",                         /* 583 */
  "BLOCKGRIPLOCATIONCOMPONENT",                 /* 584 */
  "BLOCKHORIZONTALCONSTRAINTPARAMETER",         /* 585 */
  "BLOCKLINEARCONSTRAINTPARAMETER",             /* 586 */
  "BLOCKLINEARGRIP",                            /* 587 */
  "BLOCKLINEARPARAMETER",                       /* 588 */
  "BLOCKLOOKUPACTION",                          /* 589 */
  "BLOCKLOOKUPGRIP",                            /* 590 */
  "BLOCKLOOKUPPARAMETER",                       /* 591 */
  "BLOCKMOVEACTION",                            /* 592 */
  "BLOCKPARAMDEPENDENCYBODY",                   /* 593 */
  "BLOCKPOINTPARAMETER",                        /* 594 */
  "BLOCKPOLARGRIP",                             /* 595 */
  "BLOCKPOLARPARAMETER",                        /* 596 */
  "BLOCKPOLARSTRETCHACTION",                    /* 597 */
  "BLOCKPROPERTIESTABLE",                       /* 598 */
  "BLOCKPROPERTIESTABLEGRIP",                   /* 599 */
  "BLOCKRADIALCONSTRAINTPARAMETER",             /* 600 */
  "ACDB_BLOCKREPRESENTATION_DATA",              /* 601 */
  "BLOCKROTATEACTION",                          /* 602 */
  "BLOCKROTATIONGRIP",                          /* 603 */
  "BLOCKROTATIONPARAMETER",                     /* 604 */
  "BLOCKSCALEACTION",                           /* 605 */
  "BLOCKSTRETCHACTION",                         /* 606 */
  "BLOCKUSERPARAMETER",                         /* 607 */
  "BLOCKVERTICALCONSTRAINTPARAMETER",           /* 608 */
  "BLOCKVISIBILITYGRIP",                        /* 609 */
  "BLOCKVISIBILITYPARAMETER",                   /* 610 */
  "BLOCKXYGRIP",                                /* 611 */
  "BLOCKXYPARAMETER",                           /* 612 */
  "CAMERA",                                     /* 613 */
  "CELLSTYLEMAP",                               /* 614 */
  "CONTEXTDATAMANAGER",                         /* 615 */
  "CSACDOCUMENTOPTIONS",                        /* 616 */
  "ACDBCURVEPATH",                              /* 617 */
  "DATALINK",                                   /* 618 */
  "DATATABLE",                                  /* 619 */
  "DBCOLOR",                                    /* 620 */
  "ACDBDETAILVIEWSTYLE",                        /* 621 */
  "DGNDEFINITION",                              /* 622 */
  "DGNUNDERLAY",                                /* 623 */
  "DICTIONARYVAR",                              /* 624 */
  "ACDBDICTIONARYWDFLT",                        /* 625 */
  "DIMASSOC",                                   /* 626 */
  "ACDB_DMDIMOBJECTCONTEXTDATA_CLASS",          /* 627 */
  "DWFDEFINITION",                              /* 628 */
  "DWFUNDERLAY",                                /* 629 */
  "ACAD_DYNAMICBLOCKPROXYNODE",                 /* 630 */
  "ACDB_DYNAMICBLOCKPURGEPREVENTER_VERSION",    /* 631 */
  "ACAD_EVALUATION_GRAPH",                      /* 632 */
  "EXTRUDEDSURFACE",                            /* 633 */
  "ACDB_FCFOBJECTCONTEXTDATA_CLASS",            /* 634 */
  "FIELD",                                      /* 635 */
  "FIELDLIST",                                  /* 636 */
  "FLIPPARAMETERENTITY",                        /* 637 */
  "GEODATA",                                    /* 638 */
  "GEOMAPIMAGE",                                /* 639 */
  "POSITIONMARKER",                             /* 640 */
  "GRADIENT_BACKGROUND",                        /* 641 */
  "GROUND_PLANE_BACKGROUND",                    /* 642 */
  "HELIX",                                      /* 643 */
  "RAPIDRTRENDERENVIRONMENT",                   /* 644 */
  "IDBUFFER",                                   /* 645 */
  "IMAGE",                                      /* 646 */
  "IMAGEDEF",                                   /* 647 */
  "IMAGEDEF_REACTOR",                           /* 648 */
  "IMAGE_BACKGROUND",                           /* 649 */
  "INDEX",                                      /* 650 */
  "LARGE_RADIAL_DIMENSION",                     /* 651 */
  "LAYERFILTER",                                /* 652 */
  "LAYER_INDEX",                                /* 653 */
  "LAYOUTPRINTCONFIG",                          /* 654 */
  "ACDB_LEADEROBJECTCONTEXTDATA_CLASS",         /* 655 */
  "LIGHT",                                      /* 656 */
  "LIGHTLIST",                                  /* 657 */
  "LINEARPARAMETERENTITY",                      /* 658 */
  "LOFTEDSURFACE",                              /* 659 */
  "MATERIAL",                                   /* 660 */
  "MENTALRAYRENDERSETTINGS",                    /* 661 */
  "MESH",                                       /* 662 */
  "ACDB_MLEADEROBJECTCONTEXTDATA_CLASS",        /* 663 */
  "MLEADERSTYLE",                               /* 664 */
  "ACDBMOTIONPATH",                             /* 665 */
  "MPOLYGON",                                   /* 666 */
  "ACDB_MTEXTATTRIBUTEOBJECTCONTEXTDATA_CLASS", /* 667 */
  "ACDB_MTEXTOBJECTCONTEXTDATA_CLASS",          /* 668 */
  "MULTILEADER",                                /* 669 */
  "COORDINATION_MODEL",                         /* 670 */
  "ACDBNAVISWORKSMODELDEF",                     /* 671 */
  "NPOCOLLECTION",                              /* 672 */
  "NURBSURFACE",                                /* 673 */
  "OBJECT_PTR",                                 /* 674 */
  "ACDB_ORDDIMOBJECTCONTEXTDATA_CLASS",         /* 675 */
  "PARTIAL_VIEWING_INDEX",                      /* 676 */
  "PDFDEFINITION",                              /* 677 */
  "PDFUNDERLAY",                                /* 678 */
  "ACDBPERSSUBENTMANAGER",                      /* 679 */
  "PLANESURFACE",                               /* 680 */
  "PLOTSETTINGS",                               /* 681 */
  "ACDBPOINTCLOUD",                             /* 682 */
  "ACDBPOINTCLOUDCOLORMAP",                     /* 683 */
  "ACDBPOINTCLOUDDEF",                          /* 684 */
  "ACDBPOINTCLOUDDEFEX",                        /* 685 */
  "ACDBPOINTCLOUDDEF_REACTOR",                  /* 686 */
  "ACDBPOINTCLOUDDEF_REACTOR_EX",               /* 687 */
  "ACDBPOINTCLOUDEX",                           /* 688 */
  "POINTPARAMETERENTITY",                       /* 689 */
  "ACDBPOINTPATH",                              /* 690 */
  "POLARGRIPENTITY",                            /* 691 */
  "ACDB_RADIMLGOBJECTCONTEXTDATA_CLASS",        /* 692 */
  "ACDB_RADIMOBJECTCONTEXTDATA_CLASS",          /* 693 */
  "RAPIDRTRENDERSETTINGS",                      /* 694 */
  "RASTERVARIABLES",                            /* 695 */
  "RENDERENTRY",                                /* 696 */
  "RENDERENVIRONMENT",                          /* 697 */
  "RENDERGLOBAL",                               /* 698 */
  "RENDERSETTINGS",                             /* 699 */
  "REVOLVEDSURFACE",                            /* 700 */
  "ROTATIONPARAMETERENTITY",                    /* 701 */
  "RTEXT",                                      /* 702 */
  "SCALE",                                      /* 703 */
  "SECTIONOBJECT",                              /* 704 */
  "ACDBSECTIONVIEWSTYLE",                       /* 705 */
  "SECTION_MANAGER",                            /* 706 */
  "SECTION_SETTINGS",                           /* 707 */
  "SKYLIGHT_BACKGROUND",                        /* 708 */
  "SOLID_BACKGROUND",                           /* 709 */
  "SORTENTSTABLE",                              /* 710 */
  "SPATIAL_FILTER",                             /* 711 */
  "SPATIAL_INDEX",                              /* 712 */
  "SUN",                                        /* 713 */
  "SUNSTUDY",                                   /* 714 */
  "SWEPTSURFACE",                               /* 715 */
  "ACAD_TABLE",                                 /* 716 */
  "TABLECONTENT",                               /* 717 */
  "TABLEGEOMETRY",                              /* 718 */
  "TABLESTYLE",                                 /* 719 */
  "ACDB_TEXTOBJECTCONTEXTDATA_CLASS",           /* 720 */
  "TVDEVICEPROPERTIES",                         /* 721 */
  "VISIBILITYGRIPENTITY",                       /* 722 */
  "VISIBILITYPARAMETERENTITY",                  /* 723 */
  "VISUALSTYLE",                                /* 724 */
  "WIPEOUT",                                    /* 725 */
  "WIPEOUTVARIABLES",                           /* 726 */
  "EXACXREFPANELOBJECT",                        /* 727 */
  "XYPARAMETERENTITY",                          /* 728 */
  "BREAKDATA",                                  /* 729 */
  "BREAKPOINTREF",                              /* 730 */
  "FLIPGRIPENTITY",                             /* 731 */
  "LINEARGRIPENTITY",                           /* 732 */
  "ROTATIONGRIPENTITY",                         /* 733 */
  "XYGRIPENTITY",                               /* 734 */
  "3DLINE",                                     /* 735 */
  "REPEAT",                                     /* 736 */
  "ENDREP",                                     /* 737 */
  "JUMP",                                       /* 738 */
  "POLYLINE",                                   /* 739 */
  "VERTEX",                                     /* 740 */
  "LOAD",                                       /* 741 */
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
