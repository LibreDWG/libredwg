use ExtUtils::MakeMaker;
use strict;
use ExtUtils::Embed qw(ccopts ldopts);
my ($ccflags, $ldflags);

my $ldopts = ldopts;
chomp($ldopts);

for (@ARGV) {
  if    (/^CCFLAGS=(.*)$/)  { $ccflags = ccopts . " $1"; }
  elsif (/^LDFLAGS=(.*)$/)  { $ldflags = $ldopts . " $1"; }
  else { warn "ignored arg $_. We only accept CCFLAGS and LDFLAGS"; }
}
if (!@ARGV && 
    -d '../../bindings' &&
    -d '../../src/.libs' &&
    -d '../../include') {
  $ccflags = ccopts . " -I../../include";
  $ldflags = $ldopts . " -L../../src/.libs -lredwg";
}
# wrap_perl.c is 9MB, too big
$ccflags =~ s/-O\d //;

WriteMakefile (
  NAME         => 'LibreDWG',
  VERSION      => '0.4',
  ($ccflags ? (CCFLAGS => $ccflags) : ()),
  ($ldflags ? (LDFLAGS => $ldflags) : ()),
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
