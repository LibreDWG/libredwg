#!/usr/bin/perl -w
# Copyright (C) 2019 Free Software Foundation, Inc.
# Generate c structs/arrays for all dwg objects and its fields for a dynamic API.
# -> name, type, offset
# linear search is good enough.
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
use vars qw(@entity_names @object_names @subtypes $max_entity_names $max_object_names);
use Convert::Binary::C;
#use Data::Dumper;
BEGIN { chdir 'src' if $0 =~ /src/; }
my $c = Convert::Binary::C->new->Include(".")->Define('HAVE_CONFIG_H');
my $hdr = "../include/dwg.h";
$c->parse_file($hdr);

#print Data::Dumper->Dump([$c->struct('_dwg_entity_TEXT')], ['_dwg_entity_TEXT']);
#print Data::Dumper->Dump([$c->struct('struct _dwg_header_variables')], ['Dwg_Header_Variables']);

my (%h, $n, %structs, %ENT, %HDR);
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
while (<$in>) {
  if (!$n) {
    if (/^typedef struct (_dwg_.+) \{/) {
      $n = $1;
    } elsif (/^typedef struct (_dwg_\S+)$/) {
      $n = $1;
    }
  } elsif (/^\}/) { # close the struct
    $n = '';
  } elsif ($n and $_ =~ /^ +BITCODE_([\w\*]+)\s+(\w.*);/) {
    my $type = $1;
    $type =~ s/\s+$//;
    $h{$n}{$2} = $type;
  }
}
#$h{Dwg_Bitcode_3BD} = '3BD';
#$h{Dwg_Bitcode_2BD} = '2BD';
#$h{Dwg_Bitcode_3RD} = '3RD';
#$h{Dwg_Bitcode_2RD} = '2RD';
close $in;

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
  if ($n eq '_dwg_header_variables') {
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
    "const Dwg_DYNAPI_field $n","_fields[] = {\n";
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
    $type = $bc if $bc;
    $type =~ s/\s+$//;
    my $size = $bc ? "sizeof(BITCODE_$type)" : "sizeof($type)";
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
    } elsif ($type eq 'unsigned int') {
      $type = 'BL';
    } elsif ($type eq 'unsigned int*') {
      $type = 'BL*';
    } elsif ($type eq 'Dwg_Object_Ref*') {
      $type = 'H';
    } elsif ($type =~ /\b(unsigned|char|int|long|double)\b/) {
      warn "unexpanded $type $n.$name\n";
    } elsif ($type =~ /^struct/) {
      $size = "sizeof(void*)";
    } elsif ($type =~ /^HASH\(/) { # inlined struct or union
      warn "inlined type $type  $n.$name";
      $size = "0";
      #$type = $type->{type}; # size.width, size.height
    }
    my $need_malloc = ($type =~ /\*$/ or $type =~ /^(T|CMC$|H$)/) ? 1 : 0;
    my $sname = $name;
    if ($name =~ /\[(\d+)\]$/) {
      $need_malloc = 0;
      $size = "$1*$size";
      $sname =~ s/\[(\d+)\]$//;
    }
    $ENT{$key}->{$name} = $type;
    printf $fh "  { \"%s\", \"%s\", %s, OFF(%s,%s), %d, %d },\n",
      $name, $type, $size, $tmpl, $sname, $need_malloc, 0; #TODO: DXF
  }
  print $fh "  {NULL, NULL, 0},\n";
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
          } elsif ($k =~ /^ VERTEX_(MESH|PFACE)$/) {
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
    'unsigned short int' => '%hu',
    'short' => '%hd',
    'long' => '%l',
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
    'BLL' => '%lu',
    );

my $infile = '../test/testcases/dynapi_test.c.in';
open $in, $infile or die "$infile: $!";
$cfile  = '../test/testcases/dynapi_test.c';
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
    if (dwg_dynapi_header_value(dwg, "$name", &$var, NULL) &&
        $var == dwg->header_vars.$name)
      pass ("HEADER.$name [$stype] $fmt", $var);
    else
      {
        fail ("HEADER.$name [$stype] $fmt != $fmt", dwg->header_vars.$sname, $var); error++;
      }
EOF
        if ($type =~ /(int|long|short|char ||double|_B\b|_B[BSLD]\b|_R[CSLD])/) {
          print $fh "    $var++;\n";
        }
        print $fh <<"EOF";
    if (dwg_dynapi_header_set_value(dwg, "$name", &$var) &&
        $var == dwg->header_vars.$name)
      pass ("HEADER.$name [$stype] set+1 $fmt", $var);
    else
      {
        fail ("HEADER.$name [$stype] set+1 $fmt != $fmt", dwg->header_vars.$sname, $var); error++;
      }
  }
