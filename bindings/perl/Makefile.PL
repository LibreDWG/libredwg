use ExtUtils::MakeMaker;
use strict;
use ExtUtils::Embed qw(ccopts ldopts);
use Config;

my $lddlflags = $Config{lddlflags};
my $ldopts = ldopts;
chomp($ldopts);
my $ccopts = ccopts;
chomp($ccopts);

# we are in bindings/perl. didn't check for out-of-dir configure
if (!@ARGV && 
    -d '../../bindings' &&
    -d '../../src/.libs' &&
    -d '../../include') {
  $ccopts .= " -I../../include";
  $ldopts .= " -L../../src/.libs";
  $lddlflags = $lddlflags . " -L../../src/.libs";
} # else link to the installed libredwg

# wrap_perl.c is 9MB, too big
$ccopts =~ s/-O\d //;
$ldopts .= " -lredwg";
$lddlflags .= " -lredwg";

WriteMakefile (
  NAME         => 'LibreDWG',
  VERSION      => '0.5',
  ($ccopts ? (CCFLAGS => $ccopts) : ()),
  ($ldopts ? (LDFLAGS => $ldopts) : ()),
  ($lddlflags ? (LDDLFLAGS => $lddlflags) : ()),
  #LIBS => ['-lredwg'],
  OPTIMIZE => '',
  ($] >= 5.005
   ? (ABSTRACT	=> 'LibreDWG perl5 bindings',
      AUTHOR  	=> 'Reini Urban <rurban@cpan.org>')
   : ()),
  ((ExtUtils::MakeMaker->VERSION() gt '6.30') ?
   (LICENSE  => 'GPL', SIGN => 1)  : ()), # GPLv3+
  ((ExtUtils::MakeMaker->VERSION() gt '6.46') ?
   (META_MERGE => {
     resources => {
       license     => 'GPLv3',
       repository  => 'https://github.com/LibreDWG/libredwg',
     }}) : ())
  );
