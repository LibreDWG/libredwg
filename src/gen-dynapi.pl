#!/usr/bin/perl -w
# generate c structs/arrays for all dwg objects and its fields for a dynamic API.
# -> name, type, offset
# linear search is good enough.
# This is needed for in_dxf, dwgfilter,
# a maintainable and shorter dwg_api and shorter language bindings.

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

my (%h, $n, %structs);
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
# todo: get BITCODE_ macro types for each struct field
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
  } elsif (/^ +BITCODE_(.+) (\w.*);/) {
    $h{$n}{$2} = $1;
  }
}
close $in;

open my $fh, ">", "dynapi.c" or die "dynapi.c: $!";

sub out_struct {
  my ($tmpl, $n) = @_;
  my $s = $c->struct($tmpl);
  #print $fh " /* ", Data::Dumper->Dump([$s], [$n]), "*/\n";
  $n = "_dwg_$n" unless $n =~ /^_dwg_/;
  print $fh "/* from typedef $tmpl: */\n",
    "const struct _name_type_offset $n","_fields[] = {\n";
  for my $d (@{$s->{declarations}}) {
    my $type = $d->{type};
    my $decl = $d->{declarators}->[0];
    my $name = $decl->{declarator};
    $name =~ s/^\*//g;
    # unexpand BITCODE_ macros: e.g. unsigned int -> BITCODE_BL
    my $s = $tmpl;
    $s =~ s/^struct //;
    my $bc = exists $h{$s} ? $h{$s}{$name} : undef;
    $type = $bc if $bc;
    # TODO: DIMENSION_COMMON, _3DSOLID_FIELDS macros
    if ($type =~ /\b(unsigned|char|int|long|double)\b/) {
      warn "unexpanded $type $n.$name\n";
    }
    if ($type =~ /^HASH\(/) { # inlined struct or union
      warn "inlined type $type  $n.$name";
      #$type = $type->{type}; # size.width, size.height
    }
    printf $fh "  { \"%s\", \"%s\", OFF(%s,%s, %d) },\n",
      $name, $type, $tmpl,$name,$decl->{offset};
  }
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
      for (sort keys %{$s->{enumerators}}) {
        my ($k,$v) = ($_, $s->{enumerators}->{$_});
        $k =~ s/^DWG_TYPE_//;
        if ($tmpl eq 'enum DWG_OBJECT_TYPE') {
          # see if the fields do exist:
          my $fields = exists $structs{$k} ? "_dwg_".$k."_fields" : "NULL";
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
close $fh;

__DATA__
/* ex: set ro ft=c: -*- mode: c; buffer-read-only: t -*- */
/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2018 Free Software Foundation, Inc.                        */
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
  int type;
  const struct _name_type_offset *const fields;
};

/* sorted for bsearch. from enum DWG_OBJECT_TYPE: */
const struct _name_type_fields dwg_name_types[] = {
  @@enum DWG_OBJECT_TYPE@@
};

static int
_name_cmp (const void *n1, const void *n2)
{
  return strcmp((const char*)n1, (const char*)n2);
}

#define NUM_ENTITIES \
  (sizeof(dwg_entity_names)/sizeof(dwg_entity_names[0]))
#define NUM_OBJECTS \
  (sizeof(dwg_object_names)/sizeof(dwg_object_names[0]))

EXPORT bool
is_dwg_entity(const char* name) {
  return bsearch(name, dwg_entity_names, NUM_ENTITIES, MAXLEN_ENTITIES, _name_cmp)
         ? true : false;
}

EXPORT bool
is_dwg_object(const char* name) {
  return bsearch(name, dwg_object_names, NUM_OBJECTS, MAXLEN_OBJECTS, _name_cmp)
         ? true : false;
}

const Dwg_DYNAPI_field*
dwg_dynapi_entity_fields(const char* name) {
  struct _name_type_fields *f = (struct _name_type_fields*)
    bsearch(name, dwg_entity_names, NUM_ENTITIES, MAXLEN_ENTITIES, _name_cmp);
  return f ? f->fields : NULL;
}

const Dwg_DYNAPI_field*
dwg_dynapi_object_fields(const char* name) {
  struct _name_type_fields *f = (struct _name_type_fields*)
    bsearch(name, dwg_object_names, NUM_OBJECTS, MAXLEN_OBJECTS, _name_cmp);
  return f ? f->fields : NULL;
}