EOF
      } else {
        print $fh <<"EOF";
  {
    $type $var;
    if (dwg_dynapi_header_value(dwg, "$name", &$lname, NULL)
EOF
        if ($type !~ /\*\*/) {
          print $fh <<"EOF";
        && !memcmp(&$lname, &dwg->header_vars.$sname, sizeof(dwg->header_vars.$sname))
EOF
        }
        print $fh <<"EOF";
       )
      pass ("HEADER.$name [$stype]");
    else
      {
        fail ("HEADER.$name [$stype]"); error++;
      }
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
  $struct *$lname = obj->tio.$lentity->tio.$xname;
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
    if (dwg_dynapi_entity_value($lname, "$name", "$var", &$svar, NULL) &&
        $var == $lname->$svar)
      pass ("$name.$var [$stype] $fmt", $svar);
    else
      {
        fail ("$name.$var [$stype] $fmt != $fmt", $lname->$svar, $svar); error++;
      }
EOF
      if ($type =~ /(int|long|short|char|double|_B\b|_B[BSLD]\b|_R[CSLD])/) {
        print $fh "    $svar++;\n";
      }
      print $fh <<"EOF";
    if (dwg_dynapi_entity_set_value($lname, "$name", "$var", &$svar) &&
        $var == $lname->$svar)
      pass ("$name.$var [$stype] set+1 $fmt", $svar);
    else
      {
        fail ("$name.$var [$stype] set+1 $fmt != $fmt", $lname->$svar, $svar); error++;
      }
  }
EOF
    } else { # is_ptr
      print $fh <<"EOF";
  {
    $type $var;
    if (dwg_dynapi_entity_value($lname, "$name", "$var", &$svar, NULL)
EOF
        if ($stype =~ /^(TV|RC\*|unsigned char\*|char\*)$/) {
          print $fh "        && !strcmp((char*)&$svar, (char*)&$lname->$svar))\n";
        } elsif ($type !~ /\*\*/) {
          print $fh "        && !memcmp(&$svar, &$lname->$svar, sizeof($lname->$svar)))\n";
        } else {
          print $fh ")\n";
        }
        print $fh <<"EOF";
      pass ("$name.$var [$stype]");
    else
      {
        fail ("$name.$var [$stype]"); error++;
      }
  }
EOF
      }
    }
    print $fh <<"EOF";
  return error;
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
    error += test_$xname(obj);
EOF
    }
  }
}
close $in;
chmod 0444, $fh;
close $fh;

__DATA__
/* ex: set ro ft=c: -*- mode: c; buffer-read-only: t -*- */
/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2018,2019 Free Software Foundation, Inc.                   */
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

#include "dynapi.h"
#include <string.h>
#include <stdlib.h>
#define DWG_LOGLEVEL loglevel
#include "logging.h"

#ifndef _DWG_API_H_
Dwg_Object *
dwg_obj_generic_to_object(const void *restrict obj,
                          int *restrict error);
#endif

#define MAXLEN_ENTITIES @@scalar max_entity_names@@
#define MAXLEN_OBJECTS  @@scalar max_object_names@@

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

struct _name_type_fields {
  const char *const name;
  const int type;
  const Dwg_DYNAPI_field *const fields;
};

/* sorted for bsearch. from enum DWG_OBJECT_TYPE: */
const struct _name_type_fields dwg_name_types[] = {
  @@enum DWG_OBJECT_TYPE@@
};

static int
_name_inl_cmp (const void *restrict key, const void *restrict elem)
{
  //https://en.cppreference.com/w/c/algorithm/bsearch
  return strcmp((const char*)key, (const char*)elem); //inlined
}

struct _name {
  const char *const name;
};

static int
_name_struct_cmp (const void *restrict key, const void *restrict elem)
{
  //https://en.cppreference.com/w/c/algorithm/bsearch
  const struct _name *f = (struct _name *)elem;
  return strcmp((const char*)key, f->name); //deref
}

#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof(arr[0]))
#define NUM_ENTITIES    ARRAY_SIZE(dwg_entity_names)
#define NUM_OBJECTS     ARRAY_SIZE(dwg_object_names)
#define NUM_NAME_TYPES  ARRAY_SIZE(dwg_name_types)

EXPORT bool
is_dwg_entity(const char* dxfname) {
  return bsearch(dxfname, dwg_entity_names, NUM_ENTITIES, MAXLEN_ENTITIES,
                 _name_inl_cmp)
         ? true : false;
}

EXPORT bool
is_dwg_object(const char* dxfname) {
  return bsearch(dxfname, dwg_object_names, NUM_OBJECTS, MAXLEN_OBJECTS,
                 _name_inl_cmp)
         ? true : false;
}

const Dwg_DYNAPI_field*
dwg_dynapi_entity_fields(const char* dxfname)
{
  const char* p = bsearch(dxfname, dwg_name_types,
                          NUM_NAME_TYPES-1, sizeof(dwg_name_types[0]), /* NULL terminated */
                          _name_struct_cmp);
  if (p)
    {
      const int i = (p - (char*)dwg_name_types) / sizeof(dwg_name_types[0]);
      const struct _name_type_fields *f = &dwg_name_types[i];
      return f->fields;
    }
  else
    return NULL;
}

const Dwg_DYNAPI_field*
dwg_dynapi_entity_field(const char *restrict dxfname, const char *restrict field)
{
  const Dwg_DYNAPI_field* fields = dwg_dynapi_entity_fields(dxfname);
  if (fields)
    { /* linear search (unsorted) */
      Dwg_DYNAPI_field *f = (Dwg_DYNAPI_field *)fields;
      for (; f->name; f++)
        {
          if (strcmp(f->name, field) == 0)
            return f;
        }
    }
  return NULL;
}

/* generic field getters */
EXPORT bool
dwg_dynapi_entity_value(void *restrict _obj, const char *restrict dxfname,
                        const char *restrict fieldname,
                        void *restrict out, Dwg_DYNAPI_field *restrict fp)
{
  if (!_obj)
    return false;
  {
    Dwg_Object* obj;
    int error;
    obj = dwg_obj_generic_to_object(_obj, &error);
    if (obj && strcmp(obj->dxfname, dxfname)) // objid may be 0
      {
        int loglevel = obj->parent->opts & 0xf;
        LOG_ERROR("%s: Invalid entity type %s, wanted %s", __FUNCTION__, obj->dxfname, dxfname);
        return false;
      }
    {
      const Dwg_DYNAPI_field* f;
      f = dwg_dynapi_entity_field(dxfname, fieldname);
      if (!f)
        {
          int loglevel;
          if (obj) loglevel = obj->parent->opts & 0xf;
          LOG_ERROR("%s: Invalid %s field %s", __FUNCTION__, dxfname, fieldname);
          return false;
        }
      if (fp)
        memcpy(fp, f, sizeof(Dwg_DYNAPI_field));
      //if (f->need_malloc)
      //  which size? if text strcpy. if TU wcscpy. if struct num_fieldname * f->size
      memcpy(out, &((char*)_obj)[f->offset], f->size);
      return true;
    }
  }
}

EXPORT bool
dwg_dynapi_header_value(const Dwg_Data *restrict dwg, const char *restrict fieldname,
                        void *restrict out, Dwg_DYNAPI_field *restrict fp)
{
  Dwg_DYNAPI_field *f = (Dwg_DYNAPI_field *)
    bsearch(fieldname, _dwg_header_variables_fields,
            ARRAY_SIZE(_dwg_header_variables_fields)-1, /* NULL terminated */
            sizeof(_dwg_header_variables_fields[0]), _name_struct_cmp);
  if (f)
    {
      const Dwg_Header_Variables *const _obj = &dwg->header_vars;
      if (fp)
        memcpy(fp, f, sizeof(Dwg_DYNAPI_field));
      memcpy(out, &((char*)_obj)[f->offset], f->size);
      return true;
    }
  else
    {
      int loglevel = dwg->opts & 0xf;
      LOG_ERROR("%s: Invalid header field %s", __FUNCTION__, fieldname);
      return false;
    }
}

/* generic field setters */
EXPORT bool
dwg_dynapi_entity_set_value(void *restrict _obj, const char *restrict dxfname,
                            const char *restrict fieldname, const void *restrict value)
{
  if (!_obj)
    return false;
  {
    Dwg_Object* obj;
    int error;
    obj = dwg_obj_generic_to_object(_obj, &error);
    if (obj && obj->dxfname != dxfname)
      {
        int loglevel = obj->parent->opts & 0xf;
        LOG_ERROR("%s: Invalid entity type %s, wanted %s", __FUNCTION__, obj->dxfname, dxfname);
        return false;
      }
    {
      const Dwg_DYNAPI_field* f;
      f = dwg_dynapi_entity_field(dxfname, fieldname);
      if (!f)
        {
          int loglevel = obj->parent->opts & 0xf;
          LOG_ERROR("%s: Invalid %s field %s", __FUNCTION__, dxfname, fieldname);
          return false;
        }
      //if (f->need_malloc)
      //  which size? if text strcpy. if TU wcscpy. if struct num_fieldname * f->size
      memcpy(&((char*)_obj)[f->offset], value, f->size);
      return true;
    }
  }
}

EXPORT bool
dwg_dynapi_header_set_value(const Dwg_Data *restrict dwg, const char *restrict fieldname,
                            const void *restrict value)
{
  Dwg_DYNAPI_field *f = (Dwg_DYNAPI_field *)
    bsearch(fieldname, _dwg_header_variables_fields,
            ARRAY_SIZE(_dwg_header_variables_fields)-1, /* NULL terminated */
            sizeof(_dwg_header_variables_fields[0]), _name_struct_cmp);
  if (f)
    {
      const Dwg_Header_Variables *const _obj = &dwg->header_vars;
      memcpy(&((char*)_obj)[f->offset], value, f->size);
      return true;
    }
  else
    {
      int loglevel = dwg->opts & 0xf;
      LOG_ERROR("%s: Invalid header field %s", __FUNCTION__, fieldname);
      return false;
    }
}

/* Local Variables: */
/* mode: c */
/* End: */
